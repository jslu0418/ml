#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])



# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR
# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([OS.h fcntl.h langinfo.h libintl.h limits.h locale.h netdb.h stddef.h stdint.h stdio_ext.h stdlib.h string.h sys/param.h unistd.h wchar.h wctype.h])

AC_CONFIG_FILES([Makefile
                 src/makefile])
AC_OUTPUT
