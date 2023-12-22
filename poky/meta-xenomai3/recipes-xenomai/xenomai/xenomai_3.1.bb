DESCRIPTION = "Xenomai userspace libraries"
PV = "3.1"
COMPATIBLE_MACHINE ?= "microzed-zynq7"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "https://ftp.denx.de/pub/xenomai/xenomai/stable/xenomai-${PV}.tar.bz2;unpack=1"
SRC_URI[md5sum] = "b723010a75fa0e87a09908c40a0105e9"

inherit autotools pkgconfig

EXTRA_OECONF += "--enable-smp --with-core=cobalt"
includedir = "/usr/include/xenomai"

FILES_${PN} += " /usr/demo"
FILES_${PN} += " /usr/lib"