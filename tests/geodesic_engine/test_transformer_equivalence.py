import importlib.util

import pytest

if importlib.util.find_spec("torch") is None:
    pytest.skip("PyTorch is required for transformer equivalence tests", allow_module_level=True)

import torch

from geodesic.transformer import (
    GeodesicMultiHeadAttention as LegacyMultiHeadAttention,
    GeodesicPositionalRepresentation as LegacyPositionalRepresentation,
    GeodesicTransformer as LegacyTransformer,
    GeodesicTransformerBlock as LegacyTransformerBlock,
    GeodesicTransformerConfig as LegacyConfig,
)
from geodesic_engine.transformer import (
    GeodesicMultiHeadAttention,
    GeodesicPositionalRepresentation,
    GeodesicTransformer,
    GeodesicTransformerBlock,
    GeodesicTransformerConfig,
)


def test_transformer_config_matches_legacy_defaults() -> None:
    assert GeodesicTransformerConfig() == LegacyConfig()


def test_positional_representation_matches_legacy() -> None:
    torch.manual_seed(21)
    config = GeodesicTransformerConfig(d_model=8)
    legacy = LegacyPositionalRepresentation(config.d_model)
    torch.manual_seed(21)
    extracted = GeodesicPositionalRepresentation(config.d_model)
    content = torch.randn(2, 4, 8)
    coordinates = torch.randn(2, 4, 2)
    torch.testing.assert_close(extracted(content, coordinates), legacy(content, coordinates))


@pytest.mark.parametrize("use_translator", [False, True])
def test_multi_head_attention_matches_legacy(use_translator: bool) -> None:
    config = GeodesicTransformerConfig(
        d_model=8,
        num_heads=2,
        lambda_geodesic=0.2,
        use_translator=use_translator,
        dropout=0.0,
    )
    torch.manual_seed(31)
    legacy = LegacyMultiHeadAttention(config)
    torch.manual_seed(31)
    extracted = GeodesicMultiHeadAttention(config)
    x = torch.randn(2, 5, 8)
    coordinates = torch.randn(2, 5, 2)
    expected = legacy(x, coordinates)
    actual = extracted(x, coordinates)
    torch.testing.assert_close(actual.output, expected.output)
    torch.testing.assert_close(actual.attention_weights, expected.attention_weights)
    torch.testing.assert_close(actual.scores, expected.scores)
    torch.testing.assert_close(actual.geodesic_distances, expected.geodesic_distances)


def test_transformer_block_matches_legacy() -> None:
    config = GeodesicTransformerConfig(d_model=8, num_heads=2, num_layers=1, dropout=0.0)
    torch.manual_seed(41)
    legacy = LegacyTransformerBlock(config)
    torch.manual_seed(41)
    extracted = GeodesicTransformerBlock(config)
    x = torch.randn(2, 5, 8)
    coordinates = torch.randn(2, 5, 2)
    expected_x, expected_trace = legacy(x, coordinates)
    actual_x, actual_trace = extracted(x, coordinates)
    torch.testing.assert_close(actual_x, expected_x)
    torch.testing.assert_close(actual_trace.output, expected_trace.output)
    torch.testing.assert_close(actual_trace.attention_weights, expected_trace.attention_weights)


def test_full_transformer_matches_legacy() -> None:
    config = GeodesicTransformerConfig(d_model=8, num_heads=2, num_layers=2, dropout=0.0)
    torch.manual_seed(51)
    legacy = LegacyTransformer(config)
    torch.manual_seed(51)
    extracted = GeodesicTransformer(config)
    x = torch.randn(2, 5, 8)
    coordinates = torch.randn(2, 5, 2)
    expected_logits, expected_traces = legacy(x, coordinates)
    actual_logits, actual_traces = extracted(x, coordinates)
    torch.testing.assert_close(actual_logits, expected_logits)
    assert len(actual_traces) == len(expected_traces)
    for actual, expected in zip(actual_traces, expected_traces):
        torch.testing.assert_close(actual.output, expected.output)
        torch.testing.assert_close(actual.attention_weights, expected.attention_weights)
        torch.testing.assert_close(actual.geodesic_distances, expected.geodesic_distances)


def test_transformer_rejects_non_finite_input() -> None:
    config = GeodesicTransformerConfig(d_model=8, num_heads=2)
    x = torch.zeros(1, 3, 8)
    x[0, 0, 0] = float("inf")
    with pytest.raises(ValueError, match="NaN/Inf"):
        GeodesicTransformer(config)(x)
