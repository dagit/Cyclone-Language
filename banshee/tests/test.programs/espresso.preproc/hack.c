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
extern find_inputs(pset_family A, pPLA PLA, symbolic_list_t *list, int base, int value, pset_family *newF, pset_family *newD);
extern form_bitvector(pset p, int base, int value, symbolic_list_t *list);
extern map_dcset(pPLA PLA);
extern map_output_symbolic(pPLA PLA);
extern map_symbolic(pPLA PLA);
extern pset_family map_symbolic_cover(pset_family T, symbolic_list_t *list, int base);
extern symbolic_hack_labels(pPLA PLA, symbolic_t *list, pset compress, int new_size, int old_size, int size_added);
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
map_dcset(pPLA PLA)
{
int var, i;
pset_family Tplus, Tminus, Tplusbar, Tminusbar;
pset_family newf, term1, term2, dcset, dcsetbar;
pset cplus, cminus, last, p;
if (PLA->label == ((char * *) 0) || PLA->label[0] == ((char *) 0))
return;
var = -1;
for(i = 0; i < cube.num_binary_vars * 2; i++) {
if (strncmp(PLA->label[i], "DONT_CARE", 9) == 0 ||
strncmp(PLA->label[i], "DONTCARE", 8) == 0 ||
strncmp(PLA->label[i], "dont_care", 9) == 0 ||
strncmp(PLA->label[i], "dontcare", 8) == 0) {
var = i/2;
break;
}
}
if (var == -1) {
return;
}
cplus = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cube.fullset[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cube.fullset[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cube.fullset);
cminus = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cube.fullset[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cube.fullset[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cube.fullset);
(cplus[((( var*2) >> 5) + 1)] &= ~ (1 << (( var*2) & (32-1))));
(cminus[((( var*2 + 1) >> 5) + 1)] &= ~ (1 << (( var*2 + 1) & (32-1))));
{long t=util_cpu_time();simp_comp(cofactor(cube1list(PLA->F), cplus), &Tplus, &Tplusbar);if(trace)print_trace( Tplus, "simpcomp+",util_cpu_time()-t);};
{long t=util_cpu_time();simp_comp(cofactor(cube1list(PLA->F), cminus), &Tminus, &Tminusbar);if(trace)print_trace( Tminus, "simpcomp-",util_cpu_time()-t);};
{long t=util_cpu_time();term1 = cv_intersect(Tplus, Tminusbar);if(trace)print_trace( term1, "term1    ",util_cpu_time()-t);};
{long t=util_cpu_time();term2 = cv_intersect(Tminus, Tplusbar);if(trace)print_trace( term2, "term2    ",util_cpu_time()-t);};
{long t=util_cpu_time();dcset = sf_union(term1, term2);if(trace)print_trace( dcset, "union     ",util_cpu_time()-t);};
{long t=util_cpu_time();simp_comp(cube1list(dcset), &PLA->D, &dcsetbar);if(trace)print_trace( PLA->D, "simplify",util_cpu_time()-t);};
{long t=util_cpu_time();newf = cv_intersect(PLA->F, dcsetbar);if(trace)print_trace( PLA->F, "separate  ",util_cpu_time()-t);};
sf_free(PLA->F);
PLA->F = newf;
sf_free(Tplus);
sf_free(Tminus);
sf_free(Tplusbar);
sf_free(Tminusbar);
sf_free(dcsetbar);
(void) sf_active(PLA->F);
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize) {
if (! (p[((( var*2) >> 5) + 1)] & (1 << (( var*2) & (32-1)))) || ! (p[((( var*2+1) >> 5) + 1)] & (1 << (( var*2+1) & (32-1))))) {
(p[0] &= ~ ( 0x2000));
}
}
PLA->F = sf_inactive(PLA->F);
setdown_cube();
for(i = 2*var+2; i < cube.size; i++) {
PLA->label[i-2] = PLA->label[i];
}
for(i = var+1; i < cube.num_vars; i++) {
cube.part_size[i-1] = cube.part_size[i];
}
cube.num_binary_vars--;
cube.num_vars--;
cube_setup();
PLA->F = sf_delc(PLA->F, 2*var, 2*var+1);
PLA->D = sf_delc(PLA->D, 2*var, 2*var+1);
}

map_output_symbolic(pPLA PLA)
{
pset_family newF, newD;
pset compress;
symbolic_t *p1;
symbolic_list_t *p2;
int i, bit, tot_size, base, old_size;
if (PLA->D->count > 0) {
sf_free(PLA->F);
PLA->F = complement(cube2list(PLA->D, PLA->R));
}
tot_size = 0;
for(p1=PLA->symbolic_output; p1!=((symbolic_t *) 0); p1=p1->next) {
for(p2=p1->symbolic_list; p2!=((symbolic_list_t *) 0); p2=p2->next) {
if (p2->pos<0 || p2->pos>=cube.part_size[cube.output]) {
fatal("symbolic-output index out of range");
}
}
tot_size += 1 << p1->symbolic_list_length;
}
for(p1=PLA->symbolic_output; p1!=((symbolic_t *) 0); p1=p1->next) {
for(p2=p1->symbolic_list; p2!=((symbolic_list_t *) 0); p2=p2->next) {
p2->pos += tot_size;
}
}
old_size = cube.size;
cube.part_size[cube.output] += tot_size;
setdown_cube();
cube_setup();
base = cube.first_part[cube.output];
PLA->F = sf_addcol(PLA->F, base, tot_size);
PLA->D = sf_addcol(PLA->D, base, tot_size);
PLA->R = sf_addcol(PLA->R, base, tot_size);
for(p1=PLA->symbolic_output; p1!=((symbolic_t *) 0); p1=p1->next) {
newF = sf_new(100, cube.size);
newD = sf_new(100, cube.size);
find_inputs(((set_family_t *) 0), PLA, p1->symbolic_list, base, 0,
&newF, &newD);
sf_free(PLA->F);
PLA->F = newF;
sf_free(newD);
base += 1 << p1->symbolic_list_length;
}
compress = set_fill(((unsigned int *) malloc(sizeof(unsigned int) * ( ((newF->sf_size) <= 32 ? 2 : (((((newF->sf_size)-1) >> 5) + 1) + 1))))), newF->sf_size);
for(p1=PLA->symbolic_output; p1!=((symbolic_t *) 0); p1=p1->next) {
for(p2=p1->symbolic_list; p2!=((symbolic_list_t *) 0); p2=p2->next) {
bit = cube.first_part[cube.output] + p2->pos;
(compress[((( bit) >> 5) + 1)] &= ~ (1 << (( bit) & (32-1))));
}
}
cube.part_size[cube.output] -= newF->sf_size - set_ord(compress);
setdown_cube();
cube_setup();
PLA->F = sf_compress(PLA->F, compress);
PLA->D = sf_compress(PLA->D, compress);
if (cube.size != PLA->F->sf_size) fatal("error");
PLA->F = sf_contain(PLA->F);
PLA->D = sf_contain(PLA->D);
for(i = 0; i < cube.num_vars; i++) {
PLA->F = d1merge(PLA->F, i);
PLA->D = d1merge(PLA->D, i);
}
PLA->F = sf_contain(PLA->F);
PLA->D = sf_contain(PLA->D);
sf_free(PLA->R);
PLA->R = sf_new(0, cube.size);
symbolic_hack_labels(PLA, PLA->symbolic_output,
compress, cube.size, old_size, tot_size);
((compress) ? (free((char *) (compress)), (compress) = 0) : 0);
}
find_inputs(pset_family A, pPLA PLA, symbolic_list_t *list, int base, int value, pset_family *newF, pset_family *newD)
{
pset_family S, S1;
register pset last, p;
if (list == ((symbolic_list_t *) 0)) {
S = cv_intersect(A, PLA->F);
for( p=S->data, last= p+S->count*S->wsize; p< last; p+=S->wsize) {
(p[((( base + value) >> 5) + 1)] |= 1 << (( base + value) & (32-1)));
}
*newF = sf_append(*newF, S);
} else {
S = cof_output(PLA->R, cube.first_part[cube.output] + list->pos);
if (A != ((set_family_t *) 0)) {
S1 = cv_intersect(A, S);
sf_free(S);
S = S1;
}
find_inputs(S, PLA, list->next, base, value*2, newF, newD);
sf_free(S);
S = cof_output(PLA->F, cube.first_part[cube.output] + list->pos);
if (A != ((set_family_t *) 0)) {
S1 = cv_intersect(A, S);
sf_free(S);
S = S1;
}
find_inputs(S, PLA, list->next, base, value*2 + 1, newF, newD);
sf_free(S);
}
}

map_symbolic(pPLA PLA)
{
symbolic_t *p1;
symbolic_list_t *p2;
int var, base, num_vars, num_binary_vars, *new_part_size;
int new_size, size_added, num_deleted_vars, num_added_vars, newvar;
pset compress;
for(p1 = PLA->symbolic; p1 != ((symbolic_t *) 0); p1 = p1->next) {
for(p2=p1->symbolic_list; p2!=((symbolic_list_t *) 0); p2=p2->next) {
if (p2->variable < 0 || p2->variable >= cube.num_binary_vars) {
fatal(".symbolic requires binary variables");
}
}
}
size_added = 0;
num_added_vars = 0;
for(p1 = PLA->symbolic; p1 != ((symbolic_t *) 0); p1 = p1->next) {
size_added += 1 << p1->symbolic_list_length;
num_added_vars++;
}
compress = set_fill(((unsigned int *) malloc(sizeof(unsigned int) * ( ((PLA->F->sf_size + size_added) <= 32 ? 2 : (((((PLA->F->sf_size + size_added)-1) >> 5) + 1) + 1))))), PLA->F->sf_size + size_added);
for(p1 = PLA->symbolic; p1 != ((symbolic_t *) 0); p1 = p1->next) {
for(p2=p1->symbolic_list; p2!=((symbolic_list_t *) 0); p2=p2->next) {
(compress[((( p2->variable*2) >> 5) + 1)] &= ~ (1 << (( p2->variable*2) & (32-1))));
(compress[((( p2->variable*2+1) >> 5) + 1)] &= ~ (1 << (( p2->variable*2+1) & (32-1))));
}
}
num_deleted_vars = ((PLA->F->sf_size + size_added) - set_ord(compress))/2;
num_vars = cube.num_vars - num_deleted_vars + num_added_vars;
num_binary_vars = cube.num_binary_vars - num_deleted_vars;
new_size = cube.size - num_deleted_vars*2 + size_added;
new_part_size = ((int *) malloc(sizeof(int) * ( num_vars)));
new_part_size[num_vars-1] = cube.part_size[cube.num_vars-1];
for(var = cube.num_binary_vars; var < cube.num_vars-1; var++) {
new_part_size[var-num_deleted_vars] = cube.part_size[var];
}
base = cube.first_part[cube.output];
PLA->F = sf_addcol(PLA->F, base, size_added);
PLA->D = sf_addcol(PLA->D, base, size_added);
PLA->R = sf_addcol(PLA->R, base, size_added);
newvar = (cube.num_vars - 1) - num_deleted_vars;
for(p1 = PLA->symbolic; p1 != ((symbolic_t *) 0); p1 = p1->next) {
PLA->F = map_symbolic_cover(PLA->F, p1->symbolic_list, base);
PLA->D = map_symbolic_cover(PLA->D, p1->symbolic_list, base);
PLA->R = map_symbolic_cover(PLA->R, p1->symbolic_list, base);
base += 1 << p1->symbolic_list_length;
new_part_size[newvar++] = 1 << p1->symbolic_list_length;
}
PLA->F = sf_compress(PLA->F, compress);
PLA->D = sf_compress(PLA->D, compress);
PLA->R = sf_compress(PLA->R, compress);
symbolic_hack_labels(PLA, PLA->symbolic, compress,
new_size, cube.size, size_added);
setdown_cube();
((cube.part_size) ? (free((char *) (cube.part_size)), (cube.part_size) = 0) : 0);
cube.num_vars = num_vars;
cube.num_binary_vars = num_binary_vars;
cube.part_size = new_part_size;
cube_setup();
((compress) ? (free((char *) (compress)), (compress) = 0) : 0);
}
pset_family map_symbolic_cover(pset_family T, symbolic_list_t *list, int base)
{
pset last, p;
for( p=T->data, last= p+T->count*T->wsize; p< last; p+=T->wsize) {
form_bitvector(p, base, 0, list);
}
return T;
}
form_bitvector(pset p, int base, int value, symbolic_list_t *list)
{
if (list == ((symbolic_list_t *) 0)) {
(p[((( base + value) >> 5) + 1)] |= 1 << (( base + value) & (32-1)));
} else {
switch(((p[(((2* list->variable) >> 5) + 1)] >> ((2* list->variable) & (32-1))) & 3)) {
case 1:
form_bitvector(p, base, value*2, list->next);
break;
case 2:
form_bitvector(p, base, value*2+1, list->next);
break;
case 3:
form_bitvector(p, base, value*2, list->next);
form_bitvector(p, base, value*2+1, list->next);
break;
default:
fatal("bad cube in form_bitvector");
}
}
}
symbolic_hack_labels(pPLA PLA, symbolic_t *list, pset compress, int new_size, int old_size, int size_added)
{
int i, base;
char **oldlabel;
symbolic_t *p1;
symbolic_label_t *p3;
if ((oldlabel = PLA->label) == ((char * *) 0))
return;
PLA->label = ((char * *) malloc(sizeof(char *) * ( new_size)));
for(i = 0; i < new_size; i++) {
PLA->label[i] = ((char *) 0);
}
base = 0;
for(i = 0; i < cube.first_part[cube.output]; i++) {
if ((compress[((( i) >> 5) + 1)] & (1 << (( i) & (32-1))))) {
PLA->label[base++] = oldlabel[i];
} else {
if (oldlabel[i] != ((char *) 0)) {
((oldlabel[i]) ? (free((char *) (oldlabel[i])), (oldlabel[i]) = 0) : 0);
}
}
}
for(p1 = list; p1 != ((symbolic_t *) 0); p1 = p1->next) {
p3 = p1->symbolic_label;
for(i = 0; i < (1 << p1->symbolic_list_length); i++) {
if (p3 == ((symbolic_label_t *) 0)) {
PLA->label[base+i] = ((char *) malloc(sizeof(char) * ( 10)));
(void) sprintf(PLA->label[base+i], "X%d", i);
} else {
PLA->label[base+i] = p3->label;
p3 = p3->next;
}
}
base += 1 << p1->symbolic_list_length;
}
for(i = cube.first_part[cube.output]; i < old_size; i++) {
if ((compress[((( i + size_added) >> 5) + 1)] & (1 << (( i + size_added) & (32-1))))) {
PLA->label[base++] = oldlabel[i];
} else {
if (oldlabel[i] != ((char *) 0)) {
((oldlabel[i]) ? (free((char *) (oldlabel[i])), (oldlabel[i]) = 0) : 0);
}
}
}
((oldlabel) ? (free((char *) (oldlabel)), (oldlabel) = 0) : 0);
}

static pset_family fsm_simplify(pset_family F)
{
pset_family D, R;
D = sf_new(0, cube.size);
R = complement(cube1list(F));
F = espresso(F, D, R);
sf_free(D);
sf_free(R);
return F;
}
disassemble_fsm(pPLA PLA, int verbose_mode)
{
int nin, nstates, nout;
int before, after, present_state, next_state, i, j;
pset next_state_mask, present_state_mask, state_mask, p, p1, last;
pset_family go_nowhere, F, tF;
if (cube.num_vars - cube.num_binary_vars != 2) {
fprintf((&_iob[2]),
"use .symbolic and .symbolic-output to specify\n");
fprintf((&_iob[2]),
"the present state and next state field information\n");
fatal("disassemble_pla: need two multiple-valued variables\n");
}
nin = cube.num_binary_vars;
nstates = cube.part_size[cube.num_binary_vars];
nout = cube.part_size[cube.num_vars - 1];
if (nout < nstates) {
fprintf((&_iob[2]),
"use .symbolic and .symbolic-output to specify\n");
fprintf((&_iob[2]),
"the present state and next state field information\n");
fatal("disassemble_pla: # outputs < # states\n");
}
present_state = cube.first_part[cube.num_binary_vars];
present_state_mask = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
for(i = 0; i < nstates; i++) {
(present_state_mask[((( i + present_state) >> 5) + 1)] |= 1 << (( i + present_state) & (32-1)));
}
next_state = cube.first_part[cube.num_binary_vars+1];
next_state_mask = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
for(i = 0; i < nstates; i++) {
(next_state_mask[((( i + next_state) >> 5) + 1)] |= 1 << (( i + next_state) & (32-1)));
}
state_mask = set_or(set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size), next_state_mask, present_state_mask);
F = sf_new(10, cube.size);
for(i = 0; i < nstates; i++) {
tF = sf_new(10, cube.size);
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize) {
if (setp_implies(present_state_mask, p)) { if ((p[((( next_state + i) >> 5) + 1)] & (1 << (( next_state + i) & (32-1))))) {
tF = sf_addset(tF, p);
}
}
}
before = tF->count;
if (before > 0) {
tF = fsm_simplify(tF);
for( p=tF->data, last= p+tF->count*tF->wsize; p< last; p+=tF->wsize) {
(p[((( next_state + i) >> 5) + 1)] |= 1 << (( next_state + i) & (32-1)));
}
after = tF->count;
F = sf_append(F, tF);
if (verbose_mode) {
printf("# state EVERY to %d, before=%d after=%d\n",
i, before, after);
}
}
}
go_nowhere = sf_new(10, cube.size);
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize) {
if (setp_disjoint(p, next_state_mask)) { go_nowhere = sf_addset(go_nowhere, p);
}
}
before = go_nowhere->count;
go_nowhere = unravel_range(go_nowhere,
cube.num_binary_vars, cube.num_binary_vars);
after = go_nowhere->count;
F = sf_append(F, go_nowhere);
if (verbose_mode) {
printf("# state ANY to NOWHERE, before=%d after=%d\n", before, after);
}
for(i = 0; i < nstates; i++) {
for(j = 0; j < nstates; j++) {
tF = sf_new(10, cube.size);
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize) {
if (! setp_implies(present_state_mask, p)) {
if ((p[((( present_state + i) >> 5) + 1)] & (1 << (( present_state + i) & (32-1))))) {
if ((p[((( next_state + j) >> 5) + 1)] & (1 << (( next_state + j) & (32-1))))) {
p1 = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (p[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (p[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), p);
set_diff(p1, p1, state_mask);
(p1[((( present_state + i) >> 5) + 1)] |= 1 << (( present_state + i) & (32-1)));
(p1[((( next_state + j) >> 5) + 1)] |= 1 << (( next_state + j) & (32-1)));
tF = sf_addset(tF, p1);
((p1) ? (free((char *) (p1)), (p1) = 0) : 0);
}
}
}
}
before = tF->count;
if (before > 0) {
tF = fsm_simplify(tF);
for( p=tF->data, last= p+tF->count*tF->wsize; p< last; p+=tF->wsize) {
(p[((( next_state + j) >> 5) + 1)] |= 1 << (( next_state + j) & (32-1)));
}
after = tF->count;
F = sf_append(F, tF);
if (verbose_mode) {
printf("# state %d to %d, before=%d after=%d\n",
i, j, before, after);
}
}
}
}
((state_mask) ? (free((char *) (state_mask)), (state_mask) = 0) : 0);
((present_state_mask) ? (free((char *) (present_state_mask)), (present_state_mask) = 0) : 0);
((next_state_mask) ? (free((char *) (next_state_mask)), (next_state_mask) = 0) : 0);
sf_free(PLA->F);
PLA->F = F;
sf_free(PLA->D);
PLA->D = sf_new(0, cube.size);
setdown_cube();
((cube.part_size) ? (free((char *) (cube.part_size)), (cube.part_size) = 0) : 0);
cube.num_binary_vars = nin;
cube.num_vars = nin + 3;
cube.part_size = ((int *) malloc(sizeof(int) * ( cube.num_vars)));
cube.part_size[cube.num_binary_vars] = nstates;
cube.part_size[cube.num_binary_vars+1] = nstates;
cube.part_size[cube.num_binary_vars+2] = nout - nstates;
cube_setup();
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize) {
kiss_print_cube((&_iob[1]), PLA, p, "~1");
}
}
