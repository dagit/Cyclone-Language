# -*- Makefile -*-

CC=gcc

# Assume with Windows

O := o
A := a
EXE := .exe

# What we do when using VC++
ifdef MS
  O := obj
  A := lib
endif

# If there's no sign of a Cygwin environment, it's unix
ifndef MS
  ifndef CYGWIN
    ifneq ($(OS),Windows_NT)
      ifneq ($(OSTYPE),cygwin)
        O := o
        A := a
        EXE :=
      endif
    endif
  endif
endif


CYCLONE_LIBS:=arg\
	inet\
        array\
        assert\
        bitvec\
        buffer\
        core\
        dict\
	dirent\
        errno\
	fcntl\
        filename\
        fn\
        getopt\
        getopt1\
        hashtable\
        id\
        lexing\
        lineno\
        list\
	mman\
        netdb\
        in\
        position\
        pp\
	pwd\
        queue\
        ref\
        rope\
        set\
	signal\
        slowdict\
        splay\
	stat\
	stdio\
        stdlib\
        string\
        socket\
        time\
	unistd\
        xarray

CYCLONE_H:=arg.h\
	arpa/inet.h\
        array.h\
        assert.h\
        bitvec.h\
        buffer.h\
        core.h\
        dict.h\
	dirent.h\
        errno.h\
	fcntl.h\
        filename.h\
        fn.h\
        getopt.h\
        getopt1.h\
        hashtable.h\
        id.h\
        lexing.h\
        lineno.h\
        list.h\
        netdb.h\
        netinet/in.h\
        position.h\
        pp.h\
        queue.h\
        ref.h\
        rope.h\
        set.h\
	signal.h\
        slowdict.h\
        splay.h\
	stdio.h\
        stdlib.h\
        string.h\
        sys/socket.h\
        time.h\
        xarray.h\
        config.h\
	ctype.h\
	machine/types.h\
	math.h\
	precore.h\
	reent.h\
	signal.h\
	snprintf.h\
	stddef.h\
	sys/features.h\
	sys/mman.h\
	sys/reent.h\
	sys/signal.h\
	sys/stat.h\
	sys/types.h\
	unistd.h

CYCLONE_SRCS:=absyn\
         parse_tab\
         lex\
         absynpp\
	 absyndump\
         tcenv\
         tcutil\
         tcstmt\
         tcpat\
         tcexp\
	 formatstr\
         evexp\
         tc\
         toc\
         tovc\
         cf_flowinfo\
         cf_absexp\
         new_control_flow\
         interface\
	 tcdecl\
	 cyclone

# Makefiles assume this is only one file, but they could be changed.
RUNTIME:=runtime_cyc

CYCLIB:=cyclib.$(A)
CYCCOMP:=cyclone$(EXE)

C_SRCS:=$(addsuffix .c, $(CYCLONE_SRCS))
C_LIBS:=$(addsuffix .c, $(CYCLONE_LIBS))
O_SRCS:=$(addsuffix .$(O), $(CYCLONE_SRCS))
O_LIBS:=$(addsuffix .$(O), $(CYCLONE_LIBS))
C_RUNTIME:=$(addsuffix .c, $(RUNTIME))

LDFLAGS:=-lm
