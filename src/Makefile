
# We assume the library exists -- it's your responsibility to
# make that directory if it's not.  (The Makefile in the parent
# directory does this.)

include ../Makefile.inc

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC=../bin/$(CYCCOMP)

CYCLEX=../bin/cycocamllex.exe
CYCBISON=../bin/cycbison.exe

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

CFLAGS=-g -fwritable-strings -I.. 

CYC_SRCS=$(addsuffix .cyc, $(CYCLONE_SRCS))

CYC_FLAGS=-I ../lib -tc -toc -pp

LIB=../lib/$(OUT_PREFIX)$(CYCLIB)

all: $(OUT_PREFIX)$(CYCCOMP)

$(OUT_PREFIX)$(CYCCOMP): $(addprefix $(OUT_PREFIX), $(O_SRCS)) $(LIB)
	gcc -g -o $@ $^ ../bin/gc.a

# For machine-generated we need special targets (the .cyc is prefixed)
$(OUT_PREFIX)lex.c : $(OUT_PREFIX)lex.cyc
	$(CYCC) $(CYC_FLAGS) $^ > $@
$(OUT_PREFIX)lex.cyc : lex.cyl
	$(CYCLEX) $< $@

$(OUT_PREFIX)parse_tab.c : $(OUT_PREFIX)parse_tab.cyc
	$(CYCC) $(CYC_FLAGS) $< > $@
$(OUT_PREFIX)parse_tab.cyc : parse.y
	$(CYCBISON) -v -d $< -o $@
$(OUT_PREFIX)parse_tab.h : parse.y
	$(CYCBISON) -v -d $< -o $@

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.c : %.cyc
	$(CYCC) $(CYC_FLAGS) $< > $@

# delete generated files with a particular prefix
clean_prefix:
	rm -f $(addprefix $(OUT_PREFIX), $(C_SRCS) $(O_SRCS) $(CYCCOMP))
	rm -f $(addprefix $(OUT_PREFIX), lex.cyc parse_tab.cyc parse_tab.h)

# delete generated .c and .o files with any OUT_PREFIX
clean:
	rm -f $(addprefix *, $(C_SRCS) $(O_SRCS) $(CYCCOMP))
	rm -f $(addprefix *, lex.cyc parse_tab.cyc parse_tab.h)
	rm -f *~ *.output *.stackdump 

# We do not bother to include library dependencies, but we could.
ABSYN_H     = absyn.h
PARSE_TAB_H = parse_tab.h
PARSE_H     = parse.h     $(ABSYN_H) $(PARSE_TAB_H)
ABSYNPP_H   = absynpp.h   $(ABSYN_H)
TCENV_H     = tcenv.h     $(ABSYN_H)
TCUTIL_H    = tcutil.h    $(TCENV_H)
TCSTMT_H    = tcstmt.h    $(ABSYN_H) $(TCENV_H)
TCPAT_H     = tcpat.h     $(ABSYN_H) $(TCENV_H)
EVEXP_H     = evexp.h     $(ABSYN_H)
TCEXP_H     = tcexp.h     $(ABSYN_H) $(TCENV_H) $(EXEXP_H)
TC_H        = tc.h        $(ABSYN_H) $(TCENV_H)
TOC_H       = toc.h       $(ABSYN_H)

$(OUT_PREFIX)absyn.c: $(ABSYN_H)
$(OUT_PREFIX)parse_tab.c: $(ABSYN_H)
$(OUT_PREFIX)lex.c: 
$(OUT_PREFIX)absynpp.c: $(ABSYNPP_H) $(ABSYN_H)
$(OUT_PREFIX)tcenv.c: $(TCENV_H) $(ABSYN_H) $(TCUTIL_H) $(ABSYNPP_H)
$(OUT_PREFIX)tcutil.c: $(TCUTIL_H) $(ABSYN_H) $(ABSYNPP_H) $(TCENV_H) $(EVEXP_H)
$(OUT_PREFIX)tcstmt.c: $(TCSTMT_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(TCENV_H) $(TCEXP_H) $(TCPAT_H)
$(OUT_PREFIX)tcpat.c: $(TCPAT_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(TCENV_H) $(TCEXP_H) $(TCSTMT_H)
$(OUT_PREFIX)tcexp.c: $(TCEXP_H) $(ABSYN_H) $(ABSYNPP_H) $(TCENV_H) $(TCUTIL_H) $(EVEXP_H) $(TCSTMT_H)
$(OUT_PREFIX)evexp.c: $(EVEXP_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H)
$(OUT_PREFIX)tc.c: $(TC_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(TCENV_H) $(TCEXP_H) $(TCSTMT_H) $(EVEXP_H)
$(OUT_PREFIX)toc.c: $(TOC_H) $(ABSYN_H) $(TCUTIL_H) $(EVEXP_H) $(ABSYNPP_H)
$(OUT_PREFIX)cyclone.c: $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(TCENV_H) $(TC_H) $(TOC_H)