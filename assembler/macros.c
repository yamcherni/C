#include "macros.h"
#include "common.h"
#include "defs.h"
#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Free memory linked-list Macros */
static void free_macros(struct node_s* macros_head_node)
{
    int i;
    struct node_s* temp_node = NULL;
    macro_entry_s* data = NULL;

    while (macros_head_node)
    {
        temp_node = macros_head_node;
        macros_head_node = macros_head_node->next;

        data = temp_node->node_data.data_macro_entry;
        free(data->macro_name);

        for (i = 0; i < data->num_of_lines; ++i)
        {
            free(data->macro_lines[i]);
        }
        free(data->macro_lines);

        free(temp_node->node_data.data_macro_entry);
        free(temp_node);
    }
}


/* Save .am Macro file */
static void save_macro_file(struct node_s* lines_head_node, char* src_file_name)
{
    const char am_file_extension[] = ".am";
    char* am_file_name = NULL;
    struct node_s* curr_node = NULL;
    FILE* f = NULL;

    if ((lines_head_node == NULL) || (src_file_name == NULL))
    {
        return;
    }

    am_file_name = (char*)calloc(1, strlen(src_file_name) + strlen(am_file_extension) + 1);
    if (am_file_name == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    strcpy(am_file_name, src_file_name);
    strcat(am_file_name, am_file_extension);

    f = fopen(am_file_name, "w");
    if (f == NULL)
    {
        free(am_file_name);
        fprintf(stdout, "Error opening file!\n");
        return;
    }

    fprintf(f, "; file %s\n", am_file_name);

    curr_node = lines_head_node;
    while (curr_node)
    {
        char formatted_line[MAX_SRC_LINE_SIZE] = { 0 };
        create_formated_line(curr_node->node_data.data_src_line->str, formatted_line);
        if (formatted_line[0] == ';')
        {
            curr_node = curr_node->next;
            continue;
        }

        fprintf(f, "%s\n", curr_node->node_data.data_src_line->str);

        curr_node = curr_node->next;
    }

    if (f)
    {
        fclose(f);
        fprintf(stdout, "%s file has created.\n\n", am_file_name);
    }

    free(am_file_name);
}


/* Check whether the Macro is exist in the Macro list */
static macro_entry_s* is_macro(char* macro_name, struct node_s* macros)
{
    macro_entry_s* ret_val = NULL;
    struct node_s* macro = macros;

    if ((macro_name == NULL) || (macros == NULL))
    {
        return ret_val;
    }

    if ((macros == NULL) || (macro_name == NULL))
    {
        return ret_val;
    }

    while (macro)
    {
        macro_entry_s* macro_entry = macro->node_data.data_macro_entry;
        if (!strcmp(macro_name, macro_entry->macro_name))
        {
            ret_val = macro_entry;
            break;
        }
        macro = macro->next;
    }

    return ret_val;
}

/* Handle code Macros */
static int macros_deployment(struct node_s** macros_head_node, struct node_s** lines_head_node)
{
    struct node_s* temp_node_src_line = NULL;
    int failed = 0;
    struct node_s* node_src_line = NULL;
    char* macro_name = NULL;
    macro_entry_s* new_macro_entry = NULL;
    unsigned int i;
    unsigned int macro_line_number;
    char* first_token = NULL;
    macro_entry_s* macro_entry = NULL;
    char temp_line[MAX_SRC_LINE_SIZE] = { 0 };

    if ((macros_head_node == NULL) || (lines_head_node == NULL))
    {
        return 0;
    }

    if (lines_head_node == NULL)
    {
        return 1;
    }

    node_src_line = *lines_head_node;

    while (node_src_line)
    {
        int new_macro_found = 0;
        char formatted_line[MAX_SRC_LINE_SIZE] = { 0 };
        create_formated_line(node_src_line->node_data.data_src_line->str, formatted_line);

        if ((formatted_line[0] == '\0') || (formatted_line[0] == ';') ||
            (formatted_line[0] == ' ') || (formatted_line[0] == '\t'))
        {
            node_src_line = node_src_line->next;
            continue;
        }

        memset(temp_line, 1, MAX_SRC_LINE_SIZE);
        strcpy(temp_line, formatted_line);
        first_token = strtok(temp_line, " ");

        macro_entry = NULL;
        if ((macro_entry = is_macro(first_token, *macros_head_node)))
        {
            /* Remove macro name line*/
            struct node_s* temp_node_src_line = node_src_line;
            node_src_line = node_src_line->prev;
            remove_line(lines_head_node, temp_node_src_line);
            create_formated_line(node_src_line->node_data.data_src_line->str, formatted_line);

            macro_line_number = node_src_line->node_data.data_src_line->line_num;
            for (i = 0; i < macro_entry->num_of_lines; ++i)
            {
                insert_line(lines_head_node, node_src_line, macro_entry->macro_lines[i], macro_line_number + i);
                node_src_line = node_src_line->next;
                create_formated_line(node_src_line->node_data.data_src_line->str, formatted_line);

                if (node_src_line == NULL)
                {
                    return ((failed) ? (0) : (1));
                }
            }

            continue;
        }

        new_macro_entry = NULL;
        if (!strcmp(first_token, MACRO_STR_NAME))
        {
            new_macro_found = 1;

            strcpy(temp_line, formatted_line);
            macro_name = strtok((char*)(temp_line + strlen(MACRO_STR_NAME) + 1), " ");

            if (is_register_name(macro_name) != NULL)
            {
                failed = 1;
                fprintf(stdout, "Line: %d, Error! Macro name can't get a register name.\n", node_src_line->node_data.data_src_line->line_num);
                node_src_line = node_src_line->next;
                continue;
            }

            if (find_instructions(macro_name) != NULL)
            {
                failed = 1;
                fprintf(stdout, "Line: %d, Error! Macro name can't get an instructions name.\n", node_src_line->node_data.data_src_line->line_num);
                node_src_line = node_src_line->next;
                continue;
            }

            if (!strcmp(macro_name, ".data") ||
                !strcmp(macro_name, ".string") ||
                !strcmp(macro_name, ".entry") ||
                !strcmp(macro_name, ".extern"))
            {
                failed = 1;
                fprintf(stdout, "Line: %d, Error! Illegal Macro name (%s).\n", node_src_line->node_data.data_src_line->line_num, macro_name);
                node_src_line = node_src_line->next;
                continue;
            }

            new_macro_entry = (macro_entry_s*)calloc(1, sizeof(macro_entry_s));
            if (new_macro_entry == NULL)
            {
                fprintf(stdout, "Fatal: failed to allocate memory!\n");
                exit(-1);
            }

            new_macro_entry->num_of_lines = 0;
            new_macro_entry->macro_name = (char*)calloc(1, strlen(macro_name) + 1);
            if (new_macro_entry->macro_name == NULL)
            {
                fprintf(stdout, "Fatal: failed to allocate memory!\n");
                exit(-1);
            }

            strcpy(new_macro_entry->macro_name, macro_name);

            append_node(macros_head_node, new_macro_entry);

            /* Remove macro line */
            temp_node_src_line = node_src_line;
            node_src_line = node_src_line->next;
            create_formated_line(node_src_line->node_data.data_src_line->str, formatted_line);
            remove_line(lines_head_node, temp_node_src_line);
            if (node_src_line == NULL)
            {
                return ((failed) ? (0) : (1));
            }
        }

        do {
            if (strcmp(formatted_line, MACRO_STR_NAME_END) && new_macro_found)
            {
                new_macro_entry->macro_lines = (char**)realloc(new_macro_entry->macro_lines, (new_macro_entry->num_of_lines + 1) * sizeof(char*));
                if (new_macro_entry->macro_lines == NULL)
                {
                    fprintf(stdout, "Fatal: failed to allocate memory!\n");
                    exit(-1);
                }

                new_macro_entry->macro_lines[new_macro_entry->num_of_lines] = (char*)calloc(1, strlen(formatted_line) + 1);
                if (new_macro_entry->macro_lines[new_macro_entry->num_of_lines] == NULL)
                {
                    fprintf(stdout, "Fatal: failed to allocate memory!\n");
                    exit(-1);
                }

                strcpy(new_macro_entry->macro_lines[new_macro_entry->num_of_lines], formatted_line);
                new_macro_entry->num_of_lines++;

                /* Remove macro line content */
                temp_node_src_line = node_src_line;
                node_src_line = node_src_line->prev;
                remove_line(lines_head_node, temp_node_src_line);
            }
            else
            {
                if (!(node_src_line = node_src_line->next))
                {
                    return ((failed) ? (0) : (1));
                }

                break;
            }

            if (!(node_src_line = node_src_line->next))
            {
                return ((failed) ? (0) : (1));
            }
            create_formated_line(node_src_line->node_data.data_src_line->str, formatted_line);
        } while (strcmp(formatted_line, MACRO_STR_NAME_END));

        if (!strcmp(formatted_line, MACRO_STR_NAME_END))
        {
            /* Remove end-macro line */
            struct node_s* temp_node_src_line = node_src_line;
            node_src_line = node_src_line->next;
            create_formated_line(node_src_line->node_data.data_src_line->str, formatted_line);
            remove_line(lines_head_node, temp_node_src_line);

            new_macro_found = 0;
        }
    }

    return ((failed) ? (0) : (1));
}

/* Handle Code Macros */
int handle_macros(struct node_s** lines_head_node, char* src_file_name)
{
    int ret_val = 1;
    struct node_s* macros_head = NULL;

    if ((lines_head_node == NULL) || (src_file_name == NULL))
    {
        return 0;
    }

    if ((ret_val = macros_deployment(&macros_head, lines_head_node)) && macros_head)
    {
        save_macro_file(*lines_head_node, src_file_name);
    }

    free_macros(macros_head);

    return ret_val;
}
