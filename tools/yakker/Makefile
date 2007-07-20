include ../../Makefile.inc
CYCFLAGS=
#CYCFLAGS=-g -save-c -pp
YAKFLAGS=

CURDIR=$(shell pwd)

PROG=yakker$(EXE)

# Set this to yakker to make all targets generated with yakker depend
# on yakker.  Useful to turn off when simultenously modifying yakker
# and working on a program based on generated code.
PROG_DEP=

BINDIR=$(CYCDIR)/bin
# examples directory. Use full path to avoid ambiguity.
EXDIR=$(CURDIR)/examples
CYC_LIB_PATH = $(BINDIR)/lib
CYCLONE=$(BINDIR)/cyclone -I../../include -B$(BINDIR)/lib/cyc-lib
CYCBISON=$(BINDIR)/cycbison
CYCLEX=$(BINDIR)/cyclex
STRINGIFY=$(BINDIR)/stringify

YAKKER_FILES = analyze axarray bnf cs extract fa nfa_lazy nfa_eager flatten termgrammar lex pads \
  parse_tab parsergen pr prose prose_elim_grm string_dfa_engine1 \
  string_dfa_engine2_1 corerules corerules_grm \
  util namespaces yakker

FILES=$(YAKKER_FILES) ast_main pm_bnf imap_scanf funtable

OBJS:=$(addsuffix .o,$(YAKKER_FILES))

# OBJS compiled for allocation profiling.
OBJS_PA=$(addsuffix _a.o,$(YAKKER_FILES))
PROG_PA=$(PROG)_a

###############################################################################
# Targets
###############################################################################

all: $(PROG)

cyclone-install: $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $< $@

$(PROG): $(OBJS)
	$(CYCLONE) $(CYCFLAGS) -L$(CYC_LIB_PATH) $^ -o $@ -lm

$(PROG_PA): $(OBJS_PA)
	$(CYCLONE) $(CYCFLAGS) -pa -L$(CYC_LIB_PATH) $^ -o $@ -lm

testindexer: indexer 1rfc_index
	./indexer < 1rfc_index

indexer: indexer.o ykbuf.o ssl.o bnf.o extract.o pr.o parse_tab.o util.o lex.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

# --novcgen is temporary until a bug is fixed
indexer.o: indexer.cyc
	$(CYCLONE) -c $< -o $@ --novcgen

indexer.cyc: examples/indexer.yk $(PROG_DEP)
	./yakker $(YAKFLAGS) -gen entry-list $< > $@

wf: wf.o ykbuf.o ssl.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

wf.cyc: examples/wf.yk $(PROG_DEP)
	./yakker $(YAKFLAGS) -gen file $< > $@

imapcl: imapcl.o ykbuf.o ssl.o imap.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

imapcl.o: examples/imapcl.cyc
	$(CYCLONE) -c $^ -o $@ -I.

# --novcgen is temporary until a bug is fixed
imap.o: imap.cyc
	$(CYCLONE) -c $< -o $@ --novcgen

imapp.key:
	openssl genrsa > $@

imapp.cert: imapp.key
	openssl req -new -key $< -x509 -subj /C=aa/ST=b/L=c/O=d/OU=e/CN=shoebox.research.att.com > $@

imapp: imapp.o ykbuf.o ssl.o imap.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

imapp.o: examples/imapp.cyc
	$(CYCLONE) -c $^ -o $@ -I.

#	$(CYCLONE) $^ -o $@ /Users/yitzhakm/projects/cyclone/otherlibs/sqlite3/libsqlite3.a -lpthread -lssl -lcrypto
imapserver: imapserver.o imap_scanf.o pm_bnf.o bnf.o lex.o parse_tab.o util.o funtable.o imap_genpm.o ykbuf.o ssl.o imap.o
	$(CYCLONE) $^ -o $@ /Users/yitzhakm/projects/cyclone/otherlibs/sqlite3/libsqlite3.a -lpthread -lssl -lcrypto

imapserver.o: $(EXDIR)/imapserver.cyc
	$(CYCLONE) -c $^ -o $@ -I.

# Targets for testing pattern match parsers-generators.  Generates
# BNF that can parse a pattern-match and produce a new BNF specialized
# to that pattern-match. Note that the new BNF is intended to augment
# the original.

# small_imap_genpm: small_imap_genpm.cyc ykbuf.o ssl.o tg_stack.o tgmain.o util.o
# 	$(CYCLONE) $^ -o $@ --novcgen -lssl -lcrypto

small_imap.o: small_imap.cyc
	$(CYCLONE) $(CYCFLAGS) -c $< -o $@ --novcgen

small_imap.cyc: examples/small_imap.bnf $(PROG_DEP)
	./yakker $(YAKFLAGS) -gen command -no-main -no-globals $< > $@

small_imap.h: examples/small_imap.bnf $(PROG_DEP)
	./yakker $(YAKFLAGS) -gen-header $< > $@

small_imap_genpm: small_imap_genpm.o ykbuf.o ssl.o ast_main.o funtable.o pm_bnf.o small_imap.o\
	bnf.o lex.o parse_tab.o pr.o util.o 
	$(CYCLONE) $(CYCFLAGS) $^ -o $@ --novcgen -lssl -lcrypto

#	./yakker $(YAKFLAGS) -all-start -eof 10 -gen command_pm -no-main $< > $@
# Use -no-minus-elim as minus elimination was already done, and redoing it causes problems (in particular, 
# the charset optimization loses necessary semantic actions).
small_imap_genpm.cyc: small_imap_genpm.bnf $(PROG_DEP)
	./yakker $(YAKFLAGS) -cyc-namespace SmallImapFormat -all-start -no-minus-elim -gen command -no-main $< > $@

small_imap_genpm.h: small_imap_genpm.bnf $(PROG_DEP)
	./yakker $(YAKFLAGS) -cyc-namespace SmallImapFormat -gen-header $< > $@

small_imap_genpm.bnf: examples/small_imap.bnf $(PROG_DEP)
	./yakker -escape "\\%()" -flatten -bindgrammar -no-anon-bind -termgrammar_bnf $< > $@

###############################################################################
# Rules for full imap grammar with pattern-matching support.
###############################################################################
# imap_genpm.cyc: imap_genpm.bnf $(PROG_DEP)_a
# 	./yakker_a $(YAKFLAGS) -all-start -no-minus-elim -gen command -no-main $< > $@

# Use -no-minus-elim as minus elimination was already done, and redoing it causes problems (in particular, 
# the charset optimization loses necessary semantic actions).
imap_genpm.cyc: imap_genpm.bnf $(PROG_DEP)
	./yakker $(YAKFLAGS) -gen command \
                             -cyc-namespace ImapFormat \
                             -all-start \
                             -no-minus-elim \
                             -no-main \
                             -no-globals \
                             $< > $@

imap_genpm.h: imap_genpm.bnf $(PROG_DEP)
	./yakker $(YAKFLAGS) -cyc-namespace ImapFormat -gen-header $< > $@

imap_genpm.bnf: imap.bnf $(PROG_DEP)
	./yakker -escape "\\%()" -flatten -bindgrammar -no-anon-bind -termgrammar_bnf $< > $@

imap.cyc: imap.bnf $(PROG_DEP)
	./yakker $(YAKFLAGS) -gen command -no-main -lazyfill $< > $@

imap.h: imap.bnf $(PROG_DEP)
	./yakker $(YAKFLAGS) -gen-header $< > $@

# imap_genpm.bnf: imap_flat.bnf $(PROG_DEP)
# 	./yakker -no-minus-elim -bindgrammar -no-anon-bind -termgrammar_bnf $< > $@

# imap_bind.bnf: imap_flat.bnf $(PROG_DEP)
# 	./yakker -no-minus-elim -bindgrammar -no-anon-bind $< > $@

# imap_esc.bnf: imap.bnf $(PROG_DEP)
# 	./yakker -escape "\\%()" $< > $@

# imap_none.bnf: imap.bnf $(PROG_DEP)
# 	./yakker $< > $@

# imap_flat.bnf: imap.bnf $(PROG_DEP)
# 	./yakker -escape "\\%()" -flatten $< > $@

###############################################################################
# Pattern matching BNF examples.
###############################################################################

small_imap_pm: small_imap_pm.o ykbuf.o ssl.o tg_stack.o util.o small_imap.o
	$(CYCLONE) $(CYCFLAGS) $^ -o $@ --novcgen -lssl -lcrypto

small_imap_pm.o: small_imap_pm.cyc small_imap.h

small_imap_pm.cyc: small_imap_pm.bnf $(PROG_DEP)
	./yakker -no-close-defs -gen my-command $< > $@

small_imap_pm.bnf: examples/test_genparse small_imap_genpm 
	./small_imap_genpm -unescape-pat "\\" -scan -name my-command < $< > $@

small_imap_print.cyc: examples/test_genprint.txt small_imap_genpm 
	./small_imap_genpm -unescape-pat "\\" -print -name my-command -args 7 < $< > $@

%_pm: %_pm.cyc ykbuf.o ssl.o tg_stack.o pm_main.o util.o
	$(CYCLONE) $^ -o $@ --novcgen -lssl -lcrypto

%_pm.cyc: %_pm.bnf $(PROG_DEP)
	./yakker $(YAKFLAGS) -gen command-pm -no-main $< > $@

tg_stack.o : tg_stack.cyc util.h
	$(CYCLONE) -c $< -o $@ -I.

tgmain.o : tgmain.cyc tg_stack.h
	$(CYCLONE) -c $< -o $@ -I.

ast_main.o : ast_main.cyc gen_ast_main.cyc pm_bnf.h small_imap_genpm.h
	$(CYCLONE) -c $< -o $@ -I.

pm_bnf.o : pm_bnf.cyc pm_bnf.h pads.h bnf.h
	$(CYCLONE) -c $< -o $@ -I.
###############################################################################

markdown: markdown.o ykbuf.o ssl.o bnf.o extract.o pr.o parse_tab.o util.o lex.o axarray.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

markdown.cyc: examples/markdown.yk $(PROG_DEP)
	./yakker $(YAKFLAGS) -no-main -gen elim-cr-line -d $< > $@

boxextract: boxextract.o ykbuf.o ssl.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

boxextract.cyc: boxextract.yk $(PROG_DEP)
	./yakker $(YAKFLAGS) -gen lines $< > $@

string_%.cyc: %.cyc
	$(STRINGIFY) $< > $@

string_%.cyc: %.str
	$(STRINGIFY) $< > $@

prose_elim_grm.cyc: prose_elim_grm.bnf
	$(STRINGIFY) $< > $@

corerules_grm.cyc: corerules_grm.bnf
	$(STRINGIFY) $< > $@

###############################################################################
# Automatic dependency generation
###############################################################################

%.d: %.cyc
	$(CYCLONE) -M -MG $< > $@

.PHONY: depend

depend: $(addsuffix .d, $(FILES))

###############################################################################

.SECONDARY: parse_tab.cyc

uri.p: $(PROG_DEP) uri.bnf
	$(RM) $@.tmp
	./yakker -pads uri.bnf > $@.tmp
	cat $@.tmp | ./fix_hostname.pl | ./add_precord.pl URI_reference_t | cat > $@
	$(RM) $@.tmp

uri_one_string.p: $(PROG_DEP) uri_orig.bnf
	$(RM) $@.tmp
	./yakker -pads -re uri_orig.bnf > $@.tmp
	cat $@.tmp | ./fix_hostname.pl | ./add_precord.pl URI_reference_t | cat > $@
	$(RM) $@.tmp

http.bnf: $(PROG_DEP) rfc2616.bnf rfc2617.bnf rfc2396.bnf rfc2822.bnf rfc3501.bnf
	./yakker rfc2616.bnf \
	  -externals rfc2617.bnf rfc2396.bnf rfc2822.bnf rfc3501.bnf \
	   >$@

message-header.bnf: http.bnf $(PROG_DEP)
	./yakker $< -subset message-header > $@

imap.bnf: rfc3501.bnf $(PROG_DEP)
	./yakker rfc3501.bnf -tsort -unnamespace > $@
	echo 'my-greeting = ("" greeting)$$x {greeting_hook(x);}.' >> $@
	echo 'my-response = ("" response)$$x {response_hook(x);}.' >> $@
	echo 'my-command = ("" command)$$x {command_hook(x);}.' >> $@
	echo '{ extern void greeting_hook(const char ?); }' >> $@
	echo '{ extern void response_hook(const char ?); }' >> $@
	echo '{ extern void command_hook(const char ?); }' >> $@

vcard.bnf: rfc2426.bnf rfc2425.bnf rfc1738.bnf rfc1766.bnf rfc3501.bnf $(PROG_DEP)
# 1766: for Language-Tag
# 1738: for genericurl
# 3501: for base64
	./yakker rfc2426.bnf -externals rfc2425.bnf rfc1738.bnf rfc1766.bnf rfc3501.bnf -tsort -unnamespace > $@

clean:
	$(RM) *.d *.o parse_tab.cyc parse_tab.h lex.cyc $(PROG)
	$(RM) in out errs parse.output
	$(RM) http.bnf http.p message-header.bnf message-header.p
	$(RM) imap.bnf imap.cyc rfc3501.bnf rfc2234.bnf rfc3501.txt rfc2234.txt
	$(RM) string_dfa_engine1.*
	$(RM) prose_elim_grammar.cyc
	$(RM) prose_elim_grm.cyc corerules_grm.cyc
	$(RM) indexer indexer.cyc 1rfc_index 1rfc_index.txt
	$(RM) wf wf.cyc
	$(RM) http.cyc
	$(RM) imapp imapcl imapp.key imapp.cert
	$(RM) markdown.cyc markdown

%.p: %.bnf $(PROG_DEP)
	./yakker -pads $< > $@

# .o dependency forces the _a.o to depend on the same files as the .o.
# However, the .o is not really needed.
$(OBJS_PA): %_a.o: %.cyc %.o 
	$(CYCLONE) $(CYCFLAGS) -pa -c $< -o $@

%.o: %.cyc
	$(CYCLONE) $(CYCFLAGS) -c $<

%_tab.cyc: %.y
	$(CYCBISON) -d $<
#	$(CYCBISON) -d --debug --verbose $<

%_tab.h: %.y
	$(CYCBISON) -d $<
#	$(CYCBISON) -d --debug --verbose $<

%.cyc: %.cyl
	$(CYCLEX) $< $@

rfc%.txt:
	curl -O http://www.ietf.org/rfc/$@

# Grab index from ietf
1rfc_index.txt:
	curl -O http://www.ietf.org/iesg/$@

# Strip off beginning of index, start at entry 0001 instead
# Add extra line feed, indexer assumes each entry is followed by two.
1rfc_index: 1rfc_index.txt
	sed -n -e '/0001/,$$ p' $< > $@
	echo >> $@

# Make sure the rfc%.txt files are not automatically deleted by make
.PRECIOUS: rfc%.txt

# literal needs to know about a prefixed length
# capability-data causes an unusual conflict, avoided for now
#	echo 'literal = "{" (number "")$$x "}" CRLF @repeat(atoi(x))CHAR8.' >> $@.tmp
rfc3501.bnf: rfc3501.txt $(PROG_DEP)
	./yakker -extract $< -omit literal capability-data > $@.tmp
	echo 'literal = "{" (number$$x ("}" CRLF @repeat(atoi(x))CHAR8)).' >> $@.tmp
	echo 'capability-data = "CAPABILITY" *(SP capability).' >> $@.tmp
	./yakker $@.tmp -namespace rfc3501 > $@
	$(RM) $@.tmp

# A1, A2, and passwd are extracted by mistake
# Request-URI is the proper spelling (defined in rfc2616)
# challenge is defined 3 times, the first is correct, we fix by hand
# credentials is defined more than once too
rfc2617.bnf: rfc2617.txt $(PROG_DEP)
	./yakker -extract $< -omit A1 A2 passwd challenge credentials \
          -rename request-uri Request-URI >$@
	echo "challenge = auth-scheme 1*SP 1#auth-param." >> $@
	echo "credentials = auth-scheme #auth-param." >> $@

# freshness_lifetime is extracted by mistake
# entity-body gets multiple definitions, we correct by hand
rfc2616.bnf: rfc2616.txt $(PROG_DEP)
	./yakker -extract $< -omit freshness_lifetime \
          -omit entity-body >$@
	echo "entity-body = *OCTET." >> $@

# CRLF has multiple (syntactically different) defs, we correct by hand
# rulename has a bunch of examples and the correct def
# Due to an extraneous newline in a comment the defn. of CHAR is screwed
#   Same for char-val and prose-val
# A bunch of things that look like definitions in the file but aren't:
#   b, d, x command, mumble, bar, foo, ruleset
rfc2234.bnf: rfc2234.txt $(PROG_DEP)
	./yakker -extract $< -omit CRLF rulename CHAR char-val prose-val b d x command mumble bar foo ruleset name >$@
	echo 'CRLF = CR LF.' >> $@
	echo 'rulename = ALPHA *(ALPHA / DIGIT / "-").' >> $@
	echo 'CHAR = %x01-7F.' >> $@
	echo 'char-val = DQUOTE *(%x20-21 / %x23-7E) DQUOTE.' >> $@
	echo 'prose-val = "<" *(%x20-3D / %x3F-7E) ">".' >> $@

rfc2822.bnf: rfc2822.txt $(PROG_DEP)
	./yakker -extract $< >$@
	echo 'dtext = NO-WS-CTL / %d33-90 / %d94-126.' >> $@
	echo 'qtext = NO-WS-CTL / %d33 / %d35-91 / %d93-126.' >> $@
	echo 'ctext = NO-WS-CTL / %d33-39 / %d42-91 / %d93-126.' >> $@
	echo 'item-value = 1*angle-addr / addr-spec / atom / domain / msg-id.' >> $@
	echo 'obs-zone = "UT" / "GMT" / "EST" / "EDT" / "CST" / "CDT" / "MST" / "MDT" / "PST" / "PDT" / %d65-73 / %d75-90 / %d97-105 / %d107-122.' >> $@

rfc1738.bnf: rfc1738.txt $(PROG_DEP)
	./yakker -extract $< -namespace rfc1738 > $@

rfc1766.bnf: rfc1766.txt $(PROG_DEP)
	./yakker -extract $< -namespace rfc1766 >$@

rfc2425.bnf: rfc2425.txt $(PROG_DEP)
	# time-numzome should be time-numzone
	# iana-token gets extracted twice, one defn is wrong
	# genericurl is defined in rfc1738
	# date-time is simply missing, note according to the text it will
	#  be different than the one in rfc3501
	#  We know date-time should be there because it is referred to
	#  by rfc2426, plus examples are given in the text of rfc2425
	./yakker -extract $< -omit time-numzome iana-token -rename genericurl rfc1738:genericurl > $@.tmp
	echo 'iana-token = 1*(ALPHA / DIGIT / "-").' >> $@.tmp
	echo 'time-numzone = sign time-hour [":"] time-minute.' >> $@.tmp
	echo 'date-time = date "T" time.' >> $@.tmp
	./yakker -namespace rfc2425 $@.tmp > $@
	$(RM) $@.tmp


rfc2426.bnf: rfc2426.txt $(PROG_DEP)
	# value and param have multiple definitions
	# the rename corrects a typo (but it is in a value which we omit...)
	./yakker -extract $< -rename snd-line-value snd-inline-value -omit value param > $@
	# these are the right definitions of value and param
	echo 'value = *VALUE-CHAR.' >> $@
	echo 'param = param-name "=" param-value *("," param-value).' >> $@
	# typos prevent extraction of these
	echo 'ESCAPED-CHAR = "\\" / "\;" / "\," / ("\" %d110) / ("\" %d78).' >> $@
	echo 'date-time-value = rfc2425:date-time.' >> $@
	echo 'snd-inline-param = ("VALUE" "=" "binary") / ("ENCODING" "=" "b") / ("TYPE" "=" *SAFE-CHAR).' >> $@
	# fill in some missing definitions
	echo 'uri = rfc1738:genericurl.' >> $@
	echo 'word = iana-token.' >> $@
	echo 'iana-type = iana-token.' >> $@

rfc%.bnf: rfc%.txt $(PROG_DEP)
	./yakker -extract $< >$@

.PHONY: cyclone-install

ifneq ($(MAKECMDGOALS),clean)
  DEPENDS:=$(addsuffix .d, $(FILES))
  -include $(DEPENDS)
endif
