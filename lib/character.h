#ifndef CHARACTER_H
#define CHARACTER_H 
namespace Character {

extern bool isupper(char c);
extern bool islower(char c);
extern bool isdigit(char c);
extern bool isxdigit(char c);
extern bool isspace(char c);
extern bool isalpha(char c);
extern bool isprint(char c);
extern bool isgraph(char c);

extern char toupper(char c);
extern char tolower(char c);

}

#endif
