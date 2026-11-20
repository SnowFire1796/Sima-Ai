# Architecture

Sima is a symbolic, knowledge-centric engine implemented in C++26. Language-specific labels resolve to language-independent concept IDs.

## Processing pipeline

1. **Input and language** — UTF-8 validation, normalization, tokenization, and coarse script detection.
2. **Concept store** — canonical concepts and multilingual aliases.
3. **Learning** — stream structured `.sima` or mixed prose/record `.txt` datasets from files or recursively scanned directories; report per-file and per-line diagnostics.
4. **Knowledge base** — subject–relation–object facts, provenance, confidence, explicit Q/A pairs, and indexes.
5. **Reasoner** — explicit transitive rules, deterministic breadth-first closure, and evidence-path explanations.
6. **Retrieval** — inverted term index over fact labels, relations, source metadata, and concept aliases.
7. **Evaluation** — lexical evidence coverage heuristic; matched alias terms count as evidence overlap, but this is not semantic truth verification.
8. **Generation** — exact Q/A response retrieval followed by conservative formatting of fact evidence; no unrestricted language synthesis.
9. **Memory** — bounded in-process session history; it is not persisted in the knowledge snapshot.
10. **Persistence** — versioned binary snapshots with bounds checks and deterministic concept ordering.

## Invariants

- Concept ID zero means unresolved.
- Facts require valid subject/object IDs and a non-empty relation.
- Q/A keys are generated from normalized tokens.
- Alias normalization is centralized in `text::normalize`.
- Pointers returned by `ask` remain valid only until the knowledge base mutates.
- Loading validates a temporary candidate before replacing active state.
- Derived facts are computed on demand and are not silently persisted as source facts.
- Session memory is bounded and disappears when the process exits.
- Plain-text extraction only emits facts for explicitly supported templates.

## Complexity

- Alias lookup: expected O(1).
- Subject lookup: expected O(k) for matching subject postings.
- Exact Q/A lookup: expected O(r), where r is the number of responses attached to the normalized question key.
- Fact insertion: expected O(1) index update after duplicate lookup.
- Retrieval: proportional to query postings plus ranking; each fact is indexed once per normalized token across canonical labels and aliases.
- Transitive reasoning: breadth-first traversal from each source concept; dense graphs can still be expensive, so inference output is capped.
- Directory training sorts discovered file paths before ingestion to keep concept allocation and record order reproducible.

## Symbolic boundary

Sima stores explicit records and applies explicit algorithms. It does not learn statistical language representations or acquire broad world knowledge merely by reading arbitrary text. The trainer recognizes a small, documented set of sentence structures and exact Q/A pairs; it does not pretend to reliably extract arbitrary prose.
