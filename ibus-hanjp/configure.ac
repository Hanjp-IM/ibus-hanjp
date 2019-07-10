# if not 1, append datestamp to the version number.
m4_define([ibus_released], [1])
m4_define([ibus_major_version], [0])
m4_define([ibus_minor_version], [1])
m4_define([ibus_micro_version], [0])
m4_define(ibus_maybe_datestamp,
    m4_esyscmd([if test x]ibus_released[ != x1; then date +.%Y%m%d | tr -d '\n\r'; fi]))

m4_define([ibus_version],
    ibus_major_version.ibus_minor_version.ibus_micro_version[]ibus_maybe_datestamp)

AC_INIT([ibus-hanjp], [ibus_version], [https://github.com/ubuntu-kr/hanjp-im],[ibus-hanjp])
AM_INIT_AUTOMAKE
AC_GNU_SOURCE

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# define PACKAGE_VERSION_* variables
AS_VERSION
AS_NANO
AM_SANITY_CHECK
AM_MAINTAINER_MODE
AM_DISABLE_STATIC
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_MKDIR_P
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_LIBTOOL

IT_PROG_INTLTOOL([0.35.0])

# check ibus
PKG_CHECK_MODULES(IBUS, [
    ibus-1.0 >= 1.3.99
])

# check libhangul
PKG_CHECK_MODULES(HANJP, [
    libhanjp >= 0.1.0
])

PKG_CHECK_MODULES(HANGUL, [
    libhangul >= 0.1.0
])

# check env
AC_PATH_PROG(ENV_PROG, env)
AC_SUBST(ENV_PROG)

# define GETTEXT_* variables
GETTEXT_PACKAGE=ibus-hanjp
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Define to the read-only architecture-independent data directory.])


# OUTPUT files
AC_CONFIG_FILES([po/Makefile.in
Makefile
src/Makefile
src/hanjp.xml.in
icons/Makefile
m4/Makefile
])

AC_OUTPUT
