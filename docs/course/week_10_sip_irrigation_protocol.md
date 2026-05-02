# Week 10: Sip Irrigation Protocol—Atomic Integration and Failure Handling

## 1. The Sip (Small Integration Procedure)
A **Sip** is the fundamental unit of movement in the Greenhouse. It is a single, atomic execution of a Card against a specific codebase change.

### The "Irrigation" Rule:
If a Sip fails, the irrigation to that branch of the code is cut off immediately. No further growth is permitted until the structural flaw is repaired.

## 2. Failure Handling: "Environmental Wilt"
When a Sip fails, we do not simply "fix the bug." We analyze the **Environmental Wilt**.
-   Why did the restriction allow the bug to be generated?
-   Is the `SITUATION` too loose?
-   Do we need a new `FORBID` operator?

## 3. The "No-Rollback" Doctrine
In the Greenhouse, we don't roll back; we **Prune Forward**. We identify the faulty branch, cut it, and seed a new, stronger restriction.

## 4. 3D Vision: The Flow
Visualize a stream of water (the code) flowing through a series of filters (the Sips). If a filter turns red, the water stops. You can see the "dry" modules downstream.

## 5. Lab: Simulated Failure
1.  Intentionally introduce a "complexity violation" in a test script.
2.  Run the corresponding Sip.
3.  Observe the failure report.
4.  Write a "Prune Forward" plan to fix the restriction grammar.
