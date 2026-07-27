# Syntax

#### statement

A bytescript file may contain any amount of **statement** separated either by a *newline* or a *semicolon*  
A statement may take one of the following form:
- [if statement](#if-statement)
- [for loop](#for-loop)
- [while loop](#while-loop)
- [repeat loop](#repeat-loop)
- [import statement](#import-statement)
- [class definition](#class-definition)
- [return statement](#return-statement)
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

A variable can be **defined** with the syntax : `[data type] [identifier] = [data]`  
An identifier can either start with a *letter* or `_`, and can contain any *letter*, *number*, or `_`  
The specified data must have the same *data type* as the specified [*data type*](#data-types) or the type `var`  

A variable may be defined without value using the syntax `[data type] [identifier]`  
The variable will contain **garbage value** and should not be used until it has been [assigned](#assignement) a value  

see more about [data types](#data-types)  

# Assignement

You can **assign** a value to an lvalue (usaly variables) with the syntax : `[lvalue] = [data]`  
The data must have the same [*data type*](#data-types) as the one that was specified during declaration, or `var`  

see more about [lvalues](#lvalues)  

### Lvalues

In bytescript, lvalues correspond to either variables or  table indexes

# If statement

An if statement can be made using the syntax : `if [condition] then [body] end`  
An if statement's condition must be an [operation](#operations) or a [numeric value](#numeric-values)  
An if statement's body can contain any amount of [statement](#statement) excluding [class definitions](#class-definitions)  

# For loop

Bytescript has two type of syntaxes for **for loops**  

#### numeric for loops

A numeric for loop can be made using the syntax : `for [identifier] = [start], [end], [step]? repeat [body] end`  
A numeric for loop's body can contain any amount of [statement](#statement) excluding [class definitions](#class-definitions)  
The *[start]* value is used to initialize the variable *[identifier]*, it must be a [Number](#number)  
The *[step]* is optional and defaults to 1

#### generic for loops

A generic for loop can be made using the syntax : `for [identifier], [identifier]? in [target] repeat [body] end`  
A generic for loop's body can contain any amount of [statement](#statement) excluding [class definitions](#class-definitions)  
A generic for loop's target must have a specified [data type](#data-types) with an *__iter* method, if no data type is specified, bytescript will default to treating your value as a table  
The first identifier will correspond to the current **state** of iteration  
The second (optional) identifier will correspond to the value returned by the target's iterator

# While loop

A while loop can be made using the syntax : `while [condition] repeat [body] end`  
A while loop's condition must be an [operation](#operations) or a [numeric value](#numeric-values)  
A While loop's body can contain any amount of [statement](#statement) excluding [class definitions](#class-definitions)  

# Repeat loop

A repeat loop can be made using the syntax : `repeat [body] while [condition] end`  
A repeat loop work the same way as a [while loop](#while-loop), however its body is guarenteed to be ran once before the condition is evaluated  
A repeat loop is the equivalent to `do { [body] } while [condition];` in many languages

# Import statement

An import statement can be made using the syntax : `import "[path]" as [identifier] end`  
The *[path]* correspond to the module's path, either relative to the current working directory or within the [standard library](stdlib.md)  
The *[identifier]* will contain the **module object**, the module's fields will correspond to the identifiers defined at the top level of the module  

You may import individual fields from the module using the syntax : `from "[path]" import [fields] as [nickname]? end`  
The *[fields]*'s [data types](#data-types) may be specified  
The *[nickname]* is optional  

# Class definition

A class can be made using the syntax : `class [identifier] [body] end`  
Defining a class will register the class object as a global under the name *[identifier]*, defining a class will also register two [data types](#data-types), *[identifier]* will correspond to an **object** of that class, and __*[identifier]* will correspond to the class itself, this type should preferably only be owned by the global defined earlier  

You cannot name a class with an identifier starting with `__`  

A class may contain [class fields](#field-definition), as well as **methods** wich can be declared by using the [function definition syntax](#function) at the top level of the class definition's body  

Class fields are owned by the class object itself and not given to the individual objects  
**Object fields** can be declared using the same syntax as [class fields](#field-definition) inside a method, preferably every object fields should be declared inside the class's [*constructor*](#constructor)  

Methods may include a modifier using the syntax : `func [modifier] [data type]? identifier([arguments]) [body] end`, at the moment the only modifier is `static` wich allows the method to be called without object  
Non-static method will have `self` as their first argument (automaticaly done by the compiler) corresponding to the object the method was called with  

Some methods such as a [*constructor*](#constructor) are known by the bytescript compiler and may result in special behavior  
see more about [these function](#native-functions)  

### constructor

A class's constructor must follow the signature : `func static [class name] __new([arguments]) end`  
The compiler will emit an object creation instruction at the top of this function  
Unlike other **static methods**, a constructor has a **self** argument corresponding to the created object  

# Return statement

A return statement can be made using the syntax : `return [value]`  
The returned value's type must correspond to the current function's [return type](#data-types)  

If a return statement is put at the file's top level the return value's type must correspond to the data type it is [required](#require-expression) with  

# Allocation statement

An allocation statement can be made using the syntax : `alloc [target] -> [values] end`, where each value is separated by a comma  
Allocation statements act as a shortcut to table:insert(), therefore the target **must** be a table  

# Break statement

A break statement can be made using the syntax : `break` (shocking ik)  
It breaks the 4th wall  

# Continue statement

A continue statement can be made using the syntax : `continue`  
(why are u even reading that?)  

# Field definition

A field definition can be made using the syntax : `[data type] self.[identifier] = [data]`  
Similarely to variable it can be defined without value, leading to **garbage value**  
A field definition will register the identifier under the type it is currently operating under, see [class definitions](#class-definition)  

# Require expression

A require expression can be made using the syntax : `require "[path]" as [data type] [identifier] end`  
The [data type](#data-types) and identifier are optional, therefore the following syntax is correct : `require "[path]"`  
A require expression will [import](#import-statement) the specified path and return the module's returned value  

# Operations

An operation can be done between two [numeric values](#numeric-values) using an operator  
The following operators exist in bytescript :
- *PLUS* (+)
- *MINUS* (-)
- *STAR* (*)
- *SLASH* (/), (//) for floor division
- *MODULO* (%)
- *POWER* (^)
- *CONCAT* (..), shortcut to string:concat()
- *BOOLEANS* (and, or, ||, &&)
- *COMPARAISONS* (==, !=, <, >, <=, >=)
- *BITWISE* (&, |)
- [*POSTFIX*](#postfix-operations)

The order of priority is the following :
- *POSTFIX*
- *ARITHMETICS*
- *POWER*
- *COMPARAISONS*
- *BOOLEAN*

### Unary operations

There are three unary operations in bytescript, -[value], +[value], **not** [value], wich may also be written !value  

### Postfix operations

There are three postfix operations in bytescript  

#### Calls

Calls can be made using the syntax : `[function]([arguments])`  

#### Indexing

Indexing can be done using the syntax : `[target][[value]]` / `target[value]` beacause the first version wasn't readable  

#### Method calls

Method calls can be made using the syntax : `[object]:[method name]([arguments])`  
Method calls help the compiler figure out the method's signature  

#### Field accessing

You can access a class or object's fields using the syntax : `[target].[field]`  

# Data types

A **data type** refer to a [class](#class-definition), either defined by the user, external libraries, or built inside the language  
The following data type are built-in and can be used in any bytescript program :
- `Number`, may be shortened to `num`
- `String`, may be shortened to `str`
- `Table`, may be shortened to `table`
- `Function`
- `CData`, see more about [embedding](embedding.md)
- `File`

Excluding classes wich does not serve the purpose of being a data type such as `IO`, `Module`, and `Error` with all it's sub-classes  
Adding `__` before the type's name will refer to the type of the class itself and not an object of that class  

see more about how bytescript stores [*data*](bvm.md#bytescript-value--tvalue)  

#### Number

A number can take up to 15 digits of decimal precision  
A number is a [*constant*](bvm.md#constants)  

#### String

A string can be created using the syntax : `"[content]"`  
A string is a [*constant*](bvm.md#constants)  

#### Table

A table can be created using the syntax : `{ [elements] }`  
The specified *elements* will be stored in the table's *array* part  

A syntax allowing to specify the elements to store in the *hash-table* part is planned  

#### Function

A function can be created using the syntax : `func [data type]? identifier([arguments]) [body] end`  
A function body can contain any amount of [statement](#statement) excluding [class definitions](#class-definitions)  
A function can optionaly give a **return type** by specifying a [data type](#data-types)  
A function's arguments may also specify a data type  
A function is a [*constant*](bvm.md#constants)  

#### File

The `File` class is defined in the `IO` module  
see more about bytescript's [standard library](stdlib.md)  