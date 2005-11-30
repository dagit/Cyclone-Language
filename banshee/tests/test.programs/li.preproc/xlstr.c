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
typedef	int jmp_buf[9];
typedef	int sigjmp_buf[9+1];
int	setjmp(jmp_buf);
int	_setjmp(jmp_buf);
int	sigsetjmp(sigjmp_buf, int);
void	longjmp(jmp_buf, int);
void	_longjmp(jmp_buf, int);
void	siglongjmp(sigjmp_buf, int);
typedef struct node {
char n_type;	char n_flags;	union {	struct xsym {	struct node *xsy_plist;	struct node *xsy_value;	} n_xsym;
struct xsubr {	struct node *(*xsu_subr)();	} n_xsubr;
struct xlist {	struct node *xl_car;	struct node *xl_cdr;	} n_xlist;
struct xint {	long xi_int;	} n_xint;
struct xfloat {	float xf_float;	} n_xfloat;
struct xstr {	int xst_type;	char *xst_str;	} n_xstr;
struct xfptr {	struct _iobuf *xf_fp;	int xf_savech;	} n_xfptr;
struct xvect {	int xv_size;	struct node **xv_data;	} n_xvect;
} n_info;
} NODE;
typedef struct context {
int c_flags;	struct node *c_expr;	jmp_buf c_jmpbuf;	struct context *c_xlcontext;	struct node ***c_xlstack;	struct node *c_xlenv;	int c_xltrace;	} CONTEXT;
struct fdef {
char *f_name;	int f_type;	struct node *(*f_fcn)();	};
struct segment {
int sg_size;
struct segment *sg_next;
struct node sg_nodes[1];
};
extern struct node ***xlsave(NODE ** arg1, ...);	extern struct node *xleval();	extern struct node *xlapply();	extern struct node *xlevlist();	extern struct node *xlarg();	extern struct node *xlevarg();	extern struct node *xlmatch();	extern struct node *xlevmatch();	extern struct node *xlgetfile();	extern struct node *xlsend();	extern struct node *xlenter();	extern struct node *xlsenter();	extern struct node *xlmakesym();	extern struct node *xlframe();	extern struct node *xlgetvalue();	extern struct node *xlxgetvalue();	extern struct node *xlygetvalue();	extern struct node *cons();	extern struct node *consa();	extern struct node *consd();	extern struct node *cvsymbol();	extern struct node *cvcsymbol();	extern struct node *cvstring();	extern struct node *cvcstring();	extern struct node *cvfile();	extern struct node *cvsubr();	extern struct node *cvfixnum();	extern struct node *cvflonum();	extern struct node *newstring();	extern struct node *newvector();	extern struct node *newobject();	extern struct node *xlgetprop();	extern char *xlsymname();	extern void xlsetvalue();
extern void xlprint();
extern void xltest();
extern NODE ***xlstack;
extern char buf[];
extern char *strcat(char *, const char *);
NODE *xstrcat(NODE *args)
{
NODE ***oldstk,*val,*p;
char *str;
int len;
oldstk = xlsave(&val,(NODE **)0);
for (p = args, len = 0; p; )
len += strlen(((xlmatch(6,&p))->n_info.n_xstr.xst_str));
val = newstring(len);
str = ((val)->n_info.n_xstr.xst_str);
*str = 0;
while (args)
strcat(str,((xlmatch(6,&args))->n_info.n_xstr.xst_str));
xlstack = oldstk;
return (val);
}
NODE *xsubstr(NODE *args)
{
NODE ***oldstk,*arg,*src,*val;
int start,forlen,srclen;
char *srcptr,*dstptr;
oldstk = xlsave(&arg,&src,&val,(NODE **)0);
arg = args;
src = xlmatch(6,&arg);
srcptr = ((src)->n_info.n_xstr.xst_str);
srclen = strlen(srcptr);
start = ((xlmatch(5,&arg))->n_info.n_xint.xi_int);
forlen = (arg ? ((xlmatch(5,&arg))->n_info.n_xint.xi_int) : srclen);
xllastarg(arg);
if (start + forlen > srclen)
forlen = srclen - start + 1;
if (start > srclen) {
start = 1;
forlen = 0; }
val = newstring(forlen);
dstptr = ((val)->n_info.n_xstr.xst_str);
for (srcptr += start-1; forlen--; *dstptr++ = *srcptr++)
;
*dstptr = 0;
xlstack = oldstk;
return (val);
}
NODE *xstring(NODE *args)
{
buf[0] = ((xlmatch(5,&args))->n_info.n_xint.xi_int);
xllastarg(args);
buf[1] = 0;
return (cvstring(buf));
}
NODE *xchar(NODE *args)
{
char *str;
int n;
str = ((xlmatch(6,&args))->n_info.n_xstr.xst_str);
n = ((xlmatch(5,&args))->n_info.n_xint.xi_int);
xllastarg(args);
if (n < 0 || n >= strlen(str))
xlerror("index out of range",cvfixnum((long)n));
return (cvfixnum((long)str[n]));
}
