AC_PREREQ(2.61)
AC_INIT([libamivideo], m4_esyscmd([echo -n $(cat ./version)$VERSION_SUFFIX]))
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([1.7.2 -Wall -Werror -Wno-extra-portability])

# Checks for programs
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

# Output
AC_CONFIG_FILES([
Makefile
src/libamivideo.pc
src/Makefile
src/libamivideo/Makefile
tests/Makefile
])
AC_OUTPUT
