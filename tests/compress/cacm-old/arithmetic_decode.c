/* ARITHMETIC DECODING ALGORITHM. */

#include "arithmetic_coding.h"


/* CURRENT STATE OF THE DECODING. */

static code_value value;	/* Currently-seen code value                */
static code_value low, high;	/* Ends of current code region              */


/* START DECODING A STREAM OF SYMBOLS. */

start_decoding()
{   int i;
    value = 0;					/* Input bits to fill the   */
    for (i = 1; i<=Code_value_bits; i++) {	/* code value.              */
        value = 2*value+input_bit();
    }
    low = 0;					/* Full code range.         */
    high = Top_value;
}


/* DECODE THE NEXT SYMBOL. */

int decode_symbol(cum_freq)
    int cum_freq[];		/* Cumulative symbol frequencies            */
{   long range;			/* Size of current code region              */
    int cum;			/* Cumulative frequency calculated          */
    int symbol;			/* Symbol decoded                           */
    range = (long)(high-low)+1;
    cum = 					/* Find cum freq for value. */
      (((long)(value-low)+1)*cum_freq[0]-1)/range;
    for (symbol = 1; cum_freq[symbol]>cum; symbol++) ; /* Then find symbol. */
    high = low +				/* Narrow the code region   */
      (range*cum_freq[symbol-1])/cum_freq[0]-1;	/* to that allotted to this */
    low = low + 				/* symbol.                  */
      (range*cum_freq[symbol])/cum_freq[0];
    for (;;) {					/* Loop to get rid of bits. */
        if (high<Half) {
            /* nothing */			/* Expand low half.         */
        } 
        else if (low>=Half) {			/* Expand high half.        */
            value -= Half;
            low -= Half;			/* Subtract offset to top.  */
            high -= Half;
        }
        else if (low>=First_qtr			/* Expand middle half.      */
              && high<Third_qtr) {
            value -= First_qtr;
            low -= First_qtr;			/* Subtract offset to middle*/
            high -= First_qtr;
        }
        else break;				/* Otherwise exit loop.     */
        low = 2*low;
        high = 2*high+1;			/* Scale up code range.     */
        value = 2*value+input_bit();		/* Move in next input bit.  */
    }
    return symbol;
}
