DESCRIPTION = "Simple xenomai application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

XENOMAI_FILE ?= "xenomai-3.1.tar.bz2"
XENOMAI_URI ?= "https://ftp.denx.de/pub/xenomai/xenomai/stable/${XENOMAI_FILE}"
XENOMAI_SRC_URI ?= "${XENOMAI_URI};downloadfilename=${XENOMAI_FILE};unpack=1"
SRC_URI = " ${XENOMAI_SRC_URI}"
SRC_URI[md5sum] = "b723010a75fa0e87a09908c40a0105e9"

inherit autotools pkgconfig
includedir = "/usr/include/xenomai"

EXTRA_OECONF += "--enable-smp --with-core=cobalt"

FILES_${PN} += " /usr/lib"
FILES_${PN} += " /usr/include"
FILES_${PN} += " /usr/demo"

# do_configure() {
# 	cd ${WORKDIR}/xenomai-3.1
# 	bbplain "Bootstraping Xenomai"
# 	${WORKDIR}/xenomai-3.1/scripts/bootstrap
# 	bbplain "Configuring Xenomai"
# 	bbplain "Running command ${WORKDIR}/xenomai-3.1/configure CFLAGS=\"-march=armv7-a -mfpu=vfp3 -mfloat-abi=hard\" LDFLAGS=\"-march=armv7-a\" --with-core=cobalt --enable-smp --enable-tls CC=${CC} LD=${LD}"
# 	${WORKDIR}/xenomai-3.1/configure CFLAGS="-march=armv7-a -mfpu=vfp3 -mfloat-abi=hard" LDFLAGS="-march=armv7-a" --with-core=cobalt --enable-smp --enable-tls CC=arm-poky-linux-gnueabi-gcc LD=arm-poky-linux-gnueabi-ld
# 	bbplain "Xenomai done!"
# }