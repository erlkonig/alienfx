dnl --- Process this file with autoconf to produce a configure script.
dnl --- Incept Mon Mar  4 10:14:56 CST 1996
dnl --- Update Wed Feb 19 17:02:15 CST 1997
dnl --- Update 2003-12-17 09:58:44 CST (Dec Wed) c++ rules updated; misc.
dnl --- C. Alex. North-Keys

AC_INIT(configure.ac)
AC_COPYRIGHT([Copyright (C) 2003 C. Alex. North-Keys, All Rights Reserved])

dnl --- paths
dnl AC_PREFIX_DEFAULT($HOME)
lispdir='${datadir}/emacs/site-lisp'
AC_SUBST(lispdir)

AC_CONFIG_HEADER(config.h)

dnl --- Checks for programs.
AC_CHECK_PROGS(MAKE_SHELL, bash sh, sh)
AC_CHECK_PROGS(LD, ld gld, ld)
AC_CHECK_PROGS(CC, gcc, cc)
dnl - look for a specific version of GNU C++
for ac_prog_cxxwver in "c++ -V 3.2" "g++3 -V 3.0.4" ; do
  ac_prog_cxx="`echo $ac_prog_cxxwver | awk '{print $1}'`"
  ac_prog_cxx_reqver="`echo $ac_prog_cxxwver | awk '{print $NF}'`"
  ac_prog_cxx_defver="`$ac_prog_cxx -v 2>&1 |awk '/gcc version/ {print $3}'`"
  if test _"$ac_prog_cxx_reqver" = _"$ac_prog_cxx_defver" ; then
    CCC=$ac_prog_cxx
    break
  else
    case $ac_prog_cxx_defver in
	  $ac_prog_cxx_reqver*)
        CCC=$ac_prog_cxx
        break ;;
    esac
    test -n "$CCC" && break;
  fi
done
unset ac_prog_cxx_defver ac_prog_cxx_reqver ac_prog_cxx ac_prog_cxxwver
dnl -
AC_CHECK_PROGS(CXX, "$CCC" g++3 g++ gcc c++ CC, gcc)
AC_CHECK_PROGS(JAVAC, javac guavac, javac)
AC_CHECK_PROGS(JAVA, java kaffe, java)
AC_CHECK_PROGS(INSTALL_INFO, install-info)
AC_PROG_CPP
AC_PROG_CXXCPP
dnl AC_PROG_INSTALL causes a search for install.sh
dnl AC_PROG_INSTALL
AC_DECL_YYTEXT
AC_PROG_AWK
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PROG_YACC
AC_CHECK_PROGS(XMKMF, xmkmf2 xmkmf)
AC_CHECK_PROG(BDFTOSNF, bdftosnf, bdftosnf)
AC_CHECK_PROG(BDFTOPCF, bdftopcf, bdftopcf)
AC_CHECK_PROG(LATEX, latex)
AC_CHECK_PROG(CONVERTFONT, convertfont, convertfont -f 256)
AC_CHECK_PROG(MAKEINFO, makeinfo, makeinfo)
AC_CHECK_PROG(TEXI2DVI, texi2dvi, texi2dvi)
AC_SUBST(COMPRESS_SUFFIX,gz)
AC_CHECK_PROG(COMPRESS, gzip, gzip --stdout --best)
AC_CHECK_PROG(UNCOMPRESS, gzip, gzip --decompress)
AC_CHECK_PROGS(TAR, gtar tar)

dnl --- Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS( \
	sys/ioctl.h \
	sys/time.h \
	sys/select.h \
	sys/sockio.h \
	sys/byteorder.h \
	sys/statvfs.h \
	rpcsvc/yppasswd.h \
	crypt.h \
	fcntl.h \
	pthread.h \
	stdlib.h \
	stropts.h \
	termios.h \
	unistd.h \
	)

dnl --- Checks for libraries.

dnl add library to LIBS and to HAVE_LIB* defines
dnl AC_ADD_TO_LIBS(LIBRARY)
dnl
AC_DEFUN(AC_ADD_TO_LIBS,
[	LIBS="-l$1 $LIBS"
	AH_TEMPLATE(AS_TR_CPP(HAVE_LIB$1))
])

dnl add flag to LDFLAGS
dnl AC_ADD_TO_LDFLAGS(FLAGS)
dnl
AC_DEFUN(AC_ADD_TO_LDFLAGS, [ LDFLAGS="$LDFLAGS $1" ; export LDFLAGS ])

dnl AC_CHECK_LIB_DUAL performs $4 -and- adds the found lib to LIBS
dnl AC_CHECK_LIB_DUAL(LIBNAME, LIBRARY, FUNCTION  ($1 $2 $3)
dnl         [, LIBRARY-CC-OPTION                  ($4)
dnl            [, ACTION-IF-NOT-FOUND             ($5)
dnl               [, OTHER-LIBRARIES              ($6)
dnl                  [ ADDITIONAL-ACTION-IF-FOUND ($7)
dnl                  ]]]])
dnl (LIBRARY-CC-OPTION should be in -l___ format)
dnl Ex: AC_CHECK_LIB_DUAL(LIBXAW, Xaw, XawInitializeWidgetSet, "-lXaw",,-lX11)
AC_DEFUN(AC_CHECK_LIB_DUAL,
[	AC_CHECK_LIB([$2], [$3],
		[$1=$4]
		[AC_ADD_TO_LIBS([$2])]
		[AC_DEFINE(HAVE_$1,[1],Define to 1 if library $4 exists)]
		[$7]
		,
		[$5], [$6])
])

AC_CHECK_LIB_DUAL(LIBM, m, exp, -lm)
AC_SUBST(LIBM)

AC_CHECK_LIB_DUAL(LIBMALLOC, malloc, malloc, -lmalloc)
AC_SUBST(LIBMALLOC)

AC_CHECK_LIB_DUAL(LIBPTHREAD, pthread, pthread_create, -lpthread)
AC_SUBST(LIBPTHREAD)

AC_CHECK_LIB_DUAL(LIBIMAGESGI, image, getrow, -limage)
AC_SUBST(LIBIMAGESGI)

AC_CHECK_LIB_DUAL(LIBPNG, png, png_create_write_struct, -lpng)
AC_SUBST(LIBPNG)

AC_CHECK_LIB_DUAL(LIBSOCKET, socket, socket, -lsocket)
AC_CHECK_LIB_DUAL(LIBSOCKET, xnet, getsockopt, "$LIBSOCKET -lxnet")
AC_SUBST(LIBSOCKET)

AC_CHECK_LIB_DUAL(LIBSUN, sun, getpwnam, -lsun)
AC_SUBST(LIBSUN)

AC_CHECK_LIB_DUAL(LIBCRYPT, crypt, crypt, -lcrypt)
AC_SUBST(LIBCRYPT)

AC_CHECK_LIB_DUAL(LIBRPCSVC, rpcsvc, xdr_passwd, -lrpcsvc)
AC_SUBST(LIBRPCSVC)

AC_CHECK_LIB_DUAL(LIBCRACK, crack, FascistCheck, -lcrack)
AC_SUBST(LIBCRACK)

dnl unsetting the variable prevents subcheck from using a its cached value.
dnl the unset usage may be non-portable.  Compare to libGL checks later
AC_CHECK_LIB_DUAL(LIBCURSES, curses, initscr, -lcurses,
	unset ac_cv_lib_curses_initscr
 [AC_CHECK_LIB_DUAL(LIBCURSES, curses, initscr, "-lcurses -ltermcap",
	unset ac_cv_lib_curses_initscr
  [AC_CHECK_LIB_DUAL(LIBCURSES, ncurses, initscr, "-lncurses",
	unset ac_cv_lib_curses_initscr
   [AC_CHECK_LIB_DUAL(LIBCURSES, ncurses, initscr, "-lncurses -ltermcap",,
   ,-ltermcap)]
  ,)]
 ,-ltermcap)]
)
AC_SUBST(LIBCURSES)

AC_CHECK_LIB_DUAL(LIBTERMCAP, termcap, tgetent, -ltermcap)
AC_SUBST(LIBTERMCAP)


dnl         XCB
dnl     __  __ 
dnl  \//   |__)
dnl  /\\__ |__)

AC_CHECK_LIB_DUAL(LIBXCB_CORE,   xcb,         xcb_connect,     -lxcb)
AC_SUBST(LIBXCB_CORE)
AC_CHECK_LIB_DUAL(LIBXCB_RENDER, xcb-render,  xcb_render_composite,
                 "-lxcb-render",,-lxcb)
AC_SUBST(LIBXCB_RENDER)
AC_CHECK_LIB_DUAL(LIBXCB_GLX,    xcb-glx,     xcb_glx_render,
                 "-lxcb-glx",,-lxcb)
AC_SUBST(LIBXCB_GLX)
LIBXCB="$LIBXCB_CORE $LIBXCB_RENDER $LIBXCB_GLX"
AC_SUBST(LIBXCB)

AC_CHECK_LIB_DUAL(LIBX11_XCB,    X11-xcb,     XGetXCBConnection,
                 "-lX11-xcb",,-lX11)
AC_SUBST(LIBX11_XCB)

dnl    X
dnl   
dnl  \/
dnl  /\

AC_CHECK_LIB_DUAL(LIBX11, X11, XOpenDisplay, -lX11,
				  unset ac_cv_lib_X11_XOpenDisplay
				  [AC_CHECK_LIB_DUAL(LIBX11,  X11,  XOpenDisplay,
				  					 -lX11,
									 ,
									 -L/usr/X11R6/lib -lX11,
									 [AC_ADD_TO_LDFLAGS([-L/usr/X11R6/lib])]
									 )]
		 		  ,-lX11)
AC_SUBST(LDFLAGS)

AC_CHECK_LIB_DUAL(LIBX11,  X11,  XOpenDisplay,         -lX11)
AC_SUBST(LIBX11)
AC_CHECK_LIB_DUAL(LIBXEXT, Xext, XextCreateExtension, "-lXext",,-lX11)
AC_SUBST(LIBXEXT)
AC_CHECK_LIB_DUAL(LIBXT,   Xt,   XtMainLoop,          "-lXt",,  -lX11)
AC_SUBST(LIBXT)
AC_CHECK_LIB_DUAL(LIBSM,   SM,   SmcOpenConnection,   "-lSM")
AC_SUBST(LIBSM)
AC_CHECK_LIB_DUAL(LIBICE,  ICE,  IceOpenConnection,   "-lICE")
AC_SUBST(LIBICE)
AC_CHECK_LIB_DUAL(LIBXMU,  Xmu,  XmuMakeAtom,         "-lXmu",, -lX11)
AC_SUBST(LIBXMU)
LIBX="$LIBXMU $LIBICE $LIBSM $LIBXT $LIBXEXT $LIBX11"
AC_SUBST(LIBX)

AC_CHECK_LIB_DUAL(LIBXI, Xi, XOpenDevice, "-lXi",,-lX11)
AC_SUBST(LIBXI)

AC_CHECK_LIB_DUAL(LIBXAU, Xau, XauFileName, "-lXau")
AC_SUBST(LIBXAU)

AC_CHECK_LIB_DUAL(LIBIMLIB, Imlib, Imlib_load_image, "-lImlib",,)
AC_SUBST(LIBIMLIB)

AC_CHECK_LIB_DUAL(LIBIMLIB2, Imlib2, imlib_load_image, "-lImlib2",,)
AC_SUBST(LIBIMLIB)

AC_CHECK_LIB_DUAL(LIBXAW, Xaw, XawInitializeWidgetSet, "-lXaw",,-lX11)
AC_SUBST(LIBXAW)

AC_CHECK_LIB_DUAL(LIBXM, Xm, XmCreateMainWindow, "-lXm",,-lX11)
AC_SUBST(LIBXM)

AC_CHECK_LIB_DUAL(LIBXTST, Xtst, XTestFakeKeyEvent, "-lXtst",,-lX11)
AC_SUBST(LIBXTST)

AC_CHECK_LIB_DUAL(LIBIRISGL, gl, doublebuffer, "-lgl",,-lgl -lX11)
AC_SUBST(LIBIRISGL)

ac_libs_prior="$LIBS"
LIBS="`echo $LIBS | sed 's/-lpthread//g'`"
AC_CHECK_LIB_DUAL(LIBGL, GL, glEnable, "-lGL",,)
LIBS="$ac_libs_prior"
if test no = $ac_cv_lib_GL_glEnable ; then
	unset ac_cv_lib_GL_glEnable
	AC_CHECK_LIB_DUAL(LIBGL, GL, glEnable, "-lGL -lpthread",,-lpthread)
else
	LIBS="-lGL $LIBS"
fi
AC_SUBST(LIBGL)

AC_CHECK_LIB_DUAL(LIBGLU, GLU, gluPerspective, "-lGLU",,-lGL -lX11)
AC_SUBST(LIBGLU)
AC_CHECK_LIB_DUAL(LIBGLW, GLw, GLwDrawingAreaSwapBuffers,
				  "-lGLw",,-lGLU -lGL -lX11)
AC_SUBST(LIBGLW)

AC_CHECK_LIB_DUAL(LIBGLUT, glut, glutMainLoop, "-lglut",,-lGLU -lGL -lXi -lXmu)
LIBGLUT="$LIBGLUT -lGLU -lGL -lXi -lXmu"
AC_SUBST(LIBGLUT)

AC_CHECK_LIB_DUAL(LIBGLTK, gltk, tkQuit, "-lgltk",,-lX11)
AC_SUBST(LIBGLTK)

AC_SUBST(LIBS)

dnl --- Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_VOLATILE
AC_C_INLINE
AC_C_CHAR_UNSIGNED
AC_C_LONG_DOUBLE
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_UID_T
AC_TYPE_SIGNAL
AC_TYPE_SIZE_T
AC_TYPE_OFF_T
AC_CACHE_CHECK(for chtype in curses.h, ac_cv_type_chtype,
[AC_EGREP_HEADER(chtype, curses.h, ac_cv_type_chtype=yes, ac_cv_type_chtype=no)])
if test $ac_cv_type_chtype = no; then
	AC_DEFINE(chtype, int, [Define to `int' if <curses.h> doesn't define.])
fi

dnl --- Checks for library functions.
AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(atexit on_exit bzero unsetenv setutent seteuid)
AC_CHECK_FUNCS(usleep sginap yppasswd)
AC_CHECK_FUNCS(tcsetpgrp ttyname setsid ptsname grantpt unlockpt)
AC_CHECK_FUNCS(getauid)
AC_CHECK_FUNCS(getsubopt)

ac_libs_prior="$LIBS"
LIBS="-lm"
AC_CHECK_FUNCS(truncl modfl trunc modf rint)
LIBS="$ac_libs_prior"

dnl --- Checks for functions which might be implemented as macros
dnl AC_CHECK_MACRO_OR_FUNC(MACRO_OR_FUNC_NAME, INCLUDE_FILE, TESTING_STMT,
dnl				 [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl Use this to test for features with are #defined on some systems and
dnl  are full functions on others.  Especially handy for testing that the
dnl  prospective macro takes the expected number of parameters, for
dnl  preprocessors that support this.
AC_DEFUN(AC_CHECK_MACRO_OR_FUNC,
[
	AH_TEMPLATE(AS_TR_CPP(HAVE_$1),
		[Define to 1 if function or macro $1 exists])

	AS_VAR_PUSHDEF([ac_var], [ac_cv_func_$1])dnl
	AC_CACHE_CHECK([for $1], ac_var,
		[AC_LINK_IFELSE(
	       [AC_LANG_PROGRAM([[@%:@include $2]], [[$3]])],
	                [AS_VAR_SET(ac_var, yes)],
	                [AS_VAR_SET(ac_var,  no)])])
	AS_IF([test AS_VAR_GET(ac_var) = yes],
		[AC_DEFINE_UNQUOTED(AS_TR_CPP(HAVE_$1), 1) $4],
		$5)dnl
	AS_VAR_POPDEF([ac_var])dnl
])

AC_CHECK_MACRO_OR_FUNC(getmaxyx, <curses.h>, [{int x,y;getmaxyx(stdscr,y,x);}])
AC_CHECK_MACRO_OR_FUNC(draino,	 <curses.h>, [{draino(0);}])
AC_CHECK_MACRO_OR_FUNC(wresize,	 <curses.h>, [{wresize(stdscr, 24, 80);}])

dnl --- Checks for system services.

dnl --- config.h

AH_TOP(
#define HAVE_CONFIG_H

#ifdef __GNUC__				/* added 1999-09-02 for ptsname(), etc. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#else
#define _XOPEN_SOURCE
#endif
)
AH_BOTTOM(

#ifndef HAVE_USLEEP
#ifdef HAVE_SGINAP
#define usleep(u) sginap(sysconf(3)*(u)/1000000)
#else
#error usleep unavailable
#endif
#endif /* HAVE_USLEEP */

/*------------------------------------------------------------------eof--*/
)

AC_OUTPUT(GNUmakecore)

dnl ---------------------------------------------------------------eof
