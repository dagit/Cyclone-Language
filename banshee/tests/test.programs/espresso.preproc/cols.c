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
extern sm_col *sm_col_alloc(void), *sm_col_dup(register sm_col *pcol), *sm_col_and(sm_col *p1, sm_col *p2);
extern void sm_col_free(register sm_col *pcol), sm_col_remove(register sm_col *pcol, register int row), sm_col_print(FILE *fp, sm_col *pcol);
extern sm_element *sm_col_insert(register sm_col *pcol, register int row), *sm_col_find(sm_col *pcol, int row);
extern int sm_col_contains(sm_col *p1, sm_col *p2), sm_col_intersects(sm_col *p1, sm_col *p2);
extern int sm_col_compare(sm_col *p1, sm_col *p2), sm_col_hash(sm_col *pcol, int modulus);
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
extern void sm_row_remove_element();
extern void sm_col_remove_element(register sm_col *pcol, register sm_element *p);
sm_col *
sm_col_alloc(void)
{
register sm_col *pcol;
pcol = ((sm_col *) malloc(sizeof(sm_col) * ( 1)));
pcol->col_num = 0;
pcol->length = 0;
pcol->first_row = pcol->last_row = ((sm_element *) 0);
pcol->next_col = pcol->prev_col = ((sm_col *) 0);
pcol->flag = 0;
pcol->user_word = ((char *) 0);	return pcol;
}
void
sm_col_free(register sm_col *pcol)
{
register sm_element *p, *pnext;
for(p = pcol->first_row; p != 0; p = pnext) {
pnext = p->next_row;
((p) ? (free((char *) (p)), (p) = 0) : 0);
}
((pcol) ? (free((char *) (pcol)), (pcol) = 0) : 0);
}
sm_col *
sm_col_dup(register sm_col *pcol)
{
register sm_col *pnew;
register sm_element *p;
pnew = sm_col_alloc();
for(p = pcol->first_row; p != 0; p = p->next_row) {
(void) sm_col_insert(pnew, p->row_num);
}
return pnew;
}
sm_element *
sm_col_insert(register sm_col *pcol, register int row)
{
register sm_element *test, *element;
element = ((sm_element *) malloc(sizeof(sm_element) * ( 1))); element->user_word = ((char *) 0);;
test = element;
if ( pcol->last_row == 0) { test-> row_num = row; pcol->first_row = test; pcol->last_row = test; test-> next_row = 0; test-> prev_row = 0; pcol->length++; } else if ( pcol->last_row-> row_num < row) { test-> row_num = row; pcol->last_row-> next_row = test; test-> prev_row = pcol->last_row; pcol->last_row = test; test-> next_row = 0; pcol->length++; } else if ( pcol->first_row-> row_num > row) { test-> row_num = row; pcol->first_row-> prev_row = test; test-> next_row = pcol->first_row; pcol->first_row = test; test-> prev_row = 0; pcol->length++; } else { sm_element *p; for(p = pcol->first_row; p-> row_num < row; p = p-> next_row) ; if (p-> row_num > row) { test-> row_num = row; p = p-> prev_row; p-> next_row-> prev_row = test; test-> next_row = p-> next_row; p-> next_row = test; test-> prev_row = p; pcol->length++; } else { test = p; } };
if (element != test) {
((element) ? (free((char *) (element)), (element) = 0) : 0);
}
return test;
}
void
sm_col_remove(register sm_col *pcol, register int row)
{
register sm_element *p;
for(p = pcol->first_row; p != 0 && p->row_num < row; p = p->next_row)
;
if (p != 0 && p->row_num == row) {
{ if (p-> prev_row == 0) { pcol->first_row = p-> next_row; } else { p-> prev_row-> next_row = p-> next_row; } if (p-> next_row == 0) { pcol->last_row = p-> prev_row; } else { p-> next_row-> prev_row = p-> prev_row; } pcol->length--; };
((p) ? (free((char *) (p)), (p) = 0) : 0);
}
}
sm_element *
sm_col_find(sm_col *pcol, int row)
{
register sm_element *p;
for(p = pcol->first_row; p != 0 && p->row_num < row; p = p->next_row)
;
if (p != 0 && p->row_num == row) {
return p;
} else {
return ((sm_element *) 0);
}
}

int sm_col_contains(sm_col *p1, sm_col *p2)
{
register sm_element *q1, *q2;
q1 = p1->first_row;
q2 = p2->first_row;
while (q1 != 0) {
if (q2 == 0 || q1->row_num < q2->row_num) {
return 0;
} else if (q1->row_num == q2->row_num) {
q1 = q1->next_row;
q2 = q2->next_row;
} else {
q2 = q2->next_row;
}
}
return 1;
}
int sm_col_intersects(sm_col *p1, sm_col *p2)
{
register sm_element *q1, *q2;
q1 = p1->first_row;
q2 = p2->first_row;
if (q1 == 0 || q2 == 0) return 0;
for(;;) {
if (q1->row_num < q2->row_num) {
if ((q1 = q1->next_row) == 0) {
return 0;
}
} else if (q1->row_num > q2->row_num) {
if ((q2 = q2->next_row) == 0) {
return 0;
}
} else {
return 1;
}
}
}
int sm_col_compare(sm_col *p1, sm_col *p2)
{
register sm_element *q1, *q2;
q1 = p1->first_row;
q2 = p2->first_row;
while(q1 != 0 && q2 != 0) {
if (q1->row_num != q2->row_num) {
return q1->row_num - q2->row_num;
}
q1 = q1->next_row;
q2 = q2->next_row;
}
if (q1 != 0) {
return 1;
} else if (q2 != 0) {
return -1;
} else {
return 0;
}
}
sm_col *
sm_col_and(sm_col *p1, sm_col *p2)
{
register sm_element *q1, *q2;
register sm_col *result;
result = sm_col_alloc();
q1 = p1->first_row;
q2 = p2->first_row;
if (q1 == 0 || q2 == 0) return result;
for(;;) {
if (q1->row_num < q2->row_num) {
if ((q1 = q1->next_row) == 0) {
return result;
}
} else if (q1->row_num > q2->row_num) {
if ((q2 = q2->next_row) == 0) {
return result;
}
} else {
(void) sm_col_insert(result, q1->row_num);
if ((q1 = q1->next_row) == 0) {
return result;
}
if ((q2 = q2->next_row) == 0) {
return result;
}
}
}
}

int sm_col_hash(sm_col *pcol, int modulus)
{
register int sum;
register sm_element *p;
sum = 0;
for(p = pcol->first_row; p != 0; p = p->next_row) {
sum = (sum*17 + p->row_num) % modulus;
}
return sum;
}

void
sm_col_remove_element(register sm_col *pcol, register sm_element *p)
{
{ if (p-> prev_row == 0) { pcol->first_row = p-> next_row; } else { p-> prev_row-> next_row = p-> next_row; } if (p-> next_row == 0) { pcol->last_row = p-> prev_row; } else { p-> next_row-> prev_row = p-> prev_row; } pcol->length--; };
((p) ? (free((char *) (p)), (p) = 0) : 0);
}
void
sm_col_print(FILE *fp, sm_col *pcol)
{
sm_element *p;
for(p = pcol->first_row; p != 0; p = p->next_row) {
(void) fprintf(fp, " %d", p->row_num);
}
}
