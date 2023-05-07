#ifndef READ_SRC_FILE_HEADER_FILE_H
#define READ_SRC_FILE_HEADER_FILE_H

#include "defs.h"
#include "types.h"
#include "linked_list.h"
#include "common.h"


/* Read the source code file */
int read_source_file(char const* const full_file_path, struct node_s** lines_head_node);

#endif
