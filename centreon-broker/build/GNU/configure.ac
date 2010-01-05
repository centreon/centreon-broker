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
m4_include(macros/ax_lib_mysql.m4)
m4_include(macros/ax_lib_oracle_oci.m4)
m4_include(macros/ax_lib_postgresql.m4)
m4_include(macros/ax_pthread.m4)
m4_include(macros/libgnutls.m4)

#######################################
#                                     #
#        Autoconf entry point         #
#                                     #
#######################################

AC_INIT([CentreonBroker], 0.1.0, mkermagoret@merethis.com, centreonbroker)

AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CXX

##
##  DEBUG
##
AC_MSG_CHECKING([whether binaries should be compiled in debug mode])
AC_ARG_WITH([debug],
	AS_HELP_STRING([--with-debug], [enable debug messages]),
	[
		if test x"$withval" = "xyes" ; then
			enable_debug="yes"
		else
			enable_debug="no"
		fi
	],
	[enable_debug="no"])
AC_MSG_RESULT($enable_debug)
if test "x$enable_debug" '!=' 'xyes'
then
	AC_DEFINE(NDEBUG)
fi

##
##  BOOST
##
AX_BOOST_BASE

##
##  PTHREAD
##
AX_PTHREAD(
	[],
	[AC_MSG_ERROR([pthread library is mandatory])])

##
##  MYSQL
##
AX_LIB_MYSQL
AM_CONDITIONAL([MYSQL], [test x"$want_mysql" = "xyes"])

##
##  ORACLE
##
AX_LIB_ORACLE_OCI
if test x"$HAVE_ORACLE_OCI" = "xyes"
then
	AC_DEFINE([USE_ORACLE])
fi
AM_CONDITIONAL([ORACLE], [test x"$HAVE_ORACLE_OCI" = "xyes"])

##
##  POSTGRESQL
##
AX_LIB_POSTGRESQL
AM_CONDITIONAL([POSTGRESQL], [test x"$want_postgresql" = "xyes"])

##
##  TINYXML
##
AC_DEFINE([TIXML_USE_STL])

##
##  TLS
##
AM_LIB_GNUTLS
AM_CONDITIONAL([TLS], [test x"$want_gnutls" = "xyes"])

##
##  OUTPUT
##
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
