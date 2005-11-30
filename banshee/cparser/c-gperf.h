/* C code produced by gperf version 3.0.1 */
/* Command-line: gperf -p -j1 -i 1 -g -o -t -G -N is_reserved_word -k'1,3,$'  */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
error "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gnu-gperf@gnu.org>."
#endif


/* Command-line: gperf -p -j1 -i 1 -g -o -t -N is_reserved_word -k1,3,$ c-parse.gperf  */ 
struct resword { char *name; short token; enum rid rid; };

#define TOTAL_KEYWORDS 61
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 16
#define MIN_HASH_VALUE 7
#define MAX_HASH_VALUE 87
/* maximum key range = 81, duplicates = 0 */

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
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88,  2, 88,  5, 28, 11,
      18,  1, 22, 22, 43, 20, 88, 35, 29, 39,
      26, 32, 28, 88, 16,  1,  7, 16, 45, 33,
       3, 88,  4, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88, 88, 88, 88, 88,
      88, 88, 88, 88, 88, 88
    };
  register int hval = len;

  switch (hval)
    {
      default:
        hval += asso_values[(unsigned char)str[2]];
      /*FALLTHROUGH*/
      case 2:
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval + asso_values[(unsigned char)str[len - 1]];
}

static struct resword wordlist[] =
  {
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {"else", ELSE, NORID},
    {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {"__signed__", TYPESPEC, RID_SIGNED},
    {"__asm__", ASM_KEYWORD, NORID},
    {"case", CASE, NORID},
    {"__extension__", EXTENSION, NORID},
    {"__attribute", ATTRIBUTE, NORID},
    {"__alignof__", ALIGNOF, NORID},
    {"__typeof__", TYPEOF, NORID},
    {"__attribute__", ATTRIBUTE, NORID},
    {"static", SCSPEC, RID_STATIC},
    {"__const__", TYPE_QUAL, const_qualifier},
    {"__complex", TYPESPEC, RID_COMPLEX},
    {"__complex__", TYPESPEC, RID_COMPLEX},
    {"__const", TYPE_QUAL, const_qualifier},
    {"__real__", REALPART, NORID},
    {"__signed", TYPESPEC, RID_SIGNED},
    {"struct", STRUCT, NORID},
    {"__inline", SCSPEC, RID_INLINE},
    {"__imag__", IMAGPART, NORID},
    {"sizeof", SIZEOF, NORID},
    {"__inline__", SCSPEC, RID_INLINE},
    {"__restrict", TYPE_QUAL, restrict_qualifier},
    {"char", TYPESPEC, RID_CHAR},
    {"int", TYPESPEC, RID_INT},
    {"__alignof", ALIGNOF, NORID},
    {"__typeof", TYPEOF, NORID},
    {"extern", SCSPEC, RID_EXTERN},
    {"double", TYPESPEC, RID_DOUBLE},
    {"__label__", LABEL, NORID},
    {"unsigned", TYPESPEC, RID_UNSIGNED},
    {"if", IF, NORID},
    {"short", TYPESPEC, RID_SHORT},
    {"continue", CONTINUE, NORID},
    {"signed", TYPESPEC, RID_SIGNED},
    {"auto", SCSPEC, RID_AUTO},
    {"const", TYPE_QUAL, const_qualifier},
    {"__imag", IMAGPART, NORID},
    {"__asm", ASM_KEYWORD, NORID},
    {"do", DO, NORID},
    {"__real", REALPART, NORID},
    {"default", DEFAULT, NORID},
    {"return", RETURN, NORID},
    {"inline", SCSPEC, RID_INLINE},
    {"for", FOR, NORID},
    {"__volatile", TYPE_QUAL, volatile_qualifier},
    {"while", WHILE, NORID},
    {"enum", ENUM, NORID},
    {"__volatile__", TYPE_QUAL, volatile_qualifier},
    {"register", SCSPEC, RID_REGISTER},
    {"typeof", TYPEOF, NORID},
    {"typedef", SCSPEC, RID_TYPEDEF},
    {"goto", GOTO, NORID},
    {"float", TYPESPEC, RID_FLOAT},
    {"union", UNION, NORID},
    {"__builtin_va_arg", VA_ARG, NORID},
    {"break", BREAK, NORID},
    {"switch", SWITCH, NORID},
    {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""},
    {""},
    {"long", TYPESPEC, RID_LONG},
    {""},
    {"volatile", TYPE_QUAL, volatile_qualifier},
    {""}, {""},
    {"asm", ASM_KEYWORD, NORID},
    {"void", TYPESPEC, RID_VOID}
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
