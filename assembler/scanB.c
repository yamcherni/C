#include "scanB.h"
#include "common.h"
#include <stdio.h>
#include <string.h>



/* Second Source Code Scan */
int scanB(struct node_s* lines_head_node, struct node_s* symbols_head_node)
{
    instruction_jump_addressing_label_operand_s* instruction_jump = NULL;
    instruction_direct_addressing_label_operand_s* instruction_direct = NULL;
    instruction_immediate_addressing_integer_operand_s* instruction_immediate = NULL;
    instruction_direct_reg_addressing_label_operand_s* instruction_direct_reg = NULL;
    regs_s* reg1 = NULL;
    regs_s* reg2 = NULL;
    int failed = 0;
    struct node_s* node_src_line = NULL;
    char* formatted_line = NULL;
    unsigned int  num_of_operands;
    int i;
    symbol_entry_s* symbol_entry = NULL;
    char* token = NULL;
    char temp_line[MAX_SRC_LINE_SIZE] = { 0 };
    char symbol_operand[MAX_SRC_LINE_SIZE] = { 0 };
    char param1[MAX_SRC_LINE_SIZE] = { 0 };
    char param2[MAX_SRC_LINE_SIZE] = { 0 };
    instruction_word_s* instruction_word = NULL;
    instruction_jump_addressing_label_operand_s* jump_addressing = NULL;
    short immediate_value = 0;
    symbol_entry_s* sym = NULL;

    if ((lines_head_node == NULL) || (symbols_head_node == NULL))
    {
        return 0;
    }


    /* 1 */
    node_src_line = lines_head_node;
    while (node_src_line)
    {
        formatted_line = node_src_line->node_data.data_src_line->formatted_str;
        if (formatted_line == NULL)
        {
            node_src_line = node_src_line->next;
            continue;
        }

        memset(temp_line, 0, MAX_SRC_LINE_SIZE);
        strcpy(temp_line, formatted_line);

        /* 2 */
        token = strtok(temp_line, " ");
        if (token[strlen(token) - 1] == ':')
        {
            token = strtok(NULL, " ");
        }

        /* 3 */
        if (!strcmp(token, ".data") || !strcmp(token, ".string") || !strcmp(token, ".extern") || !strcmp(token, ".struct"))
        {
            node_src_line = node_src_line->next;
            continue;
        }

        /* 4 */
        if (!strcmp(token, ".entry"))
        {
            /* 5 */
            token = strtok(NULL, " ");

            symbol_entry = NULL;
            if ((symbol_entry = find_symbol(symbols_head_node, token)))
            {
                if (symbol_entry->symbol_attributes & external_attribute)
                {
                    fprintf(stdout, "Line: %d, Error! Symbol redefinition. Symbol has defined both as an external type and as entry (%s).\n", node_src_line->node_data.data_src_line->line_num, token);
                    failed = 1;

                    node_src_line = node_src_line->next;
                    continue;
                }

                if (symbol_entry->symbol_attributes & entry_attribute)
                {
                    node_src_line = node_src_line->next;
                    continue;
                }
            }

            if (!(symbol_entry = find_symbol(symbols_head_node, token)))
            {
                fprintf(stdout, "Line: %d, Error! Symbol was not found in the Symbol Table (%s).\n", node_src_line->node_data.data_src_line->line_num, token);
                failed = 1;

                node_src_line = node_src_line->next;
                continue;
            }

            symbol_entry->symbol_attributes |= entry_attribute;

            node_src_line = node_src_line->next;
            continue;
        }

        /* 6 */
        num_of_operands = node_src_line->node_data.data_src_line->num_of_operands;
        symbol_entry = NULL;
        for (i = 0; i < num_of_operands; ++i)
        {
            operand_s* operand = &(node_src_line->node_data.data_src_line->operands[i]);
            if (operand->operand_addressing_type == immediate_addressing_type)
            {
                short immediate_value = 0;
                if (!generate_signed_number(&(operand->operand_str[1]), &immediate_value))
                {
                    fprintf(stdout, "Line: %d, Error! Not a valid number (%s).\n", node_src_line->node_data.data_src_line->line_num, &(operand->operand_str[1]));
                    failed = 1;
                    continue;
                }

                instruction_immediate = (instruction_immediate_addressing_integer_operand_s*)(operand->operand_word[0]);
                instruction_immediate->num = immediate_value;
                instruction_immediate->era = A;
            }
            else if (operand->operand_addressing_type == direct_addressing_type)
            {
                symbol_entry_s* symbol_entry = NULL;
                if (!(symbol_entry = find_symbol(symbols_head_node, operand->operand_str)))
                {
                    fprintf(stdout, "Line: %d, Error! Symbol does not exist. (%s)\n", node_src_line->node_data.data_src_line->line_num, operand->operand_str);
                    failed = 1;
                    continue;
                }

                instruction_direct = (instruction_direct_addressing_label_operand_s*)(operand->operand_word[0]);

                if (symbol_entry->symbol_attributes & external_attribute)
                {
                    instruction_direct->direct_addressing_address.era = E;
                }
                else
                {
                    instruction_direct->direct_addressing_address.address = symbol_entry->address;
                    instruction_direct->direct_addressing_address.era = R;
                }
            }
            else if (operand->operand_addressing_type == jump_addressing_type)
            {
                parse_jump_addressing(operand->operand_str, symbol_operand, param1, param2);

                symbol_entry = NULL;
                if (!(symbol_entry = find_symbol(symbols_head_node, symbol_operand)))
                {
                    fprintf(stdout, "Line: %d, Error! Symbol does not exist. (%s)\n", node_src_line->node_data.data_src_line->line_num, symbol_operand);
                    failed = 1;
                    continue;
                }

                instruction_jump = (instruction_jump_addressing_label_operand_s*)(operand->operand_word[0]);

                if (symbol_entry->symbol_attributes & external_attribute)
                {
                    instruction_jump->jump_params_addressing.jump_address.era = E;
                }
                else
                {
                    instruction_jump->jump_params_addressing.jump_address.address = symbol_entry->address;
                    instruction_jump->jump_params_addressing.jump_address.era = R;
                }

                instruction_word = (instruction_word_s*)(node_src_line->node_data.data_src_line->words[0]);
                if ((reg1 = is_register_name(param1)) && (reg2 = is_register_name(param2)))
                {
                    instruction_word->param1_addressing_type = register_direct_addressing_type;
                    instruction_word->param2_addressing_type = register_direct_addressing_type;

                    jump_addressing = (instruction_jump_addressing_label_operand_s*)operand->operand_word[1];
                    jump_addressing->jump_params_addressing.jump_params_2_words.param1 = reg1->reg_num;
                    jump_addressing->jump_params_addressing.jump_params_2_words.param2 = reg2->reg_num;
                    jump_addressing->jump_params_addressing.jump_params_2_words.era = A;
                }
                else
                {
                    jump_addressing = (instruction_jump_addressing_label_operand_s*)operand->operand_word[1];
                    if ((reg1 = is_register_name(param1)))
                    {
                        instruction_word->param1_addressing_type = register_direct_addressing_type;
                        jump_addressing->jump_params_addressing.jump_param.param = reg1->reg_num;
                        jump_addressing->jump_params_addressing.jump_param.era = A;
                    }
                    else if ((param1[0] == '#') && (generate_signed_number(param1 + 1, &immediate_value)))
                    {
                        instruction_word->param1_addressing_type = immediate_addressing_type;
                        jump_addressing->jump_params_addressing.jump_param.param = immediate_value;
                        jump_addressing->jump_params_addressing.jump_param.era = A;
                    }
                    else if ((sym = find_symbol(symbols_head_node, param1)))
                    {
                        instruction_word->param1_addressing_type = direct_addressing_type;
                        if (sym->symbol_attributes & external_attribute)
                        {
                            jump_addressing->jump_params_addressing.jump_param.era = E;
                        }
                        else
                        {
                            jump_addressing->jump_params_addressing.jump_param.param = sym->address;
                            jump_addressing->jump_params_addressing.jump_param.era = R;
                        }
                    }
                    else
                    {
                        fprintf(stdout, "Line: %d, Error! Wrong Param. (%s)\n", node_src_line->node_data.data_src_line->line_num, param1);
                        failed = 1;
                        continue;
                    }

                    jump_addressing = (instruction_jump_addressing_label_operand_s*)operand->operand_word[2];
                    if ((reg1 = is_register_name(param2)))
                    {
                        instruction_word->param2_addressing_type = register_direct_addressing_type;
                        jump_addressing->jump_params_addressing.jump_param.param = reg1->reg_num;
                        jump_addressing->jump_params_addressing.jump_param.era = A;
                    }
                    else if ((param2[0] == '#') && (generate_signed_number(param2 + 1, &immediate_value)))
                    {
                        instruction_word->param2_addressing_type = immediate_addressing_type;
                        jump_addressing->jump_params_addressing.jump_param.param = immediate_value;
                        jump_addressing->jump_params_addressing.jump_param.era = A;
                    }
                    else if ((sym = find_symbol(symbols_head_node, param2)))
                    {
                        instruction_word->param2_addressing_type = direct_addressing_type;
                        if (sym->symbol_attributes & external_attribute)
                        {
                            jump_addressing->jump_params_addressing.jump_param.era = E;
                        }
                        else
                        {
                            jump_addressing->jump_params_addressing.jump_param.param = sym->address;
                            jump_addressing->jump_params_addressing.jump_param.era = R;
                        }
                    }
                    else
                    {
                        fprintf(stdout, "Line: %d, Error! Wrong Param. (%s)\n", node_src_line->node_data.data_src_line->line_num, param2);
                        failed = 1;
                        continue;
                    }
                }
            }
            else if (operand->operand_addressing_type == register_direct_addressing_type)
            {
                instruction_direct_reg = (instruction_direct_reg_addressing_label_operand_s*)(operand->operand_word[0]);

                if ((node_src_line->node_data.data_src_line->num_of_operands == 2) &&
                    (node_src_line->node_data.data_src_line->operands[0].operand_addressing_type == register_direct_addressing_type) &&
                    (node_src_line->node_data.data_src_line->operands[1].operand_addressing_type == register_direct_addressing_type))
                {
                    reg1 = is_register_name(node_src_line->node_data.data_src_line->operands[0].operand_str);
                    reg2 = is_register_name(node_src_line->node_data.data_src_line->operands[1].operand_str);

                    instruction_direct_reg->direct_addressing_data.direct_addressing.src_reg_num = reg1->reg_num;
                    instruction_direct_reg->direct_addressing_data.direct_addressing.dst_reg_num = reg2->reg_num;
                }
                else if (node_src_line->node_data.data_src_line->operands[i].operand_type == src_operand_type)
                {
                    reg1 = is_register_name(node_src_line->node_data.data_src_line->operands[i].operand_str);
                    instruction_direct_reg->direct_addressing_data.direct_addressing_src.reg_num = reg1->reg_num;
                }
                else
                {
                    reg1 = is_register_name(node_src_line->node_data.data_src_line->operands[i].operand_str);
                    instruction_direct_reg->direct_addressing_data.direct_addressing_dst.reg_num = reg1->reg_num;
                }
            }
        }

        node_src_line = node_src_line->next;
    }

    /* 7 */
    if (failed)
    {
        return 0;
    }

    return 1;
}
