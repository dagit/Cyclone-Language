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
extern NODE
*xeval(),*xapply(),*xfuncall(),*xquote(),*xfunction(),*xbquote(),
*xlambda(),*xset(),*xsetq(),*xsetf(),*xdefun(),*xdefmacro(),
*xgensym(),*xmakesymbol(),*xintern(),
*xsymname(),*xsymvalue(),*xsymplist(),*xget(),*xputprop(),*xremprop(),
*xhash(),*xmkarray(),*xaref(),
*xcar(),*xcdr(),
*xcaar(),*xcadr(),*xcdar(),*xcddr(),
*xcaaar(),*xcaadr(),*xcadar(),*xcaddr(),
*xcdaar(),*xcdadr(),*xcddar(),*xcdddr(),
*xcaaaar(),*xcaaadr(),*xcaadar(),*xcaaddr(),
*xcadaar(),*xcadadr(),*xcaddar(),*xcadddr(),
*xcdaaar(),*xcdaadr(),*xcdadar(),*xcdaddr(),
*xcddaar(),*xcddadr(),*xcdddar(),*xcddddr(),
*xcons(),*xlist(),*xappend(),*xreverse(),*xlast(),*xnth(),*xnthcdr(),
*xmember(),*xassoc(),*xsubst(),*xsublis(),*xremove(),*xlength(),
*xmapc(),*xmapcar(),*xmapl(),*xmaplist(),
*xrplca(),*xrplcd(),*xnconc(),*xdelete(),
*xatom(),*xsymbolp(),*xnumberp(),*xboundp(),*xnull(),*xlistp(),*xconsp(),
*xeq(),*xeql(),*xequal(),
*xcond(),*xcase(),*xand(),*xor(),*xlet(),*xletstar(),*xif(),
*xprog(),*xprogstar(),*xprog1(),*xprog2(),*xprogn(),*xgo(),*xreturn(),
*xcatch(),*xthrow(),
*xerror(),*xcerror(),*xbreak(),*xcleanup(),*xcontinue(),*xerrset(),
*xbaktrace(),*xevalhook(),
*xdo(),*xdostar(),*xdolist(),*xdotimes(),
*xminusp(),*xzerop(),*xplusp(),*xevenp(),*xoddp(),
*xfix(),*xfloat(),
*xadd(),*xsub(),*xmul(),*xdiv(),*xrem(),*xmin(),*xmax(),*xabs(),
*xadd1(),*xsub1(),*xbitand(),*xbitior(),*xbitxor(),*xbitnot(),
*xsin(),*xcos(),*xtan(),*xexpt(),*xexp(),*xsqrt(),*xrand(),
*xlss(),*xleq(),*xequ(),*xneq(),*xgeq(),*xgtr(),
*xstrcat(),*xsubstr(),*xstring(),*xchar(),
*xread(),*xprint(),*xprin1(),*xprinc(),*xterpri(),
*xflatsize(),*xflatc(),
*xopeni(),*xopeno(),*xclose(),*xrdchar(),*xpkchar(),*xwrchar(),*xreadline(),
*xload(),*xgc(),*xexpand(),*xalloc(),*xmem(),*xtype(),*xexit();
struct fdef ftab[] = {
{	"EVAL",	1,	xeval	},
{	"APPLY",	1,	xapply	},
{	"FUNCALL",	1,	xfuncall	},
{	"QUOTE",	2,	xquote	},
{	"FUNCTION",	2,	xfunction	},
{	"BACKQUOTE",	2,	xbquote	},
{	"LAMBDA",	2,	xlambda	},
{	"SET",	1,	xset	},
{	"SETQ",	2,	xsetq	},
{	"SETF",	2,	xsetf	},
{	"DEFUN",	2,	xdefun	},
{	"DEFMACRO",	2,	xdefmacro	},
{	"GENSYM",	1,	xgensym	},
{	"MAKE-SYMBOL",	1,	xmakesymbol	},
{	"INTERN",	1,	xintern	},
{	"SYMBOL-NAME",	1,	xsymname	},
{	"SYMBOL-VALUE",	1,	xsymvalue	},
{	"SYMBOL-PLIST",	1,	xsymplist	},
{	"GET",	1,	xget	},
{	"PUTPROP",	1,	xputprop	},
{	"REMPROP",	1,	xremprop	},
{	"HASH",	1,	xhash	},
{	"MAKE-ARRAY",	1,	xmkarray	},
{	"AREF",	1,	xaref	},
{	"CAR",	1,	xcar	},
{	"CDR",	1,	xcdr	},
{	"CAAR",	1,	xcaar	},
{	"CADR",	1,	xcadr	},
{	"CDAR",	1,	xcdar	},
{	"CDDR",	1,	xcddr	},
{	"CAAAR",	1,	xcaaar	},
{	"CAADR",	1,	xcaadr	},
{	"CADAR",	1,	xcadar	},
{	"CADDR",	1,	xcaddr	},
{	"CDAAR",	1,	xcdaar	},
{	"CDADR",	1,	xcdadr	},
{	"CDDAR",	1,	xcddar	},
{	"CDDDR",	1,	xcdddr	},
{	"CAAAAR",	1,	xcaaaar	},
{	"CAAADR",	1,	xcaaadr	},
{	"CAADAR",	1,	xcaadar	},
{	"CAADDR",	1,	xcaaddr	},
{	"CADAAR",	1,	xcadaar	},
{	"CADADR",	1,	xcadadr	},
{	"CADDAR",	1,	xcaddar	},
{	"CADDDR",	1,	xcadddr	},
{	"CDAAAR",	1,	xcdaaar	},
{	"CDAADR",	1,	xcdaadr	},
{	"CDADAR",	1,	xcdadar	},
{	"CDADDR",	1,	xcdaddr	},
{	"CDDAAR",	1,	xcddaar	},
{	"CDDADR",	1,	xcddadr	},
{	"CDDDAR",	1,	xcdddar	},
{	"CDDDDR",	1,	xcddddr	},
{	"CONS",	1,	xcons	},
{	"LIST",	1,	xlist	},
{	"APPEND",	1,	xappend	},
{	"REVERSE",	1,	xreverse	},
{	"LAST",	1,	xlast	},
{	"NTH",	1,	xnth	},
{	"NTHCDR",	1,	xnthcdr	},
{	"MEMBER",	1,	xmember	},
{	"ASSOC",	1,	xassoc	},
{	"SUBST",	1,	xsubst	},
{	"SUBLIS",	1,	xsublis	},
{	"REMOVE",	1,	xremove	},
{	"LENGTH",	1,	xlength	},
{	"MAPC",	1,	xmapc	},
{	"MAPCAR",	1,	xmapcar	},
{	"MAPL",	1,	xmapl	},
{	"MAPLIST",	1,	xmaplist	},
{	"RPLACA",	1,	xrplca	},
{	"RPLACD",	1,	xrplcd	},
{	"NCONC",	1,	xnconc	},
{	"DELETE",	1,	xdelete	},
{	"ATOM",	1,	xatom	},
{	"SYMBOLP",	1,	xsymbolp	},
{	"NUMBERP",	1,	xnumberp	},
{	"BOUNDP",	1,	xboundp	},
{	"NULL",	1,	xnull	},
{	"NOT",	1,	xnull	},
{	"LISTP",	1,	xlistp	},
{	"CONSP",	1,	xconsp	},
{	"MINUSP",	1,	xminusp	},
{	"ZEROP",	1,	xzerop	},
{	"PLUSP",	1,	xplusp	},
{	"EVENP",	1,	xevenp	},
{	"ODDP",	1,	xoddp	},
{	"EQ",	1,	xeq	},
{	"EQL",	1,	xeql	},
{	"EQUAL",	1,	xequal	},
{	"COND",	2,	xcond	},
{	"CASE",	2,	xcase	},
{	"AND",	2,	xand	},
{	"OR",	2,	xor	},
{	"LET",	2,	xlet	},
{	"LET*",	2,	xletstar	},
{	"IF",	2,	xif	},
{	"PROG",	2,	xprog	},
{	"PROG*",	2,	xprogstar	},
{	"PROG1",	2,	xprog1	},
{	"PROG2",	2,	xprog2	},
{	"PROGN",	2,	xprogn	},
{	"GO",	2,	xgo	},
{	"RETURN",	1,	xreturn	},
{	"DO",	2,	xdo	},
{	"DO*",	2,	xdostar	},
{	"DOLIST",	2,	xdolist	},
{	"DOTIMES",	2,	xdotimes	},
{	"CATCH",	2,	xcatch	},
{	"THROW",	1,	xthrow	},
{	"ERROR",	1,	xerror	},
{	"CERROR",	1,	xcerror	},
{	"BREAK",	1,	xbreak	},
{	"CLEAN-UP",	1,	xcleanup	},
{	"CONTINUE",	1,	xcontinue	},
{	"ERRSET",	2,	xerrset	},
{	"BAKTRACE",	1,	xbaktrace	},
{	"EVALHOOK",	1,	xevalhook	},
{	"TRUNCATE",	1,	xfix	},
{	"FLOAT",	1,	xfloat	},
{	"+",	1,	xadd	},
{	"-",	1,	xsub	},
{	"*",	1,	xmul	},
{	"/",	1,	xdiv	},
{	"1+",	1,	xadd1	},
{	"1-",	1,	xsub1	},
{	"REM",	1,	xrem	},
{	"MIN",	1,	xmin	},
{	"MAX",	1,	xmax	},
{	"ABS",	1,	xabs	},
{	"SIN",	1,	xsin	},
{	"COS",	1,	xcos	},
{	"TAN",	1,	xtan	},
{	"EXPT",	1,	xexpt	},
{	"EXP",	1,	xexp	},
{	"SQRT",	1,	xsqrt	},
{	"RANDOM",	1,	xrand	},
{	"BIT-AND",	1,	xbitand	},
{	"BIT-IOR",	1,	xbitior	},
{	"BIT-XOR",	1,	xbitxor	},
{	"BIT-NOT",	1,	xbitnot	},
{	"<",	1,	xlss	},
{	"<=",	1,	xleq	},
{	"=",	1,	xequ	},
{	"/=",	1,	xneq	},
{	">=",	1,	xgeq	},
{	">",	1,	xgtr	},
{	"STRCAT",	1,	xstrcat	},
{	"SUBSTR",	1,	xsubstr	},
{	"STRING",	1,	xstring	},
{	"CHAR",	1,	xchar	},
{	"READ",	1,	xread	},
{	"PRINT",	1,	xprint	},
{	"PRIN1",	1,	xprin1	},
{	"PRINC",	1,	xprinc	},
{	"TERPRI",	1,	xterpri	},
{	"FLATSIZE",	1,	xflatsize	},
{	"FLATC",	1,	xflatc	},
{	"OPENI",	1,	xopeni	},
{	"OPENO",	1,	xopeno	},
{	"CLOSE",	1,	xclose	},
{	"READ-CHAR",	1,	xrdchar	},
{	"PEEK-CHAR",	1,	xpkchar	},
{	"WRITE-CHAR",	1,	xwrchar	},
{	"READ-LINE",	1,	xreadline	},
{	"LOAD",	1,	xload	},
{	"GC",	1,	xgc	},
{	"EXPAND",	1,	xexpand	},
{	"ALLOC",	1,	xalloc	},
{	"MEM",	1,	xmem	},
{	"TYPE-OF",	1,	xtype	},
{	"EXIT",	1,	xexit	},
{	0	}
};
