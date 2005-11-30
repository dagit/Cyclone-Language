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
extern int feasibly_covered(pset_family BB, pset c, pset RAISE, pset new_lower);
extern int most_frequent(pset_family CC, pset FREESET);
extern pset_family all_primes(pset_family F, pset_family R);
extern pset_family expand(pset_family F, pset_family R, int nonsparse);
extern pset_family find_all_primes(pset_family BB, register pset RAISE, register pset FREESET);
extern void elim_lowering(pset_family BB, pset_family CC, pset RAISE, pset FREESET);
extern void essen_parts(pset_family BB, pset_family CC, pset RAISE, pset FREESET);
extern void essen_raising(register pset_family BB, pset RAISE, pset FREESET);
extern void expand1(pset_family BB, pset_family CC, pset RAISE, pset FREESET, pset OVEREXPANDED_CUBE, pset SUPER_CUBE, pset INIT_LOWER, int *num_covered, pset c);
extern void mincov(pset_family BB, pset RAISE, pset FREESET);
extern void select_feasible(pset_family BB, pset_family CC, pset RAISE, pset FREESET, pset SUPER_CUBE, int *num_covered);
extern void setup_BB_CC(register pset_family BB, register pset_family CC);
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

pset_family expand(pset_family F, pset_family R, int nonsparse)
{
register pset last, p;
pset RAISE, FREESET, INIT_LOWER, SUPER_CUBE, OVEREXPANDED_CUBE;
int var, num_covered;
int change;
if (use_random_order)
F = random_order(F);
else
F = mini_sort(F, ascend);
RAISE = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
FREESET = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
INIT_LOWER = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
SUPER_CUBE = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
OVEREXPANDED_CUBE = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
if (nonsparse)
for(var = 0; var < cube.num_vars; var++)
if (cube.sparse[var])
(void) set_or(INIT_LOWER, INIT_LOWER, cube.var_mask[var]);
for( p=F->data, last= p+F->count*F->wsize; p< last; p+=F->wsize) {
(p[0] &= ~ ( 0x0800));
(p[0] &= ~ ( 0x4000));
}
for( p=F->data, last= p+F->count*F->wsize; p< last; p+=F->wsize) {
if (! (p[0] & ( 0x8000)) && ! (p[0] & ( 0x0800))) {
expand1(R, F, RAISE, FREESET, OVEREXPANDED_CUBE, SUPER_CUBE,
INIT_LOWER, &num_covered, p);
if (debug & 0x0004)
printf("EXPAND: %s (covered %d)\n", pc1(p), num_covered);
(void) set_copy(p, RAISE);
(p[0] |= ( 0x8000));
(p[0] &= ~ ( 0x0800));	if (num_covered == 0 && ! setp_equal(p, OVEREXPANDED_CUBE)) {
(p[0] |= ( 0x4000));
}
}
}
F->active_count = 0;
change = 0;
for( p=F->data, last= p+F->count*F->wsize; p< last; p+=F->wsize) {
if ((p[0] & ( 0x0800))) {
(p[0] &= ~ ( 0x2000));
change = 1;
} else {
(p[0] |= ( 0x2000));
F->active_count++;
}
}
if (change)
F = sf_inactive(F);
((RAISE) ? (free((char *) (RAISE)), (RAISE) = 0) : 0);
((FREESET) ? (free((char *) (FREESET)), (FREESET) = 0) : 0);
((INIT_LOWER) ? (free((char *) (INIT_LOWER)), (INIT_LOWER) = 0) : 0);
((SUPER_CUBE) ? (free((char *) (SUPER_CUBE)), (SUPER_CUBE) = 0) : 0);
((OVEREXPANDED_CUBE) ? (free((char *) (OVEREXPANDED_CUBE)), (OVEREXPANDED_CUBE) = 0) : 0);
return F;
}

void expand1(pset_family BB, pset_family CC, pset RAISE, pset FREESET, pset OVEREXPANDED_CUBE, pset SUPER_CUBE, pset INIT_LOWER, int *num_covered, pset c)
{
int bestindex;
if (debug & 0x0008)
printf("\nEXPAND1:    \t%s\n", pc1(c));
(c[0] |= ( 0x8000));	setup_BB_CC(BB, CC);
*num_covered = 0;
(void) set_copy(SUPER_CUBE, c);
(void) set_copy(RAISE, c);
(void) set_diff(FREESET, cube.fullset, RAISE);
if (! setp_empty(INIT_LOWER)) {
(void) set_diff(FREESET, FREESET, INIT_LOWER);
elim_lowering(BB, CC, RAISE, FREESET);
}
essen_parts(BB, CC, RAISE, FREESET);
(void) set_or(OVEREXPANDED_CUBE, RAISE, FREESET);
if (CC->active_count > 0) {
select_feasible(BB, CC, RAISE, FREESET, SUPER_CUBE, num_covered);
}
while (CC->active_count > 0) {
bestindex = most_frequent(CC, FREESET);
(RAISE[((( bestindex) >> 5) + 1)] |= 1 << (( bestindex) & (32-1)));
(FREESET[((( bestindex) >> 5) + 1)] &= ~ (1 << (( bestindex) & (32-1))));
essen_parts(BB, CC, RAISE, FREESET);
}
while (BB->active_count > 0) {
mincov(BB, RAISE, FREESET);
}
(void) set_or(RAISE, RAISE, FREESET);
}

void essen_parts(pset_family BB, pset_family CC, pset RAISE, pset FREESET)
{
register pset p, r = RAISE;
pset lastp, xlower = cube.temp[0];
int dist;
(void) set_copy(xlower, cube.emptyset);
for( p=BB->data, lastp= p+BB->count*BB->wsize; p< lastp; p+=BB->wsize) if (( p[0] & ( 0x2000))) {
{register int w,last;register unsigned int x;dist=0;if((last=cube.inword)!=-1)
{x=p[last]&r[last];if(x=~(x|x>>1)&cube.inmask)if((dist=(bit_count[x & 255] + bit_count[(x >> 8) & 255] + bit_count[(x >> 16) & 255] + bit_count[(x >> 24) & 255]))>1)goto
exit_if;for(w=1;w<last;w++){x=p[w]&r[w];if(x=~(x|x>>1)&0x55555555)if(dist==1||(
dist+=(bit_count[x & 255] + bit_count[(x >> 8) & 255] + bit_count[(x >> 16) & 255] + bit_count[(x >> 24) & 255]))>1)goto exit_if;}}}{register int w,var,last;register pset
mask;for(var=cube.num_binary_vars;var<cube.num_vars;var++){mask=cube.var_mask[
var];last=cube.last_word[var];for(w=cube.first_word[var];w<=last;w++)if(p[w]&r[
w]&mask[w])goto nextvar;if(++dist>1)goto exit_if;nextvar:;}}
if (dist == 0) {
fatal("ON-set and OFF-set are not orthogonal");
} else {
(void) force_lower(xlower, p, r);
BB->active_count--;
(p[0] &= ~ ( 0x2000));
}
exit_if: ;
}
if (! setp_empty(xlower)) {
(void) set_diff(FREESET, FREESET, xlower);
elim_lowering(BB, CC, RAISE, FREESET);
}
if (debug & 0x0008)
printf("ESSEN_PARTS:\tRAISE=%s FREESET=%s\n", pc1(RAISE), pc2(FREESET));
}

void essen_raising(register pset_family BB, pset RAISE, pset FREESET)
{
register pset last, p, xraise = cube.temp[0];
(void) set_copy(xraise, cube.emptyset);
for( p=BB->data, last= p+BB->count*BB->wsize; p< last; p+=BB->wsize) if (( p[0] & ( 0x2000)))
{register int i_=( xraise[0] & 0x03ff); (xraise[0] &= ~0x03ff, xraise[0] |= (i_)); do xraise[i_] = xraise[i_] | p[i_]; while (--i_>0);};
(void) set_diff(xraise, FREESET, xraise);
(void) set_or(RAISE, RAISE, xraise); (void) set_diff(FREESET, FREESET, xraise); if (debug & 0x0008)
printf("ESSEN_RAISING:\tRAISE=%s FREESET=%s\n",
pc1(RAISE), pc2(FREESET));
}

void elim_lowering(pset_family BB, pset_family CC, pset RAISE, pset FREESET)
{
register pset p, r = set_or(cube.temp[0], RAISE, FREESET);
pset last;
for( p=BB->data, last= p+BB->count*BB->wsize; p< last; p+=BB->wsize) if (( p[0] & ( 0x2000))) {
{register int w,lastw;register unsigned int x;if((lastw=cube.inword)!=-1){x=p[
lastw]&r[lastw];if(~(x|x>>1)&cube.inmask)goto false;for(w=1;w<lastw;w++){x=p[w]
&r[w];if(~(x|x>>1)&0x55555555)goto false;}}}{register int w,var,lastw;register
pset mask;for(var=cube.num_binary_vars;var<cube.num_vars;var++){mask=cube.
var_mask[var];lastw=cube.last_word[var];for(w=cube.first_word[var];w<=lastw;w++)
if(p[w]&r[w]&mask[w])goto nextvar;goto false;nextvar:;}}continue;false:
BB->active_count--, (p[0] &= ~ ( 0x2000));
}
if (CC != (pset_family) 0) {
for( p=CC->data, last= p+CC->count*CC->wsize; p< last; p+=CC->wsize) if (( p[0] & ( 0x2000))) {
{register int i_=(p[0] & 0x03ff); do if (p[i_]&~ r[i_]) break; while (--i_>0); if (i_ != 0) goto false1;};
continue;
false1:
CC->active_count--, (p[0] &= ~ ( 0x2000));
}
}
}

int most_frequent(pset_family CC, pset FREESET)
{
register int i, best_part, best_count, *count;
register pset p, last;
count = ((int *) malloc(sizeof(int) * ( cube.size)));
for(i = 0; i < cube.size; i++)
count[i] = 0;
if (CC != (pset_family) 0)
for( p=CC->data, last= p+CC->count*CC->wsize; p< last; p+=CC->wsize) if (( p[0] & ( 0x2000)))
set_adjcnt(p, count, 1);
best_count = best_part = -1;
for(i = 0; i < cube.size; i++)
if ((FREESET[(((i) >> 5) + 1)] & (1 << ((i) & (32-1)))) && count[i] > best_count) {
best_part = i;
best_count = count[i];
}
((count) ? (free((char *) (count)), (count) = 0) : 0);
if (debug & 0x0008)
printf("MOST_FREQUENT:\tbest=%d FREESET=%s\n", best_part, pc2(FREESET));
return best_part;
}

void setup_BB_CC(register pset_family BB, register pset_family CC)
{
register pset p, last;
BB->active_count = BB->count;
for( p=BB->data, last= p+BB->count*BB->wsize; p< last; p+=BB->wsize)
(p[0] |= ( 0x2000));
if (CC != (pset_family) 0) {
CC->active_count = CC->count;
for( p=CC->data, last= p+CC->count*CC->wsize; p< last; p+=CC->wsize)
if ((p[0] & ( 0x0800)) || (p[0] & ( 0x8000)))
CC->active_count--, (p[0] &= ~ ( 0x2000));
else
(p[0] |= ( 0x2000));
}
}

void select_feasible(pset_family BB, pset_family CC, pset RAISE, pset FREESET, pset SUPER_CUBE, int *num_covered)
{
register pset p, last, bestfeas, *feas;
register int i, j;
pset *feas_new_lower;
int bestcount, bestsize, count, size, numfeas, lastfeas;
pset_family new_lower;
feas = ((pset *) malloc(sizeof(pset) * ( CC->active_count)));
numfeas = 0;
for( p=CC->data, last= p+CC->count*CC->wsize; p< last; p+=CC->wsize) if (( p[0] & ( 0x2000)))
feas[numfeas++] = p;
feas_new_lower = ((pset *) malloc(sizeof(pset) * ( CC->active_count)));
new_lower = sf_new(numfeas, cube.size);
for(i = 0; i < numfeas; i++)
feas_new_lower[i] = ((new_lower)->data + (new_lower)->wsize * ( i));
loop:
essen_raising(BB, RAISE, FREESET);
lastfeas = numfeas;
numfeas = 0;
for(i = 0; i < lastfeas; i++) {
p = feas[i];
if ((p[0] & ( 0x2000))) {
if (setp_implies(p, RAISE)) {
(*num_covered) += 1;
(void) set_or(SUPER_CUBE, SUPER_CUBE, p);
CC->active_count--;
(p[0] &= ~ ( 0x2000));
(p[0] |= ( 0x0800));
} else if (feasibly_covered(BB,p,RAISE,feas_new_lower[numfeas])) {
feas[numfeas] = p;	numfeas++;
}
}
}
if (debug & 0x0008)
printf("SELECT_FEASIBLE: started with %d pfcc, ended with %d fcc\n",
lastfeas, numfeas);
if (numfeas == 0) {
((feas) ? (free((char *) (feas)), (feas) = 0) : 0);
((feas_new_lower) ? (free((char *) (feas_new_lower)), (feas_new_lower) = 0) : 0);
sf_free(new_lower);
return;
}
bestcount = 0;
bestsize = 9999;
for(i = 0; i < numfeas; i++) {
size = set_dist(feas[i], FREESET);	count = 0;	for(j = 0; j < numfeas; j++)
if (setp_disjoint(feas_new_lower[i], feas[j]))
count++;
if (count > bestcount) {
bestcount = count;
bestfeas = feas[i];
bestsize = size;
} else if (count == bestcount && size < bestsize) {
bestfeas = feas[i];
bestsize = size;
}
}
(void) set_or(RAISE, RAISE, bestfeas);
(void) set_diff(FREESET, FREESET, RAISE);
if (debug & 0x0008)
printf("FEASIBLE:  \tRAISE=%s FREESET=%s\n", pc1(RAISE), pc2(FREESET));
essen_parts(BB, CC, RAISE, FREESET);
goto loop;
}

int feasibly_covered(pset_family BB, pset c, pset RAISE, pset new_lower)
{
register pset p, r = set_or(cube.temp[0], RAISE, c);
int dist;
pset lastp;
set_copy(new_lower, cube.emptyset);
for( p=BB->data, lastp= p+BB->count*BB->wsize; p< lastp; p+=BB->wsize) if (( p[0] & ( 0x2000))) {
{register int w,last;register unsigned int x;dist=0;if((last=cube.inword)!=-1)
{x=p[last]&r[last];if(x=~(x|x>>1)&cube.inmask)if((dist=(bit_count[x & 255] + bit_count[(x >> 8) & 255] + bit_count[(x >> 16) & 255] + bit_count[(x >> 24) & 255]))>1)goto
exit_if;for(w=1;w<last;w++){x=p[w]&r[w];if(x=~(x|x>>1)&0x55555555)if(dist==1||(
dist+=(bit_count[x & 255] + bit_count[(x >> 8) & 255] + bit_count[(x >> 16) & 255] + bit_count[(x >> 24) & 255]))>1)goto exit_if;}}}{register int w,var,last;register pset
mask;for(var=cube.num_binary_vars;var<cube.num_vars;var++){mask=cube.var_mask[
var];last=cube.last_word[var];for(w=cube.first_word[var];w<=last;w++)if(p[w]&r[
w]&mask[w])goto nextvar;if(++dist>1)goto exit_if;nextvar:;}}
if (dist == 0)
return 0;
else
(void) force_lower(new_lower, p, r);
exit_if: ;
}
return 1;
}

void mincov(pset_family BB, pset RAISE, pset FREESET)
{
int expansion, nset, var, dist;
pset_family B;
register pset xraise=cube.temp[0], xlower, p, last, plower;
B = sf_new(BB->active_count, cube.size);
for( p=BB->data, last= p+BB->count*BB->wsize; p< last; p+=BB->wsize) if (( p[0] & ( 0x2000))) {
plower = set_copy(((B)->data + (B)->wsize * ( B->count++)), cube.emptyset);
(void) force_lower(plower, p, RAISE);
}
nset = 0;
for( p=B->data, last= p+B->count*B->wsize; p< last; p+=B->wsize) {
expansion = 1;
for(var = cube.num_binary_vars; var < cube.num_vars; var++) {
if ((dist=set_dist(p, cube.var_mask[var])) > 1) {
expansion *= dist;
if (expansion > 500) goto heuristic_mincov;
}
}
nset += expansion;
if (nset > 500) goto heuristic_mincov;
}
B = unravel(B, cube.num_binary_vars);
xlower = do_sm_minimum_cover(B);
(void) set_or(RAISE, RAISE, set_diff(xraise, FREESET, xlower));
(void) set_copy(FREESET, cube.emptyset);	BB->active_count = 0;	if (debug & 0x0008) {
printf("MINCOV:    \tRAISE=%s FREESET=%s\n", pc1(RAISE), pc2(FREESET));
}
sf_free(B);
((xlower) ? (free((char *) (xlower)), (xlower) = 0) : 0);
return;
heuristic_mincov:
sf_free(B);
(RAISE[((( most_frequent( (pset_family) 0, FREESET)) >> 5) + 1)] |= 1 << (( most_frequent( (pset_family) 0, FREESET)) & (32-1)));
(void) set_diff(FREESET, FREESET, RAISE);
essen_parts(BB, (pset_family) 0, RAISE, FREESET);
return;
}

pset_family find_all_primes(pset_family BB, register pset RAISE, register pset FREESET)
{
register pset last, p, plower;
pset_family B, B1;
if (BB->active_count == 0) {
B1 = sf_new(1, cube.size);
p = ((B1)->data + (B1)->wsize * ( B1->count++));
(void) set_copy(p, RAISE);
(p[0] |= ( 0x8000));
} else {
B = sf_new(BB->active_count, cube.size);
for( p=BB->data, last= p+BB->count*BB->wsize; p< last; p+=BB->wsize) if (( p[0] & ( 0x2000))) {
plower = set_copy(((B)->data + (B)->wsize * ( B->count++)), cube.emptyset);
(void) force_lower(plower, p, RAISE);
}
B = sf_rev_contain(unravel(B, cube.num_binary_vars));
B1 = exact_minimum_cover(B);
for( p=B1->data, last= p+B1->count*B1->wsize; p< last; p+=B1->wsize) {
{register int i_=( FREESET[0] & 0x03ff); (p[0] &= ~0x03ff, p[0] |= (i_)); do p[i_] = FREESET[i_] & ~ p[i_]; while (--i_>0);};
{register int i_=( p[0] & 0x03ff); (p[0] &= ~0x03ff, p[0] |= (i_)); do p[i_] = p[i_] | RAISE[i_]; while (--i_>0);};
(p[0] |= ( 0x8000));
}
sf_free(B);
}
return B1;
}
pset_family all_primes(pset_family F, pset_family R)
{
register pset last, p, RAISE, FREESET;
pset_family Fall_primes, B1;
FREESET = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
RAISE = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
Fall_primes = sf_new(F->count, cube.size);
for( p=F->data, last= p+F->count*F->wsize; p< last; p+=F->wsize) {
if ((p[0] & ( 0x8000))) {
Fall_primes = sf_addset(Fall_primes, p);
} else {
(void) set_copy(RAISE, p);
(void) set_diff(FREESET, cube.fullset, RAISE);
setup_BB_CC(R, (pset_family) 0);
essen_parts(R, (pset_family) 0, RAISE, FREESET);
B1 = find_all_primes(R, RAISE, FREESET);
Fall_primes = sf_append(Fall_primes, B1);
}
}
((RAISE) ? (free((char *) (RAISE)), (RAISE) = 0) : 0);
((FREESET) ? (free((char *) (FREESET)), (FREESET) = 0) : 0);
return Fall_primes;
}
