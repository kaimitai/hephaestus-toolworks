# Battle of Olympus – String Encoding

## Overview

Strings are encoded as Pascal-style bitstreams using 5-bit tokens.

Each string consists of:
- 1 byte: token count (number of 5-bit tokens)
- followed by: bit-packed token stream (MSB-first)

Tokens are decoded sequentially until the token count is exhausted.

---

## Token Stream

- Tokens are 5 bits each
- Packed across byte boundaries
- Read MSB-first

---

## Decoder State

The decoder maintains:

- Base table (case):
  - Uppercase (default)
  - Lowercase

- Numbers/Symbols mode:
  - Disabled (default)
  - Enabled via toggle

---

## Control Tokens

| Token | Meaning |
|------|--------|
| `00000` (0x00) | Toggle numbers/symbols mode |
| `11011` (0x1B) | Toggle upper/lower case |
| `11100` (0x1C) | Name escape (consumes next token) |
| `11111` (0x1F) | Page break (`<p>`), resets state |

---

## Name Escape

Token `11100` consumes one additional token:

| Subtoken | Meaning |
|----------|--------|
| `00000`  | `<hero>` |
| `00001`  | `<heroine>` |

---

## Character Tables

### Normal Mode

- `00001–11010` → letters (`A–Z` or `a–z`)
- `11101` → space
- `11110` → newline (`<n>`)
- `11111` → page (`<p>`)

---

### Numbers/Symbols Mode

- `00001–01010` → digits (`0–9`)
- `01011–10000` → punctuation (`? ! . , ' -`)

### Important behavior

- Tokens `< 0x0B` (digits) keep numbers mode active
- Tokens `>= 0x0B` (punctuation) automatically exit numbers mode

---

## State Reset

Page break (`11111`) resets:
- Base table → uppercase
- Numbers mode → disabled

---

## Opcode Interaction

Within scripts:

- Opcode `03` → displays string via pointer
- Opcode `09` → inline string (this encoding)

Inline strings must be fully consumed by the decoder to keep script parsing aligned.

---

## Summary

The text system is a state machine over a 5-bit token stream:

- BitReader → extracts tokens
- State machine → interprets tokens
- Tables → map tokens to characters

Correct decoding requires reproducing state transitions, not just lookup tables.