# LOTAR
LOTAR is a young but promising language.

## Variables
There are two scopes in the language. Global and local for each function. Global variables are visible inside each function, local variables are visible only inside their own function.

Variables are declared in a special section and only in it. This section should be included at the beginning of each program and at the beginning of each function.

### Example of a section:

def {\
&ensp;a = 0;\
&ensp;b = 1;\
}

All variables are of type uint32.

## For
for var in [exp1, exp2] {\
&ensp;...\
}

## While
while (exp) {\
&ensp;... \
}

## If
if (exp) {\
&ensp;... \
} else {\
&ensp;...\
}

## Expression
### The following operations are supported:
1. ||
2. &&
3. !=
4. ==
5. <=
6. \>=
7. \>
8. \<
9. \+
10. \-
11. \*
12. /
13. %

### The following operands are supported:
1. Global variable
2. Local variable
3. Int
4. RAND(random int)

## GI
The following two graphical functions are available:
1. FLUSH
2. PUT(x(expr), y(expr), argb(expr))

## Function
### The function declaration has the following syntax:
func name(arg1, arg2, ..., argn) {
&ensp;...
}

### Calling a function
call name(arg1, arg2, ..., argn)

All the arguments are expressions.

The entry point is a function main()