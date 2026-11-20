# Sima dataset program

This repository contains a small **original seed corpus** and a reproducible dataset assembly tool. It does **not** claim that 500,000 verified examples are already bundled. Large corpora must be imported from sources whose licenses permit the intended use and redistribution.

## Current seed data

- `conversation/fa/core.sima`: original Persian and English conversational Q/A seeds.
- `knowledge/core.sima`: multilingual aliases and basic structured facts.
- `examples/core.sima`: parser-format example.

The current Sima engine supports only `FACT`, `WORD`, `RULE`, and exact normalized-question `QA` records. It does not learn general conversational semantics from dialogue turns. Q/A pairs are exact-key lookup, not semantic matching.

## Target corpus composition (500,000 records)

| Track | Target records | Intended representation |
|---|---:|---|
| Persian conversational intents and responses | 60,000 | QA pairs; exact lookup only |
| English conversational intents and responses | 30,000 | QA pairs |
| Bilingual vocabulary and aliases | 80,000 | WORD records |
| General and scientific knowledge | 180,000 | FACT records |
| Curated factual QA | 90,000 | QA pairs |
| Reasoning rules and examples | 30,000 | FACT / RULE; only sound rules |
| **Total** | **500,000** | |

These are collection targets, not generated counts. Do not fill quotas with duplicated, templated, or unverified claims. Keep a source manifest for every imported corpus.


## GPT-6-generated synthetic seed (2026-11-20)

The repository contains original synthetic examples authored with GPT-6. A repository-wide recount of the current core and generated structured files reports:

- **1347 raw records** before exact duplicate elimination.
- **1347 unique exact records** after exact duplicate elimination.
- FACT: 301; WORD: 201; RULE: 4; QA: 841.
- Persian and English QA, bilingual technical vocabulary, and structured knowledge spanning science, mathematics, C++, algorithms, language, reasoning, software engineering, web development, networking, security, operating systems, and databases.

These are model-generated examples, **not** OpenAI's private pretraining dataset, a GPT-5.6/Luna dataset, or a dump of model weights or training data. Source tags identify synthetic provenance. Generated facts are not externally sourced citations and should be independently checked before high-stakes use.

This remains a seed corpus, not the 500K target. The builder performs exact deduplication and validates record structure. A parser-compatible audit of the current batch found 0 malformed records and 0 exact duplicate(s) before deduplication. Keep generated data separate from externally sourced data and preserve source tags during processing.

## Build a corpus from licensed sources

The standard-library-only builder validates record syntax, rejects malformed UTF-8 / empty fields, deduplicates exact records, writes deterministic shards, and reports category counts.

```sh
python3 scripts/build_dataset.py --input datasets --output build/dataset --target 500000
```

For external corpora, convert them to documented record types and place them in a separate input directory. Avoid including the generated output directory as an input.

## Source and quality policy

1. Record original dataset name, version, URL, license, retrieval date, and transformations in `datasets/SOURCES.md`.
2. Preserve attribution and license notices required by upstream sources.
3. Do not mix evaluation/test examples into training data.
4. Normalize Persian ی/ي and ک/ك consistently, but preserve original text where possible.
5. Deduplicate normalized questions and facts; review conflicting answers instead of silently choosing one.
6. Keep confidence/provenance metadata in source manifests; current `QA` and `FACT` formats have limited metadata support.
7. Treat generated examples as synthetic and label them as such.
8. Never infer a fact solely because it appears in a generated template.

## Important limitation

A 500K corpus will expand Sima's lookup and fact coverage, but it will not turn the current symbolic engine into an LLM. The present extractor and response system are deliberately narrow. A separate conversational intent matcher, semantic retrieval layer, conflict resolution, and richer evidence model are needed for natural multi-turn dialogue.
