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
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
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
x);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);int Cyc_zstrcmp(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
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
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg);static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);static void*Cyc_Parse_abort(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{
struct Cyc_Position_Exit_struct _tmp85A;struct Cyc_Position_Exit_struct*_tmp859;(
int)_throw((void*)((_tmp859=_cycalloc_atomic(sizeof(*_tmp859)),((_tmp859[0]=((
_tmp85A.tag=Cyc_Position_Exit,_tmp85A)),_tmp859)))));}}static void Cyc_Parse_unimp(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg);static void Cyc_Parse_unimp(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){{const char*_tmp85F;void*
_tmp85E[2];struct Cyc_String_pa_struct _tmp85D;struct Cyc_String_pa_struct _tmp85C;(
_tmp85C.tag=0,((_tmp85C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((
_tmp85D.tag=0,((_tmp85D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
sg)),((_tmp85E[0]=& _tmp85D,((_tmp85E[1]=& _tmp85C,Cyc_fprintf(Cyc_stderr,((
_tmp85F="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp85F,
sizeof(char),46))),_tag_dyneither(_tmp85E,sizeof(void*),2)))))))))))));}return;}
struct _tuple13{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union
Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple13 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs);static struct _tuple13 Cyc_Parse_collapse_pointer_quals(
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
else{_tmpE=_tmpD->f1;}}_LLE:{struct Cyc_Absyn_Upper_b_struct _tmp862;struct Cyc_Absyn_Upper_b_struct*
_tmp861;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861[0]=((_tmp862.tag=1,((
_tmp862.f1=_tmpE,_tmp862)))),_tmp861)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*
_tmpF=(struct Cyc_Absyn_Region_ptrqual_struct*)_tmp6;if(_tmpF->tag != 1)goto _LL0;
else{_tmp10=(void*)_tmpF->f1;}}_LL10: rgn=_tmp10;goto _LL0;_LL0:;}{struct _tuple13
_tmp863;return(_tmp863.f1=nullable,((_tmp863.f2=bound,((_tmp863.f3=zeroterm,((
_tmp863.f4=rgn,_tmp863)))))));}}static int Cyc_Parse_enum_counter=0;struct _tuple1*
Cyc_Parse_gensym_enum();struct _tuple1*Cyc_Parse_gensym_enum(){struct
_dyneither_ptr*_tmp870;const char*_tmp86F;void*_tmp86E[1];struct Cyc_Int_pa_struct
_tmp86D;struct _tuple1*_tmp86C;return(_tmp86C=_cycalloc(sizeof(*_tmp86C)),((
_tmp86C->f1=Cyc_Absyn_Rel_n(0),((_tmp86C->f2=((_tmp870=_cycalloc(sizeof(*_tmp870)),((
_tmp870[0]=(struct _dyneither_ptr)((_tmp86D.tag=1,((_tmp86D.f1=(unsigned long)Cyc_Parse_enum_counter
++,((_tmp86E[0]=& _tmp86D,Cyc_aprintf(((_tmp86F="__anonymous_enum_%d__",
_tag_dyneither(_tmp86F,sizeof(char),22))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))))),
_tmp870)))),_tmp86C)))));}struct _tuple14{struct _tuple1*f1;struct Cyc_Absyn_Tqual
f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple15{struct
_tuple14*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple15*field_info);static struct Cyc_Absyn_Aggrfield*
Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*loc,struct _tuple15*
field_info){struct _tuple15 _tmp1A;struct _tuple14*_tmp1B;struct _tuple14 _tmp1C;
struct _tuple1*_tmp1D;struct Cyc_Absyn_Tqual _tmp1E;void*_tmp1F;struct Cyc_List_List*
_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp22;struct _tuple15*
_tmp19=field_info;_tmp1A=*_tmp19;_tmp1B=_tmp1A.f1;_tmp1C=*_tmp1B;_tmp1D=_tmp1C.f1;
_tmp1E=_tmp1C.f2;_tmp1F=_tmp1C.f3;_tmp20=_tmp1C.f4;_tmp21=_tmp1C.f5;_tmp22=
_tmp1A.f2;if(_tmp20 != 0){const char*_tmp871;Cyc_Parse_err(((_tmp871="bad type params in struct field",
_tag_dyneither(_tmp871,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(
_tmp1D)){const char*_tmp872;Cyc_Parse_err(((_tmp872="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp872,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp873;return(_tmp873=_cycalloc(sizeof(*_tmp873)),((_tmp873->name=(*_tmp1D).f2,((
_tmp873->tq=_tmp1E,((_tmp873->type=_tmp1F,((_tmp873->width=_tmp22,((_tmp873->attributes=
_tmp21,_tmp873)))))))))));}}static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*
loc);static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){
struct Cyc_Parse_Type_spec_struct _tmp876;struct Cyc_Parse_Type_spec_struct*_tmp875;
return(void*)((_tmp875=_cycalloc(sizeof(*_tmp875)),((_tmp875[0]=((_tmp876.tag=4,((
_tmp876.f1=(void*)t,((_tmp876.f2=loc,_tmp876)))))),_tmp875))));}static void*Cyc_Parse_array2ptr(
void*t,int argposn);static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp28=
t;struct Cyc_Absyn_ArrayInfo _tmp2A;void*_tmp2B;struct Cyc_Absyn_Tqual _tmp2C;struct
Cyc_Absyn_Exp*_tmp2D;union Cyc_Absyn_Constraint*_tmp2E;struct Cyc_Position_Segment*
_tmp2F;_LL12: {struct Cyc_Absyn_ArrayType_struct*_tmp29=(struct Cyc_Absyn_ArrayType_struct*)
_tmp28;if(_tmp29->tag != 9)goto _LL14;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp2A.elt_type;
_tmp2C=_tmp2A.tq;_tmp2D=_tmp2A.num_elts;_tmp2E=_tmp2A.zero_term;_tmp2F=_tmp2A.zt_loc;}}
_LL13: {struct Cyc_Absyn_Upper_b_struct _tmp879;struct Cyc_Absyn_Upper_b_struct*
_tmp878;return Cyc_Absyn_starb_typ(_tmp2B,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp2C,_tmp2D == 0?(void*)& Cyc_Absyn_DynEither_b_val:(
void*)((void*)((_tmp878=_cycalloc(sizeof(*_tmp878)),((_tmp878[0]=((_tmp879.tag=1,((
_tmp879.f1=(struct Cyc_Absyn_Exp*)_tmp2D,_tmp879)))),_tmp878))))),_tmp2E);}_LL14:;
_LL15: return t;_LL11:;}struct _tuple16{struct _dyneither_ptr*f1;void*f2;};static
struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x);static struct
Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){struct Cyc_List_List*
_tmp32=0;for(0;x != 0;x=x->tl){struct _tuple8*_tmp33=(struct _tuple8*)x->hd;struct
_tuple8 _tmp34;struct Cyc_Core_Opt*_tmp35;struct Cyc_Core_Opt _tmp36;struct
_dyneither_ptr*_tmp37;void*_tmp38;void*_tmp3A;struct _tuple8 _tmp3B;struct Cyc_Core_Opt*
_tmp3C;struct Cyc_Core_Opt _tmp3D;struct _dyneither_ptr*_tmp3E;void*_tmp3F;void*
_tmp41;struct Cyc_Core_Opt*_tmp43;struct Cyc_Core_Opt**_tmp44;_LL17: _tmp34=*_tmp33;
_tmp35=_tmp34.f1;if(_tmp35 == 0)goto _LL19;_tmp36=*_tmp35;_tmp37=(struct
_dyneither_ptr*)_tmp36.v;_tmp38=_tmp34.f3;{struct Cyc_Absyn_TagType_struct*_tmp39=(
struct Cyc_Absyn_TagType_struct*)_tmp38;if(_tmp39->tag != 20)goto _LL19;else{_tmp3A=(
void*)_tmp39->f1;}}_LL18:{void*_tmp45=_tmp3A;struct Cyc_Core_Opt*_tmp47;struct Cyc_Core_Opt**
_tmp48;_LL1E: {struct Cyc_Absyn_Evar_struct*_tmp46=(struct Cyc_Absyn_Evar_struct*)
_tmp45;if(_tmp46->tag != 1)goto _LL20;else{_tmp47=_tmp46->f2;_tmp48=(struct Cyc_Core_Opt**)&
_tmp46->f2;}}_LL1F: {struct Cyc_String_pa_struct _tmp881;void*_tmp880[1];const char*
_tmp87F;struct _dyneither_ptr*_tmp87E;struct _dyneither_ptr*nm=(struct
_dyneither_ptr*)((_tmp87E=_cycalloc(sizeof(*_tmp87E)),((_tmp87E[0]=((_tmp881.tag=
0,((_tmp881.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp37),((_tmp880[0]=&
_tmp881,Cyc_aprintf(((_tmp87F="`%s",_tag_dyneither(_tmp87F,sizeof(char),4))),
_tag_dyneither(_tmp880,sizeof(void*),1)))))))),_tmp87E))));{struct Cyc_Absyn_VarType_struct*
_tmp896;struct Cyc_Absyn_Tvar*_tmp895;struct Cyc_Absyn_Eq_kb_struct _tmp894;struct
Cyc_Absyn_Eq_kb_struct*_tmp893;struct Cyc_Absyn_VarType_struct _tmp892;struct Cyc_Core_Opt*
_tmp891;*_tmp48=((_tmp891=_cycalloc(sizeof(*_tmp891)),((_tmp891->v=(void*)((
_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896[0]=((_tmp892.tag=2,((_tmp892.f1=((
_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895->name=nm,((_tmp895->identity=- 1,((
_tmp895->kind=(void*)((_tmp893=_cycalloc_atomic(sizeof(*_tmp893)),((_tmp893[0]=((
_tmp894.tag=0,((_tmp894.f1=Cyc_Absyn_IntKind,_tmp894)))),_tmp893)))),_tmp895)))))))),
_tmp892)))),_tmp896)))),_tmp891))));}goto _LL1D;}_LL20:;_LL21: goto _LL1D;_LL1D:;}{
struct _tuple16*_tmp899;struct Cyc_List_List*_tmp898;_tmp32=((_tmp898=_cycalloc(
sizeof(*_tmp898)),((_tmp898->hd=((_tmp899=_cycalloc(sizeof(*_tmp899)),((_tmp899->f1=
_tmp37,((_tmp899->f2=_tmp3A,_tmp899)))))),((_tmp898->tl=_tmp32,_tmp898))))));}
goto _LL16;_LL19: _tmp3B=*_tmp33;_tmp3C=_tmp3B.f1;if(_tmp3C == 0)goto _LL1B;_tmp3D=*
_tmp3C;_tmp3E=(struct _dyneither_ptr*)_tmp3D.v;_tmp3F=_tmp3B.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp40=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp3F;if(_tmp40->tag != 16)goto
_LL1B;else{_tmp41=(void*)_tmp40->f1;{struct Cyc_Absyn_Evar_struct*_tmp42=(struct
Cyc_Absyn_Evar_struct*)_tmp41;if(_tmp42->tag != 1)goto _LL1B;else{_tmp43=_tmp42->f2;
_tmp44=(struct Cyc_Core_Opt**)& _tmp42->f2;}}}}_LL1A: {struct Cyc_String_pa_struct
_tmp8A1;void*_tmp8A0[1];const char*_tmp89F;struct _dyneither_ptr*_tmp89E;struct
_dyneither_ptr*nm=(struct _dyneither_ptr*)((_tmp89E=_cycalloc(sizeof(*_tmp89E)),((
_tmp89E[0]=((_tmp8A1.tag=0,((_tmp8A1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp3E),((_tmp8A0[0]=& _tmp8A1,Cyc_aprintf(((_tmp89F="`%s",
_tag_dyneither(_tmp89F,sizeof(char),4))),_tag_dyneither(_tmp8A0,sizeof(void*),1)))))))),
_tmp89E))));{struct Cyc_Absyn_VarType_struct*_tmp8B6;struct Cyc_Absyn_Tvar*_tmp8B5;
struct Cyc_Absyn_Eq_kb_struct _tmp8B4;struct Cyc_Absyn_Eq_kb_struct*_tmp8B3;struct
Cyc_Absyn_VarType_struct _tmp8B2;struct Cyc_Core_Opt*_tmp8B1;*_tmp44=((_tmp8B1=
_cycalloc(sizeof(*_tmp8B1)),((_tmp8B1->v=(void*)((_tmp8B6=_cycalloc(sizeof(*
_tmp8B6)),((_tmp8B6[0]=((_tmp8B2.tag=2,((_tmp8B2.f1=((_tmp8B5=_cycalloc(sizeof(*
_tmp8B5)),((_tmp8B5->name=nm,((_tmp8B5->identity=- 1,((_tmp8B5->kind=(void*)((
_tmp8B3=_cycalloc_atomic(sizeof(*_tmp8B3)),((_tmp8B3[0]=((_tmp8B4.tag=0,((
_tmp8B4.f1=Cyc_Absyn_RgnKind,_tmp8B4)))),_tmp8B3)))),_tmp8B5)))))))),_tmp8B2)))),
_tmp8B6)))),_tmp8B1))));}goto _LL16;}_LL1B:;_LL1C: goto _LL16;_LL16:;}return _tmp32;}
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x);
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp5F=0;for(0;x != 0;x=x->tl){void*_tmp60=((struct Cyc_Absyn_Aggrfield*)
x->hd)->type;void*_tmp62;_LL23: {struct Cyc_Absyn_TagType_struct*_tmp61=(struct
Cyc_Absyn_TagType_struct*)_tmp60;if(_tmp61->tag != 20)goto _LL25;else{_tmp62=(void*)
_tmp61->f1;}}_LL24:{struct _tuple16*_tmp8B9;struct Cyc_List_List*_tmp8B8;_tmp5F=((
_tmp8B8=_cycalloc(sizeof(*_tmp8B8)),((_tmp8B8->hd=((_tmp8B9=_cycalloc(sizeof(*
_tmp8B9)),((_tmp8B9->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8B9->f2=
_tmp62,_tmp8B9)))))),((_tmp8B8->tl=_tmp5F,_tmp8B8))))));}goto _LL22;_LL25:;_LL26:
goto _LL22;_LL22:;}return _tmp5F;}static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(
struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(
struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){{void*_tmp65=e->r;struct _tuple1*
_tmp67;struct _tuple1 _tmp68;union Cyc_Absyn_Nmspace _tmp69;struct Cyc_List_List*
_tmp6A;struct _dyneither_ptr*_tmp6B;struct _tuple1*_tmp6D;struct _tuple1 _tmp6E;
union Cyc_Absyn_Nmspace _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr*
_tmp71;_LL28: {struct Cyc_Absyn_Var_e_struct*_tmp66=(struct Cyc_Absyn_Var_e_struct*)
_tmp65;if(_tmp66->tag != 1)goto _LL2A;else{_tmp67=_tmp66->f1;_tmp68=*_tmp67;_tmp69=
_tmp68.f1;if((_tmp69.Rel_n).tag != 1)goto _LL2A;_tmp6A=(struct Cyc_List_List*)(
_tmp69.Rel_n).val;if(_tmp6A != 0)goto _LL2A;_tmp6B=_tmp68.f2;}}_LL29: _tmp71=_tmp6B;
goto _LL2B;_LL2A: {struct Cyc_Absyn_UnknownId_e_struct*_tmp6C=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp65;if(_tmp6C->tag != 2)goto _LL2C;else{_tmp6D=_tmp6C->f1;_tmp6E=*_tmp6D;_tmp6F=
_tmp6E.f1;if((_tmp6F.Rel_n).tag != 1)goto _LL2C;_tmp70=(struct Cyc_List_List*)(
_tmp6F.Rel_n).val;if(_tmp70 != 0)goto _LL2C;_tmp71=_tmp6E.f2;}}_LL2B:{struct Cyc_List_List*
_tmp72=tags;for(0;_tmp72 != 0;_tmp72=_tmp72->tl){struct _tuple16 _tmp74;struct
_dyneither_ptr*_tmp75;void*_tmp76;struct _tuple16*_tmp73=(struct _tuple16*)_tmp72->hd;
_tmp74=*_tmp73;_tmp75=_tmp74.f1;_tmp76=_tmp74.f2;if(Cyc_strptrcmp(_tmp75,_tmp71)
== 0){struct Cyc_Absyn_Valueof_e_struct _tmp8BC;struct Cyc_Absyn_Valueof_e_struct*
_tmp8BB;return Cyc_Absyn_new_exp((void*)((_tmp8BB=_cycalloc(sizeof(*_tmp8BB)),((
_tmp8BB[0]=((_tmp8BC.tag=40,((_tmp8BC.f1=(void*)Cyc_Tcutil_copy_type(_tmp76),
_tmp8BC)))),_tmp8BB)))),e->loc);}}}goto _LL27;_LL2C:;_LL2D: goto _LL27;_LL27:;}
return e;}static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t);
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){{void*
_tmp79=t;struct Cyc_Absyn_ArrayInfo _tmp7B;void*_tmp7C;struct Cyc_Absyn_Tqual _tmp7D;
struct Cyc_Absyn_Exp*_tmp7E;union Cyc_Absyn_Constraint*_tmp7F;struct Cyc_Position_Segment*
_tmp80;struct Cyc_Absyn_PtrInfo _tmp82;void*_tmp83;struct Cyc_Absyn_Tqual _tmp84;
struct Cyc_Absyn_PtrAtts _tmp85;void*_tmp86;union Cyc_Absyn_Constraint*_tmp87;union
Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*_tmp89;struct Cyc_Absyn_PtrLoc*
_tmp8A;_LL2F: {struct Cyc_Absyn_ArrayType_struct*_tmp7A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp79;if(_tmp7A->tag != 9)goto _LL31;else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.elt_type;
_tmp7D=_tmp7B.tq;_tmp7E=_tmp7B.num_elts;_tmp7F=_tmp7B.zero_term;_tmp80=_tmp7B.zt_loc;}}
_LL30: {struct Cyc_Absyn_Exp*nelts2=_tmp7E;if(_tmp7E != 0)nelts2=(struct Cyc_Absyn_Exp*)
Cyc_Parse_substitute_tags_exp(tags,(struct Cyc_Absyn_Exp*)_tmp7E);{void*_tmp8B=
Cyc_Parse_substitute_tags(tags,_tmp7C);if(_tmp7E != nelts2  || _tmp7C != _tmp8B){
struct Cyc_Absyn_ArrayType_struct _tmp8C2;struct Cyc_Absyn_ArrayInfo _tmp8C1;struct
Cyc_Absyn_ArrayType_struct*_tmp8C0;return(void*)((_tmp8C0=_cycalloc(sizeof(*
_tmp8C0)),((_tmp8C0[0]=((_tmp8C2.tag=9,((_tmp8C2.f1=((_tmp8C1.elt_type=_tmp8B,((
_tmp8C1.tq=_tmp7D,((_tmp8C1.num_elts=nelts2,((_tmp8C1.zero_term=_tmp7F,((_tmp8C1.zt_loc=
_tmp80,_tmp8C1)))))))))),_tmp8C2)))),_tmp8C0))));}goto _LL2E;}}_LL31: {struct Cyc_Absyn_PointerType_struct*
_tmp81=(struct Cyc_Absyn_PointerType_struct*)_tmp79;if(_tmp81->tag != 5)goto _LL33;
else{_tmp82=_tmp81->f1;_tmp83=_tmp82.elt_typ;_tmp84=_tmp82.elt_tq;_tmp85=_tmp82.ptr_atts;
_tmp86=_tmp85.rgn;_tmp87=_tmp85.nullable;_tmp88=_tmp85.bounds;_tmp89=_tmp85.zero_term;
_tmp8A=_tmp85.ptrloc;}}_LL32: {void*_tmp8F=Cyc_Parse_substitute_tags(tags,_tmp83);
union Cyc_Absyn_Constraint*_tmp90=_tmp88;{union Cyc_Absyn_Constraint _tmp91=*_tmp88;
void*_tmp92;struct Cyc_Absyn_Exp*_tmp94;_LL36: if((_tmp91.Eq_constr).tag != 1)goto
_LL38;_tmp92=(void*)(_tmp91.Eq_constr).val;{struct Cyc_Absyn_Upper_b_struct*
_tmp93=(struct Cyc_Absyn_Upper_b_struct*)_tmp92;if(_tmp93->tag != 1)goto _LL38;
else{_tmp94=_tmp93->f1;}}_LL37: _tmp94=Cyc_Parse_substitute_tags_exp(tags,_tmp94);{
struct Cyc_Absyn_Upper_b_struct*_tmp8C8;struct Cyc_Absyn_Upper_b_struct _tmp8C7;
union Cyc_Absyn_Constraint*_tmp8C6;_tmp90=((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),(((
_tmp8C6->Eq_constr).val=(void*)((_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((
_tmp8C7.tag=1,((_tmp8C7.f1=_tmp94,_tmp8C7)))),_tmp8C8)))),(((_tmp8C6->Eq_constr).tag=
1,_tmp8C6))))));}goto _LL35;_LL38:;_LL39: goto _LL35;_LL35:;}if(_tmp8F != _tmp83  || 
_tmp90 != _tmp88){struct Cyc_Absyn_PointerType_struct _tmp8D2;struct Cyc_Absyn_PtrAtts
_tmp8D1;struct Cyc_Absyn_PtrInfo _tmp8D0;struct Cyc_Absyn_PointerType_struct*
_tmp8CF;return(void*)((_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF[0]=((
_tmp8D2.tag=5,((_tmp8D2.f1=((_tmp8D0.elt_typ=_tmp8F,((_tmp8D0.elt_tq=_tmp84,((
_tmp8D0.ptr_atts=((_tmp8D1.rgn=_tmp86,((_tmp8D1.nullable=_tmp87,((_tmp8D1.bounds=
_tmp90,((_tmp8D1.zero_term=_tmp89,((_tmp8D1.ptrloc=_tmp8A,_tmp8D1)))))))))),
_tmp8D0)))))),_tmp8D2)))),_tmp8CF))));}goto _LL2E;}_LL33:;_LL34: goto _LL2E;_LL2E:;}
return t;}static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,
struct Cyc_Absyn_Aggrfield*x);static void Cyc_Parse_substitute_aggrfield_tags(
struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(
tags,x->type);}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static struct
_tuple17*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple8*t);
static struct _tuple17*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,
struct _tuple8*t){struct _tuple17*_tmp8D3;return(_tmp8D3=_cycalloc(sizeof(*_tmp8D3)),((
_tmp8D3->f1=(*t).f2,((_tmp8D3->f2=(*t).f3,_tmp8D3)))));}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d);static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp9D=d->r;struct Cyc_Absyn_Vardecl*_tmp9F;_LL3B: {
struct Cyc_Absyn_Var_d_struct*_tmp9E=(struct Cyc_Absyn_Var_d_struct*)_tmp9D;if(
_tmp9E->tag != 0)goto _LL3D;else{_tmp9F=_tmp9E->f1;}}_LL3C: return _tmp9F;_LL3D:;
_LL3E: {const char*_tmp8D6;void*_tmp8D5;(_tmp8D5=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((
_tmp8D6="bad declaration in `forarray' statement",_tag_dyneither(_tmp8D6,sizeof(
char),40))),_tag_dyneither(_tmp8D5,sizeof(void*),0)));}_LL3A:;}static int Cyc_Parse_is_typeparam(
void*tm);static int Cyc_Parse_is_typeparam(void*tm){void*_tmpA2=tm;_LL40: {struct
Cyc_Absyn_TypeParams_mod_struct*_tmpA3=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpA2;if(_tmpA3->tag != 4)goto _LL42;}_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k);static void*Cyc_Parse_id2type(
struct _dyneither_ptr s,void*k){const char*_tmp8D7;if(Cyc_zstrcmp((struct
_dyneither_ptr)s,((_tmp8D7="`H",_tag_dyneither(_tmp8D7,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{const char*_tmp8D8;if(Cyc_zstrcmp((
struct _dyneither_ptr)s,((_tmp8D8="`U",_tag_dyneither(_tmp8D8,sizeof(char),3))))
== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;else{struct Cyc_Absyn_VarType_struct
_tmp8E2;struct _dyneither_ptr*_tmp8E1;struct Cyc_Absyn_Tvar*_tmp8E0;struct Cyc_Absyn_VarType_struct*
_tmp8DF;return(void*)((_tmp8DF=_cycalloc(sizeof(*_tmp8DF)),((_tmp8DF[0]=((
_tmp8E2.tag=2,((_tmp8E2.f1=((_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0->name=((
_tmp8E1=_cycalloc(sizeof(*_tmp8E1)),((_tmp8E1[0]=s,_tmp8E1)))),((_tmp8E0->identity=
- 1,((_tmp8E0->kind=k,_tmp8E0)))))))),_tmp8E2)))),_tmp8DF))));}}}static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t){void*k;{void*_tmpAA=Cyc_Absyn_compress_kb(t->kind);enum 
Cyc_Absyn_Kind _tmpAD;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*_tmpAB=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpAA;if(_tmpAB->tag != 1)goto _LL47;}_LL46:{struct Cyc_Absyn_Unknown_kb_struct
_tmp8E5;struct Cyc_Absyn_Unknown_kb_struct*_tmp8E4;k=(void*)((_tmp8E4=_cycalloc(
sizeof(*_tmp8E4)),((_tmp8E4[0]=((_tmp8E5.tag=1,((_tmp8E5.f1=0,_tmp8E5)))),
_tmp8E4))));}goto _LL44;_LL47: {struct Cyc_Absyn_Less_kb_struct*_tmpAC=(struct Cyc_Absyn_Less_kb_struct*)
_tmpAA;if(_tmpAC->tag != 2)goto _LL49;else{_tmpAD=_tmpAC->f2;}}_LL48:{struct Cyc_Absyn_Less_kb_struct
_tmp8E8;struct Cyc_Absyn_Less_kb_struct*_tmp8E7;k=(void*)((_tmp8E7=_cycalloc(
sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E8.tag=2,((_tmp8E8.f1=0,((_tmp8E8.f2=
_tmpAD,_tmp8E8)))))),_tmp8E7))));}goto _LL44;_LL49:;_LL4A: k=_tmpAA;goto _LL44;
_LL44:;}{struct Cyc_Absyn_Tvar*_tmp8E9;return(_tmp8E9=_cycalloc(sizeof(*_tmp8E9)),((
_tmp8E9->name=t->name,((_tmp8E9->identity=- 1,((_tmp8E9->kind=k,_tmp8E9)))))));}}
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,
void*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmpB3=t;struct Cyc_Absyn_Tvar*_tmpB5;_LL4C: {struct Cyc_Absyn_VarType_struct*
_tmpB4=(struct Cyc_Absyn_VarType_struct*)_tmpB3;if(_tmpB4->tag != 2)goto _LL4E;
else{_tmpB5=_tmpB4->f1;}}_LL4D: return _tmpB5;_LL4E:;_LL4F: {const char*_tmp8EC;
void*_tmp8EB;(_tmp8EB=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp8EC="expecting a list of type variables, not types",
_tag_dyneither(_tmp8EC,sizeof(char),46))),_tag_dyneither(_tmp8EB,sizeof(void*),0)));}
_LL4B:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr);static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct _tmp8EF;struct Cyc_Absyn_VarType_struct*
_tmp8EE;return(void*)((_tmp8EE=_cycalloc(sizeof(*_tmp8EE)),((_tmp8EE[0]=((
_tmp8EF.tag=2,((_tmp8EF.f1=pr,_tmp8EF)))),_tmp8EE))));}static void Cyc_Parse_set_vartyp_kind(
void*t,enum Cyc_Absyn_Kind k,int leq);static void Cyc_Parse_set_vartyp_kind(void*t,
enum Cyc_Absyn_Kind k,int leq){void*_tmpBA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmpBC;struct Cyc_Absyn_Tvar _tmpBD;void*_tmpBE;void**_tmpBF;_LL51: {struct Cyc_Absyn_VarType_struct*
_tmpBB=(struct Cyc_Absyn_VarType_struct*)_tmpBA;if(_tmpBB->tag != 2)goto _LL53;
else{_tmpBC=_tmpBB->f1;_tmpBD=*_tmpBC;_tmpBE=_tmpBD.kind;_tmpBF=(void**)&(*
_tmpBB->f1).kind;}}_LL52: {void*_tmpC0=Cyc_Absyn_compress_kb(*_tmpBF);_LL56: {
struct Cyc_Absyn_Unknown_kb_struct*_tmpC1=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpC0;if(_tmpC1->tag != 1)goto _LL58;}_LL57: if(!leq)*_tmpBF=Cyc_Tcutil_kind_to_bound(
k);else{struct Cyc_Absyn_Less_kb_struct _tmp8F2;struct Cyc_Absyn_Less_kb_struct*
_tmp8F1;*_tmpBF=(void*)((_tmp8F1=_cycalloc(sizeof(*_tmp8F1)),((_tmp8F1[0]=((
_tmp8F2.tag=2,((_tmp8F2.f1=0,((_tmp8F2.f2=k,_tmp8F2)))))),_tmp8F1))));}return;
_LL58:;_LL59: return;_LL55:;}_LL53:;_LL54: return;_LL50:;}static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*tms,struct Cyc_List_List*tds,
struct Cyc_Position_Segment*loc);static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmpC4=(void*)tms->hd;void*_tmpC6;
_LL5B: {struct Cyc_Absyn_Function_mod_struct*_tmpC5=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC4;if(_tmpC5->tag != 3)goto _LL5D;else{_tmpC6=(void*)_tmpC5->f1;}}_LL5C: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmpC7=_tmpC6;
struct Cyc_List_List*_tmpCA;_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpC8=(
struct Cyc_Absyn_WithTypes_struct*)_tmpC7;if(_tmpC8->tag != 1)goto _LL62;}_LL61:{
const char*_tmp8F5;void*_tmp8F4;(_tmp8F4=0,Cyc_Tcutil_warn(loc,((_tmp8F5="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp8F5,sizeof(char),93))),_tag_dyneither(_tmp8F4,sizeof(void*),0)));}
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpC9=(struct Cyc_Absyn_NoTypes_struct*)
_tmpC7;if(_tmpC9->tag != 0)goto _LL5F;else{_tmpCA=_tmpC9->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpCA)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp8F8;void*_tmp8F7;(_tmp8F7=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
loc,((_tmp8F8="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp8F8,sizeof(char),73))),_tag_dyneither(_tmp8F7,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpCF=0;for(0;_tmpCA != 0;_tmpCA=_tmpCA->tl){struct Cyc_List_List*
_tmpD0=tds;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){struct Cyc_Absyn_Decl*_tmpD1=(
struct Cyc_Absyn_Decl*)_tmpD0->hd;void*_tmpD2=_tmpD1->r;struct Cyc_Absyn_Vardecl*
_tmpD4;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpD3=(struct Cyc_Absyn_Var_d_struct*)
_tmpD2;if(_tmpD3->tag != 0)goto _LL67;else{_tmpD4=_tmpD3->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpD4->name).f2,(struct _dyneither_ptr*)_tmpCA->hd)!= 0)continue;if(_tmpD4->initializer
!= 0){const char*_tmp8FB;void*_tmp8FA;(_tmp8FA=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((
_tmp8FB="initializer found in parameter declaration",_tag_dyneither(_tmp8FB,
sizeof(char),43))),_tag_dyneither(_tmp8FA,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmpD4->name)){const char*_tmp8FE;void*_tmp8FD;(_tmp8FD=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((
_tmp8FE="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8FE,
sizeof(char),47))),_tag_dyneither(_tmp8FD,sizeof(void*),0)));}{struct _tuple8*
_tmp904;struct Cyc_Core_Opt*_tmp903;struct Cyc_List_List*_tmp902;_tmpCF=((_tmp902=
_cycalloc(sizeof(*_tmp902)),((_tmp902->hd=((_tmp904=_cycalloc(sizeof(*_tmp904)),((
_tmp904->f1=((_tmp903=_cycalloc(sizeof(*_tmp903)),((_tmp903->v=(*_tmpD4->name).f2,
_tmp903)))),((_tmp904->f2=_tmpD4->tq,((_tmp904->f3=_tmpD4->type,_tmp904)))))))),((
_tmp902->tl=_tmpCF,_tmp902))))));}goto L;_LL67:;_LL68: {const char*_tmp907;void*
_tmp906;(_tmp906=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,((_tmp907="nonvariable declaration in parameter type",
_tag_dyneither(_tmp907,sizeof(char),42))),_tag_dyneither(_tmp906,sizeof(void*),0)));}
_LL64:;}L: if(_tmpD0 == 0){const char*_tmp90B;void*_tmp90A[1];struct Cyc_String_pa_struct
_tmp909;(_tmp909.tag=0,((_tmp909.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmpCA->hd)),((_tmp90A[0]=& _tmp909,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp90B="%s is not given a type",_tag_dyneither(_tmp90B,sizeof(char),23))),
_tag_dyneither(_tmp90A,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp91A;struct Cyc_Absyn_WithTypes_struct*_tmp919;struct Cyc_Absyn_WithTypes_struct
_tmp918;struct Cyc_Absyn_Function_mod_struct _tmp917;struct Cyc_List_List*_tmp916;
return(_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916->hd=(void*)((_tmp91A=
_cycalloc(sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp917.tag=3,((_tmp917.f1=(void*)((
void*)((_tmp919=_cycalloc(sizeof(*_tmp919)),((_tmp919[0]=((_tmp918.tag=1,((
_tmp918.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpCF),((_tmp918.f2=0,((_tmp918.f3=0,((_tmp918.f4=0,((_tmp918.f5=0,_tmp918)))))))))))),
_tmp919))))),_tmp917)))),_tmp91A)))),((_tmp916->tl=0,_tmp916)))));}}_LL5F:;}goto
_LL5E;_LL5D:;_LL5E: {struct Cyc_List_List*_tmp91B;return(_tmp91B=_cycalloc(
sizeof(*_tmp91B)),((_tmp91B->hd=(void*)tms->hd,((_tmp91B->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp91B)))));}_LL5A:;}}struct _tuple18{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc);static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0){struct Cyc_Parse_Declarator*_tmp91C;d=((_tmp91C=_cycalloc(sizeof(*
_tmp91C)),((_tmp91C->id=d->id,((_tmp91C->tms=Cyc_Parse_oldstyle2newstyle(d->tms,
tds,loc),_tmp91C))))));}{enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;struct Cyc_List_List*
tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*
atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;
tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)
dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;
if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)switch((enum Cyc_Parse_Storage_class)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){
case Cyc_Parse_Extern_sc: _LL69: sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc:
_LL6A: sc=Cyc_Absyn_Static;break;default: _LL6B:{const char*_tmp91D;Cyc_Parse_err(((
_tmp91D="bad storage class on function",_tag_dyneither(_tmp91D,sizeof(char),30))),
loc);}break;}}{void*_tmpEA;struct Cyc_Core_Opt*_tmpEB;struct _tuple11 _tmpE9=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpEA=_tmpE9.f1;_tmpEB=_tmpE9.f2;{struct Cyc_Absyn_Tqual _tmpED;void*
_tmpEE;struct Cyc_List_List*_tmpEF;struct Cyc_List_List*_tmpF0;struct _tuple12
_tmpEC=Cyc_Parse_apply_tms(tq,_tmpEA,atts,d->tms);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;
_tmpEF=_tmpEC.f3;_tmpF0=_tmpEC.f4;if(_tmpEB != 0){const char*_tmp920;void*_tmp91F;(
_tmp91F=0,Cyc_Tcutil_warn(loc,((_tmp920="nested type declaration within function prototype",
_tag_dyneither(_tmp920,sizeof(char),50))),_tag_dyneither(_tmp91F,sizeof(void*),0)));}
if(_tmpEF != 0){const char*_tmp923;void*_tmp922;(_tmp922=0,Cyc_Tcutil_warn(loc,((
_tmp923="bad type params, ignoring",_tag_dyneither(_tmp923,sizeof(char),26))),
_tag_dyneither(_tmp922,sizeof(void*),0)));}{void*_tmpF5=_tmpEE;struct Cyc_Absyn_FnInfo
_tmpF7;struct Cyc_List_List*_tmpF8;struct Cyc_Core_Opt*_tmpF9;void*_tmpFA;struct
Cyc_List_List*_tmpFB;int _tmpFC;struct Cyc_Absyn_VarargInfo*_tmpFD;struct Cyc_List_List*
_tmpFE;struct Cyc_List_List*_tmpFF;_LL6E: {struct Cyc_Absyn_FnType_struct*_tmpF6=(
struct Cyc_Absyn_FnType_struct*)_tmpF5;if(_tmpF6->tag != 10)goto _LL70;else{_tmpF7=
_tmpF6->f1;_tmpF8=_tmpF7.tvars;_tmpF9=_tmpF7.effect;_tmpFA=_tmpF7.ret_typ;_tmpFB=
_tmpF7.args;_tmpFC=_tmpF7.c_varargs;_tmpFD=_tmpF7.cyc_varargs;_tmpFE=_tmpF7.rgn_po;
_tmpFF=_tmpF7.attributes;}}_LL6F: {struct Cyc_List_List*_tmp100=0;{struct Cyc_List_List*
_tmp101=_tmpFB;for(0;_tmp101 != 0;_tmp101=_tmp101->tl){struct _tuple8 _tmp103;
struct Cyc_Core_Opt*_tmp104;struct Cyc_Absyn_Tqual _tmp105;void*_tmp106;struct
_tuple8*_tmp102=(struct _tuple8*)_tmp101->hd;_tmp103=*_tmp102;_tmp104=_tmp103.f1;
_tmp105=_tmp103.f2;_tmp106=_tmp103.f3;if(_tmp104 == 0){{const char*_tmp924;Cyc_Parse_err(((
_tmp924="missing argument variable in function prototype",_tag_dyneither(_tmp924,
sizeof(char),48))),loc);}{struct _tuple18*_tmp927;struct Cyc_List_List*_tmp926;
_tmp100=((_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926->hd=((_tmp927=_cycalloc(
sizeof(*_tmp927)),((_tmp927->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"?",sizeof(char),2),((_tmp927->f2=_tmp105,((_tmp927->f3=_tmp106,
_tmp927)))))))),((_tmp926->tl=_tmp100,_tmp926))))));}}else{struct _tuple18*
_tmp92A;struct Cyc_List_List*_tmp929;_tmp100=((_tmp929=_cycalloc(sizeof(*_tmp929)),((
_tmp929->hd=((_tmp92A=_cycalloc(sizeof(*_tmp92A)),((_tmp92A->f1=(struct
_dyneither_ptr*)_tmp104->v,((_tmp92A->f2=_tmp105,((_tmp92A->f3=_tmp106,_tmp92A)))))))),((
_tmp929->tl=_tmp100,_tmp929))))));}}}{struct Cyc_Absyn_Fndecl*_tmp92B;return(
_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B->sc=sc,((_tmp92B->name=d->id,((
_tmp92B->tvs=_tmpF8,((_tmp92B->is_inline=is_inline,((_tmp92B->effect=_tmpF9,((
_tmp92B->ret_type=_tmpFA,((_tmp92B->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp100),((_tmp92B->c_varargs=_tmpFC,((_tmp92B->cyc_varargs=
_tmpFD,((_tmp92B->rgn_po=_tmpFE,((_tmp92B->body=body,((_tmp92B->cached_typ=0,((
_tmp92B->param_vardecls=0,((_tmp92B->fn_vardecl=0,((_tmp92B->attributes=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmpFF,_tmpF0),_tmp92B)))))))))))))))))))))))))))))));}}_LL70:;_LL71: {const char*
_tmp92E;void*_tmp92D;(_tmp92D=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp92E="declarator is not a function prototype",
_tag_dyneither(_tmp92E,sizeof(char),39))),_tag_dyneither(_tmp92D,sizeof(void*),0)));}
_LL6D:;}}}}}static char _tmp110[52]="at most one type may appear within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg1={_tmp110,_tmp110,_tmp110 + 52};static
char _tmp111[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp111,_tmp111,_tmp111 + 63};static
char _tmp112[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp112,_tmp112,_tmp112 + 50};static char _tmp113[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp113,_tmp113,_tmp113 + 60};static
struct _tuple11 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct
Cyc_Position_Segment*loc);static struct _tuple11 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*
declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp114=(
void*)ts->hd;void*_tmp116;struct Cyc_Position_Segment*_tmp117;struct Cyc_Position_Segment*
_tmp119;struct Cyc_Position_Segment*_tmp11B;struct Cyc_Position_Segment*_tmp11D;
struct Cyc_Position_Segment*_tmp11F;struct Cyc_Absyn_Decl*_tmp121;_LL73: {struct
Cyc_Parse_Type_spec_struct*_tmp115=(struct Cyc_Parse_Type_spec_struct*)_tmp114;
if(_tmp115->tag != 4)goto _LL75;else{_tmp116=(void*)_tmp115->f1;_tmp117=_tmp115->f2;}}
_LL74: if(seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp117);last_loc=_tmp117;
seen_type=1;t=_tmp116;goto _LL72;_LL75: {struct Cyc_Parse_Signed_spec_struct*
_tmp118=(struct Cyc_Parse_Signed_spec_struct*)_tmp114;if(_tmp118->tag != 0)goto
_LL77;else{_tmp119=_tmp118->f1;}}_LL76: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,
_tmp119);if(seen_type){const char*_tmp92F;Cyc_Parse_err(((_tmp92F="signed qualifier must come before type",
_tag_dyneither(_tmp92F,sizeof(char),39))),_tmp119);}last_loc=_tmp119;seen_sign=1;
sgn=Cyc_Absyn_Signed;goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*
_tmp11A=(struct Cyc_Parse_Unsigned_spec_struct*)_tmp114;if(_tmp11A->tag != 1)goto
_LL79;else{_tmp11B=_tmp11A->f1;}}_LL78: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,
_tmp11B);if(seen_type){const char*_tmp930;Cyc_Parse_err(((_tmp930="signed qualifier must come before type",
_tag_dyneither(_tmp930,sizeof(char),39))),_tmp11B);}last_loc=_tmp11B;seen_sign=1;
sgn=Cyc_Absyn_Unsigned;goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*
_tmp11C=(struct Cyc_Parse_Short_spec_struct*)_tmp114;if(_tmp11C->tag != 2)goto
_LL7B;else{_tmp11D=_tmp11C->f1;}}_LL7A: if(seen_size){const char*_tmp931;Cyc_Parse_err(((
_tmp931="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp931,sizeof(char),59))),_tmp11D);}if(seen_type){const char*
_tmp932;Cyc_Parse_err(((_tmp932="short modifier must come before base type",
_tag_dyneither(_tmp932,sizeof(char),42))),_tmp11D);}last_loc=_tmp11D;seen_size=1;
sz=Cyc_Absyn_Short_sz;goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp11E=(
struct Cyc_Parse_Long_spec_struct*)_tmp114;if(_tmp11E->tag != 3)goto _LL7D;else{
_tmp11F=_tmp11E->f1;}}_LL7C: if(seen_type){const char*_tmp933;Cyc_Parse_err(((
_tmp933="long modifier must come before base type",_tag_dyneither(_tmp933,
sizeof(char),41))),_tmp11F);}if(seen_size)switch(sz){case Cyc_Absyn_Long_sz: _LL7F:
sz=Cyc_Absyn_LongLong_sz;break;default: _LL80:{const char*_tmp934;Cyc_Parse_err(((
_tmp934="extra long in type specifier",_tag_dyneither(_tmp934,sizeof(char),29))),
_tmp11F);}break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp11F;seen_size=1;goto
_LL72;_LL7D: {struct Cyc_Parse_Decl_spec_struct*_tmp120=(struct Cyc_Parse_Decl_spec_struct*)
_tmp114;if(_tmp120->tag != 5)goto _LL72;else{_tmp121=_tmp120->f1;}}_LL7E: last_loc=
_tmp121->loc;if(declopt == 0  && !seen_type){seen_type=1;{void*_tmp128=_tmp121->r;
struct Cyc_Absyn_Aggrdecl*_tmp12A;struct Cyc_Absyn_Datatypedecl*_tmp12C;struct Cyc_Absyn_Enumdecl*
_tmp12E;_LL83: {struct Cyc_Absyn_Aggr_d_struct*_tmp129=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp128;if(_tmp129->tag != 6)goto _LL85;else{_tmp12A=_tmp129->f1;}}_LL84: {struct
Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp12A->tvs));{struct Cyc_Absyn_AggrType_struct _tmp93A;
struct Cyc_Absyn_AggrInfo _tmp939;struct Cyc_Absyn_AggrType_struct*_tmp938;t=(void*)((
_tmp938=_cycalloc(sizeof(*_tmp938)),((_tmp938[0]=((_tmp93A.tag=12,((_tmp93A.f1=((
_tmp939.aggr_info=Cyc_Absyn_UnknownAggr(_tmp12A->kind,_tmp12A->name,0),((_tmp939.targs=
_tmp12F,_tmp939)))),_tmp93A)))),_tmp938))));}if(_tmp12A->impl != 0){struct Cyc_Core_Opt*
_tmp93B;declopt=((_tmp93B=_cycalloc(sizeof(*_tmp93B)),((_tmp93B->v=_tmp121,
_tmp93B))));}goto _LL82;}_LL85: {struct Cyc_Absyn_Datatype_d_struct*_tmp12B=(
struct Cyc_Absyn_Datatype_d_struct*)_tmp128;if(_tmp12B->tag != 7)goto _LL87;else{
_tmp12C=_tmp12B->f1;}}_LL86: {struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp12C->tvs));{struct Cyc_Absyn_DatatypeType_struct
_tmp945;struct Cyc_Absyn_Datatypedecl**_tmp944;struct Cyc_Absyn_DatatypeInfo
_tmp943;struct Cyc_Absyn_DatatypeType_struct*_tmp942;t=(void*)((_tmp942=_cycalloc(
sizeof(*_tmp942)),((_tmp942[0]=((_tmp945.tag=3,((_tmp945.f1=((_tmp943.datatype_info=
Cyc_Absyn_KnownDatatype(((_tmp944=_cycalloc(sizeof(*_tmp944)),((_tmp944[0]=
_tmp12C,_tmp944))))),((_tmp943.targs=_tmp134,_tmp943)))),_tmp945)))),_tmp942))));}
if(_tmp12C->fields != 0){struct Cyc_Core_Opt*_tmp946;declopt=((_tmp946=_cycalloc(
sizeof(*_tmp946)),((_tmp946->v=_tmp121,_tmp946))));}goto _LL82;}_LL87: {struct Cyc_Absyn_Enum_d_struct*
_tmp12D=(struct Cyc_Absyn_Enum_d_struct*)_tmp128;if(_tmp12D->tag != 8)goto _LL89;
else{_tmp12E=_tmp12D->f1;}}_LL88:{struct Cyc_Absyn_EnumType_struct _tmp949;struct
Cyc_Absyn_EnumType_struct*_tmp948;t=(void*)((_tmp948=_cycalloc(sizeof(*_tmp948)),((
_tmp948[0]=((_tmp949.tag=14,((_tmp949.f1=_tmp12E->name,((_tmp949.f2=0,_tmp949)))))),
_tmp948))));}{struct Cyc_Core_Opt*_tmp94A;declopt=((_tmp94A=_cycalloc(sizeof(*
_tmp94A)),((_tmp94A->v=_tmp121,_tmp94A))));}goto _LL82;_LL89:;_LL8A: {const char*
_tmp94D;void*_tmp94C;(_tmp94C=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp121->loc,((
_tmp94D="bad declaration within type specifier",_tag_dyneither(_tmp94D,sizeof(
char),38))),_tag_dyneither(_tmp94C,sizeof(void*),0)));}_LL82:;}}else{Cyc_Parse_err(
Cyc_Parse_msg3,_tmp121->loc);}goto _LL72;_LL72:;}if(!seen_type){if(!seen_sign  && 
!seen_size){const char*_tmp950;void*_tmp94F;(_tmp94F=0,Cyc_Tcutil_warn(last_loc,((
_tmp950="missing type within specifier",_tag_dyneither(_tmp950,sizeof(char),30))),
_tag_dyneither(_tmp94F,sizeof(void*),0)));}t=Cyc_Absyn_int_typ(sgn,sz);}else{if(
seen_sign){void*_tmp141=t;enum Cyc_Absyn_Sign _tmp143;enum Cyc_Absyn_Size_of
_tmp144;_LL8C: {struct Cyc_Absyn_IntType_struct*_tmp142=(struct Cyc_Absyn_IntType_struct*)
_tmp141;if(_tmp142->tag != 6)goto _LL8E;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}
_LL8D: if(_tmp143 != sgn)t=Cyc_Absyn_int_typ(sgn,_tmp144);goto _LL8B;_LL8E:;_LL8F:{
const char*_tmp951;Cyc_Parse_err(((_tmp951="sign specification on non-integral type",
_tag_dyneither(_tmp951,sizeof(char),40))),last_loc);}goto _LL8B;_LL8B:;}if(
seen_size){void*_tmp146=t;enum Cyc_Absyn_Sign _tmp148;enum Cyc_Absyn_Size_of
_tmp149;_LL91: {struct Cyc_Absyn_IntType_struct*_tmp147=(struct Cyc_Absyn_IntType_struct*)
_tmp146;if(_tmp147->tag != 6)goto _LL93;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}
_LL92: if(_tmp149 != sz)t=Cyc_Absyn_int_typ(_tmp148,sz);goto _LL90;_LL93: {struct
Cyc_Absyn_DoubleType_struct*_tmp14A=(struct Cyc_Absyn_DoubleType_struct*)_tmp146;
if(_tmp14A->tag != 8)goto _LL95;}_LL94: t=Cyc_Absyn_double_typ(1);goto _LL90;_LL95:;
_LL96:{const char*_tmp952;Cyc_Parse_err(((_tmp952="size qualifier on non-integral type",
_tag_dyneither(_tmp952,sizeof(char),36))),last_loc);}goto _LL90;_LL90:;}}{struct
_tuple11 _tmp953;return(_tmp953.f1=t,((_tmp953.f2=declopt,_tmp953)));}}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts);static struct
Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{
struct Cyc_Parse_Declarator*_tmp14D=(struct Cyc_Parse_Declarator*)ds->hd;struct
_tuple1*_tmp14E=_tmp14D->id;struct Cyc_Absyn_Tqual _tmp150;void*_tmp151;struct Cyc_List_List*
_tmp152;struct Cyc_List_List*_tmp153;struct _tuple12 _tmp14F=Cyc_Parse_apply_tms(tq,
t,shared_atts,_tmp14D->tms);_tmp150=_tmp14F.f1;_tmp151=_tmp14F.f2;_tmp152=
_tmp14F.f3;_tmp153=_tmp14F.f4;if(ds->tl == 0){struct _tuple14*_tmp956;struct Cyc_List_List*
_tmp955;return(_tmp955=_region_malloc(r,sizeof(*_tmp955)),((_tmp955->hd=((
_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956->f1=_tmp14E,((_tmp956->f2=
_tmp150,((_tmp956->f3=_tmp151,((_tmp956->f4=_tmp152,((_tmp956->f5=_tmp153,
_tmp956)))))))))))),((_tmp955->tl=0,_tmp955)))));}else{struct _tuple14*_tmp959;
struct Cyc_List_List*_tmp958;return(_tmp958=_region_malloc(r,sizeof(*_tmp958)),((
_tmp958->hd=((_tmp959=_region_malloc(r,sizeof(*_tmp959)),((_tmp959->f1=_tmp14E,((
_tmp959->f2=_tmp150,((_tmp959->f3=_tmp151,((_tmp959->f4=_tmp152,((_tmp959->f5=
_tmp153,_tmp959)))))))))))),((_tmp958->tl=Cyc_Parse_apply_tmss(r,_tmp150,Cyc_Tcutil_copy_type(
t),ds->tl,shared_atts),_tmp958)))));}}}static struct _tuple12 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms);
static struct _tuple12 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0){struct _tuple12 _tmp95A;return(_tmp95A.f1=
tq,((_tmp95A.f2=t,((_tmp95A.f3=0,((_tmp95A.f4=atts,_tmp95A)))))));}{void*_tmp159=(
void*)tms->hd;union Cyc_Absyn_Constraint*_tmp15B;struct Cyc_Position_Segment*
_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;union Cyc_Absyn_Constraint*_tmp15F;struct Cyc_Position_Segment*
_tmp160;void*_tmp162;struct Cyc_List_List*_tmp164;struct Cyc_Position_Segment*
_tmp165;struct Cyc_Absyn_PtrAtts _tmp167;struct Cyc_Absyn_Tqual _tmp168;struct Cyc_Position_Segment*
_tmp16A;struct Cyc_List_List*_tmp16B;_LL98: {struct Cyc_Absyn_Carray_mod_struct*
_tmp15A=(struct Cyc_Absyn_Carray_mod_struct*)_tmp159;if(_tmp15A->tag != 0)goto
_LL9A;else{_tmp15B=_tmp15A->f1;_tmp15C=_tmp15A->f2;}}_LL99: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmp15B,_tmp15C),atts,tms->tl);
_LL9A: {struct Cyc_Absyn_ConstArray_mod_struct*_tmp15D=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp159;if(_tmp15D->tag != 1)goto _LL9C;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;
_tmp160=_tmp15D->f3;}}_LL9B: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmp15E,_tmp15F,_tmp160),atts,tms->tl);
_LL9C: {struct Cyc_Absyn_Function_mod_struct*_tmp161=(struct Cyc_Absyn_Function_mod_struct*)
_tmp159;if(_tmp161->tag != 3)goto _LL9E;else{_tmp162=(void*)_tmp161->f1;}}_LL9D: {
void*_tmp16C=_tmp162;struct Cyc_List_List*_tmp16E;int _tmp16F;struct Cyc_Absyn_VarargInfo*
_tmp170;struct Cyc_Core_Opt*_tmp171;struct Cyc_List_List*_tmp172;struct Cyc_Position_Segment*
_tmp174;_LLA5: {struct Cyc_Absyn_WithTypes_struct*_tmp16D=(struct Cyc_Absyn_WithTypes_struct*)
_tmp16C;if(_tmp16D->tag != 1)goto _LLA7;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;
_tmp170=_tmp16D->f3;_tmp171=_tmp16D->f4;_tmp172=_tmp16D->f5;}}_LLA6: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp95B;fn_atts=((_tmp95B=_cycalloc(sizeof(*_tmp95B)),((
_tmp95B->hd=(void*)as->hd,((_tmp95B->tl=fn_atts,_tmp95B))))));}else{struct Cyc_List_List*
_tmp95C;new_atts=((_tmp95C=_cycalloc(sizeof(*_tmp95C)),((_tmp95C->hd=(void*)as->hd,((
_tmp95C->tl=new_atts,_tmp95C))))));}}}if(tms->tl != 0){void*_tmp177=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp179;_LLAA: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp178=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp177;if(_tmp178->tag != 4)goto _LLAC;else{_tmp179=_tmp178->f1;}}_LLAB: typvars=
_tmp179;tms=tms->tl;goto _LLA9;_LLAC:;_LLAD: goto _LLA9;_LLA9:;}if(((((!_tmp16F  && 
_tmp170 == 0) && _tmp16E != 0) && _tmp16E->tl == 0) && (*((struct _tuple8*)_tmp16E->hd)).f1
== 0) && (*((struct _tuple8*)_tmp16E->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
_tmp16E=0;{struct Cyc_List_List*_tmp17A=Cyc_Parse_get_arg_tags(_tmp16E);if(
_tmp17A != 0)t=Cyc_Parse_substitute_tags(_tmp17A,t);t=Cyc_Parse_array2ptr(t,0);{
struct Cyc_List_List*_tmp17B=_tmp16E;for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){
struct _tuple8 _tmp17D;struct Cyc_Core_Opt*_tmp17E;struct Cyc_Absyn_Tqual _tmp17F;
void*_tmp180;void**_tmp181;struct _tuple8*_tmp17C=(struct _tuple8*)_tmp17B->hd;
_tmp17D=*_tmp17C;_tmp17E=_tmp17D.f1;_tmp17F=_tmp17D.f2;_tmp180=_tmp17D.f3;
_tmp181=(void**)&(*_tmp17C).f3;if(_tmp17A != 0)*_tmp181=Cyc_Parse_substitute_tags(
_tmp17A,*_tmp181);*_tmp181=Cyc_Parse_array2ptr(*_tmp181,1);}}return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(typvars,_tmp171,t,_tmp16E,
_tmp16F,_tmp170,_tmp172,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}}
_LLA7: {struct Cyc_Absyn_NoTypes_struct*_tmp173=(struct Cyc_Absyn_NoTypes_struct*)
_tmp16C;if(_tmp173->tag != 0)goto _LLA4;else{_tmp174=_tmp173->f2;}}_LLA8: {const
char*_tmp95F;void*_tmp95E;(_tmp95E=0,((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp174,((
_tmp95F="function declaration without parameter types",_tag_dyneither(_tmp95F,
sizeof(char),45))),_tag_dyneither(_tmp95E,sizeof(void*),0)));}_LLA4:;}_LL9E: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp163=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp159;if(_tmp163->tag != 4)goto _LLA0;else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}}
_LL9F: if(tms->tl == 0){struct _tuple12 _tmp960;return(_tmp960.f1=tq,((_tmp960.f2=t,((
_tmp960.f3=_tmp164,((_tmp960.f4=atts,_tmp960)))))));}{const char*_tmp963;void*
_tmp962;(_tmp962=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp165,((_tmp963="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp963,sizeof(char),68))),_tag_dyneither(_tmp962,sizeof(void*),0)));}
_LLA0: {struct Cyc_Absyn_Pointer_mod_struct*_tmp166=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp159;if(_tmp166->tag != 2)goto _LLA2;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}
_LLA1: {struct Cyc_Absyn_PointerType_struct _tmp969;struct Cyc_Absyn_PtrInfo _tmp968;
struct Cyc_Absyn_PointerType_struct*_tmp967;return Cyc_Parse_apply_tms(_tmp168,(
void*)((_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967[0]=((_tmp969.tag=5,((
_tmp969.f1=((_tmp968.elt_typ=t,((_tmp968.elt_tq=tq,((_tmp968.ptr_atts=_tmp167,
_tmp968)))))),_tmp969)))),_tmp967)))),atts,tms->tl);}_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp169=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp159;if(_tmp169->tag != 5)goto
_LL97;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}_LLA3: return Cyc_Parse_apply_tms(
tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
atts,_tmp16B),tms->tl);_LL97:;}}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc);void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc){void*_tmp18B;struct Cyc_Core_Opt*_tmp18C;
struct _tuple11 _tmp18A=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmp18B=
_tmp18A.f1;_tmp18C=_tmp18A.f2;if(_tmp18C != 0){const char*_tmp96C;void*_tmp96B;(
_tmp96B=0,Cyc_Tcutil_warn(loc,((_tmp96C="ignoring nested type declaration(s) in specifier list",
_tag_dyneither(_tmp96C,sizeof(char),54))),_tag_dyneither(_tmp96B,sizeof(void*),0)));}
return _tmp18B;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple14*t);static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple14*t){struct _tuple14 _tmp190;struct
_tuple1*_tmp191;struct Cyc_Absyn_Tqual _tmp192;void*_tmp193;struct Cyc_List_List*
_tmp194;struct Cyc_List_List*_tmp195;struct _tuple14*_tmp18F=t;_tmp190=*_tmp18F;
_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_tmp193=_tmp190.f3;_tmp194=_tmp190.f4;
_tmp195=_tmp190.f5;Cyc_Lex_register_typedef(_tmp191);{struct Cyc_Core_Opt*kind;
struct Cyc_Core_Opt*type;{void*_tmp196=_tmp193;struct Cyc_Core_Opt*_tmp198;_LLAF: {
struct Cyc_Absyn_Evar_struct*_tmp197=(struct Cyc_Absyn_Evar_struct*)_tmp196;if(
_tmp197->tag != 1)goto _LLB1;else{_tmp198=_tmp197->f1;}}_LLB0: type=0;if(_tmp198 == 
0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;else{kind=_tmp198;}goto _LLAE;_LLB1:;
_LLB2: kind=0;{struct Cyc_Core_Opt*_tmp96D;type=((_tmp96D=_cycalloc(sizeof(*
_tmp96D)),((_tmp96D->v=_tmp193,_tmp96D))));}goto _LLAE;_LLAE:;}{struct Cyc_Absyn_Typedef_d_struct
_tmp973;struct Cyc_Absyn_Typedefdecl*_tmp972;struct Cyc_Absyn_Typedef_d_struct*
_tmp971;return Cyc_Absyn_new_decl((void*)((_tmp971=_cycalloc(sizeof(*_tmp971)),((
_tmp971[0]=((_tmp973.tag=9,((_tmp973.f1=((_tmp972=_cycalloc(sizeof(*_tmp972)),((
_tmp972->name=_tmp191,((_tmp972->tvs=_tmp194,((_tmp972->kind=kind,((_tmp972->defn=
type,((_tmp972->atts=_tmp195,((_tmp972->tq=_tmp192,_tmp972)))))))))))))),_tmp973)))),
_tmp971)))),loc);}}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct
_tmp976;struct Cyc_Absyn_Decl_s_struct*_tmp975;return Cyc_Absyn_new_stmt((void*)((
_tmp975=_cycalloc(sizeof(*_tmp975)),((_tmp975[0]=((_tmp976.tag=12,((_tmp976.f1=d,((
_tmp976.f2=s,_tmp976)))))),_tmp975)))),d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc);static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct _RegionHandle _tmp19F=_new_region("mkrgn");struct _RegionHandle*mkrgn=&
_tmp19F;_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp1A1;struct Cyc_Absyn_Tqual
_tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_Parse_Declaration_spec*
_tmp1A0=ds;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.tq;_tmp1A3=_tmp1A1.type_specs;
_tmp1A4=_tmp1A1.attributes;if(_tmp1A2.loc == 0)_tmp1A2.loc=tqual_loc;if(ds->is_inline){
const char*_tmp977;Cyc_Parse_err(((_tmp977="inline is allowed only on function definitions",
_tag_dyneither(_tmp977,sizeof(char),47))),loc);}if(_tmp1A3 == 0){{const char*
_tmp978;Cyc_Parse_err(((_tmp978="missing type specifiers in declaration",
_tag_dyneither(_tmp978,sizeof(char),39))),loc);}{struct Cyc_List_List*_tmp1A7=0;
_npop_handler(0);return _tmp1A7;}}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int
istypedef=0;if(ds->sc != 0)switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v){case Cyc_Parse_Typedef_sc: _LLB3: istypedef=1;break;case
Cyc_Parse_Extern_sc: _LLB4: s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc:
_LLB5: s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LLB6: s=Cyc_Absyn_Static;
break;case Cyc_Parse_Auto_sc: _LLB7: s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc:
_LLB8: if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}
break;case Cyc_Parse_Abstract_sc: _LLB9: s=Cyc_Absyn_Abstract;break;}{struct Cyc_List_List*
_tmp1A9;struct Cyc_List_List*_tmp1AA;struct _tuple0 _tmp1A8=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
mkrgn,mkrgn,ids);_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;{int exps_empty=1;{struct
Cyc_List_List*es=_tmp1AA;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd
!= 0){exps_empty=0;break;}}}{struct _tuple11 _tmp1AB=Cyc_Parse_collapse_type_specifiers(
_tmp1A3,loc);if(_tmp1A9 == 0){void*_tmp1AD;struct Cyc_Core_Opt*_tmp1AE;struct
_tuple11 _tmp1AC=_tmp1AB;_tmp1AD=_tmp1AC.f1;_tmp1AE=_tmp1AC.f2;if(_tmp1AE != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1AE->v;{void*_tmp1AF=d->r;
struct Cyc_Absyn_Enumdecl*_tmp1B1;struct Cyc_Absyn_Aggrdecl*_tmp1B3;struct Cyc_Absyn_Datatypedecl*
_tmp1B5;_LLBC: {struct Cyc_Absyn_Enum_d_struct*_tmp1B0=(struct Cyc_Absyn_Enum_d_struct*)
_tmp1AF;if(_tmp1B0->tag != 8)goto _LLBE;else{_tmp1B1=_tmp1B0->f1;}}_LLBD: _tmp1B1->sc=
s;if(_tmp1A4 != 0){const char*_tmp979;Cyc_Parse_err(((_tmp979="bad attributes on enum",
_tag_dyneither(_tmp979,sizeof(char),23))),loc);}goto _LLBB;_LLBE: {struct Cyc_Absyn_Aggr_d_struct*
_tmp1B2=(struct Cyc_Absyn_Aggr_d_struct*)_tmp1AF;if(_tmp1B2->tag != 6)goto _LLC0;
else{_tmp1B3=_tmp1B2->f1;}}_LLBF: _tmp1B3->sc=s;_tmp1B3->attributes=_tmp1A4;goto
_LLBB;_LLC0: {struct Cyc_Absyn_Datatype_d_struct*_tmp1B4=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1AF;if(_tmp1B4->tag != 7)goto _LLC2;else{_tmp1B5=_tmp1B4->f1;}}_LLC1: _tmp1B5->sc=
s;if(_tmp1A4 != 0){const char*_tmp97A;Cyc_Parse_err(((_tmp97A="bad attributes on datatype",
_tag_dyneither(_tmp97A,sizeof(char),27))),loc);}goto _LLBB;_LLC2:;_LLC3:{const
char*_tmp97B;Cyc_Parse_err(((_tmp97B="bad declaration",_tag_dyneither(_tmp97B,
sizeof(char),16))),loc);}{struct Cyc_List_List*_tmp1B9=0;_npop_handler(0);return
_tmp1B9;}_LLBB:;}{struct Cyc_List_List*_tmp97C;struct Cyc_List_List*_tmp1BB=(
_tmp97C=_cycalloc(sizeof(*_tmp97C)),((_tmp97C->hd=d,((_tmp97C->tl=0,_tmp97C)))));
_npop_handler(0);return _tmp1BB;}}else{void*_tmp1BC=_tmp1AD;struct Cyc_Absyn_AggrInfo
_tmp1BE;union Cyc_Absyn_AggrInfoU _tmp1BF;struct _tuple3 _tmp1C0;enum Cyc_Absyn_AggrKind
_tmp1C1;struct _tuple1*_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_Absyn_DatatypeInfo
_tmp1C5;union Cyc_Absyn_DatatypeInfoU _tmp1C6;struct Cyc_Absyn_Datatypedecl**
_tmp1C7;struct Cyc_Absyn_DatatypeInfo _tmp1C9;union Cyc_Absyn_DatatypeInfoU _tmp1CA;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp1CB;struct _tuple1*_tmp1CC;int _tmp1CD;
struct Cyc_List_List*_tmp1CE;struct _tuple1*_tmp1D0;struct Cyc_List_List*_tmp1D2;
_LLC5: {struct Cyc_Absyn_AggrType_struct*_tmp1BD=(struct Cyc_Absyn_AggrType_struct*)
_tmp1BC;if(_tmp1BD->tag != 12)goto _LLC7;else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BE.aggr_info;
if((_tmp1BF.UnknownAggr).tag != 1)goto _LLC7;_tmp1C0=(struct _tuple3)(_tmp1BF.UnknownAggr).val;
_tmp1C1=_tmp1C0.f1;_tmp1C2=_tmp1C0.f2;_tmp1C3=_tmp1BE.targs;}}_LLC6: {struct Cyc_List_List*
_tmp1D3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1C3);struct Cyc_Absyn_Aggrdecl*_tmp97D;struct Cyc_Absyn_Aggrdecl*_tmp1D4=(
_tmp97D=_cycalloc(sizeof(*_tmp97D)),((_tmp97D->kind=_tmp1C1,((_tmp97D->sc=s,((
_tmp97D->name=_tmp1C2,((_tmp97D->tvs=_tmp1D3,((_tmp97D->impl=0,((_tmp97D->attributes=
0,_tmp97D)))))))))))));if(_tmp1A4 != 0){const char*_tmp97E;Cyc_Parse_err(((_tmp97E="bad attributes on type declaration",
_tag_dyneither(_tmp97E,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp984;struct Cyc_Absyn_Aggr_d_struct _tmp983;struct Cyc_List_List*_tmp982;struct
Cyc_List_List*_tmp1D9=(_tmp982=_cycalloc(sizeof(*_tmp982)),((_tmp982->hd=Cyc_Absyn_new_decl((
void*)((_tmp984=_cycalloc(sizeof(*_tmp984)),((_tmp984[0]=((_tmp983.tag=6,((
_tmp983.f1=_tmp1D4,_tmp983)))),_tmp984)))),loc),((_tmp982->tl=0,_tmp982)))));
_npop_handler(0);return _tmp1D9;}}_LLC7: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1C4=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1BC;if(_tmp1C4->tag != 3)goto
_LLC9;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C5.datatype_info;if((_tmp1C6.KnownDatatype).tag
!= 2)goto _LLC9;_tmp1C7=(struct Cyc_Absyn_Datatypedecl**)(_tmp1C6.KnownDatatype).val;}}
_LLC8: if(_tmp1A4 != 0){const char*_tmp985;Cyc_Parse_err(((_tmp985="bad attributes on datatype",
_tag_dyneither(_tmp985,sizeof(char),27))),loc);}{struct Cyc_Absyn_Datatype_d_struct*
_tmp98B;struct Cyc_Absyn_Datatype_d_struct _tmp98A;struct Cyc_List_List*_tmp989;
struct Cyc_List_List*_tmp1DF=(_tmp989=_cycalloc(sizeof(*_tmp989)),((_tmp989->hd=
Cyc_Absyn_new_decl((void*)((_tmp98B=_cycalloc(sizeof(*_tmp98B)),((_tmp98B[0]=((
_tmp98A.tag=7,((_tmp98A.f1=*_tmp1C7,_tmp98A)))),_tmp98B)))),loc),((_tmp989->tl=0,
_tmp989)))));_npop_handler(0);return _tmp1DF;}_LLC9: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1C8=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1BC;if(_tmp1C8->tag != 3)goto
_LLCB;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C9.datatype_info;if((_tmp1CA.UnknownDatatype).tag
!= 1)goto _LLCB;_tmp1CB=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1CA.UnknownDatatype).val;
_tmp1CC=_tmp1CB.name;_tmp1CD=_tmp1CB.is_extensible;_tmp1CE=_tmp1C9.targs;}}_LLCA: {
struct Cyc_List_List*_tmp1E0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp1CE);struct Cyc_Absyn_Decl*_tmp1E1=
Cyc_Absyn_datatype_decl(s,_tmp1CC,_tmp1E0,0,_tmp1CD,loc);if(_tmp1A4 != 0){const
char*_tmp98C;Cyc_Parse_err(((_tmp98C="bad attributes on datatype",_tag_dyneither(
_tmp98C,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp98D;struct Cyc_List_List*
_tmp1E4=(_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D->hd=_tmp1E1,((_tmp98D->tl=
0,_tmp98D)))));_npop_handler(0);return _tmp1E4;}}_LLCB: {struct Cyc_Absyn_EnumType_struct*
_tmp1CF=(struct Cyc_Absyn_EnumType_struct*)_tmp1BC;if(_tmp1CF->tag != 14)goto _LLCD;
else{_tmp1D0=_tmp1CF->f1;}}_LLCC: {struct Cyc_Absyn_Enumdecl*_tmp98E;struct Cyc_Absyn_Enumdecl*
_tmp1E5=(_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E->sc=s,((_tmp98E->name=
_tmp1D0,((_tmp98E->fields=0,_tmp98E)))))));if(_tmp1A4 != 0){const char*_tmp98F;Cyc_Parse_err(((
_tmp98F="bad attributes on enum",_tag_dyneither(_tmp98F,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp999;struct Cyc_Absyn_Enum_d_struct _tmp998;struct Cyc_Absyn_Enum_d_struct*
_tmp997;struct Cyc_List_List*_tmp996;struct Cyc_List_List*_tmp1EB=(_tmp996=
_cycalloc(sizeof(*_tmp996)),((_tmp996->hd=((_tmp999=_cycalloc(sizeof(*_tmp999)),((
_tmp999->r=(void*)((_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997[0]=((_tmp998.tag=
8,((_tmp998.f1=_tmp1E5,_tmp998)))),_tmp997)))),((_tmp999->loc=loc,_tmp999)))))),((
_tmp996->tl=0,_tmp996)))));_npop_handler(0);return _tmp1EB;}}_LLCD: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1D1=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1BC;if(_tmp1D1->tag != 15)goto
_LLCF;else{_tmp1D2=_tmp1D1->f1;}}_LLCE: {struct Cyc_Core_Opt*_tmp99C;struct Cyc_Absyn_Enumdecl*
_tmp99B;struct Cyc_Absyn_Enumdecl*_tmp1ED=(_tmp99B=_cycalloc(sizeof(*_tmp99B)),((
_tmp99B->sc=s,((_tmp99B->name=Cyc_Parse_gensym_enum(),((_tmp99B->fields=((
_tmp99C=_cycalloc(sizeof(*_tmp99C)),((_tmp99C->v=_tmp1D2,_tmp99C)))),_tmp99B)))))));
if(_tmp1A4 != 0){const char*_tmp99D;Cyc_Parse_err(((_tmp99D="bad attributes on enum",
_tag_dyneither(_tmp99D,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp9A7;
struct Cyc_Absyn_Enum_d_struct _tmp9A6;struct Cyc_Absyn_Enum_d_struct*_tmp9A5;
struct Cyc_List_List*_tmp9A4;struct Cyc_List_List*_tmp1F3=(_tmp9A4=_cycalloc(
sizeof(*_tmp9A4)),((_tmp9A4->hd=((_tmp9A7=_cycalloc(sizeof(*_tmp9A7)),((_tmp9A7->r=(
void*)((_tmp9A5=_cycalloc(sizeof(*_tmp9A5)),((_tmp9A5[0]=((_tmp9A6.tag=8,((
_tmp9A6.f1=_tmp1ED,_tmp9A6)))),_tmp9A5)))),((_tmp9A7->loc=loc,_tmp9A7)))))),((
_tmp9A4->tl=0,_tmp9A4)))));_npop_handler(0);return _tmp1F3;}}_LLCF:;_LLD0:{const
char*_tmp9A8;Cyc_Parse_err(((_tmp9A8="missing declarator",_tag_dyneither(_tmp9A8,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1F7=0;_npop_handler(0);return
_tmp1F7;}_LLC4:;}}else{void*t=_tmp1AB.f1;struct Cyc_List_List*_tmp1F8=Cyc_Parse_apply_tmss(
mkrgn,_tmp1A2,t,_tmp1A9,_tmp1A4);if(istypedef){if(!exps_empty){const char*_tmp9A9;
Cyc_Parse_err(((_tmp9A9="initializer in typedef declaration",_tag_dyneither(
_tmp9A9,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
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
s;goto _LLD1;_LLD8:;_LLD9:{const char*_tmp9AA;Cyc_Parse_err(((_tmp9AA="declaration within typedef is not a struct, union, or datatype",
_tag_dyneither(_tmp9AA,sizeof(char),63))),loc);}goto _LLD1;_LLD1:;}{struct Cyc_List_List*
_tmp9AB;decls=((_tmp9AB=_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB->hd=d,((_tmp9AB->tl=
decls,_tmp9AB))))));}}{struct Cyc_List_List*_tmp203=decls;_npop_handler(0);return
_tmp203;}}}else{if(_tmp1AB.f2 != 0){const char*_tmp9AC;Cyc_Parse_unimp(((_tmp9AC="nested type declaration within declarator",
_tag_dyneither(_tmp9AC,sizeof(char),42))),loc);}{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp205=_tmp1F8;for(0;_tmp205 != 0;(_tmp205=_tmp205->tl,
_tmp1AA=_tmp1AA->tl)){struct _tuple14 _tmp207;struct _tuple1*_tmp208;struct Cyc_Absyn_Tqual
_tmp209;void*_tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20C;
struct _tuple14*_tmp206=(struct _tuple14*)_tmp205->hd;_tmp207=*_tmp206;_tmp208=
_tmp207.f1;_tmp209=_tmp207.f2;_tmp20A=_tmp207.f3;_tmp20B=_tmp207.f4;_tmp20C=
_tmp207.f5;if(_tmp20B != 0){const char*_tmp9AF;void*_tmp9AE;(_tmp9AE=0,Cyc_Tcutil_warn(
loc,((_tmp9AF="bad type params, ignoring",_tag_dyneither(_tmp9AF,sizeof(char),26))),
_tag_dyneither(_tmp9AE,sizeof(void*),0)));}if(_tmp1AA == 0){const char*_tmp9B2;
void*_tmp9B1;(_tmp9B1=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp9B2="unexpected NULL in parse!",
_tag_dyneither(_tmp9B2,sizeof(char),26))),_tag_dyneither(_tmp9B1,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp211=(struct Cyc_Absyn_Exp*)_tmp1AA->hd;struct Cyc_Absyn_Vardecl*
_tmp212=Cyc_Absyn_new_vardecl(_tmp208,_tmp20A,_tmp211);_tmp212->tq=_tmp209;
_tmp212->sc=s;_tmp212->attributes=_tmp20C;{struct Cyc_Absyn_Var_d_struct*_tmp9B8;
struct Cyc_Absyn_Var_d_struct _tmp9B7;struct Cyc_Absyn_Decl*_tmp9B6;struct Cyc_Absyn_Decl*
_tmp213=(_tmp9B6=_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6->r=(void*)((_tmp9B8=
_cycalloc(sizeof(*_tmp9B8)),((_tmp9B8[0]=((_tmp9B7.tag=0,((_tmp9B7.f1=_tmp212,
_tmp9B7)))),_tmp9B8)))),((_tmp9B6->loc=loc,_tmp9B6)))));struct Cyc_List_List*
_tmp9B9;decls=((_tmp9B9=_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9->hd=_tmp213,((
_tmp9B9->tl=decls,_tmp9B9))))));}}}}{struct Cyc_List_List*_tmp218=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp218;}}}}}}}}};
_pop_region(mkrgn);}static enum Cyc_Absyn_Kind Cyc_Parse_id_to_kind(struct
_dyneither_ptr s,struct Cyc_Position_Segment*loc);static enum Cyc_Absyn_Kind Cyc_Parse_id_to_kind(
struct _dyneither_ptr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2)switch(*((const
char*)_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLDA: return Cyc_Absyn_AnyKind;
case 'M': _LLDB: return Cyc_Absyn_MemKind;case 'B': _LLDC: return Cyc_Absyn_BoxKind;case
'R': _LLDD: return Cyc_Absyn_RgnKind;case 'U': _LLDE: if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))== 'R')return Cyc_Absyn_UniqueRgnKind;
else{break;}case 'T': _LLDF: if(*((const char*)_check_dyneither_subscript(s,sizeof(
char),1))== 'R')return Cyc_Absyn_TopRgnKind;else{break;}case 'E': _LLE0: return Cyc_Absyn_EffKind;
case 'I': _LLE1: return Cyc_Absyn_IntKind;default: _LLE2: break;}{const char*_tmp9BE;
void*_tmp9BD[2];struct Cyc_String_pa_struct _tmp9BC;struct Cyc_Int_pa_struct _tmp9BB;
Cyc_Parse_err((struct _dyneither_ptr)((_tmp9BB.tag=1,((_tmp9BB.f1=(unsigned long)((
int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp9BC.tag=0,((_tmp9BC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp9BD[0]=& _tmp9BC,((_tmp9BD[1]=&
_tmp9BB,Cyc_aprintf(((_tmp9BE="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9BE,
sizeof(char),24))),_tag_dyneither(_tmp9BD,sizeof(void*),2)))))))))))))),loc);}
return Cyc_Absyn_BoxKind;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*
p);static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*
_tmp21D=p->r;struct _tuple1*_tmp21F;struct Cyc_Absyn_Vardecl*_tmp221;struct Cyc_Absyn_Pat*
_tmp222;struct Cyc_Absyn_Pat _tmp223;void*_tmp224;struct Cyc_Absyn_Pat*_tmp227;
enum Cyc_Absyn_Sign _tmp22A;int _tmp22B;char _tmp22D;struct _dyneither_ptr _tmp22F;
struct _tuple1*_tmp231;struct Cyc_List_List*_tmp232;int _tmp233;struct Cyc_Absyn_Exp*
_tmp235;_LLE5: {struct Cyc_Absyn_UnknownId_p_struct*_tmp21E=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp21D;if(_tmp21E->tag != 14)goto _LLE7;else{_tmp21F=_tmp21E->f1;}}_LLE6: {struct
Cyc_Absyn_UnknownId_e_struct _tmp9C1;struct Cyc_Absyn_UnknownId_e_struct*_tmp9C0;
return Cyc_Absyn_new_exp((void*)((_tmp9C0=_cycalloc(sizeof(*_tmp9C0)),((_tmp9C0[0]=((
_tmp9C1.tag=2,((_tmp9C1.f1=_tmp21F,_tmp9C1)))),_tmp9C0)))),p->loc);}_LLE7: {
struct Cyc_Absyn_Reference_p_struct*_tmp220=(struct Cyc_Absyn_Reference_p_struct*)
_tmp21D;if(_tmp220->tag != 2)goto _LLE9;else{_tmp221=_tmp220->f1;_tmp222=_tmp220->f2;
_tmp223=*_tmp222;_tmp224=_tmp223.r;{struct Cyc_Absyn_Wild_p_struct*_tmp225=(
struct Cyc_Absyn_Wild_p_struct*)_tmp224;if(_tmp225->tag != 0)goto _LLE9;}}}_LLE8: {
struct Cyc_Absyn_UnknownId_e_struct _tmp9C4;struct Cyc_Absyn_UnknownId_e_struct*
_tmp9C3;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp9C3=_cycalloc(
sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C4.tag=2,((_tmp9C4.f1=_tmp221->name,
_tmp9C4)))),_tmp9C3)))),p->loc),p->loc);}_LLE9: {struct Cyc_Absyn_Pointer_p_struct*
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
_tmp233=_tmp230->f3;if(_tmp233 != 0)goto _LLF5;}}_LLF4: {struct Cyc_Absyn_UnknownId_e_struct
_tmp9C7;struct Cyc_Absyn_UnknownId_e_struct*_tmp9C6;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6[0]=((_tmp9C7.tag=2,((
_tmp9C7.f1=_tmp231,_tmp9C7)))),_tmp9C6)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp232);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLF5: {struct Cyc_Absyn_Exp_p_struct*_tmp234=(struct Cyc_Absyn_Exp_p_struct*)
_tmp21D;if(_tmp234->tag != 16)goto _LLF7;else{_tmp235=_tmp234->f1;}}_LLF6: return
_tmp235;_LLF7:;_LLF8:{const char*_tmp9C8;Cyc_Parse_err(((_tmp9C8="cannot mix patterns and expressions in case",
_tag_dyneither(_tmp9C8,sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);
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
_tmp23E,_tmp23E + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1);struct
_tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct _tuple6 yyzzz;{union Cyc_YYSTYPE
_tmp23F=yy1;struct _tuple6 _tmp240;_LLFA: if((_tmp23F.Int_tok).tag != 1)goto _LLFC;
_tmp240=(struct _tuple6)(_tmp23F.Int_tok).val;_LLFB: yyzzz=_tmp240;goto _LLF9;_LLFC:;
_LLFD:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLF9:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_Int_tok(struct _tuple6 yy1);union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){
union Cyc_YYSTYPE _tmp9C9;return((_tmp9C9.Int_tok).val=yy1,(((_tmp9C9.Int_tok).tag=
1,_tmp9C9)));}static char _tmp243[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1);char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){char yyzzz;{union Cyc_YYSTYPE
_tmp244=yy1;char _tmp245;_LLFF: if((_tmp244.Char_tok).tag != 2)goto _LL101;_tmp245=(
char)(_tmp244.Char_tok).val;_LL100: yyzzz=_tmp245;goto _LLFE;_LL101:;_LL102:(int)
_throw((void*)& Cyc_yyfail_Char_tok);_LLFE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_Char_tok(
char yy1);union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9CA;return((
_tmp9CA.Char_tok).val=yy1,(((_tmp9CA.Char_tok).tag=2,_tmp9CA)));}static char
_tmp248[13]="string_t<`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp248,_tmp248,_tmp248 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE yy1);struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp249=yy1;struct _dyneither_ptr
_tmp24A;_LL104: if((_tmp249.String_tok).tag != 3)goto _LL106;_tmp24A=(struct
_dyneither_ptr)(_tmp249.String_tok).val;_LL105: yyzzz=_tmp24A;goto _LL103;_LL106:;
_LL107:(int)_throw((void*)& Cyc_yyfail_String_tok);_LL103:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1);union Cyc_YYSTYPE Cyc_String_tok(struct
_dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.String_tok).val=yy1,(((
_tmp9CB.String_tok).tag=3,_tmp9CB)));}static char _tmp24D[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp24D,
_tmp24D,_tmp24D + 56}};struct _tuple19*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1);struct
_tuple19*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE
_tmp24E=yy1;struct _tuple19*_tmp24F;_LL109: if((_tmp24E.YY1).tag != 6)goto _LL10B;
_tmp24F=(struct _tuple19*)(_tmp24E.YY1).val;_LL10A: yyzzz=_tmp24F;goto _LL108;
_LL10B:;_LL10C:(int)_throw((void*)& Cyc_yyfail_YY1);_LL108:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY1(struct _tuple19*yy1);union Cyc_YYSTYPE Cyc_YY1(struct _tuple19*
yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY1).val=yy1,(((_tmp9CC.YY1).tag=6,
_tmp9CC)));}static char _tmp252[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp252,_tmp252,_tmp252 + 19}};union Cyc_Absyn_Constraint*
Cyc_yyget_YY2(union Cyc_YYSTYPE yy1);union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union
Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp253=yy1;
union Cyc_Absyn_Constraint*_tmp254;_LL10E: if((_tmp253.YY2).tag != 7)goto _LL110;
_tmp254=(union Cyc_Absyn_Constraint*)(_tmp253.YY2).val;_LL10F: yyzzz=_tmp254;goto
_LL10D;_LL110:;_LL111:(int)_throw((void*)& Cyc_yyfail_YY2);_LL10D:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1);union Cyc_YYSTYPE Cyc_YY2(
union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY2).val=
yy1,(((_tmp9CD.YY2).tag=7,_tmp9CD)));}static char _tmp257[6]="exp_t";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp257,_tmp257,_tmp257
+ 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Exp*
Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE
_tmp258=yy1;struct Cyc_Absyn_Exp*_tmp259;_LL113: if((_tmp258.YY3).tag != 8)goto
_LL115;_tmp259=(struct Cyc_Absyn_Exp*)(_tmp258.YY3).val;_LL114: yyzzz=_tmp259;goto
_LL112;_LL115:;_LL116:(int)_throw((void*)& Cyc_yyfail_YY3);_LL112:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_Absyn_Exp*yy1);union Cyc_YYSTYPE Cyc_YY3(struct
Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY3).val=yy1,(((
_tmp9CE.YY3).tag=8,_tmp9CE)));}static char _tmp25C[10]="exp_opt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 10}};struct Cyc_Absyn_Exp*
Cyc_yyget_YY4(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp25D=yy1;struct Cyc_Absyn_Exp*
_tmp25E;_LL118: if((_tmp25D.YY4).tag != 9)goto _LL11A;_tmp25E=(struct Cyc_Absyn_Exp*)(
_tmp25D.YY4).val;_LL119: yyzzz=_tmp25E;goto _LL117;_LL11A:;_LL11B:(int)_throw((
void*)& Cyc_yyfail_YY4);_LL117:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*
yy1);union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9CF;
return((_tmp9CF.YY4).val=yy1,(((_tmp9CF.YY4).tag=9,_tmp9CF)));}static char _tmp261[
17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{
_tmp261,_tmp261,_tmp261 + 17}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE
yy1);struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp262=yy1;struct Cyc_List_List*_tmp263;_LL11D: if((
_tmp262.YY5).tag != 10)goto _LL11F;_tmp263=(struct Cyc_List_List*)(_tmp262.YY5).val;
_LL11E: yyzzz=_tmp263;goto _LL11C;_LL11F:;_LL120:(int)_throw((void*)& Cyc_yyfail_YY5);
_LL11C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY5).val=
yy1,(((_tmp9D0.YY5).tag=10,_tmp9D0)));}static char _tmp266[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp266,
_tmp266,_tmp266 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{
union Cyc_YYSTYPE _tmp267=yy1;struct Cyc_List_List*_tmp268;_LL122: if((_tmp267.YY6).tag
!= 11)goto _LL124;_tmp268=(struct Cyc_List_List*)(_tmp267.YY6).val;_LL123: yyzzz=
_tmp268;goto _LL121;_LL124:;_LL125:(int)_throw((void*)& Cyc_yyfail_YY6);_LL121:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY6).val=
yy1,(((_tmp9D1.YY6).tag=11,_tmp9D1)));}static char _tmp26B[9]="primop_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp26B,_tmp26B,
_tmp26B + 9}};enum Cyc_Absyn_Primop Cyc_yyget_YY7(union Cyc_YYSTYPE yy1);enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp26C=yy1;enum Cyc_Absyn_Primop _tmp26D;_LL127: if((_tmp26C.YY7).tag != 12)goto
_LL129;_tmp26D=(enum Cyc_Absyn_Primop)(_tmp26C.YY7).val;_LL128: yyzzz=_tmp26D;
goto _LL126;_LL129:;_LL12A:(int)_throw((void*)& Cyc_yyfail_YY7);_LL126:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1);union Cyc_YYSTYPE Cyc_YY7(
enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY7).val=yy1,(((
_tmp9D2.YY7).tag=12,_tmp9D2)));}static char _tmp270[19]="opt_t<primop_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp270,
_tmp270,_tmp270 + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1);
struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{
union Cyc_YYSTYPE _tmp271=yy1;struct Cyc_Core_Opt*_tmp272;_LL12C: if((_tmp271.YY8).tag
!= 13)goto _LL12E;_tmp272=(struct Cyc_Core_Opt*)(_tmp271.YY8).val;_LL12D: yyzzz=
_tmp272;goto _LL12B;_LL12E:;_LL12F:(int)_throw((void*)& Cyc_yyfail_YY8);_LL12B:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*yy1);union Cyc_YYSTYPE Cyc_YY8(
struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY8).val=yy1,(((
_tmp9D3.YY8).tag=13,_tmp9D3)));}static char _tmp275[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp275,_tmp275,_tmp275 + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1);struct _tuple1*Cyc_yyget_QualId_tok(
union Cyc_YYSTYPE yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp276=yy1;struct
_tuple1*_tmp277;_LL131: if((_tmp276.QualId_tok).tag != 5)goto _LL133;_tmp277=(
struct _tuple1*)(_tmp276.QualId_tok).val;_LL132: yyzzz=_tmp277;goto _LL130;_LL133:;
_LL134:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL130:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_QualId_tok(struct _tuple1*yy1);union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*
yy1){union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.QualId_tok).val=yy1,(((_tmp9D4.QualId_tok).tag=
5,_tmp9D4)));}static char _tmp27A[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp27A,_tmp27A,_tmp27A + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_YY9(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE _tmp27B=yy1;struct Cyc_Absyn_Stmt*
_tmp27C;_LL136: if((_tmp27B.YY9).tag != 14)goto _LL138;_tmp27C=(struct Cyc_Absyn_Stmt*)(
_tmp27B.YY9).val;_LL137: yyzzz=_tmp27C;goto _LL135;_LL138:;_LL139:(int)_throw((
void*)& Cyc_yyfail_YY9);_LL135:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*
yy1);union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9D5;
return((_tmp9D5.YY9).val=yy1,(((_tmp9D5.YY9).tag=14,_tmp9D5)));}static char
_tmp27F[27]="list_t<switch_clause_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={
Cyc_Core_Failure,{_tmp27F,_tmp27F,_tmp27F + 27}};struct Cyc_List_List*Cyc_yyget_YY10(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp280=yy1;struct Cyc_List_List*
_tmp281;_LL13B: if((_tmp280.YY10).tag != 15)goto _LL13D;_tmp281=(struct Cyc_List_List*)(
_tmp280.YY10).val;_LL13C: yyzzz=_tmp281;goto _LL13A;_LL13D:;_LL13E:(int)_throw((
void*)& Cyc_yyfail_YY10);_LL13A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D6;
return((_tmp9D6.YY10).val=yy1,(((_tmp9D6.YY10).tag=15,_tmp9D6)));}static char
_tmp284[6]="pat_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{
_tmp284,_tmp284,_tmp284 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE
yy1);struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*
yyzzz;{union Cyc_YYSTYPE _tmp285=yy1;struct Cyc_Absyn_Pat*_tmp286;_LL140: if((
_tmp285.YY11).tag != 16)goto _LL142;_tmp286=(struct Cyc_Absyn_Pat*)(_tmp285.YY11).val;
_LL141: yyzzz=_tmp286;goto _LL13F;_LL142:;_LL143:(int)_throw((void*)& Cyc_yyfail_YY11);
_LL13F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1);union Cyc_YYSTYPE
Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY11).val=
yy1,(((_tmp9D7.YY11).tag=16,_tmp9D7)));}static char _tmp289[28]="$(list_t<pat_t,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp289,
_tmp289,_tmp289 + 28}};struct _tuple20*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1);struct
_tuple20*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE
_tmp28A=yy1;struct _tuple20*_tmp28B;_LL145: if((_tmp28A.YY12).tag != 17)goto _LL147;
_tmp28B=(struct _tuple20*)(_tmp28A.YY12).val;_LL146: yyzzz=_tmp28B;goto _LL144;
_LL147:;_LL148:(int)_throw((void*)& Cyc_yyfail_YY12);_LL144:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY12(struct _tuple20*yy1);union Cyc_YYSTYPE Cyc_YY12(struct _tuple20*
yy1){union Cyc_YYSTYPE _tmp9D8;return((_tmp9D8.YY12).val=yy1,(((_tmp9D8.YY12).tag=
17,_tmp9D8)));}static char _tmp28E[17]="list_t<pat_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp28E,_tmp28E,_tmp28E + 17}};struct Cyc_List_List*
Cyc_yyget_YY13(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp28F=yy1;struct Cyc_List_List*
_tmp290;_LL14A: if((_tmp28F.YY13).tag != 18)goto _LL14C;_tmp290=(struct Cyc_List_List*)(
_tmp28F.YY13).val;_LL14B: yyzzz=_tmp290;goto _LL149;_LL14C:;_LL14D:(int)_throw((
void*)& Cyc_yyfail_YY13);_LL149:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D9;
return((_tmp9D9.YY13).val=yy1,(((_tmp9D9.YY13).tag=18,_tmp9D9)));}static char
_tmp293[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp293,_tmp293,_tmp293 + 36}};struct _tuple21*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1);struct _tuple21*Cyc_yyget_YY14(union Cyc_YYSTYPE
yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE _tmp294=yy1;struct _tuple21*_tmp295;
_LL14F: if((_tmp294.YY14).tag != 19)goto _LL151;_tmp295=(struct _tuple21*)(_tmp294.YY14).val;
_LL150: yyzzz=_tmp295;goto _LL14E;_LL151:;_LL152:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL14E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY14(struct _tuple21*yy1);union Cyc_YYSTYPE
Cyc_YY14(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9DA;return((_tmp9DA.YY14).val=
yy1,(((_tmp9DA.YY14).tag=19,_tmp9DA)));}static char _tmp298[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp298,
_tmp298,_tmp298 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp299=yy1;struct Cyc_List_List*_tmp29A;_LL154: if((
_tmp299.YY15).tag != 20)goto _LL156;_tmp29A=(struct Cyc_List_List*)(_tmp299.YY15).val;
_LL155: yyzzz=_tmp29A;goto _LL153;_LL156:;_LL157:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL153:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY15(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY15).val=
yy1,(((_tmp9DB.YY15).tag=20,_tmp9DB)));}static char _tmp29D[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp29D,
_tmp29D,_tmp29D + 58}};struct _tuple20*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1);struct
_tuple20*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE
_tmp29E=yy1;struct _tuple20*_tmp29F;_LL159: if((_tmp29E.YY16).tag != 21)goto _LL15B;
_tmp29F=(struct _tuple20*)(_tmp29E.YY16).val;_LL15A: yyzzz=_tmp29F;goto _LL158;
_LL15B:;_LL15C:(int)_throw((void*)& Cyc_yyfail_YY16);_LL158:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY16(struct _tuple20*yy1);union Cyc_YYSTYPE Cyc_YY16(struct _tuple20*
yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY16).val=yy1,(((_tmp9DC.YY16).tag=
21,_tmp9DC)));}static char _tmp2A2[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp2A2,_tmp2A2,_tmp2A2 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY17(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(union
Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp2A3=yy1;struct
Cyc_Absyn_Fndecl*_tmp2A4;_LL15E: if((_tmp2A3.YY17).tag != 22)goto _LL160;_tmp2A4=(
struct Cyc_Absyn_Fndecl*)(_tmp2A3.YY17).val;_LL15F: yyzzz=_tmp2A4;goto _LL15D;
_LL160:;_LL161:(int)_throw((void*)& Cyc_yyfail_YY17);_LL15D:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1);union Cyc_YYSTYPE Cyc_YY17(struct
Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9DD;return((_tmp9DD.YY17).val=yy1,(((
_tmp9DD.YY17).tag=22,_tmp9DD)));}static char _tmp2A7[18]="list_t<decl_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp2A7,
_tmp2A7,_tmp2A7 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2A8=yy1;struct Cyc_List_List*_tmp2A9;_LL163: if((
_tmp2A8.YY18).tag != 23)goto _LL165;_tmp2A9=(struct Cyc_List_List*)(_tmp2A8.YY18).val;
_LL164: yyzzz=_tmp2A9;goto _LL162;_LL165:;_LL166:(int)_throw((void*)& Cyc_yyfail_YY18);
_LL162:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY18(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY18).val=
yy1,(((_tmp9DE.YY18).tag=23,_tmp9DE)));}static char _tmp2AC[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp2AC,
_tmp2AC,_tmp2AC + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1);struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){struct
Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp2AD=yy1;struct Cyc_Parse_Declaration_spec*
_tmp2AE;_LL168: if((_tmp2AD.YY19).tag != 24)goto _LL16A;_tmp2AE=(struct Cyc_Parse_Declaration_spec*)(
_tmp2AD.YY19).val;_LL169: yyzzz=_tmp2AE;goto _LL167;_LL16A:;_LL16B:(int)_throw((
void*)& Cyc_yyfail_YY19);_LL167:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*
yy1);union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){union Cyc_YYSTYPE
_tmp9DF;return((_tmp9DF.YY19).val=yy1,(((_tmp9DF.YY19).tag=24,_tmp9DF)));}static
char _tmp2B1[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 29}};struct _tuple22*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1);struct _tuple22*Cyc_yyget_YY20(union Cyc_YYSTYPE
yy1){struct _tuple22*yyzzz;{union Cyc_YYSTYPE _tmp2B2=yy1;struct _tuple22*_tmp2B3;
_LL16D: if((_tmp2B2.YY20).tag != 25)goto _LL16F;_tmp2B3=(struct _tuple22*)(_tmp2B2.YY20).val;
_LL16E: yyzzz=_tmp2B3;goto _LL16C;_LL16F:;_LL170:(int)_throw((void*)& Cyc_yyfail_YY20);
_LL16C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY20(struct _tuple22*yy1);union Cyc_YYSTYPE
Cyc_YY20(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY20).val=
yy1,(((_tmp9E0.YY20).tag=25,_tmp9E0)));}static char _tmp2B6[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp2B6,
_tmp2B6,_tmp2B6 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2B7=yy1;struct Cyc_List_List*_tmp2B8;_LL172: if((
_tmp2B7.YY21).tag != 26)goto _LL174;_tmp2B8=(struct Cyc_List_List*)(_tmp2B7.YY21).val;
_LL173: yyzzz=_tmp2B8;goto _LL171;_LL174:;_LL175:(int)_throw((void*)& Cyc_yyfail_YY21);
_LL171:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY21(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E1;return((_tmp9E1.YY21).val=
yy1,(((_tmp9E1.YY21).tag=26,_tmp9E1)));}static char _tmp2BB[16]="storage_class_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp2BB,
_tmp2BB,_tmp2BB + 16}};enum Cyc_Parse_Storage_class Cyc_yyget_YY22(union Cyc_YYSTYPE
yy1);enum Cyc_Parse_Storage_class Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class
yyzzz;{union Cyc_YYSTYPE _tmp2BC=yy1;enum Cyc_Parse_Storage_class _tmp2BD;_LL177:
if((_tmp2BC.YY22).tag != 27)goto _LL179;_tmp2BD=(enum Cyc_Parse_Storage_class)(
_tmp2BC.YY22).val;_LL178: yyzzz=_tmp2BD;goto _LL176;_LL179:;_LL17A:(int)_throw((
void*)& Cyc_yyfail_YY22);_LL176:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class
yy1);union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){union Cyc_YYSTYPE
_tmp9E2;return((_tmp9E2.YY22).val=yy1,(((_tmp9E2.YY22).tag=27,_tmp9E2)));}static
char _tmp2C0[17]="type_specifier_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY23={
Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 17}};void*Cyc_yyget_YY23(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp2C1=yy1;void*_tmp2C2;_LL17C: if((_tmp2C1.YY23).tag != 28)goto _LL17E;_tmp2C2=(
void*)(_tmp2C1.YY23).val;_LL17D: yyzzz=_tmp2C2;goto _LL17B;_LL17E:;_LL17F:(int)
_throw((void*)& Cyc_yyfail_YY23);_LL17B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(
void*yy1);union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmp9E3;return((
_tmp9E3.YY23).val=yy1,(((_tmp9E3.YY23).tag=28,_tmp9E3)));}static char _tmp2C5[12]="aggr_kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp2C5,
_tmp2C5,_tmp2C5 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(union Cyc_YYSTYPE yy1);
enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind
yyzzz;{union Cyc_YYSTYPE _tmp2C6=yy1;enum Cyc_Absyn_AggrKind _tmp2C7;_LL181: if((
_tmp2C6.YY24).tag != 29)goto _LL183;_tmp2C7=(enum Cyc_Absyn_AggrKind)(_tmp2C6.YY24).val;
_LL182: yyzzz=_tmp2C7;goto _LL180;_LL183:;_LL184:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL180:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1);union
Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9E4;return((
_tmp9E4.YY24).val=yy1,(((_tmp9E4.YY24).tag=29,_tmp9E4)));}static char _tmp2CA[8]="tqual_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp2CA,
_tmp2CA,_tmp2CA + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE yy1);
struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Tqual
yyzzz;{union Cyc_YYSTYPE _tmp2CB=yy1;struct Cyc_Absyn_Tqual _tmp2CC;_LL186: if((
_tmp2CB.YY25).tag != 30)goto _LL188;_tmp2CC=(struct Cyc_Absyn_Tqual)(_tmp2CB.YY25).val;
_LL187: yyzzz=_tmp2CC;goto _LL185;_LL188:;_LL189:(int)_throw((void*)& Cyc_yyfail_YY25);
_LL185:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1);union
Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9E5;return((
_tmp9E5.YY25).val=yy1,(((_tmp9E5.YY25).tag=30,_tmp9E5)));}static char _tmp2CF[23]="list_t<aggrfield_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2CF,
_tmp2CF,_tmp2CF + 23}};struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2D0=yy1;struct Cyc_List_List*_tmp2D1;_LL18B: if((
_tmp2D0.YY26).tag != 31)goto _LL18D;_tmp2D1=(struct Cyc_List_List*)(_tmp2D0.YY26).val;
_LL18C: yyzzz=_tmp2D1;goto _LL18A;_LL18D:;_LL18E:(int)_throw((void*)& Cyc_yyfail_YY26);
_LL18A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.YY26).val=
yy1,(((_tmp9E6.YY26).tag=31,_tmp9E6)));}static char _tmp2D4[34]="list_t<list_t<aggrfield_t,`H>,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2D4,
_tmp2D4,_tmp2D4 + 34}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2D5=yy1;struct Cyc_List_List*_tmp2D6;_LL190: if((
_tmp2D5.YY27).tag != 32)goto _LL192;_tmp2D6=(struct Cyc_List_List*)(_tmp2D5.YY27).val;
_LL191: yyzzz=_tmp2D6;goto _LL18F;_LL192:;_LL193:(int)_throw((void*)& Cyc_yyfail_YY27);
_LL18F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E7;return((_tmp9E7.YY27).val=
yy1,(((_tmp9E7.YY27).tag=32,_tmp9E7)));}static char _tmp2D9[31]="list_t<type_modifier_t<`H>,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2D9,
_tmp2D9,_tmp2D9 + 31}};struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2DA=yy1;struct Cyc_List_List*_tmp2DB;_LL195: if((
_tmp2DA.YY28).tag != 33)goto _LL197;_tmp2DB=(struct Cyc_List_List*)(_tmp2DA.YY28).val;
_LL196: yyzzz=_tmp2DB;goto _LL194;_LL197:;_LL198:(int)_throw((void*)& Cyc_yyfail_YY28);
_LL194:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E8;return((_tmp9E8.YY28).val=
yy1,(((_tmp9E8.YY28).tag=33,_tmp9E8)));}static char _tmp2DE[13]="declarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp2DE,
_tmp2DE,_tmp2DE + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE
yy1);struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*
yyzzz;{union Cyc_YYSTYPE _tmp2DF=yy1;struct Cyc_Parse_Declarator*_tmp2E0;_LL19A: if((
_tmp2DF.YY29).tag != 34)goto _LL19C;_tmp2E0=(struct Cyc_Parse_Declarator*)(_tmp2DF.YY29).val;
_LL19B: yyzzz=_tmp2E0;goto _LL199;_LL19C:;_LL19D:(int)_throw((void*)& Cyc_yyfail_YY29);
_LL199:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1);
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE _tmp9E9;
return((_tmp9E9.YY29).val=yy1,(((_tmp9E9.YY29).tag=34,_tmp9E9)));}static char
_tmp2E3[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1);struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE
_tmp2E4=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2E5;_LL19F: if((_tmp2E4.YY30).tag
!= 35)goto _LL1A1;_tmp2E5=(struct Cyc_Parse_Abstractdeclarator*)(_tmp2E4.YY30).val;
_LL1A0: yyzzz=_tmp2E5;goto _LL19E;_LL1A1:;_LL1A2:(int)_throw((void*)& Cyc_yyfail_YY30);
_LL19E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1);union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*yy1){union Cyc_YYSTYPE
_tmp9EA;return((_tmp9EA.YY30).val=yy1,(((_tmp9EA.YY30).tag=35,_tmp9EA)));}static
char _tmp2E8[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp2E8,_tmp2E8,_tmp2E8 + 5}};int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1);int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2E9=yy1;int _tmp2EA;_LL1A4: if((
_tmp2E9.YY31).tag != 36)goto _LL1A6;_tmp2EA=(int)(_tmp2E9.YY31).val;_LL1A5: yyzzz=
_tmp2EA;goto _LL1A3;_LL1A6:;_LL1A7:(int)_throw((void*)& Cyc_yyfail_YY31);_LL1A3:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1);union Cyc_YYSTYPE Cyc_YY31(int yy1){
union Cyc_YYSTYPE _tmp9EB;return((_tmp9EB.YY31).val=yy1,(((_tmp9EB.YY31).tag=36,
_tmp9EB)));}static char _tmp2ED[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2ED,_tmp2ED,_tmp2ED + 8}};enum Cyc_Absyn_Scope
Cyc_yyget_YY32(union Cyc_YYSTYPE yy1);enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE
yy1){enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2EE=yy1;enum Cyc_Absyn_Scope
_tmp2EF;_LL1A9: if((_tmp2EE.YY32).tag != 37)goto _LL1AB;_tmp2EF=(enum Cyc_Absyn_Scope)(
_tmp2EE.YY32).val;_LL1AA: yyzzz=_tmp2EF;goto _LL1A8;_LL1AB:;_LL1AC:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL1A8:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1);union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9EC;
return((_tmp9EC.YY32).val=yy1,(((_tmp9EC.YY32).tag=37,_tmp9EC)));}static char
_tmp2F2[16]="datatypefield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp2F2,_tmp2F2,_tmp2F2 + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union Cyc_YYSTYPE
_tmp2F3=yy1;struct Cyc_Absyn_Datatypefield*_tmp2F4;_LL1AE: if((_tmp2F3.YY33).tag != 
38)goto _LL1B0;_tmp2F4=(struct Cyc_Absyn_Datatypefield*)(_tmp2F3.YY33).val;_LL1AF:
yyzzz=_tmp2F4;goto _LL1AD;_LL1B0:;_LL1B1:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1AD:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1);
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE
_tmp9ED;return((_tmp9ED.YY33).val=yy1,(((_tmp9ED.YY33).tag=38,_tmp9ED)));}static
char _tmp2F7[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,_tmp2F7 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2F8=yy1;struct Cyc_List_List*
_tmp2F9;_LL1B3: if((_tmp2F8.YY34).tag != 39)goto _LL1B5;_tmp2F9=(struct Cyc_List_List*)(
_tmp2F8.YY34).val;_LL1B4: yyzzz=_tmp2F9;goto _LL1B2;_LL1B5:;_LL1B6:(int)_throw((
void*)& Cyc_yyfail_YY34);_LL1B2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9EE;
return((_tmp9EE.YY34).val=yy1,(((_tmp9EE.YY34).tag=39,_tmp9EE)));}static char
_tmp2FC[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2FC,_tmp2FC,
_tmp2FC + 55}};struct _tuple23*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1);struct _tuple23*
Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple23*yyzzz;{union Cyc_YYSTYPE
_tmp2FD=yy1;struct _tuple23*_tmp2FE;_LL1B8: if((_tmp2FD.YY35).tag != 40)goto _LL1BA;
_tmp2FE=(struct _tuple23*)(_tmp2FD.YY35).val;_LL1B9: yyzzz=_tmp2FE;goto _LL1B7;
_LL1BA:;_LL1BB:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1B7:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY35(struct _tuple23*yy1);union Cyc_YYSTYPE Cyc_YY35(struct _tuple23*
yy1){union Cyc_YYSTYPE _tmp9EF;return((_tmp9EF.YY35).val=yy1,(((_tmp9EF.YY35).tag=
40,_tmp9EF)));}static char _tmp301[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp301,_tmp301,_tmp301 + 17}};struct Cyc_List_List*
Cyc_yyget_YY36(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp302=yy1;struct Cyc_List_List*
_tmp303;_LL1BD: if((_tmp302.YY36).tag != 41)goto _LL1BF;_tmp303=(struct Cyc_List_List*)(
_tmp302.YY36).val;_LL1BE: yyzzz=_tmp303;goto _LL1BC;_LL1BF:;_LL1C0:(int)_throw((
void*)& Cyc_yyfail_YY36);_LL1BC:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F0;
return((_tmp9F0.YY36).val=yy1,(((_tmp9F0.YY36).tag=41,_tmp9F0)));}static char
_tmp306[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp306,_tmp306,_tmp306 + 37}};struct _tuple8*
Cyc_yyget_YY37(union Cyc_YYSTYPE yy1);struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE
yy1){struct _tuple8*yyzzz;{union Cyc_YYSTYPE _tmp307=yy1;struct _tuple8*_tmp308;
_LL1C2: if((_tmp307.YY37).tag != 42)goto _LL1C4;_tmp308=(struct _tuple8*)(_tmp307.YY37).val;
_LL1C3: yyzzz=_tmp308;goto _LL1C1;_LL1C4:;_LL1C5:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1C1:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1);union Cyc_YYSTYPE
Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9F1;return((_tmp9F1.YY37).val=
yy1,(((_tmp9F1.YY37).tag=42,_tmp9F1)));}static char _tmp30B[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp30B,
_tmp30B,_tmp30B + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp30C=yy1;struct Cyc_List_List*_tmp30D;_LL1C7: if((
_tmp30C.YY38).tag != 43)goto _LL1C9;_tmp30D=(struct Cyc_List_List*)(_tmp30C.YY38).val;
_LL1C8: yyzzz=_tmp30D;goto _LL1C6;_LL1C9:;_LL1CA:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1C6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F2;return((_tmp9F2.YY38).val=
yy1,(((_tmp9F2.YY38).tag=43,_tmp9F2)));}static char _tmp310[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp310,
_tmp310,_tmp310 + 127}};struct _tuple24*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1);struct
_tuple24*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct _tuple24*yyzzz;{union Cyc_YYSTYPE
_tmp311=yy1;struct _tuple24*_tmp312;_LL1CC: if((_tmp311.YY39).tag != 44)goto _LL1CE;
_tmp312=(struct _tuple24*)(_tmp311.YY39).val;_LL1CD: yyzzz=_tmp312;goto _LL1CB;
_LL1CE:;_LL1CF:(int)_throw((void*)& Cyc_yyfail_YY39);_LL1CB:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY39(struct _tuple24*yy1);union Cyc_YYSTYPE Cyc_YY39(struct _tuple24*
yy1){union Cyc_YYSTYPE _tmp9F3;return((_tmp9F3.YY39).val=yy1,(((_tmp9F3.YY39).tag=
44,_tmp9F3)));}static char _tmp315[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp315,_tmp315,_tmp315 + 18}};struct Cyc_List_List*
Cyc_yyget_YY40(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp316=yy1;struct Cyc_List_List*
_tmp317;_LL1D1: if((_tmp316.YY40).tag != 45)goto _LL1D3;_tmp317=(struct Cyc_List_List*)(
_tmp316.YY40).val;_LL1D2: yyzzz=_tmp317;goto _LL1D0;_LL1D3:;_LL1D4:(int)_throw((
void*)& Cyc_yyfail_YY40);_LL1D0:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F4;
return((_tmp9F4.YY40).val=yy1,(((_tmp9F4.YY40).tag=45,_tmp9F4)));}static char
_tmp31A[24]="list_t<designator_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={
Cyc_Core_Failure,{_tmp31A,_tmp31A,_tmp31A + 24}};struct Cyc_List_List*Cyc_yyget_YY41(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp31B=yy1;struct Cyc_List_List*
_tmp31C;_LL1D6: if((_tmp31B.YY41).tag != 46)goto _LL1D8;_tmp31C=(struct Cyc_List_List*)(
_tmp31B.YY41).val;_LL1D7: yyzzz=_tmp31C;goto _LL1D5;_LL1D8:;_LL1D9:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1D5:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F5;
return((_tmp9F5.YY41).val=yy1,(((_tmp9F5.YY41).tag=46,_tmp9F5)));}static char
_tmp31F[13]="designator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY42={
Cyc_Core_Failure,{_tmp31F,_tmp31F,_tmp31F + 13}};void*Cyc_yyget_YY42(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp320=yy1;void*_tmp321;_LL1DB: if((_tmp320.YY42).tag != 47)goto _LL1DD;_tmp321=(
void*)(_tmp320.YY42).val;_LL1DC: yyzzz=_tmp321;goto _LL1DA;_LL1DD:;_LL1DE:(int)
_throw((void*)& Cyc_yyfail_YY42);_LL1DA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(
void*yy1);union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp9F6;return((
_tmp9F6.YY42).val=yy1,(((_tmp9F6.YY42).tag=47,_tmp9F6)));}static char _tmp324[7]="kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp324,
_tmp324,_tmp324 + 7}};enum Cyc_Absyn_Kind Cyc_yyget_YY43(union Cyc_YYSTYPE yy1);
enum Cyc_Absyn_Kind Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Kind yyzzz;{
union Cyc_YYSTYPE _tmp325=yy1;enum Cyc_Absyn_Kind _tmp326;_LL1E0: if((_tmp325.YY43).tag
!= 48)goto _LL1E2;_tmp326=(enum Cyc_Absyn_Kind)(_tmp325.YY43).val;_LL1E1: yyzzz=
_tmp326;goto _LL1DF;_LL1E2:;_LL1E3:(int)_throw((void*)& Cyc_yyfail_YY43);_LL1DF:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(enum Cyc_Absyn_Kind yy1);union Cyc_YYSTYPE
Cyc_YY43(enum Cyc_Absyn_Kind yy1){union Cyc_YYSTYPE _tmp9F7;return((_tmp9F7.YY43).val=
yy1,(((_tmp9F7.YY43).tag=48,_tmp9F7)));}static char _tmp329[7]="type_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp329,_tmp329,
_tmp329 + 7}};void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY44(union
Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp32A=yy1;void*_tmp32B;_LL1E5: if((
_tmp32A.YY44).tag != 49)goto _LL1E7;_tmp32B=(void*)(_tmp32A.YY44).val;_LL1E6: yyzzz=
_tmp32B;goto _LL1E4;_LL1E7:;_LL1E8:(int)_throw((void*)& Cyc_yyfail_YY44);_LL1E4:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1);union Cyc_YYSTYPE Cyc_YY44(void*
yy1){union Cyc_YYSTYPE _tmp9F8;return((_tmp9F8.YY44).val=yy1,(((_tmp9F8.YY44).tag=
49,_tmp9F8)));}static char _tmp32E[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp32E,_tmp32E,_tmp32E + 23}};struct Cyc_List_List*
Cyc_yyget_YY45(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp32F=yy1;struct Cyc_List_List*
_tmp330;_LL1EA: if((_tmp32F.YY45).tag != 50)goto _LL1EC;_tmp330=(struct Cyc_List_List*)(
_tmp32F.YY45).val;_LL1EB: yyzzz=_tmp330;goto _LL1E9;_LL1EC:;_LL1ED:(int)_throw((
void*)& Cyc_yyfail_YY45);_LL1E9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9F9;
return((_tmp9F9.YY45).val=yy1,(((_tmp9F9.YY45).tag=50,_tmp9F9)));}static char
_tmp333[12]="attribute_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={
Cyc_Core_Failure,{_tmp333,_tmp333,_tmp333 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp334=yy1;void*_tmp335;_LL1EF: if((_tmp334.YY46).tag != 51)goto _LL1F1;_tmp335=(
void*)(_tmp334.YY46).val;_LL1F0: yyzzz=_tmp335;goto _LL1EE;_LL1F1:;_LL1F2:(int)
_throw((void*)& Cyc_yyfail_YY46);_LL1EE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1);union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmp9FA;return((
_tmp9FA.YY46).val=yy1,(((_tmp9FA.YY46).tag=51,_tmp9FA)));}static char _tmp338[12]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp338,
_tmp338,_tmp338 + 12}};struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE
yy1);struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{union Cyc_YYSTYPE _tmp339=yy1;struct Cyc_Absyn_Enumfield*_tmp33A;_LL1F4: if((
_tmp339.YY47).tag != 52)goto _LL1F6;_tmp33A=(struct Cyc_Absyn_Enumfield*)(_tmp339.YY47).val;
_LL1F5: yyzzz=_tmp33A;goto _LL1F3;_LL1F6:;_LL1F7:(int)_throw((void*)& Cyc_yyfail_YY47);
_LL1F3:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1);
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9FB;
return((_tmp9FB.YY47).val=yy1,(((_tmp9FB.YY47).tag=52,_tmp9FB)));}static char
_tmp33D[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp33D,_tmp33D,_tmp33D + 23}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp33E=yy1;struct Cyc_List_List*
_tmp33F;_LL1F9: if((_tmp33E.YY48).tag != 53)goto _LL1FB;_tmp33F=(struct Cyc_List_List*)(
_tmp33E.YY48).val;_LL1FA: yyzzz=_tmp33F;goto _LL1F8;_LL1FB:;_LL1FC:(int)_throw((
void*)& Cyc_yyfail_YY48);_LL1F8:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FC;
return((_tmp9FC.YY48).val=yy1,(((_tmp9FC.YY48).tag=53,_tmp9FC)));}static char
_tmp342[17]="opt_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={
Cyc_Core_Failure,{_tmp342,_tmp342,_tmp342 + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(
union Cyc_YYSTYPE yy1);struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp343=yy1;struct Cyc_Core_Opt*_tmp344;
_LL1FE: if((_tmp343.YY49).tag != 54)goto _LL200;_tmp344=(struct Cyc_Core_Opt*)(
_tmp343.YY49).val;_LL1FF: yyzzz=_tmp344;goto _LL1FD;_LL200:;_LL201:(int)_throw((
void*)& Cyc_yyfail_YY49);_LL1FD:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*
yy1);union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9FD;
return((_tmp9FD.YY49).val=yy1,(((_tmp9FD.YY49).tag=54,_tmp9FD)));}static char
_tmp347[31]="list_t<$(type_t,type_t)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp347,_tmp347,_tmp347 + 31}};struct Cyc_List_List*
Cyc_yyget_YY50(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp348=yy1;struct Cyc_List_List*
_tmp349;_LL203: if((_tmp348.YY50).tag != 55)goto _LL205;_tmp349=(struct Cyc_List_List*)(
_tmp348.YY50).val;_LL204: yyzzz=_tmp349;goto _LL202;_LL205:;_LL206:(int)_throw((
void*)& Cyc_yyfail_YY50);_LL202:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9FE;
return((_tmp9FE.YY50).val=yy1,(((_tmp9FE.YY50).tag=55,_tmp9FE)));}static char
_tmp34C[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={
Cyc_Core_Failure,{_tmp34C,_tmp34C,_tmp34C + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(
union Cyc_YYSTYPE yy1);union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp34D=yy1;union Cyc_Absyn_Constraint*
_tmp34E;_LL208: if((_tmp34D.YY51).tag != 56)goto _LL20A;_tmp34E=(union Cyc_Absyn_Constraint*)(
_tmp34D.YY51).val;_LL209: yyzzz=_tmp34E;goto _LL207;_LL20A:;_LL20B:(int)_throw((
void*)& Cyc_yyfail_YY51);_LL207:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*
yy1);union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmp9FF;return((_tmp9FF.YY51).val=yy1,(((_tmp9FF.YY51).tag=56,_tmp9FF)));}static
char _tmp351[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp351,_tmp351,_tmp351 + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp352=yy1;struct Cyc_List_List*
_tmp353;_LL20D: if((_tmp352.YY52).tag != 57)goto _LL20F;_tmp353=(struct Cyc_List_List*)(
_tmp352.YY52).val;_LL20E: yyzzz=_tmp353;goto _LL20C;_LL20F:;_LL210:(int)_throw((
void*)& Cyc_yyfail_YY52);_LL20C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA00;
return((_tmpA00.YY52).val=yy1,(((_tmpA00.YY52).tag=57,_tmpA00)));}static char
_tmp356[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp356,_tmp356,_tmp356 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp357=yy1;void*_tmp358;_LL212: if((_tmp357.YY53).tag != 58)goto _LL214;_tmp358=(
void*)(_tmp357.YY53).val;_LL213: yyzzz=_tmp358;goto _LL211;_LL214:;_LL215:(int)
_throw((void*)& Cyc_yyfail_YY53);_LL211:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY53(
void*yy1);union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmpA01;return((
_tmpA01.YY53).val=yy1,(((_tmpA01.YY53).tag=58,_tmpA01)));}static char _tmp35B[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp35B,
_tmp35B,_tmp35B + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp35C=yy1;struct Cyc_List_List*_tmp35D;_LL217: if((
_tmp35C.YY54).tag != 59)goto _LL219;_tmp35D=(struct Cyc_List_List*)(_tmp35C.YY54).val;
_LL218: yyzzz=_tmp35D;goto _LL216;_LL219:;_LL21A:(int)_throw((void*)& Cyc_yyfail_YY54);
_LL216:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmpA02;return((_tmpA02.YY54).val=
yy1,(((_tmpA02.YY54).tag=59,_tmpA02)));}struct Cyc_Yyltype{int timestamp;int
first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc();
struct Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmpA03;return(_tmpA03.timestamp=
0,((_tmpA03.first_line=0,((_tmpA03.first_column=0,((_tmpA03.last_line=0,((
_tmpA03.last_column=0,_tmpA03)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
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
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmpA08(
unsigned int*_tmpA07,unsigned int*_tmpA06,short**_tmpA05){for(*_tmpA07=0;*
_tmpA07 < *_tmpA06;(*_tmpA07)++){(*_tmpA05)[*_tmpA07]=(short)0;}}static void
_tmpA0D(unsigned int*_tmpA0C,unsigned int*_tmpA0B,union Cyc_YYSTYPE**_tmpA0A){
for(*_tmpA0C=0;*_tmpA0C < *_tmpA0B;(*_tmpA0C)++){(*_tmpA0A)[*_tmpA0C]=Cyc_yylval;}}
static void _tmpA12(unsigned int*_tmpA11,unsigned int*_tmpA10,struct Cyc_Yyltype**
_tmpA0F){for(*_tmpA11=0;*_tmpA11 < *_tmpA10;(*_tmpA11)++){(*_tmpA0F)[*_tmpA11]=
Cyc_yynewloc();}}static void _tmpF0A(unsigned int*_tmpF09,unsigned int*_tmpF08,
char**_tmpF06){for(*_tmpF09=0;*_tmpF09 < *_tmpF08;(*_tmpF09)++){(*_tmpF06)[*
_tmpF09]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct
_RegionHandle _tmp480=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp480;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;unsigned int _tmpA07;unsigned int _tmpA06;short*_tmpA05;
unsigned int _tmpA04;short*yyss=(short*)((_tmpA04=10000,((_tmpA05=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmpA04)),((((_tmpA06=_tmpA04,
_tmpA08(& _tmpA07,& _tmpA06,& _tmpA05))),_tmpA05))))));int yyvsp_offset;unsigned int
_tmpA0C;unsigned int _tmpA0B;union Cyc_YYSTYPE*_tmpA0A;unsigned int _tmpA09;union
Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)((_tmpA09=10000,((_tmpA0A=(union Cyc_YYSTYPE*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmpA09)),((((
_tmpA0B=_tmpA09,_tmpA0D(& _tmpA0C,& _tmpA0B,& _tmpA0A))),_tmpA0A))))));int
yylsp_offset;unsigned int _tmpA11;unsigned int _tmpA10;struct Cyc_Yyltype*_tmpA0F;
unsigned int _tmpA0E;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmpA0E=10000,((
_tmpA0F=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct
Cyc_Yyltype),_tmpA0E)),((((_tmpA10=_tmpA0E,_tmpA12(& _tmpA11,& _tmpA10,& _tmpA0F))),
_tmpA0F))))));int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){{const char*_tmpA13;Cyc_yyerror(((_tmpA13="parser stack overflow",
_tag_dyneither(_tmpA13,sizeof(char),22))));}(int)_throw((void*)& Cyc_Yystack_overflow_val);}}
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
_LL221:;_LL222:(void)_throw(_tmp487);_LL21E:;}}}{struct _handler_cons _tmp489;
_push_handler(& _tmp489);{int _tmp48B=0;if(setjmp(_tmp489.handler))_tmp48B=1;if(!
_tmp48B){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp48A=(void*)_exn_thrown;void*_tmp48D=_tmp48A;_LL224: {
struct Cyc_Core_Failure_struct*_tmp48E=(struct Cyc_Core_Failure_struct*)_tmp48D;
if(_tmp48E->tag != Cyc_Core_Failure)goto _LL226;}_LL225: y=0;goto _LL223;_LL226:;
_LL227:(void)_throw(_tmp48D);_LL223:;}}}yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));break;}
case 3: _LL21D:{struct Cyc_Absyn_Decl*_tmpA1D;struct Cyc_Absyn_Using_d_struct _tmpA1C;
struct Cyc_Absyn_Using_d_struct*_tmpA1B;struct Cyc_List_List*_tmpA1A;yyval=Cyc_YY18(((
_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A->hd=((_tmpA1D=_cycalloc(sizeof(*
_tmpA1D)),((_tmpA1D->r=(void*)((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B[0]=((
_tmpA1C.tag=11,((_tmpA1C.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA1C.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA1C)))))),_tmpA1B)))),((
_tmpA1D->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA1D)))))),((_tmpA1A->tl=0,_tmpA1A)))))));}Cyc_Lex_leave_using();
break;case 4: _LL228:{struct Cyc_Absyn_Decl*_tmpA27;struct Cyc_Absyn_Using_d_struct
_tmpA26;struct Cyc_Absyn_Using_d_struct*_tmpA25;struct Cyc_List_List*_tmpA24;yyval=
Cyc_YY18(((_tmpA24=_cycalloc(sizeof(*_tmpA24)),((_tmpA24->hd=((_tmpA27=_cycalloc(
sizeof(*_tmpA27)),((_tmpA27->r=(void*)((_tmpA25=_cycalloc(sizeof(*_tmpA25)),((
_tmpA25[0]=((_tmpA26.tag=11,((_tmpA26.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA26.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA26)))))),
_tmpA25)))),((_tmpA27->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA27)))))),((
_tmpA24->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA24)))))));}break;case 5: _LL229:{struct Cyc_Absyn_Decl*_tmpA36;
struct Cyc_Absyn_Namespace_d_struct _tmpA35;struct _dyneither_ptr*_tmpA34;struct Cyc_Absyn_Namespace_d_struct*
_tmpA33;struct Cyc_List_List*_tmpA32;yyval=Cyc_YY18(((_tmpA32=_cycalloc(sizeof(*
_tmpA32)),((_tmpA32->hd=((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->r=(void*)((
_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33[0]=((_tmpA35.tag=10,((_tmpA35.f1=((
_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA34)))),((_tmpA35.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA35)))))),
_tmpA33)))),((_tmpA36->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA36)))))),((
_tmpA32->tl=0,_tmpA32)))))));}Cyc_Lex_leave_namespace();break;case 6: _LL22A: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp49C;_push_handler(& _tmp49C);{int _tmp49E=0;if(setjmp(_tmp49C.handler))
_tmp49E=1;if(!_tmp49E){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp49D=(void*)_exn_thrown;void*_tmp4A0=_tmp49D;_LL22D: {struct Cyc_Core_Failure_struct*
_tmp4A1=(struct Cyc_Core_Failure_struct*)_tmp4A0;if(_tmp4A1->tag != Cyc_Core_Failure)
goto _LL22F;}_LL22E:{const char*_tmpA37;nspace=((_tmpA37="",_tag_dyneither(_tmpA37,
sizeof(char),1)));}goto _LL22C;_LL22F:;_LL230:(void)_throw(_tmp4A0);_LL22C:;}}}{
struct _handler_cons _tmp4A3;_push_handler(& _tmp4A3);{int _tmp4A5=0;if(setjmp(
_tmp4A3.handler))_tmp4A5=1;if(!_tmp4A5){x=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp4A4=(void*)_exn_thrown;void*_tmp4A7=_tmp4A4;_LL232: {struct Cyc_Core_Failure_struct*
_tmp4A8=(struct Cyc_Core_Failure_struct*)_tmp4A7;if(_tmp4A8->tag != Cyc_Core_Failure)
goto _LL234;}_LL233: x=0;goto _LL231;_LL234:;_LL235:(void)_throw(_tmp4A7);_LL231:;}}}{
struct _handler_cons _tmp4A9;_push_handler(& _tmp4A9);{int _tmp4AB=0;if(setjmp(
_tmp4A9.handler))_tmp4AB=1;if(!_tmp4AB){y=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{void*
_tmp4AA=(void*)_exn_thrown;void*_tmp4AD=_tmp4AA;_LL237: {struct Cyc_Core_Failure_struct*
_tmp4AE=(struct Cyc_Core_Failure_struct*)_tmp4AD;if(_tmp4AE->tag != Cyc_Core_Failure)
goto _LL239;}_LL238: y=0;goto _LL236;_LL239:;_LL23A:(void)_throw(_tmp4AD);_LL236:;}}}{
struct Cyc_Absyn_Decl*_tmpA46;struct Cyc_Absyn_Namespace_d_struct _tmpA45;struct
_dyneither_ptr*_tmpA44;struct Cyc_Absyn_Namespace_d_struct*_tmpA43;struct Cyc_List_List*
_tmpA42;yyval=Cyc_YY18(((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->hd=((
_tmpA46=_cycalloc(sizeof(*_tmpA46)),((_tmpA46->r=(void*)((_tmpA43=_cycalloc(
sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA45.tag=10,((_tmpA45.f1=((_tmpA44=_cycalloc(
sizeof(*_tmpA44)),((_tmpA44[0]=nspace,_tmpA44)))),((_tmpA45.f2=x,_tmpA45)))))),
_tmpA43)))),((_tmpA46->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA46)))))),((
_tmpA42->tl=y,_tmpA42)))))));}break;}case 7: _LL22B:{const char*_tmpA47;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpA47="C",_tag_dyneither(_tmpA47,sizeof(char),2))))
== 0){struct Cyc_Absyn_Decl*_tmpA51;struct Cyc_Absyn_ExternC_d_struct _tmpA50;
struct Cyc_Absyn_ExternC_d_struct*_tmpA4F;struct Cyc_List_List*_tmpA4E;yyval=Cyc_YY18(((
_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->hd=((_tmpA51=_cycalloc(sizeof(*
_tmpA51)),((_tmpA51->r=(void*)((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F[0]=((
_tmpA50.tag=12,((_tmpA50.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpA50)))),_tmpA4F)))),((_tmpA51->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA51)))))),((
_tmpA4E->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA4E)))))));}else{{const char*_tmpA52;if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),((_tmpA52="C include",_tag_dyneither(_tmpA52,sizeof(char),10))))
!= 0){const char*_tmpA53;Cyc_Parse_err(((_tmpA53="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmpA53,sizeof(char),29))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_Absyn_Decl*
_tmpA5D;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA5C;struct Cyc_Absyn_ExternCinclude_d_struct*
_tmpA5B;struct Cyc_List_List*_tmpA5A;yyval=Cyc_YY18(((_tmpA5A=_cycalloc(sizeof(*
_tmpA5A)),((_tmpA5A->hd=((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->r=(void*)((
_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5C.tag=13,((_tmpA5C.f1=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA5C.f2=0,_tmpA5C)))))),_tmpA5B)))),((_tmpA5D->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA5D)))))),((
_tmpA5A->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA5A)))))));}}}break;case 8: _LL23B:{const char*_tmpA5E;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmpA5E="C include",_tag_dyneither(_tmpA5E,sizeof(
char),10))))!= 0){const char*_tmpA5F;Cyc_Parse_err(((_tmpA5F="expecting \"C include\"",
_tag_dyneither(_tmpA5F,sizeof(char),22))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Decl*_tmpA69;struct Cyc_Absyn_ExternCinclude_d_struct _tmpA68;
struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA67;struct Cyc_List_List*_tmpA66;
yyval=Cyc_YY18(((_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66->hd=((_tmpA69=
_cycalloc(sizeof(*_tmpA69)),((_tmpA69->r=(void*)((_tmpA67=_cycalloc(sizeof(*
_tmpA67)),((_tmpA67[0]=((_tmpA68.tag=13,((_tmpA68.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA68.f2=exs,_tmpA68)))))),
_tmpA67)))),((_tmpA69->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA69)))))),((
_tmpA66->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA66)))))));}break;}case 9: _LL23C:{struct Cyc_Absyn_Decl*
_tmpA6C;struct Cyc_List_List*_tmpA6B;yyval=Cyc_YY18(((_tmpA6B=_cycalloc(sizeof(*
_tmpA6B)),((_tmpA6B->hd=((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C->r=(void*)&
Cyc_Absyn_Porton_d_val,((_tmpA6C->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA6C)))))),((
_tmpA6B->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA6B)))))));}break;case 10: _LL23D:{struct Cyc_Absyn_Decl*
_tmpA6F;struct Cyc_List_List*_tmpA6E;yyval=Cyc_YY18(((_tmpA6E=_cycalloc(sizeof(*
_tmpA6E)),((_tmpA6E->hd=((_tmpA6F=_cycalloc(sizeof(*_tmpA6F)),((_tmpA6F->r=(void*)&
Cyc_Absyn_Portoff_d_val,((_tmpA6F->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA6F)))))),((
_tmpA6E->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA6E)))))));}break;case 11: _LL23E: yyval=Cyc_YY18(0);break;case
12: _LL23F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 13: _LL240: yyval=Cyc_YY52(0);break;case 14: _LL241:{struct _tuple25*
_tmpA72;struct Cyc_List_List*_tmpA71;yyval=Cyc_YY52(((_tmpA71=_cycalloc(sizeof(*
_tmpA71)),((_tmpA71->hd=((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpA72->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpA72->f3=0,_tmpA72)))))))),((_tmpA71->tl=0,_tmpA71)))))));}break;case 15:
_LL242:{struct _tuple25*_tmpA75;struct Cyc_List_List*_tmpA74;yyval=Cyc_YY52(((
_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74->hd=((_tmpA75=_cycalloc(sizeof(*
_tmpA75)),((_tmpA75->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpA75->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpA75->f3=0,_tmpA75)))))))),((_tmpA74->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA74)))))));}break;case
16: _LL243:{struct Cyc_Absyn_Fn_d_struct*_tmpA7B;struct Cyc_Absyn_Fn_d_struct
_tmpA7A;struct Cyc_List_List*_tmpA79;yyval=Cyc_YY18(((_tmpA79=_cycalloc(sizeof(*
_tmpA79)),((_tmpA79->hd=Cyc_Absyn_new_decl((void*)((_tmpA7B=_cycalloc(sizeof(*
_tmpA7B)),((_tmpA7B[0]=((_tmpA7A.tag=1,((_tmpA7A.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA7A)))),_tmpA7B)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA79->tl=0,
_tmpA79)))))));}break;case 17: _LL244: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 18: _LL245: yyval=Cyc_YY18(0);break;case 19: _LL246:
yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 20:
_LL247:{struct Cyc_Core_Opt*_tmpA7C;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA7C)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 21:
_LL248: yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 22:
_LL249:{struct Cyc_Core_Opt*_tmpA7D;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA7D)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 23:
_LL24A:{struct Cyc_Core_Opt*_tmpA7E;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA7E)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 24:
_LL24B:{struct Cyc_Core_Opt*_tmpA7F;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA7F)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 25:
_LL24C: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL24D: Cyc_Lex_leave_using();
break;case 27: _LL24E:{struct _dyneither_ptr*_tmpA80;Cyc_Lex_enter_namespace(((
_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA80)))));}yyval=yyvs[
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
_LL252:{struct Cyc_List_List*_tmpA81;yyval=Cyc_YY18(((_tmpA81=_cycalloc(sizeof(*
_tmpA81)),((_tmpA81->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA81->tl=0,
_tmpA81)))))));}break;case 32: _LL253: {struct Cyc_List_List*_tmp4D6=0;{struct Cyc_List_List*
_tmp4D7=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp4D7 != 0;_tmp4D7=_tmp4D7->tl){struct _dyneither_ptr*_tmp4D8=(struct
_dyneither_ptr*)_tmp4D7->hd;struct _tuple1*_tmpA82;struct _tuple1*qv=(_tmpA82=
_cycalloc(sizeof(*_tmpA82)),((_tmpA82->f1=Cyc_Absyn_Rel_n(0),((_tmpA82->f2=
_tmp4D8,_tmpA82)))));struct Cyc_Absyn_Vardecl*_tmp4D9=Cyc_Absyn_new_vardecl(qv,
Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*_tmpA83;_tmp4D6=((_tmpA83=_cycalloc(
sizeof(*_tmpA83)),((_tmpA83->hd=_tmp4D9,((_tmpA83->tl=_tmp4D6,_tmpA83))))));}}
_tmp4D6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4D6);{struct Cyc_List_List*_tmpA84;yyval=Cyc_YY18(((_tmpA84=_cycalloc(sizeof(*
_tmpA84)),((_tmpA84->hd=Cyc_Absyn_letv_decl(_tmp4D6,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpA84->tl=0,
_tmpA84)))))));}break;}case 33: _LL254:{const char*_tmpA85;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpA85="`H",_tag_dyneither(_tmpA85,sizeof(char),3))))== 0){
const char*_tmpA88;void*_tmpA87;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA87=0,
Cyc_aprintf(((_tmpA88="bad occurrence of heap region",_tag_dyneither(_tmpA88,
sizeof(char),30))),_tag_dyneither(_tmpA87,sizeof(void*),0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{const char*
_tmpA89;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA89="`U",
_tag_dyneither(_tmpA89,sizeof(char),3))))== 0){const char*_tmpA8C;void*_tmpA8B;
Cyc_Parse_err((struct _dyneither_ptr)((_tmpA8B=0,Cyc_aprintf(((_tmpA8C="bad occurrence of unique region",
_tag_dyneither(_tmpA8C,sizeof(char),32))),_tag_dyneither(_tmpA8B,sizeof(void*),0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmpA8F;struct Cyc_Absyn_Tvar*
_tmpA8E;struct Cyc_Absyn_Tvar*tv=(_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->name=((
_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpA8F)))),((_tmpA8E->identity=
- 1,((_tmpA8E->kind=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_RgnKind),_tmpA8E)))))));
struct Cyc_Absyn_VarType_struct _tmpA92;struct Cyc_Absyn_VarType_struct*_tmpA91;
void*t=(void*)((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91[0]=((_tmpA92.tag=2,((
_tmpA92.f1=tv,_tmpA92)))),_tmpA91))));struct Cyc_Absyn_RgnHandleType_struct
_tmpA98;struct Cyc_Absyn_RgnHandleType_struct*_tmpA97;struct _dyneither_ptr*
_tmpA95;struct _tuple1*_tmpA94;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94->f1=Cyc_Absyn_Loc_n,((_tmpA94->f2=((
_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA95)))),_tmpA94)))))),(
void*)((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97[0]=((_tmpA98.tag=16,((
_tmpA98.f1=(void*)t,_tmpA98)))),_tmpA97)))),0);{struct Cyc_List_List*_tmpA99;
yyval=Cyc_YY18(((_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpA99->tl=0,_tmpA99)))))));}break;}case 34: _LL255:{
const char*_tmpA9A;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA9A="H",
_tag_dyneither(_tmpA9A,sizeof(char),2))))== 0){const char*_tmpA9B;Cyc_Parse_err(((
_tmpA9B="bad occurrence of heap region `H",_tag_dyneither(_tmpA9B,sizeof(char),
33))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}{const char*_tmpA9C;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpA9C="U",_tag_dyneither(_tmpA9C,sizeof(char),2))))== 0){
const char*_tmpA9D;Cyc_Parse_err(((_tmpA9D="bad occurrence of unique region `U",
_tag_dyneither(_tmpA9D,sizeof(char),35))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));}}if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){const char*
_tmpA9E;Cyc_Parse_err(((_tmpA9E="open regions cannot be @resetable",
_tag_dyneither(_tmpA9E,sizeof(char),34))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{struct
_dyneither_ptr*_tmpAAB;const char*_tmpAAA;void*_tmpAA9[1];struct Cyc_String_pa_struct
_tmpAA8;struct Cyc_Absyn_Tvar*_tmpAA7;struct Cyc_Absyn_Tvar*tv=(_tmpAA7=_cycalloc(
sizeof(*_tmpAA7)),((_tmpAA7->name=((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((
_tmpAAB[0]=(struct _dyneither_ptr)((_tmpAA8.tag=0,((_tmpAA8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpAA9[0]=& _tmpAA8,
Cyc_aprintf(((_tmpAAA="`%s",_tag_dyneither(_tmpAAA,sizeof(char),4))),
_tag_dyneither(_tmpAA9,sizeof(void*),1)))))))),_tmpAAB)))),((_tmpAA7->identity=-
1,((_tmpAA7->kind=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_RgnKind),_tmpAA7)))))));
struct Cyc_Absyn_VarType_struct _tmpAAE;struct Cyc_Absyn_VarType_struct*_tmpAAD;
void*t=(void*)((_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD[0]=((_tmpAAE.tag=2,((
_tmpAAE.f1=tv,_tmpAAE)))),_tmpAAD))));struct Cyc_Absyn_RgnHandleType_struct
_tmpAB4;struct Cyc_Absyn_RgnHandleType_struct*_tmpAB3;struct _dyneither_ptr*
_tmpAB1;struct _tuple1*_tmpAB0;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0->f1=Cyc_Absyn_Loc_n,((_tmpAB0->f2=((
_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAB1)))),_tmpAB0)))))),(
void*)((_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3[0]=((_tmpAB4.tag=16,((
_tmpAB4.f1=(void*)t,_tmpAB4)))),_tmpAB3)))),0);{struct Cyc_List_List*_tmpAB5;
yyval=Cyc_YY18(((_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAB5->tl=0,
_tmpAB5)))))));}break;}case 35: _LL256: {struct _dyneither_ptr*_tmpABD;struct Cyc_Absyn_Eq_kb_struct*
_tmpABC;struct Cyc_Absyn_Eq_kb_struct _tmpABB;struct Cyc_Absyn_Tvar*_tmpABA;struct
Cyc_Absyn_Tvar*tv=(_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA->name=((_tmpABD=
_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpABD)))),((_tmpABA->identity=
- 1,((_tmpABA->kind=(void*)((_tmpABC=_cycalloc_atomic(sizeof(*_tmpABC)),((_tmpABC[
0]=((_tmpABB.tag=0,((_tmpABB.f1=Cyc_Absyn_RgnKind,_tmpABB)))),_tmpABC)))),
_tmpABA)))))));struct _dyneither_ptr*_tmpAC0;struct _tuple1*_tmpABF;struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(((_tmpABF=_cycalloc(sizeof(*_tmpABF)),((_tmpABF->f1=Cyc_Absyn_Loc_n,((
_tmpABF->f2=((_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpAC0)))),_tmpABF)))))),(
void*)& Cyc_Absyn_VoidType_val,0);{struct Cyc_List_List*_tmpAC1;yyval=Cyc_YY18(((
_tmpAC1=_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAC1->tl=0,
_tmpAC1)))))));}break;}case 36: _LL257: yyval=Cyc_YY31(0);break;case 37: _LL258: yyval=
Cyc_YY31(1);break;case 38: _LL259: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 39: _LL25A: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 40: _LL25B:{
struct Cyc_Core_Opt*_tmpAC4;struct Cyc_Parse_Declaration_spec*_tmpAC3;yyval=Cyc_YY19(((
_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3->sc=((_tmpAC4=_cycalloc_atomic(
sizeof(*_tmpAC4)),((_tmpAC4->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAC4)))),((_tmpAC3->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAC3->type_specs=
0,((_tmpAC3->is_inline=0,((_tmpAC3->attributes=0,_tmpAC3)))))))))))));}break;
case 41: _LL25C: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmpAC7;void*_tmpAC6;(_tmpAC6=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpAC7="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmpAC7,sizeof(char),51))),_tag_dyneither(_tmpAC6,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmpACA;struct Cyc_Parse_Declaration_spec*_tmpAC9;yyval=Cyc_YY19(((
_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9->sc=((_tmpACA=_cycalloc_atomic(
sizeof(*_tmpACA)),((_tmpACA->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpACA)))),((_tmpAC9->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAC9->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAC9->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAC9->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmpAC9)))))))))))));}break;case 42: _LL25D:{struct Cyc_List_List*_tmpACD;struct Cyc_Parse_Declaration_spec*
_tmpACC;yyval=Cyc_YY19(((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC->sc=0,((
_tmpACC->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpACC->type_specs=((
_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpACD->tl=0,_tmpACD)))))),((
_tmpACC->is_inline=0,((_tmpACC->attributes=0,_tmpACC)))))))))))));}break;case 43:
_LL25E:{struct Cyc_List_List*_tmpAD0;struct Cyc_Parse_Declaration_spec*_tmpACF;
yyval=Cyc_YY19(((_tmpACF=_cycalloc(sizeof(*_tmpACF)),((_tmpACF->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpACF->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpACF->type_specs=((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpAD0->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,_tmpAD0)))))),((
_tmpACF->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpACF->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpACF)))))))))))));}
break;case 44: _LL25F:{struct Cyc_Parse_Declaration_spec*_tmpAD1;yyval=Cyc_YY19(((
_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->sc=0,((_tmpAD1->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAD1->type_specs=
0,((_tmpAD1->is_inline=0,((_tmpAD1->attributes=0,_tmpAD1)))))))))))));}break;
case 45: _LL260:{struct Cyc_Parse_Declaration_spec*_tmpAD2;yyval=Cyc_YY19(((_tmpAD2=
_cycalloc(sizeof(*_tmpAD2)),((_tmpAD2->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAD2->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpAD2->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpAD2->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpAD2->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAD2)))))))))))));}
break;case 46: _LL261:{struct Cyc_Parse_Declaration_spec*_tmpAD3;yyval=Cyc_YY19(((
_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3->sc=0,((_tmpAD3->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAD3->type_specs=0,((_tmpAD3->is_inline=1,((
_tmpAD3->attributes=0,_tmpAD3)))))))))))));}break;case 47: _LL262:{struct Cyc_Parse_Declaration_spec*
_tmpAD4;yyval=Cyc_YY19(((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAD4->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAD4->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAD4->is_inline=1,((_tmpAD4->attributes=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAD4)))))))))))));}
break;case 48: _LL263:{struct Cyc_Parse_Declaration_spec*_tmpAD5;yyval=Cyc_YY19(((
_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->sc=0,((_tmpAD5->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAD5->type_specs=0,((_tmpAD5->is_inline=0,((
_tmpAD5->attributes=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpAD5)))))))))))));}break;case 49: _LL264:{struct Cyc_Parse_Declaration_spec*
_tmpAD6;yyval=Cyc_YY19(((_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAD6->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAD6->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpAD6->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAD6->attributes=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes),_tmpAD6)))))))))))));}
break;case 50: _LL265: yyval=Cyc_YY22(Cyc_Parse_Auto_sc);break;case 51: _LL266: yyval=
Cyc_YY22(Cyc_Parse_Register_sc);break;case 52: _LL267: yyval=Cyc_YY22(Cyc_Parse_Static_sc);
break;case 53: _LL268: yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 54: _LL269:{
const char*_tmpAD7;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAD7="C",
_tag_dyneither(_tmpAD7,sizeof(char),2))))!= 0){const char*_tmpAD8;Cyc_Parse_err(((
_tmpAD8="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAD8,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}yyval=Cyc_YY22(Cyc_Parse_ExternC_sc);break;case 55:
_LL26A: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);break;case 56: _LL26B: yyval=Cyc_YY22(
Cyc_Parse_Abstract_sc);break;case 57: _LL26C: yyval=Cyc_YY45(0);break;case 58: _LL26D:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 59:
_LL26E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 60: _LL26F:{struct Cyc_List_List*_tmpAD9;yyval=Cyc_YY45(((_tmpAD9=_cycalloc(
sizeof(*_tmpAD9)),((_tmpAD9->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAD9->tl=0,_tmpAD9)))))));}
break;case 61: _LL270:{struct Cyc_List_List*_tmpADA;yyval=Cyc_YY45(((_tmpADA=
_cycalloc(sizeof(*_tmpADA)),((_tmpADA->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpADA->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpADA)))))));}break;
case 62: _LL271: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};
static struct _tuple26 att_map[17]={{{_tmp51A,_tmp51A,_tmp51A + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
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
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmpADB;Cyc_Parse_err(((
_tmpADB="unrecognized attribute",_tag_dyneither(_tmpADB,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=Cyc_YY46((
void*)& Cyc_Absyn_Cdecl_att_val);}break;}}case 63: _LL272: yyval=Cyc_YY46((void*)&
Cyc_Absyn_Const_att_val);break;case 64: _LL273: {struct _dyneither_ptr _tmp52C=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp52E;struct
_tuple6 _tmp52D=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp52E=_tmp52D.f2;{void*a;{const char*_tmpADD;const char*
_tmpADC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,((_tmpADD="regparm",
_tag_dyneither(_tmpADD,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52C,((_tmpADC="__regparm__",_tag_dyneither(_tmpADC,sizeof(
char),12))))== 0){if(_tmp52E < 0  || _tmp52E > 3){const char*_tmpADE;Cyc_Parse_err(((
_tmpADE="regparm requires value between 0 and 3",_tag_dyneither(_tmpADE,sizeof(
char),39))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct _tmpAE1;
struct Cyc_Absyn_Regparm_att_struct*_tmpAE0;a=(void*)((_tmpAE0=_cycalloc_atomic(
sizeof(*_tmpAE0)),((_tmpAE0[0]=((_tmpAE1.tag=0,((_tmpAE1.f1=_tmp52E,_tmpAE1)))),
_tmpAE0))));}}else{const char*_tmpAE3;const char*_tmpAE2;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52C,((_tmpAE3="aligned",_tag_dyneither(_tmpAE3,sizeof(char),8))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,((_tmpAE2="__aligned__",
_tag_dyneither(_tmpAE2,sizeof(char),12))))== 0){if(_tmp52E < 0){const char*_tmpAE4;
Cyc_Parse_err(((_tmpAE4="aligned requires positive power of two",_tag_dyneither(
_tmpAE4,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp52E;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpAE5;Cyc_Parse_err(((_tmpAE5="aligned requires positive power of two",
_tag_dyneither(_tmpAE5,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpAE8;struct Cyc_Absyn_Aligned_att_struct*_tmpAE7;a=(void*)((_tmpAE7=
_cycalloc_atomic(sizeof(*_tmpAE7)),((_tmpAE7[0]=((_tmpAE8.tag=6,((_tmpAE8.f1=
_tmp52E,_tmpAE8)))),_tmpAE7))));}}}else{const char*_tmpAEA;const char*_tmpAE9;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,((_tmpAEA="initializes",_tag_dyneither(
_tmpAEA,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,((
_tmpAE9="__initializes__",_tag_dyneither(_tmpAE9,sizeof(char),16))))== 0){struct
Cyc_Absyn_Initializes_att_struct _tmpAED;struct Cyc_Absyn_Initializes_att_struct*
_tmpAEC;a=(void*)((_tmpAEC=_cycalloc_atomic(sizeof(*_tmpAEC)),((_tmpAEC[0]=((
_tmpAED.tag=20,((_tmpAED.f1=_tmp52E,_tmpAED)))),_tmpAEC))));}else{{const char*
_tmpAEE;Cyc_Parse_err(((_tmpAEE="unrecognized attribute",_tag_dyneither(_tmpAEE,
sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}}}yyval=Cyc_YY46(a);break;}}case 65: _LL274: {struct
_dyneither_ptr _tmp53F=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp540=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpAF0;const char*_tmpAEF;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53F,((_tmpAF0="section",
_tag_dyneither(_tmpAF0,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp53F,((_tmpAEF="__section__",_tag_dyneither(_tmpAEF,sizeof(
char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpAF3;struct Cyc_Absyn_Section_att_struct*
_tmpAF2;a=(void*)((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2[0]=((_tmpAF3.tag=
8,((_tmpAF3.f1=_tmp540,_tmpAF3)))),_tmpAF2))));}else{{const char*_tmpAF4;Cyc_Parse_err(((
_tmpAF4="unrecognized attribute",_tag_dyneither(_tmpAF4,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 66: _LL275: {struct
_dyneither_ptr _tmp546=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp547=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpAF5;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp546,((_tmpAF5="__mode__",
_tag_dyneither(_tmpAF5,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpAF8;struct Cyc_Absyn_Mode_att_struct*_tmpAF7;a=(void*)((_tmpAF7=_cycalloc(
sizeof(*_tmpAF7)),((_tmpAF7[0]=((_tmpAF8.tag=22,((_tmpAF8.f1=_tmp547,_tmpAF8)))),
_tmpAF7))));}else{{const char*_tmpAF9;Cyc_Parse_err(((_tmpAF9="unrecognized attribute",
_tag_dyneither(_tmpAF9,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 67: _LL276: {struct
_dyneither_ptr _tmp54C=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp54D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp54F;struct _tuple6
_tmp54E=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp54F=_tmp54E.f2;{int _tmp551;struct _tuple6 _tmp550=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp551=_tmp550.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;{const char*_tmpAFB;const char*_tmpAFA;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp54C,((_tmpAFB="format",_tag_dyneither(
_tmpAFB,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp54C,((
_tmpAFA="__format__",_tag_dyneither(_tmpAFA,sizeof(char),11))))== 0){const char*
_tmpAFC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54D,((_tmpAFC="printf",
_tag_dyneither(_tmpAFC,sizeof(char),7))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpAFF;struct Cyc_Absyn_Format_att_struct*_tmpAFE;a=(void*)((_tmpAFE=
_cycalloc_atomic(sizeof(*_tmpAFE)),((_tmpAFE[0]=((_tmpAFF.tag=19,((_tmpAFF.f1=
Cyc_Absyn_Printf_ft,((_tmpAFF.f2=_tmp54F,((_tmpAFF.f3=_tmp551,_tmpAFF)))))))),
_tmpAFE))));}else{const char*_tmpB00;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54D,((
_tmpB00="scanf",_tag_dyneither(_tmpB00,sizeof(char),6))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpB03;struct Cyc_Absyn_Format_att_struct*_tmpB02;a=(void*)((_tmpB02=
_cycalloc_atomic(sizeof(*_tmpB02)),((_tmpB02[0]=((_tmpB03.tag=19,((_tmpB03.f1=
Cyc_Absyn_Scanf_ft,((_tmpB03.f2=_tmp54F,((_tmpB03.f3=_tmp551,_tmpB03)))))))),
_tmpB02))));}else{const char*_tmpB04;Cyc_Parse_err(((_tmpB04="unrecognized format type",
_tag_dyneither(_tmpB04,sizeof(char),25))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpB05;Cyc_Parse_err(((_tmpB05="unrecognized attribute",
_tag_dyneither(_tmpB05,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY46(
a);break;}}}case 68: _LL277: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 69: _LL278:{struct Cyc_Absyn_TypedefType_struct _tmpB08;
struct Cyc_Absyn_TypedefType_struct*_tmpB07;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB07=_cycalloc(sizeof(*_tmpB07)),((_tmpB07[0]=((_tmpB08.tag=18,((
_tmpB08.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB08.f2=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB08.f3=0,((_tmpB08.f4=
0,_tmpB08)))))))))),_tmpB07)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 70:
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
_LL27D:{struct Cyc_Parse_Short_spec_struct _tmpB0B;struct Cyc_Parse_Short_spec_struct*
_tmpB0A;yyval=Cyc_YY23((void*)((_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A[0]=((
_tmpB0B.tag=2,((_tmpB0B.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB0B)))),
_tmpB0A)))));}break;case 75: _LL27E: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 76: _LL27F:{struct Cyc_Parse_Long_spec_struct
_tmpB0E;struct Cyc_Parse_Long_spec_struct*_tmpB0D;yyval=Cyc_YY23((void*)((_tmpB0D=
_cycalloc(sizeof(*_tmpB0D)),((_tmpB0D[0]=((_tmpB0E.tag=3,((_tmpB0E.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB0E)))),
_tmpB0D)))));}break;case 77: _LL280: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 78: _LL281: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 79:
_LL282:{struct Cyc_Parse_Signed_spec_struct _tmpB11;struct Cyc_Parse_Signed_spec_struct*
_tmpB10;yyval=Cyc_YY23((void*)((_tmpB10=_cycalloc(sizeof(*_tmpB10)),((_tmpB10[0]=((
_tmpB11.tag=0,((_tmpB11.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB11)))),
_tmpB10)))));}break;case 80: _LL283:{struct Cyc_Parse_Unsigned_spec_struct _tmpB14;
struct Cyc_Parse_Unsigned_spec_struct*_tmpB13;yyval=Cyc_YY23((void*)((_tmpB13=
_cycalloc(sizeof(*_tmpB13)),((_tmpB13[0]=((_tmpB14.tag=1,((_tmpB14.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB14)))),
_tmpB13)))));}break;case 81: _LL284: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 82: _LL285: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 83: _LL286: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL287:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 85:
_LL288:{struct Cyc_Absyn_TupleType_struct _tmpB17;struct Cyc_Absyn_TupleType_struct*
_tmpB16;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB16=_cycalloc(sizeof(*
_tmpB16)),((_tmpB16[0]=((_tmpB17.tag=11,((_tmpB17.f1=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpB17)))),_tmpB16)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 86: _LL289:{struct Cyc_Absyn_RgnHandleType_struct
_tmpB1A;struct Cyc_Absyn_RgnHandleType_struct*_tmpB19;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19[0]=((_tmpB1A.tag=16,((
_tmpB1A.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpB1A)))),_tmpB19)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 87:
_LL28A:{struct Cyc_Absyn_RgnHandleType_struct _tmpB1D;struct Cyc_Absyn_RgnHandleType_struct*
_tmpB1C;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB1C=_cycalloc(sizeof(*
_tmpB1C)),((_tmpB1C[0]=((_tmpB1D.tag=16,((_tmpB1D.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0),_tmpB1D)))),_tmpB1C)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 88:
_LL28B: {void*_tmp56C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);{
struct Cyc_Absyn_DynRgnType_struct _tmpB20;struct Cyc_Absyn_DynRgnType_struct*
_tmpB1F;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB1F=_cycalloc(sizeof(*
_tmpB1F)),((_tmpB1F[0]=((_tmpB20.tag=17,((_tmpB20.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB20.f2=(void*)
_tmp56C,_tmpB20)))))),_tmpB1F)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 89:
_LL28C:{struct Cyc_Absyn_DynRgnType_struct _tmpB23;struct Cyc_Absyn_DynRgnType_struct*
_tmpB22;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB22=_cycalloc(sizeof(*
_tmpB22)),((_tmpB22[0]=((_tmpB23.tag=17,((_tmpB23.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB23.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB23)))))),
_tmpB22)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 90: _LL28D:{struct Cyc_Absyn_TagType_struct
_tmpB26;struct Cyc_Absyn_TagType_struct*_tmpB25;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25[0]=((_tmpB26.tag=20,((
_tmpB26.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpB26)))),_tmpB25)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 91:
_LL28E:{struct Cyc_Absyn_TagType_struct _tmpB29;struct Cyc_Absyn_TagType_struct*
_tmpB28;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB28=_cycalloc(sizeof(*
_tmpB28)),((_tmpB28[0]=((_tmpB29.tag=20,((_tmpB29.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,0),_tmpB29)))),_tmpB28)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 92:
_LL28F:{struct Cyc_Absyn_ValueofType_struct _tmpB2C;struct Cyc_Absyn_ValueofType_struct*
_tmpB2B;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB2B=_cycalloc(sizeof(*
_tmpB2B)),((_tmpB2B[0]=((_tmpB2C.tag=19,((_tmpB2C.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB2C)))),_tmpB2B)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 93: _LL290: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 94: _LL291: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;{struct Cyc_Absyn_Tqual
_tmpB2D;yyval=Cyc_YY25(((_tmpB2D.print_const=1,((_tmpB2D.q_volatile=0,((_tmpB2D.q_restrict=
0,((_tmpB2D.real_const=1,((_tmpB2D.loc=loc,_tmpB2D)))))))))));}break;}case 95:
_LL292:{struct Cyc_Absyn_Tqual _tmpB2E;yyval=Cyc_YY25(((_tmpB2E.print_const=0,((
_tmpB2E.q_volatile=1,((_tmpB2E.q_restrict=0,((_tmpB2E.real_const=0,((_tmpB2E.loc=
0,_tmpB2E)))))))))));}break;case 96: _LL293:{struct Cyc_Absyn_Tqual _tmpB2F;yyval=
Cyc_YY25(((_tmpB2F.print_const=0,((_tmpB2F.q_volatile=0,((_tmpB2F.q_restrict=1,((
_tmpB2F.real_const=0,((_tmpB2F.loc=0,_tmpB2F)))))))))));}break;case 97: _LL294:{
struct Cyc_Parse_Decl_spec_struct _tmpB4B;struct Cyc_Absyn_Enum_d_struct*_tmpB4A;
struct Cyc_Absyn_Enumdecl*_tmpB49;struct Cyc_Core_Opt*_tmpB48;struct Cyc_Absyn_Enum_d_struct
_tmpB47;struct Cyc_Absyn_Decl*_tmpB46;struct Cyc_Parse_Decl_spec_struct*_tmpB45;
yyval=Cyc_YY23((void*)((_tmpB45=_cycalloc(sizeof(*_tmpB45)),((_tmpB45[0]=((
_tmpB4B.tag=5,((_tmpB4B.f1=((_tmpB46=_cycalloc(sizeof(*_tmpB46)),((_tmpB46->r=(
void*)((_tmpB4A=_cycalloc(sizeof(*_tmpB4A)),((_tmpB4A[0]=((_tmpB47.tag=8,((
_tmpB47.f1=((_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49->sc=Cyc_Absyn_Public,((
_tmpB49->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpB49->fields=((_tmpB48=_cycalloc(sizeof(*_tmpB48)),((
_tmpB48->v=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),_tmpB48)))),_tmpB49)))))))),_tmpB47)))),_tmpB4A)))),((_tmpB46->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB46)))))),
_tmpB4B)))),_tmpB45)))));}break;case 98: _LL295:{struct Cyc_Absyn_EnumType_struct
_tmpB4E;struct Cyc_Absyn_EnumType_struct*_tmpB4D;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpB4D=_cycalloc(sizeof(*_tmpB4D)),((_tmpB4D[0]=((_tmpB4E.tag=14,((
_tmpB4E.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB4E.f2=0,_tmpB4E)))))),_tmpB4D)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 99:
_LL296:{struct Cyc_Parse_Type_spec_struct _tmpB58;struct Cyc_Absyn_AnonEnumType_struct
_tmpB57;struct Cyc_Absyn_AnonEnumType_struct*_tmpB56;struct Cyc_Parse_Type_spec_struct*
_tmpB55;yyval=Cyc_YY23((void*)((_tmpB55=_cycalloc(sizeof(*_tmpB55)),((_tmpB55[0]=((
_tmpB58.tag=4,((_tmpB58.f1=(void*)((void*)((_tmpB56=_cycalloc(sizeof(*_tmpB56)),((
_tmpB56[0]=((_tmpB57.tag=15,((_tmpB57.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB57)))),_tmpB56))))),((
_tmpB58.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB58)))))),_tmpB55)))));}break;case 100: _LL297:{
struct Cyc_Absyn_Enumfield*_tmpB59;yyval=Cyc_YY47(((_tmpB59=_cycalloc(sizeof(*
_tmpB59)),((_tmpB59->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB59->tag=0,((_tmpB59->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB59)))))))));}break;case 101: _LL298:{struct Cyc_Absyn_Enumfield*
_tmpB5A;yyval=Cyc_YY47(((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB5A->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB5A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB5A)))))))));}
break;case 102: _LL299:{struct Cyc_List_List*_tmpB5B;yyval=Cyc_YY48(((_tmpB5B=
_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB5B->tl=0,_tmpB5B)))))));}
break;case 103: _LL29A:{struct Cyc_List_List*_tmpB5C;yyval=Cyc_YY48(((_tmpB5C=
_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB5C->tl=0,_tmpB5C)))))));}
break;case 104: _LL29B:{struct Cyc_List_List*_tmpB5D;yyval=Cyc_YY48(((_tmpB5D=
_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB5D->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB5D)))))));}break;
case 105: _LL29C:{struct Cyc_Absyn_AnonAggrType_struct _tmpB60;struct Cyc_Absyn_AnonAggrType_struct*
_tmpB5F;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB5F=_cycalloc(sizeof(*
_tmpB5F)),((_tmpB5F[0]=((_tmpB60.tag=13,((_tmpB60.f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB60.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB60)))))),
_tmpB5F)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 106: _LL29D: {struct Cyc_List_List*_tmp58E=((
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
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct
_tmpB63;struct Cyc_Parse_Decl_spec_struct*_tmpB62;yyval=Cyc_YY23((void*)((_tmpB62=
_cycalloc(sizeof(*_tmpB62)),((_tmpB62[0]=((_tmpB63.tag=5,((_tmpB63.f1=Cyc_Absyn_aggr_decl(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tmp58E,(struct Cyc_Absyn_AggrdeclImpl*)Cyc_Absyn_aggrdecl_impl(_tmp58F,Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB63)))),
_tmpB62)))));}break;}case 107: _LL29E: {struct Cyc_List_List*_tmp592=((struct Cyc_List_List*(*)(
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
10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct _tmpB66;struct Cyc_Parse_Decl_spec_struct*
_tmpB65;yyval=Cyc_YY23((void*)((_tmpB65=_cycalloc(sizeof(*_tmpB65)),((_tmpB65[0]=((
_tmpB66.tag=5,((_tmpB66.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp592,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp593,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB66)))),
_tmpB65)))));}break;}case 108: _LL29F:{struct Cyc_Absyn_AggrType_struct _tmpB70;
struct Cyc_Core_Opt*_tmpB6F;struct Cyc_Absyn_AggrInfo _tmpB6E;struct Cyc_Absyn_AggrType_struct*
_tmpB6D;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpB6D=_cycalloc(sizeof(*
_tmpB6D)),((_tmpB6D[0]=((_tmpB70.tag=12,((_tmpB70.f1=((_tmpB6E.aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB6F=
_cycalloc_atomic(sizeof(*_tmpB6F)),((_tmpB6F->v=(void*)1,_tmpB6F))))),((_tmpB6E.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB6E)))),
_tmpB70)))),_tmpB6D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 109:
_LL2A0:{struct Cyc_Absyn_AggrType_struct _tmpB76;struct Cyc_Absyn_AggrInfo _tmpB75;
struct Cyc_Absyn_AggrType_struct*_tmpB74;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74[0]=((_tmpB76.tag=12,((_tmpB76.f1=((
_tmpB75.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),((_tmpB75.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB75)))),
_tmpB76)))),_tmpB74)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 110:
_LL2A1: yyval=Cyc_YY40(0);break;case 111: _LL2A2: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 112: _LL2A3:
yyval=Cyc_YY24(Cyc_Absyn_StructA);break;case 113: _LL2A4: yyval=Cyc_YY24(Cyc_Absyn_UnionA);
break;case 114: _LL2A5: yyval=Cyc_YY26(0);break;case 115: _LL2A6: {struct Cyc_List_List*
_tmp59D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));struct Cyc_List_List*
_tmp59E=Cyc_Parse_get_argrfield_tags(_tmp59D);if(_tmp59E != 0)((void(*)(void(*f)(
struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct
Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp59E,
_tmp59D);yyval=Cyc_YY26(_tmp59D);break;}case 116: _LL2A7:{struct Cyc_List_List*
_tmpB77;yyval=Cyc_YY27(((_tmpB77=_cycalloc(sizeof(*_tmpB77)),((_tmpB77->hd=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB77->tl=0,
_tmpB77)))))));}break;case 117: _LL2A8:{struct Cyc_List_List*_tmpB78;yyval=Cyc_YY27(((
_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB78->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB78)))))));}
break;case 118: _LL2A9: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 119: _LL2AA:{struct Cyc_List_List*_tmpB79;yyval=Cyc_YY21(((
_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB79->tl=0,_tmpB79)))))));}
break;case 120: _LL2AB:{struct Cyc_List_List*_tmpB7A;yyval=Cyc_YY21(((_tmpB7A=
_cycalloc(sizeof(*_tmpB7A)),((_tmpB7A->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB7A->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB7A)))))));}
break;case 121: _LL2AC:{struct _tuple22*_tmpB7B;yyval=Cyc_YY20(((_tmpB7B=_cycalloc(
sizeof(*_tmpB7B)),((_tmpB7B->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB7B->f2=0,_tmpB7B)))))));}
break;case 122: _LL2AD:{struct _tuple22*_tmpB7C;yyval=Cyc_YY20(((_tmpB7C=_cycalloc(
sizeof(*_tmpB7C)),((_tmpB7C->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB7C->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB7C)))))));}
break;case 123: _LL2AE: {struct _RegionHandle _tmp5A5=_new_region("temp");struct
_RegionHandle*temp=& _tmp5A5;_push_region(temp);{struct _tuple23 _tmp5A7;struct Cyc_Absyn_Tqual
_tmp5A8;struct Cyc_List_List*_tmp5A9;struct Cyc_List_List*_tmp5AA;struct _tuple23*
_tmp5A6=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5A7=*_tmp5A6;_tmp5A8=_tmp5A7.f1;_tmp5A9=_tmp5A7.f2;_tmp5AA=_tmp5A7.f3;if(
_tmp5A8.loc == 0)_tmp5A8.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);{struct Cyc_List_List*
_tmp5AC;struct Cyc_List_List*_tmp5AD;struct _tuple0 _tmp5AB=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]));_tmp5AC=_tmp5AB.f1;_tmp5AD=_tmp5AB.f2;{void*_tmp5AE=Cyc_Parse_speclist2typ(
_tmp5A9,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp5AF=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(temp,_tmp5A8,_tmp5AE,_tmp5AC,
_tmp5AA),_tmp5AD);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple15*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp5AF));}}}
_npop_handler(0);break;;_pop_region(temp);}case 124: _LL2AF:{struct Cyc_List_List*
_tmpB7F;struct _tuple23*_tmpB7E;yyval=Cyc_YY35(((_tmpB7E=_cycalloc(sizeof(*
_tmpB7E)),((_tmpB7E->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB7E->f2=((
_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB7F->tl=0,_tmpB7F)))))),((
_tmpB7E->f3=0,_tmpB7E)))))))));}break;case 125: _LL2B0:{struct Cyc_List_List*
_tmpB82;struct _tuple23*_tmpB81;yyval=Cyc_YY35(((_tmpB81=_cycalloc(sizeof(*
_tmpB81)),((_tmpB81->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpB81->f2=((_tmpB82=_cycalloc(sizeof(*_tmpB82)),((
_tmpB82->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB82->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB82)))))),((_tmpB81->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB81)))))))));}break;case 126: _LL2B1:{struct _tuple23*_tmpB83;yyval=Cyc_YY35(((
_tmpB83=_cycalloc(sizeof(*_tmpB83)),((_tmpB83->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB83->f2=0,((_tmpB83->f3=
0,_tmpB83)))))))));}break;case 127: _LL2B2:{struct _tuple23*_tmpB84;yyval=Cyc_YY35(((
_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB84->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpB84->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpB84)))))))));}break;case 128: _LL2B3:{struct _tuple23*
_tmpB85;yyval=Cyc_YY35(((_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB85->f2=0,((_tmpB85->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB85)))))))));}break;
case 129: _LL2B4:{struct _tuple23*_tmpB86;yyval=Cyc_YY35(((_tmpB86=_cycalloc(
sizeof(*_tmpB86)),((_tmpB86->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpB86->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB86->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB86)))))))));}break;case 130: _LL2B5:{struct Cyc_List_List*_tmpB89;struct
_tuple23*_tmpB88;yyval=Cyc_YY35(((_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB88->f2=((
_tmpB89=_cycalloc(sizeof(*_tmpB89)),((_tmpB89->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB89->tl=0,_tmpB89)))))),((
_tmpB88->f3=0,_tmpB88)))))))));}break;case 131: _LL2B6:{struct Cyc_List_List*
_tmpB8C;struct _tuple23*_tmpB8B;yyval=Cyc_YY35(((_tmpB8B=_cycalloc(sizeof(*
_tmpB8B)),((_tmpB8B->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpB8B->f2=((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((
_tmpB8C->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB8C->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpB8C)))))),((_tmpB8B->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpB8B)))))))));}break;case 132: _LL2B7:{struct _tuple23*_tmpB8D;yyval=Cyc_YY35(((
_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((_tmpB8D->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB8D->f2=0,((_tmpB8D->f3=
0,_tmpB8D)))))))));}break;case 133: _LL2B8:{struct _tuple23*_tmpB8E;yyval=Cyc_YY35(((
_tmpB8E=_cycalloc(sizeof(*_tmpB8E)),((_tmpB8E->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpB8E->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpB8E->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpB8E)))))))));}break;case 134: _LL2B9:{struct _tuple23*
_tmpB8F;yyval=Cyc_YY35(((_tmpB8F=_cycalloc(sizeof(*_tmpB8F)),((_tmpB8F->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpB8F->f2=0,((_tmpB8F->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB8F)))))))));}break;
case 135: _LL2BA:{struct _tuple23*_tmpB90;yyval=Cyc_YY35(((_tmpB90=_cycalloc(
sizeof(*_tmpB90)),((_tmpB90->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpB90->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpB90->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpB90)))))))));}break;case 136: _LL2BB: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 137: _LL2BC:{
struct Cyc_List_List*_tmpB91;yyval=Cyc_YY21(((_tmpB91=_cycalloc(sizeof(*_tmpB91)),((
_tmpB91->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpB91->tl=0,_tmpB91)))))));}break;case 138: _LL2BD:{struct Cyc_List_List*
_tmpB92;yyval=Cyc_YY21(((_tmpB92=_cycalloc(sizeof(*_tmpB92)),((_tmpB92->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB92->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB92)))))));}
break;case 139: _LL2BE:{struct _tuple22*_tmpB93;yyval=Cyc_YY20(((_tmpB93=_cycalloc(
sizeof(*_tmpB93)),((_tmpB93->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB93->f2=0,_tmpB93)))))));}
break;case 140: _LL2BF:{struct Cyc_Parse_Declarator*_tmpB99;struct _tuple1*_tmpB98;
struct _tuple22*_tmpB97;yyval=Cyc_YY20(((_tmpB97=_cycalloc(sizeof(*_tmpB97)),((
_tmpB97->f1=((_tmpB99=_cycalloc(sizeof(*_tmpB99)),((_tmpB99->id=((_tmpB98=
_cycalloc(sizeof(*_tmpB98)),((_tmpB98->f1=Cyc_Absyn_Rel_n(0),((_tmpB98->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpB98)))))),((_tmpB99->tms=0,_tmpB99)))))),((_tmpB97->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB97)))))));}
break;case 141: _LL2C0:{struct _tuple22*_tmpB9A;yyval=Cyc_YY20(((_tmpB9A=_cycalloc(
sizeof(*_tmpB9A)),((_tmpB9A->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB9A->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB9A)))))));}
break;case 142: _LL2C1: {int _tmp5C8=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);struct Cyc_List_List*
_tmp5C9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct _tmpBA0;struct Cyc_Core_Opt*
_tmpB9F;struct Cyc_Parse_Decl_spec_struct*_tmpB9E;yyval=Cyc_YY23((void*)((_tmpB9E=
_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E[0]=((_tmpBA0.tag=5,((_tmpBA0.f1=Cyc_Absyn_datatype_decl(
Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),_tmp5C9,((_tmpB9F=_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F->v=
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpB9F)))),_tmp5C8,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBA0)))),
_tmpB9E)))));}break;}case 143: _LL2C2: {int _tmp5CD=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);{struct Cyc_Absyn_DatatypeType_struct
_tmpBAA;struct Cyc_Absyn_UnknownDatatypeInfo _tmpBA9;struct Cyc_Absyn_DatatypeInfo
_tmpBA8;struct Cyc_Absyn_DatatypeType_struct*_tmpBA7;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpBA7=_cycalloc(sizeof(*_tmpBA7)),((_tmpBA7[0]=((_tmpBAA.tag=3,((
_tmpBAA.f1=((_tmpBA8.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpBA9.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBA9.is_extensible=
_tmp5CD,_tmpBA9))))),((_tmpBA8.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBA8)))),_tmpBAA)))),
_tmpBA7)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 144: _LL2C3: {int _tmp5D2=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);{struct Cyc_Absyn_DatatypeFieldType_struct
_tmpBB4;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpBB3;struct Cyc_Absyn_DatatypeFieldInfo
_tmpBB2;struct Cyc_Absyn_DatatypeFieldType_struct*_tmpBB1;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpBB1=_cycalloc(sizeof(*_tmpBB1)),((_tmpBB1[0]=((_tmpBB4.tag=4,((
_tmpBB4.f1=((_tmpBB2.field_info=Cyc_Absyn_UnknownDatatypefield(((_tmpBB3.datatype_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpBB3.field_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpBB3.is_extensible=_tmp5D2,_tmpBB3))))))),((
_tmpBB2.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpBB2)))),_tmpBB4)))),_tmpBB1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
145: _LL2C4: yyval=Cyc_YY31(0);break;case 146: _LL2C5: yyval=Cyc_YY31(1);break;case
147: _LL2C6:{struct Cyc_List_List*_tmpBB5;yyval=Cyc_YY34(((_tmpBB5=_cycalloc(
sizeof(*_tmpBB5)),((_tmpBB5->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBB5->tl=0,_tmpBB5)))))));}
break;case 148: _LL2C7:{struct Cyc_List_List*_tmpBB6;yyval=Cyc_YY34(((_tmpBB6=
_cycalloc(sizeof(*_tmpBB6)),((_tmpBB6->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBB6->tl=0,_tmpBB6)))))));}
break;case 149: _LL2C8:{struct Cyc_List_List*_tmpBB7;yyval=Cyc_YY34(((_tmpBB7=
_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBB7->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBB7)))))));}break;
case 150: _LL2C9:{struct Cyc_List_List*_tmpBB8;yyval=Cyc_YY34(((_tmpBB8=_cycalloc(
sizeof(*_tmpBB8)),((_tmpBB8->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBB8->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBB8)))))));}break;
case 151: _LL2CA: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 152: _LL2CB: yyval=Cyc_YY32(
Cyc_Absyn_Extern);break;case 153: _LL2CC: yyval=Cyc_YY32(Cyc_Absyn_Static);break;
case 154: _LL2CD:{struct Cyc_Absyn_Datatypefield*_tmpBB9;yyval=Cyc_YY33(((_tmpBB9=
_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBB9->typs=0,((_tmpBB9->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpBB9->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBB9)))))))))));}
break;case 155: _LL2CE: {struct Cyc_List_List*_tmp5DC=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Datatypefield*
_tmpBBA;yyval=Cyc_YY33(((_tmpBBA=_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpBBA->typs=
_tmp5DC,((_tmpBBA->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpBBA->sc=
Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpBBA)))))))))));}break;}case 156: _LL2CF: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 157: _LL2D0:{struct
Cyc_Parse_Declarator*_tmpBBB;yyval=Cyc_YY29(((_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((
_tmpBBB->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpBBB->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpBBB)))))));}
break;case 158: _LL2D1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 159: _LL2D2:{struct Cyc_Parse_Declarator*_tmpBBC;yyval=
Cyc_YY29(((_tmpBBC=_cycalloc(sizeof(*_tmpBBC)),((_tmpBBC->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpBBC->tms=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpBBC)))))));}
break;case 160: _LL2D3:{struct Cyc_Parse_Declarator*_tmpBBD;yyval=Cyc_YY29(((
_tmpBBD=_cycalloc(sizeof(*_tmpBBD)),((_tmpBBD->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBBD->tms=0,_tmpBBD)))))));}
break;case 161: _LL2D4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 162: _LL2D5: {struct Cyc_Parse_Declarator*_tmp5E1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpBC3;struct Cyc_Absyn_Attributes_mod_struct _tmpBC2;struct Cyc_List_List*_tmpBC1;
_tmp5E1->tms=((_tmpBC1=_cycalloc(sizeof(*_tmpBC1)),((_tmpBC1->hd=(void*)((
_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((_tmpBC3[0]=((_tmpBC2.tag=5,((_tmpBC2.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpBC2.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpBC2)))))),_tmpBC3)))),((
_tmpBC1->tl=_tmp5E1->tms,_tmpBC1))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 163: _LL2D6:{
struct Cyc_List_List*_tmpBCD;struct Cyc_Absyn_Carray_mod_struct _tmpBCC;struct Cyc_Absyn_Carray_mod_struct*
_tmpBCB;struct Cyc_Parse_Declarator*_tmpBCA;yyval=Cyc_YY29(((_tmpBCA=_cycalloc(
sizeof(*_tmpBCA)),((_tmpBCA->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBCA->tms=((
_tmpBCD=_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD->hd=(void*)((_tmpBCB=_cycalloc(
sizeof(*_tmpBCB)),((_tmpBCB[0]=((_tmpBCC.tag=0,((_tmpBCC.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBCC.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBCC)))))),
_tmpBCB)))),((_tmpBCD->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpBCD)))))),_tmpBCA)))))));}break;case 164:
_LL2D7:{struct Cyc_List_List*_tmpBD7;struct Cyc_Absyn_ConstArray_mod_struct _tmpBD6;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpBD5;struct Cyc_Parse_Declarator*_tmpBD4;
yyval=Cyc_YY29(((_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpBD4->tms=((
_tmpBD7=_cycalloc(sizeof(*_tmpBD7)),((_tmpBD7->hd=(void*)((_tmpBD5=_cycalloc(
sizeof(*_tmpBD5)),((_tmpBD5[0]=((_tmpBD6.tag=1,((_tmpBD6.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBD6.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBD6.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBD6)))))))),
_tmpBD5)))),((_tmpBD7->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpBD7)))))),_tmpBD4)))))));}break;case 165:
_LL2D8: {struct _tuple24 _tmp5EE;struct Cyc_List_List*_tmp5EF;int _tmp5F0;struct Cyc_Absyn_VarargInfo*
_tmp5F1;struct Cyc_Core_Opt*_tmp5F2;struct Cyc_List_List*_tmp5F3;struct _tuple24*
_tmp5ED=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5EE=*_tmp5ED;_tmp5EF=_tmp5EE.f1;_tmp5F0=_tmp5EE.f2;_tmp5F1=_tmp5EE.f3;
_tmp5F2=_tmp5EE.f4;_tmp5F3=_tmp5EE.f5;{struct Cyc_List_List*_tmpBEC;struct Cyc_Absyn_Function_mod_struct
_tmpBEB;struct Cyc_Absyn_WithTypes_struct _tmpBEA;struct Cyc_Absyn_WithTypes_struct*
_tmpBE9;struct Cyc_Absyn_Function_mod_struct*_tmpBE8;struct Cyc_Parse_Declarator*
_tmpBE7;yyval=Cyc_YY29(((_tmpBE7=_cycalloc(sizeof(*_tmpBE7)),((_tmpBE7->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBE7->tms=((
_tmpBEC=_cycalloc(sizeof(*_tmpBEC)),((_tmpBEC->hd=(void*)((_tmpBE8=_cycalloc(
sizeof(*_tmpBE8)),((_tmpBE8[0]=((_tmpBEB.tag=3,((_tmpBEB.f1=(void*)((void*)((
_tmpBE9=_cycalloc(sizeof(*_tmpBE9)),((_tmpBE9[0]=((_tmpBEA.tag=1,((_tmpBEA.f1=
_tmp5EF,((_tmpBEA.f2=_tmp5F0,((_tmpBEA.f3=_tmp5F1,((_tmpBEA.f4=_tmp5F2,((_tmpBEA.f5=
_tmp5F3,_tmpBEA)))))))))))),_tmpBE9))))),_tmpBEB)))),_tmpBE8)))),((_tmpBEC->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpBEC)))))),_tmpBE7)))))));}break;}case 166: _LL2D9:{struct Cyc_List_List*_tmpC01;
struct Cyc_Absyn_Function_mod_struct _tmpC00;struct Cyc_Absyn_WithTypes_struct
_tmpBFF;struct Cyc_Absyn_WithTypes_struct*_tmpBFE;struct Cyc_Absyn_Function_mod_struct*
_tmpBFD;struct Cyc_Parse_Declarator*_tmpBFC;yyval=Cyc_YY29(((_tmpBFC=_cycalloc(
sizeof(*_tmpBFC)),((_tmpBFC->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpBFC->tms=((
_tmpC01=_cycalloc(sizeof(*_tmpC01)),((_tmpC01->hd=(void*)((_tmpBFD=_cycalloc(
sizeof(*_tmpBFD)),((_tmpBFD[0]=((_tmpC00.tag=3,((_tmpC00.f1=(void*)((void*)((
_tmpBFE=_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE[0]=((_tmpBFF.tag=1,((_tmpBFF.f1=0,((
_tmpBFF.f2=0,((_tmpBFF.f3=0,((_tmpBFF.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBFF.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBFF)))))))))))),
_tmpBFE))))),_tmpC00)))),_tmpBFD)))),((_tmpC01->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpC01)))))),
_tmpBFC)))))));}break;case 167: _LL2DA:{struct Cyc_List_List*_tmpC16;struct Cyc_Absyn_Function_mod_struct
_tmpC15;struct Cyc_Absyn_NoTypes_struct _tmpC14;struct Cyc_Absyn_NoTypes_struct*
_tmpC13;struct Cyc_Absyn_Function_mod_struct*_tmpC12;struct Cyc_Parse_Declarator*
_tmpC11;yyval=Cyc_YY29(((_tmpC11=_cycalloc(sizeof(*_tmpC11)),((_tmpC11->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC11->tms=((
_tmpC16=_cycalloc(sizeof(*_tmpC16)),((_tmpC16->hd=(void*)((_tmpC12=_cycalloc(
sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC15.tag=3,((_tmpC15.f1=(void*)((void*)((
_tmpC13=_cycalloc(sizeof(*_tmpC13)),((_tmpC13[0]=((_tmpC14.tag=0,((_tmpC14.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpC14.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpC14)))))),_tmpC13))))),_tmpC15)))),_tmpC12)))),((
_tmpC16->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpC16)))))),_tmpC11)))))));}break;case 168: _LL2DB: {
struct Cyc_List_List*_tmp606=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpC20;struct Cyc_Absyn_TypeParams_mod_struct _tmpC1F;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC1E;struct Cyc_Parse_Declarator*_tmpC1D;yyval=Cyc_YY29(((_tmpC1D=_cycalloc(
sizeof(*_tmpC1D)),((_tmpC1D->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC1D->tms=((
_tmpC20=_cycalloc(sizeof(*_tmpC20)),((_tmpC20->hd=(void*)((_tmpC1E=_cycalloc(
sizeof(*_tmpC1E)),((_tmpC1E[0]=((_tmpC1F.tag=4,((_tmpC1F.f1=_tmp606,((_tmpC1F.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpC1F.f3=0,_tmpC1F)))))))),_tmpC1E)))),((_tmpC20->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC20)))))),_tmpC1D)))))));}break;}case 169: _LL2DC:{struct Cyc_List_List*_tmpC2A;
struct Cyc_Absyn_Attributes_mod_struct _tmpC29;struct Cyc_Absyn_Attributes_mod_struct*
_tmpC28;struct Cyc_Parse_Declarator*_tmpC27;yyval=Cyc_YY29(((_tmpC27=_cycalloc(
sizeof(*_tmpC27)),((_tmpC27->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpC27->tms=((
_tmpC2A=_cycalloc(sizeof(*_tmpC2A)),((_tmpC2A->hd=(void*)((_tmpC28=_cycalloc(
sizeof(*_tmpC28)),((_tmpC28[0]=((_tmpC29.tag=5,((_tmpC29.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC29.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC29)))))),_tmpC28)))),((
_tmpC2A->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC2A)))))),_tmpC27)))))));}break;case 170: _LL2DD:{
struct Cyc_Parse_Declarator*_tmpC2B;yyval=Cyc_YY29(((_tmpC2B=_cycalloc(sizeof(*
_tmpC2B)),((_tmpC2B->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC2B->tms=0,_tmpC2B)))))));}break;case 171: _LL2DE:{
struct Cyc_Parse_Declarator*_tmpC2C;yyval=Cyc_YY29(((_tmpC2C=_cycalloc(sizeof(*
_tmpC2C)),((_tmpC2C->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC2C->tms=0,_tmpC2C)))))));}break;case 172: _LL2DF:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 173:
_LL2E0: {struct Cyc_Parse_Declarator*_tmp611=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpC32;struct Cyc_Absyn_Attributes_mod_struct _tmpC31;struct Cyc_List_List*_tmpC30;
_tmp611->tms=((_tmpC30=_cycalloc(sizeof(*_tmpC30)),((_tmpC30->hd=(void*)((
_tmpC32=_cycalloc(sizeof(*_tmpC32)),((_tmpC32[0]=((_tmpC31.tag=5,((_tmpC31.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpC31.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC31)))))),_tmpC32)))),((
_tmpC30->tl=_tmp611->tms,_tmpC30))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 174: _LL2E1:{
struct Cyc_List_List*_tmpC3C;struct Cyc_Absyn_Carray_mod_struct _tmpC3B;struct Cyc_Absyn_Carray_mod_struct*
_tmpC3A;struct Cyc_Parse_Declarator*_tmpC39;yyval=Cyc_YY29(((_tmpC39=_cycalloc(
sizeof(*_tmpC39)),((_tmpC39->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC39->tms=((
_tmpC3C=_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C->hd=(void*)((_tmpC3A=_cycalloc(
sizeof(*_tmpC3A)),((_tmpC3A[0]=((_tmpC3B.tag=0,((_tmpC3B.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC3B.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC3B)))))),
_tmpC3A)))),((_tmpC3C->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpC3C)))))),_tmpC39)))))));}break;case 175:
_LL2E2:{struct Cyc_List_List*_tmpC46;struct Cyc_Absyn_ConstArray_mod_struct _tmpC45;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpC44;struct Cyc_Parse_Declarator*_tmpC43;
yyval=Cyc_YY29(((_tmpC43=_cycalloc(sizeof(*_tmpC43)),((_tmpC43->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC43->tms=((
_tmpC46=_cycalloc(sizeof(*_tmpC46)),((_tmpC46->hd=(void*)((_tmpC44=_cycalloc(
sizeof(*_tmpC44)),((_tmpC44[0]=((_tmpC45.tag=1,((_tmpC45.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC45.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC45.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC45)))))))),
_tmpC44)))),((_tmpC46->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpC46)))))),_tmpC43)))))));}break;case 176:
_LL2E3: {struct _tuple24 _tmp61E;struct Cyc_List_List*_tmp61F;int _tmp620;struct Cyc_Absyn_VarargInfo*
_tmp621;struct Cyc_Core_Opt*_tmp622;struct Cyc_List_List*_tmp623;struct _tuple24*
_tmp61D=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp61E=*_tmp61D;_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;_tmp621=_tmp61E.f3;
_tmp622=_tmp61E.f4;_tmp623=_tmp61E.f5;{struct Cyc_List_List*_tmpC5B;struct Cyc_Absyn_Function_mod_struct
_tmpC5A;struct Cyc_Absyn_WithTypes_struct _tmpC59;struct Cyc_Absyn_WithTypes_struct*
_tmpC58;struct Cyc_Absyn_Function_mod_struct*_tmpC57;struct Cyc_Parse_Declarator*
_tmpC56;yyval=Cyc_YY29(((_tmpC56=_cycalloc(sizeof(*_tmpC56)),((_tmpC56->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC56->tms=((
_tmpC5B=_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B->hd=(void*)((_tmpC57=_cycalloc(
sizeof(*_tmpC57)),((_tmpC57[0]=((_tmpC5A.tag=3,((_tmpC5A.f1=(void*)((void*)((
_tmpC58=_cycalloc(sizeof(*_tmpC58)),((_tmpC58[0]=((_tmpC59.tag=1,((_tmpC59.f1=
_tmp61F,((_tmpC59.f2=_tmp620,((_tmpC59.f3=_tmp621,((_tmpC59.f4=_tmp622,((_tmpC59.f5=
_tmp623,_tmpC59)))))))))))),_tmpC58))))),_tmpC5A)))),_tmpC57)))),((_tmpC5B->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC5B)))))),_tmpC56)))))));}break;}case 177: _LL2E4:{struct Cyc_List_List*_tmpC70;
struct Cyc_Absyn_Function_mod_struct _tmpC6F;struct Cyc_Absyn_WithTypes_struct
_tmpC6E;struct Cyc_Absyn_WithTypes_struct*_tmpC6D;struct Cyc_Absyn_Function_mod_struct*
_tmpC6C;struct Cyc_Parse_Declarator*_tmpC6B;yyval=Cyc_YY29(((_tmpC6B=_cycalloc(
sizeof(*_tmpC6B)),((_tmpC6B->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpC6B->tms=((
_tmpC70=_cycalloc(sizeof(*_tmpC70)),((_tmpC70->hd=(void*)((_tmpC6C=_cycalloc(
sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6F.tag=3,((_tmpC6F.f1=(void*)((void*)((
_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D[0]=((_tmpC6E.tag=1,((_tmpC6E.f1=0,((
_tmpC6E.f2=0,((_tmpC6E.f3=0,((_tmpC6E.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC6E.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC6E)))))))))))),
_tmpC6D))))),_tmpC6F)))),_tmpC6C)))),((_tmpC70->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpC70)))))),
_tmpC6B)))))));}break;case 178: _LL2E5:{struct Cyc_List_List*_tmpC85;struct Cyc_Absyn_Function_mod_struct
_tmpC84;struct Cyc_Absyn_NoTypes_struct _tmpC83;struct Cyc_Absyn_NoTypes_struct*
_tmpC82;struct Cyc_Absyn_Function_mod_struct*_tmpC81;struct Cyc_Parse_Declarator*
_tmpC80;yyval=Cyc_YY29(((_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC80->tms=((
_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85->hd=(void*)((_tmpC81=_cycalloc(
sizeof(*_tmpC81)),((_tmpC81[0]=((_tmpC84.tag=3,((_tmpC84.f1=(void*)((void*)((
_tmpC82=_cycalloc(sizeof(*_tmpC82)),((_tmpC82[0]=((_tmpC83.tag=0,((_tmpC83.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpC83.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpC83)))))),_tmpC82))))),_tmpC84)))),_tmpC81)))),((
_tmpC85->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpC85)))))),_tmpC80)))))));}break;case 179: _LL2E6: {
struct Cyc_List_List*_tmp636=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpC8F;struct Cyc_Absyn_TypeParams_mod_struct _tmpC8E;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpC8D;struct Cyc_Parse_Declarator*_tmpC8C;yyval=Cyc_YY29(((_tmpC8C=_cycalloc(
sizeof(*_tmpC8C)),((_tmpC8C->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpC8C->tms=((
_tmpC8F=_cycalloc(sizeof(*_tmpC8F)),((_tmpC8F->hd=(void*)((_tmpC8D=_cycalloc(
sizeof(*_tmpC8D)),((_tmpC8D[0]=((_tmpC8E.tag=4,((_tmpC8E.f1=_tmp636,((_tmpC8E.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpC8E.f3=0,_tmpC8E)))))))),_tmpC8D)))),((_tmpC8F->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpC8F)))))),_tmpC8C)))))));}break;}case 180: _LL2E7:{struct Cyc_List_List*_tmpC99;
struct Cyc_Absyn_Attributes_mod_struct _tmpC98;struct Cyc_Absyn_Attributes_mod_struct*
_tmpC97;struct Cyc_Parse_Declarator*_tmpC96;yyval=Cyc_YY29(((_tmpC96=_cycalloc(
sizeof(*_tmpC96)),((_tmpC96->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpC96->tms=((
_tmpC99=_cycalloc(sizeof(*_tmpC99)),((_tmpC99->hd=(void*)((_tmpC97=_cycalloc(
sizeof(*_tmpC97)),((_tmpC97[0]=((_tmpC98.tag=5,((_tmpC98.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC98.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC98)))))),_tmpC97)))),((
_tmpC99->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpC99)))))),_tmpC96)))))));}break;case 181: _LL2E8:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 182:
_LL2E9: yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 183: _LL2EA: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpC9F;struct Cyc_Absyn_Attributes_mod_struct _tmpC9E;struct Cyc_List_List*_tmpC9D;
ans=((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->hd=(void*)((_tmpC9F=
_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F[0]=((_tmpC9E.tag=5,((_tmpC9E.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpC9E.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpC9E)))))),_tmpC9F)))),((_tmpC9D->tl=ans,_tmpC9D))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;struct Cyc_Position_Segment*_tmp643;union Cyc_Absyn_Constraint*_tmp644;
union Cyc_Absyn_Constraint*_tmp645;struct _tuple19 _tmp642=*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp643=_tmp642.f1;
_tmp644=_tmp642.f2;_tmp645=_tmp642.f3;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*
_tmpCA0;ptrloc=((_tmpCA0=_cycalloc(sizeof(*_tmpCA0)),((_tmpCA0->ptr_loc=_tmp643,((
_tmpCA0->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpCA0->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),_tmpCA0))))))));}{union Cyc_Absyn_Constraint*
_tmp648;union Cyc_Absyn_Constraint*_tmp649;union Cyc_Absyn_Constraint*_tmp64A;void*
_tmp64B;struct _tuple13 _tmp647=Cyc_Parse_collapse_pointer_quals(_tmp643,_tmp644,
_tmp645,Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY54(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
_tmp648=_tmp647.f1;_tmp649=_tmp647.f2;_tmp64A=_tmp647.f3;_tmp64B=_tmp647.f4;{
struct Cyc_Absyn_Pointer_mod_struct*_tmpCAA;struct Cyc_Absyn_PtrAtts _tmpCA9;struct
Cyc_Absyn_Pointer_mod_struct _tmpCA8;struct Cyc_List_List*_tmpCA7;ans=((_tmpCA7=
_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7->hd=(void*)((_tmpCAA=_cycalloc(sizeof(*
_tmpCAA)),((_tmpCAA[0]=((_tmpCA8.tag=2,((_tmpCA8.f1=((_tmpCA9.rgn=_tmp64B,((
_tmpCA9.nullable=_tmp648,((_tmpCA9.bounds=_tmp649,((_tmpCA9.zero_term=_tmp64A,((
_tmpCA9.ptrloc=ptrloc,_tmpCA9)))))))))),((_tmpCA8.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCA8)))))),_tmpCAA)))),((
_tmpCA7->tl=ans,_tmpCA7))))));}yyval=Cyc_YY28(ans);break;}}}case 184: _LL2EB: yyval=
Cyc_YY54(0);break;case 185: _LL2EC:{struct Cyc_List_List*_tmpCAB;yyval=Cyc_YY54(((
_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->hd=Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCAB->tl=Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCAB)))))));}break;
case 186: _LL2ED:{struct Cyc_Absyn_Numelts_ptrqual_struct _tmpCAE;struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpCAD;yyval=Cyc_YY53((void*)((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD[0]=((
_tmpCAE.tag=0,((_tmpCAE.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpCAE)))),_tmpCAD)))));}break;case 187: _LL2EE:{struct
Cyc_Absyn_Region_ptrqual_struct _tmpCB1;struct Cyc_Absyn_Region_ptrqual_struct*
_tmpCB0;yyval=Cyc_YY53((void*)((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0[0]=((
_tmpCB1.tag=1,((_tmpCB1.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCB1)))),_tmpCB0)))));}
break;case 188: _LL2EF:{struct Cyc_Absyn_Thin_ptrqual_struct _tmpCB4;struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpCB3;yyval=Cyc_YY53((void*)((_tmpCB3=_cycalloc_atomic(sizeof(*_tmpCB3)),((
_tmpCB3[0]=((_tmpCB4.tag=2,_tmpCB4)),_tmpCB3)))));}break;case 189: _LL2F0:{struct
Cyc_Absyn_Fat_ptrqual_struct _tmpCB7;struct Cyc_Absyn_Fat_ptrqual_struct*_tmpCB6;
yyval=Cyc_YY53((void*)((_tmpCB6=_cycalloc_atomic(sizeof(*_tmpCB6)),((_tmpCB6[0]=((
_tmpCB7.tag=3,_tmpCB7)),_tmpCB6)))));}break;case 190: _LL2F1:{struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmpCBA;struct Cyc_Absyn_Zeroterm_ptrqual_struct*_tmpCB9;yyval=Cyc_YY53((void*)((
_tmpCB9=_cycalloc_atomic(sizeof(*_tmpCB9)),((_tmpCB9[0]=((_tmpCBA.tag=4,_tmpCBA)),
_tmpCB9)))));}break;case 191: _LL2F2:{struct Cyc_Absyn_Nozeroterm_ptrqual_struct
_tmpCBD;struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmpCBC;yyval=Cyc_YY53((void*)((
_tmpCBC=_cycalloc_atomic(sizeof(*_tmpCBC)),((_tmpCBC[0]=((_tmpCBD.tag=5,_tmpCBD)),
_tmpCBC)))));}break;case 192: _LL2F3:{struct Cyc_Absyn_Notnull_ptrqual_struct
_tmpCC0;struct Cyc_Absyn_Notnull_ptrqual_struct*_tmpCBF;yyval=Cyc_YY53((void*)((
_tmpCBF=_cycalloc_atomic(sizeof(*_tmpCBF)),((_tmpCBF[0]=((_tmpCC0.tag=6,_tmpCC0)),
_tmpCBF)))));}break;case 193: _LL2F4:{struct Cyc_Absyn_Nullable_ptrqual_struct
_tmpCC3;struct Cyc_Absyn_Nullable_ptrqual_struct*_tmpCC2;yyval=Cyc_YY53((void*)((
_tmpCC2=_cycalloc_atomic(sizeof(*_tmpCC2)),((_tmpCC2[0]=((_tmpCC3.tag=7,_tmpCC3)),
_tmpCC2)))));}break;case 194: _LL2F5: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple19*_tmpCC4;yyval=Cyc_YY1(((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->f1=
loc,((_tmpCC4->f2=Cyc_Absyn_true_conref,((_tmpCC4->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCC4)))))))));}break;}
case 195: _LL2F6: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple19*_tmpCC5;yyval=Cyc_YY1(((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5->f1=
loc,((_tmpCC5->f2=Cyc_Absyn_false_conref,((_tmpCC5->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCC5)))))))));}break;}
case 196: _LL2F7: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{struct _tuple19*
_tmpCC6;yyval=Cyc_YY1(((_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((_tmpCC6->f1=loc,((
_tmpCC6->f2=Cyc_Absyn_true_conref,((_tmpCC6->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpCC6)))))))));}break;}case 197: _LL2F8: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 198: _LL2F9:{struct Cyc_Absyn_Upper_b_struct _tmpCC9;struct Cyc_Absyn_Upper_b_struct*
_tmpCC8;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8[0]=((_tmpCC9.tag=1,((
_tmpCC9.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpCC9)))),_tmpCC8))))));}break;case 199: _LL2FA: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)());break;case 200: _LL2FB: yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;case 201: _LL2FC: yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 202: _LL2FD:
yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));
break;case 203: _LL2FE: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_TopRgnKind,1);
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 204:
_LL2FF: yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));
break;case 205: _LL300: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
206: _LL301: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 207: _LL302:{
struct _tuple24*_tmpCCA;yyval=Cyc_YY39(((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((
_tmpCCA->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpCCA->f2=0,((_tmpCCA->f3=0,((_tmpCCA->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCCA->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCCA)))))))))))));}
break;case 208: _LL303:{struct _tuple24*_tmpCCB;yyval=Cyc_YY39(((_tmpCCB=_cycalloc(
sizeof(*_tmpCCB)),((_tmpCCB->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),((_tmpCCB->f2=1,((_tmpCCB->f3=0,((_tmpCCB->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCCB->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCCB)))))))))))));}
break;case 209: _LL304: {struct _tuple8 _tmp669;struct Cyc_Core_Opt*_tmp66A;struct Cyc_Absyn_Tqual
_tmp66B;void*_tmp66C;struct _tuple8*_tmp668=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp669=*_tmp668;_tmp66A=
_tmp669.f1;_tmp66B=_tmp669.f2;_tmp66C=_tmp669.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCCC;struct Cyc_Absyn_VarargInfo*_tmp66D=(_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((
_tmpCCC->name=_tmp66A,((_tmpCCC->tq=_tmp66B,((_tmpCCC->type=_tmp66C,((_tmpCCC->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCCC)))))))));{struct _tuple24*_tmpCCD;yyval=Cyc_YY39(((_tmpCCD=_cycalloc(
sizeof(*_tmpCCD)),((_tmpCCD->f1=0,((_tmpCCD->f2=0,((_tmpCCD->f3=_tmp66D,((
_tmpCCD->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpCCD->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCCD)))))))))))));}
break;}}case 210: _LL305: {struct _tuple8 _tmp671;struct Cyc_Core_Opt*_tmp672;struct
Cyc_Absyn_Tqual _tmp673;void*_tmp674;struct _tuple8*_tmp670=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp671=*_tmp670;_tmp672=
_tmp671.f1;_tmp673=_tmp671.f2;_tmp674=_tmp671.f3;{struct Cyc_Absyn_VarargInfo*
_tmpCCE;struct Cyc_Absyn_VarargInfo*_tmp675=(_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((
_tmpCCE->name=_tmp672,((_tmpCCE->tq=_tmp673,((_tmpCCE->type=_tmp674,((_tmpCCE->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpCCE)))))))));{struct _tuple24*_tmpCCF;yyval=Cyc_YY39(((_tmpCCF=_cycalloc(
sizeof(*_tmpCCF)),((_tmpCCF->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)])),((_tmpCCF->f2=0,((_tmpCCF->f3=_tmp675,((_tmpCCF->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCCF->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCCF)))))))))))));}
break;}}case 211: _LL306:{struct Cyc_Absyn_Unknown_kb_struct _tmpCD2;struct Cyc_Absyn_Unknown_kb_struct*
_tmpCD1;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)((_tmpCD1=_cycalloc(
sizeof(*_tmpCD1)),((_tmpCD1[0]=((_tmpCD2.tag=1,((_tmpCD2.f1=0,_tmpCD2)))),
_tmpCD1))))));}break;case 212: _LL307: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 213: _LL308: yyval=Cyc_YY49(0);break;case 214: _LL309:{struct Cyc_Absyn_JoinEff_struct*
_tmpCD8;struct Cyc_Absyn_JoinEff_struct _tmpCD7;struct Cyc_Core_Opt*_tmpCD6;yyval=
Cyc_YY49(((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->v=(void*)((_tmpCD8=
_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8[0]=((_tmpCD7.tag=24,((_tmpCD7.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCD7)))),_tmpCD8)))),
_tmpCD6)))));}break;case 215: _LL30A: yyval=Cyc_YY50(0);break;case 216: _LL30B: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 217: _LL30C: {
struct Cyc_Absyn_Less_kb_struct _tmpCDB;struct Cyc_Absyn_Less_kb_struct*_tmpCDA;
struct Cyc_Absyn_Less_kb_struct*_tmp67D=(_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((
_tmpCDA[0]=((_tmpCDB.tag=2,((_tmpCDB.f1=0,((_tmpCDB.f2=Cyc_Absyn_TopRgnKind,
_tmpCDB)))))),_tmpCDA)));struct _dyneither_ptr _tmp67E=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp67F=Cyc_Parse_id2type(
_tmp67E,(void*)_tmp67D);{struct _tuple10*_tmpCE5;struct Cyc_Absyn_JoinEff_struct
_tmpCE4;struct Cyc_Absyn_JoinEff_struct*_tmpCE3;struct Cyc_List_List*_tmpCE2;yyval=
Cyc_YY50(((_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2->hd=((_tmpCE5=_cycalloc(
sizeof(*_tmpCE5)),((_tmpCE5->f1=(void*)((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((
_tmpCE3[0]=((_tmpCE4.tag=24,((_tmpCE4.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCE4)))),_tmpCE3)))),((
_tmpCE5->f2=_tmp67F,_tmpCE5)))))),((_tmpCE2->tl=0,_tmpCE2)))))));}break;}case 218:
_LL30D: {struct Cyc_Absyn_Less_kb_struct _tmpCE8;struct Cyc_Absyn_Less_kb_struct*
_tmpCE7;struct Cyc_Absyn_Less_kb_struct*_tmp686=(_tmpCE7=_cycalloc(sizeof(*
_tmpCE7)),((_tmpCE7[0]=((_tmpCE8.tag=2,((_tmpCE8.f1=0,((_tmpCE8.f2=Cyc_Absyn_TopRgnKind,
_tmpCE8)))))),_tmpCE7)));struct _dyneither_ptr _tmp687=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp688=Cyc_Parse_id2type(
_tmp687,(void*)_tmp686);{struct _tuple10*_tmpCF2;struct Cyc_Absyn_JoinEff_struct
_tmpCF1;struct Cyc_Absyn_JoinEff_struct*_tmpCF0;struct Cyc_List_List*_tmpCEF;yyval=
Cyc_YY50(((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF->hd=((_tmpCF2=_cycalloc(
sizeof(*_tmpCF2)),((_tmpCF2->f1=(void*)((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((
_tmpCF0[0]=((_tmpCF1.tag=24,((_tmpCF1.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpCF1)))),_tmpCF0)))),((
_tmpCF2->f2=_tmp688,_tmpCF2)))))),((_tmpCEF->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCEF)))))));}break;}case
219: _LL30E: yyval=Cyc_YY31(0);break;case 220: _LL30F:{const char*_tmpCF3;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpCF3="inject",_tag_dyneither(_tmpCF3,sizeof(char),7))))
!= 0){const char*_tmpCF4;Cyc_Parse_err(((_tmpCF4="missing type in function declaration",
_tag_dyneither(_tmpCF4,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY31(
1);break;case 221: _LL310: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 222: _LL311: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 223: _LL312:
yyval=Cyc_YY40(0);break;case 224: _LL313: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 225: _LL314:{struct Cyc_Absyn_RgnsEff_struct*
_tmpCFA;struct Cyc_Absyn_RgnsEff_struct _tmpCF9;struct Cyc_List_List*_tmpCF8;yyval=
Cyc_YY40(((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8->hd=(void*)((_tmpCFA=
_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA[0]=((_tmpCF9.tag=25,((_tmpCF9.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpCF9)))),_tmpCFA)))),((_tmpCF8->tl=0,_tmpCF8)))))));}break;case 226: _LL315: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_EffKind,
0);{struct Cyc_List_List*_tmpCFB;yyval=Cyc_YY40(((_tmpCFB=_cycalloc(sizeof(*
_tmpCFB)),((_tmpCFB->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCFB->tl=0,_tmpCFB)))))));}break;case 227: _LL316: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_TopRgnKind,
1);{struct Cyc_Absyn_AccessEff_struct*_tmpD01;struct Cyc_Absyn_AccessEff_struct
_tmpD00;struct Cyc_List_List*_tmpCFF;yyval=Cyc_YY40(((_tmpCFF=_cycalloc(sizeof(*
_tmpCFF)),((_tmpCFF->hd=(void*)((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01[0]=((
_tmpD00.tag=23,((_tmpD00.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD00)))),_tmpD01)))),((
_tmpCFF->tl=0,_tmpCFF)))))));}break;case 228: _LL317: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Absyn_TopRgnKind,
1);{struct Cyc_Absyn_AccessEff_struct*_tmpD07;struct Cyc_Absyn_AccessEff_struct
_tmpD06;struct Cyc_List_List*_tmpD05;yyval=Cyc_YY40(((_tmpD05=_cycalloc(sizeof(*
_tmpD05)),((_tmpD05->hd=(void*)((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07[0]=((
_tmpD06.tag=23,((_tmpD06.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD06)))),_tmpD07)))),((
_tmpD05->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpD05)))))));}break;case 229: _LL318:{struct Cyc_List_List*
_tmpD08;yyval=Cyc_YY38(((_tmpD08=_cycalloc(sizeof(*_tmpD08)),((_tmpD08->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD08->tl=0,
_tmpD08)))))));}break;case 230: _LL319:{struct Cyc_List_List*_tmpD09;yyval=Cyc_YY38(((
_tmpD09=_cycalloc(sizeof(*_tmpD09)),((_tmpD09->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD09->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD09)))))));}
break;case 231: _LL31A: {struct _tuple23 _tmp69E;struct Cyc_Absyn_Tqual _tmp69F;struct
Cyc_List_List*_tmp6A0;struct Cyc_List_List*_tmp6A1;struct _tuple23*_tmp69D=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69E=*_tmp69D;
_tmp69F=_tmp69E.f1;_tmp6A0=_tmp69E.f2;_tmp6A1=_tmp69E.f3;if(_tmp69F.loc == 0)
_tmp69F.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp6A3;struct _tuple1*
_tmp6A4;struct Cyc_List_List*_tmp6A5;struct Cyc_Parse_Declarator*_tmp6A2=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A3=*_tmp6A2;
_tmp6A4=_tmp6A3.id;_tmp6A5=_tmp6A3.tms;{void*_tmp6A6=Cyc_Parse_speclist2typ(
_tmp6A0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp6A8;void*_tmp6A9;struct
Cyc_List_List*_tmp6AA;struct Cyc_List_List*_tmp6AB;struct _tuple12 _tmp6A7=Cyc_Parse_apply_tms(
_tmp69F,_tmp6A6,_tmp6A1,_tmp6A5);_tmp6A8=_tmp6A7.f1;_tmp6A9=_tmp6A7.f2;_tmp6AA=
_tmp6A7.f3;_tmp6AB=_tmp6A7.f4;if(_tmp6AA != 0){const char*_tmpD0A;Cyc_Parse_err(((
_tmpD0A="parameter with bad type params",_tag_dyneither(_tmpD0A,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp6A4)){const char*
_tmpD0B;Cyc_Parse_err(((_tmpD0B="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpD0B,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpD0C;struct Cyc_Core_Opt*_tmp6AE=(struct Cyc_Core_Opt*)((_tmpD0C=_cycalloc(
sizeof(*_tmpD0C)),((_tmpD0C->v=(*_tmp6A4).f2,_tmpD0C))));if(_tmp6AB != 0){const
char*_tmpD0F;void*_tmpD0E;(_tmpD0E=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD0F="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpD0F,sizeof(char),40))),_tag_dyneither(_tmpD0E,sizeof(void*),0)));}{
struct _tuple8*_tmpD10;yyval=Cyc_YY37(((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((
_tmpD10->f1=_tmp6AE,((_tmpD10->f2=_tmp6A8,((_tmpD10->f3=_tmp6A9,_tmpD10)))))))));}
break;}}}}case 232: _LL31B: {struct _tuple23 _tmp6B4;struct Cyc_Absyn_Tqual _tmp6B5;
struct Cyc_List_List*_tmp6B6;struct Cyc_List_List*_tmp6B7;struct _tuple23*_tmp6B3=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B4=*
_tmp6B3;_tmp6B5=_tmp6B4.f1;_tmp6B6=_tmp6B4.f2;_tmp6B7=_tmp6B4.f3;if(_tmp6B5.loc
== 0)_tmp6B5.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{void*_tmp6B8=
Cyc_Parse_speclist2typ(_tmp6B6,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6B7 != 0){
const char*_tmpD13;void*_tmpD12;(_tmpD12=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD13="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD13,sizeof(char),38))),_tag_dyneither(_tmpD12,sizeof(void*),0)));}{
struct _tuple8*_tmpD14;yyval=Cyc_YY37(((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((
_tmpD14->f1=0,((_tmpD14->f2=_tmp6B5,((_tmpD14->f3=_tmp6B8,_tmpD14)))))))));}
break;}}case 233: _LL31C: {struct _tuple23 _tmp6BD;struct Cyc_Absyn_Tqual _tmp6BE;
struct Cyc_List_List*_tmp6BF;struct Cyc_List_List*_tmp6C0;struct _tuple23*_tmp6BC=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6BD=*_tmp6BC;_tmp6BE=_tmp6BD.f1;_tmp6BF=_tmp6BD.f2;_tmp6C0=_tmp6BD.f3;if(
_tmp6BE.loc == 0)_tmp6BE.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp6C1=
Cyc_Parse_speclist2typ(_tmp6BF,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6C2=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp6C4;void*_tmp6C5;struct Cyc_List_List*_tmp6C6;struct Cyc_List_List*
_tmp6C7;struct _tuple12 _tmp6C3=Cyc_Parse_apply_tms(_tmp6BE,_tmp6C1,_tmp6C0,
_tmp6C2);_tmp6C4=_tmp6C3.f1;_tmp6C5=_tmp6C3.f2;_tmp6C6=_tmp6C3.f3;_tmp6C7=
_tmp6C3.f4;if(_tmp6C6 != 0){const char*_tmpD17;void*_tmpD16;(_tmpD16=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpD17="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpD17,sizeof(char),49))),_tag_dyneither(_tmpD16,sizeof(void*),0)));}
if(_tmp6C7 != 0){const char*_tmpD1A;void*_tmpD19;(_tmpD19=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD1A="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpD1A,sizeof(char),38))),_tag_dyneither(_tmpD19,sizeof(void*),0)));}{
struct _tuple8*_tmpD1B;yyval=Cyc_YY37(((_tmpD1B=_cycalloc(sizeof(*_tmpD1B)),((
_tmpD1B->f1=0,((_tmpD1B->f2=_tmp6C4,((_tmpD1B->f3=_tmp6C5,_tmpD1B)))))))));}
break;}}case 234: _LL31D: yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 235: _LL31E:{struct _dyneither_ptr*_tmpD1E;struct Cyc_List_List*
_tmpD1D;yyval=Cyc_YY36(((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->hd=((
_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD1E)))),((_tmpD1D->tl=0,
_tmpD1D)))))));}break;case 236: _LL31F:{struct _dyneither_ptr*_tmpD21;struct Cyc_List_List*
_tmpD20;yyval=Cyc_YY36(((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->hd=((
_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD21)))),((_tmpD20->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpD20)))))));}break;case 237: _LL320: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 238: _LL321: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 239: _LL322:{struct
Cyc_Absyn_UnresolvedMem_e_struct _tmpD24;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD23;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD23=_cycalloc(sizeof(*
_tmpD23)),((_tmpD23[0]=((_tmpD24.tag=37,((_tmpD24.f1=0,((_tmpD24.f2=0,_tmpD24)))))),
_tmpD23)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 240: _LL323:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpD27;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD26;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26[0]=((_tmpD27.tag=37,((
_tmpD27.f1=0,((_tmpD27.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
_tmpD27)))))),_tmpD26)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 241:
_LL324:{struct Cyc_Absyn_UnresolvedMem_e_struct _tmpD2A;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpD29;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD29=_cycalloc(sizeof(*
_tmpD29)),((_tmpD29[0]=((_tmpD2A.tag=37,((_tmpD2A.f1=0,((_tmpD2A.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpD2A)))))),_tmpD29)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 242: _LL325: {struct _dyneither_ptr*
_tmpD2D;struct _tuple1*_tmpD2C;struct Cyc_Absyn_Vardecl*_tmp6D7=Cyc_Absyn_new_vardecl(((
_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->f1=Cyc_Absyn_Loc_n,((_tmpD2C->f2=((
_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpD2D)))),_tmpD2C)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6D7->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct _tmpD30;struct Cyc_Absyn_Comprehension_e_struct*
_tmpD2F;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpD2F=_cycalloc(sizeof(*
_tmpD2F)),((_tmpD2F[0]=((_tmpD30.tag=29,((_tmpD30.f1=_tmp6D7,((_tmpD30.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD30.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD30.f4=0,
_tmpD30)))))))))),_tmpD2F)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
243: _LL326:{struct _tuple27*_tmpD33;struct Cyc_List_List*_tmpD32;yyval=Cyc_YY6(((
_tmpD32=_cycalloc(sizeof(*_tmpD32)),((_tmpD32->hd=((_tmpD33=_cycalloc(sizeof(*
_tmpD33)),((_tmpD33->f1=0,((_tmpD33->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD33)))))),((_tmpD32->tl=
0,_tmpD32)))))));}break;case 244: _LL327:{struct _tuple27*_tmpD36;struct Cyc_List_List*
_tmpD35;yyval=Cyc_YY6(((_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35->hd=((
_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD36->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD36)))))),((
_tmpD35->tl=0,_tmpD35)))))));}break;case 245: _LL328:{struct _tuple27*_tmpD39;
struct Cyc_List_List*_tmpD38;yyval=Cyc_YY6(((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((
_tmpD38->hd=((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->f1=0,((_tmpD39->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD39)))))),((
_tmpD38->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpD38)))))));}break;case 246: _LL329:{struct _tuple27*_tmpD3C;struct Cyc_List_List*
_tmpD3B;yyval=Cyc_YY6(((_tmpD3B=_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B->hd=((
_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD3C->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD3C)))))),((
_tmpD3B->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpD3B)))))));}break;case 247: _LL32A: yyval=Cyc_YY41(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 248: _LL32B:{
struct Cyc_List_List*_tmpD3D;yyval=Cyc_YY41(((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((
_tmpD3D->hd=Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD3D->tl=0,_tmpD3D)))))));}break;case 249: _LL32C:{struct Cyc_List_List*
_tmpD3E;yyval=Cyc_YY41(((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->hd=Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD3E->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD3E)))))));}
break;case 250: _LL32D:{struct Cyc_Absyn_ArrayElement_struct _tmpD41;struct Cyc_Absyn_ArrayElement_struct*
_tmpD40;yyval=Cyc_YY42((void*)((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40[0]=((
_tmpD41.tag=0,((_tmpD41.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpD41)))),_tmpD40)))));}break;case 251: _LL32E:{struct
Cyc_Absyn_FieldName_struct _tmpD47;struct _dyneither_ptr*_tmpD46;struct Cyc_Absyn_FieldName_struct*
_tmpD45;yyval=Cyc_YY42((void*)((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45[0]=((
_tmpD47.tag=1,((_tmpD47.f1=((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpD46)))),_tmpD47)))),_tmpD45)))));}break;case 252: _LL32F: {struct _tuple23
_tmp6EC;struct Cyc_Absyn_Tqual _tmp6ED;struct Cyc_List_List*_tmp6EE;struct Cyc_List_List*
_tmp6EF;struct _tuple23*_tmp6EB=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6EC=*_tmp6EB;_tmp6ED=
_tmp6EC.f1;_tmp6EE=_tmp6EC.f2;_tmp6EF=_tmp6EC.f3;{void*_tmp6F0=Cyc_Parse_speclist2typ(
_tmp6EE,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp6EF != 0){const char*_tmpD4A;void*_tmpD49;(
_tmpD49=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD4A="ignoring attributes in type",
_tag_dyneither(_tmpD4A,sizeof(char),28))),_tag_dyneither(_tmpD49,sizeof(void*),0)));}{
struct _tuple8*_tmpD4B;yyval=Cyc_YY37(((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((
_tmpD4B->f1=0,((_tmpD4B->f2=_tmp6ED,((_tmpD4B->f3=_tmp6F0,_tmpD4B)))))))));}
break;}}case 253: _LL330: {struct _tuple23 _tmp6F5;struct Cyc_Absyn_Tqual _tmp6F6;
struct Cyc_List_List*_tmp6F7;struct Cyc_List_List*_tmp6F8;struct _tuple23*_tmp6F4=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6F5=*_tmp6F4;_tmp6F6=_tmp6F5.f1;_tmp6F7=_tmp6F5.f2;_tmp6F8=_tmp6F5.f3;{void*
_tmp6F9=Cyc_Parse_speclist2typ(_tmp6F7,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6FA=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple12 _tmp6FB=Cyc_Parse_apply_tms(_tmp6F6,_tmp6F9,_tmp6F8,_tmp6FA);if(
_tmp6FB.f3 != 0){const char*_tmpD4E;void*_tmpD4D;(_tmpD4D=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD4E="bad type params, ignoring",
_tag_dyneither(_tmpD4E,sizeof(char),26))),_tag_dyneither(_tmpD4D,sizeof(void*),0)));}
if(_tmp6FB.f4 != 0){const char*_tmpD51;void*_tmpD50;(_tmpD50=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD51="bad specifiers, ignoring",
_tag_dyneither(_tmpD51,sizeof(char),25))),_tag_dyneither(_tmpD50,sizeof(void*),0)));}{
struct _tuple8*_tmpD52;yyval=Cyc_YY37(((_tmpD52=_cycalloc(sizeof(*_tmpD52)),((
_tmpD52->f1=0,((_tmpD52->f2=_tmp6FB.f1,((_tmpD52->f3=_tmp6FB.f2,_tmpD52)))))))));}
break;}}case 254: _LL331: yyval=Cyc_YY44((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);break;case 255: _LL332:{
struct Cyc_Absyn_JoinEff_struct _tmpD55;struct Cyc_Absyn_JoinEff_struct*_tmpD54;
yyval=Cyc_YY44((void*)((_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54[0]=((
_tmpD55.tag=24,((_tmpD55.f1=0,_tmpD55)))),_tmpD54)))));}break;case 256: _LL333:{
struct Cyc_Absyn_JoinEff_struct _tmpD58;struct Cyc_Absyn_JoinEff_struct*_tmpD57;
yyval=Cyc_YY44((void*)((_tmpD57=_cycalloc(sizeof(*_tmpD57)),((_tmpD57[0]=((
_tmpD58.tag=24,((_tmpD58.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpD58)))),_tmpD57)))));}break;case 257: _LL334:{struct
Cyc_Absyn_RgnsEff_struct _tmpD5B;struct Cyc_Absyn_RgnsEff_struct*_tmpD5A;yyval=Cyc_YY44((
void*)((_tmpD5A=_cycalloc(sizeof(*_tmpD5A)),((_tmpD5A[0]=((_tmpD5B.tag=25,((
_tmpD5B.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpD5B)))),_tmpD5A)))));}break;case 258: _LL335:{struct Cyc_Absyn_JoinEff_struct
_tmpD61;struct Cyc_List_List*_tmpD60;struct Cyc_Absyn_JoinEff_struct*_tmpD5F;yyval=
Cyc_YY44((void*)((_tmpD5F=_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F[0]=((_tmpD61.tag=
24,((_tmpD61.f1=((_tmpD60=_cycalloc(sizeof(*_tmpD60)),((_tmpD60->hd=Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD60->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD60)))))),_tmpD61)))),
_tmpD5F)))));}break;case 259: _LL336:{struct Cyc_List_List*_tmpD62;yyval=Cyc_YY40(((
_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD62->tl=0,_tmpD62)))))));}
break;case 260: _LL337:{struct Cyc_List_List*_tmpD63;yyval=Cyc_YY40(((_tmpD63=
_cycalloc(sizeof(*_tmpD63)),((_tmpD63->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD63->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD63)))))));}
break;case 261: _LL338:{struct Cyc_Parse_Abstractdeclarator*_tmpD64;yyval=Cyc_YY30(((
_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD64)))));}break;case 262:
_LL339: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
263: _LL33A:{struct Cyc_Parse_Abstractdeclarator*_tmpD65;yyval=Cyc_YY30(((_tmpD65=
_cycalloc(sizeof(*_tmpD65)),((_tmpD65->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpD65)))));}break;
case 264: _LL33B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 265: _LL33C:{struct Cyc_List_List*_tmpD6F;struct Cyc_Absyn_Carray_mod_struct
_tmpD6E;struct Cyc_Absyn_Carray_mod_struct*_tmpD6D;struct Cyc_Parse_Abstractdeclarator*
_tmpD6C;yyval=Cyc_YY30(((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->tms=((
_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->hd=(void*)((_tmpD6D=_cycalloc(
sizeof(*_tmpD6D)),((_tmpD6D[0]=((_tmpD6E.tag=0,((_tmpD6E.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD6E.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD6E)))))),
_tmpD6D)))),((_tmpD6F->tl=0,_tmpD6F)))))),_tmpD6C)))));}break;case 266: _LL33D:{
struct Cyc_List_List*_tmpD79;struct Cyc_Absyn_Carray_mod_struct _tmpD78;struct Cyc_Absyn_Carray_mod_struct*
_tmpD77;struct Cyc_Parse_Abstractdeclarator*_tmpD76;yyval=Cyc_YY30(((_tmpD76=
_cycalloc(sizeof(*_tmpD76)),((_tmpD76->tms=((_tmpD79=_cycalloc(sizeof(*_tmpD79)),((
_tmpD79->hd=(void*)((_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77[0]=((_tmpD78.tag=
0,((_tmpD78.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpD78.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD78)))))),
_tmpD77)))),((_tmpD79->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD79)))))),_tmpD76)))));}break;case 267: _LL33E:{
struct Cyc_List_List*_tmpD83;struct Cyc_Absyn_ConstArray_mod_struct _tmpD82;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD81;struct Cyc_Parse_Abstractdeclarator*
_tmpD80;yyval=Cyc_YY30(((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->tms=((
_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83->hd=(void*)((_tmpD81=_cycalloc(
sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD82.tag=1,((_tmpD82.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD82.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD82.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD82)))))))),
_tmpD81)))),((_tmpD83->tl=0,_tmpD83)))))),_tmpD80)))));}break;case 268: _LL33F:{
struct Cyc_List_List*_tmpD8D;struct Cyc_Absyn_ConstArray_mod_struct _tmpD8C;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD8B;struct Cyc_Parse_Abstractdeclarator*
_tmpD8A;yyval=Cyc_YY30(((_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A->tms=((
_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D->hd=(void*)((_tmpD8B=_cycalloc(
sizeof(*_tmpD8B)),((_tmpD8B[0]=((_tmpD8C.tag=1,((_tmpD8C.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD8C.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD8C.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD8C)))))))),
_tmpD8B)))),((_tmpD8D->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpD8D)))))),_tmpD8A)))));}break;case 269: _LL340:{
struct Cyc_List_List*_tmpDA2;struct Cyc_Absyn_Function_mod_struct _tmpDA1;struct Cyc_Absyn_WithTypes_struct
_tmpDA0;struct Cyc_Absyn_WithTypes_struct*_tmpD9F;struct Cyc_Absyn_Function_mod_struct*
_tmpD9E;struct Cyc_Parse_Abstractdeclarator*_tmpD9D;yyval=Cyc_YY30(((_tmpD9D=
_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->tms=((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((
_tmpDA2->hd=(void*)((_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E[0]=((_tmpDA1.tag=
3,((_tmpDA1.f1=(void*)((void*)((_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F[0]=((
_tmpDA0.tag=1,((_tmpDA0.f1=0,((_tmpDA0.f2=0,((_tmpDA0.f3=0,((_tmpDA0.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDA0.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDA0)))))))))))),
_tmpD9F))))),_tmpDA1)))),_tmpD9E)))),((_tmpDA2->tl=0,_tmpDA2)))))),_tmpD9D)))));}
break;case 270: _LL341: {struct _tuple24 _tmp725;struct Cyc_List_List*_tmp726;int
_tmp727;struct Cyc_Absyn_VarargInfo*_tmp728;struct Cyc_Core_Opt*_tmp729;struct Cyc_List_List*
_tmp72A;struct _tuple24*_tmp724=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp725=*_tmp724;_tmp726=
_tmp725.f1;_tmp727=_tmp725.f2;_tmp728=_tmp725.f3;_tmp729=_tmp725.f4;_tmp72A=
_tmp725.f5;{struct Cyc_List_List*_tmpDB7;struct Cyc_Absyn_Function_mod_struct
_tmpDB6;struct Cyc_Absyn_WithTypes_struct _tmpDB5;struct Cyc_Absyn_WithTypes_struct*
_tmpDB4;struct Cyc_Absyn_Function_mod_struct*_tmpDB3;struct Cyc_Parse_Abstractdeclarator*
_tmpDB2;yyval=Cyc_YY30(((_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->tms=((
_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7->hd=(void*)((_tmpDB3=_cycalloc(
sizeof(*_tmpDB3)),((_tmpDB3[0]=((_tmpDB6.tag=3,((_tmpDB6.f1=(void*)((void*)((
_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4[0]=((_tmpDB5.tag=1,((_tmpDB5.f1=
_tmp726,((_tmpDB5.f2=_tmp727,((_tmpDB5.f3=_tmp728,((_tmpDB5.f4=_tmp729,((_tmpDB5.f5=
_tmp72A,_tmpDB5)))))))))))),_tmpDB4))))),_tmpDB6)))),_tmpDB3)))),((_tmpDB7->tl=0,
_tmpDB7)))))),_tmpDB2)))));}break;}case 271: _LL342:{struct Cyc_List_List*_tmpDCC;
struct Cyc_Absyn_Function_mod_struct _tmpDCB;struct Cyc_Absyn_WithTypes_struct
_tmpDCA;struct Cyc_Absyn_WithTypes_struct*_tmpDC9;struct Cyc_Absyn_Function_mod_struct*
_tmpDC8;struct Cyc_Parse_Abstractdeclarator*_tmpDC7;yyval=Cyc_YY30(((_tmpDC7=
_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->tms=((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((
_tmpDCC->hd=(void*)((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8[0]=((_tmpDCB.tag=
3,((_tmpDCB.f1=(void*)((void*)((_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9[0]=((
_tmpDCA.tag=1,((_tmpDCA.f1=0,((_tmpDCA.f2=0,((_tmpDCA.f3=0,((_tmpDCA.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDCA.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDCA)))))))))))),
_tmpDC9))))),_tmpDCB)))),_tmpDC8)))),((_tmpDCC->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpDCC)))))),
_tmpDC7)))));}break;case 272: _LL343: {struct _tuple24 _tmp738;struct Cyc_List_List*
_tmp739;int _tmp73A;struct Cyc_Absyn_VarargInfo*_tmp73B;struct Cyc_Core_Opt*_tmp73C;
struct Cyc_List_List*_tmp73D;struct _tuple24*_tmp737=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp738=*_tmp737;_tmp739=
_tmp738.f1;_tmp73A=_tmp738.f2;_tmp73B=_tmp738.f3;_tmp73C=_tmp738.f4;_tmp73D=
_tmp738.f5;{struct Cyc_List_List*_tmpDE1;struct Cyc_Absyn_Function_mod_struct
_tmpDE0;struct Cyc_Absyn_WithTypes_struct _tmpDDF;struct Cyc_Absyn_WithTypes_struct*
_tmpDDE;struct Cyc_Absyn_Function_mod_struct*_tmpDDD;struct Cyc_Parse_Abstractdeclarator*
_tmpDDC;yyval=Cyc_YY30(((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC->tms=((
_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->hd=(void*)((_tmpDDD=_cycalloc(
sizeof(*_tmpDDD)),((_tmpDDD[0]=((_tmpDE0.tag=3,((_tmpDE0.f1=(void*)((void*)((
_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE[0]=((_tmpDDF.tag=1,((_tmpDDF.f1=
_tmp739,((_tmpDDF.f2=_tmp73A,((_tmpDDF.f3=_tmp73B,((_tmpDDF.f4=_tmp73C,((_tmpDDF.f5=
_tmp73D,_tmpDDF)))))))))))),_tmpDDE))))),_tmpDE0)))),_tmpDDD)))),((_tmpDE1->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDE1)))))),_tmpDDC)))));}break;}case 273: _LL344: {struct Cyc_List_List*_tmp744=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_List_List*_tmpDEB;struct Cyc_Absyn_TypeParams_mod_struct _tmpDEA;struct
Cyc_Absyn_TypeParams_mod_struct*_tmpDE9;struct Cyc_Parse_Abstractdeclarator*
_tmpDE8;yyval=Cyc_YY30(((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->tms=((
_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->hd=(void*)((_tmpDE9=_cycalloc(
sizeof(*_tmpDE9)),((_tmpDE9[0]=((_tmpDEA.tag=4,((_tmpDEA.f1=_tmp744,((_tmpDEA.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDEA.f3=0,_tmpDEA)))))))),_tmpDE9)))),((_tmpDEB->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDEB)))))),_tmpDE8)))));}break;}case 274: _LL345:{struct Cyc_List_List*_tmpDF5;
struct Cyc_Absyn_Attributes_mod_struct _tmpDF4;struct Cyc_Absyn_Attributes_mod_struct*
_tmpDF3;struct Cyc_Parse_Abstractdeclarator*_tmpDF2;yyval=Cyc_YY30(((_tmpDF2=
_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2->tms=((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((
_tmpDF5->hd=(void*)((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF4.tag=
5,((_tmpDF4.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDF4.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDF4)))))),_tmpDF3)))),((
_tmpDF5->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpDF5)))))),_tmpDF2)))));}break;case 275: _LL346: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 276: _LL347:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 277:
_LL348: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
278: _LL349: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 279: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 280: _LL34B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 281: _LL34C:{struct Cyc_Absyn_ResetRegion_s_struct _tmpDF8;
struct Cyc_Absyn_ResetRegion_s_struct*_tmpDF7;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((
void*)((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF8.tag=16,((
_tmpDF8.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),_tmpDF8)))),_tmpDF7)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 282:
_LL34D: yyval=Cyc_YY4(0);break;case 283: _LL34E:{const char*_tmpDF9;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpDF9="open",_tag_dyneither(_tmpDF9,sizeof(char),5))))
!= 0){const char*_tmpDFA;Cyc_Parse_err(((_tmpDFA="expecting `open'",
_tag_dyneither(_tmpDFA,sizeof(char),17))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 284: _LL34F:{struct Cyc_Absyn_Label_s_struct _tmpE00;
struct _dyneither_ptr*_tmpDFF;struct Cyc_Absyn_Label_s_struct*_tmpDFE;yyval=Cyc_YY9(
Cyc_Absyn_new_stmt((void*)((_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE[0]=((
_tmpE00.tag=13,((_tmpE00.f1=((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpDFF)))),((_tmpE00.f2=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE00)))))),_tmpDFE)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 285:
_LL350: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 286:
_LL351: yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
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
_LL358:{struct Cyc_Absyn_Fn_d_struct _tmpE03;struct Cyc_Absyn_Fn_d_struct*_tmpE02;
yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE02=
_cycalloc(sizeof(*_tmpE02)),((_tmpE02[0]=((_tmpE03.tag=1,((_tmpE03.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE03)))),_tmpE02)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0)));}break;case 294: _LL359:{
struct Cyc_Absyn_Fn_d_struct _tmpE06;struct Cyc_Absyn_Fn_d_struct*_tmpE05;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpE05=_cycalloc(sizeof(*
_tmpE05)),((_tmpE05[0]=((_tmpE06.tag=1,((_tmpE06.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE06)))),_tmpE05)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));}break;case 295: _LL35A: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
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
_LL35D: {struct Cyc_Absyn_UnknownId_e_struct _tmpE09;struct Cyc_Absyn_UnknownId_e_struct*
_tmpE08;struct Cyc_Absyn_Exp*_tmp758=Cyc_Absyn_new_exp((void*)((_tmpE08=_cycalloc(
sizeof(*_tmpE08)),((_tmpE08[0]=((_tmpE09.tag=2,((_tmpE09.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpE09)))),_tmpE08)))),
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
_LL360: yyval=Cyc_YY10(0);break;case 302: _LL361:{struct Cyc_Absyn_Switch_clause*
_tmpE0C;struct Cyc_List_List*_tmpE0B;yyval=Cyc_YY10(((_tmpE0B=_cycalloc(sizeof(*
_tmpE0B)),((_tmpE0B->hd=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmpE0C->pat_vars=
0,((_tmpE0C->where_clause=0,((_tmpE0C->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE0C->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE0C)))))))))))),((
_tmpE0B->tl=0,_tmpE0B)))))));}break;case 303: _LL362:{struct Cyc_Absyn_Switch_clause*
_tmpE0F;struct Cyc_List_List*_tmpE0E;yyval=Cyc_YY10(((_tmpE0E=_cycalloc(sizeof(*
_tmpE0E)),((_tmpE0E->hd=((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpE0F->pat_vars=0,((_tmpE0F->where_clause=0,((_tmpE0F->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpE0F->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE0F)))))))))))),((
_tmpE0E->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE0E)))))));}break;case 304: _LL363:{struct Cyc_Absyn_Switch_clause*
_tmpE12;struct Cyc_List_List*_tmpE11;yyval=Cyc_YY10(((_tmpE11=_cycalloc(sizeof(*
_tmpE11)),((_tmpE11->hd=((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpE12->pat_vars=0,((_tmpE12->where_clause=0,((_tmpE12->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE12->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpE12)))))))))))),((
_tmpE11->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE11)))))));}break;case 305: _LL364:{struct Cyc_Absyn_Switch_clause*
_tmpE15;struct Cyc_List_List*_tmpE14;yyval=Cyc_YY10(((_tmpE14=_cycalloc(sizeof(*
_tmpE14)),((_tmpE14->hd=((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((
_tmpE15->pat_vars=0,((_tmpE15->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpE15->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpE15->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE15)))))))))))),((
_tmpE14->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE14)))))));}break;case 306: _LL365:{struct Cyc_Absyn_Switch_clause*
_tmpE18;struct Cyc_List_List*_tmpE17;yyval=Cyc_YY10(((_tmpE17=_cycalloc(sizeof(*
_tmpE17)),((_tmpE17->hd=((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((
_tmpE18->pat_vars=0,((_tmpE18->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpE18->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE18->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpE18)))))))))))),((
_tmpE17->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE17)))))));}break;case 307: _LL366: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
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
Cyc_Parse_flatten_declarations(_tmp76C,_tmp76D));break;}case 321: _LL374:{struct
_dyneither_ptr*_tmpE19;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpE19=_cycalloc(
sizeof(*_tmpE19)),((_tmpE19[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE19)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));}break;case
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
_LL3A2:{struct Cyc_Absyn_StructField_struct _tmpE1F;struct _dyneither_ptr*_tmpE1E;
struct Cyc_Absyn_StructField_struct*_tmpE1D;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D[0]=((_tmpE1F.tag=0,((
_tmpE1F.f1=((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE1E)))),_tmpE1F)))),
_tmpE1D)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 368: _LL3A3:{struct Cyc_Absyn_TupleIndex_struct
_tmpE22;struct Cyc_Absyn_TupleIndex_struct*_tmpE21;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)((_tmpE21=_cycalloc_atomic(sizeof(*_tmpE21)),((
_tmpE21[0]=((_tmpE22.tag=1,((_tmpE22.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,_tmpE22)))),_tmpE21)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 369: _LL3A4: yyval=yyvs[
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
_tmp779=_tmp777.f2;}}_LL3AC:{struct Cyc_Absyn_Char_p_struct _tmpE25;struct Cyc_Absyn_Char_p_struct*
_tmpE24;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE24=_cycalloc_atomic(
sizeof(*_tmpE24)),((_tmpE24[0]=((_tmpE25.tag=10,((_tmpE25.f1=_tmp779,_tmpE25)))),
_tmpE24)))),e->loc));}goto _LL3AA;_LL3AD: {struct Cyc_Absyn_Const_e_struct*_tmp77A=(
struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp77A->tag != 0)goto _LL3AF;else{
_tmp77B=_tmp77A->f1;if((_tmp77B.Short_c).tag != 3)goto _LL3AF;_tmp77C=(struct
_tuple5)(_tmp77B.Short_c).val;_tmp77D=_tmp77C.f1;_tmp77E=_tmp77C.f2;}}_LL3AE:{
struct Cyc_Absyn_Int_p_struct _tmpE28;struct Cyc_Absyn_Int_p_struct*_tmpE27;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE27=_cycalloc_atomic(sizeof(*_tmpE27)),((
_tmpE27[0]=((_tmpE28.tag=9,((_tmpE28.f1=_tmp77D,((_tmpE28.f2=(int)_tmp77E,
_tmpE28)))))),_tmpE27)))),e->loc));}goto _LL3AA;_LL3AF: {struct Cyc_Absyn_Const_e_struct*
_tmp77F=(struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp77F->tag != 0)goto _LL3B1;
else{_tmp780=_tmp77F->f1;if((_tmp780.Int_c).tag != 4)goto _LL3B1;_tmp781=(struct
_tuple6)(_tmp780.Int_c).val;_tmp782=_tmp781.f1;_tmp783=_tmp781.f2;}}_LL3B0:{
struct Cyc_Absyn_Int_p_struct _tmpE2B;struct Cyc_Absyn_Int_p_struct*_tmpE2A;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE2A=_cycalloc_atomic(sizeof(*_tmpE2A)),((
_tmpE2A[0]=((_tmpE2B.tag=9,((_tmpE2B.f1=_tmp782,((_tmpE2B.f2=_tmp783,_tmpE2B)))))),
_tmpE2A)))),e->loc));}goto _LL3AA;_LL3B1: {struct Cyc_Absyn_Const_e_struct*_tmp784=(
struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp784->tag != 0)goto _LL3B3;else{
_tmp785=_tmp784->f1;if((_tmp785.Float_c).tag != 6)goto _LL3B3;_tmp786=(struct
_dyneither_ptr)(_tmp785.Float_c).val;}}_LL3B2:{struct Cyc_Absyn_Float_p_struct
_tmpE2E;struct Cyc_Absyn_Float_p_struct*_tmpE2D;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=((_tmpE2E.tag=11,((
_tmpE2E.f1=_tmp786,_tmpE2E)))),_tmpE2D)))),e->loc));}goto _LL3AA;_LL3B3: {struct
Cyc_Absyn_Const_e_struct*_tmp787=(struct Cyc_Absyn_Const_e_struct*)_tmp774;if(
_tmp787->tag != 0)goto _LL3B5;else{_tmp788=_tmp787->f1;if((_tmp788.Null_c).tag != 1)
goto _LL3B5;_tmp789=(int)(_tmp788.Null_c).val;}}_LL3B4: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3AA;_LL3B5: {struct Cyc_Absyn_Const_e_struct*
_tmp78A=(struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp78A->tag != 0)goto _LL3B7;
else{_tmp78B=_tmp78A->f1;if((_tmp78B.String_c).tag != 7)goto _LL3B7;_tmp78C=(
struct _dyneither_ptr)(_tmp78B.String_c).val;}}_LL3B6:{const char*_tmpE2F;Cyc_Parse_err(((
_tmpE2F="strings cannot occur within patterns",_tag_dyneither(_tmpE2F,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL3AA;_LL3B7: {struct Cyc_Absyn_Const_e_struct*
_tmp78D=(struct Cyc_Absyn_Const_e_struct*)_tmp774;if(_tmp78D->tag != 0)goto _LL3B9;
else{_tmp78E=_tmp78D->f1;if((_tmp78E.LongLong_c).tag != 5)goto _LL3B9;_tmp78F=(
struct _tuple7)(_tmp78E.LongLong_c).val;}}_LL3B8:{const char*_tmpE30;Cyc_Parse_unimp(((
_tmpE30="long long's in patterns",_tag_dyneither(_tmpE30,sizeof(char),24))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}goto _LL3AA;
_LL3B9:;_LL3BA: {const char*_tmpE31;Cyc_Parse_err(((_tmpE31="bad constant in case",
_tag_dyneither(_tmpE31,sizeof(char),21))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL3AA:;}
break;}case 374: _LL3A9:{struct Cyc_Absyn_UnknownId_p_struct _tmpE34;struct Cyc_Absyn_UnknownId_p_struct*
_tmpE33;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE33=_cycalloc(sizeof(*
_tmpE33)),((_tmpE33[0]=((_tmpE34.tag=14,((_tmpE34.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE34)))),_tmpE33)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 375:
_LL3BB:{const char*_tmpE35;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE35="as",
_tag_dyneither(_tmpE35,sizeof(char),3))))!= 0){const char*_tmpE36;Cyc_Parse_err(((
_tmpE36="expecting `as'",_tag_dyneither(_tmpE36,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct
_tmpE40;struct _dyneither_ptr*_tmpE3F;struct _tuple1*_tmpE3E;struct Cyc_Absyn_Var_p_struct*
_tmpE3D;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE3D=_cycalloc(sizeof(*
_tmpE3D)),((_tmpE3D[0]=((_tmpE40.tag=1,((_tmpE40.f1=Cyc_Absyn_new_vardecl(((
_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->f1=Cyc_Absyn_Loc_n,((_tmpE3E->f2=((
_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE3F)))),_tmpE3E)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE40.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE40)))))),_tmpE3D)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 376: _LL3BC: {struct Cyc_List_List*
_tmp7A4;int _tmp7A5;struct _tuple20 _tmp7A3=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A4=_tmp7A3.f1;
_tmp7A5=_tmp7A3.f2;{struct Cyc_Absyn_Tuple_p_struct _tmpE43;struct Cyc_Absyn_Tuple_p_struct*
_tmpE42;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE42=_cycalloc(sizeof(*
_tmpE42)),((_tmpE42[0]=((_tmpE43.tag=4,((_tmpE43.f1=_tmp7A4,((_tmpE43.f2=_tmp7A5,
_tmpE43)))))),_tmpE42)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
377: _LL3BD: {struct Cyc_List_List*_tmp7A9;int _tmp7AA;struct _tuple20 _tmp7A8=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A9=_tmp7A8.f1;
_tmp7AA=_tmp7A8.f2;{struct Cyc_Absyn_UnknownCall_p_struct _tmpE46;struct Cyc_Absyn_UnknownCall_p_struct*
_tmpE45;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE45=_cycalloc(sizeof(*
_tmpE45)),((_tmpE45[0]=((_tmpE46.tag=15,((_tmpE46.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpE46.f2=_tmp7A9,((
_tmpE46.f3=_tmp7AA,_tmpE46)))))))),_tmpE45)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
378: _LL3BE: {struct Cyc_List_List*_tmp7AE;int _tmp7AF;struct _tuple20 _tmp7AD=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AE=_tmp7AD.f1;
_tmp7AF=_tmp7AD.f2;{struct Cyc_List_List*_tmp7B0=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE4C;struct Cyc_Absyn_AggrInfo*_tmpE4B;struct Cyc_Absyn_Aggr_p_struct*_tmpE4A;
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((
_tmpE4A[0]=((_tmpE4C.tag=6,((_tmpE4C.f1=((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((
_tmpE4B->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0),((_tmpE4B->targs=
0,_tmpE4B)))))),((_tmpE4C.f2=_tmp7B0,((_tmpE4C.f3=_tmp7AE,((_tmpE4C.f4=_tmp7AF,
_tmpE4C)))))))))),_tmpE4A)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}}case
379: _LL3BF: {struct Cyc_List_List*_tmp7B5;int _tmp7B6;struct _tuple20 _tmp7B4=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B5=_tmp7B4.f1;
_tmp7B6=_tmp7B4.f2;{struct Cyc_List_List*_tmp7B7=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpE4F;struct Cyc_Absyn_Aggr_p_struct*_tmpE4E;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE4F.tag=6,((
_tmpE4F.f1=0,((_tmpE4F.f2=_tmp7B7,((_tmpE4F.f3=_tmp7B5,((_tmpE4F.f4=_tmp7B6,
_tmpE4F)))))))))),_tmpE4E)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}}case
380: _LL3C0:{struct Cyc_Absyn_Pointer_p_struct _tmpE52;struct Cyc_Absyn_Pointer_p_struct*
_tmpE51;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE51=_cycalloc(sizeof(*
_tmpE51)),((_tmpE51[0]=((_tmpE52.tag=5,((_tmpE52.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE52)))),_tmpE51)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 381:
_LL3C1:{struct Cyc_Absyn_Pointer_p_struct _tmpE5C;struct Cyc_Absyn_Pointer_p_struct
_tmpE5B;struct Cyc_Absyn_Pointer_p_struct*_tmpE5A;struct Cyc_Absyn_Pointer_p_struct*
_tmpE59;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE59=_cycalloc(sizeof(*
_tmpE59)),((_tmpE59[0]=((_tmpE5C.tag=5,((_tmpE5C.f1=Cyc_Absyn_new_pat((void*)((
_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=((_tmpE5B.tag=5,((_tmpE5B.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE5B)))),
_tmpE5A)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE5C)))),_tmpE59)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 382:
_LL3C2:{struct Cyc_Absyn_Reference_p_struct _tmpE66;struct _dyneither_ptr*_tmpE65;
struct _tuple1*_tmpE64;struct Cyc_Absyn_Reference_p_struct*_tmpE63;yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63[0]=((
_tmpE66.tag=2,((_tmpE66.f1=Cyc_Absyn_new_vardecl(((_tmpE64=_cycalloc(sizeof(*
_tmpE64)),((_tmpE64->f1=Cyc_Absyn_Loc_n,((_tmpE64->f2=((_tmpE65=_cycalloc(
sizeof(*_tmpE65)),((_tmpE65[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE65)))),_tmpE64)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE66.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpE66)))))),_tmpE63)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 383:
_LL3C3:{const char*_tmpE67;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE67="as",
_tag_dyneither(_tmpE67,sizeof(char),3))))!= 0){const char*_tmpE68;Cyc_Parse_err(((
_tmpE68="expecting `as'",_tag_dyneither(_tmpE68,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct
_tmpE72;struct _dyneither_ptr*_tmpE71;struct _tuple1*_tmpE70;struct Cyc_Absyn_Reference_p_struct*
_tmpE6F;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE6F=_cycalloc(sizeof(*
_tmpE6F)),((_tmpE6F[0]=((_tmpE72.tag=2,((_tmpE72.f1=Cyc_Absyn_new_vardecl(((
_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70->f1=Cyc_Absyn_Loc_n,((_tmpE70->f2=((
_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE71)))),_tmpE70)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpE72.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE72)))))),_tmpE6F)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 384: _LL3C4: {void*_tmp7CA=Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Tcutil_kind_to_bound(Cyc_Absyn_IntKind));{struct Cyc_Absyn_TagInt_p_struct
_tmpE83;struct Cyc_Absyn_TagType_struct _tmpE82;struct Cyc_Absyn_TagType_struct*
_tmpE81;struct _dyneither_ptr*_tmpE80;struct _tuple1*_tmpE7F;struct Cyc_Absyn_TagInt_p_struct*
_tmpE7E;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpE7E=_cycalloc(sizeof(*
_tmpE7E)),((_tmpE7E[0]=((_tmpE83.tag=3,((_tmpE83.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7CA),((
_tmpE83.f2=Cyc_Absyn_new_vardecl(((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F->f1=
Cyc_Absyn_Loc_n,((_tmpE7F->f2=((_tmpE80=_cycalloc(sizeof(*_tmpE80)),((_tmpE80[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpE80)))),_tmpE7F)))))),(void*)((_tmpE81=_cycalloc(sizeof(*_tmpE81)),((_tmpE81[
0]=((_tmpE82.tag=20,((_tmpE82.f1=(void*)_tmp7CA,_tmpE82)))),_tmpE81)))),0),
_tmpE83)))))),_tmpE7E)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
385: _LL3C5: {struct Cyc_Absyn_Tvar*_tmp7D1=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_IntKind));{struct Cyc_Absyn_TagInt_p_struct _tmpE9F;struct Cyc_Absyn_TagType_struct
_tmpE9E;struct Cyc_Absyn_VarType_struct _tmpE9D;struct Cyc_Absyn_VarType_struct*
_tmpE9C;struct Cyc_Absyn_TagType_struct*_tmpE9B;struct _dyneither_ptr*_tmpE9A;
struct _tuple1*_tmpE99;struct Cyc_Absyn_TagInt_p_struct*_tmpE98;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpE98=_cycalloc(sizeof(*_tmpE98)),((_tmpE98[0]=((_tmpE9F.tag=3,((
_tmpE9F.f1=_tmp7D1,((_tmpE9F.f2=Cyc_Absyn_new_vardecl(((_tmpE99=_cycalloc(
sizeof(*_tmpE99)),((_tmpE99->f1=Cyc_Absyn_Loc_n,((_tmpE99->f2=((_tmpE9A=
_cycalloc(sizeof(*_tmpE9A)),((_tmpE9A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpE9A)))),_tmpE99)))))),(
void*)((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=((_tmpE9E.tag=20,((
_tmpE9E.f1=(void*)((void*)((_tmpE9C=_cycalloc(sizeof(*_tmpE9C)),((_tmpE9C[0]=((
_tmpE9D.tag=2,((_tmpE9D.f1=_tmp7D1,_tmpE9D)))),_tmpE9C))))),_tmpE9E)))),_tmpE9B)))),
0),_tmpE9F)))))),_tmpE98)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
386: _LL3C6:{struct _tuple20*_tmpEA0;yyval=Cyc_YY12(((_tmpEA0=_cycalloc(sizeof(*
_tmpEA0)),((_tmpEA0->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpEA0->f2=0,_tmpEA0)))))));}break;case 387: _LL3C7:{struct _tuple20*_tmpEA1;yyval=
Cyc_YY12(((_tmpEA1=_cycalloc(sizeof(*_tmpEA1)),((_tmpEA1->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpEA1->f2=1,_tmpEA1)))))));}
break;case 388: _LL3C8:{struct _tuple20*_tmpEA2;yyval=Cyc_YY12(((_tmpEA2=_cycalloc(
sizeof(*_tmpEA2)),((_tmpEA2->f1=0,((_tmpEA2->f2=1,_tmpEA2)))))));}break;case 389:
_LL3C9:{struct Cyc_List_List*_tmpEA3;yyval=Cyc_YY13(((_tmpEA3=_cycalloc(sizeof(*
_tmpEA3)),((_tmpEA3->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpEA3->tl=0,_tmpEA3)))))));}break;case 390: _LL3CA:{struct Cyc_List_List*
_tmpEA4;yyval=Cyc_YY13(((_tmpEA4=_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEA4->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEA4)))))));}
break;case 391: _LL3CB:{struct _tuple21*_tmpEA5;yyval=Cyc_YY14(((_tmpEA5=_cycalloc(
sizeof(*_tmpEA5)),((_tmpEA5->f1=0,((_tmpEA5->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEA5)))))));}break;case
392: _LL3CC:{struct _tuple21*_tmpEA6;yyval=Cyc_YY14(((_tmpEA6=_cycalloc(sizeof(*
_tmpEA6)),((_tmpEA6->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpEA6->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEA6)))))));}break;case
393: _LL3CD:{struct _tuple20*_tmpEA7;yyval=Cyc_YY16(((_tmpEA7=_cycalloc(sizeof(*
_tmpEA7)),((_tmpEA7->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpEA7->f2=0,_tmpEA7)))))));}break;case 394: _LL3CE:{struct _tuple20*_tmpEA8;yyval=
Cyc_YY16(((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpEA8->f2=1,_tmpEA8)))))));}
break;case 395: _LL3CF:{struct _tuple20*_tmpEA9;yyval=Cyc_YY16(((_tmpEA9=_cycalloc(
sizeof(*_tmpEA9)),((_tmpEA9->f1=0,((_tmpEA9->f2=1,_tmpEA9)))))));}break;case 396:
_LL3D0:{struct Cyc_List_List*_tmpEAA;yyval=Cyc_YY15(((_tmpEAA=_cycalloc(sizeof(*
_tmpEAA)),((_tmpEAA->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpEAA->tl=0,_tmpEAA)))))));}break;case 397: _LL3D1:{struct Cyc_List_List*
_tmpEAB;yyval=Cyc_YY15(((_tmpEAB=_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEAB->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEAB)))))));}
break;case 398: _LL3D2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 399: _LL3D3: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
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
_LL3D6:{struct Cyc_Absyn_Swap_e_struct _tmpEAE;struct Cyc_Absyn_Swap_e_struct*
_tmpEAD;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEAD=_cycalloc(sizeof(*
_tmpEAD)),((_tmpEAD[0]=((_tmpEAE.tag=36,((_tmpEAE.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEAE.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEAE)))))),_tmpEAD)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 403: _LL3D7: yyval=Cyc_YY8(0);break;case
404: _LL3D8:{struct Cyc_Core_Opt*_tmpEAF;yyval=Cyc_YY8(((_tmpEAF=_cycalloc_atomic(
sizeof(*_tmpEAF)),((_tmpEAF->v=(void*)Cyc_Absyn_Times,_tmpEAF)))));}break;case
405: _LL3D9:{struct Cyc_Core_Opt*_tmpEB0;yyval=Cyc_YY8(((_tmpEB0=_cycalloc_atomic(
sizeof(*_tmpEB0)),((_tmpEB0->v=(void*)Cyc_Absyn_Div,_tmpEB0)))));}break;case 406:
_LL3DA:{struct Cyc_Core_Opt*_tmpEB1;yyval=Cyc_YY8(((_tmpEB1=_cycalloc_atomic(
sizeof(*_tmpEB1)),((_tmpEB1->v=(void*)Cyc_Absyn_Mod,_tmpEB1)))));}break;case 407:
_LL3DB:{struct Cyc_Core_Opt*_tmpEB2;yyval=Cyc_YY8(((_tmpEB2=_cycalloc_atomic(
sizeof(*_tmpEB2)),((_tmpEB2->v=(void*)Cyc_Absyn_Plus,_tmpEB2)))));}break;case 408:
_LL3DC:{struct Cyc_Core_Opt*_tmpEB3;yyval=Cyc_YY8(((_tmpEB3=_cycalloc_atomic(
sizeof(*_tmpEB3)),((_tmpEB3->v=(void*)Cyc_Absyn_Minus,_tmpEB3)))));}break;case
409: _LL3DD:{struct Cyc_Core_Opt*_tmpEB4;yyval=Cyc_YY8(((_tmpEB4=_cycalloc_atomic(
sizeof(*_tmpEB4)),((_tmpEB4->v=(void*)Cyc_Absyn_Bitlshift,_tmpEB4)))));}break;
case 410: _LL3DE:{struct Cyc_Core_Opt*_tmpEB5;yyval=Cyc_YY8(((_tmpEB5=
_cycalloc_atomic(sizeof(*_tmpEB5)),((_tmpEB5->v=(void*)Cyc_Absyn_Bitlrshift,
_tmpEB5)))));}break;case 411: _LL3DF:{struct Cyc_Core_Opt*_tmpEB6;yyval=Cyc_YY8(((
_tmpEB6=_cycalloc_atomic(sizeof(*_tmpEB6)),((_tmpEB6->v=(void*)Cyc_Absyn_Bitand,
_tmpEB6)))));}break;case 412: _LL3E0:{struct Cyc_Core_Opt*_tmpEB7;yyval=Cyc_YY8(((
_tmpEB7=_cycalloc_atomic(sizeof(*_tmpEB7)),((_tmpEB7->v=(void*)Cyc_Absyn_Bitxor,
_tmpEB7)))));}break;case 413: _LL3E1:{struct Cyc_Core_Opt*_tmpEB8;yyval=Cyc_YY8(((
_tmpEB8=_cycalloc_atomic(sizeof(*_tmpEB8)),((_tmpEB8->v=(void*)Cyc_Absyn_Bitor,
_tmpEB8)))));}break;case 414: _LL3E2: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 415: _LL3E3: yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
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
_LL411:{struct Cyc_Absyn_StructField_struct _tmpEBE;struct _dyneither_ptr*_tmpEBD;
struct Cyc_Absyn_StructField_struct*_tmpEBC;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC[0]=((_tmpEBE.tag=0,((
_tmpEBE.f1=((_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((_tmpEBD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEBD)))),_tmpEBE)))),
_tmpEBC)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 462: _LL412:{struct Cyc_Absyn_TupleIndex_struct
_tmpEC1;struct Cyc_Absyn_TupleIndex_struct*_tmpEC0;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpEC0=_cycalloc_atomic(sizeof(*_tmpEC0)),((_tmpEC0[0]=((_tmpEC1.tag=1,((
_tmpEC1.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2,_tmpEC1)))),_tmpEC0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 463:
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
_LL414:{struct Cyc_Absyn_Malloc_e_struct _tmpEC7;struct Cyc_Absyn_MallocInfo _tmpEC6;
struct Cyc_Absyn_Malloc_e_struct*_tmpEC5;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEC5=_cycalloc(sizeof(*_tmpEC5)),((_tmpEC5[0]=((_tmpEC7.tag=35,((_tmpEC7.f1=((
_tmpEC6.is_calloc=0,((_tmpEC6.rgn=0,((_tmpEC6.elt_type=0,((_tmpEC6.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpEC6.fat_result=
0,_tmpEC6)))))))))),_tmpEC7)))),_tmpEC5)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 465:
_LL415:{struct Cyc_Absyn_Malloc_e_struct _tmpECD;struct Cyc_Absyn_MallocInfo _tmpECC;
struct Cyc_Absyn_Malloc_e_struct*_tmpECB;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpECB=_cycalloc(sizeof(*_tmpECB)),((_tmpECB[0]=((_tmpECD.tag=35,((_tmpECD.f1=((
_tmpECC.is_calloc=0,((_tmpECC.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpECC.elt_type=0,((
_tmpECC.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpECC.fat_result=0,_tmpECC)))))))))),_tmpECD)))),_tmpECB)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 466: _LL416: {void*_tmp800;struct _tuple8
_tmp7FF=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp800=_tmp7FF.f3;{struct Cyc_Absyn_Malloc_e_struct _tmpED7;void**_tmpED6;
struct Cyc_Absyn_MallocInfo _tmpED5;struct Cyc_Absyn_Malloc_e_struct*_tmpED4;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4[
0]=((_tmpED7.tag=35,((_tmpED7.f1=((_tmpED5.is_calloc=1,((_tmpED5.rgn=0,((_tmpED5.elt_type=((
_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6[0]=_tmp800,_tmpED6)))),((_tmpED5.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpED5.fat_result=0,_tmpED5)))))))))),_tmpED7)))),_tmpED4)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
467: _LL417: {void*_tmp806;struct _tuple8 _tmp805=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp806=_tmp805.f3;{
struct Cyc_Absyn_Malloc_e_struct _tmpEE1;void**_tmpEE0;struct Cyc_Absyn_MallocInfo
_tmpEDF;struct Cyc_Absyn_Malloc_e_struct*_tmpEDE;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE[0]=((_tmpEE1.tag=35,((
_tmpEE1.f1=((_tmpEDF.is_calloc=1,((_tmpEDF.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmpEDF.elt_type=((
_tmpEE0=_cycalloc(sizeof(*_tmpEE0)),((_tmpEE0[0]=_tmp806,_tmpEE0)))),((_tmpEDF.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpEDF.fat_result=0,_tmpEDF)))))))))),_tmpEE1)))),_tmpEDE)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
468: _LL418:{struct Cyc_Absyn_Exp*_tmpEE2[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(
Cyc_Absyn_Numelts,((_tmpEE2[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpEE2,sizeof(struct Cyc_Absyn_Exp*),1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 469:
_LL419:{struct Cyc_Absyn_Tagcheck_e_struct _tmpEE8;struct _dyneither_ptr*_tmpEE7;
struct Cyc_Absyn_Tagcheck_e_struct*_tmpEE6;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpEE6=_cycalloc(sizeof(*_tmpEE6)),((_tmpEE6[0]=((_tmpEE8.tag=39,((_tmpEE8.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpEE8.f2=((_tmpEE7=_cycalloc(sizeof(*_tmpEE7)),((_tmpEE7[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEE7)))),_tmpEE8)))))),
_tmpEE6)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 470: _LL41A:{struct Cyc_Absyn_Tagcheck_e_struct
_tmpEEE;struct _dyneither_ptr*_tmpEED;struct Cyc_Absyn_Tagcheck_e_struct*_tmpEEC;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEEC=_cycalloc(sizeof(*_tmpEEC)),((
_tmpEEC[0]=((_tmpEEE.tag=39,((_tmpEEE.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),((_tmpEEE.f2=((
_tmpEED=_cycalloc(sizeof(*_tmpEED)),((_tmpEED[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpEED)))),_tmpEEE)))))),
_tmpEEC)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 471: _LL41B: {void*_tmp813;struct _tuple8
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
_LL423:{struct _dyneither_ptr*_tmpEEF;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEEF=_cycalloc(
sizeof(*_tmpEEF)),((_tmpEEF[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEEF)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 480:
_LL424:{struct _dyneither_ptr*_tmpEF0;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEF0=_cycalloc(
sizeof(*_tmpEF0)),((_tmpEF0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEF0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 481:
_LL425: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 482:
_LL426: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 483:
_LL427:{struct Cyc_Absyn_CompoundLit_e_struct _tmpEF3;struct Cyc_Absyn_CompoundLit_e_struct*
_tmpEF2;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEF2=_cycalloc(sizeof(*
_tmpEF2)),((_tmpEF2[0]=((_tmpEF3.tag=27,((_tmpEF3.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpEF3.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpEF3)))))),_tmpEF2)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 484: _LL428:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpEF6;struct Cyc_Absyn_CompoundLit_e_struct*_tmpEF5;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpEF5=_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5[0]=((_tmpEF6.tag=27,((
_tmpEF6.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 5)]),((_tmpEF6.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),
_tmpEF6)))))),_tmpEF5)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 485:
_LL429:{struct Cyc_Absyn_UnknownId_e_struct _tmpEF9;struct Cyc_Absyn_UnknownId_e_struct*
_tmpEF8;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEF8=_cycalloc(sizeof(*
_tmpEF8)),((_tmpEF8[0]=((_tmpEF9.tag=2,((_tmpEF9.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEF9)))),_tmpEF8)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 486:
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
_LL430:{struct Cyc_Absyn_Aggregate_e_struct _tmpEFC;struct Cyc_Absyn_Aggregate_e_struct*
_tmpEFB;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpEFB=_cycalloc(sizeof(*
_tmpEFB)),((_tmpEFB[0]=((_tmpEFC.tag=30,((_tmpEFC.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpEFC.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpEFC.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmpEFC.f4=0,
_tmpEFC)))))))))),_tmpEFB)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 493:
_LL431: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 494:
_LL432: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 495: _LL433:{struct Cyc_List_List*_tmpEFD;yyval=Cyc_YY5(((_tmpEFD=_cycalloc(
sizeof(*_tmpEFD)),((_tmpEFD->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEFD->tl=0,_tmpEFD)))))));}
break;case 496: _LL434:{struct Cyc_List_List*_tmpEFE;yyval=Cyc_YY5(((_tmpEFE=
_cycalloc(sizeof(*_tmpEFE)),((_tmpEFE->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEFE->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEFE)))))));}
break;case 497: _LL435: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(yyvs[
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
_LL439:{struct _dyneither_ptr*_tmpF01;struct _tuple1*_tmpF00;yyval=Cyc_QualId_tok(((
_tmpF00=_cycalloc(sizeof(*_tmpF00)),((_tmpF00->f1=Cyc_Absyn_Rel_n(0),((_tmpF00->f2=((
_tmpF01=_cycalloc(sizeof(*_tmpF01)),((_tmpF01[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF01)))),_tmpF00)))))));}
break;case 502: _LL43A: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 503: _LL43B:{struct _dyneither_ptr*_tmpF04;struct _tuple1*
_tmpF03;yyval=Cyc_QualId_tok(((_tmpF03=_cycalloc(sizeof(*_tmpF03)),((_tmpF03->f1=
Cyc_Absyn_Rel_n(0),((_tmpF03->f2=((_tmpF04=_cycalloc(sizeof(*_tmpF04)),((_tmpF04[
0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpF04)))),_tmpF03)))))));}break;case 504: _LL43C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 505: _LL43D: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 506: _LL43E:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 507:
_LL43F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
508: _LL440: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 509: _LL441: break;case 510: _LL442: yylex_buf->lex_curr_pos -=1;break;default:
_LL443: break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
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
_check_known_subscript_notnull(287,x)])+ 15,count ++);}{unsigned int _tmpF09;
unsigned int _tmpF08;struct _dyneither_ptr _tmpF07;char*_tmpF06;unsigned int _tmpF05;
msg=((_tmpF05=(unsigned int)(sze + 15),((_tmpF06=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpF05 + 1)),((_tmpF07=_tag_dyneither(_tmpF06,sizeof(
char),_tmpF05 + 1),((((_tmpF08=_tmpF05,((_tmpF0A(& _tmpF09,& _tmpF08,& _tmpF06),
_tmpF06[_tmpF08]=(char)0)))),_tmpF07))))))));}{const char*_tmpF0B;Cyc_strcpy(msg,((
_tmpF0B="parse error",_tag_dyneither(_tmpF0B,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 287 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6372,x + yyn)]== x){{const char*_tmpF0D;const char*
_tmpF0C;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpF0D=", expecting `",_tag_dyneither(_tmpF0D,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpF0C=" or `",_tag_dyneither(_tmpF0C,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(287,x)]);{
const char*_tmpF0E;Cyc_strcat(msg,((_tmpF0E="'",_tag_dyneither(_tmpF0E,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpF0F;Cyc_yyerror(((_tmpF0F="parse error",_tag_dyneither(_tmpF0F,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp82D=1;_npop_handler(0);return _tmp82D;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp82E=1;
_npop_handler(0);return _tmp82E;}-- yyvsp_offset;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];-- yylsp_offset;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,yystate)];if(yyn == -
32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6371) || Cyc_yycheck[
_check_known_subscript_notnull(6372,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6372,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1031){int
_tmp82F=0;_npop_handler(0);return _tmp82F;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v);void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE
_tmp839=v;struct _tuple6 _tmp83A;int _tmp83B;char _tmp83C;struct _dyneither_ptr
_tmp83D;struct _tuple1*_tmp83E;struct _tuple1 _tmp83F;union Cyc_Absyn_Nmspace _tmp840;
struct _dyneither_ptr*_tmp841;_LL446: if((_tmp839.Int_tok).tag != 1)goto _LL448;
_tmp83A=(struct _tuple6)(_tmp839.Int_tok).val;_tmp83B=_tmp83A.f2;_LL447:{const
char*_tmpF13;void*_tmpF12[1];struct Cyc_Int_pa_struct _tmpF11;(_tmpF11.tag=1,((
_tmpF11.f1=(unsigned long)_tmp83B,((_tmpF12[0]=& _tmpF11,Cyc_fprintf(Cyc_stderr,((
_tmpF13="%d",_tag_dyneither(_tmpF13,sizeof(char),3))),_tag_dyneither(_tmpF12,
sizeof(void*),1)))))));}goto _LL445;_LL448: if((_tmp839.Char_tok).tag != 2)goto
_LL44A;_tmp83C=(char)(_tmp839.Char_tok).val;_LL449:{const char*_tmpF17;void*
_tmpF16[1];struct Cyc_Int_pa_struct _tmpF15;(_tmpF15.tag=1,((_tmpF15.f1=(
unsigned long)((int)_tmp83C),((_tmpF16[0]=& _tmpF15,Cyc_fprintf(Cyc_stderr,((
_tmpF17="%c",_tag_dyneither(_tmpF17,sizeof(char),3))),_tag_dyneither(_tmpF16,
sizeof(void*),1)))))));}goto _LL445;_LL44A: if((_tmp839.String_tok).tag != 3)goto
_LL44C;_tmp83D=(struct _dyneither_ptr)(_tmp839.String_tok).val;_LL44B:{const char*
_tmpF1B;void*_tmpF1A[1];struct Cyc_String_pa_struct _tmpF19;(_tmpF19.tag=0,((
_tmpF19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp83D),((_tmpF1A[0]=&
_tmpF19,Cyc_fprintf(Cyc_stderr,((_tmpF1B="\"%s\"",_tag_dyneither(_tmpF1B,sizeof(
char),5))),_tag_dyneither(_tmpF1A,sizeof(void*),1)))))));}goto _LL445;_LL44C: if((
_tmp839.QualId_tok).tag != 5)goto _LL44E;_tmp83E=(struct _tuple1*)(_tmp839.QualId_tok).val;
_tmp83F=*_tmp83E;_tmp840=_tmp83F.f1;_tmp841=_tmp83F.f2;_LL44D: {struct Cyc_List_List*
_tmp84B=0;{union Cyc_Absyn_Nmspace _tmp84C=_tmp840;struct Cyc_List_List*_tmp84D;
struct Cyc_List_List*_tmp84E;int _tmp84F;_LL451: if((_tmp84C.Rel_n).tag != 1)goto
_LL453;_tmp84D=(struct Cyc_List_List*)(_tmp84C.Rel_n).val;_LL452: _tmp84B=_tmp84D;
goto _LL450;_LL453: if((_tmp84C.Abs_n).tag != 2)goto _LL455;_tmp84E=(struct Cyc_List_List*)(
_tmp84C.Abs_n).val;_LL454: _tmp84B=_tmp84E;goto _LL450;_LL455: if((_tmp84C.Loc_n).tag
!= 3)goto _LL450;_tmp84F=(int)(_tmp84C.Loc_n).val;_LL456: goto _LL450;_LL450:;}for(
0;_tmp84B != 0;_tmp84B=_tmp84B->tl){const char*_tmpF1F;void*_tmpF1E[1];struct Cyc_String_pa_struct
_tmpF1D;(_tmpF1D.tag=0,((_tmpF1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp84B->hd)),((_tmpF1E[0]=& _tmpF1D,Cyc_fprintf(Cyc_stderr,((
_tmpF1F="%s::",_tag_dyneither(_tmpF1F,sizeof(char),5))),_tag_dyneither(_tmpF1E,
sizeof(void*),1)))))));}{const char*_tmpF23;void*_tmpF22[1];struct Cyc_String_pa_struct
_tmpF21;(_tmpF21.tag=0,((_tmpF21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp841),((_tmpF22[0]=& _tmpF21,Cyc_fprintf(Cyc_stderr,((_tmpF23="%s::",
_tag_dyneither(_tmpF23,sizeof(char),5))),_tag_dyneither(_tmpF22,sizeof(void*),1)))))));}
goto _LL445;}_LL44E:;_LL44F:{const char*_tmpF26;void*_tmpF25;(_tmpF25=0,Cyc_fprintf(
Cyc_stderr,((_tmpF26="?",_tag_dyneither(_tmpF26,sizeof(char),2))),_tag_dyneither(
_tmpF25,sizeof(void*),0)));}goto _LL445;_LL445:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*
f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(f));return Cyc_Parse_parse_result;}
