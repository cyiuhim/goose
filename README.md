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

Requires a C++20 compiler, CMake 3.20 or newer, and LLVM.

```bash
sh compile.sh
```

This configures the project, builds it, and installs the `goose` binary. To build manually:

```bash
cmake -S . -B build
cmake --build build
```

## Usage

```bash
goose path/to/program.goose
```

## Tests

The lexer and parser are covered by unit tests (via CTest) with a suite of `.goose` fixtures exercising tokenization and grammar edge cases.

```bash
sh test.sh
```

## Project layout

```
goose/
├── include/                # public headers
│   ├── lexer/              # DFA, states, transition table, keyword trie
│   ├── parser/             # LL(1) parser, grammar definition
│   ├── tree_transformer/   # AST node definitions
│   └── semantic_analyzer/  # visitor interface
├── src/                    # implementations mirroring include/
├── tests/                  # unit tests + .goose fixtures
└── code-snippets/          # example Goose programs
```
