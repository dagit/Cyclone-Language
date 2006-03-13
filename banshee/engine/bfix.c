/*
 * File Name:
 *    bfix.c
 *
 * Prototypes:
 *    void              bfi(unsigned char *, unsigned long, unsigned long, long);
 *    unsigned long     bfx(const unsigned char *, unsigned long, unsigned long);
 *
 * Description:
 *    These routines are used to insert and extract bit fields from an
 *    array of characters pointed to by an unsigned char* pointer.
 *
 * Usage:
 *    Bit field insert:
 *       bfi(cptr, bit_offset, bit_len, value);
 *          cptr       - pointer to unsigned char array
 *          bit_offset - bit offset(starting from 1) from the start of the char array
 *          bit_len    - bit length of field to insert
 *          value      - value to be inserted
 *
 *    Bit field extract:
 *       l = bfx(cptr, bit_offset, bit_len);
 *          cptr       - pointer to unsigned char array
 *          bit_offset - bit offset(starting from 1) from the start of the char array
 *          bit_len    - bit length of field to extract
 *
 *    Insert bit fields:
 *       unsigned char cp[2];
 *
 *       bfi(cp, 1, 1, 0);     set bit 1 to 0(b0)
 *       bfi(cp, 2, 2, 3);     set bits 2-3 to 3(b11)
 *       bfi(cp, 4, 4, 8);     set bits 4-7 to 8(b1000)
 *       bfi(cp, 8, 2, 1);     set bits 8-9 to 1(b01)
 *       bfi(cp, 10, 3, 0x7);  set bits 10-12 to 7(b111)
 *       bfi(cp, 13, 4, 0xf);  set bits 13-16 to 0xf(b1111)
 *
 *       cp[2] will be set to 0x70ff
 *
 *    Extract bit fields:
 *
 *       l1 = bfx(cp, 1, 2);   sets l1 to 1
 *       l2 = bfx(cp, 3, 1);   sets l2 to 1
 *       l3 = bfx(cp, 4, 6);   sets l3 to 33
 *       l4 = bfx(cp, 10, 4);  sets l4 to 15
 *       l5 = bfx(cp, 14, 3);  sets l5 to 7
 *
 * Notes:
 *    a. normally, no checking for reasonable argument values is done.  if DEBUG
 *       is defined then reasonable values for bit_offset and bit_len are checked.
 *       a reasonable value for bit_len is dependent on the starting bit_offset value
 *       as explained in note b.  see the code for DEBUG inclusions.
 *    b. bit_len should be <=32 to 25. it depends on the value of bit_offset.  the
 *       method always uses a memmove() of 4 bytes to a long temporary storage in
 *       which logical operations can be performed.  this means that the bit_len can
 *       be at most 4 bytes, but in a case in which the start bit is not at the
 *       beginning of a byte, then the bit_len can only extend until the end of the
 *       fourth byte.  if the start bit is the last bit of a byte this will limit
 *       bit_len to 25 bits - the last bit of the first byte plus the next three bytes.
 *    c. and bit_offset+bit_len should not overrun the array.
 *    d. value should not be too long to fit into the bit field.  if it is, the high
 *       order bits in front of the low order bit_len bits will be truncated.
 *    e. all bit_len bits will be set and no bit outside the bit field will be changed.
 *    f. value may be negative and prefix 2,s complement sign bits are truncated to fit
 *       into bit_len bits.
 *    g. 4 bytes are always read from the unsigned char array, modified and then
 *       written back.  this means that if you set the last bit of the array, then the
 *       next three bytes will be read and written back, thus seemingly overrunning the
 *       array.  this may or may not be a problem.  if bit_offset+bit_len does not
 *       overrun the array then no bits beyond the end of the array will be changed.
 *       however, if writing 3 bytes beyond the end of the array, even if no change is
 *       made, is a potential problem then some provision must be made to deal with
 *       this possibility.  the array could be padded by 3 extra bytes.
 *
 * Author:
 *    Dick Hogaboom
 *
 */

#include <stdio.h>
#include <string.h>

#include "bfix.h"

/* machine dependencies */
#define BITS_PER_BYTE                    8
#define BYTES_PER_LONG                   4
#define BITS_PER_LONG                    32



/*
 * bit field insertion
 */

#ifdef DEBUG
   int
#else
   void
#endif
bfi(unsigned char *cptr, unsigned long bit_offset, unsigned long bit_len, long value)
{
   unsigned long l;
   unsigned long mask = 0xffffffff;
   unsigned long byte_offset;
   unsigned long pre_shift;
   unsigned long post_shift;

   #ifdef DEBUG
      if ( bit_offset < 1 )
      {
         printf("bfi: arg #2, bit_offset = %d is < 1.\n", bit_offset);
         return -1;
      }

      if ( bit_len < 1 )
      {
         printf("bfi: arg #3, bit_len = %d is < 1.\n", bit_len);
         return -1;
      }
   #endif

   /* calculate byte offset(first byte=0) of start of BYTES_PER_LONG bytes containing bit field */
   byte_offset = (bit_offset-1)/BITS_PER_BYTE;

   /* calculate how many bits to shift bit field left to clear bits above bit field */
   pre_shift = bit_offset - byte_offset*BITS_PER_BYTE - 1;

   #ifdef DEBUG
      if ( bit_len > (BITS_PER_LONG-pre_shift) )
      {
         printf("bfi: arg #3, bit_len = %d to long.\n", bit_len);
         return -1;
      }
   #endif

   /* calculate how many bits to shift bit field left to clear bits below bit field */
   post_shift = BITS_PER_LONG - bit_len - pre_shift;

   /* move bit field into position over bits to set in l corresponding to correct bits in cptr */
   value <<= post_shift;

   /* zero out mask bits after bit field */
   mask <<= post_shift;

   /* zero out mask bits before bit field */
   mask <<= pre_shift;
   mask >>= pre_shift;

   /* zero out value bits before and after bit field */
   value &= mask;

   /* move BYTES_PER_LONG bytes to tmp storage */
   memmove((unsigned char *)&l, &cptr[byte_offset], BYTES_PER_LONG);

   /* zero out bit field bits and then or value bits into them */
   l = (l & (~mask)) | value;

   /* move tmp storage back to cptr array */
   memmove(&cptr[byte_offset], (unsigned char *)&l, BYTES_PER_LONG);

   #ifdef DEBUG
      return 0;
   #endif
}


/*
 * bit field extraction
 */

   unsigned long
bfx(const unsigned char *cptr, unsigned long bit_offset, unsigned long bit_len)
{
   unsigned long l;
   unsigned long byte_offset;
   unsigned long left_shift;
   unsigned long right_shift;

   #ifdef DEBUG
      if ( bit_offset < 1 )
      {
         printf("bfx: arg #2, bit_offset < 1.\n");
         return -1;
      }

      if ( bit_len < 1 )
      {
         printf("bfx: arg #3, bit_len < 1.\n");
         return -1;
      }
   #endif

   /* calculate byte offset(first byte=0) of start of BYTES_PER_LONG bytes containing bit field */
   byte_offset = (bit_offset-1)/BITS_PER_BYTE;

   /* calculate how many bits to shift bit field left to clear bits above bit field */
   left_shift = bit_offset - byte_offset*BITS_PER_BYTE - 1;

   #ifdef DEBUG
      if ( bit_len > (BITS_PER_LONG-left_shift) )
      {
         printf("bfx: arg #3, bit_len to long.\n");
         return -1;
      }
   #endif

   /* calculate how many bits to shift bit field right to right justify bit field */
   right_shift = BITS_PER_LONG - bit_len;

   /* move BYTES_PER_LONG bytes to tmp storage */
   memmove((unsigned char *)&l, &cptr[byte_offset], BYTES_PER_LONG);

   /* clear bits above bit field, right justify bit field, and return this value */
   return ( l << left_shift ) >> right_shift;
}

