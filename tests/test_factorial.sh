#!/usr/bin/env bash
# Unit tests for the Factorial program.
# Verifies compilation, valid factorial values, invalid-input rejection,
# and clean-up behavior.

set -e

cd "$(dirname "$0")/.."

echo "[TEST] Building factorial..."
make clean
make factorial

echo "[TEST] Checking factorial binary exists..."
if [ ! -f factorial ]; then
    echo "[FAIL] factorial binary was not built"
    exit 1
fi
echo "[PASS] factorial binary exists."

run_test() {
    local name="$1"
    local input="$2"
    local expected="$3"

    echo "[TEST] $name (input: '$input')"
    local output
    local exit_code
    output=$(printf '%s' "$input" | ./factorial)
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

run_test "Factorial of 0" "0" "1"
run_test "Factorial of 1" "1" "1"
run_test "Factorial of 5" "5" "120"
run_test "Factorial of 10" "10" "3628800"
run_test "Factorial of 20" "20" "2432902008176640000"

run_invalid_test() {
    local name="$1"
    local input="$2"

    echo "[TEST] $name (input: '$input')"
    local output
    local exit_code

    # The program is expected to exit non-zero for these cases, so disable
    # errexit while running it inside a command substitution.
    set +e
    output=$(printf '%s' "$input" | ./factorial 2>&1)
    exit_code=$?
    set -e

    if [ "$exit_code" -eq 0 ]; then
        echo "[FAIL] $name: expected non-zero exit code, got $exit_code"
        exit 1
    fi

    if [ -z "$output" ]; then
        echo "[FAIL] $name: expected diagnostic on stderr, got no output"
        exit 1
    fi

    echo "[PASS] $name."
}

run_invalid_test "Negative value" "-1"
run_invalid_test "Value greater than 20" "21"
run_invalid_test "Non-numeric input" "abc"
run_invalid_test "Trailing non-whitespace input" "5abc"
run_invalid_test "Missing input" ""

echo "[TEST] Checking clean target removes factorial..."
make clean
if [ -f factorial ]; then
    echo "[FAIL] clean target did not remove factorial"
    exit 1
fi
echo "[PASS] clean target removed factorial."

echo "[PASS] All factorial tests passed."
