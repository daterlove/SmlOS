#MakeFile定义
OBJS_BOOTPACK = kernel.obj AsmFunc.obj Font.obj GDT_IDT.obj Graphic.obj Interruput.obj FIFO.obj Key_Mouse.obj MemManage.obj SheetManage.obj

TOOLPATH = tools/
INCPATH  = tools/haribote/

NASK     = $(TOOLPATH)nask.exe
EDIMG    = $(TOOLPATH)edimg.exe
MAKE     = $(TOOLPATH)make.exe -r

GCC      = $(TOOLPATH)cc1.exe -I$(INCPATH) -Os -Wall -quiet
GAS2NASK = $(TOOLPATH)gas2nask.exe -a
BIN2OBJ  = $(TOOLPATH)bin2obj.exe
OBJ2BIM  = $(TOOLPATH)obj2bim.exe
BIM2HRB  = $(TOOLPATH)bim2hrb.exe
RULEFILE = $(TOOLPATH)haribote/haribote.rul
EDIMG    = $(TOOLPATH)edimg.exe
IMGTOL   = $(TOOLPATH)imgtol.com

MAKEFONT = $(TOOLPATH)makefont.exe

DEL      = del

#默认生成规则
default :
	$(MAKE) img

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
	
kernel.bim : $(OBJS_BOOTPACK)
	$(OBJ2BIM) @$(RULEFILE) out:kernel.bim stack:3136k map:kernel.map \
		$(OBJS_BOOTPACK)

		
kernel.hrb : kernel.bim 
	$(BIM2HRB) kernel.bim kernel.hrb 0
	
OsHead.bin : OsHead.nas 
	$(NASK) OsHead.nas OsHead.bin OsHead.lst

OS_Kernel.sys : OsHead.bin	kernel.hrb	
	copy /B OsHead.bin+kernel.hrb OS_Kernel.sys
	
#字体生成
Font.bin : Font.txt
	$(MAKEFONT) Font.txt Font.bin
Font.obj : Font.bin Makefile
	$(BIN2OBJ) Font.bin Font.obj _Font
	
SmlOS.img:boot.bin OS_Kernel.sys 
	$(EDIMG) imgin:$(TOOLPATH)fdimg0at.tek \
	wbinimg src:boot.bin len:512 from:0 to:0 \
	copy from:OS_Kernel.sys to:@: \
	imgout:SmlOS.img
	
#一般生成规则
%.gas : %.c Makefile
	$(GCC) -o $*.gas $*.c

%.nas : %.gas Makefile
	$(GAS2NASK) $*.gas $*.nas

%.obj : %.nas Makefile
	$(NASK) $*.nas $*.obj $*.lst

img:
	$(MAKE) -r SmlOS.img

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
	-$(DEL) kernel.gas
	-$(DEL) kernel.nas