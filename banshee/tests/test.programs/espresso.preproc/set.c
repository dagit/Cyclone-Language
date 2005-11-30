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
typedef struct sm_element_struct sm_element;
typedef struct sm_row_struct sm_row;
typedef struct sm_col_struct sm_col;
typedef struct sm_matrix_struct sm_matrix;
struct sm_element_struct {
int row_num;	int col_num;	sm_element *next_row;	sm_element *prev_row;	sm_element *next_col;	sm_element *prev_col;	char *user_word;	};
struct sm_row_struct {
int row_num;	int length;	int flag;	sm_element *first_col;	sm_element *last_col;	sm_row *next_row;	sm_row *prev_row;	char *user_word;	};
struct sm_col_struct {
int col_num;	int length;	int flag;	sm_element *first_row;	sm_element *last_row;	sm_col *next_col;	sm_col *prev_col;	char *user_word;	};
struct sm_matrix_struct {
sm_row **rows;	int rows_size;	sm_col **cols;	int cols_size;	sm_row *first_row;	sm_row *last_row;	int nrows;	sm_col *first_col;	sm_col *last_col;	int ncols;	char *user_word;	};
extern sm_matrix *sm_alloc(), *sm_alloc_size(), *sm_dup();
extern void sm_free(), sm_delrow(), sm_delcol(), sm_resize();
extern void sm_write(), sm_print(), sm_dump(), sm_cleanup();
extern void sm_copy_row(), sm_copy_col();
extern void sm_remove(), sm_remove_element();
extern sm_element *sm_insert(), *sm_find();
extern sm_row *sm_longest_row();
extern sm_col *sm_longest_col();
extern int sm_read(), sm_read_compressed();
extern sm_row *sm_row_alloc(), *sm_row_dup(), *sm_row_and();
extern void sm_row_free(), sm_row_remove(), sm_row_print();
extern sm_element *sm_row_insert(), *sm_row_find();
extern int sm_row_contains(), sm_row_intersects();
extern int sm_row_compare(), sm_row_hash();
extern sm_col *sm_col_alloc(), *sm_col_dup(), *sm_col_and();
extern void sm_col_free(), sm_col_remove(), sm_col_print();
extern sm_element *sm_col_insert(), *sm_col_find();
extern int sm_col_contains(), sm_col_intersects();
extern int sm_col_compare(), sm_col_hash();
extern int sm_row_dominance(), sm_col_dominance(), sm_block_partition();
extern sm_row *sm_minimum_cover();
extern	char	_ctype_[];
extern struct _iobuf *popen(const char *, const char *), *tmpfile(void);
extern int pclose(FILE *);
extern void rewind(FILE *);
extern void abort(void), free(void *), exit(int), perror(const char *);
extern char *getenv(const char *), *malloc(size_t), *realloc(void *, size_t);
extern int system(const char *);
extern double atof(const char *);
extern char *strcpy(char *, const char *), *strncpy(char *, const char *, size_t), *strcat(char *, const char *), *strncat(char *, const char *, size_t), *strerror(int);
extern char *strpbrk(const char *, const char *), *strtok(char *, const char *), *strchr(const char *, int), *strrchr(const char *, int), *strstr(const char *, const char *);
extern int strcoll(const char *, const char *), strxfrm(char *, const char *, size_t), strncmp(const char *, const char *, size_t), strlen(const char *), strspn(const char *, const char *), strcspn(const char *, const char *);
extern char *memmove(void *, const void *, size_t), *memccpy(void *, const void *, int, size_t), *memchr(const void *, int, size_t), *memcpy(void *, const void *, size_t), *memset(void *, int, size_t);
extern int memcmp(const void *, const void *, size_t), strcmp(const char *, const char *);
extern long util_cpu_time();
extern int util_getopt();
extern void util_getopt_reset();
extern char *util_path_search();
extern char *util_file_search();
extern int util_pipefork();
extern void util_print_cpu_stats();
extern char *util_print_time();
extern int util_save_image();
extern char *util_strsav();
extern char *util_tilde_expand();
extern void util_restart();
extern int util_optind;
extern char *util_optarg;

typedef unsigned int *pset;
typedef struct set_family {
int wsize; int sf_size; int capacity; int count; int active_count; pset data; struct set_family *next; } set_family_t, *pset_family;
extern int bit_count[256];

typedef struct cost_struct {
int cubes;	int in;	int out;	int mv;	int total;	int primes;	} cost_t, *pcost;
typedef struct pair_struct {
int cnt;
int *var1;
int *var2;
} pair_t, *ppair;
typedef struct symbolic_list_struct {
int variable;
int pos;
struct symbolic_list_struct *next;
} symbolic_list_t;
typedef struct symbolic_label_struct {
char *label;
struct symbolic_label_struct *next;
} symbolic_label_t;
typedef struct symbolic_struct {
symbolic_list_t *symbolic_list;	int symbolic_list_length;	symbolic_label_t *symbolic_label;	int symbolic_label_length;	struct symbolic_struct *next;
} symbolic_t;
typedef struct {
pset_family F, D, R;	char *filename; int pla_type; pset phase; ppair pair; char **label;	symbolic_t *symbolic;	symbolic_t *symbolic_output;
} PLA_t, *pPLA;

extern unsigned int debug; extern int verbose_debug; extern char *total_name[16]; extern long total_time[16]; extern int total_calls[16]; extern int echo_comments;	extern int echo_unknown_commands;	extern int force_irredundant; extern int skip_make_sparse;
extern int kiss; extern int pos; extern int print_solution; extern int recompute_onset; extern int remove_essential; extern int single_expand; extern int summary; extern int trace; extern int unwrap_onset; extern int use_random_order;	extern int use_super_gasp;	extern char *filename;	extern int debug_exact_minimization; struct pla_types_struct {
char *key;
int value;
};
struct cube_struct {
int size; int num_vars; int num_binary_vars; int *first_part; int *last_part; int *part_size; int *first_word; int *last_word; pset binary_mask; pset mv_mask; pset *var_mask; pset *temp; pset fullset; pset emptyset; unsigned int inmask; int inword; int *sparse; int num_mv_vars; int output; };
struct cdata_struct {
int *part_zeros; int *var_zeros; int *parts_active; int *is_unate; int vars_active; int vars_unate; int best; };
extern struct pla_types_struct pla_types[];
extern struct cube_struct cube, temp_cube_save;
extern struct cdata_struct cdata, temp_cdata_save;

extern int binate_split_select();
extern pset_family cubeunlist();
extern pset *cofactor();
extern pset *cube1list();
extern pset *cube2list();
extern pset *cube3list();
extern pset *scofactor();
extern void massive_count();
extern pset_family complement();
extern pset_family simplify();
extern void simp_comp();
extern int d1_rm_equal();
extern int rm2_contain();
extern int rm2_equal();
extern int rm_contain();
extern int rm_equal();
extern int rm_rev_contain();
extern pset *sf_list();
extern pset *sf_sort();
extern pset_family d1merge();
extern pset_family dist_merge();
extern pset_family sf_contain();
extern pset_family sf_dupl();
extern pset_family sf_ind_contain();
extern pset_family sf_ind_unlist();
extern pset_family sf_merge();
extern pset_family sf_rev_contain();
extern pset_family sf_union();
extern pset_family sf_unlist();
extern void cube_setup();
extern void restore_cube_struct();
extern void save_cube_struct();
extern void setdown_cube();
extern PLA_labels();
extern char *get_word();
extern int label_index();
extern int read_pla();
extern int read_symbolic();
extern pPLA new_PLA();
extern void PLA_summary();
extern void free_PLA();
extern void parse_pla();
extern void read_cube();
extern void skip_line();
extern foreach_output_function();
extern int cubelist_partition();
extern int so_both_do_espresso();
extern int so_both_do_exact();
extern int so_both_save();
extern int so_do_espresso();
extern int so_do_exact();
extern int so_save();
extern pset_family cof_output();
extern pset_family lex_sort();
extern pset_family mini_sort();
extern pset_family random_order();
extern pset_family size_sort();
extern pset_family sort_reduce();
extern pset_family uncof_output();
extern pset_family unravel();
extern pset_family unravel_range();
extern void so_both_espresso();
extern void so_espresso();
extern char *fmt_cost();
extern char *print_cost();
extern char *util_strsav();
extern void copy_cost();
extern void cover_cost();
extern void fatal();
extern void print_trace();
extern void size_stamp();
extern void totals();
extern char *fmt_cube();
extern char *fmt_expanded_cube();
extern char *pc1();
extern char *pc2();
extern char *pc3();
extern int makeup_labels();
extern kiss_output();
extern kiss_print_cube();
extern output_symbolic_constraints();
extern void cprint();
extern void debug1_print();
extern void debug_print();
extern void eqn_output();
extern void fpr_header();
extern void fprint_pla();
extern void pls_group();
extern void pls_label();
extern void pls_output();
extern void print_cube();
extern void print_expanded_cube();
extern void sf_debug_print();
extern find_equiv_outputs();
extern int check_equiv();
extern pset_family espresso();
extern int essen_cube();
extern pset_family cb_consensus();
extern pset_family cb_consensus_dist0();
extern pset_family essential();
extern pset_family minimize_exact();
extern pset_family minimize_exact_literals();
extern int feasibly_covered();
extern int most_frequent();
extern pset_family all_primes();
extern pset_family expand();
extern pset_family find_all_primes();
extern void elim_lowering();
extern void essen_parts();
extern void essen_raising();
extern void expand1();
extern void mincov();
extern void select_feasible();
extern void setup_BB_CC();
extern pset_family expand_gasp();
extern pset_family irred_gasp();
extern pset_family last_gasp();
extern pset_family super_gasp();
extern void expand1_gasp();
extern int util_getopt();
extern find_dc_inputs();
extern find_inputs();
extern form_bitvector();
extern map_dcset();
extern map_output_symbolic();
extern map_symbolic();
extern pset_family map_symbolic_cover();
extern symbolic_hack_labels();
extern int cube_is_covered();
extern int taut_special_cases();
extern int tautology();
extern pset_family irredundant();
extern void mark_irredundant();
extern void irred_split_cover();
extern sm_matrix *irred_derive_table();
extern pset minterms();
extern void explode();
extern void map();
extern output_phase_setup();
extern pPLA set_phase();
extern pset_family opo();
extern pset find_phase();
extern pset_family find_covers();
extern pset_family form_cover_table();
extern pset_family opo_leaf();
extern pset_family opo_recur();
extern void opoall();
extern void phase_assignment();
extern void repeated_phase_assignment();
extern generate_all_pairs();
extern int **find_pairing_cost();
extern int find_best_cost();
extern int greedy_best_cost();
extern int minimize_pair();
extern int pair_free();
extern pair_all();
extern pset_family delvar();
extern pset_family pairvar();
extern ppair pair_best_cost();
extern ppair pair_new();
extern ppair pair_save();
extern print_pair();
extern void find_optimal_pairing();
extern void set_pair();
extern void set_pair1();
extern pset_family primes_consensus();
extern int sccc_special_cases();
extern pset_family reduce();
extern pset reduce_cube();
extern pset sccc();
extern pset sccc_cube();
extern pset sccc_merge();
extern int set_andp(register pset r, register pset a, register pset b);
extern int set_orp(register pset r, register pset a, register pset b);
extern int setp_disjoint(register pset a, register pset b);
extern int setp_empty(register pset a);
extern int setp_equal(register pset a, register pset b);
extern int setp_full(register pset a, register int size);
extern int setp_implies(register pset a, register pset b);
extern char *pbv1(pset s, int n);
extern char *ps1(register pset a);
extern int *sf_count(pset_family A);
extern int *sf_count_restricted(pset_family A, register pset r);
extern int bit_index(register unsigned int a);
extern int set_dist(register pset a, register pset b);
extern int set_ord(register pset a);
extern void set_adjcnt(register pset a, register int *count, register int weight);
extern pset set_and(register pset r, register pset a, register pset b);
extern pset set_clear(register pset r, int size);
extern pset set_copy(register pset r, register pset a);
extern pset set_diff(register pset r, register pset a, register pset b);
extern pset set_fill(register pset r, register int size);
extern pset set_merge(register pset r, register pset a, register pset b, register pset mask);
extern pset set_or(register pset r, register pset a, register pset b);
extern pset set_xor(register pset r, register pset a, register pset b);
extern pset sf_and(pset_family A);
extern pset sf_or(pset_family A);
extern pset_family sf_active(pset_family A);
extern pset_family sf_addcol(pset_family A, int firstcol, int n);
extern pset_family sf_addset(pset_family A, pset s);
extern pset_family sf_append(pset_family A, pset_family B);
extern pset_family sf_bm_read(FILE *fp);
extern pset_family sf_compress(pset_family A, register pset c);
extern pset_family sf_copy(pset_family R, pset_family A);
extern pset_family sf_copy_col(pset_family dst, int dstcol, pset_family src, int srccol);
extern pset_family sf_delc(pset_family A, int first, int last);
extern pset_family sf_delcol(pset_family A, register int firstcol, register int n);
extern pset_family sf_inactive(pset_family A);
extern pset_family sf_join(pset_family A, pset_family B);
extern pset_family sf_new(int num, int size);
extern pset_family sf_permute(pset_family A, register int *permute, register int npermute);
extern pset_family sf_read(FILE *fp);
extern pset_family sf_save(register pset_family A);
extern pset_family sf_transpose(pset_family A);
extern void set_write(register FILE *fp, register pset a);
extern void sf_bm_print(pset_family A);
extern void sf_cleanup(void);
extern void sf_delset(pset_family A, int i);
extern void sf_free(pset_family A);
extern void sf_print(pset_family A);
extern void sf_write(FILE *fp, pset_family A);
extern int ccommon();
extern int cdist0();
extern int full_row();
extern int ascend();
extern int cactive();
extern int cdist();
extern int cdist01();
extern int cvolume();
extern int d1_order();
extern int d1_order_size();
extern int desc1();
extern int descend();
extern int lex_order();
extern int lex_order1();
extern pset force_lower();
extern void consensus();
extern pset_family cb1_dsharp();
extern pset_family cb_dsharp();
extern pset_family cb_recur_dsharp();
extern pset_family cb_recur_sharp();
extern pset_family cb_sharp();
extern pset_family cv_dsharp();
extern pset_family cv_intersect();
extern pset_family cv_sharp();
extern pset_family dsharp();
extern pset_family make_disjoint();
extern pset_family sharp();
pset do_sm_minimum_cover();
extern pset_family make_sparse();
extern pset_family mv_reduce();
extern qsort(void *, size_t, size_t, int (*) (const void *, const void *));
extern qst();
extern pset_family find_all_minimal_covers_petrick();
extern pset_family map_cover_to_unate();
extern pset_family map_unate_to_cover();
extern pset_family exact_minimum_cover();
extern pset_family gen_primes();
extern pset_family unate_compl();
extern pset_family unate_complement();
extern pset_family unate_intersect();
extern PLA_permute();
extern int PLA_verify();
extern int check_consistency();
extern int verify();
static pset_family set_family_garbage = 0;
static int intcpy(register unsigned int *d, register unsigned int *s, register long int n)
{
register int i;
for(i = 0; i < n; i++) {
*d++ = *s++;
}
}
int bit_index(register unsigned int a)
{
register int i;
if (a == 0)
return -1;
for(i = 0; (a & 1) == 0; a >>= 1, i++)
;
return i;
}
int set_ord(register pset a)
{
register int i, sum = 0;
register unsigned int val;
for(i = (a[0] & 0x03ff); i > 0; i--)
if ((val = a[i]) != 0)
sum += (bit_count[val & 255] + bit_count[(val >> 8) & 255] + bit_count[(val >> 16) & 255] + bit_count[(val >> 24) & 255]);
return sum;
}
int set_dist(register pset a, register pset b)
{
register int i, sum = 0;
register unsigned int val;
for(i = (a[0] & 0x03ff); i > 0; i--)
if ((val = a[i] & b[i]) != 0)
sum += (bit_count[val & 255] + bit_count[(val >> 8) & 255] + bit_count[(val >> 16) & 255] + bit_count[(val >> 24) & 255]);
return sum;
}
pset set_clear(register pset r, int size)
{
register int i = ((size <= 32) ? 1 : ((((size)-1) >> 5) + 1));
*r = i; do r[i] = 0; while (--i > 0);
return r;
}
pset set_fill(register pset r, register int size)
{
register int i = ((size <= 32) ? 1 : ((((size)-1) >> 5) + 1));
*r = i;
r[i] = ~ (unsigned) 0;
r[i] >>= i * 32 - size;
while (--i > 0)
r[i] = ~ (unsigned) 0;
return r;
}
pset set_copy(register pset r, register pset a)
{
register int i = (a[0] & 0x03ff);
do r[i] = a[i]; while (--i >= 0);
return r;
}
pset set_and(register pset r, register pset a, register pset b)
{
register int i = (a[0] & 0x03ff);
(r[0] &= ~0x03ff, r[0] |= (i)); do r[i] = a[i] & b[i]; while (--i > 0);
return r;
}
pset set_or(register pset r, register pset a, register pset b)
{
register int i = (a[0] & 0x03ff);
(r[0] &= ~0x03ff, r[0] |= (i)); do r[i] = a[i] | b[i]; while (--i > 0);
return r;
}
pset set_diff(register pset r, register pset a, register pset b)
{
register int i = (a[0] & 0x03ff);
(r[0] &= ~0x03ff, r[0] |= (i)); do r[i] = a[i] & ~b[i]; while (--i > 0);
return r;
}
pset set_xor(register pset r, register pset a, register pset b)
{
register int i = (a[0] & 0x03ff);
(r[0] &= ~0x03ff, r[0] |= (i)); do r[i] = a[i] ^ b[i]; while (--i > 0);
return r;
}
pset set_merge(register pset r, register pset a, register pset b, register pset mask)
{
register int i = (a[0] & 0x03ff);
(r[0] &= ~0x03ff, r[0] |= (i)); do r[i] = (a[i]&mask[i]) | (b[i]&~mask[i]); while (--i > 0);
return r;
}
int set_andp(register pset r, register pset a, register pset b)
{
register int i = (a[0] & 0x03ff);
register unsigned int x = 0;
(r[0] &= ~0x03ff, r[0] |= (i)); do {r[i] = a[i] & b[i]; x |= r[i];} while (--i > 0);
return x != 0;
}
int set_orp(register pset r, register pset a, register pset b)
{
register int i = (a[0] & 0x03ff);
register unsigned int x = 0;
(r[0] &= ~0x03ff, r[0] |= (i)); do {r[i] = a[i] | b[i]; x |= r[i];} while (--i > 0);
return x != 0;
}
int setp_empty(register pset a)
{
register int i = (a[0] & 0x03ff);
do if (a[i]) return 0; while (--i > 0);
return 1;
}
int setp_full(register pset a, register int size)
{
register int i = (a[0] & 0x03ff);
register unsigned int test;
test = ~ (unsigned) 0;
test >>= i * 32 - size;
if (a[i] != test)
return 0;
while (--i > 0)
if (a[i] != (~(unsigned) 0))
return 0;
return 1;
}
int setp_equal(register pset a, register pset b)
{
register int i = (a[0] & 0x03ff);
do if (a[i] != b[i]) return 0; while (--i > 0);
return 1;
}
int setp_disjoint(register pset a, register pset b)
{
register int i = (a[0] & 0x03ff);
do if (a[i] & b[i]) return 0; while (--i > 0);
return 1;
}
int setp_implies(register pset a, register pset b)
{
register int i = (a[0] & 0x03ff);
do if (a[i] & ~b[i]) return 0; while (--i > 0);
return 1;
}

pset sf_or(pset_family A)
{
register pset or, last, p;
or = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((A->sf_size) <= 32 ? 2 : (((((A->sf_size)-1) >> 5) + 1) + 1))))), A->sf_size);
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize)
{register int i_=( or[0] & 0x03ff); (or[0] &= ~0x03ff, or[0] |= (i_)); do or[i_] = or[i_] | p[i_]; while (--i_>0);};
return or;
}
pset sf_and(pset_family A)
{
register pset and, last, p;
and = set_fill(set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((A->sf_size) <= 32 ? 2 : (((((A->sf_size)-1) >> 5) + 1) + 1))))), A->sf_size), A->sf_size);
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize)
{register int i_=( and[0] & 0x03ff); (and[0] &= ~0x03ff, and[0] |= (i_)); do and[i_] = and[i_] & p[i_]; while (--i_>0);};
return and;
}
pset_family sf_active(pset_family A)
{
register pset p, last;
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
(p[0] |= ( 0x2000));
}
A->active_count = A->count;
return A;
}
pset_family sf_inactive(pset_family A)
{
register pset p, last, pdest;
pdest = A->data;
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
if ((p[0] & ( 0x2000))) {
if (pdest != p) {
{register int i_=( p[0] & 0x03ff); do pdest[i_]= p[i_]; while (--i_>=0);};
}
pdest += A->wsize;
} else {
A->count--;
}
}
return A;
}
pset_family sf_copy(pset_family R, pset_family A)
{
R->sf_size = A->sf_size;
R->wsize = A->wsize;
R->count = A->count;
R->active_count = A->active_count;
intcpy(R->data, A->data, (long) A->wsize * A->count);
return R;
}
pset_family sf_join(pset_family A, pset_family B)
{
pset_family R;
long asize = A->count * A->wsize;
long bsize = B->count * B->wsize;
if (A->sf_size != B->sf_size) fatal("sf_join: sf_size mismatch");
R = sf_new(A->count + B->count, A->sf_size);
R->count = A->count + B->count;
R->active_count = A->active_count + B->active_count;
intcpy(R->data, A->data, asize);
intcpy(R->data + asize, B->data, bsize);
return R;
}
pset_family sf_append(pset_family A, pset_family B)
{
long asize = A->count * A->wsize;
long bsize = B->count * B->wsize;
if (A->sf_size != B->sf_size) fatal("sf_append: sf_size mismatch");
A->capacity = A->count + B->count;
A->data = ( A->data) ? ((unsigned int *) realloc((char *) A->data, sizeof(unsigned int) * ( (long) A->capacity * A->wsize))) : ((unsigned int *) malloc(sizeof(unsigned int) * ( (long) A->capacity * A->wsize)));
intcpy(A->data + asize, B->data, bsize);
A->count += B->count;
A->active_count += B->active_count;
sf_free(B);
return A;
}
pset_family sf_new(int num, int size)
{
pset_family A;
if (set_family_garbage == 0) {
A = ((set_family_t *) malloc(sizeof(set_family_t) * ( 1)));
} else {
A = set_family_garbage;
set_family_garbage = A->next;
}
A->sf_size = size;
A->wsize = ((size) <= 32 ? 2 : (((((size)-1) >> 5) + 1) + 1));
A->capacity = num;
A->data = ((unsigned int *) malloc(sizeof(unsigned int) * ( (long) A->capacity * A->wsize)));
A->count = 0;
A->active_count = 0;
return A;
}
pset_family sf_save(register pset_family A)
{
return sf_copy(sf_new(A->count, A->sf_size), A);
}
void sf_free(pset_family A)
{
((A->data) ? (free((char *) (A->data)), (A->data) = 0) : 0);
A->next = set_family_garbage;
set_family_garbage = A;
}
void sf_cleanup(void)
{
register pset_family p, pnext;
for(p = set_family_garbage; p != (pset_family) 0; p = pnext) {
pnext = p->next;
((p) ? (free((char *) (p)), (p) = 0) : 0);
}
set_family_garbage = (pset_family) 0;
}
pset_family sf_addset(pset_family A, pset s)
{
register pset p;
if (A->count >= A->capacity) {
A->capacity = A->capacity + A->capacity/2 + 1;
A->data = ( A->data) ? ((unsigned int *) realloc((char *) A->data, sizeof(unsigned int) * ( (long) A->capacity * A->wsize))) : ((unsigned int *) malloc(sizeof(unsigned int) * ( (long) A->capacity * A->wsize)));
}
p = ((A)->data + (A)->wsize * ( A->count++));
{register int i_=( s[0] & 0x03ff); do p[i_]= s[i_]; while (--i_>=0);};
return A;
}
void sf_delset(pset_family A, int i)
{ (void) set_copy(((A)->data + (A)->wsize * (i)), ((A)->data + (A)->wsize * ( --A->count)));}
void sf_print(pset_family A)
{
char *ps1(register pset a);
register pset p;
register int i;
for( p=A->data, i=0; i<A->count; p+=A->wsize, i++)
printf("A[%d] = %s\n", i, ps1(p));
}
void sf_bm_print(pset_family A)
{
char *pbv1(pset s, int n);
register pset p;
register int i;
for( p=A->data, i=0; i<A->count; p+=A->wsize, i++)
printf("[%4d] %s\n", i, pbv1(p, A->sf_size));
}
void sf_write(FILE *fp, pset_family A)
{
register pset p, last;
fprintf(fp, "%d %d\n", A->count, A->sf_size);
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize)
set_write(fp, p);
(void) fflush(fp);
}
pset_family sf_read(FILE *fp)
{
int i, j;
register pset p, last;
pset_family A;
(void) fscanf(fp, "%d %d\n", &i, &j);
A = sf_new(i, j);
A->count = i;
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
(void) fscanf(fp, "%x", p);
for(j = 1; j <= (p[0] & 0x03ff); j++)
(void) fscanf(fp, "%x", p+j);
}
return A;
}
void set_write(register FILE *fp, register pset a)
{
register int n = (a[0] & 0x03ff), j;
for(j = 0; j <= n; j++) {
fprintf(fp, "%x ", a[j]);
if ((j+1) % 8 == 0 && j != n)
fprintf(fp, "\n\t");
}
fprintf(fp, "\n");
}
pset_family sf_bm_read(FILE *fp)
{
int i, j, rows, cols;
register pset pdest;
pset_family A;
(void) fscanf(fp, "%d %d\n", &rows, &cols);
A = sf_new(rows, cols);
for(i = 0; i < rows; i++) {
pdest = ((A)->data + (A)->wsize * ( A->count++));
(void) set_clear(pdest, A->sf_size);
for(j = 0; j < cols; j++) {
switch((--(fp)->_cnt>=0? ((int)*(fp)->_ptr++):_filbuf(fp))) {
case '0':
break;
case '1':
(pdest[((( j) >> 5) + 1)] |= 1 << (( j) & (32-1)));
break;
default:
fatal("Error reading set family");
}
}
if ((--(fp)->_cnt>=0? ((int)*(fp)->_ptr++):_filbuf(fp)) != '\n') {
fatal("Error reading set family (at end of line)");
}
}
return A;
}
static char s1[120];
char *ps1(register pset a)
{
register int i, num, l, len = 0, n = (32 * (a[0] & 0x03ff));
char temp[20];
int first = 1;
s1[len++] = '[';
for(i = 0; i < n; i++)
if ((a[(((i) >> 5) + 1)] & (1 << ((i) & (32-1))))) {
if (! first)
s1[len++] = ',';
first = 0; num = i;
l = 0; do temp[l++] = num % 10 + '0'; while ((num /= 10) > 0);
do s1[len++] = temp[--l]; while (l > 0);
if (len > 120-15) {
s1[len++] = '.'; s1[len++] = '.'; s1[len++] = '.';
break;
}
}
s1[len++] = ']';
s1[len++] = '\0';
return s1;
}
char *pbv1(pset s, int n)
{
register int i;
for(i = 0; i < n; i++)
s1[i] = (s[(((i) >> 5) + 1)] & (1 << ((i) & (32-1)))) ? '1' : '0';
s1[n] = '\0';
return s1;
}
void
set_adjcnt(register pset a, register int *count, register int weight)
{
register int i, base;
register unsigned int val;
for(i = (a[0] & 0x03ff); i > 0; ) {
for(val = a[i], base = --i << 5; val != 0; base++, val >>= 1) {
if (val & 1) {
count[base] += weight;
}
}
}
}
int *sf_count(pset_family A)
{
register pset p, last;
register int i, base, *count;
register unsigned int val;
count = ((int *) malloc(sizeof(int) * ( A->sf_size)));
for(i = A->sf_size - 1; i >= 0; i--) {
count[i] = 0;
}
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
for(i = (p[0] & 0x03ff); i > 0; ) {
for(val = p[i], base = --i << 5; val != 0; base++, val >>= 1) {
if (val & 1) {
count[base]++;
}
}
}
}
return count;
}
int *sf_count_restricted(pset_family A, register pset r)
{
register pset p;
register int i, base, *count;
register unsigned int val;
int weight;
pset last;
count = ((int *) malloc(sizeof(int) * ( A->sf_size)));
for(i = A->sf_size - 1; i >= 0; i--) {
count[i] = 0;
}
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
weight = 1024 / (set_ord(p) - 1);
for(i = (p[0] & 0x03ff); i > 0; ) {
for(val=p[i]&r[i], base= --i<<5; val!=0; base++, val >>= 1) {
if (val & 1) {
count[base] += weight;
}
}
}
}
return count;
}
pset_family sf_delc(pset_family A, int first, int last)
{
return sf_delcol(A, first, last-first + 1);
}
pset_family sf_addcol(pset_family A, int firstcol, int n)
{
int maxsize;
if (firstcol == A->sf_size) {
maxsize = 32 * ((A->sf_size <= 32) ? 1 : ((((A->sf_size)-1) >> 5) + 1));
if ((A->sf_size + n) <= maxsize) {
A->sf_size += n;
return A;
}
}
return sf_delcol(A, firstcol, -n);
}
pset_family sf_delcol(pset_family A, register int firstcol, register int n)
{
register pset p, last, pdest;
register int i;
pset_family B;
B = sf_new(A->count, A->sf_size - n);
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
pdest = ((B)->data + (B)->wsize * ( B->count++));
{register int i_=(( B->sf_size <= 32) ? 1 : (((( B->sf_size)-1) >> 5) + 1)); *pdest=i_; do pdest[i_] = 0; while (--i_ > 0);};
for(i = 0; i < firstcol; i++)
if ((p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))))
(pdest[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1)));
for(i = n > 0 ? firstcol + n : firstcol; i < A->sf_size; i++)
if ((p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))))
(pdest[((( i - n) >> 5) + 1)] |= 1 << (( i - n) & (32-1)));
}
sf_free(A);
return B;
}
pset_family sf_copy_col(pset_family dst, int dstcol, pset_family src, int srccol)
{
register pset last, p, pdest;
register int word_test, word_set;
unsigned int bit_set, bit_test;
word_test = (((srccol) >> 5) + 1);
bit_test = 1 << ((srccol) & (32-1));
word_set = (((dstcol) >> 5) + 1);
bit_set = 1 << ((dstcol) & (32-1));
pdest = dst->data;
for( p=src->data, last= p+src->count*src->wsize; p< last; p+=src->wsize) {
if ((p[word_test] & bit_test) != 0)
pdest[word_set] |= bit_set;
pdest += dst->wsize;
}
return dst;
}
pset_family sf_compress(pset_family A, register pset c)
{
register pset p;
register int i, bcol;
pset_family B;
B = sf_new(A->count, set_ord(c));
for(i = 0; i < A->count; i++) {
p = ((B)->data + (B)->wsize * ( B->count++));
{register int i_=(( B->sf_size <= 32) ? 1 : (((( B->sf_size)-1) >> 5) + 1)); *p=i_; do p[i_] = 0; while (--i_ > 0);};
}
bcol = 0;
for(i = 0; i < A->sf_size; i++) {
if ((c[((( i) >> 5) + 1)] & (1 << (( i) & (32-1))))) {
(void) sf_copy_col(B, bcol++, A, i);
}
}
sf_free(A);
return B;
}
pset_family sf_transpose(pset_family A)
{
pset_family B;
register pset p;
register int i, j;
B = sf_new(A->sf_size, A->count);
B->count = A->sf_size;
for( p=B->data, i=0; i<B->count; p+=B->wsize, i++) {
{register int i_=(( B->sf_size <= 32) ? 1 : (((( B->sf_size)-1) >> 5) + 1)); *p=i_; do p[i_] = 0; while (--i_ > 0);};
}
for( p=A->data, i=0; i<A->count; p+=A->wsize, i++) {
for(j = 0; j < A->sf_size; j++) {
if ((p[((( j) >> 5) + 1)] & (1 << (( j) & (32-1))))) {
(((B)->data + (B)->wsize * ( j))[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1)));
}
}
}
sf_free(A);
return B;
}
pset_family sf_permute(pset_family A, register int *permute, register int npermute)
{
pset_family B;
register pset p, last, pdest;
register int j;
B = sf_new(A->count, npermute);
B->count = A->count;
for( p=B->data, last= p+B->count*B->wsize; p< last; p+=B->wsize)
{register int i_=(( npermute <= 32) ? 1 : (((( npermute)-1) >> 5) + 1)); *p=i_; do p[i_] = 0; while (--i_ > 0);};
pdest = B->data;
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
for(j = 0; j < npermute; j++)
if ((p[((( permute[j]) >> 5) + 1)] & (1 << (( permute[j]) & (32-1)))))
(pdest[((( j) >> 5) + 1)] |= 1 << (( j) & (32-1)));
pdest += B->wsize;
}
sf_free(A);
return B;
}
