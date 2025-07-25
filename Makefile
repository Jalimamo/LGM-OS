GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -fno-stack-protector
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = loader.o gdt.o port.o interruptstubs.o interrupts.o kernel.o

%.o: %.cpp
	g++ $(GPPPARAMS) -o $@ -c $<

%.o: %.s
	as $(ASPARAMS) -o $@ -c $<

lgmkernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

install: lgmkernel.bin
	sudo cp $< /boot/lgmkernel.bin

lgmkernel.iso: lgmkernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' >> iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "LGM-OS" {' >> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/lgmkernel.bin' >> iso/boot/grub/grub.cfg
	echo '	boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -r iso

run: lgmkernel.iso
	(killall VirtualBoxVM&& sleep 1) || true
	VirtualBoxVM --startvm LGM-OS &

#reset:
#	rm *.o *.bin *.iso

cleanAndRun:
	make clean && sleep 1
	make run


.PHONY: clean
clean:
	rm -rf $(objects) lgmkernel.bin lgmkernel.iso