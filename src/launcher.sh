qemu-system-i386 \
	 -drive file=boot.img,index=0,if=ide \
	-drive file=disk.img,index=1,if=ide \
	-drive file=disk2.img,index=2,if=ide \
	-audiodev pipewire,id=speaker -machine pcspk-audiodev=speaker
