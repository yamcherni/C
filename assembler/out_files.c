#include "out_files.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void generate_ob_line(char ob_line[WORD_SIZE_BITS + 1], word_s* word)
{
    unsigned short mask = 1 << (WORD_SIZE_BITS - 1);
    int index = 0;

    if (word == NULL)
    {
        fprintf(stdout, "word param has NULL value\n");
        return;
    }

    while (mask)
    {
        ob_line[index] = ((word->data & mask) ? ('/') : ('.'));
        ++index;
        mask >>= 1;
    }
}

/* Save an object file */
void save_object_file(struct node_s* lines_head_node, word_s** data_segment,
                      char* src_file_name, unsigned int ICF, unsigned int DCF)
{
    unsigned int i;
    struct node_s* temp_lines_node = lines_head_node;
    unsigned int size;
    const char ob_file_extension[] = ".ob";
    char* ob_file_name = NULL;
    FILE* f = NULL;
    char ob_line[WORD_SIZE_BITS + 1] = { 0 };

    if ((lines_head_node == NULL) || (data_segment == NULL) || (src_file_name == NULL))
    {
        return;
    }

    ob_file_name = (char*)calloc(1, strlen(src_file_name) + strlen(ob_file_extension) + 1);
    if (ob_file_name == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    strcpy(ob_file_name, src_file_name);
    strcat(ob_file_name, ob_file_extension);

    f = fopen(ob_file_name, "w");
    if (f == NULL)
    {
        free(ob_file_name);
        fprintf(stdout, "Error opening file!\n");
        return;
    }

    size = ICF - INSTRUCTION_CODE_MEM_OFFSET;
    fprintf(f, "\t%d", size);

    size = DCF;
    fprintf(f, "  %d\n", size);

    /* Code Segment*/
    while (temp_lines_node)
    {
        for (i = 0; i < temp_lines_node->node_data.data_src_line->L; ++i)
        {
            memset(ob_line, 0, sizeof(ob_line));
            generate_ob_line(ob_line, temp_lines_node->node_data.data_src_line->words[i]);

            fprintf(f, "%04d\t%s\n", temp_lines_node->node_data.data_src_line->IC + i, ob_line);
        }
        temp_lines_node = temp_lines_node->next;
    }

    /* Data Segment*/
    for (i = 0; i < DCF; ++i)
    {
        memset(ob_line, 0, sizeof(ob_line));
        generate_ob_line(ob_line, data_segment[i]);

        fprintf(f, "%04d\t%s\n", ICF + i, ob_line);
    }

    if (f)
    {
        fclose(f);
        fprintf(stdout, "%s file has created.\n\n", ob_file_name);
    }

    free(ob_file_name);
}

/* Save an entry file */
void save_entry_symbols_file(struct node_s* symbols_head_node, char* src_file_name)
{
    FILE* f = NULL;
    struct node_s* symbol = symbols_head_node;
    const char ent_file_extension[] = ".ent";
    char* ent_file_name = NULL;
    symbol_entry_s* symbol_entry = NULL;

    if ((symbols_head_node == NULL) || (src_file_name == NULL))
    {
        return;
    }

    ent_file_name = (char*)calloc(1, strlen(src_file_name) + strlen(ent_file_extension) + 1);
    if (ent_file_name == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    strcpy(ent_file_name, src_file_name);
    strcat(ent_file_name, ent_file_extension);

    while (symbol)
    {
        symbol_entry = symbol->node_data.data_symbol_entry;

        if (symbol_entry->symbol_attributes & entry_attribute)
        {
            if (f == NULL)
            {
                f = fopen(ent_file_name, "w");
                if (f == NULL)
                {
                    free(ent_file_name);
                    fprintf(stdout, "Error opening file!\n");
                    return;
                }
            }

            fprintf(f, "%s\t%d\n", symbol_entry->symbol_name, symbol_entry->address);
        }

        symbol = symbol->next;
    }

    if (f)
    {
        fclose(f);
        fprintf(stdout, "%s file has created.\n\n", ent_file_name);
    }

    free(ent_file_name);
}

/* Save an extern file */
void save_extern_symbols_file(struct node_s* lines_head_node, struct node_s* symbols_head_node, char* src_file_name)
{
    FILE* f = NULL;
    struct node_s* symbol = NULL;
    const char ext_file_extension[] = ".ext";
    char* ext_file_name = NULL;
    char jump_param1[MAX_SRC_LINE_SIZE] = { 0 };
    char jump_param2[MAX_SRC_LINE_SIZE] = { 0 };
    char jump_param3[MAX_SRC_LINE_SIZE] = { 0 };
    int is_jump_addressing;
    unsigned int word_address_offset = 0;
    unsigned int i;
    struct node_s* temp_lines_node = lines_head_node;

    if ((lines_head_node == NULL) || (symbols_head_node == NULL) || (src_file_name == NULL))
    {
        return;
    }

    ext_file_name = (char*)calloc(1, strlen(src_file_name) + strlen(ext_file_extension) + 1);
    if (ext_file_name == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    strcpy(ext_file_name, src_file_name);
    strcat(ext_file_name, ext_file_extension);

    while (temp_lines_node)
    {
        for (i = 0; i < temp_lines_node->node_data.data_src_line->num_of_operands; ++i)
        {
            operand_s* operand = &(temp_lines_node->node_data.data_src_line->operands[i]);
            symbol = symbols_head_node;

            while (symbol)
            {
                symbol_entry_s* symbol_entry = symbol->node_data.data_symbol_entry;

                if (symbol_entry->symbol_attributes & external_attribute)
                {
                    is_jump_addressing = parse_jump_addressing(operand->operand_str, jump_param1, jump_param2, jump_param3);

                    if ((is_jump_addressing &&
                        ((!strcmp(jump_param1, symbol_entry->symbol_name) ||
                        (!strcmp(jump_param2, symbol_entry->symbol_name)) ||
                        (!strcmp(jump_param3, symbol_entry->symbol_name))))) ||
                        (!strcmp(operand->operand_str, symbol_entry->symbol_name)))
                    {
                        if (f == NULL)
                        {
                            f = fopen(ext_file_name, "w");
                            if (f == NULL)
                            {
                                free(ext_file_name);
                                fprintf(stdout, "Error opening file!\n");
                                return;
                            }
                        }

                        if (is_jump_addressing)
                        {
                            if (!strcmp(jump_param2, symbol_entry->symbol_name))
                            {
                                word_address_offset = 1;
                            }
                            else if (!strcmp(jump_param3, symbol_entry->symbol_name))
                            {
                                word_address_offset = 2;
                            }
                        }

                        fprintf(f, "%s\t%d\n", symbol_entry->symbol_name, operand->word_address + word_address_offset);
                        word_address_offset = 0;
                    }
                }

                symbol = symbol->next;
            }
        }

                temp_lines_node = temp_lines_node->next;
            }

    if (f)
    {
        fclose(f);
        fprintf(stdout, "%s file has created.\n\n", ext_file_name);
    }

    free(ext_file_name);
}
