#ifndef _PARSER_H_
#define _PARSER_H_
#include "smlng.h"
#include "buffer.h"
#include <stdio.h>
namespace Parser {
using Buffer;
using Stdio;
extern doc_t get_docs(context_t, buf_t b, FILE @f);
}
#endif
