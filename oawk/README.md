# oawk

old AWK — the pre-nawk implementation.

## Where this fits

This directory is part of `moonman81/heirloom-toolchest-darwin`, the
Darwin port of the toolchest package from Gunnar Ritter's Heirloom
Project. See the repo root `README.md`, `PROVENANCE.md`, and
`NOTICE.md` for context.

**Not authoritative.** Upstream is
`http://heirloom.sourceforge.net/` (unmaintained since ≈ 2008).
Port fixes here are for macOS 26.4 arm64 compatibility, not for
new feature work.

## Contents

- **C sources**: awk.g.c, awk.lx.c, b.c, freeze.c, lib.c, main.c, parse.c, proc.c, proctab.c, run.c, token.c, tran.c (+1 more)
- **Headers**: awk.g.h, awk.h, heirloom_flags.h
- **Build**: Makefile, Makefile.mk
- **Man pages**: oawk.1

## Modality

Every installed binary honours the shared help / version / variant
/ dialect flag set:

- `--help`, `--usage`, `-H`  → man page
- `--version`, `-V`          → port banner (built variant + active variant)
- `--variants`               → list personality variants installed
- `--describe-modality`      → full modality matrix
- `--variant=<name>`, `HEIRLOOM_VARIANT=<name>`, `HEIRLOOM_DIALECT=<name>`
  → re-exec into the requested personality binary

See `heirloom_flags.h` (in each source directory) for the shared shim.

## Licence

Per-file patchwork — CDDL-1.0 / Caldera / Lucent / GPL-2.0-or-later /
LGPL-2.0-or-later / zlib. See headers on each source file and the
per-package `NOTICE.md`.
