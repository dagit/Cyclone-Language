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
enum keys {
KEY_ESPRESSO, KEY_PLA_verify, KEY_check, KEY_contain, KEY_d1merge,
KEY_disjoint, KEY_dsharp, KEY_echo, KEY_essen, KEY_exact, KEY_expand,
KEY_gasp, KEY_intersect, KEY_irred, KEY_lexsort, KEY_make_sparse,
KEY_map, KEY_mapdc, KEY_minterms, KEY_opo, KEY_opoall,
KEY_pair, KEY_pairall, KEY_primes, KEY_qm, KEY_reduce, KEY_sharp,
KEY_simplify, KEY_so, KEY_so_both, KEY_stats, KEY_super_gasp, KEY_taut,
KEY_test, KEY_equiv, KEY_union, KEY_verify, KEY_MANY_ESPRESSO,
KEY_separate, KEY_xor, KEY_d1merge_in, KEY_fsm,
KEY_unknown
};
struct {
char *name;
enum keys key;
int num_plas;
int needs_offset;
int needs_dcset;
} option_table [] = {
"ESPRESSO", KEY_ESPRESSO, 1, 1, 1, "many", KEY_MANY_ESPRESSO, 1, 1, 1,
"exact", KEY_exact, 1, 1, 1,
"qm", KEY_qm, 1, 1, 1,
"single_output", KEY_so, 1, 1, 1,
"so", KEY_so, 1, 1, 1,
"so_both", KEY_so_both, 1, 1, 1,
"simplify", KEY_simplify, 1, 0, 0,
"echo", KEY_echo, 1, 0, 0,
"opo", KEY_opo, 1, 1, 1,
"opoall", KEY_opoall, 1, 1, 1,
"pair", KEY_pair, 1, 1, 1,
"pairall", KEY_pairall, 1, 1, 1,
"check", KEY_check, 1, 1, 1,
"stats", KEY_stats, 1, 0, 0,
"verify", KEY_verify, 2, 0, 1,
"PLAverify", KEY_PLA_verify, 2, 0, 1,
"equiv", KEY_equiv, 1, 1, 1,
"map", KEY_map, 1, 0, 0,
"mapdc", KEY_mapdc, 1, 0, 0,
"fsm", KEY_fsm, 1, 0, 1,
"contain", KEY_contain, 1, 0, 0,
"d1merge", KEY_d1merge, 1, 0, 0,
"d1merge_in", KEY_d1merge_in, 1, 0, 0,
"disjoint", KEY_disjoint, 1, 1, 0,
"dsharp", KEY_dsharp, 2, 0, 0,
"intersect", KEY_intersect, 2, 0, 0,
"minterms", KEY_minterms, 1, 0, 0,
"primes", KEY_primes, 1, 0, 1,
"separate", KEY_separate, 1, 1, 1,
"sharp", KEY_sharp, 2, 0, 0,
"union", KEY_union, 2, 0, 0,
"xor", KEY_xor, 2, 1, 1,
"essen", KEY_essen, 1, 0, 1,
"expand", KEY_expand, 1, 1, 0,
"gasp", KEY_gasp, 1, 1, 1,
"irred", KEY_irred, 1, 0, 1,
"make_sparse", KEY_make_sparse, 1, 1, 1,
"reduce", KEY_reduce, 1, 0, 1,
"taut", KEY_taut, 1, 0, 0,
"super_gasp", KEY_super_gasp, 1, 1, 1,
"lexsort", KEY_lexsort, 1, 0, 0,
"test", KEY_test, 1, 1, 1,
0, KEY_unknown, 0, 0, 0 };
struct {
char *name;
int value;
} debug_table[] = {
"", 0x0004 + 0x0002 + 0x0020 + 0x0040 + 0x0100 + 0x0010 + 0x2000 + 0x0800,
"compl", 0x0001, "essen", 0x0002,
"expand", 0x0004, "expand1", 0x0008|0x0004,
"irred", 0x0020, "irred1", 0x4000|0x0020,
"reduce", 0x0040, "reduce1", 0x0080|0x0040,
"mincov", 0x0800, "mincov1", 0x1000|0x0800,
"sparse", 0x0100, "sharp", 0x2000,
"taut", 0x0200, "gasp", 0x0010,
"exact", 0x0400,
0,
};
struct {
char *name;
int *variable;
int value;
} esp_opt_table[] = {
"eat", &echo_comments, 0,
"eatdots", &echo_unknown_commands, 0,
"fast", &single_expand, 1,
"kiss", &kiss, 1,
"ness", &remove_essential, 0,
"nirr", &force_irredundant, 0,
"nunwrap", &unwrap_onset, 0,
"onset", &recompute_onset, 1,
"pos", &pos, 1,
"random", &use_random_order, 1,
"strong", &use_super_gasp, 1,
0,
};
static struct _iobuf *last_fp;
static int input_type = (1 | 2);
main(int argc, char **argv)
{
int i, j, first, last, strategy, out_type, option;
pPLA PLA, PLA1;
pset_family F, Fold, Dold;
pset last1, p;
cost_t cost;
int error, exact_cover;
long start;
extern char *util_optarg;
extern int util_optind;
start = util_cpu_time();
error = 0;
init_runtime();
option = 0;	out_type = 1;	debug = 0;	verbose_debug = 0;	print_solution = 1;	summary = 0;	trace = 0;	strategy = 0;	first = -1;	last = -1;
remove_essential = 1;	force_irredundant = 1;
unwrap_onset = 1;
single_expand = 0;
pos = 0;
recompute_onset = 0;
use_super_gasp = 0;
use_random_order = 0;
kiss = 0;
echo_comments = 1;
echo_unknown_commands = 1;
exact_cover = 0;	backward_compatibility_hack(&argc, argv, &option, &out_type);
while ((i = util_getopt(argc, argv, "D:S:de:o:r:stv:x")) != (-1)) {
switch(i) {
case 'D':	for(j = 0; option_table[j].name != 0; j++) {
if (strcmp(util_optarg, option_table[j].name) == 0) {
option = j;
break;
}
}
if (option_table[j].name == 0) {
fprintf((&_iob[2]), "%s: bad subcommand \"%s\"\n",
argv[0], util_optarg);
exit(1);
}
break;
case 'o':	for(j = 0; pla_types[j].key != 0; j++) {
if (strcmp(util_optarg, pla_types[j].key+1) == 0) {
out_type = pla_types[j].value;
break;
}
}
if (pla_types[j].key == 0) {
fprintf((&_iob[2]), "%s: bad output type \"%s\"\n",
argv[0], util_optarg);
exit(1);
}
break;
case 'e':	for(j = 0; esp_opt_table[j].name != 0; j++) {
if (strcmp(util_optarg, esp_opt_table[j].name) == 0) {
*(esp_opt_table[j].variable) = esp_opt_table[j].value;
break;
}
}
if (esp_opt_table[j].name == 0) {
fprintf((&_iob[2]), "%s: bad espresso option \"%s\"\n",
argv[0], util_optarg);
exit(1);
}
break;
case 'd':	debug = debug_table[0].value;
trace = 1;
summary = 1;
break;
case 'v':	verbose_debug = 1;
for(j = 0; debug_table[j].name != 0; j++) {
if (strcmp(util_optarg, debug_table[j].name) == 0) {
debug |= debug_table[j].value;
break;
}
}
if (debug_table[j].name == 0) {
fprintf((&_iob[2]), "%s: bad debug type \"%s\"\n",
argv[0], util_optarg);
exit(1);
}
break;
case 't':
trace = 1;
break;
case 's':
summary = 1;
break;
case 'x':	print_solution = 0;
break;
case 'S':	strategy = atoi(util_optarg);
break;
case 'r':	if (sscanf(util_optarg, "%d-%d", &first, &last) < 2) {
fprintf((&_iob[2]), "%s: bad output range \"%s\"\n",
argv[0], util_optarg);
exit(1);
}
break;
default:
usage();
exit(1);
}
}
if (summary || trace) {
printf("# espresso");
for(i = 1; i < argc; i++) {
printf(" %s", argv[i]);
}
printf("\n");
printf("# %s\n", "UC Berkeley, Espresso Version #2.3, Release date 01/31/88");
}
PLA = PLA1 = ((PLA_t *) 0);
switch(option_table[option].num_plas) {
case 2:
if (util_optind+2 < argc) fatal("trailing arguments on command line");
getPLA(util_optind++, argc, argv, option, &PLA, out_type);
getPLA(util_optind++, argc, argv, option, &PLA1, out_type);
break;
case 1:
if (util_optind+1 < argc) fatal("trailing arguments on command line");
getPLA(util_optind++, argc, argv, option, &PLA, out_type);
break;
}
if (util_optind < argc) fatal("trailing arguments on command line");
if (summary || trace) {
if (PLA != ((PLA_t *) 0)) PLA_summary(PLA);
if (PLA1 != ((PLA_t *) 0)) PLA_summary(PLA1);
}

switch(option_table[option].key) {
case KEY_ESPRESSO:
Fold = sf_save(PLA->F);
PLA->F = espresso(PLA->F, PLA->D, PLA->R);
{long t=util_cpu_time();error=verify(PLA->F,Fold,PLA->D);totals(t, 14, PLA->F,&( cost));};
if (error) {
print_solution = 0;
PLA->F = Fold;
(void) check_consistency(PLA);
} else {
sf_free(Fold);
}
break;
case KEY_MANY_ESPRESSO: {
int pla_type;
do {
{long t=util_cpu_time();PLA->F=espresso(PLA->F,PLA->D,PLA->R);if(trace)print_trace(PLA->F,"ESPRESSO   ",util_cpu_time()-t);};
if (print_solution) {
fprint_pla((&_iob[1]), PLA, out_type);
(void) fflush((&_iob[1]));
}
pla_type = PLA->pla_type;
free_PLA(PLA);
setdown_cube();
((cube.part_size) ? (free((char *) (cube.part_size)), (cube.part_size) = 0) : 0);
} while (read_pla(last_fp, 1, 1, pla_type, &PLA) != (-1));
exit(0);
}
case KEY_simplify:
{long t=util_cpu_time();PLA->F = simplify(cube1list(PLA->F));if(trace)print_trace( PLA->F, "SIMPLIFY  ",util_cpu_time()-t);};
break;
case KEY_so:	if (strategy < 0 || strategy > 1) {
strategy = 0;
}
so_espresso(PLA, strategy);
break;
case KEY_so_both:	if (strategy < 0 || strategy > 1) {
strategy = 0;
}
so_both_espresso(PLA, strategy);
break;
case KEY_expand: {long t=util_cpu_time();PLA->F=expand(PLA->F,PLA->R,0);totals(t,4, PLA->F,&( cost));};
break;
case KEY_irred: {long t=util_cpu_time();PLA->F = irredundant(PLA->F, PLA->D);totals(t, 5, PLA->F,&( cost));};
break;
case KEY_reduce: {long t=util_cpu_time();PLA->F = reduce(PLA->F, PLA->D);totals(t, 6, PLA->F,&( cost));};
break;
case KEY_essen: for( p=PLA->F->data, last1= p+PLA->F->count*PLA->F->wsize; p< last1; p+=PLA->F->wsize) {
(p[0] |= ( 0x0400));
(p[0] &= ~ ( 0x4000));
}
{long t=util_cpu_time();F = essential(&(PLA->F), &(PLA->D));totals(t, 3, PLA->F,&( cost));};
sf_free(F);
break;
case KEY_super_gasp:
PLA->F = super_gasp(PLA->F, PLA->D, PLA->R, &cost);
break;
case KEY_gasp:
PLA->F = last_gasp(PLA->F, PLA->D, PLA->R, &cost);
break;
case KEY_make_sparse: PLA->F = make_sparse(PLA->F, PLA->D, PLA->R);
break;
case KEY_exact:
exact_cover = 1;
case KEY_qm:
Fold = sf_save(PLA->F);
PLA->F = minimize_exact(PLA->F, PLA->D, PLA->R, exact_cover);
{long t=util_cpu_time();error=verify(PLA->F,Fold,PLA->D);totals(t, 14, PLA->F,&( cost));};
if (error) {
print_solution = 0;
PLA->F = Fold;
(void) check_consistency(PLA);
}
sf_free(Fold);
break;
case KEY_primes: {long t=util_cpu_time();PLA->F = primes_consensus(cube2list(PLA->F, PLA->D));if(trace)print_trace( PLA->F, "PRIMES     ",util_cpu_time()-t);};
break;
case KEY_map: map(PLA->F);
print_solution = 0;
break;
case KEY_opo: phase_assignment(PLA, strategy);
break;
case KEY_opoall:	if (first < 0 || first >= cube.part_size[cube.output]) {
first = 0;
}
if (last < 0 || last >= cube.part_size[cube.output]) {
last = cube.part_size[cube.output] - 1;
}
opoall(PLA, first, last, strategy);
break;
case KEY_pair: find_optimal_pairing(PLA, strategy);
break;
case KEY_pairall:	pair_all(PLA, strategy);
break;
case KEY_echo:	break;
case KEY_taut:	printf("ON-set is%sa tautology\n",
tautology(cube1list(PLA->F)) ? " " : " not ");
print_solution = 0;
break;
case KEY_contain:	PLA->F = sf_contain(PLA->F);
break;
case KEY_intersect:	PLA->F = cv_intersect(PLA->F, PLA1->F);
break;
case KEY_union:	PLA->F = sf_union(PLA->F, PLA1->F);
break;
case KEY_disjoint:	PLA->F = make_disjoint(PLA->F);
break;
case KEY_dsharp:	PLA->F = cv_dsharp(PLA->F, PLA1->F);
break;
case KEY_sharp:	PLA->F = cv_sharp(PLA->F, PLA1->F);
break;
case KEY_lexsort:	PLA->F = lex_sort(PLA->F);
break;
case KEY_stats:	if (! summary) PLA_summary(PLA);
print_solution = 0;
break;
case KEY_minterms:	if (first < 0 || first >= cube.num_vars) {
first = 0;
}
if (last < 0 || last >= cube.num_vars) {
last = cube.num_vars - 1;
}
PLA->F = sf_dupl(unravel_range(PLA->F, first, last));
break;
case KEY_d1merge:	if (first < 0 || first >= cube.num_vars) {
first = 0;
}
if (last < 0 || last >= cube.num_vars) {
last = cube.num_vars - 1;
}
for(i = first; i <= last; i++) {
PLA->F = d1merge(PLA->F, i);
}
break;
case KEY_d1merge_in:	for(i = 0; i < cube.num_binary_vars; i++) {
PLA->F = d1merge(PLA->F, i);
}
break;
case KEY_PLA_verify:	{long t=util_cpu_time();error = PLA_verify(PLA, PLA1);totals(t, 14, PLA->F,&( cost));};
if (error) {
printf("PLA comparison failed; the PLA's are not equivalent\n");
exit(1);
} else {
printf("PLA's compared equal\n");
exit(0);
}
break;	case KEY_verify:	Fold = PLA->F;	Dold = PLA->D;	F = PLA1->F;
{long t=util_cpu_time();error=verify(F, Fold, Dold);totals(t, 14, PLA->F,&( cost));};
if (error) {
printf("PLA comparison failed; the PLA's are not equivalent\n");
exit(1);
} else {
printf("PLA's compared equal\n");
exit(0);
}	break;	case KEY_check:	(void) check_consistency(PLA);
print_solution = 0;
break;
case KEY_mapdc:	map_dcset(PLA);
out_type = (1 | 2);
break;
case KEY_equiv:
find_equiv_outputs(PLA);
print_solution = 0;
break;
case KEY_separate:	PLA->F = complement(cube2list(PLA->D, PLA->R));
break;
case KEY_xor: {
pset_family T1 = cv_intersect(PLA->F, PLA1->R);
pset_family T2 = cv_intersect(PLA1->F, PLA->R);
sf_free(PLA->F);
PLA->F = sf_contain(sf_join(T1, T2));
sf_free(T1);
sf_free(T2);
break;
}
case KEY_fsm: {
disassemble_fsm(PLA, summary);
print_solution = 0;
break;
}
case KEY_test: {
pset_family T, E;
T = sf_join(PLA->D, PLA->R);
E = sf_new(10, cube.size);
sf_free(PLA->F);
{long t=util_cpu_time();PLA->F = complement(cube1list(T));totals(t, 1, PLA->F,&( cost));};
{long t=util_cpu_time();PLA->F = expand(PLA->F, T, 0);totals(t, 4, PLA->F,&( cost));};
{long t=util_cpu_time();PLA->F = irredundant(PLA->F, E);totals(t, 5, PLA->F,&( cost));};
sf_free(T);
T = sf_join(PLA->F, PLA->R);
{long t=util_cpu_time();PLA->D = expand(PLA->D, T, 0);totals(t, 4, PLA->D,&( cost));};
{long t=util_cpu_time();PLA->D = irredundant(PLA->D, E);totals(t, 5, PLA->D,&( cost));};
sf_free(T);
sf_free(E);
break;
}
}
if (trace) {
runtime();
}
if (summary || trace) {
print_trace(PLA->F, option_table[option].name, util_cpu_time()-start);
}
if (print_solution) {
{long t=util_cpu_time();fprint_pla((&_iob[1]), PLA, out_type);totals(t, 15, PLA->F,&( cost));};
}
if (error) {
fatal("cover verification failed");
}
free_PLA(PLA);
((cube.part_size) ? (free((char *) (cube.part_size)), (cube.part_size) = 0) : 0);
setdown_cube(); sf_cleanup(); sm_cleanup(); exit(0);
}
getPLA(int opt, int argc, char **argv, int option, pPLA *PLA, int out_type)
{
struct _iobuf *fp;
int needs_dcset, needs_offset;
char *fname;
if (opt >= argc) {
fp = (&_iob[0]);
fname = "(stdin)";
} else {
fname = argv[opt];
if (strcmp(fname, "-") == 0) {
fp = (&_iob[0]);
} else if ((fp = fopen(argv[opt], "r")) == 0) {
fprintf((&_iob[2]), "%s: Unable to open %s\n", argv[0], fname);
exit(1);
}
}
if (option_table[option].key == KEY_echo) {
needs_dcset = (out_type & 2) != 0;
needs_offset = (out_type & 4) != 0;
} else {
needs_dcset = option_table[option].needs_dcset;
needs_offset = option_table[option].needs_offset;
}
if (read_pla(fp, needs_dcset, needs_offset, input_type, PLA) == (-1)) {
fprintf((&_iob[2]), "%s: Unable to find PLA on file %s\n", argv[0], fname);
exit(1);
}
(*PLA)->filename = util_strsav(fname);
filename = (*PLA)->filename;
last_fp = fp;
}
runtime(void)
{
int i;
long total = 1, temp;
for(i = 0; i < 16; i++) {
total += total_time[i];
}
for(i = 0; i < 16; i++) {
if (total_calls[i] != 0) {
temp = 100 * total_time[i];
printf("# %s\t%2d call(s) for %s (%2ld.%01ld%%)\n",
total_name[i], total_calls[i], util_print_time(total_time[i]),
temp/total, (10 * (temp%total)) / total);
}
}
}
init_runtime(void)
{
total_name[0] = "READ       ";
total_name[15] = "WRITE      ";
total_name[1] = "COMPL      ";
total_name[6] = "REDUCE     ";
total_name[4] = "EXPAND     ";
total_name[3] = "ESSEN      ";
total_name[5] = "IRRED      ";
total_name[9] = "REDUCE_GASP";
total_name[7] = "EXPAND_GASP";
total_name[8] = "IRRED_GASP ";
total_name[12] ="MV_REDUCE  ";
total_name[13] = "RAISE_IN   ";
total_name[14] = "VERIFY     ";
total_name[10] = "PRIMES     ";
total_name[11] = "MINCOV     ";
}
subcommands(void)
{
int i, col;
printf("                ");
col = 16;
for(i = 0; option_table[i].name != 0; i++) {
if ((col + strlen(option_table[i].name) + 1) > 76) {
printf(",\n                ");
col = 16;
} else if (i != 0) {
printf(", ");
}
printf("%s", option_table[i].name);
col += strlen(option_table[i].name) + 2;
}
printf("\n");
}
usage(void)
{
printf("%s\n\n", "UC Berkeley, Espresso Version #2.3, Release date 01/31/88");
printf("SYNOPSIS: espresso [options] [file]\n\n");
printf("  -d        Enable debugging\n");
printf("  -e[opt]   Select espresso option:\n");
printf("                fast, ness, nirr, nunwrap, onset, pos, strong,\n");
printf("                eat, eatdots, kiss, random\n");
printf("  -o[type]  Select output format:\n");
printf("                f, fd, fr, fdr, pleasure, eqntott, kiss, cons\n");
printf("  -rn-m     Select range for subcommands:\n");
printf("                d1merge: first and last variables (0 ... m-1)\n");
printf("                minterms: first and last variables (0 ... m-1)\n");
printf("                opoall: first and last outputs (0 ... m-1)\n");
printf("  -s        Provide short execution summary\n");
printf("  -t        Provide longer execution trace\n");
printf("  -x        Suppress printing of solution\n");
printf("  -v[type]  Verbose debugging detail (-v '' for all)\n");
printf("  -D[cmd]   Execute subcommand 'cmd':\n");
subcommands();
printf("  -Sn       Select strategy for subcommands:\n");
printf("                opo: bit2=exact bit1=repeated bit0=skip sparse\n");
printf("                opoall: 0=minimize, 1=exact\n");
printf("                pair: 0=algebraic, 1=strongd, 2=espresso, 3=exact\n");
printf("                pairall: 0=minimize, 1=exact, 2=opo\n");
printf("                so_espresso: 0=minimize, 1=exact\n");
printf("                so_both: 0=minimize, 1=exact\n");
}

backward_compatibility_hack(int *argc, char **argv, int *option, int *out_type)
{
int i, j;
*option = 0;
for(i = 1; i < (*argc)-1; i++) {
if (strcmp(argv[i], "-do") == 0) {
for(j = 0; option_table[j].name != 0; j++)
if (strcmp(argv[i+1], option_table[j].name) == 0) {
*option = j;
delete_arg(argc, argv, i+1);
delete_arg(argc, argv, i);
break;
}
if (option_table[j].name == 0) {
fprintf((&_iob[2]),
"espresso: bad keyword \"%s\" following -do\n",argv[i+1]);
exit(1);
}
break;
}
}
for(i = 1; i < (*argc)-1; i++) {
if (strcmp(argv[i], "-out") == 0) {
for(j = 0; pla_types[j].key != 0; j++)
if (strcmp(pla_types[j].key+1, argv[i+1]) == 0) {
*out_type = pla_types[j].value;
delete_arg(argc, argv, i+1);
delete_arg(argc, argv, i);
break;
}
if (pla_types[j].key == 0) {
fprintf((&_iob[2]),
"espresso: bad keyword \"%s\" following -out\n",argv[i+1]);
exit(1);
}
break;
}
}
for(i = 1; i < (*argc); i++) {
if (argv[i][0] == '-') {
for(j = 0; esp_opt_table[j].name != 0; j++) {
if (strcmp(argv[i]+1, esp_opt_table[j].name) == 0) {
delete_arg(argc, argv, i);
*(esp_opt_table[j].variable) = esp_opt_table[j].value;
break;
}
}
}
}
if (check_arg(argc, argv, "-fdr")) input_type = (1 | 2 | 4);
if (check_arg(argc, argv, "-fr")) input_type = (1 | 4);
if (check_arg(argc, argv, "-f")) input_type = 1;
}
delete_arg(int *argc, register char **argv, int num)
{
register int i;
(*argc)--;
for(i = num; i < *argc; i++) {
argv[i] = argv[i+1];
}
}
int check_arg(int *argc, register char **argv, register char *s)
{
register int i;
for(i = 1; i < *argc; i++) {
if (strcmp(argv[i], s) == 0) {
delete_arg(argc, argv, i);
return 1;
}
}
return 0;
}
