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
extern char *fmt_cube(register pset c, register char *out_map, register char *s);
extern char *fmt_expanded_cube();
extern char *pc1(pset c);
extern char *pc2(pset c);
extern char *pc3();
extern int makeup_labels(pPLA PLA);
extern kiss_output(FILE *fp, pPLA PLA);
extern kiss_print_cube(FILE *fp, pPLA PLA, pset p, char *out_string);
extern output_symbolic_constraints(FILE *fp, pPLA PLA, int output_symbolic);
extern void cprint(pset_family T);
extern void debug1_print(pset_family T, char *name, int num);
extern void debug_print(pset *T, char *name, int level);
extern void eqn_output(pPLA PLA);
extern void fpr_header(FILE *fp, pPLA PLA, int output_type);
extern void fprint_pla(FILE *fp, pPLA PLA, int output_type);
extern void pls_group(pPLA PLA, FILE *fp);
extern void pls_label(pPLA PLA, FILE *fp);
extern void pls_output(pPLA PLA);
extern void print_cube(register FILE *fp, register pset c, register char *out_map);
extern void print_expanded_cube(register FILE *fp, register pset c, pset phase);
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
void fprint_pla(FILE *fp, pPLA PLA, int output_type)
{
int num;
register pset last, p;
if ((output_type & 256) != 0) {
output_symbolic_constraints(fp, PLA, 0);
output_type &= ~ 256;
if (output_type == 0) {
return;
}
}
if ((output_type & 512) != 0) {
output_symbolic_constraints(fp, PLA, 1);
output_type &= ~ 512;
if (output_type == 0) {
return;
}
}
if (output_type == 8) {
pls_output(PLA);
} else if (output_type == 16) {
eqn_output(PLA);
} else if (output_type == 128) {
kiss_output(fp, PLA);
} else {
fpr_header(fp, PLA, output_type);
num = 0;
if (output_type & 1) num += (PLA->F)->count;
if (output_type & 2) num += (PLA->D)->count;
if (output_type & 4) num += (PLA->R)->count;
fprintf(fp, ".p %d\n", num);
if (output_type == 1) {
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize) {
print_cube(fp, p, "01");
}
fprintf(fp, ".e\n");
} else {
if (output_type & 1) {
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize) {
print_cube(fp, p, "~1");
}
}
if (output_type & 2) {
for( p=PLA->D->data, last= p+PLA->D->count*PLA->D->wsize; p< last; p+=PLA->D->wsize) {
print_cube(fp, p, "~2");
}
}
if (output_type & 4) {
for( p=PLA->R->data, last= p+PLA->R->count*PLA->R->wsize; p< last; p+=PLA->R->wsize) {
print_cube(fp, p, "~0");
}
}
fprintf(fp, ".end\n");
}
}
}

void fpr_header(FILE *fp, pPLA PLA, int output_type)
{
register int i, var;
int first, last;
if (output_type != 1) {
fprintf(fp, ".type ");
if (output_type & 1) (--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('f')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('f')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('f'), fp)));
if (output_type & 2) (--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('d')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('d')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('d'), fp)));
if (output_type & 4) (--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('r')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('r')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('r'), fp)));
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
}
if (cube.num_mv_vars <= 1) {
fprintf(fp, ".i %d\n", cube.num_binary_vars);
if (cube.output != -1)
fprintf(fp, ".o %d\n", cube.part_size[cube.output]);
} else {
fprintf(fp, ".mv %d %d", cube.num_vars, cube.num_binary_vars);
for(var = cube.num_binary_vars; var < cube.num_vars; var++)
fprintf(fp, " %d", cube.part_size[var]);
fprintf(fp, "\n");
}
if (PLA->label != ((char * *) 0) && PLA->label[1] != ((char *) 0)
&& cube.num_binary_vars > 0) {
fprintf(fp, ".ilb");
for(var = 0; var < cube.num_binary_vars; var++)
fprintf(fp, " %s", PLA->label[cube.first_part[var] + 1]);
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
}
if (PLA->label != ((char * *) 0) &&
PLA->label[cube.first_part[cube.output]] != ((char *) 0)
&& cube.output != -1) {
fprintf(fp, ".ob");
for(i = 0; i < cube.part_size[cube.output]; i++)
fprintf(fp, " %s", PLA->label[cube.first_part[cube.output] + i]);
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
}
for(var = cube.num_binary_vars; var < cube.num_vars-1; var++) {
first = cube.first_part[var];
last = cube.last_part[var];
if (PLA->label != 0 && PLA->label[first] != 0) {
fprintf(fp, ".label var=%d", var);
for(i = first; i <= last; i++) {
fprintf(fp, " %s", PLA->label[i]);
}
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
}
}
if (PLA->phase != (pset) 0) {
first = cube.first_part[cube.output];
last = cube.last_part[cube.output];
fprintf(fp, "#.phase ");
for(i = first; i <= last; i++)
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)((PLA->phase[(((i) >> 5) + 1)] & (1 << ((i) & (32-1)))) ? '1' : '0')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)((PLA->phase[(((i) >> 5) + 1)] & (1 << ((i) & (32-1)))) ? '1' : '0')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)((PLA->phase[(((i) >> 5) + 1)] & (1 << ((i) & (32-1)))) ? '1' : '0'), fp)));
fprintf(fp, "\n");
}
}

void pls_output(pPLA PLA)
{
register pset last, p;
printf(".option unmerged\n");
makeup_labels(PLA);
pls_label(PLA, (&_iob[1]));
pls_group(PLA, (&_iob[1]));
printf(".p %d\n", PLA->F->count);
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize) {
print_expanded_cube((&_iob[1]), p, PLA->phase);
}
printf(".end\n");
}
void pls_group(pPLA PLA, FILE *fp)
{
int var, i, col, len;
fprintf(fp, "\n.group");
col = 6;
for(var = 0; var < cube.num_vars-1; var++) {
fprintf(fp, " ("), col += 2;
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
len = strlen(PLA->label[i]);
if (col + len > 75)
fprintf(fp, " \\\n"), col = 0;
else if (i != 0)
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(' ')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(' ')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(' '), fp))), col += 1;
fprintf(fp, "%s", PLA->label[i]), col += len;
}
fprintf(fp, ")"), col += 1;
}
fprintf(fp, "\n");
}
void pls_label(pPLA PLA, FILE *fp)
{
int var, i, col, len;
fprintf(fp, ".label");
col = 6;
for(var = 0; var < cube.num_vars; var++)
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
len = strlen(PLA->label[i]);
if (col + len > 75)
fprintf(fp, " \\\n"), col = 0;
else
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(' ')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(' ')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(' '), fp))), col += 1;
fprintf(fp, "%s", PLA->label[i]), col += len;
}
}
void eqn_output(pPLA PLA)
{
register pset p, last;
register int i, var, col, len;
int x;
int firstand, firstor;
if (cube.output == -1)
fatal("Cannot have no-output function for EQNTOTT output mode");
if (cube.num_mv_vars != 1)
fatal("Must have binary-valued function for EQNTOTT output mode");
makeup_labels(PLA);
for(i = 0; i < cube.part_size[cube.output]; i++) {
printf("%s = ", PLA->label[cube.first_part[cube.output] + i]);
col = strlen(PLA->label[cube.first_part[cube.output] + i]) + 3;
firstor = 1;
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize)
if ((p[((( i + cube.first_part[cube.output]) >> 5) + 1)] & (1 << (( i + cube.first_part[cube.output]) & (32-1))))) {
if (firstor)
printf("("), col += 1;
else
printf(" | ("), col += 4;
firstor = 0;
firstand = 1;
for(var = 0; var < cube.num_binary_vars; var++)
if ((x=((p[(((2* var) >> 5) + 1)] >> ((2* var) & (32-1))) & 3)) != 3) {
len = strlen(PLA->label[cube.first_part[var] + 1]);
if (col+len > 72)
printf("\n    "), col = 4;
if (! firstand)
printf("&"), col += 1;
firstand = 0;
if (x == 1)
printf("!"), col += 1;
printf("%s", PLA->label[cube.first_part[var] + 1]), col += len;
}
printf(")"), col += 1;
}
printf(";\n\n");
}
}
char *fmt_cube(register pset c, register char *out_map, register char *s)
{
register int i, var, last, len = 0;
for(var = 0; var < cube.num_binary_vars; var++) {
s[len++] = "?01-" [((c[(((2* var) >> 5) + 1)] >> ((2* var) & (32-1))) & 3)];
}
for(var = cube.num_binary_vars; var < cube.num_vars - 1; var++) {
s[len++] = ' ';
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
s[len++] = "01" [(c[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))) != 0];
}
}
if (cube.output != -1) {
last = cube.last_part[cube.output];
s[len++] = ' ';
for(i = cube.first_part[cube.output]; i <= last; i++) {
s[len++] = out_map [(c[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))) != 0];
}
}
s[len] = '\0';
return s;
}
void print_cube(register FILE *fp, register pset c, register char *out_map)
{
register int i, var, ch;
int last;
for(var = 0; var < cube.num_binary_vars; var++) {
ch = "?01-" [((c[(((2* var) >> 5) + 1)] >> ((2* var) & (32-1))) & 3)];
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(ch)) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(ch)) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(ch), fp)));
}
for(var = cube.num_binary_vars; var < cube.num_vars - 1; var++) {
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(' ')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(' ')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(' '), fp)));
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
ch = "01" [(c[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))) != 0];
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(ch)) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(ch)) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(ch), fp)));
}
}
if (cube.output != -1) {
last = cube.last_part[cube.output];
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(' ')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(' ')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(' '), fp)));
for(i = cube.first_part[cube.output]; i <= last; i++) {
ch = out_map [(c[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))) != 0];
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(ch)) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(ch)) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(ch), fp)));
}
}
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
}
void print_expanded_cube(register FILE *fp, register pset c, pset phase)
{
register int i, var, ch;
char *out_map;
for(var = 0; var < cube.num_binary_vars; var++) {
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
ch = "~1" [(c[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))) != 0];
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(ch)) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(ch)) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(ch), fp)));
}
}
for(var = cube.num_binary_vars; var < cube.num_vars - 1; var++) {
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
ch = "1~" [(c[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))) != 0];
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(ch)) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(ch)) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(ch), fp)));
}
}
if (cube.output != -1) {
var = cube.num_vars - 1;
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(' ')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(' ')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(' '), fp)));
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
if (phase == (pset) 0 || (phase[((( i) >> 5) + 1)] & (1 << (( i) & (32-1))))) {
out_map = "~1";
} else {
out_map = "~0";
}
ch = out_map[(c[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))) != 0];
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(ch)) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(ch)) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(ch), fp)));
}
}
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
}
char *pc1(pset c)
{static char s1[256];return fmt_cube(c, "01", s1);}
char *pc2(pset c)
{static char s2[256];return fmt_cube(c, "01", s2);}
void debug_print(pset *T, char *name, int level)
{
register pset *T1, p, temp;
register int cnt;
cnt = (((pset *) T[1] - T) - 3);
temp = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
if (verbose_debug && level == 0)
printf("\n");
printf("%s[%d]: ord(T)=%d\n", name, level, cnt);
if (verbose_debug) {
printf("cofactor=%s\n", pc1(T[0]));
for(T1 = T+2, cnt = 1; (p = *T1++) != (pset) 0; cnt++)
printf("%4d. %s\n", cnt, pc1(set_or(temp, p, T[0])));
}
((temp) ? (free((char *) (temp)), (temp) = 0) : 0);
}
void debug1_print(pset_family T, char *name, int num)
{
register int cnt = 1;
register pset p, last;
if (verbose_debug && num == 0)
printf("\n");
printf("%s[%d]: ord(T)=%d\n", name, num, T->count);
if (verbose_debug)
for( p=T->data, last= p+T->count*T->wsize; p< last; p+=T->wsize)
printf("%4d. %s\n", cnt++, pc1(p));
}
void cprint(pset_family T)
{
register pset p, last;
for( p=T->data, last= p+T->count*T->wsize; p< last; p+=T->wsize)
printf("%s\n", pc1(p));
}
int makeup_labels(pPLA PLA)
{
int var, i, ind;
if (PLA->label == (char **) 0)
PLA_labels(PLA);
for(var = 0; var < cube.num_vars; var++)
for(i = 0; i < cube.part_size[var]; i++) {
ind = cube.first_part[var] + i;
if (PLA->label[ind] == (char *) 0) {
PLA->label[ind] = ((char *) malloc(sizeof(char) * ( 15)));
if (var < cube.num_binary_vars)
if ((i % 2) == 0)
(void) sprintf(PLA->label[ind], "v%d.bar", var);
else
(void) sprintf(PLA->label[ind], "v%d", var);
else
(void) sprintf(PLA->label[ind], "v%d.%d", var, i);
}
}
}
kiss_output(FILE *fp, pPLA PLA)
{
register pset last, p;
for( p=PLA->F->data, last= p+PLA->F->count*PLA->F->wsize; p< last; p+=PLA->F->wsize) {
kiss_print_cube(fp, PLA, p, "~1");
}
for( p=PLA->D->data, last= p+PLA->D->count*PLA->D->wsize; p< last; p+=PLA->D->wsize) {
kiss_print_cube(fp, PLA, p, "~2");
}
}
kiss_print_cube(FILE *fp, pPLA PLA, pset p, char *out_string)
{
register int i, var;
int part, x;
for(var = 0; var < cube.num_binary_vars; var++) {
x = "?01-" [((p[(((2* var) >> 5) + 1)] >> ((2* var) & (32-1))) & 3)];
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(x)) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(x)) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(x), fp)));
}
for(var = cube.num_binary_vars; var < cube.num_vars - 1; var++) {
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(' ')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(' ')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(' '), fp)));
if (setp_implies(cube.var_mask[var], p)) {
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('-')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('-')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('-'), fp)));
} else {
part = -1;
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
if ((p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1))))) {
if (part != -1) {
fatal("more than 1 part in a symbolic variable\n");
}
part = i;
}
}
if (part == -1) {
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('~')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('~')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('~'), fp)));	} else {
(void) fputs(PLA->label[part], fp);
}
}
}
if ((var = cube.output) != -1) {
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(' ')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(' ')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(' '), fp)));
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
x = out_string [(p[((( i) >> 5) + 1)] & (1 << (( i) & (32-1)))) != 0];
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(x)) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(x)) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(x), fp)));
}
}
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
}

output_symbolic_constraints(FILE *fp, pPLA PLA, int output_symbolic)
{
pset_family A;
register int i, j;
int size, var, npermute, *permute, *weight, noweight;
if ((cube.num_vars - cube.num_binary_vars) <= 1) {
return;
}
makeup_labels(PLA);
for(var=cube.num_binary_vars; var < cube.num_vars-1; var++) {
npermute = cube.part_size[var];
permute = ((int *) malloc(sizeof(int) * ( npermute)));
for(i=0; i < npermute; i++) {
permute[i] = cube.first_part[var] + i;
}
A = sf_permute(sf_save(PLA->F), permute, npermute);
((permute) ? (free((char *) (permute)), (permute) = 0) : 0);
noweight = 0;
for(i = 0; i < A->count; i++) {
size = set_ord(((A)->data + (A)->wsize * (i)));
if (size == 1 || size == A->sf_size) {
sf_delset(A, i--);
noweight++;
}
}
weight = ((int *) malloc(sizeof(int) * ( A->count)));
for(i = 0; i < A->count; i++) {
(((A)->data + (A)->wsize * ( i))[0] &= ~ ( 0x0800));
}
for(i = 0; i < A->count; i++) {
weight[i] = 0;
if (! (((A)->data + (A)->wsize * (i))[0] & ( 0x0800))) {
weight[i] = 1;
for(j = i+1; j < A->count; j++) {
if (setp_equal(((A)->data + (A)->wsize * (i)), ((A)->data + (A)->wsize * (j)))) {
weight[i]++;
(((A)->data + (A)->wsize * (j))[0] |= ( 0x0800));
}
}
}
}
if (! output_symbolic) {
(void) fprintf(fp,
"# Symbolic constraints for variable %d (Numeric form)\n", var);
(void) fprintf(fp, "# unconstrained weight = %d\n", noweight);
(void) fprintf(fp, "num_codes=%d\n", cube.part_size[var]);
for(i = 0; i < A->count; i++) {
if (weight[i] > 0) {
(void) fprintf(fp, "weight=%d: ", weight[i]);
for(j = 0; j < A->sf_size; j++) {
if ((((A)->data + (A)->wsize * (i))[((( j) >> 5) + 1)] & (1 << (( j) & (32-1))))) {
(void) fprintf(fp, " %d", j);
}
}
(void) fprintf(fp, "\n");
}
}
} else {
(void) fprintf(fp,
"# Symbolic constraints for variable %d (Symbolic form)\n", var);
for(i = 0; i < A->count; i++) {
if (weight[i] > 0) {
(void) fprintf(fp, "#   w=%d: (", weight[i]);
for(j = 0; j < A->sf_size; j++) {
if ((((A)->data + (A)->wsize * (i))[((( j) >> 5) + 1)] & (1 << (( j) & (32-1))))) {
(void) fprintf(fp, " %s",
PLA->label[cube.first_part[var]+j]);
}
}
(void) fprintf(fp, " )\n");
}
}
((weight) ? (free((char *) (weight)), (weight) = 0) : 0);
}
}
}
