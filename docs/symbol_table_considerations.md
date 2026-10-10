# Design decision for symbol table

Given the requirement that we want to have nested functions to be able to access variables/functions in the outside scope(s), what are possible options for the data structure used for the symbol table?

This problem is concerned about the symbol table both during the traversal and the cummulative result. They can be designed as two separate data structures for easier tracking, or the same data structure.

Requirements:

- The visitor will use DFS traversal on the AST
- The data structure must support lookup on the outside scoped variables to check whether the currently inspected variable exists within current scope or any outside scopes
- The data structure must support addition of scoped variables as it goes down the tree of nested functions in the AST
- The data structure must support removal of scoped variables as the visitor goes up on the DFS traversal, i.e. leaving an inner scope
- The visitor would return a tree of symbol tables at the end to represent the variables in the entire program

## Design option 1: Tree based

- The cummulative tree structure of the symbol table is shared amongst during traversal of the type check visitor, as well as the final return result
- Each node corresponds to a scope
- Each node would contain the types of the local variables, as well as the nested functions' type tables
- Pros:
  - We can just probably return this tree as the result of the semantic analyzer, for code generation, no need to do extra work at the end to parse the data structure
- Cons:
  - From a nested function, if we want to find the reference of a function/variable within an outside scope, it becomes much harder
    - For a fix, we can just allow the nodes to point to their parents
    - This can avoid the extra use of the data structure (e.g. a stack) to keep track of the ancestors when doing the DFS traversal of the AST

## Design option 2: a flat map of names to types

- Give each identifier a unique name (e.g. foo.a vs bar.a), so that there are no collisions in names
- Keep track of an unordered map of variable/function identifiers to types
- Pros:
  - Fast access of type names
  - We can know very easily whether a variable/function from the outside scope exists or not by constructing the unique name of the identifier as if it could appear in each level
  - For LLVM code gen purposes, it would be easier to flatten a tree of nested functions
- cons:
  - Quite annoying or inefficient to remove the inside scoped variables once done with traversing the inside scope

## Design option 3: Tree based + vector of scoped type tables

- Still keep track of a tree of symbol tables, just like in option 1
- However, different from option 1, it keeps track of the type tables at each scope level as one entry within the vector
- Pros:
  - Easy to check the innermost outside scope that has the variable
  - Easy traversal, and when exiting an inside scope it can nicely clean up all the inner scoped variables that are now out of scope
- Cons:
  - Easy to forget to update both data structures for each new variable seen

### Verdict:

- Rationale:
