#!/usr/bin/env bash
# Unit test for the factorial program
# Verifies compilation, valid/boundary values, invalid-input behavior, and cleanup.

set -e

cd "$(dirname "$0")/.."

expect_output() {
    local input="$1"
    local expected="$2"
    local output
    output=$(printf '%s\n' "$input" | ./factorial)
    if [ "$output" != "$expected" ]; then
        echo "[FAIL] input '$input': expected '$expected', got '$output'"
        exit 1
    fi
}

expect_failure() {
    local description="$1"
    shift
    if "$@" 2>/dev/null; then
        echo "[FAIL] $description: expected non-zero exit status"
        exit 1
    fi
}

echo "[TEST] Building factorial..."
make clean
make factorial

echo "[TEST] Checking valid inputs..."
expect_output 0 1
expect_output 1 1
expect_output 5 120
expect_output 10 3628800
expect_output 20 2432902008176640000

echo "[TEST] Checking invalid inputs..."
expect_failure "negative input" sh -c "printf '%s\n' -1 | ./factorial"
expect_failure "input above 20" sh -c "printf '%s\n' 21 | ./factorial"
expect_failure "non-numeric input" sh -c "printf '%s\n' abc | ./factorial"
expect_failure "missing input" sh -c "./factorial < /dev/null"
expect_failure "trailing text" sh -c "printf '%s\n' '5xyz' | ./factorial"

echo "[TEST] Checking clean target..."
make clean
if [ -f factorial ]; then
    echo "[FAIL] clean target did not remove factorial"
    exit 1
fi
echo "[PASS] clean target removed factorial."

echo "[PASS] All tests passed."
