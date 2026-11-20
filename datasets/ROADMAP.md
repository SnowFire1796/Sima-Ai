# 500K corpus roadmap

The 500,000-record target is a collection goal, not a claim about the seed pack.

## Acceptance gates

- [ ] Every source has a recorded license and attribution requirements.
- [ ] Every record passes the Sima parser's actual format constraints.
- [ ] Exact duplicates and normalized-question collisions are audited.
- [ ] Conflicting factual claims are reviewed with source precedence.
- [ ] A held-out evaluation split is maintained outside training input.
- [ ] Dataset build manifest records hashes, counts, and transformations.
- [ ] Large-corpus training is profiled for memory, disk, and index growth.
- [ ] A sample of each domain is manually reviewed before release.

## Suggested acquisition order

1. Persian and English conversational intent-response examples.
2. Bilingual lexicons and synonym resources with compatible licenses.
3. Structured geographic, scientific, and general knowledge.
4. High-quality factual QA pairs with citations or source IDs.
5. Carefully curated logical rules and reasoning examples.

Do not convert an entire instruction-tuning dataset into QA blindly: multi-turn assistant traces, tool calls, personal data, and unsupported claims need filtering and transformation. The current Sima engine also cannot represent dialogue state or semantic intent robustly; those require implementation work, not merely more rows.
