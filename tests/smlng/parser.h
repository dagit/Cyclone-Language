#ifndef _PARSER_H_
#define _PARSER_H_
#include "smlng.h"
#include "buffer.h"
#include <cstdio.h>
namespace Parser {
using Buffer;
using Std;
extern doc_t parse_rec(context_t, buf_t b, FILE @f);
extern doc_t parse_iter(context_t, buf_t b, FILE @f);
}
#endif
