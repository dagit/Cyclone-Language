#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef ___sigset_t_def_
#define ___sigset_t_def_
typedef struct {unsigned long __val[1024 / (8 * sizeof(unsigned long))];} __sigset_t;
#endif
#ifndef ___jmp_buf_def_
#define ___jmp_buf_def_
typedef int __jmp_buf[6];
#endif
#ifndef ___jmp_buf_tag_def_
#define ___jmp_buf_tag_def_
struct __jmp_buf_tag{
  __jmp_buf __jmpbuf;
  int __mask_was_saved;
  __sigset_t __saved_mask;
};
#endif
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef struct __jmp_buf_tag jmp_buf[1];
#endif
extern int setjmp(jmp_buf);
#endif
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file
   lib/runtime_cyc.c
*/
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Tagged arrays */
struct _dyneither_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Discriminated Unions */
struct _xtunion_struct { char *tag; };

/* Need one of these per thread (we don't have threads)
   The runtime maintains a stack that contains either _handler_cons
   structs or _RegionHandle structs.  The tag is 0 for a handler_cons
   and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; /* 0 for an exception handler, 1 for a region handle */
  struct _RuntimeStack *next;
};

/* Regions */
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];  /*FJS: used to be size 0, but that's forbidden in ansi c*/
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);
extern struct _RegionHandle *_open_dynregion(struct _DynRegionFrame *f,
                                             struct _DynRegionHandle *h);
extern void   _pop_dynregion();

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();

#ifndef _throw
extern int _throw_null();
extern int _throw_arraybounds();
extern int _throw_badalloc();
extern int _throw(void* e);
#endif

extern struct _xtunion_struct *_exn_thrown;

/* Built-in Exceptions */
struct Cyc_Null_Exception_struct { char *tag; };
struct Cyc_Array_bounds_struct { char *tag; };
struct Cyc_Match_Exception_struct { char *tag; };
struct Cyc_Bad_alloc_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

/* Built-in Run-time Checks and company */
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#define _INLINE_FUNCTIONS
#else
#define _INLINE inline
#endif

#ifdef VC_C
#define _CYC_U_LONG_LONG_T __int64
#else
#ifdef GCC_C
#define _CYC_U_LONG_LONG_T unsigned long long
#else
#define _CYC_U_LONG_LONG_T unsigned long long
#endif
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE void *
_check_null(void *ptr) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null();
  return _check_null_temp;
}
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE char *
_check_known_subscript_null(void *ptr, unsigned bound, unsigned elt_sz, unsigned index) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null();
  if (_cks_index >= _cks_bound) _throw_arraybounds();
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_check_known_subscript_notnull(unsigned bound,unsigned index) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); 
  return _cksnn_index;
}
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif
#endif

/* Add i to zero-terminated pointer x.  Checks for x being null and
   ensures that x[0..i-1] are not 0. */
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus_char(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_short(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_int(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_float(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_double(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char(char *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short(short *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int(int *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float(float *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double(double *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble(long double *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar(void **orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
#endif


/* Calculates the number of elements in a zero-terminated, thin array.
   If non-null, the array is guaranteed to have orig_offset elements. */
static _INLINE int
_get_zero_arr_size_char(const char *orig_x, unsigned int orig_offset) {
  const char *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_short(const short *orig_x, unsigned int orig_offset) {
  const short *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_int(const int *orig_x, unsigned int orig_offset) {
  const int *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_float(const float *orig_x, unsigned int orig_offset) {
  const float *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_double(const double *orig_x, unsigned int orig_offset) {
  const double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_longdouble(const long double *orig_x, unsigned int orig_offset) {
  const long double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_voidstar(const void **orig_x, unsigned int orig_offset) {
  const void **_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}


/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus. */
/*#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })
  */
static _INLINE void 
_zero_arr_inplace_plus_char(char *x, int orig_i) {
  char **_zap_x = &x;
  *_zap_x = _zero_arr_plus_char(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_short(short *x, int orig_i) {
  short **_zap_x = &x;
  *_zap_x = _zero_arr_plus_short(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_int(int *x, int orig_i) {
  int **_zap_x = &x;
  *_zap_x = _zero_arr_plus_int(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_float(float *x, int orig_i) {
  float **_zap_x = &x;
  *_zap_x = _zero_arr_plus_float(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_double(double *x, int orig_i) {
  double **_zap_x = &x;
  *_zap_x = _zero_arr_plus_double(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_longdouble(long double *x, int orig_i) {
  long double **_zap_x = &x;
  *_zap_x = _zero_arr_plus_longdouble(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_voidstar(void **x, int orig_i) {
  void ***_zap_x = &x;
  *_zap_x = _zero_arr_plus_voidstar(*_zap_x,1,orig_i);
}




/* Does in-place increment of a zero-terminated pointer (e.g., x++).
   Note that this expands to call _zero_arr_plus. */
/*#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })*/
  
static _INLINE char *
_zero_arr_inplace_plus_post_char(char *x, int orig_i){
  char ** _zap_x = &x;
  char * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_char(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE short *
_zero_arr_inplace_plus_post_short(short *x, int orig_i){
  short **_zap_x = &x;
  short * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_short(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE int *
_zero_arr_inplace_plus_post_int(int *x, int orig_i){
  int **_zap_x = &x;
  int * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_int(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE float *
_zero_arr_inplace_plus_post_float(float *x, int orig_i){
  float **_zap_x = &x;
  float * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_float(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE double *
_zero_arr_inplace_plus_post_double(double *x, int orig_i){
  double **_zap_x = &x;
  double * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_double(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble(long double *x, int orig_i){
  long double **_zap_x = &x;
  long double * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_longdouble(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar(void **x, int orig_i){
  void ***_zap_x = &x;
  void ** _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_voidstar(_zap_res,1,orig_i);
  return _zap_res;
}



/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_tag_dyneither(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr *
_init_dyneither_ptr(struct _dyneither_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((unsigned char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_dyneither_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_untag_dyneither_ptr(struct _dyneither_ptr arr, 
                     unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_get_dyneither_size(struct _dyneither_ptr arr,unsigned elt_sz) {
  struct _dyneither_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_dyneither_size(arr,elt_sz) \
  ({struct _dyneither_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,
                            int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus_post(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  struct _dyneither_ptr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
static struct 
_dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Allocation */
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static _INLINE void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
  _CYC_U_LONG_LONG_T whole_ans = 
    ((_CYC_U_LONG_LONG_T)x)*((_CYC_U_LONG_LONG_T)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,char *file,int lineno);
extern void* _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                     char *file,int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						char *file,int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
#endif
#endif

/* the next two routines swap [x] and [y]; not thread safe! */
static _INLINE void _swap_word(void *x, void *y) {
  unsigned long *lx = (unsigned long *)x, *ly = (unsigned long *)y, tmp;
  tmp = *lx;
  *lx = *ly;
  *ly = tmp;
}
static _INLINE void _swap_dyneither(struct _dyneither_ptr *x, 
				   struct _dyneither_ptr *y) {
  struct _dyneither_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(
void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0
Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x);unsigned int Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);int Cyc_zstrcmp(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned int n);
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct
Cyc_Position_Segment*,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract
 = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register
 = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int
real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz
 = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz
 = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind  = 5,
Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
 = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;enum Cyc_Absyn_Kind f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;enum Cyc_Absyn_Kind f2;};struct Cyc_Absyn_Tvar{struct
_dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct
Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;
};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct
Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct
Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;
struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3
val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};
union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct
_union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,
Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct
Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};
struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
int tag;};struct Cyc_Absyn_Const_att_struct{int tag;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{int tag;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_struct{int tag;};
struct Cyc_Absyn_Shared_att_struct{int tag;};struct Cyc_Absyn_Unused_att_struct{int
tag;};struct Cyc_Absyn_Weak_att_struct{int tag;};struct Cyc_Absyn_Dllimport_att_struct{
int tag;};struct Cyc_Absyn_Dllexport_att_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{
int tag;};struct Cyc_Absyn_Constructor_att_struct{int tag;};struct Cyc_Absyn_Destructor_att_struct{
int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{int tag;};struct Cyc_Absyn_Format_att_struct{
int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{int tag;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};extern struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_struct Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct
Cyc_Absyn_Fastcall_att_val;extern struct Cyc_Absyn_Noreturn_att_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_struct Cyc_Absyn_Const_att_val;extern struct Cyc_Absyn_Packed_att_struct
Cyc_Absyn_Packed_att_val;extern struct Cyc_Absyn_Shared_att_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val;extern struct Cyc_Absyn_Weak_att_struct
Cyc_Absyn_Weak_att_val;extern struct Cyc_Absyn_Dllimport_att_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_struct Cyc_Absyn_Dllexport_att_val;extern
struct Cyc_Absyn_No_instrument_function_att_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_struct Cyc_Absyn_Constructor_att_val;extern
struct Cyc_Absyn_Destructor_att_struct Cyc_Absyn_Destructor_att_val;extern struct
Cyc_Absyn_No_check_memory_usage_att_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_struct{
int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};struct
_union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct
_union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times
 = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,
Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte
 = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor
 = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,
Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc
 = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,
Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion
 = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;
};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct
Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{
int tag;struct Cyc_List_List*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};
struct Cyc_Absyn_Swap_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{int tag;struct Cyc_Core_Opt*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Tagcheck_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;}
;struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_Absyn_Reference_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct
Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};
struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*
r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum Cyc_Absyn_Scope
sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{
struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};
struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*
fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;struct
Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*
f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};extern struct Cyc_Absyn_Porton_d_struct
Cyc_Absyn_Porton_d_val;extern struct Cyc_Absyn_Portoff_d_struct Cyc_Absyn_Portoff_d_val;
struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(
void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
enum Cyc_Absyn_Primop,struct Cyc_List_List*es,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple1*
n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct _tuple10{void*f1;void*f2;};struct
_tuple10*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct
_tuple10*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*
Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(void*t);extern struct Cyc_Core_Opt
Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ik;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(
enum Cyc_Absyn_Kind k);void*Cyc_Tcutil_kind_to_bound(enum Cyc_Absyn_Kind k);struct
Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Lex_register_typedef(struct _tuple1*s);void Cyc_Lex_enter_namespace(
struct _dyneither_ptr*);void Cyc_Lex_leave_namespace();void Cyc_Lex_enter_using(
struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=0;struct Cyc_Parse_Signed_spec_struct{
int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Unsigned_spec_struct{int
tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Short_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Long_spec_struct{int tag;struct
Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{int tag;void*f1;struct
Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{int tag;struct Cyc_Absyn_Decl*
f1;};enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc
 = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc
 = 5,Cyc_Parse_Abstract_sc  = 6};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple11{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple11 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple12{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple12
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Parse_err((struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)({
struct Cyc_Position_Exit_struct*_tmp0=_cycalloc_atomic(sizeof(*_tmp0));_tmp0[0]=({
struct Cyc_Position_Exit_struct _tmp1;_tmp1.tag=Cyc_Position_Exit;_tmp1;});_tmp0;}));}
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){({
struct Cyc_String_pa_struct _tmp5;_tmp5.tag=0;_tmp5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg);({struct Cyc_String_pa_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg));({
void*_tmp2[2]={& _tmp4,& _tmp5};Cyc_fprintf(Cyc_stderr,({const char*_tmp3="%s: Warning: Cyclone does not yet support %s\n";
_tag_dyneither(_tmp3,sizeof(char),46);}),_tag_dyneither(_tmp2,sizeof(void*),2));});});});
return;}struct _tuple13{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple13 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs){union Cyc_Absyn_Constraint*zeroterm=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();for(0;pqs != 0;pqs=pqs->tl){
void*_tmp6=(void*)pqs->hd;struct Cyc_Absyn_Exp*_tmpE;void*_tmp10;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_struct*
_tmp7=(struct Cyc_Absyn_Zeroterm_ptrqual_struct*)_tmp6;if(_tmp7->tag != 4)goto _LL3;}
_LL2: zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_struct*
_tmp8=(struct Cyc_Absyn_Nozeroterm_ptrqual_struct*)_tmp6;if(_tmp8->tag != 5)goto
_LL5;}_LL4: zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_struct*
_tmp9=(struct Cyc_Absyn_Nullable_ptrqual_struct*)_tmp6;if(_tmp9->tag != 7)goto _LL7;}
_LL6: nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_struct*
_tmpA=(struct Cyc_Absyn_Notnull_ptrqual_struct*)_tmp6;if(_tmpA->tag != 6)goto _LL9;}
_LL8: nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_struct*
_tmpB=(struct Cyc_Absyn_Fat_ptrqual_struct*)_tmp6;if(_tmpB->tag != 3)goto _LLB;}
_LLA: bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpC=(struct Cyc_Absyn_Thin_ptrqual_struct*)_tmp6;if(_tmpC->tag != 2)goto _LLD;}
_LLC: bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpD=(struct Cyc_Absyn_Numelts_ptrqual_struct*)_tmp6;if(_tmpD->tag != 0)goto _LLF;
else{_tmpE=_tmpD->f1;}}_LLE: bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp12;_tmp12.tag=1;_tmp12.f1=_tmpE;_tmp12;});
_tmp11;}));goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*_tmpF=(struct
Cyc_Absyn_Region_ptrqual_struct*)_tmp6;if(_tmpF->tag != 1)goto _LL0;else{_tmp10=(
void*)_tmpF->f1;}}_LL10: rgn=_tmp10;goto _LL0;_LL0:;}return({struct _tuple13 _tmp13;
_tmp13.f1=nullable;_tmp13.f2=bound;_tmp13.f3=zeroterm;_tmp13.f4=rgn;_tmp13;});}
static int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum(){return({
struct _tuple1*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->f1=Cyc_Absyn_Rel_n(0);
_tmp14->f2=({struct _dyneither_ptr*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=(
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp18;_tmp18.tag=1;_tmp18.f1=(
unsigned long)Cyc_Parse_enum_counter ++;({void*_tmp16[1]={& _tmp18};Cyc_aprintf(({
const char*_tmp17="__anonymous_enum_%d__";_tag_dyneither(_tmp17,sizeof(char),22);}),
_tag_dyneither(_tmp16,sizeof(void*),1));});});_tmp15;});_tmp14;});}struct
_tuple14{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;
struct Cyc_List_List*f5;};struct _tuple15{struct _tuple14*f1;struct Cyc_Absyn_Exp*f2;
};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple15*field_info){struct _tuple15 _tmp1A;struct _tuple14*_tmp1B;struct
_tuple14 _tmp1C;struct _tuple1*_tmp1D;struct Cyc_Absyn_Tqual _tmp1E;void*_tmp1F;
struct Cyc_List_List*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp22;
struct _tuple15*_tmp19=field_info;_tmp1A=*_tmp19;_tmp1B=_tmp1A.f1;_tmp1C=*_tmp1B;
_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;_tmp1F=_tmp1C.f3;_tmp20=_tmp1C.f4;_tmp21=
_tmp1C.f5;_tmp22=_tmp1A.f2;if(_tmp20 != 0)Cyc_Parse_err(({const char*_tmp23="bad type params in struct field";
_tag_dyneither(_tmp23,sizeof(char),32);}),loc);if(Cyc_Absyn_is_qvar_qualified(
_tmp1D))Cyc_Parse_err(({const char*_tmp24="struct or union field cannot be qualified with a namespace";
_tag_dyneither(_tmp24,sizeof(char),59);}),loc);return({struct Cyc_Absyn_Aggrfield*
_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->name=(*_tmp1D).f2;_tmp25->tq=_tmp1E;
_tmp25->type=_tmp1F;_tmp25->width=_tmp22;_tmp25->attributes=_tmp21;_tmp25;});}
static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){return(
void*)({struct Cyc_Parse_Type_spec_struct*_tmp26=_cycalloc(sizeof(*_tmp26));
_tmp26[0]=({struct Cyc_Parse_Type_spec_struct _tmp27;_tmp27.tag=4;_tmp27.f1=(void*)
t;_tmp27.f2=loc;_tmp27;});_tmp26;});}static void*Cyc_Parse_array2ptr(void*t,int
argposn){void*_tmp28=t;struct Cyc_Absyn_ArrayInfo _tmp2A;void*_tmp2B;struct Cyc_Absyn_Tqual
_tmp2C;struct Cyc_Absyn_Exp*_tmp2D;union Cyc_Absyn_Constraint*_tmp2E;struct Cyc_Position_Segment*
_tmp2F;_LL12: {struct Cyc_Absyn_ArrayType_struct*_tmp29=(struct Cyc_Absyn_ArrayType_struct*)
_tmp28;if(_tmp29->tag != 9)goto _LL14;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp2A.elt_type;
_tmp2C=_tmp2A.tq;_tmp2D=_tmp2A.num_elts;_tmp2E=_tmp2A.zero_term;_tmp2F=_tmp2A.zt_loc;}}
_LL13: return Cyc_Absyn_starb_typ(_tmp2B,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp2C,_tmp2D == 0?(void*)& Cyc_Absyn_DynEither_b_val:(
void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp30=_cycalloc(sizeof(*_tmp30));
_tmp30[0]=({struct Cyc_Absyn_Upper_b_struct _tmp31;_tmp31.tag=1;_tmp31.f1=(struct
Cyc_Absyn_Exp*)_tmp2D;_tmp31;});_tmp30;})),_tmp2E);_LL14:;_LL15: return t;_LL11:;}
struct _tuple16{struct _dyneither_ptr*f1;void*f2;};static struct Cyc_List_List*Cyc_Parse_get_arg_tags(
struct Cyc_List_List*x){struct Cyc_List_List*_tmp32=0;for(0;x != 0;x=x->tl){struct
_tuple8*_tmp33=(struct _tuple8*)x->hd;struct _tuple8 _tmp34;struct Cyc_Core_Opt*
_tmp35;struct Cyc_Core_Opt _tmp36;struct _dyneither_ptr*_tmp37;void*_tmp38;void*
_tmp3A;struct _tuple8 _tmp3B;struct Cyc_Core_Opt*_tmp3C;struct Cyc_Core_Opt _tmp3D;
struct _dyneither_ptr*_tmp3E;void*_tmp3F;void*_tmp41;struct Cyc_Core_Opt*_tmp43;
struct Cyc_Core_Opt**_tmp44;_LL17: _tmp34=*_tmp33;_tmp35=_tmp34.f1;if(_tmp35 == 0)
goto _LL19;_tmp36=*_tmp35;_tmp37=(struct _dyneither_ptr*)_tmp36.v;_tmp38=_tmp34.f3;{
struct Cyc_Absyn_TagType_struct*_tmp39=(struct Cyc_Absyn_TagType_struct*)_tmp38;
if(_tmp39->tag != 20)goto _LL19;else{_tmp3A=(void*)_tmp39->f1;}};_LL18:{void*
_tmp45=_tmp3A;struct Cyc_Core_Opt*_tmp47;struct Cyc_Core_Opt**_tmp48;_LL1E: {
struct Cyc_Absyn_Evar_struct*_tmp46=(struct Cyc_Absyn_Evar_struct*)_tmp45;if(
_tmp46->tag != 1)goto _LL20;else{_tmp47=_tmp46->f2;_tmp48=(struct Cyc_Core_Opt**)&
_tmp46->f2;}}_LL1F: {struct _dyneither_ptr*nm=(struct _dyneither_ptr*)({struct
_dyneither_ptr*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp37);({void*_tmp50[1]={& _tmp52};Cyc_aprintf(({const char*_tmp51="`%s";
_tag_dyneither(_tmp51,sizeof(char),4);}),_tag_dyneither(_tmp50,sizeof(void*),1));});});
_tmp4F;});*_tmp48=({struct Cyc_Core_Opt*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->v=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[
0]=({struct Cyc_Absyn_VarType_struct _tmp4B;_tmp4B.tag=2;_tmp4B.f1=({struct Cyc_Absyn_Tvar*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->name=nm;_tmp4C->identity=- 1;_tmp4C->kind=(
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp4D=_cycalloc_atomic(sizeof(*_tmp4D));
_tmp4D[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp4E;_tmp4E.tag=0;_tmp4E.f1=Cyc_Absyn_IntKind;
_tmp4E;});_tmp4D;});_tmp4C;});_tmp4B;});_tmp4A;});_tmp49;});goto _LL1D;}_LL20:;
_LL21: goto _LL1D;_LL1D:;}_tmp32=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*
_tmp53));_tmp53->hd=({struct _tuple16*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->f1=
_tmp37;_tmp54->f2=_tmp3A;_tmp54;});_tmp53->tl=_tmp32;_tmp53;});goto _LL16;_LL19:
_tmp3B=*_tmp33;_tmp3C=_tmp3B.f1;if(_tmp3C == 0)goto _LL1B;_tmp3D=*_tmp3C;_tmp3E=(
struct _dyneither_ptr*)_tmp3D.v;_tmp3F=_tmp3B.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp40=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp3F;if(_tmp40->tag != 16)goto
_LL1B;else{_tmp41=(void*)_tmp40->f1;{struct Cyc_Absyn_Evar_struct*_tmp42=(struct
Cyc_Absyn_Evar_struct*)_tmp41;if(_tmp42->tag != 1)goto _LL1B;else{_tmp43=_tmp42->f2;
_tmp44=(struct Cyc_Core_Opt**)& _tmp42->f2;}};}};_LL1A: {struct _dyneither_ptr*nm=(
struct _dyneither_ptr*)({struct _dyneither_ptr*_tmp5B=_cycalloc(sizeof(*_tmp5B));
_tmp5B[0]=({struct Cyc_String_pa_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3E);({void*_tmp5C[1]={& _tmp5E};Cyc_aprintf(({
const char*_tmp5D="`%s";_tag_dyneither(_tmp5D,sizeof(char),4);}),_tag_dyneither(
_tmp5C,sizeof(void*),1));});});_tmp5B;});*_tmp44=({struct Cyc_Core_Opt*_tmp55=
_cycalloc(sizeof(*_tmp55));_tmp55->v=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56[0]=({struct Cyc_Absyn_VarType_struct
_tmp57;_tmp57.tag=2;_tmp57.f1=({struct Cyc_Absyn_Tvar*_tmp58=_cycalloc(sizeof(*
_tmp58));_tmp58->name=nm;_tmp58->identity=- 1;_tmp58->kind=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp59=_cycalloc_atomic(sizeof(*_tmp59));_tmp59[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5A;_tmp5A.tag=0;_tmp5A.f1=Cyc_Absyn_RgnKind;_tmp5A;});_tmp59;});_tmp58;});
_tmp57;});_tmp56;});_tmp55;});goto _LL16;}_LL1B:;_LL1C: goto _LL16;_LL16:;}return
_tmp32;}static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*
x){struct Cyc_List_List*_tmp5F=0;for(0;x != 0;x=x->tl){void*_tmp60=((struct Cyc_Absyn_Aggrfield*)
x->hd)->type;void*_tmp62;_LL23: {struct Cyc_Absyn_TagType_struct*_tmp61=(struct
Cyc_Absyn_TagType_struct*)_tmp60;if(_tmp61->tag != 20)goto _LL25;else{_tmp62=(void*)
_tmp61->f1;}}_LL24: _tmp5F=({struct Cyc_List_List*_tmp63=_cycalloc(sizeof(*_tmp63));
_tmp63->hd=({struct _tuple16*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->f1=((
struct Cyc_Absyn_Aggrfield*)x->hd)->name;_tmp64->f2=_tmp62;_tmp64;});_tmp63->tl=
_tmp5F;_tmp63;});goto _LL22;_LL25:;_LL26: goto _LL22;_LL22:;}return _tmp5F;}static
struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,
struct Cyc_Absyn_Exp*e){{void*_tmp65=e->r;struct _tuple1*_tmp67;struct _tuple1
_tmp68;union Cyc_Absyn_Nmspace _tmp69;struct Cyc_List_List*_tmp6A;struct
_dyneither_ptr*_tmp6B;struct _tuple1*_tmp6D;struct _tuple1 _tmp6E;union Cyc_Absyn_Nmspace
_tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr*_tmp71;_LL28: {struct Cyc_Absyn_Var_e_struct*
_tmp66=(struct Cyc_Absyn_Var_e_struct*)_tmp65;if(_tmp66->tag != 1)goto _LL2A;else{
_tmp67=_tmp66->f1;_tmp68=*_tmp67;_tmp69=_tmp68.f1;if((_tmp69.Rel_n).tag != 1)goto
_LL2A;_tmp6A=(struct Cyc_List_List*)(_tmp69.Rel_n).val;if(_tmp6A != 0)goto _LL2A;
_tmp6B=_tmp68.f2;}}_LL29: _tmp71=_tmp6B;goto _LL2B;_LL2A: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp6C=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp65;if(_tmp6C->tag != 2)goto _LL2C;
else{_tmp6D=_tmp6C->f1;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.f1;if((_tmp6F.Rel_n).tag != 1)
goto _LL2C;_tmp70=(struct Cyc_List_List*)(_tmp6F.Rel_n).val;if(_tmp70 != 0)goto
_LL2C;_tmp71=_tmp6E.f2;}}_LL2B:{struct Cyc_List_List*_tmp72=tags;for(0;_tmp72 != 0;
_tmp72=_tmp72->tl){struct _tuple16 _tmp74;struct _dyneither_ptr*_tmp75;void*_tmp76;
struct _tuple16*_tmp73=(struct _tuple16*)_tmp72->hd;_tmp74=*_tmp73;_tmp75=_tmp74.f1;
_tmp76=_tmp74.f2;if(Cyc_strptrcmp(_tmp75,_tmp71)== 0)return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Valueof_e_struct*_tmp77=_cycalloc(sizeof(*_tmp77));
_tmp77[0]=({struct Cyc_Absyn_Valueof_e_struct _tmp78;_tmp78.tag=40;_tmp78.f1=(void*)
Cyc_Tcutil_copy_type(_tmp76);_tmp78;});_tmp77;}),e->loc);}}goto _LL27;_LL2C:;
_LL2D: goto _LL27;_LL27:;}return e;}static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*
tags,void*t){{void*_tmp79=t;struct Cyc_Absyn_ArrayInfo _tmp7B;void*_tmp7C;struct
Cyc_Absyn_Tqual _tmp7D;struct Cyc_Absyn_Exp*_tmp7E;union Cyc_Absyn_Constraint*
_tmp7F;struct Cyc_Position_Segment*_tmp80;struct Cyc_Absyn_PtrInfo _tmp82;void*
_tmp83;struct Cyc_Absyn_Tqual _tmp84;struct Cyc_Absyn_PtrAtts _tmp85;void*_tmp86;
union Cyc_Absyn_Constraint*_tmp87;union Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*
_tmp89;struct Cyc_Absyn_PtrLoc*_tmp8A;_LL2F: {struct Cyc_Absyn_ArrayType_struct*
_tmp7A=(struct Cyc_Absyn_ArrayType_struct*)_tmp79;if(_tmp7A->tag != 9)goto _LL31;
else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.elt_type;_tmp7D=_tmp7B.tq;_tmp7E=_tmp7B.num_elts;
_tmp7F=_tmp7B.zero_term;_tmp80=_tmp7B.zt_loc;}}_LL30: {struct Cyc_Absyn_Exp*
nelts2=_tmp7E;if(_tmp7E != 0)nelts2=(struct Cyc_Absyn_Exp*)Cyc_Parse_substitute_tags_exp(
tags,(struct Cyc_Absyn_Exp*)_tmp7E);{void*_tmp8B=Cyc_Parse_substitute_tags(tags,
_tmp7C);if(_tmp7E != nelts2  || _tmp7C != _tmp8B)return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp8D;_tmp8D.tag=9;_tmp8D.f1=({struct Cyc_Absyn_ArrayInfo _tmp8E;_tmp8E.elt_type=
_tmp8B;_tmp8E.tq=_tmp7D;_tmp8E.num_elts=nelts2;_tmp8E.zero_term=_tmp7F;_tmp8E.zt_loc=
_tmp80;_tmp8E;});_tmp8D;});_tmp8C;});goto _LL2E;};}_LL31: {struct Cyc_Absyn_PointerType_struct*
_tmp81=(struct Cyc_Absyn_PointerType_struct*)_tmp79;if(_tmp81->tag != 5)goto _LL33;
else{_tmp82=_tmp81->f1;_tmp83=_tmp82.elt_typ;_tmp84=_tmp82.elt_tq;_tmp85=_tmp82.ptr_atts;
_tmp86=_tmp85.rgn;_tmp87=_tmp85.nullable;_tmp88=_tmp85.bounds;_tmp89=_tmp85.zero_term;
_tmp8A=_tmp85.ptrloc;}}_LL32: {void*_tmp8F=Cyc_Parse_substitute_tags(tags,_tmp83);
union Cyc_Absyn_Constraint*_tmp90=_tmp88;{union Cyc_Absyn_Constraint _tmp91=*_tmp88;
void*_tmp92;struct Cyc_Absyn_Exp*_tmp94;_LL36: if((_tmp91.Eq_constr).tag != 1)goto
_LL38;_tmp92=(void*)(_tmp91.Eq_constr).val;{struct Cyc_Absyn_Upper_b_struct*
_tmp93=(struct Cyc_Absyn_Upper_b_struct*)_tmp92;if(_tmp93->tag != 1)goto _LL38;
else{_tmp94=_tmp93->f1;}};_LL37: _tmp94=Cyc_Parse_substitute_tags_exp(tags,_tmp94);
_tmp90=({union Cyc_Absyn_Constraint*_tmp95=_cycalloc(sizeof(*_tmp95));(_tmp95->Eq_constr).val=(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp97;_tmp97.tag=1;_tmp97.f1=_tmp94;_tmp97;});
_tmp96;});(_tmp95->Eq_constr).tag=1;_tmp95;});goto _LL35;_LL38:;_LL39: goto _LL35;
_LL35:;}if(_tmp8F != _tmp83  || _tmp90 != _tmp88)return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct Cyc_Absyn_PointerType_struct
_tmp99;_tmp99.tag=5;_tmp99.f1=({struct Cyc_Absyn_PtrInfo _tmp9A;_tmp9A.elt_typ=
_tmp8F;_tmp9A.elt_tq=_tmp84;_tmp9A.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9B;
_tmp9B.rgn=_tmp86;_tmp9B.nullable=_tmp87;_tmp9B.bounds=_tmp90;_tmp9B.zero_term=
_tmp89;_tmp9B.ptrloc=_tmp8A;_tmp9B;});_tmp9A;});_tmp99;});_tmp98;});goto _LL2E;}
_LL33:;_LL34: goto _LL2E;_LL2E:;}return t;}static void Cyc_Parse_substitute_aggrfield_tags(
struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(
tags,x->type);}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static struct
_tuple17*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple8*t){
return({struct _tuple17*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->f1=(*t).f2;
_tmp9C->f2=(*t).f3;_tmp9C;});}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp9D=d->r;struct Cyc_Absyn_Vardecl*_tmp9F;_LL3B: {
struct Cyc_Absyn_Var_d_struct*_tmp9E=(struct Cyc_Absyn_Var_d_struct*)_tmp9D;if(
_tmp9E->tag != 0)goto _LL3D;else{_tmp9F=_tmp9E->f1;}}_LL3C: return _tmp9F;_LL3D:;
_LL3E:({void*_tmpA0=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,({const char*
_tmpA1="bad declaration in `forarray' statement";_tag_dyneither(_tmpA1,sizeof(
char),40);}),_tag_dyneither(_tmpA0,sizeof(void*),0));});_LL3A:;}static int Cyc_Parse_is_typeparam(
void*tm){void*_tmpA2=tm;_LL40: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpA3=(
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpA2;if(_tmpA3->tag != 4)goto _LL42;}
_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}static void*Cyc_Parse_id2type(struct
_dyneither_ptr s,void*k){if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*
_tmpA4="`H";_tag_dyneither(_tmpA4,sizeof(char),3);}))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;
else{if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmpA5="`U";
_tag_dyneither(_tmpA5,sizeof(char),3);}))== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;
else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmpA6=_cycalloc(sizeof(*
_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_VarType_struct _tmpA7;_tmpA7.tag=2;_tmpA7.f1=({
struct Cyc_Absyn_Tvar*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->name=({struct
_dyneither_ptr*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=s;_tmpA9;});_tmpA8->identity=
- 1;_tmpA8->kind=k;_tmpA8;});_tmpA7;});_tmpA6;});}}}static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{void*_tmpAA=Cyc_Absyn_compress_kb(
t->kind);enum Cyc_Absyn_Kind _tmpAD;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpAB=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpAA;if(_tmpAB->tag != 1)goto _LL47;}
_LL46: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmpAE=_cycalloc(sizeof(*
_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmpAF;_tmpAF.tag=1;_tmpAF.f1=
0;_tmpAF;});_tmpAE;});goto _LL44;_LL47: {struct Cyc_Absyn_Less_kb_struct*_tmpAC=(
struct Cyc_Absyn_Less_kb_struct*)_tmpAA;if(_tmpAC->tag != 2)goto _LL49;else{_tmpAD=
_tmpAC->f2;}}_LL48: k=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmpB0=_cycalloc(
sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_Less_kb_struct _tmpB1;_tmpB1.tag=2;
_tmpB1.f1=0;_tmpB1.f2=_tmpAD;_tmpB1;});_tmpB0;});goto _LL44;_LL49:;_LL4A: k=_tmpAA;
goto _LL44;_LL44:;}return({struct Cyc_Absyn_Tvar*_tmpB2=_cycalloc(sizeof(*_tmpB2));
_tmpB2->name=t->name;_tmpB2->identity=- 1;_tmpB2->kind=k;_tmpB2;});}static struct
Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,void*t){void*
_tmpB3=t;struct Cyc_Absyn_Tvar*_tmpB5;_LL4C: {struct Cyc_Absyn_VarType_struct*
_tmpB4=(struct Cyc_Absyn_VarType_struct*)_tmpB3;if(_tmpB4->tag != 2)goto _LL4E;
else{_tmpB5=_tmpB4->f1;}}_LL4D: return _tmpB5;_LL4E:;_LL4F:({void*_tmpB6=0;((int(*)(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Parse_abort)(loc,({const char*_tmpB7="expecting a list of type variables, not types";
_tag_dyneither(_tmpB7,sizeof(char),46);}),_tag_dyneither(_tmpB6,sizeof(void*),0));});
_LL4B:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(void*)({
struct Cyc_Absyn_VarType_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({
struct Cyc_Absyn_VarType_struct _tmpB9;_tmpB9.tag=2;_tmpB9.f1=pr;_tmpB9;});_tmpB8;});}
static void Cyc_Parse_set_vartyp_kind(void*t,enum Cyc_Absyn_Kind k,int leq){void*
_tmpBA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpBC;struct Cyc_Absyn_Tvar
_tmpBD;void*_tmpBE;void**_tmpBF;_LL51: {struct Cyc_Absyn_VarType_struct*_tmpBB=(
struct Cyc_Absyn_VarType_struct*)_tmpBA;if(_tmpBB->tag != 2)goto _LL53;else{_tmpBC=
_tmpBB->f1;_tmpBD=*_tmpBC;_tmpBE=_tmpBD.kind;_tmpBF=(void**)&(*_tmpBB->f1).kind;}}
_LL52: {void*_tmpC0=Cyc_Absyn_compress_kb(*_tmpBF);_LL56: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC1=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC0;if(_tmpC1->tag != 1)goto _LL58;}
_LL57: if(!leq)*_tmpBF=Cyc_Tcutil_kind_to_bound(k);else{*_tmpBF=(void*)({struct
Cyc_Absyn_Less_kb_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_Less_kb_struct
_tmpC3;_tmpC3.tag=2;_tmpC3.f1=0;_tmpC3.f2=k;_tmpC3;});_tmpC2;});}return;_LL58:;
_LL59: return;_LL55:;}_LL53:;_LL54: return;_LL50:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmpC4=(void*)tms->hd;void*_tmpC6;
_LL5B: {struct Cyc_Absyn_Function_mod_struct*_tmpC5=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC4;if(_tmpC5->tag != 3)goto _LL5D;else{_tmpC6=(void*)_tmpC5->f1;}}_LL5C: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmpC7=_tmpC6;
struct Cyc_List_List*_tmpCA;_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpC8=(
struct Cyc_Absyn_WithTypes_struct*)_tmpC7;if(_tmpC8->tag != 1)goto _LL62;}_LL61:({
void*_tmpCB=0;Cyc_Tcutil_warn(loc,({const char*_tmpCC="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dyneither(_tmpCC,sizeof(char),93);}),_tag_dyneither(_tmpCB,sizeof(void*),0));});
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpC9=(struct Cyc_Absyn_NoTypes_struct*)
_tmpC7;if(_tmpC9->tag != 0)goto _LL5F;else{_tmpCA=_tmpC9->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpCA)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds))({void*_tmpCD=0;((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmpCE="wrong number of parameter declarations in old-style function declaration";
_tag_dyneither(_tmpCE,sizeof(char),73);}),_tag_dyneither(_tmpCD,sizeof(void*),0));});{
struct Cyc_List_List*_tmpCF=0;for(0;_tmpCA != 0;_tmpCA=_tmpCA->tl){struct Cyc_List_List*
_tmpD0=tds;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){struct Cyc_Absyn_Decl*_tmpD1=(
struct Cyc_Absyn_Decl*)_tmpD0->hd;void*_tmpD2=_tmpD1->r;struct Cyc_Absyn_Vardecl*
_tmpD4;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpD3=(struct Cyc_Absyn_Var_d_struct*)
_tmpD2;if(_tmpD3->tag != 0)goto _LL67;else{_tmpD4=_tmpD3->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpD4->name).f2,(struct _dyneither_ptr*)_tmpCA->hd)!= 0)continue;if(_tmpD4->initializer
!= 0)({void*_tmpD5=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,({const char*_tmpD6="initializer found in parameter declaration";
_tag_dyneither(_tmpD6,sizeof(char),43);}),_tag_dyneither(_tmpD5,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmpD4->name))({void*_tmpD7=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,({
const char*_tmpD8="namespaces forbidden in parameter declarations";_tag_dyneither(
_tmpD8,sizeof(char),47);}),_tag_dyneither(_tmpD7,sizeof(void*),0));});_tmpCF=({
struct Cyc_List_List*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->hd=({struct _tuple8*
_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->f1=({struct Cyc_Core_Opt*_tmpDB=
_cycalloc(sizeof(*_tmpDB));_tmpDB->v=(*_tmpD4->name).f2;_tmpDB;});_tmpDA->f2=
_tmpD4->tq;_tmpDA->f3=_tmpD4->type;_tmpDA;});_tmpD9->tl=_tmpCF;_tmpD9;});goto L;
_LL67:;_LL68:({void*_tmpDC=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,({const
char*_tmpDD="nonvariable declaration in parameter type";_tag_dyneither(_tmpDD,
sizeof(char),42);}),_tag_dyneither(_tmpDC,sizeof(void*),0));});_LL64:;}L: if(
_tmpD0 == 0)({struct Cyc_String_pa_struct _tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpCA->hd));({
void*_tmpDE[1]={& _tmpE0};((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmpDF="%s is not given a type";_tag_dyneither(_tmpDF,sizeof(char),23);}),
_tag_dyneither(_tmpDE,sizeof(void*),1));});});}return({struct Cyc_List_List*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_Function_mod_struct
_tmpE3;_tmpE3.tag=3;_tmpE3.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_WithTypes_struct
_tmpE5;_tmpE5.tag=1;_tmpE5.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmpCF);_tmpE5.f2=0;_tmpE5.f3=0;_tmpE5.f4=0;_tmpE5.f5=0;_tmpE5;});
_tmpE4;}));_tmpE3;});_tmpE2;});_tmpE1->tl=0;_tmpE1;});};_LL5F:;}goto _LL5E;_LL5D:;
_LL5E: return({struct Cyc_List_List*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->hd=(
void*)tms->hd;_tmpE6->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmpE6;});
_LL5A:;};}struct _tuple18{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;
};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,
struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,
struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmpE7=
_cycalloc(sizeof(*_tmpE7));_tmpE7->id=d->id;_tmpE7->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmpE7;});{enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;struct Cyc_List_List*
tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*
atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;
tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)
dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;
if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)switch((enum Cyc_Parse_Storage_class)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){
case Cyc_Parse_Extern_sc: _LL69: sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc:
_LL6A: sc=Cyc_Absyn_Static;break;default: _LL6B: Cyc_Parse_err(({const char*_tmpE8="bad storage class on function";
_tag_dyneither(_tmpE8,sizeof(char),30);}),loc);break;}}{void*_tmpEA;struct Cyc_Core_Opt*
_tmpEB;struct _tuple11 _tmpE9=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpEA=
_tmpE9.f1;_tmpEB=_tmpE9.f2;{struct Cyc_Absyn_Tqual _tmpED;void*_tmpEE;struct Cyc_List_List*
_tmpEF;struct Cyc_List_List*_tmpF0;struct _tuple12 _tmpEC=Cyc_Parse_apply_tms(tq,
_tmpEA,atts,d->tms);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;_tmpEF=_tmpEC.f3;_tmpF0=
_tmpEC.f4;if(_tmpEB != 0)({void*_tmpF1=0;Cyc_Tcutil_warn(loc,({const char*_tmpF2="nested type declaration within function prototype";
_tag_dyneither(_tmpF2,sizeof(char),50);}),_tag_dyneither(_tmpF1,sizeof(void*),0));});
if(_tmpEF != 0)({void*_tmpF3=0;Cyc_Tcutil_warn(loc,({const char*_tmpF4="bad type params, ignoring";
_tag_dyneither(_tmpF4,sizeof(char),26);}),_tag_dyneither(_tmpF3,sizeof(void*),0));});{
void*_tmpF5=_tmpEE;struct Cyc_Absyn_FnInfo _tmpF7;struct Cyc_List_List*_tmpF8;
struct Cyc_Core_Opt*_tmpF9;void*_tmpFA;struct Cyc_List_List*_tmpFB;int _tmpFC;
struct Cyc_Absyn_VarargInfo*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*
_tmpFF;_LL6E: {struct Cyc_Absyn_FnType_struct*_tmpF6=(struct Cyc_Absyn_FnType_struct*)
_tmpF5;if(_tmpF6->tag != 10)goto _LL70;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF7.tvars;
_tmpF9=_tmpF7.effect;_tmpFA=_tmpF7.ret_typ;_tmpFB=_tmpF7.args;_tmpFC=_tmpF7.c_varargs;
_tmpFD=_tmpF7.cyc_varargs;_tmpFE=_tmpF7.rgn_po;_tmpFF=_tmpF7.attributes;}}_LL6F: {
struct Cyc_List_List*_tmp100=0;{struct Cyc_List_List*_tmp101=_tmpFB;for(0;_tmp101
!= 0;_tmp101=_tmp101->tl){struct _tuple8 _tmp103;struct Cyc_Core_Opt*_tmp104;struct
Cyc_Absyn_Tqual _tmp105;void*_tmp106;struct _tuple8*_tmp102=(struct _tuple8*)
_tmp101->hd;_tmp103=*_tmp102;_tmp104=_tmp103.f1;_tmp105=_tmp103.f2;_tmp106=
_tmp103.f3;if(_tmp104 == 0){Cyc_Parse_err(({const char*_tmp107="missing argument variable in function prototype";
_tag_dyneither(_tmp107,sizeof(char),48);}),loc);_tmp100=({struct Cyc_List_List*
_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->hd=({struct _tuple18*_tmp109=
_cycalloc(sizeof(*_tmp109));_tmp109->f1=_init_dyneither_ptr(_cycalloc(sizeof(
struct _dyneither_ptr)),"?",sizeof(char),2);_tmp109->f2=_tmp105;_tmp109->f3=
_tmp106;_tmp109;});_tmp108->tl=_tmp100;_tmp108;});}else{_tmp100=({struct Cyc_List_List*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->hd=({struct _tuple18*_tmp10C=
_cycalloc(sizeof(*_tmp10C));_tmp10C->f1=(struct _dyneither_ptr*)_tmp104->v;
_tmp10C->f2=_tmp105;_tmp10C->f3=_tmp106;_tmp10C;});_tmp10B->tl=_tmp100;_tmp10B;});}}}
return({struct Cyc_Absyn_Fndecl*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->sc=
sc;_tmp10D->name=d->id;_tmp10D->tvs=_tmpF8;_tmp10D->is_inline=is_inline;_tmp10D->effect=
_tmpF9;_tmp10D->ret_type=_tmpFA;_tmp10D->args=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp100);_tmp10D->c_varargs=_tmpFC;_tmp10D->cyc_varargs=
_tmpFD;_tmp10D->rgn_po=_tmpFE;_tmp10D->body=body;_tmp10D->cached_typ=0;_tmp10D->param_vardecls=
0;_tmp10D->fn_vardecl=0;_tmp10D->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmpFF,_tmpF0);_tmp10D;});}_LL70:;
_LL71:({void*_tmp10E=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmp10F="declarator is not a function prototype";_tag_dyneither(_tmp10F,sizeof(
char),39);}),_tag_dyneither(_tmp10E,sizeof(void*),0));});_LL6D:;};};};};}static
char _tmp110[52]="at most one type may appear within a type specifier";static
struct _dyneither_ptr Cyc_Parse_msg1={_tmp110,_tmp110,_tmp110 + 52};static char
_tmp111[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp111,_tmp111,_tmp111 + 63};static
char _tmp112[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp112,_tmp112,_tmp112 + 50};static char _tmp113[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp113,_tmp113,_tmp113 + 60};static
struct _tuple11 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct
Cyc_Position_Segment*loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int
seen_sign=0;int seen_size=0;void*t=(void*)& Cyc_Absyn_VoidType_val;enum Cyc_Absyn_Size_of
sz=Cyc_Absyn_Int_sz;enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp114=(void*)ts->hd;void*_tmp116;
struct Cyc_Position_Segment*_tmp117;struct Cyc_Position_Segment*_tmp119;struct Cyc_Position_Segment*
_tmp11B;struct Cyc_Position_Segment*_tmp11D;struct Cyc_Position_Segment*_tmp11F;
struct Cyc_Absyn_Decl*_tmp121;_LL73: {struct Cyc_Parse_Type_spec_struct*_tmp115=(
struct Cyc_Parse_Type_spec_struct*)_tmp114;if(_tmp115->tag != 4)goto _LL75;else{
_tmp116=(void*)_tmp115->f1;_tmp117=_tmp115->f2;}}_LL74: if(seen_type)Cyc_Parse_err(
Cyc_Parse_msg1,_tmp117);last_loc=_tmp117;seen_type=1;t=_tmp116;goto _LL72;_LL75: {
struct Cyc_Parse_Signed_spec_struct*_tmp118=(struct Cyc_Parse_Signed_spec_struct*)
_tmp114;if(_tmp118->tag != 0)goto _LL77;else{_tmp119=_tmp118->f1;}}_LL76: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp119);if(seen_type)Cyc_Parse_err(({
const char*_tmp122="signed qualifier must come before type";_tag_dyneither(
_tmp122,sizeof(char),39);}),_tmp119);last_loc=_tmp119;seen_sign=1;sgn=Cyc_Absyn_Signed;
goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*_tmp11A=(struct Cyc_Parse_Unsigned_spec_struct*)
_tmp114;if(_tmp11A->tag != 1)goto _LL79;else{_tmp11B=_tmp11A->f1;}}_LL78: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11B);if(seen_type)Cyc_Parse_err(({
const char*_tmp123="signed qualifier must come before type";_tag_dyneither(
_tmp123,sizeof(char),39);}),_tmp11B);last_loc=_tmp11B;seen_sign=1;sgn=Cyc_Absyn_Unsigned;
goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*_tmp11C=(struct Cyc_Parse_Short_spec_struct*)
_tmp114;if(_tmp11C->tag != 2)goto _LL7B;else{_tmp11D=_tmp11C->f1;}}_LL7A: if(
seen_size)Cyc_Parse_err(({const char*_tmp124="integral size may appear only once within a type specifier";
_tag_dyneither(_tmp124,sizeof(char),59);}),_tmp11D);if(seen_type)Cyc_Parse_err(({
const char*_tmp125="short modifier must come before base type";_tag_dyneither(
_tmp125,sizeof(char),42);}),_tmp11D);last_loc=_tmp11D;seen_size=1;sz=Cyc_Absyn_Short_sz;
goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp11E=(struct Cyc_Parse_Long_spec_struct*)
_tmp114;if(_tmp11E->tag != 3)goto _LL7D;else{_tmp11F=_tmp11E->f1;}}_LL7C: if(
seen_type)Cyc_Parse_err(({const char*_tmp126="long modifier must come before base type";
_tag_dyneither(_tmp126,sizeof(char),41);}),_tmp11F);if(seen_size)switch(sz){case
Cyc_Absyn_Long_sz: _LL7F: sz=Cyc_Absyn_LongLong_sz;break;default: _LL80: Cyc_Parse_err(({
const char*_tmp127="extra long in type specifier";_tag_dyneither(_tmp127,sizeof(
char),29);}),_tmp11F);break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp11F;
seen_size=1;goto _LL72;_LL7D: {struct Cyc_Parse_Decl_spec_struct*_tmp120=(struct
Cyc_Parse_Decl_spec_struct*)_tmp114;if(_tmp120->tag != 5)goto _LL72;else{_tmp121=
_tmp120->f1;}}_LL7E: last_loc=_tmp121->loc;if(declopt == 0  && !seen_type){
seen_type=1;{void*_tmp128=_tmp121->r;struct Cyc_Absyn_Aggrdecl*_tmp12A;struct Cyc_Absyn_Datatypedecl*
_tmp12C;struct Cyc_Absyn_Enumdecl*_tmp12E;_LL83: {struct Cyc_Absyn_Aggr_d_struct*
_tmp129=(struct Cyc_Absyn_Aggr_d_struct*)_tmp128;if(_tmp129->tag != 6)goto _LL85;
else{_tmp12A=_tmp129->f1;}}_LL84: {struct Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp12A->tvs));t=(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130[0]=({
struct Cyc_Absyn_AggrType_struct _tmp131;_tmp131.tag=12;_tmp131.f1=({struct Cyc_Absyn_AggrInfo
_tmp132;_tmp132.aggr_info=Cyc_Absyn_UnknownAggr(_tmp12A->kind,_tmp12A->name,0);
_tmp132.targs=_tmp12F;_tmp132;});_tmp131;});_tmp130;});if(_tmp12A->impl != 0)
declopt=({struct Cyc_Core_Opt*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->v=
_tmp121;_tmp133;});goto _LL82;}_LL85: {struct Cyc_Absyn_Datatype_d_struct*_tmp12B=(
struct Cyc_Absyn_Datatype_d_struct*)_tmp128;if(_tmp12B->tag != 7)goto _LL87;else{
_tmp12C=_tmp12B->f1;}}_LL86: {struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp12C->tvs));t=(void*)({
struct Cyc_Absyn_DatatypeType_struct*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135[
0]=({struct Cyc_Absyn_DatatypeType_struct _tmp136;_tmp136.tag=3;_tmp136.f1=({
struct Cyc_Absyn_DatatypeInfo _tmp137;_tmp137.datatype_info=Cyc_Absyn_KnownDatatype(({
struct Cyc_Absyn_Datatypedecl**_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=
_tmp12C;_tmp138;}));_tmp137.targs=_tmp134;_tmp137;});_tmp136;});_tmp135;});if(
_tmp12C->fields != 0)declopt=({struct Cyc_Core_Opt*_tmp139=_cycalloc(sizeof(*
_tmp139));_tmp139->v=_tmp121;_tmp139;});goto _LL82;}_LL87: {struct Cyc_Absyn_Enum_d_struct*
_tmp12D=(struct Cyc_Absyn_Enum_d_struct*)_tmp128;if(_tmp12D->tag != 8)goto _LL89;
else{_tmp12E=_tmp12D->f1;}}_LL88: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=({struct Cyc_Absyn_EnumType_struct
_tmp13B;_tmp13B.tag=14;_tmp13B.f1=_tmp12E->name;_tmp13B.f2=0;_tmp13B;});_tmp13A;});
declopt=({struct Cyc_Core_Opt*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->v=
_tmp121;_tmp13C;});goto _LL82;_LL89:;_LL8A:({void*_tmp13D=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp121->loc,({
const char*_tmp13E="bad declaration within type specifier";_tag_dyneither(_tmp13E,
sizeof(char),38);}),_tag_dyneither(_tmp13D,sizeof(void*),0));});_LL82:;};}else{
Cyc_Parse_err(Cyc_Parse_msg3,_tmp121->loc);}goto _LL72;_LL72:;}if(!seen_type){if(
!seen_sign  && !seen_size)({void*_tmp13F=0;Cyc_Tcutil_warn(last_loc,({const char*
_tmp140="missing type within specifier";_tag_dyneither(_tmp140,sizeof(char),30);}),
_tag_dyneither(_tmp13F,sizeof(void*),0));});t=Cyc_Absyn_int_typ(sgn,sz);}else{
if(seen_sign){void*_tmp141=t;enum Cyc_Absyn_Sign _tmp143;enum Cyc_Absyn_Size_of
_tmp144;_LL8C: {struct Cyc_Absyn_IntType_struct*_tmp142=(struct Cyc_Absyn_IntType_struct*)
_tmp141;if(_tmp142->tag != 6)goto _LL8E;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}
_LL8D: if(_tmp143 != sgn)t=Cyc_Absyn_int_typ(sgn,_tmp144);goto _LL8B;_LL8E:;_LL8F:
Cyc_Parse_err(({const char*_tmp145="sign specification on non-integral type";
_tag_dyneither(_tmp145,sizeof(char),40);}),last_loc);goto _LL8B;_LL8B:;}if(
seen_size){void*_tmp146=t;enum Cyc_Absyn_Sign _tmp148;enum Cyc_Absyn_Size_of
_tmp149;_LL91: {struct Cyc_Absyn_IntType_struct*_tmp147=(struct Cyc_Absyn_IntType_struct*)
_tmp146;if(_tmp147->tag != 6)goto _LL93;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}
_LL92: if(_tmp149 != sz)t=Cyc_Absyn_int_typ(_tmp148,sz);goto _LL90;_LL93: {struct
Cyc_Absyn_DoubleType_struct*_tmp14A=(struct Cyc_Absyn_DoubleType_struct*)_tmp146;
if(_tmp14A->tag != 8)goto _LL95;}_LL94: t=Cyc_Absyn_double_typ(1);goto _LL90;_LL95:;
_LL96: Cyc_Parse_err(({const char*_tmp14B="size qualifier on non-integral type";
_tag_dyneither(_tmp14B,sizeof(char),36);}),last_loc);goto _LL90;_LL90:;}}return({
struct _tuple11 _tmp14C;_tmp14C.f1=t;_tmp14C.f2=declopt;_tmp14C;});}static struct
Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{
struct Cyc_Parse_Declarator*_tmp14D=(struct Cyc_Parse_Declarator*)ds->hd;struct
_tuple1*_tmp14E=_tmp14D->id;struct Cyc_Absyn_Tqual _tmp150;void*_tmp151;struct Cyc_List_List*
_tmp152;struct Cyc_List_List*_tmp153;struct _tuple12 _tmp14F=Cyc_Parse_apply_tms(tq,
t,shared_atts,_tmp14D->tms);_tmp150=_tmp14F.f1;_tmp151=_tmp14F.f2;_tmp152=
_tmp14F.f3;_tmp153=_tmp14F.f4;if(ds->tl == 0)return({struct Cyc_List_List*_tmp154=
_region_malloc(r,sizeof(*_tmp154));_tmp154->hd=({struct _tuple14*_tmp155=
_region_malloc(r,sizeof(*_tmp155));_tmp155->f1=_tmp14E;_tmp155->f2=_tmp150;
_tmp155->f3=_tmp151;_tmp155->f4=_tmp152;_tmp155->f5=_tmp153;_tmp155;});_tmp154->tl=
0;_tmp154;});else{return({struct Cyc_List_List*_tmp156=_region_malloc(r,sizeof(*
_tmp156));_tmp156->hd=({struct _tuple14*_tmp157=_region_malloc(r,sizeof(*_tmp157));
_tmp157->f1=_tmp14E;_tmp157->f2=_tmp150;_tmp157->f3=_tmp151;_tmp157->f4=_tmp152;
_tmp157->f5=_tmp153;_tmp157;});_tmp156->tl=Cyc_Parse_apply_tmss(r,_tmp150,Cyc_Tcutil_copy_type(
t),ds->tl,shared_atts);_tmp156;});}};}static struct _tuple12 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
if(tms == 0)return({struct _tuple12 _tmp158;_tmp158.f1=tq;_tmp158.f2=t;_tmp158.f3=0;
_tmp158.f4=atts;_tmp158;});{void*_tmp159=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmp15B;struct Cyc_Position_Segment*_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;union Cyc_Absyn_Constraint*
_tmp15F;struct Cyc_Position_Segment*_tmp160;void*_tmp162;struct Cyc_List_List*
_tmp164;struct Cyc_Position_Segment*_tmp165;struct Cyc_Absyn_PtrAtts _tmp167;struct
Cyc_Absyn_Tqual _tmp168;struct Cyc_Position_Segment*_tmp16A;struct Cyc_List_List*
_tmp16B;_LL98: {struct Cyc_Absyn_Carray_mod_struct*_tmp15A=(struct Cyc_Absyn_Carray_mod_struct*)
_tmp159;if(_tmp15A->tag != 0)goto _LL9A;else{_tmp15B=_tmp15A->f1;_tmp15C=_tmp15A->f2;}}
_LL99: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,
tq,0,_tmp15B,_tmp15C),atts,tms->tl);_LL9A: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmp15D=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmp159;if(_tmp15D->tag != 1)goto
_LL9C;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;_tmp160=_tmp15D->f3;}}_LL9B:
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(
struct Cyc_Absyn_Exp*)_tmp15E,_tmp15F,_tmp160),atts,tms->tl);_LL9C: {struct Cyc_Absyn_Function_mod_struct*
_tmp161=(struct Cyc_Absyn_Function_mod_struct*)_tmp159;if(_tmp161->tag != 3)goto
_LL9E;else{_tmp162=(void*)_tmp161->f1;}}_LL9D: {void*_tmp16C=_tmp162;struct Cyc_List_List*
_tmp16E;int _tmp16F;struct Cyc_Absyn_VarargInfo*_tmp170;struct Cyc_Core_Opt*_tmp171;
struct Cyc_List_List*_tmp172;struct Cyc_Position_Segment*_tmp174;_LLA5: {struct Cyc_Absyn_WithTypes_struct*
_tmp16D=(struct Cyc_Absyn_WithTypes_struct*)_tmp16C;if(_tmp16D->tag != 1)goto _LLA7;
else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;_tmp170=_tmp16D->f3;_tmp171=_tmp16D->f4;
_tmp172=_tmp16D->f5;}}_LLA6: {struct Cyc_List_List*typvars=0;struct Cyc_List_List*
fn_atts=0;struct Cyc_List_List*new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 
0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))fn_atts=({struct Cyc_List_List*
_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->hd=(void*)as->hd;_tmp175->tl=
fn_atts;_tmp175;});else{new_atts=({struct Cyc_List_List*_tmp176=_cycalloc(sizeof(*
_tmp176));_tmp176->hd=(void*)as->hd;_tmp176->tl=new_atts;_tmp176;});}}}if(tms->tl
!= 0){void*_tmp177=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
struct Cyc_List_List*_tmp179;_LLAA: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp178=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp177;if(_tmp178->tag != 4)goto
_LLAC;else{_tmp179=_tmp178->f1;}}_LLAB: typvars=_tmp179;tms=tms->tl;goto _LLA9;
_LLAC:;_LLAD: goto _LLA9;_LLA9:;}if(((((!_tmp16F  && _tmp170 == 0) && _tmp16E != 0)
 && _tmp16E->tl == 0) && (*((struct _tuple8*)_tmp16E->hd)).f1 == 0) && (*((struct
_tuple8*)_tmp16E->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)_tmp16E=0;{struct Cyc_List_List*
_tmp17A=Cyc_Parse_get_arg_tags(_tmp16E);if(_tmp17A != 0)t=Cyc_Parse_substitute_tags(
_tmp17A,t);t=Cyc_Parse_array2ptr(t,0);{struct Cyc_List_List*_tmp17B=_tmp16E;for(0;
_tmp17B != 0;_tmp17B=_tmp17B->tl){struct _tuple8 _tmp17D;struct Cyc_Core_Opt*_tmp17E;
struct Cyc_Absyn_Tqual _tmp17F;void*_tmp180;void**_tmp181;struct _tuple8*_tmp17C=(
struct _tuple8*)_tmp17B->hd;_tmp17D=*_tmp17C;_tmp17E=_tmp17D.f1;_tmp17F=_tmp17D.f2;
_tmp180=_tmp17D.f3;_tmp181=(void**)&(*_tmp17C).f3;if(_tmp17A != 0)*_tmp181=Cyc_Parse_substitute_tags(
_tmp17A,*_tmp181);*_tmp181=Cyc_Parse_array2ptr(*_tmp181,1);}}return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(typvars,_tmp171,t,_tmp16E,
_tmp16F,_tmp170,_tmp172,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}
_LLA7: {struct Cyc_Absyn_NoTypes_struct*_tmp173=(struct Cyc_Absyn_NoTypes_struct*)
_tmp16C;if(_tmp173->tag != 0)goto _LLA4;else{_tmp174=_tmp173->f2;}}_LLA8:({void*
_tmp182=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(_tmp174,({const char*_tmp183="function declaration without parameter types";
_tag_dyneither(_tmp183,sizeof(char),45);}),_tag_dyneither(_tmp182,sizeof(void*),
0));});_LLA4:;}_LL9E: {struct Cyc_Absyn_TypeParams_mod_struct*_tmp163=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp159;if(_tmp163->tag != 4)goto _LLA0;else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}}
_LL9F: if(tms->tl == 0)return({struct _tuple12 _tmp184;_tmp184.f1=tq;_tmp184.f2=t;
_tmp184.f3=_tmp164;_tmp184.f4=atts;_tmp184;});({void*_tmp185=0;((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
_tmp165,({const char*_tmp186="type parameters must appear before function arguments in declarator";
_tag_dyneither(_tmp186,sizeof(char),68);}),_tag_dyneither(_tmp185,sizeof(void*),
0));});_LLA0: {struct Cyc_Absyn_Pointer_mod_struct*_tmp166=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp159;if(_tmp166->tag != 2)goto _LLA2;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}
_LLA1: return Cyc_Parse_apply_tms(_tmp168,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_PointerType_struct
_tmp188;_tmp188.tag=5;_tmp188.f1=({struct Cyc_Absyn_PtrInfo _tmp189;_tmp189.elt_typ=
t;_tmp189.elt_tq=tq;_tmp189.ptr_atts=_tmp167;_tmp189;});_tmp188;});_tmp187;}),
atts,tms->tl);_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*_tmp169=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmp159;if(_tmp169->tag != 5)goto _LL97;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}
_LLA3: return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp16B),tms->tl);_LL97:;};}void*
Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){
void*_tmp18B;struct Cyc_Core_Opt*_tmp18C;struct _tuple11 _tmp18A=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp18B=_tmp18A.f1;_tmp18C=_tmp18A.f2;if(_tmp18C != 0)({void*_tmp18D=0;
Cyc_Tcutil_warn(loc,({const char*_tmp18E="ignoring nested type declaration(s) in specifier list";
_tag_dyneither(_tmp18E,sizeof(char),54);}),_tag_dyneither(_tmp18D,sizeof(void*),
0));});return _tmp18B;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple14*t){struct _tuple14 _tmp190;struct
_tuple1*_tmp191;struct Cyc_Absyn_Tqual _tmp192;void*_tmp193;struct Cyc_List_List*
_tmp194;struct Cyc_List_List*_tmp195;struct _tuple14*_tmp18F=t;_tmp190=*_tmp18F;
_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_tmp193=_tmp190.f3;_tmp194=_tmp190.f4;
_tmp195=_tmp190.f5;Cyc_Lex_register_typedef(_tmp191);{struct Cyc_Core_Opt*kind;
struct Cyc_Core_Opt*type;{void*_tmp196=_tmp193;struct Cyc_Core_Opt*_tmp198;_LLAF: {
struct Cyc_Absyn_Evar_struct*_tmp197=(struct Cyc_Absyn_Evar_struct*)_tmp196;if(
_tmp197->tag != 1)goto _LLB1;else{_tmp198=_tmp197->f1;}}_LLB0: type=0;if(_tmp198 == 
0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;else{kind=_tmp198;}goto _LLAE;_LLB1:;
_LLB2: kind=0;type=({struct Cyc_Core_Opt*_tmp199=_cycalloc(sizeof(*_tmp199));
_tmp199->v=_tmp193;_tmp199;});goto _LLAE;_LLAE:;}return Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Typedef_d_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A[0]=({
struct Cyc_Absyn_Typedef_d_struct _tmp19B;_tmp19B.tag=9;_tmp19B.f1=({struct Cyc_Absyn_Typedefdecl*
_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->name=_tmp191;_tmp19C->tvs=_tmp194;
_tmp19C->kind=kind;_tmp19C->defn=type;_tmp19C->atts=_tmp195;_tmp19C->tq=_tmp192;
_tmp19C;});_tmp19B;});_tmp19A;}),loc);};}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Decl_s_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D[0]=({
struct Cyc_Absyn_Decl_s_struct _tmp19E;_tmp19E.tag=12;_tmp19E.f1=d;_tmp19E.f2=s;
_tmp19E;});_tmp19D;}),d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct _RegionHandle _tmp19F=_new_region("mkrgn");struct _RegionHandle*mkrgn=&
_tmp19F;_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp1A1;struct Cyc_Absyn_Tqual
_tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_Parse_Declaration_spec*
_tmp1A0=ds;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.tq;_tmp1A3=_tmp1A1.type_specs;
_tmp1A4=_tmp1A1.attributes;if(_tmp1A2.loc == 0)_tmp1A2.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp1A5="inline is allowed only on function definitions";
_tag_dyneither(_tmp1A5,sizeof(char),47);}),loc);if(_tmp1A3 == 0){Cyc_Parse_err(({
const char*_tmp1A6="missing type specifiers in declaration";_tag_dyneither(
_tmp1A6,sizeof(char),39);}),loc);{struct Cyc_List_List*_tmp1A7=0;_npop_handler(0);
return _tmp1A7;};}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int istypedef=0;if(ds->sc
!= 0)switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v){
case Cyc_Parse_Typedef_sc: _LLB3: istypedef=1;break;case Cyc_Parse_Extern_sc: _LLB4: s=
Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc: _LLB5: s=Cyc_Absyn_ExternC;break;
case Cyc_Parse_Static_sc: _LLB6: s=Cyc_Absyn_Static;break;case Cyc_Parse_Auto_sc:
_LLB7: s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc: _LLB8: if(Cyc_Parse_no_register)
s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}break;case Cyc_Parse_Abstract_sc:
_LLB9: s=Cyc_Absyn_Abstract;break;}{struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*
_tmp1AA;struct _tuple0 _tmp1A8=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(mkrgn,mkrgn,ids);
_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmp1AA;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=
0;break;}}}{struct _tuple11 _tmp1AB=Cyc_Parse_collapse_type_specifiers(_tmp1A3,loc);
if(_tmp1A9 == 0){void*_tmp1AD;struct Cyc_Core_Opt*_tmp1AE;struct _tuple11 _tmp1AC=
_tmp1AB;_tmp1AD=_tmp1AC.f1;_tmp1AE=_tmp1AC.f2;if(_tmp1AE != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmp1AE->v;{void*_tmp1AF=d->r;struct Cyc_Absyn_Enumdecl*
_tmp1B1;struct Cyc_Absyn_Aggrdecl*_tmp1B3;struct Cyc_Absyn_Datatypedecl*_tmp1B5;
_LLBC: {struct Cyc_Absyn_Enum_d_struct*_tmp1B0=(struct Cyc_Absyn_Enum_d_struct*)
_tmp1AF;if(_tmp1B0->tag != 8)goto _LLBE;else{_tmp1B1=_tmp1B0->f1;}}_LLBD: _tmp1B1->sc=
s;if(_tmp1A4 != 0)Cyc_Parse_err(({const char*_tmp1B6="bad attributes on enum";
_tag_dyneither(_tmp1B6,sizeof(char),23);}),loc);goto _LLBB;_LLBE: {struct Cyc_Absyn_Aggr_d_struct*
_tmp1B2=(struct Cyc_Absyn_Aggr_d_struct*)_tmp1AF;if(_tmp1B2->tag != 6)goto _LLC0;
else{_tmp1B3=_tmp1B2->f1;}}_LLBF: _tmp1B3->sc=s;_tmp1B3->attributes=_tmp1A4;goto
_LLBB;_LLC0: {struct Cyc_Absyn_Datatype_d_struct*_tmp1B4=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1AF;if(_tmp1B4->tag != 7)goto _LLC2;else{_tmp1B5=_tmp1B4->f1;}}_LLC1: _tmp1B5->sc=
s;if(_tmp1A4 != 0)Cyc_Parse_err(({const char*_tmp1B7="bad attributes on datatype";
_tag_dyneither(_tmp1B7,sizeof(char),27);}),loc);goto _LLBB;_LLC2:;_LLC3: Cyc_Parse_err(({
const char*_tmp1B8="bad declaration";_tag_dyneither(_tmp1B8,sizeof(char),16);}),
loc);{struct Cyc_List_List*_tmp1B9=0;_npop_handler(0);return _tmp1B9;};_LLBB:;}{
struct Cyc_List_List*_tmp1BB=({struct Cyc_List_List*_tmp1BA=_cycalloc(sizeof(*
_tmp1BA));_tmp1BA->hd=d;_tmp1BA->tl=0;_tmp1BA;});_npop_handler(0);return _tmp1BB;};}
else{void*_tmp1BC=_tmp1AD;struct Cyc_Absyn_AggrInfo _tmp1BE;union Cyc_Absyn_AggrInfoU
_tmp1BF;struct _tuple3 _tmp1C0;enum Cyc_Absyn_AggrKind _tmp1C1;struct _tuple1*
_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_Absyn_DatatypeInfo _tmp1C5;union Cyc_Absyn_DatatypeInfoU
_tmp1C6;struct Cyc_Absyn_Datatypedecl**_tmp1C7;struct Cyc_Absyn_DatatypeInfo
_tmp1C9;union Cyc_Absyn_DatatypeInfoU _tmp1CA;struct Cyc_Absyn_UnknownDatatypeInfo
_tmp1CB;struct _tuple1*_tmp1CC;int _tmp1CD;struct Cyc_List_List*_tmp1CE;struct
_tuple1*_tmp1D0;struct Cyc_List_List*_tmp1D2;_LLC5: {struct Cyc_Absyn_AggrType_struct*
_tmp1BD=(struct Cyc_Absyn_AggrType_struct*)_tmp1BC;if(_tmp1BD->tag != 12)goto _LLC7;
else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BE.aggr_info;if((_tmp1BF.UnknownAggr).tag
!= 1)goto _LLC7;_tmp1C0=(struct _tuple3)(_tmp1BF.UnknownAggr).val;_tmp1C1=_tmp1C0.f1;
_tmp1C2=_tmp1C0.f2;_tmp1C3=_tmp1BE.targs;}}_LLC6: {struct Cyc_List_List*_tmp1D3=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1C3);struct Cyc_Absyn_Aggrdecl*_tmp1D4=({struct Cyc_Absyn_Aggrdecl*_tmp1DA=
_cycalloc(sizeof(*_tmp1DA));_tmp1DA->kind=_tmp1C1;_tmp1DA->sc=s;_tmp1DA->name=
_tmp1C2;_tmp1DA->tvs=_tmp1D3;_tmp1DA->impl=0;_tmp1DA->attributes=0;_tmp1DA;});
if(_tmp1A4 != 0)Cyc_Parse_err(({const char*_tmp1D5="bad attributes on type declaration";
_tag_dyneither(_tmp1D5,sizeof(char),35);}),loc);{struct Cyc_List_List*_tmp1D9=({
struct Cyc_List_List*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));
_tmp1D7[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp1D8;_tmp1D8.tag=6;_tmp1D8.f1=
_tmp1D4;_tmp1D8;});_tmp1D7;}),loc);_tmp1D6->tl=0;_tmp1D6;});_npop_handler(0);
return _tmp1D9;};}_LLC7: {struct Cyc_Absyn_DatatypeType_struct*_tmp1C4=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp1BC;if(_tmp1C4->tag != 3)goto _LLC9;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C5.datatype_info;
if((_tmp1C6.KnownDatatype).tag != 2)goto _LLC9;_tmp1C7=(struct Cyc_Absyn_Datatypedecl**)(
_tmp1C6.KnownDatatype).val;}}_LLC8: if(_tmp1A4 != 0)Cyc_Parse_err(({const char*
_tmp1DB="bad attributes on datatype";_tag_dyneither(_tmp1DB,sizeof(char),27);}),
loc);{struct Cyc_List_List*_tmp1DF=({struct Cyc_List_List*_tmp1DC=_cycalloc(
sizeof(*_tmp1DC));_tmp1DC->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_struct*
_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=({struct Cyc_Absyn_Datatype_d_struct
_tmp1DE;_tmp1DE.tag=7;_tmp1DE.f1=*_tmp1C7;_tmp1DE;});_tmp1DD;}),loc);_tmp1DC->tl=
0;_tmp1DC;});_npop_handler(0);return _tmp1DF;};_LLC9: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1C8=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1BC;if(_tmp1C8->tag != 3)goto
_LLCB;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C9.datatype_info;if((_tmp1CA.UnknownDatatype).tag
!= 1)goto _LLCB;_tmp1CB=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1CA.UnknownDatatype).val;
_tmp1CC=_tmp1CB.name;_tmp1CD=_tmp1CB.is_extensible;_tmp1CE=_tmp1C9.targs;}}_LLCA: {
struct Cyc_List_List*_tmp1E0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp1CE);struct Cyc_Absyn_Decl*_tmp1E1=
Cyc_Absyn_datatype_decl(s,_tmp1CC,_tmp1E0,0,_tmp1CD,loc);if(_tmp1A4 != 0)Cyc_Parse_err(({
const char*_tmp1E2="bad attributes on datatype";_tag_dyneither(_tmp1E2,sizeof(
char),27);}),loc);{struct Cyc_List_List*_tmp1E4=({struct Cyc_List_List*_tmp1E3=
_cycalloc(sizeof(*_tmp1E3));_tmp1E3->hd=_tmp1E1;_tmp1E3->tl=0;_tmp1E3;});
_npop_handler(0);return _tmp1E4;};}_LLCB: {struct Cyc_Absyn_EnumType_struct*
_tmp1CF=(struct Cyc_Absyn_EnumType_struct*)_tmp1BC;if(_tmp1CF->tag != 14)goto _LLCD;
else{_tmp1D0=_tmp1CF->f1;}}_LLCC: {struct Cyc_Absyn_Enumdecl*_tmp1E5=({struct Cyc_Absyn_Enumdecl*
_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->sc=s;_tmp1EC->name=_tmp1D0;_tmp1EC->fields=
0;_tmp1EC;});if(_tmp1A4 != 0)Cyc_Parse_err(({const char*_tmp1E6="bad attributes on enum";
_tag_dyneither(_tmp1E6,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp1EB=({
struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=({struct Cyc_Absyn_Decl*
_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->r=(void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp1EA;_tmp1EA.tag=8;_tmp1EA.f1=_tmp1E5;_tmp1EA;});_tmp1E9;});_tmp1E8->loc=loc;
_tmp1E8;});_tmp1E7->tl=0;_tmp1E7;});_npop_handler(0);return _tmp1EB;};}_LLCD: {
struct Cyc_Absyn_AnonEnumType_struct*_tmp1D1=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1BC;if(_tmp1D1->tag != 15)goto _LLCF;else{_tmp1D2=_tmp1D1->f1;}}_LLCE: {struct
Cyc_Absyn_Enumdecl*_tmp1ED=({struct Cyc_Absyn_Enumdecl*_tmp1F4=_cycalloc(sizeof(*
_tmp1F4));_tmp1F4->sc=s;_tmp1F4->name=Cyc_Parse_gensym_enum();_tmp1F4->fields=({
struct Cyc_Core_Opt*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->v=_tmp1D2;
_tmp1F5;});_tmp1F4;});if(_tmp1A4 != 0)Cyc_Parse_err(({const char*_tmp1EE="bad attributes on enum";
_tag_dyneither(_tmp1EE,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp1F3=({
struct Cyc_List_List*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->hd=({struct Cyc_Absyn_Decl*
_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->r=(void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp1F2;_tmp1F2.tag=8;_tmp1F2.f1=_tmp1ED;_tmp1F2;});_tmp1F1;});_tmp1F0->loc=loc;
_tmp1F0;});_tmp1EF->tl=0;_tmp1EF;});_npop_handler(0);return _tmp1F3;};}_LLCF:;
_LLD0: Cyc_Parse_err(({const char*_tmp1F6="missing declarator";_tag_dyneither(
_tmp1F6,sizeof(char),19);}),loc);{struct Cyc_List_List*_tmp1F7=0;_npop_handler(0);
return _tmp1F7;};_LLC4:;}}else{void*t=_tmp1AB.f1;struct Cyc_List_List*_tmp1F8=Cyc_Parse_apply_tmss(
mkrgn,_tmp1A2,t,_tmp1A9,_tmp1A4);if(istypedef){if(!exps_empty)Cyc_Parse_err(({
const char*_tmp1F9="initializer in typedef declaration";_tag_dyneither(_tmp1F9,
sizeof(char),35);}),loc);{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple14*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp1F8);if(_tmp1AB.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp1AB.f2)->v;{void*_tmp1FA=d->r;struct Cyc_Absyn_Aggrdecl*_tmp1FC;struct Cyc_Absyn_Datatypedecl*
_tmp1FE;struct Cyc_Absyn_Enumdecl*_tmp200;_LLD2: {struct Cyc_Absyn_Aggr_d_struct*
_tmp1FB=(struct Cyc_Absyn_Aggr_d_struct*)_tmp1FA;if(_tmp1FB->tag != 6)goto _LLD4;
else{_tmp1FC=_tmp1FB->f1;}}_LLD3: _tmp1FC->sc=s;_tmp1FC->attributes=_tmp1A4;
_tmp1A4=0;goto _LLD1;_LLD4: {struct Cyc_Absyn_Datatype_d_struct*_tmp1FD=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1FA;if(_tmp1FD->tag != 7)goto _LLD6;else{_tmp1FE=_tmp1FD->f1;}}_LLD5: _tmp1FE->sc=
s;goto _LLD1;_LLD6: {struct Cyc_Absyn_Enum_d_struct*_tmp1FF=(struct Cyc_Absyn_Enum_d_struct*)
_tmp1FA;if(_tmp1FF->tag != 8)goto _LLD8;else{_tmp200=_tmp1FF->f1;}}_LLD7: _tmp200->sc=
s;goto _LLD1;_LLD8:;_LLD9: Cyc_Parse_err(({const char*_tmp201="declaration within typedef is not a struct, union, or datatype";
_tag_dyneither(_tmp201,sizeof(char),63);}),loc);goto _LLD1;_LLD1:;}decls=({struct
Cyc_List_List*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->hd=d;_tmp202->tl=
decls;_tmp202;});}{struct Cyc_List_List*_tmp203=decls;_npop_handler(0);return
_tmp203;};};}else{if(_tmp1AB.f2 != 0)Cyc_Parse_unimp(({const char*_tmp204="nested type declaration within declarator";
_tag_dyneither(_tmp204,sizeof(char),42);}),loc);{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp205=_tmp1F8;for(0;_tmp205 != 0;(_tmp205=_tmp205->tl,
_tmp1AA=_tmp1AA->tl)){struct _tuple14 _tmp207;struct _tuple1*_tmp208;struct Cyc_Absyn_Tqual
_tmp209;void*_tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20C;
struct _tuple14*_tmp206=(struct _tuple14*)_tmp205->hd;_tmp207=*_tmp206;_tmp208=
_tmp207.f1;_tmp209=_tmp207.f2;_tmp20A=_tmp207.f3;_tmp20B=_tmp207.f4;_tmp20C=
_tmp207.f5;if(_tmp20B != 0)({void*_tmp20D=0;Cyc_Tcutil_warn(loc,({const char*
_tmp20E="bad type params, ignoring";_tag_dyneither(_tmp20E,sizeof(char),26);}),
_tag_dyneither(_tmp20D,sizeof(void*),0));});if(_tmp1AA == 0)({void*_tmp20F=0;((
int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp210="unexpected NULL in parse!";
_tag_dyneither(_tmp210,sizeof(char),26);}),_tag_dyneither(_tmp20F,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp211=(struct Cyc_Absyn_Exp*)_tmp1AA->hd;struct Cyc_Absyn_Vardecl*
_tmp212=Cyc_Absyn_new_vardecl(_tmp208,_tmp20A,_tmp211);_tmp212->tq=_tmp209;
_tmp212->sc=s;_tmp212->attributes=_tmp20C;{struct Cyc_Absyn_Decl*_tmp213=({struct
Cyc_Absyn_Decl*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_Var_d_struct
_tmp217;_tmp217.tag=0;_tmp217.f1=_tmp212;_tmp217;});_tmp216;});_tmp215->loc=loc;
_tmp215;});decls=({struct Cyc_List_List*_tmp214=_cycalloc(sizeof(*_tmp214));
_tmp214->hd=_tmp213;_tmp214->tl=decls;_tmp214;});};};}}{struct Cyc_List_List*
_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);
_npop_handler(0);return _tmp218;};};}}};};};};};_pop_region(mkrgn);}static enum 
Cyc_Absyn_Kind Cyc_Parse_id_to_kind(struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct
_dyneither_ptr)s)== 2)switch(*((const char*)_check_dyneither_subscript(s,sizeof(
char),0))){case 'A': _LLDA: return Cyc_Absyn_AnyKind;case 'M': _LLDB: return Cyc_Absyn_MemKind;
case 'B': _LLDC: return Cyc_Absyn_BoxKind;case 'R': _LLDD: return Cyc_Absyn_RgnKind;case
'U': _LLDE: if(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))== 'R')
return Cyc_Absyn_UniqueRgnKind;else{break;}case 'T': _LLDF: if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))== 'R')return Cyc_Absyn_TopRgnKind;
else{break;}case 'E': _LLE0: return Cyc_Absyn_EffKind;case 'I': _LLE1: return Cyc_Absyn_IntKind;
default: _LLE2: break;}Cyc_Parse_err((struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp21C;_tmp21C.tag=1;_tmp21C.f1=(unsigned long)((int)Cyc_strlen((struct
_dyneither_ptr)s));({struct Cyc_String_pa_struct _tmp21B;_tmp21B.tag=0;_tmp21B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp219[2]={& _tmp21B,&
_tmp21C};Cyc_aprintf(({const char*_tmp21A="bad kind: %s; strlen=%d";
_tag_dyneither(_tmp21A,sizeof(char),24);}),_tag_dyneither(_tmp219,sizeof(void*),
2));});});}),loc);return Cyc_Absyn_BoxKind;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(
struct Cyc_Absyn_Pat*p){void*_tmp21D=p->r;struct _tuple1*_tmp21F;struct Cyc_Absyn_Vardecl*
_tmp221;struct Cyc_Absyn_Pat*_tmp222;struct Cyc_Absyn_Pat _tmp223;void*_tmp224;
struct Cyc_Absyn_Pat*_tmp227;enum Cyc_Absyn_Sign _tmp22A;int _tmp22B;char _tmp22D;
struct _dyneither_ptr _tmp22F;struct _tuple1*_tmp231;struct Cyc_List_List*_tmp232;
int _tmp233;struct Cyc_Absyn_Exp*_tmp235;_LLE5: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp21E=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp21D;if(_tmp21E->tag != 14)goto
_LLE7;else{_tmp21F=_tmp21E->f1;}}_LLE6: return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp237;_tmp237.tag=2;_tmp237.f1=_tmp21F;
_tmp237;});_tmp236;}),p->loc);_LLE7: {struct Cyc_Absyn_Reference_p_struct*_tmp220=(
struct Cyc_Absyn_Reference_p_struct*)_tmp21D;if(_tmp220->tag != 2)goto _LLE9;else{
_tmp221=_tmp220->f1;_tmp222=_tmp220->f2;_tmp223=*_tmp222;_tmp224=_tmp223.r;{
struct Cyc_Absyn_Wild_p_struct*_tmp225=(struct Cyc_Absyn_Wild_p_struct*)_tmp224;
if(_tmp225->tag != 0)goto _LLE9;};}}_LLE8: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp238=_cycalloc(sizeof(*_tmp238));
_tmp238[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp239;_tmp239.tag=2;_tmp239.f1=
_tmp221->name;_tmp239;});_tmp238;}),p->loc),p->loc);_LLE9: {struct Cyc_Absyn_Pointer_p_struct*
_tmp226=(struct Cyc_Absyn_Pointer_p_struct*)_tmp21D;if(_tmp226->tag != 5)goto _LLEB;
else{_tmp227=_tmp226->f1;}}_LLEA: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp227),p->loc);_LLEB: {struct Cyc_Absyn_Null_p_struct*_tmp228=(struct Cyc_Absyn_Null_p_struct*)
_tmp21D;if(_tmp228->tag != 8)goto _LLED;}_LLEC: return Cyc_Absyn_null_exp(p->loc);
_LLED: {struct Cyc_Absyn_Int_p_struct*_tmp229=(struct Cyc_Absyn_Int_p_struct*)
_tmp21D;if(_tmp229->tag != 9)goto _LLEF;else{_tmp22A=_tmp229->f1;_tmp22B=_tmp229->f2;}}
_LLEE: return Cyc_Absyn_int_exp(_tmp22A,_tmp22B,p->loc);_LLEF: {struct Cyc_Absyn_Char_p_struct*
_tmp22C=(struct Cyc_Absyn_Char_p_struct*)_tmp21D;if(_tmp22C->tag != 10)goto _LLF1;
else{_tmp22D=_tmp22C->f1;}}_LLF0: return Cyc_Absyn_char_exp(_tmp22D,p->loc);_LLF1: {
struct Cyc_Absyn_Float_p_struct*_tmp22E=(struct Cyc_Absyn_Float_p_struct*)_tmp21D;
if(_tmp22E->tag != 11)goto _LLF3;else{_tmp22F=_tmp22E->f1;}}_LLF2: return Cyc_Absyn_float_exp(
_tmp22F,p->loc);_LLF3: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp230=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp21D;if(_tmp230->tag != 15)goto _LLF5;else{_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;
_tmp233=_tmp230->f3;if(_tmp233 != 0)goto _LLF5;}}_LLF4: {struct Cyc_Absyn_Exp*e1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp23A=_cycalloc(
sizeof(*_tmp23A));_tmp23A[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp23B;_tmp23B.tag=
2;_tmp23B.f1=_tmp231;_tmp23B;});_tmp23A;}),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp232);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLF5: {struct Cyc_Absyn_Exp_p_struct*_tmp234=(struct Cyc_Absyn_Exp_p_struct*)
_tmp21D;if(_tmp234->tag != 16)goto _LLF7;else{_tmp235=_tmp234->f1;}}_LLF6: return
_tmp235;_LLF7:;_LLF8: Cyc_Parse_err(({const char*_tmp23C="cannot mix patterns and expressions in case";
_tag_dyneither(_tmp23C,sizeof(char),44);}),p->loc);return Cyc_Absyn_null_exp(p->loc);
_LLE4:;}union Cyc_YYSTYPE;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};
struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{
int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;
struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*
val;};struct _tuple19{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple19*
val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple20{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple20*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple21{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple21*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec*val;};
struct _tuple22{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Parse_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator*val;};
struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum 
Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*
val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple23{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple24{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple24*val;};
struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{
int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;enum Cyc_Absyn_Kind val;};
struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct
_union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct
_union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{
int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};
struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct
_union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct
_union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok
Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_YY1
YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct
_union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;
struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9
YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct
_union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14
YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct
_union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19
YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct
_union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24
YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct
_union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29
YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct
_union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34
YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct
_union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39
YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct
_union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44
YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct
_union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49
YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct
_union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp23E[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp23E,
_tmp23E,_tmp23E + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct
_tuple6 yyzzz;{union Cyc_YYSTYPE _tmp23F=yy1;struct _tuple6 _tmp240;_LLFA: if((_tmp23F.Int_tok).tag
!= 1)goto _LLFC;_tmp240=(struct _tuple6)(_tmp23F.Int_tok).val;_LLFB: yyzzz=_tmp240;
goto _LLF9;_LLFC:;_LLFD:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLF9:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){return({union Cyc_YYSTYPE
_tmp241;(_tmp241.Int_tok).val=yy1;(_tmp241.Int_tok).tag=1;_tmp241;});}static char
_tmp243[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{
_tmp243,_tmp243,_tmp243 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){char
yyzzz;{union Cyc_YYSTYPE _tmp244=yy1;char _tmp245;_LLFF: if((_tmp244.Char_tok).tag != 
2)goto _LL101;_tmp245=(char)(_tmp244.Char_tok).val;_LL100: yyzzz=_tmp245;goto _LLFE;
_LL101:;_LL102:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLFE:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp246;(_tmp246.Char_tok).val=
yy1;(_tmp246.Char_tok).tag=2;_tmp246;});}static char _tmp248[13]="string_t<`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp248,_tmp248,_tmp248 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp249=yy1;struct _dyneither_ptr
_tmp24A;_LL104: if((_tmp249.String_tok).tag != 3)goto _LL106;_tmp24A=(struct
_dyneither_ptr)(_tmp249.String_tok).val;_LL105: yyzzz=_tmp24A;goto _LL103;_LL106:;
_LL107:(int)_throw((void*)& Cyc_yyfail_String_tok);_LL103:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp24B;(_tmp24B.String_tok).val=
yy1;(_tmp24B.String_tok).tag=3;_tmp24B;});}static char _tmp24D[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp24D,
_tmp24D,_tmp24D + 56}};struct _tuple19*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp24E=yy1;struct _tuple19*_tmp24F;_LL109: if((
_tmp24E.YY1).tag != 6)goto _LL10B;_tmp24F=(struct _tuple19*)(_tmp24E.YY1).val;
_LL10A: yyzzz=_tmp24F;goto _LL108;_LL10B:;_LL10C:(int)_throw((void*)& Cyc_yyfail_YY1);
_LL108:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple19*yy1){return({union
Cyc_YYSTYPE _tmp250;(_tmp250.YY1).val=yy1;(_tmp250.YY1).tag=6;_tmp250;});}static
char _tmp252[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={
Cyc_Core_Failure,{_tmp252,_tmp252,_tmp252 + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(
union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp253=
yy1;union Cyc_Absyn_Constraint*_tmp254;_LL10E: if((_tmp253.YY2).tag != 7)goto _LL110;
_tmp254=(union Cyc_Absyn_Constraint*)(_tmp253.YY2).val;_LL10F: yyzzz=_tmp254;goto
_LL10D;_LL110:;_LL111:(int)_throw((void*)& Cyc_yyfail_YY2);_LL10D:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE
_tmp255;(_tmp255.YY2).val=yy1;(_tmp255.YY2).tag=7;_tmp255;});}static char _tmp257[
6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{
_tmp257,_tmp257,_tmp257 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp258=yy1;struct Cyc_Absyn_Exp*
_tmp259;_LL113: if((_tmp258.YY3).tag != 8)goto _LL115;_tmp259=(struct Cyc_Absyn_Exp*)(
_tmp258.YY3).val;_LL114: yyzzz=_tmp259;goto _LL112;_LL115:;_LL116:(int)_throw((
void*)& Cyc_yyfail_YY3);_LL112:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY3(struct Cyc_Absyn_Exp*
yy1){return({union Cyc_YYSTYPE _tmp25A;(_tmp25A.YY3).val=yy1;(_tmp25A.YY3).tag=8;
_tmp25A;});}static char _tmp25C[10]="exp_opt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 10}};struct Cyc_Absyn_Exp*
Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE
_tmp25D=yy1;struct Cyc_Absyn_Exp*_tmp25E;_LL118: if((_tmp25D.YY4).tag != 9)goto
_LL11A;_tmp25E=(struct Cyc_Absyn_Exp*)(_tmp25D.YY4).val;_LL119: yyzzz=_tmp25E;goto
_LL117;_LL11A:;_LL11B:(int)_throw((void*)& Cyc_yyfail_YY4);_LL117:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp25F;(
_tmp25F.YY4).val=yy1;(_tmp25F.YY4).tag=9;_tmp25F;});}static char _tmp261[17]="list_t<exp_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp261,
_tmp261,_tmp261 + 17}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp262=yy1;struct Cyc_List_List*
_tmp263;_LL11D: if((_tmp262.YY5).tag != 10)goto _LL11F;_tmp263=(struct Cyc_List_List*)(
_tmp262.YY5).val;_LL11E: yyzzz=_tmp263;goto _LL11C;_LL11F:;_LL120:(int)_throw((
void*)& Cyc_yyfail_YY5);_LL11C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp264;(_tmp264.YY5).val=yy1;(_tmp264.YY5).tag=10;
_tmp264;});}static char _tmp266[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp266,
_tmp266,_tmp266 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp267=yy1;struct Cyc_List_List*
_tmp268;_LL122: if((_tmp267.YY6).tag != 11)goto _LL124;_tmp268=(struct Cyc_List_List*)(
_tmp267.YY6).val;_LL123: yyzzz=_tmp268;goto _LL121;_LL124:;_LL125:(int)_throw((
void*)& Cyc_yyfail_YY6);_LL121:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp269;(_tmp269.YY6).val=yy1;(_tmp269.YY6).tag=11;
_tmp269;});}static char _tmp26B[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp26B,_tmp26B,_tmp26B + 9}};enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp26C=yy1;enum Cyc_Absyn_Primop _tmp26D;_LL127: if((_tmp26C.YY7).tag != 12)goto
_LL129;_tmp26D=(enum Cyc_Absyn_Primop)(_tmp26C.YY7).val;_LL128: yyzzz=_tmp26D;
goto _LL126;_LL129:;_LL12A:(int)_throw((void*)& Cyc_yyfail_YY7);_LL126:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE
_tmp26E;(_tmp26E.YY7).val=yy1;(_tmp26E.YY7).tag=12;_tmp26E;});}static char _tmp270[
19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{
_tmp270,_tmp270,_tmp270 + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE
yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp271=yy1;struct Cyc_Core_Opt*
_tmp272;_LL12C: if((_tmp271.YY8).tag != 13)goto _LL12E;_tmp272=(struct Cyc_Core_Opt*)(
_tmp271.YY8).val;_LL12D: yyzzz=_tmp272;goto _LL12B;_LL12E:;_LL12F:(int)_throw((
void*)& Cyc_yyfail_YY8);_LL12B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*
yy1){return({union Cyc_YYSTYPE _tmp273;(_tmp273.YY8).val=yy1;(_tmp273.YY8).tag=13;
_tmp273;});}static char _tmp275[7]="qvar_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{_tmp275,_tmp275,_tmp275 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(
union Cyc_YYSTYPE yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp276=yy1;struct
_tuple1*_tmp277;_LL131: if((_tmp276.QualId_tok).tag != 5)goto _LL133;_tmp277=(
struct _tuple1*)(_tmp276.QualId_tok).val;_LL132: yyzzz=_tmp277;goto _LL130;_LL133:;
_LL134:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL130:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_QualId_tok(struct _tuple1*yy1){return({union Cyc_YYSTYPE _tmp278;(_tmp278.QualId_tok).val=
yy1;(_tmp278.QualId_tok).tag=5;_tmp278;});}static char _tmp27A[7]="stmt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp27A,_tmp27A,
_tmp27A + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*
yyzzz;{union Cyc_YYSTYPE _tmp27B=yy1;struct Cyc_Absyn_Stmt*_tmp27C;_LL136: if((
_tmp27B.YY9).tag != 14)goto _LL138;_tmp27C=(struct Cyc_Absyn_Stmt*)(_tmp27B.YY9).val;
_LL137: yyzzz=_tmp27C;goto _LL135;_LL138:;_LL139:(int)_throw((void*)& Cyc_yyfail_YY9);
_LL135:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){return({
union Cyc_YYSTYPE _tmp27D;(_tmp27D.YY9).val=yy1;(_tmp27D.YY9).tag=14;_tmp27D;});}
static char _tmp27F[27]="list_t<switch_clause_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp27F,_tmp27F,_tmp27F + 27}};struct Cyc_List_List*
Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp280=yy1;struct Cyc_List_List*_tmp281;_LL13B: if((_tmp280.YY10).tag != 15)goto
_LL13D;_tmp281=(struct Cyc_List_List*)(_tmp280.YY10).val;_LL13C: yyzzz=_tmp281;
goto _LL13A;_LL13D:;_LL13E:(int)_throw((void*)& Cyc_yyfail_YY10);_LL13A:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp282;(_tmp282.YY10).val=yy1;(_tmp282.YY10).tag=15;_tmp282;});}static char
_tmp284[6]="pat_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{
_tmp284,_tmp284,_tmp284 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE _tmp285=yy1;struct Cyc_Absyn_Pat*
_tmp286;_LL140: if((_tmp285.YY11).tag != 16)goto _LL142;_tmp286=(struct Cyc_Absyn_Pat*)(
_tmp285.YY11).val;_LL141: yyzzz=_tmp286;goto _LL13F;_LL142:;_LL143:(int)_throw((
void*)& Cyc_yyfail_YY11);_LL13F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*
yy1){return({union Cyc_YYSTYPE _tmp287;(_tmp287.YY11).val=yy1;(_tmp287.YY11).tag=
16;_tmp287;});}static char _tmp289[28]="$(list_t<pat_t,`H>,bool)@`H";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp289,_tmp289,
_tmp289 + 28}};struct _tuple20*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){struct _tuple20*
yyzzz;{union Cyc_YYSTYPE _tmp28A=yy1;struct _tuple20*_tmp28B;_LL145: if((_tmp28A.YY12).tag
!= 17)goto _LL147;_tmp28B=(struct _tuple20*)(_tmp28A.YY12).val;_LL146: yyzzz=
_tmp28B;goto _LL144;_LL147:;_LL148:(int)_throw((void*)& Cyc_yyfail_YY12);_LL144:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(struct _tuple20*yy1){return({union Cyc_YYSTYPE
_tmp28C;(_tmp28C.YY12).val=yy1;(_tmp28C.YY12).tag=17;_tmp28C;});}static char
_tmp28E[17]="list_t<pat_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={
Cyc_Core_Failure,{_tmp28E,_tmp28E,_tmp28E + 17}};struct Cyc_List_List*Cyc_yyget_YY13(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp28F=yy1;
struct Cyc_List_List*_tmp290;_LL14A: if((_tmp28F.YY13).tag != 18)goto _LL14C;_tmp290=(
struct Cyc_List_List*)(_tmp28F.YY13).val;_LL14B: yyzzz=_tmp290;goto _LL149;_LL14C:;
_LL14D:(int)_throw((void*)& Cyc_yyfail_YY13);_LL149:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp291;(_tmp291.YY13).val=
yy1;(_tmp291.YY13).tag=18;_tmp291;});}static char _tmp293[36]="$(list_t<designator_t,`H>,pat_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp293,
_tmp293,_tmp293 + 36}};struct _tuple21*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct
_tuple21*yyzzz;{union Cyc_YYSTYPE _tmp294=yy1;struct _tuple21*_tmp295;_LL14F: if((
_tmp294.YY14).tag != 19)goto _LL151;_tmp295=(struct _tuple21*)(_tmp294.YY14).val;
_LL150: yyzzz=_tmp295;goto _LL14E;_LL151:;_LL152:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL14E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY14(struct _tuple21*yy1){return({union
Cyc_YYSTYPE _tmp296;(_tmp296.YY14).val=yy1;(_tmp296.YY14).tag=19;_tmp296;});}
static char _tmp298[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp298,_tmp298,
_tmp298 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp299=yy1;struct Cyc_List_List*_tmp29A;_LL154: if((
_tmp299.YY15).tag != 20)goto _LL156;_tmp29A=(struct Cyc_List_List*)(_tmp299.YY15).val;
_LL155: yyzzz=_tmp29A;goto _LL153;_LL156:;_LL157:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL153:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp29B;(_tmp29B.YY15).val=yy1;(_tmp29B.YY15).tag=20;_tmp29B;});}
static char _tmp29D[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp29D,
_tmp29D,_tmp29D + 58}};struct _tuple20*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple20*yyzzz;{union Cyc_YYSTYPE _tmp29E=yy1;struct _tuple20*_tmp29F;_LL159: if((
_tmp29E.YY16).tag != 21)goto _LL15B;_tmp29F=(struct _tuple20*)(_tmp29E.YY16).val;
_LL15A: yyzzz=_tmp29F;goto _LL158;_LL15B:;_LL15C:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL158:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple20*yy1){return({union
Cyc_YYSTYPE _tmp2A0;(_tmp2A0.YY16).val=yy1;(_tmp2A0.YY16).tag=21;_tmp2A0;});}
static char _tmp2A2[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp2A2,_tmp2A2,_tmp2A2 + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp2A3=yy1;
struct Cyc_Absyn_Fndecl*_tmp2A4;_LL15E: if((_tmp2A3.YY17).tag != 22)goto _LL160;
_tmp2A4=(struct Cyc_Absyn_Fndecl*)(_tmp2A3.YY17).val;_LL15F: yyzzz=_tmp2A4;goto
_LL15D;_LL160:;_LL161:(int)_throw((void*)& Cyc_yyfail_YY17);_LL15D:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE
_tmp2A5;(_tmp2A5.YY17).val=yy1;(_tmp2A5.YY17).tag=22;_tmp2A5;});}static char
_tmp2A7[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp2A7,_tmp2A7,_tmp2A7 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A8=yy1;
struct Cyc_List_List*_tmp2A9;_LL163: if((_tmp2A8.YY18).tag != 23)goto _LL165;_tmp2A9=(
struct Cyc_List_List*)(_tmp2A8.YY18).val;_LL164: yyzzz=_tmp2A9;goto _LL162;_LL165:;
_LL166:(int)_throw((void*)& Cyc_yyfail_YY18);_LL162:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2AA;(_tmp2AA.YY18).val=
yy1;(_tmp2AA.YY18).tag=23;_tmp2AA;});}static char _tmp2AC[12]="decl_spec_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp2AC,_tmp2AC,
_tmp2AC + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp2AD=yy1;struct
Cyc_Parse_Declaration_spec*_tmp2AE;_LL168: if((_tmp2AD.YY19).tag != 24)goto _LL16A;
_tmp2AE=(struct Cyc_Parse_Declaration_spec*)(_tmp2AD.YY19).val;_LL169: yyzzz=
_tmp2AE;goto _LL167;_LL16A:;_LL16B:(int)_throw((void*)& Cyc_yyfail_YY19);_LL167:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
return({union Cyc_YYSTYPE _tmp2AF;(_tmp2AF.YY19).val=yy1;(_tmp2AF.YY19).tag=24;
_tmp2AF;});}static char _tmp2B1[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 29}};struct _tuple22*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple22*yyzzz;{union Cyc_YYSTYPE
_tmp2B2=yy1;struct _tuple22*_tmp2B3;_LL16D: if((_tmp2B2.YY20).tag != 25)goto _LL16F;
_tmp2B3=(struct _tuple22*)(_tmp2B2.YY20).val;_LL16E: yyzzz=_tmp2B3;goto _LL16C;
_LL16F:;_LL170:(int)_throw((void*)& Cyc_yyfail_YY20);_LL16C:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp2B4;(_tmp2B4.YY20).val=
yy1;(_tmp2B4.YY20).tag=25;_tmp2B4;});}static char _tmp2B6[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp2B6,
_tmp2B6,_tmp2B6 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2B7=yy1;struct Cyc_List_List*
_tmp2B8;_LL172: if((_tmp2B7.YY21).tag != 26)goto _LL174;_tmp2B8=(struct Cyc_List_List*)(
_tmp2B7.YY21).val;_LL173: yyzzz=_tmp2B8;goto _LL171;_LL174:;_LL175:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL171:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp2B9;(_tmp2B9.YY21).val=yy1;(_tmp2B9.YY21).tag=
26;_tmp2B9;});}static char _tmp2BB[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp2BB,_tmp2BB,_tmp2BB + 16}};enum Cyc_Parse_Storage_class
Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class yyzzz;{union Cyc_YYSTYPE
_tmp2BC=yy1;enum Cyc_Parse_Storage_class _tmp2BD;_LL177: if((_tmp2BC.YY22).tag != 
27)goto _LL179;_tmp2BD=(enum Cyc_Parse_Storage_class)(_tmp2BC.YY22).val;_LL178:
yyzzz=_tmp2BD;goto _LL176;_LL179:;_LL17A:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL176:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){
return({union Cyc_YYSTYPE _tmp2BE;(_tmp2BE.YY22).val=yy1;(_tmp2BE.YY22).tag=27;
_tmp2BE;});}static char _tmp2C0[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2C1=yy1;void*_tmp2C2;_LL17C:
if((_tmp2C1.YY23).tag != 28)goto _LL17E;_tmp2C2=(void*)(_tmp2C1.YY23).val;_LL17D:
yyzzz=_tmp2C2;goto _LL17B;_LL17E:;_LL17F:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL17B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(void*yy1){return({union Cyc_YYSTYPE
_tmp2C3;(_tmp2C3.YY23).val=yy1;(_tmp2C3.YY23).tag=28;_tmp2C3;});}static char
_tmp2C5[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp2C5,_tmp2C5,_tmp2C5 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind yyzzz;{union Cyc_YYSTYPE _tmp2C6=yy1;
enum Cyc_Absyn_AggrKind _tmp2C7;_LL181: if((_tmp2C6.YY24).tag != 29)goto _LL183;
_tmp2C7=(enum Cyc_Absyn_AggrKind)(_tmp2C6.YY24).val;_LL182: yyzzz=_tmp2C7;goto
_LL180;_LL183:;_LL184:(int)_throw((void*)& Cyc_yyfail_YY24);_LL180:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE
_tmp2C8;(_tmp2C8.YY24).val=yy1;(_tmp2C8.YY24).tag=29;_tmp2C8;});}static char
_tmp2CA[8]="tqual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{
_tmp2CA,_tmp2CA,_tmp2CA + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp2CB=yy1;struct Cyc_Absyn_Tqual
_tmp2CC;_LL186: if((_tmp2CB.YY25).tag != 30)goto _LL188;_tmp2CC=(struct Cyc_Absyn_Tqual)(
_tmp2CB.YY25).val;_LL187: yyzzz=_tmp2CC;goto _LL185;_LL188:;_LL189:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL185:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1){return({union Cyc_YYSTYPE _tmp2CD;(_tmp2CD.YY25).val=yy1;(_tmp2CD.YY25).tag=
30;_tmp2CD;});}static char _tmp2CF[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2CF,_tmp2CF,_tmp2CF + 23}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2D0=yy1;struct Cyc_List_List*_tmp2D1;_LL18B: if((_tmp2D0.YY26).tag != 31)goto
_LL18D;_tmp2D1=(struct Cyc_List_List*)(_tmp2D0.YY26).val;_LL18C: yyzzz=_tmp2D1;
goto _LL18A;_LL18D:;_LL18E:(int)_throw((void*)& Cyc_yyfail_YY26);_LL18A:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp2D2;(_tmp2D2.YY26).val=yy1;(_tmp2D2.YY26).tag=31;_tmp2D2;});}static char
_tmp2D4[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2D4,_tmp2D4,_tmp2D4 + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2D5=yy1;struct Cyc_List_List*_tmp2D6;_LL190: if((_tmp2D5.YY27).tag != 32)goto
_LL192;_tmp2D6=(struct Cyc_List_List*)(_tmp2D5.YY27).val;_LL191: yyzzz=_tmp2D6;
goto _LL18F;_LL192:;_LL193:(int)_throw((void*)& Cyc_yyfail_YY27);_LL18F:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp2D7;(_tmp2D7.YY27).val=yy1;(_tmp2D7.YY27).tag=32;_tmp2D7;});}static char
_tmp2D9[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2D9,_tmp2D9,_tmp2D9 + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2DA=yy1;struct Cyc_List_List*_tmp2DB;_LL195: if((_tmp2DA.YY28).tag != 33)goto
_LL197;_tmp2DB=(struct Cyc_List_List*)(_tmp2DA.YY28).val;_LL196: yyzzz=_tmp2DB;
goto _LL194;_LL197:;_LL198:(int)_throw((void*)& Cyc_yyfail_YY28);_LL194:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp2DC;(_tmp2DC.YY28).val=yy1;(_tmp2DC.YY28).tag=33;_tmp2DC;});}static char
_tmp2DE[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp2DE,_tmp2DE,_tmp2DE + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp2DF=
yy1;struct Cyc_Parse_Declarator*_tmp2E0;_LL19A: if((_tmp2DF.YY29).tag != 34)goto
_LL19C;_tmp2E0=(struct Cyc_Parse_Declarator*)(_tmp2DF.YY29).val;_LL19B: yyzzz=
_tmp2E0;goto _LL199;_LL19C:;_LL19D:(int)_throw((void*)& Cyc_yyfail_YY29);_LL199:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){return({
union Cyc_YYSTYPE _tmp2E1;(_tmp2E1.YY29).val=yy1;(_tmp2E1.YY29).tag=34;_tmp2E1;});}
static char _tmp2E3[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp2E4=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2E5;_LL19F:
if((_tmp2E4.YY30).tag != 35)goto _LL1A1;_tmp2E5=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp2E4.YY30).val;_LL1A0: yyzzz=_tmp2E5;goto _LL19E;_LL1A1:;_LL1A2:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL19E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){return({union Cyc_YYSTYPE _tmp2E6;(_tmp2E6.YY30).val=yy1;(_tmp2E6.YY30).tag=
35;_tmp2E6;});}static char _tmp2E8[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2E8,_tmp2E8,_tmp2E8 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2E9=yy1;int _tmp2EA;_LL1A4: if((
_tmp2E9.YY31).tag != 36)goto _LL1A6;_tmp2EA=(int)(_tmp2E9.YY31).val;_LL1A5: yyzzz=
_tmp2EA;goto _LL1A3;_LL1A6:;_LL1A7:(int)_throw((void*)& Cyc_yyfail_YY31);_LL1A3:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp2EB;(
_tmp2EB.YY31).val=yy1;(_tmp2EB.YY31).tag=36;_tmp2EB;});}static char _tmp2ED[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2ED,
_tmp2ED,_tmp2ED + 8}};enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2EE=yy1;enum Cyc_Absyn_Scope
_tmp2EF;_LL1A9: if((_tmp2EE.YY32).tag != 37)goto _LL1AB;_tmp2EF=(enum Cyc_Absyn_Scope)(
_tmp2EE.YY32).val;_LL1AA: yyzzz=_tmp2EF;goto _LL1A8;_LL1AB:;_LL1AC:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL1A8:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1){return({union Cyc_YYSTYPE _tmp2F0;(_tmp2F0.YY32).val=yy1;(_tmp2F0.YY32).tag=
37;_tmp2F0;});}static char _tmp2F2[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2F2,_tmp2F2,_tmp2F2 + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp2F3=yy1;struct Cyc_Absyn_Datatypefield*_tmp2F4;_LL1AE: if((_tmp2F3.YY33).tag
!= 38)goto _LL1B0;_tmp2F4=(struct Cyc_Absyn_Datatypefield*)(_tmp2F3.YY33).val;
_LL1AF: yyzzz=_tmp2F4;goto _LL1AD;_LL1B0:;_LL1B1:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1AD:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
return({union Cyc_YYSTYPE _tmp2F5;(_tmp2F5.YY33).val=yy1;(_tmp2F5.YY33).tag=38;
_tmp2F5;});}static char _tmp2F7[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,_tmp2F7 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2F8=yy1;struct Cyc_List_List*_tmp2F9;_LL1B3: if((_tmp2F8.YY34).tag != 39)goto
_LL1B5;_tmp2F9=(struct Cyc_List_List*)(_tmp2F8.YY34).val;_LL1B4: yyzzz=_tmp2F9;
goto _LL1B2;_LL1B5:;_LL1B6:(int)_throw((void*)& Cyc_yyfail_YY34);_LL1B2:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp2FA;(_tmp2FA.YY34).val=yy1;(_tmp2FA.YY34).tag=39;_tmp2FA;});}static char
_tmp2FC[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2FC,_tmp2FC,
_tmp2FC + 55}};struct _tuple23*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple23*
yyzzz;{union Cyc_YYSTYPE _tmp2FD=yy1;struct _tuple23*_tmp2FE;_LL1B8: if((_tmp2FD.YY35).tag
!= 40)goto _LL1BA;_tmp2FE=(struct _tuple23*)(_tmp2FD.YY35).val;_LL1B9: yyzzz=
_tmp2FE;goto _LL1B7;_LL1BA:;_LL1BB:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1B7:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple23*yy1){return({union Cyc_YYSTYPE
_tmp2FF;(_tmp2FF.YY35).val=yy1;(_tmp2FF.YY35).tag=40;_tmp2FF;});}static char
_tmp301[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp301,_tmp301,_tmp301 + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp302=yy1;
struct Cyc_List_List*_tmp303;_LL1BD: if((_tmp302.YY36).tag != 41)goto _LL1BF;_tmp303=(
struct Cyc_List_List*)(_tmp302.YY36).val;_LL1BE: yyzzz=_tmp303;goto _LL1BC;_LL1BF:;
_LL1C0:(int)_throw((void*)& Cyc_yyfail_YY36);_LL1BC:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp304;(_tmp304.YY36).val=
yy1;(_tmp304.YY36).tag=41;_tmp304;});}static char _tmp306[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp306,
_tmp306,_tmp306 + 37}};struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple8*yyzzz;{union Cyc_YYSTYPE _tmp307=yy1;struct _tuple8*_tmp308;_LL1C2: if((
_tmp307.YY37).tag != 42)goto _LL1C4;_tmp308=(struct _tuple8*)(_tmp307.YY37).val;
_LL1C3: yyzzz=_tmp308;goto _LL1C1;_LL1C4:;_LL1C5:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1C1:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union
Cyc_YYSTYPE _tmp309;(_tmp309.YY37).val=yy1;(_tmp309.YY37).tag=42;_tmp309;});}
static char _tmp30B[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp30B,_tmp30B,
_tmp30B + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp30C=yy1;struct Cyc_List_List*_tmp30D;_LL1C7: if((
_tmp30C.YY38).tag != 43)goto _LL1C9;_tmp30D=(struct Cyc_List_List*)(_tmp30C.YY38).val;
_LL1C8: yyzzz=_tmp30D;goto _LL1C6;_LL1C9:;_LL1CA:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1C6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp30E;(_tmp30E.YY38).val=yy1;(_tmp30E.YY38).tag=43;_tmp30E;});}
static char _tmp310[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp310,
_tmp310,_tmp310 + 127}};struct _tuple24*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple24*yyzzz;{union Cyc_YYSTYPE _tmp311=yy1;struct _tuple24*_tmp312;_LL1CC: if((
_tmp311.YY39).tag != 44)goto _LL1CE;_tmp312=(struct _tuple24*)(_tmp311.YY39).val;
_LL1CD: yyzzz=_tmp312;goto _LL1CB;_LL1CE:;_LL1CF:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1CB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple24*yy1){return({union
Cyc_YYSTYPE _tmp313;(_tmp313.YY39).val=yy1;(_tmp313.YY39).tag=44;_tmp313;});}
static char _tmp315[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp315,_tmp315,_tmp315 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp316=yy1;
struct Cyc_List_List*_tmp317;_LL1D1: if((_tmp316.YY40).tag != 45)goto _LL1D3;_tmp317=(
struct Cyc_List_List*)(_tmp316.YY40).val;_LL1D2: yyzzz=_tmp317;goto _LL1D0;_LL1D3:;
_LL1D4:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1D0:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp318;(_tmp318.YY40).val=
yy1;(_tmp318.YY40).tag=45;_tmp318;});}static char _tmp31A[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp31A,
_tmp31A,_tmp31A + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp31B=yy1;struct Cyc_List_List*
_tmp31C;_LL1D6: if((_tmp31B.YY41).tag != 46)goto _LL1D8;_tmp31C=(struct Cyc_List_List*)(
_tmp31B.YY41).val;_LL1D7: yyzzz=_tmp31C;goto _LL1D5;_LL1D8:;_LL1D9:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1D5:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp31D;(_tmp31D.YY41).val=yy1;(_tmp31D.YY41).tag=
46;_tmp31D;});}static char _tmp31F[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp31F,_tmp31F,_tmp31F + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp320=yy1;void*_tmp321;_LL1DB:
if((_tmp320.YY42).tag != 47)goto _LL1DD;_tmp321=(void*)(_tmp320.YY42).val;_LL1DC:
yyzzz=_tmp321;goto _LL1DA;_LL1DD:;_LL1DE:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1DA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE
_tmp322;(_tmp322.YY42).val=yy1;(_tmp322.YY42).tag=47;_tmp322;});}static char
_tmp324[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp324,_tmp324,_tmp324 + 7}};enum Cyc_Absyn_Kind Cyc_yyget_YY43(union Cyc_YYSTYPE
yy1){enum Cyc_Absyn_Kind yyzzz;{union Cyc_YYSTYPE _tmp325=yy1;enum Cyc_Absyn_Kind
_tmp326;_LL1E0: if((_tmp325.YY43).tag != 48)goto _LL1E2;_tmp326=(enum Cyc_Absyn_Kind)(
_tmp325.YY43).val;_LL1E1: yyzzz=_tmp326;goto _LL1DF;_LL1E2:;_LL1E3:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1DF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(enum Cyc_Absyn_Kind
yy1){return({union Cyc_YYSTYPE _tmp327;(_tmp327.YY43).val=yy1;(_tmp327.YY43).tag=
48;_tmp327;});}static char _tmp329[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp329,_tmp329,_tmp329 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp32A=yy1;void*_tmp32B;_LL1E5:
if((_tmp32A.YY44).tag != 49)goto _LL1E7;_tmp32B=(void*)(_tmp32A.YY44).val;_LL1E6:
yyzzz=_tmp32B;goto _LL1E4;_LL1E7:;_LL1E8:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1E4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE
_tmp32C;(_tmp32C.YY44).val=yy1;(_tmp32C.YY44).tag=49;_tmp32C;});}static char
_tmp32E[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp32E,_tmp32E,_tmp32E + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp32F=yy1;
struct Cyc_List_List*_tmp330;_LL1EA: if((_tmp32F.YY45).tag != 50)goto _LL1EC;_tmp330=(
struct Cyc_List_List*)(_tmp32F.YY45).val;_LL1EB: yyzzz=_tmp330;goto _LL1E9;_LL1EC:;
_LL1ED:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1E9:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp331;(_tmp331.YY45).val=
yy1;(_tmp331.YY45).tag=50;_tmp331;});}static char _tmp333[12]="attribute_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp333,_tmp333,
_tmp333 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp334=yy1;void*_tmp335;_LL1EF: if((_tmp334.YY46).tag != 51)goto _LL1F1;_tmp335=(
void*)(_tmp334.YY46).val;_LL1F0: yyzzz=_tmp335;goto _LL1EE;_LL1F1:;_LL1F2:(int)
_throw((void*)& Cyc_yyfail_YY46);_LL1EE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){return({union Cyc_YYSTYPE _tmp336;(_tmp336.YY46).val=yy1;(_tmp336.YY46).tag=
51;_tmp336;});}static char _tmp338[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp338,_tmp338,_tmp338 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp339=yy1;struct Cyc_Absyn_Enumfield*_tmp33A;_LL1F4: if((_tmp339.YY47).tag != 52)
goto _LL1F6;_tmp33A=(struct Cyc_Absyn_Enumfield*)(_tmp339.YY47).val;_LL1F5: yyzzz=
_tmp33A;goto _LL1F3;_LL1F6:;_LL1F7:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1F3:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({
union Cyc_YYSTYPE _tmp33B;(_tmp33B.YY47).val=yy1;(_tmp33B.YY47).tag=52;_tmp33B;});}
static char _tmp33D[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp33D,_tmp33D,_tmp33D + 23}};struct Cyc_List_List*
Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp33E=yy1;struct Cyc_List_List*_tmp33F;_LL1F9: if((_tmp33E.YY48).tag != 53)goto
_LL1FB;_tmp33F=(struct Cyc_List_List*)(_tmp33E.YY48).val;_LL1FA: yyzzz=_tmp33F;
goto _LL1F8;_LL1FB:;_LL1FC:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1F8:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp340;(_tmp340.YY48).val=yy1;(_tmp340.YY48).tag=53;_tmp340;});}static char
_tmp342[17]="opt_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={
Cyc_Core_Failure,{_tmp342,_tmp342,_tmp342 + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp343=yy1;
struct Cyc_Core_Opt*_tmp344;_LL1FE: if((_tmp343.YY49).tag != 54)goto _LL200;_tmp344=(
struct Cyc_Core_Opt*)(_tmp343.YY49).val;_LL1FF: yyzzz=_tmp344;goto _LL1FD;_LL200:;
_LL201:(int)_throw((void*)& Cyc_yyfail_YY49);_LL1FD:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY49(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp345;(_tmp345.YY49).val=
yy1;(_tmp345.YY49).tag=54;_tmp345;});}static char _tmp347[31]="list_t<$(type_t,type_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp347,
_tmp347,_tmp347 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp348=yy1;struct Cyc_List_List*
_tmp349;_LL203: if((_tmp348.YY50).tag != 55)goto _LL205;_tmp349=(struct Cyc_List_List*)(
_tmp348.YY50).val;_LL204: yyzzz=_tmp349;goto _LL202;_LL205:;_LL206:(int)_throw((
void*)& Cyc_yyfail_YY50);_LL202:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp34A;(_tmp34A.YY50).val=yy1;(_tmp34A.YY50).tag=
55;_tmp34A;});}static char _tmp34C[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp34C,_tmp34C,_tmp34C + 15}};union Cyc_Absyn_Constraint*
Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE
_tmp34D=yy1;union Cyc_Absyn_Constraint*_tmp34E;_LL208: if((_tmp34D.YY51).tag != 56)
goto _LL20A;_tmp34E=(union Cyc_Absyn_Constraint*)(_tmp34D.YY51).val;_LL209: yyzzz=
_tmp34E;goto _LL207;_LL20A:;_LL20B:(int)_throw((void*)& Cyc_yyfail_YY51);_LL207:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){return({
union Cyc_YYSTYPE _tmp34F;(_tmp34F.YY51).val=yy1;(_tmp34F.YY51).tag=56;_tmp34F;});}
static char _tmp351[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp351,_tmp351,
_tmp351 + 45}};struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp352=yy1;struct Cyc_List_List*_tmp353;_LL20D: if((
_tmp352.YY52).tag != 57)goto _LL20F;_tmp353=(struct Cyc_List_List*)(_tmp352.YY52).val;
_LL20E: yyzzz=_tmp353;goto _LL20C;_LL20F:;_LL210:(int)_throw((void*)& Cyc_yyfail_YY52);
_LL20C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp354;(_tmp354.YY52).val=yy1;(_tmp354.YY52).tag=57;_tmp354;});}
static char _tmp356[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp356,_tmp356,_tmp356 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp357=yy1;void*_tmp358;_LL212: if((_tmp357.YY53).tag
!= 58)goto _LL214;_tmp358=(void*)(_tmp357.YY53).val;_LL213: yyzzz=_tmp358;goto
_LL211;_LL214:;_LL215:(int)_throw((void*)& Cyc_yyfail_YY53);_LL211:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp359;(_tmp359.YY53).val=
yy1;(_tmp359.YY53).tag=58;_tmp359;});}static char _tmp35B[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp35B,
_tmp35B,_tmp35B + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp35C=yy1;struct Cyc_List_List*
_tmp35D;_LL217: if((_tmp35C.YY54).tag != 59)goto _LL219;_tmp35D=(struct Cyc_List_List*)(
_tmp35C.YY54).val;_LL218: yyzzz=_tmp35D;goto _LL216;_LL219:;_LL21A:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL216:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp35E;(_tmp35E.YY54).val=yy1;(_tmp35E.YY54).tag=
59;_tmp35E;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int
last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp35F;_tmp35F.timestamp=0;_tmp35F.first_line=0;_tmp35F.first_column=0;_tmp35F.last_line=
0;_tmp35F.last_column=0;_tmp35F;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[372]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,143,2,2,127,141,138,2,124,125,132,135,120,139,129,140,2,2,2,2,
2,2,2,2,2,2,128,117,122,121,123,134,133,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,130,2,131,137,126,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,118,136,119,142,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,
99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116};static
char _tmp360[2]="$";static char _tmp361[6]="error";static char _tmp362[12]="$undefined.";
static char _tmp363[5]="AUTO";static char _tmp364[9]="REGISTER";static char _tmp365[7]="STATIC";
static char _tmp366[7]="EXTERN";static char _tmp367[8]="TYPEDEF";static char _tmp368[5]="VOID";
static char _tmp369[5]="CHAR";static char _tmp36A[6]="SHORT";static char _tmp36B[4]="INT";
static char _tmp36C[5]="LONG";static char _tmp36D[6]="FLOAT";static char _tmp36E[7]="DOUBLE";
static char _tmp36F[7]="SIGNED";static char _tmp370[9]="UNSIGNED";static char _tmp371[
6]="CONST";static char _tmp372[9]="VOLATILE";static char _tmp373[9]="RESTRICT";
static char _tmp374[7]="STRUCT";static char _tmp375[6]="UNION";static char _tmp376[5]="CASE";
static char _tmp377[8]="DEFAULT";static char _tmp378[7]="INLINE";static char _tmp379[7]="SIZEOF";
static char _tmp37A[9]="OFFSETOF";static char _tmp37B[3]="IF";static char _tmp37C[5]="ELSE";
static char _tmp37D[7]="SWITCH";static char _tmp37E[6]="WHILE";static char _tmp37F[3]="DO";
static char _tmp380[4]="FOR";static char _tmp381[5]="GOTO";static char _tmp382[9]="CONTINUE";
static char _tmp383[6]="BREAK";static char _tmp384[7]="RETURN";static char _tmp385[5]="ENUM";
static char _tmp386[8]="NULL_kw";static char _tmp387[4]="LET";static char _tmp388[6]="THROW";
static char _tmp389[4]="TRY";static char _tmp38A[6]="CATCH";static char _tmp38B[7]="EXPORT";
static char _tmp38C[4]="NEW";static char _tmp38D[9]="ABSTRACT";static char _tmp38E[9]="FALLTHRU";
static char _tmp38F[6]="USING";static char _tmp390[10]="NAMESPACE";static char _tmp391[
9]="DATATYPE";static char _tmp392[8]="XTUNION";static char _tmp393[7]="TUNION";
static char _tmp394[7]="MALLOC";static char _tmp395[8]="RMALLOC";static char _tmp396[7]="CALLOC";
static char _tmp397[8]="RCALLOC";static char _tmp398[5]="SWAP";static char _tmp399[9]="REGION_T";
static char _tmp39A[6]="TAG_T";static char _tmp39B[7]="REGION";static char _tmp39C[5]="RNEW";
static char _tmp39D[8]="REGIONS";static char _tmp39E[13]="RESET_REGION";static char
_tmp39F[4]="GEN";static char _tmp3A0[16]="NOZEROTERM_QUAL";static char _tmp3A1[14]="ZEROTERM_QUAL";
static char _tmp3A2[12]="REGION_QUAL";static char _tmp3A3[7]="PORTON";static char
_tmp3A4[8]="PORTOFF";static char _tmp3A5[12]="DYNREGION_T";static char _tmp3A6[6]="ALIAS";
static char _tmp3A7[8]="NUMELTS";static char _tmp3A8[8]="VALUEOF";static char _tmp3A9[
10]="VALUEOF_T";static char _tmp3AA[9]="TAGCHECK";static char _tmp3AB[13]="NUMELTS_QUAL";
static char _tmp3AC[10]="THIN_QUAL";static char _tmp3AD[9]="FAT_QUAL";static char
_tmp3AE[13]="NOTNULL_QUAL";static char _tmp3AF[14]="NULLABLE_QUAL";static char
_tmp3B0[12]="TAGGED_QUAL";static char _tmp3B1[16]="EXTENSIBLE_QUAL";static char
_tmp3B2[15]="RESETABLE_QUAL";static char _tmp3B3[7]="PTR_OP";static char _tmp3B4[7]="INC_OP";
static char _tmp3B5[7]="DEC_OP";static char _tmp3B6[8]="LEFT_OP";static char _tmp3B7[9]="RIGHT_OP";
static char _tmp3B8[6]="LE_OP";static char _tmp3B9[6]="GE_OP";static char _tmp3BA[6]="EQ_OP";
static char _tmp3BB[6]="NE_OP";static char _tmp3BC[7]="AND_OP";static char _tmp3BD[6]="OR_OP";
static char _tmp3BE[11]="MUL_ASSIGN";static char _tmp3BF[11]="DIV_ASSIGN";static char
_tmp3C0[11]="MOD_ASSIGN";static char _tmp3C1[11]="ADD_ASSIGN";static char _tmp3C2[11]="SUB_ASSIGN";
static char _tmp3C3[12]="LEFT_ASSIGN";static char _tmp3C4[13]="RIGHT_ASSIGN";static
char _tmp3C5[11]="AND_ASSIGN";static char _tmp3C6[11]="XOR_ASSIGN";static char
_tmp3C7[10]="OR_ASSIGN";static char _tmp3C8[9]="ELLIPSIS";static char _tmp3C9[11]="LEFT_RIGHT";
static char _tmp3CA[12]="COLON_COLON";static char _tmp3CB[11]="IDENTIFIER";static
char _tmp3CC[17]="INTEGER_CONSTANT";static char _tmp3CD[7]="STRING";static char
_tmp3CE[19]="CHARACTER_CONSTANT";static char _tmp3CF[18]="FLOATING_CONSTANT";
static char _tmp3D0[9]="TYPE_VAR";static char _tmp3D1[13]="TYPEDEF_NAME";static char
_tmp3D2[16]="QUAL_IDENTIFIER";static char _tmp3D3[18]="QUAL_TYPEDEF_NAME";static
char _tmp3D4[10]="ATTRIBUTE";static char _tmp3D5[4]="';'";static char _tmp3D6[4]="'{'";
static char _tmp3D7[4]="'}'";static char _tmp3D8[4]="','";static char _tmp3D9[4]="'='";
static char _tmp3DA[4]="'<'";static char _tmp3DB[4]="'>'";static char _tmp3DC[4]="'('";
static char _tmp3DD[4]="')'";static char _tmp3DE[4]="'_'";static char _tmp3DF[4]="'$'";
static char _tmp3E0[4]="':'";static char _tmp3E1[4]="'.'";static char _tmp3E2[4]="'['";
static char _tmp3E3[4]="']'";static char _tmp3E4[4]="'*'";static char _tmp3E5[4]="'@'";
static char _tmp3E6[4]="'?'";static char _tmp3E7[4]="'+'";static char _tmp3E8[4]="'|'";
static char _tmp3E9[4]="'^'";static char _tmp3EA[4]="'&'";static char _tmp3EB[4]="'-'";
static char _tmp3EC[4]="'/'";static char _tmp3ED[4]="'%'";static char _tmp3EE[4]="'~'";
static char _tmp3EF[4]="'!'";static char _tmp3F0[5]="prog";static char _tmp3F1[17]="translation_unit";
static char _tmp3F2[12]="export_list";static char _tmp3F3[19]="export_list_values";
static char _tmp3F4[21]="external_declaration";static char _tmp3F5[20]="function_definition";
static char _tmp3F6[21]="function_definition2";static char _tmp3F7[13]="using_action";
static char _tmp3F8[15]="unusing_action";static char _tmp3F9[17]="namespace_action";
static char _tmp3FA[19]="unnamespace_action";static char _tmp3FB[12]="declaration";
static char _tmp3FC[19]="resetable_qual_opt";static char _tmp3FD[17]="declaration_list";
static char _tmp3FE[23]="declaration_specifiers";static char _tmp3FF[24]="storage_class_specifier";
static char _tmp400[15]="attributes_opt";static char _tmp401[11]="attributes";static
char _tmp402[15]="attribute_list";static char _tmp403[10]="attribute";static char
_tmp404[15]="type_specifier";static char _tmp405[25]="type_specifier_notypedef";
static char _tmp406[5]="kind";static char _tmp407[15]="type_qualifier";static char
_tmp408[15]="enum_specifier";static char _tmp409[11]="enum_field";static char
_tmp40A[22]="enum_declaration_list";static char _tmp40B[26]="struct_or_union_specifier";
static char _tmp40C[16]="type_params_opt";static char _tmp40D[16]="struct_or_union";
static char _tmp40E[24]="struct_declaration_list";static char _tmp40F[25]="struct_declaration_list0";
static char _tmp410[21]="init_declarator_list";static char _tmp411[22]="init_declarator_list0";
static char _tmp412[16]="init_declarator";static char _tmp413[19]="struct_declaration";
static char _tmp414[25]="specifier_qualifier_list";static char _tmp415[35]="notypedef_specifier_qualifier_list";
static char _tmp416[23]="struct_declarator_list";static char _tmp417[24]="struct_declarator_list0";
static char _tmp418[18]="struct_declarator";static char _tmp419[19]="datatype_specifier";
static char _tmp41A[14]="qual_datatype";static char _tmp41B[19]="datatypefield_list";
static char _tmp41C[20]="datatypefield_scope";static char _tmp41D[14]="datatypefield";
static char _tmp41E[11]="declarator";static char _tmp41F[23]="declarator_withtypedef";
static char _tmp420[18]="direct_declarator";static char _tmp421[30]="direct_declarator_withtypedef";
static char _tmp422[8]="pointer";static char _tmp423[12]="one_pointer";static char
_tmp424[14]="pointer_quals";static char _tmp425[13]="pointer_qual";static char
_tmp426[23]="pointer_null_and_bound";static char _tmp427[14]="pointer_bound";
static char _tmp428[18]="zeroterm_qual_opt";static char _tmp429[8]="rgn_opt";static
char _tmp42A[11]="tqual_list";static char _tmp42B[20]="parameter_type_list";static
char _tmp42C[9]="type_var";static char _tmp42D[16]="optional_effect";static char
_tmp42E[19]="optional_rgn_order";static char _tmp42F[10]="rgn_order";static char
_tmp430[16]="optional_inject";static char _tmp431[11]="effect_set";static char
_tmp432[14]="atomic_effect";static char _tmp433[11]="region_set";static char _tmp434[
15]="parameter_list";static char _tmp435[22]="parameter_declaration";static char
_tmp436[16]="identifier_list";static char _tmp437[17]="identifier_list0";static
char _tmp438[12]="initializer";static char _tmp439[18]="array_initializer";static
char _tmp43A[17]="initializer_list";static char _tmp43B[12]="designation";static
char _tmp43C[16]="designator_list";static char _tmp43D[11]="designator";static char
_tmp43E[10]="type_name";static char _tmp43F[14]="any_type_name";static char _tmp440[
15]="type_name_list";static char _tmp441[20]="abstract_declarator";static char
_tmp442[27]="direct_abstract_declarator";static char _tmp443[10]="statement";
static char _tmp444[13]="open_exp_opt";static char _tmp445[18]="labeled_statement";
static char _tmp446[21]="expression_statement";static char _tmp447[19]="compound_statement";
static char _tmp448[16]="block_item_list";static char _tmp449[20]="selection_statement";
static char _tmp44A[15]="switch_clauses";static char _tmp44B[20]="iteration_statement";
static char _tmp44C[15]="jump_statement";static char _tmp44D[12]="exp_pattern";
static char _tmp44E[20]="conditional_pattern";static char _tmp44F[19]="logical_or_pattern";
static char _tmp450[20]="logical_and_pattern";static char _tmp451[21]="inclusive_or_pattern";
static char _tmp452[21]="exclusive_or_pattern";static char _tmp453[12]="and_pattern";
static char _tmp454[17]="equality_pattern";static char _tmp455[19]="relational_pattern";
static char _tmp456[14]="shift_pattern";static char _tmp457[17]="additive_pattern";
static char _tmp458[23]="multiplicative_pattern";static char _tmp459[13]="cast_pattern";
static char _tmp45A[14]="unary_pattern";static char _tmp45B[16]="postfix_pattern";
static char _tmp45C[16]="primary_pattern";static char _tmp45D[8]="pattern";static
char _tmp45E[19]="tuple_pattern_list";static char _tmp45F[20]="tuple_pattern_list0";
static char _tmp460[14]="field_pattern";static char _tmp461[19]="field_pattern_list";
static char _tmp462[20]="field_pattern_list0";static char _tmp463[11]="expression";
static char _tmp464[22]="assignment_expression";static char _tmp465[20]="assignment_operator";
static char _tmp466[23]="conditional_expression";static char _tmp467[20]="constant_expression";
static char _tmp468[22]="logical_or_expression";static char _tmp469[23]="logical_and_expression";
static char _tmp46A[24]="inclusive_or_expression";static char _tmp46B[24]="exclusive_or_expression";
static char _tmp46C[15]="and_expression";static char _tmp46D[20]="equality_expression";
static char _tmp46E[22]="relational_expression";static char _tmp46F[17]="shift_expression";
static char _tmp470[20]="additive_expression";static char _tmp471[26]="multiplicative_expression";
static char _tmp472[16]="cast_expression";static char _tmp473[17]="unary_expression";
static char _tmp474[15]="unary_operator";static char _tmp475[19]="postfix_expression";
static char _tmp476[19]="primary_expression";static char _tmp477[25]="argument_expression_list";
static char _tmp478[26]="argument_expression_list0";static char _tmp479[9]="constant";
static char _tmp47A[20]="qual_opt_identifier";static char _tmp47B[17]="qual_opt_typedef";
static char _tmp47C[18]="struct_union_name";static char _tmp47D[11]="field_name";
static char _tmp47E[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[287]={{
_tmp360,_tmp360,_tmp360 + 2},{_tmp361,_tmp361,_tmp361 + 6},{_tmp362,_tmp362,
_tmp362 + 12},{_tmp363,_tmp363,_tmp363 + 5},{_tmp364,_tmp364,_tmp364 + 9},{_tmp365,
_tmp365,_tmp365 + 7},{_tmp366,_tmp366,_tmp366 + 7},{_tmp367,_tmp367,_tmp367 + 8},{
_tmp368,_tmp368,_tmp368 + 5},{_tmp369,_tmp369,_tmp369 + 5},{_tmp36A,_tmp36A,
_tmp36A + 6},{_tmp36B,_tmp36B,_tmp36B + 4},{_tmp36C,_tmp36C,_tmp36C + 5},{_tmp36D,
_tmp36D,_tmp36D + 6},{_tmp36E,_tmp36E,_tmp36E + 7},{_tmp36F,_tmp36F,_tmp36F + 7},{
_tmp370,_tmp370,_tmp370 + 9},{_tmp371,_tmp371,_tmp371 + 6},{_tmp372,_tmp372,
_tmp372 + 9},{_tmp373,_tmp373,_tmp373 + 9},{_tmp374,_tmp374,_tmp374 + 7},{_tmp375,
_tmp375,_tmp375 + 6},{_tmp376,_tmp376,_tmp376 + 5},{_tmp377,_tmp377,_tmp377 + 8},{
_tmp378,_tmp378,_tmp378 + 7},{_tmp379,_tmp379,_tmp379 + 7},{_tmp37A,_tmp37A,
_tmp37A + 9},{_tmp37B,_tmp37B,_tmp37B + 3},{_tmp37C,_tmp37C,_tmp37C + 5},{_tmp37D,
_tmp37D,_tmp37D + 7},{_tmp37E,_tmp37E,_tmp37E + 6},{_tmp37F,_tmp37F,_tmp37F + 3},{
_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 5},{_tmp382,_tmp382,
_tmp382 + 9},{_tmp383,_tmp383,_tmp383 + 6},{_tmp384,_tmp384,_tmp384 + 7},{_tmp385,
_tmp385,_tmp385 + 5},{_tmp386,_tmp386,_tmp386 + 8},{_tmp387,_tmp387,_tmp387 + 4},{
_tmp388,_tmp388,_tmp388 + 6},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,
_tmp38A + 6},{_tmp38B,_tmp38B,_tmp38B + 7},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,
_tmp38D,_tmp38D + 9},{_tmp38E,_tmp38E,_tmp38E + 9},{_tmp38F,_tmp38F,_tmp38F + 6},{
_tmp390,_tmp390,_tmp390 + 10},{_tmp391,_tmp391,_tmp391 + 9},{_tmp392,_tmp392,
_tmp392 + 8},{_tmp393,_tmp393,_tmp393 + 7},{_tmp394,_tmp394,_tmp394 + 7},{_tmp395,
_tmp395,_tmp395 + 8},{_tmp396,_tmp396,_tmp396 + 7},{_tmp397,_tmp397,_tmp397 + 8},{
_tmp398,_tmp398,_tmp398 + 5},{_tmp399,_tmp399,_tmp399 + 9},{_tmp39A,_tmp39A,
_tmp39A + 6},{_tmp39B,_tmp39B,_tmp39B + 7},{_tmp39C,_tmp39C,_tmp39C + 5},{_tmp39D,
_tmp39D,_tmp39D + 8},{_tmp39E,_tmp39E,_tmp39E + 13},{_tmp39F,_tmp39F,_tmp39F + 4},{
_tmp3A0,_tmp3A0,_tmp3A0 + 16},{_tmp3A1,_tmp3A1,_tmp3A1 + 14},{_tmp3A2,_tmp3A2,
_tmp3A2 + 12},{_tmp3A3,_tmp3A3,_tmp3A3 + 7},{_tmp3A4,_tmp3A4,_tmp3A4 + 8},{_tmp3A5,
_tmp3A5,_tmp3A5 + 12},{_tmp3A6,_tmp3A6,_tmp3A6 + 6},{_tmp3A7,_tmp3A7,_tmp3A7 + 8},{
_tmp3A8,_tmp3A8,_tmp3A8 + 8},{_tmp3A9,_tmp3A9,_tmp3A9 + 10},{_tmp3AA,_tmp3AA,
_tmp3AA + 9},{_tmp3AB,_tmp3AB,_tmp3AB + 13},{_tmp3AC,_tmp3AC,_tmp3AC + 10},{_tmp3AD,
_tmp3AD,_tmp3AD + 9},{_tmp3AE,_tmp3AE,_tmp3AE + 13},{_tmp3AF,_tmp3AF,_tmp3AF + 14},{
_tmp3B0,_tmp3B0,_tmp3B0 + 12},{_tmp3B1,_tmp3B1,_tmp3B1 + 16},{_tmp3B2,_tmp3B2,
_tmp3B2 + 15},{_tmp3B3,_tmp3B3,_tmp3B3 + 7},{_tmp3B4,_tmp3B4,_tmp3B4 + 7},{_tmp3B5,
_tmp3B5,_tmp3B5 + 7},{_tmp3B6,_tmp3B6,_tmp3B6 + 8},{_tmp3B7,_tmp3B7,_tmp3B7 + 9},{
_tmp3B8,_tmp3B8,_tmp3B8 + 6},{_tmp3B9,_tmp3B9,_tmp3B9 + 6},{_tmp3BA,_tmp3BA,
_tmp3BA + 6},{_tmp3BB,_tmp3BB,_tmp3BB + 6},{_tmp3BC,_tmp3BC,_tmp3BC + 7},{_tmp3BD,
_tmp3BD,_tmp3BD + 6},{_tmp3BE,_tmp3BE,_tmp3BE + 11},{_tmp3BF,_tmp3BF,_tmp3BF + 11},{
_tmp3C0,_tmp3C0,_tmp3C0 + 11},{_tmp3C1,_tmp3C1,_tmp3C1 + 11},{_tmp3C2,_tmp3C2,
_tmp3C2 + 11},{_tmp3C3,_tmp3C3,_tmp3C3 + 12},{_tmp3C4,_tmp3C4,_tmp3C4 + 13},{
_tmp3C5,_tmp3C5,_tmp3C5 + 11},{_tmp3C6,_tmp3C6,_tmp3C6 + 11},{_tmp3C7,_tmp3C7,
_tmp3C7 + 10},{_tmp3C8,_tmp3C8,_tmp3C8 + 9},{_tmp3C9,_tmp3C9,_tmp3C9 + 11},{_tmp3CA,
_tmp3CA,_tmp3CA + 12},{_tmp3CB,_tmp3CB,_tmp3CB + 11},{_tmp3CC,_tmp3CC,_tmp3CC + 17},{
_tmp3CD,_tmp3CD,_tmp3CD + 7},{_tmp3CE,_tmp3CE,_tmp3CE + 19},{_tmp3CF,_tmp3CF,
_tmp3CF + 18},{_tmp3D0,_tmp3D0,_tmp3D0 + 9},{_tmp3D1,_tmp3D1,_tmp3D1 + 13},{_tmp3D2,
_tmp3D2,_tmp3D2 + 16},{_tmp3D3,_tmp3D3,_tmp3D3 + 18},{_tmp3D4,_tmp3D4,_tmp3D4 + 10},{
_tmp3D5,_tmp3D5,_tmp3D5 + 4},{_tmp3D6,_tmp3D6,_tmp3D6 + 4},{_tmp3D7,_tmp3D7,
_tmp3D7 + 4},{_tmp3D8,_tmp3D8,_tmp3D8 + 4},{_tmp3D9,_tmp3D9,_tmp3D9 + 4},{_tmp3DA,
_tmp3DA,_tmp3DA + 4},{_tmp3DB,_tmp3DB,_tmp3DB + 4},{_tmp3DC,_tmp3DC,_tmp3DC + 4},{
_tmp3DD,_tmp3DD,_tmp3DD + 4},{_tmp3DE,_tmp3DE,_tmp3DE + 4},{_tmp3DF,_tmp3DF,
_tmp3DF + 4},{_tmp3E0,_tmp3E0,_tmp3E0 + 4},{_tmp3E1,_tmp3E1,_tmp3E1 + 4},{_tmp3E2,
_tmp3E2,_tmp3E2 + 4},{_tmp3E3,_tmp3E3,_tmp3E3 + 4},{_tmp3E4,_tmp3E4,_tmp3E4 + 4},{
_tmp3E5,_tmp3E5,_tmp3E5 + 4},{_tmp3E6,_tmp3E6,_tmp3E6 + 4},{_tmp3E7,_tmp3E7,
_tmp3E7 + 4},{_tmp3E8,_tmp3E8,_tmp3E8 + 4},{_tmp3E9,_tmp3E9,_tmp3E9 + 4},{_tmp3EA,
_tmp3EA,_tmp3EA + 4},{_tmp3EB,_tmp3EB,_tmp3EB + 4},{_tmp3EC,_tmp3EC,_tmp3EC + 4},{
_tmp3ED,_tmp3ED,_tmp3ED + 4},{_tmp3EE,_tmp3EE,_tmp3EE + 4},{_tmp3EF,_tmp3EF,
_tmp3EF + 4},{_tmp3F0,_tmp3F0,_tmp3F0 + 5},{_tmp3F1,_tmp3F1,_tmp3F1 + 17},{_tmp3F2,
_tmp3F2,_tmp3F2 + 12},{_tmp3F3,_tmp3F3,_tmp3F3 + 19},{_tmp3F4,_tmp3F4,_tmp3F4 + 21},{
_tmp3F5,_tmp3F5,_tmp3F5 + 20},{_tmp3F6,_tmp3F6,_tmp3F6 + 21},{_tmp3F7,_tmp3F7,
_tmp3F7 + 13},{_tmp3F8,_tmp3F8,_tmp3F8 + 15},{_tmp3F9,_tmp3F9,_tmp3F9 + 17},{
_tmp3FA,_tmp3FA,_tmp3FA + 19},{_tmp3FB,_tmp3FB,_tmp3FB + 12},{_tmp3FC,_tmp3FC,
_tmp3FC + 19},{_tmp3FD,_tmp3FD,_tmp3FD + 17},{_tmp3FE,_tmp3FE,_tmp3FE + 23},{
_tmp3FF,_tmp3FF,_tmp3FF + 24},{_tmp400,_tmp400,_tmp400 + 15},{_tmp401,_tmp401,
_tmp401 + 11},{_tmp402,_tmp402,_tmp402 + 15},{_tmp403,_tmp403,_tmp403 + 10},{
_tmp404,_tmp404,_tmp404 + 15},{_tmp405,_tmp405,_tmp405 + 25},{_tmp406,_tmp406,
_tmp406 + 5},{_tmp407,_tmp407,_tmp407 + 15},{_tmp408,_tmp408,_tmp408 + 15},{_tmp409,
_tmp409,_tmp409 + 11},{_tmp40A,_tmp40A,_tmp40A + 22},{_tmp40B,_tmp40B,_tmp40B + 26},{
_tmp40C,_tmp40C,_tmp40C + 16},{_tmp40D,_tmp40D,_tmp40D + 16},{_tmp40E,_tmp40E,
_tmp40E + 24},{_tmp40F,_tmp40F,_tmp40F + 25},{_tmp410,_tmp410,_tmp410 + 21},{
_tmp411,_tmp411,_tmp411 + 22},{_tmp412,_tmp412,_tmp412 + 16},{_tmp413,_tmp413,
_tmp413 + 19},{_tmp414,_tmp414,_tmp414 + 25},{_tmp415,_tmp415,_tmp415 + 35},{
_tmp416,_tmp416,_tmp416 + 23},{_tmp417,_tmp417,_tmp417 + 24},{_tmp418,_tmp418,
_tmp418 + 18},{_tmp419,_tmp419,_tmp419 + 19},{_tmp41A,_tmp41A,_tmp41A + 14},{
_tmp41B,_tmp41B,_tmp41B + 19},{_tmp41C,_tmp41C,_tmp41C + 20},{_tmp41D,_tmp41D,
_tmp41D + 14},{_tmp41E,_tmp41E,_tmp41E + 11},{_tmp41F,_tmp41F,_tmp41F + 23},{
_tmp420,_tmp420,_tmp420 + 18},{_tmp421,_tmp421,_tmp421 + 30},{_tmp422,_tmp422,
_tmp422 + 8},{_tmp423,_tmp423,_tmp423 + 12},{_tmp424,_tmp424,_tmp424 + 14},{_tmp425,
_tmp425,_tmp425 + 13},{_tmp426,_tmp426,_tmp426 + 23},{_tmp427,_tmp427,_tmp427 + 14},{
_tmp428,_tmp428,_tmp428 + 18},{_tmp429,_tmp429,_tmp429 + 8},{_tmp42A,_tmp42A,
_tmp42A + 11},{_tmp42B,_tmp42B,_tmp42B + 20},{_tmp42C,_tmp42C,_tmp42C + 9},{_tmp42D,
_tmp42D,_tmp42D + 16},{_tmp42E,_tmp42E,_tmp42E + 19},{_tmp42F,_tmp42F,_tmp42F + 10},{
_tmp430,_tmp430,_tmp430 + 16},{_tmp431,_tmp431,_tmp431 + 11},{_tmp432,_tmp432,
_tmp432 + 14},{_tmp433,_tmp433,_tmp433 + 11},{_tmp434,_tmp434,_tmp434 + 15},{
_tmp435,_tmp435,_tmp435 + 22},{_tmp436,_tmp436,_tmp436 + 16},{_tmp437,_tmp437,
_tmp437 + 17},{_tmp438,_tmp438,_tmp438 + 12},{_tmp439,_tmp439,_tmp439 + 18},{
_tmp43A,_tmp43A,_tmp43A + 17},{_tmp43B,_tmp43B,_tmp43B + 12},{_tmp43C,_tmp43C,
_tmp43C + 16},{_tmp43D,_tmp43D,_tmp43D + 11},{_tmp43E,_tmp43E,_tmp43E + 10},{
_tmp43F,_tmp43F,_tmp43F + 14},{_tmp440,_tmp440,_tmp440 + 15},{_tmp441,_tmp441,
_tmp441 + 20},{_tmp442,_tmp442,_tmp442 + 27},{_tmp443,_tmp443,_tmp443 + 10},{
_tmp444,_tmp444,_tmp444 + 13},{_tmp445,_tmp445,_tmp445 + 18},{_tmp446,_tmp446,
_tmp446 + 21},{_tmp447,_tmp447,_tmp447 + 19},{_tmp448,_tmp448,_tmp448 + 16},{
_tmp449,_tmp449,_tmp449 + 20},{_tmp44A,_tmp44A,_tmp44A + 15},{_tmp44B,_tmp44B,
_tmp44B + 20},{_tmp44C,_tmp44C,_tmp44C + 15},{_tmp44D,_tmp44D,_tmp44D + 12},{
_tmp44E,_tmp44E,_tmp44E + 20},{_tmp44F,_tmp44F,_tmp44F + 19},{_tmp450,_tmp450,
_tmp450 + 20},{_tmp451,_tmp451,_tmp451 + 21},{_tmp452,_tmp452,_tmp452 + 21},{
_tmp453,_tmp453,_tmp453 + 12},{_tmp454,_tmp454,_tmp454 + 17},{_tmp455,_tmp455,
_tmp455 + 19},{_tmp456,_tmp456,_tmp456 + 14},{_tmp457,_tmp457,_tmp457 + 17},{
_tmp458,_tmp458,_tmp458 + 23},{_tmp459,_tmp459,_tmp459 + 13},{_tmp45A,_tmp45A,
_tmp45A + 14},{_tmp45B,_tmp45B,_tmp45B + 16},{_tmp45C,_tmp45C,_tmp45C + 16},{
_tmp45D,_tmp45D,_tmp45D + 8},{_tmp45E,_tmp45E,_tmp45E + 19},{_tmp45F,_tmp45F,
_tmp45F + 20},{_tmp460,_tmp460,_tmp460 + 14},{_tmp461,_tmp461,_tmp461 + 19},{
_tmp462,_tmp462,_tmp462 + 20},{_tmp463,_tmp463,_tmp463 + 11},{_tmp464,_tmp464,
_tmp464 + 22},{_tmp465,_tmp465,_tmp465 + 20},{_tmp466,_tmp466,_tmp466 + 23},{
_tmp467,_tmp467,_tmp467 + 20},{_tmp468,_tmp468,_tmp468 + 22},{_tmp469,_tmp469,
_tmp469 + 23},{_tmp46A,_tmp46A,_tmp46A + 24},{_tmp46B,_tmp46B,_tmp46B + 24},{
_tmp46C,_tmp46C,_tmp46C + 15},{_tmp46D,_tmp46D,_tmp46D + 20},{_tmp46E,_tmp46E,
_tmp46E + 22},{_tmp46F,_tmp46F,_tmp46F + 17},{_tmp470,_tmp470,_tmp470 + 20},{
_tmp471,_tmp471,_tmp471 + 26},{_tmp472,_tmp472,_tmp472 + 16},{_tmp473,_tmp473,
_tmp473 + 17},{_tmp474,_tmp474,_tmp474 + 15},{_tmp475,_tmp475,_tmp475 + 19},{
_tmp476,_tmp476,_tmp476 + 19},{_tmp477,_tmp477,_tmp477 + 25},{_tmp478,_tmp478,
_tmp478 + 26},{_tmp479,_tmp479,_tmp479 + 9},{_tmp47A,_tmp47A,_tmp47A + 20},{_tmp47B,
_tmp47B,_tmp47B + 17},{_tmp47C,_tmp47C,_tmp47C + 18},{_tmp47D,_tmp47D,_tmp47D + 11},{
_tmp47E,_tmp47E,_tmp47E + 12}};static short Cyc_yyr1[511]={0,144,145,145,145,145,
145,145,145,145,145,145,146,146,147,147,148,148,148,149,149,149,149,150,150,151,
152,153,154,155,155,155,155,155,155,155,156,156,157,157,158,158,158,158,158,158,
158,158,158,158,159,159,159,159,159,159,159,160,160,161,162,162,163,163,163,163,
163,163,164,164,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
165,165,165,165,165,165,165,166,167,167,167,168,168,168,169,169,170,170,170,171,
171,171,171,171,172,172,173,173,174,174,175,175,176,177,177,178,178,179,180,180,
180,180,180,180,181,181,181,181,181,181,182,183,183,184,184,184,185,185,185,186,
186,187,187,187,187,188,188,188,189,189,190,190,191,191,192,192,192,192,192,192,
192,192,192,192,193,193,193,193,193,193,193,193,193,193,193,194,194,195,196,196,
197,197,197,197,197,197,197,197,198,198,198,199,199,200,200,200,201,201,201,202,
202,203,203,203,203,204,204,205,205,206,206,207,207,208,208,209,209,210,210,210,
210,211,211,212,212,213,213,213,214,215,215,216,216,217,217,217,217,218,218,218,
218,219,220,220,221,221,222,222,223,223,223,223,223,224,224,225,225,225,226,226,
226,226,226,226,226,226,226,226,226,227,227,227,227,227,227,227,228,228,229,230,
230,231,231,232,232,232,232,232,232,233,233,233,233,233,233,234,234,234,234,234,
234,235,235,235,235,235,235,235,235,235,235,235,235,235,235,236,236,236,236,236,
236,236,236,237,238,238,239,239,240,240,241,241,242,242,243,243,244,244,244,245,
245,245,245,245,246,246,246,247,247,247,248,248,248,248,249,249,250,250,250,250,
250,250,250,251,252,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,
254,254,254,255,255,256,256,257,257,257,258,258,259,259,260,260,260,261,261,261,
261,261,261,261,261,261,261,261,262,262,262,262,262,262,262,263,264,264,265,265,
266,266,267,267,268,268,269,269,269,270,270,270,270,270,271,271,271,272,272,272,
273,273,273,273,274,274,275,275,275,275,275,275,275,275,275,275,275,275,275,275,
275,275,275,275,275,275,276,276,276,277,277,277,277,277,277,277,277,277,277,278,
278,278,278,278,278,278,278,278,279,280,280,281,281,281,281,282,282,283,283,284,
284,285,285,286,286};static short Cyc_yyr2[511]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,
1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,
0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,
5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,
1,3,1,2,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,
5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,
3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,
4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,
7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,
3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,
4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,
2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,
2,2,4,2,6,6,5,4,6,9,11,4,6,6,4,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,
1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[1032]={0,18,50,51,52,53,55,
70,73,74,75,76,77,78,79,80,94,95,96,112,113,46,0,0,56,0,0,145,87,91,0,0,0,0,0,0,
0,37,501,211,503,502,504,0,0,71,0,197,197,196,1,0,16,0,0,17,0,0,40,48,42,68,44,
81,82,0,83,0,36,156,0,181,184,84,160,110,54,53,47,0,98,500,0,501,497,498,499,110,
0,371,0,0,0,0,234,0,373,374,25,27,0,0,0,0,0,0,0,0,146,0,0,0,0,0,0,0,194,195,2,0,
0,0,0,0,29,0,118,119,121,41,49,43,45,114,505,506,110,110,36,38,36,0,19,0,213,0,
169,157,182,191,190,0,0,188,189,192,193,202,184,0,69,0,54,102,0,100,0,501,381,0,
0,0,0,0,0,0,0,0,0,0,0,110,0,0,0,0,0,487,0,0,0,0,0,474,472,473,0,398,400,414,422,
424,426,428,430,432,435,440,443,446,450,0,452,475,486,485,0,382,380,32,0,0,110,0,
0,0,128,124,126,252,254,0,0,9,10,0,0,0,110,507,508,212,93,0,0,161,72,232,0,229,0,
3,0,5,0,282,0,30,0,0,36,20,0,115,116,0,109,0,143,0,0,0,0,0,0,0,0,0,0,0,0,501,285,
287,0,293,289,0,291,275,276,277,0,278,279,280,0,39,21,121,259,0,219,235,0,0,215,
213,0,199,0,0,0,204,57,203,185,0,0,103,99,0,0,375,0,0,395,0,0,0,0,248,391,396,0,
393,0,460,0,416,450,0,417,418,0,0,0,0,0,0,0,0,0,0,453,454,36,0,0,0,456,457,455,0,
372,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,404,405,406,407,408,409,410,411,412,
413,403,0,458,0,481,482,0,0,0,489,0,110,388,389,0,386,0,236,0,0,0,0,255,227,0,
129,134,130,132,125,127,213,0,261,253,262,510,509,0,86,90,0,88,0,92,108,63,62,0,
60,162,213,231,158,261,233,170,171,0,85,198,26,0,28,0,0,0,0,120,122,238,237,22,
105,117,0,0,0,136,137,139,0,110,110,151,0,0,0,0,0,0,36,0,322,323,324,0,0,326,0,0,
0,0,294,290,121,292,288,286,0,168,220,0,0,0,226,214,221,165,0,0,0,215,167,201,
200,163,199,0,0,205,58,111,0,104,421,101,97,384,385,251,0,392,247,249,379,0,0,0,
0,239,243,0,0,0,0,0,0,0,0,0,0,0,0,0,0,488,495,0,494,399,423,0,425,427,429,431,
433,434,438,439,436,437,441,442,444,445,447,448,449,402,401,480,477,0,479,0,0,0,
376,0,383,31,0,377,0,0,256,135,131,133,0,215,0,199,0,263,0,213,0,274,258,0,0,110,
0,0,0,128,0,110,0,213,0,180,159,230,4,6,0,34,0,0,140,123,0,0,215,144,153,152,0,0,
147,0,0,0,301,0,0,0,0,0,321,325,0,0,0,0,284,36,23,260,213,0,223,0,0,216,0,166,
219,207,164,187,186,205,183,0,7,0,250,394,397,459,0,0,240,0,244,464,0,0,0,0,0,
468,471,0,0,0,0,0,451,491,0,0,478,476,0,0,387,390,378,257,228,270,0,264,265,199,
0,0,215,199,0,89,0,215,0,0,0,59,61,0,172,0,0,215,0,199,0,0,0,138,141,114,142,154,
151,151,0,0,0,0,0,0,0,0,0,0,0,0,0,301,327,0,0,24,215,0,224,222,0,215,0,206,0,8,0,
0,0,241,245,0,0,0,0,419,420,463,480,479,493,0,496,415,490,492,0,269,267,273,272,
0,266,199,0,114,0,66,64,65,173,179,176,0,178,174,199,0,33,0,0,150,149,295,301,0,
0,0,0,0,0,329,330,332,334,336,338,340,342,345,350,353,356,360,362,369,370,0,36,
298,307,0,0,0,0,0,0,0,0,0,0,328,281,209,225,217,208,213,13,0,0,462,461,0,246,465,
0,0,470,469,483,0,271,268,35,0,0,177,175,283,107,0,0,0,301,0,366,0,0,363,36,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,364,302,0,309,0,0,0,317,0,0,0,0,0,0,300,0,
215,12,14,0,0,0,0,484,106,0,155,296,297,0,0,0,0,301,303,333,0,335,337,339,341,
343,344,348,349,346,347,351,352,354,355,357,358,359,0,308,310,311,0,319,318,0,
313,0,0,0,218,210,15,0,0,0,0,299,365,0,361,304,0,36,312,320,314,315,0,242,466,0,
67,0,0,331,301,305,316,0,368,367,306,467,0,0,0};static short Cyc_yydefgoto[143]={
1029,50,685,877,51,52,285,53,457,54,459,55,56,140,57,58,525,226,443,444,227,61,
241,228,63,163,164,64,160,65,262,263,125,126,127,264,229,424,472,473,474,66,67,
646,647,648,68,475,69,448,70,71,157,158,72,116,521,314,682,606,73,607,515,673,
507,511,512,419,307,249,93,94,546,465,547,329,330,331,230,300,301,608,430,288,
461,289,290,291,292,293,761,294,295,838,839,840,841,842,843,844,845,846,847,848,
849,850,851,852,853,332,409,410,333,334,335,296,197,396,198,531,199,200,201,202,
203,204,205,206,207,208,209,210,211,212,213,563,564,214,215,75,878,242,434};
static short Cyc_yypact[1032]={2762,- -32768,- -32768,- -32768,- -32768,- 78,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,3216,456,4445,- -32768,- 60,- 10,- -32768,- 6,7,39,73,
51,93,94,325,192,- -32768,- -32768,167,- -32768,- -32768,- -32768,159,634,194,185,132,
132,- -32768,- -32768,2627,- -32768,370,720,- -32768,313,189,3216,3216,3216,- -32768,
3216,- -32768,- -32768,656,- -32768,- 60,3136,165,107,726,815,- -32768,- -32768,198,
271,295,- -32768,- 60,311,- -32768,4538,242,- -32768,- -32768,- -32768,198,5805,-
-32768,312,347,4538,344,354,346,- -32768,70,- -32768,- -32768,3364,3364,2627,2627,
3364,387,5805,520,- -32768,277,397,585,386,277,3586,5805,- -32768,- -32768,- -32768,
2627,2896,2627,2896,42,- -32768,415,434,- -32768,3056,- -32768,- -32768,- -32768,-
-32768,3586,- -32768,- -32768,198,222,1653,- -32768,3136,189,- -32768,3364,3290,4209,
- -32768,165,- -32768,- -32768,- -32768,440,442,- -32768,- -32768,- -32768,- -32768,158,
815,3364,- -32768,2896,- -32768,457,475,499,- 60,47,- -32768,4538,221,1012,5982,498,
5805,5846,507,519,536,551,553,198,560,599,601,6074,6074,- -32768,2212,603,6115,
6115,6115,- -32768,- -32768,- -32768,204,- -32768,- -32768,- 22,612,641,598,671,663,
140,710,- 69,261,- -32768,505,6115,349,13,- -32768,689,3596,719,- -32768,- -32768,728,
5805,198,3596,731,199,3586,3660,3586,578,- -32768,85,85,- -32768,- -32768,181,717,
338,198,- -32768,- -32768,- -32768,- -32768,44,744,- -32768,- -32768,668,448,- -32768,
758,- -32768,766,- -32768,782,789,791,- -32768,585,4305,3136,- -32768,793,3586,-
-32768,600,796,- 60,806,805,383,811,3729,813,829,821,822,4401,3729,233,816,814,-
-32768,- -32768,817,1794,1794,189,1794,- -32768,- -32768,- -32768,824,- -32768,-
-32768,- -32768,- 67,- -32768,- -32768,825,810,126,842,- -32768,80,826,830,381,827,
752,819,3364,5805,- -32768,838,- -32768,- -32768,126,834,- 60,- -32768,5805,843,-
-32768,844,845,- -32768,277,5805,4538,287,- -32768,- -32768,- -32768,847,850,2212,-
-32768,3586,- -32768,- -32768,3825,- -32768,871,5805,5805,5805,5805,5805,848,5805,
3586,228,2212,- -32768,- -32768,1935,840,530,5805,- -32768,- -32768,- -32768,5805,-
-32768,6115,5805,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,
6115,6115,6115,6115,6115,5805,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,5805,- -32768,277,- -32768,- -32768,4494,277,
5805,- -32768,849,198,- -32768,- -32768,851,854,4538,- -32768,403,1012,853,3364,-
-32768,855,860,- -32768,3660,3660,3660,- -32768,- -32768,1463,4587,148,- -32768,251,
- -32768,- -32768,80,- -32768,- -32768,3364,- -32768,874,- -32768,862,- -32768,858,859,
863,- -32768,1156,- -32768,321,412,- -32768,- -32768,- -32768,3586,- -32768,- -32768,-
-32768,2627,- -32768,2627,878,869,864,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,738,5805,875,879,- -32768,867,474,198,198,837,5805,5805,876,883,
5805,961,2076,885,- -32768,- -32768,- -32768,414,962,- -32768,4683,5805,3729,2348,-
-32768,- -32768,3056,- -32768,- -32768,- -32768,3364,- -32768,- -32768,3586,881,258,-
-32768,- -32768,868,- -32768,80,882,3512,830,- -32768,- -32768,- -32768,- -32768,752,-
26,884,846,- -32768,- -32768,2492,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,886,- -32768,- -32768,- -32768,- -32768,3226,887,896,899,- -32768,- -32768,764,
4305,537,898,900,902,543,3586,546,894,613,901,904,5941,- -32768,- -32768,903,905,-
-32768,612,83,641,598,671,663,140,140,710,710,710,710,- 69,- 69,261,261,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,906,- -32768,48,3364,4113,- -32768,
6233,- -32768,- -32768,908,- -32768,98,917,- -32768,- -32768,- -32768,- -32768,907,830,
909,752,913,251,3364,3438,4776,- -32768,- -32768,85,912,198,759,911,44,2976,915,
198,3364,3290,4872,- -32768,321,- -32768,- -32768,- -32768,914,- -32768,930,715,-
-32768,- -32768,600,5805,830,- -32768,- -32768,- -32768,926,- 60,458,563,580,5805,873,
606,923,4968,5061,492,- -32768,- -32768,933,935,928,627,- -32768,3136,- -32768,810,
937,3364,- -32768,938,80,- -32768,939,- -32768,152,- -32768,- -32768,- -32768,- -32768,
846,- -32768,943,- -32768,2627,- -32768,- -32768,- -32768,946,306,944,- -32768,3921,-
-32768,- -32768,5805,1040,5805,5846,942,- -32768,- -32768,277,277,946,945,4113,-
-32768,- -32768,5805,5805,- -32768,- -32768,126,778,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,947,- -32768,- -32768,752,126,948,830,752,940,- -32768,5805,830,636,
949,950,- -32768,- -32768,951,- -32768,126,952,830,955,752,967,5805,982,- -32768,-
-32768,3586,- -32768,976,86,837,3729,983,977,3522,975,986,3729,5805,5154,556,5247,
574,5340,873,- -32768,989,992,- -32768,830,105,- -32768,- -32768,956,830,3586,-
-32768,510,- -32768,985,987,5805,- -32768,- -32768,4305,659,990,993,- -32768,871,-
-32768,996,999,- -32768,798,- -32768,- -32768,- -32768,- -32768,4113,- -32768,- -32768,
- -32768,- -32768,1000,- -32768,752,619,3586,1007,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,1004,- -32768,- -32768,752,666,- -32768,998,3586,- -32768,- -32768,
1090,873,1013,6207,1008,2348,6115,1009,- -32768,88,- -32768,1043,1010,783,832,243,
841,514,377,- -32768,- -32768,- -32768,- -32768,1048,6115,1935,- -32768,- -32768,669,
3729,679,5433,3729,683,5526,5619,645,1024,- -32768,- -32768,- -32768,- -32768,1025,-
-32768,937,- -32768,1028,673,- -32768,- -32768,174,- -32768,- -32768,3586,1123,-
-32768,- -32768,- -32768,4017,- -32768,- -32768,- -32768,1030,1031,- -32768,- -32768,-
-32768,- -32768,686,3729,1033,873,2212,- -32768,3586,1034,- -32768,1327,6115,5805,
6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,
6115,5805,- -32768,- -32768,1036,- -32768,3729,3729,690,- -32768,3729,3729,699,3729,
707,5712,- -32768,80,830,- -32768,- -32768,520,5805,1035,1037,- -32768,- -32768,1047,
- -32768,- -32768,- -32768,1044,1054,1060,6115,873,- -32768,612,266,641,598,598,663,
140,140,710,710,710,710,- 69,- 69,261,261,- -32768,- -32768,- -32768,322,- -32768,-
-32768,- -32768,3729,- -32768,- -32768,3729,- -32768,3729,3729,716,- -32768,- -32768,-
-32768,812,1056,3586,1057,- -32768,946,539,- -32768,- -32768,5805,1327,- -32768,-
-32768,- -32768,- -32768,3729,- -32768,- -32768,1058,- -32768,1059,1061,- -32768,873,-
-32768,- -32768,1062,- -32768,- -32768,- -32768,- -32768,1189,1190,- -32768};static
short Cyc_yypgoto[143]={- -32768,136,- -32768,244,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- 51,- -32768,- 105,22,- -32768,- -32768,0,569,- -32768,55,- 170,
1081,5,- -32768,- -32768,- 134,- -32768,138,1161,- 715,- -32768,- -32768,- -32768,941,
953,681,453,- -32768,- -32768,558,- -32768,- -32768,179,- -32768,- -32768,95,- 199,1130,
- 397,14,- -32768,1045,- -32768,- -32768,1153,- 342,- -32768,523,- 138,- 62,- 111,- 416,
262,532,538,- 420,- 471,- 106,- 411,- 124,- -32768,- 222,- 153,- 548,- 339,- -32768,888,-
154,144,- 120,- 58,- 318,56,- -32768,- -32768,- -32768,4,- 270,- -32768,- 349,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1,988,- -32768,674,795,-
-32768,292,614,- -32768,- 173,- 293,- 150,- 358,- 355,- 365,852,- 356,- 344,- 239,- 337,-
305,- 181,714,460,865,- -32768,- 352,- -32768,- 4,374,- 61,46,- 298,- 25};static short Cyc_yytable[
6372]={59,339,548,569,135,62,305,566,248,360,361,362,568,616,571,498,499,139,501,
96,308,59,342,260,95,343,62,572,573,535,397,76,322,306,357,536,827,464,671,317,
578,579,631,78,111,715,135,38,447,589,503,59,630,363,41,60,62,422,59,59,59,441,
59,62,62,62,379,62,59,146,380,365,142,62,580,581,60,139,96,630,129,130,131,168,
132,148,559,286,96,297,141,644,645,218,674,315,668,99,893,679,587,677,59,59,590,
631,60,62,62,433,611,136,366,60,60,60,100,60,404,59,59,59,59,60,62,62,62,62,59,
101,720,611,261,62,574,575,576,577,59,112,59,508,662,62,298,62,405,146,530,255,
141,442,128,238,169,530,102,60,60,799,287,59,141,418,256,96,62,96,363,170,323,
429,431,104,60,60,60,60,638,713,678,909,542,60,543,529,452,118,222,450,103,722,
39,60,223,60,517,556,509,558,582,583,584,363,452,- 148,244,435,432,297,437,711,96,
431,38,105,60,408,106,96,433,41,910,719,408,171,751,421,373,374,872,44,423,433,
286,286,299,286,233,234,433,108,510,428,231,232,504,624,235,432,115,422,422,422,
548,251,252,253,254,506,59,449,375,376,467,62,81,724,431,304,39,624,426,109,266,
268,505,- 213,427,476,- 213,43,141,110,313,59,59,143,59,144,62,62,527,62,363,145,
38,318,530,758,113,436,949,41,432,286,124,287,287,114,287,39,809,44,526,60,433,
813,417,349,159,47,48,49,363,96,694,821,485,364,537,917,918,324,492,38,84,187,85,
86,60,60,41,60,159,19,20,325,750,169,493,267,353,299,789,189,59,494,871,- 235,414,
62,- 235,874,170,919,920,43,548,875,39,510,123,612,74,613,440,670,287,708,196,614,
500,806,239,625,363,810,452,161,240,248,785,381,1007,665,80,97,237,98,700,382,
383,823,162,796,797,96,538,452,96,60,596,239,784,452,327,328,74,240,868,421,421,
421,739,74,423,423,423,166,615,74,398,399,400,656,216,43,739,134,428,137,363,626,
74,627,623,418,629,139,1008,628,510,165,217,523,97,59,363,59,449,219,62,439,62,
789,97,221,530,891,637,788,714,401,220,727,74,74,402,403,358,134,896,901,476,74,
59,119,120,742,38,62,726,74,74,74,74,41,304,236,59,516,728,744,666,62,741,481,
141,925,482,245,60,413,60,74,743,926,927,38,597,243,141,363,674,40,41,42,59,996,
681,659,257,62,363,74,446,96,801,418,165,60,427,97,593,97,793,967,968,794,789,
964,664,958,258,60,966,774,969,963,600,384,625,38,311,779,312,882,453,491,41,970,
971,454,79,754,452,319,755,452,617,38,788,60,976,977,930,40,41,42,97,96,731,632,
320,633,717,97,470,385,386,387,388,389,390,391,392,393,394,768,510,615,363,1006,
297,642,643,38,978,979,321,451,338,40,41,42,395,38,358,876,629,344,74,40,41,42,
549,476,962,451,553,478,555,345,483,358,239,1018,667,923,363,476,240,924,476,561,
907,363,567,1022,346,74,695,363,684,59,363,788,699,773,62,701,1027,862,929,347,
363,348,972,973,974,975,906,363,350,59,681,141,756,802,62,865,38,165,363,591,703,
399,400,41,363,807,426,97,367,757,1019,38,427,44,47,48,49,40,41,42,819,47,48,49,
60,135,899,351,470,352,363,359,471,250,950,762,47,48,49,369,892,401,1021,363,60,
38,704,403,980,981,982,363,41,959,43,960,772,371,372,96,814,733,44,310,854,815,
942,38,945,363,47,48,49,40,41,42,649,650,133,38,653,368,657,363,1005,40,41,42,
883,97,363,663,97,363,947,897,446,948,931,247,377,378,427,363,47,48,49,363,933,
286,453,406,937,370,363,955,831,775,265,987,519,520,858,363,451,783,38,451,990,
247,411,363,40,41,42,74,992,74,1020,412,363,121,122,470,438,1013,644,645,451,38,
1016,47,48,49,451,40,41,42,43,416,59,286,47,48,49,62,470,16,17,18,734,735,736,
445,47,48,49,466,603,604,605,455,287,149,150,151,510,692,693,456,337,135,340,340,
152,153,154,155,156,759,760,803,804,354,355,458,74,462,340,340,340,420,59,425,
460,60,468,62,477,97,932,888,889,936,913,914,915,916,479,340,524,921,922,480,287,
1014,363,829,830,484,487,486,488,489,495,497,496,502,265,433,259,765,767,506,522,
513,518,528,43,466,956,286,514,550,551,552,532,60,365,560,540,533,534,97,541,592,
554,562,595,601,594,565,599,602,619,618,620,622,621,634,635,636,790,985,986,654,
639,988,989,641,991,451,585,640,651,652,658,672,660,669,691,675,59,680,586,451,
689,62,451,562,690,686,696,702,697,753,698,706,812,710,705,718,709,39,287,712,
721,732,723,340,737,748,747,825,740,610,340,1009,725,752,1010,763,1011,1012,81,
769,770,771,304,859,861,776,864,74,867,782,778,60,707,791,786,795,873,1023,798,
811,805,808,816,817,818,820,881,340,822,340,340,340,340,340,340,340,340,340,340,
340,340,340,340,340,340,340,824,826,828,832,833,856,82,857,869,247,562,870,879,
562,880,885,884,894,326,898,900,167,84,886,85,86,887,890,41,247,196,895,87,902,
905,97,247,911,88,908,89,90,928,327,328,943,91,944,912,946,951,953,92,954,957,
984,935,1001,134,939,941,961,999,1000,466,1002,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,247,1003,1004,1015,1017,1024,1025,340,1026,1028,247,1030,1031,738,997,22,
246,358,107,247,749,463,147,117,965,316,781,27,995,466,780,598,777,415,0,28,29,
688,469,557,539,855,983,570,0,0,0,32,0,0,730,34,0,0,0,0,994,0,35,36,0,0,0,998,
746,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,302,0,0,38,0,562,0,0,39,40,41,42,43,304,
340,0,0,0,0,0,446,0,45,46,0,0,427,0,47,48,49,0,0,0,247,0,0,0,0,0,0,0,0,0,420,0,0,
466,247,0,0,0,792,0,0,0,0,0,0,0,0,466,134,0,800,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,759,760,21,172,173,269,340,270,271,272,273,274,275,
276,277,22,81,23,174,278,0,0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,
0,280,181,0,0,0,0,0,32,33,182,183,34,184,0,466,0,0,0,35,36,37,0,185,186,340,0,0,
0,0,466,0,0,0,0,0,0,340,0,0,0,0,0,0,265,0,281,84,187,85,86,39,40,41,42,43,282,
138,- 301,0,0,0,0,188,0,45,284,0,0,0,0,190,0,247,191,0,0,192,193,0,0,194,195,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,265,0,0,0,0,0,22,0,0,466,0,
0,0,0,0,247,0,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,
0,35,36,0,0,0,904,0,0,340,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,302,0,340,0,0,0,0,0,39,
40,0,42,43,304,0,0,0,0,0,0,426,0,45,46,0,0,427,0,47,48,49,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,340,0,340,340,340,340,340,340,340,340,340,340,340,
340,340,340,340,340,340,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,340,0,21,172,173,269,0,270,271,272,273,274,275,276,277,22,
81,23,174,278,0,0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,280,181,
0,0,0,0,340,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,281,84,187,85,86,39,40,41,42,43,282,138,283,0,0,0,0,188,
0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,172,173,269,0,270,271,272,273,274,275,276,277,22,
81,23,174,278,0,0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,- 36,180,0,280,
181,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,281,84,187,85,86,39,40,41,42,43,282,138,0,0,0,0,0,188,
0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,172,173,269,0,270,271,272,273,274,275,276,277,22,
81,23,174,278,0,0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,280,181,
0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,281,84,187,85,86,39,40,41,42,43,282,138,0,0,0,0,0,188,0,45,
284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,21,172,173,0,0,0,0,0,0,0,0,0,0,22,81,23,174,0,0,0,175,24,0,
0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,0,181,0,0,0,0,0,32,33,182,183,34,184,
0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,
85,86,39,40,41,42,43,655,0,0,0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,191,0,0,192,
193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,172,173,0,0,0,0,0,0,
0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,0,
181,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,39,40,41,42,43,0,356,0,0,0,0,0,188,0,45,
284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,0,0,176,
177,178,179,0,28,29,0,180,0,0,181,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,
0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,39,40,41,
42,43,0,0,0,0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,
1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,
0,0,22,0,23,0,0,0,683,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,
31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,38,0,0,0,0,39,40,41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,
0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,
0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,
33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
38,0,0,0,0,39,40,41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,
0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,
0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,
0,0,39,40,41,42,43,0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,
0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,
0,0,35,36,37,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,38,0,0,0,
0,39,40,41,42,43,22,0,- 11,0,0,0,0,44,0,45,46,0,27,0,0,47,48,49,0,0,28,29,0,0,0,0,
0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,38,0,0,0,0,39,40,41,42,43,22,0,23,0,0,0,0,470,24,45,46,
0,27,0,0,47,48,49,0,0,28,29,- 36,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,
37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,
0,42,43,22,138,23,0,259,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,
0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,138,0,0,0,0,0,0,24,45,46,81,27,0,
0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,82,0,0,0,0,0,0,0,0,22,39,40,687,42,43,167,
84,0,85,86,0,27,41,0,45,46,87,0,0,28,29,0,88,0,89,90,0,327,328,0,91,32,0,0,0,34,
92,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,302,0,
0,303,0,0,0,22,39,40,0,42,43,304,0,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,224,0,0,
0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,39,40,0,42,43,0,225,0,0,0,0,27,0,0,45,46,0,0,0,28,
29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,0,0,302,0,0,0,0,834,835,22,39,40,0,42,43,304,0,0,0,0,81,
27,0,0,45,46,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,82,0,676,0,0,0,0,0,0,22,39,40,0,42,
43,167,84,0,85,86,81,27,41,0,45,46,87,0,0,28,29,0,836,0,89,90,0,0,0,0,91,32,0,
837,0,34,92,193,0,0,194,195,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,
0,0,82,0,0,0,0,0,0,0,0,22,39,40,407,42,43,167,84,0,85,86,0,27,41,0,45,46,87,0,0,
28,29,0,88,0,89,90,0,0,0,0,91,32,0,0,0,34,92,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,
0,0,172,173,269,0,270,271,272,273,274,275,276,277,0,81,0,174,278,0,39,175,0,279,
43,0,0,0,0,176,177,178,179,0,45,46,0,180,0,280,181,0,0,0,0,0,0,0,182,183,0,184,0,
0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,281,84,187,85,
86,0,0,41,0,0,282,138,0,0,172,173,0,188,0,0,189,544,0,0,0,190,0,81,191,174,0,192,
193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,
183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,187,85,86,0,0,41,0,0,0,341,545,0,172,173,0,188,0,0,189,0,327,328,0,190,0,81,
191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,
0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,341,787,0,172,173,0,188,0,0,189,0,327,
328,0,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,
180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,341,952,0,172,173,0,188,
0,0,189,0,327,328,0,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,
179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,341,0,0,172,
173,0,188,0,0,189,0,327,328,0,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,
176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,
0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,
0,0,0,172,173,0,188,0,0,189,0,0,0,309,190,0,81,191,174,0,192,193,175,0,194,195,0,
0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,
0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,
0,0,0,341,0,0,172,173,0,188,0,0,189,0,0,0,0,190,0,81,191,174,0,192,193,175,0,194,
195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,
0,0,0,0,0,81,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,
0,0,41,0,0,490,172,173,0,0,0,0,188,0,0,189,0,0,0,81,190,174,0,191,82,175,192,193,
0,0,194,195,0,176,177,178,179,0,0,83,84,180,85,86,181,0,41,0,0,0,87,0,182,183,0,
184,88,0,89,90,0,0,0,81,91,185,186,0,0,0,92,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,
84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,588,0,189,0,0,0,81,190,174,0,191,
82,175,192,193,0,0,194,195,0,176,177,178,179,0,0,167,84,180,85,86,181,0,41,0,0,0,
87,0,182,183,0,184,88,0,89,90,0,0,0,0,91,185,186,0,0,0,92,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,0,189,0,0,0,609,
190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,661,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,0,189,0,0,
0,729,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,
180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,0,
189,0,0,0,745,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,
0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,764,172,173,0,0,0,0,
188,0,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,
0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,766,172,173,0,0,0,0,
188,0,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,
0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,
188,860,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,
0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,
188,863,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,
0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,866,172,173,0,0,0,
0,188,0,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,
0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,
188,934,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,
0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,
188,938,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,
0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,
188,940,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,
0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,
188,993,0,189,0,0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,
0,0,0,0,180,0,0,181,0,0,172,173,0,0,0,182,183,0,184,0,0,0,0,81,0,0,0,0,185,186,0,
0,0,0,0,0,0,176,177,178,179,0,0,0,0,0,0,0,181,0,0,38,84,187,85,86,182,183,41,184,
0,0,0,0,0,0,0,0,188,185,186,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,
0,0,38,84,187,85,86,0,0,41,0,0,0,341,0,172,173,0,0,188,0,0,189,0,0,0,0,190,81,0,
191,0,0,192,193,0,0,194,195,0,0,0,176,177,178,179,0,0,0,0,0,0,0,181,0,0,172,173,
0,0,0,182,183,0,184,0,0,0,0,81,0,0,0,0,185,186,0,0,0,0,0,0,0,176,177,178,179,0,0,
0,0,0,0,0,181,0,0,38,84,187,85,86,182,183,41,184,0,0,707,0,0,0,0,0,188,185,186,
189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,38,84,187,85,86,0,0,41,0,
0,172,173,0,0,0,0,0,336,0,0,189,0,0,81,0,190,0,0,191,0,0,192,193,0,0,194,195,176,
177,178,179,0,0,0,0,0,0,0,181,0,0,172,173,0,0,0,182,183,0,184,0,0,0,0,81,0,0,0,0,
185,186,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,0,0,0,181,0,0,38,84,187,85,86,182,
183,41,184,0,0,0,0,0,0,0,0,353,185,186,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,
194,195,0,0,0,0,38,84,187,85,86,0,0,41,0,0,172,173,0,0,0,0,0,188,0,0,189,0,0,81,
0,190,0,0,191,0,0,192,193,0,0,194,195,176,177,178,179,0,0,0,0,0,0,0,181,81,0,0,0,
0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,82,0,0,0,0,0,903,0,0,189,0,0,716,0,190,167,
84,191,85,86,192,193,41,0,194,195,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,
92};static short Cyc_yycheck[6372]={0,174,341,368,65,0,144,365,114,190,191,192,367,
433,370,285,286,68,288,23,144,21,175,128,23,175,21,371,372,327,211,109,166,144,
188,328,751,259,509,159,377,378,453,21,44,593,107,107,247,401,117,51,449,120,114,
0,51,227,58,59,60,17,62,58,59,60,135,62,68,69,139,93,68,68,379,380,21,128,82,476,
58,59,60,82,62,71,356,138,92,140,68,5,6,92,514,157,507,107,813,125,398,517,102,
103,402,516,51,102,103,135,428,65,134,58,59,60,122,62,105,119,120,121,122,68,119,
120,121,122,128,122,601,449,128,128,373,374,375,376,138,44,140,61,494,138,140,
140,133,147,321,107,128,107,57,107,107,328,117,102,103,707,138,161,140,225,122,
169,161,171,120,122,169,229,87,122,119,120,121,122,471,131,522,93,336,128,338,
319,247,51,118,247,117,607,112,138,124,140,307,351,118,353,381,382,383,120,265,
119,111,232,123,260,235,128,216,87,107,122,161,216,124,223,135,114,134,125,223,
87,642,227,88,89,125,124,227,135,285,286,141,288,102,103,135,49,304,229,100,101,
120,446,104,123,118,421,422,423,593,119,120,121,122,107,260,247,122,123,260,260,
38,609,87,117,112,470,124,106,136,137,301,125,130,265,128,116,260,124,126,285,
286,122,288,124,285,286,317,288,120,130,107,161,471,651,106,120,128,114,123,356,
117,285,286,124,288,112,728,124,314,260,135,733,119,181,122,132,133,134,120,329,
548,743,272,125,329,88,89,112,278,107,108,109,110,111,285,286,114,288,122,20,21,
126,641,107,117,129,124,258,693,127,356,124,774,117,222,356,120,779,122,122,123,
116,707,780,112,433,59,122,0,124,238,119,356,560,88,130,287,725,107,446,120,729,
449,118,113,497,690,132,128,500,22,23,106,25,554,140,141,745,109,703,704,411,121,
470,414,356,411,107,108,476,129,130,44,113,769,421,422,423,623,51,421,422,423,
118,430,57,83,84,85,486,124,116,637,65,426,67,120,122,70,124,446,509,448,500,128,
130,514,79,107,311,82,457,120,459,446,117,457,125,459,804,92,121,641,811,470,693,
592,124,120,613,102,103,129,130,188,107,824,832,470,111,486,117,118,627,107,486,
612,119,120,121,122,114,117,112,500,120,613,627,500,500,626,124,486,132,127,125,
457,221,459,141,627,140,141,107,117,124,500,120,944,113,114,115,528,945,525,117,
117,528,120,161,124,541,711,601,166,486,130,169,406,171,699,912,913,699,889,909,
496,902,120,500,911,668,914,908,416,56,623,107,124,676,124,789,120,277,114,915,
916,125,118,117,637,120,120,640,436,107,804,528,921,922,856,113,114,115,216,595,
617,457,119,459,595,223,124,94,95,96,97,98,99,100,101,102,103,117,672,611,120,
962,665,477,478,107,923,924,121,247,124,113,114,115,121,107,336,119,630,124,258,
113,114,115,344,623,908,265,348,267,350,124,270,353,107,108,504,135,120,637,113,
139,640,125,837,120,366,1008,124,287,125,120,528,665,120,889,125,665,665,125,
1021,117,855,124,120,124,917,918,919,920,836,120,124,685,681,665,125,714,685,117,
107,319,120,403,83,84,85,114,120,726,124,329,92,125,1004,107,130,124,132,133,134,
113,114,115,741,132,133,134,665,782,828,124,124,124,120,124,128,115,884,125,132,
133,134,137,117,124,1008,120,685,107,129,130,925,926,927,120,114,903,116,905,125,
90,91,759,120,619,124,145,759,125,117,107,875,120,132,133,134,113,114,115,480,
481,118,107,484,136,486,120,961,113,114,115,125,411,120,495,414,120,117,125,124,
120,125,114,86,87,130,120,132,133,134,120,125,856,120,118,125,138,120,125,756,
669,133,125,64,65,762,120,446,685,107,449,125,144,107,120,113,114,115,457,125,
459,1007,107,120,117,118,124,123,125,5,6,470,107,1000,132,133,134,476,113,114,
115,116,124,856,908,132,133,134,856,124,17,18,19,107,108,109,125,132,133,134,259,
421,422,423,119,856,64,65,66,944,119,120,119,172,948,174,175,75,76,77,78,79,22,
23,119,120,185,186,119,528,112,190,191,192,226,908,228,121,856,119,908,118,541,
860,119,120,863,137,138,90,91,118,211,312,86,87,124,908,119,120,754,755,124,107,
124,117,117,124,124,128,119,263,135,121,655,656,107,131,125,125,119,116,341,900,
1008,128,345,346,347,119,908,93,125,119,123,123,595,120,122,124,359,120,120,125,
363,125,119,118,107,124,120,125,107,117,123,696,933,934,30,117,937,938,128,940,
623,384,120,124,118,117,135,42,124,107,125,1008,125,396,637,125,1008,640,401,120,
131,120,125,120,647,120,119,732,120,125,119,125,112,1008,125,125,121,125,321,125,
107,124,747,125,427,328,987,131,119,990,124,992,993,38,118,117,125,117,763,764,
119,766,685,768,118,123,1008,118,25,122,125,112,1013,125,131,125,125,125,125,125,
125,786,365,125,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,
383,131,117,124,118,125,128,92,119,117,426,494,117,125,497,125,120,124,108,104,
119,28,107,108,125,110,111,125,125,114,446,836,125,118,118,124,759,453,92,124,
128,126,127,92,129,130,119,132,120,136,119,25,119,138,120,119,117,862,108,782,
865,866,125,125,124,548,119,8,9,10,11,12,13,14,15,16,17,18,19,20,21,497,125,120,
125,125,125,125,471,125,125,507,0,0,622,948,37,113,903,35,516,640,258,70,48,910,
158,681,49,944,593,676,414,672,223,- 1,57,58,541,263,352,330,759,928,369,- 1,- 1,- 1,
69,- 1,- 1,614,73,- 1,- 1,- 1,- 1,942,- 1,80,81,- 1,- 1,- 1,949,628,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,651,- 1,- 1,112,113,114,115,116,117,
560,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,130,- 1,132,133,134,- 1,- 1,- 1,613,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,623,- 1,- 1,693,627,- 1,- 1,- 1,698,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,707,948,
- 1,710,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
25,26,27,641,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,-
1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,789,- 1,
- 1,- 1,80,81,82,- 1,84,85,699,- 1,- 1,- 1,- 1,804,- 1,- 1,- 1,- 1,- 1,- 1,711,- 1,- 1,- 1,- 1,- 1,
- 1,751,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,124,- 1,
126,127,- 1,- 1,- 1,- 1,132,- 1,780,135,- 1,- 1,138,139,- 1,- 1,142,143,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,813,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,
889,- 1,- 1,- 1,- 1,- 1,828,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,834,- 1,- 1,837,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,855,- 1,- 1,- 1,- 1,- 1,112,113,- 1,115,116,
117,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,130,- 1,132,133,134,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,909,- 1,911,912,913,914,
915,916,917,918,919,920,921,922,923,924,925,926,927,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,961,- 1,24,25,26,
27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,
54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,1007,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,
80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,124,- 1,126,127,-
1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,
41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,
- 1,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,
118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,
142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,
29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,
- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,
- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,
109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,-
1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,
44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,69,70,
71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,- 1,- 1,- 1,
- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,
63,- 1,- 1,- 1,- 1,- 1,69,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,84,85,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,
114,115,116,- 1,118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,
138,139,- 1,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,
55,- 1,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,69,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,80,81,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,
- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,
- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,67,68,
69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,119,-
1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,0,1,- 1,3,4,5,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,
- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,116,- 1,- 1,
119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,0,1,- 1,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,116,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,133,134,1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,67,68,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,- 1,- 1,- 1,- 1,- 1,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,115,
116,37,- 1,119,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,49,- 1,- 1,132,133,134,- 1,- 1,57,58,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,- 1,3,4,5,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,107,- 1,- 1,- 1,- 1,112,113,114,
115,116,37,- 1,39,- 1,- 1,- 1,- 1,124,45,126,127,- 1,49,- 1,- 1,132,133,134,- 1,- 1,57,58,
59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,
- 1,115,116,37,118,39,- 1,121,- 1,- 1,- 1,45,126,127,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,3,4,5,6,
7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,
- 1,115,116,37,118,- 1,- 1,- 1,- 1,- 1,- 1,45,126,127,38,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
112,113,104,115,116,107,108,- 1,110,111,- 1,49,114,- 1,126,127,118,- 1,- 1,57,58,- 1,
124,- 1,126,127,- 1,129,130,- 1,132,69,- 1,- 1,- 1,73,138,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,- 1,- 1,
37,112,113,- 1,115,116,117,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,
61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,
- 1,115,116,- 1,118,- 1,- 1,- 1,- 1,49,- 1,- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,- 1,- 1,25,26,37,112,113,- 1,115,116,
117,- 1,- 1,- 1,- 1,38,49,- 1,- 1,126,127,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,- 1,- 1,- 1,- 1,92,- 1,104,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,- 1,115,116,107,108,- 1,
110,111,38,49,114,- 1,126,127,118,- 1,- 1,57,58,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,
69,- 1,135,- 1,73,138,139,- 1,- 1,142,143,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,112,113,104,115,116,107,108,-
1,110,111,- 1,49,114,- 1,126,127,118,- 1,- 1,57,58,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,
69,- 1,- 1,- 1,73,138,- 1,- 1,- 1,- 1,- 1,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,
26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,112,44,- 1,46,116,- 1,- 1,- 1,- 1,
52,53,54,55,- 1,126,127,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,118,- 1,- 1,25,26,- 1,124,- 1,- 1,127,
32,- 1,- 1,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,
- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,
110,111,- 1,- 1,114,- 1,- 1,- 1,118,119,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,-
1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,-
1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,
114,- 1,- 1,- 1,118,119,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,
138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,-
1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,
118,119,- 1,25,26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,-
1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,- 1,- 1,25,
26,- 1,124,- 1,- 1,127,- 1,129,130,- 1,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,-
1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,
127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,- 1,
132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,84,85,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,
- 1,114,- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,92,44,
138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,107,108,60,110,111,63,- 1,114,- 1,- 1,- 1,
118,- 1,71,72,- 1,74,124,- 1,126,127,- 1,- 1,- 1,38,132,84,85,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,92,44,138,139,- 1,- 1,142,
143,- 1,52,53,54,55,- 1,- 1,107,108,60,110,111,63,- 1,114,- 1,- 1,- 1,118,- 1,71,72,- 1,
74,124,- 1,126,127,- 1,- 1,- 1,- 1,132,84,85,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,
124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,
- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,
- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,
114,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,
138,139,44,- 1,142,143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,-
1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,- 1,
- 1,- 1,25,26,- 1,124,- 1,- 1,127,- 1,- 1,- 1,131,132,- 1,38,135,40,- 1,138,139,44,- 1,142,
143,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,-
1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,
124,- 1,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,- 1,- 1,114,- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,
132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,-
1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,-
1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,- 1,- 1,114,- 1,- 1,117,25,26,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,38,
132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,-
1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,-
1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,
124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,
55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,
108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,
132,40,- 1,135,- 1,44,138,139,- 1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,124,125,- 1,127,- 1,- 1,- 1,38,132,40,- 1,135,- 1,44,138,139,-
1,- 1,142,143,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,25,26,- 1,- 1,- 1,71,72,-
1,74,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,63,- 1,- 1,107,108,109,110,111,71,72,114,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
124,84,85,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,
107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,118,- 1,25,26,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,
- 1,132,38,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,63,- 1,- 1,25,26,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,84,85,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,107,108,109,110,111,71,
72,114,74,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,124,84,85,127,- 1,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,
138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,25,26,- 1,-
1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,38,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,
52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,25,26,- 1,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,
38,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,
- 1,107,108,109,110,111,71,72,114,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,84,85,127,- 1,- 1,
- 1,- 1,132,- 1,- 1,135,- 1,- 1,138,139,- 1,- 1,142,143,- 1,- 1,- 1,- 1,107,108,109,110,111,
- 1,- 1,114,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,38,- 1,132,- 1,- 1,135,- 1,
- 1,138,139,- 1,- 1,142,143,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,38,- 1,- 1,- 1,- 1,- 1,-
1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,- 1,- 1,114,- 1,- 1,- 1,92,- 1,- 1,- 1,
- 1,- 1,124,- 1,- 1,127,- 1,- 1,104,- 1,132,107,108,135,110,111,138,139,114,- 1,142,143,
118,- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,138};char Cyc_Yystack_overflow[
17]="Yystack_overflow";struct Cyc_Yystack_overflow_struct{char*tag;int f1;};struct
Cyc_Yystack_overflow_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
void Cyc_yyerror(struct _dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);
static int Cyc_yychar=(int)'\000';union Cyc_YYSTYPE Cyc_yylval={.YYINITIALSVAL={60,0}};
static int Cyc_yynerrs=0;struct _tuple25{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple26{struct _dyneither_ptr f1;void*f2;};static char
_tmp51A[8]="stdcall";static char _tmp51B[6]="cdecl";static char _tmp51C[9]="fastcall";
static char _tmp51D[9]="noreturn";static char _tmp51E[6]="const";static char _tmp51F[8]="aligned";
static char _tmp520[7]="packed";static char _tmp521[7]="shared";static char _tmp522[7]="unused";
static char _tmp523[5]="weak";static char _tmp524[10]="dllimport";static char _tmp525[
10]="dllexport";static char _tmp526[23]="no_instrument_function";static char _tmp527[
12]="constructor";static char _tmp528[11]="destructor";static char _tmp529[22]="no_check_memory_usage";
static char _tmp52A[5]="pure";struct _tuple27{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct _RegionHandle
_tmp480=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp480;
_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp836=10000;short*_tmp837=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp836));{unsigned int
_tmp838=_tmp836;unsigned int i;for(i=0;i < _tmp838;i ++){_tmp837[i]=(short)0;}}
_tmp837;});int yyvsp_offset;union Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)({
unsigned int _tmp833=10000;union Cyc_YYSTYPE*_tmp834=(union Cyc_YYSTYPE*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp833));{
unsigned int _tmp835=_tmp833;unsigned int i;for(i=0;i < _tmp835;i ++){_tmp834[i]=Cyc_yylval;}}
_tmp834;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp830=10000;struct Cyc_Yyltype*_tmp831=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp830));{
unsigned int _tmp832=_tmp830;unsigned int i;for(i=0;i < _tmp832;i ++){_tmp831[i]=Cyc_yynewloc();}}
_tmp831;});int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp481="parser stack overflow";_tag_dyneither(
_tmp481,sizeof(char),22);}));(int)_throw((void*)& Cyc_Yystack_overflow_val);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 371?(int)Cyc_yytranslate[_check_known_subscript_notnull(372,Cyc_yychar)]:
287;}yyn +=yychar1;if((yyn < 0  || yyn > 6371) || Cyc_yycheck[
_check_known_subscript_notnull(6372,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6372,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1031){int
_tmp482=0;_npop_handler(0);return _tmp482;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1032,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(511,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL21B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL21C: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp483;_push_handler(& _tmp483);{int _tmp485=0;if(setjmp(_tmp483.handler))_tmp485=
1;if(!_tmp485){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp484=(void*)_exn_thrown;void*
_tmp487=_tmp484;_LL21F: {struct Cyc_Core_Failure_struct*_tmp488=(struct Cyc_Core_Failure_struct*)
_tmp487;if(_tmp488->tag != Cyc_Core_Failure)goto _LL221;}_LL220: x=0;goto _LL21E;
_LL221:;_LL222:(void)_throw(_tmp487);_LL21E:;}};}{struct _handler_cons _tmp489;
_push_handler(& _tmp489);{int _tmp48B=0;if(setjmp(_tmp489.handler))_tmp48B=1;if(!
_tmp48B){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp48A=(void*)_exn_thrown;void*_tmp48D=_tmp48A;_LL224: {
struct Cyc_Core_Failure_struct*_tmp48E=(struct Cyc_Core_Failure_struct*)_tmp48D;
if(_tmp48E->tag != Cyc_Core_Failure)goto _LL226;}_LL225: y=0;goto _LL223;_LL226:;
_LL227:(void)_throw(_tmp48D);_LL223:;}};}yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));break;}
case 3: _LL21D: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp48F=_cycalloc(sizeof(*
_tmp48F));_tmp48F->hd=({struct Cyc_Absyn_Decl*_tmp490=_cycalloc(sizeof(*_tmp490));
_tmp490->r=(void*)({struct Cyc_Absyn_Using_d_struct*_tmp491=_cycalloc(sizeof(*
_tmp491));_tmp491[0]=({struct Cyc_Absyn_Using_d_struct _tmp492;_tmp492.tag=11;
_tmp492.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp492.f2=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp492;});_tmp491;});_tmp490->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp490;});
_tmp48F->tl=0;_tmp48F;}));Cyc_Lex_leave_using();break;case 4: _LL228: yyval=Cyc_YY18(({
struct Cyc_List_List*_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->hd=({struct Cyc_Absyn_Decl*
_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->r=(void*)({struct Cyc_Absyn_Using_d_struct*
_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495[0]=({struct Cyc_Absyn_Using_d_struct
_tmp496;_tmp496.tag=11;_tmp496.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp496.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp496;});_tmp495;});
_tmp494->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp494;});_tmp493->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp493;}));break;case 5:
_LL229: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp497=_cycalloc(sizeof(*_tmp497));
_tmp497->hd=({struct Cyc_Absyn_Decl*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->r=(
void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp499=_cycalloc(sizeof(*_tmp499));
_tmp499[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp49A;_tmp49A.tag=10;_tmp49A.f1=({
struct _dyneither_ptr*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp49B;});_tmp49A.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp49A;});
_tmp499;});_tmp498->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp498;});
_tmp497->tl=0;_tmp497;}));Cyc_Lex_leave_namespace();break;case 6: _LL22A: {struct
_dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp49C;_push_handler(& _tmp49C);{int _tmp49E=0;if(setjmp(_tmp49C.handler))
_tmp49E=1;if(!_tmp49E){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp49D=(void*)_exn_thrown;void*_tmp4A0=_tmp49D;_LL22D: {struct Cyc_Core_Failure_struct*
_tmp4A1=(struct Cyc_Core_Failure_struct*)_tmp4A0;if(_tmp4A1->tag != Cyc_Core_Failure)
goto _LL22F;}_LL22E: nspace=({const char*_tmp4A2="";_tag_dyneither(_tmp4A2,sizeof(
char),1);});goto _LL22C;_LL22F:;_LL230:(void)_throw(_tmp4A0);_LL22C:;}};}{struct
_handler_cons _tmp4A3;_push_handler(& _tmp4A3);{int _tmp4A5=0;if(setjmp(_tmp4A3.handler))
_tmp4A5=1;if(!_tmp4A5){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);;_pop_handler();}else{void*_tmp4A4=(void*)_exn_thrown;
void*_tmp4A7=_tmp4A4;_LL232: {struct Cyc_Core_Failure_struct*_tmp4A8=(struct Cyc_Core_Failure_struct*)
_tmp4A7;if(_tmp4A8->tag != Cyc_Core_Failure)goto _LL234;}_LL233: x=0;goto _LL231;
_LL234:;_LL235:(void)_throw(_tmp4A7);_LL231:;}};}{struct _handler_cons _tmp4A9;
_push_handler(& _tmp4A9);{int _tmp4AB=0;if(setjmp(_tmp4A9.handler))_tmp4AB=1;if(!
_tmp4AB){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp4AA=(void*)_exn_thrown;void*_tmp4AD=_tmp4AA;_LL237: {
struct Cyc_Core_Failure_struct*_tmp4AE=(struct Cyc_Core_Failure_struct*)_tmp4AD;
if(_tmp4AE->tag != Cyc_Core_Failure)goto _LL239;}_LL238: y=0;goto _LL236;_LL239:;
_LL23A:(void)_throw(_tmp4AD);_LL236:;}};}yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp4AF=_cycalloc(sizeof(*_tmp4AF));_tmp4AF->hd=({struct Cyc_Absyn_Decl*_tmp4B0=
_cycalloc(sizeof(*_tmp4B0));_tmp4B0->r=(void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp4B2;_tmp4B2.tag=10;_tmp4B2.f1=({struct _dyneither_ptr*_tmp4B3=_cycalloc(
sizeof(*_tmp4B3));_tmp4B3[0]=nspace;_tmp4B3;});_tmp4B2.f2=x;_tmp4B2;});_tmp4B1;});
_tmp4B0->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp4B0;});_tmp4AF->tl=y;_tmp4AF;}));break;}case 7:
_LL22B: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp4B4="C";
_tag_dyneither(_tmp4B4,sizeof(char),2);}))== 0)yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->hd=({struct Cyc_Absyn_Decl*_tmp4B6=
_cycalloc(sizeof(*_tmp4B6));_tmp4B6->r=(void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp4B8;_tmp4B8.tag=12;_tmp4B8.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4B8;});_tmp4B7;});
_tmp4B6->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp4B6;});_tmp4B5->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4B5;}));else{if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),({const char*_tmp4B9="C include";_tag_dyneither(_tmp4B9,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp4BA="expecting \"C\" or \"C include\"";
_tag_dyneither(_tmp4BA,sizeof(char),29);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=Cyc_YY18(({
struct Cyc_List_List*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->hd=({struct Cyc_Absyn_Decl*
_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC->r=(void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp4BE;_tmp4BE.tag=13;_tmp4BE.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4BE.f2=0;_tmp4BE;});
_tmp4BD;});_tmp4BC->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp4BC;});
_tmp4BB->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4BB;}));}break;case 8: _LL23B: if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp4BF="C include";_tag_dyneither(_tmp4BF,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp4C0="expecting \"C include\"";
_tag_dyneither(_tmp4C0,sizeof(char),22);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1->hd=({
struct Cyc_Absyn_Decl*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));_tmp4C2->r=(void*)({
struct Cyc_Absyn_ExternCinclude_d_struct*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));
_tmp4C3[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp4C4;_tmp4C4.tag=13;
_tmp4C4.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp4C4.f2=exs;_tmp4C4;});_tmp4C3;});_tmp4C2->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp4C2;});
_tmp4C1->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4C1;}));break;};case 9: _LL23C: yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->hd=({struct Cyc_Absyn_Decl*_tmp4C6=
_cycalloc(sizeof(*_tmp4C6));_tmp4C6->r=(void*)& Cyc_Absyn_Porton_d_val;_tmp4C6->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp4C6;});_tmp4C5->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C5;}));break;case 10:
_LL23D: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));
_tmp4C7->hd=({struct Cyc_Absyn_Decl*_tmp4C8=_cycalloc(sizeof(*_tmp4C8));_tmp4C8->r=(
void*)& Cyc_Absyn_Portoff_d_val;_tmp4C8->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp4C8;});
_tmp4C7->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4C7;}));break;case 11: _LL23E: yyval=Cyc_YY18(0);break;case 12:
_LL23F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 13: _LL240: yyval=Cyc_YY52(0);break;case 14: _LL241: yyval=Cyc_YY52(({struct Cyc_List_List*
_tmp4C9=_cycalloc(sizeof(*_tmp4C9));_tmp4C9->hd=({struct _tuple25*_tmp4CA=
_cycalloc(sizeof(*_tmp4CA));_tmp4CA->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp4CA->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4CA->f3=0;_tmp4CA;});_tmp4C9->tl=0;_tmp4C9;}));break;case 15: _LL242: yyval=Cyc_YY52(({
struct Cyc_List_List*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->hd=({struct
_tuple25*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));_tmp4CC->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp4CC->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp4CC->f3=0;_tmp4CC;});_tmp4CB->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4CB;}));break;case 16:
_LL243: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4CD=_cycalloc(sizeof(*_tmp4CD));
_tmp4CD->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*_tmp4CE=
_cycalloc(sizeof(*_tmp4CE));_tmp4CE[0]=({struct Cyc_Absyn_Fn_d_struct _tmp4CF;
_tmp4CF.tag=1;_tmp4CF.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4CF;});_tmp4CE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CD->tl=0;
_tmp4CD;}));break;case 17: _LL244: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 18: _LL245: yyval=Cyc_YY18(0);break;case 19: _LL246: yyval=
Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 20:
_LL247: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp4D0=
_cycalloc(sizeof(*_tmp4D0));_tmp4D0->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D0;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 21:
_LL248: yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 22:
_LL249: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp4D1=
_cycalloc(sizeof(*_tmp4D1));_tmp4D1->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4D1;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 23:
_LL24A: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp4D2=
_cycalloc(sizeof(*_tmp4D2));_tmp4D2->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D2;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 24:
_LL24B: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp4D3=
_cycalloc(sizeof(*_tmp4D3));_tmp4D3->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4D3;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 25:
_LL24C: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL24D: Cyc_Lex_leave_using();
break;case 27: _LL24E: Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp4D4=
_cycalloc(sizeof(*_tmp4D4));_tmp4D4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D4;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL24F: Cyc_Lex_leave_namespace();
break;case 29: _LL250: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
30: _LL251: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 31:
_LL252: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4D5=_cycalloc(sizeof(*_tmp4D5));
_tmp4D5->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4D5->tl=0;
_tmp4D5;}));break;case 32: _LL253: {struct Cyc_List_List*_tmp4D6=0;{struct Cyc_List_List*
_tmp4D7=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp4D7 != 0;_tmp4D7=_tmp4D7->tl){struct _dyneither_ptr*_tmp4D8=(struct
_dyneither_ptr*)_tmp4D7->hd;struct _tuple1*qv=({struct _tuple1*_tmp4DB=_cycalloc(
sizeof(*_tmp4DB));_tmp4DB->f1=Cyc_Absyn_Rel_n(0);_tmp4DB->f2=_tmp4D8;_tmp4DB;});
struct Cyc_Absyn_Vardecl*_tmp4D9=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
_tmp4D6=({struct Cyc_List_List*_tmp4DA=_cycalloc(sizeof(*_tmp4DA));_tmp4DA->hd=
_tmp4D9;_tmp4DA->tl=_tmp4D6;_tmp4DA;});}}_tmp4D6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4D6);yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->hd=Cyc_Absyn_letv_decl(_tmp4D6,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4DC->tl=0;
_tmp4DC;}));break;}case 33: _LL254: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp4DD="`H";
_tag_dyneither(_tmp4DD,sizeof(char),3);}))== 0)Cyc_Parse_err((struct
_dyneither_ptr)({void*_tmp4DE=0;Cyc_aprintf(({const char*_tmp4DF="bad occurrence of heap region";
_tag_dyneither(_tmp4DF,sizeof(char),30);}),_tag_dyneither(_tmp4DE,sizeof(void*),
0));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp4E0="`U";
_tag_dyneither(_tmp4E0,sizeof(char),3);}))== 0)Cyc_Parse_err((struct
_dyneither_ptr)({void*_tmp4E1=0;Cyc_aprintf(({const char*_tmp4E2="bad occurrence of unique region";
_tag_dyneither(_tmp4E2,sizeof(char),32);}),_tag_dyneither(_tmp4E1,sizeof(void*),
0));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->name=({struct _dyneither_ptr*_tmp4EB=
_cycalloc(sizeof(*_tmp4EB));_tmp4EB[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4EB;});_tmp4EA->identity=
- 1;_tmp4EA->kind=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_RgnKind);_tmp4EA;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));
_tmp4E8[0]=({struct Cyc_Absyn_VarType_struct _tmp4E9;_tmp4E9.tag=2;_tmp4E9.f1=tv;
_tmp4E9;});_tmp4E8;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->f1=Cyc_Absyn_Loc_n;_tmp4E4->f2=({
struct _dyneither_ptr*_tmp4E5=_cycalloc(sizeof(*_tmp4E5));_tmp4E5[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E5;});_tmp4E4;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));
_tmp4E6[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp4E7;_tmp4E7.tag=16;_tmp4E7.f1=(
void*)t;_tmp4E7;});_tmp4E6;}),0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4E3=
_cycalloc(sizeof(*_tmp4E3));_tmp4E3->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4E3->tl=0;
_tmp4E3;}));break;};case 34: _LL255: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({const char*_tmp4EC="H";
_tag_dyneither(_tmp4EC,sizeof(char),2);}))== 0)Cyc_Parse_err(({const char*_tmp4ED="bad occurrence of heap region `H";
_tag_dyneither(_tmp4ED,sizeof(char),33);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({const char*_tmp4EE="U";_tag_dyneither(_tmp4EE,sizeof(
char),2);}))== 0)Cyc_Parse_err(({const char*_tmp4EF="bad occurrence of unique region `U";
_tag_dyneither(_tmp4EF,sizeof(char),35);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)Cyc_Parse_err(({
const char*_tmp4F0="open regions cannot be @resetable";_tag_dyneither(_tmp4F0,
sizeof(char),34);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->name=({
struct _dyneither_ptr*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp4FC;_tmp4FC.tag=0;_tmp4FC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));({void*_tmp4FA[1]={&
_tmp4FC};Cyc_aprintf(({const char*_tmp4FB="`%s";_tag_dyneither(_tmp4FB,sizeof(
char),4);}),_tag_dyneither(_tmp4FA,sizeof(void*),1));});});_tmp4F9;});_tmp4F8->identity=
- 1;_tmp4F8->kind=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_RgnKind);_tmp4F8;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));
_tmp4F6[0]=({struct Cyc_Absyn_VarType_struct _tmp4F7;_tmp4F7.tag=2;_tmp4F7.f1=tv;
_tmp4F7;});_tmp4F6;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->f1=Cyc_Absyn_Loc_n;_tmp4F2->f2=({
struct _dyneither_ptr*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));_tmp4F3[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F3;});_tmp4F2;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));
_tmp4F4[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp4F5;_tmp4F5.tag=16;_tmp4F5.f1=(
void*)t;_tmp4F5;});_tmp4F4;}),0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4F1=
_cycalloc(sizeof(*_tmp4F1));_tmp4F1->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F1->tl=0;
_tmp4F1;}));break;};case 35: _LL256: {struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->name=({struct _dyneither_ptr*_tmp503=
_cycalloc(sizeof(*_tmp503));_tmp503[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp503;});_tmp500->identity=
- 1;_tmp500->kind=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp501=_cycalloc_atomic(
sizeof(*_tmp501));_tmp501[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp502;_tmp502.tag=0;
_tmp502.f1=Cyc_Absyn_RgnKind;_tmp502;});_tmp501;});_tmp500;});struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));
_tmp4FE->f1=Cyc_Absyn_Loc_n;_tmp4FE->f2=({struct _dyneither_ptr*_tmp4FF=_cycalloc(
sizeof(*_tmp4FF));_tmp4FF[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4FF;});_tmp4FE;}),(
void*)& Cyc_Absyn_VoidType_val,0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4FD=
_cycalloc(sizeof(*_tmp4FD));_tmp4FD->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4FD->tl=0;
_tmp4FD;}));break;}case 36: _LL257: yyval=Cyc_YY31(0);break;case 37: _LL258: yyval=Cyc_YY31(
1);break;case 38: _LL259: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 39: _LL25A: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 40: _LL25B: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp504=_cycalloc(sizeof(*_tmp504));
_tmp504->sc=({struct Cyc_Core_Opt*_tmp505=_cycalloc_atomic(sizeof(*_tmp505));
_tmp505->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp505;});_tmp504->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp504->type_specs=
0;_tmp504->is_inline=0;_tmp504->attributes=0;_tmp504;}));break;case 41: _LL25C: if((
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 
0)({void*_tmp506=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp507="Only one storage class is allowed in a declaration";_tag_dyneither(
_tmp507,sizeof(char),51);}),_tag_dyneither(_tmp506,sizeof(void*),0));});yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp508=_cycalloc(sizeof(*_tmp508));
_tmp508->sc=({struct Cyc_Core_Opt*_tmp509=_cycalloc_atomic(sizeof(*_tmp509));
_tmp509->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp509;});_tmp508->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp508->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp508->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp508->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp508;}));
break;case 42: _LL25D: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp50A=
_cycalloc(sizeof(*_tmp50A));_tmp50A->sc=0;_tmp50A->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp50A->type_specs=({
struct Cyc_List_List*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50B->tl=0;_tmp50B;});
_tmp50A->is_inline=0;_tmp50A->attributes=0;_tmp50A;}));break;case 43: _LL25E: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp50C=_cycalloc(sizeof(*_tmp50C));
_tmp50C->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp50C->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp50C->type_specs=({
struct Cyc_List_List*_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp50D->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp50D;});
_tmp50C->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp50C->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp50C;}));
break;case 44: _LL25F: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp50E=
_cycalloc(sizeof(*_tmp50E));_tmp50E->sc=0;_tmp50E->tq=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50E->type_specs=0;
_tmp50E->is_inline=0;_tmp50E->attributes=0;_tmp50E;}));break;case 45: _LL260: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp50F=_cycalloc(sizeof(*_tmp50F));
_tmp50F->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp50F->tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp50F->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp50F->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp50F->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp50F;}));
break;case 46: _LL261: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp510=
_cycalloc(sizeof(*_tmp510));_tmp510->sc=0;_tmp510->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp510->type_specs=
0;_tmp510->is_inline=1;_tmp510->attributes=0;_tmp510;}));break;case 47: _LL262:
yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp511=_cycalloc(sizeof(*
_tmp511));_tmp511->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp511->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp511->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp511->is_inline=1;_tmp511->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp511;}));
break;case 48: _LL263: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp512=
_cycalloc(sizeof(*_tmp512));_tmp512->sc=0;_tmp512->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp512->type_specs=
0;_tmp512->is_inline=0;_tmp512->attributes=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp512;}));break;case 49:
_LL264: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp513=_cycalloc(
sizeof(*_tmp513));_tmp513->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp513->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp513->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp513->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp513->attributes=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp513;}));
break;case 50: _LL265: yyval=Cyc_YY22(Cyc_Parse_Auto_sc);break;case 51: _LL266: yyval=
Cyc_YY22(Cyc_Parse_Register_sc);break;case 52: _LL267: yyval=Cyc_YY22(Cyc_Parse_Static_sc);
break;case 53: _LL268: yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 54: _LL269: if(
Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp514="C";
_tag_dyneither(_tmp514,sizeof(char),2);}))!= 0)Cyc_Parse_err(({const char*_tmp515="only extern or extern \"C\" is allowed";
_tag_dyneither(_tmp515,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY22(
Cyc_Parse_ExternC_sc);break;case 55: _LL26A: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);
break;case 56: _LL26B: yyval=Cyc_YY22(Cyc_Parse_Abstract_sc);break;case 57: _LL26C:
yyval=Cyc_YY45(0);break;case 58: _LL26D: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 59: _LL26E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 60: _LL26F: yyval=
Cyc_YY45(({struct Cyc_List_List*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->hd=
Cyc_yyget_YY46(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp516->tl=
0;_tmp516;}));break;case 61: _LL270: yyval=Cyc_YY45(({struct Cyc_List_List*_tmp517=
_cycalloc(sizeof(*_tmp517));_tmp517->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp517->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp517;}));break;case
62: _LL271: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};static
struct _tuple26 att_map[17]={{{_tmp51A,_tmp51A,_tmp51A + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
_tmp51B,_tmp51B,_tmp51B + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp51C,_tmp51C,
_tmp51C + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp51D,_tmp51D,_tmp51D + 9},(
void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp51E,_tmp51E,_tmp51E + 6},(void*)& Cyc_Absyn_Const_att_val},{{
_tmp51F,_tmp51F,_tmp51F + 8},(void*)& att_aligned},{{_tmp520,_tmp520,_tmp520 + 7},(
void*)& Cyc_Absyn_Packed_att_val},{{_tmp521,_tmp521,_tmp521 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{
_tmp522,_tmp522,_tmp522 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp523,_tmp523,
_tmp523 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp524,_tmp524,_tmp524 + 10},(void*)&
Cyc_Absyn_Dllimport_att_val},{{_tmp525,_tmp525,_tmp525 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{
_tmp526,_tmp526,_tmp526 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{
_tmp527,_tmp527,_tmp527 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp528,
_tmp528,_tmp528 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp529,_tmp529,
_tmp529 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp52A,_tmp52A,
_tmp52A + 5},(void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp518=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp518,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp518,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp518,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp518,sizeof(char),(int)(_get_dyneither_size(
_tmp518,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp518,sizeof(char),(int)(_get_dyneither_size(_tmp518,sizeof(char))- 3)))== '_')
_tmp518=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp518,2,
_get_dyneither_size(_tmp518,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp518,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){Cyc_Parse_err(({const char*_tmp519="unrecognized attribute";
_tag_dyneither(_tmp519,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY46((
void*)& Cyc_Absyn_Cdecl_att_val);}break;};}case 63: _LL272: yyval=Cyc_YY46((void*)&
Cyc_Absyn_Const_att_val);break;case 64: _LL273: {struct _dyneither_ptr _tmp52C=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp52E;struct
_tuple6 _tmp52D=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp52E=_tmp52D.f2;{void*a;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52C,({const char*_tmp52F="regparm";_tag_dyneither(_tmp52F,
sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,({const char*
_tmp530="__regparm__";_tag_dyneither(_tmp530,sizeof(char),12);}))== 0){if(
_tmp52E < 0  || _tmp52E > 3)Cyc_Parse_err(({const char*_tmp531="regparm requires value between 0 and 3";
_tag_dyneither(_tmp531,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp532=_cycalloc_atomic(sizeof(*_tmp532));
_tmp532[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp533;_tmp533.tag=0;_tmp533.f1=
_tmp52E;_tmp533;});_tmp532;});}else{if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,({
const char*_tmp534="aligned";_tag_dyneither(_tmp534,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,({const char*_tmp535="__aligned__";
_tag_dyneither(_tmp535,sizeof(char),12);}))== 0){if(_tmp52E < 0)Cyc_Parse_err(({
const char*_tmp536="aligned requires positive power of two";_tag_dyneither(
_tmp536,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp52E;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp537="aligned requires positive power of two";
_tag_dyneither(_tmp537,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp538=_cycalloc_atomic(sizeof(*_tmp538));
_tmp538[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp539;_tmp539.tag=6;_tmp539.f1=
_tmp52E;_tmp539;});_tmp538;});};}else{if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp52C,({const char*_tmp53A="initializes";_tag_dyneither(_tmp53A,sizeof(char),12);}))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,({const char*_tmp53B="__initializes__";
_tag_dyneither(_tmp53B,sizeof(char),16);}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp53C=_cycalloc_atomic(sizeof(*_tmp53C));_tmp53C[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp53D;_tmp53D.tag=20;_tmp53D.f1=_tmp52E;_tmp53D;});_tmp53C;});else{Cyc_Parse_err(({
const char*_tmp53E="unrecognized attribute";_tag_dyneither(_tmp53E,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}yyval=Cyc_YY46(
a);break;};}case 65: _LL274: {struct _dyneither_ptr _tmp53F=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct
_dyneither_ptr _tmp540=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53F,({
const char*_tmp541="section";_tag_dyneither(_tmp541,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp53F,({const char*_tmp542="__section__";
_tag_dyneither(_tmp542,sizeof(char),12);}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543[0]=({struct Cyc_Absyn_Section_att_struct
_tmp544;_tmp544.tag=8;_tmp544.f1=_tmp540;_tmp544;});_tmp543;});else{Cyc_Parse_err(({
const char*_tmp545="unrecognized attribute";_tag_dyneither(_tmp545,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)& Cyc_Absyn_Cdecl_att_val;}yyval=Cyc_YY46(
a);break;}case 66: _LL275: {struct _dyneither_ptr _tmp546=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _dyneither_ptr
_tmp547=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp546,({const
char*_tmp548="__mode__";_tag_dyneither(_tmp548,sizeof(char),9);}))== 0)a=(void*)({
struct Cyc_Absyn_Mode_att_struct*_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549[0]=({
struct Cyc_Absyn_Mode_att_struct _tmp54A;_tmp54A.tag=22;_tmp54A.f1=_tmp547;_tmp54A;});
_tmp549;});else{Cyc_Parse_err(({const char*_tmp54B="unrecognized attribute";
_tag_dyneither(_tmp54B,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)&
Cyc_Absyn_Cdecl_att_val;}yyval=Cyc_YY46(a);break;}case 67: _LL276: {struct
_dyneither_ptr _tmp54C=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp54D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp54F;struct _tuple6
_tmp54E=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp54F=_tmp54E.f2;{int _tmp551;struct _tuple6 _tmp550=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp551=_tmp550.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp54C,({const char*_tmp552="format";_tag_dyneither(_tmp552,sizeof(char),7);}))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp54C,({const char*_tmp553="__format__";
_tag_dyneither(_tmp553,sizeof(char),11);}))== 0){if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp54D,({const char*_tmp554="printf";_tag_dyneither(_tmp554,
sizeof(char),7);}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp555=
_cycalloc_atomic(sizeof(*_tmp555));_tmp555[0]=({struct Cyc_Absyn_Format_att_struct
_tmp556;_tmp556.tag=19;_tmp556.f1=Cyc_Absyn_Printf_ft;_tmp556.f2=_tmp54F;_tmp556.f3=
_tmp551;_tmp556;});_tmp555;});else{if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54D,({
const char*_tmp557="scanf";_tag_dyneither(_tmp557,sizeof(char),6);}))== 0)a=(void*)({
struct Cyc_Absyn_Format_att_struct*_tmp558=_cycalloc_atomic(sizeof(*_tmp558));
_tmp558[0]=({struct Cyc_Absyn_Format_att_struct _tmp559;_tmp559.tag=19;_tmp559.f1=
Cyc_Absyn_Scanf_ft;_tmp559.f2=_tmp54F;_tmp559.f3=_tmp551;_tmp559;});_tmp558;});
else{Cyc_Parse_err(({const char*_tmp55A="unrecognized format type";_tag_dyneither(
_tmp55A,sizeof(char),25);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp55B="unrecognized attribute";_tag_dyneither(_tmp55B,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}yyval=Cyc_YY46(a);break;};};}case 68: _LL277: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 69: _LL278:
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp55D;_tmp55D.tag=18;_tmp55D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp55D.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp55D.f3=0;_tmp55D.f4=
0;_tmp55D;});_tmp55C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 70:
_LL279: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 71:
_LL27A: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 72:
_LL27B: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 73:
_LL27C: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 74:
_LL27D: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Short_spec_struct*_tmp55E=
_cycalloc(sizeof(*_tmp55E));_tmp55E[0]=({struct Cyc_Parse_Short_spec_struct
_tmp55F;_tmp55F.tag=2;_tmp55F.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp55F;});
_tmp55E;}));break;case 75: _LL27E: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 76: _LL27F: yyval=Cyc_YY23((void*)({struct
Cyc_Parse_Long_spec_struct*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560[0]=({
struct Cyc_Parse_Long_spec_struct _tmp561;_tmp561.tag=3;_tmp561.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp561;});
_tmp560;}));break;case 77: _LL280: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 78: _LL281: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 79:
_LL282: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Signed_spec_struct*_tmp562=
_cycalloc(sizeof(*_tmp562));_tmp562[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp563;_tmp563.tag=0;_tmp563.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp563;});
_tmp562;}));break;case 80: _LL283: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp564=_cycalloc(sizeof(*_tmp564));_tmp564[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp565;_tmp565.tag=1;_tmp565.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp565;});
_tmp564;}));break;case 81: _LL284: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 82: _LL285: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 83: _LL286: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL287: yyval=
Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 85:
_LL288: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TupleType_struct*
_tmp566=_cycalloc(sizeof(*_tmp566));_tmp566[0]=({struct Cyc_Absyn_TupleType_struct
_tmp567;_tmp567.tag=11;_tmp567.f1=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(
struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp567;});_tmp566;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 86: _LL289: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp568=_cycalloc(sizeof(*_tmp568));
_tmp568[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp569;_tmp569.tag=16;_tmp569.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp569;});_tmp568;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 87:
_LL28A: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp56B;_tmp56B.tag=16;_tmp56B.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0);_tmp56B;});_tmp56A;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 88:
_LL28B: {void*_tmp56C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp56E;_tmp56E.tag=17;_tmp56E.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp56E.f2=(void*)
_tmp56C;_tmp56E;});_tmp56D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 89:
_LL28C: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp570;_tmp570.tag=17;_tmp570.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp570.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp570;});_tmp56F;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 90: _LL28D: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp571=_cycalloc(sizeof(*_tmp571));
_tmp571[0]=({struct Cyc_Absyn_TagType_struct _tmp572;_tmp572.tag=20;_tmp572.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp572;});_tmp571;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 91:
_LL28E: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573[0]=({struct Cyc_Absyn_TagType_struct
_tmp574;_tmp574.tag=20;_tmp574.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ik,0);_tmp574;});_tmp573;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 92:
_LL28F: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_ValueofType_struct*
_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp576;_tmp576.tag=19;_tmp576.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp576;});_tmp575;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 93: _LL290: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 94: _LL291: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;yyval=Cyc_YY25(({
struct Cyc_Absyn_Tqual _tmp577;_tmp577.print_const=1;_tmp577.q_volatile=0;_tmp577.q_restrict=
0;_tmp577.real_const=1;_tmp577.loc=loc;_tmp577;}));break;}case 95: _LL292: yyval=
Cyc_YY25(({struct Cyc_Absyn_Tqual _tmp578;_tmp578.print_const=0;_tmp578.q_volatile=
1;_tmp578.q_restrict=0;_tmp578.real_const=0;_tmp578.loc=0;_tmp578;}));break;case
96: _LL293: yyval=Cyc_YY25(({struct Cyc_Absyn_Tqual _tmp579;_tmp579.print_const=0;
_tmp579.q_volatile=0;_tmp579.q_restrict=1;_tmp579.real_const=0;_tmp579.loc=0;
_tmp579;}));break;case 97: _LL294: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp57B;_tmp57B.tag=5;_tmp57B.f1=({struct Cyc_Absyn_Decl*_tmp57C=_cycalloc(
sizeof(*_tmp57C));_tmp57C->r=(void*)({struct Cyc_Absyn_Enum_d_struct*_tmp57D=
_cycalloc(sizeof(*_tmp57D));_tmp57D[0]=({struct Cyc_Absyn_Enum_d_struct _tmp57E;
_tmp57E.tag=8;_tmp57E.f1=({struct Cyc_Absyn_Enumdecl*_tmp57F=_cycalloc(sizeof(*
_tmp57F));_tmp57F->sc=Cyc_Absyn_Public;_tmp57F->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp57F->fields=({struct
Cyc_Core_Opt*_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->v=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp580;});_tmp57F;});
_tmp57E;});_tmp57D;});_tmp57C->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp57C;});
_tmp57B;});_tmp57A;}));break;case 98: _LL295: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp581=_cycalloc(sizeof(*_tmp581));
_tmp581[0]=({struct Cyc_Absyn_EnumType_struct _tmp582;_tmp582.tag=14;_tmp582.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp582.f2=0;_tmp582;});_tmp581;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 99:
_LL296: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Type_spec_struct*_tmp583=
_cycalloc(sizeof(*_tmp583));_tmp583[0]=({struct Cyc_Parse_Type_spec_struct _tmp584;
_tmp584.tag=4;_tmp584.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp586;_tmp586.tag=15;_tmp586.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp586;});_tmp585;}));
_tmp584.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp584;});_tmp583;}));break;case 100: _LL297: yyval=Cyc_YY47(({
struct Cyc_Absyn_Enumfield*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp587->tag=0;_tmp587->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp587;}));break;case 101: _LL298: yyval=Cyc_YY47(({
struct Cyc_Absyn_Enumfield*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp588->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp588->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp588;}));
break;case 102: _LL299: yyval=Cyc_YY48(({struct Cyc_List_List*_tmp589=_cycalloc(
sizeof(*_tmp589));_tmp589->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp589->tl=0;_tmp589;}));break;case 103: _LL29A: yyval=Cyc_YY48(({
struct Cyc_List_List*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->hd=Cyc_yyget_YY47(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58A->tl=0;
_tmp58A;}));break;case 104: _LL29B: yyval=Cyc_YY48(({struct Cyc_List_List*_tmp58B=
_cycalloc(sizeof(*_tmp58B));_tmp58B->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp58B->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58B;}));break;case
105: _LL29C: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp58D;_tmp58D.tag=13;_tmp58D.f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp58D.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58D;});_tmp58C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 106: _LL29D: {struct Cyc_List_List*_tmp58E=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp58F=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp590=_cycalloc(sizeof(*_tmp590));
_tmp590[0]=({struct Cyc_Parse_Decl_spec_struct _tmp591;_tmp591.tag=5;_tmp591.f1=
Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp58E,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp58F,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp591;});
_tmp590;}));break;}case 107: _LL29E: {struct Cyc_List_List*_tmp592=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp593=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp595;_tmp595.tag=5;_tmp595.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp592,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp593,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp595;});
_tmp594;}));break;}case 108: _LL29F: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AggrType_struct*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596[0]=({
struct Cyc_Absyn_AggrType_struct _tmp597;_tmp597.tag=12;_tmp597.f1=({struct Cyc_Absyn_AggrInfo
_tmp598;_tmp598.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({struct Cyc_Core_Opt*
_tmp599=_cycalloc_atomic(sizeof(*_tmp599));_tmp599->v=(void*)1;_tmp599;}));
_tmp598.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp598;});_tmp597;});_tmp596;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 109:
_LL2A0: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AggrType_struct*
_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A[0]=({struct Cyc_Absyn_AggrType_struct
_tmp59B;_tmp59B.tag=12;_tmp59B.f1=({struct Cyc_Absyn_AggrInfo _tmp59C;_tmp59C.aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),0);_tmp59C.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59C;});_tmp59B;});
_tmp59A;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 110: _LL2A1: yyval=Cyc_YY40(0);break;case
111: _LL2A2: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
break;case 112: _LL2A3: yyval=Cyc_YY24(Cyc_Absyn_StructA);break;case 113: _LL2A4:
yyval=Cyc_YY24(Cyc_Absyn_UnionA);break;case 114: _LL2A5: yyval=Cyc_YY26(0);break;
case 115: _LL2A6: {struct Cyc_List_List*_tmp59D=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));
struct Cyc_List_List*_tmp59E=Cyc_Parse_get_argrfield_tags(_tmp59D);if(_tmp59E != 0)((
void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,
_tmp59E,_tmp59D);yyval=Cyc_YY26(_tmp59D);break;}case 116: _LL2A7: yyval=Cyc_YY27(({
struct Cyc_List_List*_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->hd=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59F->tl=0;_tmp59F;}));
break;case 117: _LL2A8: yyval=Cyc_YY27(({struct Cyc_List_List*_tmp5A0=_cycalloc(
sizeof(*_tmp5A0));_tmp5A0->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp5A0->tl=Cyc_yyget_YY27(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5A0;}));break;case
118: _LL2A9: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));
break;case 119: _LL2AA: yyval=Cyc_YY21(({struct Cyc_List_List*_tmp5A1=_cycalloc(
sizeof(*_tmp5A1));_tmp5A1->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp5A1->tl=0;_tmp5A1;}));break;case 120: _LL2AB: yyval=Cyc_YY21(({
struct Cyc_List_List*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A2->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A2;}));break;
case 121: _LL2AC: yyval=Cyc_YY20(({struct _tuple22*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));
_tmp5A3->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5A3->f2=0;_tmp5A3;}));break;case 122: _LL2AD: yyval=Cyc_YY20(({
struct _tuple22*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A4->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5A4;}));break;case 123: _LL2AE: {struct _RegionHandle _tmp5A5=
_new_region("temp");struct _RegionHandle*temp=& _tmp5A5;_push_region(temp);{struct
_tuple23 _tmp5A7;struct Cyc_Absyn_Tqual _tmp5A8;struct Cyc_List_List*_tmp5A9;struct
Cyc_List_List*_tmp5AA;struct _tuple23*_tmp5A6=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A7=*_tmp5A6;_tmp5A8=
_tmp5A7.f1;_tmp5A9=_tmp5A7.f2;_tmp5AA=_tmp5A7.f3;if(_tmp5A8.loc == 0)_tmp5A8.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp5AC;struct Cyc_List_List*
_tmp5AD;struct _tuple0 _tmp5AB=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp5AC=_tmp5AB.f1;
_tmp5AD=_tmp5AB.f2;{void*_tmp5AE=Cyc_Parse_speclist2typ(_tmp5A9,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp5AF=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp5A8,_tmp5AE,_tmp5AC,_tmp5AA),_tmp5AD);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple15*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp5AF));};};}_npop_handler(0);break;;_pop_region(
temp);}case 124: _LL2AF: yyval=Cyc_YY35(({struct _tuple23*_tmp5B0=_cycalloc(sizeof(*
_tmp5B0));_tmp5B0->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp5B0->f2=({
struct Cyc_List_List*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B1->tl=0;_tmp5B1;});
_tmp5B0->f3=0;_tmp5B0;}));break;case 125: _LL2B0: yyval=Cyc_YY35(({struct _tuple23*
_tmp5B2=_cycalloc(sizeof(*_tmp5B2));_tmp5B2->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp5B2->f2=({struct Cyc_List_List*
_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B3->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5B3;});_tmp5B2->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp5B2;}));break;case 126: _LL2B1: yyval=Cyc_YY35(({struct _tuple23*_tmp5B4=
_cycalloc(sizeof(*_tmp5B4));_tmp5B4->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B4->f2=0;_tmp5B4->f3=0;
_tmp5B4;}));break;case 127: _LL2B2: yyval=Cyc_YY35(({struct _tuple23*_tmp5B5=
_cycalloc(sizeof(*_tmp5B5));_tmp5B5->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp5B5->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5B5->f3=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp5B5;}));break;
case 128: _LL2B3: yyval=Cyc_YY35(({struct _tuple23*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));
_tmp5B6->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp5B6->f2=0;
_tmp5B6->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5B6;}));break;case 129: _LL2B4: yyval=Cyc_YY35(({struct _tuple23*
_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp5B7->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5B7->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp5B7;}));break;case 130: _LL2B5: yyval=Cyc_YY35(({struct _tuple23*_tmp5B8=
_cycalloc(sizeof(*_tmp5B8));_tmp5B8->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp5B8->f2=({
struct Cyc_List_List*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B9->tl=0;_tmp5B9;});
_tmp5B8->f3=0;_tmp5B8;}));break;case 131: _LL2B6: yyval=Cyc_YY35(({struct _tuple23*
_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp5BA->f2=({struct Cyc_List_List*
_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5BB->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5BB;});_tmp5BA->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp5BA;}));break;case 132: _LL2B7: yyval=Cyc_YY35(({struct _tuple23*_tmp5BC=
_cycalloc(sizeof(*_tmp5BC));_tmp5BC->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5BC->f2=0;_tmp5BC->f3=0;
_tmp5BC;}));break;case 133: _LL2B8: yyval=Cyc_YY35(({struct _tuple23*_tmp5BD=
_cycalloc(sizeof(*_tmp5BD));_tmp5BD->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp5BD->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5BD->f3=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp5BD;}));break;
case 134: _LL2B9: yyval=Cyc_YY35(({struct _tuple23*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));
_tmp5BE->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp5BE->f2=0;
_tmp5BE->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5BE;}));break;case 135: _LL2BA: yyval=Cyc_YY35(({struct _tuple23*
_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp5BF->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5BF->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp5BF;}));break;case 136: _LL2BB: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 137: _LL2BC:
yyval=Cyc_YY21(({struct Cyc_List_List*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->hd=
Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C0->tl=
0;_tmp5C0;}));break;case 138: _LL2BD: yyval=Cyc_YY21(({struct Cyc_List_List*_tmp5C1=
_cycalloc(sizeof(*_tmp5C1));_tmp5C1->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C1->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5C1;}));break;
case 139: _LL2BE: yyval=Cyc_YY20(({struct _tuple22*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));
_tmp5C2->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5C2->f2=0;_tmp5C2;}));break;case 140: _LL2BF: yyval=Cyc_YY20(({
struct _tuple22*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->f1=({struct Cyc_Parse_Declarator*
_tmp5C4=_cycalloc(sizeof(*_tmp5C4));_tmp5C4->id=({struct _tuple1*_tmp5C5=
_cycalloc(sizeof(*_tmp5C5));_tmp5C5->f1=Cyc_Absyn_Rel_n(0);_tmp5C5->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
_tmp5C5;});_tmp5C4->tms=0;_tmp5C4;});_tmp5C3->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C3;}));break;case
141: _LL2C0: yyval=Cyc_YY20(({struct _tuple22*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));
_tmp5C7->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5C7->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C7;}));break;case 142:
_LL2C1: {int _tmp5C8=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_List_List*_tmp5C9=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));
_tmp5CA[0]=({struct Cyc_Parse_Decl_spec_struct _tmp5CB;_tmp5CB.tag=5;_tmp5CB.f1=
Cyc_Absyn_datatype_decl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp5C9,({struct Cyc_Core_Opt*
_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CC;}),_tmp5C8,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp5CB;});
_tmp5CA;}));break;}case 143: _LL2C2: {int _tmp5CD=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_DatatypeType_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));
_tmp5CE[0]=({struct Cyc_Absyn_DatatypeType_struct _tmp5CF;_tmp5CF.tag=3;_tmp5CF.f1=({
struct Cyc_Absyn_DatatypeInfo _tmp5D0;_tmp5D0.datatype_info=Cyc_Absyn_UnknownDatatype(({
struct Cyc_Absyn_UnknownDatatypeInfo _tmp5D1;_tmp5D1.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D1.is_extensible=
_tmp5CD;_tmp5D1;}));_tmp5D0.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D0;});_tmp5CF;});
_tmp5CE;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 144: _LL2C3: {int _tmp5D2=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_DatatypeFieldType_struct*_tmp5D3=_cycalloc(sizeof(*
_tmp5D3));_tmp5D3[0]=({struct Cyc_Absyn_DatatypeFieldType_struct _tmp5D4;_tmp5D4.tag=
4;_tmp5D4.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp5D5;_tmp5D5.field_info=Cyc_Absyn_UnknownDatatypefield(({
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp5D6;_tmp5D6.datatype_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5D6.field_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5D6.is_extensible=_tmp5D2;_tmp5D6;}));_tmp5D5.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D5;});_tmp5D4;});
_tmp5D3;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 145: _LL2C4: yyval=Cyc_YY31(0);break;case
146: _LL2C5: yyval=Cyc_YY31(1);break;case 147: _LL2C6: yyval=Cyc_YY34(({struct Cyc_List_List*
_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D7->tl=0;_tmp5D7;}));
break;case 148: _LL2C7: yyval=Cyc_YY34(({struct Cyc_List_List*_tmp5D8=_cycalloc(
sizeof(*_tmp5D8));_tmp5D8->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5D8->tl=0;_tmp5D8;}));break;case 149: _LL2C8: yyval=
Cyc_YY34(({struct Cyc_List_List*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->hd=
Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5D9->tl=Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5D9;}));break;case 150: _LL2C9: yyval=Cyc_YY34(({struct Cyc_List_List*
_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5DA->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DA;}));break;case
151: _LL2CA: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 152: _LL2CB: yyval=Cyc_YY32(
Cyc_Absyn_Extern);break;case 153: _LL2CC: yyval=Cyc_YY32(Cyc_Absyn_Static);break;
case 154: _LL2CD: yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp5DB=_cycalloc(
sizeof(*_tmp5DB));_tmp5DB->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DB->typs=0;_tmp5DB->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5DB->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DB;}));break;case
155: _LL2CE: {struct Cyc_List_List*_tmp5DC=((struct Cyc_List_List*(*)(struct
_tuple17*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY33(({
struct Cyc_Absyn_Datatypefield*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5DD->typs=_tmp5DC;_tmp5DD->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5DD->sc=Cyc_yyget_YY32(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp5DD;}));break;}
case 156: _LL2CF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 157: _LL2D0: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp5DE=
_cycalloc(sizeof(*_tmp5DE));_tmp5DE->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp5DE->tms=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp5DE;}));
break;case 158: _LL2D1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 159: _LL2D2: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp5DF->tms=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp5DF;}));
break;case 160: _LL2D3: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp5E0=
_cycalloc(sizeof(*_tmp5E0));_tmp5E0->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E0->tms=0;_tmp5E0;}));
break;case 161: _LL2D4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 162: _LL2D5: {struct Cyc_Parse_Declarator*_tmp5E1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E1->tms=({
struct Cyc_List_List*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));
_tmp5E3[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5E4;_tmp5E4.tag=5;_tmp5E4.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp5E4.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5E4;});_tmp5E3;});
_tmp5E2->tl=_tmp5E1->tms;_tmp5E2;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 163: _LL2D6: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5E5->tms=({
struct Cyc_List_List*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->hd=(void*)({
struct Cyc_Absyn_Carray_mod_struct*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7[0]=({
struct Cyc_Absyn_Carray_mod_struct _tmp5E8;_tmp5E8.tag=0;_tmp5E8.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E8.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5E8;});
_tmp5E7;});_tmp5E6->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp5E6;});_tmp5E5;}));break;case 164: _LL2D7: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5E9->tms=({
struct Cyc_List_List*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA->hd=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));
_tmp5EB[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp5EC;_tmp5EC.tag=1;_tmp5EC.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5EC.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5EC.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5EC;});_tmp5EB;});_tmp5EA->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5EA;});
_tmp5E9;}));break;case 165: _LL2D8: {struct _tuple24 _tmp5EE;struct Cyc_List_List*
_tmp5EF;int _tmp5F0;struct Cyc_Absyn_VarargInfo*_tmp5F1;struct Cyc_Core_Opt*_tmp5F2;
struct Cyc_List_List*_tmp5F3;struct _tuple24*_tmp5ED=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5EE=*_tmp5ED;_tmp5EF=
_tmp5EE.f1;_tmp5F0=_tmp5EE.f2;_tmp5F1=_tmp5EE.f3;_tmp5F2=_tmp5EE.f4;_tmp5F3=
_tmp5EE.f5;yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp5F4=_cycalloc(sizeof(*
_tmp5F4));_tmp5F4->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5F4->tms=({struct Cyc_List_List*_tmp5F5=_cycalloc(
sizeof(*_tmp5F5));_tmp5F5->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp5F7;_tmp5F7.tag=3;_tmp5F7.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp5F9;_tmp5F9.tag=1;_tmp5F9.f1=_tmp5EF;_tmp5F9.f2=_tmp5F0;_tmp5F9.f3=_tmp5F1;
_tmp5F9.f4=_tmp5F2;_tmp5F9.f5=_tmp5F3;_tmp5F9;});_tmp5F8;}));_tmp5F7;});_tmp5F6;});
_tmp5F5->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp5F5;});_tmp5F4;}));break;}case 166: _LL2D9: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5FA->tms=({
struct Cyc_List_List*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp5FD;_tmp5FD.tag=3;_tmp5FD.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));
_tmp5FE[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5FF;_tmp5FF.tag=1;_tmp5FF.f1=0;
_tmp5FF.f2=0;_tmp5FF.f3=0;_tmp5FF.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5FF.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5FF;});_tmp5FE;}));
_tmp5FD;});_tmp5FC;});_tmp5FB->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5FB;});_tmp5FA;}));
break;case 167: _LL2DA: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp600=
_cycalloc(sizeof(*_tmp600));_tmp600->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp600->tms=({
struct Cyc_List_List*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp603;_tmp603.tag=3;_tmp603.f1=(void*)((
void*)({struct Cyc_Absyn_NoTypes_struct*_tmp604=_cycalloc(sizeof(*_tmp604));
_tmp604[0]=({struct Cyc_Absyn_NoTypes_struct _tmp605;_tmp605.tag=0;_tmp605.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp605.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp605;});
_tmp604;}));_tmp603;});_tmp602;});_tmp601->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp601;});_tmp600;}));
break;case 168: _LL2DB: {struct Cyc_List_List*_tmp606=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp607->tms=({
struct Cyc_List_List*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->hd=(void*)({
struct Cyc_Absyn_TypeParams_mod_struct*_tmp609=_cycalloc(sizeof(*_tmp609));
_tmp609[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp60A;_tmp60A.tag=4;_tmp60A.f1=
_tmp606;_tmp60A.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp60A.f3=0;
_tmp60A;});_tmp609;});_tmp608->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp608;});_tmp607;}));
break;}case 169: _LL2DC: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp60B=
_cycalloc(sizeof(*_tmp60B));_tmp60B->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp60B->tms=({
struct Cyc_List_List*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp60D=_cycalloc(sizeof(*_tmp60D));
_tmp60D[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp60E;_tmp60E.tag=5;_tmp60E.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp60E.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60E;});_tmp60D;});
_tmp60C->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp60C;});_tmp60B;}));break;case 170: _LL2DD: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60F->tms=0;_tmp60F;}));
break;case 171: _LL2DE: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp610=
_cycalloc(sizeof(*_tmp610));_tmp610->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp610->tms=0;_tmp610;}));
break;case 172: _LL2DF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 173: _LL2E0: {struct Cyc_Parse_Declarator*_tmp611=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp611->tms=({
struct Cyc_List_List*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp613=_cycalloc(sizeof(*_tmp613));
_tmp613[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp614;_tmp614.tag=5;_tmp614.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp614.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp614;});_tmp613;});
_tmp612->tl=_tmp611->tms;_tmp612;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 174: _LL2E1: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp615->tms=({
struct Cyc_List_List*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->hd=(void*)({
struct Cyc_Absyn_Carray_mod_struct*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617[0]=({
struct Cyc_Absyn_Carray_mod_struct _tmp618;_tmp618.tag=0;_tmp618.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp618.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp618;});
_tmp617;});_tmp616->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp616;});_tmp615;}));break;case 175: _LL2E2: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp619->tms=({
struct Cyc_List_List*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->hd=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp61B=_cycalloc(sizeof(*_tmp61B));
_tmp61B[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp61C;_tmp61C.tag=1;_tmp61C.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp61C.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp61C.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp61C;});_tmp61B;});_tmp61A->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp61A;});
_tmp619;}));break;case 176: _LL2E3: {struct _tuple24 _tmp61E;struct Cyc_List_List*
_tmp61F;int _tmp620;struct Cyc_Absyn_VarargInfo*_tmp621;struct Cyc_Core_Opt*_tmp622;
struct Cyc_List_List*_tmp623;struct _tuple24*_tmp61D=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp61E=*_tmp61D;_tmp61F=
_tmp61E.f1;_tmp620=_tmp61E.f2;_tmp621=_tmp61E.f3;_tmp622=_tmp61E.f4;_tmp623=
_tmp61E.f5;yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp624=_cycalloc(sizeof(*
_tmp624));_tmp624->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp624->tms=({struct Cyc_List_List*_tmp625=_cycalloc(
sizeof(*_tmp625));_tmp625->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp627;_tmp627.tag=3;_tmp627.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp629;_tmp629.tag=1;_tmp629.f1=_tmp61F;_tmp629.f2=_tmp620;_tmp629.f3=_tmp621;
_tmp629.f4=_tmp622;_tmp629.f5=_tmp623;_tmp629;});_tmp628;}));_tmp627;});_tmp626;});
_tmp625->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp625;});_tmp624;}));break;}case 177: _LL2E4: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp62A->tms=({
struct Cyc_List_List*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp62D;_tmp62D.tag=3;_tmp62D.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp62E=_cycalloc(sizeof(*_tmp62E));
_tmp62E[0]=({struct Cyc_Absyn_WithTypes_struct _tmp62F;_tmp62F.tag=1;_tmp62F.f1=0;
_tmp62F.f2=0;_tmp62F.f3=0;_tmp62F.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp62F.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp62F;});_tmp62E;}));
_tmp62D;});_tmp62C;});_tmp62B->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp62B;});_tmp62A;}));
break;case 178: _LL2E5: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp630=
_cycalloc(sizeof(*_tmp630));_tmp630->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp630->tms=({
struct Cyc_List_List*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp633;_tmp633.tag=3;_tmp633.f1=(void*)((
void*)({struct Cyc_Absyn_NoTypes_struct*_tmp634=_cycalloc(sizeof(*_tmp634));
_tmp634[0]=({struct Cyc_Absyn_NoTypes_struct _tmp635;_tmp635.tag=0;_tmp635.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp635.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp635;});
_tmp634;}));_tmp633;});_tmp632;});_tmp631->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp631;});_tmp630;}));
break;case 179: _LL2E6: {struct Cyc_List_List*_tmp636=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp637->tms=({
struct Cyc_List_List*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->hd=(void*)({
struct Cyc_Absyn_TypeParams_mod_struct*_tmp639=_cycalloc(sizeof(*_tmp639));
_tmp639[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp63A;_tmp63A.tag=4;_tmp63A.f1=
_tmp636;_tmp63A.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp63A.f3=0;
_tmp63A;});_tmp639;});_tmp638->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp638;});_tmp637;}));
break;}case 180: _LL2E7: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp63B=
_cycalloc(sizeof(*_tmp63B));_tmp63B->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp63B->tms=({
struct Cyc_List_List*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp63D=_cycalloc(sizeof(*_tmp63D));
_tmp63D[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp63E;_tmp63E.tag=5;_tmp63E.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp63E.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63E;});_tmp63D;});
_tmp63C->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp63C;});_tmp63B;}));break;case 181: _LL2E8: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 182: _LL2E9: yyval=
Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 183: _LL2EA: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->hd=(void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp641;_tmp641.tag=5;_tmp641.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp641.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp641;});_tmp640;});_tmp63F->tl=ans;_tmp63F;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;struct Cyc_Position_Segment*_tmp643;union Cyc_Absyn_Constraint*_tmp644;union Cyc_Absyn_Constraint*
_tmp645;struct _tuple19 _tmp642=*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp643=_tmp642.f1;_tmp644=_tmp642.f2;_tmp645=_tmp642.f3;
if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp646=_cycalloc(
sizeof(*_tmp646));_tmp646->ptr_loc=_tmp643;_tmp646->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp646->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp646;});{union Cyc_Absyn_Constraint*_tmp648;
union Cyc_Absyn_Constraint*_tmp649;union Cyc_Absyn_Constraint*_tmp64A;void*_tmp64B;
struct _tuple13 _tmp647=Cyc_Parse_collapse_pointer_quals(_tmp643,_tmp644,_tmp645,
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));_tmp648=_tmp647.f1;
_tmp649=_tmp647.f2;_tmp64A=_tmp647.f3;_tmp64B=_tmp647.f4;ans=({struct Cyc_List_List*
_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->hd=(void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp64E;_tmp64E.tag=2;_tmp64E.f1=({struct Cyc_Absyn_PtrAtts _tmp64F;_tmp64F.rgn=
_tmp64B;_tmp64F.nullable=_tmp648;_tmp64F.bounds=_tmp649;_tmp64F.zero_term=
_tmp64A;_tmp64F.ptrloc=ptrloc;_tmp64F;});_tmp64E.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64E;});_tmp64D;});
_tmp64C->tl=ans;_tmp64C;});yyval=Cyc_YY28(ans);break;};};}case 184: _LL2EB: yyval=
Cyc_YY54(0);break;case 185: _LL2EC: yyval=Cyc_YY54(({struct Cyc_List_List*_tmp650=
_cycalloc(sizeof(*_tmp650));_tmp650->hd=Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp650->tl=Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp650;}));break;case
186: _LL2ED: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651[0]=({struct Cyc_Absyn_Numelts_ptrqual_struct
_tmp652;_tmp652.tag=0;_tmp652.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp652;});_tmp651;}));
break;case 187: _LL2EE: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Region_ptrqual_struct*
_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653[0]=({struct Cyc_Absyn_Region_ptrqual_struct
_tmp654;_tmp654.tag=1;_tmp654.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp654;});_tmp653;}));
break;case 188: _LL2EF: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Thin_ptrqual_struct*
_tmp655=_cycalloc_atomic(sizeof(*_tmp655));_tmp655[0]=({struct Cyc_Absyn_Thin_ptrqual_struct
_tmp656;_tmp656.tag=2;_tmp656;});_tmp655;}));break;case 189: _LL2F0: yyval=Cyc_YY53((
void*)({struct Cyc_Absyn_Fat_ptrqual_struct*_tmp657=_cycalloc_atomic(sizeof(*
_tmp657));_tmp657[0]=({struct Cyc_Absyn_Fat_ptrqual_struct _tmp658;_tmp658.tag=3;
_tmp658;});_tmp657;}));break;case 190: _LL2F1: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Zeroterm_ptrqual_struct*
_tmp659=_cycalloc_atomic(sizeof(*_tmp659));_tmp659[0]=({struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmp65A;_tmp65A.tag=4;_tmp65A;});_tmp659;}));break;case 191: _LL2F2: yyval=Cyc_YY53((
void*)({struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmp65B=_cycalloc_atomic(
sizeof(*_tmp65B));_tmp65B[0]=({struct Cyc_Absyn_Nozeroterm_ptrqual_struct _tmp65C;
_tmp65C.tag=5;_tmp65C;});_tmp65B;}));break;case 192: _LL2F3: yyval=Cyc_YY53((void*)({
struct Cyc_Absyn_Notnull_ptrqual_struct*_tmp65D=_cycalloc_atomic(sizeof(*_tmp65D));
_tmp65D[0]=({struct Cyc_Absyn_Notnull_ptrqual_struct _tmp65E;_tmp65E.tag=6;_tmp65E;});
_tmp65D;}));break;case 193: _LL2F4: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Nullable_ptrqual_struct*
_tmp65F=_cycalloc_atomic(sizeof(*_tmp65F));_tmp65F[0]=({struct Cyc_Absyn_Nullable_ptrqual_struct
_tmp660;_tmp660.tag=7;_tmp660;});_tmp65F;}));break;case 194: _LL2F5: {struct Cyc_Position_Segment*
loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);yyval=Cyc_YY1(({struct _tuple19*_tmp661=_cycalloc(
sizeof(*_tmp661));_tmp661->f1=loc;_tmp661->f2=Cyc_Absyn_true_conref;_tmp661->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp661;}));
break;}case 195: _LL2F6: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);yyval=Cyc_YY1(({
struct _tuple19*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->f1=loc;_tmp662->f2=
Cyc_Absyn_false_conref;_tmp662->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp662;}));break;}case 196:
_LL2F7: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);yyval=Cyc_YY1(({
struct _tuple19*_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663->f1=loc;_tmp663->f2=
Cyc_Absyn_true_conref;_tmp663->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp663;}));
break;}case 197: _LL2F8: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);break;case 198:
_LL2F9: yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp664=_cycalloc(sizeof(*_tmp664));
_tmp664[0]=({struct Cyc_Absyn_Upper_b_struct _tmp665;_tmp665.tag=1;_tmp665.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp665;});_tmp664;})));
break;case 199: _LL2FA: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 200: _LL2FB: yyval=Cyc_YY51(Cyc_Absyn_true_conref);break;case 201: _LL2FC:
yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 202: _LL2FD: yyval=Cyc_YY44(Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));break;case 203: _LL2FE: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_TopRgnKind,
1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 204:
_LL2FF: yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));
break;case 205: _LL300: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
206: _LL301: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 207: _LL302:
yyval=Cyc_YY39(({struct _tuple24*_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp666->f2=0;
_tmp666->f3=0;_tmp666->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp666->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp666;}));break;case 208:
_LL303: yyval=Cyc_YY39(({struct _tuple24*_tmp667=_cycalloc(sizeof(*_tmp667));
_tmp667->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));
_tmp667->f2=1;_tmp667->f3=0;_tmp667->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp667->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp667;}));break;case
209: _LL304: {struct _tuple8 _tmp669;struct Cyc_Core_Opt*_tmp66A;struct Cyc_Absyn_Tqual
_tmp66B;void*_tmp66C;struct _tuple8*_tmp668=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp669=*_tmp668;_tmp66A=
_tmp669.f1;_tmp66B=_tmp669.f2;_tmp66C=_tmp669.f3;{struct Cyc_Absyn_VarargInfo*
_tmp66D=({struct Cyc_Absyn_VarargInfo*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->name=
_tmp66A;_tmp66F->tq=_tmp66B;_tmp66F->type=_tmp66C;_tmp66F->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp66F;});yyval=
Cyc_YY39(({struct _tuple24*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->f1=0;
_tmp66E->f2=0;_tmp66E->f3=_tmp66D;_tmp66E->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp66E->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66E;}));break;};}
case 210: _LL305: {struct _tuple8 _tmp671;struct Cyc_Core_Opt*_tmp672;struct Cyc_Absyn_Tqual
_tmp673;void*_tmp674;struct _tuple8*_tmp670=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp671=*_tmp670;_tmp672=
_tmp671.f1;_tmp673=_tmp671.f2;_tmp674=_tmp671.f3;{struct Cyc_Absyn_VarargInfo*
_tmp675=({struct Cyc_Absyn_VarargInfo*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->name=
_tmp672;_tmp677->tq=_tmp673;_tmp677->type=_tmp674;_tmp677->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp677;});yyval=
Cyc_YY39(({struct _tuple24*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp676->f2=0;
_tmp676->f3=_tmp675;_tmp676->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp676->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp676;}));break;};}
case 211: _LL306: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp679;_tmp679.tag=1;_tmp679.f1=0;_tmp679;});_tmp678;})));break;case 212: _LL307:
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 213: _LL308: yyval=Cyc_YY49(0);break;case 214: _LL309: yyval=Cyc_YY49(({
struct Cyc_Core_Opt*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->v=(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp67C;_tmp67C.tag=24;_tmp67C.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67C;});_tmp67B;});
_tmp67A;}));break;case 215: _LL30A: yyval=Cyc_YY50(0);break;case 216: _LL30B: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 217: _LL30C: {
struct Cyc_Absyn_Less_kb_struct*_tmp67D=({struct Cyc_Absyn_Less_kb_struct*_tmp684=
_cycalloc(sizeof(*_tmp684));_tmp684[0]=({struct Cyc_Absyn_Less_kb_struct _tmp685;
_tmp685.tag=2;_tmp685.f1=0;_tmp685.f2=Cyc_Absyn_TopRgnKind;_tmp685;});_tmp684;});
struct _dyneither_ptr _tmp67E=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp67F=Cyc_Parse_id2type(
_tmp67E,(void*)_tmp67D);yyval=Cyc_YY50(({struct Cyc_List_List*_tmp680=_cycalloc(
sizeof(*_tmp680));_tmp680->hd=({struct _tuple10*_tmp681=_cycalloc(sizeof(*_tmp681));
_tmp681->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp682=_cycalloc(sizeof(*
_tmp682));_tmp682[0]=({struct Cyc_Absyn_JoinEff_struct _tmp683;_tmp683.tag=24;
_tmp683.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);_tmp683;});_tmp682;});_tmp681->f2=_tmp67F;_tmp681;});_tmp680->tl=0;
_tmp680;}));break;}case 218: _LL30D: {struct Cyc_Absyn_Less_kb_struct*_tmp686=({
struct Cyc_Absyn_Less_kb_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=({
struct Cyc_Absyn_Less_kb_struct _tmp68E;_tmp68E.tag=2;_tmp68E.f1=0;_tmp68E.f2=Cyc_Absyn_TopRgnKind;
_tmp68E;});_tmp68D;});struct _dyneither_ptr _tmp687=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp688=Cyc_Parse_id2type(
_tmp687,(void*)_tmp686);yyval=Cyc_YY50(({struct Cyc_List_List*_tmp689=_cycalloc(
sizeof(*_tmp689));_tmp689->hd=({struct _tuple10*_tmp68A=_cycalloc(sizeof(*_tmp68A));
_tmp68A->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp68B=_cycalloc(sizeof(*
_tmp68B));_tmp68B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp68C;_tmp68C.tag=24;
_tmp68C.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp68C;});_tmp68B;});_tmp68A->f2=_tmp688;_tmp68A;});_tmp689->tl=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp689;}));break;}
case 219: _LL30E: yyval=Cyc_YY31(0);break;case 220: _LL30F: if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),({const char*_tmp68F="inject";_tag_dyneither(_tmp68F,sizeof(char),
7);}))!= 0)Cyc_Parse_err(({const char*_tmp690="missing type in function declaration";
_tag_dyneither(_tmp690,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY31(
1);break;case 221: _LL310: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 222: _LL311: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 223: _LL312:
yyval=Cyc_YY40(0);break;case 224: _LL313: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 225: _LL314: yyval=Cyc_YY40(({struct Cyc_List_List*
_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->hd=(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp693;_tmp693.tag=25;_tmp693.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp693;});_tmp692;});
_tmp691->tl=0;_tmp691;}));break;case 226: _LL315: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_EffKind,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694->hd=
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp694->tl=
0;_tmp694;}));break;case 227: _LL316: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_TopRgnKind,1);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->hd=(
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp696=_cycalloc(sizeof(*_tmp696));
_tmp696[0]=({struct Cyc_Absyn_AccessEff_struct _tmp697;_tmp697.tag=23;_tmp697.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp697;});_tmp696;});_tmp695->tl=0;_tmp695;}));break;case 228: _LL317: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Absyn_TopRgnKind,
1);yyval=Cyc_YY40(({struct Cyc_List_List*_tmp698=_cycalloc(sizeof(*_tmp698));
_tmp698->hd=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp699=_cycalloc(sizeof(*
_tmp699));_tmp699[0]=({struct Cyc_Absyn_AccessEff_struct _tmp69A;_tmp69A.tag=23;
_tmp69A.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp69A;});_tmp699;});_tmp698->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp698;}));break;case 229:
_LL318: yyval=Cyc_YY38(({struct Cyc_List_List*_tmp69B=_cycalloc(sizeof(*_tmp69B));
_tmp69B->hd=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp69B->tl=0;_tmp69B;}));break;case 230: _LL319: yyval=Cyc_YY38(({
struct Cyc_List_List*_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69C->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp69C;}));break;
case 231: _LL31A: {struct _tuple23 _tmp69E;struct Cyc_Absyn_Tqual _tmp69F;struct Cyc_List_List*
_tmp6A0;struct Cyc_List_List*_tmp6A1;struct _tuple23*_tmp69D=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69E=*_tmp69D;_tmp69F=
_tmp69E.f1;_tmp6A0=_tmp69E.f2;_tmp6A1=_tmp69E.f3;if(_tmp69F.loc == 0)_tmp69F.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp6A3;struct _tuple1*
_tmp6A4;struct Cyc_List_List*_tmp6A5;struct Cyc_Parse_Declarator*_tmp6A2=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A3=*_tmp6A2;
_tmp6A4=_tmp6A3.id;_tmp6A5=_tmp6A3.tms;{void*_tmp6A6=Cyc_Parse_speclist2typ(
_tmp6A0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp6A8;void*_tmp6A9;struct
Cyc_List_List*_tmp6AA;struct Cyc_List_List*_tmp6AB;struct _tuple12 _tmp6A7=Cyc_Parse_apply_tms(
_tmp69F,_tmp6A6,_tmp6A1,_tmp6A5);_tmp6A8=_tmp6A7.f1;_tmp6A9=_tmp6A7.f2;_tmp6AA=
_tmp6A7.f3;_tmp6AB=_tmp6A7.f4;if(_tmp6AA != 0)Cyc_Parse_err(({const char*_tmp6AC="parameter with bad type params";
_tag_dyneither(_tmp6AC,sizeof(char),31);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp6A4))Cyc_Parse_err(({const char*_tmp6AD="parameter cannot be qualified with a namespace";
_tag_dyneither(_tmp6AD,sizeof(char),47);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp6AE=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp6B2=_cycalloc(sizeof(*
_tmp6B2));_tmp6B2->v=(*_tmp6A4).f2;_tmp6B2;});if(_tmp6AB != 0)({void*_tmp6AF=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6B0="extra attributes on parameter, ignoring";
_tag_dyneither(_tmp6B0,sizeof(char),40);}),_tag_dyneither(_tmp6AF,sizeof(void*),
0));});yyval=Cyc_YY37(({struct _tuple8*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));
_tmp6B1->f1=_tmp6AE;_tmp6B1->f2=_tmp6A8;_tmp6B1->f3=_tmp6A9;_tmp6B1;}));break;};};};}
case 232: _LL31B: {struct _tuple23 _tmp6B4;struct Cyc_Absyn_Tqual _tmp6B5;struct Cyc_List_List*
_tmp6B6;struct Cyc_List_List*_tmp6B7;struct _tuple23*_tmp6B3=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B4=*_tmp6B3;_tmp6B5=
_tmp6B4.f1;_tmp6B6=_tmp6B4.f2;_tmp6B7=_tmp6B4.f3;if(_tmp6B5.loc == 0)_tmp6B5.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp6B8=Cyc_Parse_speclist2typ(_tmp6B6,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6B7 != 0)({
void*_tmp6B9=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6BA="bad attributes on parameter, ignoring";_tag_dyneither(_tmp6BA,sizeof(
char),38);}),_tag_dyneither(_tmp6B9,sizeof(void*),0));});yyval=Cyc_YY37(({struct
_tuple8*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB->f1=0;_tmp6BB->f2=_tmp6B5;
_tmp6BB->f3=_tmp6B8;_tmp6BB;}));break;};}case 233: _LL31C: {struct _tuple23 _tmp6BD;
struct Cyc_Absyn_Tqual _tmp6BE;struct Cyc_List_List*_tmp6BF;struct Cyc_List_List*
_tmp6C0;struct _tuple23*_tmp6BC=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6BD=*_tmp6BC;_tmp6BE=
_tmp6BD.f1;_tmp6BF=_tmp6BD.f2;_tmp6C0=_tmp6BD.f3;if(_tmp6BE.loc == 0)_tmp6BE.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp6C1=Cyc_Parse_speclist2typ(_tmp6BF,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6C2=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp6C4;void*_tmp6C5;struct Cyc_List_List*_tmp6C6;struct Cyc_List_List*
_tmp6C7;struct _tuple12 _tmp6C3=Cyc_Parse_apply_tms(_tmp6BE,_tmp6C1,_tmp6C0,
_tmp6C2);_tmp6C4=_tmp6C3.f1;_tmp6C5=_tmp6C3.f2;_tmp6C6=_tmp6C3.f3;_tmp6C7=
_tmp6C3.f4;if(_tmp6C6 != 0)({void*_tmp6C8=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6C9="bad type parameters on formal argument, ignoring";_tag_dyneither(
_tmp6C9,sizeof(char),49);}),_tag_dyneither(_tmp6C8,sizeof(void*),0));});if(
_tmp6C7 != 0)({void*_tmp6CA=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6CB="bad attributes on parameter, ignoring";_tag_dyneither(_tmp6CB,sizeof(
char),38);}),_tag_dyneither(_tmp6CA,sizeof(void*),0));});yyval=Cyc_YY37(({struct
_tuple8*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC->f1=0;_tmp6CC->f2=_tmp6C4;
_tmp6CC->f3=_tmp6C5;_tmp6CC;}));break;};}case 234: _LL31D: yyval=Cyc_YY36(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 235: _LL31E:
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD->hd=({
struct _dyneither_ptr*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6CE;});_tmp6CD->tl=
0;_tmp6CD;}));break;case 236: _LL31F: yyval=Cyc_YY36(({struct Cyc_List_List*_tmp6CF=
_cycalloc(sizeof(*_tmp6CF));_tmp6CF->hd=({struct _dyneither_ptr*_tmp6D0=_cycalloc(
sizeof(*_tmp6D0));_tmp6D0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6D0;});_tmp6CF->tl=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CF;}));break;
case 237: _LL320: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 238: _LL321: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 239: _LL322: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));
_tmp6D1[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp6D2;_tmp6D2.tag=37;
_tmp6D2.f1=0;_tmp6D2.f2=0;_tmp6D2;});_tmp6D1;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 240:
_LL323: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp6D3=_cycalloc(sizeof(*_tmp6D3));_tmp6D3[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp6D4;_tmp6D4.tag=37;_tmp6D4.f1=0;_tmp6D4.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp6D4;});_tmp6D3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 241: _LL324: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));
_tmp6D5[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp6D6;_tmp6D6.tag=37;
_tmp6D6.f1=0;_tmp6D6.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp6D6;});_tmp6D5;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 242:
_LL325: {struct Cyc_Absyn_Vardecl*_tmp6D7=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA->f1=Cyc_Absyn_Loc_n;_tmp6DA->f2=({
struct _dyneither_ptr*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp6DB;});_tmp6DA;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6D7->tq).real_const=
1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp6D9;_tmp6D9.tag=29;_tmp6D9.f1=_tmp6D7;_tmp6D9.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6D9.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D9.f4=0;
_tmp6D9;});_tmp6D8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 243:
_LL326: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));
_tmp6DC->hd=({struct _tuple27*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD->f1=0;
_tmp6DD->f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6DD;});_tmp6DC->tl=0;_tmp6DC;}));break;case 244: _LL327: yyval=Cyc_YY6(({struct
Cyc_List_List*_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE->hd=({struct _tuple27*
_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DF->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6DF;});_tmp6DE->tl=
0;_tmp6DE;}));break;case 245: _LL328: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp6E0=
_cycalloc(sizeof(*_tmp6E0));_tmp6E0->hd=({struct _tuple27*_tmp6E1=_cycalloc(
sizeof(*_tmp6E1));_tmp6E1->f1=0;_tmp6E1->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E1;});_tmp6E0->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6E0;}));break;
case 246: _LL329: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp6E2=_cycalloc(sizeof(*
_tmp6E2));_tmp6E2->hd=({struct _tuple27*_tmp6E3=_cycalloc(sizeof(*_tmp6E3));
_tmp6E3->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6E3->f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6E3;});_tmp6E2->tl=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6E2;}));break;case
247: _LL32A: yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
break;case 248: _LL32B: yyval=Cyc_YY41(({struct Cyc_List_List*_tmp6E4=_cycalloc(
sizeof(*_tmp6E4));_tmp6E4->hd=Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6E4->tl=0;_tmp6E4;}));break;case 249: _LL32C: yyval=Cyc_YY41(({
struct Cyc_List_List*_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5->hd=Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E5->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E5;}));break;
case 250: _LL32D: yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp6E7;_tmp6E7.tag=0;_tmp6E7.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E7;});_tmp6E6;}));
break;case 251: _LL32E: yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp6E8=_cycalloc(sizeof(*_tmp6E8));_tmp6E8[0]=({struct Cyc_Absyn_FieldName_struct
_tmp6E9;_tmp6E9.tag=1;_tmp6E9.f1=({struct _dyneither_ptr*_tmp6EA=_cycalloc(
sizeof(*_tmp6EA));_tmp6EA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6EA;});_tmp6E9;});
_tmp6E8;}));break;case 252: _LL32F: {struct _tuple23 _tmp6EC;struct Cyc_Absyn_Tqual
_tmp6ED;struct Cyc_List_List*_tmp6EE;struct Cyc_List_List*_tmp6EF;struct _tuple23*
_tmp6EB=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6EC=*_tmp6EB;_tmp6ED=_tmp6EC.f1;_tmp6EE=_tmp6EC.f2;_tmp6EF=_tmp6EC.f3;{void*
_tmp6F0=Cyc_Parse_speclist2typ(_tmp6EE,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6EF != 0)({
void*_tmp6F1=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6F2="ignoring attributes in type";_tag_dyneither(_tmp6F2,sizeof(char),28);}),
_tag_dyneither(_tmp6F1,sizeof(void*),0));});yyval=Cyc_YY37(({struct _tuple8*
_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3->f1=0;_tmp6F3->f2=_tmp6ED;_tmp6F3->f3=
_tmp6F0;_tmp6F3;}));break;};}case 253: _LL330: {struct _tuple23 _tmp6F5;struct Cyc_Absyn_Tqual
_tmp6F6;struct Cyc_List_List*_tmp6F7;struct Cyc_List_List*_tmp6F8;struct _tuple23*
_tmp6F4=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6F5=*_tmp6F4;_tmp6F6=_tmp6F5.f1;_tmp6F7=_tmp6F5.f2;_tmp6F8=_tmp6F5.f3;{void*
_tmp6F9=Cyc_Parse_speclist2typ(_tmp6F7,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6FA=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple12 _tmp6FB=Cyc_Parse_apply_tms(_tmp6F6,_tmp6F9,_tmp6F8,_tmp6FA);if(
_tmp6FB.f3 != 0)({void*_tmp6FC=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6FD="bad type params, ignoring";_tag_dyneither(_tmp6FD,sizeof(char),26);}),
_tag_dyneither(_tmp6FC,sizeof(void*),0));});if(_tmp6FB.f4 != 0)({void*_tmp6FE=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6FF="bad specifiers, ignoring";
_tag_dyneither(_tmp6FF,sizeof(char),25);}),_tag_dyneither(_tmp6FE,sizeof(void*),
0));});yyval=Cyc_YY37(({struct _tuple8*_tmp700=_cycalloc(sizeof(*_tmp700));
_tmp700->f1=0;_tmp700->f2=_tmp6FB.f1;_tmp700->f3=_tmp6FB.f2;_tmp700;}));break;};}
case 254: _LL331: yyval=Cyc_YY44((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);break;case 255: _LL332:
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp701=_cycalloc(sizeof(*
_tmp701));_tmp701[0]=({struct Cyc_Absyn_JoinEff_struct _tmp702;_tmp702.tag=24;
_tmp702.f1=0;_tmp702;});_tmp701;}));break;case 256: _LL333: yyval=Cyc_YY44((void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp704;_tmp704.tag=24;_tmp704.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp704;});_tmp703;}));
break;case 257: _LL334: yyval=Cyc_YY44((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp706;_tmp706.tag=25;_tmp706.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp706;});_tmp705;}));
break;case 258: _LL335: yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp708;_tmp708.tag=24;_tmp708.f1=({struct Cyc_List_List*_tmp709=_cycalloc(
sizeof(*_tmp709));_tmp709->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp709->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp709;});_tmp708;});
_tmp707;}));break;case 259: _LL336: yyval=Cyc_YY40(({struct Cyc_List_List*_tmp70A=
_cycalloc(sizeof(*_tmp70A));_tmp70A->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70A->tl=0;_tmp70A;}));
break;case 260: _LL337: yyval=Cyc_YY40(({struct Cyc_List_List*_tmp70B=_cycalloc(
sizeof(*_tmp70B));_tmp70B->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp70B->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp70B;}));break;case
261: _LL338: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp70C=_cycalloc(
sizeof(*_tmp70C));_tmp70C->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70C;}));break;case 262:
_LL339: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
263: _LL33A: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp70D=_cycalloc(
sizeof(*_tmp70D));_tmp70D->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp70D;}));break;
case 264: _LL33B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 265: _LL33C: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp70E=
_cycalloc(sizeof(*_tmp70E));_tmp70E->tms=({struct Cyc_List_List*_tmp70F=_cycalloc(
sizeof(*_tmp70F));_tmp70F->hd=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp710=
_cycalloc(sizeof(*_tmp710));_tmp710[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp711;_tmp711.tag=0;_tmp711.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp711.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp711;});
_tmp710;});_tmp70F->tl=0;_tmp70F;});_tmp70E;}));break;case 266: _LL33D: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712->tms=({
struct Cyc_List_List*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713->hd=(void*)({
struct Cyc_Absyn_Carray_mod_struct*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714[0]=({
struct Cyc_Absyn_Carray_mod_struct _tmp715;_tmp715.tag=0;_tmp715.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp715.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp715;});
_tmp714;});_tmp713->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp713;});_tmp712;}));break;case 267: _LL33E: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716->tms=({
struct Cyc_List_List*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717->hd=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp718=_cycalloc(sizeof(*_tmp718));
_tmp718[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp719;_tmp719.tag=1;_tmp719.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp719.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp719.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp719;});_tmp718;});_tmp717->tl=0;_tmp717;});
_tmp716;}));break;case 268: _LL33F: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A->tms=({struct Cyc_List_List*_tmp71B=
_cycalloc(sizeof(*_tmp71B));_tmp71B->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp71C=_cycalloc(sizeof(*_tmp71C));_tmp71C[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp71D;_tmp71D.tag=1;_tmp71D.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp71D.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp71D.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp71D;});
_tmp71C;});_tmp71B->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->tms;_tmp71B;});_tmp71A;}));break;case 269: _LL340: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->tms=({
struct Cyc_List_List*_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp721;_tmp721.tag=3;_tmp721.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp722=_cycalloc(sizeof(*_tmp722));
_tmp722[0]=({struct Cyc_Absyn_WithTypes_struct _tmp723;_tmp723.tag=1;_tmp723.f1=0;
_tmp723.f2=0;_tmp723.f3=0;_tmp723.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp723.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp723;});_tmp722;}));
_tmp721;});_tmp720;});_tmp71F->tl=0;_tmp71F;});_tmp71E;}));break;case 270: _LL341: {
struct _tuple24 _tmp725;struct Cyc_List_List*_tmp726;int _tmp727;struct Cyc_Absyn_VarargInfo*
_tmp728;struct Cyc_Core_Opt*_tmp729;struct Cyc_List_List*_tmp72A;struct _tuple24*
_tmp724=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp725=*_tmp724;_tmp726=_tmp725.f1;_tmp727=_tmp725.f2;_tmp728=_tmp725.f3;
_tmp729=_tmp725.f4;_tmp72A=_tmp725.f5;yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp72B=_cycalloc(sizeof(*_tmp72B));_tmp72B->tms=({struct Cyc_List_List*_tmp72C=
_cycalloc(sizeof(*_tmp72C));_tmp72C->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp72D=_cycalloc(sizeof(*_tmp72D));_tmp72D[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp72E;_tmp72E.tag=3;_tmp72E.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp72F=_cycalloc(sizeof(*_tmp72F));_tmp72F[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp730;_tmp730.tag=1;_tmp730.f1=_tmp726;_tmp730.f2=_tmp727;_tmp730.f3=_tmp728;
_tmp730.f4=_tmp729;_tmp730.f5=_tmp72A;_tmp730;});_tmp72F;}));_tmp72E;});_tmp72D;});
_tmp72C->tl=0;_tmp72C;});_tmp72B;}));break;}case 271: _LL342: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731->tms=({
struct Cyc_List_List*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp733=_cycalloc(sizeof(*_tmp733));_tmp733[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp734;_tmp734.tag=3;_tmp734.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp735=_cycalloc(sizeof(*_tmp735));
_tmp735[0]=({struct Cyc_Absyn_WithTypes_struct _tmp736;_tmp736.tag=1;_tmp736.f1=0;
_tmp736.f2=0;_tmp736.f3=0;_tmp736.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp736.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp736;});_tmp735;}));
_tmp734;});_tmp733;});_tmp732->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp732;});_tmp731;}));
break;case 272: _LL343: {struct _tuple24 _tmp738;struct Cyc_List_List*_tmp739;int
_tmp73A;struct Cyc_Absyn_VarargInfo*_tmp73B;struct Cyc_Core_Opt*_tmp73C;struct Cyc_List_List*
_tmp73D;struct _tuple24*_tmp737=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp738=*_tmp737;_tmp739=
_tmp738.f1;_tmp73A=_tmp738.f2;_tmp73B=_tmp738.f3;_tmp73C=_tmp738.f4;_tmp73D=
_tmp738.f5;yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp73E=_cycalloc(
sizeof(*_tmp73E));_tmp73E->tms=({struct Cyc_List_List*_tmp73F=_cycalloc(sizeof(*
_tmp73F));_tmp73F->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*_tmp740=
_cycalloc(sizeof(*_tmp740));_tmp740[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp741;_tmp741.tag=3;_tmp741.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp743;_tmp743.tag=1;_tmp743.f1=_tmp739;_tmp743.f2=_tmp73A;_tmp743.f3=_tmp73B;
_tmp743.f4=_tmp73C;_tmp743.f5=_tmp73D;_tmp743;});_tmp742;}));_tmp741;});_tmp740;});
_tmp73F->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp73F;});_tmp73E;}));break;}case 273: _LL344: {struct
Cyc_List_List*_tmp744=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745->tms=({
struct Cyc_List_List*_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746->hd=(void*)({
struct Cyc_Absyn_TypeParams_mod_struct*_tmp747=_cycalloc(sizeof(*_tmp747));
_tmp747[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp748;_tmp748.tag=4;_tmp748.f1=
_tmp744;_tmp748.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp748.f3=0;
_tmp748;});_tmp747;});_tmp746->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp746;});_tmp745;}));
break;}case 274: _LL345: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp749=_cycalloc(sizeof(*_tmp749));_tmp749->tms=({struct Cyc_List_List*_tmp74A=
_cycalloc(sizeof(*_tmp74A));_tmp74A->hd=(void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp74C;_tmp74C.tag=5;_tmp74C.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp74C.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp74C;});_tmp74B;});
_tmp74A->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp74A;});_tmp749;}));break;case 275: _LL346: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 276: _LL347: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 277: _LL348:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 278:
_LL349: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
279: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 280: _LL34B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 281: _LL34C: yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_ResetRegion_s_struct*
_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D[0]=({struct Cyc_Absyn_ResetRegion_s_struct
_tmp74E;_tmp74E.tag=16;_tmp74E.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp74E;});_tmp74D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 282: _LL34D: yyval=Cyc_YY4(0);break;case 283:
_LL34E: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp74F="open";
_tag_dyneither(_tmp74F,sizeof(char),5);}))!= 0)Cyc_Parse_err(({const char*_tmp750="expecting `open'";
_tag_dyneither(_tmp750,sizeof(char),17);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 284: _LL34F: yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Label_s_struct*_tmp751=_cycalloc(sizeof(*_tmp751));_tmp751[0]=({
struct Cyc_Absyn_Label_s_struct _tmp752;_tmp752.tag=13;_tmp752.f1=({struct
_dyneither_ptr*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp753;});_tmp752.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp752;});
_tmp751;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 285: _LL350: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 286: _LL351: yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 287:
_LL352: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 288:
_LL353: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 289: _LL354: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0)));
break;case 290: _LL355: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 291: _LL356:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 292:
_LL357: yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 293:
_LL358: yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Fn_d_struct*_tmp754=_cycalloc(sizeof(*_tmp754));_tmp754[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp755;_tmp755.tag=1;_tmp755.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp755;});_tmp754;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(
0)));break;case 294: _LL359: yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp757;_tmp757.tag=1;_tmp757.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp757;});_tmp756;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 295: _LL35A: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 296: _LL35B: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 297:
_LL35C: yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 298:
_LL35D: {struct Cyc_Absyn_Exp*_tmp758=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp759=_cycalloc(sizeof(*_tmp759));_tmp759[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp75A;_tmp75A.tag=2;_tmp75A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp75A;});_tmp759;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp758,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 299:
_LL35E: {struct Cyc_Absyn_Exp*_tmp75B=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp75B,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 300:
_LL35F: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 301:
_LL360: yyval=Cyc_YY10(0);break;case 302: _LL361: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C->hd=({struct Cyc_Absyn_Switch_clause*
_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D->pattern=Cyc_Absyn_new_pat((void*)&
Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp75D->pat_vars=
0;_tmp75D->where_clause=0;_tmp75D->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp75D;});
_tmp75C->tl=0;_tmp75C;}));break;case 303: _LL362: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E->hd=({struct Cyc_Absyn_Switch_clause*
_tmp75F=_cycalloc(sizeof(*_tmp75F));_tmp75F->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75F->pat_vars=0;
_tmp75F->where_clause=0;_tmp75F->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp75F->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp75F;});_tmp75E->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75E;}));break;case 304:
_LL363: yyval=Cyc_YY10(({struct Cyc_List_List*_tmp760=_cycalloc(sizeof(*_tmp760));
_tmp760->hd=({struct Cyc_Absyn_Switch_clause*_tmp761=_cycalloc(sizeof(*_tmp761));
_tmp761->pattern=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp761->pat_vars=0;_tmp761->where_clause=0;_tmp761->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp761->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp761;});
_tmp760->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp760;}));break;case 305: _LL364: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762->hd=({struct Cyc_Absyn_Switch_clause*
_tmp763=_cycalloc(sizeof(*_tmp763));_tmp763->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp763->pat_vars=0;
_tmp763->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp763->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp763->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp763;});
_tmp762->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp762;}));break;case 306: _LL365: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764->hd=({struct Cyc_Absyn_Switch_clause*
_tmp765=_cycalloc(sizeof(*_tmp765));_tmp765->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp765->pat_vars=0;
_tmp765->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp765->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp765->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp765;});
_tmp764->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp764;}));break;case 307: _LL366: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 308:
_LL367: yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 309:
_LL368: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 310:
_LL369: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 311:
_LL36A: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 312:
_LL36B: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
313: _LL36C: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 314: _LL36D: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 315:
_LL36E: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 316:
_LL36F: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 317:
_LL370: {struct Cyc_List_List*_tmp766=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp767=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp766,_tmp767));break;}case 318: _LL371: {struct
Cyc_List_List*_tmp768=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp769=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp768,
_tmp769));break;}case 319: _LL372: {struct Cyc_List_List*_tmp76A=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp76B=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp76A,_tmp76B));break;}case 320: _LL373: {struct
Cyc_List_List*_tmp76C=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp76D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp76C,_tmp76D));break;}case 321: _LL374: yyval=Cyc_YY9(
Cyc_Absyn_goto_stmt(({struct _dyneither_ptr*_tmp76E=_cycalloc(sizeof(*_tmp76E));
_tmp76E[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp76E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
322: _LL375: yyval=Cyc_YY9(Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
323: _LL376: yyval=Cyc_YY9(Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
324: _LL377: yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
325: _LL378: yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
326: _LL379: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
327: _LL37A: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)));break;case
328: _LL37B: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)));break;case
329: _LL37C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 330: _LL37D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 331: _LL37E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 332: _LL37F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 333: _LL380: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 334:
_LL381: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
335: _LL382: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 336:
_LL383: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
337: _LL384: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 338: _LL385: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 339: _LL386: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 340:
_LL387: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
341: _LL388: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 342: _LL389: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 343: _LL38A: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 344:
_LL38B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 345:
_LL38C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
346: _LL38D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 347:
_LL38E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 348:
_LL38F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 349:
_LL390: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 350:
_LL391: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
351: _LL392: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 352: _LL393: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 353:
_LL394: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
354: _LL395: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 355: _LL396: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 356:
_LL397: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
357: _LL398: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 358: _LL399: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 359:
_LL39A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 360:
_LL39B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
361: _LL39C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 362: _LL39D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 363: _LL39E: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 364: _LL39F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 365: _LL3A0: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 366:
_LL3A1: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 367:
_LL3A2: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp76F=_cycalloc(sizeof(*_tmp76F));_tmp76F[0]=({
struct Cyc_Absyn_StructField_struct _tmp770;_tmp770.tag=0;_tmp770.f1=({struct
_dyneither_ptr*_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp771;});_tmp770;});
_tmp76F;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 368: _LL3A3: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp772=
_cycalloc_atomic(sizeof(*_tmp772));_tmp772[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp773;_tmp773.tag=1;_tmp773.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp773;});_tmp772;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 369: _LL3A4: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 370: _LL3A5: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 371: _LL3A6:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 372:
_LL3A7: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 373: _LL3A8: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp774=e->r;union Cyc_Absyn_Cnst _tmp776;struct _tuple4
_tmp777;enum Cyc_Absyn_Sign _tmp778;char _tmp779;union Cyc_Absyn_Cnst _tmp77B;struct
_tuple5 _tmp77C;enum Cyc_Absyn_Sign _tmp77D;short _tmp77E;union Cyc_Absyn_Cnst
_tmp780;struct _tuple6 _tmp781;enum Cyc_Absyn_Sign _tmp782;int _tmp783;union Cyc_Absyn_Cnst
_tmp785;struct _dyneither_ptr _tmp786;union Cyc_Absyn_Cnst _tmp788;int _tmp789;union
Cyc_Absyn_Cnst _tmp78B;struct _dyneither_ptr _tmp78C;union Cyc_Absyn_Cnst _tmp78E;
struct _tuple7 _tmp78F;_LL3AB: {struct Cyc_Absyn_Const_e_struct*_tmp775=(struct Cyc_Absyn_Const_e_struct*)
_tmp774;if(_tmp775->tag != 0)goto _LL3AD;else{_tmp776=_tmp775->f1;if((_tmp776.Char_c).tag
!= 2)goto _LL3AD;_tmp777=(struct _tuple4)(_tmp776.Char_c).val;_tmp778=_tmp777.f1;
_tmp779=_tmp777.f2;}}_LL3AC: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp790=_cycalloc_atomic(sizeof(*_tmp790));_tmp790[0]=({struct Cyc_Absyn_Char_p_struct
_tmp791;_tmp791.tag=10;_tmp791.f1=_tmp779;_tmp791;});_tmp790;}),e->loc));goto
_LL3AA;_LL3AD: {struct Cyc_Absyn_Const_e_struct*_tmp77A=(struct Cyc_Absyn_Const_e_struct*)
_tmp774;if(_tmp77A->tag != 0)goto _LL3AF;else{_tmp77B=_tmp77A->f1;if((_tmp77B.Short_c).tag
!= 3)goto _LL3AF;_tmp77C=(struct _tuple5)(_tmp77B.Short_c).val;_tmp77D=_tmp77C.f1;
_tmp77E=_tmp77C.f2;}}_LL3AE: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp792=_cycalloc_atomic(sizeof(*_tmp792));_tmp792[0]=({struct Cyc_Absyn_Int_p_struct
_tmp793;_tmp793.tag=9;_tmp793.f1=_tmp77D;_tmp793.f2=(int)_tmp77E;_tmp793;});
_tmp792;}),e->loc));goto _LL3AA;_LL3AF: {struct Cyc_Absyn_Const_e_struct*_tmp77F=(
struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp77F->tag != 0)goto _LL3B1;else{
_tmp780=_tmp77F->f1;if((_tmp780.Int_c).tag != 4)goto _LL3B1;_tmp781=(struct _tuple6)(
_tmp780.Int_c).val;_tmp782=_tmp781.f1;_tmp783=_tmp781.f2;}}_LL3B0: yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp794=_cycalloc_atomic(
sizeof(*_tmp794));_tmp794[0]=({struct Cyc_Absyn_Int_p_struct _tmp795;_tmp795.tag=9;
_tmp795.f1=_tmp782;_tmp795.f2=_tmp783;_tmp795;});_tmp794;}),e->loc));goto _LL3AA;
_LL3B1: {struct Cyc_Absyn_Const_e_struct*_tmp784=(struct Cyc_Absyn_Const_e_struct*)
_tmp774;if(_tmp784->tag != 0)goto _LL3B3;else{_tmp785=_tmp784->f1;if((_tmp785.Float_c).tag
!= 6)goto _LL3B3;_tmp786=(struct _dyneither_ptr)(_tmp785.Float_c).val;}}_LL3B2:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp796=
_cycalloc(sizeof(*_tmp796));_tmp796[0]=({struct Cyc_Absyn_Float_p_struct _tmp797;
_tmp797.tag=11;_tmp797.f1=_tmp786;_tmp797;});_tmp796;}),e->loc));goto _LL3AA;
_LL3B3: {struct Cyc_Absyn_Const_e_struct*_tmp787=(struct Cyc_Absyn_Const_e_struct*)
_tmp774;if(_tmp787->tag != 0)goto _LL3B5;else{_tmp788=_tmp787->f1;if((_tmp788.Null_c).tag
!= 1)goto _LL3B5;_tmp789=(int)(_tmp788.Null_c).val;}}_LL3B4: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3AA;_LL3B5: {struct Cyc_Absyn_Const_e_struct*
_tmp78A=(struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp78A->tag != 0)goto _LL3B7;
else{_tmp78B=_tmp78A->f1;if((_tmp78B.String_c).tag != 7)goto _LL3B7;_tmp78C=(
struct _dyneither_ptr)(_tmp78B.String_c).val;}}_LL3B6: Cyc_Parse_err(({const char*
_tmp798="strings cannot occur within patterns";_tag_dyneither(_tmp798,sizeof(
char),37);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));goto _LL3AA;_LL3B7: {struct Cyc_Absyn_Const_e_struct*
_tmp78D=(struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp78D->tag != 0)goto _LL3B9;
else{_tmp78E=_tmp78D->f1;if((_tmp78E.LongLong_c).tag != 5)goto _LL3B9;_tmp78F=(
struct _tuple7)(_tmp78E.LongLong_c).val;}}_LL3B8: Cyc_Parse_unimp(({const char*
_tmp799="long long's in patterns";_tag_dyneither(_tmp799,sizeof(char),24);}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL3AA;
_LL3B9:;_LL3BA: Cyc_Parse_err(({const char*_tmp79A="bad constant in case";
_tag_dyneither(_tmp79A,sizeof(char),21);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL3AA:;}break;}
case 374: _LL3A9: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp79B=_cycalloc(sizeof(*_tmp79B));_tmp79B[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp79C;_tmp79C.tag=14;_tmp79C.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp79C;});_tmp79B;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 375:
_LL3BB: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp79D="as";
_tag_dyneither(_tmp79D,sizeof(char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp79E="expecting `as'";
_tag_dyneither(_tmp79E,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp79F=_cycalloc(
sizeof(*_tmp79F));_tmp79F[0]=({struct Cyc_Absyn_Var_p_struct _tmp7A0;_tmp7A0.tag=1;
_tmp7A0.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7A1=_cycalloc(sizeof(*
_tmp7A1));_tmp7A1->f1=Cyc_Absyn_Loc_n;_tmp7A1->f2=({struct _dyneither_ptr*_tmp7A2=
_cycalloc(sizeof(*_tmp7A2));_tmp7A2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7A2;});_tmp7A1;}),(
void*)& Cyc_Absyn_VoidType_val,0);_tmp7A0.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7A0;});_tmp79F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)));break;case
376: _LL3BC: {struct Cyc_List_List*_tmp7A4;int _tmp7A5;struct _tuple20 _tmp7A3=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A4=_tmp7A3.f1;
_tmp7A5=_tmp7A3.f2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*
_tmp7A6=_cycalloc(sizeof(*_tmp7A6));_tmp7A6[0]=({struct Cyc_Absyn_Tuple_p_struct
_tmp7A7;_tmp7A7.tag=4;_tmp7A7.f1=_tmp7A4;_tmp7A7.f2=_tmp7A5;_tmp7A7;});_tmp7A6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 377: _LL3BD: {struct Cyc_List_List*_tmp7A9;
int _tmp7AA;struct _tuple20 _tmp7A8=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A9=_tmp7A8.f1;
_tmp7AA=_tmp7A8.f2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*
_tmp7AB=_cycalloc(sizeof(*_tmp7AB));_tmp7AB[0]=({struct Cyc_Absyn_UnknownCall_p_struct
_tmp7AC;_tmp7AC.tag=15;_tmp7AC.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7AC.f2=_tmp7A9;
_tmp7AC.f3=_tmp7AA;_tmp7AC;});_tmp7AB;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 378:
_LL3BE: {struct Cyc_List_List*_tmp7AE;int _tmp7AF;struct _tuple20 _tmp7AD=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AE=_tmp7AD.f1;
_tmp7AF=_tmp7AD.f2;{struct Cyc_List_List*_tmp7B0=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));
_tmp7B1[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp7B2;_tmp7B2.tag=6;_tmp7B2.f1=({
struct Cyc_Absyn_AggrInfo*_tmp7B3=_cycalloc(sizeof(*_tmp7B3));_tmp7B3->aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0);_tmp7B3->targs=0;
_tmp7B3;});_tmp7B2.f2=_tmp7B0;_tmp7B2.f3=_tmp7AE;_tmp7B2.f4=_tmp7AF;_tmp7B2;});
_tmp7B1;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;};}case 379: _LL3BF: {struct Cyc_List_List*
_tmp7B5;int _tmp7B6;struct _tuple20 _tmp7B4=*Cyc_yyget_YY16(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B5=_tmp7B4.f1;
_tmp7B6=_tmp7B4.f2;{struct Cyc_List_List*_tmp7B7=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));
_tmp7B8[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp7B9;_tmp7B9.tag=6;_tmp7B9.f1=0;
_tmp7B9.f2=_tmp7B7;_tmp7B9.f3=_tmp7B5;_tmp7B9.f4=_tmp7B6;_tmp7B9;});_tmp7B8;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;};}case 380: _LL3C0: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7BA=_cycalloc(sizeof(*_tmp7BA));
_tmp7BA[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7BB;_tmp7BB.tag=5;_tmp7BB.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7BB;});
_tmp7BA;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 381: _LL3C1: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7BC=_cycalloc(sizeof(*_tmp7BC));
_tmp7BC[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7BD;_tmp7BD.tag=5;_tmp7BD.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7BE=_cycalloc(
sizeof(*_tmp7BE));_tmp7BE[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7BF;_tmp7BF.tag=
5;_tmp7BF.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7BF;});_tmp7BE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7BD;});
_tmp7BC;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 382: _LL3C2: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7C0=_cycalloc(sizeof(*_tmp7C0));
_tmp7C0[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7C1;_tmp7C1.tag=2;_tmp7C1.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7C2=_cycalloc(sizeof(*_tmp7C2));
_tmp7C2->f1=Cyc_Absyn_Loc_n;_tmp7C2->f2=({struct _dyneither_ptr*_tmp7C3=_cycalloc(
sizeof(*_tmp7C3));_tmp7C3[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7C3;});_tmp7C2;}),(void*)&
Cyc_Absyn_VoidType_val,0);_tmp7C1.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7C1;});_tmp7C0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 383:
_LL3C3: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7C4="as";
_tag_dyneither(_tmp7C4,sizeof(char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp7C5="expecting `as'";
_tag_dyneither(_tmp7C5,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7C6=_cycalloc(
sizeof(*_tmp7C6));_tmp7C6[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7C7;_tmp7C7.tag=
2;_tmp7C7.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7C8=_cycalloc(sizeof(*
_tmp7C8));_tmp7C8->f1=Cyc_Absyn_Loc_n;_tmp7C8->f2=({struct _dyneither_ptr*_tmp7C9=
_cycalloc(sizeof(*_tmp7C9));_tmp7C9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7C9;});_tmp7C8;}),(
void*)& Cyc_Absyn_VoidType_val,0);_tmp7C7.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7C7;});_tmp7C6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)));break;case
384: _LL3C4: {void*_tmp7CA=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_IntKind));yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp7CB=_cycalloc(sizeof(*_tmp7CB));_tmp7CB[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp7CC;_tmp7CC.tag=3;_tmp7CC.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7CA);
_tmp7CC.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7CD=_cycalloc(sizeof(*
_tmp7CD));_tmp7CD->f1=Cyc_Absyn_Loc_n;_tmp7CD->f2=({struct _dyneither_ptr*_tmp7CE=
_cycalloc(sizeof(*_tmp7CE));_tmp7CE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7CE;});_tmp7CD;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp7CF=_cycalloc(sizeof(*_tmp7CF));
_tmp7CF[0]=({struct Cyc_Absyn_TagType_struct _tmp7D0;_tmp7D0.tag=20;_tmp7D0.f1=(
void*)_tmp7CA;_tmp7D0;});_tmp7CF;}),0);_tmp7CC;});_tmp7CB;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 385:
_LL3C5: {struct Cyc_Absyn_Tvar*_tmp7D1=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_IntKind));yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp7D3;_tmp7D3.tag=3;_tmp7D3.f1=_tmp7D1;_tmp7D3.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4->f1=Cyc_Absyn_Loc_n;
_tmp7D4->f2=({struct _dyneither_ptr*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7D5;});_tmp7D4;}),(void*)({struct Cyc_Absyn_TagType_struct*_tmp7D6=_cycalloc(
sizeof(*_tmp7D6));_tmp7D6[0]=({struct Cyc_Absyn_TagType_struct _tmp7D7;_tmp7D7.tag=
20;_tmp7D7.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7D8=_cycalloc(
sizeof(*_tmp7D8));_tmp7D8[0]=({struct Cyc_Absyn_VarType_struct _tmp7D9;_tmp7D9.tag=
2;_tmp7D9.f1=_tmp7D1;_tmp7D9;});_tmp7D8;}));_tmp7D7;});_tmp7D6;}),0);_tmp7D3;});
_tmp7D2;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 386: _LL3C6: yyval=Cyc_YY12(({struct
_tuple20*_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp7DA->f2=0;_tmp7DA;}));
break;case 387: _LL3C7: yyval=Cyc_YY12(({struct _tuple20*_tmp7DB=_cycalloc(sizeof(*
_tmp7DB));_tmp7DB->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp7DB->f2=1;_tmp7DB;}));break;case 388: _LL3C8: yyval=Cyc_YY12(({struct _tuple20*
_tmp7DC=_cycalloc(sizeof(*_tmp7DC));_tmp7DC->f1=0;_tmp7DC->f2=1;_tmp7DC;}));
break;case 389: _LL3C9: yyval=Cyc_YY13(({struct Cyc_List_List*_tmp7DD=_cycalloc(
sizeof(*_tmp7DD));_tmp7DD->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7DD->tl=0;_tmp7DD;}));break;case 390: _LL3CA: yyval=Cyc_YY13(({
struct Cyc_List_List*_tmp7DE=_cycalloc(sizeof(*_tmp7DE));_tmp7DE->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7DE->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7DE;}));break;
case 391: _LL3CB: yyval=Cyc_YY14(({struct _tuple21*_tmp7DF=_cycalloc(sizeof(*_tmp7DF));
_tmp7DF->f1=0;_tmp7DF->f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7DF;}));break;case 392: _LL3CC: yyval=Cyc_YY14(({struct
_tuple21*_tmp7E0=_cycalloc(sizeof(*_tmp7E0));_tmp7E0->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7E0->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E0;}));break;case
393: _LL3CD: yyval=Cyc_YY16(({struct _tuple20*_tmp7E1=_cycalloc(sizeof(*_tmp7E1));
_tmp7E1->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp7E1->f2=0;_tmp7E1;}));
break;case 394: _LL3CE: yyval=Cyc_YY16(({struct _tuple20*_tmp7E2=_cycalloc(sizeof(*
_tmp7E2));_tmp7E2->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp7E2->f2=1;_tmp7E2;}));break;case 395: _LL3CF: yyval=Cyc_YY16(({struct _tuple20*
_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3->f1=0;_tmp7E3->f2=1;_tmp7E3;}));
break;case 396: _LL3D0: yyval=Cyc_YY15(({struct Cyc_List_List*_tmp7E4=_cycalloc(
sizeof(*_tmp7E4));_tmp7E4->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7E4->tl=0;_tmp7E4;}));break;case 397: _LL3D1: yyval=Cyc_YY15(({
struct Cyc_List_List*_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E5->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7E5;}));break;
case 398: _LL3D2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 399: _LL3D3: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 400:
_LL3D4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
401: _LL3D5: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 402:
_LL3D6: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Swap_e_struct*
_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6[0]=({struct Cyc_Absyn_Swap_e_struct
_tmp7E7;_tmp7E7.tag=36;_tmp7E7.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7E7.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E7;});_tmp7E6;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 403: _LL3D7: yyval=Cyc_YY8(0);break;case 404:
_LL3D8: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7E8=_cycalloc_atomic(sizeof(*
_tmp7E8));_tmp7E8->v=(void*)Cyc_Absyn_Times;_tmp7E8;}));break;case 405: _LL3D9:
yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7E9=_cycalloc_atomic(sizeof(*_tmp7E9));
_tmp7E9->v=(void*)Cyc_Absyn_Div;_tmp7E9;}));break;case 406: _LL3DA: yyval=Cyc_YY8(({
struct Cyc_Core_Opt*_tmp7EA=_cycalloc_atomic(sizeof(*_tmp7EA));_tmp7EA->v=(void*)
Cyc_Absyn_Mod;_tmp7EA;}));break;case 407: _LL3DB: yyval=Cyc_YY8(({struct Cyc_Core_Opt*
_tmp7EB=_cycalloc_atomic(sizeof(*_tmp7EB));_tmp7EB->v=(void*)Cyc_Absyn_Plus;
_tmp7EB;}));break;case 408: _LL3DC: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7EC=
_cycalloc_atomic(sizeof(*_tmp7EC));_tmp7EC->v=(void*)Cyc_Absyn_Minus;_tmp7EC;}));
break;case 409: _LL3DD: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7ED=_cycalloc_atomic(
sizeof(*_tmp7ED));_tmp7ED->v=(void*)Cyc_Absyn_Bitlshift;_tmp7ED;}));break;case
410: _LL3DE: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7EE=_cycalloc_atomic(sizeof(*
_tmp7EE));_tmp7EE->v=(void*)Cyc_Absyn_Bitlrshift;_tmp7EE;}));break;case 411:
_LL3DF: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7EF=_cycalloc_atomic(sizeof(*
_tmp7EF));_tmp7EF->v=(void*)Cyc_Absyn_Bitand;_tmp7EF;}));break;case 412: _LL3E0:
yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7F0=_cycalloc_atomic(sizeof(*_tmp7F0));
_tmp7F0->v=(void*)Cyc_Absyn_Bitxor;_tmp7F0;}));break;case 413: _LL3E1: yyval=Cyc_YY8(({
struct Cyc_Core_Opt*_tmp7F1=_cycalloc_atomic(sizeof(*_tmp7F1));_tmp7F1->v=(void*)
Cyc_Absyn_Bitor;_tmp7F1;}));break;case 414: _LL3E2: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 415: _LL3E3: yyval=
Cyc_YY3(Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 416:
_LL3E4: yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 417:
_LL3E5: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 418:
_LL3E6: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 419:
_LL3E7: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 420:
_LL3E8: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 421:
_LL3E9: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
422: _LL3EA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 423: _LL3EB: yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 424:
_LL3EC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
425: _LL3ED: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 426:
_LL3EE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
427: _LL3EF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 428:
_LL3F0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
429: _LL3F1: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 430:
_LL3F2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
431: _LL3F3: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 432:
_LL3F4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
433: _LL3F5: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 434:
_LL3F6: yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 435:
_LL3F7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
436: _LL3F8: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 437:
_LL3F9: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 438:
_LL3FA: yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 439:
_LL3FB: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 440:
_LL3FC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
441: _LL3FD: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 442:
_LL3FE: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 443:
_LL3FF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
444: _LL400: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 445:
_LL401: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 446:
_LL402: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
447: _LL403: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 448:
_LL404: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 449:
_LL405: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 450:
_LL406: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
451: _LL407: yyval=Cyc_YY3(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 452: _LL408: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 453: _LL409: yyval=
Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 454:
_LL40A: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 455:
_LL40B: yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 456:
_LL40C: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 457:
_LL40D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
458: _LL40E: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 459:
_LL40F: yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 460:
_LL410: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 461:
_LL411: yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp7F2=_cycalloc(sizeof(*_tmp7F2));_tmp7F2[0]=({struct Cyc_Absyn_StructField_struct
_tmp7F3;_tmp7F3.tag=0;_tmp7F3.f1=({struct _dyneither_ptr*_tmp7F4=_cycalloc(
sizeof(*_tmp7F4));_tmp7F4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7F4;});_tmp7F3;});
_tmp7F2;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 462: _LL412: yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7F5=_cycalloc_atomic(sizeof(*
_tmp7F5));_tmp7F5[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp7F6;_tmp7F6.tag=1;
_tmp7F6.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp7F6;});_tmp7F5;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 463:
_LL413: {struct Cyc_Position_Segment*_tmp7F7=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp7F8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp7F7,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=Cyc_YY3(Cyc_Absyn_gentyp_exp(_tmp7F8,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 464:
_LL414: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp7FA;_tmp7FA.tag=35;_tmp7FA.f1=({struct Cyc_Absyn_MallocInfo _tmp7FB;_tmp7FB.is_calloc=
0;_tmp7FB.rgn=0;_tmp7FB.elt_type=0;_tmp7FB.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7FB.fat_result=0;
_tmp7FB;});_tmp7FA;});_tmp7F9;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 465:
_LL415: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp7FC=_cycalloc(sizeof(*_tmp7FC));_tmp7FC[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp7FD;_tmp7FD.tag=35;_tmp7FD.f1=({struct Cyc_Absyn_MallocInfo _tmp7FE;_tmp7FE.is_calloc=
0;_tmp7FE.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7FE.elt_type=0;
_tmp7FE.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp7FE.fat_result=0;_tmp7FE;});_tmp7FD;});_tmp7FC;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 466:
_LL416: {void*_tmp800;struct _tuple8 _tmp7FF=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp800=_tmp7FF.f3;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp801=
_cycalloc(sizeof(*_tmp801));_tmp801[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp802;
_tmp802.tag=35;_tmp802.f1=({struct Cyc_Absyn_MallocInfo _tmp803;_tmp803.is_calloc=
1;_tmp803.rgn=0;_tmp803.elt_type=({void**_tmp804=_cycalloc(sizeof(*_tmp804));
_tmp804[0]=_tmp800;_tmp804;});_tmp803.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp803.fat_result=0;
_tmp803;});_tmp802;});_tmp801;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 467:
_LL417: {void*_tmp806;struct _tuple8 _tmp805=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp806=_tmp805.f3;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp807=
_cycalloc(sizeof(*_tmp807));_tmp807[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp808;
_tmp808.tag=35;_tmp808.f1=({struct Cyc_Absyn_MallocInfo _tmp809;_tmp809.is_calloc=
1;_tmp809.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp809.elt_type=({void**
_tmp80A=_cycalloc(sizeof(*_tmp80A));_tmp80A[0]=_tmp806;_tmp80A;});_tmp809.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp809.fat_result=0;_tmp809;});_tmp808;});_tmp807;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 468:
_LL418: yyval=Cyc_YY3(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,({struct Cyc_Absyn_Exp*
_tmp80B[1];_tmp80B[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp80B,sizeof(struct Cyc_Absyn_Exp*),1));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 469:
_LL419: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_struct*
_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C[0]=({struct Cyc_Absyn_Tagcheck_e_struct
_tmp80D;_tmp80D.tag=39;_tmp80D.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp80D.f2=({struct
_dyneither_ptr*_tmp80E=_cycalloc(sizeof(*_tmp80E));_tmp80E[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp80E;});_tmp80D;});
_tmp80C;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 470: _LL41A: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Tagcheck_e_struct*_tmp80F=_cycalloc(sizeof(*_tmp80F));
_tmp80F[0]=({struct Cyc_Absyn_Tagcheck_e_struct _tmp810;_tmp810.tag=39;_tmp810.f1=
Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp810.f2=({
struct _dyneither_ptr*_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp811;});_tmp810;});
_tmp80F;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 471: _LL41B: {void*_tmp813;struct _tuple8
_tmp812=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp813=_tmp812.f3;yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp813,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 472:
_LL41C: yyval=Cyc_YY7(Cyc_Absyn_Bitnot);break;case 473: _LL41D: yyval=Cyc_YY7(Cyc_Absyn_Not);
break;case 474: _LL41E: yyval=Cyc_YY7(Cyc_Absyn_Minus);break;case 475: _LL41F: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 476: _LL420:
yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 477:
_LL421: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 478:
_LL422: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 479:
_LL423: yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _dyneither_ptr*
_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp814;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 480:
_LL424: yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _dyneither_ptr*
_tmp815=_cycalloc(sizeof(*_tmp815));_tmp815[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp815;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 481:
_LL425: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 482:
_LL426: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 483:
_LL427: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp817;_tmp817.tag=27;_tmp817.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp817.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp817;});_tmp816;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 484: _LL428: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp818=_cycalloc(sizeof(*_tmp818));
_tmp818[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp819;_tmp819.tag=27;_tmp819.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp819.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp819;});_tmp818;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 485:
_LL429: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp81A=_cycalloc(sizeof(*_tmp81A));_tmp81A[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp81B;_tmp81B.tag=2;_tmp81B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81B;});_tmp81A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 486:
_LL42A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
487: _LL42B: yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 488:
_LL42C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 489: _LL42D: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 490:
_LL42E: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 491:
_LL42F: yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 492:
_LL430: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_struct*
_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C[0]=({struct Cyc_Absyn_Aggregate_e_struct
_tmp81D;_tmp81D.tag=30;_tmp81D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp81D.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp81D.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp81D.f4=0;_tmp81D;});
_tmp81C;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 493: _LL431: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 494:
_LL432: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 495: _LL433: yyval=Cyc_YY5(({struct Cyc_List_List*_tmp81E=_cycalloc(sizeof(*
_tmp81E));_tmp81E->hd=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp81E->tl=0;_tmp81E;}));break;case 496: _LL434: yyval=Cyc_YY5(({
struct Cyc_List_List*_tmp81F=_cycalloc(sizeof(*_tmp81F));_tmp81F->hd=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81F->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp81F;}));break;
case 497: _LL435: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 498:
_LL436: yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 499:
_LL437: yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 500:
_LL438: yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 501:
_LL439: yyval=Cyc_QualId_tok(({struct _tuple1*_tmp820=_cycalloc(sizeof(*_tmp820));
_tmp820->f1=Cyc_Absyn_Rel_n(0);_tmp820->f2=({struct _dyneither_ptr*_tmp821=
_cycalloc(sizeof(*_tmp821));_tmp821[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp821;});_tmp820;}));
break;case 502: _LL43A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 503: _LL43B: yyval=Cyc_QualId_tok(({struct _tuple1*_tmp822=
_cycalloc(sizeof(*_tmp822));_tmp822->f1=Cyc_Absyn_Rel_n(0);_tmp822->f2=({struct
_dyneither_ptr*_tmp823=_cycalloc(sizeof(*_tmp823));_tmp823[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp823;});_tmp822;}));
break;case 504: _LL43C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 505: _LL43D: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 506: _LL43E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 507: _LL43F: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 508: _LL440:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 509:
_LL441: break;case 510: _LL442: yylex_buf->lex_curr_pos -=1;break;default: _LL443:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;++ yylsp_offset;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(511,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(143,yyn - 144)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6371) && Cyc_yycheck[_check_known_subscript_notnull(6372,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6372,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(143,yyn - 144)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1032,yystate)];if(yyn > - 32768  && yyn < 6371){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 287 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6372,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(287,x)])+ 15,count ++);}msg=({unsigned int _tmp824=(
unsigned int)(sze + 15);char*_tmp825=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp824 + 1));struct _dyneither_ptr _tmp827=_tag_dyneither(_tmp825,
sizeof(char),_tmp824 + 1);{unsigned int _tmp826=_tmp824;unsigned int i;for(i=0;i < 
_tmp826;i ++){_tmp825[i]='\000';}_tmp825[_tmp826]=(char)0;}_tmp827;});Cyc_strcpy(
msg,({const char*_tmp828="parse error";_tag_dyneither(_tmp828,sizeof(char),12);}));
if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 287 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6372,x + yyn)]== x){Cyc_strcat(msg,(struct
_dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp829=", expecting `";
_tag_dyneither(_tmp829,sizeof(char),14);}):(struct _dyneither_ptr)({const char*
_tmp82A=" or `";_tag_dyneither(_tmp82A,sizeof(char),6);})));Cyc_strcat(msg,(
struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(287,x)]);Cyc_strcat(
msg,({const char*_tmp82B="'";_tag_dyneither(_tmp82B,sizeof(char),2);}));++ count;}}}
Cyc_yyerror((struct _dyneither_ptr)msg);}else{Cyc_yyerror(({const char*_tmp82C="parse error";
_tag_dyneither(_tmp82C,sizeof(char),12);}));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp82D=1;_npop_handler(0);return _tmp82D;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp82E=1;_npop_handler(0);return _tmp82E;}-- yyvsp_offset;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];--
yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,
yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6371) || 
Cyc_yycheck[_check_known_subscript_notnull(6372,yyn)]!= 1)goto yyerrdefault;yyn=(
int)Cyc_yytable[_check_known_subscript_notnull(6372,yyn)];if(yyn < 0){if(yyn == -
32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 
1031){int _tmp82F=0;_npop_handler(0);return _tmp82F;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE
_tmp839=v;struct _tuple6 _tmp83A;int _tmp83B;char _tmp83C;struct _dyneither_ptr
_tmp83D;struct _tuple1*_tmp83E;struct _tuple1 _tmp83F;union Cyc_Absyn_Nmspace _tmp840;
struct _dyneither_ptr*_tmp841;_LL446: if((_tmp839.Int_tok).tag != 1)goto _LL448;
_tmp83A=(struct _tuple6)(_tmp839.Int_tok).val;_tmp83B=_tmp83A.f2;_LL447:({struct
Cyc_Int_pa_struct _tmp844;_tmp844.tag=1;_tmp844.f1=(unsigned long)_tmp83B;({void*
_tmp842[1]={& _tmp844};Cyc_fprintf(Cyc_stderr,({const char*_tmp843="%d";
_tag_dyneither(_tmp843,sizeof(char),3);}),_tag_dyneither(_tmp842,sizeof(void*),1));});});
goto _LL445;_LL448: if((_tmp839.Char_tok).tag != 2)goto _LL44A;_tmp83C=(char)(
_tmp839.Char_tok).val;_LL449:({struct Cyc_Int_pa_struct _tmp847;_tmp847.tag=1;
_tmp847.f1=(unsigned long)((int)_tmp83C);({void*_tmp845[1]={& _tmp847};Cyc_fprintf(
Cyc_stderr,({const char*_tmp846="%c";_tag_dyneither(_tmp846,sizeof(char),3);}),
_tag_dyneither(_tmp845,sizeof(void*),1));});});goto _LL445;_LL44A: if((_tmp839.String_tok).tag
!= 3)goto _LL44C;_tmp83D=(struct _dyneither_ptr)(_tmp839.String_tok).val;_LL44B:({
struct Cyc_String_pa_struct _tmp84A;_tmp84A.tag=0;_tmp84A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp83D);({void*_tmp848[1]={& _tmp84A};Cyc_fprintf(Cyc_stderr,({
const char*_tmp849="\"%s\"";_tag_dyneither(_tmp849,sizeof(char),5);}),
_tag_dyneither(_tmp848,sizeof(void*),1));});});goto _LL445;_LL44C: if((_tmp839.QualId_tok).tag
!= 5)goto _LL44E;_tmp83E=(struct _tuple1*)(_tmp839.QualId_tok).val;_tmp83F=*
_tmp83E;_tmp840=_tmp83F.f1;_tmp841=_tmp83F.f2;_LL44D: {struct Cyc_List_List*
_tmp84B=0;{union Cyc_Absyn_Nmspace _tmp84C=_tmp840;struct Cyc_List_List*_tmp84D;
struct Cyc_List_List*_tmp84E;int _tmp84F;_LL451: if((_tmp84C.Rel_n).tag != 1)goto
_LL453;_tmp84D=(struct Cyc_List_List*)(_tmp84C.Rel_n).val;_LL452: _tmp84B=_tmp84D;
goto _LL450;_LL453: if((_tmp84C.Abs_n).tag != 2)goto _LL455;_tmp84E=(struct Cyc_List_List*)(
_tmp84C.Abs_n).val;_LL454: _tmp84B=_tmp84E;goto _LL450;_LL455: if((_tmp84C.Loc_n).tag
!= 3)goto _LL450;_tmp84F=(int)(_tmp84C.Loc_n).val;_LL456: goto _LL450;_LL450:;}for(
0;_tmp84B != 0;_tmp84B=_tmp84B->tl){({struct Cyc_String_pa_struct _tmp852;_tmp852.tag=
0;_tmp852.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_tmp84B->hd));({void*_tmp850[1]={& _tmp852};Cyc_fprintf(Cyc_stderr,({
const char*_tmp851="%s::";_tag_dyneither(_tmp851,sizeof(char),5);}),
_tag_dyneither(_tmp850,sizeof(void*),1));});});}({struct Cyc_String_pa_struct
_tmp855;_tmp855.tag=0;_tmp855.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp841);({void*_tmp853[1]={& _tmp855};Cyc_fprintf(Cyc_stderr,({const char*_tmp854="%s::";
_tag_dyneither(_tmp854,sizeof(char),5);}),_tag_dyneither(_tmp853,sizeof(void*),1));});});
goto _LL445;}_LL44E:;_LL44F:({void*_tmp856=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp857="?";_tag_dyneither(_tmp857,sizeof(char),2);}),_tag_dyneither(_tmp856,
sizeof(void*),0));});goto _LL445;_LL445:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
