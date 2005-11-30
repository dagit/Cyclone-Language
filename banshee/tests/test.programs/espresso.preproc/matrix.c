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
extern sm_matrix *sm_alloc(void), *sm_alloc_size(int row, int col), *sm_dup(sm_matrix *A);
extern void sm_free(sm_matrix *A), sm_delrow(sm_matrix *A, int i), sm_delcol(sm_matrix *A, int i), sm_resize(register sm_matrix *A, int row, int col);
extern void sm_write(FILE *fp, sm_matrix *A), sm_print(FILE *fp, sm_matrix *A), sm_dump(sm_matrix *A, char *s, int max), sm_cleanup(void);
extern void sm_copy_row(register sm_matrix *dest, int dest_row, sm_row *prow), sm_copy_col(register sm_matrix *dest, int dest_col, sm_col *pcol);
extern void sm_remove(sm_matrix *A, int rownum, int colnum), sm_remove_element(register sm_matrix *A, register sm_element *p);
extern sm_element *sm_insert(register sm_matrix *A, register int row, register int col), *sm_find(sm_matrix *A, int rownum, int colnum);
extern sm_row *sm_longest_row(sm_matrix *A);
extern sm_col *sm_longest_col(sm_matrix *A);
extern int sm_read(FILE *fp, sm_matrix **A), sm_read_compressed(FILE *fp, sm_matrix **A);
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
extern void sm_col_remove_element();
sm_matrix *
sm_alloc(void)
{
register sm_matrix *A;
A = ((sm_matrix *) malloc(sizeof(sm_matrix) * ( 1)));
A->rows = ((sm_row * *) 0);
A->cols = ((sm_col * *) 0);
A->nrows = A->ncols = 0;
A->rows_size = A->cols_size = 0;
A->first_row = A->last_row = ((sm_row *) 0);
A->first_col = A->last_col = ((sm_col *) 0);
A->user_word = ((char *) 0);	return A;
}
sm_matrix *
sm_alloc_size(int row, int col)
{
register sm_matrix *A;
A = sm_alloc();
sm_resize(A, row, col);
return A;
}
void
sm_free(sm_matrix *A)
{
register sm_row *prow, *pnext_row;
register sm_col *pcol, *pnext_col;
for(prow = A->first_row; prow != 0; prow = pnext_row) {
pnext_row = prow->next_row;
sm_row_free(prow);
}
for(pcol = A->first_col; pcol != 0; pcol = pnext_col) {
pnext_col = pcol->next_col;
pcol->first_row = pcol->last_row = ((sm_element *) 0);
sm_col_free(pcol);
}
((A->rows) ? (free((char *) (A->rows)), (A->rows) = 0) : 0);
((A->cols) ? (free((char *) (A->cols)), (A->cols) = 0) : 0);
((A) ? (free((char *) (A)), (A) = 0) : 0);
}
sm_matrix *
sm_dup(sm_matrix *A)
{
register sm_row *prow;
register sm_element *p;
register sm_matrix *B;
B = sm_alloc();
if (A->last_row != 0) {
sm_resize(B, A->last_row->row_num, A->last_col->col_num);
for(prow = A->first_row; prow != 0; prow = prow->next_row) {
for(p = prow->first_col; p != 0; p = p->next_col) {
(void) sm_insert(B, p->row_num, p->col_num);
}
}
}
return B;
}
void sm_resize(register sm_matrix *A, int row, int col)
{
register int i, new_size;
if (row >= A->rows_size) {
new_size = ((A->rows_size*2) > ( row+1) ? (A->rows_size*2) : ( row+1));
A->rows = ( A->rows) ? ((sm_row * *) realloc((char *) A->rows, sizeof(sm_row *) * ( new_size))) : ((sm_row * *) malloc(sizeof(sm_row *) * ( new_size)));
for(i = A->rows_size; i < new_size; i++) {
A->rows[i] = ((sm_row *) 0);
}
A->rows_size = new_size;
}
if (col >= A->cols_size) {
new_size = ((A->cols_size*2) > ( col+1) ? (A->cols_size*2) : ( col+1));
A->cols = ( A->cols) ? ((sm_col * *) realloc((char *) A->cols, sizeof(sm_col *) * ( new_size))) : ((sm_col * *) malloc(sizeof(sm_col *) * ( new_size)));
for(i = A->cols_size; i < new_size; i++) {
A->cols[i] = ((sm_col *) 0);
}
A->cols_size = new_size;
}
}
sm_element *
sm_insert(register sm_matrix *A, register int row, register int col)
{
register sm_row *prow;
register sm_col *pcol;
register sm_element *element;
sm_element *save_element;
if (row >= A->rows_size || col >= A->cols_size) {
sm_resize(A, row, col);
}
prow = A->rows[row];
if (prow == ((sm_row *) 0)) {
prow = A->rows[row] = sm_row_alloc();
prow->row_num = row;
if ( A->last_row == 0) { prow-> row_num = row; A->first_row = prow; A->last_row = prow; prow-> next_row = 0; prow-> prev_row = 0; A->nrows++; } else if ( A->last_row-> row_num < row) { prow-> row_num = row; A->last_row-> next_row = prow; prow-> prev_row = A->last_row; A->last_row = prow; prow-> next_row = 0; A->nrows++; } else if ( A->first_row-> row_num > row) { prow-> row_num = row; A->first_row-> prev_row = prow; prow-> next_row = A->first_row; A->first_row = prow; prow-> prev_row = 0; A->nrows++; } else { sm_row *p; for(p = A->first_row; p-> row_num < row; p = p-> next_row) ; if (p-> row_num > row) { prow-> row_num = row; p = p-> prev_row; p-> next_row-> prev_row = prow; prow-> next_row = p-> next_row; p-> next_row = prow; prow-> prev_row = p; A->nrows++; } else { prow = p; } };
}
pcol = A->cols[col];
if (pcol == ((sm_col *) 0)) {
pcol = A->cols[col] = sm_col_alloc();
pcol->col_num = col;
if ( A->last_col == 0) { pcol-> col_num = col; A->first_col = pcol; A->last_col = pcol; pcol-> next_col = 0; pcol-> prev_col = 0; A->ncols++; } else if ( A->last_col-> col_num < col) { pcol-> col_num = col; A->last_col-> next_col = pcol; pcol-> prev_col = A->last_col; A->last_col = pcol; pcol-> next_col = 0; A->ncols++; } else if ( A->first_col-> col_num > col) { pcol-> col_num = col; A->first_col-> prev_col = pcol; pcol-> next_col = A->first_col; A->first_col = pcol; pcol-> prev_col = 0; A->ncols++; } else { sm_col *p; for(p = A->first_col; p-> col_num < col; p = p-> next_col) ; if (p-> col_num > col) { pcol-> col_num = col; p = p-> prev_col; p-> next_col-> prev_col = pcol; pcol-> next_col = p-> next_col; p-> next_col = pcol; pcol-> prev_col = p; A->ncols++; } else { pcol = p; } };
}
element = ((sm_element *) malloc(sizeof(sm_element) * ( 1))); element->user_word = ((char *) 0);;
save_element = element;
if ( prow->last_col == 0) { element-> col_num = col; prow->first_col = element; prow->last_col = element; element-> next_col = 0; element-> prev_col = 0; prow->length++; } else if ( prow->last_col-> col_num < col) { element-> col_num = col; prow->last_col-> next_col = element; element-> prev_col = prow->last_col; prow->last_col = element; element-> next_col = 0; prow->length++; } else if ( prow->first_col-> col_num > col) { element-> col_num = col; prow->first_col-> prev_col = element; element-> next_col = prow->first_col; prow->first_col = element; element-> prev_col = 0; prow->length++; } else { sm_element *p; for(p = prow->first_col; p-> col_num < col; p = p-> next_col) ; if (p-> col_num > col) { element-> col_num = col; p = p-> prev_col; p-> next_col-> prev_col = element; element-> next_col = p-> next_col; p-> next_col = element; element-> prev_col = p; prow->length++; } else { element = p; } };
if (element == save_element) {
if ( pcol->last_row == 0) { element-> row_num = row; pcol->first_row = element; pcol->last_row = element; element-> next_row = 0; element-> prev_row = 0; pcol->length++; } else if ( pcol->last_row-> row_num < row) { element-> row_num = row; pcol->last_row-> next_row = element; element-> prev_row = pcol->last_row; pcol->last_row = element; element-> next_row = 0; pcol->length++; } else if ( pcol->first_row-> row_num > row) { element-> row_num = row; pcol->first_row-> prev_row = element; element-> next_row = pcol->first_row; pcol->first_row = element; element-> prev_row = 0; pcol->length++; } else { sm_element *p; for(p = pcol->first_row; p-> row_num < row; p = p-> next_row) ; if (p-> row_num > row) { element-> row_num = row; p = p-> prev_row; p-> next_row-> prev_row = element; element-> next_row = p-> next_row; p-> next_row = element; element-> prev_row = p; pcol->length++; } else { element = p; } };
} else {
((save_element) ? (free((char *) (save_element)), (save_element) = 0) : 0);
}
return element;
}
sm_element *
sm_find(sm_matrix *A, int rownum, int colnum)
{
sm_row *prow;
sm_col *pcol;
prow = ((( rownum) >= 0 && ( rownum) < (A)->rows_size) ? (A)->rows[ rownum] : (sm_row *) 0);
if (prow == ((sm_row *) 0)) {
return ((sm_element *) 0);
} else {
pcol = ((( colnum) >= 0 && ( colnum) < (A)->cols_size) ? (A)->cols[ colnum] : (sm_col *) 0);
if (pcol == ((sm_col *) 0)) {
return ((sm_element *) 0);
}
if (prow->length < pcol->length) {
return sm_row_find(prow, colnum);
} else {
return sm_col_find(pcol, rownum);
}
}
}
void
sm_remove(sm_matrix *A, int rownum, int colnum)
{
sm_remove_element(A, sm_find(A, rownum, colnum));
}
void
sm_remove_element(register sm_matrix *A, register sm_element *p)
{
register sm_row *prow;
register sm_col *pcol;
if (p == 0) return;
prow = ((( p->row_num) >= 0 && ( p->row_num) < (A)->rows_size) ? (A)->rows[ p->row_num] : (sm_row *) 0);
{ if (p-> prev_col == 0) { prow->first_col = p-> next_col; } else { p-> prev_col-> next_col = p-> next_col; } if (p-> next_col == 0) { prow->last_col = p-> prev_col; } else { p-> next_col-> prev_col = p-> prev_col; } prow->length--; };
if (prow->first_col == ((sm_element *) 0)) {
sm_delrow(A, p->row_num);
}
pcol = ((( p->col_num) >= 0 && ( p->col_num) < (A)->cols_size) ? (A)->cols[ p->col_num] : (sm_col *) 0);
{ if (p-> prev_row == 0) { pcol->first_row = p-> next_row; } else { p-> prev_row-> next_row = p-> next_row; } if (p-> next_row == 0) { pcol->last_row = p-> prev_row; } else { p-> next_row-> prev_row = p-> prev_row; } pcol->length--; };
if (pcol->first_row == ((sm_element *) 0)) {
sm_delcol(A, p->col_num);
}
((p) ? (free((char *) (p)), (p) = 0) : 0);
}

void sm_delrow(sm_matrix *A, int i)
{
register sm_element *p, *pnext;
sm_col *pcol;
sm_row *prow;
prow = ((( i) >= 0 && ( i) < (A)->rows_size) ? (A)->rows[ i] : (sm_row *) 0);
if (prow != ((sm_row *) 0)) {
for(p = prow->first_col; p != 0; p = pnext) {
pnext = p->next_col;
pcol = ((( p->col_num) >= 0 && ( p->col_num) < (A)->cols_size) ? (A)->cols[ p->col_num] : (sm_col *) 0);
sm_col_remove_element(pcol, p);
if (pcol->first_row == ((sm_element *) 0)) {
sm_delcol(A, pcol->col_num);
}
}
A->rows[i] = ((sm_row *) 0);
{ if (prow-> prev_row == 0) { A->first_row = prow-> next_row; } else { prow-> prev_row-> next_row = prow-> next_row; } if (prow-> next_row == 0) { A->last_row = prow-> prev_row; } else { prow-> next_row-> prev_row = prow-> prev_row; } A->nrows--; };
prow->first_col = prow->last_col = ((sm_element *) 0);
sm_row_free(prow);
}
}

void sm_delcol(sm_matrix *A, int i)
{
register sm_element *p, *pnext;
sm_row *prow;
sm_col *pcol;
pcol = ((( i) >= 0 && ( i) < (A)->cols_size) ? (A)->cols[ i] : (sm_col *) 0);
if (pcol != ((sm_col *) 0)) {
for(p = pcol->first_row; p != 0; p = pnext) {
pnext = p->next_row;
prow = ((( p->row_num) >= 0 && ( p->row_num) < (A)->rows_size) ? (A)->rows[ p->row_num] : (sm_row *) 0);
sm_row_remove_element(prow, p);
if (prow->first_col == ((sm_element *) 0)) {
sm_delrow(A, prow->row_num);
}
}
A->cols[i] = ((sm_col *) 0);
{ if (pcol-> prev_col == 0) { A->first_col = pcol-> next_col; } else { pcol-> prev_col-> next_col = pcol-> next_col; } if (pcol-> next_col == 0) { A->last_col = pcol-> prev_col; } else { pcol-> next_col-> prev_col = pcol-> prev_col; } A->ncols--; };
pcol->first_row = pcol->last_row = ((sm_element *) 0);
sm_col_free(pcol);
}
}

void
sm_copy_row(register sm_matrix *dest, int dest_row, sm_row *prow)
{
register sm_element *p;
for(p = prow->first_col; p != 0; p = p->next_col) {
(void) sm_insert(dest, dest_row, p->col_num);
}
}
void
sm_copy_col(register sm_matrix *dest, int dest_col, sm_col *pcol)
{
register sm_element *p;
for(p = pcol->first_row; p != 0; p = p->next_row) {
(void) sm_insert(dest, dest_col, p->row_num);
}
}
sm_row *
sm_longest_row(sm_matrix *A)
{
register sm_row *large_row, *prow;
register int max_length;
max_length = 0;
large_row = ((sm_row *) 0);
for(prow = A->first_row; prow != 0; prow = prow->next_row) {
if (prow->length > max_length) {
max_length = prow->length;
large_row = prow;
}
}
return large_row;
}
sm_col *
sm_longest_col(sm_matrix *A)
{
register sm_col *large_col, *pcol;
register int max_length;
max_length = 0;
large_col = ((sm_col *) 0);
for(pcol = A->first_col; pcol != 0; pcol = pcol->next_col) {
if (pcol->length > max_length) {
max_length = pcol->length;
large_col = pcol;
}
}
return large_col;
}

int
sm_num_elements(sm_matrix *A)
{
register sm_row *prow;
register int num;
num = 0;
for( prow = A->first_row; prow != 0; prow = prow->next_row) {
num += prow->length;
}
return num;
}

int sm_read(FILE *fp, sm_matrix **A)
{
int i, j, err;
*A = sm_alloc();
while (! (((fp)->_flag&020)!=0)) {
err = fscanf(fp, "%d %d", &i, &j);
if (err == (-1)) {
return 1;
} else if (err != 2) {
return 0;
}
(void) sm_insert(*A, i, j);
}
return 1;
}
int sm_read_compressed(FILE *fp, sm_matrix **A)
{
int i, j, k, nrows, ncols;
unsigned long x;
*A = sm_alloc();
if (fscanf(fp, "%d %d", &nrows, &ncols) != 2) {
return 0;
}
sm_resize(*A, nrows, ncols);
for(i = 0; i < nrows; i++) {
if (fscanf(fp, "%lx", &x) != 1) {
return 0;
}
for(j = 0; j < ncols; j += 32) {
if (fscanf(fp, "%lx", &x) != 1) {
return 0;
}
for(k = j; x != 0; x >>= 1, k++) {
if (x & 1) {
(void) sm_insert(*A, i, k);
}
}
}
}
return 1;
}
void sm_write(FILE *fp, sm_matrix *A)
{
register sm_row *prow;
register sm_element *p;
for(prow = A->first_row; prow != 0; prow = prow->next_row) {
for(p = prow->first_col; p != 0; p = p->next_col) {
(void) fprintf(fp, "%d %d\n", p->row_num, p->col_num);
}
}
}

void sm_print(FILE *fp, sm_matrix *A)
{
register sm_row *prow;
register sm_col *pcol;
int c;
if (A->last_col->col_num >= 100) {
(void) fprintf(fp, "    ");
for(pcol = A->first_col; pcol != 0; pcol = pcol->next_col) {
(void) fprintf(fp, "%d", (pcol->col_num / 100)%10);
}
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
}
if (A->last_col->col_num >= 10) {
(void) fprintf(fp, "    ");
for(pcol = A->first_col; pcol != 0; pcol = pcol->next_col) {
(void) fprintf(fp, "%d", (pcol->col_num / 10)%10);
}
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
}
(void) fprintf(fp, "    ");
for(pcol = A->first_col; pcol != 0; pcol = pcol->next_col) {
(void) fprintf(fp, "%d", pcol->col_num % 10);
}
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
(void) fprintf(fp, "    ");
for(pcol = A->first_col; pcol != 0; pcol = pcol->next_col) {
(void) fprintf(fp, "-");
}
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
for(prow = A->first_row; prow != 0; prow = prow->next_row) {
(void) fprintf(fp, "%3d:", prow->row_num, prow->length);
for(pcol = A->first_col; pcol != 0; pcol = pcol->next_col) {
c = sm_row_find(prow, pcol->col_num) ? '1' : '.';
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)(c)) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)(c)) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)(c), fp)));
}
(--( fp)->_cnt >= 0 ? (int)(*( fp)->_ptr++ = (unsigned char)('\n')) : ((( fp)->_flag & 0200) && -( fp)->_cnt < ( fp)->_bufsiz ? ((*( fp)->_ptr = (unsigned char)('\n')) != '\n' ? (int)(*( fp)->_ptr++) : _flsbuf(*(unsigned char *)( fp)->_ptr, fp)) : _flsbuf((unsigned char)('\n'), fp)));
}
}
void sm_dump(sm_matrix *A, char *s, int max)
{
struct _iobuf *fp = (&_iob[1]);
(void) fprintf(fp, "%s %d rows by %d cols\n", s, A->nrows, A->ncols);
if (A->nrows < max) {
sm_print(fp, A);
}
}

void
sm_cleanup(void)
{
}
