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
extern sm_row *sm_minimum_cover(sm_matrix *A, int *weight, int heuristic, int debug_level);
typedef struct stats_struct stats_t;
struct stats_struct {
int debug;	int max_print_depth;	int max_depth;	int nodes;	int component;	int comp_count;	int gimpel_count;	int gimpel;	long start_time;	int no_branching;
int lower_bound;
};
typedef struct solution_struct solution_t;
struct solution_struct {
sm_row *row;
int cost;
};
extern solution_t *solution_alloc();
extern void solution_free();
extern solution_t *solution_dup();
extern void solution_accept();
extern void solution_reject();
extern void solution_add();
extern solution_t *solution_choose_best();
extern solution_t *sm_maximal_independent_set();
extern solution_t *sm_mincov(sm_matrix *A, solution_t *select, int *weight, int lb, int bound, int depth, stats_t *stats);
extern int gimpel_reduce();
extern int select_column();
extern void select_essential();
extern int verify_cover();
sm_row *
sm_minimum_cover(sm_matrix *A, int *weight, int heuristic, int debug_level)
{
stats_t stats;
solution_t *best, *select;
sm_row *prow, *sol;
sm_col *pcol;
sm_matrix *dup_A;
int nelem, bound;
double sparsity;
if (A->nrows <= 0) {
return sm_row_alloc();	}
stats.start_time = util_cpu_time();
stats.debug = debug_level > 0;
stats.max_print_depth = debug_level;
stats.max_depth = -1;
stats.nodes = 0;
stats.component = stats.comp_count = 0;
stats.gimpel = stats.gimpel_count = 0;
stats.no_branching = heuristic != 0;
stats.lower_bound = -1;
nelem = 0;
for( prow = A->first_row; prow != 0; prow = prow->next_row) {
nelem += prow->length;
}
sparsity = (double) nelem / (double) (A->nrows * A->ncols);
bound = 1;
for( pcol = A->first_col; pcol != 0; pcol = pcol->next_col) {
bound += (weight == ((int *) 0) ? 1 : weight[ pcol->col_num]);
}
select = solution_alloc();
dup_A = sm_dup(A);
best = sm_mincov(dup_A, select, weight, 0, bound, 0, &stats);
sm_free(dup_A);
solution_free(select);
if (stats.debug) {
if (stats.no_branching) {
(void) printf("**** heuristic covering ...\n");
(void) printf("lower bound = %d\n", stats.lower_bound);
}
(void) printf("matrix     = %d by %d with %d elements (%4.3f%%)\n",
A->nrows, A->ncols, nelem, sparsity * 100.0);
(void) printf("cover size = %d elements\n", best->row->length);
(void) printf("cover cost = %d\n", best->cost);
(void) printf("time       = %s\n", util_print_time(util_cpu_time() - stats.start_time));
(void) printf("components = %d\n", stats.comp_count);
(void) printf("gimpel     = %d\n", stats.gimpel_count);
(void) printf("nodes      = %d\n", stats.nodes);
(void) printf("max_depth  = %d\n", stats.max_depth);
}
sol = sm_row_dup(best->row);
if (! verify_cover(A, sol)) {
{ (void) fprintf((&_iob[2]), "Fatal error: file %s, line %d\n%s\n", "mincov.c", 86, "mincov: internal error -- cover verification failed\n"); (void) fflush((&_iob[1])); abort();};
}
solution_free(best);
return sol;
}

solution_t * sm_mincov(sm_matrix *A, solution_t *select, int *weight, int lb, int bound, int depth, stats_t *stats)
{
sm_matrix *A1, *A2, *L, *R;
sm_element *p;
solution_t *select1, *select2, *best, *best1, *best2, *indep;
int pick, lb_new, debug;
stats->nodes++;
if (depth > stats->max_depth) stats->max_depth = depth;
debug = stats->debug && (depth <= stats->max_print_depth);
select_essential(A, select, weight, bound);
if (select->cost >= bound) {
return ((solution_t *) 0);
}
if ( weight == ((int *) 0)) {	if (gimpel_reduce(A, select, weight, lb, bound, depth, stats, &best)) {
return best;
}
}
indep = sm_maximal_independent_set(A, weight);
lb_new = ((select->cost + indep->cost) > ( lb) ? (select->cost + indep->cost) : ( lb));
pick = select_column(A, weight, indep);
solution_free(indep);
if (depth == 0) {
stats->lower_bound = lb_new + stats->gimpel;
}
if (debug) {
(void) printf("ABSMIN[%2d]%s", depth, stats->component ? "*" : " ");
(void) printf(" %3dx%3d sel=%3d bnd=%3d lb=%3d %12s ",
A->nrows, A->ncols, select->cost + stats->gimpel, bound + stats->gimpel, lb_new + stats->gimpel, util_print_time(util_cpu_time()-stats->start_time));
}
if (lb_new >= bound) {
if (debug) (void) printf("bounded\n");
best = ((solution_t *) 0);
} else if (A->nrows == 0) {
best = solution_dup(select);
if (debug) (void) printf("BEST\n");
if (stats->debug && stats->component == 0) {
(void) printf("new 'best' solution %d at level %d (time is %s)\n", best->cost + stats->gimpel, depth, util_print_time(util_cpu_time() - stats->start_time));
}
} else if (sm_block_partition(A, &L, &R)) {
if (L->ncols > R->ncols) {
A1 = L;
L = R;
R = A1;
}
if (debug) (void) printf("comp %d %d\n", L->nrows, R->nrows);
stats->comp_count++;
select1 = solution_alloc();
stats->component++;
best1 = sm_mincov(L, select1, weight, 0, bound-select->cost, depth+1, stats);
stats->component--;
solution_free(select1);
sm_free(L);
if (best1 == ((solution_t *) 0)) {
best = ((solution_t *) 0);
} else {
for(p = best1->row->first_col; p != 0; p = p->next_col) {
solution_add(select, weight, p->col_num);
}
solution_free(best1);
best = sm_mincov(R, select, weight, lb_new, bound, depth+1, stats);
}
sm_free(R);
} else {
if (debug) (void) printf("pick=%d\n", pick);
A1 = sm_dup(A);
select1 = solution_dup(select);
solution_accept(select1, A1, weight, pick);
best1 = sm_mincov(A1, select1, weight, lb_new, bound, depth+1, stats);
solution_free(select1);
sm_free(A1);
if (best1 != ((solution_t *) 0) && bound > best1->cost) {
bound = best1->cost;
}
if (stats->no_branching) {
return best1;
}
if (best1 != ((solution_t *) 0) && best1->cost == lb_new) {
return best1;
}
A2 = sm_dup(A);
select2 = solution_dup(select);
solution_reject(select2, A2, weight, pick);
best2 = sm_mincov(A2, select2, weight, lb_new, bound, depth+1, stats);
solution_free(select2);
sm_free(A2);
best = solution_choose_best(best1, best2);
}
return best;
}

static int select_column(sm_matrix *A, int *weight, solution_t *indep)
{
register sm_col *pcol;
register sm_row *prow, *indep_cols;
register sm_element *p, *p1;
double w, best;
int best_col;
indep_cols = sm_row_alloc();
if (indep != ((solution_t *) 0)) {
for(p = indep->row->first_col; p != 0; p = p->next_col) {
prow = ((( p->col_num) >= 0 && ( p->col_num) < (A)->rows_size) ? (A)->rows[ p->col_num] : (sm_row *) 0);
for(p1 = prow->first_col; p1 != 0; p1 = p1->next_col) {
(void) sm_row_insert(indep_cols, p1->col_num);
}
}
} else {
for( pcol = A->first_col; pcol != 0; pcol = pcol->next_col) {
(void) sm_row_insert(indep_cols, pcol->col_num);
}
}
best_col = -1;
best = -1;
for( p1 = indep_cols->first_col; p1 != 0; p1 = p1->next_col) {
pcol = ((( p1->col_num) >= 0 && ( p1->col_num) < (A)->cols_size) ? (A)->cols[ p1->col_num] : (sm_col *) 0);
w = 0.0;
for(p = pcol->first_row; p != 0; p = p->next_row) {
prow = ((( p->row_num) >= 0 && ( p->row_num) < (A)->rows_size) ? (A)->rows[ p->row_num] : (sm_row *) 0);
w += 1.0 / ((double) prow->length - 1.0);
}
w = w / (double) (weight == ((int *) 0) ? 1 : weight[ pcol->col_num]);
if (w > best) {
best_col = pcol->col_num;
best = w;
}
}
sm_row_free(indep_cols);
return best_col;
}

static void select_essential(sm_matrix *A, solution_t *select, int *weight, int bound)
{
register sm_element *p;
register sm_row *prow, *essen;
int delcols, delrows, essen_count;
do {
delcols = sm_col_dominance(A, weight);
essen = sm_row_alloc();
for( prow = A->first_row; prow != 0; prow = prow->next_row) {
if (prow->length == 1) {
(void) sm_row_insert(essen, prow->first_col->col_num);
}
}
for( p = essen->first_col; p != 0; p = p->next_col) {
solution_accept(select, A, weight, p->col_num);
if (select->cost >= bound) {
sm_row_free(essen);
return;
}
}
essen_count = essen->length;
sm_row_free(essen);
delrows = sm_row_dominance(A);
} while (delcols > 0 || delrows > 0 || essen_count > 0);
}

static int verify_cover(sm_matrix *A, sm_row *cover)
{
sm_row *prow;
for( prow = A->first_row; prow != 0; prow = prow->next_row) {
if (! sm_row_intersects(prow, cover)) {
return 0;
}
}
return 1;
}
