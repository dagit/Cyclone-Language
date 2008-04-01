# args: GrammarPrefix, GrammarName, GrammarNSPrefix

GrammarPrefix = IMAP
GrammarName = imap
GrammarNSPrefix = IMAP
GrammarStart = command

################################
## Earley version
################################
$(GrammarPrefix)_FILES = $(GrammarName)-flat-dfa $(GrammarName)crawl
$(GrammarPrefix)_OBJS:=$(foreach yfile,$($(GrammarPrefix)_FILES),$(yfile).o)

$(GrammarName)-earley: $($(GrammarPrefix)_OBJS) $(LIB_YAKKER)
	$(CYCLONE) -o $@ $($(GrammarPrefix)_OBJS)  -lssl -lm $(LIB_YAKKER)

$(GrammarName)-flat-dfa.cyc: examples/$(GrammarName).bnf yakker
	./yakker -flatten-full -earley-gen-cyc $(GrammarStart) $< > $@

$(GrammarName)crawl.cyc: examples/$(GrammarName).bnf yakker crawl-main.cyc
	./yakker -flatten-full -gen-crawl $(GrammarStart) $< > $@
	echo '#define CRAWLFUN p_'$(GrammarStart) >> $@
	echo '#define CYC_DFA_NS CycDFA' >> $@
	echo '#include "crawl-main.cyc"' >> $@

$(GrammarName)crawl.o: crawl-main.cyc

#########
$(GrammarPrefix)_TG_FILES = $(GrammarName)-tg-dfa $(GrammarName)-tg-crawl parse_tab
$(GrammarPrefix)_TG_OBJS:=$(foreach yfile,$($(GrammarPrefix)_TG_FILES),$(yfile).o)

$(GrammarName)-tg-earley: $($(GrammarPrefix)_TG_OBJS) $(LIB_YAKKER)
	$(CYCLONE) -o $@ $($(GrammarPrefix)_TG_OBJS)  -lssl -lm $(LIB_YAKKER) -lcrypto

$(GrammarPrefix)_TG_RUN_FILES = $(GrammarName)-tg-dfa $(GrammarName)-grm-dfa $(GrammarName)-tg-run parse_tab
$(GrammarPrefix)_TG_RUN_OBJS:=$(foreach yfile,$($(GrammarPrefix)_TG_RUN_FILES),$(yfile).o)

$(GrammarName)-tg-run: $($(GrammarPrefix)_TG_RUN_OBJS) $(LIB_YAKKER)
	$(CYCLONE) -o $@ $($(GrammarPrefix)_TG_RUN_OBJS)  -lssl -lm $(LIB_YAKKER) -lcrypto

$(GrammarName)-tg-dfa.cyc: gen/$(GrammarName)-tg.bnf yakker
	./yakker -flatten-full -flatten-prefix TG000 -no-minus-elim -cyc-namespace $(GrammarNSPrefix)TGCycDFA -earley-gen-cyc $(GrammarStart) -no-main $< > $@

$(GrammarName)-grm-dfa.cyc: examples/$(GrammarName).bnf yakker
	./yakker -flatten-full -cyc-namespace $(GrammarNSPrefix)CycDFA -earley-gen-grm-cyc examples/$(GrammarName).bnf > $@

$(GrammarName)-tg-crawl.cyc: gen/$(GrammarName)-tg.bnf yakker crawl-main.cyc
	./yakker $(YAKFLAGS) -gen-crawl $(GrammarStart) \
                             -flatten-full -flatten-prefix TG000 \
                             -no-minus-elim \
                             -no-globals \
                             $< > $@
	echo '#define CRAWLFUN p_'$(GrammarStart) >> $@
	echo '#include "crawl-main.cyc"' >> $@

# Reflatten, b/c termgrammar creation introduces new elements 
# that need to be flattened.
$(GrammarName)-tg-run.cyc: gen/$(GrammarName)-tg.bnf yakker tge-main.cyc
	./yakker $(YAKFLAGS) -gen-crawl $(GrammarStart) \
                             -flatten-full -flatten-prefix TG000 \
                             -no-minus-elim \
                             -no-globals \
                             $< > $@
	echo '#define TGFUN p_'$(GrammarStart) >> $@
	echo '#define CYC_TG_DFA_NS '$(GrammarNSPrefix)'TGCycDFA' >> $@
	echo '#define CYC_GRM_DFA_NS '$(GrammarNSPrefix)'CycDFA' >> $@
	echo '#include "tge-main.cyc"' >> $@

gen/$(GrammarName)-tg.bnf: examples/$(GrammarName).bnf yakker
	./yakker -escape "\\%()" -flatten-full -bindgrammar -no-named-bind -termgrammar_bnf $< > $@

# State-count related targets

.PHONY: $(GrammarName)-O-sc $(GrammarName)-F-sc $(GrammarName)-NB-sc \
        $(GrammarName)-AB-sc $(GrammarName)-BB-sc 
        
$(GrammarName)-O-sc: examples/$(GrammarName).bnf yakker
	wc -l $<
	./yakker -earley-gen-cyc $(GrammarStart) $< | grep "num_dfa_states"

$(GrammarName)-F-sc: examples/$(GrammarName).bnf yakker
	./yakker -flatten-full $< > $@.bnf
	wc -l $@.bnf
	./yakker -no-minus-elim -earley-gen-cyc $(GrammarStart) $@.bnf | grep "num_dfa_states"

$(GrammarName)-NB-sc: examples/$(GrammarName).bnf yakker
	./yakker -flatten-full -bindgrammar -no-anon-bind  $< > $@.bnf
	wc -l $@.bnf
	./yakker -no-minus-elim -earley-gen-cyc $(GrammarStart) $@.bnf | grep "num_dfa_states"

$(GrammarName)-AB-sc: examples/$(GrammarName).bnf yakker
	./yakker -flatten-full -bindgrammar -no-named-bind $< > $@.bnf
	wc -l $@.bnf
	./yakker -no-minus-elim -earley-gen-cyc $(GrammarStart) $@.bnf | grep "num_dfa_states"

$(GrammarName)-BB-sc: examples/$(GrammarName).bnf yakker
	./yakker -flatten-full -bindgrammar $< > $@.bnf
	wc -l $@.bnf
	./yakker -no-minus-elim -earley-gen-cyc $(GrammarStart) $@.bnf | grep "num_dfa_states"

# Testing-related targets

gen/$(GrammarName)-flat.bnf: examples/$(GrammarName).bnf yakker
	./yakker -flatten-full $< > $@

gen/$(GrammarName)-tg-noanon.bnf: examples/$(GrammarName).bnf yakker
	./yakker -escape "\\%()" -flatten-full -bindgrammar -no-anon-bind $< > $@

gen/$(GrammarName)-tg-flat.bnf: gen/$(GrammarName)-tg.bnf yakker
	./yakker $(YAKFLAGS) -flatten-full \
                             -no-minus-elim \
                             $< > $@
gen/$(GrammarName)-tg-dfa.dot: gen/$(GrammarName)-tg.bnf yakker
	./yakker -flatten-full -no-minus-elim -earley-gen-dot $(GrammarStart) $< > $@

gen/$(GrammarName)-flat-grm-dfa.txt: examples/$(GrammarName).bnf yakker
	./yakker -flatten-full -earley-gen-grm-fsm $< > $@

gen/$(GrammarName)-flat-grm-dfa.dot: examples/$(GrammarName).bnf yakker
	./yakker -flatten-full -earley-gen-grm-dot $< > $@

$(GrammarName)-flat-dfa.txt: examples/$(GrammarName).bnf yakker
	./yakker -flatten-full -earley-gen-fsm $(GrammarStart) $< > $@

