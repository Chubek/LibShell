# Standard Kernels

`stdkern` is the in-tree standard-kernel catalog. It is intentionally compiled C++ over packaged script content: registration is explicit, metadata is inspectable, and execution remains under the `kernel::Kernel` lifecycle.

## Discovery

Use `lsh::stdkern::registry()` from `stdkern/StdKern.hpp` and attach it to a shell with `Shell::set_kernels`. `lsh::stdkern::catalog()` returns the same metadata list without exposing implementation objects.

## Precedence

Once registered, standard kernels resolve through the shell kernel registry before `PATH` lookup. This is intentional: standard kernels are deterministic runtime extensions and should not depend on host utility drift.

## Trust Policy

`stdkern` source is trusted at compile time. External kernel packages remain outside this catalog and must pass `PackageLoader::inspect`, compatibility checks, archive validation, and sandbox policy before `load`.

## Catalog

| Kernel | Status | Contract |
|---|---:|---|
| `:` | implemented | zero status, no output |
| `true` | implemented | zero status, no output |
| `false` | implemented | non-zero status, no output |
| `echo` | implemented | space-joined operands; repeated `-n` suppresses newline |
| `printf` | implemented | deterministic subset: `%s`, `%d`, `%%`, `\n`, `\t` |
| `basename` | implemented | final pathname component plus optional suffix removal |
| `dirname` | implemented | directory component, `.` for relative leaf names |
| `env` | implemented | exported environment listing only |
| `printenv` | implemented | selected variables or exported environment listing |

## Deferred POSIX Surface

Special builtins requiring shell control-flow mutation or assignment persistence are deferred until the IR exposes typed contracts for those effects: `break`, `continue`, `.`, `eval`, `exec`, `exit`, `export`, `readonly`, `return`, `set`, `shift`, `times`, `trap`, and `unset`.
