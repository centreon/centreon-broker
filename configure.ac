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
AC_ARG_WITH([soci],
	AC_HELP_STRING([--with-soci=@<:@ARG@:>@], [specify SOCI directory if not in standard path]),
	[SOCI_DIR="$withval"],
	[SOCI_DIR=/usr])
# Search for soci.h
found_soci="no"
for includedir in "$SOCI_DIR"/include "$SOCI_DIR"/include/soci "$SOCI_DIR"/src/core ; do
	if test "$found_soci" = "no" ; then
		AC_CHECK_FILE([$includedir/soci.h],
			[SOCI_CPPFLAGS="-I$includedir"
			AC_SUBST([SOCI_CPPFLAGS])
			found_soci="yes"],
			[])
	fi
done
if test "$found_soci" = "no" ; then
	AC_MSG_ERROR([could not find soci.h in $SOCI_DIR])
fi
# Search for libsoci_core.a
found_soci="no"
for libdir in "$SOCI_DIR/lib" "$SOCI_DIR/src/core" ; do
	for lib in "$libdir"/libsoci_core.a "$libdir"/libsoci_core-gcc-3_0.a ; do
		if test "$found_soci" = "no" ; then
			AC_CHECK_FILE([$lib],
				[SOCI_LDFLAGS="-ldl -L$libdir -l`basename $lib | cut -c 4- | rev | cut -c 3- | rev`"
				AC_SUBST([SOCI_LDFLAGS])
				found_soci="yes"])
		fi
	done
done
if test "$found_soci" = "no" ; then
	AC_MSG_ERROR([could not find libsoci_core*.a in $SOCI_DIR])
fi

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
# Search for mysql_config
AC_MSG_RESULT($want_mysql)
if test "$want_mysql" = "yes"; then
	if test -z "$MYSQL_CONFIG" -o test; then
		AC_PATH_PROG([MYSQL_CONFIG], [mysql_config], [no])
	fi
	if test ! -x "$MYSQL_CONFIG"; then
		AC_MSG_ERROR([$MYSQL_CONFIG does not exist or it is not an executable file])
	fi
	MYSQL_CPPFLAGS="`$MYSQL_CONFIG --include`"
# Search for soci-mysql.h
	found_soci_mysql="no"
	for includedir in "$SOCI_DIR"/include "$SOCI_DIR"/include/soci "$SOCI_DIR"/include/soci/mysql "$SOCI_DIR"/src/backends/mysql ; do
		if test "$found_soci_mysql" = "no" ; then
			AC_CHECK_FILE([$includedir/soci-mysql.h],
				[MYSQL_CPPFLAGS+=" -I$includedir"
				found_soci_mysql="yes"])
		fi
	done
	if test "$found_soci_mysql" = "no" ; then
		AC_MSG_ERROR([could not find soci-mysql.h in $SOCI_DIR])
	fi
# Search for libsoci_mysql.a
	found_soci_mysql="no"
	for libdir in "$SOCI_DIR/lib" "$SOCI_DIR/src/backends/mysql" ; do
		for lib in "$libdir"/libsoci_mysql.a "$libdir"/libsoci_mysql-gcc-3_0.a ; do
			if test "$found_soci_mysql" = "no" ; then
				AC_CHECK_FILE([$lib],
					[MYSQL_LDFLAGS="-L$libdir -l`basename $lib | cut -c 4- | rev | cut -c 3- | rev`"
					AC_SUBST([MYSQL_LDFLAGS])
					found_soci_mysql="yes"])
			fi
		done
	done
	if test "$found_soci_mysql" = "no" ; then
		AC_MSG_ERROR([could not find libsoci_mysql*.a in $SOCI_DIR])
	fi
	AC_DEFINE([USE_MYSQL],
		[1],
		[Define to 1 if MySQL support is required and available])
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
##  TLS
##
AM_LIB_GNUTLS
AM_CONDITIONAL([TLS], [test x"$want_gnutls" = "xyes"])

##
##  OUTPUT
##
AC_CONFIG_FILES([Makefile])

AC_OUTPUT
