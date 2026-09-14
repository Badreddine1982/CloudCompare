# qPhoenixAI Adapter Boundary

`qPhoenixAI` is an optional CloudCompare host adapter. It is intentionally outside the Geodesic engine and outside the Phoenix/M5 core.

## Contract

```text
CloudCompare UI
      |
      v
 qPhoenixAI
      |
      | local loopback plan request
      v
 Phoenix planning service
```

The plugin must remain preview/planning-only unless a future execution contract is separately designed, explicitly confirmed, audited, and made reversible where possible.

The current MVP sends command text only and does not send point-cloud data. The endpoint must remain loopback-only unless a future security review explicitly changes that contract.

## Dependency Rule

`qPhoenixAI` may depend on CloudCompare host APIs and its local HTTP client stack. It must not import Geodesic implementation internals directly. Future Geodesic integration must go through the public adapter contract defined in `docs/architecture/GEODESIC_BOUNDARY.md`.

## Extraction Rule

Do not move Geodesic implementation code into this plugin. The plugin remains the integration shell while the Geodesic engine is extracted and independently validated.
