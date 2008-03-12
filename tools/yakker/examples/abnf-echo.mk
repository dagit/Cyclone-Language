################################
## Lookahead version
################################

abnf-echo: abnf-echo.o bnf.o pr.o cs.o $(LIB_YAKKER)
	$(CYCLONE) -o $@ $^

abnf-echo.cyc: examples/abnf-echo.bnf
	./yakker -gen rulelist $< > $@

################################
## Earley version
################################

abnf-echo-earley: abnf-flat-dfa.o bnf.o pr.o cs.o $(LIB_YAKKER)
	$(CYCLONE) -o $@ $^ -lm

abnf-flat-dfa.cyc: examples/abnf-echo.bnf yakker
	./yakker -flatten-full -cyc-namespace ABNFCycDFA -earley-gen-cyc rulelist $< > $@

aecrawl.cyc: examples/abnf-echo.bnf yakker
	./yakker -flatten-full -gen-crawl rulelist $< > $@
	echo '#define CRAWLFUN p_rulelist' >> $@
	echo '#define CYC_DFA_NS ABNFCycDFA' >> $@
	echo '#include "crawl-main.cyc"' >> $@

aecrawl.o: crawl-main.cyc

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
	cyclone -o $@ $^ -lm

abnf-dfa-eb: $(AE_DFA_OBJS)
	cyclone -o $@ $^ -lm

abnf-dfa.cyc: examples/abnf-echo.bnf yakker
	./yakker -earley-gen-cyc rulelist $< > $@

abnf-dfa.txt: examples/abnf-echo.bnf yakker
	./yakker -earley-gen-fsm rulelist examples/abnf-echo.bnf > $@
