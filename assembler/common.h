#ifndef COMMON_HEADER_FILE_H
#define COMMON_HEADER_FILE_H

#include "defs.h"
#include "types.h"


/* Create formatted source code string line */
void create_formated_line(char* str_line, char* formatted_str_line);

/* Search for the given string register name in the register look-up table */
regs_s* is_register_name(char* str);

/* Clear white spaces and tabs from the given string */
void clear_white_spaces_and_tabs(char* line);

/* Clear leading & trailing white spaces and tabs from the given string */
void clear_leading_trailing_white_spaces_and_tabs(char* formatted_str_line);

/* Search for the given string instruction in the instruction look-up table */
instruction_info_entry_s* find_instructions(char* token);

/* Search for the given Symbol in the Symbol linked-list */
symbol_entry_s* find_symbol(struct node_s* symbols, char* symbol_name);

/* Validate number syntax and return the appropriate value */
int generate_signed_number(char* str_operand, short* immediate_value);

/* Return the filename from a given full path */
char* get_file_name(char* file_path);

int parse_jump_addressing(char* param, char* jump_param1, char* jump_param2, char* jump_param3);

/* Free memory linked-list lines */
void free_lines_mem(struct node_s* lines_head_node);

/* Free memory linked-list Symbols */
void free_symbols_mem(struct node_s* symbols_head_node);

/* Free data segment memory */
void free_data_segment_mem(word_s** data_segment, unsigned int DCF);

#endif
