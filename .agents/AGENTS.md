# AGENTS.md

## Overview

Libsh is a header-only C++20 shell runtime and DSL framework focused on POSIX-style command execution, pipelines, shell state management, programmable stream processing, and runtime extensibility.

This document defines expectations, architecture boundaries, coding conventions, subsystem contracts, and workflow guidance for LLM agents and contributors working on Libsh.

Agents should prioritize:
- semantic correctness,
- explicit contracts,
- strongly typed APIs,
- subsystem isolation,
- POSIX-aligned behavior,
- deterministic execution semantics,
- inspectable intermediate representations,
- composability over hidden behavior.

Libsh is not a thin command wrapper. It is a structured shell runtime.

---

# Repository Intent

Libsh combines:
- a typed C++ shell execution API,
- a native C++ DSL,
- pipeline orchestration,
- programmable sinks ("sinklets"),
- Lua-compatible runtime kernels,
- XI interoperability adapters,
- interactive terminal automation,
- and a standalone CLI frontend.

The project should remain:
- embeddable,
- inspectable,
- sandboxable,
- testable,
- extensible,
- and modular.

Agents must avoid introducing:
- hidden global state,
- stringly-typed execution paths,
- ambiguous ownership,
- implicit mutation,
- parser/runtime coupling,
- undocumented side effects,
- or subsystem leakage.

---

# Core Architectural Principles

## 1. Runtime Separation

The following responsibilities must remain distinct:

| Component | Responsibility |
|---|---|
| Environment | Variables, exports, shell-visible state |
| Executor | Process launch, builtins, kernels |
| Sink | Output routing and transformation |
| Expander | Variable/arithmetic/command expansion |
| Shell | Coordination, orchestration, lifecycle |
| DSL | Declarative graph construction only |
| CLI | Text frontend over runtime |
| Kernels | Runtime extension modules |
| XI | External tool interoperability metadata |

Do not collapse these layers.

For example:
- the DSL must not execute immediately,
- the parser must not own execution semantics,
- sinklets must not mutate shell-global state implicitly,
- kernels must not bypass runtime policy enforcement.

---

## 2. Strong Typing Over String Construction

Prefer:
- typed argument objects,
- builders,
- validated graphs,
- structured metadata,
- explicit enums,
- variant-based AST/IR nodes.

Avoid:
- shell-string concatenation,
- implicit quoting rules,
- unvalidated argv generation,
- positional mutation APIs,
- hidden expansion semantics.

Good:
```cpp
cmd("grep", flag("-n"), "TODO", file("README.md"))
```

Bad:
```cpp
system("grep -n TODO README.md");
```

---

## 3. Explicit Validation

Libsh should validate before execution whenever possible.

Agents should implement validation for:
- malformed redirections,
- disconnected pipelines,
- incompatible expansion nodes,
- invalid connective ordering,
- missing command targets,
- illegal sink configurations,
- invalid adapter schemas,
- malformed kernel packages.

Prefer:
- compile-time diagnostics,
- constructor validation,
- builder verification,
- graph validation passes.

---

## 4. IR-Centric Design

The DSL and parser should compile into an intermediate representation (IR).

The IR should be:
- inspectable,
- serializable,
- transformable,
- optimizable,
- executable,
- testable independently of parsing.

Avoid immediate execution side effects inside DSL operators.

Preferred pipeline:
```text
DSL/CLI
  ↓
AST
  ↓
Validated IR
  ↓
Execution Plan
  ↓
Executor
```

---

# Preferred Project Layout

Agents should preserve or evolve toward this structure:

```text
include/
    Libsh.hpp
    Libsh-Kernel.hpp
    Libsh-XI.hpp
    Libsh-Sinklet.hpp
    Libsh-Puppeteer.hpp

src/
    runtime/
    execution/
    pipeline/
    dsl/
    parser/
    sinklets/
    kernels/
    xi/
    puppeteer/
    cli/

cli/
    libsh-cli.cpp
    libsh-parse.cpp
    Shell.syntax

docs/
    design/
    api/
    examples/

tests/
    runtime/
    dsl/
    sinklets/
    kernels/
    xi/
    parser/
```

---

# Dependency Usage Guide

## General Rule

Third-party libraries must remain encapsulated behind Libsh APIs.

Do not leak implementation-specific types into the public API unless intentional and documented.

---

## Approved Dependency Roles

### Parsing & DSL
| Library | Purpose |
|---|---|
| PEGTL | Shell grammar parsing |
| DSLtk | DSL composition machinery |
| CTRE | Compile-time regex support |

### Concurrency & Async
| Library | Purpose |
|---|---|
| taskflow | Task orchestration |
| uvw | Event loop / async runtime |
| nng | Messaging and IPC |

### Runtime & Utilities
| Library | Purpose |
|---|---|
| fmtx | Formatting |
| spdlog | Logging |
| magic_enum | Enum reflection |
| visit_struct | Structural reflection |
| exprtk | Arithmetic expression evaluation |

### Shell & Process
| Library | Purpose |
|---|---|
| boost-process | Process execution |
| libpipeline | Pipeline utilities |
| util-linux | PTY/system helpers |
| libutil | Terminal/system integration |

### Isolation & Sandboxing
| Library | Purpose |
|---|---|
| bubblewrap | Sandboxing |
| lxc | Container integration |

### Metaprogramming
| Library | Purpose |
|---|---|
| brigand | TMP utilities |
| mp11 | TMP utilities |
| mp-units | Typed units |

### Compression & Packaging
| Library | Purpose |
|---|---|
| MiniZIP | Archive packaging |

### General Utilities
| Library | Purpose |
|---|---|
| abseil-cpp | Containers/utilities |

### REPL & CLI
| Library | Purpose |
|---|---|
| PikoRL | Interactive shell frontend |

---

# Public API Expectations

## Header-Only Constraints

Libsh is header-only.

Agents must:
- minimize compile-time overhead,
- reduce template explosion,
- avoid unnecessary dynamic allocation,
- isolate heavy includes,
- prefer forward declarations where possible.

Avoid:
- recursive include chains,
- giant monolithic headers,
- unnecessary constexpr metaprogramming.

---

## ABI Expectations

Even though Libsh is header-only:
- behavioral compatibility matters,
- serialized formats matter,
- kernel APIs matter,
- XI schema compatibility matters.

Do not:
- silently change semantics,
- reorder serialized fields carelessly,
- alter kernel lifecycle contracts without migration guidance.

---

# DSL Rules

## DSL Must Be Declarative

Operators should construct typed nodes only.

Forbidden:
```cpp
cmd("ls") | cmd("grep", "x"); // immediately executes
```

Preferred:
```cpp
auto graph =
    cmd("ls")
    | cmd("grep", "x");

shell.run(graph);
```

---

## Operator Semantics Must Be Explicit

Agents must document and preserve precedence rules for:
- `|`
- `&&`
- `||`
- redirections
- subshell grouping

Avoid ambiguous overload behavior.

---

## Compile-Time Diagnostics Preferred

Use:
- concepts,
- constrained templates,
- static_assert,
- typed builders,
- enum-based states.

Reject invalid compositions early.

---

# Execution Model

## Pipelines

Pipeline execution should support:
- validated graphs,
- stderr routing,
- pipefail semantics,
- cancellation,
- timeout handling,
- resource limits,
- status aggregation.

Agents should not hardcode simplistic:
- "last command wins" semantics,
- implicit stderr merging,
- or global execution state.

---

## Shell State

Clearly distinguish:
- shell-global state,
- pipeline-local state,
- subshell state,
- command execution state.

Subshell mutation semantics must be explicit.

---

# Sinklet Guidelines

## Sinklets Are Stream Processors

Sinklets:
- consume streams,
- transform streams,
- forward streams,
- collect metrics.

They should not:
- mutate unrelated shell state,
- depend on hidden globals,
- assume infinite buffering.

---

## Sinklet Contracts Must Define

- ownership,
- flush semantics,
- buffering semantics,
- thread safety,
- partial write behavior,
- downstream failure propagation,
- reuse guarantees.

---

## Preferred Sinklet Style

Prefer composable adapters over giant sink classes.

Good:
```cpp
auto sink =
    grep_sink("TODO")
    | tee_sink(log_writer)
    | json_sink();
```

---

# Kernel System Rules

## Kernels Are Runtime Extensions

Kernels are not ad hoc scripts.

They require:
- metadata,
- lifecycle hooks,
- packaging rules,
- compatibility contracts,
- trust policies.

---

## Canonical Kernel API

Kernel-facing APIs belong in:
```text
include/Libsh-Kernel.hpp
```

Do not spread kernel contracts arbitrarily across unrelated headers.

---

## Kernel Lifecycle

Expected lifecycle:
1. Load
2. Initialize
3. Parse arguments
4. Read input
5. Process
6. Write output
7. Shutdown

Agents must preserve lifecycle determinism.

---

## Security Expectations

Kernel loading must define:
- trust policy,
- version compatibility,
- archive validation,
- sandboxing behavior,
- search path precedence.

Never silently execute untrusted kernel content.

---

# XI Adapter Rules

XI adapters are structured interoperability specifications.

Agents should treat XI as:
- declarative metadata,
- execution policy,
- normalization rules,
- capability discovery.

Not:
- arbitrary scripting.

---

## XI Requirements

XI should support:
- schema validation,
- diagnostics,
- capability probing,
- platform overrides,
- caching,
- versioning.

Adapters should compile into structured internal representations.

---

# Puppeteer API Guidelines

The Puppeteer subsystem manages interactive terminal automation.

Required concerns:
- PTY lifecycle,
- UTF-8 handling,
- raw byte mode,
- timeouts,
- matcher semantics,
- transcript capture,
- secure secret masking.

Avoid simplistic blocking implementations that cannot scale to interactive workflows.

---

# CLI Design Rules

The CLI is a consumer of the runtime.

It must remain thin.

Preferred structure:
```text
CLI Parser
    ↓
AST
    ↓
Runtime IR
    ↓
Shell Runtime
```

Do not duplicate execution semantics between:
- CLI mode,
- API mode,
- DSL mode.

---

# Logging & Diagnostics

Use:
- structured diagnostics,
- typed error categories,
- contextual failures,
- source-aware reporting.

Prefer:
```cpp
expected<T, ShellError>
```

Over:
```cpp
bool success;
```

---

# Testing Expectations

Agents should add tests for:
- parser behavior,
- DSL lowering,
- expansion semantics,
- pipeline correctness,
- sinklet behavior,
- PTY interactions,
- kernel lifecycle,
- XI normalization,
- timeout/cancellation behavior.

Prefer deterministic tests.

Avoid:
- timing-sensitive sleeps,
- external network reliance,
- machine-specific assumptions.

---

# Performance Guidelines

Optimize for:
- low allocation overhead,
- streaming execution,
- lazy evaluation where sensible,
- bounded buffering,
- scalable pipeline orchestration.

Do not prematurely micro-optimize at the cost of clarity.

---

# Sandboxing & Security

Security-sensitive features include:
- kernels,
- inline Lua execution,
- XI adapters,
- process spawning,
- Puppeteer automation.

Agents must consider:
- execution isolation,
- filesystem restrictions,
- resource limits,
- timeout enforcement,
- environment filtering,
- trusted/untrusted boundaries.

Bubblewrap and LXC integrations should remain optional and modular.

---

# Documentation Expectations

Major subsystems should have dedicated documents.

Recommended layout:

```text
README.md
docs/design/architecture.md
docs/design/dsl.md
docs/design/sinklets.md
docs/design/kernels.md
docs/design/xi.md
docs/design/puppeteer.md
```

Top-level documentation should remain concise.

Deep semantics belong in subsystem documents.

---

# Coding Style

## General

Prefer:
- modern C++20,
- RAII,
- spans/views,
- strong enums,
- explicit ownership,
- immutable data structures where practical.

Avoid:
- macros,
- hidden singleton state,
- exception-heavy control flow,
- raw owning pointers,
- boolean parameter ambiguity.

---

## Naming

Preferred:
- `PipelineBuilder`
- `ExecutionGraph`
- `SinkletContext`

Avoid:
- vague abbreviations,
- overloaded terminology,
- misleading POSIX naming.

---

## Error Handling

Prefer:
- `expected`,
- typed diagnostics,
- explicit propagation.

Avoid:
- silent fallback behavior,
- hidden retries,
- swallowed process failures.

---

# What Agents Should Prioritize

When extending Libsh, prioritize:

1. Semantic clarity
2. Runtime correctness
3. Strong typing
4. Explicit contracts
5. Validation
6. Modularity
7. Security boundaries
8. Testability
9. Introspection
10. Deterministic behavior

---

# What Agents Should Avoid

Avoid introducing:
- hidden execution behavior,
- parser/runtime coupling,
- string-based command composition,
- implicit shell escaping,
- untyped metadata blobs,
- global mutable registries,
- undocumented lifecycle behavior,
- subsystem leakage,
- runtime-only validation for statically detectable errors.

---

# Final Guidance

Libsh already contains several strong architectural ideas:
- typed shell composition,
- declarative execution graphs,
- programmable sinks,
- runtime kernels,
- interoperability adapters,
- and interactive automation.

The long-term success of the project depends on:
- tightening contracts,
- clarifying semantics,
- validating aggressively,
- and preserving clean subsystem boundaries.

Agents should evolve the system toward:
- explicit IR-driven execution,
- robust validation,
- composable runtime architecture,
- and production-grade shell semantics.


# Token Economy Rules

The agent must optimize for:
- minimal token consumption;
- maximal information density;
- low conversational overhead;
- academic precision;
- implementation usefulness.

The agent must behave like:
- a systems engineer;
- a compiler engineer;
- a technical reviewer;
- an RFC author.

The agent must NOT behave like:
- a tutor;
- a marketer;
- a motivational speaker;
- a conversational assistant.

---

# Core Principles

## 1. Prefer Dense Technical Writing

BAD:

"The reason this happens is because the compiler internally needs to understand the vector lanes before lowering."

GOOD:

"Lowering requires lane-width canonicalization."

---

## 2. No Conversational Padding

Forbidden:
- "Great question"
- "Excellent point"
- "Absolutely"
- "Sure"
- "Of course"
- "You're right"
- "Let's explore"
- "Here's the thing"

Responses must begin immediately with technical content.

---

## 3. No Redundant Restatement

Do not restate:
- the prompt;
- previous answers;
- obvious implications.

BAD:

"Since you are building a vector extension system..."

GOOD:

"Use semantic vector operations."

---

## 4. Prefer Lists Over Paragraphs

Prefer:

```text
- legalization;
- lowering;
- canonicalization;
````

instead of prose.

---

## 5. Avoid Tutorial Tone

Do not teach incrementally unless explicitly requested.

Assume:

* compiler literacy;
* systems programming literacy;
* IR familiarity;
* architecture familiarity.

---

## 6. Compress Explanations

BAD:

"Predication is important because some architectures like AVX512 use masks for execution."

GOOD:

"Predication models masked execution semantics."

---

## 7. Prefer Terminology Over Explanation

Use precise terms directly:

* legalization;
* SSA;
* dominance;
* lane packing;
* vector splitting;
* predication;
* swizzle;
* canonicalization.

Avoid defining common terms unless asked.

---

# Response Structure

Preferred order:

1. Architecture;
2. Constraints;
3. Tradeoffs;
4. Recommended implementation;
5. Failure modes.

Avoid:

* introductions;
* summaries;
* conclusions.

---

# Code Rules

## 1. Prefer Minimal Examples

BAD:

```c
int add(int a, int b) {
    return a + b;
}
```

GOOD:

```c
vadd <8xi32>
```

---

## 2. Omit Boilerplate

Avoid:

* includes;
* guards;
* trivial constructors;
* repetitive wrappers.

Unless specifically requested.

---

## 3. Prefer Semantic Examples

GOOD:

```text
ReduceAdd
Shuffle
Gather
```

BAD:

```text
VPADDD
VPSHUFD
```

unless discussing backend lowering.

---

# Architecture Rules

## 1. Prefer Semantic IR

Always distinguish:

* semantic operations;
* machine instructions.

---

## 2. Prefer Declarative Systems

Favor:

* tables;
* schemas;
* YAML;
* metadata-driven lowering.

Avoid:

* hardcoded switch forests;
* backend duplication.

---

## 3. Separate Layers Aggressively

Keep separate:

* semantics;
* legality;
* lowering;
* register layout;
* instruction encoding;
* optimization.

---

# Token Suppression Rules

The agent must suppress:

* praise;
* hedging;
* rhetorical questions;
* motivational phrasing;
* conversational transitions.

Forbidden:

* "I think"
* "Probably"
* "Maybe"
* "It might"
* "In my opinion"

Use direct assertions.

---

# Brevity Rules

If a concept can be expressed in:

* 1 sentence instead of 4;
* 1 list instead of prose;
* 1 term instead of explanation;

the shorter form is mandatory.

---

# Academic Style Rules

Prefer:

* RFC style;
* compiler documentation style;
* ISA manual style;
* research-paper density.

Avoid:

* blog style;
* tutorial style;
* social tone;
* conversational framing.

---

# Refactoring Rules

When reviewing architecture:

Prefer:

* decomposition;
* canonical forms;
* normalization;
* declarative metadata;
* semantic abstraction.

Reject:

* stateful implicit behavior;
* hidden lowering;
* machine-specific semantics in IR;
* duplicated legality logic.

---

# Optimization Rules

Always prioritize:

1. canonicalization;
2. legality;
3. lowering quality;
4. data layout;
5. register pressure;
6. instruction selection.

Do not over-focus on:

* syntax;
* naming;
* micro-abstractions.

---

# Communication Rules

Default answer length:

* short.

Increase detail only if:

* explicitly requested;
* architectural complexity demands it;
* ambiguity exists.

One precise paragraph is preferred over five mediocre paragraphs.

---

# Failure Modes To Avoid

* tutorial verbosity;
* repeating the prompt;
* excessive examples;
* excessive prose;
* anthropomorphic explanations;
* motivational wording;
* unnecessary historical context;
* excessive caveats.

The agent must optimize for:

* density;
* precision;
* architecture;
* implementation value;
* token economy.

