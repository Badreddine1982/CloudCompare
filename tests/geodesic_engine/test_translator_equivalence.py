"""E2 differential tests for Translator + g_theta."""

from __future__ import annotations

import importlib.util

import pytest

if importlib.util.find_spec("torch") is None:
    pytestmark = pytest.mark.skip(reason="E2 translator equivalence requires PyTorch")
else:
    import torch
    from geodesic.translator.translator import GeodesicTranslator as LegacyGeodesicTranslator
    from geodesic.translator.translator import Translator as LegacyTranslator
    from geodesic.translator.translator import TranslatorState as LegacyTranslatorState
    from geodesic_engine.translator.translator import GeodesicTranslator as EngineGeodesicTranslator
    from geodesic_engine.translator.translator import Translator as EngineTranslator
    from geodesic_engine.translator.translator import TranslatorState as EngineTranslatorState


def test_record_translation_equivalence() -> None:
    records = [
        {"id": "a", "coordinates": (0.0, 0.0), "kind": "reference"},
        {"name": "b", "coordinates": (3.0, 4.0), "kind": "sample"},
    ]
    legacy = LegacyTranslator().manifold_from_records(records)
    engine = EngineTranslator().manifold_from_records(records)
    assert engine.dimensions == legacy.dimensions
    assert list(engine.anchors) == list(legacy.anchors)
    for name in legacy.anchors:
        assert engine.anchors[name].coordinates == legacy.anchors[name].coordinates
        assert dict(engine.anchors[name].metadata) == dict(legacy.anchors[name].metadata)
    assert EngineTranslator().encode_text("AI 2026") == LegacyTranslator().encode_text("AI 2026")


def test_translator_measurements_and_correction_equivalence() -> None:
    torch.manual_seed(7)
    queries = torch.tensor([[[0.1, 0.2], [0.4, -0.1]]], dtype=torch.float64)
    keys = torch.tensor([[[0.0, 0.3], [0.5, -0.2]]], dtype=torch.float64)
    distances = torch.tensor([[0.2, 0.7]], dtype=torch.float64)
    previous = queries - 0.05
    legacy = LegacyGeodesicTranslator(gamma_max=0.1)
    engine = EngineGeodesicTranslator(gamma_max=0.1)
    old = legacy(queries, keys, distances, previous_queries=previous)
    new = engine(queries, keys, distances, previous_queries=previous)
    assert EngineTranslatorState(new.translator_state.value) == LegacyTranslatorState(old.translator_state.value)
    assert new.measurements == old.measurements
    assert torch.allclose(new.correction, old.correction, rtol=0.0, atol=1e-12)


def test_translator_rejects_nonfinite_inputs_in_both_boundaries() -> None:
    queries = torch.tensor([[[float("nan")]]])
    keys = torch.tensor([[[0.0]]])
    distances = torch.tensor([[0.1]])
    for cls in (LegacyGeodesicTranslator, EngineGeodesicTranslator):
        with pytest.raises(ValueError):
            cls()(queries, keys, distances)


def test_correction_bound_is_preserved() -> None:
    translator = EngineGeodesicTranslator(gamma_max=0.1)
    measurements = translator.measure(
        torch.tensor([[[0.0]]]),
        torch.tensor([[[0.0]]]),
        torch.tensor([[0.0]]),
    )
    correction = translator.correction((1, 1), measurements, raw=torch.tensor(1_000.0))
    assert torch.all(correction.abs() <= 0.1)
