# Sima AI

A deterministic, knowledge-driven bilingual AI engine written in standard C++. Sima deliberately uses **no neural networks, model weights, tensor libraries, Python runtime, or third-party runtime dependencies**.

The initial release provides a working symbolic foundation: text normalization, language-independent concept IDs, multilingual aliases, structured facts, rule-based inference, dataset ingestion, indexed retrieval, evidence explanations, and binary persistence.

> Sima is not an LLM. It does not claim unrestricted language understanding. It reasons over the facts and rules supplied to it and reports when evidence is missing.

## Features

- C++20 baseline; C++26-ready; standard library only.
- UTF-8 validation, basic Persian/Arabic character normalization, tokenization, coarse language detection.
- Shared concepts across languages through aliases (for example, `Dog` and `سگ`).
- Subject–relation–object facts with source, confidence, and origin metadata.
- Bounded, line-by-line dataset reader with per-line diagnostics.
- Explicit transitive inference rules and path explanations.
- Inverted retrieval index with deterministic result ordering.
- Versioned little-endian binary snapshots and validated loading.
- Console interface and automated tests.

## Build

Requirements: CMake 3.20+ and a C++20 compiler. The implementation intentionally uses the C++20 standard-library subset for broad compiler compatibility.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j2
ctest --test-dir build --output-on-failure
```

Direct build:

```sh
g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -Iinclude \
  src/main.cpp src/text.cpp src/storage.cpp src/dataset.cpp \
  src/reasoner.cpp src/retrieval.cpp src/engine.cpp -o sima
```

## Quick start

```sh
./build/sima
```

Example session:

```text
train datasets/examples/core.sima
stats
ask Earth IsA
query planet sun
why Dog IsA Animal
save storage/knowledge.sima
exit
```

## Dataset format

Datasets are UTF-8 text files, one record per line. Blank lines and lines beginning with `#` are ignored.

```text
FACT|Earth|IsA|Planet
FACT|Earth|Orbits|Sun|astronomy-corpus
WORD|زمین|Earth
WORD|سیاره|Planet
RULE|IsA|TRANSITIVE
```

- `FACT|subject|relation|object[|source]`
- `WORD|alias|canonical-concept`
- `RULE|relation|TRANSITIVE`

Escaping: `\\|` means a literal pipe and `\\\\` means a literal backslash. The default maximum line length is 1 MiB. Malformed lines are reported and skipped rather than aborting the complete training run.

Only a tiny illustrative sample is included. Add your own licensed corpora; dataset collection and licensing are intentionally outside this implementation task.

## Console commands

| Command | Purpose |
|---|---|
| `train <file>` | Ingest a dataset |
| `ask <subject> <relation>` | Find direct facts |
| `query <text>` | Retrieve matching evidence |
| `why <subject> <relation> <object>` | Explain a direct or inferred path |
| `list [limit]` | List stored facts |
| `stats` | Show counts and index size |
| `save [file]` | Save a binary snapshot |
| `load [file]` | Load a binary snapshot |
| `help` | Show commands |
| `exit` | Quit |

Default snapshot: `storage/knowledge.sima`.

## Architecture

- `include/sima/`: public interfaces and core data structures.
- `src/`: implementation and console application.
- `tests/`: deterministic integration tests.
- `datasets/examples/`: a tiny format demonstration, not a real corpus.
- `docs/`: architecture, format, limitations, and roadmap.

Read [Architecture](docs/ARCHITECTURE.md), [Dataset format](docs/DATA_FORMAT.md), [Limitations](docs/LIMITATIONS.md), and [Roadmap](docs/ROADMAP.md).

## Design principles

1. Evidence before eloquence.
2. Knowledge independent of its surface language.
3. Deterministic and explainable behavior.
4. Bounded input and defensive binary parsing.
5. Explicit uncertainty instead of fabricated answers.
6. Keep inferred facts separate from source facts.

## License

MIT. Anyone adding external datasets is responsible for their licensing and redistribution rights.
