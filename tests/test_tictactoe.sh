#!/usr/bin/env bash
# Integration test for the Tic-Tac-Toe HTTP server.

set -e

cd "$(dirname "$0")/.."

echo "[TEST] Building tictactoe_server..."
make tictactoe_server

PORT=18080
./tictactoe_server "$PORT" &
SERVER_PID=$!

# Ensure server is killed on exit.
cleanup() {
    kill "$SERVER_PID" 2>/dev/null || true
    wait "$SERVER_PID" 2>/dev/null || true
}
trap cleanup EXIT

# Wait for server to become ready.
for i in {1..30}; do
    if curl -sf "http://127.0.0.1:$PORT/" >/dev/null 2>&1; then
        break
    fi
    sleep 0.1
done

echo "[TEST] Checking GET / returns HTML..."
response=$(curl -s "http://127.0.0.1:$PORT/")
if ! printf '%s' "$response" | grep -q "Tic-Tac-Toe"; then
    echo "[FAIL] GET / did not return expected HTML"
    exit 1
fi
echo "[PASS] GET / returns HTML."

echo "[TEST] Checking initial state..."
state=$(curl -s "http://127.0.0.1:$PORT/state")
echo "$state"
if ! printf '%s' "$state" | grep -q '"turn":"X"'; then
    echo "[FAIL] Initial state does not show X's turn"
    exit 1
fi
echo "[PASS] Initial state is correct."

echo "[TEST] Making a valid move as X..."
result=$(curl -s -X POST "http://127.0.0.1:$PORT/move?player=X&cell=0")
if ! printf '%s' "$result" | grep -q '"result":"ok"'; then
    echo "[FAIL] Valid X move was rejected: $result"
    exit 1
fi
echo "[PASS] X move accepted."

echo "[TEST] Checking occupied cell is rejected..."
result=$(curl -s -X POST "http://127.0.0.1:$PORT/move?player=O&cell=0")
if ! printf '%s' "$result" | grep -q '"result":"occupied"'; then
    echo "[FAIL] Occupied cell move was not rejected: $result"
    exit 1
fi
echo "[PASS] Occupied cell rejected."

echo "[TEST] Checking out-of-turn move is rejected..."
result=$(curl -s -X POST "http://127.0.0.1:$PORT/move?player=X&cell=1")
if ! printf '%s' "$result" | grep -q '"result":"not your turn"'; then
    echo "[FAIL] Out-of-turn move was not rejected: $result"
    exit 1
fi
echo "[PASS] Out-of-turn move rejected."

echo "[TEST] Completing a winning game for X (0,1,2)..."
curl -s -X POST "http://127.0.0.1:$PORT/move?player=O&cell=3" >/dev/null
curl -s -X POST "http://127.0.0.1:$PORT/move?player=X&cell=1" >/dev/null
curl -s -X POST "http://127.0.0.1:$PORT/move?player=O&cell=4" >/dev/null
curl -s -X POST "http://127.0.0.1:$PORT/move?player=X&cell=2" >/dev/null

state=$(curl -s "http://127.0.0.1:$PORT/state")
echo "$state"
if ! printf '%s' "$state" | grep -q '"winner":"X"'; then
    echo "[FAIL] X win was not detected: $state"
    exit 1
fi
echo "[PASS] X win detected."

echo "[TEST] Resetting game..."
curl -s -X POST "http://127.0.0.1:$PORT/reset" >/dev/null
state=$(curl -s "http://127.0.0.1:$PORT/state")
if ! printf '%s' "$state" | grep -q '"turn":"X"'; then
    echo "[FAIL] Reset did not clear the game: $state"
    exit 1
fi
echo "[PASS] Game reset."

echo "[TEST] Checking win/draw detection..."
curl -s -X POST "http://127.0.0.1:$PORT/move?player=X&cell=4" >/dev/null
curl -s -X POST "http://127.0.0.1:$PORT/move?player=O&cell=0" >/dev/null
curl -s -X POST "http://127.0.0.1:$PORT/move?player=X&cell=0" >/dev/null
if ! printf '%s' "$(curl -s -X POST "http://127.0.0.1:$PORT/move?player=X&cell=0")" | grep -q '"result":"occupied\|not your turn\|game over"'; then
    echo "[FAIL] Invalid move after win/reset accepted unexpectedly"
    exit 1
fi
echo "[PASS] Invalid move rejected."

echo "[TEST] Checking illegal move parameters..."
result=$(curl -s -X POST "http://127.0.0.1:$PORT/move?player=Z&cell=0")
if ! printf '%s' "$result" | grep -q '"error":"invalid parameters"'; then
    echo "[FAIL] Invalid player parameter was accepted: $result"
    exit 1
fi
result=$(curl -s -X POST "http://127.0.0.1:$PORT/move?player=X&cell=9")
if ! printf '%s' "$result" | grep -q '"error":"invalid parameters"'; then
    echo "[FAIL] Invalid cell parameter was accepted: $result"
    exit 1
fi
echo "[PASS] Illegal parameters rejected."

cleanup
trap - EXIT

echo "[PASS] All Tic-Tac-Toe server tests passed."
