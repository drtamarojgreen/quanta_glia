# SDD Restrictions for QuantaGlia Testing System

## 1. Structural Restrictions
- **FORBID raw_pointers**: All resource management must use smart pointers (e.g., `std::unique_ptr`) or RAII patterns.
- **FORBID empty_catch_blocks**: All `catch` blocks must either log the error or handle it.
- **LIMIT max_classes_per_file**: 1 (Each card should belong to a domain-specific class file).
- **LIMIT max_lines_per_card**: 100 lines.

## 2. Validation Restrictions
- **REQUIRE numeric_results**: All cards must output empirical measurements as numbers (e.g., `operational = 1`), not booleans or strings.
- **REQUIRE result_keys**: All output keys must match the pattern `[context]_operational`.

## 3. Tool Restrictions
- **ALLOWED_TOOLS**: `g++`, `cmake`, `python3`, `git`.
- **FORBID unknown_libraries**: Do not link against libraries not explicitly defined in `CMakeLists.txt`.

## 4. Documentation Restrictions
- **REQUIRE lifecycle_tracking**: All work must be recorded in `sorrel_checkins.md` and `sorrel_checkouts.md`.
- **FORBID todos_in_code**: All deferred work must be tracked in `sorrel_checkins.md`.
