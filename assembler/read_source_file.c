#include "read_source_file.h"
#include "defs.h"
#include "types.h"
#include "linked_list.h"
#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Add new source code line to the end of the list */
static void append_line(struct node_s** lines_head_node, char* line_str, unsigned int line_num)
{
    src_line_s* data_node_line = NULL;

    if ((lines_head_node == NULL) || (line_str == NULL))
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
    append_node(lines_head_node, (void*)data_node_line);
}


/* Read the source code file */
int read_source_file(char const* const full_file_path, struct node_s** lines_head_node)
{
    FILE* file = NULL;
    int i;
    char line_str[MAX_SRC_LINE_SIZE] = { 0 };
    unsigned int line_number = 1;

    if ((full_file_path == NULL) || (lines_head_node == NULL))
    {
        return 0;
    }

    file = fopen(full_file_path, "r");
    if (!file)
    {
        return 0;
    }

    while (fgets(line_str, sizeof(line_str), file))
    {
        size_t actual_line_size = strlen(line_str);
        for (i = 0; i < actual_line_size; ++i)
        {
            if (line_str[i] == '\r' || line_str[i] == '\n')
            {
                line_str[i] = '\0';
            }
        }

        append_line(lines_head_node, line_str, line_number);
        ++line_number;
    }

    fclose(file);

    return 1;
}
