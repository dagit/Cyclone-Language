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
extern void xlprint(NODE *fptr, NODE *vptr, int flag);
extern void xltest();
extern char buf[];
void xlprint(NODE *fptr, NODE *vptr, int flag)
{
NODE *nptr;
NODE *next = (NODE *)0;
int n,i;
if (vptr == (NODE *)0) {
xlputstr(fptr,"NIL");
return;
}
switch (((vptr)->n_type)) {
case 1:
putatm(fptr,"Subr",vptr);
break;
case 2:
putatm(fptr,"FSubr",vptr);
break;
case 3:
xlputc(fptr,'(');
for (nptr = vptr; nptr != (NODE *)0; nptr = next) {
xlprint(fptr,((nptr)->n_info.n_xlist.xl_car),flag);
if (next = ((nptr)->n_info.n_xlist.xl_cdr))
if (((next) && (next)->n_type == 3))
xlputc(fptr,' ');
else {
xlputstr(fptr," . ");
xlprint(fptr,next,flag);
break;
}
}
xlputc(fptr,')');
break;
case 4:
xlputstr(fptr,((((vptr)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_car))->n_info.n_xstr.xst_str));
break;
case 5:
putdec(fptr,((vptr)->n_info.n_xint.xi_int));
break;
case 9:
putfloat(fptr,((vptr)->n_info.n_xfloat.xf_float));
break;
case 6:
if (flag)
putstring(fptr,((vptr)->n_info.n_xstr.xst_str));
else
xlputstr(fptr,((vptr)->n_info.n_xstr.xst_str));
break;
case 8:
putatm(fptr,"File",vptr);
break;
case 7:
putatm(fptr,"Object",vptr);
break;
case 10:
xlputc(fptr,'#'); xlputc(fptr,'(');
for (i = 0, n = ((vptr)->n_info.n_xvect.xv_size); n-- > 0; ) {
xlprint(fptr,((vptr)->n_info.n_xvect.xv_data[i++]),flag);
if (n) xlputc(fptr,' ');
}
xlputc(fptr,')');
break;
case 0:
putatm(fptr,"Free",vptr);
break;
default:
putatm(fptr,"Foo",vptr);
break;
}
}
xlterpri(NODE *fptr)
{
xlputc(fptr,'\n');
}
xlputstr(NODE *fptr, char *str)
{
while (*str)
xlputc(fptr,*str++);
}
static putstring(NODE *fptr, char *str)
{
int ch;
xlputc(fptr,'"');
while (ch = *str++)
if (ch < 040 || ch == '\\') {
xlputc(fptr,'\\');
switch (ch) {
case '\033':
xlputc(fptr,'e');
break;
case '\n':
xlputc(fptr,'n');
break;
case '\r':
xlputc(fptr,'r');
break;
case '\t':
xlputc(fptr,'t');
break;
case '\\':
xlputc(fptr,'\\');
break;
default:
putoct(fptr,ch);
break;
}
}
else
xlputc(fptr,ch);
xlputc(fptr,'"');
}
static putatm(NODE *fptr, char *tag, NODE *val)
{
sprintf(buf,"#<%s: #",tag); xlputstr(fptr,buf);
sprintf(buf,"%x",val); xlputstr(fptr,buf);
xlputc(fptr,'>');
}
static putdec(NODE *fptr, long int n)
{
sprintf(buf,"%ld",n);
xlputstr(fptr,buf);
}
static putfloat(NODE *fptr, float n)
{
sprintf(buf,"%g",n);
xlputstr(fptr,buf);
}
static putoct(NODE *fptr, int n)
{
sprintf(buf,"%03o",n);
xlputstr(fptr,buf);
}
