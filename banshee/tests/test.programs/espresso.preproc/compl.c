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
extern pset_family complement(pset *T);
extern pset_family simplify(pset *T);
extern void simp_comp(pset *T, pset_family *Tnew, pset_family *Tbar);
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
int compl_special_cases();
pset_family compl_merge();
void compl_d1merge();
pset_family compl_cube();
void compl_lift();
void compl_lift_onset();
void compl_lift_onset_complex();
int simp_comp_special_cases();
int simplify_special_cases();
pset_family complement(pset *T)
{
register pset cl, cr;
register int best;
pset_family Tbar, Tl, Tr;
int lifting;
static int compl_level = 0;
if (debug & 0x0001)
debug_print(T, "COMPLEMENT", compl_level++);
if (compl_special_cases(T, &Tbar) == 2) {
cl = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
cr = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
best = binate_split_select(T, cl, cr, 0x0001);
Tl = complement(scofactor(T, cl, best));
Tr = complement(scofactor(T, cr, best));
if (Tr->count*Tl->count > (Tr->count+Tl->count)*(((pset *) T[1] - T) - 3)) {
lifting = 1;
} else {
lifting = 0;
}
Tbar = compl_merge(T, Tl, Tr, cl, cr, best, lifting);
((cl) ? (free((char *) (cl)), (cl) = 0) : 0);
((cr) ? (free((char *) (cr)), (cr) = 0) : 0);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
}
if (debug & 0x0001)
debug1_print(Tbar, "exit COMPLEMENT", --compl_level);
return Tbar;
}

static int compl_special_cases(pset *T, pset_family *Tbar)
{
register pset *T1, p, ceil, cof=T[0];
pset_family A, ceil_compl;
if (T[2] == 0) {
*Tbar = sf_addset(sf_new(1, cube.size), cube.fullset);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
if (T[3] == 0) {
*Tbar = compl_cube(set_or(cof, cof, T[2]));
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
for(T1 = T+2; (p = *T1++) != 0; ) {
if (full_row(p, cof)) {
*Tbar = sf_new(0, cube.size);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
}
ceil = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cof[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cof[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cof);
for(T1 = T+2; (p = *T1++) != 0; ) {
{register int i_=( ceil[0] & 0x03ff); (ceil[0] &= ~0x03ff, ceil[0] |= (i_)); do ceil[i_] = ceil[i_] | p[i_]; while (--i_>0);};
}
if (! setp_equal(ceil, cube.fullset)) {
ceil_compl = compl_cube(ceil);
(void) set_or(cof, cof, set_diff(ceil, cube.fullset, ceil));
((ceil) ? (free((char *) (ceil)), (ceil) = 0) : 0);
*Tbar = sf_append(complement(T), ceil_compl);
return 1;
}
((ceil) ? (free((char *) (ceil)), (ceil) = 0) : 0);
massive_count(T);
if (cdata.vars_active == 1) {
*Tbar = sf_new(0, cube.size);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
} else if (cdata.vars_unate == cdata.vars_active) {
A = map_cover_to_unate(T);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
A = unate_compl(A);
*Tbar = map_unate_to_cover(A);
sf_free(A);
return 1;
} else {
return 2;
}
}

static pset_family compl_merge(pset *T1, pset_family L, pset_family R, register pset cl, register pset cr, int var, int lifting)
{
register pset p, last, pt;
pset_family T, Tbar;
pset *L1, *R1;
if (debug & 0x0001) {
printf("compl_merge: left %d, right %d\n", L->count, R->count);
printf("%s (cl)\n%s (cr)\nLeft is\n", pc1(cl), pc2(cr));
cprint(L);
printf("Right is\n");
cprint(R);
}
for( p=L->data, last= p+L->count*L->wsize; p< last; p+=L->wsize) {
{register int i_=( p[0] & 0x03ff); (p[0] &= ~0x03ff, p[0] |= (i_)); do p[i_] = p[i_] & cl[i_]; while (--i_>0);};
(p[0] |= ( 0x2000));
}
for( p=R->data, last= p+R->count*R->wsize; p< last; p+=R->wsize) {
{register int i_=( p[0] & 0x03ff); (p[0] &= ~0x03ff, p[0] |= (i_)); do p[i_] = p[i_] & cr[i_]; while (--i_>0);};
(p[0] |= ( 0x2000));
}
(void) set_copy(cube.temp[0], cube.var_mask[var]);
qsort((char *) (L1 = sf_list(L)), L->count, sizeof(pset), d1_order);
qsort((char *) (R1 = sf_list(R)), R->count, sizeof(pset), d1_order);
compl_d1merge(L1, R1);
switch(lifting) {
case 1:
T = cubeunlist(T1);
compl_lift_onset(L1, T, cr, var);
compl_lift_onset(R1, T, cl, var);
sf_free(T);
break;
case 2:
T = cubeunlist(T1);
compl_lift_onset_complex(L1, T, var);
compl_lift_onset_complex(R1, T, var);
sf_free(T);
break;
case 0:
compl_lift(L1, R1, cr, var);
compl_lift(R1, L1, cl, var);
break;
case 3:
break;
}
((L1) ? (free((char *) (L1)), (L1) = 0) : 0);
((R1) ? (free((char *) (R1)), (R1) = 0) : 0);
Tbar = sf_new(L->count + R->count, cube.size);
pt = Tbar->data;
for( p=L->data, last= p+L->count*L->wsize; p< last; p+=L->wsize) {
{register int i_=( p[0] & 0x03ff); do pt[i_]= p[i_]; while (--i_>=0);};
Tbar->count++;
pt += Tbar->wsize;
}
for( p=R->data, last= p+R->count*R->wsize; p< last; p+=R->wsize) if (( p[0] & ( 0x2000))) {
{register int i_=( p[0] & 0x03ff); do pt[i_]= p[i_]; while (--i_>=0);};
Tbar->count++;
pt += Tbar->wsize;
}
if (debug & 0x0001) {
printf("Result %d\n", Tbar->count);
if (verbose_debug)
cprint(Tbar);
}
sf_free(L);
sf_free(R);
return Tbar;
}

static void compl_lift(pset *A1, pset *B1, pset bcube, int var)
{
register pset a, b, *B2, lift=cube.temp[4], liftor=cube.temp[5];
pset mask = cube.var_mask[var];
(void) set_and(liftor, bcube, mask);
for(; (a = *A1++) != 0; ) {
if ((a[0] & ( 0x2000))) {
(void) set_merge(lift, bcube, a, mask);
for(B2 = B1; (b = *B2++) != 0; ) {
{register int i_=(lift[0] & 0x03ff); do if (lift[i_]&~ b[i_]) break; while (--i_>0); if (i_ != 0) continue;};
{register int i_=( a[0] & 0x03ff); (a[0] &= ~0x03ff, a[0] |= (i_)); do a[i_] = a[i_] | liftor[i_]; while (--i_>0);};
break;
}
}
}
}
static void compl_lift_onset(pset *A1, pset_family T, pset bcube, int var)
{
register pset a, last, p, lift=cube.temp[4], mask=cube.var_mask[var];
for(; (a = *A1++) != 0; ) {
if ((a[0] & ( 0x2000))) {
{register int i_=( bcube[0] & 0x03ff); (lift[0] &= ~0x03ff, lift[0] |= (i_)); do lift[i_] = bcube[i_] & mask[i_]; while (--i_>0);};	{register int i_=( a[0] & 0x03ff); (lift[0] &= ~0x03ff, lift[0] |= (i_)); do lift[i_] = a[i_] | lift[i_]; while (--i_>0);};	for( p=T->data, last= p+T->count*T->wsize; p< last; p+=T->wsize) {
if (cdist0(p, lift)) {
goto nolift;
}
}
{register int i_=( lift[0] & 0x03ff); do a[i_]= lift[i_]; while (--i_>=0);};	(a[0] |= ( 0x2000));
nolift : ;
}
}
}

static void compl_lift_onset_complex(pset *A1, pset_family T, int var)
{
register int dist;
register pset last, p, a, xlower;
xlower = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
for(; (a = *A1++) != 0; ) {
if ((a[0] & ( 0x2000))) {
{register int i_=(( cube.size <= 32) ? 1 : (((( cube.size)-1) >> 5) + 1)); *xlower=i_; do xlower[i_] = 0; while (--i_ > 0);};
for( p=T->data, last= p+T->count*T->wsize; p< last; p+=T->wsize) {
if ((dist = cdist01(p, a)) < 2) {
if (dist == 0) {
fatal("compl: ON-set and OFF-set are not orthogonal");
} else {
(void) force_lower(xlower, p, a);
}
}
}
(void) set_diff(xlower, cube.var_mask[var], xlower);
(void) set_or(a, a, xlower);
((xlower) ? (free((char *) (xlower)), (xlower) = 0) : 0);
}
}
}
static void compl_d1merge(register pset *L1, register pset *R1)
{
register pset pl, pr;
for(pl = *L1, pr = *R1; (pl != 0) && (pr != 0); )
switch (d1_order(L1, R1)) {
case 1:
pr = *(++R1); break; case -1:
pl = *(++L1); break; case 0:
(pr[0] &= ~ ( 0x2000));
{register int i_=( pl[0] & 0x03ff); (pl[0] &= ~0x03ff, pl[0] |= (i_)); do pl[i_] = pl[i_] | pr[i_]; while (--i_>0);};
pr = *(++R1);
}
}
static pset_family compl_cube(register pset p)
{
register pset diff=cube.temp[7], pdest, mask, full=cube.fullset;
int var;
pset_family R;
R = sf_new(cube.num_vars, cube.size);
{register int i_=( full[0] & 0x03ff); (diff[0] &= ~0x03ff, diff[0] |= (i_)); do diff[i_] = full[i_] & ~ p[i_]; while (--i_>0);};
for(var = 0; var < cube.num_vars; var++) {
mask = cube.var_mask[var];
if (! setp_disjoint(diff, mask)) {
pdest = ((R)->data + (R)->wsize * ( R->count++));
{register int i_=( diff[0] & 0x03ff); (pdest[0] &= ~0x03ff, pdest[0] |= (i_)); do pdest[i_] = ( diff[i_]& mask[i_]) | ( full[i_]&~ mask[i_]); while (--i_>0);};
}
}
return R;
}

void simp_comp(pset *T, pset_family *Tnew, pset_family *Tbar)
{
register pset cl, cr;
register int best;
pset_family Tl, Tr, Tlbar, Trbar;
int lifting;
static int simplify_level = 0;
if (debug & 0x0001)
debug_print(T, "SIMPCOMP", simplify_level++);
if (simp_comp_special_cases(T, Tnew, Tbar) == 2) {
cl = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
cr = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
best = binate_split_select(T, cl, cr, 0x0001);
simp_comp(scofactor(T, cl, best), &Tl, &Tlbar);
simp_comp(scofactor(T, cr, best), &Tr, &Trbar);
lifting = 0;
*Tnew = compl_merge(T, Tl, Tr, cl, cr, best, lifting);
lifting = 0;
*Tbar = compl_merge(T, Tlbar, Trbar, cl, cr, best, lifting);
if ((*Tnew)->count > (((pset *) T[1] - T) - 3)) {
sf_free(*Tnew);
*Tnew = cubeunlist(T);
}
((cl) ? (free((char *) (cl)), (cl) = 0) : 0);
((cr) ? (free((char *) (cr)), (cr) = 0) : 0);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
}
if (debug & 0x0001) {
debug1_print(*Tnew, "exit SIMPCOMP (new)", simplify_level);
debug1_print(*Tbar, "exit SIMPCOMP (compl)", simplify_level);
simplify_level--;
}
}

static int simp_comp_special_cases(pset *T, pset_family *Tnew, pset_family *Tbar)
{
register pset *T1, p, ceil, cof=T[0];
pset last;
pset_family A;
if (T[2] == 0) {
*Tnew = sf_new(1, cube.size);
*Tbar = sf_addset(sf_new(1, cube.size), cube.fullset);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
if (T[3] == 0) {
(void) set_or(cof, cof, T[2]);
*Tnew = sf_addset(sf_new(1, cube.size), cof);
*Tbar = compl_cube(cof);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
for(T1 = T+2; (p = *T1++) != 0; ) {
if (full_row(p, cof)) {
*Tnew = sf_addset(sf_new(1, cube.size), cube.fullset);
*Tbar = sf_new(1, cube.size);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
}
ceil = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cof[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cof[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cof);
for(T1 = T+2; (p = *T1++) != 0; ) {
{register int i_=( ceil[0] & 0x03ff); (ceil[0] &= ~0x03ff, ceil[0] |= (i_)); do ceil[i_] = ceil[i_] | p[i_]; while (--i_>0);};
}
if (! setp_equal(ceil, cube.fullset)) {
p = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
(void) set_diff(p, cube.fullset, ceil);
(void) set_or(cof, cof, p);
((p) ? (free((char *) (p)), (p) = 0) : 0);
simp_comp(T, Tnew, Tbar);
A = *Tnew;
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
{register int i_=( p[0] & 0x03ff); (p[0] &= ~0x03ff, p[0] |= (i_)); do p[i_] = p[i_] & ceil[i_]; while (--i_>0);};
}
*Tbar = sf_append(*Tbar, compl_cube(ceil));
((ceil) ? (free((char *) (ceil)), (ceil) = 0) : 0);
return 1;
}
((ceil) ? (free((char *) (ceil)), (ceil) = 0) : 0);
massive_count(T);
if (cdata.vars_active == 1) {
*Tnew = sf_addset(sf_new(1, cube.size), cube.fullset);
*Tbar = sf_new(1, cube.size);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
} else if (cdata.vars_unate == cdata.vars_active) {
A = cubeunlist(T);
*Tnew = sf_contain(A);
A = map_cover_to_unate(T);
A = unate_compl(A);
*Tbar = map_unate_to_cover(A);
sf_free(A);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
} else {
return 2;
}
}

pset_family simplify(pset *T)
{
register pset cl, cr;
register int best;
pset_family Tbar, Tl, Tr;
int lifting;
static int simplify_level = 0;
if (debug & 0x0001) {
debug_print(T, "SIMPLIFY", simplify_level++);
}
if (simplify_special_cases(T, &Tbar) == 2) {
cl = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
cr = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
best = binate_split_select(T, cl, cr, 0x0001);
Tl = simplify(scofactor(T, cl, best));
Tr = simplify(scofactor(T, cr, best));
lifting = 0;
Tbar = compl_merge(T, Tl, Tr, cl, cr, best, lifting);
if (Tbar->count > (((pset *) T[1] - T) - 3)) {
sf_free(Tbar);
Tbar = cubeunlist(T);
}
((cl) ? (free((char *) (cl)), (cl) = 0) : 0);
((cr) ? (free((char *) (cr)), (cr) = 0) : 0);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
}
if (debug & 0x0001) {
debug1_print(Tbar, "exit SIMPLIFY", --simplify_level);
}
return Tbar;
}

static int simplify_special_cases(pset *T, pset_family *Tnew)
{
register pset *T1, p, ceil, cof=T[0];
pset last;
pset_family A;
if (T[2] == 0) {
*Tnew = sf_new(0, cube.size);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
if (T[3] == 0) {
*Tnew = sf_addset(sf_new(1, cube.size), set_or(cof, cof, T[2]));
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
for(T1 = T+2; (p = *T1++) != 0; ) {
if (full_row(p, cof)) {
*Tnew = sf_addset(sf_new(1, cube.size), cube.fullset);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
}
ceil = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cof[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cof[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cof);
for(T1 = T+2; (p = *T1++) != 0; ) {
{register int i_=( ceil[0] & 0x03ff); (ceil[0] &= ~0x03ff, ceil[0] |= (i_)); do ceil[i_] = ceil[i_] | p[i_]; while (--i_>0);};
}
if (! setp_equal(ceil, cube.fullset)) {
p = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
(void) set_diff(p, cube.fullset, ceil);
(void) set_or(cof, cof, p);
((p) ? (free((char *) (p)), (p) = 0) : 0);
A = simplify(T);
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
{register int i_=( p[0] & 0x03ff); (p[0] &= ~0x03ff, p[0] |= (i_)); do p[i_] = p[i_] & ceil[i_]; while (--i_>0);};
}
*Tnew = A;
((ceil) ? (free((char *) (ceil)), (ceil) = 0) : 0);
return 1;
}
((ceil) ? (free((char *) (ceil)), (ceil) = 0) : 0);
massive_count(T);
if (cdata.vars_active == 1) {
*Tnew = sf_addset(sf_new(1, cube.size), cube.fullset);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
} else if (cdata.vars_unate == cdata.vars_active) {
A = cubeunlist(T);
*Tnew = sf_contain(A);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
} else {
return 2;
}
}
