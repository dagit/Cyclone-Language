
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
         assert

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
	 control_flow\
         toc\
         new_control_flow\
         cyclone

# Makefiles assume this is only one file, but they could be changed.
RUNTIME:=cyc_runtime

CYCLIB:=cyclib.a
CYCCOMP:=cyclone.exe

C_SRCS:=$(addsuffix .c, $(CYCLONE_SRCS))
C_LIBS:=$(addsuffix .c, $(CYCLONE_LIBS))
O_SRCS:=$(addsuffix .o, $(CYCLONE_SRCS))
O_LIBS:=$(addsuffix .o, $(CYCLONE_LIBS))
C_RUNTIME:=$(addsuffix .c, $(RUNTIME))

LDFLAGS:=-lm
