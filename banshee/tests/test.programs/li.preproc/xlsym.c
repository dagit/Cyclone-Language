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
extern struct node ***xlsave(NODE ** arg1, ...);	extern struct node *xleval();	extern struct node *xlapply();	extern struct node *xlevlist();	extern struct node *xlarg();	extern struct node *xlevarg();	extern struct node *xlmatch();	extern struct node *xlevmatch();	extern struct node *xlgetfile();	extern struct node *xlsend();	extern struct node *xlenter(char *name, int type);	extern struct node *xlsenter(char *name);	extern struct node *xlmakesym(char *name, int type);	extern struct node *xlframe(NODE *env);	extern struct node *xlgetvalue(NODE *sym);	extern struct node *xlxgetvalue(NODE *sym);	extern struct node *xlygetvalue(NODE *sym);	extern struct node *cons();	extern struct node *consa();	extern struct node *consd();	extern struct node *cvsymbol();	extern struct node *cvcsymbol();	extern struct node *cvstring();	extern struct node *cvcstring();	extern struct node *cvfile();	extern struct node *cvsubr();	extern struct node *cvfixnum();	extern struct node *cvflonum();	extern struct node *newstring();	extern struct node *newvector();	extern struct node *newobject();	extern struct node *xlgetprop(NODE *sym, NODE *prp);	extern char *xlsymname();	extern void xlsetvalue(NODE *sym, NODE *val);
extern void xlprint();
extern void xltest();
extern NODE *obarray,*s_unbound,*self;
extern NODE ***xlstack,*xlenv;
static NODE *findprop();
NODE *xlenter(char *name, int type)
{
NODE ***oldstk,*sym,*array;
int i;
if (strcmp(name,"NIL") == 0)
return ((NODE *)0);
array = ((obarray)->n_info.n_xsym.xsy_value);
i = hash(name,199);
for (sym = ((array)->n_info.n_xvect.xv_data[i]); sym; sym = ((sym)->n_info.n_xlist.xl_cdr))
if (strcmp(name,((((((sym)->n_info.n_xlist.xl_car))->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_car))->n_info.n_xstr.xst_str)) == 0)
return (((sym)->n_info.n_xlist.xl_car));
oldstk = xlsave(&sym,(NODE **)0);
sym = consd(((array)->n_info.n_xvect.xv_data[i]));
((sym)->n_info.n_xlist.xl_car = (xlmakesym(name,type)));
((array)->n_info.n_xvect.xv_data[i] = (sym));
xlstack = oldstk;
return (((sym)->n_info.n_xlist.xl_car));
}
NODE *xlsenter(char *name)
{
return (xlenter(name,1));
}
NODE *xlmakesym(char *name, int type)
{
NODE *sym;
sym = (type == 0 ? cvsymbol(name) : cvcsymbol(name));
((sym)->n_info.n_xsym.xsy_value = (*name == ':' ? sym : s_unbound));
return (sym);
}
NODE *xlframe(NODE *env)
{
return (consd(env));
}
xlbind(NODE *sym, NODE *val, NODE *env)
{
NODE *ptr;
ptr = consd(((env)->n_info.n_xlist.xl_car));
((env)->n_info.n_xlist.xl_car = (ptr));
((ptr)->n_info.n_xlist.xl_car = (cons(sym,val)));
}
NODE *xlgetvalue(NODE *sym)
{
register NODE *val;
while ((val = xlxgetvalue(sym)) == s_unbound)
xlunbound(sym);
return (val);
}
NODE *xlxgetvalue(NODE *sym)
{
register NODE *fp,*ep;
NODE *val;
if (((self)->n_info.n_xsym.xsy_value) && xlobgetvalue(sym,&val))
return (val);
for (fp = xlenv; fp; fp = ((fp)->n_info.n_xlist.xl_cdr))
for (ep = ((fp)->n_info.n_xlist.xl_car); ep; ep = ((ep)->n_info.n_xlist.xl_cdr))
if (sym == ((((ep)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_car))
return (((((ep)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_cdr));
return (((sym)->n_info.n_xsym.xsy_value));
}
NODE *xlygetvalue(NODE *sym)
{
register NODE *fp,*ep;
for (fp = xlenv; fp; fp = ((fp)->n_info.n_xlist.xl_cdr))
for (ep = ((fp)->n_info.n_xlist.xl_car); ep; ep = ((ep)->n_info.n_xlist.xl_cdr))
if (sym == ((((ep)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_car))
return (((((ep)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_cdr));
return (((sym)->n_info.n_xsym.xsy_value));
}
void xlsetvalue(NODE *sym, NODE *val)
{
register NODE *fp,*ep;
if (((self)->n_info.n_xsym.xsy_value) && xlobsetvalue(sym,val))
return;
for (fp = xlenv; fp; fp = ((fp)->n_info.n_xlist.xl_cdr))
for (ep = ((fp)->n_info.n_xlist.xl_car); ep; ep = ((ep)->n_info.n_xlist.xl_cdr))
if (sym == ((((ep)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_car)) {
((((ep)->n_info.n_xlist.xl_car))->n_info.n_xlist.xl_cdr = (val));
return;
}
((sym)->n_info.n_xsym.xsy_value = (val));
}
NODE *xlgetprop(NODE *sym, NODE *prp)
{
NODE *p;
return ((p = findprop(sym,prp)) ? ((p)->n_info.n_xlist.xl_car) : (NODE *)0);
}
xlputprop(NODE *sym, NODE *val, NODE *prp)
{
NODE ***oldstk,*p,*pair;
if ((pair = findprop(sym,prp)) == (NODE *)0) {
oldstk = xlsave(&p,(NODE **)0);
p = consa(prp);
((p)->n_info.n_xlist.xl_cdr = (pair = cons(val,((sym)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_cdr))));
((sym)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_cdr = (p));
xlstack = oldstk;
}
((pair)->n_info.n_xlist.xl_car = (val));
}
xlremprop(NODE *sym, NODE *prp)
{
NODE *last,*p;
last = (NODE *)0;
for (p = ((sym)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_cdr); ((p) && (p)->n_type == 3) && ((((p)->n_info.n_xlist.xl_cdr)) && (((p)->n_info.n_xlist.xl_cdr))->n_type == 3); p = ((last)->n_info.n_xlist.xl_cdr)) {
if (((p)->n_info.n_xlist.xl_car) == prp)
if (last)
((last)->n_info.n_xlist.xl_cdr = (((((p)->n_info.n_xlist.xl_cdr))->n_info.n_xlist.xl_cdr)));
else
((sym)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_cdr = (((((p)->n_info.n_xlist.xl_cdr))->n_info.n_xlist.xl_cdr)));
last = ((p)->n_info.n_xlist.xl_cdr);
}
}
static NODE *findprop(NODE *sym, NODE *prp)
{
NODE *p;
for (p = ((sym)->n_info.n_xsym.xsy_plist->n_info.n_xlist.xl_cdr); ((p) && (p)->n_type == 3) && ((((p)->n_info.n_xlist.xl_cdr)) && (((p)->n_info.n_xlist.xl_cdr))->n_type == 3); p = ((((p)->n_info.n_xlist.xl_cdr))->n_info.n_xlist.xl_cdr))
if (((p)->n_info.n_xlist.xl_car) == prp)
return (((p)->n_info.n_xlist.xl_cdr));
return ((NODE *)0);
}
int hash(char *str, int len)
{
int i;
for (i = 0; *str; )
i = (i << 2) ^ *str++;
i %= len;
return (((i) < 0 ? -(i) : (i)));
}
xlsinit(void)
{
NODE *array,*p;
obarray = xlmakesym("*OBARRAY*",1);
array = newvector(199);
((obarray)->n_info.n_xsym.xsy_value = (array));
p = consa(obarray);
((array)->n_info.n_xvect.xv_data[hash("*OBARRAY*",199)] = (p));
s_unbound = xlsenter("*UNBOUND*");
((s_unbound)->n_info.n_xsym.xsy_value = (s_unbound));
}
