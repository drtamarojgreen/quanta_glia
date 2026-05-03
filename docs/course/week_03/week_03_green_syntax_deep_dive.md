# Week 3: Green Syntax Technical Manual

## 1. The Grammar of Restrictions
Green Syntax is a deterministic language designed to bypass the ambiguity of English. It uses four primary "Gating Operators":

### I. `REQUIRE(tool_name)`
Forces the AI to include a specific library or module.
-   *Failure Mode:* If the agent generates code without the required import, the Sip fails.

### II. `FORBID(pattern_regex)`
Prevents the use of specific code patterns (e.g., `FORBID("while(true)")`).
-   *Failure Mode:* If the regex matches any part of the generated code, the build is rejected.

### III. `LIMIT(parameter, value)`
Sets hard numerical boundaries (e.g., `LIMIT(complexity, 10)`).

### IV. `EXPECT(fact_name, value)`
The desired outcome of the reasoning gate.

## 2. Reasoning Gates
A **Reasoning Gate** is a CDD check that occurs *before* the code is written. The AI must "explain its work" in Green Syntax.
-   **Step 1:** The AI proposes a solution in natural language.
-   **Step 2:** The AI translates that solution into Green Syntax.
-   **Step 3:** The CDD Parser validates the Syntax.
-   **Step 4:** Only if Syntax is valid does the AI generate the actual code.

## 3. Lab: Drafting a Reasoning Gate
Draft a Green Syntax block for a new function that calculates "Stress Levels" based on heart rate.
-   Require: `math`
-   Forbid: `global_variables`
-   Limit: `nesting_depth` to 2.
