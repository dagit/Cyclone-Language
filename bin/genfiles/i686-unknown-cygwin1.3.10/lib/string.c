#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[52];
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
void*v;};struct _dyneither_ptr Cyc_Core_new_string(unsigned int);struct
_dyneither_ptr Cyc_Core_rnew_string(struct _RegionHandle*,unsigned int);extern char
Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{char*tag;
struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct _dyneither_ptr wrap_Cbuffer_as_buffer(char*,unsigned int);struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int toupper(
int);char*strerror(int errnum);unsigned int Cyc_strlen(struct _dyneither_ptr s);int
Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);int Cyc_strncmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2,unsigned int len);int Cyc_zstrcmp(struct
_dyneither_ptr,struct _dyneither_ptr);int Cyc_zstrncmp(struct _dyneither_ptr s1,
struct _dyneither_ptr s2,unsigned int n);int Cyc_zstrptrcmp(struct _dyneither_ptr*,
struct _dyneither_ptr*);int Cyc_strcasecmp(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,
unsigned int len);struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct
_dyneither_ptr src);struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_rstrconcat(struct _RegionHandle*,struct
_dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strconcat_l(struct
Cyc_List_List*);struct _dyneither_ptr Cyc_rstrconcat_l(struct _RegionHandle*,struct
Cyc_List_List*);struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct
_dyneither_ptr);struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct
Cyc_List_List*,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strcpy(struct
_dyneither_ptr dest,struct _dyneither_ptr src);struct _dyneither_ptr Cyc_strncpy(
struct _dyneither_ptr,struct _dyneither_ptr,unsigned int);struct _dyneither_ptr Cyc_zstrncpy(
struct _dyneither_ptr,struct _dyneither_ptr,unsigned int);struct _dyneither_ptr Cyc_realloc(
struct _dyneither_ptr,unsigned int);struct _dyneither_ptr Cyc__memcpy(struct
_dyneither_ptr d,struct _dyneither_ptr s,unsigned int,unsigned int);struct
_dyneither_ptr Cyc__memmove(struct _dyneither_ptr d,struct _dyneither_ptr s,
unsigned int,unsigned int);int Cyc_memcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2,unsigned int n);struct _dyneither_ptr Cyc_memchr(struct
_dyneither_ptr s,char c,unsigned int n);struct _dyneither_ptr Cyc_mmemchr(struct
_dyneither_ptr s,char c,unsigned int n);struct _dyneither_ptr Cyc_memset(struct
_dyneither_ptr s,char c,unsigned int n);void Cyc_bzero(struct _dyneither_ptr s,
unsigned int n);void Cyc__bcopy(struct _dyneither_ptr src,struct _dyneither_ptr dst,
unsigned int n,unsigned int sz);struct _dyneither_ptr Cyc_expand(struct
_dyneither_ptr s,unsigned int sz);struct _dyneither_ptr Cyc_rexpand(struct
_RegionHandle*,struct _dyneither_ptr s,unsigned int sz);struct _dyneither_ptr Cyc_realloc_str(
struct _dyneither_ptr str,unsigned int sz);struct _dyneither_ptr Cyc_rrealloc_str(
struct _RegionHandle*r,struct _dyneither_ptr str,unsigned int sz);struct
_dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);struct _dyneither_ptr Cyc_rstrdup(
struct _RegionHandle*,struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned int n);struct _dyneither_ptr Cyc_rsubstring(
struct _RegionHandle*,struct _dyneither_ptr,int ofs,unsigned int n);struct
_dyneither_ptr Cyc_replace_suffix(struct _dyneither_ptr,struct _dyneither_ptr,
struct _dyneither_ptr);struct _dyneither_ptr Cyc_rreplace_suffix(struct
_RegionHandle*r,struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct
_dyneither_ptr new_suffix);struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,
char c);struct _dyneither_ptr Cyc_mstrchr(struct _dyneither_ptr s,char c);struct
_dyneither_ptr Cyc_mstrrchr(struct _dyneither_ptr s,char c);struct _dyneither_ptr Cyc_strrchr(
struct _dyneither_ptr s,char c);struct _dyneither_ptr Cyc_mstrstr(struct
_dyneither_ptr haystack,struct _dyneither_ptr needle);struct _dyneither_ptr Cyc_strstr(
struct _dyneither_ptr haystack,struct _dyneither_ptr needle);struct _dyneither_ptr Cyc_strpbrk(
struct _dyneither_ptr s,struct _dyneither_ptr accept);struct _dyneither_ptr Cyc_mstrpbrk(
struct _dyneither_ptr s,struct _dyneither_ptr accept);unsigned int Cyc_strspn(struct
_dyneither_ptr s,struct _dyneither_ptr accept);unsigned int Cyc_strcspn(struct
_dyneither_ptr s,struct _dyneither_ptr accept);struct _dyneither_ptr Cyc_strtok(
struct _dyneither_ptr s,struct _dyneither_ptr delim);struct Cyc_List_List*Cyc_explode(
struct _dyneither_ptr s);struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*,
struct _dyneither_ptr s);struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*c);
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,
unsigned int line);char*strerror(int errnum);unsigned int Cyc_strlen(struct
_dyneither_ptr s){unsigned int i;unsigned int sz=_get_dyneither_size(s,sizeof(char));
for(i=0;i < sz;++ i){if(((const char*)s.curr)[(int)i]== '\000')return i;}return i;}
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){if(s1.curr == s2.curr)
return 0;{int i=0;unsigned int sz1=_get_dyneither_size(s1,sizeof(char));
unsigned int sz2=_get_dyneither_size(s2,sizeof(char));unsigned int minsz=sz1 < sz2?
sz1: sz2;minsz <= _get_dyneither_size(s1,sizeof(char)) && minsz <= 
_get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,
struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp0="minsz <= numelts(s1) && minsz <= numelts(s2)";
_tag_dyneither(_tmp0,sizeof(char),45);}),({const char*_tmp1="string.cyc";
_tag_dyneither(_tmp1,sizeof(char),11);}),60);while(i < minsz){char c1=((const char*)
s1.curr)[i];char c2=((const char*)s2.curr)[i];if(c1 == '\000'){if(c2 == '\000')
return 0;else{return - 1;}}else{if(c2 == '\000')return 1;else{int diff=c1 - c2;if(diff
!= 0)return diff;}}++ i;}if(sz1 == sz2)return 0;if(minsz < sz2){if(*((const char*)
_check_dyneither_subscript(s2,sizeof(char),i))== '\000')return 0;else{return - 1;}}
else{if(*((const char*)_check_dyneither_subscript(s1,sizeof(char),i))== '\000')
return 0;else{return 1;}}};}int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2){return Cyc_strcmp((struct _dyneither_ptr)*s1,(struct
_dyneither_ptr)*s2);}inline static int Cyc_ncmp(struct _dyneither_ptr s1,unsigned int
len1,struct _dyneither_ptr s2,unsigned int len2,unsigned int n){if(n <= 0)return 0;{
unsigned int min_len=len1 > len2?len2: len1;unsigned int bound=min_len > n?n: min_len;
bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= _get_dyneither_size(s2,
sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,
unsigned int line))Cyc___assert_fail)(({const char*_tmp2="bound <= numelts(s1) && bound <= numelts(s2)";
_tag_dyneither(_tmp2,sizeof(char),45);}),({const char*_tmp3="string.cyc";
_tag_dyneither(_tmp3,sizeof(char),11);}),96);{int i=0;for(0;i < bound;++ i){int retc;
if((retc=((const char*)s1.curr)[i]- ((const char*)s2.curr)[i])!= 0)return retc;}}
if(len1 < n  || len2 < n)return(int)len1 - (int)len2;return 0;};}int Cyc_strncmp(
struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned int n){unsigned int len1=
Cyc_strlen(s1);unsigned int len2=Cyc_strlen(s2);return Cyc_ncmp(s1,len1,s2,len2,n);}
int Cyc_zstrcmp(struct _dyneither_ptr a,struct _dyneither_ptr b){if(a.curr == b.curr)
return 0;{unsigned int as=_get_dyneither_size(a,sizeof(char));unsigned int bs=
_get_dyneither_size(b,sizeof(char));unsigned int min_length=as < bs?as: bs;int i=- 1;
min_length <= _get_dyneither_size(a,sizeof(char)) && min_length <= 
_get_dyneither_size(b,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,
struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp4="min_length <= numelts(a) && min_length <= numelts(b)";
_tag_dyneither(_tmp4,sizeof(char),53);}),({const char*_tmp5="string.cyc";
_tag_dyneither(_tmp5,sizeof(char),11);}),128);while((++ i,i < min_length)){int diff=(
int)((const char*)a.curr)[i]- (int)((const char*)b.curr)[i];if(diff != 0)return diff;}
return(int)as - (int)bs;};}int Cyc_zstrncmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2,unsigned int n){if(n <= 0)return 0;{unsigned int s1size=
_get_dyneither_size(s1,sizeof(char));unsigned int s2size=_get_dyneither_size(s2,
sizeof(char));unsigned int min_size=s1size > s2size?s2size: s1size;unsigned int
bound=min_size > n?n: min_size;bound <= _get_dyneither_size(s1,sizeof(char)) && 
bound <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr
assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const
char*_tmp6="bound <= numelts(s1) && bound <= numelts(s2)";_tag_dyneither(_tmp6,
sizeof(char),45);}),({const char*_tmp7="string.cyc";_tag_dyneither(_tmp7,sizeof(
char),11);}),146);{int i=0;for(0;i < bound;++ i){if(((const char*)s1.curr)[i]< ((
const char*)s2.curr)[i])return - 1;else{if(((const char*)s2.curr)[i]< ((const char*)
s1.curr)[i])return 1;}}}if(min_size <= bound)return 0;if(s1size < s2size)return - 1;
else{return 1;}};}int Cyc_zstrptrcmp(struct _dyneither_ptr*a,struct _dyneither_ptr*b){
if((int)a == (int)b)return 0;return Cyc_zstrcmp((struct _dyneither_ptr)*a,(struct
_dyneither_ptr)*b);}inline static struct _dyneither_ptr Cyc_int_strcato(struct
_dyneither_ptr dest,struct _dyneither_ptr src){int i;unsigned int dsize;unsigned int
slen;unsigned int dlen;dsize=_get_dyneither_size(dest,sizeof(char));dlen=Cyc_strlen((
struct _dyneither_ptr)dest);slen=Cyc_strlen(src);if(slen + dlen <= dsize){slen <= 
_get_dyneither_size(src,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,
struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp8="slen <= numelts(src)";
_tag_dyneither(_tmp8,sizeof(char),21);}),({const char*_tmp9="string.cyc";
_tag_dyneither(_tmp9,sizeof(char),11);}),182);for(i=0;i < slen;++ i){({struct
_dyneither_ptr _tmpA=_dyneither_ptr_plus(dest,sizeof(char),(int)(i + dlen));char
_tmpB=*((char*)_check_dyneither_subscript(_tmpA,sizeof(char),0));char _tmpC=((
const char*)src.curr)[i];if(_get_dyneither_size(_tmpA,sizeof(char))== 1  && (_tmpB
== '\000'  && _tmpC != '\000'))_throw_arraybounds();*((char*)_tmpA.curr)=_tmpC;});}
if(i != dsize)({struct _dyneither_ptr _tmpD=_dyneither_ptr_plus(dest,sizeof(char),(
int)(i + dlen));char _tmpE=*((char*)_check_dyneither_subscript(_tmpD,sizeof(char),
0));char _tmpF='\000';if(_get_dyneither_size(_tmpD,sizeof(char))== 1  && (_tmpE == '\000'
 && _tmpF != '\000'))_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});}else{(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp10=_cycalloc(
sizeof(*_tmp10));_tmp10[0]=({struct Cyc_Core_Invalid_argument_struct _tmp11;_tmp11.tag=
Cyc_Core_Invalid_argument;_tmp11.f1=({const char*_tmp12="strcat";_tag_dyneither(
_tmp12,sizeof(char),7);});_tmp11;});_tmp10;}));}return dest;}struct _dyneither_ptr
Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src){return Cyc_int_strcato(
dest,src);}struct _dyneither_ptr Cyc_rstrconcat(struct _RegionHandle*r,struct
_dyneither_ptr a,struct _dyneither_ptr b){unsigned int _tmp13=Cyc_strlen(a);
unsigned int _tmp14=Cyc_strlen(b);struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,(
_tmp13 + _tmp14)+ 1);int i;int j;_tmp13 <= _get_dyneither_size(ans,sizeof(char)) && 
_tmp13 <= _get_dyneither_size(a,sizeof(char))?0:((int(*)(struct _dyneither_ptr
assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const
char*_tmp15="alen <= numelts(ans) && alen <= numelts(a)";_tag_dyneither(_tmp15,
sizeof(char),43);}),({const char*_tmp16="string.cyc";_tag_dyneither(_tmp16,
sizeof(char),11);}),206);for(i=0;i < _tmp13;++ i){({struct _dyneither_ptr _tmp17=
_dyneither_ptr_plus(ans,sizeof(char),i);char _tmp18=*((char*)
_check_dyneither_subscript(_tmp17,sizeof(char),0));char _tmp19=((const char*)a.curr)[
i];if(_get_dyneither_size(_tmp17,sizeof(char))== 1  && (_tmp18 == '\000'  && _tmp19
!= '\000'))_throw_arraybounds();*((char*)_tmp17.curr)=_tmp19;});}_tmp14 <= 
_get_dyneither_size(b,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,
struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp1A="blen <= numelts(b)";
_tag_dyneither(_tmp1A,sizeof(char),19);}),({const char*_tmp1B="string.cyc";
_tag_dyneither(_tmp1B,sizeof(char),11);}),208);for(j=0;j < _tmp14;++ j){({struct
_dyneither_ptr _tmp1C=_dyneither_ptr_plus(ans,sizeof(char),i + j);char _tmp1D=*((
char*)_check_dyneither_subscript(_tmp1C,sizeof(char),0));char _tmp1E=((const char*)
b.curr)[j];if(_get_dyneither_size(_tmp1C,sizeof(char))== 1  && (_tmp1D == '\000'
 && _tmp1E != '\000'))_throw_arraybounds();*((char*)_tmp1C.curr)=_tmp1E;});}
return ans;}struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr a,struct
_dyneither_ptr b){return Cyc_rstrconcat(Cyc_Core_heap_region,a,b);}struct
_dyneither_ptr Cyc_rstrconcat_l(struct _RegionHandle*r,struct Cyc_List_List*strs){
unsigned int len;unsigned int total_len=0;struct _dyneither_ptr ans;struct
_RegionHandle _tmp1F=_new_region("temp");struct _RegionHandle*temp=& _tmp1F;
_push_region(temp);{struct Cyc_List_List*lens=({struct Cyc_List_List*_tmp22=
_region_malloc(temp,sizeof(*_tmp22));_tmp22->hd=(void*)((unsigned int)0);_tmp22->tl=
0;_tmp22;});struct Cyc_List_List*end=lens;{struct Cyc_List_List*p=strs;for(0;p != 0;
p=p->tl){len=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)p->hd));
total_len +=len;((struct Cyc_List_List*)_check_null(end))->tl=({struct Cyc_List_List*
_tmp20=_region_malloc(temp,sizeof(*_tmp20));_tmp20->hd=(void*)len;_tmp20->tl=0;
_tmp20;});end=((struct Cyc_List_List*)_check_null(end))->tl;}}lens=lens->tl;ans=
Cyc_Core_rnew_string(r,total_len + 1);{unsigned int i=0;while(strs != 0){struct
_dyneither_ptr _tmp21=*((struct _dyneither_ptr*)strs->hd);len=(unsigned int)((
struct Cyc_List_List*)_check_null(lens))->hd;Cyc_strncpy(
_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),
sizeof(char),1),(struct _dyneither_ptr)_tmp21,len);i +=len;strs=strs->tl;lens=lens->tl;}};}{
struct _dyneither_ptr _tmp23=ans;_npop_handler(0);return _tmp23;};;_pop_region(temp);}
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*strs){return Cyc_rstrconcat_l(
Cyc_Core_heap_region,strs);}struct _dyneither_ptr Cyc_rstr_sepstr(struct
_RegionHandle*r,struct Cyc_List_List*strs,struct _dyneither_ptr separator){if(strs
== 0)return Cyc_Core_rnew_string(r,0);if(strs->tl == 0)return Cyc_rstrdup(r,(struct
_dyneither_ptr)*((struct _dyneither_ptr*)strs->hd));{struct Cyc_List_List*_tmp24=
strs;struct _RegionHandle _tmp25=_new_region("temp");struct _RegionHandle*temp=&
_tmp25;_push_region(temp);{struct Cyc_List_List*lens=({struct Cyc_List_List*_tmp29=
_region_malloc(temp,sizeof(*_tmp29));_tmp29->hd=(void*)((unsigned int)0);_tmp29->tl=
0;_tmp29;});struct Cyc_List_List*end=lens;unsigned int len;unsigned int total_len=0;
unsigned int list_len=0;for(0;_tmp24 != 0;_tmp24=_tmp24->tl){len=Cyc_strlen((
struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp24->hd));total_len +=len;((
struct Cyc_List_List*)_check_null(end))->tl=({struct Cyc_List_List*_tmp26=
_region_malloc(temp,sizeof(*_tmp26));_tmp26->hd=(void*)len;_tmp26->tl=0;_tmp26;});
end=((struct Cyc_List_List*)_check_null(end))->tl;++ list_len;}lens=lens->tl;{
unsigned int seplen=Cyc_strlen(separator);total_len +=(list_len - 1)* seplen;{
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,total_len + 1);unsigned int i=0;
while(strs->tl != 0){struct _dyneither_ptr _tmp27=*((struct _dyneither_ptr*)strs->hd);
len=(unsigned int)((struct Cyc_List_List*)_check_null(lens))->hd;Cyc_strncpy(
_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),
sizeof(char),1),(struct _dyneither_ptr)_tmp27,len);i +=len;Cyc_strncpy(
_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),
sizeof(char),1),separator,seplen);i +=seplen;strs=strs->tl;lens=lens->tl;}Cyc_strncpy(
_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),
sizeof(char),1),(struct _dyneither_ptr)*((struct _dyneither_ptr*)strs->hd),(
unsigned int)((struct Cyc_List_List*)_check_null(lens))->hd);{struct
_dyneither_ptr _tmp28=ans;_npop_handler(0);return _tmp28;};};};};_pop_region(temp);};}
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*strs,struct _dyneither_ptr
separator){return Cyc_rstr_sepstr(Cyc_Core_heap_region,strs,separator);}struct
_dyneither_ptr Cyc_strncpy(struct _dyneither_ptr dest,struct _dyneither_ptr src,
unsigned int n){int i;n <= _get_dyneither_size(src,sizeof(char)) && n <= 
_get_dyneither_size(dest,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,
struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp2A="n <= numelts(src) && n <= numelts(dest)";
_tag_dyneither(_tmp2A,sizeof(char),40);}),({const char*_tmp2B="string.cyc";
_tag_dyneither(_tmp2B,sizeof(char),11);}),299);for(i=0;i < n;++ i){char _tmp2C=((
const char*)src.curr)[i];if(_tmp2C == '\000')break;((char*)dest.curr)[i]=_tmp2C;}
for(0;i < n;++ i){((char*)dest.curr)[i]='\000';}return dest;}struct _dyneither_ptr
Cyc_zstrncpy(struct _dyneither_ptr dest,struct _dyneither_ptr src,unsigned int n){n <= 
_get_dyneither_size(dest,sizeof(char)) && n <= _get_dyneither_size(src,sizeof(
char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,
unsigned int line))Cyc___assert_fail)(({const char*_tmp2D="n <= numelts(dest) && n <= numelts(src)";
_tag_dyneither(_tmp2D,sizeof(char),40);}),({const char*_tmp2E="string.cyc";
_tag_dyneither(_tmp2E,sizeof(char),11);}),313);{int i;for(i=0;i < n;++ i){((char*)
dest.curr)[i]=((const char*)src.curr)[i];}return dest;};}struct _dyneither_ptr Cyc_strcpy(
struct _dyneither_ptr dest,struct _dyneither_ptr src){unsigned int ssz=
_get_dyneither_size(src,sizeof(char));unsigned int dsz=_get_dyneither_size(dest,
sizeof(char));if(ssz <= dsz){unsigned int i;for(i=0;i < ssz;++ i){char _tmp2F=((const
char*)src.curr)[(int)i];({struct _dyneither_ptr _tmp30=_dyneither_ptr_plus(dest,
sizeof(char),(int)i);char _tmp31=*((char*)_check_dyneither_subscript(_tmp30,
sizeof(char),0));char _tmp32=_tmp2F;if(_get_dyneither_size(_tmp30,sizeof(char))== 
1  && (_tmp31 == '\000'  && _tmp32 != '\000'))_throw_arraybounds();*((char*)_tmp30.curr)=
_tmp32;});if(_tmp2F == '\000')break;}}else{unsigned int len=Cyc_strlen(src);Cyc_strncpy(
_dyneither_ptr_decrease_size(dest,sizeof(char),1),src,len);if(len < 
_get_dyneither_size(dest,sizeof(char)))({struct _dyneither_ptr _tmp33=
_dyneither_ptr_plus(dest,sizeof(char),(int)len);char _tmp34=*((char*)
_check_dyneither_subscript(_tmp33,sizeof(char),0));char _tmp35='\000';if(
_get_dyneither_size(_tmp33,sizeof(char))== 1  && (_tmp34 == '\000'  && _tmp35 != '\000'))
_throw_arraybounds();*((char*)_tmp33.curr)=_tmp35;});}return dest;}struct
_dyneither_ptr Cyc_rstrdup(struct _RegionHandle*r,struct _dyneither_ptr src){
unsigned int len;struct _dyneither_ptr temp;len=Cyc_strlen(src);temp=Cyc_Core_rnew_string(
r,len + 1);Cyc_strncpy(_dyneither_ptr_decrease_size(temp,sizeof(char),1),src,len);
return temp;}struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src){return Cyc_rstrdup(
Cyc_Core_heap_region,src);}struct _dyneither_ptr Cyc_rexpand(struct _RegionHandle*r,
struct _dyneither_ptr s,unsigned int sz){struct _dyneither_ptr temp;unsigned int slen;
slen=Cyc_strlen(s);sz=sz > slen?sz: slen;temp=Cyc_Core_rnew_string(r,sz);Cyc_strncpy(
_dyneither_ptr_decrease_size(temp,sizeof(char),1),s,slen);if(slen != 
_get_dyneither_size(s,sizeof(char)))({struct _dyneither_ptr _tmp36=
_dyneither_ptr_plus(temp,sizeof(char),(int)slen);char _tmp37=*((char*)
_check_dyneither_subscript(_tmp36,sizeof(char),0));char _tmp38='\000';if(
_get_dyneither_size(_tmp36,sizeof(char))== 1  && (_tmp37 == '\000'  && _tmp38 != '\000'))
_throw_arraybounds();*((char*)_tmp36.curr)=_tmp38;});return temp;}struct
_dyneither_ptr Cyc_expand(struct _dyneither_ptr s,unsigned int sz){return Cyc_rexpand(
Cyc_Core_heap_region,s,sz);}struct _dyneither_ptr Cyc_rrealloc_str(struct
_RegionHandle*r,struct _dyneither_ptr str,unsigned int sz){unsigned int maxsizeP=
_get_dyneither_size(str,sizeof(char));if(maxsizeP == 0){maxsizeP=30 > sz?30: sz;str=
Cyc_Core_rnew_string(r,maxsizeP);({struct _dyneither_ptr _tmp39=
_dyneither_ptr_plus(str,sizeof(char),0);char _tmp3A=*((char*)
_check_dyneither_subscript(_tmp39,sizeof(char),0));char _tmp3B='\000';if(
_get_dyneither_size(_tmp39,sizeof(char))== 1  && (_tmp3A == '\000'  && _tmp3B != '\000'))
_throw_arraybounds();*((char*)_tmp39.curr)=_tmp3B;});}else{if(sz > maxsizeP){
maxsizeP=maxsizeP * 2 > (sz * 5)/ 4?maxsizeP * 2:(sz * 5)/ 4;str=Cyc_rexpand(r,(
struct _dyneither_ptr)str,maxsizeP);}}return str;}struct _dyneither_ptr Cyc_realloc_str(
struct _dyneither_ptr str,unsigned int sz){return Cyc_rrealloc_str(Cyc_Core_heap_region,
str,sz);}struct _dyneither_ptr Cyc_rsubstring(struct _RegionHandle*r,struct
_dyneither_ptr s,int start,unsigned int amt){struct _dyneither_ptr ans=Cyc_Core_rnew_string(
r,amt + 1);s=_dyneither_ptr_plus(s,sizeof(char),start);amt < _get_dyneither_size(
ans,sizeof(char)) && amt <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct
_dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({
const char*_tmp3C="amt < numelts(ans) && amt <= numelts(s)";_tag_dyneither(_tmp3C,
sizeof(char),40);}),({const char*_tmp3D="string.cyc";_tag_dyneither(_tmp3D,
sizeof(char),11);}),409);{unsigned int i=0;for(0;i < amt;++ i){({struct
_dyneither_ptr _tmp3E=_dyneither_ptr_plus(ans,sizeof(char),(int)i);char _tmp3F=*((
char*)_check_dyneither_subscript(_tmp3E,sizeof(char),0));char _tmp40=((const char*)
s.curr)[(int)i];if(_get_dyneither_size(_tmp3E,sizeof(char))== 1  && (_tmp3F == '\000'
 && _tmp40 != '\000'))_throw_arraybounds();*((char*)_tmp3E.curr)=_tmp40;});}}({
struct _dyneither_ptr _tmp41=_dyneither_ptr_plus(ans,sizeof(char),(int)amt);char
_tmp42=*((char*)_check_dyneither_subscript(_tmp41,sizeof(char),0));char _tmp43='\000';
if(_get_dyneither_size(_tmp41,sizeof(char))== 1  && (_tmp42 == '\000'  && _tmp43 != '\000'))
_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});return ans;}struct
_dyneither_ptr Cyc_substring(struct _dyneither_ptr s,int start,unsigned int amt){
return Cyc_rsubstring(Cyc_Core_heap_region,s,start,amt);}struct _dyneither_ptr Cyc_rreplace_suffix(
struct _RegionHandle*r,struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,
struct _dyneither_ptr new_suffix){unsigned int m=_get_dyneither_size(src,sizeof(
char));unsigned int n=_get_dyneither_size(curr_suffix,sizeof(char));struct
_dyneither_ptr err=({const char*_tmp48="replace_suffix";_tag_dyneither(_tmp48,
sizeof(char),15);});if(m < n)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp45;_tmp45.tag=Cyc_Core_Invalid_argument;_tmp45.f1=err;_tmp45;});_tmp44;}));{
unsigned int i=1;for(0;i <= n;++ i){if(*((const char*)_check_dyneither_subscript(src,
sizeof(char),(int)(m - i)))!= *((const char*)_check_dyneither_subscript(
curr_suffix,sizeof(char),(int)(n - i))))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp47;_tmp47.tag=Cyc_Core_Invalid_argument;_tmp47.f1=err;_tmp47;});_tmp46;}));}}{
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,((m - n)+ _get_dyneither_size(
new_suffix,sizeof(char)))+ 1);Cyc_strncpy(_dyneither_ptr_decrease_size(ans,
sizeof(char),1),src,m - n);Cyc_strncpy(_dyneither_ptr_decrease_size(
_dyneither_ptr_plus(ans,sizeof(char),(int)(m - n)),sizeof(char),1),new_suffix,
_get_dyneither_size(new_suffix,sizeof(char)));return ans;};}struct _dyneither_ptr
Cyc_replace_suffix(struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,
struct _dyneither_ptr new_suffix){return Cyc_rreplace_suffix(Cyc_Core_heap_region,
src,curr_suffix,new_suffix);}struct _dyneither_ptr Cyc_strpbrk(struct
_dyneither_ptr s,struct _dyneither_ptr accept){int len=(int)_get_dyneither_size(s,
sizeof(char));unsigned int asize=_get_dyneither_size(accept,sizeof(char));char c;
unsigned int i;for(i=0;i < len  && (c=((const char*)s.curr)[(int)i])!= 0;++ i){
unsigned int j=0;for(0;j < asize;++ j){if(c == ((const char*)accept.curr)[(int)j])
return _dyneither_ptr_plus(s,sizeof(char),(int)i);}}return(struct _dyneither_ptr)
_tag_dyneither(0,0,0);}struct _dyneither_ptr Cyc_mstrpbrk(struct _dyneither_ptr s,
struct _dyneither_ptr accept){int len=(int)_get_dyneither_size(s,sizeof(char));
unsigned int asize=_get_dyneither_size(accept,sizeof(char));char c;unsigned int i;
for(i=0;i < len  && (c=((char*)s.curr)[(int)i])!= 0;++ i){unsigned int j=0;for(0;j < 
asize;++ j){if(c == ((const char*)accept.curr)[(int)j])return _dyneither_ptr_plus(s,
sizeof(char),(int)i);}}return _tag_dyneither(0,0,0);}struct _dyneither_ptr Cyc_mstrchr(
struct _dyneither_ptr s,char c){int len=(int)_get_dyneither_size(s,sizeof(char));
char c2;unsigned int i;for(i=0;i < len  && (c2=((char*)s.curr)[(int)i])!= 0;++ i){if(
c2 == c)return _dyneither_ptr_plus(s,sizeof(char),(int)i);}return _tag_dyneither(0,
0,0);}struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c){int len=(int)
_get_dyneither_size(s,sizeof(char));char c2;unsigned int i;for(i=0;i < len  && (c2=((
const char*)s.curr)[(int)i])!= 0;++ i){if(c2 == c)return _dyneither_ptr_plus(s,
sizeof(char),(int)i);}return(struct _dyneither_ptr)_tag_dyneither(0,0,0);}struct
_dyneither_ptr Cyc_strrchr(struct _dyneither_ptr s0,char c){int len=(int)Cyc_strlen((
struct _dyneither_ptr)s0);int i=len - 1;struct _dyneither_ptr s=s0;
_dyneither_ptr_inplace_plus(& s,sizeof(char),i);for(0;i >= 0;(i --,
_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),0))== c)return(struct _dyneither_ptr)s;}
return(struct _dyneither_ptr)_tag_dyneither(0,0,0);}struct _dyneither_ptr Cyc_mstrrchr(
struct _dyneither_ptr s0,char c){int len=(int)Cyc_strlen((struct _dyneither_ptr)s0);
int i=len - 1;struct _dyneither_ptr s=s0;_dyneither_ptr_inplace_plus(& s,sizeof(char),
i);for(0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){if(*((
char*)_check_dyneither_subscript(s,sizeof(char),0))== c)return(struct
_dyneither_ptr)s;}return _tag_dyneither(0,0,0);}struct _dyneither_ptr Cyc_strstr(
struct _dyneither_ptr haystack,struct _dyneither_ptr needle){if(!((unsigned int)
haystack.curr) || !((unsigned int)needle.curr))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp4A;_tmp4A.tag=Cyc_Core_Invalid_argument;_tmp4A.f1=({const char*_tmp4B="strstr";
_tag_dyneither(_tmp4B,sizeof(char),7);});_tmp4A;});_tmp49;}));if(*((const char*)
_check_dyneither_subscript(needle,sizeof(char),0))== '\000')return haystack;{int
len=(int)Cyc_strlen((struct _dyneither_ptr)needle);{struct _dyneither_ptr start=
haystack;for(0;(start=Cyc_strchr(start,*((const char*)_check_dyneither_subscript(
needle,sizeof(char),0)))).curr != (_tag_dyneither(0,0,0)).curr;start=Cyc_strchr(
_dyneither_ptr_plus(start,sizeof(char),1),*((const char*)
_check_dyneither_subscript(needle,sizeof(char),0)))){if(Cyc_strncmp((struct
_dyneither_ptr)start,(struct _dyneither_ptr)needle,(unsigned int)len)== 0)return
start;}}return(struct _dyneither_ptr)_tag_dyneither(0,0,0);};}struct
_dyneither_ptr Cyc_mstrstr(struct _dyneither_ptr haystack,struct _dyneither_ptr
needle){if(!((unsigned int)haystack.curr) || !((unsigned int)needle.curr))(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp4C=_cycalloc(sizeof(*
_tmp4C));_tmp4C[0]=({struct Cyc_Core_Invalid_argument_struct _tmp4D;_tmp4D.tag=Cyc_Core_Invalid_argument;
_tmp4D.f1=({const char*_tmp4E="mstrstr";_tag_dyneither(_tmp4E,sizeof(char),8);});
_tmp4D;});_tmp4C;}));if(*((const char*)_check_dyneither_subscript(needle,sizeof(
char),0))== '\000')return haystack;{int len=(int)Cyc_strlen((struct _dyneither_ptr)
needle);{struct _dyneither_ptr start=haystack;for(0;(start=Cyc_mstrchr(start,*((
const char*)_check_dyneither_subscript(needle,sizeof(char),0)))).curr != (
_tag_dyneither(0,0,0)).curr;start=Cyc_mstrchr(_dyneither_ptr_plus(start,sizeof(
char),1),*((const char*)_check_dyneither_subscript(needle,sizeof(char),0)))){if(
Cyc_strncmp((struct _dyneither_ptr)start,(struct _dyneither_ptr)needle,(
unsigned int)len)== 0)return start;}}return _tag_dyneither(0,0,0);};}unsigned int
Cyc_strspn(struct _dyneither_ptr s,struct _dyneither_ptr accept){unsigned int len=Cyc_strlen((
struct _dyneither_ptr)s);unsigned int asize=_get_dyneither_size(accept,sizeof(char));
len <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct _dyneither_ptr
assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const
char*_tmp4F="len <= numelts(s)";_tag_dyneither(_tmp4F,sizeof(char),18);}),({
const char*_tmp50="string.cyc";_tag_dyneither(_tmp50,sizeof(char),11);}),559);{
unsigned int i=0;for(0;i < len;++ i){int j;for(j=0;j < asize;++ j){if(((const char*)s.curr)[(
int)i]== ((const char*)accept.curr)[j])break;}if(j == asize)return i;}}return len;}
unsigned int Cyc_strcspn(struct _dyneither_ptr s,struct _dyneither_ptr accept){
unsigned int len=Cyc_strlen((struct _dyneither_ptr)s);unsigned int asize=
_get_dyneither_size(accept,sizeof(char));len <= _get_dyneither_size(s,sizeof(char))?
0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int
line))Cyc___assert_fail)(({const char*_tmp51="len <= numelts(s)";_tag_dyneither(
_tmp51,sizeof(char),18);}),({const char*_tmp52="string.cyc";_tag_dyneither(_tmp52,
sizeof(char),11);}),579);{unsigned int i=0;for(0;i < len;++ i){int j;for(j=0;j < 
asize;++ j){if(((const char*)s.curr)[(int)i]!= ((const char*)accept.curr)[j])break;}
if(j == asize)return i;}}return len;}struct _dyneither_ptr Cyc_strtok(struct
_dyneither_ptr s,struct _dyneither_ptr delim){static struct _dyneither_ptr olds={(void*)
0,(void*)0,(void*)(0 + 0)};struct _dyneither_ptr token;if(s.curr == (_tag_dyneither(
0,0,0)).curr){if(olds.curr == (_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,
0,0);s=olds;}{unsigned int inc=Cyc_strspn((struct _dyneither_ptr)s,delim);if(inc >= 
_get_dyneither_size(s,sizeof(char)) || *((char*)_check_dyneither_subscript(
_dyneither_ptr_plus(s,sizeof(char),(int)inc),sizeof(char),0))== '\000'){olds=
_tag_dyneither(0,0,0);return _tag_dyneither(0,0,0);}else{
_dyneither_ptr_inplace_plus(& s,sizeof(char),(int)inc);}token=s;s=Cyc_mstrpbrk(
token,(struct _dyneither_ptr)delim);if(s.curr == (_tag_dyneither(0,0,0)).curr)olds=
_tag_dyneither(0,0,0);else{({struct _dyneither_ptr _tmp53=s;char _tmp54=*((char*)
_check_dyneither_subscript(_tmp53,sizeof(char),0));char _tmp55='\000';if(
_get_dyneither_size(_tmp53,sizeof(char))== 1  && (_tmp54 == '\000'  && _tmp55 != '\000'))
_throw_arraybounds();*((char*)_tmp53.curr)=_tmp55;});olds=_dyneither_ptr_plus(s,
sizeof(char),1);}return token;};}struct Cyc_List_List*Cyc_rexplode(struct
_RegionHandle*r,struct _dyneither_ptr s){struct Cyc_List_List*result=0;{int i=(int)(
Cyc_strlen(s)- 1);for(0;i >= 0;-- i){result=({struct Cyc_List_List*_tmp56=
_region_malloc(r,sizeof(*_tmp56));_tmp56->hd=(void*)((int)*((const char*)
_check_dyneither_subscript(s,sizeof(char),i)));_tmp56->tl=result;_tmp56;});}}
return result;}struct Cyc_List_List*Cyc_explode(struct _dyneither_ptr s){return Cyc_rexplode(
Cyc_Core_heap_region,s);}struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*
chars){struct _dyneither_ptr s=Cyc_Core_new_string((unsigned int)(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(chars)+ 1));unsigned int i=0;while(chars != 0){({
struct _dyneither_ptr _tmp57=_dyneither_ptr_plus(s,sizeof(char),(int)i ++);char
_tmp58=*((char*)_check_dyneither_subscript(_tmp57,sizeof(char),0));char _tmp59=(
char)((int)chars->hd);if(_get_dyneither_size(_tmp57,sizeof(char))== 1  && (_tmp58
== '\000'  && _tmp59 != '\000'))_throw_arraybounds();*((char*)_tmp57.curr)=_tmp59;});
chars=chars->tl;}return s;}inline static int Cyc_casecmp(struct _dyneither_ptr s1,
unsigned int len1,struct _dyneither_ptr s2,unsigned int len2){unsigned int min_length=
len1 < len2?len1: len2;min_length <= _get_dyneither_size(s1,sizeof(char)) && 
min_length <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct
_dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({
const char*_tmp5A="min_length <= numelts(s1) && min_length <= numelts(s2)";
_tag_dyneither(_tmp5A,sizeof(char),55);}),({const char*_tmp5B="string.cyc";
_tag_dyneither(_tmp5B,sizeof(char),11);}),660);{int i=- 1;while((++ i,i < min_length)){
int diff=toupper((int)((const char*)s1.curr)[i])- toupper((int)((const char*)s2.curr)[
i]);if(diff != 0)return diff;}return(int)len1 - (int)len2;};}int Cyc_strcasecmp(
struct _dyneither_ptr s1,struct _dyneither_ptr s2){if(s1.curr == s2.curr)return 0;{
unsigned int len1=Cyc_strlen(s1);unsigned int len2=Cyc_strlen(s2);return Cyc_casecmp(
s1,len1,s2,len2);};}inline static int Cyc_caseless_ncmp(struct _dyneither_ptr s1,
unsigned int len1,struct _dyneither_ptr s2,unsigned int len2,unsigned int n){if(n <= 0)
return 0;{unsigned int min_len=len1 > len2?len2: len1;unsigned int bound=min_len > n?n:
min_len;bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= 
_get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,
struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp5C="bound <= numelts(s1) && bound <= numelts(s2)";
_tag_dyneither(_tmp5C,sizeof(char),45);}),({const char*_tmp5D="string.cyc";
_tag_dyneither(_tmp5D,sizeof(char),11);}),687);{int i=0;for(0;i < bound;++ i){int
retc;if((retc=toupper((int)((const char*)s1.curr)[i])- toupper((int)((const char*)
s2.curr)[i]))!= 0)return retc;}}if(len1 < n  || len2 < n)return(int)len1 - (int)len2;
return 0;};}int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,
unsigned int n){unsigned int len1=Cyc_strlen(s1);unsigned int len2=Cyc_strlen(s2);
return Cyc_caseless_ncmp(s1,len1,s2,len2,n);}void*memcpy(void*,const void*,
unsigned int n);void*memmove(void*,const void*,unsigned int n);int memcmp(const void*,
const void*,unsigned int n);char*memchr(const char*,char c,unsigned int n);void*
memset(void*,int c,unsigned int n);void bcopy(const void*src,void*dest,unsigned int n);
void bzero(void*s,unsigned int n);char*GC_realloc(char*,unsigned int n);struct
_dyneither_ptr Cyc_realloc(struct _dyneither_ptr s,unsigned int n){char*_tmp5E=
GC_realloc((char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1)),n);return
wrap_Cbuffer_as_buffer(_tmp5E,n);}struct _dyneither_ptr Cyc__memcpy(struct
_dyneither_ptr d,struct _dyneither_ptr s,unsigned int n,unsigned int sz){if(((d.curr
== (_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(d,sizeof(void))< n) || s.curr
== (_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(s,sizeof(void))< n)(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp5F=_cycalloc(sizeof(*
_tmp5F));_tmp5F[0]=({struct Cyc_Core_Invalid_argument_struct _tmp60;_tmp60.tag=Cyc_Core_Invalid_argument;
_tmp60.f1=({const char*_tmp61="memcpy";_tag_dyneither(_tmp61,sizeof(char),7);});
_tmp60;});_tmp5F;}));memcpy((void*)_check_null(_untag_dyneither_ptr(d,sizeof(
void),1)),(const void*)_check_null(_untag_dyneither_ptr(s,sizeof(void),1)),n * sz);
return d;}struct _dyneither_ptr Cyc__memmove(struct _dyneither_ptr d,struct
_dyneither_ptr s,unsigned int n,unsigned int sz){if(((d.curr == (_tag_dyneither(0,0,
0)).curr  || _get_dyneither_size(d,sizeof(void))< n) || s.curr == (_tag_dyneither(
0,0,0)).curr) || _get_dyneither_size(s,sizeof(void))< n)(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[
0]=({struct Cyc_Core_Invalid_argument_struct _tmp63;_tmp63.tag=Cyc_Core_Invalid_argument;
_tmp63.f1=({const char*_tmp64="memove";_tag_dyneither(_tmp64,sizeof(char),7);});
_tmp63;});_tmp62;}));memmove((void*)_check_null(_untag_dyneither_ptr(d,sizeof(
void),1)),(const void*)_check_null(_untag_dyneither_ptr(s,sizeof(void),1)),n * sz);
return d;}int Cyc_memcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,
unsigned int n){if(((s1.curr == (_tag_dyneither(0,0,0)).curr  || s2.curr == (
_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(s1,sizeof(char))>= n) || 
_get_dyneither_size(s2,sizeof(char))>= n)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp66;_tmp66.tag=Cyc_Core_Invalid_argument;_tmp66.f1=({const char*_tmp67="memcmp";
_tag_dyneither(_tmp67,sizeof(char),7);});_tmp66;});_tmp65;}));return memcmp((
const void*)_check_null(_untag_dyneither_ptr(s1,sizeof(char),1)),(const void*)
_check_null(_untag_dyneither_ptr(s2,sizeof(char),1)),n);}struct _dyneither_ptr Cyc_memchr(
struct _dyneither_ptr s,char c,unsigned int n){unsigned int sz=_get_dyneither_size(s,
sizeof(char));if(s.curr == (_tag_dyneither(0,0,0)).curr  || n > sz)(int)_throw((
void*)({struct Cyc_Core_Invalid_argument_struct*_tmp68=_cycalloc(sizeof(*_tmp68));
_tmp68[0]=({struct Cyc_Core_Invalid_argument_struct _tmp69;_tmp69.tag=Cyc_Core_Invalid_argument;
_tmp69.f1=({const char*_tmp6A="memchr";_tag_dyneither(_tmp6A,sizeof(char),7);});
_tmp69;});_tmp68;}));{char*_tmp6B=memchr((const char*)_check_null(
_untag_dyneither_ptr(s,sizeof(char),1)),c,n);if(_tmp6B == 0)return(struct
_dyneither_ptr)_tag_dyneither(0,0,0);{unsigned int _tmp6C=(unsigned int)((const
char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1)));unsigned int _tmp6D=(
unsigned int)((const char*)_tmp6B);unsigned int _tmp6E=_tmp6D - _tmp6C;return
_dyneither_ptr_plus(s,sizeof(char),(int)_tmp6E);};};}struct _dyneither_ptr Cyc_mmemchr(
struct _dyneither_ptr s,char c,unsigned int n){unsigned int sz=_get_dyneither_size(s,
sizeof(char));if(s.curr == (_tag_dyneither(0,0,0)).curr  || n > sz)(int)_throw((
void*)({struct Cyc_Core_Invalid_argument_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));
_tmp6F[0]=({struct Cyc_Core_Invalid_argument_struct _tmp70;_tmp70.tag=Cyc_Core_Invalid_argument;
_tmp70.f1=({const char*_tmp71="mmemchr";_tag_dyneither(_tmp71,sizeof(char),8);});
_tmp70;});_tmp6F;}));{char*_tmp72=memchr((const char*)_check_null(
_untag_dyneither_ptr(s,sizeof(char),1)),c,n);if(_tmp72 == 0)return _tag_dyneither(
0,0,0);{unsigned int _tmp73=(unsigned int)((const char*)_check_null(
_untag_dyneither_ptr(s,sizeof(char),1)));unsigned int _tmp74=(unsigned int)_tmp72;
unsigned int _tmp75=_tmp74 - _tmp73;return _dyneither_ptr_plus(s,sizeof(char),(int)
_tmp75);};};}struct _dyneither_ptr Cyc_memset(struct _dyneither_ptr s,char c,
unsigned int n){if(s.curr == (_tag_dyneither(0,0,0)).curr  || n > 
_get_dyneither_size(s,sizeof(char)))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp77;_tmp77.tag=Cyc_Core_Invalid_argument;_tmp77.f1=({const char*_tmp78="memset";
_tag_dyneither(_tmp78,sizeof(char),7);});_tmp77;});_tmp76;}));memset((void*)((
char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1))),(int)c,n);return s;}
void Cyc_bzero(struct _dyneither_ptr s,unsigned int n){if(s.curr == (_tag_dyneither(0,
0,0)).curr  || _get_dyneither_size(s,sizeof(char))< n)(int)_throw((void*)({struct
Cyc_Core_Invalid_argument_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp7A;_tmp7A.tag=Cyc_Core_Invalid_argument;
_tmp7A.f1=({const char*_tmp7B="bzero";_tag_dyneither(_tmp7B,sizeof(char),6);});
_tmp7A;});_tmp79;}));((void(*)(char*s,unsigned int n))bzero)((char*)_check_null(
_untag_dyneither_ptr(s,sizeof(char),1)),n);}void Cyc__bcopy(struct _dyneither_ptr
src,struct _dyneither_ptr dst,unsigned int n,unsigned int sz){if(((src.curr == (
_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(src,sizeof(void))< n) || dst.curr
== (_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(dst,sizeof(void))< n)(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp7C=_cycalloc(
sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Core_Invalid_argument_struct _tmp7D;_tmp7D.tag=
Cyc_Core_Invalid_argument;_tmp7D.f1=({const char*_tmp7E="bcopy";_tag_dyneither(
_tmp7E,sizeof(char),6);});_tmp7D;});_tmp7C;}));bcopy((const void*)_check_null(
_untag_dyneither_ptr(src,sizeof(void),1)),(void*)_check_null(
_untag_dyneither_ptr(dst,sizeof(void),1)),n * sz);}
