# This version extension should match CONFIG_LOCALVERSION in defconfig
XILINX_RELEASE_VERSION ?= ""
LINUX_VERSION_EXTENSION ?= "-xilinx-${XILINX_RELEASE_VERSION}"
# PV = "${LINUX_VERSION}${LINUX_VERSION_EXTENSION}+git${SRCPV}"
PV = "4.19.82"

# Sources, by default allow for the use of SRCREV pointing to orphaned tags/commits
KBRANCH ?= ""
SRCBRANCHARG = "${@['nobranch=1', 'branch=${KBRANCH}'][d.getVar('KBRANCH', True) != '']}"

FILESOVERRIDES_append = ":${XILINX_RELEASE_VERSION}"

# Disable checksum cheks, because of changing PV value
BB_STRICT_CHECKSUM = "0"

#KERNELURI ?= "git://github.com/Xilinx/linux-xlnx.git;protocol=https"
KERNELURI ?= "git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git;protocol=https"
SRC_URI = "${KERNELURI};${SRCBRANCHARG}"

# Append xilinx defconfig
XILINX_DEFCONFIG_URI ?= "https://raw.githubusercontent.com/Xilinx/linux-xlnx/master/arch/arm/configs/xilinx_zynq_defconfig;protocol=https"
SRC_URI += " ${XILINX_DEFCONFIG_URI};downloadfilename=xilinx_zynq_defconfig;unpack=0"

# == IPIPE SECTION ==
IPIPE_VERSION ?= "${PV}-arm-6"
IPIPE_FILE ?= "ipipe-core-${IPIPE_VERSION}.patch"
SRC_URI += "https://ftp.denx.de/pub/xenomai/ipipe/v4.x/arm/${IPIPE_FILE};protocol=https;apply=no;downloadfilename=ipipe-core.patch;unpack=0"
# == END IPIPE SECTION ==

# == XENOMAI SECTION ==
XENOMAI_VERSION ?= "3.1"
XENOMAI_FILE ?= "xenomai-${XENOMAI_VERSION}.tar.bz2"
XENOMAI_URI ?= "https://ftp.denx.de/pub/xenomai/xenomai/stable/${XENOMAI_FILE}"
XENOMAI_SRC_URI ?= "${XENOMAI_URI};downloadfilename=${XENOMAI_FILE};unpack=1"
SRC_URI += " ${XENOMAI_SRC_URI}"
# == END XENOMAI SECTION ==

SRCREV_machine ?= "${SRCREV}"

require recipes-kernel/linux/linux-yocto.inc
require linux-microblaze.inc

DESCRIPTION = "Xilinx Kernel"
LIC_FILES_CHKSUM = "file://COPYING;md5=bbea815ee2795b2f4230826c0c6b8814"

EXTKERNELSRC = "${@'1' if d.getVar('EXTERNALSRC') else ''}"

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

inherit kernel-simpleimage

# Default to be only compatible with specific machines or soc families
COMPATIBLE_MACHINE ?= "^$"
COMPATIBLE_MACHINE_zynq = ".*"
COMPATIBLE_MACHINE_zynqmp = ".*"
COMPATIBLE_MACHINE_microblaze = ".*"
COMPATIBLE_MACHINE_versal = ".*"

# Use DEFCONFIGs for configuring linux-xlnx kernels
KCONFIG_MODE ?= "alldefconfig"
KBUILD_DEFCONFIG_zynqmp = "xilinx_zynqmp_defconfig"
KBUILD_DEFCONFIG_zynq = "xilinx_zynq_defconfig"
KBUILD_DEFCONFIG_microblaze = "mmu_defconfig"
KBUILD_DEFCONFIG_versal = "xilinx_versal_defconfig"

# Add meta-xilinx kmeta, used for MicroBlaze BSP fragments
FILESEXTRAPATHS_prepend := "${THISDIR}:"
SRC_URI_append = " file://xilinx-kmeta;type=kmeta;name=xilinx-kmeta;destsuffix=xilinx-kmeta"

# MicroBlaze BSP fragments
KERNEL_FEATURES_append_kc705-microblazeel = " bsp/kc705-microblazeel/kc705-microblazeel.scc"

KERNEL_FEATURES_append_zynqmp = "${@bb.utils.contains('DISTRO_FEATURES', 'xen', ' bsp/xilinx/xen.scc', '', d)}"

KERNEL_FEATURES_append_zynqmp = "${@' features/overlay/overlay.scc' if d.getVar('FPGA_MNGR_RECONFIG_ENABLE') == '1' else ''}"

KERNEL_FEATURES_append_versal = "${@bb.utils.contains('DISTRO_FEATURES', 'xen', ' bsp/xilinx/xen.scc', '', d)}"

