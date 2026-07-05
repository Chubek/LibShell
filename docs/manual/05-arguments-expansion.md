# 05. Arguments and Expansion {#manual_arguments_expansion}

## Types

- `lsh::Expansion`: one expansion fragment.
- `lsh::Argument`: ordered fragment sequence.
- `lsh::Expander`: runtime materializer.
- `lsh::ScriptingBackend`: command/Lua expansion backend interface.

## Expansion Kinds

| Kind | Builder | Contract |
|---|---|---|
| raw | `literal` | no shell quoting semantics |
| single quoted | `single_quoted` | literal payload |
| double quoted | `double_quoted` | quoted payload semantics |
| variable | `variable` | environment lookup |
| arithmetic | `arithmetic` | integer expression evaluation |
| command | `command_substitution` | nested shell evaluation |
| Lua | `lua` | scripting backend evaluation |
| glob | `glob` | filesystem pattern expansion |

## Field Splitting

Variable and command substitutions can request field splitting. Literal fragments remain single argv components.

## Failure Modes

- malformed arithmetic: `ErrorCode::bad_expansion`;
- failed command substitution: backend diagnostic;
- unmatched glob: literal pattern preservation;
- unavailable backend: expansion diagnostic.
