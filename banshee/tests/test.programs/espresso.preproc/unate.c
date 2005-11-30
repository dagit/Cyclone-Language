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
extern int set_andp();
extern int set_orp();
extern int setp_disjoint();
extern int setp_empty();
extern int setp_equal();
extern int setp_full();
extern int setp_implies();
extern char *pbv1();
extern char *ps1();
extern int *sf_count();
extern int *sf_count_restricted();
extern int bit_index();
extern int set_dist();
extern int set_ord();
extern void set_adjcnt();
extern pset set_and();
extern pset set_clear();
extern pset set_copy();
extern pset set_diff();
extern pset set_fill();
extern pset set_merge();
extern pset set_or();
extern pset set_xor();
extern pset sf_and();
extern pset sf_or();
extern pset_family sf_active();
extern pset_family sf_addcol();
extern pset_family sf_addset();
extern pset_family sf_append();
extern pset_family sf_bm_read();
extern pset_family sf_compress();
extern pset_family sf_copy();
extern pset_family sf_copy_col();
extern pset_family sf_delc();
extern pset_family sf_delcol();
extern pset_family sf_inactive();
extern pset_family sf_join();
extern pset_family sf_new();
extern pset_family sf_permute();
extern pset_family sf_read();
extern pset_family sf_save();
extern pset_family sf_transpose();
extern void set_write();
extern void sf_bm_print();
extern void sf_cleanup();
extern void sf_delset();
extern void sf_free();
extern void sf_print();
extern void sf_write();
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
extern pset_family map_cover_to_unate(pset *T);
extern pset_family map_unate_to_cover(pset_family A);
extern pset_family exact_minimum_cover(pset_family T);
extern pset_family gen_primes();
extern pset_family unate_compl(pset_family A);
extern pset_family unate_complement(pset_family A);
extern pset_family unate_intersect(pset_family A, pset_family B, int largest_only);
extern PLA_permute();
extern int PLA_verify();
extern int check_consistency();
extern int verify();
static pset_family abs_covered(pset_family A, register int pick);
static pset_family abs_covered_many(pset_family A, register pset pick_set);
static int abs_select_restricted(pset_family A, pset restrict);
pset_family map_cover_to_unate(pset *T)
{
register unsigned int word_test, word_set, bit_test, bit_set;
register pset p, pA;
pset_family A;
pset *T1;
int ncol, i;
A = sf_new((((pset *) T[1] - T) - 3), cdata.vars_unate);
A->count = (((pset *) T[1] - T) - 3);
for( p=A->data, i=0; i<A->count; p+=A->wsize, i++) {
(void) set_clear(p, A->sf_size);
}
ncol = 0;
for(i = 0; i < cube.size; i++) {
if (cdata.part_zeros[i] > 0) {
{ if (! (ncol <= cdata.vars_unate)) { (void) fprintf((&_iob[2]), "Assertion failed: file %s, line %d\n\"%s\"\n", "unate.c", 29, "ncol <= cdata.vars_unate"); (void) fflush((&_iob[1])); abort(); }};
word_test = (((i) >> 5) + 1);
bit_test = 1 << ((i) & (32-1));
word_set = (((ncol) >> 5) + 1);
bit_set = 1 << ((ncol) & (32-1));
pA = A->data;
for(T1 = T+2; (p = *T1++) != 0; ) {
if ((p[word_test] & bit_test) == 0) {
pA[word_set] |= bit_set;
}
pA += A->wsize;
}
ncol++;
}
}
return A;
}

pset_family map_unate_to_cover(pset_family A)
{
register int i, ncol, lp;
register pset p, pB;
int var, nunate, *unate;
pset last;
pset_family B;
B = sf_new(A->count, cube.size);
B->count = A->count;
unate = ((int *) malloc(sizeof(int) * ( cube.num_vars)));
nunate = 0;
for(var = 0; var < cube.num_vars; var++) {
if (cdata.is_unate[var]) {
unate[nunate++] = var;
}
}
pB = B->data;
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
{register int i_=(( cube.size <= 32) ? 1 : (((( cube.size)-1) >> 5) + 1)); *pB=i_; pB[i_]=((unsigned int)(~0))>>(i_*32- cube.size); while(--i_>0) pB[i_]=~0;};
for(ncol = 0; ncol < nunate; ncol++) {
if ((p[((( ncol) >> 5) + 1)] & (1 << (( ncol) & (32-1))))) {
lp = cube.last_part[unate[ncol]];
for(i = cube.first_part[unate[ncol]]; i <= lp; i++) {
if (cdata.part_zeros[i] == 0) {
(pB[((( i) >> 5) + 1)] &= ~ (1 << (( i) & (32-1))));
}
}
}
}
pB += B->wsize;
}
((unate) ? (free((char *) (unate)), (unate) = 0) : 0);
return B;
}

pset_family unate_compl(pset_family A)
{
register pset p, last;
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
(p[0] &= 0xffff, p[0] |= (( set_ord(p)) << 16));
}
A = unate_complement(A);
A = sf_rev_contain(A);
return A;
}
pset_family unate_complement(pset_family A)
{
pset_family Abar;
register pset p, p1, restrict;
register int i;
int max_i, min_set_ord, j;
if (A->count == 0) {
sf_free(A);
Abar = sf_new(1, A->sf_size);
(void) set_clear(((Abar)->data + (Abar)->wsize * ( Abar->count++)), A->sf_size);
} else if (A->count == 1) {
p = A->data;
Abar = sf_new(A->sf_size, A->sf_size);
for(i = 0; i < A->sf_size; i++) {
if ((p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1))))) {
p1 = set_clear(((Abar)->data + (Abar)->wsize * ( Abar->count++)), A->sf_size);
(p1[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1)));
}
}
sf_free(A);
} else {
restrict = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((A->sf_size) <= 32 ? 2 : (((((A->sf_size)-1) >> 5) + 1) + 1))))), A->sf_size);
min_set_ord = A->sf_size + 1;
for( p=A->data, i=0; i<A->count; p+=A->wsize, i++) {
if ((p[0] >> 16) < min_set_ord) {
set_copy(restrict, p);
min_set_ord = (p[0] >> 16);
} else if ((p[0] >> 16) == min_set_ord) {
set_or(restrict, restrict, p);
}
}
if (min_set_ord == 0) {
A->count = 0;
Abar = A;
} else if (min_set_ord == 1) {
Abar = unate_complement(abs_covered_many(A, restrict));
sf_free(A);
for( p=Abar->data, i=0; i<Abar->count; p+=Abar->wsize, i++) {
set_or(p, p, restrict);
}
} else {
max_i = abs_select_restricted(A, restrict);
Abar = unate_complement(abs_covered(A, max_i));
for( p=Abar->data, i=0; i<Abar->count; p+=Abar->wsize, i++) {
(p[((( max_i) >> 5) + 1)] |= 1 << (( max_i) & (32-1)));
}
for( p=A->data, i=0; i<A->count; p+=A->wsize, i++) {
if ((p[((( max_i) >> 5) + 1)] & (1 << (( max_i) & (32-1))))) {
(p[((( max_i) >> 5) + 1)] &= ~ (1 << (( max_i) & (32-1))));
j = (p[0] >> 16) - 1;
(p[0] &= 0xffff, p[0] |= (( j) << 16));
}
}
Abar = sf_append(Abar, unate_complement(A));
}
((restrict) ? (free((char *) (restrict)), (restrict) = 0) : 0);
}
return Abar;
}

pset_family exact_minimum_cover(pset_family T)
{
register pset p, last, p1;
register int i, n;
int lev, lvl;
pset nlast;
pset_family temp;
long start = util_cpu_time();
struct {
pset_family sf;
int level;
} stack[32]; if (T->count <= 0)
return sf_new(1, T->sf_size);
for(n = T->count, lev = 0; n != 0; n >>= 1, lev++) ;
T = lex_sort(sf_save(T));
n = 1;
stack[0].sf = sf_new(1, T->sf_size);
stack[0].level = lev;
set_fill(((stack[0].sf)->data + (stack[0].sf)->wsize * ( stack[0].sf->count++)), T->sf_size);
nlast = ((T)->data + (T)->wsize * ( T->count - 1));
for( p=T->data, last= p+T->count*T->wsize; p< last; p+=T->wsize) {
temp = sf_new(set_ord(p), T->sf_size);
for(i = 0; i < T->sf_size; i++)
if ((p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1))))) {
p1 = set_fill(((temp)->data + (temp)->wsize * ( temp->count++)), T->sf_size);
(p1[((( i) >> 5) + 1)] &= ~ (1 << (( i) & (32-1))));
}
stack[n].sf = temp;
stack[n++].level = lev;
while (n > 1 && (stack[n-1].level==stack[n-2].level || p == nlast)) {
temp = unate_intersect(stack[n-1].sf, stack[n-2].sf, 0);
lvl = ((stack[n-1].level) < ( stack[n-2].level) ? (stack[n-1].level) : ( stack[n-2].level)) - 1;
if (debug & 0x0800 && lvl < 10) {
printf("# EXACT_MINCOV[%d]: %4d = %4d x %4d, time = %s\n",
lvl, temp->count, stack[n-1].sf->count,
stack[n-2].sf->count, util_print_time(util_cpu_time() - start));
(void) fflush((&_iob[1]));
}
sf_free(stack[n-2].sf);
sf_free(stack[n-1].sf);
stack[n-2].sf = temp;
stack[n-2].level = lvl;
n--;
}
}
temp = stack[0].sf;
p1 = set_fill(set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((T->sf_size) <= 32 ? 2 : (((((T->sf_size)-1) >> 5) + 1) + 1))))), T->sf_size), T->sf_size);
for( p=temp->data, last= p+temp->count*temp->wsize; p< last; p+=temp->wsize)
{register int i_=( p1[0] & 0x03ff); (p[0] &= ~0x03ff, p[0] |= (i_)); do p[i_] = p1[i_] & ~ p[i_]; while (--i_>0);};
((p1) ? (free((char *) (p1)), (p1) = 0) : 0);
if (debug & 0x1000) {
printf("MINCOV: family of all minimal coverings is\n");
sf_print(temp);
}
sf_free(T); return temp;
}

pset_family unate_intersect(pset_family A, pset_family B, int largest_only)
{
register pset pi, pj, lasti, lastj, pt;
pset_family T, Tsave;
int save;
int maxord, ord;
T = sf_new(500, A->sf_size);
Tsave = 0;
maxord = 0;
pt = T->data;
for( pi=A->data, lasti= pi+A->count*A->wsize; pi< lasti; pi+=A->wsize) {
for( pj=B->data, lastj= pj+B->count*B->wsize; pj< lastj; pj+=B->wsize) {
save = set_andp(pt, pi, pj);
if (save && largest_only) {
if ((ord = set_ord(pt)) > maxord) {
if (Tsave != 0) {
sf_free(Tsave);
Tsave = 0;
}
pt = T->data;
T->count = 0;
(void) set_and(pt, pi, pj);
maxord = ord;
} else if (ord < maxord) {
save = 0;
}
}
if (save) {
if (++T->count >= T->capacity) {
T = sf_contain(T);
Tsave = (Tsave == 0) ? T : sf_union(Tsave, T);
T = sf_new(500, A->sf_size);
pt = T->data;
} else {
pt += T->wsize;
}
}
}
}
T = sf_contain(T);
Tsave = (Tsave == 0) ? T : sf_union(Tsave, T);
return Tsave;
}

static pset_family
abs_covered(pset_family A, register int pick)
{
register pset last, p, pdest;
register pset_family Aprime;
Aprime = sf_new(A->count, A->sf_size);
pdest = Aprime->data;
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize)
if (! (p[((( pick) >> 5) + 1)] & (1 << (( pick) & (32-1))))) {
{register int i_=( p[0] & 0x03ff); do pdest[i_]= p[i_]; while (--i_>=0);};
Aprime->count++;
pdest += Aprime->wsize;
}
return Aprime;
}
static pset_family
abs_covered_many(pset_family A, register pset pick_set)
{
register pset last, p, pdest;
register pset_family Aprime;
Aprime = sf_new(A->count, A->sf_size);
pdest = Aprime->data;
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize)
if (setp_disjoint(p, pick_set)) {
{register int i_=( p[0] & 0x03ff); do pdest[i_]= p[i_]; while (--i_>=0);};
Aprime->count++;
pdest += Aprime->wsize;
}
return Aprime;
}
static int
abs_select_restricted(pset_family A, pset restrict)
{
register int i, best_var, best_count, *count;
count = sf_count_restricted(A, restrict);
best_var = -1;
best_count = 0;
for(i = 0; i < A->sf_size; i++) {
if (count[i] > best_count) {
best_var = i;
best_count = count[i];
}
}
((count) ? (free((char *) (count)), (count) = 0) : 0);
if (best_var == -1)
fatal("abs_select_restricted: should not have best_var == -1");
return best_var;
}
