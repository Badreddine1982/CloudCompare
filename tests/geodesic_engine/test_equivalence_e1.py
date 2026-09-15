"""E1 differential tests: extracted core must preserve legacy behavior."""

from __future__ import annotations

import math

import pytest

from geodesic.core.anchor import Anchor as LegacyAnchor
from geodesic.core.manifold import Manifold as LegacyManifold
from geodesic.core.measurements import distortion as legacy_distortion
from geodesic.core.measurements import path_length as legacy_path_length
from geodesic.core.metric import Metric as LegacyMetric
from geodesic_engine.core.anchor import Anchor as EngineAnchor
from geodesic_engine.core.manifold import Manifold as EngineManifold
from geodesic_engine.core.measurements import distortion as engine_distortion
from geodesic_engine.core.measurements import path_length as engine_path_length
from geodesic_engine.core.metric import Metric as EngineMetric


CASES = [
    ((0.0, 0.0), (3.0, 4.0)),
    ((-2.5, 1.25, 9.0), (4.5, -3.75, 0.5)),
    ((1.0, -1.0, 2.0, -2.0), (-1.0, 3.0, 0.0, 4.0)),
]


@pytest.mark.parametrize("left,right", CASES)
def test_metric_differential(left: tuple[float, ...], right: tuple[float, ...]) -> None:
    for name in ("euclidean", "l2", "manhattan", "l1"):
        legacy = LegacyMetric.from_name(name).distance(left, right)
        engine = EngineMetric.from_name(name).distance(left, right)
        assert math.isclose(engine, legacy, rel_tol=0.0, abs_tol=1e-12)
        assert EngineMetric.from_name(name).name == LegacyMetric.from_name(name).name


@pytest.mark.parametrize("left,right", CASES)
def test_named_p_norm_differential(left: tuple[float, ...], right: tuple[float, ...]) -> None:
    # Keep the corpus argument explicit so failures identify the affected case.
    del left, right
    from geodesic.core.metric import named_p_norm as legacy_named_p_norm
    from geodesic_engine.core.metric import named_p_norm as engine_named_p_norm

    for name in ("euclidean", "L2", "manhattan", "L1"):
        assert engine_named_p_norm(name) == legacy_named_p_norm(name)


def test_anchor_and_manifold_differential() -> None:
    records = [
        ("origin", (0.0, 0.0, 0.0), {"kind": "reference"}),
        ("a", (3.0, 4.0, 0.0), {"kind": "sample"}),
        ("b", (-1.0, 2.0, 5.0), {"kind": "sample"}),
    ]

    legacy = LegacyManifold(3)
    engine = EngineManifold(3)
    for name, coords, metadata in records:
        legacy.add_point(name, coords, **metadata)
        engine.add_point(name, coords, **metadata)

    assert list(engine.anchors) == list(legacy.anchors)
    for name in legacy.anchors:
        old = legacy.anchors[name]
        new = engine.anchors[name]
        assert new.name == old.name
        assert new.coordinates == old.coordinates
        assert dict(new.metadata) == dict(old.metadata)

    queries = [(1.0, 1.0, 1.0), (2.5, 3.5, 0.0), (-0.5, 1.5, 4.0)]
    for query in queries:
        assert engine.nearest_anchor(query).name == legacy.nearest_anchor(query).name
    for left, right in (("origin", "a"), ("a", "b"), ((0.0, 0.0, 0.0), "b")):
        assert engine.distance(left, right) == legacy.distance(left, right)


def test_measurements_differential() -> None:
    points = [(0.0, 0.0), (3.0, 4.0), (6.0, 4.0), (6.0, 8.0)]
    assert engine_path_length(points) == legacy_path_length(points)
    assert engine_distortion(points, 10.0) == legacy_distortion(points, 10.0)


@pytest.mark.parametrize(
    "factory,args",
    [
        (LegacyMetric.from_name, ("unsupported",)),
        (EngineMetric.from_name, ("unsupported",)),
    ],
)
def test_invalid_metric_fails_closed(factory, args) -> None:
    with pytest.raises(ValueError):
        factory(*args)


def test_dimension_and_empty_manifold_behavior_match() -> None:
    for cls in (LegacyManifold, EngineManifold):
        with pytest.raises(ValueError):
            cls(0)
        manifold = cls(2)
        with pytest.raises(ValueError):
            manifold.nearest_anchor((0.0, 0.0))


def test_anchor_is_immutable_in_both_boundaries() -> None:
    for cls in (LegacyAnchor, EngineAnchor):
        anchor = cls("a", (1.0, 2.0), {"kind": "test"})
        with pytest.raises((AttributeError, TypeError)):
            anchor.name = "changed"
