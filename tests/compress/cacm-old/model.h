/* INTERFACE TO THE MODEL. */


/* THE SET OF SYMBOLS THAT MAY BE ENCODED. */

#define No_of_chars 256			/* Number of character symbols      */
#define EOF_symbol (No_of_chars+1)	/* Index of EOF symbol              */

#define No_of_symbols (No_of_chars+1)	/* Total number of symbols          */


/* TRANSLATION TABLES BETWEEN CHARACTERS AND SYMBOL INDEXES. */

int char_to_index[No_of_chars];		/* To index from character          */
unsigned char index_to_char[No_of_symbols+1]; /* To character from index    */


/* CUMULATIVE FREQUENCY TABLE. */

#define Max_frequency 16383		/* Maximum allowed frequency count  */

int cum_freq[No_of_symbols+1];		/* Cumulative symbol frequencies    */
