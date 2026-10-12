#!/usr/bin/env bash
# Unit tests for the Linear Search program.
# Verifies AC1: program compiles, AC2: program reads an array and correctly
# searches it regardless of whether the array is sorted or unsorted.

set -e

cd "$(dirname "$0")/.."

echo "[TEST] Building linear_search..."
make clean
make

echo "[TEST] Checking linear_search binary exists..."
if [ ! -f linear_search ]; then
    echo "[FAIL] linear_search binary was not built"
    exit 1
fi
echo "[PASS] linear_search binary exists."

run_test() {
    local name="$1"
    local input="$2"
    local expected="$3"

    echo "[TEST] $name (input: '$input')"
    local output
    local exit_code
    output=$(printf '%s' "$input" | ./linear_search)
    exit_code=$?

    if [ "$exit_code" -ne 0 ]; then
        echo "[FAIL] $name: expected exit code 0, got $exit_code"
        exit 1
    fi

    if [ "$output" != "$expected" ]; then
        echo "[FAIL] $name: expected '$expected', got '$output'"
        exit 1
    fi

    echo "[PASS] $name."
}

run_test "Found in unsorted array" "5 2 8 1 9 8" "Found at index 2"
run_test "Found in sorted array" "1 2 3 4 5 4" "Found at index 3"
run_test "Found at first index" "7 3 5 7" "Found at index 0"
run_test "Found at last index" "1 2 3 4 5 5" "Found at index 4"
run_test "Not found" "10 20 30 40 25" "Not Found"
run_test "Single element found" "42 42" "Found at index 0"
run_test "Single element not found" "42 43" "Not Found"
run_test "Duplicate returns first index" "4 4 4 4 4" "Found at index 0"
run_test "Negative numbers" "-3 -10 5 0 -10" "Found at index 1"

echo "[TEST] Empty input..."
output=$(printf '' | ./linear_search)
exit_code=$?
if [ "$exit_code" -ne 0 ]; then
    echo "[FAIL] Empty input: expected exit code 0, got $exit_code"
    exit 1
fi
if [ "$output" != "Not Found" ]; then
    echo "[FAIL] Empty input: expected 'Not Found', got '$output'"
    exit 1
fi
echo "[PASS] Empty input."

echo "[TEST] Checking clean target removes linear_search..."
make clean
if [ -f linear_search ]; then
    echo "[FAIL] clean target did not remove linear_search"
    exit 1
fi
echo "[PASS] clean target removed linear_search."

echo "[PASS] All linear_search tests passed."
