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
extern generate_all_pairs(ppair pair, int n, pset candidate, int (*action) ());
extern int **find_pairing_cost(pPLA PLA, int strategy);
extern int find_best_cost(register ppair pair);
extern int greedy_best_cost(int **cost_array_local, ppair *pair_p);
extern int minimize_pair(ppair pair);
extern int pair_free(register ppair pair);
extern pair_all(pPLA PLA, int pair_strategy);
extern pset_family delvar(pset_family A, int *paired);
extern pset_family pairvar(pset_family A, ppair pair);
extern ppair pair_best_cost(int **cost_array_local);
extern ppair pair_new(register int n);
extern ppair pair_save(register ppair pair, register int n);
extern print_pair(ppair pair);
extern void find_optimal_pairing(pPLA PLA, int strategy);
extern void set_pair(pPLA PLA);
extern void set_pair1(pPLA PLA, int adjust_labels);
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
void set_pair(pPLA PLA)
{
set_pair1(PLA, 1);
}
void set_pair1(pPLA PLA, int adjust_labels)
{
int i, var, *paired, newvar;
int old_num_vars, old_num_binary_vars, old_size, old_mv_start;
int *new_part_size, new_num_vars, new_num_binary_vars, new_mv_start;
ppair pair = PLA->pair;
char scratch[1000], **oldlabel, *var1, *var1bar, *var2, *var2bar;
if (adjust_labels)
makeup_labels(PLA);
paired = ((int *) malloc(sizeof(int) * ( cube.num_binary_vars)));
for(var = 0; var < cube.num_binary_vars; var++)
paired[var] = 0;
for(i = 0; i < pair->cnt; i++)
if ((pair->var1[i] > 0 && pair->var1[i] <= cube.num_binary_vars) &&
(pair->var2[i] > 0 && pair->var2[i] <= cube.num_binary_vars)) {
paired[pair->var1[i]-1] = 1;
paired[pair->var2[i]-1] = 1;
} else
fatal("can only pair binary-valued variables");
PLA->F = delvar(pairvar(PLA->F, pair), paired);
PLA->R = delvar(pairvar(PLA->R, pair), paired);
PLA->D = delvar(pairvar(PLA->D, pair), paired);
old_size = cube.size;
old_num_vars = cube.num_vars;
old_num_binary_vars = cube.num_binary_vars;
old_mv_start = cube.first_part[cube.num_binary_vars];
new_num_binary_vars = 0;
for(var = 0; var < old_num_binary_vars; var++)
new_num_binary_vars += (paired[var] == 0);
new_num_vars = new_num_binary_vars + pair->cnt;
new_num_vars += old_num_vars - old_num_binary_vars;
new_part_size = ((int *) malloc(sizeof(int) * ( new_num_vars)));
for(var = 0; var < pair->cnt; var++)
new_part_size[new_num_binary_vars + var] = 4;
for(var = 0; var < old_num_vars - old_num_binary_vars; var++)
new_part_size[new_num_binary_vars + pair->cnt + var] =
cube.part_size[old_num_binary_vars + var];
setdown_cube();
((cube.part_size) ? (free((char *) (cube.part_size)), (cube.part_size) = 0) : 0);
cube.num_vars = new_num_vars;
cube.num_binary_vars = new_num_binary_vars;
cube.part_size = new_part_size;
cube_setup();
if (adjust_labels) {
oldlabel = PLA->label;
PLA->label = ((char * *) malloc(sizeof(char *) * ( cube.size)));
for(var = 0; var < pair->cnt; var++) {
newvar = cube.num_binary_vars*2 + var*4;
var1 = oldlabel[ (pair->var1[var]-1) * 2 + 1];
var2 = oldlabel[ (pair->var2[var]-1) * 2 + 1];
var1bar = oldlabel[ (pair->var1[var]-1) * 2];
var2bar = oldlabel[ (pair->var2[var]-1) * 2];
(void) sprintf(scratch, "%s+%s", var1bar, var2bar);
PLA->label[newvar] = util_strsav(scratch);
(void) sprintf(scratch, "%s+%s", var1bar, var2);
PLA->label[newvar+1] = util_strsav(scratch);
(void) sprintf(scratch, "%s+%s", var1, var2bar);
PLA->label[newvar+2] = util_strsav(scratch);
(void) sprintf(scratch, "%s+%s", var1, var2);
PLA->label[newvar+3] = util_strsav(scratch);
}
i = 0;
for(var = 0; var < old_num_binary_vars; var++) {
if (paired[var] == 0) {
PLA->label[2*i] = oldlabel[2*var];
PLA->label[2*i+1] = oldlabel[2*var+1];
oldlabel[2*var] = oldlabel[2*var+1] = (char *) 0;
i++;
}
}
new_mv_start = cube.num_binary_vars*2 + pair->cnt*4;
for(i = old_mv_start; i < old_size; i++) {
PLA->label[new_mv_start + i - old_mv_start] = oldlabel[i];
oldlabel[i] = (char *) 0;
}
for(i = 0; i < old_size; i++)
if (oldlabel[i] != (char *) 0)
((oldlabel[i]) ? (free((char *) (oldlabel[i])), (oldlabel[i]) = 0) : 0);
((oldlabel) ? (free((char *) (oldlabel)), (oldlabel) = 0) : 0);
}
for(var = 0; var < pair->cnt; var++)
cube.sparse[cube.num_binary_vars + var] = 0;
((paired) ? (free((char *) (paired)), (paired) = 0) : 0);
}

pset_family pairvar(pset_family A, ppair pair)
{
register pset last, p;
register int val, p1, p2, b1, b0;
int insert_col, pairnum;
insert_col = cube.first_part[cube.num_vars - 1];
A = sf_delcol(A, insert_col, -4*pair->cnt);
for( p=A->data, last= p+A->count*A->wsize; p< last; p+=A->wsize) {
for(pairnum = 0; pairnum < pair->cnt; pairnum++) {
p1 = cube.first_part[pair->var1[pairnum] - 1];
p2 = cube.first_part[pair->var2[pairnum] - 1];
b1 = (p[((( p2+1) >> 5) + 1)] & (1 << (( p2+1) & (32-1))));
b0 = (p[((( p2) >> 5) + 1)] & (1 << (( p2) & (32-1))));
val = insert_col + pairnum * 4;
if ( (p[((( p1) >> 5) + 1)] & (1 << (( p1) & (32-1))))) {
if (b0)
(p[((( val + 3) >> 5) + 1)] |= 1 << (( val + 3) & (32-1)));
if (b1)
(p[((( val + 2) >> 5) + 1)] |= 1 << (( val + 2) & (32-1)));
}
if ( (p[((( p1+1) >> 5) + 1)] & (1 << (( p1+1) & (32-1))))) {
if (b0)
(p[((( val + 1) >> 5) + 1)] |= 1 << (( val + 1) & (32-1)));
if (b1)
(p[((( val) >> 5) + 1)] |= 1 << (( val) & (32-1)));
}
}
}
return A;
}
pset_family delvar(pset_family A, int *paired)
{
int run;
int first_run, run_length, var, offset = 0;
run = 0; run_length = 0;
for(var = 0; var < cube.num_binary_vars; var++)
if (paired[var])
if (run)
run_length += cube.part_size[var];
else {
run = 1;
first_run = cube.first_part[var];
run_length = cube.part_size[var];
}
else
if (run) {
A = sf_delcol(A, first_run-offset, run_length);
run = 0;
offset += run_length;
}
if (run)
A = sf_delcol(A, first_run-offset, run_length);
return A;
}

void find_optimal_pairing(pPLA PLA, int strategy)
{
int i, j, **cost_array;
cost_array = find_pairing_cost(PLA, strategy);
if (summary) {
printf("    ");
for(i = 0; i < cube.num_binary_vars; i++)
printf("%3d ", i+1);
printf("\n");
for(i = 0; i < cube.num_binary_vars; i++) {
printf("%3d ", i+1);
for(j = 0; j < cube.num_binary_vars; j++)
printf("%3d ", cost_array[i][j]);
printf("\n");
}
}
if (cube.num_binary_vars <= 14) {
PLA->pair = pair_best_cost(cost_array);
} else {
(void) greedy_best_cost(cost_array, &(PLA->pair));
}
printf("# ");
print_pair(PLA->pair);
for(i = 0; i < cube.num_binary_vars; i++)
((cost_array[i]) ? (free((char *) (cost_array[i])), (cost_array[i]) = 0) : 0);
((cost_array) ? (free((char *) (cost_array)), (cost_array) = 0) : 0);
set_pair(PLA);
{long t=util_cpu_time();PLA->F=espresso(PLA->F,PLA->D,PLA->R);if(summary)print_trace(PLA->F,"ESPRESSO  ",util_cpu_time()-t);};
}

int **find_pairing_cost(pPLA PLA, int strategy)
{
int var1, var2, **cost_array;
int i, j, xnum_binary_vars, xnum_vars, *xpart_size, cost;
pset_family T, Fsave, Dsave, Rsave;
pset mask;
cost_array = ((int * *) malloc(sizeof(int *) * ( cube.num_binary_vars)));
for(i = 0; i < cube.num_binary_vars; i++)
cost_array[i] = ((int *) malloc(sizeof(int) * ( cube.num_binary_vars)));
for(i = 0; i < cube.num_binary_vars; i++)
for(j = 0; j < cube.num_binary_vars; j++)
cost_array[i][j] = 0;
PLA->pair = pair_new(1);
PLA->pair->cnt = 1;
for(var1 = 0; var1 < cube.num_binary_vars-1; var1++) {
for(var2 = var1+1; var2 < cube.num_binary_vars; var2++) {
if (strategy > 0) {
Fsave = sf_save(PLA->F);
Dsave = sf_save(PLA->D);
Rsave = sf_save(PLA->R);
xnum_binary_vars = cube.num_binary_vars;
xnum_vars = cube.num_vars;
xpart_size = ((int *) malloc(sizeof(int) * ( cube.num_vars)));
for(i = 0; i < cube.num_vars; i++)
xpart_size[i] = cube.part_size[i];
PLA->pair->var1[0] = var1 + 1;
PLA->pair->var2[0] = var2 + 1;
set_pair1(PLA, 0);
}
switch(strategy) {
case 3:
PLA->F = minimize_exact(PLA->F, PLA->D, PLA->R, 1);
cost = Fsave->count - PLA->F->count;
break;
case 2:
PLA->F = espresso(PLA->F, PLA->D, PLA->R);
cost = Fsave->count - PLA->F->count;
break;
case 1:
PLA->F = reduce(PLA->F, PLA->D);
PLA->F = expand(PLA->F, PLA->R, 0);
PLA->F = irredundant(PLA->F, PLA->D);
cost = Fsave->count - PLA->F->count;
break;
case 0:
mask = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
set_or(mask, cube.var_mask[var1], cube.var_mask[var2]);
T = dist_merge(sf_save(PLA->F), mask);
cost = PLA->F->count - T->count;
sf_free(T);
((mask) ? (free((char *) (mask)), (mask) = 0) : 0);
}
cost_array[var1][var2] = cost;
if (strategy > 0) {
setdown_cube();
((cube.part_size) ? (free((char *) (cube.part_size)), (cube.part_size) = 0) : 0);
cube.num_binary_vars = xnum_binary_vars;
cube.num_vars = xnum_vars;
cube.part_size = xpart_size;
cube_setup();
sf_free(PLA->F);
sf_free(PLA->D);
sf_free(PLA->R);
PLA->F = Fsave;
PLA->D = Dsave;
PLA->R = Rsave;
}
}
}
pair_free(PLA->pair);
PLA->pair = 0;
return cost_array;
}

static int best_cost;
static int **cost_array;
static ppair best_pair;
static pset best_phase;
static pPLA global_PLA;
static pset_family best_F, best_D, best_R;
static int pair_minim_strategy;
print_pair(ppair pair)
{
int i;
printf("pair is");
for(i = 0; i < pair->cnt; i++)
printf (" (%d %d)", pair->var1[i], pair->var2[i]);
printf("\n");
}
int greedy_best_cost(int **cost_array_local, ppair *pair_p)
{
int i, j, besti, bestj, maxcost, total_cost;
pset cand;
ppair pair;
pair = pair_new(cube.num_binary_vars);
cand = set_fill(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.num_binary_vars) <= 32 ? 2 : (((((cube.num_binary_vars)-1) >> 5) + 1) + 1))))), cube.num_binary_vars);
total_cost = 0;
while (set_ord(cand) >= 2) {
maxcost = -1;
for(i = 0; i < cube.num_binary_vars; i++) {
if ((cand[((( i) >> 5) + 1)] & (1 << (( i) & (32-1))))) {
for(j = i+1; j < cube.num_binary_vars; j++) {
if ((cand[((( j) >> 5) + 1)] & (1 << (( j) & (32-1))))) {
if (cost_array_local[i][j] > maxcost) {
maxcost = cost_array_local[i][j];
besti = i;
bestj = j;
}
}
}
}
}
pair->var1[pair->cnt] = besti+1;
pair->var2[pair->cnt] = bestj+1;
pair->cnt++;
(cand[((( besti) >> 5) + 1)] &= ~ (1 << (( besti) & (32-1))));
(cand[((( bestj) >> 5) + 1)] &= ~ (1 << (( bestj) & (32-1))));
total_cost += maxcost;
}
((cand) ? (free((char *) (cand)), (cand) = 0) : 0);
*pair_p = pair;
return total_cost;
}
ppair pair_best_cost(int **cost_array_local)
{
ppair pair;
pset candidate;
best_cost = -1;
best_pair = 0;
cost_array = cost_array_local;
pair = pair_new(cube.num_binary_vars);
candidate = set_fill(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.num_binary_vars) <= 32 ? 2 : (((((cube.num_binary_vars)-1) >> 5) + 1) + 1))))), cube.num_binary_vars);
generate_all_pairs(pair, cube.num_binary_vars, candidate, find_best_cost);
pair_free(pair);
((candidate) ? (free((char *) (candidate)), (candidate) = 0) : 0);
return best_pair;
}
int find_best_cost(register ppair pair)
{
register int i, cost;
cost = 0;
for(i = 0; i < pair->cnt; i++)
cost += cost_array[pair->var1[i]-1][pair->var2[i]-1];
if (cost > best_cost) {
best_cost = cost;
best_pair = pair_save(pair, pair->cnt);
}
if ((debug & 0x0800) && trace) {
printf("cost is %d ", cost);
print_pair(pair);
}
}

pair_all(pPLA PLA, int pair_strategy)
{
ppair pair;
pset candidate;
global_PLA = PLA;
pair_minim_strategy = pair_strategy;
best_cost = PLA->F->count + 1;
best_pair = 0;
best_phase = 0;
best_F = best_D = best_R = 0;
pair = pair_new(cube.num_binary_vars);
candidate = set_fill(set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.num_binary_vars) <= 32 ? 2 : (((((cube.num_binary_vars)-1) >> 5) + 1) + 1))))), cube.num_binary_vars), cube.num_binary_vars);
generate_all_pairs(pair, cube.num_binary_vars, candidate, minimize_pair);
pair_free(pair);
((candidate) ? (free((char *) (candidate)), (candidate) = 0) : 0);
PLA->pair = best_pair;
PLA->phase = best_phase;
set_pair(PLA);
printf("# ");
print_pair(PLA->pair);
sf_free(PLA->F);
sf_free(PLA->D);
sf_free(PLA->R);
PLA->F = best_F;
PLA->D = best_D;
PLA->R = best_R;
}
int minimize_pair(ppair pair)
{
pset_family Fsave, Dsave, Rsave;
int i, xnum_binary_vars, xnum_vars, *xpart_size;
Fsave = sf_save(global_PLA->F);
Dsave = sf_save(global_PLA->D);
Rsave = sf_save(global_PLA->R);
xnum_binary_vars = cube.num_binary_vars;
xnum_vars = cube.num_vars;
xpart_size = ((int *) malloc(sizeof(int) * ( cube.num_vars)));
for(i = 0; i < cube.num_vars; i++)
xpart_size[i] = cube.part_size[i];
global_PLA->pair = pair;
set_pair1(global_PLA, 0);
if (summary)
print_pair(pair);
switch(pair_minim_strategy) {
case 2:
{long t=util_cpu_time();phase_assignment(global_PLA,0);if(summary)print_trace( global_PLA->F, "OPO       ",util_cpu_time()-t);};
if (summary)
printf("# phase is %s\n", pc1(global_PLA->phase));
break;
case 1:
{long t=util_cpu_time();global_PLA->F = minimize_exact(global_PLA->F, global_PLA->D, global_PLA->R, 1);if(summary)print_trace( global_PLA->F, "EXACT     ",util_cpu_time()-t);};
break;
case 0:
{long t=util_cpu_time();global_PLA->F = espresso(global_PLA->F, global_PLA->D, global_PLA->R);if(summary)print_trace( global_PLA->F, "ESPRESSO  ",util_cpu_time()-t);};
break;
default:
break;
}
if (global_PLA->F->count < best_cost) {
best_cost = global_PLA->F->count;
best_pair = pair_save(pair, pair->cnt);
best_phase = global_PLA->phase!=0?set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (global_PLA->phase[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (global_PLA->phase[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), global_PLA->phase):0;
if (best_F != 0) sf_free(best_F);
if (best_D != 0) sf_free(best_D);
if (best_R != 0) sf_free(best_R);
best_F = sf_save(global_PLA->F);
best_D = sf_save(global_PLA->D);
best_R = sf_save(global_PLA->R);
}
setdown_cube();
((cube.part_size) ? (free((char *) (cube.part_size)), (cube.part_size) = 0) : 0);
cube.num_binary_vars = xnum_binary_vars;
cube.num_vars = xnum_vars;
cube.part_size = xpart_size;
cube_setup();
sf_free(global_PLA->F);
sf_free(global_PLA->D);
sf_free(global_PLA->R);
global_PLA->F = Fsave;
global_PLA->D = Dsave;
global_PLA->R = Rsave;
global_PLA->pair = 0;
global_PLA->phase = 0;
}

generate_all_pairs(ppair pair, int n, pset candidate, int (*action) ())
{
int i, j;
pset recur_candidate;
ppair recur_pair;
if (set_ord(candidate) < 2) {
(*action)(pair);
return;
}
recur_pair = pair_save(pair, n);
recur_candidate = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (candidate[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (candidate[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), candidate);
for(i = 0; i < n; i++)
if ((candidate[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))))
break;
for(j = i+1; j < n; j++)
if ((candidate[((( j) >> 5) + 1)] & (1 << (( j) & (32-1))))) {
(recur_candidate[((( i) >> 5) + 1)] &= ~ (1 << (( i) & (32-1))));
(recur_candidate[((( j) >> 5) + 1)] &= ~ (1 << (( j) & (32-1))));
recur_pair->var1[recur_pair->cnt] = i+1;
recur_pair->var2[recur_pair->cnt] = j+1;
recur_pair->cnt++;
generate_all_pairs(recur_pair, n, recur_candidate, action);
recur_pair->cnt--;
(recur_candidate[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1)));
(recur_candidate[((( j) >> 5) + 1)] |= 1 << (( j) & (32-1)));
}
if ((set_ord(candidate) % 2) == 1) {
(recur_candidate[((( i) >> 5) + 1)] &= ~ (1 << (( i) & (32-1))));
generate_all_pairs(recur_pair, n, recur_candidate, action);
}
pair_free(recur_pair);
((recur_candidate) ? (free((char *) (recur_candidate)), (recur_candidate) = 0) : 0);
}

ppair pair_new(register int n)
{
register ppair pair1;
pair1 = ((pair_t *) malloc(sizeof(pair_t) * ( 1)));
pair1->cnt = 0;
pair1->var1 = ((int *) malloc(sizeof(int) * ( n)));
pair1->var2 = ((int *) malloc(sizeof(int) * ( n)));
return pair1;
}
ppair pair_save(register ppair pair, register int n)
{
register int k;
register ppair pair1;
pair1 = pair_new(n);
pair1->cnt = pair->cnt;
for(k = 0; k < pair->cnt; k++) {
pair1->var1[k] = pair->var1[k];
pair1->var2[k] = pair->var2[k];
}
return pair1;
}
int pair_free(register ppair pair)
{
((pair->var1) ? (free((char *) (pair->var1)), (pair->var1) = 0) : 0);
((pair->var2) ? (free((char *) (pair->var2)), (pair->var2) = 0) : 0);
((pair) ? (free((char *) (pair)), (pair) = 0) : 0);
}
