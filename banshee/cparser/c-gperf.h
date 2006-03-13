/* C code produced by gperf version 2.7.2 */
/* Command-line: gperf -p -j1 -i 1 -g -o -t -G -N is_reserved_word -k'1,3,$'  */
/* Command-line: gperf -p -j1 -i 1 -g -o -t -N is_reserved_word -k1,3,$ c-parse.gperf  */ 
struct resword { char *name; short token; enum rid rid; };

#define TOTAL_KEYWORDS 61
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 16
#define MIN_HASH_VALUE 5
#define MAX_HASH_VALUE 106
/* maximum key range = 102, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static unsigned char asso_values[] =
    {
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107,   1, 107,  16,   2,  31,
        6,   6,   1,  17,   5,   2, 107,   3,  41,  10,
       37,  51,   1, 107,  12,  49,   1,  23,  34,   5,
        6, 107,   1, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107, 107, 107, 107, 107,
      107, 107, 107, 107, 107, 107
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static struct resword wordlist[] =
  {
    {""}, {""}, {""}, {""}, {""},
    {"if", IF, NORID},
    {""},
    {"int", TYPESPEC, RID_INT},
    {""},
    {"typeof", TYPEOF, NORID},
    {"typedef", SCSPEC, RID_TYPEDEF},
    {"__typeof", TYPEOF, NORID},
    {"__imag__", IMAGPART, NORID},
    {"__typeof__", TYPEOF, NORID},
    {"__inline__", SCSPEC, RID_INLINE},
    {"default", DEFAULT, NORID},
    {"break", BREAK, NORID},
    {"__inline", SCSPEC, RID_INLINE},
    {"while", WHILE, NORID},
    {""}, {""},
    {"__extension__", EXTENSION, NORID},
    {"__real__", REALPART, NORID},
    {""},
    {"__restrict", TYPE_QUAL, restrict_qualifier},
    {"__asm__", ASM_KEYWORD, NORID},
    {"__imag", IMAGPART, NORID},
    {"__alignof", ALIGNOF, NORID},
    {"for", FOR, NORID},
    {"__alignof__", ALIGNOF, NORID},
    {""},
    {"__attribute__", ATTRIBUTE, NORID},
    {"__asm", ASM_KEYWORD, NORID},
    {""},
    {"__attribute", ATTRIBUTE, NORID},
    {""},
    {"__builtin_va_arg", VA_ARG, NORID},
    {""}, {""},
    {"asm", ASM_KEYWORD, NORID},
    {"__const", TYPE_QUAL, const_qualifier},
    {"double", TYPESPEC, RID_DOUBLE},
    {"__const__", TYPE_QUAL, const_qualifier},
    {"enum", ENUM, NORID},
    {"__complex__", TYPESPEC, RID_COMPLEX},
    {""},
    {"void", TYPESPEC, RID_VOID},
    {"__complex", TYPESPEC, RID_COMPLEX},
    {"__volatile__", TYPE_QUAL, volatile_qualifier},
    {"register", SCSPEC, RID_REGISTER},
    {"extern", SCSPEC, RID_EXTERN},
    {"__volatile", TYPE_QUAL, volatile_qualifier},
    {"__label__", LABEL, NORID},
    {""}, {""},
    {"inline", SCSPEC, RID_INLINE},
    {"return", RETURN, NORID},
    {"sizeof", SIZEOF, NORID},
    {"float", TYPESPEC, RID_FLOAT},
    {"do", DO, NORID},
    {"__real", REALPART, NORID},
    {"__signed__", TYPESPEC, RID_SIGNED},
    {"switch", SWITCH, NORID},
    {"char", TYPESPEC, RID_CHAR},
    {"__signed", TYPESPEC, RID_SIGNED},
    {"else", ELSE, NORID},
    {""},
    {"union", UNION, NORID},
    {"struct", STRUCT, NORID},
    {""}, {""}, {""},
    {"auto", SCSPEC, RID_AUTO},
    {"goto", GOTO, NORID},
    {"const", TYPE_QUAL, const_qualifier},
    {""}, {""}, {""},
    {"signed", TYPESPEC, RID_SIGNED},
    {""}, {""}, {""},
    {"continue", CONTINUE, NORID},
    {""}, {""}, {""},
    {"unsigned", TYPESPEC, RID_UNSIGNED},
    {""}, {""},
    {"volatile", TYPE_QUAL, volatile_qualifier},
    {"case", CASE, NORID},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {"long", TYPESPEC, RID_LONG},
    {""}, {""},
    {"static", SCSPEC, RID_STATIC},
    {""}, {""}, {""},
    {"short", TYPESPEC, RID_SHORT}
  };

#ifdef __GNUC__
__inline
#endif
struct resword *
is_reserved_word (str, len)
     register const char *str;
     register unsigned int len;
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
