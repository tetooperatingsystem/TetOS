qemu-system-i386 \
	-drive file=HERE/boot.img,format=raw,if=ide,index=0 \
	-drive file=HERE/drive.img,format=raw,if=ide,index=1 \
	#-s -S
