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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(
struct Cyc_List_List*x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple1{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static
 = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC
 = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;
int q_restrict;int real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{
Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz
 = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind
 = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind
 = 5,Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
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
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct
_tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(
struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU
aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;
struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
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
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
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
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union
Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus
 = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt
 = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,
Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor
 = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 
18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc
 = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{
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
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
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
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*defn;
struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*
Cyc_Absyn_new_conref(void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union
Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_dyneither_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}
__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*,struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);int Cyc_Tcutil_kind_leq(enum Cyc_Absyn_Kind k1,enum Cyc_Absyn_Kind
k2);enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,enum Cyc_Absyn_Kind
def);enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*,void*);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple10{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple11*Cyc_Tcutil_r_make_inst_var(struct _tuple10*,struct Cyc_Absyn_Tvar*);void
Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,enum Cyc_Absyn_Kind k,int allow_evars,void*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct
_tuple12{int f1;void*f2;};struct _tuple12 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_rgn_of_pointer(void*
t,void**rgn);struct _tuple13{unsigned int f1;int f2;};struct _tuple13 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*);struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct Cyc_List_List*
path;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple14{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple14 val;};struct _union_RelnOp_LessNumelts{
int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_LessConst{int tag;
unsigned int val;};struct _union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*
val;};union Cyc_CfFlowInfo_RelnOp{struct _union_RelnOp_EqualConst EqualConst;struct
_union_RelnOp_LessVar LessVar;struct _union_RelnOp_LessNumelts LessNumelts;struct
_union_RelnOp_LessConst LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;
};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop cmp;void*bd;};extern char
Cyc_CfFlowInfo_HasTagCmps[11];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;
struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{int
tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct _union_FlowInfo_BottomFL{int tag;
int val;};struct _tuple15{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple15 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*
notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;
void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct
Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){({void*_tmp0=0;
Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,
ap),_tag_dyneither(_tmp0,sizeof(void*),0));});if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle*_tmp4=Cyc_Tcenv_get_fnrgn(te);void*_tmp5=
Cyc_Tcenv_lookup_ordinary(_tmp4,te,e->loc,q);void*_tmp7;struct Cyc_Absyn_Enumdecl*
_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;void*_tmpC;struct Cyc_Absyn_Enumfield*_tmpD;
struct Cyc_Absyn_Datatypedecl*_tmpF;struct Cyc_Absyn_Datatypefield*_tmp10;_LL1: {
struct Cyc_Tcenv_VarRes_struct*_tmp6=(struct Cyc_Tcenv_VarRes_struct*)_tmp5;if(
_tmp6->tag != 0)goto _LL3;else{_tmp7=(void*)_tmp6->f1;}}_LL2: e->r=(void*)({struct
Cyc_Absyn_Var_e_struct*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_Absyn_Var_e_struct
_tmp13;_tmp13.tag=1;_tmp13.f1=q;_tmp13.f2=(void*)_tmp7;_tmp13;});_tmp12;});goto
_LL0;_LL3: {struct Cyc_Tcenv_EnumRes_struct*_tmp8=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp5;if(_tmp8->tag != 3)goto _LL5;else{_tmp9=_tmp8->f1;_tmpA=_tmp8->f2;}}_LL4: e->r=(
void*)({struct Cyc_Absyn_Enum_e_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({
struct Cyc_Absyn_Enum_e_struct _tmp15;_tmp15.tag=32;_tmp15.f1=q;_tmp15.f2=(struct
Cyc_Absyn_Enumdecl*)_tmp9;_tmp15.f3=(struct Cyc_Absyn_Enumfield*)_tmpA;_tmp15;});
_tmp14;});goto _LL0;_LL5: {struct Cyc_Tcenv_AnonEnumRes_struct*_tmpB=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp5;if(_tmpB->tag != 4)goto _LL7;else{_tmpC=(void*)_tmpB->f1;_tmpD=_tmpB->f2;}}
_LL6: e->r=(void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp16=_cycalloc(sizeof(*
_tmp16));_tmp16[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp17;_tmp17.tag=33;
_tmp17.f1=q;_tmp17.f2=(void*)_tmpC;_tmp17.f3=(struct Cyc_Absyn_Enumfield*)_tmpD;
_tmp17;});_tmp16;});goto _LL0;_LL7: {struct Cyc_Tcenv_DatatypeRes_struct*_tmpE=(
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp5;if(_tmpE->tag != 2)goto _LL9;else{_tmpF=
_tmpE->f1;_tmp10=_tmpE->f2;}}_LL8: e->r=(void*)({struct Cyc_Absyn_Datatype_e_struct*
_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Absyn_Datatype_e_struct
_tmp19;_tmp19.tag=31;_tmp19.f1=0;_tmp19.f2=_tmpF;_tmp19.f3=_tmp10;_tmp19;});
_tmp18;});goto _LL0;_LL9: {struct Cyc_Tcenv_AggrRes_struct*_tmp11=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp5;if(_tmp11->tag != 1)goto _LL0;}_LLA:({struct Cyc_String_pa_struct _tmp1C;
_tmp1C.tag=0;_tmp1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q));({void*_tmp1A[1]={& _tmp1C};Cyc_Tcutil_terr(e->loc,({const char*_tmp1B="bad occurrence of type name %s";
_tag_dyneither(_tmp1B,sizeof(char),31);}),_tag_dyneither(_tmp1A,sizeof(void*),1));});});
e->r=(void*)({struct Cyc_Absyn_Var_e_struct*_tmp1D=_cycalloc(sizeof(*_tmp1D));
_tmp1D[0]=({struct Cyc_Absyn_Var_e_struct _tmp1E;_tmp1E.tag=1;_tmp1E.f1=q;_tmp1E.f2=(
void*)((void*)& Cyc_Absyn_Unresolved_b_val);_tmp1E;});_tmp1D;});goto _LL0;_LL0:;};
_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp20=_tmp2;_LLC: {
struct Cyc_Dict_Absent_struct*_tmp21=(struct Cyc_Dict_Absent_struct*)_tmp20;if(
_tmp21->tag != Cyc_Dict_Absent)goto _LLE;}_LLD: e->r=(void*)({struct Cyc_Absyn_Var_e_struct*
_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Absyn_Var_e_struct _tmp23;
_tmp23.tag=1;_tmp23.f1=q;_tmp23.f2=(void*)((void*)& Cyc_Absyn_Unresolved_b_val);
_tmp23;});_tmp22;});goto _LLB;_LLE:;_LLF:(void)_throw(_tmp20);_LLB:;}};}struct
_tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple16*
Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){return({struct _tuple16*_tmp24=
_cycalloc(sizeof(*_tmp24));_tmp24->f1=0;_tmp24->f2=e;_tmp24;});}static void Cyc_Tcexp_resolve_unknown_fn(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
es){void*_tmp25=e1->r;struct _tuple1*_tmp27;_LL11: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp26=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp25;if(_tmp26->tag != 2)goto _LL13;
else{_tmp27=_tmp26->f1;}}_LL12: {struct _handler_cons _tmp28;_push_handler(& _tmp28);{
int _tmp2A=0;if(setjmp(_tmp28.handler))_tmp2A=1;if(!_tmp2A){{struct _RegionHandle*
_tmp2B=Cyc_Tcenv_get_fnrgn(te);void*_tmp2C=Cyc_Tcenv_lookup_ordinary(_tmp2B,te,
e1->loc,_tmp27);void*_tmp2E;struct Cyc_Absyn_Datatypedecl*_tmp30;struct Cyc_Absyn_Datatypefield*
_tmp31;struct Cyc_Absyn_Aggrdecl*_tmp33;_LL16: {struct Cyc_Tcenv_VarRes_struct*
_tmp2D=(struct Cyc_Tcenv_VarRes_struct*)_tmp2C;if(_tmp2D->tag != 0)goto _LL18;else{
_tmp2E=(void*)_tmp2D->f1;}}_LL17: e->r=(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp37;_tmp37.tag=11;_tmp37.f1=e1;_tmp37.f2=es;_tmp37.f3=0;_tmp37;});_tmp36;});
_npop_handler(0);return;_LL18: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp2F=(
struct Cyc_Tcenv_DatatypeRes_struct*)_tmp2C;if(_tmp2F->tag != 2)goto _LL1A;else{
_tmp30=_tmp2F->f1;_tmp31=_tmp2F->f2;}}_LL19: if(_tmp31->typs == 0)({struct Cyc_String_pa_struct
_tmp3A;_tmp3A.tag=0;_tmp3A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp31->name));({void*_tmp38[1]={& _tmp3A};Cyc_Tcutil_terr(e->loc,({const char*
_tmp39="%s is a constant, not a function";_tag_dyneither(_tmp39,sizeof(char),33);}),
_tag_dyneither(_tmp38,sizeof(void*),1));});});e->r=(void*)({struct Cyc_Absyn_Datatype_e_struct*
_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Absyn_Datatype_e_struct
_tmp3C;_tmp3C.tag=31;_tmp3C.f1=es;_tmp3C.f2=_tmp30;_tmp3C.f3=_tmp31;_tmp3C;});
_tmp3B;});_npop_handler(0);return;_LL1A: {struct Cyc_Tcenv_AggrRes_struct*_tmp32=(
struct Cyc_Tcenv_AggrRes_struct*)_tmp2C;if(_tmp32->tag != 1)goto _LL1C;else{_tmp33=
_tmp32->f1;}}_LL1B: {struct Cyc_List_List*_tmp3D=((struct Cyc_List_List*(*)(struct
_tuple16*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,
es);e->r=(void*)({struct Cyc_Absyn_Aggregate_e_struct*_tmp3E=_cycalloc(sizeof(*
_tmp3E));_tmp3E[0]=({struct Cyc_Absyn_Aggregate_e_struct _tmp3F;_tmp3F.tag=29;
_tmp3F.f1=_tmp33->name;_tmp3F.f2=0;_tmp3F.f3=_tmp3D;_tmp3F.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp33;_tmp3F;});_tmp3E;});_npop_handler(0);return;}_LL1C: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp34=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp2C;if(_tmp34->tag != 4)goto _LL1E;}
_LL1D: goto _LL1F;_LL1E: {struct Cyc_Tcenv_EnumRes_struct*_tmp35=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp2C;if(_tmp35->tag != 3)goto _LL15;}_LL1F:({struct Cyc_String_pa_struct _tmp42;
_tmp42.tag=0;_tmp42.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp27));({void*_tmp40[1]={& _tmp42};Cyc_Tcutil_terr(e->loc,({const char*_tmp41="%s is an enum constructor, not a function";
_tag_dyneither(_tmp41,sizeof(char),42);}),_tag_dyneither(_tmp40,sizeof(void*),1));});});
_npop_handler(0);return;_LL15:;};_pop_handler();}else{void*_tmp29=(void*)
_exn_thrown;void*_tmp44=_tmp29;_LL21: {struct Cyc_Dict_Absent_struct*_tmp45=(
struct Cyc_Dict_Absent_struct*)_tmp44;if(_tmp45->tag != Cyc_Dict_Absent)goto _LL23;}
_LL22: e->r=(void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp46=_cycalloc(sizeof(*
_tmp46));_tmp46[0]=({struct Cyc_Absyn_FnCall_e_struct _tmp47;_tmp47.tag=11;_tmp47.f1=
e1;_tmp47.f2=es;_tmp47.f3=0;_tmp47;});_tmp46;});return;_LL23:;_LL24:(void)_throw(
_tmp44);_LL20:;}};}_LL13:;_LL14: e->r=(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp49;_tmp49.tag=11;_tmp49.f1=e1;_tmp49.f2=es;_tmp49.f3=0;_tmp49;});_tmp48;});
return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){e->r=(void*)({struct Cyc_Absyn_Array_e_struct*_tmp4A=_cycalloc(
sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_Array_e_struct _tmp4B;_tmp4B.tag=27;
_tmp4B.f1=des;_tmp4B;});_tmp4A;});return;}{void*t=*topt;void*_tmp4C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_AggrInfo _tmp4E;union Cyc_Absyn_AggrInfoU _tmp4F;struct Cyc_Absyn_ArrayInfo
_tmp51;void*_tmp52;struct Cyc_Absyn_Tqual _tmp53;_LL26: {struct Cyc_Absyn_AggrType_struct*
_tmp4D=(struct Cyc_Absyn_AggrType_struct*)_tmp4C;if(_tmp4D->tag != 12)goto _LL28;
else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4E.aggr_info;}}_LL27:{union Cyc_Absyn_AggrInfoU
_tmp55=_tmp4F;struct _tuple3 _tmp56;struct Cyc_Absyn_Aggrdecl**_tmp57;struct Cyc_Absyn_Aggrdecl*
_tmp58;_LL2F: if((_tmp55.UnknownAggr).tag != 1)goto _LL31;_tmp56=(struct _tuple3)(
_tmp55.UnknownAggr).val;_LL30:({void*_tmp59=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5A="struct type not properly set";
_tag_dyneither(_tmp5A,sizeof(char),29);}),_tag_dyneither(_tmp59,sizeof(void*),0));});
_LL31: if((_tmp55.KnownAggr).tag != 2)goto _LL2E;_tmp57=(struct Cyc_Absyn_Aggrdecl**)(
_tmp55.KnownAggr).val;_tmp58=*_tmp57;_LL32: e->r=(void*)({struct Cyc_Absyn_Aggregate_e_struct*
_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Absyn_Aggregate_e_struct
_tmp5C;_tmp5C.tag=29;_tmp5C.f1=_tmp58->name;_tmp5C.f2=0;_tmp5C.f3=des;_tmp5C.f4=(
struct Cyc_Absyn_Aggrdecl*)_tmp58;_tmp5C;});_tmp5B;});_LL2E:;}goto _LL25;_LL28: {
struct Cyc_Absyn_ArrayType_struct*_tmp50=(struct Cyc_Absyn_ArrayType_struct*)
_tmp4C;if(_tmp50->tag != 9)goto _LL2A;else{_tmp51=_tmp50->f1;_tmp52=_tmp51.elt_type;
_tmp53=_tmp51.tq;}}_LL29: e->r=(void*)({struct Cyc_Absyn_Array_e_struct*_tmp5D=
_cycalloc(sizeof(*_tmp5D));_tmp5D[0]=({struct Cyc_Absyn_Array_e_struct _tmp5E;
_tmp5E.tag=27;_tmp5E.f1=des;_tmp5E;});_tmp5D;});goto _LL25;_LL2A: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp54=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4C;if(_tmp54->tag != 13)goto
_LL2C;}_LL2B: e->r=(void*)({struct Cyc_Absyn_AnonStruct_e_struct*_tmp5F=_cycalloc(
sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Absyn_AnonStruct_e_struct _tmp60;_tmp60.tag=
30;_tmp60.f1=(void*)t;_tmp60.f2=des;_tmp60;});_tmp5F;});goto _LL25;_LL2C:;_LL2D: e->r=(
void*)({struct Cyc_Absyn_Array_e_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[
0]=({struct Cyc_Absyn_Array_e_struct _tmp62;_tmp62.tag=27;_tmp62.f1=des;_tmp62;});
_tmp61;});goto _LL25;_LL25:;};}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e))({struct Cyc_String_pa_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));({struct Cyc_String_pa_struct _tmp65;_tmp65.tag=0;
_tmp65.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);({void*_tmp63[2]={&
_tmp65,& _tmp66};Cyc_Tcutil_terr(e->loc,({const char*_tmp64="test of %s has type %s instead of integral or * type";
_tag_dyneither(_tmp64,sizeof(char),53);}),_tag_dyneither(_tmp63,sizeof(void*),2));});});});{
void*_tmp67=e->r;enum Cyc_Absyn_Primop _tmp69;struct Cyc_List_List*_tmp6A;_LL34: {
struct Cyc_Absyn_Primop_e_struct*_tmp68=(struct Cyc_Absyn_Primop_e_struct*)_tmp67;
if(_tmp68->tag != 3)goto _LL36;else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL35:
if(_tmp69 == Cyc_Absyn_Eq  || _tmp69 == Cyc_Absyn_Neq){struct _tuple0 _tmp6C=({struct
_tuple0 _tmp6B;_tmp6B.f1=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp6A))->hd)->topt))->v;_tmp6B.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp6A))->tl))->hd)->topt))->v;
_tmp6B;});void*_tmp6D;void*_tmp6F;void*_tmp70;void*_tmp72;_LL39: _tmp6D=_tmp6C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp6E=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp6D;if(_tmp6E->tag != 16)goto _LL3B;else{_tmp6F=(void*)_tmp6E->f1;}};_tmp70=
_tmp6C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp71=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp70;if(_tmp71->tag != 16)goto _LL3B;else{_tmp72=(void*)_tmp71->f1;}};_LL3A:
return({struct Cyc_Tcexp_TestEnv _tmp73;_tmp73.eq=({struct _tuple0*_tmp74=
_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp74));_tmp74->f1=_tmp6F;_tmp74->f2=
_tmp72;_tmp74;});_tmp73.isTrue=_tmp69 == Cyc_Absyn_Eq;_tmp73;});_LL3B:;_LL3C: goto
_LL38;_LL38:;}goto _LL33;_LL36:;_LL37: goto _LL33;_LL33:;}return({struct Cyc_Tcexp_TestEnv
_tmp75;_tmp75.eq=0;_tmp75.isTrue=0;_tmp75;});}static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst*
c,struct Cyc_Absyn_Exp*e){void*t;{union Cyc_Absyn_Cnst _tmp76=*((union Cyc_Absyn_Cnst*)
_check_null(c));struct _tuple4 _tmp77;enum Cyc_Absyn_Sign _tmp78;struct _tuple4
_tmp79;enum Cyc_Absyn_Sign _tmp7A;struct _tuple4 _tmp7B;enum Cyc_Absyn_Sign _tmp7C;
struct _tuple5 _tmp7D;enum Cyc_Absyn_Sign _tmp7E;struct _tuple7 _tmp7F;enum Cyc_Absyn_Sign
_tmp80;struct _dyneither_ptr _tmp81;struct _tuple6 _tmp82;enum Cyc_Absyn_Sign _tmp83;
int _tmp84;struct _dyneither_ptr _tmp85;int _tmp86;_LL3E: if((_tmp76.Char_c).tag != 2)
goto _LL40;_tmp77=(struct _tuple4)(_tmp76.Char_c).val;_tmp78=_tmp77.f1;if(_tmp78 != 
Cyc_Absyn_Signed)goto _LL40;_LL3F: t=Cyc_Absyn_schar_typ;goto _LL3D;_LL40: if((
_tmp76.Char_c).tag != 2)goto _LL42;_tmp79=(struct _tuple4)(_tmp76.Char_c).val;
_tmp7A=_tmp79.f1;if(_tmp7A != Cyc_Absyn_Unsigned)goto _LL42;_LL41: t=Cyc_Absyn_uchar_typ;
goto _LL3D;_LL42: if((_tmp76.Char_c).tag != 2)goto _LL44;_tmp7B=(struct _tuple4)(
_tmp76.Char_c).val;_tmp7C=_tmp7B.f1;if(_tmp7C != Cyc_Absyn_None)goto _LL44;_LL43: t=
Cyc_Absyn_char_typ;goto _LL3D;_LL44: if((_tmp76.Short_c).tag != 3)goto _LL46;_tmp7D=(
struct _tuple5)(_tmp76.Short_c).val;_tmp7E=_tmp7D.f1;_LL45: t=_tmp7E == Cyc_Absyn_Unsigned?
Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL3D;_LL46: if((_tmp76.LongLong_c).tag
!= 5)goto _LL48;_tmp7F=(struct _tuple7)(_tmp76.LongLong_c).val;_tmp80=_tmp7F.f1;
_LL47: t=_tmp80 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;
goto _LL3D;_LL48: if((_tmp76.Float_c).tag != 6)goto _LL4A;_tmp81=(struct
_dyneither_ptr)(_tmp76.Float_c).val;_LL49: t=Cyc_Absyn_float_typ;goto _LL3D;_LL4A:
if((_tmp76.Int_c).tag != 4)goto _LL4C;_tmp82=(struct _tuple6)(_tmp76.Int_c).val;
_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;_LL4B: if(topt == 0)t=_tmp83 == Cyc_Absyn_Unsigned?
Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp87=Cyc_Tcutil_compress(*topt);
enum Cyc_Absyn_Sign _tmp89;enum Cyc_Absyn_Size_of _tmp8A;enum Cyc_Absyn_Sign _tmp8C;
enum Cyc_Absyn_Size_of _tmp8D;enum Cyc_Absyn_Sign _tmp8F;enum Cyc_Absyn_Size_of
_tmp90;enum Cyc_Absyn_Sign _tmp92;enum Cyc_Absyn_Size_of _tmp93;void*_tmp96;_LL51: {
struct Cyc_Absyn_IntType_struct*_tmp88=(struct Cyc_Absyn_IntType_struct*)_tmp87;
if(_tmp88->tag != 6)goto _LL53;else{_tmp89=_tmp88->f1;_tmp8A=_tmp88->f2;if(_tmp8A
!= Cyc_Absyn_Char_sz)goto _LL53;}}_LL52: switch(_tmp89){case Cyc_Absyn_Unsigned:
_LL5F: t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL60: t=Cyc_Absyn_schar_typ;
break;case Cyc_Absyn_None: _LL61: t=Cyc_Absyn_char_typ;break;}*c=Cyc_Absyn_Char_c(
_tmp89,(char)_tmp84);goto _LL50;_LL53: {struct Cyc_Absyn_IntType_struct*_tmp8B=(
struct Cyc_Absyn_IntType_struct*)_tmp87;if(_tmp8B->tag != 6)goto _LL55;else{_tmp8C=
_tmp8B->f1;_tmp8D=_tmp8B->f2;if(_tmp8D != Cyc_Absyn_Short_sz)goto _LL55;}}_LL54: t=
_tmp8C == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;*c=Cyc_Absyn_Short_c(
_tmp8C,(short)_tmp84);goto _LL50;_LL55: {struct Cyc_Absyn_IntType_struct*_tmp8E=(
struct Cyc_Absyn_IntType_struct*)_tmp87;if(_tmp8E->tag != 6)goto _LL57;else{_tmp8F=
_tmp8E->f1;_tmp90=_tmp8E->f2;if(_tmp90 != Cyc_Absyn_Int_sz)goto _LL57;}}_LL56: t=
_tmp8F == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL50;
_LL57: {struct Cyc_Absyn_IntType_struct*_tmp91=(struct Cyc_Absyn_IntType_struct*)
_tmp87;if(_tmp91->tag != 6)goto _LL59;else{_tmp92=_tmp91->f1;_tmp93=_tmp91->f2;if(
_tmp93 != Cyc_Absyn_Long_sz)goto _LL59;}}_LL58: t=_tmp92 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL50;_LL59:{struct Cyc_Absyn_PointerType_struct*_tmp94=(
struct Cyc_Absyn_PointerType_struct*)_tmp87;if(_tmp94->tag != 5)goto _LL5B;}if(!(
_tmp84 == 0))goto _LL5B;_LL5A: e->r=(void*)({struct Cyc_Absyn_Const_e_struct*_tmp97=
_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_Const_e_struct _tmp98;
_tmp98.tag=0;_tmp98.f1=Cyc_Absyn_Null_c;_tmp98;});_tmp97;});{struct Cyc_List_List*
_tmp99=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_PointerType_struct
_tmp9B;_tmp9B.tag=5;_tmp9B.f1=({struct Cyc_Absyn_PtrInfo _tmp9C;_tmp9C.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp9F=_cycalloc(sizeof(*
_tmp9F));_tmp9F->v=_tmp99;_tmp9F;}));_tmp9C.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmp9C.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9D;_tmp9D.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmp9E=_cycalloc(
sizeof(*_tmp9E));_tmp9E->v=_tmp99;_tmp9E;}));_tmp9D.nullable=Cyc_Absyn_true_conref;
_tmp9D.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp9D.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp9D.ptrloc=0;_tmp9D;});
_tmp9C;});_tmp9B;});_tmp9A;});goto _LL50;};_LL5B: {struct Cyc_Absyn_TagType_struct*
_tmp95=(struct Cyc_Absyn_TagType_struct*)_tmp87;if(_tmp95->tag != 20)goto _LL5D;
else{_tmp96=(void*)_tmp95->f1;}}_LL5C: {struct Cyc_Absyn_ValueofType_struct*
_tmpA0=({struct Cyc_Absyn_ValueofType_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));
_tmpA7[0]=({struct Cyc_Absyn_ValueofType_struct _tmpA8;_tmpA8.tag=19;_tmpA8.f1=Cyc_Absyn_uint_exp((
unsigned int)_tmp84,0);_tmpA8;});_tmpA7;});if(!Cyc_Tcutil_unify(_tmp96,(void*)
_tmpA0)){({struct Cyc_String_pa_struct _tmpA4;_tmpA4.tag=0;_tmpA4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmpA0));({
struct Cyc_String_pa_struct _tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp96));({void*_tmpA1[2]={& _tmpA3,&
_tmpA4};Cyc_Tcutil_terr(loc,({const char*_tmpA2="expecting %s but found %s";
_tag_dyneither(_tmpA2,sizeof(char),26);}),_tag_dyneither(_tmpA1,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}t=(void*)({struct Cyc_Absyn_TagType_struct*_tmpA5=
_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_TagType_struct _tmpA6;
_tmpA6.tag=20;_tmpA6.f1=(void*)((void*)_tmpA0);_tmpA6;});_tmpA5;});goto _LL50;}
_LL5D:;_LL5E: t=_tmp83 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL50;_LL50:;}goto _LL3D;_LL4C: if((_tmp76.String_c).tag != 7)goto _LL4E;_tmp85=(
struct _dyneither_ptr)(_tmp76.String_c).val;_LL4D: {int len=(int)
_get_dyneither_size(_tmp85,sizeof(char));struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,len),loc);elen->topt=({struct Cyc_Core_Opt*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->v=Cyc_Absyn_uint_typ;_tmpA9;});t=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({
struct Cyc_Absyn_Upper_b_struct _tmpAB;_tmpAB.tag=1;_tmpAB.f1=elen;_tmpAB;});
_tmpAA;}),Cyc_Absyn_true_conref);if(topt != 0){void*_tmpAC=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_ArrayInfo _tmpAE;struct Cyc_Absyn_Tqual _tmpAF;_LL64: {struct
Cyc_Absyn_ArrayType_struct*_tmpAD=(struct Cyc_Absyn_ArrayType_struct*)_tmpAC;if(
_tmpAD->tag != 9)goto _LL66;else{_tmpAE=_tmpAD->f1;_tmpAF=_tmpAE.tq;}}_LL65: return
Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmpAF,(struct Cyc_Absyn_Exp*)elen,((union
Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL66: {struct Cyc_Absyn_PointerType_struct*
_tmpB0=(struct Cyc_Absyn_PointerType_struct*)_tmpAC;if(_tmpB0->tag != 5)goto _LL68;}
_LL67: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=({struct Cyc_Core_Opt*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->v=t;
_tmpB1;});Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);t=*topt;}
else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({
struct Cyc_Absyn_Upper_b_struct _tmpB3;_tmpB3.tag=1;_tmpB3.f1=elen;_tmpB3;});
_tmpB2;}),Cyc_Absyn_true_conref);if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){e->topt=({struct Cyc_Core_Opt*_tmpB4=_cycalloc(sizeof(*_tmpB4));
_tmpB4->v=t;_tmpB4;});Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}goto _LL63;_LL68:;_LL69: goto _LL63;_LL63:;}return t;}_LL4E: if((_tmp76.Null_c).tag
!= 1)goto _LL3D;_tmp86=(int)(_tmp76.Null_c).val;_LL4F: {struct Cyc_List_List*
_tmpB5=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_PointerType_struct
_tmpB7;_tmpB7.tag=5;_tmpB7.f1=({struct Cyc_Absyn_PtrInfo _tmpB8;_tmpB8.elt_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmpBB=_cycalloc(sizeof(*
_tmpBB));_tmpBB->v=_tmpB5;_tmpBB;}));_tmpB8.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmpB8.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpB9;_tmpB9.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({struct Cyc_Core_Opt*_tmpBA=_cycalloc(
sizeof(*_tmpBA));_tmpBA->v=_tmpB5;_tmpBA;}));_tmpB9.nullable=Cyc_Absyn_true_conref;
_tmpB9.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmpB9.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmpB9.ptrloc=0;_tmpB9;});
_tmpB8;});_tmpB7;});_tmpB6;});goto _LL3D;}_LL3D:;}return t;}static void*Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e,
struct _tuple1*q,void*b){void*_tmpBC=b;struct Cyc_Absyn_Vardecl*_tmpBF;struct Cyc_Absyn_Fndecl*
_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC5;struct Cyc_Absyn_Vardecl*
_tmpC7;_LL6B: {struct Cyc_Absyn_Unresolved_b_struct*_tmpBD=(struct Cyc_Absyn_Unresolved_b_struct*)
_tmpBC;if(_tmpBD->tag != 0)goto _LL6D;}_LL6C: return({struct Cyc_String_pa_struct
_tmpCA;_tmpCA.tag=0;_tmpCA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q));({void*_tmpC8[1]={& _tmpCA};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmpC9="undeclared identifier: %s";
_tag_dyneither(_tmpC9,sizeof(char),26);}),_tag_dyneither(_tmpC8,sizeof(void*),1));});});
_LL6D: {struct Cyc_Absyn_Global_b_struct*_tmpBE=(struct Cyc_Absyn_Global_b_struct*)
_tmpBC;if(_tmpBE->tag != 1)goto _LL6F;else{_tmpBF=_tmpBE->f1;}}_LL6E:*q=*_tmpBF->name;
return _tmpBF->type;_LL6F: {struct Cyc_Absyn_Funname_b_struct*_tmpC0=(struct Cyc_Absyn_Funname_b_struct*)
_tmpBC;if(_tmpC0->tag != 2)goto _LL71;else{_tmpC1=_tmpC0->f1;}}_LL70:*q=*_tmpC1->name;
return Cyc_Tcutil_fndecl2typ(_tmpC1);_LL71: {struct Cyc_Absyn_Pat_b_struct*_tmpC2=(
struct Cyc_Absyn_Pat_b_struct*)_tmpBC;if(_tmpC2->tag != 5)goto _LL73;else{_tmpC3=
_tmpC2->f1;}}_LL72: _tmpC5=_tmpC3;goto _LL74;_LL73: {struct Cyc_Absyn_Local_b_struct*
_tmpC4=(struct Cyc_Absyn_Local_b_struct*)_tmpBC;if(_tmpC4->tag != 4)goto _LL75;
else{_tmpC5=_tmpC4->f1;}}_LL74: _tmpC7=_tmpC5;goto _LL76;_LL75: {struct Cyc_Absyn_Param_b_struct*
_tmpC6=(struct Cyc_Absyn_Param_b_struct*)_tmpBC;if(_tmpC6->tag != 3)goto _LL6A;
else{_tmpC7=_tmpC6->f1;}}_LL76:(*q).f1=({union Cyc_Absyn_Nmspace _tmpCB;(_tmpCB.Loc_n).val=
0;(_tmpCB.Loc_n).tag=3;_tmpCB;});if(te->allow_valueof){void*_tmpCC=Cyc_Tcutil_compress(
_tmpC7->type);void*_tmpCE;_LL78: {struct Cyc_Absyn_TagType_struct*_tmpCD=(struct
Cyc_Absyn_TagType_struct*)_tmpCC;if(_tmpCD->tag != 20)goto _LL7A;else{_tmpCE=(void*)
_tmpCD->f1;}}_LL79: e->r=(void*)({struct Cyc_Absyn_Valueof_e_struct*_tmpCF=
_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=({struct Cyc_Absyn_Valueof_e_struct _tmpD0;
_tmpD0.tag=39;_tmpD0.f1=(void*)_tmpCE;_tmpD0;});_tmpCF;});goto _LL77;_LL7A:;_LL7B:
goto _LL77;_LL77:;}return _tmpC7->type;_LL6A:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*)){struct
Cyc_List_List*desc_types;{void*_tmpD1=fmt->r;union Cyc_Absyn_Cnst _tmpD3;struct
_dyneither_ptr _tmpD4;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp _tmpD7;void*
_tmpD8;union Cyc_Absyn_Cnst _tmpDA;struct _dyneither_ptr _tmpDB;_LL7D: {struct Cyc_Absyn_Const_e_struct*
_tmpD2=(struct Cyc_Absyn_Const_e_struct*)_tmpD1;if(_tmpD2->tag != 0)goto _LL7F;
else{_tmpD3=_tmpD2->f1;if((_tmpD3.String_c).tag != 7)goto _LL7F;_tmpD4=(struct
_dyneither_ptr)(_tmpD3.String_c).val;}}_LL7E: _tmpDB=_tmpD4;goto _LL80;_LL7F: {
struct Cyc_Absyn_Cast_e_struct*_tmpD5=(struct Cyc_Absyn_Cast_e_struct*)_tmpD1;if(
_tmpD5->tag != 15)goto _LL81;else{_tmpD6=_tmpD5->f2;_tmpD7=*_tmpD6;_tmpD8=_tmpD7.r;{
struct Cyc_Absyn_Const_e_struct*_tmpD9=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;
if(_tmpD9->tag != 0)goto _LL81;else{_tmpDA=_tmpD9->f1;if((_tmpDA.String_c).tag != 7)
goto _LL81;_tmpDB=(struct _dyneither_ptr)(_tmpDA.String_c).val;}};}}_LL80:
desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpDB,fmt->loc);goto _LL7C;
_LL81:;_LL82: if(opt_args != 0){struct Cyc_List_List*_tmpDC=(struct Cyc_List_List*)
opt_args->v;for(0;_tmpDC != 0;_tmpDC=_tmpDC->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmpDC->hd);{struct _RegionHandle*_tmpDD=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpDD,(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpDC->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path(_tmpDD,(struct Cyc_Absyn_Exp*)_tmpDC->hd))({void*
_tmpDE=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpDC->hd)->loc,({const char*
_tmpDF="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmpDF,
sizeof(char),49);}),_tag_dyneither(_tmpDE,sizeof(void*),0));});};}}return;_LL7C:;}
if(opt_args != 0){struct Cyc_List_List*_tmpE0=(struct Cyc_List_List*)opt_args->v;
for(0;desc_types != 0  && _tmpE0 != 0;(desc_types=desc_types->tl,_tmpE0=_tmpE0->tl)){
void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpE0->hd;
Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_String_pa_struct
_tmpE4;_tmpE4.tag=0;_tmpE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));({struct Cyc_String_pa_struct
_tmpE3;_tmpE3.tag=0;_tmpE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));({void*_tmpE1[2]={& _tmpE3,& _tmpE4};Cyc_Tcutil_terr(e->loc,({const char*_tmpE2="descriptor has type \n%s\n but argument has type \n%s";
_tag_dyneither(_tmpE2,sizeof(char),51);}),_tag_dyneither(_tmpE1,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}{struct _RegionHandle*_tmpE5=Cyc_Tcenv_get_fnrgn(te);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpE5,t) && !Cyc_Tcutil_is_noalias_path(
_tmpE5,e))({void*_tmpE6=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,({
const char*_tmpE7="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmpE7,sizeof(char),49);}),_tag_dyneither(_tmpE6,sizeof(void*),0));});};}
if(desc_types != 0)({void*_tmpE8=0;Cyc_Tcutil_terr(fmt->loc,({const char*_tmpE9="too few arguments";
_tag_dyneither(_tmpE9,sizeof(char),18);}),_tag_dyneither(_tmpE8,sizeof(void*),0));});
if(_tmpE0 != 0)({void*_tmpEA=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE0->hd)->loc,({
const char*_tmpEB="too many arguments";_tag_dyneither(_tmpEB,sizeof(char),19);}),
_tag_dyneither(_tmpEA,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,enum Cyc_Absyn_Primop
p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);switch(p){case Cyc_Absyn_Plus: _LL83: goto _LL84;case Cyc_Absyn_Minus:
_LL84: if(!Cyc_Tcutil_is_numeric(e))({struct Cyc_String_pa_struct _tmpEE;_tmpEE.tag=
0;_tmpEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));({void*_tmpEC[1]={& _tmpEE};Cyc_Tcutil_terr(loc,({const char*_tmpED="expecting arithmetic type but found %s";
_tag_dyneither(_tmpED,sizeof(char),39);}),_tag_dyneither(_tmpEC,sizeof(void*),1));});});
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Not:
_LL85: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(te,e))({
struct Cyc_String_pa_struct _tmpF1;_tmpF1.tag=0;_tmpF1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpEF[1]={& _tmpF1};Cyc_Tcutil_terr(
loc,({const char*_tmpF0="expecting integral or * type but found %s";
_tag_dyneither(_tmpF0,sizeof(char),42);}),_tag_dyneither(_tmpEF,sizeof(void*),1));});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL86: if(!Cyc_Tcutil_is_integral(e))({
struct Cyc_String_pa_struct _tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpF2[1]={& _tmpF4};Cyc_Tcutil_terr(
loc,({const char*_tmpF3="expecting integral type but found %s";_tag_dyneither(
_tmpF3,sizeof(char),37);}),_tag_dyneither(_tmpF2,sizeof(void*),1));});});return(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;case Cyc_Absyn_Numelts: _LL87:{
void*_tmpF5=t;struct Cyc_Absyn_PtrInfo _tmpF8;struct Cyc_Absyn_PtrAtts _tmpF9;union
Cyc_Absyn_Constraint*_tmpFA;_LL8A: {struct Cyc_Absyn_ArrayType_struct*_tmpF6=(
struct Cyc_Absyn_ArrayType_struct*)_tmpF5;if(_tmpF6->tag != 9)goto _LL8C;}_LL8B:
goto _LL89;_LL8C: {struct Cyc_Absyn_PointerType_struct*_tmpF7=(struct Cyc_Absyn_PointerType_struct*)
_tmpF5;if(_tmpF7->tag != 5)goto _LL8E;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF8.ptr_atts;
_tmpFA=_tmpF9.bounds;}}_LL8D:{void*_tmpFB=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpFA);struct Cyc_Absyn_Exp*
_tmpFE;_LL91: {struct Cyc_Absyn_DynEither_b_struct*_tmpFC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpFB;if(_tmpFC->tag != 0)goto _LL93;}_LL92: goto _LL90;_LL93: {struct Cyc_Absyn_Upper_b_struct*
_tmpFD=(struct Cyc_Absyn_Upper_b_struct*)_tmpFB;if(_tmpFD->tag != 1)goto _LL90;
else{_tmpFE=_tmpFD->f1;}}_LL94: if(!Cyc_Evexp_c_can_eval(_tmpFE))({void*_tmpFF=0;
Cyc_Tcutil_terr(loc,({const char*_tmp100="cannot apply numelts to a pointer with abstract bounds";
_tag_dyneither(_tmp100,sizeof(char),55);}),_tag_dyneither(_tmpFF,sizeof(void*),0));});
goto _LL90;_LL90:;}goto _LL89;_LL8E:;_LL8F:({struct Cyc_String_pa_struct _tmp103;
_tmp103.tag=0;_tmp103.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));({void*_tmp101[1]={& _tmp103};Cyc_Tcutil_terr(loc,({const char*_tmp102="numelts requires pointer or array type, not %s";
_tag_dyneither(_tmp102,sizeof(char),47);}),_tag_dyneither(_tmp101,sizeof(void*),
1));});});_LL89:;}return Cyc_Absyn_uint_typ;default: _LL88:({void*_tmp104=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp105="Non-unary primop";_tag_dyneither(_tmp105,sizeof(char),17);}),
_tag_dyneither(_tmp104,sizeof(void*),0));});}}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*)){if(!checker(e1)){({struct Cyc_String_pa_struct
_tmp108;_tmp108.tag=0;_tmp108.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));({
void*_tmp106[1]={& _tmp108};Cyc_Tcutil_terr(e1->loc,({const char*_tmp107="type %s cannot be used here";
_tag_dyneither(_tmp107,sizeof(char),28);}),_tag_dyneither(_tmp106,sizeof(void*),
1));});});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}if(!
checker(e2)){({struct Cyc_String_pa_struct _tmp10B;_tmp10B.tag=0;_tmp10B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v));({void*_tmp109[1]={& _tmp10B};Cyc_Tcutil_terr(
e2->loc,({const char*_tmp10A="type %s cannot be used here";_tag_dyneither(_tmp10A,
sizeof(char),28);}),_tag_dyneither(_tmp109,sizeof(void*),1));});});return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);};}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);void*_tmp10C=t1;struct Cyc_Absyn_PtrInfo _tmp10E;void*
_tmp10F;struct Cyc_Absyn_Tqual _tmp110;struct Cyc_Absyn_PtrAtts _tmp111;void*_tmp112;
union Cyc_Absyn_Constraint*_tmp113;union Cyc_Absyn_Constraint*_tmp114;union Cyc_Absyn_Constraint*
_tmp115;_LL97: {struct Cyc_Absyn_PointerType_struct*_tmp10D=(struct Cyc_Absyn_PointerType_struct*)
_tmp10C;if(_tmp10D->tag != 5)goto _LL99;else{_tmp10E=_tmp10D->f1;_tmp10F=_tmp10E.elt_typ;
_tmp110=_tmp10E.elt_tq;_tmp111=_tmp10E.ptr_atts;_tmp112=_tmp111.rgn;_tmp113=
_tmp111.nullable;_tmp114=_tmp111.bounds;_tmp115=_tmp111.zero_term;}}_LL98: if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp10F),Cyc_Absyn_MemKind))({void*
_tmp116=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp117="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmp117,sizeof(char),50);}),_tag_dyneither(_tmp116,sizeof(void*),
0));});if(Cyc_Tcutil_is_noalias_pointer(t1))({void*_tmp118=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmp119="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp119,sizeof(char),54);}),_tag_dyneither(_tmp118,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2))({struct Cyc_String_pa_struct _tmp11C;
_tmp11C.tag=0;_tmp11C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));({void*_tmp11A[1]={& _tmp11C};Cyc_Tcutil_terr(e2->loc,({const char*_tmp11B="expecting int but found %s";
_tag_dyneither(_tmp11B,sizeof(char),27);}),_tag_dyneither(_tmp11A,sizeof(void*),
1));});});{void*_tmp11D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((
void*)& Cyc_Absyn_DynEither_b_val,_tmp114);struct Cyc_Absyn_Exp*_tmp120;_LL9C: {
struct Cyc_Absyn_DynEither_b_struct*_tmp11E=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp11D;if(_tmp11E->tag != 0)goto _LL9E;}_LL9D: return t1;_LL9E: {struct Cyc_Absyn_Upper_b_struct*
_tmp11F=(struct Cyc_Absyn_Upper_b_struct*)_tmp11D;if(_tmp11F->tag != 1)goto _LL9B;
else{_tmp120=_tmp11F->f1;}}_LL9F: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp115))({void*_tmp121=0;Cyc_Tcutil_warn(e1->loc,({
const char*_tmp122="pointer arithmetic on thin, zero-terminated pointer may be expensive.";
_tag_dyneither(_tmp122,sizeof(char),70);}),_tag_dyneither(_tmp121,sizeof(void*),
0));});{struct Cyc_Absyn_PointerType_struct*_tmp123=({struct Cyc_Absyn_PointerType_struct*
_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124[0]=({struct Cyc_Absyn_PointerType_struct
_tmp125;_tmp125.tag=5;_tmp125.f1=({struct Cyc_Absyn_PtrInfo _tmp126;_tmp126.elt_typ=
_tmp10F;_tmp126.elt_tq=_tmp110;_tmp126.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp127;
_tmp127.rgn=_tmp112;_tmp127.nullable=Cyc_Absyn_true_conref;_tmp127.bounds=Cyc_Absyn_bounds_dyneither_conref;
_tmp127.zero_term=_tmp115;_tmp127.ptrloc=0;_tmp127;});_tmp126;});_tmp125;});
_tmp124;});Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp123,Cyc_Absyn_Other_coercion);
return(void*)_tmp123;};_LL9B:;};_LL99:;_LL9A: return Cyc_Tcexp_tcArithBinop(te,e1,
e2,Cyc_Tcutil_is_numeric);_LL96:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(!Cyc_Tcutil_unify(t1,t2)){({
struct Cyc_String_pa_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v));({struct Cyc_String_pa_struct _tmp12A;_tmp12A.tag=0;
_tmp12A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));({void*_tmp128[2]={&
_tmp12A,& _tmp12B};Cyc_Tcutil_terr(e1->loc,({const char*_tmp129="pointer arithmetic on values of different types (%s != %s)";
_tag_dyneither(_tmp129,sizeof(char),59);}),_tag_dyneither(_tmp128,sizeof(void*),
2));});});});Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!
Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),Cyc_Absyn_MemKind))({void*
_tmp12C=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp12D="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmp12D,sizeof(char),50);}),_tag_dyneither(_tmp12C,sizeof(void*),
0));});if(Cyc_Tcutil_is_noalias_pointer(t1))({void*_tmp12E=0;Cyc_Tcutil_terr(e1->loc,({
const char*_tmp12F="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp12F,sizeof(char),54);}),_tag_dyneither(_tmp12E,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){({struct Cyc_String_pa_struct
_tmp133;_tmp133.tag=0;_tmp133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_struct _tmp132;_tmp132.tag=0;
_tmp132.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));({void*_tmp130[2]={& _tmp132,& _tmp133};Cyc_Tcutil_terr(e2->loc,({const char*
_tmp131="expecting either %s or int but found %s";_tag_dyneither(_tmp131,sizeof(
char),40);}),_tag_dyneither(_tmp130,sizeof(void*),2));});});});Cyc_Tcutil_explain_failure();}
return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,
Cyc_Absyn_Other_coercion);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);}static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num  && e2_is_num)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_typ_kind(
t1)== Cyc_Absyn_BoxKind  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_bk,Cyc_Tcenv_lookup_opt_type_vars(te)))){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(
te,e2,t1,Cyc_Absyn_Other_coercion);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(
te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(
te,t1,e2))return Cyc_Absyn_sint_typ;else{goto pointer_cmp;}}}}}else{pointer_cmp: {
struct _tuple0 _tmp135=({struct _tuple0 _tmp134;_tmp134.f1=Cyc_Tcutil_compress(t1);
_tmp134.f2=Cyc_Tcutil_compress(t2);_tmp134;});void*_tmp136;struct Cyc_Absyn_PtrInfo
_tmp138;void*_tmp139;void*_tmp13A;struct Cyc_Absyn_PtrInfo _tmp13C;void*_tmp13D;
void*_tmp13E;void*_tmp140;_LLA1: _tmp136=_tmp135.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp137=(struct Cyc_Absyn_PointerType_struct*)_tmp136;if(_tmp137->tag != 5)goto
_LLA3;else{_tmp138=_tmp137->f1;_tmp139=_tmp138.elt_typ;}};_tmp13A=_tmp135.f2;{
struct Cyc_Absyn_PointerType_struct*_tmp13B=(struct Cyc_Absyn_PointerType_struct*)
_tmp13A;if(_tmp13B->tag != 5)goto _LLA3;else{_tmp13C=_tmp13B->f1;_tmp13D=_tmp13C.elt_typ;}};
_LLA2: if(Cyc_Tcutil_unify(_tmp139,_tmp13D))return Cyc_Absyn_sint_typ;goto _LLA0;
_LLA3: _tmp13E=_tmp135.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp13F=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmp13E;if(_tmp13F->tag != 16)goto _LLA5;};_tmp140=
_tmp135.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp141=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp140;if(_tmp141->tag != 16)goto _LLA5;};_LLA4: return Cyc_Absyn_sint_typ;_LLA5:;
_LLA6: goto _LLA0;_LLA0:;}({struct Cyc_String_pa_struct _tmp145;_tmp145.tag=0;
_tmp145.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));({struct Cyc_String_pa_struct _tmp144;_tmp144.tag=0;_tmp144.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp142[
2]={& _tmp144,& _tmp145};Cyc_Tcutil_terr(loc,({const char*_tmp143="comparison not allowed between %s and %s";
_tag_dyneither(_tmp143,sizeof(char),41);}),_tag_dyneither(_tmp142,sizeof(void*),
2));});});});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){if(Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2)== Cyc_Absyn_sint_typ)return Cyc_Absyn_sint_typ;{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{struct _tuple0 _tmp147=({
struct _tuple0 _tmp146;_tmp146.f1=t1;_tmp146.f2=t2;_tmp146;});void*_tmp148;void*
_tmp14A;void*_tmp14B;void*_tmp14D;_LLA8: _tmp148=_tmp147.f1;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp149=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp148;if(_tmp149->tag != 16)goto
_LLAA;else{_tmp14A=(void*)_tmp149->f1;}};_tmp14B=_tmp147.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp14C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp14B;if(_tmp14C->tag != 16)goto
_LLAA;else{_tmp14D=(void*)_tmp14C->f1;}};_LLA9: return Cyc_Absyn_sint_typ;_LLAA:;
_LLAB: goto _LLA7;_LLA7:;}({struct Cyc_String_pa_struct _tmp151;_tmp151.tag=0;
_tmp151.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));({struct Cyc_String_pa_struct _tmp150;_tmp150.tag=0;_tmp150.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp14E[
2]={& _tmp150,& _tmp151};Cyc_Tcutil_terr(loc,({const char*_tmp14F="comparison not allowed between %s and %s";
_tag_dyneither(_tmp14F,sizeof(char),41);}),_tag_dyneither(_tmp14E,sizeof(void*),
2));});});});Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));};}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){switch(p){case Cyc_Absyn_Plus: _LLAC: return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus:
_LLAD: return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLAE: goto _LLAF;case
Cyc_Absyn_Div: _LLAF: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
case Cyc_Absyn_Mod: _LLB0: goto _LLB1;case Cyc_Absyn_Bitand: _LLB1: goto _LLB2;case Cyc_Absyn_Bitor:
_LLB2: goto _LLB3;case Cyc_Absyn_Bitxor: _LLB3: goto _LLB4;case Cyc_Absyn_Bitlshift:
_LLB4: goto _LLB5;case Cyc_Absyn_Bitlrshift: _LLB5: goto _LLB6;case Cyc_Absyn_Bitarshift:
_LLB6: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq:
_LLB7: goto _LLB8;case Cyc_Absyn_Neq: _LLB8: return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);
case Cyc_Absyn_Gt: _LLB9: goto _LLBA;case Cyc_Absyn_Lt: _LLBA: goto _LLBB;case Cyc_Absyn_Gte:
_LLBB: goto _LLBC;case Cyc_Absyn_Lte: _LLBC: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);
default: _LLBD:({void*_tmp152=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp153="bad binary primop";
_tag_dyneither(_tmp153,sizeof(char),18);}),_tag_dyneither(_tmp152,sizeof(void*),
0));});}}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){if(p == Cyc_Absyn_Minus
 && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1)return Cyc_Tcexp_tcExp(
te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);Cyc_Tcexp_tcExpList(
te,es);{void*t;switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case
0: _LLBF: return({void*_tmp154=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*
_tmp155="primitive operator has 0 arguments";_tag_dyneither(_tmp155,sizeof(char),
35);}),_tag_dyneither(_tmp154,sizeof(void*),0));});case 1: _LLC0: t=Cyc_Tcexp_tcUnPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);
break;case 2: _LLC1: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default:
_LLC2: return({void*_tmp156=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp157="primitive operator has > 2 arguments";
_tag_dyneither(_tmp157,sizeof(char),37);}),_tag_dyneither(_tmp156,sizeof(void*),
0));});}return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void
Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*
_tmp158=e->r;void*_tmp15A;struct Cyc_Absyn_Vardecl*_tmp15C;void*_tmp15E;struct Cyc_Absyn_Vardecl*
_tmp160;void*_tmp162;struct Cyc_Absyn_Vardecl*_tmp164;void*_tmp166;struct Cyc_Absyn_Vardecl*
_tmp168;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*
_tmp16D;struct _dyneither_ptr*_tmp16E;struct Cyc_Absyn_Exp*_tmp170;struct
_dyneither_ptr*_tmp171;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp175;
struct Cyc_Absyn_Exp*_tmp177;_LLC5: {struct Cyc_Absyn_Var_e_struct*_tmp159=(struct
Cyc_Absyn_Var_e_struct*)_tmp158;if(_tmp159->tag != 1)goto _LLC7;else{_tmp15A=(void*)
_tmp159->f2;{struct Cyc_Absyn_Param_b_struct*_tmp15B=(struct Cyc_Absyn_Param_b_struct*)
_tmp15A;if(_tmp15B->tag != 3)goto _LLC7;else{_tmp15C=_tmp15B->f1;}};}}_LLC6:
_tmp160=_tmp15C;goto _LLC8;_LLC7: {struct Cyc_Absyn_Var_e_struct*_tmp15D=(struct
Cyc_Absyn_Var_e_struct*)_tmp158;if(_tmp15D->tag != 1)goto _LLC9;else{_tmp15E=(void*)
_tmp15D->f2;{struct Cyc_Absyn_Local_b_struct*_tmp15F=(struct Cyc_Absyn_Local_b_struct*)
_tmp15E;if(_tmp15F->tag != 4)goto _LLC9;else{_tmp160=_tmp15F->f1;}};}}_LLC8:
_tmp164=_tmp160;goto _LLCA;_LLC9: {struct Cyc_Absyn_Var_e_struct*_tmp161=(struct
Cyc_Absyn_Var_e_struct*)_tmp158;if(_tmp161->tag != 1)goto _LLCB;else{_tmp162=(void*)
_tmp161->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp163=(struct Cyc_Absyn_Pat_b_struct*)
_tmp162;if(_tmp163->tag != 5)goto _LLCB;else{_tmp164=_tmp163->f1;}};}}_LLCA:
_tmp168=_tmp164;goto _LLCC;_LLCB: {struct Cyc_Absyn_Var_e_struct*_tmp165=(struct
Cyc_Absyn_Var_e_struct*)_tmp158;if(_tmp165->tag != 1)goto _LLCD;else{_tmp166=(void*)
_tmp165->f2;{struct Cyc_Absyn_Global_b_struct*_tmp167=(struct Cyc_Absyn_Global_b_struct*)
_tmp166;if(_tmp167->tag != 1)goto _LLCD;else{_tmp168=_tmp167->f1;}};}}_LLCC: if(!(
_tmp168->tq).real_const)return;goto _LLC4;_LLCD: {struct Cyc_Absyn_Subscript_e_struct*
_tmp169=(struct Cyc_Absyn_Subscript_e_struct*)_tmp158;if(_tmp169->tag != 24)goto
_LLCF;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}_LLCE:{void*_tmp178=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16A->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp17A;struct Cyc_Absyn_Tqual _tmp17B;struct Cyc_Absyn_ArrayInfo _tmp17D;struct Cyc_Absyn_Tqual
_tmp17E;struct Cyc_List_List*_tmp180;_LLDC: {struct Cyc_Absyn_PointerType_struct*
_tmp179=(struct Cyc_Absyn_PointerType_struct*)_tmp178;if(_tmp179->tag != 5)goto
_LLDE;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp17A.elt_tq;}}_LLDD: _tmp17E=_tmp17B;
goto _LLDF;_LLDE: {struct Cyc_Absyn_ArrayType_struct*_tmp17C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp178;if(_tmp17C->tag != 9)goto _LLE0;else{_tmp17D=_tmp17C->f1;_tmp17E=_tmp17D.tq;}}
_LLDF: if(!_tmp17E.real_const)return;goto _LLDB;_LLE0: {struct Cyc_Absyn_TupleType_struct*
_tmp17F=(struct Cyc_Absyn_TupleType_struct*)_tmp178;if(_tmp17F->tag != 11)goto
_LLE2;else{_tmp180=_tmp17F->f1;}}_LLE1: {unsigned int _tmp182;int _tmp183;struct
_tuple13 _tmp181=Cyc_Evexp_eval_const_uint_exp(_tmp16B);_tmp182=_tmp181.f1;
_tmp183=_tmp181.f2;if(!_tmp183){({void*_tmp184=0;Cyc_Tcutil_terr(e->loc,({const
char*_tmp185="tuple projection cannot use sizeof or offsetof";_tag_dyneither(
_tmp185,sizeof(char),47);}),_tag_dyneither(_tmp184,sizeof(void*),0));});return;}{
struct _handler_cons _tmp186;_push_handler(& _tmp186);{int _tmp188=0;if(setjmp(
_tmp186.handler))_tmp188=1;if(!_tmp188){{struct _tuple17 _tmp18A;struct Cyc_Absyn_Tqual
_tmp18B;struct _tuple17*_tmp189=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(_tmp180,(int)_tmp182);_tmp18A=*_tmp189;_tmp18B=_tmp18A.f1;if(!
_tmp18B.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmp187=(
void*)_exn_thrown;void*_tmp18D=_tmp187;_LLE5: {struct Cyc_List_Nth_struct*_tmp18E=(
struct Cyc_List_Nth_struct*)_tmp18D;if(_tmp18E->tag != Cyc_List_Nth)goto _LLE7;}
_LLE6: return;_LLE7:;_LLE8:(void)_throw(_tmp18D);_LLE4:;}};}goto _LLDB;}_LLE2:;
_LLE3: goto _LLDB;_LLDB:;}goto _LLC4;_LLCF: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp16C=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp158;if(_tmp16C->tag != 22)goto
_LLD1;else{_tmp16D=_tmp16C->f1;_tmp16E=_tmp16C->f2;}}_LLD0:{void*_tmp18F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16D->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp191;union Cyc_Absyn_AggrInfoU _tmp192;struct Cyc_Absyn_Aggrdecl**_tmp193;struct
Cyc_List_List*_tmp195;_LLEA: {struct Cyc_Absyn_AggrType_struct*_tmp190=(struct Cyc_Absyn_AggrType_struct*)
_tmp18F;if(_tmp190->tag != 12)goto _LLEC;else{_tmp191=_tmp190->f1;_tmp192=_tmp191.aggr_info;
if((_tmp192.KnownAggr).tag != 2)goto _LLEC;_tmp193=(struct Cyc_Absyn_Aggrdecl**)(
_tmp192.KnownAggr).val;}}_LLEB: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp193 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp193,_tmp16E);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLE9;}_LLEC: {struct Cyc_Absyn_AnonAggrType_struct*_tmp194=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp18F;if(_tmp194->tag != 13)goto _LLEE;else{_tmp195=_tmp194->f2;}}_LLED: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp195,_tmp16E);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLE9;}_LLEE:;_LLEF: goto _LLE9;_LLE9:;}goto _LLC4;_LLD1: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp16F=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp158;if(_tmp16F->tag != 23)goto
_LLD3;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;}}_LLD2:{void*_tmp196=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp170->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp198;void*_tmp199;_LLF1: {struct Cyc_Absyn_PointerType_struct*_tmp197=(struct
Cyc_Absyn_PointerType_struct*)_tmp196;if(_tmp197->tag != 5)goto _LLF3;else{_tmp198=
_tmp197->f1;_tmp199=_tmp198.elt_typ;}}_LLF2:{void*_tmp19A=Cyc_Tcutil_compress(
_tmp199);struct Cyc_Absyn_AggrInfo _tmp19C;union Cyc_Absyn_AggrInfoU _tmp19D;struct
Cyc_Absyn_Aggrdecl**_tmp19E;struct Cyc_List_List*_tmp1A0;_LLF6: {struct Cyc_Absyn_AggrType_struct*
_tmp19B=(struct Cyc_Absyn_AggrType_struct*)_tmp19A;if(_tmp19B->tag != 12)goto _LLF8;
else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19C.aggr_info;if((_tmp19D.KnownAggr).tag != 
2)goto _LLF8;_tmp19E=(struct Cyc_Absyn_Aggrdecl**)(_tmp19D.KnownAggr).val;}}_LLF7: {
struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)_tmp19E == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp19E,_tmp171);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF5;}_LLF8: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp19F=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp19A;if(_tmp19F->tag != 13)goto _LLFA;else{_tmp1A0=_tmp19F->f2;}}_LLF9: {struct
Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1A0,_tmp171);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF5;}_LLFA:;_LLFB: goto _LLF5;_LLF5:;}goto _LLF0;_LLF3:;_LLF4: goto
_LLF0;_LLF0:;}goto _LLC4;_LLD3: {struct Cyc_Absyn_Deref_e_struct*_tmp172=(struct
Cyc_Absyn_Deref_e_struct*)_tmp158;if(_tmp172->tag != 21)goto _LLD5;else{_tmp173=
_tmp172->f1;}}_LLD4:{void*_tmp1A1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp173->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1A3;struct Cyc_Absyn_Tqual
_tmp1A4;struct Cyc_Absyn_ArrayInfo _tmp1A6;struct Cyc_Absyn_Tqual _tmp1A7;_LLFD: {
struct Cyc_Absyn_PointerType_struct*_tmp1A2=(struct Cyc_Absyn_PointerType_struct*)
_tmp1A1;if(_tmp1A2->tag != 5)goto _LLFF;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A3.elt_tq;}}
_LLFE: _tmp1A7=_tmp1A4;goto _LL100;_LLFF: {struct Cyc_Absyn_ArrayType_struct*
_tmp1A5=(struct Cyc_Absyn_ArrayType_struct*)_tmp1A1;if(_tmp1A5->tag != 9)goto
_LL101;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A6.tq;}}_LL100: if(!_tmp1A7.real_const)
return;goto _LLFC;_LL101:;_LL102: goto _LLFC;_LLFC:;}goto _LLC4;_LLD5: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp174=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp158;if(_tmp174->tag != 13)
goto _LLD7;else{_tmp175=_tmp174->f1;}}_LLD6: _tmp177=_tmp175;goto _LLD8;_LLD7: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp176=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp158;if(_tmp176->tag != 14)goto _LLD9;else{_tmp177=_tmp176->f1;}}_LLD8: Cyc_Tcexp_check_writable(
te,_tmp177);return;_LLD9:;_LLDA: goto _LLC4;_LLC4:;}({struct Cyc_String_pa_struct
_tmp1AA;_tmp1AA.tag=0;_tmp1AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));({void*_tmp1A8[1]={& _tmp1AA};Cyc_Tcutil_terr(e->loc,({
const char*_tmp1A9="attempt to write a const location: %s";_tag_dyneither(_tmp1A9,
sizeof(char),38);}),_tag_dyneither(_tmp1A8,sizeof(void*),1));});});}static void*
Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){Cyc_Tcexp_tcExpNoPromote(
te,0,e);if(!Cyc_Absyn_is_lvalue(e))return({void*_tmp1AB=0;Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp1AC="increment/decrement of non-lvalue";_tag_dyneither(
_tmp1AC,sizeof(char),34);}),_tag_dyneither(_tmp1AB,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(!Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc
 || i == Cyc_Absyn_PostInc)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),
Cyc_Absyn_MemKind))({void*_tmp1AD=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp1AE="can't perform arithmetic on abstract pointer type";
_tag_dyneither(_tmp1AE,sizeof(char),50);}),_tag_dyneither(_tmp1AD,sizeof(void*),
0));});if(Cyc_Tcutil_is_noalias_pointer(t))({void*_tmp1AF=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp1B0="can't perform arithmetic on non-aliasing pointer type";
_tag_dyneither(_tmp1B0,sizeof(char),54);}),_tag_dyneither(_tmp1AF,sizeof(void*),
0));});}else{({struct Cyc_String_pa_struct _tmp1B3;_tmp1B3.tag=0;_tmp1B3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*
_tmp1B1[1]={& _tmp1B3};Cyc_Tcutil_terr(e->loc,({const char*_tmp1B2="expecting arithmetic or ? type but found %s";
_tag_dyneither(_tmp1B2,sizeof(char),44);}),_tag_dyneither(_tmp1B1,sizeof(void*),
1));});});}}return t;};}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3){struct _tuple0*_tmp1B6;int _tmp1B7;struct Cyc_Tcexp_TestEnv
_tmp1B5=Cyc_Tcexp_tcTest(te,e1,({const char*_tmp1B4="conditional expression";
_tag_dyneither(_tmp1B4,sizeof(char),23);}));_tmp1B6=_tmp1B5.eq;_tmp1B7=_tmp1B5.isTrue;
Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_List_List
_tmp1B8=({struct Cyc_List_List _tmp1BF;_tmp1BF.hd=e3;_tmp1BF.tl=0;_tmp1BF;});
struct Cyc_List_List _tmp1B9=({struct Cyc_List_List _tmp1BE;_tmp1BE.hd=e2;_tmp1BE.tl=(
struct Cyc_List_List*)& _tmp1B8;_tmp1BE;});if(!Cyc_Tcutil_coerce_list(te,t,(struct
Cyc_List_List*)& _tmp1B9)){({struct Cyc_String_pa_struct _tmp1BD;_tmp1BD.tag=0;
_tmp1BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v));({struct Cyc_String_pa_struct
_tmp1BC;_tmp1BC.tag=0;_tmp1BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v));({
void*_tmp1BA[2]={& _tmp1BC,& _tmp1BD};Cyc_Tcutil_terr(loc,({const char*_tmp1BB="conditional clause types do not match: %s != %s";
_tag_dyneither(_tmp1BB,sizeof(char),48);}),_tag_dyneither(_tmp1BA,sizeof(void*),
2));});});});Cyc_Tcutil_explain_failure();}return t;};}static void*Cyc_Tcexp_tcAnd(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcTest(te,e1,({const char*_tmp1C0="logical-and expression";
_tag_dyneither(_tmp1C0,sizeof(char),23);}));Cyc_Tcexp_tcTest(te,e2,({const char*
_tmp1C1="logical-and expression";_tag_dyneither(_tmp1C1,sizeof(char),23);}));
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
Cyc_Tcexp_tcTest(te,e1,({const char*_tmp1C2="logical-or expression";
_tag_dyneither(_tmp1C2,sizeof(char),22);}));Cyc_Tcexp_tcTest(te,e2,({const char*
_tmp1C3="logical-or expression";_tag_dyneither(_tmp1C3,sizeof(char),22);}));
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp1C4=Cyc_Tcenv_get_fnrgn(te);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(_tmp1C4,te),0,e1);Cyc_Tcexp_tcExp(
te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),
e2);{void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{void*_tmp1C5=Cyc_Tcutil_compress(
t1);_LL104: {struct Cyc_Absyn_ArrayType_struct*_tmp1C6=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1C5;if(_tmp1C6->tag != 9)goto _LL106;}_LL105:({void*_tmp1C7=0;Cyc_Tcutil_terr(
loc,({const char*_tmp1C8="cannot assign to an array";_tag_dyneither(_tmp1C8,
sizeof(char),26);}),_tag_dyneither(_tmp1C7,sizeof(void*),0));});goto _LL103;
_LL106:;_LL107: goto _LL103;_LL103:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t1),Cyc_Absyn_MemKind))({void*_tmp1C9=0;Cyc_Tcutil_terr(loc,({const char*_tmp1CA="type is abstract (can't determine size).";
_tag_dyneither(_tmp1CA,sizeof(char),41);}),_tag_dyneither(_tmp1C9,sizeof(void*),
0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp1CB=0;Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp1CC="assignment to non-lvalue";_tag_dyneither(_tmp1CC,
sizeof(char),25);}),_tag_dyneither(_tmp1CB,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);if(po == 0){struct _RegionHandle*_tmp1CD=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1CD,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1CD,e2))({void*_tmp1CE=0;Cyc_Tcutil_terr(
e2->loc,({const char*_tmp1CF="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp1CF,sizeof(char),49);}),_tag_dyneither(_tmp1CE,sizeof(void*),
0));});if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp1D0=({struct Cyc_String_pa_struct
_tmp1D4;_tmp1D4.tag=0;_tmp1D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_struct _tmp1D3;_tmp1D3.tag=0;
_tmp1D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));({void*_tmp1D1[2]={& _tmp1D3,& _tmp1D4};Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp1D2="type mismatch: %s != %s";_tag_dyneither(_tmp1D2,sizeof(char),
24);}),_tag_dyneither(_tmp1D1,sizeof(void*),2));});});});Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();return _tmp1D0;}}else{enum Cyc_Absyn_Primop _tmp1D5=(
enum Cyc_Absyn_Primop)po->v;void*_tmp1D6=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp1D5,
e1,e2);if(!(Cyc_Tcutil_unify(_tmp1D6,t1) || Cyc_Tcutil_coerceable(_tmp1D6) && 
Cyc_Tcutil_coerceable(t1))){void*_tmp1D7=({struct Cyc_String_pa_struct _tmp1DB;
_tmp1DB.tag=0;_tmp1DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2));({struct Cyc_String_pa_struct _tmp1DA;_tmp1DA.tag=0;_tmp1DA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp1D8[
2]={& _tmp1DA,& _tmp1DB};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp1D9="Cannot use this operator in an assignment when the arguments have types %s and %s";
_tag_dyneither(_tmp1D9,sizeof(char),82);}),_tag_dyneither(_tmp1D8,sizeof(void*),
2));});});});Cyc_Tcutil_unify(_tmp1D6,t1);Cyc_Tcutil_explain_failure();return
_tmp1D7;}return _tmp1D6;}return t1;};}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct
Cyc_Core_Opt*)_check_null(e2->topt))->v;}static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={8,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp1DC=Cyc_Tcutil_compress(
t1);enum Cyc_Absyn_Size_of _tmp1DF;enum Cyc_Absyn_Size_of _tmp1E1;_LL109: {struct
Cyc_Absyn_FloatType_struct*_tmp1DD=(struct Cyc_Absyn_FloatType_struct*)_tmp1DC;
if(_tmp1DD->tag != 7)goto _LL10B;}_LL10A: Cyc_Tcutil_unchecked_cast(te,e,dbl_typ,
Cyc_Absyn_No_coercion);t1=dbl_typ;goto _LL108;_LL10B: {struct Cyc_Absyn_IntType_struct*
_tmp1DE=(struct Cyc_Absyn_IntType_struct*)_tmp1DC;if(_tmp1DE->tag != 6)goto _LL10D;
else{_tmp1DF=_tmp1DE->f2;if(_tmp1DF != Cyc_Absyn_Char_sz)goto _LL10D;}}_LL10C: goto
_LL10E;_LL10D: {struct Cyc_Absyn_IntType_struct*_tmp1E0=(struct Cyc_Absyn_IntType_struct*)
_tmp1DC;if(_tmp1E0->tag != 6)goto _LL10F;else{_tmp1E1=_tmp1E0->f2;if(_tmp1E1 != Cyc_Absyn_Short_sz)
goto _LL10F;}}_LL10E: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
t1=Cyc_Absyn_sint_typ;goto _LL108;_LL10F:;_LL110: goto _LL108;_LL108:;}for(fields=
fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp1E3;struct Cyc_List_List*
_tmp1E4;struct Cyc_Position_Segment*_tmp1E5;enum Cyc_Absyn_Scope _tmp1E6;struct Cyc_Absyn_Datatypefield
_tmp1E2=*((struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1E3=_tmp1E2.name;
_tmp1E4=_tmp1E2.typs;_tmp1E5=_tmp1E2.loc;_tmp1E6=_tmp1E2.sc;if(_tmp1E4 == 0  || 
_tmp1E4->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple17*)_tmp1E4->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Datatypefield*)((
struct Cyc_Absyn_Datatypefield*)fields->hd);};}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp1E8;struct Cyc_List_List*_tmp1E9;struct Cyc_Position_Segment*
_tmp1EA;enum Cyc_Absyn_Scope _tmp1EB;struct Cyc_Absyn_Datatypefield _tmp1E7=*((
struct Cyc_Absyn_Datatypefield*)fields->hd);_tmp1E8=_tmp1E7.name;_tmp1E9=_tmp1E7.typs;
_tmp1EA=_tmp1E7.loc;_tmp1EB=_tmp1E7.sc;if(_tmp1E9 == 0  || _tmp1E9->tl != 0)
continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp1E9->hd)).f2);
if(Cyc_Tcutil_coerce_arg(te,e,t2))return(struct Cyc_Absyn_Datatypefield*)((struct
Cyc_Absyn_Datatypefield*)fields->hd);};}({struct Cyc_String_pa_struct _tmp1EF;
_tmp1EF.tag=0;_tmp1EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1));({struct Cyc_String_pa_struct _tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));({void*_tmp1EC[
2]={& _tmp1EE,& _tmp1EF};Cyc_Tcutil_terr(e->loc,({const char*_tmp1ED="can't find a field in %s to inject a value of type %s";
_tag_dyneither(_tmp1ED,sizeof(char),54);}),_tag_dyneither(_tmp1EC,sizeof(void*),
2));});});});return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp1F1=args;struct _RegionHandle*_tmp1F2=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp1F3=Cyc_Tcenv_new_block(_tmp1F2,loc,te_orig);Cyc_Tcexp_tcExp(_tmp1F3,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp1F4=t;struct Cyc_Absyn_PtrInfo _tmp1F6;void*_tmp1F7;struct Cyc_Absyn_Tqual
_tmp1F8;struct Cyc_Absyn_PtrAtts _tmp1F9;void*_tmp1FA;union Cyc_Absyn_Constraint*
_tmp1FB;union Cyc_Absyn_Constraint*_tmp1FC;union Cyc_Absyn_Constraint*_tmp1FD;
_LL112: {struct Cyc_Absyn_PointerType_struct*_tmp1F5=(struct Cyc_Absyn_PointerType_struct*)
_tmp1F4;if(_tmp1F5->tag != 5)goto _LL114;else{_tmp1F6=_tmp1F5->f1;_tmp1F7=_tmp1F6.elt_typ;
_tmp1F8=_tmp1F6.elt_tq;_tmp1F9=_tmp1F6.ptr_atts;_tmp1FA=_tmp1F9.rgn;_tmp1FB=
_tmp1F9.nullable;_tmp1FC=_tmp1F9.bounds;_tmp1FD=_tmp1F9.zero_term;}}_LL113: Cyc_Tcenv_check_rgn_accessible(
_tmp1F3,loc,_tmp1FA);Cyc_Tcutil_check_nonzero_bound(loc,_tmp1FC);{void*_tmp1FE=
Cyc_Tcutil_compress(_tmp1F7);struct Cyc_Absyn_FnInfo _tmp200;struct Cyc_List_List*
_tmp201;struct Cyc_Core_Opt*_tmp202;void*_tmp203;struct Cyc_List_List*_tmp204;int
_tmp205;struct Cyc_Absyn_VarargInfo*_tmp206;struct Cyc_List_List*_tmp207;struct Cyc_List_List*
_tmp208;_LL117: {struct Cyc_Absyn_FnType_struct*_tmp1FF=(struct Cyc_Absyn_FnType_struct*)
_tmp1FE;if(_tmp1FF->tag != 10)goto _LL119;else{_tmp200=_tmp1FF->f1;_tmp201=_tmp200.tvars;
_tmp202=_tmp200.effect;_tmp203=_tmp200.ret_typ;_tmp204=_tmp200.args;_tmp205=
_tmp200.c_varargs;_tmp206=_tmp200.cyc_varargs;_tmp207=_tmp200.rgn_po;_tmp208=
_tmp200.attributes;}}_LL118: if(topt != 0)Cyc_Tcutil_unify(_tmp203,*topt);while(
_tmp1F1 != 0  && _tmp204 != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1F1->hd;
void*t2=(*((struct _tuple8*)_tmp204->hd)).f3;Cyc_Tcexp_tcExp(_tmp1F3,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp1F3,e1,t2)){({struct Cyc_String_pa_struct
_tmp20C;_tmp20C.tag=0;_tmp20C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_struct _tmp20B;_tmp20B.tag=0;
_tmp20B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));({void*_tmp209[2]={&
_tmp20B,& _tmp20C};Cyc_Tcutil_terr(e1->loc,({const char*_tmp20A="actual argument has type \n\t%s\n but formal has type \n\t%s.";
_tag_dyneither(_tmp20A,sizeof(char),57);}),_tag_dyneither(_tmp209,sizeof(void*),
2));});});});Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,
t2);Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1F2,t2) && !Cyc_Tcutil_is_noalias_path(_tmp1F2,e1))({void*_tmp20D=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp20E="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp20E,sizeof(char),49);}),_tag_dyneither(_tmp20D,sizeof(void*),
0));});_tmp1F1=_tmp1F1->tl;_tmp204=_tmp204->tl;}{int args_already_checked=0;{
struct Cyc_List_List*a=_tmp208;for(0;a != 0;a=a->tl){void*_tmp20F=(void*)a->hd;
enum Cyc_Absyn_Format_Type _tmp211;int _tmp212;int _tmp213;_LL11C: {struct Cyc_Absyn_Format_att_struct*
_tmp210=(struct Cyc_Absyn_Format_att_struct*)_tmp20F;if(_tmp210->tag != 19)goto
_LL11E;else{_tmp211=_tmp210->f1;_tmp212=_tmp210->f2;_tmp213=_tmp210->f3;}}_LL11D:{
struct _handler_cons _tmp214;_push_handler(& _tmp214);{int _tmp216=0;if(setjmp(
_tmp214.handler))_tmp216=1;if(!_tmp216){{struct Cyc_Absyn_Exp*_tmp217=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp212 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp213 == 0)fmt_args=0;else{fmt_args=({struct Cyc_Core_Opt*_tmp218=
_cycalloc(sizeof(*_tmp218));_tmp218->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,int i))Cyc_List_nth_tail)(args,_tmp213 - 1);_tmp218;});}args_already_checked=1;{
struct _RegionHandle*_tmp219=Cyc_Tcenv_get_fnrgn(te_orig);switch(_tmp211){case Cyc_Absyn_Printf_ft:
_LL120: Cyc_Tcexp_check_format_args(_tmp1F3,_tmp217,fmt_args,_tmp219,Cyc_Formatstr_get_format_typs);
break;case Cyc_Absyn_Scanf_ft: _LL121: Cyc_Tcexp_check_format_args(_tmp1F3,_tmp217,
fmt_args,_tmp219,Cyc_Formatstr_get_scanf_typs);break;}};};_pop_handler();}else{
void*_tmp215=(void*)_exn_thrown;void*_tmp21B=_tmp215;_LL124: {struct Cyc_List_Nth_struct*
_tmp21C=(struct Cyc_List_Nth_struct*)_tmp21B;if(_tmp21C->tag != Cyc_List_Nth)goto
_LL126;}_LL125:({void*_tmp21D=0;Cyc_Tcutil_terr(loc,({const char*_tmp21E="bad format arguments";
_tag_dyneither(_tmp21E,sizeof(char),21);}),_tag_dyneither(_tmp21D,sizeof(void*),
0));});goto _LL123;_LL126:;_LL127:(void)_throw(_tmp21B);_LL123:;}};}goto _LL11B;
_LL11E:;_LL11F: goto _LL11B;_LL11B:;}}if(_tmp204 != 0)({void*_tmp21F=0;Cyc_Tcutil_terr(
loc,({const char*_tmp220="too few arguments for function";_tag_dyneither(_tmp220,
sizeof(char),31);}),_tag_dyneither(_tmp21F,sizeof(void*),0));});else{if((_tmp1F1
!= 0  || _tmp205) || _tmp206 != 0){if(_tmp205)for(0;_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){
Cyc_Tcexp_tcExp(_tmp1F3,0,(struct Cyc_Absyn_Exp*)_tmp1F1->hd);}else{if(_tmp206 == 
0)({void*_tmp221=0;Cyc_Tcutil_terr(loc,({const char*_tmp222="too many arguments for function";
_tag_dyneither(_tmp222,sizeof(char),32);}),_tag_dyneither(_tmp221,sizeof(void*),
0));});else{void*_tmp224;int _tmp225;struct Cyc_Absyn_VarargInfo _tmp223=*_tmp206;
_tmp224=_tmp223.type;_tmp225=_tmp223.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp226=({struct Cyc_Absyn_VarargCallInfo*_tmp244=_cycalloc(sizeof(*_tmp244));
_tmp244->num_varargs=0;_tmp244->injectors=0;_tmp244->vai=(struct Cyc_Absyn_VarargInfo*)
_tmp206;_tmp244;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)_tmp226;
if(!_tmp225)for(0;_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp1F1->hd;++ _tmp226->num_varargs;Cyc_Tcexp_tcExp(_tmp1F3,(
void**)& _tmp224,e1);if(!Cyc_Tcutil_coerce_arg(_tmp1F3,e1,_tmp224)){({struct Cyc_String_pa_struct
_tmp22A;_tmp22A.tag=0;_tmp22A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v));({
struct Cyc_String_pa_struct _tmp229;_tmp229.tag=0;_tmp229.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp224));({void*_tmp227[2]={&
_tmp229,& _tmp22A};Cyc_Tcutil_terr(loc,({const char*_tmp228="vararg requires type %s but argument has type %s";
_tag_dyneither(_tmp228,sizeof(char),49);}),_tag_dyneither(_tmp227,sizeof(void*),
2));});});});Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp1F2,_tmp224) && !Cyc_Tcutil_is_noalias_path(_tmp1F2,e1))({void*_tmp22B=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp22C="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp22C,sizeof(char),49);}),_tag_dyneither(_tmp22B,sizeof(void*),
0));});}else{void*_tmp22D=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp224));struct Cyc_Absyn_DatatypeInfo _tmp22F;union Cyc_Absyn_DatatypeInfoU
_tmp230;struct Cyc_Absyn_Datatypedecl**_tmp231;struct Cyc_Absyn_Datatypedecl*
_tmp232;struct Cyc_List_List*_tmp233;_LL129: {struct Cyc_Absyn_DatatypeType_struct*
_tmp22E=(struct Cyc_Absyn_DatatypeType_struct*)_tmp22D;if(_tmp22E->tag != 3)goto
_LL12B;else{_tmp22F=_tmp22E->f1;_tmp230=_tmp22F.datatype_info;if((_tmp230.KnownDatatype).tag
!= 2)goto _LL12B;_tmp231=(struct Cyc_Absyn_Datatypedecl**)(_tmp230.KnownDatatype).val;
_tmp232=*_tmp231;_tmp233=_tmp22F.targs;}}_LL12A: {struct Cyc_Absyn_Datatypedecl*
_tmp234=*Cyc_Tcenv_lookup_datatypedecl(_tmp1F3,loc,_tmp232->name);struct Cyc_List_List*
fields=0;if(_tmp234->fields == 0)({struct Cyc_String_pa_struct _tmp237;_tmp237.tag=
0;_tmp237.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp224));({void*_tmp235[1]={& _tmp237};Cyc_Tcutil_terr(loc,({const char*_tmp236="can't inject into abstract datatype %s";
_tag_dyneither(_tmp236,sizeof(char),39);}),_tag_dyneither(_tmp235,sizeof(void*),
1));});});else{fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp234->fields))->v;}if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp224),
Cyc_Tcenv_curr_rgn(_tmp1F3)))({struct Cyc_String_pa_struct _tmp23B;_tmp23B.tag=0;
_tmp23B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcenv_curr_rgn(_tmp1F3)));({struct Cyc_String_pa_struct _tmp23A;_tmp23A.tag=0;
_tmp23A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp224));({void*_tmp238[2]={& _tmp23A,& _tmp23B};Cyc_Tcutil_terr(loc,({const char*
_tmp239="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(
_tmp239,sizeof(char),49);}),_tag_dyneither(_tmp238,sizeof(void*),2));});});});{
struct _RegionHandle*_tmp23C=Cyc_Tcenv_get_fnrgn(_tmp1F3);{struct Cyc_List_List*
_tmp23D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp23C,_tmp23C,
_tmp234->tvs,_tmp233);for(0;_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){++ _tmp226->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp1F1->hd;if(!
args_already_checked){Cyc_Tcexp_tcExp(_tmp1F3,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp23C,(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
_tmp23C,e1))({void*_tmp23E=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp23F="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp23F,sizeof(char),49);}),_tag_dyneither(_tmp23E,sizeof(void*),
0));});}{struct Cyc_Absyn_Datatypefield*_tmp240=Cyc_Tcexp_tcInjection(_tmp1F3,e1,
Cyc_Tcutil_pointer_elt_type(_tmp224),_tmp23C,_tmp23D,fields);if(_tmp240 != 0)
_tmp226->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp226->injectors,({struct Cyc_List_List*_tmp241=_cycalloc(
sizeof(*_tmp241));_tmp241->hd=(struct Cyc_Absyn_Datatypefield*)_tmp240;_tmp241->tl=
0;_tmp241;}));};};}}goto _LL128;};}_LL12B:;_LL12C:({void*_tmp242=0;Cyc_Tcutil_terr(
loc,({const char*_tmp243="bad inject vararg type";_tag_dyneither(_tmp243,sizeof(
char),23);}),_tag_dyneither(_tmp242,sizeof(void*),0));});goto _LL128;_LL128:;}};}}}}
Cyc_Tcenv_check_effect_accessible(_tmp1F3,loc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp202))->v);Cyc_Tcenv_check_rgn_partial_order(_tmp1F3,loc,_tmp207);
return _tmp203;};_LL119:;_LL11A: return({void*_tmp245=0;Cyc_Tcexp_expr_err(_tmp1F3,
loc,topt,({const char*_tmp246="expected pointer to function";_tag_dyneither(
_tmp246,sizeof(char),29);}),_tag_dyneither(_tmp245,sizeof(void*),0));});_LL116:;};
_LL114:;_LL115: return({void*_tmp247=0;Cyc_Tcexp_expr_err(_tmp1F3,loc,topt,({
const char*_tmp248="expected pointer to function";_tag_dyneither(_tmp248,sizeof(
char),29);}),_tag_dyneither(_tmp247,sizeof(void*),0));});_LL111:;};}static void*
Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e){void*_tmp249=Cyc_Absyn_exn_typ();Cyc_Tcexp_tcExp(te,(
void**)& _tmp249,e);if(!Cyc_Tcutil_coerce_arg(te,e,_tmp249))({struct Cyc_String_pa_struct
_tmp24D;_tmp24D.tag=0;_tmp24D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));({
struct Cyc_String_pa_struct _tmp24C;_tmp24C.tag=0;_tmp24C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp249));({void*_tmp24A[2]={&
_tmp24C,& _tmp24D};Cyc_Tcutil_terr(loc,({const char*_tmp24B="expected %s but found %s";
_tag_dyneither(_tmp24B,sizeof(char),25);}),_tag_dyneither(_tmp24A,sizeof(void*),
2));});});});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static
void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){Cyc_Tcexp_tcExpNoInst(
te,0,e);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp24E=t1;struct Cyc_Absyn_PtrInfo
_tmp250;void*_tmp251;struct Cyc_Absyn_Tqual _tmp252;struct Cyc_Absyn_PtrAtts _tmp253;
void*_tmp254;union Cyc_Absyn_Constraint*_tmp255;union Cyc_Absyn_Constraint*_tmp256;
union Cyc_Absyn_Constraint*_tmp257;_LL12E: {struct Cyc_Absyn_PointerType_struct*
_tmp24F=(struct Cyc_Absyn_PointerType_struct*)_tmp24E;if(_tmp24F->tag != 5)goto
_LL130;else{_tmp250=_tmp24F->f1;_tmp251=_tmp250.elt_typ;_tmp252=_tmp250.elt_tq;
_tmp253=_tmp250.ptr_atts;_tmp254=_tmp253.rgn;_tmp255=_tmp253.nullable;_tmp256=
_tmp253.bounds;_tmp257=_tmp253.zero_term;}}_LL12F:{void*_tmp258=Cyc_Tcutil_compress(
_tmp251);struct Cyc_Absyn_FnInfo _tmp25A;struct Cyc_List_List*_tmp25B;struct Cyc_Core_Opt*
_tmp25C;void*_tmp25D;struct Cyc_List_List*_tmp25E;int _tmp25F;struct Cyc_Absyn_VarargInfo*
_tmp260;struct Cyc_List_List*_tmp261;struct Cyc_List_List*_tmp262;_LL133: {struct
Cyc_Absyn_FnType_struct*_tmp259=(struct Cyc_Absyn_FnType_struct*)_tmp258;if(
_tmp259->tag != 10)goto _LL135;else{_tmp25A=_tmp259->f1;_tmp25B=_tmp25A.tvars;
_tmp25C=_tmp25A.effect;_tmp25D=_tmp25A.ret_typ;_tmp25E=_tmp25A.args;_tmp25F=
_tmp25A.c_varargs;_tmp260=_tmp25A.cyc_varargs;_tmp261=_tmp25A.rgn_po;_tmp262=
_tmp25A.attributes;}}_LL134: {struct _RegionHandle*_tmp263=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*instantiation=0;for(0;ts != 0  && _tmp25B != 0;(ts=ts->tl,
_tmp25B=_tmp25B->tl)){enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp25B->hd,Cyc_Absyn_BoxKind);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),k,1,(void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp264=
_region_malloc(_tmp263,sizeof(*_tmp264));_tmp264->hd=({struct _tuple11*_tmp265=
_region_malloc(_tmp263,sizeof(*_tmp265));_tmp265->f1=(struct Cyc_Absyn_Tvar*)
_tmp25B->hd;_tmp265->f2=(void*)ts->hd;_tmp265;});_tmp264->tl=instantiation;
_tmp264;});}if(ts != 0)return({void*_tmp266=0;Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp267="too many type variables in instantiation";_tag_dyneither(
_tmp267,sizeof(char),41);}),_tag_dyneither(_tmp266,sizeof(void*),0));});{void*
new_fn_typ=Cyc_Tcutil_rsubstitute(_tmp263,instantiation,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C[0]=({struct Cyc_Absyn_FnType_struct
_tmp26D;_tmp26D.tag=10;_tmp26D.f1=({struct Cyc_Absyn_FnInfo _tmp26E;_tmp26E.tvars=
_tmp25B;_tmp26E.effect=_tmp25C;_tmp26E.ret_typ=_tmp25D;_tmp26E.args=_tmp25E;
_tmp26E.c_varargs=_tmp25F;_tmp26E.cyc_varargs=_tmp260;_tmp26E.rgn_po=_tmp261;
_tmp26E.attributes=_tmp262;_tmp26E;});_tmp26D;});_tmp26C;}));return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268[0]=({
struct Cyc_Absyn_PointerType_struct _tmp269;_tmp269.tag=5;_tmp269.f1=({struct Cyc_Absyn_PtrInfo
_tmp26A;_tmp26A.elt_typ=new_fn_typ;_tmp26A.elt_tq=_tmp252;_tmp26A.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp26B;_tmp26B.rgn=_tmp254;_tmp26B.nullable=_tmp255;
_tmp26B.bounds=_tmp256;_tmp26B.zero_term=_tmp257;_tmp26B.ptrloc=0;_tmp26B;});
_tmp26A;});_tmp269;});_tmp268;});};}_LL135:;_LL136: goto _LL132;_LL132:;}goto
_LL12D;_LL130:;_LL131: goto _LL12D;_LL12D:;}return({struct Cyc_String_pa_struct
_tmp271;_tmp271.tag=0;_tmp271.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));({void*_tmp26F[1]={& _tmp271};Cyc_Tcexp_expr_err(te,
loc,topt,({const char*_tmp270="expecting polymorphic type but found %s";
_tag_dyneither(_tmp270,sizeof(char),40);}),_tag_dyneither(_tmp26F,sizeof(void*),
1));});});};}static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),Cyc_Absyn_MemKind,1,t);Cyc_Tcexp_tcExp(te,(
void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(
Cyc_Tcutil_silent_castable(te,loc,t2,t))*((enum Cyc_Absyn_Coercion*)_check_null(
c))=Cyc_Absyn_No_coercion;else{enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,
loc,t2,t);if(crc != Cyc_Absyn_Unknown_coercion)*((enum Cyc_Absyn_Coercion*)
_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((enum Cyc_Absyn_Coercion*)
_check_null(c))=Cyc_Absyn_No_coercion;else{Cyc_Tcutil_unify(t2,t);{void*_tmp272=({
struct Cyc_String_pa_struct _tmp276;_tmp276.tag=0;_tmp276.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_struct
_tmp275;_tmp275.tag=0;_tmp275.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({void*_tmp273[2]={& _tmp275,& _tmp276};Cyc_Tcexp_expr_err(
te,loc,(void**)& t,({const char*_tmp274="cannot cast %s to %s";_tag_dyneither(
_tmp274,sizeof(char),21);}),_tag_dyneither(_tmp273,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();return _tmp272;};}}}{struct _tuple0 _tmp278=({struct
_tuple0 _tmp277;_tmp277.f1=e->r;_tmp277.f2=Cyc_Tcutil_compress(t);_tmp277;});void*
_tmp279;struct Cyc_Absyn_MallocInfo _tmp27B;int _tmp27C;void*_tmp27D;struct Cyc_Absyn_PtrInfo
_tmp27F;struct Cyc_Absyn_PtrAtts _tmp280;union Cyc_Absyn_Constraint*_tmp281;union
Cyc_Absyn_Constraint*_tmp282;union Cyc_Absyn_Constraint*_tmp283;_LL138: _tmp279=
_tmp278.f1;{struct Cyc_Absyn_Malloc_e_struct*_tmp27A=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp279;if(_tmp27A->tag != 34)goto _LL13A;else{_tmp27B=_tmp27A->f1;_tmp27C=_tmp27B.fat_result;}};
_tmp27D=_tmp278.f2;{struct Cyc_Absyn_PointerType_struct*_tmp27E=(struct Cyc_Absyn_PointerType_struct*)
_tmp27D;if(_tmp27E->tag != 5)goto _LL13A;else{_tmp27F=_tmp27E->f1;_tmp280=_tmp27F.ptr_atts;
_tmp281=_tmp280.nullable;_tmp282=_tmp280.bounds;_tmp283=_tmp280.zero_term;}};
_LL139: if((_tmp27C  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp283)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp281)){void*_tmp284=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp282);struct Cyc_Absyn_Exp*_tmp286;_LL13D: {struct Cyc_Absyn_Upper_b_struct*
_tmp285=(struct Cyc_Absyn_Upper_b_struct*)_tmp284;if(_tmp285->tag != 1)goto _LL13F;
else{_tmp286=_tmp285->f1;}}_LL13E: if((Cyc_Evexp_eval_const_uint_exp(_tmp286)).f1
== 1)({void*_tmp287=0;Cyc_Tcutil_warn(loc,({const char*_tmp288="cast from ? pointer to * pointer will lose size information";
_tag_dyneither(_tmp288,sizeof(char),60);}),_tag_dyneither(_tmp287,sizeof(void*),
0));});goto _LL13C;_LL13F:;_LL140: goto _LL13C;_LL13C:;}goto _LL137;_LL13A:;_LL13B:
goto _LL137;_LL137:;}return t;};}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp289=0;struct Cyc_Absyn_Tqual _tmp28A=Cyc_Absyn_empty_tqual(0);if(topt
!= 0){void*_tmp28B=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp28D;
void*_tmp28E;struct Cyc_Absyn_Tqual _tmp28F;struct Cyc_Absyn_PtrAtts _tmp290;union
Cyc_Absyn_Constraint*_tmp291;_LL142: {struct Cyc_Absyn_PointerType_struct*_tmp28C=(
struct Cyc_Absyn_PointerType_struct*)_tmp28B;if(_tmp28C->tag != 5)goto _LL144;else{
_tmp28D=_tmp28C->f1;_tmp28E=_tmp28D.elt_typ;_tmp28F=_tmp28D.elt_tq;_tmp290=
_tmp28D.ptr_atts;_tmp291=_tmp290.zero_term;}}_LL143: _tmp289=({void**_tmp292=
_cycalloc(sizeof(*_tmp292));_tmp292[0]=_tmp28E;_tmp292;});_tmp28A=_tmp28F;goto
_LL141;_LL144:;_LL145: goto _LL141;_LL141:;}{struct _RegionHandle*_tmp293=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(_tmp293,te),_tmp289,e);if(
Cyc_Tcutil_is_noalias_path(_tmp293,e))({void*_tmp294=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp295="Cannot take the address of an alias-free path";_tag_dyneither(
_tmp295,sizeof(char),46);}),_tag_dyneither(_tmp294,sizeof(void*),0));});{void*
_tmp296=e->r;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp299;_LL147: {
struct Cyc_Absyn_Subscript_e_struct*_tmp297=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp296;if(_tmp297->tag != 24)goto _LL149;else{_tmp298=_tmp297->f1;_tmp299=_tmp297->f2;}}
_LL148:{void*_tmp29A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp298->topt))->v);_LL14C: {struct Cyc_Absyn_TupleType_struct*
_tmp29B=(struct Cyc_Absyn_TupleType_struct*)_tmp29A;if(_tmp29B->tag != 11)goto
_LL14E;}_LL14D: goto _LL14B;_LL14E:;_LL14F: e0->r=(Cyc_Absyn_add_exp(_tmp298,
_tmp299,0))->r;return Cyc_Tcexp_tcPlus(te,_tmp298,_tmp299);_LL14B:;}goto _LL146;
_LL149:;_LL14A: goto _LL146;_LL146:;}{void*_tmp29C=e->r;int _tmp29E;int _tmp2A0;
_LL151: {struct Cyc_Absyn_AggrMember_e_struct*_tmp29D=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp29C;if(_tmp29D->tag != 22)goto _LL153;else{_tmp29E=_tmp29D->f3;if(_tmp29E != 1)
goto _LL153;}}_LL152: goto _LL154;_LL153: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp29F=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp29C;if(_tmp29F->tag != 23)goto
_LL155;else{_tmp2A0=_tmp29F->f3;if(_tmp2A0 != 1)goto _LL155;}}_LL154:({void*
_tmp2A1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp2A2="cannot take the address of a @tagged union member";
_tag_dyneither(_tmp2A2,sizeof(char),50);}),_tag_dyneither(_tmp2A1,sizeof(void*),
0));});goto _LL150;_LL155:;_LL156: goto _LL150;_LL150:;}{int _tmp2A4;void*_tmp2A5;
struct _tuple12 _tmp2A3=Cyc_Tcutil_addressof_props(te,e);_tmp2A4=_tmp2A3.f1;
_tmp2A5=_tmp2A3.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp2A4){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp2A5,tq,Cyc_Absyn_false_conref);return t;};};};};}
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),Cyc_Absyn_MemKind,1,t);if(!Cyc_Evexp_okay_szofarg(
t))({struct Cyc_String_pa_struct _tmp2A8;_tmp2A8.tag=0;_tmp2A8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp2A6[
1]={& _tmp2A8};Cyc_Tcutil_terr(loc,({const char*_tmp2A7="sizeof applied to type %s, which has unknown size here";
_tag_dyneither(_tmp2A7,sizeof(char),55);}),_tag_dyneither(_tmp2A6,sizeof(void*),
1));});});if(topt != 0){void*_tmp2A9=Cyc_Tcutil_compress(*topt);void*_tmp2AB;
_LL158: {struct Cyc_Absyn_TagType_struct*_tmp2AA=(struct Cyc_Absyn_TagType_struct*)
_tmp2A9;if(_tmp2AA->tag != 20)goto _LL15A;else{_tmp2AB=(void*)_tmp2AA->f1;}}_LL159: {
struct Cyc_Absyn_Exp*_tmp2AC=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct*
_tmp2AD=({struct Cyc_Absyn_ValueofType_struct*_tmp2AE=_cycalloc(sizeof(*_tmp2AE));
_tmp2AE[0]=({struct Cyc_Absyn_ValueofType_struct _tmp2AF;_tmp2AF.tag=19;_tmp2AF.f1=
_tmp2AC;_tmp2AF;});_tmp2AE;});if(Cyc_Tcutil_unify(_tmp2AB,(void*)_tmp2AD))return
_tmp2A9;goto _LL157;}_LL15A:;_LL15B: goto _LL157;_LL157:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),Cyc_Absyn_MemKind,1,t);{void*_tmp2B0=n;struct _dyneither_ptr*_tmp2B2;
unsigned int _tmp2B4;_LL15D: {struct Cyc_Absyn_StructField_struct*_tmp2B1=(struct
Cyc_Absyn_StructField_struct*)_tmp2B0;if(_tmp2B1->tag != 0)goto _LL15F;else{
_tmp2B2=_tmp2B1->f1;}}_LL15E: {int bad_type=1;{void*_tmp2B5=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_AggrInfo _tmp2B7;union Cyc_Absyn_AggrInfoU _tmp2B8;struct Cyc_Absyn_Aggrdecl**
_tmp2B9;struct Cyc_List_List*_tmp2BB;_LL162: {struct Cyc_Absyn_AggrType_struct*
_tmp2B6=(struct Cyc_Absyn_AggrType_struct*)_tmp2B5;if(_tmp2B6->tag != 12)goto
_LL164;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=_tmp2B7.aggr_info;if((_tmp2B8.KnownAggr).tag
!= 2)goto _LL164;_tmp2B9=(struct Cyc_Absyn_Aggrdecl**)(_tmp2B8.KnownAggr).val;}}
_LL163: if((*_tmp2B9)->impl == 0)goto _LL161;if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2B2,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp2B9)->impl))->fields))({struct Cyc_String_pa_struct _tmp2BE;
_tmp2BE.tag=0;_tmp2BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B2);({
void*_tmp2BC[1]={& _tmp2BE};Cyc_Tcutil_terr(loc,({const char*_tmp2BD="no field of struct/union has name %s";
_tag_dyneither(_tmp2BD,sizeof(char),37);}),_tag_dyneither(_tmp2BC,sizeof(void*),
1));});});bad_type=0;goto _LL161;_LL164: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp2BA=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp2B5;if(_tmp2BA->tag != 13)goto
_LL166;else{_tmp2BB=_tmp2BA->f2;}}_LL165: if(!((int(*)(int(*pred)(struct
_dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp2B2,_tmp2BB))({struct
Cyc_String_pa_struct _tmp2C1;_tmp2C1.tag=0;_tmp2C1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp2B2);({void*_tmp2BF[1]={& _tmp2C1};Cyc_Tcutil_terr(loc,({
const char*_tmp2C0="no field of struct/union has name %s";_tag_dyneither(_tmp2C0,
sizeof(char),37);}),_tag_dyneither(_tmp2BF,sizeof(void*),1));});});bad_type=0;
goto _LL161;_LL166:;_LL167: goto _LL161;_LL161:;}if(bad_type)({struct Cyc_String_pa_struct
_tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmp2C2[1]={& _tmp2C4};Cyc_Tcutil_terr(loc,({
const char*_tmp2C3="%s is not a known struct/union type";_tag_dyneither(_tmp2C3,
sizeof(char),36);}),_tag_dyneither(_tmp2C2,sizeof(void*),1));});});goto _LL15C;}
_LL15F: {struct Cyc_Absyn_TupleIndex_struct*_tmp2B3=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp2B0;if(_tmp2B3->tag != 1)goto _LL15C;else{_tmp2B4=_tmp2B3->f1;}}_LL160: {int
bad_type=1;{void*_tmp2C5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp2C7;
union Cyc_Absyn_AggrInfoU _tmp2C8;struct Cyc_Absyn_Aggrdecl**_tmp2C9;struct Cyc_List_List*
_tmp2CB;struct Cyc_List_List*_tmp2CD;struct Cyc_Absyn_DatatypeFieldInfo _tmp2CF;
union Cyc_Absyn_DatatypeFieldInfoU _tmp2D0;struct _tuple2 _tmp2D1;struct Cyc_Absyn_Datatypefield*
_tmp2D2;_LL169: {struct Cyc_Absyn_AggrType_struct*_tmp2C6=(struct Cyc_Absyn_AggrType_struct*)
_tmp2C5;if(_tmp2C6->tag != 12)goto _LL16B;else{_tmp2C7=_tmp2C6->f1;_tmp2C8=_tmp2C7.aggr_info;
if((_tmp2C8.KnownAggr).tag != 2)goto _LL16B;_tmp2C9=(struct Cyc_Absyn_Aggrdecl**)(
_tmp2C8.KnownAggr).val;}}_LL16A: if((*_tmp2C9)->impl == 0)goto _LL168;_tmp2CB=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp2C9)->impl))->fields;goto _LL16C;
_LL16B: {struct Cyc_Absyn_AnonAggrType_struct*_tmp2CA=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp2C5;if(_tmp2CA->tag != 13)goto _LL16D;else{_tmp2CB=_tmp2CA->f2;}}_LL16C: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2CB)<= _tmp2B4)({struct Cyc_Int_pa_struct
_tmp2D6;_tmp2D6.tag=1;_tmp2D6.f1=(unsigned long)((int)_tmp2B4);({struct Cyc_Int_pa_struct
_tmp2D5;_tmp2D5.tag=1;_tmp2D5.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp2CB);({void*_tmp2D3[2]={& _tmp2D5,& _tmp2D6};Cyc_Tcutil_terr(
loc,({const char*_tmp2D4="struct/union has too few components: %d <= %d";
_tag_dyneither(_tmp2D4,sizeof(char),46);}),_tag_dyneither(_tmp2D3,sizeof(void*),
2));});});});bad_type=0;goto _LL168;_LL16D: {struct Cyc_Absyn_TupleType_struct*
_tmp2CC=(struct Cyc_Absyn_TupleType_struct*)_tmp2C5;if(_tmp2CC->tag != 11)goto
_LL16F;else{_tmp2CD=_tmp2CC->f1;}}_LL16E: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2CD)<= _tmp2B4)({struct Cyc_Int_pa_struct _tmp2DA;_tmp2DA.tag=1;_tmp2DA.f1=(
unsigned long)((int)_tmp2B4);({struct Cyc_Int_pa_struct _tmp2D9;_tmp2D9.tag=1;
_tmp2D9.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2CD);({void*_tmp2D7[2]={& _tmp2D9,& _tmp2DA};Cyc_Tcutil_terr(loc,({const char*
_tmp2D8="tuple has too few components: %d <= %d";_tag_dyneither(_tmp2D8,sizeof(
char),39);}),_tag_dyneither(_tmp2D7,sizeof(void*),2));});});});bad_type=0;goto
_LL168;_LL16F: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp2CE=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp2C5;if(_tmp2CE->tag != 4)goto _LL171;else{_tmp2CF=_tmp2CE->f1;_tmp2D0=_tmp2CF.field_info;
if((_tmp2D0.KnownDatatypefield).tag != 2)goto _LL171;_tmp2D1=(struct _tuple2)(
_tmp2D0.KnownDatatypefield).val;_tmp2D2=_tmp2D1.f2;}}_LL170: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp2D2->typs)< _tmp2B4)({struct Cyc_Int_pa_struct
_tmp2DE;_tmp2DE.tag=1;_tmp2DE.f1=(unsigned long)((int)_tmp2B4);({struct Cyc_Int_pa_struct
_tmp2DD;_tmp2DD.tag=1;_tmp2DD.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp2D2->typs);({void*_tmp2DB[2]={& _tmp2DD,& _tmp2DE};Cyc_Tcutil_terr(
loc,({const char*_tmp2DC="datatype field has too few components: %d < %d";
_tag_dyneither(_tmp2DC,sizeof(char),47);}),_tag_dyneither(_tmp2DB,sizeof(void*),
2));});});});bad_type=0;goto _LL168;_LL171:;_LL172: goto _LL168;_LL168:;}if(
bad_type)({struct Cyc_String_pa_struct _tmp2E1;_tmp2E1.tag=0;_tmp2E1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp2DF[
1]={& _tmp2E1};Cyc_Tcutil_terr(loc,({const char*_tmp2E0="%s is not a known type";
_tag_dyneither(_tmp2E0,sizeof(char),23);}),_tag_dyneither(_tmp2DF,sizeof(void*),
1));});});goto _LL15C;}_LL15C:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){struct _RegionHandle*_tmp2E2=Cyc_Tcenv_get_fnrgn(te_orig);struct Cyc_Tcenv_Tenv*
_tmp2E3=Cyc_Tcenv_clear_notreadctxt(_tmp2E2,te_orig);Cyc_Tcexp_tcExp(_tmp2E3,0,e);{
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp2E4=t;struct Cyc_Absyn_PtrInfo _tmp2E7;void*_tmp2E8;struct Cyc_Absyn_PtrAtts
_tmp2E9;void*_tmp2EA;union Cyc_Absyn_Constraint*_tmp2EB;union Cyc_Absyn_Constraint*
_tmp2EC;_LL174: {struct Cyc_Absyn_Evar_struct*_tmp2E5=(struct Cyc_Absyn_Evar_struct*)
_tmp2E4;if(_tmp2E5->tag != 1)goto _LL176;}_LL175: {struct Cyc_List_List*_tmp2ED=Cyc_Tcenv_lookup_type_vars(
_tmp2E3);void*_tmp2EE=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->v=_tmp2ED;
_tmp2F9;}));void*_tmp2EF=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,({
struct Cyc_Core_Opt*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));_tmp2F8->v=_tmp2ED;
_tmp2F8;}));union Cyc_Absyn_Constraint*_tmp2F0=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();union Cyc_Absyn_Constraint*_tmp2F1=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();struct Cyc_Absyn_PtrAtts _tmp2F2=({struct Cyc_Absyn_PtrAtts
_tmp2F7;_tmp2F7.rgn=_tmp2EF;_tmp2F7.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmp2F7.bounds=_tmp2F0;_tmp2F7.zero_term=_tmp2F1;
_tmp2F7.ptrloc=0;_tmp2F7;});struct Cyc_Absyn_PointerType_struct*_tmp2F3=({struct
Cyc_Absyn_PointerType_struct*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4[0]=({
struct Cyc_Absyn_PointerType_struct _tmp2F5;_tmp2F5.tag=5;_tmp2F5.f1=({struct Cyc_Absyn_PtrInfo
_tmp2F6;_tmp2F6.elt_typ=_tmp2EE;_tmp2F6.elt_tq=Cyc_Absyn_empty_tqual(0);_tmp2F6.ptr_atts=
_tmp2F2;_tmp2F6;});_tmp2F5;});_tmp2F4;});Cyc_Tcutil_unify(t,(void*)_tmp2F3);
_tmp2E8=_tmp2EE;_tmp2EA=_tmp2EF;_tmp2EB=_tmp2F0;_tmp2EC=_tmp2F1;goto _LL177;}
_LL176: {struct Cyc_Absyn_PointerType_struct*_tmp2E6=(struct Cyc_Absyn_PointerType_struct*)
_tmp2E4;if(_tmp2E6->tag != 5)goto _LL178;else{_tmp2E7=_tmp2E6->f1;_tmp2E8=_tmp2E7.elt_typ;
_tmp2E9=_tmp2E7.ptr_atts;_tmp2EA=_tmp2E9.rgn;_tmp2EB=_tmp2E9.bounds;_tmp2EC=
_tmp2E9.zero_term;}}_LL177: Cyc_Tcenv_check_rgn_accessible(_tmp2E3,loc,_tmp2EA);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2EB);if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
_tmp2E8),Cyc_Absyn_MemKind))({void*_tmp2FA=0;Cyc_Tcutil_terr(loc,({const char*
_tmp2FB="can't dereference abstract pointer type";_tag_dyneither(_tmp2FB,sizeof(
char),40);}),_tag_dyneither(_tmp2FA,sizeof(void*),0));});return _tmp2E8;_LL178:;
_LL179: return({struct Cyc_String_pa_struct _tmp2FE;_tmp2FE.tag=0;_tmp2FE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*
_tmp2FC[1]={& _tmp2FE};Cyc_Tcexp_expr_err(_tmp2E3,loc,topt,({const char*_tmp2FD="expecting *, @, or ? type but found %s";
_tag_dyneither(_tmp2FD,sizeof(char),39);}),_tag_dyneither(_tmp2FC,sizeof(void*),
1));});});_LL173:;};}static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){Cyc_Tcexp_tcExpNoPromote(te,
0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(te);{void*_tmp2FF=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo
_tmp301;union Cyc_Absyn_AggrInfoU _tmp302;struct Cyc_Absyn_Aggrdecl**_tmp303;struct
Cyc_Absyn_Aggrdecl*_tmp304;struct Cyc_List_List*_tmp305;enum Cyc_Absyn_AggrKind
_tmp307;struct Cyc_List_List*_tmp308;_LL17B: {struct Cyc_Absyn_AggrType_struct*
_tmp300=(struct Cyc_Absyn_AggrType_struct*)_tmp2FF;if(_tmp300->tag != 12)goto
_LL17D;else{_tmp301=_tmp300->f1;_tmp302=_tmp301.aggr_info;if((_tmp302.KnownAggr).tag
!= 2)goto _LL17D;_tmp303=(struct Cyc_Absyn_Aggrdecl**)(_tmp302.KnownAggr).val;
_tmp304=*_tmp303;_tmp305=_tmp301.targs;}}_LL17C: {struct Cyc_Absyn_Aggrfield*
_tmp30D=Cyc_Absyn_lookup_decl_field(_tmp304,f);if(_tmp30D == 0)return({struct Cyc_String_pa_struct
_tmp311;_tmp311.tag=0;_tmp311.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({
struct Cyc_String_pa_struct _tmp310;_tmp310.tag=0;_tmp310.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp304->name));({void*_tmp30E[2]={&
_tmp310,& _tmp311};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp30F="type %s has no %s field";
_tag_dyneither(_tmp30F,sizeof(char),24);}),_tag_dyneither(_tmp30E,sizeof(void*),
2));});});});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->tagged)*
is_tagged=1;{void*t2;struct _RegionHandle*_tmp312=Cyc_Tcenv_get_fnrgn(te);{struct
Cyc_List_List*_tmp313=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp312,_tmp312,_tmp304->tvs,_tmp305);t2=Cyc_Tcutil_rsubstitute(_tmp312,_tmp313,
_tmp30D->type);if(((_tmp304->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp304->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t2))return({struct Cyc_String_pa_struct _tmp316;_tmp316.tag=0;_tmp316.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp314[1]={& _tmp316};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp315="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp315,sizeof(char),56);}),_tag_dyneither(_tmp314,sizeof(void*),
1));});});}if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->exist_vars
!= 0){if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te))))return({struct Cyc_String_pa_struct _tmp319;_tmp319.tag=0;_tmp319.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp317[1]={& _tmp319};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp318="must use pattern-matching to access field %s\n\tdue to existential type variables.";
_tag_dyneither(_tmp318,sizeof(char),81);}),_tag_dyneither(_tmp317,sizeof(void*),
1));});});}return t2;};}_LL17D: {struct Cyc_Absyn_AnonAggrType_struct*_tmp306=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp2FF;if(_tmp306->tag != 13)goto _LL17F;
else{_tmp307=_tmp306->f1;_tmp308=_tmp306->f2;}}_LL17E: {struct Cyc_Absyn_Aggrfield*
_tmp31A=Cyc_Absyn_lookup_field(_tmp308,f);if(_tmp31A == 0)return({struct Cyc_String_pa_struct
_tmp31D;_tmp31D.tag=0;_tmp31D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({
void*_tmp31B[1]={& _tmp31D};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp31C="type has no %s field";
_tag_dyneither(_tmp31C,sizeof(char),21);}),_tag_dyneither(_tmp31B,sizeof(void*),
1));});});if((_tmp307 == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !
Cyc_Tcutil_bits_only(_tmp31A->type))return({struct Cyc_String_pa_struct _tmp320;
_tmp320.tag=0;_tmp320.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*
_tmp31E[1]={& _tmp320};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp31F="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp31F,sizeof(char),56);}),_tag_dyneither(_tmp31E,sizeof(void*),
1));});});return _tmp31A->type;}_LL17F:{struct Cyc_Absyn_ArrayType_struct*_tmp309=(
struct Cyc_Absyn_ArrayType_struct*)_tmp2FF;if(_tmp309->tag != 9)goto _LL181;}if(!(
Cyc_strcmp((struct _dyneither_ptr)*f,({const char*_tmp30A="size";_tag_dyneither(
_tmp30A,sizeof(char),5);}))== 0))goto _LL181;_LL180: goto _LL182;_LL181:{struct Cyc_Absyn_PointerType_struct*
_tmp30B=(struct Cyc_Absyn_PointerType_struct*)_tmp2FF;if(_tmp30B->tag != 5)goto
_LL183;}if(!(Cyc_strcmp((struct _dyneither_ptr)*f,({const char*_tmp30C="size";
_tag_dyneither(_tmp30C,sizeof(char),5);}))== 0))goto _LL183;_LL182:({struct Cyc_String_pa_struct
_tmp323;_tmp323.tag=0;_tmp323.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));({void*_tmp321[1]={& _tmp323};Cyc_Tcutil_warn(e->loc,({
const char*_tmp322="deprecated `.size' used here -- change to numelts(%s)";
_tag_dyneither(_tmp322,sizeof(char),54);}),_tag_dyneither(_tmp321,sizeof(void*),
1));});});outer_e->r=(void*)({struct Cyc_Absyn_Primop_e_struct*_tmp324=_cycalloc(
sizeof(*_tmp324));_tmp324[0]=({struct Cyc_Absyn_Primop_e_struct _tmp325;_tmp325.tag=
3;_tmp325.f1=Cyc_Absyn_Numelts;_tmp325.f2=({struct Cyc_List_List*_tmp326=
_cycalloc(sizeof(*_tmp326));_tmp326->hd=e;_tmp326->tl=0;_tmp326;});_tmp325;});
_tmp324;});return Cyc_Absyn_uint_typ;_LL183:;_LL184: if(Cyc_strcmp((struct
_dyneither_ptr)*f,({const char*_tmp327="size";_tag_dyneither(_tmp327,sizeof(char),
5);}))== 0)return({struct Cyc_String_pa_struct _tmp32A;_tmp32A.tag=0;_tmp32A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));({void*_tmp328[1]={& _tmp32A};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp329="expecting struct, union, or array, found %s";
_tag_dyneither(_tmp329,sizeof(char),44);}),_tag_dyneither(_tmp328,sizeof(void*),
1));});});else{return({struct Cyc_String_pa_struct _tmp32D;_tmp32D.tag=0;_tmp32D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v));({void*_tmp32B[1]={& _tmp32D};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp32C="expecting struct or union, found %s";
_tag_dyneither(_tmp32C,sizeof(char),36);}),_tag_dyneither(_tmp32B,sizeof(void*),
1));});});}_LL17A:;};}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*f,int*is_tagged,int*is_read){struct _RegionHandle*_tmp32E=Cyc_Tcenv_get_fnrgn(
te);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(_tmp32E,te),0,e);*is_read=!Cyc_Tcenv_in_notreadctxt(
te);{void*_tmp32F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp331;void*_tmp332;struct Cyc_Absyn_PtrAtts _tmp333;void*
_tmp334;union Cyc_Absyn_Constraint*_tmp335;union Cyc_Absyn_Constraint*_tmp336;
_LL186: {struct Cyc_Absyn_PointerType_struct*_tmp330=(struct Cyc_Absyn_PointerType_struct*)
_tmp32F;if(_tmp330->tag != 5)goto _LL188;else{_tmp331=_tmp330->f1;_tmp332=_tmp331.elt_typ;
_tmp333=_tmp331.ptr_atts;_tmp334=_tmp333.rgn;_tmp335=_tmp333.bounds;_tmp336=
_tmp333.zero_term;}}_LL187: Cyc_Tcutil_check_nonzero_bound(loc,_tmp335);{void*
_tmp337=Cyc_Tcutil_compress(_tmp332);struct Cyc_Absyn_AggrInfo _tmp339;union Cyc_Absyn_AggrInfoU
_tmp33A;struct Cyc_Absyn_Aggrdecl**_tmp33B;struct Cyc_Absyn_Aggrdecl*_tmp33C;
struct Cyc_List_List*_tmp33D;enum Cyc_Absyn_AggrKind _tmp33F;struct Cyc_List_List*
_tmp340;_LL18B: {struct Cyc_Absyn_AggrType_struct*_tmp338=(struct Cyc_Absyn_AggrType_struct*)
_tmp337;if(_tmp338->tag != 12)goto _LL18D;else{_tmp339=_tmp338->f1;_tmp33A=_tmp339.aggr_info;
if((_tmp33A.KnownAggr).tag != 2)goto _LL18D;_tmp33B=(struct Cyc_Absyn_Aggrdecl**)(
_tmp33A.KnownAggr).val;_tmp33C=*_tmp33B;_tmp33D=_tmp339.targs;}}_LL18C: {struct
Cyc_Absyn_Aggrfield*_tmp341=Cyc_Absyn_lookup_decl_field(_tmp33C,f);if(_tmp341 == 
0)return({struct Cyc_String_pa_struct _tmp345;_tmp345.tag=0;_tmp345.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_struct _tmp344;
_tmp344.tag=0;_tmp344.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp33C->name));({void*_tmp342[2]={& _tmp344,& _tmp345};Cyc_Tcexp_expr_err(te,loc,
topt,({const char*_tmp343="type %s has no %s field";_tag_dyneither(_tmp343,
sizeof(char),24);}),_tag_dyneither(_tmp342,sizeof(void*),2));});});});if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp33C->impl))->tagged)*is_tagged=1;{
void*t3;struct _RegionHandle*_tmp346=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
_tmp347=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp346,_tmp346,
_tmp33C->tvs,_tmp33D);t3=Cyc_Tcutil_rsubstitute(_tmp346,_tmp347,_tmp341->type);}
if(((_tmp33C->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp33C->impl))->tagged) && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(
t3))return({struct Cyc_String_pa_struct _tmp34A;_tmp34A.tag=0;_tmp34A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp348[1]={& _tmp34A};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp349="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp349,sizeof(char),56);}),_tag_dyneither(_tmp348,sizeof(void*),
1));});});if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp33C->impl))->exist_vars
!= 0){if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te))))return({struct Cyc_String_pa_struct _tmp34D;_tmp34D.tag=0;_tmp34D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp34B[1]={& _tmp34D};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp34C="must use pattern-matching to access field %s\n\tdue to extistential types";
_tag_dyneither(_tmp34C,sizeof(char),72);}),_tag_dyneither(_tmp34B,sizeof(void*),
1));});});}return t3;};}_LL18D: {struct Cyc_Absyn_AnonAggrType_struct*_tmp33E=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp337;if(_tmp33E->tag != 13)goto _LL18F;
else{_tmp33F=_tmp33E->f1;_tmp340=_tmp33E->f2;}}_LL18E: {struct Cyc_Absyn_Aggrfield*
_tmp34E=Cyc_Absyn_lookup_field(_tmp340,f);if(_tmp34E == 0)return({struct Cyc_String_pa_struct
_tmp351;_tmp351.tag=0;_tmp351.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({
void*_tmp34F[1]={& _tmp351};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp350="type has no %s field";
_tag_dyneither(_tmp350,sizeof(char),21);}),_tag_dyneither(_tmp34F,sizeof(void*),
1));});});if((_tmp33F == Cyc_Absyn_UnionA  && !Cyc_Tcenv_in_notreadctxt(te)) && !
Cyc_Tcutil_bits_only(_tmp34E->type))return({struct Cyc_String_pa_struct _tmp354;
_tmp354.tag=0;_tmp354.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*
_tmp352[1]={& _tmp354};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp353="cannot read union member %s since it is not `bits-only'";
_tag_dyneither(_tmp353,sizeof(char),56);}),_tag_dyneither(_tmp352,sizeof(void*),
1));});});return _tmp34E->type;}_LL18F:;_LL190: goto _LL18A;_LL18A:;}goto _LL185;
_LL188:;_LL189: goto _LL185;_LL185:;}return({struct Cyc_String_pa_struct _tmp357;
_tmp357.tag=0;_tmp357.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));({void*_tmp355[1]={&
_tmp357};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp356="expecting struct or union pointer, found %s";
_tag_dyneither(_tmp356,sizeof(char),44);}),_tag_dyneither(_tmp355,sizeof(void*),
1));});});}static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int _tmp359;int
_tmp35A;struct _tuple13 _tmp358=Cyc_Evexp_eval_const_uint_exp(index);_tmp359=
_tmp358.f1;_tmp35A=_tmp358.f2;if(!_tmp35A)return({void*_tmp35B=0;Cyc_Tcexp_expr_err(
te,loc,0,({const char*_tmp35C="tuple projection cannot use sizeof or offsetof";
_tag_dyneither(_tmp35C,sizeof(char),47);}),_tag_dyneither(_tmp35B,sizeof(void*),
0));});{struct _handler_cons _tmp35D;_push_handler(& _tmp35D);{int _tmp35F=0;if(
setjmp(_tmp35D.handler))_tmp35F=1;if(!_tmp35F){{void*_tmp360=(*((struct _tuple17*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp359)).f2;_npop_handler(0);
return _tmp360;};_pop_handler();}else{void*_tmp35E=(void*)_exn_thrown;void*
_tmp362=_tmp35E;_LL192: {struct Cyc_List_Nth_struct*_tmp363=(struct Cyc_List_Nth_struct*)
_tmp362;if(_tmp363->tag != Cyc_List_Nth)goto _LL194;}_LL193: return({struct Cyc_Int_pa_struct
_tmp367;_tmp367.tag=1;_tmp367.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts);({struct Cyc_Int_pa_struct _tmp366;_tmp366.tag=1;_tmp366.f1=(
unsigned long)((int)_tmp359);({void*_tmp364[2]={& _tmp366,& _tmp367};Cyc_Tcexp_expr_err(
te,loc,0,({const char*_tmp365="index is %d but tuple has only %d fields";
_tag_dyneither(_tmp365,sizeof(char),41);}),_tag_dyneither(_tmp364,sizeof(void*),
2));});});});_LL194:;_LL195:(void)_throw(_tmp362);_LL191:;}};};}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){struct _RegionHandle*_tmp368=Cyc_Tcenv_get_fnrgn(
te_orig);struct Cyc_Tcenv_Tenv*_tmp369=Cyc_Tcenv_clear_notreadctxt(_tmp368,
te_orig);Cyc_Tcexp_tcExp(_tmp369,0,e1);Cyc_Tcexp_tcExp(_tmp369,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp369,e2))return({struct Cyc_String_pa_struct
_tmp36C;_tmp36C.tag=0;_tmp36C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({void*_tmp36A[1]={& _tmp36C};Cyc_Tcexp_expr_err(
_tmp369,e2->loc,topt,({const char*_tmp36B="expecting int subscript, found %s";
_tag_dyneither(_tmp36B,sizeof(char),34);}),_tag_dyneither(_tmp36A,sizeof(void*),
1));});});{void*_tmp36D=t1;struct Cyc_Absyn_PtrInfo _tmp36F;void*_tmp370;struct Cyc_Absyn_Tqual
_tmp371;struct Cyc_Absyn_PtrAtts _tmp372;void*_tmp373;union Cyc_Absyn_Constraint*
_tmp374;union Cyc_Absyn_Constraint*_tmp375;struct Cyc_List_List*_tmp377;_LL197: {
struct Cyc_Absyn_PointerType_struct*_tmp36E=(struct Cyc_Absyn_PointerType_struct*)
_tmp36D;if(_tmp36E->tag != 5)goto _LL199;else{_tmp36F=_tmp36E->f1;_tmp370=_tmp36F.elt_typ;
_tmp371=_tmp36F.elt_tq;_tmp372=_tmp36F.ptr_atts;_tmp373=_tmp372.rgn;_tmp374=
_tmp372.bounds;_tmp375=_tmp372.zero_term;}}_LL198: if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp375)){int emit_warning=1;{void*_tmp378=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp374);struct Cyc_Absyn_Exp*_tmp37A;_LL19E: {struct Cyc_Absyn_Upper_b_struct*
_tmp379=(struct Cyc_Absyn_Upper_b_struct*)_tmp378;if(_tmp379->tag != 1)goto _LL1A0;
else{_tmp37A=_tmp379->f1;}}_LL19F: if(Cyc_Tcutil_is_const_exp(_tmp369,e2)){
unsigned int _tmp37D;int _tmp37E;struct _tuple13 _tmp37C=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp37D=_tmp37C.f1;_tmp37E=_tmp37C.f2;if(_tmp37E){unsigned int _tmp380;int
_tmp381;struct _tuple13 _tmp37F=Cyc_Evexp_eval_const_uint_exp(_tmp37A);_tmp380=
_tmp37F.f1;_tmp381=_tmp37F.f2;if(_tmp381  && _tmp380 > _tmp37D)emit_warning=0;}}
goto _LL19D;_LL1A0: {struct Cyc_Absyn_DynEither_b_struct*_tmp37B=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp378;if(_tmp37B->tag != 0)goto _LL19D;}_LL1A1: emit_warning=0;goto _LL19D;_LL19D:;}
if(emit_warning)({void*_tmp382=0;Cyc_Tcutil_warn(e2->loc,({const char*_tmp383="subscript on thin, zero-terminated pointer could be expensive.";
_tag_dyneither(_tmp383,sizeof(char),63);}),_tag_dyneither(_tmp382,sizeof(void*),
0));});}else{if(Cyc_Tcutil_is_const_exp(_tmp369,e2)){unsigned int _tmp385;int
_tmp386;struct _tuple13 _tmp384=Cyc_Evexp_eval_const_uint_exp(e2);_tmp385=_tmp384.f1;
_tmp386=_tmp384.f2;if(_tmp386)Cyc_Tcutil_check_bound(loc,_tmp385,_tmp374);}else{
if(Cyc_Tcutil_is_bound_one(_tmp374) && !((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp375))({void*_tmp387=0;Cyc_Tcutil_warn(e1->loc,({
const char*_tmp388="subscript applied to pointer to one object";_tag_dyneither(
_tmp388,sizeof(char),43);}),_tag_dyneither(_tmp387,sizeof(void*),0));});Cyc_Tcutil_check_nonzero_bound(
loc,_tmp374);}}Cyc_Tcenv_check_rgn_accessible(_tmp369,loc,_tmp373);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp370),Cyc_Absyn_MemKind))({void*_tmp389=0;Cyc_Tcutil_terr(
e1->loc,({const char*_tmp38A="can't subscript an abstract pointer";_tag_dyneither(
_tmp38A,sizeof(char),36);}),_tag_dyneither(_tmp389,sizeof(void*),0));});return
_tmp370;_LL199: {struct Cyc_Absyn_TupleType_struct*_tmp376=(struct Cyc_Absyn_TupleType_struct*)
_tmp36D;if(_tmp376->tag != 11)goto _LL19B;else{_tmp377=_tmp376->f1;}}_LL19A: return
Cyc_Tcexp_ithTupleType(_tmp369,loc,_tmp377,e2);_LL19B:;_LL19C: return({struct Cyc_String_pa_struct
_tmp38D;_tmp38D.tag=0;_tmp38D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));({void*_tmp38B[1]={& _tmp38D};Cyc_Tcexp_expr_err(
_tmp369,loc,topt,({const char*_tmp38C="subscript applied to %s";_tag_dyneither(
_tmp38C,sizeof(char),24);}),_tag_dyneither(_tmp38B,sizeof(void*),1));});});
_LL196:;};};}static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp38E=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp390;
_LL1A3: {struct Cyc_Absyn_TupleType_struct*_tmp38F=(struct Cyc_Absyn_TupleType_struct*)
_tmp38E;if(_tmp38F->tag != 11)goto _LL1A5;else{_tmp390=_tmp38F->f1;}}_LL1A4: if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp390)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1A2;for(0;es != 0;(es=es->tl,_tmp390=_tmp390->tl)){
void*_tmp391=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp390))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp391,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp390->hd)).f2);fields=({
struct Cyc_List_List*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392->hd=({struct
_tuple17*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->f1=(*((struct _tuple17*)
_tmp390->hd)).f1;_tmp393->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Exp*)es->hd)->topt))->v;_tmp393;});_tmp392->tl=fields;_tmp392;});}done=
1;goto _LL1A2;_LL1A5:;_LL1A6: goto _LL1A2;_LL1A2:;}if(!done)for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->hd=({struct _tuple17*_tmp395=
_cycalloc(sizeof(*_tmp395));_tmp395->f1=Cyc_Absyn_empty_tqual(0);_tmp395->f2=(
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp395;});_tmp394->tl=fields;_tmp394;});}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396[0]=({struct Cyc_Absyn_TupleType_struct
_tmp397;_tmp397.tag=11;_tmp397.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);_tmp397;});_tmp396;});}static void*Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple8*
t,struct Cyc_List_List*des){return({void*_tmp398=0;Cyc_Tcexp_expr_err(te,loc,topt,({
const char*_tmp399="tcCompoundLit";_tag_dyneither(_tmp399,sizeof(char),14);}),
_tag_dyneither(_tmp398,sizeof(void*),0));});}static void*Cyc_Tcexp_tcArray(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int zero_term,
struct Cyc_List_List*des){void*res_t2;struct _RegionHandle*_tmp39A=Cyc_Tcenv_get_fnrgn(
te);{int _tmp39B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple16*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp39A,(struct Cyc_Absyn_Exp*(*)(
struct _tuple16*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct*
_tmp39C=({struct Cyc_Absyn_Const_e_struct*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));
_tmp3A4[0]=({struct Cyc_Absyn_Const_e_struct _tmp3A5;_tmp3A5.tag=0;_tmp3A5.f1=Cyc_Absyn_Int_c(
Cyc_Absyn_Unsigned,_tmp39B);_tmp3A5;});_tmp3A4;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((
void*)_tmp39C,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp39D=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp39B - 1);if(!Cyc_Tcutil_is_zero(
_tmp39D))({void*_tmp39E=0;Cyc_Tcutil_terr(_tmp39D->loc,({const char*_tmp39F="zero-terminated array doesn't end with zero.";
_tag_dyneither(_tmp39F,sizeof(char),45);}),_tag_dyneither(_tmp39E,sizeof(void*),
0));});}sz_exp->topt=({struct Cyc_Core_Opt*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));
_tmp3A0->v=Cyc_Absyn_uint_typ;_tmp3A0;});res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es))({struct Cyc_String_pa_struct _tmp3A3;_tmp3A3.tag=0;_tmp3A3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));({void*
_tmp3A1[1]={& _tmp3A3};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,({const char*_tmp3A2="elements of array do not all have the same type (%s)";
_tag_dyneither(_tmp3A2,sizeof(char),53);}),_tag_dyneither(_tmp3A1,sizeof(void*),
1));});});}{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*
ds=(*((struct _tuple16*)des->hd)).f1;if(ds != 0){void*_tmp3A6=(void*)ds->hd;struct
Cyc_Absyn_Exp*_tmp3A9;_LL1A8: {struct Cyc_Absyn_FieldName_struct*_tmp3A7=(struct
Cyc_Absyn_FieldName_struct*)_tmp3A6;if(_tmp3A7->tag != 1)goto _LL1AA;}_LL1A9:({
void*_tmp3AA=0;Cyc_Tcutil_terr(loc,({const char*_tmp3AB="only array index designators are supported";
_tag_dyneither(_tmp3AB,sizeof(char),43);}),_tag_dyneither(_tmp3AA,sizeof(void*),
0));});goto _LL1A7;_LL1AA: {struct Cyc_Absyn_ArrayElement_struct*_tmp3A8=(struct
Cyc_Absyn_ArrayElement_struct*)_tmp3A6;if(_tmp3A8->tag != 0)goto _LL1A7;else{
_tmp3A9=_tmp3A8->f1;}}_LL1AB: Cyc_Tcexp_tcExpInitializer(te,0,_tmp3A9);{
unsigned int _tmp3AD;int _tmp3AE;struct _tuple13 _tmp3AC=Cyc_Evexp_eval_const_uint_exp(
_tmp3A9);_tmp3AD=_tmp3AC.f1;_tmp3AE=_tmp3AC.f2;if(!_tmp3AE)({void*_tmp3AF=0;Cyc_Tcutil_terr(
_tmp3A9->loc,({const char*_tmp3B0="index designator cannot use sizeof or offsetof";
_tag_dyneither(_tmp3B0,sizeof(char),47);}),_tag_dyneither(_tmp3AF,sizeof(void*),
0));});else{if(_tmp3AD != offset)({struct Cyc_Int_pa_struct _tmp3B4;_tmp3B4.tag=1;
_tmp3B4.f1=(unsigned long)((int)_tmp3AD);({struct Cyc_Int_pa_struct _tmp3B3;
_tmp3B3.tag=1;_tmp3B3.f1=(unsigned long)offset;({void*_tmp3B1[2]={& _tmp3B3,&
_tmp3B4};Cyc_Tcutil_terr(_tmp3A9->loc,({const char*_tmp3B2="expecting index designator %d but found %d";
_tag_dyneither(_tmp3B2,sizeof(char),43);}),_tag_dyneither(_tmp3B1,sizeof(void*),
2));});});});}goto _LL1A7;};_LL1A7:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){Cyc_Tcexp_tcExp(
te,0,bound);{void*_tmp3B5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);_LL1AD: {struct Cyc_Absyn_TagType_struct*_tmp3B6=(
struct Cyc_Absyn_TagType_struct*)_tmp3B5;if(_tmp3B6->tag != 20)goto _LL1AF;}_LL1AE:
goto _LL1AC;_LL1AF:;_LL1B0: if(!Cyc_Tcutil_coerce_uint_typ(te,bound))({struct Cyc_String_pa_struct
_tmp3B9;_tmp3B9.tag=0;_tmp3B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v));({
void*_tmp3B7[1]={& _tmp3B9};Cyc_Tcutil_terr(bound->loc,({const char*_tmp3B8="expecting unsigned int, found %s";
_tag_dyneither(_tmp3B8,sizeof(char),33);}),_tag_dyneither(_tmp3B7,sizeof(void*),
1));});});_LL1AC:;}if(!(vd->tq).real_const)({void*_tmp3BA=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3BB="comprehension index variable is not declared const!";
_tag_dyneither(_tmp3BB,sizeof(char),52);}),_tag_dyneither(_tmp3BA,sizeof(void*),
0));});{struct _RegionHandle*_tmp3BC=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_copy_tenv(_tmp3BC,te);if(te2->le != 0){te2=Cyc_Tcenv_new_block(
_tmp3BC,loc,te2);te2=Cyc_Tcenv_add_local_var(_tmp3BC,loc,te2,vd);}{struct Cyc_Tcenv_Tenv*
_tmp3BD=te2;struct Cyc_Absyn_PtrInfo pinfo;void**_tmp3BE=0;struct Cyc_Absyn_Tqual*
_tmp3BF=0;union Cyc_Absyn_Constraint**_tmp3C0=0;if(topt != 0){void*_tmp3C1=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3C3;struct Cyc_Absyn_ArrayInfo _tmp3C5;void*
_tmp3C6;void**_tmp3C7;struct Cyc_Absyn_Tqual _tmp3C8;struct Cyc_Absyn_Tqual*_tmp3C9;
struct Cyc_Absyn_Exp*_tmp3CA;union Cyc_Absyn_Constraint*_tmp3CB;union Cyc_Absyn_Constraint**
_tmp3CC;_LL1B2: {struct Cyc_Absyn_PointerType_struct*_tmp3C2=(struct Cyc_Absyn_PointerType_struct*)
_tmp3C1;if(_tmp3C2->tag != 5)goto _LL1B4;else{_tmp3C3=_tmp3C2->f1;}}_LL1B3: pinfo=
_tmp3C3;_tmp3BE=(void**)& pinfo.elt_typ;_tmp3BF=(struct Cyc_Absyn_Tqual*)& pinfo.elt_tq;
_tmp3C0=(union Cyc_Absyn_Constraint**)&(pinfo.ptr_atts).zero_term;goto _LL1B1;
_LL1B4: {struct Cyc_Absyn_ArrayType_struct*_tmp3C4=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3C1;if(_tmp3C4->tag != 9)goto _LL1B6;else{_tmp3C5=_tmp3C4->f1;_tmp3C6=_tmp3C5.elt_type;
_tmp3C7=(void**)&(_tmp3C4->f1).elt_type;_tmp3C8=_tmp3C5.tq;_tmp3C9=(struct Cyc_Absyn_Tqual*)&(
_tmp3C4->f1).tq;_tmp3CA=_tmp3C5.num_elts;_tmp3CB=_tmp3C5.zero_term;_tmp3CC=(
union Cyc_Absyn_Constraint**)&(_tmp3C4->f1).zero_term;}}_LL1B5: _tmp3BE=(void**)
_tmp3C7;_tmp3BF=(struct Cyc_Absyn_Tqual*)_tmp3C9;_tmp3C0=(union Cyc_Absyn_Constraint**)
_tmp3CC;goto _LL1B1;_LL1B6:;_LL1B7: goto _LL1B1;_LL1B1:;}{void*t=Cyc_Tcexp_tcExp(
_tmp3BD,_tmp3BE,body);if(_tmp3BD->le == 0){if(!Cyc_Tcutil_is_const_exp(_tmp3BD,
bound))({void*_tmp3CD=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp3CE="bound is not constant";
_tag_dyneither(_tmp3CE,sizeof(char),22);}),_tag_dyneither(_tmp3CD,sizeof(void*),
0));});if(!Cyc_Tcutil_is_const_exp(_tmp3BD,body))({void*_tmp3CF=0;Cyc_Tcutil_terr(
bound->loc,({const char*_tmp3D0="body is not constant";_tag_dyneither(_tmp3D0,
sizeof(char),21);}),_tag_dyneither(_tmp3CF,sizeof(void*),0));});}if(_tmp3C0 != 0
 && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3C0)){
struct Cyc_Absyn_Exp*_tmp3D1=Cyc_Absyn_uint_exp(1,0);_tmp3D1->topt=({struct Cyc_Core_Opt*
_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->v=Cyc_Absyn_uint_typ;_tmp3D2;});
bound=Cyc_Absyn_add_exp(bound,_tmp3D1,0);bound->topt=({struct Cyc_Core_Opt*
_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->v=Cyc_Absyn_uint_typ;_tmp3D3;});*
is_zero_term=1;}{void*_tmp3D4=Cyc_Absyn_array_typ(t,_tmp3BF == 0?Cyc_Absyn_empty_tqual(
0):*_tmp3BF,(struct Cyc_Absyn_Exp*)bound,_tmp3C0 == 0?Cyc_Absyn_false_conref:*
_tmp3C0,0);return _tmp3D4;};};};};}struct _tuple18{struct Cyc_Absyn_Aggrfield*f1;
struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp3D5;_push_handler(& _tmp3D5);{int _tmp3D7=0;if(setjmp(
_tmp3D5.handler))_tmp3D7=1;if(!_tmp3D7){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp3D6=(void*)_exn_thrown;void*_tmp3D9=_tmp3D6;_LL1B9: {
struct Cyc_Dict_Absent_struct*_tmp3DA=(struct Cyc_Dict_Absent_struct*)_tmp3D9;if(
_tmp3DA->tag != Cyc_Dict_Absent)goto _LL1BB;}_LL1BA:({struct Cyc_String_pa_struct
_tmp3DD;_tmp3DD.tag=0;_tmp3DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn));({void*_tmp3DB[1]={& _tmp3DD};Cyc_Tcutil_terr(loc,({
const char*_tmp3DC="unbound struct/union name %s";_tag_dyneither(_tmp3DC,sizeof(
char),29);}),_tag_dyneither(_tmp3DB,sizeof(void*),1));});});return topt != 0?*topt:(
void*)& Cyc_Absyn_VoidType_val;_LL1BB:;_LL1BC:(void)_throw(_tmp3D9);_LL1B8:;}};}*
ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if(ad->impl == 0){({struct Cyc_String_pa_struct
_tmp3E0;_tmp3E0.tag=0;_tmp3E0.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({
const char*_tmp3E1="struct";_tag_dyneither(_tmp3E1,sizeof(char),7);}):({const char*
_tmp3E2="union";_tag_dyneither(_tmp3E2,sizeof(char),6);}));({void*_tmp3DE[1]={&
_tmp3E0};Cyc_Tcutil_terr(loc,({const char*_tmp3DF="can't construct abstract %s";
_tag_dyneither(_tmp3DF,sizeof(char),28);}),_tag_dyneither(_tmp3DE,sizeof(void*),
1));});});return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{struct
_RegionHandle*_tmp3E3=Cyc_Tcenv_get_fnrgn(te);struct _tuple10 _tmp3E4=({struct
_tuple10 _tmp405;_tmp405.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp405.f2=_tmp3E3;
_tmp405;});struct Cyc_List_List*_tmp3E5=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3E3,Cyc_Tcutil_r_make_inst_var,&
_tmp3E4,ad->tvs);struct Cyc_List_List*_tmp3E6=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3E3,Cyc_Tcutil_r_make_inst_var,&
_tmp3E4,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp3E7=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),
struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp3E5);struct Cyc_List_List*_tmp3E8=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))Cyc_Core_snd,
_tmp3E6);struct Cyc_List_List*_tmp3E9=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp3E3,_tmp3E5,_tmp3E6);struct Cyc_Absyn_AggrType_struct*_tmp3EA=({struct Cyc_Absyn_AggrType_struct*
_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401[0]=({struct Cyc_Absyn_AggrType_struct
_tmp402;_tmp402.tag=12;_tmp402.f1=({struct Cyc_Absyn_AggrInfo _tmp403;_tmp403.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp404=_cycalloc(sizeof(*
_tmp404));_tmp404[0]=ad;_tmp404;}));_tmp403.targs=_tmp3E7;_tmp403;});_tmp402;});
_tmp401;});struct Cyc_List_List*_tmp3EB=*ts;struct Cyc_List_List*_tmp3EC=_tmp3E8;
while(_tmp3EB != 0  && _tmp3EC != 0){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),Cyc_Absyn_AnyKind,1,(void*)_tmp3EB->hd);Cyc_Tcutil_unify((void*)_tmp3EB->hd,(
void*)_tmp3EC->hd);_tmp3EB=_tmp3EB->tl;_tmp3EC=_tmp3EC->tl;}if(_tmp3EB != 0)({
void*_tmp3ED=0;Cyc_Tcutil_terr(loc,({const char*_tmp3EE="too many explicit witness types";
_tag_dyneither(_tmp3EE,sizeof(char),32);}),_tag_dyneither(_tmp3ED,sizeof(void*),
0));});*ts=_tmp3E8;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp3EA,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))
Cyc_Tcutil_resolve_aggregate_designators)(_tmp3E3,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields);for(0;fields != 0;fields=fields->tl){struct
_tuple18 _tmp3F0;struct Cyc_Absyn_Aggrfield*_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F2;
struct _tuple18*_tmp3EF=(struct _tuple18*)fields->hd;_tmp3F0=*_tmp3EF;_tmp3F1=
_tmp3F0.f1;_tmp3F2=_tmp3F0.f2;{void*_tmp3F3=Cyc_Tcutil_rsubstitute(_tmp3E3,
_tmp3E9,_tmp3F1->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp3F3,_tmp3F2);
if(!Cyc_Tcutil_coerce_arg(te,_tmp3F2,_tmp3F3)){({struct Cyc_String_pa_struct
_tmp3FA;_tmp3FA.tag=0;_tmp3FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F2->topt))->v));({
struct Cyc_String_pa_struct _tmp3F9;_tmp3F9.tag=0;_tmp3F9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp3F3));({struct Cyc_String_pa_struct
_tmp3F8;_tmp3F8.tag=0;_tmp3F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(*tn));({struct Cyc_String_pa_struct _tmp3F7;_tmp3F7.tag=0;
_tmp3F7.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*
_tmp3FB="struct";_tag_dyneither(_tmp3FB,sizeof(char),7);}):({const char*_tmp3FC="union";
_tag_dyneither(_tmp3FC,sizeof(char),6);}));({struct Cyc_String_pa_struct _tmp3F6;
_tmp3F6.tag=0;_tmp3F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3F1->name);({
void*_tmp3F4[5]={& _tmp3F6,& _tmp3F7,& _tmp3F8,& _tmp3F9,& _tmp3FA};Cyc_Tcutil_terr(
_tmp3F2->loc,({const char*_tmp3F5="field %s of %s %s expects type %s != %s";
_tag_dyneither(_tmp3F5,sizeof(char),40);}),_tag_dyneither(_tmp3F4,sizeof(void*),
5));});});});});});});Cyc_Tcutil_explain_failure();}};}{struct Cyc_List_List*
_tmp3FD=0;{struct Cyc_List_List*_tmp3FE=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->rgn_po;for(0;_tmp3FE != 0;_tmp3FE=_tmp3FE->tl){_tmp3FD=({
struct Cyc_List_List*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->hd=({struct
_tuple0*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->f1=Cyc_Tcutil_rsubstitute(
_tmp3E3,_tmp3E9,(*((struct _tuple0*)_tmp3FE->hd)).f1);_tmp400->f2=Cyc_Tcutil_rsubstitute(
_tmp3E3,_tmp3E9,(*((struct _tuple0*)_tmp3FE->hd)).f2);_tmp400;});_tmp3FF->tl=
_tmp3FD;_tmp3FF;});}}_tmp3FD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp3FD);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp3FD);
return(void*)_tmp3EA;};};};}static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*args){struct
_RegionHandle*_tmp406=Cyc_Tcenv_get_fnrgn(te);{void*_tmp407=Cyc_Tcutil_compress(
ts);enum Cyc_Absyn_AggrKind _tmp409;struct Cyc_List_List*_tmp40A;_LL1BE: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp408=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp407;if(_tmp408->tag != 13)goto _LL1C0;else{_tmp409=_tmp408->f1;_tmp40A=_tmp408->f2;}}
_LL1BF: if(_tmp409 == Cyc_Absyn_UnionA)({void*_tmp40B=0;Cyc_Tcutil_terr(loc,({
const char*_tmp40C="expecting struct but found union";_tag_dyneither(_tmp40C,
sizeof(char),33);}),_tag_dyneither(_tmp40B,sizeof(void*),0));});{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))
Cyc_Tcutil_resolve_aggregate_designators)(_tmp406,loc,args,Cyc_Absyn_StructA,
_tmp40A);for(0;fields != 0;fields=fields->tl){struct _tuple18 _tmp40E;struct Cyc_Absyn_Aggrfield*
_tmp40F;struct Cyc_Absyn_Exp*_tmp410;struct _tuple18*_tmp40D=(struct _tuple18*)
fields->hd;_tmp40E=*_tmp40D;_tmp40F=_tmp40E.f1;_tmp410=_tmp40E.f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp40F->type,_tmp410);if(!Cyc_Tcutil_coerce_arg(te,_tmp410,_tmp40F->type)){({
struct Cyc_String_pa_struct _tmp415;_tmp415.tag=0;_tmp415.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp410->topt))->v));({struct Cyc_String_pa_struct _tmp414;_tmp414.tag=
0;_tmp414.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp40F->type));({struct Cyc_String_pa_struct _tmp413;_tmp413.tag=0;_tmp413.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp40F->name);({void*_tmp411[3]={&
_tmp413,& _tmp414,& _tmp415};Cyc_Tcutil_terr(_tmp410->loc,({const char*_tmp412="field %s of struct expects type %s != %s";
_tag_dyneither(_tmp412,sizeof(char),41);}),_tag_dyneither(_tmp411,sizeof(void*),
3));});});});});Cyc_Tcutil_explain_failure();}}goto _LL1BD;};_LL1C0:;_LL1C1:({
void*_tmp416=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp417="tcAnonStruct: wrong type";_tag_dyneither(_tmp417,sizeof(char),
25);}),_tag_dyneither(_tmp416,sizeof(void*),0));});_LL1BD:;}return ts;}static void*
Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*
tud,struct Cyc_Absyn_Datatypefield*tuf){struct _RegionHandle*_tmp418=Cyc_Tcenv_get_fnrgn(
te);struct _tuple10 _tmp419=({struct _tuple10 _tmp42D;_tmp42D.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp42D.f2=_tmp418;_tmp42D;});struct Cyc_List_List*_tmp41A=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),
struct _tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp418,Cyc_Tcutil_r_make_inst_var,&
_tmp419,tud->tvs);struct Cyc_List_List*_tmp41B=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,_tmp41A);void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A[0]=({struct Cyc_Absyn_DatatypeFieldType_struct
_tmp42B;_tmp42B.tag=4;_tmp42B.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp42C;
_tmp42C.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmp42C.targs=_tmp41B;
_tmp42C;});_tmp42B;});_tmp42A;});if(topt != 0){void*_tmp41C=Cyc_Tcutil_compress(*
topt);_LL1C3: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp41D=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp41C;if(_tmp41D->tag != 4)goto _LL1C5;}_LL1C4: Cyc_Tcutil_unify(*topt,res);goto
_LL1C2;_LL1C5:;_LL1C6: goto _LL1C2;_LL1C2:;}{struct Cyc_List_List*ts=tuf->typs;for(
0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
es->hd;void*t=Cyc_Tcutil_rsubstitute(_tmp418,_tmp41A,(*((struct _tuple17*)ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){({
struct Cyc_String_pa_struct _tmp422;_tmp422.tag=0;_tmp422.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const char*_tmp423="?";
_tag_dyneither(_tmp423,sizeof(char),2);}): Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v)));({struct Cyc_String_pa_struct _tmp421;
_tmp421.tag=0;_tmp421.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));({struct Cyc_String_pa_struct _tmp420;_tmp420.tag=0;_tmp420.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));({void*
_tmp41E[3]={& _tmp420,& _tmp421,& _tmp422};Cyc_Tcutil_terr(e->loc,({const char*
_tmp41F="datatype constructor %s expects argument of type %s but this argument has type %s";
_tag_dyneither(_tmp41F,sizeof(char),82);}),_tag_dyneither(_tmp41E,sizeof(void*),
3));});});});});Cyc_Tcutil_explain_failure();}}if(es != 0)return({struct Cyc_String_pa_struct
_tmp426;_tmp426.tag=0;_tmp426.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));({void*_tmp424[1]={& _tmp426};Cyc_Tcexp_expr_err(
te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,({const char*_tmp425="too many arguments for datatype constructor %s";
_tag_dyneither(_tmp425,sizeof(char),47);}),_tag_dyneither(_tmp424,sizeof(void*),
1));});});if(ts != 0)return({struct Cyc_String_pa_struct _tmp429;_tmp429.tag=0;
_tmp429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
tuf->name));({void*_tmp427[1]={& _tmp429};Cyc_Tcexp_expr_err(te,loc,topt,({const
char*_tmp428="too few arguments for datatype constructor %s";_tag_dyneither(
_tmp428,sizeof(char),46);}),_tag_dyneither(_tmp427,sizeof(void*),1));});});
return res;};}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp42E=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp435;struct Cyc_Absyn_PtrAtts _tmp436;union Cyc_Absyn_Constraint*
_tmp437;struct Cyc_Absyn_ArrayInfo _tmp43C;void*_tmp43D;struct Cyc_List_List*
_tmp440;struct Cyc_Absyn_AggrInfo _tmp442;union Cyc_Absyn_AggrInfoU _tmp443;struct
Cyc_List_List*_tmp444;struct Cyc_List_List*_tmp448;_LL1C8: {struct Cyc_Absyn_VoidType_struct*
_tmp42F=(struct Cyc_Absyn_VoidType_struct*)_tmp42E;if(_tmp42F->tag != 0)goto _LL1CA;}
_LL1C9: return 1;_LL1CA: {struct Cyc_Absyn_Evar_struct*_tmp430=(struct Cyc_Absyn_Evar_struct*)
_tmp42E;if(_tmp430->tag != 1)goto _LL1CC;}_LL1CB: goto _LL1CD;_LL1CC: {struct Cyc_Absyn_VarType_struct*
_tmp431=(struct Cyc_Absyn_VarType_struct*)_tmp42E;if(_tmp431->tag != 2)goto _LL1CE;}
_LL1CD: goto _LL1CF;_LL1CE: {struct Cyc_Absyn_DatatypeType_struct*_tmp432=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp42E;if(_tmp432->tag != 3)goto _LL1D0;}_LL1CF:
goto _LL1D1;_LL1D0: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp433=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp42E;if(_tmp433->tag != 4)goto _LL1D2;}_LL1D1: return 0;_LL1D2: {struct Cyc_Absyn_PointerType_struct*
_tmp434=(struct Cyc_Absyn_PointerType_struct*)_tmp42E;if(_tmp434->tag != 5)goto
_LL1D4;else{_tmp435=_tmp434->f1;_tmp436=_tmp435.ptr_atts;_tmp437=_tmp436.nullable;}}
_LL1D3: return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,
_tmp437);_LL1D4: {struct Cyc_Absyn_IntType_struct*_tmp438=(struct Cyc_Absyn_IntType_struct*)
_tmp42E;if(_tmp438->tag != 6)goto _LL1D6;}_LL1D5: goto _LL1D7;_LL1D6: {struct Cyc_Absyn_FloatType_struct*
_tmp439=(struct Cyc_Absyn_FloatType_struct*)_tmp42E;if(_tmp439->tag != 7)goto
_LL1D8;}_LL1D7: goto _LL1D9;_LL1D8: {struct Cyc_Absyn_DoubleType_struct*_tmp43A=(
struct Cyc_Absyn_DoubleType_struct*)_tmp42E;if(_tmp43A->tag != 8)goto _LL1DA;}
_LL1D9: return 1;_LL1DA: {struct Cyc_Absyn_ArrayType_struct*_tmp43B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp42E;if(_tmp43B->tag != 9)goto _LL1DC;else{_tmp43C=_tmp43B->f1;_tmp43D=_tmp43C.elt_type;}}
_LL1DB: return Cyc_Tcexp_zeroable_type(_tmp43D);_LL1DC: {struct Cyc_Absyn_FnType_struct*
_tmp43E=(struct Cyc_Absyn_FnType_struct*)_tmp42E;if(_tmp43E->tag != 10)goto _LL1DE;}
_LL1DD: return 0;_LL1DE: {struct Cyc_Absyn_TupleType_struct*_tmp43F=(struct Cyc_Absyn_TupleType_struct*)
_tmp42E;if(_tmp43F->tag != 11)goto _LL1E0;else{_tmp440=_tmp43F->f1;}}_LL1DF: for(0;(
unsigned int)_tmp440;_tmp440=_tmp440->tl){if(!Cyc_Tcexp_zeroable_type((*((struct
_tuple17*)_tmp440->hd)).f2))return 0;}return 1;_LL1E0: {struct Cyc_Absyn_AggrType_struct*
_tmp441=(struct Cyc_Absyn_AggrType_struct*)_tmp42E;if(_tmp441->tag != 12)goto
_LL1E2;else{_tmp442=_tmp441->f1;_tmp443=_tmp442.aggr_info;_tmp444=_tmp442.targs;}}
_LL1E1: {struct Cyc_Absyn_Aggrdecl*_tmp453=Cyc_Absyn_get_known_aggrdecl(_tmp443);
if(_tmp453->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp453->impl))->exist_vars
!= 0)return 0;{struct _RegionHandle _tmp454=_new_region("r");struct _RegionHandle*r=&
_tmp454;_push_region(r);{struct Cyc_List_List*_tmp455=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(r,r,_tmp453->tvs,_tmp444);{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp453->impl))->fields;for(0;fs != 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp455,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp456=0;_npop_handler(0);return _tmp456;}}}{int _tmp457=1;_npop_handler(0);
return _tmp457;};};_pop_region(r);};}_LL1E2: {struct Cyc_Absyn_EnumType_struct*
_tmp445=(struct Cyc_Absyn_EnumType_struct*)_tmp42E;if(_tmp445->tag != 14)goto
_LL1E4;}_LL1E3: return 1;_LL1E4: {struct Cyc_Absyn_TagType_struct*_tmp446=(struct
Cyc_Absyn_TagType_struct*)_tmp42E;if(_tmp446->tag != 20)goto _LL1E6;}_LL1E5: return
1;_LL1E6: {struct Cyc_Absyn_AnonAggrType_struct*_tmp447=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp42E;if(_tmp447->tag != 13)goto _LL1E8;else{_tmp448=_tmp447->f2;}}_LL1E7: for(0;
_tmp448 != 0;_tmp448=_tmp448->tl){if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)
_tmp448->hd)->type))return 0;}return 1;_LL1E8: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp449=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp42E;if(_tmp449->tag != 15)goto
_LL1EA;}_LL1E9: return 1;_LL1EA: {struct Cyc_Absyn_TypedefType_struct*_tmp44A=(
struct Cyc_Absyn_TypedefType_struct*)_tmp42E;if(_tmp44A->tag != 18)goto _LL1EC;}
_LL1EB: return 0;_LL1EC: {struct Cyc_Absyn_DynRgnType_struct*_tmp44B=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp42E;if(_tmp44B->tag != 17)goto _LL1EE;}_LL1ED: return 0;_LL1EE: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp44C=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp42E;if(_tmp44C->tag != 16)goto
_LL1F0;}_LL1EF: return 0;_LL1F0: {struct Cyc_Absyn_ValueofType_struct*_tmp44D=(
struct Cyc_Absyn_ValueofType_struct*)_tmp42E;if(_tmp44D->tag != 19)goto _LL1F2;}
_LL1F1: goto _LL1F3;_LL1F2: {struct Cyc_Absyn_HeapRgn_struct*_tmp44E=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp42E;if(_tmp44E->tag != 21)goto _LL1F4;}_LL1F3: goto _LL1F5;_LL1F4: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp44F=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp42E;if(_tmp44F->tag != 22)goto
_LL1F6;}_LL1F5: goto _LL1F7;_LL1F6: {struct Cyc_Absyn_AccessEff_struct*_tmp450=(
struct Cyc_Absyn_AccessEff_struct*)_tmp42E;if(_tmp450->tag != 23)goto _LL1F8;}
_LL1F7: goto _LL1F9;_LL1F8: {struct Cyc_Absyn_JoinEff_struct*_tmp451=(struct Cyc_Absyn_JoinEff_struct*)
_tmp42E;if(_tmp451->tag != 24)goto _LL1FA;}_LL1F9: goto _LL1FB;_LL1FA: {struct Cyc_Absyn_RgnsEff_struct*
_tmp452=(struct Cyc_Absyn_RgnsEff_struct*)_tmp42E;if(_tmp452->tag != 25)goto _LL1C7;}
_LL1FB:({struct Cyc_String_pa_struct _tmp45A;_tmp45A.tag=0;_tmp45A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp458[
1]={& _tmp45A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp459="bad type `%s' in zeroable type";_tag_dyneither(_tmp459,
sizeof(char),31);}),_tag_dyneither(_tmp458,sizeof(void*),1));});});_LL1C7:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(
t))return;if(topt != 0){void*_tmp45B=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp45D;void*_tmp45E;_LL1FD: {struct Cyc_Absyn_PointerType_struct*_tmp45C=(struct
Cyc_Absyn_PointerType_struct*)_tmp45B;if(_tmp45C->tag != 5)goto _LL1FF;else{
_tmp45D=_tmp45C->f1;_tmp45E=_tmp45D.elt_typ;}}_LL1FE: Cyc_Tcutil_unify(_tmp45E,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL1FC;_LL1FF:;_LL200: goto _LL1FC;_LL1FC:;}({struct Cyc_String_pa_struct
_tmp462;_tmp462.tag=0;_tmp462.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_struct _tmp461;_tmp461.tag=0;
_tmp461.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)({const char*_tmp463="calloc";_tag_dyneither(_tmp463,sizeof(char),
7);}):(struct _dyneither_ptr)({const char*_tmp464="malloc";_tag_dyneither(_tmp464,
sizeof(char),7);})));({void*_tmp45F[2]={& _tmp461,& _tmp462};Cyc_Tcutil_terr(loc,({
const char*_tmp460="%s cannot be used with type %s\n\t(type needs initialization)";
_tag_dyneither(_tmp460,sizeof(char),60);}),_tag_dyneither(_tmp45F,sizeof(void*),
2));});});});}static void*Cyc_Tcexp_mallocRgn(void*rgn){switch(Cyc_Tcutil_typ_kind(
Cyc_Tcutil_compress(rgn))){case Cyc_Absyn_UniqueRgnKind: _LL201: return(void*)& Cyc_Absyn_UniqueRgn_val;
default: _LL202: return(void*)& Cyc_Absyn_HeapRgn_val;}}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)&
Cyc_Absyn_HeapRgn_val;if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp46C;_tmp46C.tag=16;_tmp46C.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp46C;});_tmp46B;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp465=Cyc_Tcutil_compress(handle_type);void*_tmp467;_LL205: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp466=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp465;if(_tmp466->tag != 16)goto
_LL207;else{_tmp467=(void*)_tmp466->f1;}}_LL206: rgn=_tmp467;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL204;_LL207:;_LL208:({struct Cyc_String_pa_struct _tmp46A;
_tmp46A.tag=0;_tmp46A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
handle_type));({void*_tmp468[1]={& _tmp46A};Cyc_Tcutil_terr(ropt->loc,({const char*
_tmp469="expecting region_t type but found %s";_tag_dyneither(_tmp469,sizeof(
char),37);}),_tag_dyneither(_tmp468,sizeof(void*),1));});});goto _LL204;_LL204:;}
else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_rgn_of_pointer(*
topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*
e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 
0)({void*_tmp46D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp46E="calloc with empty type";_tag_dyneither(_tmp46E,sizeof(char),
23);}),_tag_dyneither(_tmp46D,sizeof(void*),0));});elt_type=*((void**)
_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),Cyc_Absyn_MemKind,
1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);num_elts=*e;one_elt=
0;}else{void*_tmp46F=(*e)->r;void*_tmp471;enum Cyc_Absyn_Primop _tmp473;struct Cyc_List_List*
_tmp474;struct Cyc_List_List _tmp475;struct Cyc_Absyn_Exp*_tmp476;struct Cyc_List_List*
_tmp477;struct Cyc_List_List _tmp478;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_List_List*
_tmp47A;_LL20A: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp470=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp46F;if(_tmp470->tag != 18)goto _LL20C;else{_tmp471=(void*)_tmp470->f1;}}_LL20B:
elt_type=_tmp471;*t=(void**)({void**_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B[
0]=elt_type;_tmp47B;});num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(te,(void**)&
Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL209;_LL20C: {struct Cyc_Absyn_Primop_e_struct*
_tmp472=(struct Cyc_Absyn_Primop_e_struct*)_tmp46F;if(_tmp472->tag != 3)goto _LL20E;
else{_tmp473=_tmp472->f1;if(_tmp473 != Cyc_Absyn_Times)goto _LL20E;_tmp474=_tmp472->f2;
if(_tmp474 == 0)goto _LL20E;_tmp475=*_tmp474;_tmp476=(struct Cyc_Absyn_Exp*)_tmp475.hd;
_tmp477=_tmp475.tl;if(_tmp477 == 0)goto _LL20E;_tmp478=*_tmp477;_tmp479=(struct Cyc_Absyn_Exp*)
_tmp478.hd;_tmp47A=_tmp478.tl;if(_tmp47A != 0)goto _LL20E;}}_LL20D:{struct _tuple0
_tmp47D=({struct _tuple0 _tmp47C;_tmp47C.f1=_tmp476->r;_tmp47C.f2=_tmp479->r;
_tmp47C;});void*_tmp47E;void*_tmp480;void*_tmp481;void*_tmp483;_LL211: _tmp47E=
_tmp47D.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp47F=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp47E;if(_tmp47F->tag != 18)goto _LL213;else{_tmp480=(void*)_tmp47F->f1;}};
_LL212: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp480);elt_type=_tmp480;*t=(void**)({
void**_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484[0]=elt_type;_tmp484;});
num_elts=_tmp479;one_elt=0;goto _LL210;_LL213: _tmp481=_tmp47D.f2;{struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp482=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp481;if(_tmp482->tag != 18)goto
_LL215;else{_tmp483=(void*)_tmp482->f1;}};_LL214: Cyc_Tcexp_check_malloc_type(0,
loc,topt,_tmp483);elt_type=_tmp483;*t=(void**)({void**_tmp485=_cycalloc(sizeof(*
_tmp485));_tmp485[0]=elt_type;_tmp485;});num_elts=_tmp476;one_elt=0;goto _LL210;
_LL215:;_LL216: goto No_sizeof;_LL210:;}goto _LL209;_LL20E:;_LL20F: No_sizeof:
elt_type=Cyc_Absyn_char_typ;*t=(void**)({void**_tmp486=_cycalloc(sizeof(*_tmp486));
_tmp486[0]=elt_type;_tmp486;});num_elts=*e;one_elt=0;goto _LL209;_LL209:;}*e=
num_elts;*is_fat=!one_elt;{void*_tmp487=elt_type;struct Cyc_Absyn_AggrInfo _tmp489;
union Cyc_Absyn_AggrInfoU _tmp48A;struct Cyc_Absyn_Aggrdecl**_tmp48B;struct Cyc_Absyn_Aggrdecl*
_tmp48C;_LL218: {struct Cyc_Absyn_AggrType_struct*_tmp488=(struct Cyc_Absyn_AggrType_struct*)
_tmp487;if(_tmp488->tag != 12)goto _LL21A;else{_tmp489=_tmp488->f1;_tmp48A=_tmp489.aggr_info;
if((_tmp48A.KnownAggr).tag != 2)goto _LL21A;_tmp48B=(struct Cyc_Absyn_Aggrdecl**)(
_tmp48A.KnownAggr).val;_tmp48C=*_tmp48B;}}_LL219: if(_tmp48C->impl != 0  && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48C->impl))->exist_vars != 0)({void*
_tmp48D=0;Cyc_Tcutil_terr(loc,({const char*_tmp48E="malloc with existential types not yet implemented";
_tag_dyneither(_tmp48E,sizeof(char),50);}),_tag_dyneither(_tmp48D,sizeof(void*),
0));});goto _LL217;_LL21A:;_LL21B: goto _LL217;_LL217:;}{void*(*_tmp48F)(void*t,
void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp490=Cyc_Absyn_false_conref;if(topt != 0){void*
_tmp491=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp493;struct Cyc_Absyn_PtrAtts
_tmp494;union Cyc_Absyn_Constraint*_tmp495;union Cyc_Absyn_Constraint*_tmp496;
union Cyc_Absyn_Constraint*_tmp497;_LL21D: {struct Cyc_Absyn_PointerType_struct*
_tmp492=(struct Cyc_Absyn_PointerType_struct*)_tmp491;if(_tmp492->tag != 5)goto
_LL21F;else{_tmp493=_tmp492->f1;_tmp494=_tmp493.ptr_atts;_tmp495=_tmp494.nullable;
_tmp496=_tmp494.bounds;_tmp497=_tmp494.zero_term;}}_LL21E: _tmp490=_tmp497;if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp495))
_tmp48F=Cyc_Absyn_star_typ;if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp497) && !(*is_calloc)){({void*_tmp498=0;Cyc_Tcutil_warn(loc,({const char*
_tmp499="converting malloc to calloc to ensure zero-termination";_tag_dyneither(
_tmp499,sizeof(char),55);}),_tag_dyneither(_tmp498,sizeof(void*),0));});*
is_calloc=1;}{void*_tmp49A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp496);struct Cyc_Absyn_Exp*_tmp49D;_LL222: {struct Cyc_Absyn_DynEither_b_struct*
_tmp49B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp49A;if(_tmp49B->tag != 0)goto
_LL224;}_LL223: goto _LL221;_LL224:{struct Cyc_Absyn_Upper_b_struct*_tmp49C=(struct
Cyc_Absyn_Upper_b_struct*)_tmp49A;if(_tmp49C->tag != 1)goto _LL226;else{_tmp49D=
_tmp49C->f1;}}if(!(!one_elt))goto _LL226;_LL225: {int _tmp49E=Cyc_Evexp_c_can_eval(
num_elts);if(_tmp49E  && Cyc_Evexp_same_const_exp(_tmp49D,num_elts)){*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp496),_tmp490);}{void*_tmp49F=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);
void*_tmp4A1;_LL229: {struct Cyc_Absyn_TagType_struct*_tmp4A0=(struct Cyc_Absyn_TagType_struct*)
_tmp49F;if(_tmp4A0->tag != 20)goto _LL22B;else{_tmp4A1=(void*)_tmp4A0->f1;}}_LL22A: {
struct Cyc_Absyn_Exp*_tmp4A2=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp4A1,0),0,Cyc_Absyn_No_coercion,0);if(Cyc_Evexp_same_const_exp(_tmp4A2,
_tmp49D)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp496),_tmp490);}
goto _LL228;}_LL22B:;_LL22C: goto _LL228;_LL228:;}goto _LL221;}_LL226:;_LL227: goto
_LL221;_LL221:;}goto _LL21C;_LL21F:;_LL220: goto _LL21C;_LL21C:;}if(!one_elt)
_tmp48F=Cyc_Absyn_dyneither_typ;return _tmp48F(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp490);};};}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp4A3=Cyc_Tcutil_compress(t1);_LL22E: {struct Cyc_Absyn_ArrayType_struct*
_tmp4A4=(struct Cyc_Absyn_ArrayType_struct*)_tmp4A3;if(_tmp4A4->tag != 9)goto
_LL230;}_LL22F:({void*_tmp4A5=0;Cyc_Tcutil_terr(loc,({const char*_tmp4A6="cannot assign to an array";
_tag_dyneither(_tmp4A6,sizeof(char),26);}),_tag_dyneither(_tmp4A5,sizeof(void*),
0));});goto _LL22D;_LL230:;_LL231: goto _LL22D;_LL22D:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1))({void*_tmp4A7=0;Cyc_Tcutil_terr(loc,({const char*_tmp4A8="Swap not allowed for non-pointer or non-word-sized types.";
_tag_dyneither(_tmp4A8,sizeof(char),58);}),_tag_dyneither(_tmp4A7,sizeof(void*),
0));});if(!Cyc_Absyn_is_lvalue(e1))return({void*_tmp4A9=0;Cyc_Tcexp_expr_err(te,
e1->loc,topt,({const char*_tmp4AA="swap non-lvalue";_tag_dyneither(_tmp4AA,
sizeof(char),16);}),_tag_dyneither(_tmp4A9,sizeof(void*),0));});if(!Cyc_Absyn_is_lvalue(
e2))return({void*_tmp4AB=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*
_tmp4AC="swap non-lvalue";_tag_dyneither(_tmp4AC,sizeof(char),16);}),
_tag_dyneither(_tmp4AB,sizeof(void*),0));});{void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;
void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(
e1,& t_ign1,& b_ign1,& t_ign2))return({void*_tmp4AD=0;Cyc_Tcexp_expr_err(te,e1->loc,
topt,({const char*_tmp4AE="swap value in zeroterm array";_tag_dyneither(_tmp4AE,
sizeof(char),29);}),_tag_dyneither(_tmp4AD,sizeof(void*),0));});if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2))return({void*_tmp4AF=0;Cyc_Tcexp_expr_err(te,e2->loc,
topt,({const char*_tmp4B0="swap value in zeroterm array";_tag_dyneither(_tmp4B0,
sizeof(char),29);}),_tag_dyneither(_tmp4AF,sizeof(void*),0));});Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){void*_tmp4B1=({
struct Cyc_String_pa_struct _tmp4B5;_tmp4B5.tag=0;_tmp4B5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_struct
_tmp4B4;_tmp4B4.tag=0;_tmp4B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));({void*_tmp4B2[2]={& _tmp4B4,& _tmp4B5};Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp4B3="type mismatch: %s != %s";_tag_dyneither(_tmp4B3,
sizeof(char),24);}),_tag_dyneither(_tmp4B2,sizeof(void*),2));});});});return
_tmp4B1;}return(void*)& Cyc_Absyn_VoidType_val;};};};}int Cyc_Tcexp_in_stmt_exp=0;
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){struct _RegionHandle*_tmp4B6=Cyc_Tcenv_get_fnrgn(
te);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
_tmp4B6,te,s),s,1);Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp4B7=s->r;struct Cyc_Absyn_Exp*
_tmp4B9;struct Cyc_Absyn_Stmt*_tmp4BB;struct Cyc_Absyn_Stmt*_tmp4BC;struct Cyc_Absyn_Decl*
_tmp4BE;struct Cyc_Absyn_Stmt*_tmp4BF;_LL233: {struct Cyc_Absyn_Exp_s_struct*
_tmp4B8=(struct Cyc_Absyn_Exp_s_struct*)_tmp4B7;if(_tmp4B8->tag != 1)goto _LL235;
else{_tmp4B9=_tmp4B8->f1;}}_LL234: {void*_tmp4C0=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B9->topt))->v;if(!Cyc_Tcutil_unify(_tmp4C0,Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te)))){({struct Cyc_String_pa_struct _tmp4C3;
_tmp4C3.tag=0;_tmp4C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp4C0));({void*_tmp4C1[1]={& _tmp4C3};Cyc_Tcutil_terr(loc,({const char*_tmp4C2="statement expression returns type %s";
_tag_dyneither(_tmp4C2,sizeof(char),37);}),_tag_dyneither(_tmp4C1,sizeof(void*),
1));});});Cyc_Tcutil_explain_failure();}return _tmp4C0;}_LL235: {struct Cyc_Absyn_Seq_s_struct*
_tmp4BA=(struct Cyc_Absyn_Seq_s_struct*)_tmp4B7;if(_tmp4BA->tag != 2)goto _LL237;
else{_tmp4BB=_tmp4BA->f1;_tmp4BC=_tmp4BA->f2;}}_LL236: s=_tmp4BC;continue;_LL237: {
struct Cyc_Absyn_Decl_s_struct*_tmp4BD=(struct Cyc_Absyn_Decl_s_struct*)_tmp4B7;
if(_tmp4BD->tag != 12)goto _LL239;else{_tmp4BE=_tmp4BD->f1;_tmp4BF=_tmp4BD->f2;}}
_LL238: s=_tmp4BF;continue;_LL239:;_LL23A: return({void*_tmp4C4=0;Cyc_Tcexp_expr_err(
te,loc,topt,({const char*_tmp4C5="statement expression must end with expression";
_tag_dyneither(_tmp4C5,sizeof(char),46);}),_tag_dyneither(_tmp4C4,sizeof(void*),
0));});_LL232:;}}static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(te,0,e));{void*_tmp4C6=t;struct Cyc_Absyn_AggrInfo
_tmp4C8;union Cyc_Absyn_AggrInfoU _tmp4C9;struct Cyc_Absyn_Aggrdecl**_tmp4CA;struct
Cyc_Absyn_Aggrdecl*_tmp4CB;_LL23C: {struct Cyc_Absyn_AggrType_struct*_tmp4C7=(
struct Cyc_Absyn_AggrType_struct*)_tmp4C6;if(_tmp4C7->tag != 12)goto _LL23E;else{
_tmp4C8=_tmp4C7->f1;_tmp4C9=_tmp4C8.aggr_info;if((_tmp4C9.KnownAggr).tag != 2)
goto _LL23E;_tmp4CA=(struct Cyc_Absyn_Aggrdecl**)(_tmp4C9.KnownAggr).val;_tmp4CB=*
_tmp4CA;}}_LL23D: if((_tmp4CB->kind == Cyc_Absyn_UnionA  && _tmp4CB->impl != 0) && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CB->impl))->tagged)goto _LL23B;
goto _LL23F;_LL23E:;_LL23F:({struct Cyc_String_pa_struct _tmp4CE;_tmp4CE.tag=0;
_tmp4CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({
void*_tmp4CC[1]={& _tmp4CE};Cyc_Tcutil_terr(loc,({const char*_tmp4CD="expecting @tagged union but found %s";
_tag_dyneither(_tmp4CD,sizeof(char),37);}),_tag_dyneither(_tmp4CC,sizeof(void*),
1));});});goto _LL23B;_LL23B:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(rgn_handle != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp4D6;_tmp4D6.tag=16;_tmp4D6.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp4D6;});_tmp4D5;});void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)
rgn_handle);void*_tmp4CF=Cyc_Tcutil_compress(handle_type);void*_tmp4D1;_LL241: {
struct Cyc_Absyn_RgnHandleType_struct*_tmp4D0=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4CF;if(_tmp4D0->tag != 16)goto _LL243;else{_tmp4D1=(void*)_tmp4D0->f1;}}_LL242:
rgn=_tmp4D1;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL240;_LL243:;_LL244:({
struct Cyc_String_pa_struct _tmp4D4;_tmp4D4.tag=0;_tmp4D4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));({void*_tmp4D2[1]={&
_tmp4D4};Cyc_Tcutil_terr(rgn_handle->loc,({const char*_tmp4D3="expecting region_t type but found %s";
_tag_dyneither(_tmp4D3,sizeof(char),37);}),_tag_dyneither(_tmp4D2,sizeof(void*),
1));});});goto _LL240;_LL240:;}else{if(topt != 0){void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn))rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{
void*_tmp4D7=e1->r;struct Cyc_Core_Opt*_tmp4DA;struct Cyc_List_List*_tmp4DB;struct
Cyc_List_List*_tmp4DD;union Cyc_Absyn_Cnst _tmp4DF;struct _dyneither_ptr _tmp4E0;
_LL246: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4D8=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4D7;if(_tmp4D8->tag != 28)goto _LL248;}_LL247: {void*_tmp4E1=Cyc_Tcexp_tcExpNoPromote(
te,topt,e1);void*_tmp4E2=Cyc_Tcutil_compress(_tmp4E1);struct Cyc_Absyn_ArrayInfo
_tmp4E4;void*_tmp4E5;struct Cyc_Absyn_Tqual _tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;
union Cyc_Absyn_Constraint*_tmp4E8;_LL251: {struct Cyc_Absyn_ArrayType_struct*
_tmp4E3=(struct Cyc_Absyn_ArrayType_struct*)_tmp4E2;if(_tmp4E3->tag != 9)goto
_LL253;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E4.elt_type;_tmp4E6=_tmp4E4.tq;
_tmp4E7=_tmp4E4.num_elts;_tmp4E8=_tmp4E4.zero_term;}}_LL252: {struct Cyc_Absyn_Exp*
bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp4E7);void*b;{void*_tmp4E9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);void*_tmp4EB;_LL256: {
struct Cyc_Absyn_TagType_struct*_tmp4EA=(struct Cyc_Absyn_TagType_struct*)_tmp4E9;
if(_tmp4EA->tag != 20)goto _LL258;else{_tmp4EB=(void*)_tmp4EA->f1;}}_LL257: b=(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp4ED;_tmp4ED.tag=1;_tmp4ED.f1=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp4EB,0),0,Cyc_Absyn_No_coercion,0);
_tmp4ED;});_tmp4EC;});goto _LL255;_LL258:;_LL259: if(Cyc_Tcutil_is_const_exp(te,
bnd))b=(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));
_tmp4EE[0]=({struct Cyc_Absyn_Upper_b_struct _tmp4EF;_tmp4EF.tag=1;_tmp4EF.f1=bnd;
_tmp4EF;});_tmp4EE;});else{b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL255:;}{void*
res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp4F1=_cycalloc(sizeof(*
_tmp4F1));_tmp4F1[0]=({struct Cyc_Absyn_PointerType_struct _tmp4F2;_tmp4F2.tag=5;
_tmp4F2.f1=({struct Cyc_Absyn_PtrInfo _tmp4F3;_tmp4F3.elt_typ=_tmp4E5;_tmp4F3.elt_tq=
_tmp4E6;_tmp4F3.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp4F4;_tmp4F4.rgn=rgn;
_tmp4F4.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmp4F4.bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b);
_tmp4F4.zero_term=_tmp4E8;_tmp4F4.ptrloc=0;_tmp4F4;});_tmp4F3;});_tmp4F2;});
_tmp4F1;});if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp4F0=_cycalloc(sizeof(*
_tmp4F0));_tmp4F0->v=res_typ;_tmp4F0;});Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL253:;_LL254:({void*_tmp4F5=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4F6="tcNew: comprehension returned non-array type";
_tag_dyneither(_tmp4F6,sizeof(char),45);}),_tag_dyneither(_tmp4F5,sizeof(void*),
0));});_LL250:;}_LL248: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp4D9=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmp4D7;if(_tmp4D9->tag != 36)goto _LL24A;else{
_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4D9->f2;}}_LL249: e1->r=(void*)({struct Cyc_Absyn_Array_e_struct*
_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7[0]=({struct Cyc_Absyn_Array_e_struct
_tmp4F8;_tmp4F8.tag=27;_tmp4F8.f1=_tmp4DB;_tmp4F8;});_tmp4F7;});_tmp4DD=_tmp4DB;
goto _LL24B;_LL24A: {struct Cyc_Absyn_Array_e_struct*_tmp4DC=(struct Cyc_Absyn_Array_e_struct*)
_tmp4D7;if(_tmp4DC->tag != 27)goto _LL24C;else{_tmp4DD=_tmp4DC->f1;}}_LL24B: {void**
elt_typ_opt=0;int zero_term=0;if(topt != 0){void*_tmp4F9=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmp4FB;void*_tmp4FC;void**_tmp4FD;struct Cyc_Absyn_Tqual
_tmp4FE;struct Cyc_Absyn_PtrAtts _tmp4FF;union Cyc_Absyn_Constraint*_tmp500;_LL25B: {
struct Cyc_Absyn_PointerType_struct*_tmp4FA=(struct Cyc_Absyn_PointerType_struct*)
_tmp4F9;if(_tmp4FA->tag != 5)goto _LL25D;else{_tmp4FB=_tmp4FA->f1;_tmp4FC=_tmp4FB.elt_typ;
_tmp4FD=(void**)&(_tmp4FA->f1).elt_typ;_tmp4FE=_tmp4FB.elt_tq;_tmp4FF=_tmp4FB.ptr_atts;
_tmp500=_tmp4FF.zero_term;}}_LL25C: elt_typ_opt=(void**)_tmp4FD;zero_term=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp500);goto _LL25A;
_LL25D:;_LL25E: goto _LL25A;_LL25A:;}{void*_tmp501=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp4DD);e1->topt=({struct Cyc_Core_Opt*_tmp502=_cycalloc(
sizeof(*_tmp502));_tmp502->v=_tmp501;_tmp502;});{void*res_typ;{void*_tmp503=Cyc_Tcutil_compress(
_tmp501);struct Cyc_Absyn_ArrayInfo _tmp505;void*_tmp506;struct Cyc_Absyn_Tqual
_tmp507;struct Cyc_Absyn_Exp*_tmp508;union Cyc_Absyn_Constraint*_tmp509;_LL260: {
struct Cyc_Absyn_ArrayType_struct*_tmp504=(struct Cyc_Absyn_ArrayType_struct*)
_tmp503;if(_tmp504->tag != 9)goto _LL262;else{_tmp505=_tmp504->f1;_tmp506=_tmp505.elt_type;
_tmp507=_tmp505.tq;_tmp508=_tmp505.num_elts;_tmp509=_tmp505.zero_term;}}_LL261:
res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp50A=_cycalloc(sizeof(*
_tmp50A));_tmp50A[0]=({struct Cyc_Absyn_PointerType_struct _tmp50B;_tmp50B.tag=5;
_tmp50B.f1=({struct Cyc_Absyn_PtrInfo _tmp50C;_tmp50C.elt_typ=_tmp506;_tmp50C.elt_tq=
_tmp507;_tmp50C.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp50D;_tmp50D.rgn=rgn;
_tmp50D.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmp50D.bounds=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp50E=_cycalloc(sizeof(*_tmp50E));
_tmp50E[0]=({struct Cyc_Absyn_Upper_b_struct _tmp50F;_tmp50F.tag=1;_tmp50F.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp508);_tmp50F;});_tmp50E;}));_tmp50D.zero_term=
_tmp509;_tmp50D.ptrloc=0;_tmp50D;});_tmp50C;});_tmp50B;});_tmp50A;});if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(te,loc,res_typ,*
topt)){e->topt=({struct Cyc_Core_Opt*_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510->v=
res_typ;_tmp510;});Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}goto _LL25F;_LL262:;_LL263:({void*_tmp511=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp512="tcExpNoPromote on Array_e returned non-array type";
_tag_dyneither(_tmp512,sizeof(char),50);}),_tag_dyneither(_tmp511,sizeof(void*),
0));});_LL25F:;}return res_typ;};};}_LL24C: {struct Cyc_Absyn_Const_e_struct*
_tmp4DE=(struct Cyc_Absyn_Const_e_struct*)_tmp4D7;if(_tmp4DE->tag != 0)goto _LL24E;
else{_tmp4DF=_tmp4DE->f1;if((_tmp4DF.String_c).tag != 7)goto _LL24E;_tmp4E0=(
struct _dyneither_ptr)(_tmp4DF.String_c).val;}}_LL24D: {void*_tmp513=Cyc_Absyn_atb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,
Cyc_Absyn_true_conref);void*_tmp514=Cyc_Tcexp_tcExp(te,(void**)& _tmp513,e1);
return Cyc_Absyn_atb_typ(_tmp514,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp516;_tmp516.tag=1;_tmp516.f1=Cyc_Absyn_uint_exp(1,0);_tmp516;});_tmp515;}),
Cyc_Absyn_false_conref);}_LL24E:;_LL24F: {void**topt2=0;if(topt != 0){void*
_tmp517=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp519;void*_tmp51A;
void**_tmp51B;struct Cyc_Absyn_Tqual _tmp51C;_LL265: {struct Cyc_Absyn_PointerType_struct*
_tmp518=(struct Cyc_Absyn_PointerType_struct*)_tmp517;if(_tmp518->tag != 5)goto
_LL267;else{_tmp519=_tmp518->f1;_tmp51A=_tmp519.elt_typ;_tmp51B=(void**)&(
_tmp518->f1).elt_typ;_tmp51C=_tmp519.elt_tq;}}_LL266: topt2=(void**)_tmp51B;goto
_LL264;_LL267: {struct Cyc_Absyn_DatatypeType_struct*_tmp51D=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp517;if(_tmp51D->tag != 3)goto _LL269;}_LL268: topt2=({void**_tmp51E=_cycalloc(
sizeof(*_tmp51E));_tmp51E[0]=*topt;_tmp51E;});goto _LL264;_LL269:;_LL26A: goto
_LL264;_LL264:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520[0]=({
struct Cyc_Absyn_PointerType_struct _tmp521;_tmp521.tag=5;_tmp521.f1=({struct Cyc_Absyn_PtrInfo
_tmp522;_tmp522.elt_typ=telt;_tmp522.elt_tq=Cyc_Absyn_empty_tqual(0);_tmp522.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp523;_tmp523.rgn=rgn;_tmp523.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmp523.bounds=Cyc_Absyn_bounds_one_conref;_tmp523.zero_term=
Cyc_Absyn_false_conref;_tmp523.ptrloc=0;_tmp523;});_tmp522;});_tmp521;});_tmp520;});
if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){e->topt=({struct Cyc_Core_Opt*_tmp51F=_cycalloc(sizeof(*
_tmp51F));_tmp51F->v=res_typ;_tmp51F;});Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}return res_typ;};}_LL245:;};}void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(
te,topt,e));void*_tmp524=t;struct Cyc_Absyn_ArrayInfo _tmp526;void*_tmp527;struct
Cyc_Absyn_Tqual _tmp528;struct Cyc_Absyn_Exp*_tmp529;union Cyc_Absyn_Constraint*
_tmp52A;_LL26C: {struct Cyc_Absyn_ArrayType_struct*_tmp525=(struct Cyc_Absyn_ArrayType_struct*)
_tmp524;if(_tmp525->tag != 9)goto _LL26E;else{_tmp526=_tmp525->f1;_tmp527=_tmp526.elt_type;
_tmp528=_tmp526.tq;_tmp529=_tmp526.num_elts;_tmp52A=_tmp526.zero_term;}}_LL26D: {
void*_tmp52C;struct _tuple12 _tmp52B=Cyc_Tcutil_addressof_props(te,e);_tmp52C=
_tmp52B.f2;{void*_tmp52D=_tmp529 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp52F;_tmp52F.tag=1;_tmp52F.f1=(struct Cyc_Absyn_Exp*)
_tmp529;_tmp52F;});_tmp52E;});t=Cyc_Absyn_atb_typ(_tmp527,_tmp52C,_tmp528,
_tmp52D,_tmp52A);((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t;return t;};}
_LL26E:;_LL26F: return t;_LL26B:;}void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
struct _RegionHandle*_tmp530=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp530,t) && !Cyc_Tcutil_is_noalias_path(_tmp530,e)){({struct Cyc_String_pa_struct
_tmp535;_tmp535.tag=0;_tmp535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));({struct Cyc_String_pa_struct _tmp534;_tmp534.tag=0;
_tmp534.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));({struct Cyc_String_pa_struct
_tmp533;_tmp533.tag=0;_tmp533.f1=(struct _dyneither_ptr)(topt == 0?({const char*
_tmp536="NULL";_tag_dyneither(_tmp536,sizeof(char),5);}):(struct _dyneither_ptr)
Cyc_Absynpp_typ2string(*topt));({void*_tmp531[3]={& _tmp533,& _tmp534,& _tmp535};
Cyc_fprintf(Cyc_stderr,({const char*_tmp532="topt=%s, e->topt->v=%s, e=%s\n";
_tag_dyneither(_tmp532,sizeof(char),30);}),_tag_dyneither(_tmp531,sizeof(void*),
3));});});});});({void*_tmp537=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp538="Cannot consume non-unique paths; do swap instead";
_tag_dyneither(_tmp538,sizeof(char),49);}),_tag_dyneither(_tmp537,sizeof(void*),
0));});}{void*_tmp539=e->r;union Cyc_Absyn_Cnst _tmp53D;struct _dyneither_ptr
_tmp53E;_LL271: {struct Cyc_Absyn_Array_e_struct*_tmp53A=(struct Cyc_Absyn_Array_e_struct*)
_tmp539;if(_tmp53A->tag != 27)goto _LL273;}_LL272: goto _LL274;_LL273: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp53B=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp539;if(_tmp53B->tag != 28)
goto _LL275;}_LL274: goto _LL276;_LL275: {struct Cyc_Absyn_Const_e_struct*_tmp53C=(
struct Cyc_Absyn_Const_e_struct*)_tmp539;if(_tmp53C->tag != 0)goto _LL277;else{
_tmp53D=_tmp53C->f1;if((_tmp53D.String_c).tag != 7)goto _LL277;_tmp53E=(struct
_dyneither_ptr)(_tmp53D.String_c).val;}}_LL276: return t;_LL277:;_LL278: t=Cyc_Tcutil_compress(
t);{void*_tmp53F=t;struct Cyc_Absyn_ArrayInfo _tmp541;void*_tmp542;struct Cyc_Absyn_Tqual
_tmp543;struct Cyc_Absyn_Exp*_tmp544;union Cyc_Absyn_Constraint*_tmp545;_LL27A: {
struct Cyc_Absyn_ArrayType_struct*_tmp540=(struct Cyc_Absyn_ArrayType_struct*)
_tmp53F;if(_tmp540->tag != 9)goto _LL27C;else{_tmp541=_tmp540->f1;_tmp542=_tmp541.elt_type;
_tmp543=_tmp541.tq;_tmp544=_tmp541.num_elts;_tmp545=_tmp541.zero_term;}}_LL27B: {
void*_tmp547;struct _tuple12 _tmp546=Cyc_Tcutil_addressof_props(te,e);_tmp547=
_tmp546.f2;{void*b=_tmp544 == 0?(void*)& Cyc_Absyn_DynEither_b_val:(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp549;_tmp549.tag=1;_tmp549.f1=(struct Cyc_Absyn_Exp*)
_tmp544;_tmp549;});_tmp548;});t=Cyc_Absyn_atb_typ(_tmp542,_tmp547,_tmp543,b,
_tmp545);Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);return t;};}
_LL27C:;_LL27D: return t;_LL279:;};_LL270:;};}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp54A=e->r;
struct Cyc_Absyn_Exp*_tmp54C;_LL27F: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp54B=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp54A;if(_tmp54B->tag != 13)
goto _LL281;else{_tmp54C=_tmp54B->f1;}}_LL280: Cyc_Tcexp_tcExpNoInst(te,topt,
_tmp54C);((struct Cyc_Core_Opt*)_check_null(_tmp54C->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp54C->topt))->v,0);e->topt=_tmp54C->topt;
goto _LL27E;_LL281:;_LL282: Cyc_Tcexp_tcExpNoInst(te,topt,e);((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,0);{void*_tmp54D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp54F;void*_tmp550;struct Cyc_Absyn_Tqual
_tmp551;struct Cyc_Absyn_PtrAtts _tmp552;void*_tmp553;union Cyc_Absyn_Constraint*
_tmp554;union Cyc_Absyn_Constraint*_tmp555;union Cyc_Absyn_Constraint*_tmp556;
_LL284: {struct Cyc_Absyn_PointerType_struct*_tmp54E=(struct Cyc_Absyn_PointerType_struct*)
_tmp54D;if(_tmp54E->tag != 5)goto _LL286;else{_tmp54F=_tmp54E->f1;_tmp550=_tmp54F.elt_typ;
_tmp551=_tmp54F.elt_tq;_tmp552=_tmp54F.ptr_atts;_tmp553=_tmp552.rgn;_tmp554=
_tmp552.nullable;_tmp555=_tmp552.bounds;_tmp556=_tmp552.zero_term;}}_LL285:{void*
_tmp557=Cyc_Tcutil_compress(_tmp550);struct Cyc_Absyn_FnInfo _tmp559;struct Cyc_List_List*
_tmp55A;struct Cyc_Core_Opt*_tmp55B;void*_tmp55C;struct Cyc_List_List*_tmp55D;int
_tmp55E;struct Cyc_Absyn_VarargInfo*_tmp55F;struct Cyc_List_List*_tmp560;struct Cyc_List_List*
_tmp561;_LL289: {struct Cyc_Absyn_FnType_struct*_tmp558=(struct Cyc_Absyn_FnType_struct*)
_tmp557;if(_tmp558->tag != 10)goto _LL28B;else{_tmp559=_tmp558->f1;_tmp55A=_tmp559.tvars;
_tmp55B=_tmp559.effect;_tmp55C=_tmp559.ret_typ;_tmp55D=_tmp559.args;_tmp55E=
_tmp559.c_varargs;_tmp55F=_tmp559.cyc_varargs;_tmp560=_tmp559.rgn_po;_tmp561=
_tmp559.attributes;}}_LL28A: if(_tmp55A != 0){struct _RegionHandle*_tmp562=Cyc_Tcenv_get_fnrgn(
te);struct _tuple10 _tmp563=({struct _tuple10 _tmp570;_tmp570.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp570.f2=_tmp562;_tmp570;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),
struct _tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp562,Cyc_Tcutil_r_make_inst_var,&
_tmp563,_tmp55A);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple11*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(_tmp562,inst,(void*)({struct
Cyc_Absyn_FnType_struct*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D[0]=({struct
Cyc_Absyn_FnType_struct _tmp56E;_tmp56E.tag=10;_tmp56E.f1=({struct Cyc_Absyn_FnInfo
_tmp56F;_tmp56F.tvars=0;_tmp56F.effect=_tmp55B;_tmp56F.ret_typ=_tmp55C;_tmp56F.args=
_tmp55D;_tmp56F.c_varargs=_tmp55E;_tmp56F.cyc_varargs=_tmp55F;_tmp56F.rgn_po=
_tmp560;_tmp56F.attributes=_tmp561;_tmp56F;});_tmp56E;});_tmp56D;}));struct Cyc_Absyn_PointerType_struct*
_tmp564=({struct Cyc_Absyn_PointerType_struct*_tmp569=_cycalloc(sizeof(*_tmp569));
_tmp569[0]=({struct Cyc_Absyn_PointerType_struct _tmp56A;_tmp56A.tag=5;_tmp56A.f1=({
struct Cyc_Absyn_PtrInfo _tmp56B;_tmp56B.elt_typ=ftyp;_tmp56B.elt_tq=_tmp551;
_tmp56B.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp56C;_tmp56C.rgn=_tmp553;_tmp56C.nullable=
_tmp554;_tmp56C.bounds=_tmp555;_tmp56C.zero_term=_tmp556;_tmp56C.ptrloc=0;
_tmp56C;});_tmp56B;});_tmp56A;});_tmp569;});struct Cyc_Absyn_Exp*_tmp565=Cyc_Absyn_copy_exp(
e);e->r=(void*)({struct Cyc_Absyn_Instantiate_e_struct*_tmp566=_cycalloc(sizeof(*
_tmp566));_tmp566[0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp567;_tmp567.tag=
14;_tmp567.f1=_tmp565;_tmp567.f2=ts;_tmp567;});_tmp566;});e->topt=({struct Cyc_Core_Opt*
_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->v=(void*)_tmp564;_tmp568;});}goto
_LL288;_LL28B:;_LL28C: goto _LL288;_LL288:;}goto _LL283;_LL286:;_LL287: goto _LL283;
_LL283:;}goto _LL27E;_LL27E:;}return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*
e){struct Cyc_Position_Segment*loc=e->loc;void*t;{void*_tmp571=e->r;struct Cyc_Absyn_Exp*
_tmp573;struct _tuple1*_tmp575;struct Cyc_Absyn_Exp*_tmp577;struct Cyc_List_List*
_tmp578;struct Cyc_Core_Opt*_tmp57A;struct Cyc_List_List*_tmp57B;union Cyc_Absyn_Cnst
_tmp57D;union Cyc_Absyn_Cnst*_tmp57E;struct _tuple1*_tmp580;void*_tmp581;enum Cyc_Absyn_Primop
_tmp583;struct Cyc_List_List*_tmp584;struct Cyc_Absyn_Exp*_tmp586;enum Cyc_Absyn_Incrementor
_tmp587;struct Cyc_Absyn_Exp*_tmp589;struct Cyc_Core_Opt*_tmp58A;struct Cyc_Absyn_Exp*
_tmp58B;struct Cyc_Absyn_Exp*_tmp58D;struct Cyc_Absyn_Exp*_tmp58E;struct Cyc_Absyn_Exp*
_tmp58F;struct Cyc_Absyn_Exp*_tmp591;struct Cyc_Absyn_Exp*_tmp592;struct Cyc_Absyn_Exp*
_tmp594;struct Cyc_Absyn_Exp*_tmp595;struct Cyc_Absyn_Exp*_tmp597;struct Cyc_Absyn_Exp*
_tmp598;struct Cyc_Absyn_Exp*_tmp59A;struct Cyc_List_List*_tmp59B;struct Cyc_Absyn_VarargCallInfo*
_tmp59C;struct Cyc_Absyn_VarargCallInfo**_tmp59D;struct Cyc_Absyn_Exp*_tmp59F;
struct Cyc_Absyn_Exp*_tmp5A1;struct Cyc_List_List*_tmp5A2;void*_tmp5A4;struct Cyc_Absyn_Exp*
_tmp5A5;enum Cyc_Absyn_Coercion _tmp5A6;enum Cyc_Absyn_Coercion*_tmp5A7;struct Cyc_Absyn_Exp*
_tmp5A9;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Exp*
_tmp5AE;void*_tmp5B0;void*_tmp5B2;void*_tmp5B3;struct Cyc_Absyn_Exp*_tmp5B5;
struct Cyc_Absyn_Exp*_tmp5B7;struct _dyneither_ptr*_tmp5B8;int _tmp5B9;int*_tmp5BA;
int _tmp5BB;int*_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BE;struct _dyneither_ptr*_tmp5BF;
int _tmp5C0;int*_tmp5C1;int _tmp5C2;int*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C5;struct
Cyc_Absyn_Exp*_tmp5C6;struct Cyc_List_List*_tmp5C8;struct _tuple8*_tmp5CA;struct
Cyc_List_List*_tmp5CB;struct Cyc_List_List*_tmp5CD;struct Cyc_Absyn_Stmt*_tmp5CF;
struct Cyc_Absyn_Vardecl*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_Absyn_Exp*
_tmp5D3;int _tmp5D4;int*_tmp5D5;struct _tuple1*_tmp5D7;struct _tuple1**_tmp5D8;
struct Cyc_List_List*_tmp5D9;struct Cyc_List_List**_tmp5DA;struct Cyc_List_List*
_tmp5DB;struct Cyc_Absyn_Aggrdecl*_tmp5DC;struct Cyc_Absyn_Aggrdecl**_tmp5DD;void*
_tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*_tmp5E2;struct Cyc_Absyn_Datatypedecl*
_tmp5E3;struct Cyc_Absyn_Datatypefield*_tmp5E4;struct _tuple1*_tmp5E6;struct
_tuple1**_tmp5E7;struct Cyc_Absyn_Enumdecl*_tmp5E8;struct Cyc_Absyn_Enumfield*
_tmp5E9;struct _tuple1*_tmp5EB;struct _tuple1**_tmp5EC;void*_tmp5ED;struct Cyc_Absyn_Enumfield*
_tmp5EE;struct Cyc_Absyn_MallocInfo _tmp5F0;int _tmp5F1;int*_tmp5F2;struct Cyc_Absyn_Exp*
_tmp5F3;void**_tmp5F4;void***_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F6;struct Cyc_Absyn_Exp**
_tmp5F7;int _tmp5F8;int*_tmp5F9;struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_Absyn_Exp*
_tmp5FC;struct Cyc_Absyn_Exp*_tmp5FE;struct _dyneither_ptr*_tmp5FF;void*_tmp601;
_LL28E: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp572=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp571;if(_tmp572->tag != 13)goto _LL290;else{_tmp573=_tmp572->f1;}}_LL28F: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp573);return;_LL290: {struct Cyc_Absyn_UnknownId_e_struct*_tmp574=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp571;if(_tmp574->tag != 2)goto _LL292;else{
_tmp575=_tmp574->f1;}}_LL291: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp575);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL292: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp576=(struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp571;if(_tmp576->tag != 10)goto _LL294;else{
_tmp577=_tmp576->f1;_tmp578=_tmp576->f2;}}_LL293: Cyc_Tcexp_resolve_unknown_fn(te,
e,_tmp577,_tmp578);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL294: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp579=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp571;if(_tmp579->tag != 36)
goto _LL296;else{_tmp57A=_tmp579->f1;_tmp57B=_tmp579->f2;}}_LL295: Cyc_Tcexp_resolve_unresolved_mem(
te,loc,topt,e,_tmp57B);Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL296: {struct
Cyc_Absyn_Const_e_struct*_tmp57C=(struct Cyc_Absyn_Const_e_struct*)_tmp571;if(
_tmp57C->tag != 0)goto _LL298;else{_tmp57D=_tmp57C->f1;_tmp57E=(union Cyc_Absyn_Cnst*)&
_tmp57C->f1;}}_LL297: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst*)
_tmp57E,e);goto _LL28D;_LL298: {struct Cyc_Absyn_Var_e_struct*_tmp57F=(struct Cyc_Absyn_Var_e_struct*)
_tmp571;if(_tmp57F->tag != 1)goto _LL29A;else{_tmp580=_tmp57F->f1;_tmp581=(void*)
_tmp57F->f2;}}_LL299: t=Cyc_Tcexp_tcVar(te,loc,e,_tmp580,_tmp581);goto _LL28D;
_LL29A: {struct Cyc_Absyn_Primop_e_struct*_tmp582=(struct Cyc_Absyn_Primop_e_struct*)
_tmp571;if(_tmp582->tag != 3)goto _LL29C;else{_tmp583=_tmp582->f1;_tmp584=_tmp582->f2;}}
_LL29B: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp583,_tmp584);goto _LL28D;_LL29C: {
struct Cyc_Absyn_Increment_e_struct*_tmp585=(struct Cyc_Absyn_Increment_e_struct*)
_tmp571;if(_tmp585->tag != 5)goto _LL29E;else{_tmp586=_tmp585->f1;_tmp587=_tmp585->f2;}}
_LL29D: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp586,_tmp587);goto _LL28D;_LL29E: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp588=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp571;if(_tmp588->tag != 4)goto _LL2A0;else{_tmp589=_tmp588->f1;_tmp58A=_tmp588->f2;
_tmp58B=_tmp588->f3;}}_LL29F: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp589,_tmp58A,
_tmp58B);goto _LL28D;_LL2A0: {struct Cyc_Absyn_Conditional_e_struct*_tmp58C=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp571;if(_tmp58C->tag != 6)goto _LL2A2;
else{_tmp58D=_tmp58C->f1;_tmp58E=_tmp58C->f2;_tmp58F=_tmp58C->f3;}}_LL2A1: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp58D,_tmp58E,_tmp58F);goto _LL28D;_LL2A2: {struct Cyc_Absyn_And_e_struct*
_tmp590=(struct Cyc_Absyn_And_e_struct*)_tmp571;if(_tmp590->tag != 7)goto _LL2A4;
else{_tmp591=_tmp590->f1;_tmp592=_tmp590->f2;}}_LL2A3: t=Cyc_Tcexp_tcAnd(te,loc,
_tmp591,_tmp592);goto _LL28D;_LL2A4: {struct Cyc_Absyn_Or_e_struct*_tmp593=(struct
Cyc_Absyn_Or_e_struct*)_tmp571;if(_tmp593->tag != 8)goto _LL2A6;else{_tmp594=
_tmp593->f1;_tmp595=_tmp593->f2;}}_LL2A5: t=Cyc_Tcexp_tcOr(te,loc,_tmp594,_tmp595);
goto _LL28D;_LL2A6: {struct Cyc_Absyn_SeqExp_e_struct*_tmp596=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp571;if(_tmp596->tag != 9)goto _LL2A8;else{_tmp597=_tmp596->f1;_tmp598=_tmp596->f2;}}
_LL2A7: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp597,_tmp598);goto _LL28D;_LL2A8: {
struct Cyc_Absyn_FnCall_e_struct*_tmp599=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp571;if(_tmp599->tag != 11)goto _LL2AA;else{_tmp59A=_tmp599->f1;_tmp59B=_tmp599->f2;
_tmp59C=_tmp599->f3;_tmp59D=(struct Cyc_Absyn_VarargCallInfo**)& _tmp599->f3;}}
_LL2A9: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp59A,_tmp59B,_tmp59D);goto _LL28D;
_LL2AA: {struct Cyc_Absyn_Throw_e_struct*_tmp59E=(struct Cyc_Absyn_Throw_e_struct*)
_tmp571;if(_tmp59E->tag != 12)goto _LL2AC;else{_tmp59F=_tmp59E->f1;}}_LL2AB: t=Cyc_Tcexp_tcThrow(
te,loc,topt,_tmp59F);goto _LL28D;_LL2AC: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp5A0=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp571;if(_tmp5A0->tag != 14)goto
_LL2AE;else{_tmp5A1=_tmp5A0->f1;_tmp5A2=_tmp5A0->f2;}}_LL2AD: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp5A1,_tmp5A2);goto _LL28D;_LL2AE: {struct Cyc_Absyn_Cast_e_struct*
_tmp5A3=(struct Cyc_Absyn_Cast_e_struct*)_tmp571;if(_tmp5A3->tag != 15)goto _LL2B0;
else{_tmp5A4=(void*)_tmp5A3->f1;_tmp5A5=_tmp5A3->f2;_tmp5A6=_tmp5A3->f4;_tmp5A7=(
enum Cyc_Absyn_Coercion*)& _tmp5A3->f4;}}_LL2AF: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp5A4,_tmp5A5,(enum Cyc_Absyn_Coercion*)_tmp5A7);goto _LL28D;_LL2B0: {struct Cyc_Absyn_Address_e_struct*
_tmp5A8=(struct Cyc_Absyn_Address_e_struct*)_tmp571;if(_tmp5A8->tag != 16)goto
_LL2B2;else{_tmp5A9=_tmp5A8->f1;}}_LL2B1: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,
_tmp5A9);goto _LL28D;_LL2B2: {struct Cyc_Absyn_New_e_struct*_tmp5AA=(struct Cyc_Absyn_New_e_struct*)
_tmp571;if(_tmp5AA->tag != 17)goto _LL2B4;else{_tmp5AB=_tmp5AA->f1;_tmp5AC=_tmp5AA->f2;}}
_LL2B3: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp5AB,e,_tmp5AC);goto _LL28D;_LL2B4: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp5AD=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp571;if(_tmp5AD->tag != 19)goto _LL2B6;else{_tmp5AE=_tmp5AD->f1;}}_LL2B5: {void*
_tmp602=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp5AE);t=Cyc_Tcexp_tcSizeof(te,loc,topt,
_tmp602);goto _LL28D;}_LL2B6: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp5AF=(struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp571;if(_tmp5AF->tag != 18)goto _LL2B8;else{
_tmp5B0=(void*)_tmp5AF->f1;}}_LL2B7: t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp5B0);
goto _LL28D;_LL2B8: {struct Cyc_Absyn_Offsetof_e_struct*_tmp5B1=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp571;if(_tmp5B1->tag != 20)goto _LL2BA;else{_tmp5B2=(void*)_tmp5B1->f1;_tmp5B3=(
void*)_tmp5B1->f2;}}_LL2B9: t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp5B2,_tmp5B3);
goto _LL28D;_LL2BA: {struct Cyc_Absyn_Deref_e_struct*_tmp5B4=(struct Cyc_Absyn_Deref_e_struct*)
_tmp571;if(_tmp5B4->tag != 21)goto _LL2BC;else{_tmp5B5=_tmp5B4->f1;}}_LL2BB: t=Cyc_Tcexp_tcDeref(
te,loc,topt,_tmp5B5);goto _LL28D;_LL2BC: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp5B6=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp571;if(_tmp5B6->tag != 22)goto
_LL2BE;else{_tmp5B7=_tmp5B6->f1;_tmp5B8=_tmp5B6->f2;_tmp5B9=_tmp5B6->f3;_tmp5BA=(
int*)& _tmp5B6->f3;_tmp5BB=_tmp5B6->f4;_tmp5BC=(int*)& _tmp5B6->f4;}}_LL2BD: t=Cyc_Tcexp_tcAggrMember(
te,loc,topt,e,_tmp5B7,_tmp5B8,_tmp5BA,_tmp5BC);goto _LL28D;_LL2BE: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp5BD=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp571;if(_tmp5BD->tag != 23)goto
_LL2C0;else{_tmp5BE=_tmp5BD->f1;_tmp5BF=_tmp5BD->f2;_tmp5C0=_tmp5BD->f3;_tmp5C1=(
int*)& _tmp5BD->f3;_tmp5C2=_tmp5BD->f4;_tmp5C3=(int*)& _tmp5BD->f4;}}_LL2BF: t=Cyc_Tcexp_tcAggrArrow(
te,loc,topt,_tmp5BE,_tmp5BF,_tmp5C1,_tmp5C3);goto _LL28D;_LL2C0: {struct Cyc_Absyn_Subscript_e_struct*
_tmp5C4=(struct Cyc_Absyn_Subscript_e_struct*)_tmp571;if(_tmp5C4->tag != 24)goto
_LL2C2;else{_tmp5C5=_tmp5C4->f1;_tmp5C6=_tmp5C4->f2;}}_LL2C1: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp5C5,_tmp5C6);goto _LL28D;_LL2C2: {struct Cyc_Absyn_Tuple_e_struct*
_tmp5C7=(struct Cyc_Absyn_Tuple_e_struct*)_tmp571;if(_tmp5C7->tag != 25)goto _LL2C4;
else{_tmp5C8=_tmp5C7->f1;}}_LL2C3: t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp5C8);goto
_LL28D;_LL2C4: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp5C9=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp571;if(_tmp5C9->tag != 26)goto _LL2C6;else{_tmp5CA=_tmp5C9->f1;_tmp5CB=_tmp5C9->f2;}}
_LL2C5: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp5CA,_tmp5CB);goto _LL28D;_LL2C6: {
struct Cyc_Absyn_Array_e_struct*_tmp5CC=(struct Cyc_Absyn_Array_e_struct*)_tmp571;
if(_tmp5CC->tag != 27)goto _LL2C8;else{_tmp5CD=_tmp5CC->f1;}}_LL2C7: {void**
elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp603=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_ArrayInfo _tmp605;void*_tmp606;void**_tmp607;union Cyc_Absyn_Constraint*
_tmp608;_LL2DF: {struct Cyc_Absyn_ArrayType_struct*_tmp604=(struct Cyc_Absyn_ArrayType_struct*)
_tmp603;if(_tmp604->tag != 9)goto _LL2E1;else{_tmp605=_tmp604->f1;_tmp606=_tmp605.elt_type;
_tmp607=(void**)&(_tmp604->f1).elt_type;_tmp608=_tmp605.zero_term;}}_LL2E0:
elt_topt=(void**)_tmp607;zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmp608);goto _LL2DE;_LL2E1:;_LL2E2: goto _LL2DE;_LL2DE:;}t=
Cyc_Tcexp_tcArray(te,loc,elt_topt,zero_term,_tmp5CD);goto _LL28D;}_LL2C8: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp5CE=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp571;
if(_tmp5CE->tag != 37)goto _LL2CA;else{_tmp5CF=_tmp5CE->f1;}}_LL2C9: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp5CF);goto _LL28D;_LL2CA: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp5D0=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp571;if(_tmp5D0->tag != 28)
goto _LL2CC;else{_tmp5D1=_tmp5D0->f1;_tmp5D2=_tmp5D0->f2;_tmp5D3=_tmp5D0->f3;
_tmp5D4=_tmp5D0->f4;_tmp5D5=(int*)& _tmp5D0->f4;}}_LL2CB: t=Cyc_Tcexp_tcComprehension(
te,loc,topt,_tmp5D1,_tmp5D2,_tmp5D3,_tmp5D5);goto _LL28D;_LL2CC: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp5D6=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp571;if(_tmp5D6->tag != 29)goto
_LL2CE;else{_tmp5D7=_tmp5D6->f1;_tmp5D8=(struct _tuple1**)& _tmp5D6->f1;_tmp5D9=
_tmp5D6->f2;_tmp5DA=(struct Cyc_List_List**)& _tmp5D6->f2;_tmp5DB=_tmp5D6->f3;
_tmp5DC=_tmp5D6->f4;_tmp5DD=(struct Cyc_Absyn_Aggrdecl**)& _tmp5D6->f4;}}_LL2CD: t=
Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp5D8,_tmp5DA,_tmp5DB,_tmp5DD);goto _LL28D;
_LL2CE: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp5DE=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp571;if(_tmp5DE->tag != 30)goto _LL2D0;else{_tmp5DF=(void*)_tmp5DE->f1;_tmp5E0=
_tmp5DE->f2;}}_LL2CF: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp5DF,_tmp5E0);goto _LL28D;
_LL2D0: {struct Cyc_Absyn_Datatype_e_struct*_tmp5E1=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp571;if(_tmp5E1->tag != 31)goto _LL2D2;else{_tmp5E2=_tmp5E1->f1;_tmp5E3=_tmp5E1->f2;
_tmp5E4=_tmp5E1->f3;}}_LL2D1: t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp5E2,
_tmp5E3,_tmp5E4);goto _LL28D;_LL2D2: {struct Cyc_Absyn_Enum_e_struct*_tmp5E5=(
struct Cyc_Absyn_Enum_e_struct*)_tmp571;if(_tmp5E5->tag != 32)goto _LL2D4;else{
_tmp5E6=_tmp5E5->f1;_tmp5E7=(struct _tuple1**)& _tmp5E5->f1;_tmp5E8=_tmp5E5->f2;
_tmp5E9=_tmp5E5->f3;}}_LL2D3:*_tmp5E7=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp5E9))->name;t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp609=_cycalloc(
sizeof(*_tmp609));_tmp609[0]=({struct Cyc_Absyn_EnumType_struct _tmp60A;_tmp60A.tag=
14;_tmp60A.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp5E8))->name;_tmp60A.f2=
_tmp5E8;_tmp60A;});_tmp609;});goto _LL28D;_LL2D4: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp5EA=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp571;if(_tmp5EA->tag != 33)goto
_LL2D6;else{_tmp5EB=_tmp5EA->f1;_tmp5EC=(struct _tuple1**)& _tmp5EA->f1;_tmp5ED=(
void*)_tmp5EA->f2;_tmp5EE=_tmp5EA->f3;}}_LL2D5:*_tmp5EC=((struct Cyc_Absyn_Enumfield*)
_check_null(_tmp5EE))->name;t=_tmp5ED;goto _LL28D;_LL2D6: {struct Cyc_Absyn_Malloc_e_struct*
_tmp5EF=(struct Cyc_Absyn_Malloc_e_struct*)_tmp571;if(_tmp5EF->tag != 34)goto
_LL2D8;else{_tmp5F0=_tmp5EF->f1;_tmp5F1=_tmp5F0.is_calloc;_tmp5F2=(int*)&(
_tmp5EF->f1).is_calloc;_tmp5F3=_tmp5F0.rgn;_tmp5F4=_tmp5F0.elt_type;_tmp5F5=(
void***)&(_tmp5EF->f1).elt_type;_tmp5F6=_tmp5F0.num_elts;_tmp5F7=(struct Cyc_Absyn_Exp**)&(
_tmp5EF->f1).num_elts;_tmp5F8=_tmp5F0.fat_result;_tmp5F9=(int*)&(_tmp5EF->f1).fat_result;}}
_LL2D7: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp5F3,_tmp5F5,_tmp5F7,_tmp5F2,_tmp5F9);
goto _LL28D;_LL2D8: {struct Cyc_Absyn_Swap_e_struct*_tmp5FA=(struct Cyc_Absyn_Swap_e_struct*)
_tmp571;if(_tmp5FA->tag != 35)goto _LL2DA;else{_tmp5FB=_tmp5FA->f1;_tmp5FC=_tmp5FA->f2;}}
_LL2D9: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp5FB,_tmp5FC);goto _LL28D;_LL2DA: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp5FD=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp571;if(_tmp5FD->tag != 38)goto _LL2DC;else{_tmp5FE=_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;}}
_LL2DB: t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp5FE,_tmp5FF);goto _LL28D;_LL2DC: {
struct Cyc_Absyn_Valueof_e_struct*_tmp600=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp571;if(_tmp600->tag != 39)goto _LL28D;else{_tmp601=(void*)_tmp600->f1;}}_LL2DD:
if(!te->allow_valueof)({void*_tmp60B=0;Cyc_Tcutil_terr(e->loc,({const char*
_tmp60C="valueof(-) can only occur within types";_tag_dyneither(_tmp60C,sizeof(
char),39);}),_tag_dyneither(_tmp60B,sizeof(void*),0));});t=Cyc_Absyn_sint_typ;
goto _LL28D;_LL28D:;}e->topt=({struct Cyc_Core_Opt*_tmp60D=_cycalloc(sizeof(*
_tmp60D));_tmp60D->v=t;_tmp60D;});}
