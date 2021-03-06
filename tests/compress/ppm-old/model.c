/* PPM* model for arithmetic encoder. */
#include "stdio.h"
#include "assert.h"
#include "arithcode.h"
#include "model.h"

#define FILE_SIZE 13000000 /* Maximum size of a file */
#define TRIE_SIZE 2000000 /* Maximum number of trie records. */
#define SYMBOLS_SIZE 3000000 /* Maximum number of symbols records. */
#define CLIST_SIZE 16000 /* Maximum size of the context list */
#define SYMBOL_SIZE 257 /* Number of symbols + 1 for eof */
#define ESCAPE (SYMBOL_SIZE) /* for encoding escape symbols */
#define CONTEXT_SIZE 8 /* Number of context lengths to store for statistics */
#define MAX_TOTAL_COUNT 16383 /* Max. total count used for scaling the counts */
#define MIN_TOTAL_COUNT 258 /* Mininum max. total count used for scaling the counts */
#define MIN_ORDER -1 /* Minimum fixed order */
#define MAX_ORDER 5 /* Default maximum fixed order */
#define TCOUNT_INIT 1 /* Initial count for new node */
#define TESCAPE_INIT 1 /* Initial escape count for new node */
#define TCOUNT_INCR 2 /* Increment count for node */
#define TESCAPE_INCR 1 /* Increment escape count for node */
#define NIL 0 /* Indicates ptr is nil */

#define ROOT 1 /* root of Trie */

/* the following global variables are set from the command line arguments */
/* debugging output is redirected to stderr */
/* Levels of debugging output :
 * level 1 : trace of arithmetic coding values
 * level 2 : level 1 plus prediction output
 * level 3 : level 2 plus dump of tries and context list
 * level 4 : comprehensive trace information
 */
static int debugLevel = 0; /* for various levels of debugging output */
static int debugContextlengths = 0; /* for dumping out the context lengths */

static int max_total_count = MAX_TOTAL_COUNT;
static int max_order = MAX_ORDER;
static int preload_limit = NO_PRELOAD_LIMIT;
static int tcount_init = TCOUNT_INIT;
static int tescape_init = TESCAPE_INIT;
static int tcount_incr = TCOUNT_INCR;
static int tescape_incr = TESCAPE_INCR;
static float determ_factor = 3.0;
static float recent_factor = 0.1;

static unsigned char Input [FILE_SIZE]; /* the input file */
static int inputPos = 0; /* Current position in the input */
static int inputLen = 0; /* length of input */

struct contextType { /* context list record */
    int Cptr; /* pointer to node in the trie */
    int Cnext; /* next in the context list */
};

static struct contextType Context [CLIST_SIZE];
/* linked list of the current active contexts, longest first */
static int contextLength; /* length of the context list */
static int contextUsed; /* list of deleted context list records */
static int contextFree; /* next unused context list record */
static int contextHead; /* Head of the context list = longest context */
static int contextTail; /* Tail of the context list = shortest context */

struct symbolsType { /* symbol list record */
    int Sptr; /* pointer to node in the trie */
    int Snext; /* next symbol in the list */
    int Symbol; /* the symbol */
};

static struct symbolsType Symbols [SYMBOLS_SIZE];
/* for storing lists of symbols associated with each node in the Trie */
static int symbolsFree; /* next unused symbol record */

struct trieType { /* trie record */
    int Tsymbols; /* linked list of symbols & ptrs to lower levels in the trie */
    int Tcount; /* count associated with the node; sum of counts of all lower levels */
    int Tescape; /* escape count associated with the node */
    int Trecent; /* most recent symbol */
};

static struct trieType Trie [TRIE_SIZE]; /* the trie */
static int trieFree; /* next unused trie record */

void init_arguments(argc,argv,argp)
int argc;
char *argv[];
int argp;
{
    /* set defaults */
    debugLevel = 0;

    if (*argv [0] != '-') {
	fprintf( stderr, "Invalid argument\n" );
	exit(1);
    }
    argp++;
    sscanf( *argv++, "-%d", &debugLevel );
    if (debugLevel >= 20) {
	switch (debugLevel) {
          case 21 : /* print out contextlengths */
            debugLevel = 0;
            debugContextlengths = 1;
            break;
	}
    }
    if (debugLevel > 0)
	fprintf( stderr, "debug Level %d\n", debugLevel );

    if (argp <= argc) {
	argp++;
	sscanf( *argv++, "%d", &max_order );
	if (max_order < MIN_ORDER)
	    max_order = MIN_ORDER;
    }
    if (argp <= argc) {
	argp++;
	sscanf( *argv++, "%d", &preload_limit );
    }
    if (argp <= argc) {
	argp++;
	sscanf( *argv++, "%f", &determ_factor );
	if (debugLevel > 0)
	    fprintf( stderr, "deterministic factor = %4.2f\n", determ_factor );
    }
    if (argp <= argc) {
	argp++;
	sscanf( *argv++, "%f", &recent_factor );
	if (debugLevel > 0)
	    fprintf( stderr, "recent factor = %4.2f\n", recent_factor );
    }
    if (argp <= argc) {
	argp++;
	sscanf( *argv++, "%d", &tcount_init );
	if (tcount_init < 1)
	    tcount_init = 1;
    }
    if (argp <= argc) {
	argp++;
	sscanf( *argv++, "%d", &tescape_init );
	if (tescape_init < 1)
	    tescape_init = 1;
    }
    if (argp <= argc) {
	argp++;
	sscanf( *argv++, "%d", &tcount_incr );
	if (tcount_incr < 1)
	    tcount_incr = 1;
    }
    if (argp <= argc) {
	argp++;
	sscanf( *argv++, "%d", &tescape_incr );
	if (tescape_incr < 1)
	    tescape_incr = 1;
    }
    if (argp <= argc) {
	argp++;
	sscanf( *argv++, "%d", &max_total_count );
	if (max_total_count > MAX_TOTAL_COUNT)
	    max_total_count = MAX_TOTAL_COUNT;
	else if (max_total_count < MIN_TOTAL_COUNT)
	    max_total_count = MIN_TOTAL_COUNT;
	if (debugLevel > 0)
	    fprintf( stderr, "max total count = %d\n", max_total_count );
    }
}

void initContext()
/* Initialize the context list for the linked list of context pointers */
{
    contextUsed = 0;
    contextFree = 0;
    contextHead = 0;
    contextTail = 0;
}

int freeContext()
/* Return a new pointer to a list record */
{
    int c;

    if (contextUsed == NIL) {
      if (contextFree < CLIST_SIZE) {
          contextFree += 1;
          c = contextFree;
      }
      else {
          fprintf( stderr, "Fatal error: out of context list space at position %d\n",
		   inputPos );
	  exit(1);
      }
    }
    else { /* use the first record on the free list */
      c = contextUsed;
      contextUsed = Context [c].Cnext;
    }

    if (c != NIL) {
	Context [c].Cptr = NIL;
	Context [c].Cnext = NIL;
    }

    return( c );
}

void releaseList( c )
/* Release the context list C to the free list */
int c;
{
   Context [c].Cnext = contextUsed;
   contextUsed = c;
   contextLength--;
}

void dumpList( fp, c )
/* Dump the context list C */
FILE *fp;
int c;
{
    fprintf( fp, "Dump of context list : " );
    while (c) {
	fprintf( fp, "%d ", Context [c].Cptr );
	c = Context [c].Cnext;
    }
    fprintf( fp, "\n" );
}

void dumpSymbol( fp, symbol )
/* Dump the symbol */
FILE *fp;
int symbol;
{
    if ((symbol <= 32) || (symbol >= 127))
        fprintf( fp, "<%d>", symbol );
    else
        fprintf( fp, "%c", symbol );
}

void dumpString( fp, str, pos, len )
/* Dump the string STR starting at position POS. */
FILE *fp;
char *str;
int pos;
int len;
{
    char cc;
    int p;

    for (p = pos; p<pos+len; p++) {
	cc = str [p];
	if ((cc <= 31) || (cc >= 127))
	    fprintf( fp, "<%d>", cc );
	else
            fprintf( fp, "%c", cc );
    }
}

char dumpTrieStr [CLIST_SIZE];

void dumpTrie( fp, t, d )
/* Dump the trie T; d is 0 when at the top level. */
FILE *fp;
int t;
int d;
{
    int sym, s, sptr;

    fprintf( fp, "%5d %5d ", d, t );
    if (t < 0) /* pointer to input */
	fprintf( fp, "               <" );
    else {
	fprintf( fp, " %5d %5d   <", Trie [t].Tescape, Trie [t].Tcount );
    }

    dumpString( fp, dumpTrieStr, 0, d );
    fprintf( fp, ">\n" );
    if (t > 0) {
      s = Trie [t].Tsymbols;
      while (s != NIL) {
	sym = Symbols [s].Symbol;
	sptr = Symbols [s].Sptr;
	dumpTrieStr [d] = sym;
	dumpTrie( fp, sptr, d+1 );
	s = Symbols [s].Snext;
      }
    }
}

void dumpRoot( fp )
/* Dump the root trie. */
FILE *fp;
{
    fprintf( fp, "Dump of Trie : \n" );
    fprintf( fp, "---------------\n" );
    fprintf( fp, "depth node   esc count context\n" );
    dumpTrie( fp, ROOT, 0 );
    fprintf( fp, "---------------\n" );
    fprintf( fp, "\n" );
}

void junk()
{
    fprintf( stderr, "Got here\n" );
    dumpRoot( stderr );
}

int getCount( s )
/* Gets the count for the symbol at node s in the symbol list. */
int s;
{
    int freq, sptr;

    if (s == NIL)
	return( 0 );

    freq = 0; /* frequency count for the symbol */
    sptr = Symbols [s].Sptr;
    assert( sptr != 0 );
    if (sptr < 0) /* points to input stream */
	freq = 1;
    else if (sptr == 0)
	freq = 0;
    else
	freq = Trie [sptr].Tcount;
    if ((debugLevel > 3) && (freq > 0)) {
	fprintf( stderr, "getCount for symbol " );
	dumpSymbol( stderr, Symbols [s].Symbol );
	fprintf( stderr, " at node %d = %d\n", sptr, freq );
    }
    return( freq );
}

int freeTrie()
/* Return a pointer to the next free trie record. */
{
    int t;

    if (trieFree >= TRIE_SIZE) {
	fprintf( stderr, "Fatal error: out of trie space at position %d\n",
		 inputPos );
	exit(1);
    }
    t = trieFree++;
    Trie [t].Tsymbols = NIL;
    Trie [t].Tcount = 0;
    Trie [t].Tescape = 0;
    Trie [t].Trecent = -1;
    return( t );
}

void initTrie()
/* Initialize the trie. */
{
    trieFree = ROOT + 1;
    Trie [ROOT].Tsymbols = NIL;
    Trie [ROOT].Tcount = 0;
    Trie [ROOT].Tescape = 0;
    Trie [ROOT].Trecent = -1;
}

void initSymbols()
/* Initialize the trie. */
{
    symbolsFree = 1;
}

int freeSymbol(symbol)
/* Return a pointer to the next free symbols record. */
int symbol;
{
    int s;

    if (symbolsFree >= SYMBOLS_SIZE) {
	fprintf( stderr, "Fatal error: out of symbols space at position %d\n",
		 inputPos );
	exit(1);
    }
    s = symbolsFree++;
    Symbols [s].Sptr = NIL;
    Symbols [s].Snext = NIL;
    Symbols [s].Symbol = symbol;
    return( s );
}

int findSymbol( t, symbol )
/* Return a pointer to the trie node that contains the symbol. */
int t;
int symbol;
{
    int s, sym;

    s = Trie [t].Tsymbols;
    while (s != NIL) {
	sym = Symbols [s].Symbol;
	if (sym == symbol)
	    return( s );
	s = Symbols [s].Snext;
    }
    return( NIL );
}

int addSymbol( t, symbol )
/* Add a new symbol if required and return a pointer to it. */
int t;
int symbol;
{
    int s;

    s = findSymbol( t, symbol ); /* does the symbol already exist? */
    if (!s) { /* symbol not found */
      /* create new node for symbol */
	s = freeSymbol( symbol );
	if (Trie [t].Tsymbols == 0) /* empty list */
	    Trie [t].Tsymbols = s;
	else { /* add the new symbol at the head of the list */
	    Symbols [s].Snext = Trie [t].Tsymbols;
	    Trie [t].Tsymbols = s;
	}
    }
    return( s );
}

void startContext()
{
    int c;

    if (contextLength-1 > max_order)
	return; /* don't add any new contexts longer than max_order */

    /* create new context pointing to root of trie for the new symbol */
    c = freeContext();
    Context [c].Cptr = ROOT;
    Context [c].Cnext = NIL;
    /* now add new context at tail of the contexts list */
    if (contextTail == NIL)
        contextHead = c;
    else
        Context [contextTail].Cnext = c;
    contextTail = c;
    contextLength++;
    if (debugLevel > 2) {
        fprintf( stderr, "\n" );
        dumpList( stderr, contextHead );
    }
}

void printFree(fp)
FILE *fp;
{
   fprintf( fp, "Tries used: %d Tries unused %d, Lists used: %d Lists unused: %d\n",
	    trieFree-1, (TRIE_SIZE-trieFree), contextFree-1, (CLIST_SIZE-contextFree) );
} /* printFree */

int predict()
/* Make a prediction of the next symbol. Return a pointer to the predicted context node. */
{
    int context;

    context = contextHead; /* start at the longest context */
    if ((context != NIL) && (contextLength-1 > max_order))
	context = Context [context].Cnext; /* go to next context */
    return( context );
}

void arith_encode( lbnd, hbnd, totl )
/* Arithmetically encode the range. */
int lbnd, hbnd, totl;
{
    if ((lbnd < 0) || (hbnd < 0) || (totl < 0))
        fprintf( stderr, "Fatal error - invalid range : lbnd %d hbnd %d totl %d at position %d\n", lbnd, hbnd, totl, inputPos );
    assert( (lbnd >= 0) && (hbnd >= 0) && (totl >= 0) );

    if (totl > max_total_count ) {
        fprintf( stderr, "Fatal error - totl too big : lbnd = %d, hbnd = %d, totl = %d at position %d\n",
		 lbnd, hbnd, totl, inputPos );
	dumpRoot( stderr );
    }
    assert( totl <= max_total_count );

    if ((lbnd == 0) && (hbnd == totl)) {
	/* probility = 1 - no need to encode it */
	if (debugLevel > 0)
	    fprintf( stderr, "probability = 1\n" );
    }
    else
	arithmetic_encode( lbnd, hbnd, totl );
}

int arith_decode_target( totl )
/* Arithmetically decodes the target. */
int totl;
{
    int target;
    target = arithmetic_decode_target( totl );
    if (debugLevel > 1)
        fprintf( stderr, "target = %d, totl = %d\n", target, totl );
    return( arithmetic_decode_target( totl ));
}

void arith_decode( lbnd, hbnd, totl )
/* Arithmetically decode the range. */
int lbnd, hbnd, totl;
{
    if ((lbnd < 0) || (hbnd < 0) || (totl < 0))
        fprintf( stderr, "Fatal error - invalid range : lbnd %d hbnd %d totl %d at position %d\n", lbnd, hbnd, totl, inputPos );
    assert( (lbnd >= 0) && (hbnd >= 0) && (totl >= 0) );
    if ((lbnd == 0) && (hbnd == totl)) {
        /* probility = 1 - no need to decode it */
        if (debugLevel > 0)
            fprintf( stderr, "probability = 1\n" );
    }
    else {
        if (debugLevel > 0)
            fprintf( stderr, "lbnd = %d, hbnd = %d, totl = %d\n", lbnd, hbnd, totl );
        arithmetic_decode( lbnd, hbnd, totl );
    }
}

void scale_counts( t )
/* Scale the symbols at node T of the trie if required. */
int t;
{
    int s, sptr;

    Trie [t].Tcount = (Trie [t].Tcount + 1)/2;
    Trie [t].Tescape = (Trie [t].Tescape + 1)/2;
    s = Trie [t].Tsymbols;
    while (s != NIL) { /* scale all the counts */
	sptr = Symbols [s].Sptr;
	if (sptr > 0)
	    Trie [sptr].Tcount = (Trie [sptr].Tcount + 1)/2;
	s = Symbols [s].Snext;
    }
}

void encode_counts( t, symbol, exclusions, load )
/* Encode the symbol using the counts at node t in the trie. Do not include any
   of the exclusions. (If t is NIL, then encode using order-1 frequencies). */
int t;
int symbol;
int exclusions [SYMBOL_SIZE];
int load;
{
    int s, sym, freq;
    int lbnd, hbnd, totl;

    if (debugLevel > 1) {
	if (symbol == ESCAPE)
	    fprintf( stderr, "Encoding escape symbol\n" );
	else {
	    fprintf( stderr, "Encoding symbol (%d) ", symbol );
	    dumpSymbol( stderr, symbol );
	    fprintf( stderr, "\n" );
	}
    }

    lbnd = 0; hbnd = 0; totl = 0;
    if (t == NIL) { /* order-1 */
	for (sym = 0; sym<SYMBOL_SIZE; sym++) {
	    if (!exclusions [sym]) {
		freq = 1; /* set order-1 frequency to 1 */
		if (sym == symbol) {
		    lbnd = totl;
		    hbnd = lbnd + freq;
		}
		totl += freq;
	    }
	}
    }
    else {
	for (;;) { /* keep scaling until totl < max_total_count */
	    int determ;

	    lbnd = 0;
	    totl = Trie [t].Tescape; /* add in escape count */
	    if (!totl) { /* escape count must be non-zero, except at pos 0 */
		assert( inputPos == 0 );
		return;
	    }
	    determ = (totl == 1);
	    hbnd = totl;
	    s = Trie [t].Tsymbols;
	    while (s != NIL) {
		sym = Symbols [s].Symbol;
		if (!exclusions [sym]) {
		    freq = getCount( s );
		    if ((determ) && (freq > 1))
		        freq *= determ_factor;
		    else if ((sym == Trie [t].Trecent) && (Trie [t].Tcount))
		        freq += Trie [t].Tcount * recent_factor;
		    if (sym == symbol) {
			lbnd = totl;
			hbnd = lbnd + freq;
		    }
		    totl += freq;
		}
		s = Symbols [s].Snext;
	    }
	    if (totl <= max_total_count) /* No need to scale the counts */
	        break;
	    scale_counts( t ); /* try again */
	}
    }

    if ((hbnd <= lbnd) || (totl < hbnd)) {
        fprintf( stderr, "Fatal error - invalid range : lbnd %d hbnd %d totl %d at position %d\n",
		 lbnd, hbnd, totl, inputPos );
	fprintf( stderr, "Dumping node %d\n", t );
	dumpTrie( stderr, t, 0 );
    }
    assert( hbnd > lbnd );
    assert( totl >= hbnd );

    if (!load)
        arith_encode( lbnd, hbnd, totl );

    if (totl > max_total_count) {
        fprintf( stderr, "Check range : lbnd %d hbnd %d totl %d at position %d\n",
		 lbnd, hbnd, totl, inputPos );
	fprintf( stderr, "Dumping node %d\n", t );
	dumpTrie( stderr, t, 0 );
	exit(1);
    }
    if (debugLevel > 0)
	fprintf( stderr, "lbnd = %d hbnd = %d totl = %d\n", lbnd, hbnd, totl );
}

int decode_counts( t, exclusions )
/* Decode the symbol at node t in the trie. Do not include any of the exclusions. */
int t;
int exclusions [SYMBOL_SIZE];
{
    int s, sym, freq, target, determ;
    int lbnd, hbnd, totl;

    /* Find the total count first */
    if (t == NIL) { /* order-1 */
	totl = 0;
	for (sym = 0; sym<SYMBOL_SIZE; sym++) {
	    if (!exclusions [sym])
	        totl += 1; /* order-1 frequency = 1 */
	}
    }
    else {
	for (;;) { /* keep scaling until totl < max_total_count */
	    totl = Trie [t].Tescape;
	    if (!totl) { /* escape count must be non-zero, except at pos 0 */
	        assert( inputPos == 0 );
		return( ESCAPE );
	    }
	    determ = (totl == 1);
	    s = Trie [t].Tsymbols;
	    while (s != NIL) {
	        sym = Symbols [s].Symbol;
		if (!exclusions [sym]) {
		    freq = getCount( s );
		    if ((determ) && (freq > 1))
		        freq *= determ_factor;
		    else if ((sym == Trie [t].Trecent) && (Trie [t].Tcount))
		        freq += Trie [t].Tcount * recent_factor;
		    totl += freq;
		}
		s = Symbols [s].Snext;
	    }
	    if (totl <= max_total_count) /* No need to scale the counts */
	        break;
	    scale_counts( t ); /* try again */
	}
    }
    target = arith_decode_target( totl );

    /* Find the symbol and arithmetic range which target falls into */
    if (t == NIL) { /* order-1 */
	hbnd = 0;
	for (sym = 0; (sym<SYMBOL_SIZE) && (hbnd <= target); sym++) {
	    if (!exclusions [sym])
	        hbnd += 1; /* order-1 frequency = 1 */
	}
	lbnd = hbnd - 1;
	sym--;
    }
    else {
	sym = ESCAPE; /* first symbol is an escape */
	hbnd = Trie [t].Tescape;
	determ = (hbnd == 1);
	freq = hbnd;
	s = Trie [t].Tsymbols;
	while ((s != NIL) && (hbnd <= target)) {
	    sym = Symbols [s].Symbol;
	    if (!exclusions [sym]) {
	        freq = getCount( s );
		if ((determ) && (freq > 1))
		    freq *= determ_factor;
		else if ((sym == Trie [t].Trecent) && (Trie [t].Tcount))
		    freq += Trie [t].Tcount * recent_factor;
		hbnd += freq;
	    }
	    s = Symbols [s].Snext;
	}
	lbnd = hbnd - freq;
    }

    if (hbnd <= lbnd)
        fprintf( stderr, "Fatal error - invalid range : lbnd %d hbnd %d totl %d at position %d\n",
		 lbnd, hbnd, totl, inputPos );
    assert( hbnd > lbnd );
    arith_decode( lbnd, hbnd, totl );

    if (debugLevel > 0) {
	if (sym == ESCAPE)
	    fprintf( stderr, "Decoded escape symbol\n" );
	else {
	    fprintf( stderr, "Decoded symbol (%d) ", sym );
	    dumpSymbol( stderr, sym );
	    fprintf( stderr, "\n" );
	}
    }
    return( sym );
}

void encode_context( context, symbol, load )
/* Encode the symbol at the context in the trie. */
int context;
int symbol;
int load;
{
    int s, sym, cptr, freq;
    int exclusions [SYMBOL_SIZE];

    /* set up the exclusions array */
    for (s=0; s < SYMBOL_SIZE; s++)
        exclusions [s] = 0;

    while (context > NIL) { /* encode escapes until we find a context that has the symbol */
	cptr = Context [context].Cptr;
	s = findSymbol( cptr, symbol ); /* get the symbol's node for the context */
	freq = getCount( s );
	if (freq != 0)
	    break;

	/* Symbol not found in current context; escape to shorter contexts */
	encode_counts( cptr, ESCAPE, exclusions, load );
        Trie [cptr].Tescape += tescape_incr;

	/* exclude all symbols in current context from being counted in shorter contexts */
	s = Trie [cptr].Tsymbols;
	while (s != NIL) {
	    sym = Symbols [s].Symbol;
	    exclusions [sym] = 1; /* 1 means excluded */
	    s = Symbols [s].Snext;
	}
        context = Context [context].Cnext; /* Go to next longest context */
    }
    if (context == NIL)
	cptr = NIL;
    else
	cptr = Context [context].Cptr; /* get the node associated with the context */
    encode_counts( cptr, symbol, exclusions, load );
}

int decode_context( context )
/* Decode the symbol based on the context at node t. */
int context;
{
    int s, sym, cptr, symbol;
    int exclusions [SYMBOL_SIZE];

    /* set up the exclusions array */
    for (s=0; s < SYMBOL_SIZE; s++)
        exclusions [s] = 0;

    for (;;) { /* keep decoding until no more escapes */
	cptr = Context [context].Cptr; /* get the node associated with the context */
	
	symbol = decode_counts( cptr, exclusions );
	if (symbol != ESCAPE)
	    return( symbol );

        Trie [cptr].Tescape += tescape_incr;

	/* exclude all symbols in current context from being counted in shorter contexts */
	s = Trie [cptr].Tsymbols;
	while (s != NIL) {
	    sym = Symbols [s].Symbol;
	    exclusions [sym] = 1; /* 1 means excluded */
	    s = Symbols [s].Snext;
	}

        context = Context [context].Cnext; /* Go to next longest context */
    }
}

int nextContext( context )
/* Return a pointer to the next context. */
int context;
{
    return( Context [context].Cnext );
}

int dropContext( context )
/* Drop the context from the context list, and return a pointer to the next
   in the list. */
int context;
{
    if (context != contextHead) {
	printFree( stderr );
	fprintf( stderr, "Fatal error at position %d\n", inputPos );
    }
    assert( context == contextHead ); /* must be at head of context list */

    /* remove this context from the context list */
    contextHead = Context [context].Cnext;
    if (contextHead == NIL)
        contextTail = NIL;
    releaseList( context );
    context = contextHead;

    return( context );
}

int upd( context, symbol, contextLen, updatecnt )
/* Update the symbol in the context. Return the next context. */
int context;
int symbol;
int contextLen;
int updatecnt;
{
    int p, s, cptr, snew, tnew;

    cptr = Context [context].Cptr;
    assert( cptr > NIL );

    if (symbol != Trie [cptr].Trecent)
	Trie [cptr].Trecent = symbol;

    if (updatecnt) /* this test performs update exclusions */
        Trie [cptr].Tcount += tcount_incr; /* Only add the count for the longest context */

    if (contextLen-1 > max_order)
        return( dropContext( context ));

    s = addSymbol( cptr, symbol );
    p = Symbols [s].Sptr;
    if (p > 0) { /* in middle of trie */
	Context [context].Cptr = p; /* replace context with next level down */
	return( nextContext(context) ); /* move to next context */
    }
    else if (p == 0) { /* context has not occurred before */
	Symbols [s].Sptr = - (inputPos);
        return( dropContext(context) ); /* new context, so drop from context list */
    }
    else { /* p < 0; previous context points to input stream; add it one level down */
	tnew = freeTrie();
	Trie [tnew].Tcount = tcount_init; /* set count to 1 */
	Trie [tnew].Tescape = tescape_init; /* set escape count to 1 */
	Symbols [s].Sptr = tnew;
	if (contextLen-1 < max_order) {
	    snew = addSymbol( tnew, (int) Input [-p] ); /* next one in input */
	    Symbols [snew].Sptr = p-1; /* point to next one back in input */
	}
	Context [context].Cptr = tnew; /* replace context with new ptr */
	return( nextContext(context) ); /* move to next context */
    }
} /* upd */

void update( symbol )
/* Update the Trie structure using the incoming symbol at the current input position. */
int symbol;
{
    int context, contextLen, updatecnt;

    if (inputPos >= FILE_SIZE) {
	fprintf( stderr, "Fatal error: out of file space at position %d\n",
		 inputPos );
	exit(1);
    }
    inputPos++;

    /* Update all the current contexts */
    context = contextHead;
    contextLen = contextLength;
    updatecnt = 1;
    while (context != NIL) {
	context = upd( context, symbol, contextLen, updatecnt );
	contextLen--;
	updatecnt = 0;
    }
} /* update */

void init_symbols()
/* Initialize the model. */
{
    initContext();
    initSymbols();
    initTrie();
}

int eof_symbol()
/* Return the end of file symbol */
{
    return( SYMBOL_SIZE-1 );
}

void encode_symbol( symbol, load )
/* Encode the symbol. */
int symbol;
int load;
{
    int pContext; /* node in the trie predicted to have "best" context */

    if (debugLevel > 1) {
        fprintf( stderr, "Input " );
	dumpSymbol( stderr, symbol );
        fprintf( stderr, " at %d\n", inputPos );
	if ((inputPos % 200) == 0)
	  fprintf( stderr, "pos %d context %d symbols %d trie %d\n",
		   inputPos, contextFree, symbolsFree, trieFree );
    }

    startContext();
    pContext = predict(); /* Choose the context */

    /* Encode the symbol based on the predicted context */
    encode_context( pContext, symbol, load );

    update( symbol );

    if (debugLevel > 2)
        dumpRoot( stderr );
}

int decode_symbol()
/* Return the decoded symbol. */
{
    int pContext; /* node in the trie with the predict "best" context */
    int symbol;

    startContext();
    pContext = predict(); /* Choose the context */

    /* Find the symbol based on the predicted context */
    symbol = decode_context( pContext );

    Input [inputPos] = symbol;
    update( symbol );

    if (debugLevel > 2)
        dumpRoot( stderr );

    return( symbol );
}

void report_model()
/* Report back on statistics collected during encoding */
{
    dumpRoot( stderr );
}

int preloadInput()
/* Return the preload limit for the Input. */
{
    return( preload_limit );
}

int loadInput( fp, p, max )
FILE *fp;
int p;
int max;
{
    int cc;

    while ((cc = getc( fp )) != EOF) {
	Input [p++] = cc;
	if ((max > 0) && (p >= max))
	    break;
    }
    inputLen = p;
    return( p );
}

void startInput()
{
    inputPos = 0;
}

int getInput()
{
    if (inputPos >= inputLen)
        return( -1 );
    else
        return( Input [inputPos] );
}

int lengthInput()
{
    return( inputLen );
}
