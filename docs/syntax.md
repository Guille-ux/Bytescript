# Syntax

#### statement

A bytescript file may contain any number of **statement** separated either by a *newline* or a *semicolon*
A statement may take one of the following form:
- [return statement](#return-statement)
- [if statement](#if-statement)
- [for loop](#for-loop)
- [while loop](#while-loop)
- [repeat loop](#repeat-loop)
- [import statement](#import-statement)
- [class definition](#class-definition)
- [allocation statement](#allocation-statement)
- [break statement](#break-statement)
- [continue statement](#continue-statement)
- [expression](#expression)

#### expression

An expression may take one of the following form:
- [variable definition](#variable-definition)
- [field definition](#field-definition)
- [assignement](#assignement)
- [require expression](#require-expression)
- [operation](#operations)
- [numeric value](#numeric-values)

#### numeric values

A numeric value may take one of the following form:
- [data](#data-types)
- [expression](#expression) within parentheses `(expression)`

# Variable definition

A **variable** can be **defined** with the syntax : `[data type] [identifier] = [data]`
An **identifier** can either start with a *letter* or `_`, and can contain any *letter*, *number*, or `_`
The specified **data** must have the same *data type* as the specified **data type** or the type `var`

A variable may be defined without value using the syntax `[data type] [identifier]`
The variable will contain **garbage value** and should not be used until it has been [assigned](#assignement) a value

see more about [data types](#data-types)

# Assignement

You can **assign** a value to an lvalue (usaly variables) with the syntax : `[lvalue] = [data]`
The **data** must have the same *data type* as the one that was specified during declaration, or `var`

see more about [lvalues](#lvalues)

# Data types

A **data type** refer to a [class](#class-definition), either defined by the user, external libraries, or built inside the language
The following data type are built-in and can be used in any bytescript program :
- `Number`, may be shortened to `num`
- `String`, may be shortened to `str`
- `Table`, may be shortened to `table`
- `Function`
- `CData`, see more about [embedding](docs/embedding.md)
- `File`

Excluding classes wich does not serve the purpose of being a data type such as `IO`, `Module`, and `Error` with all it's sub-classes
Adding `__` before the type's name will refer to the type of the class itself and not an object o that class