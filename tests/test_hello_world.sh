#!/usr/bin/env bash
# Unit test for the Hello World program
# Verifies AC1: program compiles, prints "Hello World", and exits with code 0.

set -e

cd "$(dirname "$0")/.."

echo "[TEST] Building hello_world..."
make clean
make

echo "[TEST] Running hello_world..."
output=$(./hello_world)
exit_code=$?

echo "[TEST] Checking output..."
expected="Hello World"
if [ "$output" != "$expected" ]; then
    echo "[FAIL] Expected: '$expected', Got: '$output'"
    exit 1
fi

echo "[TEST] Checking exit code..."
if [ "$exit_code" -ne 0 ]; then
    echo "[FAIL] Expected exit code 0, Got: $exit_code"
    exit 1
fi

echo "[PASS] hello_world output and exit code are correct."

echo "[TEST] Checking clean target..."
make clean
if [ -f hello_world ]; then
    echo "[FAIL] clean target did not remove hello_world"
    exit 1
fi
echo "[PASS] clean target removed hello_world."

echo "[PASS] All tests passed."
