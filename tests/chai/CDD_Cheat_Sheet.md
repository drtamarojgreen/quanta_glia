# CDD Cheat Sheet: **C**ards, **F**acts, **R**unner \u2014 Your AI Coding Guide

## **What is CDD?**

**Chai Driven Development**: An AI-aware coding methodology for **reliable, incremental, and empirically verified** software construction. It turns chaotic AI code generation into a disciplined workflow.

---

## **The Sip Principle \u2014 Your Core Loop**

**Act small, verify often.** Every action is a "sip" \u2014 a minimal, verifiable, disposable increment.

1.  **Define Sip:** What *one capability* are you proving?
2.  **Code Sip:** Write *minimal C++ code* (no placeholders, no frameworks).
3.  **Execute Sip:** Run the code. *(Human/System)*
4.  **Observe Results:** Note empirical outputs (e.g., `key = value`).
5.  **Record Fact:** Update `tests/chai/cdd/facts/environment.facts` (append-only).
6.  **Repeat:** Take the next sip.

---

## **Key CDD Artifacts**

*   **Card (C++ file in `tests/chai/cdd/cards/`):**
    *   An executable C++ program.
    *   Implements *one capability* (e.g., `filesystem_create_file.cpp`).
    *   Outputs *empirical observations*, not `pass/fail`.
    *   _Example Output:_ `filesystem_create_file_operational = true`

*   **Fact (Line in `tests/chai/cdd/facts/environment.facts`):**
    *   A single line recording a *verified observation*.
    *   Always `key = value` (e.g., `compiler_available = g++`).
    *   **Append-only! Never modify existing facts.**

*   **Runner (C++ executable):**
    *   Executes Cards against Facts.
    *   Records Card results.

*   **`chai_checkins.md`:**
    *   Located in `tests/chai/cdd/`.
    *   Lists *unimplemented functions/features*.
    *   **No conceptual code in working files; only real code or nothing.**

---

## **CHAI CLI \u2014 Your Tools**

*   `chai`: Main CLI entrypoint (`CHAI CLI`).
*   `chai sip`: Displays the `SIP RULES`.
*   `chai discover cdd`: Finds the `cdd` directory.
*   `chai discover facts`: Finds the `facts` directory.

---

## **Guiding Principles \u2014 Wear Your Gloves!**

*   **Reality > Structure:** Prefer real capability over abstract architecture.
*   **Observe > Mutate:** Look first, then act.
*   **Minimize Write Surface:** Create new files; avoid modifying existing ones.
*   **Human Verify:** Stop for human checks (compile, run, observe).
*   **Small Executables:** Favor small, testable programs.
*   **Fear Silent Failure:** Explicitly observe everything.

---

## **Why CDD?**

Turns AI into a disciplined, real-world-aware collaborator. Prevents:
*   Over-engineering
*   Broken builds
*   Tool hallucination
*   Unverified code
*   Chaotic development

**CDD: The AI coding safety net for real-world environments.**
