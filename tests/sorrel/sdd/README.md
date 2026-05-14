# SORREL SDD Technical Specification

This document provides the technical details for the SORREL Card Runner and its supporting artifacts.

## 1. Directory Structure

```text
sdd/
├── card_runner.cpp    # Source for the SORREL runner
├── sorrel_checkins.md   # Progress tracking
├── sorrel_checkouts.md  # Completion history
├── facts/             # Truth data
│   ├── *.facts        # Environment state and numeric evidence definitions
└── cards/             # SDD Classes (source code)
    └── *.cpp          # Logic implementations
```

## 2. Fact Files (.facts)

Fact files are structured by **Situations**. If no situation is specified, facts belong to the `Default` situation.

### Syntax
- **Situation Header:** `Situation: Name` (Plain text)
- **Fact Lines:** `[Level] [key] = [value]`; `Results` facts should be numeric evidence.
- **Levels:**
    - `Is`: Current system state.
    - `Needs`: Mandatory prerequisite for execution.
    - `Results`: Expected empirical measurement after execution. Do not use pass/fail, true/false, or success strings for result evidence.
- **Comments:** Start with `#`.

### Example
```text
Situation: Default
Is platform = linux
Is network_available_count = 1

Situation: LowResources
Is platform = linux
Is disk_space = low
```

## 3. Card Classes (.cpp)

A **Class** is a single C++ file containing one or more **Logical Cards**.

### Decorators
Logical cards are defined by specific comment decorators:
- `// @Card: name` - Marks the start of a logical card block.
- `// @Is key == value` - Validates system state before execution.
- `// @Needs key == value` - Checks prerequisites.
- `// @Results key == number` - Checks the expected numeric output measurement.
- `// @Situation name` - Targets a specific situation from fact files.

### Execution Model
The current `card_runner` does not compile source files. It discovers prebuilt executables whose filenames match `.cpp` sources and calls the selected executable with the logical card name:
`./[ClassExecutable] [CardName]`

The class implementation is responsible for reading `argv[1]` and dispatching to the correct logic.

### Example Card Block
```cpp
// @Card: check_integrity
// @Situation Default
// @Is platform == linux
// @Results system_integrity_score == 1

#include <iostream>
int main(int argc, char** argv) {
    if (std::string(argv[1]) == "check_integrity") {
        std::cout << "system_integrity_score = 1" << std::endl;
    }
    return 0;
}
```

## 4. Runner Workflow

1. **Parse Facts:** Loads situation-aware facts into memory.
2. **Discover Cards:** Scans `cards/` directory for `.cpp` files and parses decorators.
3. **Execute Loop:**
    - Selects the target `Situation` (defaults to `Default`).
    - Evaluates `@Is` and `@Needs` against the chosen situation.
    - Executes the card if conditions are met.
    - Parses stdout into `observed_results`.
    - Validates observed results against `@Results`.
    - Reports observed numbers and any mismatch against `@Results`; the report is evidence, not a pass/fail replacement.

## 5. SDD Checkins vs Checkouts

SORREL SDD uses two complementary logs to make iterative development auditable and reproducible:

- **`sorrel_checkins.md` (planned work):** Records pending or proposed sips/tasks that still need implementation.
- **`sorrel_checkouts.md` (completed work):** Records completed sips/tasks, what changed, and the resulting observation(s).

### Why checkouts matter
- They create a durable execution history for SDD evolution over time.
- They capture evidence-based outcomes (what was actually verified), not just intent.
- They provide fast onboarding context for new contributors by linking each increment to artifacts and observations.

### Expected checkout entry content
A checkout entry should minimally include:
1. **Sip/Capability name** (what was implemented).
2. **Artifacts touched** (files or directories created/modified).
3. **Observation(s)** (numeric empirical result, such as counts, byte sizes, durations, or exit codes).
4. **Notes** (optional: constraints, follow-ups, or corrections).

Together, checkins + checkouts provide a complete SDD loop: propose -> implement -> observe -> record.

