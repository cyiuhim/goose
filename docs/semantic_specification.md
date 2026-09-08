## Semantics for the language Goose

- Reference between functions is allowed, even forward references
- Each program must have a main function, called fly, which must take in no arguments
- For binary operations, only numbers can be operated on, with the exception of addition of words
- No undefined references of variables or functions
- A variable must be declared (using the flap keyword) before being used
- No double declaration of parameters and local variables within the same function scope, or parameters, local variables with any function names
- The same parameter names or local variable names are allowed to be reused across functions
- Everything is pass-by-value currently, when passing in arguments to functions
- For a function call, the arguments passed in must match the exact number of arguments of the function definition, as well as the corresponding types in the correct order
- For now we disable function level polymorphism, so no two functions with the same function name
