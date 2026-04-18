# Forge CLI Documentation

This document describes the ASM format and scripting system used by [Hephaestus Toolworks – Forge](https://github.com/kaimitai/hephaestus-toolworks).

---

## Overview

Forge extracts and rebuilds the scripting layer of *The Battle of Olympus*.

Scripts are represented as a structured assembly format with:

- Defines (symbolic constants)
- Entry points (script roots)
- Instructions (opcodes + arguments)
- Labels (for control flow)

---

## File Structure

ASM files consist of two sections:

    [defines]
    [script]

### [defines]

Contains symbolic constants used by scripts.

Example:

    define NPC_ZEUS $40
    define ITEM_SANDALS $09
    define FLAG_ZEUS_MET $12

Used for:
- NPC identifiers
- Items
- Flags

---

### [script]

Contains all script logic.

Scripts are grouped by entrypoints:

    .entrypoint <world>.<room>

Example:

    .entrypoint 1.13

Each entrypoint defines the script executed for that world/room.

---

## Number Formats

The assembler accepts multiple numeric formats:

- Hex ($):        $0a
- Hex (0x):       0x0a
- Binary (%):     %1010
- Binary (0b):    0b1010
- Decimal:        10

All formats are interchangeable.

---

## Labels and Control Flow

Labels define jump targets:

    @label_name:

Used with control flow instructions:

    Jump @label_name
    IfFlag FLAG_X @label_name

Rules:
- Labels must be uniquely defined (but can have multiple references)
- Resolution is handled automatically
- Jumps are forward-only and relative to current instruction. Labels can not be more than 255 bytes ahead of where they are referenced. Assembly will fail if this happens.
- Labels start with @
- Labels end with : when defined, but not when referenced

---

## Strings

Strings are written inline:

    Msg "Hello <hero>"

Special tokens:

- &lt;n&gt; : New line
- &lt;p&gt; : Page break
- &lt;hero&gt; : Name of Hero
- &lt;heroine&gt; : Name of Heroine

The textboxes are 24 characters wide, so if a line has exactly that many characters no line-break has to be inserted.

Allowed characters are A-Z, a-z, 0-9, space and ?!.,'-

---

## Instruction Format

Each instruction has the form:

    <mnemonic> [argument] [label]

---

## Opcodes

### Core Instructions

SetNPC  
    SetNPC &lt;npc&gt;  
    Sets the active NPC.

SetFlag / ClearFlag  
    SetFlag &lt;flag&gt;  
    ClearFlag &lt;flag&gt;  
    Manipulates game flags.

GetItem  
    GetItem &lt;item&gt;  
    Grants an item.

    Important:
    In many scripts, this must be called twice:
    - First call prepares graphics
    - Second call (before End/MsgEnd) triggers the item drop

    If omitted, item graphics may be incorrect.

Msg  
    Msg "text"  
    Displays a message and continues execution.

MsgEnd  
    MsgEnd "text"  
    Displays a message and ends the script.

End  
    End  
    Terminates the script.

Note that the strings used by MsgEnd have to be encoded inline, while the strings used by Msg can be referenced in multiple places without space penalty. The assembler will make sure no duplicate strings will be generated, when used by Opcode Msg.

---

### Control Flow

Jump  
    Jump @label  
    Unconditional jump.

IfFlag / IfNotFlag  
    IfFlag &lt;flag&gt; @label  
    IfNotFlag &lt;flag&gt; @label  
    Conditional branching based on flags.

IfDecline  
    IfDecline @label  
    Branches if the player declines a choice.

IfSkinsLessThan20  
    IfSkinsLessThan20 @label  
    Branches if the player has fewer than 20 salamander skins.
    The value (20) is encoded in the opcode.

IfOlivesLessThan  
    IfOlivesLessThan &lt;value&gt; @label  
    Branches if the player has fewer olives than the given value.

---

### Script End Variants

These terminate execution with additional behavior:

    PasswordEnd
    HealthEnd
    NectarEnd

---

## Validation Rules

The assembler will fail if:

- Opcode is invalid
- Label is undefined
- Argument is missing or invalid
- Entry point format is incorrect

---

## Limits

Scripts must fit within available ROM space.

Forge reports usage:

    Used space: XXXX, available space: XXXX (XX.XX% used)

If the script exceeds available space, patching will fail.

---

## Notes

- Pointer tables and layout are handled automatically
- String deduplication is automatic
- No manual memory management is required
- ASM output is deterministic (round-trip stable)

---

## Status

This documentation reflects the current understanding of the scripting system.

Some behavior is inferred and may be refined over time.

Please contact the author for bug reports, suggestions or if you have useful information to share.
