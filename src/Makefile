
# We assume the library exists -- it's your responsibility to
# make that directory if it's not.  (The Makefile in the parent
# directory does this.)

include ../Makefile.inc

# Override this variables to use a bootstrapped compiler (you may want
# an absolute path)
CYCC:=../bin/$(CYCCOMP)

CYCLEX:=../bin/cyclex.exe
CYCBISON:=../bin/cycbison.exe

# Override this prefix to check the compiler on itself w/o clobbering files
OUT_PREFIX=

CFLAGS:=-fwritable-strings -I..
#CFLAGS:=-g -fwritable-strings -I.. 
#CFLAGS:=-pg -fwritable-strings -I.. 
#CFLAGS:=-O -fwritable-strings -I.. 

CYC_SRCS:=$(addsuffix .cyc, $(CYCLONE_SRCS))

#CYC_FLAGS:=-I ../lib -tc -toc -pp
CYC_FLAGS:=-I ../lib -tc -toc -up

LIB:=../lib/$(OUT_PREFIX)$(CYCLIB)

all: $(OUT_PREFIX)$(CYCCOMP)

$(OUT_PREFIX)$(CYCCOMP): $(addprefix $(OUT_PREFIX), $(O_SRCS)) $(LIB)
	gcc -o $@ $^ ../bin/gc.a $(LDFLAGS)
#	gcc -g -o $@ $^ ../bin/gc_pg.a -pg $(LDFLAGS)

# For machine-generated we need special targets (the .cyc is prefixed)
$(OUT_PREFIX)lex.c : $(OUT_PREFIX)lex.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $<
$(OUT_PREFIX)lex.cyc : lex.cyl $(CYCLEX)
	$(CYCLEX) $< $@

$(OUT_PREFIX)parse_tab.c: $(OUT_PREFIX)parse_tab.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $< 
$(OUT_PREFIX)parse_tab.cyc: parse.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o $@
$(OUT_PREFIX)parse_tab.h: parse.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o $(OUT_PREFIX)parse_tab.cyc

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.c: %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ $<

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
ABSYN_H        := absyn.h
PARSE_TAB_H    := parse_tab.h
PARSE_H        := parse.h        $(ABSYN_H) $(PARSE_TAB_H)
ABSYNPP_H      := absynpp.h      $(ABSYN_H)
ABSYNDUMP_H    := absyndump.h    $(ABSYN_H)
TCENV_H        := tcenv.h        $(ABSYN_H)
TCUTIL_H       := tcutil.h       $(TCENV_H)
TCSTMT_H       := tcstmt.h       $(ABSYN_H) $(TCENV_H)
TCPAT_H        := tcpat.h        $(ABSYN_H) $(TCENV_H)
FORMATSTR_H    := formatstr.h    $(ABSYN_H)
EVEXP_H        := evexp.h        $(ABSYN_H)
TCEXP_H        := tcexp.h        $(ABSYN_H) $(TCENV_H)
TC_H           := tc.h           $(ABSYN_H) $(TCENV_H)
CONTROL_FLOW_H := control_flow.h $(ABSYN_H)
TOC_H          := toc.h          $(ABSYN_H)
NEW_CONTROL_FLOW_H := new_control_flow.h

$(OUT_PREFIX)absyn.c: $(ABSYN_H)
$(OUT_PREFIX)parse_tab.c: $(ABSYN_H)
$(OUT_PREFIX)lex.c: 
$(OUT_PREFIX)absynpp.c: $(ABSYNPP_H) $(ABSYN_H)
$(OUT_PREFIX)absyndump.c: $(ABSYNDUMP_H) $(ABSYNPP_H)
$(OUT_PREFIX)tcenv.c: $(TCENV_H) $(ABSYN_H) $(TCUTIL_H) $(ABSYNPP_H)
$(OUT_PREFIX)tcutil.c: $(TCUTIL_H) $(ABSYN_H) $(ABSYNPP_H) $(TCENV_H) $(EVEXP_H)
$(OUT_PREFIX)tcstmt.c: $(TCSTMT_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(TCENV_H) $(TCEXP_H) $(TCPAT_H) $(CONTROL_FLOW_H)
$(OUT_PREFIX)tcpat.c: $(TCPAT_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(TCENV_H) $(TCEXP_H)
$(OUT_PREFIX)tcexp.c: $(TCEXP_H) $(ABSYN_H) $(ABSYNPP_H) $(TCENV_H) $(TCUTIL_H) $(EVEXP_H) $(TCSTMT_H) $(FORMATSTR_H) $(CONTROL_FLOW_H)
$(OUT_PREFIX)formatstr.c: $(FORMATSTR_H) $(ABSYN_H) $(TCUTIL_H)
$(OUT_PREFIX)evexp.c: $(EVEXP_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H)
$(OUT_PREFIX)tc.c: $(TC_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(TCENV_H) $(TCEXP_H) $(TCSTMT_H) $(EVEXP_H)
$(OUT_PREFIX)control_flow.c: $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(CONTROL_FLOW_H) $(EVEXP_H)
$(OUT_PREFIX)toc.c: $(TOC_H) $(ABSYN_H) $(TCUTIL_H) $(EVEXP_H) $(ABSYNPP_H) $(FORMATSTR_H)
$(OUT_PREFIX)cyclone.c: $(ABSYN_H) $(ABSYNPP_H) $(ABSYNDUMP_H) $(TCUTIL_H) $(TCENV_H) $(TC_H) $(CONTROL_FLOW_H) $(TOC_H) $(NEW_CONTROL_FLOW_H)

$(OUT_PREFIX)new_control_flow.c: $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(EVEXP_H)

