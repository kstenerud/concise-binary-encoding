%{

#include <stdio.h>
#include "cte_internal.h"
#include "parser.h"

#if YYBISON
union YYSTYPE;
int yylex(union YYSTYPE *, void *);
#endif


%}

%define api.pure full
%define parse.error verbose

%lex-param { void *scanner }

%parse-param { void *scanner }
%parse-param { cte_real_decode_process* process }

%union {
    char* string_v;
}

%type <string_v> TOKEN_INTEGER_2 TOKEN_INTEGER_8 TOKEN_INTEGER_10 TOKEN_INTEGER_16 TOKEN_FLOAT TOKEN_DECIMAL TOKEN_TIME_DD TOKEN_TIME_DDD TOKEN_STRING TOKEN_UNEXPECTED TOKEN_BAD_DATA string

%token TOKEN_TRUE TOKEN_FALSE TOKEN_FLOAT TOKEN_DECIMAL TOKEN_STRING TOKEN_TIME_DD TOKEN_TIME_DDD TOKEN_EMPTY TOKEN_UNEXPECTED TOKEN_BAD_DATA
%token TOKEN_INTEGER_2 TOKEN_INTEGER_8 TOKEN_INTEGER_10 TOKEN_INTEGER_16
%token TOKEN_MAP_START TOKEN_MAP_END TOKEN_LIST_START TOKEN_LIST_END TOKEN_ASSIGNMENT_SEPARATOR
%token TOKEN_ARRAY_BOOLEAN_START TOKEN_ARRAY_INT_8_START TOKEN_ARRAY_INT_16_START TOKEN_ARRAY_INT_32_START
%token TOKEN_ARRAY_INT_64_START TOKEN_ARRAY_INT_128_START TOKEN_ARRAY_FLOAT_32_START TOKEN_ARRAY_FLOAT_64_START
%token TOKEN_ARRAY_FLOAT_128_START TOKEN_ARRAY_DECIMAL_32_START TOKEN_ARRAY_DECIMAL_64_START TOKEN_ARRAY_DECIMAL_128_START
%token TOKEN_ARRAY_TIME_START TOKEN_ARRAY_END

%start object

%%

array_object:
      TOKEN_TRUE       { if(!process->parse_callbacks.on_true(process)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_FALSE      { if(!process->parse_callbacks.on_false(process)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_INTEGER_2  { if(!process->parse_callbacks.on_int(process, 2, $1)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_INTEGER_8  { if(!process->parse_callbacks.on_int(process, 8, $1)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_INTEGER_10 { if(!process->parse_callbacks.on_int(process, 10, $1)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_INTEGER_16 { if(!process->parse_callbacks.on_int(process, 16, $1)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_FLOAT      { if(!process->parse_callbacks.on_float(process, $1)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_DECIMAL    { if(!process->parse_callbacks.on_decimal(process, $1)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_TIME_DD    { if(!process->parse_callbacks.on_time(process, 2, $1)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_TIME_DDD   { if(!process->parse_callbacks.on_time(process, 3, $1)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }

object:
      array_object
    | string           { if(!process->parse_callbacks.on_string(process, $1)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_EMPTY      { if(!process->parse_callbacks.on_empty(process)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | list
    | map
    | array
    | TOKEN_UNEXPECTED {
        char buff[1000];
        snprintf(buff, sizeof(buff), "Unexpected token: %s", $1);
        buff[sizeof(buff)-1] = 0;
        process->parse_callbacks.on_error(process, buff);
        return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
    }
    | TOKEN_BAD_DATA {
        char buff[1000];
        snprintf(buff, sizeof(buff), "Bad encoding: %s", $1);
        buff[sizeof(buff)-1] = 0;
        process->parse_callbacks.on_error(process, buff);
        return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK;
    }

string: TOKEN_STRING

array:  array_start array_entries array_end

array_start:
      TOKEN_ARRAY_BOOLEAN_START     { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_BOOLEAN_8)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_INT_8_START       { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_INT_8)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_INT_16_START      { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_INT_16)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_INT_32_START      { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_INT_32)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_INT_64_START      { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_INT_64)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_INT_128_START     { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_INT_128)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_FLOAT_32_START    { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_FLOAT_32)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_FLOAT_64_START    { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_FLOAT_64)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_FLOAT_128_START   { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_FLOAT_128)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_DECIMAL_32_START  { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_DECIMAL_32)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_DECIMAL_64_START  { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_DECIMAL_64)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_DECIMAL_128_START { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_DECIMAL_128)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }
    | TOKEN_ARRAY_TIME_START        { if(!process->parse_callbacks.on_array_begin(process, CTE_TYPE_TIME_64)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }

array_end: TOKEN_ARRAY_END { if(!process->parse_callbacks.on_array_end(process)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }

array_entries: /* empty */
    | array_object
    | array_entries array_object

list:  list_start list_entries list_end

list_start: TOKEN_LIST_START { if(!process->parse_callbacks.on_list_begin(process)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }

list_end: TOKEN_LIST_END { if(!process->parse_callbacks.on_list_end(process)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }

list_entries: /* empty */
    | object
    | list_entries object

map: map_start map_entries map_end

map_start: TOKEN_MAP_START { if(!process->parse_callbacks.on_map_begin(process)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }

map_end: TOKEN_MAP_END { if(!process->parse_callbacks.on_map_end(process)) return CTE_DECODE_STATUS_STOPPED_IN_CALLBACK; }

map_tuple: object TOKEN_ASSIGNMENT_SEPARATOR object

map_entries: /* empty */
    | map_tuple
    | map_entries map_tuple

%%

int yylex(YYSTYPE *, void *);
