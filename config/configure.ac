AC_INIT([[cyclone]],
        [[0.4]],
        [[cyclone-bugs-l@lists.cs.cornell.edu]])
# Look into the config subdir to find config.guess, config.sub, install-sh
AC_CONFIG_AUX_DIR(config)
# Figure out build system type
AC_CANONICAL_BUILD

# quit if we know the architecture won't work
case "$build_os" in
  cygwin1.3.2)
    echo "Cygwin 1.3.2 is not supported---upgrade to version 1.3.10"
    exit 1
  ;;
  cygwin1.3.3)
    echo "Cygwin 1.3.3 is not supported---upgrade to version 1.3.10"
    exit 1
  ;;
esac

# Starting with autoconf 2.53 abs_srcdir is already defined,
# however, I am running 2.52 so I do it from scratch.
# Note: under Cygwin, pwd can return a path name containing a colon.
#  Such path names don't work in make's VPATH, so we convert them
#  to path names without colons using the cygpath command.
case "$build_os" in
  cygwin*)
    MYPWD=`pwd`
    abs_srcdir=`cygpath -u "$MYPWD"`
  ;;
  *)
    abs_srcdir=`pwd`
  ;;
esac
AC_SUBST(abs_srcdir)

# Figure out compiler characteristics
AC_PROG_CC
# Special cases
if [[ "$build_vendor" = "apple" ]]; then
  CC="$CC -no-cpp-precomp -Wno-long-double"
fi
if [[ "$build_cpu" = "i686" ]]; then
  CC="$CC -march=i686"
fi
if [[ "$build_vendor" = "sun" ]]; then
  LDFLAGS="-lnsl -lsocket -lresolv"
fi

# Cyclone assumes sizeof(int) == sizeof(void *)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(void *)
if [[ "$ac_cv_sizeof_int" != "$ac_cv_sizeof_void_p" ]]; then
  echo "ERROR: cannot build because sizeof(int) == $ac_cv_sizeof_int != sizeof(void *) == $ac_cv_sizeof_void_p"
  exit
fi

# The rest...
AC_SET_MAKE
AC_PROG_RANLIB
AC_CONFIG_FILES(Makefile.inc)
AC_OUTPUT
