# Current limitations and honest guarantees

Sima 0.1 is a symbolic prototype with real data structures and algorithms, not a general-purpose conversational model.

## Language

- Persian normalization handles common Arabic/Persian character variants and removes joiner/tatweel code points for matching.
- It does not implement full Unicode normalization, stemming, morphology, transliteration, or grammatical parsing.
- Language detection is a coarse script heuristic, not a language identification model.

## Learning

- The reader ingests explicit FACT, WORD, and RULE records.
- It does not reliably extract facts from arbitrary prose or learn from unstructured conversation.
- Dataset quality, source attribution, and licensing remain external responsibilities.

## Reasoning

- Only explicitly declared transitive relations are inferred.
- No general first-order logic, negation, temporal reasoning, uncertainty calculus, or contradiction resolution is implemented.
- Confidence is metadata; it is not a calibrated probability.

## Answering

- Query is lexical retrieval, not unrestricted natural-language question answering.
- A relevant search hit is evidence of lexical overlap, not proof that it answers the user's intent.
- Unsupported questions should remain unanswered.

## Operations

- Large-corpus performance requires benchmarking and likely optimized incremental indexes.
- Binary format version 1 is an initial format; future incompatible changes must increment the version and provide migration tooling.
