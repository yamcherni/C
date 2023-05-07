#ifndef LINKED_LIST_HEADER_FILE_H
#define LINKED_LIST_HEADER_FILE_H

#include "types.h"


/* Add new node to the end of the list */
void append_node(struct node_s** head_node, void* data);

/* Insert new node right after the required node */
void insert_node(struct node_s** head_node, struct node_s* node, void* data);

/* Remove the required node */
void remove_node(struct node_s** head_node, struct node_s* node);

/* Insert new source code line right after the required line */
void insert_line(struct node_s** lines_head_node, struct node_s* line,
                 char* line_str, unsigned int line_num);

/* Remove the required source code line */
void remove_line(struct node_s** lines_head_node, struct node_s* line_node);

/* Add new symbol node to the end of the list */
void append_symbol(struct node_s** symbols_head_node, char* symbol_name,
                   unsigned int address, symbol_attribute_e attributes);


#endif
