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
extern NODE ***xlstack,*xlenv;
extern NODE *s_car,*s_cdr,*s_nth,*s_get,*s_svalue,*s_splist,*s_aref;
extern NODE *s_lambda,*s_macro;
extern NODE *s_comma,*s_comat;
extern NODE *s_unbound;
extern char gsprefix[];
extern int gsnumber;
static NODE *bquote1();
static NODE *defun();
static NODE *makesymbol();
NODE *xeval(NODE *args)
{
NODE ***oldstk,*expr,*val;
oldstk = xlsave(&expr,(NODE **)0);
expr = xlarg(&args);
xllastarg(args);
val = xleval(expr);
xlstack = oldstk;
return (val);
}
NODE *xapply(NODE *args)
{
NODE ***oldstk,*fun,*arglist,*val;
oldstk = xlsave(&fun,&arglist,(NODE **)0);
fun = xlarg(&args);
arglist = xlmatch(3,&args);
xllastarg(args);
if (((fun) && (fun)->n_type == 4))
fun = xleval(fun);
val = xlapply(fun,arglist);
xlstack = oldstk;
return (val);
}
NODE *xfuncall(NODE *args)
{
NODE ***oldstk,*fun,*arglist,*val;
oldstk = xlsave(&fun,&arglist,(NODE **)0);
fun = xlarg(&args);
arglist = args;
if (((fun) && (fun)->n_type == 4))
fun = xleval(fun);
val = xlapply(fun,arglist);
xlstack = oldstk;
return (val);
}
NODE *xquote(NODE *args)
{
NODE *val;
val = xlarg(&args);
xllastarg(args);
return (val);
}
NODE *xfunction(NODE *args)
{
NODE *val;
val = xlarg(&args);
xllastarg(args);
if (((val) && (val)->n_type == 3) && ((val)->n_info.n_xlist.xl_car) == s_lambda)
val = cons(val,xlenv);
else if (((val) && (val)->n_type == 4))
val = xlgetvalue(val);
else
xlerror("not a function",val);
return (val);
}
NODE *xlambda(NODE *args)
{
NODE ***oldstk,*fargs,*closure;
oldstk = xlsave(&fargs,&closure,(NODE **)0);
fargs = xlmatch(3,&args);
closure = cons(fargs,args);
closure = cons(s_lambda,closure);
closure = cons(closure,xlenv);
xlstack = oldstk;
return (closure);
}
NODE *xbquote(NODE *args)
{
NODE ***oldstk,*expr,*val;
oldstk = xlsave(&expr,(NODE **)0);
expr = xlarg(&args);
xllastarg(args);
val = bquote1(expr);
xlstack = oldstk;
return (val);
}
static NODE *bquote1(NODE *expr)
{
NODE ***oldstk,*val,*list,*last,*new;
if (((expr) == (NODE *)0 || (expr)->n_type != 3))
val = expr;
else if (((expr)->n_info.n_xlist.xl_car) == s_comma) {
if (((((expr)->n_info.n_xlist.xl_cdr)) == (NODE *)0 || (((expr)->n_info.n_xlist.xl_cdr))->n_type != 3))
xlfail("bad comma expression");
val = xleval(((((expr)->n_info.n_xlist.xl_cdr))->n_info.n_xlist.xl_car));
}
else if (((((expr)->n_info.n_xlist.xl_car)) && (((expr)->n_info.n_xlist.xl_car))->n_type == 3) && ((((expr)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_car) == s_comat) {
oldstk = xlsave(&list,&val,(NODE **)0);
if (((((((expr)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_cdr)) == (NODE *)0 || (((((expr)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_cdr))->n_type != 3))
xlfail("bad comma-at expression");
list = xleval(((((((expr)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_cdr))->n_info.n_xlist.xl_car));
for (last = (NODE *)0; ((list) && (list)->n_type == 3); list = ((list)->n_info.n_xlist.xl_cdr)) {
new = consa(((list)->n_info.n_xlist.xl_car));
if (last)
((last)->n_info.n_xlist.xl_cdr = (new));
else
val = new;
last = new;
}
if (last)
((last)->n_info.n_xlist.xl_cdr = (bquote1(((expr)->n_info.n_xlist.xl_cdr))));
else
val = bquote1(((expr)->n_info.n_xlist.xl_cdr));
xlstack = oldstk;
}
else {
oldstk = xlsave(&val,(NODE **)0);
val = consa((NODE *)0);
((val)->n_info.n_xlist.xl_car = (bquote1(((expr)->n_info.n_xlist.xl_car))));
((val)->n_info.n_xlist.xl_cdr = (bquote1(((expr)->n_info.n_xlist.xl_cdr))));
xlstack = oldstk;
}
return (val);
}
NODE *xset(NODE *args)
{
NODE *sym,*val;
sym = xlmatch(4,&args);
val = xlarg(&args);
xllastarg(args);
((sym)->n_info.n_xsym.xsy_value = (val));
return (val);
}
NODE *xsetq(NODE *args)
{
NODE ***oldstk,*arg,*sym,*val;
oldstk = xlsave(&arg,&sym,&val,(NODE **)0);
arg = args;
while (arg) {
sym = xlmatch(4,&arg);
val = xlevarg(&arg);
xlsetvalue(sym,val);
}
xlstack = oldstk;
return (val);
}
NODE *xsetf(NODE *args)
{
NODE ***oldstk,*arg,*place,*value;
oldstk = xlsave(&arg,&place,&value,(NODE **)0);
arg = args;
while (arg) {
place = xlarg(&arg);
value = xlevarg(&arg);
if (((place) && (place)->n_type == 4))
xlsetvalue(place,value);
else if (((place) && (place)->n_type == 3))
placeform(place,value);
else
xlfail("bad place form");
}
xlstack = oldstk;
return (value);
}
static placeform(NODE *place, NODE *value)
{
NODE ***oldstk,*fun,*arg1,*arg2;
int i;
if ((fun = xlmatch(4,&place)) == s_get) {
oldstk = xlsave(&arg1,&arg2,(NODE **)0);
arg1 = xlevmatch(4,&place);
arg2 = xlevmatch(4,&place);
xllastarg(place);
xlputprop(arg1,value,arg2);
xlstack = oldstk;
}
else if (fun == s_svalue || fun == s_splist) {
oldstk = xlsave(&arg1,(NODE **)0);
arg1 = xlevmatch(4,&place);
xllastarg(place);
if (fun == s_svalue)
((arg1)->n_info.n_xsym.xsy_value = (value));
else
((arg1)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_cdr = (value));
xlstack = oldstk;
}
else if (fun == s_car || fun == s_cdr) {
oldstk = xlsave(&arg1,(NODE **)0);
arg1 = xlevmatch(3,&place);
xllastarg(place);
if (((arg1) && (arg1)->n_type == 3))
if (fun == s_car)
((arg1)->n_info.n_xlist.xl_car = (value));
else
((arg1)->n_info.n_xlist.xl_cdr = (value));
xlstack = oldstk;
}
else if (fun == s_nth) {
oldstk = xlsave(&arg1,&arg2,(NODE **)0);
arg1 = xlevmatch(5,&place);
arg2 = xlevmatch(3,&place);
xllastarg(place);
for (i = (int)((arg1)->n_info.n_xint.xi_int); i > 0 && ((arg2) && (arg2)->n_type == 3); --i)
arg2 = ((arg2)->n_info.n_xlist.xl_cdr);
if (((arg2) && (arg2)->n_type == 3))
((arg2)->n_info.n_xlist.xl_car = (value));
xlstack = oldstk;
}
else if (fun == s_aref) {
oldstk = xlsave(&arg1,&arg2,(NODE **)0);
arg1 = xlevmatch(10,&place);
arg2 = xlevmatch(5,&place); i = (int)((arg2)->n_info.n_xint.xi_int);
xllastarg(place);
if (i < 0 || i >= ((arg1)->n_info.n_xvect.xv_size))
xlerror("index out of range",arg2);
((arg1)->n_info.n_xvect.xv_data[i] = (value));
xlstack = oldstk;
}
else
xlfail("bad place form");
}
NODE *xdefun(NODE *args)
{
return (defun(args,s_lambda));
}
NODE *xdefmacro(NODE *args)
{
return (defun(args,s_macro));
}
static NODE *defun(NODE *args, NODE *type)
{
NODE ***oldstk,*sym,*fargs,*closure;
oldstk = xlsave(&sym,&fargs,&closure,(NODE **)0);
sym = xlmatch(4,&args);
fargs = xlmatch(3,&args);
closure = cons(fargs,args);
closure = cons(type,closure);
closure = cons(closure,xlenv);
xlsetvalue(sym,closure);
xlstack = oldstk;
return (sym);
}
NODE *xgensym(NODE *args)
{
char sym[100+1];
NODE *x;
if (args) {
x = xlarg(&args);
switch (((x)->n_type)) {
case 6:
strcpy(gsprefix,((x)->n_info.n_xstr.xst_str));
break;
case 5:
gsnumber = ((x)->n_info.n_xint.xi_int);
break;
default:
xlerror("bad argument type",x);
}
}
xllastarg(args);
sprintf(sym,"%s%d",gsprefix,gsnumber++);
return (xlmakesym(sym,0));
}
NODE *xmakesymbol(NODE *args)
{
return (makesymbol(args,0));
}
NODE *xintern(NODE *args)
{
return (makesymbol(args,1));
}
static NODE *makesymbol(NODE *args, int iflag)
{
NODE ***oldstk,*pname,*val;
char *str;
oldstk = xlsave(&pname,(NODE **)0);
pname = xlmatch(6,&args);
xllastarg(args);
str = ((pname)->n_info.n_xstr.xst_str);
val = (iflag ? xlenter(str,0) : xlmakesym(str,0));
xlstack = oldstk;
return (val);
}
NODE *xsymname(NODE *args)
{
NODE *sym;
sym = xlmatch(4,&args);
xllastarg(args);
return (((sym)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_car));
}
NODE *xsymvalue(NODE *args)
{
NODE *sym,*val;
sym = xlmatch(4,&args);
xllastarg(args);
while ((val = ((sym)->n_info.n_xsym.xsy_value)) == s_unbound)
xlcerror("try evaluating symbol again","unbound variable",sym);
return (val);
}
NODE *xsymplist(NODE *args)
{
NODE *sym;
sym = xlmatch(4,&args);
xllastarg(args);
return (((sym)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_cdr));
}
NODE *xget(NODE *args)
{
NODE *sym,*prp;
sym = xlmatch(4,&args);
prp = xlmatch(4,&args);
xllastarg(args);
return (xlgetprop(sym,prp));
}
NODE *xputprop(NODE *args)
{
NODE *sym,*val,*prp;
sym = xlmatch(4,&args);
val = xlarg(&args);
prp = xlmatch(4,&args);
xllastarg(args);
xlputprop(sym,val,prp);
return (val);
}
NODE *xremprop(NODE *args)
{
NODE *sym,*prp;
sym = xlmatch(4,&args);
prp = xlmatch(4,&args);
xllastarg(args);
xlremprop(sym,prp);
return ((NODE *)0);
}
NODE *xhash(NODE *args)
{
char *str;
NODE *val;
int len;
val = xlarg(&args);
len = (int)((xlmatch(5,&args))->n_info.n_xint.xi_int);
xllastarg(args);
if (((val) && (val)->n_type == 4))
str = ((((val)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_car))->n_info.n_xstr.xst_str);
else if (((val) && (val)->n_type == 6))
str = ((val)->n_info.n_xstr.xst_str);
else
xlerror("bad argument type",val);
return (cvfixnum((long)hash(str,len)));
}
NODE *xaref(NODE *args)
{
NODE *array,*index;
int i;
array = xlmatch(10,&args);
index = xlmatch(5,&args); i = (int)((index)->n_info.n_xint.xi_int);
xllastarg(args);
if (i < 0 || i >= ((array)->n_info.n_xvect.xv_size))
xlerror("array index out of bounds",index);
return (((array)->n_info.n_xvect.xv_data[i]));
}
NODE *xmkarray(NODE *args)
{
int size;
size = (int)((xlmatch(5,&args))->n_info.n_xint.xi_int);
xllastarg(args);
return (newvector(size));
}
