#生成规则
TOOLPATH = tools/
INCPATH  = tools/haribote/

NASK     = $(TOOLPATH)nask.exe
EDIMG    = $(TOOLPATH)edimg.exe

GCC      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com

DEL      = del

boot.bin:boot.nas
	$(NASK) boot.nas boot.bin boot.lst

kernel.gas : kernel.c Makefile
	$(GCC) -o kernel.gas kernel.c
	
kernel.nas : kernel.gas Makefile
	$(GAS2NASK) kernel.gas kernel.nas
	
kernel.obj : kernel.nas 
	$(NASK) kernel.nas kernel.obj kernel.lst

AsmFunc.obj : AsmFunc.nas 
	$(NASK) AsmFunc.nas AsmFunc.obj AsmFunc.lst
	
kernel.bim : kernel.obj AsmFunc.obj
	$(OBJ2BIM) @$(RULEFILE) out:kernel.bim stack:3136k map:kernel.map \
		kernel.obj AsmFunc.obj

		
kernel.hrb : kernel.bim 
	$(BIM2HRB) kernel.bim kernel.hrb 0
	
OsHead.bin : OsHead.nas 
	$(NASK) OsHead.nas OsHead.bin OsHead.lst

OS_Kernel.sys : OsHead.bin	kernel.hrb	
	copy /B OsHead.bin+kernel.hrb OS_Kernel.sys
	
SmlOS.img:boot.bin OS_Kernel.sys 
	$(EDIMG) imgin:$(TOOLPATH)fdimg0at.tek \
	wbinimg src:boot.bin len:512 from:0 to:0 \
	copy from:OS_Kernel.sys to:@: \
	imgout:SmlOS.img
	

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
	-$(DEL) *.map
	-$(DEL) *.bim
	-$(DEL) *.hrb
	-$(DEL) *.sys