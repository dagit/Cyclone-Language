/* BIT OUTPUT ROUTINES. */

#include <stdio.h>


/* THE BIT BUFFER. */

static int buffer;		/* Bits buffered for output                 */
static int bits_to_go;		/* Number of bits free in buffer            */


/* INITIALIZE FOR BIT OUTPUT. */

start_outputing_bits()
{   buffer = 0;					/* Buffer is empty to start */
    bits_to_go= 8;				/* with.                    */
}


/* OUTPUT A BIT. */

output_bit(bit)
    int bit;
{   buffer >>= 1; if (bit) buffer |= 0x80;	/* Put bit in top of buffer.*/
    bits_to_go -= 1;
    if (bits_to_go==0) {			/* Output buffer if it is   */
        putc(buffer,stdout);			/* now full.                */
        bits_to_go = 8;
    }
}


/* FLUSH OUT THE LAST BITS. */

done_outputing_bits()
{   putc(buffer>>bits_to_go,stdout);
}
