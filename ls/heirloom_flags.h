/*
 * heirloom_flags.h - shared -h/--help/--usage/-v/--version flag handling
 * for the Heirloom Darwin port.
 *
 * This is port scaffolding, NOT upstream Ritter code.
 * SPDX-License-Identifier: Zlib
 * Portions Copyright (c) 2026 moonman81 <i.am.moonman@gmail.com>
 *
 * USAGE
 *   #include "heirloom_flags.h"
 *   int main(int argc, char **argv) {
 *           heirloom_flags(argc, argv,
 *                          "toolname",          // basename shown in output
 *                          HF_VERBOSE_TAKEN);   // 0, or HF_VERBOSE_TAKEN if -v means verbose
 *           ...
 *   }
 *
 *   heirloom_flags scans argv[1..argc-1] and, on the first match against
 *   -h / -H / --help / --usage / -v / -V / --version (skipping -v when
 *   HF_VERBOSE_TAKEN is set), prints the appropriate output and calls
 *   exit(0). If nothing matches, it returns and the caller proceeds with
 *   its normal getopt() loop.
 *
 *   -H and -V are ALWAYS recognised, even when the lowercase form is
 *   claimed by legacy semantics, so callers can advertise the caps flag.
 */
#ifndef HEIRLOOM_FLAGS_H
#define HEIRLOOM_FLAGS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define HF_VERBOSE_TAKEN  0x1  /* -v means "verbose" in this tool */
#define HF_H_TAKEN        0x2  /* -h means something else (rare) */

#ifndef HEIRLOOM_PORT_VERSION
# define HEIRLOOM_PORT_VERSION "1.0.0-darwin-arm64"
#endif
#ifndef HEIRLOOM_PORT_DATE
# define HEIRLOOM_PORT_DATE "2026-07-03"
#endif

static void heirloom_flags_help(const char *tool) {
	/* Try to exec `man tool`. Fall back to a stub if man is not on PATH. */
	execlp("man", "man", tool, (char *)NULL);
	fprintf(stdout,
	    "%s (Heirloom Darwin port %s, %s)\n"
	    "\n"
	    "For full documentation run:  man %s\n"
	    "\n"
	    "This is a downstream Darwin port of the Heirloom Project.\n"
	    "It is NOT an authoritative source. Upstream:\n"
	    "  http://heirloom.sourceforge.net\n",
	    tool, HEIRLOOM_PORT_VERSION, HEIRLOOM_PORT_DATE, tool);
	exit(0);
}

static void heirloom_flags_version(const char *tool) {
	fprintf(stdout,
	    "%s (Heirloom Darwin port) %s\n"
	    "  build date:      %s\n"
	    "  upstream:        Heirloom Project <http://heirloom.sourceforge.net>\n"
	    "  port maintainer: moonman81 <i.am.moonman@gmail.com>\n"
	    "  licence:         per-file patchwork (CDDL/Caldera/Lucent/GPL/LGPL/zlib)\n"
	    "  warranty:        none, no fitness guarantee, port-status only\n",
	    tool, HEIRLOOM_PORT_VERSION, HEIRLOOM_PORT_DATE);
	exit(0);
}

static void heirloom_flags(int argc, char **argv, const char *tool, int mask) {
	int i;
	if (argc < 2) return;
	for (i = 1; i < argc; i++) {
		if (argv[i] == NULL) break;
		/* Stop at first non-option — legacy tools may have positional
		 * paths after flags; don't scan past them. */
		if (argv[i][0] != '-') return;
		if (strcmp(argv[i], "--") == 0) return;
		/* Help flags */
		if (!(mask & HF_H_TAKEN) &&
		    (strcmp(argv[i], "-h") == 0))
			heirloom_flags_help(tool);
		if (strcmp(argv[i], "-H") == 0 ||
		    strcmp(argv[i], "--help") == 0 ||
		    strcmp(argv[i], "--usage") == 0)
			heirloom_flags_help(tool);
		/* Version flags */
		if (!(mask & HF_VERBOSE_TAKEN) &&
		    (strcmp(argv[i], "-v") == 0))
			heirloom_flags_version(tool);
		if (strcmp(argv[i], "-V") == 0 ||
		    strcmp(argv[i], "--version") == 0)
			heirloom_flags_version(tool);
	}
}

#endif /* HEIRLOOM_FLAGS_H */
