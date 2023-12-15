DESCRIPTION = "Simple helloworld application"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://hello.c"

S = "${WORKDIR}"

do_compile() {
        ${CC} hello.c ${LDFLAGS} -o hello
}

do_install() {
	install -d ${D}${bindir}
    install -m 0755 hello ${D}${bindir}
}

python do_prepare_kernel() {	
    # bb.build.exec_func("real_install", d)

    bb.plain("********************");
    bb.plain("*                  *");
    bb.plain("*  Hello, World!   *");
    bb.plain("*                  *");
    bb.plain("********************");
    bb.plain("PV: " + str(d.getVar("PV")))
    bb.plain("PN: " + str(d.getVar("PN")))
    bb.plain("KERNEL_SRC: " + str(d.getVar("KERNEL_PATH")))
    bb.plain("KERNEL_VERSION: " + str(d.getVar("KERNEL_VERSION")))
    bb.plain("P: " + str(d.getVar("P")))
    bb.plain("S: " + str(d.getVar("S")))
    bb.plain("SRCREV: " + str(d.getVar("SRCREV")))
    bb.plain("LINUX_VERSION: " + str(d.getVar("LINUX_VERSION")))
}

addtask prepare_kernel after do_patch before do_configure