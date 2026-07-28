# Embedding

you should find within your copy of **Bytescript** the following files :
- `bytescript.lib`
- `bytescript.h`

If you cannot find those you can get them within the bytescript github repo  

You may include the header `bytescript.h` in your program in order to get access to functions exported by bytescript.  
During compilation make sure to link your object files with `bytescript.lib`  

Most function available within `bytescript.h` are of type **CFunc** wich take an array of argument and an argument count  
`typedef TValue (*CFunc)(TValue* argv, size_t argc);`

These function are the same as those than can be found in [stdlib](stdlib.md), they follow the format **[function name]_[class_name]**
A list of non-CFuncs functions available in `bytescript.h` can be found [here](#bytescript-header)

**exemple** :  
```
#include "bytescript.h>
#include <stdio>

int main(int argc, char* argv[]) {
    TValue my_table = new_table(NULL, 0);
    TValue my_value = { .num = 5 };

    insert_table(&my_value, 1);
    print_table(&my_table, 1);
    return 0;
}
```

Should you plan on executing bytescript source code these function will  be needed to be called first :
- intialize_dependencies();
- initialize_type_specifiers();
- initialize_oop();
- initialize_globals();

Some features such as **strings** may not work if the necessary dependencies haven't been initialized  

# C-Modules creation

In order to create a bytescript module in c you will need to define and **export** two values :
- `char* bytescript_module_map;`
- `TValue bytescript_module_loader(TValue* argv, size_t argc);`

`bytescript_module_loader` is a **CFunc** that will be called when importing your module for the first time with 256 arguments corresponding to the 256 slots you get to store your module's value  

`bytescript_module_map` is the path to your module's **map**, your module's map should include the definition of variables, funtions, or other value that you will register in your module afterward  
Similarely to C headers, your module's map should **NOT** assign any of the values  
The order in wich you wrote your values in the module's map must be the same order you register them in  

You can then compile your module into a **shared library** (as of now, only the **dll** extention is recognized) and put it somewhere bytescript can find (stdlib, cwd)

**exemple :**
```C
// HEADER

#ifndef MY_HEADER
#define MY_HEADER

#include "bytescript.h"

extern char* bytescript_module_map;
TValue bytescript_module_loader(TValue* argv, size_t argc);

extern TValue my_cool_field;
TValue my_cool_function(TValue* argv, size_t argc);

#endif
```

```C
// MAIN FILE

#include "my_header.h"

TValue my_cool_field = { .num = 5 };

__declspec(dllexport) char* bytescript_module_map = "my_map.bs";
__declspec(dllexport) TValue bytescript_module_loader(TValue* argv, size_t argc) {
    TValue fields[] = {
        my_cool_field,
        new_cfunc(my_cool_function)
    };

    size_t field_count = sizeof(fields) / sizeof(fields[0]);
    memcpy(argv, fields, BSIZE(field_count));
}

TValue my_cool_function(TValue* argv, size_t argc) {
    // do stuff
}

```

```
Number myCoolField
func myCoolFunction(String stuffToDo) end
```

# Bytescript header

- *MIN_ARGC(n)*, macro wich returns NIL if there is less than **n** arguments in the current CFunc  

- *STRICT_MIN_ARGC(n, fname)*, macro wich calls an error if there is less than **n** arguments in the current CFunc, **fname** is the current function's name

- NUM_CHECK(n, fname), macro wich calls a type error if the **n**'th argument isn't a number, **fname** is the current function's name

- TYPE_CHECK(n, type, tnm, fname), macro wich calls a type error if the **n**'th arguments isn't of type **type**, **tnm** is the type's name, **fname** is the current function's name

- TValue easy_compile(char filename[]), compiles the given file
- TValue vm_run(TValue func), runs the given function using bvm

- TValue new_class(TValue inherited[], TValue fields[], size_t inherited_count, size_t field_count), creates a class, the arguments are self-explenatory
- TValue new_object(TValue cls, CFunc constructor, size_t fields_count), creates an object of class **cls**, the constructor is never used idk why it's here  
- TValue import_path_finder(TValue path), search for the given file in stdlib and the cwd

- int add_global(TValue value, TValue name), adds a global with the given name
- int overwrite_global(TValue value, TValue name), overwrites a global
- TValue get_global(TValue name), returns the asked global

- TValue make_string(char* string, size_t len), make a TStr with the given ptr and lenght
- TValue make_fstring(char* str), make a TStr out of a null terminated char[]

- void call_error(CFunc err_func, char details[]), calls an error using the given CFunc (the error's constructor) and details

#endif