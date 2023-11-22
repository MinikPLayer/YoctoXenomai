SUMMARY = "Provides a way to load and enumerate PKCS#11 modules"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://COPYING;md5=02933887f609807fbb57aa4237d14a50"

inherit autotools gettext pkgconfig gtk-doc

DEPENDS = "libtasn1 libffi"

SRC_URI = "git://github.com/p11-glue/p11-kit \
           file://0001-LINGUAS-drop-the-languages-for-which-upstream-does-n.patch \
           "
SRCREV = "1ba80c67c492f25581ed97c8c31ffb5f20636d06"
S = "${WORKDIR}/git"

AUTOTOOLS_AUXDIR = "${S}/build/litter"

PACKAGECONFIG ??= ""
PACKAGECONFIG[trust-paths] = "--with-trust-paths=/etc/ssl/certs/ca-certificates.crt,--without-trust-paths,,ca-certificates"

# This recipe does not use the standard gtk-doc m4 macros, and so the ./configure flags
# that control gtk-doc build are non-standard
EXTRA_OECONF_prepend_class-target = "${@bb.utils.contains('GTKDOC_ENABLED', 'True', '--enable-doc --enable-doc-html --disable-doc-pdf', \
                                                                                    '--disable-doc', d)} "

# When building native recipes, disable gtkdoc, as it is not necessary,
# pulls in additional dependencies, and makes build times longer
EXTRA_OECONF_prepend_class-native = "--disable-doc "
EXTRA_OECONF_prepend_class-nativesdk = "--disable-doc "

UNKNOWN_CONFIGURE_WHITELIST_append = " --enable-gtk-doc-html --disable-gtk-doc-pdf --enable-gtk-doc --disable-gtk-doc"

# p11-kit relies on these two being copied from source tree
# instead of being regenerated by gtkdoc-scan, but doesn't setup
# dependencies correctly when there is a parallel build. Let's pre-copy
# them instead.
do_compile_prepend () {
        cp ${S}/doc/manual/p11-kit-overrides.txt ${S}/doc/manual/p11-kit-sections.txt ${B}/doc/manual/
}

FILES_${PN} += " \
    ${libdir}/p11-kit-proxy.so \
    ${libdir}/pkcs11/*.so \
    ${libdir}/pkcs11/*.la"

# PN contains p11-kit-proxy.so, a symlink to a loadable module
INSANE_SKIP_${PN} = "dev-so"
