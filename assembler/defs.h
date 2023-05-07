#ifndef DEFS_HEADER_FILE_H
#define DEFS_HEADER_FILE_H


#define MEM_SIZE                     (256)

#define MAX_SRC_LINE_SIZE            (82)

#define WORD_SIZE_BITS               (14)

#define MAX_ADDITIONAL_WORDS         (3)

#define MAX_REGISTER_NAME_SIZE       (4)

#define MAX_INSTRUCTION_NAME_SIZE    (5)
#define MAX_INSTRUCTION_OPERANDS     (2)

#define INSTRUCTION_CODE_MEM_OFFSET  (100)

#define MAX_LABEL_LENGTH             (30)

#define MACRO_STR_NAME               ("mcr")
#define MACRO_STR_NAME_END           ("endmcr")


/* Machine instruction op-codes */
typedef enum {
    MOV  = 0,
    CMP  = 1,
    ADD  = 2,
    SUB  = 3,
    NOT  = 4,
    CLR  = 5,
    LEA  = 6,
    INC  = 7,
    DEC  = 8,
    JMP  = 9,
    BNE  = 10,
    RED  = 11,
    PRN  = 12,
    JSR  = 13,
    RTS  = 14,
    STOP = 15
} opcode_e;

/* Operand addressing types */
typedef enum {
    none                           = 0,        /* No Operands */
    immediate_addressing_type_bit  = 1 << 0,   /* 0: Immediate Addressing - Integer Operand */
    direct_addressing_type_bit     = 1 << 1,   /* 1: Direct Addressing - Label Operand */
    jump_addressing_type_bit       = 1 << 2,   /* 2: Jump Addressing - Jump Label Operand with Params */
    reg_direct_addressing_type_bit = 1 << 3    /* 3: Register Direct Addressing - Register Operand (no additional words are needed) */
} operand_addressing_type_e;

/* Struct register entry for s_registers look-up table */
typedef struct {
    char           reg[MAX_REGISTER_NAME_SIZE];
    unsigned int   reg_num : 4;
} regs_s;

/* Machine instruction entry for instruction look-up table */
typedef struct
{
    char           name[MAX_INSTRUCTION_NAME_SIZE];
    opcode_e       opcode;
    unsigned int   num_of_operands;
    unsigned int   src_valid_addressing_type : 4;
    unsigned int   dst_valid_addressing_type : 4;
} instruction_info_entry_s;

#endif
