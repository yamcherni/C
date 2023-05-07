#include "linked_list.h"
#include "types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* Add new node to the end of the list */
void append_node(struct node_s** head_node, void* data)
{
    struct node_s* allocated_node = NULL;

    if ((head_node == NULL) || (data == NULL))
    {
        return;
    }

    allocated_node = (struct node_s*)calloc(1, sizeof(struct node_s));
    if (allocated_node == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    if (*head_node == NULL)
    {
        *head_node = allocated_node;
        (*head_node)->node_data.raw_data = data;
        (*head_node)->next = NULL;
        (*head_node)->prev = NULL;
    }
    else
    {
        struct node_s* current = *head_node;
        struct node_s* next = current->next;

        while (next != NULL)
        {
            current = next;
            next = current->next;
        }

        current->next = allocated_node;
        allocated_node->node_data.raw_data = data;
        allocated_node->next = NULL;
        allocated_node->prev = current;
    }
}

/* Insert new node right after the required node */
void insert_node(struct node_s** head_node, struct node_s* node, void* data)
{
    struct node_s* allocated_node = NULL;

    if ((head_node == NULL) || (node == NULL) || (data == NULL))
    {
        return;
    }

    allocated_node = (struct node_s*)calloc(1, sizeof(struct node_s));
    if (allocated_node == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    if (*head_node == NULL)
    {
        *head_node = allocated_node;
        (*head_node)->node_data.raw_data = data;
        (*head_node)->next = NULL;
        (*head_node)->prev = NULL;
    }
    else
    {
        allocated_node->node_data.raw_data = data;
        allocated_node->next = node->next;
        allocated_node->prev = node;
        allocated_node->next->prev = allocated_node;
        node->next = allocated_node;
    }
}

/* Remove the required node */
void remove_node(struct node_s** head_node, struct node_s* node)
{
    if ((head_node == NULL) || (node == NULL))
    {
        return;
    }

    if (*head_node == node)
    {
        if (node->next != NULL)
        {
            node->next->prev = NULL;
        }

        head_node = &(node->next);
    }
    else
    {
        node->prev->next = node->next;

        if (node->next != NULL)
        {
            node->next->prev = node->prev;
        }
    }
    free(node);
}

/* Insert new source code line right after the required line */
void insert_line(struct node_s** lines_head_node, struct node_s* line, char* line_str, unsigned int line_num)
{
    src_line_s* data_node_line = NULL;
    struct node_s* temp = NULL;

    if ((lines_head_node == NULL) || (line == NULL) || (line_str == NULL))
    {
        return;
    }

    data_node_line = (src_line_s*)calloc(1, sizeof(src_line_s));
    if (data_node_line == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    data_node_line->str = (char*)calloc(1, strlen(line_str) + 1);
    if (data_node_line->str == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    strcpy(data_node_line->str, line_str);
    data_node_line->line_num = line_num;
    insert_node(lines_head_node, line, (void*)data_node_line);

    temp = line->next;
    while (temp)
    {
        temp->node_data.data_src_line->line_num++;
        temp = temp->next;
    }
}

/* Remove the required source code line */
void remove_line(struct node_s** lines_head_node, struct node_s* line_node)
{
    struct node_s* temp = NULL;

    if ((lines_head_node == NULL) || (line_node == NULL))
    {
        return;
    }

    temp = line_node->next;
    free(line_node->node_data.data_src_line->str);
    free(line_node->node_data.data_src_line->formatted_str);
    free(line_node->node_data.data_src_line);
    remove_node(lines_head_node, line_node);

    while (temp)
    {
        temp->node_data.data_src_line->line_num--;
        temp = temp->next;
    }
}

/* Add new symbol node to the end of the list */
void append_symbol(struct node_s** symbols_head_node, char* symbol_name,
                   unsigned int address, symbol_attribute_e attributes)
{
    symbol_entry_s* new_symbol_entry = NULL;

    if ((symbols_head_node == NULL) || (symbol_name == NULL))
    {
        return;
    }

    new_symbol_entry = (symbol_entry_s*)calloc(1, sizeof(symbol_entry_s));
    if (new_symbol_entry == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    new_symbol_entry->symbol_name = (char*)calloc(1, strlen(symbol_name) + 1);
    if (new_symbol_entry->symbol_name == NULL)
    {
        fprintf(stdout, "Fatal: failed to allocate memory!\n");
        exit(-1);
    }

    strcpy(new_symbol_entry->symbol_name, symbol_name);

    new_symbol_entry->symbol_attributes = attributes;
    new_symbol_entry->address = address;
    append_node(symbols_head_node, new_symbol_entry);
}
