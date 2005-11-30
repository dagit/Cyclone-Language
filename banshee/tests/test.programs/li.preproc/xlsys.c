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
extern int anodes;
extern NODE *a_subr,*a_fsubr;
extern NODE *a_list,*a_sym,*a_int,*a_float,*a_str,*a_obj,*a_fptr,*a_vect;
extern NODE *true;
NODE *xload(NODE *args)
{
NODE ***oldstk,*fname,*val;
int vflag,pflag;
char *name;
oldstk = xlsave(&fname,(NODE **)0);
fname = xlarg(&args);
vflag = (args ? xlarg(&args) != (NODE *)0 : 1);
pflag = (args ? xlarg(&args) != (NODE *)0 : 0);
xllastarg(args);
if (((fname) && (fname)->n_type == 4))
name = ((((fname)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_car))->n_info.n_xstr.xst_str);
else if (((fname) && (fname)->n_type == 6))
name = ((fname)->n_info.n_xstr.xst_str);
else
xlfail( fname);
val = (xlload(name,vflag,pflag) ? true : (NODE *)0);
xlstack = oldstk;
return (val);
}
NODE *xgc(NODE *args)
{
xllastarg(args);
gc();
return ((NODE *)0);
}
NODE *xexpand(NODE *args)
{
int n,i;
n = (args ? ((xlmatch(5,&args))->n_info.n_xint.xi_int) : 1);
xllastarg(args);
for (i = 0; i < n; i++)
if (!addseg())
break;
return (cvfixnum((long)i));
}
NODE *xalloc(NODE *args)
{
int n,oldn;
n = ((xlmatch(5,&args))->n_info.n_xint.xi_int);
xllastarg(args);
oldn = anodes;
anodes = n;
return (cvfixnum((long)oldn));
}
NODE *xmem(NODE *args)
{
xllastarg(args);
stats();
return ((NODE *)0);
}
NODE *xtype(NODE *args)
{
NODE *arg;
if (!(arg = xlarg(&args)))
return ((NODE *)0);
switch (((arg)->n_type)) {
case 1:	return (a_subr);
case 2:	return (a_fsubr);
case 3:	return (a_list);
case 4:	return (a_sym);
case 5:	return (a_int);
case 9:	return (a_float);
case 6:	return (a_str);
case 7:	return (a_obj);
case 8:	return (a_fptr);
case 10:	return (a_vect);
default:	xlfail("bad node type");
}
}
NODE *xbaktrace(NODE *args)
{
int n;
n = (args ? ((xlmatch(5,&args))->n_info.n_xint.xi_int) : -1);
xllastarg(args);
xlbaktrace(n);
return ((NODE *)0);
}
NODE *xexit(NODE *args)
{
xllastarg(args);
osfinish ();
exit(0);
}
