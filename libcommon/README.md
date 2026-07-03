# libcommon

shared C support library (getopt, memalign, pfmt, etc.).

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

- **C sources**: CHECK.c, asciitype.c, getdir.c, getopt.c, gmatch.c, ib_alloc.c, ib_close.c, ib_free.c, ib_getlin.c, ib_getw.c, ib_open.c, ib_popen.c (+21 more)
- **Headers**: _alloca.h, _malloc.h, _utmpx.h, alloca.h, asciitype.h, atoll.h, blank.h, getdir.h, heirloom_flags.h, iblok.h, malloc.h, mbtowi.h (+10 more)
- **Build**: Makefile, Makefile.mk
- **Subdirs**: sys

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
