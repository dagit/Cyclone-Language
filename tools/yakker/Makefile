CYCFLAGS=
#CYCFLAGS=-pg

FILES=analyze axarray bnf ykbuf corerules cs extract fa termgrammar lex pads parse_tab parsergen pr prose util yakker

all: yakker

yakker: analyze.o axarray.o bnf.o cs.o extract.o fa.o termgrammar.o lex.o pads.o \
  parse_tab.o parsergen.o pr.o prose.o prose_elim_grm.o string_dfa_engine1.o \
  string_dfa_engine2_1.o corerules.o corerules_grm.o \
  util.o yakker.o
	cyclone $(CYCFLAGS) $^ -o $@

testindexer: indexer 1rfc_index
	./indexer < 1rfc_index

indexer: indexer.o ykbuf.o ssl.o bnf.o extract.o pr.o parse_tab.o util.o lex.o
	cyclone $^ -o $@ -lssl -lcrypto

# --novcgen is temporary until a bug is fixed
indexer.o: indexer.cyc
	cyclone -c $< -o $@ --novcgen

indexer.cyc: examples/indexer.yk yakker
	./yakker -gen entry-list $< > $@

wf: wf.o ykbuf.o ssl.o
	cyclone $^ -o $@ -lssl -lcrypto

wf.cyc: examples/wf.yk yakker
	./yakker -gen file $< > $@

imapcl: imapcl.o ykbuf.o ssl.o imap.o
	cyclone $^ -o $@ -lssl -lcrypto

imapcl.o: examples/imapcl.cyc
	cyclone -c $^ -o $@ -I.

# --novcgen is temporary until a bug is fixed
imap.o: imap.cyc
	cyclone -c $< -o $@ --novcgen

imap.cyc: imap.bnf
	./yakker -gen command -no-main -lazyfill $< > $@

imapp.key:
	openssl genrsa > $@

imapp.cert: imapp.key
	openssl req -new -key $< -x509 -subj /C=aa/ST=b/L=c/O=d/OU=e/CN=shoebox.research.att.com > $@

imapp: imapp.o ykbuf.o ssl.o imap.o
	cyclone $^ -o $@ -lssl -lcrypto

imapp.o: examples/imapp.cyc
	cyclone -c $^ -o $@ -I.

string_%.cyc: %.cyc
	stringify $< > $@

string_%.cyc: %.str
	stringify $< > $@

prose_elim_grm.cyc: prose_elim_grm.bnf
	stringify $< > $@

corerules_grm.cyc: corerules_grm.bnf
	stringify $< > $@

%.d: %.cyc
	cyclone -M -MG $< > $@

-include $(addsuffix .d, $(FILES))
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
	./yakker rfc3501.bnf -tsort > $@
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
	./yakker rfc2426.bnf -externals rfc2425.bnf rfc1738.bnf rfc1766.bnf rfc3501.bnf -tsort > $@

clean:
	$(RM) *.d *.o parse_tab.cyc parse_tab.h lex.cyc yakker
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

%.p: %.bnf yakker
	./yakker -pads $< > $@

%.o: %.cyc
	cyclone $(CYCFLAGS) -c $<

%_tab.cyc: %.y
	cycbison -d $<
#	cycbison -d --debug --verbose $<

%_tab.h: %.y
	cycbison -d $<
#	cycbison -d --debug --verbose $<

%.cyc: %.cyl
	cyclex $< $@

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
	./yakker -extract $< -omit literal capability-data > $@
	echo 'literal = "{" (number "")$$x "}" CRLF @repeat(atoi(x))CHAR8.' >> $@
	echo 'capability-data = "CAPABILITY" *(SP capability).' >> $@

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

rfc2425.bnf: rfc2425.txt yakker
	# time-numzome should be time-numzone
	./yakker -extract $< -omit time-numzome > $@
	echo 'time-numzone = sign time-hour [":"] time-minute.' >> $@

rfc2426.bnf: rfc2426.txt yakker
	# value and param have multiple definitions
	# the rename corrects a typo (but it is in a value which we omit...)
	./yakker -extract $< -rename snd-line-value snd-inline-value -omit value param > $@
	# these are the right definitions of value and param
	echo 'value = *VALUE-CHAR.' >> $@
	echo 'param = param-name "=" param-value *("," param-value).' >> $@
	# typos prevent extraction of these
	echo 'ESCAPED-CHAR = "\\" / "\;" / "\," / ("\" %d110) / ("\" %d78).' >> $@
	echo 'date-time-value = date-time.' >> $@
	echo 'snd-inline-param = ("VALUE" "=" "binary") / ("ENCODING" "=" "b") / ("TYPE" "=" *SAFE-CHAR).' >> $@
	# fill in some missing definitions
	echo 'uri = genericurl.' >> $@
	echo 'word = iana-token.' >> $@
	echo 'iana-type = iana-token.' >> $@

rfc%.bnf: rfc%.txt yakker
	./yakker -extract $< >$@


