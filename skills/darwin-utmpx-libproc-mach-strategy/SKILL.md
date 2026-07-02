---
name: darwin-utmpx-libproc-mach-strategy
description: "Darwin porting strategy for utilities that expect Solaris /proc, Linux /proc, or BSD kvm — use Apple libproc for process enumeration (proc_listpids + proc_pidinfo) and Mach umbrella <mach/mach.h> for task_info / thread_info / mach_port_deallocate. Utmpx is deprecated on Darwin but still functional; the Mach approach is the durable path. Applies to ps, whodo, pgrep, and any Heirloom utility that walks the process table."
gate: 3
version: "1.0.0"
author: moonman81
tags: [darwin, macos, libproc, mach, utmpx, ps, whodo, pgrep, process-enumeration]
depends_on: []
allowed-tools:
  - Read
  - Grep
  - Write
when_to_use: "Invoke when porting a Unix utility that scans /proc, walks kvm, reads utmpx, or otherwise enumerates processes / logins to Darwin. Triggers: 'port ps to Darwin', 'port pgrep', 'utmpx deprecated', 'kvm not on macOS', 'proc_listpids', 'mach_task_self', 'task_info Darwin', 'process enumeration on macOS'."
---

# Darwin utmpx / libproc / Mach strategy

## The problem

Heirloom's `ps`, `whodo`, and `pgrep` were authored against a mix of:

- **Solaris `/proc`** — file-per-process directory tree.
- **Linux `/proc`** — different layout but same idea.
- **BSD `<kvm.h>`** — kernel-memory library for direct kernel struct
  reads.
- **`<utmpx.h>`** — user login table.

Darwin has **none** of these in the form the code expects. `/proc`
doesn't exist. There is no `kvm(3)` library. `<utmpx.h>` is deprecated
since 10.9.

## The Darwin path

**Process enumeration:** `<libproc.h>`, specifically:
```c
#include <libproc.h>
proc_listpids(PROC_ALL_PIDS, 0, buf, bufsize);
proc_pidinfo(pid, PROC_PIDTASKALLINFO, 0, &info, sizeof info);
proc_pidpath(pid, buf, sizeof buf);
```

**Task / thread inspection:** the Mach umbrella:
```c
#include <mach/mach.h>       /* the umbrella — pulls in mach_init.h,
                                 mach_traps.h, task.h, thread_act.h,
                                 mach_port.h */
mach_task_self();
task_info(task, TASK_BASIC_INFO, &ti, &count);
task_threads(task, &tl, &tc);
thread_info(t, THREAD_BASIC_INFO, &ti, &count);
mach_port_deallocate(mach_task_self(), t);
```

**User logins (utmpx):** the header `<utmpx.h>` still exists, and
`getutxent()` / `endutxent()` still function. They are marked
deprecated (`__API_DEPRECATED_BEGIN(...)`). Silence via
`-Wno-deprecated-declarations` scoped to the translation unit.

**Kvm (`<kvm.h>`):** does not exist on Darwin. Do not include it.
The `pgrep` / `ps` upstream code has `#if defined(__NetBSD__) ||
defined(__OpenBSD__) || defined(__APPLE__)` guards that include
`<kvm.h>` in the Apple branch — these are **wrong** and must be
restructured so `__APPLE__` gets `<libproc.h>` while `__NetBSD__` /
`__OpenBSD__` retain `<kvm.h>`.

## Concrete patterns in the port

The port applies this shape to `pgrep.c`, `ps.c`, `whodo.c`:

```c
#if defined (__NetBSD__) || defined (__OpenBSD__) || defined (__APPLE__)
#include <sys/param.h>
#include <sys/sysctl.h>
#if defined (__APPLE__)
    /*
     * Darwin port: <kvm.h> was originally in this block, but Darwin
     * does not ship a kvm(3) library — macOS enumerates processes via
     * libproc (proc_listpids/proc_pidinfo) or sysctl KERN_PROC.
     * Include the Mach umbrella <mach/mach.h> so mach_task_self(),
     * task_info(), mach_port_deallocate() all resolve.
     */
    #include <mach/mach.h>
    #include <libproc.h>
#else
    #include <kvm.h>
#endif
#define proc process
#undef  p_pgid
#define p_pgid p__pgid
#endif
```

## Why the Mach umbrella specifically

Individual Mach headers like `<mach/mach_types.h>` and
`<mach/task_info.h>` provide **types and constants** but not
**function prototypes**. `mach_task_self()`, `task_info()`, and
friends live in the umbrella. The port's actual bug was including
only the sub-headers and getting `use of undeclared function
'mach_task_self'`. The fix in every case: replace the sub-header
includes with `<mach/mach.h>`.

## utmpx deprecation posture

The port chose to keep the utmpx code path (`who`, `users`, `shl`,
`logins`, `whodo`) on Darwin rather than rewriting them via libproc,
because `<utmpx.h>` is still functional. Deprecation warnings are
silenced at the translation-unit level. When Apple eventually removes
utmpx entirely, the utilities that depend on it will need a libproc
+ `IOKit`-based re-authoring.

## References

- Apple `libproc.h`, `<mach/mach.h>` — SDK-shipped headers.
- Sun / Solaris `proc(4)`, `psrinfo(1M)` — for the original semantics
  ports are trying to preserve.
- Upstream Heirloom `ps/ps.c`, `whodo/whodo.c`, `pgrep/pgrep.c`.
- Port commit that applied this pattern:
  `phase 3b-iii: pgrep/ps/whodo — Darwin Mach + libproc`.
