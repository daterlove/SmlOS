TARGET = kernel.bin

DIR_SRC = ./src
DIR_OBJ = ./obj
DIR_BIN = ./bin

CC = gcc
CFLAGS = -mcmodel=large -fno-builtin -m64
AS = as
AS_FLAGS = --64

SOURCE_C = $(wildcard ${DIR_SRC}/*.c)  
OBJECT_C = $(patsubst %.c, ${DIR_OBJ}/%.o, $(notdir ${SOURCE_C})) 
BIN_TARGET = ${DIR_BIN}/${TARGET}

${BIN_TARGET}: ${DIR_OBJ}/system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary ${DIR_OBJ}/system ${BIN_TARGET}

${DIR_OBJ}/system: ${DIR_OBJ}/head.o ${OBJECT_C}
	ld -b elf64-x86-64 -o ${DIR_OBJ}/system ${DIR_OBJ}/head.o ${OBJECT_C} -T kernel.lds 

${DIR_OBJ}/%.o: ${DIR_SRC}/%.c
	$(CC) $(CFLAGS) -c $< -o $@

all: system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary system kernel.bin

${DIR_OBJ}/head.o: ${DIR_SRC}/head.S
	$(CC) -E ${DIR_SRC}/head.S > ${DIR_OBJ}/head.s
	$(AS) $(AS_FLAGS) -o ${DIR_OBJ}/head.o ${DIR_OBJ}/head.s

.PHONY:
clean:
	rm -rf ${DIR_OBJ}/* ${DIR_BIN}/*
