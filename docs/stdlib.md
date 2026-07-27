# Bytescript Standard library

## Built-ins

Built-ins modules do not need to be imported by the user and can be accessed at any time  
These include :
- String
- Table
- IO
- File
- Error

### Table

- `Table:new()`  
**Signature** : `func static Table __new() end`  
**Description** : Creates an empty table and returns it  
**Arguments** : None  
  
- `Table:free()`  
**Signature** : `func Table free() end`  
**Description** : Frees a table  
**Arguments** : None  
  
- `Table:pop(Number index)`  
**Signature** : `func pop(Number index) end`  
**Description** : Creates an empty table  
**Arguments** :
    - `index` : The index of the value you wish to remove from the table, **optional**, *defaults to the last index*
  
- `Table:shrink()`  
**Signature** : `func Table shrink() end`  
**Description** : Shrinks a table's capacity  
**Arguments** : None  
  
- `Table:expand()`  
**Signature** : `func Table expand() end`  
**Description** : Expands a table's capacity  
**Arguments** : None  
  
- `Table:len()`  
**Signature** : `func Number len() end`  
**Description** : Returns the lenght of the table  
**Arguments** : None  
  
- `Table:reserve(Number amount)`    
**Signature** : `func Number reserve(Number amount) end`  
**Description** : Expands a table's capacity to match your needs  
**Arguments** :
    - `amount` : The amount of value you wish to reserve
  
- `Table:print()`  
**Signature** : `func Table print() end`  
**Description** : Nicely prints a table  
**Arguments** : None  
  
- `Table:slice(Number start, Number end)`  
**Signature** : `func Table slice(Number start, Number end) end`  
**Description** : Returns a sub-table of the original table  
**Arguments** : 
    - `start` : The index at wich you wish to begin the sub-table
    - `end` : the index at wich you wish to end the sub-table, must be bigger than `start` 
  
- `Table:fill(value)`  
**Signature** : `func Table fill(value) end`  
**Description** : Fills a table with the given value  
**Arguments** : 
    - `value` : the value to fill the table with
  
- `Table:copy()`  
**Signature** : `func Table copy() end`  
**Description** : Creates a copy of the table and returns it  
**Arguments** : None  
  
- `Table:insert(value, Number idx)`  
**Signature** : `func insert(value, Number idx) end`  
**Description** : Add a value to the table's array part at a given index  
**Arguments** : 
    - `value` : The value to add to the table
    - `idx` : The index at wich you wish to add the value, **optional** defaults to the last index
  
- `Table:appendHash(key, value)`  
**Signature** : `func appendHash(key, value) end`  
**Description** : Add a value to the table's hash-table/dictionnary part  
**Arguments** : 
    - `key` : The key to store the value under
    - `value` : The value to store
  
- `Table:get(key)`  
**Signature** : `func get(key) end`  
**Description** : Gets the value corresponding to the given key from the table's hash-table/dictionnary part, returns NIL if not found  
**Arguments** : 
    - `key` : the key that the value was stored under
  
- `Table:clean()`  
**Signature** : `func Table clean() end`  
**Description** : Cleans the table by removing its elements  
**Arguments** : None  
  
- `Table:reset()`  
**Signature** : `func Table reset() end`  
**Description** : Resets a table by allocating a brand new array and hash-table  
**Arguments** : None  
  
- `Table:hashedKeys()`  
**Signature** : `func Table hashedKeys() end`  
**Description** : Returns all the keys stored in the table's hash-table/dictionnary  
**Arguments** : None  
  
- `Table:hashedValues()`  
**Signature** : `func Table hashedValues() end`  
**Description** : Returns all the values stored in the table's hash-table/dictionnary  
**Arguments** : None  
  
- `Table:buckets()`  
**Signature** : `func Table buckets() end`  
**Description** : Returns all the buckets (key, value) stored in the table's hash-table/dictionnary  
**Arguments** : None  
  
- `Table:inArray()`  
**Signature** : `func Number inArray(value) end`  
**Description** : Checks if a value is in the table's array part  
**Arguments** : None  
  
- `Table:hashValue(value)`  
**Signature** : `func static hashValue(value) end`  
**Description** : Returns the hashed version of the value  
**Arguments** : 
    - `value` : The value to hash
  
  
### String

- `String:compare(String other)`  
**Signature** : `func Number compare(String other) end`  
**Description** : Compares the string with another one  
**Arguments** : 
    - `other` : The string to compare with

- `String:format(var value)`  
**Signature** : `func Number format(var value) end`  
**Description** : Formats a string using format specifiers  
**Arguments** : 
    - `value` : The value to format

- `String:toString(var value)`  
**Signature** : `func static String toString(var value) end`  
**Description** : Turn a value into a string  
**Arguments** : 
    - `value` : The value to cast into a string


### IO

- `IO.stdout`
**Description** : A File wrapper to the standard output  

- `IO:open(String filename, String openType)`  
**Signature** : `func static File open(String filename, String openType) end`  
**Description** : opens a file with the specified openType, returns a File object  
**Arguments** : 
    - `filename` : The path to the file
    - `openType` : The open type to open the file with


### File

- `File:read()`  
**Signature** : `func String read() end`  
**Description** : Reads a file and returns a string with its content  
**Arguments** : None

- `File:write(String content)`  
**Signature** : `func write(String content) end`  
**Description** : Writes content into a file  
**Arguments** : 
    - `content` : The content to write into the file

- `File:close()`  
**Signature** : `func close() end`    
**Description** : Closes the file  
**Arguments** : None


## Standard library

Modules that needs to be imported, they come with bytescript in the `stdlib` folder  

