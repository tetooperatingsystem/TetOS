
# COMPILE

for file in src/*.s; do
	echo "Assembling: $file"
	out="bin/${file:4:-2}.o"
	nasm -f bin $file -o $out
done

#echo "Creating a 16 MB drive..."

#dd if=/dev/zero of=HERE/drive.img bs=16000000 count=1

#echo "Creating a boot drive..."

#dd if=/dev/zero of=HERE/boot.img bs=2048 count=1

#echo "Creating a FAT16 filesystem..."

#mkfs.vfat -F 16 HERE/drive.img

echo "Writing to sectors."

dd if=bin/bootloader.o of=HERE/boot.img seek=0
echo "Written bootloader to sector 1 of boot."

dd if=bin/fat16.o of=HERE/boot.img seek=1
echo "Written fat16 to sector 2 of boot."







echo "Done!"
echo "Run ./launcher.sh to start."
