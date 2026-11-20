# Testing

The test executable is dependency-free and is registered with CTest.

Run:

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Current coverage includes:

- ASCII and Persian character normalization.
- UTF-8 validation and rejection of malformed dataset lines.
- Coarse language detection and Persian punctuation tokenization.
- Concept aliases shared across scripts and alias-aware retrieval.
- Fact ingestion, duplicate rejection, and alias-collision handling.
- English and Persian text-template extraction.
- Structured Q/A ingestion, normalized duplicate detection, punctuation-normalized lookup, and response persistence.
- Explicit transitive inference and path explanation.
- The rule engine's refusal to assume transitivity without a rule.
- Indexed retrieval and conservative unsupported-query behavior.
- Bounded session memory and lexical recall.
- Escaped delimiters in dataset records.
- Directory training that mixes structured and plain-text files.
- Binary version 2 round-trip, replacement of an existing snapshot, version 1 compatibility, and failed-load state preservation.

The tests are not a substitute for fuzzing, large-corpus benchmarks, cross-platform CI, or adversarial file-format testing. Those remain release-hardening work.
