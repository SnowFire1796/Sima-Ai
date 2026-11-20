# Dataset and binary formats

## Text records

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

### Escaping and limits

- `\\|` encodes a literal pipe.
- `\\\\` encodes a literal backslash.
- Unknown escape sequences are preserved.
- Each input line is limited to 1 MiB by default.

## Binary snapshot

A snapshot begins with ASCII `SIMA` and a little-endian 32-bit version. Version 1 stores concepts, facts, and rules using length-prefixed UTF-8 strings and explicit little-endian integers. Load-time limits protect against unreasonable counts and string sizes.

Saving writes a sibling temporary file and renames it into place. Atomic replacement behavior varies by filesystem and operating system; the implementation does not claim fsync-level durability.
