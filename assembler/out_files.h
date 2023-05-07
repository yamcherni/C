#ifndef OUT_FILES_HEADER_FILE_H
#define OUT_FILES_HEADER_FILE_H

#include "types.h"


/* Save an object file */
void save_object_file(struct node_s* lines_head_node, word_s** data_segment,
                      char* src_file_name, unsigned int ICF, unsigned int DCF);

/* Save an entry file */
void save_entry_symbols_file(struct node_s* symbols_head_node, char* src_file_name);

/* Save an extern file */
void save_extern_symbols_file(struct node_s* lines_head_node, struct node_s* symbols_head_node, char* src_file_name);

#endif
