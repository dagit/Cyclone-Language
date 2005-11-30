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
extern NODE *s_stdin,*s_stdout;
extern NODE *s_evalhook,*s_applyhook;
extern int xldebug;
extern NODE *true;
main(int argc, char **argv)
{
CONTEXT cntxt;
NODE *expr;
int i;
osinit("XLISP version 1.6, Copyright (c) 1985, by David Betz");
xlbegin(&cntxt,64|8,(NODE *) 1);
if (setjmp(cntxt.c_jmpbuf)) {
printf("fatal initialization error\n");
osfinish();
exit(1);
}
xlinit();
xlend(&cntxt);
xlbegin(&cntxt,64|8,true);
if (setjmp(cntxt.c_jmpbuf) == 0)
xlload("init.lsp",0,0);
if (setjmp(cntxt.c_jmpbuf) == 0)
for (i = 1; i < argc; i++)
if (!xlload(argv[i],1,0))
xlfail("can't load file");
xlsave(&expr,(NODE **)0);
while (1) {
if (i = setjmp(cntxt.c_jmpbuf)) {
if (i == 64)
stdputstr("[ back to the top level ]\n");
((s_evalhook)->n_info.n_xsym.xsy_value = ((NODE *)0));
((s_applyhook)->n_info.n_xsym.xsy_value = ((NODE *)0));
xldebug = 0;
xlflush();
}
if (!xlread(((s_stdin)->n_info.n_xsym.xsy_value),&expr,0))
break;
expr = xleval(expr);
stdprint(expr);
}
xlend(&cntxt);
osfinish ();
exit (0);
}
stdprint(NODE *expr)
{
xlprint(((s_stdout)->n_info.n_xsym.xsy_value),expr,1);
xlterpri(((s_stdout)->n_info.n_xsym.xsy_value));
}
stdputstr(char *str)
{
xlputstr(((s_stdout)->n_info.n_xsym.xsy_value),str);
}
