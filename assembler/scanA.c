#include "scanA.h"
#include "linked_list.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>



/* Add new Word to the Data-Segment */
static void append_data_word(word_s*** data_segment, word_s* word, unsigned int* DC)
{
    if ((word == NULL) || (data_segment == NULL))
    {
        return;
    }

    *data_segment = (word_s**)realloc(*data_segment, sizeof(word_s*) * (*DC + 1));
    if (*data_segment == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    (*data_segment)[*DC] = (word_s*)calloc(1, sizeof(word_s));
    if ((*data_segment)[*DC] == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    (*data_segment)[*DC]->data = word->data;
    (*DC)++;
}

/* Validate line syntax in term of redundant colons and trailing/leading colons */
static int validate_line_syntax(char* formatted_line, unsigned int line_num)
{
    int i;

    if (formatted_line == NULL)
    {
        return 0;
    }

    if ((formatted_line[0] == ',') || (formatted_line[strlen(formatted_line) - 1] == ','))
    {
        fprintf(stdout, "Line: %d, Error! line syntax error. leading/trailing comma is not allowed.\n", line_num);
        return 0;
    }

    for (i = 0; i < strlen(formatted_line) - 1; ++i)
    {
        if ((formatted_line[i] == ',') && (formatted_line[i + 1] == ','))
        {
            fprintf(stdout, "Line: %d, Error! line syntax error. extra comma .\n", line_num);
            return 0;
        }
    }

    return 1;
}


int scanA(struct node_s* lines_head_node, struct node_s** symbols_head_node,
               word_s*** data_segment, unsigned int* IC, unsigned int* DC,
               unsigned int* ICF, unsigned int* DCF)
{
    struct node_s* temp_symbol_node = NULL;
    word_s* word1 = NULL;
    word_s* word2 = NULL;
    word_s* word3 = NULL;
    regs_s* reg = NULL;
    instruction_immediate_addressing_integer_operand_s* immediate_word = NULL;
    short immediate_value;
    unsigned int valid_addressing_type;
    int i = 0;
    char str_operands[MAX_INSTRUCTION_OPERANDS][MAX_SRC_LINE_SIZE] = { 0 };
    unsigned int operand_index;
    instruction_word_s* instruction_word = NULL;
    instruction_info_entry_s* instruction = NULL;
    symbol_entry_s* symbol_entry = NULL;
    word_s word = { 0 };
    char* token = NULL;
    int symbol_def;
    char symbol_name[MAX_SRC_LINE_SIZE] = { 0 };
    char temp_line[MAX_SRC_LINE_SIZE] = { 0 };
    char jump_param1[MAX_SRC_LINE_SIZE] = { 0 };
    char jump_param2[MAX_SRC_LINE_SIZE] = { 0 };
    char jump_param3[MAX_SRC_LINE_SIZE] = { 0 };
    int failed = 0;
    struct node_s* node_src_line = NULL;
    int is_jump_addressing = 0;

    if ((lines_head_node == NULL) || (symbols_head_node == NULL) || (data_segment == NULL))
    {
        return 0;
    }

    /* 1 */
    *IC = INSTRUCTION_CODE_MEM_OFFSET;
    *DC = 0;

    /* 2 */
    node_src_line = lines_head_node;
    while (node_src_line)
    {
        char formatted_line[MAX_SRC_LINE_SIZE] = { 0 };
        create_formated_line(node_src_line->node_data.data_src_line->str, formatted_line);

        if ((formatted_line[0] == '\0') || (formatted_line[0] == ';') ||
            (formatted_line[0] == ' ') || (formatted_line[0] == '\t'))
        {
            node_src_line = node_src_line->next;
            continue;
        }

        if (!validate_line_syntax(formatted_line, node_src_line->node_data.data_src_line->line_num))
        {
            failed = 1;
            node_src_line = node_src_line->next;
            continue;
        }

        node_src_line->node_data.data_src_line->formatted_str = (char*)calloc(1, strlen(formatted_line) + 1);
        if (node_src_line->node_data.data_src_line->formatted_str == NULL)
        {
            fprintf(stdout, "Fatal: failed to allocate memory!\n");
            exit(-1);
        }

        strcpy(node_src_line->node_data.data_src_line->formatted_str, formatted_line);

        symbol_def = 0;
        memset(symbol_name, 0, MAX_SRC_LINE_SIZE);

        memset(temp_line, 0, MAX_SRC_LINE_SIZE);
        strcpy(temp_line, formatted_line);

        /* 3 */
        token = strtok(temp_line, " ");
        if (token[strlen(token) - 1] == ':')
        {
            /* 4 */
            /* Handle Symbol/Label*/
            symbol_def = 1;
            token[strlen(token) - 1] = '\0';
            strcpy(symbol_name, token);

            if (isdigit(symbol_name[0]))
            {
                fprintf(stdout, "Line: %d, Error! Label can't start with a digit (%s).\n", node_src_line->node_data.data_src_line->line_num, symbol_name);
                failed = 1;
                node_src_line = node_src_line->next;
                continue;
            }

            if (strlen(symbol_name) > MAX_LABEL_LENGTH)
            {
                fprintf(stdout, "Line: %d, Error! Label length is greather than %d. (%ld)\n", node_src_line->node_data.data_src_line->line_num, MAX_LABEL_LENGTH, strlen(token));
                failed = 1;
                node_src_line = node_src_line->next;
                continue;
            }

            if (is_register_name(symbol_name) != NULL)
            {
                fprintf(stdout, "Line: %d, Error! Label name can't get a register name.\n", node_src_line->node_data.data_src_line->line_num);
                failed = 1;
                node_src_line = node_src_line->next;
                continue;
            }

            if (find_instructions(symbol_name) != NULL)
            {
                fprintf(stdout, "Line: %d, Error! Label name can't get an instructions name.\n", node_src_line->node_data.data_src_line->line_num);
                failed = 1;
                node_src_line = node_src_line->next;
                continue;
            }

            token = strtok(NULL, " ");
        }

        /* 5 */
        if (!strcmp(token, ".data") || !strcmp(token, ".string"))
        {
            /* 6 */
            if (symbol_def)
            {
                if (find_symbol(*symbols_head_node, symbol_name))
                {
                    fprintf(stdout, "Line: %d, Error! Symbol is already defined. (%s)\n", node_src_line->node_data.data_src_line->line_num, symbol_name);
                    failed = 1;
                }
                else
                {
                    append_symbol(symbols_head_node, symbol_name, *DC, data_attribute);
                }
            }

            /* 7 */
            if (!strcmp(token, ".data"))
            {
                while ((token = strtok(NULL, " ,")))
                {
                    short num;
                    if (!generate_signed_number(token, &num))
                    {
                        fprintf(stdout, "Line: %d, Error! Wrong number operand. (%s)\n", node_src_line->node_data.data_src_line->line_num, token);
                        failed = 1;
                        node_src_line = node_src_line->next;
                        continue;
                    }

                    append_data_word(data_segment, ((word_s*)(&num)), DC);
                }
            }
            else if (!strcmp(token, ".string"))
            {
                token = strtok(NULL, " ");
                if ((token[0] != '"') || (token[strlen(token) - 1] != '"'))
                {
                    fprintf(stdout, "Line: %d, Error! one or more apostrophe is missing. (%s)\n", node_src_line->node_data.data_src_line->line_num, token);
                    failed = 1;
                    node_src_line = node_src_line->next;
                    continue;
                }

                for (i = 1; i < strlen(token) - 1; ++i)
                {
                    word_s word = { 0 };

                    if (!isprint(token[i]))
                    {
                        fprintf(stdout, "Line: %d, Error! Non-printable character has defined.\n", node_src_line->node_data.data_src_line->line_num);
                        failed = 1;
                    }

                    ((char*)&word)[0] = token[i];
                    append_data_word(data_segment, &word, DC);
                }

                /* Adding NULL character */
                memset(&word, 0, sizeof(word));
                append_data_word(data_segment, &word, DC);
            }

            node_src_line = node_src_line->next;
            continue;
        }

        /* 8, 9 and 10 */
        if (!strcmp(token, ".entry"))
        {
            if (symbol_def)
            {
                fprintf(stdout, "Line: %d, Warning! Ignoring label definition in .entry line.\n", node_src_line->node_data.data_src_line->line_num);
            }

            node_src_line = node_src_line->next;
            continue;
        }

        if (!strcmp(token, ".extern"))
        {
            if (symbol_def)
            {
                fprintf(stdout, "Line: %d, Warning! Ignoring label definition in .extern line.\n", node_src_line->node_data.data_src_line->line_num);
                symbol_def = 0;
            }

            token = strtok(NULL, " ");

            if ((symbol_entry = find_symbol(*symbols_head_node, token)))
            {
                if (symbol_entry->symbol_attributes & external_attribute)
                {
                    node_src_line = node_src_line->next;
                    continue;
                }

                if (symbol_entry->symbol_attributes & entry_attribute)
                {
                    fprintf(stdout, "Line: %d, Error! Symbol is already defined as entry. (%s)\n", node_src_line->node_data.data_src_line->line_num, token);
                    failed = 1;
                    node_src_line = node_src_line->next;
                    continue;
                }
            }

            if (is_register_name(token) != NULL)
            {
                fprintf(stdout, "Line: %d, Error! Symbol name can't get a register name.\n", node_src_line->node_data.data_src_line->line_num);
                failed = 1;
                node_src_line = node_src_line->next;
                continue;
            }

            if (find_instructions(token) != NULL)
            {
                fprintf(stdout, "Line: %d, Error! Symbol name can't get an instructions name.\n", node_src_line->node_data.data_src_line->line_num);
                failed = 1;
                node_src_line = node_src_line->next;
                continue;
            }

            append_symbol(symbols_head_node, token, 0, external_attribute);

            node_src_line = node_src_line->next;
            continue;
        }


        /* 11 */
        if (symbol_def)
        {
            if (find_symbol(*symbols_head_node, symbol_name))
            {
                fprintf(stdout, "Line: %d, Error! Symbol is already defined. (%s)\n", node_src_line->node_data.data_src_line->line_num, symbol_name);
                failed = 1;
            }
            else
            {
                append_symbol(symbols_head_node, symbol_name, *IC, code_attribute);
            }
        }

        /* 12 */
        if (!(instruction = find_instructions(token)))
        {
            fprintf(stdout, "Line: %d, Error! (%s) is not a valid instruction name.\n", node_src_line->node_data.data_src_line->line_num, token);
            failed = 1;
            node_src_line = node_src_line->next;
            continue;
        }

        /* 13, 14 and 15*/
        if ((instruction->opcode == RTS) ||
            (instruction->opcode == STOP))
        {
            instruction_word_s* instruction_word = (instruction_word_s*)calloc(1, sizeof(instruction_word_s));
            if (instruction_word == NULL)
            {
                fprintf(stdout, "Fatal: failed to allocate memory!\n");
                exit(-1);
            }

            instruction_word->opcode = instruction->opcode;
            instruction_word->era = A;

            node_src_line->node_data.data_src_line->L = 1;
            node_src_line->node_data.data_src_line->words = (word_s**)calloc(1, sizeof(word_s*));
            if (node_src_line->node_data.data_src_line->words == NULL)
            {
                fprintf(stdout, "Fatal: failed to allocate memory!\n");
                exit(-1);
            }

            node_src_line->node_data.data_src_line->words[0] = (word_s*)instruction_word;

            node_src_line->node_data.data_src_line->IC = *IC;
            *IC += node_src_line->node_data.data_src_line->L;

            node_src_line = node_src_line->next;
            continue;
        }

        instruction_word = (instruction_word_s*)calloc(1, sizeof(instruction_word_s));
        if (instruction_word == NULL)
        {
            fprintf(stdout, "Fatal: failed to allocate memory!\n");
            exit(-1);
        }

        instruction_word->opcode = instruction->opcode;
        instruction_word->era = A;

        node_src_line->node_data.data_src_line->L = 1;
        node_src_line->node_data.data_src_line->words = (word_s**)calloc(1, sizeof(word_s*));
        if (node_src_line->node_data.data_src_line->words == NULL)
        {
            fprintf(stdout, "Fatal: failed to allocate memory!\n");
            exit(-1);
        }

        node_src_line->node_data.data_src_line->words[0] = (word_s*)instruction_word;

        memset(str_operands, 0, MAX_INSTRUCTION_OPERANDS * MAX_SRC_LINE_SIZE);

        is_jump_addressing = parse_jump_addressing(token + strlen(token) + 1, jump_param1, jump_param2, jump_param3);
        if (is_jump_addressing == 1)
        {
            strcpy(str_operands[0], token + strlen(token) + 1);
            operand_index = 1;
        }
        else if (is_jump_addressing == 0)
        {
            operand_index = 0;
            while ((token = strtok(NULL, " ,")))
            {
                if ((operand_index + 1) > MAX_INSTRUCTION_OPERANDS)
                {
                    fprintf(stdout, "Line: %d, Error! Too much operands were detected (%s).\n", node_src_line->node_data.data_src_line->line_num, token);
                    failed = 1;
                    break;
                }

                strcpy(str_operands[operand_index], token);
                ++operand_index;
            }
        }

        node_src_line->node_data.data_src_line->num_of_operands = operand_index;

        if (node_src_line->node_data.data_src_line->num_of_operands != instruction->num_of_operands)
        {
            fprintf(stdout, "Line: %d, Error! Illegal number of operands (%d).\n", node_src_line->node_data.data_src_line->line_num, instruction->num_of_operands);
            failed = 1;
            node_src_line = node_src_line->next;
            continue;
        }

        for (i = 0; i < node_src_line->node_data.data_src_line->num_of_operands; ++i)
        {
            node_src_line->node_data.data_src_line->operands[i].operand_str = calloc(1, strlen(str_operands[i]) + 1);
            if (node_src_line->node_data.data_src_line->operands[i].operand_str == NULL)
            {
                fprintf(stdout, "Fatal: failed to allocate memory!\n");
                exit(-1);
            }

            strcpy(node_src_line->node_data.data_src_line->operands[i].operand_str, str_operands[i]);

            if ((instruction->num_of_operands == 1) || ((instruction->num_of_operands == 2) && (i == 1)))
            {
                valid_addressing_type = instruction->dst_valid_addressing_type;
                node_src_line->node_data.data_src_line->operands[i].operand_type = dst_operand_type;
            }
            else
            {
                valid_addressing_type = instruction->src_valid_addressing_type;
                node_src_line->node_data.data_src_line->operands[i].operand_type = src_operand_type;
            }

            /* Immediate Addressing Operand Type */
            if (str_operands[i][0] == '#')
            {
                if (strlen(str_operands[i]) == 1)
                {
                    fprintf(stdout, "Line: %d, Error! An operand number has to be attached to the # character.\n", node_src_line->node_data.data_src_line->line_num);
                    failed = 1;
                    continue;
                }

                if ((valid_addressing_type & immediate_addressing_type_bit) == 0)
                {
                    fprintf(stdout, "Line: %d, Error! Immediate addressing type is not allowed for instruction %s.\n", node_src_line->node_data.data_src_line->line_num, instruction->name);
                    failed = 1;
                    continue;
                }

                immediate_value = 0;
                if (!generate_signed_number(&(str_operands[i][1]), &immediate_value))
                {
                    fprintf(stdout, "Line: %d, Error! Not a valid number (%s).\n", node_src_line->node_data.data_src_line->line_num, &(str_operands[i][1]));
                    failed = 1;
                    continue;
                }

                immediate_word = (instruction_immediate_addressing_integer_operand_s*)calloc(1, sizeof(instruction_immediate_addressing_integer_operand_s));
                if (immediate_word == NULL)
                {
                    fprintf(stdout, "Fatal: failed to allocate memory!\n");
                    exit(-1);
                }

                immediate_word->num = (unsigned char)immediate_value;
                immediate_word->era = A;

                node_src_line->node_data.data_src_line->words = (word_s**)realloc(node_src_line->node_data.data_src_line->words, (node_src_line->node_data.data_src_line->L + 1) * sizeof(word_s*));
                if (*node_src_line->node_data.data_src_line->words == NULL)
                {
                    fprintf(stdout, "Fatal: failed to allocate memory!\n");
                    exit(-1);
                }

                node_src_line->node_data.data_src_line->words[node_src_line->node_data.data_src_line->L] = (word_s*)immediate_word;
                node_src_line->node_data.data_src_line->L++;

                node_src_line->node_data.data_src_line->operands[i].operand_addressing_type = immediate_addressing_type;
                node_src_line->node_data.data_src_line->operands[i].operand_word[0] = (word_s*)immediate_word;
                node_src_line->node_data.data_src_line->operands[i].word_address = *IC + node_src_line->node_data.data_src_line->L - 1;
            }
            else if ((reg = is_register_name(str_operands[i]))) /* Direct-Register Addressing Operand Type */
            {
                if ((valid_addressing_type & reg_direct_addressing_type_bit) == 0)
                {
                    fprintf(stdout, "Line: %d, Error! Direct register addressing type is not allowed for instruction %s.\n", node_src_line->node_data.data_src_line->line_num, instruction->name);
                    failed = 1;
                    continue;
                }

                if (node_src_line->node_data.data_src_line->operands[i].operand_type == src_operand_type)
                {
                    instruction_word->src_addressing_type = register_direct_addressing_type;

                    word1 = (word_s*)calloc(1, sizeof(word_s));
                    if (word1 == NULL)
                    {
                        fprintf(stdout, "Fatal: failed to allocate memory!\n");
                        exit(-1);
                    }

                    node_src_line->node_data.data_src_line->words = (word_s**)realloc(node_src_line->node_data.data_src_line->words, (node_src_line->node_data.data_src_line->L + 1) * sizeof(word_s*));
                    if (*node_src_line->node_data.data_src_line->words == NULL)
                    {
                        fprintf(stdout, "Fatal: failed to allocate memory!\n");
                        exit(-1);
                    }

                    node_src_line->node_data.data_src_line->words[node_src_line->node_data.data_src_line->L] = word1;
                    node_src_line->node_data.data_src_line->L++;
                }
                else
                {
                    instruction_word->dst_addressing_type = register_direct_addressing_type;

                    if (node_src_line->node_data.data_src_line->operands[0].operand_addressing_type != register_direct_addressing_type)
                    {
                        word1 = (word_s*)calloc(1, sizeof(word_s));
                        if (word1 == NULL)
                        {
                            fprintf(stdout, "Fatal: failed to allocate memory!\n");
                            exit(-1);
                        }

                        node_src_line->node_data.data_src_line->words = (word_s**)realloc(node_src_line->node_data.data_src_line->words, (node_src_line->node_data.data_src_line->L + 1) * sizeof(word_s*));
                        if (*node_src_line->node_data.data_src_line->words == NULL)
                        {
                            fprintf(stdout, "Fatal: failed to allocate memory!\n");
                            exit(-1);
                        }

                        node_src_line->node_data.data_src_line->words[node_src_line->node_data.data_src_line->L] = word1;
                        node_src_line->node_data.data_src_line->L++;
                    }
                }

                node_src_line->node_data.data_src_line->operands[i].operand_addressing_type = register_direct_addressing_type;
                node_src_line->node_data.data_src_line->operands[i].operand_word[0] = word1;
            }
            else if (is_jump_addressing) /* Jump Addressing Operand Type*/
            {
                is_jump_addressing = 0;

                if (((instruction->opcode != JMP) &&
                    (instruction->opcode != BNE) &&
                    (instruction->opcode != JSR)) ||
                    ((valid_addressing_type & jump_addressing_type_bit) == 0))
                {
                    fprintf(stdout, "Line: %d, Error! Jump addressing type is not allowed for instruction %s.\n", node_src_line->node_data.data_src_line->line_num, instruction->name);
                    failed = 1;
                    continue;
                }

                word1 = (word_s*)calloc(1, sizeof(word_s));
                if (word1 == NULL)
                {
                    fprintf(stdout, "Fatal: failed to allocate memory!\n");
                    exit(-1);
                }

                word2 = (word_s*)calloc(1, sizeof(word_s));
                if (word2 == NULL)
                {
                    fprintf(stdout, "Fatal: failed to allocate memory!\n");
                    exit(-1);
                }

                if (!is_register_name(jump_param2) || !is_register_name(jump_param3))
                {
                    word3 = (word_s*)calloc(1, sizeof(word_s));
                    if (word3 == NULL)
                    {
                        fprintf(stdout, "Fatal: failed to allocate memory!\n");
                        exit(-1);
                    }
                    node_src_line->node_data.data_src_line->words = (word_s**)realloc(node_src_line->node_data.data_src_line->words, (node_src_line->node_data.data_src_line->L + 3) * sizeof(word_s*));
                }
                else
                {
                    node_src_line->node_data.data_src_line->words = (word_s**)realloc(node_src_line->node_data.data_src_line->words, (node_src_line->node_data.data_src_line->L + 2) * sizeof(word_s*));
                }

                if (node_src_line->node_data.data_src_line->words == NULL)
                {
                    fprintf(stdout, "Fatal: failed to allocate memory!\n");
                    exit(-1);
                }

                node_src_line->node_data.data_src_line->words[node_src_line->node_data.data_src_line->L] = word1;
                node_src_line->node_data.data_src_line->L++;

                node_src_line->node_data.data_src_line->words[node_src_line->node_data.data_src_line->L] = word2;
                node_src_line->node_data.data_src_line->L++;

                if (!is_register_name(jump_param2) || !is_register_name(jump_param3))
                {
                    node_src_line->node_data.data_src_line->words[node_src_line->node_data.data_src_line->L] = word3;
                    node_src_line->node_data.data_src_line->L++;
                }

                if (node_src_line->node_data.data_src_line->operands[i].operand_type == dst_operand_type)
                {
                    instruction_word->dst_addressing_type = jump_addressing_type;
                }
                else
                {
                    instruction_word->src_addressing_type = jump_addressing_type;
                }

                node_src_line->node_data.data_src_line->operands[i].operand_addressing_type = jump_addressing_type;
                node_src_line->node_data.data_src_line->operands[i].operand_word[0] = word1;
                node_src_line->node_data.data_src_line->operands[i].operand_word[1] = word2;

                if (!is_register_name(jump_param2) || !is_register_name(jump_param3))
                {
                    node_src_line->node_data.data_src_line->operands[i].operand_word[2] = word3;
                    node_src_line->node_data.data_src_line->operands[i].word_address = *IC + node_src_line->node_data.data_src_line->L - 3;
                }
                else
                {
                    node_src_line->node_data.data_src_line->operands[i].word_address = *IC + node_src_line->node_data.data_src_line->L - 2;
                }
            }
            else /* Direct Addressing Operand Type */
            {
                if ((valid_addressing_type & direct_addressing_type_bit) == 0)
                {
                    fprintf(stdout, "Line: %d, Error! Direct addressing type is not allowed for instruction %s.\n", node_src_line->node_data.data_src_line->line_num, instruction->name);
                    failed = 1;
                    continue;
                }


                word1 = (word_s*)calloc(1, sizeof(word_s));
                if (word1 == NULL)
                {
                    fprintf(stdout, "Fatal: failed to allocate memory!\n");
                    exit(-1);
                }

                node_src_line->node_data.data_src_line->words = (word_s**)realloc(node_src_line->node_data.data_src_line->words, (node_src_line->node_data.data_src_line->L + 1) * sizeof(word_s*));
                if (node_src_line->node_data.data_src_line->words == NULL)
                {
                    fprintf(stdout, "Fatal: failed to allocate memory!\n");
                    exit(-1);
                }

                node_src_line->node_data.data_src_line->words[node_src_line->node_data.data_src_line->L] = word1;
                node_src_line->node_data.data_src_line->L++;

                if (node_src_line->node_data.data_src_line->operands[i].operand_type == dst_operand_type)
                {
                    instruction_word->dst_addressing_type = direct_addressing_type;
                }
                else
                {
                    instruction_word->src_addressing_type = direct_addressing_type;
                }

                node_src_line->node_data.data_src_line->operands[i].operand_addressing_type = direct_addressing_type;
                node_src_line->node_data.data_src_line->operands[i].operand_word[0] = word1;
                node_src_line->node_data.data_src_line->operands[i].word_address = *IC + node_src_line->node_data.data_src_line->L - 1;
            }
        }

        /* 16 */
        node_src_line->node_data.data_src_line->IC = *IC;
        *IC += node_src_line->node_data.data_src_line->L;

        node_src_line = node_src_line->next;
    }

    /* 17 */
    if ((*IC + *DC - INSTRUCTION_CODE_MEM_OFFSET) > MEM_SIZE)
    {
        failed = 1;
    }

    /* 18 */
    *ICF = *IC;
    *DCF = *DC;


    if (failed)
    {
        return 0;
    }

    /* 19 */
    temp_symbol_node = *symbols_head_node;
    while (temp_symbol_node != NULL)
    {
        if (temp_symbol_node->node_data.data_symbol_entry->symbol_attributes & data_attribute)
        {
            temp_symbol_node->node_data.data_symbol_entry->address += *ICF;
        }

        temp_symbol_node = temp_symbol_node->next;
    }

    return 1;
}
