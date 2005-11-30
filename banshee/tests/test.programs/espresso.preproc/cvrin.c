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
extern PLA_labels(pPLA PLA);
extern char *get_word(register FILE *fp, register char *word);
extern int label_index(pPLA PLA, char *word, int *varp, int *ip);
extern int read_pla(FILE *fp, int needs_dcset, int needs_offset, int pla_type, pPLA *PLA_return);
extern int read_symbolic(FILE *fp, pPLA PLA, char *word, symbolic_t **retval);
extern pPLA new_PLA(void);
extern void PLA_summary(pPLA PLA);
extern void free_PLA(pPLA PLA);
extern void parse_pla(FILE *fp, pPLA PLA);
extern void read_cube(register FILE *fp, pPLA PLA);
extern void skip_line(register FILE *fpin, register FILE *fpout, register int echo);
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
static pset_family
reset_flags(pset_family F)
{
pset last, p;
for( p=F->data, last= p+F->count*F->wsize; p< last; p+=F->wsize) {
(p[0] &= ~ ( 0x8000));
(p[0] &= ~ ( 0x4000));
(p[0] &= ~ ( 0x2000));
(p[0] &= ~ ( 0x1000));
(p[0] &= ~ ( 0x0800));
(p[0] &= ~ ( 0x0400));
}
return F;
}
static int line_length_error;
static int lineno;
void skip_line(register FILE *fpin, register FILE *fpout, register int echo)
{
register int ch;
while ((ch=(--(fpin)->_cnt>=0? ((int)*(fpin)->_ptr++):_filbuf(fpin))) != (-1) && ch != '\n')
if (echo)
(--( fpout)->_cnt >= 0 ? (int)(*( fpout)->_ptr++ = (unsigned char)(ch)) : ((( fpout)->_flag & 0200) && -( fpout)->_cnt < ( fpout)->_bufsiz ? ((*( fpout)->_ptr = (unsigned char)(ch)) != '\n' ? (int)(*( fpout)->_ptr++) : _flsbuf(*(unsigned char *)( fpout)->_ptr, fpout)) : _flsbuf((unsigned char)(ch), fpout)));
if (echo)
(--( fpout)->_cnt >= 0 ? (int)(*( fpout)->_ptr++ = (unsigned char)('\n')) : ((( fpout)->_flag & 0200) && -( fpout)->_cnt < ( fpout)->_bufsiz ? ((*( fpout)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fpout)->_ptr++) : _flsbuf(*(unsigned char *)( fpout)->_ptr, fpout)) : _flsbuf((unsigned char)('\n'), fpout)));
lineno++;
}
char *get_word(register FILE *fp, register char *word)
{
register int ch, i = 0;
while ((ch = (--(fp)->_cnt>=0? ((int)*(fp)->_ptr++):_filbuf(fp))) != (-1) && ((_ctype_+1)[ch]&010))
;
word[i++] = ch;
while ((ch = (--(fp)->_cnt>=0? ((int)*(fp)->_ptr++):_filbuf(fp))) != (-1) && ! ((_ctype_+1)[ch]&010))
word[i++] = ch;
word[i++] = '\0';
return word;
}

void read_cube(register FILE *fp, pPLA PLA)
{
register int var, i;
pset cf = cube.temp[0], cr = cube.temp[1], cd = cube.temp[2];
int savef = 0, saved = 0, saver = 0;
char token[256]; int varx, first, last, offset;	set_clear(cf, cube.size);
for(var = 0; var < cube.num_binary_vars; var++)
switch((--(fp)->_cnt>=0? ((int)*(fp)->_ptr++):_filbuf(fp))) {
case (-1):
goto bad_char;
case '\n':
if (! line_length_error)
fprintf((&_iob[2]), "product term(s) %s\n",
"span more than one line (warning only)");
line_length_error = 1;
lineno++;
var--;
break;
case ' ': case '|': case '\t':
var--;
break;
case '2': case '-':
(cf[((( var*2+1) >> 5) + 1)] |= 1 << (( var*2+1) & (32-1)));
case '0':
(cf[((( var*2) >> 5) + 1)] |= 1 << (( var*2) & (32-1)));
break;
case '1':
(cf[((( var*2+1) >> 5) + 1)] |= 1 << (( var*2+1) & (32-1)));
break;
case '?':
break;
default:
goto bad_char;
}
for(var = cube.num_binary_vars; var < cube.num_vars-1; var++)
if (cube.part_size[var] < 0) {
(void) fscanf(fp, "%s", token);
if ((strcmp(token, "-") == 0) || (strcmp(token, "ANY") == 0)) {
if (kiss && var == cube.num_vars - 2) {
} else {
set_or(cf, cf, cube.var_mask[var]);
}
} else if ((strcmp(token, "~") == 0)) {
;
} else {
if (kiss && var == cube.num_vars - 2)
varx = var - 1, offset = ((cube.part_size[var-1]) < 0 ? -(cube.part_size[var-1]) : (cube.part_size[var-1]));
else
varx = var, offset = 0;
first = cube.first_part[varx];
last = cube.last_part[varx];
for(i = first; i <= last; i++)
if (PLA->label[i] == (char *) 0) {
PLA->label[i] = util_strsav(token);	(cf[((( i+offset) >> 5) + 1)] |= 1 << (( i+offset) & (32-1)));
break;
} else if ((strcmp(PLA->label[i], token) == 0)) {
(cf[((( i+offset) >> 5) + 1)] |= 1 << (( i+offset) & (32-1)));	break;
}
if (i > last) {
fprintf((&_iob[2]),
"declared size of variable %d (counting from variable 0) is too small\n", var);
exit(-1);
}
}
} else for(i = cube.first_part[var]; i <= cube.last_part[var]; i++)
switch ((--(fp)->_cnt>=0? ((int)*(fp)->_ptr++):_filbuf(fp))) {
case (-1):
goto bad_char;
case '\n':
if (! line_length_error)
fprintf((&_iob[2]), "product term(s) %s\n",
"span more than one line (warning only)");
line_length_error = 1;
lineno++;
i--;
break;
case ' ': case '|': case '\t':
i--;
break;
case '1':
(cf[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1)));
case '0':
break;
default:
goto bad_char;
}
if (kiss) {
saver = savef = 1;
(void) set_xor(cr, cf, cube.var_mask[cube.num_vars - 2]);
} else
set_copy(cr, cf);
set_copy(cd, cf);
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++)
switch ((--(fp)->_cnt>=0? ((int)*(fp)->_ptr++):_filbuf(fp))) {
case (-1):
goto bad_char;
case '\n':
if (! line_length_error)
fprintf((&_iob[2]), "product term(s) %s\n",
"span more than one line (warning only)");
line_length_error = 1;
lineno++;
i--;
break;
case ' ': case '|': case '\t':
i--;
break;
case '4': case '1':
if (PLA->pla_type & 1)
(cf[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1))), savef = 1;
break;
case '3': case '0':
if (PLA->pla_type & 4)
(cr[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1))), saver = 1;
break;
case '2': case '-':
if (PLA->pla_type & 2)
(cd[((( i) >> 5) + 1)] |= 1 << (( i) & (32-1))), saved = 1;
case '~':
break;
default:
goto bad_char;
}
if (savef) PLA->F = sf_addset(PLA->F, cf);
if (saved) PLA->D = sf_addset(PLA->D, cd);
if (saver) PLA->R = sf_addset(PLA->R, cr);
return;
bad_char:
fprintf((&_iob[2]), "(warning): input line #%d ignored\n", lineno);
skip_line(fp, (&_iob[1]), 1);
return;
}
void parse_pla(FILE *fp, pPLA PLA)
{
int i, var, ch, np, last;
char word[256];
lineno = 1;
line_length_error = 0;
loop:
switch(ch = (--(fp)->_cnt>=0? ((int)*(fp)->_ptr++):_filbuf(fp))) {
case (-1):
return;
case '\n':
lineno++;
case ' ': case '\t': case '\f': case '\r':
break;
case '#':
(void) ungetc(ch, fp);
skip_line(fp, (&_iob[1]), echo_comments);
break;
case '.':
if ((strcmp(get_word(fp, word), "i") == 0)) {
if (cube.fullset != 0) {
fprintf((&_iob[2]), "extra .i ignored\n");
skip_line(fp, (&_iob[1]), 0);
} else {
if (fscanf(fp, "%d", &cube.num_binary_vars) != 1)
fatal("error reading .i");
cube.num_vars = cube.num_binary_vars + 1;
cube.part_size = ((int *) malloc(sizeof(int) * ( cube.num_vars)));
}
} else if ((strcmp(word, "o") == 0)) {
if (cube.fullset != 0) {
fprintf((&_iob[2]), "extra .o ignored\n");
skip_line(fp, (&_iob[1]), 0);
} else {
if (cube.part_size == 0)
fatal(".o cannot appear before .i");
if (fscanf(fp, "%d", &(cube.part_size[cube.num_vars-1]))!=1)
fatal("error reading .o");
cube_setup();
PLA_labels(PLA);
}
} else if ((strcmp(word, "mv") == 0)) {
if (cube.fullset != 0) {
fprintf((&_iob[2]), "extra .mv ignored\n");
skip_line(fp, (&_iob[1]), 0);
} else {
if (cube.part_size != 0)
fatal("cannot mix .i and .mv");
if (fscanf(fp,"%d %d",
&cube.num_vars,&cube.num_binary_vars) != 2)
fatal("error reading .mv");
if (cube.num_binary_vars < 0)
fatal("num_binary_vars (second field of .mv) cannot be negative");
if (cube.num_vars < cube.num_binary_vars)
fatal(
"num_vars (1st field of .mv) must exceed num_binary_vars (2nd field of .mv)");
cube.part_size = ((int *) malloc(sizeof(int) * ( cube.num_vars)));
for(var=cube.num_binary_vars; var < cube.num_vars; var++)
if (fscanf(fp, "%d", &(cube.part_size[var])) != 1)
fatal("error reading .mv");
cube_setup();
PLA_labels(PLA);
}
} else if ((strcmp(word, "p") == 0))
(void) fscanf(fp, "%d", &np);
else if ((strcmp(word, "e") == 0) || (strcmp(word,"end") == 0))
return;
else if ((strcmp(word, "kiss") == 0))
kiss = 1;
else if ((strcmp(word, "type") == 0)) {
(void) get_word(fp, word);
for(i = 0; pla_types[i].key != 0; i++)
if ((strcmp(pla_types[i].key + 1, word) == 0)) {
PLA->pla_type = pla_types[i].value;
break;
}
if (pla_types[i].key == 0)
fatal("unknown type in .type command");
} else if ((strcmp(word, "ilb") == 0)) {
if (cube.fullset == 0)
fatal("PLA size must be declared before .ilb or .ob");
if (PLA->label == 0)
PLA_labels(PLA);
for(var = 0; var < cube.num_binary_vars; var++) {
(void) get_word(fp, word);
i = cube.first_part[var];
PLA->label[i+1] = util_strsav(word);
PLA->label[i] = ((char *) malloc(sizeof(char) * ( strlen(word) + 6)));
(void) sprintf(PLA->label[i], "%s.bar", word);
}
} else if ((strcmp(word, "ob") == 0)) {
if (cube.fullset == 0)
fatal("PLA size must be declared before .ilb or .ob");
if (PLA->label == 0)
PLA_labels(PLA);
var = cube.num_vars - 1;
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
(void) get_word(fp, word);
PLA->label[i] = util_strsav(word);
}
} else if ((strcmp(word, "label") == 0)) {
if (cube.fullset == 0)
fatal("PLA size must be declared before .label");
if (PLA->label == 0)
PLA_labels(PLA);
if (fscanf(fp, "var=%d", &var) != 1)
fatal("Error reading labels");
for(i = cube.first_part[var]; i <= cube.last_part[var]; i++) {
(void) get_word(fp, word);
PLA->label[i] = util_strsav(word);
}
} else if ((strcmp(word, "symbolic") == 0)) {
symbolic_t *newlist, *p1;
if (read_symbolic(fp, PLA, word, &newlist)) {
if (PLA->symbolic == ((symbolic_t *) 0)) {
PLA->symbolic = newlist;
} else {
for(p1=PLA->symbolic;p1->next!=((symbolic_t *) 0);
p1=p1->next){
}
p1->next = newlist;
}
} else {
fatal("error reading .symbolic");
}
} else if ((strcmp(word, "symbolic-output") == 0)) {
symbolic_t *newlist, *p1;
if (read_symbolic(fp, PLA, word, &newlist)) {
if (PLA->symbolic_output == ((symbolic_t *) 0)) {
PLA->symbolic_output = newlist;
} else {
for(p1=PLA->symbolic_output;p1->next!=((symbolic_t *) 0);
p1=p1->next){
}
p1->next = newlist;
}
} else {
fatal("error reading .symbolic-output");
}
} else if ((strcmp(word, "phase") == 0)) {
if (cube.fullset == 0)
fatal("PLA size must be declared before .phase");
if (PLA->phase != 0) {
fprintf((&_iob[2]), "extra .phase ignored\n");
skip_line(fp, (&_iob[1]), 0);
} else {
do ch = (--(fp)->_cnt>=0? ((int)*(fp)->_ptr++):_filbuf(fp)); while (ch == ' ' || ch == '\t');
(void) ungetc(ch, fp);
PLA->phase = set_copy(((unsigned int *) malloc(sizeof(unsigned int) * ( (((32 * (cube.fullset[0] & 0x03ff))) <= 32 ? 2 : ((((((32 * (cube.fullset[0] & 0x03ff)))-1) >> 5) + 1) + 1))))), cube.fullset);
last = cube.last_part[cube.num_vars - 1];
for(i=cube.first_part[cube.num_vars - 1]; i <= last; i++)
if ((ch = (--(fp)->_cnt>=0? ((int)*(fp)->_ptr++):_filbuf(fp))) == '0')
(PLA->phase[((( i) >> 5) + 1)] &= ~ (1 << (( i) & (32-1))));
else if (ch != '1')
fatal("only 0 or 1 allowed in phase description");
}
} else if ((strcmp(word, "pair") == 0)) {
int j;
if (PLA->pair != 0) {
fprintf((&_iob[2]), "extra .pair ignored\n");
} else {
ppair pair;
PLA->pair = pair = ((pair_t *) malloc(sizeof(pair_t) * ( 1)));
if (fscanf(fp, "%d", &(pair->cnt)) != 1)
fatal("syntax error in .pair");
pair->var1 = ((int *) malloc(sizeof(int) * ( pair->cnt)));
pair->var2 = ((int *) malloc(sizeof(int) * ( pair->cnt)));
for(i = 0; i < pair->cnt; i++) {
(void) get_word(fp, word);
if (word[0] == '(') (void) strcpy(word, word+1);
if (label_index(PLA, word, &var, &j)) {
pair->var1[i] = var+1;
} else {
fatal("syntax error in .pair");
}
(void) get_word(fp, word);
if (word[strlen(word)-1] == ')') {
word[strlen(word)-1]='\0';
}
if (label_index(PLA, word, &var, &j)) {
pair->var2[i] = var+1;
} else {
fatal("syntax error in .pair");
}
}
}
} else {
if (echo_unknown_commands)
printf("%c%s ", ch, word);
skip_line(fp, (&_iob[1]), echo_unknown_commands);
}
break;
default:
(void) ungetc(ch, fp);
if (cube.fullset == 0) {
if (echo_comments)
(--((&_iob[1]))->_cnt >= 0 ? (int)(*((&_iob[1]))->_ptr++ = (unsigned char)(('#'))) : ((((&_iob[1]))->_flag & 0200) && -((&_iob[1]))->_cnt < ((&_iob[1]))->_bufsiz ? ((*((&_iob[1]))->_ptr = (unsigned char)(('#'))) != '\n' ? (int)(*((&_iob[1]))->_ptr++) : _flsbuf(*(unsigned char *)((&_iob[1]))->_ptr, (&_iob[1]))) : _flsbuf((unsigned char)(('#')), (&_iob[1]))));
skip_line(fp, (&_iob[1]), echo_comments);
break;
}
if (PLA->F == 0) {
PLA->F = sf_new(10, cube.size);
PLA->D = sf_new(10, cube.size);
PLA->R = sf_new(10, cube.size);
}
read_cube(fp, PLA);
}
goto loop;
}
int read_pla(FILE *fp, int needs_dcset, int needs_offset, int pla_type, pPLA *PLA_return)
{
pPLA PLA;
int i, second, third;
long time;
cost_t cost;
PLA = *PLA_return = new_PLA();
PLA->pla_type = pla_type;
time = util_cpu_time();
parse_pla(fp, PLA);
if (PLA->F == 0) {
return (-1);
}
for(i = 0; i < cube.num_vars; i++) {
cube.part_size[i] = ((cube.part_size[i]) < 0 ? -(cube.part_size[i]) : (cube.part_size[i]));
}
if (kiss) {
third = cube.num_vars - 3;
second = cube.num_vars - 2;
if (cube.part_size[third] != cube.part_size[second]) {
fprintf((&_iob[2])," with .kiss option, third to last and second\n");
fprintf((&_iob[2]), "to last variables must be the same size.\n");
return (-1);
}
for(i = 0; i < cube.part_size[second]; i++) {
PLA->label[i + cube.first_part[second]] =
util_strsav(PLA->label[i + cube.first_part[third]]);
}
cube.part_size[second] += cube.part_size[cube.num_vars-1];
cube.num_vars--;
setdown_cube();
cube_setup();
}
if (trace) {
totals(time, 0, PLA->F, &cost);
}
time = util_cpu_time();
if (pos || PLA->phase != 0 || PLA->symbolic_output != ((symbolic_t *) 0)) {
needs_offset = 1;
}
if (needs_offset && (PLA->pla_type==1 || PLA->pla_type==(1 | 2))) {
sf_free(PLA->R);
PLA->R = reset_flags(complement(cube2list(PLA->F, PLA->D)));
} else if (needs_dcset && PLA->pla_type == (1 | 4)) {
pset_family X;
sf_free(PLA->D);
X = d1merge(sf_join(PLA->F, PLA->R), cube.num_vars - 1);
PLA->D = reset_flags(complement(cube1list(X)));
sf_free(X);
} else if (PLA->pla_type == 4 || PLA->pla_type == (2 | 4)) {
sf_free(PLA->F);
PLA->F = reset_flags(complement(cube2list(PLA->D, PLA->R)));
}
if (trace) {
totals(time, 1, PLA->R, &cost);
}
if (pos) {
pset_family onset = PLA->F;
PLA->F = PLA->R;
PLA->R = onset;
PLA->phase = set_clear(((unsigned int *) malloc(sizeof(unsigned int) * ( ((cube.size) <= 32 ? 2 : (((((cube.size)-1) >> 5) + 1) + 1))))), cube.size);
set_diff(PLA->phase, cube.fullset, cube.var_mask[cube.num_vars-1]);
} else if (PLA->phase != 0) {
(void) set_phase(PLA);
}
if (PLA->pair != (ppair) 0) {
set_pair(PLA);
}
if (PLA->symbolic != ((symbolic_t *) 0)) {
{long t=util_cpu_time();map_symbolic(PLA);if(trace)print_trace( PLA->F, "MAP-INPUT  ",util_cpu_time()-t);};
}
if (PLA->symbolic_output != ((symbolic_t *) 0)) {
{long t=util_cpu_time();map_output_symbolic(PLA);if(trace)print_trace( PLA->F, "MAP-OUTPUT ",util_cpu_time()-t);};
if (needs_offset) {
sf_free(PLA->R);
{long t=util_cpu_time();PLA->R=reset_flags(complement(cube2list(PLA->F,PLA->D)));totals(t, 1, PLA->R,&( cost));};
}
}
return 1;
}

void PLA_summary(pPLA PLA)
{
int var, i;
symbolic_list_t *p2;
symbolic_t *p1;
printf("# PLA is %s", PLA->filename);
if (cube.num_binary_vars == cube.num_vars - 1)
printf(" with %d inputs and %d outputs\n",
cube.num_binary_vars, cube.part_size[cube.num_vars - 1]);
else {
printf(" with %d variables (%d binary, mv sizes",
cube.num_vars, cube.num_binary_vars);
for(var = cube.num_binary_vars; var < cube.num_vars; var++)
printf(" %d", cube.part_size[var]);
printf(")\n");
}
printf("# ON-set cost is  %s\n", print_cost(PLA->F));
printf("# OFF-set cost is %s\n", print_cost(PLA->R));
printf("# DC-set cost is  %s\n", print_cost(PLA->D));
if (PLA->phase != 0)
printf("# phase is %s\n", pc1(PLA->phase));
if (PLA->pair != 0) {
printf("# two-bit decoders:");
for(i = 0; i < PLA->pair->cnt; i++)
printf(" (%d %d)", PLA->pair->var1[i], PLA->pair->var2[i]);
printf("\n");
}
if (PLA->symbolic != ((symbolic_t *) 0)) {
for(p1 = PLA->symbolic; p1 != ((symbolic_t *) 0); p1 = p1->next) {
printf("# symbolic: ");
for(p2=p1->symbolic_list; p2!=((symbolic_list_t *) 0); p2=p2->next) {
printf(" %d", p2->variable);
}
printf("\n");
}
}
if (PLA->symbolic_output != ((symbolic_t *) 0)) {
for(p1 = PLA->symbolic_output; p1 != ((symbolic_t *) 0); p1 = p1->next) {
printf("# output symbolic: ");
for(p2=p1->symbolic_list; p2!=((symbolic_list_t *) 0); p2=p2->next) {
printf(" %d", p2->pos);
}
printf("\n");
}
}
(void) fflush((&_iob[1]));
}
pPLA new_PLA(void)
{
pPLA PLA;
PLA = ((PLA_t *) malloc(sizeof(PLA_t) * ( 1)));
PLA->F = PLA->D = PLA->R = (pset_family) 0;
PLA->phase = (pset) 0;
PLA->pair = (ppair) 0;
PLA->label = (char **) 0;
PLA->filename = (char *) 0;
PLA->pla_type = 0;
PLA->symbolic = ((symbolic_t *) 0);
PLA->symbolic_output = ((symbolic_t *) 0);
return PLA;
}
PLA_labels(pPLA PLA)
{
int i;
PLA->label = ((char * *) malloc(sizeof(char *) * ( cube.size)));
for(i = 0; i < cube.size; i++)
PLA->label[i] = (char *) 0;
}
void free_PLA(pPLA PLA)
{
symbolic_list_t *p2, *p2next;
symbolic_t *p1, *p1next;
int i;
if (PLA->F != (pset_family) 0)
sf_free(PLA->F);
if (PLA->R != (pset_family) 0)
sf_free(PLA->R);
if (PLA->D != (pset_family) 0)
sf_free(PLA->D);
if (PLA->phase != (pset) 0)
((PLA->phase) ? (free((char *) (PLA->phase)), (PLA->phase) = 0) : 0);
if (PLA->pair != (ppair) 0) {
((PLA->pair->var1) ? (free((char *) (PLA->pair->var1)), (PLA->pair->var1) = 0) : 0);
((PLA->pair->var2) ? (free((char *) (PLA->pair->var2)), (PLA->pair->var2) = 0) : 0);
((PLA->pair) ? (free((char *) (PLA->pair)), (PLA->pair) = 0) : 0);
}
if (PLA->label != 0) {
for(i = 0; i < cube.size; i++)
if (PLA->label[i] != 0)
((PLA->label[i]) ? (free((char *) (PLA->label[i])), (PLA->label[i]) = 0) : 0);
((PLA->label) ? (free((char *) (PLA->label)), (PLA->label) = 0) : 0);
}
if (PLA->filename != 0) {
((PLA->filename) ? (free((char *) (PLA->filename)), (PLA->filename) = 0) : 0);
}
for(p1 = PLA->symbolic; p1 != ((symbolic_t *) 0); p1 = p1next) {
for(p2 = p1->symbolic_list; p2 != ((symbolic_list_t *) 0); p2 = p2next) {
p2next = p2->next;
((p2) ? (free((char *) (p2)), (p2) = 0) : 0);
}
p1next = p1->next;
((p1) ? (free((char *) (p1)), (p1) = 0) : 0);
}
PLA->symbolic = ((symbolic_t *) 0);
for(p1 = PLA->symbolic_output; p1 != ((symbolic_t *) 0); p1 = p1next) {
for(p2 = p1->symbolic_list; p2 != ((symbolic_list_t *) 0); p2 = p2next) {
p2next = p2->next;
((p2) ? (free((char *) (p2)), (p2) = 0) : 0);
}
p1next = p1->next;
((p1) ? (free((char *) (p1)), (p1) = 0) : 0);
}
PLA->symbolic_output = ((symbolic_t *) 0);
((PLA) ? (free((char *) (PLA)), (PLA) = 0) : 0);
}
int read_symbolic(FILE *fp, pPLA PLA, char *word, symbolic_t **retval)
{
symbolic_list_t *listp, *prev_listp;
symbolic_label_t *labelp, *prev_labelp;
symbolic_t *newlist;
int i, var;
newlist = ((symbolic_t *) malloc(sizeof(symbolic_t) * ( 1)));
newlist->next = ((symbolic_t *) 0);
newlist->symbolic_list = ((symbolic_list_t *) 0);
newlist->symbolic_list_length = 0;
newlist->symbolic_label = ((symbolic_label_t *) 0);
newlist->symbolic_label_length = 0;
prev_listp = ((symbolic_list_t *) 0);
prev_labelp = ((symbolic_label_t *) 0);
for(;;) {
(void) get_word(fp, word);
if ((strcmp(word, ";") == 0))
break;
if (label_index(PLA, word, &var, &i)) {
listp = ((symbolic_list_t *) malloc(sizeof(symbolic_list_t) * ( 1)));
listp->variable = var;
listp->pos = i;
listp->next = ((symbolic_list_t *) 0);
if (prev_listp == ((symbolic_list_t *) 0)) {
newlist->symbolic_list = listp;
} else {
prev_listp->next = listp;
}
prev_listp = listp;
newlist->symbolic_list_length++;
} else {
return 0;
}
}
for(;;) {
(void) get_word(fp, word);
if ((strcmp(word, ";") == 0))
break;
labelp = ((symbolic_label_t *) malloc(sizeof(symbolic_label_t) * ( 1)));
labelp->label = util_strsav(word);
labelp->next = ((symbolic_label_t *) 0);
if (prev_labelp == ((symbolic_label_t *) 0)) {
newlist->symbolic_label = labelp;
} else {
prev_labelp->next = labelp;
}
prev_labelp = labelp;
newlist->symbolic_label_length++;
}
*retval = newlist;
return 1;
}
int label_index(pPLA PLA, char *word, int *varp, int *ip)
{
int var, i;
if (PLA->label == ((char * *) 0) || PLA->label[0] == ((char *) 0)) {
if (sscanf(word, "%d", varp) == 1) {
*ip = *varp;
return 1;
}
} else {
for(var = 0; var < cube.num_vars; var++) {
for(i = 0; i < cube.part_size[var]; i++) {
if ((strcmp(PLA->label[cube.first_part[var]+i], word) == 0)) {
*varp = var;
*ip = i;
return 1;
}
}
}
}
return 0;
}
