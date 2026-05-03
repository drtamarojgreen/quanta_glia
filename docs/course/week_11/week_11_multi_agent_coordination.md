# Week 11: Multi-Agent Coordination—Swarm Intelligence and Fact Locks

## 1. The Swarm
In a large greenhouse, many agents (human and AI) work simultaneously. Without coordination, they will over-water (duplicate work) or plant invasive species (conflicting code).

## 2. The Fact Lock
To prevent conflicts, we use **Fact Locks**.
-   When an agent starts a Sip, it "Locks" the relevant Facts in the Registry.
-   Other agents can read the Facts but cannot overwrite them until the Sip is complete and the "Checkout" is verified.

## 3. Coordinating the "Neuro-Flow"
Coordination is achieved through the **Shared Registry**. Agents communicate not through chat, but through **Factual State**.
-   "Agent A has verified `postgres_version = 15.3`. I can now proceed with the database migration."

## 4. 3D Vision: Swarm Intelligence
Visualize the agents as glowing orbs moving through the 3D greenhouse. When an agent locks a Foundation Plate, the plate glows with a protective aura.

## 5. Lab: Multi-Agent Scenario
You are Agent B. Agent A has just failed a Sip in the `reporting` module.
1.  Read the Fact Registry to understand the failure.
2.  Determine if your current task (the `harvester` module) is "downstream" of the failure.
3.  Propose a "Locking Strategy" to prevent you from building on top of Agent A's wilted code.
