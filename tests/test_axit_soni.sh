#!/usr/bin/env bash
# Unit test for the Axit Soni program
# Verifies the program compiles, prints "Axit Soni", and exits with code 0.

set -e

cd "$(dirname "$0")/.."

echo "[TEST] Building axit_soni..."
make clean
make axit_soni

echo "[TEST] Running axit_soni..."
output=$(./axit_soni)
exit_code=$?

echo "[TEST] Checking output..."
expected="Axit Soni"
if [ "$output" != "$expected" ]; then
    echo "[FAIL] Expected: '$expected', Got: '$output'"
    exit 1
fi

echo "[TEST] Checking exit code..."
if [ "$exit_code" -ne 0 ]; then
    echo "[FAIL] Expected exit code 0, Got: $exit_code"
    exit 1
fi

echo "[PASS] axit_soni output and exit code are correct."

echo "[TEST] Checking clean target..."
make clean
if [ -f axit_soni ]; then
    echo "[FAIL] clean target did not remove axit_soni"
    exit 1
fi
echo "[PASS] clean target removed axit_soni."

echo "[PASS] All tests passed."
