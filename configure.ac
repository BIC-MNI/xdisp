# Require autoconf 2.57 or newer.
AC_PREREQ([2.57])

# The arguments are package name, and package version.
AC_INIT([xdisp],[4.5.1],[Bruce Pike <bruce@bic.mni.mcgill.ca>])
AC_CONFIG_SRCDIR([xdisp.c])

AM_INIT_AUTOMAKE

# The argument is the name of the generated header file.
# It is recommended to leave it as "config.h".
AC_CONFIG_HEADERS([config.h])

# This macro eases building the package; see m4/README.
smr_WITH_BUILD_PATH

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_INSTALL

AC_PATH_XTRA

# Checks for libraries.  See m4/README.
mni_REQUIRE_MINC
mni_REQUIRE_VOLUMEIO

# configure EZWGL
# AC_CONFIG_SUBDIRS(EZWGL-1.50)

# Finish up by writing output files.
# AC_CONFIG_FILES([Makefile])
AC_OUTPUT(epm-header Makefile olgx/Makefile)
