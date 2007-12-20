################################
## Lookahead version
################################

# Targets for abnf-echo lib.

abnf-lookahead.cyc: examples/abnf-echo.bnf
	./yakker -no-main -gen rulelist $< > $@

# Targets for stand-alone abnf-echo program.

abnf-echo.cyc: examples/abnf-echo.bnf
	./yakker -gen rulelist $< > $@

################################
## Earley version
################################

AE_CR_FILES = abnf-flat-dfa aecrawl
AE_CR_OBJS:=$(foreach yfile,$(AE_CR_FILES),$(yfile).o)

abnf-flat-dfa.cyc: examples/abnf-echo.bnf yakker
	./yakker -flatten-full -earley-gen-cyc rulelist $< > $@

# Targets for abnf-echo lib.

abnf-earley.cyc: examples/abnf-echo.bnf yakker
	./yakker -flatten-full -gen-crawl rulelist $< > $@

# Targets for stand-alone abnf-echo program.

aecrawl.cyc: examples/abnf-echo.bnf yakker crawl-main.cyc
	./yakker -flatten-full -gen-crawl rulelist $< > $@
	echo '#define CRAWLFUN p_rulelist' >> $@
	echo '#include "crawl-main.cyc"' >> $@


#######################################
# Earley debugging targets
######################################
AE_CYC_FILES = $(EBE_CYC_FILES) abnf-dfa parse-main
AE_DFA_FILES = $(EBE_DFA_FILES) earley parse-main
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

	