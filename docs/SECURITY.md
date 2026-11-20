# Security and data handling

Sima is an offline local program. It does not open network connections, execute dataset contents, load plugins, or evaluate embedded code.

## Dataset input

- Input is read as UTF-8 text, one line at a time.
- The default line-size limit is 1 MiB.
- Invalid UTF-8 and malformed records are skipped with diagnostics.
- Only the explicit FACT, WORD, RULE, and QA record types are interpreted.
- Dataset files are data, not executable scripts.

## Binary snapshots

The loader checks the magic signature, version, record counts, string lengths, concept references, enum values, confidence range, duplicate records, and trailing bytes before replacing the active knowledge base. Version 1 snapshots remain readable; new snapshots are written as version 2.

Do not treat a snapshot from an untrusted source as inherently safe. Keep backups of valuable knowledge files. The format is versioned but has not yet undergone independent security review or fuzz testing.

## Privacy

Session memory is held in process memory and is discarded when the application exits. Knowledge snapshots store facts, aliases, rules, and Q/A responses; they do not store session history. Do not ingest private or sensitive datasets unless you have a clear reason and appropriate authorization.
