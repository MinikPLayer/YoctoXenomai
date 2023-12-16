part1=/dev/sdd1
part2=/dev/sdd2

echo "Copying to OUT directory..."
./copy.sh
echo "Mounting partitions..."
sudo mount $part1 /mnt/sd1
sudo mount $part2 /mnt/sd2
cd OUT
echo "Copying bootloaders and kernel image..."
sudo cp boot.bin u-boot.img uEnv.txt uImage zynq-microzed.dtb /mnt/sd1/

echo "Modifying uEnv.txt..."
sudo sed -e "s/run loadfpga && //g" -i /mnt/sd1/uEnv.txt

echo "Copying filesystem..."
sudo tar -C /mnt/sd2/ -xzpf core-image-minimal-microzed-zynq7.tar.gz

echo "Unmounting partitions..."
sudo umount $part1
sudo umount $part2

echo "Done!"
