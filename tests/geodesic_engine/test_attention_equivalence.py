import importlib.util

import pytest

if importlib.util.find_spec("torch") is None:
    pytest.skip("PyTorch is required for attention equivalence tests", allow_module_level=True)

import torch

from geodesic.attention import GeodesicAttention as LegacyGeodesicAttention
from geodesic.point_attention import PointAttentionAssistant as LegacyPointAttentionAssistant
from geodesic_engine.attention import GeodesicAttention, PointAttentionAssistant
from geodesic_engine.translator import GeodesicTranslator


def test_geodesic_attention_matches_legacy_without_translator() -> None:
    queries = torch.tensor([[0.2, 0.4, 0.1], [0.8, -0.1, 0.3]], dtype=torch.float64)
    keys = torch.tensor([[0.1, 0.3, 0.2], [0.7, 0.0, 0.4], [-0.2, 0.5, 0.1]], dtype=torch.float64)
    values = torch.tensor([[1.0, 0.0], [0.0, 1.0], [0.5, 0.5]], dtype=torch.float64)

    legacy = LegacyGeodesicAttention(lambda_geodesic=0.35, geodesic_metric="euclidean")
    extracted = GeodesicAttention(lambda_geodesic=0.35, geodesic_metric="euclidean")

    expected = legacy(queries, keys, values)
    actual = extracted(queries, keys, values)

    torch.testing.assert_close(actual.scores, expected.scores)
    torch.testing.assert_close(actual.attention, expected.attention)
    torch.testing.assert_close(actual.output, expected.output)
    torch.testing.assert_close(actual.geodesic_distances, expected.geodesic_distances)
    assert actual.translator is None


def test_geodesic_attention_matches_legacy_with_translator() -> None:
    torch.manual_seed(7)
    queries = torch.randn(2, 3, 4)
    keys = torch.randn(2, 5, 4)
    values = torch.randn(2, 5, 3)

    legacy = LegacyGeodesicAttention(
        lambda_geodesic=0.2,
        translator=GeodesicTranslator(gamma_max=0.1),
    )
    extracted = GeodesicAttention(
        lambda_geodesic=0.2,
        translator=GeodesicTranslator(gamma_max=0.1),
    )

    expected = legacy(queries, keys, values)
    actual = extracted(queries, keys, values)

    torch.testing.assert_close(actual.scores, expected.scores)
    torch.testing.assert_close(actual.attention, expected.attention)
    torch.testing.assert_close(actual.output, expected.output)
    torch.testing.assert_close(actual.geodesic_distances, expected.geodesic_distances)
    assert actual.translator is not None
    assert expected.translator is not None
    torch.testing.assert_close(
        actual.translator.correction,
        expected.translator.correction,
    )


def test_point_attention_matches_legacy() -> None:
    torch.manual_seed(11)
    local_states = torch.randn(2, 3, 4)
    neighborhoods = torch.randn(2, 3, 5, 4)

    expected = LegacyPointAttentionAssistant()(local_states, neighborhoods)
    actual = PointAttentionAssistant()(local_states, neighborhoods)

    torch.testing.assert_close(actual.scores, expected.scores)
    torch.testing.assert_close(actual.attention_weights, expected.attention_weights)
    torch.testing.assert_close(actual.points, expected.points)


def test_attention_rejects_non_finite_inputs() -> None:
    queries = torch.zeros(1, 2, 3)
    keys = torch.zeros(1, 2, 3)
    values = torch.zeros(1, 2, 3)
    queries[0, 0, 0] = float("nan")
    with pytest.raises(ValueError, match="NaN/Inf"):
        GeodesicAttention()(queries, keys, values)


def test_point_attention_validates_shapes() -> None:
    local_states = torch.zeros(1, 2, 3)
    neighborhoods = torch.zeros(1, 2, 4)
    with pytest.raises(ValueError, match="shape"):
        PointAttentionAssistant()(local_states, neighborhoods)
