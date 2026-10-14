# Test Report: KAN-18 -- LAN-playable Tic-Tac-Toe Web Server

**Branch:** `devin/KAN-18-tictactoe`
**Commit:** `e83833e4df0f555f9ba9aa87d4c7b6099486d7a7`
**Date:** 2026-10-14

## Test Command & Results

**Command:** `make test` (runs all 8 test suites including `tests/test_tictactoe.sh`)

| Suite | Result |
|---|---|
| test_hello_world.sh | PASS |
| test_bubble_sort.sh | PASS |
| test_linear_search.sh | PASS |
| test_gstreamer_app.sh | PASS |
| test_stack_queue_app.sh | PASS |
| test_string_concat.sh | PASS |
| test_axit_soni.sh | PASS |
| **test_tictactoe.sh** | **PASS** |

**Pass/Fail:** 8/8 suites passed, 0 failures
**Build warnings:** 0 (gcc -Wall -Wextra -std=c11)

## Acceptance Criteria Checklist

- [x] **Buildable target `tictactoe_server`** -- Makefile target exists, compiles with `gcc -Wall -Wextra -std=c11 -fPIC -lpthread`, binds to `INADDR_ANY` (0.0.0.0) so LAN devices can connect via host IP.
- [x] **Two browser clients see same game state** -- Server stores game state in global variables protected by a pthread mutex. All clients poll `GET /state` and receive identical JSON. Client JS polls every 500ms.
- [x] **Turn order enforcement, occupied cell rejection, win/draw detection** -- `handle_move()` enforces: game-over check, occupied-cell check, wrong-turn check. `check_winner()` tests all 8 win lines; `is_draw()` checks full board. Test script confirms: X move accepted, occupied rejected ("occupied"), out-of-turn rejected ("not your turn"), X win detected, invalid params rejected (400).
- [x] **Reset without restarting server** -- `POST /reset` calls `init_game()` clearing board/turn/winner/draw. Test confirms state resets to initial after reset. UI has "New Game" button.
- [x] **UI renders 3x3 board, current turn, win/draw result** -- Embedded HTML/CSS/JS: 3x3 grid via CSS grid, status div shows "Turn: X/O (you are X/O)", "Winner: X/O", or "Draw!".
- [x] **Repo conventions followed** -- Makefile target named `tictactoe_server`, compiled with required flags, bash test under `tests/test_tictactoe.sh`, `make test` invokes it, conventional commit with JIRA key prefix.

## New Tests Added

- `tests/test_tictactoe.sh` -- 9 test cases covering: HTML served, initial state, valid move, occupied cell rejection, out-of-turn rejection, win detection, game reset, invalid parameters (bad player, bad cell).

## Verdict

**PASS** -- All acceptance criteria met. Build is clean (zero warnings). Full test suite passes including the new tictactoe integration tests. No regressions in existing tests.
