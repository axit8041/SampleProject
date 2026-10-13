#!/usr/bin/env bash
# Unit tests for the Palindrome program.
# Verifies the program compiles, prints "Palindrome"/"Not Palindrome" for
# numeric, alphabetic (case-insensitive, punctuation-ignoring), mixed,
# single-character, empty/whitespace, and non-palindrome inputs, and that
# `make clean` removes the binary.

set -e

cd "$(dirname "$0")/.."

echo "[TEST] Building palindrome..."
make palindrome

echo "[TEST] Checking palindrome binary exists..."
if [ ! -f palindrome ]; then
    echo "[FAIL] palindrome binary was not built"
    exit 1
fi
echo "[PASS] palindrome binary exists."

check_output() {
    local input="$1"
    local expected="$2"
    local output
    output=$(printf '%s' "$input" | ./palindrome) || true
    if [ "$output" != "$expected" ]; then
        echo "[FAIL] Input '$input': expected '$expected', got '$output'"
        exit 1
    fi
    echo "[PASS] Input '$input' -> '$output'."
}

check_exit_code() {
    local input="$1"
    local expected="$2"
    local code=0
    printf '%s' "$input" | ./palindrome > /dev/null || code=$?
    if [ "$code" -ne "$expected" ]; then
        echo "[FAIL] Input '$input': expected exit $expected, got $code"
        exit 1
    fi
    echo "[PASS] Input '$input' exits with $code."
}

echo "[TEST] Numeric palindromes..."
check_output "121" "Palindrome"
check_output "12321" "Palindrome"

echo "[TEST] Alphabetic palindromes (case-insensitive, ignores non-alphanumeric)..."
check_output "Madam" "Palindrome"
check_output "A man a plan a canal Panama" "Palindrome"

echo "[TEST] Mixed alphanumeric palindromes..."
check_output "1a2A1" "Palindrome"
check_output "A1b2B1a" "Palindrome"

echo "[TEST] Single-character input..."
check_output "5" "Palindrome"
check_output "z" "Palindrome"

echo "[TEST] Whitespace-trimmed input..."
check_output "  121  " "Palindrome"

echo "[TEST] Non-palindromes..."
check_output "123" "Not Palindrome"
check_output "Hello" "Not Palindrome"
check_output "ab1" "Not Palindrome"

echo "[TEST] Empty and whitespace-only input..."
check_output "" "Not Palindrome"
check_output "   " "Not Palindrome"
check_exit_code "" 1
check_exit_code "121" 0

echo "[TEST] Checking clean target removes palindrome..."
make clean
if [ -f palindrome ]; then
    echo "[FAIL] clean target did not remove palindrome"
    exit 1
fi
echo "[PASS] clean target removed palindrome."

echo "[PASS] All palindrome tests passed."
