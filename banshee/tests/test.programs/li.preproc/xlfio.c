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
extern NODE *s_stdin,*s_stdout,*true;
extern NODE ***xlstack;
extern int xlfsize;
extern char buf[];
extern struct _iobuf *fopen(const char *, const char *);
static NODE *printit();
static NODE *flatsize();
static NODE *openit();
NODE *xread(NODE *args)
{
NODE ***oldstk,*fptr,*eof,*rflag,*val;
oldstk = xlsave(&fptr,&eof,(NODE **)0);
fptr = (args ? xlgetfile(&args) : ((s_stdin)->n_info.n_xsym.xsy_value));
eof = (args ? xlarg(&args) : (NODE *)0);
rflag = (args ? xlarg(&args) : (NODE *)0);
xllastarg(args);
if (!xlread(fptr,&val,rflag != (NODE *)0))
val = eof;
xlstack = oldstk;
return (val);
}
NODE *xprint(NODE *args)
{
return (printit(args,1,1));
}
NODE *xprin1(NODE *args)
{
return (printit(args,1,0));
}
NODE *xprinc(NODE *args)
{
return (printit(args,0,0));
}
NODE *xterpri(NODE *args)
{
NODE *fptr;
fptr = (args ? xlgetfile(&args) : ((s_stdout)->n_info.n_xsym.xsy_value));
xllastarg(args);
xlterpri(fptr);
return ((NODE *)0);
}
static NODE *printit(NODE *args, int pflag, int tflag)
{
NODE ***oldstk,*fptr,*val;
oldstk = xlsave(&fptr,&val,(NODE **)0);
val = xlarg(&args);
fptr = (args ? xlgetfile(&args) : ((s_stdout)->n_info.n_xsym.xsy_value));
xllastarg(args);
xlprint(fptr,val,pflag);
if (tflag)
xlterpri(fptr);
xlstack = oldstk;
return (val);
}
NODE *xflatsize(NODE *args)
{
return (flatsize(args,1));
}
NODE *xflatc(NODE *args)
{
return (flatsize(args,0));
}
static NODE *flatsize(NODE *args, int pflag)
{
NODE ***oldstk,*val;
oldstk = xlsave(&val,(NODE **)0);
val = xlarg(&args);
xllastarg(args);
xlfsize = 0;
xlprint((NODE *)0,val,pflag);
xlstack = oldstk;
return (cvfixnum((long)xlfsize));
}
NODE *xopeni(NODE *args)
{
return (openit(args,"r"));
}
NODE *xopeno(NODE *args)
{
return (openit(args,"w"));
}
static NODE *openit(NODE *args, char *mode)
{
NODE *fname,*val;
char *name;
struct _iobuf *fp;
fname = xlarg(&args);
xllastarg(args);
if (((fname) && (fname)->n_type == 4))
name = ((((fname)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_car))->n_info.n_xstr.xst_str);
else if (((fname) && (fname)->n_type == 6))
name = ((fname)->n_info.n_xstr.xst_str);
else
xlfail( fname); if ((fp = fopen(name,mode)) != 0)
val = cvfile(fp);
else
val = (NODE *)0;
return (val);
}
NODE *xclose(NODE *args)
{
NODE *fptr;
fptr = xlmatch(8,&args);
xllastarg(args);
if (((fptr)->n_info.n_xfptr.xf_fp) == 0)
xlfail("file not open");
fclose(((fptr)->n_info.n_xfptr.xf_fp));
((fptr)->n_info.n_xfptr.xf_fp = (0));
return ((NODE *)0);
}
NODE *xrdchar(NODE *args)
{
NODE *fptr;
int ch;
fptr = (args ? xlgetfile(&args) : ((s_stdin)->n_info.n_xsym.xsy_value));
xllastarg(args);
return ((ch = xlgetc(fptr)) == (-1) ? (NODE *)0 : cvfixnum((long)ch));
}
NODE *xpkchar(NODE *args)
{
NODE *flag,*fptr;
int ch;
flag = (args ? xlarg(&args) : (NODE *)0);
fptr = (args ? xlgetfile(&args) : ((s_stdin)->n_info.n_xsym.xsy_value));
xllastarg(args);
if (flag)
while ((ch = xlpeek(fptr)) != (-1) && ((_ctype_+1)[ch]&010))
xlgetc(fptr);
else
ch = xlpeek(fptr);
return (ch == (-1) ? (NODE *)0 : cvfixnum((long)ch));
}
NODE *xwrchar(NODE *args)
{
NODE *fptr,*chr;
chr = xlmatch(5,&args);
fptr = (args ? xlgetfile(&args) : ((s_stdout)->n_info.n_xsym.xsy_value));
xllastarg(args);
xlputc(fptr,(int)((chr)->n_info.n_xint.xi_int));
return (chr);
}
NODE *xreadline(NODE *args)
{
NODE ***oldstk,*fptr,*str,*newstr;
int len,blen,ch;
char *p,*sptr;
oldstk = xlsave(&fptr,&str,(NODE **)0);
fptr = (args ? xlgetfile(&args) : ((s_stdin)->n_info.n_xsym.xsy_value));
xllastarg(args);
len = blen = 0; p = buf;
while ((ch = xlgetc(fptr)) != (-1) && ch != '\n') {
if (blen >= 100) {
newstr = newstring(len+100);
sptr = ((newstr)->n_info.n_xstr.xst_str); *sptr = 0;
if (str) strcat(sptr,((str)->n_info.n_xstr.xst_str));
*p = 0; strcat(sptr,buf);
p = buf; blen = 0;
len += 100;
str = newstr;
}
*p++ = ch; blen++;
}
if (len == 0 && p == buf && ch == (-1)) {
xlstack = oldstk;
return ((NODE *)0);
}
if (str == (NODE *)0 || blen) {
newstr = newstring(len+blen);
sptr = ((newstr)->n_info.n_xstr.xst_str); *sptr = 0;
if (str) strcat(sptr,((str)->n_info.n_xstr.xst_str));
*p = 0; strcat(sptr,buf);
str = newstr;
}
xlstack = oldstk;
return (str);
}
