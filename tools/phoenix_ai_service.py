#!/usr/bin/env python3
"""Minimal local-only Phoenix planning service for qPhoenixAI.

This is intentionally a deterministic MVP.  It returns a reviewable plan and
never receives or changes CloudCompare geometry.  Replace plan_for() with a
model-backed planner only after adding authentication, auditing, and approval.
"""

from __future__ import annotations

import json
from http import HTTPStatus
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer


HOST = "127.0.0.1"
PORT = 8765


def plan_for(prompt: str) -> dict:
    text = prompt.lower()
    steps = []
    if any(word in text for word in ("noise", "ضوضاء", "isolated", "outlier")):
        steps.append({
            "operation": "statistical_outlier_removal",
            "description": "Identify isolated points using user-reviewed neighbourhood parameters.",
            "requires_confirmation": True,
        })
    if any(word in text for word in ("volume", "حجم", "pile", "كومة")):
        steps.append({
            "operation": "volume_measurement",
            "description": "Select a ground reference and calculate a volume report.",
            "requires_confirmation": True,
        })
    if any(word in text for word in ("segment", "تصنيف", "classif", "building", "مبنى")):
        steps.append({
            "operation": "segmentation",
            "description": "Preview segmentation settings before creating a derived entity.",
            "requires_confirmation": True,
        })
    if not steps:
        steps.append({
            "operation": "clarify_request",
            "description": "No executable operation is inferred. Ask the user to specify a supported workflow.",
            "requires_confirmation": True,
        })
    return {
        "status": "preview",
        "summary": "Non-destructive Phoenix plan; no CloudCompare action has run.",
        "steps": steps,
        "warnings": ["Review every parameter and approve each future executable step."],
        "data_transferred": "none; only the command text was processed locally",
    }


class Handler(BaseHTTPRequestHandler):
    def do_POST(self) -> None:  # noqa: N802 - required stdlib method name
        if self.path != "/v1/plans":
            self.send_error(HTTPStatus.NOT_FOUND, "Unknown endpoint")
            return
        try:
            size = int(self.headers.get("Content-Length", "0"))
            payload = json.loads(self.rfile.read(size))
            prompt = payload["prompt"].strip()
            if payload.get("mode") != "preview" or not prompt:
                raise ValueError("'prompt' and preview mode are required")
        except (json.JSONDecodeError, KeyError, ValueError) as error:
            self.send_error(HTTPStatus.BAD_REQUEST, str(error))
            return
        body = json.dumps(plan_for(prompt), ensure_ascii=False).encode("utf-8")
        self.send_response(HTTPStatus.OK)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def log_message(self, format: str, *args: object) -> None:
        print("Phoenix service: " + format % args)


if __name__ == "__main__":
    print(f"Phoenix planning service listening at http://{HOST}:{PORT}/v1/plans")
    ThreadingHTTPServer((HOST, PORT), Handler).serve_forever()
