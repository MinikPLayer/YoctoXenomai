DESCRIPTION = "Simple xenomai application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

XENOMAI_FILE ?= "xenomai-3.1.tar.bz2"
XENOMAI_URI ?= "https://ftp.denx.de/pub/xenomai/xenomai/stable/${XENOMAI_FILE}"
XENOMAI_SRC_URI ?= "${XENOMAI_URI};downloadfilename=${XENOMAI_FILE};unpack=1"
SRC_URI = " ${XENOMAI_SRC_URI}"
SRC_URI[md5sum] = "b723010a75fa0e87a09908c40a0105e9"

inherit autotools-brokensep pkgconfig
includedir = "/usr/include/xenomai"

EXTRA_OECONF += "--enable-smp --with-core=cobalt"

# do_install_append() {
#     install -d ${D}/usr/include/xenomai
#     cp -R ${S}/include/xenomai/* ${D}/usr/include/xenomai/
# }

FILES_${PN} += " /usr/demo"
FILES_${PN} += " /usr/lib"
FILES_${PN}-dev += " /usr/include/xenomai"