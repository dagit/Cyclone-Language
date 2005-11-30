typedef int size_t;
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
extern struct node ***xlsave(NODE ** arg1, ...);	extern struct node *xleval();	extern struct node *xlapply();	extern struct node *xlevlist();	extern struct node *xlarg();	extern struct node *xlevarg();	extern struct node *xlmatch();	extern struct node *xlevmatch();	extern struct node *xlgetfile();	extern struct node *xlsend();	extern struct node *xlenter();	extern struct node *xlsenter();	extern struct node *xlmakesym();	extern struct node *xlframe();	extern struct node *xlgetvalue();	extern struct node *xlxgetvalue();	extern struct node *xlygetvalue();	extern struct node *cons(NODE *x, NODE *y);	extern struct node *consa(NODE *x);	extern struct node *consd(NODE *x);	extern struct node *cvsymbol(char *pname);	extern struct node *cvcsymbol(char *pname);	extern struct node *cvstring(char *str);	extern struct node *cvcstring(char *str);	extern struct node *cvfile(FILE *fp);	extern struct node *cvsubr(NODE *(*fcn) (), int type);	extern struct node *cvfixnum(long int n);	extern struct node *cvflonum(float n);	extern struct node *newstring(int size);	extern struct node *newvector(int size);	extern struct node *newobject(NODE *cls, int size);	extern struct node *xlgetprop();	extern char *xlsymname();	extern void xlsetvalue();
extern void xlprint();
extern void xltest();
extern NODE ***xlstack,***xlstkbase,***xlstktop;
extern NODE *obarray;
extern NODE *xlenv;
extern long total;
extern int anodes,nnodes,nsegs,nfree,gccalls;
extern struct segment *segs;
extern NODE *fnodes;
extern char buf[];
extern char *malloc(size_t);
extern char *calloc(size_t, size_t);
static NODE *newnode();
static char *strsave();
static char *stralloc();
NODE *cons(NODE *x, NODE *y)
{
NODE *val;
val = newnode(3);
((val)->n_info.n_xlist.xl_car = (x));
((val)->n_info.n_xlist.xl_cdr = (y));
return (val);
}
NODE *consa(NODE *x)
{
NODE *val;
val = newnode(3);
((val)->n_info.n_xlist.xl_car = (x));
return (val);
}
NODE *consd(NODE *x)
{
NODE *val;
val = newnode(3);
((val)->n_info.n_xlist.xl_cdr = (x));
return (val);
}
NODE *cvstring(char *str)
{
NODE ***oldstk,*val;
oldstk = xlsave(&val,(NODE **)0);
val = newnode(6);
val->n_info.n_xstr.xst_str = strsave(str);
val->n_info.n_xstr.xst_type = 0;
xlstack = oldstk;
return (val);
}
NODE *cvcstring(char *str)
{
NODE *val;
val = newnode(6);
val->n_info.n_xstr.xst_str = str;
val->n_info.n_xstr.xst_type = 1;
return (val);
}
NODE *cvsymbol(char *pname)
{
NODE ***oldstk,*val;
oldstk = xlsave(&val,(NODE **)0);
val = newnode(4);
val->n_info.n_xsym.xsy_plist = newnode(3);
((val->n_info.n_xsym.xsy_plist)->n_info.n_xlist.xl_car = (cvstring(pname)));
xlstack = oldstk;
return (val);
}
NODE *cvcsymbol(char *pname)
{
NODE ***oldstk,*val;
oldstk = xlsave(&val,(NODE **)0);
val = newnode(4);
val->n_info.n_xsym.xsy_plist = newnode(3);
((val->n_info.n_xsym.xsy_plist)->n_info.n_xlist.xl_car = (cvcstring(pname)));
xlstack = oldstk;
return (val);
}
NODE *cvsubr(NODE *(*fcn) (), int type)
{
NODE *val;
val = newnode(type);
val->n_info.n_xsubr.xsu_subr = fcn;
return (val);
}
NODE *cvfile(FILE *fp)
{
NODE *val;
val = newnode(8);
((val)->n_info.n_xfptr.xf_fp = (fp));
((val)->n_info.n_xfptr.xf_savech = (0));
return (val);
}
NODE *cvfixnum(long int n)
{
NODE *val;
val = newnode(5);
val->n_info.n_xint.xi_int = n;
return (val);
}
NODE *cvflonum(float n)
{
NODE *val;
val = newnode(9);
val->n_info.n_xfloat.xf_float = n;
return (val);
}
NODE *newstring(int size)
{
NODE ***oldstk,*val;
oldstk = xlsave(&val,(NODE **)0);
val = newnode(6);
val->n_info.n_xstr.xst_str = stralloc(size);
*((val)->n_info.n_xstr.xst_str) = 0;
val->n_info.n_xstr.xst_type = 0;
xlstack = oldstk;
return (val);
}
NODE *newobject(NODE *cls, int size)
{
NODE *val;
val = newvector(size+1);
((val)->n_info.n_xvect.xv_data[0] = (cls));
val->n_type = 7;
return (val);
}
NODE *newvector(int size)
{
NODE ***oldstk,*vect;
int bsize;
oldstk = xlsave(&vect,(NODE **)0);
vect = newnode(10);
vect->n_info.n_xvect.xv_size = 0;
bsize = size * sizeof(NODE *);
if ((vect->n_info.n_xvect.xv_data = (NODE **) calloc(1,bsize)) == 0) {
findmem();
if ((vect->n_info.n_xvect.xv_data = (NODE **) calloc(1,bsize)) == 0)
xlfail("insufficient vector space");
}
vect->n_info.n_xvect.xv_size = size;
total += (long) bsize;
xlstack = oldstk;
return (vect);
}
static NODE *newnode(int type)
{
NODE *nnode;
if ((nnode = fnodes) == (NODE *)0) {
findmem();
if ((nnode = fnodes) == (NODE *)0)
xlabort("insufficient node space");
}
fnodes = ((nnode)->n_info.n_xlist.xl_cdr);
nfree -= 1;
nnode->n_type = type;
((nnode)->n_info.n_xlist.xl_cdr = ((NODE *)0));
return (nnode);
}
static char *stralloc(int size)
{
char *sptr;
if ((sptr = malloc(size+1)) == 0) {
findmem(); if ((sptr = malloc(size+1)) == 0)
xlfail("insufficient string space");
}
total += (long) (size+1);
return (sptr);
}
static char *strsave(char *str)
{
char *sptr;
sptr = stralloc(strlen(str));
strcpy(sptr,str);
return (sptr);
}
findmem(void)
{
gc();
if (nfree < anodes)
addseg();
}
gc(void)
{
NODE ***p;
void mark(NODE *ptr);
mark(obarray);
mark(xlenv);
for (p = xlstack; p < xlstktop; )
mark(**p++);
sweep();
gccalls++;
}
void mark(NODE *ptr)
{
NODE *this,*prev,*tmp;
if (ptr == (NODE *)0)
return;
prev = (NODE *)0;
this = ptr;
while (1) {
while (1) {
if (this->n_flags & 1)
break;
else {
this->n_flags |= 1;
if (livecar(this)) {
this->n_flags |= 2;
tmp = prev;
prev = this;
this = ((prev)->n_info.n_xlist.xl_car);
((prev)->n_info.n_xlist.xl_car = (tmp));
}
else if (livecdr(this)) {
this->n_flags &= ~2;
tmp = prev;
prev = this;
this = ((prev)->n_info.n_xlist.xl_cdr);
((prev)->n_info.n_xlist.xl_cdr = (tmp));
}
else
break;
}
}
while (1) {
if (prev == (NODE *)0)
return;
if (prev->n_flags & 2)
if (livecdr(prev)) {
prev->n_flags &= ~2;
tmp = ((prev)->n_info.n_xlist.xl_car);
((prev)->n_info.n_xlist.xl_car = (this));
this = ((prev)->n_info.n_xlist.xl_cdr);
((prev)->n_info.n_xlist.xl_cdr = (tmp));
break;
}
else {
tmp = prev;
prev = ((tmp)->n_info.n_xlist.xl_car);
((tmp)->n_info.n_xlist.xl_car = (this));
this = tmp;
}
else {
tmp = prev;
prev = ((tmp)->n_info.n_xlist.xl_cdr);
((tmp)->n_info.n_xlist.xl_cdr = (this));
this = tmp;
}
}
}
}
vmark(NODE *n)
{
int i;
for (i = 0; i < ((n)->n_info.n_xvect.xv_size); ++i)
mark(((n)->n_info.n_xvect.xv_data[i]));
}
static sweep(void)
{
struct segment *seg;
NODE *p;
int n;
fnodes = (NODE *)0;
nfree = 0;
for (seg = segs; seg != 0; seg = seg->sg_next) {
p = &seg->sg_nodes[0];
for (n = seg->sg_size; n--; p++)
if (!(p->n_flags & 1)) {
switch (((p)->n_type)) {
case 6:
if (p->n_info.n_xstr.xst_type == 0 && p->n_info.n_xstr.xst_str != 0) {
total -= (long) (strlen(p->n_info.n_xstr.xst_str)+1);
free(p->n_info.n_xstr.xst_str);
}
break;
case 8:
if (p->n_info.n_xfptr.xf_fp)
fclose(p->n_info.n_xfptr.xf_fp);
break;
case 10:
if (p->n_info.n_xvect.xv_size) {
total -= (long) (p->n_info.n_xvect.xv_size * sizeof(NODE **));
free(p->n_info.n_xvect.xv_data);
}
break;
}
p->n_type = 0;
p->n_flags = 0;
((p)->n_info.n_xlist.xl_car = ((NODE *)0));
((p)->n_info.n_xlist.xl_cdr = (fnodes));
fnodes = p;
nfree++;
}
else
p->n_flags &= ~(1 | 2);
}
}
int addseg(void)
{
struct segment *newseg;
NODE *p;
int n;
if (anodes == 0)
return (0);
if ((newseg = (struct segment *) calloc(1,(sizeof(struct segment) + (anodes-1) * sizeof(NODE)))) != 0) {
newseg->sg_size = anodes;
newseg->sg_next = segs;
segs = newseg;
p = &newseg->sg_nodes[0];
for (n = anodes; n--; ) {
((p)->n_info.n_xlist.xl_cdr = (fnodes));
fnodes = p++;
}
total += (long) (sizeof(struct segment) + (anodes-1) * sizeof(NODE));
nnodes += anodes;
nfree += anodes;
nsegs++;
return (1);
}
else
return (0);
}
static int livecar(NODE *n)
{
switch (((n)->n_type)) {
case 7:
case 10:
vmark(n);
case 1:
case 2:
case 5:
case 9:
case 6:
case 8:
return (0);
case 4:
case 3:
return (((n)->n_info.n_xlist.xl_car) != (NODE *)0);
default:
printf("bad node type (%d) found during left scan\n",((n)->n_type));
osfinish ();
exit(1);
}
}
static int livecdr(NODE *n)
{
switch (((n)->n_type)) {
case 1:
case 2:
case 5:
case 9:
case 6:
case 8:
case 7:
case 10:
return (0);
case 4:
case 3:
return (((n)->n_info.n_xlist.xl_cdr) != (NODE *)0);
default:
printf("bad node type (%d) found during right scan\n",((n)->n_type));
osfinish ();
exit(1);
}
}
stats(void)
{
sprintf(buf,"Nodes:       %d\n",nnodes); stdputstr(buf);
sprintf(buf,"Free nodes:  %d\n",nfree); stdputstr(buf);
sprintf(buf,"Segments:    %d\n",nsegs); stdputstr(buf);
sprintf(buf,"Allocate:    %d\n",anodes); stdputstr(buf);
sprintf(buf,"Total:       %ld\n",total); stdputstr(buf);
sprintf(buf,"Collections: %d\n",gccalls); stdputstr(buf);
}
xlminit(void)
{
anodes = 1000;
total = 0L;
nnodes = nsegs = nfree = gccalls = 0;
fnodes = (NODE *)0;
segs = 0;
xlenv = obarray = (NODE *)0;
if ((xlstkbase = (NODE ***)malloc(1000 * sizeof(NODE **))) == 0) {
printf("insufficient memory");
osfinish ();
exit(1);
}
total = total + 2000 ;
xlstack = xlstktop = xlstkbase + 1000;
}
