# Geodesic ↔ CloudCompare Decomposition Map

## Status

**P0 — Architecture documentation freeze.**

This document defines the decomposition and extraction blueprint for the current Geodesic research stack embedded in the CloudCompare research line. It does **not** move, delete, or rewrite implementation code.

The governing principle is:

> **Decompose → Extract → Validate → Benchmark → Optimize → Integrate**

Optimization and deep CloudCompare integration are deliberately deferred until the independent boundaries are validated.

---

## 1. Purpose

The current repository contains two different concerns:

1. **CloudCompare** — a mature point-cloud application and host environment.
2. **Geodesic research** — an independently evolvable mathematical, geometric, translation, learning, and logical-projection stack.

Keeping both concerns physically and architecturally entangled makes validation, performance measurement, replacement of research components, and future reuse harder.

The objective is therefore not to rewrite CloudCompare. The objective is to establish a clean extraction boundary so that Geodesic can become an independent engine and CloudCompare can consume it through an explicit adapter.

The existing boundary specification already establishes that Geodesic owns mathematical state, translation, metrics, learning/research algorithms, validation contracts, and deterministic research interfaces, while the CloudCompare adapter owns host conversion and UI integration. fileciteturn174file0

---

## 2. Current State

The current Geodesic implementation already forms an executable research path:

```text
Kernel
  ↓
Attention
  ↓
Translator
  ↓
Transformer
  ↓
Chess / logical projection
```

The current pipeline combines the core manifold/measurement/metric layer with translator and chess projection, and conditionally enables the transformer stage when PyTorch is available. fileciteturn172file0

The current research tree includes, at minimum:

```text
geodesic/
├── core/
│   ├── anchor.py
│   ├── manifold.py
│   ├── measurements.py
│   └── metric.py
│
├── translator/
│   ├── g_theta.py
│   └── translator.py
│
├── chess/
│   ├── board.py
│   └── projection.py
│
├── transformer/        # research/model stage
├── attention/          # attention research stage
├── pipeline.py         # end-to-end composition
└── ...                 # supporting research modules
```

This tree is treated as a **research snapshot to be decomposed**, not as a final API contract. Exact package boundaries will be frozen only after dependency inspection and independent validation.

---

## 3. GEODESIC EXTRACTION MAP

### 3.1 Core mathematical foundation

**Ownership: Geodesic Engine**

```text
core/
├── anchor.py
├── manifold.py
├── measurements.py
└── metric.py
```

Responsibilities:

- mathematical state and anchors;
- manifold representation;
- metric selection/dispatch;
- path and distortion measurements;
- deterministic numerical contracts.

This layer must remain free of CloudCompare application/UI dependencies.

### 3.2 Translation layer

**Ownership: Geodesic Engine**

```text
translator/
├── translator.py
└── g_theta.py
```

Responsibilities:

- geodesic state translation;
- bounded learnable transformation parameters;
- translation-specific numerical invariants;
- stable interfaces between geometric state and downstream research stages.

The translator is a boundary component, not a CloudCompare adapter.

### 3.3 Attention / representation layer

**Ownership: Geodesic Engine**

Responsibilities:

- point/state attention;
- representation transformation;
- attention stability and ablation experiments;
- interfaces consumed by the learning stage.

This layer may depend on numerical/ML libraries, but it must not require CloudCompare.

### 3.4 Learning / transformer layer

**Ownership: Geodesic Engine**

Responsibilities:

- Geodesic Transformer configuration and execution;
- optional PyTorch-backed research computation;
- parameter accounting;
- learning-stage ablations and benchmarks.

The ML implementation is an adapterable research component. It must not become a hidden requirement of the mathematical core.

### 3.5 Logical projection layer

**Ownership: Geodesic Engine / Research Extension**

```text
chess/
├── board.py
└── projection.py
```

Chess is classified as a **logical projection / experimental environment**, not as the Geodesic mathematical core.

The projection currently evaluates legal moves and derives channels such as control, threat, mobility, stability, geodesic error, pressure, and overall score; it can optionally combine transformer-backed quality. The end-to-end pipeline serializes these research outputs as candidate hypotheses. fileciteturn172file0

Future logical environments may replace Chess without changing the mathematical core.

### 3.6 Pipeline / composition layer

**Ownership: Geodesic Engine**

`geodesic/pipeline.py` is a composition layer. It should remain thin and orchestration-oriented.

It may connect:

```text
Core → Attention → Translator → Learning → Projection
```

It must not absorb CloudCompare-specific conversion, UI, plugin lifecycle, or database concerns.

### 3.7 Validation and research tooling

**Ownership: Geodesic Engine / Research Tooling**

The existing Geodesic test corpus is a primary extraction asset. Tests must migrate with their corresponding coherent modules rather than being recreated after extraction.

Validation must distinguish:

- mathematical correctness;
- numerical stability;
- deterministic behavior;
- ML-stage availability;
- projection legality/correctness;
- end-to-end pipeline behavior.

---

## 4. CloudCompare Decomposition Map

CloudCompare remains the host application. Its responsibilities are separated as follows.

```text
CloudCompare
│
├── Host Application              KEEP
├── Point-Cloud / DB Engine       KEEP
├── GUI / Visualization           KEEP
├── I/O                            KEEP
├── Native Algorithms              KEEP
├── Plugin System                  KEEP
│
├── Geodesic Research Code        EXTRACT
├── Geodesic Integration          ADAPT
└── Phoenix Planning Integration  ADAPTER / ISOLATED
```

### 4.1 Host application

**KEEP IN CLOUDCOMPARE**

CloudCompare owns application lifecycle, project/session state, UI, visualization, and native host behavior.

These are not Geodesic responsibilities.

### 4.2 Point-cloud database and algorithms

**KEEP IN CLOUDCOMPARE**

CloudCompare remains authoritative for its point-cloud structures, spatial data handling, native algorithms, and host-side data transformations.

Geodesic should receive a defined numerical representation rather than importing CloudCompare's internal object model into the engine.

### 4.3 GUI and visualization

**KEEP IN CLOUDCOMPARE**

The GUI is a presentation layer. Geodesic returns structured research results; CloudCompare decides how those results are displayed.

### 4.4 I/O

**KEEP IN CLOUDCOMPARE**

File formats, import/export, and host-side persistence remain CloudCompare responsibilities unless a future independent Geodesic format is explicitly designed.

### 4.5 Plugin system

**KEEP IN CLOUDCOMPARE**

Plugins are the natural host-side extension mechanism. They must consume public contracts rather than Geodesic implementation internals.

### 4.6 Geodesic research code currently residing in the host repository

**EXTRACT**

The research implementation becomes an independently testable engine. Extraction is incremental and reversible; no module is removed until its independent replacement passes validation. This is consistent with the established boundary extraction rule. fileciteturn174file0

### 4.7 Geodesic adapter

**ADAPT**

The adapter translates between:

```text
CloudCompare objects
        ↕
Geodesic public data contract
```

It owns:

- host-to-engine conversion;
- engine-to-host result conversion;
- lifecycle integration;
- host-specific errors;
- optional UI hooks.

It must not contain Geodesic algorithms.

---

## 5. Ownership Boundaries

| Concern | Owner | CloudCompare dependency | Extraction status |
|---|---|---:|---|
| Anchors / manifold | Geodesic | None | Extract |
| Metrics / measurements | Geodesic | None | Extract |
| Translator / gθ | Geodesic | None | Extract |
| Attention | Geodesic | None | Extract |
| Transformer | Geodesic | None | Extract |
| Chess projection | Geodesic research | None | Extract as optional module |
| Geodesic pipeline | Geodesic | None | Extract |
| Point-cloud native storage | CloudCompare | Native | Keep |
| Visualization | CloudCompare | Native | Keep |
| GUI | CloudCompare | Native | Keep |
| I/O | CloudCompare | Native | Keep |
| Geodesic conversion | Adapter | Host API | Adapt |
| qPhoenixAI | CloudCompare plugin | Host API | Keep isolated |

The qPhoenixAI boundary is already explicitly defined as an optional CloudCompare host adapter, outside both the Geodesic engine and Phoenix/M5 core. fileciteturn175file0

---

## 6. Dependency Graph

Target dependency direction:

```text
                    ┌─────────────────────┐
                    │   GEODESIC ENGINE   │
                    │                     │
                    │ Core / Translator   │
                    │ Attention / ML      │
                    │ Projection / Tests  │
                    └──────────┬──────────┘
                               │
                         Public Contract
                               │
                    ┌──────────▼──────────┐
                    │ GEODESIC ADAPTER    │
                    └──────────┬──────────┘
                               │
                    ┌──────────▼──────────┐
                    │ CLOUDCOMPARE HOST   │
                    └─────────────────────┘
```

The dependency direction must **not** become:

```text
Geodesic → CloudCompare internals
```

The desired direction is:

```text
CloudCompare → Adapter → Geodesic public API
```

This is the principal anti-coupling rule.

---

## 7. Public Contract

The future public contract should expose data, not host internals.

Conceptually:

```text
GeodesicInput
├── points / coordinates
├── optional weights / attributes
├── metric configuration
└── research configuration

GeodesicResult
├── status
├── measurements
├── translated state
├── candidate hypotheses
├── validation information
└── diagnostics / benchmark metadata
```

The exact schema must be finalized from the migrated implementation and tests. This document deliberately does not freeze speculative field names beyond the architectural categories above.

### Contract rules

1. No CloudCompare classes in the Geodesic public API.
2. No GUI objects in the Geodesic API.
3. No implicit file-system dependency.
4. Deterministic numerical behavior must be testable independently.
5. Optional ML dependencies must remain explicit.
6. Host conversion occurs only inside adapters.

---

## 8. Extraction Order

Extraction proceeds in dependency order, not by file size or convenience.

### Stage E0 — Freeze

- freeze current test corpus;
- record baseline behavior;
- record imports and dependencies;
- identify host-coupled code;
- freeze initial public data contracts.

### Stage E1 — Core

Extract:

```text
core/
```

Validate mathematical tests independently.

### Stage E2 — Translator

Extract:

```text
translator/
```

Validate translator invariants and gθ behavior.

### Stage E3 — Attention / learning

Extract attention and transformer components as independent research modules.

Validate both:

- with ML dependencies available;
- without optional ML dependencies, where the API supports blocked/optional operation.

The current pipeline already distinguishes the PyTorch-unavailable case from a successful transformer execution. fileciteturn172file0

### Stage E4 — Logical projection

Move Chess as a research/projection module, keeping it replaceable.

### Stage E5 — Pipeline

Rebuild the end-to-end composition against the independent engine.

### Stage E6 — Adapter

Introduce the CloudCompare adapter and restore host integration.

### Stage E7 — Host cleanup

Only after independent validation:

- remove migrated implementation from the host research tree;
- remove obsolete imports;
- remove duplicate tests;
- update build and packaging metadata;
- retain migration history and rollback references.

---

## 9. Validation Strategy

Every extraction stage requires three levels of validation.

### A. Unit validation

The migrated tests must pass without CloudCompare.

### B. Equivalence validation

For a fixed corpus:

```text
old implementation output
          ≈
new independent engine output
```

The tolerance policy must be defined per numerical operation. Exact equality is required where the implementation promises determinism; numerical tolerances are allowed only where mathematically justified.

### C. Integration validation

After the independent engine passes:

```text
CloudCompare
   ↓
Adapter
   ↓
Geodesic
   ↓
Adapter
   ↓
CloudCompare
```

must be validated without importing Geodesic internals into the host plugin.

### Required gates

- Geodesic tests independent of CloudCompare UI;
- adapter conversion tests;
- error propagation tests;
- numerical regression tests;
- host build with adapter enabled;
- host build with adapter disabled where configuration permits;
- no circular dependency between engine and host.

These gates extend the existing boundary validation rules. fileciteturn174file0

---

## 10. Integration Architecture

The target integrated tool is:

```text
                         GEODESIC ENGINE
                               │
                         Public API / ABI
                               │
                    ┌──────────▼──────────┐
                    │ GEODESIC ADAPTER    │
                    └──────────┬──────────┘
                               │
                     CloudCompare objects
                               │
                    ┌──────────▼──────────┐
                    │ CLOUDCOMPARE HOST   │
                    └─────────────────────┘
```

Later, the same engine can support multiple adapters without changing the core:

```text
                       GEODESIC ENGINE
                              │
          ┌───────────────────┼───────────────────┐
          ▼                   ▼                   ▼
   CloudCompare          Research Runtime     Other Adapter
      Adapter
```

This is the key to making the eventual integrated tool larger without inflating the mathematical core.

---

## 11. qPhoenixAI Relationship

`qPhoenixAI` remains a **host integration shell**, not a Geodesic implementation container.

Its established contract is:

```text
CloudCompare UI
      ↓
 qPhoenixAI
      ↓
 local planning service
```

The current MVP sends command text only and does not send point-cloud data; the endpoint is intentionally loopback-only under the current contract. fileciteturn175file0

Therefore:

```text
qPhoenixAI ≠ Geodesic Core
qPhoenixAI ≠ Phoenix/M5 Core
qPhoenixAI = optional host adapter
```

Future Geodesic integration must enter through the public Geodesic adapter contract rather than by embedding Geodesic implementation files in the plugin.

---

## 12. Performance Strategy

Performance work starts **after decomposition and extraction**, because optimization of a coupled research snapshot would make ownership and regressions difficult to measure.

The benchmark sequence is:

```text
Baseline
   ↓
Independent Geodesic
   ↓
Adapter overhead
   ↓
CloudCompare integration
   ↓
End-to-end workload
```

Measure separately:

- mathematical kernel latency;
- translator latency;
- attention latency;
- transformer latency;
- projection latency;
- memory consumption;
- adapter conversion cost;
- host/UI overhead;
- end-to-end throughput.

Only then should optimization decisions be made.

Potential optimization mechanisms—batching, caching, zero-copy conversion, parallel workers, model quantization, or accelerator support—remain **future implementation choices**, not architectural requirements of this document.

---

## 13. Risks and Conflict Controls

| Risk | Control |
|---|---|
| Geodesic imports CloudCompare internals | CI import-boundary test |
| CloudCompare embeds Geodesic internals | Adapter-only dependency rule |
| Numerical behavior changes during extraction | Golden/regression corpus |
| ML dependency becomes mandatory | Explicit optional dependency path |
| Chess becomes mistaken for Geodesic core | Classify it as projection/research module |
| qPhoenixAI absorbs Geodesic logic | Keep plugin as adapter only |
| Duplicate implementations remain | Post-extraction duplicate scan |
| Performance regression is hidden by host overhead | Layered benchmarks |
| Premature optimization changes semantics | Validate before optimization |
| Extraction becomes irreversible | One-module commits + compatibility tests |

---

## 14. Non-Goals

This phase does **not**:

- rewrite CloudCompare;
- redesign CloudCompare's database or GUI;
- replace its native algorithms;
- turn Chess into a mandatory Geodesic dependency;
- embed an LLM into Geodesic core;
- move Phoenix/M5 into CloudCompare;
- make qPhoenixAI an execution engine;
- optimize before establishing independent baselines;
- delete research code before migration validation;
- declare extraction complete before the independent test suite passes.

---

## 15. Repository / Git Strategy

All extraction work should remain reviewable and reversible.

Recommended commit granularity:

```text
1. documentation / contract
2. dependency isolation
3. module extraction
4. compatibility tests
5. adapter introduction
6. host rewiring
7. obsolete-code removal
8. validation / benchmark evidence
```

Each stage should be independently reviewable. No large destructive commit should combine extraction, host rewiring, optimization, and deletion.

The current P0 branch therefore freezes the architecture before the actual extraction.

---

## 16. Exit Criteria for Documentation Phase

Documentation is considered frozen when:

- Geodesic ownership is explicit;
- CloudCompare ownership is explicit;
- adapter responsibility is explicit;
- extraction order is explicit;
- validation gates are explicit;
- qPhoenixAI remains isolated;
- performance work is explicitly deferred until after extraction;
- no implementation has been moved solely because of this document.

At that point the project can proceed to **E0 / extraction preparation** without reopening the architectural question unless new repository evidence reveals a conflict.

---

## 17. Final Architectural Decision

The repository is not being converted into a single monolithic application.

Instead:

```text
                GEODESIC ENGINE
                      │
                 Public Contract
                      │
                GEODESIC ADAPTER
                      │
                CLOUDCOMPARE HOST
```

Geodesic remains the independently evolvable mathematical/research engine.

CloudCompare remains the point-cloud application and host.

The adapter is the controlled integration boundary.

`qPhoenixAI` remains an optional planning adapter and does not become part of either core.

This decomposition is the foundation for building an integrated tool **without sacrificing modularity, validation, reversibility, or future performance optimization**.
