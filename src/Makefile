
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

ifdef PROFILE_REGIONS
PROF_RGN_FLAG := -DCYC_REGION_PROFILE
else
PROF_RGN_FLAG := 
endif

ifdef MS
TOC_FLAG = -tovc
else
TOC_FLAG =
endif

CYC_FLAGS := -I../lib -save-c $(TOC_FLAG) $(PROF_RGN_FLAG)

LIBR:=../lib/$(OUT_PREFIX)$(CYCLIB)

CYC_SRCS := $(addsuffix .cyc, $(CYCLONE_SRCS))

all: $(OUT_PREFIX)$(CYCCOMP)

# Note: although cyclone now automatically adds CYCLIB to the command
# line when calling gcc, we leave in an explicit $(LIBR) below so that
# we get the $(OUT_PREFIX) version instead of ../lib/cyclib.a
$(OUT_PREFIX)$(CYCCOMP): $(addprefix $(OUT_PREFIX), $(O_SRCS)) $(LIBR)
ifdef MS
	cl /Fe$@ ../lib/snprintf.obj $^ ../bin/cyc-lib/gc.lib
else
	$(CYCC) -B../lib -o $@ $^ $(LDFLAGS)
#	gcc -g -o $@ $^ ../bin/cyc-lib/gc.a $(LDFLAGS)
#	gcc -o $@ $^ ../bin/cyc-lib/gc_pg.a -pg $(LDFLAGS)
endif
# For machine-generated we need special targets (the .cyc is prefixed)
$(OUT_PREFIX)lex.o : $(OUT_PREFIX)lex.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -c $<
$(OUT_PREFIX)lex.cyc : lex.cyl $(CYCLEX)
	$(CYCLEX) $< $@

$(OUT_PREFIX)parse_tab.o: $(OUT_PREFIX)parse_tab.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -c $< 
$(OUT_PREFIX)parse_tab.cyc: parse.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o $@
$(OUT_PREFIX)parse_tab.h: parse.y $(CYCBISON)
	$(CYCBISON) -v -d $< -o $(OUT_PREFIX)parse_tab.cyc

# This takes care of non-machine-generated versions (the .cyc is already there)
$(OUT_PREFIX)%.o: %.cyc $(CYCC)
	$(CYCC) $(CYC_FLAGS) -o $@ -c $<

ifdef MS
%.obj: %.c
	cl /I.. /Dinline=__inline /c /w /nologo /Fo$@ $^
endif

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
TOC_H          := toc.h          $(ABSYN_H)
TOVC_H         := tovc.h         $(ABSYN_H)
CF_FLOWINFO_H  := cf_flowinfo.h  $(ABSYN_H)
CF_ABSEXP_H    := cf_absexp.h $(NEW_CONTROL_FLOW_H)
CONTROL_FLOW_H := new_control_flow.h $(ABSYN_H) $(CF_FLOWINFO_H)
INTERFACE_H    := interface.h $(ABSYN_H) $(TCENV_H)
TCDECL_H       := tcdecl.h $(ABSYN_H)

$(OUT_PREFIX)absyn.o: $(ABSYN_H)
$(OUT_PREFIX)parse_tab.o: $(ABSYN_H)
$(OUT_PREFIX)lex.o: $(PARSE_H)
$(OUT_PREFIX)absynpp.o: $(ABSYNPP_H) $(ABSYN_H)
$(OUT_PREFIX)absyndump.o: $(ABSYNDUMP_H) $(ABSYNPP_H)
$(OUT_PREFIX)tcenv.o: $(TCENV_H) $(ABSYN_H) $(TCUTIL_H) $(ABSYNPP_H)
$(OUT_PREFIX)tcutil.o: $(TCUTIL_H) $(ABSYN_H) $(ABSYNPP_H) $(TCENV_H) $(EVEXP_H)
$(OUT_PREFIX)tcstmt.o: $(TCSTMT_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(TCENV_H) $(TCEXP_H) $(TCPAT_H) $(CONTROL_FLOW_H)
$(OUT_PREFIX)tcpat.o: $(TCPAT_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(TCENV_H) $(TCEXP_H)
$(OUT_PREFIX)tcexp.o: $(TCEXP_H) $(ABSYN_H) $(ABSYNPP_H) $(TCENV_H) $(TCUTIL_H) $(EVEXP_H) $(TCSTMT_H) $(FORMATSTR_H) $(CONTROL_FLOW_H)
$(OUT_PREFIX)formatstr.o: $(FORMATSTR_H) $(ABSYN_H) $(TCUTIL_H)
$(OUT_PREFIX)evexp.o: $(EVEXP_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H)
$(OUT_PREFIX)tc.o: $(TC_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(TCENV_H) $(TCEXP_H) $(TCSTMT_H) $(EVEXP_H)
$(OUT_PREFIX)toc.o: $(TOC_H) $(ABSYN_H) $(TCUTIL_H) $(EVEXP_H) $(ABSYNPP_H) $(FORMATSTR_H)
$(OUT_PREFIX)tovc.o: $(TOVC_H) $(ABSYN_H) $(TCUTIL_H) $(TOC_H)
$(OUT_PREFIX)cyclone.o: $(ABSYN_H) $(ABSYNPP_H) $(ABSYNDUMP_H) $(TCUTIL_H) $(TCENV_H) $(TC_H) $(CONTROL_FLOW_H) $(TOC_H) $(INTERFACE_H)

$(OUT_PREFIX)cf_flowinfo.o: $(CF_FLOWINFO_H)
$(OUT_PREFIX)cf_absexp.o: $(CF_ABSEXP_H) $(CONTROL_FLOW_H) $(TCUTIL_H)
$(OUT_PREFIX)new_control_flow.o: $(CONTROL_FLOW_H) $(ABSYN_H) $(ABSYNPP_H) $(TCUTIL_H) $(EVEXP_H)

$(OUT_PREFIX)interface.o: $(INTERFACE_H) $(ABSYNPP_H) $(TC_H) $(PARSE_H) $(TCUTIL_H) $(TCDECL_H)
$(OUT_PREFIX)tcdecl.o: $(TCDECL_H) $(ABSYNPP_H) $(TCUTIL_H)
