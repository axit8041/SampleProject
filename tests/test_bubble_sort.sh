#!/usr/bin/env bash
# Unit tests for the Bubble Sort program.
# Verifies AC1: program compiles, AC3: program reads array input and sorts it.

set -e

cd "$(dirname "$0")/.."

echo "[TEST] Building bubble_sort..."
make clean
make

echo "[TEST] Checking bubble_sort binary exists..."
if [ ! -f bubble_sort ]; then
    echo "[FAIL] bubble_sort binary was not built"
    exit 1
fi
echo "[PASS] bubble_sort binary exists."

run_test() {
    local name="$1"
    local input="$2"
    local expected="$3"

    echo "[TEST] $name (input: '$input')"
    output=$(printf '%s' "$input" | ./bubble_sort)
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

run_test "Normal unsorted input" "5 2 8 1 9" "1 2 5 8 9"
run_test "Already sorted input" "1 2 3 4 5" "1 2 3 4 5"
run_test "Reverse sorted input" "9 7 5 3 1" "1 3 5 7 9"
run_test "Single element input" "42" "42"
run_test "All equal elements" "4 4 4 4" "4 4 4 4"
run_test "Negative numbers" "-3 5 -10 0 2" "-10 -3 0 2 5"

echo "[TEST] Empty input..."
output=$(printf '' | ./bubble_sort)
exit_code=$?
if [ "$exit_code" -ne 0 ]; then
    echo "[FAIL] Empty input: expected exit code 0, got $exit_code"
    exit 1
fi
if [ "$output" != "" ]; then
    echo "[FAIL] Empty input: expected empty output, got '$output'"
    exit 1
fi
echo "[PASS] Empty input."

echo "[TEST] Checking clean target removes bubble_sort..."
make clean
if [ -f bubble_sort ]; then
    echo "[FAIL] clean target did not remove bubble_sort"
    exit 1
fi
echo "[PASS] clean target removed bubble_sort."

echo "[PASS] All bubble_sort tests passed."
