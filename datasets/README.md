# Datasets

No large corpus is bundled. The `examples/core.sima` file is a tiny structured-format demonstration only.

Place your own UTF-8 datasets here. Use `.sima` for explicit FACT, WORD, RULE, and QA records; use `.txt` for supported prose templates or mixed structured/plain text. See [the structured format](../docs/DATA_FORMAT.md) and [the prose extraction guide](../docs/TEXT_EXTRACTION.md).

The reader processes files line by line, so large datasets do not need to be loaded into memory at once. Use sources with explicit redistribution and training permissions.
