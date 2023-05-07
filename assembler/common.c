#include "common.h"
#include "defs.h"
#include "types.h"
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>



/* Machine instruction look-up table */
static instruction_info_entry_s s_instructions[] = { { "mov",  MOV,  2, (immediate_addressing_type_bit | direct_addressing_type_bit | jump_addressing_type_bit | reg_direct_addressing_type_bit), (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "cmp",  CMP,  2, (immediate_addressing_type_bit | direct_addressing_type_bit | jump_addressing_type_bit | reg_direct_addressing_type_bit), (immediate_addressing_type_bit | direct_addressing_type_bit  | jump_addressing_type_bit | reg_direct_addressing_type_bit) },
                                                     { "add",  ADD,  2, (immediate_addressing_type_bit | direct_addressing_type_bit | jump_addressing_type_bit | reg_direct_addressing_type_bit), (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "sub",  SUB,  2, (immediate_addressing_type_bit | direct_addressing_type_bit | jump_addressing_type_bit | reg_direct_addressing_type_bit), (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "not",  NOT,  1, (none),                                                                                                                   (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "clr",  CLR,  1, (none),                                                                                                                   (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "lea",  LEA,  2, (direct_addressing_type_bit | jump_addressing_type_bit),                                                                  (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "inc",  INC,  1, (none),                                                                                                                   (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "dec",  DEC,  1, (none),                                                                                                                   (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "jmp",  JMP,  1, (none),                                                                                                                   (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "bne",  BNE,  1, (none),                                                                                                                   (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "red",  RED,  1, (none),                                                                                                                   (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "prn",  PRN,  1, (none),                                                                                                                   (immediate_addressing_type_bit | direct_addressing_type_bit  | jump_addressing_type_bit | reg_direct_addressing_type_bit) },
                                                     { "jsr",  JSR,  1, (none),                                                                                                                   (direct_addressing_type_bit    | jump_addressing_type_bit    | reg_direct_addressing_type_bit) },
                                                     { "rts",  RTS,  0, (none),                                                                                                                   (none) },
                                                     { "stop", STOP, 0, (none),                                                                                                                   (none) } };

/* Registers look-up table */
static regs_s s_regs[] = { { "r0",  0  },
                           { "r1",  1  },
                           { "r2",  2  },
                           { "r3",  3  },
                           { "r4",  4  },
                           { "r5",  5  },
                           { "r6",  6  },
                           { "r7",  7  } };


/* Clear white spaces and tabs from the given string */
void clear_white_spaces_and_tabs(char* line)
{
    if (line == NULL)
    {
        return;
    }

    /* When string is empty, return */
    if (line[0] == '\0')
    {
        return;
    }

    /* if the adjacent characters are same */
    if (((line[0] == ' ') && (line[1] == ' ')) ||
        ((line[0] == '\t') && (line[1] == '\t')) ||
        ((line[0] == ' ') && (line[1] == '\t')) ||
        ((line[0] == '\t') && (line[1] == ' ')) ||
        ((line[0] == ' ') && (line[1] == ',')) ||
        ((line[0] == '\t') && (line[1] == ',')) ||
        ((line[0] == ',') && (line[1] == ' ')) ||
        ((line[0] == ',') && (line[1] == '\t')))
    {

        int i = 0;
        if (((line[0] == ',') && (line[1] == ' ')) ||
            ((line[0] == ',') && (line[1] == '\t')))
        {
            i = 1;
        }

        /* Shift character by one to left */
        while (line[i] != '\0')
        {
            line[i] = line[i + 1];
            i++;
        }

        /* Check on Updated String line */
        clear_white_spaces_and_tabs(line);
    }

    /* If the adjacent characters are not same
       Check from line+1 string address */
    clear_white_spaces_and_tabs(line + 1);
}


/* Remove leading & trailing white spaces and tabs from the given string */
void clear_leading_trailing_white_spaces_and_tabs(char* formatted_str_line)
{
    char temp[MAX_SRC_LINE_SIZE] = { 0 };
    char* end = NULL;
    char* temp_str = formatted_str_line;

    /* Trim leading space */
    while ((*temp_str == ' ') || (*temp_str == '\t'))
    {
        temp_str++;
    }

    /* All spaces */
    if (*temp_str == '\0')
    {
        formatted_str_line[0] = '\0';
        return;
    }

    /* Trim trailing space */
    end = temp_str + strlen(temp_str) - 1;
    while ((end > temp_str) && ((*end == ' ') || (*end == '\t')))
    {
        end--;
    }

    /* Write new null terminator character */
    end[1] = '\0';

    strcpy(temp, temp_str);
    strcpy(formatted_str_line, temp);
}


/* Generate formatted source code string line */
void create_formated_line(char* str_line, char* formatted_str_line)
{
    if ((str_line == NULL) || (formatted_str_line == NULL))
    {
        return;
    }

    strcpy(formatted_str_line, str_line);
    clear_leading_trailing_white_spaces_and_tabs(formatted_str_line);

    if (strlen(formatted_str_line) > 1)
    {
        clear_white_spaces_and_tabs(formatted_str_line);
    }
}

/* Search for the given string register name in the register look-up table */
regs_s* is_register_name(char* str)
{
    regs_s* reg = NULL;
    int i;

    if (str == NULL)
    {
        return reg;
    }

    for (i = 0; i < (sizeof(s_regs)/sizeof(s_regs[0])); ++i)
    {
        if (!strcmp(str, s_regs[i].reg))
        {
            reg = &(s_regs[i]);
            break;
        }
    }

    return reg;
}

/* Search for the given string instruction in the instruction look-up table */
instruction_info_entry_s* find_instructions(char* token)
{
    instruction_info_entry_s* instruction_info_entry = NULL;
    int i;

    if (token == NULL)
    {
        return NULL;
    }

    for (i = 0; i < (sizeof(s_instructions) / sizeof(s_instructions[0])); ++i)
    {
        instruction_info_entry_s* temp_entry = &s_instructions[i];
        if (!strcmp(temp_entry->name, token))
        {
            instruction_info_entry = temp_entry;
            break;
        }
    }

    return instruction_info_entry;
}

/* Search for the given Symbol in the Symbol linked-list */
symbol_entry_s* find_symbol(struct node_s* symbols, char* symbol_name)
{
    symbol_entry_s* ret_val = NULL;
    struct node_s* symbol = symbols;

    if ((symbols == NULL) || (symbol_name == NULL))
    {
        return NULL;
    }

    while (symbol)
    {
        symbol_entry_s* symbol_entry = symbol->node_data.data_symbol_entry;
        if (!strcmp(symbol_entry->symbol_name, symbol_name))
        {
            ret_val = symbol_entry;
            break;
        }

        symbol = symbol->next;
    }

    return ret_val;
}

/* Validate number syntax and return the appropriate value */
int generate_signed_number(char* str_operand, short* immediate_value)
{
    long value;
    char* endptr = NULL;

    if ((immediate_value == NULL) || (str_operand == NULL))
    {
        return 0;
    }

    value = strtol(str_operand, &endptr, 10);
    if (*endptr != '\0' || (str_operand[0] == *endptr))
    {
        return 0;
    }

    if (((value > SHRT_MAX)) || ((value < SHRT_MIN)))
    {
        return 0;
    }

    *immediate_value = (short)value;
    return 1;
}

/* Return the filename from a given full path */
char* get_file_name(char* file_path)
{
    size_t file_path_len;
    size_t idx;

    if (file_path == NULL)
    {
        return NULL;
    }

    file_path_len = strlen(file_path);
    idx = file_path_len - 1;

    while ((file_path[idx] != '/') && (idx != 0))
    {
        --idx;
    }

    return &file_path[((idx == 0) ? (0) : (idx + 1))];
}

int parse_jump_addressing(char* param, char* jump_param1, char* jump_param2, char* jump_param3)
{
    char temp_param[MAX_SRC_LINE_SIZE] = { 0 };
    char* token = temp_param;
    int i, j, k;

    if (param && jump_param1 && jump_param2 && jump_param3)
    {
        strcpy(temp_param, param);

        for (i = 0; i < strlen(param); ++i)
        {
            if (temp_param[i] == '(')
            {
                temp_param[i] = '\0';
                strcpy(jump_param1, token);
                token = &temp_param[i + 1];

                for (j = i; j < strlen(param); ++j)
                {
                    if (temp_param[j] == ',')
                    {
                        temp_param[j] = '\0';
                        strcpy(jump_param2, token);
                        token = &temp_param[j + 1];

                        for (k = i; k < strlen(param); ++k)
                        {
                            if (temp_param[k] == ')')
                            {
                                temp_param[k] = '\0';
                                strcpy(jump_param3, token);
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

/* Free memory linked-list lines */
void free_lines_mem(struct node_s* lines_head_node)
{
    int i;

    while (lines_head_node)
    {
        struct node_s* temp_node = lines_head_node;
        lines_head_node = lines_head_node->next;
        free(temp_node->node_data.data_src_line->str);

        for (i = 0; i < temp_node->node_data.data_src_line->L; ++i)
        {
            free(temp_node->node_data.data_src_line->words[i]);
        }

        if (temp_node->node_data.data_src_line->L != 0)
        {
            free(temp_node->node_data.data_src_line->words);
        }


        for (i = 0; i < temp_node->node_data.data_src_line->num_of_operands; ++i)
        {
            free(temp_node->node_data.data_src_line->operands[i].operand_str);
        }

        free(temp_node->node_data.data_src_line);
        free(temp_node);
    }
}

/* Free memory linked-list Symbols */
void free_symbols_mem(struct node_s* symbols_head_node)
{
    while (symbols_head_node)
    {
        struct node_s* temp_node = symbols_head_node;
        symbols_head_node = symbols_head_node->next;
        free(temp_node->node_data.data_symbol_entry->symbol_name);
        free(temp_node->node_data.data_symbol_entry);
        free(temp_node);
    }
}

/* Free data segment memory */
void free_data_segment_mem(word_s** data_segment, unsigned int DCF)
{
    int i;

    if (data_segment == NULL)
    {
        return;
    }

    for (i = 0; i < DCF; ++i)
    {
        free(data_segment[i]);
    }

    free(data_segment);
}
