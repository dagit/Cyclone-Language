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
extern sm_row *sm_row_alloc(void), *sm_row_dup(register sm_row *prow), *sm_row_and(sm_row *p1, sm_row *p2);
extern void sm_row_free(register sm_row *prow), sm_row_remove(register sm_row *prow, register int col), sm_row_print(FILE *fp, sm_row *prow);
extern sm_element *sm_row_insert(register sm_row *prow, register int col), *sm_row_find(sm_row *prow, int col);
extern int sm_row_contains(sm_row *p1, sm_row *p2), sm_row_intersects(sm_row *p1, sm_row *p2);
extern int sm_row_compare(sm_row *p1, sm_row *p2), sm_row_hash(sm_row *prow, int modulus);
extern sm_col *sm_col_alloc(), *sm_col_dup(), *sm_col_and();
extern void sm_col_free(), sm_col_remove(), sm_col_print();
extern sm_element *sm_col_insert(), *sm_col_find();
extern int sm_col_contains(), sm_col_intersects();
extern int sm_col_compare(), sm_col_hash();
extern int sm_row_dominance(), sm_col_dominance(), sm_block_partition();
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
extern void sm_row_remove_element(register sm_row *prow, register sm_element *p);
extern void sm_col_remove_element();
sm_row *
sm_row_alloc(void)
{
register sm_row *prow;
prow = ((sm_row *) malloc(sizeof(sm_row) * ( 1)));
prow->row_num = 0;
prow->length = 0;
prow->first_col = prow->last_col = ((sm_element *) 0);
prow->next_row = prow->prev_row = ((sm_row *) 0);
prow->flag = 0;
prow->user_word = ((char *) 0);	return prow;
}
void
sm_row_free(register sm_row *prow)
{
register sm_element *p, *pnext;
for(p = prow->first_col; p != 0; p = pnext) {
pnext = p->next_col;
((p) ? (free((char *) (p)), (p) = 0) : 0);
}
((prow) ? (free((char *) (prow)), (prow) = 0) : 0);
}
sm_row *
sm_row_dup(register sm_row *prow)
{
register sm_row *pnew;
register sm_element *p;
pnew = sm_row_alloc();
for(p = prow->first_col; p != 0; p = p->next_col) {
(void) sm_row_insert(pnew, p->col_num);
}
return pnew;
}
sm_element *
sm_row_insert(register sm_row *prow, register int col)
{
register sm_element *test, *element;
element = ((sm_element *) malloc(sizeof(sm_element) * ( 1))); element->user_word = ((char *) 0);;
test = element;
if ( prow->last_col == 0) { test-> col_num = col; prow->first_col = test; prow->last_col = test; test-> next_col = 0; test-> prev_col = 0; prow->length++; } else if ( prow->last_col-> col_num < col) { test-> col_num = col; prow->last_col-> next_col = test; test-> prev_col = prow->last_col; prow->last_col = test; test-> next_col = 0; prow->length++; } else if ( prow->first_col-> col_num > col) { test-> col_num = col; prow->first_col-> prev_col = test; test-> next_col = prow->first_col; prow->first_col = test; test-> prev_col = 0; prow->length++; } else { sm_element *p; for(p = prow->first_col; p-> col_num < col; p = p-> next_col) ; if (p-> col_num > col) { test-> col_num = col; p = p-> prev_col; p-> next_col-> prev_col = test; test-> next_col = p-> next_col; p-> next_col = test; test-> prev_col = p; prow->length++; } else { test = p; } };
if (element != test) {
((element) ? (free((char *) (element)), (element) = 0) : 0);
}
return test;
}
void
sm_row_remove(register sm_row *prow, register int col)
{
register sm_element *p;
for(p = prow->first_col; p != 0 && p->col_num < col; p = p->next_col)
;
if (p != 0 && p->col_num == col) {
{ if (p-> prev_col == 0) { prow->first_col = p-> next_col; } else { p-> prev_col-> next_col = p-> next_col; } if (p-> next_col == 0) { prow->last_col = p-> prev_col; } else { p-> next_col-> prev_col = p-> prev_col; } prow->length--; };
((p) ? (free((char *) (p)), (p) = 0) : 0);
}
}
sm_element *
sm_row_find(sm_row *prow, int col)
{
register sm_element *p;
for(p = prow->first_col; p != 0 && p->col_num < col; p = p->next_col)
;
if (p != 0 && p->col_num == col) {
return p;
} else {
return ((sm_element *) 0);
}
}

int sm_row_contains(sm_row *p1, sm_row *p2)
{
register sm_element *q1, *q2;
q1 = p1->first_col;
q2 = p2->first_col;
while (q1 != 0) {
if (q2 == 0 || q1->col_num < q2->col_num) {
return 0;
} else if (q1->col_num == q2->col_num) {
q1 = q1->next_col;
q2 = q2->next_col;
} else {
q2 = q2->next_col;
}
}
return 1;
}
int sm_row_intersects(sm_row *p1, sm_row *p2)
{
register sm_element *q1, *q2;
q1 = p1->first_col;
q2 = p2->first_col;
if (q1 == 0 || q2 == 0) return 0;
for(;;) {
if (q1->col_num < q2->col_num) {
if ((q1 = q1->next_col) == 0) {
return 0;
}
} else if (q1->col_num > q2->col_num) {
if ((q2 = q2->next_col) == 0) {
return 0;
}
} else {
return 1;
}
}
}
int sm_row_compare(sm_row *p1, sm_row *p2)
{
register sm_element *q1, *q2;
q1 = p1->first_col;
q2 = p2->first_col;
while(q1 != 0 && q2 != 0) {
if (q1->col_num != q2->col_num) {
return q1->col_num - q2->col_num;
}
q1 = q1->next_col;
q2 = q2->next_col;
}
if (q1 != 0) {
return 1;
} else if (q2 != 0) {
return -1;
} else {
return 0;
}
}
sm_row *
sm_row_and(sm_row *p1, sm_row *p2)
{
register sm_element *q1, *q2;
register sm_row *result;
result = sm_row_alloc();
q1 = p1->first_col;
q2 = p2->first_col;
if (q1 == 0 || q2 == 0) return result;
for(;;) {
if (q1->col_num < q2->col_num) {
if ((q1 = q1->next_col) == 0) {
return result;
}
} else if (q1->col_num > q2->col_num) {
if ((q2 = q2->next_col) == 0) {
return result;
}
} else {
(void) sm_row_insert(result, q1->col_num);
if ((q1 = q1->next_col) == 0) {
return result;
}
if ((q2 = q2->next_col) == 0) {
return result;
}
}
}
}

int sm_row_hash(sm_row *prow, int modulus)
{
register int sum;
register sm_element *p;
sum = 0;
for(p = prow->first_col; p != 0; p = p->next_col) {
sum = (sum*17 + p->col_num) % modulus;
}
return sum;
}

void
sm_row_remove_element(register sm_row *prow, register sm_element *p)
{
{ if (p-> prev_col == 0) { prow->first_col = p-> next_col; } else { p-> prev_col-> next_col = p-> next_col; } if (p-> next_col == 0) { prow->last_col = p-> prev_col; } else { p-> next_col-> prev_col = p-> prev_col; } prow->length--; };
((p) ? (free((char *) (p)), (p) = 0) : 0);
}
void
sm_row_print(FILE *fp, sm_row *prow)
{
sm_element *p;
for(p = prow->first_col; p != 0; p = p->next_col) {
(void) fprintf(fp, " %d", p->col_num);
}
}
