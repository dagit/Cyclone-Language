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
extern CONTEXT *xlcontext;
extern NODE *xlvalue;
extern NODE ***xlstack,*xlenv;
extern int xltrace,xldebug;
xlbegin(CONTEXT *cptr, int flags, NODE *expr)
{
cptr->c_flags = flags;
cptr->c_expr = expr;
cptr->c_xlstack = xlstack;
cptr->c_xlenv = xlenv;
cptr->c_xltrace = xltrace;
cptr->c_xlcontext = xlcontext;
xlcontext = cptr;
}
xlend(CONTEXT *cptr)
{
xlcontext = cptr->c_xlcontext;
}
xljump(CONTEXT *cptr, int type, NODE *val)
{
xlcontext = cptr;
xlstack = xlcontext->c_xlstack;
xlenv = xlcontext->c_xlenv;
xltrace = xlcontext->c_xltrace;
xlvalue = val;
longjmp(xlcontext->c_jmpbuf,type);
}
xltoplevel(void)
{
findtarget(64,"no top level");
}
xlcleanup(void)
{
findtarget(16,"not in a break loop");
}
xlcontinue(void)
{
findtarget(32,"not in a break loop");
}
xlgo(NODE *label)
{
CONTEXT *cptr;
NODE *p;
for (cptr = xlcontext; cptr; cptr = cptr->c_xlcontext)
if (cptr->c_flags & 1)
for (p = cptr->c_expr; ((p) && (p)->n_type == 3); p = ((p)->n_info.n_xlist.xl_cdr))
if (((p)->n_info.n_xlist.xl_car) == label)
xljump(cptr,1,p);
xlfail("no target for GO");
}
xlreturn(NODE *val)
{
CONTEXT *cptr;
for (cptr = xlcontext; cptr; cptr = cptr->c_xlcontext)
if (cptr->c_flags & 2)
xljump(cptr,2,val);
xlfail("no target for RETURN");
}
xlthrow(NODE *tag, NODE *val)
{
CONTEXT *cptr;
for (cptr = xlcontext; cptr; cptr = cptr->c_xlcontext)
if ((cptr->c_flags & 4) && cptr->c_expr == tag)
xljump(cptr,4,val);
xlfail("no target for THROW");
}
xlsignal(char *emsg, NODE *arg)
{
CONTEXT *cptr;
for (cptr = xlcontext; cptr; cptr = cptr->c_xlcontext)
if (cptr->c_flags & 8) {
if (cptr->c_expr && emsg)
xlerrprint("error",0,emsg,arg);
xljump(cptr,8,(NODE *)0);
}
xlfail("no target for error");
}
static findtarget(int flag, char *error)
{
CONTEXT *cptr;
for (cptr = xlcontext; cptr; cptr = cptr->c_xlcontext)
if (cptr->c_flags & flag)
xljump(cptr,flag,(NODE *)0);
xlabort(error);
}
