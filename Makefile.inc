
CYCLONE_LIBS:=arg\
         bitvec\
         buffer\
         character\
         core\
         slowdict\
         dict\
         filename\
         hashtable\
         list\
         queue\
         ref\
         rope\
         set\
         splay\
         string\
         lexing\
         id\
         xarray\
         fn\
         array\
         position\
         pp\
         lineno\
	 stdio\
         assert\
         errno\
         time\
         stdlib


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

CYCLIB:=cyclib.a
CYCCOMP:=cyclone.exe

C_SRCS:=$(addsuffix .c, $(CYCLONE_SRCS))
C_LIBS:=$(addsuffix .c, $(CYCLONE_LIBS))
O_SRCS:=$(addsuffix .o, $(CYCLONE_SRCS))
O_LIBS:=$(addsuffix .o, $(CYCLONE_LIBS))
C_RUNTIME:=$(addsuffix .c, $(RUNTIME))

LDFLAGS:=-lm
