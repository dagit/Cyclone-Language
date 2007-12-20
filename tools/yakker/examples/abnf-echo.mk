################################
## Lookahead version
################################

abnf-echo.cyc: examples/abnf-echo.bnf
	./yakker -gen rulelist $< > $@

abnf-lookahead.cyc: examples/abnf-echo.bnf
	./yakker -no-main -gen rulelist $< > $@

################################
## Earley version
################################

AE_CYC_CR_FILES = abnf-flat-dfa aecrawl-cyc
AE_DFA_CR_FILES = $(EBE_DFA_FILES) earley aecrawl-dfa
AE_CYC_CR_OBJS:=$(foreach yfile,$(AE_CYC_CR_FILES),$(yfile).o)
AE_DFA_CR_OBJS:=$(foreach yfile,$(AE_DFA_CR_FILES),$(yfile).o)

abnf-flat-dfa.cyc: examples/abnf-echo.bnf yakker
	./yakker -flatten-full -earley-gen-cyc rulelist $< > $@

abnf-earley.cyc: examples/abnf-echo.bnf yakker
	./yakker -flatten-full -gen-crawl rulelist $< > $@


# Targets for stand-alone abnf-echo program.

aecrawl-cyc.o: aecrawl.cyc
	cyclone $(CYCFLAGS) -o $@ -c -DUSE_COMPILED_DFA $<
	
aecrawl-dfa.o: aecrawl.cyc
	cyclone $(CYCFLAGS) -o $@ -c -DUSE_FSM_DFA $<

aecrawl.cyc: examples/abnf-echo.bnf yakker crawl-main.cyc
	./yakker -flatten-full -gen-crawl rulelist $< > $@
	echo '#define CRAWLFUN p_rulelist' >> $@
	echo '#include "crawl-main.cyc"' >> $@


#######################################
# Earley debugging targets
######################################
AE_CYC_FILES = $(EBE_CYC_FILES) abnf-dfa parse-main-c
AE_DFA_FILES = $(EBE_DFA_FILES) earley parse-main-i
AE_CYC_OBJS:=$(foreach yfile,$(AE_CYC_FILES),$(yfile).o)
AE_DFA_OBJS:=$(foreach yfile,$(AE_DFA_FILES),$(yfile).o)

abnf-flat-dfa.dot: examples/abnf-echo.bnf yakker
	./yakker -flatten-full -earley-gen-dot rulelist $< > $@

abnf-cyc-eb: $(AE_CYC_OBJS)
	cyclone -o $@ $^  -lssl -lm

abnf-dfa-eb: $(AE_DFA_OBJS)
	cyclone -o $@ $^ -lssl -lm

abnf-dfa.cyc: examples/abnf-echo.bnf yakker
	./yakker -earley-gen-cyc rulelist $< > $@

abnf-dfa.txt: examples/abnf-echo.bnf yakker
	./yakker -earley-gen-fsm rulelist examples/abnf-echo.bnf > $@

#testing/abnf-trunc-flat.bnf: testing/abnf-trunc.bnf yakker
#	./yakker -flatten-full $< > $@

#testing/abnf-flat.bnf: examples/abnf-echo.bnf yakker
#	./yakker -flatten-full $< > $@

#abnf-trunc-flat-dfa.txt: testing/abnf-trunc.bnf yakker
#	./yakker -flatten-full -earley-gen-fsm rulelist $< > $@

#abnf-trunc-flat-dfa.dot: testing/abnf-trunc.bnf yakker
#	./yakker -flatten-full -earley-gen-dot rulelist $< > $@

	