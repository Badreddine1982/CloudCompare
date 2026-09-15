from __future__ import annotations

import pytest

from geodesic_engine import Anchor, Manifold, Metric, distortion, path_length


def test_anchor_metadata_is_immutable() -> None:
    anchor = Anchor("a", (1, 2), {"kind": "test"})
    assert anchor.coordinates == (1.0, 2.0)
    with pytest.raises(TypeError):
        anchor.metadata["kind"] = "changed"  # type: ignore[index]


def test_manifold_distance_and_nearest_anchor() -> None:
    manifold = Manifold(2)
    manifold.add_point("origin", (0, 0))
    manifold.add_point("unit", (1, 0))
    assert manifold.distance("origin", "unit") == pytest.approx(1.0)
    assert manifold.nearest_anchor((0.1, 0.0)).name == "origin"


def test_metric_dispatch() -> None:
    assert Metric.from_name("l2").distance((0, 0), (3, 4)) == pytest.approx(5.0)
    assert Metric.from_name("l1").distance((0, 0), (3, -4)) == pytest.approx(7.0)


def test_path_length_and_distortion() -> None:
    manifold = Manifold(2)
    assert path_length(manifold, [(0, 0), (3, 0), (3, 4)]) == pytest.approx(7.0)
    assert distortion(manifold, [(0, 0), (3, 0), (3, 4)]) == pytest.approx(1.4)


def test_invalid_dimensions_and_metric_vectors_fail_closed() -> None:
    with pytest.raises(ValueError):
        Manifold(0)
    with pytest.raises(ValueError):
        Metric().distance((1,), (1, 2))
    with pytest.raises(ValueError):
        Metric.from_name("unknown")
