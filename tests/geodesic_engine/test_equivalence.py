"""Equivalence tests between the legacy Geodesic core and E1 extraction."""

from __future__ import annotations

import math

import pytest

from geodesic.core.anchor import Anchor as LegacyAnchor
from geodesic.core.manifold import Manifold as LegacyManifold
from geodesic.core.metric import Metric as LegacyMetric
from geodesic.core.measurements import distortion as legacy_distortion
from geodesic.core.measurements import path_length as legacy_path_length
from geodesic_engine.core.anchor import Anchor as EngineAnchor
from geodesic_engine.core.manifold import Manifold as EngineManifold
from geodesic_engine.core.metric import Metric as EngineMetric
from geodesic_engine.core.measurements import distortion as engine_distortion
from geodesic_engine.core.measurements import path_length as engine_path_length


@pytest.mark.parametrize(
    "name,coordinates,metadata",
    [
        ("origin", (0, 0), {}),
        ("a", (1.5, -2.0, 3.25), {"kind": "test", "index": 2}),
        ("unicode", (1, 2, 3, 4), {"label": "α"}),
    ],
)
def test_anchor_equivalence(name, coordinates, metadata):
    legacy = LegacyAnchor(name, coordinates, metadata)
    engine = EngineAnchor(name, coordinates, metadata)
    assert legacy.name == engine.name
    assert legacy.coordinates == engine.coordinates
    assert dict(legacy.metadata) == dict(engine.metadata)


@pytest.mark.parametrize("metric_name", ["euclidean", "l2", "manhattan", "l1"])
def test_metric_equivalence(metric_name):
    legacy = LegacyMetric.from_name(metric_name)
    engine = EngineMetric.from_name(metric_name)
    vectors = [((0, 0, 0), (3, 4, 0)), ((-2.5, 1), (4, -3)), ((1, 2), (1, 2))]
    for left, right in vectors:
        assert math.isclose(
            legacy.distance(left, right), engine.distance(left, right), rel_tol=0, abs_tol=1e-12
        )


@pytest.mark.parametrize("metric_name", ["euclidean", "manhattan"])
def test_manifold_and_measurement_equivalence(metric_name):
    legacy = LegacyManifold(dimensions=3, metric=LegacyMetric.from_name(metric_name))
    engine = EngineManifold(dimensions=3, metric=EngineMetric.from_name(metric_name))
    for name, point in [("a", (0, 0, 0)), ("b", (3, 4, 0)), ("c", (3, 4, 12))]:
        legacy.add_point(name, point)
        engine.add_point(name, point)

    path = ["a", "b", "c"]
    for left, right in [("a", "c"), ("a", "b"), ((1, 1, 1), "c")]:
        assert math.isclose(legacy.distance(left, right), engine.distance(left, right), rel_tol=0, abs_tol=1e-12)
    assert legacy.nearest_anchor((2.9, 4.1, 0.1)).name == engine.nearest_anchor((2.9, 4.1, 0.1)).name
    assert math.isclose(legacy_path_length(legacy, path), engine_path_length(engine, path), rel_tol=0, abs_tol=1e-12)
    assert math.isclose(legacy_distortion(legacy, path), engine_distortion(engine, path), rel_tol=0, abs_tol=1e-12)


@pytest.mark.parametrize(
    "factory,arguments",
    [
        (LegacyAnchor, ("", (1,))),
        (EngineAnchor, ("", (1,))),
    ],
)
def test_invalid_anchor_equivalence(factory, arguments):
    with pytest.raises(ValueError):
        factory(*arguments)


@pytest.mark.parametrize("metric_factory", [LegacyMetric, EngineMetric])
def test_invalid_metric_inputs_remain_fail_closed(metric_factory):
    metric = metric_factory()
    with pytest.raises(ValueError):
        metric.distance((1, 2), (1,))
    with pytest.raises(ValueError):
        metric.distance((), ())
