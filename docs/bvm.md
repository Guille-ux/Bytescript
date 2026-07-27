# Bytescript Virtual Machine

*This page is not complete*

## Bytescript Value / TValue

Bytescript stores all its values using **NaN boxing**  
Therefore all bytescript values are represented by a double, a **TValue**.  

```
typedef union {
    uint64_t nan;
    double  num;
} TValue;
```

A TValue can take one of the following types :
- *Number*, represnted by a double
- *String*, **(!)** strings are interned and freeing them is not safe
- *Func*, the only executable to the virtual machine
- *Table*, array and hash-table
- *Class*, up to 256 fields
- *Object*, up to 256 fields
- *CData*, cannot be read the the virtual machine

see more about [*data types*](syntax.md#data-types)  

#### String

```
typedef struct Str {
    size_t len;
    char str[];
} TStr;
```

#### Func

```
typedef struct Func {
    uint8_t isCfunc;
    union {
        CFunc cfunc;
        struct {
            const uint8_t* bytecode;
            uint8_t const_len;
            const TValue constPool[];
        } bytescriptFunction;
    };
} TFunc;
```

`typedef TValue (*CFunc)(TValue* argv, size_t argc);`

#### Table

```
typedef struct Table {
    Array array;
    HashTable hash_table;
} TTable;
```

see the definitions of *Array* and *HashTable* in the header **bytev.h**  

#### Class & Object

```
typedef struct {
    size_t inherited_count;
    TValue fields[];
} TClass;

typedef struct {
    TClass* class;
    TValue fields[];
} TObject;
```

size_t and TClass* both being 64bit, a class tvalue can safely be read as an object  

#### CData

```
typedef struct {
    char* name;
    // add stuff later
} TCDataType;

typedef TCDataType* TCData;
#define CDATA_HEADER TCData cdata_type
```

see more about [embedding](embedding.md)  

## Constants

**Constants** are values that are resolved and stored into a *constant pool* at **compile time**  
Due to the *BVM* taking 8bit instructions, only up to 256 can be stored in a function's constant pool  

see wich [data types](syntax.md#data-types) are considered constants  