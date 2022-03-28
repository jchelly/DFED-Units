# This file is part of SWIFT.
# Copyright (C) 2012 pedro.gonnet@durham.ac.uk.
#               2016 p.w.draper@durham.ac.uk.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Init the project.
AC_INIT([SWIFTUnits],[0.0.1],[])

AC_COPYRIGHT
AC_CONFIG_SRCDIR([src/units.c])
AC_CONFIG_AUX_DIR([./conf/])
AM_INIT_AUTOMAKE([subdir-objects])

# Add local macro collection.
AC_CONFIG_MACRO_DIR([m4])

# Stop default CFLAGS from anyone except the environment.
: ${CFLAGS=""}

# Generate header file.
AM_CONFIG_HEADER(config.h)

# Find and test the compiler.
AC_PROG_CC
AM_PROG_CC_C_O

# Enable POSIX and platform extension preprocessor macros.
AC_USE_SYSTEM_EXTENSIONS

# Add libtool support (now that CC is defined).
LT_INIT

# Need C99 and inline support.
AC_PROG_CC_C99
AC_C_INLINE

# Autoconf stuff.
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_HEADER_STDC

# Check for the libraries we will need.
AC_CHECK_LIB(m,sqrt,,AC_MSG_ERROR(something is wrong with the math library!))

# Check for HDF5. This is required.
AX_LIB_HDF5
if test "$with_hdf5" != "yes"; then
    AC_MSG_ERROR([Could not find a working HDF5 library])
fi

# Check for git, needed for revision stamps.
AC_PATH_PROG([GIT_CMD], [git])
AC_SUBST([GIT_CMD])

# Save the compilation options
AC_DEFINE_UNQUOTED([SWIFT_CONFIG_FLAGS],["$swift_config_flags"],[Flags passed to configure])

# Make sure the latest git revision string gets included, when we are
# working in a checked out repository.
test -d ${srcdir}/.git && touch ${srcdir}/src/version.c

# Generate output.
AC_OUTPUT(Makefile src/Makefile)

# Report general configuration.
AC_MSG_RESULT([
 ------- Summary --------

   $PACKAGE_NAME v.$PACKAGE_VERSION

   Compiler             : $CC
    - flags             : $CFLAGS
   HDF5 enabled         : $with_hdf5

 ------------------------]
)

#  Report any unrecognised options loudly.
if test "$ac_unrecognized_opts" != ""; then
AC_MSG_RESULT([
  *** WARNING: unrecognized options: $ac_unrecognized_opts ***
])
fi
