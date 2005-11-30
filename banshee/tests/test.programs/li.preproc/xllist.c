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
extern NODE *s_unbound;
extern NODE *true;
extern int eq(),eql(),equal();
static NODE *cxr();
static NODE *nth(),*assoc();
static NODE *subst(),*sublis(),*map();
static NODE *cequal();
NODE *xcar(NODE *args) { return (cxr(args,"a")); }
NODE *xcdr(NODE *args) { return (cxr(args,"d")); }
NODE *xcaar(NODE *args) { return (cxr(args,"aa")); }
NODE *xcadr(NODE *args) { return (cxr(args,"da")); }
NODE *xcdar(NODE *args) { return (cxr(args,"ad")); }
NODE *xcddr(NODE *args) { return (cxr(args,"dd")); }
NODE *xcaaar(NODE *args) { return (cxr(args,"aaa")); }
NODE *xcaadr(NODE *args) { return (cxr(args,"daa")); }
NODE *xcadar(NODE *args) { return (cxr(args,"ada")); }
NODE *xcaddr(NODE *args) { return (cxr(args,"dda")); }
NODE *xcdaar(NODE *args) { return (cxr(args,"aad")); }
NODE *xcdadr(NODE *args) { return (cxr(args,"dad")); }
NODE *xcddar(NODE *args) { return (cxr(args,"add")); }
NODE *xcdddr(NODE *args) { return (cxr(args,"ddd")); }
NODE *xcaaaar(NODE *args) { return (cxr(args,"aaaa")); }
NODE *xcaaadr(NODE *args) { return (cxr(args,"daaa")); }
NODE *xcaadar(NODE *args) { return (cxr(args,"adaa")); }
NODE *xcaaddr(NODE *args) { return (cxr(args,"ddaa")); }
NODE *xcadaar(NODE *args) { return (cxr(args,"aada")); }
NODE *xcadadr(NODE *args) { return (cxr(args,"dada")); }
NODE *xcaddar(NODE *args) { return (cxr(args,"adda")); }
NODE *xcadddr(NODE *args) { return (cxr(args,"ddda")); }
NODE *xcdaaar(NODE *args) { return (cxr(args,"aaad")); }
NODE *xcdaadr(NODE *args) { return (cxr(args,"daad")); }
NODE *xcdadar(NODE *args) { return (cxr(args,"adad")); }
NODE *xcdaddr(NODE *args) { return (cxr(args,"ddad")); }
NODE *xcddaar(NODE *args) { return (cxr(args,"aadd")); }
NODE *xcddadr(NODE *args) { return (cxr(args,"dadd")); }
NODE *xcdddar(NODE *args) { return (cxr(args,"addd")); }
NODE *xcddddr(NODE *args) { return (cxr(args,"dddd")); }
static NODE *cxr(NODE *args, char *adstr)
{
NODE *list;
list = xlmatch(3,&args);
xllastarg(args);
while (*adstr && ((list) && (list)->n_type == 3))
list = (*adstr++ == 'a' ? ((list)->n_info.n_xlist.xl_car) : ((list)->n_info.n_xlist.xl_cdr));
if (*adstr && list)
xlfail("bad argument");
return (list);
}
NODE *xcons(NODE *args)
{
NODE *arg1,*arg2;
arg1 = xlarg(&args);
arg2 = xlarg(&args);
xllastarg(args);
return (cons(arg1,arg2));
}
NODE *xlist(NODE *args)
{
NODE ***oldstk,*arg,*list,*val,*last;
NODE *lptr = (NODE *)0;
oldstk = xlsave(&arg,&list,&val,(NODE **)0);
arg = args;
for (last = (NODE *)0; arg; last = lptr) {
val = xlarg(&arg);
lptr = consa(val);
if (last == (NODE *)0)
list = lptr;
else
((last)->n_info.n_xlist.xl_cdr = (lptr));
}
xlstack = oldstk;
return (list);
}
NODE *xappend(NODE *args)
{
NODE ***oldstk,*arg,*list,*last,*val,*lptr;
oldstk = xlsave(&arg,&list,&last,&val,(NODE **)0);
arg = args;
while (arg) {
list = xlmatch(3,&arg);
while (((list) && (list)->n_type == 3)) {
lptr = consa(((list)->n_info.n_xlist.xl_car));
if (last == (NODE *)0)
val = lptr;
else
((last)->n_info.n_xlist.xl_cdr = (lptr));
last = lptr;
list = ((list)->n_info.n_xlist.xl_cdr);
}
}
xlstack = oldstk;
return (val);
}
NODE *xreverse(NODE *args)
{
NODE ***oldstk,*list,*val;
oldstk = xlsave(&list,&val,(NODE **)0);
list = xlmatch(3,&args);
xllastarg(args);
while (((list) && (list)->n_type == 3)) {
val = cons(((list)->n_info.n_xlist.xl_car),val);
list = ((list)->n_info.n_xlist.xl_cdr);
}
xlstack = oldstk;
return (val);
}
NODE *xlast(NODE *args)
{
NODE *list;
list = xlmatch(3,&args);
xllastarg(args);
while (((list) && (list)->n_type == 3) && ((list)->n_info.n_xlist.xl_cdr))
list = ((list)->n_info.n_xlist.xl_cdr);
return (list);
}
NODE *xmember(NODE *args)
{
NODE ***oldstk,*x,*list,*fcn,*val;
int tresult;
oldstk = xlsave(&x,&list,&fcn,(NODE **)0);
x = xlarg(&args);
list = xlmatch(3,&args);
xltest(&fcn,&tresult,&args);
xllastarg(args);
for (val = (NODE *)0; ((list) && (list)->n_type == 3); list = ((list)->n_info.n_xlist.xl_cdr))
if (dotest(x,((list)->n_info.n_xlist.xl_car),fcn) == tresult) {
val = list;
break;
}
xlstack = oldstk;
return (val);
}
NODE *xassoc(NODE *args)
{
NODE ***oldstk,*x,*alist,*fcn,*pair,*val;
int tresult;
oldstk = xlsave(&x,&alist,&fcn,(NODE **)0);
x = xlarg(&args);
alist = xlmatch(3,&args);
xltest(&fcn,&tresult,&args);
xllastarg(args);
for (val = (NODE *)0; ((alist) && (alist)->n_type == 3); alist = ((alist)->n_info.n_xlist.xl_cdr))
if ((pair = ((alist)->n_info.n_xlist.xl_car)) && ((pair) && (pair)->n_type == 3))
if (dotest(x,((pair)->n_info.n_xlist.xl_car),fcn) == tresult) {
val = pair;
break;
}
xlstack = oldstk;
return (val);
}
NODE *xsubst(NODE *args)
{
NODE ***oldstk,*to,*from,*expr,*fcn,*val;
int tresult;
oldstk = xlsave(&to,&from,&expr,&fcn,(NODE **)0);
to = xlarg(&args);
from = xlarg(&args);
expr = xlarg(&args);
xltest(&fcn,&tresult,&args);
xllastarg(args);
val = subst(to,from,expr,fcn,tresult);
xlstack = oldstk;
return (val);
}
static NODE *subst(NODE *to, NODE *from, NODE *expr, NODE *fcn, int tresult)
{
NODE ***oldstk,*carval,*cdrval,*val;
if (dotest(expr,from,fcn) == tresult)
val = to;
else if (((expr) && (expr)->n_type == 3)) {
oldstk = xlsave(&carval,&cdrval,(NODE **)0);
carval = subst(to,from,((expr)->n_info.n_xlist.xl_car),fcn,tresult);
cdrval = subst(to,from,((expr)->n_info.n_xlist.xl_cdr),fcn,tresult);
val = cons(carval,cdrval);
xlstack = oldstk;
}
else
val = expr;
return (val);
}
NODE *xsublis(NODE *args)
{
NODE ***oldstk,*alist,*expr,*fcn,*val;
int tresult;
oldstk = xlsave(&alist,&expr,&fcn,(NODE **)0);
alist = xlmatch(3,&args);
expr = xlarg(&args);
xltest(&fcn,&tresult,&args);
xllastarg(args);
val = sublis(alist,expr,fcn,tresult);
xlstack = oldstk;
return (val);
}
static NODE *sublis(NODE *alist, NODE *expr, NODE *fcn, int tresult)
{
NODE ***oldstk,*carval,*cdrval,*val;
if (val = assoc(expr,alist,fcn,tresult))
val = ((val)->n_info.n_xlist.xl_cdr);
else if (((expr) && (expr)->n_type == 3)) {
oldstk = xlsave(&carval,&cdrval,(NODE **)0);
carval = sublis(alist,((expr)->n_info.n_xlist.xl_car),fcn,tresult);
cdrval = sublis(alist,((expr)->n_info.n_xlist.xl_cdr),fcn,tresult);
val = cons(carval,cdrval);
xlstack = oldstk;
}
else
val = expr;
return (val);
}
static NODE *assoc(NODE *expr, NODE *alist, NODE *fcn, int tresult)
{
NODE *pair;
for (; ((alist) && (alist)->n_type == 3); alist = ((alist)->n_info.n_xlist.xl_cdr))
if ((pair = ((alist)->n_info.n_xlist.xl_car)) && ((pair) && (pair)->n_type == 3))
if (dotest(expr,((pair)->n_info.n_xlist.xl_car),fcn) == tresult)
return (pair);
return ((NODE *)0);
}
NODE *xremove(NODE *args)
{
NODE ***oldstk,*x,*list,*fcn,*val,*p;
NODE *last = (NODE *)0;
int tresult;
oldstk = xlsave(&x,&list,&fcn,&val,(NODE **)0);
x = xlarg(&args);
list = xlmatch(3,&args);
xltest(&fcn,&tresult,&args);
xllastarg(args);
while (((list) && (list)->n_type == 3)) {
if (dotest(x,((list)->n_info.n_xlist.xl_car),fcn) != tresult) {
p = consa(((list)->n_info.n_xlist.xl_car));
if (val) ((last)->n_info.n_xlist.xl_cdr = (p));
else val = p;
last = p;
}
list = ((list)->n_info.n_xlist.xl_cdr);
}
xlstack = oldstk;
return (val);
}
int dotest(NODE *arg1, NODE *arg2, NODE *fcn)
{
NODE ***oldstk,*args,*val;
oldstk = xlsave(&args,(NODE **)0);
args = consa(arg1);
((args)->n_info.n_xlist.xl_cdr = (consa(arg2)));
val = xlapply(fcn,args);
xlstack = oldstk;
return (val != (NODE *)0);
}
NODE *xnth(NODE *args)
{
return (nth(args,1));
}
NODE *xnthcdr(NODE *args)
{
return (nth(args,0));
}
static NODE *nth(NODE *args, int carflag)
{
NODE *list;
int n;
if ((n = ((xlmatch(5,&args))->n_info.n_xint.xi_int)) < 0)
xlfail("bad argument");
if ((list = xlmatch(3,&args)) == (NODE *)0)
xlfail("bad argument");
xllastarg(args);
while (((list) && (list)->n_type == 3) && n--)
list = ((list)->n_info.n_xlist.xl_cdr);
return (carflag && ((list) && (list)->n_type == 3) ? ((list)->n_info.n_xlist.xl_car) : list);
}
NODE *xlength(NODE *args)
{
NODE *arg;
int n;
arg = xlarg(&args);
xllastarg(args);
if (((arg) == (NODE *)0 || (arg)->n_type == 3))
for (n = 0; ((arg) && (arg)->n_type == 3); n++)
arg = ((arg)->n_info.n_xlist.xl_cdr);
else if (((arg) && (arg)->n_type == 6))
n = strlen(((arg)->n_info.n_xstr.xst_str));
else if (((arg) && (arg)->n_type == 10))
n = ((arg)->n_info.n_xvect.xv_size);
else
xlerror("bad argument type",arg);
return (cvfixnum((long)n));
}
NODE *xmapc(NODE *args)
{
return (map(args,1,0));
}
NODE *xmapcar(NODE *args)
{
return (map(args,1,1));
}
NODE *xmapl(NODE *args)
{
return (map(args,0,0));
}
NODE *xmaplist(NODE *args)
{
return (map(args,0,1));
}
static NODE *map(NODE *args, int carflag, int valflag)
{
NODE ***oldstk,*fcn,*lists,*arglist,*val,*p,*x,*y;
NODE *last = (NODE *)0;
oldstk = xlsave(&fcn,&lists,&arglist,&val,(NODE **)0);
fcn = xlarg(&args);
lists = xlmatch(3,&args);
if (!valflag)
val = lists;
lists = consa(lists);
while (args) {
lists = consd(lists);
((lists)->n_info.n_xlist.xl_car = (xlmatch(3,&args)));
}
if (((fcn) && (fcn)->n_type == 4))
fcn = xleval(fcn);
for (;;) {
arglist = (NODE *)0;
for (x = lists; x && (y = ((x)->n_info.n_xlist.xl_car)) && ((y) && (y)->n_type == 3); x = ((x)->n_info.n_xlist.xl_cdr)) {
arglist = consd(arglist);
((arglist)->n_info.n_xlist.xl_car = (carflag ? ((y)->n_info.n_xlist.xl_car) : y));
((x)->n_info.n_xlist.xl_car = (((y)->n_info.n_xlist.xl_cdr)));
}
if (x) break;
if (valflag) {
p = consa((NODE *)0);
if (val) ((last)->n_info.n_xlist.xl_cdr = (p));
else val = p;
((p)->n_info.n_xlist.xl_car = (xlapply(fcn,arglist)));
last = p;
}
else
xlapply(fcn,arglist);
}
xlstack = oldstk;
return (val);
}
NODE *xrplca(NODE *args)
{
NODE *list,*newcar;
if ((list = xlmatch(3,&args)) == (NODE *)0)
xlfail("bad argument");
newcar = xlarg(&args);
xllastarg(args);
((list)->n_info.n_xlist.xl_car = (newcar));
return (list);
}
NODE *xrplcd(NODE *args)
{
NODE *list,*newcdr;
if ((list = xlmatch(3,&args)) == (NODE *)0)
xlfail("bad argument");
newcdr = xlarg(&args);
xllastarg(args);
((list)->n_info.n_xlist.xl_cdr = (newcdr));
return (list);
}
NODE *xnconc(NODE *args)
{
NODE *list,*val;
NODE *last = (NODE *)0;
for (val = (NODE *)0; args; ) {
if (list = xlmatch(3,&args)) {
if (val)
((last)->n_info.n_xlist.xl_cdr = (list));
else
val = list;
while (((((list)->n_info.n_xlist.xl_cdr)) && (((list)->n_info.n_xlist.xl_cdr))->n_type == 3))
list = ((list)->n_info.n_xlist.xl_cdr);
last = list;
}
}
return (val);
}
NODE *xdelete(NODE *args)
{
NODE ***oldstk,*x,*list,*fcn,*last,*val;
int tresult;
oldstk = xlsave(&x,&list,&fcn,(NODE **)0);
x = xlarg(&args);
list = xlmatch(3,&args);
xltest(&fcn,&tresult,&args);
xllastarg(args);
while (((list) && (list)->n_type == 3)) {
if (dotest(x,((list)->n_info.n_xlist.xl_car),fcn) != tresult)
break;
list = ((list)->n_info.n_xlist.xl_cdr);
}
val = last = list;
if (((list) && (list)->n_type == 3)) {
list = ((list)->n_info.n_xlist.xl_cdr);
while (((list) && (list)->n_type == 3)) {
if (dotest(x,((list)->n_info.n_xlist.xl_car),fcn) == tresult)
((last)->n_info.n_xlist.xl_cdr = (((list)->n_info.n_xlist.xl_cdr)));
else
last = list;
list = ((list)->n_info.n_xlist.xl_cdr);
}
}
xlstack = oldstk;
return (val);
}
NODE *xatom(NODE *args)
{
NODE *arg;
arg = xlarg(&args);
xllastarg(args);
return (((arg) == (NODE *)0 || (arg)->n_type != 3) ? true : (NODE *)0);
}
NODE *xsymbolp(NODE *args)
{
NODE *arg;
arg = xlarg(&args);
xllastarg(args);
return (arg == (NODE *)0 || ((arg) && (arg)->n_type == 4) ? true : (NODE *)0);
}
NODE *xnumberp(NODE *args)
{
NODE *arg;
arg = xlarg(&args);
xllastarg(args);
return (((arg) && (arg)->n_type == 5) || ((arg) && (arg)->n_type == 9) ? true : (NODE *)0);
}
NODE *xboundp(NODE *args)
{
NODE *sym;
sym = xlmatch(4,&args);
xllastarg(args);
return (((sym)->n_info.n_xsym.xsy_value) == s_unbound ? (NODE *)0 : true);
}
NODE *xnull(NODE *args)
{
NODE *arg;
arg = xlarg(&args);
xllastarg(args);
return (((arg) == (NODE *)0) ? true : (NODE *)0);
}
NODE *xlistp(NODE *args)
{
NODE *arg;
arg = xlarg(&args);
xllastarg(args);
return (((arg) == (NODE *)0 || (arg)->n_type == 3) ? true : (NODE *)0);
}
NODE *xconsp(NODE *args)
{
NODE *arg;
arg = xlarg(&args);
xllastarg(args);
return (((arg) && (arg)->n_type == 3) ? true : (NODE *)0);
}
NODE *xeq(NODE *args)
{
return (cequal(args,eq));
}
NODE *xeql(NODE *args)
{
return (cequal(args,eql));
}
NODE *xequal(NODE *args)
{
return (cequal(args,equal));
}
static NODE *cequal(NODE *args, int (*fcn) ())
{
NODE *arg1,*arg2;
arg1 = xlarg(&args);
arg2 = xlarg(&args);
xllastarg(args);
return ((*fcn)(arg1,arg2) ? true : (NODE *)0);
}
