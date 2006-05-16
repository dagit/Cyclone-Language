include ../../Makefile.inc
CYCFLAGS=
#CYCFLAGS=-pg

PROG=yakker$(EXE)
BINDIR=$(CYCDIR)/bin
CYC_LIB_PATH = $(BINDIR)/lib
CYCLONE=$(BINDIR)/cyclone -I../../include -B$(BINDIR)/lib/cyc-lib
CYCBISON=$(BINDIR)/cycbison
CYCLEX=$(BINDIR)/cyclex
STRINGIFY=$(BINDIR)/stringify

FILES=analyze axarray bnf ykbuf corerules cs extract fa termgrammar lex pads parse_tab parsergen pr prose util yakker

all: $(PROG)

cyclone-install: $(BINDIR)/$(PROG)

$(BINDIR)/$(PROG): $(PROG)
	cp $< $@

$(PROG): analyze.o axarray.o bnf.o cs.o extract.o fa.o termgrammar.o lex.o pads.o \
  parse_tab.o parsergen.o pr.o prose.o prose_elim_grm.o string_dfa_engine1.o \
  string_dfa_engine2_1.o corerules.o corerules_grm.o \
  util.o namespaces.o yakker.o
	$(CYCLONE) $(CYCFLAGS) -L$(CYC_LIB_PATH) $^ -o $@

testindexer: indexer 1rfc_index
	./indexer < 1rfc_index

indexer: indexer.o ykbuf.o ssl.o bnf.o extract.o pr.o parse_tab.o util.o lex.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

# --novcgen is temporary until a bug is fixed
indexer.o: indexer.cyc
	$(CYCLONE) -c $< -o $@ --novcgen

indexer.cyc: examples/indexer.yk yakker
	./yakker -gen entry-list $< > $@

wf: wf.o ykbuf.o ssl.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

wf.cyc: examples/wf.yk yakker
	./yakker -gen file $< > $@

imapcl: imapcl.o ykbuf.o ssl.o imap.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

imapcl.o: examples/imapcl.cyc
	$(CYCLONE) -c $^ -o $@ -I.

# --novcgen is temporary until a bug is fixed
imap.o: imap.cyc
	$(CYCLONE) -c $< -o $@ --novcgen

imap.cyc: imap.bnf
	./yakker -gen command -no-main -lazyfill $< > $@

imapp.key:
	openssl genrsa > $@

imapp.cert: imapp.key
	openssl req -new -key $< -x509 -subj /C=aa/ST=b/L=c/O=d/OU=e/CN=shoebox.research.att.com > $@

imapp: imapp.o ykbuf.o ssl.o imap.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

imapp.o: examples/imapp.cyc
	$(CYCLONE) -c $^ -o $@ -I.

#
markdown: markdown.o ykbuf.o ssl.o bnf.o extract.o pr.o parse_tab.o util.o lex.o axarray.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

markdown.cyc: examples/markdown.yk yakker
	./yakker -no-main -gen elim-cr-line -d $< > $@

boxextract: boxextract.o ykbuf.o ssl.o
	$(CYCLONE) $^ -o $@ -lssl -lcrypto

boxextract.cyc: boxextract.yk yakker
	./yakker -gen lines $< > $@

string_%.cyc: %.cyc
	$(STRINGIFY) $< > $@

string_%.cyc: %.str
	$(STRINGIFY) $< > $@

prose_elim_grm.cyc: prose_elim_grm.bnf
	$(STRINGIFY) $< > $@

corerules_grm.cyc: corerules_grm.bnf
	$(STRINGIFY) $< > $@

%.d: %.cyc
	$(CYCLONE) -M -MG $< > $@

depend:

# Interferes with make clean, so do it manually at the end of this file
# -include $(addsuffix .d, $(FILES))

depend: $(addsuffix .d, $(FILES))

.SECONDARY: parse_tab.cyc

uri.p: yakker uri.bnf
	$(RM) $@.tmp
	./yakker -pads uri.bnf > $@.tmp
	cat $@.tmp | ./fix_hostname.pl | ./add_precord.pl URI_reference_t | cat > $@
	$(RM) $@.tmp

uri_one_string.p: yakker uri_orig.bnf
	$(RM) $@.tmp
	./yakker -pads -re uri_orig.bnf > $@.tmp
	cat $@.tmp | ./fix_hostname.pl | ./add_precord.pl URI_reference_t | cat > $@
	$(RM) $@.tmp

http.bnf: yakker rfc2616.bnf rfc2617.bnf rfc2396.bnf rfc2822.bnf rfc3501.bnf
	./yakker rfc2616.bnf \
	  -externals rfc2617.bnf rfc2396.bnf rfc2822.bnf rfc3501.bnf \
	   >$@

message-header.bnf: http.bnf yakker
	./yakker $< -subset message-header > $@

imap.bnf: rfc3501.bnf yakker
	./yakker rfc3501.bnf -tsort -unnamespace > $@
	echo 'my-greeting = ("" greeting)$$x {greeting_hook(x);}.' >> $@
	echo 'my-response = ("" response)$$x {response_hook(x);}.' >> $@
	echo 'my-command = ("" command)$$x {command_hook(x);}.' >> $@
	echo '{ extern void greeting_hook(const char ?); }' >> $@
	echo '{ extern void response_hook(const char ?); }' >> $@
	echo '{ extern void command_hook(const char ?); }' >> $@

vcard.bnf: rfc2426.bnf rfc2425.bnf rfc1738.bnf rfc1766.bnf rfc3501.bnf yakker
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

%.p: %.bnf yakker
	./yakker -pads $< > $@

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
rfc3501.bnf: rfc3501.txt yakker
	./yakker -extract $< -omit literal capability-data > $@.tmp
	echo 'literal = "{" (number "")$$x "}" CRLF @repeat(atoi(x))CHAR8.' >> $@.tmp
	echo 'capability-data = "CAPABILITY" *(SP capability).' >> $@.tmp
	./yakker $@.tmp -namespace rfc3501 > $@
	$(RM) $@.tmp

# A1, A2, and passwd are extracted by mistake
# Request-URI is the proper spelling (defined in rfc2616)
# challenge is defined 3 times, the first is correct, we fix by hand
# credentials is defined more than once too
rfc2617.bnf: rfc2617.txt yakker
	./yakker -extract $< -omit A1 A2 passwd challenge credentials \
          -rename request-uri Request-URI >$@
	echo "challenge = auth-scheme 1*SP 1#auth-param." >> $@
	echo "credentials = auth-scheme #auth-param." >> $@

# freshness_lifetime is extracted by mistake
# entity-body gets multiple definitions, we correct by hand
rfc2616.bnf: rfc2616.txt yakker
	./yakker -extract $< -omit freshness_lifetime \
          -omit entity-body >$@
	echo "entity-body = *OCTET." >> $@

# CRLF has multiple (syntactically different) defs, we correct by hand
# rulename has a bunch of examples and the correct def
# Due to an extraneous newline in a comment the defn. of CHAR is screwed
#   Same for char-val and prose-val
# A bunch of things that look like definitions in the file but aren't:
#   b, d, x command, mumble, bar, foo, ruleset
rfc2234.bnf: rfc2234.txt yakker
	./yakker -extract $< -omit CRLF rulename CHAR char-val prose-val b d x command mumble bar foo ruleset name >$@
	echo 'CRLF = CR LF.' >> $@
	echo 'rulename = ALPHA *(ALPHA / DIGIT / "-").' >> $@
	echo 'CHAR = %x01-7F.' >> $@
	echo 'char-val = DQUOTE *(%x20-21 / %x23-7E) DQUOTE.' >> $@
	echo 'prose-val = "<" *(%x20-3D / %x3F-7E) ">".' >> $@

rfc2822.bnf: rfc2822.txt yakker
	./yakker -extract $< >$@
	echo 'dtext = NO-WS-CTL / %d33-90 / %d94-126.' >> $@
	echo 'qtext = NO-WS-CTL / %d33 / %d35-91 / %d93-126.' >> $@
	echo 'ctext = NO-WS-CTL / %d33-39 / %d42-91 / %d93-126.' >> $@
	echo 'item-value = 1*angle-addr / addr-spec / atom / domain / msg-id.' >> $@
	echo 'obs-zone = "UT" / "GMT" / "EST" / "EDT" / "CST" / "CDT" / "MST" / "MDT" / "PST" / "PDT" / %d65-73 / %d75-90 / %d97-105 / %d107-122.' >> $@

rfc1738.bnf: rfc1738.txt yakker
	./yakker -extract $< -namespace rfc1738 > $@

rfc1766.bnf: rfc1766.txt yakker
	./yakker -extract $< -namespace rfc1766 >$@

rfc2425.bnf: rfc2425.txt yakker
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


rfc2426.bnf: rfc2426.txt yakker
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

rfc%.bnf: rfc%.txt yakker
	./yakker -extract $< >$@

.PHONY: cyclone-install

### dependencies generated by "make depend" and appended manually
analyze.o: analyze.cyc analyze.h ../../include/graph.h \
  ../../include/set.h ../../include/list.h ../../include/core.h \
  ../../include/iter.h ../../include/dict.h ../../include/cycboot.h bnf.h \
  ../../include/hashtable.h util.h cs.h pr.h fa.h ../../include/string.h
axarray.o: axarray.cyc axarray.h ../../include/core.h
bnf.o: bnf.cyc ../../include/string.h ../../include/core.h \
  ../../include/list.h ../../include/cycboot.h bnf.h \
  ../../include/hashtable.h ../../include/graph.h ../../include/set.h \
  ../../include/iter.h ../../include/dict.h util.h
boxextract.o: boxextract.cyc ../../include/buffer.h ../../include/core.h \
  ../../include/cycboot.h \
  /home/trevor/cyclone/bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdio.h \
  /home/trevor/cyclone/bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdlib.h \
  ../../include/string.h ../../include/list.h ykbuf.h ssl.h
corerules.o: corerules.cyc ../../include/hashtable.h ../../include/core.h \
  ../../include/string.h ../../include/list.h ../../include/cycboot.h \
  corerules.h bnf.h ../../include/graph.h ../../include/set.h \
  ../../include/iter.h ../../include/dict.h util.h
cs.o: cs.cyc cs.h bnf.h ../../include/core.h ../../include/hashtable.h \
  ../../include/graph.h ../../include/set.h ../../include/list.h \
  ../../include/iter.h ../../include/dict.h ../../include/cycboot.h \
  util.h ../../include/string.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/ctype.h \
  ../../include/buffer.h
extract.o: extract.cyc \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdio.h \
  ../../include/string.h ../../include/core.h ../../include/list.h \
  ../../include/cycboot.h ../../include/buffer.h bnf.h \
  ../../include/hashtable.h ../../include/graph.h ../../include/set.h \
  ../../include/iter.h ../../include/dict.h util.h pr.h extract.h prose.h
fa.o: fa.cyc ../../include/core.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdio.h \
  ../../include/string.h ../../include/list.h ../../include/cycboot.h \
  ../../include/hashtable.h bnf.h ../../include/graph.h \
  ../../include/set.h ../../include/iter.h ../../include/dict.h util.h \
  cs.h axarray.h analyze.h fa.h pr.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/time.h \
  ../../include/array.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/sys/stat.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/errno.h
lex.o: lex.cyc ../../include/lexing.h ../../include/core.h \
  ../../include/cycboot.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdio.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdlib.h \
  ../../include/string.h ../../include/list.h bnf.h \
  ../../include/hashtable.h ../../include/graph.h ../../include/set.h \
  ../../include/iter.h ../../include/dict.h util.h parse_tab.h
pads.o: pads.cyc ../../include/core.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdio.h \
  ../../include/list.h ../../include/string.h ../../include/cycboot.h \
  ../../include/set.h ../../include/iter.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/ctype.h \
  ../../include/graph.h ../../include/dict.h bnf.h \
  ../../include/hashtable.h util.h pr.h cs.h prose.h
parsergen.o: parsergen.cyc bnf.h ../../include/core.h \
  ../../include/hashtable.h ../../include/graph.h ../../include/set.h \
  ../../include/list.h ../../include/iter.h ../../include/dict.h \
  ../../include/cycboot.h util.h fa.h analyze.h cs.h pr.h pads.h \
  ../../include/string.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdio.h \
  ../../include/buffer.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdlib.h
parse_tab.o: parse_tab.cyc ../../include/string.h ../../include/core.h \
  ../../include/list.h ../../include/cycboot.h ../../include/lexing.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdio.h bnf.h \
  ../../include/hashtable.h ../../include/graph.h ../../include/set.h \
  ../../include/iter.h ../../include/dict.h util.h
pr.o: pr.cyc ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdio.h \
  ../../include/string.h ../../include/core.h ../../include/list.h \
  ../../include/cycboot.h ../../include/graph.h ../../include/set.h \
  ../../include/iter.h ../../include/dict.h bnf.h \
  ../../include/hashtable.h util.h pr.h
prose.o: prose.cyc ../../include/hashtable.h ../../include/core.h \
  ../../include/string.h ../../include/list.h ../../include/cycboot.h \
  prose.h bnf.h ../../include/graph.h ../../include/set.h \
  ../../include/iter.h ../../include/dict.h util.h
termgrammar.o: termgrammar.cyc bnf.h ../../include/core.h \
  ../../include/hashtable.h ../../include/graph.h ../../include/set.h \
  ../../include/list.h ../../include/iter.h ../../include/dict.h \
  ../../include/cycboot.h util.h pr.h analyze.h cs.h termgrammar.h \
  ../../include/buffer.h ../../include/string.h
util.o: util.cyc util.h ../../include/graph.h ../../include/set.h \
  ../../include/list.h ../../include/core.h ../../include/iter.h \
  ../../include/dict.h ../../include/cycboot.h
yakker.o: yakker.cyc bnf.h ../../include/core.h ../../include/hashtable.h \
  ../../include/graph.h ../../include/set.h ../../include/list.h \
  ../../include/iter.h ../../include/dict.h ../../include/cycboot.h \
  util.h pr.h pads.h extract.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdio.h cs.h prose.h \
  analyze.h fa.h parsergen.h termgrammar.h corerules.h namespaces.h \
  ../../include/string.h
ykbuf.o: ykbuf.cyc \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/unistd.h \
  ../../include/getopt.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/stdio.h \
  ../../bin/lib/cyc-lib/i686-pc-linux-gnu/include/errno.h \
  ../../include/string.h ../../include/core.h ../../include/list.h \
  ../../include/cycboot.h ykbuf.h ssl.h
