#ifndef DATA_TYPES_HEADER_FILE_H
#define DATA_TYPES_HEADER_FILE_H

#include "defs.h"

typedef enum {
    src_operand_type = 0,
    dst_operand_type = 1
} operand_type_e;

typedef enum {
    A = 0,
    E = 1,
    R = 2
} era_e;

/* Symbol Attributes */
typedef enum {
    code_attribute = 1 << 0,
    data_attribute = 1 << 1,
    entry_attribute = 1 << 2,
    external_attribute = 1 << 3
} symbol_attribute_e;

/* Symbol Entry for Symbol table*/
typedef struct {
    char* symbol_name;
    unsigned int        address;
    unsigned int        index;
    symbol_attribute_e  symbol_attributes;
} symbol_entry_s;

/* Addresing Types */
typedef enum {
    immediate_addressing_type = 0UL,
    direct_addressing_type = 1UL,
    jump_addressing_type = 2UL,
    register_direct_addressing_type = 3UL
} addressing_type_e;

#pragma pack(push, 1)

/* The packed structs representing the encoded source code lines */

/* Simple 14-bits word */
typedef struct {
    unsigned int data : WORD_SIZE_BITS;
} word_s;


typedef struct {
    unsigned int        era                    : 2;
    unsigned int        dst_addressing_type    : 2;
    unsigned int        src_addressing_type    : 2;
    unsigned int        opcode                 : 4;
    unsigned int        param2_addressing_type : 2;
    unsigned int        param1_addressing_type : 2;
} instruction_word_s;



/* ADDRESSING TYPES */

/* 0: Immediate Addressing - Integer Operand */
typedef struct {
    /* word */
    unsigned int era : 2;  /* A */
    unsigned int num : 12;
} instruction_immediate_addressing_integer_operand_s;


/* 1: Direct Addressing - Label Operand */
typedef struct {
    /* word */
    struct direct_addressing_address {
        unsigned int       era     : 2;  /* R ; E for external label */
        unsigned int       address : 12;
    } direct_addressing_address;
} instruction_direct_addressing_label_operand_s;


/* 2: Jump Params Addressing - Label Operand with Params */
typedef struct {
    union jump_params_addressing
    {
        /* word */
        struct jump_address {
            unsigned int       era : 2;
            unsigned int       address : 12;
        } jump_address;

        struct jump_param {
            /* word */
            unsigned int       era   : 2;
            unsigned int       param : 12;
        }jump_param;

        struct jump_2_params {
            /* word */
            unsigned int       era    : 2;
            unsigned int       param2 : 6;
            unsigned int       param1 : 6;
        }jump_params_2_words;

    } jump_params_addressing;
} instruction_jump_addressing_label_operand_s;


/* 3: Register Direct Addressing - Register Operand */
typedef struct {
    union direct_addressing_data
    {
        /* word */
        struct direct_addressing_dst {
            unsigned int       era      : 2;  /* R ; E for external label */
            unsigned int       reg_num  : 6;
            unsigned int       reserved : 6;
        } direct_addressing_dst;

        /* word */
        struct direct_addressing_src {
            unsigned int       era      : 2;  /* R ; E for external label */
            unsigned int       reserved : 6;
            unsigned int       reg_num  : 6;
        } direct_addressing_src;

        struct direct_addressing {
            unsigned int       era         : 2;  /* R ; E for external label */
            unsigned int       dst_reg_num : 6;
            unsigned int       src_reg_num : 6;
        } direct_addressing;
    } direct_addressing_data;
} instruction_direct_reg_addressing_label_operand_s;

#pragma pack(pop)

/* Macro entry for Macros DB */
typedef struct {
    char*          macro_name;
    char**         macro_lines;
    unsigned long  num_of_lines;
} macro_entry_s;

/* Operand info for source code line struct */
typedef struct {
    char*              operand_str;
    addressing_type_e  operand_addressing_type;
    unsigned int       word_address;
    operand_type_e     operand_type;
    word_s*            operand_word[MAX_ADDITIONAL_WORDS];
} operand_s;

/* Source code line struct */
typedef struct {
    char*         str;
    char*         formatted_str;
    unsigned int  line_num;

    unsigned int  DC;
    unsigned int  IC;
    unsigned int  L;
    word_s**      words;

    unsigned int  num_of_operands;
    operand_s     operands[MAX_INSTRUCTION_OPERANDS];
} src_line_s;

/* A linked-list node for storing the parsed data */
struct node_s {
    struct node_s* next;
    struct node_s* prev;
    union node_data
    {
        void*           raw_data;
        src_line_s*     data_src_line;
        macro_entry_s*  data_macro_entry;
        symbol_entry_s* data_symbol_entry;
    } node_data;
};

#endif
