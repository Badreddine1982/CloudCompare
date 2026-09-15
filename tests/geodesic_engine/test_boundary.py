from pathlib import Path


def test_engine_source_has_no_cloudcompare_imports() -> None:
    root = Path(__file__).parents[2] / "geodesic_engine"
    forbidden = ("cloudcompare", "qcloudcompare", "ccmain", "qcc_db")
    for path in root.rglob("*.py"):
        text = path.read_text(encoding="utf-8").lower()
        assert not any(token in text for token in forbidden), path
