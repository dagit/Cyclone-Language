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
extern struct node ***xlsave(NODE ** arg1, ...);	extern struct node *xleval();	extern struct node *xlapply();	extern struct node *xlevlist();	extern struct node *xlarg();	extern struct node *xlevarg();	extern struct node *xlmatch();	extern struct node *xlevmatch();	extern struct node *xlgetfile();	extern struct node *xlsend(NODE *obj, NODE *args);	extern struct node *xlenter();	extern struct node *xlsenter();	extern struct node *xlmakesym();	extern struct node *xlframe();	extern struct node *xlgetvalue();	extern struct node *xlxgetvalue();	extern struct node *xlygetvalue();	extern struct node *cons();	extern struct node *consa();	extern struct node *consd();	extern struct node *cvsymbol();	extern struct node *cvcsymbol();	extern struct node *cvstring();	extern struct node *cvcstring();	extern struct node *cvfile();	extern struct node *cvsubr();	extern struct node *cvfixnum();	extern struct node *cvflonum();	extern struct node *newstring();	extern struct node *newvector();	extern struct node *newobject();	extern struct node *xlgetprop();	extern char *xlsymname();	extern void xlsetvalue();
extern void xlprint();
extern void xltest();
extern NODE ***xlstack,*xlenv;
extern NODE *s_stdout;
extern NODE *self,*msgclass,*msgcls,*class,*object;
extern NODE *new,*isnew;
static NODE *entermsg();
static NODE *findmsg();
static NODE *sendmsg();
NODE *xlclass(char *name, int vcnt)
{
NODE *sym,*cls;
sym = xlsenter(name);
cls = newobject(class,7);
((sym)->n_info.n_xsym.xsy_value = (cls));
((cls)->n_info.n_xvect.xv_data[5+1] = (cvfixnum((long)vcnt)));
((cls)->n_info.n_xvect.xv_data[6+1] = (cvfixnum((long)vcnt)));
((cls)->n_info.n_xvect.xv_data[4+1] = (object));
return (cls);
}
xladdivar(NODE *cls, char *var)
{
((cls)->n_info.n_xvect.xv_data[1+1] = (cons(xlsenter(var),((cls)->n_info.n_xvect.xv_data[1+1]))));
}
xladdmsg(NODE *cls, char *msg, NODE *(*code) ())
{
NODE *mptr;
mptr = entermsg(cls,xlsenter(msg));
((mptr)->n_info.n_xlist.xl_cdr = (cvsubr(code,1)));
}
NODE *xlsend(NODE *obj, NODE *args)
{
NODE ***oldstk,*arglist,*msg,*val;
if ((msg = findmsg(((obj)->n_info.n_xvect.xv_data[0]),xlevmatch(4,&args))) == (NODE *)0)
xlfail("no method for this message");
oldstk = xlsave(&arglist,(NODE **)0);
arglist = xlevlist(args);
val = sendmsg(obj,msg,arglist);
xlstack = oldstk;
return (val);
}
int xlobgetvalue(NODE *sym, NODE **pval)
{
NODE *obj,*cls,*names;
int ivtotal,n;
obj = xlygetvalue(self);
cls = xlygetvalue(msgclass);
if (!(((obj) && (obj)->n_type == 7) && ((cls) && (cls)->n_type == 7)))
return (0);
for (; ((cls) && (cls)->n_type == 7); cls = ((cls)->n_info.n_xvect.xv_data[4+1])) {
names = ((cls)->n_info.n_xvect.xv_data[1+1]);
ivtotal = getivcnt(cls,6);
for (n = ivtotal - getivcnt(cls,5); n < ivtotal; ++n) {
if (((names)->n_info.n_xlist.xl_car) == sym) {
*pval = ((obj)->n_info.n_xvect.xv_data[n+1]);
return (1);
}
names = ((names)->n_info.n_xlist.xl_cdr);
}
names = ((cls)->n_info.n_xvect.xv_data[2+1]);
for (n = 0; ((names) && (names)->n_type == 3); ++n) {
if (((names)->n_info.n_xlist.xl_car) == sym) {
*pval = ((((cls)->n_info.n_xvect.xv_data[3+1]))->n_info.n_xvect.xv_data[n]);
return (1);
}
names = ((names)->n_info.n_xlist.xl_cdr);
}
}
return (0);
}
int xlobsetvalue(NODE *sym, NODE *val)
{
NODE *obj,*cls,*names;
int ivtotal,n;
obj = xlygetvalue(self);
cls = xlygetvalue(msgclass);
if (!(((obj) && (obj)->n_type == 7) && ((cls) && (cls)->n_type == 7)))
return (0);
for (; ((cls) && (cls)->n_type == 7); cls = ((cls)->n_info.n_xvect.xv_data[4+1])) {
names = ((cls)->n_info.n_xvect.xv_data[1+1]);
ivtotal = getivcnt(cls,6);
for (n = ivtotal - getivcnt(cls,5); n < ivtotal; ++n) {
if (((names)->n_info.n_xlist.xl_car) == sym) {
((obj)->n_info.n_xvect.xv_data[n+1] = (val));
return (1);
}
names = ((names)->n_info.n_xlist.xl_cdr);
}
names = ((cls)->n_info.n_xvect.xv_data[2+1]);
for (n = 0; ((names) && (names)->n_type == 3); ++n) {
if (((names)->n_info.n_xlist.xl_car) == sym) {
((((cls)->n_info.n_xvect.xv_data[3+1]))->n_info.n_xvect.xv_data[n] = (val));
return (1);
}
names = ((names)->n_info.n_xlist.xl_cdr);
}
}
return (0);
}
static NODE *obisnew(NODE *args)
{
xllastarg(args);
return (xlygetvalue(self));
}
static NODE *obclass(NODE *args)
{
xllastarg(args);
return (((xlygetvalue(self))->n_info.n_xvect.xv_data[0]));
}
static NODE *obshow(NODE *args)
{
NODE ***oldstk,*fptr,*obj,*cls,*names;
int ivtotal,n;
oldstk = xlsave(&fptr,(NODE **)0);
fptr = (args ? xlgetfile(&args) : ((s_stdout)->n_info.n_xsym.xsy_value));
xllastarg(args);
obj = xlygetvalue(self);
cls = ((obj)->n_info.n_xvect.xv_data[0]);
xlputstr(fptr,"Object is ");
xlprint(fptr,obj,1);
xlputstr(fptr,", Class is ");
xlprint(fptr,cls,1);
xlterpri(fptr);
for (cls = ((obj)->n_info.n_xvect.xv_data[0]); cls; cls = ((cls)->n_info.n_xvect.xv_data[4+1])) {
names = ((cls)->n_info.n_xvect.xv_data[1+1]);
ivtotal = getivcnt(cls,6);
for (n = ivtotal - getivcnt(cls,5); n < ivtotal; ++n) {
xlputstr(fptr,"  ");
xlprint(fptr,((names)->n_info.n_xlist.xl_car),1);
xlputstr(fptr," = ");
xlprint(fptr,((obj)->n_info.n_xvect.xv_data[n+1]),1);
xlterpri(fptr);
names = ((names)->n_info.n_xlist.xl_cdr);
}
}
xlstack = oldstk;
return (obj);
}
static NODE *obsendsuper(NODE *args)
{
NODE *obj,*super,*msg;
obj = xlygetvalue(self);
super = ((((obj)->n_info.n_xvect.xv_data[0]))->n_info.n_xvect.xv_data[4+1]);
if ((msg = findmsg(super,xlmatch(4,&args))) == (NODE *)0)
xlfail("no method for this message");
return (sendmsg(obj,msg,args));
}
static NODE *clnew(void)
{
NODE *cls;
cls = xlygetvalue(self);
return (newobject(cls,getivcnt(cls,6)));
}
static NODE *clisnew(NODE *args)
{
NODE *ivars,*cvars,*super,*cls;
int n;
ivars = xlmatch(3,&args);
cvars = (args ? xlmatch(3,&args) : (NODE *)0);
super = (args ? xlmatch(7,&args) : object);
xllastarg(args);
cls = xlygetvalue(self);
((cls)->n_info.n_xvect.xv_data[1+1] = (ivars));
((cls)->n_info.n_xvect.xv_data[2+1] = (cvars));
((cls)->n_info.n_xvect.xv_data[3+1] = (newvector(listlength(cvars))));
((cls)->n_info.n_xvect.xv_data[4+1] = (super));
n = listlength(ivars);
((cls)->n_info.n_xvect.xv_data[5+1] = (cvfixnum((long)n)));
n += getivcnt(super,6);
((cls)->n_info.n_xvect.xv_data[6+1] = (cvfixnum((long)n)));
return (cls);
}
static NODE *clanswer(NODE *args)
{
NODE ***oldstk,*arg,*msg,*fargs,*code,*obj,*mptr;
oldstk = xlsave(&arg,&msg,&fargs,&code,(NODE **)0);
arg = args;
msg = xlmatch(4,&arg);
fargs = xlmatch(3,&arg);
code = xlmatch(3,&arg);
xllastarg(arg);
obj = xlygetvalue(self);
mptr = entermsg(obj,msg);
((mptr)->n_info.n_xlist.xl_cdr = (cons(fargs,code)));
xlstack = oldstk;
return (obj);
}
static NODE *entermsg(NODE *cls, NODE *msg)
{
NODE ***oldstk,*lptr,*mptr;
for (lptr = ((cls)->n_info.n_xvect.xv_data[0+1]); lptr; lptr = ((lptr)->n_info.n_xlist.xl_cdr))
if (((mptr = ((lptr)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_car) == msg)
return (mptr);
oldstk = xlsave(&mptr,(NODE **)0);
mptr = consa(msg);
((cls)->n_info.n_xvect.xv_data[0+1] = (cons(mptr,((cls)->n_info.n_xvect.xv_data[0+1]))));
xlstack = oldstk;
return (mptr);
}
static NODE *findmsg(NODE *cls, NODE *sym)
{
NODE *lptr,*msg;
for (msgcls = cls; msgcls != (NODE *)0; ) {
for (lptr = ((msgcls)->n_info.n_xvect.xv_data[0+1]); lptr != (NODE *)0; lptr = ((lptr)->n_info.n_xlist.xl_cdr))
if ((msg = ((lptr)->n_info.n_xlist.xl_car)) != (NODE *)0 && ((msg)->n_info.n_xlist.xl_car) == sym)
return (msg);
msgcls = ((msgcls)->n_info.n_xvect.xv_data[4+1]);
}
return ((NODE *)0);
}
static NODE *sendmsg(NODE *obj, NODE *msg, NODE *args)
{
NODE ***oldstk,*oldenv,*newenv,*method,*cptr,*val,*isnewmsg;
oldstk = xlsave(&oldenv,&newenv,&method,&cptr,&val,(NODE **)0);
method = ((msg)->n_info.n_xlist.xl_cdr);
if (!((method) && (method)->n_type == 1) && !((method) && (method)->n_type == 3))
xlfail("bad method");
newenv = xlframe((NODE *)0);
oldenv = xlenv;
xlbind(self,obj,newenv);
xlbind(msgclass,msgcls,newenv);
if (((method) && (method)->n_type == 1)) {
xlenv = newenv;
val = (*((method)->n_info.n_xsubr.xsu_subr))(args);
}
else {
xlabind(((method)->n_info.n_xlist.xl_car),args,newenv);
xlenv = newenv;
cptr = ((method)->n_info.n_xlist.xl_cdr);
while (cptr)
val = xlevarg(&cptr);
}
xlenv = oldenv;
if (((msg)->n_info.n_xlist.xl_car) == new && val) {
if ((isnewmsg = findmsg(((val)->n_info.n_xvect.xv_data[0]),isnew)) == (NODE *)0)
xlfail("no method for the isnew message");
sendmsg(val,isnewmsg,args);
}
xlstack = oldstk;
return (val);
}
static int getivcnt(NODE *cls, int ivar)
{
NODE *cnt;
if ((cnt = ((cls)->n_info.n_xvect.xv_data[ivar+1])) == (NODE *)0 || !((cnt) && (cnt)->n_type == 5))
xlfail("bad value for instance variable count");
return ((int)((cnt)->n_info.n_xint.xi_int));
}
static int listlength(NODE *list)
{
int len;
for (len = 0; ((list) && (list)->n_type == 3); len++)
list = ((list)->n_info.n_xlist.xl_cdr);
return (len);
}
xloinit(void)
{
class = object = (NODE *)0;
self	= xlsenter("SELF");
msgclass	= xlsenter("MSGCLASS");
new	= xlsenter(":NEW");
isnew	= xlsenter(":ISNEW");
class = xlclass("CLASS",7);
((class)->n_info.n_xvect.xv_data[0] = (class));
object = xlclass("OBJECT",0);
((class)->n_info.n_xvect.xv_data[4+1] = (object));
xladdivar(class,"IVARTOTAL");	xladdivar(class,"IVARCNT");	xladdivar(class,"SUPERCLASS");	xladdivar(class,"CVALS");	xladdivar(class,"CVARS");	xladdivar(class,"IVARS");	xladdivar(class,"MESSAGES");	xladdmsg(class,":NEW",clnew);
xladdmsg(class,":ISNEW",clisnew);
xladdmsg(class,":ANSWER",clanswer);
xladdmsg(object,":ISNEW",obisnew);
xladdmsg(object,":CLASS",obclass);
xladdmsg(object,":SHOW",obshow);
xladdmsg(object,":SENDSUPER",obsendsuper);
}
