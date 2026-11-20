#!/usr/bin/env python3
"""Validate, deduplicate, count, and shard Sima's structured datasets.

Uses only the Python standard library. It never fabricates records to meet a target.
"""
from __future__ import annotations
import argparse
import hashlib
import json
import sys
from pathlib import Path

KINDS = {"FACT": 5, "WORD": 3, "RULE": 3, "QA": 4}


def split_record(line: str) -> list[str]:
    fields: list[str] = [""]
    escaped = False
    for char in line:
        if escaped:
            if char in ("|", "\\"):
                fields[-1] += char
            else:
                fields[-1] += "\\" + char
            escaped = False
        elif char == "\\":
            escaped = True
        elif char == "|":
            fields.append("")
        else:
            fields[-1] += char
    if escaped:
        fields[-1] += "\\"
    return [field.strip() for field in fields]


def validate(line: str) -> tuple[bool, str]:
    try:
        line.encode("utf-8", errors="strict")
    except UnicodeError:
        return False, "invalid UTF-8"
    fields = split_record(line)
    if not fields or fields[0] not in KINDS:
        return False, "unknown record type"
    kind = fields[0]
    if kind == "FACT":
        valid = len(fields) in (4, 5) and all(fields[i] for i in (1, 2, 3))
    elif kind == "WORD":
        valid = len(fields) == 3 and bool(fields[1]) and bool(fields[2])
    elif kind == "RULE":
        valid = len(fields) == 3 and bool(fields[1]) and fields[2] == "TRANSITIVE"
    else:
        valid = len(fields) in (3, 4) and bool(fields[1]) and bool(fields[2])
    return (True, "") if valid else (False, f"malformed {kind} record")


def discover(inputs: list[Path], output: Path):
    files: list[Path] = []
    for root in inputs:
        if root.is_file():
            candidates = [root]
        elif root.exists():
            candidates = sorted(p for p in root.rglob("*") if p.is_file())
        else:
            print(f"warning: input does not exist: {root}", file=sys.stderr)
            continue
        for path in candidates:
            if output == path or output in path.parents:
                continue
            if path.suffix.lower() in {".sima", ".txt"}:
                files.append(path)
    return sorted(set(files))


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", action="append", type=Path, required=True,
                        help="input file or directory; may be repeated")
    parser.add_argument("--output", type=Path, required=True,
                        help="output directory for deterministic shards and manifest")
    parser.add_argument("--target", type=int, default=500_000,
                        help="report progress toward this target (default: 500000)")
    parser.add_argument("--shard-records", type=int, default=50_000,
                        help="maximum records per output shard")
    args = parser.parse_args()
    if args.target < 1 or args.shard_records < 1:
        parser.error("--target and --shard-records must be positive")

    output = args.output.resolve()
    files = discover(args.input, output)
    seen: set[str] = set()
    records: list[str] = []
    counts = {kind: 0 for kind in KINDS}
    invalid = duplicates = 0
    sources: list[dict[str, str]] = []

    for path in files:
        digest = hashlib.sha256()
        accepted = 0
        try:
            with path.open("r", encoding="utf-8", errors="strict") as stream:
                for number, raw in enumerate(stream, 1):
                    line = raw.rstrip("\r\n")
                    digest.update(raw.encode("utf-8"))
                    stripped = line.strip()
                    if not stripped or stripped.startswith("#"):
                        continue
                    ok, reason = validate(stripped)
                    if not ok:
                        invalid += 1
                        print(f"{path}:{number}: {reason}", file=sys.stderr)
                        continue
                    # Keep exact source record; stable hash prevents exact duplicates.
                    key = hashlib.sha256(stripped.encode("utf-8")).hexdigest()
                    if key in seen:
                        duplicates += 1
                        continue
                    seen.add(key)
                    records.append(stripped)
                    counts[split_record(stripped)[0]] += 1
                    accepted += 1
            sources.append({"path": str(path), "sha256": digest.hexdigest(),
                            "unique_records": str(accepted)})
        except (OSError, UnicodeError) as exc:
            invalid += 1
            print(f"{path}: {exc}", file=sys.stderr)

    output.mkdir(parents=True, exist_ok=True)
    for old in output.glob("shard-*.sima"):
        old.unlink()
    for index in range(0, len(records), args.shard_records):
        shard = output / f"shard-{index // args.shard_records + 1:04d}.sima"
        with shard.open("w", encoding="utf-8", newline="\n") as stream:
            stream.write("# Generated by scripts/build_dataset.py; source provenance is in manifest.json\n")
            stream.write("\n".join(records[index:index + args.shard_records]))
            stream.write("\n")

    total = len(records)
    manifest = {
        "format": "Sima structured records",
        "format_version": 1,
        "target_records": args.target,
        "unique_records": total,
        "target_reached": total >= args.target,
        "counts_by_type": counts,
        "invalid_records": invalid,
        "duplicate_records": duplicates,
        "sources": sources,
        "shard_records": args.shard_records,
    }
    (output / "manifest.json").write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(manifest, ensure_ascii=False, indent=2))
    if total < args.target:
        print(f"Target not reached: {total:,}/{args.target:,}. No synthetic padding was added.")
    return 0 if invalid == 0 else 2


if __name__ == "__main__":
    raise SystemExit(main())
