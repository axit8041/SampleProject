#!/usr/bin/env bash
# Unit tests for the Stack and Queue application and its reusable ADTs.
# Verifies AC1: stack operation, AC2: queue operation, AC3: application with main,
# AC4: Makefile builds shared/static libraries.

set -e

cd "$(dirname "$0")/.."

echo "[TEST] Building stack/queue targets and libraries..."
make clean
make

echo "[TEST] Checking stack_queue_app binary exists..."
if [ ! -f stack_queue_app ]; then
    echo "[FAIL] stack_queue_app binary was not built"
    exit 1
fi
echo "[PASS] stack_queue_app binary exists."

echo "[TEST] Checking static library exists..."
if [ ! -f libdatastructures.a ]; then
    echo "[FAIL] libdatastructures.a was not built"
    exit 1
fi
echo "[PASS] libdatastructures.a exists."

echo "[TEST] Checking shared library exists..."
if [ ! -f libdatastructures.so ]; then
    echo "[FAIL] libdatastructures.so was not built"
    exit 1
fi
echo "[PASS] libdatastructures.so exists."

echo "[TEST] Building direct stack/queue unit test binary..."
gcc -Wall -Wextra -std=c11 -I. -o tests/test_stack_queue tests/test_stack_queue.c libdatastructures.a
echo "[PASS] test_stack_queue binary built."

echo "[TEST] Running direct stack/queue unit tests..."
./tests/test_stack_queue
echo "[PASS] Direct stack/queue unit tests passed."

run_app_test() {
    local name="$1"
    local input="$2"
    local expected="$3"

    echo "[TEST] $name (input: '$input')"
    local output
    local exit_code
    output=$(printf '%s' "$input" | ./stack_queue_app)
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

run_app_test "Odd values stack, even values queue" "1 2 3 4 5 6" $'Stack: top=5 size=3\nQueue: front=2 size=3'
run_app_test "Only odd values" "9 7 5" $'Stack: top=5 size=3\nQueue: empty'
run_app_test "Only even values" "2 4 6" $'Stack: empty\nQueue: front=2 size=3'
run_app_test "Single odd value" "11" $'Stack: top=11 size=1\nQueue: empty'
run_app_test "Single even value" "22" $'Stack: empty\nQueue: front=22 size=1'
run_app_test "Negative odd values stack" "-1 -2 -3" $'Stack: top=-3 size=2\nQueue: front=-2 size=1'

echo "[TEST] Empty input..."
output=$(printf '' | ./stack_queue_app)
exit_code=$?
if [ "$exit_code" -ne 0 ]; then
    echo "[FAIL] Empty input: expected exit code 0, got $exit_code"
    exit 1
fi
expected=$'Stack: empty\nQueue: empty'
if [ "$output" != "$expected" ]; then
    echo "[FAIL] Empty input: expected '$expected', got '$output'"
    exit 1
fi
echo "[PASS] Empty input."

echo "[TEST] Checking clean target removes stack_queue_app and libraries..."
make clean
if [ -f stack_queue_app ]; then
    echo "[FAIL] clean target did not remove stack_queue_app"
    exit 1
fi
if [ -f libdatastructures.a ]; then
    echo "[FAIL] clean target did not remove libdatastructures.a"
    exit 1
fi
if [ -f libdatastructures.so ]; then
    echo "[FAIL] clean target did not remove libdatastructures.so"
    exit 1
fi
echo "[PASS] clean target removed stack/queue artifacts."

echo "[PASS] All stack_queue_app tests passed."
