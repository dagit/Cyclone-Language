################################
## Lookahead version
################################

abnf-echo.cyc: abnf-echo.bnf
	./yakker -gen rulelist $< > $@

################################
## Earley version
################################

AE_CYC_FILES = $(EBE_CYC_FILES) abnf-dfa parse-main-c
AE_CYC_CR_FILES = $(EBE_CYC_FILES) abnf-flat-dfa aecrawl-cyc

AE_DFA_FILES = $(EBE_DFA_FILES) earley parse-main-i
AE_DFA_CR_FILES = $(EBE_DFA_FILES) earley aecrawl-dfa

AE_CYC_OBJS:=$(foreach yfile,$(AE_CYC_FILES),$(yfile).o)
AE_CYC_CR_OBJS:=$(foreach yfile,$(AE_CYC_CR_FILES),$(yfile).o)

AE_DFA_OBJS:=$(foreach yfile,$(AE_DFA_FILES),$(yfile).o)
AE_DFA_CR_OBJS:=$(foreach yfile,$(AE_DFA_CR_FILES),$(yfile).o)

earley-backend-c.o: earley-backend.cyc
	cyclone $(CYCFLAGS) -o $@ -c -DUSE_COMPILED_DFA $< 

earley-backend-i.o: earley-backend.o
	cp $< $@

parse-main-c.o: parse-main.cyc
	cyclone $(CYCFLAGS) -o $@ -c -DUSE_COMPILED_DFA $< 

parse-main-i.o: parse-main.o
	cp $< $@

abnf-cyc-eb: $(AE_CYC_OBJS)
	cyclone -o $@ $^  -lssl -lm

abnf-dfa-eb: $(AE_DFA_OBJS)
	cyclone -o $@ $^ -lssl -lm

abnf-dfa.cyc: abnf-echo.bnf yakker
	./yakker -earley-gen-cyc rulelist $< > $@

abnf-flat-dfa.cyc: abnf-echo.bnf yakker
	./yakker -flatten-full -earley-gen-cyc rulelist $< > $@

abnf-dfa.txt: abnf-echo.bnf yakker
	./yakker -earley-gen-fsm rulelist abnf-echo.bnf > $@

abnf-flat-dfa.dot: abnf-echo.bnf yakker
	./yakker -flatten-full -earley-gen-dot rulelist $< > $@

testing/abnf-trunc-flat.bnf: testing/abnf-trunc.bnf yakker
	./yakker -flatten-full $< > $@

testing/abnf-flat.bnf: abnf-echo.bnf yakker
	./yakker -flatten-full $< > $@

#abnf-trunc-flat-dfa.txt: testing/abnf-trunc.bnf yakker
#	./yakker -flatten-full -earley-gen-fsm rulelist $< > $@

#abnf-trunc-flat-dfa.dot: testing/abnf-trunc.bnf yakker
#	./yakker -flatten-full -earley-gen-dot rulelist $< > $@

aecrawl-cyc.o: aecrawl.cyc
	cyclone $(CYCFLAGS) -o $@ -c -DUSE_COMPILED_DFA $<
	
aecrawl-dfa.o: aecrawl.o
	cp $< $@

aecrawl.cyc: abnf-echo.bnf yakker crawl-main.cyc
	./yakker -flatten-full -gen-crawl rulelist $< > $@
	echo '#define CRAWLFUN p_rulelist' >> $@
	echo '#include "crawl-main.cyc"' >> $@
