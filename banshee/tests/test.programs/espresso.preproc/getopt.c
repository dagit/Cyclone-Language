typedef int size_t;
extern	struct	_iobuf {
int	_cnt;
unsigned char *_ptr;
unsigned char *_base;
int	_bufsiz;
short	_flag;
char	_file;	} _iob[];
typedef struct _iobuf FILE;
extern struct _iobuf	*fopen(const char *, const char *);
extern struct _iobuf	*fdopen(int, const char *);
extern struct _iobuf	*freopen(const char *, const char *, FILE *);
extern struct _iobuf	*popen(const char *, const char *);
extern struct _iobuf	*tmpfile(void);
extern long	ftell(FILE *);
extern char	*fgets(char *, int, FILE *);
extern char	*gets(char *);
extern char	*sprintf(char *, const char *, ...);
extern char	*ctermid(char *);
extern char	*cuserid(char *);
extern char	*tempnam(const char *, const char *);
extern char	*tmpnam(char *);
extern	char	_ctype_[];
extern struct _iobuf *popen(const char *, const char *), *tmpfile(void);
extern int pclose(FILE *);
extern void rewind(FILE *);
extern void abort(void), free(void *), exit(int), perror(const char *);
extern char *getenv(const char *), *malloc(size_t), *realloc(void *, size_t);
extern int system(const char *);
extern double atof(const char *);
extern char *strcpy(char *, const char *), *strncpy(char *, const char *, size_t), *strcat(char *, const char *), *strncat(char *, const char *, size_t), *strerror(int);
extern char *strpbrk(const char *, const char *), *strtok(char *, const char *), *strchr(const char *, int), *strrchr(const char *, int), *strstr(const char *, const char *);
extern int strcoll(const char *, const char *), strxfrm(char *, const char *, size_t), strncmp(const char *, const char *, size_t), strlen(const char *), strspn(const char *, const char *), strcspn(const char *, const char *);
extern char *memmove(void *, const void *, size_t), *memccpy(void *, const void *, int, size_t), *memchr(const void *, int, size_t), *memcpy(void *, const void *, size_t), *memset(void *, int, size_t);
extern int memcmp(const void *, const void *, size_t), strcmp(const char *, const char *);
extern long util_cpu_time();
extern int util_getopt(int argc, char **argv, char *optstring);
extern void util_getopt_reset(void);
extern char *util_path_search();
extern char *util_file_search();
extern int util_pipefork();
extern void util_print_cpu_stats();
extern char *util_print_time();
extern int util_save_image();
extern char *util_strsav();
extern char *util_tilde_expand();
extern void util_restart();
extern int util_optind;
extern char *util_optarg;
char *util_optarg;	int util_optind = 0;	static char *scan;
void
util_getopt_reset(void)
{
util_optarg = 0;
util_optind = 0;
scan = 0;
}
int util_getopt(int argc, char **argv, char *optstring)
{
register int c;
register char *place;
extern char *strchr(const char *, int);
util_optarg = ((char *) 0);
if (scan == ((char *) 0) || *scan == '\0') {
if (util_optind == 0) util_optind++;
if (util_optind >= argc) return (-1);
place = argv[util_optind];
if (place[0] != '-' || place[1] == '\0') return (-1);
util_optind++;
if (place[1] == '-' && place[2] == '\0') return (-1);
scan = place+1;
}
c = *scan++;
place = strchr(optstring, c);
if (place == ((char *) 0) || c == ':') {
(void) fprintf((&_iob[2]), "%s: unknown option %c\n", argv[0], c);
return '?';
}
if (*++place == ':') {
if (*scan != '\0') {
util_optarg = scan;
scan = ((char *) 0);
} else {
if (util_optind >= argc) {
(void) fprintf((&_iob[2]), "%s: %c requires an argument\n", argv[0], c);
return '?';
}
util_optarg = argv[util_optind];
util_optind++;
}
}
return c;
}
