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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);int
Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_rlist(
struct _RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*
tag;};struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
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
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);
struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct
Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**
ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,void*);
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(
struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(
struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct
Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};struct
Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};int Cyc_Position_error_p();struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
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
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
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
char*tag;};void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);int Cyc_Absyn_is_nontagged_union_type(void*);
int Cyc_Absyn_is_aggr_type(void*t);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
struct _RegionHandle*,void*t);int Cyc_Tcutil_is_noreturn(void*);struct _tuple10{
unsigned int f1;int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_UniquePlace{struct
Cyc_CfFlowInfo_Place place;struct Cyc_List_List*path;};enum Cyc_CfFlowInfo_InitLevel{
Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple11{struct Cyc_Absyn_Vardecl*
f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct _tuple11 val;};struct
_union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct
_union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;};union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessVar(struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp
Cyc_CfFlowInfo_LessConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*);struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;
union Cyc_CfFlowInfo_RelnOp rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop
cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[11];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};extern struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;
struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
_union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct
_union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);union Cyc_CfFlowInfo_AbsLVal
Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_Zero_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_struct{
int tag;};struct Cyc_CfFlowInfo_NotZeroThis_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_struct{
int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_struct{int tag;
enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;
struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;
struct _dyneither_ptr f2;};int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*
set,void*place,struct Cyc_Position_Segment*loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_CfFlowInfo_and_consume(struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo
c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);int Cyc_CfFlowInfo_consume_approx(struct
Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2);struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple12{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple12 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*,struct Cyc_CfFlowInfo_ConsumeInfo);
struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;
void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);int Cyc_CfFlowInfo_get_field_index(
void*t,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*
fs,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*
r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,
union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*
p);void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));void
Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,
int);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,
struct _tuple13 pr2,int);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo
f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_struct{
int tag;struct Cyc_Absyn_Tvar*f1;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_consume_unique_rvals(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);struct _tuple14{
struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct
_tuple14 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo
f,int clear);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_restore_consume_info(
union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo c,int
may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*
place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct
Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*
encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct
Cyc_NewControlFlow_CFAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,
struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct _tmp6EA;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp6E9;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp6E8;enclosee->annot=(void*)((_tmp6E8=_cycalloc(sizeof(*_tmp6E8)),((_tmp6E8[0]=((
_tmp6EA.tag=Cyc_NewControlFlow_CFAnnot,((_tmp6EA.f1=((_tmp6E9.encloser=encloser,((
_tmp6E9.visited=0,_tmp6E9)))),_tmp6EA)))),_tmp6E8))));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple15{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_AbsLVal f2;};static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;};static struct _tuple16 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){void*_tmp3=s->annot;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp5;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_struct*_tmp4=(struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3;if(_tmp4->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp5=_tmp4->f1;
_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp4->f1;}}_LL2: return _tmp6;_LL3:;
_LL4: {struct Cyc_Core_Impossible_struct _tmp6F0;const char*_tmp6EF;struct Cyc_Core_Impossible_struct*
_tmp6EE;(int)_throw((void*)((_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((_tmp6EE[0]=((
_tmp6F0.tag=Cyc_Core_Impossible,((_tmp6F0.f1=((_tmp6EF="ControlFlow -- wrong stmt annotation",
_tag_dyneither(_tmp6EF,sizeof(char),37))),_tmp6F0)))),_tmp6EE)))));}_LL0:;}
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo**
sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*
key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo*
_tmp6F1;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp6F1=_region_malloc(env->r,sizeof(*
_tmp6F1)),((_tmp6F1[0]=Cyc_CfFlowInfo_BottomFL(),_tmp6F1)));((void(*)(struct Cyc_Hashtable_Table*
t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(
env->flow_table,s,res);return res;}return*sflow;}struct _tuple17{struct Cyc_NewControlFlow_CFStmtAnnot*
f1;union Cyc_CfFlowInfo_FlowInfo*f2;};static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Stmt*s);static struct _tuple17 Cyc_NewControlFlow_pre_stmt_check(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*
s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*
_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC,1);_tmpB->visited=
env->iteration_num;{struct _tuple17 _tmp6F2;return(_tmp6F2.f1=_tmpB,((_tmp6F2.f2=
_tmpC,_tmp6F2)));};}static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo new_flow);static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){if(
env->in_try){env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
new_flow,env->tryflow,1);{struct Cyc_CfFlowInfo_ConsumeInfo _tmpF;struct Cyc_Dict_Dict
_tmp10;struct Cyc_List_List*_tmp11;struct _tuple14 _tmpE=Cyc_CfFlowInfo_save_consume_info(
env->fenv,env->tryflow,0);_tmpF=_tmpE.f1;_tmp10=_tmpF.consumed;_tmp11=_tmpF.may_consume;{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp6F3;env->tryflow=Cyc_CfFlowInfo_restore_consume_info(
env->tryflow,((_tmp6F3.consumed=_tmp10,((_tmp6F3.may_consume=0,_tmp6F3)))),0);};};}}
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow);static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp13=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo*_tmp14=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp15=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
flow,*_tmp14,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp15,*_tmp14)){*_tmp14=
_tmp15;if(_tmp13->visited == env->iteration_num)env->iterate_again=1;}}static
union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,
struct Cyc_Position_Segment*loc);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo
_tmp16=inflow;int _tmp17;struct _tuple12 _tmp18;struct Cyc_Dict_Dict _tmp19;struct Cyc_List_List*
_tmp1A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B;_LL6: if((_tmp16.BottomFL).tag != 1)
goto _LL8;_tmp17=(int)(_tmp16.BottomFL).val;_LL7: return Cyc_CfFlowInfo_BottomFL();
_LL8: if((_tmp16.ReachableFL).tag != 2)goto _LL5;_tmp18=(struct _tuple12)(_tmp16.ReachableFL).val;
_tmp19=_tmp18.f1;_tmp1A=_tmp18.f2;_tmp1B=_tmp18.f3;_LL9: for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_struct _tmp6F6;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp6F5;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp1C=(_tmp6F5=_region_malloc(fenv->r,
sizeof(*_tmp6F5)),((_tmp6F5[0]=((_tmp6F6.tag=0,((_tmp6F6.f1=(struct Cyc_Absyn_Vardecl*)
vds->hd,_tmp6F6)))),_tmp6F5)));_tmp19=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,void*v))Cyc_Dict_insert)(_tmp19,(void*)_tmp1C,Cyc_CfFlowInfo_typ_to_absrval(
fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return Cyc_CfFlowInfo_ReachableFL(
_tmp19,_tmp1A,_tmp1B);_LL5:;}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;struct
Cyc_Position_Segment*f2;};static void Cyc_NewControlFlow_remove_vars(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Dict_Dict*out_unique_places,
struct Cyc_Dict_Dict old_unique_places);static void Cyc_NewControlFlow_remove_vars(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Dict_Dict*
out_unique_places,struct Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp20;struct Cyc_Dict_Dict _tmp21;struct _tuple14 _tmp1F=Cyc_CfFlowInfo_save_consume_info(
fenv,outflow,0);_tmp20=_tmp1F.f1;_tmp21=_tmp20.consumed;{struct _RegionHandle*
_tmp22=fenv->r;{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct
_RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(_tmp22,*((struct Cyc_Dict_Dict*)
_check_null(out_unique_places)));struct _tuple18 _tmp23=*((struct _tuple18*(*)(
struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(_tmp22,*
out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(
iter,& _tmp23)){struct Cyc_CfFlowInfo_Place*_tmp24=_tmp23.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp21,_tmp24)){
const char*_tmp6FA;void*_tmp6F9[1];struct Cyc_String_pa_struct _tmp6F8;(_tmp6F8.tag=
0,((_tmp6F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_CfFlowInfo_place_err_string(
_tmp24)),((_tmp6F9[0]=& _tmp6F8,Cyc_Tcutil_terr(_tmp23.f2,((_tmp6FA="Failed to consume unique variable %s",
_tag_dyneither(_tmp6FA,sizeof(char),37))),_tag_dyneither(_tmp6F9,sizeof(void*),1)))))));}}}*
out_unique_places=old_unique_places;};}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
inflow,void*r);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;int _tmp29;struct
_tuple12 _tmp2A;struct Cyc_Dict_Dict _tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2D;_LLB: if((_tmp28.BottomFL).tag != 1)goto _LLD;_tmp29=(int)(_tmp28.BottomFL).val;
_LLC: return Cyc_CfFlowInfo_BottomFL();_LLD: if((_tmp28.ReachableFL).tag != 2)goto
_LLA;_tmp2A=(struct _tuple12)(_tmp28.ReachableFL).val;_tmp2B=_tmp2A.f1;_tmp2C=
_tmp2A.f2;_tmp2D=_tmp2A.f3;_LLE: if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp2B,r)
!= Cyc_CfFlowInfo_AllIL){const char*_tmp6FD;void*_tmp6FC;(_tmp6FC=0,Cyc_Tcutil_terr(
loc,((_tmp6FD="expression may not be fully initialized",_tag_dyneither(_tmp6FD,
sizeof(char),40))),_tag_dyneither(_tmp6FC,sizeof(void*),0)));}{struct Cyc_Dict_Dict
_tmp30=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp2B,env->all_changed,r);if(
_tmp2B.t == _tmp30.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo _tmp31=Cyc_CfFlowInfo_ReachableFL(
_tmp30,_tmp2C,_tmp2D);Cyc_NewControlFlow_update_tryflow(env,_tmp31);return _tmp31;};};
_LLA:;}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*
rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,
struct Cyc_List_List*es,int arg1_unconsumed);static struct _tuple19 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0){struct _tuple19
_tmp6FE;return(_tmp6FE.f1=inflow,((_tmp6FE.f2=0,_tmp6FE)));}if(es->tl == 0){union
Cyc_CfFlowInfo_FlowInfo _tmp34;void*_tmp35;struct _tuple13 _tmp33=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp34=_tmp33.f1;_tmp35=_tmp33.f2;{
struct Cyc_List_List*_tmp701;struct _tuple19 _tmp700;return(_tmp700.f1=_tmp34,((
_tmp700.f2=((_tmp701=_region_malloc(rgn,sizeof(*_tmp701)),((_tmp701->hd=_tmp35,((
_tmp701->tl=0,_tmp701)))))),_tmp700)));};}{struct Cyc_Dict_Dict*outer_all_changed=
env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo
old_inflow;union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_List_List*rvals;struct
Cyc_CfFlowInfo_ConsumeInfo _tmp39;union Cyc_CfFlowInfo_FlowInfo _tmp3A;struct
_tuple14 _tmp38=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,1);_tmp39=
_tmp38.f1;_tmp3A=_tmp38.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp702;struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=(_tmp702.consumed=(env->fenv)->mt_place_set,((_tmp702.may_consume=
0,_tmp702)));int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp3A=Cyc_CfFlowInfo_restore_consume_info(_tmp3A,_tmp39,0);{struct Cyc_Dict_Dict*
_tmp703;env->all_changed=((_tmp703=_region_malloc(env->r,sizeof(*_tmp703)),((
_tmp703[0]=(env->fenv)->mt_place_set,_tmp703))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp3D;void*_tmp3E;struct _tuple13 _tmp3C=Cyc_NewControlFlow_anal_Rexp(env,_tmp3A,(
struct Cyc_Absyn_Exp*)es->hd);_tmp3D=_tmp3C.f1;_tmp3E=_tmp3C.f2;outflow=_tmp3D;{
struct Cyc_List_List*_tmp704;rvals=((_tmp704=_region_malloc(rgn,sizeof(*_tmp704)),((
_tmp704->hd=_tmp3E,((_tmp704->tl=0,_tmp704))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(
arg1_unconsumed){union Cyc_CfFlowInfo_FlowInfo _tmp40=outflow;int _tmp41;struct
_tuple12 _tmp42;struct Cyc_Dict_Dict _tmp43;struct Cyc_List_List*_tmp44;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp45;_LL10: if((_tmp40.BottomFL).tag != 1)goto _LL12;_tmp41=(int)(_tmp40.BottomFL).val;
_LL11: goto _LLF;_LL12: if((_tmp40.ReachableFL).tag != 2)goto _LLF;_tmp42=(struct
_tuple12)(_tmp40.ReachableFL).val;_tmp43=_tmp42.f1;_tmp44=_tmp42.f2;_tmp45=
_tmp42.f3;_LL13: {struct Cyc_CfFlowInfo_ConsumeInfo _tmp705;outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp43,_tmp44,((_tmp705.consumed=_tmp45.consumed,((_tmp705.may_consume=_tmp39.may_consume,
_tmp705)))));}_LLF:;}{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){{
struct Cyc_Dict_Dict*_tmp706;env->all_changed=((_tmp706=_region_malloc(env->r,
sizeof(*_tmp706)),((_tmp706[0]=(env->fenv)->mt_place_set,_tmp706))));}{union Cyc_CfFlowInfo_FlowInfo
_tmp49;void*_tmp4A;struct _tuple13 _tmp48=Cyc_NewControlFlow_anal_Rexp(env,_tmp3A,(
struct Cyc_Absyn_Exp*)es2->hd);_tmp49=_tmp48.f1;_tmp4A=_tmp48.f2;{struct Cyc_List_List*
_tmp707;rvals=((_tmp707=_region_malloc(rgn,sizeof(*_tmp707)),((_tmp707->hd=
_tmp4A,((_tmp707->tl=rvals,_tmp707))))));}outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(
struct Cyc_Dict_Dict*)& this_all_changed,outflow,_tmp49,this_all_changed,*((struct
Cyc_Dict_Dict*)_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp4D;struct _tuple14 _tmp4C=Cyc_CfFlowInfo_save_consume_info(
env->fenv,outflow,0);_tmp4D=_tmp4C.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
outflow_consume,_tmp4D)){{const char*_tmp70A;void*_tmp709;(_tmp709=0,Cyc_fprintf(
Cyc_stderr,((_tmp70A="sanity consumed: ",_tag_dyneither(_tmp70A,sizeof(char),18))),
_tag_dyneither(_tmp709,sizeof(void*),0)));}((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,
Cyc_CfFlowInfo_print_place);{const char*_tmp70D;void*_tmp70C;(_tmp70C=0,Cyc_fprintf(
Cyc_stderr,((_tmp70D="\ncurrent consumed: ",_tag_dyneither(_tmp70D,sizeof(char),
20))),_tag_dyneither(_tmp70C,sizeof(void*),0)));}((void(*)(struct Cyc_Dict_Dict p,
void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp4D.consumed,
Cyc_CfFlowInfo_print_place);{const char*_tmp710;void*_tmp70F;(_tmp70F=0,Cyc_fprintf(
Cyc_stderr,((_tmp710="\nsanity may_consume: ",_tag_dyneither(_tmp710,sizeof(char),
22))),_tag_dyneither(_tmp70F,sizeof(void*),0)));}((void(*)(struct Cyc_List_List*p,
void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(
outflow_consume.may_consume,Cyc_CfFlowInfo_print_place);{const char*_tmp713;void*
_tmp712;(_tmp712=0,Cyc_fprintf(Cyc_stderr,((_tmp713="\ncurrent may_consume: ",
_tag_dyneither(_tmp713,sizeof(char),23))),_tag_dyneither(_tmp712,sizeof(void*),0)));}((
void(*)(struct Cyc_List_List*p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(
_tmp4D.may_consume,Cyc_CfFlowInfo_print_place);{const char*_tmp716;void*_tmp715;(
_tmp715=0,Cyc_fprintf(Cyc_stderr,((_tmp716="\n",_tag_dyneither(_tmp716,sizeof(
char),2))),_tag_dyneither(_tmp715,sizeof(void*),0)));}{const char*_tmp719;void*
_tmp718;(_tmp718=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp719="anal_unordered_exps failed to preserve consumed info",_tag_dyneither(
_tmp719,sizeof(char),53))),_tag_dyneither(_tmp718,sizeof(void*),0)));};}
old_inflow=Cyc_CfFlowInfo_restore_consume_info(_tmp3A,outflow_consume,0);}else{
old_inflow=_tmp3A;}init_consume=1;outflow_consume=_tmp4D;_tmp3A=Cyc_CfFlowInfo_join_flow(
env->fenv,outer_all_changed,_tmp3A,outflow,1);};};}while(!Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3A,old_inflow));if(outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*
_tmp71A;env->all_changed=((_tmp71A=_region_malloc(env->r,sizeof(*_tmp71A)),((
_tmp71A[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),
_tmp71A))));}Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple19
_tmp71B;return(_tmp71B.f1=outflow,((_tmp71B.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp71B)));};};};}static struct _tuple13
Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_List_List*es,int arg1_unconsumed);static struct _tuple13 Cyc_NewControlFlow_anal_use_ints(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle*_tmp5D=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp5F;struct Cyc_List_List*_tmp60;struct _tuple19 _tmp5E=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp5D,env,inflow,es,arg1_unconsumed);_tmp5F=_tmp5E.f1;_tmp60=_tmp5E.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp61=_tmp5F;struct _tuple12 _tmp62;struct Cyc_Dict_Dict
_tmp63;int _tmp64;_LL15: if((_tmp61.ReachableFL).tag != 2)goto _LL17;_tmp62=(struct
_tuple12)(_tmp61.ReachableFL).val;_tmp63=_tmp62.f1;_LL16: for(0;_tmp60 != 0;(
_tmp60=_tmp60->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp63,(void*)_tmp60->hd)== Cyc_CfFlowInfo_NoneIL){const char*_tmp71E;
void*_tmp71D;(_tmp71D=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp71E="expression may not be initialized",
_tag_dyneither(_tmp71E,sizeof(char),34))),_tag_dyneither(_tmp71D,sizeof(void*),0)));}}
goto _LL14;_LL17: if((_tmp61.BottomFL).tag != 1)goto _LL14;_tmp64=(int)(_tmp61.BottomFL).val;
_LL18: goto _LL14;_LL14:;}{struct _tuple13 _tmp71F;return(_tmp71F.f1=_tmp5F,((
_tmp71F.f2=(env->fenv)->unknown_all,_tmp71F)));};}static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel
il);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo
_tmp68=outflow;int _tmp69;struct _tuple12 _tmp6A;struct Cyc_Dict_Dict _tmp6B;struct
Cyc_List_List*_tmp6C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp6D;_LL1A: if((_tmp68.BottomFL).tag
!= 1)goto _LL1C;_tmp69=(int)(_tmp68.BottomFL).val;_LL1B: return outflow;_LL1C: if((
_tmp68.ReachableFL).tag != 2)goto _LL19;_tmp6A=(struct _tuple12)(_tmp68.ReachableFL).val;
_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;_tmp6D=_tmp6A.f3;_LL1D: {union Cyc_CfFlowInfo_AbsLVal
_tmp6E=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;int _tmp6F;struct Cyc_CfFlowInfo_Place*
_tmp70;_LL1F: if((_tmp6E.UnknownL).tag != 2)goto _LL21;_tmp6F=(int)(_tmp6E.UnknownL).val;
_LL20: return outflow;_LL21: if((_tmp6E.PlaceL).tag != 1)goto _LL1E;_tmp70=(struct Cyc_CfFlowInfo_Place*)(
_tmp6E.PlaceL).val;_LL22: {void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(
env->fenv)->notzerothis;union Cyc_CfFlowInfo_FlowInfo _tmp71=Cyc_CfFlowInfo_ReachableFL(
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6B,env->all_changed,_tmp70,
nzval),_tmp6C,_tmp6D);return _tmp71;}_LL1E:;}_LL19:;}static struct _tuple16 Cyc_NewControlFlow_splitzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il);static struct
_tuple16 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*
e,enum Cyc_CfFlowInfo_InitLevel il){union Cyc_CfFlowInfo_FlowInfo _tmp72=outflow;
int _tmp73;struct _tuple12 _tmp74;struct Cyc_Dict_Dict _tmp75;struct Cyc_List_List*
_tmp76;struct Cyc_CfFlowInfo_ConsumeInfo _tmp77;_LL24: if((_tmp72.BottomFL).tag != 1)
goto _LL26;_tmp73=(int)(_tmp72.BottomFL).val;_LL25: {struct _tuple16 _tmp720;return(
_tmp720.f1=outflow,((_tmp720.f2=outflow,_tmp720)));}_LL26: if((_tmp72.ReachableFL).tag
!= 2)goto _LL23;_tmp74=(struct _tuple12)(_tmp72.ReachableFL).val;_tmp75=_tmp74.f1;
_tmp76=_tmp74.f2;_tmp77=_tmp74.f3;_LL27: {union Cyc_CfFlowInfo_AbsLVal _tmp79=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;int _tmp7A;struct Cyc_CfFlowInfo_Place*_tmp7B;_LL29: if((_tmp79.UnknownL).tag
!= 2)goto _LL2B;_tmp7A=(int)(_tmp79.UnknownL).val;_LL2A: {struct _tuple16 _tmp721;
return(_tmp721.f1=outflow,((_tmp721.f2=outflow,_tmp721)));}_LL2B: if((_tmp79.PlaceL).tag
!= 1)goto _LL28;_tmp7B=(struct Cyc_CfFlowInfo_Place*)(_tmp79.PlaceL).val;_LL2C: {
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
struct _tuple16 _tmp722;return(_tmp722.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp75,env->all_changed,_tmp7B,nzval),_tmp76,_tmp77),((_tmp722.f2=
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp75,
env->all_changed,_tmp7B,(env->fenv)->zero),_tmp76,_tmp77),_tmp722)));}_LL28:;}
_LL23:;}struct _tuple20{enum Cyc_Absyn_Primop f1;enum Cyc_Absyn_Primop f2;};static
union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo flow,struct Cyc_Absyn_Exp*e1,void*r1,enum Cyc_Absyn_Primop
p,void*r2);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_if_tagcmp(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow,struct Cyc_Absyn_Exp*
e1,void*r1,enum Cyc_Absyn_Primop p,void*r2){union Cyc_CfFlowInfo_FlowInfo _tmp7E=
flow;int _tmp7F;struct _tuple12 _tmp80;struct Cyc_Dict_Dict _tmp81;struct Cyc_List_List*
_tmp82;struct Cyc_CfFlowInfo_ConsumeInfo _tmp83;_LL2E: if((_tmp7E.BottomFL).tag != 1)
goto _LL30;_tmp7F=(int)(_tmp7E.BottomFL).val;_LL2F: return flow;_LL30: if((_tmp7E.ReachableFL).tag
!= 2)goto _LL2D;_tmp80=(struct _tuple12)(_tmp7E.ReachableFL).val;_tmp81=_tmp80.f1;
_tmp82=_tmp80.f2;_tmp83=_tmp80.f3;_LL31: {void*_tmp84=r2;struct Cyc_List_List*
_tmp86;_LL33: {struct Cyc_CfFlowInfo_TagCmps_struct*_tmp85=(struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp84;if(_tmp85->tag != 6)goto _LL35;else{_tmp86=_tmp85->f1;}}_LL34: {union Cyc_CfFlowInfo_AbsLVal
_tmp87=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;int _tmp88;struct Cyc_CfFlowInfo_Place*
_tmp89;_LL38: if((_tmp87.UnknownL).tag != 2)goto _LL3A;_tmp88=(int)(_tmp87.UnknownL).val;
_LL39: return flow;_LL3A: if((_tmp87.PlaceL).tag != 1)goto _LL37;_tmp89=(struct Cyc_CfFlowInfo_Place*)(
_tmp87.PlaceL).val;_LL3B: {struct Cyc_List_List*new_cl;{void*_tmp8A=r1;struct Cyc_List_List*
_tmp8C;enum Cyc_CfFlowInfo_InitLevel _tmp8E;_LL3D: {struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp8B=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp8A;if(_tmp8B->tag != 6)goto _LL3F;
else{_tmp8C=_tmp8B->f1;}}_LL3E: new_cl=_tmp8C;goto _LL3C;_LL3F: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp8D=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp8A;if(_tmp8D->tag != 3)goto
_LL41;else{_tmp8E=_tmp8D->f1;if(_tmp8E != Cyc_CfFlowInfo_AllIL)goto _LL41;}}_LL40:
goto _LL42;_LL41: {struct Cyc_CfFlowInfo_Zero_struct*_tmp8F=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp8A;if(_tmp8F->tag != 0)goto _LL43;}_LL42: goto _LL44;_LL43: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp90=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp8A;if(_tmp90->tag != 1)goto
_LL45;}_LL44: new_cl=0;goto _LL3C;_LL45:;_LL46: return flow;_LL3C:;}for(0;_tmp86 != 0;
_tmp86=_tmp86->tl){enum Cyc_Absyn_Primop new_cmp;{struct _tuple20 _tmp723;struct
_tuple20 _tmp92=(_tmp723.f1=p,((_tmp723.f2=((struct Cyc_CfFlowInfo_TagCmp*)_tmp86->hd)->cmp,
_tmp723)));enum Cyc_Absyn_Primop _tmp93;enum Cyc_Absyn_Primop _tmp94;enum Cyc_Absyn_Primop
_tmp95;enum Cyc_Absyn_Primop _tmp96;_LL48: _tmp93=_tmp92.f1;if(_tmp93 != Cyc_Absyn_Lt)
goto _LL4A;_LL49: goto _LL4B;_LL4A: _tmp94=_tmp92.f2;if(_tmp94 != Cyc_Absyn_Lt)goto
_LL4C;_LL4B: new_cmp=Cyc_Absyn_Lt;goto _LL47;_LL4C: _tmp95=_tmp92.f1;if(_tmp95 != 
Cyc_Absyn_Eq)goto _LL4E;_tmp96=_tmp92.f2;if(_tmp96 != Cyc_Absyn_Eq)goto _LL4E;_LL4D:
new_cmp=Cyc_Absyn_Eq;goto _LL47;_LL4E:;_LL4F: new_cmp=Cyc_Absyn_Lte;goto _LL47;
_LL47:;}{struct Cyc_CfFlowInfo_TagCmp*_tmp726;struct Cyc_List_List*_tmp725;new_cl=((
_tmp725=_region_malloc(env->r,sizeof(*_tmp725)),((_tmp725->hd=((_tmp726=
_region_malloc(env->r,sizeof(*_tmp726)),((_tmp726->cmp=new_cmp,((_tmp726->bd=((
struct Cyc_CfFlowInfo_TagCmp*)_tmp86->hd)->bd,_tmp726)))))),((_tmp725->tl=new_cl,
_tmp725))))));};}{struct Cyc_CfFlowInfo_TagCmps_struct _tmp729;struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp728;return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,
e1->loc,_tmp81,env->all_changed,_tmp89,(void*)((_tmp728=_region_malloc(env->r,
sizeof(*_tmp728)),((_tmp728[0]=((_tmp729.tag=6,((_tmp729.f1=new_cl,_tmp729)))),
_tmp728))))),_tmp82,_tmp83);};}_LL37:;}_LL35:;_LL36: return flow;_LL32:;}_LL2D:;}
static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,
0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple13 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r);static struct _tuple13 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo
f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp9D=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp9F;void*
_tmpA0;struct Cyc_Absyn_PtrAtts _tmpA1;union Cyc_Absyn_Constraint*_tmpA2;union Cyc_Absyn_Constraint*
_tmpA3;_LL51: {struct Cyc_Absyn_PointerType_struct*_tmp9E=(struct Cyc_Absyn_PointerType_struct*)
_tmp9D;if(_tmp9E->tag != 5)goto _LL53;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9F.elt_typ;
_tmpA1=_tmp9F.ptr_atts;_tmpA2=_tmpA1.bounds;_tmpA3=_tmpA1.zero_term;}}_LL52: {
union Cyc_CfFlowInfo_FlowInfo _tmpA4=f;int _tmpA5;struct _tuple12 _tmpA6;struct Cyc_Dict_Dict
_tmpA7;struct Cyc_List_List*_tmpA8;_LL56: if((_tmpA4.BottomFL).tag != 1)goto _LL58;
_tmpA5=(int)(_tmpA4.BottomFL).val;_LL57: {struct _tuple13 _tmp72A;return(_tmp72A.f1=
f,((_tmp72A.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA0,(env->fenv)->unknown_all),
_tmp72A)));}_LL58: if((_tmpA4.ReachableFL).tag != 2)goto _LL55;_tmpA6=(struct
_tuple12)(_tmpA4.ReachableFL).val;_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;_LL59: if(Cyc_Tcutil_is_bound_one(
_tmpA2)){void*_tmpAA=r;struct Cyc_CfFlowInfo_Place*_tmpAE;enum Cyc_CfFlowInfo_InitLevel
_tmpB1;_LL5B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmpAB=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmpAA;if(_tmpAB->tag != 1)goto _LL5D;}_LL5C: goto _LL5E;_LL5D: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmpAC=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmpAA;if(_tmpAC->tag != 2)goto
_LL5F;}_LL5E:{void*_tmpB2=e->annot;struct Cyc_List_List*_tmpB4;_LL68: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmpB3=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmpB2;if(_tmpB3->tag != Cyc_CfFlowInfo_NotZero)
goto _LL6A;else{_tmpB4=_tmpB3->f1;}}_LL69: if(!Cyc_CfFlowInfo_same_relns(_tmpA8,
_tmpB4))goto _LL6B;goto _LL67;_LL6A:;_LL6B:{void*_tmpB5=e->r;_LL6D: {struct Cyc_Absyn_Subscript_e_struct*
_tmpB6=(struct Cyc_Absyn_Subscript_e_struct*)_tmpB5;if(_tmpB6->tag != 25)goto _LL6F;}
_LL6E:{struct Cyc_CfFlowInfo_NotZero_struct _tmp72D;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp72C;e->annot=(void*)((_tmp72C=_cycalloc(sizeof(*_tmp72C)),((_tmp72C[0]=((
_tmp72D.tag=Cyc_CfFlowInfo_NotZero,((_tmp72D.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA8),_tmp72D)))),_tmp72C))));}goto _LL6C;_LL6F:;_LL70: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL6C;_LL6C:;}goto _LL67;_LL67:;}goto _LL5A;_LL5F: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmpAD=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpAA;if(_tmpAD->tag != 5)goto
_LL61;else{_tmpAE=_tmpAD->f1;}}_LL60:{void*_tmpB9=e->annot;struct Cyc_List_List*
_tmpBB;_LL72: {struct Cyc_CfFlowInfo_NotZero_struct*_tmpBA=(struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpB9;if(_tmpBA->tag != Cyc_CfFlowInfo_NotZero)goto _LL74;else{_tmpBB=_tmpBA->f1;}}
_LL73: if(!Cyc_CfFlowInfo_same_relns(_tmpA8,_tmpBB))goto _LL75;goto _LL71;_LL74:;
_LL75:{void*_tmpBC=e->r;_LL77: {struct Cyc_Absyn_Subscript_e_struct*_tmpBD=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpBC;if(_tmpBD->tag != 25)goto _LL79;}_LL78:{
struct Cyc_CfFlowInfo_NotZero_struct _tmp730;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp72F;e->annot=(void*)((_tmp72F=_cycalloc(sizeof(*_tmp72F)),((_tmp72F[0]=((
_tmp730.tag=Cyc_CfFlowInfo_NotZero,((_tmp730.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA8),_tmp730)))),_tmp72F))));}goto _LL76;_LL79:;_LL7A: e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL76;_LL76:;}goto _LL71;_LL71:;}{struct _tuple13 _tmp731;return(_tmp731.f1=f,((
_tmp731.f2=Cyc_CfFlowInfo_lookup_place(_tmpA7,_tmpAE),_tmp731)));};_LL61: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpAF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpAA;if(_tmpAF->tag != 0)goto _LL63;}_LL62: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple13 _tmp732;return(_tmp732.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp732.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA0,(env->fenv)->unknown_all),_tmp732)));};
_LL63: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpB0=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpAA;if(_tmpB0->tag != 3)goto _LL65;else{_tmpB1=_tmpB0->f1;}}_LL64: f=Cyc_NewControlFlow_notzero(
env,inflow,f,e,_tmpB1);goto _LL66;_LL65:;_LL66:{void*_tmpC2=e->r;_LL7C: {struct
Cyc_Absyn_Subscript_e_struct*_tmpC3=(struct Cyc_Absyn_Subscript_e_struct*)_tmpC2;
if(_tmpC3->tag != 25)goto _LL7E;}_LL7D:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp735;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp734;e->annot=(void*)((_tmp734=
_cycalloc(sizeof(*_tmp734)),((_tmp734[0]=((_tmp735.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp735.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmpA8),_tmp735)))),
_tmp734))));}goto _LL7B;_LL7E:;_LL7F: e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL7B;_LL7B:;}goto _LL5A;_LL5A:;}else{void*_tmpC6=e->annot;struct Cyc_List_List*
_tmpC8;_LL81: {struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpC7=(struct Cyc_CfFlowInfo_UnknownZ_struct*)
_tmpC6;if(_tmpC7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL83;else{_tmpC8=_tmpC7->f1;}}
_LL82: if(!Cyc_CfFlowInfo_same_relns(_tmpA8,_tmpC8))goto _LL84;goto _LL80;_LL83:;
_LL84:{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp738;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp737;e->annot=(void*)((_tmp737=_cycalloc(sizeof(*_tmp737)),((_tmp737[0]=((
_tmp738.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp738.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmpA8),_tmp738)))),_tmp737))));}goto _LL80;_LL80:;}switch(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmpA7,r)){case Cyc_CfFlowInfo_NoneIL: _LL85:{const char*_tmp73B;void*
_tmp73A;(_tmp73A=0,Cyc_Tcutil_terr(e->loc,((_tmp73B="dereference of possibly uninitialized pointer",
_tag_dyneither(_tmp73B,sizeof(char),46))),_tag_dyneither(_tmp73A,sizeof(void*),0)));}
goto _LL86;case Cyc_CfFlowInfo_AllIL: _LL86: {struct _tuple13 _tmp73C;return(_tmp73C.f1=
f,((_tmp73C.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA0,(env->fenv)->unknown_all),
_tmp73C)));}case Cyc_CfFlowInfo_ThisIL: _LL87: {struct _tuple13 _tmp73D;return(
_tmp73D.f1=f,((_tmp73D.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpA0,(env->fenv)->unknown_none),
_tmp73D)));}}_LL55:;}_LL53:;_LL54: {struct Cyc_Core_Impossible_struct _tmp743;
const char*_tmp742;struct Cyc_Core_Impossible_struct*_tmp741;(int)_throw((void*)((
_tmp741=_cycalloc(sizeof(*_tmp741)),((_tmp741[0]=((_tmp743.tag=Cyc_Core_Impossible,((
_tmp743.f1=((_tmp742="right deref of non-pointer-type",_tag_dyneither(_tmp742,
sizeof(char),32))),_tmp743)))),_tmp741)))));}_LL50:;}static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*
relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmpD2=e1->r;void*
_tmpD4;struct Cyc_Absyn_Vardecl*_tmpD6;void*_tmpD8;struct Cyc_Absyn_Vardecl*_tmpDA;
void*_tmpDC;struct Cyc_Absyn_Vardecl*_tmpDE;void*_tmpE0;struct Cyc_Absyn_Vardecl*
_tmpE2;_LL8A: {struct Cyc_Absyn_Var_e_struct*_tmpD3=(struct Cyc_Absyn_Var_e_struct*)
_tmpD2;if(_tmpD3->tag != 1)goto _LL8C;else{_tmpD4=(void*)_tmpD3->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmpD5=(struct Cyc_Absyn_Pat_b_struct*)_tmpD4;if(_tmpD5->tag != 5)goto _LL8C;else{
_tmpD6=_tmpD5->f1;}};}}_LL8B: _tmpDA=_tmpD6;goto _LL8D;_LL8C: {struct Cyc_Absyn_Var_e_struct*
_tmpD7=(struct Cyc_Absyn_Var_e_struct*)_tmpD2;if(_tmpD7->tag != 1)goto _LL8E;else{
_tmpD8=(void*)_tmpD7->f2;{struct Cyc_Absyn_Local_b_struct*_tmpD9=(struct Cyc_Absyn_Local_b_struct*)
_tmpD8;if(_tmpD9->tag != 4)goto _LL8E;else{_tmpDA=_tmpD9->f1;}};}}_LL8D: _tmpDE=
_tmpDA;goto _LL8F;_LL8E: {struct Cyc_Absyn_Var_e_struct*_tmpDB=(struct Cyc_Absyn_Var_e_struct*)
_tmpD2;if(_tmpDB->tag != 1)goto _LL90;else{_tmpDC=(void*)_tmpDB->f2;{struct Cyc_Absyn_Param_b_struct*
_tmpDD=(struct Cyc_Absyn_Param_b_struct*)_tmpDC;if(_tmpDD->tag != 3)goto _LL90;
else{_tmpDE=_tmpDD->f1;}};}}_LL8F: _tmpE2=_tmpDE;goto _LL91;_LL90: {struct Cyc_Absyn_Var_e_struct*
_tmpDF=(struct Cyc_Absyn_Var_e_struct*)_tmpD2;if(_tmpDF->tag != 1)goto _LL92;else{
_tmpE0=(void*)_tmpDF->f2;{struct Cyc_Absyn_Global_b_struct*_tmpE1=(struct Cyc_Absyn_Global_b_struct*)
_tmpE0;if(_tmpE1->tag != 1)goto _LL92;else{_tmpE2=_tmpE1->f1;}};}}_LL91: if(!_tmpE2->escapes){
void*_tmpE3=e2->r;void*_tmpE5;struct Cyc_Absyn_Vardecl*_tmpE7;void*_tmpE9;struct
Cyc_Absyn_Vardecl*_tmpEB;void*_tmpED;struct Cyc_Absyn_Vardecl*_tmpEF;void*_tmpF1;
struct Cyc_Absyn_Vardecl*_tmpF3;_LL95: {struct Cyc_Absyn_Var_e_struct*_tmpE4=(
struct Cyc_Absyn_Var_e_struct*)_tmpE3;if(_tmpE4->tag != 1)goto _LL97;else{_tmpE5=(
void*)_tmpE4->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpE6=(struct Cyc_Absyn_Pat_b_struct*)
_tmpE5;if(_tmpE6->tag != 5)goto _LL97;else{_tmpE7=_tmpE6->f1;}};}}_LL96: _tmpEB=
_tmpE7;goto _LL98;_LL97: {struct Cyc_Absyn_Var_e_struct*_tmpE8=(struct Cyc_Absyn_Var_e_struct*)
_tmpE3;if(_tmpE8->tag != 1)goto _LL99;else{_tmpE9=(void*)_tmpE8->f2;{struct Cyc_Absyn_Local_b_struct*
_tmpEA=(struct Cyc_Absyn_Local_b_struct*)_tmpE9;if(_tmpEA->tag != 4)goto _LL99;
else{_tmpEB=_tmpEA->f1;}};}}_LL98: _tmpEF=_tmpEB;goto _LL9A;_LL99: {struct Cyc_Absyn_Var_e_struct*
_tmpEC=(struct Cyc_Absyn_Var_e_struct*)_tmpE3;if(_tmpEC->tag != 1)goto _LL9B;else{
_tmpED=(void*)_tmpEC->f2;{struct Cyc_Absyn_Param_b_struct*_tmpEE=(struct Cyc_Absyn_Param_b_struct*)
_tmpED;if(_tmpEE->tag != 3)goto _LL9B;else{_tmpEF=_tmpEE->f1;}};}}_LL9A: _tmpF3=
_tmpEF;goto _LL9C;_LL9B: {struct Cyc_Absyn_Var_e_struct*_tmpF0=(struct Cyc_Absyn_Var_e_struct*)
_tmpE3;if(_tmpF0->tag != 1)goto _LL9D;else{_tmpF1=(void*)_tmpF0->f2;{struct Cyc_Absyn_Global_b_struct*
_tmpF2=(struct Cyc_Absyn_Global_b_struct*)_tmpF1;if(_tmpF2->tag != 1)goto _LL9D;
else{_tmpF3=_tmpF2->f1;}};}}_LL9C: if(!_tmpF3->escapes){int found=0;{struct Cyc_List_List*
_tmpF4=relns;for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){struct Cyc_CfFlowInfo_Reln*
_tmpF5=(struct Cyc_CfFlowInfo_Reln*)_tmpF4->hd;if(_tmpF5->vd == _tmpF3){union Cyc_CfFlowInfo_RelnOp
_tmpF6=_tmpF5->rop;struct Cyc_Absyn_Vardecl*_tmpF7;_LLA0: if((_tmpF6.LessNumelts).tag
!= 3)goto _LLA2;_tmpF7=(struct Cyc_Absyn_Vardecl*)(_tmpF6.LessNumelts).val;if(!(
_tmpF7 == _tmpE2))goto _LLA2;_LLA1: return relns;_LLA2:;_LLA3: continue;_LL9F:;}}}if(
!found){struct Cyc_CfFlowInfo_Reln*_tmp746;struct Cyc_List_List*_tmp745;return(
_tmp745=_region_malloc(rgn,sizeof(*_tmp745)),((_tmp745->hd=((_tmp746=
_region_malloc(rgn,sizeof(*_tmp746)),((_tmp746->vd=_tmpF3,((_tmp746->rop=Cyc_CfFlowInfo_LessNumelts(
_tmpE2),_tmp746)))))),((_tmp745->tl=relns,_tmp745)))));}}return relns;_LL9D:;
_LL9E: return relns;_LL94:;}else{return relns;}_LL92:;_LL93: return relns;_LL89:;}
static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_may_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,
struct Cyc_CfFlowInfo_Place*place,void*t,struct Cyc_List_List**ps);static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_may_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_CfFlowInfo_Place*place,void*t,struct
Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*_tmpFA=env->fenv;struct Cyc_List_List*
_tmpFB=Cyc_NewControlFlow_noalias_ptrs_rec(env,place,t);if(ps != 0)*ps=_tmpFB;if(
_tmpFB == 0)return cinfo;cinfo.may_consume=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(_tmpFA->r,Cyc_CfFlowInfo_place_cmp,
_tmpFB,cinfo.may_consume);return cinfo;}struct _tuple21{union Cyc_CfFlowInfo_AbsLVal
f1;union Cyc_CfFlowInfo_FlowInfo f2;};static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e){struct _RegionHandle*_tmpFC=env->r;if(Cyc_Tcutil_is_noalias_path(
_tmpFC,e)){struct Cyc_CfFlowInfo_FlowEnv*_tmpFD=env->fenv;union Cyc_CfFlowInfo_AbsLVal
_tmpFF;struct _tuple15 _tmpFE=Cyc_NewControlFlow_anal_Lexp(env,inflow,e);_tmpFF=
_tmpFE.f2;{struct _tuple21 _tmp747;struct _tuple21 _tmp101=(_tmp747.f1=_tmpFF,((
_tmp747.f2=inflow,_tmp747)));union Cyc_CfFlowInfo_AbsLVal _tmp102;struct Cyc_CfFlowInfo_Place*
_tmp103;union Cyc_CfFlowInfo_FlowInfo _tmp104;struct _tuple12 _tmp105;struct Cyc_Dict_Dict
_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_CfFlowInfo_ConsumeInfo _tmp108;
_LLA5: _tmp102=_tmp101.f1;if((_tmp102.PlaceL).tag != 1)goto _LLA7;_tmp103=(struct
Cyc_CfFlowInfo_Place*)(_tmp102.PlaceL).val;_tmp104=_tmp101.f2;if((_tmp104.ReachableFL).tag
!= 2)goto _LLA7;_tmp105=(struct _tuple12)(_tmp104.ReachableFL).val;_tmp106=_tmp105.f1;
_tmp107=_tmp105.f2;_tmp108=_tmp105.f3;_LLA6: {struct Cyc_CfFlowInfo_ConsumeInfo
_tmp109=Cyc_NewControlFlow_may_consume_place(env,_tmp108,_tmp103,(void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,0);return Cyc_CfFlowInfo_ReachableFL(
_tmp106,_tmp107,_tmp109);}_LLA7:;_LLA8:{const char*_tmp74B;void*_tmp74A[1];struct
Cyc_String_pa_struct _tmp749;(_tmp749.tag=0,((_tmp749.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp74A[0]=& _tmp749,Cyc_fprintf(
Cyc_stderr,((_tmp74B="Oops---no location for noalias_path |%s|\n",_tag_dyneither(
_tmp74B,sizeof(char),42))),_tag_dyneither(_tmp74A,sizeof(void*),1)))))));}return
inflow;_LLA4:;};}return inflow;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*loc);static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*
p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*
loc){if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
cinfo.consumed,p)){struct Cyc_CfFlowInfo_Place _tmp10E;void*_tmp10F;struct Cyc_List_List*
_tmp110;struct Cyc_CfFlowInfo_Place*_tmp10D=p;_tmp10E=*_tmp10D;_tmp10F=_tmp10E.root;
_tmp110=_tmp10E.fields;{void*rval=(env->fenv)->zero;if((((int(*)(struct Cyc_Dict_Dict
d,void*k,void**ans))Cyc_Dict_lookup_bool)(outdict,_tmp10F,& rval) && Cyc_CfFlowInfo_initlevel(
env->fenv,outdict,rval)!= Cyc_CfFlowInfo_NoneIL) && rval != (env->fenv)->zero){
void*_tmp111=_tmp10F;struct Cyc_Absyn_Vardecl*_tmp113;_LLAA: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp112=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp111;if(_tmp112->tag != 0)goto
_LLAC;else{_tmp113=_tmp112->f1;}}_LLAB: {struct _dyneither_ptr _tmp114=Cyc_Absynpp_qvar2string(
_tmp113->name);if(_tmp110 == 0){const char*_tmp74F;void*_tmp74E[1];struct Cyc_String_pa_struct
_tmp74D;(_tmp74D.tag=0,((_tmp74D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp114),((_tmp74E[0]=& _tmp74D,Cyc_Tcutil_warn(loc,((_tmp74F="may clobber unique pointer %s",
_tag_dyneither(_tmp74F,sizeof(char),30))),_tag_dyneither(_tmp74E,sizeof(void*),1)))))));}
else{const char*_tmp753;void*_tmp752[1];struct Cyc_String_pa_struct _tmp751;(
_tmp751.tag=0,((_tmp751.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp114),((
_tmp752[0]=& _tmp751,Cyc_Tcutil_warn(loc,((_tmp753="may clobber unique pointer contained in %s",
_tag_dyneither(_tmp753,sizeof(char),43))),_tag_dyneither(_tmp752,sizeof(void*),1)))))));}
goto _LLA9;}_LLAC:;_LLAD: {const char*_tmp756;void*_tmp755;(_tmp755=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp756="warning locations not for VarRoots",
_tag_dyneither(_tmp756,sizeof(char),35))),_tag_dyneither(_tmp755,sizeof(void*),0)));}
_LLA9:;}};}if(env->all_changed == 0)cinfo.consumed=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)((
env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e);static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_assignment(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Absyn_Exp*e){struct _RegionHandle*
_tmp11D=env->r;if(Cyc_Tcutil_is_noalias_path(_tmp11D,e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmp11E=env->fenv;struct Cyc_List_List*_tmp11F=0;cinfo=Cyc_NewControlFlow_may_consume_place(
env,cinfo,p,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)&
_tmp11F);while(_tmp11F != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct
Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(_tmp11F))->hd,cinfo,
outdict,e->loc);_tmp11F=((struct Cyc_List_List*)_check_null(_tmp11F))->tl;}}
return cinfo;}static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_aux(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*
ts);static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;
struct Cyc_CfFlowInfo_Place _tmp121;void*_tmp122;struct Cyc_List_List*_tmp123;
struct Cyc_CfFlowInfo_Place*_tmp120=p;_tmp121=*_tmp120;_tmp122=_tmp121.root;
_tmp123=_tmp121.fields;{int fld=0;for(0;ts != 0;(fld ++,ts=ts->tl)){void*_tmp124=(
void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(_tmp124)){int _tmp757[1];struct Cyc_List_List*
_tmp125=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_rappend)(env->r,_tmp123,((_tmp757[0]=fld,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
env->r,_tag_dyneither(_tmp757,sizeof(int),1)))));struct Cyc_CfFlowInfo_Place*
_tmp75A;struct Cyc_List_List*_tmp759;l=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,((
_tmp759=_region_malloc(env->r,sizeof(*_tmp759)),((_tmp759->hd=((_tmp75A=
_region_malloc(env->r,sizeof(*_tmp75A)),((_tmp75A->root=_tmp122,((_tmp75A->fields=
_tmp125,_tmp75A)))))),((_tmp759->tl=0,_tmp759)))))),l);}else{if(Cyc_Absyn_is_aggr_type(
_tmp124)){int _tmp75B[1];struct Cyc_List_List*_tmp129=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmp123,((_tmp75B[0]=fld,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp75B,sizeof(int),
1)))));struct Cyc_CfFlowInfo_Place*_tmp75C;struct Cyc_CfFlowInfo_Place*_tmp12A=(
_tmp75C=_region_malloc(env->r,sizeof(*_tmp75C)),((_tmp75C->root=_tmp122,((
_tmp75C->fields=_tmp129,_tmp75C)))));l=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,
l,Cyc_NewControlFlow_noalias_ptrs_rec(env,_tmp12A,_tmp124));}}}}return l;}struct
_tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);static
struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(t)){
struct Cyc_List_List*_tmp75D;return(_tmp75D=_region_malloc(env->r,sizeof(*_tmp75D)),((
_tmp75D->hd=p,((_tmp75D->tl=0,_tmp75D)))));}{void*_tmp12E=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp130;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_AggrInfo
_tmp134;union Cyc_Absyn_AggrInfoU _tmp135;struct Cyc_Absyn_Aggrdecl**_tmp136;struct
Cyc_List_List*_tmp137;struct Cyc_Absyn_AggrInfo _tmp139;union Cyc_Absyn_AggrInfoU
_tmp13A;struct _tuple3 _tmp13B;struct Cyc_Absyn_DatatypeFieldInfo _tmp13E;union Cyc_Absyn_DatatypeFieldInfoU
_tmp13F;struct Cyc_List_List*_tmp140;_LLAF: {struct Cyc_Absyn_TupleType_struct*
_tmp12F=(struct Cyc_Absyn_TupleType_struct*)_tmp12E;if(_tmp12F->tag != 11)goto
_LLB1;else{_tmp130=_tmp12F->f1;}}_LLB0: {struct Cyc_List_List*_tmp141=0;while(
_tmp130 != 0){{struct Cyc_List_List*_tmp75E;_tmp141=((_tmp75E=_region_malloc(env->r,
sizeof(*_tmp75E)),((_tmp75E->hd=(*((struct _tuple22*)_tmp130->hd)).f2,((_tmp75E->tl=
_tmp141,_tmp75E))))));}_tmp130=_tmp130->tl;}_tmp141=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp141);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp141);}_LLB1: {struct Cyc_Absyn_AnonAggrType_struct*_tmp131=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp12E;if(_tmp131->tag != 13)goto _LLB3;else{_tmp132=_tmp131->f2;}}_LLB2: {struct
Cyc_List_List*_tmp143=0;while(_tmp132 != 0){{struct Cyc_List_List*_tmp75F;_tmp143=((
_tmp75F=_region_malloc(env->r,sizeof(*_tmp75F)),((_tmp75F->hd=((struct Cyc_Absyn_Aggrfield*)
_tmp132->hd)->type,((_tmp75F->tl=_tmp143,_tmp75F))))));}_tmp132=_tmp132->tl;}
_tmp143=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp143);return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp143);}_LLB3: {
struct Cyc_Absyn_AggrType_struct*_tmp133=(struct Cyc_Absyn_AggrType_struct*)
_tmp12E;if(_tmp133->tag != 12)goto _LLB5;else{_tmp134=_tmp133->f1;_tmp135=_tmp134.aggr_info;
if((_tmp135.KnownAggr).tag != 2)goto _LLB5;_tmp136=(struct Cyc_Absyn_Aggrdecl**)(
_tmp135.KnownAggr).val;_tmp137=_tmp134.targs;}}_LLB4: if((*_tmp136)->impl == 0)
return 0;else{struct Cyc_List_List*_tmp145=0;struct _RegionHandle*_tmp146=env->r;{
struct Cyc_List_List*_tmp147=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
_tmp146,_tmp146,(*_tmp136)->tvs,_tmp137);struct Cyc_List_List*_tmp148=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((*_tmp136)->impl))->fields;while(_tmp148 != 0){{struct Cyc_List_List*
_tmp760;_tmp145=((_tmp760=_region_malloc(env->r,sizeof(*_tmp760)),((_tmp760->hd=
Cyc_Tcutil_rsubstitute(_tmp146,_tmp147,((struct Cyc_Absyn_Aggrfield*)_tmp148->hd)->type),((
_tmp760->tl=_tmp145,_tmp760))))));}_tmp148=_tmp148->tl;}}_tmp145=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp145);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp145);}_LLB5: {struct Cyc_Absyn_AggrType_struct*_tmp138=(struct Cyc_Absyn_AggrType_struct*)
_tmp12E;if(_tmp138->tag != 12)goto _LLB7;else{_tmp139=_tmp138->f1;_tmp13A=_tmp139.aggr_info;
if((_tmp13A.UnknownAggr).tag != 1)goto _LLB7;_tmp13B=(struct _tuple3)(_tmp13A.UnknownAggr).val;}}
_LLB6: {const char*_tmp763;void*_tmp762;(_tmp762=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp763="got unknown aggr in noalias_ptrs_rec",
_tag_dyneither(_tmp763,sizeof(char),37))),_tag_dyneither(_tmp762,sizeof(void*),0)));}
_LLB7: {struct Cyc_Absyn_DatatypeType_struct*_tmp13C=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp12E;if(_tmp13C->tag != 3)goto _LLB9;}_LLB8: {struct _RegionHandle*_tmp14C=env->r;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp14C,t)){struct Cyc_List_List*_tmp764;
return(_tmp764=_region_malloc(env->r,sizeof(*_tmp764)),((_tmp764->hd=p,((_tmp764->tl=
0,_tmp764)))));}else{return 0;}}_LLB9: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp13D=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp12E;if(_tmp13D->tag != 4)
goto _LLBB;else{_tmp13E=_tmp13D->f1;_tmp13F=_tmp13E.field_info;_tmp140=_tmp13E.targs;}}
_LLBA: {union Cyc_Absyn_DatatypeFieldInfoU _tmp14E=_tmp13F;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmp14F;struct _tuple2 _tmp150;struct Cyc_Absyn_Datatypedecl*_tmp151;struct Cyc_Absyn_Datatypefield*
_tmp152;_LLBE: if((_tmp14E.UnknownDatatypefield).tag != 1)goto _LLC0;_tmp14F=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp14E.UnknownDatatypefield).val;
_LLBF: {const char*_tmp767;void*_tmp766;(_tmp766=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp767="got unknown datatype field in noalias_ptrs_rec",
_tag_dyneither(_tmp767,sizeof(char),47))),_tag_dyneither(_tmp766,sizeof(void*),0)));}
_LLC0: if((_tmp14E.KnownDatatypefield).tag != 2)goto _LLBD;_tmp150=(struct _tuple2)(
_tmp14E.KnownDatatypefield).val;_tmp151=_tmp150.f1;_tmp152=_tmp150.f2;_LLC1: {
struct Cyc_List_List*_tmp155=0;struct _RegionHandle*_tmp156=env->r;{struct Cyc_List_List*
_tmp157=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp156,_tmp156,
_tmp151->tvs,_tmp140);struct Cyc_List_List*_tmp158=_tmp152->typs;while(_tmp158 != 
0){{struct Cyc_List_List*_tmp768;_tmp155=((_tmp768=_region_malloc(env->r,sizeof(*
_tmp768)),((_tmp768->hd=Cyc_Tcutil_rsubstitute(_tmp156,_tmp157,(*((struct
_tuple22*)_tmp158->hd)).f2),((_tmp768->tl=_tmp155,_tmp768))))));}_tmp158=_tmp158->tl;}}
_tmp155=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp155);return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp155);}_LLBD:;}_LLBB:;
_LLBC: return 0;_LLAE:;};}static struct _tuple13 Cyc_NewControlFlow_do_assign(struct
Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_CfFlowInfo_ConsumeInfo outer_cinfo,struct Cyc_Position_Segment*
loc);static struct _tuple13 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,
struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_CfFlowInfo_ConsumeInfo outer_cinfo,struct Cyc_Position_Segment*
loc){outflow=Cyc_CfFlowInfo_consume_unique_rvals(fenv,loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp15A=outflow;int _tmp15B;struct _tuple12 _tmp15C;struct Cyc_Dict_Dict _tmp15D;
struct Cyc_List_List*_tmp15E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp15F;_LLC3: if((
_tmp15A.BottomFL).tag != 1)goto _LLC5;_tmp15B=(int)(_tmp15A.BottomFL).val;_LLC4: {
struct _tuple13 _tmp769;return(_tmp769.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp769.f2=
rval,_tmp769)));}_LLC5: if((_tmp15A.ReachableFL).tag != 2)goto _LLC2;_tmp15C=(
struct _tuple12)(_tmp15A.ReachableFL).val;_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;
_tmp15F=_tmp15C.f3;_LLC6: _tmp15F=Cyc_CfFlowInfo_and_consume(fenv->r,outer_cinfo,
_tmp15F);{union Cyc_CfFlowInfo_AbsLVal _tmp161=lval;struct Cyc_CfFlowInfo_Place*
_tmp162;int _tmp163;_LLC8: if((_tmp161.PlaceL).tag != 1)goto _LLCA;_tmp162=(struct
Cyc_CfFlowInfo_Place*)(_tmp161.PlaceL).val;_LLC9: _tmp15F=Cyc_NewControlFlow_consume_assignment(
env,_tmp162,_tmp15F,_tmp15D,lexp);_tmp15D=Cyc_CfFlowInfo_assign_place(fenv,loc,
_tmp15D,env->all_changed,_tmp162,rval);_tmp15E=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp15E,lexp,rexp);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp15D,_tmp15E,
_tmp15F);Cyc_NewControlFlow_update_tryflow(env,outflow);{struct _tuple13 _tmp76A;
return(_tmp76A.f1=outflow,((_tmp76A.f2=rval,_tmp76A)));};_LLCA: if((_tmp161.UnknownL).tag
!= 2)goto _LLC7;_tmp163=(int)(_tmp161.UnknownL).val;_LLCB: {struct _tuple13 _tmp76B;
return(_tmp76B.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((
_tmp76B.f2=rval,_tmp76B)));}_LLC7:;};_LLC2:;};}struct _tuple23{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple13 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e);static void _tmp7FD(unsigned int*_tmp7FC,unsigned int*_tmp7FB,
void***_tmp7F9,struct Cyc_List_List**_tmp33E){for(*_tmp7FC=0;*_tmp7FC < *_tmp7FB;(*
_tmp7FC)++){void*_tmp7F7;(*_tmp7F9)[*_tmp7FC]=((_tmp7F7=(void*)((struct Cyc_List_List*)
_check_null(*_tmp33E))->hd,((*_tmp33E=(*_tmp33E)->tl,_tmp7F7))));}}static struct
_tuple13 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp166=env->fenv;struct Cyc_Dict_Dict d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo
cinfo;{union Cyc_CfFlowInfo_FlowInfo _tmp167=inflow;int _tmp168;struct _tuple12
_tmp169;struct Cyc_Dict_Dict _tmp16A;struct Cyc_List_List*_tmp16B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp16C;_LLCD: if((_tmp167.BottomFL).tag != 1)goto _LLCF;_tmp168=(int)(_tmp167.BottomFL).val;
_LLCE: {struct _tuple13 _tmp76C;return(_tmp76C.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp76C.f2=_tmp166->unknown_all,_tmp76C)));}_LLCF: if((_tmp167.ReachableFL).tag != 
2)goto _LLCC;_tmp169=(struct _tuple12)(_tmp167.ReachableFL).val;_tmp16A=_tmp169.f1;
_tmp16B=_tmp169.f2;_tmp16C=_tmp169.f3;_LLD0: d=_tmp16A;relns=_tmp16B;cinfo=
_tmp16C;_LLCC:;}{void*_tmp16E=e->r;struct Cyc_Absyn_Exp*_tmp170;enum Cyc_Absyn_Coercion
_tmp171;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*
_tmp177;union Cyc_Absyn_Cnst _tmp179;int _tmp17A;union Cyc_Absyn_Cnst _tmp17C;struct
_tuple6 _tmp17D;int _tmp17E;union Cyc_Absyn_Cnst _tmp180;struct _tuple6 _tmp181;void*
_tmp183;struct Cyc_List_List*_tmp186;void*_tmp18F;void*_tmp192;struct Cyc_Absyn_Vardecl*
_tmp194;void*_tmp196;struct Cyc_Absyn_Vardecl*_tmp198;void*_tmp19A;struct Cyc_Absyn_Vardecl*
_tmp19C;enum Cyc_Absyn_Primop _tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_Absyn_Exp*
_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Core_Opt*_tmp1A4;struct Cyc_Core_Opt
_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct Cyc_Core_Opt*
_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AA;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*
_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1B1;struct Cyc_List_List*
_tmp1B2;struct Cyc_Absyn_MallocInfo _tmp1B4;int _tmp1B5;struct Cyc_Absyn_Exp*_tmp1B6;
void**_tmp1B7;struct Cyc_Absyn_Exp*_tmp1B8;int _tmp1B9;struct Cyc_Absyn_Exp*_tmp1BB;
struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Exp*
_tmp1BF;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Exp*
_tmp1C5;struct _dyneither_ptr*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C8;struct
_dyneither_ptr*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CB;struct _dyneither_ptr*_tmp1CC;
struct Cyc_Absyn_Exp*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_Absyn_Exp*
_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_Absyn_Exp*
_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_Exp*
_tmp1D9;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Datatypedecl*_tmp1DC;struct
Cyc_List_List*_tmp1DE;struct Cyc_List_List*_tmp1E0;struct Cyc_List_List*_tmp1E2;
struct Cyc_Absyn_Aggrdecl*_tmp1E3;struct Cyc_Absyn_Aggrdecl _tmp1E4;enum Cyc_Absyn_AggrKind
_tmp1E5;struct Cyc_Absyn_AggrdeclImpl*_tmp1E6;struct Cyc_Absyn_AggrdeclImpl _tmp1E7;
struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1EB;struct Cyc_Absyn_Vardecl*
_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;int _tmp1F0;
struct Cyc_Absyn_Stmt*_tmp1F2;void*_tmp1F4;_LLD2: {struct Cyc_Absyn_Cast_e_struct*
_tmp16F=(struct Cyc_Absyn_Cast_e_struct*)_tmp16E;if(_tmp16F->tag != 15)goto _LLD4;
else{_tmp170=_tmp16F->f2;_tmp171=_tmp16F->f4;if(_tmp171 != Cyc_Absyn_NonNull_to_Null)
goto _LLD4;}}_LLD3: {union Cyc_CfFlowInfo_FlowInfo _tmp1FC;void*_tmp1FD;struct
_tuple13 _tmp1FB=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp170);_tmp1FC=_tmp1FB.f1;
_tmp1FD=_tmp1FB.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp1FF;void*_tmp200;struct
_tuple13 _tmp1FE=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp1FC,_tmp170,
_tmp1FD);_tmp1FF=_tmp1FE.f1;_tmp200=_tmp1FE.f2;{struct _tuple13 _tmp76D;return(
_tmp76D.f1=_tmp1FF,((_tmp76D.f2=_tmp1FD,_tmp76D)));};};}_LLD4: {struct Cyc_Absyn_Cast_e_struct*
_tmp172=(struct Cyc_Absyn_Cast_e_struct*)_tmp16E;if(_tmp172->tag != 15)goto _LLD6;
else{_tmp173=_tmp172->f2;}}_LLD5: _tmp175=_tmp173;goto _LLD7;_LLD6: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp174=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp16E;if(_tmp174->tag != 13)
goto _LLD8;else{_tmp175=_tmp174->f1;}}_LLD7: _tmp177=_tmp175;goto _LLD9;_LLD8: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp176=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp16E;if(_tmp176->tag != 14)goto _LLDA;else{_tmp177=_tmp176->f1;}}_LLD9: return
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp177);_LLDA: {struct Cyc_Absyn_Const_e_struct*
_tmp178=(struct Cyc_Absyn_Const_e_struct*)_tmp16E;if(_tmp178->tag != 0)goto _LLDC;
else{_tmp179=_tmp178->f1;if((_tmp179.Null_c).tag != 1)goto _LLDC;_tmp17A=(int)(
_tmp179.Null_c).val;}}_LLDB: goto _LLDD;_LLDC: {struct Cyc_Absyn_Const_e_struct*
_tmp17B=(struct Cyc_Absyn_Const_e_struct*)_tmp16E;if(_tmp17B->tag != 0)goto _LLDE;
else{_tmp17C=_tmp17B->f1;if((_tmp17C.Int_c).tag != 4)goto _LLDE;_tmp17D=(struct
_tuple6)(_tmp17C.Int_c).val;_tmp17E=_tmp17D.f2;if(_tmp17E != 0)goto _LLDE;}}_LLDD: {
struct _tuple13 _tmp76E;return(_tmp76E.f1=inflow,((_tmp76E.f2=_tmp166->zero,
_tmp76E)));}_LLDE: {struct Cyc_Absyn_Const_e_struct*_tmp17F=(struct Cyc_Absyn_Const_e_struct*)
_tmp16E;if(_tmp17F->tag != 0)goto _LLE0;else{_tmp180=_tmp17F->f1;if((_tmp180.Int_c).tag
!= 4)goto _LLE0;_tmp181=(struct _tuple6)(_tmp180.Int_c).val;}}_LLDF: goto _LLE1;
_LLE0: {struct Cyc_Absyn_Var_e_struct*_tmp182=(struct Cyc_Absyn_Var_e_struct*)
_tmp16E;if(_tmp182->tag != 1)goto _LLE2;else{_tmp183=(void*)_tmp182->f2;{struct Cyc_Absyn_Funname_b_struct*
_tmp184=(struct Cyc_Absyn_Funname_b_struct*)_tmp183;if(_tmp184->tag != 2)goto _LLE2;};}}
_LLE1: {struct _tuple13 _tmp76F;return(_tmp76F.f1=inflow,((_tmp76F.f2=_tmp166->notzeroall,
_tmp76F)));}_LLE2: {struct Cyc_Absyn_Datatype_e_struct*_tmp185=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp16E;if(_tmp185->tag != 32)goto _LLE4;else{_tmp186=_tmp185->f1;if(_tmp186 != 0)
goto _LLE4;}}_LLE3: goto _LLE5;_LLE4: {struct Cyc_Absyn_Const_e_struct*_tmp187=(
struct Cyc_Absyn_Const_e_struct*)_tmp16E;if(_tmp187->tag != 0)goto _LLE6;}_LLE5:
goto _LLE7;_LLE6: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp188=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp16E;if(_tmp188->tag != 19)goto _LLE8;}_LLE7: goto _LLE9;_LLE8: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp189=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp16E;if(_tmp189->tag != 18)goto
_LLEA;}_LLE9: goto _LLEB;_LLEA: {struct Cyc_Absyn_Offsetof_e_struct*_tmp18A=(struct
Cyc_Absyn_Offsetof_e_struct*)_tmp16E;if(_tmp18A->tag != 20)goto _LLEC;}_LLEB: goto
_LLED;_LLEC: {struct Cyc_Absyn_Gentyp_e_struct*_tmp18B=(struct Cyc_Absyn_Gentyp_e_struct*)
_tmp16E;if(_tmp18B->tag != 21)goto _LLEE;}_LLED: goto _LLEF;_LLEE: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp18C=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp16E;if(_tmp18C->tag != 34)goto
_LLF0;}_LLEF: goto _LLF1;_LLF0: {struct Cyc_Absyn_Enum_e_struct*_tmp18D=(struct Cyc_Absyn_Enum_e_struct*)
_tmp16E;if(_tmp18D->tag != 33)goto _LLF2;}_LLF1: {struct _tuple13 _tmp770;return(
_tmp770.f1=inflow,((_tmp770.f2=_tmp166->unknown_all,_tmp770)));}_LLF2: {struct
Cyc_Absyn_Var_e_struct*_tmp18E=(struct Cyc_Absyn_Var_e_struct*)_tmp16E;if(_tmp18E->tag
!= 1)goto _LLF4;else{_tmp18F=(void*)_tmp18E->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp190=(struct Cyc_Absyn_Global_b_struct*)_tmp18F;if(_tmp190->tag != 1)goto _LLF4;};}}
_LLF3: {struct _tuple13 _tmp771;return(_tmp771.f1=inflow,((_tmp771.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp166->unknown_all),
_tmp771)));}_LLF4: {struct Cyc_Absyn_Var_e_struct*_tmp191=(struct Cyc_Absyn_Var_e_struct*)
_tmp16E;if(_tmp191->tag != 1)goto _LLF6;else{_tmp192=(void*)_tmp191->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp193=(struct Cyc_Absyn_Param_b_struct*)_tmp192;if(_tmp193->tag != 3)goto _LLF6;
else{_tmp194=_tmp193->f1;}};}}_LLF5: _tmp198=_tmp194;goto _LLF7;_LLF6: {struct Cyc_Absyn_Var_e_struct*
_tmp195=(struct Cyc_Absyn_Var_e_struct*)_tmp16E;if(_tmp195->tag != 1)goto _LLF8;
else{_tmp196=(void*)_tmp195->f2;{struct Cyc_Absyn_Local_b_struct*_tmp197=(struct
Cyc_Absyn_Local_b_struct*)_tmp196;if(_tmp197->tag != 4)goto _LLF8;else{_tmp198=
_tmp197->f1;}};}}_LLF7: _tmp19C=_tmp198;goto _LLF9;_LLF8: {struct Cyc_Absyn_Var_e_struct*
_tmp199=(struct Cyc_Absyn_Var_e_struct*)_tmp16E;if(_tmp199->tag != 1)goto _LLFA;
else{_tmp19A=(void*)_tmp199->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp19B=(struct Cyc_Absyn_Pat_b_struct*)
_tmp19A;if(_tmp19B->tag != 5)goto _LLFA;else{_tmp19C=_tmp19B->f1;}};}}_LLF9: inflow=
Cyc_NewControlFlow_may_consume_exp(env,inflow,e);{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp777;struct Cyc_CfFlowInfo_VarRoot_struct _tmp776;struct _tuple13 _tmp775;return(
_tmp775.f1=inflow,((_tmp775.f2=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
d,(void*)((_tmp777=_region_malloc(env->r,sizeof(*_tmp777)),((_tmp777[0]=((
_tmp776.tag=0,((_tmp776.f1=_tmp19C,_tmp776)))),_tmp777))))),_tmp775)));};_LLFA: {
struct Cyc_Absyn_Primop_e_struct*_tmp19D=(struct Cyc_Absyn_Primop_e_struct*)
_tmp16E;if(_tmp19D->tag != 3)goto _LLFC;else{_tmp19E=_tmp19D->f1;_tmp19F=_tmp19D->f2;}}
_LLFB: {union Cyc_CfFlowInfo_FlowInfo _tmp20A;void*_tmp20B;struct _tuple13 _tmp209=
Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp19F,0);_tmp20A=_tmp209.f1;
_tmp20B=_tmp209.f2;switch(_tmp19E){case Cyc_Absyn_Plus: _LL13C: goto _LL13D;case Cyc_Absyn_Minus:
_LL13D: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp19F))->hd)->loc,_tmp20A);break;default: _LL13E: _tmp20A=Cyc_CfFlowInfo_readthrough_unique_rvals(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp19F))->hd)->loc,
_tmp20A);break;}{struct _tuple13 _tmp778;return(_tmp778.f1=_tmp20A,((_tmp778.f2=
_tmp20B,_tmp778)));};}_LLFC: {struct Cyc_Absyn_Increment_e_struct*_tmp1A0=(struct
Cyc_Absyn_Increment_e_struct*)_tmp16E;if(_tmp1A0->tag != 5)goto _LLFE;else{_tmp1A1=
_tmp1A0->f1;}}_LLFD: {struct Cyc_List_List _tmp779;struct Cyc_List_List _tmp20D=(
_tmp779.hd=_tmp1A1,((_tmp779.tl=0,_tmp779)));union Cyc_CfFlowInfo_FlowInfo _tmp20F;
struct _tuple13 _tmp20E=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)&
_tmp20D,0);_tmp20F=_tmp20E.f1;Cyc_CfFlowInfo_check_unique_rvals(_tmp1A1->loc,
_tmp20F);{union Cyc_CfFlowInfo_AbsLVal _tmp211;struct _tuple15 _tmp210=Cyc_NewControlFlow_anal_Lexp(
env,_tmp20F,_tmp1A1);_tmp211=_tmp210.f2;{struct _tuple21 _tmp77A;struct _tuple21
_tmp213=(_tmp77A.f1=_tmp211,((_tmp77A.f2=_tmp20F,_tmp77A)));union Cyc_CfFlowInfo_AbsLVal
_tmp214;struct Cyc_CfFlowInfo_Place*_tmp215;union Cyc_CfFlowInfo_FlowInfo _tmp216;
struct _tuple12 _tmp217;struct Cyc_Dict_Dict _tmp218;struct Cyc_List_List*_tmp219;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp21A;_LL141: _tmp214=_tmp213.f1;if((_tmp214.PlaceL).tag
!= 1)goto _LL143;_tmp215=(struct Cyc_CfFlowInfo_Place*)(_tmp214.PlaceL).val;
_tmp216=_tmp213.f2;if((_tmp216.ReachableFL).tag != 2)goto _LL143;_tmp217=(struct
_tuple12)(_tmp216.ReachableFL).val;_tmp218=_tmp217.f1;_tmp219=_tmp217.f2;_tmp21A=
_tmp217.f3;_LL142: _tmp219=Cyc_CfFlowInfo_reln_kill_exp(_tmp166->r,_tmp219,
_tmp1A1);_tmp20F=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp166,
_tmp1A1->loc,_tmp218,env->all_changed,_tmp215,_tmp166->unknown_all),_tmp219,
_tmp21A);goto _LL140;_LL143:;_LL144: goto _LL140;_LL140:;}{struct _tuple13 _tmp77B;
return(_tmp77B.f1=_tmp20F,((_tmp77B.f2=_tmp166->unknown_all,_tmp77B)));};};}
_LLFE: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1A2=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp16E;if(_tmp1A2->tag != 4)goto _LL100;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;
if(_tmp1A4 == 0)goto _LL100;_tmp1A5=*_tmp1A4;_tmp1A6=_tmp1A2->f3;}}_LLFF: {struct
Cyc_List_List _tmp77C;struct Cyc_List_List _tmp21D=(_tmp77C.hd=_tmp1A6,((_tmp77C.tl=
0,_tmp77C)));struct Cyc_List_List _tmp77D;struct Cyc_List_List _tmp21E=(_tmp77D.hd=
_tmp1A3,((_tmp77D.tl=(struct Cyc_List_List*)& _tmp21D,_tmp77D)));union Cyc_CfFlowInfo_FlowInfo
_tmp220;struct _tuple13 _tmp21F=Cyc_NewControlFlow_anal_use_ints(env,inflow,(
struct Cyc_List_List*)& _tmp21E,1);_tmp220=_tmp21F.f1;{union Cyc_CfFlowInfo_AbsLVal
_tmp222;struct _tuple15 _tmp221=Cyc_NewControlFlow_anal_Lexp(env,_tmp220,_tmp1A3);
_tmp222=_tmp221.f2;_tmp220=Cyc_CfFlowInfo_consume_unique_rvals(_tmp166,e->loc,
_tmp220);{union Cyc_CfFlowInfo_FlowInfo _tmp223=_tmp220;struct _tuple12 _tmp224;
struct Cyc_Dict_Dict _tmp225;struct Cyc_List_List*_tmp226;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp227;_LL146: if((_tmp223.ReachableFL).tag != 2)goto _LL148;_tmp224=(struct
_tuple12)(_tmp223.ReachableFL).val;_tmp225=_tmp224.f1;_tmp226=_tmp224.f2;_tmp227=
_tmp224.f3;_LL147:{union Cyc_CfFlowInfo_AbsLVal _tmp228=_tmp222;struct Cyc_CfFlowInfo_Place*
_tmp229;int _tmp22A;_LL14B: if((_tmp228.PlaceL).tag != 1)goto _LL14D;_tmp229=(struct
Cyc_CfFlowInfo_Place*)(_tmp228.PlaceL).val;_LL14C: _tmp227=Cyc_NewControlFlow_consume_assignment(
env,_tmp229,_tmp227,_tmp225,_tmp1A3);_tmp226=Cyc_CfFlowInfo_reln_kill_exp(
_tmp166->r,_tmp226,_tmp1A3);_tmp225=Cyc_CfFlowInfo_assign_place(_tmp166,_tmp1A3->loc,
_tmp225,env->all_changed,_tmp229,_tmp166->unknown_all);_tmp220=Cyc_CfFlowInfo_ReachableFL(
_tmp225,_tmp226,_tmp227);goto _LL14A;_LL14D: if((_tmp228.UnknownL).tag != 2)goto
_LL14A;_tmp22A=(int)(_tmp228.UnknownL).val;_LL14E: goto _LL14A;_LL14A:;}goto _LL145;
_LL148:;_LL149: goto _LL145;_LL145:;}{struct _tuple13 _tmp77E;return(_tmp77E.f1=
_tmp220,((_tmp77E.f2=_tmp166->unknown_all,_tmp77E)));};};}_LL100: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp1A7=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp16E;if(_tmp1A7->tag != 4)goto
_LL102;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;if(_tmp1A9 != 0)goto _LL102;
_tmp1AA=_tmp1A7->f3;}}_LL101: {struct Cyc_Dict_Dict*_tmp22E=env->all_changed;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp230;union Cyc_CfFlowInfo_FlowInfo _tmp231;
struct _tuple14 _tmp22F=Cyc_CfFlowInfo_save_consume_info(_tmp166,inflow,1);_tmp230=
_tmp22F.f1;_tmp231=_tmp22F.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp77F;struct Cyc_CfFlowInfo_ConsumeInfo
empty_consume=(_tmp77F.consumed=_tmp166->mt_place_set,((_tmp77F.may_consume=0,
_tmp77F)));struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=empty_consume;int
init_consume=0;while(1){{struct Cyc_Dict_Dict*_tmp780;env->all_changed=((_tmp780=
_region_malloc(env->r,sizeof(*_tmp780)),((_tmp780[0]=_tmp166->mt_place_set,
_tmp780))));}{union Cyc_CfFlowInfo_FlowInfo _tmp234;union Cyc_CfFlowInfo_AbsLVal
_tmp235;struct _tuple15 _tmp233=Cyc_NewControlFlow_anal_Lexp(env,_tmp231,_tmp1A8);
_tmp234=_tmp233.f1;_tmp235=_tmp233.f2;{struct Cyc_Dict_Dict _tmp236=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp781;env->all_changed=((
_tmp781=_region_malloc(env->r,sizeof(*_tmp781)),((_tmp781[0]=_tmp166->mt_place_set,
_tmp781))));}{union Cyc_CfFlowInfo_FlowInfo _tmp239;void*_tmp23A;struct _tuple13
_tmp238=Cyc_NewControlFlow_anal_Rexp(env,_tmp231,_tmp1AA);_tmp239=_tmp238.f1;
_tmp23A=_tmp238.f2;{struct Cyc_Dict_Dict _tmp23B=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo _tmp23C=Cyc_CfFlowInfo_after_flow(
_tmp166,(struct Cyc_Dict_Dict*)& _tmp236,_tmp234,_tmp239,_tmp236,_tmp23B);union Cyc_CfFlowInfo_FlowInfo
_tmp23D=Cyc_CfFlowInfo_join_flow(_tmp166,_tmp22E,_tmp231,_tmp23C,1);struct Cyc_CfFlowInfo_ConsumeInfo
_tmp23F;struct _tuple14 _tmp23E=Cyc_CfFlowInfo_save_consume_info(_tmp166,_tmp23D,0);
_tmp23F=_tmp23E.f1;if(init_consume){if(!Cyc_CfFlowInfo_consume_approx(
outflow_consume,_tmp23F)){{const char*_tmp784;void*_tmp783;(_tmp783=0,Cyc_fprintf(
Cyc_stderr,((_tmp784="sanity consumed: ",_tag_dyneither(_tmp784,sizeof(char),18))),
_tag_dyneither(_tmp783,sizeof(void*),0)));}((void(*)(struct Cyc_Dict_Dict p,void(*
pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(outflow_consume.consumed,
Cyc_CfFlowInfo_print_place);{const char*_tmp787;void*_tmp786;(_tmp786=0,Cyc_fprintf(
Cyc_stderr,((_tmp787="\ncurrent consumed: ",_tag_dyneither(_tmp787,sizeof(char),
20))),_tag_dyneither(_tmp786,sizeof(void*),0)));}((void(*)(struct Cyc_Dict_Dict p,
void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_dict_set)(_tmp23F.consumed,
Cyc_CfFlowInfo_print_place);{const char*_tmp78A;void*_tmp789;(_tmp789=0,Cyc_fprintf(
Cyc_stderr,((_tmp78A="\nsanity may_consume: ",_tag_dyneither(_tmp78A,sizeof(char),
22))),_tag_dyneither(_tmp789,sizeof(void*),0)));}((void(*)(struct Cyc_List_List*p,
void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(
outflow_consume.may_consume,Cyc_CfFlowInfo_print_place);{const char*_tmp78D;void*
_tmp78C;(_tmp78C=0,Cyc_fprintf(Cyc_stderr,((_tmp78D="\ncurrent may_consume: ",
_tag_dyneither(_tmp78D,sizeof(char),23))),_tag_dyneither(_tmp78C,sizeof(void*),0)));}((
void(*)(struct Cyc_List_List*p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))Cyc_CfFlowInfo_print_list)(
_tmp23F.may_consume,Cyc_CfFlowInfo_print_place);{const char*_tmp790;void*_tmp78F;(
_tmp78F=0,Cyc_fprintf(Cyc_stderr,((_tmp790="\n",_tag_dyneither(_tmp790,sizeof(
char),2))),_tag_dyneither(_tmp78F,sizeof(void*),0)));}{const char*_tmp793;void*
_tmp792;(_tmp792=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp793="assignOp_e failed to preserve consume info",_tag_dyneither(_tmp793,
sizeof(char),43))),_tag_dyneither(_tmp792,sizeof(void*),0)));};}_tmp231=Cyc_CfFlowInfo_restore_consume_info(
_tmp231,outflow_consume,0);}outflow_consume=_tmp23F;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp23D,_tmp231)){if(_tmp22E == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp22E,Cyc_CfFlowInfo_union_place_set(
_tmp236,_tmp23B,0),0);}return Cyc_NewControlFlow_do_assign(_tmp166,env,_tmp23C,
_tmp1A8,_tmp235,_tmp1AA,_tmp23A,_tmp230,e->loc);}_tmp231=Cyc_CfFlowInfo_restore_consume_info(
_tmp23D,empty_consume,0);};};};};}};}_LL102: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp1AB=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp16E;if(_tmp1AB->tag != 9)goto _LL104;
else{_tmp1AC=_tmp1AB->f1;_tmp1AD=_tmp1AB->f2;}}_LL103: {union Cyc_CfFlowInfo_FlowInfo
_tmp24E;void*_tmp24F;struct _tuple13 _tmp24D=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1AC);_tmp24E=_tmp24D.f1;_tmp24F=_tmp24D.f2;_tmp24E=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp1AC->loc,_tmp24E);return Cyc_NewControlFlow_anal_Rexp(env,_tmp24E,_tmp1AD);}
_LL104: {struct Cyc_Absyn_Throw_e_struct*_tmp1AE=(struct Cyc_Absyn_Throw_e_struct*)
_tmp16E;if(_tmp1AE->tag != 12)goto _LL106;else{_tmp1AF=_tmp1AE->f1;}}_LL105: {
union Cyc_CfFlowInfo_FlowInfo _tmp251;void*_tmp252;struct _tuple13 _tmp250=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1AF);_tmp251=_tmp250.f1;_tmp252=_tmp250.f2;_tmp251=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,_tmp1AF->loc,_tmp251);Cyc_NewControlFlow_use_Rval(env,_tmp1AF->loc,
_tmp251,_tmp252);{struct _tuple13 _tmp794;return(_tmp794.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp794.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp166,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp166->unknown_all),_tmp794)));};}_LL106: {struct Cyc_Absyn_FnCall_e_struct*
_tmp1B0=(struct Cyc_Absyn_FnCall_e_struct*)_tmp16E;if(_tmp1B0->tag != 11)goto
_LL108;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;}}_LL107: {struct
_RegionHandle*_tmp254=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp257;struct Cyc_List_List*
_tmp258;struct Cyc_List_List*_tmp795;struct _tuple19 _tmp256=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp254,env,inflow,((_tmp795=_region_malloc(_tmp254,sizeof(*_tmp795)),((_tmp795->hd=
_tmp1B1,((_tmp795->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(_tmp254,_tmp1B2),_tmp795)))))),1);_tmp257=_tmp256.f1;_tmp258=
_tmp256.f2;_tmp257=Cyc_CfFlowInfo_consume_unique_rvals(_tmp166,e->loc,_tmp257);{
union Cyc_CfFlowInfo_FlowInfo _tmp259=Cyc_NewControlFlow_use_Rval(env,_tmp1B1->loc,
_tmp257,(void*)((struct Cyc_List_List*)_check_null(_tmp258))->hd);_tmp258=_tmp258->tl;{
struct Cyc_List_List*init_params=0;{void*_tmp25A=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1B1->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp25C;void*_tmp25D;_LL150: {struct Cyc_Absyn_PointerType_struct*_tmp25B=(struct
Cyc_Absyn_PointerType_struct*)_tmp25A;if(_tmp25B->tag != 5)goto _LL152;else{
_tmp25C=_tmp25B->f1;_tmp25D=_tmp25C.elt_typ;}}_LL151:{void*_tmp25E=Cyc_Tcutil_compress(
_tmp25D);struct Cyc_Absyn_FnInfo _tmp260;struct Cyc_List_List*_tmp261;_LL155: {
struct Cyc_Absyn_FnType_struct*_tmp25F=(struct Cyc_Absyn_FnType_struct*)_tmp25E;
if(_tmp25F->tag != 10)goto _LL157;else{_tmp260=_tmp25F->f1;_tmp261=_tmp260.attributes;}}
_LL156: for(0;_tmp261 != 0;_tmp261=_tmp261->tl){void*_tmp262=(void*)_tmp261->hd;
int _tmp264;_LL15A: {struct Cyc_Absyn_Initializes_att_struct*_tmp263=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp262;if(_tmp263->tag != 20)goto _LL15C;else{_tmp264=_tmp263->f1;}}_LL15B:{
struct Cyc_List_List*_tmp796;init_params=((_tmp796=_region_malloc(_tmp254,sizeof(*
_tmp796)),((_tmp796->hd=(void*)_tmp264,((_tmp796->tl=init_params,_tmp796))))));}
goto _LL159;_LL15C:;_LL15D: goto _LL159;_LL159:;}goto _LL154;_LL157:;_LL158: {const
char*_tmp799;void*_tmp798;(_tmp798=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp799="anal_Rexp: bad function type",
_tag_dyneither(_tmp799,sizeof(char),29))),_tag_dyneither(_tmp798,sizeof(void*),0)));}
_LL154:;}goto _LL14F;_LL152:;_LL153: {const char*_tmp79C;void*_tmp79B;(_tmp79B=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp79C="anal_Rexp: bad function type",_tag_dyneither(_tmp79C,sizeof(char),29))),
_tag_dyneither(_tmp79B,sizeof(void*),0)));}_LL14F:;}{int i=1;for(0;_tmp258 != 0;(((
_tmp258=_tmp258->tl,_tmp1B2=((struct Cyc_List_List*)_check_null(_tmp1B2))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
_tmp259=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1B2))->hd)->loc,_tmp259,(void*)_tmp258->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo
_tmp26A=_tmp257;int _tmp26B;struct _tuple12 _tmp26C;struct Cyc_Dict_Dict _tmp26D;
_LL15F: if((_tmp26A.BottomFL).tag != 1)goto _LL161;_tmp26B=(int)(_tmp26A.BottomFL).val;
_LL160: goto _LL15E;_LL161: if((_tmp26A.ReachableFL).tag != 2)goto _LL15E;_tmp26C=(
struct _tuple12)(_tmp26A.ReachableFL).val;_tmp26D=_tmp26C.f1;_LL162: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp26D,(void*)_tmp258->hd)== Cyc_CfFlowInfo_NoneIL){const char*_tmp79F;
void*_tmp79E;(_tmp79E=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1B2))->hd)->loc,((_tmp79F="expression may not be initialized",
_tag_dyneither(_tmp79F,sizeof(char),34))),_tag_dyneither(_tmp79E,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp270=_tmp259;int _tmp271;struct _tuple12 _tmp272;
struct Cyc_Dict_Dict _tmp273;struct Cyc_List_List*_tmp274;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp275;_LL164: if((_tmp270.BottomFL).tag != 1)goto _LL166;_tmp271=(int)(_tmp270.BottomFL).val;
_LL165: goto _LL163;_LL166: if((_tmp270.ReachableFL).tag != 2)goto _LL163;_tmp272=(
struct _tuple12)(_tmp270.ReachableFL).val;_tmp273=_tmp272.f1;_tmp274=_tmp272.f2;
_tmp275=_tmp272.f3;_LL167: {struct Cyc_Dict_Dict _tmp276=Cyc_CfFlowInfo_escape_deref(
_tmp166,_tmp273,env->all_changed,(void*)_tmp258->hd);{void*_tmp277=(void*)
_tmp258->hd;struct Cyc_CfFlowInfo_Place*_tmp279;_LL169: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp278=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp277;if(_tmp278->tag != 5)goto
_LL16B;else{_tmp279=_tmp278->f1;}}_LL16A:{void*_tmp27A=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp1B2))->hd)->topt))->v);struct Cyc_Absyn_PtrInfo _tmp27C;void*
_tmp27D;_LL16E: {struct Cyc_Absyn_PointerType_struct*_tmp27B=(struct Cyc_Absyn_PointerType_struct*)
_tmp27A;if(_tmp27B->tag != 5)goto _LL170;else{_tmp27C=_tmp27B->f1;_tmp27D=_tmp27C.elt_typ;}}
_LL16F: _tmp276=Cyc_CfFlowInfo_assign_place(_tmp166,((struct Cyc_Absyn_Exp*)
_tmp1B2->hd)->loc,_tmp276,env->all_changed,_tmp279,Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,_tmp27D,_tmp166->esc_all));goto _LL16D;_LL170:;_LL171: {const char*_tmp7A2;
void*_tmp7A1;(_tmp7A1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp7A2="anal_Rexp:bad type for initialized arg",
_tag_dyneither(_tmp7A2,sizeof(char),39))),_tag_dyneither(_tmp7A1,sizeof(void*),0)));}
_LL16D:;}goto _LL168;_LL16B:;_LL16C: goto _LL168;_LL168:;}_tmp259=Cyc_CfFlowInfo_ReachableFL(
_tmp276,_tmp274,_tmp275);goto _LL163;}_LL163:;}goto _LL15E;_LL15E:;};}}if(Cyc_Tcutil_is_noreturn((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1B1->topt))->v)){struct _tuple13
_tmp7A3;return(_tmp7A3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7A3.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp166->unknown_all),
_tmp7A3)));}else{struct _tuple13 _tmp7A4;return(_tmp7A4.f1=_tmp259,((_tmp7A4.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp166,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
_tmp166->unknown_all),_tmp7A4)));}};};}_LL108: {struct Cyc_Absyn_Malloc_e_struct*
_tmp1B3=(struct Cyc_Absyn_Malloc_e_struct*)_tmp16E;if(_tmp1B3->tag != 35)goto
_LL10A;else{_tmp1B4=_tmp1B3->f1;_tmp1B5=_tmp1B4.is_calloc;_tmp1B6=_tmp1B4.rgn;
_tmp1B7=_tmp1B4.elt_type;_tmp1B8=_tmp1B4.num_elts;_tmp1B9=_tmp1B4.fat_result;}}
_LL109: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp7A7;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp7A6;void*root=(void*)((_tmp7A6=_region_malloc(_tmp166->r,sizeof(*_tmp7A6)),((
_tmp7A6[0]=((_tmp7A7.tag=1,((_tmp7A7.f1=_tmp1B8,((_tmp7A7.f2=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp7A7)))))),_tmp7A6))));struct
Cyc_CfFlowInfo_Place*_tmp7A8;struct Cyc_CfFlowInfo_Place*place=(_tmp7A8=
_region_malloc(_tmp166->r,sizeof(*_tmp7A8)),((_tmp7A8->root=root,((_tmp7A8->fields=
0,_tmp7A8)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp7AB;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp7AA;void*rval=(void*)((_tmp7AA=_region_malloc(_tmp166->r,sizeof(*_tmp7AA)),((
_tmp7AA[0]=((_tmp7AB.tag=5,((_tmp7AB.f1=place,_tmp7AB)))),_tmp7AA))));void*
place_val=_tmp1B9?_tmp166->notzeroall: Cyc_CfFlowInfo_typ_to_absrval(_tmp166,*((
void**)_check_null(_tmp1B7)),_tmp166->unknown_none);union Cyc_CfFlowInfo_FlowInfo
outflow;((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,
place,0);if(_tmp1B6 != 0){struct _RegionHandle*_tmp282=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp285;struct Cyc_List_List*_tmp286;struct Cyc_Absyn_Exp*_tmp7AC[2];struct
_tuple19 _tmp284=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp282,env,inflow,((
_tmp7AC[1]=_tmp1B8,((_tmp7AC[0]=(struct Cyc_Absyn_Exp*)_tmp1B6,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp282,
_tag_dyneither(_tmp7AC,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp285=_tmp284.f1;
_tmp286=_tmp284.f2;outflow=_tmp285;}else{union Cyc_CfFlowInfo_FlowInfo _tmp288;
struct _tuple13 _tmp287=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1B8);_tmp288=
_tmp287.f1;outflow=_tmp288;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1B8->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo _tmp289=outflow;int _tmp28A;
struct _tuple12 _tmp28B;struct Cyc_Dict_Dict _tmp28C;struct Cyc_List_List*_tmp28D;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp28E;_LL173: if((_tmp289.BottomFL).tag != 1)
goto _LL175;_tmp28A=(int)(_tmp289.BottomFL).val;_LL174: {struct _tuple13 _tmp7AD;
return(_tmp7AD.f1=outflow,((_tmp7AD.f2=rval,_tmp7AD)));}_LL175: if((_tmp289.ReachableFL).tag
!= 2)goto _LL172;_tmp28B=(struct _tuple12)(_tmp289.ReachableFL).val;_tmp28C=
_tmp28B.f1;_tmp28D=_tmp28B.f2;_tmp28E=_tmp28B.f3;_LL176: {struct _tuple13 _tmp7AE;
return(_tmp7AE.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,void*v))Cyc_Dict_insert)(_tmp28C,root,place_val),_tmp28D,_tmp28E),((
_tmp7AE.f2=rval,_tmp7AE)));}_LL172:;};}_LL10A: {struct Cyc_Absyn_Swap_e_struct*
_tmp1BA=(struct Cyc_Absyn_Swap_e_struct*)_tmp16E;if(_tmp1BA->tag != 36)goto _LL10C;
else{_tmp1BB=_tmp1BA->f1;_tmp1BC=_tmp1BA->f2;}}_LL10B: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo outflow;struct _RegionHandle*_tmp296=env->r;{
union Cyc_CfFlowInfo_FlowInfo _tmp299;struct Cyc_List_List*_tmp29A;struct Cyc_Absyn_Exp*
_tmp7AF[2];struct _tuple19 _tmp298=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp296,
env,inflow,((_tmp7AF[1]=_tmp1BC,((_tmp7AF[0]=_tmp1BB,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp296,
_tag_dyneither(_tmp7AF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp299=_tmp298.f1;
_tmp29A=_tmp298.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp29A))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp29A->tl))->hd;outflow=
_tmp299;}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,outflow);{union Cyc_CfFlowInfo_AbsLVal
_tmp29C;struct _tuple15 _tmp29B=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp1BB);
_tmp29C=_tmp29B.f2;{union Cyc_CfFlowInfo_AbsLVal _tmp29E;struct _tuple15 _tmp29D=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp1BC);_tmp29E=_tmp29D.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp29F=
outflow;struct _tuple12 _tmp2A0;struct Cyc_Dict_Dict _tmp2A1;struct Cyc_List_List*
_tmp2A2;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2A3;_LL178: if((_tmp29F.ReachableFL).tag
!= 2)goto _LL17A;_tmp2A0=(struct _tuple12)(_tmp29F.ReachableFL).val;_tmp2A1=
_tmp2A0.f1;_tmp2A2=_tmp2A0.f2;_tmp2A3=_tmp2A0.f3;_LL179:{union Cyc_CfFlowInfo_AbsLVal
_tmp2A4=_tmp29C;struct Cyc_CfFlowInfo_Place*_tmp2A5;int _tmp2A6;_LL17D: if((_tmp2A4.PlaceL).tag
!= 1)goto _LL17F;_tmp2A5=(struct Cyc_CfFlowInfo_Place*)(_tmp2A4.PlaceL).val;_LL17E:
_tmp2A1=Cyc_CfFlowInfo_assign_place(_tmp166,_tmp1BB->loc,_tmp2A1,env->all_changed,
_tmp2A5,right_rval);goto _LL17C;_LL17F: if((_tmp2A4.UnknownL).tag != 2)goto _LL17C;
_tmp2A6=(int)(_tmp2A4.UnknownL).val;_LL180: goto _LL17C;_LL17C:;}{union Cyc_CfFlowInfo_AbsLVal
_tmp2A7=_tmp29E;struct Cyc_CfFlowInfo_Place*_tmp2A8;int _tmp2A9;_LL182: if((_tmp2A7.PlaceL).tag
!= 1)goto _LL184;_tmp2A8=(struct Cyc_CfFlowInfo_Place*)(_tmp2A7.PlaceL).val;_LL183:
_tmp2A1=Cyc_CfFlowInfo_assign_place(_tmp166,_tmp1BC->loc,_tmp2A1,env->all_changed,
_tmp2A8,left_rval);goto _LL181;_LL184: if((_tmp2A7.UnknownL).tag != 2)goto _LL181;
_tmp2A9=(int)(_tmp2A7.UnknownL).val;_LL185: goto _LL181;_LL181:;}_tmp2A2=Cyc_CfFlowInfo_reln_kill_exp(
_tmp166->r,_tmp2A2,_tmp1BB);_tmp2A2=Cyc_CfFlowInfo_reln_kill_exp(_tmp166->r,
_tmp2A2,_tmp1BC);outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2A1,_tmp2A2,_tmp2A3);
goto _LL177;_LL17A:;_LL17B: goto _LL177;_LL177:;}{struct _tuple13 _tmp7B0;return(
_tmp7B0.f1=outflow,((_tmp7B0.f2=_tmp166->unknown_all,_tmp7B0)));};};};}_LL10C: {
struct Cyc_Absyn_New_e_struct*_tmp1BD=(struct Cyc_Absyn_New_e_struct*)_tmp16E;if(
_tmp1BD->tag != 17)goto _LL10E;else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BD->f2;}}
_LL10D: {struct Cyc_CfFlowInfo_MallocPt_struct _tmp7B3;struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp7B2;void*root=(void*)((_tmp7B2=_region_malloc(_tmp166->r,sizeof(*_tmp7B2)),((
_tmp7B2[0]=((_tmp7B3.tag=1,((_tmp7B3.f1=_tmp1BF,((_tmp7B3.f2=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp7B3)))))),_tmp7B2))));struct
Cyc_CfFlowInfo_Place*_tmp7B4;struct Cyc_CfFlowInfo_Place*place=(_tmp7B4=
_region_malloc(_tmp166->r,sizeof(*_tmp7B4)),((_tmp7B4->root=root,((_tmp7B4->fields=
0,_tmp7B4)))));struct Cyc_CfFlowInfo_AddressOf_struct _tmp7B7;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp7B6;void*rval=(void*)((_tmp7B6=_region_malloc(_tmp166->r,sizeof(*_tmp7B6)),((
_tmp7B6[0]=((_tmp7B7.tag=5,((_tmp7B7.f1=place,_tmp7B7)))),_tmp7B6))));((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{union Cyc_CfFlowInfo_FlowInfo
outflow;void*place_val;if(_tmp1BE != 0){struct _RegionHandle*_tmp2AB=env->r;union
Cyc_CfFlowInfo_FlowInfo _tmp2AE;struct Cyc_List_List*_tmp2AF;struct Cyc_Absyn_Exp*
_tmp7B8[2];struct _tuple19 _tmp2AD=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp2AB,
env,inflow,((_tmp7B8[1]=_tmp1BF,((_tmp7B8[0]=(struct Cyc_Absyn_Exp*)_tmp1BE,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(
_tmp2AB,_tag_dyneither(_tmp7B8,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp2AE=
_tmp2AD.f1;_tmp2AF=_tmp2AD.f2;outflow=_tmp2AE;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp2AF))->tl))->hd;}else{union
Cyc_CfFlowInfo_FlowInfo _tmp2B1;void*_tmp2B2;struct _tuple13 _tmp2B0=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1BF);_tmp2B1=_tmp2B0.f1;_tmp2B2=_tmp2B0.f2;outflow=_tmp2B1;
place_val=_tmp2B2;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1BF->loc,
outflow);{union Cyc_CfFlowInfo_FlowInfo _tmp2B3=outflow;int _tmp2B4;struct _tuple12
_tmp2B5;struct Cyc_Dict_Dict _tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2B8;_LL187: if((_tmp2B3.BottomFL).tag != 1)goto _LL189;_tmp2B4=(int)(_tmp2B3.BottomFL).val;
_LL188: {struct _tuple13 _tmp7B9;return(_tmp7B9.f1=outflow,((_tmp7B9.f2=rval,
_tmp7B9)));}_LL189: if((_tmp2B3.ReachableFL).tag != 2)goto _LL186;_tmp2B5=(struct
_tuple12)(_tmp2B3.ReachableFL).val;_tmp2B6=_tmp2B5.f1;_tmp2B7=_tmp2B5.f2;_tmp2B8=
_tmp2B5.f3;_LL18A: {struct _tuple13 _tmp7BA;return(_tmp7BA.f1=Cyc_CfFlowInfo_ReachableFL(((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp2B6,root,place_val),_tmp2B7,_tmp2B8),((_tmp7BA.f2=rval,_tmp7BA)));}_LL186:;};};}
_LL10E: {struct Cyc_Absyn_Address_e_struct*_tmp1C0=(struct Cyc_Absyn_Address_e_struct*)
_tmp16E;if(_tmp1C0->tag != 16)goto _LL110;else{_tmp1C1=_tmp1C0->f1;}}_LL10F: {
union Cyc_CfFlowInfo_FlowInfo _tmp2C1;struct _tuple13 _tmp2C0=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1C1);_tmp2C1=_tmp2C0.f1;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp2C3;
struct _tuple14 _tmp2C2=Cyc_CfFlowInfo_save_consume_info(env->fenv,_tmp2C1,0);
_tmp2C3=_tmp2C2.f1;{union Cyc_CfFlowInfo_FlowInfo _tmp2C5;union Cyc_CfFlowInfo_AbsLVal
_tmp2C6;struct _tuple15 _tmp2C4=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp1C1);
_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;_tmp2C5=Cyc_CfFlowInfo_restore_consume_info(
_tmp2C5,_tmp2C3,0);{union Cyc_CfFlowInfo_AbsLVal _tmp2C7=_tmp2C6;int _tmp2C8;struct
Cyc_CfFlowInfo_Place*_tmp2C9;_LL18C: if((_tmp2C7.UnknownL).tag != 2)goto _LL18E;
_tmp2C8=(int)(_tmp2C7.UnknownL).val;_LL18D: {struct _tuple13 _tmp7BB;return(
_tmp7BB.f1=_tmp2C5,((_tmp7BB.f2=_tmp166->notzeroall,_tmp7BB)));}_LL18E: if((
_tmp2C7.PlaceL).tag != 1)goto _LL18B;_tmp2C9=(struct Cyc_CfFlowInfo_Place*)(_tmp2C7.PlaceL).val;
_LL18F: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp7C1;struct Cyc_CfFlowInfo_AddressOf_struct
_tmp7C0;struct _tuple13 _tmp7BF;return(_tmp7BF.f1=_tmp2C5,((_tmp7BF.f2=(void*)((
_tmp7C1=_region_malloc(env->r,sizeof(*_tmp7C1)),((_tmp7C1[0]=((_tmp7C0.tag=5,((
_tmp7C0.f1=_tmp2C9,_tmp7C0)))),_tmp7C1)))),_tmp7BF)));}_LL18B:;};};};}_LL110: {
struct Cyc_Absyn_Deref_e_struct*_tmp1C2=(struct Cyc_Absyn_Deref_e_struct*)_tmp16E;
if(_tmp1C2->tag != 22)goto _LL112;else{_tmp1C3=_tmp1C2->f1;}}_LL111: {union Cyc_CfFlowInfo_FlowInfo
_tmp2CF;void*_tmp2D0;struct _tuple13 _tmp2CE=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1C3);_tmp2CF=_tmp2CE.f1;_tmp2D0=_tmp2CE.f2;_tmp2CF=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp2CF);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2CF,_tmp1C3,
_tmp2D0);}_LL112: {struct Cyc_Absyn_AggrMember_e_struct*_tmp1C4=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp16E;if(_tmp1C4->tag != 23)goto _LL114;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C4->f2;}}
_LL113: {union Cyc_CfFlowInfo_FlowInfo _tmp2D2;void*_tmp2D3;struct _tuple13 _tmp2D1=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1C5);_tmp2D2=_tmp2D1.f1;_tmp2D3=
_tmp2D1.f2;_tmp2D2=Cyc_CfFlowInfo_drop_unique_rvals(e->loc,_tmp2D2);_tmp2D2=Cyc_NewControlFlow_may_consume_exp(
env,_tmp2D2,e);if(Cyc_Absyn_is_nontagged_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1C5->topt))->v)){struct _tuple13 _tmp7C2;return(_tmp7C2.f1=_tmp2D2,((
_tmp7C2.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp166,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp166->unknown_all),_tmp7C2)));}{void*_tmp2D5=_tmp2D3;
int _tmp2D7;struct _dyneither_ptr _tmp2D8;_LL191: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2D6=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2D5;if(_tmp2D6->tag != 7)goto
_LL193;else{_tmp2D7=_tmp2D6->f1;_tmp2D8=_tmp2D6->f2;}}_LL192: {int _tmp2D9=Cyc_CfFlowInfo_get_field_index((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1C5->topt))->v,_tmp1C6);struct
_tuple13 _tmp7C3;return(_tmp7C3.f1=_tmp2D2,((_tmp7C3.f2=*((void**)
_check_dyneither_subscript(_tmp2D8,sizeof(void*),_tmp2D9)),_tmp7C3)));}_LL193:;
_LL194: {struct Cyc_Core_Impossible_struct _tmp7D0;const char*_tmp7CF;void*_tmp7CE[
1];struct Cyc_String_pa_struct _tmp7CD;struct Cyc_Core_Impossible_struct*_tmp7CC;(
int)_throw((void*)((_tmp7CC=_cycalloc(sizeof(*_tmp7CC)),((_tmp7CC[0]=((_tmp7D0.tag=
Cyc_Core_Impossible,((_tmp7D0.f1=(struct _dyneither_ptr)((_tmp7CD.tag=0,((_tmp7CD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7CE[
0]=& _tmp7CD,Cyc_aprintf(((_tmp7CF="anal_Rexp: AggrMember: %s",_tag_dyneither(
_tmp7CF,sizeof(char),26))),_tag_dyneither(_tmp7CE,sizeof(void*),1)))))))),
_tmp7D0)))),_tmp7CC)))));}_LL190:;};}_LL114: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp1C7=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp16E;if(_tmp1C7->tag != 39)goto
_LL116;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;}}_LL115: {union Cyc_CfFlowInfo_FlowInfo
_tmp2E1;void*_tmp2E2;struct _tuple13 _tmp2E0=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1C8);_tmp2E1=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;_tmp2E1=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp2E1);_tmp2E1=Cyc_NewControlFlow_may_consume_exp(env,_tmp2E1,e);{
struct _tuple13 _tmp7D1;return(_tmp7D1.f1=_tmp2E1,((_tmp7D1.f2=_tmp166->unknown_all,
_tmp7D1)));};}_LL116: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp1CA=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp16E;if(_tmp1CA->tag != 24)goto _LL118;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CA->f2;}}
_LL117: {union Cyc_CfFlowInfo_FlowInfo _tmp2E5;void*_tmp2E6;struct _tuple13 _tmp2E4=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1CB);_tmp2E5=_tmp2E4.f1;_tmp2E6=
_tmp2E4.f2;_tmp2E5=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,_tmp2E5);{
union Cyc_CfFlowInfo_FlowInfo _tmp2E8;void*_tmp2E9;struct _tuple13 _tmp2E7=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp2E5,_tmp1CB,_tmp2E6);_tmp2E8=_tmp2E7.f1;_tmp2E9=_tmp2E7.f2;{void*
_tmp2EA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1CB->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp2EC;void*_tmp2ED;_LL196: {struct Cyc_Absyn_PointerType_struct*
_tmp2EB=(struct Cyc_Absyn_PointerType_struct*)_tmp2EA;if(_tmp2EB->tag != 5)goto
_LL198;else{_tmp2EC=_tmp2EB->f1;_tmp2ED=_tmp2EC.elt_typ;}}_LL197: if(Cyc_Absyn_is_nontagged_union_type(
_tmp2ED)){struct _tuple13 _tmp7D2;return(_tmp7D2.f1=_tmp2E8,((_tmp7D2.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp166->unknown_all),
_tmp7D2)));}{void*_tmp2EF=_tmp2E9;int _tmp2F1;struct _dyneither_ptr _tmp2F2;_LL19B: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp2F0=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp2EF;if(_tmp2F0->tag != 7)goto _LL19D;else{_tmp2F1=_tmp2F0->f1;_tmp2F2=_tmp2F0->f2;}}
_LL19C: {int _tmp2F3=Cyc_CfFlowInfo_get_field_index(_tmp2ED,_tmp1CC);struct
_tuple13 _tmp7D3;return(_tmp7D3.f1=_tmp2E8,((_tmp7D3.f2=*((void**)
_check_dyneither_subscript(_tmp2F2,sizeof(void*),_tmp2F3)),_tmp7D3)));}_LL19D:;
_LL19E: {struct Cyc_Core_Impossible_struct _tmp7D9;const char*_tmp7D8;struct Cyc_Core_Impossible_struct*
_tmp7D7;(int)_throw((void*)((_tmp7D7=_cycalloc(sizeof(*_tmp7D7)),((_tmp7D7[0]=((
_tmp7D9.tag=Cyc_Core_Impossible,((_tmp7D9.f1=((_tmp7D8="anal_Rexp: AggrArrow",
_tag_dyneither(_tmp7D8,sizeof(char),21))),_tmp7D9)))),_tmp7D7)))));}_LL19A:;};
_LL198:;_LL199: {struct Cyc_Core_Impossible_struct _tmp7DF;const char*_tmp7DE;
struct Cyc_Core_Impossible_struct*_tmp7DD;(int)_throw((void*)((_tmp7DD=_cycalloc(
sizeof(*_tmp7DD)),((_tmp7DD[0]=((_tmp7DF.tag=Cyc_Core_Impossible,((_tmp7DF.f1=((
_tmp7DE="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp7DE,sizeof(char),25))),
_tmp7DF)))),_tmp7DD)))));}_LL195:;};};}_LL118: {struct Cyc_Absyn_Conditional_e_struct*
_tmp1CD=(struct Cyc_Absyn_Conditional_e_struct*)_tmp16E;if(_tmp1CD->tag != 6)goto
_LL11A;else{_tmp1CE=_tmp1CD->f1;_tmp1CF=_tmp1CD->f2;_tmp1D0=_tmp1CD->f3;}}_LL119: {
union Cyc_CfFlowInfo_FlowInfo _tmp2FC;union Cyc_CfFlowInfo_FlowInfo _tmp2FD;struct
_tuple16 _tmp2FB=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1CE);_tmp2FC=_tmp2FB.f1;
_tmp2FD=_tmp2FB.f2;_tmp2FC=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1CE->loc,
_tmp2FC);_tmp2FD=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1CE->loc,_tmp2FD);{
struct _tuple13 _tmp2FE=Cyc_NewControlFlow_anal_Rexp(env,_tmp2FC,_tmp1CF);struct
_tuple13 _tmp2FF=Cyc_NewControlFlow_anal_Rexp(env,_tmp2FD,_tmp1D0);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp166,env->all_changed,_tmp2FE,_tmp2FF,1);};}_LL11A: {struct Cyc_Absyn_And_e_struct*
_tmp1D1=(struct Cyc_Absyn_And_e_struct*)_tmp16E;if(_tmp1D1->tag != 7)goto _LL11C;
else{_tmp1D2=_tmp1D1->f1;_tmp1D3=_tmp1D1->f2;}}_LL11B: {union Cyc_CfFlowInfo_FlowInfo
_tmp301;union Cyc_CfFlowInfo_FlowInfo _tmp302;struct _tuple16 _tmp300=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1D2);_tmp301=_tmp300.f1;_tmp302=_tmp300.f2;_tmp301=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1D2->loc,_tmp301);_tmp302=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D2->loc,
_tmp302);{union Cyc_CfFlowInfo_FlowInfo _tmp304;void*_tmp305;struct _tuple13 _tmp303=
Cyc_NewControlFlow_anal_Rexp(env,_tmp301,_tmp1D3);_tmp304=_tmp303.f1;_tmp305=
_tmp303.f2;_tmp304=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D3->loc,_tmp304);{
struct _tuple13 _tmp7E0;struct _tuple13 _tmp306=(_tmp7E0.f1=_tmp304,((_tmp7E0.f2=
_tmp305,_tmp7E0)));struct _tuple13 _tmp7E1;struct _tuple13 _tmp307=(_tmp7E1.f1=
_tmp302,((_tmp7E1.f2=_tmp166->zero,_tmp7E1)));return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp166,env->all_changed,_tmp306,_tmp307,0);};};}_LL11C: {struct Cyc_Absyn_Or_e_struct*
_tmp1D4=(struct Cyc_Absyn_Or_e_struct*)_tmp16E;if(_tmp1D4->tag != 8)goto _LL11E;
else{_tmp1D5=_tmp1D4->f1;_tmp1D6=_tmp1D4->f2;}}_LL11D: {union Cyc_CfFlowInfo_FlowInfo
_tmp30B;union Cyc_CfFlowInfo_FlowInfo _tmp30C;struct _tuple16 _tmp30A=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp1D5);_tmp30B=_tmp30A.f1;_tmp30C=_tmp30A.f2;_tmp30B=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1D5->loc,_tmp30B);_tmp30C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D5->loc,
_tmp30C);{union Cyc_CfFlowInfo_FlowInfo _tmp30E;void*_tmp30F;struct _tuple13 _tmp30D=
Cyc_NewControlFlow_anal_Rexp(env,_tmp30C,_tmp1D6);_tmp30E=_tmp30D.f1;_tmp30F=
_tmp30D.f2;_tmp30E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D6->loc,_tmp30E);{
struct _tuple13 _tmp7E2;struct _tuple13 _tmp310=(_tmp7E2.f1=_tmp30E,((_tmp7E2.f2=
_tmp30F,_tmp7E2)));struct _tuple13 _tmp7E3;struct _tuple13 _tmp311=(_tmp7E3.f1=
_tmp30B,((_tmp7E3.f2=_tmp166->notzeroall,_tmp7E3)));return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp166,env->all_changed,_tmp310,_tmp311,0);};};}_LL11E: {struct Cyc_Absyn_Subscript_e_struct*
_tmp1D7=(struct Cyc_Absyn_Subscript_e_struct*)_tmp16E;if(_tmp1D7->tag != 25)goto
_LL120;else{_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D7->f2;}}_LL11F: {struct
_RegionHandle*_tmp314=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp317;struct Cyc_List_List*
_tmp318;struct Cyc_Absyn_Exp*_tmp7E4[2];struct _tuple19 _tmp316=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp314,env,inflow,((_tmp7E4[1]=_tmp1D9,((_tmp7E4[0]=_tmp1D8,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmp314,
_tag_dyneither(_tmp7E4,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp317=_tmp316.f1;
_tmp318=_tmp316.f2;_tmp317=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp1D9->loc,
_tmp317);{union Cyc_CfFlowInfo_FlowInfo _tmp319=_tmp317;{union Cyc_CfFlowInfo_FlowInfo
_tmp31A=_tmp317;struct _tuple12 _tmp31B;struct Cyc_Dict_Dict _tmp31C;struct Cyc_List_List*
_tmp31D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp31E;_LL1A0: if((_tmp31A.ReachableFL).tag
!= 2)goto _LL1A2;_tmp31B=(struct _tuple12)(_tmp31A.ReachableFL).val;_tmp31C=
_tmp31B.f1;_tmp31D=_tmp31B.f2;_tmp31E=_tmp31B.f3;_LL1A1: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp31C,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp318))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){const char*_tmp7E7;void*
_tmp7E6;(_tmp7E6=0,Cyc_Tcutil_terr(_tmp1D9->loc,((_tmp7E7="expression may not be initialized",
_tag_dyneither(_tmp7E7,sizeof(char),34))),_tag_dyneither(_tmp7E6,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp321=Cyc_NewControlFlow_add_subscript_reln(_tmp166->r,
_tmp31D,_tmp1D8,_tmp1D9);if(_tmp31D != _tmp321)_tmp319=Cyc_CfFlowInfo_ReachableFL(
_tmp31C,_tmp321,_tmp31E);goto _LL19F;};_LL1A2:;_LL1A3: goto _LL19F;_LL19F:;}{void*
_tmp322=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D8->topt))->v);
struct Cyc_List_List*_tmp324;struct Cyc_Absyn_PtrInfo _tmp326;struct Cyc_Absyn_PtrAtts
_tmp327;union Cyc_Absyn_Constraint*_tmp328;_LL1A5: {struct Cyc_Absyn_TupleType_struct*
_tmp323=(struct Cyc_Absyn_TupleType_struct*)_tmp322;if(_tmp323->tag != 11)goto
_LL1A7;else{_tmp324=_tmp323->f1;}}_LL1A6: {void*_tmp329=(void*)((struct Cyc_List_List*)
_check_null(_tmp318))->hd;int _tmp32B;struct _dyneither_ptr _tmp32C;_LL1AC: {struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp32A=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp329;if(_tmp32A->tag != 7)goto _LL1AE;else{_tmp32B=_tmp32A->f1;_tmp32C=_tmp32A->f2;}}
_LL1AD: _tmp319=Cyc_NewControlFlow_may_consume_exp(env,_tmp319,e);{unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp1D9)).f1;struct _tuple13 _tmp7E8;return(_tmp7E8.f1=
_tmp319,((_tmp7E8.f2=*((void**)_check_dyneither_subscript(_tmp32C,sizeof(void*),(
int)i)),_tmp7E8)));};_LL1AE:;_LL1AF: {struct Cyc_Core_Impossible_struct _tmp7EE;
const char*_tmp7ED;struct Cyc_Core_Impossible_struct*_tmp7EC;(int)_throw((void*)((
_tmp7EC=_cycalloc(sizeof(*_tmp7EC)),((_tmp7EC[0]=((_tmp7EE.tag=Cyc_Core_Impossible,((
_tmp7EE.f1=((_tmp7ED="anal_Rexp: Subscript",_tag_dyneither(_tmp7ED,sizeof(char),
21))),_tmp7EE)))),_tmp7EC)))));}_LL1AB:;}_LL1A7: {struct Cyc_Absyn_PointerType_struct*
_tmp325=(struct Cyc_Absyn_PointerType_struct*)_tmp322;if(_tmp325->tag != 5)goto
_LL1A9;else{_tmp326=_tmp325->f1;_tmp327=_tmp326.ptr_atts;_tmp328=_tmp327.bounds;}}
_LL1A8: {union Cyc_CfFlowInfo_FlowInfo _tmp332;void*_tmp333;struct _tuple13 _tmp331=
Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp317,_tmp1D8,(void*)((struct Cyc_List_List*)
_check_null(_tmp318))->hd);_tmp332=_tmp331.f1;_tmp333=_tmp331.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp334=_tmp332;int _tmp335;_LL1B1: if((_tmp334.BottomFL).tag != 1)goto _LL1B3;
_tmp335=(int)(_tmp334.BottomFL).val;_LL1B2: {struct _tuple13 _tmp7EF;return(
_tmp7EF.f1=_tmp332,((_tmp7EF.f2=_tmp333,_tmp7EF)));}_LL1B3:;_LL1B4: {struct
_tuple13 _tmp7F0;return(_tmp7F0.f1=_tmp319,((_tmp7F0.f2=_tmp333,_tmp7F0)));}
_LL1B0:;};}_LL1A9:;_LL1AA: {struct Cyc_Core_Impossible_struct _tmp7F6;const char*
_tmp7F5;struct Cyc_Core_Impossible_struct*_tmp7F4;(int)_throw((void*)((_tmp7F4=
_cycalloc(sizeof(*_tmp7F4)),((_tmp7F4[0]=((_tmp7F6.tag=Cyc_Core_Impossible,((
_tmp7F6.f1=((_tmp7F5="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp7F5,
sizeof(char),33))),_tmp7F6)))),_tmp7F4)))));}_LL1A4:;};};}_LL120: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1DA=(struct Cyc_Absyn_Datatype_e_struct*)_tmp16E;if(_tmp1DA->tag != 32)goto
_LL122;else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DA->f2;}}_LL121: _tmp1DE=_tmp1DB;
goto _LL123;_LL122: {struct Cyc_Absyn_Tuple_e_struct*_tmp1DD=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp16E;if(_tmp1DD->tag != 26)goto _LL124;else{_tmp1DE=_tmp1DD->f1;}}_LL123: {
struct _RegionHandle*_tmp33B=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp33D;struct Cyc_List_List*
_tmp33E;struct _tuple19 _tmp33C=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp33B,
env,inflow,_tmp1DE,0);_tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;_tmp33D=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,e->loc,_tmp33D);{unsigned int _tmp7FC;unsigned int _tmp7FB;struct
_dyneither_ptr _tmp7FA;void**_tmp7F9;unsigned int _tmp7F8;struct _dyneither_ptr
aggrdict=(_tmp7F8=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp1DE),((_tmp7F9=(void**)_region_malloc(env->r,_check_times(sizeof(void*),
_tmp7F8)),((_tmp7FA=_tag_dyneither(_tmp7F9,sizeof(void*),_tmp7F8),((((_tmp7FB=
_tmp7F8,_tmp7FD(& _tmp7FC,& _tmp7FB,& _tmp7F9,& _tmp33E))),_tmp7FA)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp803;struct Cyc_CfFlowInfo_Aggregate_struct _tmp802;struct _tuple13 _tmp801;
return(_tmp801.f1=_tmp33D,((_tmp801.f2=(void*)((_tmp803=_region_malloc(env->r,
sizeof(*_tmp803)),((_tmp803[0]=((_tmp802.tag=7,((_tmp802.f1=0,((_tmp802.f2=
aggrdict,_tmp802)))))),_tmp803)))),_tmp801)));};}_LL124: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1DF=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp16E;if(_tmp1DF->tag != 31)goto
_LL126;else{_tmp1E0=_tmp1DF->f2;}}_LL125: {struct Cyc_List_List*fs;{void*_tmp347=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_List_List*_tmp349;_LL1B6: {struct Cyc_Absyn_AnonAggrType_struct*_tmp348=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp347;if(_tmp348->tag != 13)goto _LL1B8;
else{_tmp349=_tmp348->f2;}}_LL1B7: fs=_tmp349;goto _LL1B5;_LL1B8:;_LL1B9: {struct
Cyc_Core_Impossible_struct _tmp809;const char*_tmp808;struct Cyc_Core_Impossible_struct*
_tmp807;(int)_throw((void*)((_tmp807=_cycalloc(sizeof(*_tmp807)),((_tmp807[0]=((
_tmp809.tag=Cyc_Core_Impossible,((_tmp809.f1=((_tmp808="anal_Rexp:anon struct has bad type",
_tag_dyneither(_tmp808,sizeof(char),35))),_tmp809)))),_tmp807)))));}_LL1B5:;}
_tmp1E2=_tmp1E0;_tmp1E5=Cyc_Absyn_StructA;_tmp1E8=fs;goto _LL127;}_LL126: {struct
Cyc_Absyn_Aggregate_e_struct*_tmp1E1=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp16E;if(_tmp1E1->tag != 30)goto _LL128;else{_tmp1E2=_tmp1E1->f3;_tmp1E3=_tmp1E1->f4;
if(_tmp1E3 == 0)goto _LL128;_tmp1E4=*_tmp1E3;_tmp1E5=_tmp1E4.kind;_tmp1E6=_tmp1E4.impl;
if(_tmp1E6 == 0)goto _LL128;_tmp1E7=*_tmp1E6;_tmp1E8=_tmp1E7.fields;}}_LL127: {
struct _RegionHandle*_tmp34D=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp34F;struct Cyc_List_List*
_tmp350;struct _tuple19 _tmp34E=Cyc_NewControlFlow_anal_unordered_Rexps(_tmp34D,
env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple23*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp34D,(struct Cyc_Absyn_Exp*(*)(
struct _tuple23*))Cyc_Core_snd,_tmp1E2),0);_tmp34F=_tmp34E.f1;_tmp350=_tmp34E.f2;
_tmp34F=Cyc_CfFlowInfo_consume_unique_rvals(_tmp166,e->loc,_tmp34F);{struct
_dyneither_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp166,_tmp1E8,
_tmp166->unknown_all);{int i=0;for(0;_tmp350 != 0;(((_tmp350=_tmp350->tl,_tmp1E2=
_tmp1E2->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple23*)((struct Cyc_List_List*)
_check_null(_tmp1E2))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*_tmp351=(void*)ds->hd;
struct _dyneither_ptr*_tmp354;_LL1BB: {struct Cyc_Absyn_ArrayElement_struct*
_tmp352=(struct Cyc_Absyn_ArrayElement_struct*)_tmp351;if(_tmp352->tag != 0)goto
_LL1BD;}_LL1BC: {struct Cyc_Core_Impossible_struct _tmp80F;const char*_tmp80E;
struct Cyc_Core_Impossible_struct*_tmp80D;(int)_throw((void*)((_tmp80D=_cycalloc(
sizeof(*_tmp80D)),((_tmp80D[0]=((_tmp80F.tag=Cyc_Core_Impossible,((_tmp80F.f1=((
_tmp80E="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp80E,sizeof(char),22))),
_tmp80F)))),_tmp80D)))));}_LL1BD: {struct Cyc_Absyn_FieldName_struct*_tmp353=(
struct Cyc_Absyn_FieldName_struct*)_tmp351;if(_tmp353->tag != 1)goto _LL1BA;else{
_tmp354=_tmp353->f1;}}_LL1BE: {int _tmp358=Cyc_CfFlowInfo_get_field_index_fs(
_tmp1E8,_tmp354);*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),
_tmp358))=(void*)_tmp350->hd;if(_tmp1E5 == Cyc_Absyn_UnionA){struct Cyc_Absyn_Exp*
_tmp359=(*((struct _tuple23*)_tmp1E2->hd)).f2;_tmp34F=Cyc_NewControlFlow_use_Rval(
env,_tmp359->loc,_tmp34F,(void*)_tmp350->hd);}}_LL1BA:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp815;struct Cyc_CfFlowInfo_Aggregate_struct _tmp814;struct _tuple13 _tmp813;
return(_tmp813.f1=_tmp34F,((_tmp813.f2=(void*)((_tmp815=_region_malloc(env->r,
sizeof(*_tmp815)),((_tmp815[0]=((_tmp814.tag=7,((_tmp814.f1=_tmp1E5 == Cyc_Absyn_UnionA,((
_tmp814.f2=aggrdict,_tmp814)))))),_tmp815)))),_tmp813)));};};}_LL128: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1E9=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp16E;if(_tmp1E9->tag != 30)goto
_LL12A;}_LL129: {struct Cyc_Core_Impossible_struct _tmp81B;const char*_tmp81A;
struct Cyc_Core_Impossible_struct*_tmp819;(int)_throw((void*)((_tmp819=_cycalloc(
sizeof(*_tmp819)),((_tmp819[0]=((_tmp81B.tag=Cyc_Core_Impossible,((_tmp81B.f1=((
_tmp81A="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp81A,sizeof(char),31))),
_tmp81B)))),_tmp819)))));}_LL12A: {struct Cyc_Absyn_Array_e_struct*_tmp1EA=(
struct Cyc_Absyn_Array_e_struct*)_tmp16E;if(_tmp1EA->tag != 28)goto _LL12C;else{
_tmp1EB=_tmp1EA->f1;}}_LL12B: {struct _RegionHandle*_tmp360=env->r;struct Cyc_List_List*
_tmp361=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(
struct _tuple23*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp360,(struct Cyc_Absyn_Exp*(*)(
struct _tuple23*))Cyc_Core_snd,_tmp1EB);union Cyc_CfFlowInfo_FlowInfo _tmp363;
struct Cyc_List_List*_tmp364;struct _tuple19 _tmp362=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp360,env,inflow,_tmp361,0);_tmp363=_tmp362.f1;_tmp364=_tmp362.f2;_tmp363=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,e->loc,_tmp363);for(0;_tmp364 != 0;(_tmp364=_tmp364->tl,_tmp361=_tmp361->tl)){
_tmp363=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp361))->hd)->loc,_tmp363,(void*)_tmp364->hd);}{struct _tuple13
_tmp81C;return(_tmp81C.f1=_tmp363,((_tmp81C.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp166,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp166->unknown_all),
_tmp81C)));};}_LL12C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp1EC=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp16E;if(_tmp1EC->tag != 29)goto _LL12E;else{_tmp1ED=_tmp1EC->f1;_tmp1EE=_tmp1EC->f2;
_tmp1EF=_tmp1EC->f3;_tmp1F0=_tmp1EC->f4;}}_LL12D: {union Cyc_CfFlowInfo_FlowInfo
_tmp367;void*_tmp368;struct _tuple13 _tmp366=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1EE);_tmp367=_tmp366.f1;_tmp368=_tmp366.f2;_tmp367=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp1EE->loc,_tmp367);{union Cyc_CfFlowInfo_FlowInfo _tmp369=_tmp367;int _tmp36A;
struct _tuple12 _tmp36B;struct Cyc_Dict_Dict _tmp36C;struct Cyc_List_List*_tmp36D;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp36E;_LL1C0: if((_tmp369.BottomFL).tag != 1)
goto _LL1C2;_tmp36A=(int)(_tmp369.BottomFL).val;_LL1C1: {struct _tuple13 _tmp81D;
return(_tmp81D.f1=_tmp367,((_tmp81D.f2=_tmp166->unknown_all,_tmp81D)));}_LL1C2:
if((_tmp369.ReachableFL).tag != 2)goto _LL1BF;_tmp36B=(struct _tuple12)(_tmp369.ReachableFL).val;
_tmp36C=_tmp36B.f1;_tmp36D=_tmp36B.f2;_tmp36E=_tmp36B.f3;_LL1C3: if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp36C,_tmp368)== Cyc_CfFlowInfo_NoneIL){const char*_tmp820;void*
_tmp81F;(_tmp81F=0,Cyc_Tcutil_terr(_tmp1EE->loc,((_tmp820="expression may not be initialized",
_tag_dyneither(_tmp820,sizeof(char),34))),_tag_dyneither(_tmp81F,sizeof(void*),0)));}{
struct Cyc_List_List*new_relns=_tmp36D;comp_loop: {void*_tmp372=_tmp1EE->r;struct
Cyc_Absyn_Exp*_tmp374;void*_tmp376;struct Cyc_Absyn_Vardecl*_tmp378;void*_tmp37A;
struct Cyc_Absyn_Vardecl*_tmp37C;void*_tmp37E;struct Cyc_Absyn_Vardecl*_tmp380;
void*_tmp382;struct Cyc_Absyn_Vardecl*_tmp384;union Cyc_Absyn_Cnst _tmp386;struct
_tuple6 _tmp387;int _tmp388;enum Cyc_Absyn_Primop _tmp38A;struct Cyc_List_List*
_tmp38B;struct Cyc_List_List _tmp38C;struct Cyc_Absyn_Exp*_tmp38D;_LL1C5: {struct
Cyc_Absyn_Cast_e_struct*_tmp373=(struct Cyc_Absyn_Cast_e_struct*)_tmp372;if(
_tmp373->tag != 15)goto _LL1C7;else{_tmp374=_tmp373->f2;}}_LL1C6: _tmp1EE=_tmp374;
goto comp_loop;_LL1C7:{struct Cyc_Absyn_Var_e_struct*_tmp375=(struct Cyc_Absyn_Var_e_struct*)
_tmp372;if(_tmp375->tag != 1)goto _LL1C9;else{_tmp376=(void*)_tmp375->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp377=(struct Cyc_Absyn_Global_b_struct*)_tmp376;if(
_tmp377->tag != 1)goto _LL1C9;else{_tmp378=_tmp377->f1;}};}}if(!(!_tmp378->escapes))
goto _LL1C9;_LL1C8: _tmp37C=_tmp378;goto _LL1CA;_LL1C9:{struct Cyc_Absyn_Var_e_struct*
_tmp379=(struct Cyc_Absyn_Var_e_struct*)_tmp372;if(_tmp379->tag != 1)goto _LL1CB;
else{_tmp37A=(void*)_tmp379->f2;{struct Cyc_Absyn_Local_b_struct*_tmp37B=(struct
Cyc_Absyn_Local_b_struct*)_tmp37A;if(_tmp37B->tag != 4)goto _LL1CB;else{_tmp37C=
_tmp37B->f1;}};}}if(!(!_tmp37C->escapes))goto _LL1CB;_LL1CA: _tmp380=_tmp37C;goto
_LL1CC;_LL1CB:{struct Cyc_Absyn_Var_e_struct*_tmp37D=(struct Cyc_Absyn_Var_e_struct*)
_tmp372;if(_tmp37D->tag != 1)goto _LL1CD;else{_tmp37E=(void*)_tmp37D->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp37F=(struct Cyc_Absyn_Pat_b_struct*)_tmp37E;if(_tmp37F->tag
!= 5)goto _LL1CD;else{_tmp380=_tmp37F->f1;}};}}if(!(!_tmp380->escapes))goto _LL1CD;
_LL1CC: _tmp384=_tmp380;goto _LL1CE;_LL1CD:{struct Cyc_Absyn_Var_e_struct*_tmp381=(
struct Cyc_Absyn_Var_e_struct*)_tmp372;if(_tmp381->tag != 1)goto _LL1CF;else{
_tmp382=(void*)_tmp381->f2;{struct Cyc_Absyn_Param_b_struct*_tmp383=(struct Cyc_Absyn_Param_b_struct*)
_tmp382;if(_tmp383->tag != 3)goto _LL1CF;else{_tmp384=_tmp383->f1;}};}}if(!(!
_tmp384->escapes))goto _LL1CF;_LL1CE:{struct Cyc_CfFlowInfo_Reln*_tmp823;struct Cyc_List_List*
_tmp822;new_relns=((_tmp822=_region_malloc(env->r,sizeof(*_tmp822)),((_tmp822->hd=((
_tmp823=_region_malloc(env->r,sizeof(*_tmp823)),((_tmp823->vd=_tmp1ED,((_tmp823->rop=
Cyc_CfFlowInfo_LessVar(_tmp384,_tmp384->type),_tmp823)))))),((_tmp822->tl=
_tmp36D,_tmp822))))));}goto _LL1C4;_LL1CF: {struct Cyc_Absyn_Const_e_struct*
_tmp385=(struct Cyc_Absyn_Const_e_struct*)_tmp372;if(_tmp385->tag != 0)goto _LL1D1;
else{_tmp386=_tmp385->f1;if((_tmp386.Int_c).tag != 4)goto _LL1D1;_tmp387=(struct
_tuple6)(_tmp386.Int_c).val;_tmp388=_tmp387.f2;}}_LL1D0:{struct Cyc_CfFlowInfo_Reln*
_tmp826;struct Cyc_List_List*_tmp825;new_relns=((_tmp825=_region_malloc(env->r,
sizeof(*_tmp825)),((_tmp825->hd=((_tmp826=_region_malloc(env->r,sizeof(*_tmp826)),((
_tmp826->vd=_tmp1ED,((_tmp826->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp388),_tmp826)))))),((_tmp825->tl=_tmp36D,_tmp825))))));}goto _LL1C4;_LL1D1: {
struct Cyc_Absyn_Primop_e_struct*_tmp389=(struct Cyc_Absyn_Primop_e_struct*)
_tmp372;if(_tmp389->tag != 3)goto _LL1D3;else{_tmp38A=_tmp389->f1;_tmp38B=_tmp389->f2;
if(_tmp38B == 0)goto _LL1D3;_tmp38C=*_tmp38B;_tmp38D=(struct Cyc_Absyn_Exp*)_tmp38C.hd;}}
_LL1D2:{void*_tmp392=_tmp38D->r;void*_tmp394;struct Cyc_Absyn_Vardecl*_tmp396;
void*_tmp398;struct Cyc_Absyn_Vardecl*_tmp39A;void*_tmp39C;struct Cyc_Absyn_Vardecl*
_tmp39E;void*_tmp3A0;struct Cyc_Absyn_Vardecl*_tmp3A2;_LL1D6:{struct Cyc_Absyn_Var_e_struct*
_tmp393=(struct Cyc_Absyn_Var_e_struct*)_tmp392;if(_tmp393->tag != 1)goto _LL1D8;
else{_tmp394=(void*)_tmp393->f2;{struct Cyc_Absyn_Global_b_struct*_tmp395=(struct
Cyc_Absyn_Global_b_struct*)_tmp394;if(_tmp395->tag != 1)goto _LL1D8;else{_tmp396=
_tmp395->f1;}};}}if(!(!_tmp396->escapes))goto _LL1D8;_LL1D7: _tmp39A=_tmp396;goto
_LL1D9;_LL1D8:{struct Cyc_Absyn_Var_e_struct*_tmp397=(struct Cyc_Absyn_Var_e_struct*)
_tmp392;if(_tmp397->tag != 1)goto _LL1DA;else{_tmp398=(void*)_tmp397->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp399=(struct Cyc_Absyn_Local_b_struct*)_tmp398;if(
_tmp399->tag != 4)goto _LL1DA;else{_tmp39A=_tmp399->f1;}};}}if(!(!_tmp39A->escapes))
goto _LL1DA;_LL1D9: _tmp39E=_tmp39A;goto _LL1DB;_LL1DA:{struct Cyc_Absyn_Var_e_struct*
_tmp39B=(struct Cyc_Absyn_Var_e_struct*)_tmp392;if(_tmp39B->tag != 1)goto _LL1DC;
else{_tmp39C=(void*)_tmp39B->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp39D=(struct Cyc_Absyn_Pat_b_struct*)
_tmp39C;if(_tmp39D->tag != 5)goto _LL1DC;else{_tmp39E=_tmp39D->f1;}};}}if(!(!
_tmp39E->escapes))goto _LL1DC;_LL1DB: _tmp3A2=_tmp39E;goto _LL1DD;_LL1DC:{struct Cyc_Absyn_Var_e_struct*
_tmp39F=(struct Cyc_Absyn_Var_e_struct*)_tmp392;if(_tmp39F->tag != 1)goto _LL1DE;
else{_tmp3A0=(void*)_tmp39F->f2;{struct Cyc_Absyn_Param_b_struct*_tmp3A1=(struct
Cyc_Absyn_Param_b_struct*)_tmp3A0;if(_tmp3A1->tag != 3)goto _LL1DE;else{_tmp3A2=
_tmp3A1->f1;}};}}if(!(!_tmp3A2->escapes))goto _LL1DE;_LL1DD:{struct Cyc_CfFlowInfo_Reln*
_tmp829;struct Cyc_List_List*_tmp828;new_relns=((_tmp828=_region_malloc(env->r,
sizeof(*_tmp828)),((_tmp828->hd=((_tmp829=_region_malloc(env->r,sizeof(*_tmp829)),((
_tmp829->vd=_tmp1ED,((_tmp829->rop=Cyc_CfFlowInfo_LessNumelts(_tmp3A2),_tmp829)))))),((
_tmp828->tl=_tmp36D,_tmp828))))));}goto _LL1D5;_LL1DE:;_LL1DF: goto _LL1D5;_LL1D5:;}
goto _LL1C4;_LL1D3:;_LL1D4: goto _LL1C4;_LL1C4:;}if(_tmp36D != new_relns)_tmp367=Cyc_CfFlowInfo_ReachableFL(
_tmp36C,new_relns,_tmp36E);{void*_tmp3A5=_tmp368;_LL1E1: {struct Cyc_CfFlowInfo_Zero_struct*
_tmp3A6=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp3A5;if(_tmp3A6->tag != 0)goto
_LL1E3;}_LL1E2: {struct _tuple13 _tmp82A;return(_tmp82A.f1=_tmp367,((_tmp82A.f2=
_tmp166->unknown_all,_tmp82A)));}_LL1E3: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3A7=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3A5;if(_tmp3A7->tag != 2)
goto _LL1E5;}_LL1E4: goto _LL1E6;_LL1E5: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3A8=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3A5;if(_tmp3A8->tag != 1)
goto _LL1E7;}_LL1E6: goto _LL1E8;_LL1E7: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3A9=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3A5;if(_tmp3A9->tag != 5)goto
_LL1E9;}_LL1E8: {struct Cyc_List_List _tmp82B;struct Cyc_List_List _tmp3AB=(_tmp82B.hd=
_tmp1ED,((_tmp82B.tl=0,_tmp82B)));_tmp367=Cyc_NewControlFlow_add_vars(_tmp166,
_tmp367,(struct Cyc_List_List*)& _tmp3AB,_tmp166->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3AD;void*_tmp3AE;struct _tuple13 _tmp3AC=Cyc_NewControlFlow_anal_Rexp(env,
_tmp367,_tmp1EF);_tmp3AD=_tmp3AC.f1;_tmp3AE=_tmp3AC.f2;_tmp3AD=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,_tmp1EF->loc,_tmp3AD);{union Cyc_CfFlowInfo_FlowInfo _tmp3AF=_tmp3AD;int
_tmp3B0;struct _tuple12 _tmp3B1;struct Cyc_Dict_Dict _tmp3B2;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3B3;_LL1EC: if((_tmp3AF.BottomFL).tag != 1)goto _LL1EE;_tmp3B0=(int)(_tmp3AF.BottomFL).val;
_LL1ED: {struct _tuple13 _tmp82C;return(_tmp82C.f1=_tmp3AD,((_tmp82C.f2=_tmp166->unknown_all,
_tmp82C)));}_LL1EE: if((_tmp3AF.ReachableFL).tag != 2)goto _LL1EB;_tmp3B1=(struct
_tuple12)(_tmp3AF.ReachableFL).val;_tmp3B2=_tmp3B1.f1;_tmp3B3=_tmp3B1.f3;_LL1EF:
if(Cyc_CfFlowInfo_initlevel(_tmp166,_tmp3B2,_tmp3AE)!= Cyc_CfFlowInfo_AllIL){{
const char*_tmp82F;void*_tmp82E;(_tmp82E=0,Cyc_Tcutil_terr(_tmp1EE->loc,((_tmp82F="expression may not be initialized",
_tag_dyneither(_tmp82F,sizeof(char),34))),_tag_dyneither(_tmp82E,sizeof(void*),0)));}{
struct _tuple13 _tmp830;return(_tmp830.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp830.f2=
_tmp166->unknown_all,_tmp830)));};}_LL1EB:;}_tmp367=_tmp3AD;goto _LL1EA;};}_LL1E9:;
_LL1EA: while(1){struct Cyc_List_List _tmp831;struct Cyc_List_List _tmp3B9=(_tmp831.hd=
_tmp1ED,((_tmp831.tl=0,_tmp831)));_tmp367=Cyc_NewControlFlow_add_vars(_tmp166,
_tmp367,(struct Cyc_List_List*)& _tmp3B9,_tmp166->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo
_tmp3BB;void*_tmp3BC;struct _tuple13 _tmp3BA=Cyc_NewControlFlow_anal_Rexp(env,
_tmp367,_tmp1EF);_tmp3BB=_tmp3BA.f1;_tmp3BC=_tmp3BA.f2;_tmp3BB=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp166,_tmp1EF->loc,_tmp3BB);{union Cyc_CfFlowInfo_FlowInfo _tmp3BD=_tmp3BB;int
_tmp3BE;struct _tuple12 _tmp3BF;struct Cyc_Dict_Dict _tmp3C0;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3C1;_LL1F1: if((_tmp3BD.BottomFL).tag != 1)goto _LL1F3;_tmp3BE=(int)(_tmp3BD.BottomFL).val;
_LL1F2: goto _LL1F0;_LL1F3: if((_tmp3BD.ReachableFL).tag != 2)goto _LL1F0;_tmp3BF=(
struct _tuple12)(_tmp3BD.ReachableFL).val;_tmp3C0=_tmp3BF.f1;_tmp3C1=_tmp3BF.f3;
_LL1F4: if(Cyc_CfFlowInfo_initlevel(_tmp166,_tmp3C0,_tmp3BC)!= Cyc_CfFlowInfo_AllIL){{
const char*_tmp834;void*_tmp833;(_tmp833=0,Cyc_Tcutil_terr(_tmp1EE->loc,((_tmp834="expression may not be initialized",
_tag_dyneither(_tmp834,sizeof(char),34))),_tag_dyneither(_tmp833,sizeof(void*),0)));}{
struct _tuple13 _tmp835;return(_tmp835.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp835.f2=
_tmp166->unknown_all,_tmp835)));};}_LL1F0:;}{union Cyc_CfFlowInfo_FlowInfo _tmp3C5=
Cyc_CfFlowInfo_join_flow(_tmp166,env->all_changed,_tmp367,_tmp3BB,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp3C5,_tmp367))break;_tmp367=_tmp3C5;};};}{struct _tuple13 _tmp836;return(
_tmp836.f1=_tmp367,((_tmp836.f2=_tmp166->unknown_all,_tmp836)));};_LL1E0:;};};
_LL1BF:;};}_LL12E: {struct Cyc_Absyn_StmtExp_e_struct*_tmp1F1=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp16E;if(_tmp1F1->tag != 38)goto _LL130;else{_tmp1F2=_tmp1F1->f1;}}_LL12F: while(
1){union Cyc_CfFlowInfo_FlowInfo*_tmp3C9;struct _tuple17 _tmp3C8=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp1F2);_tmp3C9=_tmp3C8.f2;inflow=*_tmp3C9;{void*_tmp3CA=_tmp1F2->r;
struct Cyc_Absyn_Stmt*_tmp3CC;struct Cyc_Absyn_Stmt*_tmp3CD;struct Cyc_Absyn_Decl*
_tmp3CF;struct Cyc_Absyn_Stmt*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D2;_LL1F6: {struct
Cyc_Absyn_Seq_s_struct*_tmp3CB=(struct Cyc_Absyn_Seq_s_struct*)_tmp3CA;if(_tmp3CB->tag
!= 2)goto _LL1F8;else{_tmp3CC=_tmp3CB->f1;_tmp3CD=_tmp3CB->f2;}}_LL1F7: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp3CC);_tmp1F2=_tmp3CD;goto _LL1F5;_LL1F8: {struct Cyc_Absyn_Decl_s_struct*
_tmp3CE=(struct Cyc_Absyn_Decl_s_struct*)_tmp3CA;if(_tmp3CE->tag != 12)goto _LL1FA;
else{_tmp3CF=_tmp3CE->f1;_tmp3D0=_tmp3CE->f2;}}_LL1F9: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp3CF);_tmp1F2=_tmp3D0;goto _LL1F5;_LL1FA: {struct Cyc_Absyn_Exp_s_struct*
_tmp3D1=(struct Cyc_Absyn_Exp_s_struct*)_tmp3CA;if(_tmp3D1->tag != 1)goto _LL1FC;
else{_tmp3D2=_tmp3D1->f1;}}_LL1FB: return Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp3D2);_LL1FC:;_LL1FD: {struct Cyc_Core_Impossible_struct _tmp83C;const char*
_tmp83B;struct Cyc_Core_Impossible_struct*_tmp83A;(int)_throw((void*)((_tmp83A=
_cycalloc(sizeof(*_tmp83A)),((_tmp83A[0]=((_tmp83C.tag=Cyc_Core_Impossible,((
_tmp83C.f1=((_tmp83B="analyze_Rexp: ill-formed StmtExp",_tag_dyneither(_tmp83B,
sizeof(char),33))),_tmp83C)))),_tmp83A)))));}_LL1F5:;};}_LL130: {struct Cyc_Absyn_Var_e_struct*
_tmp1F3=(struct Cyc_Absyn_Var_e_struct*)_tmp16E;if(_tmp1F3->tag != 1)goto _LL132;
else{_tmp1F4=(void*)_tmp1F3->f2;{struct Cyc_Absyn_Unresolved_b_struct*_tmp1F5=(
struct Cyc_Absyn_Unresolved_b_struct*)_tmp1F4;if(_tmp1F5->tag != 0)goto _LL132;};}}
_LL131: goto _LL133;_LL132: {struct Cyc_Absyn_UnknownId_e_struct*_tmp1F6=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp16E;if(_tmp1F6->tag != 2)goto _LL134;}_LL133: goto _LL135;_LL134: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp1F7=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp16E;if(_tmp1F7->tag != 10)goto
_LL136;}_LL135: goto _LL137;_LL136: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1F8=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp16E;if(_tmp1F8->tag != 37)
goto _LL138;}_LL137: goto _LL139;_LL138: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp1F9=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp16E;if(_tmp1F9->tag != 27)goto
_LL13A;}_LL139: goto _LL13B;_LL13A: {struct Cyc_Absyn_Valueof_e_struct*_tmp1FA=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp16E;if(_tmp1FA->tag != 40)goto _LLD1;}_LL13B: {
struct Cyc_Core_Impossible_struct _tmp842;const char*_tmp841;struct Cyc_Core_Impossible_struct*
_tmp840;(int)_throw((void*)((_tmp840=_cycalloc(sizeof(*_tmp840)),((_tmp840[0]=((
_tmp842.tag=Cyc_Core_Impossible,((_tmp842.f1=((_tmp841="anal_Rexp, unexpected exp form",
_tag_dyneither(_tmp841,sizeof(char),31))),_tmp842)))),_tmp840)))));}_LLD1:;};}
static struct _tuple15 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo
f,void*r,struct Cyc_List_List*flds);static struct _tuple15 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,struct Cyc_List_List*flds){
struct Cyc_CfFlowInfo_FlowEnv*_tmp3D9=env->fenv;void*_tmp3DA=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp3DC;void*_tmp3DD;struct Cyc_Absyn_PtrAtts _tmp3DE;union Cyc_Absyn_Constraint*
_tmp3DF;union Cyc_Absyn_Constraint*_tmp3E0;_LL1FF: {struct Cyc_Absyn_PointerType_struct*
_tmp3DB=(struct Cyc_Absyn_PointerType_struct*)_tmp3DA;if(_tmp3DB->tag != 5)goto
_LL201;else{_tmp3DC=_tmp3DB->f1;_tmp3DD=_tmp3DC.elt_typ;_tmp3DE=_tmp3DC.ptr_atts;
_tmp3DF=_tmp3DE.bounds;_tmp3E0=_tmp3DE.zero_term;}}_LL200: {union Cyc_CfFlowInfo_FlowInfo
_tmp3E1=f;int _tmp3E2;struct _tuple12 _tmp3E3;struct Cyc_Dict_Dict _tmp3E4;struct Cyc_List_List*
_tmp3E5;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3E6;_LL204: if((_tmp3E1.BottomFL).tag
!= 1)goto _LL206;_tmp3E2=(int)(_tmp3E1.BottomFL).val;_LL205: {struct _tuple15
_tmp843;return(_tmp843.f1=f,((_tmp843.f2=Cyc_CfFlowInfo_UnknownL(),_tmp843)));}
_LL206: if((_tmp3E1.ReachableFL).tag != 2)goto _LL203;_tmp3E3=(struct _tuple12)(
_tmp3E1.ReachableFL).val;_tmp3E4=_tmp3E3.f1;_tmp3E5=_tmp3E3.f2;_tmp3E6=_tmp3E3.f3;
_LL207: if(Cyc_Tcutil_is_bound_one(_tmp3DF)){void*_tmp3E8=r;struct Cyc_CfFlowInfo_Place*
_tmp3EC;struct Cyc_CfFlowInfo_Place _tmp3ED;void*_tmp3EE;struct Cyc_List_List*
_tmp3EF;enum Cyc_CfFlowInfo_InitLevel _tmp3F2;_LL209: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp3E9=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp3E8;if(_tmp3E9->tag != 1)
goto _LL20B;}_LL20A: goto _LL20C;_LL20B: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp3EA=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp3E8;if(_tmp3EA->tag != 2)
goto _LL20D;}_LL20C:{struct Cyc_CfFlowInfo_NotZero_struct _tmp846;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp845;e->annot=(void*)((_tmp845=_cycalloc(sizeof(*_tmp845)),((_tmp845[0]=((
_tmp846.tag=Cyc_CfFlowInfo_NotZero,((_tmp846.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3E5),_tmp846)))),_tmp845))));}goto _LL208;_LL20D: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp3EB=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp3E8;if(_tmp3EB->tag != 5)goto
_LL20F;else{_tmp3EC=_tmp3EB->f1;_tmp3ED=*_tmp3EC;_tmp3EE=_tmp3ED.root;_tmp3EF=
_tmp3ED.fields;}}_LL20E:{struct Cyc_CfFlowInfo_NotZero_struct _tmp849;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp848;e->annot=(void*)((_tmp848=_cycalloc(sizeof(*_tmp848)),((_tmp848[0]=((
_tmp849.tag=Cyc_CfFlowInfo_NotZero,((_tmp849.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3E5),_tmp849)))),_tmp848))));}{struct Cyc_CfFlowInfo_Place*_tmp84C;struct
_tuple15 _tmp84B;return(_tmp84B.f1=f,((_tmp84B.f2=Cyc_CfFlowInfo_PlaceL(((_tmp84C=
_region_malloc(_tmp3D9->r,sizeof(*_tmp84C)),((_tmp84C->root=_tmp3EE,((_tmp84C->fields=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(_tmp3D9->r,_tmp3EF,flds),_tmp84C))))))),_tmp84B)));};_LL20F: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp3F0=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp3E8;if(_tmp3F0->tag != 0)goto _LL211;}_LL210: e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple15 _tmp84D;return(_tmp84D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp84D.f2=
Cyc_CfFlowInfo_UnknownL(),_tmp84D)));};_LL211: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp3F1=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3E8;if(_tmp3F1->tag != 3)goto
_LL213;else{_tmp3F2=_tmp3F1->f1;}}_LL212: f=Cyc_NewControlFlow_notzero(env,inflow,
f,e,_tmp3F2);goto _LL214;_LL213:;_LL214: {struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp850;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp84F;e->annot=(void*)((_tmp84F=
_cycalloc(sizeof(*_tmp84F)),((_tmp84F[0]=((_tmp850.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp850.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp3E5),_tmp850)))),
_tmp84F))));}_LL208:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct _tmp853;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp852;e->annot=(void*)((_tmp852=_cycalloc(sizeof(*_tmp852)),((_tmp852[0]=((
_tmp853.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp853.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp3E5),_tmp853)))),_tmp852))));}if(Cyc_CfFlowInfo_initlevel(_tmp3D9,_tmp3E4,r)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp856;void*_tmp855;(_tmp855=0,Cyc_Tcutil_terr(
e->loc,((_tmp856="dereference of possibly uninitialized pointer",_tag_dyneither(
_tmp856,sizeof(char),46))),_tag_dyneither(_tmp855,sizeof(void*),0)));}{struct
_tuple15 _tmp857;return(_tmp857.f1=f,((_tmp857.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp857)));};_LL203:;}_LL201:;_LL202: {struct Cyc_Core_Impossible_struct _tmp85D;
const char*_tmp85C;struct Cyc_Core_Impossible_struct*_tmp85B;(int)_throw((void*)((
_tmp85B=_cycalloc(sizeof(*_tmp85B)),((_tmp85B[0]=((_tmp85D.tag=Cyc_Core_Impossible,((
_tmp85D.f1=((_tmp85C="left deref of non-pointer-type",_tag_dyneither(_tmp85C,
sizeof(char),31))),_tmp85D)))),_tmp85B)))));}_LL1FE:;}static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,struct Cyc_List_List*flds);static struct _tuple15 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*
_tmp404=env->fenv;{union Cyc_CfFlowInfo_FlowInfo _tmp405=inflow;int _tmp406;struct
_tuple12 _tmp407;struct Cyc_Dict_Dict _tmp408;struct Cyc_List_List*_tmp409;_LL216:
if((_tmp405.BottomFL).tag != 1)goto _LL218;_tmp406=(int)(_tmp405.BottomFL).val;
_LL217: {struct _tuple15 _tmp85E;return(_tmp85E.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp85E.f2=Cyc_CfFlowInfo_UnknownL(),_tmp85E)));}_LL218: if((_tmp405.ReachableFL).tag
!= 2)goto _LL215;_tmp407=(struct _tuple12)(_tmp405.ReachableFL).val;_tmp408=
_tmp407.f1;_tmp409=_tmp407.f2;_LL219: d=_tmp408;_LL215:;}{void*_tmp40B=e->r;
struct Cyc_Absyn_Exp*_tmp40D;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Exp*
_tmp411;void*_tmp413;struct Cyc_Absyn_Vardecl*_tmp415;void*_tmp417;struct Cyc_Absyn_Vardecl*
_tmp419;void*_tmp41B;struct Cyc_Absyn_Vardecl*_tmp41D;void*_tmp41F;struct Cyc_Absyn_Vardecl*
_tmp421;struct Cyc_Absyn_Exp*_tmp423;struct _dyneither_ptr*_tmp424;struct Cyc_Absyn_Exp*
_tmp426;struct Cyc_Absyn_Exp*_tmp428;struct Cyc_Absyn_Exp*_tmp429;struct Cyc_Absyn_Exp*
_tmp42B;struct _dyneither_ptr*_tmp42C;_LL21B: {struct Cyc_Absyn_Cast_e_struct*
_tmp40C=(struct Cyc_Absyn_Cast_e_struct*)_tmp40B;if(_tmp40C->tag != 15)goto _LL21D;
else{_tmp40D=_tmp40C->f2;}}_LL21C: _tmp40F=_tmp40D;goto _LL21E;_LL21D: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp40E=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp40B;if(_tmp40E->tag != 13)
goto _LL21F;else{_tmp40F=_tmp40E->f1;}}_LL21E: _tmp411=_tmp40F;goto _LL220;_LL21F: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp410=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp40B;if(_tmp410->tag != 14)goto _LL221;else{_tmp411=_tmp410->f1;}}_LL220: return
Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp411,flds);_LL221: {struct Cyc_Absyn_Var_e_struct*
_tmp412=(struct Cyc_Absyn_Var_e_struct*)_tmp40B;if(_tmp412->tag != 1)goto _LL223;
else{_tmp413=(void*)_tmp412->f2;{struct Cyc_Absyn_Param_b_struct*_tmp414=(struct
Cyc_Absyn_Param_b_struct*)_tmp413;if(_tmp414->tag != 3)goto _LL223;else{_tmp415=
_tmp414->f1;}};}}_LL222: _tmp419=_tmp415;goto _LL224;_LL223: {struct Cyc_Absyn_Var_e_struct*
_tmp416=(struct Cyc_Absyn_Var_e_struct*)_tmp40B;if(_tmp416->tag != 1)goto _LL225;
else{_tmp417=(void*)_tmp416->f2;{struct Cyc_Absyn_Local_b_struct*_tmp418=(struct
Cyc_Absyn_Local_b_struct*)_tmp417;if(_tmp418->tag != 4)goto _LL225;else{_tmp419=
_tmp418->f1;}};}}_LL224: _tmp41D=_tmp419;goto _LL226;_LL225: {struct Cyc_Absyn_Var_e_struct*
_tmp41A=(struct Cyc_Absyn_Var_e_struct*)_tmp40B;if(_tmp41A->tag != 1)goto _LL227;
else{_tmp41B=(void*)_tmp41A->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp41C=(struct Cyc_Absyn_Pat_b_struct*)
_tmp41B;if(_tmp41C->tag != 5)goto _LL227;else{_tmp41D=_tmp41C->f1;}};}}_LL226: {
struct Cyc_CfFlowInfo_Place*_tmp868;struct Cyc_CfFlowInfo_VarRoot_struct _tmp867;
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp866;struct _tuple15 _tmp865;return(_tmp865.f1=
inflow,((_tmp865.f2=Cyc_CfFlowInfo_PlaceL(((_tmp868=_region_malloc(env->r,
sizeof(*_tmp868)),((_tmp868->root=(void*)((_tmp866=_region_malloc(env->r,sizeof(*
_tmp866)),((_tmp866[0]=((_tmp867.tag=0,((_tmp867.f1=_tmp41D,_tmp867)))),_tmp866)))),((
_tmp868->fields=flds,_tmp868))))))),_tmp865)));}_LL227: {struct Cyc_Absyn_Var_e_struct*
_tmp41E=(struct Cyc_Absyn_Var_e_struct*)_tmp40B;if(_tmp41E->tag != 1)goto _LL229;
else{_tmp41F=(void*)_tmp41E->f2;{struct Cyc_Absyn_Global_b_struct*_tmp420=(struct
Cyc_Absyn_Global_b_struct*)_tmp41F;if(_tmp420->tag != 1)goto _LL229;else{_tmp421=
_tmp420->f1;}};}}_LL228: {struct _tuple15 _tmp869;return(_tmp869.f1=inflow,((
_tmp869.f2=Cyc_CfFlowInfo_UnknownL(),_tmp869)));}_LL229: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp422=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp40B;if(_tmp422->tag != 24)goto
_LL22B;else{_tmp423=_tmp422->f1;_tmp424=_tmp422->f2;}}_LL22A:{void*_tmp432=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp423->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp434;void*_tmp435;_LL234: {struct Cyc_Absyn_PointerType_struct*_tmp433=(struct
Cyc_Absyn_PointerType_struct*)_tmp432;if(_tmp433->tag != 5)goto _LL236;else{
_tmp434=_tmp433->f1;_tmp435=_tmp434.elt_typ;}}_LL235: if(!Cyc_Absyn_is_nontagged_union_type(
_tmp435)){struct Cyc_List_List*_tmp86A;flds=((_tmp86A=_region_malloc(env->r,
sizeof(*_tmp86A)),((_tmp86A->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp435,
_tmp424),((_tmp86A->tl=flds,_tmp86A))))));}goto _LL233;_LL236:;_LL237: {struct Cyc_Core_Impossible_struct
_tmp870;const char*_tmp86F;struct Cyc_Core_Impossible_struct*_tmp86E;(int)_throw((
void*)((_tmp86E=_cycalloc(sizeof(*_tmp86E)),((_tmp86E[0]=((_tmp870.tag=Cyc_Core_Impossible,((
_tmp870.f1=((_tmp86F="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp86F,sizeof(
char),25))),_tmp870)))),_tmp86E)))));}_LL233:;}_tmp426=_tmp423;goto _LL22C;_LL22B: {
struct Cyc_Absyn_Deref_e_struct*_tmp425=(struct Cyc_Absyn_Deref_e_struct*)_tmp40B;
if(_tmp425->tag != 22)goto _LL22D;else{_tmp426=_tmp425->f1;}}_LL22C: {union Cyc_CfFlowInfo_FlowInfo
_tmp43B;void*_tmp43C;struct _tuple13 _tmp43A=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp426);_tmp43B=_tmp43A.f1;_tmp43C=_tmp43A.f2;_tmp43B=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp43B);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp426,_tmp43B,
_tmp43C,flds);}_LL22D: {struct Cyc_Absyn_Subscript_e_struct*_tmp427=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp40B;if(_tmp427->tag != 25)goto _LL22F;else{_tmp428=_tmp427->f1;_tmp429=_tmp427->f2;}}
_LL22E: {void*_tmp43D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp428->topt))->v);struct Cyc_Absyn_PtrInfo _tmp440;struct Cyc_Absyn_PtrAtts
_tmp441;union Cyc_Absyn_Constraint*_tmp442;_LL239: {struct Cyc_Absyn_TupleType_struct*
_tmp43E=(struct Cyc_Absyn_TupleType_struct*)_tmp43D;if(_tmp43E->tag != 11)goto
_LL23B;}_LL23A: {unsigned int _tmp443=(Cyc_Evexp_eval_const_uint_exp(_tmp429)).f1;
struct Cyc_List_List*_tmp871;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,
_tmp428,((_tmp871=_region_malloc(env->r,sizeof(*_tmp871)),((_tmp871->hd=(void*)((
int)_tmp443),((_tmp871->tl=flds,_tmp871)))))));}_LL23B: {struct Cyc_Absyn_PointerType_struct*
_tmp43F=(struct Cyc_Absyn_PointerType_struct*)_tmp43D;if(_tmp43F->tag != 5)goto
_LL23D;else{_tmp440=_tmp43F->f1;_tmp441=_tmp440.ptr_atts;_tmp442=_tmp441.bounds;}}
_LL23C: {struct _RegionHandle*_tmp445=env->r;union Cyc_CfFlowInfo_FlowInfo _tmp448;
struct Cyc_List_List*_tmp449;struct Cyc_Absyn_Exp*_tmp872[2];struct _tuple19 _tmp447=
Cyc_NewControlFlow_anal_unordered_Rexps(_tmp445,env,inflow,((_tmp872[1]=_tmp429,((
_tmp872[0]=_tmp428,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_dyneither_ptr))Cyc_List_rlist)(_tmp445,_tag_dyneither(_tmp872,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);_tmp448=_tmp447.f1;_tmp449=_tmp447.f2;_tmp448=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp429->loc,_tmp448);{union Cyc_CfFlowInfo_FlowInfo _tmp44A=_tmp448;{union Cyc_CfFlowInfo_FlowInfo
_tmp44B=_tmp448;struct _tuple12 _tmp44C;struct Cyc_Dict_Dict _tmp44D;struct Cyc_List_List*
_tmp44E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp44F;_LL240: if((_tmp44B.ReachableFL).tag
!= 2)goto _LL242;_tmp44C=(struct _tuple12)(_tmp44B.ReachableFL).val;_tmp44D=
_tmp44C.f1;_tmp44E=_tmp44C.f2;_tmp44F=_tmp44C.f3;_LL241: if(Cyc_CfFlowInfo_initlevel(
_tmp404,_tmp44D,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp449))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){const char*_tmp875;void*
_tmp874;(_tmp874=0,Cyc_Tcutil_terr(_tmp429->loc,((_tmp875="expression may not be initialized",
_tag_dyneither(_tmp875,sizeof(char),34))),_tag_dyneither(_tmp874,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp452=Cyc_NewControlFlow_add_subscript_reln(_tmp404->r,
_tmp44E,_tmp428,_tmp429);if(_tmp44E != _tmp452)_tmp44A=Cyc_CfFlowInfo_ReachableFL(
_tmp44D,_tmp452,_tmp44F);goto _LL23F;};_LL242:;_LL243: goto _LL23F;_LL23F:;}{union
Cyc_CfFlowInfo_FlowInfo _tmp454;union Cyc_CfFlowInfo_AbsLVal _tmp455;struct _tuple15
_tmp453=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp428,_tmp448,(void*)((
struct Cyc_List_List*)_check_null(_tmp449))->hd,flds);_tmp454=_tmp453.f1;_tmp455=
_tmp453.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp456=_tmp454;int _tmp457;_LL245: if((
_tmp456.BottomFL).tag != 1)goto _LL247;_tmp457=(int)(_tmp456.BottomFL).val;_LL246: {
struct _tuple15 _tmp876;return(_tmp876.f1=_tmp454,((_tmp876.f2=_tmp455,_tmp876)));}
_LL247:;_LL248: {struct _tuple15 _tmp877;return(_tmp877.f1=_tmp44A,((_tmp877.f2=
_tmp455,_tmp877)));}_LL244:;};};};}_LL23D:;_LL23E: {struct Cyc_Core_Impossible_struct
_tmp87D;const char*_tmp87C;struct Cyc_Core_Impossible_struct*_tmp87B;(int)_throw((
void*)((_tmp87B=_cycalloc(sizeof(*_tmp87B)),((_tmp87B[0]=((_tmp87D.tag=Cyc_Core_Impossible,((
_tmp87D.f1=((_tmp87C="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp87C,
sizeof(char),33))),_tmp87D)))),_tmp87B)))));}_LL238:;}_LL22F: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp42A=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp40B;if(_tmp42A->tag != 23)goto
_LL231;else{_tmp42B=_tmp42A->f1;_tmp42C=_tmp42A->f2;}}_LL230: if(Cyc_Absyn_is_nontagged_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp42B->topt))->v)){struct _tuple15
_tmp87E;return(_tmp87E.f1=inflow,((_tmp87E.f2=Cyc_CfFlowInfo_UnknownL(),_tmp87E)));}{
struct Cyc_List_List*_tmp87F;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,
_tmp42B,((_tmp87F=_region_malloc(env->r,sizeof(*_tmp87F)),((_tmp87F->hd=(void*)
Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)_check_null(_tmp42B->topt))->v,
_tmp42C),((_tmp87F->tl=flds,_tmp87F)))))));};_LL231:;_LL232: {struct _tuple15
_tmp880;return(_tmp880.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp880.f2=Cyc_CfFlowInfo_UnknownL(),
_tmp880)));}_LL21A:;};}static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*
e);static struct _tuple15 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_ConsumeInfo
_tmp461;struct _tuple14 _tmp460=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,
0);_tmp461=_tmp460.f1;{union Cyc_CfFlowInfo_FlowInfo _tmp463;union Cyc_CfFlowInfo_AbsLVal
_tmp464;struct _tuple15 _tmp462=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,e,0);
_tmp463=_tmp462.f1;_tmp464=_tmp462.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp466;
struct _tuple14 _tmp465=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,0);
_tmp466=_tmp465.f1;if(_tmp461.may_consume != _tmp466.may_consume  || (_tmp461.consumed).t
!= (_tmp466.consumed).t){const char*_tmp883;void*_tmp882;(_tmp882=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp883="oops: anal_Lexp modified consume lists",
_tag_dyneither(_tmp883,sizeof(char),39))),_tag_dyneither(_tmp882,sizeof(void*),0)));}{
struct _tuple15 _tmp884;return(_tmp884.f1=_tmp463,((_tmp884.f2=_tmp464,_tmp884)));};};};}
static struct _tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static struct
_tuple16 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*
_tmp46A=env->fenv;{void*_tmp46B=e->r;struct Cyc_Absyn_Exp*_tmp46D;struct Cyc_Absyn_Exp*
_tmp46E;struct Cyc_Absyn_Exp*_tmp46F;struct Cyc_Absyn_Exp*_tmp471;struct Cyc_Absyn_Exp*
_tmp472;struct Cyc_Absyn_Exp*_tmp474;struct Cyc_Absyn_Exp*_tmp475;struct Cyc_Absyn_Exp*
_tmp477;struct Cyc_Absyn_Exp*_tmp478;enum Cyc_Absyn_Primop _tmp47A;struct Cyc_List_List*
_tmp47B;struct Cyc_List_List _tmp47C;struct Cyc_Absyn_Exp*_tmp47D;struct Cyc_List_List*
_tmp47E;enum Cyc_Absyn_Primop _tmp480;struct Cyc_List_List*_tmp481;_LL24A: {struct
Cyc_Absyn_Conditional_e_struct*_tmp46C=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp46B;if(_tmp46C->tag != 6)goto _LL24C;else{_tmp46D=_tmp46C->f1;_tmp46E=_tmp46C->f2;
_tmp46F=_tmp46C->f3;}}_LL24B: {union Cyc_CfFlowInfo_FlowInfo _tmp483;union Cyc_CfFlowInfo_FlowInfo
_tmp484;struct _tuple16 _tmp482=Cyc_NewControlFlow_anal_test(env,inflow,_tmp46D);
_tmp483=_tmp482.f1;_tmp484=_tmp482.f2;_tmp483=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp46D->loc,_tmp483);_tmp484=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp46D->loc,
_tmp484);{union Cyc_CfFlowInfo_FlowInfo _tmp486;union Cyc_CfFlowInfo_FlowInfo
_tmp487;struct _tuple16 _tmp485=Cyc_NewControlFlow_anal_test(env,_tmp483,_tmp46E);
_tmp486=_tmp485.f1;_tmp487=_tmp485.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp489;union
Cyc_CfFlowInfo_FlowInfo _tmp48A;struct _tuple16 _tmp488=Cyc_NewControlFlow_anal_test(
env,_tmp484,_tmp46F);_tmp489=_tmp488.f1;_tmp48A=_tmp488.f2;{struct _tuple16
_tmp885;return(_tmp885.f1=Cyc_CfFlowInfo_join_flow(_tmp46A,env->all_changed,
_tmp486,_tmp489,1),((_tmp885.f2=Cyc_CfFlowInfo_join_flow(_tmp46A,env->all_changed,
_tmp487,_tmp48A,1),_tmp885)));};};};}_LL24C: {struct Cyc_Absyn_And_e_struct*
_tmp470=(struct Cyc_Absyn_And_e_struct*)_tmp46B;if(_tmp470->tag != 7)goto _LL24E;
else{_tmp471=_tmp470->f1;_tmp472=_tmp470->f2;}}_LL24D: {union Cyc_CfFlowInfo_FlowInfo
_tmp48D;union Cyc_CfFlowInfo_FlowInfo _tmp48E;struct _tuple16 _tmp48C=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp471);_tmp48D=_tmp48C.f1;_tmp48E=_tmp48C.f2;_tmp48D=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp471->loc,_tmp48D);_tmp48E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp471->loc,
_tmp48E);{union Cyc_CfFlowInfo_FlowInfo _tmp490;union Cyc_CfFlowInfo_FlowInfo
_tmp491;struct _tuple16 _tmp48F=Cyc_NewControlFlow_anal_test(env,_tmp48D,_tmp472);
_tmp490=_tmp48F.f1;_tmp491=_tmp48F.f2;_tmp490=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp472->loc,_tmp490);_tmp491=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp472->loc,
_tmp491);{struct _tuple16 _tmp886;return(_tmp886.f1=_tmp490,((_tmp886.f2=Cyc_CfFlowInfo_join_flow(
_tmp46A,env->all_changed,_tmp48E,_tmp491,0),_tmp886)));};};}_LL24E: {struct Cyc_Absyn_Or_e_struct*
_tmp473=(struct Cyc_Absyn_Or_e_struct*)_tmp46B;if(_tmp473->tag != 8)goto _LL250;
else{_tmp474=_tmp473->f1;_tmp475=_tmp473->f2;}}_LL24F: {union Cyc_CfFlowInfo_FlowInfo
_tmp494;union Cyc_CfFlowInfo_FlowInfo _tmp495;struct _tuple16 _tmp493=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp474);_tmp494=_tmp493.f1;_tmp495=_tmp493.f2;_tmp494=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp474->loc,_tmp494);_tmp495=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp474->loc,
_tmp495);{union Cyc_CfFlowInfo_FlowInfo _tmp497;union Cyc_CfFlowInfo_FlowInfo
_tmp498;struct _tuple16 _tmp496=Cyc_NewControlFlow_anal_test(env,_tmp495,_tmp475);
_tmp497=_tmp496.f1;_tmp498=_tmp496.f2;_tmp497=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp475->loc,_tmp497);_tmp498=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp475->loc,
_tmp498);{struct _tuple16 _tmp887;return(_tmp887.f1=Cyc_CfFlowInfo_join_flow(
_tmp46A,env->all_changed,_tmp494,_tmp497,0),((_tmp887.f2=_tmp498,_tmp887)));};};}
_LL250: {struct Cyc_Absyn_SeqExp_e_struct*_tmp476=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp46B;if(_tmp476->tag != 9)goto _LL252;else{_tmp477=_tmp476->f1;_tmp478=_tmp476->f2;}}
_LL251: {union Cyc_CfFlowInfo_FlowInfo _tmp49B;void*_tmp49C;struct _tuple13 _tmp49A=
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp477);_tmp49B=_tmp49A.f1;_tmp49C=
_tmp49A.f2;_tmp49B=Cyc_CfFlowInfo_drop_unique_rvals(_tmp477->loc,_tmp49B);return
Cyc_NewControlFlow_anal_test(env,_tmp49B,_tmp478);}_LL252: {struct Cyc_Absyn_Primop_e_struct*
_tmp479=(struct Cyc_Absyn_Primop_e_struct*)_tmp46B;if(_tmp479->tag != 3)goto _LL254;
else{_tmp47A=_tmp479->f1;if(_tmp47A != Cyc_Absyn_Not)goto _LL254;_tmp47B=_tmp479->f2;
if(_tmp47B == 0)goto _LL254;_tmp47C=*_tmp47B;_tmp47D=(struct Cyc_Absyn_Exp*)_tmp47C.hd;
_tmp47E=_tmp47C.tl;if(_tmp47E != 0)goto _LL254;}}_LL253: {union Cyc_CfFlowInfo_FlowInfo
_tmp49E;union Cyc_CfFlowInfo_FlowInfo _tmp49F;struct _tuple16 _tmp49D=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp47D);_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;{struct _tuple16 _tmp888;
return(_tmp888.f1=_tmp49F,((_tmp888.f2=_tmp49E,_tmp888)));};}_LL254: {struct Cyc_Absyn_Primop_e_struct*
_tmp47F=(struct Cyc_Absyn_Primop_e_struct*)_tmp46B;if(_tmp47F->tag != 3)goto _LL256;
else{_tmp480=_tmp47F->f1;_tmp481=_tmp47F->f2;}}_LL255: {void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo
f;struct _RegionHandle*_tmp4A1=env->r;{union Cyc_CfFlowInfo_FlowInfo _tmp4A3;struct
Cyc_List_List*_tmp4A4;struct _tuple19 _tmp4A2=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp4A1,env,inflow,_tmp481,0);_tmp4A3=_tmp4A2.f1;_tmp4A4=_tmp4A2.f2;r1=(void*)((
struct Cyc_List_List*)_check_null(_tmp4A4))->hd;r2=(void*)((struct Cyc_List_List*)
_check_null(_tmp4A4->tl))->hd;f=_tmp4A3;}{union Cyc_CfFlowInfo_FlowInfo _tmp4A5=f;
int _tmp4A6;struct _tuple12 _tmp4A7;struct Cyc_Dict_Dict _tmp4A8;struct Cyc_List_List*
_tmp4A9;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4AA;_LL259: if((_tmp4A5.BottomFL).tag
!= 1)goto _LL25B;_tmp4A6=(int)(_tmp4A5.BottomFL).val;_LL25A: {struct _tuple16
_tmp889;return(_tmp889.f1=f,((_tmp889.f2=f,_tmp889)));}_LL25B: if((_tmp4A5.ReachableFL).tag
!= 2)goto _LL258;_tmp4A7=(struct _tuple12)(_tmp4A5.ReachableFL).val;_tmp4A8=
_tmp4A7.f1;_tmp4A9=_tmp4A7.f2;_tmp4AA=_tmp4A7.f3;_LL25C: {struct Cyc_Absyn_Exp*
_tmp4AC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp481))->hd;
struct Cyc_Absyn_Exp*_tmp4AD=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp481->tl))->hd;if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4A8,r1)
== Cyc_CfFlowInfo_NoneIL){const char*_tmp88C;void*_tmp88B;(_tmp88B=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp481->hd)->loc,((_tmp88C="expression may not be initialized",
_tag_dyneither(_tmp88C,sizeof(char),34))),_tag_dyneither(_tmp88B,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4A8,r2)== Cyc_CfFlowInfo_NoneIL){const
char*_tmp88F;void*_tmp88E;(_tmp88E=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp481->tl))->hd)->loc,((_tmp88F="expression may not be initialized",
_tag_dyneither(_tmp88F,sizeof(char),34))),_tag_dyneither(_tmp88E,sizeof(void*),0)));}
if(_tmp480 == Cyc_Absyn_Eq  || _tmp480 == Cyc_Absyn_Neq){struct _tuple0 _tmp890;
struct _tuple0 _tmp4B3=(_tmp890.f1=r1,((_tmp890.f2=r2,_tmp890)));void*_tmp4B4;
enum Cyc_CfFlowInfo_InitLevel _tmp4B6;void*_tmp4B7;void*_tmp4B9;void*_tmp4BB;
enum Cyc_CfFlowInfo_InitLevel _tmp4BD;void*_tmp4BE;void*_tmp4C0;void*_tmp4C2;void*
_tmp4C4;void*_tmp4C6;void*_tmp4C8;void*_tmp4CA;void*_tmp4CC;void*_tmp4CE;void*
_tmp4D0;void*_tmp4D2;void*_tmp4D4;void*_tmp4D6;void*_tmp4D8;_LL25E: _tmp4B4=
_tmp4B3.f1;{struct Cyc_CfFlowInfo_UnknownR_struct*_tmp4B5=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4B4;if(_tmp4B5->tag != 3)goto _LL260;else{_tmp4B6=_tmp4B5->f1;}};_tmp4B7=
_tmp4B3.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4B8=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4B7;if(_tmp4B8->tag != 0)goto _LL260;};_LL25F: {union Cyc_CfFlowInfo_FlowInfo
_tmp4DB;union Cyc_CfFlowInfo_FlowInfo _tmp4DC;struct _tuple16 _tmp4DA=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4AC,_tmp4B6);_tmp4DB=_tmp4DA.f1;_tmp4DC=_tmp4DA.f2;switch(_tmp480){
case Cyc_Absyn_Eq: _LL272: {struct _tuple16 _tmp891;return(_tmp891.f1=_tmp4DC,((
_tmp891.f2=_tmp4DB,_tmp891)));}case Cyc_Absyn_Neq: _LL273: {struct _tuple16 _tmp892;
return(_tmp892.f1=_tmp4DB,((_tmp892.f2=_tmp4DC,_tmp892)));}default: _LL274: {
struct Cyc_Core_Impossible_struct _tmp898;const char*_tmp897;struct Cyc_Core_Impossible_struct*
_tmp896;(int)_throw((void*)((_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896[0]=((
_tmp898.tag=Cyc_Core_Impossible,((_tmp898.f1=((_tmp897="anal_test, zero-split",
_tag_dyneither(_tmp897,sizeof(char),22))),_tmp898)))),_tmp896)))));}}}_LL260:
_tmp4B9=_tmp4B3.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4BA=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4B9;if(_tmp4BA->tag != 0)goto _LL262;};_tmp4BB=_tmp4B3.f2;{struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp4BC=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4BB;if(_tmp4BC->tag != 3)goto
_LL262;else{_tmp4BD=_tmp4BC->f1;}};_LL261: {union Cyc_CfFlowInfo_FlowInfo _tmp4E3;
union Cyc_CfFlowInfo_FlowInfo _tmp4E4;struct _tuple16 _tmp4E2=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp4AD,_tmp4BD);_tmp4E3=_tmp4E2.f1;_tmp4E4=_tmp4E2.f2;switch(_tmp480){
case Cyc_Absyn_Eq: _LL276: {struct _tuple16 _tmp899;return(_tmp899.f1=_tmp4E4,((
_tmp899.f2=_tmp4E3,_tmp899)));}case Cyc_Absyn_Neq: _LL277: {struct _tuple16 _tmp89A;
return(_tmp89A.f1=_tmp4E3,((_tmp89A.f2=_tmp4E4,_tmp89A)));}default: _LL278: {
struct Cyc_Core_Impossible_struct _tmp8A0;const char*_tmp89F;struct Cyc_Core_Impossible_struct*
_tmp89E;(int)_throw((void*)((_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=((
_tmp8A0.tag=Cyc_Core_Impossible,((_tmp8A0.f1=((_tmp89F="anal_test, zero-split",
_tag_dyneither(_tmp89F,sizeof(char),22))),_tmp8A0)))),_tmp89E)))));}}}_LL262:
_tmp4BE=_tmp4B3.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4BF=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4BE;if(_tmp4BF->tag != 0)goto _LL264;};_tmp4C0=_tmp4B3.f2;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4C1=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4C0;if(_tmp4C1->tag != 0)goto
_LL264;};_LL263: if(_tmp480 == Cyc_Absyn_Eq){struct _tuple16 _tmp8A1;return(_tmp8A1.f1=
f,((_tmp8A1.f2=Cyc_CfFlowInfo_BottomFL(),_tmp8A1)));}else{struct _tuple16 _tmp8A2;
return(_tmp8A2.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8A2.f2=f,_tmp8A2)));}_LL264:
_tmp4C2=_tmp4B3.f1;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4C3=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4C2;if(_tmp4C3->tag != 0)goto _LL266;};_tmp4C4=_tmp4B3.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4C5=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4C4;if(_tmp4C5->tag != 1)
goto _LL266;};_LL265: goto _LL267;_LL266: _tmp4C6=_tmp4B3.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4C7=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4C6;if(_tmp4C7->tag != 0)goto
_LL268;};_tmp4C8=_tmp4B3.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp4C9=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4C8;if(_tmp4C9->tag != 2)goto _LL268;};
_LL267: goto _LL269;_LL268: _tmp4CA=_tmp4B3.f1;{struct Cyc_CfFlowInfo_Zero_struct*
_tmp4CB=(struct Cyc_CfFlowInfo_Zero_struct*)_tmp4CA;if(_tmp4CB->tag != 0)goto
_LL26A;};_tmp4CC=_tmp4B3.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp4CD=(
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4CC;if(_tmp4CD->tag != 5)goto _LL26A;};
_LL269: goto _LL26B;_LL26A: _tmp4CE=_tmp4B3.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp4CF=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp4CE;if(_tmp4CF->tag != 1)
goto _LL26C;};_tmp4D0=_tmp4B3.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D1=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4D0;if(_tmp4D1->tag != 0)goto _LL26C;};
_LL26B: goto _LL26D;_LL26C: _tmp4D2=_tmp4B3.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp4D3=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp4D2;if(_tmp4D3->tag != 2)
goto _LL26E;};_tmp4D4=_tmp4B3.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D5=(
struct Cyc_CfFlowInfo_Zero_struct*)_tmp4D4;if(_tmp4D5->tag != 0)goto _LL26E;};
_LL26D: goto _LL26F;_LL26E: _tmp4D6=_tmp4B3.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp4D7=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp4D6;if(_tmp4D7->tag != 5)goto
_LL270;};_tmp4D8=_tmp4B3.f2;{struct Cyc_CfFlowInfo_Zero_struct*_tmp4D9=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp4D8;if(_tmp4D9->tag != 0)goto _LL270;};_LL26F: if(_tmp480 == Cyc_Absyn_Neq){
struct _tuple16 _tmp8A3;return(_tmp8A3.f1=f,((_tmp8A3.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp8A3)));}else{struct _tuple16 _tmp8A4;return(_tmp8A4.f1=Cyc_CfFlowInfo_BottomFL(),((
_tmp8A4.f2=f,_tmp8A4)));}_LL270:;_LL271: goto _LL25D;_LL25D:;}{struct _tuple0
_tmp8A5;struct _tuple0 _tmp4EF=(_tmp8A5.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4AC->topt))->v),((_tmp8A5.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4AD->topt))->v),_tmp8A5)));void*_tmp4F0;enum Cyc_Absyn_Sign
_tmp4F2;void*_tmp4F3;enum Cyc_Absyn_Sign _tmp4F5;void*_tmp4F6;void*_tmp4F8;_LL27B:
_tmp4F0=_tmp4EF.f1;{struct Cyc_Absyn_IntType_struct*_tmp4F1=(struct Cyc_Absyn_IntType_struct*)
_tmp4F0;if(_tmp4F1->tag != 6)goto _LL27D;else{_tmp4F2=_tmp4F1->f1;if(_tmp4F2 != Cyc_Absyn_Unsigned)
goto _LL27D;}};_LL27C: goto _LL27E;_LL27D: _tmp4F3=_tmp4EF.f2;{struct Cyc_Absyn_IntType_struct*
_tmp4F4=(struct Cyc_Absyn_IntType_struct*)_tmp4F3;if(_tmp4F4->tag != 6)goto _LL27F;
else{_tmp4F5=_tmp4F4->f1;if(_tmp4F5 != Cyc_Absyn_Unsigned)goto _LL27F;}};_LL27E:
goto _LL280;_LL27F: _tmp4F6=_tmp4EF.f1;{struct Cyc_Absyn_TagType_struct*_tmp4F7=(
struct Cyc_Absyn_TagType_struct*)_tmp4F6;if(_tmp4F7->tag != 20)goto _LL281;};_LL280:
goto _LL282;_LL281: _tmp4F8=_tmp4EF.f2;{struct Cyc_Absyn_TagType_struct*_tmp4F9=(
struct Cyc_Absyn_TagType_struct*)_tmp4F8;if(_tmp4F9->tag != 20)goto _LL283;};_LL282:
goto _LL27A;_LL283:;_LL284: {struct _tuple16 _tmp8A6;return(_tmp8A6.f1=f,((_tmp8A6.f2=
f,_tmp8A6)));}_LL27A:;}switch(_tmp480){case Cyc_Absyn_Eq: _LL285: {union Cyc_CfFlowInfo_FlowInfo
_tmp4FB=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AC,r1,Cyc_Absyn_Eq,r2);_tmp4FB=
Cyc_NewControlFlow_if_tagcmp(env,_tmp4FB,_tmp4AD,r2,Cyc_Absyn_Eq,r1);{struct
_tuple16 _tmp8A7;return(_tmp8A7.f1=_tmp4FB,((_tmp8A7.f2=f,_tmp8A7)));};}case Cyc_Absyn_Neq:
_LL286: {union Cyc_CfFlowInfo_FlowInfo _tmp4FD=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp4AC,r1,Cyc_Absyn_Eq,r2);_tmp4FD=Cyc_NewControlFlow_if_tagcmp(env,_tmp4FD,
_tmp4AD,r2,Cyc_Absyn_Eq,r1);{struct _tuple16 _tmp8A8;return(_tmp8A8.f1=f,((_tmp8A8.f2=
_tmp4FD,_tmp8A8)));};}case Cyc_Absyn_Gt: _LL287: {union Cyc_CfFlowInfo_FlowInfo
_tmp4FF=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AD,r2,Cyc_Absyn_Lt,r1);union Cyc_CfFlowInfo_FlowInfo
_tmp500=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AC,r1,Cyc_Absyn_Lte,r2);struct
_tuple16 _tmp8A9;return(_tmp8A9.f1=_tmp4FF,((_tmp8A9.f2=_tmp500,_tmp8A9)));}case
Cyc_Absyn_Gte: _LL288: {union Cyc_CfFlowInfo_FlowInfo _tmp502=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp4AD,r2,Cyc_Absyn_Lte,r1);union Cyc_CfFlowInfo_FlowInfo _tmp503=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp4AC,r1,Cyc_Absyn_Lt,r2);struct _tuple16 _tmp8AA;return(_tmp8AA.f1=
_tmp502,((_tmp8AA.f2=_tmp503,_tmp8AA)));}case Cyc_Absyn_Lt: _LL289: {union Cyc_CfFlowInfo_FlowInfo
_tmp505=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AC,r1,Cyc_Absyn_Lt,r2);union Cyc_CfFlowInfo_FlowInfo
_tmp506=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp4AD,r2,Cyc_Absyn_Lte,r1);{union
Cyc_CfFlowInfo_FlowInfo _tmp507=_tmp505;int _tmp508;struct _tuple12 _tmp509;struct
Cyc_Dict_Dict _tmp50A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp50B;_LL28C: if((_tmp507.BottomFL).tag
!= 1)goto _LL28E;_tmp508=(int)(_tmp507.BottomFL).val;_LL28D: {struct Cyc_Core_Impossible_struct
_tmp8B0;const char*_tmp8AF;struct Cyc_Core_Impossible_struct*_tmp8AE;(int)_throw((
void*)((_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE[0]=((_tmp8B0.tag=Cyc_Core_Impossible,((
_tmp8B0.f1=((_tmp8AF="anal_test, Lt",_tag_dyneither(_tmp8AF,sizeof(char),14))),
_tmp8B0)))),_tmp8AE)))));}_LL28E: if((_tmp507.ReachableFL).tag != 2)goto _LL28B;
_tmp509=(struct _tuple12)(_tmp507.ReachableFL).val;_tmp50A=_tmp509.f1;_tmp50B=
_tmp509.f3;_LL28F: _tmp4A8=_tmp50A;_tmp4AA=_tmp50B;_LL28B:;}{void*_tmp50F=_tmp4AC->r;
void*_tmp511;struct Cyc_Absyn_Vardecl*_tmp513;void*_tmp515;struct Cyc_Absyn_Vardecl*
_tmp517;void*_tmp519;struct Cyc_Absyn_Vardecl*_tmp51B;void*_tmp51D;struct Cyc_Absyn_Vardecl*
_tmp51F;_LL291:{struct Cyc_Absyn_Var_e_struct*_tmp510=(struct Cyc_Absyn_Var_e_struct*)
_tmp50F;if(_tmp510->tag != 1)goto _LL293;else{_tmp511=(void*)_tmp510->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp512=(struct Cyc_Absyn_Global_b_struct*)_tmp511;if(
_tmp512->tag != 1)goto _LL293;else{_tmp513=_tmp512->f1;}};}}if(!(!_tmp513->escapes))
goto _LL293;_LL292: _tmp517=_tmp513;goto _LL294;_LL293:{struct Cyc_Absyn_Var_e_struct*
_tmp514=(struct Cyc_Absyn_Var_e_struct*)_tmp50F;if(_tmp514->tag != 1)goto _LL295;
else{_tmp515=(void*)_tmp514->f2;{struct Cyc_Absyn_Local_b_struct*_tmp516=(struct
Cyc_Absyn_Local_b_struct*)_tmp515;if(_tmp516->tag != 4)goto _LL295;else{_tmp517=
_tmp516->f1;}};}}if(!(!_tmp517->escapes))goto _LL295;_LL294: _tmp51B=_tmp517;goto
_LL296;_LL295:{struct Cyc_Absyn_Var_e_struct*_tmp518=(struct Cyc_Absyn_Var_e_struct*)
_tmp50F;if(_tmp518->tag != 1)goto _LL297;else{_tmp519=(void*)_tmp518->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp51A=(struct Cyc_Absyn_Pat_b_struct*)_tmp519;if(_tmp51A->tag
!= 5)goto _LL297;else{_tmp51B=_tmp51A->f1;}};}}if(!(!_tmp51B->escapes))goto _LL297;
_LL296: _tmp51F=_tmp51B;goto _LL298;_LL297:{struct Cyc_Absyn_Var_e_struct*_tmp51C=(
struct Cyc_Absyn_Var_e_struct*)_tmp50F;if(_tmp51C->tag != 1)goto _LL299;else{
_tmp51D=(void*)_tmp51C->f2;{struct Cyc_Absyn_Param_b_struct*_tmp51E=(struct Cyc_Absyn_Param_b_struct*)
_tmp51D;if(_tmp51E->tag != 3)goto _LL299;else{_tmp51F=_tmp51E->f1;}};}}if(!(!
_tmp51F->escapes))goto _LL299;_LL298: {void*_tmp520=_tmp4AD->r;void*_tmp522;
struct Cyc_Absyn_Vardecl*_tmp524;void*_tmp526;struct Cyc_Absyn_Vardecl*_tmp528;
void*_tmp52A;struct Cyc_Absyn_Vardecl*_tmp52C;void*_tmp52E;struct Cyc_Absyn_Vardecl*
_tmp530;union Cyc_Absyn_Cnst _tmp532;struct _tuple6 _tmp533;int _tmp534;enum Cyc_Absyn_Primop
_tmp536;struct Cyc_List_List*_tmp537;struct Cyc_List_List _tmp538;struct Cyc_Absyn_Exp*
_tmp539;_LL29C:{struct Cyc_Absyn_Var_e_struct*_tmp521=(struct Cyc_Absyn_Var_e_struct*)
_tmp520;if(_tmp521->tag != 1)goto _LL29E;else{_tmp522=(void*)_tmp521->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp523=(struct Cyc_Absyn_Global_b_struct*)_tmp522;if(
_tmp523->tag != 1)goto _LL29E;else{_tmp524=_tmp523->f1;}};}}if(!(!_tmp524->escapes))
goto _LL29E;_LL29D: _tmp528=_tmp524;goto _LL29F;_LL29E:{struct Cyc_Absyn_Var_e_struct*
_tmp525=(struct Cyc_Absyn_Var_e_struct*)_tmp520;if(_tmp525->tag != 1)goto _LL2A0;
else{_tmp526=(void*)_tmp525->f2;{struct Cyc_Absyn_Local_b_struct*_tmp527=(struct
Cyc_Absyn_Local_b_struct*)_tmp526;if(_tmp527->tag != 4)goto _LL2A0;else{_tmp528=
_tmp527->f1;}};}}if(!(!_tmp528->escapes))goto _LL2A0;_LL29F: _tmp52C=_tmp528;goto
_LL2A1;_LL2A0:{struct Cyc_Absyn_Var_e_struct*_tmp529=(struct Cyc_Absyn_Var_e_struct*)
_tmp520;if(_tmp529->tag != 1)goto _LL2A2;else{_tmp52A=(void*)_tmp529->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp52B=(struct Cyc_Absyn_Pat_b_struct*)_tmp52A;if(_tmp52B->tag
!= 5)goto _LL2A2;else{_tmp52C=_tmp52B->f1;}};}}if(!(!_tmp52C->escapes))goto _LL2A2;
_LL2A1: _tmp530=_tmp52C;goto _LL2A3;_LL2A2:{struct Cyc_Absyn_Var_e_struct*_tmp52D=(
struct Cyc_Absyn_Var_e_struct*)_tmp520;if(_tmp52D->tag != 1)goto _LL2A4;else{
_tmp52E=(void*)_tmp52D->f2;{struct Cyc_Absyn_Param_b_struct*_tmp52F=(struct Cyc_Absyn_Param_b_struct*)
_tmp52E;if(_tmp52F->tag != 3)goto _LL2A4;else{_tmp530=_tmp52F->f1;}};}}if(!(!
_tmp530->escapes))goto _LL2A4;_LL2A3: {struct _RegionHandle*_tmp53A=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp8B3;struct Cyc_List_List*_tmp8B2;struct Cyc_List_List*
_tmp53B=(_tmp8B2=_region_malloc(_tmp53A,sizeof(*_tmp8B2)),((_tmp8B2->hd=((
_tmp8B3=_region_malloc(_tmp53A,sizeof(*_tmp8B3)),((_tmp8B3->vd=_tmp51F,((_tmp8B3->rop=
Cyc_CfFlowInfo_LessVar(_tmp530,_tmp530->type),_tmp8B3)))))),((_tmp8B2->tl=
_tmp4A9,_tmp8B2)))));struct _tuple16 _tmp8B4;return(_tmp8B4.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4A8,_tmp53B,_tmp4AA),((_tmp8B4.f2=_tmp506,_tmp8B4)));}_LL2A4: {struct Cyc_Absyn_Const_e_struct*
_tmp531=(struct Cyc_Absyn_Const_e_struct*)_tmp520;if(_tmp531->tag != 0)goto _LL2A6;
else{_tmp532=_tmp531->f1;if((_tmp532.Int_c).tag != 4)goto _LL2A6;_tmp533=(struct
_tuple6)(_tmp532.Int_c).val;_tmp534=_tmp533.f2;}}_LL2A5: {struct _RegionHandle*
_tmp53F=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8B7;struct Cyc_List_List*
_tmp8B6;struct Cyc_List_List*_tmp540=(_tmp8B6=_region_malloc(_tmp53F,sizeof(*
_tmp8B6)),((_tmp8B6->hd=((_tmp8B7=_region_malloc(_tmp53F,sizeof(*_tmp8B7)),((
_tmp8B7->vd=_tmp51F,((_tmp8B7->rop=Cyc_CfFlowInfo_LessConst((unsigned int)
_tmp534),_tmp8B7)))))),((_tmp8B6->tl=_tmp4A9,_tmp8B6)))));struct _tuple16 _tmp8B8;
return(_tmp8B8.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4A8,_tmp540,_tmp4AA),((_tmp8B8.f2=
_tmp506,_tmp8B8)));}_LL2A6: {struct Cyc_Absyn_Primop_e_struct*_tmp535=(struct Cyc_Absyn_Primop_e_struct*)
_tmp520;if(_tmp535->tag != 3)goto _LL2A8;else{_tmp536=_tmp535->f1;_tmp537=_tmp535->f2;
if(_tmp537 == 0)goto _LL2A8;_tmp538=*_tmp537;_tmp539=(struct Cyc_Absyn_Exp*)_tmp538.hd;}}
_LL2A7: {void*_tmp544=_tmp539->r;void*_tmp546;struct Cyc_Absyn_Vardecl*_tmp548;
void*_tmp54A;struct Cyc_Absyn_Vardecl*_tmp54C;void*_tmp54E;struct Cyc_Absyn_Vardecl*
_tmp550;void*_tmp552;struct Cyc_Absyn_Vardecl*_tmp554;_LL2AB:{struct Cyc_Absyn_Var_e_struct*
_tmp545=(struct Cyc_Absyn_Var_e_struct*)_tmp544;if(_tmp545->tag != 1)goto _LL2AD;
else{_tmp546=(void*)_tmp545->f2;{struct Cyc_Absyn_Global_b_struct*_tmp547=(struct
Cyc_Absyn_Global_b_struct*)_tmp546;if(_tmp547->tag != 1)goto _LL2AD;else{_tmp548=
_tmp547->f1;}};}}if(!(!_tmp548->escapes))goto _LL2AD;_LL2AC: _tmp54C=_tmp548;goto
_LL2AE;_LL2AD:{struct Cyc_Absyn_Var_e_struct*_tmp549=(struct Cyc_Absyn_Var_e_struct*)
_tmp544;if(_tmp549->tag != 1)goto _LL2AF;else{_tmp54A=(void*)_tmp549->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp54B=(struct Cyc_Absyn_Local_b_struct*)_tmp54A;if(
_tmp54B->tag != 4)goto _LL2AF;else{_tmp54C=_tmp54B->f1;}};}}if(!(!_tmp54C->escapes))
goto _LL2AF;_LL2AE: _tmp550=_tmp54C;goto _LL2B0;_LL2AF:{struct Cyc_Absyn_Var_e_struct*
_tmp54D=(struct Cyc_Absyn_Var_e_struct*)_tmp544;if(_tmp54D->tag != 1)goto _LL2B1;
else{_tmp54E=(void*)_tmp54D->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp54F=(struct Cyc_Absyn_Pat_b_struct*)
_tmp54E;if(_tmp54F->tag != 5)goto _LL2B1;else{_tmp550=_tmp54F->f1;}};}}if(!(!
_tmp550->escapes))goto _LL2B1;_LL2B0: _tmp554=_tmp550;goto _LL2B2;_LL2B1:{struct Cyc_Absyn_Var_e_struct*
_tmp551=(struct Cyc_Absyn_Var_e_struct*)_tmp544;if(_tmp551->tag != 1)goto _LL2B3;
else{_tmp552=(void*)_tmp551->f2;{struct Cyc_Absyn_Param_b_struct*_tmp553=(struct
Cyc_Absyn_Param_b_struct*)_tmp552;if(_tmp553->tag != 3)goto _LL2B3;else{_tmp554=
_tmp553->f1;}};}}if(!(!_tmp554->escapes))goto _LL2B3;_LL2B2: {struct _RegionHandle*
_tmp555=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp8BB;struct Cyc_List_List*
_tmp8BA;struct Cyc_List_List*_tmp556=(_tmp8BA=_region_malloc(_tmp555,sizeof(*
_tmp8BA)),((_tmp8BA->hd=((_tmp8BB=_region_malloc(_tmp555,sizeof(*_tmp8BB)),((
_tmp8BB->vd=_tmp51F,((_tmp8BB->rop=Cyc_CfFlowInfo_LessNumelts(_tmp554),_tmp8BB)))))),((
_tmp8BA->tl=_tmp4A9,_tmp8BA)))));struct _tuple16 _tmp8BC;return(_tmp8BC.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4A8,_tmp556,_tmp4AA),((_tmp8BC.f2=_tmp506,_tmp8BC)));}_LL2B3:;_LL2B4: {
struct _tuple16 _tmp8BD;return(_tmp8BD.f1=_tmp505,((_tmp8BD.f2=_tmp506,_tmp8BD)));}
_LL2AA:;}_LL2A8:;_LL2A9: {struct _tuple16 _tmp8BE;return(_tmp8BE.f1=_tmp505,((
_tmp8BE.f2=_tmp506,_tmp8BE)));}_LL29B:;}_LL299:;_LL29A: {struct _tuple16 _tmp8BF;
return(_tmp8BF.f1=_tmp505,((_tmp8BF.f2=_tmp506,_tmp8BF)));}_LL290:;};}case Cyc_Absyn_Lte:
_LL28A: {union Cyc_CfFlowInfo_FlowInfo _tmp55D=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp4AC,r1,Cyc_Absyn_Lte,r2);union Cyc_CfFlowInfo_FlowInfo _tmp55E=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp4AD,r1,Cyc_Absyn_Lt,r1);{union Cyc_CfFlowInfo_FlowInfo _tmp55F=_tmp55D;
int _tmp560;struct _tuple12 _tmp561;struct Cyc_Dict_Dict _tmp562;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp563;_LL2B7: if((_tmp55F.BottomFL).tag != 1)goto _LL2B9;_tmp560=(int)(_tmp55F.BottomFL).val;
_LL2B8: {struct Cyc_Core_Impossible_struct _tmp8C5;const char*_tmp8C4;struct Cyc_Core_Impossible_struct*
_tmp8C3;(int)_throw((void*)((_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3[0]=((
_tmp8C5.tag=Cyc_Core_Impossible,((_tmp8C5.f1=((_tmp8C4="anal_test, Lte",
_tag_dyneither(_tmp8C4,sizeof(char),15))),_tmp8C5)))),_tmp8C3)))));}_LL2B9: if((
_tmp55F.ReachableFL).tag != 2)goto _LL2B6;_tmp561=(struct _tuple12)(_tmp55F.ReachableFL).val;
_tmp562=_tmp561.f1;_tmp563=_tmp561.f3;_LL2BA: _tmp4A8=_tmp562;_tmp4AA=_tmp563;
_LL2B6:;}{void*_tmp567=_tmp4AC->r;void*_tmp569;struct Cyc_Absyn_Vardecl*_tmp56B;
void*_tmp56D;struct Cyc_Absyn_Vardecl*_tmp56F;void*_tmp571;struct Cyc_Absyn_Vardecl*
_tmp573;void*_tmp575;struct Cyc_Absyn_Vardecl*_tmp577;_LL2BC:{struct Cyc_Absyn_Var_e_struct*
_tmp568=(struct Cyc_Absyn_Var_e_struct*)_tmp567;if(_tmp568->tag != 1)goto _LL2BE;
else{_tmp569=(void*)_tmp568->f2;{struct Cyc_Absyn_Global_b_struct*_tmp56A=(struct
Cyc_Absyn_Global_b_struct*)_tmp569;if(_tmp56A->tag != 1)goto _LL2BE;else{_tmp56B=
_tmp56A->f1;}};}}if(!(!_tmp56B->escapes))goto _LL2BE;_LL2BD: _tmp56F=_tmp56B;goto
_LL2BF;_LL2BE:{struct Cyc_Absyn_Var_e_struct*_tmp56C=(struct Cyc_Absyn_Var_e_struct*)
_tmp567;if(_tmp56C->tag != 1)goto _LL2C0;else{_tmp56D=(void*)_tmp56C->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp56E=(struct Cyc_Absyn_Local_b_struct*)_tmp56D;if(
_tmp56E->tag != 4)goto _LL2C0;else{_tmp56F=_tmp56E->f1;}};}}if(!(!_tmp56F->escapes))
goto _LL2C0;_LL2BF: _tmp573=_tmp56F;goto _LL2C1;_LL2C0:{struct Cyc_Absyn_Var_e_struct*
_tmp570=(struct Cyc_Absyn_Var_e_struct*)_tmp567;if(_tmp570->tag != 1)goto _LL2C2;
else{_tmp571=(void*)_tmp570->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp572=(struct Cyc_Absyn_Pat_b_struct*)
_tmp571;if(_tmp572->tag != 5)goto _LL2C2;else{_tmp573=_tmp572->f1;}};}}if(!(!
_tmp573->escapes))goto _LL2C2;_LL2C1: _tmp577=_tmp573;goto _LL2C3;_LL2C2:{struct Cyc_Absyn_Var_e_struct*
_tmp574=(struct Cyc_Absyn_Var_e_struct*)_tmp567;if(_tmp574->tag != 1)goto _LL2C4;
else{_tmp575=(void*)_tmp574->f2;{struct Cyc_Absyn_Param_b_struct*_tmp576=(struct
Cyc_Absyn_Param_b_struct*)_tmp575;if(_tmp576->tag != 3)goto _LL2C4;else{_tmp577=
_tmp576->f1;}};}}if(!(!_tmp577->escapes))goto _LL2C4;_LL2C3: {void*_tmp578=
_tmp4AD->r;enum Cyc_Absyn_Primop _tmp57A;struct Cyc_List_List*_tmp57B;struct Cyc_List_List
_tmp57C;struct Cyc_Absyn_Exp*_tmp57D;_LL2C7: {struct Cyc_Absyn_Primop_e_struct*
_tmp579=(struct Cyc_Absyn_Primop_e_struct*)_tmp578;if(_tmp579->tag != 3)goto _LL2C9;
else{_tmp57A=_tmp579->f1;_tmp57B=_tmp579->f2;if(_tmp57B == 0)goto _LL2C9;_tmp57C=*
_tmp57B;_tmp57D=(struct Cyc_Absyn_Exp*)_tmp57C.hd;}}_LL2C8: {void*_tmp57E=_tmp57D->r;
void*_tmp580;struct Cyc_Absyn_Vardecl*_tmp582;void*_tmp584;struct Cyc_Absyn_Vardecl*
_tmp586;void*_tmp588;struct Cyc_Absyn_Vardecl*_tmp58A;void*_tmp58C;struct Cyc_Absyn_Vardecl*
_tmp58E;_LL2CC:{struct Cyc_Absyn_Var_e_struct*_tmp57F=(struct Cyc_Absyn_Var_e_struct*)
_tmp57E;if(_tmp57F->tag != 1)goto _LL2CE;else{_tmp580=(void*)_tmp57F->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp581=(struct Cyc_Absyn_Global_b_struct*)_tmp580;if(
_tmp581->tag != 1)goto _LL2CE;else{_tmp582=_tmp581->f1;}};}}if(!(!_tmp582->escapes))
goto _LL2CE;_LL2CD: _tmp586=_tmp582;goto _LL2CF;_LL2CE:{struct Cyc_Absyn_Var_e_struct*
_tmp583=(struct Cyc_Absyn_Var_e_struct*)_tmp57E;if(_tmp583->tag != 1)goto _LL2D0;
else{_tmp584=(void*)_tmp583->f2;{struct Cyc_Absyn_Local_b_struct*_tmp585=(struct
Cyc_Absyn_Local_b_struct*)_tmp584;if(_tmp585->tag != 4)goto _LL2D0;else{_tmp586=
_tmp585->f1;}};}}if(!(!_tmp586->escapes))goto _LL2D0;_LL2CF: _tmp58A=_tmp586;goto
_LL2D1;_LL2D0:{struct Cyc_Absyn_Var_e_struct*_tmp587=(struct Cyc_Absyn_Var_e_struct*)
_tmp57E;if(_tmp587->tag != 1)goto _LL2D2;else{_tmp588=(void*)_tmp587->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp589=(struct Cyc_Absyn_Pat_b_struct*)_tmp588;if(_tmp589->tag
!= 5)goto _LL2D2;else{_tmp58A=_tmp589->f1;}};}}if(!(!_tmp58A->escapes))goto _LL2D2;
_LL2D1: _tmp58E=_tmp58A;goto _LL2D3;_LL2D2:{struct Cyc_Absyn_Var_e_struct*_tmp58B=(
struct Cyc_Absyn_Var_e_struct*)_tmp57E;if(_tmp58B->tag != 1)goto _LL2D4;else{
_tmp58C=(void*)_tmp58B->f2;{struct Cyc_Absyn_Param_b_struct*_tmp58D=(struct Cyc_Absyn_Param_b_struct*)
_tmp58C;if(_tmp58D->tag != 3)goto _LL2D4;else{_tmp58E=_tmp58D->f1;}};}}if(!(!
_tmp58E->escapes))goto _LL2D4;_LL2D3: {struct Cyc_CfFlowInfo_FlowEnv*_tmp58F=env->fenv;
struct Cyc_CfFlowInfo_Reln*_tmp8C8;struct Cyc_List_List*_tmp8C7;struct Cyc_List_List*
_tmp590=(_tmp8C7=_region_malloc(_tmp58F->r,sizeof(*_tmp8C7)),((_tmp8C7->hd=((
_tmp8C8=_region_malloc(_tmp58F->r,sizeof(*_tmp8C8)),((_tmp8C8->vd=_tmp577,((
_tmp8C8->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp58E),_tmp8C8)))))),((_tmp8C7->tl=
_tmp4A9,_tmp8C7)))));struct _tuple16 _tmp8C9;return(_tmp8C9.f1=Cyc_CfFlowInfo_ReachableFL(
_tmp4A8,_tmp590,_tmp4AA),((_tmp8C9.f2=_tmp55E,_tmp8C9)));}_LL2D4:;_LL2D5: {
struct _tuple16 _tmp8CA;return(_tmp8CA.f1=_tmp55D,((_tmp8CA.f2=_tmp55E,_tmp8CA)));}
_LL2CB:;}_LL2C9:;_LL2CA: {struct _tuple16 _tmp8CB;return(_tmp8CB.f1=_tmp55D,((
_tmp8CB.f2=_tmp55E,_tmp8CB)));}_LL2C6:;}_LL2C4:;_LL2C5: {struct _tuple16 _tmp8CC;
return(_tmp8CC.f1=_tmp55D,((_tmp8CC.f2=_tmp55E,_tmp8CC)));}_LL2BB:;};}default:
_LL2B5: {struct _tuple16 _tmp8CD;return(_tmp8CD.f1=f,((_tmp8CD.f2=f,_tmp8CD)));}}}
_LL258:;};}_LL256:;_LL257: goto _LL249;_LL249:;}{union Cyc_CfFlowInfo_FlowInfo
_tmp599;void*_tmp59A;struct _tuple13 _tmp598=Cyc_NewControlFlow_anal_Rexp(env,
inflow,e);_tmp599=_tmp598.f1;_tmp59A=_tmp598.f2;_tmp599=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp599);{union Cyc_CfFlowInfo_FlowInfo _tmp59B=_tmp599;int _tmp59C;struct
_tuple12 _tmp59D;struct Cyc_Dict_Dict _tmp59E;_LL2D8: if((_tmp59B.BottomFL).tag != 1)
goto _LL2DA;_tmp59C=(int)(_tmp59B.BottomFL).val;_LL2D9: {struct _tuple16 _tmp8CE;
return(_tmp8CE.f1=_tmp599,((_tmp8CE.f2=_tmp599,_tmp8CE)));}_LL2DA: if((_tmp59B.ReachableFL).tag
!= 2)goto _LL2D7;_tmp59D=(struct _tuple12)(_tmp59B.ReachableFL).val;_tmp59E=
_tmp59D.f1;_LL2DB: {void*_tmp5A0=_tmp59A;enum Cyc_CfFlowInfo_InitLevel _tmp5A6;
enum Cyc_CfFlowInfo_InitLevel _tmp5A8;enum Cyc_CfFlowInfo_InitLevel _tmp5AA;_LL2DD: {
struct Cyc_CfFlowInfo_Zero_struct*_tmp5A1=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp5A0;if(_tmp5A1->tag != 0)goto _LL2DF;}_LL2DE: {struct _tuple16 _tmp8CF;return(
_tmp8CF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8CF.f2=_tmp599,_tmp8CF)));}_LL2DF: {
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp5A2=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp5A0;if(_tmp5A2->tag != 2)goto _LL2E1;}_LL2E0: goto _LL2E2;_LL2E1: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp5A3=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp5A0;if(_tmp5A3->tag != 1)
goto _LL2E3;}_LL2E2: goto _LL2E4;_LL2E3: {struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5A4=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp5A0;if(_tmp5A4->tag != 5)goto
_LL2E5;}_LL2E4: {struct _tuple16 _tmp8D0;return(_tmp8D0.f1=_tmp599,((_tmp8D0.f2=
Cyc_CfFlowInfo_BottomFL(),_tmp8D0)));}_LL2E5: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp5A5=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp5A0;if(_tmp5A5->tag != 3)goto
_LL2E7;else{_tmp5A6=_tmp5A5->f1;if(_tmp5A6 != Cyc_CfFlowInfo_NoneIL)goto _LL2E7;}}
_LL2E6: goto _LL2E8;_LL2E7: {struct Cyc_CfFlowInfo_Esc_struct*_tmp5A7=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp5A0;if(_tmp5A7->tag != 4)goto _LL2E9;else{_tmp5A8=_tmp5A7->f1;if(_tmp5A8 != Cyc_CfFlowInfo_NoneIL)
goto _LL2E9;}}_LL2E8:{const char*_tmp8D3;void*_tmp8D2;(_tmp8D2=0,Cyc_Tcutil_terr(e->loc,((
_tmp8D3="expression may not be initialized",_tag_dyneither(_tmp8D3,sizeof(char),
34))),_tag_dyneither(_tmp8D2,sizeof(void*),0)));}{struct _tuple16 _tmp8D4;return(
_tmp8D4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8D4.f2=Cyc_CfFlowInfo_BottomFL(),
_tmp8D4)));};_LL2E9: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp5A9=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp5A0;if(_tmp5A9->tag != 3)goto _LL2EB;else{_tmp5AA=_tmp5A9->f1;}}_LL2EA: return
Cyc_NewControlFlow_splitzero(env,inflow,_tmp599,e,_tmp5AA);_LL2EB: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp5AB=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp5A0;if(_tmp5AB->tag != 4)goto _LL2ED;}
_LL2EC: {struct _tuple16 _tmp8D5;return(_tmp8D5.f1=_tmp599,((_tmp8D5.f2=_tmp599,
_tmp8D5)));}_LL2ED: {struct Cyc_CfFlowInfo_TagCmps_struct*_tmp5AC=(struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp5A0;if(_tmp5AC->tag != 6)goto _LL2EF;}_LL2EE: {struct _tuple16 _tmp8D6;return(
_tmp8D6.f1=_tmp599,((_tmp8D6.f2=_tmp599,_tmp8D6)));}_LL2EF: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp5AD=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp5A0;if(_tmp5AD->tag != 7)goto
_LL2DC;}_LL2F0: {struct Cyc_Core_Impossible_struct _tmp8DC;const char*_tmp8DB;
struct Cyc_Core_Impossible_struct*_tmp8DA;(int)_throw((void*)((_tmp8DA=_cycalloc(
sizeof(*_tmp8DA)),((_tmp8DA[0]=((_tmp8DC.tag=Cyc_Core_Impossible,((_tmp8DC.f1=((
_tmp8DB="anal_test",_tag_dyneither(_tmp8DB,sizeof(char),10))),_tmp8DC)))),
_tmp8DA)))));}_LL2DC:;}_LL2D7:;};};}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
flow);static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*
loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){union
Cyc_CfFlowInfo_FlowInfo _tmp5B8=flow;int _tmp5B9;struct _tuple12 _tmp5BA;struct Cyc_Dict_Dict
_tmp5BB;_LL2F2: if((_tmp5B8.BottomFL).tag != 1)goto _LL2F4;_tmp5B9=(int)(_tmp5B8.BottomFL).val;
_LL2F3: return;_LL2F4: if((_tmp5B8.ReachableFL).tag != 2)goto _LL2F1;_tmp5BA=(struct
_tuple12)(_tmp5B8.ReachableFL).val;_tmp5BB=_tmp5BA.f1;_LL2F5:{struct Cyc_List_List*
_tmp5BC=env->param_roots;for(0;_tmp5BC != 0;_tmp5BC=_tmp5BC->tl){if(Cyc_CfFlowInfo_initlevel(
env->fenv,_tmp5BB,Cyc_CfFlowInfo_lookup_place(_tmp5BB,(struct Cyc_CfFlowInfo_Place*)
_tmp5BC->hd))!= Cyc_CfFlowInfo_AllIL){const char*_tmp8DF;void*_tmp8DE;(_tmp8DE=0,
Cyc_Tcutil_terr(loc,((_tmp8DF="function may not initialize all the parameters with attribute 'initializes'",
_tag_dyneither(_tmp8DF,sizeof(char),76))),_tag_dyneither(_tmp8DE,sizeof(void*),0)));}}}
return;_LL2F1:;}static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct
Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp5BF=env->fenv;for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp5C1;struct Cyc_Core_Opt*_tmp5C2;
struct Cyc_Absyn_Exp*_tmp5C3;struct Cyc_Absyn_Stmt*_tmp5C4;struct Cyc_Position_Segment*
_tmp5C5;struct Cyc_Absyn_Switch_clause*_tmp5C0=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp5C1=*_tmp5C0;_tmp5C2=_tmp5C1.pat_vars;_tmp5C3=_tmp5C1.where_clause;
_tmp5C4=_tmp5C1.body;_tmp5C5=_tmp5C1.loc;{union Cyc_CfFlowInfo_FlowInfo
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp5BF,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5C2))->v,_tmp5BF->unknown_all,_tmp5C5);union
Cyc_CfFlowInfo_FlowInfo clause_outflow;if(_tmp5C3 != 0){struct Cyc_Absyn_Exp*wexp=(
struct Cyc_Absyn_Exp*)_tmp5C3;union Cyc_CfFlowInfo_FlowInfo _tmp5C7;union Cyc_CfFlowInfo_FlowInfo
_tmp5C8;struct _tuple16 _tmp5C6=Cyc_NewControlFlow_anal_test(env,clause_inflow,
wexp);_tmp5C7=_tmp5C6.f1;_tmp5C8=_tmp5C6.f2;_tmp5C7=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp5C7);_tmp5C8=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp5C8);inflow=_tmp5C8;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5C7,
_tmp5C4);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp5C4);}{union Cyc_CfFlowInfo_FlowInfo _tmp5C9=clause_outflow;int _tmp5CA;_LL2F7:
if((_tmp5C9.BottomFL).tag != 1)goto _LL2F9;_tmp5CA=(int)(_tmp5C9.BottomFL).val;
_LL2F8: goto _LL2F6;_LL2F9:;_LL2FA: if(scs->tl == 0)return clause_outflow;else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){const char*
_tmp8E2;void*_tmp8E1;(_tmp8E1=0,Cyc_Tcutil_terr(_tmp5C4->loc,((_tmp8E2="switch clause may implicitly fallthru",
_tag_dyneither(_tmp8E2,sizeof(char),38))),_tag_dyneither(_tmp8E1,sizeof(void*),0)));}
else{const char*_tmp8E5;void*_tmp8E4;(_tmp8E4=0,Cyc_Tcutil_warn(_tmp5C4->loc,((
_tmp8E5="switch clause may implicitly fallthru",_tag_dyneither(_tmp8E5,sizeof(
char),38))),_tag_dyneither(_tmp8E4,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL2F6;_LL2F6:;};};}return Cyc_CfFlowInfo_BottomFL();}static
union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5D0;union Cyc_CfFlowInfo_FlowInfo*_tmp5D1;struct _tuple17 _tmp5CF=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp5D0=_tmp5CF.f1;_tmp5D1=_tmp5CF.f2;inflow=*_tmp5D1;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp5D2=env->fenv;{void*_tmp5D3=s->r;struct Cyc_Absyn_Exp*_tmp5D6;struct Cyc_Absyn_Exp*
_tmp5D8;struct Cyc_Absyn_Exp*_tmp5DA;struct Cyc_Absyn_Stmt*_tmp5DC;struct Cyc_Absyn_Stmt*
_tmp5DD;struct Cyc_Absyn_Exp*_tmp5DF;struct Cyc_Absyn_Stmt*_tmp5E0;struct Cyc_Absyn_Stmt*
_tmp5E1;struct _tuple9 _tmp5E3;struct Cyc_Absyn_Exp*_tmp5E4;struct Cyc_Absyn_Stmt*
_tmp5E5;struct Cyc_Absyn_Stmt*_tmp5E6;struct Cyc_Absyn_Stmt*_tmp5E8;struct _tuple9
_tmp5E9;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Stmt*_tmp5EB;struct Cyc_Absyn_Exp*
_tmp5ED;struct _tuple9 _tmp5EE;struct Cyc_Absyn_Exp*_tmp5EF;struct Cyc_Absyn_Stmt*
_tmp5F0;struct _tuple9 _tmp5F1;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Stmt*
_tmp5F3;struct Cyc_Absyn_Stmt*_tmp5F4;struct Cyc_Absyn_Stmt*_tmp5F6;struct Cyc_List_List*
_tmp5F8;struct Cyc_Absyn_Switch_clause**_tmp5F9;struct Cyc_Absyn_Switch_clause*
_tmp5FA;struct Cyc_Absyn_Stmt*_tmp5FC;struct Cyc_Absyn_Stmt*_tmp5FE;struct Cyc_Absyn_Stmt*
_tmp600;struct Cyc_Absyn_Exp*_tmp602;struct Cyc_List_List*_tmp603;struct Cyc_Absyn_Stmt*
_tmp605;struct Cyc_List_List*_tmp606;struct Cyc_Absyn_Decl*_tmp608;struct Cyc_Absyn_Decl
_tmp609;void*_tmp60A;struct Cyc_Absyn_Exp*_tmp60C;struct Cyc_Absyn_Tvar*_tmp60D;
struct Cyc_Absyn_Vardecl*_tmp60E;struct Cyc_Absyn_Stmt*_tmp60F;struct Cyc_Absyn_Decl*
_tmp611;struct Cyc_Absyn_Stmt*_tmp612;struct Cyc_Absyn_Stmt*_tmp614;struct Cyc_Absyn_Exp*
_tmp616;_LL2FC: {struct Cyc_Absyn_Skip_s_struct*_tmp5D4=(struct Cyc_Absyn_Skip_s_struct*)
_tmp5D3;if(_tmp5D4->tag != 0)goto _LL2FE;}_LL2FD: return inflow;_LL2FE: {struct Cyc_Absyn_Return_s_struct*
_tmp5D5=(struct Cyc_Absyn_Return_s_struct*)_tmp5D3;if(_tmp5D5->tag != 3)goto _LL300;
else{_tmp5D6=_tmp5D5->f1;if(_tmp5D6 != 0)goto _LL300;}}_LL2FF: if(env->noreturn){
const char*_tmp8E8;void*_tmp8E7;(_tmp8E7=0,Cyc_Tcutil_terr(s->loc,((_tmp8E8="`noreturn' function might return",
_tag_dyneither(_tmp8E8,sizeof(char),33))),_tag_dyneither(_tmp8E7,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);return Cyc_CfFlowInfo_BottomFL();
_LL300: {struct Cyc_Absyn_Return_s_struct*_tmp5D7=(struct Cyc_Absyn_Return_s_struct*)
_tmp5D3;if(_tmp5D7->tag != 3)goto _LL302;else{_tmp5D8=_tmp5D7->f1;}}_LL301: if(env->noreturn){
const char*_tmp8EB;void*_tmp8EA;(_tmp8EA=0,Cyc_Tcutil_terr(s->loc,((_tmp8EB="`noreturn' function might return",
_tag_dyneither(_tmp8EB,sizeof(char),33))),_tag_dyneither(_tmp8EA,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo _tmp61C;void*_tmp61D;struct _tuple13 _tmp61B=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp5D8));_tmp61C=_tmp61B.f1;
_tmp61D=_tmp61B.f2;_tmp61C=Cyc_CfFlowInfo_consume_unique_rvals(_tmp5D2,_tmp5D8->loc,
_tmp61C);_tmp61C=Cyc_NewControlFlow_use_Rval(env,_tmp5D8->loc,_tmp61C,_tmp61D);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp61C);return Cyc_CfFlowInfo_BottomFL();};
_LL302: {struct Cyc_Absyn_Exp_s_struct*_tmp5D9=(struct Cyc_Absyn_Exp_s_struct*)
_tmp5D3;if(_tmp5D9->tag != 1)goto _LL304;else{_tmp5DA=_tmp5D9->f1;}}_LL303: outflow=(
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp5DA)).f1;goto _LL2FB;_LL304: {struct
Cyc_Absyn_Seq_s_struct*_tmp5DB=(struct Cyc_Absyn_Seq_s_struct*)_tmp5D3;if(_tmp5DB->tag
!= 2)goto _LL306;else{_tmp5DC=_tmp5DB->f1;_tmp5DD=_tmp5DB->f2;}}_LL305: outflow=
Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5DC),
_tmp5DD);goto _LL2FB;_LL306: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp5DE=(struct
Cyc_Absyn_IfThenElse_s_struct*)_tmp5D3;if(_tmp5DE->tag != 4)goto _LL308;else{
_tmp5DF=_tmp5DE->f1;_tmp5E0=_tmp5DE->f2;_tmp5E1=_tmp5DE->f3;}}_LL307: {union Cyc_CfFlowInfo_FlowInfo
_tmp61F;union Cyc_CfFlowInfo_FlowInfo _tmp620;struct _tuple16 _tmp61E=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp5DF);_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;_tmp61F=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp5DF->loc,_tmp61F);_tmp620=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp5DF->loc,
_tmp620);outflow=Cyc_CfFlowInfo_join_flow(_tmp5D2,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp61F,_tmp5E0),Cyc_NewControlFlow_anal_stmt(env,_tmp620,_tmp5E1),1);goto
_LL2FB;}_LL308: {struct Cyc_Absyn_While_s_struct*_tmp5E2=(struct Cyc_Absyn_While_s_struct*)
_tmp5D3;if(_tmp5E2->tag != 5)goto _LL30A;else{_tmp5E3=_tmp5E2->f1;_tmp5E4=_tmp5E3.f1;
_tmp5E5=_tmp5E3.f2;_tmp5E6=_tmp5E2->f2;}}_LL309: {union Cyc_CfFlowInfo_FlowInfo*
_tmp622;struct _tuple17 _tmp621=Cyc_NewControlFlow_pre_stmt_check(env,inflow,
_tmp5E5);_tmp622=_tmp621.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp623=*_tmp622;union
Cyc_CfFlowInfo_FlowInfo _tmp625;union Cyc_CfFlowInfo_FlowInfo _tmp626;struct
_tuple16 _tmp624=Cyc_NewControlFlow_anal_test(env,_tmp623,_tmp5E4);_tmp625=
_tmp624.f1;_tmp626=_tmp624.f2;_tmp625=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp5E4->loc,_tmp625);_tmp626=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp5E4->loc,
_tmp626);{union Cyc_CfFlowInfo_FlowInfo _tmp627=Cyc_NewControlFlow_anal_stmt(env,
_tmp625,_tmp5E6);Cyc_NewControlFlow_update_flow(env,_tmp5E5,_tmp627);outflow=
_tmp626;goto _LL2FB;};};}_LL30A: {struct Cyc_Absyn_Do_s_struct*_tmp5E7=(struct Cyc_Absyn_Do_s_struct*)
_tmp5D3;if(_tmp5E7->tag != 14)goto _LL30C;else{_tmp5E8=_tmp5E7->f1;_tmp5E9=_tmp5E7->f2;
_tmp5EA=_tmp5E9.f1;_tmp5EB=_tmp5E9.f2;}}_LL30B: {union Cyc_CfFlowInfo_FlowInfo
_tmp628=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp5E8);union Cyc_CfFlowInfo_FlowInfo*
_tmp62A;struct _tuple17 _tmp629=Cyc_NewControlFlow_pre_stmt_check(env,_tmp628,
_tmp5EB);_tmp62A=_tmp629.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp62B=*_tmp62A;union
Cyc_CfFlowInfo_FlowInfo _tmp62D;union Cyc_CfFlowInfo_FlowInfo _tmp62E;struct
_tuple16 _tmp62C=Cyc_NewControlFlow_anal_test(env,_tmp62B,_tmp5EA);_tmp62D=
_tmp62C.f1;_tmp62E=_tmp62C.f2;Cyc_NewControlFlow_update_flow(env,_tmp5E8,_tmp62D);
outflow=_tmp62E;goto _LL2FB;};}_LL30C: {struct Cyc_Absyn_For_s_struct*_tmp5EC=(
struct Cyc_Absyn_For_s_struct*)_tmp5D3;if(_tmp5EC->tag != 9)goto _LL30E;else{
_tmp5ED=_tmp5EC->f1;_tmp5EE=_tmp5EC->f2;_tmp5EF=_tmp5EE.f1;_tmp5F0=_tmp5EE.f2;
_tmp5F1=_tmp5EC->f3;_tmp5F2=_tmp5F1.f1;_tmp5F3=_tmp5F1.f2;_tmp5F4=_tmp5EC->f4;}}
_LL30D: {union Cyc_CfFlowInfo_FlowInfo _tmp62F=(Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp5ED)).f1;_tmp62F=Cyc_CfFlowInfo_drop_unique_rvals(_tmp5ED->loc,
_tmp62F);{union Cyc_CfFlowInfo_FlowInfo*_tmp631;struct _tuple17 _tmp630=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp62F,_tmp5F0);_tmp631=_tmp630.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp632=*
_tmp631;union Cyc_CfFlowInfo_FlowInfo _tmp634;union Cyc_CfFlowInfo_FlowInfo _tmp635;
struct _tuple16 _tmp633=Cyc_NewControlFlow_anal_test(env,_tmp632,_tmp5EF);_tmp634=
_tmp633.f1;_tmp635=_tmp633.f2;{union Cyc_CfFlowInfo_FlowInfo _tmp636=Cyc_NewControlFlow_anal_stmt(
env,_tmp634,_tmp5F4);union Cyc_CfFlowInfo_FlowInfo*_tmp638;struct _tuple17 _tmp637=
Cyc_NewControlFlow_pre_stmt_check(env,_tmp636,_tmp5F3);_tmp638=_tmp637.f2;{union
Cyc_CfFlowInfo_FlowInfo _tmp639=*_tmp638;union Cyc_CfFlowInfo_FlowInfo _tmp63A=(Cyc_NewControlFlow_anal_Rexp(
env,_tmp639,_tmp5F2)).f1;_tmp63A=Cyc_CfFlowInfo_drop_unique_rvals(_tmp5F2->loc,
_tmp63A);Cyc_NewControlFlow_update_flow(env,_tmp5F0,_tmp63A);outflow=_tmp635;
goto _LL2FB;};};};};}_LL30E: {struct Cyc_Absyn_Break_s_struct*_tmp5F5=(struct Cyc_Absyn_Break_s_struct*)
_tmp5D3;if(_tmp5F5->tag != 6)goto _LL310;else{_tmp5F6=_tmp5F5->f1;if(_tmp5F6 != 0)
goto _LL310;}}_LL30F: return Cyc_CfFlowInfo_BottomFL();_LL310: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp5F7=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp5D3;if(_tmp5F7->tag != 11)goto
_LL312;else{_tmp5F8=_tmp5F7->f1;_tmp5F9=_tmp5F7->f2;if(_tmp5F9 == 0)goto _LL312;
_tmp5FA=*_tmp5F9;}}_LL311: {struct _RegionHandle*_tmp63B=env->r;union Cyc_CfFlowInfo_FlowInfo
_tmp63D;struct Cyc_List_List*_tmp63E;struct _tuple19 _tmp63C=Cyc_NewControlFlow_anal_unordered_Rexps(
_tmp63B,env,inflow,_tmp5F8,0);_tmp63D=_tmp63C.f1;_tmp63E=_tmp63C.f2;for(0;
_tmp63E != 0;(_tmp63E=_tmp63E->tl,_tmp5F8=_tmp5F8->tl)){_tmp63D=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5F8))->hd)->loc,
_tmp63D,(void*)_tmp63E->hd);}_tmp63D=Cyc_CfFlowInfo_consume_unique_rvals(_tmp5D2,
s->loc,_tmp63D);_tmp63D=Cyc_NewControlFlow_add_vars(_tmp5D2,_tmp63D,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp5FA->pat_vars))->v,_tmp5D2->unknown_all,s->loc);
Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)_tmp5FA->body,_tmp63D);
return Cyc_CfFlowInfo_BottomFL();}_LL312: {struct Cyc_Absyn_Break_s_struct*_tmp5FB=(
struct Cyc_Absyn_Break_s_struct*)_tmp5D3;if(_tmp5FB->tag != 6)goto _LL314;else{
_tmp5FC=_tmp5FB->f1;}}_LL313: _tmp5FE=_tmp5FC;goto _LL315;_LL314: {struct Cyc_Absyn_Continue_s_struct*
_tmp5FD=(struct Cyc_Absyn_Continue_s_struct*)_tmp5D3;if(_tmp5FD->tag != 7)goto
_LL316;else{_tmp5FE=_tmp5FD->f1;}}_LL315: _tmp600=_tmp5FE;goto _LL317;_LL316: {
struct Cyc_Absyn_Goto_s_struct*_tmp5FF=(struct Cyc_Absyn_Goto_s_struct*)_tmp5D3;
if(_tmp5FF->tag != 8)goto _LL318;else{_tmp600=_tmp5FF->f2;}}_LL317: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp63F=_tmp5D0->encloser;struct Cyc_Absyn_Stmt*
_tmp640=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp600)))->encloser;while(_tmp640 != _tmp63F){struct Cyc_Absyn_Stmt*_tmp641=(Cyc_NewControlFlow_get_stmt_annot(
_tmp63F))->encloser;if(_tmp641 == _tmp63F){{const char*_tmp8EE;void*_tmp8ED;(
_tmp8ED=0,Cyc_Tcutil_terr(s->loc,((_tmp8EE="goto enters local scope or exception handler",
_tag_dyneither(_tmp8EE,sizeof(char),45))),_tag_dyneither(_tmp8ED,sizeof(void*),0)));}
break;}_tmp63F=_tmp641;}}Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_check_null(_tmp600),inflow);return Cyc_CfFlowInfo_BottomFL();_LL318: {struct Cyc_Absyn_Switch_s_struct*
_tmp601=(struct Cyc_Absyn_Switch_s_struct*)_tmp5D3;if(_tmp601->tag != 10)goto
_LL31A;else{_tmp602=_tmp601->f1;_tmp603=_tmp601->f2;}}_LL319: {union Cyc_CfFlowInfo_FlowInfo
_tmp645;void*_tmp646;struct _tuple13 _tmp644=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp602);_tmp645=_tmp644.f1;_tmp646=_tmp644.f2;_tmp645=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp5D2,_tmp602->loc,_tmp645);_tmp645=Cyc_NewControlFlow_use_Rval(env,_tmp602->loc,
_tmp645,_tmp646);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp645,_tmp603);goto
_LL2FB;}_LL31A: {struct Cyc_Absyn_TryCatch_s_struct*_tmp604=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp5D3;if(_tmp604->tag != 15)goto _LL31C;else{_tmp605=_tmp604->f1;_tmp606=_tmp604->f2;}}
_LL31B: {int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp605);union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;env->in_try=
old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp606);{union Cyc_CfFlowInfo_FlowInfo _tmp647=scs_outflow;int
_tmp648;_LL327: if((_tmp647.BottomFL).tag != 1)goto _LL329;_tmp648=(int)(_tmp647.BottomFL).val;
_LL328: goto _LL326;_LL329:;_LL32A: {const char*_tmp8F1;void*_tmp8F0;(_tmp8F0=0,Cyc_Tcutil_terr(
s->loc,((_tmp8F1="last catch clause may implicitly fallthru",_tag_dyneither(
_tmp8F1,sizeof(char),42))),_tag_dyneither(_tmp8F0,sizeof(void*),0)));}_LL326:;}
outflow=s1_outflow;goto _LL2FB;};};}_LL31C: {struct Cyc_Absyn_Decl_s_struct*
_tmp607=(struct Cyc_Absyn_Decl_s_struct*)_tmp5D3;if(_tmp607->tag != 12)goto _LL31E;
else{_tmp608=_tmp607->f1;_tmp609=*_tmp608;_tmp60A=_tmp609.r;{struct Cyc_Absyn_Alias_d_struct*
_tmp60B=(struct Cyc_Absyn_Alias_d_struct*)_tmp60A;if(_tmp60B->tag != 5)goto _LL31E;
else{_tmp60C=_tmp60B->f1;_tmp60D=_tmp60B->f2;_tmp60E=_tmp60B->f3;}};_tmp60F=
_tmp607->f2;}}_LL31D: {union Cyc_CfFlowInfo_FlowInfo _tmp64C;void*_tmp64D;struct
_tuple13 _tmp64B=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp60C);_tmp64C=_tmp64B.f1;
_tmp64D=_tmp64B.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp64F;struct Cyc_List_List*
_tmp650;struct _tuple14 _tmp64E=Cyc_CfFlowInfo_save_consume_info(_tmp5D2,_tmp64C,0);
_tmp64F=_tmp64E.f1;_tmp650=_tmp64F.may_consume;_tmp64C=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp5D2,_tmp60C->loc,_tmp64C);_tmp64C=Cyc_NewControlFlow_use_Rval(env,_tmp60C->loc,
_tmp64C,_tmp64D);{struct Cyc_List_List _tmp8F2;struct Cyc_List_List _tmp651=(_tmp8F2.hd=
_tmp60E,((_tmp8F2.tl=0,_tmp8F2)));_tmp64C=Cyc_NewControlFlow_add_vars(_tmp5D2,
_tmp64C,(struct Cyc_List_List*)& _tmp651,_tmp5D2->unknown_all,s->loc);outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp64C,_tmp60F);{union Cyc_CfFlowInfo_FlowInfo _tmp652=outflow;int _tmp653;
struct _tuple12 _tmp654;struct Cyc_Dict_Dict _tmp655;struct Cyc_List_List*_tmp656;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp657;_LL32C: if((_tmp652.BottomFL).tag != 1)
goto _LL32E;_tmp653=(int)(_tmp652.BottomFL).val;_LL32D: goto _LL32B;_LL32E: if((
_tmp652.ReachableFL).tag != 2)goto _LL32B;_tmp654=(struct _tuple12)(_tmp652.ReachableFL).val;
_tmp655=_tmp654.f1;_tmp656=_tmp654.f2;_tmp657=_tmp654.f3;_LL32F: while(_tmp650 != 
0){struct Cyc_Dict_Dict _tmp658=_tmp657.consumed;_tmp657.consumed=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))Cyc_Dict_rdelete)(
_tmp5D2->r,_tmp657.consumed,(struct Cyc_CfFlowInfo_Place*)_tmp650->hd);if((
_tmp657.consumed).t != _tmp658.t);_tmp650=_tmp650->tl;}outflow=Cyc_CfFlowInfo_ReachableFL(
_tmp655,_tmp656,_tmp657);goto _LL32B;_LL32B:;}goto _LL2FB;};};}_LL31E: {struct Cyc_Absyn_Decl_s_struct*
_tmp610=(struct Cyc_Absyn_Decl_s_struct*)_tmp5D3;if(_tmp610->tag != 12)goto _LL320;
else{_tmp611=_tmp610->f1;_tmp612=_tmp610->f2;}}_LL31F: outflow=Cyc_NewControlFlow_anal_stmt(
env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp611),_tmp612);goto _LL2FB;_LL320: {
struct Cyc_Absyn_Label_s_struct*_tmp613=(struct Cyc_Absyn_Label_s_struct*)_tmp5D3;
if(_tmp613->tag != 13)goto _LL322;else{_tmp614=_tmp613->f2;}}_LL321: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp614);goto _LL2FB;_LL322: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp615=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp5D3;if(_tmp615->tag != 16)goto
_LL324;else{_tmp616=_tmp615->f1;}}_LL323: {union Cyc_CfFlowInfo_FlowInfo _tmp65B;
void*_tmp65C;struct _tuple13 _tmp65A=Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp616);_tmp65B=_tmp65A.f1;_tmp65C=_tmp65A.f2;_tmp65B=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp616->loc,_tmp65B);{union Cyc_CfFlowInfo_FlowInfo _tmp65D=Cyc_NewControlFlow_use_Rval(
env,_tmp616->loc,_tmp65B,_tmp65C);{void*_tmp65E=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp616->topt))->v);void*_tmp660;_LL331: {struct
Cyc_Absyn_RgnHandleType_struct*_tmp65F=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp65E;if(_tmp65F->tag != 16)goto _LL333;else{_tmp660=(void*)_tmp65F->f1;}}_LL332:
outflow=Cyc_CfFlowInfo_kill_flow_region(_tmp5D2,_tmp65B,_tmp660);goto _LL330;
_LL333:;_LL334: {struct Cyc_Core_Impossible_struct _tmp8F8;const char*_tmp8F7;
struct Cyc_Core_Impossible_struct*_tmp8F6;(int)_throw((void*)((_tmp8F6=_cycalloc(
sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F8.tag=Cyc_Core_Impossible,((_tmp8F8.f1=((
_tmp8F7="anal_stmt -- reset_region",_tag_dyneither(_tmp8F7,sizeof(char),26))),
_tmp8F8)))),_tmp8F6)))));}_LL330:;}goto _LL2FB;};}_LL324:;_LL325: {struct Cyc_Core_Impossible_struct
_tmp8FE;const char*_tmp8FD;struct Cyc_Core_Impossible_struct*_tmp8FC;(int)_throw((
void*)((_tmp8FC=_cycalloc(sizeof(*_tmp8FC)),((_tmp8FC[0]=((_tmp8FE.tag=Cyc_Core_Impossible,((
_tmp8FE.f1=((_tmp8FD="anal_stmt -- bad stmt syntax or unimplemented stmt form",
_tag_dyneither(_tmp8FD,sizeof(char),56))),_tmp8FE)))),_tmp8FC)))));}_LL2FB:;}
outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo
_tmp667=outflow;struct _tuple12 _tmp668;struct Cyc_CfFlowInfo_ConsumeInfo _tmp669;
_LL336: if((_tmp667.ReachableFL).tag != 2)goto _LL338;_tmp668=(struct _tuple12)(
_tmp667.ReachableFL).val;_tmp669=_tmp668.f3;_LL337: goto _LL335;_LL338:;_LL339:
goto _LL335;_LL335:;}return outflow;};}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl);static union Cyc_CfFlowInfo_FlowInfo
Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo
inflow,struct Cyc_Absyn_Decl*decl){struct Cyc_CfFlowInfo_FlowEnv*_tmp66A=env->fenv;
void*_tmp66B=decl->r;struct Cyc_Absyn_Vardecl*_tmp66D;struct Cyc_Core_Opt*_tmp66F;
struct Cyc_Core_Opt _tmp670;struct Cyc_List_List*_tmp671;struct Cyc_Absyn_Exp*
_tmp672;struct Cyc_List_List*_tmp674;struct Cyc_Absyn_Fndecl*_tmp676;struct Cyc_Absyn_Tvar*
_tmp678;struct Cyc_Absyn_Vardecl*_tmp679;int _tmp67A;struct Cyc_Absyn_Exp*_tmp67B;
_LL33B: {struct Cyc_Absyn_Var_d_struct*_tmp66C=(struct Cyc_Absyn_Var_d_struct*)
_tmp66B;if(_tmp66C->tag != 0)goto _LL33D;else{_tmp66D=_tmp66C->f1;}}_LL33C: {
struct Cyc_List_List _tmp8FF;struct Cyc_List_List _tmp67C=(_tmp8FF.hd=_tmp66D,((
_tmp8FF.tl=0,_tmp8FF)));inflow=Cyc_NewControlFlow_add_vars(_tmp66A,inflow,(
struct Cyc_List_List*)& _tmp67C,_tmp66A->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*
_tmp67D=_tmp66D->initializer;if(_tmp67D == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo
_tmp67F;void*_tmp680;struct _tuple13 _tmp67E=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp67D);_tmp67F=_tmp67E.f1;_tmp680=_tmp67E.f2;
_tmp67F=Cyc_CfFlowInfo_consume_unique_rvals(_tmp66A,_tmp67D->loc,_tmp67F);{union
Cyc_CfFlowInfo_FlowInfo _tmp681=_tmp67F;int _tmp682;struct _tuple12 _tmp683;struct
Cyc_Dict_Dict _tmp684;struct Cyc_List_List*_tmp685;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp686;_LL348: if((_tmp681.BottomFL).tag != 1)goto _LL34A;_tmp682=(int)(_tmp681.BottomFL).val;
_LL349: return Cyc_CfFlowInfo_BottomFL();_LL34A: if((_tmp681.ReachableFL).tag != 2)
goto _LL347;_tmp683=(struct _tuple12)(_tmp681.ReachableFL).val;_tmp684=_tmp683.f1;
_tmp685=_tmp683.f2;_tmp686=_tmp683.f3;_LL34B:{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp905;struct Cyc_CfFlowInfo_VarRoot_struct _tmp904;struct Cyc_CfFlowInfo_Place*
_tmp903;_tmp684=Cyc_CfFlowInfo_assign_place(_tmp66A,decl->loc,_tmp684,env->all_changed,((
_tmp903=_region_malloc(env->r,sizeof(*_tmp903)),((_tmp903->root=(void*)((_tmp905=
_region_malloc(env->r,sizeof(*_tmp905)),((_tmp905[0]=((_tmp904.tag=0,((_tmp904.f1=
_tmp66D,_tmp904)))),_tmp905)))),((_tmp903->fields=0,_tmp903)))))),_tmp680);}
_tmp685=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp685,_tmp66D,(struct Cyc_Absyn_Exp*)
_check_null(_tmp66D->initializer));{union Cyc_CfFlowInfo_FlowInfo _tmp68A=Cyc_CfFlowInfo_ReachableFL(
_tmp684,_tmp685,_tmp686);Cyc_NewControlFlow_update_tryflow(env,_tmp68A);return
_tmp68A;};_LL347:;};};};}_LL33D: {struct Cyc_Absyn_Let_d_struct*_tmp66E=(struct
Cyc_Absyn_Let_d_struct*)_tmp66B;if(_tmp66E->tag != 2)goto _LL33F;else{_tmp66F=
_tmp66E->f2;if(_tmp66F == 0)goto _LL33F;_tmp670=*_tmp66F;_tmp671=(struct Cyc_List_List*)
_tmp670.v;_tmp672=_tmp66E->f3;}}_LL33E: {union Cyc_CfFlowInfo_FlowInfo _tmp68D;
void*_tmp68E;struct _tuple13 _tmp68C=Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp672);_tmp68D=_tmp68C.f1;_tmp68E=_tmp68C.f2;_tmp68D=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp66A,_tmp672->loc,_tmp68D);_tmp68D=Cyc_NewControlFlow_use_Rval(env,_tmp672->loc,
_tmp68D,_tmp68E);return Cyc_NewControlFlow_add_vars(_tmp66A,_tmp68D,_tmp671,
_tmp66A->unknown_all,decl->loc);}_LL33F: {struct Cyc_Absyn_Letv_d_struct*_tmp673=(
struct Cyc_Absyn_Letv_d_struct*)_tmp66B;if(_tmp673->tag != 3)goto _LL341;else{
_tmp674=_tmp673->f1;}}_LL340: return Cyc_NewControlFlow_add_vars(_tmp66A,inflow,
_tmp674,_tmp66A->unknown_none,decl->loc);_LL341: {struct Cyc_Absyn_Fn_d_struct*
_tmp675=(struct Cyc_Absyn_Fn_d_struct*)_tmp66B;if(_tmp675->tag != 1)goto _LL343;
else{_tmp676=_tmp675->f1;}}_LL342: Cyc_NewControlFlow_check_nested_fun(_tmp66A,
inflow,_tmp676);{void*_tmp68F=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp676->cached_typ))->v;
struct Cyc_Absyn_Vardecl*_tmp690=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp676->fn_vardecl);
struct Cyc_List_List*_tmp906;return Cyc_NewControlFlow_add_vars(_tmp66A,inflow,((
_tmp906=_region_malloc(env->r,sizeof(*_tmp906)),((_tmp906->hd=_tmp690,((_tmp906->tl=
0,_tmp906)))))),_tmp66A->unknown_all,decl->loc);};_LL343: {struct Cyc_Absyn_Region_d_struct*
_tmp677=(struct Cyc_Absyn_Region_d_struct*)_tmp66B;if(_tmp677->tag != 4)goto _LL345;
else{_tmp678=_tmp677->f1;_tmp679=_tmp677->f2;_tmp67A=_tmp677->f3;_tmp67B=_tmp677->f4;}}
_LL344: if(_tmp67B != 0){struct Cyc_Absyn_Exp*_tmp692=(struct Cyc_Absyn_Exp*)_tmp67B;
union Cyc_CfFlowInfo_FlowInfo _tmp694;void*_tmp695;struct _tuple13 _tmp693=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp692);_tmp694=_tmp693.f1;_tmp695=_tmp693.f2;_tmp694=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp66A,_tmp692->loc,_tmp694);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp692->loc,
_tmp694,_tmp695);}{struct Cyc_List_List _tmp907;struct Cyc_List_List _tmp696=(
_tmp907.hd=_tmp679,((_tmp907.tl=0,_tmp907)));return Cyc_NewControlFlow_add_vars(
_tmp66A,inflow,(struct Cyc_List_List*)& _tmp696,_tmp66A->unknown_all,decl->loc);};
_LL345:;_LL346: {struct Cyc_Core_Impossible_struct _tmp90D;const char*_tmp90C;
struct Cyc_Core_Impossible_struct*_tmp90B;(int)_throw((void*)((_tmp90B=_cycalloc(
sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90D.tag=Cyc_Core_Impossible,((_tmp90D.f1=((
_tmp90C="anal_decl: unexpected decl variant",_tag_dyneither(_tmp90C,sizeof(char),
35))),_tmp90D)))),_tmp90B)))));}_LL33A:;}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp69B=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp69B;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);struct Cyc_CfFlowInfo_ConsumeInfo _tmp90E;Cyc_NewControlFlow_check_nested_fun(
fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0,((_tmp90E.consumed=fenv->mt_place_set,((
_tmp90E.may_consume=0,_tmp90E))))),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s);static int Cyc_NewControlFlow_hash_ptr(void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*
fd);static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp69D=fenv->r;struct Cyc_Position_Segment*_tmp69E=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp69E);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo _tmp69F=inflow;int _tmp6A0;struct
_tuple12 _tmp6A1;struct Cyc_Dict_Dict _tmp6A2;struct Cyc_List_List*_tmp6A3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp6A4;_LL34D: if((_tmp69F.BottomFL).tag != 1)goto _LL34F;_tmp6A0=(int)(_tmp69F.BottomFL).val;
_LL34E: {const char*_tmp911;void*_tmp910;(_tmp910=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp911="check_fun",
_tag_dyneither(_tmp911,sizeof(char),10))),_tag_dyneither(_tmp910,sizeof(void*),0)));}
_LL34F: if((_tmp69F.ReachableFL).tag != 2)goto _LL34C;_tmp6A1=(struct _tuple12)(
_tmp69F.ReachableFL).val;_tmp6A2=_tmp6A1.f1;_tmp6A3=_tmp6A1.f2;_tmp6A4=_tmp6A1.f3;
_LL350: {struct Cyc_List_List*atts;{void*_tmp6A7=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo
_tmp6A9;struct Cyc_List_List*_tmp6AA;_LL352: {struct Cyc_Absyn_FnType_struct*
_tmp6A8=(struct Cyc_Absyn_FnType_struct*)_tmp6A7;if(_tmp6A8->tag != 10)goto _LL354;
else{_tmp6A9=_tmp6A8->f1;_tmp6AA=_tmp6A9.attributes;}}_LL353: atts=_tmp6AA;goto
_LL351;_LL354:;_LL355: {const char*_tmp914;void*_tmp913;(_tmp913=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp914="check_fun: non-function type cached with fndecl_t",
_tag_dyneither(_tmp914,sizeof(char),50))),_tag_dyneither(_tmp913,sizeof(void*),0)));}
_LL351:;}for(0;atts != 0;atts=atts->tl){void*_tmp6AD=(void*)atts->hd;int _tmp6AF;
_LL357: {struct Cyc_Absyn_Initializes_att_struct*_tmp6AE=(struct Cyc_Absyn_Initializes_att_struct*)
_tmp6AD;if(_tmp6AE->tag != 20)goto _LL359;else{_tmp6AF=_tmp6AE->f1;}}_LL358: {
unsigned int j=(unsigned int)_tmp6AF;if(j > ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v)){{
const char*_tmp917;void*_tmp916;(_tmp916=0,Cyc_Tcutil_terr(_tmp69E,((_tmp917="initializes attribute exceeds number of parameters",
_tag_dyneither(_tmp917,sizeof(char),51))),_tag_dyneither(_tmp916,sizeof(void*),0)));}
continue;}{struct Cyc_Absyn_Vardecl*_tmp6B2=((struct Cyc_Absyn_Vardecl*(*)(struct
Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(int)(j - 1));{void*_tmp6B3=Cyc_Tcutil_compress(
_tmp6B2->type);struct Cyc_Absyn_PtrInfo _tmp6B5;void*_tmp6B6;struct Cyc_Absyn_PtrAtts
_tmp6B7;union Cyc_Absyn_Constraint*_tmp6B8;union Cyc_Absyn_Constraint*_tmp6B9;
union Cyc_Absyn_Constraint*_tmp6BA;_LL35C: {struct Cyc_Absyn_PointerType_struct*
_tmp6B4=(struct Cyc_Absyn_PointerType_struct*)_tmp6B3;if(_tmp6B4->tag != 5)goto
_LL35E;else{_tmp6B5=_tmp6B4->f1;_tmp6B6=_tmp6B5.elt_typ;_tmp6B7=_tmp6B5.ptr_atts;
_tmp6B8=_tmp6B7.nullable;_tmp6B9=_tmp6B7.bounds;_tmp6BA=_tmp6B7.zero_term;}}
_LL35D: if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6B8)){
const char*_tmp91A;void*_tmp919;(_tmp919=0,Cyc_Tcutil_terr(_tmp69E,((_tmp91A="initializes attribute not allowed on nullable pointers",
_tag_dyneither(_tmp91A,sizeof(char),55))),_tag_dyneither(_tmp919,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_bound_one(_tmp6B9)){const char*_tmp91D;void*_tmp91C;(_tmp91C=0,
Cyc_Tcutil_terr(_tmp69E,((_tmp91D="initializes attribute allowed only on pointers of size 1",
_tag_dyneither(_tmp91D,sizeof(char),57))),_tag_dyneither(_tmp91C,sizeof(void*),0)));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6BA)){
const char*_tmp920;void*_tmp91F;(_tmp91F=0,Cyc_Tcutil_terr(_tmp69E,((_tmp920="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dyneither(_tmp920,sizeof(char),77))),_tag_dyneither(_tmp91F,sizeof(void*),0)));}{
struct Cyc_CfFlowInfo_InitParam_struct _tmp923;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp922;struct Cyc_CfFlowInfo_InitParam_struct*_tmp6C1=(_tmp922=_region_malloc(
_tmp69D,sizeof(*_tmp922)),((_tmp922[0]=((_tmp923.tag=2,((_tmp923.f1=(int)j,((
_tmp923.f2=(void*)_tmp6B6,_tmp923)))))),_tmp922)));struct Cyc_CfFlowInfo_Place*
_tmp924;struct Cyc_CfFlowInfo_Place*_tmp6C2=(_tmp924=_region_malloc(_tmp69D,
sizeof(*_tmp924)),((_tmp924->root=(void*)_tmp6C1,((_tmp924->fields=0,_tmp924)))));
_tmp6A2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6A2,(void*)_tmp6C1,Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp6B6,fenv->esc_none));{
struct Cyc_CfFlowInfo_AddressOf_struct _tmp92A;struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp929;struct Cyc_CfFlowInfo_VarRoot_struct _tmp927;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp926;_tmp6A2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(
_tmp6A2,(void*)((_tmp926=_region_malloc(_tmp69D,sizeof(*_tmp926)),((_tmp926[0]=((
_tmp927.tag=0,((_tmp927.f1=_tmp6B2,_tmp927)))),_tmp926)))),(void*)((_tmp929=
_region_malloc(_tmp69D,sizeof(*_tmp929)),((_tmp929[0]=((_tmp92A.tag=5,((_tmp92A.f1=
_tmp6C2,_tmp92A)))),_tmp929)))));}{struct Cyc_List_List*_tmp92B;param_roots=((
_tmp92B=_region_malloc(_tmp69D,sizeof(*_tmp92B)),((_tmp92B->hd=_tmp6C2,((_tmp92B->tl=
param_roots,_tmp92B))))));}goto _LL35B;};_LL35E:;_LL35F: {const char*_tmp92E;void*
_tmp92D;(_tmp92D=0,Cyc_Tcutil_terr(_tmp69E,((_tmp92E="initializes attribute on non-pointer",
_tag_dyneither(_tmp92E,sizeof(char),37))),_tag_dyneither(_tmp92D,sizeof(void*),0)));}
_LL35B:;}goto _LL356;};}_LL359:;_LL35A: goto _LL356;_LL356:;}inflow=Cyc_CfFlowInfo_ReachableFL(
_tmp6A2,_tmp6A3,_tmp6A4);}_LL34C:;}{int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(struct
_RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*
hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp69D,33,(int(*)(struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*_tmp92F;struct
Cyc_NewControlFlow_AnalEnv*env=(_tmp92F=_region_malloc(_tmp69D,sizeof(*_tmp92F)),((
_tmp92F->r=_tmp69D,((_tmp92F->fenv=fenv,((_tmp92F->iterate_again=1,((_tmp92F->iteration_num=
0,((_tmp92F->in_try=0,((_tmp92F->tryflow=inflow,((_tmp92F->all_changed=0,((
_tmp92F->noreturn=noreturn,((_tmp92F->param_roots=param_roots,((_tmp92F->flow_table=
flow_table,_tmp92F)))))))))))))))))))));union Cyc_CfFlowInfo_FlowInfo outflow=
inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo
_tmp6CD=outflow;int _tmp6CE;_LL361: if((_tmp6CD.BottomFL).tag != 1)goto _LL363;
_tmp6CE=(int)(_tmp6CD.BottomFL).val;_LL362: goto _LL360;_LL363:;_LL364: Cyc_NewControlFlow_check_init_params(
_tmp69E,env,outflow);if(noreturn){const char*_tmp932;void*_tmp931;(_tmp931=0,Cyc_Tcutil_terr(
_tmp69E,((_tmp932="`noreturn' function might (implicitly) return",_tag_dyneither(
_tmp932,sizeof(char),46))),_tag_dyneither(_tmp931,sizeof(void*),0)));}else{void*
_tmp6D1=Cyc_Tcutil_compress(fd->ret_type);_LL366: {struct Cyc_Absyn_VoidType_struct*
_tmp6D2=(struct Cyc_Absyn_VoidType_struct*)_tmp6D1;if(_tmp6D2->tag != 0)goto _LL368;}
_LL367: goto _LL365;_LL368: {struct Cyc_Absyn_DoubleType_struct*_tmp6D3=(struct Cyc_Absyn_DoubleType_struct*)
_tmp6D1;if(_tmp6D3->tag != 8)goto _LL36A;}_LL369: goto _LL36B;_LL36A: {struct Cyc_Absyn_FloatType_struct*
_tmp6D4=(struct Cyc_Absyn_FloatType_struct*)_tmp6D1;if(_tmp6D4->tag != 7)goto
_LL36C;}_LL36B: goto _LL36D;_LL36C: {struct Cyc_Absyn_IntType_struct*_tmp6D5=(
struct Cyc_Absyn_IntType_struct*)_tmp6D1;if(_tmp6D5->tag != 6)goto _LL36E;}_LL36D:{
const char*_tmp935;void*_tmp934;(_tmp934=0,Cyc_Tcutil_warn(_tmp69E,((_tmp935="function may not return a value",
_tag_dyneither(_tmp935,sizeof(char),32))),_tag_dyneither(_tmp934,sizeof(void*),0)));}
goto _LL365;_LL36E:;_LL36F:{const char*_tmp938;void*_tmp937;(_tmp937=0,Cyc_Tcutil_terr(
_tmp69E,((_tmp938="function may not return a value",_tag_dyneither(_tmp938,
sizeof(char),32))),_tag_dyneither(_tmp937,sizeof(void*),0)));}goto _LL365;_LL365:;}
goto _LL360;_LL360:;};};};}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){
void*_tmp6DB=((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp6DD;
struct Cyc_List_List*_tmp6DF;struct Cyc_List_List*_tmp6E1;struct Cyc_List_List*
_tmp6E3;_LL371: {struct Cyc_Absyn_Fn_d_struct*_tmp6DC=(struct Cyc_Absyn_Fn_d_struct*)
_tmp6DB;if(_tmp6DC->tag != 1)goto _LL373;else{_tmp6DD=_tmp6DC->f1;}}_LL372: Cyc_NewControlFlow_check_fun(
_tmp6DD);goto _LL370;_LL373: {struct Cyc_Absyn_ExternC_d_struct*_tmp6DE=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp6DB;if(_tmp6DE->tag != 12)goto _LL375;else{_tmp6DF=_tmp6DE->f1;}}_LL374:
_tmp6E1=_tmp6DF;goto _LL376;_LL375: {struct Cyc_Absyn_Using_d_struct*_tmp6E0=(
struct Cyc_Absyn_Using_d_struct*)_tmp6DB;if(_tmp6E0->tag != 11)goto _LL377;else{
_tmp6E1=_tmp6E0->f2;}}_LL376: _tmp6E3=_tmp6E1;goto _LL378;_LL377: {struct Cyc_Absyn_Namespace_d_struct*
_tmp6E2=(struct Cyc_Absyn_Namespace_d_struct*)_tmp6DB;if(_tmp6E2->tag != 10)goto
_LL379;else{_tmp6E3=_tmp6E2->f2;}}_LL378: Cyc_NewControlFlow_cf_check(_tmp6E3);
goto _LL370;_LL379: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp6E4=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp6DB;if(_tmp6E4->tag != 13)goto _LL37B;}_LL37A: goto _LL370;_LL37B:;_LL37C: goto
_LL370;_LL370:;}}
