AC_INIT([[cyclone]],
        [[0.8.2]],
        [[cyclone-bugs-l@lists.cs.cornell.edu]])
# Look into the config subdir to find config.guess, config.sub, install-sh
AC_CONFIG_AUX_DIR(config)
# Figure out system type:
#  build is set to system we are building on,
#  host is set to system the cyclone compiler will run on,
#  target is set to system the cyclone compiler will compile code for.
# target defaults to host defaults to build
# use --target to build a Cyclone cross-compiler
AC_CANONICAL_TARGET

# quit if user wants to cross-compile the compiler itself
if [[ "$build" != "$host" ]]; then
    echo "Cross-compiling the Cyclone compiler itself is not supported"
    exit 1
fi

# quit if we know the architecture won't work
case "$build_os" in
  cygwin1.3.2)
    echo "Cygwin 1.3.2 is not supported---upgrade to latest version"
    exit 1
  ;;
  cygwin1.3.3)
    echo "Cygwin 1.3.3 is not supported---upgrade to latest version"
    exit 1
  ;;
esac

# Starting with autoconf 2.53 abs_srcdir is already defined,
# however, I am running 2.52 so I do it from scratch.
# UPDATE: I had to upgrade to 2.57 so this should no longer be needed.
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
CFLAGS=    # prevents AC_PROG_CC from setting CFLAGS=-g -O2
AC_PROG_CC

# Special cases -- these are for compiling the bin/genfiles
if [[ "$build_vendor" = "apple" ]]; then
  CFLAGS="-no-cpp-precomp -Wno-long-double"
fi
if [[ "$build_cpu" = "i686" ]]; then
  CFLAGS=-march=i686
fi
if [[ "$build_vendor" = "sun" ]]; then
  LDFLAGS="-lnsl -lsocket -lresolv"
fi

# Special cases -- these are for compiling the C output of Cyclone
TARGET_CC="gcc -b $target"
TARGET_AR="ar"
TARGET_RANLIB="ranlib"
if [[ "$target_vendor" = "apple" ]]; then
  TARGET_CFLAGS="-no-cpp-precomp -Wno-long-double"
fi
if [[ "$target_cpu" = "i686" ]]; then
  TARGET_CFLAGS="-march=i686"
fi
if [[ "$target_vendor" = "sun" ]]; then
  TARGET_LDFLAGS="-lnsl -lsocket -lresolv"
fi
if [[ "$target" = "arm-unknown-elf" ]]; then
  # because the cross gcc is called arm-elf-gcc, not arm-unknown-elf-gcc-3.2
  target="arm-elf"
  TARGET_CC="arm-elf-gcc"
#  TARGET_CFLAGS="-elf2flt" # for ipod but not GameBoy devkitARM
  TARGET_AR="arm-elf-ar"
  TARGET_RANLIB="arm-elf-ranlib"
fi
if [[ "$target" = "arm-agb-elf" ]]; then
  # because the cross gcc is called arm-agb-elf-gcc, not arm-agb-elf-gcc-3.2
  TARGET_CC="arm-agb-elf-gcc"
  TARGET_AR="arm-agb-elf-ar"
  TARGET_RANLIB="arm-agb-elf-ranlib"
fi
if [[ "$target" = "arm-gba-elf" ]]; then
  # because the cross gcc is called arm-elf-gcc, not arm-unknown-elf-gcc-3.2
  target="arm-elf"
  TARGET_CC="arm-elf-gcc"
  TARGET_CFLAGS="-mthumb -mthumb-interwork"
  TARGET_AR="arm-elf-ar"
  TARGET_RANLIB="arm-elf-ranlib"
fi
AC_SUBST(TARGET_CC)
AC_SUBST(TARGET_AR)
AC_SUBST(TARGET_RANLIB)
AC_SUBST(TARGET_CFLAGS)
AC_SUBST(TARGET_LDFLAGS)

# Cyclone makes some assumptions about the size of types.
# We believe the genfiles will not build a proper compiler if the
# assumptions are violated, so we check for this at configure time.
# FIX: we should check for target too??
AC_CHECK_SIZEOF(char)
if [[ "$ac_cv_sizeof_char" != 1 ]]; then
  echo "ERROR: cannot build because sizeof(char) == $ac_cv_sizeof_char != 1"
  exit
fi
AC_CHECK_SIZEOF(short)
if [[ "$ac_cv_sizeof_short" != 2 ]]; then
  echo "ERROR: cannot build because sizeof(short) == $ac_cv_sizeof_short != 2"
  exit
fi
AC_CHECK_SIZEOF(int)
if [[ "$ac_cv_sizeof_int" != 4 ]]; then
  echo "ERROR: cannot build because sizeof(int) == $ac_cv_sizeof_int != 4"
  exit
fi
AC_CHECK_SIZEOF(long)
if [[ "$ac_cv_sizeof_long" != 4 ]]; then
  echo "ERROR: cannot build because sizeof(long) == $ac_cv_sizeof_long != 4"
  exit
fi
AC_CHECK_SIZEOF(long long)
if [[ "$ac_cv_sizeof_long_long" != 8 ]]; then
  echo "ERROR: cannot build because sizeof(long long) == $ac_cv_sizeof_long_long != 8"
  exit
fi
AC_CHECK_SIZEOF(void *)
if [[ "$ac_cv_sizeof_void_p" != 4 ]]; then
  echo "ERROR: cannot build because sizeof(void *) == $ac_cv_sizeof_void_p != 4"
  exit
fi

# The compiler needs to know the size of wchar_t...
AC_CHECK_SIZEOF(wchar_t)
AC_SUBST(ac_cv_sizeof_wchar_t)
# ... and whether it is unsigned
AC_C_WCHAR_T_UNSIGNED

# Check for threads.  If available, set HAVE_PTHREADS=yes for use in
# the Makefiles.
# The next line is not sufficient:
#  AC_CHECK_HEADERS([pthread.h],[ac_cv_pthreads=pthreads],[ac_cv_pthreads=no])
# because some systems define an empty pthreads.h.  So we look for a
# specific type we use from pthread.h, tlocal_key_t.
AC_CHECK_TYPE(tlocal_key_t,[ac_cv_pthreads=pthreads],[ac_cv_pthreads=no],[pthread.h])
if [[ "$ac_cv_pthreads" = "pthreads" ]]; then
  HAVE_PTHREAD=yes
else
  HAVE_PTHREAD=no
fi
AC_SUBST(HAVE_PTHREAD)


# The rest...
AC_SET_MAKE
AC_PROG_RANLIB

# configure the garbage collector (without threads; configured later)
# don't use AC_CONFIG_SUBDIRS(gc) because it can't pass flags
(cd gc; ./configure --enable-threads=no --enable-shared=no --enable-cplusplus=no)

AC_CONFIG_FILES(Makefile.inc)
AC_OUTPUT
