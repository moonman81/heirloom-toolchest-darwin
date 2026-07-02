---
name: svr4-personality-tree-preservation
description: "SVR4-style personality tree convention preserved by Heirloom toolchest — /bin (SVID3 default), /bin/s42 (SVID4/SVR4.2), /bin/posix (POSIX.2/SUSv2), /bin/posix2001 (POSIX.1-2001/SUSv3), /ucb (BSD/UCB). Each utility can install multiple variants — same source, different -D flags (-DSUS, -DSU3, -DUCB) select the behaviour personality. Preserving all five install paths is what makes 20-year-old scripts run unchanged."
gate: 2
version: "1.0.0"
author: moonman81
tags: [heirloom, toolchest, svr4, svid3, svid4, posix, ucb, personality-tree]
depends_on: []
allowed-tools:
  - Read
when_to_use: "Invoke when scoping a toolchest install path, when a user asks why the same utility exists in multiple bin/ subdirs, or when debugging behaviour differences between /opt/heirloom/bin/awk and /opt/heirloom/bin/posix/awk. Triggers: 'personality tree', 'SVR4 install layout', '/bin/s42', '/bin/posix', 'SVID3 vs SVID4', 'oawk vs nawk', 'multiple awks'."
---

# SVR4 personality tree preservation

## The tree

`/opt/heirloom/` layout:

```
bin/            SVID3 personality (default, most traditional)
bin/s42/        SVID4 / SVR4.2 personality
bin/posix/      POSIX.2 / SUSv2 personality
bin/posix2001/  POSIX.1-2001 / SUSv3 personality
ucb/            UCB / BSD-style utilities (not a full personality)
ccs/bin/        CCS — yacc, lex, m4, make, sccs
```

Same utility, different personality directories = different
behaviour. Example: `awk`.

- `/opt/heirloom/bin/awk` → the **old awk** (oawk) — Aho-Weinberger-
  Kernighan original.
- `/opt/heirloom/bin/s42/awk` → nawk under SVID4 rules.
- `/opt/heirloom/bin/posix/awk` → nawk under POSIX.2 rules.
- `/opt/heirloom/bin/posix2001/awk` → nawk under POSIX.1-2001 (SUSv3)
  rules.

## Compile-time selection

The toolchest builds one utility source under multiple `-D` flags to
select personality:

```make
utility:        utility.o           → bin/utility          (SVID3)
utility_sus:    -DSUS utility.o     → bin/posix/utility    (POSIX.2)
utility_su3:    -DSU3 utility.o     → bin/posix2001/utility (POSIX.1-2001)
utility_ucb:    -DUCB utility.o     → ucb/utility          (BSD)
utility_s42:    -DS42 utility.o     → bin/s42/utility      (SVID4)
```

Each `#ifdef SUS / #ifdef SU3 / #ifdef UCB` in the source flips a
behavioural switch: option-parsing rules, argument order, exit codes,
error-message format, locale handling.

## Why the tree matters

- 20-year-old SVR4 scripts run unchanged when `PATH` starts with
  `/opt/heirloom/bin`.
- A user maintaining a mixed-era corpus can select the correct
  personality per script via a PATH prefix.
- The differences are behavioural, not just cosmetic — some POSIX
  utilities have exit codes that differ from SVR4 on error paths.

## Personality preservation rules

1. **Never install into just one path.** If upstream builds multiple
   personality variants, install all of them.
2. **Never merge personalities.** Some downstream ports collapse the
   tree into a single `/opt/heirloom/bin`. This defeats the whole
   point.
3. **`mk.config` `SV3BIN` / `S42BIN` / `SUSBIN` / `SU3BIN` / `UCBBIN`
   / `DEFBIN`** are the six variables. All should be set. All should
   be distinct.

## Anti-pattern: personality skew

The Darwin port has a known personality install skew — some
personality variants build but don't install with `make phase3-install`
(`bin/s42` shows 7 installed vs 41 built at time of writing). This is a
real issue documented in the appraisal and workspace `PORT.md`.
Investigate `_install/install_ucb` and the toolchest top-level
`makefile`'s install rule.

## Common utility personality matrix

Utilities with the most personality variants:

- `awk` — 3 personalities (old awk = SVID3; nawk = POSIX / POSIX-2001)
- `cp`, `ls`, `sort`, `du`, `df` — 3+ personalities
- `expr`, `test` — 2 personalities
- `basename`, `chmod`, `uniq` — 3 personalities
- `ed`, `sed` — 3+ personalities (regex behaviour changes)

## Reference

- SVID3 = System V Interface Definition, Third Edition (USL, 1992).
- SVID4 = System V Interface Definition, Fourth Edition (Novell, 1995).
- SUSv2 = POSIX.2 / IEEE 1003.2-1992.
- SUSv3 = POSIX.1-2001 / IEEE 1003.1-2001.
- Upstream `intro.1` in this repo.
