DESCRIPTION = "Xenomai kernel for MicroZed"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

COMPATIBLE_MACHINE ?= "microzed-zynq7"
KCONFIG_MODE ?= "alldefconfig"
KBUILD_DEFCONFIG_zynq = "xilinx_zynq_defconfig"

LINUX_VERSION_EXTENSION ?= "-ipipe"
PV = "4.19.82"

SRCREV = "5ee93551c703f8fa1a6c414a7d08f956de311df3"

# Kernel source [mainline]
SRC_URI = "git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git;protocol=https;nobranch=1;name=kernel"

# Append xilinx defconfig
XILINX_DEFCONFIG_URI ?= "https://raw.githubusercontent.com/Xilinx/linux-xlnx/master/arch/arm/configs/xilinx_zynq_defconfig;protocol=https"
SRC_URI += " ${XILINX_DEFCONFIG_URI};downloadfilename=xilinx_zynq_defconfig;unpack=0;name=xilinx_zynq_defconfig"
SRC_URI[xilinx_zynq_defconfig.md5sum] = "2b8627ea25de3cd33b00a69712f66b15"

# == IPIPE SECTION ==
IPIPE_VERSION ?= "${PV}-arm-6"
IPIPE_FILE ?= "ipipe-core-${IPIPE_VERSION}.patch"
SRC_URI += "https://ftp.denx.de/pub/xenomai/ipipe/v4.x/arm/${IPIPE_FILE};protocol=https;apply=no;downloadfilename=ipipe-core.patch;unpack=0;name=ipipe-core"
SRC_URI[ipipe-core.md5sum] = "0666a51560116ee765b2bddcf88114b0"
# == END IPIPE SECTION ==

# == XENOMAI SECTION ==
XENOMAI_VERSION ?= "3.1"
XENOMAI_FILE ?= "xenomai-${XENOMAI_VERSION}.tar.bz2"
XENOMAI_URI ?= "https://ftp.denx.de/pub/xenomai/xenomai/stable/${XENOMAI_FILE}"
XENOMAI_SRC_URI ?= "${XENOMAI_URI};downloadfilename=${XENOMAI_FILE};unpack=1;name=xenomai"
SRC_URI += " ${XENOMAI_SRC_URI}"
SRC_URI[xenomai.md5sum] = "b723010a75fa0e87a09908c40a0105e9"
# == END XENOMAI SECTION ==

SRCREV_machine ?= "${SRCREV}"

require recipes-kernel/linux/linux-yocto.inc

# Force the use of the KBUILD_DEFCONFIG even if some other defconfig was generated in the ${WORKDIR}
do_kernel_metadata_prepend () {
	cp ${WORKDIR}/${KBUILD_DEFCONFIG} ${S}/arch/arm/configs/
	[ -n "${KBUILD_DEFCONFIG}" ] && [ -e ${WORKDIR}/defconfig ] && rm ${WORKDIR}/defconfig
}

do_configure_prepend () {
	if [ -n "${KBUILD_DEFCONFIG}" ] && [ -n "${EXTKERNELSRC}" ]; then
		cp ${S}/arch/${ARCH}/configs/${KBUILD_DEFCONFIG} ${WORKDIR}/defconfig
	fi
}

do_patch() {
	${WORKDIR}/xenomai-${XENOMAI_VERSION}/scripts/prepare-kernel.sh --linux=${S} --ipipe=${WORKDIR}/ipipe-core.patch --arch=arm
}

