# SORREL SDD Technical Specification

This document provides the technical details for the SORREL Card Runner and its supporting artifacts.

## 1. Directory Structure

```text
sdd/
├── card_runner.cpp    # Source for the SORREL runner
├── sorrel_checkins.md   # Progress tracking
├── sorrel_checkouts.md  # Completion history
├── facts/             # Truth data
│   ├── *.facts        # Environment and Situation definitions
└── cards/             # SDD Classes (source code)
    └── *.cpp          # Logic implementations
```

## 2. Fact Files (.facts)

Fact files are structured by **Situations**. If no situation is specified, facts belong to the `Default` situation.

### Syntax
- **Situation Header:** `Situation: Name` (Plain text)
- **Fact Lines:** `[Level] [key] = [value]`
- **Levels:**
    - `Is`: Current system state.
    - `Needs`: Mandatory prerequisite for execution.
    - `Results`: Expected empirical observation after execution.
- **Comments:** Start with `#`.

### Example
```text
Situation: Default
Is platform = linux
Is network_available = true

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
- `// @Results key == value` - Asserts the expected output fact.
- `// @Situation name` - Targets a specific situation from fact files.

### Execution Model
The `card_runner` compiles each Class file into an executable. To run a specific card, the runner calls:
`./[ClassExecutable] [CardName]`

The class implementation is responsible for reading `argv[1]` and dispatching to the correct logic.

### Example Card Block
```cpp
// @Card: check_integrity
// @Situation Default
// @Is platform == linux
// @Results system_integrity == passed

#include <iostream>
int main(int argc, char** argv) {
    if (std::string(argv[1]) == "check_integrity") {
        std::cout << "system_integrity = passed" << std::endl;
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
    - Reports success or mismatch.

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
3. **Observation(s)** (empirical result or validation status).
4. **Notes** (optional: constraints, follow-ups, or corrections).

Together, checkins + checkouts provide a complete SDD loop: propose -> implement -> observe -> record.

