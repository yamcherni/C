CC=gcc
CFLAGS=-Wall -ansi -pedantic

assembler: scanA.o common.o linked_list.o macros.o main.o read_source_file.o scanB.o out_files.o
	$(CC) $(CFLAGS) -o assembler scanA.o common.o linked_list.o macros.o main.o read_source_file.o scanB.o out_files.o
