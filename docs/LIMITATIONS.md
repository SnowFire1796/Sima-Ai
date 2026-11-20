# Current limitations and honest guarantees

Sima 0.1 is a symbolic engine with explicit data structures and algorithms, not a general-purpose conversational model.

## Language

- Persian normalization handles common Arabic/Persian character variants and removes joiner/tatweel code points for matching.
- It does not implement full Unicode normalization, stemming, morphology, transliteration, or grammatical parsing.
- Language detection is a coarse script heuristic, not a language identification model.

## Learning and extraction

- Structured datasets support FACT, WORD, RULE, and QA records.
- Plain-text extraction supports only the finite templates listed in [TEXT_EXTRACTION.md](TEXT_EXTRACTION.md).
- It does not perform unrestricted information extraction, coreference resolution, entity linking, negation handling, or fact verification.
- Q/A responses are returned only for normalized exact-question matches; they are not semantic paraphrase matching.
- Unsupported sentences are counted and skipped; they are not converted into guessed facts.
- Dataset quality, source attribution, and licensing remain external responsibilities.

## Reasoning

- Only explicitly declared transitive relations are inferred.
- No general first-order logic, negation, temporal reasoning, uncertainty calculus, or contradiction resolution is implemented.
- Confidence is metadata; it is not a calibrated probability.

## Answering

- Query is exact Q/A lookup followed by lexical fact retrieval, not unrestricted natural-language question answering.
- A relevant search hit is evidence of lexical overlap, not proof that it answers the user's intent.
- Unsupported questions should remain unanswered.

## Operations

- Large-corpus performance requires benchmarking and likely optimized incremental indexes.
- Binary format version 2 adds Q/A pairs and remains backward-readable for version 1 snapshots.
- Future incompatible changes must increment the version and provide migration tooling.
