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
extern NODE ***xlstack,*xlenv,*xlvalue;
extern NODE *s_unbound;
extern NODE *s_evalhook,*s_applyhook;
extern NODE *true;
extern NODE *xlxeval();
static NODE *let();
static NODE *prog();
static NODE *progx();
static NODE *doloop();
NODE *xcond(NODE *args)
{
NODE ***oldstk,*arg,*list,*val;
oldstk = xlsave(&arg,&list,(NODE **)0);
arg = args;
val = (NODE *)0;
while (arg) {
list = xlmatch(3,&arg);
if (val = xlevarg(&list)) {
while (list)
val = xlevarg(&list);
break;
}
}
xlstack = oldstk;
return (val);
}
NODE *xcase(NODE *args)
{
NODE ***oldstk,*key,*arg,*clause,*list,*val;
oldstk = xlsave(&key,&arg,&clause,(NODE **)0);
arg = args;
key = xlevarg(&arg);
val = (NODE *)0;
while (arg) {
clause = xlmatch(3,&arg);
if ((list = xlarg(&clause)) == true ||
(((list) == (NODE *)0 || (list)->n_type == 3) && keypresent(key,list)) ||
eql(key,list)) {
while (clause)
val = xlevarg(&clause);
break;
}
}
xlstack = oldstk;
return (val);
}
static int keypresent(NODE *key, NODE *list)
{
for (; ((list) && (list)->n_type == 3); list = ((list)->n_info.n_xlist.xl_cdr))
if (eql(((list)->n_info.n_xlist.xl_car),key))
return (1);
return (0);
}
NODE *xand(NODE *args)
{
NODE ***oldstk,*arg,*val;
oldstk = xlsave(&arg,(NODE **)0);
arg = args;
val = true;
while (arg)
if ((val = xlevarg(&arg)) == (NODE *)0)
break;
xlstack = oldstk;
return (val);
}
NODE *xor(NODE *args)
{
NODE ***oldstk,*arg,*val;
oldstk = xlsave(&arg,(NODE **)0);
arg = args;
val = (NODE *)0;
while (arg)
if ((val = xlevarg(&arg)))
break;
xlstack = oldstk;
return (val);
}
NODE *xif(NODE *args)
{
NODE ***oldstk,*testexpr,*thenexpr,*elseexpr,*val;
oldstk = xlsave(&testexpr,&thenexpr,&elseexpr,(NODE **)0);
testexpr = xlarg(&args);
thenexpr = xlarg(&args);
elseexpr = (args ? xlarg(&args) : (NODE *)0);
xllastarg(args);
val = xleval(xleval(testexpr) ? thenexpr : elseexpr);
xlstack = oldstk;
return (val);
}
NODE *xlet(NODE *args)
{
return (let(args,1));
}
NODE *xletstar(NODE *args)
{
return (let(args,0));
}
static NODE *let(NODE *args, int pflag)
{
NODE ***oldstk,*newenv,*arg,*val;
oldstk = xlsave(&newenv,&arg,(NODE **)0);
arg = args;
newenv = xlframe(xlenv);
if (!pflag) xlenv = newenv;
dobindings(xlmatch(3,&arg),newenv);
if (pflag) xlenv = newenv;
for (val = (NODE *)0; arg; )
val = xlevarg(&arg);
xlenv = ((xlenv)->n_info.n_xlist.xl_cdr);
xlstack = oldstk;
return (val);
}
NODE *xprog(NODE *args)
{
return (prog(args,1));
}
NODE *xprogstar(NODE *args)
{
return (prog(args,0));
}
static NODE *prog(NODE *args, int pflag)
{
NODE ***oldstk,*newenv,*arg,*val;
oldstk = xlsave(&newenv,&arg,(NODE **)0);
arg = args;
newenv = xlframe(xlenv);
if (!pflag) xlenv = newenv;
dobindings(xlmatch(3,&arg),newenv);
if (pflag) xlenv = newenv;
tagblock(arg,&val);
xlenv = ((xlenv)->n_info.n_xlist.xl_cdr);
xlstack = oldstk;
return (val);
}
NODE *xgo(NODE *args)
{
NODE *label;
label = xlarg(&args);
xllastarg(args);
xlgo(label);
}
NODE *xreturn(NODE *args)
{
NODE *val;
val = (args ? xlarg(&args) : (NODE *)0);
xllastarg(args);
xlreturn(val);
}
NODE *xprog1(NODE *args)
{
return (progx(args,1));
}
NODE *xprog2(NODE *args)
{
return (progx(args,2));
}
static NODE *progx(NODE *args, int n)
{
NODE ***oldstk,*arg,*val;
oldstk = xlsave(&arg,&val,(NODE **)0);
arg = args;
while (n--)
val = xlevarg(&arg);
while (arg)
xlevarg(&arg);
xlstack = oldstk;
return (val);
}
NODE *xprogn(NODE *args)
{
NODE ***oldstk,*arg,*val;
oldstk = xlsave(&arg,(NODE **)0);
arg = args;
for (val = (NODE *)0; arg; )
val = xlevarg(&arg);
xlstack = oldstk;
return (val);
}
NODE *xdo(NODE *args)
{
return (doloop(args,1));
}
NODE *xdostar(NODE *args)
{
return (doloop(args,0));
}
static NODE *doloop(NODE *args, int pflag)
{
NODE ***oldstk,*newenv,*arg,*blist,*clist,*test,*rval;
int rbreak;
oldstk = xlsave(&newenv,&arg,&blist,&clist,&test,(NODE **)0);
arg = args;
blist = xlmatch(3,&arg);
newenv = xlframe(xlenv);
if (!pflag) xlenv = newenv;
dobindings(blist,newenv);
if (pflag) xlenv = newenv;
clist = xlmatch(3,&arg);
test = xlarg(&clist);
rbreak = 0;
while (xleval(test) == (NODE *)0) {
if (tagblock(arg,&rval)) {
rbreak = 1;
break;
}
doupdates(blist,pflag);
}
if (!rbreak)
for (rval = (NODE *)0; ((clist) && (clist)->n_type == 3); )
rval = xlevarg(&clist);
xlenv = ((xlenv)->n_info.n_xlist.xl_cdr);
xlstack = oldstk;
return (rval);
}
NODE *xdolist(NODE *args)
{
NODE ***oldstk,*arg,*clist,*sym,*list,*val,*rval;
int rbreak;
oldstk = xlsave(&arg,&clist,&sym,&list,&val,(NODE **)0);
arg = args;
clist = xlmatch(3,&arg);
sym = xlmatch(4,&clist);
list = xlevmatch(3,&clist);
val = (clist ? xlarg(&clist) : (NODE *)0);
xlenv = xlframe(xlenv);
xlbind(sym,(NODE *)0,xlenv);
rbreak = 0;
for (; ((list) && (list)->n_type == 3); list = ((list)->n_info.n_xlist.xl_cdr)) {
xlsetvalue(sym,((list)->n_info.n_xlist.xl_car));
if (tagblock(arg,&rval)) {
rbreak = 1;
break;
}
}
if (!rbreak) {
xlsetvalue(sym,(NODE *)0);
rval = xleval(val);
}
xlenv = ((xlenv)->n_info.n_xlist.xl_cdr);
xlstack = oldstk;
return (rval);
}
NODE *xdotimes(NODE *args)
{
NODE ***oldstk,*arg,*clist,*sym,*val,*rval;
int rbreak,cnt,i;
oldstk = xlsave(&arg,&clist,&sym,&val,(NODE **)0);
arg = args;
clist = xlmatch(3,&arg);
sym = xlmatch(4,&clist);
cnt = ((xlevmatch(5,&clist))->n_info.n_xint.xi_int);
val = (clist ? xlarg(&clist) : (NODE *)0);
xlenv = xlframe(xlenv);
xlbind(sym,(NODE *)0,xlenv);
rbreak = 0;
for (i = 0; i < cnt; i++) {
xlsetvalue(sym,cvfixnum((long)i));
if (tagblock(arg,&rval)) {
rbreak = 1;
break;
}
}
if (!rbreak) {
xlsetvalue(sym,cvfixnum((long)cnt));
rval = xleval(val);
}
xlenv = ((xlenv)->n_info.n_xlist.xl_cdr);
xlstack = oldstk;
return (rval);
}
NODE *xcatch(NODE *args)
{
NODE ***oldstk,*tag,*arg,*val;
CONTEXT cntxt;
oldstk = xlsave(&tag,&arg,(NODE **)0);
tag = xlevarg(&args);
arg = args;
val = (NODE *)0;
xlbegin(&cntxt,4,tag);
if (setjmp(cntxt.c_jmpbuf))
val = xlvalue;
else {
while (arg)
val = xlevarg(&arg);
}
xlend(&cntxt);
xlstack = oldstk;
return (val);
}
NODE *xthrow(NODE *args)
{
NODE *tag,*val;
tag = xlarg(&args);
val = (args ? xlarg(&args) : (NODE *)0);
xllastarg(args);
xlthrow(tag,val);
}
NODE *xerror(NODE *args)
{
char *emsg; NODE *arg;
emsg = ((xlmatch(6,&args))->n_info.n_xstr.xst_str);
arg = (args ? xlarg(&args) : s_unbound);
xllastarg(args);
xlerror(emsg,arg);
}
NODE *xcerror(NODE *args)
{
char *cmsg,*emsg; NODE *arg;
cmsg = ((xlmatch(6,&args))->n_info.n_xstr.xst_str);
emsg = ((xlmatch(6,&args))->n_info.n_xstr.xst_str);
arg = (args ? xlarg(&args) : s_unbound);
xllastarg(args);
xlcerror(cmsg,emsg,arg);
return ((NODE *)0);
}
NODE *xbreak(NODE *args)
{
char *emsg; NODE *arg;
emsg = (args ? ((xlmatch(6,&args))->n_info.n_xstr.xst_str) : "**BREAK**");
arg = (args ? xlarg(&args) : s_unbound);
xllastarg(args);
xlbreak(emsg,arg);
return ((NODE *)0);
}
NODE *xcleanup(NODE *args)
{
xllastarg(args);
xlcleanup();
}
NODE *xcontinue(NODE *args)
{
xllastarg(args);
xlcontinue();
}
NODE *xerrset(NODE *args)
{
NODE ***oldstk,*expr,*flag,*val;
CONTEXT cntxt;
oldstk = xlsave(&expr,&flag,(NODE **)0);
expr = xlarg(&args);
flag = (args ? xlarg(&args) : true);
xllastarg(args);
xlbegin(&cntxt,8,flag);
if (setjmp(cntxt.c_jmpbuf))
val = (NODE *)0;
else {
expr = xleval(expr);
val = consa(expr);
}
xlend(&cntxt);
xlstack = oldstk;
return (val);
}
NODE *xevalhook(NODE *args)
{
NODE ***oldstk,*expr,*ehook,*ahook,*env,*newehook,*newahook,*newenv,*val;
oldstk = xlsave(&expr,&ehook,&ahook,&env,&newehook,&newahook,&newenv,(NODE **)0);
expr = xlarg(&args);
newehook = xlarg(&args);
newahook = xlarg(&args);
newenv = (args ? xlarg(&args) : xlenv);
xllastarg(args);
ehook = ((s_evalhook)->n_info.n_xsym.xsy_value);
((s_evalhook)->n_info.n_xsym.xsy_value = (newehook));
ahook = ((s_applyhook)->n_info.n_xsym.xsy_value);
((s_applyhook)->n_info.n_xsym.xsy_value = (newahook));
env = xlenv;
xlenv = newenv;
val = xlxeval(expr);
((s_evalhook)->n_info.n_xsym.xsy_value = (ehook));
((s_applyhook)->n_info.n_xsym.xsy_value = (ahook));
xlenv = env;
xlstack = oldstk;
return (val);
}
static dobindings(NODE *blist, NODE *env)
{
NODE ***oldstk,*list,*bnd,*sym,*val;
oldstk = xlsave(&list,&bnd,&sym,&val,(NODE **)0);
for (list = blist; ((list) && (list)->n_type == 3); list = ((list)->n_info.n_xlist.xl_cdr)) {
bnd = ((list)->n_info.n_xlist.xl_car);
if (((bnd) && (bnd)->n_type == 4)) {
sym = bnd;
val = (NODE *)0;
}
else if (((bnd) && (bnd)->n_type == 3)) {
sym = xlmatch(4,&bnd);
val = xlevarg(&bnd);
}
else
xlfail("bad binding");
xlbind(sym,val,env);
}
xlstack = oldstk;
}
doupdates(NODE *blist, int pflag)
{
NODE ***oldstk,*plist,*list,*bnd,*sym,*val;
oldstk = xlsave(&plist,&list,&bnd,&sym,&val,(NODE **)0);
for (list = blist; ((list) && (list)->n_type == 3); list = ((list)->n_info.n_xlist.xl_cdr)) {
bnd = ((list)->n_info.n_xlist.xl_car);
if (((bnd) && (bnd)->n_type == 3)) {
sym = xlmatch(4,&bnd);
bnd = ((bnd)->n_info.n_xlist.xl_cdr);
if (bnd) {
val = xlevarg(&bnd);
if (pflag) {
plist = consd(plist);
((plist)->n_info.n_xlist.xl_car = (cons(sym,val)));
}
else
xlsetvalue(sym,val);
}
}
}
for (; plist; plist = ((plist)->n_info.n_xlist.xl_cdr))
xlsetvalue(((((plist)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_car),((((plist)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_cdr));
xlstack = oldstk;
}
int tagblock(NODE *code, NODE **pval)
{
NODE ***oldstk,*arg;
CONTEXT cntxt;
int type,sts;
oldstk = xlsave(&arg,(NODE **)0);
arg = code;
xlbegin(&cntxt,1|2,arg);
if ((type = setjmp(cntxt.c_jmpbuf)) == 2) {
*pval = xlvalue;
sts = 1;
}
else {
if (type == 1)
arg = xlvalue;
while (((arg) && (arg)->n_type == 3))
if (((((arg)->n_info.n_xlist.xl_car)) && (((arg)->n_info.n_xlist.xl_car))->n_type == 3))
xlevarg(&arg);
else
arg = ((arg)->n_info.n_xlist.xl_cdr);
*pval = (NODE *)0;
sts = 0;
}
xlend(&cntxt);
xlstack = oldstk;
return (sts);
}
