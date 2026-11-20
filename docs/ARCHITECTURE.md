# Architecture

Sima is a symbolic, knowledge-centric engine. Language-specific labels resolve to language-independent concept IDs.

## Data flow

1. **Text layer** — UTF-8 validation, normalization, tokenization, coarse language detection.
2. **Concept store** — canonical concepts and aliases.
3. **Dataset reader** — bounded line-by-line ingestion and diagnostics.
4. **Knowledge base** — subject–relation–object facts, provenance, confidence, and subject index.
5. **Reasoner** — explicit transitive rules and explainable paths.
6. **Retrieval** — inverted term index over labels, relations, and source metadata.
7. **Answer layer** — evidence-first retrieval responses.
8. **Persistence** — versioned binary snapshots with bounds checks.

## Invariants

- Concept ID zero means unresolved.
- Facts require valid subject/object IDs and a non-empty relation.
- Alias normalization is centralized in `text::normalize`.
- Pointers returned by `ask` remain valid only until the knowledge base mutates.
- Loading validates a temporary candidate before replacing active state.
- Derived facts are computed on demand and are not silently persisted as source facts.

## Complexity

- Alias lookup: expected O(1).
- Subject lookup: expected O(k) for matching subject postings.
- Retrieval: proportional to query postings plus ranking.
- The initial transitive reasoner uses breadth-first traversal from each source concept. Large dense graphs can still be expensive; inference output is capped.

## Symbolic boundary

Sima stores explicit records and applies explicit algorithms. It does not learn statistical language representations or acquire broad world knowledge merely by reading arbitrary text. The extractor in this version is intentionally schema-driven: it ingests structured facts rather than pretending to reliably extract arbitrary natural-language statements.
