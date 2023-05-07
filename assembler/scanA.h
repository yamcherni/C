#ifndef FIRST_SCAN_HEADER_FILE_H
#define FIRST_SCAN_HEADER_FILE_H

#include "common.h"


/* First Source Code Scan */
int scanA(struct node_s* lines_head_node, struct node_s** symbols_head_node,
          word_s*** data_segment, unsigned int* IC, unsigned int* DC,
          unsigned int* ICF, unsigned int* DCF);

#endif
