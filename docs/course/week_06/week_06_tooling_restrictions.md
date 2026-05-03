# Week 6: Tooling Restrictions—The Tool Rack and Ecosystem Constraints

## 1. The Tool Rack
In the Greenhouse, an AI agent is only as good as the tools it is allowed to hold. Allowing an agent to "hallucinate" tools (import non-existent libraries) is like giving a gardener a holographic shovel—it looks right, but nothing gets planted.

## 2. Technical Implementation: `REQUIRE(tool_name)`
Every module must start with a `SITUATION` that defines the **Tool Rack**.
-   **Strict Imports:** If it's not in the `REQUIRE` list, the parser blocks the import.
-   **Version Locking:** We lock tools to specific "Climates" (versions).

## 3. Ecosystem Harmony
We prioritize tools that are "Ecologically Harmonious"—those that are already part of the PrismQuanta workspace.
-   **Internal APIs:** Calling `quanta_tissu_connector`.
-   **Shared Utils:** Using `agent_utils.logging_utils`.

## 4. 3D Vision: The Spatial Tool Rack
Imagine a physical rack at the entrance of the greenhouse. If an agent tries to enter with a tool that isn't on the rack, the "Reasoning Gate" (a physical light-curtain) turns red.

## 5. Lab: Designing a Situation
Create a `SITUATION` block for a module that needs to interact with the Fact Registry.
-   What tools are `REQUIRED`?
-   What external domains are `FORBIDDEN` (to prevent data leaks)?
