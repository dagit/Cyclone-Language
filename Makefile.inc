CC=gcc

# Assume with Windows

O := obj
A := lib
EXE := .exe

# If there's no sign of a Cygwin environment, it's unix
ifndef MS
  ifndef CYGWIN
    ifneq ($(OSTYPE),cygwin)
      O := o
      A := a
      EXE :=
    endif
  endif
endif

CYCLONE_LIBS:=arg\
         array\
         assert\
         bitvec\
         buffer\
         core\
         dict\
         errno\
         filename\
         fn\
         hashtable\
         id\
         lexing\
         lineno\
         list\
         position\
         pp\
         queue\
         ref\
         rope\
         set\
         slowdict\
         splay\
	 stdio\
         stdlib\
         string\
         time\
         xarray

CYCLONE_OTHER_H:=config.h\
	ctype.h\
	machine/types.h\
	math.h\
	precore.h\
	reent.h\
	signal.h\
	snprintf.h\
	stddef.h\
	sys/features.h\
	sys/reent.h\
	sys/signal.h\
	sys/types.h\
	types.h

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

CYCLONE_H:=$(addsuffix .h, $(CYCLONE_LIBS)) $(CYCLONE_OTHER_H)

LDFLAGS:=-lm
