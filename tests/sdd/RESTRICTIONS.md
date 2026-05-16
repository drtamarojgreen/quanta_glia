# SDD Restrictions for QuantaGlia Testing System

## 1. Structural Restrictions
- **FORBID raw_pointers**: All resource management must use smart pointers (e.g., `std::unique_ptr`) or RAII patterns.
- **FORBID empty_catch_blocks**: All `catch` blocks must either log the error or handle it.
- **LIMIT max_classes_per_file**: 1 (Each card should belong to a domain-specific class file).
- **LIMIT max_lines_per_card**: 150 lines.

## 2. Validation Restrictions
- **FORBID binary_flags**: All cards are strictly prohibited from outputting boolean-like flags (0/1, true/false) as primary results.
- **REQUIRE empirical_metrics**: All cards must output real-world measurements such as byte sizes, row counts, parsed item counts, or numeric scores.
- **REQUIRE temp_directory**: All temporary test artifacts (files, repositories, logs) must be strictly created within `tests/temp/`.

## 3. Tool Restrictions
- **ALLOWED_TOOLS**: `g++`, `cmake`, `python3`, `git`.
- **FORBID unknown_libraries**: Do not link against libraries not explicitly defined in `CMakeLists.txt`.

## 4. Documentation Restrictions
- **REQUIRE lifecycle_tracking**: All work must be recorded in `sorrel_checkins.md` and `sorrel_checkouts.md`.
- **FORBID todos_in_code**: All deferred work must be tracked in `sorrel_checkins.md`.
