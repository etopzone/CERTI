## CERTI acinclude.m4
## Copyright (C) 2001 ONERA
## $Id: acinclude.m4,v 3.0 2002/11/21 01:27:51 breholee Exp $
## 
## This M4 file contains additionnal macros for Autoconf, some of them coming
## from the Autoconf Macro Archive at http://www.gnu.org/software/ac-archive/

AC_DEFUN([AC_CXX_IOSTREAM_HEADER],
[AC_CACHE_CHECK([for ISO C++ headers],
ac_cv_cxx_have_iostream,
	[AC_LANG_SAVE
	AC_LANG_CPLUSPLUS
	AC_TRY_COMPILE([#include <iostream>]
		,[return 0;],
		ac_cv_cxx_have_iostream=yes, ac_cv_cxx_have_iostream=no)
	AC_LANG_RESTORE
	])
if test "$ac_cv_cxx_have_iostream" = yes; then
AC_DEFINE(HAVE_IOSTREAM_HEADER,,[define if the compiler supports ISO C++ iostream header])
fi
])

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
dnl @version $Id: acinclude.m4,v 3.0 2002/11/21 01:27:51 breholee Exp $
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

## Synopsis	AC_CXX_NAMESPACES 
## Version	1.2 (2000/07/19) 
## Author	Luc Maisonobe 
## Description	If the compiler can prevent names clashes using namespaces, 
##		define HAVE_NAMESPACES. 

AC_DEFUN([AC_CXX_NAMESPACES],
          [AC_CACHE_CHECK([for namespaces],
          ac_cv_cxx_namespaces,
          [AC_LANG_SAVE
           AC_LANG_CPLUSPLUS
           AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],
                          [using namespace Outer::Inner; return i;],
           ac_cv_cxx_namespaces=yes, ac_cv_cxx_namespaces=no)
           AC_LANG_RESTORE
          ])
          if test "$ac_cv_cxx_namespaces" = yes; then
            AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
          fi
          ])

## Synopsis 	AC_CXX_HAVE_STD 
## Version 	1.2 (2000/07/19) 
## Author 	Luc Maisonobe 
## Description 	If the compiler supports ISO C++ standard library (i.e., can 
##		include the files iostream, map, iomanip and cmath}), define 
##		HAVE_STD. 

AC_DEFUN([AC_CXX_HAVE_STD],
[AC_CACHE_CHECK(whether the compiler supports ISO C++ standard library,
ac_cv_cxx_have_std,
[AC_REQUIRE([AC_CXX_NAMESPACES])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([#include <iostream>
	#include <map>
	#include <iomanip>
	#include <cmath>
	#ifdef HAVE_NAMESPACES
	using namespace std;
	#endif]
	,[return 0;],
ac_cv_cxx_have_std=yes, ac_cv_cxx_have_std=no)
AC_LANG_RESTORE
])
if test "$ac_cv_cxx_have_std" = yes; then
AC_DEFINE(HAVE_STD,,[define if the compiler supports ISO C++ standard library])
fi
])

## Synopsis 	AC_CXX_BOOL 
## Version 	1.2 (2000/07/19) 
## Author  	Luc Maisonobe 
## Description 	If the compiler recognizes bool as a separate built-in type, 
##		define HAVE_BOOL. Note that a typedef is not a separate type 
##		since you cannot overload a function such that it accepts 
##		either the basic type or the typedef. 

AC_DEFUN([AC_CXX_BOOL],
          [AC_CACHE_CHECK([for bool built-in type],
          ac_cv_cxx_bool,
          [AC_LANG_SAVE
           AC_LANG_CPLUSPLUS
           AC_TRY_COMPILE([
          int f(int  x){return 1;}
          int f(char x){return 1;}
          int f(bool x){return 1;}
          ],[bool b = true; return f(b);],
           ac_cv_cxx_bool=yes, ac_cv_cxx_bool=no)
           AC_LANG_RESTORE
          ])
          if test "$ac_cv_cxx_bool" = yes; then
            AC_DEFINE(HAVE_BOOL,,[define if bool is a built-in type])
          fi
          ])

## +----------------+ ########################################################
## | GLIB/GTKMacros | ########################################################
## +----------------+ ########################################################

dnl AM_PATH_GLIB([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for GLIB, and define GLIB_CFLAGS and GLIB_LIBS, if "gmodule" or 
dnl gthread is specified in MODULES, pass to glib-config
dnl
AC_DEFUN(AM_PATH_GLIB,
[dnl 
dnl Get the cflags and libraries from the glib-config script
dnl
AC_ARG_WITH(glib-prefix,[  --with-glib-prefix=PFX   Prefix where GLIB is installed (optional)],
            glib_config_prefix="$withval", glib_config_prefix="")
AC_ARG_WITH(glib-exec-prefix,[  --with-glib-exec-prefix=PFX Exec prefix where GLIB is installed (optional)],
            glib_config_exec_prefix="$withval", glib_config_exec_prefix="")
AC_ARG_ENABLE(glibtest, [  --disable-glibtest       Do not try to compile and run a test GLIB program],
		    , enable_glibtest=yes)

  if test x$glib_config_exec_prefix != x ; then
     glib_config_args="$glib_config_args --exec-prefix=$glib_config_exec_prefix"
     if test x${GLIB_CONFIG+set} != xset ; then
        GLIB_CONFIG=$glib_config_exec_prefix/bin/glib-config
     fi
  fi
  if test x$glib_config_prefix != x ; then
     glib_config_args="$glib_config_args --prefix=$glib_config_prefix"
     if test x${GLIB_CONFIG+set} != xset ; then
        GLIB_CONFIG=$glib_config_prefix/bin/glib-config
     fi
  fi

  for module in . $4
  do
      case "$module" in
         gmodule) 
             glib_config_args="$glib_config_args gmodule"
         ;;
         gthread) 
             glib_config_args="$glib_config_args gthread"
         ;;
      esac
  done

  AC_PATH_PROG(GLIB_CONFIG, glib-config, no)
  min_glib_version=ifelse([$1], ,0.99.7,$1)
  AC_MSG_CHECKING(for GLIB - version >= $min_glib_version)
  no_glib=""
  if test "$GLIB_CONFIG" = "no" ; then
    no_glib=yes
  else
    GLIB_CFLAGS=`$GLIB_CONFIG $glib_config_args --cflags`
    GLIB_LIBS=`$GLIB_CONFIG $glib_config_args --libs`
    glib_config_major_version=`$GLIB_CONFIG $glib_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    glib_config_minor_version=`$GLIB_CONFIG $glib_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    glib_config_micro_version=`$GLIB_CONFIG $glib_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_glibtest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $GLIB_CFLAGS"
      LIBS="$GLIB_LIBS $LIBS"
dnl
dnl Now check if the installed GLIB is sufficiently new. (Also sanity
dnl checks the results of glib-config to some extent
dnl
      rm -f conf.glibtest
      AC_TRY_RUN([
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.glibtest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_glib_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_glib_version");
     exit(1);
   }

  if ((glib_major_version != $glib_config_major_version) ||
      (glib_minor_version != $glib_config_minor_version) ||
      (glib_micro_version != $glib_config_micro_version))
    {
      printf("\n*** 'glib-config --version' returned %d.%d.%d, but GLIB (%d.%d.%d)\n", 
             $glib_config_major_version, $glib_config_minor_version, $glib_config_micro_version,
             glib_major_version, glib_minor_version, glib_micro_version);
      printf ("*** was found! If glib-config was correct, then it is best\n");
      printf ("*** to remove the old version of GLIB. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If glib-config was wrong, set the environment variable GLIB_CONFIG\n");
      printf("*** to point to the correct copy of glib-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
  else if ((glib_major_version != GLIB_MAJOR_VERSION) ||
	   (glib_minor_version != GLIB_MINOR_VERSION) ||
           (glib_micro_version != GLIB_MICRO_VERSION))
    {
      printf("*** GLIB header files (version %d.%d.%d) do not match\n",
	     GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION, GLIB_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     glib_major_version, glib_minor_version, glib_micro_version);
    }
  else
    {
      if ((glib_major_version > major) ||
        ((glib_major_version == major) && (glib_minor_version > minor)) ||
        ((glib_major_version == major) && (glib_minor_version == minor) && (glib_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of GLIB (%d.%d.%d) was found.\n",
               glib_major_version, glib_minor_version, glib_micro_version);
        printf("*** You need a version of GLIB newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** GLIB is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the glib-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of GLIB, but you can also set the GLIB_CONFIG environment to point to the\n");
        printf("*** correct copy of glib-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_glib=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_glib" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$GLIB_CONFIG" = "no" ; then
       echo "*** The glib-config script installed by GLIB could not be found"
       echo "*** If GLIB was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the GLIB_CONFIG environment variable to the"
       echo "*** full path to glib-config."
     else
       if test -f conf.glibtest ; then
        :
       else
          echo "*** Could not run GLIB test program, checking why..."
          CFLAGS="$CFLAGS $GLIB_CFLAGS"
          LIBS="$LIBS $GLIB_LIBS"
          AC_TRY_LINK([
#include <glib.h>
#include <stdio.h>
],      [ return ((glib_major_version) || (glib_minor_version) || (glib_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GLIB or finding the wrong"
          echo "*** version of GLIB. If it is not finding GLIB, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the GTK package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps gtk gtk-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GLIB was incorrectly installed"
          echo "*** or that you have moved GLIB since it was installed. In the latter case, you"
          echo "*** may want to edit the glib-config script: $GLIB_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GLIB_CFLAGS=""
     GLIB_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GLIB_CFLAGS)
  AC_SUBST(GLIB_LIBS)
  rm -f conf.glibtest
])

# Configure paths for GTK+
# Owen Taylor     97-11-3

dnl AM_PATH_GTK([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND [, MODULES]]]])
dnl Test for GTK, and define GTK_CFLAGS and GTK_LIBS
dnl
AC_DEFUN(AM_PATH_GTK,
[dnl 
dnl Get the cflags and libraries from the gtk-config script
dnl
AC_ARG_WITH(gtk-prefix,[  --with-gtk-prefix=PFX   Prefix where GTK is installed (optional)],
            gtk_config_prefix="$withval", gtk_config_prefix="")
AC_ARG_WITH(gtk-exec-prefix,[  --with-gtk-exec-prefix=PFX Exec prefix where GTK is installed (optional)],
            gtk_config_exec_prefix="$withval", gtk_config_exec_prefix="")
AC_ARG_ENABLE(gtktest, [  --disable-gtktest       Do not try to compile and run a test GTK program],
		    , enable_gtktest=yes)

  for module in . $4
  do
      case "$module" in
         gthread) 
             gtk_config_args="$gtk_config_args gthread"
         ;;
      esac
  done

  if test x$gtk_config_exec_prefix != x ; then
     gtk_config_args="$gtk_config_args --exec-prefix=$gtk_config_exec_prefix"
     if test x${GTK_CONFIG+set} != xset ; then
        GTK_CONFIG=$gtk_config_exec_prefix/bin/gtk-config
     fi
  fi
  if test x$gtk_config_prefix != x ; then
     gtk_config_args="$gtk_config_args --prefix=$gtk_config_prefix"
     if test x${GTK_CONFIG+set} != xset ; then
        GTK_CONFIG=$gtk_config_prefix/bin/gtk-config
     fi
  fi

  AC_PATH_PROG(GTK_CONFIG, gtk-config, no)
  min_gtk_version=ifelse([$1], ,0.99.7,$1)
  AC_MSG_CHECKING(for GTK - version >= $min_gtk_version)
  no_gtk=""
  if test "$GTK_CONFIG" = "no" ; then
    no_gtk=yes
  else
    GTK_CFLAGS=`$GTK_CONFIG $gtk_config_args --cflags`
    GTK_LIBS=`$GTK_CONFIG $gtk_config_args --libs`
    gtk_config_major_version=`$GTK_CONFIG $gtk_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    gtk_config_minor_version=`$GTK_CONFIG $gtk_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    gtk_config_micro_version=`$GTK_CONFIG $gtk_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_gtktest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $GTK_CFLAGS"
      LIBS="$GTK_LIBS $LIBS"
dnl
dnl Now check if the installed GTK is sufficiently new. (Also sanity
dnl checks the results of gtk-config to some extent
dnl
      rm -f conf.gtktest
      AC_TRY_RUN([
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>

int 
main ()
{
  int major, minor, micro;
  char *tmp_version;

  system ("touch conf.gtktest");

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = g_strdup("$min_gtk_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_gtk_version");
     exit(1);
   }

  if ((gtk_major_version != $gtk_config_major_version) ||
      (gtk_minor_version != $gtk_config_minor_version) ||
      (gtk_micro_version != $gtk_config_micro_version))
    {
      printf("\n*** 'gtk-config --version' returned %d.%d.%d, but GTK+ (%d.%d.%d)\n", 
             $gtk_config_major_version, $gtk_config_minor_version, $gtk_config_micro_version,
             gtk_major_version, gtk_minor_version, gtk_micro_version);
      printf ("*** was found! If gtk-config was correct, then it is best\n");
      printf ("*** to remove the old version of GTK+. You may also be able to fix the error\n");
      printf("*** by modifying your LD_LIBRARY_PATH enviroment variable, or by editing\n");
      printf("*** /etc/ld.so.conf. Make sure you have run ldconfig if that is\n");
      printf("*** required on your system.\n");
      printf("*** If gtk-config was wrong, set the environment variable GTK_CONFIG\n");
      printf("*** to point to the correct copy of gtk-config, and remove the file config.cache\n");
      printf("*** before re-running configure\n");
    } 
#if defined (GTK_MAJOR_VERSION) && defined (GTK_MINOR_VERSION) && defined (GTK_MICRO_VERSION)
  else if ((gtk_major_version != GTK_MAJOR_VERSION) ||
	   (gtk_minor_version != GTK_MINOR_VERSION) ||
           (gtk_micro_version != GTK_MICRO_VERSION))
    {
      printf("*** GTK+ header files (version %d.%d.%d) do not match\n",
	     GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);
      printf("*** library (version %d.%d.%d)\n",
	     gtk_major_version, gtk_minor_version, gtk_micro_version);
    }
#endif /* defined (GTK_MAJOR_VERSION) ... */
  else
    {
      if ((gtk_major_version > major) ||
        ((gtk_major_version == major) && (gtk_minor_version > minor)) ||
        ((gtk_major_version == major) && (gtk_minor_version == minor) && (gtk_micro_version >= micro)))
      {
        return 0;
       }
     else
      {
        printf("\n*** An old version of GTK+ (%d.%d.%d) was found.\n",
               gtk_major_version, gtk_minor_version, gtk_micro_version);
        printf("*** You need a version of GTK+ newer than %d.%d.%d. The latest version of\n",
	       major, minor, micro);
        printf("*** GTK+ is always available from ftp://ftp.gtk.org.\n");
        printf("***\n");
        printf("*** If you have already installed a sufficiently new version, this error\n");
        printf("*** probably means that the wrong copy of the gtk-config shell script is\n");
        printf("*** being found. The easiest way to fix this is to remove the old version\n");
        printf("*** of GTK+, but you can also set the GTK_CONFIG environment to point to the\n");
        printf("*** correct copy of gtk-config. (In this case, you will have to\n");
        printf("*** modify your LD_LIBRARY_PATH enviroment variable, or edit /etc/ld.so.conf\n");
        printf("*** so that the correct libraries are found at run-time))\n");
      }
    }
  return 1;
}
],, no_gtk=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_gtk" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$GTK_CONFIG" = "no" ; then
       echo "*** The gtk-config script installed by GTK could not be found"
       echo "*** If GTK was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the GTK_CONFIG environment variable to the"
       echo "*** full path to gtk-config."
     else
       if test -f conf.gtktest ; then
        :
       else
          echo "*** Could not run GTK test program, checking why..."
          CFLAGS="$CFLAGS $GTK_CFLAGS"
          LIBS="$LIBS $GTK_LIBS"
          AC_TRY_LINK([
#include <gtk/gtk.h>
#include <stdio.h>
],      [ return ((gtk_major_version) || (gtk_minor_version) || (gtk_micro_version)); ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding GTK or finding the wrong"
          echo "*** version of GTK. If it is not finding GTK, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"
          echo "***"
          echo "*** If you have a RedHat 5.0 system, you should remove the GTK package that"
          echo "*** came with the system with the command"
          echo "***"
          echo "***    rpm --erase --nodeps gtk gtk-devel" ],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means GTK was incorrectly installed"
          echo "*** or that you have moved GTK since it was installed. In the latter case, you"
          echo "*** may want to edit the gtk-config script: $GTK_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     GTK_CFLAGS=""
     GTK_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(GTK_CFLAGS)
  AC_SUBST(GTK_LIBS)
  rm -f conf.gtktest
])

## +--------------+ ##########################################################
## | Other Macros | ##########################################################
## +--------------+ ##########################################################

# /* This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2, or (at your option)
#    any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.  */

AC_DEFUN(ACX_WITH_GSSAPI,[
#
# Use --with-gssapi[=DIR] to enable GSSAPI support.
#
# defaults to enabled with DIR in default list below
#
# Search for /SUNHEA/ and read the comments about this default below.
#
AC_ARG_WITH([gssapi],
            [  --with-gssapi=value     GSSAPI directory],
            [acx_gssapi_withgssapi=$withval], [acx_gssapi_withgssapi=yes])dnl

dnl
dnl FIXME - cache withval and obliterate later cache values when options change
dnl
#
# Try to locate a GSSAPI installation if no location was specified, assuming
# GSSAPI was enabled (the default).
# 
if test -n "$acx_gssapi_cv_gssapi"; then
  # Granted, this is a slightly ugly way to print this info, but the
  # AC_CHECK_HEADER used in the search for a GSSAPI installation makes using
  # AC_CACHE_CHECK worse
  AC_MSG_CHECKING([for GSSAPI])
else :; fi
AC_CACHE_VAL([acx_gssapi_cv_gssapi], [
if test x$acx_gssapi_withgssapi = xyes; then
  # --with but no location specified
  # assume a gssapi.h or gssapi/gssapi.h locates our install.
  #
  # This isn't always strictly true.  For instance Solaris 7's SUNHEA (header)
  # package installs gssapi.h whether or not the necessary libraries are
  # installed.  I'm still not sure whether to consider this a bug.  The long
  # way around is to not consider GSSPAI installed unless gss_import_name is
  # found, but that brings up a lot of other hassles, like continuing to let
  # gcc & ld generate the error messages when the user uses --with-gssapi=dir
  # as a debugging aid.  The short way around is to disable GSSAPI by default,
  # but I think Sun users have been faced with this for awhile and I haven't
  # heard many complaints.
  acx_gssapi_save_CPPFLAGS=$CPPFLAGS
  for acx_gssapi_cv_gssapi in yes /usr/kerberos /usr/cygnus/kerbnet no; do
    if test x$acx_gssapi_cv_gssapi = xno; then
      break
    fi
    if test x$acx_gssapi_cv_gssapi = xyes; then
      AC_CHECKING([for GSSAPI])
    else
      CPPFLAGS="$acx_gssapi_save_CPPFLAGS -I$acx_gssapi_cv_gssapi/include"
      AC_CHECKING([for GSSAPI in $acx_gssapi_cv_gssapi])
    fi
    unset ac_cv_header_gssapi_h
    unset ac_cv_header_gssapi_gssapi_h
    AC_CHECK_HEADERS([gssapi.h gssapi/gssapi.h])
    if test "$ac_cv_header_gssapi_h" = "yes" ||
        test "$ac_cv_header_gssapi_gssapi_h" = "yes"; then
      break
    fi
  done
  CPPFLAGS=$acx_gssapi_save_CPPFLAGS
else
  acx_gssapi_cv_gssapi=$acx_gssapi_withgssapi
fi
AC_MSG_CHECKING([for GSSAPI])
])dnl
AC_MSG_RESULT([$acx_gssapi_cv_gssapi])

#
# Set up GSSAPI includes for later use.  We don't bother to check for
# $acx_gssapi_cv_gssapi=no here since that will be caught later.
#
if test x$acx_gssapi_cv_gssapi = yes; then
  # no special includes necessary
  GSSAPI_INCLUDES=""
else
  # GSSAPI at $acx_gssapi_cv_gssapi (could be 'no')
  GSSAPI_INCLUDES=" -I$acx_gssapi_cv_gssapi/include"
fi

#
# Get the rest of the information CVS needs to compile with GSSAPI support
#
if test x$acx_gssapi_cv_gssapi != xno; then
  # define HAVE_GSSAPI and set up the includes
  AC_DEFINE([HAVE_GSSAPI],, [Define if you have GSSAPI with Kerberos version 5 available.])
  includeopt=$includeopt$GSSAPI_INCLUDES

  # locate any other headers
  acx_gssapi_save_CPPFLAGS=$CPPFLAGS
  CPPFLAGS=$CPPFLAGS$GSSAPI_INCLUDES
  dnl We don't use HAVE_KRB5_H anywhere, but including it here might make it
  dnl easier to spot errors by reading configure output
  AC_CHECK_HEADERS([gssapi.h gssapi/gssapi.h gssapi/gssapi_generic.h krb5.h])
  # And look through them for GSS_C_NT_HOSTBASED_SERVICE or its alternatives
  AC_CACHE_CHECK([for GSS_C_NT_HOSTBASED_SERVICE], [acx_gssapi_cv_gss_c_nt_hostbased_service],
   [acx_gssapi_cv_gss_c_nt_hostbased_service=no
    if test "$ac_cv_header_gssapi_h" = "yes"; then
      AC_EGREP_HEADER([GSS_C_NT_HOSTBASED_SERVICE], [gssapi.h],
                      [acx_gssapi_cv_gss_c_nt_hostbased_service=yes],
                      AC_EGREP_HEADER([gss_nt_service_name], [gssapi.h],
                                      [acx_gssapi_cv_gss_c_nt_hostbased_service=gss_nt_service_name]))
    fi
    if test $acx_gssapi_cv_gss_c_nt_hostbased_service = no &&
        test "$ac_cv_header_gssapi_gssapi_h" = "yes"; then
      AC_EGREP_HEADER([GSS_C_NT_HOSTBASED_SERVICE], [gssapi/gssapi.h],
                      [acx_gssapi_cv_gss_c_nt_hostbased_service],
                      AC_EGREP_HEADER([gss_nt_service_name], [gssapi/gssapi.h],
                                      [acx_gssapi_cv_gss_c_nt_hostbased_service=gss_nt_service_name]))
    fi
    if test $acx_gssapi_cv_gss_c_nt_hostbased_service = no &&
        test "$ac_cv_header_gssapi_gssapi_generic_h" = "yes"; then
      AC_EGREP_HEADER([GSS_C_NT_HOSTBASED_SERVICE], [gssapi/gssapi_generic.h],
                      [acx_gssapi_cv_gss_c_nt_hostbased_service],
                      AC_EGREP_HEADER([gss_nt_service_name], [gssapi/gssapi_generic.h],
                                      [acx_gssapi_cv_gss_c_nt_hostbased_service=gss_nt_service_name]))
    fi])
  if test $acx_gssapi_cv_gss_c_nt_hostbased_service != yes &&
      test $acx_gssapi_cv_gss_c_nt_hostbased_service != no; then
    # don't define for yes since that means it already means something and
    # don't define for no since we'd rather the compiler catch the error
    AC_DEFINE_UNQUOTED([GSS_C_NT_HOSTBASED_SERVICE], [$acx_gssapi_cv_gss_c_nt_hostbased_service],
[Define to an alternative value if GSS_C_NT_HOSTBASED_SERVICE isn't defined
in the gssapi.h header file.  MIT Kerberos 1.2.1 requires this.  Only relevant
when using GSSAPI.])
  fi
  CPPFLAGS=$acx_gssapi_save_CPPFLAGS

  # Expect the libs to be installed parallel to the headers
  #
  # We could try once with and once without, but I'm not sure it's worth the
  # trouble.
  if test x$acx_gssapi_cv_gssapi != xyes; then
    if test -z "$LIBS"; then
      LIBS="-L$acx_gssapi_cv_gssapi/lib"
    else
      LIBS="-L$acx_gssapi_cv_gssapi/lib $LIBS"
    fi
  else :; fi

  dnl What happens if we want to enable, say, krb5 and some other GSSAPI
  dnl authentication method at the same time?
  #
  # Some of the order below is particular due to library dependencies
  #

  #
  # des                 Heimdal K 0.3d, but Heimdal seems to be set up such
  #                     that it could have been installed from elsewhere.
  #
  AC_SEARCH_LIBS([des_set_odd_parity], [des])

  #
  # com_err             Heimdal K 0.3d
  #
  # com_err             MIT K5 v1.2.2-beta1
  #
  AC_SEARCH_LIBS([com_err], [com_err])

  #
  # asn1                Heimdal K 0.3d          -lcom_err
  #
  AC_SEARCH_LIBS([initialize_asn1_error_table_r], [asn1])

  #
  # resolv              required, but not installed by Heimdal K 0.3d
  #
  # resolv              MIT K5 1.2.2-beta1
  #                     Linux 2.2.17
  #
  AC_SEARCH_LIBS([__dn_expand], [resolv])

  #
  # roken               Heimdal K 0.3d          -lresolv
  #
  AC_SEARCH_LIBS([roken_gethostbyaddr], [roken])

  #
  # k5crypto            MIT K5 v1.2.2-beta1
  #
  AC_SEARCH_LIBS([valid_enctype], [k5crypto])

  #
  # gen                 ? ? ?                   Needed on Irix 5.3 with some
  #                     Irix 5.3                version of Kerberos.  I'm not
  #                                             sure which since Irix didn't
  #                                             get any testing this time
  #                                             around.  Original comment:
  #
  # This is necessary on Irix 5.3, in order to link against libkrb5 --
  # there, an_to_ln.o refers to things defined only in -lgen.
  #
  AC_SEARCH_LIBS([compile], [gen])

  #
  # krb5                ? ? ?                   -lgen -l???
  #                     Irix 5.3
  #
  # krb5                MIT K5 v1.1.1
  #
  # krb5                MIT K5 v1.2.2-beta1     -lcrypto -lcom_err
  #                     Linux 2.2.17
  #
  # krb5                MIT K5 v1.2.2-beta1     -lcrypto -lcom_err -lresolv
  #
  # krb5                Heimdal K 0.3d          -lasn1 -lroken -ldes
  #
  AC_SEARCH_LIBS([krb5_free_context], [krb5])

  #
  # gssapi_krb5         Only lib needed with MIT K5 v1.2.1, so find it first in
  #                     order to prefer MIT Kerberos.  If both MIT & Heimdal
  #                     Kerberos are installed and in the path, this will leave
  #                     some of the libraries above in LIBS unnecessarily, but
  #                     noone would ever do that, right?
  #
  # gssapi_krb5         MIT K5 v1.2.2-beta1     -lkrb5
  #
  # gssapi              Heimdal K 0.3d          -lkrb5
  #
  AC_SEARCH_LIBS([gss_import_name], [gssapi_krb5 gssapi])
fi
])dnl
