# Geodesic Engine Boundary

## Status
P0 architectural boundary specification. No Geodesic implementation is moved by this document.

## Objective
Keep Geodesic research independently evolvable while CloudCompare remains a host/adapter. The boundary prevents CloudCompare UI, database, and application internals from becoming part of the Geodesic core.

## Ownership

- `Geodesic Engine`: owns mathematical state, translation, metrics, learning/research algorithms, validation contracts, and deterministic research interfaces.
- `CloudCompare Adapter`: owns conversion between CloudCompare objects and the Geodesic public contract, plugin lifecycle, UI integration, and host-specific error handling.
- `qPhoenixAI`: remains an optional planning adapter. It is not part of the Geodesic core and must not execute destructive host operations implicitly.

## Target Shape

```text
Geodesic Engine
  core/
  geometry/
  translator/
  metrics/
  learning/
  validation/
  adapters/
    cloudcompare/

CloudCompare
  -> GeodesicAdapter
  -> Geodesic public contract
```

## Extraction Rule

Extraction must be incremental and reversible:

1. Identify the current Geodesic implementation and tests.
2. Freeze the public mathematical/data contracts.
3. Introduce an adapter boundary without changing algorithm semantics.
4. Move one coherent module at a time.
5. Keep compatibility tests against the pre-extraction behavior.
6. Only remove host-coupled code after the independent implementation passes validation.

## Non-Goals

- No broad CloudCompare refactor.
- No automatic execution of Phoenix plans.
- No copying of unrelated CloudCompare application code into Geodesic.
- No claim that extraction is complete until independent validation passes.

## Validation Gates

- Geodesic tests run independently of CloudCompare UI.
- Adapter tests cover input/output conversion and error propagation.
- No Geodesic core import depends on CloudCompare application/UI classes.
- Numerical behavior is preserved for the migrated test corpus.
- CloudCompare can build with the adapter disabled when the host configuration permits it.
