qemu-system-i386 \
	-drive file=boot.img,format=raw,if=ide,index=0 \
	-drive file=disk.img,format=raw,if=ide,index=1 \
	#-s -S
