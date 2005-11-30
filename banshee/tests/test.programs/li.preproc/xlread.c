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
extern NODE *s_stdout,*true,*s_dot;
extern NODE *s_quote,*s_function,*s_bquote,*s_comma,*s_comat;
extern NODE *s_rtable,*k_wspace,*k_const,*k_nmacro,*k_tmacro;
extern NODE ***xlstack;
extern int xlplevel;
extern char buf[];
extern struct _iobuf *fopen(const char *, const char *);
extern double atof(const char *);
extern long atol(const char *);
NODE *callmacro(NODE *fptr, int ch);
static NODE *phexnumber(),*pquote(),*plist(),*pvector(),*pname();
static NODE *tentry();
int xlload(char *fname, int vflag, int pflag)
{
NODE ***oldstk,*fptr,*expr;
char fullname[100+1];
CONTEXT cntxt;
struct _iobuf *fp;
int sts;
oldstk = xlsave(&fptr,&expr,(NODE **)0);
if (needsextension(fname)) {
strcpy(fullname,fname);
strcat(fullname,".lsp");
fname = fullname;
}
fptr = cvfile(0);
if (vflag)
{ sprintf(buf,"; loading \"%s\"\n",fname); stdputstr(buf); }
if ((fp = fopen(fname,"r")) == 0) {
xlstack = oldstk;
return (0);
}
((fptr)->n_info.n_xfptr.xf_fp = (fp));
xlbegin(&cntxt,8,true);
if (setjmp(cntxt.c_jmpbuf))
sts = 0;
else {
while (xlread(fptr,&expr,0)) {
expr = xleval(expr);
if (pflag)
stdprint(expr);
}
sts = 1;
}
xlend(&cntxt);
fclose(((fptr)->n_info.n_xfptr.xf_fp));
((fptr)->n_info.n_xfptr.xf_fp = (0));
xlstack = oldstk;
return (sts);
}
int xlread(NODE *fptr, NODE **pval, int rflag)
{
int sts;
if (!rflag)
xlplevel = 0;
while ((sts = readone(fptr,pval)) == 0)
;
return (sts == (-1) ? 0 : 1);
}
int readone(NODE *fptr, NODE **pval)
{
NODE *val,*type;
int ch;
if ((ch = xlgetc(fptr)) == (-1))
return ((-1));
if ((type = tentry(ch)) == k_wspace)
return (0);
else if (type == k_const) {
*pval = pname(fptr,ch);
return (1);
}
else if (((type) && (type)->n_type == 3)) {
if ((val = callmacro(fptr,ch)) && ((val) && (val)->n_type == 3)) {
*pval = ((val)->n_info.n_xlist.xl_car);
return (1);
}
else
return (0);
}
else
xlerror("illegal character",cvfixnum((long)ch));
}
NODE *rmhash(NODE *args)
{
NODE ***oldstk,*fptr,*mch,*val;
int ch;
oldstk = xlsave(&fptr,&mch,&val,(NODE **)0);
fptr = xlgetfile(&args);
mch = xlmatch(5,&args);
xllastarg(args);
val = consa((NODE *)0);
switch (ch = xlgetc(fptr)) {
case '\'':
((val)->n_info.n_xlist.xl_car = (pquote(fptr,s_function)));
break;
case '(':
((val)->n_info.n_xlist.xl_car = (pvector(fptr)));
break;
case 'x':
case 'X':
((val)->n_info.n_xlist.xl_car = (phexnumber(fptr)));
break;
case '\\':
((val)->n_info.n_xlist.xl_car = (cvfixnum((long)xlgetc(fptr))));
break;
default:
xlerror("illegal character after #",cvfixnum((long)ch));
}
xlstack = oldstk;
return (val);
}
NODE *rmquote(NODE *args)
{
NODE ***oldstk,*fptr,*mch,*val;
oldstk = xlsave(&fptr,&mch,&val,(NODE **)0);
fptr = xlgetfile(&args);
mch = xlmatch(5,&args);
xllastarg(args);
val = consa((NODE *)0);
((val)->n_info.n_xlist.xl_car = (pquote(fptr,s_quote)));
xlstack = oldstk;
return (val);
}
NODE *rmdquote(NODE *args)
{
NODE ***oldstk,*fptr,*mch,*val;
int ch,i,d1,d2,d3;
oldstk = xlsave(&fptr,&mch,&val,(NODE **)0);
fptr = xlgetfile(&args);
mch = xlmatch(5,&args);
xllastarg(args);
for (i = 0; i < 100 && (ch = checkeof(fptr)) != '"'; i++) {
switch (ch) {
case '\\':
switch (ch = checkeof(fptr)) {
case 'f':
ch = '\f';
break;
case 'n':
ch = '\n';
break;
case 'r':
ch = '\r';
break;
case 't':
ch = '\t';
break;
default:
if (ch >= '0' && ch <= '7') {
d1 = ch - '0';
d2 = checkeof(fptr) - '0';
d3 = checkeof(fptr) - '0';
ch = (d1 << 6) + (d2 << 3) + d3;
}
break;
}
}
buf[i] = ch;
}
buf[i] = 0;
val = consa((NODE *)0);
((val)->n_info.n_xlist.xl_car = (cvstring(buf)));
xlstack = oldstk;
return (val);
}
NODE *rmbquote(NODE *args)
{
NODE ***oldstk,*fptr,*mch,*val;
oldstk = xlsave(&fptr,&mch,&val,(NODE **)0);
fptr = xlgetfile(&args);
mch = xlmatch(5,&args);
xllastarg(args);
val = consa((NODE *)0);
((val)->n_info.n_xlist.xl_car = (pquote(fptr,s_bquote)));
xlstack = oldstk;
return (val);
}
NODE *rmcomma(NODE *args)
{
NODE ***oldstk,*fptr,*mch,*val,*sym;
oldstk = xlsave(&fptr,&mch,&val,(NODE **)0);
fptr = xlgetfile(&args);
mch = xlmatch(5,&args);
xllastarg(args);
if (xlpeek(fptr) == '@') {
sym = s_comat;
xlgetc(fptr);
}
else
sym = s_comma;
val = consa((NODE *)0);
((val)->n_info.n_xlist.xl_car = (pquote(fptr,sym)));
xlstack = oldstk;
return (val);
}
NODE *rmlpar(NODE *args)
{
NODE ***oldstk,*fptr,*mch,*val;
oldstk = xlsave(&fptr,&mch,&val,(NODE **)0);
fptr = xlgetfile(&args);
mch = xlmatch(5,&args);
xllastarg(args);
val = consa((NODE *)0);
((val)->n_info.n_xlist.xl_car = (plist(fptr)));
xlstack = oldstk;
return (val);
}
NODE *rmrpar(NODE *args)
{
xlfail("misplaced right paren");
}
NODE *rmsemi(NODE *args)
{
NODE ***oldstk,*fptr,*mch;
int ch;
oldstk = xlsave(&fptr,&mch,(NODE **)0);
fptr = xlgetfile(&args);
mch = xlmatch(5,&args);
xllastarg(args);
while ((ch = xlgetc(fptr)) != (-1) && ch != '\n')
;
xlstack = oldstk;
return ((NODE *)0);
}
static NODE *phexnumber(NODE *fptr)
{
long num;
int ch;
num = 0L;
while ((ch = xlpeek(fptr)) != (-1)) {
if (((_ctype_+1)[ch]&02)) ch = toupper(ch);
if (!((_ctype_+1)[ch]&04) && !(ch >= 'A' && ch <= 'F'))
break;
xlgetc(fptr);
num = num * 16L + (long)(ch <= '9' ? ch - '0' : ch - 'A' + 10);
}
return (cvfixnum((long)num));
}
static NODE *plist(NODE *fptr)
{
NODE ***oldstk,*val,*expr,*lastnptr;
NODE *nptr = (NODE *)0;
oldstk = xlsave(&val,&expr,(NODE **)0);
++xlplevel;
lastnptr = (NODE *)0;
for (lastnptr = (NODE *)0; nextch(fptr) != ')'; lastnptr = nptr)
switch (readone(fptr,&expr)) {
case (-1):
badeof(fptr);
case 1:
if (expr == s_dot) {
if (lastnptr == (NODE *)0)
xlfail("invalid dotted pair");
if (!xlread(fptr,&expr,1))
badeof(fptr);
((lastnptr)->n_info.n_xlist.xl_cdr = (expr));
if (nextch(fptr) != ')')
xlfail("invalid dotted pair");
break;
}
else {
nptr = consa(expr);
if (lastnptr == (NODE *)0)
val = nptr;
else
((lastnptr)->n_info.n_xlist.xl_cdr = (nptr));
}
break;
}
xlgetc(fptr);
--xlplevel;
xlstack = oldstk;
return (val);
}
static NODE *pvector(NODE *fptr)
{
NODE ***oldstk,*list,*expr,*val,*lastnptr;
NODE *nptr = (NODE *)0;
int len,ch,i;
oldstk = xlsave(&list,&expr,(NODE **)0);
lastnptr = (NODE *)0; len = 0;
for (lastnptr = (NODE *)0; (ch = nextch(fptr)) != ')'; lastnptr = nptr) {
if (ch == (-1))
badeof(fptr);
switch (readone(fptr,&expr)) {
case (-1):
badeof(fptr);
case 1:
nptr = consa(expr);
if (lastnptr == (NODE *)0)
list = nptr;
else
((lastnptr)->n_info.n_xlist.xl_cdr = (nptr));
len++;
break;
}
}
xlgetc(fptr);
val = newvector(len);
for (i = 0; i < len; ++i, list = ((list)->n_info.n_xlist.xl_cdr))
((val)->n_info.n_xvect.xv_data[i] = (((list)->n_info.n_xlist.xl_car)));
xlstack = oldstk;
return (val);
}
static NODE *pquote(NODE *fptr, NODE *sym)
{
NODE ***oldstk,*val,*p;
oldstk = xlsave(&val,(NODE **)0);
val = consa(sym);
((val)->n_info.n_xlist.xl_cdr = (consa((NODE *)0)));
if (!xlread(fptr,&p,1))
badeof(fptr);
((((val)->n_info.n_xlist.xl_cdr))->n_info.n_xlist.xl_car = (p));
xlstack = oldstk;
return (val);
}
static NODE *pname(NODE *fptr, int ch)
{
NODE *val,*type;
int i;
for (i = 0; ; xlgetc(fptr)) {
if (i < 100)
buf[i++] = (((_ctype_+1)[ch]&02) ? toupper(ch) : ch);
if ((ch = xlpeek(fptr)) == (-1) ||
((type = tentry(ch)) != k_const &&
!(((type) && (type)->n_type == 3) && ((type)->n_info.n_xlist.xl_car) == k_nmacro)))
break;
}
buf[i] = 0;
return (isnumber(buf,&val) ? val : xlenter(buf,0));
}
static NODE *tentry(int ch)
{
NODE *rtable;
rtable = ((s_rtable)->n_info.n_xsym.xsy_value);
if (!((rtable) && (rtable)->n_type == 10) || ch < 0 || ch >= ((rtable)->n_info.n_xvect.xv_size))
return ((NODE *)0);
return (((rtable)->n_info.n_xvect.xv_data[ch]));
}
static int nextch(NODE *fptr)
{
int ch;
while ((ch = xlpeek(fptr)) != (-1) && ((_ctype_+1)[ch]&010))
xlgetc(fptr);
return (ch);
}
static int checkeof(NODE *fptr)
{
int ch;
if ((ch = xlgetc(fptr)) == (-1))
badeof(fptr);
return (ch);
}
static badeof(NODE *fptr)
{
xlgetc(fptr);
xlfail("unexpected EOF");
}
int isnumber(char *str, NODE **pval)
{
int dl,dr;
char *p;
p = str; dl = dr = 0;
if (*p == '+' || *p == '-')
p++;
while (((_ctype_+1)[*p]&04))
p++, dl++;
if (*p == '.') {
p++;
while (((_ctype_+1)[*p]&04))
p++, dr++;
}
if ((dl || dr) && *p == 'E') {
p++;
if (*p == '+' || *p == '-')
p++;
while (((_ctype_+1)[*p]&04))
p++, dr++;
}
if ((dl == 0 && dr == 0) || *p)
return (0);
if (*str == '+') ++str;
if (str[strlen(str)-1] == '.') str[strlen(str)-1] = 0;
*pval = (dr ? cvflonum(atof(str)) : cvfixnum(atol(str)));
return (1);
}
defmacro(int ch, NODE *type, NODE *(*fun) ())
{
NODE *p;
p = consa(type);
((((s_rtable)->n_info.n_xsym.xsy_value))->n_info.n_xvect.xv_data[ch] = (p));
((p)->n_info.n_xlist.xl_cdr = (cvsubr(fun,1)));
}
NODE *callmacro(NODE *fptr, int ch)
{
NODE ***oldstk,*fun,*args,*val;
oldstk = xlsave(&fun,&args,(NODE **)0);
fun = ((((((s_rtable)->n_info.n_xsym.xsy_value))->n_info.n_xvect.xv_data[ch]))->n_info.n_xlist.xl_cdr);
args = consa(fptr);
((args)->n_info.n_xlist.xl_cdr = (consa((NODE *)0)));
((((args)->n_info.n_xlist.xl_cdr))->n_info.n_xlist.xl_car = (cvfixnum((long)ch)));
val = xlapply(fun,args);
xlstack = oldstk;
return (val);
}
int needsextension(char *name)
{
while (*name)
if (*name++ == '.')
return (0);
return (1);
}
xlrinit(void)
{
NODE *rtable;
char *p;
int ch;
rtable = newvector(256);
((s_rtable)->n_info.n_xsym.xsy_value = (rtable));
for (p = "\t \f\r\n"; ch = *p++; )
((rtable)->n_info.n_xvect.xv_data[ch] = (k_wspace));
for (p = "!$%&*+-./0123456789:<=>?@[]^_{}~"; ch = *p++; )
((rtable)->n_info.n_xvect.xv_data[ch] = (k_const));
for (p = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"; ch = *p++; )
((rtable)->n_info.n_xvect.xv_data[ch] = (k_const));
defmacro('#', k_nmacro,rmhash);
defmacro('\'',k_tmacro,rmquote);
defmacro('"', k_tmacro,rmdquote);
defmacro('`', k_tmacro,rmbquote);
defmacro(',', k_tmacro,rmcomma);
defmacro('(', k_tmacro,rmlpar);
defmacro(')', k_tmacro,rmrpar);
defmacro(';', k_tmacro,rmsemi);
}
