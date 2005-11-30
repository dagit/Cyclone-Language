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
extern struct node ***xlsave(struct node **arg1, struct node **arg2, struct node **arg3, struct node **arg4, struct node **arg5,
			     struct node **arg6, struct node **arg7, struct node **arg8);	extern struct node *xleval(NODE *expr);	extern struct node *xlapply(NODE *fun, NODE *args);	extern struct node *xlevlist(NODE *args);	extern struct node *xlarg();	extern struct node *xlevarg();	extern struct node *xlmatch();	extern struct node *xlevmatch();	extern struct node *xlgetfile();	extern struct node *xlsend();	extern struct node *xlenter();	extern struct node *xlsenter();	extern struct node *xlmakesym();	extern struct node *xlframe();	extern struct node *xlgetvalue();	extern struct node *xlxgetvalue();	extern struct node *xlygetvalue();	extern struct node *cons();	extern struct node *consa();	extern struct node *consd();	extern struct node *cvsymbol();	extern struct node *cvcsymbol();	extern struct node *cvstring();	extern struct node *cvcstring();	extern struct node *cvfile();	extern struct node *cvsubr();	extern struct node *cvfixnum();	extern struct node *cvflonum();	extern struct node *newstring();	extern struct node *newvector();	extern struct node *newobject();	extern struct node *xlgetprop();	extern char *xlsymname();	extern void xlsetvalue();
extern void xlprint();
extern void xltest();
extern int xlsample;
extern NODE ***xlstack,***xlstkbase,*xlenv;
extern NODE *s_lambda,*s_macro;
extern NODE *k_optional,*k_rest,*k_aux;
extern NODE *s_evalhook,*s_applyhook;
extern NODE *s_unbound;
extern NODE *s_stdout;
extern NODE **trace_stack;
extern int xltrace;
NODE *xlxeval(NODE *expr);
static NODE *evalhook();
static NODE *evform();
static NODE *evfun();
NODE *xleval(NODE *expr)
{
if (--xlsample <= 0) {
xlsample = 100;
oscheck();
}
if (((s_evalhook)->n_info.n_xsym.xsy_value))
return (evalhook(expr));
if (++xltrace < 500)
trace_stack[xltrace] = expr;
if (((expr) && (expr)->n_type == 3))
expr = evform(expr);
else if (((expr) && (expr)->n_type == 4))
expr = xlgetvalue(expr);
--xltrace;
return (expr);
}
NODE *xlxeval(NODE *expr)
{
if (((expr) && (expr)->n_type == 3))
expr = evform(expr);
else if (((expr) && (expr)->n_type == 4))
expr = xlgetvalue(expr);
return (expr);
}
NODE *xlapply(NODE *fun, NODE *args)
{
NODE *env,*val;
if (fun == (NODE *)0)
xlfail("bad function");
if (((fun) && (fun)->n_type == 1))
val = (*((fun)->n_info.n_xsubr.xsu_subr))(args);
else if (((fun) && (fun)->n_type == 3)) {
if (((((fun)->n_info.n_xlist.xl_car)) && (((fun)->n_info.n_xlist.xl_car))->n_type == 3)) {
env = ((fun)->n_info.n_xlist.xl_cdr);
fun = ((fun)->n_info.n_xlist.xl_car);
}
else
env = xlenv;
if (((fun)->n_info.n_xlist.xl_car) != s_lambda)
xlfail("bad function type");
val = evfun(fun,args,env);
}
else
xlfail("bad function");
return (val);
}
static NODE *evform(NODE *expr)
{
NODE ***oldstk,*fun,*args,*env,*val,*type;
oldstk = xlsave(&fun,&args,(NODE **)0,0,0,0,0,0);
fun = ((expr)->n_info.n_xlist.xl_car);
args = ((expr)->n_info.n_xlist.xl_cdr);
if ((fun = xleval(fun)) == (NODE *)0)
xlfail("bad function");
if (((fun) && (fun)->n_type == 1) || ((fun) && (fun)->n_type == 2)) {
if (((fun) && (fun)->n_type == 1))
args = xlevlist(args);
val = (*((fun)->n_info.n_xsubr.xsu_subr))(args);
}
else if (((fun) && (fun)->n_type == 3)) {
if (((((fun)->n_info.n_xlist.xl_car)) && (((fun)->n_info.n_xlist.xl_car))->n_type == 3)) {
env = ((fun)->n_info.n_xlist.xl_cdr);
fun = ((fun)->n_info.n_xlist.xl_car);
}
else
env = xlenv;
if ((type = ((fun)->n_info.n_xlist.xl_car)) == s_lambda) {
args = xlevlist(args);
val = evfun(fun,args,env);
}
else if (type == s_macro) {
args = evfun(fun,args,env);
val = xleval(args);
}
else
xlfail("bad function type");
}
else if (((fun) && (fun)->n_type == 7))
val = xlsend(fun,args);
else
xlfail("bad function");
xlstack = oldstk;
return (val);
}
static NODE *evalhook(NODE *expr)
{
NODE ***oldstk,*ehook,*ahook,*args,*val;
oldstk = xlsave(&ehook,&ahook,&args,(NODE **)0,0,0,0,0);
args = consa(expr);
((args)->n_info.n_xlist.xl_cdr = (consa(xlenv)));
ehook = ((s_evalhook)->n_info.n_xsym.xsy_value);
((s_evalhook)->n_info.n_xsym.xsy_value = ((NODE *)0));
ahook = ((s_applyhook)->n_info.n_xsym.xsy_value);
((s_applyhook)->n_info.n_xsym.xsy_value = ((NODE *)0));
val = xlapply(ehook,args);
((s_evalhook)->n_info.n_xsym.xsy_value = (ehook));
((s_applyhook)->n_info.n_xsym.xsy_value = (ahook));
xlstack = oldstk;
return (val);
}
NODE *xlevlist(NODE *args)
{
NODE ***oldstk,*src,*dst,*new,*val;
NODE *last = (NODE *)0;
oldstk = xlsave(&src,&dst,(NODE **)0,0,0,0,0,0);
src = args;
for (val = (NODE *)0; src; src = ((src)->n_info.n_xlist.xl_cdr)) {
if (!((src) && (src)->n_type == 3))
xlfail("bad argument list");
new = consa((NODE *)0);
if (val)
((last)->n_info.n_xlist.xl_cdr = (new));
else
val = dst = new;
((new)->n_info.n_xlist.xl_car = (xleval(((src)->n_info.n_xlist.xl_car))));
last = new;
}
xlstack = oldstk;
return (val);
}
xlunbound(NODE *sym)
{
xlcerror("try evaluating symbol again","unbound variable",sym);
}
static NODE *evfun(NODE *fun, NODE *args, NODE *env)
{
NODE ***oldstk,*oldenv,*newenv,*cptr,*fargs,*val;
oldstk = xlsave(&oldenv,&newenv,&cptr,(NODE **)0, 0, 0, 0, 0);
if ((fun = ((fun)->n_info.n_xlist.xl_cdr)) == (NODE *)0 || !((fun) && (fun)->n_type == 3))
xlfail("bad function definition");
if ((fargs = ((fun)->n_info.n_xlist.xl_car)) && !((fargs) && (fargs)->n_type == 3))
xlfail("bad formal argument list");
newenv = xlframe(env);
oldenv = xlenv;
xlabind(fargs,args,newenv);
xlenv = newenv;
for (cptr = ((fun)->n_info.n_xlist.xl_cdr); cptr; )
val = xlevarg(&cptr);
xlenv = oldenv;
xlstack = oldstk;
return (val);
}
xlabind(NODE *fargs, NODE *aargs, NODE *env)
{
NODE *arg;
while (((fargs) && (fargs)->n_type == 3) && !iskeyword(arg = ((fargs)->n_info.n_xlist.xl_car)) && ((aargs) && (aargs)->n_type == 3)) {
xlbind(arg,((aargs)->n_info.n_xlist.xl_car),env);
fargs = ((fargs)->n_info.n_xlist.xl_cdr);
aargs = ((aargs)->n_info.n_xlist.xl_cdr);
}
if (((fargs) && (fargs)->n_type == 3) && ((fargs)->n_info.n_xlist.xl_car) == k_optional) {
fargs = ((fargs)->n_info.n_xlist.xl_cdr);
while (((fargs) && (fargs)->n_type == 3) && !iskeyword(arg = ((fargs)->n_info.n_xlist.xl_car)) && ((aargs) && (aargs)->n_type == 3)) {
xlbind(arg,((aargs)->n_info.n_xlist.xl_car),env);
fargs = ((fargs)->n_info.n_xlist.xl_cdr);
aargs = ((aargs)->n_info.n_xlist.xl_cdr);
}
while (((fargs) && (fargs)->n_type == 3) && !iskeyword(arg = ((fargs)->n_info.n_xlist.xl_car))) {
xlbind(arg,(NODE *)0,env);
fargs = ((fargs)->n_info.n_xlist.xl_cdr);
}
}
if (((fargs) && (fargs)->n_type == 3) && ((fargs)->n_info.n_xlist.xl_car) == k_rest) {
fargs = ((fargs)->n_info.n_xlist.xl_cdr);
if (((fargs) && (fargs)->n_type == 3) && (arg = ((fargs)->n_info.n_xlist.xl_car)) && !iskeyword(arg))
xlbind(arg,aargs,env);
else
xlfail("symbol missing after &rest");
fargs = ((fargs)->n_info.n_xlist.xl_cdr);
aargs = (NODE *)0;
}
if (((fargs) && (fargs)->n_type == 3) && ((fargs)->n_info.n_xlist.xl_car) == k_aux)
while ((fargs = ((fargs)->n_info.n_xlist.xl_cdr)) != (NODE *)0 && ((fargs) && (fargs)->n_type == 3))
xlbind(((fargs)->n_info.n_xlist.xl_car),(NODE *)0,env);
if (fargs != aargs)
xlfail(fargs ? "too few arguments" : "too many arguments");
}
static int iskeyword(NODE *sym)
{
return (sym == k_optional || sym == k_rest || sym == k_aux);
}
NODE ***xlsave(NODE **arg1, NODE **arg2, NODE **arg3, NODE **arg4, NODE **arg5,
NODE **arg6, NODE **arg7, NODE **arg8)
{
char *	pvar;
NODE ***oldstk,**nptr;
oldstk = xlstack;
*--xlstack = arg1;
(pvar = (char *)&( arg1));
pvar = &arg2; pvar = &arg3; pvar = &arg4; pvar = &arg5; pvar = &arg6;
pvar = &arg7; pvar = &arg8;
for (nptr = (pvar = pvar + sizeof( NODE **), ( NODE **)(pvar - sizeof( NODE **)));
nptr != (NODE **) 0; nptr = (pvar = pvar + sizeof( NODE **), ( NODE **)(pvar - sizeof( NODE **)))) {
if (xlstack <= xlstkbase)
xlabort("evaluation stack overflow");
*--xlstack = nptr;
*nptr = (NODE *)0;
}
;
return (oldstk);
}
