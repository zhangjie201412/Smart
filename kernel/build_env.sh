export PATH=/usr/local/arm_linux_4.8/bin:$PATH
#export PATH=/home/jay/nuvoton_bsp/nuc970bsp/uboot/tools:$PATH
#make nuc97x_r4_defconfig
make nuc97x_bf200_defconfig
make -j4
../tool/mkimage -A arm -O linux -T kernel -a 0x7fc0 -e 0x8000 -d ../image/970image 970image.ub
#cp 970image.ub /media/sf_BSP/nuc/
