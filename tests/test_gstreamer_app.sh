#!/usr/bin/env bash
# Unit test for the GStreamer two-plugin application.
# Verifies AC1: program compiles, AC2: pipeline uses videotestsrc and fakesink,
# AC3: pipeline runs to EOS and exits with code 0.

set -e

cd "$(dirname "$0")/.."

echo "[TEST] Building gstreamer_app..."
make clean
make

echo "[TEST] Checking gstreamer_app binary exists..."
if [ ! -x ./gstreamer_app ]; then
    echo "[FAIL] gstreamer_app binary was not produced"
    exit 1
fi
echo "[PASS] gstreamer_app binary exists."

echo "[TEST] Running gstreamer_app..."
output=$(./gstreamer_app)
exit_code=$?

echo "[TEST] Checking exit code..."
if [ "$exit_code" -ne 0 ]; then
    echo "[FAIL] Expected exit code 0, Got: $exit_code"
    exit 1
fi
echo "[PASS] gstreamer_app exited with code 0."

echo "[TEST] Checking output..."
expected="GStreamer pipeline completed: videotestsrc -> fakesink"
if [ "$output" != "$expected" ]; then
    echo "[FAIL] Expected: '$expected', Got: '$output'"
    exit 1
fi
echo "[PASS] gstreamer_app output confirms videotestsrc -> fakesink pipeline completed."

echo "[TEST] Checking clean target..."
make clean
if [ -f gstreamer_app ]; then
    echo "[FAIL] clean target did not remove gstreamer_app"
    exit 1
fi
echo "[PASS] clean target removed gstreamer_app."

echo "[PASS] All tests passed."
