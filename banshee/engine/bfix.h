/*
 * File Name:
 *    bfix.h
 *
 * Description:
 *    header file for bfix.c
 *
 * Author:
 *    Dick Hogaboom
 */

#ifndef _BFIX_H
#define _BFIX_H

#ifdef DEBUG
   int
#else
   void
#endif
bfi(unsigned char *cptr, unsigned long bit_offset, unsigned long bit_len, long value);
unsigned long bfx(const unsigned char *cptr, unsigned long bit_offset, unsigned long bit_len);

#endif /* _BFIX_H */

