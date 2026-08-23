# Phoenix AI integration (MVP)

`qPhoenixAI` is an optional CloudCompare standard plugin. It deliberately has a
small, safe boundary: it submits only the user's text request to a **local**
Phoenix service and shows the returned plan. It neither uploads geometry nor
runs a CloudCompare action in this MVP.

## Build

The plugin is enabled by default when configuring CloudCompare. Disable it with:

```sh
cmake -DPLUGIN_PHOENIX_AI=OFF …
```

## Run the included local planning service

No external Python package is needed for the deterministic MVP:

```sh
python3 tools/phoenix_ai_service.py
```

It binds only to `127.0.0.1`. Open **Plugins → Phoenix AI plan preview** in
CloudCompare, enter a request, and keep the default endpoint.

## Local API contract

`POST http://127.0.0.1:8765/v1/plans`

```json
{"prompt":"Remove isolated noise", "mode":"preview"}
```

A compatible service responds with JSON such as:

```json
{
  "status":"preview",
  "summary":"Proposed non-destructive noise-removal workflow",
  "steps":[
    {"operation":"statistical_outlier_removal", "requires_confirmation":true}
  ],
  "warnings":["Review parameters before running."],
  "data_transferred":"none"
}
```

Future releases must keep execution separate from planning: each executable
step needs explicit user confirmation, an audit record, and a reversible
operation where CloudCompare supports it.
