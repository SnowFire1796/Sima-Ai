# C++ API overview

The public headers live in `include/sima`. Link an application against the `sima::core` CMake target.

## Train a knowledge base

```cpp
#include "sima/engine.hpp"

sima::SimaEngine engine;
const auto report = engine.train("datasets/examples");
```

A path can be a single `.sima` / `.txt` file or a directory. Directory ingestion is recursive and sorted by path. `.sima` uses explicit records; `.txt` supports documented English/Persian templates and explicit records, including Q/A pairs.

## Query and explain

```cpp
const auto direct = engine.ask("Earth", "IsA");
const auto answer = engine.query("Earth is a planet?");
const auto inferred = engine.infer(1000);

std::vector<sima::Fact> path;
const bool explained = engine.explain("Dog", "IsA", "Animal", path);
```

- `ask` returns direct facts only.
- `query` first checks normalized exact Q/A keys, then performs lexical fact retrieval and conservative evidence evaluation.
- `infer` computes explicit rule consequences without mutating source facts.
- `explain` returns a direct fact or a path supported by an enabled transitive rule.

## Persistence

```cpp
std::string error;
if (!engine.save("storage/knowledge.sima", error)) {
    // Handle error.
}
if (!engine.load("storage/knowledge.sima", error)) {
    // The existing in-memory knowledge remains unchanged on failure.
}
```

Binary version 2 stores concepts, facts, rules, and Q/A pairs. Version 1 snapshots remain readable. Bounded session memory is intentionally not persisted.

## Ownership notes

- `SimaEngine` owns its knowledge base, indexes, and session memory.
- Fact pointers returned by `ask` are non-owning and are invalidated by later knowledge mutations or loads.
- `infer` returns an owned vector of derived facts.
- `query` returns an owned `Answer` with copied evidence or a copied Q/A response.
