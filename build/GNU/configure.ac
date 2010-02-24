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
m4_include(macros/ax_pthread.m4)
m4_include(macros/libgnutls.m4)

#######################################
#                                     #
#        Autoconf entry point         #
#                                     #
#######################################

AC_INIT([CentreonBroker], 0.9.0, mkermagoret@merethis.com, centreonbroker)

AC_PROG_CXX
AC_LANG(C++)
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

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
##  SOCI
##
SOCI_INCLUDEDIR=/usr/include/soci
SOCI_CPPFLAGS=-I/usr/include/soci
SOCI_LDFLAGS=-lsoci_core-gcc-3_0
AC_SUBST([SOCI_CPPFLAGS])
AC_SUBST([SOCI_LDFLAGS])

##
##  MYSQL
##
AC_MSG_CHECKING([for MySQL support])
AC_ARG_WITH([mysql],
	AC_HELP_STRING([--with-mysql=@<:@ARG@:>@],
		[use MySQL client library @<:@default=yes@:>@, optionally specify path to mysql_config]),
	[if test x"$withval" = "xno"; then
		want_mysql="no"
	elif test x"$withval" = "xyes" ; then
		want_mysql="yes"
	else
		want_mysql="yes"
		MYSQL_CONFIG="$withval"
	fi],
	[want_mysql="yes"])
AC_MSG_RESULT($want_mysql)
if test "$want_mysql" = "yes"; then
	if test -z "$MYSQL_CONFIG" -o test; then
		AC_PATH_PROG([MYSQL_CONFIG], [mysql_config], [no])
	fi
	if test ! -x "$MYSQL_CONFIG"; then
		AC_MSG_ERROR([$MYSQL_CONFIG does not exist or it is not an executable file])
		MYSQL_CONFIG="no"
	fi
	if test "$MYSQL_CONFIG" != "no"; then
		AC_MSG_CHECKING([for MySQL development files])
		MYSQL_CPPFLAGS="`$MYSQL_CONFIG --include` -I$SOCI_INCLUDEDIR/mysql"
		MYSQL_LDFLAGS=-lsoci_mysql-gcc-3_0
		AC_DEFINE([USE_MYSQL],
			[1],
			[Define to 1 if MySQL support is required and available])
		AC_MSG_RESULT([yes])
	fi
fi
AC_SUBST([MYSQL_CPPFLAGS])
AC_SUBST([MYSQL_LDFLAGS])

##
##  PTHREAD
##
AX_PTHREAD(
	[],
	[AC_MSG_ERROR([pthread library is mandatory])])

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
