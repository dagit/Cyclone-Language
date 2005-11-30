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
extern output_phase_setup(pPLA PLA, int first_output);
extern pPLA set_phase(pPLA PLA);
extern pset_family opo(pset phase, pset_family T, pset_family D, pset_family R, int first_output);
extern pset find_phase(pPLA PLA, int first_output, pset phase1);
extern pset_family find_covers();
extern pset_family form_cover_table();
extern pset_family opo_leaf(register pset_family T, pset select, int out1, int out2);
extern pset_family opo_recur(pset_family T, pset_family D, pset select, int offset, int first, int last);
extern void opoall(pPLA PLA, int first_output, int last_output, int opo_strategy);
extern void phase_assignment(pPLA PLA, int opo_strategy);
extern void repeated_phase_assignment(pPLA PLA);
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

static int opo_no_make_sparse;
static int opo_repeated;
static int opo_exact;
void minimize();
void phase_assignment(pPLA PLA, int opo_strategy)
{
opo_no_make_sparse = opo_strategy % 2;
skip_make_sparse = opo_no_make_sparse;
opo_repeated = (opo_strategy / 2) % 2;
opo_exact = (opo_strategy / 4) % 2;
if (PLA->phase != 0) {
((PLA->phase) ? (free((char *) (PLA->phase)), (PLA->phase) = 0) : 0);
}
if (opo_repeated) {
PLA->phase = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cube.fullset[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cube.fullset[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cube.fullset);
repeated_phase_assignment(PLA);
} else {
PLA->phase = find_phase(PLA, 0, (pset) 0);
}
skip_make_sparse = 0;
(void) set_phase(PLA);
minimize(PLA);
}

void repeated_phase_assignment(pPLA PLA)
{
int i;
pset phase;
for(i = 0; i < cube.part_size[cube.output]; i++) {
phase = find_phase(PLA, i, PLA->phase);
if (! (phase[((( cube.first_part[cube.output] + i) >> 5) + 1)] & (1 << (( cube.first_part[cube.output] + i) & (32-1))))) {
(PLA->phase[((( cube.first_part[cube.output] + i) >> 5) + 1)] &= ~ (1 << (( cube.first_part[cube.output] + i) & (32-1))));
}
if (trace || summary) {
printf("\nOPO loop for output #%d\n", i);
printf("PLA->phase is %s\n", pc1(PLA->phase));
printf("phase      is %s\n", pc1(phase));
}
((phase) ? (free((char *) (phase)), (phase) = 0) : 0);
}
}
pset find_phase(pPLA PLA, int first_output, pset phase1)
{
pset phase;
pPLA PLA1;
phase = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cube.fullset[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cube.fullset[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cube.fullset);
PLA1 = new_PLA();
PLA1->F = sf_save(PLA->F);
PLA1->R = sf_save(PLA->R);
PLA1->D = sf_save(PLA->D);
if (phase1 != 0) {
PLA1->phase = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (phase1[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (phase1[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), phase1);
(void) set_phase(PLA1);
}
{long t=util_cpu_time();output_phase_setup(PLA1, first_output);if(summary)print_trace( PLA1->F, "OPO-SETUP ",util_cpu_time()-t);};
minimize(PLA1);
{long t=util_cpu_time();PLA1->F = opo(phase, PLA1->F, PLA1->D, PLA1->R, first_output);if(summary)print_trace( PLA1->F, "OPO       ",util_cpu_time()-t);};
free_PLA(PLA1);
setdown_cube();
cube.part_size[cube.output] -=
(cube.part_size[cube.output] - first_output) / 2;
cube_setup();
return phase;
}

pset_family opo(pset phase, pset_family T, pset_family D, pset_family R, int first_output)
{
int offset, output, i, last_output, ind;
pset pdest, select, p, p1, last, last1, not_covered, tmp;
pset_family temp, T1, T2;
select = set_fill(((unsigned int *) malloc(sizeof(unsigned int) * ( ((T->count) <= 32 ? 2 : (((((T->count)-1) >> 5) + 1) + 1))))), T->count);
for(output = 0; output < first_output; output++) {
ind = cube.first_part[cube.output] + output;
for( p=T->data, i=0; i<T->count; p+=T->wsize, i++) {
if ((p[((( ind) >> 5) + 1)] & (1 << (( ind) & (32-1))))) {
(select[((( i) >> 5) + 1)] &= ~ (1 << (( i) & (32-1))));
}
}
}
offset = (cube.part_size[cube.output] - first_output) / 2;
last_output = first_output + offset - 1;
temp = opo_recur(T, D, select, offset, first_output, last_output);
pdest = temp->data;
T1 = sf_new(T->count, cube.size);
for( p=T->data, i=0; i<T->count; p+=T->wsize, i++) {
if (! (pdest[((( i) >> 5) + 1)] & (1 << (( i) & (32-1))))) {
T1 = sf_addset(T1, p);
}
}
((select) ? (free((char *) (select)), (select) = 0) : 0);
sf_free(temp);
T2 = complement(cube1list(T1));
not_covered = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
tmp = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
for( p=T->data, last= p+T->count*T->wsize; p< last; p+=T->wsize) {
for( p1=T2->data, last1= p1+T2->count*T2->wsize; p1< last1; p1+=T2->wsize) {
if (cdist0(p, p1)) {
(void) set_or(not_covered, not_covered, set_and(tmp, p, p1));
}
}
}
sf_free(T);
sf_free(T2);
((tmp) ? (free((char *) (tmp)), (tmp) = 0) : 0);
for(output = first_output; output <= last_output; output++) {
ind = cube.first_part[cube.output] + output;
if ((not_covered[((( ind) >> 5) + 1)] & (1 << (( ind) & (32-1))))) {
if ((not_covered[((( ind + offset) >> 5) + 1)] & (1 << (( ind + offset) & (32-1))))) {
fatal("error in output phase assignment");
} else {
(phase[((( ind) >> 5) + 1)] &= ~ (1 << (( ind) & (32-1))));
}
}
}
((not_covered) ? (free((char *) (not_covered)), (not_covered) = 0) : 0);
return T1;
}

pset_family opo_recur(pset_family T, pset_family D, pset select, int offset, int first, int last)
{
static int level = 0;
int middle;
pset_family sl, sr, temp;
level++;
if (first == last) {
temp = opo_leaf(T, select, first, first + offset);
} else {
middle = (first + last) / 2;
sl = opo_recur(T, D, select, offset, first, middle);
sr = opo_recur(T, D, select, offset, middle+1, last);
temp = unate_intersect(sl, sr, level == 1);
if (trace) {
printf("# OPO[%d]: %4d = %4d x %4d, time = %s\n", level - 1,
temp->count, sl->count, sr->count, util_print_time(util_cpu_time()));
(void) fflush((&_iob[1]));
}
sf_free(sl);
sf_free(sr);
}
level--;
return temp;
}
pset_family opo_leaf(register pset_family T, pset select, int out1, int out2)
{
register pset_family temp;
register pset p, pdest;
register int i;
out1 += cube.first_part[cube.output];
out2 += cube.first_part[cube.output];
temp = sf_new(2, T->count);
pdest = ((temp)->data + (temp)->wsize * ( temp->count++));
(void) set_copy(pdest, select);
for( p=T->data, i=0; i<T->count; p+=T->wsize, i++) {
if ((p[((( out1) >> 5) + 1)] & (1 << (( out1) & (32-1))))) {
(pdest[((( i) >> 5) + 1)] &= ~ (1 << (( i) & (32-1))));
}
}
pdest = ((temp)->data + (temp)->wsize * ( temp->count++));
(void) set_copy(pdest, select);
for( p=T->data, i=0; i<T->count; p+=T->wsize, i++) {
if ((p[((( out2) >> 5) + 1)] & (1 << (( out2) & (32-1))))) {
(pdest[((( i) >> 5) + 1)] &= ~ (1 << (( i) & (32-1))));
}
}
return temp;
}

output_phase_setup(pPLA PLA, int first_output)
{
pset_family F, R, D;
pset mask, mask1, last;
int first_part, offset;
int save;
register pset p, pr, pf;
register int i, last_part;
if (cube.output == -1)
fatal("output_phase_setup: must have an output");
F = PLA->F;
D = PLA->D;
R = PLA->R;
first_part = cube.first_part[cube.output] + first_output;
last_part = cube.last_part[cube.output];
offset = cube.part_size[cube.output] - first_output;
setdown_cube();
cube.part_size[cube.output] += offset;
cube_setup();
mask = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cube.fullset[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cube.fullset[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cube.fullset);
for(i = first_part; i < cube.size; i++)
(mask[((( i) >> 5) + 1)] &= ~ (1 << (( i) & (32-1))));
mask1 = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (mask[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (mask[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), mask);
for(i = cube.first_part[cube.output]; i < first_part; i++) {
(mask1[((( i) >> 5) + 1)] &= ~ (1 << (( i) & (32-1))));
}
PLA->F = sf_new(F->count + R->count, cube.size);
PLA->R = sf_new(F->count + R->count, cube.size);
PLA->D = sf_new(D->count, cube.size);
for( p=F->data, last= p+F->count*F->wsize; p< last; p+=F->wsize) {
pf = ((PLA->F)->data + (PLA->F)->wsize * ( (PLA->F)->count++));
pr = ((PLA->R)->data + (PLA->R)->wsize * ( (PLA->R)->count++));
{register int i_=( mask[0] & 0x03ff); (pf[0] &= ~0x03ff, pf[0] |= (i_)); do pf[i_] = mask[i_] & p[i_]; while (--i_>0);};
{register int i_=( mask1[0] & 0x03ff); (pr[0] &= ~0x03ff, pr[0] |= (i_)); do pr[i_] = mask1[i_] & p[i_]; while (--i_>0);};
for(i = first_part; i <= last_part; i++)
if ((p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))))
(pf[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1)));
save = 0;
for(i = first_part; i <= last_part; i++)
if ((p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))))
save = 1, (pr[((( i+offset) >> 5) + 1)] |= 1 << (( i+offset) & (32-1)));
if (! save) PLA->R->count--;
}
for( p=R->data, last= p+R->count*R->wsize; p< last; p+=R->wsize) {
pf = ((PLA->F)->data + (PLA->F)->wsize * ( (PLA->F)->count++));
pr = ((PLA->R)->data + (PLA->R)->wsize * ( (PLA->R)->count++));
{register int i_=( mask1[0] & 0x03ff); (pf[0] &= ~0x03ff, pf[0] |= (i_)); do pf[i_] = mask1[i_] & p[i_]; while (--i_>0);};
{register int i_=( mask[0] & 0x03ff); (pr[0] &= ~0x03ff, pr[0] |= (i_)); do pr[i_] = mask[i_] & p[i_]; while (--i_>0);};
save = 0;
for(i = first_part; i <= last_part; i++)
if ((p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))))
save = 1, (pf[((( i+offset) >> 5) + 1)] |= 1 << (( i+offset) & (32-1)));
if (! save) PLA->F->count--;
for(i = first_part; i <= last_part; i++)
if ((p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))))
(pr[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1)));
}
for( p=D->data, last= p+D->count*D->wsize; p< last; p+=D->wsize) {
pf = ((PLA->D)->data + (PLA->D)->wsize * ( (PLA->D)->count++));
{register int i_=( mask[0] & 0x03ff); (pf[0] &= ~0x03ff, pf[0] |= (i_)); do pf[i_] = mask[i_] & p[i_]; while (--i_>0);};
for(i = first_part; i <= last_part; i++)
if ((p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1))))) {
(pf[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1)));
(pf[((( i+offset) >> 5) + 1)] |= 1 << (( i+offset) & (32-1)));
}
}
sf_free(F);
sf_free(D);
sf_free(R);
((mask) ? (free((char *) (mask)), (mask) = 0) : 0);
((mask1) ? (free((char *) (mask1)), (mask1) = 0) : 0);
}

pPLA set_phase(pPLA PLA)
{
pset_family F1, R1;
register pset last, p, outmask;
register pset temp=cube.temp[0], phase=PLA->phase, phase1=cube.temp[1];
outmask = cube.var_mask[cube.num_vars - 1];
set_diff(phase1, outmask, phase);
set_or(phase1, set_diff(temp, cube.fullset, outmask), phase1);
F1 = sf_new((PLA->F)->count + (PLA->R)->count, cube.size);
R1 = sf_new((PLA->F)->count + (PLA->R)->count, cube.size);
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize) {
if (! setp_disjoint(set_and(temp, p, phase), outmask))
set_copy(((F1)->data + (F1)->wsize * ( F1->count++)), temp);
if (! setp_disjoint(set_and(temp, p, phase1), outmask))
set_copy(((R1)->data + (R1)->wsize * ( R1->count++)), temp);
}
for( p=PLA->R->data, last= p+PLA->R->count*PLA->R->wsize; p< last; p+=PLA->R->wsize) {
if (! setp_disjoint(set_and(temp, p, phase), outmask))
set_copy(((R1)->data + (R1)->wsize * ( R1->count++)), temp);
if (! setp_disjoint(set_and(temp, p, phase1), outmask))
set_copy(((F1)->data + (F1)->wsize * ( F1->count++)), temp);
}
sf_free(PLA->F);
sf_free(PLA->R);
PLA->F = F1;
PLA->R = R1;
return PLA;
}

void opoall(pPLA PLA, int first_output, int last_output, int opo_strategy)
{
pset_family F, D, R, best_F, best_D, best_R;
int i, j, ind, num;
pset bestphase;
opo_exact = opo_strategy;
if (PLA->phase != 0) {
((PLA->phase) ? (free((char *) (PLA->phase)), (PLA->phase) = 0) : 0);
}
bestphase = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cube.fullset[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cube.fullset[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cube.fullset);
best_F = sf_save(PLA->F);
best_D = sf_save(PLA->D);
best_R = sf_save(PLA->R);
for(i = 0; i < (1 << (last_output - first_output + 1)); i++) {
F = sf_save(PLA->F);
D = sf_save(PLA->D);
R = sf_save(PLA->R);
PLA->phase = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cube.fullset[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cube.fullset[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cube.fullset);
num = i;
for(j = last_output; j >= first_output; j--) {
if (num % 2 == 0) {
ind = cube.first_part[cube.output] + j;
(PLA->phase[((( ind) >> 5) + 1)] &= ~ (1 << (( ind) & (32-1))));
}
num /= 2;
}
(void) set_phase(PLA);
printf("# phase is %s\n", pc1(PLA->phase));
summary = 1;
minimize(PLA);
if (PLA->F->count < best_F->count) {
set_copy(bestphase, PLA->phase);
sf_free(best_F);
sf_free(best_D);
sf_free(best_R);
best_F = PLA->F;
best_D = PLA->D;
best_R = PLA->R;
} else {
sf_free(PLA->F);
sf_free(PLA->D);
sf_free(PLA->R);
}
((PLA->phase) ? (free((char *) (PLA->phase)), (PLA->phase) = 0) : 0);
PLA->F = F;
PLA->D = D;
PLA->R = R;
}
PLA->phase = bestphase;
sf_free(PLA->F);
sf_free(PLA->D);
sf_free(PLA->R);
PLA->F = best_F;
PLA->D = best_D;
PLA->R = best_R;
}

static void minimize(pPLA PLA)
{
if (opo_exact) {
{long t=util_cpu_time();PLA->F = minimize_exact(PLA->F,PLA->D,PLA->R,1);if(summary)print_trace( PLA->F, "EXACT",util_cpu_time()-t);};
} else {
{long t=util_cpu_time();PLA->F = espresso(PLA->F, PLA->D, PLA->R);if(summary)print_trace(PLA->F, "ESPRESSO  ",util_cpu_time()-t);};
}
}
