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
struct Cyc_Core_NewRegion Cyc_Core_new_dynregion();extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_struct{char*tag;};void Cyc_Core_free_dynregion(
struct _DynRegionHandle*);typedef struct{int __count;union{unsigned int __wch;char
__wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t
__state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern
struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);void Cyc_List_iter_c(void(*f)(void*,void*),void*env,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,
int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_strcmp(
struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct
_RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(struct
_RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;
struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct _dyneither_ptr
elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);void*Cyc_Xarray_get(
struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
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
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_VoidType_struct{int tag;};struct Cyc_Absyn_Evar_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;}
;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;enum 
Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_struct{int
tag;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
extern struct Cyc_Absyn_HeapRgn_struct Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_VoidType_struct
Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{int tag;
struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft
 = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{
int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};struct Cyc_Absyn_Fastcall_att_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_struct{int tag;};struct Cyc_Absyn_Const_att_struct{
int tag;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{
int tag;};struct Cyc_Absyn_Section_att_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Nocommon_att_struct{int tag;};struct Cyc_Absyn_Shared_att_struct{
int tag;};struct Cyc_Absyn_Unused_att_struct{int tag;};struct Cyc_Absyn_Weak_att_struct{
int tag;};struct Cyc_Absyn_Dllimport_att_struct{int tag;};struct Cyc_Absyn_Dllexport_att_struct{
int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{int tag;};struct Cyc_Absyn_Constructor_att_struct{
int tag;};struct Cyc_Absyn_Destructor_att_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{
int tag;};struct Cyc_Absyn_Format_att_struct{int tag;enum Cyc_Absyn_Format_Type f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};struct
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union
Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*f3;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};
struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple7 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,
char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);union Cyc_Absyn_Cnst
Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(
enum Cyc_Absyn_Sign,long long);union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct
_dyneither_ptr);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus
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
f2;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_Skip_s_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;
struct Cyc_Position_Segment*loc;struct Cyc_List_List*non_local_preds;int try_depth;
void*annot;};struct Cyc_Absyn_Wild_p_struct{int tag;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct
Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_struct{int tag;};struct Cyc_Absyn_Int_p_struct{
int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;
};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern struct Cyc_Absyn_Unresolved_b_struct
Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;
struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*
body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*
fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct
_dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*
exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};
struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;void*Cyc_Absyn_exn_typ();extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(struct
_tuple1*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(
void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _dyneither_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;
struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct
Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_NotLoop_j_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_struct{
int tag;};struct Cyc_Tcenv_FnEnd_j_struct{int tag;};struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,enum Cyc_Absyn_Kind def);enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(
void*t);int Cyc_Tcutil_unify(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple10*);struct _tuple11{
unsigned int f1;int f2;};struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct Cyc_List_List*
path;};enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL
 = 1,Cyc_CfFlowInfo_AllIL  = 2};struct _union_RelnOp_EqualConst{int tag;
unsigned int val;};struct _tuple12{struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
_union_RelnOp_LessVar{int tag;struct _tuple12 val;};struct _union_RelnOp_LessNumelts{
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
int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*
notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;
void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct
Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_UniqueRgn_k_struct{
int tag;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**datatypes_so_far;
struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple14{
struct _tuple1*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple14*
x,struct _tuple14*y){struct _tuple1*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple14
_tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple1*_tmp4;struct _dyneither_ptr
_tmp5;struct _tuple14 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(
_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(
struct _dyneither_ptr)_tmp5);}}}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*
f2;};void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState
_tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;
_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct _RegionHandle*d=
_open_dynregion(& _tmp9,_tmp7);{struct _tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict
d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8,q);if(v == 0){void*_tmpA=
type_maker(q);_npop_handler(0);return _tmpA;}else{struct _tuple15 _tmpC;void*_tmpD;
struct _tuple15*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{void*_tmpE=_tmpD;
_npop_handler(0);return _tmpE;}}};_pop_dynregion(d);}}static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({void*_tmpF=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp10="\n";_tag_dyneither(_tmp10,sizeof(char),2);}),_tag_dyneither(
_tmpF,sizeof(void*),0));});Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)
_throw((void*)({struct Cyc_Toc_Toc_Unimplemented_struct*_tmp11=_cycalloc_atomic(
sizeof(*_tmp11));_tmp11[0]=({struct Cyc_Toc_Toc_Unimplemented_struct _tmp12;_tmp12.tag=
Cyc_Toc_Toc_Unimplemented;_tmp12;});_tmp11;}));}static void*Cyc_Toc_toc_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmp13=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp14="\n";_tag_dyneither(
_tmp14,sizeof(char),2);}),_tag_dyneither(_tmp13,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)({struct Cyc_Toc_Toc_Impossible_struct*
_tmp15=_cycalloc_atomic(sizeof(*_tmp15));_tmp15[0]=({struct Cyc_Toc_Toc_Impossible_struct
_tmp16;_tmp16.tag=Cyc_Toc_Toc_Impossible;_tmp16;});_tmp15;}));}char Cyc_Toc_Match_error[
12]="Match_error";struct Cyc_Toc_Match_error_struct{char*tag;};static char _tmp17[5]="curr";
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp17,_tmp17,_tmp17 + 5};static
struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp18[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp18,_tmp18,_tmp18 + 4};static
struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp19[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp19,_tmp19,_tmp19 + 4};static
struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp1A[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp1A,_tmp1A,_tmp1A + 14};
static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp1B[8]="handler";static struct _dyneither_ptr Cyc_Toc_handler_string={
_tmp1B,_tmp1B,_tmp1B + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp1C[14]="_RegionHandle";static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={
_tmp1C,_tmp1C,_tmp1C + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string;static char _tmp1D[17]="_DynRegionHandle";static
struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp1D,_tmp1D,_tmp1D + 17};
static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmp1E[16]="_DynRegionFrame";static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={
_tmp1E,_tmp1E,_tmp1E + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=&
Cyc_Toc__DynRegionFrame_string;struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(
void*)0,(void*)(0 + 0)};static char _tmp1F[7]="_throw";static struct _dyneither_ptr
Cyc_Toc__throw_str={_tmp1F,_tmp1F,_tmp1F + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={
3,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={
1,& Cyc_Toc__throw_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct
Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7};static
struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={3,0}},& Cyc_Toc_setjmp_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)&
Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)&
Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*
Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp23[14]="_push_handler";static
struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp23,_tmp23,_tmp23 + 14};static
struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={3,0}},& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={
0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char
_tmp25[13]="_pop_handler";static struct _dyneither_ptr Cyc_Toc__pop_handler_str={
_tmp25,_tmp25,_tmp25 + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={3,
0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=&
Cyc_Toc__pop_handler_ev;static char _tmp27[12]="_exn_thrown";static struct
_dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp27,_tmp27,_tmp27 + 12};static struct
_tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={3,0}},& Cyc_Toc__exn_thrown_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char
_tmp29[14]="_npop_handler";static struct _dyneither_ptr Cyc_Toc__npop_handler_str={
_tmp29,_tmp29,_tmp29 + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={
3,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev;static char _tmp2B[12]="_check_null";static struct
_dyneither_ptr Cyc_Toc__check_null_str={_tmp2B,_tmp2B,_tmp2B + 12};static struct
_tuple1 Cyc_Toc__check_null_pr={{.Loc_n={3,0}},& Cyc_Toc__check_null_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={
0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char
_tmp2D[28]="_check_known_subscript_null";static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={
_tmp2D,_tmp2D,_tmp2D + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={
3,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp2F[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp2F,_tmp2F,_tmp2F + 31};static
struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={3,0}},& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)&
Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;
static char _tmp31[27]="_check_dyneither_subscript";static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp31,_tmp31,_tmp31 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={
3,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp33[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp33,_tmp33,_tmp33 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=&
Cyc_Toc__dyneither_ptr_ev;static char _tmp35[15]="_tag_dyneither";static struct
_dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp35,_tmp35,_tmp35 + 15};static struct
_tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={3,0}},& Cyc_Toc__tag_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static
char _tmp37[20]="_init_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp37,_tmp37,_tmp37 + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={
3,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={
1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=&
Cyc_Toc__init_dyneither_ptr_ev;static char _tmp39[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp39,_tmp39,_tmp39
+ 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={3,0}},& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={
0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp3B[20]="_get_dyneither_size";static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={
_tmp3B,_tmp3B,_tmp3B + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={
1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=&
Cyc_Toc__get_dyneither_size_ev;static char _tmp3D[19]="_get_zero_arr_size";static
struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3D,_tmp3D,_tmp3D + 19};
static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={3,0}},& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp3F[24]="_get_zero_arr_size_char";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp3F,_tmp3F,_tmp3F + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp41[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp41,_tmp41,_tmp41 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp43[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp43,_tmp43,_tmp43 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={
1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=&
Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp45[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp45,_tmp45,
_tmp45 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={3,0}},&
Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={
1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=&
Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp47[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp47,_tmp47,
_tmp47 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={3,
0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp49[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp49,_tmp49,_tmp49 + 30};static
struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={3,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp4B[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp4B,_tmp4B,
_tmp4B + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp4D[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp4D,_tmp4D,_tmp4D + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={
1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=&
Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4F[15]="_zero_arr_plus";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4F,_tmp4F,_tmp4F + 15};static
struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp51[20]="_zero_arr_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={
_tmp51,_tmp51,_tmp51 + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={
1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=&
Cyc_Toc__zero_arr_plus_char_ev;static char _tmp53[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp53,_tmp53,_tmp53
+ 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;
static char _tmp55[19]="_zero_arr_plus_int";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={
_tmp55,_tmp55,_tmp55 + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={
1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=&
Cyc_Toc__zero_arr_plus_int_ev;static char _tmp57[21]="_zero_arr_plus_float";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp57,_tmp57,_tmp57 + 21};
static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;
static char _tmp59[22]="_zero_arr_plus_double";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={
_tmp59,_tmp59,_tmp59 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={
1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=&
Cyc_Toc__zero_arr_plus_double_ev;static char _tmp5B[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp5B,_tmp5B,
_tmp5B + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp5D[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp5D,_tmp5D,_tmp5D + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp5F[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp5F,_tmp5F,_tmp5F + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp61[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp61,_tmp61,_tmp61 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={
1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=&
Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp63[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp63,_tmp63,
_tmp63 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp65[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp65,_tmp65,_tmp65 + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,&
Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp67[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp67,_tmp67,
_tmp67 + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp69[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp69,_tmp69,_tmp69 + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,&
Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp6B[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp6B,
_tmp6B,_tmp6B + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;
static char _tmp6D[34]="_zero_arr_inplace_plus_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp6D,_tmp6D,
_tmp6D + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;
static char _tmp6F[32]="_zero_arr_inplace_plus_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6F,_tmp6F,_tmp6F + 
32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;
static char _tmp71[33]="_dyneither_ptr_inplace_plus_post";static struct
_dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp71,_tmp71,_tmp71
+ 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={3,
0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;
static char _tmp73[28]="_zero_arr_inplace_plus_post";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={
_tmp73,_tmp73,_tmp73 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp75[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp75,_tmp75,_tmp75
+ 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;
static char _tmp77[34]="_zero_arr_inplace_plus_post_short";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp77,_tmp77,
_tmp77 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;
static char _tmp79[32]="_zero_arr_inplace_plus_post_int";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp79,_tmp79,_tmp79 + 
32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;
static char _tmp7B[34]="_zero_arr_inplace_plus_post_float";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp7B,_tmp7B,
_tmp7B + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;
static char _tmp7D[35]="_zero_arr_inplace_plus_post_double";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp7D,_tmp7D,
_tmp7D + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmp7F[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7F,_tmp7F,
_tmp7F + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmp81[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp81,_tmp81,
_tmp81 + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmp83[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmp83,_tmp83,_tmp83 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={3,0}},&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct
Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char
_tmp85[11]="_cyccalloc";static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp85,
_tmp85,_tmp85 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={3,0}},& Cyc_Toc__cyccalloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp87[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp87,_tmp87,_tmp87 + 17};
static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={3,0}},& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmp89[18]="_cyccalloc_atomic";static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmp89,_tmp89,_tmp89 + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={
3,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=&
Cyc_Toc__cyccalloc_atomic_ev;static char _tmp8B[15]="_region_malloc";static struct
_dyneither_ptr Cyc_Toc__region_malloc_str={_tmp8B,_tmp8B,_tmp8B + 15};static struct
_tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={3,0}},& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={
0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static
char _tmp8D[15]="_region_calloc";static struct _dyneither_ptr Cyc_Toc__region_calloc_str={
_tmp8D,_tmp8D,_tmp8D + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={
3,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=&
Cyc_Toc__region_calloc_ev;static char _tmp8F[13]="_check_times";static struct
_dyneither_ptr Cyc_Toc__check_times_str={_tmp8F,_tmp8F,_tmp8F + 13};static struct
_tuple1 Cyc_Toc__check_times_pr={{.Loc_n={3,0}},& Cyc_Toc__check_times_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={
0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char
_tmp91[12]="_new_region";static struct _dyneither_ptr Cyc_Toc__new_region_str={
_tmp91,_tmp91,_tmp91 + 12};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={3,
0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=&
Cyc_Toc__new_region_ev;static char _tmp93[13]="_push_region";static struct
_dyneither_ptr Cyc_Toc__push_region_str={_tmp93,_tmp93,_tmp93 + 13};static struct
_tuple1 Cyc_Toc__push_region_pr={{.Loc_n={3,0}},& Cyc_Toc__push_region_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={
0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char
_tmp95[12]="_pop_region";static struct _dyneither_ptr Cyc_Toc__pop_region_str={
_tmp95,_tmp95,_tmp95 + 12};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={3,
0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=&
Cyc_Toc__pop_region_ev;static char _tmp97[16]="_open_dynregion";static struct
_dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp97,_tmp97,_tmp97 + 16};static
struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={
0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static
char _tmp99[16]="_push_dynregion";static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={
_tmp99,_tmp99,_tmp99 + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={
3,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={
1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=&
Cyc_Toc__push_dynregion_ev;static char _tmp9B[15]="_pop_dynregion";static struct
_dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp9B,_tmp9B,_tmp9B + 15};static struct
_tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={
0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static
char _tmp9D[14]="_reset_region";static struct _dyneither_ptr Cyc_Toc__reset_region_str={
_tmp9D,_tmp9D,_tmp9D + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={{.Loc_n={
3,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmp9F[19]="_throw_arraybounds";static struct
_dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9F,_tmp9F,_tmp9F + 19};static
struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={3,0}},& Cyc_Toc__throw_arraybounds_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={
0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpA1[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr
Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpA1,_tmpA1,_tmpA1 + 29};static struct
_tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={3,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpA3[11]="_swap_word";static
struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpA3,_tmpA3,_tmpA3 + 11};static
struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={3,0}},& Cyc_Toc__swap_word_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct
Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA5[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA5,_tmpA5,_tmpA5 + 16};
static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={3,0}},& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={
0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static
struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={12,{{.UnknownAggr={1,{
Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};static void*Cyc_Toc_dyneither_ptr_typ=(
void*)& Cyc_Toc_dyneither_ptr_typ_v;static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,
0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**
skip_stmt_opt=0;if(skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8[0]=Cyc_Absyn_skip_stmt(0);_tmpA8;});
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e){return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Cast_e_struct*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpAA;_tmpAA.tag=15;_tmpAA.f1=(void*)t;_tmpAA.f2=e;_tmpAA.f3=0;_tmpAA.f4=Cyc_Absyn_No_coercion;
_tmpAA;});_tmpA9;});}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));
_tmpAB[0]=({struct Cyc_Absyn_Deref_e_struct _tmpAC;_tmpAC.tag=22;_tmpAC.f1=e;
_tmpAC;});_tmpAB;});}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpAD=
_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpAE;
_tmpAE.tag=25;_tmpAE.f1=e1;_tmpAE.f2=e2;_tmpAE;});_tmpAD;});}static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpAF=
_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpB0;
_tmpB0.tag=38;_tmpB0.f1=s;_tmpB0;});_tmpAF;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpB1=_cycalloc(
sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpB2;_tmpB2.tag=
18;_tmpB2.f1=(void*)t;_tmpB2;});_tmpB1;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpB4;_tmpB4.tag=11;_tmpB4.f1=e;_tmpB4.f2=es;_tmpB4.f3=0;_tmpB4;});_tmpB3;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_Exp_s_struct _tmpB6;
_tmpB6.tag=1;_tmpB6.f1=e;_tmpB6;});_tmpB5;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_Seq_s_struct _tmpB8;
_tmpB8.tag=2;_tmpB8.f1=s1;_tmpB8.f2=s2;_tmpB8;});_tmpB7;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));
_tmpB9[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpBA;_tmpBA.tag=6;_tmpBA.f1=e1;
_tmpBA.f2=e2;_tmpBA.f3=e3;_tmpBA;});_tmpB9;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpBC;_tmpBC.tag=23;_tmpBC.f1=e;_tmpBC.f2=n;_tmpBC.f3=0;_tmpBC.f4=0;_tmpBC;});
_tmpBB;});}static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpBD=
_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpBE;
_tmpBE.tag=24;_tmpBE.f1=e;_tmpBE.f2=n;_tmpBE.f3=0;_tmpBE.f4=0;_tmpBE;});_tmpBD;});}
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*
ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpBF=_cycalloc(
sizeof(*_tmpBF));_tmpBF[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmpC0;_tmpC0.tag=
37;_tmpC0.f1=tdopt;_tmpC0.f2=ds;_tmpC0;});_tmpBF;});}static void*Cyc_Toc_goto_stmt_r(
struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_Goto_s_struct*
_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1[0]=({struct Cyc_Absyn_Goto_s_struct
_tmpC2;_tmpC2.tag=8;_tmpC2.f1=v;_tmpC2.f2=s;_tmpC2;});_tmpC1;});}static struct Cyc_Absyn_Const_e_struct
Cyc_Toc_zero_exp={0,{.Int_c={4,{Cyc_Absyn_Signed,0}}}};struct Cyc_Toc_functionSet{
struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;
struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*
flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={&
Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,&
Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,&
Cyc_Toc__zero_arr_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={&
Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,&
Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmpC4=t;
enum Cyc_Absyn_Size_of _tmpC6;int _tmpC9;_LL1: {struct Cyc_Absyn_IntType_struct*
_tmpC5=(struct Cyc_Absyn_IntType_struct*)_tmpC4;if(_tmpC5->tag != 6)goto _LL3;else{
_tmpC6=_tmpC5->f2;}}_LL2: switch(_tmpC6){case Cyc_Absyn_Char_sz: _LLB: function=fS->fchar;
break;case Cyc_Absyn_Short_sz: _LLC: function=fS->fshort;break;case Cyc_Absyn_Int_sz:
_LLD: function=fS->fint;break;default: _LLE:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Core_Impossible_struct
_tmpCC;_tmpCC.tag=Cyc_Core_Impossible;_tmpCC.f1=({const char*_tmpCD="impossible IntType (not char, short or int)";
_tag_dyneither(_tmpCD,sizeof(char),44);});_tmpCC;});_tmpCB;}));}goto _LL0;_LL3: {
struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpCE=_cycalloc(
sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Core_Impossible_struct _tmpCF;_tmpCF.tag=
Cyc_Core_Impossible;_tmpCF.f1=({const char*_tmpD0="impossible expression type (not int, float, double, or pointer)";
_tag_dyneither(_tmpD0,sizeof(char),64);});_tmpCF;});_tmpCE;}));_LL0:;}return
function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,
struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(fS,Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpD1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD3;void*_tmpD4;_LL14: {struct Cyc_Absyn_PointerType_struct*
_tmpD2=(struct Cyc_Absyn_PointerType_struct*)_tmpD1;if(_tmpD2->tag != 5)goto _LL16;
else{_tmpD3=_tmpD2->f1;_tmpD4=_tmpD3.elt_typ;}}_LL15: return Cyc_Toc_getFunctionType(
fS,_tmpD4);_LL16:;_LL17:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5[0]=({struct Cyc_Core_Impossible_struct
_tmpD6;_tmpD6.tag=Cyc_Core_Impossible;_tmpD6.f1=({const char*_tmpD7="impossible type (not pointer)";
_tag_dyneither(_tmpD7,sizeof(char),30);});_tmpD6;});_tmpD5;}));_LL13:;}struct
_tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmpD8=e->r;union Cyc_Absyn_Cnst _tmpDA;struct _tuple4
_tmpDB;char _tmpDC;union Cyc_Absyn_Cnst _tmpDE;struct _tuple5 _tmpDF;short _tmpE0;
union Cyc_Absyn_Cnst _tmpE2;struct _tuple6 _tmpE3;int _tmpE4;union Cyc_Absyn_Cnst
_tmpE6;struct _tuple7 _tmpE7;long long _tmpE8;union Cyc_Absyn_Cnst _tmpEA;int _tmpEB;
struct Cyc_Absyn_Exp*_tmpED;struct Cyc_List_List*_tmpEF;struct Cyc_List_List*_tmpF1;
struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*_tmpF7;
_LL19: {struct Cyc_Absyn_Const_e_struct*_tmpD9=(struct Cyc_Absyn_Const_e_struct*)
_tmpD8;if(_tmpD9->tag != 0)goto _LL1B;else{_tmpDA=_tmpD9->f1;if((_tmpDA.Char_c).tag
!= 2)goto _LL1B;_tmpDB=(struct _tuple4)(_tmpDA.Char_c).val;_tmpDC=_tmpDB.f2;}}
_LL1A: return _tmpDC == '\000';_LL1B: {struct Cyc_Absyn_Const_e_struct*_tmpDD=(
struct Cyc_Absyn_Const_e_struct*)_tmpD8;if(_tmpDD->tag != 0)goto _LL1D;else{_tmpDE=
_tmpDD->f1;if((_tmpDE.Short_c).tag != 3)goto _LL1D;_tmpDF=(struct _tuple5)(_tmpDE.Short_c).val;
_tmpE0=_tmpDF.f2;}}_LL1C: return _tmpE0 == 0;_LL1D: {struct Cyc_Absyn_Const_e_struct*
_tmpE1=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;if(_tmpE1->tag != 0)goto _LL1F;
else{_tmpE2=_tmpE1->f1;if((_tmpE2.Int_c).tag != 4)goto _LL1F;_tmpE3=(struct _tuple6)(
_tmpE2.Int_c).val;_tmpE4=_tmpE3.f2;}}_LL1E: return _tmpE4 == 0;_LL1F: {struct Cyc_Absyn_Const_e_struct*
_tmpE5=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;if(_tmpE5->tag != 0)goto _LL21;
else{_tmpE6=_tmpE5->f1;if((_tmpE6.LongLong_c).tag != 5)goto _LL21;_tmpE7=(struct
_tuple7)(_tmpE6.LongLong_c).val;_tmpE8=_tmpE7.f2;}}_LL20: return _tmpE8 == 0;_LL21: {
struct Cyc_Absyn_Const_e_struct*_tmpE9=(struct Cyc_Absyn_Const_e_struct*)_tmpD8;
if(_tmpE9->tag != 0)goto _LL23;else{_tmpEA=_tmpE9->f1;if((_tmpEA.Null_c).tag != 1)
goto _LL23;_tmpEB=(int)(_tmpEA.Null_c).val;}}_LL22: return 1;_LL23: {struct Cyc_Absyn_Cast_e_struct*
_tmpEC=(struct Cyc_Absyn_Cast_e_struct*)_tmpD8;if(_tmpEC->tag != 15)goto _LL25;
else{_tmpED=_tmpEC->f2;}}_LL24: return Cyc_Toc_is_zero(_tmpED);_LL25: {struct Cyc_Absyn_Tuple_e_struct*
_tmpEE=(struct Cyc_Absyn_Tuple_e_struct*)_tmpD8;if(_tmpEE->tag != 26)goto _LL27;
else{_tmpEF=_tmpEE->f1;}}_LL26: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpEF);_LL27: {struct
Cyc_Absyn_Array_e_struct*_tmpF0=(struct Cyc_Absyn_Array_e_struct*)_tmpD8;if(
_tmpF0->tag != 28)goto _LL29;else{_tmpF1=_tmpF0->f1;}}_LL28: _tmpF3=_tmpF1;goto
_LL2A;_LL29: {struct Cyc_Absyn_Aggregate_e_struct*_tmpF2=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpD8;if(_tmpF2->tag != 30)goto _LL2B;else{_tmpF3=_tmpF2->f3;}}_LL2A: _tmpF5=
_tmpF3;goto _LL2C;_LL2B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpF4=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpD8;if(_tmpF4->tag != 27)goto _LL2D;else{_tmpF5=_tmpF4->f2;}}_LL2C: _tmpF7=
_tmpF5;goto _LL2E;_LL2D: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpF6=(struct
Cyc_Absyn_UnresolvedMem_e_struct*)_tmpD8;if(_tmpF6->tag != 37)goto _LL2F;else{
_tmpF7=_tmpF6->f2;}}_LL2E: for(0;_tmpF7 != 0;_tmpF7=_tmpF7->tl){if(!Cyc_Toc_is_zero((*((
struct _tuple16*)_tmpF7->hd)).f2))return 0;}return 1;_LL2F:;_LL30: return 0;_LL18:;}
static int Cyc_Toc_is_nullable(void*t){void*_tmpF8=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmpFA;struct Cyc_Absyn_PtrAtts _tmpFB;union Cyc_Absyn_Constraint*
_tmpFC;_LL32: {struct Cyc_Absyn_PointerType_struct*_tmpF9=(struct Cyc_Absyn_PointerType_struct*)
_tmpF8;if(_tmpF9->tag != 5)goto _LL34;else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpFA.ptr_atts;
_tmpFC=_tmpFB.nullable;}}_LL33: return((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,_tmpFC);_LL34:;_LL35:({void*_tmpFD=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpFE="is_nullable";
_tag_dyneither(_tmpFE,sizeof(char),12);}),_tag_dyneither(_tmpFD,sizeof(void*),0));});
_LL31:;}static char _tmp10B[1]="";static char _tmp10C[8]="*bogus*";static struct
_tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct _dyneither_ptr tag){
struct _DynRegionHandle*_tmp100;struct Cyc_Dict_Dict*_tmp101;struct Cyc_Toc_TocState
_tmpFF=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp100=_tmpFF.dyn;
_tmp101=_tmpFF.qvar_tags;{static struct _dyneither_ptr bogus_string={_tmp10C,
_tmp10C,_tmp10C + 8};static struct _tuple1 bogus_qvar={{.Loc_n={3,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,{_tmp10B,_tmp10B,_tmp10B + 1}};pair=({
struct _tuple14 _tmp102;_tmp102.f1=x;_tmp102.f2=tag;_tmp102;});{struct
_DynRegionFrame _tmp103;struct _RegionHandle*d=_open_dynregion(& _tmp103,_tmp100);{
struct _tuple1**_tmp104=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple14*
k))Cyc_Dict_lookup_opt)(*_tmp101,(struct _tuple14*)& pair);if(_tmp104 != 0){struct
_tuple1*_tmp105=*_tmp104;_npop_handler(0);return _tmp105;}{struct _tuple14*_tmp106=({
struct _tuple14*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->f1=x;_tmp10A->f2=tag;
_tmp10A;});struct _tuple1*res=({struct _tuple1*_tmp108=_cycalloc(sizeof(*_tmp108));
_tmp108->f1=(*x).f1;_tmp108->f2=({struct _dyneither_ptr*_tmp109=_cycalloc(sizeof(*
_tmp109));_tmp109[0]=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*
x).f2,(struct _dyneither_ptr)tag);_tmp109;});_tmp108;});*_tmp101=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp101,(
struct _tuple14*)_tmp106,res);{struct _tuple1*_tmp107=res;_npop_handler(0);return
_tmp107;}}};_pop_dynregion(d);}}}struct _tuple17{void*f1;struct Cyc_List_List*f2;}
;static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){struct
_DynRegionHandle*_tmp10E;struct Cyc_List_List**_tmp10F;struct Cyc_Toc_TocState
_tmp10D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp10E=
_tmp10D.dyn;_tmp10F=_tmp10D.tuple_types;{struct _DynRegionFrame _tmp110;struct
_RegionHandle*d=_open_dynregion(& _tmp110,_tmp10E);{struct Cyc_List_List*_tmp111=*
_tmp10F;for(0;_tmp111 != 0;_tmp111=_tmp111->tl){struct _tuple17 _tmp113;void*
_tmp114;struct Cyc_List_List*_tmp115;struct _tuple17*_tmp112=(struct _tuple17*)
_tmp111->hd;_tmp113=*_tmp112;_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;{struct Cyc_List_List*
_tmp116=tqs0;for(0;_tmp116 != 0  && _tmp115 != 0;(_tmp116=_tmp116->tl,_tmp115=
_tmp115->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp116->hd)).f2,(void*)
_tmp115->hd))break;}if(_tmp116 == 0  && _tmp115 == 0){void*_tmp117=_tmp114;
_npop_handler(0);return _tmp117;}}}}{struct _dyneither_ptr*xname=({struct
_dyneither_ptr*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp128;_tmp128.tag=1;_tmp128.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmp126[1]={& _tmp128};Cyc_aprintf(({
const char*_tmp127="_tuple%d";_tag_dyneither(_tmp127,sizeof(char),9);}),
_tag_dyneither(_tmp126,sizeof(void*),1));}});_tmp125;});void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp118=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){_tmp118=({struct Cyc_List_List*_tmp119=_cycalloc(sizeof(*
_tmp119));_tmp119->hd=({struct Cyc_Absyn_Aggrfield*_tmp11A=_cycalloc(sizeof(*
_tmp11A));_tmp11A->name=Cyc_Absyn_fieldname(i);_tmp11A->tq=Cyc_Toc_mt_tq;_tmp11A->type=(
void*)ts2->hd;_tmp11A->width=0;_tmp11A->attributes=0;_tmp11A;});_tmp119->tl=
_tmp118;_tmp119;});}}_tmp118=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp118);{struct Cyc_Absyn_Aggrdecl*_tmp11B=({struct Cyc_Absyn_Aggrdecl*
_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->kind=Cyc_Absyn_StructA;_tmp122->sc=
Cyc_Absyn_Public;_tmp122->name=({struct _tuple1*_tmp124=_cycalloc(sizeof(*_tmp124));
_tmp124->f1=Cyc_Absyn_Rel_n(0);_tmp124->f2=xname;_tmp124;});_tmp122->tvs=0;
_tmp122->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp123=_cycalloc(sizeof(*_tmp123));
_tmp123->exist_vars=0;_tmp123->rgn_po=0;_tmp123->fields=_tmp118;_tmp123->tagged=
0;_tmp123;});_tmp122->attributes=0;_tmp122;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp11E;_tmp11E.tag=6;_tmp11E.f1=_tmp11B;_tmp11E;});
_tmp11D;}),0);_tmp11C->tl=Cyc_Toc_result_decls;_tmp11C;});*_tmp10F=({struct Cyc_List_List*
_tmp11F=_region_malloc(d,sizeof(*_tmp11F));_tmp11F->hd=({struct _tuple17*_tmp120=
_region_malloc(d,sizeof(*_tmp120));_tmp120->f1=x;_tmp120->f2=ts;_tmp120;});
_tmp11F->tl=*_tmp10F;_tmp11F;});{void*_tmp121=x;_npop_handler(0);return _tmp121;}}};
_pop_dynregion(d);}}struct _tuple1*Cyc_Toc_temp_var(){int _tmp129=Cyc_Toc_temp_var_counter
++;struct _tuple1*res=({struct _tuple1*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->f1=
Cyc_Absyn_Loc_n;_tmp12A->f2=({struct _dyneither_ptr*_tmp12B=_cycalloc(sizeof(*
_tmp12B));_tmp12B[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp12E;
_tmp12E.tag=1;_tmp12E.f1=(unsigned int)_tmp129;{void*_tmp12C[1]={& _tmp12E};Cyc_aprintf(({
const char*_tmp12D="_tmp%X";_tag_dyneither(_tmp12D,sizeof(char),7);}),
_tag_dyneither(_tmp12C,sizeof(void*),1));}});_tmp12B;});_tmp12A;});return res;}
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp130;
struct Cyc_Xarray_Xarray*_tmp131;struct Cyc_Toc_TocState _tmp12F=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp130=_tmp12F.dyn;_tmp131=_tmp12F.temp_labels;{
struct _DynRegionFrame _tmp132;struct _RegionHandle*d=_open_dynregion(& _tmp132,
_tmp130);{int _tmp133=Cyc_Toc_fresh_label_counter ++;if(_tmp133 < ((int(*)(struct
Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp131)){struct _dyneither_ptr*_tmp134=((
struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp131,
_tmp133);_npop_handler(0);return _tmp134;}{struct _dyneither_ptr*res=({struct
_dyneither_ptr*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp13B;_tmp13B.tag=1;_tmp13B.f1=(
unsigned int)_tmp133;{void*_tmp139[1]={& _tmp13B};Cyc_aprintf(({const char*_tmp13A="_LL%X";
_tag_dyneither(_tmp13A,sizeof(char),6);}),_tag_dyneither(_tmp139,sizeof(void*),1));}});
_tmp138;});if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(
_tmp131,res)!= _tmp133)({void*_tmp135=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp136="fresh_label: add_ind returned bad index...";
_tag_dyneither(_tmp136,sizeof(char),43);}),_tag_dyneither(_tmp135,sizeof(void*),
0));});{struct _dyneither_ptr*_tmp137=res;_npop_handler(0);return _tmp137;}}};
_pop_dynregion(d);}}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple1*name){int ans=0;struct Cyc_List_List*_tmp13C=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp13C))->hd)->name)
!= 0){++ ans;_tmp13C=_tmp13C->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple8*Cyc_Toc_arg_to_c(struct
_tuple8*a){struct Cyc_Core_Opt*_tmp13E;struct Cyc_Absyn_Tqual _tmp13F;void*_tmp140;
struct _tuple8 _tmp13D=*a;_tmp13E=_tmp13D.f1;_tmp13F=_tmp13D.f2;_tmp140=_tmp13D.f3;
return({struct _tuple8*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->f1=_tmp13E;
_tmp141->f2=_tmp13F;_tmp141->f3=Cyc_Toc_typ_to_c(_tmp140);_tmp141;});}static
struct _tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x){struct Cyc_Absyn_Tqual _tmp143;
void*_tmp144;struct _tuple10 _tmp142=*x;_tmp143=_tmp142.f1;_tmp144=_tmp142.f2;
return({struct _tuple10*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->f1=_tmp143;
_tmp145->f2=Cyc_Toc_typ_to_c(_tmp144);_tmp145;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp146=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp148;
void*_tmp149;struct Cyc_Absyn_Tqual _tmp14A;struct Cyc_Absyn_Exp*_tmp14B;union Cyc_Absyn_Constraint*
_tmp14C;struct Cyc_Position_Segment*_tmp14D;struct Cyc_Core_Opt*_tmp14F;struct Cyc_Core_Opt
_tmp150;void*_tmp151;_LL37: {struct Cyc_Absyn_ArrayType_struct*_tmp147=(struct Cyc_Absyn_ArrayType_struct*)
_tmp146;if(_tmp147->tag != 9)goto _LL39;else{_tmp148=_tmp147->f1;_tmp149=_tmp148.elt_type;
_tmp14A=_tmp148.tq;_tmp14B=_tmp148.num_elts;_tmp14C=_tmp148.zero_term;_tmp14D=
_tmp148.zt_loc;}}_LL38: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp149),
_tmp14A,_tmp14B,Cyc_Absyn_false_conref,_tmp14D);_LL39: {struct Cyc_Absyn_Evar_struct*
_tmp14E=(struct Cyc_Absyn_Evar_struct*)_tmp146;if(_tmp14E->tag != 1)goto _LL3B;
else{_tmp14F=_tmp14E->f2;if(_tmp14F == 0)goto _LL3B;_tmp150=*_tmp14F;_tmp151=(void*)
_tmp150.v;}}_LL3A: return Cyc_Toc_typ_to_c_array(_tmp151);_LL3B:;_LL3C: return Cyc_Toc_typ_to_c(
t);_LL36:;}static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){return({struct Cyc_Absyn_Aggrfield*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->name=
f->name;_tmp152->tq=Cyc_Toc_mt_tq;_tmp152->type=Cyc_Toc_typ_to_c(f->type);
_tmp152->width=f->width;_tmp152->attributes=f->attributes;_tmp152;});}static void
Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0)cs=({void**_tmp153=_cycalloc(sizeof(*_tmp153));
_tmp153[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp153;});return*cs;}static void*Cyc_Toc_rgn_typ(){
static void**r=0;if(r == 0)r=({void**_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=
Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);
_tmp154;});return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0)r=({
void**_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(
Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_tmp155;});return*r;}static void*Cyc_Toc_typ_to_c(
void*t){void*_tmp156=t;struct Cyc_Core_Opt*_tmp159;struct Cyc_Core_Opt*_tmp15B;
struct Cyc_Core_Opt _tmp15C;void*_tmp15D;struct Cyc_Absyn_Tvar*_tmp15F;struct Cyc_Absyn_DatatypeFieldInfo
_tmp162;union Cyc_Absyn_DatatypeFieldInfoU _tmp163;struct _tuple2 _tmp164;struct Cyc_Absyn_Datatypedecl*
_tmp165;struct Cyc_Absyn_Datatypefield*_tmp166;struct Cyc_Absyn_PtrInfo _tmp169;
void*_tmp16A;struct Cyc_Absyn_Tqual _tmp16B;struct Cyc_Absyn_PtrAtts _tmp16C;union
Cyc_Absyn_Constraint*_tmp16D;struct Cyc_Absyn_ArrayInfo _tmp172;void*_tmp173;
struct Cyc_Absyn_Tqual _tmp174;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Position_Segment*
_tmp176;struct Cyc_Absyn_FnInfo _tmp178;void*_tmp179;struct Cyc_List_List*_tmp17A;
int _tmp17B;struct Cyc_Absyn_VarargInfo*_tmp17C;struct Cyc_List_List*_tmp17D;struct
Cyc_List_List*_tmp17F;enum Cyc_Absyn_AggrKind _tmp181;struct Cyc_List_List*_tmp182;
struct Cyc_Absyn_AggrInfo _tmp184;union Cyc_Absyn_AggrInfoU _tmp185;struct Cyc_List_List*
_tmp186;struct _tuple1*_tmp188;struct Cyc_List_List*_tmp18A;struct _tuple1*_tmp18C;
struct Cyc_List_List*_tmp18D;struct Cyc_Absyn_Typedefdecl*_tmp18E;void**_tmp18F;
void*_tmp192;_LL3E: {struct Cyc_Absyn_VoidType_struct*_tmp157=(struct Cyc_Absyn_VoidType_struct*)
_tmp156;if(_tmp157->tag != 0)goto _LL40;}_LL3F: return t;_LL40: {struct Cyc_Absyn_Evar_struct*
_tmp158=(struct Cyc_Absyn_Evar_struct*)_tmp156;if(_tmp158->tag != 1)goto _LL42;
else{_tmp159=_tmp158->f2;if(_tmp159 != 0)goto _LL42;}}_LL41: return Cyc_Absyn_sint_typ;
_LL42: {struct Cyc_Absyn_Evar_struct*_tmp15A=(struct Cyc_Absyn_Evar_struct*)
_tmp156;if(_tmp15A->tag != 1)goto _LL44;else{_tmp15B=_tmp15A->f2;if(_tmp15B == 0)
goto _LL44;_tmp15C=*_tmp15B;_tmp15D=(void*)_tmp15C.v;}}_LL43: return Cyc_Toc_typ_to_c(
_tmp15D);_LL44: {struct Cyc_Absyn_VarType_struct*_tmp15E=(struct Cyc_Absyn_VarType_struct*)
_tmp156;if(_tmp15E->tag != 2)goto _LL46;else{_tmp15F=_tmp15E->f1;}}_LL45: if(Cyc_Tcutil_tvar_kind(
_tmp15F,Cyc_Absyn_BoxKind)== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;
else{return Cyc_Absyn_void_star_typ();}_LL46: {struct Cyc_Absyn_DatatypeType_struct*
_tmp160=(struct Cyc_Absyn_DatatypeType_struct*)_tmp156;if(_tmp160->tag != 3)goto
_LL48;}_LL47: return(void*)& Cyc_Absyn_VoidType_val;_LL48: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp161=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp156;if(_tmp161->tag != 4)
goto _LL4A;else{_tmp162=_tmp161->f1;_tmp163=_tmp162.field_info;if((_tmp163.KnownDatatypefield).tag
!= 2)goto _LL4A;_tmp164=(struct _tuple2)(_tmp163.KnownDatatypefield).val;_tmp165=
_tmp164.f1;_tmp166=_tmp164.f2;}}_LL49: return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp166->name,({const char*_tmp19A="_struct";_tag_dyneither(_tmp19A,sizeof(char),
8);})));_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp167=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp156;if(_tmp167->tag != 4)goto _LL4C;}_LL4B:({void*_tmp19B=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp19C="unresolved DatatypeFieldType";
_tag_dyneither(_tmp19C,sizeof(char),29);}),_tag_dyneither(_tmp19B,sizeof(void*),
0));});_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp168=(struct Cyc_Absyn_PointerType_struct*)
_tmp156;if(_tmp168->tag != 5)goto _LL4E;else{_tmp169=_tmp168->f1;_tmp16A=_tmp169.elt_typ;
_tmp16B=_tmp169.elt_tq;_tmp16C=_tmp169.ptr_atts;_tmp16D=_tmp16C.bounds;}}_LL4D:
_tmp16A=Cyc_Toc_typ_to_c_array(_tmp16A);{void*_tmp19D=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp16D);_LL77: {
struct Cyc_Absyn_DynEither_b_struct*_tmp19E=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp19D;if(_tmp19E->tag != 0)goto _LL79;}_LL78: return Cyc_Toc_dyneither_ptr_typ;
_LL79:;_LL7A: return Cyc_Absyn_star_typ(_tmp16A,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp16B,Cyc_Absyn_false_conref);_LL76:;}_LL4E: {struct Cyc_Absyn_IntType_struct*
_tmp16E=(struct Cyc_Absyn_IntType_struct*)_tmp156;if(_tmp16E->tag != 6)goto _LL50;}
_LL4F: goto _LL51;_LL50: {struct Cyc_Absyn_FloatType_struct*_tmp16F=(struct Cyc_Absyn_FloatType_struct*)
_tmp156;if(_tmp16F->tag != 7)goto _LL52;}_LL51: goto _LL53;_LL52: {struct Cyc_Absyn_DoubleType_struct*
_tmp170=(struct Cyc_Absyn_DoubleType_struct*)_tmp156;if(_tmp170->tag != 8)goto
_LL54;}_LL53: return t;_LL54: {struct Cyc_Absyn_ArrayType_struct*_tmp171=(struct Cyc_Absyn_ArrayType_struct*)
_tmp156;if(_tmp171->tag != 9)goto _LL56;else{_tmp172=_tmp171->f1;_tmp173=_tmp172.elt_type;
_tmp174=_tmp172.tq;_tmp175=_tmp172.num_elts;_tmp176=_tmp172.zt_loc;}}_LL55:
return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp173),_tmp174,_tmp175,Cyc_Absyn_false_conref,
_tmp176);_LL56: {struct Cyc_Absyn_FnType_struct*_tmp177=(struct Cyc_Absyn_FnType_struct*)
_tmp156;if(_tmp177->tag != 10)goto _LL58;else{_tmp178=_tmp177->f1;_tmp179=_tmp178.ret_typ;
_tmp17A=_tmp178.args;_tmp17B=_tmp178.c_varargs;_tmp17C=_tmp178.cyc_varargs;
_tmp17D=_tmp178.attributes;}}_LL57: {struct Cyc_List_List*_tmp19F=0;for(0;_tmp17D
!= 0;_tmp17D=_tmp17D->tl){void*_tmp1A0=(void*)_tmp17D->hd;_LL7C: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp1A1=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp1A0;if(_tmp1A1->tag != 4)goto
_LL7E;}_LL7D: goto _LL7F;_LL7E: {struct Cyc_Absyn_Const_att_struct*_tmp1A2=(struct
Cyc_Absyn_Const_att_struct*)_tmp1A0;if(_tmp1A2->tag != 5)goto _LL80;}_LL7F: goto
_LL81;_LL80: {struct Cyc_Absyn_Format_att_struct*_tmp1A3=(struct Cyc_Absyn_Format_att_struct*)
_tmp1A0;if(_tmp1A3->tag != 19)goto _LL82;}_LL81: continue;_LL82: {struct Cyc_Absyn_Initializes_att_struct*
_tmp1A4=(struct Cyc_Absyn_Initializes_att_struct*)_tmp1A0;if(_tmp1A4->tag != 20)
goto _LL84;}_LL83: continue;_LL84:;_LL85: _tmp19F=({struct Cyc_List_List*_tmp1A5=
_cycalloc(sizeof(*_tmp1A5));_tmp1A5->hd=(void*)_tmp17D->hd;_tmp1A5->tl=_tmp19F;
_tmp1A5;});goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A6=((struct Cyc_List_List*(*)(
struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp17A);if(_tmp17C != 0){void*_tmp1A7=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17C->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmp1A8=({struct _tuple8*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));
_tmp1AA->f1=_tmp17C->name;_tmp1AA->f2=_tmp17C->tq;_tmp1AA->f3=_tmp1A7;_tmp1AA;});
_tmp1A6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp1A6,({struct Cyc_List_List*_tmp1A9=_cycalloc(sizeof(*
_tmp1A9));_tmp1A9->hd=_tmp1A8;_tmp1A9->tl=0;_tmp1A9;}));}return(void*)({struct
Cyc_Absyn_FnType_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB[0]=({struct
Cyc_Absyn_FnType_struct _tmp1AC;_tmp1AC.tag=10;_tmp1AC.f1=({struct Cyc_Absyn_FnInfo
_tmp1AD;_tmp1AD.tvars=0;_tmp1AD.effect=0;_tmp1AD.ret_typ=Cyc_Toc_typ_to_c(
_tmp179);_tmp1AD.args=_tmp1A6;_tmp1AD.c_varargs=_tmp17B;_tmp1AD.cyc_varargs=0;
_tmp1AD.rgn_po=0;_tmp1AD.attributes=_tmp19F;_tmp1AD;});_tmp1AC;});_tmp1AB;});}}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp17E=(struct Cyc_Absyn_TupleType_struct*)
_tmp156;if(_tmp17E->tag != 11)goto _LL5A;else{_tmp17F=_tmp17E->f1;}}_LL59: _tmp17F=((
struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp17F);return Cyc_Toc_add_tuple_type(
_tmp17F);_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp180=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp156;if(_tmp180->tag != 13)goto _LL5C;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;}}
_LL5B: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp1AE=_cycalloc(
sizeof(*_tmp1AE));_tmp1AE[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp1AF;
_tmp1AF.tag=13;_tmp1AF.f1=_tmp181;_tmp1AF.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp182);_tmp1AF;});_tmp1AE;});_LL5C: {struct Cyc_Absyn_AggrType_struct*_tmp183=(
struct Cyc_Absyn_AggrType_struct*)_tmp156;if(_tmp183->tag != 12)goto _LL5E;else{
_tmp184=_tmp183->f1;_tmp185=_tmp184.aggr_info;_tmp186=_tmp184.targs;}}_LL5D:{
union Cyc_Absyn_AggrInfoU _tmp1B0=_tmp185;struct _tuple3 _tmp1B1;_LL87: if((_tmp1B0.UnknownAggr).tag
!= 1)goto _LL89;_tmp1B1=(struct _tuple3)(_tmp1B0.UnknownAggr).val;_LL88: return t;
_LL89:;_LL8A: goto _LL86;_LL86:;}{struct Cyc_Absyn_Aggrdecl*_tmp1B2=Cyc_Absyn_get_known_aggrdecl(
_tmp185);if(_tmp1B2->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1B2->name,
Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1B2->name,Cyc_Absyn_strctq);}}
_LL5E: {struct Cyc_Absyn_EnumType_struct*_tmp187=(struct Cyc_Absyn_EnumType_struct*)
_tmp156;if(_tmp187->tag != 14)goto _LL60;else{_tmp188=_tmp187->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp189=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp156;if(_tmp189->tag != 15)goto _LL62;else{_tmp18A=_tmp189->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18A);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp18B=(struct Cyc_Absyn_TypedefType_struct*)
_tmp156;if(_tmp18B->tag != 18)goto _LL64;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;
_tmp18E=_tmp18B->f3;_tmp18F=_tmp18B->f4;}}_LL63: if(_tmp18F == 0  || Cyc_noexpand_r){
if(_tmp18D != 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp1B3=
_cycalloc(sizeof(*_tmp1B3));_tmp1B3[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp1B4;_tmp1B4.tag=18;_tmp1B4.f1=_tmp18C;_tmp1B4.f2=0;_tmp1B4.f3=_tmp18E;
_tmp1B4.f4=0;_tmp1B4;});_tmp1B3;});else{return t;}}else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp1B5=_cycalloc(sizeof(*_tmp1B5));_tmp1B5[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp1B6;_tmp1B6.tag=18;_tmp1B6.f1=_tmp18C;_tmp1B6.f2=0;_tmp1B6.f3=_tmp18E;
_tmp1B6.f4=({void**_tmp1B7=_cycalloc(sizeof(*_tmp1B7));_tmp1B7[0]=Cyc_Toc_typ_to_c_array(*
_tmp18F);_tmp1B7;});_tmp1B6;});_tmp1B5;});}_LL64: {struct Cyc_Absyn_TagType_struct*
_tmp190=(struct Cyc_Absyn_TagType_struct*)_tmp156;if(_tmp190->tag != 20)goto _LL66;}
_LL65: return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp191=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp156;if(_tmp191->tag != 16)goto
_LL68;else{_tmp192=(void*)_tmp191->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {
struct Cyc_Absyn_DynRgnType_struct*_tmp193=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp156;if(_tmp193->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {
struct Cyc_Absyn_HeapRgn_struct*_tmp194=(struct Cyc_Absyn_HeapRgn_struct*)_tmp156;
if(_tmp194->tag != 21)goto _LL6C;}_LL6B:({void*_tmp1B8=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1B9="Toc::typ_to_c: type translation passed the heap region";
_tag_dyneither(_tmp1B9,sizeof(char),55);}),_tag_dyneither(_tmp1B8,sizeof(void*),
0));});_LL6C: {struct Cyc_Absyn_UniqueRgn_struct*_tmp195=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp156;if(_tmp195->tag != 22)goto _LL6E;}_LL6D:({void*_tmp1BA=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1BB="Toc::typ_to_c: type translation passed the unique region";
_tag_dyneither(_tmp1BB,sizeof(char),57);}),_tag_dyneither(_tmp1BA,sizeof(void*),
0));});_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp196=(struct Cyc_Absyn_AccessEff_struct*)
_tmp156;if(_tmp196->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp197=(struct Cyc_Absyn_JoinEff_struct*)_tmp156;if(_tmp197->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp198=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp156;if(_tmp198->tag != 25)goto _LL74;}_LL73:({void*_tmp1BC=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1BD="Toc::typ_to_c: type translation passed an effect";
_tag_dyneither(_tmp1BD,sizeof(char),49);}),_tag_dyneither(_tmp1BC,sizeof(void*),
0));});_LL74: {struct Cyc_Absyn_ValueofType_struct*_tmp199=(struct Cyc_Absyn_ValueofType_struct*)
_tmp156;if(_tmp199->tag != 19)goto _LL3D;}_LL75:({void*_tmp1BE=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1BF="Toc::typ_to_c: type translation passed a valueof_t";
_tag_dyneither(_tmp1BF,sizeof(char),51);}),_tag_dyneither(_tmp1BE,sizeof(void*),
0));});_LL3D:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1C0=t;struct Cyc_Absyn_ArrayInfo
_tmp1C2;void*_tmp1C3;struct Cyc_Absyn_Tqual _tmp1C4;_LL8C: {struct Cyc_Absyn_ArrayType_struct*
_tmp1C1=(struct Cyc_Absyn_ArrayType_struct*)_tmp1C0;if(_tmp1C1->tag != 9)goto _LL8E;
else{_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C2.elt_type;_tmp1C4=_tmp1C2.tq;}}_LL8D:
return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1C3,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp1C4,Cyc_Absyn_false_conref),e);_LL8E:;_LL8F: return Cyc_Toc_cast_it(t,e);_LL8B:;}
static int Cyc_Toc_atomic_typ(void*t){void*_tmp1C5=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_ArrayInfo _tmp1D0;void*_tmp1D1;struct Cyc_Absyn_AggrInfo _tmp1D3;union Cyc_Absyn_AggrInfoU
_tmp1D4;struct Cyc_List_List*_tmp1D6;struct Cyc_Absyn_DatatypeFieldInfo _tmp1D8;
union Cyc_Absyn_DatatypeFieldInfoU _tmp1D9;struct _tuple2 _tmp1DA;struct Cyc_Absyn_Datatypedecl*
_tmp1DB;struct Cyc_Absyn_Datatypefield*_tmp1DC;struct Cyc_List_List*_tmp1DE;_LL91: {
struct Cyc_Absyn_VoidType_struct*_tmp1C6=(struct Cyc_Absyn_VoidType_struct*)
_tmp1C5;if(_tmp1C6->tag != 0)goto _LL93;}_LL92: return 1;_LL93: {struct Cyc_Absyn_VarType_struct*
_tmp1C7=(struct Cyc_Absyn_VarType_struct*)_tmp1C5;if(_tmp1C7->tag != 2)goto _LL95;}
_LL94: return 0;_LL95: {struct Cyc_Absyn_IntType_struct*_tmp1C8=(struct Cyc_Absyn_IntType_struct*)
_tmp1C5;if(_tmp1C8->tag != 6)goto _LL97;}_LL96: goto _LL98;_LL97: {struct Cyc_Absyn_EnumType_struct*
_tmp1C9=(struct Cyc_Absyn_EnumType_struct*)_tmp1C5;if(_tmp1C9->tag != 14)goto _LL99;}
_LL98: goto _LL9A;_LL99: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1CA=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1C5;if(_tmp1CA->tag != 15)goto _LL9B;}_LL9A: goto _LL9C;_LL9B: {struct Cyc_Absyn_FloatType_struct*
_tmp1CB=(struct Cyc_Absyn_FloatType_struct*)_tmp1C5;if(_tmp1CB->tag != 7)goto _LL9D;}
_LL9C: goto _LL9E;_LL9D: {struct Cyc_Absyn_DoubleType_struct*_tmp1CC=(struct Cyc_Absyn_DoubleType_struct*)
_tmp1C5;if(_tmp1CC->tag != 8)goto _LL9F;}_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_FnType_struct*
_tmp1CD=(struct Cyc_Absyn_FnType_struct*)_tmp1C5;if(_tmp1CD->tag != 10)goto _LLA1;}
_LLA0: goto _LLA2;_LLA1: {struct Cyc_Absyn_TagType_struct*_tmp1CE=(struct Cyc_Absyn_TagType_struct*)
_tmp1C5;if(_tmp1CE->tag != 20)goto _LLA3;}_LLA2: return 1;_LLA3: {struct Cyc_Absyn_ArrayType_struct*
_tmp1CF=(struct Cyc_Absyn_ArrayType_struct*)_tmp1C5;if(_tmp1CF->tag != 9)goto _LLA5;
else{_tmp1D0=_tmp1CF->f1;_tmp1D1=_tmp1D0.elt_type;}}_LLA4: return Cyc_Toc_atomic_typ(
_tmp1D1);_LLA5: {struct Cyc_Absyn_AggrType_struct*_tmp1D2=(struct Cyc_Absyn_AggrType_struct*)
_tmp1C5;if(_tmp1D2->tag != 12)goto _LLA7;else{_tmp1D3=_tmp1D2->f1;_tmp1D4=_tmp1D3.aggr_info;}}
_LLA6:{union Cyc_Absyn_AggrInfoU _tmp1E2=_tmp1D4;struct _tuple3 _tmp1E3;_LLB6: if((
_tmp1E2.UnknownAggr).tag != 1)goto _LLB8;_tmp1E3=(struct _tuple3)(_tmp1E2.UnknownAggr).val;
_LLB7: return 0;_LLB8:;_LLB9: goto _LLB5;_LLB5:;}{struct Cyc_Absyn_Aggrdecl*_tmp1E4=
Cyc_Absyn_get_known_aggrdecl(_tmp1D4);if(_tmp1E4->impl == 0)return 0;{struct Cyc_List_List*
_tmp1E5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E4->impl))->fields;for(
0;_tmp1E5 != 0;_tmp1E5=_tmp1E5->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1E5->hd)->type))return 0;}}return 1;}_LLA7: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1D5=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1C5;if(_tmp1D5->tag != 13)goto
_LLA9;else{_tmp1D6=_tmp1D5->f2;}}_LLA8: for(0;_tmp1D6 != 0;_tmp1D6=_tmp1D6->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1D6->hd)->type))return 0;}
return 1;_LLA9: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1D7=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1C5;if(_tmp1D7->tag != 4)goto _LLAB;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D8.field_info;
if((_tmp1D9.KnownDatatypefield).tag != 2)goto _LLAB;_tmp1DA=(struct _tuple2)(
_tmp1D9.KnownDatatypefield).val;_tmp1DB=_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;}}_LLAA:
_tmp1DE=_tmp1DC->typs;goto _LLAC;_LLAB: {struct Cyc_Absyn_TupleType_struct*_tmp1DD=(
struct Cyc_Absyn_TupleType_struct*)_tmp1C5;if(_tmp1DD->tag != 11)goto _LLAD;else{
_tmp1DE=_tmp1DD->f1;}}_LLAC: for(0;_tmp1DE != 0;_tmp1DE=_tmp1DE->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple10*)_tmp1DE->hd)).f2))return 0;}return 1;_LLAD: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1DF=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1C5;if(_tmp1DF->tag != 3)goto
_LLAF;}_LLAE: goto _LLB0;_LLAF: {struct Cyc_Absyn_PointerType_struct*_tmp1E0=(
struct Cyc_Absyn_PointerType_struct*)_tmp1C5;if(_tmp1E0->tag != 5)goto _LLB1;}_LLB0:
goto _LLB2;_LLB1: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1E1=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1C5;if(_tmp1E1->tag != 16)goto _LLB3;}_LLB2: return 0;_LLB3:;_LLB4:({struct Cyc_String_pa_struct
_tmp1E8;_tmp1E8.tag=0;_tmp1E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));{void*_tmp1E6[1]={& _tmp1E8};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E7="atomic_typ:  bad type %s";
_tag_dyneither(_tmp1E7,sizeof(char),25);}),_tag_dyneither(_tmp1E6,sizeof(void*),
1));}});_LL90:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp1E9=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1EB;void*_tmp1EC;_LLBB: {struct Cyc_Absyn_PointerType_struct*
_tmp1EA=(struct Cyc_Absyn_PointerType_struct*)_tmp1E9;if(_tmp1EA->tag != 5)goto
_LLBD;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EB.elt_typ;}}_LLBC: {void*_tmp1ED=
Cyc_Tcutil_compress(_tmp1EC);_LLC0: {struct Cyc_Absyn_VoidType_struct*_tmp1EE=(
struct Cyc_Absyn_VoidType_struct*)_tmp1ED;if(_tmp1EE->tag != 0)goto _LLC2;}_LLC1:
return 1;_LLC2:;_LLC3: return 0;_LLBF:;}_LLBD:;_LLBE: return 0;_LLBA:;}static int Cyc_Toc_is_tvar(
void*t){void*_tmp1EF=Cyc_Tcutil_compress(t);_LLC5: {struct Cyc_Absyn_VarType_struct*
_tmp1F0=(struct Cyc_Absyn_VarType_struct*)_tmp1EF;if(_tmp1F0->tag != 2)goto _LLC7;}
_LLC6: return 1;_LLC7:;_LLC8: return 0;_LLC4:;}static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp1F1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1F3;union Cyc_Absyn_AggrInfoU _tmp1F4;struct Cyc_List_List*_tmp1F6;_LLCA: {
struct Cyc_Absyn_AggrType_struct*_tmp1F2=(struct Cyc_Absyn_AggrType_struct*)
_tmp1F1;if(_tmp1F2->tag != 12)goto _LLCC;else{_tmp1F3=_tmp1F2->f1;_tmp1F4=_tmp1F3.aggr_info;}}
_LLCB: {struct Cyc_Absyn_Aggrdecl*_tmp1F7=Cyc_Absyn_get_known_aggrdecl(_tmp1F4);
if(_tmp1F7->impl == 0)({void*_tmp1F8=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1F9="is_poly_field: type missing fields";
_tag_dyneither(_tmp1F9,sizeof(char),35);}),_tag_dyneither(_tmp1F8,sizeof(void*),
0));});_tmp1F6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F7->impl))->fields;
goto _LLCD;}_LLCC: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F5=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F1;if(_tmp1F5->tag != 13)goto _LLCE;else{_tmp1F6=_tmp1F5->f2;}}_LLCD: {struct
Cyc_Absyn_Aggrfield*_tmp1FA=Cyc_Absyn_lookup_field(_tmp1F6,f);if(_tmp1FA == 0)({
struct Cyc_String_pa_struct _tmp1FD;_tmp1FD.tag=0;_tmp1FD.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*f);{void*_tmp1FB[1]={& _tmp1FD};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1FC="is_poly_field: bad field %s";
_tag_dyneither(_tmp1FC,sizeof(char),28);}),_tag_dyneither(_tmp1FB,sizeof(void*),
1));}});return Cyc_Toc_is_void_star_or_tvar(_tmp1FA->type);}_LLCE:;_LLCF:({struct
Cyc_String_pa_struct _tmp200;_tmp200.tag=0;_tmp200.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp1FE[1]={& _tmp200};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1FF="is_poly_field: bad type %s";_tag_dyneither(_tmp1FF,sizeof(
char),27);}),_tag_dyneither(_tmp1FE,sizeof(void*),1));}});_LLC9:;}static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e){void*_tmp201=e->r;struct Cyc_Absyn_Exp*_tmp203;struct
_dyneither_ptr*_tmp204;struct Cyc_Absyn_Exp*_tmp206;struct _dyneither_ptr*_tmp207;
_LLD1: {struct Cyc_Absyn_AggrMember_e_struct*_tmp202=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp201;if(_tmp202->tag != 23)goto _LLD3;else{_tmp203=_tmp202->f1;_tmp204=_tmp202->f2;}}
_LLD2: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp203->topt))->v,_tmp204);_LLD3: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp205=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp201;if(_tmp205->tag != 24)goto _LLD5;else{
_tmp206=_tmp205->f1;_tmp207=_tmp205->f2;}}_LLD4: {void*_tmp208=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp206->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp20A;void*_tmp20B;_LLD8: {struct Cyc_Absyn_PointerType_struct*_tmp209=(struct
Cyc_Absyn_PointerType_struct*)_tmp208;if(_tmp209->tag != 5)goto _LLDA;else{_tmp20A=
_tmp209->f1;_tmp20B=_tmp20A.elt_typ;}}_LLD9: return Cyc_Toc_is_poly_field(_tmp20B,
_tmp207);_LLDA:;_LLDB:({struct Cyc_String_pa_struct _tmp20E;_tmp20E.tag=0;_tmp20E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp206->topt))->v));{void*_tmp20C[1]={& _tmp20E};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp20D="is_poly_project: bad type %s";_tag_dyneither(_tmp20D,sizeof(
char),29);}),_tag_dyneither(_tmp20C,sizeof(void*),1));}});_LLD7:;}_LLD5:;_LLD6:
return 0;_LLD0:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp20F=
_cycalloc(sizeof(*_tmp20F));_tmp20F->hd=s;_tmp20F->tl=0;_tmp20F;}),0);}static
struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp210=_cycalloc(sizeof(*
_tmp210));_tmp210->hd=s;_tmp210->tl=0;_tmp210;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return
Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*
Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp211[2];_tmp211[1]=s;_tmp211[0]=
rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp211,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({
struct Cyc_Absyn_Exp*_tmp212[2];_tmp212[1]=n;_tmp212[0]=s;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp212,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);else{return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*
_tmp213[2];_tmp213[1]=n;_tmp213[0]=s;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp213,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*
rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({
struct Cyc_Absyn_Exp*_tmp214[3];_tmp214[2]=n;_tmp214[1]=s;_tmp214[0]=rgn;((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp214,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*
_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->hd=e;_tmp215->tl=0;_tmp215;}),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**
throw_match_stmt_opt=0;if(throw_match_stmt_opt == 0)throw_match_stmt_opt=({struct
Cyc_Absyn_Stmt**_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=Cyc_Absyn_exp_stmt(
Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(0)),0);_tmp216;});return*
throw_match_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp217=e->r;union Cyc_Absyn_Cnst _tmp219;struct _dyneither_ptr _tmp21A;_LLDD: {
struct Cyc_Absyn_Const_e_struct*_tmp218=(struct Cyc_Absyn_Const_e_struct*)_tmp217;
if(_tmp218->tag != 0)goto _LLDF;else{_tmp219=_tmp218->f1;if((_tmp219.String_c).tag
!= 7)goto _LLDF;_tmp21A=(struct _dyneither_ptr)(_tmp219.String_c).val;}}_LLDE:
is_string=1;goto _LLDC;_LLDF:;_LLE0: goto _LLDC;_LLDC:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp21B=_cycalloc(sizeof(*_tmp21B));_tmp21B->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp21C=_cycalloc(sizeof(*_tmp21C));_tmp21C[0]=({
struct Cyc_Absyn_Var_d_struct _tmp21D;_tmp21D.tag=0;_tmp21D.f1=vd;_tmp21D;});
_tmp21C;}),0);_tmp21B->tl=Cyc_Toc_result_decls;_tmp21B;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple16*_tmp21E[3];_tmp21E[2]=({struct _tuple16*_tmp221=_cycalloc(sizeof(*
_tmp221));_tmp221->f1=0;_tmp221->f2=xplussz;_tmp221;});_tmp21E[1]=({struct
_tuple16*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220->f1=0;_tmp220->f2=xexp;
_tmp220;});_tmp21E[0]=({struct _tuple16*_tmp21F=_cycalloc(sizeof(*_tmp21F));
_tmp21F->f1=0;_tmp21F->f2=xexp;_tmp21F;});((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp21E,sizeof(struct _tuple16*),3));}),
0);return urm_exp;}}}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct
Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp223;int
_tmp224;struct Cyc_Toc_Env*_tmp222=nv;_tmp223=*_tmp222;_tmp224=_tmp223.toplevel;
return _tmp224;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*
nv,struct _tuple1*x){struct Cyc_Toc_Env _tmp226;struct Cyc_Dict_Dict _tmp227;struct
Cyc_Toc_Env*_tmp225=nv;_tmp226=*_tmp225;_tmp227=_tmp226.varmap;return((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp227,x);}static struct
Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*
_tmp228=_region_malloc(r,sizeof(*_tmp228));_tmp228->break_lab=(struct
_dyneither_ptr**)0;_tmp228->continue_lab=(struct _dyneither_ptr**)0;_tmp228->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp228->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp228->toplevel=(int)1;_tmp228->rgn=(struct _RegionHandle*)
r;_tmp228;});}static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp22A;struct _dyneither_ptr**_tmp22B;
struct _dyneither_ptr**_tmp22C;struct Cyc_Toc_FallthruInfo*_tmp22D;struct Cyc_Dict_Dict
_tmp22E;int _tmp22F;struct Cyc_Toc_Env*_tmp229=e;_tmp22A=*_tmp229;_tmp22B=_tmp22A.break_lab;
_tmp22C=_tmp22A.continue_lab;_tmp22D=_tmp22A.fallthru_info;_tmp22E=_tmp22A.varmap;
_tmp22F=_tmp22A.toplevel;return({struct Cyc_Toc_Env*_tmp230=_region_malloc(r,
sizeof(*_tmp230));_tmp230->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp22B);_tmp230->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp22C);_tmp230->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp22D;_tmp230->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp22E);_tmp230->toplevel=(
int)_tmp22F;_tmp230->rgn=(struct _RegionHandle*)r;_tmp230;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp232;struct _dyneither_ptr**_tmp233;struct _dyneither_ptr**_tmp234;struct Cyc_Toc_FallthruInfo*
_tmp235;struct Cyc_Dict_Dict _tmp236;int _tmp237;struct Cyc_Toc_Env*_tmp231=e;
_tmp232=*_tmp231;_tmp233=_tmp232.break_lab;_tmp234=_tmp232.continue_lab;_tmp235=
_tmp232.fallthru_info;_tmp236=_tmp232.varmap;_tmp237=_tmp232.toplevel;return({
struct Cyc_Toc_Env*_tmp238=_region_malloc(r,sizeof(*_tmp238));_tmp238->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp233);_tmp238->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp234);_tmp238->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp235;_tmp238->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp236);_tmp238->toplevel=(int)0;_tmp238->rgn=(struct _RegionHandle*)r;_tmp238;});}
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23A;struct _dyneither_ptr**_tmp23B;struct _dyneither_ptr**
_tmp23C;struct Cyc_Toc_FallthruInfo*_tmp23D;struct Cyc_Dict_Dict _tmp23E;int _tmp23F;
struct Cyc_Toc_Env*_tmp239=e;_tmp23A=*_tmp239;_tmp23B=_tmp23A.break_lab;_tmp23C=
_tmp23A.continue_lab;_tmp23D=_tmp23A.fallthru_info;_tmp23E=_tmp23A.varmap;
_tmp23F=_tmp23A.toplevel;return({struct Cyc_Toc_Env*_tmp240=_region_malloc(r,
sizeof(*_tmp240));_tmp240->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp23B);_tmp240->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp23C);_tmp240->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp23D;_tmp240->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp23E);_tmp240->toplevel=(
int)1;_tmp240->rgn=(struct _RegionHandle*)r;_tmp240;});}static struct Cyc_Toc_Env*
Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,
struct Cyc_Absyn_Exp*y){{union Cyc_Absyn_Nmspace _tmp241=(*x).f1;struct Cyc_List_List*
_tmp242;_LLE2: if((_tmp241.Rel_n).tag != 1)goto _LLE4;_tmp242=(struct Cyc_List_List*)(
_tmp241.Rel_n).val;_LLE3:({struct Cyc_String_pa_struct _tmp245;_tmp245.tag=0;
_tmp245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x));{void*_tmp243[1]={& _tmp245};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp244="Toc::add_varmap on Rel_n: %s\n";
_tag_dyneither(_tmp244,sizeof(char),30);}),_tag_dyneither(_tmp243,sizeof(void*),
1));}});_LLE4:;_LLE5: goto _LLE1;_LLE1:;}{struct Cyc_Toc_Env _tmp247;struct
_dyneither_ptr**_tmp248;struct _dyneither_ptr**_tmp249;struct Cyc_Toc_FallthruInfo*
_tmp24A;struct Cyc_Dict_Dict _tmp24B;int _tmp24C;struct Cyc_Toc_Env*_tmp246=e;
_tmp247=*_tmp246;_tmp248=_tmp247.break_lab;_tmp249=_tmp247.continue_lab;_tmp24A=
_tmp247.fallthru_info;_tmp24B=_tmp247.varmap;_tmp24C=_tmp247.toplevel;{struct Cyc_Dict_Dict
_tmp24D=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp24B),x,y);return({struct Cyc_Toc_Env*_tmp24E=
_region_malloc(r,sizeof(*_tmp24E));_tmp24E->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp248);_tmp24E->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp249);_tmp24E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24A;_tmp24E->varmap=(struct Cyc_Dict_Dict)_tmp24D;_tmp24E->toplevel=(int)
_tmp24C;_tmp24E->rgn=(struct _RegionHandle*)r;_tmp24E;});}}}static struct Cyc_Toc_Env*
Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp250;struct _dyneither_ptr**_tmp251;struct _dyneither_ptr**_tmp252;struct Cyc_Toc_FallthruInfo*
_tmp253;struct Cyc_Dict_Dict _tmp254;int _tmp255;struct Cyc_Toc_Env*_tmp24F=e;
_tmp250=*_tmp24F;_tmp251=_tmp250.break_lab;_tmp252=_tmp250.continue_lab;_tmp253=
_tmp250.fallthru_info;_tmp254=_tmp250.varmap;_tmp255=_tmp250.toplevel;return({
struct Cyc_Toc_Env*_tmp256=_region_malloc(r,sizeof(*_tmp256));_tmp256->break_lab=(
struct _dyneither_ptr**)0;_tmp256->continue_lab=(struct _dyneither_ptr**)0;_tmp256->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp253;_tmp256->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp254);_tmp256->toplevel=(int)_tmp255;_tmp256->rgn=(struct _RegionHandle*)r;
_tmp256;});}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){fallthru_vars=({struct Cyc_List_List*
_tmp257=_region_malloc(r,sizeof(*_tmp257));_tmp257->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp257->tl=fallthru_vars;_tmp257;});}fallthru_vars=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env _tmp259;struct _dyneither_ptr**_tmp25A;struct _dyneither_ptr**
_tmp25B;struct Cyc_Toc_FallthruInfo*_tmp25C;struct Cyc_Dict_Dict _tmp25D;int _tmp25E;
struct Cyc_Toc_Env*_tmp258=e;_tmp259=*_tmp258;_tmp25A=_tmp259.break_lab;_tmp25B=
_tmp259.continue_lab;_tmp25C=_tmp259.fallthru_info;_tmp25D=_tmp259.varmap;
_tmp25E=_tmp259.toplevel;{struct Cyc_Toc_Env _tmp260;struct Cyc_Dict_Dict _tmp261;
struct Cyc_Toc_Env*_tmp25F=next_case_env;_tmp260=*_tmp25F;_tmp261=_tmp260.varmap;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp264=
_region_malloc(r,sizeof(*_tmp264));_tmp264->label=fallthru_l;_tmp264->binders=
fallthru_vars;_tmp264->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp261);_tmp264;});
return({struct Cyc_Toc_Env*_tmp262=_region_malloc(r,sizeof(*_tmp262));_tmp262->break_lab=(
struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp263=_region_malloc(r,sizeof(*
_tmp263));_tmp263[0]=break_l;_tmp263;});_tmp262->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp25B);_tmp262->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp262->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25D);_tmp262->toplevel=(
int)_tmp25E;_tmp262->rgn=(struct _RegionHandle*)r;_tmp262;});}}}}static struct Cyc_Toc_Env*
Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct
_dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp266;struct _dyneither_ptr**_tmp267;
struct _dyneither_ptr**_tmp268;struct Cyc_Toc_FallthruInfo*_tmp269;struct Cyc_Dict_Dict
_tmp26A;int _tmp26B;struct Cyc_Toc_Env*_tmp265=e;_tmp266=*_tmp265;_tmp267=_tmp266.break_lab;
_tmp268=_tmp266.continue_lab;_tmp269=_tmp266.fallthru_info;_tmp26A=_tmp266.varmap;
_tmp26B=_tmp266.toplevel;return({struct Cyc_Toc_Env*_tmp26C=_region_malloc(r,
sizeof(*_tmp26C));_tmp26C->break_lab=(struct _dyneither_ptr**)({struct
_dyneither_ptr**_tmp26D=_region_malloc(r,sizeof(*_tmp26D));_tmp26D[0]=break_l;
_tmp26D;});_tmp26C->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)
_tmp268);_tmp26C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp26C->varmap=(
struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp26A);_tmp26C->toplevel=(int)_tmp26B;_tmp26C->rgn=(struct
_RegionHandle*)r;_tmp26C;});}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp26F;struct _dyneither_ptr**_tmp270;struct _dyneither_ptr**_tmp271;struct Cyc_Toc_FallthruInfo*
_tmp272;struct Cyc_Dict_Dict _tmp273;int _tmp274;struct Cyc_Toc_Env*_tmp26E=e;
_tmp26F=*_tmp26E;_tmp270=_tmp26F.break_lab;_tmp271=_tmp26F.continue_lab;_tmp272=
_tmp26F.fallthru_info;_tmp273=_tmp26F.varmap;_tmp274=_tmp26F.toplevel;return({
struct Cyc_Toc_Env*_tmp275=_region_malloc(r,sizeof(*_tmp275));_tmp275->break_lab=(
struct _dyneither_ptr**)0;_tmp275->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp271);_tmp275->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({
struct Cyc_Toc_FallthruInfo*_tmp276=_region_malloc(r,sizeof(*_tmp276));_tmp276->label=
next_l;_tmp276->binders=0;_tmp276->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);
_tmp276;});_tmp275->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp273);_tmp275->toplevel=(
int)_tmp274;_tmp275->rgn=(struct _RegionHandle*)r;_tmp275;});}static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct
Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct
Cyc_Absyn_Exp*e){void*_tmp277=e->annot;_LLE7: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp278=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp277;if(_tmp278->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLE9;}_LLE8: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLE9: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp279=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp277;if(_tmp279->tag != Cyc_CfFlowInfo_NotZero)
goto _LLEB;}_LLEA: return 0;_LLEB: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27A=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp277;if(_tmp27A->tag != Cyc_CfFlowInfo_IsZero)
goto _LLED;}_LLEC:({void*_tmp27D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp27E="dereference of NULL pointer";
_tag_dyneither(_tmp27E,sizeof(char),28);}),_tag_dyneither(_tmp27D,sizeof(void*),
0));});return 0;_LLED: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp27B=(struct Cyc_Absyn_EmptyAnnot_struct*)
_tmp277;if(_tmp27B->tag != Cyc_Absyn_EmptyAnnot)goto _LLEF;}_LLEE: return 0;_LLEF: {
struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp27C=(struct Cyc_CfFlowInfo_HasTagCmps_struct*)
_tmp277;if(_tmp27C->tag != Cyc_CfFlowInfo_HasTagCmps)goto _LLF1;}_LLF0:({void*
_tmp27F=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp280="compiler oddity: pointer compared to tag type";
_tag_dyneither(_tmp280,sizeof(char),46);}),_tag_dyneither(_tmp27F,sizeof(void*),
0));});return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LLF1:;_LLF2:({void*_tmp281=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp282="need_null_check";
_tag_dyneither(_tmp282,sizeof(char),16);}),_tag_dyneither(_tmp281,sizeof(void*),
0));});_LLE6:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp283=e->annot;struct Cyc_List_List*_tmp285;struct Cyc_List_List*_tmp287;
_LLF4: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp284=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmp283;if(_tmp284->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLF6;else{_tmp285=_tmp284->f1;}}
_LLF5: return _tmp285;_LLF6: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp286=(struct
Cyc_CfFlowInfo_NotZero_struct*)_tmp283;if(_tmp286->tag != Cyc_CfFlowInfo_NotZero)
goto _LLF8;else{_tmp287=_tmp286->f1;}}_LLF7: return _tmp287;_LLF8: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp288=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp283;if(_tmp288->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFA;}_LLF9:({void*_tmp28B=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp28C="dereference of NULL pointer";
_tag_dyneither(_tmp28C,sizeof(char),28);}),_tag_dyneither(_tmp28B,sizeof(void*),
0));});return 0;_LLFA: {struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp289=(struct Cyc_CfFlowInfo_HasTagCmps_struct*)
_tmp283;if(_tmp289->tag != Cyc_CfFlowInfo_HasTagCmps)goto _LLFC;}_LLFB: goto _LLFD;
_LLFC: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp28A=(struct Cyc_Absyn_EmptyAnnot_struct*)
_tmp283;if(_tmp28A->tag != Cyc_Absyn_EmptyAnnot)goto _LLFE;}_LLFD: return 0;_LLFE:;
_LLFF:({void*_tmp28D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp28E="get_relns";_tag_dyneither(_tmp28E,
sizeof(char),10);}),_tag_dyneither(_tmp28D,sizeof(void*),0));});_LLF3:;}static
int Cyc_Toc_check_const_array(unsigned int i,void*t){void*_tmp28F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp291;struct Cyc_Absyn_PtrAtts _tmp292;union Cyc_Absyn_Constraint*
_tmp293;union Cyc_Absyn_Constraint*_tmp294;struct Cyc_Absyn_ArrayInfo _tmp296;
struct Cyc_Absyn_Exp*_tmp297;_LL101: {struct Cyc_Absyn_PointerType_struct*_tmp290=(
struct Cyc_Absyn_PointerType_struct*)_tmp28F;if(_tmp290->tag != 5)goto _LL103;else{
_tmp291=_tmp290->f1;_tmp292=_tmp291.ptr_atts;_tmp293=_tmp292.bounds;_tmp294=
_tmp292.zero_term;}}_LL102: {void*_tmp298=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp293);struct Cyc_Absyn_Exp*
_tmp29B;_LL108: {struct Cyc_Absyn_DynEither_b_struct*_tmp299=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp298;if(_tmp299->tag != 0)goto _LL10A;}_LL109: return 0;_LL10A: {struct Cyc_Absyn_Upper_b_struct*
_tmp29A=(struct Cyc_Absyn_Upper_b_struct*)_tmp298;if(_tmp29A->tag != 1)goto _LL107;
else{_tmp29B=_tmp29A->f1;}}_LL10B: {unsigned int _tmp29D;int _tmp29E;struct
_tuple11 _tmp29C=Cyc_Evexp_eval_const_uint_exp(_tmp29B);_tmp29D=_tmp29C.f1;
_tmp29E=_tmp29C.f2;return _tmp29E  && i <= _tmp29D;}_LL107:;}_LL103: {struct Cyc_Absyn_ArrayType_struct*
_tmp295=(struct Cyc_Absyn_ArrayType_struct*)_tmp28F;if(_tmp295->tag != 9)goto
_LL105;else{_tmp296=_tmp295->f1;_tmp297=_tmp296.num_elts;}}_LL104: if(_tmp297 == 0)
return 0;{unsigned int _tmp2A0;int _tmp2A1;struct _tuple11 _tmp29F=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp297);_tmp2A0=_tmp29F.f1;_tmp2A1=_tmp29F.f2;return
_tmp2A1  && i <= _tmp2A0;}_LL105:;_LL106: return 0;_LL100:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp2A2=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp2A2->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp2A3=_tmp2A2->rop;
struct Cyc_Absyn_Vardecl*_tmp2A4;struct Cyc_Absyn_Vardecl*_tmp2A5;_LL10D: if((
_tmp2A3.LessNumelts).tag != 3)goto _LL10F;_tmp2A4=(struct Cyc_Absyn_Vardecl*)(
_tmp2A3.LessNumelts).val;_LL10E: _tmp2A5=_tmp2A4;goto _LL110;_LL10F: if((_tmp2A3.LessEqNumelts).tag
!= 5)goto _LL111;_tmp2A5=(struct Cyc_Absyn_Vardecl*)(_tmp2A3.LessEqNumelts).val;
_LL110: if(_tmp2A5 == v)return 1;else{goto _LL10C;}_LL111:;_LL112: continue;_LL10C:;}}
return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e){{void*_tmp2A6=e->r;void*_tmp2A8;struct Cyc_Absyn_Vardecl*
_tmp2AA;void*_tmp2AC;struct Cyc_Absyn_Vardecl*_tmp2AE;void*_tmp2B0;struct Cyc_Absyn_Vardecl*
_tmp2B2;void*_tmp2B4;struct Cyc_Absyn_Vardecl*_tmp2B6;enum Cyc_Absyn_Primop
_tmp2B8;struct Cyc_List_List*_tmp2B9;struct Cyc_List_List _tmp2BA;struct Cyc_Absyn_Exp*
_tmp2BB;_LL114: {struct Cyc_Absyn_Var_e_struct*_tmp2A7=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A6;if(_tmp2A7->tag != 1)goto _LL116;else{_tmp2A8=(void*)_tmp2A7->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2A9=(struct Cyc_Absyn_Pat_b_struct*)_tmp2A8;if(_tmp2A9->tag
!= 5)goto _LL116;else{_tmp2AA=_tmp2A9->f1;}}}}_LL115: _tmp2AE=_tmp2AA;goto _LL117;
_LL116: {struct Cyc_Absyn_Var_e_struct*_tmp2AB=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A6;if(_tmp2AB->tag != 1)goto _LL118;else{_tmp2AC=(void*)_tmp2AB->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2AD=(struct Cyc_Absyn_Local_b_struct*)_tmp2AC;if(
_tmp2AD->tag != 4)goto _LL118;else{_tmp2AE=_tmp2AD->f1;}}}}_LL117: _tmp2B2=_tmp2AE;
goto _LL119;_LL118: {struct Cyc_Absyn_Var_e_struct*_tmp2AF=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A6;if(_tmp2AF->tag != 1)goto _LL11A;else{_tmp2B0=(void*)_tmp2AF->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2B1=(struct Cyc_Absyn_Global_b_struct*)_tmp2B0;if(
_tmp2B1->tag != 1)goto _LL11A;else{_tmp2B2=_tmp2B1->f1;}}}}_LL119: _tmp2B6=_tmp2B2;
goto _LL11B;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2B3=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A6;if(_tmp2B3->tag != 1)goto _LL11C;else{_tmp2B4=(void*)_tmp2B3->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2B5=(struct Cyc_Absyn_Param_b_struct*)_tmp2B4;if(
_tmp2B5->tag != 3)goto _LL11C;else{_tmp2B6=_tmp2B5->f1;}}}}_LL11B: if(_tmp2B6->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2B6))return 1;goto _LL113;_LL11C: {
struct Cyc_Absyn_Primop_e_struct*_tmp2B7=(struct Cyc_Absyn_Primop_e_struct*)
_tmp2A6;if(_tmp2B7->tag != 3)goto _LL11E;else{_tmp2B8=_tmp2B7->f1;if(_tmp2B8 != Cyc_Absyn_Numelts)
goto _LL11E;_tmp2B9=_tmp2B7->f2;if(_tmp2B9 == 0)goto _LL11E;_tmp2BA=*_tmp2B9;
_tmp2BB=(struct Cyc_Absyn_Exp*)_tmp2BA.hd;}}_LL11D:{void*_tmp2BC=_tmp2BB->r;void*
_tmp2BE;struct Cyc_Absyn_Vardecl*_tmp2C0;void*_tmp2C2;struct Cyc_Absyn_Vardecl*
_tmp2C4;void*_tmp2C6;struct Cyc_Absyn_Vardecl*_tmp2C8;void*_tmp2CA;struct Cyc_Absyn_Vardecl*
_tmp2CC;_LL121: {struct Cyc_Absyn_Var_e_struct*_tmp2BD=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BC;if(_tmp2BD->tag != 1)goto _LL123;else{_tmp2BE=(void*)_tmp2BD->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2BF=(struct Cyc_Absyn_Pat_b_struct*)_tmp2BE;if(_tmp2BF->tag
!= 5)goto _LL123;else{_tmp2C0=_tmp2BF->f1;}}}}_LL122: _tmp2C4=_tmp2C0;goto _LL124;
_LL123: {struct Cyc_Absyn_Var_e_struct*_tmp2C1=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BC;if(_tmp2C1->tag != 1)goto _LL125;else{_tmp2C2=(void*)_tmp2C1->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2C3=(struct Cyc_Absyn_Local_b_struct*)_tmp2C2;if(
_tmp2C3->tag != 4)goto _LL125;else{_tmp2C4=_tmp2C3->f1;}}}}_LL124: _tmp2C8=_tmp2C4;
goto _LL126;_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp2C5=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BC;if(_tmp2C5->tag != 1)goto _LL127;else{_tmp2C6=(void*)_tmp2C5->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2C7=(struct Cyc_Absyn_Global_b_struct*)_tmp2C6;if(
_tmp2C7->tag != 1)goto _LL127;else{_tmp2C8=_tmp2C7->f1;}}}}_LL126: _tmp2CC=_tmp2C8;
goto _LL128;_LL127: {struct Cyc_Absyn_Var_e_struct*_tmp2C9=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BC;if(_tmp2C9->tag != 1)goto _LL129;else{_tmp2CA=(void*)_tmp2C9->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2CB=(struct Cyc_Absyn_Param_b_struct*)_tmp2CA;if(
_tmp2CB->tag != 3)goto _LL129;else{_tmp2CC=_tmp2CB->f1;}}}}_LL128: return _tmp2CC == 
v;_LL129:;_LL12A: goto _LL120;_LL120:;}goto _LL113;_LL11E:;_LL11F: goto _LL113;_LL113:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp2CD=a->r;void*_tmp2CF;struct Cyc_Absyn_Vardecl*
_tmp2D1;void*_tmp2D3;struct Cyc_Absyn_Vardecl*_tmp2D5;void*_tmp2D7;struct Cyc_Absyn_Vardecl*
_tmp2D9;void*_tmp2DB;struct Cyc_Absyn_Vardecl*_tmp2DD;_LL12C: {struct Cyc_Absyn_Var_e_struct*
_tmp2CE=(struct Cyc_Absyn_Var_e_struct*)_tmp2CD;if(_tmp2CE->tag != 1)goto _LL12E;
else{_tmp2CF=(void*)_tmp2CE->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2D0=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2CF;if(_tmp2D0->tag != 5)goto _LL12E;else{_tmp2D1=_tmp2D0->f1;}}}}_LL12D:
_tmp2D5=_tmp2D1;goto _LL12F;_LL12E: {struct Cyc_Absyn_Var_e_struct*_tmp2D2=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CD;if(_tmp2D2->tag != 1)goto _LL130;else{_tmp2D3=(
void*)_tmp2D2->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2D4=(struct Cyc_Absyn_Local_b_struct*)
_tmp2D3;if(_tmp2D4->tag != 4)goto _LL130;else{_tmp2D5=_tmp2D4->f1;}}}}_LL12F:
_tmp2D9=_tmp2D5;goto _LL131;_LL130: {struct Cyc_Absyn_Var_e_struct*_tmp2D6=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CD;if(_tmp2D6->tag != 1)goto _LL132;else{_tmp2D7=(
void*)_tmp2D6->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2D8=(struct Cyc_Absyn_Global_b_struct*)
_tmp2D7;if(_tmp2D8->tag != 1)goto _LL132;else{_tmp2D9=_tmp2D8->f1;}}}}_LL131:
_tmp2DD=_tmp2D9;goto _LL133;_LL132: {struct Cyc_Absyn_Var_e_struct*_tmp2DA=(struct
Cyc_Absyn_Var_e_struct*)_tmp2CD;if(_tmp2DA->tag != 1)goto _LL134;else{_tmp2DB=(
void*)_tmp2DA->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2DC=(struct Cyc_Absyn_Param_b_struct*)
_tmp2DB;if(_tmp2DC->tag != 3)goto _LL134;else{_tmp2DD=_tmp2DC->f1;}}}}_LL133: if(
_tmp2DD->escapes)return 0;inner_loop: {void*_tmp2DE=i->r;void*_tmp2E0;struct Cyc_Absyn_Exp*
_tmp2E1;union Cyc_Absyn_Cnst _tmp2E3;struct _tuple6 _tmp2E4;enum Cyc_Absyn_Sign
_tmp2E5;int _tmp2E6;union Cyc_Absyn_Cnst _tmp2E8;struct _tuple6 _tmp2E9;enum Cyc_Absyn_Sign
_tmp2EA;int _tmp2EB;union Cyc_Absyn_Cnst _tmp2ED;struct _tuple6 _tmp2EE;enum Cyc_Absyn_Sign
_tmp2EF;int _tmp2F0;enum Cyc_Absyn_Primop _tmp2F2;struct Cyc_List_List*_tmp2F3;
struct Cyc_List_List _tmp2F4;struct Cyc_Absyn_Exp*_tmp2F5;struct Cyc_List_List*
_tmp2F6;struct Cyc_List_List _tmp2F7;struct Cyc_Absyn_Exp*_tmp2F8;void*_tmp2FA;
struct Cyc_Absyn_Vardecl*_tmp2FC;void*_tmp2FE;struct Cyc_Absyn_Vardecl*_tmp300;
void*_tmp302;struct Cyc_Absyn_Vardecl*_tmp304;void*_tmp306;struct Cyc_Absyn_Vardecl*
_tmp308;_LL137: {struct Cyc_Absyn_Cast_e_struct*_tmp2DF=(struct Cyc_Absyn_Cast_e_struct*)
_tmp2DE;if(_tmp2DF->tag != 15)goto _LL139;else{_tmp2E0=(void*)_tmp2DF->f1;_tmp2E1=
_tmp2DF->f2;}}_LL138: i=_tmp2E1;goto inner_loop;_LL139: {struct Cyc_Absyn_Const_e_struct*
_tmp2E2=(struct Cyc_Absyn_Const_e_struct*)_tmp2DE;if(_tmp2E2->tag != 0)goto _LL13B;
else{_tmp2E3=_tmp2E2->f1;if((_tmp2E3.Int_c).tag != 4)goto _LL13B;_tmp2E4=(struct
_tuple6)(_tmp2E3.Int_c).val;_tmp2E5=_tmp2E4.f1;if(_tmp2E5 != Cyc_Absyn_None)goto
_LL13B;_tmp2E6=_tmp2E4.f2;}}_LL13A: _tmp2EB=_tmp2E6;goto _LL13C;_LL13B: {struct Cyc_Absyn_Const_e_struct*
_tmp2E7=(struct Cyc_Absyn_Const_e_struct*)_tmp2DE;if(_tmp2E7->tag != 0)goto _LL13D;
else{_tmp2E8=_tmp2E7->f1;if((_tmp2E8.Int_c).tag != 4)goto _LL13D;_tmp2E9=(struct
_tuple6)(_tmp2E8.Int_c).val;_tmp2EA=_tmp2E9.f1;if(_tmp2EA != Cyc_Absyn_Signed)
goto _LL13D;_tmp2EB=_tmp2E9.f2;}}_LL13C: return _tmp2EB >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp2EB + 1),_tmp2DD->type);_LL13D: {struct Cyc_Absyn_Const_e_struct*
_tmp2EC=(struct Cyc_Absyn_Const_e_struct*)_tmp2DE;if(_tmp2EC->tag != 0)goto _LL13F;
else{_tmp2ED=_tmp2EC->f1;if((_tmp2ED.Int_c).tag != 4)goto _LL13F;_tmp2EE=(struct
_tuple6)(_tmp2ED.Int_c).val;_tmp2EF=_tmp2EE.f1;if(_tmp2EF != Cyc_Absyn_Unsigned)
goto _LL13F;_tmp2F0=_tmp2EE.f2;}}_LL13E: return Cyc_Toc_check_const_array((
unsigned int)(_tmp2F0 + 1),_tmp2DD->type);_LL13F: {struct Cyc_Absyn_Primop_e_struct*
_tmp2F1=(struct Cyc_Absyn_Primop_e_struct*)_tmp2DE;if(_tmp2F1->tag != 3)goto _LL141;
else{_tmp2F2=_tmp2F1->f1;if(_tmp2F2 != Cyc_Absyn_Mod)goto _LL141;_tmp2F3=_tmp2F1->f2;
if(_tmp2F3 == 0)goto _LL141;_tmp2F4=*_tmp2F3;_tmp2F5=(struct Cyc_Absyn_Exp*)_tmp2F4.hd;
_tmp2F6=_tmp2F4.tl;if(_tmp2F6 == 0)goto _LL141;_tmp2F7=*_tmp2F6;_tmp2F8=(struct Cyc_Absyn_Exp*)
_tmp2F7.hd;}}_LL140: return Cyc_Toc_check_leq_size(relns,_tmp2DD,_tmp2F8);_LL141: {
struct Cyc_Absyn_Var_e_struct*_tmp2F9=(struct Cyc_Absyn_Var_e_struct*)_tmp2DE;if(
_tmp2F9->tag != 1)goto _LL143;else{_tmp2FA=(void*)_tmp2F9->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp2FB=(struct Cyc_Absyn_Pat_b_struct*)_tmp2FA;if(_tmp2FB->tag != 5)goto _LL143;
else{_tmp2FC=_tmp2FB->f1;}}}}_LL142: _tmp300=_tmp2FC;goto _LL144;_LL143: {struct
Cyc_Absyn_Var_e_struct*_tmp2FD=(struct Cyc_Absyn_Var_e_struct*)_tmp2DE;if(_tmp2FD->tag
!= 1)goto _LL145;else{_tmp2FE=(void*)_tmp2FD->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp2FF=(struct Cyc_Absyn_Local_b_struct*)_tmp2FE;if(_tmp2FF->tag != 4)goto _LL145;
else{_tmp300=_tmp2FF->f1;}}}}_LL144: _tmp304=_tmp300;goto _LL146;_LL145: {struct
Cyc_Absyn_Var_e_struct*_tmp301=(struct Cyc_Absyn_Var_e_struct*)_tmp2DE;if(_tmp301->tag
!= 1)goto _LL147;else{_tmp302=(void*)_tmp301->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp303=(struct Cyc_Absyn_Global_b_struct*)_tmp302;if(_tmp303->tag != 1)goto _LL147;
else{_tmp304=_tmp303->f1;}}}}_LL146: _tmp308=_tmp304;goto _LL148;_LL147: {struct
Cyc_Absyn_Var_e_struct*_tmp305=(struct Cyc_Absyn_Var_e_struct*)_tmp2DE;if(_tmp305->tag
!= 1)goto _LL149;else{_tmp306=(void*)_tmp305->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp307=(struct Cyc_Absyn_Param_b_struct*)_tmp306;if(_tmp307->tag != 3)goto _LL149;
else{_tmp308=_tmp307->f1;}}}}_LL148: if(_tmp308->escapes)return 0;{struct Cyc_List_List*
_tmp309=relns;for(0;_tmp309 != 0;_tmp309=_tmp309->tl){struct Cyc_CfFlowInfo_Reln*
_tmp30A=(struct Cyc_CfFlowInfo_Reln*)_tmp309->hd;if(_tmp30A->vd == _tmp308){union
Cyc_CfFlowInfo_RelnOp _tmp30B=_tmp30A->rop;struct Cyc_Absyn_Vardecl*_tmp30C;struct
_tuple12 _tmp30D;struct Cyc_Absyn_Vardecl*_tmp30E;void*_tmp30F;unsigned int _tmp310;
_LL14C: if((_tmp30B.LessNumelts).tag != 3)goto _LL14E;_tmp30C=(struct Cyc_Absyn_Vardecl*)(
_tmp30B.LessNumelts).val;_LL14D: if(_tmp2DD == _tmp30C)return 1;else{goto _LL14B;}
_LL14E: if((_tmp30B.LessVar).tag != 2)goto _LL150;_tmp30D=(struct _tuple12)(_tmp30B.LessVar).val;
_tmp30E=_tmp30D.f1;_tmp30F=_tmp30D.f2;_LL14F:{struct _tuple0 _tmp312=({struct
_tuple0 _tmp311;_tmp311.f1=Cyc_Tcutil_compress(_tmp30F);_tmp311.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(a->topt))->v);_tmp311;});void*_tmp313;
void*_tmp315;void*_tmp316;struct Cyc_Absyn_PtrInfo _tmp318;struct Cyc_Absyn_PtrAtts
_tmp319;union Cyc_Absyn_Constraint*_tmp31A;_LL155: _tmp313=_tmp312.f1;{struct Cyc_Absyn_TagType_struct*
_tmp314=(struct Cyc_Absyn_TagType_struct*)_tmp313;if(_tmp314->tag != 20)goto _LL157;
else{_tmp315=(void*)_tmp314->f1;}}_tmp316=_tmp312.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp317=(struct Cyc_Absyn_PointerType_struct*)_tmp316;if(_tmp317->tag != 5)goto
_LL157;else{_tmp318=_tmp317->f1;_tmp319=_tmp318.ptr_atts;_tmp31A=_tmp319.bounds;}}
_LL156:{void*_tmp31B=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp31A);struct Cyc_Absyn_Exp*_tmp31D;_LL15A: {struct Cyc_Absyn_Upper_b_struct*
_tmp31C=(struct Cyc_Absyn_Upper_b_struct*)_tmp31B;if(_tmp31C->tag != 1)goto _LL15C;
else{_tmp31D=_tmp31C->f1;}}_LL15B: {struct Cyc_Absyn_Exp*_tmp31E=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp315,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp31E,_tmp31D))return 1;goto _LL159;}_LL15C:;_LL15D:
goto _LL159;_LL159:;}goto _LL154;_LL157:;_LL158: goto _LL154;_LL154:;}{struct Cyc_List_List*
_tmp31F=relns;for(0;_tmp31F != 0;_tmp31F=_tmp31F->tl){struct Cyc_CfFlowInfo_Reln*
_tmp320=(struct Cyc_CfFlowInfo_Reln*)_tmp31F->hd;if(_tmp320->vd == _tmp30E){union
Cyc_CfFlowInfo_RelnOp _tmp321=_tmp320->rop;struct Cyc_Absyn_Vardecl*_tmp322;struct
Cyc_Absyn_Vardecl*_tmp323;unsigned int _tmp324;struct _tuple12 _tmp325;struct Cyc_Absyn_Vardecl*
_tmp326;_LL15F: if((_tmp321.LessEqNumelts).tag != 5)goto _LL161;_tmp322=(struct Cyc_Absyn_Vardecl*)(
_tmp321.LessEqNumelts).val;_LL160: _tmp323=_tmp322;goto _LL162;_LL161: if((_tmp321.LessNumelts).tag
!= 3)goto _LL163;_tmp323=(struct Cyc_Absyn_Vardecl*)(_tmp321.LessNumelts).val;
_LL162: if(_tmp2DD == _tmp323)return 1;goto _LL15E;_LL163: if((_tmp321.EqualConst).tag
!= 1)goto _LL165;_tmp324=(unsigned int)(_tmp321.EqualConst).val;_LL164: return Cyc_Toc_check_const_array(
_tmp324,_tmp2DD->type);_LL165: if((_tmp321.LessVar).tag != 2)goto _LL167;_tmp325=(
struct _tuple12)(_tmp321.LessVar).val;_tmp326=_tmp325.f1;_LL166: if(Cyc_Toc_check_leq_size_var(
relns,_tmp2DD,_tmp326))return 1;goto _LL15E;_LL167:;_LL168: goto _LL15E;_LL15E:;}}}
goto _LL14B;_LL150: if((_tmp30B.LessConst).tag != 4)goto _LL152;_tmp310=(
unsigned int)(_tmp30B.LessConst).val;_LL151: return Cyc_Toc_check_const_array(
_tmp310,_tmp2DD->type);_LL152:;_LL153: goto _LL14B;_LL14B:;}}}goto _LL136;_LL149:;
_LL14A: goto _LL136;_LL136:;}goto _LL12B;_LL134:;_LL135: goto _LL12B;_LL12B:;}return 0;}
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp327=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp328="Missing type in primop ";_tag_dyneither(_tmp328,sizeof(char),
24);}),_tag_dyneither(_tmp327,sizeof(void*),0));});return Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp329=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp32A="Missing type in primop ";
_tag_dyneither(_tmp32A,sizeof(char),24);}),_tag_dyneither(_tmp329,sizeof(void*),
0));});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct
_tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple10*_tmp32B=
_cycalloc(sizeof(*_tmp32B));_tmp32B->f1=Cyc_Toc_mt_tq;_tmp32B->f2=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp32B;});}static struct
_tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple16*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->f1=0;
_tmp32C->f2=e;_tmp32C;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp32D=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp32D): Cyc_Toc_malloc_ptr(_tmp32D));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp32D);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp32E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp32E != 0;_tmp32E=_tmp32E->tl){struct _tuple16 _tmp330;struct Cyc_List_List*
_tmp331;struct Cyc_Absyn_Exp*_tmp332;struct _tuple16*_tmp32F=(struct _tuple16*)
_tmp32E->hd;_tmp330=*_tmp32F;_tmp331=_tmp330.f1;_tmp332=_tmp330.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp331 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp331->tl != 0)({void*_tmp333=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp334="multiple designators in array";
_tag_dyneither(_tmp334,sizeof(char),30);}),_tag_dyneither(_tmp333,sizeof(void*),
0));});{void*_tmp335=(void*)_tmp331->hd;void*_tmp336=_tmp335;struct Cyc_Absyn_Exp*
_tmp338;_LL16A: {struct Cyc_Absyn_ArrayElement_struct*_tmp337=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp336;if(_tmp337->tag != 0)goto _LL16C;else{_tmp338=_tmp337->f1;}}_LL16B: Cyc_Toc_exp_to_c(
nv,_tmp338);e_index=_tmp338;goto _LL169;_LL16C: {struct Cyc_Absyn_FieldName_struct*
_tmp339=(struct Cyc_Absyn_FieldName_struct*)_tmp336;if(_tmp339->tag != 1)goto
_LL169;}_LL16D:({void*_tmp33A=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp33B="field name designators in array";
_tag_dyneither(_tmp33B,sizeof(char),32);}),_tag_dyneither(_tmp33A,sizeof(void*),
0));});_LL169:;}}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp33C=_tmp332->r;struct Cyc_List_List*_tmp33E;struct Cyc_Absyn_Vardecl*
_tmp340;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_Absyn_Exp*_tmp342;int _tmp343;void*
_tmp345;struct Cyc_List_List*_tmp346;_LL16F: {struct Cyc_Absyn_Array_e_struct*
_tmp33D=(struct Cyc_Absyn_Array_e_struct*)_tmp33C;if(_tmp33D->tag != 28)goto _LL171;
else{_tmp33E=_tmp33D->f1;}}_LL170: s=Cyc_Toc_init_array(nv,lval,_tmp33E,s);goto
_LL16E;_LL171: {struct Cyc_Absyn_Comprehension_e_struct*_tmp33F=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp33C;if(_tmp33F->tag != 29)goto _LL173;else{_tmp340=_tmp33F->f1;_tmp341=_tmp33F->f2;
_tmp342=_tmp33F->f3;_tmp343=_tmp33F->f4;}}_LL172: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp340,_tmp341,_tmp342,_tmp343,s,0);goto _LL16E;_LL173: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp344=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp33C;if(_tmp344->tag != 31)goto
_LL175;else{_tmp345=(void*)_tmp344->f1;_tmp346=_tmp344->f2;}}_LL174: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp345,_tmp346,s);goto _LL16E;_LL175:;_LL176: Cyc_Toc_exp_to_c(nv,_tmp332);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp332,0),s,0);goto _LL16E;_LL16E:;}}}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple1*_tmp347=vd->name;void*_tmp348=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp34A;struct _RegionHandle*_tmp34B;
struct Cyc_Toc_Env*_tmp349=nv;_tmp34A=*_tmp349;_tmp34B=_tmp34A.rgn;{struct Cyc_Toc_Env*
nv2=Cyc_Toc_add_varmap(_tmp34B,nv,_tmp347,Cyc_Absyn_varb_exp(_tmp347,(void*)({
struct Cyc_Absyn_Local_b_struct*_tmp357=_cycalloc(sizeof(*_tmp357));_tmp357[0]=({
struct Cyc_Absyn_Local_b_struct _tmp358;_tmp358.tag=4;_tmp358.f1=vd;_tmp358;});
_tmp357;}),0));struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp347,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp347,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp347,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp347,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp34C=e2->r;struct Cyc_List_List*_tmp34E;
struct Cyc_Absyn_Vardecl*_tmp350;struct Cyc_Absyn_Exp*_tmp351;struct Cyc_Absyn_Exp*
_tmp352;int _tmp353;void*_tmp355;struct Cyc_List_List*_tmp356;_LL178: {struct Cyc_Absyn_Array_e_struct*
_tmp34D=(struct Cyc_Absyn_Array_e_struct*)_tmp34C;if(_tmp34D->tag != 28)goto _LL17A;
else{_tmp34E=_tmp34D->f1;}}_LL179: body=Cyc_Toc_init_array(nv2,lval,_tmp34E,Cyc_Toc_skip_stmt_dl());
goto _LL177;_LL17A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp34F=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp34C;if(_tmp34F->tag != 29)goto _LL17C;else{_tmp350=_tmp34F->f1;_tmp351=_tmp34F->f2;
_tmp352=_tmp34F->f3;_tmp353=_tmp34F->f4;}}_LL17B: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp350,_tmp351,_tmp352,_tmp353,Cyc_Toc_skip_stmt_dl(),0);goto _LL177;
_LL17C: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp354=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp34C;if(_tmp354->tag != 31)goto _LL17E;else{_tmp355=(void*)_tmp354->f1;_tmp356=
_tmp354->f2;}}_LL17D: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp355,_tmp356,Cyc_Toc_skip_stmt_dl());
goto _LL177;_LL17E:;_LL17F: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL177;_LL177:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp348,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp347,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);}}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp359=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp359 != 0;_tmp359=_tmp359->tl){struct _tuple16 _tmp35B;
struct Cyc_List_List*_tmp35C;struct Cyc_Absyn_Exp*_tmp35D;struct _tuple16*_tmp35A=(
struct _tuple16*)_tmp359->hd;_tmp35B=*_tmp35A;_tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;
if(_tmp35C == 0)({void*_tmp35E=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp35F="empty designator list";
_tag_dyneither(_tmp35F,sizeof(char),22);}),_tag_dyneither(_tmp35E,sizeof(void*),
0));});if(_tmp35C->tl != 0)({void*_tmp360=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp361="too many designators in anonymous struct";
_tag_dyneither(_tmp361,sizeof(char),41);}),_tag_dyneither(_tmp360,sizeof(void*),
0));});{void*_tmp362=(void*)_tmp35C->hd;struct _dyneither_ptr*_tmp364;_LL181: {
struct Cyc_Absyn_FieldName_struct*_tmp363=(struct Cyc_Absyn_FieldName_struct*)
_tmp362;if(_tmp363->tag != 1)goto _LL183;else{_tmp364=_tmp363->f1;}}_LL182: {
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp364,0);{void*_tmp365=
_tmp35D->r;struct Cyc_List_List*_tmp367;struct Cyc_Absyn_Vardecl*_tmp369;struct Cyc_Absyn_Exp*
_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;int _tmp36C;void*_tmp36E;struct Cyc_List_List*
_tmp36F;_LL186: {struct Cyc_Absyn_Array_e_struct*_tmp366=(struct Cyc_Absyn_Array_e_struct*)
_tmp365;if(_tmp366->tag != 28)goto _LL188;else{_tmp367=_tmp366->f1;}}_LL187: s=Cyc_Toc_init_array(
nv,lval,_tmp367,s);goto _LL185;_LL188: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp368=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp365;if(_tmp368->tag != 29)
goto _LL18A;else{_tmp369=_tmp368->f1;_tmp36A=_tmp368->f2;_tmp36B=_tmp368->f3;
_tmp36C=_tmp368->f4;}}_LL189: s=Cyc_Toc_init_comprehension(nv,lval,_tmp369,
_tmp36A,_tmp36B,_tmp36C,s,0);goto _LL185;_LL18A: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp36D=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp365;if(_tmp36D->tag != 31)goto
_LL18C;else{_tmp36E=(void*)_tmp36D->f1;_tmp36F=_tmp36D->f2;}}_LL18B: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp36E,_tmp36F,s);goto _LL185;_LL18C:;_LL18D: Cyc_Toc_exp_to_c(nv,_tmp35D);
if(Cyc_Toc_is_poly_field(struct_type,_tmp364))_tmp35D=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp35D);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp35D,0),0),s,0);goto _LL185;_LL185:;}goto _LL180;}_LL183:;_LL184:({void*_tmp370=
0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp371="array designator in struct";_tag_dyneither(_tmp371,sizeof(
char),27);}),_tag_dyneither(_tmp370,sizeof(void*),0));});_LL180:;}}}return s;}
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,
struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){struct Cyc_Toc_Env _tmp373;
struct _RegionHandle*_tmp374;struct Cyc_Toc_Env*_tmp372=nv;_tmp373=*_tmp372;
_tmp374=_tmp373.rgn;{struct Cyc_List_List*_tmp375=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp374,Cyc_Toc_tup_to_c,es);void*_tmp376=Cyc_Toc_add_tuple_type(
_tmp375);struct _tuple1*_tmp377=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp378=
Cyc_Absyn_var_exp(_tmp377,0);struct Cyc_Absyn_Stmt*_tmp379=Cyc_Absyn_exp_stmt(
_tmp378,0);struct Cyc_Absyn_Exp*(*_tmp37A)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp37B=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp374,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp37B);for(0;_tmp37B != 0;(_tmp37B=
_tmp37B->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp37B->hd;struct
Cyc_Absyn_Exp*lval=_tmp37A(_tmp378,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp37C=e->r;struct Cyc_List_List*_tmp37E;struct Cyc_Absyn_Vardecl*_tmp380;
struct Cyc_Absyn_Exp*_tmp381;struct Cyc_Absyn_Exp*_tmp382;int _tmp383;_LL18F: {
struct Cyc_Absyn_Array_e_struct*_tmp37D=(struct Cyc_Absyn_Array_e_struct*)_tmp37C;
if(_tmp37D->tag != 28)goto _LL191;else{_tmp37E=_tmp37D->f1;}}_LL190: _tmp379=Cyc_Toc_init_array(
nv,lval,_tmp37E,_tmp379);goto _LL18E;_LL191: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp37F=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp37C;if(_tmp37F->tag != 29)
goto _LL193;else{_tmp380=_tmp37F->f1;_tmp381=_tmp37F->f2;_tmp382=_tmp37F->f3;
_tmp383=_tmp37F->f4;}}_LL192: _tmp379=Cyc_Toc_init_comprehension(nv,lval,_tmp380,
_tmp381,_tmp382,_tmp383,_tmp379,0);goto _LL18E;_LL193:;_LL194: Cyc_Toc_exp_to_c(nv,
e);_tmp379=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp37A(
_tmp378,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp379,0);goto _LL18E;_LL18E:;}}}
return Cyc_Toc_make_struct(nv,_tmp377,_tmp376,_tmp379,pointer,rgnopt,is_atomic);}}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp384=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp384 != 0;_tmp384=_tmp384->tl){struct Cyc_Absyn_Aggrfield*
_tmp385=(struct Cyc_Absyn_Aggrfield*)_tmp384->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp385->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}({void*
_tmp386=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp389;_tmp389.tag=0;_tmp389.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp387[1]={& _tmp389};Cyc_aprintf(({
const char*_tmp388="get_member_offset %s failed";_tag_dyneither(_tmp388,sizeof(
char),28);}),_tag_dyneither(_tmp387,sizeof(void*),1));}}),_tag_dyneither(_tmp386,
sizeof(void*),0));});}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp38A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp38B=Cyc_Absyn_var_exp(_tmp38A,0);struct Cyc_Absyn_Stmt*
_tmp38C=Cyc_Absyn_exp_stmt(_tmp38B,0);struct Cyc_Absyn_Exp*(*_tmp38D)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp38E=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp38F=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp391;union Cyc_Absyn_AggrInfoU _tmp392;
_LL196: {struct Cyc_Absyn_AggrType_struct*_tmp390=(struct Cyc_Absyn_AggrType_struct*)
_tmp38F;if(_tmp390->tag != 12)goto _LL198;else{_tmp391=_tmp390->f1;_tmp392=_tmp391.aggr_info;}}
_LL197: ad=Cyc_Absyn_get_known_aggrdecl(_tmp392);goto _LL195;_LL198:;_LL199:({void*
_tmp393=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp394="init_struct: bad struct type";_tag_dyneither(_tmp394,sizeof(
char),29);}),_tag_dyneither(_tmp393,sizeof(void*),0));});_LL195:;}{int
is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp396;struct _RegionHandle*_tmp397;struct Cyc_Toc_Env*_tmp395=
nv;_tmp396=*_tmp395;_tmp397=_tmp396.rgn;{struct Cyc_List_List*_tmp398=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp397,dles);for(0;
_tmp398 != 0;_tmp398=_tmp398->tl){struct _tuple16 _tmp39A;struct Cyc_List_List*
_tmp39B;struct Cyc_Absyn_Exp*_tmp39C;struct _tuple16*_tmp399=(struct _tuple16*)
_tmp398->hd;_tmp39A=*_tmp399;_tmp39B=_tmp39A.f1;_tmp39C=_tmp39A.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp39C->topt))->v);
if(_tmp39B == 0)({void*_tmp39D=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp39E="empty designator list";
_tag_dyneither(_tmp39E,sizeof(char),22);}),_tag_dyneither(_tmp39D,sizeof(void*),
0));});if(_tmp39B->tl != 0){struct _tuple1*_tmp39F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3A0=Cyc_Absyn_var_exp(_tmp39F,0);for(0;_tmp39B != 0;_tmp39B=_tmp39B->tl){void*
_tmp3A1=(void*)_tmp39B->hd;struct _dyneither_ptr*_tmp3A3;_LL19B: {struct Cyc_Absyn_FieldName_struct*
_tmp3A2=(struct Cyc_Absyn_FieldName_struct*)_tmp3A1;if(_tmp3A2->tag != 1)goto
_LL19D;else{_tmp3A3=_tmp3A2->f1;}}_LL19C: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3A3))_tmp3A0=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A0);_tmp38C=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38D(_tmp38B,_tmp3A3,0),_tmp3A0,0),0),
_tmp38C,0);goto _LL19A;_LL19D:;_LL19E:({void*_tmp3A4=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3A5="array designator in struct";
_tag_dyneither(_tmp3A5,sizeof(char),27);}),_tag_dyneither(_tmp3A4,sizeof(void*),
0));});_LL19A:;}Cyc_Toc_exp_to_c(nv,_tmp39C);_tmp38C=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(_tmp3A0,_tmp39C,0),0),_tmp38C,0);}else{void*_tmp3A6=(void*)
_tmp39B->hd;struct _dyneither_ptr*_tmp3A8;_LL1A0: {struct Cyc_Absyn_FieldName_struct*
_tmp3A7=(struct Cyc_Absyn_FieldName_struct*)_tmp3A6;if(_tmp3A7->tag != 1)goto
_LL1A2;else{_tmp3A8=_tmp3A7->f1;}}_LL1A1: {struct Cyc_Absyn_Exp*lval=_tmp38D(
_tmp38B,_tmp3A8,0);if(is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3A8);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*
lhs=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=
Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);_tmp38C=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
assn_exp,0),_tmp38C,0);lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{
void*_tmp3A9=_tmp39C->r;struct Cyc_List_List*_tmp3AB;struct Cyc_Absyn_Vardecl*
_tmp3AD;struct Cyc_Absyn_Exp*_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AF;int _tmp3B0;void*
_tmp3B2;struct Cyc_List_List*_tmp3B3;_LL1A5: {struct Cyc_Absyn_Array_e_struct*
_tmp3AA=(struct Cyc_Absyn_Array_e_struct*)_tmp3A9;if(_tmp3AA->tag != 28)goto _LL1A7;
else{_tmp3AB=_tmp3AA->f1;}}_LL1A6: _tmp38C=Cyc_Toc_init_array(nv,lval,_tmp3AB,
_tmp38C);goto _LL1A4;_LL1A7: {struct Cyc_Absyn_Comprehension_e_struct*_tmp3AC=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp3A9;if(_tmp3AC->tag != 29)goto _LL1A9;
else{_tmp3AD=_tmp3AC->f1;_tmp3AE=_tmp3AC->f2;_tmp3AF=_tmp3AC->f3;_tmp3B0=_tmp3AC->f4;}}
_LL1A8: _tmp38C=Cyc_Toc_init_comprehension(nv,lval,_tmp3AD,_tmp3AE,_tmp3AF,
_tmp3B0,_tmp38C,0);goto _LL1A4;_LL1A9: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3B1=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3A9;if(_tmp3B1->tag != 31)goto
_LL1AB;else{_tmp3B2=(void*)_tmp3B1->f1;_tmp3B3=_tmp3B1->f2;}}_LL1AA: _tmp38C=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3B2,_tmp3B3,_tmp38C);goto _LL1A4;_LL1AB:;_LL1AC: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp39C->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp39C);{struct Cyc_Absyn_Aggrfield*_tmp3B4=Cyc_Absyn_lookup_decl_field(ad,
_tmp3A8);if(Cyc_Toc_is_poly_field(struct_type,_tmp3A8) && !was_ptr_type)_tmp39C=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp39C);if(has_exists)_tmp39C=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3B4))->type),
_tmp39C);_tmp38C=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp39C,0),0),_tmp38C,0);goto _LL1A4;}}_LL1A4:;}goto _LL19F;}_LL1A2:;_LL1A3:({void*
_tmp3B5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3B6="array designator in struct";_tag_dyneither(_tmp3B6,sizeof(
char),27);}),_tag_dyneither(_tmp3B5,sizeof(void*),0));});_LL19F:;}}}return Cyc_Toc_make_struct(
nv,_tmp38A,_tmp38E,_tmp38C,pointer,rgnopt,is_atomic);}}struct _tuple18{struct Cyc_Core_Opt*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp*el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*
pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*
e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Increment_e_struct*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));
_tmp3B7[0]=({struct Cyc_Absyn_Increment_e_struct _tmp3B8;_tmp3B8.tag=5;_tmp3B8.f1=
e1;_tmp3B8.f2=incr;_tmp3B8;});_tmp3B7;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp3B9=e1->r;struct Cyc_Absyn_Stmt*_tmp3BB;void*_tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;struct Cyc_Absyn_Exp*_tmp3C0;struct _dyneither_ptr*_tmp3C1;int _tmp3C2;int
_tmp3C3;_LL1AE: {struct Cyc_Absyn_StmtExp_e_struct*_tmp3BA=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp3B9;if(_tmp3BA->tag != 38)goto _LL1B0;else{_tmp3BB=_tmp3BA->f1;}}_LL1AF: Cyc_Toc_lvalue_assign_stmt(
_tmp3BB,fs,f,f_env);goto _LL1AD;_LL1B0: {struct Cyc_Absyn_Cast_e_struct*_tmp3BC=(
struct Cyc_Absyn_Cast_e_struct*)_tmp3B9;if(_tmp3BC->tag != 15)goto _LL1B2;else{
_tmp3BD=(void*)_tmp3BC->f1;_tmp3BE=_tmp3BC->f2;}}_LL1B1: Cyc_Toc_lvalue_assign(
_tmp3BE,fs,f,f_env);goto _LL1AD;_LL1B2: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp3BF=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp3B9;if(_tmp3BF->tag != 23)goto
_LL1B4;else{_tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;_tmp3C2=_tmp3BF->f3;_tmp3C3=
_tmp3BF->f4;}}_LL1B3: e1->r=_tmp3C0->r;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->hd=_tmp3C1;
_tmp3C4->tl=fs;_tmp3C4;}),f,f_env);goto _LL1AD;_LL1B4:;_LL1B5: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}e1->r=(f(e1_copy,f_env))->r;
goto _LL1AD;}_LL1AD:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env){void*_tmp3C5=s->r;struct Cyc_Absyn_Exp*_tmp3C7;struct Cyc_Absyn_Decl*
_tmp3C9;struct Cyc_Absyn_Stmt*_tmp3CA;struct Cyc_Absyn_Stmt*_tmp3CC;_LL1B7: {
struct Cyc_Absyn_Exp_s_struct*_tmp3C6=(struct Cyc_Absyn_Exp_s_struct*)_tmp3C5;if(
_tmp3C6->tag != 1)goto _LL1B9;else{_tmp3C7=_tmp3C6->f1;}}_LL1B8: Cyc_Toc_lvalue_assign(
_tmp3C7,fs,f,f_env);goto _LL1B6;_LL1B9: {struct Cyc_Absyn_Decl_s_struct*_tmp3C8=(
struct Cyc_Absyn_Decl_s_struct*)_tmp3C5;if(_tmp3C8->tag != 12)goto _LL1BB;else{
_tmp3C9=_tmp3C8->f1;_tmp3CA=_tmp3C8->f2;}}_LL1BA: Cyc_Toc_lvalue_assign_stmt(
_tmp3CA,fs,f,f_env);goto _LL1B6;_LL1BB: {struct Cyc_Absyn_Seq_s_struct*_tmp3CB=(
struct Cyc_Absyn_Seq_s_struct*)_tmp3C5;if(_tmp3CB->tag != 2)goto _LL1BD;else{
_tmp3CC=_tmp3CB->f2;}}_LL1BC: Cyc_Toc_lvalue_assign_stmt(_tmp3CC,fs,f,f_env);goto
_LL1B6;_LL1BD:;_LL1BE:({struct Cyc_String_pa_struct _tmp3CF;_tmp3CF.tag=0;_tmp3CF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));{void*
_tmp3CD[1]={& _tmp3CF};Cyc_Toc_toc_impos(({const char*_tmp3CE="lvalue_assign_stmt: %s";
_tag_dyneither(_tmp3CE,sizeof(char),23);}),_tag_dyneither(_tmp3CD,sizeof(void*),
1));}});_LL1B6:;}static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*
e){void*_tmp3D0=e->r;void*_tmp3D2;void**_tmp3D3;struct Cyc_Absyn_Exp*_tmp3D4;
struct Cyc_Absyn_Exp**_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D7;_LL1C0: {struct Cyc_Absyn_Cast_e_struct*
_tmp3D1=(struct Cyc_Absyn_Cast_e_struct*)_tmp3D0;if(_tmp3D1->tag != 15)goto _LL1C2;
else{_tmp3D2=(void**)& _tmp3D1->f1;_tmp3D3=(void**)((void**)& _tmp3D1->f1);_tmp3D4=
_tmp3D1->f2;_tmp3D5=(struct Cyc_Absyn_Exp**)& _tmp3D1->f2;}}_LL1C1:*_tmp3D5=Cyc_Toc_push_address_exp(*
_tmp3D5);*_tmp3D3=Cyc_Absyn_cstar_typ(*_tmp3D3,Cyc_Toc_mt_tq);return e;_LL1C2: {
struct Cyc_Absyn_Deref_e_struct*_tmp3D6=(struct Cyc_Absyn_Deref_e_struct*)_tmp3D0;
if(_tmp3D6->tag != 22)goto _LL1C4;else{_tmp3D7=_tmp3D6->f1;}}_LL1C3: return _tmp3D7;
_LL1C4:;_LL1C5: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);({
struct Cyc_String_pa_struct _tmp3DA;_tmp3DA.tag=0;_tmp3DA.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp3D8[1]={& _tmp3DA};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3D9="can't take & of %s";_tag_dyneither(_tmp3D9,sizeof(char),19);}),
_tag_dyneither(_tmp3D8,sizeof(void*),1));}});_LL1BF:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({
struct Cyc_List_List*_tmp3DB=_region_malloc(r2,sizeof(*_tmp3DB));_tmp3DB->hd=(
void*)f((void*)x->hd,env);_tmp3DB->tl=0;_tmp3DB;});prev=result;for(x=x->tl;x != 0;
x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp3DC=_region_malloc(r2,sizeof(*_tmp3DC));_tmp3DC->hd=(void*)f((void*)x->hd,
env);_tmp3DC->tl=0;_tmp3DC;});prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,
x);}static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({struct
_tuple16*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));_tmp3DD->f1=0;_tmp3DD->f2=e;_tmp3DD;});}
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3DE=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp3E0;_LL1C7: {struct Cyc_Absyn_PointerType_struct*
_tmp3DF=(struct Cyc_Absyn_PointerType_struct*)_tmp3DE;if(_tmp3DF->tag != 5)goto
_LL1C9;else{_tmp3E0=_tmp3DF->f1;}}_LL1C8: return _tmp3E0;_LL1C9:;_LL1CA:({void*
_tmp3E1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3E2="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp3E2,sizeof(
char),28);}),_tag_dyneither(_tmp3E1,sizeof(void*),0));});_LL1C6:;}static struct
Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*
_tmp3E3=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Sign _tmp3E6;enum Cyc_Absyn_Size_of
_tmp3E7;enum Cyc_Absyn_Sign _tmp3E9;enum Cyc_Absyn_Size_of _tmp3EA;enum Cyc_Absyn_Sign
_tmp3EE;enum Cyc_Absyn_Size_of _tmp3EF;enum Cyc_Absyn_Sign _tmp3F1;enum Cyc_Absyn_Size_of
_tmp3F2;enum Cyc_Absyn_Sign _tmp3F4;enum Cyc_Absyn_Size_of _tmp3F5;_LL1CC: {struct
Cyc_Absyn_PointerType_struct*_tmp3E4=(struct Cyc_Absyn_PointerType_struct*)
_tmp3E3;if(_tmp3E4->tag != 5)goto _LL1CE;}_LL1CD: res=Cyc_Absyn_null_exp(0);goto
_LL1CB;_LL1CE: {struct Cyc_Absyn_IntType_struct*_tmp3E5=(struct Cyc_Absyn_IntType_struct*)
_tmp3E3;if(_tmp3E5->tag != 6)goto _LL1D0;else{_tmp3E6=_tmp3E5->f1;_tmp3E7=_tmp3E5->f2;
if(_tmp3E7 != Cyc_Absyn_Char_sz)goto _LL1D0;}}_LL1CF: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp3E6,'\000'),0);goto _LL1CB;_LL1D0: {struct Cyc_Absyn_IntType_struct*_tmp3E8=(
struct Cyc_Absyn_IntType_struct*)_tmp3E3;if(_tmp3E8->tag != 6)goto _LL1D2;else{
_tmp3E9=_tmp3E8->f1;_tmp3EA=_tmp3E8->f2;if(_tmp3EA != Cyc_Absyn_Short_sz)goto
_LL1D2;}}_LL1D1: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp3E9,0),0);goto
_LL1CB;_LL1D2: {struct Cyc_Absyn_EnumType_struct*_tmp3EB=(struct Cyc_Absyn_EnumType_struct*)
_tmp3E3;if(_tmp3EB->tag != 14)goto _LL1D4;}_LL1D3: goto _LL1D5;_LL1D4: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp3EC=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp3E3;if(_tmp3EC->tag != 15)goto
_LL1D6;}_LL1D5: _tmp3EE=Cyc_Absyn_Unsigned;goto _LL1D7;_LL1D6: {struct Cyc_Absyn_IntType_struct*
_tmp3ED=(struct Cyc_Absyn_IntType_struct*)_tmp3E3;if(_tmp3ED->tag != 6)goto _LL1D8;
else{_tmp3EE=_tmp3ED->f1;_tmp3EF=_tmp3ED->f2;if(_tmp3EF != Cyc_Absyn_Int_sz)goto
_LL1D8;}}_LL1D7: _tmp3F1=_tmp3EE;goto _LL1D9;_LL1D8: {struct Cyc_Absyn_IntType_struct*
_tmp3F0=(struct Cyc_Absyn_IntType_struct*)_tmp3E3;if(_tmp3F0->tag != 6)goto _LL1DA;
else{_tmp3F1=_tmp3F0->f1;_tmp3F2=_tmp3F0->f2;if(_tmp3F2 != Cyc_Absyn_Long_sz)goto
_LL1DA;}}_LL1D9: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp3F1,0),0);goto _LL1CB;
_LL1DA: {struct Cyc_Absyn_IntType_struct*_tmp3F3=(struct Cyc_Absyn_IntType_struct*)
_tmp3E3;if(_tmp3F3->tag != 6)goto _LL1DC;else{_tmp3F4=_tmp3F3->f1;_tmp3F5=_tmp3F3->f2;
if(_tmp3F5 != Cyc_Absyn_LongLong_sz)goto _LL1DC;}}_LL1DB: res=Cyc_Absyn_const_exp(
Cyc_Absyn_LongLong_c(_tmp3F4,(long long)0),0);goto _LL1CB;_LL1DC: {struct Cyc_Absyn_FloatType_struct*
_tmp3F6=(struct Cyc_Absyn_FloatType_struct*)_tmp3E3;if(_tmp3F6->tag != 7)goto
_LL1DE;}_LL1DD: goto _LL1DF;_LL1DE: {struct Cyc_Absyn_DoubleType_struct*_tmp3F7=(
struct Cyc_Absyn_DoubleType_struct*)_tmp3E3;if(_tmp3F7->tag != 8)goto _LL1E0;}
_LL1DF: res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp3F8="0.0";
_tag_dyneither(_tmp3F8,sizeof(char),4);})),0);goto _LL1CB;_LL1E0:;_LL1E1:({struct
Cyc_String_pa_struct _tmp3FB;_tmp3FB.tag=0;_tmp3FB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp3F9[1]={& _tmp3FB};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3FA="found non-zero type %s in generate_zero";_tag_dyneither(
_tmp3FA,sizeof(char),40);}),_tag_dyneither(_tmp3F9,sizeof(void*),1));}});_LL1CB:;}
res->topt=({struct Cyc_Core_Opt*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC->v=t;
_tmp3FC;});return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct
Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=
Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp3FD=Cyc_Toc_typ_to_c(elt_type);void*_tmp3FE=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp3FF=Cyc_Absyn_cstar_typ(_tmp3FD,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmp400=({struct Cyc_Core_Opt*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->v=
_tmp3FF;_tmp441;});struct Cyc_Absyn_Exp*xinit;{void*_tmp401=e1->r;struct Cyc_Absyn_Exp*
_tmp403;struct Cyc_Absyn_Exp*_tmp405;struct Cyc_Absyn_Exp*_tmp406;_LL1E3: {struct
Cyc_Absyn_Deref_e_struct*_tmp402=(struct Cyc_Absyn_Deref_e_struct*)_tmp401;if(
_tmp402->tag != 22)goto _LL1E5;else{_tmp403=_tmp402->f1;}}_LL1E4: if(!is_dyneither){
_tmp403=Cyc_Toc_cast_it(fat_ptr_type,_tmp403);_tmp403->topt=({struct Cyc_Core_Opt*
_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->v=fat_ptr_type;_tmp407;});}Cyc_Toc_exp_to_c(
nv,_tmp403);xinit=_tmp403;goto _LL1E2;_LL1E5: {struct Cyc_Absyn_Subscript_e_struct*
_tmp404=(struct Cyc_Absyn_Subscript_e_struct*)_tmp401;if(_tmp404->tag != 25)goto
_LL1E7;else{_tmp405=_tmp404->f1;_tmp406=_tmp404->f2;}}_LL1E6: if(!is_dyneither){
_tmp405=Cyc_Toc_cast_it(fat_ptr_type,_tmp405);_tmp405->topt=({struct Cyc_Core_Opt*
_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->v=fat_ptr_type;_tmp408;});}Cyc_Toc_exp_to_c(
nv,_tmp405);Cyc_Toc_exp_to_c(nv,_tmp406);xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({
struct Cyc_Absyn_Exp*_tmp409[3];_tmp409[2]=_tmp406;_tmp409[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp409[0]=_tmp405;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp409,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);goto _LL1E2;_LL1E7:;_LL1E8:({void*_tmp40A=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp40B="found bad lhs for zero-terminated pointer assignment";
_tag_dyneither(_tmp40B,sizeof(char),53);}),_tag_dyneither(_tmp40A,sizeof(void*),
0));});_LL1E2:;}{struct _tuple1*_tmp40C=Cyc_Toc_temp_var();struct Cyc_Toc_Env
_tmp40E;struct _RegionHandle*_tmp40F;struct Cyc_Toc_Env*_tmp40D=nv;_tmp40E=*
_tmp40D;_tmp40F=_tmp40E.rgn;{struct Cyc_Toc_Env*_tmp410=Cyc_Toc_add_varmap(
_tmp40F,nv,_tmp40C,Cyc_Absyn_var_exp(_tmp40C,0));struct Cyc_Absyn_Vardecl*_tmp411=({
struct Cyc_Absyn_Vardecl*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->sc=Cyc_Absyn_Public;
_tmp440->name=_tmp40C;_tmp440->tq=Cyc_Toc_mt_tq;_tmp440->type=_tmp3FE;_tmp440->initializer=(
struct Cyc_Absyn_Exp*)xinit;_tmp440->rgn=0;_tmp440->attributes=0;_tmp440->escapes=
0;_tmp440;});struct Cyc_Absyn_Local_b_struct*_tmp412=({struct Cyc_Absyn_Local_b_struct*
_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E[0]=({struct Cyc_Absyn_Local_b_struct
_tmp43F;_tmp43F.tag=4;_tmp43F.f1=_tmp411;_tmp43F;});_tmp43E;});struct Cyc_Absyn_Exp*
_tmp413=Cyc_Absyn_varb_exp(_tmp40C,(void*)_tmp412,0);_tmp413->topt=({struct Cyc_Core_Opt*
_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->v=fat_ptr_type;_tmp414;});{struct
Cyc_Absyn_Exp*_tmp415=Cyc_Absyn_deref_exp(_tmp413,0);_tmp415->topt=({struct Cyc_Core_Opt*
_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->v=elt_type;_tmp416;});Cyc_Toc_exp_to_c(
_tmp410,_tmp415);{struct _tuple1*_tmp417=Cyc_Toc_temp_var();_tmp410=Cyc_Toc_add_varmap(
_tmp40F,_tmp410,_tmp417,Cyc_Absyn_var_exp(_tmp417,0));{struct Cyc_Absyn_Vardecl*
_tmp418=({struct Cyc_Absyn_Vardecl*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->sc=
Cyc_Absyn_Public;_tmp43D->name=_tmp417;_tmp43D->tq=Cyc_Toc_mt_tq;_tmp43D->type=
_tmp3FD;_tmp43D->initializer=(struct Cyc_Absyn_Exp*)_tmp415;_tmp43D->rgn=0;
_tmp43D->attributes=0;_tmp43D->escapes=0;_tmp43D;});struct Cyc_Absyn_Local_b_struct*
_tmp419=({struct Cyc_Absyn_Local_b_struct*_tmp43B=_cycalloc(sizeof(*_tmp43B));
_tmp43B[0]=({struct Cyc_Absyn_Local_b_struct _tmp43C;_tmp43C.tag=4;_tmp43C.f1=
_tmp418;_tmp43C;});_tmp43B;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct
Cyc_Absyn_Exp*_tmp41A=Cyc_Absyn_varb_exp(_tmp417,(void*)_tmp419,0);_tmp41A->topt=
_tmp415->topt;z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp41A,
e2,0);z_init->topt=_tmp41A->topt;}Cyc_Toc_exp_to_c(_tmp410,z_init);{struct
_tuple1*_tmp41B=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*_tmp41C=({struct Cyc_Absyn_Vardecl*
_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->sc=Cyc_Absyn_Public;_tmp43A->name=
_tmp41B;_tmp43A->tq=Cyc_Toc_mt_tq;_tmp43A->type=_tmp3FD;_tmp43A->initializer=(
struct Cyc_Absyn_Exp*)z_init;_tmp43A->rgn=0;_tmp43A->attributes=0;_tmp43A->escapes=
0;_tmp43A;});struct Cyc_Absyn_Local_b_struct*_tmp41D=({struct Cyc_Absyn_Local_b_struct*
_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438[0]=({struct Cyc_Absyn_Local_b_struct
_tmp439;_tmp439.tag=4;_tmp439.f1=_tmp41C;_tmp439;});_tmp438;});_tmp410=Cyc_Toc_add_varmap(
_tmp40F,_tmp410,_tmp41B,Cyc_Absyn_var_exp(_tmp41B,0));{struct Cyc_Absyn_Exp*
_tmp41E=Cyc_Absyn_varb_exp(_tmp417,(void*)_tmp419,0);_tmp41E->topt=_tmp415->topt;{
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*
_tmp420=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp41E,_tmp41F,0);_tmp420->topt=({
struct Cyc_Core_Opt*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->v=Cyc_Absyn_sint_typ;
_tmp421;});Cyc_Toc_exp_to_c(_tmp410,_tmp420);{struct Cyc_Absyn_Exp*_tmp422=Cyc_Absyn_varb_exp(
_tmp41B,(void*)_tmp41D,0);_tmp422->topt=_tmp415->topt;{struct Cyc_Absyn_Exp*
_tmp423=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp424=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Neq,_tmp422,_tmp423,0);_tmp424->topt=({struct Cyc_Core_Opt*_tmp425=
_cycalloc(sizeof(*_tmp425));_tmp425->v=Cyc_Absyn_sint_typ;_tmp425;});Cyc_Toc_exp_to_c(
_tmp410,_tmp424);{struct Cyc_List_List*_tmp426=({struct Cyc_Absyn_Exp*_tmp437[2];
_tmp437[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0);_tmp437[0]=Cyc_Absyn_varb_exp(
_tmp40C,(void*)_tmp412,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp437,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp427=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp426,0),
_tmp427,0);{struct Cyc_Absyn_Exp*_tmp428=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp420,_tmp424,0),0);struct Cyc_Absyn_Stmt*_tmp429=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp42A=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp40C,(void*)_tmp412,0),Cyc_Toc_curr_sp,0);_tmp42A=Cyc_Toc_cast_it(
_tmp3FF,_tmp42A);{struct Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_deref_exp(_tmp42A,0);
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_assign_exp(_tmp42B,Cyc_Absyn_var_exp(
_tmp41B,0),0);struct Cyc_Absyn_Stmt*_tmp42D=Cyc_Absyn_exp_stmt(_tmp42C,0);_tmp42D=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp428,_tmp429,Cyc_Absyn_skip_stmt(
0),0),_tmp42D,0);_tmp42D=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp42E=
_cycalloc(sizeof(*_tmp42E));_tmp42E->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F[0]=({struct Cyc_Absyn_Var_d_struct
_tmp430;_tmp430.tag=0;_tmp430.f1=_tmp41C;_tmp430;});_tmp42F;});_tmp42E->loc=0;
_tmp42E;}),_tmp42D,0);_tmp42D=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp431=
_cycalloc(sizeof(*_tmp431));_tmp431->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432[0]=({struct Cyc_Absyn_Var_d_struct
_tmp433;_tmp433.tag=0;_tmp433.f1=_tmp418;_tmp433;});_tmp432;});_tmp431->loc=0;
_tmp431;}),_tmp42D,0);_tmp42D=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp434=
_cycalloc(sizeof(*_tmp434));_tmp434->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435[0]=({struct Cyc_Absyn_Var_d_struct
_tmp436;_tmp436.tag=0;_tmp436.f1=_tmp411;_tmp436;});_tmp435;});_tmp434->loc=0;
_tmp434;}),_tmp42D,0);e->r=Cyc_Toc_stmt_exp_r(_tmp42D);}}}}}}}}}}}}}}static void*
Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,
struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{
void*_tmp442=Cyc_Tcutil_compress(aggrtype);struct Cyc_Absyn_AggrInfo _tmp444;union
Cyc_Absyn_AggrInfoU _tmp445;_LL1EA: {struct Cyc_Absyn_AggrType_struct*_tmp443=(
struct Cyc_Absyn_AggrType_struct*)_tmp442;if(_tmp443->tag != 12)goto _LL1EC;else{
_tmp444=_tmp443->f1;_tmp445=_tmp444.aggr_info;}}_LL1EB: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp445);goto _LL1E9;_LL1EC:;_LL1ED:({void*_tmp446=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp449;_tmp449.tag=0;_tmp449.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));{
void*_tmp447[1]={& _tmp449};Cyc_aprintf(({const char*_tmp448="expecting union but found %s in check_tagged_union";
_tag_dyneither(_tmp448,sizeof(char),51);}),_tag_dyneither(_tmp447,sizeof(void*),
1));}}),_tag_dyneither(_tmp446,sizeof(void*),0));});_LL1E9:;}{struct _tuple1*
_tmp44A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp44B=Cyc_Absyn_var_exp(_tmp44A,
0);struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44B,f,0),
Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*_tmp44D=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
ad,f),0);struct Cyc_Absyn_Exp*_tmp44E=Cyc_Absyn_neq_exp(_tmp44C,_tmp44D,0);struct
Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44B,f,0),Cyc_Toc_val_sp,
0);struct Cyc_Absyn_Stmt*_tmp450=Cyc_Absyn_exp_stmt(_tmp44F,0);struct Cyc_Absyn_Stmt*
_tmp451=Cyc_Absyn_ifthenelse_stmt(_tmp44E,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp452=Cyc_Absyn_declare_stmt(_tmp44A,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp451,_tmp450,0),0);return Cyc_Toc_stmt_exp_r(
_tmp452);}}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp453=e->r;struct Cyc_Absyn_Exp*
_tmp455;struct Cyc_Absyn_Exp*_tmp457;struct _dyneither_ptr*_tmp458;int _tmp459;int*
_tmp45A;struct Cyc_Absyn_Exp*_tmp45C;struct _dyneither_ptr*_tmp45D;int _tmp45E;int*
_tmp45F;_LL1EF: {struct Cyc_Absyn_Cast_e_struct*_tmp454=(struct Cyc_Absyn_Cast_e_struct*)
_tmp453;if(_tmp454->tag != 15)goto _LL1F1;else{_tmp455=_tmp454->f2;}}_LL1F0:({void*
_tmp460=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp461="cast on lhs!";_tag_dyneither(_tmp461,sizeof(char),13);}),
_tag_dyneither(_tmp460,sizeof(void*),0));});_LL1F1: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp456=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp453;if(_tmp456->tag != 23)goto
_LL1F3;else{_tmp457=_tmp456->f1;_tmp458=_tmp456->f2;_tmp459=_tmp456->f4;_tmp45A=(
int*)& _tmp456->f4;}}_LL1F2: {void*_tmp462=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp457->topt))->v);struct Cyc_Absyn_AggrInfo _tmp464;union Cyc_Absyn_AggrInfoU
_tmp465;_LL1F8: {struct Cyc_Absyn_AggrType_struct*_tmp463=(struct Cyc_Absyn_AggrType_struct*)
_tmp462;if(_tmp463->tag != 12)goto _LL1FA;else{_tmp464=_tmp463->f1;_tmp465=_tmp464.aggr_info;}}
_LL1F9: {struct Cyc_Absyn_Aggrdecl*_tmp466=Cyc_Absyn_get_known_aggrdecl(_tmp465);*
f_tag=Cyc_Toc_get_member_offset(_tmp466,_tmp458);{struct _dyneither_ptr str=(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp46B;_tmp46B.tag=0;_tmp46B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp458);{struct Cyc_String_pa_struct
_tmp46A;_tmp46A.tag=0;_tmp46A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp466->name).f2);{void*_tmp468[2]={& _tmp46A,& _tmp46B};Cyc_aprintf(({const char*
_tmp469="_union_%s_%s";_tag_dyneither(_tmp469,sizeof(char),13);}),_tag_dyneither(
_tmp468,sizeof(void*),2));}}});*tagged_member_type=Cyc_Absyn_strct(({struct
_dyneither_ptr*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467[0]=str;_tmp467;}));
if(clear_read)*_tmp45A=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp466->impl))->tagged;}}_LL1FA:;_LL1FB: return 0;_LL1F7:;}_LL1F3: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp45B=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp453;if(_tmp45B->tag != 24)goto
_LL1F5;else{_tmp45C=_tmp45B->f1;_tmp45D=_tmp45B->f2;_tmp45E=_tmp45B->f4;_tmp45F=(
int*)& _tmp45B->f4;}}_LL1F4: {void*_tmp46C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp45C->topt))->v);struct Cyc_Absyn_PtrInfo _tmp46E;void*_tmp46F;
_LL1FD: {struct Cyc_Absyn_PointerType_struct*_tmp46D=(struct Cyc_Absyn_PointerType_struct*)
_tmp46C;if(_tmp46D->tag != 5)goto _LL1FF;else{_tmp46E=_tmp46D->f1;_tmp46F=_tmp46E.elt_typ;}}
_LL1FE: {void*_tmp470=Cyc_Tcutil_compress(_tmp46F);struct Cyc_Absyn_AggrInfo
_tmp472;union Cyc_Absyn_AggrInfoU _tmp473;_LL202: {struct Cyc_Absyn_AggrType_struct*
_tmp471=(struct Cyc_Absyn_AggrType_struct*)_tmp470;if(_tmp471->tag != 12)goto
_LL204;else{_tmp472=_tmp471->f1;_tmp473=_tmp472.aggr_info;}}_LL203: {struct Cyc_Absyn_Aggrdecl*
_tmp474=Cyc_Absyn_get_known_aggrdecl(_tmp473);*f_tag=Cyc_Toc_get_member_offset(
_tmp474,_tmp45D);{struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp479;_tmp479.tag=0;_tmp479.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp45D);{struct Cyc_String_pa_struct _tmp478;_tmp478.tag=0;_tmp478.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp474->name).f2);{void*_tmp476[2]={&
_tmp478,& _tmp479};Cyc_aprintf(({const char*_tmp477="_union_%s_%s";_tag_dyneither(
_tmp477,sizeof(char),13);}),_tag_dyneither(_tmp476,sizeof(void*),2));}}});*
tagged_member_type=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp475=_cycalloc(
sizeof(*_tmp475));_tmp475[0]=str;_tmp475;}));if(clear_read)*_tmp45F=0;return((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp474->impl))->tagged;}}_LL204:;
_LL205: return 0;_LL201:;}_LL1FF:;_LL200: return 0;_LL1FC:;}_LL1F5:;_LL1F6: return 0;
_LL1EE:;}static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*
e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int
tag_num,void*member_type){struct _tuple1*_tmp47A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp47A,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(
temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 0)s2=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*_tmp47B=Cyc_Absyn_neq_exp(
temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp47B,Cyc_Toc_throw_match_stmt(),
Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(
_tmp47A,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(
e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);return Cyc_Toc_stmt_exp_r(s1);}}struct _tuple19{
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){void*_tmp47C=e->r;if(e->topt == 0)({struct Cyc_String_pa_struct
_tmp47F;_tmp47F.tag=0;_tmp47F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp47D[1]={& _tmp47F};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp47E="exp_to_c: no type for %s";
_tag_dyneither(_tmp47E,sizeof(char),25);}),_tag_dyneither(_tmp47D,sizeof(void*),
1));}});{void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*
_tmp480=_tmp47C;union Cyc_Absyn_Cnst _tmp482;int _tmp483;struct _tuple1*_tmp486;void*
_tmp487;struct _tuple1*_tmp489;enum Cyc_Absyn_Primop _tmp48B;struct Cyc_List_List*
_tmp48C;struct Cyc_Absyn_Exp*_tmp48E;enum Cyc_Absyn_Incrementor _tmp48F;struct Cyc_Absyn_Exp*
_tmp491;struct Cyc_Core_Opt*_tmp492;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*
_tmp495;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*
_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*
_tmp49D;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*
_tmp4A2;struct Cyc_List_List*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_List_List*
_tmp4A6;struct Cyc_Absyn_VarargCallInfo*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A9;
struct Cyc_List_List*_tmp4AA;struct Cyc_Absyn_VarargCallInfo*_tmp4AB;struct Cyc_Absyn_VarargCallInfo
_tmp4AC;int _tmp4AD;struct Cyc_List_List*_tmp4AE;struct Cyc_Absyn_VarargInfo*
_tmp4AF;struct Cyc_Absyn_Exp*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*
_tmp4B5;struct Cyc_List_List*_tmp4B6;void*_tmp4B8;void**_tmp4B9;struct Cyc_Absyn_Exp*
_tmp4BA;int _tmp4BB;enum Cyc_Absyn_Coercion _tmp4BC;struct Cyc_Absyn_Exp*_tmp4BE;
struct Cyc_Absyn_Exp*_tmp4C0;struct Cyc_Absyn_Exp*_tmp4C1;struct Cyc_Absyn_Exp*
_tmp4C3;void*_tmp4C5;void*_tmp4C8;void*_tmp4C9;struct _dyneither_ptr*_tmp4CB;void*
_tmp4CD;void*_tmp4CE;unsigned int _tmp4D0;struct Cyc_Absyn_Exp*_tmp4D2;struct Cyc_Absyn_Exp*
_tmp4D4;struct _dyneither_ptr*_tmp4D5;int _tmp4D6;int _tmp4D7;struct Cyc_Absyn_Exp*
_tmp4D9;struct _dyneither_ptr*_tmp4DA;int _tmp4DB;int _tmp4DC;struct Cyc_Absyn_Exp*
_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DF;struct Cyc_List_List*_tmp4E1;struct Cyc_List_List*
_tmp4E3;struct Cyc_Absyn_Vardecl*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E6;struct Cyc_Absyn_Exp*
_tmp4E7;int _tmp4E8;struct _tuple1*_tmp4EA;struct Cyc_List_List*_tmp4EB;struct Cyc_List_List*
_tmp4EC;struct Cyc_Absyn_Aggrdecl*_tmp4ED;void*_tmp4EF;struct Cyc_List_List*
_tmp4F0;struct Cyc_List_List*_tmp4F2;struct Cyc_Absyn_Datatypedecl*_tmp4F3;struct
Cyc_Absyn_Datatypefield*_tmp4F4;struct Cyc_Absyn_MallocInfo _tmp4F8;int _tmp4F9;
struct Cyc_Absyn_Exp*_tmp4FA;void**_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FC;int _tmp4FD;
struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*
_tmp502;struct _dyneither_ptr*_tmp503;struct Cyc_Absyn_Stmt*_tmp505;_LL207: {
struct Cyc_Absyn_Const_e_struct*_tmp481=(struct Cyc_Absyn_Const_e_struct*)_tmp480;
if(_tmp481->tag != 0)goto _LL209;else{_tmp482=_tmp481->f1;if((_tmp482.Null_c).tag
!= 1)goto _LL209;_tmp483=(int)(_tmp482.Null_c).val;}}_LL208: {struct Cyc_Absyn_Exp*
_tmp509=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp509,
_tmp509))->r;else{e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp50A[3];_tmp50A[2]=_tmp509;_tmp50A[1]=_tmp509;_tmp50A[0]=_tmp509;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp50A,sizeof(struct Cyc_Absyn_Exp*),
3));}));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL206;}_LL209: {struct Cyc_Absyn_Const_e_struct*
_tmp484=(struct Cyc_Absyn_Const_e_struct*)_tmp480;if(_tmp484->tag != 0)goto _LL20B;}
_LL20A: goto _LL206;_LL20B: {struct Cyc_Absyn_Var_e_struct*_tmp485=(struct Cyc_Absyn_Var_e_struct*)
_tmp480;if(_tmp485->tag != 1)goto _LL20D;else{_tmp486=_tmp485->f1;_tmp487=(void*)
_tmp485->f2;}}_LL20C:{struct _handler_cons _tmp50B;_push_handler(& _tmp50B);{int
_tmp50D=0;if(setjmp(_tmp50B.handler))_tmp50D=1;if(!_tmp50D){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp486))->r;;_pop_handler();}else{void*_tmp50C=(void*)_exn_thrown;void*
_tmp50F=_tmp50C;_LL260: {struct Cyc_Dict_Absent_struct*_tmp510=(struct Cyc_Dict_Absent_struct*)
_tmp50F;if(_tmp510->tag != Cyc_Dict_Absent)goto _LL262;}_LL261:({struct Cyc_String_pa_struct
_tmp513;_tmp513.tag=0;_tmp513.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp486));{void*_tmp511[1]={& _tmp513};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp512="Can't find %s in exp_to_c, Var\n";
_tag_dyneither(_tmp512,sizeof(char),32);}),_tag_dyneither(_tmp511,sizeof(void*),
1));}});_LL262:;_LL263:(void)_throw(_tmp50F);_LL25F:;}}}goto _LL206;_LL20D: {
struct Cyc_Absyn_UnknownId_e_struct*_tmp488=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp480;if(_tmp488->tag != 2)goto _LL20F;else{_tmp489=_tmp488->f1;}}_LL20E:({void*
_tmp514=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp515="unknownid";_tag_dyneither(_tmp515,sizeof(char),10);}),
_tag_dyneither(_tmp514,sizeof(void*),0));});_LL20F: {struct Cyc_Absyn_Primop_e_struct*
_tmp48A=(struct Cyc_Absyn_Primop_e_struct*)_tmp480;if(_tmp48A->tag != 3)goto _LL211;
else{_tmp48B=_tmp48A->f1;_tmp48C=_tmp48A->f2;}}_LL210: {struct Cyc_List_List*
_tmp516=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp48C);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp48C);switch(_tmp48B){case Cyc_Absyn_Numelts: _LL264: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp48C))->hd;{void*_tmp517=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp519;struct Cyc_Absyn_Exp*
_tmp51A;struct Cyc_Absyn_PtrInfo _tmp51C;void*_tmp51D;struct Cyc_Absyn_PtrAtts
_tmp51E;union Cyc_Absyn_Constraint*_tmp51F;union Cyc_Absyn_Constraint*_tmp520;
union Cyc_Absyn_Constraint*_tmp521;_LL267: {struct Cyc_Absyn_ArrayType_struct*
_tmp518=(struct Cyc_Absyn_ArrayType_struct*)_tmp517;if(_tmp518->tag != 9)goto
_LL269;else{_tmp519=_tmp518->f1;_tmp51A=_tmp519.num_elts;}}_LL268: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp51A)))({void*_tmp522=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp523="can't calculate numelts";_tag_dyneither(_tmp523,sizeof(char),
24);}),_tag_dyneither(_tmp522,sizeof(void*),0));});e->r=_tmp51A->r;goto _LL266;
_LL269: {struct Cyc_Absyn_PointerType_struct*_tmp51B=(struct Cyc_Absyn_PointerType_struct*)
_tmp517;if(_tmp51B->tag != 5)goto _LL26B;else{_tmp51C=_tmp51B->f1;_tmp51D=_tmp51C.elt_typ;
_tmp51E=_tmp51C.ptr_atts;_tmp51F=_tmp51E.nullable;_tmp520=_tmp51E.bounds;_tmp521=
_tmp51E.zero_term;}}_LL26A:{void*_tmp524=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp520);struct Cyc_Absyn_Exp*
_tmp527;_LL26E: {struct Cyc_Absyn_DynEither_b_struct*_tmp525=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp524;if(_tmp525->tag != 0)goto _LL270;}_LL26F: e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({
struct Cyc_Absyn_Exp*_tmp528[2];_tmp528[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp51D),0);_tmp528[0]=(struct Cyc_Absyn_Exp*)_tmp48C->hd;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp528,sizeof(struct Cyc_Absyn_Exp*),
2));}));goto _LL26D;_LL270: {struct Cyc_Absyn_Upper_b_struct*_tmp526=(struct Cyc_Absyn_Upper_b_struct*)
_tmp524;if(_tmp526->tag != 1)goto _LL26D;else{_tmp527=_tmp526->f1;}}_LL271: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp521)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp48C->hd);e->r=Cyc_Toc_fncall_exp_r(function_e,({struct
Cyc_Absyn_Exp*_tmp529[2];_tmp529[1]=_tmp527;_tmp529[0]=(struct Cyc_Absyn_Exp*)
_tmp48C->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp529,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp51F)){if(!Cyc_Evexp_c_can_eval(
_tmp527))({void*_tmp52A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp52B="can't calculate numelts";
_tag_dyneither(_tmp52B,sizeof(char),24);}),_tag_dyneither(_tmp52A,sizeof(void*),
0));});e->r=Cyc_Toc_conditional_exp_r(arg,_tmp527,Cyc_Absyn_uint_exp(0,0));}
else{e->r=_tmp527->r;goto _LL26D;}}goto _LL26D;_LL26D:;}goto _LL266;_LL26B:;_LL26C:({
struct Cyc_String_pa_struct _tmp52F;_tmp52F.tag=0;_tmp52F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v));{struct Cyc_String_pa_struct _tmp52E;_tmp52E.tag=0;
_tmp52E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp52C[2]={&
_tmp52E,& _tmp52F};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp52D="size primop applied to non-array %s (%s)";_tag_dyneither(
_tmp52D,sizeof(char),41);}),_tag_dyneither(_tmp52C,sizeof(void*),2));}}});_LL266:;}
break;}case Cyc_Absyn_Plus: _LL265:{void*_tmp530=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp516))->hd);struct Cyc_Absyn_PtrInfo _tmp532;
void*_tmp533;struct Cyc_Absyn_PtrAtts _tmp534;union Cyc_Absyn_Constraint*_tmp535;
union Cyc_Absyn_Constraint*_tmp536;_LL274: {struct Cyc_Absyn_PointerType_struct*
_tmp531=(struct Cyc_Absyn_PointerType_struct*)_tmp530;if(_tmp531->tag != 5)goto
_LL276;else{_tmp532=_tmp531->f1;_tmp533=_tmp532.elt_typ;_tmp534=_tmp532.ptr_atts;
_tmp535=_tmp534.bounds;_tmp536=_tmp534.zero_term;}}_LL275:{void*_tmp537=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp535);struct Cyc_Absyn_Exp*_tmp53A;_LL279: {struct Cyc_Absyn_DynEither_b_struct*
_tmp538=(struct Cyc_Absyn_DynEither_b_struct*)_tmp537;if(_tmp538->tag != 0)goto
_LL27B;}_LL27A: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48C))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp48C->tl))->hd;e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({
struct Cyc_Absyn_Exp*_tmp53B[3];_tmp53B[2]=e2;_tmp53B[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp533),0);_tmp53B[0]=e1;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp53B,sizeof(struct Cyc_Absyn_Exp*),
3));}));goto _LL278;}_LL27B: {struct Cyc_Absyn_Upper_b_struct*_tmp539=(struct Cyc_Absyn_Upper_b_struct*)
_tmp537;if(_tmp539->tag != 1)goto _LL278;else{_tmp53A=_tmp539->f1;}}_LL27C: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp536)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp48C))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48C->tl))->hd;e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*_tmp53C[3];_tmp53C[
2]=e2;_tmp53C[1]=_tmp53A;_tmp53C[0]=e1;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp53C,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r;}goto _LL278;_LL278:;}goto _LL273;_LL276:;_LL277: goto _LL273;_LL273:;}
break;case Cyc_Absyn_Minus: _LL272: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp516))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48C))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48C->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp516->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({
struct Cyc_Absyn_Exp*_tmp53D[3];_tmp53D[2]=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,
0);_tmp53D[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp53D[0]=e1;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp53D,sizeof(struct Cyc_Absyn_Exp*),3));}));}}break;}case Cyc_Absyn_Eq: _LL27D:
goto _LL27E;case Cyc_Absyn_Neq: _LL27E: goto _LL27F;case Cyc_Absyn_Gt: _LL27F: goto
_LL280;case Cyc_Absyn_Gte: _LL280: goto _LL281;case Cyc_Absyn_Lt: _LL281: goto _LL282;
case Cyc_Absyn_Lte: _LL282: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp48C))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48C->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp516))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp516->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);if(Cyc_Tcutil_is_tagged_pointer_typ(
t2))e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
break;}default: _LL283: break;}goto _LL206;}_LL211: {struct Cyc_Absyn_Increment_e_struct*
_tmp48D=(struct Cyc_Absyn_Increment_e_struct*)_tmp480;if(_tmp48D->tag != 5)goto
_LL213;else{_tmp48E=_tmp48D->f1;_tmp48F=_tmp48D->f2;}}_LL212: {void*e2_cyc_typ=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp48E->topt))->v;void*ptr_type=(void*)&
Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int
is_dyneither=0;struct _dyneither_ptr incr_str=({const char*_tmp560="increment";
_tag_dyneither(_tmp560,sizeof(char),10);});if(_tmp48F == Cyc_Absyn_PreDec  || 
_tmp48F == Cyc_Absyn_PostDec)incr_str=({const char*_tmp53E="decrement";
_tag_dyneither(_tmp53E,sizeof(char),10);});if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp48E,& ptr_type,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct _tmp541;
_tmp541.tag=0;_tmp541.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);{
void*_tmp53F[1]={& _tmp541};Cyc_Tcutil_terr(e->loc,({const char*_tmp540="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dyneither(_tmp540,sizeof(char),74);}),_tag_dyneither(_tmp53F,sizeof(void*),
1));}});({void*_tmp542=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp543="in-place inc/dec on zero-term";
_tag_dyneither(_tmp543,sizeof(char),30);}),_tag_dyneither(_tmp542,sizeof(void*),
0));});}{void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(
Cyc_Toc_is_tagged_union_project(_tmp48E,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*
_tmp544=Cyc_Absyn_signed_int_exp(1,0);_tmp544->topt=({struct Cyc_Core_Opt*_tmp545=
_cycalloc(sizeof(*_tmp545));_tmp545->v=Cyc_Absyn_sint_typ;_tmp545;});switch(
_tmp48F){case Cyc_Absyn_PreInc: _LL285: e->r=(void*)({struct Cyc_Absyn_AssignOp_e_struct*
_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546[0]=({struct Cyc_Absyn_AssignOp_e_struct
_tmp547;_tmp547.tag=4;_tmp547.f1=_tmp48E;_tmp547.f2=({struct Cyc_Core_Opt*_tmp548=
_cycalloc_atomic(sizeof(*_tmp548));_tmp548->v=(void*)Cyc_Absyn_Plus;_tmp548;});
_tmp547.f3=_tmp544;_tmp547;});_tmp546;});Cyc_Toc_exp_to_c(nv,e);return;case Cyc_Absyn_PreDec:
_LL286: e->r=(void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmp549=_cycalloc(sizeof(*
_tmp549));_tmp549[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmp54A;_tmp54A.tag=4;
_tmp54A.f1=_tmp48E;_tmp54A.f2=({struct Cyc_Core_Opt*_tmp54B=_cycalloc_atomic(
sizeof(*_tmp54B));_tmp54B->v=(void*)Cyc_Absyn_Minus;_tmp54B;});_tmp54A.f3=
_tmp544;_tmp54A;});_tmp549;});Cyc_Toc_exp_to_c(nv,e);return;default: _LL287:({
struct Cyc_String_pa_struct _tmp54E;_tmp54E.tag=0;_tmp54E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)incr_str);{void*_tmp54C[1]={& _tmp54E};Cyc_Tcutil_terr(e->loc,({
const char*_tmp54D="in-place post-%s is not supported on @tagged union members";
_tag_dyneither(_tmp54D,sizeof(char),59);}),_tag_dyneither(_tmp54C,sizeof(void*),
1));}});({void*_tmp54F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp550="in-place inc/dec on @tagged union";
_tag_dyneither(_tmp550,sizeof(char),34);}),_tag_dyneither(_tmp54F,sizeof(void*),
0));});}}Cyc_Toc_exp_to_c(nv,_tmp48E);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
fn_e;int change=1;fn_e=(_tmp48F == Cyc_Absyn_PostInc  || _tmp48F == Cyc_Absyn_PostDec)?
Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp48F == Cyc_Absyn_PreDec  || _tmp48F == Cyc_Absyn_PostDec)change=- 1;e->r=Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp551[3];_tmp551[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp551[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp551[0]=
Cyc_Toc_push_address_exp(_tmp48E);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp551,sizeof(struct Cyc_Absyn_Exp*),3));}));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
fn_e;{void*_tmp552=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp48E->topt))->v);enum Cyc_Absyn_Size_of _tmp554;int _tmp557;_LL28A: {struct Cyc_Absyn_IntType_struct*
_tmp553=(struct Cyc_Absyn_IntType_struct*)_tmp552;if(_tmp553->tag != 6)goto _LL28C;
else{_tmp554=_tmp553->f2;}}_LL28B: switch(_tmp554){case Cyc_Absyn_Char_sz: _LL294:
fn_e=_tmp48F == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_char_e: Cyc_Toc__zero_arr_inplace_plus_char_e;
break;case Cyc_Absyn_Short_sz: _LL295: fn_e=_tmp48F == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_short_e:
Cyc_Toc__zero_arr_inplace_plus_short_e;break;case Cyc_Absyn_Int_sz: _LL296: fn_e=
_tmp48F == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_int_e: Cyc_Toc__zero_arr_inplace_plus_int_e;
break;default: _LL297:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_Core_Impossible_struct
_tmp55A;_tmp55A.tag=Cyc_Core_Impossible;_tmp55A.f1=({const char*_tmp55B="impossible IntType (not char, short or int)";
_tag_dyneither(_tmp55B,sizeof(char),44);});_tmp55A;});_tmp559;}));}goto _LL289;
_LL28C: {struct Cyc_Absyn_FloatType_struct*_tmp555=(struct Cyc_Absyn_FloatType_struct*)
_tmp552;if(_tmp555->tag != 7)goto _LL28E;}_LL28D: fn_e=_tmp48F == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_float_e: Cyc_Toc__zero_arr_inplace_plus_float_e;
goto _LL289;_LL28E: {struct Cyc_Absyn_DoubleType_struct*_tmp556=(struct Cyc_Absyn_DoubleType_struct*)
_tmp552;if(_tmp556->tag != 8)goto _LL290;else{_tmp557=_tmp556->f1;}}_LL28F: switch(
_tmp557){case 1: _LL299: fn_e=_tmp48F == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL29A: fn_e=_tmp48F == 
Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL289;_LL290: {struct Cyc_Absyn_PointerType_struct*_tmp558=(struct Cyc_Absyn_PointerType_struct*)
_tmp552;if(_tmp558->tag != 5)goto _LL292;}_LL291: fn_e=_tmp48F == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL289;_LL292:;_LL293:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C[0]=({struct Cyc_Core_Impossible_struct
_tmp55D;_tmp55D.tag=Cyc_Core_Impossible;_tmp55D.f1=({const char*_tmp55E="impossible expression type (not int, float, double, or pointer)";
_tag_dyneither(_tmp55E,sizeof(char),64);});_tmp55D;});_tmp55C;}));_LL289:;}e->r=
Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp55F[2];_tmp55F[1]=Cyc_Absyn_signed_int_exp(
1,0);_tmp55F[0]=_tmp48E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp55F,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp48E)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp48E,0,Cyc_Toc_incr_lvalue,_tmp48F);e->r=_tmp48E->r;}}}goto _LL206;}}}_LL213: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp490=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp480;if(_tmp490->tag != 4)goto _LL215;else{_tmp491=_tmp490->f1;_tmp492=_tmp490->f2;
_tmp493=_tmp490->f3;}}_LL214: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp491->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp493->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp491,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp491);Cyc_Toc_exp_to_c(nv,
_tmp493);e->r=Cyc_Toc_tagged_union_assignop(_tmp491,e1_old_typ,_tmp492,_tmp493,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp491,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp491,_tmp492,_tmp493,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp491);Cyc_Toc_exp_to_c(nv,_tmp491);Cyc_Toc_exp_to_c(nv,_tmp493);{int done=0;
if(_tmp492 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp492->v){case Cyc_Absyn_Plus: _LL29C: change=_tmp493;break;case Cyc_Absyn_Minus:
_LL29D: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp493,0);break;default:
_LL29E:({void*_tmp561=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp562="bad t ? pointer arithmetic";
_tag_dyneither(_tmp562,sizeof(char),27);}),_tag_dyneither(_tmp561,sizeof(void*),
0));});}done=1;{struct Cyc_Absyn_Exp*_tmp563=Cyc_Toc__dyneither_ptr_inplace_plus_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp563,({struct Cyc_Absyn_Exp*_tmp564[3];_tmp564[2]=
change;_tmp564[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp564[0]=
Cyc_Toc_push_address_exp(_tmp491);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp564,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)
_tmp492->v){case Cyc_Absyn_Plus: _LL2A0: done=1;e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp491),({struct Cyc_Absyn_Exp*
_tmp565[2];_tmp565[1]=_tmp493;_tmp565[0]=_tmp491;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp565,sizeof(struct Cyc_Absyn_Exp*),
2));}));break;default: _LL2A1:({void*_tmp566=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp567="bad zero-terminated pointer arithmetic";
_tag_dyneither(_tmp567,sizeof(char),39);}),_tag_dyneither(_tmp566,sizeof(void*),
0));});}}}if(!done){if(e1_poly)_tmp493->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp493->r,0));if(!Cyc_Absyn_is_lvalue(_tmp491)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
struct _tuple18*),struct _tuple18*f_env))Cyc_Toc_lvalue_assign)(_tmp491,0,Cyc_Toc_assignop_lvalue,({
struct _tuple18*_tmp568=_cycalloc(sizeof(struct _tuple18)* 1);_tmp568[0]=({struct
_tuple18 _tmp569;_tmp569.f1=_tmp492;_tmp569.f2=_tmp493;_tmp569;});_tmp568;}));e->r=
_tmp491->r;}}goto _LL206;}}}}_LL215: {struct Cyc_Absyn_Conditional_e_struct*
_tmp494=(struct Cyc_Absyn_Conditional_e_struct*)_tmp480;if(_tmp494->tag != 6)goto
_LL217;else{_tmp495=_tmp494->f1;_tmp496=_tmp494->f2;_tmp497=_tmp494->f3;}}_LL216:
Cyc_Toc_exp_to_c(nv,_tmp495);Cyc_Toc_exp_to_c(nv,_tmp496);Cyc_Toc_exp_to_c(nv,
_tmp497);goto _LL206;_LL217: {struct Cyc_Absyn_And_e_struct*_tmp498=(struct Cyc_Absyn_And_e_struct*)
_tmp480;if(_tmp498->tag != 7)goto _LL219;else{_tmp499=_tmp498->f1;_tmp49A=_tmp498->f2;}}
_LL218: Cyc_Toc_exp_to_c(nv,_tmp499);Cyc_Toc_exp_to_c(nv,_tmp49A);goto _LL206;
_LL219: {struct Cyc_Absyn_Or_e_struct*_tmp49B=(struct Cyc_Absyn_Or_e_struct*)
_tmp480;if(_tmp49B->tag != 8)goto _LL21B;else{_tmp49C=_tmp49B->f1;_tmp49D=_tmp49B->f2;}}
_LL21A: Cyc_Toc_exp_to_c(nv,_tmp49C);Cyc_Toc_exp_to_c(nv,_tmp49D);goto _LL206;
_LL21B: {struct Cyc_Absyn_SeqExp_e_struct*_tmp49E=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp480;if(_tmp49E->tag != 9)goto _LL21D;else{_tmp49F=_tmp49E->f1;_tmp4A0=_tmp49E->f2;}}
_LL21C: Cyc_Toc_exp_to_c(nv,_tmp49F);Cyc_Toc_exp_to_c(nv,_tmp4A0);goto _LL206;
_LL21D: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp4A1=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp480;if(_tmp4A1->tag != 10)goto _LL21F;else{_tmp4A2=_tmp4A1->f1;_tmp4A3=_tmp4A1->f2;}}
_LL21E: _tmp4A5=_tmp4A2;_tmp4A6=_tmp4A3;goto _LL220;_LL21F: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4A4=(struct Cyc_Absyn_FnCall_e_struct*)_tmp480;if(_tmp4A4->tag != 11)goto
_LL221;else{_tmp4A5=_tmp4A4->f1;_tmp4A6=_tmp4A4->f2;_tmp4A7=_tmp4A4->f3;if(
_tmp4A7 != 0)goto _LL221;}}_LL220: Cyc_Toc_exp_to_c(nv,_tmp4A5);((void(*)(void(*f)(
struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4A6);goto _LL206;_LL221: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4A8=(struct Cyc_Absyn_FnCall_e_struct*)_tmp480;if(_tmp4A8->tag != 11)goto
_LL223;else{_tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A8->f2;_tmp4AB=_tmp4A8->f3;if(
_tmp4AB == 0)goto _LL223;_tmp4AC=*_tmp4AB;_tmp4AD=_tmp4AC.num_varargs;_tmp4AE=
_tmp4AC.injectors;_tmp4AF=_tmp4AC.vai;}}_LL222: {struct Cyc_Toc_Env _tmp56B;struct
_RegionHandle*_tmp56C;struct Cyc_Toc_Env*_tmp56A=nv;_tmp56B=*_tmp56A;_tmp56C=
_tmp56B.rgn;{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4AD,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4AF->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4AA);int num_normargs=num_args - _tmp4AD;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4AA=_tmp4AA->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AA))->hd);
new_args=({struct Cyc_List_List*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->hd=(
struct Cyc_Absyn_Exp*)_tmp4AA->hd;_tmp56D->tl=new_args;_tmp56D;});}}new_args=({
struct Cyc_List_List*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp56F[3];_tmp56F[2]=
num_varargs_exp;_tmp56F[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp56F[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp56F,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp56E->tl=
new_args;_tmp56E;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4A9);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4A9,new_args,0),0);if(_tmp4AF->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp570=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4AF->type));struct Cyc_Absyn_DatatypeInfo _tmp572;union Cyc_Absyn_DatatypeInfoU
_tmp573;struct Cyc_Absyn_Datatypedecl**_tmp574;struct Cyc_Absyn_Datatypedecl*
_tmp575;_LL2A4: {struct Cyc_Absyn_DatatypeType_struct*_tmp571=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp570;if(_tmp571->tag != 3)goto _LL2A6;else{_tmp572=_tmp571->f1;_tmp573=_tmp572.datatype_info;
if((_tmp573.KnownDatatype).tag != 2)goto _LL2A6;_tmp574=(struct Cyc_Absyn_Datatypedecl**)(
_tmp573.KnownDatatype).val;_tmp575=*_tmp574;}}_LL2A5: tud=_tmp575;goto _LL2A3;
_LL2A6:;_LL2A7:({void*_tmp576=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp577="toc: unknown datatype in vararg with inject";
_tag_dyneither(_tmp577,sizeof(char),44);}),_tag_dyneither(_tmp576,sizeof(void*),
0));});_LL2A3:;}{struct _dyneither_ptr vs=({unsigned int _tmp581=(unsigned int)
_tmp4AD;struct _tuple1**_tmp582=(struct _tuple1**)_region_malloc(_tmp56C,
_check_times(sizeof(struct _tuple1*),_tmp581));struct _dyneither_ptr _tmp584=
_tag_dyneither(_tmp582,sizeof(struct _tuple1*),_tmp581);{unsigned int _tmp583=
_tmp581;unsigned int i;for(i=0;i < _tmp583;i ++){_tmp582[i]=Cyc_Toc_temp_var();}}
_tmp584;});if(_tmp4AD != 0){struct Cyc_List_List*_tmp578=0;{int i=_tmp4AD - 1;for(0;
i >= 0;-- i){_tmp578=({struct Cyc_List_List*_tmp579=_cycalloc(sizeof(*_tmp579));
_tmp579->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct
_tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));
_tmp579->tl=_tmp578;_tmp579;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct
Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp578,0),s,0);{int i=0;for(0;
_tmp4AA != 0;(((_tmp4AA=_tmp4AA->tl,_tmp4AE=_tmp4AE->tl)),++ i)){struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)_tmp4AA->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp57B;struct
_tuple1*_tmp57C;struct Cyc_List_List*_tmp57D;struct Cyc_Absyn_Datatypefield*
_tmp57A=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4AE))->hd;_tmp57B=*_tmp57A;_tmp57C=_tmp57B.name;_tmp57D=_tmp57B.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp57D))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp57C),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp57C,({const char*_tmp57E="_struct";_tag_dyneither(_tmp57E,sizeof(char),8);}))),
0,s,0);}}}}else{struct Cyc_List_List*_tmp57F=({struct _tuple16*_tmp580[3];_tmp580[
2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp580[1]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(
0,0));_tmp580[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp580,sizeof(struct
_tuple16*),3));});s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}}}else{{int i=0;for(0;_tmp4AA
!= 0;(_tmp4AA=_tmp4AA->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4AA->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4AA->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);}}goto _LL206;}_LL223: {struct Cyc_Absyn_Throw_e_struct*_tmp4B0=(struct Cyc_Absyn_Throw_e_struct*)
_tmp480;if(_tmp4B0->tag != 12)goto _LL225;else{_tmp4B1=_tmp4B0->f1;}}_LL224: Cyc_Toc_exp_to_c(
nv,_tmp4B1);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4B1),0))->r;goto _LL206;_LL225: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4B2=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp480;if(_tmp4B2->tag != 13)
goto _LL227;else{_tmp4B3=_tmp4B2->f1;}}_LL226: Cyc_Toc_exp_to_c(nv,_tmp4B3);goto
_LL206;_LL227: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4B4=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp480;if(_tmp4B4->tag != 14)goto _LL229;else{_tmp4B5=_tmp4B4->f1;_tmp4B6=_tmp4B4->f2;}}
_LL228: Cyc_Toc_exp_to_c(nv,_tmp4B5);for(0;_tmp4B6 != 0;_tmp4B6=_tmp4B6->tl){enum 
Cyc_Absyn_Kind k=Cyc_Tcutil_typ_kind((void*)_tmp4B6->hd);if(((k != Cyc_Absyn_EffKind
 && k != Cyc_Absyn_RgnKind) && k != Cyc_Absyn_UniqueRgnKind) && k != Cyc_Absyn_TopRgnKind){{
void*_tmp585=Cyc_Tcutil_compress((void*)_tmp4B6->hd);_LL2A9: {struct Cyc_Absyn_VarType_struct*
_tmp586=(struct Cyc_Absyn_VarType_struct*)_tmp585;if(_tmp586->tag != 2)goto _LL2AB;}
_LL2AA: goto _LL2AC;_LL2AB: {struct Cyc_Absyn_DatatypeType_struct*_tmp587=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp585;if(_tmp587->tag != 3)goto _LL2AD;}_LL2AC:
continue;_LL2AD:;_LL2AE: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4B5,0))->r;goto _LL2A8;_LL2A8:;}
break;}}goto _LL206;_LL229: {struct Cyc_Absyn_Cast_e_struct*_tmp4B7=(struct Cyc_Absyn_Cast_e_struct*)
_tmp480;if(_tmp4B7->tag != 15)goto _LL22B;else{_tmp4B8=(void**)& _tmp4B7->f1;
_tmp4B9=(void**)((void**)& _tmp4B7->f1);_tmp4BA=_tmp4B7->f2;_tmp4BB=_tmp4B7->f3;
_tmp4BC=_tmp4B7->f4;}}_LL22A: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4BA->topt))->v;void*new_typ=*_tmp4B9;*_tmp4B9=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp4BA);{struct _tuple0 _tmp589=({struct _tuple0
_tmp588;_tmp588.f1=Cyc_Tcutil_compress(old_t2);_tmp588.f2=Cyc_Tcutil_compress(
new_typ);_tmp588;});void*_tmp58A;struct Cyc_Absyn_PtrInfo _tmp58C;void*_tmp58D;
struct Cyc_Absyn_PtrInfo _tmp58F;void*_tmp590;struct Cyc_Absyn_PtrInfo _tmp592;void*
_tmp593;_LL2B0: _tmp58A=_tmp589.f1;{struct Cyc_Absyn_PointerType_struct*_tmp58B=(
struct Cyc_Absyn_PointerType_struct*)_tmp58A;if(_tmp58B->tag != 5)goto _LL2B2;else{
_tmp58C=_tmp58B->f1;}}_tmp58D=_tmp589.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp58E=(struct Cyc_Absyn_PointerType_struct*)_tmp58D;if(_tmp58E->tag != 5)goto
_LL2B2;else{_tmp58F=_tmp58E->f1;}}_LL2B1: {int _tmp595=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp58C.ptr_atts).nullable);int _tmp596=((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp58F.ptr_atts).nullable);
void*_tmp597=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp58C.ptr_atts).bounds);void*_tmp598=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp58F.ptr_atts).bounds);
int _tmp599=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp58C.ptr_atts).zero_term);int _tmp59A=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp58F.ptr_atts).zero_term);{struct _tuple0 _tmp59C=({
struct _tuple0 _tmp59B;_tmp59B.f1=_tmp597;_tmp59B.f2=_tmp598;_tmp59B;});void*
_tmp59D;struct Cyc_Absyn_Exp*_tmp59F;void*_tmp5A0;struct Cyc_Absyn_Exp*_tmp5A2;
void*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A5;void*_tmp5A6;void*_tmp5A8;void*_tmp5AA;
struct Cyc_Absyn_Exp*_tmp5AC;void*_tmp5AD;void*_tmp5AF;_LL2B7: _tmp59D=_tmp59C.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp59E=(struct Cyc_Absyn_Upper_b_struct*)_tmp59D;
if(_tmp59E->tag != 1)goto _LL2B9;else{_tmp59F=_tmp59E->f1;}}_tmp5A0=_tmp59C.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5A1=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A0;
if(_tmp5A1->tag != 1)goto _LL2B9;else{_tmp5A2=_tmp5A1->f1;}}_LL2B8: if((!Cyc_Evexp_c_can_eval(
_tmp59F) || !Cyc_Evexp_c_can_eval(_tmp5A2)) && !Cyc_Evexp_same_const_exp(_tmp59F,
_tmp5A2))({void*_tmp5B1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5B2="can't validate cast due to potential size differences";
_tag_dyneither(_tmp5B2,sizeof(char),54);}),_tag_dyneither(_tmp5B1,sizeof(void*),
0));});if(_tmp595  && !_tmp596){if(Cyc_Toc_is_toplevel(nv))({void*_tmp5B3=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp5B4="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp5B4,
sizeof(char),44);}),_tag_dyneither(_tmp5B3,sizeof(void*),0));});if(_tmp4BC != Cyc_Absyn_NonNull_to_Null)({
struct Cyc_String_pa_struct _tmp5B7;_tmp5B7.tag=0;_tmp5B7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp5B5[1]={& _tmp5B7};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp5B6="null-check conversion mis-classified: %s";_tag_dyneither(_tmp5B6,
sizeof(char),41);}),_tag_dyneither(_tmp5B5,sizeof(void*),1));}});{int
do_null_check=Cyc_Toc_need_null_check(_tmp4BA);if(do_null_check){if(!_tmp4BB)({
void*_tmp5B8=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp5B9="inserted null check due to implicit cast from * to @ type";
_tag_dyneither(_tmp5B9,sizeof(char),58);}),_tag_dyneither(_tmp5B8,sizeof(void*),
0));});e->r=Cyc_Toc_cast_it_r(*_tmp4B9,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->hd=_tmp4BA;
_tmp5BA->tl=0;_tmp5BA;}),0));}}}goto _LL2B6;_LL2B9: _tmp5A3=_tmp59C.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp5A4=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A3;if(_tmp5A4->tag != 1)goto _LL2BB;
else{_tmp5A5=_tmp5A4->f1;}}_tmp5A6=_tmp59C.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5A7=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5A6;if(_tmp5A7->tag != 0)goto
_LL2BB;}_LL2BA: if(!Cyc_Evexp_c_can_eval(_tmp5A5))({void*_tmp5BB=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp5BC="cannot perform coercion since numelts cannot be determined statically.";
_tag_dyneither(_tmp5BC,sizeof(char),71);}),_tag_dyneither(_tmp5BB,sizeof(void*),
0));});if(_tmp4BC == Cyc_Absyn_NonNull_to_Null)({struct Cyc_String_pa_struct
_tmp5BF;_tmp5BF.tag=0;_tmp5BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp5BD[1]={& _tmp5BF};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp5BE="conversion mis-classified as null-check: %s";
_tag_dyneither(_tmp5BE,sizeof(char),44);}),_tag_dyneither(_tmp5BD,sizeof(void*),
1));}});if(Cyc_Toc_is_toplevel(nv)){if((_tmp599  && !(_tmp58F.elt_tq).real_const)
 && !_tmp59A)_tmp5A5=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5A5,Cyc_Absyn_uint_exp(
1,0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5A5,_tmp4BA))->r;}else{
struct Cyc_Absyn_Exp*_tmp5C0=Cyc_Toc__tag_dyneither_e;if(_tmp599){struct _tuple1*
_tmp5C1=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5C2=Cyc_Absyn_var_exp(_tmp5C1,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5C3=_tmp4BA->r;union Cyc_Absyn_Cnst _tmp5C5;
struct _dyneither_ptr _tmp5C6;_LL2C0: {struct Cyc_Absyn_Const_e_struct*_tmp5C4=(
struct Cyc_Absyn_Const_e_struct*)_tmp5C3;if(_tmp5C4->tag != 0)goto _LL2C2;else{
_tmp5C5=_tmp5C4->f1;if((_tmp5C5.String_c).tag != 7)goto _LL2C2;_tmp5C6=(struct
_dyneither_ptr)(_tmp5C5.String_c).val;}}_LL2C1: arg3=_tmp5A5;goto _LL2BF;_LL2C2:;
_LL2C3: arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,
_tmp4BA),({struct Cyc_Absyn_Exp*_tmp5C7[2];_tmp5C7[1]=_tmp5A5;_tmp5C7[0]=_tmp5C2;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp5C7,sizeof(struct Cyc_Absyn_Exp*),2));}),0);goto _LL2BF;_LL2BF:;}if(!_tmp59A
 && !(_tmp58F.elt_tq).real_const)arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,
Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5C8=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp58F.elt_typ),0);struct Cyc_Absyn_Exp*_tmp5C9=Cyc_Absyn_fncall_exp(
_tmp5C0,({struct Cyc_Absyn_Exp*_tmp5CB[3];_tmp5CB[2]=arg3;_tmp5CB[1]=_tmp5C8;
_tmp5CB[0]=_tmp5C2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5CB,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp5CA=Cyc_Absyn_exp_stmt(_tmp5C9,0);_tmp5CA=Cyc_Absyn_declare_stmt(_tmp5C1,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp4BA,_tmp5CA,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5CA);}}
else{e->r=Cyc_Toc_fncall_exp_r(_tmp5C0,({struct Cyc_Absyn_Exp*_tmp5CC[3];_tmp5CC[
2]=_tmp5A5;_tmp5CC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58F.elt_typ),
0);_tmp5CC[0]=_tmp4BA;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5CC,sizeof(struct Cyc_Absyn_Exp*),3));}));}}goto _LL2B6;_LL2BB:
_tmp5A8=_tmp59C.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5A9=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5A8;if(_tmp5A9->tag != 0)goto _LL2BD;}_tmp5AA=_tmp59C.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5AB=(struct Cyc_Absyn_Upper_b_struct*)_tmp5AA;if(_tmp5AB->tag != 1)goto _LL2BD;
else{_tmp5AC=_tmp5AB->f1;}}_LL2BC: if(!Cyc_Evexp_c_can_eval(_tmp5AC))({void*
_tmp5CD=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5CE="cannot perform coercion since numelts cannot be determined statically.";
_tag_dyneither(_tmp5CE,sizeof(char),71);}),_tag_dyneither(_tmp5CD,sizeof(void*),
0));});if(Cyc_Toc_is_toplevel(nv))({void*_tmp5CF=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5D0="can't coerce t? to t* or t@ at the top-level";
_tag_dyneither(_tmp5D0,sizeof(char),45);}),_tag_dyneither(_tmp5CF,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp5D1=_tmp5AC;if(_tmp599  && !_tmp59A)_tmp5D1=Cyc_Absyn_add_exp(
_tmp5AC,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5D2=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp5D3=Cyc_Absyn_fncall_exp(_tmp5D2,({struct Cyc_Absyn_Exp*
_tmp5D5[3];_tmp5D5[2]=_tmp5D1;_tmp5D5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp58C.elt_typ),0);_tmp5D5[0]=_tmp4BA;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D5,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp596)_tmp5D3->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->hd=Cyc_Absyn_copy_exp(
_tmp5D3);_tmp5D4->tl=0;_tmp5D4;}));e->r=Cyc_Toc_cast_it_r(*_tmp4B9,_tmp5D3);goto
_LL2B6;}}_LL2BD: _tmp5AD=_tmp59C.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5AE=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp5AD;if(_tmp5AE->tag != 0)goto _LL2B6;}
_tmp5AF=_tmp59C.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5B0=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5AF;if(_tmp5B0->tag != 0)goto _LL2B6;}_LL2BE: DynCast: if((_tmp599  && !_tmp59A)
 && !(_tmp58F.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({void*_tmp5D6=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const
char*_tmp5D7="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dyneither(_tmp5D7,sizeof(char),70);}),_tag_dyneither(_tmp5D6,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp5D8=Cyc_Toc__dyneither_ptr_decrease_size_e;e->r=
Cyc_Toc_fncall_exp_r(_tmp5D8,({struct Cyc_Absyn_Exp*_tmp5D9[3];_tmp5D9[2]=Cyc_Absyn_uint_exp(
1,0);_tmp5D9[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58C.elt_typ),0);
_tmp5D9[0]=_tmp4BA;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5D9,sizeof(struct Cyc_Absyn_Exp*),3));}));}}goto _LL2B6;_LL2B6:;}
goto _LL2AF;}_LL2B2: _tmp590=_tmp589.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp591=(struct Cyc_Absyn_PointerType_struct*)_tmp590;if(_tmp591->tag != 5)goto
_LL2B4;else{_tmp592=_tmp591->f1;}}_tmp593=_tmp589.f2;{struct Cyc_Absyn_IntType_struct*
_tmp594=(struct Cyc_Absyn_IntType_struct*)_tmp593;if(_tmp594->tag != 6)goto _LL2B4;}
_LL2B3:{void*_tmp5DA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp592.ptr_atts).bounds);_LL2C5: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5DB=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5DA;if(_tmp5DB->tag != 0)goto
_LL2C7;}_LL2C6: _tmp4BA->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4BA->r,
_tmp4BA->loc),Cyc_Toc_curr_sp);goto _LL2C4;_LL2C7:;_LL2C8: goto _LL2C4;_LL2C4:;}
goto _LL2AF;_LL2B4:;_LL2B5: goto _LL2AF;_LL2AF:;}goto _LL206;}_LL22B: {struct Cyc_Absyn_Address_e_struct*
_tmp4BD=(struct Cyc_Absyn_Address_e_struct*)_tmp480;if(_tmp4BD->tag != 16)goto
_LL22D;else{_tmp4BE=_tmp4BD->f1;}}_LL22C:{void*_tmp5DC=_tmp4BE->r;struct _tuple1*
_tmp5DE;struct Cyc_List_List*_tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*
_tmp5E2;_LL2CA: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5DD=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5DC;if(_tmp5DD->tag != 30)goto _LL2CC;else{_tmp5DE=_tmp5DD->f1;_tmp5DF=_tmp5DD->f2;
_tmp5E0=_tmp5DD->f3;}}_LL2CB: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp5E5;_tmp5E5.tag=0;_tmp5E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4BE->loc));{void*_tmp5E3[1]={& _tmp5E5};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp5E4="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp5E4,
sizeof(char),42);}),_tag_dyneither(_tmp5E3,sizeof(void*),1));}});e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4BE->topt))->v,_tmp5DF != 0,1,0,
_tmp5E0,_tmp5DE))->r;goto _LL2C9;_LL2CC: {struct Cyc_Absyn_Tuple_e_struct*_tmp5E1=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5DC;if(_tmp5E1->tag != 26)goto _LL2CE;else{
_tmp5E2=_tmp5E1->f1;}}_LL2CD: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp5E8;_tmp5E8.tag=0;_tmp5E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4BE->loc));{void*_tmp5E6[1]={& _tmp5E8};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp5E7="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp5E7,
sizeof(char),42);}),_tag_dyneither(_tmp5E6,sizeof(void*),1));}});e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5E2))->r;goto _LL2C9;_LL2CE:;_LL2CF: Cyc_Toc_exp_to_c(nv,_tmp4BE);if(!
Cyc_Absyn_is_lvalue(_tmp4BE)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4BE,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4BE);}goto _LL2C9;
_LL2C9:;}goto _LL206;_LL22D: {struct Cyc_Absyn_New_e_struct*_tmp4BF=(struct Cyc_Absyn_New_e_struct*)
_tmp480;if(_tmp4BF->tag != 17)goto _LL22F;else{_tmp4C0=_tmp4BF->f1;_tmp4C1=_tmp4BF->f2;}}
_LL22E: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct _tmp5EB;_tmp5EB.tag=
0;_tmp5EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp4C1->loc));{void*_tmp5E9[1]={& _tmp5EB};((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp5EA="%s: new at top-level";
_tag_dyneither(_tmp5EA,sizeof(char),21);}),_tag_dyneither(_tmp5E9,sizeof(void*),
1));}});{void*_tmp5EC=_tmp4C1->r;struct Cyc_List_List*_tmp5EE;struct Cyc_Absyn_Vardecl*
_tmp5F0;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F2;int _tmp5F3;
struct _tuple1*_tmp5F5;struct Cyc_List_List*_tmp5F6;struct Cyc_List_List*_tmp5F7;
struct Cyc_Absyn_Aggrdecl*_tmp5F8;struct Cyc_List_List*_tmp5FA;_LL2D1: {struct Cyc_Absyn_Array_e_struct*
_tmp5ED=(struct Cyc_Absyn_Array_e_struct*)_tmp5EC;if(_tmp5ED->tag != 28)goto _LL2D3;
else{_tmp5EE=_tmp5ED->f1;}}_LL2D2: {struct _tuple1*_tmp5FB=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5FC=Cyc_Absyn_var_exp(_tmp5FB,0);struct Cyc_Absyn_Stmt*
_tmp5FD=Cyc_Toc_init_array(nv,_tmp5FC,_tmp5EE,Cyc_Absyn_exp_stmt(_tmp5FC,0));
void*old_elt_typ;{void*_tmp5FE=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp600;void*_tmp601;struct Cyc_Absyn_Tqual _tmp602;struct Cyc_Absyn_PtrAtts _tmp603;
union Cyc_Absyn_Constraint*_tmp604;_LL2DC: {struct Cyc_Absyn_PointerType_struct*
_tmp5FF=(struct Cyc_Absyn_PointerType_struct*)_tmp5FE;if(_tmp5FF->tag != 5)goto
_LL2DE;else{_tmp600=_tmp5FF->f1;_tmp601=_tmp600.elt_typ;_tmp602=_tmp600.elt_tq;
_tmp603=_tmp600.ptr_atts;_tmp604=_tmp603.zero_term;}}_LL2DD: old_elt_typ=_tmp601;
goto _LL2DB;_LL2DE:;_LL2DF: old_elt_typ=({void*_tmp605=0;((void*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp606="exp_to_c:new array expression doesn't have ptr type";
_tag_dyneither(_tmp606,sizeof(char),52);}),_tag_dyneither(_tmp605,sizeof(void*),
0));});_LL2DB:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp607=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp608=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5EE),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4C0 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp608);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4C0;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp608);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp5FB,_tmp607,(struct Cyc_Absyn_Exp*)e1,_tmp5FD,0));goto
_LL2D0;}}_LL2D3: {struct Cyc_Absyn_Comprehension_e_struct*_tmp5EF=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5EC;if(_tmp5EF->tag != 29)goto _LL2D5;else{_tmp5F0=_tmp5EF->f1;_tmp5F1=_tmp5EF->f2;
_tmp5F2=_tmp5EF->f3;_tmp5F3=_tmp5EF->f4;}}_LL2D4: {int is_dyneither_ptr=0;{void*
_tmp609=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp60B;void*_tmp60C;
struct Cyc_Absyn_Tqual _tmp60D;struct Cyc_Absyn_PtrAtts _tmp60E;union Cyc_Absyn_Constraint*
_tmp60F;union Cyc_Absyn_Constraint*_tmp610;_LL2E1: {struct Cyc_Absyn_PointerType_struct*
_tmp60A=(struct Cyc_Absyn_PointerType_struct*)_tmp609;if(_tmp60A->tag != 5)goto
_LL2E3;else{_tmp60B=_tmp60A->f1;_tmp60C=_tmp60B.elt_typ;_tmp60D=_tmp60B.elt_tq;
_tmp60E=_tmp60B.ptr_atts;_tmp60F=_tmp60E.bounds;_tmp610=_tmp60E.zero_term;}}
_LL2E2: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp60F)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2E0;
_LL2E3:;_LL2E4:({void*_tmp611=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp612="exp_to_c: comprehension not an array type";
_tag_dyneither(_tmp612,sizeof(char),42);}),_tag_dyneither(_tmp611,sizeof(void*),
0));});_LL2E0:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5F2->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp5F1);{struct Cyc_Absyn_Exp*_tmp613=
Cyc_Absyn_var_exp(max,0);if(_tmp5F3)_tmp613=Cyc_Absyn_add_exp(_tmp613,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp5F0,Cyc_Absyn_var_exp(max,0),_tmp5F2,_tmp5F3,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp615;struct _RegionHandle*_tmp616;struct Cyc_Toc_Env*_tmp614=
nv;_tmp615=*_tmp614;_tmp616=_tmp615.rgn;{struct Cyc_List_List*decls=({struct Cyc_List_List*
_tmp627=_region_malloc(_tmp616,sizeof(*_tmp627));_tmp627->hd=({struct _tuple19*
_tmp628=_region_malloc(_tmp616,sizeof(*_tmp628));_tmp628->f1=max;_tmp628->f2=Cyc_Absyn_uint_typ;
_tmp628->f3=(struct Cyc_Absyn_Exp*)_tmp5F1;_tmp628;});_tmp627->tl=0;_tmp627;});
struct Cyc_Absyn_Exp*ai;if(_tmp4C0 == 0  || Cyc_Absyn_no_regions)ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*
_tmp617[2];_tmp617[1]=_tmp613;_tmp617[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp617,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{struct Cyc_Absyn_Exp*r=(
struct Cyc_Absyn_Exp*)_tmp4C0;Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp618[2];_tmp618[1]=_tmp613;
_tmp618[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp618,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({
struct Cyc_List_List*_tmp619=_region_malloc(_tmp616,sizeof(*_tmp619));_tmp619->hd=({
struct _tuple19*_tmp61A=_region_malloc(_tmp616,sizeof(*_tmp61A));_tmp61A->f1=a;
_tmp61A->f2=ptr_typ;_tmp61A->f3=(struct Cyc_Absyn_Exp*)ainit;_tmp61A;});_tmp619->tl=
decls;_tmp619;});if(is_dyneither_ptr){struct _tuple1*_tmp61B=Cyc_Toc_temp_var();
void*_tmp61C=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp61D=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp61E=Cyc_Absyn_fncall_exp(_tmp61D,({struct Cyc_Absyn_Exp*
_tmp621[3];_tmp621[2]=_tmp613;_tmp621[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);
_tmp621[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp621,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
decls=({struct Cyc_List_List*_tmp61F=_region_malloc(_tmp616,sizeof(*_tmp61F));
_tmp61F->hd=({struct _tuple19*_tmp620=_region_malloc(_tmp616,sizeof(*_tmp620));
_tmp620->f1=_tmp61B;_tmp620->f2=_tmp61C;_tmp620->f3=(struct Cyc_Absyn_Exp*)
_tmp61E;_tmp620;});_tmp61F->tl=decls;_tmp61F;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(_tmp61B,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(a,0),0),0);}{struct Cyc_List_List*_tmp622=decls;for(0;_tmp622 != 
0;_tmp622=_tmp622->tl){struct _tuple1*_tmp624;void*_tmp625;struct Cyc_Absyn_Exp*
_tmp626;struct _tuple19 _tmp623=*((struct _tuple19*)_tmp622->hd);_tmp624=_tmp623.f1;
_tmp625=_tmp623.f2;_tmp626=_tmp623.f3;s=Cyc_Absyn_declare_stmt(_tmp624,_tmp625,
_tmp626,s,0);}}e->r=Cyc_Toc_stmt_exp_r(s);}}goto _LL2D0;}}}}_LL2D5: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp5F4=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp5EC;if(_tmp5F4->tag != 30)goto
_LL2D7;else{_tmp5F5=_tmp5F4->f1;_tmp5F6=_tmp5F4->f2;_tmp5F7=_tmp5F4->f3;_tmp5F8=
_tmp5F4->f4;}}_LL2D6: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4C1->topt))->v,_tmp5F6 != 0,1,_tmp4C0,_tmp5F7,_tmp5F5))->r;goto
_LL2D0;_LL2D7: {struct Cyc_Absyn_Tuple_e_struct*_tmp5F9=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp5EC;if(_tmp5F9->tag != 26)goto _LL2D9;else{_tmp5FA=_tmp5F9->f1;}}_LL2D8: e->r=(
Cyc_Toc_init_tuple(nv,1,_tmp4C0,_tmp5FA))->r;goto _LL2D0;_LL2D9:;_LL2DA: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4C1->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp629=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp62A=Cyc_Absyn_var_exp(_tmp629,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp62A,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp4C0 == 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4C0;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp62B=_tmp4C1->r;void*
_tmp62D;struct Cyc_Absyn_Exp*_tmp62E;_LL2E6: {struct Cyc_Absyn_Cast_e_struct*
_tmp62C=(struct Cyc_Absyn_Cast_e_struct*)_tmp62B;if(_tmp62C->tag != 15)goto _LL2E8;
else{_tmp62D=(void*)_tmp62C->f1;_tmp62E=_tmp62C->f2;}}_LL2E7:{struct _tuple0
_tmp630=({struct _tuple0 _tmp62F;_tmp62F.f1=Cyc_Tcutil_compress(_tmp62D);_tmp62F.f2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp62E->topt))->v);
_tmp62F;});void*_tmp631;struct Cyc_Absyn_PtrInfo _tmp633;void*_tmp634;struct Cyc_Absyn_PtrAtts
_tmp635;union Cyc_Absyn_Constraint*_tmp636;void*_tmp637;struct Cyc_Absyn_PtrInfo
_tmp639;struct Cyc_Absyn_PtrAtts _tmp63A;union Cyc_Absyn_Constraint*_tmp63B;_LL2EB:
_tmp631=_tmp630.f1;{struct Cyc_Absyn_PointerType_struct*_tmp632=(struct Cyc_Absyn_PointerType_struct*)
_tmp631;if(_tmp632->tag != 5)goto _LL2ED;else{_tmp633=_tmp632->f1;_tmp634=_tmp633.elt_typ;
_tmp635=_tmp633.ptr_atts;_tmp636=_tmp635.bounds;}}_tmp637=_tmp630.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp638=(struct Cyc_Absyn_PointerType_struct*)_tmp637;if(_tmp638->tag != 5)goto
_LL2ED;else{_tmp639=_tmp638->f1;_tmp63A=_tmp639.ptr_atts;_tmp63B=_tmp63A.bounds;}}
_LL2EC:{struct _tuple0 _tmp63D=({struct _tuple0 _tmp63C;_tmp63C.f1=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp636);
_tmp63C.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp63B);_tmp63C;});void*_tmp63E;void*_tmp640;struct Cyc_Absyn_Exp*
_tmp642;_LL2F0: _tmp63E=_tmp63D.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp63F=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp63E;if(_tmp63F->tag != 0)goto _LL2F2;}
_tmp640=_tmp63D.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp641=(struct Cyc_Absyn_Upper_b_struct*)
_tmp640;if(_tmp641->tag != 1)goto _LL2F2;else{_tmp642=_tmp641->f1;}}_LL2F1: Cyc_Toc_exp_to_c(
nv,_tmp62E);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp643=Cyc_Toc__init_dyneither_ptr_e;e->r=Cyc_Toc_fncall_exp_r(_tmp643,({struct
Cyc_Absyn_Exp*_tmp644[4];_tmp644[3]=_tmp642;_tmp644[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp634),0);_tmp644[1]=_tmp62E;_tmp644[0]=mexp;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp644,sizeof(struct Cyc_Absyn_Exp*),
4));}));goto _LL2EF;}_LL2F2:;_LL2F3: goto _LL2EF;_LL2EF:;}goto _LL2EA;_LL2ED:;_LL2EE:
goto _LL2EA;_LL2EA:;}goto _LL2E5;_LL2E8:;_LL2E9: goto _LL2E5;_LL2E5:;}if(!done){
struct Cyc_Absyn_Stmt*_tmp645=Cyc_Absyn_exp_stmt(_tmp62A,0);struct Cyc_Absyn_Exp*
_tmp646=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4C1);_tmp645=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp62A,_tmp646,0),_tmp4C1,0),
_tmp645,0);{void*_tmp647=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp629,_tmp647,(struct Cyc_Absyn_Exp*)mexp,_tmp645,0));}}
goto _LL2D0;}}_LL2D0:;}goto _LL206;_LL22F: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4C2=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp480;if(_tmp4C2->tag != 19)goto
_LL231;else{_tmp4C3=_tmp4C2->f1;}}_LL230: Cyc_Toc_exp_to_c(nv,_tmp4C3);goto _LL206;
_LL231: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4C4=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp480;if(_tmp4C4->tag != 18)goto _LL233;else{_tmp4C5=(void*)_tmp4C4->f1;}}_LL232:
e->r=(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp648=_cycalloc(sizeof(*
_tmp648));_tmp648[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmp649;_tmp649.tag=18;
_tmp649.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4C5);_tmp649;});_tmp648;});goto
_LL206;_LL233: {struct Cyc_Absyn_Gentyp_e_struct*_tmp4C6=(struct Cyc_Absyn_Gentyp_e_struct*)
_tmp480;if(_tmp4C6->tag != 21)goto _LL235;}_LL234:({void*_tmp64A=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp64B="__gen() in code generator";
_tag_dyneither(_tmp64B,sizeof(char),26);}),_tag_dyneither(_tmp64A,sizeof(void*),
0));});_LL235: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4C7=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp480;if(_tmp4C7->tag != 20)goto _LL237;else{_tmp4C8=(void*)_tmp4C7->f1;_tmp4C9=(
void*)_tmp4C7->f2;{struct Cyc_Absyn_StructField_struct*_tmp4CA=(struct Cyc_Absyn_StructField_struct*)
_tmp4C9;if(_tmp4CA->tag != 0)goto _LL237;else{_tmp4CB=_tmp4CA->f1;}}}}_LL236: e->r=(
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp64C=_cycalloc(sizeof(*_tmp64C));
_tmp64C[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp64D;_tmp64D.tag=20;_tmp64D.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp4C8);_tmp64D.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*
_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E[0]=({struct Cyc_Absyn_StructField_struct
_tmp64F;_tmp64F.tag=0;_tmp64F.f1=_tmp4CB;_tmp64F;});_tmp64E;}));_tmp64D;});
_tmp64C;});goto _LL206;_LL237: {struct Cyc_Absyn_Offsetof_e_struct*_tmp4CC=(struct
Cyc_Absyn_Offsetof_e_struct*)_tmp480;if(_tmp4CC->tag != 20)goto _LL239;else{
_tmp4CD=(void*)_tmp4CC->f1;_tmp4CE=(void*)_tmp4CC->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp4CF=(struct Cyc_Absyn_TupleIndex_struct*)_tmp4CE;if(_tmp4CF->tag != 1)goto
_LL239;else{_tmp4D0=_tmp4CF->f1;}}}}_LL238:{void*_tmp650=Cyc_Tcutil_compress(
_tmp4CD);struct Cyc_Absyn_AggrInfo _tmp652;union Cyc_Absyn_AggrInfoU _tmp653;struct
Cyc_List_List*_tmp655;_LL2F5: {struct Cyc_Absyn_AggrType_struct*_tmp651=(struct
Cyc_Absyn_AggrType_struct*)_tmp650;if(_tmp651->tag != 12)goto _LL2F7;else{_tmp652=
_tmp651->f1;_tmp653=_tmp652.aggr_info;}}_LL2F6: {struct Cyc_Absyn_Aggrdecl*
_tmp658=Cyc_Absyn_get_known_aggrdecl(_tmp653);if(_tmp658->impl == 0)({void*
_tmp659=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp65A="struct fields must be known";_tag_dyneither(_tmp65A,sizeof(
char),28);}),_tag_dyneither(_tmp659,sizeof(void*),0));});_tmp655=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp658->impl))->fields;goto _LL2F8;}_LL2F7: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp654=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp650;if(_tmp654->tag != 13)goto
_LL2F9;else{_tmp655=_tmp654->f2;}}_LL2F8: {struct Cyc_Absyn_Aggrfield*_tmp65B=((
struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp655,(
int)_tmp4D0);e->r=(void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp65C=_cycalloc(
sizeof(*_tmp65C));_tmp65C[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp65D;_tmp65D.tag=
20;_tmp65D.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4CD);_tmp65D.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E[0]=({
struct Cyc_Absyn_StructField_struct _tmp65F;_tmp65F.tag=0;_tmp65F.f1=_tmp65B->name;
_tmp65F;});_tmp65E;}));_tmp65D;});_tmp65C;});goto _LL2F4;}_LL2F9: {struct Cyc_Absyn_TupleType_struct*
_tmp656=(struct Cyc_Absyn_TupleType_struct*)_tmp650;if(_tmp656->tag != 11)goto
_LL2FB;}_LL2FA: e->r=(void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp660=_cycalloc(
sizeof(*_tmp660));_tmp660[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp661;_tmp661.tag=
20;_tmp661.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4CD);_tmp661.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662[0]=({
struct Cyc_Absyn_StructField_struct _tmp663;_tmp663.tag=0;_tmp663.f1=Cyc_Absyn_fieldname((
int)(_tmp4D0 + 1));_tmp663;});_tmp662;}));_tmp661;});_tmp660;});goto _LL2F4;_LL2FB: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp657=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp650;if(_tmp657->tag != 4)goto _LL2FD;}_LL2FC: if(_tmp4D0 == 0)e->r=(void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp665;_tmp665.tag=20;_tmp665.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp4CD);_tmp665.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp666=
_cycalloc(sizeof(*_tmp666));_tmp666[0]=({struct Cyc_Absyn_StructField_struct
_tmp667;_tmp667.tag=0;_tmp667.f1=Cyc_Toc_tag_sp;_tmp667;});_tmp666;}));_tmp665;});
_tmp664;});else{e->r=(void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp668=
_cycalloc(sizeof(*_tmp668));_tmp668[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp669;_tmp669.tag=20;_tmp669.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4CD);_tmp669.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp66A=_cycalloc(sizeof(*
_tmp66A));_tmp66A[0]=({struct Cyc_Absyn_StructField_struct _tmp66B;_tmp66B.tag=0;
_tmp66B.f1=Cyc_Absyn_fieldname((int)_tmp4D0);_tmp66B;});_tmp66A;}));_tmp669;});
_tmp668;});}goto _LL2F4;_LL2FD:;_LL2FE:({void*_tmp66C=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp66D="impossible type for offsetof tuple index";
_tag_dyneither(_tmp66D,sizeof(char),41);}),_tag_dyneither(_tmp66C,sizeof(void*),
0));});_LL2F4:;}goto _LL206;_LL239: {struct Cyc_Absyn_Deref_e_struct*_tmp4D1=(
struct Cyc_Absyn_Deref_e_struct*)_tmp480;if(_tmp4D1->tag != 22)goto _LL23B;else{
_tmp4D2=_tmp4D1->f1;}}_LL23A: {void*_tmp66E=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4D2->topt))->v);{void*_tmp66F=_tmp66E;struct Cyc_Absyn_PtrInfo
_tmp671;void*_tmp672;struct Cyc_Absyn_Tqual _tmp673;struct Cyc_Absyn_PtrAtts _tmp674;
void*_tmp675;union Cyc_Absyn_Constraint*_tmp676;union Cyc_Absyn_Constraint*_tmp677;
union Cyc_Absyn_Constraint*_tmp678;_LL300: {struct Cyc_Absyn_PointerType_struct*
_tmp670=(struct Cyc_Absyn_PointerType_struct*)_tmp66F;if(_tmp670->tag != 5)goto
_LL302;else{_tmp671=_tmp670->f1;_tmp672=_tmp671.elt_typ;_tmp673=_tmp671.elt_tq;
_tmp674=_tmp671.ptr_atts;_tmp675=_tmp674.rgn;_tmp676=_tmp674.nullable;_tmp677=
_tmp674.bounds;_tmp678=_tmp674.zero_term;}}_LL301:{void*_tmp679=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp677);struct Cyc_Absyn_Exp*_tmp67B;_LL305: {struct Cyc_Absyn_Upper_b_struct*
_tmp67A=(struct Cyc_Absyn_Upper_b_struct*)_tmp679;if(_tmp67A->tag != 1)goto _LL307;
else{_tmp67B=_tmp67A->f1;}}_LL306: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4D2);Cyc_Toc_exp_to_c(nv,_tmp4D2);if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({
void*_tmp67D=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp67E="inserted null check due to dereference";
_tag_dyneither(_tmp67E,sizeof(char),39);}),_tag_dyneither(_tmp67D,sizeof(void*),
0));});_tmp4D2->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp66E),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp67F=_cycalloc(sizeof(*_tmp67F));
_tmp67F->hd=Cyc_Absyn_copy_exp(_tmp4D2);_tmp67F->tl=0;_tmp67F;}),0));}if(!((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp678)){unsigned int
_tmp681;int _tmp682;struct _tuple11 _tmp680=Cyc_Evexp_eval_const_uint_exp(_tmp67B);
_tmp681=_tmp680.f1;_tmp682=_tmp680.f2;if(!_tmp682  || _tmp681 <= 0)({void*_tmp683=
0;Cyc_Tcutil_terr(e->loc,({const char*_tmp684="cannot determine dereference is in bounds";
_tag_dyneither(_tmp684,sizeof(char),42);}),_tag_dyneither(_tmp683,sizeof(void*),
0));});}goto _LL304;}_LL307: {struct Cyc_Absyn_DynEither_b_struct*_tmp67C=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp679;if(_tmp67C->tag != 0)goto _LL304;}_LL308: {
struct Cyc_Absyn_Exp*_tmp685=Cyc_Absyn_uint_exp(0,0);_tmp685->topt=({struct Cyc_Core_Opt*
_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->v=Cyc_Absyn_uint_typ;_tmp686;});e->r=
Cyc_Toc_subscript_exp_r(_tmp4D2,_tmp685);Cyc_Toc_exp_to_c(nv,e);goto _LL304;}
_LL304:;}goto _LL2FF;_LL302:;_LL303:({void*_tmp687=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp688="exp_to_c: Deref: non-pointer";
_tag_dyneither(_tmp688,sizeof(char),29);}),_tag_dyneither(_tmp687,sizeof(void*),
0));});_LL2FF:;}goto _LL206;}_LL23B: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4D3=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp480;if(_tmp4D3->tag != 23)goto _LL23D;
else{_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;_tmp4D6=_tmp4D3->f3;_tmp4D7=_tmp4D3->f4;}}
_LL23C: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4D4->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4D4);if(
_tmp4D6  && _tmp4D7)e->r=Cyc_Toc_check_tagged_union(_tmp4D4,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4D5,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL206;}_LL23D: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4D8=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp480;if(_tmp4D8->tag != 24)goto _LL23F;
else{_tmp4D9=_tmp4D8->f1;_tmp4DA=_tmp4D8->f2;_tmp4DB=_tmp4D8->f3;_tmp4DC=_tmp4D8->f4;}}
_LL23E: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4D9->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp4D9);Cyc_Toc_exp_to_c(
nv,_tmp4D9);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp68A;struct Cyc_Absyn_Tqual
_tmp68B;struct Cyc_Absyn_PtrAtts _tmp68C;void*_tmp68D;union Cyc_Absyn_Constraint*
_tmp68E;union Cyc_Absyn_Constraint*_tmp68F;union Cyc_Absyn_Constraint*_tmp690;
struct Cyc_Absyn_PtrInfo _tmp689=Cyc_Toc_get_ptr_type(e1typ);_tmp68A=_tmp689.elt_typ;
_tmp68B=_tmp689.elt_tq;_tmp68C=_tmp689.ptr_atts;_tmp68D=_tmp68C.rgn;_tmp68E=
_tmp68C.nullable;_tmp68F=_tmp68C.bounds;_tmp690=_tmp68C.zero_term;{void*_tmp691=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp68F);struct Cyc_Absyn_Exp*_tmp693;_LL30A: {struct Cyc_Absyn_Upper_b_struct*
_tmp692=(struct Cyc_Absyn_Upper_b_struct*)_tmp691;if(_tmp692->tag != 1)goto _LL30C;
else{_tmp693=_tmp692->f1;}}_LL30B: {unsigned int _tmp696;int _tmp697;struct
_tuple11 _tmp695=Cyc_Evexp_eval_const_uint_exp(_tmp693);_tmp696=_tmp695.f1;
_tmp697=_tmp695.f2;if(_tmp697){if(_tmp696 < 1)({void*_tmp698=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp699="exp_to_c:  AggrArrow_e on pointer of size 0";
_tag_dyneither(_tmp699,sizeof(char),44);}),_tag_dyneither(_tmp698,sizeof(void*),
0));});if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp69A=0;Cyc_Tcutil_warn(
e->loc,({const char*_tmp69B="inserted null check due to dereference";
_tag_dyneither(_tmp69B,sizeof(char),39);}),_tag_dyneither(_tmp69A,sizeof(void*),
0));});_tmp4D9->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_Absyn_Exp*_tmp69C[1];_tmp69C[0]=Cyc_Absyn_new_exp(
_tmp4D9->r,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp69C,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}}else{if(!Cyc_Evexp_c_can_eval(
_tmp693))({void*_tmp69D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp69E="cannot determine pointer dereference in bounds";
_tag_dyneither(_tmp69E,sizeof(char),47);}),_tag_dyneither(_tmp69D,sizeof(void*),
0));});_tmp4D9->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp69F[4];_tmp69F[3]=
Cyc_Absyn_uint_exp(0,0);_tmp69F[2]=Cyc_Absyn_sizeoftyp_exp(_tmp68A,0);_tmp69F[1]=
_tmp693;_tmp69F[0]=Cyc_Absyn_new_exp(_tmp4D9->r,0);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp69F,sizeof(struct Cyc_Absyn_Exp*),
4));}),0));}goto _LL309;}_LL30C: {struct Cyc_Absyn_DynEither_b_struct*_tmp694=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp691;if(_tmp694->tag != 0)goto _LL309;}
_LL30D: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp68A);_tmp4D9->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_cstar_typ(ta1,_tmp68B),Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,({
struct Cyc_Absyn_Exp*_tmp6A0[3];_tmp6A0[2]=Cyc_Absyn_uint_exp(0,0);_tmp6A0[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp6A0[0]=Cyc_Absyn_new_exp(_tmp4D9->r,0);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6A0,sizeof(struct Cyc_Absyn_Exp*),
3));}),0));goto _LL309;}_LL309:;}if(_tmp4DB  && _tmp4DC)e->r=Cyc_Toc_check_tagged_union(
_tmp4D9,Cyc_Toc_typ_to_c(e1typ),_tmp68A,_tmp4DA,Cyc_Absyn_aggrarrow_exp);if(
is_poly  && _tmp4DC)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL206;}}_LL23F: {struct Cyc_Absyn_Subscript_e_struct*_tmp4DD=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp480;if(_tmp4DD->tag != 25)goto _LL241;else{_tmp4DE=_tmp4DD->f1;_tmp4DF=_tmp4DD->f2;}}
_LL240: {void*_tmp6A1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4DE->topt))->v);{void*_tmp6A2=_tmp6A1;struct Cyc_List_List*
_tmp6A4;struct Cyc_Absyn_PtrInfo _tmp6A6;void*_tmp6A7;struct Cyc_Absyn_Tqual _tmp6A8;
struct Cyc_Absyn_PtrAtts _tmp6A9;void*_tmp6AA;union Cyc_Absyn_Constraint*_tmp6AB;
union Cyc_Absyn_Constraint*_tmp6AC;union Cyc_Absyn_Constraint*_tmp6AD;_LL30F: {
struct Cyc_Absyn_TupleType_struct*_tmp6A3=(struct Cyc_Absyn_TupleType_struct*)
_tmp6A2;if(_tmp6A3->tag != 11)goto _LL311;else{_tmp6A4=_tmp6A3->f1;}}_LL310: Cyc_Toc_exp_to_c(
nv,_tmp4DE);Cyc_Toc_exp_to_c(nv,_tmp4DF);{unsigned int _tmp6AF;int _tmp6B0;struct
_tuple11 _tmp6AE=Cyc_Evexp_eval_const_uint_exp(_tmp4DF);_tmp6AF=_tmp6AE.f1;
_tmp6B0=_tmp6AE.f2;if(!_tmp6B0)({void*_tmp6B1=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp6B2="unknown tuple subscript in translation to C";
_tag_dyneither(_tmp6B2,sizeof(char),44);}),_tag_dyneither(_tmp6B1,sizeof(void*),
0));});e->r=Cyc_Toc_aggrmember_exp_r(_tmp4DE,Cyc_Absyn_fieldname((int)(_tmp6AF + 
1)));goto _LL30E;}_LL311: {struct Cyc_Absyn_PointerType_struct*_tmp6A5=(struct Cyc_Absyn_PointerType_struct*)
_tmp6A2;if(_tmp6A5->tag != 5)goto _LL313;else{_tmp6A6=_tmp6A5->f1;_tmp6A7=_tmp6A6.elt_typ;
_tmp6A8=_tmp6A6.elt_tq;_tmp6A9=_tmp6A6.ptr_atts;_tmp6AA=_tmp6A9.rgn;_tmp6AB=
_tmp6A9.nullable;_tmp6AC=_tmp6A9.bounds;_tmp6AD=_tmp6A9.zero_term;}}_LL312: {
struct Cyc_List_List*_tmp6B3=Cyc_Toc_get_relns(_tmp4DE);int in_bnds=0;{void*
_tmp6B4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6AC);_LL316:;_LL317: in_bnds=Cyc_Toc_check_bounds(_tmp6B3,_tmp4DE,_tmp4DF);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp6B7;
_tmp6B7.tag=0;_tmp6B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp6B5[1]={& _tmp6B7};Cyc_Tcutil_warn(e->loc,({const char*_tmp6B6="bounds check necessary for %s";
_tag_dyneither(_tmp6B6,sizeof(char),30);}),_tag_dyneither(_tmp6B5,sizeof(void*),
1));}});_LL315:;}Cyc_Toc_exp_to_c(nv,_tmp4DE);Cyc_Toc_exp_to_c(nv,_tmp4DF);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6B8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6AC);struct Cyc_Absyn_Exp*_tmp6BA;_LL319: {struct Cyc_Absyn_Upper_b_struct*
_tmp6B9=(struct Cyc_Absyn_Upper_b_struct*)_tmp6B8;if(_tmp6B9->tag != 1)goto _LL31B;
else{_tmp6BA=_tmp6B9->f1;}}_LL31A: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6AB);void*ta1=Cyc_Toc_typ_to_c(_tmp6A7);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6A8);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6AD)){if(!Cyc_Evexp_c_can_eval(_tmp6BA))({void*_tmp6BC=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp6BD="cannot determine subscript is in bounds";_tag_dyneither(
_tmp6BD,sizeof(char),40);}),_tag_dyneither(_tmp6BC,sizeof(void*),0));});{struct
Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4DE);e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
function_e,({struct Cyc_Absyn_Exp*_tmp6BE[3];_tmp6BE[2]=_tmp4DF;_tmp6BE[1]=
_tmp6BA;_tmp6BE[0]=_tmp4DE;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp6BE,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6BA))({void*_tmp6BF=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp6C0="cannot determine subscript is in bounds";
_tag_dyneither(_tmp6C0,sizeof(char),40);}),_tag_dyneither(_tmp6BF,sizeof(void*),
0));});if(Cyc_Toc_warn_all_null_deref)({void*_tmp6C1=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp6C2="inserted null check due to dereference";_tag_dyneither(
_tmp6C2,sizeof(char),39);}),_tag_dyneither(_tmp6C1,sizeof(void*),0));});e->r=Cyc_Toc_deref_exp_r(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp6C3[4];_tmp6C3[3]=_tmp4DF;_tmp6C3[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp6C3[1]=_tmp6BA;_tmp6C3[0]=_tmp4DE;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6C3,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)));}else{_tmp4DF->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp6C4[2];_tmp6C4[1]=Cyc_Absyn_copy_exp(_tmp4DF);_tmp6C4[0]=
_tmp6BA;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp6C4,sizeof(struct Cyc_Absyn_Exp*),2));}));}}}goto _LL318;}
_LL31B: {struct Cyc_Absyn_DynEither_b_struct*_tmp6BB=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6B8;if(_tmp6BB->tag != 0)goto _LL318;}_LL31C: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp6A7);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;e->r=Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A8),Cyc_Absyn_aggrmember_exp(
_tmp4DE,Cyc_Toc_curr_sp,0)),_tmp4DF);}else{struct Cyc_Absyn_Exp*_tmp6C5=Cyc_Toc__check_dyneither_subscript_e;
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A8),Cyc_Absyn_fncall_exp(
_tmp6C5,({struct Cyc_Absyn_Exp*_tmp6C6[3];_tmp6C6[2]=_tmp4DF;_tmp6C6[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp6C6[0]=_tmp4DE;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp6C6,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}goto _LL318;}
_LL318:;}goto _LL30E;}_LL313:;_LL314:({void*_tmp6C7=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6C8="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dyneither(_tmp6C8,sizeof(char),49);}),_tag_dyneither(_tmp6C7,sizeof(void*),
0));});_LL30E:;}goto _LL206;}_LL241: {struct Cyc_Absyn_Tuple_e_struct*_tmp4E0=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp480;if(_tmp4E0->tag != 26)goto _LL243;else{
_tmp4E1=_tmp4E0->f1;}}_LL242: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(
nv,0,0,_tmp4E1))->r;else{struct Cyc_List_List*_tmp6C9=((struct Cyc_List_List*(*)(
struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp4E1);void*_tmp6CA=Cyc_Toc_add_tuple_type(_tmp6C9);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp4E1 != 0;(_tmp4E1=_tmp4E1->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp4E1->hd);dles=({struct Cyc_List_List*_tmp6CB=_cycalloc(
sizeof(*_tmp6CB));_tmp6CB->hd=({struct _tuple16*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));
_tmp6CC->f1=0;_tmp6CC->f2=(struct Cyc_Absyn_Exp*)_tmp4E1->hd;_tmp6CC;});_tmp6CB->tl=
dles;_tmp6CB;});}}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles);e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL206;_LL243: {struct Cyc_Absyn_Array_e_struct*
_tmp4E2=(struct Cyc_Absyn_Array_e_struct*)_tmp480;if(_tmp4E2->tag != 28)goto _LL245;
else{_tmp4E3=_tmp4E2->f1;}}_LL244: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4E3);{
struct Cyc_List_List*_tmp6CD=_tmp4E3;for(0;_tmp6CD != 0;_tmp6CD=_tmp6CD->tl){
struct _tuple16 _tmp6CF;struct Cyc_Absyn_Exp*_tmp6D0;struct _tuple16*_tmp6CE=(struct
_tuple16*)_tmp6CD->hd;_tmp6CF=*_tmp6CE;_tmp6D0=_tmp6CF.f2;Cyc_Toc_exp_to_c(nv,
_tmp6D0);}}goto _LL206;_LL245: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4E4=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp480;if(_tmp4E4->tag != 29)goto _LL247;
else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E4->f2;_tmp4E7=_tmp4E4->f3;_tmp4E8=_tmp4E4->f4;}}
_LL246: {unsigned int _tmp6D2;int _tmp6D3;struct _tuple11 _tmp6D1=Cyc_Evexp_eval_const_uint_exp(
_tmp4E6);_tmp6D2=_tmp6D1.f1;_tmp6D3=_tmp6D1.f2;{void*_tmp6D4=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E7->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4E7);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4E7)){if(!_tmp6D3)({
void*_tmp6D5=0;Cyc_Tcutil_terr(_tmp4E6->loc,({const char*_tmp6D6="cannot determine value of constant";
_tag_dyneither(_tmp6D6,sizeof(char),35);}),_tag_dyneither(_tmp6D5,sizeof(void*),
0));});{unsigned int i=0;for(0;i < _tmp6D2;++ i){es=({struct Cyc_List_List*_tmp6D7=
_cycalloc(sizeof(*_tmp6D7));_tmp6D7->hd=({struct _tuple16*_tmp6D8=_cycalloc(
sizeof(*_tmp6D8));_tmp6D8->f1=0;_tmp6D8->f2=_tmp4E7;_tmp6D8;});_tmp6D7->tl=es;
_tmp6D7;});}}if(_tmp4E8){struct Cyc_Absyn_Exp*_tmp6D9=Cyc_Toc_cast_it(_tmp6D4,Cyc_Absyn_uint_exp(
0,0));es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));
_tmp6DA->hd=({struct _tuple16*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));_tmp6DB->f1=0;
_tmp6DB->f2=_tmp6D9;_tmp6DB;});_tmp6DA->tl=0;_tmp6DA;}));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL206;}}}_LL247: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4E9=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp480;if(_tmp4E9->tag != 30)goto _LL249;else{
_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4E9->f2;_tmp4EC=_tmp4E9->f3;_tmp4ED=_tmp4E9->f4;}}
_LL248: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp4EB
!= 0,0,0,_tmp4EC,_tmp4EA))->r;else{if(_tmp4ED == 0)({void*_tmp6DC=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp6DD="Aggregate_e: missing aggrdecl pointer";_tag_dyneither(_tmp6DD,sizeof(
char),38);}),_tag_dyneither(_tmp6DC,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp4ED;struct Cyc_Toc_Env _tmp6DF;struct
_RegionHandle*_tmp6E0;struct Cyc_Toc_Env*_tmp6DE=nv;_tmp6DF=*_tmp6DE;_tmp6E0=
_tmp6DF.rgn;{struct Cyc_List_List*_tmp6E1=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6E0,e->loc,_tmp4EC,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4ED->impl))->tagged){struct
_tuple20 _tmp6E3;struct Cyc_Absyn_Aggrfield*_tmp6E4;struct Cyc_Absyn_Exp*_tmp6E5;
struct _tuple20*_tmp6E2=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6E1))->hd;_tmp6E3=*_tmp6E2;_tmp6E4=_tmp6E3.f1;_tmp6E5=_tmp6E3.f2;{void*
_tmp6E6=_tmp6E4->type;Cyc_Toc_exp_to_c(nv,_tmp6E5);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6E6))_tmp6E5->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6E5->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp4ED,_tmp6E4->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct Cyc_List_List*_tmp6E7=({struct _tuple16*_tmp6ED[2];_tmp6ED[1]=({struct
_tuple16*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->f1=0;_tmp6EF->f2=_tmp6E5;
_tmp6EF;});_tmp6ED[0]=({struct _tuple16*_tmp6EE=_cycalloc(sizeof(*_tmp6EE));
_tmp6EE->f1=0;_tmp6EE->f2=field_tag_exp;_tmp6EE;});((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6ED,sizeof(struct
_tuple16*),2));});struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp6E7,
0);struct Cyc_List_List*ds=({void*_tmp6EA[1];_tmp6EA[0]=(void*)({struct Cyc_Absyn_FieldName_struct*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB[0]=({struct Cyc_Absyn_FieldName_struct
_tmp6EC;_tmp6EC.tag=1;_tmp6EC.f1=_tmp6E4->name;_tmp6EC;});_tmp6EB;});((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6EA,sizeof(void*),1));});
struct Cyc_List_List*dles=({struct _tuple16*_tmp6E8[1];_tmp6E8[0]=({struct _tuple16*
_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9->f1=ds;_tmp6E9->f2=umem;_tmp6E9;});((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp6E8,sizeof(struct _tuple16*),1));});e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}}}
else{struct Cyc_List_List*_tmp6F0=0;struct Cyc_List_List*_tmp6F1=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp6F1 != 0;_tmp6F1=_tmp6F1->tl){struct Cyc_List_List*
_tmp6F2=_tmp6E1;for(0;_tmp6F2 != 0;_tmp6F2=_tmp6F2->tl){if((*((struct _tuple20*)
_tmp6F2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6F1->hd){struct _tuple20
_tmp6F4;struct Cyc_Absyn_Aggrfield*_tmp6F5;struct Cyc_Absyn_Exp*_tmp6F6;struct
_tuple20*_tmp6F3=(struct _tuple20*)_tmp6F2->hd;_tmp6F4=*_tmp6F3;_tmp6F5=_tmp6F4.f1;
_tmp6F6=_tmp6F4.f2;{void*_tmp6F7=_tmp6F5->type;Cyc_Toc_exp_to_c(nv,_tmp6F6);if(
Cyc_Toc_is_void_star_or_tvar(_tmp6F7))_tmp6F6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp6F6->r,0));_tmp6F0=({struct Cyc_List_List*_tmp6F8=_cycalloc(
sizeof(*_tmp6F8));_tmp6F8->hd=({struct _tuple16*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));
_tmp6F9->f1=0;_tmp6F9->f2=_tmp6F6;_tmp6F9;});_tmp6F8->tl=_tmp6F0;_tmp6F8;});
break;}}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp6F0));}}}}goto _LL206;_LL249: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp4EE=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp480;if(_tmp4EE->tag != 31)goto
_LL24B;else{_tmp4EF=(void*)_tmp4EE->f1;_tmp4F0=_tmp4EE->f2;}}_LL24A: {struct Cyc_List_List*
fs;{void*_tmp6FA=Cyc_Tcutil_compress(_tmp4EF);struct Cyc_List_List*_tmp6FC;_LL31E: {
struct Cyc_Absyn_AnonAggrType_struct*_tmp6FB=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6FA;if(_tmp6FB->tag != 13)goto _LL320;else{_tmp6FC=_tmp6FB->f2;}}_LL31F: fs=
_tmp6FC;goto _LL31D;_LL320:;_LL321:({struct Cyc_String_pa_struct _tmp6FF;_tmp6FF.tag=
0;_tmp6FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp4EF));{void*_tmp6FD[1]={& _tmp6FF};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp6FE="anon struct has type %s";
_tag_dyneither(_tmp6FE,sizeof(char),24);}),_tag_dyneither(_tmp6FD,sizeof(void*),
1));}});_LL31D:;}{struct Cyc_Toc_Env _tmp701;struct _RegionHandle*_tmp702;struct Cyc_Toc_Env*
_tmp700=nv;_tmp701=*_tmp700;_tmp702=_tmp701.rgn;{struct Cyc_List_List*_tmp703=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp702,e->loc,_tmp4F0,Cyc_Absyn_StructA,fs);for(0;_tmp703 != 0;_tmp703=_tmp703->tl){
struct _tuple20 _tmp705;struct Cyc_Absyn_Aggrfield*_tmp706;struct Cyc_Absyn_Exp*
_tmp707;struct _tuple20*_tmp704=(struct _tuple20*)_tmp703->hd;_tmp705=*_tmp704;
_tmp706=_tmp705.f1;_tmp707=_tmp705.f2;{void*_tmp708=_tmp706->type;Cyc_Toc_exp_to_c(
nv,_tmp707);if(Cyc_Toc_is_void_star_or_tvar(_tmp708))_tmp707->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp707->r,0));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp4F0);}goto _LL206;}}_LL24B: {struct Cyc_Absyn_Datatype_e_struct*_tmp4F1=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp480;if(_tmp4F1->tag != 32)goto _LL24D;else{
_tmp4F2=_tmp4F1->f1;_tmp4F3=_tmp4F1->f2;_tmp4F4=_tmp4F1->f3;}}_LL24C: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp709=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp70A=Cyc_Absyn_var_exp(_tmp709,0);struct Cyc_Absyn_Exp*
member_exp;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp4F4->name,({
const char*_tmp70B="_struct";_tag_dyneither(_tmp70B,sizeof(char),8);})));tag_exp=
_tmp4F3->is_extensible?Cyc_Absyn_var_exp(_tmp4F4->name,0): Cyc_Toc_datatype_tag(
_tmp4F3,_tmp4F4->name);member_exp=_tmp70A;{struct Cyc_List_List*_tmp70C=_tmp4F4->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp4F2 != 0;(
_tmp4F2=_tmp4F2->tl,_tmp70C=_tmp70C->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp4F2->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp70C))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);dles=({struct Cyc_List_List*
_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D->hd=({struct _tuple16*_tmp70E=
_cycalloc(sizeof(*_tmp70E));_tmp70E->f1=0;_tmp70E->f2=cur_e;_tmp70E;});_tmp70D->tl=
dles;_tmp70D;});}dles=({struct Cyc_List_List*_tmp70F=_cycalloc(sizeof(*_tmp70F));
_tmp70F->hd=({struct _tuple16*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710->f1=0;
_tmp710->f2=tag_exp;_tmp710;});_tmp70F->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp70F;});e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
else{struct Cyc_List_List*_tmp711=({struct Cyc_List_List*_tmp717=_cycalloc(sizeof(*
_tmp717));_tmp717->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,
Cyc_Toc_tag_sp,0),tag_exp,0);_tmp717->tl=0;_tmp717;});{int i=1;for(0;_tmp4F2 != 0;(((
_tmp4F2=_tmp4F2->tl,i ++)),_tmp70C=_tmp70C->tl)){struct Cyc_Absyn_Exp*cur_e=(
struct Cyc_Absyn_Exp*)_tmp4F2->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct
_tuple10*)((struct Cyc_List_List*)_check_null(_tmp70C))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(field_typ))cur_e=Cyc_Toc_cast_it(
field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp712=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);_tmp711=({struct Cyc_List_List*
_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713->hd=_tmp712;_tmp713->tl=_tmp711;
_tmp713;});}}}{struct Cyc_Absyn_Stmt*_tmp714=Cyc_Absyn_exp_stmt(_tmp70A,0);struct
Cyc_Absyn_Stmt*_tmp715=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp716=_cycalloc(sizeof(*_tmp716));
_tmp716->hd=_tmp714;_tmp716->tl=_tmp711;_tmp716;})),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp709,datatype_ctype,0,_tmp715,0));}}goto _LL206;}}
_LL24D: {struct Cyc_Absyn_Enum_e_struct*_tmp4F5=(struct Cyc_Absyn_Enum_e_struct*)
_tmp480;if(_tmp4F5->tag != 33)goto _LL24F;}_LL24E: goto _LL250;_LL24F: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp4F6=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp480;if(_tmp4F6->tag != 34)goto
_LL251;}_LL250: goto _LL206;_LL251: {struct Cyc_Absyn_Malloc_e_struct*_tmp4F7=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp480;if(_tmp4F7->tag != 35)goto _LL253;else{
_tmp4F8=_tmp4F7->f1;_tmp4F9=_tmp4F8.is_calloc;_tmp4FA=_tmp4F8.rgn;_tmp4FB=
_tmp4F8.elt_type;_tmp4FC=_tmp4F8.num_elts;_tmp4FD=_tmp4F8.fat_result;}}_LL252: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4FB)));Cyc_Toc_exp_to_c(nv,
_tmp4FC);if(_tmp4FD){struct _tuple1*_tmp718=Cyc_Toc_temp_var();struct _tuple1*
_tmp719=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp4F9){xexp=_tmp4FC;if(_tmp4FA != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FA;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp718,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp4FB,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp718,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp71A[3];_tmp71A[2]=Cyc_Absyn_var_exp(_tmp718,0);_tmp71A[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp71A[0]=Cyc_Absyn_var_exp(_tmp719,0);((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp71A,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp4FC,0);if(_tmp4FA != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp4FA;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(
rgn,Cyc_Absyn_var_exp(_tmp718,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp4FB,Cyc_Absyn_var_exp(
_tmp718,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp71B[3];_tmp71B[2]=Cyc_Absyn_var_exp(_tmp718,0);_tmp71B[1]=Cyc_Absyn_uint_exp(
1,0);_tmp71B[0]=Cyc_Absyn_var_exp(_tmp719,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp71B,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp71C=Cyc_Absyn_declare_stmt(_tmp718,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp719,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp71C);}}else{struct Cyc_Absyn_Exp*_tmp71D=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp71E=_tmp4FC->r;union Cyc_Absyn_Cnst _tmp720;struct _tuple6 _tmp721;
int _tmp722;_LL323: {struct Cyc_Absyn_Const_e_struct*_tmp71F=(struct Cyc_Absyn_Const_e_struct*)
_tmp71E;if(_tmp71F->tag != 0)goto _LL325;else{_tmp720=_tmp71F->f1;if((_tmp720.Int_c).tag
!= 4)goto _LL325;_tmp721=(struct _tuple6)(_tmp720.Int_c).val;_tmp722=_tmp721.f2;
if(_tmp722 != 1)goto _LL325;}}_LL324: goto _LL322;_LL325:;_LL326: _tmp71D=Cyc_Absyn_times_exp(
_tmp71D,_tmp4FC,0);goto _LL322;_LL322:;}if(_tmp4FA != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FA;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp71D))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp4FB,
_tmp71D))->r;}}goto _LL206;}_LL253: {struct Cyc_Absyn_Swap_e_struct*_tmp4FE=(
struct Cyc_Absyn_Swap_e_struct*)_tmp480;if(_tmp4FE->tag != 36)goto _LL255;else{
_tmp4FF=_tmp4FE->f1;_tmp500=_tmp4FE->f2;}}_LL254: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4FF->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp500->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr))({void*_tmp723=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp724="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";
_tag_dyneither(_tmp724,sizeof(char),57);}),_tag_dyneither(_tmp723,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp4FF);Cyc_Toc_exp_to_c(
nv,_tmp500);e->r=Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*_tmp725[2];
_tmp725[1]=Cyc_Toc_push_address_exp(_tmp500);_tmp725[0]=Cyc_Toc_push_address_exp(
_tmp4FF);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp725,sizeof(struct Cyc_Absyn_Exp*),2));}));goto _LL206;}}_LL255: {
struct Cyc_Absyn_Tagcheck_e_struct*_tmp501=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp480;if(_tmp501->tag != 39)goto _LL257;else{_tmp502=_tmp501->f1;_tmp503=_tmp501->f2;}}
_LL256: {void*_tmp726=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp502->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp502);{void*_tmp727=
_tmp726;struct Cyc_Absyn_AggrInfo _tmp729;union Cyc_Absyn_AggrInfoU _tmp72A;struct
Cyc_Absyn_Aggrdecl**_tmp72B;struct Cyc_Absyn_Aggrdecl*_tmp72C;_LL328: {struct Cyc_Absyn_AggrType_struct*
_tmp728=(struct Cyc_Absyn_AggrType_struct*)_tmp727;if(_tmp728->tag != 12)goto
_LL32A;else{_tmp729=_tmp728->f1;_tmp72A=_tmp729.aggr_info;if((_tmp72A.KnownAggr).tag
!= 2)goto _LL32A;_tmp72B=(struct Cyc_Absyn_Aggrdecl**)(_tmp72A.KnownAggr).val;
_tmp72C=*_tmp72B;}}_LL329: {struct Cyc_Absyn_Exp*_tmp72D=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp72C,_tmp503),0);struct Cyc_Absyn_Exp*_tmp72E=Cyc_Absyn_aggrmember_exp(
_tmp502,_tmp503,0);struct Cyc_Absyn_Exp*_tmp72F=Cyc_Absyn_aggrmember_exp(_tmp72E,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp72F,_tmp72D,0))->r;goto _LL327;}
_LL32A:;_LL32B:({void*_tmp730=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp731="non-aggregate type in tagcheck";
_tag_dyneither(_tmp731,sizeof(char),31);}),_tag_dyneither(_tmp730,sizeof(void*),
0));});_LL327:;}goto _LL206;}_LL257: {struct Cyc_Absyn_StmtExp_e_struct*_tmp504=(
struct Cyc_Absyn_StmtExp_e_struct*)_tmp480;if(_tmp504->tag != 38)goto _LL259;else{
_tmp505=_tmp504->f1;}}_LL258: Cyc_Toc_stmt_to_c(nv,_tmp505);goto _LL206;_LL259: {
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp506=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp480;if(_tmp506->tag != 37)goto _LL25B;}_LL25A:({void*_tmp732=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp733="UnresolvedMem";
_tag_dyneither(_tmp733,sizeof(char),14);}),_tag_dyneither(_tmp732,sizeof(void*),
0));});_LL25B: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp507=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp480;if(_tmp507->tag != 27)goto _LL25D;}_LL25C:({void*_tmp734=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp735="compoundlit";
_tag_dyneither(_tmp735,sizeof(char),12);}),_tag_dyneither(_tmp734,sizeof(void*),
0));});_LL25D: {struct Cyc_Absyn_Valueof_e_struct*_tmp508=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp480;if(_tmp508->tag != 40)goto _LL206;}_LL25E:({void*_tmp736=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp737="valueof(-)";
_tag_dyneither(_tmp737,sizeof(char),11);}),_tag_dyneither(_tmp736,sizeof(void*),
0));});_LL206:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){return Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0);}struct _tuple21{
struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple22{struct _tuple1*f1;void*f2;};struct _tuple23{struct Cyc_List_List*f1;struct
Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,
struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp738=p->r;struct Cyc_Absyn_Vardecl*_tmp73A;struct
Cyc_Absyn_Vardecl _tmp73B;struct _tuple1*_tmp73C;struct Cyc_Absyn_Pat*_tmp73D;
struct Cyc_Absyn_Vardecl*_tmp73F;struct Cyc_Absyn_Vardecl _tmp740;struct _tuple1*
_tmp741;struct Cyc_Absyn_Vardecl*_tmp744;struct Cyc_Absyn_Pat*_tmp745;enum Cyc_Absyn_Sign
_tmp748;int _tmp749;char _tmp74B;struct _dyneither_ptr _tmp74D;struct Cyc_Absyn_Enumdecl*
_tmp74F;struct Cyc_Absyn_Enumfield*_tmp750;void*_tmp752;struct Cyc_Absyn_Enumfield*
_tmp753;struct Cyc_Absyn_Pat*_tmp755;struct Cyc_Absyn_Pat _tmp756;void*_tmp757;
struct Cyc_Absyn_Datatypedecl*_tmp759;struct Cyc_Absyn_Datatypefield*_tmp75A;
struct Cyc_List_List*_tmp75B;struct Cyc_List_List*_tmp75D;struct Cyc_List_List*
_tmp75F;struct Cyc_Absyn_AggrInfo*_tmp761;struct Cyc_Absyn_AggrInfo*_tmp763;struct
Cyc_Absyn_AggrInfo _tmp764;union Cyc_Absyn_AggrInfoU _tmp765;struct Cyc_List_List*
_tmp766;struct Cyc_Absyn_Pat*_tmp768;_LL32D: {struct Cyc_Absyn_Var_p_struct*
_tmp739=(struct Cyc_Absyn_Var_p_struct*)_tmp738;if(_tmp739->tag != 1)goto _LL32F;
else{_tmp73A=_tmp739->f1;_tmp73B=*_tmp73A;_tmp73C=_tmp73B.name;_tmp73D=_tmp739->f2;}}
_LL32E: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp73C,r),rgn,t,r,path,
_tmp73D,fail_stmt,decls);_LL32F: {struct Cyc_Absyn_TagInt_p_struct*_tmp73E=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp738;if(_tmp73E->tag != 3)goto _LL331;else{
_tmp73F=_tmp73E->f2;_tmp740=*_tmp73F;_tmp741=_tmp740.name;}}_LL330: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp741,r);goto _LL332;_LL331: {struct Cyc_Absyn_Wild_p_struct*_tmp742=(
struct Cyc_Absyn_Wild_p_struct*)_tmp738;if(_tmp742->tag != 0)goto _LL333;}_LL332: s=
Cyc_Toc_skip_stmt_dl();goto _LL32C;_LL333: {struct Cyc_Absyn_Reference_p_struct*
_tmp743=(struct Cyc_Absyn_Reference_p_struct*)_tmp738;if(_tmp743->tag != 2)goto
_LL335;else{_tmp744=_tmp743->f1;_tmp745=_tmp743->f2;}}_LL334: {struct _tuple1*
_tmp76C=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*_tmp76D=_region_malloc(
rgn,sizeof(*_tmp76D));_tmp76D->hd=({struct _tuple22*_tmp76E=_region_malloc(rgn,
sizeof(*_tmp76E));_tmp76E->f1=_tmp76C;_tmp76E->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq);_tmp76E;});_tmp76D->tl=decls;_tmp76D;});nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp744->name,Cyc_Absyn_var_exp(_tmp76C,0));s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp76C,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp76F=Cyc_Toc_xlate_pat(nv,
rgn,t,r,path,_tmp745,fail_stmt,decls);_tmp76F.f3=Cyc_Absyn_seq_stmt(s,_tmp76F.f3,
0);return _tmp76F;}}_LL335: {struct Cyc_Absyn_Null_p_struct*_tmp746=(struct Cyc_Absyn_Null_p_struct*)
_tmp738;if(_tmp746->tag != 8)goto _LL337;}_LL336: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL32C;_LL337: {struct Cyc_Absyn_Int_p_struct*_tmp747=(struct
Cyc_Absyn_Int_p_struct*)_tmp738;if(_tmp747->tag != 9)goto _LL339;else{_tmp748=
_tmp747->f1;_tmp749=_tmp747->f2;}}_LL338: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp748,_tmp749,0),fail_stmt);goto _LL32C;_LL339: {struct Cyc_Absyn_Char_p_struct*
_tmp74A=(struct Cyc_Absyn_Char_p_struct*)_tmp738;if(_tmp74A->tag != 10)goto _LL33B;
else{_tmp74B=_tmp74A->f1;}}_LL33A: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp74B,0),fail_stmt);goto _LL32C;_LL33B: {struct Cyc_Absyn_Float_p_struct*_tmp74C=(
struct Cyc_Absyn_Float_p_struct*)_tmp738;if(_tmp74C->tag != 11)goto _LL33D;else{
_tmp74D=_tmp74C->f1;}}_LL33C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp74D,
0),fail_stmt);goto _LL32C;_LL33D: {struct Cyc_Absyn_Enum_p_struct*_tmp74E=(struct
Cyc_Absyn_Enum_p_struct*)_tmp738;if(_tmp74E->tag != 12)goto _LL33F;else{_tmp74F=
_tmp74E->f1;_tmp750=_tmp74E->f2;}}_LL33E: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Enum_e_struct*_tmp770=_cycalloc(sizeof(*_tmp770));
_tmp770[0]=({struct Cyc_Absyn_Enum_e_struct _tmp771;_tmp771.tag=33;_tmp771.f1=
_tmp750->name;_tmp771.f2=(struct Cyc_Absyn_Enumdecl*)_tmp74F;_tmp771.f3=(struct
Cyc_Absyn_Enumfield*)_tmp750;_tmp771;});_tmp770;}),0),fail_stmt);goto _LL32C;
_LL33F: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp751=(struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp738;if(_tmp751->tag != 13)goto _LL341;else{_tmp752=(void*)_tmp751->f1;_tmp753=
_tmp751->f2;}}_LL340: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_AnonEnum_e_struct*_tmp772=_cycalloc(sizeof(*_tmp772));_tmp772[0]=({
struct Cyc_Absyn_AnonEnum_e_struct _tmp773;_tmp773.tag=34;_tmp773.f1=_tmp753->name;
_tmp773.f2=(void*)_tmp752;_tmp773.f3=(struct Cyc_Absyn_Enumfield*)_tmp753;_tmp773;});
_tmp772;}),0),fail_stmt);goto _LL32C;_LL341: {struct Cyc_Absyn_Pointer_p_struct*
_tmp754=(struct Cyc_Absyn_Pointer_p_struct*)_tmp738;if(_tmp754->tag != 5)goto
_LL343;else{_tmp755=_tmp754->f1;_tmp756=*_tmp755;_tmp757=_tmp756.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp758=(struct Cyc_Absyn_Datatype_p_struct*)_tmp757;if(_tmp758->tag != 7)goto
_LL343;else{_tmp759=_tmp758->f1;_tmp75A=_tmp758->f2;_tmp75B=_tmp758->f3;}}}}
_LL342: s=Cyc_Toc_skip_stmt_dl();{struct _tuple1*_tmp774=Cyc_Toc_temp_var();struct
_tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp75A->name,({const char*_tmp782="_struct";
_tag_dyneither(_tmp782,sizeof(char),8);}));void*_tmp775=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(
tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp775,r);struct Cyc_List_List*_tmp776=_tmp75A->typs;for(0;_tmp75B != 0;(((
_tmp75B=_tmp75B->tl,_tmp776=((struct Cyc_List_List*)_check_null(_tmp776))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp777=(struct Cyc_Absyn_Pat*)_tmp75B->hd;if(_tmp777->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp778=(*((struct _tuple10*)((
struct Cyc_List_List*)_check_null(_tmp776))->hd)).f2;struct _tuple1*_tmp779=Cyc_Toc_temp_var();
void*_tmp77A=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp777->topt))->v;void*
_tmp77B=Cyc_Toc_typ_to_c(_tmp77A);struct Cyc_Absyn_Exp*_tmp77C=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp774,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp778)))_tmp77C=Cyc_Toc_cast_it(_tmp77B,_tmp77C);decls=({
struct Cyc_List_List*_tmp77D=_region_malloc(rgn,sizeof(*_tmp77D));_tmp77D->hd=({
struct _tuple22*_tmp77E=_region_malloc(rgn,sizeof(*_tmp77E));_tmp77E->f1=_tmp779;
_tmp77E->f2=_tmp77B;_tmp77E;});_tmp77D->tl=decls;_tmp77D;});{struct _tuple21
_tmp77F=Cyc_Toc_xlate_pat(nv,rgn,_tmp77A,Cyc_Absyn_var_exp(_tmp779,0),_tmp77C,
_tmp777,fail_stmt,decls);nv=_tmp77F.f1;decls=_tmp77F.f2;{struct Cyc_Absyn_Stmt*
_tmp780=_tmp77F.f3;struct Cyc_Absyn_Stmt*_tmp781=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp779,0),_tmp77C,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp781,_tmp780,
0),0);}}}}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(
_tmp774,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=_tmp759->is_extensible?
Cyc_Absyn_var_exp(_tmp75A->name,0): Cyc_Toc_datatype_tag(_tmp759,_tmp75A->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(
test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(
_tmp774,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(
_tmp774,_tmp775,(struct Cyc_Absyn_Exp*)rcast,s,0);goto _LL32C;}}_LL343: {struct Cyc_Absyn_Datatype_p_struct*
_tmp75C=(struct Cyc_Absyn_Datatype_p_struct*)_tmp738;if(_tmp75C->tag != 7)goto
_LL345;else{_tmp75D=_tmp75C->f3;}}_LL344: _tmp75F=_tmp75D;goto _LL346;_LL345: {
struct Cyc_Absyn_Tuple_p_struct*_tmp75E=(struct Cyc_Absyn_Tuple_p_struct*)_tmp738;
if(_tmp75E->tag != 4)goto _LL347;else{_tmp75F=_tmp75E->f1;}}_LL346: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp75F != 0;(_tmp75F=_tmp75F->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp783=(struct Cyc_Absyn_Pat*)_tmp75F->hd;if(_tmp783->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{struct _tuple1*_tmp784=Cyc_Toc_temp_var();void*_tmp785=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp783->topt))->v;decls=({struct Cyc_List_List*_tmp786=
_region_malloc(rgn,sizeof(*_tmp786));_tmp786->hd=({struct _tuple22*_tmp787=
_region_malloc(rgn,sizeof(*_tmp787));_tmp787->f1=_tmp784;_tmp787->f2=Cyc_Toc_typ_to_c(
_tmp785);_tmp787;});_tmp786->tl=decls;_tmp786;});{struct _tuple21 _tmp788=Cyc_Toc_xlate_pat(
nv,rgn,_tmp785,Cyc_Absyn_var_exp(_tmp784,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp783,fail_stmt,decls);nv=_tmp788.f1;decls=_tmp788.f2;{struct Cyc_Absyn_Stmt*
_tmp789=_tmp788.f3;struct Cyc_Absyn_Stmt*_tmp78A=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp784,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp78A,_tmp789,0),0);}}}}goto _LL32C;}_LL347: {struct Cyc_Absyn_Aggr_p_struct*
_tmp760=(struct Cyc_Absyn_Aggr_p_struct*)_tmp738;if(_tmp760->tag != 6)goto _LL349;
else{_tmp761=_tmp760->f1;if(_tmp761 != 0)goto _LL349;}}_LL348:({void*_tmp78B=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp78C="unresolved aggregate pattern!";_tag_dyneither(_tmp78C,sizeof(
char),30);}),_tag_dyneither(_tmp78B,sizeof(void*),0));});_LL349: {struct Cyc_Absyn_Aggr_p_struct*
_tmp762=(struct Cyc_Absyn_Aggr_p_struct*)_tmp738;if(_tmp762->tag != 6)goto _LL34B;
else{_tmp763=_tmp762->f1;if(_tmp763 == 0)goto _LL34B;_tmp764=*_tmp763;_tmp765=
_tmp764.aggr_info;_tmp766=_tmp762->f3;}}_LL34A: {struct Cyc_Absyn_Aggrdecl*
_tmp78D=Cyc_Absyn_get_known_aggrdecl(_tmp765);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp78D->impl))->tagged){struct _tuple23 _tmp78F;struct Cyc_List_List*
_tmp790;struct Cyc_Absyn_Pat*_tmp791;struct _tuple23*_tmp78E=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp766))->hd;_tmp78F=*_tmp78E;_tmp790=_tmp78F.f1;
_tmp791=_tmp78F.f2;{struct _dyneither_ptr*f;{void*_tmp792=(void*)((struct Cyc_List_List*)
_check_null(_tmp790))->hd;struct _dyneither_ptr*_tmp794;_LL354: {struct Cyc_Absyn_FieldName_struct*
_tmp793=(struct Cyc_Absyn_FieldName_struct*)_tmp792;if(_tmp793->tag != 1)goto
_LL356;else{_tmp794=_tmp793->f1;}}_LL355: f=_tmp794;goto _LL353;_LL356:;_LL357:({
void*_tmp795=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp796="no field name in tagged union pattern";_tag_dyneither(_tmp796,
sizeof(char),38);}),_tag_dyneither(_tmp795,sizeof(void*),0));});_LL353:;}{struct
_tuple1*_tmp797=Cyc_Toc_temp_var();void*_tmp798=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp791->topt))->v;void*_tmp799=Cyc_Toc_typ_to_c(_tmp798);decls=({
struct Cyc_List_List*_tmp79A=_region_malloc(rgn,sizeof(*_tmp79A));_tmp79A->hd=({
struct _tuple22*_tmp79B=_region_malloc(rgn,sizeof(*_tmp79B));_tmp79B->f1=_tmp797;
_tmp79B->f2=_tmp799;_tmp79B;});_tmp79A->tl=decls;_tmp79A;});{struct Cyc_Absyn_Exp*
_tmp79C=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,
0);struct Cyc_Absyn_Exp*_tmp79D=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(
r,f,0),Cyc_Toc_val_sp,0);_tmp79D=Cyc_Toc_cast_it(_tmp799,_tmp79D);_tmp79C=Cyc_Toc_cast_it(
_tmp799,_tmp79C);{struct _tuple21 _tmp79E=Cyc_Toc_xlate_pat(nv,rgn,_tmp798,Cyc_Absyn_var_exp(
_tmp797,0),_tmp79C,_tmp791,fail_stmt,decls);nv=_tmp79E.f1;decls=_tmp79E.f2;{
struct Cyc_Absyn_Stmt*_tmp79F=_tmp79E.f3;struct Cyc_Absyn_Stmt*_tmp7A0=Cyc_Toc_if_neq_stmt(
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp78D,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7A1=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp797,0),_tmp79D,0);s=Cyc_Absyn_seq_stmt(
_tmp7A0,Cyc_Absyn_seq_stmt(_tmp7A1,_tmp79F,0),0);}}}}}}else{s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp766 != 0;_tmp766=_tmp766->tl){struct _tuple23*_tmp7A2=(struct _tuple23*)
_tmp766->hd;struct Cyc_Absyn_Pat*_tmp7A3=(*_tmp7A2).f2;if(_tmp7A3->r == (void*)&
Cyc_Absyn_Wild_p_val)continue;{struct _dyneither_ptr*f;{void*_tmp7A4=(void*)((
struct Cyc_List_List*)_check_null((*_tmp7A2).f1))->hd;struct _dyneither_ptr*
_tmp7A6;_LL359: {struct Cyc_Absyn_FieldName_struct*_tmp7A5=(struct Cyc_Absyn_FieldName_struct*)
_tmp7A4;if(_tmp7A5->tag != 1)goto _LL35B;else{_tmp7A6=_tmp7A5->f1;}}_LL35A: f=
_tmp7A6;goto _LL358;_LL35B:;_LL35C:(int)_throw((void*)({struct Cyc_Toc_Match_error_struct*
_tmp7A7=_cycalloc_atomic(sizeof(*_tmp7A7));_tmp7A7[0]=({struct Cyc_Toc_Match_error_struct
_tmp7A8;_tmp7A8.tag=Cyc_Toc_Match_error;_tmp7A8;});_tmp7A7;}));_LL358:;}{struct
_tuple1*_tmp7A9=Cyc_Toc_temp_var();void*_tmp7AA=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7A3->topt))->v;void*_tmp7AB=Cyc_Toc_typ_to_c(_tmp7AA);decls=({
struct Cyc_List_List*_tmp7AC=_region_malloc(rgn,sizeof(*_tmp7AC));_tmp7AC->hd=({
struct _tuple22*_tmp7AD=_region_malloc(rgn,sizeof(*_tmp7AD));_tmp7AD->f1=_tmp7A9;
_tmp7AD->f2=_tmp7AB;_tmp7AD;});_tmp7AC->tl=decls;_tmp7AC;});{struct _tuple21
_tmp7AE=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AA,Cyc_Absyn_var_exp(_tmp7A9,0),Cyc_Absyn_aggrmember_exp(
path,f,0),_tmp7A3,fail_stmt,decls);nv=_tmp7AE.f1;decls=_tmp7AE.f2;{struct Cyc_Absyn_Exp*
_tmp7AF=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star_or_tvar(((struct
Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp78D->impl))->fields,f)))->type))_tmp7AF=Cyc_Toc_cast_it(_tmp7AB,
_tmp7AF);{struct Cyc_Absyn_Stmt*_tmp7B0=_tmp7AE.f3;struct Cyc_Absyn_Stmt*_tmp7B1=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7A9,0),_tmp7AF,0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp7B1,_tmp7B0,0),0);}}}}}}}goto _LL32C;}_LL34B: {struct Cyc_Absyn_Pointer_p_struct*
_tmp767=(struct Cyc_Absyn_Pointer_p_struct*)_tmp738;if(_tmp767->tag != 5)goto
_LL34D;else{_tmp768=_tmp767->f1;}}_LL34C: {struct _tuple1*_tmp7B2=Cyc_Toc_temp_var();
void*_tmp7B3=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp768->topt))->v;decls=({
struct Cyc_List_List*_tmp7B4=_region_malloc(rgn,sizeof(*_tmp7B4));_tmp7B4->hd=({
struct _tuple22*_tmp7B5=_region_malloc(rgn,sizeof(*_tmp7B5));_tmp7B5->f1=_tmp7B2;
_tmp7B5->f2=Cyc_Toc_typ_to_c(_tmp7B3);_tmp7B5;});_tmp7B4->tl=decls;_tmp7B4;});{
struct _tuple21 _tmp7B6=Cyc_Toc_xlate_pat(nv,rgn,_tmp7B3,Cyc_Absyn_var_exp(_tmp7B2,
0),Cyc_Absyn_deref_exp(path,0),_tmp768,fail_stmt,decls);nv=_tmp7B6.f1;decls=
_tmp7B6.f2;{struct Cyc_Absyn_Stmt*_tmp7B7=_tmp7B6.f3;struct Cyc_Absyn_Stmt*_tmp7B8=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B2,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7B7,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7B8,0);else{s=_tmp7B8;}goto _LL32C;}}}_LL34D: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp769=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp738;if(_tmp769->tag != 14)goto
_LL34F;}_LL34E:({void*_tmp7B9=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp7BA="unknownid pat";
_tag_dyneither(_tmp7BA,sizeof(char),14);}),_tag_dyneither(_tmp7B9,sizeof(void*),
0));});_LL34F: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp76A=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp738;if(_tmp76A->tag != 15)goto _LL351;}_LL350:({void*_tmp7BB=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp7BC="unknowncall pat";
_tag_dyneither(_tmp7BC,sizeof(char),16);}),_tag_dyneither(_tmp7BB,sizeof(void*),
0));});_LL351: {struct Cyc_Absyn_Exp_p_struct*_tmp76B=(struct Cyc_Absyn_Exp_p_struct*)
_tmp738;if(_tmp76B->tag != 16)goto _LL32C;}_LL352:({void*_tmp7BD=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp7BE="exp pat";
_tag_dyneither(_tmp7BE,sizeof(char),8);}),_tag_dyneither(_tmp7BD,sizeof(void*),0));});
_LL32C:;}return({struct _tuple21 _tmp7BF;_tmp7BF.f1=nv;_tmp7BF.f2=decls;_tmp7BF.f3=
s;_tmp7BF;});}struct _tuple24{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;
struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(struct
_RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){return({struct _tuple24*_tmp7C0=
_region_malloc(r,sizeof(*_tmp7C0));_tmp7C0->f1=Cyc_Toc_fresh_label();_tmp7C0->f2=
Cyc_Toc_fresh_label();_tmp7C0->f3=sc;_tmp7C0;});}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp7C1=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp7C2=Cyc_Tcutil_compress(
_tmp7C1);_LL35E: {struct Cyc_Absyn_IntType_struct*_tmp7C3=(struct Cyc_Absyn_IntType_struct*)
_tmp7C2;if(_tmp7C3->tag != 6)goto _LL360;}_LL35F: goto _LL361;_LL360: {struct Cyc_Absyn_EnumType_struct*
_tmp7C4=(struct Cyc_Absyn_EnumType_struct*)_tmp7C2;if(_tmp7C4->tag != 14)goto
_LL362;}_LL361: leave_as_switch=1;goto _LL35D;_LL362:;_LL363: leave_as_switch=0;
goto _LL35D;_LL35D:;}{struct Cyc_List_List*_tmp7C5=scs;for(0;_tmp7C5 != 0;_tmp7C5=
_tmp7C5->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Absyn_Switch_clause*)_tmp7C5->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)
_tmp7C5->hd)->where_clause != 0){leave_as_switch=0;break;}}}if(leave_as_switch){
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp7C6=
scs;for(0;_tmp7C6 != 0;_tmp7C6=_tmp7C6->tl){struct Cyc_Absyn_Stmt*_tmp7C7=((struct
Cyc_Absyn_Switch_clause*)_tmp7C6->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7C6->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7C7,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7C9;struct _RegionHandle*_tmp7CA;struct Cyc_Toc_Env*_tmp7C8=
nv;_tmp7C9=*_tmp7C8;_tmp7CA=_tmp7C9.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7CA,nv,next_l),_tmp7C7);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7CC;
struct _RegionHandle*_tmp7CD;struct Cyc_Toc_Env*_tmp7CB=nv;_tmp7CC=*_tmp7CB;
_tmp7CD=_tmp7CC.rgn;{struct Cyc_Toc_Env*_tmp7CE=Cyc_Toc_share_env(_tmp7CD,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7CD,Cyc_Toc_gen_label,
_tmp7CD,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7CF=lscs;for(0;_tmp7CF != 0;
_tmp7CF=_tmp7CF->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7CF->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7CF->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7CF->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7D1;struct Cyc_List_List*_tmp7D2;struct Cyc_Absyn_Stmt*_tmp7D3;struct _tuple21
_tmp7D0=Cyc_Toc_xlate_pat(_tmp7CE,_tmp7CD,_tmp7C1,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7D1=_tmp7D0.f1;_tmp7D2=_tmp7D0.f2;_tmp7D3=_tmp7D0.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7D4=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7D1,_tmp7D4);_tmp7D3=Cyc_Absyn_seq_stmt(
_tmp7D3,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7D4,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7D2;nvs=({
struct Cyc_List_List*_tmp7D5=_region_malloc(_tmp7CD,sizeof(*_tmp7D5));_tmp7D5->hd=
_tmp7D1;_tmp7D5->tl=nvs;_tmp7D5;});test_stmts=({struct Cyc_List_List*_tmp7D6=
_region_malloc(_tmp7CD,sizeof(*_tmp7D6));_tmp7D6->hd=_tmp7D3;_tmp7D6->tl=
test_stmts;_tmp7D6;});}}nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
nvs);test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
test_stmts);{struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple24 _tmp7D8;struct _dyneither_ptr*_tmp7D9;
struct _dyneither_ptr*_tmp7DA;struct Cyc_Absyn_Switch_clause*_tmp7DB;struct
_tuple24*_tmp7D7=(struct _tuple24*)lscs->hd;_tmp7D8=*_tmp7D7;_tmp7D9=_tmp7D8.f1;
_tmp7DA=_tmp7D8.f2;_tmp7DB=_tmp7D8.f3;{struct Cyc_Toc_Env*_tmp7DC=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7DB->body;
struct Cyc_Toc_Env _tmp7DE;struct _RegionHandle*_tmp7DF;struct Cyc_Toc_Env*_tmp7DD=
_tmp7CE;_tmp7DE=*_tmp7DD;_tmp7DF=_tmp7DE.rgn;if(lscs->tl != 0){struct _tuple24
_tmp7E1;struct _dyneither_ptr*_tmp7E2;struct Cyc_Absyn_Switch_clause*_tmp7E3;
struct _tuple24*_tmp7E0=(struct _tuple24*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;
_tmp7E1=*_tmp7E0;_tmp7E2=_tmp7E1.f2;_tmp7E3=_tmp7E1.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp7DF,_tmp7DC,end_l,_tmp7E2,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp7E3->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(
_tmp7DF,_tmp7DC,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7D9,(
struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7DA,s,0),0);stmts=({struct Cyc_List_List*_tmp7E4=_region_malloc(_tmp7CD,
sizeof(*_tmp7E4));_tmp7E4->hd=s;_tmp7E4->tl=stmts;_tmp7E4;});}}{struct Cyc_Absyn_Stmt*
res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22 _tmp7E6;struct _tuple1*
_tmp7E7;void*_tmp7E8;struct _tuple22*_tmp7E5=(struct _tuple22*)decls->hd;_tmp7E6=*
_tmp7E5;_tmp7E7=_tmp7E6.f1;_tmp7E8=_tmp7E6.f2;res=Cyc_Absyn_declare_stmt(_tmp7E7,
_tmp7E8,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r;}}}}}}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(
int n){return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({
struct Cyc_List_List*_tmp7E9=_cycalloc(sizeof(*_tmp7E9));_tmp7E9->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0);_tmp7E9->tl=0;_tmp7E9;}),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp7EA=s->r;struct Cyc_Absyn_Exp*_tmp7ED;
struct Cyc_Absyn_Stmt*_tmp7EF;struct Cyc_Absyn_Stmt*_tmp7F0;struct Cyc_Absyn_Exp*
_tmp7F2;struct Cyc_Absyn_Exp*_tmp7F4;struct Cyc_Absyn_Stmt*_tmp7F5;struct Cyc_Absyn_Stmt*
_tmp7F6;struct _tuple9 _tmp7F8;struct Cyc_Absyn_Exp*_tmp7F9;struct Cyc_Absyn_Stmt*
_tmp7FA;struct Cyc_Absyn_Stmt*_tmp7FC;struct Cyc_Absyn_Stmt*_tmp7FE;struct Cyc_Absyn_Stmt*
_tmp800;struct Cyc_Absyn_Exp*_tmp802;struct _tuple9 _tmp803;struct Cyc_Absyn_Exp*
_tmp804;struct _tuple9 _tmp805;struct Cyc_Absyn_Exp*_tmp806;struct Cyc_Absyn_Stmt*
_tmp807;struct Cyc_Absyn_Exp*_tmp809;struct Cyc_List_List*_tmp80A;struct Cyc_List_List*
_tmp80C;struct Cyc_Absyn_Switch_clause**_tmp80D;struct Cyc_Absyn_Decl*_tmp80F;
struct Cyc_Absyn_Stmt*_tmp810;struct _dyneither_ptr*_tmp812;struct Cyc_Absyn_Stmt*
_tmp813;struct Cyc_Absyn_Stmt*_tmp815;struct _tuple9 _tmp816;struct Cyc_Absyn_Exp*
_tmp817;struct Cyc_Absyn_Stmt*_tmp819;struct Cyc_List_List*_tmp81A;struct Cyc_Absyn_Exp*
_tmp81C;_LL365: {struct Cyc_Absyn_Skip_s_struct*_tmp7EB=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7EA;if(_tmp7EB->tag != 0)goto _LL367;}_LL366: return;_LL367: {struct Cyc_Absyn_Exp_s_struct*
_tmp7EC=(struct Cyc_Absyn_Exp_s_struct*)_tmp7EA;if(_tmp7EC->tag != 1)goto _LL369;
else{_tmp7ED=_tmp7EC->f1;}}_LL368: Cyc_Toc_exp_to_c(nv,_tmp7ED);return;_LL369: {
struct Cyc_Absyn_Seq_s_struct*_tmp7EE=(struct Cyc_Absyn_Seq_s_struct*)_tmp7EA;if(
_tmp7EE->tag != 2)goto _LL36B;else{_tmp7EF=_tmp7EE->f1;_tmp7F0=_tmp7EE->f2;}}
_LL36A: Cyc_Toc_stmt_to_c(nv,_tmp7EF);s=_tmp7F0;continue;_LL36B: {struct Cyc_Absyn_Return_s_struct*
_tmp7F1=(struct Cyc_Absyn_Return_s_struct*)_tmp7EA;if(_tmp7F1->tag != 3)goto _LL36D;
else{_tmp7F2=_tmp7F1->f1;}}_LL36C: {struct Cyc_Core_Opt*topt=0;if(_tmp7F2 != 0){
topt=({struct Cyc_Core_Opt*_tmp81D=_cycalloc(sizeof(*_tmp81D));_tmp81D->v=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F2->topt))->v);_tmp81D;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp7F2);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp81E=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp81F=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp81E,0),0);s->r=(Cyc_Absyn_declare_stmt(
_tmp81E,(void*)topt->v,_tmp7F2,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),
_tmp81F,0),0))->r;}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}return;}_LL36D: {
struct Cyc_Absyn_IfThenElse_s_struct*_tmp7F3=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp7EA;if(_tmp7F3->tag != 4)goto _LL36F;else{_tmp7F4=_tmp7F3->f1;_tmp7F5=_tmp7F3->f2;
_tmp7F6=_tmp7F3->f3;}}_LL36E: Cyc_Toc_exp_to_c(nv,_tmp7F4);Cyc_Toc_stmt_to_c(nv,
_tmp7F5);s=_tmp7F6;continue;_LL36F: {struct Cyc_Absyn_While_s_struct*_tmp7F7=(
struct Cyc_Absyn_While_s_struct*)_tmp7EA;if(_tmp7F7->tag != 5)goto _LL371;else{
_tmp7F8=_tmp7F7->f1;_tmp7F9=_tmp7F8.f1;_tmp7FA=_tmp7F7->f2;}}_LL370: Cyc_Toc_exp_to_c(
nv,_tmp7F9);{struct Cyc_Toc_Env _tmp821;struct _RegionHandle*_tmp822;struct Cyc_Toc_Env*
_tmp820=nv;_tmp821=*_tmp820;_tmp822=_tmp821.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp822,nv),_tmp7FA);return;}_LL371: {struct Cyc_Absyn_Break_s_struct*_tmp7FB=(
struct Cyc_Absyn_Break_s_struct*)_tmp7EA;if(_tmp7FB->tag != 6)goto _LL373;else{
_tmp7FC=_tmp7FB->f1;}}_LL372: {struct Cyc_Toc_Env _tmp824;struct _dyneither_ptr**
_tmp825;struct Cyc_Toc_Env*_tmp823=nv;_tmp824=*_tmp823;_tmp825=_tmp824.break_lab;
if(_tmp825 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp825,0);{int dest_depth=_tmp7FC == 0?0:
_tmp7FC->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}
_LL373: {struct Cyc_Absyn_Continue_s_struct*_tmp7FD=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7EA;if(_tmp7FD->tag != 7)goto _LL375;else{_tmp7FE=_tmp7FD->f1;}}_LL374: {
struct Cyc_Toc_Env _tmp827;struct _dyneither_ptr**_tmp828;struct Cyc_Toc_Env*_tmp826=
nv;_tmp827=*_tmp826;_tmp828=_tmp827.continue_lab;if(_tmp828 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp828,0);_tmp800=_tmp7FE;goto _LL376;}_LL375: {struct Cyc_Absyn_Goto_s_struct*
_tmp7FF=(struct Cyc_Absyn_Goto_s_struct*)_tmp7EA;if(_tmp7FF->tag != 8)goto _LL377;
else{_tmp800=_tmp7FF->f2;}}_LL376: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp800))->try_depth,s);return;_LL377: {struct Cyc_Absyn_For_s_struct*
_tmp801=(struct Cyc_Absyn_For_s_struct*)_tmp7EA;if(_tmp801->tag != 9)goto _LL379;
else{_tmp802=_tmp801->f1;_tmp803=_tmp801->f2;_tmp804=_tmp803.f1;_tmp805=_tmp801->f3;
_tmp806=_tmp805.f1;_tmp807=_tmp801->f4;}}_LL378: Cyc_Toc_exp_to_c(nv,_tmp802);Cyc_Toc_exp_to_c(
nv,_tmp804);Cyc_Toc_exp_to_c(nv,_tmp806);{struct Cyc_Toc_Env _tmp82A;struct
_RegionHandle*_tmp82B;struct Cyc_Toc_Env*_tmp829=nv;_tmp82A=*_tmp829;_tmp82B=
_tmp82A.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp82B,nv),_tmp807);return;}
_LL379: {struct Cyc_Absyn_Switch_s_struct*_tmp808=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7EA;if(_tmp808->tag != 10)goto _LL37B;else{_tmp809=_tmp808->f1;_tmp80A=_tmp808->f2;}}
_LL37A: Cyc_Toc_xlate_switch(nv,s,_tmp809,_tmp80A);return;_LL37B: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp80B=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7EA;if(_tmp80B->tag != 11)goto
_LL37D;else{_tmp80C=_tmp80B->f1;_tmp80D=_tmp80B->f2;}}_LL37C: {struct Cyc_Toc_Env
_tmp82D;struct Cyc_Toc_FallthruInfo*_tmp82E;struct Cyc_Toc_Env*_tmp82C=nv;_tmp82D=*
_tmp82C;_tmp82E=_tmp82D.fallthru_info;if(_tmp82E == 0)({void*_tmp82F=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp830="fallthru in unexpected place";_tag_dyneither(_tmp830,sizeof(char),29);}),
_tag_dyneither(_tmp82F,sizeof(void*),0));});{struct _dyneither_ptr*_tmp832;struct
Cyc_List_List*_tmp833;struct Cyc_Dict_Dict _tmp834;struct Cyc_Toc_FallthruInfo
_tmp831=*_tmp82E;_tmp832=_tmp831.label;_tmp833=_tmp831.binders;_tmp834=_tmp831.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp832,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp80D)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp835=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp833);struct Cyc_List_List*_tmp836=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp80C);for(0;_tmp835 != 0;(_tmp835=_tmp835->tl,
_tmp836=_tmp836->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp836))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp834,(
struct _tuple1*)_tmp835->hd),(struct Cyc_Absyn_Exp*)_tmp836->hd,0),s2,0);}s->r=s2->r;
return;}}}}_LL37D: {struct Cyc_Absyn_Decl_s_struct*_tmp80E=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7EA;if(_tmp80E->tag != 12)goto _LL37F;else{_tmp80F=_tmp80E->f1;_tmp810=_tmp80E->f2;}}
_LL37E:{void*_tmp837=_tmp80F->r;struct Cyc_Absyn_Vardecl*_tmp839;struct Cyc_Absyn_Pat*
_tmp83B;struct Cyc_Absyn_Exp*_tmp83C;struct Cyc_List_List*_tmp83E;struct Cyc_Absyn_Fndecl*
_tmp840;struct Cyc_Absyn_Tvar*_tmp842;struct Cyc_Absyn_Vardecl*_tmp843;int _tmp844;
struct Cyc_Absyn_Exp*_tmp845;struct Cyc_Absyn_Exp*_tmp847;struct Cyc_Absyn_Tvar*
_tmp848;struct Cyc_Absyn_Vardecl*_tmp849;_LL388: {struct Cyc_Absyn_Var_d_struct*
_tmp838=(struct Cyc_Absyn_Var_d_struct*)_tmp837;if(_tmp838->tag != 0)goto _LL38A;
else{_tmp839=_tmp838->f1;}}_LL389: {struct Cyc_Toc_Env _tmp84B;struct _RegionHandle*
_tmp84C;struct Cyc_Toc_Env*_tmp84A=nv;_tmp84B=*_tmp84A;_tmp84C=_tmp84B.rgn;{
struct Cyc_Toc_Env*_tmp84D=Cyc_Toc_add_varmap(_tmp84C,nv,_tmp839->name,Cyc_Absyn_varb_exp(
_tmp839->name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp84E=_cycalloc(sizeof(*
_tmp84E));_tmp84E[0]=({struct Cyc_Absyn_Local_b_struct _tmp84F;_tmp84F.tag=4;
_tmp84F.f1=_tmp839;_tmp84F;});_tmp84E;}),0));Cyc_Toc_local_decl_to_c(_tmp84D,
_tmp84D,_tmp839,_tmp810);}goto _LL387;}_LL38A: {struct Cyc_Absyn_Let_d_struct*
_tmp83A=(struct Cyc_Absyn_Let_d_struct*)_tmp837;if(_tmp83A->tag != 2)goto _LL38C;
else{_tmp83B=_tmp83A->f1;_tmp83C=_tmp83A->f3;}}_LL38B:{void*_tmp850=_tmp83B->r;
struct Cyc_Absyn_Vardecl*_tmp852;struct Cyc_Absyn_Pat*_tmp853;struct Cyc_Absyn_Pat
_tmp854;void*_tmp855;_LL397: {struct Cyc_Absyn_Var_p_struct*_tmp851=(struct Cyc_Absyn_Var_p_struct*)
_tmp850;if(_tmp851->tag != 1)goto _LL399;else{_tmp852=_tmp851->f1;_tmp853=_tmp851->f2;
_tmp854=*_tmp853;_tmp855=_tmp854.r;{struct Cyc_Absyn_Wild_p_struct*_tmp856=(
struct Cyc_Absyn_Wild_p_struct*)_tmp855;if(_tmp856->tag != 0)goto _LL399;}}}_LL398: {
struct _tuple1*old_name=_tmp852->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp852->name=new_name;_tmp852->initializer=(struct Cyc_Absyn_Exp*)_tmp83C;
_tmp80F->r=(void*)({struct Cyc_Absyn_Var_d_struct*_tmp857=_cycalloc(sizeof(*
_tmp857));_tmp857[0]=({struct Cyc_Absyn_Var_d_struct _tmp858;_tmp858.tag=0;_tmp858.f1=
_tmp852;_tmp858;});_tmp857;});{struct Cyc_Toc_Env _tmp85A;struct _RegionHandle*
_tmp85B;struct Cyc_Toc_Env*_tmp859=nv;_tmp85A=*_tmp859;_tmp85B=_tmp85A.rgn;{
struct Cyc_Toc_Env*_tmp85C=Cyc_Toc_add_varmap(_tmp85B,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp85D=_cycalloc(sizeof(*
_tmp85D));_tmp85D[0]=({struct Cyc_Absyn_Local_b_struct _tmp85E;_tmp85E.tag=4;
_tmp85E.f1=_tmp852;_tmp85E;});_tmp85D;}),0));Cyc_Toc_local_decl_to_c(_tmp85C,nv,
_tmp852,_tmp810);}goto _LL396;}}_LL399:;_LL39A: s->r=(Cyc_Toc_letdecl_to_c(nv,
_tmp83B,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp83C->topt))->v,_tmp83C,
_tmp810))->r;goto _LL396;_LL396:;}goto _LL387;_LL38C: {struct Cyc_Absyn_Letv_d_struct*
_tmp83D=(struct Cyc_Absyn_Letv_d_struct*)_tmp837;if(_tmp83D->tag != 3)goto _LL38E;
else{_tmp83E=_tmp83D->f1;}}_LL38D: {struct Cyc_List_List*_tmp85F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp83E);if(_tmp85F == 0)({void*_tmp860=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp861="empty Letv_d";_tag_dyneither(_tmp861,sizeof(char),13);}),
_tag_dyneither(_tmp860,sizeof(void*),0));});_tmp80F->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp862=_cycalloc(sizeof(*_tmp862));_tmp862[0]=({struct Cyc_Absyn_Var_d_struct
_tmp863;_tmp863.tag=0;_tmp863.f1=(struct Cyc_Absyn_Vardecl*)_tmp85F->hd;_tmp863;});
_tmp862;});_tmp85F=_tmp85F->tl;for(0;_tmp85F != 0;_tmp85F=_tmp85F->tl){struct Cyc_Absyn_Decl*
_tmp864=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp865=
_cycalloc(sizeof(*_tmp865));_tmp865[0]=({struct Cyc_Absyn_Var_d_struct _tmp866;
_tmp866.tag=0;_tmp866.f1=(struct Cyc_Absyn_Vardecl*)_tmp85F->hd;_tmp866;});
_tmp865;}),0);s->r=(Cyc_Absyn_decl_stmt(_tmp864,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
Cyc_Toc_stmt_to_c(nv,s);goto _LL387;}_LL38E: {struct Cyc_Absyn_Fn_d_struct*_tmp83F=(
struct Cyc_Absyn_Fn_d_struct*)_tmp837;if(_tmp83F->tag != 1)goto _LL390;else{_tmp840=
_tmp83F->f1;}}_LL38F: {struct _tuple1*_tmp867=_tmp840->name;struct Cyc_Toc_Env
_tmp869;struct _RegionHandle*_tmp86A;struct Cyc_Toc_Env*_tmp868=nv;_tmp869=*
_tmp868;_tmp86A=_tmp869.rgn;{struct Cyc_Toc_Env*_tmp86B=Cyc_Toc_add_varmap(
_tmp86A,nv,_tmp840->name,Cyc_Absyn_var_exp(_tmp867,0));Cyc_Toc_fndecl_to_c(
_tmp86B,_tmp840,0);Cyc_Toc_stmt_to_c(_tmp86B,_tmp810);}goto _LL387;}_LL390: {
struct Cyc_Absyn_Region_d_struct*_tmp841=(struct Cyc_Absyn_Region_d_struct*)
_tmp837;if(_tmp841->tag != 4)goto _LL392;else{_tmp842=_tmp841->f1;_tmp843=_tmp841->f2;
_tmp844=_tmp841->f3;_tmp845=_tmp841->f4;}}_LL391: {struct Cyc_Absyn_Stmt*_tmp86C=
_tmp810;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*
rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp843->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp86E;struct _RegionHandle*_tmp86F;struct Cyc_Toc_Env*
_tmp86D=nv;_tmp86E=*_tmp86D;_tmp86F=_tmp86E.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp86F,nv,x_var,x_exp),_tmp86C);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp86C,0))->r;
else{if(_tmp845 == 0)s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp870=
_cycalloc(sizeof(*_tmp870));_tmp870->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0);_tmp870->tl=0;_tmp870;}),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*
_tmp871[1];_tmp871[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp871,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),Cyc_Absyn_seq_stmt(
_tmp86C,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct
Cyc_Absyn_Exp*_tmp872[1];_tmp872[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp872,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0),0))->r;else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp845);s->r=(Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),
0,Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(
Cyc_Toc__open_dynregion_e,({struct Cyc_Absyn_Exp*_tmp873[2];_tmp873[1]=(struct Cyc_Absyn_Exp*)
_tmp845;_tmp873[0]=Cyc_Absyn_address_exp(rh_exp,0);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp873,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_seq_stmt(_tmp86C,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp874[1];_tmp874[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp874,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0))->r;}}return;}_LL392: {struct Cyc_Absyn_Alias_d_struct*
_tmp846=(struct Cyc_Absyn_Alias_d_struct*)_tmp837;if(_tmp846->tag != 5)goto _LL394;
else{_tmp847=_tmp846->f1;_tmp848=_tmp846->f2;_tmp849=_tmp846->f3;}}_LL393: {
struct _tuple1*old_name=_tmp849->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp849->name=new_name;_tmp849->initializer=(struct Cyc_Absyn_Exp*)_tmp847;s->r=(
void*)({struct Cyc_Absyn_Decl_s_struct*_tmp875=_cycalloc(sizeof(*_tmp875));
_tmp875[0]=({struct Cyc_Absyn_Decl_s_struct _tmp876;_tmp876.tag=12;_tmp876.f1=({
struct Cyc_Absyn_Decl*_tmp877=_cycalloc(sizeof(*_tmp877));_tmp877->r=(void*)({
struct Cyc_Absyn_Var_d_struct*_tmp878=_cycalloc(sizeof(*_tmp878));_tmp878[0]=({
struct Cyc_Absyn_Var_d_struct _tmp879;_tmp879.tag=0;_tmp879.f1=_tmp849;_tmp879;});
_tmp878;});_tmp877->loc=0;_tmp877;});_tmp876.f2=_tmp810;_tmp876;});_tmp875;});{
struct Cyc_Toc_Env _tmp87B;struct _RegionHandle*_tmp87C;struct Cyc_Toc_Env*_tmp87A=
nv;_tmp87B=*_tmp87A;_tmp87C=_tmp87B.rgn;{struct Cyc_Toc_Env*_tmp87D=Cyc_Toc_add_varmap(
_tmp87C,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp87E=_cycalloc(sizeof(*_tmp87E));_tmp87E[0]=({struct Cyc_Absyn_Local_b_struct
_tmp87F;_tmp87F.tag=4;_tmp87F.f1=_tmp849;_tmp87F;});_tmp87E;}),0));Cyc_Toc_local_decl_to_c(
_tmp87D,nv,_tmp849,_tmp810);}return;}}_LL394:;_LL395:({void*_tmp880=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp881="bad nested declaration within function";_tag_dyneither(_tmp881,sizeof(
char),39);}),_tag_dyneither(_tmp880,sizeof(void*),0));});_LL387:;}return;_LL37F: {
struct Cyc_Absyn_Label_s_struct*_tmp811=(struct Cyc_Absyn_Label_s_struct*)_tmp7EA;
if(_tmp811->tag != 13)goto _LL381;else{_tmp812=_tmp811->f1;_tmp813=_tmp811->f2;}}
_LL380: s=_tmp813;continue;_LL381: {struct Cyc_Absyn_Do_s_struct*_tmp814=(struct
Cyc_Absyn_Do_s_struct*)_tmp7EA;if(_tmp814->tag != 14)goto _LL383;else{_tmp815=
_tmp814->f1;_tmp816=_tmp814->f2;_tmp817=_tmp816.f1;}}_LL382: {struct Cyc_Toc_Env
_tmp883;struct _RegionHandle*_tmp884;struct Cyc_Toc_Env*_tmp882=nv;_tmp883=*
_tmp882;_tmp884=_tmp883.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp884,nv),
_tmp815);Cyc_Toc_exp_to_c(nv,_tmp817);return;}_LL383: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp818=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7EA;if(_tmp818->tag != 15)goto
_LL385;else{_tmp819=_tmp818->f1;_tmp81A=_tmp818->f2;}}_LL384: {struct _tuple1*
h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();struct _tuple1*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp885=_cycalloc(sizeof(*_tmp885));_tmp885->v=e_typ;_tmp885;});{struct Cyc_Toc_Env
_tmp887;struct _RegionHandle*_tmp888;struct Cyc_Toc_Env*_tmp886=nv;_tmp887=*
_tmp886;_tmp888=_tmp887.rgn;{struct Cyc_Toc_Env*_tmp889=Cyc_Toc_add_varmap(
_tmp888,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp889,_tmp819);{struct Cyc_Absyn_Stmt*
_tmp88A=Cyc_Absyn_seq_stmt(_tmp819,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp88B=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp88C=
Cyc_Absyn_var_exp(_tmp88B,0);struct Cyc_Absyn_Vardecl*_tmp88D=Cyc_Absyn_new_vardecl(
_tmp88B,Cyc_Absyn_exn_typ(),0);_tmp88C->topt=({struct Cyc_Core_Opt*_tmp88E=
_cycalloc(sizeof(*_tmp88E));_tmp88E->v=Cyc_Absyn_exn_typ();_tmp88E;});{struct Cyc_Absyn_Pat*
_tmp88F=({struct Cyc_Absyn_Pat*_tmp89F=_cycalloc(sizeof(*_tmp89F));_tmp89F->r=(
void*)({struct Cyc_Absyn_Var_p_struct*_tmp8A1=_cycalloc(sizeof(*_tmp8A1));_tmp8A1[
0]=({struct Cyc_Absyn_Var_p_struct _tmp8A2;_tmp8A2.tag=1;_tmp8A2.f1=_tmp88D;
_tmp8A2.f2=({struct Cyc_Absyn_Pat*_tmp8A3=_cycalloc(sizeof(*_tmp8A3));_tmp8A3->r=(
void*)& Cyc_Absyn_Wild_p_val;_tmp8A3->topt=({struct Cyc_Core_Opt*_tmp8A4=_cycalloc(
sizeof(*_tmp8A4));_tmp8A4->v=Cyc_Absyn_exn_typ();_tmp8A4;});_tmp8A3->loc=0;
_tmp8A3;});_tmp8A2;});_tmp8A1;});_tmp89F->topt=({struct Cyc_Core_Opt*_tmp8A0=
_cycalloc(sizeof(*_tmp8A0));_tmp8A0->v=Cyc_Absyn_exn_typ();_tmp8A0;});_tmp89F->loc=
0;_tmp89F;});struct Cyc_Absyn_Exp*_tmp890=Cyc_Absyn_throw_exp(_tmp88C,0);_tmp890->topt=({
struct Cyc_Core_Opt*_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891->v=(void*)& Cyc_Absyn_VoidType_val;
_tmp891;});{struct Cyc_Absyn_Stmt*_tmp892=Cyc_Absyn_exp_stmt(_tmp890,0);struct Cyc_Absyn_Switch_clause*
_tmp893=({struct Cyc_Absyn_Switch_clause*_tmp89C=_cycalloc(sizeof(*_tmp89C));
_tmp89C->pattern=_tmp88F;_tmp89C->pat_vars=({struct Cyc_Core_Opt*_tmp89D=
_cycalloc(sizeof(*_tmp89D));_tmp89D->v=({struct Cyc_List_List*_tmp89E=_cycalloc(
sizeof(*_tmp89E));_tmp89E->hd=_tmp88D;_tmp89E->tl=0;_tmp89E;});_tmp89D;});
_tmp89C->where_clause=0;_tmp89C->body=_tmp892;_tmp89C->loc=0;_tmp89C;});struct
Cyc_Absyn_Stmt*_tmp894=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp81A,({struct
Cyc_List_List*_tmp89B=_cycalloc(sizeof(*_tmp89B));_tmp89B->hd=_tmp893;_tmp89B->tl=
0;_tmp89B;})),0);Cyc_Toc_stmt_to_c(_tmp889,_tmp894);{struct Cyc_Absyn_Exp*_tmp895=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp89A=_cycalloc(
sizeof(*_tmp89A));_tmp89A->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp89A->tl=0;_tmp89A;}),0);struct Cyc_Absyn_Stmt*_tmp896=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp899=
_cycalloc(sizeof(*_tmp899));_tmp899->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp899->tl=
0;_tmp899;}),0),0);struct Cyc_Absyn_Exp*_tmp897=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
0,0);struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp896,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp897,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp895,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp898,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp88A,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp894,0),0),0),0),0),0))->r;}}}}}
return;}}_LL385: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp81B=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7EA;if(_tmp81B->tag != 16)goto _LL364;else{_tmp81C=_tmp81B->f1;}}_LL386: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp81C);s->r=Cyc_Toc_exp_stmt_r(
Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp8A5=
_cycalloc(sizeof(*_tmp8A5));_tmp8A5->hd=_tmp81C;_tmp8A5->tl=0;_tmp8A5;}),0));}
return;_LL364:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);struct
_tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){f->tvs=0;f->effect=0;
f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{struct _RegionHandle
_tmp8A6=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8A6;_push_region(frgn);{
struct Cyc_Toc_Env*_tmp8A7=Cyc_Toc_share_env(frgn,nv);{struct Cyc_List_List*
_tmp8A8=f->args;for(0;_tmp8A8 != 0;_tmp8A8=_tmp8A8->tl){struct _tuple1*_tmp8A9=({
struct _tuple1*_tmp8AA=_cycalloc(sizeof(*_tmp8AA));_tmp8AA->f1=(union Cyc_Absyn_Nmspace)
Cyc_Absyn_Loc_n;_tmp8AA->f2=(*((struct _tuple25*)_tmp8A8->hd)).f1;_tmp8AA;});(*((
struct _tuple25*)_tmp8A8->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp8A8->hd)).f3);
_tmp8A7=Cyc_Toc_add_varmap(frgn,_tmp8A7,_tmp8A9,Cyc_Absyn_var_exp(_tmp8A9,0));}}
if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);_npop_handler(0);return;}if((
unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name
!= 0){struct Cyc_Core_Opt*_tmp8AC;struct Cyc_Absyn_Tqual _tmp8AD;void*_tmp8AE;int
_tmp8AF;struct Cyc_Absyn_VarargInfo _tmp8AB=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8AC=_tmp8AB.name;_tmp8AD=_tmp8AB.tq;_tmp8AE=
_tmp8AB.type;_tmp8AF=_tmp8AB.inject;{void*_tmp8B0=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8AE,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8AD,Cyc_Absyn_false_conref));struct
_tuple1*_tmp8B1=({struct _tuple1*_tmp8B4=_cycalloc(sizeof(*_tmp8B4));_tmp8B4->f1=(
union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n;_tmp8B4->f2=(struct _dyneither_ptr*)((
struct Cyc_Core_Opt*)_check_null(_tmp8AC))->v;_tmp8B4;});f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct
Cyc_List_List*_tmp8B2=_cycalloc(sizeof(*_tmp8B2));_tmp8B2->hd=({struct _tuple25*
_tmp8B3=_cycalloc(sizeof(*_tmp8B3));_tmp8B3->f1=(struct _dyneither_ptr*)_tmp8AC->v;
_tmp8B3->f2=_tmp8AD;_tmp8B3->f3=_tmp8B0;_tmp8B3;});_tmp8B2->tl=0;_tmp8B2;}));
_tmp8A7=Cyc_Toc_add_varmap(frgn,_tmp8A7,_tmp8B1,Cyc_Absyn_var_exp(_tmp8B1,0));f->cyc_varargs=
0;}}{struct Cyc_List_List*_tmp8B5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(f->param_vardecls))->v;for(0;_tmp8B5 != 0;_tmp8B5=_tmp8B5->tl){((
struct Cyc_Absyn_Vardecl*)_tmp8B5->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)
_tmp8B5->hd)->type);}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8A7),f->body);};
_pop_region(frgn);}}static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope
s){switch(s){case Cyc_Absyn_Abstract: _LL39B: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC:
_LL39C: return Cyc_Absyn_Extern;default: _LL39D: return s;}}static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*_tmp8B6=ad->name;struct
_DynRegionHandle*_tmp8B8;struct Cyc_Dict_Dict*_tmp8B9;struct Cyc_Toc_TocState
_tmp8B7=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8B8=
_tmp8B7.dyn;_tmp8B9=_tmp8B7.aggrs_so_far;{struct _DynRegionFrame _tmp8BA;struct
_RegionHandle*d=_open_dynregion(& _tmp8BA,_tmp8B8);{int seen_defn_before;struct
_tuple15**_tmp8BB=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))
Cyc_Dict_lookup_opt)(*_tmp8B9,_tmp8B6);if(_tmp8BB == 0){seen_defn_before=0;{
struct _tuple15*v;if(ad->kind == Cyc_Absyn_StructA)v=({struct _tuple15*_tmp8BC=
_region_malloc(d,sizeof(*_tmp8BC));_tmp8BC->f1=ad;_tmp8BC->f2=Cyc_Absyn_strctq(
_tmp8B6);_tmp8BC;});else{v=({struct _tuple15*_tmp8BD=_region_malloc(d,sizeof(*
_tmp8BD));_tmp8BD->f1=ad;_tmp8BD->f2=Cyc_Absyn_unionq_typ(_tmp8B6);_tmp8BD;});}*
_tmp8B9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct
_tuple15*v))Cyc_Dict_insert)(*_tmp8B9,_tmp8B6,v);}}else{struct _tuple15 _tmp8BF;
struct Cyc_Absyn_Aggrdecl*_tmp8C0;void*_tmp8C1;struct _tuple15*_tmp8BE=*_tmp8BB;
_tmp8BF=*_tmp8BE;_tmp8C0=_tmp8BF.f1;_tmp8C1=_tmp8BF.f2;if(_tmp8C0->impl == 0){*
_tmp8B9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct
_tuple15*v))Cyc_Dict_insert)(*_tmp8B9,_tmp8B6,({struct _tuple15*_tmp8C2=
_region_malloc(d,sizeof(*_tmp8C2));_tmp8C2->f1=ad;_tmp8C2->f2=_tmp8C1;_tmp8C2;}));
seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*new_ad=({
struct Cyc_Absyn_Aggrdecl*_tmp8E1=_cycalloc(sizeof(*_tmp8E1));_tmp8E1->kind=ad->kind;
_tmp8E1->sc=Cyc_Absyn_Public;_tmp8E1->name=ad->name;_tmp8E1->tvs=0;_tmp8E1->impl=
0;_tmp8E1->attributes=ad->attributes;_tmp8E1;});if(ad->impl != 0  && !
seen_defn_before){new_ad->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp8C3=_cycalloc(
sizeof(*_tmp8C3));_tmp8C3->exist_vars=0;_tmp8C3->rgn_po=0;_tmp8C3->fields=0;
_tmp8C3->tagged=0;_tmp8C3;});{struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*
_tmp8C4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp8C4 != 0;_tmp8C4=_tmp8C4->tl){struct Cyc_Absyn_Aggrfield*_tmp8C5=(struct Cyc_Absyn_Aggrfield*)
_tmp8C4->hd;struct Cyc_Absyn_Aggrfield*_tmp8C6=({struct Cyc_Absyn_Aggrfield*
_tmp8DC=_cycalloc(sizeof(*_tmp8DC));_tmp8DC->name=_tmp8C5->name;_tmp8DC->tq=Cyc_Toc_mt_tq;
_tmp8DC->type=Cyc_Toc_typ_to_c_array(_tmp8C5->type);_tmp8DC->width=_tmp8C5->width;
_tmp8DC->attributes=_tmp8C5->attributes;_tmp8DC;});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->tagged){void*_tmp8C7=_tmp8C6->type;struct _dyneither_ptr*
_tmp8C8=_tmp8C6->name;struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp8DA;_tmp8DA.tag=0;_tmp8DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp8C8);{struct Cyc_String_pa_struct _tmp8D9;_tmp8D9.tag=0;_tmp8D9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2);{void*_tmp8D7[2]={&
_tmp8D9,& _tmp8DA};Cyc_aprintf(({const char*_tmp8D8="_union_%s_%s";_tag_dyneither(
_tmp8D8,sizeof(char),13);}),_tag_dyneither(_tmp8D7,sizeof(void*),2));}}});struct
_dyneither_ptr*str=({struct _dyneither_ptr*_tmp8D6=_cycalloc(sizeof(*_tmp8D6));
_tmp8D6[0]=s;_tmp8D6;});struct Cyc_Absyn_Aggrfield*_tmp8C9=({struct Cyc_Absyn_Aggrfield*
_tmp8D5=_cycalloc(sizeof(*_tmp8D5));_tmp8D5->name=Cyc_Toc_val_sp;_tmp8D5->tq=Cyc_Toc_mt_tq;
_tmp8D5->type=_tmp8C7;_tmp8D5->width=0;_tmp8D5->attributes=0;_tmp8D5;});struct
Cyc_Absyn_Aggrfield*_tmp8CA=({struct Cyc_Absyn_Aggrfield*_tmp8D4=_cycalloc(
sizeof(*_tmp8D4));_tmp8D4->name=Cyc_Toc_tag_sp;_tmp8D4->tq=Cyc_Toc_mt_tq;_tmp8D4->type=
Cyc_Absyn_sint_typ;_tmp8D4->width=0;_tmp8D4->attributes=0;_tmp8D4;});struct Cyc_List_List*
_tmp8CB=({struct Cyc_Absyn_Aggrfield*_tmp8D3[2];_tmp8D3[1]=_tmp8C9;_tmp8D3[0]=
_tmp8CA;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp8D3,sizeof(struct Cyc_Absyn_Aggrfield*),2));});struct Cyc_Absyn_Aggrdecl*
_tmp8CC=({struct Cyc_Absyn_Aggrdecl*_tmp8D0=_cycalloc(sizeof(*_tmp8D0));_tmp8D0->kind=
Cyc_Absyn_StructA;_tmp8D0->sc=Cyc_Absyn_Public;_tmp8D0->name=({struct _tuple1*
_tmp8D2=_cycalloc(sizeof(*_tmp8D2));_tmp8D2->f1=Cyc_Absyn_Loc_n;_tmp8D2->f2=str;
_tmp8D2;});_tmp8D0->tvs=0;_tmp8D0->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp8D1=
_cycalloc(sizeof(*_tmp8D1));_tmp8D1->exist_vars=0;_tmp8D1->rgn_po=0;_tmp8D1->fields=
_tmp8CB;_tmp8D1->tagged=0;_tmp8D1;});_tmp8D0->attributes=0;_tmp8D0;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp8CD=_cycalloc(sizeof(*_tmp8CD));_tmp8CD->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp8CE=_cycalloc(sizeof(*_tmp8CE));
_tmp8CE[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp8CF;_tmp8CF.tag=6;_tmp8CF.f1=
_tmp8CC;_tmp8CF;});_tmp8CE;}),0);_tmp8CD->tl=Cyc_Toc_result_decls;_tmp8CD;});
_tmp8C6->type=Cyc_Absyn_strct(str);}new_fields=({struct Cyc_List_List*_tmp8DB=
_cycalloc(sizeof(*_tmp8DB));_tmp8DB->hd=_tmp8C6;_tmp8DB->tl=new_fields;_tmp8DB;});}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);}}Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8DD=_cycalloc(
sizeof(*_tmp8DD));_tmp8DD->hd=({struct Cyc_Absyn_Decl*_tmp8DE=_cycalloc(sizeof(*
_tmp8DE));_tmp8DE->r=(void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp8DF=_cycalloc(
sizeof(*_tmp8DF));_tmp8DF[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp8E0;_tmp8E0.tag=
6;_tmp8E0.f1=new_ad;_tmp8E0;});_tmp8DF;});_tmp8DE->loc=0;_tmp8DE;});_tmp8DD->tl=
Cyc_Toc_result_decls;_tmp8DD;});}};_pop_dynregion(d);}}static void Cyc_Toc_datatypedecl_to_c(
struct Cyc_Absyn_Datatypedecl*tud){struct _DynRegionHandle*_tmp8E3;struct Cyc_Set_Set**
_tmp8E4;struct Cyc_Toc_TocState _tmp8E2=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8E3=_tmp8E2.dyn;_tmp8E4=_tmp8E2.datatypes_so_far;{struct _DynRegionFrame
_tmp8E5;struct _RegionHandle*d=_open_dynregion(& _tmp8E5,_tmp8E3);{struct _tuple1*
_tmp8E6=tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct
_tuple1*elt))Cyc_Set_member)(*_tmp8E4,_tmp8E6)){_npop_handler(0);return;}*
_tmp8E4=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct
_tuple1*elt))Cyc_Set_rinsert)(d,*_tmp8E4,_tmp8E6);}{struct Cyc_List_List*_tmp8E7=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;
_tmp8E7 != 0;_tmp8E7=_tmp8E7->tl){struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)
_tmp8E7->hd;struct Cyc_List_List*_tmp8E8=0;int i=1;{struct Cyc_List_List*_tmp8E9=f->typs;
for(0;_tmp8E9 != 0;(_tmp8E9=_tmp8E9->tl,i ++)){struct _dyneither_ptr*_tmp8EA=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Aggrfield*_tmp8EB=({struct Cyc_Absyn_Aggrfield*_tmp8ED=
_cycalloc(sizeof(*_tmp8ED));_tmp8ED->name=_tmp8EA;_tmp8ED->tq=(*((struct _tuple10*)
_tmp8E9->hd)).f1;_tmp8ED->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)
_tmp8E9->hd)).f2);_tmp8ED->width=0;_tmp8ED->attributes=0;_tmp8ED;});_tmp8E8=({
struct Cyc_List_List*_tmp8EC=_cycalloc(sizeof(*_tmp8EC));_tmp8EC->hd=_tmp8EB;
_tmp8EC->tl=_tmp8E8;_tmp8EC;});}}_tmp8E8=({struct Cyc_List_List*_tmp8EE=_cycalloc(
sizeof(*_tmp8EE));_tmp8EE->hd=({struct Cyc_Absyn_Aggrfield*_tmp8EF=_cycalloc(
sizeof(*_tmp8EF));_tmp8EF->name=Cyc_Toc_tag_sp;_tmp8EF->tq=Cyc_Toc_mt_tq;_tmp8EF->type=
Cyc_Absyn_sint_typ;_tmp8EF->width=0;_tmp8EF->attributes=0;_tmp8EF;});_tmp8EE->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8E8);
_tmp8EE;});{struct Cyc_Absyn_Aggrdecl*_tmp8F0=({struct Cyc_Absyn_Aggrdecl*_tmp8F4=
_cycalloc(sizeof(*_tmp8F4));_tmp8F4->kind=Cyc_Absyn_StructA;_tmp8F4->sc=Cyc_Absyn_Public;
_tmp8F4->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*_tmp8F6="_struct";
_tag_dyneither(_tmp8F6,sizeof(char),8);}));_tmp8F4->tvs=0;_tmp8F4->impl=({struct
Cyc_Absyn_AggrdeclImpl*_tmp8F5=_cycalloc(sizeof(*_tmp8F5));_tmp8F5->exist_vars=0;
_tmp8F5->rgn_po=0;_tmp8F5->fields=_tmp8E8;_tmp8F5->tagged=0;_tmp8F5;});_tmp8F4->attributes=
0;_tmp8F4;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp8F1=_cycalloc(
sizeof(*_tmp8F1));_tmp8F1->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp8F2=_cycalloc(sizeof(*_tmp8F2));_tmp8F2[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp8F3;_tmp8F3.tag=6;_tmp8F3.f1=_tmp8F0;_tmp8F3;});_tmp8F2;}),0);_tmp8F1->tl=
Cyc_Toc_result_decls;_tmp8F1;});}}};_pop_dynregion(d);}}static void Cyc_Toc_xdatatypedecl_to_c(
struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields == 0)return;{struct
_DynRegionHandle*_tmp8F8;struct Cyc_Dict_Dict*_tmp8F9;struct Cyc_Toc_TocState
_tmp8F7=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8F8=
_tmp8F7.dyn;_tmp8F9=_tmp8F7.xdatatypes_so_far;{struct _DynRegionFrame _tmp8FA;
struct _RegionHandle*d=_open_dynregion(& _tmp8FA,_tmp8F8);{struct _tuple1*_tmp8FB=
xd->name;struct Cyc_List_List*_tmp8FC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp8FC != 0;_tmp8FC=_tmp8FC->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8FC->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8FD=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp8FE=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp8FD,Cyc_Absyn_false_conref,0);int*_tmp8FF=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8F9,f->name);int
_tmp900;_LL3A0: if(_tmp8FF != 0)goto _LL3A2;_LL3A1: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp901=Cyc_Absyn_new_vardecl(f->name,_tmp8FE,
initopt);_tmp901->sc=f->sc;Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp902=
_cycalloc(sizeof(*_tmp902));_tmp902->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp903=_cycalloc(sizeof(*_tmp903));_tmp903[0]=({struct Cyc_Absyn_Var_d_struct
_tmp904;_tmp904.tag=0;_tmp904.f1=_tmp901;_tmp904;});_tmp903;}),0);_tmp902->tl=
Cyc_Toc_result_decls;_tmp902;});*_tmp8F9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp8F9,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*_tmp905=f->typs;for(0;
_tmp905 != 0;(_tmp905=_tmp905->tl,i ++)){struct _dyneither_ptr*_tmp906=({struct
_dyneither_ptr*_tmp90A=_cycalloc(sizeof(*_tmp90A));_tmp90A[0]=(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp90D;_tmp90D.tag=1;_tmp90D.f1=(
unsigned long)i;{void*_tmp90B[1]={& _tmp90D};Cyc_aprintf(({const char*_tmp90C="f%d";
_tag_dyneither(_tmp90C,sizeof(char),4);}),_tag_dyneither(_tmp90B,sizeof(void*),1));}});
_tmp90A;});struct Cyc_Absyn_Aggrfield*_tmp907=({struct Cyc_Absyn_Aggrfield*_tmp909=
_cycalloc(sizeof(*_tmp909));_tmp909->name=_tmp906;_tmp909->tq=(*((struct _tuple10*)
_tmp905->hd)).f1;_tmp909->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)
_tmp905->hd)).f2);_tmp909->width=0;_tmp909->attributes=0;_tmp909;});fields=({
struct Cyc_List_List*_tmp908=_cycalloc(sizeof(*_tmp908));_tmp908->hd=_tmp907;
_tmp908->tl=fields;_tmp908;});}}fields=({struct Cyc_List_List*_tmp90E=_cycalloc(
sizeof(*_tmp90E));_tmp90E->hd=({struct Cyc_Absyn_Aggrfield*_tmp90F=_cycalloc(
sizeof(*_tmp90F));_tmp90F->name=Cyc_Toc_tag_sp;_tmp90F->tq=Cyc_Toc_mt_tq;_tmp90F->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp90F->width=0;_tmp90F->attributes=
0;_tmp90F;});_tmp90E->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp90E;});{struct Cyc_Absyn_Aggrdecl*_tmp910=({struct Cyc_Absyn_Aggrdecl*
_tmp914=_cycalloc(sizeof(*_tmp914));_tmp914->kind=Cyc_Absyn_StructA;_tmp914->sc=
Cyc_Absyn_Public;_tmp914->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp916="_struct";_tag_dyneither(_tmp916,sizeof(char),8);}));_tmp914->tvs=0;
_tmp914->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp915=_cycalloc(sizeof(*_tmp915));
_tmp915->exist_vars=0;_tmp915->rgn_po=0;_tmp915->fields=fields;_tmp915->tagged=0;
_tmp915;});_tmp914->attributes=0;_tmp914;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp911=_cycalloc(sizeof(*_tmp911));_tmp911->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp912=_cycalloc(sizeof(*_tmp912));_tmp912[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp913;_tmp913.tag=6;_tmp913.f1=_tmp910;_tmp913;});
_tmp912;}),0);_tmp911->tl=Cyc_Toc_result_decls;_tmp911;});goto _LL39F;}}}}_LL3A2:
if(_tmp8FF == 0)goto _LL3A4;_tmp900=*_tmp8FF;if(_tmp900 != 0)goto _LL3A4;_LL3A3: if(f->sc
!= Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp917=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp918=Cyc_Absyn_new_vardecl(f->name,_tmp8FE,(struct Cyc_Absyn_Exp*)
_tmp917);_tmp918->sc=f->sc;Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp919=
_cycalloc(sizeof(*_tmp919));_tmp919->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp91A=_cycalloc(sizeof(*_tmp91A));_tmp91A[0]=({struct Cyc_Absyn_Var_d_struct
_tmp91B;_tmp91B.tag=0;_tmp91B.f1=_tmp918;_tmp91B;});_tmp91A;}),0);_tmp919->tl=
Cyc_Toc_result_decls;_tmp919;});*_tmp8F9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp8F9,f->name,1);}goto _LL39F;_LL3A4:;
_LL3A5: goto _LL39F;_LL39F:;}};_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){ed->sc=Cyc_Absyn_Public;if(ed->fields
!= 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(old_typ);if(vd->sc == 
Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(
struct Cyc_Absyn_Exp*)_check_null(vd->initializer);void*_tmp91C=init->r;struct Cyc_Absyn_Vardecl*
_tmp91E;struct Cyc_Absyn_Exp*_tmp91F;struct Cyc_Absyn_Exp*_tmp920;int _tmp921;
_LL3A7: {struct Cyc_Absyn_Comprehension_e_struct*_tmp91D=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp91C;if(_tmp91D->tag != 29)goto _LL3A9;else{_tmp91E=_tmp91D->f1;_tmp91F=_tmp91D->f2;
_tmp920=_tmp91D->f3;_tmp921=_tmp91D->f4;}}_LL3A8: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp91E,_tmp91F,_tmp920,_tmp921,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3A6;_LL3A9:;_LL3AA: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp923;struct _RegionHandle*_tmp924;struct Cyc_Toc_Env*_tmp922=init_nv;_tmp923=*
_tmp922;_tmp924=_tmp923.rgn;{struct Cyc_Toc_Env*_tmp925=Cyc_Toc_set_toplevel(
_tmp924,init_nv);Cyc_Toc_exp_to_c(_tmp925,init);}}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3A6;_LL3A6:;}else{void*_tmp926=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp928;void*_tmp929;struct Cyc_Absyn_Exp*_tmp92A;union Cyc_Absyn_Constraint*
_tmp92B;_LL3AC:{struct Cyc_Absyn_ArrayType_struct*_tmp927=(struct Cyc_Absyn_ArrayType_struct*)
_tmp926;if(_tmp927->tag != 9)goto _LL3AE;else{_tmp928=_tmp927->f1;_tmp929=_tmp928.elt_type;
_tmp92A=_tmp928.num_elts;_tmp92B=_tmp928.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp92B))goto _LL3AE;_LL3AD: if(_tmp92A == 0)({void*
_tmp92C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp92D="can't initialize zero-terminated array -- size unknown";
_tag_dyneither(_tmp92D,sizeof(char),55);}),_tag_dyneither(_tmp92C,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp92A;struct Cyc_Absyn_Exp*
_tmp92E=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp92F=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp92E,
_tmp92F,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3AB;}_LL3AE:;_LL3AF: goto _LL3AB;
_LL3AB:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple1*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp931;struct
_RegionHandle*_tmp932;struct Cyc_Toc_Env*_tmp930=nv;_tmp931=*_tmp930;_tmp932=
_tmp931.rgn;{struct Cyc_Absyn_Stmt*_tmp933=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp934=Cyc_Toc_share_env(_tmp932,nv);struct Cyc_Toc_Env*_tmp936;struct Cyc_List_List*
_tmp937;struct Cyc_Absyn_Stmt*_tmp938;struct _tuple21 _tmp935=Cyc_Toc_xlate_pat(
_tmp934,_tmp932,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp936=_tmp935.f1;_tmp937=_tmp935.f2;_tmp938=_tmp935.f3;Cyc_Toc_stmt_to_c(
_tmp936,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp938,s,0),0);for(0;_tmp937 != 0;_tmp937=_tmp937->tl){
struct _tuple22 _tmp93A;struct _tuple1*_tmp93B;void*_tmp93C;struct _tuple22*_tmp939=(
struct _tuple22*)_tmp937->hd;_tmp93A=*_tmp939;_tmp93B=_tmp93A.f1;_tmp93C=_tmp93A.f2;
s=Cyc_Absyn_declare_stmt(_tmp93B,_tmp93C,0,s,0);}}return s;}}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e){void*_tmp93D=e->r;struct Cyc_Absyn_Exp*_tmp93F;struct Cyc_Absyn_Exp*
_tmp941;struct Cyc_Absyn_Exp*_tmp943;struct Cyc_Absyn_Exp*_tmp945;struct Cyc_Absyn_Exp*
_tmp947;struct Cyc_Absyn_Exp*_tmp949;struct Cyc_Absyn_Exp*_tmp94B;struct Cyc_Absyn_Exp*
_tmp94D;struct Cyc_List_List*_tmp94F;struct Cyc_Absyn_Exp*_tmp951;struct Cyc_Absyn_Exp*
_tmp952;struct Cyc_Absyn_Exp*_tmp954;struct Cyc_Absyn_Exp*_tmp955;struct Cyc_Absyn_Exp*
_tmp957;struct Cyc_Absyn_Exp*_tmp958;struct Cyc_Absyn_Exp*_tmp95A;struct Cyc_Absyn_Exp*
_tmp95B;struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_Absyn_Exp*_tmp95E;struct Cyc_Absyn_Exp*
_tmp960;struct Cyc_Absyn_Exp*_tmp961;struct Cyc_Absyn_Exp*_tmp963;struct Cyc_Absyn_Exp*
_tmp964;struct Cyc_Absyn_Exp*_tmp965;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_List_List*
_tmp968;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_List_List*_tmp96B;void*_tmp96D;
void**_tmp96E;struct Cyc_Absyn_Exp*_tmp96F;struct _tuple8*_tmp971;struct _tuple8
_tmp972;void*_tmp973;void**_tmp974;struct Cyc_List_List*_tmp975;struct Cyc_List_List*
_tmp977;struct Cyc_List_List*_tmp979;void*_tmp97B;void**_tmp97C;void*_tmp97E;void**
_tmp97F;struct Cyc_Absyn_Stmt*_tmp981;struct Cyc_Absyn_MallocInfo _tmp983;struct Cyc_Absyn_MallocInfo*
_tmp984;_LL3B1: {struct Cyc_Absyn_Deref_e_struct*_tmp93E=(struct Cyc_Absyn_Deref_e_struct*)
_tmp93D;if(_tmp93E->tag != 22)goto _LL3B3;else{_tmp93F=_tmp93E->f1;}}_LL3B2:
_tmp941=_tmp93F;goto _LL3B4;_LL3B3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp940=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp93D;if(_tmp940->tag != 23)goto _LL3B5;
else{_tmp941=_tmp940->f1;}}_LL3B4: _tmp943=_tmp941;goto _LL3B6;_LL3B5: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp942=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp93D;if(_tmp942->tag != 24)goto
_LL3B7;else{_tmp943=_tmp942->f1;}}_LL3B6: _tmp945=_tmp943;goto _LL3B8;_LL3B7: {
struct Cyc_Absyn_Address_e_struct*_tmp944=(struct Cyc_Absyn_Address_e_struct*)
_tmp93D;if(_tmp944->tag != 16)goto _LL3B9;else{_tmp945=_tmp944->f1;}}_LL3B8:
_tmp947=_tmp945;goto _LL3BA;_LL3B9: {struct Cyc_Absyn_Throw_e_struct*_tmp946=(
struct Cyc_Absyn_Throw_e_struct*)_tmp93D;if(_tmp946->tag != 12)goto _LL3BB;else{
_tmp947=_tmp946->f1;}}_LL3BA: _tmp949=_tmp947;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp948=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp93D;if(_tmp948->tag != 13)
goto _LL3BD;else{_tmp949=_tmp948->f1;}}_LL3BC: _tmp94B=_tmp949;goto _LL3BE;_LL3BD: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp94A=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp93D;if(_tmp94A->tag != 19)goto _LL3BF;else{_tmp94B=_tmp94A->f1;}}_LL3BE:
_tmp94D=_tmp94B;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_Increment_e_struct*_tmp94C=(
struct Cyc_Absyn_Increment_e_struct*)_tmp93D;if(_tmp94C->tag != 5)goto _LL3C1;else{
_tmp94D=_tmp94C->f1;}}_LL3C0: Cyc_Toc_exptypes_to_c(_tmp94D);goto _LL3B0;_LL3C1: {
struct Cyc_Absyn_Primop_e_struct*_tmp94E=(struct Cyc_Absyn_Primop_e_struct*)
_tmp93D;if(_tmp94E->tag != 3)goto _LL3C3;else{_tmp94F=_tmp94E->f2;}}_LL3C2:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp94F);goto _LL3B0;_LL3C3: {struct Cyc_Absyn_And_e_struct*_tmp950=(struct Cyc_Absyn_And_e_struct*)
_tmp93D;if(_tmp950->tag != 7)goto _LL3C5;else{_tmp951=_tmp950->f1;_tmp952=_tmp950->f2;}}
_LL3C4: _tmp954=_tmp951;_tmp955=_tmp952;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Or_e_struct*
_tmp953=(struct Cyc_Absyn_Or_e_struct*)_tmp93D;if(_tmp953->tag != 8)goto _LL3C7;
else{_tmp954=_tmp953->f1;_tmp955=_tmp953->f2;}}_LL3C6: _tmp957=_tmp954;_tmp958=
_tmp955;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_SeqExp_e_struct*_tmp956=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp93D;if(_tmp956->tag != 9)goto _LL3C9;else{_tmp957=_tmp956->f1;_tmp958=_tmp956->f2;}}
_LL3C8: _tmp95A=_tmp957;_tmp95B=_tmp958;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Subscript_e_struct*
_tmp959=(struct Cyc_Absyn_Subscript_e_struct*)_tmp93D;if(_tmp959->tag != 25)goto
_LL3CB;else{_tmp95A=_tmp959->f1;_tmp95B=_tmp959->f2;}}_LL3CA: _tmp95D=_tmp95A;
_tmp95E=_tmp95B;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Swap_e_struct*_tmp95C=(
struct Cyc_Absyn_Swap_e_struct*)_tmp93D;if(_tmp95C->tag != 36)goto _LL3CD;else{
_tmp95D=_tmp95C->f1;_tmp95E=_tmp95C->f2;}}_LL3CC: _tmp960=_tmp95D;_tmp961=_tmp95E;
goto _LL3CE;_LL3CD: {struct Cyc_Absyn_AssignOp_e_struct*_tmp95F=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp93D;if(_tmp95F->tag != 4)goto _LL3CF;else{_tmp960=_tmp95F->f1;_tmp961=_tmp95F->f3;}}
_LL3CE: Cyc_Toc_exptypes_to_c(_tmp960);Cyc_Toc_exptypes_to_c(_tmp961);goto _LL3B0;
_LL3CF: {struct Cyc_Absyn_Conditional_e_struct*_tmp962=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp93D;if(_tmp962->tag != 6)goto _LL3D1;else{_tmp963=_tmp962->f1;_tmp964=_tmp962->f2;
_tmp965=_tmp962->f3;}}_LL3D0: Cyc_Toc_exptypes_to_c(_tmp963);Cyc_Toc_exptypes_to_c(
_tmp964);Cyc_Toc_exptypes_to_c(_tmp965);goto _LL3B0;_LL3D1: {struct Cyc_Absyn_FnCall_e_struct*
_tmp966=(struct Cyc_Absyn_FnCall_e_struct*)_tmp93D;if(_tmp966->tag != 11)goto
_LL3D3;else{_tmp967=_tmp966->f1;_tmp968=_tmp966->f2;}}_LL3D2: _tmp96A=_tmp967;
_tmp96B=_tmp968;goto _LL3D4;_LL3D3: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp969=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp93D;if(_tmp969->tag != 10)goto _LL3D5;
else{_tmp96A=_tmp969->f1;_tmp96B=_tmp969->f2;}}_LL3D4: Cyc_Toc_exptypes_to_c(
_tmp96A);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp96B);goto _LL3B0;_LL3D5: {struct Cyc_Absyn_Cast_e_struct*
_tmp96C=(struct Cyc_Absyn_Cast_e_struct*)_tmp93D;if(_tmp96C->tag != 15)goto _LL3D7;
else{_tmp96D=(void**)& _tmp96C->f1;_tmp96E=(void**)((void**)& _tmp96C->f1);_tmp96F=
_tmp96C->f2;}}_LL3D6:*_tmp96E=Cyc_Toc_typ_to_c(*_tmp96E);Cyc_Toc_exptypes_to_c(
_tmp96F);goto _LL3B0;_LL3D7: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp970=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp93D;if(_tmp970->tag != 27)goto _LL3D9;
else{_tmp971=_tmp970->f1;_tmp972=*_tmp971;_tmp973=_tmp972.f3;_tmp974=(void**)&(*
_tmp970->f1).f3;_tmp975=_tmp970->f2;}}_LL3D8:*_tmp974=Cyc_Toc_typ_to_c(*_tmp974);
_tmp977=_tmp975;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp976=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp93D;if(_tmp976->tag != 37)
goto _LL3DB;else{_tmp977=_tmp976->f2;}}_LL3DA: _tmp979=_tmp977;goto _LL3DC;_LL3DB: {
struct Cyc_Absyn_Array_e_struct*_tmp978=(struct Cyc_Absyn_Array_e_struct*)_tmp93D;
if(_tmp978->tag != 28)goto _LL3DD;else{_tmp979=_tmp978->f1;}}_LL3DC: for(0;_tmp979
!= 0;_tmp979=_tmp979->tl){struct Cyc_Absyn_Exp*_tmp995;struct _tuple16 _tmp994=*((
struct _tuple16*)_tmp979->hd);_tmp995=_tmp994.f2;Cyc_Toc_exptypes_to_c(_tmp995);}
goto _LL3B0;_LL3DD: {struct Cyc_Absyn_Offsetof_e_struct*_tmp97A=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp93D;if(_tmp97A->tag != 20)goto _LL3DF;else{_tmp97B=(void**)& _tmp97A->f1;
_tmp97C=(void**)((void**)& _tmp97A->f1);}}_LL3DE: _tmp97F=_tmp97C;goto _LL3E0;
_LL3DF: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp97D=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp93D;if(_tmp97D->tag != 18)goto _LL3E1;else{_tmp97E=(void**)& _tmp97D->f1;
_tmp97F=(void**)((void**)& _tmp97D->f1);}}_LL3E0:*_tmp97F=Cyc_Toc_typ_to_c(*
_tmp97F);goto _LL3B0;_LL3E1: {struct Cyc_Absyn_StmtExp_e_struct*_tmp980=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp93D;if(_tmp980->tag != 38)goto _LL3E3;else{_tmp981=_tmp980->f1;}}_LL3E2: Cyc_Toc_stmttypes_to_c(
_tmp981);goto _LL3B0;_LL3E3: {struct Cyc_Absyn_Malloc_e_struct*_tmp982=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp93D;if(_tmp982->tag != 35)goto _LL3E5;else{_tmp983=_tmp982->f1;_tmp984=(struct
Cyc_Absyn_MallocInfo*)& _tmp982->f1;}}_LL3E4: if(_tmp984->elt_type != 0)_tmp984->elt_type=({
void**_tmp996=_cycalloc(sizeof(*_tmp996));_tmp996[0]=Cyc_Toc_typ_to_c(*((void**)
_check_null(_tmp984->elt_type)));_tmp996;});Cyc_Toc_exptypes_to_c(_tmp984->num_elts);
goto _LL3B0;_LL3E5: {struct Cyc_Absyn_Const_e_struct*_tmp985=(struct Cyc_Absyn_Const_e_struct*)
_tmp93D;if(_tmp985->tag != 0)goto _LL3E7;}_LL3E6: goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Var_e_struct*
_tmp986=(struct Cyc_Absyn_Var_e_struct*)_tmp93D;if(_tmp986->tag != 1)goto _LL3E9;}
_LL3E8: goto _LL3EA;_LL3E9: {struct Cyc_Absyn_UnknownId_e_struct*_tmp987=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp93D;if(_tmp987->tag != 2)goto _LL3EB;}_LL3EA: goto _LL3EC;_LL3EB: {struct Cyc_Absyn_Enum_e_struct*
_tmp988=(struct Cyc_Absyn_Enum_e_struct*)_tmp93D;if(_tmp988->tag != 33)goto _LL3ED;}
_LL3EC: goto _LL3EE;_LL3ED: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp989=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp93D;if(_tmp989->tag != 34)goto _LL3EF;}_LL3EE: goto _LL3B0;_LL3EF: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp98A=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp93D;if(_tmp98A->tag != 31)goto
_LL3F1;}_LL3F0: goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Datatype_e_struct*_tmp98B=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp93D;if(_tmp98B->tag != 32)goto _LL3F3;}
_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Aggregate_e_struct*_tmp98C=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp93D;if(_tmp98C->tag != 30)goto _LL3F5;}_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp98D=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp93D;if(_tmp98D->tag != 29)
goto _LL3F7;}_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Tuple_e_struct*_tmp98E=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp93D;if(_tmp98E->tag != 26)goto _LL3F9;}_LL3F8:
goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Instantiate_e_struct*_tmp98F=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp93D;if(_tmp98F->tag != 14)goto _LL3FB;}_LL3FA: goto _LL3FC;_LL3FB: {struct Cyc_Absyn_New_e_struct*
_tmp990=(struct Cyc_Absyn_New_e_struct*)_tmp93D;if(_tmp990->tag != 17)goto _LL3FD;}
_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Valueof_e_struct*_tmp991=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp93D;if(_tmp991->tag != 40)goto _LL3FF;}_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp992=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp93D;if(_tmp992->tag != 39)goto
_LL401;}_LL400: goto _LL402;_LL401: {struct Cyc_Absyn_Gentyp_e_struct*_tmp993=(
struct Cyc_Absyn_Gentyp_e_struct*)_tmp93D;if(_tmp993->tag != 21)goto _LL3B0;}_LL402:({
void*_tmp997=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp998="Cyclone expression within C code";
_tag_dyneither(_tmp998,sizeof(char),33);}),_tag_dyneither(_tmp997,sizeof(void*),
0));});goto _LL3B0;_LL3B0:;}static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*
d){void*_tmp999=d->r;struct Cyc_Absyn_Vardecl*_tmp99B;struct Cyc_Absyn_Fndecl*
_tmp99D;struct Cyc_Absyn_Aggrdecl*_tmp99F;struct Cyc_Absyn_Enumdecl*_tmp9A1;struct
Cyc_Absyn_Typedefdecl*_tmp9A3;_LL404: {struct Cyc_Absyn_Var_d_struct*_tmp99A=(
struct Cyc_Absyn_Var_d_struct*)_tmp999;if(_tmp99A->tag != 0)goto _LL406;else{
_tmp99B=_tmp99A->f1;}}_LL405: _tmp99B->type=Cyc_Toc_typ_to_c(_tmp99B->type);if(
_tmp99B->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(
_tmp99B->initializer));goto _LL403;_LL406: {struct Cyc_Absyn_Fn_d_struct*_tmp99C=(
struct Cyc_Absyn_Fn_d_struct*)_tmp999;if(_tmp99C->tag != 1)goto _LL408;else{_tmp99D=
_tmp99C->f1;}}_LL407: _tmp99D->ret_type=Cyc_Toc_typ_to_c(_tmp99D->ret_type);{
struct Cyc_List_List*_tmp9AF=_tmp99D->args;for(0;_tmp9AF != 0;_tmp9AF=_tmp9AF->tl){(*((
struct _tuple25*)_tmp9AF->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp9AF->hd)).f3);}}
goto _LL403;_LL408: {struct Cyc_Absyn_Aggr_d_struct*_tmp99E=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp999;if(_tmp99E->tag != 6)goto _LL40A;else{_tmp99F=_tmp99E->f1;}}_LL409: Cyc_Toc_aggrdecl_to_c(
_tmp99F);goto _LL403;_LL40A: {struct Cyc_Absyn_Enum_d_struct*_tmp9A0=(struct Cyc_Absyn_Enum_d_struct*)
_tmp999;if(_tmp9A0->tag != 8)goto _LL40C;else{_tmp9A1=_tmp9A0->f1;}}_LL40B: if(
_tmp9A1->fields != 0){struct Cyc_List_List*_tmp9B0=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9A1->fields))->v;for(0;_tmp9B0 != 0;_tmp9B0=_tmp9B0->tl){
struct Cyc_Absyn_Enumfield*_tmp9B1=(struct Cyc_Absyn_Enumfield*)_tmp9B0->hd;if(
_tmp9B1->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9B1->tag));}}
goto _LL403;_LL40C: {struct Cyc_Absyn_Typedef_d_struct*_tmp9A2=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp999;if(_tmp9A2->tag != 9)goto _LL40E;else{_tmp9A3=_tmp9A2->f1;}}_LL40D: _tmp9A3->defn=({
struct Cyc_Core_Opt*_tmp9B2=_cycalloc(sizeof(*_tmp9B2));_tmp9B2->v=Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp9A3->defn))->v);_tmp9B2;});goto
_LL403;_LL40E: {struct Cyc_Absyn_Let_d_struct*_tmp9A4=(struct Cyc_Absyn_Let_d_struct*)
_tmp999;if(_tmp9A4->tag != 2)goto _LL410;}_LL40F: goto _LL411;_LL410: {struct Cyc_Absyn_Letv_d_struct*
_tmp9A5=(struct Cyc_Absyn_Letv_d_struct*)_tmp999;if(_tmp9A5->tag != 3)goto _LL412;}
_LL411: goto _LL413;_LL412: {struct Cyc_Absyn_Datatype_d_struct*_tmp9A6=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp999;if(_tmp9A6->tag != 7)goto _LL414;}_LL413: goto _LL415;_LL414: {struct Cyc_Absyn_Namespace_d_struct*
_tmp9A7=(struct Cyc_Absyn_Namespace_d_struct*)_tmp999;if(_tmp9A7->tag != 10)goto
_LL416;}_LL415: goto _LL417;_LL416: {struct Cyc_Absyn_Using_d_struct*_tmp9A8=(
struct Cyc_Absyn_Using_d_struct*)_tmp999;if(_tmp9A8->tag != 11)goto _LL418;}_LL417:
goto _LL419;_LL418: {struct Cyc_Absyn_ExternC_d_struct*_tmp9A9=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp999;if(_tmp9A9->tag != 12)goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9AA=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp999;if(_tmp9AA->tag != 13)
goto _LL41C;}_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_Region_d_struct*_tmp9AB=(
struct Cyc_Absyn_Region_d_struct*)_tmp999;if(_tmp9AB->tag != 4)goto _LL41E;}_LL41D:
goto _LL41F;_LL41E: {struct Cyc_Absyn_Alias_d_struct*_tmp9AC=(struct Cyc_Absyn_Alias_d_struct*)
_tmp999;if(_tmp9AC->tag != 5)goto _LL420;}_LL41F:({void*_tmp9B3=0;Cyc_Tcutil_terr(
d->loc,({const char*_tmp9B4="Cyclone declaration within C code";_tag_dyneither(
_tmp9B4,sizeof(char),34);}),_tag_dyneither(_tmp9B3,sizeof(void*),0));});goto
_LL403;_LL420: {struct Cyc_Absyn_Porton_d_struct*_tmp9AD=(struct Cyc_Absyn_Porton_d_struct*)
_tmp999;if(_tmp9AD->tag != 14)goto _LL422;}_LL421: goto _LL423;_LL422: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9AE=(struct Cyc_Absyn_Portoff_d_struct*)_tmp999;if(_tmp9AE->tag != 15)goto
_LL403;}_LL423: goto _LL403;_LL403:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmp9B5=s->r;struct Cyc_Absyn_Exp*_tmp9B7;struct Cyc_Absyn_Stmt*_tmp9B9;
struct Cyc_Absyn_Stmt*_tmp9BA;struct Cyc_Absyn_Exp*_tmp9BC;struct Cyc_Absyn_Exp*
_tmp9BE;struct Cyc_Absyn_Stmt*_tmp9BF;struct Cyc_Absyn_Stmt*_tmp9C0;struct _tuple9
_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C3;struct Cyc_Absyn_Stmt*_tmp9C4;struct Cyc_Absyn_Exp*
_tmp9C6;struct _tuple9 _tmp9C7;struct Cyc_Absyn_Exp*_tmp9C8;struct _tuple9 _tmp9C9;
struct Cyc_Absyn_Exp*_tmp9CA;struct Cyc_Absyn_Stmt*_tmp9CB;struct Cyc_Absyn_Exp*
_tmp9CD;struct Cyc_List_List*_tmp9CE;struct Cyc_Absyn_Decl*_tmp9D0;struct Cyc_Absyn_Stmt*
_tmp9D1;struct Cyc_Absyn_Stmt*_tmp9D3;struct _tuple9 _tmp9D4;struct Cyc_Absyn_Exp*
_tmp9D5;_LL425: {struct Cyc_Absyn_Exp_s_struct*_tmp9B6=(struct Cyc_Absyn_Exp_s_struct*)
_tmp9B5;if(_tmp9B6->tag != 1)goto _LL427;else{_tmp9B7=_tmp9B6->f1;}}_LL426: Cyc_Toc_exptypes_to_c(
_tmp9B7);goto _LL424;_LL427: {struct Cyc_Absyn_Seq_s_struct*_tmp9B8=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9B5;if(_tmp9B8->tag != 2)goto _LL429;else{_tmp9B9=_tmp9B8->f1;_tmp9BA=_tmp9B8->f2;}}
_LL428: Cyc_Toc_stmttypes_to_c(_tmp9B9);Cyc_Toc_stmttypes_to_c(_tmp9BA);goto
_LL424;_LL429: {struct Cyc_Absyn_Return_s_struct*_tmp9BB=(struct Cyc_Absyn_Return_s_struct*)
_tmp9B5;if(_tmp9BB->tag != 3)goto _LL42B;else{_tmp9BC=_tmp9BB->f1;}}_LL42A: if(
_tmp9BC != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9BC);goto _LL424;
_LL42B: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9BD=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9B5;if(_tmp9BD->tag != 4)goto _LL42D;else{_tmp9BE=_tmp9BD->f1;_tmp9BF=_tmp9BD->f2;
_tmp9C0=_tmp9BD->f3;}}_LL42C: Cyc_Toc_exptypes_to_c(_tmp9BE);Cyc_Toc_stmttypes_to_c(
_tmp9BF);Cyc_Toc_stmttypes_to_c(_tmp9C0);goto _LL424;_LL42D: {struct Cyc_Absyn_While_s_struct*
_tmp9C1=(struct Cyc_Absyn_While_s_struct*)_tmp9B5;if(_tmp9C1->tag != 5)goto _LL42F;
else{_tmp9C2=_tmp9C1->f1;_tmp9C3=_tmp9C2.f1;_tmp9C4=_tmp9C1->f2;}}_LL42E: Cyc_Toc_exptypes_to_c(
_tmp9C3);Cyc_Toc_stmttypes_to_c(_tmp9C4);goto _LL424;_LL42F: {struct Cyc_Absyn_For_s_struct*
_tmp9C5=(struct Cyc_Absyn_For_s_struct*)_tmp9B5;if(_tmp9C5->tag != 9)goto _LL431;
else{_tmp9C6=_tmp9C5->f1;_tmp9C7=_tmp9C5->f2;_tmp9C8=_tmp9C7.f1;_tmp9C9=_tmp9C5->f3;
_tmp9CA=_tmp9C9.f1;_tmp9CB=_tmp9C5->f4;}}_LL430: Cyc_Toc_exptypes_to_c(_tmp9C6);
Cyc_Toc_exptypes_to_c(_tmp9C8);Cyc_Toc_exptypes_to_c(_tmp9CA);Cyc_Toc_stmttypes_to_c(
_tmp9CB);goto _LL424;_LL431: {struct Cyc_Absyn_Switch_s_struct*_tmp9CC=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9B5;if(_tmp9CC->tag != 10)goto _LL433;else{_tmp9CD=_tmp9CC->f1;_tmp9CE=_tmp9CC->f2;}}
_LL432: Cyc_Toc_exptypes_to_c(_tmp9CD);for(0;_tmp9CE != 0;_tmp9CE=_tmp9CE->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9CE->hd)->body);}goto _LL424;_LL433: {struct
Cyc_Absyn_Decl_s_struct*_tmp9CF=(struct Cyc_Absyn_Decl_s_struct*)_tmp9B5;if(
_tmp9CF->tag != 12)goto _LL435;else{_tmp9D0=_tmp9CF->f1;_tmp9D1=_tmp9CF->f2;}}
_LL434: Cyc_Toc_decltypes_to_c(_tmp9D0);Cyc_Toc_stmttypes_to_c(_tmp9D1);goto
_LL424;_LL435: {struct Cyc_Absyn_Do_s_struct*_tmp9D2=(struct Cyc_Absyn_Do_s_struct*)
_tmp9B5;if(_tmp9D2->tag != 14)goto _LL437;else{_tmp9D3=_tmp9D2->f1;_tmp9D4=_tmp9D2->f2;
_tmp9D5=_tmp9D4.f1;}}_LL436: Cyc_Toc_stmttypes_to_c(_tmp9D3);Cyc_Toc_exptypes_to_c(
_tmp9D5);goto _LL424;_LL437: {struct Cyc_Absyn_Skip_s_struct*_tmp9D6=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9B5;if(_tmp9D6->tag != 0)goto _LL439;}_LL438: goto _LL43A;_LL439: {struct Cyc_Absyn_Break_s_struct*
_tmp9D7=(struct Cyc_Absyn_Break_s_struct*)_tmp9B5;if(_tmp9D7->tag != 6)goto _LL43B;}
_LL43A: goto _LL43C;_LL43B: {struct Cyc_Absyn_Continue_s_struct*_tmp9D8=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9B5;if(_tmp9D8->tag != 7)goto _LL43D;}_LL43C: goto _LL43E;_LL43D: {struct Cyc_Absyn_Goto_s_struct*
_tmp9D9=(struct Cyc_Absyn_Goto_s_struct*)_tmp9B5;if(_tmp9D9->tag != 8)goto _LL43F;}
_LL43E: goto _LL424;_LL43F: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9DA=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9B5;if(_tmp9DA->tag != 11)goto _LL441;}_LL440: goto _LL442;_LL441: {struct Cyc_Absyn_Label_s_struct*
_tmp9DB=(struct Cyc_Absyn_Label_s_struct*)_tmp9B5;if(_tmp9DB->tag != 13)goto _LL443;}
_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9DC=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9B5;if(_tmp9DC->tag != 15)goto _LL445;}_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9DD=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9B5;if(_tmp9DD->tag != 16)goto
_LL424;}_LL446:({void*_tmp9DE=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp9DF="Cyclone statement in C code";
_tag_dyneither(_tmp9DF,sizeof(char),28);}),_tag_dyneither(_tmp9DE,sizeof(void*),
0));});goto _LL424;_LL424:;}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv))({void*_tmp9E0=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp9E1="decls_to_c: not at toplevel!";_tag_dyneither(_tmp9E1,sizeof(char),29);}),
_tag_dyneither(_tmp9E0,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9E2=d->r;struct Cyc_Absyn_Vardecl*_tmp9E4;struct Cyc_Absyn_Fndecl*
_tmp9E6;struct Cyc_Absyn_Aggrdecl*_tmp9EC;struct Cyc_Absyn_Datatypedecl*_tmp9EE;
struct Cyc_Absyn_Enumdecl*_tmp9F0;struct Cyc_Absyn_Typedefdecl*_tmp9F2;struct Cyc_List_List*
_tmp9F6;struct Cyc_List_List*_tmp9F8;struct Cyc_List_List*_tmp9FA;struct Cyc_List_List*
_tmp9FC;_LL448: {struct Cyc_Absyn_Var_d_struct*_tmp9E3=(struct Cyc_Absyn_Var_d_struct*)
_tmp9E2;if(_tmp9E3->tag != 0)goto _LL44A;else{_tmp9E4=_tmp9E3->f1;}}_LL449: {
struct _tuple1*_tmp9FD=_tmp9E4->name;if(_tmp9E4->sc == Cyc_Absyn_ExternC)_tmp9FD=({
struct _tuple1*_tmp9FE=_cycalloc(sizeof(*_tmp9FE));_tmp9FE->f1=Cyc_Absyn_Rel_n(0);
_tmp9FE->f2=(*_tmp9FD).f2;_tmp9FE;});if(_tmp9E4->initializer != 0){if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9E4->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9E4->initializer));}}
nv=Cyc_Toc_add_varmap(r,nv,_tmp9E4->name,Cyc_Absyn_varb_exp(_tmp9FD,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp9FF=_cycalloc(sizeof(*_tmp9FF));_tmp9FF[0]=({
struct Cyc_Absyn_Global_b_struct _tmpA00;_tmpA00.tag=1;_tmpA00.f1=_tmp9E4;_tmpA00;});
_tmp9FF;}),0));_tmp9E4->name=_tmp9FD;_tmp9E4->sc=Cyc_Toc_scope_to_c(_tmp9E4->sc);
_tmp9E4->type=Cyc_Toc_typ_to_c_array(_tmp9E4->type);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmpA01=_cycalloc(sizeof(*_tmpA01));_tmpA01->hd=d;_tmpA01->tl=
Cyc_Toc_result_decls;_tmpA01;});goto _LL447;}_LL44A: {struct Cyc_Absyn_Fn_d_struct*
_tmp9E5=(struct Cyc_Absyn_Fn_d_struct*)_tmp9E2;if(_tmp9E5->tag != 1)goto _LL44C;
else{_tmp9E6=_tmp9E5->f1;}}_LL44B: {struct _tuple1*_tmpA02=_tmp9E6->name;if(
_tmp9E6->sc == Cyc_Absyn_ExternC){_tmpA02=({struct _tuple1*_tmpA03=_cycalloc(
sizeof(*_tmpA03));_tmpA03->f1=Cyc_Absyn_Rel_n(0);_tmpA03->f2=(*_tmpA02).f2;
_tmpA03;});_tmp9E6->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,nv,_tmp9E6->name,
Cyc_Absyn_var_exp(_tmpA02,0));_tmp9E6->name=_tmpA02;Cyc_Toc_fndecl_to_c(nv,
_tmp9E6,cinclude);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpA04=_cycalloc(
sizeof(*_tmpA04));_tmpA04->hd=d;_tmpA04->tl=Cyc_Toc_result_decls;_tmpA04;});goto
_LL447;}_LL44C: {struct Cyc_Absyn_Let_d_struct*_tmp9E7=(struct Cyc_Absyn_Let_d_struct*)
_tmp9E2;if(_tmp9E7->tag != 2)goto _LL44E;}_LL44D: goto _LL44F;_LL44E: {struct Cyc_Absyn_Letv_d_struct*
_tmp9E8=(struct Cyc_Absyn_Letv_d_struct*)_tmp9E2;if(_tmp9E8->tag != 3)goto _LL450;}
_LL44F:({void*_tmpA05=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmpA06="letdecl at toplevel";_tag_dyneither(
_tmpA06,sizeof(char),20);}),_tag_dyneither(_tmpA05,sizeof(void*),0));});_LL450: {
struct Cyc_Absyn_Region_d_struct*_tmp9E9=(struct Cyc_Absyn_Region_d_struct*)
_tmp9E2;if(_tmp9E9->tag != 4)goto _LL452;}_LL451:({void*_tmpA07=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpA08="region decl at toplevel";
_tag_dyneither(_tmpA08,sizeof(char),24);}),_tag_dyneither(_tmpA07,sizeof(void*),
0));});_LL452: {struct Cyc_Absyn_Alias_d_struct*_tmp9EA=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9E2;if(_tmp9EA->tag != 5)goto _LL454;}_LL453:({void*_tmpA09=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpA0A="alias decl at toplevel";
_tag_dyneither(_tmpA0A,sizeof(char),23);}),_tag_dyneither(_tmpA09,sizeof(void*),
0));});_LL454: {struct Cyc_Absyn_Aggr_d_struct*_tmp9EB=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp9E2;if(_tmp9EB->tag != 6)goto _LL456;else{_tmp9EC=_tmp9EB->f1;}}_LL455: Cyc_Toc_aggrdecl_to_c(
_tmp9EC);goto _LL447;_LL456: {struct Cyc_Absyn_Datatype_d_struct*_tmp9ED=(struct
Cyc_Absyn_Datatype_d_struct*)_tmp9E2;if(_tmp9ED->tag != 7)goto _LL458;else{_tmp9EE=
_tmp9ED->f1;}}_LL457: if(_tmp9EE->is_extensible)Cyc_Toc_xdatatypedecl_to_c(
_tmp9EE);else{Cyc_Toc_datatypedecl_to_c(_tmp9EE);}goto _LL447;_LL458: {struct Cyc_Absyn_Enum_d_struct*
_tmp9EF=(struct Cyc_Absyn_Enum_d_struct*)_tmp9E2;if(_tmp9EF->tag != 8)goto _LL45A;
else{_tmp9F0=_tmp9EF->f1;}}_LL459: Cyc_Toc_enumdecl_to_c(nv,_tmp9F0);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmpA0B=_cycalloc(sizeof(*_tmpA0B));_tmpA0B->hd=d;_tmpA0B->tl=
Cyc_Toc_result_decls;_tmpA0B;});goto _LL447;_LL45A: {struct Cyc_Absyn_Typedef_d_struct*
_tmp9F1=(struct Cyc_Absyn_Typedef_d_struct*)_tmp9E2;if(_tmp9F1->tag != 9)goto
_LL45C;else{_tmp9F2=_tmp9F1->f1;}}_LL45B: _tmp9F2->name=_tmp9F2->name;_tmp9F2->tvs=
0;if(_tmp9F2->defn != 0)_tmp9F2->defn=({struct Cyc_Core_Opt*_tmpA0C=_cycalloc(
sizeof(*_tmpA0C));_tmpA0C->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9F2->defn))->v);_tmpA0C;});else{switch((enum Cyc_Absyn_Kind)((
struct Cyc_Core_Opt*)_check_null(_tmp9F2->kind))->v){case Cyc_Absyn_BoxKind: _LL468:
_tmp9F2->defn=({struct Cyc_Core_Opt*_tmpA0D=_cycalloc(sizeof(*_tmpA0D));_tmpA0D->v=
Cyc_Absyn_void_star_typ();_tmpA0D;});break;default: _LL469: _tmp9F2->defn=({struct
Cyc_Core_Opt*_tmpA0E=_cycalloc(sizeof(*_tmpA0E));_tmpA0E->v=(void*)& Cyc_Absyn_VoidType_val;
_tmpA0E;});break;}}Cyc_Toc_result_decls=({struct Cyc_List_List*_tmpA0F=_cycalloc(
sizeof(*_tmpA0F));_tmpA0F->hd=d;_tmpA0F->tl=Cyc_Toc_result_decls;_tmpA0F;});goto
_LL447;_LL45C: {struct Cyc_Absyn_Porton_d_struct*_tmp9F3=(struct Cyc_Absyn_Porton_d_struct*)
_tmp9E2;if(_tmp9F3->tag != 14)goto _LL45E;}_LL45D: goto _LL45F;_LL45E: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9F4=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9E2;if(_tmp9F4->tag != 15)goto
_LL460;}_LL45F: goto _LL447;_LL460: {struct Cyc_Absyn_Namespace_d_struct*_tmp9F5=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9E2;if(_tmp9F5->tag != 10)goto _LL462;
else{_tmp9F6=_tmp9F5->f2;}}_LL461: _tmp9F8=_tmp9F6;goto _LL463;_LL462: {struct Cyc_Absyn_Using_d_struct*
_tmp9F7=(struct Cyc_Absyn_Using_d_struct*)_tmp9E2;if(_tmp9F7->tag != 11)goto _LL464;
else{_tmp9F8=_tmp9F7->f2;}}_LL463: _tmp9FA=_tmp9F8;goto _LL465;_LL464: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9F9=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9E2;if(_tmp9F9->tag != 12)goto
_LL466;else{_tmp9FA=_tmp9F9->f1;}}_LL465: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FA,top,
cinclude);goto _LL447;_LL466: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp9FB=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9E2;if(_tmp9FB->tag != 13)goto _LL447;
else{_tmp9FC=_tmp9FB->f1;}}_LL467: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FC,top,1);goto
_LL447;_LL447:;}}return nv;}static void Cyc_Toc_init(){struct _DynRegionHandle*
_tmpA11;struct Cyc_Core_NewRegion _tmpA10=Cyc_Core_new_dynregion();_tmpA11=_tmpA10.dynregion;{
struct _DynRegionFrame _tmpA12;struct _RegionHandle*d=_open_dynregion(& _tmpA12,
_tmpA11);Cyc_Toc_toc_state=({struct Cyc_Toc_TocState*_tmpA13=_cycalloc(sizeof(*
_tmpA13));_tmpA13->dyn=(struct _DynRegionHandle*)_tmpA11;_tmpA13->tuple_types=(
struct Cyc_List_List**)({struct Cyc_List_List**_tmpA18=_region_malloc(d,sizeof(*
_tmpA18));_tmpA18[0]=0;_tmpA18;});_tmpA13->aggrs_so_far=(struct Cyc_Dict_Dict*)({
struct Cyc_Dict_Dict*_tmpA17=_region_malloc(d,sizeof(*_tmpA17));_tmpA17[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpA17;});_tmpA13->datatypes_so_far=(
struct Cyc_Set_Set**)({struct Cyc_Set_Set**_tmpA16=_region_malloc(d,sizeof(*
_tmpA16));_tmpA16[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);_tmpA16;});
_tmpA13->xdatatypes_so_far=(struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*_tmpA15=
_region_malloc(d,sizeof(*_tmpA15));_tmpA15[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);
_tmpA15;});_tmpA13->qvar_tags=(struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*
_tmpA14=_region_malloc(d,sizeof(*_tmpA14));_tmpA14[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(
d,Cyc_Toc_qvar_tag_cmp);_tmpA14;});_tmpA13->temp_labels=(struct Cyc_Xarray_Xarray*)((
struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);
_tmpA13;});Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=
0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmpA19=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38);_tmpA19[0]=& Cyc_Toc__throw_str;_tmpA19[1]=&
Cyc_Toc_setjmp_str;_tmpA19[2]=& Cyc_Toc__push_handler_str;_tmpA19[3]=& Cyc_Toc__pop_handler_str;
_tmpA19[4]=& Cyc_Toc__exn_thrown_str;_tmpA19[5]=& Cyc_Toc__npop_handler_str;
_tmpA19[6]=& Cyc_Toc__check_null_str;_tmpA19[7]=& Cyc_Toc__check_known_subscript_null_str;
_tmpA19[8]=& Cyc_Toc__check_known_subscript_notnull_str;_tmpA19[9]=& Cyc_Toc__check_dyneither_subscript_str;
_tmpA19[10]=& Cyc_Toc__dyneither_ptr_str;_tmpA19[11]=& Cyc_Toc__tag_dyneither_str;
_tmpA19[12]=& Cyc_Toc__init_dyneither_ptr_str;_tmpA19[13]=& Cyc_Toc__untag_dyneither_ptr_str;
_tmpA19[14]=& Cyc_Toc__get_dyneither_size_str;_tmpA19[15]=& Cyc_Toc__get_zero_arr_size_str;
_tmpA19[16]=& Cyc_Toc__dyneither_ptr_plus_str;_tmpA19[17]=& Cyc_Toc__zero_arr_plus_str;
_tmpA19[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmpA19[19]=& Cyc_Toc__zero_arr_inplace_plus_str;
_tmpA19[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmpA19[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str;
_tmpA19[22]=& Cyc_Toc__cycalloc_str;_tmpA19[23]=& Cyc_Toc__cyccalloc_str;_tmpA19[
24]=& Cyc_Toc__cycalloc_atomic_str;_tmpA19[25]=& Cyc_Toc__cyccalloc_atomic_str;
_tmpA19[26]=& Cyc_Toc__region_malloc_str;_tmpA19[27]=& Cyc_Toc__region_calloc_str;
_tmpA19[28]=& Cyc_Toc__check_times_str;_tmpA19[29]=& Cyc_Toc__new_region_str;
_tmpA19[30]=& Cyc_Toc__push_region_str;_tmpA19[31]=& Cyc_Toc__pop_region_str;
_tmpA19[32]=& Cyc_Toc__open_dynregion_str;_tmpA19[33]=& Cyc_Toc__push_dynregion_str;
_tmpA19[34]=& Cyc_Toc__pop_dynregion_str;_tmpA19[35]=& Cyc_Toc__reset_region_str;
_tmpA19[36]=& Cyc_Toc__throw_arraybounds_str;_tmpA19[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str;
_tmpA19;}),sizeof(struct _dyneither_ptr*),38);;_pop_dynregion(d);}}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmpA1A=
_new_region("start");struct _RegionHandle*start=& _tmpA1A;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmpA1C;struct Cyc_Toc_TocState
_tmpA1B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA1C=
_tmpA1B.dyn;Cyc_Core_free_dynregion(_tmpA1C);}{struct Cyc_List_List*_tmpA1D=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmpA1D;};_pop_region(start);}}
