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
- Coarse language detection.
- Concept aliases shared across scripts.
- Fact ingestion, duplicate rejection, and alias-collision handling.
- Explicit transitive inference and path explanation.
- The rule engine's refusal to assume transitivity without a rule.
- Indexed retrieval and conservative unsupported-query behavior.
- Bounded session memory and lexical recall.
- Escaped delimiters in dataset records.
- Recursive directory training.
- Binary round-trip, replacement of an existing snapshot, and failed-load state preservation.

The tests are not a substitute for fuzzing, large-corpus benchmarks, cross-platform CI, or adversarial file-format testing. Those remain release-hardening work.
