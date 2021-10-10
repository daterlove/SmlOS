all: system
	objcopy -I elf64-x86-64 -S -R ".eh_frame" -R ".comment" -O binary system kernel.bin

system:	head.o kernel.o 
	ld -b elf64-x86-64 -o system head.o kernel.o -T kernel.lds 

kernel.o: kernel.c
	gcc -mcmodel=large -fno-builtin -m64 -c kernel.c -o kernel.o

head.o:	head.S
	gcc -E head.S > head.s
	as --64 -o head.o head.s

clean:
	rm -rf *.bin *.o head.s system kernel.bin
