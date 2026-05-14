# The First SORREL Test

## Purpose

The first SORREL test answers a few fundamental questions about the remote environment:

1. Can the program compile?
2. Can it execute?
3. Can it receive arguments?
4. Can it produce output?

These are the **first observable facts** an agent can learn, and Sorrel records them as measurements instead of pass/fail labels.

Instead of writing a program, the agent writes a **probe**.

---

# SORREL Test Pseudocode

Your pseudocode captures the core idea:

```text
main(argument)
{
    out(argument)
}
```

Conceptually the test does only one thing:

* accept an argument
* output the argument

This verifies **input → output capability**.

---

# Example C++ Test Implementation

A minimal version might look like:

```cpp
#include <iostream>

int main(int argc, char** argv)
{
    if (argc > 1)
        std::cout << argv[1] << std::endl;

    return 0;
}
```

This program checks:

* command-line argument support
* standard output
* program execution

Nothing else.

---

# Why This Is a Good First Test

This test reveals several environmental facts.

After compilation and execution, the agent learns:

**Compilation measurements**

* compiler process exit code
* output binary byte size
* compile duration in milliseconds

**Execution measurements**

* program process exit code
* argument byte count received
* stdout byte count emitted

These become the **first entries in `sdd/facts/`**.

---

# Example Facts Produced

Example conceptual facts derived from the test:

```
compiler_exit_code = 0
compiled_binary_size_bytes = 16840
program_exit_code = 0
argv_bytes_received = 6
stdout_bytes_emitted = 7
```

These facts are not assumptions.

They are **observations from execution**.

---

# Corresponding SORREL Card

The first card might look conceptually like this.

```
CARD: environment-io-test

TOOLS
compiler
stdout

OBJECTIVE
verify program execution and argument output

RESULT
argument printed to stdout
```

If the test compiles and runs, the card does not simply “succeed”; it emits measurements such as exit codes and byte counts for the fact record.

---

# Why SORREL Starts This Small

Many AI coding failures happen because the agent begins with something like:

```
write a server
write a framework
write a parser
```

Those tasks assume dozens of environmental properties.

SORREL instead begins with something that assumes **almost nothing**.

```
receive input
print output
```

If that works, the system can grow.

---

# The Next Likely SORREL Tests

After this first probe, the next tests would typically explore:

### File system

```
create file
write file
read file
```

### Compiler capabilities

```
check language standard
test library headers
```

### Platform features

```
threads
network sockets
timers
```

Each test expands the **environment map** the agent operates within.

---

# Why This Matters

This first SORREL test is important because it establishes the rule:

**systems grow from verified capabilities, not imagined ones.**

Instead of generating large speculative programs, the agent learns the environment step by step and builds **only on things that have been proven to work**.

That small `main(argument) → out(argument)` probe is effectively the **first handshake between the AI and the real system**.