## CERTI acinclude.m4
## Copyright (C) 2001 ONERA
## $Id: acinclude.m4,v 3.1 2003/10/02 16:57:47 breholee Exp $
## 
## This M4 file contains additionnal macros for Autoconf, some of them coming
## from the Autoconf Macro Archive at http://www.gnu.org/software/ac-archive/

## +--------------------------------------------+ ############################
## | Macros from the GNU Autoconf Macro Archive | ############################
## +--------------------------------------------+ ############################

## Please note that every macro contained in this archive is
## copyrighted by its respective author, unless the macro source
## explicitely says otherwise. Permission has been granted, though, to
## use and distribute all macros under the following license, which is
## a modified version of the GNU General Public License version 2:
##
## Every Autoconf macro presented on this web site is free software; you
## can redistribute it and/or modify it under the terms of the GNU
## General Public License as published by the Free Software Foundation;
## either version 2, or (at your option) any later version.
## 
## They are distributed in the hope that they will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
## for more details. (You should have received a copy of the GNU General
## Public License along with this program; if not, write to the Free
## Software Foundation, Inc., 59 Temple Place -- Suite 330, Boston, MA
## 02111-1307, USA.)
## 
## As a special exception, the Free Software Foundation gives unlimited
## permission to copy, distribute and modify the configure scripts that
## are the output of Autoconf. You need not follow the terms of the GNU
## General Public License when using or distributing such scripts, even
## though portions of the text of Autoconf appear in them. The GNU
## General Public License (GPL) does govern all other use of the material
## that constitutes the Autoconf program.
## 
## Certain portions of the Autoconf source text are designed to be copied
## (in certain cases, depending on the input) into the output of
## Autoconf. We call these the "data" portions. The rest of the Autoconf
## source text consists of comments plus executable code that decides
## which of the data portions to output in any given case. We call these
## comments and executable code the "non-data" portions. Autoconf never
## copies any of the non-data portions into its output.
## 
## This special exception to the GPL applies to versions of Autoconf
## released by the Free Software Foundation. When you make and distribute
## a modified version of Autoconf, you may extend this special exception
## to the GPL to apply to your modified version as well, *unless* your
## modified version has the potential to copy into its output some of the
## text that was the non-data portion of the version that you started
## with. (In other words, unless your change moves or copies text from
## the non-data portions to the data portions.) If your modification has
## such potential, you must delete any notice of this special exception
## to the GPL from your modified version.

## ---------------------------------------------------------------------------
## Synopsis 	TYPE_SOCKLEN_T 
## Version 	1.2 (2000/07/19) 
## Author 	Lars Brinkhoff <lars@nocrew.org> 
## Description 	Check whether sys/socket.h defines type socklen_t. Please
##		note that some systems require sys/types.h to be included 
##		before sys/socket.h can be compiled.
AC_DEFUN([AC_TYPE_SOCKLEN_T],
[AC_CACHE_CHECK([for socklen_t], ac_cv_type_socklen_t,
[
  AC_TRY_COMPILE(
  [#include <sys/types.h>
   #include <sys/socket.h>],
  [socklen_t len = 42; return 0;],
  ac_cv_type_socklen_t=yes,
  ac_cv_type_socklen_t=no)
])
  if test $ac_cv_type_socklen_t != yes; then
    AC_DEFINE(socklen_t, int)
  fi
])

## ---------------------------------------------------------------------------
dnl @synopsis ETR_SOCKET_NSL
dnl
dnl This macro figures out what libraries are required on this platform
dnl to link sockets programs.  It's usually -lsocket and/or -lnsl or
dnl neither.  We test for all three combinations.
dnl
dnl @version $Id: acinclude.m4,v 3.1 2003/10/02 16:57:47 breholee Exp $
dnl @author Warren Young <warren@etr-usa.com>
dnl
AC_DEFUN([AC_ETR_SOCKET_NSL],
[
AC_CACHE_CHECK(for libraries containing socket functions,
ac_cv_socket_libs, [
        oCFLAGS=$CFLAGS

        AC_TRY_LINK([
                        #include <sys/types.h>
                        #include <sys/socket.h>
                        #include <netinet/in.h>
                        #include <arpa/inet.h>
                ],
                [
                        struct in_addr add;
                        int sd = socket(AF_INET, SOCK_STREAM, 0);
                        inet_ntoa(add);
                ],
                ac_cv_socket_libs=-lc, ac_cv_socket_libs=no)

        if test x"$ac_cv_socket_libs" = "xno"
        then
                CFLAGS="$oCFLAGS -lsocket"
                AC_TRY_LINK([
                                #include <sys/types.h>
                                #include <sys/socket.h>
                                #include <netinet/in.h>
                                #include <arpa/inet.h>
                        ],
                        [
                                struct in_addr add;
                                int sd = socket(AF_INET, SOCK_STREAM, 0);
                                inet_ntoa(add);
                        ],
                        ac_cv_socket_libs=-lsocket, ac_cv_socket_libs=no)
        fi

        if test x"$ac_cv_socket_libs" = "xno"
        then
                CFLAGS="$oCFLAGS -lsocket -lnsl"
                AC_TRY_LINK([
                                #include <sys/types.h>
                                #include <sys/socket.h>
                                #include <netinet/in.h>
                                #include <arpa/inet.h>
                        ],
                        [
                                struct in_addr add;
                                int sd = socket(AF_INET, SOCK_STREAM, 0);
                                inet_ntoa(add);
                        ],
                        ac_cv_socket_libs="-lsocket -lnsl", ac_cv_socket_libs=no)
        fi

        CFLAGS=$oCFLAGS
])

        if test x"$ac_cv_socket_libs" = "xno"
        then
                AC_MSG_ERROR([Cannot find socket libraries])
        elif test x"$ac_cv_socket_libs" = "x-lc"
        then
                ETR_SOCKET_LIBS=""
        else
                ETR_SOCKET_LIBS="$ac_cv_socket_libs"
        fi

        AC_SUBST(ETR_SOCKET_LIBS)
]) dnl AC_ETR_SOCKET_NSL
