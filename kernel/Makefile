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

SOURCE_ASM = $(wildcard ${DIR_SRC}/*.S)  
OUTPUT_ASM = $(patsubst %.S, ${DIR_OBJ}/%.s, $(notdir ${SOURCE_ASM})) 
OBJECT_ASM = $(patsubst %.s, ${DIR_OBJ}/%.o, $(notdir ${OUTPUT_ASM})) 
BIN_TARGET = ${DIR_BIN}/${TARGET}

$(info info ${OBJECT_ASM})
${BIN_TARGET}: ${DIR_OBJ}/system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary ${DIR_OBJ}/system ${BIN_TARGET}

${DIR_OBJ}/system: ${OBJECT_ASM} ${OBJECT_C}
	ld -b elf64-x86-64 -o ${DIR_OBJ}/system ${DIR_OBJ}/head.o ${DIR_OBJ}/entry.o ${OBJECT_C} -T kernel.lds 

${DIR_OBJ}/%.o: ${DIR_SRC}/%.c
	$(CC) $(CFLAGS) -c $< -o $@

${DIR_OBJ}/%.s: ${DIR_SRC}/%.S
	$(CC) -E $< > $@ && $(AS) $(AS_FLAGS) -o $(patsubst %.s, %.o, $@) $@

all: system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary system kernel.bin

.PHONY:
clean:
	rm -rf ${DIR_OBJ}/* ${DIR_BIN}/*
