#ifndef _BUFFER_H_
#define _BUFFER_H_
#include <stdio.h>
namespace Buffer {
using Stdio;

// abstract buffers for storing text
extern struct Buf;
typedef struct Buf @buf_t;
// initial buffer
extern buf_t  init_buf();

// get current position within buffer
extern unsigned int position(buf_t);

// return the size of the string at offset i in buffer b
extern unsigned int size(buf_t b, unsigned int pos);

// add a character to the buffer
extern void   add_char(buf_t b, char c);

// get the character at position i
extern char   get_char(buf_t b, unsigned int pos);

// dumps the string at offset start to the FILE f
extern void   write_buf(FILE @f, buf_t b, unsigned int start);
}
#endif _BUFFER_H_
