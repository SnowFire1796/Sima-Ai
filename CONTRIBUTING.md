# Contributing

Thanks for helping improve Sima.

## Development requirements

- CMake 3.25 or newer.
- A compiler with C++26 language-mode support.
- No third-party runtime libraries are required.

## Build and test

```sh
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

Before submitting a change:

1. Add or update tests for observable behavior.
2. Keep symbolic inference explicit and explainable.
3. Preserve dataset compatibility or document a format-version change.
4. Do not silently discard source provenance.
5. Do not describe lexical retrieval as semantic understanding.
6. Keep external datasets out of commits unless their license permits redistribution.

## Code conventions

Use the repository's `.clang-format` configuration. Prefer small interfaces, explicit ownership, bounded input, and deterministic algorithms.
