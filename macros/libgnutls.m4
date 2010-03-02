AC_DEFUN([AM_LIB_GNUTLS],
[
	gnutls_path=""
	AC_MSG_CHECKING([for TLS support])
	AC_ARG_WITH(tls,
		AC_HELP_STRING([--with-tls=@<:@ARG@:>@],
			[prefix where libgnutls is installed (optional)]),
		[
			if test x"$withval" = "xno" ; then
				want_gnutls="no"
			elif test x"$withval" = "xyes" ; then
				want_gnutls="yes"
			else
				if test -d "$withval" ; then
					gnutls_path="$withval"
					want_gnutls="yes"
				else
					AC_MSG_ERROR([--with-tls expected directory name])
				fi
			fi
		],
		[want_gnutls="no"])
	AC_MSG_RESULT($want_gnutls)

	AC_ARG_WITH([tls-libdir],
		AS_HELP_STRING([--with-tls-libdir=LIB_DIR],
			[Force given directory for GNU TLS libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your GNU TLS libraries are located.]),
		[
			if test -d "$withval" ; then
				gnutls_lib_path="$withval"
			else
				AC_MSG_ERROR([--with-tls-libdir expected directory name])
			fi
		],
		[gnutls_lib_path=""])

	GNUTLS_CPPFLAGS=""
	GNUTLS_LDFLAGS=""

dnl
dnl Check GNU TLS libraries
dnl

	if test "$want_gnutls" = "yes" ; then
		if test x"$gnutls_path" != "x" ; then
			GNUTLS_LDFLAGS="-L$gnutls_path/lib -lgnutls"
			GNUTLS_CPPFLAGS="-I$gnutls_path/include"
		else
			GNUTLS_LDFLAGS="-lgnutls"
##			for gnutls_path_tmp in /usr /usr/local /opt /opt/local ; do
##				if test -d "$gnutls_path_tmp/include/gnutls" && test -r "$gnutls_path_tmp/include/gnutls" ; then
##					GNUTLS_LDFLAGS="-L$gnutls_path_tmp/lib -lgnutls"
##					GNUTLS_CPPFLAGS="-I$gnutls_path_tmp/include"
##					break;
##				fi
##			done
		fi

dnl Overwrite ld flags if we have required special directory with
dnl --with-gnutls-libdir parameter
		if test x"$gnutls_lib_path" != "x" ; then
			GNUTLS_LDFLAGS="-L$gnutls_lib_path -lgnutls"
		fi

		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $GNUTLS_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $GNUTLS_LDFLAGS"
		export LDFLAGS

		AC_MSG_CHECKING([for GNU TLS library])
		AC_LANG_PUSH(C++)
		AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
					@%:@include <gnutls/gnutls.h>
				]],
				[[ ]])],
			[succeeded=yes],
			[succeeded=no])
		AC_LANG_POP([C++])

		CPPFLAGS="$CPPFLAGS_SAVED"
		LDFLAGS="$LDFLAGS_SAVED"

		AC_MSG_RESULT([$succeeded])
		if test x"$succeeded" = "xyes" ; then
			false
			AC_DEFINE([USE_TLS],
				[1],
				[Define to 1 if TLS support is required and available])
		fi
	fi

	AC_SUBST([GNUTLS_CPPFLAGS])
	AC_SUBST([GNUTLS_LDFLAGS])
])
