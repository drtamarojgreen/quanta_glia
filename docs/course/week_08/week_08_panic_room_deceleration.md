# Week 8: Case Study—The Panic Room Deceleration

## 1. The Psychological Objective
Anxiety is characterized by **Chaotic Motion, High Contrast, and Sharp Edges**.
Calm is characterized by **Rhythmic Motion, Smooth Gradients, and Soft Curves**.

## 2. Translating State to Structure
To build a "Deceleration" module, we must define the transition in technical terms.

### The "Stress" Trellis (Initial State):
-   `LIMIT(particle_speed, 10.0)`
-   `REQUIRE(sharp_geometry_shader)`
-   `EXPECT(user_state, "high_arousal")`

### The "Calm" Trellis (Target State):
-   `LIMIT(particle_speed, 0.5)`
-   `FORBID(high_contrast_colors)`
-   `REQUIRE(rhythmic_pulse_util)`
-   `EXPECT(user_state, "decelerated")`

## 3. The Deceleration Bridge
How do we move from one to the other? In SDD, we create a **Bridge Card**. This Card monitors the Fact Registry and slowly adjusts the `LIMIT` operators over time.

## 4. 3D Vision: The Pulse
Visualize the entire environment "breathing." The walls of the greenhouse expand and contract in time with the `rhythmic_pulse` Fact.

## 5. Mid-Term Assessment: The Panic Room Implementation
Submit your Green Syntax and 3D conceptual design for a "Breathing Trellis."
-   Must include a `SITUATION`.
-   Must include at least three `LIMIT` operators that change based on user input.
