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
extern struct node ***xlsave(NODE ** arg1, ...);	extern struct node *xleval();	extern struct node *xlapply();	extern struct node *xlevlist();	extern struct node *xlarg(NODE **pargs);	extern struct node *xlevarg(NODE **pargs);	extern struct node *xlmatch(int type, NODE **pargs);	extern struct node *xlevmatch(int type, NODE **pargs);	extern struct node *xlgetfile(NODE **pargs);	extern struct node *xlsend();	extern struct node *xlenter();	extern struct node *xlsenter();	extern struct node *xlmakesym();	extern struct node *xlframe();	extern struct node *xlgetvalue();	extern struct node *xlxgetvalue();	extern struct node *xlygetvalue();	extern struct node *cons();	extern struct node *consa();	extern struct node *consd();	extern struct node *cvsymbol();	extern struct node *cvcsymbol();	extern struct node *cvstring();	extern struct node *cvcstring();	extern struct node *cvfile();	extern struct node *cvsubr();	extern struct node *cvfixnum();	extern struct node *cvflonum();	extern struct node *newstring();	extern struct node *newvector();	extern struct node *newobject();	extern struct node *xlgetprop();	extern char *xlsymname();	extern void xlsetvalue();
extern void xlprint();
extern void xltest(NODE **pfcn, int *ptresult, NODE **pargs);
extern NODE *k_test,*k_tnot,*s_eql;
extern NODE ***xlstack;
xlsubr(char *sname, int type, NODE *(*subr) ())
{
NODE *sym;
sym = xlsenter(sname);
((sym)->n_info.n_xsym.xsy_value = (cvsubr(subr,type)));
}
NODE *xlarg(NODE **pargs)
{
NODE *arg;
if (!((*pargs) && (*pargs)->n_type == 3))
xlfail("too few arguments");
arg = ((*pargs)->n_info.n_xlist.xl_car);
*pargs = ((*pargs)->n_info.n_xlist.xl_cdr);
return (arg);
}
NODE *xlmatch(int type, NODE **pargs)
{
NODE *arg;
arg = xlarg(pargs);
if (type == 3) {
if (arg && ((arg)->n_type) != 3)
xlerror("bad argument type",arg);
}
else {
if (arg == (NODE *)0 || ((arg)->n_type) != type)
xlerror("bad argument type",arg);
}
return (arg);
}
NODE *xlevarg(NODE **pargs)
{
NODE ***oldstk,*val;
oldstk = xlsave(&val,(NODE **)0);
val = xlarg(pargs);
val = xleval(val);
xlstack = oldstk;
return (val);
}
NODE *xlevmatch(int type, NODE **pargs)
{
NODE *arg;
arg = xlevarg(pargs);
if (type == 3) {
if (arg && ((arg)->n_type) != 3)
xlerror("bad argument type",arg);
}
else {
if (arg == (NODE *)0 || ((arg)->n_type) != type)
xlerror("bad argument type",arg);
}
return (arg);
}
void xltest(NODE **pfcn, int *ptresult, NODE **pargs)
{
NODE *arg;
if (!((*pargs) && (*pargs)->n_type == 3)) {
*pfcn = ((s_eql)->n_info.n_xsym.xsy_value);
*ptresult = 1;
return;
}
arg = ((*pargs)->n_info.n_xlist.xl_car);
if (arg == k_test)
*ptresult = 1;
else if (arg == k_tnot)
*ptresult = 0;
else
xlfail("expecting :test or :test-not");
*pargs = ((*pargs)->n_info.n_xlist.xl_cdr);
if (!((*pargs) && (*pargs)->n_type == 3))
xlfail("no value for keyword argument");
*pfcn = ((*pargs)->n_info.n_xlist.xl_car);
if (((*pfcn) && (*pfcn)->n_type == 4))
*pfcn = xleval(*pfcn);
*pargs = ((*pargs)->n_info.n_xlist.xl_cdr);
}
NODE *xlgetfile(NODE **pargs)
{
NODE *arg;
if (arg = xlarg(pargs)) {
if (((arg) && (arg)->n_type == 8)) {
if (arg->n_info.n_xfptr.xf_fp == 0)
xlfail("file not open");
}
else if (!((arg) && (arg)->n_type == 3))
xlerror("bad argument type",arg);
}
return (arg);
}
xllastarg(NODE *args)
{
if (args)
xlfail("too many arguments");
}
int eq(NODE *arg1, NODE *arg2)
{
return (arg1 == arg2);
}
int eql(NODE *arg1, NODE *arg2)
{
if (eq(arg1,arg2))
return (1);
else if (((arg1) && (arg1)->n_type == 5) && ((arg2) && (arg2)->n_type == 5))
return (arg1->n_info.n_xint.xi_int == arg2->n_info.n_xint.xi_int);
else if (((arg1) && (arg1)->n_type == 9) && ((arg2) && (arg2)->n_type == 9))
return (arg1->n_info.n_xfloat.xf_float == arg2->n_info.n_xfloat.xf_float);
else if (((arg1) && (arg1)->n_type == 6) && ((arg2) && (arg2)->n_type == 6))
return (strcmp(arg1->n_info.n_xstr.xst_str,arg2->n_info.n_xstr.xst_str) == 0);
else
return (0);
}
int equal(NODE *arg1, NODE *arg2)
{
if (eql(arg1,arg2))
return (1);
else if (((arg1) && (arg1)->n_type == 3) && ((arg2) && (arg2)->n_type == 3))
return (equal(((arg1)->n_info.n_xlist.xl_car),((arg2)->n_info.n_xlist.xl_car)) && equal(((arg1)->n_info.n_xlist.xl_cdr),((arg2)->n_info.n_xlist.xl_cdr)));
else
return (0);
}
