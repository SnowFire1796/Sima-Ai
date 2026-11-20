# Dataset and binary formats

## Structured records

UTF-8, newline-delimited. Blank lines and comment lines beginning with `#` are ignored.

### Facts

`FACT|subject|relation|object[|source]`

Example: `FACT|Earth|Orbits|Sun|astronomy-intro`

### Aliases

`WORD|alias|canonical-concept`

Example: `WORD|زمین|Earth`

An alias resolves to the same concept ID as its canonical concept. Alias collisions are diagnosed instead of silently remapped.

### Rules

`RULE|relation|TRANSITIVE`

Example: `RULE|IsA|TRANSITIVE`

The reasoner computes transitive consequences for that relation.

### Q/A responses

`QA|question|answer[|source]`

Example: `QA|What is the capital of France?|Paris|world-atlas`

Question lookup uses normalized tokens, including case and common punctuation normalization. Multiple answers for the same question are retained; the highest-confidence answer is selected, with deterministic tie-breaking. The initial dataset format assigns confidence 1.0 to explicit Q/A records.

## Escaping and limits

- `\\|` encodes a literal pipe.
- `\\\\` encodes a literal backslash.
- Unknown escape sequences are preserved.
- Each input line is limited to 1 MiB by default.

## Plain-text datasets

Files ending in `.txt` are processed by the conservative template extractor. They may contain ordinary English or Persian sentences and may also contain FACT, WORD, RULE, and QA records. Unmatched sentences are counted, not guessed.

Files ending in `.sima` use only the structured record parser.

## Binary snapshot

A snapshot begins with ASCII `SIMA` and a little-endian 32-bit version. Version 2 stores concepts, facts, rules, and Q/A responses using length-prefixed UTF-8 strings and explicit little-endian integers. Counts and string lengths are bounded during load. The loader also accepts version 1 snapshots, which contain no Q/A response section.

The file is written to a sibling temporary file and renamed into place. Atomic replacement behavior varies by filesystem and operating system; the implementation does not claim fsync-level durability.
