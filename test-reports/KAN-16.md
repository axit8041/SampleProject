# Test Report: KAN-16

## Branch Under Test
`devin/KAN-16-standalone-recursive-factorial` (commit `9552326cb7b89188c6383f9464d0ace1f5322a3a`)

## Test Environment
- Repository: `/home/einfochips/extension/SampleProject/`
- Test command: `make test`
- Shell: bash

## Full Test Suite Results

**Command run:**
```bash
cd /home/einfochips/extension/SampleProject/ && time make test
```

**Outcome:** PASS

**Summary:**
- All six test scripts executed successfully.
- `tests/test_factorial.sh` was included in the aggregate `make test` workflow and passed.
- Total wall-clock duration: ~2.38 seconds (real 0m2.383s, user 0m1.697s, sys 0m0.687s).
- Pass/fail counts: 6 scripts passed / 0 failed.

**Scripts executed:**
1. `tests/test_hello_world.sh` — PASS
2. `tests/test_bubble_sort.sh` — PASS
3. `tests/test_linear_search.sh` — PASS
4. `tests/test_gstreamer_app.sh` — PASS
5. `tests/test_stack_queue_app.sh` — PASS
6. `tests/test_factorial.sh` — PASS

## Acceptance Criteria Checklist

| # | Criterion | Status | Evidence |
|---|-----------|--------|----------|
| 1 | `make factorial` builds a `factorial` executable from `factorial.c` using the repository's C11 warning flags, and `make clean` removes it. | **MET** | `Makefile` line 10 adds `factorial` to `TARGETS`; line 32-33 compiles with `$(CC) $(CFLAGS)` (`gcc -Wall -Wextra -std=c11 -fPIC`). `make clean` (line 59-60) removes `$(TARGETS)`, including `factorial`. `test_factorial.sh` and manual run both confirmed build and removal. |
| 2 | The factorial calculation is implemented recursively and produces correct results for `0!`, `1!`, and representative positive values such as `5!` and `10!`. | **MET** | `factorial.c` lines 21-28 implement `factorial(unsigned int n)` recursively (base case `n==0` returns 1, otherwise `n * factorial(n - 1)`). Verified outputs: `0! -> 1`, `1! -> 1`, `5! -> 120`, `10! -> 3628800`. |
| 3 | Integer inputs from 0 through 20 are accepted and produce only the decimal factorial value followed by a newline; in particular, input `20` produces `2432902008176640000` without overflow. | **MET** | `read_input` validates `*value <= MAX_FACTORIAL_INPUT` (20) and uses `unsigned long long`. Verified `printf '20\n' | ./factorial` outputs exactly `2432902008176640000` followed by a newline. `5!` and `10!` also produced exact single-line output. |
| 4 | Negative values, values greater than 20, non-numeric input, missing input, and trailing non-whitespace input are rejected with a non-zero exit status and a clear diagnostic on standard error. | **MET** | Manual verification: `-1` -> "Error: input out of range" exit 1; `21` -> "Error: input out of range" exit 1; `abc` -> "Error: invalid input" exit 1; `5abc` -> "Error: invalid input" exit 1; missing input -> "Error: missing input" exit 1. `test_factorial.sh` covers all five rejection cases. |
| 5 | A factorial test script is included in the aggregate `make test` workflow and verifies compilation, valid values, invalid-input behavior, and cleanup. | **MET** | `tests/test_factorial.sh` exists, is executable, and is invoked from `Makefile` line 57. It verifies `make factorial` compilation, valid values (0, 1, 5, 10, 20), invalid inputs (negative, >20, non-numeric, trailing, missing), and `make clean` cleanup. |

## New Tests / Coverage

- **New test script added:** `tests/test_factorial.sh` (lines 1-93)
  - Builds `factorial` with `make factorial`.
  - Verifies the binary exists.
  - Runs five valid-input cases: `0`, `1`, `5`, `10`, `20`.
  - Runs five invalid-input cases: `-1`, `21`, `abc`, `5abc`, empty input.
  - Verifies `make clean` removes the `factorial` binary.
- **Integration:** The script is invoked by the existing `make test` target (`Makefile` line 57), so the new functionality is exercised by the full project test suite.

## Missing Coverage / Notes

- No additional missing coverage identified for the scoped task. The implementation correctly stays within the stated scope (non-negative integers 0-20, no arbitrary-precision arithmetic, no library/API).
- The `test_factorial.sh` script uses `set -e`; invalid-input tests temporarily disable `errexit` around the command substitution to capture the expected non-zero exit code, which is the correct pattern.

## Verdict

**PASS**

All acceptance criteria for KAN-16 are satisfied. The standalone recursive factorial program builds with the repository's C11 warning flags, handles the required valid and invalid inputs correctly, and is integrated into the aggregate `make test` workflow with passing automated tests.
