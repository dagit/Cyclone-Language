
CYCLONE_LIBS=arg\
         bitvec\
         buffer\
         character\
         core\
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
         lineno

CYCLONE_SRCS=absyn\
         parse_tab\
         lex\
         absynpp\
         tcenv\
         tcutil\
         tcstmt\
         tcpat\
         tcexp\
         evexp\
         tc\
         toc\
         cyclone

# Makefiles assume this is only one file, but they could be changed.
RUNTIME=cyc_runtime

CYCLIB=cyclib.a
CYCCOMP=cyclone.exe

C_SRCS=$(addsuffix .c, $(CYCLONE_SRCS))
C_LIBS=$(addsuffix .c, $(CYCLONE_LIBS))
O_SRCS=$(addsuffix .o, $(CYCLONE_SRCS))
O_LIBS=$(addsuffix .o, $(CYCLONE_LIBS))
C_RUNTIME=$(addsuffix .c, $(RUNTIME))
