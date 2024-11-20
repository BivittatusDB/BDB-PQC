# gcc -o pqc_example pqc_example.c -loqs

CC=gcc
CFLAGS = -loqs
WIN32_TARGET = -o lib_bdb_pqc_win32.dll
WIN64_TARGET = -o lib_bdb_pqc_win64.dll
ELF_TARGET = -o lib_bdb_pqc.elf
OBJ=lib_bdb_pqc.o
SRC=lib_bdb_pqc.c

compile: $(SRC)
	$(CC) $(ELF_TARGET) $(SRC) $(CFLAGS) 

all: compile
