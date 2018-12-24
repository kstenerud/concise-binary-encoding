#pragma once
#ifdef __cplusplus 
extern "C" {
#endif

#include "cte/cte.h"

typedef enum
{
    // enum value & 0xff == sizeof(type)
    CTE_TYPE_NONE         = 0,
    CTE_TYPE_BOOLEAN_8    = 0x100 + sizeof(bool),
    CTE_TYPE_INT_8        = 0x200 + sizeof(int8_t),
    CTE_TYPE_INT_16       = 0x300 + sizeof(int16_t),
    CTE_TYPE_INT_32       = 0x400 + sizeof(int32_t),
    CTE_TYPE_INT_64       = 0x500 + sizeof(int64_t),
    CTE_TYPE_INT_128      = 0x600 + sizeof(__int128),
    CTE_TYPE_FLOAT_32     = 0x700 + sizeof(float),
    CTE_TYPE_FLOAT_64     = 0x800 + sizeof(double),
    CTE_TYPE_FLOAT_128    = 0x900 + sizeof(__float128),
    CTE_TYPE_DECIMAL_32   = 0xa00 + sizeof(_Decimal32),
    CTE_TYPE_DECIMAL_64   = 0xb00 + sizeof(_Decimal64),
    CTE_TYPE_DECIMAL_128  = 0xc00 + sizeof(_Decimal128),
    CTE_TYPE_TIME_64      = 0xd00 + sizeof(smalltime),
} cte_data_type;

typedef struct
{
    void (*on_error)             (struct cte_decode_process* decode_process, const char* message);
    bool (*on_empty)             (struct cte_decode_process* decode_process);
    bool (*on_true)              (struct cte_decode_process* decode_process);
    bool (*on_false)             (struct cte_decode_process* decode_process);
    bool (*on_int)               (struct cte_decode_process* decode_process, int base, char* value);
    bool (*on_float)             (struct cte_decode_process* decode_process, char* value);
    bool (*on_decimal)           (struct cte_decode_process* decode_process, char* value);
    bool (*on_time)              (struct cte_decode_process* decode_process, int day_digits, char* value);
    bool (*on_list_begin)        (struct cte_decode_process* decode_process);
    bool (*on_list_end)          (struct cte_decode_process* decode_process);
    bool (*on_map_begin)         (struct cte_decode_process* decode_process);
    bool (*on_map_end)           (struct cte_decode_process* decode_process);
    bool (*on_string)            (struct cte_decode_process* decode_process, char* value);
    bool (*on_binary_data_begin) (struct cte_decode_process* decode_process);
    bool (*on_binary_data_end)   (struct cte_decode_process* decode_process);
} internal_parse_callbacks;



struct cte_decode_process
{
    internal_parse_callbacks parse_callbacks;
    const cte_decode_callbacks* callbacks;
    int document_depth;
    void* user_context;
    void* scanner;
};
typedef struct cte_decode_process cte_decode_process;

void yyerror(const void* const scanner, struct cte_decode_process* decode_process, const char *msg);


#ifdef __cplusplus 
}
#endif