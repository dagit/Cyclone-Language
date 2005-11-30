extern double	__infinity();
extern double	acos(double);
extern double	asin(double);
extern double	atan(double);
extern double	atan2(double, double);
extern double	ceil(double);
extern double	cos(double);
extern double	cosh(double);
extern double	exp(double);
extern double	fabs(double);
extern double	floor(double);
extern double	fmod(double, double);
extern double	frexp(double, int *);
extern double	ldexp(double, int);
extern double	log(double);
extern double	log10(double);
extern double	modf(double, double *);
extern double	pow(double, double);
extern double	sin(double);
extern double	sinh(double);
extern double	sqrt(double);
extern double	tan(double);
extern double	tanh(double);
enum fp_direction_type {
fp_nearest	= 0,
fp_tozero	= 1,
fp_positive	= 2,
fp_negative	= 3
} ;
enum fp_precision_type	{
fp_extended	= 0,
fp_single	= 1,
fp_double	= 2,
fp_precision_3	= 3
} ;
enum fp_exception_type	{
fp_inexact	= 0,
fp_division	= 1,
fp_underflow	= 2,
fp_overflow	= 3,
fp_invalid	= 4
} ;
enum fp_class_type	{
fp_zero	= 0,
fp_subnormal	= 1,
fp_normal	= 2,
fp_infinity = 3,
fp_quiet	= 4,
fp_signaling	= 5
} ;
typedef float single;
typedef unsigned long extended[3];	typedef struct {
unsigned long u[4];
} quadruple;	typedef unsigned fp_exception_field_type;
typedef int sigfpe_code_type;	typedef void (*sigfpe_handler_type) ();
extern enum fp_direction_type fp_direction;
extern enum fp_precision_type fp_precision;
extern fp_exception_field_type fp_accrued_exceptions;
typedef char decimal_string[512];
typedef struct {
enum fp_class_type fpclass;
int sign;
int exponent;
decimal_string ds;	int more;	int ndigits;
}
decimal_record;
enum decimal_form {
fixed_form,	floating_form	};
typedef struct {
enum fp_direction_type rd;
enum decimal_form df;	int ndigits;
}
decimal_mode;
enum decimal_string_form {	invalid_form,	whitespace_form,	fixed_int_form,	fixed_intdot_form,	fixed_dotfrac_form,	fixed_intdotfrac_form,	floating_int_form,	floating_intdot_form,	floating_dotfrac_form,	floating_intdotfrac_form,	inf_form,	infinity_form,	nan_form,	nanstring_form	};
extern void double_to_decimal();
extern void quadruple_to_decimal();
extern char *econvert();
extern char *fconvert();
extern char *gconvert();
extern char *qeconvert();
extern char *qfconvert();
extern char *qgconvert();
extern sigfpe_handler_type ieee_handlers[5];
extern sigfpe_handler_type sigfpe();
extern void single_to_decimal();
extern void extended_to_decimal();
extern void decimal_to_single();
extern void decimal_to_double();
extern void decimal_to_extended();
extern void decimal_to_quadruple();
extern char *seconvert();
extern char *sfconvert();
extern char *sgconvert();
extern void string_to_decimal();
extern void file_to_decimal();
extern void func_to_decimal();
extern double atof(const char *);
extern int errno;
extern double strtod(const char *, char **);
extern double	acosh(double);
extern double	asinh(double);
extern double	atanh(double);
extern double	cbrt(double);
extern double	copysign(double, double);
extern double	erf(double);
extern double	erfc(double);
extern double	expm1();
extern int	finite(double);
extern double	hypot(double, double);
extern double	j0(double);
extern double	j1(double);
extern double	jn(int, double);
extern double	lgamma(double);
extern double	log1p();
extern double	rint(double);
extern double	y0(double);
extern double	y1(double);
extern double	yn(int, double);
enum fp_pi_type {
fp_pi_infinite	= 0,	fp_pi_66	= 1,	fp_pi_53	= 2	};
extern enum	fp_pi_type fp_pi;
extern enum	fp_class_type fp_class();
extern int	ieee_flags();
extern int	ieee_handler();
extern void	ieee_retrospective();
extern int	ilogb();
extern double	infinity();
extern int	irint();
extern int	isinf(double);
extern int	isnan(double);
extern int	isnormal();
extern int	issubnormal();
extern int	iszero();
extern double	logb(double);
extern double	max_normal();
extern double	max_subnormal();
extern double	min_normal();
extern double	min_subnormal();
extern double	nextafter(double, double);
extern void	nonstandard_arithmetic();
extern double	quiet_nan();
extern double	remainder(double, double);
extern double	scalb(double, double);
extern double	scalbn();
extern double	signaling_nan();
extern int	signbit();
extern double	significand();
extern void	standard_arithmetic();
extern double	acospi();
extern double	aint();
extern double	anint();
extern double	annuity();
extern double	asinpi();
extern double	atan2pi();
extern double	atanpi();
extern double	compound();
extern double	cospi();
extern double	exp10();
extern double	exp2();
extern double	log2();
extern int	nint();
extern void	sincos();
extern void	sincospi();
extern double	sinpi();
extern double	tanpi();
extern int	matherr(struct exception *);
extern int	ir_finite_();
extern enum fp_class_type ir_fp_class_();
extern int	ir_ilogb_();
extern int	ir_irint_();
extern int	ir_isinf_();
extern int	ir_isnan_();
extern int	ir_isnormal_();
extern int	ir_issubnormal_();
extern int	ir_iszero_();
extern int	ir_nint_();
extern int	ir_signbit_();
extern void	r_sincos_();
extern void	r_sincospi_();
extern double r_acos_();
extern double r_acosh_();
extern double r_acospi_();
extern double r_aint_();
extern double r_anint_();
extern double r_annuity_();
extern double r_asin_();
extern double r_asinh_();
extern double r_asinpi_();
extern double r_atan2_();
extern double r_atan2pi_();
extern double r_atan_();
extern double r_atanh_();
extern double r_atanpi_();
extern double r_cbrt_();
extern double r_ceil_();
extern double r_compound_();
extern double r_copysign_();
extern double r_cos_();
extern double r_cosh_();
extern double r_cospi_();
extern double r_erf_();
extern double r_erfc_();
extern double r_exp10_();
extern double r_exp2_();
extern double r_exp_();
extern double r_expm1_();
extern double r_fabs_();
extern double r_floor_();
extern double r_fmod_();
extern double r_hypot_();
extern double r_infinity_();
extern double r_j0_();
extern double r_j1_();
extern double r_jn_();
extern double r_lgamma_();
extern double r_log10_();
extern double r_log1p_();
extern double r_log2_();
extern double r_log_();
extern double r_logb_();
extern double r_max_normal_();
extern double r_max_subnormal_();
extern double r_min_normal_();
extern double r_min_subnormal_();
extern double r_nextafter_();
extern double r_pow_();
extern double r_quiet_nan_();
extern double r_remainder_();
extern double r_rint_();
extern double r_scalb_();
extern double r_scalbn_();
extern double r_signaling_nan_();
extern double r_significand_();
extern double r_sin_();
extern double r_sinh_();
extern double r_sinpi_();
extern double r_sqrt_();
extern double r_tan_();
extern double r_tanh_();
extern double r_tanpi_();
extern double r_y0_();
extern double r_y1_();
extern double r_yn_();
struct exception {
int type;
char *name;
double arg1;
double arg2;
double retval;
};
extern int	signgam;
extern double cabs();	extern double drem(double, double);	extern double gamma(double);	extern	struct	_iobuf {
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
extern NODE *true;
static NODE *unary();
static NODE *binary();
static NODE *predicate();
static NODE *compare();
NODE *xadd(NODE *args)
{
return (binary(args,'+'));
}
NODE *xsub(NODE *args)
{
return (binary(args,'-'));
}
NODE *xmul(NODE *args)
{
return (binary(args,'*'));
}
NODE *xdiv(NODE *args)
{
return (binary(args,'/'));
}
NODE *xrem(NODE *args)
{
return (binary(args,'%'));
}
NODE *xmin(NODE *args)
{
return (binary(args,'m'));
}
NODE *xmax(NODE *args)
{
return (binary(args,'M'));
}
NODE *xexpt(NODE *args)
{
return (binary(args,'E'));
}
NODE *xbitand(NODE *args)
{
return (binary(args,'&'));
}
NODE *xbitior(NODE *args)
{
return (binary(args,'|'));
}
NODE *xbitxor(NODE *args)
{
return (binary(args,'^'));
}
static NODE *binary(NODE *args, int fcn)
{
long ival,iarg;
float fval,farg;
NODE *arg;
int imode;
arg = xlarg(&args);
if (((arg) && (arg)->n_type == 5)) {
ival = ((arg)->n_info.n_xint.xi_int);
imode = 1;
}
else if (((arg) && (arg)->n_type == 9)) {
fval = ((arg)->n_info.n_xfloat.xf_float);
imode = 0;
}
else
xlerror("bad argument type",arg);
if (fcn == '-' && args == (NODE *)0)
if (imode)
ival = -ival;
else
fval = -fval;
while (args) {
arg = xlarg(&args);
if (((arg) && (arg)->n_type == 5))
if (imode) iarg = ((arg)->n_info.n_xint.xi_int);
else farg = (float)((arg)->n_info.n_xint.xi_int);
else if (((arg) && (arg)->n_type == 9))
if (imode) { fval = (float)ival; farg = ((arg)->n_info.n_xfloat.xf_float); imode = 0; }
else farg = ((arg)->n_info.n_xfloat.xf_float);
else
xlerror("bad argument type",arg);
if (imode)
switch (fcn) {
case '+':	ival += iarg; break;
case '-':	ival -= iarg; break;
case '*':	ival *= iarg; break;
case '/':	checkizero(iarg); ival /= iarg; break;
case '%':	checkizero(iarg); ival %= iarg; break;
case 'M':	if (iarg > ival) ival = iarg; break;
case 'm':	if (iarg < ival) ival = iarg; break;
case '&':	ival &= iarg; break;
case '|':	ival |= iarg; break;
case '^':	ival ^= iarg; break;
default:	badiop();
}
else
switch (fcn) {
case '+':	fval += farg; break;
case '-':	fval -= farg; break;
case '*':	fval *= farg; break;
case '/':	checkfzero(farg); fval /= farg; break;
case 'M':	if (farg > fval) fval = farg; break;
case 'm':	if (farg < fval) fval = farg; break;
case 'E':	fval = pow(fval,farg); break;
default:	badfop();
}
}
return (imode ? cvfixnum(ival) : cvflonum(fval));
}
checkizero(long int iarg)
{
if (iarg == 0)
xlfail("division by zero");
}
checkfzero(float farg)
{
if (farg == 0.0)
xlfail("division by zero");
}
checkfneg(float farg)
{
if (farg < 0.0)
xlfail("square root of a negative number");
}
NODE *xbitnot(NODE *args)
{
return (unary(args,'~'));
}
NODE *xabs(NODE *args)
{
return (unary(args,'A'));
}
NODE *xadd1(NODE *args)
{
return (unary(args,'+'));
}
NODE *xsub1(NODE *args)
{
return (unary(args,'-'));
}
NODE *xsin(NODE *args)
{
return (unary(args,'S'));
}
NODE *xcos(NODE *args)
{
return (unary(args,'C'));
}
NODE *xtan(NODE *args)
{
return (unary(args,'T'));
}
NODE *xexp(NODE *args)
{
return (unary(args,'E'));
}
NODE *xsqrt(NODE *args)
{
return (unary(args,'R'));
}
NODE *xfix(NODE *args)
{
return (unary(args,'I'));
}
NODE *xfloat(NODE *args)
{
return (unary(args,'F'));
}
NODE *xrand(NODE *args)
{
return (unary(args,'R'));
}
static NODE *unary(NODE *args, int fcn)
{
float fval;
long ival;
NODE *arg;
arg = xlarg(&args);
xllastarg(args);
if (((arg) && (arg)->n_type == 5)) {
ival = ((arg)->n_info.n_xint.xi_int);
switch (fcn) {
case '~':	ival = ~ival; break;
case 'A':	ival = ((ival) < 0 ? -(ival) : (ival)); break;
case '+':	ival++; break;
case '-':	ival--; break;
case 'I':	break;
case 'F':	return (cvflonum((float)ival));
case 'R':	ival = (long)osrand((int)ival); break;
default:	badiop();
}
return (cvfixnum(ival));
}
else if (((arg) && (arg)->n_type == 9)) {
fval = ((arg)->n_info.n_xfloat.xf_float);
switch (fcn) {
case 'A':	fval = ((fval) < 0.0 ? -(fval) : (fval)); break;
case '+':	fval += 1.0; break;
case '-':	fval -= 1.0; break;
case 'S':	fval = sin(fval); break;
case 'C':	fval = cos(fval); break;
case 'T':	fval = tan(fval); break;
case 'E':	fval = exp(fval); break;
case 'R':	checkfneg(fval); fval = sqrt(fval); break;
case 'I':	return (cvfixnum((long)fval));
case 'F':	break;
default:	badfop();
}
return (cvflonum(fval));
}
else
xlerror("bad argument type",arg);
}
NODE *xminusp(NODE *args)
{
return (predicate(args,'-'));
}
NODE *xzerop(NODE *args)
{
return (predicate(args,'Z'));
}
NODE *xplusp(NODE *args)
{
return (predicate(args,'+'));
}
NODE *xevenp(NODE *args)
{
return (predicate(args,'E'));
}
NODE *xoddp(NODE *args)
{
return (predicate(args,'O'));
}
static NODE *predicate(NODE *args, int fcn)
{
float fval;
long ival;
NODE *arg;
arg = xlarg(&args);
xllastarg(args);
if (((arg) && (arg)->n_type == 5)) {
ival = ((arg)->n_info.n_xint.xi_int);
switch (fcn) {
case '-':	ival = (ival < 0); break;
case 'Z':	ival = (ival == 0); break;
case '+':	ival = (ival > 0); break;
case 'E':	ival = ((ival & 1) == 0); break;
case 'O':	ival = ((ival & 1) != 0); break;
default:	badiop();
}
}
else if (((arg) && (arg)->n_type == 9)) {
fval = ((arg)->n_info.n_xfloat.xf_float);
switch (fcn) {
case '-':	ival = (fval < 0); break;
case 'Z':	ival = (fval == 0); break;
case '+':	ival = (fval > 0); break;
default:	badfop();
}
}
else
xlerror("bad argument type",arg);
return (ival ? true : (NODE *)0);
}
NODE *xlss(NODE *args)
{
return (compare(args,'<'));
}
NODE *xleq(NODE *args)
{
return (compare(args,'L'));
}
NODE *xequ(NODE *args)
{
return (compare(args,'='));
}
NODE *xneq(NODE *args)
{
return (compare(args,'#'));
}
NODE *xgeq(NODE *args)
{
return (compare(args,'G'));
}
NODE *xgtr(NODE *args)
{
return (compare(args,'>'));
}
static NODE *compare(NODE *args, int fcn)
{
NODE *arg1,*arg2;
long icmp;
float fcmp;
int imode;
arg1 = xlarg(&args);
arg2 = xlarg(&args);
xllastarg(args);
if (((arg1) && (arg1)->n_type == 6) && ((arg2) && (arg2)->n_type == 6)) {
icmp = strcmp(((arg1)->n_info.n_xstr.xst_str),((arg2)->n_info.n_xstr.xst_str));
imode = 1;
}
else if (((arg1) && (arg1)->n_type == 5) && ((arg2) && (arg2)->n_type == 5)) {
icmp = ((arg1)->n_info.n_xint.xi_int) - ((arg2)->n_info.n_xint.xi_int);
imode = 1;
}
else if (((arg1) && (arg1)->n_type == 9) && ((arg2) && (arg2)->n_type == 9)) {
fcmp = ((arg1)->n_info.n_xfloat.xf_float) - ((arg2)->n_info.n_xfloat.xf_float);
imode = 0;
}
else if (((arg1) && (arg1)->n_type == 5) && ((arg2) && (arg2)->n_type == 9)) {
fcmp = (float)((arg1)->n_info.n_xint.xi_int) - ((arg2)->n_info.n_xfloat.xf_float);
imode = 0;
}
else if (((arg1) && (arg1)->n_type == 9) && ((arg2) && (arg2)->n_type == 5)) {
fcmp = ((arg1)->n_info.n_xfloat.xf_float) - (float)((arg2)->n_info.n_xint.xi_int);
imode = 0;
}
else
xlfail("expecting strings, integers or floats");
if (imode)
switch (fcn) {
case '<':	icmp = (icmp < 0); break;
case 'L':	icmp = (icmp <= 0); break;
case '=':	icmp = (icmp == 0); break;
case '#':	icmp = (icmp != 0); break;
case 'G':	icmp = (icmp >= 0); break;
case '>':	icmp = (icmp > 0); break;
}
else
switch (fcn) {
case '<':	icmp = (fcmp < 0.0); break;
case 'L':	icmp = (fcmp <= 0.0); break;
case '=':	icmp = (fcmp == 0.0); break;
case '#':	icmp = (fcmp != 0.0); break;
case 'G':	icmp = (fcmp >= 0.0); break;
case '>':	icmp = (fcmp > 0.0); break;
}
return (icmp ? true : (NODE *)0);
}
static badiop(void)
{
xlfail("bad integer operation");
}
static badfop(void)
{
xlfail("bad floating point operation");
}
