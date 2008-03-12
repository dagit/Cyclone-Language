indexer_CYC_FILES = $(EBE_CYC_FILES) indexer-dfa parse-main-c
indexer_CYC_CR_FILES = $(EBE_CYC_FILES) indexer-flat-dfa indexer-crawl-cyc

indexer_DFA_FILES = $(EBE_DFA_FILES) earley parse-main-i
indexer_DFA_CR_FILES = $(EBE_DFA_FILES) earley indexer-crawl-dfa

indexer_CYC_OBJS:=$(foreach yfile,$(indexer_CYC_FILES),$(yfile).o)
indexer_CYC_CR_OBJS:=$(foreach yfile,$(indexer_CYC_CR_FILES),$(yfile).o)

indexer_DFA_OBJS:=$(foreach yfile,$(indexer_DFA_FILES),$(yfile).o)
indexer_DFA_CR_OBJS:=$(foreach yfile,$(indexer_DFA_CR_FILES),$(yfile).o)

indexer_BNF = examples/indexer.yk
# Start symbol
indexer_START = entry-list
# Function corresponding to start symbol
indexer_START_F = entry-list

indexer-cyc-eb: $(indexer_CYC_OBJS)
	cyclone -o $@ $^  -lssl -lm

indexer-cyc-cr-eb: $(indexer_CYC_CR_OBJS)
	cyclone -o $@ $^  -lssl -lm

indexer-dfa-eb: $(indexer_DFA_OBJS)
	cyclone -o $@ $^ -lssl -lm

indexer-dfa-cr-eb: $(indexer_DFA_CR_OBJS)
	cyclone -o $@ $^ -lssl -lm

indexer-dfa.cyc: $(indexer_BNF) yakker
	./yakker -earley-gen-cyc $(indexer_START) $< > $@

indexer-flat-dfa.cyc: $(indexer_BNF) yakker
	./yakker -flatten-full -earley-gen-cyc $(indexer_START) $< > $@

indexer-dfa.txt: $(indexer_BNF) yakker
	./yakker -earley-gen-fsm $(indexer_START) $(indexer_BNF) > $@

indexer-flat-dfa.txt: $(indexer_BNF) yakker
	./yakker -flatten-full -earley-gen-fsm $(indexer_START) $(indexer_BNF) > $@

indexercrawl-cyc.o: indexercrawl.cyc
	cyclone $(CYCFLAGS) -o $@ -c -DUSE_COMPILED_DFA $<

indexercrawl-dfa.o: indexercrawl.o
	cyclone $(CYCFLAGS) -o $@ -c -DUSE_FSM_DFA $<

indexercrawl.cyc: $(indexer_BNF) yakker crawl-main.cyc
	./yakker -flatten-full -gen-crawl $(indexer_START) $< > $@
	echo '#define CRAWLFUN' $(indexer_START_F) >> $@
	echo '#include "crawl-main.cyc"' >> $@
