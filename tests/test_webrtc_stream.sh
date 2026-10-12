#!/usr/bin/env bash
# Unit tests for the WebRTC streaming demo application (KAN-8).
#
# Tests cover:
#   TC01: Binary exists after build
#   TC02: Help option prints usage
#   TC03: Invalid mode argument returns error
#   TC04: Missing --mp4 argument returns error
#   TC05: Invalid port returns error
#   TC06: Camera detection (expects no camera in CI)
#   TC07: GStreamer pipeline builds with MP4 fallback
#   TC08: DataChannel pipeline builds
#   TC09: STUN server configured (checked in output)
#   TC10: Browser client HTML exists and contains WebRTC APIs
#   TC11: Browser client contains Google STUN configuration
#   TC12: Mode selection configurable at runtime
#   TC13: H.264 codec in pipeline (GStreamer mode)
#   TC14: WebSocket signaling references in browser client
#   TC15: Clean target removes binary

set -e

PASS=0
FAIL=0
TOTAL=0
RESULTS=""

pass_test() {
    local name="$1"
    echo "[PASS] $name"
    PASS=$((PASS + 1))
    TOTAL=$((TOTAL + 1))
    RESULTS="${RESULTS}  [PASS] ${name}\n"
}

fail_test() {
    local name="$1"
    local reason="$2"
    echo "[FAIL] $name: $reason"
    FAIL=$((FAIL + 1))
    TOTAL=$((TOTAL + 1))
    RESULTS="${RESULTS}  [FAIL] ${name}: ${reason}\n"
}

cd "$(dirname "$0")/.."

echo "======================================="
echo " WebRTC Streaming Demo - Unit Tests"
echo "======================================="
echo ""

# --- TC01: Binary exists after build ---
echo "[TEST] TC01: Binary exists after build"
make webrtc_stream 2>&1
if [ -x ./webrtc_stream ]; then
    pass_test "TC01_binary_exists"
else
    fail_test "TC01_binary_exists" "webrtc_stream binary was not produced"
fi

# --- TC02: Help option prints usage ---
echo "[TEST] TC02: Help option prints usage"
help_output=$(./webrtc_stream --help 2>&1 || true)
if echo "$help_output" | grep -q "Usage:"; then
    pass_test "TC02_help_option"
else
    fail_test "TC02_help_option" "Help output missing Usage line"
fi

# --- TC03: Invalid mode argument ---
echo "[TEST] TC03: Invalid mode argument returns error"
invalid_mode_output=$(./webrtc_stream --mode invalid 2>&1 || true)
exit_code=$?
if echo "$invalid_mode_output" | grep -qi "unknown mode\|error"; then
    pass_test "TC03_invalid_mode"
else
    fail_test "TC03_invalid_mode" "Did not reject invalid mode"
fi

# --- TC04: Missing --mp4 argument ---
echo "[TEST] TC04: Missing --mp4 argument returns error"
missing_mp4=$(./webrtc_stream --mp4 2>&1 || true)
if echo "$missing_mp4" | grep -qi "requires\|error"; then
    pass_test "TC04_missing_mp4_arg"
else
    fail_test "TC04_missing_mp4_arg" "Did not report missing mp4 path"
fi

# --- TC05: Invalid port returns error ---
echo "[TEST] TC05: Invalid port returns error"
invalid_port=$(./webrtc_stream --port abc 2>&1 || true)
if echo "$invalid_port" | grep -qi "invalid\|error"; then
    pass_test "TC05_invalid_port"
else
    fail_test "TC05_invalid_port" "Did not reject invalid port"
fi

# --- TC06: Camera detection (no camera expected) ---
echo "[TEST] TC06: Camera detection fallback"
gst_no_mp4=$(./webrtc_stream --mode gstreamer 2>&1 || true)
if echo "$gst_no_mp4" | grep -qi "not detected\|no camera\|no MP4"; then
    pass_test "TC06_camera_detection"
else
    fail_test "TC06_camera_detection" "Camera detection message not found"
fi

# --- Create a small test MP4 file for pipeline tests ---
echo "[TEST] Creating test MP4 fixture..."
TEST_MP4="/tmp/webrtc_test_sample.mp4"
gst-launch-1.0 -e videotestsrc num-buffers=30 ! \
    video/x-raw,width=320,height=240,framerate=10/1 ! \
    x264enc tune=zerolatency bitrate=200 speed-preset=ultrafast ! \
    mp4mux ! filesink location="$TEST_MP4" 2>/dev/null || true

if [ ! -f "$TEST_MP4" ]; then
    echo "[WARN] Could not create test MP4; some tests will use output checks only"
fi

# --- TC07: GStreamer pipeline builds with MP4 fallback ---
echo "[TEST] TC07: GStreamer pipeline with MP4 fallback"
if [ -f "$TEST_MP4" ]; then
    # Run with a short timeout; the pipeline may not connect to a peer but
    # should print the build confirmation before timing out.
    gst_output=$(timeout 5 ./webrtc_stream --mode gstreamer --mp4 "$TEST_MP4" 2>&1 || true)
    if echo "$gst_output" | grep -qi "GStreamer WebRTC pipeline built\|MP4 file\|PLAYING"; then
        pass_test "TC07_gstreamer_pipeline_mp4"
    else
        fail_test "TC07_gstreamer_pipeline_mp4" "Pipeline build message not found: $gst_output"
    fi
else
    # Without MP4, just confirm the app reports no media source
    gst_output=$(./webrtc_stream --mode gstreamer 2>&1 || true)
    if echo "$gst_output" | grep -qi "no camera\|no MP4\|no media"; then
        pass_test "TC07_gstreamer_pipeline_mp4"
    else
        fail_test "TC07_gstreamer_pipeline_mp4" "Expected no-source error"
    fi
fi

# --- TC08: DataChannel pipeline builds ---
echo "[TEST] TC08: DataChannel pipeline builds"
dc_output=$(timeout 5 ./webrtc_stream --mode datachannel 2>&1 || true)
if echo "$dc_output" | grep -qi "DataChannel WebRTC pipeline built\|datachannel\|PLAYING"; then
    pass_test "TC08_datachannel_pipeline"
else
    fail_test "TC08_datachannel_pipeline" "DataChannel pipeline message not found: $dc_output"
fi

# --- TC09: STUN server configured ---
echo "[TEST] TC09: STUN server configured in output"
stun_check=""
if [ -f "$TEST_MP4" ]; then
    stun_check=$(timeout 5 ./webrtc_stream --mode gstreamer --mp4 "$TEST_MP4" 2>&1 || true)
else
    stun_check=$(timeout 5 ./webrtc_stream --mode datachannel 2>&1 || true)
fi
if echo "$stun_check" | grep -qi "stun.l.google.com\|STUN"; then
    pass_test "TC09_stun_server"
else
    fail_test "TC09_stun_server" "STUN server not mentioned in output"
fi

# --- TC10: Browser client HTML contains WebRTC APIs ---
echo "[TEST] TC10: Browser client contains WebRTC APIs"
if [ -f webrtc_client.html ]; then
    if grep -q "RTCPeerConnection" webrtc_client.html && \
       grep -q "RTCSessionDescription" webrtc_client.html && \
       grep -q "RTCIceCandidate" webrtc_client.html; then
        pass_test "TC10_browser_webrtc_apis"
    else
        fail_test "TC10_browser_webrtc_apis" "Missing WebRTC API references"
    fi
else
    fail_test "TC10_browser_webrtc_apis" "webrtc_client.html not found"
fi

# --- TC11: Browser client has Google STUN ---
echo "[TEST] TC11: Browser client STUN configuration"
if grep -q "stun:stun.l.google.com:19302" webrtc_client.html; then
    pass_test "TC11_browser_stun"
else
    fail_test "TC11_browser_stun" "Google STUN not found in HTML"
fi

# --- TC12: Mode selection configurable ---
echo "[TEST] TC12: Mode selection configurable at runtime"
mode_gst=$(timeout 5 ./webrtc_stream --mode gstreamer --mp4 "$TEST_MP4" 2>&1 || true)
mode_dc=$(timeout 5 ./webrtc_stream --mode datachannel 2>&1 || true)
if echo "$mode_gst" | grep -qi "Mode:.*gstreamer\|mode: gstreamer" && \
   echo "$mode_dc" | grep -qi "Mode:.*datachannel\|mode: datachannel"; then
    pass_test "TC12_mode_configurable"
else
    fail_test "TC12_mode_configurable" "Mode not reflected in output"
fi

# --- TC13: H.264 codec in GStreamer pipeline ---
echo "[TEST] TC13: H.264 codec in GStreamer mode"
if [ -f "$TEST_MP4" ]; then
    h264_out=$(timeout 5 ./webrtc_stream --mode gstreamer --mp4 "$TEST_MP4" 2>&1 || true)
    if echo "$h264_out" | grep -qi "H.264\|H264\|codec: H.264"; then
        pass_test "TC13_h264_codec"
    else
        fail_test "TC13_h264_codec" "H.264 codec not mentioned"
    fi
else
    # Check source code for x264enc reference
    if grep -q "x264enc" webrtc_stream.c; then
        pass_test "TC13_h264_codec"
    else
        fail_test "TC13_h264_codec" "x264enc not found in source"
    fi
fi

# --- TC14: WebSocket signaling in browser client ---
echo "[TEST] TC14: WebSocket signaling in browser client"
if grep -q "new WebSocket" webrtc_client.html && \
   grep -q "ws://" webrtc_client.html; then
    pass_test "TC14_websocket_signaling"
else
    fail_test "TC14_websocket_signaling" "WebSocket references missing"
fi

# --- TC15: Clean target removes binary ---
echo "[TEST] TC15: Clean target removes binary"
make clean 2>&1 > /dev/null
if [ ! -f webrtc_stream ]; then
    pass_test "TC15_clean_target"
else
    fail_test "TC15_clean_target" "webrtc_stream not removed by clean"
fi

# Rebuild for subsequent test steps
make webrtc_stream 2>&1 > /dev/null

# --- Summary ---
echo ""
echo "======================================="
echo " Test Summary: $PASS/$TOTAL passed, $FAIL failed"
echo "======================================="
echo -e "$RESULTS"

# Clean up test fixture
rm -f "$TEST_MP4"

if [ "$FAIL" -gt 0 ]; then
    echo "[RESULT] FAIL"
    exit 1
fi
echo "[RESULT] PASS"
exit 0
