#include "Json/tJson.h"
#include <ctype.h>
#include <stdio.h>

__declspec(dllexport) char* bytescript_module_map = "Json_map.bs";
__declspec(dllexport) TValue bytescript_module_loader(TValue* argv, size_t argc) {
    TValue fields[] = {
        new_cfunc(parse_json),
        new_cfunc(stringify_json),
        new_cfunc(valid_json),
        new_cfunc(load_json),
        new_cfunc(write_json),
    };

    size_t field_count = sizeof(fields) / sizeof(fields[0]);
    memcpy(argv, fields, BSIZE(field_count));
}


TValue parse_json(TValue* argv, size_t argc) {
    MIN_ARGC(1); TYPE_CHECK(0, STRING, "String", "Json.parse");
    TStr* str = AS_STRING(*argv);
    char* ptr = str->str;
    
    return parse_json_value(&ptr);
}

TValue stringify_json(TValue* argv, size_t argc) {

}

TValue valid_json(TValue* argv, size_t argc) {

}

TValue load_json(TValue* argv, size_t argc) {

}

TValue write_json(TValue* argv, size_t argc) {

}


TValue parse_json_value(char** str) {
    switch (**str) {
        case '{': (*str)++; return parse_json_object(str);
        case '[': (*str)++; return parse_json_array(str);
        case 't': {
            TValue word = get_next_word(str);
            *str += AS_STRING(word)->len + 1;

            return !strcmp(AS_STRING(word)->str, "true") ? word : NIL;
        }

        case 'f': {
            TValue word = get_next_word(str);
            *str += AS_STRING(word)->len + 1;

            return !strcmp(AS_STRING(word)->str, "false") ? word : NIL;
        }

        case 'n': {
            TValue word = get_next_word(str);
            *str += AS_STRING(word)->len + 1;

            return !strcmp(AS_STRING(word)->str, "null") ? word : NIL;
        }

        case '"': return make_string_json(str, '"');
        default: {
            if (isdigit(**str) || **str == '+' || **str == '-') {
                char* end;
                double res = strtod(*str, &end);

                *str = end;
                return NUM_AS_TVALUE(res);
            }

            return NIL;
        }
    }
}

TValue parse_json_object(char** str) {
    TValue new = new_table(NULL, 0);
    TValue current;

    next_value(str);
    while (!IS_NIL(current = parse_json_value(str))) {
        if (!IS_STRING(current)) call_error(new_syntax_error, "Expected a string as object key. in Json"); next_value(str);
        if (**str != ':') call_error(new_syntax_error, "Expected ':' after a key. in Json"); (*str)++; next_value(str);

        TValue value = parse_json_value(str); next_value(str);
        append_hash_table((TValue[3]){ new, current, value }, 3);

        if (**str != ',') break; else { (*str)++; next_value(str); }
    }

    if (**str != '}') call_error(new_syntax_error, "Expected '}' to end the object. in Json"); (*str)++;
    return new;
}

TValue parse_json_array(char** str) {
    TValue new = new_table(NULL, 0);
    TValue current;

    next_value(str);
    while (!IS_NIL(current = parse_json_value(str))) {
        insert_table((TValue[2]){ new, current }, 2); next_value(str);
        if (**str != ',') break; else { (*str)++; next_value(str); }
    }

    if (**str != ']') call_error(new_syntax_error, "Expected ']' to end the array. in Json"); (*str)++;
    return new;
}


TValue make_string_json(char** text, char initializer) {
    char* pre_text = *text;
    size_t len = 0;
    size_t full = 0;
    
    while (*++pre_text != initializer && *pre_text) len++;
    char* buff = malloc(len + 1);

    if (buff == NULL) call_error(new_failed_allocation_error, "Failed allocation while making string");
    while (*++*text != initializer && **text) {
        char to_add = **text;

         if (**text == '\\') {
            char to_add;
            
            switch (*++*text) {
                case 'n': to_add = '\n'; break;
                case 't': to_add = '\t'; break;
                default : break;
            }
        }

        buff[full] = to_add;
        full++;
    }

    buff[full] = '\0'; (*text)++;
    TValue str =  make_string(buff, full); free(buff);
    return str;
}

TValue get_next_word(char** str) {
    size_t len = 0;
    while (*++*str != ' ') len++;

    return make_string(*str, len);
}

void next_value(char** str) {
    while (isspace(**str) || **str == '\n' || **str == '\t' || **str == '\r') (*str)++;
}