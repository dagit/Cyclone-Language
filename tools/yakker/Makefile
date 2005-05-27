CYCFLAGS=
#CYCFLAGS=-pg

FILES=analyze axarray bnf ykbuf corerules cs extract fa termgrammar lex pads parse_tab parsergen pr prose stringify util yakker

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

imap.cyc: imap.bnf
	./yakker -gen command -no-main -lazyfill $< > $@

stringify: stringify.cyc
	cyclone $^ -nogc -o $@

string_%.cyc: %.cyc stringify
	./stringify $< > $@

string_%.cyc: %.str stringify
	./stringify $< > $@

prose_elim_grm.cyc: prose_elim_grm.bnf stringify
	./stringify $< > $@

corerules_grm.cyc: corerules_grm.bnf stringify
	./stringify $< > $@

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

HTTP_NOWS=CRLF LWS quoted-pair HTTP-Version http_URL HTTP-date rfc1123-date rfc850-date asctime-date date1 date2 date3 time-http delta-seconds language-tag primary-tag subtag HEX separators LHEX ALPHA challenge Request-Line Status-Line byte-content-range-spec warn-agent warn-code warn-date warn-text warning-value Request generic-message trailer Chunked-Body chunk last-chunk Response qvalue entity-body month language-range
HTTP_OMIT=OCTET CHAR UPALPHA LOALPHA DIGIT CTL CR LF SP HT TEXT token ctext field-content Reason-Phrase message-body md5-digest
http.bnf: yakker ws.bnf fixup.bnf mailbox.bnf uri.bnf auth.bnf http-grammar.bnf
	./yakker ws.bnf -subset > $@
	./yakker fixup.bnf -subset >> $@
	./yakker mailbox.bnf -re-on -subset mailbox -rename ctext ctext-mailbox -rename comment comment-mailbox -rename quoted-pair quoted-pair-mailbox >> $@
	./yakker uri.bnf -re-on -subset host port relativeURI absoluteURI abs_path >> $@
	./yakker auth.bnf -ws-on -nows $(HTTP_NOWS) \
	  -subset authority challenge credentials -omit base64-user-pass >> $@
	./yakker http-grammar.bnf -ws-on -nows $(HTTP_NOWS) -subset \
	  -rename time time-http -omit $(HTTP_OMIT) >> $@

http1.bnf: yakker ws.bnf fixup.bnf rfc2822.bnf rfc2396.bnf rfc2617.bnf rfc2616.bnf
	./yakker ws.bnf -subset > $@
	./yakker fixup.bnf -subset >> $@
#	./yakker rfc2822.bnf -re-on -subset mailbox -rename ctext ctext-mailbox -rename comment comment-mailbox -rename quoted-pair quoted-pair-mailbox >> $@
	./yakker rfc2822.bnf -subset mailbox -rename quoted-pair quoted-pair-mailbox -rename quoted-string quoted-string-mailbox -rename comment comment-mailbox >> $@
	./yakker rfc2396.bnf -subset host port relativeURI absoluteURI abs_path >> $@
	./yakker rfc2617.bnf -ws-on -nows $(HTTP_NOWS) \
	  -subset authority challenge credentials -omit base64-user-pass \
          >> $@
	./yakker rfc2616.bnf -ws-on -nows $(HTTP_NOWS) -subset \
	  -omit $(HTTP_OMIT) >> $@

http-re.bnf: yakker ws.bnf fixup.bnf mailbox.bnf uri.bnf auth.bnf http-grammar.bnf
	./yakker ws.bnf -re-on -subset > $@
	./yakker fixup.bnf -re-on -subset >> $@
	./yakker mailbox.bnf -re-on -subset mailbox -rename ctext ctext-mailbox -rename comment comment-mailbox -rename quoted-pair quoted-pair-mailbox -omit ccontent >> $@
	./yakker uri.bnf -re-on -subset host port relativeURI absoluteURI abs_path >> $@
	./yakker auth.bnf -re-on -ws-on -nows $(HTTP_NOWS) \
	  -subset authority challenge credentials -omit base64-user-pass >> $@
	./yakker http-grammar.bnf -re-on -ws-on -nows $(HTTP_NOWS) -subset \
	  -rename time time-http -omit $(HTTP_OMIT) >> $@

message-header.bnf: http.bnf yakker
	./yakker $< -subset message-header > $@

imap.bnf: rfc3501.bnf yakker
	./yakker rfc3501.bnf -omit literal capability-data > $@.tmp
	echo 'literal = "{" (number "")$$x "}" CRLF @repeat(atoi(x))CHAR8.' >> $@.tmp
	echo 'capability-data = "CAPABILITY" *(SP capability).' >> $@.tmp
	echo 'my-greeting = ("" greeting)$$x {printf("S: %s",x);}.' >> $@.tmp
	echo 'my-response = ("" response)$$x {printf("S: %s",x);}.' >> $@.tmp
	./yakker $@.tmp -tsort > $@
	$(RM) $@.tmp

clean:
	$(RM) *.d *.o parse_tab.cyc parse_tab.h lex.cyc yakker
	$(RM) in out errs parse.output
	$(RM) http.bnf http.p message-header.bnf message-header.p
	$(RM) imap.bnf imap.cyc rfc3501.bnf rfc2234.bnf rfc3501.txt rfc2234.txt
	$(RM) string_dfa_engine1.*
	$(RM) prose_elim_grammar.cyc
	$(RM) prose_elim_grm.cyc corerules_grm.cyc
	$(RM) stringify
	$(RM) indexer indexer.cyc 1rfc_index 1rfc_index.txt
	$(RM) wf wf.cyc
	$(RM) http.cyc

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
	echo 'ctext = NO-WS-CTL / %d33-39 / %d42-91 / %d93-126.' >> $@
	echo 'item-value = 1*angle-addr / addr-spec / atom / domain / msg-id.' >> $@
	echo 'obs-zone = "UT" / "GMT" / "EST" / "EDT" / "CST" / "CDT" / "MST" / "MDT" / "PST" / "PDT" / %d65-73 / %d75-90 / %d97-105 / %d107-122.' >> $@

rfc%.bnf: rfc%.txt yakker
	./yakker -extract $< >$@


