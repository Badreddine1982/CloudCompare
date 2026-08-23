import pytest

from geodesic.core.metric import named_p_norm


def test_named_p_norm_maps_kernel_aliases():
    assert named_p_norm("euclidean") == 2
    assert named_p_norm("manhattan") == 1
    with pytest.raises(ValueError):
        named_p_norm("chebyshev")


def test_attention_and_transformer_dispatch_metrics_when_torch_available():
    torch = pytest.importorskip("torch")
    from geodesic.attention import GeodesicAttention
    from geodesic.transformer import GeodesicMultiHeadAttention, GeodesicTransformerConfig

    queries = torch.tensor([[[0.0, 0.0]]])
    keys = torch.tensor([[[3.0, 4.0]]])
    euclidean = GeodesicAttention(geodesic_metric="euclidean").geodesic_distances(queries, keys)
    manhattan = GeodesicAttention(geodesic_metric="manhattan").geodesic_distances(queries, keys)
    assert torch.isclose(euclidean.squeeze(), torch.tensor(5.0))
    assert torch.isclose(manhattan.squeeze(), torch.tensor(7.0))

    # Zero the learned projections so the combined field is exactly half of the
    # coordinate metric (the other half is the zero-valued projected-token field).
    coordinates = torch.tensor([[[0.0, 0.0], [3.0, 4.0], [1.0, 1.0]]])
    model = GeodesicMultiHeadAttention(GeodesicTransformerConfig(d_model=8, num_heads=2, geodesic_metric="manhattan"))
    for projection in (model.q_proj, model.k_proj):
        projection.weight.data.zero_()
        projection.bias.data.zero_()
    output = model(torch.randn(1, 3, 8), coordinates)
    assert output.geodesic_distances.shape == (1, 2, 3, 3)
    assert torch.isfinite(output.geodesic_distances).all()
    # Manhattan distance (0, 0) → (3, 4) is 7, not Euclidean distance 5.
    assert torch.allclose(output.geodesic_distances[0, :, 0, 1], torch.full((2,), 3.5))
