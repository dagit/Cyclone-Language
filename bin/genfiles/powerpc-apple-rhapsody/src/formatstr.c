#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[192];
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
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*
tag;};int isdigit(int);unsigned long Cyc_strlen(struct _dyneither_ptr s);struct
_dyneither_ptr Cyc_implode(struct Cyc_List_List*c);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
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
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;
void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{
int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
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
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_HeapRgn_struct{int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;
};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{
Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{int tag;};struct Cyc_Absyn_Cdecl_att_struct{
int tag;};struct Cyc_Absyn_Fastcall_att_struct{int tag;};struct Cyc_Absyn_Noreturn_att_struct{
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
_tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6
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
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct
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
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;
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
int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);extern
union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;void*Cyc_Absyn_new_evar(struct
Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_char_typ;extern void*
Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_dyneither_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,struct Cyc_Position_Segment*);struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*);
struct _tuple10{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;char f5;int f6;};struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(
struct _RegionHandle*r,struct _dyneither_ptr s,int i);struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(
struct _RegionHandle*r,struct _dyneither_ptr s,int i){unsigned long _tmp0=Cyc_strlen((
struct _dyneither_ptr)s);if(i < 0  || i >= _tmp0)return 0;{struct Cyc_List_List*_tmp1=
0;char c=' ';for(0;i < _tmp0;++ i){c=*((const char*)_check_dyneither_subscript(s,
sizeof(char),i));switch(c){case '+': _LL0: goto _LL1;case '-': _LL1: goto _LL2;case ' ':
_LL2: goto _LL3;case '#': _LL3: goto _LL4;case '0': _LL4:{struct Cyc_List_List*_tmp138;
_tmp1=((_tmp138=_region_malloc(r,sizeof(*_tmp138)),((_tmp138->hd=(void*)((int)c),((
_tmp138->tl=_tmp1,_tmp138))))));}continue;default: _LL5: break;}break;}if(i >= 
_tmp0)return 0;_tmp1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp1);{struct Cyc_List_List*_tmp3=0;c=*((const char*)_check_dyneither_subscript(s,
sizeof(char),i));if(c == '*'){{struct Cyc_List_List*_tmp139;_tmp3=((_tmp139=
_region_malloc(r,sizeof(*_tmp139)),((_tmp139->hd=(void*)((int)c),((_tmp139->tl=
_tmp3,_tmp139))))));}++ i;}else{for(0;i < _tmp0;++ i){c=*((const char*)
_check_dyneither_subscript(s,sizeof(char),i));if(isdigit((int)c)){struct Cyc_List_List*
_tmp13A;_tmp3=((_tmp13A=_region_malloc(r,sizeof(*_tmp13A)),((_tmp13A->hd=(void*)((
int)c),((_tmp13A->tl=_tmp3,_tmp13A))))));}else{break;}}}if(i >= _tmp0)return 0;
_tmp3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3);{
struct Cyc_List_List*_tmp6=0;c=*((const char*)_check_dyneither_subscript(s,sizeof(
char),i));if(c == '.'){{struct Cyc_List_List*_tmp13B;_tmp6=((_tmp13B=
_region_malloc(r,sizeof(*_tmp13B)),((_tmp13B->hd=(void*)((int)c),((_tmp13B->tl=
_tmp6,_tmp13B))))));}++ i;if(i >= _tmp0)return 0;c=*((const char*)
_check_dyneither_subscript(s,sizeof(char),i));if(c == '*'){{struct Cyc_List_List*
_tmp13C;_tmp6=((_tmp13C=_region_malloc(r,sizeof(*_tmp13C)),((_tmp13C->hd=(void*)((
int)c),((_tmp13C->tl=_tmp6,_tmp13C))))));}++ i;}else{for(0;i < _tmp0;++ i){c=*((
const char*)_check_dyneither_subscript(s,sizeof(char),i));if(isdigit((int)c)){
struct Cyc_List_List*_tmp13D;_tmp6=((_tmp13D=_region_malloc(r,sizeof(*_tmp13D)),((
_tmp13D->hd=(void*)((int)c),((_tmp13D->tl=_tmp6,_tmp13D))))));}else{break;}}}}
if(i >= _tmp0)return 0;_tmp6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp6);{struct Cyc_List_List*_tmpA=0;c=*((const char*)_check_dyneither_subscript(s,
sizeof(char),i));switch(c){case 'h': _LL7:{struct Cyc_List_List*_tmp13E;_tmpA=((
_tmp13E=_region_malloc(r,sizeof(*_tmp13E)),((_tmp13E->hd=(void*)((int)c),((
_tmp13E->tl=_tmpA,_tmp13E))))));}++ i;if(i >= _tmp0)return 0;c=*((const char*)
_check_dyneither_subscript(s,sizeof(char),i));if(c == 'h'){{struct Cyc_List_List*
_tmp13F;_tmpA=((_tmp13F=_region_malloc(r,sizeof(*_tmp13F)),((_tmp13F->hd=(void*)((
int)c),((_tmp13F->tl=_tmpA,_tmp13F))))));}++ i;}break;case 'l': _LL8:{struct Cyc_List_List*
_tmp140;_tmpA=((_tmp140=_region_malloc(r,sizeof(*_tmp140)),((_tmp140->hd=(void*)((
int)c),((_tmp140->tl=_tmpA,_tmp140))))));}++ i;if(i >= _tmp0)return 0;c=*((const
char*)_check_dyneither_subscript(s,sizeof(char),i));if(c == 'l'){{struct Cyc_List_List*
_tmp141;_tmpA=((_tmp141=_region_malloc(r,sizeof(*_tmp141)),((_tmp141->hd=(void*)((
int)c),((_tmp141->tl=_tmpA,_tmp141))))));}++ i;}break;case 'j': _LL9: goto _LLA;case 'z':
_LLA: goto _LLB;case 't': _LLB: goto _LLC;case 'L': _LLC:{struct Cyc_List_List*_tmp142;
_tmpA=((_tmp142=_region_malloc(r,sizeof(*_tmp142)),((_tmp142->hd=(void*)((int)c),((
_tmp142->tl=_tmpA,_tmp142))))));}++ i;break;default: _LLD: break;}if(i >= _tmp0)
return 0;_tmpA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpA);c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));switch(c){
case 'd': _LLF: goto _LL10;case 'i': _LL10: goto _LL11;case 'o': _LL11: goto _LL12;case 'u':
_LL12: goto _LL13;case 'x': _LL13: goto _LL14;case 'X': _LL14: goto _LL15;case 'f': _LL15:
goto _LL16;case 'F': _LL16: goto _LL17;case 'e': _LL17: goto _LL18;case 'E': _LL18: goto
_LL19;case 'g': _LL19: goto _LL1A;case 'G': _LL1A: goto _LL1B;case 'a': _LL1B: goto _LL1C;
case 'A': _LL1C: goto _LL1D;case 'c': _LL1D: goto _LL1E;case 's': _LL1E: goto _LL1F;case 'p':
_LL1F: goto _LL20;case 'n': _LL20: goto _LL21;case '%': _LL21: break;default: _LL22: return
0;}{struct _tuple10*_tmp145;struct Cyc_Core_Opt*_tmp144;return(_tmp144=
_region_malloc(r,sizeof(*_tmp144)),((_tmp144->v=((_tmp145=_region_malloc(r,
sizeof(*_tmp145)),((_tmp145->f1=_tmp1,((_tmp145->f2=_tmp3,((_tmp145->f3=_tmp6,((
_tmp145->f4=_tmpA,((_tmp145->f5=c,((_tmp145->f6=i + 1,_tmp145)))))))))))))),
_tmp144)));}}}}}}struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,struct Cyc_Position_Segment*loc){
unsigned long _tmp12=Cyc_strlen((struct _dyneither_ptr)s);struct Cyc_List_List*
_tmp13=0;int i;struct _RegionHandle _tmp14=_new_region("temp");struct _RegionHandle*
temp=& _tmp14;_push_region(temp);for(i=0;i < _tmp12;++ i){if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),i))!= '%')continue;{struct Cyc_Core_Opt*
_tmp15=Cyc_Formatstr_parse_conversionspecification(temp,s,i + 1);if(_tmp15 == 0){{
const char*_tmp148;void*_tmp147;(_tmp147=0,Cyc_Tcutil_terr(loc,((_tmp148="bad format string",
_tag_dyneither(_tmp148,sizeof(char),18))),_tag_dyneither(_tmp147,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp18=0;_npop_handler(0);return _tmp18;}}{struct _tuple10
_tmp1A;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1C;struct Cyc_List_List*
_tmp1D;struct Cyc_List_List*_tmp1E;char _tmp1F;int _tmp20;struct _tuple10*_tmp19=(
struct _tuple10*)_tmp15->v;_tmp1A=*_tmp19;_tmp1B=_tmp1A.f1;_tmp1C=_tmp1A.f2;
_tmp1D=_tmp1A.f3;_tmp1E=_tmp1A.f4;_tmp1F=_tmp1A.f5;_tmp20=_tmp1A.f6;i=_tmp20 - 1;{
struct Cyc_List_List*_tmp21=_tmp1E;struct Cyc_List_List _tmp22;int _tmp23;struct Cyc_List_List*
_tmp24;_LL25: if(_tmp21 == 0)goto _LL27;_tmp22=*_tmp21;_tmp23=(int)_tmp22.hd;_tmp24=
_tmp22.tl;if(_tmp24 != 0)goto _LL27;if(!((_tmp23 == 'j'  || _tmp23 == 'z') || _tmp23
== 't'))goto _LL27;_LL26:{const char*_tmp14C;void*_tmp14B[1];struct Cyc_Int_pa_struct
_tmp14A;(_tmp14A.tag=1,((_tmp14A.f1=(unsigned long)_tmp23,((_tmp14B[0]=& _tmp14A,
Cyc_Tcutil_terr(loc,((_tmp14C="length modifier '%c' is not supported",
_tag_dyneither(_tmp14C,sizeof(char),38))),_tag_dyneither(_tmp14B,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp28=0;_npop_handler(0);return _tmp28;}_LL27:;_LL28: goto
_LL24;_LL24:;}{struct Cyc_List_List*_tmp29=_tmp1C;struct Cyc_List_List _tmp2A;int
_tmp2B;struct Cyc_List_List*_tmp2C;_LL2A: if(_tmp29 == 0)goto _LL2C;_tmp2A=*_tmp29;
_tmp2B=(int)_tmp2A.hd;_tmp2C=_tmp2A.tl;if(_tmp2C != 0)goto _LL2C;if(!(_tmp2B == '*'))
goto _LL2C;_LL2B:{struct Cyc_List_List*_tmp14D;_tmp13=((_tmp14D=_region_malloc(r,
sizeof(*_tmp14D)),((_tmp14D->hd=Cyc_Absyn_sint_typ,((_tmp14D->tl=_tmp13,_tmp14D))))));}
goto _LL29;_LL2C:;_LL2D: goto _LL29;_LL29:;}{struct Cyc_List_List*_tmp2E=_tmp1D;
struct Cyc_List_List _tmp2F;int _tmp30;struct Cyc_List_List*_tmp31;struct Cyc_List_List
_tmp32;int _tmp33;struct Cyc_List_List*_tmp34;_LL2F: if(_tmp2E == 0)goto _LL31;_tmp2F=*
_tmp2E;_tmp30=(int)_tmp2F.hd;_tmp31=_tmp2F.tl;if(_tmp31 == 0)goto _LL31;_tmp32=*
_tmp31;_tmp33=(int)_tmp32.hd;_tmp34=_tmp32.tl;if(_tmp34 != 0)goto _LL31;if(!(
_tmp30 == '.'  && _tmp33 == '*'))goto _LL31;_LL30:{struct Cyc_List_List*_tmp14E;
_tmp13=((_tmp14E=_region_malloc(r,sizeof(*_tmp14E)),((_tmp14E->hd=Cyc_Absyn_sint_typ,((
_tmp14E->tl=_tmp13,_tmp14E))))));}goto _LL2E;_LL31:;_LL32: goto _LL2E;_LL2E:;}{void*
t;switch(_tmp1F){case 'd': _LL33: goto _LL34;case 'i': _LL34:{struct Cyc_List_List*f=
_tmp1B;for(0;f != 0;f=f->tl){if((int)f->hd == '#'){{const char*_tmp152;void*_tmp151[
1];struct Cyc_Int_pa_struct _tmp150;(_tmp150.tag=1,((_tmp150.f1=(unsigned long)((
int)_tmp1F),((_tmp151[0]=& _tmp150,Cyc_Tcutil_terr(loc,((_tmp152="flag '#' is not valid with %%%c",
_tag_dyneither(_tmp152,sizeof(char),32))),_tag_dyneither(_tmp151,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp39=0;_npop_handler(0);return _tmp39;}}}}{struct Cyc_List_List*
_tmp3A=_tmp1E;struct Cyc_List_List _tmp3B;int _tmp3C;struct Cyc_List_List*_tmp3D;
struct Cyc_List_List _tmp3E;int _tmp3F;struct Cyc_List_List*_tmp40;struct Cyc_List_List
_tmp41;int _tmp42;struct Cyc_List_List*_tmp43;struct Cyc_List_List _tmp44;int _tmp45;
struct Cyc_List_List*_tmp46;_LL37: if(_tmp3A != 0)goto _LL39;_LL38: t=Cyc_Absyn_sint_typ;
goto _LL36;_LL39: if(_tmp3A == 0)goto _LL3B;_tmp3B=*_tmp3A;_tmp3C=(int)_tmp3B.hd;
_tmp3D=_tmp3B.tl;if(_tmp3D != 0)goto _LL3B;if(!(_tmp3C == 'l'))goto _LL3B;_LL3A: t=
Cyc_Absyn_slong_typ;goto _LL36;_LL3B: if(_tmp3A == 0)goto _LL3D;_tmp3E=*_tmp3A;
_tmp3F=(int)_tmp3E.hd;_tmp40=_tmp3E.tl;if(_tmp40 != 0)goto _LL3D;if(!(_tmp3F == 'h'))
goto _LL3D;_LL3C: t=Cyc_Absyn_sshort_typ;goto _LL36;_LL3D: if(_tmp3A == 0)goto _LL3F;
_tmp41=*_tmp3A;_tmp42=(int)_tmp41.hd;_tmp43=_tmp41.tl;if(_tmp43 == 0)goto _LL3F;
_tmp44=*_tmp43;_tmp45=(int)_tmp44.hd;_tmp46=_tmp44.tl;if(_tmp46 != 0)goto _LL3F;
if(!(_tmp42 == 'h'  && _tmp45 == 'h'))goto _LL3F;_LL3E: t=Cyc_Absyn_schar_typ;goto
_LL36;_LL3F:;_LL40:{const char*_tmp157;void*_tmp156[2];struct Cyc_String_pa_struct
_tmp155;struct Cyc_Int_pa_struct _tmp154;(_tmp154.tag=1,((_tmp154.f1=(
unsigned long)((int)_tmp1F),((_tmp155.tag=0,((_tmp155.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_implode(_tmp1E)),((_tmp156[0]=& _tmp155,((_tmp156[1]=&
_tmp154,Cyc_Tcutil_terr(loc,((_tmp157="length modifier '%s' is not allowed with %%%c",
_tag_dyneither(_tmp157,sizeof(char),46))),_tag_dyneither(_tmp156,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmp4B=0;_npop_handler(0);return _tmp4B;}_LL36:;}{struct Cyc_List_List*
_tmp158;_tmp13=((_tmp158=_region_malloc(r,sizeof(*_tmp158)),((_tmp158->hd=t,((
_tmp158->tl=_tmp13,_tmp158))))));}break;case 'u': _LL35:{struct Cyc_List_List*f=
_tmp1B;for(0;f != 0;f=f->tl){if((int)f->hd == '#'){{const char*_tmp15B;void*_tmp15A;(
_tmp15A=0,Cyc_Tcutil_terr(loc,((_tmp15B="Flag '#' not valid with %%u",
_tag_dyneither(_tmp15B,sizeof(char),28))),_tag_dyneither(_tmp15A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp4F=0;_npop_handler(0);return _tmp4F;}}}}goto _LL41;case 'o':
_LL41: goto _LL42;case 'x': _LL42: goto _LL43;case 'X': _LL43:{struct Cyc_List_List*
_tmp50=_tmp1E;struct Cyc_List_List _tmp51;int _tmp52;struct Cyc_List_List*_tmp53;
struct Cyc_List_List _tmp54;int _tmp55;struct Cyc_List_List*_tmp56;struct Cyc_List_List
_tmp57;int _tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List _tmp5A;int _tmp5B;
struct Cyc_List_List*_tmp5C;_LL46: if(_tmp50 != 0)goto _LL48;_LL47: t=Cyc_Absyn_uint_typ;
goto _LL45;_LL48: if(_tmp50 == 0)goto _LL4A;_tmp51=*_tmp50;_tmp52=(int)_tmp51.hd;
_tmp53=_tmp51.tl;if(_tmp53 != 0)goto _LL4A;if(!(_tmp52 == 'l'))goto _LL4A;_LL49: t=
Cyc_Absyn_ulong_typ;goto _LL45;_LL4A: if(_tmp50 == 0)goto _LL4C;_tmp54=*_tmp50;
_tmp55=(int)_tmp54.hd;_tmp56=_tmp54.tl;if(_tmp56 != 0)goto _LL4C;if(!(_tmp55 == 'h'))
goto _LL4C;_LL4B: t=Cyc_Absyn_ushort_typ;goto _LL45;_LL4C: if(_tmp50 == 0)goto _LL4E;
_tmp57=*_tmp50;_tmp58=(int)_tmp57.hd;_tmp59=_tmp57.tl;if(_tmp59 == 0)goto _LL4E;
_tmp5A=*_tmp59;_tmp5B=(int)_tmp5A.hd;_tmp5C=_tmp5A.tl;if(_tmp5C != 0)goto _LL4E;
if(!(_tmp58 == 'h'  && _tmp5B == 'h'))goto _LL4E;_LL4D: t=Cyc_Absyn_uchar_typ;goto
_LL45;_LL4E:;_LL4F:{const char*_tmp160;void*_tmp15F[2];struct Cyc_String_pa_struct
_tmp15E;struct Cyc_Int_pa_struct _tmp15D;(_tmp15D.tag=1,((_tmp15D.f1=(
unsigned long)((int)_tmp1F),((_tmp15E.tag=0,((_tmp15E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_implode(_tmp1E)),((_tmp15F[0]=& _tmp15E,((_tmp15F[1]=&
_tmp15D,Cyc_Tcutil_terr(loc,((_tmp160="length modifier '%s' is not allowed with %%%c",
_tag_dyneither(_tmp160,sizeof(char),46))),_tag_dyneither(_tmp15F,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmp61=0;_npop_handler(0);return _tmp61;}_LL45:;}{struct Cyc_List_List*
_tmp161;_tmp13=((_tmp161=_region_malloc(r,sizeof(*_tmp161)),((_tmp161->hd=t,((
_tmp161->tl=_tmp13,_tmp161))))));}break;case 'f': _LL44: goto _LL50;case 'F': _LL50:
goto _LL51;case 'e': _LL51: goto _LL52;case 'E': _LL52: goto _LL53;case 'g': _LL53: goto
_LL54;case 'G': _LL54: goto _LL55;case 'a': _LL55: goto _LL56;case 'A': _LL56:{struct Cyc_List_List*
_tmp63=_tmp1E;struct Cyc_List_List _tmp64;int _tmp65;struct Cyc_List_List*_tmp66;
_LL59: if(_tmp63 != 0)goto _LL5B;_LL5A:{struct Cyc_List_List*_tmp162;_tmp13=((
_tmp162=_region_malloc(r,sizeof(*_tmp162)),((_tmp162->hd=Cyc_Absyn_double_typ(0),((
_tmp162->tl=_tmp13,_tmp162))))));}goto _LL58;_LL5B: if(_tmp63 == 0)goto _LL5D;_tmp64=*
_tmp63;_tmp65=(int)_tmp64.hd;_tmp66=_tmp64.tl;if(_tmp66 != 0)goto _LL5D;if(!(
_tmp65 == 'l'))goto _LL5D;_LL5C:{struct Cyc_List_List*_tmp163;_tmp13=((_tmp163=
_region_malloc(r,sizeof(*_tmp163)),((_tmp163->hd=Cyc_Absyn_double_typ(1),((
_tmp163->tl=_tmp13,_tmp163))))));}goto _LL58;_LL5D:;_LL5E:{const char*_tmp168;void*
_tmp167[2];struct Cyc_String_pa_struct _tmp166;struct Cyc_Int_pa_struct _tmp165;(
_tmp165.tag=1,((_tmp165.f1=(unsigned long)((int)_tmp1F),((_tmp166.tag=0,((
_tmp166.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1E)),((
_tmp167[0]=& _tmp166,((_tmp167[1]=& _tmp165,Cyc_Tcutil_terr(loc,((_tmp168="length modifier '%s' is not allowed with %%%c",
_tag_dyneither(_tmp168,sizeof(char),46))),_tag_dyneither(_tmp167,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmp6D=0;_npop_handler(0);return _tmp6D;}_LL58:;}break;case 'c':
_LL57:{struct Cyc_List_List*f=_tmp1B;for(0;f != 0;f=f->tl){if((int)f->hd == '#'  || (
int)f->hd == '0'){{const char*_tmp16C;void*_tmp16B[1];struct Cyc_Int_pa_struct
_tmp16A;(_tmp16A.tag=1,((_tmp16A.f1=(unsigned long)((int)f->hd),((_tmp16B[0]=&
_tmp16A,Cyc_Tcutil_terr(loc,((_tmp16C="flag '%c' not allowed with %%c",
_tag_dyneither(_tmp16C,sizeof(char),31))),_tag_dyneither(_tmp16B,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp71=0;_npop_handler(0);return _tmp71;}}}}if(_tmp1E != 0){{
const char*_tmp170;void*_tmp16F[1];struct Cyc_String_pa_struct _tmp16E;(_tmp16E.tag=
0,((_tmp16E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1E)),((
_tmp16F[0]=& _tmp16E,Cyc_Tcutil_terr(loc,((_tmp170="length modifier '%s' not allowed with %%c",
_tag_dyneither(_tmp170,sizeof(char),42))),_tag_dyneither(_tmp16F,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp75=0;_npop_handler(0);return _tmp75;}}if(_tmp1D != 0){{
const char*_tmp174;void*_tmp173[1];struct Cyc_String_pa_struct _tmp172;(_tmp172.tag=
0,((_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1D)),((
_tmp173[0]=& _tmp172,Cyc_Tcutil_terr(loc,((_tmp174="precision '%s' not allowed with %%c",
_tag_dyneither(_tmp174,sizeof(char),36))),_tag_dyneither(_tmp173,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp79=0;_npop_handler(0);return _tmp79;}}{struct Cyc_List_List*
_tmp175;_tmp13=((_tmp175=_region_malloc(r,sizeof(*_tmp175)),((_tmp175->hd=Cyc_Absyn_sint_typ,((
_tmp175->tl=_tmp13,_tmp175))))));}break;case 's': _LL5F:{struct Cyc_List_List*f=
_tmp1B;for(0;f != 0;f=f->tl){if((int)f->hd != '-'){{const char*_tmp178;void*_tmp177;(
_tmp177=0,Cyc_Tcutil_terr(loc,((_tmp178="a flag not allowed with %%s",
_tag_dyneither(_tmp178,sizeof(char),28))),_tag_dyneither(_tmp177,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp7D=0;_npop_handler(0);return _tmp7D;}}}}if(_tmp1E != 0){{
const char*_tmp17B;void*_tmp17A;(_tmp17A=0,Cyc_Tcutil_terr(loc,((_tmp17B="length modifiers not allowed with %%s",
_tag_dyneither(_tmp17B,sizeof(char),38))),_tag_dyneither(_tmp17A,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp80=0;_npop_handler(0);return _tmp80;}}{struct Cyc_Core_Opt*
_tmp17C;void*ptr=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,((_tmp17C=_cycalloc(sizeof(*_tmp17C)),((
_tmp17C->v=Cyc_Tcenv_lookup_type_vars(te),_tmp17C))))),Cyc_Absyn_const_tqual(0),
Cyc_Absyn_false_conref);{struct Cyc_List_List*_tmp17D;_tmp13=((_tmp17D=
_region_malloc(r,sizeof(*_tmp17D)),((_tmp17D->hd=ptr,((_tmp17D->tl=_tmp13,
_tmp17D))))));}break;}case 'p': _LL60:{struct Cyc_List_List*_tmp17E;_tmp13=((
_tmp17E=_region_malloc(r,sizeof(*_tmp17E)),((_tmp17E->hd=Cyc_Absyn_uint_typ,((
_tmp17E->tl=_tmp13,_tmp17E))))));}break;case 'n': _LL61:{struct Cyc_List_List*f=
_tmp1B;for(0;f != 0;f=f->tl){if((int)f->hd == '#'  || (int)f->hd == '0'){{const char*
_tmp182;void*_tmp181[1];struct Cyc_Int_pa_struct _tmp180;(_tmp180.tag=1,((_tmp180.f1=(
unsigned long)((int)f->hd),((_tmp181[0]=& _tmp180,Cyc_Tcutil_terr(loc,((_tmp182="flag '%c' not allowed with %%n",
_tag_dyneither(_tmp182,sizeof(char),31))),_tag_dyneither(_tmp181,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp87=0;_npop_handler(0);return _tmp87;}}}}if(_tmp1D != 0){{
const char*_tmp186;void*_tmp185[1];struct Cyc_String_pa_struct _tmp184;(_tmp184.tag=
0,((_tmp184.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1D)),((
_tmp185[0]=& _tmp184,Cyc_Tcutil_terr(loc,((_tmp186="precision '%s' not allowed with %%n",
_tag_dyneither(_tmp186,sizeof(char),36))),_tag_dyneither(_tmp185,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp8B=0;_npop_handler(0);return _tmp8B;}}{struct Cyc_Core_Opt*
_tmp187;struct Cyc_Core_Opt*_tmp8C=(_tmp187=_cycalloc(sizeof(*_tmp187)),((_tmp187->v=
Cyc_Tcenv_lookup_type_vars(te),_tmp187)));{struct Cyc_List_List*_tmp8D=_tmp1E;
struct Cyc_List_List _tmp8E;int _tmp8F;struct Cyc_List_List*_tmp90;struct Cyc_List_List
_tmp91;int _tmp92;struct Cyc_List_List*_tmp93;struct Cyc_List_List _tmp94;int _tmp95;
struct Cyc_List_List*_tmp96;struct Cyc_List_List _tmp97;int _tmp98;struct Cyc_List_List*
_tmp99;_LL64: if(_tmp8D != 0)goto _LL66;_LL65: t=Cyc_Absyn_sint_typ;goto _LL63;_LL66:
if(_tmp8D == 0)goto _LL68;_tmp8E=*_tmp8D;_tmp8F=(int)_tmp8E.hd;_tmp90=_tmp8E.tl;
if(_tmp90 != 0)goto _LL68;if(!(_tmp8F == 'l'))goto _LL68;_LL67: t=Cyc_Absyn_ulong_typ;
goto _LL63;_LL68: if(_tmp8D == 0)goto _LL6A;_tmp91=*_tmp8D;_tmp92=(int)_tmp91.hd;
_tmp93=_tmp91.tl;if(_tmp93 != 0)goto _LL6A;if(!(_tmp92 == 'h'))goto _LL6A;_LL69: t=
Cyc_Absyn_sshort_typ;goto _LL63;_LL6A: if(_tmp8D == 0)goto _LL6C;_tmp94=*_tmp8D;
_tmp95=(int)_tmp94.hd;_tmp96=_tmp94.tl;if(_tmp96 == 0)goto _LL6C;_tmp97=*_tmp96;
_tmp98=(int)_tmp97.hd;_tmp99=_tmp97.tl;if(_tmp99 != 0)goto _LL6C;if(!(_tmp95 == 'h'
 && _tmp98 == 'h'))goto _LL6C;_LL6B: t=Cyc_Absyn_schar_typ;goto _LL63;_LL6C:;_LL6D:{
const char*_tmp18C;void*_tmp18B[2];struct Cyc_String_pa_struct _tmp18A;struct Cyc_Int_pa_struct
_tmp189;(_tmp189.tag=1,((_tmp189.f1=(unsigned long)((int)_tmp1F),((_tmp18A.tag=0,((
_tmp18A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1E)),((
_tmp18B[0]=& _tmp18A,((_tmp18B[1]=& _tmp189,Cyc_Tcutil_terr(loc,((_tmp18C="length modifier '%s' is not allowed with %%%c",
_tag_dyneither(_tmp18C,sizeof(char),46))),_tag_dyneither(_tmp18B,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmp9E=0;_npop_handler(0);return _tmp9E;}_LL63:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmp8C),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);{struct Cyc_List_List*_tmp18D;_tmp13=((_tmp18D=
_region_malloc(r,sizeof(*_tmp18D)),((_tmp18D->hd=t,((_tmp18D->tl=_tmp13,_tmp18D))))));}
break;}case '%': _LL62: if(_tmp1B != 0){{const char*_tmp191;void*_tmp190[1];struct Cyc_String_pa_struct
_tmp18F;(_tmp18F.tag=0,((_tmp18F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_implode(_tmp1B)),((_tmp190[0]=& _tmp18F,Cyc_Tcutil_terr(loc,((_tmp191="flags '%s' not allowed with %%%%",
_tag_dyneither(_tmp191,sizeof(char),33))),_tag_dyneither(_tmp190,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmpA4=0;_npop_handler(0);return _tmpA4;}}if(_tmp1C != 0){{
const char*_tmp195;void*_tmp194[1];struct Cyc_String_pa_struct _tmp193;(_tmp193.tag=
0,((_tmp193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1C)),((
_tmp194[0]=& _tmp193,Cyc_Tcutil_terr(loc,((_tmp195="width '%s' not allowed with %%%%",
_tag_dyneither(_tmp195,sizeof(char),33))),_tag_dyneither(_tmp194,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmpA8=0;_npop_handler(0);return _tmpA8;}}if(_tmp1D != 0){{
const char*_tmp199;void*_tmp198[1];struct Cyc_String_pa_struct _tmp197;(_tmp197.tag=
0,((_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1D)),((
_tmp198[0]=& _tmp197,Cyc_Tcutil_terr(loc,((_tmp199="precision '%s' not allowed with %%%%",
_tag_dyneither(_tmp199,sizeof(char),37))),_tag_dyneither(_tmp198,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmpAC=0;_npop_handler(0);return _tmpAC;}}if(_tmp1E != 0){{
const char*_tmp19D;void*_tmp19C[1];struct Cyc_String_pa_struct _tmp19B;(_tmp19B.tag=
0,((_tmp19B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmp1E)),((
_tmp19C[0]=& _tmp19B,Cyc_Tcutil_terr(loc,((_tmp19D="length modifier '%s' not allowed with %%%%",
_tag_dyneither(_tmp19D,sizeof(char),43))),_tag_dyneither(_tmp19C,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmpB0=0;_npop_handler(0);return _tmpB0;}}break;default: _LL6E: {
struct Cyc_List_List*_tmpB1=0;_npop_handler(0);return _tmpB1;}}}}}}{struct Cyc_List_List*
_tmpB2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp13);
_npop_handler(0);return _tmpB2;};_pop_region(temp);}struct _tuple11{int f1;struct
Cyc_List_List*f2;struct Cyc_List_List*f3;char f4;int f5;};struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(
struct _RegionHandle*r,struct _dyneither_ptr s,int i);struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(
struct _RegionHandle*r,struct _dyneither_ptr s,int i){unsigned long _tmpB3=Cyc_strlen((
struct _dyneither_ptr)s);if(i < 0  || i >= _tmpB3)return 0;{int _tmpB4=0;char _tmpB5=*((
const char*)_check_dyneither_subscript(s,sizeof(char),i));if(_tmpB5 == '*'){_tmpB4=
1;++ i;if(i >= _tmpB3)return 0;}{struct Cyc_List_List*_tmpB6=0;for(0;i < _tmpB3;++ i){
_tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));if(isdigit((
int)_tmpB5)){struct Cyc_List_List*_tmp19E;_tmpB6=((_tmp19E=_region_malloc(r,
sizeof(*_tmp19E)),((_tmp19E->hd=(void*)((int)_tmpB5),((_tmp19E->tl=_tmpB6,
_tmp19E))))));}else{break;}}if(i >= _tmpB3)return 0;_tmpB6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB6);{struct Cyc_List_List*_tmpB8=0;
_tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));switch(_tmpB5){
case 'h': _LL70:{struct Cyc_List_List*_tmp19F;_tmpB8=((_tmp19F=_region_malloc(r,
sizeof(*_tmp19F)),((_tmp19F->hd=(void*)((int)_tmpB5),((_tmp19F->tl=_tmpB8,
_tmp19F))))));}++ i;if(i >= _tmpB3)return 0;_tmpB5=*((const char*)
_check_dyneither_subscript(s,sizeof(char),i));if(_tmpB5 == 'h'){{struct Cyc_List_List*
_tmp1A0;_tmpB8=((_tmp1A0=_region_malloc(r,sizeof(*_tmp1A0)),((_tmp1A0->hd=(void*)((
int)_tmpB5),((_tmp1A0->tl=_tmpB8,_tmp1A0))))));}++ i;}break;case 'l': _LL71:{struct
Cyc_List_List*_tmp1A1;_tmpB8=((_tmp1A1=_region_malloc(r,sizeof(*_tmp1A1)),((
_tmp1A1->hd=(void*)((int)_tmpB5),((_tmp1A1->tl=_tmpB8,_tmp1A1))))));}++ i;if(i >= 
_tmpB3)return 0;_tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpB5 == 'l'){{struct Cyc_List_List*_tmp1A2;_tmpB8=((_tmp1A2=_region_malloc(r,
sizeof(*_tmp1A2)),((_tmp1A2->hd=(void*)((int)_tmpB5),((_tmp1A2->tl=_tmpB8,
_tmp1A2))))));}++ i;}break;case 'j': _LL72: goto _LL73;case 'z': _LL73: goto _LL74;case 't':
_LL74: goto _LL75;case 'L': _LL75:{struct Cyc_List_List*_tmp1A3;_tmpB8=((_tmp1A3=
_region_malloc(r,sizeof(*_tmp1A3)),((_tmp1A3->hd=(void*)((int)_tmpB5),((_tmp1A3->tl=
_tmpB8,_tmp1A3))))));}++ i;break;default: _LL76: break;}if(i >= _tmpB3)return 0;
_tmpB8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB8);
_tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));switch(_tmpB5){
case 'd': _LL78: goto _LL79;case 'i': _LL79: goto _LL7A;case 'o': _LL7A: goto _LL7B;case 'u':
_LL7B: goto _LL7C;case 'x': _LL7C: goto _LL7D;case 'X': _LL7D: goto _LL7E;case 'f': _LL7E:
goto _LL7F;case 'F': _LL7F: goto _LL80;case 'e': _LL80: goto _LL81;case 'E': _LL81: goto
_LL82;case 'g': _LL82: goto _LL83;case 'G': _LL83: goto _LL84;case 'a': _LL84: goto _LL85;
case 'A': _LL85: goto _LL86;case 'c': _LL86: goto _LL87;case 's': _LL87: goto _LL88;case 'p':
_LL88: goto _LL89;case 'n': _LL89: goto _LL8A;case '%': _LL8A: break;default: _LL8B: return
0;}{struct _tuple11*_tmp1A6;struct Cyc_Core_Opt*_tmp1A5;return(_tmp1A5=
_region_malloc(r,sizeof(*_tmp1A5)),((_tmp1A5->v=((_tmp1A6=_region_malloc(r,
sizeof(*_tmp1A6)),((_tmp1A6->f1=_tmpB4,((_tmp1A6->f2=_tmpB6,((_tmp1A6->f3=_tmpB8,((
_tmp1A6->f4=_tmpB5,((_tmp1A6->f5=i + 1,_tmp1A6)))))))))))),_tmp1A5)));}}}}}struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct _dyneither_ptr s,struct Cyc_Position_Segment*loc);struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc){unsigned long _tmpC0=Cyc_strlen((struct _dyneither_ptr)s);struct Cyc_List_List*
_tmpC1=0;int i;struct _RegionHandle _tmpC2=_new_region("temp");struct _RegionHandle*
temp=& _tmpC2;_push_region(temp);for(i=0;i < _tmpC0;++ i){if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),i))!= '%')continue;{struct Cyc_Core_Opt*
_tmpC3=Cyc_Formatstr_parse_inputformat(temp,s,i + 1);if(_tmpC3 == 0){{const char*
_tmp1A9;void*_tmp1A8;(_tmp1A8=0,Cyc_Tcutil_terr(loc,((_tmp1A9="bad format string",
_tag_dyneither(_tmp1A9,sizeof(char),18))),_tag_dyneither(_tmp1A8,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC6=0;_npop_handler(0);return _tmpC6;}}{struct _tuple11
_tmpC8;int _tmpC9;struct Cyc_List_List*_tmpCA;struct Cyc_List_List*_tmpCB;char
_tmpCC;int _tmpCD;struct _tuple11*_tmpC7=(struct _tuple11*)_tmpC3->v;_tmpC8=*_tmpC7;
_tmpC9=_tmpC8.f1;_tmpCA=_tmpC8.f2;_tmpCB=_tmpC8.f3;_tmpCC=_tmpC8.f4;_tmpCD=
_tmpC8.f5;i=_tmpCD - 1;{struct Cyc_List_List*_tmpCE=_tmpCB;struct Cyc_List_List
_tmpCF;int _tmpD0;struct Cyc_List_List*_tmpD1;_LL8E: if(_tmpCE == 0)goto _LL90;_tmpCF=*
_tmpCE;_tmpD0=(int)_tmpCF.hd;_tmpD1=_tmpCF.tl;if(_tmpD1 != 0)goto _LL90;if(!((
_tmpD0 == 'j'  || _tmpD0 == 'z') || _tmpD0 == 't'))goto _LL90;_LL8F:{const char*
_tmp1AD;void*_tmp1AC[1];struct Cyc_Int_pa_struct _tmp1AB;(_tmp1AB.tag=1,((_tmp1AB.f1=(
unsigned long)_tmpD0,((_tmp1AC[0]=& _tmp1AB,Cyc_Tcutil_terr(loc,((_tmp1AD="length modifier '%c' is not supported",
_tag_dyneither(_tmp1AD,sizeof(char),38))),_tag_dyneither(_tmp1AC,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmpD5=0;_npop_handler(0);return _tmpD5;}_LL90:;_LL91: goto
_LL8D;_LL8D:;}if(_tmpC9)continue;{void*t;switch(_tmpCC){case 'd': _LL92: goto _LL93;
case 'i': _LL93: {struct Cyc_Core_Opt*_tmp1AE;struct Cyc_Core_Opt*_tmpD6=(_tmp1AE=
_cycalloc(sizeof(*_tmp1AE)),((_tmp1AE->v=Cyc_Tcenv_lookup_type_vars(te),_tmp1AE)));{
struct Cyc_List_List*_tmpD7=_tmpCB;struct Cyc_List_List _tmpD8;int _tmpD9;struct Cyc_List_List*
_tmpDA;struct Cyc_List_List _tmpDB;int _tmpDC;struct Cyc_List_List*_tmpDD;struct Cyc_List_List
_tmpDE;int _tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_List_List _tmpE1;int _tmpE2;
struct Cyc_List_List*_tmpE3;_LL96: if(_tmpD7 != 0)goto _LL98;_LL97: t=Cyc_Absyn_sint_typ;
goto _LL95;_LL98: if(_tmpD7 == 0)goto _LL9A;_tmpD8=*_tmpD7;_tmpD9=(int)_tmpD8.hd;
_tmpDA=_tmpD8.tl;if(_tmpDA != 0)goto _LL9A;if(!(_tmpD9 == 'l'))goto _LL9A;_LL99: t=
Cyc_Absyn_slong_typ;goto _LL95;_LL9A: if(_tmpD7 == 0)goto _LL9C;_tmpDB=*_tmpD7;
_tmpDC=(int)_tmpDB.hd;_tmpDD=_tmpDB.tl;if(_tmpDD != 0)goto _LL9C;if(!(_tmpDC == 'h'))
goto _LL9C;_LL9B: t=Cyc_Absyn_sshort_typ;goto _LL95;_LL9C: if(_tmpD7 == 0)goto _LL9E;
_tmpDE=*_tmpD7;_tmpDF=(int)_tmpDE.hd;_tmpE0=_tmpDE.tl;if(_tmpE0 == 0)goto _LL9E;
_tmpE1=*_tmpE0;_tmpE2=(int)_tmpE1.hd;_tmpE3=_tmpE1.tl;if(_tmpE3 != 0)goto _LL9E;
if(!(_tmpDF == 'h'  && _tmpE2 == 'h'))goto _LL9E;_LL9D: t=Cyc_Absyn_schar_typ;goto
_LL95;_LL9E:;_LL9F:{const char*_tmp1B3;void*_tmp1B2[2];struct Cyc_String_pa_struct
_tmp1B1;struct Cyc_Int_pa_struct _tmp1B0;(_tmp1B0.tag=1,((_tmp1B0.f1=(
unsigned long)((int)_tmpCC),((_tmp1B1.tag=0,((_tmp1B1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_implode(_tmpCB)),((_tmp1B2[0]=& _tmp1B1,((_tmp1B2[1]=&
_tmp1B0,Cyc_Tcutil_terr(loc,((_tmp1B3="length modifier '%s' is not allowed with %%%c",
_tag_dyneither(_tmp1B3,sizeof(char),46))),_tag_dyneither(_tmp1B2,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmpE8=0;_npop_handler(0);return _tmpE8;}_LL95:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmpD6),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);{struct Cyc_List_List*_tmp1B4;_tmpC1=((_tmp1B4=
_region_malloc(r,sizeof(*_tmp1B4)),((_tmp1B4->hd=t,((_tmp1B4->tl=_tmpC1,_tmp1B4))))));}
break;}case 'u': _LL94: goto _LLA0;case 'o': _LLA0: goto _LLA1;case 'x': _LLA1: goto _LLA2;
case 'X': _LLA2: {struct Cyc_Core_Opt*_tmp1B5;struct Cyc_Core_Opt*_tmpEB=(_tmp1B5=
_cycalloc(sizeof(*_tmp1B5)),((_tmp1B5->v=Cyc_Tcenv_lookup_type_vars(te),_tmp1B5)));{
struct Cyc_List_List*_tmpEC=_tmpCB;struct Cyc_List_List _tmpED;int _tmpEE;struct Cyc_List_List*
_tmpEF;struct Cyc_List_List _tmpF0;int _tmpF1;struct Cyc_List_List*_tmpF2;struct Cyc_List_List
_tmpF3;int _tmpF4;struct Cyc_List_List*_tmpF5;struct Cyc_List_List _tmpF6;int _tmpF7;
struct Cyc_List_List*_tmpF8;_LLA5: if(_tmpEC != 0)goto _LLA7;_LLA6: t=Cyc_Absyn_uint_typ;
goto _LLA4;_LLA7: if(_tmpEC == 0)goto _LLA9;_tmpED=*_tmpEC;_tmpEE=(int)_tmpED.hd;
_tmpEF=_tmpED.tl;if(_tmpEF != 0)goto _LLA9;if(!(_tmpEE == 'l'))goto _LLA9;_LLA8: t=
Cyc_Absyn_ulong_typ;goto _LLA4;_LLA9: if(_tmpEC == 0)goto _LLAB;_tmpF0=*_tmpEC;
_tmpF1=(int)_tmpF0.hd;_tmpF2=_tmpF0.tl;if(_tmpF2 != 0)goto _LLAB;if(!(_tmpF1 == 'h'))
goto _LLAB;_LLAA: t=Cyc_Absyn_ushort_typ;goto _LLA4;_LLAB: if(_tmpEC == 0)goto _LLAD;
_tmpF3=*_tmpEC;_tmpF4=(int)_tmpF3.hd;_tmpF5=_tmpF3.tl;if(_tmpF5 == 0)goto _LLAD;
_tmpF6=*_tmpF5;_tmpF7=(int)_tmpF6.hd;_tmpF8=_tmpF6.tl;if(_tmpF8 != 0)goto _LLAD;
if(!(_tmpF4 == 'h'  && _tmpF7 == 'h'))goto _LLAD;_LLAC: t=Cyc_Absyn_uchar_typ;goto
_LLA4;_LLAD:;_LLAE:{const char*_tmp1BA;void*_tmp1B9[2];struct Cyc_String_pa_struct
_tmp1B8;struct Cyc_Int_pa_struct _tmp1B7;(_tmp1B7.tag=1,((_tmp1B7.f1=(
unsigned long)((int)_tmpCC),((_tmp1B8.tag=0,((_tmp1B8.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_implode(_tmpCB)),((_tmp1B9[0]=& _tmp1B8,((_tmp1B9[1]=&
_tmp1B7,Cyc_Tcutil_terr(loc,((_tmp1BA="length modifier '%s' is not allowed with %%%c",
_tag_dyneither(_tmp1BA,sizeof(char),46))),_tag_dyneither(_tmp1B9,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmpFD=0;_npop_handler(0);return _tmpFD;}_LLA4:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmpEB),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);{struct Cyc_List_List*_tmp1BB;_tmpC1=((_tmp1BB=
_region_malloc(r,sizeof(*_tmp1BB)),((_tmp1BB->hd=t,((_tmp1BB->tl=_tmpC1,_tmp1BB))))));}
break;}case 'f': _LLA3: goto _LLAF;case 'F': _LLAF: goto _LLB0;case 'e': _LLB0: goto _LLB1;
case 'E': _LLB1: goto _LLB2;case 'g': _LLB2: goto _LLB3;case 'G': _LLB3: goto _LLB4;case 'a':
_LLB4: goto _LLB5;case 'A': _LLB5: {struct Cyc_Core_Opt*_tmp1BC;struct Cyc_Core_Opt*
_tmp100=(_tmp1BC=_cycalloc(sizeof(*_tmp1BC)),((_tmp1BC->v=Cyc_Tcenv_lookup_type_vars(
te),_tmp1BC)));{struct Cyc_List_List*_tmp101=_tmpCB;struct Cyc_List_List _tmp102;
int _tmp103;struct Cyc_List_List*_tmp104;_LLB8: if(_tmp101 != 0)goto _LLBA;_LLB9: t=
Cyc_Absyn_float_typ;goto _LLB7;_LLBA: if(_tmp101 == 0)goto _LLBC;_tmp102=*_tmp101;
_tmp103=(int)_tmp102.hd;_tmp104=_tmp102.tl;if(_tmp104 != 0)goto _LLBC;if(!(_tmp103
== 'l'))goto _LLBC;_LLBB: t=Cyc_Absyn_double_typ(0);goto _LLB7;_LLBC:;_LLBD:{const
char*_tmp1C1;void*_tmp1C0[2];struct Cyc_String_pa_struct _tmp1BF;struct Cyc_Int_pa_struct
_tmp1BE;(_tmp1BE.tag=1,((_tmp1BE.f1=(unsigned long)((int)_tmpCC),((_tmp1BF.tag=0,((
_tmp1BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmpCB)),((
_tmp1C0[0]=& _tmp1BF,((_tmp1C0[1]=& _tmp1BE,Cyc_Tcutil_terr(loc,((_tmp1C1="length modifier '%s' is not allowed with %%%c",
_tag_dyneither(_tmp1C1,sizeof(char),46))),_tag_dyneither(_tmp1C0,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmp109=0;_npop_handler(0);return _tmp109;}_LLB7:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmp100),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);{struct Cyc_List_List*_tmp1C2;_tmpC1=((_tmp1C2=
_region_malloc(r,sizeof(*_tmp1C2)),((_tmp1C2->hd=t,((_tmp1C2->tl=_tmpC1,_tmp1C2))))));}
break;}case 'c': _LLB6: {struct Cyc_Core_Opt*_tmp1C3;struct Cyc_Core_Opt*_tmp10C=(
_tmp1C3=_cycalloc(sizeof(*_tmp1C3)),((_tmp1C3->v=Cyc_Tcenv_lookup_type_vars(te),
_tmp1C3)));{struct Cyc_List_List*_tmp1C4;_tmpC1=((_tmp1C4=_region_malloc(r,
sizeof(*_tmp1C4)),((_tmp1C4->hd=Cyc_Absyn_dyneither_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmp10C),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((
_tmp1C4->tl=_tmpC1,_tmp1C4))))));}break;}case 's': _LLBE: {struct Cyc_Core_Opt*
_tmp1C5;struct Cyc_Core_Opt*_tmp10F=(_tmp1C5=_cycalloc(sizeof(*_tmp1C5)),((
_tmp1C5->v=Cyc_Tcenv_lookup_type_vars(te),_tmp1C5)));void*ptr=Cyc_Absyn_dyneither_typ(
Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,
_tmp10F),Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);{struct Cyc_List_List*
_tmp1C6;_tmpC1=((_tmp1C6=_region_malloc(r,sizeof(*_tmp1C6)),((_tmp1C6->hd=ptr,((
_tmp1C6->tl=_tmpC1,_tmp1C6))))));}break;}case '[': _LLBF: goto _LLC0;case 'p': _LLC0:{
const char*_tmp1CA;void*_tmp1C9[1];struct Cyc_Int_pa_struct _tmp1C8;(_tmp1C8.tag=1,((
_tmp1C8.f1=(unsigned long)((int)_tmpCC),((_tmp1C9[0]=& _tmp1C8,Cyc_Tcutil_terr(
loc,((_tmp1CA="%%%c is not supported",_tag_dyneither(_tmp1CA,sizeof(char),22))),
_tag_dyneither(_tmp1C9,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp115=0;
_npop_handler(0);return _tmp115;}case 'n': _LLC1: {struct Cyc_Core_Opt*_tmp1CB;
struct Cyc_Core_Opt*_tmp116=(_tmp1CB=_cycalloc(sizeof(*_tmp1CB)),((_tmp1CB->v=Cyc_Tcenv_lookup_type_vars(
te),_tmp1CB)));{struct Cyc_List_List*_tmp117=_tmpCB;struct Cyc_List_List _tmp118;
int _tmp119;struct Cyc_List_List*_tmp11A;struct Cyc_List_List _tmp11B;int _tmp11C;
struct Cyc_List_List*_tmp11D;struct Cyc_List_List _tmp11E;int _tmp11F;struct Cyc_List_List*
_tmp120;struct Cyc_List_List _tmp121;int _tmp122;struct Cyc_List_List*_tmp123;_LLC4:
if(_tmp117 != 0)goto _LLC6;_LLC5: t=Cyc_Absyn_sint_typ;goto _LLC3;_LLC6: if(_tmp117 == 
0)goto _LLC8;_tmp118=*_tmp117;_tmp119=(int)_tmp118.hd;_tmp11A=_tmp118.tl;if(
_tmp11A != 0)goto _LLC8;if(!(_tmp119 == 'l'))goto _LLC8;_LLC7: t=Cyc_Absyn_ulong_typ;
goto _LLC3;_LLC8: if(_tmp117 == 0)goto _LLCA;_tmp11B=*_tmp117;_tmp11C=(int)_tmp11B.hd;
_tmp11D=_tmp11B.tl;if(_tmp11D != 0)goto _LLCA;if(!(_tmp11C == 'h'))goto _LLCA;_LLC9:
t=Cyc_Absyn_sshort_typ;goto _LLC3;_LLCA: if(_tmp117 == 0)goto _LLCC;_tmp11E=*_tmp117;
_tmp11F=(int)_tmp11E.hd;_tmp120=_tmp11E.tl;if(_tmp120 == 0)goto _LLCC;_tmp121=*
_tmp120;_tmp122=(int)_tmp121.hd;_tmp123=_tmp121.tl;if(_tmp123 != 0)goto _LLCC;if(!(
_tmp11F == 'h'  && _tmp122 == 'h'))goto _LLCC;_LLCB: t=Cyc_Absyn_schar_typ;goto _LLC3;
_LLCC:;_LLCD:{const char*_tmp1D0;void*_tmp1CF[2];struct Cyc_String_pa_struct
_tmp1CE;struct Cyc_Int_pa_struct _tmp1CD;(_tmp1CD.tag=1,((_tmp1CD.f1=(
unsigned long)((int)_tmpCC),((_tmp1CE.tag=0,((_tmp1CE.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_implode(_tmpCB)),((_tmp1CF[0]=& _tmp1CE,((_tmp1CF[1]=&
_tmp1CD,Cyc_Tcutil_terr(loc,((_tmp1D0="length modifier '%s' is not allowed with %%%c",
_tag_dyneither(_tmp1D0,sizeof(char),46))),_tag_dyneither(_tmp1CF,sizeof(void*),2)))))))))))));}{
struct Cyc_List_List*_tmp128=0;_npop_handler(0);return _tmp128;}_LLC3:;}t=Cyc_Absyn_at_typ(
t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmp116),Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);{struct Cyc_List_List*_tmp1D1;_tmpC1=((_tmp1D1=
_region_malloc(r,sizeof(*_tmp1D1)),((_tmp1D1->hd=t,((_tmp1D1->tl=_tmpC1,_tmp1D1))))));}
break;}case '%': _LLC2: if(_tmpC9){{const char*_tmp1D4;void*_tmp1D3;(_tmp1D3=0,Cyc_Tcutil_terr(
loc,((_tmp1D4="Assignment suppression (*) is not allowed with %%%%",
_tag_dyneither(_tmp1D4,sizeof(char),52))),_tag_dyneither(_tmp1D3,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp12D=0;_npop_handler(0);return _tmp12D;}}if(_tmpCA != 0){{
const char*_tmp1D8;void*_tmp1D7[1];struct Cyc_String_pa_struct _tmp1D6;(_tmp1D6.tag=
0,((_tmp1D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmpCA)),((
_tmp1D7[0]=& _tmp1D6,Cyc_Tcutil_terr(loc,((_tmp1D8="width '%s' not allowed with %%%%",
_tag_dyneither(_tmp1D8,sizeof(char),33))),_tag_dyneither(_tmp1D7,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp131=0;_npop_handler(0);return _tmp131;}}if(_tmpCB != 0){{
const char*_tmp1DC;void*_tmp1DB[1];struct Cyc_String_pa_struct _tmp1DA;(_tmp1DA.tag=
0,((_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_implode(_tmpCB)),((
_tmp1DB[0]=& _tmp1DA,Cyc_Tcutil_terr(loc,((_tmp1DC="length modifier '%s' not allowed with %%%%",
_tag_dyneither(_tmp1DC,sizeof(char),43))),_tag_dyneither(_tmp1DB,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp135=0;_npop_handler(0);return _tmp135;}}break;default:
_LLCE: {struct Cyc_List_List*_tmp136=0;_npop_handler(0);return _tmp136;}}}}}}{
struct Cyc_List_List*_tmp137=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpC1);_npop_handler(0);return _tmp137;};_pop_region(temp);}
