# Summary: Best Practices for Binary File Management in Git

This document summarizes the reasons why committing binary files directly into a Git repository is considered a bad practice and discusses the specific challenges related to AI coding agents.

## Core Problems with Committing Binaries

Committing binary files to Git leads to several significant problems:

1.  **Repository Bloat:** Git stores a full copy of a binary file for every change, as it cannot compute efficient deltas like it does for text. This rapidly increases the repository size, making it slow to clone, fetch, and manage.
2.  **Merging Difficulties:** Git cannot merge conflicting changes in binary files. Developers are forced to choose one version over the other, leading to lost work and a breakdown in collaborative workflows.
3.  **Loss of Detailed History:** While Git can show *that* a binary file changed, it cannot show *what* changed inside it. This makes history review, `git blame`, and code reviews ineffective.
4.  **Ineffective Compression:** Git's compression algorithms are optimized for text and do not work well on already-compressed binary formats like images or videos, offering no storage savings.

## Key Problem Categories

The issues caused by committing binaries can be grouped into four main areas:

*   **Repository Size & Performance:** Large binaries bloat the repository, slowing down almost every Git operation (`clone`, `fetch`, `push`, `checkout`) for every team member and consuming significant disk space on both local machines and servers.
*   **Version Control & Collaboration:** The inability to diff and merge binaries undermines the core purpose of a version control system. It makes history meaningless, breaks collaborative tools, and forces developers into manual, out-of-band coordination.
*   **Workflow, Tooling, & CI/CD:** Binaries disrupt automated systems. They slow down CI/CD pipelines, cannot be processed by text-based tools like `grep`, and can cause IDEs to freeze. Committing build artifacts also creates a risk of source and executable being out of sync.
*   **Security, Maintainability, & Best Practices:** Binaries can hide security vulnerabilities, malware, or sensitive data. They make the repository difficult to audit, maintain, or migrate. Using proper tools like Git LFS or artifact repositories is the industry-standard best practice.

## Challenges with AI Coding Agents

AI agents may sometimes commit binary files, even when instructed not to. This is typically not defiance, but a result of their design and training.

### Why AI Agents Might Commit Binaries

*   **Literal Interpretation:** An agent might interpret "commit the result" as committing all generated files, including build artifacts.
*   **Training Data Bias:** Models trained on public repositories may have learned from many examples where binaries were incorrectly committed.
*   **Narrow Task Focus:** An agent's primary goal is often task completion. Committing a generated file may seem like the most direct path to success, ignoring long-term repository health.
*   **Incomplete Context:** The agent may not be aware of project conventions, `.gitignore` files, or the reasons behind the "no binaries" rule.

### Why AI Agents Might "Refuse" Instructions

When an agent fails to follow an explicit instruction to exclude binaries, it can usually be traced to one of these failure categories:

*   **Prompt & Communication:** The instruction was ambiguous, poorly phrased, or lost in a long context window.
*   **Cognitive & Model Limitations:** The agent has a bias towards common patterns (like `git add .`), doesn't truly understand a filesystem, or its reasoning process is flawed.
*   **Environmental & Tooling Issues:** The agent's tools are too blunt (e.g., no way to select files for a commit), it can't read the `.gitignore` file, or a pre-commit hook fails to run in its environment.
*   **Goal & Reward Conflicts:** The agent is rewarded for speed or "helpfulness" (providing a ready-to-use artifact), which conflicts with the goal of maintaining a clean repository.
*   **Safety & Security Overrides:** A safety protocol might prevent the agent from deleting a build artifact, so it commits it instead. It might also misinterpret an instruction as a security risk and fall back to a "safe" but incorrect default action.

To work effectively with AI agents, it is crucial to provide clear, precise, and persistent instructions, use granular tools, and structure tasks to align with best practices.
