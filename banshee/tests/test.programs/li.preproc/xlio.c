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
extern NODE *s_stdin,*s_unbound;
extern int xlfsize;
extern int xlplevel;
extern int xldebug;
extern int prompt;
extern char buf[];
int xlgetc(NODE *fptr)
{
NODE *lptr,*cptr;
struct _iobuf *fp;
int ch;
if (fptr == (NODE *)0)
ch = (-1);
else if (((fptr) && (fptr)->n_type == 3)) {
if ((lptr = ((fptr)->n_info.n_xlist.xl_car)) == (NODE *)0)
ch = (-1);
else {
if (!((lptr) && (lptr)->n_type == 3) ||
(cptr = ((lptr)->n_info.n_xlist.xl_car)) == (NODE *)0 || !((cptr) && (cptr)->n_type == 5))
xlfail("bad stream");
if (((fptr)->n_info.n_xlist.xl_car = (((lptr)->n_info.n_xlist.xl_cdr))) == (NODE *)0)
((fptr)->n_info.n_xlist.xl_cdr = ((NODE *)0));
ch = ((cptr)->n_info.n_xint.xi_int);
}
}
else if (ch = ((fptr)->n_info.n_xfptr.xf_savech))
((fptr)->n_info.n_xfptr.xf_savech = (0));
else {
fp = ((fptr)->n_info.n_xfptr.xf_fp);
if (prompt && fp == (&_iob[0])) {
if (xldebug)
{ sprintf(buf,"%d:",xldebug); stdputstr(buf); }
if (xlplevel > 0)
{ sprintf(buf,"%d",xlplevel); stdputstr(buf); }
stdputstr("> ");
prompt = 0;
}
if (((ch = osgetc(fp)) == '\n' || ch == (-1)) && fp == (&_iob[0]))
prompt = 1;
}
return (ch);
}
int xlpeek(NODE *fptr)
{
NODE *lptr,*cptr;
int ch;
if (fptr == (NODE *)0)
ch = (-1);
else if (((fptr) && (fptr)->n_type == 3)) {
if ((lptr = ((fptr)->n_info.n_xlist.xl_car)) == (NODE *)0)
ch = (-1);
else {
if (!((lptr) && (lptr)->n_type == 3) ||
(cptr = ((lptr)->n_info.n_xlist.xl_car)) == (NODE *)0 || !((cptr) && (cptr)->n_type == 5))
xlfail("bad stream");
ch = ((cptr)->n_info.n_xint.xi_int);
}
}
else
((fptr)->n_info.n_xfptr.xf_savech = (ch = xlgetc(fptr)));
return (ch);
}
xlputc(NODE *fptr, int ch)
{
NODE ***oldstk,*lptr;
xlfsize++;
if (fptr == (NODE *)0)
;
else if (((fptr) && (fptr)->n_type == 3)) {
oldstk = xlsave(&lptr,(NODE **)0);
lptr = consa((NODE *)0);
((lptr)->n_info.n_xlist.xl_car = (cvfixnum((long)ch)));
if (((fptr)->n_info.n_xlist.xl_cdr))
((((fptr)->n_info.n_xlist.xl_cdr))->n_info.n_xlist.xl_cdr = (lptr));
else
((fptr)->n_info.n_xlist.xl_car = (lptr));
((fptr)->n_info.n_xlist.xl_cdr = (lptr));
xlstack = oldstk;
}
else
osputc(ch,((fptr)->n_info.n_xfptr.xf_fp));
}
int xlflush(void)
{
if (!prompt)
while (xlgetc(((s_stdin)->n_info.n_xsym.xsy_value)) != '\n')
;
}
