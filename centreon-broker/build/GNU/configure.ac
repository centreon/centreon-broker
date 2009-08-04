##
##  Copyright 2009 MERETHIS
##  This file is part of CentreonBroker.
##
##  CentreonBroker is free software: you can redistribute it and/or modify it
##  under the terms of the GNU General Public License as published by the Free
##  Software Foundation, either version 2 of the License, or (at your option)
##  any later version.
##
##  CentreonBroker is distributed in the hope that it will be useful, but
##  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
##  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
##  for more details.
##
##  You should have received a copy of the GNU General Public License along
##  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
##
##  For more information : contact@centreon.com
##

#
# Include macro files
#
m4_include(macros/ax_boost_base.m4)
m4_include(macros/ax_boost_system.m4)
m4_include(macros/ax_boost_thread.m4)
m4_include(macros/ax_lib_mysql.m4)
m4_include(macros/ax_lib_oracle_oci.m4)
m4_include(macros/ax_lib_postgresql.m4)
m4_include(macros/libgnutls.m4)

#
# Main code
#
AC_INIT([CentreonBroker], 0.1.0, mkermagoret@merethis.com, centreonbroker)

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX

# Check if program should be debug aware
AC_MSG_CHECKING([whether the program should be compiled in debug mode])
AC_ARG_ENABLE([debug],
	AS_HELP_STRING([--enable-debug], [Debug flags]),
	[enable_debug=$enableval],
	[enable_debug="no"])
AC_MSG_RESULT($enable_debug)
if test "x$enable_debug" = "xyes"
then
	DEBUG_CFLAGS="-g";
else
	AC_DEFINE(NDEBUG)
fi
AC_SUBST([DEBUG_CFLAGS])

# Mandatory Boost dependencies
AX_BOOST_BASE
AX_BOOST_SYSTEM
AX_BOOST_THREAD
# We should check if all these libraries are present. If not, compilation will
# fail. However I don't know how to do it...

# Optional DBMS dependencies
AX_LIB_MYSQL
AX_LIB_ORACLE_OCI
AX_LIB_POSTGRESQL

# Optional TLS support
AC_MSG_CHECKING([whether the program should support TLS])
AC_ARG_ENABLE([tls],
	AS_HELP_STRING([--enable-tls], [TLS code should be compiled]),
	[enable_tls=$enableval],
	[enable_tls="no"])
AC_MSG_RESULT($enable_tls)
if test "x$enable_tls" = "xyes"
then
	AM_PATH_LIBGNUTLS
	AC_DEFINE(USE_TLS)
fi

# Configure tools flags according to previous tests
CPPFLAGS="$CPPFLAGS -I../../inc $BOOST_CPPFLAGS $MYSQL_CPPFLAGS"
CXXFLAGS="-std=c++0x -W -Wall -pedantic -O2 $DEBUG_CFLAGS $ORACLE_OCI_CFLAGS \
	$POSTGRESQL_CFLAGS $LIBGNUTLS_CFLAGS"
LDFLAGS="$LDFLAGS $BOOST_LDFLAGS $MYSQL_LDFLAGS $ORACLE_OCI_LDFLAGS \
	$POSTGRESQL_LDFLAGS $LIBGNUTLS_LIBS"
LIBS="$LIBS $BOOST_ASIO_LIB $BOOST_SYSTEM_LIB $BOOST_THREAD_LIB"

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
