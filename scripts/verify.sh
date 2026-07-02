#!/bin/sh
# verify.sh — post-install smoke test for heirloom-toolchest
set -eu
PREFIX="${1:-/opt/heirloom}"
BIN="$PREFIX/bin"

if tty >/dev/null 2>&1; then
	C_OK='\033[32m'; C_FAIL='\033[31m'; C_RESET='\033[0m'
else
	C_OK=''; C_FAIL=''; C_RESET=''
fi
ok()   { printf '  %b✓%b %s\n' "$C_OK" "$C_RESET" "$*"; }
fail() { printf '  %b✗ %s%b\n' "$C_FAIL" "$*" "$C_RESET"; exit 1; }

# Core utilities present
for t in cat cp mv rm ls date echo cpio tar awk sed grep sort wc; do
	[ -x "$BIN/$t" ] || fail "$BIN/$t missing"
done
ok 'core utilities present'

# Personality directories
for pd in s42 posix posix2001; do
	[ -d "$BIN/$pd" ] || fail "personality dir $BIN/$pd missing"
done
[ -d "$PREFIX/ucb" ] || fail "$PREFIX/ucb missing"
ok 'personality dirs present'

# awk smoke — SVID3 (default = oawk) + POSIX-2001 (nawk)
out=$("$BIN/awk" 'BEGIN{print "svid3"}' </dev/null)
[ "$out" = 'svid3' ] || fail "svid3 awk failed: $out"
ok 'svid3 awk'
out=$("$BIN/posix2001/awk" 'BEGIN{print "posix2001"}' </dev/null)
[ "$out" = 'posix2001' ] || fail "posix2001 awk failed: $out"
ok 'posix2001 awk'

# ISO 8601 date
d=$("$BIN/date" '+%Y-%m-%d')
echo "$d" | grep -qE '^[0-9]{4}-[0-9]{2}-[0-9]{2}$' || fail "date ISO 8601: $d"
ok "date ISO 8601: $d"

# cpio round-trip
TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP/src" "$TMP/dst"
echo hello > "$TMP/src/a"
(cd "$TMP/src" && ls a | "$BIN/cpio" -o > "$TMP/arch") 2>/dev/null
(cd "$TMP/dst" && "$BIN/cpio" -i < "$TMP/arch") 2>/dev/null
[ "$(cat "$TMP/dst/a")" = hello ] || fail 'cpio round-trip'
ok 'cpio round-trip'

# tar round-trip
mkdir -p "$TMP/tsrc" "$TMP/tdst"
echo world > "$TMP/tsrc/b"
(cd "$TMP/tsrc" && "$BIN/tar" -cf "$TMP/tarball" .)
(cd "$TMP/tdst" && "$BIN/tar" -xf "$TMP/tarball")
[ "$(cat "$TMP/tdst/b")" = world ] || fail 'tar round-trip'
ok 'tar round-trip'

printf '%bverify: toolchest OK%b\n' "$C_OK" "$C_RESET"
