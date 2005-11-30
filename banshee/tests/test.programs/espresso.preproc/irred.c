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
extern int cube_is_covered(pset *T, pset c);
extern int taut_special_cases(pset *T);
extern int tautology(pset *T);
extern pset_family irredundant(pset_family F, pset_family D);
extern void mark_irredundant(pset_family F, pset_family D);
extern void irred_split_cover(pset_family F, pset_family D, pset_family *E, pset_family *Rt, pset_family *Rp);
extern sm_matrix *irred_derive_table(pset_family D, pset_family E, pset_family Rp);
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
static void fcube_is_covered(pset *T, pset c, sm_matrix *table);
static void ftautology(pset *T, sm_matrix *table);
static int ftaut_special_cases(pset *T, sm_matrix *table);
static int Rp_current;

pset_family
irredundant(pset_family F, pset_family D)
{
mark_irredundant(F, D);
return sf_inactive(F);
}
void
mark_irredundant(pset_family F, pset_family D)
{
pset_family E, Rt, Rp;
pset p, p1, last;
sm_matrix *table;
sm_row *cover;
sm_element *pe;
irred_split_cover(F, D, &E, &Rt, &Rp);
table = irred_derive_table(D, E, Rp);
cover = sm_minimum_cover(table, ((int *) 0), 1, 0);
for( p=F->data, last= p+F->count*F->wsize; p< last; p+=F->wsize) {
(p[0] &= ~ ( 0x2000));
(p[0] &= ~ ( 0x0400));
}
for( p=E->data, last= p+E->count*E->wsize; p< last; p+=E->wsize) {
p1 = ((F)->data + (F)->wsize * ( (p[0] >> 16)));
{ if (! (setp_equal(p1, p))) { (void) fprintf((&_iob[2]), "Assertion failed: file %s, line %d\n\"%s\"\n", "irred.c", 49, "setp_equal(p1, p)"); (void) fflush((&_iob[1])); abort(); }};
(p1[0] |= ( 0x2000));
(p1[0] |= ( 0x0400));	}
for( pe = cover->first_col; pe != 0; pe = pe->next_col) {
p1 = ((F)->data + (F)->wsize * ( pe->col_num));
(p1[0] |= ( 0x2000));
}
if (debug & 0x0020) {
printf("# IRRED: F=%d E=%d R=%d Rt=%d Rp=%d Rc=%d Final=%d Bound=%d\n",
F->count, E->count, Rt->count+Rp->count, Rt->count, Rp->count,
cover->length, E->count + cover->length, 0);
}
sf_free(E);
sf_free(Rt);
sf_free(Rp);
sm_free(table);
sm_row_free(cover);
}

void
irred_split_cover(pset_family F, pset_family D, pset_family *E, pset_family *Rt, pset_family *Rp)
{
register pset p, last;
register int index;
pset_family R;
pset *FD, *ED;
index = 0;
for( p=F->data, last= p+F->count*F->wsize; p< last; p+=F->wsize) {
(p[0] &= 0xffff, p[0] |= (( index) << 16));
index++;
}
*E = sf_new(10, cube.size);
*Rt = sf_new(10, cube.size);
*Rp = sf_new(10, cube.size);
R = sf_new(10, cube.size);
FD = cube2list(F, D);
for( p=F->data, last= p+F->count*F->wsize; p< last; p+=F->wsize) {
if (cube_is_covered(FD, p)) {
R = sf_addset(R, p);
} else {
*E = sf_addset(*E, p);
}
if (debug & 0x4000) {
(void) printf("IRRED1: zr=%d ze=%d to-go=%d time=%s\n",
R->count, (*E)->count, F->count - (R->count + (*E)->count),
util_print_time(util_cpu_time()));
}
}
((FD[0]) ? (free((char *) (FD[0])), (FD[0]) = 0) : 0); ((FD) ? (free((char *) (FD)), (FD) = 0) : 0);;
ED = cube2list(*E, D);
for( p=R->data, last= p+R->count*R->wsize; p< last; p+=R->wsize) {
if (cube_is_covered(ED, p)) {
*Rt = sf_addset(*Rt, p);
} else {
*Rp = sf_addset(*Rp, p);
}
if (debug & 0x4000) {
(void) printf("IRRED1: zr=%d zrt=%d to-go=%d time=%s\n",
(*Rp)->count, (*Rt)->count,
R->count - ((*Rp)->count +(*Rt)->count), util_print_time(util_cpu_time()));
}
}
((ED[0]) ? (free((char *) (ED[0])), (ED[0]) = 0) : 0); ((ED) ? (free((char *) (ED)), (ED) = 0) : 0);;
sf_free(R);
}

sm_matrix *
irred_derive_table(pset_family D, pset_family E, pset_family Rp)
{
register pset last, p, *list;
sm_matrix *table;
int size_last_dominance, i;
for( p=D->data, last= p+D->count*D->wsize; p< last; p+=D->wsize) {
(p[0] &= ~ ( 0x1000));
}
for( p=E->data, last= p+E->count*E->wsize; p< last; p+=E->wsize) {
(p[0] &= ~ ( 0x1000));
}
for( p=Rp->data, last= p+Rp->count*Rp->wsize; p< last; p+=Rp->wsize) {
(p[0] |= ( 0x1000)); }
list = cube3list(D, E, Rp);
table = sm_alloc();
size_last_dominance = 0;
i = 0;
for( p=Rp->data, last= p+Rp->count*Rp->wsize; p< last; p+=Rp->wsize) {
Rp_current = (p[0] >> 16);
fcube_is_covered(list, p, table);
(p[0] &= ~ ( 0x1000));	if (debug & 0x4000) {
(void) printf("IRRED1: %d of %d to-go=%d, table=%dx%d time=%s\n",
i, Rp->count, Rp->count - i,
table->nrows, table->ncols, util_print_time(util_cpu_time()));
}
if (table->nrows - size_last_dominance > 1000) {
(void) sm_row_dominance(table);
size_last_dominance = table->nrows;
if (debug & 0x4000) {
(void) printf("IRRED1: delete redundant rows, now %dx%d\n",
table->nrows, table->ncols);
}
}
i++;
}
((list[0]) ? (free((char *) (list[0])), (list[0]) = 0) : 0); ((list) ? (free((char *) (list)), (list) = 0) : 0);;
return table;
}

int
cube_is_covered(pset *T, pset c)
{
return tautology(cofactor(T,c));
}
int
tautology(pset *T)
{
register pset cl, cr;
register int best, result;
static int taut_level = 0;
if (debug & 0x0200) {
debug_print(T, "TAUTOLOGY", taut_level++);
}
if ((result = taut_special_cases(T)) == 2) {
cl = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
cr = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
best = binate_split_select(T, cl, cr, 0x0200);
result = tautology(scofactor(T, cl, best)) &&
tautology(scofactor(T, cr, best));
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
((cl) ? (free((char *) (cl)), (cl) = 0) : 0);
((cr) ? (free((char *) (cr)), (cr) = 0) : 0);
}
if (debug & 0x0200) {
printf("exit TAUTOLOGY[%d]: %s\n", --taut_level, ((result) == 0 ? "FALSE" : ((result) == 1 ? "TRUE" : "MAYBE")));
}
return result;
}

int
taut_special_cases(pset *T)
{
register pset *T1, *Tsave, p, ceil=cube.temp[0], temp=cube.temp[1];
pset *A, *B;
int var;
for(T1 = T+2; (p = *T1++) != 0; ) {
if (full_row(p, T[0])) {
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
}
start:
{register int i_=( T[0][0] & 0x03ff); do ceil[i_]= T[0][i_]; while (--i_>=0);};
for(T1 = T+2; (p = *T1++) != 0; ) {
{register int i_=( ceil[0] & 0x03ff); (ceil[0] &= ~0x03ff, ceil[0] |= (i_)); do ceil[i_] = ceil[i_] | p[i_]; while (--i_>0);};
}
if (! setp_equal(ceil, cube.fullset)) {
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 0;
}
massive_count(T);
if (cdata.vars_unate == cdata.vars_active) {
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 0;
} else if (cdata.vars_active == 1) {
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
} else if (cdata.vars_unate != 0) {
(void) set_copy(ceil, cube.emptyset);
for(var = 0; var < cube.num_vars; var++) {
if (cdata.is_unate[var]) {
{register int i_=( ceil[0] & 0x03ff); (ceil[0] &= ~0x03ff, ceil[0] |= (i_)); do ceil[i_] = ceil[i_] | cube.var_mask[var][i_]; while (--i_>0);};
}
}
for(Tsave = T1 = T+2; (p = *T1++) != 0; ) {
if (setp_implies(ceil, set_or(temp, p, T[0]))) {
*Tsave++ = p;
}
}
*Tsave++ = 0;
T[1] = (pset) Tsave;
if (debug & 0x0200) {
printf("UNATE_REDUCTION: %d unate variables, reduced to %d\n",
cdata.vars_unate, (((pset *) T[1] - T) - 3));
}
goto start;
} else if (cdata.var_zeros[cdata.best] < (((pset *) T[1] - T) - 3) / 2) {
if (cubelist_partition(T, &A, &B, debug & 0x0200) == 0) {
return 2;
} else {
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
if (tautology(A)) {
((B[0]) ? (free((char *) (B[0])), (B[0]) = 0) : 0); ((B) ? (free((char *) (B)), (B) = 0) : 0);;
return 1;
} else {
return tautology(B);
}
}
}
return 2;
}

static void
fcube_is_covered(pset *T, pset c, sm_matrix *table)
{
ftautology(cofactor(T,c), table);
}
static void
ftautology(pset *T, sm_matrix *table)
{
register pset cl, cr;
register int best;
static int ftaut_level = 0;
if (debug & 0x0200) {
debug_print(T, "FIND_TAUTOLOGY", ftaut_level++);
}
if (ftaut_special_cases(T, table) == 2) {
cl = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
cr = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
best = binate_split_select(T, cl, cr, 0x0200);
ftautology(scofactor(T, cl, best), table);
ftautology(scofactor(T, cr, best), table);
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
((cl) ? (free((char *) (cl)), (cl) = 0) : 0);
((cr) ? (free((char *) (cr)), (cr) = 0) : 0);
}
if (debug & 0x0200) {
(void) printf("exit FIND_TAUTOLOGY[%d]: table is %d by %d\n",
--ftaut_level, table->nrows, table->ncols);
}
}

static int
ftaut_special_cases(pset *T, sm_matrix *table)
{
register pset *T1, *Tsave, p, temp = cube.temp[0], ceil = cube.temp[1];
int var, rownum;
for(T1 = T+2; (p = *T1++) != 0; ) {
if (! (p[0] & ( 0x1000))) {
if (full_row(p, T[0])) {
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
}
}
}
start:
massive_count(T);
if (cdata.vars_unate == cdata.vars_active) {
rownum = table->last_row ? table->last_row->row_num+1 : 0;
(void) sm_insert(table, rownum, Rp_current);
for(T1 = T+2; (p = *T1++) != 0; ) {
if ((p[0] & ( 0x1000))) {
if (full_row(p, T[0])) {
(void) sm_insert(table, rownum, (int) (p[0] >> 16));
}
}
}
((T[0]) ? (free((char *) (T[0])), (T[0]) = 0) : 0); ((T) ? (free((char *) (T)), (T) = 0) : 0);;
return 1;
} else if (cdata.vars_unate != 0) {
(void) set_copy(ceil, cube.emptyset);
for(var = 0; var < cube.num_vars; var++) {
if (cdata.is_unate[var]) {
{register int i_=( ceil[0] & 0x03ff); (ceil[0] &= ~0x03ff, ceil[0] |= (i_)); do ceil[i_] = ceil[i_] | cube.var_mask[var][i_]; while (--i_>0);};
}
}
for(Tsave = T1 = T+2; (p = *T1++) != 0; ) {
if (setp_implies(ceil, set_or(temp, p, T[0]))) {
*Tsave++ = p;
}
}
*Tsave++ = 0;
T[1] = (pset) Tsave;
if (debug & 0x0200) {
printf("UNATE_REDUCTION: %d unate variables, reduced to %d\n",
cdata.vars_unate, (((pset *) T[1] - T) - 3));
}
goto start;
}
return 2;
}
