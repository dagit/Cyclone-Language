/*
	Declarations for arithmetic coding.
	Alistair Moffat, alistair@cs.mu.oz.au, July 1987.

	@(#)arithcode.h	1.1 5/3/91
*/

#define codevaluebits 16
#if (codevaluebits<16)
typedef unsigned short codevalue;
#else
typedef unsigned long  codevalue;
#endif

#define topvalue ((codevalue)((1<<codevaluebits)-1))
#define maxfrequency ((topvalue+1)/4 + 1)

#define firstqtr (topvalue/4+1)
#define half     (2*firstqtr)
#define thirdqtr (3*firstqtr)

#define	escape_event	U->totalcnt-U->notfound, U->totalcnt, U->totalcnt

void arithmetic_encode();
/* codevalue  arithmetic_decode_target(); */
#define	arithmetic_decode_target(totl)	\
	(  ((S_value-S_low+1)*totl-1) / (S_high-S_low+1)  )
void arithmetic_decode();

void startencoding();
void startoutputingbits();
void doneoutputingbits();
void doneencoding();
void startinputingbits();
void startdecoding();

codevalue	S_low, S_high, S_value;
long		S_bitstofollow;
int		S_buffer, S_bitstogo;

long		cmpbytes, rawbytes;
