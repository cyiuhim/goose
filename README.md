# Goose

A compiler for Goose, a small toy language, written from scratch in modern C++20 and targeting LLVM IR. Every stage of the pipeline (lexing, parsing, and tree transformation) is hand-implemented.

The goal of this project is to work through compiler theory end to end: finite automata, formal grammars, LL(1) parsing tables, abstract syntax trees, and lowering to LLVM IR.

## Highlights

- Hand-written DFA lexer. Tokenization runs on an explicit deterministic finite automaton with a declarative state-transition table. Keywords are resolved through a trie for efficient longest-match lookup.
- Table-driven LL(1) parser. The parser reads a formal context-free grammar and computes the nullable/epsilon-reachable non-terminals, the FIRST and FOLLOW sets, and the LL(1) parsing table, with grammar validation for conflicts.
- AST construction. A TreeTransformer lowers the concrete parse tree into a cleaner abstract syntax tree (ProgramNode, FunctionNode, StatementNode) for semantic analysis and code generation via the visitor pattern.
- LLVM backend. The AST is lowered to LLVM IR, letting LLVM handle optimization and native code generation.
- Modern C++. C++20, RAII with std::unique_ptr ownership throughout, and a modular CMake build.

## Architecture

Goose processes source through a staged pipeline, with each stage a self-contained, independently testable module.

```
  .goose source
       |
       v
   Lexer          DFA + keyword trie, produces a token stream
       |
       v
   Parser         LL(1) table-driven parser (FIRST/FOLLOW/parse table),
       |          produces a concrete parse tree
       v
   AST Builder    TreeTransformer, produces an abstract syntax tree
       |
       v
   Semantic       Type checking, undeclared variables, etc.
   Analyzer
       |
       v
   LLVM Codegen   visitor-based lowering to LLVM IR, then native binary
```

| Stage                       | Module                      | Notes                                                         |
| --------------------------- | --------------------------- | ------------------------------------------------------------- |
| Lexing                      | `src/lexer`                 | DFA with declarative transitions, trie-based keyword matching |
| Parsing                     | `src/parser`                | LL(1) parser generated from a formal grammar                  |
| AST                         | `src/tree_transformer`      | Parse tree to typed AST nodes                                 |
| Semantic analysis / codegen | `include/semantic_analyzer` | Visitor pattern over the AST, lowering to LLVM IR             |

## The Goose language

Goose is a small language with playful, themed syntax. Functions print with `honk`, variables are declared with `flap`, and the entry point is `fly`.

```goose
## define a function with start/end; names are wrapped in @...@
## the entry point is called fly

start @fly@
    flap a = 5.            ## declare a variable with `flap`, end statements with `.`
    a = 6.                 ## reassignment
    honk<<"a is: ", a>>.   ## `honk` prints; params go inside <<...>>
end @fly@
```

Language reference:

| Concept              | Goose syntax                  |
| -------------------- | ----------------------------- |
| Function definition  | `start @name@ ... end @name@` |
| Entry point          | `@fly@`                       |
| Variable declaration | `flap x = 5.`                 |
| Assignment           | `x = 6.`                      |
| Print                | `honk<<...>>.`                |
| Statement terminator | `.`                           |
| Comment              | `## comment`                  |

More examples live in [`code-snippets/`](code-snippets/).

## Build

Requires a C++20 compiler, CMake 3.20 or newer, and LLVM. The checked-in CMake presets keep all generated files under `build/`.

For a development build with tests enabled:

```bash
cmake --preset dev
cmake --build --preset dev
```

The compiler is written to `build/dev/goose`. The compatibility wrapper runs the same commands:

```bash
sh compile.sh
```

For an optimized build without tests:

```bash
cmake --preset release
cmake --build --preset release
```

Installation is optional and uses an explicit, unprivileged prefix:

```bash
cmake --install build/release --prefix "$HOME/.local"
```

## Usage

Run a development build directly:

```bash
./build/dev/goose path/to/program.goose
```

If installed to a directory on `PATH`:

```bash
goose path/to/program.goose
```

## Tests

The lexer, parser, and tree transformer are covered by CTest unit tests with a suite of `.goose` fixtures exercising tokenization and grammar edge cases.

```bash
cmake --preset dev
cmake --build --preset dev
ctest --preset dev
```

The test wrapper invokes the same workflow:

```bash
sh test.sh
```

## Project layout

```
goose/
├── CMakeLists.txt          # project configuration and top-level orchestration
├── CMakePresets.json       # shared development and release workflows
├── include/                # public headers
│   ├── lexer/              # DFA, states, transition table, keyword trie
│   ├── parser/             # LL(1) parser, grammar definition
│   ├── ast_nodes/          # AST node definitions
│   ├── tree_transformer/   # parse-tree to AST transformation
│   └── semantic_analyzer/  # semantic analysis interface
├── src/                    # implementations and production target definitions
│   └── CMakeLists.txt
├── tests/                  # unit tests + .goose fixtures
└── code-snippets/          # example Goose programs
```
