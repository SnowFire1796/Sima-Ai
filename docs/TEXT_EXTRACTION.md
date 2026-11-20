# Plain-text extraction

Files ending in `.txt` are processed by the deterministic template extractor. Files ending in `.sima` use the explicit structured-record parser. A TXT file may mix both formats.

## Supported English patterns

- `Earth is a planet.` → `Earth --IsA--> planet`
- `The Sun is a star.` → `Sun --IsA--> star`
- `Earth orbits the Sun.` → `Earth --Orbits--> the sun`
- `A dog has a tail.` → `dog --Has--> a tail`
- `Paris is located in France.` → `paris --LocatedIn--> france`
- `A wheel is part of a car.` → `wheel --PartOf--> a car`

## Supported Persian patterns

- `زمین یک سیاره است.` → `زمین --IsA--> سیاره`
- `سگ پستاندار است.` → `سگ --IsA--> پستاندار`
- `آب در اقیانوس است.` → `آب --LocatedIn--> اقیانوس`
- `درخت دارای برگ است.` → `درخت --Has--> برگ`
- `چرخ بخشی از خودرو است.` → `چرخ --PartOf--> خودرو`
- `زمین به دور خورشید می‌چرخد.` → `زمین --Orbits--> خورشید`

The extractor is intentionally conservative and supports one-token subjects plus simple relation templates. It does not perform full grammar parsing, entity resolution, negation handling, coreference, tense analysis, or fact verification. Unsupported sentences are counted rather than guessed.

Extracted prose facts use confidence metadata 0.65 and retain the source file and line number. This value is a heuristic provenance marker, not a calibrated probability. Structured FACT records remain the preferred way to provide high-quality knowledge.
