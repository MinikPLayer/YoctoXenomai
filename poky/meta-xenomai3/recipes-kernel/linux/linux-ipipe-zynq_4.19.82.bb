DESCRIPTION = "Ipipe-core patched kernel for MicroZed"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

COMPATIBLE_MACHINE ?= "microzed-zynq7"

PV = "4.19.82"
LINUX_VERSION_EXTENSION ?= "-yocto-mainline-ipipe"

# Kernel source [mainline]
SRC_URI = "git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git;nobranch=1;name=kernel"
SRCREV = "5ee93551c703f8fa1a6c414a7d08f956de311df3"

# Append xilinx defconfig
XILINX_DEFCONFIG_URI ?= "https://raw.githubusercontent.com/Xilinx/linux-xlnx/master/arch/arm/configs/xilinx_zynq_defconfig;protocol=https"
SRC_URI += " ${XILINX_DEFCONFIG_URI};downloadfilename=xilinx_zynq_defconfig;unpack=0;name=xilinx_zynq_defconfig"
SRC_URI[xilinx_zynq_defconfig.md5sum] = "2b8627ea25de3cd33b00a69712f66b15"

# == IPIPE ==
IPIPE_VERSION ?= "${PV}-arm-6"
IPIPE_URL ?= "https://ftp.denx.de/pub/xenomai/ipipe/v4.x/arm/ipipe-core-${IPIPE_VERSION}.patch"
IPIPE_DOWNLOAD_PARAMETERS ?= "apply=no;downloadfilename=ipipe-core.patch;unpack=0;name=ipipe-core"
SRC_URI += "${IPIPE_URL};${IPIPE_DOWNLOAD_PARAMETERS}"
SRC_URI[ipipe-core.md5sum] = "0666a51560116ee765b2bddcf88114b0"

# == XENOMAI ==
XENOMAI_VERSION ?= "3.1"
XENOMAI_FILE ?= "xenomai-${XENOMAI_VERSION}.tar.bz2"
XENOMAI_URI ?= "https://ftp.denx.de/pub/xenomai/xenomai/stable/${XENOMAI_FILE}"
XENOMAI_SRC_URI ?= "${XENOMAI_URI};downloadfilename=${XENOMAI_FILE};unpack=1;name=xenomai"
SRC_URI += " ${XENOMAI_SRC_URI}"
SRC_URI[xenomai.md5sum] = "b723010a75fa0e87a09908c40a0105e9"

KCONFIG_MODE ?= "alldefconfig"
KBUILD_DEFCONFIG = "xilinx_zynq_defconfig"
do_kernel_metadata_prepend () {
	cp ${WORKDIR}/${KBUILD_DEFCONFIG} ${S}/arch/arm/configs/${KBUILD_DEFCONFIG}
}

require recipes-kernel/linux/linux-yocto.inc

# == PATCH ==
do_patch() {
	${WORKDIR}/xenomai-${XENOMAI_VERSION}/scripts/prepare-kernel.sh --linux=${S} --ipipe=${WORKDIR}/ipipe-core.patch --arch=arm
}