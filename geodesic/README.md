# Geodesic Engine

This package is the independently evolvable Geodesic research engine extracted from the CloudCompare research line.

## E1 scope

E1 establishes the mathematical core boundary:

- `geodesic.core.anchor`
- `geodesic.core.manifold`
- `geodesic.core.metric`
- `geodesic.core.measurements`

The core has no CloudCompare dependency. It operates on plain numerical vectors and immutable anchor metadata. `Metric` provides the current Euclidean and Manhattan kernels, while `Manifold` owns anchors and metric-based distance queries. fileciteturn184file0 fileciteturn185file0 fileciteturn187file0

## Extraction rule

E1 is intentionally non-destructive. The CloudCompare copy remains untouched until equivalence validation is complete.

## Planned layers

```text
core
  ↓
translator
  ↓
attention
  ↓
learning / transformer
  ↓
logical projection
  ↓
pipeline
```

CloudCompare integration belongs in a separate adapter and must not be imported by this package.
