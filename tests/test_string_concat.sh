#!/usr/bin/env bash
# Unit tests for the String Concat program.
# Verifies AC1: program compiles, AC2: program uses hardcoded inputs, and
# AC3: program outputs the reversed concatenation "ilhoKtariV".

set -e

cd "$(dirname "$0")/.."

echo "[TEST] Building string_concat..."
make clean
make

echo "[TEST] Checking string_concat binary exists..."
if [ ! -f string_concat ]; then
    echo "[FAIL] string_concat binary was not built"
    exit 1
fi
echo "[PASS] string_concat binary exists."

echo "[TEST] Running string_concat with hardcoded input..."
output=$(./string_concat)
exit_code=$?

echo "[TEST] Checking exit code..."
if [ "$exit_code" -ne 0 ]; then
    echo "[FAIL] Expected exit code 0, Got: $exit_code"
    exit 1
fi
echo "[PASS] Exit code is 0."

echo "[TEST] Checking output..."
expected="ilhoKtariV"
if [ "$output" != "$expected" ]; then
    echo "[FAIL] Expected: '$expected', Got: '$output'"
    exit 1
fi
echo "[PASS] Output is '$expected'."

echo "[TEST] Checking clean target removes string_concat..."
make clean
if [ -f string_concat ]; then
    echo "[FAIL] clean target did not remove string_concat"
    exit 1
fi
echo "[PASS] clean target removed string_concat."

echo "[PASS] All string_concat tests passed."
