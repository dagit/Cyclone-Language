CALC_CYC_FILES = $(EBE_CYC_FILES) calc-dfa parse-main-c
CALC_CYC_CR_FILES = $(EBE_CYC_FILES) calc-flat-dfa calccrawl-cyc

CALC_DFA_FILES = $(EBE_DFA_FILES) earley parse-main-i
CALC_DFA_CR_FILES = $(EBE_DFA_FILES) earley calccrawl-dfa

CALC_CYC_OBJS:=$(foreach yfile,$(CALC_CYC_FILES),$(yfile).o)
CALC_CYC_CR_OBJS:=$(foreach yfile,$(CALC_CYC_CR_FILES),$(yfile).o)

CALC_DFA_OBJS:=$(foreach yfile,$(CALC_DFA_FILES),$(yfile).o)
CALC_DFA_CR_OBJS:=$(foreach yfile,$(CALC_DFA_CR_FILES),$(yfile).o)


calc-cyc-eb: $(CALC_CYC_OBJS)
	cyclone -o $@ $^  -lssl -lm

calc-cyc-cr-eb: $(CALC_CYC_CR_OBJS)
	cyclone -o $@ $^  -lssl -lm

calc-dfa-eb: $(CALC_DFA_OBJS)
	cyclone -o $@ $^ -lssl -lm

calc-dfa-cr-eb: $(CALC_DFA_CR_OBJS)
	cyclone -o $@ $^ -lssl -lm

calc-dfa.cyc: examples/calc.bnf yakker
	./yakker -earley-gen-cyc start $< > $@

calc-flat-dfa.cyc: examples/calc.bnf yakker
	./yakker -flatten-full -earley-gen-cyc start $< > $@

calc-dfa.txt: examples/calc.bnf yakker
	./yakker -earley-gen-fsm start examples/calc.bnf > $@

calc-flat-dfa.txt: examples/calc.bnf yakker
	./yakker -flatten-full -earley-gen-fsm start examples/calc.bnf > $@

calccrawl-cyc.o: calccrawl.cyc
	cyclone $(CYCFLAGS) -o $@ -c -DUSE_COMPILED_DFA $<
	
calccrawl-dfa.o: calccrawl.o
	cp $< $@

calccrawl.cyc: examples/calc.bnf yakker crawl-main.cyc
	./yakker -flatten-full -gen-crawl start $< > $@
	echo '#define CRAWLFUN p_start' >> $@
	echo '#include "crawl-main.cyc"' >> $@
