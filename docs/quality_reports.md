# Quality Reports

This page tracks the unit-test and Valgrind results for the SampleProject
programs. Tables are updated each time the implementation is rebuilt and
re-verified.

## Unit Test Report

| Date       | Target                  | Test Case                                      | Result |
|------------|-------------------------|------------------------------------------------|--------|
| 2026-10-12 | stack_queue_app         | Build all targets and libraries                | PASS   |
| 2026-10-12 | stack_queue_app         | Application integration tests                  | PASS   |
| 2026-10-12 | tests/test_stack_queue  | Direct stack/queue API tests                   | PASS   |
| 2026-10-12 | string_concat           | Build string_concat target                     | PASS   |
| 2026-10-12 | string_concat           | Hardcoded input produces reversed output       | PASS   |
| 2026-10-12 | string_concat           | Clean target removes binary                    | PASS   |

## Valgrind Report

| Date       | Command                                                       | Errors | Leak Summary | Result |
|------------|---------------------------------------------------------------|--------|--------------|--------|
| 2026-10-12 | valgrind --leak-check=full ./stack_queue_app &lt; input.txt   | 0      | 0 bytes lost   | PASS   |
| 2026-10-12 | valgrind --leak-check=full ./tests/test_stack_queue          | 0      | 0 bytes lost   | PASS   |
| 2026-10-12 | valgrind --leak-check=full ./string_concat                   | 0      | 0 bytes lost   | PASS   |

## Architecture Diagram

The architecture diagram below illustrates the relationship between the standalone
programs, the reusable data-structure library, the main library consumer, and
the test suite.

\image html architecture.svg "SampleProject Architecture"
