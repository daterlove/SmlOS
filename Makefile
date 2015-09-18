#生成规则
DEL      = del

boot.bin:boot.nas
	tools/nask.exe boot.nas boot.bin boot.lst
	
SmlOS.img:boot.bin
	tools/edimg.exe imgin:tools/fdimg0at.tek \
	wbinimg src:boot.bin len:512 from:0 to:0 imgout:SmlOS.img

img:
	tools/make.exe -r SmlOS.img

run:
	tools/make.exe img
	copy SmlOS.img	tools\qemu\fdimage0.bin
	tools/make.exe -C tools/qemu
	
clean:
	-$(DEL) *.bin
	-$(DEL) *.lst
	-$(DEL) *.obj
	-$(DEL) *.img
	-$(DEL) bootpack.map
	-$(DEL) bootpack.bim
	-$(DEL) bootpack.hrb
	-$(DEL) haribote.sys