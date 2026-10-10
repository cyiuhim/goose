## Semantics for the language Goose

- Reference between functions is allowed, no forward references
- Each program must have a main function, called fly, which must take in no arguments
- For binary operations, only numbers can be operated on, with the exception of addition of words
- No undefined references of variables or functions
- A variable must be declared (using the flap keyword) before being used
- No double declaration of parameters and local variables within the same function scope, or parameters, local variables with any function names
- The same parameter names or local variable names are allowed to be reused across functions
- Everything is pass-by-value currently, when passing in arguments to functions
- For a function call, the arguments passed in must match the exact number of arguments of the function definition, as well as the corresponding types in the correct order
- For now we disable function level polymorphism, so no two functions with the same function name
- Any reference to a variable would be referring to the variable that exists within the innermost outer scope, within nested functions.

## Special cases

### Case 1: Nested function different references

```
start @foo@ () -> number
    start @bar@ () -> number
    flap a = 3.
    return a.
    end @bar@
    flap a = 5.
    flap b = bar().
    return a + b.
end @foo@
```

This is allowed. This is because the variable `a` in the outside scope (`foo`), is different from the variable `a` in the inside scope (`bar`). The `return a.` statement is referring to the inner `a` (within the `bar` scope, not the `foo` scope).

This is somewhat analogous to loops in C++:

```
int func() {
    int sum = 0;
    for (int i = 0; i < 10; i++) {
        int n = i * i;
        sum += n;
    }
    int n = rand();
    return sum + n;
}
```

Even this would be allowed (this wouldn't cause redefinition of variable `a`):

```
start @foo@ () -> number
flap a = 5.

    start @bar@ () -> number
    flap a = 3.
    return a.
    end @bar@


    flap b = bar().
    return a + b.

end @foo@
```
