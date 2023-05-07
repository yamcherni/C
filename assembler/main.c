#include "defs.h"
#include "common.h"
#include "scanA.h"
#include "scanB.h"
#include "macros.h"
#include "read_source_file.h"
#include "out_files.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>



int main(int argc, char* argv[])
{
    unsigned int IC = INSTRUCTION_CODE_MEM_OFFSET;
    unsigned int ICF = 0;

    unsigned int DC = 0;
    unsigned int DCF = 0;

    const char src_file_extension[] = ".as";
    char** src_file_paths = NULL;
    word_s** data_segment = NULL;
    struct node_s* symbols_head_node = NULL;
    struct node_s* lines_head_node = NULL;
    char* src_file_name = NULL;
    char* full_file_path = NULL;
    char* src_file_path = NULL;

    if (argc < 2)
    {
        fprintf(stdout, "ERROR: Please provide the source file names, as program arguments.\n");
        return -1;
    }

    for (src_file_paths = argv + 1; *src_file_paths != argv[argc]; src_file_paths++)
    {
        IC = INSTRUCTION_CODE_MEM_OFFSET;
        ICF = 0;

        DC = 0;
        DCF = 0;

        data_segment = NULL;
        symbols_head_node = NULL;
        lines_head_node = NULL;
        full_file_path = NULL;

        src_file_path = *src_file_paths;
        fprintf(stdout, "\nScanning %s%s ...\n", src_file_path, src_file_extension);

        full_file_path = (char*)calloc(1, strlen(src_file_path) + strlen(src_file_extension) + 1);
        if (full_file_path == NULL)
        {
            fprintf(stdout, "Fatal: failed to allocate memory!\n");
            return -1;
        }

        strcpy(full_file_path, src_file_path);
        strcat(full_file_path, src_file_extension);

        src_file_name = NULL;
        if (!(src_file_name = get_file_name(src_file_path)))
        {
            fprintf(stdout, "ERROR: Failed to generate file name!\n");
            free(full_file_path);
            continue;
        }

        lines_head_node = NULL;
        if (!read_source_file(full_file_path, &lines_head_node))
        {
            fprintf(stdout, "ERROR: File Not Found! (%s)\n", full_file_path);
            free(full_file_path);
            continue;
        }
        free(full_file_path);

        if (!handle_macros(&lines_head_node, src_file_name))
        {
            free_lines_mem(lines_head_node);
            continue;
        }

        data_segment = NULL;

        /* 1. First scan */
        symbols_head_node = NULL;
        if (scanA(lines_head_node, &symbols_head_node, &data_segment, &IC, &DC, &ICF, &DCF))
        {
            /* 2. Second scan */
            if (scanB(lines_head_node, symbols_head_node))
            {
                /* 8 */
                save_object_file(lines_head_node, data_segment, src_file_name, ICF, DCF);
                save_entry_symbols_file(symbols_head_node, src_file_name);
                save_extern_symbols_file(lines_head_node, symbols_head_node, src_file_name);
            }
        }

        free_data_segment_mem(data_segment, DCF);

        free_symbols_mem(symbols_head_node);

        free_lines_mem(lines_head_node);
    }

    fprintf(stdout, "\nDone!\n\n");
    return 0;
}
