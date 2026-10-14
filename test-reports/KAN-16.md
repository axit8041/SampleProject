# Test Report: KAN-16 — factorial CLI program

Branch: `devin/KAN-16-factorial` (commit fdbd591)

## Test command

`make test` (aggregate suite, includes `bash tests/test_factorial.sh`)

- Result: PASS — all 7 test scripts passed, 0 failures
- Duration: ~2.4s (`real 0m2.352s`)

## Acceptance criteria

| Criterion | Result | Evidence |
|-----------|--------|----------|
| `make factorial` builds with `-Wall -Wextra -std=c11 -fPIC`; `make clean` removes it | Met | Build log shows `gcc -Wall -Wextra -std=c11 -fPIC -o factorial factorial.c`; after `make clean`, `ls factorial` fails |
| Recursive factorial, correct for 0!, 1!, 5!, 10! | Met | `factorial()` is recursive in `factorial.c`; outputs 1, 1, 120, 3628800 |
| Inputs 0–20 accepted, decimal value + newline; 20! = 2432902008176640000 no overflow | Met | `echo 20 \| ./factorial` → `2432902008176640000` (unsigned long long) |
| Negative, >20, non-numeric, missing, trailing input rejected with non-zero exit + stderr diagnostic | Met | `-1`→exit 1 "must be non-negative"; `21`→exit 1 "between 0 and 20"; `abc`→exit 1 "invalid input"; `5x`→exit 1 "invalid trailing input"; empty/EOF→exit 1 "missing input" |
| `tests/test_factorial.sh` in `make test`, covers build/valid/invalid/cleanup | Met | Script runs in `make test` output: builds, checks valid + invalid inputs, verifies clean removes binary; reported `[PASS] All tests passed.` |

## Coverage notes

- New test file `tests/test_factorial.sh` added and wired into the `test` target.
- Verified manually in addition to script: trailing whitespace after a valid integer is accepted (`7 \n` → 5040, exit 0), matching spec (only trailing *non-whitespace* rejected).

## Verdict

**PASS** — full suite green, all acceptance criteria verified.
