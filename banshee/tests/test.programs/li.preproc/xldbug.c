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
extern long total;
extern int xldebug;
extern int xltrace;
extern int xlsample;
extern NODE *s_unbound;
extern NODE *s_stdin,*s_stdout;
extern NODE *s_tracenable,*s_tlimit,*s_breakenable;
extern NODE ***xlstack;
extern NODE *true;
extern NODE **trace_stack;
extern char buf[];
extern char *malloc(size_t);
static NODE *stacktop();
xlfail(char *emsg)
{
xlerror(emsg,stacktop());
}
xlabort(char *emsg)
{
xlsignal(emsg,s_unbound);
}
xlbreak(char *emsg, NODE *arg)
{
breakloop("break",0,emsg,arg,1);
}
xlerror(char *emsg, NODE *arg)
{
doerror(0,emsg,arg,0);
}
xlcerror(char *cmsg, char *emsg, NODE *arg)
{
doerror(cmsg,emsg,arg,1);
}
xlerrprint(char *hdr, char *cmsg, char *emsg, NODE *arg)
{
sprintf(buf,"%s: %s",hdr,emsg); stdputstr(buf);
if (arg != s_unbound) { stdputstr(" - "); stdprint(arg); }
else xlterpri(((s_stdout)->n_info.n_xsym.xsy_value));
if (cmsg) { sprintf(buf,"if continued: %s\n",cmsg); stdputstr(buf); }
}
static doerror(char *cmsg, char *emsg, NODE *arg, int cflag)
{
if (((s_breakenable)->n_info.n_xsym.xsy_value) == (NODE *)0)
xlsignal(emsg,arg);
breakloop("error",cmsg,emsg,arg,cflag);
}
static int breakloop(char *hdr, char *cmsg, char *emsg, NODE *arg, int cflag)
{
NODE ***oldstk,*expr,*val;
CONTEXT cntxt;
int type;
xlerrprint(hdr,cmsg,emsg,arg);
xlflush();
if (((s_tracenable)->n_info.n_xsym.xsy_value)) {
val = ((s_tlimit)->n_info.n_xsym.xsy_value);
xlbaktrace(((val) && (val)->n_type == 5) ? (int)((val)->n_info.n_xint.xi_int) : -1);
}
oldstk = xlsave(&expr,(NODE **)0);
xldebug++;
xlbegin(&cntxt,8|16|32,true);
for (type = 0; type == 0; ) {
if (type = setjmp(cntxt.c_jmpbuf))
switch (type) {
case 8:
xlflush();
type = 0;
continue;
case 16:
continue;
case 32:
if (cflag) {
stdputstr("[ continue from break loop ]\n");
continue;
}
else xlabort("this error can't be continued");
}
if (!xlread(((s_stdin)->n_info.n_xsym.xsy_value),&expr,0)) {
type = 16;
break;
}
expr = xleval(expr);
xlprint(((s_stdout)->n_info.n_xsym.xsy_value),expr,1);
xlterpri(((s_stdout)->n_info.n_xsym.xsy_value));
}
xlend(&cntxt);
xldebug--;
xlstack = oldstk;
if (type == 16) {
stdputstr("[ abort to previous level ]\n");
xlsignal(0,(NODE *)0);
}
}
static NODE *stacktop(void)
{
return (xltrace >= 0 && xltrace < 500 ? trace_stack[xltrace] : s_unbound);
}
xlbaktrace(int n)
{
int i;
for (i = xltrace; (n < 0 || n--) && i >= 0; i--)
if (i < 500)
stdprint(trace_stack[i]);
}
xldinit(void)
{
if ((trace_stack = (NODE **)malloc(500 * sizeof(NODE *))) == 0) {
printf("insufficient memory");
osfinish();
exit(1);
}
total += (long)(500 * sizeof(NODE *));
xlsample = 0;
xltrace = -1;
xldebug = 0;
}
