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
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(void*,void*),
struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_zstrcmp(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct
_dyneither_ptr*);struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct
_dyneither_ptr src);struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,
struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,
int ofs,unsigned long n);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;
struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];
struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int,int);struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
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
struct Cyc_Position_Exit_struct _tmp7E4;struct Cyc_Position_Exit_struct*_tmp7E3;(
int)_throw((void*)((_tmp7E3=_cycalloc_atomic(sizeof(*_tmp7E3)),((_tmp7E3[0]=((
_tmp7E4.tag=Cyc_Position_Exit,_tmp7E4)),_tmp7E3)))));}}static void Cyc_Parse_unimp(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg);static void Cyc_Parse_unimp(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){{const char*_tmp7E9;void*
_tmp7E8[2];struct Cyc_String_pa_struct _tmp7E7;struct Cyc_String_pa_struct _tmp7E6;(
_tmp7E6.tag=0,((_tmp7E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((
_tmp7E7.tag=0,((_tmp7E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
sg)),((_tmp7E8[0]=& _tmp7E7,((_tmp7E8[1]=& _tmp7E6,Cyc_fprintf(Cyc_stderr,((
_tmp7E9="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp7E9,
sizeof(char),46))),_tag_dyneither(_tmp7E8,sizeof(void*),2)))))))))))));}return;}
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
else{_tmpE=_tmpD->f1;}}_LLE:{struct Cyc_Absyn_Upper_b_struct _tmp7EC;struct Cyc_Absyn_Upper_b_struct*
_tmp7EB;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmp7EB=_cycalloc(sizeof(*_tmp7EB)),((_tmp7EB[0]=((_tmp7EC.tag=1,((
_tmp7EC.f1=_tmpE,_tmp7EC)))),_tmp7EB)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*
_tmpF=(struct Cyc_Absyn_Region_ptrqual_struct*)_tmp6;if(_tmpF->tag != 1)goto _LL0;
else{_tmp10=(void*)_tmpF->f1;}}_LL10: rgn=_tmp10;goto _LL0;_LL0:;}{struct _tuple13
_tmp7ED;return(_tmp7ED.f1=nullable,((_tmp7ED.f2=bound,((_tmp7ED.f3=zeroterm,((
_tmp7ED.f4=rgn,_tmp7ED)))))));}}static int Cyc_Parse_enum_counter=0;struct _tuple1*
Cyc_Parse_gensym_enum();struct _tuple1*Cyc_Parse_gensym_enum(){struct
_dyneither_ptr*_tmp7FA;const char*_tmp7F9;void*_tmp7F8[1];struct Cyc_Int_pa_struct
_tmp7F7;struct _tuple1*_tmp7F6;return(_tmp7F6=_cycalloc(sizeof(*_tmp7F6)),((
_tmp7F6->f1=Cyc_Absyn_Rel_n(0),((_tmp7F6->f2=((_tmp7FA=_cycalloc(sizeof(*_tmp7FA)),((
_tmp7FA[0]=(struct _dyneither_ptr)((_tmp7F7.tag=1,((_tmp7F7.f1=(unsigned long)Cyc_Parse_enum_counter
++,((_tmp7F8[0]=& _tmp7F7,Cyc_aprintf(((_tmp7F9="__anonymous_enum_%d__",
_tag_dyneither(_tmp7F9,sizeof(char),22))),_tag_dyneither(_tmp7F8,sizeof(void*),1)))))))),
_tmp7FA)))),_tmp7F6)))));}struct _tuple14{struct _tuple1*f1;struct Cyc_Absyn_Tqual
f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple15{struct
_tuple14*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple15*field_info);static struct Cyc_Absyn_Aggrfield*
Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*loc,struct _tuple15*
field_info){struct _tuple15 _tmp1A;struct _tuple14*_tmp1B;struct _tuple14 _tmp1C;
struct _tuple1*_tmp1D;struct Cyc_Absyn_Tqual _tmp1E;void*_tmp1F;struct Cyc_List_List*
_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp22;struct _tuple15*
_tmp19=field_info;_tmp1A=*_tmp19;_tmp1B=_tmp1A.f1;_tmp1C=*_tmp1B;_tmp1D=_tmp1C.f1;
_tmp1E=_tmp1C.f2;_tmp1F=_tmp1C.f3;_tmp20=_tmp1C.f4;_tmp21=_tmp1C.f5;_tmp22=
_tmp1A.f2;if(_tmp20 != 0){const char*_tmp7FB;Cyc_Parse_err(((_tmp7FB="bad type params in struct field",
_tag_dyneither(_tmp7FB,sizeof(char),32))),loc);}if(Cyc_Absyn_is_qvar_qualified(
_tmp1D)){const char*_tmp7FC;Cyc_Parse_err(((_tmp7FC="struct or union field cannot be qualified with a namespace",
_tag_dyneither(_tmp7FC,sizeof(char),59))),loc);}{struct Cyc_Absyn_Aggrfield*
_tmp7FD;return(_tmp7FD=_cycalloc(sizeof(*_tmp7FD)),((_tmp7FD->name=(*_tmp1D).f2,((
_tmp7FD->tq=_tmp1E,((_tmp7FD->type=_tmp1F,((_tmp7FD->width=_tmp22,((_tmp7FD->attributes=
_tmp21,_tmp7FD)))))))))));}}static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*
loc);static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){
struct Cyc_Parse_Type_spec_struct _tmp800;struct Cyc_Parse_Type_spec_struct*_tmp7FF;
return(void*)((_tmp7FF=_cycalloc(sizeof(*_tmp7FF)),((_tmp7FF[0]=((_tmp800.tag=4,((
_tmp800.f1=(void*)t,((_tmp800.f2=loc,_tmp800)))))),_tmp7FF))));}static void*Cyc_Parse_array2ptr(
void*t,int argposn);static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp28=
t;struct Cyc_Absyn_ArrayInfo _tmp2A;void*_tmp2B;struct Cyc_Absyn_Tqual _tmp2C;struct
Cyc_Absyn_Exp*_tmp2D;union Cyc_Absyn_Constraint*_tmp2E;struct Cyc_Position_Segment*
_tmp2F;_LL12: {struct Cyc_Absyn_ArrayType_struct*_tmp29=(struct Cyc_Absyn_ArrayType_struct*)
_tmp28;if(_tmp29->tag != 9)goto _LL14;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp2A.elt_type;
_tmp2C=_tmp2A.tq;_tmp2D=_tmp2A.num_elts;_tmp2E=_tmp2A.zero_term;_tmp2F=_tmp2A.zt_loc;}}
_LL13: {struct Cyc_Absyn_Upper_b_struct _tmp803;struct Cyc_Absyn_Upper_b_struct*
_tmp802;return Cyc_Absyn_starb_typ(_tmp2B,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp2C,_tmp2D == 0?(void*)& Cyc_Absyn_DynEither_b_val:(
void*)((void*)((_tmp802=_cycalloc(sizeof(*_tmp802)),((_tmp802[0]=((_tmp803.tag=1,((
_tmp803.f1=(struct Cyc_Absyn_Exp*)_tmp2D,_tmp803)))),_tmp802))))),_tmp2E);}_LL14:;
_LL15: return t;_LL11:;}static void Cyc_Parse_arg_array2ptr(struct _tuple8*x);static
void Cyc_Parse_arg_array2ptr(struct _tuple8*x){(*x).f3=Cyc_Parse_array2ptr((*x).f3,
1);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple16*Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment*loc,struct _tuple8*t);static struct _tuple16*Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment*loc,struct _tuple8*t){struct _tuple16*_tmp804;return(
_tmp804=_cycalloc(sizeof(*_tmp804)),((_tmp804->f1=(*t).f2,((_tmp804->f2=(*t).f3,
_tmp804)))));}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*
d);static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp33=d->r;struct Cyc_Absyn_Vardecl*_tmp35;_LL17: {struct Cyc_Absyn_Var_d_struct*
_tmp34=(struct Cyc_Absyn_Var_d_struct*)_tmp33;if(_tmp34->tag != 0)goto _LL19;else{
_tmp35=_tmp34->f1;}}_LL18: return _tmp35;_LL19:;_LL1A: {const char*_tmp807;void*
_tmp806;(_tmp806=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,((_tmp807="bad declaration in `forarray' statement",
_tag_dyneither(_tmp807,sizeof(char),40))),_tag_dyneither(_tmp806,sizeof(void*),0)));}
_LL16:;}static int Cyc_Parse_is_typeparam(void*tm);static int Cyc_Parse_is_typeparam(
void*tm){void*_tmp38=tm;_LL1C: {struct Cyc_Absyn_TypeParams_mod_struct*_tmp39=(
struct Cyc_Absyn_TypeParams_mod_struct*)_tmp38;if(_tmp39->tag != 4)goto _LL1E;}
_LL1D: return 1;_LL1E:;_LL1F: return 0;_LL1B:;}static void*Cyc_Parse_id2type(struct
_dyneither_ptr s,void*k);static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*
k){const char*_tmp808;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp808="`H",
_tag_dyneither(_tmp808,sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;
else{const char*_tmp809;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp809="`U",
_tag_dyneither(_tmp809,sizeof(char),3))))== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;
else{struct Cyc_Absyn_VarType_struct _tmp813;struct _dyneither_ptr*_tmp812;struct
Cyc_Absyn_Tvar*_tmp811;struct Cyc_Absyn_VarType_struct*_tmp810;return(void*)((
_tmp810=_cycalloc(sizeof(*_tmp810)),((_tmp810[0]=((_tmp813.tag=2,((_tmp813.f1=((
_tmp811=_cycalloc(sizeof(*_tmp811)),((_tmp811->name=((_tmp812=_cycalloc(sizeof(*
_tmp812)),((_tmp812[0]=s,_tmp812)))),((_tmp811->identity=- 1,((_tmp811->kind=k,
_tmp811)))))))),_tmp813)))),_tmp810))));}}}static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(
struct Cyc_Absyn_Tvar*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*
t){void*k;{void*_tmp40=Cyc_Absyn_compress_kb(t->kind);enum Cyc_Absyn_Kind _tmp43;
_LL21: {struct Cyc_Absyn_Unknown_kb_struct*_tmp41=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp40;if(_tmp41->tag != 1)goto _LL23;}_LL22:{struct Cyc_Absyn_Unknown_kb_struct
_tmp816;struct Cyc_Absyn_Unknown_kb_struct*_tmp815;k=(void*)((_tmp815=_cycalloc(
sizeof(*_tmp815)),((_tmp815[0]=((_tmp816.tag=1,((_tmp816.f1=0,_tmp816)))),
_tmp815))));}goto _LL20;_LL23: {struct Cyc_Absyn_Less_kb_struct*_tmp42=(struct Cyc_Absyn_Less_kb_struct*)
_tmp40;if(_tmp42->tag != 2)goto _LL25;else{_tmp43=_tmp42->f2;}}_LL24:{struct Cyc_Absyn_Less_kb_struct
_tmp819;struct Cyc_Absyn_Less_kb_struct*_tmp818;k=(void*)((_tmp818=_cycalloc(
sizeof(*_tmp818)),((_tmp818[0]=((_tmp819.tag=2,((_tmp819.f1=0,((_tmp819.f2=
_tmp43,_tmp819)))))),_tmp818))));}goto _LL20;_LL25:;_LL26: k=_tmp40;goto _LL20;
_LL20:;}{struct Cyc_Absyn_Tvar*_tmp81A;return(_tmp81A=_cycalloc(sizeof(*_tmp81A)),((
_tmp81A->name=t->name,((_tmp81A->identity=- 1,((_tmp81A->kind=k,_tmp81A)))))));}}
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,
void*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp49=t;struct Cyc_Absyn_Tvar*_tmp4B;_LL28: {struct Cyc_Absyn_VarType_struct*
_tmp4A=(struct Cyc_Absyn_VarType_struct*)_tmp49;if(_tmp4A->tag != 2)goto _LL2A;
else{_tmp4B=_tmp4A->f1;}}_LL29: return _tmp4B;_LL2A:;_LL2B: {const char*_tmp81D;
void*_tmp81C;(_tmp81C=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp81D="expecting a list of type variables, not types",
_tag_dyneither(_tmp81D,sizeof(char),46))),_tag_dyneither(_tmp81C,sizeof(void*),0)));}
_LL27:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr);static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr){struct Cyc_Absyn_VarType_struct _tmp820;struct Cyc_Absyn_VarType_struct*
_tmp81F;return(void*)((_tmp81F=_cycalloc(sizeof(*_tmp81F)),((_tmp81F[0]=((
_tmp820.tag=2,((_tmp820.f1=pr,_tmp820)))),_tmp81F))));}static void Cyc_Parse_set_vartyp_kind(
void*t,enum Cyc_Absyn_Kind k,int leq);static void Cyc_Parse_set_vartyp_kind(void*t,
enum Cyc_Absyn_Kind k,int leq){void*_tmp50=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*
_tmp52;struct Cyc_Absyn_Tvar _tmp53;void*_tmp54;void**_tmp55;_LL2D: {struct Cyc_Absyn_VarType_struct*
_tmp51=(struct Cyc_Absyn_VarType_struct*)_tmp50;if(_tmp51->tag != 2)goto _LL2F;
else{_tmp52=_tmp51->f1;_tmp53=*_tmp52;_tmp54=_tmp53.kind;_tmp55=(void**)&(*
_tmp51->f1).kind;}}_LL2E: {void*_tmp56=Cyc_Absyn_compress_kb(*_tmp55);_LL32: {
struct Cyc_Absyn_Unknown_kb_struct*_tmp57=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp56;if(_tmp57->tag != 1)goto _LL34;}_LL33: if(!leq)*_tmp55=Cyc_Tcutil_kind_to_bound(
k);else{struct Cyc_Absyn_Less_kb_struct _tmp823;struct Cyc_Absyn_Less_kb_struct*
_tmp822;*_tmp55=(void*)((_tmp822=_cycalloc(sizeof(*_tmp822)),((_tmp822[0]=((
_tmp823.tag=2,((_tmp823.f1=0,((_tmp823.f2=k,_tmp823)))))),_tmp822))));}return;
_LL34:;_LL35: return;_LL31:;}_LL2F:;_LL30: return;_LL2C:;}static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*tms,struct Cyc_List_List*tds,
struct Cyc_Position_Segment*loc);static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp5A=(void*)tms->hd;void*_tmp5C;
_LL37: {struct Cyc_Absyn_Function_mod_struct*_tmp5B=(struct Cyc_Absyn_Function_mod_struct*)
_tmp5A;if(_tmp5B->tag != 3)goto _LL39;else{_tmp5C=(void*)_tmp5B->f1;}}_LL38: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmp5D=_tmp5C;
struct Cyc_List_List*_tmp60;_LL3C: {struct Cyc_Absyn_WithTypes_struct*_tmp5E=(
struct Cyc_Absyn_WithTypes_struct*)_tmp5D;if(_tmp5E->tag != 1)goto _LL3E;}_LL3D:{
const char*_tmp826;void*_tmp825;(_tmp825=0,Cyc_Tcutil_warn(loc,((_tmp826="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dyneither(_tmp826,sizeof(char),93))),_tag_dyneither(_tmp825,sizeof(void*),0)));}
return tms;_LL3E: {struct Cyc_Absyn_NoTypes_struct*_tmp5F=(struct Cyc_Absyn_NoTypes_struct*)
_tmp5D;if(_tmp5F->tag != 0)goto _LL3B;else{_tmp60=_tmp5F->f1;}}_LL3F: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp60)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp829;void*_tmp828;(_tmp828=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
loc,((_tmp829="wrong number of parameter declarations in old-style function declaration",
_tag_dyneither(_tmp829,sizeof(char),73))),_tag_dyneither(_tmp828,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp65=0;for(0;_tmp60 != 0;_tmp60=_tmp60->tl){struct Cyc_List_List*
_tmp66=tds;for(0;_tmp66 != 0;_tmp66=_tmp66->tl){struct Cyc_Absyn_Decl*_tmp67=(
struct Cyc_Absyn_Decl*)_tmp66->hd;void*_tmp68=_tmp67->r;struct Cyc_Absyn_Vardecl*
_tmp6A;_LL41: {struct Cyc_Absyn_Var_d_struct*_tmp69=(struct Cyc_Absyn_Var_d_struct*)
_tmp68;if(_tmp69->tag != 0)goto _LL43;else{_tmp6A=_tmp69->f1;}}_LL42: if(Cyc_zstrptrcmp((*
_tmp6A->name).f2,(struct _dyneither_ptr*)_tmp60->hd)!= 0)continue;if(_tmp6A->initializer
!= 0){const char*_tmp82C;void*_tmp82B;(_tmp82B=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp67->loc,((
_tmp82C="initializer found in parameter declaration",_tag_dyneither(_tmp82C,
sizeof(char),43))),_tag_dyneither(_tmp82B,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmp6A->name)){const char*_tmp82F;void*_tmp82E;(_tmp82E=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp67->loc,((
_tmp82F="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp82F,
sizeof(char),47))),_tag_dyneither(_tmp82E,sizeof(void*),0)));}{struct _tuple8*
_tmp835;struct Cyc_Core_Opt*_tmp834;struct Cyc_List_List*_tmp833;_tmp65=((_tmp833=
_cycalloc(sizeof(*_tmp833)),((_tmp833->hd=((_tmp835=_cycalloc(sizeof(*_tmp835)),((
_tmp835->f1=((_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834->v=(*_tmp6A->name).f2,
_tmp834)))),((_tmp835->f2=_tmp6A->tq,((_tmp835->f3=_tmp6A->type,_tmp835)))))))),((
_tmp833->tl=_tmp65,_tmp833))))));}goto L;_LL43:;_LL44: {const char*_tmp838;void*
_tmp837;(_tmp837=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp67->loc,((_tmp838="nonvariable declaration in parameter type",
_tag_dyneither(_tmp838,sizeof(char),42))),_tag_dyneither(_tmp837,sizeof(void*),0)));}
_LL40:;}L: if(_tmp66 == 0){const char*_tmp83C;void*_tmp83B[1];struct Cyc_String_pa_struct
_tmp83A;(_tmp83A.tag=0,((_tmp83A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp60->hd)),((_tmp83B[0]=& _tmp83A,((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((
_tmp83C="%s is not given a type",_tag_dyneither(_tmp83C,sizeof(char),23))),
_tag_dyneither(_tmp83B,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp84B;struct Cyc_Absyn_WithTypes_struct*_tmp84A;struct Cyc_Absyn_WithTypes_struct
_tmp849;struct Cyc_Absyn_Function_mod_struct _tmp848;struct Cyc_List_List*_tmp847;
return(_tmp847=_cycalloc(sizeof(*_tmp847)),((_tmp847->hd=(void*)((_tmp84B=
_cycalloc(sizeof(*_tmp84B)),((_tmp84B[0]=((_tmp848.tag=3,((_tmp848.f1=(void*)((
void*)((_tmp84A=_cycalloc(sizeof(*_tmp84A)),((_tmp84A[0]=((_tmp849.tag=1,((
_tmp849.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp65),((_tmp849.f2=0,((_tmp849.f3=0,((_tmp849.f4=0,((_tmp849.f5=0,_tmp849)))))))))))),
_tmp84A))))),_tmp848)))),_tmp84B)))),((_tmp847->tl=0,_tmp847)))));}}_LL3B:;}goto
_LL3A;_LL39:;_LL3A: {struct Cyc_List_List*_tmp84C;return(_tmp84C=_cycalloc(
sizeof(*_tmp84C)),((_tmp84C->hd=(void*)tms->hd,((_tmp84C->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp84C)))));}_LL36:;}}struct _tuple17{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc);static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0){struct Cyc_Parse_Declarator*_tmp84D;d=((_tmp84D=_cycalloc(sizeof(*
_tmp84D)),((_tmp84D->id=d->id,((_tmp84D->tms=Cyc_Parse_oldstyle2newstyle(d->tms,
tds,loc),_tmp84D))))));}{enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;struct Cyc_List_List*
tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*
atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;
tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)
dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;
if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)switch((enum Cyc_Parse_Storage_class)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){
case Cyc_Parse_Extern_sc: _LL45: sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc:
_LL46: sc=Cyc_Absyn_Static;break;default: _LL47:{const char*_tmp84E;Cyc_Parse_err(((
_tmp84E="bad storage class on function",_tag_dyneither(_tmp84E,sizeof(char),30))),
loc);}break;}}{void*_tmp80;struct Cyc_Core_Opt*_tmp81;struct _tuple11 _tmp7F=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp80=_tmp7F.f1;_tmp81=_tmp7F.f2;{struct Cyc_Absyn_Tqual _tmp83;void*
_tmp84;struct Cyc_List_List*_tmp85;struct Cyc_List_List*_tmp86;struct _tuple12
_tmp82=Cyc_Parse_apply_tms(tq,_tmp80,atts,d->tms);_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;
_tmp85=_tmp82.f3;_tmp86=_tmp82.f4;if(_tmp81 != 0){const char*_tmp851;void*_tmp850;(
_tmp850=0,Cyc_Tcutil_warn(loc,((_tmp851="nested type declaration within function prototype",
_tag_dyneither(_tmp851,sizeof(char),50))),_tag_dyneither(_tmp850,sizeof(void*),0)));}
if(_tmp85 != 0){const char*_tmp854;void*_tmp853;(_tmp853=0,Cyc_Tcutil_warn(loc,((
_tmp854="bad type params, ignoring",_tag_dyneither(_tmp854,sizeof(char),26))),
_tag_dyneither(_tmp853,sizeof(void*),0)));}{void*_tmp8B=_tmp84;struct Cyc_Absyn_FnInfo
_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Core_Opt*_tmp8F;void*_tmp90;struct
Cyc_List_List*_tmp91;int _tmp92;struct Cyc_Absyn_VarargInfo*_tmp93;struct Cyc_List_List*
_tmp94;struct Cyc_List_List*_tmp95;_LL4A: {struct Cyc_Absyn_FnType_struct*_tmp8C=(
struct Cyc_Absyn_FnType_struct*)_tmp8B;if(_tmp8C->tag != 10)goto _LL4C;else{_tmp8D=
_tmp8C->f1;_tmp8E=_tmp8D.tvars;_tmp8F=_tmp8D.effect;_tmp90=_tmp8D.ret_typ;_tmp91=
_tmp8D.args;_tmp92=_tmp8D.c_varargs;_tmp93=_tmp8D.cyc_varargs;_tmp94=_tmp8D.rgn_po;
_tmp95=_tmp8D.attributes;}}_LL4B: {struct Cyc_List_List*_tmp96=0;{struct Cyc_List_List*
_tmp97=_tmp91;for(0;_tmp97 != 0;_tmp97=_tmp97->tl){struct _tuple8 _tmp99;struct Cyc_Core_Opt*
_tmp9A;struct Cyc_Absyn_Tqual _tmp9B;void*_tmp9C;struct _tuple8*_tmp98=(struct
_tuple8*)_tmp97->hd;_tmp99=*_tmp98;_tmp9A=_tmp99.f1;_tmp9B=_tmp99.f2;_tmp9C=
_tmp99.f3;if(_tmp9A == 0){{const char*_tmp855;Cyc_Parse_err(((_tmp855="missing argument variable in function prototype",
_tag_dyneither(_tmp855,sizeof(char),48))),loc);}{struct _tuple17*_tmp858;struct
Cyc_List_List*_tmp857;_tmp96=((_tmp857=_cycalloc(sizeof(*_tmp857)),((_tmp857->hd=((
_tmp858=_cycalloc(sizeof(*_tmp858)),((_tmp858->f1=_init_dyneither_ptr(_cycalloc(
sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp858->f2=_tmp9B,((_tmp858->f3=
_tmp9C,_tmp858)))))))),((_tmp857->tl=_tmp96,_tmp857))))));}}else{struct _tuple17*
_tmp85B;struct Cyc_List_List*_tmp85A;_tmp96=((_tmp85A=_cycalloc(sizeof(*_tmp85A)),((
_tmp85A->hd=((_tmp85B=_cycalloc(sizeof(*_tmp85B)),((_tmp85B->f1=(struct
_dyneither_ptr*)_tmp9A->v,((_tmp85B->f2=_tmp9B,((_tmp85B->f3=_tmp9C,_tmp85B)))))))),((
_tmp85A->tl=_tmp96,_tmp85A))))));}}}{struct Cyc_Absyn_Fndecl*_tmp85C;return(
_tmp85C=_cycalloc(sizeof(*_tmp85C)),((_tmp85C->sc=sc,((_tmp85C->name=d->id,((
_tmp85C->tvs=_tmp8E,((_tmp85C->is_inline=is_inline,((_tmp85C->effect=_tmp8F,((
_tmp85C->ret_type=_tmp90,((_tmp85C->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp96),((_tmp85C->c_varargs=_tmp92,((_tmp85C->cyc_varargs=
_tmp93,((_tmp85C->rgn_po=_tmp94,((_tmp85C->body=body,((_tmp85C->cached_typ=0,((
_tmp85C->param_vardecls=0,((_tmp85C->fn_vardecl=0,((_tmp85C->attributes=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp95,_tmp86),_tmp85C)))))))))))))))))))))))))))))));}}_LL4C:;_LL4D: {const char*
_tmp85F;void*_tmp85E;(_tmp85E=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp85F="declarator is not a function prototype",
_tag_dyneither(_tmp85F,sizeof(char),39))),_tag_dyneither(_tmp85E,sizeof(void*),0)));}
_LL49:;}}}}}static char _tmpA6[52]="at most one type may appear within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpA6,_tmpA6,_tmpA6 + 52};static char
_tmpA7[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpA7,_tmpA7,_tmpA7 + 63};static char
_tmpA8[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmpA8,_tmpA8,_tmpA8 + 50};static char _tmpA9[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpA9,_tmpA9,_tmpA9 + 60};static struct
_tuple11 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc);static struct _tuple11 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*
ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;
int seen_sign=0;int seen_size=0;void*t=(void*)& Cyc_Absyn_VoidType_val;enum Cyc_Absyn_Size_of
sz=Cyc_Absyn_Int_sz;enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmpAA=(void*)ts->hd;void*_tmpAC;
struct Cyc_Position_Segment*_tmpAD;struct Cyc_Position_Segment*_tmpAF;struct Cyc_Position_Segment*
_tmpB1;struct Cyc_Position_Segment*_tmpB3;struct Cyc_Position_Segment*_tmpB5;
struct Cyc_Absyn_Decl*_tmpB7;_LL4F: {struct Cyc_Parse_Type_spec_struct*_tmpAB=(
struct Cyc_Parse_Type_spec_struct*)_tmpAA;if(_tmpAB->tag != 4)goto _LL51;else{
_tmpAC=(void*)_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL50: if(seen_type)Cyc_Parse_err(
Cyc_Parse_msg1,_tmpAD);last_loc=_tmpAD;seen_type=1;t=_tmpAC;goto _LL4E;_LL51: {
struct Cyc_Parse_Signed_spec_struct*_tmpAE=(struct Cyc_Parse_Signed_spec_struct*)
_tmpAA;if(_tmpAE->tag != 0)goto _LL53;else{_tmpAF=_tmpAE->f1;}}_LL52: if(seen_sign)
Cyc_Parse_err(Cyc_Parse_msg4,_tmpAF);if(seen_type){const char*_tmp860;Cyc_Parse_err(((
_tmp860="signed qualifier must come before type",_tag_dyneither(_tmp860,sizeof(
char),39))),_tmpAF);}last_loc=_tmpAF;seen_sign=1;sgn=Cyc_Absyn_Signed;goto _LL4E;
_LL53: {struct Cyc_Parse_Unsigned_spec_struct*_tmpB0=(struct Cyc_Parse_Unsigned_spec_struct*)
_tmpAA;if(_tmpB0->tag != 1)goto _LL55;else{_tmpB1=_tmpB0->f1;}}_LL54: if(seen_sign)
Cyc_Parse_err(Cyc_Parse_msg4,_tmpB1);if(seen_type){const char*_tmp861;Cyc_Parse_err(((
_tmp861="signed qualifier must come before type",_tag_dyneither(_tmp861,sizeof(
char),39))),_tmpB1);}last_loc=_tmpB1;seen_sign=1;sgn=Cyc_Absyn_Unsigned;goto
_LL4E;_LL55: {struct Cyc_Parse_Short_spec_struct*_tmpB2=(struct Cyc_Parse_Short_spec_struct*)
_tmpAA;if(_tmpB2->tag != 2)goto _LL57;else{_tmpB3=_tmpB2->f1;}}_LL56: if(seen_size){
const char*_tmp862;Cyc_Parse_err(((_tmp862="integral size may appear only once within a type specifier",
_tag_dyneither(_tmp862,sizeof(char),59))),_tmpB3);}if(seen_type){const char*
_tmp863;Cyc_Parse_err(((_tmp863="short modifier must come before base type",
_tag_dyneither(_tmp863,sizeof(char),42))),_tmpB3);}last_loc=_tmpB3;seen_size=1;
sz=Cyc_Absyn_Short_sz;goto _LL4E;_LL57: {struct Cyc_Parse_Long_spec_struct*_tmpB4=(
struct Cyc_Parse_Long_spec_struct*)_tmpAA;if(_tmpB4->tag != 3)goto _LL59;else{
_tmpB5=_tmpB4->f1;}}_LL58: if(seen_type){const char*_tmp864;Cyc_Parse_err(((
_tmp864="long modifier must come before base type",_tag_dyneither(_tmp864,
sizeof(char),41))),_tmpB5);}if(seen_size)switch(sz){case Cyc_Absyn_Long_sz: _LL5B:
sz=Cyc_Absyn_LongLong_sz;break;default: _LL5C:{const char*_tmp865;Cyc_Parse_err(((
_tmp865="extra long in type specifier",_tag_dyneither(_tmp865,sizeof(char),29))),
_tmpB5);}break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmpB5;seen_size=1;goto _LL4E;
_LL59: {struct Cyc_Parse_Decl_spec_struct*_tmpB6=(struct Cyc_Parse_Decl_spec_struct*)
_tmpAA;if(_tmpB6->tag != 5)goto _LL4E;else{_tmpB7=_tmpB6->f1;}}_LL5A: last_loc=
_tmpB7->loc;if(declopt == 0  && !seen_type){seen_type=1;{void*_tmpBE=_tmpB7->r;
struct Cyc_Absyn_Aggrdecl*_tmpC0;struct Cyc_Absyn_Datatypedecl*_tmpC2;struct Cyc_Absyn_Enumdecl*
_tmpC4;_LL5F: {struct Cyc_Absyn_Aggr_d_struct*_tmpBF=(struct Cyc_Absyn_Aggr_d_struct*)
_tmpBE;if(_tmpBF->tag != 6)goto _LL61;else{_tmpC0=_tmpBF->f1;}}_LL60: {struct Cyc_List_List*
_tmpC5=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpC0->tvs));{struct Cyc_Absyn_AggrType_struct _tmp86B;struct Cyc_Absyn_AggrInfo
_tmp86A;struct Cyc_Absyn_AggrType_struct*_tmp869;t=(void*)((_tmp869=_cycalloc(
sizeof(*_tmp869)),((_tmp869[0]=((_tmp86B.tag=12,((_tmp86B.f1=((_tmp86A.aggr_info=
Cyc_Absyn_UnknownAggr(_tmpC0->kind,_tmpC0->name,0),((_tmp86A.targs=_tmpC5,
_tmp86A)))),_tmp86B)))),_tmp869))));}if(_tmpC0->impl != 0){struct Cyc_Core_Opt*
_tmp86C;declopt=((_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C->v=_tmpB7,
_tmp86C))));}goto _LL5E;}_LL61: {struct Cyc_Absyn_Datatype_d_struct*_tmpC1=(struct
Cyc_Absyn_Datatype_d_struct*)_tmpBE;if(_tmpC1->tag != 7)goto _LL63;else{_tmpC2=
_tmpC1->f1;}}_LL62: {struct Cyc_List_List*_tmpCA=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpC2->tvs));{struct Cyc_Absyn_DatatypeType_struct
_tmp876;struct Cyc_Absyn_Datatypedecl**_tmp875;struct Cyc_Absyn_DatatypeInfo
_tmp874;struct Cyc_Absyn_DatatypeType_struct*_tmp873;t=(void*)((_tmp873=_cycalloc(
sizeof(*_tmp873)),((_tmp873[0]=((_tmp876.tag=3,((_tmp876.f1=((_tmp874.datatype_info=
Cyc_Absyn_KnownDatatype(((_tmp875=_cycalloc(sizeof(*_tmp875)),((_tmp875[0]=
_tmpC2,_tmp875))))),((_tmp874.targs=_tmpCA,_tmp874)))),_tmp876)))),_tmp873))));}
if(_tmpC2->fields != 0){struct Cyc_Core_Opt*_tmp877;declopt=((_tmp877=_cycalloc(
sizeof(*_tmp877)),((_tmp877->v=_tmpB7,_tmp877))));}goto _LL5E;}_LL63: {struct Cyc_Absyn_Enum_d_struct*
_tmpC3=(struct Cyc_Absyn_Enum_d_struct*)_tmpBE;if(_tmpC3->tag != 8)goto _LL65;else{
_tmpC4=_tmpC3->f1;}}_LL64:{struct Cyc_Absyn_EnumType_struct _tmp87A;struct Cyc_Absyn_EnumType_struct*
_tmp879;t=(void*)((_tmp879=_cycalloc(sizeof(*_tmp879)),((_tmp879[0]=((_tmp87A.tag=
14,((_tmp87A.f1=_tmpC4->name,((_tmp87A.f2=0,_tmp87A)))))),_tmp879))));}{struct
Cyc_Core_Opt*_tmp87B;declopt=((_tmp87B=_cycalloc(sizeof(*_tmp87B)),((_tmp87B->v=
_tmpB7,_tmp87B))));}goto _LL5E;_LL65:;_LL66: {const char*_tmp87E;void*_tmp87D;(
_tmp87D=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(_tmpB7->loc,((_tmp87E="bad declaration within type specifier",
_tag_dyneither(_tmp87E,sizeof(char),38))),_tag_dyneither(_tmp87D,sizeof(void*),0)));}
_LL5E:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmpB7->loc);}goto _LL4E;_LL4E:;}if(!
seen_type){if(!seen_sign  && !seen_size){const char*_tmp881;void*_tmp880;(_tmp880=
0,Cyc_Tcutil_warn(last_loc,((_tmp881="missing type within specifier",
_tag_dyneither(_tmp881,sizeof(char),30))),_tag_dyneither(_tmp880,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*_tmpD7=t;enum Cyc_Absyn_Sign
_tmpD9;enum Cyc_Absyn_Size_of _tmpDA;_LL68: {struct Cyc_Absyn_IntType_struct*
_tmpD8=(struct Cyc_Absyn_IntType_struct*)_tmpD7;if(_tmpD8->tag != 6)goto _LL6A;
else{_tmpD9=_tmpD8->f1;_tmpDA=_tmpD8->f2;}}_LL69: if(_tmpD9 != sgn)t=Cyc_Absyn_int_typ(
sgn,_tmpDA);goto _LL67;_LL6A:;_LL6B:{const char*_tmp882;Cyc_Parse_err(((_tmp882="sign specification on non-integral type",
_tag_dyneither(_tmp882,sizeof(char),40))),last_loc);}goto _LL67;_LL67:;}if(
seen_size){void*_tmpDC=t;enum Cyc_Absyn_Sign _tmpDE;enum Cyc_Absyn_Size_of _tmpDF;
_LL6D: {struct Cyc_Absyn_IntType_struct*_tmpDD=(struct Cyc_Absyn_IntType_struct*)
_tmpDC;if(_tmpDD->tag != 6)goto _LL6F;else{_tmpDE=_tmpDD->f1;_tmpDF=_tmpDD->f2;}}
_LL6E: if(_tmpDF != sz)t=Cyc_Absyn_int_typ(_tmpDE,sz);goto _LL6C;_LL6F: {struct Cyc_Absyn_DoubleType_struct*
_tmpE0=(struct Cyc_Absyn_DoubleType_struct*)_tmpDC;if(_tmpE0->tag != 8)goto _LL71;}
_LL70: t=Cyc_Absyn_double_typ(1);goto _LL6C;_LL71:;_LL72:{const char*_tmp883;Cyc_Parse_err(((
_tmp883="size qualifier on non-integral type",_tag_dyneither(_tmp883,sizeof(char),
36))),last_loc);}goto _LL6C;_LL6C:;}}{struct _tuple11 _tmp884;return(_tmp884.f1=t,((
_tmp884.f2=declopt,_tmp884)));}}static struct Cyc_List_List*Cyc_Parse_apply_tmss(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts);static struct Cyc_List_List*Cyc_Parse_apply_tmss(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpE3=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpE4=_tmpE3->id;
struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;struct Cyc_List_List*
_tmpE9;struct _tuple12 _tmpE5=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpE3->tms);
_tmpE6=_tmpE5.f1;_tmpE7=_tmpE5.f2;_tmpE8=_tmpE5.f3;_tmpE9=_tmpE5.f4;if(ds->tl == 
0){struct _tuple14*_tmp887;struct Cyc_List_List*_tmp886;return(_tmp886=
_region_malloc(r,sizeof(*_tmp886)),((_tmp886->hd=((_tmp887=_region_malloc(r,
sizeof(*_tmp887)),((_tmp887->f1=_tmpE4,((_tmp887->f2=_tmpE6,((_tmp887->f3=_tmpE7,((
_tmp887->f4=_tmpE8,((_tmp887->f5=_tmpE9,_tmp887)))))))))))),((_tmp886->tl=0,
_tmp886)))));}else{struct _tuple14*_tmp88A;struct Cyc_List_List*_tmp889;return(
_tmp889=_region_malloc(r,sizeof(*_tmp889)),((_tmp889->hd=((_tmp88A=
_region_malloc(r,sizeof(*_tmp88A)),((_tmp88A->f1=_tmpE4,((_tmp88A->f2=_tmpE6,((
_tmp88A->f3=_tmpE7,((_tmp88A->f4=_tmpE8,((_tmp88A->f5=_tmpE9,_tmp88A)))))))))))),((
_tmp889->tl=Cyc_Parse_apply_tmss(r,_tmpE6,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts),_tmp889)))));}}}static struct _tuple12 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms);static struct _tuple12
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,
struct Cyc_List_List*tms){if(tms == 0){struct _tuple12 _tmp88B;return(_tmp88B.f1=tq,((
_tmp88B.f2=t,((_tmp88B.f3=0,((_tmp88B.f4=atts,_tmp88B)))))));}{void*_tmpEF=(void*)
tms->hd;union Cyc_Absyn_Constraint*_tmpF1;struct Cyc_Position_Segment*_tmpF2;
struct Cyc_Absyn_Exp*_tmpF4;union Cyc_Absyn_Constraint*_tmpF5;struct Cyc_Position_Segment*
_tmpF6;void*_tmpF8;struct Cyc_List_List*_tmpFA;struct Cyc_Position_Segment*_tmpFB;
struct Cyc_Absyn_PtrAtts _tmpFD;struct Cyc_Absyn_Tqual _tmpFE;struct Cyc_Position_Segment*
_tmp100;struct Cyc_List_List*_tmp101;_LL74: {struct Cyc_Absyn_Carray_mod_struct*
_tmpF0=(struct Cyc_Absyn_Carray_mod_struct*)_tmpEF;if(_tmpF0->tag != 0)goto _LL76;
else{_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;}}_LL75: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_array_typ(t,tq,0,_tmpF1,_tmpF2),atts,tms->tl);_LL76: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF3=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpEF;if(_tmpF3->tag != 1)goto
_LL78;else{_tmpF4=_tmpF3->f1;_tmpF5=_tmpF3->f2;_tmpF6=_tmpF3->f3;}}_LL77: return
Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)
_tmpF4,_tmpF5,_tmpF6),atts,tms->tl);_LL78: {struct Cyc_Absyn_Function_mod_struct*
_tmpF7=(struct Cyc_Absyn_Function_mod_struct*)_tmpEF;if(_tmpF7->tag != 3)goto _LL7A;
else{_tmpF8=(void*)_tmpF7->f1;}}_LL79: {void*_tmp102=_tmpF8;struct Cyc_List_List*
_tmp104;int _tmp105;struct Cyc_Absyn_VarargInfo*_tmp106;struct Cyc_Core_Opt*_tmp107;
struct Cyc_List_List*_tmp108;struct Cyc_Position_Segment*_tmp10A;_LL81: {struct Cyc_Absyn_WithTypes_struct*
_tmp103=(struct Cyc_Absyn_WithTypes_struct*)_tmp102;if(_tmp103->tag != 1)goto _LL83;
else{_tmp104=_tmp103->f1;_tmp105=_tmp103->f2;_tmp106=_tmp103->f3;_tmp107=_tmp103->f4;
_tmp108=_tmp103->f5;}}_LL82: {struct Cyc_List_List*typvars=0;struct Cyc_List_List*
fn_atts=0;struct Cyc_List_List*new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 
0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd)){struct Cyc_List_List*_tmp88C;
fn_atts=((_tmp88C=_cycalloc(sizeof(*_tmp88C)),((_tmp88C->hd=(void*)as->hd,((
_tmp88C->tl=fn_atts,_tmp88C))))));}else{struct Cyc_List_List*_tmp88D;new_atts=((
_tmp88D=_cycalloc(sizeof(*_tmp88D)),((_tmp88D->hd=(void*)as->hd,((_tmp88D->tl=
new_atts,_tmp88D))))));}}}if(tms->tl != 0){void*_tmp10D=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmp10F;_LL86: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp10E=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp10D;if(_tmp10E->tag != 4)goto
_LL88;else{_tmp10F=_tmp10E->f1;}}_LL87: typvars=_tmp10F;tms=tms->tl;goto _LL85;
_LL88:;_LL89: goto _LL85;_LL85:;}if(((((!_tmp105  && _tmp106 == 0) && _tmp104 != 0)
 && _tmp104->tl == 0) && (*((struct _tuple8*)_tmp104->hd)).f1 == 0) && (*((struct
_tuple8*)_tmp104->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)_tmp104=0;t=Cyc_Parse_array2ptr(
t,0);((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Parse_arg_array2ptr,_tmp104);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmp107,t,_tmp104,_tmp105,_tmp106,_tmp108,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL83: {struct
Cyc_Absyn_NoTypes_struct*_tmp109=(struct Cyc_Absyn_NoTypes_struct*)_tmp102;if(
_tmp109->tag != 0)goto _LL80;else{_tmp10A=_tmp109->f2;}}_LL84: {const char*_tmp890;
void*_tmp88F;(_tmp88F=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp10A,((_tmp890="function declaration without parameter types",
_tag_dyneither(_tmp890,sizeof(char),45))),_tag_dyneither(_tmp88F,sizeof(void*),0)));}
_LL80:;}_LL7A: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpF9=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpEF;if(_tmpF9->tag != 4)goto _LL7C;else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpF9->f2;}}
_LL7B: if(tms->tl == 0){struct _tuple12 _tmp891;return(_tmp891.f1=tq,((_tmp891.f2=t,((
_tmp891.f3=_tmpFA,((_tmp891.f4=atts,_tmp891)))))));}{const char*_tmp894;void*
_tmp893;(_tmp893=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpFB,((_tmp894="type parameters must appear before function arguments in declarator",
_tag_dyneither(_tmp894,sizeof(char),68))),_tag_dyneither(_tmp893,sizeof(void*),0)));}
_LL7C: {struct Cyc_Absyn_Pointer_mod_struct*_tmpFC=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpEF;if(_tmpFC->tag != 2)goto _LL7E;else{_tmpFD=_tmpFC->f1;_tmpFE=_tmpFC->f2;}}
_LL7D: {struct Cyc_Absyn_PointerType_struct _tmp89A;struct Cyc_Absyn_PtrInfo _tmp899;
struct Cyc_Absyn_PointerType_struct*_tmp898;return Cyc_Parse_apply_tms(_tmpFE,(
void*)((_tmp898=_cycalloc(sizeof(*_tmp898)),((_tmp898[0]=((_tmp89A.tag=5,((
_tmp89A.f1=((_tmp899.elt_typ=t,((_tmp899.elt_tq=tq,((_tmp899.ptr_atts=_tmpFD,
_tmp899)))))),_tmp89A)))),_tmp898)))),atts,tms->tl);}_LL7E: {struct Cyc_Absyn_Attributes_mod_struct*
_tmpFF=(struct Cyc_Absyn_Attributes_mod_struct*)_tmpEF;if(_tmpFF->tag != 5)goto
_LL73;else{_tmp100=_tmpFF->f1;_tmp101=_tmpFF->f2;}}_LL7F: return Cyc_Parse_apply_tms(
tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
atts,_tmp101),tms->tl);_LL73:;}}void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc);void*Cyc_Parse_speclist2typ(struct Cyc_List_List*
tss,struct Cyc_Position_Segment*loc){void*_tmp119;struct Cyc_Core_Opt*_tmp11A;
struct _tuple11 _tmp118=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmp119=
_tmp118.f1;_tmp11A=_tmp118.f2;if(_tmp11A != 0){const char*_tmp89D;void*_tmp89C;(
_tmp89C=0,Cyc_Tcutil_warn(loc,((_tmp89D="ignoring nested type declaration(s) in specifier list",
_tag_dyneither(_tmp89D,sizeof(char),54))),_tag_dyneither(_tmp89C,sizeof(void*),0)));}
return _tmp119;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple14*t);static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple14*t){struct _tuple14 _tmp11E;struct
_tuple1*_tmp11F;struct Cyc_Absyn_Tqual _tmp120;void*_tmp121;struct Cyc_List_List*
_tmp122;struct Cyc_List_List*_tmp123;struct _tuple14*_tmp11D=t;_tmp11E=*_tmp11D;
_tmp11F=_tmp11E.f1;_tmp120=_tmp11E.f2;_tmp121=_tmp11E.f3;_tmp122=_tmp11E.f4;
_tmp123=_tmp11E.f5;Cyc_Lex_register_typedef(_tmp11F);{struct Cyc_Core_Opt*kind;
struct Cyc_Core_Opt*type;{void*_tmp124=_tmp121;struct Cyc_Core_Opt*_tmp126;_LL8B: {
struct Cyc_Absyn_Evar_struct*_tmp125=(struct Cyc_Absyn_Evar_struct*)_tmp124;if(
_tmp125->tag != 1)goto _LL8D;else{_tmp126=_tmp125->f1;}}_LL8C: type=0;if(_tmp126 == 
0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;else{kind=_tmp126;}goto _LL8A;_LL8D:;
_LL8E: kind=0;{struct Cyc_Core_Opt*_tmp89E;type=((_tmp89E=_cycalloc(sizeof(*
_tmp89E)),((_tmp89E->v=_tmp121,_tmp89E))));}goto _LL8A;_LL8A:;}{struct Cyc_Absyn_Typedef_d_struct
_tmp8A4;struct Cyc_Absyn_Typedefdecl*_tmp8A3;struct Cyc_Absyn_Typedef_d_struct*
_tmp8A2;return Cyc_Absyn_new_decl((void*)((_tmp8A2=_cycalloc(sizeof(*_tmp8A2)),((
_tmp8A2[0]=((_tmp8A4.tag=9,((_tmp8A4.f1=((_tmp8A3=_cycalloc(sizeof(*_tmp8A3)),((
_tmp8A3->name=_tmp11F,((_tmp8A3->tvs=_tmp122,((_tmp8A3->kind=kind,((_tmp8A3->defn=
type,((_tmp8A3->atts=_tmp123,((_tmp8A3->tq=_tmp120,_tmp8A3)))))))))))))),_tmp8A4)))),
_tmp8A2)))),loc);}}}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct
_tmp8A7;struct Cyc_Absyn_Decl_s_struct*_tmp8A6;return Cyc_Absyn_new_stmt((void*)((
_tmp8A6=_cycalloc(sizeof(*_tmp8A6)),((_tmp8A6[0]=((_tmp8A7.tag=12,((_tmp8A7.f1=d,((
_tmp8A7.f2=s,_tmp8A7)))))),_tmp8A6)))),d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc);static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct _RegionHandle _tmp12D=_new_region("mkrgn");struct _RegionHandle*mkrgn=&
_tmp12D;_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp12F;struct Cyc_Absyn_Tqual
_tmp130;struct Cyc_List_List*_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_Parse_Declaration_spec*
_tmp12E=ds;_tmp12F=*_tmp12E;_tmp130=_tmp12F.tq;_tmp131=_tmp12F.type_specs;
_tmp132=_tmp12F.attributes;if(_tmp130.loc == 0)_tmp130.loc=tqual_loc;if(ds->is_inline){
const char*_tmp8A8;Cyc_Parse_err(((_tmp8A8="inline is allowed only on function definitions",
_tag_dyneither(_tmp8A8,sizeof(char),47))),loc);}if(_tmp131 == 0){{const char*
_tmp8A9;Cyc_Parse_err(((_tmp8A9="missing type specifiers in declaration",
_tag_dyneither(_tmp8A9,sizeof(char),39))),loc);}{struct Cyc_List_List*_tmp135=0;
_npop_handler(0);return _tmp135;}}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int
istypedef=0;if(ds->sc != 0)switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v){case Cyc_Parse_Typedef_sc: _LL8F: istypedef=1;break;case
Cyc_Parse_Extern_sc: _LL90: s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc:
_LL91: s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LL92: s=Cyc_Absyn_Static;
break;case Cyc_Parse_Auto_sc: _LL93: s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc:
_LL94: if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}
break;case Cyc_Parse_Abstract_sc: _LL95: s=Cyc_Absyn_Abstract;break;}{struct Cyc_List_List*
_tmp137;struct Cyc_List_List*_tmp138;struct _tuple0 _tmp136=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
mkrgn,mkrgn,ids);_tmp137=_tmp136.f1;_tmp138=_tmp136.f2;{int exps_empty=1;{struct
Cyc_List_List*es=_tmp138;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd
!= 0){exps_empty=0;break;}}}{struct _tuple11 _tmp139=Cyc_Parse_collapse_type_specifiers(
_tmp131,loc);if(_tmp137 == 0){void*_tmp13B;struct Cyc_Core_Opt*_tmp13C;struct
_tuple11 _tmp13A=_tmp139;_tmp13B=_tmp13A.f1;_tmp13C=_tmp13A.f2;if(_tmp13C != 0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp13C->v;{void*_tmp13D=d->r;
struct Cyc_Absyn_Enumdecl*_tmp13F;struct Cyc_Absyn_Aggrdecl*_tmp141;struct Cyc_Absyn_Datatypedecl*
_tmp143;_LL98: {struct Cyc_Absyn_Enum_d_struct*_tmp13E=(struct Cyc_Absyn_Enum_d_struct*)
_tmp13D;if(_tmp13E->tag != 8)goto _LL9A;else{_tmp13F=_tmp13E->f1;}}_LL99: _tmp13F->sc=
s;if(_tmp132 != 0){const char*_tmp8AA;Cyc_Parse_err(((_tmp8AA="bad attributes on enum",
_tag_dyneither(_tmp8AA,sizeof(char),23))),loc);}goto _LL97;_LL9A: {struct Cyc_Absyn_Aggr_d_struct*
_tmp140=(struct Cyc_Absyn_Aggr_d_struct*)_tmp13D;if(_tmp140->tag != 6)goto _LL9C;
else{_tmp141=_tmp140->f1;}}_LL9B: _tmp141->sc=s;_tmp141->attributes=_tmp132;goto
_LL97;_LL9C: {struct Cyc_Absyn_Datatype_d_struct*_tmp142=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp13D;if(_tmp142->tag != 7)goto _LL9E;else{_tmp143=_tmp142->f1;}}_LL9D: _tmp143->sc=
s;if(_tmp132 != 0){const char*_tmp8AB;Cyc_Parse_err(((_tmp8AB="bad attributes on datatype",
_tag_dyneither(_tmp8AB,sizeof(char),27))),loc);}goto _LL97;_LL9E:;_LL9F:{const
char*_tmp8AC;Cyc_Parse_err(((_tmp8AC="bad declaration",_tag_dyneither(_tmp8AC,
sizeof(char),16))),loc);}{struct Cyc_List_List*_tmp147=0;_npop_handler(0);return
_tmp147;}_LL97:;}{struct Cyc_List_List*_tmp8AD;struct Cyc_List_List*_tmp149=(
_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD->hd=d,((_tmp8AD->tl=0,_tmp8AD)))));
_npop_handler(0);return _tmp149;}}else{void*_tmp14A=_tmp13B;struct Cyc_Absyn_AggrInfo
_tmp14C;union Cyc_Absyn_AggrInfoU _tmp14D;struct _tuple3 _tmp14E;enum Cyc_Absyn_AggrKind
_tmp14F;struct _tuple1*_tmp150;struct Cyc_List_List*_tmp151;struct Cyc_Absyn_DatatypeInfo
_tmp153;union Cyc_Absyn_DatatypeInfoU _tmp154;struct Cyc_Absyn_Datatypedecl**
_tmp155;struct Cyc_Absyn_DatatypeInfo _tmp157;union Cyc_Absyn_DatatypeInfoU _tmp158;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp159;struct _tuple1*_tmp15A;int _tmp15B;
struct Cyc_List_List*_tmp15C;struct _tuple1*_tmp15E;struct Cyc_List_List*_tmp160;
_LLA1: {struct Cyc_Absyn_AggrType_struct*_tmp14B=(struct Cyc_Absyn_AggrType_struct*)
_tmp14A;if(_tmp14B->tag != 12)goto _LLA3;else{_tmp14C=_tmp14B->f1;_tmp14D=_tmp14C.aggr_info;
if((_tmp14D.UnknownAggr).tag != 1)goto _LLA3;_tmp14E=(struct _tuple3)(_tmp14D.UnknownAggr).val;
_tmp14F=_tmp14E.f1;_tmp150=_tmp14E.f2;_tmp151=_tmp14C.targs;}}_LLA2: {struct Cyc_List_List*
_tmp161=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp151);struct Cyc_Absyn_Aggrdecl*_tmp8AE;struct Cyc_Absyn_Aggrdecl*_tmp162=(
_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE->kind=_tmp14F,((_tmp8AE->sc=s,((
_tmp8AE->name=_tmp150,((_tmp8AE->tvs=_tmp161,((_tmp8AE->impl=0,((_tmp8AE->attributes=
0,_tmp8AE)))))))))))));if(_tmp132 != 0){const char*_tmp8AF;Cyc_Parse_err(((_tmp8AF="bad attributes on type declaration",
_tag_dyneither(_tmp8AF,sizeof(char),35))),loc);}{struct Cyc_Absyn_Aggr_d_struct*
_tmp8B5;struct Cyc_Absyn_Aggr_d_struct _tmp8B4;struct Cyc_List_List*_tmp8B3;struct
Cyc_List_List*_tmp167=(_tmp8B3=_cycalloc(sizeof(*_tmp8B3)),((_tmp8B3->hd=Cyc_Absyn_new_decl((
void*)((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5[0]=((_tmp8B4.tag=6,((
_tmp8B4.f1=_tmp162,_tmp8B4)))),_tmp8B5)))),loc),((_tmp8B3->tl=0,_tmp8B3)))));
_npop_handler(0);return _tmp167;}}_LLA3: {struct Cyc_Absyn_DatatypeType_struct*
_tmp152=(struct Cyc_Absyn_DatatypeType_struct*)_tmp14A;if(_tmp152->tag != 3)goto
_LLA5;else{_tmp153=_tmp152->f1;_tmp154=_tmp153.datatype_info;if((_tmp154.KnownDatatype).tag
!= 2)goto _LLA5;_tmp155=(struct Cyc_Absyn_Datatypedecl**)(_tmp154.KnownDatatype).val;}}
_LLA4: if(_tmp132 != 0){const char*_tmp8B6;Cyc_Parse_err(((_tmp8B6="bad attributes on datatype",
_tag_dyneither(_tmp8B6,sizeof(char),27))),loc);}{struct Cyc_Absyn_Datatype_d_struct*
_tmp8BC;struct Cyc_Absyn_Datatype_d_struct _tmp8BB;struct Cyc_List_List*_tmp8BA;
struct Cyc_List_List*_tmp16D=(_tmp8BA=_cycalloc(sizeof(*_tmp8BA)),((_tmp8BA->hd=
Cyc_Absyn_new_decl((void*)((_tmp8BC=_cycalloc(sizeof(*_tmp8BC)),((_tmp8BC[0]=((
_tmp8BB.tag=7,((_tmp8BB.f1=*_tmp155,_tmp8BB)))),_tmp8BC)))),loc),((_tmp8BA->tl=0,
_tmp8BA)))));_npop_handler(0);return _tmp16D;}_LLA5: {struct Cyc_Absyn_DatatypeType_struct*
_tmp156=(struct Cyc_Absyn_DatatypeType_struct*)_tmp14A;if(_tmp156->tag != 3)goto
_LLA7;else{_tmp157=_tmp156->f1;_tmp158=_tmp157.datatype_info;if((_tmp158.UnknownDatatype).tag
!= 1)goto _LLA7;_tmp159=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp158.UnknownDatatype).val;
_tmp15A=_tmp159.name;_tmp15B=_tmp159.is_extensible;_tmp15C=_tmp157.targs;}}_LLA6: {
struct Cyc_List_List*_tmp16E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp15C);struct Cyc_Absyn_Decl*_tmp16F=
Cyc_Absyn_datatype_decl(s,_tmp15A,_tmp16E,0,_tmp15B,loc);if(_tmp132 != 0){const
char*_tmp8BD;Cyc_Parse_err(((_tmp8BD="bad attributes on datatype",_tag_dyneither(
_tmp8BD,sizeof(char),27))),loc);}{struct Cyc_List_List*_tmp8BE;struct Cyc_List_List*
_tmp172=(_tmp8BE=_cycalloc(sizeof(*_tmp8BE)),((_tmp8BE->hd=_tmp16F,((_tmp8BE->tl=
0,_tmp8BE)))));_npop_handler(0);return _tmp172;}}_LLA7: {struct Cyc_Absyn_EnumType_struct*
_tmp15D=(struct Cyc_Absyn_EnumType_struct*)_tmp14A;if(_tmp15D->tag != 14)goto _LLA9;
else{_tmp15E=_tmp15D->f1;}}_LLA8: {struct Cyc_Absyn_Enumdecl*_tmp8BF;struct Cyc_Absyn_Enumdecl*
_tmp173=(_tmp8BF=_cycalloc(sizeof(*_tmp8BF)),((_tmp8BF->sc=s,((_tmp8BF->name=
_tmp15E,((_tmp8BF->fields=0,_tmp8BF)))))));if(_tmp132 != 0){const char*_tmp8C0;Cyc_Parse_err(((
_tmp8C0="bad attributes on enum",_tag_dyneither(_tmp8C0,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp8CA;struct Cyc_Absyn_Enum_d_struct _tmp8C9;struct Cyc_Absyn_Enum_d_struct*
_tmp8C8;struct Cyc_List_List*_tmp8C7;struct Cyc_List_List*_tmp179=(_tmp8C7=
_cycalloc(sizeof(*_tmp8C7)),((_tmp8C7->hd=((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((
_tmp8CA->r=(void*)((_tmp8C8=_cycalloc(sizeof(*_tmp8C8)),((_tmp8C8[0]=((_tmp8C9.tag=
8,((_tmp8C9.f1=_tmp173,_tmp8C9)))),_tmp8C8)))),((_tmp8CA->loc=loc,_tmp8CA)))))),((
_tmp8C7->tl=0,_tmp8C7)))));_npop_handler(0);return _tmp179;}}_LLA9: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp15F=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp14A;if(_tmp15F->tag != 15)goto
_LLAB;else{_tmp160=_tmp15F->f1;}}_LLAA: {struct Cyc_Core_Opt*_tmp8CD;struct Cyc_Absyn_Enumdecl*
_tmp8CC;struct Cyc_Absyn_Enumdecl*_tmp17B=(_tmp8CC=_cycalloc(sizeof(*_tmp8CC)),((
_tmp8CC->sc=s,((_tmp8CC->name=Cyc_Parse_gensym_enum(),((_tmp8CC->fields=((
_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD->v=_tmp160,_tmp8CD)))),_tmp8CC)))))));
if(_tmp132 != 0){const char*_tmp8CE;Cyc_Parse_err(((_tmp8CE="bad attributes on enum",
_tag_dyneither(_tmp8CE,sizeof(char),23))),loc);}{struct Cyc_Absyn_Decl*_tmp8D8;
struct Cyc_Absyn_Enum_d_struct _tmp8D7;struct Cyc_Absyn_Enum_d_struct*_tmp8D6;
struct Cyc_List_List*_tmp8D5;struct Cyc_List_List*_tmp181=(_tmp8D5=_cycalloc(
sizeof(*_tmp8D5)),((_tmp8D5->hd=((_tmp8D8=_cycalloc(sizeof(*_tmp8D8)),((_tmp8D8->r=(
void*)((_tmp8D6=_cycalloc(sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D7.tag=8,((
_tmp8D7.f1=_tmp17B,_tmp8D7)))),_tmp8D6)))),((_tmp8D8->loc=loc,_tmp8D8)))))),((
_tmp8D5->tl=0,_tmp8D5)))));_npop_handler(0);return _tmp181;}}_LLAB:;_LLAC:{const
char*_tmp8D9;Cyc_Parse_err(((_tmp8D9="missing declarator",_tag_dyneither(_tmp8D9,
sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp185=0;_npop_handler(0);return
_tmp185;}_LLA0:;}}else{void*t=_tmp139.f1;struct Cyc_List_List*_tmp186=Cyc_Parse_apply_tmss(
mkrgn,_tmp130,t,_tmp137,_tmp132);if(istypedef){if(!exps_empty){const char*_tmp8DA;
Cyc_Parse_err(((_tmp8DA="initializer in typedef declaration",_tag_dyneither(
_tmp8DA,sizeof(char),35))),loc);}{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple14*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp186);if(_tmp139.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp139.f2)->v;{void*_tmp188=d->r;struct Cyc_Absyn_Aggrdecl*_tmp18A;struct Cyc_Absyn_Datatypedecl*
_tmp18C;struct Cyc_Absyn_Enumdecl*_tmp18E;_LLAE: {struct Cyc_Absyn_Aggr_d_struct*
_tmp189=(struct Cyc_Absyn_Aggr_d_struct*)_tmp188;if(_tmp189->tag != 6)goto _LLB0;
else{_tmp18A=_tmp189->f1;}}_LLAF: _tmp18A->sc=s;_tmp18A->attributes=_tmp132;
_tmp132=0;goto _LLAD;_LLB0: {struct Cyc_Absyn_Datatype_d_struct*_tmp18B=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp188;if(_tmp18B->tag != 7)goto _LLB2;else{_tmp18C=_tmp18B->f1;}}_LLB1: _tmp18C->sc=
s;goto _LLAD;_LLB2: {struct Cyc_Absyn_Enum_d_struct*_tmp18D=(struct Cyc_Absyn_Enum_d_struct*)
_tmp188;if(_tmp18D->tag != 8)goto _LLB4;else{_tmp18E=_tmp18D->f1;}}_LLB3: _tmp18E->sc=
s;goto _LLAD;_LLB4:;_LLB5:{const char*_tmp8DB;Cyc_Parse_err(((_tmp8DB="declaration within typedef is not a struct, union, or datatype",
_tag_dyneither(_tmp8DB,sizeof(char),63))),loc);}goto _LLAD;_LLAD:;}{struct Cyc_List_List*
_tmp8DC;decls=((_tmp8DC=_cycalloc(sizeof(*_tmp8DC)),((_tmp8DC->hd=d,((_tmp8DC->tl=
decls,_tmp8DC))))));}}{struct Cyc_List_List*_tmp191=decls;_npop_handler(0);return
_tmp191;}}}else{if(_tmp139.f2 != 0){const char*_tmp8DD;Cyc_Parse_unimp(((_tmp8DD="nested type declaration within declarator",
_tag_dyneither(_tmp8DD,sizeof(char),42))),loc);}{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp193=_tmp186;for(0;_tmp193 != 0;(_tmp193=_tmp193->tl,
_tmp138=_tmp138->tl)){struct _tuple14 _tmp195;struct _tuple1*_tmp196;struct Cyc_Absyn_Tqual
_tmp197;void*_tmp198;struct Cyc_List_List*_tmp199;struct Cyc_List_List*_tmp19A;
struct _tuple14*_tmp194=(struct _tuple14*)_tmp193->hd;_tmp195=*_tmp194;_tmp196=
_tmp195.f1;_tmp197=_tmp195.f2;_tmp198=_tmp195.f3;_tmp199=_tmp195.f4;_tmp19A=
_tmp195.f5;if(_tmp199 != 0){const char*_tmp8E0;void*_tmp8DF;(_tmp8DF=0,Cyc_Tcutil_warn(
loc,((_tmp8E0="bad type params, ignoring",_tag_dyneither(_tmp8E0,sizeof(char),26))),
_tag_dyneither(_tmp8DF,sizeof(void*),0)));}if(_tmp138 == 0){const char*_tmp8E3;
void*_tmp8E2;(_tmp8E2=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,((_tmp8E3="unexpected NULL in parse!",
_tag_dyneither(_tmp8E3,sizeof(char),26))),_tag_dyneither(_tmp8E2,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp19F=(struct Cyc_Absyn_Exp*)_tmp138->hd;struct Cyc_Absyn_Vardecl*
_tmp1A0=Cyc_Absyn_new_vardecl(_tmp196,_tmp198,_tmp19F);_tmp1A0->tq=_tmp197;
_tmp1A0->sc=s;_tmp1A0->attributes=_tmp19A;{struct Cyc_Absyn_Var_d_struct*_tmp8E9;
struct Cyc_Absyn_Var_d_struct _tmp8E8;struct Cyc_Absyn_Decl*_tmp8E7;struct Cyc_Absyn_Decl*
_tmp1A1=(_tmp8E7=_cycalloc(sizeof(*_tmp8E7)),((_tmp8E7->r=(void*)((_tmp8E9=
_cycalloc(sizeof(*_tmp8E9)),((_tmp8E9[0]=((_tmp8E8.tag=0,((_tmp8E8.f1=_tmp1A0,
_tmp8E8)))),_tmp8E9)))),((_tmp8E7->loc=loc,_tmp8E7)))));struct Cyc_List_List*
_tmp8EA;decls=((_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA->hd=_tmp1A1,((
_tmp8EA->tl=decls,_tmp8EA))))));}}}}{struct Cyc_List_List*_tmp1A6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1A6;}}}}}}}}};
_pop_region(mkrgn);}static enum Cyc_Absyn_Kind Cyc_Parse_id_to_kind(struct
_dyneither_ptr s,struct Cyc_Position_Segment*loc);static enum Cyc_Absyn_Kind Cyc_Parse_id_to_kind(
struct _dyneither_ptr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2)switch(*((const
char*)_check_dyneither_subscript(s,sizeof(char),0))){case 'A': _LLB6: return Cyc_Absyn_AnyKind;
case 'M': _LLB7: return Cyc_Absyn_MemKind;case 'B': _LLB8: return Cyc_Absyn_BoxKind;case
'R': _LLB9: return Cyc_Absyn_RgnKind;case 'U': _LLBA: if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))== 'R')return Cyc_Absyn_UniqueRgnKind;
else{break;}case 'T': _LLBB: if(*((const char*)_check_dyneither_subscript(s,sizeof(
char),1))== 'R')return Cyc_Absyn_TopRgnKind;else{break;}case 'E': _LLBC: return Cyc_Absyn_EffKind;
case 'I': _LLBD: return Cyc_Absyn_IntKind;default: _LLBE: break;}{const char*_tmp8EF;
void*_tmp8EE[2];struct Cyc_String_pa_struct _tmp8ED;struct Cyc_Int_pa_struct _tmp8EC;
Cyc_Parse_err((struct _dyneither_ptr)((_tmp8EC.tag=1,((_tmp8EC.f1=(unsigned long)((
int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp8ED.tag=0,((_tmp8ED.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp8EE[0]=& _tmp8ED,((_tmp8EE[1]=&
_tmp8EC,Cyc_aprintf(((_tmp8EF="bad kind: %s; strlen=%d",_tag_dyneither(_tmp8EF,
sizeof(char),24))),_tag_dyneither(_tmp8EE,sizeof(void*),2)))))))))))))),loc);}
return Cyc_Absyn_BoxKind;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*
p);static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*
_tmp1AB=p->r;struct _tuple1*_tmp1AD;struct Cyc_Absyn_Vardecl*_tmp1AF;struct Cyc_Absyn_Pat*
_tmp1B0;struct Cyc_Absyn_Pat _tmp1B1;void*_tmp1B2;struct Cyc_Absyn_Pat*_tmp1B5;
enum Cyc_Absyn_Sign _tmp1B8;int _tmp1B9;char _tmp1BB;struct _dyneither_ptr _tmp1BD;
struct _tuple1*_tmp1BF;struct Cyc_List_List*_tmp1C0;int _tmp1C1;struct Cyc_Absyn_Exp*
_tmp1C3;_LLC1: {struct Cyc_Absyn_UnknownId_p_struct*_tmp1AC=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp1AB;if(_tmp1AC->tag != 14)goto _LLC3;else{_tmp1AD=_tmp1AC->f1;}}_LLC2: {struct
Cyc_Absyn_UnknownId_e_struct _tmp8F2;struct Cyc_Absyn_UnknownId_e_struct*_tmp8F1;
return Cyc_Absyn_new_exp((void*)((_tmp8F1=_cycalloc(sizeof(*_tmp8F1)),((_tmp8F1[0]=((
_tmp8F2.tag=2,((_tmp8F2.f1=_tmp1AD,_tmp8F2)))),_tmp8F1)))),p->loc);}_LLC3: {
struct Cyc_Absyn_Reference_p_struct*_tmp1AE=(struct Cyc_Absyn_Reference_p_struct*)
_tmp1AB;if(_tmp1AE->tag != 2)goto _LLC5;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;
_tmp1B1=*_tmp1B0;_tmp1B2=_tmp1B1.r;{struct Cyc_Absyn_Wild_p_struct*_tmp1B3=(
struct Cyc_Absyn_Wild_p_struct*)_tmp1B2;if(_tmp1B3->tag != 0)goto _LLC5;}}}_LLC4: {
struct Cyc_Absyn_UnknownId_e_struct _tmp8F5;struct Cyc_Absyn_UnknownId_e_struct*
_tmp8F4;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmp8F4=_cycalloc(
sizeof(*_tmp8F4)),((_tmp8F4[0]=((_tmp8F5.tag=2,((_tmp8F5.f1=_tmp1AF->name,
_tmp8F5)))),_tmp8F4)))),p->loc),p->loc);}_LLC5: {struct Cyc_Absyn_Pointer_p_struct*
_tmp1B4=(struct Cyc_Absyn_Pointer_p_struct*)_tmp1AB;if(_tmp1B4->tag != 5)goto _LLC7;
else{_tmp1B5=_tmp1B4->f1;}}_LLC6: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp1B5),p->loc);_LLC7: {struct Cyc_Absyn_Null_p_struct*_tmp1B6=(struct Cyc_Absyn_Null_p_struct*)
_tmp1AB;if(_tmp1B6->tag != 8)goto _LLC9;}_LLC8: return Cyc_Absyn_null_exp(p->loc);
_LLC9: {struct Cyc_Absyn_Int_p_struct*_tmp1B7=(struct Cyc_Absyn_Int_p_struct*)
_tmp1AB;if(_tmp1B7->tag != 9)goto _LLCB;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B7->f2;}}
_LLCA: return Cyc_Absyn_int_exp(_tmp1B8,_tmp1B9,p->loc);_LLCB: {struct Cyc_Absyn_Char_p_struct*
_tmp1BA=(struct Cyc_Absyn_Char_p_struct*)_tmp1AB;if(_tmp1BA->tag != 10)goto _LLCD;
else{_tmp1BB=_tmp1BA->f1;}}_LLCC: return Cyc_Absyn_char_exp(_tmp1BB,p->loc);_LLCD: {
struct Cyc_Absyn_Float_p_struct*_tmp1BC=(struct Cyc_Absyn_Float_p_struct*)_tmp1AB;
if(_tmp1BC->tag != 11)goto _LLCF;else{_tmp1BD=_tmp1BC->f1;}}_LLCE: return Cyc_Absyn_float_exp(
_tmp1BD,p->loc);_LLCF: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp1BE=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1AB;if(_tmp1BE->tag != 15)goto _LLD1;else{_tmp1BF=_tmp1BE->f1;_tmp1C0=_tmp1BE->f2;
_tmp1C1=_tmp1BE->f3;if(_tmp1C1 != 0)goto _LLD1;}}_LLD0: {struct Cyc_Absyn_UnknownId_e_struct
_tmp8F8;struct Cyc_Absyn_UnknownId_e_struct*_tmp8F7;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmp8F7=_cycalloc(sizeof(*_tmp8F7)),((_tmp8F7[0]=((_tmp8F8.tag=2,((
_tmp8F8.f1=_tmp1BF,_tmp8F8)))),_tmp8F7)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1C0);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLD1: {struct Cyc_Absyn_Exp_p_struct*_tmp1C2=(struct Cyc_Absyn_Exp_p_struct*)
_tmp1AB;if(_tmp1C2->tag != 16)goto _LLD3;else{_tmp1C3=_tmp1C2->f1;}}_LLD2: return
_tmp1C3;_LLD3:;_LLD4:{const char*_tmp8F9;Cyc_Parse_err(((_tmp8F9="cannot mix patterns and expressions in case",
_tag_dyneither(_tmp8F9,sizeof(char),44))),p->loc);}return Cyc_Absyn_null_exp(p->loc);
_LLC0:;}union Cyc_YYSTYPE;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};
struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{
int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;
struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*
val;};struct _tuple18{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple18*
val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple19{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple19*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple20*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec*val;};
struct _tuple21{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Parse_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator*val;};
struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum 
Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*
val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple22{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple23{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple23*val;};
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
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1CC[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1CC,
_tmp1CC,_tmp1CC + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1);struct
_tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct _tuple6 yyzzz;{union Cyc_YYSTYPE
_tmp1CD=yy1;struct _tuple6 _tmp1CE;_LLD6: if((_tmp1CD.Int_tok).tag != 1)goto _LLD8;
_tmp1CE=(struct _tuple6)(_tmp1CD.Int_tok).val;_LLD7: yyzzz=_tmp1CE;goto _LLD5;_LLD8:;
_LLD9:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLD5:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_Int_tok(struct _tuple6 yy1);union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){
union Cyc_YYSTYPE _tmp8FA;return((_tmp8FA.Int_tok).val=yy1,(((_tmp8FA.Int_tok).tag=
1,_tmp8FA)));}static char _tmp1D1[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={
Cyc_Core_Failure,{_tmp1D1,_tmp1D1,_tmp1D1 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE
yy1);char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){char yyzzz;{union Cyc_YYSTYPE
_tmp1D2=yy1;char _tmp1D3;_LLDB: if((_tmp1D2.Char_tok).tag != 2)goto _LLDD;_tmp1D3=(
char)(_tmp1D2.Char_tok).val;_LLDC: yyzzz=_tmp1D3;goto _LLDA;_LLDD:;_LLDE:(int)
_throw((void*)& Cyc_yyfail_Char_tok);_LLDA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_Char_tok(
char yy1);union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp8FB;return((
_tmp8FB.Char_tok).val=yy1,(((_tmp8FB.Char_tok).tag=2,_tmp8FB)));}static char
_tmp1D6[13]="string_t<`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp1D6,_tmp1D6,_tmp1D6 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE yy1);struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp1D7=yy1;struct _dyneither_ptr
_tmp1D8;_LLE0: if((_tmp1D7.String_tok).tag != 3)goto _LLE2;_tmp1D8=(struct
_dyneither_ptr)(_tmp1D7.String_tok).val;_LLE1: yyzzz=_tmp1D8;goto _LLDF;_LLE2:;
_LLE3:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLDF:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1);union Cyc_YYSTYPE Cyc_String_tok(struct
_dyneither_ptr yy1){union Cyc_YYSTYPE _tmp8FC;return((_tmp8FC.String_tok).val=yy1,(((
_tmp8FC.String_tok).tag=3,_tmp8FC)));}static char _tmp1DB[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1DB,
_tmp1DB,_tmp1DB + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1);struct
_tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct _tuple18*yyzzz;{union Cyc_YYSTYPE
_tmp1DC=yy1;struct _tuple18*_tmp1DD;_LLE5: if((_tmp1DC.YY1).tag != 6)goto _LLE7;
_tmp1DD=(struct _tuple18*)(_tmp1DC.YY1).val;_LLE6: yyzzz=_tmp1DD;goto _LLE4;_LLE7:;
_LLE8:(int)_throw((void*)& Cyc_yyfail_YY1);_LLE4:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY1(struct _tuple18*yy1);union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1){union Cyc_YYSTYPE
_tmp8FD;return((_tmp8FD.YY1).val=yy1,(((_tmp8FD.YY1).tag=6,_tmp8FD)));}static
char _tmp1E0[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={
Cyc_Core_Failure,{_tmp1E0,_tmp1E0,_tmp1E0 + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(
union Cyc_YYSTYPE yy1);union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp1E1=yy1;union Cyc_Absyn_Constraint*
_tmp1E2;_LLEA: if((_tmp1E1.YY2).tag != 7)goto _LLEC;_tmp1E2=(union Cyc_Absyn_Constraint*)(
_tmp1E1.YY2).val;_LLEB: yyzzz=_tmp1E2;goto _LLE9;_LLEC:;_LLED:(int)_throw((void*)&
Cyc_yyfail_YY2);_LLE9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*
yy1);union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmp8FE;return((_tmp8FE.YY2).val=yy1,(((_tmp8FE.YY2).tag=7,_tmp8FE)));}static
char _tmp1E5[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{
_tmp1E5,_tmp1E5,_tmp1E5 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE
yy1);struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE _tmp1E6=yy1;struct Cyc_Absyn_Exp*_tmp1E7;_LLEF: if((_tmp1E6.YY3).tag
!= 8)goto _LLF1;_tmp1E7=(struct Cyc_Absyn_Exp*)(_tmp1E6.YY3).val;_LLF0: yyzzz=
_tmp1E7;goto _LLEE;_LLF1:;_LLF2:(int)_throw((void*)& Cyc_yyfail_YY3);_LLEE:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY3(struct Cyc_Absyn_Exp*yy1);union Cyc_YYSTYPE
Cyc_YY3(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp8FF;return((_tmp8FF.YY3).val=
yy1,(((_tmp8FF.YY3).tag=8,_tmp8FF)));}static char _tmp1EA[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1EA,_tmp1EA,
_tmp1EA + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Exp*
Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE
_tmp1EB=yy1;struct Cyc_Absyn_Exp*_tmp1EC;_LLF4: if((_tmp1EB.YY4).tag != 9)goto _LLF6;
_tmp1EC=(struct Cyc_Absyn_Exp*)(_tmp1EB.YY4).val;_LLF5: yyzzz=_tmp1EC;goto _LLF3;
_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_YY4);_LLF3:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY4(struct Cyc_Absyn_Exp*yy1);union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){
union Cyc_YYSTYPE _tmp900;return((_tmp900.YY4).val=yy1,(((_tmp900.YY4).tag=9,
_tmp900)));}static char _tmp1EF[17]="list_t<exp_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1EF,_tmp1EF,_tmp1EF + 17}};struct Cyc_List_List*
Cyc_yyget_YY5(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp1F0=yy1;struct Cyc_List_List*
_tmp1F1;_LLF9: if((_tmp1F0.YY5).tag != 10)goto _LLFB;_tmp1F1=(struct Cyc_List_List*)(
_tmp1F0.YY5).val;_LLFA: yyzzz=_tmp1F1;goto _LLF8;_LLFB:;_LLFC:(int)_throw((void*)&
Cyc_yyfail_YY5);_LLF8:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp901;
return((_tmp901.YY5).val=yy1,(((_tmp901.YY5).tag=10,_tmp901)));}static char
_tmp1F4[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1F4,_tmp1F4,_tmp1F4 + 47}};struct Cyc_List_List*
Cyc_yyget_YY6(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp1F5=yy1;struct Cyc_List_List*
_tmp1F6;_LLFE: if((_tmp1F5.YY6).tag != 11)goto _LL100;_tmp1F6=(struct Cyc_List_List*)(
_tmp1F5.YY6).val;_LLFF: yyzzz=_tmp1F6;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)&
Cyc_yyfail_YY6);_LLFD:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp902;
return((_tmp902.YY6).val=yy1,(((_tmp902.YY6).tag=11,_tmp902)));}static char
_tmp1F9[9]="primop_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{
_tmp1F9,_tmp1F9,_tmp1F9 + 9}};enum Cyc_Absyn_Primop Cyc_yyget_YY7(union Cyc_YYSTYPE
yy1);enum Cyc_Absyn_Primop Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop
yyzzz;{union Cyc_YYSTYPE _tmp1FA=yy1;enum Cyc_Absyn_Primop _tmp1FB;_LL103: if((
_tmp1FA.YY7).tag != 12)goto _LL105;_tmp1FB=(enum Cyc_Absyn_Primop)(_tmp1FA.YY7).val;
_LL104: yyzzz=_tmp1FB;goto _LL102;_LL105:;_LL106:(int)_throw((void*)& Cyc_yyfail_YY7);
_LL102:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1);union Cyc_YYSTYPE
Cyc_YY7(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp903;return((_tmp903.YY7).val=
yy1,(((_tmp903.YY7).tag=12,_tmp903)));}static char _tmp1FE[19]="opt_t<primop_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1FE,
_tmp1FE,_tmp1FE + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1);
struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{
union Cyc_YYSTYPE _tmp1FF=yy1;struct Cyc_Core_Opt*_tmp200;_LL108: if((_tmp1FF.YY8).tag
!= 13)goto _LL10A;_tmp200=(struct Cyc_Core_Opt*)(_tmp1FF.YY8).val;_LL109: yyzzz=
_tmp200;goto _LL107;_LL10A:;_LL10B:(int)_throw((void*)& Cyc_yyfail_YY8);_LL107:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*yy1);union Cyc_YYSTYPE Cyc_YY8(
struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp904;return((_tmp904.YY8).val=yy1,(((
_tmp904.YY8).tag=13,_tmp904)));}static char _tmp203[7]="qvar_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp203,_tmp203,_tmp203 + 7}};struct
_tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1);struct _tuple1*Cyc_yyget_QualId_tok(
union Cyc_YYSTYPE yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp204=yy1;struct
_tuple1*_tmp205;_LL10D: if((_tmp204.QualId_tok).tag != 5)goto _LL10F;_tmp205=(
struct _tuple1*)(_tmp204.QualId_tok).val;_LL10E: yyzzz=_tmp205;goto _LL10C;_LL10F:;
_LL110:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL10C:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_QualId_tok(struct _tuple1*yy1);union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple1*
yy1){union Cyc_YYSTYPE _tmp905;return((_tmp905.QualId_tok).val=yy1,(((_tmp905.QualId_tok).tag=
5,_tmp905)));}static char _tmp208[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp208,_tmp208,_tmp208 + 7}};struct Cyc_Absyn_Stmt*
Cyc_yyget_YY9(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE _tmp209=yy1;struct Cyc_Absyn_Stmt*
_tmp20A;_LL112: if((_tmp209.YY9).tag != 14)goto _LL114;_tmp20A=(struct Cyc_Absyn_Stmt*)(
_tmp209.YY9).val;_LL113: yyzzz=_tmp20A;goto _LL111;_LL114:;_LL115:(int)_throw((
void*)& Cyc_yyfail_YY9);_LL111:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*
yy1);union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp906;
return((_tmp906.YY9).val=yy1,(((_tmp906.YY9).tag=14,_tmp906)));}static char
_tmp20D[27]="list_t<switch_clause_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={
Cyc_Core_Failure,{_tmp20D,_tmp20D,_tmp20D + 27}};struct Cyc_List_List*Cyc_yyget_YY10(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp20E=yy1;struct Cyc_List_List*
_tmp20F;_LL117: if((_tmp20E.YY10).tag != 15)goto _LL119;_tmp20F=(struct Cyc_List_List*)(
_tmp20E.YY10).val;_LL118: yyzzz=_tmp20F;goto _LL116;_LL119:;_LL11A:(int)_throw((
void*)& Cyc_yyfail_YY10);_LL116:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp907;
return((_tmp907.YY10).val=yy1,(((_tmp907.YY10).tag=15,_tmp907)));}static char
_tmp212[6]="pat_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{
_tmp212,_tmp212,_tmp212 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE
yy1);struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Pat*
yyzzz;{union Cyc_YYSTYPE _tmp213=yy1;struct Cyc_Absyn_Pat*_tmp214;_LL11C: if((
_tmp213.YY11).tag != 16)goto _LL11E;_tmp214=(struct Cyc_Absyn_Pat*)(_tmp213.YY11).val;
_LL11D: yyzzz=_tmp214;goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY11);
_LL11B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*yy1);union Cyc_YYSTYPE
Cyc_YY11(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp908;return((_tmp908.YY11).val=
yy1,(((_tmp908.YY11).tag=16,_tmp908)));}static char _tmp217[28]="$(list_t<pat_t,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp217,
_tmp217,_tmp217 + 28}};struct _tuple19*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1);struct
_tuple19*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE
_tmp218=yy1;struct _tuple19*_tmp219;_LL121: if((_tmp218.YY12).tag != 17)goto _LL123;
_tmp219=(struct _tuple19*)(_tmp218.YY12).val;_LL122: yyzzz=_tmp219;goto _LL120;
_LL123:;_LL124:(int)_throw((void*)& Cyc_yyfail_YY12);_LL120:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY12(struct _tuple19*yy1);union Cyc_YYSTYPE Cyc_YY12(struct _tuple19*
yy1){union Cyc_YYSTYPE _tmp909;return((_tmp909.YY12).val=yy1,(((_tmp909.YY12).tag=
17,_tmp909)));}static char _tmp21C[17]="list_t<pat_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp21C,_tmp21C,_tmp21C + 17}};struct Cyc_List_List*
Cyc_yyget_YY13(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp21D=yy1;struct Cyc_List_List*
_tmp21E;_LL126: if((_tmp21D.YY13).tag != 18)goto _LL128;_tmp21E=(struct Cyc_List_List*)(
_tmp21D.YY13).val;_LL127: yyzzz=_tmp21E;goto _LL125;_LL128:;_LL129:(int)_throw((
void*)& Cyc_yyfail_YY13);_LL125:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp90A;
return((_tmp90A.YY13).val=yy1,(((_tmp90A.YY13).tag=18,_tmp90A)));}static char
_tmp221[36]="$(list_t<designator_t,`H>,pat_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp221,_tmp221,_tmp221 + 36}};struct _tuple20*
Cyc_yyget_YY14(union Cyc_YYSTYPE yy1);struct _tuple20*Cyc_yyget_YY14(union Cyc_YYSTYPE
yy1){struct _tuple20*yyzzz;{union Cyc_YYSTYPE _tmp222=yy1;struct _tuple20*_tmp223;
_LL12B: if((_tmp222.YY14).tag != 19)goto _LL12D;_tmp223=(struct _tuple20*)(_tmp222.YY14).val;
_LL12C: yyzzz=_tmp223;goto _LL12A;_LL12D:;_LL12E:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL12A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1);union Cyc_YYSTYPE
Cyc_YY14(struct _tuple20*yy1){union Cyc_YYSTYPE _tmp90B;return((_tmp90B.YY14).val=
yy1,(((_tmp90B.YY14).tag=19,_tmp90B)));}static char _tmp226[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp226,
_tmp226,_tmp226 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp227=yy1;struct Cyc_List_List*_tmp228;_LL130: if((
_tmp227.YY15).tag != 20)goto _LL132;_tmp228=(struct Cyc_List_List*)(_tmp227.YY15).val;
_LL131: yyzzz=_tmp228;goto _LL12F;_LL132:;_LL133:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL12F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY15(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp90C;return((_tmp90C.YY15).val=
yy1,(((_tmp90C.YY15).tag=20,_tmp90C)));}static char _tmp22B[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp22B,
_tmp22B,_tmp22B + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1);struct
_tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct _tuple19*yyzzz;{union Cyc_YYSTYPE
_tmp22C=yy1;struct _tuple19*_tmp22D;_LL135: if((_tmp22C.YY16).tag != 21)goto _LL137;
_tmp22D=(struct _tuple19*)(_tmp22C.YY16).val;_LL136: yyzzz=_tmp22D;goto _LL134;
_LL137:;_LL138:(int)_throw((void*)& Cyc_yyfail_YY16);_LL134:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1);union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*
yy1){union Cyc_YYSTYPE _tmp90D;return((_tmp90D.YY16).val=yy1,(((_tmp90D.YY16).tag=
21,_tmp90D)));}static char _tmp230[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp230,_tmp230,_tmp230 + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY17(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(union
Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp231=yy1;struct
Cyc_Absyn_Fndecl*_tmp232;_LL13A: if((_tmp231.YY17).tag != 22)goto _LL13C;_tmp232=(
struct Cyc_Absyn_Fndecl*)(_tmp231.YY17).val;_LL13B: yyzzz=_tmp232;goto _LL139;
_LL13C:;_LL13D:(int)_throw((void*)& Cyc_yyfail_YY17);_LL139:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1);union Cyc_YYSTYPE Cyc_YY17(struct
Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp90E;return((_tmp90E.YY17).val=yy1,(((
_tmp90E.YY17).tag=22,_tmp90E)));}static char _tmp235[18]="list_t<decl_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp235,
_tmp235,_tmp235 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp236=yy1;struct Cyc_List_List*_tmp237;_LL13F: if((
_tmp236.YY18).tag != 23)goto _LL141;_tmp237=(struct Cyc_List_List*)(_tmp236.YY18).val;
_LL140: yyzzz=_tmp237;goto _LL13E;_LL141:;_LL142:(int)_throw((void*)& Cyc_yyfail_YY18);
_LL13E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY18(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp90F;return((_tmp90F.YY18).val=
yy1,(((_tmp90F.YY18).tag=23,_tmp90F)));}static char _tmp23A[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp23A,
_tmp23A,_tmp23A + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1);struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){struct
Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp23B=yy1;struct Cyc_Parse_Declaration_spec*
_tmp23C;_LL144: if((_tmp23B.YY19).tag != 24)goto _LL146;_tmp23C=(struct Cyc_Parse_Declaration_spec*)(
_tmp23B.YY19).val;_LL145: yyzzz=_tmp23C;goto _LL143;_LL146:;_LL147:(int)_throw((
void*)& Cyc_yyfail_YY19);_LL143:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*
yy1);union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){union Cyc_YYSTYPE
_tmp910;return((_tmp910.YY19).val=yy1,(((_tmp910.YY19).tag=24,_tmp910)));}static
char _tmp23F[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp23F,_tmp23F,_tmp23F + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1);struct _tuple21*Cyc_yyget_YY20(union Cyc_YYSTYPE
yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE _tmp240=yy1;struct _tuple21*_tmp241;
_LL149: if((_tmp240.YY20).tag != 25)goto _LL14B;_tmp241=(struct _tuple21*)(_tmp240.YY20).val;
_LL14A: yyzzz=_tmp241;goto _LL148;_LL14B:;_LL14C:(int)_throw((void*)& Cyc_yyfail_YY20);
_LL148:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1);union Cyc_YYSTYPE
Cyc_YY20(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp911;return((_tmp911.YY20).val=
yy1,(((_tmp911.YY20).tag=25,_tmp911)));}static char _tmp244[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp244,
_tmp244,_tmp244 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp245=yy1;struct Cyc_List_List*_tmp246;_LL14E: if((
_tmp245.YY21).tag != 26)goto _LL150;_tmp246=(struct Cyc_List_List*)(_tmp245.YY21).val;
_LL14F: yyzzz=_tmp246;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY21);
_LL14D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY21(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp912;return((_tmp912.YY21).val=
yy1,(((_tmp912.YY21).tag=26,_tmp912)));}static char _tmp249[16]="storage_class_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp249,
_tmp249,_tmp249 + 16}};enum Cyc_Parse_Storage_class Cyc_yyget_YY22(union Cyc_YYSTYPE
yy1);enum Cyc_Parse_Storage_class Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class
yyzzz;{union Cyc_YYSTYPE _tmp24A=yy1;enum Cyc_Parse_Storage_class _tmp24B;_LL153:
if((_tmp24A.YY22).tag != 27)goto _LL155;_tmp24B=(enum Cyc_Parse_Storage_class)(
_tmp24A.YY22).val;_LL154: yyzzz=_tmp24B;goto _LL152;_LL155:;_LL156:(int)_throw((
void*)& Cyc_yyfail_YY22);_LL152:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class
yy1);union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){union Cyc_YYSTYPE
_tmp913;return((_tmp913.YY22).val=yy1,(((_tmp913.YY22).tag=27,_tmp913)));}static
char _tmp24E[17]="type_specifier_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY23={
Cyc_Core_Failure,{_tmp24E,_tmp24E,_tmp24E + 17}};void*Cyc_yyget_YY23(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp24F=yy1;void*_tmp250;_LL158: if((_tmp24F.YY23).tag != 28)goto _LL15A;_tmp250=(
void*)(_tmp24F.YY23).val;_LL159: yyzzz=_tmp250;goto _LL157;_LL15A:;_LL15B:(int)
_throw((void*)& Cyc_yyfail_YY23);_LL157:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(
void*yy1);union Cyc_YYSTYPE Cyc_YY23(void*yy1){union Cyc_YYSTYPE _tmp914;return((
_tmp914.YY23).val=yy1,(((_tmp914.YY23).tag=28,_tmp914)));}static char _tmp253[12]="aggr_kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp253,
_tmp253,_tmp253 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(union Cyc_YYSTYPE yy1);
enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind
yyzzz;{union Cyc_YYSTYPE _tmp254=yy1;enum Cyc_Absyn_AggrKind _tmp255;_LL15D: if((
_tmp254.YY24).tag != 29)goto _LL15F;_tmp255=(enum Cyc_Absyn_AggrKind)(_tmp254.YY24).val;
_LL15E: yyzzz=_tmp255;goto _LL15C;_LL15F:;_LL160:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL15C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1);union
Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp915;return((
_tmp915.YY24).val=yy1,(((_tmp915.YY24).tag=29,_tmp915)));}static char _tmp258[8]="tqual_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp258,
_tmp258,_tmp258 + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE yy1);
struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Tqual
yyzzz;{union Cyc_YYSTYPE _tmp259=yy1;struct Cyc_Absyn_Tqual _tmp25A;_LL162: if((
_tmp259.YY25).tag != 30)goto _LL164;_tmp25A=(struct Cyc_Absyn_Tqual)(_tmp259.YY25).val;
_LL163: yyzzz=_tmp25A;goto _LL161;_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY25);
_LL161:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1);union
Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp916;return((
_tmp916.YY25).val=yy1,(((_tmp916.YY25).tag=30,_tmp916)));}static char _tmp25D[23]="list_t<aggrfield_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp25D,
_tmp25D,_tmp25D + 23}};struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp25E=yy1;struct Cyc_List_List*_tmp25F;_LL167: if((
_tmp25E.YY26).tag != 31)goto _LL169;_tmp25F=(struct Cyc_List_List*)(_tmp25E.YY26).val;
_LL168: yyzzz=_tmp25F;goto _LL166;_LL169:;_LL16A:(int)_throw((void*)& Cyc_yyfail_YY26);
_LL166:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp917;return((_tmp917.YY26).val=
yy1,(((_tmp917.YY26).tag=31,_tmp917)));}static char _tmp262[34]="list_t<list_t<aggrfield_t,`H>,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp262,
_tmp262,_tmp262 + 34}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp263=yy1;struct Cyc_List_List*_tmp264;_LL16C: if((
_tmp263.YY27).tag != 32)goto _LL16E;_tmp264=(struct Cyc_List_List*)(_tmp263.YY27).val;
_LL16D: yyzzz=_tmp264;goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY27);
_LL16B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp918;return((_tmp918.YY27).val=
yy1,(((_tmp918.YY27).tag=32,_tmp918)));}static char _tmp267[31]="list_t<type_modifier_t<`H>,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp267,
_tmp267,_tmp267 + 31}};struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp268=yy1;struct Cyc_List_List*_tmp269;_LL171: if((
_tmp268.YY28).tag != 33)goto _LL173;_tmp269=(struct Cyc_List_List*)(_tmp268.YY28).val;
_LL172: yyzzz=_tmp269;goto _LL170;_LL173:;_LL174:(int)_throw((void*)& Cyc_yyfail_YY28);
_LL170:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp919;return((_tmp919.YY28).val=
yy1,(((_tmp919.YY28).tag=33,_tmp919)));}static char _tmp26C[13]="declarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp26C,
_tmp26C,_tmp26C + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE
yy1);struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*
yyzzz;{union Cyc_YYSTYPE _tmp26D=yy1;struct Cyc_Parse_Declarator*_tmp26E;_LL176: if((
_tmp26D.YY29).tag != 34)goto _LL178;_tmp26E=(struct Cyc_Parse_Declarator*)(_tmp26D.YY29).val;
_LL177: yyzzz=_tmp26E;goto _LL175;_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY29);
_LL175:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1);
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){union Cyc_YYSTYPE _tmp91A;
return((_tmp91A.YY29).val=yy1,(((_tmp91A.YY29).tag=34,_tmp91A)));}static char
_tmp271[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp271,_tmp271,_tmp271 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1);struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE
_tmp272=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp273;_LL17B: if((_tmp272.YY30).tag
!= 35)goto _LL17D;_tmp273=(struct Cyc_Parse_Abstractdeclarator*)(_tmp272.YY30).val;
_LL17C: yyzzz=_tmp273;goto _LL17A;_LL17D:;_LL17E:(int)_throw((void*)& Cyc_yyfail_YY30);
_LL17A:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1);union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*yy1){union Cyc_YYSTYPE
_tmp91B;return((_tmp91B.YY30).val=yy1,(((_tmp91B.YY30).tag=35,_tmp91B)));}static
char _tmp276[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp276,_tmp276,_tmp276 + 5}};int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1);int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp277=yy1;int _tmp278;_LL180: if((
_tmp277.YY31).tag != 36)goto _LL182;_tmp278=(int)(_tmp277.YY31).val;_LL181: yyzzz=
_tmp278;goto _LL17F;_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY31);_LL17F:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1);union Cyc_YYSTYPE Cyc_YY31(int yy1){
union Cyc_YYSTYPE _tmp91C;return((_tmp91C.YY31).val=yy1,(((_tmp91C.YY31).tag=36,
_tmp91C)));}static char _tmp27B[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp27B,_tmp27B,_tmp27B + 8}};enum Cyc_Absyn_Scope
Cyc_yyget_YY32(union Cyc_YYSTYPE yy1);enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE
yy1){enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp27C=yy1;enum Cyc_Absyn_Scope
_tmp27D;_LL185: if((_tmp27C.YY32).tag != 37)goto _LL187;_tmp27D=(enum Cyc_Absyn_Scope)(
_tmp27C.YY32).val;_LL186: yyzzz=_tmp27D;goto _LL184;_LL187:;_LL188:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL184:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1);union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp91D;
return((_tmp91D.YY32).val=yy1,(((_tmp91D.YY32).tag=37,_tmp91D)));}static char
_tmp280[16]="datatypefield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp280,_tmp280,_tmp280 + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1);struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union Cyc_YYSTYPE
_tmp281=yy1;struct Cyc_Absyn_Datatypefield*_tmp282;_LL18A: if((_tmp281.YY33).tag != 
38)goto _LL18C;_tmp282=(struct Cyc_Absyn_Datatypefield*)(_tmp281.YY33).val;_LL18B:
yyzzz=_tmp282;goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL189:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1);
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE
_tmp91E;return((_tmp91E.YY33).val=yy1,(((_tmp91E.YY33).tag=38,_tmp91E)));}static
char _tmp285[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp285,_tmp285,_tmp285 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp286=yy1;struct Cyc_List_List*
_tmp287;_LL18F: if((_tmp286.YY34).tag != 39)goto _LL191;_tmp287=(struct Cyc_List_List*)(
_tmp286.YY34).val;_LL190: yyzzz=_tmp287;goto _LL18E;_LL191:;_LL192:(int)_throw((
void*)& Cyc_yyfail_YY34);_LL18E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp91F;
return((_tmp91F.YY34).val=yy1,(((_tmp91F.YY34).tag=39,_tmp91F)));}static char
_tmp28A[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp28A,_tmp28A,
_tmp28A + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1);struct _tuple22*
Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*yyzzz;{union Cyc_YYSTYPE
_tmp28B=yy1;struct _tuple22*_tmp28C;_LL194: if((_tmp28B.YY35).tag != 40)goto _LL196;
_tmp28C=(struct _tuple22*)(_tmp28B.YY35).val;_LL195: yyzzz=_tmp28C;goto _LL193;
_LL196:;_LL197:(int)_throw((void*)& Cyc_yyfail_YY35);_LL193:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1);union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*
yy1){union Cyc_YYSTYPE _tmp920;return((_tmp920.YY35).val=yy1,(((_tmp920.YY35).tag=
40,_tmp920)));}static char _tmp28F[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp28F,_tmp28F,_tmp28F + 17}};struct Cyc_List_List*
Cyc_yyget_YY36(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp290=yy1;struct Cyc_List_List*
_tmp291;_LL199: if((_tmp290.YY36).tag != 41)goto _LL19B;_tmp291=(struct Cyc_List_List*)(
_tmp290.YY36).val;_LL19A: yyzzz=_tmp291;goto _LL198;_LL19B:;_LL19C:(int)_throw((
void*)& Cyc_yyfail_YY36);_LL198:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp921;
return((_tmp921.YY36).val=yy1,(((_tmp921.YY36).tag=41,_tmp921)));}static char
_tmp294[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp294,_tmp294,_tmp294 + 37}};struct _tuple8*
Cyc_yyget_YY37(union Cyc_YYSTYPE yy1);struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE
yy1){struct _tuple8*yyzzz;{union Cyc_YYSTYPE _tmp295=yy1;struct _tuple8*_tmp296;
_LL19E: if((_tmp295.YY37).tag != 42)goto _LL1A0;_tmp296=(struct _tuple8*)(_tmp295.YY37).val;
_LL19F: yyzzz=_tmp296;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL19D:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1);union Cyc_YYSTYPE
Cyc_YY37(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp922;return((_tmp922.YY37).val=
yy1,(((_tmp922.YY37).tag=42,_tmp922)));}static char _tmp299[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp299,
_tmp299,_tmp299 + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp29A=yy1;struct Cyc_List_List*_tmp29B;_LL1A3: if((
_tmp29A.YY38).tag != 43)goto _LL1A5;_tmp29B=(struct Cyc_List_List*)(_tmp29A.YY38).val;
_LL1A4: yyzzz=_tmp29B;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1A2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY38(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp923;return((_tmp923.YY38).val=
yy1,(((_tmp923.YY38).tag=43,_tmp923)));}static char _tmp29E[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp29E,
_tmp29E,_tmp29E + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1);struct
_tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct _tuple23*yyzzz;{union Cyc_YYSTYPE
_tmp29F=yy1;struct _tuple23*_tmp2A0;_LL1A8: if((_tmp29F.YY39).tag != 44)goto _LL1AA;
_tmp2A0=(struct _tuple23*)(_tmp29F.YY39).val;_LL1A9: yyzzz=_tmp2A0;goto _LL1A7;
_LL1AA:;_LL1AB:(int)_throw((void*)& Cyc_yyfail_YY39);_LL1A7:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1);union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*
yy1){union Cyc_YYSTYPE _tmp924;return((_tmp924.YY39).val=yy1,(((_tmp924.YY39).tag=
44,_tmp924)));}static char _tmp2A3[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2A3,_tmp2A3,_tmp2A3 + 18}};struct Cyc_List_List*
Cyc_yyget_YY40(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A4=yy1;struct Cyc_List_List*
_tmp2A5;_LL1AD: if((_tmp2A4.YY40).tag != 45)goto _LL1AF;_tmp2A5=(struct Cyc_List_List*)(
_tmp2A4.YY40).val;_LL1AE: yyzzz=_tmp2A5;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((
void*)& Cyc_yyfail_YY40);_LL1AC:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp925;
return((_tmp925.YY40).val=yy1,(((_tmp925.YY40).tag=45,_tmp925)));}static char
_tmp2A8[24]="list_t<designator_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={
Cyc_Core_Failure,{_tmp2A8,_tmp2A8,_tmp2A8 + 24}};struct Cyc_List_List*Cyc_yyget_YY41(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A9=yy1;struct Cyc_List_List*
_tmp2AA;_LL1B2: if((_tmp2A9.YY41).tag != 46)goto _LL1B4;_tmp2AA=(struct Cyc_List_List*)(
_tmp2A9.YY41).val;_LL1B3: yyzzz=_tmp2AA;goto _LL1B1;_LL1B4:;_LL1B5:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1B1:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp926;
return((_tmp926.YY41).val=yy1,(((_tmp926.YY41).tag=46,_tmp926)));}static char
_tmp2AD[13]="designator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY42={
Cyc_Core_Failure,{_tmp2AD,_tmp2AD,_tmp2AD + 13}};void*Cyc_yyget_YY42(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp2AE=yy1;void*_tmp2AF;_LL1B7: if((_tmp2AE.YY42).tag != 47)goto _LL1B9;_tmp2AF=(
void*)(_tmp2AE.YY42).val;_LL1B8: yyzzz=_tmp2AF;goto _LL1B6;_LL1B9:;_LL1BA:(int)
_throw((void*)& Cyc_yyfail_YY42);_LL1B6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(
void*yy1);union Cyc_YYSTYPE Cyc_YY42(void*yy1){union Cyc_YYSTYPE _tmp927;return((
_tmp927.YY42).val=yy1,(((_tmp927.YY42).tag=47,_tmp927)));}static char _tmp2B2[7]="kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2B2,
_tmp2B2,_tmp2B2 + 7}};enum Cyc_Absyn_Kind Cyc_yyget_YY43(union Cyc_YYSTYPE yy1);
enum Cyc_Absyn_Kind Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Kind yyzzz;{
union Cyc_YYSTYPE _tmp2B3=yy1;enum Cyc_Absyn_Kind _tmp2B4;_LL1BC: if((_tmp2B3.YY43).tag
!= 48)goto _LL1BE;_tmp2B4=(enum Cyc_Absyn_Kind)(_tmp2B3.YY43).val;_LL1BD: yyzzz=
_tmp2B4;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY43);_LL1BB:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(enum Cyc_Absyn_Kind yy1);union Cyc_YYSTYPE
Cyc_YY43(enum Cyc_Absyn_Kind yy1){union Cyc_YYSTYPE _tmp928;return((_tmp928.YY43).val=
yy1,(((_tmp928.YY43).tag=48,_tmp928)));}static char _tmp2B7[7]="type_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2B7,_tmp2B7,
_tmp2B7 + 7}};void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1);void*Cyc_yyget_YY44(union
Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2B8=yy1;void*_tmp2B9;_LL1C1: if((
_tmp2B8.YY44).tag != 49)goto _LL1C3;_tmp2B9=(void*)(_tmp2B8.YY44).val;_LL1C2: yyzzz=
_tmp2B9;goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY44);_LL1C0:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1);union Cyc_YYSTYPE Cyc_YY44(void*
yy1){union Cyc_YYSTYPE _tmp929;return((_tmp929.YY44).val=yy1,(((_tmp929.YY44).tag=
49,_tmp929)));}static char _tmp2BC[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2BC,_tmp2BC,_tmp2BC + 23}};struct Cyc_List_List*
Cyc_yyget_YY45(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2BD=yy1;struct Cyc_List_List*
_tmp2BE;_LL1C6: if((_tmp2BD.YY45).tag != 50)goto _LL1C8;_tmp2BE=(struct Cyc_List_List*)(
_tmp2BD.YY45).val;_LL1C7: yyzzz=_tmp2BE;goto _LL1C5;_LL1C8:;_LL1C9:(int)_throw((
void*)& Cyc_yyfail_YY45);_LL1C5:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp92A;
return((_tmp92A.YY45).val=yy1,(((_tmp92A.YY45).tag=50,_tmp92A)));}static char
_tmp2C1[12]="attribute_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={
Cyc_Core_Failure,{_tmp2C1,_tmp2C1,_tmp2C1 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp2C2=yy1;void*_tmp2C3;_LL1CB: if((_tmp2C2.YY46).tag != 51)goto _LL1CD;_tmp2C3=(
void*)(_tmp2C2.YY46).val;_LL1CC: yyzzz=_tmp2C3;goto _LL1CA;_LL1CD:;_LL1CE:(int)
_throw((void*)& Cyc_yyfail_YY46);_LL1CA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1);union Cyc_YYSTYPE Cyc_YY46(void*yy1){union Cyc_YYSTYPE _tmp92B;return((
_tmp92B.YY46).val=yy1,(((_tmp92B.YY46).tag=51,_tmp92B)));}static char _tmp2C6[12]="enumfield_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2C6,
_tmp2C6,_tmp2C6 + 12}};struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE
yy1);struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*
yyzzz;{union Cyc_YYSTYPE _tmp2C7=yy1;struct Cyc_Absyn_Enumfield*_tmp2C8;_LL1D0: if((
_tmp2C7.YY47).tag != 52)goto _LL1D2;_tmp2C8=(struct Cyc_Absyn_Enumfield*)(_tmp2C7.YY47).val;
_LL1D1: yyzzz=_tmp2C8;goto _LL1CF;_LL1D2:;_LL1D3:(int)_throw((void*)& Cyc_yyfail_YY47);
_LL1CF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1);
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp92C;
return((_tmp92C.YY47).val=yy1,(((_tmp92C.YY47).tag=52,_tmp92C)));}static char
_tmp2CB[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp2CB,_tmp2CB,_tmp2CB + 23}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2CC=yy1;struct Cyc_List_List*
_tmp2CD;_LL1D5: if((_tmp2CC.YY48).tag != 53)goto _LL1D7;_tmp2CD=(struct Cyc_List_List*)(
_tmp2CC.YY48).val;_LL1D6: yyzzz=_tmp2CD;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((
void*)& Cyc_yyfail_YY48);_LL1D4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp92D;
return((_tmp92D.YY48).val=yy1,(((_tmp92D.YY48).tag=53,_tmp92D)));}static char
_tmp2D0[17]="opt_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={
Cyc_Core_Failure,{_tmp2D0,_tmp2D0,_tmp2D0 + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(
union Cyc_YYSTYPE yy1);struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp2D1=yy1;struct Cyc_Core_Opt*_tmp2D2;
_LL1DA: if((_tmp2D1.YY49).tag != 54)goto _LL1DC;_tmp2D2=(struct Cyc_Core_Opt*)(
_tmp2D1.YY49).val;_LL1DB: yyzzz=_tmp2D2;goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((
void*)& Cyc_yyfail_YY49);_LL1D9:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*
yy1);union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp92E;
return((_tmp92E.YY49).val=yy1,(((_tmp92E.YY49).tag=54,_tmp92E)));}static char
_tmp2D5[31]="list_t<$(type_t,type_t)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2D5,_tmp2D5,_tmp2D5 + 31}};struct Cyc_List_List*
Cyc_yyget_YY50(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2D6=yy1;struct Cyc_List_List*
_tmp2D7;_LL1DF: if((_tmp2D6.YY50).tag != 55)goto _LL1E1;_tmp2D7=(struct Cyc_List_List*)(
_tmp2D6.YY50).val;_LL1E0: yyzzz=_tmp2D7;goto _LL1DE;_LL1E1:;_LL1E2:(int)_throw((
void*)& Cyc_yyfail_YY50);_LL1DE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp92F;
return((_tmp92F.YY50).val=yy1,(((_tmp92F.YY50).tag=55,_tmp92F)));}static char
_tmp2DA[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={
Cyc_Core_Failure,{_tmp2DA,_tmp2DA,_tmp2DA + 15}};union Cyc_Absyn_Constraint*Cyc_yyget_YY51(
union Cyc_YYSTYPE yy1);union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE
yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp2DB=yy1;union Cyc_Absyn_Constraint*
_tmp2DC;_LL1E4: if((_tmp2DB.YY51).tag != 56)goto _LL1E6;_tmp2DC=(union Cyc_Absyn_Constraint*)(
_tmp2DB.YY51).val;_LL1E5: yyzzz=_tmp2DC;goto _LL1E3;_LL1E6:;_LL1E7:(int)_throw((
void*)& Cyc_yyfail_YY51);_LL1E3:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*
yy1);union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE
_tmp930;return((_tmp930.YY51).val=yy1,(((_tmp930.YY51).tag=56,_tmp930)));}static
char _tmp2DF[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2DF,_tmp2DF,_tmp2DF + 45}};struct Cyc_List_List*
Cyc_yyget_YY52(union Cyc_YYSTYPE yy1);struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2E0=yy1;struct Cyc_List_List*
_tmp2E1;_LL1E9: if((_tmp2E0.YY52).tag != 57)goto _LL1EB;_tmp2E1=(struct Cyc_List_List*)(
_tmp2E0.YY52).val;_LL1EA: yyzzz=_tmp2E1;goto _LL1E8;_LL1EB:;_LL1EC:(int)_throw((
void*)& Cyc_yyfail_YY52);_LL1E8:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*
yy1);union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp931;
return((_tmp931.YY52).val=yy1,(((_tmp931.YY52).tag=57,_tmp931)));}static char
_tmp2E4[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp2E4,_tmp2E4,_tmp2E4 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1);void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp2E5=yy1;void*_tmp2E6;_LL1EE: if((_tmp2E5.YY53).tag != 58)goto _LL1F0;_tmp2E6=(
void*)(_tmp2E5.YY53).val;_LL1EF: yyzzz=_tmp2E6;goto _LL1ED;_LL1F0:;_LL1F1:(int)
_throw((void*)& Cyc_yyfail_YY53);_LL1ED:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY53(
void*yy1);union Cyc_YYSTYPE Cyc_YY53(void*yy1){union Cyc_YYSTYPE _tmp932;return((
_tmp932.YY53).val=yy1,(((_tmp932.YY53).tag=58,_tmp932)));}static char _tmp2E9[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp2E9,
_tmp2E9,_tmp2E9 + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1);
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp2EA=yy1;struct Cyc_List_List*_tmp2EB;_LL1F3: if((
_tmp2EA.YY54).tag != 59)goto _LL1F5;_tmp2EB=(struct Cyc_List_List*)(_tmp2EA.YY54).val;
_LL1F4: yyzzz=_tmp2EB;goto _LL1F2;_LL1F5:;_LL1F6:(int)_throw((void*)& Cyc_yyfail_YY54);
_LL1F2:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1);union Cyc_YYSTYPE
Cyc_YY54(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp933;return((_tmp933.YY54).val=
yy1,(((_tmp933.YY54).tag=59,_tmp933)));}struct Cyc_Yyltype{int timestamp;int
first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc();
struct Cyc_Yyltype Cyc_yynewloc(){struct Cyc_Yyltype _tmp934;return(_tmp934.timestamp=
0,((_tmp934.first_line=0,((_tmp934.first_column=0,((_tmp934.last_line=0,((
_tmp934.last_column=0,_tmp934)))))))));}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
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
char _tmp2EE[2]="$";static char _tmp2EF[6]="error";static char _tmp2F0[12]="$undefined.";
static char _tmp2F1[5]="AUTO";static char _tmp2F2[9]="REGISTER";static char _tmp2F3[7]="STATIC";
static char _tmp2F4[7]="EXTERN";static char _tmp2F5[8]="TYPEDEF";static char _tmp2F6[5]="VOID";
static char _tmp2F7[5]="CHAR";static char _tmp2F8[6]="SHORT";static char _tmp2F9[4]="INT";
static char _tmp2FA[5]="LONG";static char _tmp2FB[6]="FLOAT";static char _tmp2FC[7]="DOUBLE";
static char _tmp2FD[7]="SIGNED";static char _tmp2FE[9]="UNSIGNED";static char _tmp2FF[
6]="CONST";static char _tmp300[9]="VOLATILE";static char _tmp301[9]="RESTRICT";
static char _tmp302[7]="STRUCT";static char _tmp303[6]="UNION";static char _tmp304[5]="CASE";
static char _tmp305[8]="DEFAULT";static char _tmp306[7]="INLINE";static char _tmp307[7]="SIZEOF";
static char _tmp308[9]="OFFSETOF";static char _tmp309[3]="IF";static char _tmp30A[5]="ELSE";
static char _tmp30B[7]="SWITCH";static char _tmp30C[6]="WHILE";static char _tmp30D[3]="DO";
static char _tmp30E[4]="FOR";static char _tmp30F[5]="GOTO";static char _tmp310[9]="CONTINUE";
static char _tmp311[6]="BREAK";static char _tmp312[7]="RETURN";static char _tmp313[5]="ENUM";
static char _tmp314[8]="NULL_kw";static char _tmp315[4]="LET";static char _tmp316[6]="THROW";
static char _tmp317[4]="TRY";static char _tmp318[6]="CATCH";static char _tmp319[7]="EXPORT";
static char _tmp31A[4]="NEW";static char _tmp31B[9]="ABSTRACT";static char _tmp31C[9]="FALLTHRU";
static char _tmp31D[6]="USING";static char _tmp31E[10]="NAMESPACE";static char _tmp31F[
9]="DATATYPE";static char _tmp320[8]="XTUNION";static char _tmp321[7]="TUNION";
static char _tmp322[7]="MALLOC";static char _tmp323[8]="RMALLOC";static char _tmp324[7]="CALLOC";
static char _tmp325[8]="RCALLOC";static char _tmp326[5]="SWAP";static char _tmp327[9]="REGION_T";
static char _tmp328[6]="TAG_T";static char _tmp329[7]="REGION";static char _tmp32A[5]="RNEW";
static char _tmp32B[8]="REGIONS";static char _tmp32C[13]="RESET_REGION";static char
_tmp32D[4]="GEN";static char _tmp32E[16]="NOZEROTERM_QUAL";static char _tmp32F[14]="ZEROTERM_QUAL";
static char _tmp330[12]="REGION_QUAL";static char _tmp331[7]="PORTON";static char
_tmp332[8]="PORTOFF";static char _tmp333[12]="DYNREGION_T";static char _tmp334[6]="ALIAS";
static char _tmp335[8]="NUMELTS";static char _tmp336[8]="VALUEOF";static char _tmp337[
10]="VALUEOF_T";static char _tmp338[9]="TAGCHECK";static char _tmp339[13]="NUMELTS_QUAL";
static char _tmp33A[10]="THIN_QUAL";static char _tmp33B[9]="FAT_QUAL";static char
_tmp33C[13]="NOTNULL_QUAL";static char _tmp33D[14]="NULLABLE_QUAL";static char
_tmp33E[12]="TAGGED_QUAL";static char _tmp33F[16]="EXTENSIBLE_QUAL";static char
_tmp340[15]="RESETABLE_QUAL";static char _tmp341[7]="PTR_OP";static char _tmp342[7]="INC_OP";
static char _tmp343[7]="DEC_OP";static char _tmp344[8]="LEFT_OP";static char _tmp345[9]="RIGHT_OP";
static char _tmp346[6]="LE_OP";static char _tmp347[6]="GE_OP";static char _tmp348[6]="EQ_OP";
static char _tmp349[6]="NE_OP";static char _tmp34A[7]="AND_OP";static char _tmp34B[6]="OR_OP";
static char _tmp34C[11]="MUL_ASSIGN";static char _tmp34D[11]="DIV_ASSIGN";static char
_tmp34E[11]="MOD_ASSIGN";static char _tmp34F[11]="ADD_ASSIGN";static char _tmp350[11]="SUB_ASSIGN";
static char _tmp351[12]="LEFT_ASSIGN";static char _tmp352[13]="RIGHT_ASSIGN";static
char _tmp353[11]="AND_ASSIGN";static char _tmp354[11]="XOR_ASSIGN";static char
_tmp355[10]="OR_ASSIGN";static char _tmp356[9]="ELLIPSIS";static char _tmp357[11]="LEFT_RIGHT";
static char _tmp358[12]="COLON_COLON";static char _tmp359[11]="IDENTIFIER";static
char _tmp35A[17]="INTEGER_CONSTANT";static char _tmp35B[7]="STRING";static char
_tmp35C[19]="CHARACTER_CONSTANT";static char _tmp35D[18]="FLOATING_CONSTANT";
static char _tmp35E[9]="TYPE_VAR";static char _tmp35F[13]="TYPEDEF_NAME";static char
_tmp360[16]="QUAL_IDENTIFIER";static char _tmp361[18]="QUAL_TYPEDEF_NAME";static
char _tmp362[10]="ATTRIBUTE";static char _tmp363[4]="';'";static char _tmp364[4]="'{'";
static char _tmp365[4]="'}'";static char _tmp366[4]="','";static char _tmp367[4]="'='";
static char _tmp368[4]="'<'";static char _tmp369[4]="'>'";static char _tmp36A[4]="'('";
static char _tmp36B[4]="')'";static char _tmp36C[4]="'_'";static char _tmp36D[4]="'$'";
static char _tmp36E[4]="':'";static char _tmp36F[4]="'.'";static char _tmp370[4]="'['";
static char _tmp371[4]="']'";static char _tmp372[4]="'*'";static char _tmp373[4]="'@'";
static char _tmp374[4]="'?'";static char _tmp375[4]="'+'";static char _tmp376[4]="'|'";
static char _tmp377[4]="'^'";static char _tmp378[4]="'&'";static char _tmp379[4]="'-'";
static char _tmp37A[4]="'/'";static char _tmp37B[4]="'%'";static char _tmp37C[4]="'~'";
static char _tmp37D[4]="'!'";static char _tmp37E[5]="prog";static char _tmp37F[17]="translation_unit";
static char _tmp380[12]="export_list";static char _tmp381[19]="export_list_values";
static char _tmp382[21]="external_declaration";static char _tmp383[20]="function_definition";
static char _tmp384[21]="function_definition2";static char _tmp385[13]="using_action";
static char _tmp386[15]="unusing_action";static char _tmp387[17]="namespace_action";
static char _tmp388[19]="unnamespace_action";static char _tmp389[12]="declaration";
static char _tmp38A[19]="resetable_qual_opt";static char _tmp38B[17]="declaration_list";
static char _tmp38C[23]="declaration_specifiers";static char _tmp38D[24]="storage_class_specifier";
static char _tmp38E[15]="attributes_opt";static char _tmp38F[11]="attributes";static
char _tmp390[15]="attribute_list";static char _tmp391[10]="attribute";static char
_tmp392[15]="type_specifier";static char _tmp393[25]="type_specifier_notypedef";
static char _tmp394[5]="kind";static char _tmp395[15]="type_qualifier";static char
_tmp396[15]="enum_specifier";static char _tmp397[11]="enum_field";static char
_tmp398[22]="enum_declaration_list";static char _tmp399[26]="struct_or_union_specifier";
static char _tmp39A[16]="type_params_opt";static char _tmp39B[16]="struct_or_union";
static char _tmp39C[24]="struct_declaration_list";static char _tmp39D[25]="struct_declaration_list0";
static char _tmp39E[21]="init_declarator_list";static char _tmp39F[22]="init_declarator_list0";
static char _tmp3A0[16]="init_declarator";static char _tmp3A1[19]="struct_declaration";
static char _tmp3A2[25]="specifier_qualifier_list";static char _tmp3A3[35]="notypedef_specifier_qualifier_list";
static char _tmp3A4[23]="struct_declarator_list";static char _tmp3A5[24]="struct_declarator_list0";
static char _tmp3A6[18]="struct_declarator";static char _tmp3A7[19]="datatype_specifier";
static char _tmp3A8[14]="qual_datatype";static char _tmp3A9[19]="datatypefield_list";
static char _tmp3AA[20]="datatypefield_scope";static char _tmp3AB[14]="datatypefield";
static char _tmp3AC[11]="declarator";static char _tmp3AD[23]="declarator_withtypedef";
static char _tmp3AE[18]="direct_declarator";static char _tmp3AF[30]="direct_declarator_withtypedef";
static char _tmp3B0[8]="pointer";static char _tmp3B1[12]="one_pointer";static char
_tmp3B2[14]="pointer_quals";static char _tmp3B3[13]="pointer_qual";static char
_tmp3B4[23]="pointer_null_and_bound";static char _tmp3B5[14]="pointer_bound";
static char _tmp3B6[18]="zeroterm_qual_opt";static char _tmp3B7[8]="rgn_opt";static
char _tmp3B8[11]="tqual_list";static char _tmp3B9[20]="parameter_type_list";static
char _tmp3BA[9]="type_var";static char _tmp3BB[16]="optional_effect";static char
_tmp3BC[19]="optional_rgn_order";static char _tmp3BD[10]="rgn_order";static char
_tmp3BE[16]="optional_inject";static char _tmp3BF[11]="effect_set";static char
_tmp3C0[14]="atomic_effect";static char _tmp3C1[11]="region_set";static char _tmp3C2[
15]="parameter_list";static char _tmp3C3[22]="parameter_declaration";static char
_tmp3C4[16]="identifier_list";static char _tmp3C5[17]="identifier_list0";static
char _tmp3C6[12]="initializer";static char _tmp3C7[18]="array_initializer";static
char _tmp3C8[17]="initializer_list";static char _tmp3C9[12]="designation";static
char _tmp3CA[16]="designator_list";static char _tmp3CB[11]="designator";static char
_tmp3CC[10]="type_name";static char _tmp3CD[14]="any_type_name";static char _tmp3CE[
15]="type_name_list";static char _tmp3CF[20]="abstract_declarator";static char
_tmp3D0[27]="direct_abstract_declarator";static char _tmp3D1[10]="statement";
static char _tmp3D2[13]="open_exp_opt";static char _tmp3D3[18]="labeled_statement";
static char _tmp3D4[21]="expression_statement";static char _tmp3D5[19]="compound_statement";
static char _tmp3D6[16]="block_item_list";static char _tmp3D7[20]="selection_statement";
static char _tmp3D8[15]="switch_clauses";static char _tmp3D9[20]="iteration_statement";
static char _tmp3DA[15]="jump_statement";static char _tmp3DB[12]="exp_pattern";
static char _tmp3DC[20]="conditional_pattern";static char _tmp3DD[19]="logical_or_pattern";
static char _tmp3DE[20]="logical_and_pattern";static char _tmp3DF[21]="inclusive_or_pattern";
static char _tmp3E0[21]="exclusive_or_pattern";static char _tmp3E1[12]="and_pattern";
static char _tmp3E2[17]="equality_pattern";static char _tmp3E3[19]="relational_pattern";
static char _tmp3E4[14]="shift_pattern";static char _tmp3E5[17]="additive_pattern";
static char _tmp3E6[23]="multiplicative_pattern";static char _tmp3E7[13]="cast_pattern";
static char _tmp3E8[14]="unary_pattern";static char _tmp3E9[16]="postfix_pattern";
static char _tmp3EA[16]="primary_pattern";static char _tmp3EB[8]="pattern";static
char _tmp3EC[19]="tuple_pattern_list";static char _tmp3ED[20]="tuple_pattern_list0";
static char _tmp3EE[14]="field_pattern";static char _tmp3EF[19]="field_pattern_list";
static char _tmp3F0[20]="field_pattern_list0";static char _tmp3F1[11]="expression";
static char _tmp3F2[22]="assignment_expression";static char _tmp3F3[20]="assignment_operator";
static char _tmp3F4[23]="conditional_expression";static char _tmp3F5[20]="constant_expression";
static char _tmp3F6[22]="logical_or_expression";static char _tmp3F7[23]="logical_and_expression";
static char _tmp3F8[24]="inclusive_or_expression";static char _tmp3F9[24]="exclusive_or_expression";
static char _tmp3FA[15]="and_expression";static char _tmp3FB[20]="equality_expression";
static char _tmp3FC[22]="relational_expression";static char _tmp3FD[17]="shift_expression";
static char _tmp3FE[20]="additive_expression";static char _tmp3FF[26]="multiplicative_expression";
static char _tmp400[16]="cast_expression";static char _tmp401[17]="unary_expression";
static char _tmp402[15]="unary_operator";static char _tmp403[19]="postfix_expression";
static char _tmp404[19]="primary_expression";static char _tmp405[25]="argument_expression_list";
static char _tmp406[26]="argument_expression_list0";static char _tmp407[9]="constant";
static char _tmp408[20]="qual_opt_identifier";static char _tmp409[17]="qual_opt_typedef";
static char _tmp40A[18]="struct_union_name";static char _tmp40B[11]="field_name";
static char _tmp40C[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[287]={{
_tmp2EE,_tmp2EE,_tmp2EE + 2},{_tmp2EF,_tmp2EF,_tmp2EF + 6},{_tmp2F0,_tmp2F0,
_tmp2F0 + 12},{_tmp2F1,_tmp2F1,_tmp2F1 + 5},{_tmp2F2,_tmp2F2,_tmp2F2 + 9},{_tmp2F3,
_tmp2F3,_tmp2F3 + 7},{_tmp2F4,_tmp2F4,_tmp2F4 + 7},{_tmp2F5,_tmp2F5,_tmp2F5 + 8},{
_tmp2F6,_tmp2F6,_tmp2F6 + 5},{_tmp2F7,_tmp2F7,_tmp2F7 + 5},{_tmp2F8,_tmp2F8,
_tmp2F8 + 6},{_tmp2F9,_tmp2F9,_tmp2F9 + 4},{_tmp2FA,_tmp2FA,_tmp2FA + 5},{_tmp2FB,
_tmp2FB,_tmp2FB + 6},{_tmp2FC,_tmp2FC,_tmp2FC + 7},{_tmp2FD,_tmp2FD,_tmp2FD + 7},{
_tmp2FE,_tmp2FE,_tmp2FE + 9},{_tmp2FF,_tmp2FF,_tmp2FF + 6},{_tmp300,_tmp300,
_tmp300 + 9},{_tmp301,_tmp301,_tmp301 + 9},{_tmp302,_tmp302,_tmp302 + 7},{_tmp303,
_tmp303,_tmp303 + 6},{_tmp304,_tmp304,_tmp304 + 5},{_tmp305,_tmp305,_tmp305 + 8},{
_tmp306,_tmp306,_tmp306 + 7},{_tmp307,_tmp307,_tmp307 + 7},{_tmp308,_tmp308,
_tmp308 + 9},{_tmp309,_tmp309,_tmp309 + 3},{_tmp30A,_tmp30A,_tmp30A + 5},{_tmp30B,
_tmp30B,_tmp30B + 7},{_tmp30C,_tmp30C,_tmp30C + 6},{_tmp30D,_tmp30D,_tmp30D + 3},{
_tmp30E,_tmp30E,_tmp30E + 4},{_tmp30F,_tmp30F,_tmp30F + 5},{_tmp310,_tmp310,
_tmp310 + 9},{_tmp311,_tmp311,_tmp311 + 6},{_tmp312,_tmp312,_tmp312 + 7},{_tmp313,
_tmp313,_tmp313 + 5},{_tmp314,_tmp314,_tmp314 + 8},{_tmp315,_tmp315,_tmp315 + 4},{
_tmp316,_tmp316,_tmp316 + 6},{_tmp317,_tmp317,_tmp317 + 4},{_tmp318,_tmp318,
_tmp318 + 6},{_tmp319,_tmp319,_tmp319 + 7},{_tmp31A,_tmp31A,_tmp31A + 4},{_tmp31B,
_tmp31B,_tmp31B + 9},{_tmp31C,_tmp31C,_tmp31C + 9},{_tmp31D,_tmp31D,_tmp31D + 6},{
_tmp31E,_tmp31E,_tmp31E + 10},{_tmp31F,_tmp31F,_tmp31F + 9},{_tmp320,_tmp320,
_tmp320 + 8},{_tmp321,_tmp321,_tmp321 + 7},{_tmp322,_tmp322,_tmp322 + 7},{_tmp323,
_tmp323,_tmp323 + 8},{_tmp324,_tmp324,_tmp324 + 7},{_tmp325,_tmp325,_tmp325 + 8},{
_tmp326,_tmp326,_tmp326 + 5},{_tmp327,_tmp327,_tmp327 + 9},{_tmp328,_tmp328,
_tmp328 + 6},{_tmp329,_tmp329,_tmp329 + 7},{_tmp32A,_tmp32A,_tmp32A + 5},{_tmp32B,
_tmp32B,_tmp32B + 8},{_tmp32C,_tmp32C,_tmp32C + 13},{_tmp32D,_tmp32D,_tmp32D + 4},{
_tmp32E,_tmp32E,_tmp32E + 16},{_tmp32F,_tmp32F,_tmp32F + 14},{_tmp330,_tmp330,
_tmp330 + 12},{_tmp331,_tmp331,_tmp331 + 7},{_tmp332,_tmp332,_tmp332 + 8},{_tmp333,
_tmp333,_tmp333 + 12},{_tmp334,_tmp334,_tmp334 + 6},{_tmp335,_tmp335,_tmp335 + 8},{
_tmp336,_tmp336,_tmp336 + 8},{_tmp337,_tmp337,_tmp337 + 10},{_tmp338,_tmp338,
_tmp338 + 9},{_tmp339,_tmp339,_tmp339 + 13},{_tmp33A,_tmp33A,_tmp33A + 10},{_tmp33B,
_tmp33B,_tmp33B + 9},{_tmp33C,_tmp33C,_tmp33C + 13},{_tmp33D,_tmp33D,_tmp33D + 14},{
_tmp33E,_tmp33E,_tmp33E + 12},{_tmp33F,_tmp33F,_tmp33F + 16},{_tmp340,_tmp340,
_tmp340 + 15},{_tmp341,_tmp341,_tmp341 + 7},{_tmp342,_tmp342,_tmp342 + 7},{_tmp343,
_tmp343,_tmp343 + 7},{_tmp344,_tmp344,_tmp344 + 8},{_tmp345,_tmp345,_tmp345 + 9},{
_tmp346,_tmp346,_tmp346 + 6},{_tmp347,_tmp347,_tmp347 + 6},{_tmp348,_tmp348,
_tmp348 + 6},{_tmp349,_tmp349,_tmp349 + 6},{_tmp34A,_tmp34A,_tmp34A + 7},{_tmp34B,
_tmp34B,_tmp34B + 6},{_tmp34C,_tmp34C,_tmp34C + 11},{_tmp34D,_tmp34D,_tmp34D + 11},{
_tmp34E,_tmp34E,_tmp34E + 11},{_tmp34F,_tmp34F,_tmp34F + 11},{_tmp350,_tmp350,
_tmp350 + 11},{_tmp351,_tmp351,_tmp351 + 12},{_tmp352,_tmp352,_tmp352 + 13},{
_tmp353,_tmp353,_tmp353 + 11},{_tmp354,_tmp354,_tmp354 + 11},{_tmp355,_tmp355,
_tmp355 + 10},{_tmp356,_tmp356,_tmp356 + 9},{_tmp357,_tmp357,_tmp357 + 11},{_tmp358,
_tmp358,_tmp358 + 12},{_tmp359,_tmp359,_tmp359 + 11},{_tmp35A,_tmp35A,_tmp35A + 17},{
_tmp35B,_tmp35B,_tmp35B + 7},{_tmp35C,_tmp35C,_tmp35C + 19},{_tmp35D,_tmp35D,
_tmp35D + 18},{_tmp35E,_tmp35E,_tmp35E + 9},{_tmp35F,_tmp35F,_tmp35F + 13},{_tmp360,
_tmp360,_tmp360 + 16},{_tmp361,_tmp361,_tmp361 + 18},{_tmp362,_tmp362,_tmp362 + 10},{
_tmp363,_tmp363,_tmp363 + 4},{_tmp364,_tmp364,_tmp364 + 4},{_tmp365,_tmp365,
_tmp365 + 4},{_tmp366,_tmp366,_tmp366 + 4},{_tmp367,_tmp367,_tmp367 + 4},{_tmp368,
_tmp368,_tmp368 + 4},{_tmp369,_tmp369,_tmp369 + 4},{_tmp36A,_tmp36A,_tmp36A + 4},{
_tmp36B,_tmp36B,_tmp36B + 4},{_tmp36C,_tmp36C,_tmp36C + 4},{_tmp36D,_tmp36D,
_tmp36D + 4},{_tmp36E,_tmp36E,_tmp36E + 4},{_tmp36F,_tmp36F,_tmp36F + 4},{_tmp370,
_tmp370,_tmp370 + 4},{_tmp371,_tmp371,_tmp371 + 4},{_tmp372,_tmp372,_tmp372 + 4},{
_tmp373,_tmp373,_tmp373 + 4},{_tmp374,_tmp374,_tmp374 + 4},{_tmp375,_tmp375,
_tmp375 + 4},{_tmp376,_tmp376,_tmp376 + 4},{_tmp377,_tmp377,_tmp377 + 4},{_tmp378,
_tmp378,_tmp378 + 4},{_tmp379,_tmp379,_tmp379 + 4},{_tmp37A,_tmp37A,_tmp37A + 4},{
_tmp37B,_tmp37B,_tmp37B + 4},{_tmp37C,_tmp37C,_tmp37C + 4},{_tmp37D,_tmp37D,
_tmp37D + 4},{_tmp37E,_tmp37E,_tmp37E + 5},{_tmp37F,_tmp37F,_tmp37F + 17},{_tmp380,
_tmp380,_tmp380 + 12},{_tmp381,_tmp381,_tmp381 + 19},{_tmp382,_tmp382,_tmp382 + 21},{
_tmp383,_tmp383,_tmp383 + 20},{_tmp384,_tmp384,_tmp384 + 21},{_tmp385,_tmp385,
_tmp385 + 13},{_tmp386,_tmp386,_tmp386 + 15},{_tmp387,_tmp387,_tmp387 + 17},{
_tmp388,_tmp388,_tmp388 + 19},{_tmp389,_tmp389,_tmp389 + 12},{_tmp38A,_tmp38A,
_tmp38A + 19},{_tmp38B,_tmp38B,_tmp38B + 17},{_tmp38C,_tmp38C,_tmp38C + 23},{
_tmp38D,_tmp38D,_tmp38D + 24},{_tmp38E,_tmp38E,_tmp38E + 15},{_tmp38F,_tmp38F,
_tmp38F + 11},{_tmp390,_tmp390,_tmp390 + 15},{_tmp391,_tmp391,_tmp391 + 10},{
_tmp392,_tmp392,_tmp392 + 15},{_tmp393,_tmp393,_tmp393 + 25},{_tmp394,_tmp394,
_tmp394 + 5},{_tmp395,_tmp395,_tmp395 + 15},{_tmp396,_tmp396,_tmp396 + 15},{_tmp397,
_tmp397,_tmp397 + 11},{_tmp398,_tmp398,_tmp398 + 22},{_tmp399,_tmp399,_tmp399 + 26},{
_tmp39A,_tmp39A,_tmp39A + 16},{_tmp39B,_tmp39B,_tmp39B + 16},{_tmp39C,_tmp39C,
_tmp39C + 24},{_tmp39D,_tmp39D,_tmp39D + 25},{_tmp39E,_tmp39E,_tmp39E + 21},{
_tmp39F,_tmp39F,_tmp39F + 22},{_tmp3A0,_tmp3A0,_tmp3A0 + 16},{_tmp3A1,_tmp3A1,
_tmp3A1 + 19},{_tmp3A2,_tmp3A2,_tmp3A2 + 25},{_tmp3A3,_tmp3A3,_tmp3A3 + 35},{
_tmp3A4,_tmp3A4,_tmp3A4 + 23},{_tmp3A5,_tmp3A5,_tmp3A5 + 24},{_tmp3A6,_tmp3A6,
_tmp3A6 + 18},{_tmp3A7,_tmp3A7,_tmp3A7 + 19},{_tmp3A8,_tmp3A8,_tmp3A8 + 14},{
_tmp3A9,_tmp3A9,_tmp3A9 + 19},{_tmp3AA,_tmp3AA,_tmp3AA + 20},{_tmp3AB,_tmp3AB,
_tmp3AB + 14},{_tmp3AC,_tmp3AC,_tmp3AC + 11},{_tmp3AD,_tmp3AD,_tmp3AD + 23},{
_tmp3AE,_tmp3AE,_tmp3AE + 18},{_tmp3AF,_tmp3AF,_tmp3AF + 30},{_tmp3B0,_tmp3B0,
_tmp3B0 + 8},{_tmp3B1,_tmp3B1,_tmp3B1 + 12},{_tmp3B2,_tmp3B2,_tmp3B2 + 14},{_tmp3B3,
_tmp3B3,_tmp3B3 + 13},{_tmp3B4,_tmp3B4,_tmp3B4 + 23},{_tmp3B5,_tmp3B5,_tmp3B5 + 14},{
_tmp3B6,_tmp3B6,_tmp3B6 + 18},{_tmp3B7,_tmp3B7,_tmp3B7 + 8},{_tmp3B8,_tmp3B8,
_tmp3B8 + 11},{_tmp3B9,_tmp3B9,_tmp3B9 + 20},{_tmp3BA,_tmp3BA,_tmp3BA + 9},{_tmp3BB,
_tmp3BB,_tmp3BB + 16},{_tmp3BC,_tmp3BC,_tmp3BC + 19},{_tmp3BD,_tmp3BD,_tmp3BD + 10},{
_tmp3BE,_tmp3BE,_tmp3BE + 16},{_tmp3BF,_tmp3BF,_tmp3BF + 11},{_tmp3C0,_tmp3C0,
_tmp3C0 + 14},{_tmp3C1,_tmp3C1,_tmp3C1 + 11},{_tmp3C2,_tmp3C2,_tmp3C2 + 15},{
_tmp3C3,_tmp3C3,_tmp3C3 + 22},{_tmp3C4,_tmp3C4,_tmp3C4 + 16},{_tmp3C5,_tmp3C5,
_tmp3C5 + 17},{_tmp3C6,_tmp3C6,_tmp3C6 + 12},{_tmp3C7,_tmp3C7,_tmp3C7 + 18},{
_tmp3C8,_tmp3C8,_tmp3C8 + 17},{_tmp3C9,_tmp3C9,_tmp3C9 + 12},{_tmp3CA,_tmp3CA,
_tmp3CA + 16},{_tmp3CB,_tmp3CB,_tmp3CB + 11},{_tmp3CC,_tmp3CC,_tmp3CC + 10},{
_tmp3CD,_tmp3CD,_tmp3CD + 14},{_tmp3CE,_tmp3CE,_tmp3CE + 15},{_tmp3CF,_tmp3CF,
_tmp3CF + 20},{_tmp3D0,_tmp3D0,_tmp3D0 + 27},{_tmp3D1,_tmp3D1,_tmp3D1 + 10},{
_tmp3D2,_tmp3D2,_tmp3D2 + 13},{_tmp3D3,_tmp3D3,_tmp3D3 + 18},{_tmp3D4,_tmp3D4,
_tmp3D4 + 21},{_tmp3D5,_tmp3D5,_tmp3D5 + 19},{_tmp3D6,_tmp3D6,_tmp3D6 + 16},{
_tmp3D7,_tmp3D7,_tmp3D7 + 20},{_tmp3D8,_tmp3D8,_tmp3D8 + 15},{_tmp3D9,_tmp3D9,
_tmp3D9 + 20},{_tmp3DA,_tmp3DA,_tmp3DA + 15},{_tmp3DB,_tmp3DB,_tmp3DB + 12},{
_tmp3DC,_tmp3DC,_tmp3DC + 20},{_tmp3DD,_tmp3DD,_tmp3DD + 19},{_tmp3DE,_tmp3DE,
_tmp3DE + 20},{_tmp3DF,_tmp3DF,_tmp3DF + 21},{_tmp3E0,_tmp3E0,_tmp3E0 + 21},{
_tmp3E1,_tmp3E1,_tmp3E1 + 12},{_tmp3E2,_tmp3E2,_tmp3E2 + 17},{_tmp3E3,_tmp3E3,
_tmp3E3 + 19},{_tmp3E4,_tmp3E4,_tmp3E4 + 14},{_tmp3E5,_tmp3E5,_tmp3E5 + 17},{
_tmp3E6,_tmp3E6,_tmp3E6 + 23},{_tmp3E7,_tmp3E7,_tmp3E7 + 13},{_tmp3E8,_tmp3E8,
_tmp3E8 + 14},{_tmp3E9,_tmp3E9,_tmp3E9 + 16},{_tmp3EA,_tmp3EA,_tmp3EA + 16},{
_tmp3EB,_tmp3EB,_tmp3EB + 8},{_tmp3EC,_tmp3EC,_tmp3EC + 19},{_tmp3ED,_tmp3ED,
_tmp3ED + 20},{_tmp3EE,_tmp3EE,_tmp3EE + 14},{_tmp3EF,_tmp3EF,_tmp3EF + 19},{
_tmp3F0,_tmp3F0,_tmp3F0 + 20},{_tmp3F1,_tmp3F1,_tmp3F1 + 11},{_tmp3F2,_tmp3F2,
_tmp3F2 + 22},{_tmp3F3,_tmp3F3,_tmp3F3 + 20},{_tmp3F4,_tmp3F4,_tmp3F4 + 23},{
_tmp3F5,_tmp3F5,_tmp3F5 + 20},{_tmp3F6,_tmp3F6,_tmp3F6 + 22},{_tmp3F7,_tmp3F7,
_tmp3F7 + 23},{_tmp3F8,_tmp3F8,_tmp3F8 + 24},{_tmp3F9,_tmp3F9,_tmp3F9 + 24},{
_tmp3FA,_tmp3FA,_tmp3FA + 15},{_tmp3FB,_tmp3FB,_tmp3FB + 20},{_tmp3FC,_tmp3FC,
_tmp3FC + 22},{_tmp3FD,_tmp3FD,_tmp3FD + 17},{_tmp3FE,_tmp3FE,_tmp3FE + 20},{
_tmp3FF,_tmp3FF,_tmp3FF + 26},{_tmp400,_tmp400,_tmp400 + 16},{_tmp401,_tmp401,
_tmp401 + 17},{_tmp402,_tmp402,_tmp402 + 15},{_tmp403,_tmp403,_tmp403 + 19},{
_tmp404,_tmp404,_tmp404 + 19},{_tmp405,_tmp405,_tmp405 + 25},{_tmp406,_tmp406,
_tmp406 + 26},{_tmp407,_tmp407,_tmp407 + 9},{_tmp408,_tmp408,_tmp408 + 20},{_tmp409,
_tmp409,_tmp409 + 17},{_tmp40A,_tmp40A,_tmp40A + 18},{_tmp40B,_tmp40B,_tmp40B + 11},{
_tmp40C,_tmp40C,_tmp40C + 12}};static short Cyc_yyr1[510]={0,144,145,145,145,145,
145,145,145,145,145,145,146,146,147,147,148,148,148,149,149,149,149,150,150,151,
152,153,154,155,155,155,155,155,155,155,156,156,157,157,158,158,158,158,158,158,
158,158,158,158,159,159,159,159,159,159,159,160,160,161,162,162,163,163,163,163,
163,163,164,164,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
165,165,165,165,165,165,166,167,167,167,168,168,168,169,169,170,170,170,171,171,
171,171,171,172,172,173,173,174,174,175,175,176,177,177,178,178,179,180,180,180,
180,180,180,181,181,181,181,181,181,182,183,183,184,184,184,185,185,185,186,186,
187,187,187,187,188,188,188,189,189,190,190,191,191,192,192,192,192,192,192,192,
192,192,192,193,193,193,193,193,193,193,193,193,193,193,194,194,195,196,196,197,
197,197,197,197,197,197,197,198,198,198,199,199,200,200,200,201,201,201,202,202,
203,203,203,203,204,204,205,205,206,206,207,207,208,208,209,209,210,210,210,210,
211,211,212,212,213,213,213,214,215,215,216,216,217,217,217,217,218,218,218,218,
219,220,220,221,221,222,222,223,223,223,223,223,224,224,225,225,225,226,226,226,
226,226,226,226,226,226,226,226,227,227,227,227,227,227,227,228,228,229,230,230,
231,231,232,232,232,232,232,232,233,233,233,233,233,233,234,234,234,234,234,234,
235,235,235,235,235,235,235,235,235,235,235,235,235,235,236,236,236,236,236,236,
236,236,237,238,238,239,239,240,240,241,241,242,242,243,243,244,244,244,245,245,
245,245,245,246,246,246,247,247,247,248,248,248,248,249,249,250,250,250,250,250,
250,250,251,252,253,253,253,253,253,253,253,253,253,253,253,253,253,253,253,254,
254,254,255,255,256,256,257,257,257,258,258,259,259,260,260,260,261,261,261,261,
261,261,261,261,261,261,261,262,262,262,262,262,262,262,263,264,264,265,265,266,
266,267,267,268,268,269,269,269,270,270,270,270,270,271,271,271,272,272,272,273,
273,273,273,274,274,275,275,275,275,275,275,275,275,275,275,275,275,275,275,275,
275,275,275,275,275,276,276,276,277,277,277,277,277,277,277,277,277,277,278,278,
278,278,278,278,278,278,278,279,280,280,281,281,281,281,282,282,283,283,284,284,
285,285,286,286};static short Cyc_yyr2[510]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,
2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,8,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,
6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,4,6,4,1,4,1,1,1,1,5,2,4,
1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,
2,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,
2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,
1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,
3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,
6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,
1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,4,
2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,
5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,
2,6,6,5,4,6,9,11,4,6,6,4,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,
1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[1032]={0,18,50,51,52,53,55,70,73,
74,75,76,77,78,79,80,93,94,95,111,112,46,0,0,56,0,0,144,0,90,0,0,0,0,0,0,0,37,
500,210,502,501,503,0,0,71,0,196,196,195,1,0,16,0,0,17,0,0,40,48,42,68,44,81,82,
0,83,0,36,155,0,180,183,84,159,109,54,53,47,0,97,499,0,500,496,497,498,109,0,370,
0,0,0,0,233,0,372,373,25,27,0,0,0,0,0,0,0,0,145,0,0,0,0,0,0,0,193,194,2,0,0,0,0,
0,29,0,117,118,120,41,49,43,45,113,504,505,109,109,36,38,36,0,19,0,212,0,168,156,
181,190,189,0,0,187,188,191,192,201,183,0,69,0,54,101,0,99,0,500,380,0,0,0,0,0,0,
0,0,0,0,0,0,109,0,0,0,0,0,486,0,0,0,0,0,473,471,472,0,397,399,413,421,423,425,
427,429,431,434,439,442,445,449,0,451,474,485,484,0,381,379,32,0,0,109,0,0,0,127,
123,125,251,253,0,0,9,10,0,0,0,109,506,507,211,92,0,0,160,72,231,0,228,0,3,0,5,0,
281,0,30,0,0,36,20,0,114,115,0,108,0,142,0,0,0,0,0,0,0,0,0,0,0,0,500,284,286,0,
292,288,0,290,274,275,276,0,277,278,279,0,39,21,120,258,0,218,234,0,0,214,212,0,
198,0,0,0,203,57,202,184,0,0,102,98,0,0,374,0,0,394,0,0,0,0,247,390,395,0,392,0,
459,0,415,449,0,416,417,0,0,0,0,0,0,0,0,0,0,452,453,36,0,0,0,455,456,454,0,371,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,403,404,405,406,407,408,409,410,411,412,
402,0,457,0,480,481,0,0,0,488,0,109,387,388,0,385,0,235,0,0,0,0,254,226,0,128,
133,129,131,124,126,212,0,260,252,261,509,508,0,86,89,0,87,0,91,107,63,62,0,60,
161,212,230,157,260,232,169,170,0,85,197,26,0,28,0,0,0,0,119,121,237,236,22,104,
116,0,0,0,135,136,138,0,109,109,150,0,0,0,0,0,0,36,0,321,322,323,0,0,325,0,0,0,0,
293,289,120,291,287,285,0,167,219,0,0,0,225,213,220,164,0,0,0,214,166,200,199,
162,198,0,0,204,58,110,0,103,420,100,96,383,384,250,0,391,246,248,378,0,0,0,0,
238,242,0,0,0,0,0,0,0,0,0,0,0,0,0,0,487,494,0,493,398,422,0,424,426,428,430,432,
433,437,438,435,436,440,441,443,444,446,447,448,401,400,479,476,0,478,0,0,0,375,
0,382,31,0,376,0,0,255,134,130,132,0,214,0,198,0,262,0,212,0,273,257,0,0,109,0,0,
0,127,0,109,0,212,0,179,158,229,4,6,0,34,0,0,139,122,0,0,214,143,152,151,0,0,146,
0,0,0,300,0,0,0,0,0,320,324,0,0,0,0,283,36,23,259,212,0,222,0,0,215,0,165,218,
206,163,186,185,204,182,0,7,0,249,393,396,458,0,0,239,0,243,463,0,0,0,0,0,467,
470,0,0,0,0,0,450,490,0,0,477,475,0,0,386,389,377,256,227,269,0,263,264,198,0,0,
214,198,0,88,0,214,0,0,0,59,61,0,171,0,0,214,0,198,0,0,0,137,140,113,141,153,150,
150,0,0,0,0,0,0,0,0,0,0,0,0,0,300,326,0,0,24,214,0,223,221,0,214,0,205,0,8,0,0,0,
240,244,0,0,0,0,418,419,462,479,478,492,0,495,414,489,491,0,268,266,272,271,0,
265,198,0,113,0,66,64,65,172,178,175,0,177,173,198,0,33,0,0,149,148,294,300,0,0,
0,0,0,0,328,329,331,333,335,337,339,341,344,349,352,355,359,361,368,369,0,36,297,
306,0,0,0,0,0,0,0,0,0,0,327,280,208,224,216,207,212,13,0,0,461,460,0,245,464,0,0,
469,468,482,0,270,267,35,0,0,176,174,282,106,0,0,0,300,0,365,0,0,362,36,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,363,301,0,308,0,0,0,316,0,0,0,0,0,0,299,0,214,12,
14,0,0,0,0,483,105,0,154,295,296,0,0,0,0,300,302,332,0,334,336,338,340,342,343,
347,348,345,346,350,351,353,354,356,357,358,0,307,309,310,0,318,317,0,312,0,0,0,
217,209,15,0,0,0,0,298,364,0,360,303,0,36,311,319,313,314,0,241,465,0,67,0,0,330,
300,304,315,0,367,366,305,466,0,0,0};static short Cyc_yydefgoto[143]={1029,50,685,
877,51,52,285,53,457,54,459,55,56,140,57,58,525,226,443,444,227,61,241,228,63,
163,164,64,160,65,262,263,125,126,127,264,229,424,472,473,474,66,67,646,647,648,
68,475,69,448,70,71,157,158,72,116,521,314,682,606,73,607,515,673,507,511,512,
419,307,249,93,94,546,465,547,329,330,331,230,300,301,608,430,288,461,289,290,
291,292,293,761,294,295,838,839,840,841,842,843,844,845,846,847,848,849,850,851,
852,853,332,409,410,333,334,335,296,197,396,198,531,199,200,201,202,203,204,205,
206,207,208,209,210,211,212,213,563,564,214,215,75,878,242,434};static short Cyc_yypact[
1032]={2762,- -32768,- -32768,- -32768,- -32768,- 78,- -32768,- -32768,- -32768,- -32768,
- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
- -32768,3216,456,4445,- -32768,- 60,- 10,- -32768,- 6,7,39,73,51,93,94,325,192,-
-32768,- -32768,167,- -32768,- -32768,- -32768,159,634,194,185,132,132,- -32768,-
-32768,2627,- -32768,370,720,- -32768,313,189,3216,3216,3216,- -32768,3216,- -32768,
- -32768,656,- -32768,- 60,3136,165,107,726,815,- -32768,- -32768,198,271,295,- -32768,
- 60,311,- -32768,4538,242,- -32768,- -32768,- -32768,198,5805,- -32768,312,347,4538,
344,354,346,- -32768,70,- -32768,- -32768,3364,3364,2627,2627,3364,387,5805,520,-
-32768,277,397,585,386,277,3586,5805,- -32768,- -32768,- -32768,2627,2896,2627,2896,
42,- -32768,415,434,- -32768,3056,- -32768,- -32768,- -32768,- -32768,3586,- -32768,-
-32768,198,222,1653,- -32768,3136,189,- -32768,3364,3290,4209,- -32768,165,- -32768,
- -32768,- -32768,440,442,- -32768,- -32768,- -32768,- -32768,158,815,3364,- -32768,
2896,- -32768,457,475,499,- 60,47,- -32768,4538,221,1012,5982,498,5805,5846,507,519,
536,551,553,198,560,599,601,6074,6074,- -32768,2212,603,6115,6115,6115,- -32768,-
-32768,- -32768,204,- -32768,- -32768,- 22,612,641,598,671,663,140,710,- 69,261,-
-32768,505,6115,349,13,- -32768,689,3596,719,- -32768,- -32768,728,5805,198,3596,
731,199,3586,3660,3586,578,- -32768,85,85,- -32768,- -32768,181,717,338,198,- -32768,
- -32768,- -32768,- -32768,44,744,- -32768,- -32768,668,448,- -32768,758,- -32768,766,-
-32768,782,789,791,- -32768,585,4305,3136,- -32768,793,3586,- -32768,600,796,- 60,
806,805,383,811,3729,813,829,821,822,4401,3729,233,816,814,- -32768,- -32768,817,
1794,1794,189,1794,- -32768,- -32768,- -32768,824,- -32768,- -32768,- -32768,- 67,-
-32768,- -32768,825,810,126,842,- -32768,80,826,830,381,827,752,819,3364,5805,-
-32768,838,- -32768,- -32768,126,834,- 60,- -32768,5805,843,- -32768,844,845,- -32768,
277,5805,4538,287,- -32768,- -32768,- -32768,847,850,2212,- -32768,3586,- -32768,-
-32768,3825,- -32768,871,5805,5805,5805,5805,5805,848,5805,3586,228,2212,- -32768,
- -32768,1935,840,530,5805,- -32768,- -32768,- -32768,5805,- -32768,6115,5805,6115,
6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,
5805,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,5805,- -32768,277,- -32768,- -32768,4494,277,5805,- -32768,849,198,-
-32768,- -32768,851,854,4538,- -32768,403,1012,853,3364,- -32768,855,860,- -32768,
3660,3660,3660,- -32768,- -32768,1463,4587,148,- -32768,251,- -32768,- -32768,80,-
-32768,- -32768,3364,- -32768,874,- -32768,862,- -32768,858,859,863,- -32768,1156,-
-32768,321,412,- -32768,- -32768,- -32768,3586,- -32768,- -32768,- -32768,2627,- -32768,
2627,878,869,864,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,738,
5805,875,879,- -32768,867,474,198,198,837,5805,5805,876,883,5805,961,2076,885,-
-32768,- -32768,- -32768,414,962,- -32768,4683,5805,3729,2348,- -32768,- -32768,3056,
- -32768,- -32768,- -32768,3364,- -32768,- -32768,3586,881,258,- -32768,- -32768,868,-
-32768,80,882,3512,830,- -32768,- -32768,- -32768,- -32768,752,- 26,884,846,- -32768,-
-32768,2492,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,886,- -32768,
- -32768,- -32768,- -32768,3226,887,896,899,- -32768,- -32768,764,4305,537,898,900,
902,543,3586,546,894,613,901,904,5941,- -32768,- -32768,903,905,- -32768,612,83,641,
598,671,663,140,140,710,710,710,710,- 69,- 69,261,261,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,906,- -32768,48,3364,4113,- -32768,6233,- -32768,-
-32768,908,- -32768,98,917,- -32768,- -32768,- -32768,- -32768,907,830,909,752,913,
251,3364,3438,4776,- -32768,- -32768,85,912,198,759,911,44,2976,915,198,3364,3290,
4872,- -32768,321,- -32768,- -32768,- -32768,914,- -32768,930,715,- -32768,- -32768,600,
5805,830,- -32768,- -32768,- -32768,926,- 60,458,563,580,5805,873,606,923,4968,5061,
492,- -32768,- -32768,933,935,928,627,- -32768,3136,- -32768,810,937,3364,- -32768,
938,80,- -32768,939,- -32768,152,- -32768,- -32768,- -32768,- -32768,846,- -32768,943,-
-32768,2627,- -32768,- -32768,- -32768,946,306,944,- -32768,3921,- -32768,- -32768,
5805,1040,5805,5846,942,- -32768,- -32768,277,277,946,945,4113,- -32768,- -32768,
5805,5805,- -32768,- -32768,126,778,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,947,- -32768,- -32768,752,126,948,830,752,940,- -32768,5805,830,636,949,950,
- -32768,- -32768,951,- -32768,126,952,830,955,752,967,5805,982,- -32768,- -32768,
3586,- -32768,976,86,837,3729,983,977,3522,975,986,3729,5805,5154,556,5247,574,
5340,873,- -32768,989,992,- -32768,830,105,- -32768,- -32768,956,830,3586,- -32768,
510,- -32768,985,987,5805,- -32768,- -32768,4305,659,990,993,- -32768,871,- -32768,
996,999,- -32768,798,- -32768,- -32768,- -32768,- -32768,4113,- -32768,- -32768,- -32768,
- -32768,1000,- -32768,752,619,3586,1007,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,1004,- -32768,- -32768,752,666,- -32768,998,3586,- -32768,- -32768,1090,873,
1013,6207,1008,2348,6115,1009,- -32768,88,- -32768,1043,1010,783,832,243,841,514,
377,- -32768,- -32768,- -32768,- -32768,1048,6115,1935,- -32768,- -32768,669,3729,679,
5433,3729,683,5526,5619,645,1024,- -32768,- -32768,- -32768,- -32768,1025,- -32768,
937,- -32768,1028,673,- -32768,- -32768,174,- -32768,- -32768,3586,1123,- -32768,-
-32768,- -32768,4017,- -32768,- -32768,- -32768,1030,1031,- -32768,- -32768,- -32768,-
-32768,686,3729,1033,873,2212,- -32768,3586,1034,- -32768,1327,6115,5805,6115,6115,
6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,6115,5805,
- -32768,- -32768,1036,- -32768,3729,3729,690,- -32768,3729,3729,699,3729,707,5712,-
-32768,80,830,- -32768,- -32768,520,5805,1035,1037,- -32768,- -32768,1047,- -32768,-
-32768,- -32768,1044,1054,1060,6115,873,- -32768,612,266,641,598,598,663,140,140,
710,710,710,710,- 69,- 69,261,261,- -32768,- -32768,- -32768,322,- -32768,- -32768,-
-32768,3729,- -32768,- -32768,3729,- -32768,3729,3729,716,- -32768,- -32768,- -32768,
812,1056,3586,1057,- -32768,946,539,- -32768,- -32768,5805,1327,- -32768,- -32768,-
-32768,- -32768,3729,- -32768,- -32768,1058,- -32768,1059,1061,- -32768,873,- -32768,-
-32768,1062,- -32768,- -32768,- -32768,- -32768,1189,1190,- -32768};static short Cyc_yypgoto[
143]={- -32768,136,- -32768,244,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- 51,- -32768,- 105,22,- -32768,- -32768,0,569,- -32768,55,- 170,1081,5,- -32768,
- -32768,- 134,- -32768,138,1161,- 715,- -32768,- -32768,- -32768,941,953,681,453,-
-32768,- -32768,558,- -32768,- -32768,179,- -32768,- -32768,95,- 199,1130,- 397,14,-
-32768,1045,- -32768,- -32768,1153,- 342,- -32768,523,- 138,- 62,- 111,- 416,262,532,538,
- 420,- 471,- 106,- 411,- 124,- -32768,- 222,- 153,- 548,- 339,- -32768,888,- 154,144,- 120,-
58,- 318,56,- -32768,- -32768,- -32768,4,- 270,- -32768,- 349,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,1,988,- -32768,674,795,- -32768,292,614,-
-32768,- 173,- 293,- 150,- 358,- 355,- 365,852,- 356,- 344,- 239,- 337,- 305,- 181,714,460,
865,- -32768,- 352,- -32768,- 4,374,- 61,46,- 298,- 25};static short Cyc_yytable[6372]={
59,339,548,569,135,62,305,566,248,360,361,362,568,616,571,498,499,139,501,96,308,
59,342,260,95,343,62,572,573,535,397,76,322,306,357,536,827,464,671,317,578,579,
631,78,111,715,135,38,447,589,503,59,630,363,41,60,62,422,59,59,59,441,59,62,62,
62,379,62,59,146,380,365,142,62,580,581,60,139,96,630,129,130,131,168,132,148,
559,286,96,297,141,644,645,218,674,315,668,99,893,679,587,677,59,59,590,631,60,
62,62,433,611,136,366,60,60,60,100,60,404,59,59,59,59,60,62,62,62,62,59,101,720,
611,261,62,574,575,576,577,59,112,59,508,662,62,298,62,405,146,530,255,141,442,
128,238,169,530,102,60,60,799,287,59,141,418,256,96,62,96,363,170,323,429,431,
104,60,60,60,60,638,713,678,909,542,60,543,529,452,118,222,450,103,722,39,60,223,
60,517,556,509,558,582,583,584,363,452,- 147,244,435,432,297,437,711,96,431,38,
105,60,408,106,96,433,41,910,719,408,171,751,421,373,374,872,44,423,433,286,286,
299,286,233,234,433,108,510,428,231,232,504,624,235,432,115,422,422,422,548,251,
252,253,254,506,59,449,375,376,467,62,81,724,431,304,39,624,426,109,266,268,505,
- 212,427,476,- 212,43,141,110,313,59,59,143,59,144,62,62,527,62,363,145,38,318,
530,758,113,436,949,41,432,286,124,287,287,114,287,39,809,44,526,60,433,813,417,
349,159,47,48,49,363,96,694,821,485,364,537,917,918,324,492,38,84,187,85,86,60,
60,41,60,159,19,20,325,750,169,493,267,353,299,789,189,59,494,871,- 234,414,62,-
234,874,170,919,920,43,548,875,39,510,123,612,74,613,440,670,287,708,196,614,500,
806,239,625,363,810,452,161,240,248,785,381,1007,665,80,97,237,98,700,382,383,
823,162,796,797,96,538,452,96,60,596,239,784,452,327,328,74,240,868,421,421,421,
739,74,423,423,423,166,615,74,398,399,400,656,216,43,739,134,428,137,363,626,74,
627,623,418,629,139,1008,628,510,165,217,523,97,59,363,59,449,219,62,439,62,789,
97,221,530,891,637,788,714,401,220,727,74,74,402,403,358,134,896,901,476,74,59,
119,120,742,38,62,726,74,74,74,74,41,304,236,59,516,728,744,666,62,741,481,141,
925,482,245,60,413,60,74,743,926,927,38,597,243,141,363,674,40,41,42,59,996,681,
659,257,62,363,74,446,96,801,418,165,60,427,97,593,97,793,967,968,794,789,964,
664,958,258,60,966,774,969,963,600,384,625,38,311,779,312,882,453,491,41,970,971,
454,79,754,452,319,755,452,617,38,788,60,976,977,930,40,41,42,97,96,731,632,320,
633,717,97,470,385,386,387,388,389,390,391,392,393,394,768,510,615,363,1006,297,
642,643,38,978,979,321,451,338,40,41,42,395,38,358,876,629,344,74,40,41,42,549,
476,962,451,553,478,555,345,483,358,239,1018,667,923,363,476,240,924,476,561,907,
363,567,1022,346,74,695,363,684,59,363,788,699,773,62,701,1027,862,929,347,363,
348,972,973,974,975,906,363,350,59,681,141,756,802,62,865,38,165,363,591,703,399,
400,41,363,807,426,97,367,757,1019,38,427,44,47,48,49,40,41,42,819,47,48,49,60,
135,899,351,470,352,363,359,471,250,950,762,47,48,49,369,892,401,1021,363,60,38,
704,403,980,981,982,363,41,959,43,960,772,371,372,96,814,733,44,310,854,815,942,
38,945,363,47,48,49,40,41,42,649,650,133,38,653,368,657,363,1005,40,41,42,883,97,
363,663,97,363,947,897,446,948,931,247,377,378,427,363,47,48,49,363,933,286,453,
406,937,370,363,955,831,775,265,987,519,520,858,363,451,783,38,451,990,247,411,
363,40,41,42,74,992,74,1020,412,363,121,122,470,438,1013,644,645,451,38,1016,47,
48,49,451,40,41,42,43,416,59,286,47,48,49,62,470,16,17,18,734,735,736,445,47,48,
49,466,603,604,605,455,287,149,150,151,510,692,693,456,337,135,340,340,152,153,
154,155,156,759,760,803,804,354,355,458,74,462,340,340,340,420,59,425,460,60,468,
62,477,97,932,888,889,936,913,914,915,916,479,340,524,921,922,480,287,1014,363,
829,830,484,487,486,488,489,495,497,496,502,265,433,259,765,767,506,522,513,518,
528,43,466,956,286,514,550,551,552,532,60,365,560,540,533,534,97,541,592,554,562,
595,601,594,565,599,602,619,618,620,622,621,634,635,636,790,985,986,654,639,988,
989,641,991,451,585,640,651,652,658,672,660,669,691,675,59,680,586,451,689,62,
451,562,690,686,696,702,697,753,698,706,812,710,705,718,709,39,287,712,721,732,
723,340,737,748,747,825,740,610,340,1009,725,752,1010,763,1011,1012,81,769,770,
771,304,859,861,776,864,74,867,782,778,60,707,791,786,795,873,1023,798,811,805,
808,816,817,818,820,881,340,822,340,340,340,340,340,340,340,340,340,340,340,340,
340,340,340,340,340,824,826,828,832,833,856,82,857,869,247,562,870,879,562,880,
885,884,894,326,898,900,167,84,886,85,86,887,890,41,247,196,895,87,902,905,97,
247,911,88,908,89,90,928,327,328,943,91,944,912,946,951,953,92,954,957,984,935,
1001,134,939,941,961,999,1000,466,1002,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
247,1003,1004,1015,1017,1024,1025,340,1026,1028,247,1030,1031,738,997,22,246,358,
107,247,749,463,147,117,965,316,781,27,995,466,780,598,777,415,0,28,29,688,469,
557,539,855,983,570,0,0,0,32,0,0,730,34,0,0,0,0,994,0,35,36,0,0,0,998,746,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,302,0,0,38,0,562,0,0,39,40,41,42,43,304,340,0,0,0,0,
0,446,0,45,46,0,0,427,0,47,48,49,0,0,0,247,0,0,0,0,0,0,0,0,0,420,0,0,466,247,0,0,
0,792,0,0,0,0,0,0,0,0,466,134,0,800,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,759,760,21,172,173,269,340,270,271,272,273,274,275,276,277,22,81,
23,174,278,0,0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,280,181,0,0,
0,0,0,32,33,182,183,34,184,0,466,0,0,0,35,36,37,0,185,186,340,0,0,0,0,466,0,0,0,
0,0,0,340,0,0,0,0,0,0,265,0,281,84,187,85,86,39,40,41,42,43,282,138,- 300,0,0,0,0,
188,0,45,284,0,0,0,0,190,0,247,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,265,0,0,0,0,0,22,0,0,466,0,0,0,0,0,247,0,0,
27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,0,0,
904,0,0,340,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,302,0,340,0,0,0,0,0,39,40,0,42,43,304,
0,0,0,0,0,0,426,0,45,46,0,0,427,0,47,48,49,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,340,0,340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,
340,340,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,340,0,21,172,173,269,0,270,271,272,273,274,275,276,277,22,81,23,174,278,0,
0,175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,280,181,0,0,0,0,340,32,
33,182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,281,84,187,85,86,39,40,41,42,43,282,138,283,0,0,0,0,188,0,45,284,0,0,0,0,
190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,21,172,173,269,0,270,271,272,273,274,275,276,277,22,81,23,174,278,0,0,
175,24,279,0,0,27,0,0,176,177,178,179,0,28,29,- 36,180,0,280,181,0,0,0,0,0,32,33,
182,183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,281,84,187,85,86,39,40,41,42,43,282,138,0,0,0,0,0,188,0,45,284,0,0,0,0,190,
0,0,191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,0,0,21,172,173,269,0,270,271,272,273,274,275,276,277,22,81,23,174,278,0,0,175,
24,279,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,280,181,0,0,0,0,0,32,33,182,
183,34,184,0,0,0,0,0,35,36,37,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,281,84,187,85,86,39,40,41,42,43,282,138,0,0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,
191,0,0,192,193,0,0,194,195,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,21,172,173,0,0,0,0,0,0,0,0,0,0,22,81,23,174,0,0,0,175,24,0,0,0,27,0,0,176,177,
178,179,0,28,29,0,180,0,0,181,0,0,0,0,0,32,33,182,183,34,184,0,0,0,0,0,35,36,37,
0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,39,40,41,42,
43,655,0,0,0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,
0,0,0,175,0,0,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,0,181,0,0,0,0,0,32,0,
182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,38,84,187,85,86,39,40,41,42,43,0,356,0,0,0,0,0,188,0,45,284,0,0,0,0,190,0,0,
191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,172,173,
0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,0,0,176,177,178,179,0,28,29,
0,180,0,0,181,0,0,0,0,0,32,0,182,183,34,184,0,0,0,0,0,35,36,0,0,185,186,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,39,40,41,42,43,0,0,0,0,0,0,0,
188,0,45,284,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,1,0,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,
683,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,
0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,
40,41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,
0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,
0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,
42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,
25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,
36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,42,43,
0,0,0,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,
0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,
0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,38,0,0,0,0,39,40,41,42,43,
22,0,- 11,0,0,0,0,44,0,45,46,0,27,0,0,47,48,49,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,0,
0,0,34,0,0,0,0,0,0,35,36,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
21,0,0,38,0,0,0,0,39,40,41,42,43,22,0,23,0,0,0,0,470,24,45,46,0,27,0,0,47,48,49,
0,0,28,29,- 36,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,2,3,4,77,6,7,8,
9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,138,23,
0,259,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,0,32,33,0,0,34,0,
0,0,0,0,0,35,36,37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,
0,0,0,0,0,39,40,0,42,43,22,138,0,0,0,0,0,0,24,45,46,81,27,0,0,0,0,0,0,0,28,29,0,
0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,0,0,0,0,0,0,82,0,0,0,0,0,0,0,0,22,39,40,687,42,43,167,84,0,85,86,0,27,41,0,
45,46,87,0,0,28,29,0,88,0,89,90,0,327,328,0,91,32,0,0,0,34,92,0,0,0,0,0,35,36,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,302,0,0,303,0,0,0,22,39,40,
0,42,43,304,0,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,224,0,0,0,0,0,0,0,32,0,0,0,34,
0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,22,39,40,0,42,43,0,225,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,0,0,0,0,0,0,0,
0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,
0,0,0,302,0,0,0,0,834,835,22,39,40,0,42,43,304,0,0,0,0,81,27,0,0,45,46,0,0,0,28,
29,0,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,0,0,0,0,0,82,0,676,0,0,0,0,0,0,22,39,40,0,42,43,167,84,0,85,86,81,
27,41,0,45,46,87,0,0,28,29,0,836,0,89,90,0,0,0,0,91,32,0,837,0,34,92,193,0,0,194,
195,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,82,0,0,0,0,0,0,0,0,
22,39,40,407,42,43,167,84,0,85,86,0,27,41,0,45,46,87,0,0,28,29,0,88,0,89,90,0,0,
0,0,91,32,0,0,0,34,92,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,0,0,172,173,269,0,270,
271,272,273,274,275,276,277,0,81,0,174,278,0,39,175,0,279,43,0,0,0,0,176,177,178,
179,0,45,46,0,180,0,280,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,
186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,281,84,187,85,86,0,0,41,0,0,282,
138,0,0,172,173,0,188,0,0,189,544,0,0,0,190,0,81,191,174,0,192,193,175,0,194,195,
0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,
0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,
41,0,0,0,341,545,0,172,173,0,188,0,0,189,0,327,328,0,190,0,81,191,174,0,192,193,
175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,
0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,85,86,0,0,41,0,0,0,341,787,0,172,173,0,188,0,0,189,0,327,328,0,190,0,81,191,
174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,
0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,341,952,0,172,173,0,188,0,0,189,0,327,328,0,
190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,341,0,0,172,173,0,188,0,0,189,0,
327,328,0,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,
0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,
0,189,0,0,0,309,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,
0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,341,0,0,172,173,
0,188,0,0,189,0,0,0,0,190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,
178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,81,0,185,
186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,490,172,
173,0,0,0,0,188,0,0,189,0,0,0,81,190,174,0,191,82,175,192,193,0,0,194,195,0,176,
177,178,179,0,0,83,84,180,85,86,181,0,41,0,0,0,87,0,182,183,0,184,88,0,89,90,0,0,
0,81,91,185,186,0,0,0,92,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,
41,0,0,0,172,173,0,0,0,0,188,588,0,189,0,0,0,81,190,174,0,191,82,175,192,193,0,0,
194,195,0,176,177,178,179,0,0,167,84,180,85,86,181,0,41,0,0,0,87,0,182,183,0,184,
88,0,89,90,0,0,0,0,91,185,186,0,0,0,92,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,0,189,0,0,0,609,190,0,81,191,174,0,
192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,0,
182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,661,0,189,0,0,0,81,190,174,0,
191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,0,0,0,0,
0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,38,84,187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,0,189,0,0,0,729,190,0,81,
191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,181,0,0,
0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,0,0,0,172,173,0,188,0,0,189,0,0,0,745,
190,0,81,191,174,0,192,193,175,0,194,195,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,764,172,173,0,0,0,0,188,0,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,766,172,173,0,0,0,0,188,0,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,860,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,863,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,866,172,173,0,0,0,0,188,0,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,934,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,938,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,940,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,0,0,0,0,0,182,183,0,184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,38,84,187,85,86,0,0,41,0,0,0,172,173,0,0,0,0,188,993,0,189,0,
0,0,81,190,174,0,191,0,175,192,193,0,0,194,195,0,176,177,178,179,0,0,0,0,180,0,0,
181,0,0,172,173,0,0,0,182,183,0,184,0,0,0,0,81,0,0,0,0,185,186,0,0,0,0,0,0,0,176,
177,178,179,0,0,0,0,0,0,0,181,0,0,38,84,187,85,86,182,183,41,184,0,0,0,0,0,0,0,0,
188,185,186,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,38,84,187,85,
86,0,0,41,0,0,0,341,0,172,173,0,0,188,0,0,189,0,0,0,0,190,81,0,191,0,0,192,193,0,
0,194,195,0,0,0,176,177,178,179,0,0,0,0,0,0,0,181,0,0,172,173,0,0,0,182,183,0,
184,0,0,0,0,81,0,0,0,0,185,186,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,0,0,0,181,0,
0,38,84,187,85,86,182,183,41,184,0,0,707,0,0,0,0,0,188,185,186,189,0,0,0,0,190,0,
0,191,0,0,192,193,0,0,194,195,0,0,0,0,38,84,187,85,86,0,0,41,0,0,172,173,0,0,0,0,
0,336,0,0,189,0,0,81,0,190,0,0,191,0,0,192,193,0,0,194,195,176,177,178,179,0,0,0,
0,0,0,0,181,0,0,172,173,0,0,0,182,183,0,184,0,0,0,0,81,0,0,0,0,185,186,0,0,0,0,0,
0,0,176,177,178,179,0,0,0,0,0,0,0,181,0,0,38,84,187,85,86,182,183,41,184,0,0,0,0,
0,0,0,0,353,185,186,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,38,
84,187,85,86,0,0,41,0,0,172,173,0,0,0,0,0,188,0,0,189,0,0,81,0,190,0,0,191,0,0,
192,193,0,0,194,195,176,177,178,179,0,0,0,0,0,0,0,181,81,0,0,0,0,0,0,182,183,0,
184,0,0,0,0,0,0,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,
187,85,86,0,0,41,0,0,0,82,0,0,0,0,0,903,0,0,189,0,0,716,0,190,167,84,191,85,86,
192,193,41,0,194,195,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92};static
short Cyc_yycheck[6372]={0,174,341,368,65,0,144,365,114,190,191,192,367,433,370,
285,286,68,288,23,144,21,175,128,23,175,21,371,372,327,211,109,166,144,188,328,
751,259,509,159,377,378,453,21,44,593,107,107,247,401,117,51,449,120,114,0,51,
227,58,59,60,17,62,58,59,60,135,62,68,69,139,93,68,68,379,380,21,128,82,476,58,
59,60,82,62,71,356,138,92,140,68,5,6,92,514,157,507,107,813,125,398,517,102,103,
402,516,51,102,103,135,428,65,134,58,59,60,122,62,105,119,120,121,122,68,119,120,
121,122,128,122,601,449,128,128,373,374,375,376,138,44,140,61,494,138,140,140,
133,147,321,107,128,107,57,107,107,328,117,102,103,707,138,161,140,225,122,169,
161,171,120,122,169,229,87,122,119,120,121,122,471,131,522,93,336,128,338,319,
247,51,118,247,117,607,112,138,124,140,307,351,118,353,381,382,383,120,265,119,
111,232,123,260,235,128,216,87,107,122,161,216,124,223,135,114,134,125,223,87,
642,227,88,89,125,124,227,135,285,286,141,288,102,103,135,49,304,229,100,101,120,
446,104,123,118,421,422,423,593,119,120,121,122,107,260,247,122,123,260,260,38,
609,87,117,112,470,124,106,136,137,301,125,130,265,128,116,260,124,126,285,286,
122,288,124,285,286,317,288,120,130,107,161,471,651,106,120,128,114,123,356,117,
285,286,124,288,112,728,124,314,260,135,733,119,181,122,132,133,134,120,329,548,
743,272,125,329,88,89,112,278,107,108,109,110,111,285,286,114,288,122,20,21,126,
641,107,117,129,124,258,693,127,356,124,774,117,222,356,120,779,122,122,123,116,
707,780,112,433,59,122,0,124,238,119,356,560,88,130,287,725,107,446,120,729,449,
118,113,497,690,132,128,500,22,23,106,25,554,140,141,745,109,703,704,411,121,470,
414,356,411,107,108,476,129,130,44,113,769,421,422,423,623,51,421,422,423,118,
430,57,83,84,85,486,124,116,637,65,426,67,120,122,70,124,446,509,448,500,128,130,
514,79,107,311,82,457,120,459,446,117,457,125,459,804,92,121,641,811,470,693,592,
124,120,613,102,103,129,130,188,107,824,832,470,111,486,117,118,627,107,486,612,
119,120,121,122,114,117,112,500,120,613,627,500,500,626,124,486,132,127,125,457,
221,459,141,627,140,141,107,117,124,500,120,944,113,114,115,528,945,525,117,117,
528,120,161,124,541,711,601,166,486,130,169,406,171,699,912,913,699,889,909,496,
902,120,500,911,668,914,908,416,56,623,107,124,676,124,789,120,277,114,915,916,
125,118,117,637,120,120,640,436,107,804,528,921,922,856,113,114,115,216,595,617,
457,119,459,595,223,124,94,95,96,97,98,99,100,101,102,103,117,672,611,120,962,
665,477,478,107,923,924,121,247,124,113,114,115,121,107,336,119,630,124,258,113,
114,115,344,623,908,265,348,267,350,124,270,353,107,108,504,135,120,637,113,139,
640,125,837,120,366,1008,124,287,125,120,528,665,120,889,125,665,665,125,1021,
117,855,124,120,124,917,918,919,920,836,120,124,685,681,665,125,714,685,117,107,
319,120,403,83,84,85,114,120,726,124,329,92,125,1004,107,130,124,132,133,134,113,
114,115,741,132,133,134,665,782,828,124,124,124,120,124,128,115,884,125,132,133,
134,137,117,124,1008,120,685,107,129,130,925,926,927,120,114,903,116,905,125,90,
91,759,120,619,124,145,759,125,117,107,875,120,132,133,134,113,114,115,480,481,
118,107,484,136,486,120,961,113,114,115,125,411,120,495,414,120,117,125,124,120,
125,114,86,87,130,120,132,133,134,120,125,856,120,118,125,138,120,125,756,669,
133,125,64,65,762,120,446,685,107,449,125,144,107,120,113,114,115,457,125,459,
1007,107,120,117,118,124,123,125,5,6,470,107,1000,132,133,134,476,113,114,115,
116,124,856,908,132,133,134,856,124,17,18,19,107,108,109,125,132,133,134,259,421,
422,423,119,856,64,65,66,944,119,120,119,172,948,174,175,75,76,77,78,79,22,23,
119,120,185,186,119,528,112,190,191,192,226,908,228,121,856,119,908,118,541,860,
119,120,863,137,138,90,91,118,211,312,86,87,124,908,119,120,754,755,124,107,124,
117,117,124,124,128,119,263,135,121,655,656,107,131,125,125,119,116,341,900,1008,
128,345,346,347,119,908,93,125,119,123,123,595,120,122,124,359,120,120,125,363,
125,119,118,107,124,120,125,107,117,123,696,933,934,30,117,937,938,128,940,623,
384,120,124,118,117,135,42,124,107,125,1008,125,396,637,125,1008,640,401,120,131,
120,125,120,647,120,119,732,120,125,119,125,112,1008,125,125,121,125,321,125,107,
124,747,125,427,328,987,131,119,990,124,992,993,38,118,117,125,117,763,764,119,
766,685,768,118,123,1008,118,25,122,125,112,1013,125,131,125,125,125,125,125,125,
786,365,125,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,
131,117,124,118,125,128,92,119,117,426,494,117,125,497,125,120,124,108,104,119,
28,107,108,125,110,111,125,125,114,446,836,125,118,118,124,759,453,92,124,128,
126,127,92,129,130,119,132,120,136,119,25,119,138,120,119,117,862,108,782,865,
866,125,125,124,548,119,8,9,10,11,12,13,14,15,16,17,18,19,20,21,497,125,120,125,
125,125,125,471,125,125,507,0,0,622,948,37,113,903,35,516,640,258,70,48,910,158,
681,49,944,593,676,414,672,223,- 1,57,58,541,263,352,330,759,928,369,- 1,- 1,- 1,69,
- 1,- 1,614,73,- 1,- 1,- 1,- 1,942,- 1,80,81,- 1,- 1,- 1,949,628,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,- 1,- 1,107,- 1,651,- 1,- 1,112,113,114,115,116,117,560,
- 1,- 1,- 1,- 1,- 1,124,- 1,126,127,- 1,- 1,130,- 1,132,133,134,- 1,- 1,- 1,613,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,623,- 1,- 1,693,627,- 1,- 1,- 1,698,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,707,948,- 1,
710,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,
26,27,641,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,
52,53,54,55,- 1,57,58,- 1,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,70,71,72,73,74,- 1,789,- 1,-
1,- 1,80,81,82,- 1,84,85,699,- 1,- 1,- 1,- 1,804,- 1,- 1,- 1,- 1,- 1,- 1,711,- 1,- 1,- 1,- 1,- 1,
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
static int Cyc_yynerrs=0;struct _tuple24{struct Cyc_Position_Segment*f1;struct
_tuple1*f2;int f3;};struct _tuple25{struct _dyneither_ptr f1;void*f2;};static char
_tmp4A8[8]="stdcall";static char _tmp4A9[6]="cdecl";static char _tmp4AA[9]="fastcall";
static char _tmp4AB[9]="noreturn";static char _tmp4AC[6]="const";static char _tmp4AD[8]="aligned";
static char _tmp4AE[7]="packed";static char _tmp4AF[7]="shared";static char _tmp4B0[7]="unused";
static char _tmp4B1[5]="weak";static char _tmp4B2[10]="dllimport";static char _tmp4B3[
10]="dllexport";static char _tmp4B4[23]="no_instrument_function";static char _tmp4B5[
12]="constructor";static char _tmp4B6[11]="destructor";static char _tmp4B7[22]="no_check_memory_usage";
static char _tmp4B8[5]="pure";struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp939(
unsigned int*_tmp938,unsigned int*_tmp937,short**_tmp936){for(*_tmp938=0;*
_tmp938 < *_tmp937;(*_tmp938)++){(*_tmp936)[*_tmp938]=(short)0;}}static void
_tmp93E(unsigned int*_tmp93D,unsigned int*_tmp93C,union Cyc_YYSTYPE**_tmp93B){
for(*_tmp93D=0;*_tmp93D < *_tmp93C;(*_tmp93D)++){(*_tmp93B)[*_tmp93D]=Cyc_yylval;}}
static void _tmp943(unsigned int*_tmp942,unsigned int*_tmp941,struct Cyc_Yyltype**
_tmp940){for(*_tmp942=0;*_tmp942 < *_tmp941;(*_tmp942)++){(*_tmp940)[*_tmp942]=
Cyc_yynewloc();}}static void _tmpE38(unsigned int*_tmpE37,unsigned int*_tmpE36,
char**_tmpE34){for(*_tmpE37=0;*_tmpE37 < *_tmpE36;(*_tmpE37)++){(*_tmpE34)[*
_tmpE37]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct
_RegionHandle _tmp40E=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp40E;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;unsigned int _tmp938;unsigned int _tmp937;short*_tmp936;
unsigned int _tmp935;short*yyss=(short*)((_tmp935=10000,((_tmp936=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp935)),((((_tmp937=_tmp935,
_tmp939(& _tmp938,& _tmp937,& _tmp936))),_tmp936))))));int yyvsp_offset;unsigned int
_tmp93D;unsigned int _tmp93C;union Cyc_YYSTYPE*_tmp93B;unsigned int _tmp93A;union
Cyc_YYSTYPE*yyvs=(union Cyc_YYSTYPE*)((_tmp93A=10000,((_tmp93B=(union Cyc_YYSTYPE*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp93A)),((((
_tmp93C=_tmp93A,_tmp93E(& _tmp93D,& _tmp93C,& _tmp93B))),_tmp93B))))));int
yylsp_offset;unsigned int _tmp942;unsigned int _tmp941;struct Cyc_Yyltype*_tmp940;
unsigned int _tmp93F;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmp93F=10000,((
_tmp940=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct
Cyc_Yyltype),_tmp93F)),((((_tmp941=_tmp93F,_tmp943(& _tmp942,& _tmp941,& _tmp940))),
_tmp940))))));int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){{const char*_tmp944;Cyc_yyerror(((_tmp944="parser stack overflow",
_tag_dyneither(_tmp944,sizeof(char),22))));}(int)_throw((void*)& Cyc_Yystack_overflow_val);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 371?(int)Cyc_yytranslate[_check_known_subscript_notnull(372,Cyc_yychar)]:
287;}yyn +=yychar1;if((yyn < 0  || yyn > 6371) || Cyc_yycheck[
_check_known_subscript_notnull(6372,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6372,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1031){int
_tmp410=0;_npop_handler(0);return _tmp410;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1032,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(510,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL1F7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL1F8: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp411;_push_handler(& _tmp411);{int _tmp413=0;if(setjmp(_tmp411.handler))_tmp413=
1;if(!_tmp413){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp412=(void*)_exn_thrown;void*
_tmp415=_tmp412;_LL1FB: {struct Cyc_Core_Failure_struct*_tmp416=(struct Cyc_Core_Failure_struct*)
_tmp415;if(_tmp416->tag != Cyc_Core_Failure)goto _LL1FD;}_LL1FC: x=0;goto _LL1FA;
_LL1FD:;_LL1FE:(void)_throw(_tmp415);_LL1FA:;}}}{struct _handler_cons _tmp417;
_push_handler(& _tmp417);{int _tmp419=0;if(setjmp(_tmp417.handler))_tmp419=1;if(!
_tmp419){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp418=(void*)_exn_thrown;void*_tmp41B=_tmp418;_LL200: {
struct Cyc_Core_Failure_struct*_tmp41C=(struct Cyc_Core_Failure_struct*)_tmp41B;
if(_tmp41C->tag != Cyc_Core_Failure)goto _LL202;}_LL201: y=0;goto _LL1FF;_LL202:;
_LL203:(void)_throw(_tmp41B);_LL1FF:;}}}yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));break;}
case 3: _LL1F9:{struct Cyc_Absyn_Decl*_tmp94E;struct Cyc_Absyn_Using_d_struct _tmp94D;
struct Cyc_Absyn_Using_d_struct*_tmp94C;struct Cyc_List_List*_tmp94B;yyval=Cyc_YY18(((
_tmp94B=_cycalloc(sizeof(*_tmp94B)),((_tmp94B->hd=((_tmp94E=_cycalloc(sizeof(*
_tmp94E)),((_tmp94E->r=(void*)((_tmp94C=_cycalloc(sizeof(*_tmp94C)),((_tmp94C[0]=((
_tmp94D.tag=11,((_tmp94D.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp94D.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp94D)))))),_tmp94C)))),((
_tmp94E->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmp94E)))))),((_tmp94B->tl=0,_tmp94B)))))));}Cyc_Lex_leave_using();
break;case 4: _LL204:{struct Cyc_Absyn_Decl*_tmp958;struct Cyc_Absyn_Using_d_struct
_tmp957;struct Cyc_Absyn_Using_d_struct*_tmp956;struct Cyc_List_List*_tmp955;yyval=
Cyc_YY18(((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955->hd=((_tmp958=_cycalloc(
sizeof(*_tmp958)),((_tmp958->r=(void*)((_tmp956=_cycalloc(sizeof(*_tmp956)),((
_tmp956[0]=((_tmp957.tag=11,((_tmp957.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp957.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp957)))))),
_tmp956)))),((_tmp958->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp958)))))),((
_tmp955->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp955)))))));}break;case 5: _LL205:{struct Cyc_Absyn_Decl*_tmp967;
struct Cyc_Absyn_Namespace_d_struct _tmp966;struct _dyneither_ptr*_tmp965;struct Cyc_Absyn_Namespace_d_struct*
_tmp964;struct Cyc_List_List*_tmp963;yyval=Cyc_YY18(((_tmp963=_cycalloc(sizeof(*
_tmp963)),((_tmp963->hd=((_tmp967=_cycalloc(sizeof(*_tmp967)),((_tmp967->r=(void*)((
_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964[0]=((_tmp966.tag=10,((_tmp966.f1=((
_tmp965=_cycalloc(sizeof(*_tmp965)),((_tmp965[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp965)))),((_tmp966.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp966)))))),
_tmp964)))),((_tmp967->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp967)))))),((
_tmp963->tl=0,_tmp963)))))));}Cyc_Lex_leave_namespace();break;case 6: _LL206: {
struct _dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp42A;_push_handler(& _tmp42A);{int _tmp42C=0;if(setjmp(_tmp42A.handler))
_tmp42C=1;if(!_tmp42C){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp42B=(void*)_exn_thrown;void*_tmp42E=_tmp42B;_LL209: {struct Cyc_Core_Failure_struct*
_tmp42F=(struct Cyc_Core_Failure_struct*)_tmp42E;if(_tmp42F->tag != Cyc_Core_Failure)
goto _LL20B;}_LL20A:{const char*_tmp968;nspace=((_tmp968="",_tag_dyneither(_tmp968,
sizeof(char),1)));}goto _LL208;_LL20B:;_LL20C:(void)_throw(_tmp42E);_LL208:;}}}{
struct _handler_cons _tmp431;_push_handler(& _tmp431);{int _tmp433=0;if(setjmp(
_tmp431.handler))_tmp433=1;if(!_tmp433){x=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}else{
void*_tmp432=(void*)_exn_thrown;void*_tmp435=_tmp432;_LL20E: {struct Cyc_Core_Failure_struct*
_tmp436=(struct Cyc_Core_Failure_struct*)_tmp435;if(_tmp436->tag != Cyc_Core_Failure)
goto _LL210;}_LL20F: x=0;goto _LL20D;_LL210:;_LL211:(void)_throw(_tmp435);_LL20D:;}}}{
struct _handler_cons _tmp437;_push_handler(& _tmp437);{int _tmp439=0;if(setjmp(
_tmp437.handler))_tmp439=1;if(!_tmp439){y=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{void*
_tmp438=(void*)_exn_thrown;void*_tmp43B=_tmp438;_LL213: {struct Cyc_Core_Failure_struct*
_tmp43C=(struct Cyc_Core_Failure_struct*)_tmp43B;if(_tmp43C->tag != Cyc_Core_Failure)
goto _LL215;}_LL214: y=0;goto _LL212;_LL215:;_LL216:(void)_throw(_tmp43B);_LL212:;}}}{
struct Cyc_Absyn_Decl*_tmp977;struct Cyc_Absyn_Namespace_d_struct _tmp976;struct
_dyneither_ptr*_tmp975;struct Cyc_Absyn_Namespace_d_struct*_tmp974;struct Cyc_List_List*
_tmp973;yyval=Cyc_YY18(((_tmp973=_cycalloc(sizeof(*_tmp973)),((_tmp973->hd=((
_tmp977=_cycalloc(sizeof(*_tmp977)),((_tmp977->r=(void*)((_tmp974=_cycalloc(
sizeof(*_tmp974)),((_tmp974[0]=((_tmp976.tag=10,((_tmp976.f1=((_tmp975=_cycalloc(
sizeof(*_tmp975)),((_tmp975[0]=nspace,_tmp975)))),((_tmp976.f2=x,_tmp976)))))),
_tmp974)))),((_tmp977->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp977)))))),((
_tmp973->tl=y,_tmp973)))))));}break;}case 7: _LL207:{const char*_tmp978;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmp978="C",_tag_dyneither(_tmp978,sizeof(char),2))))
== 0){struct Cyc_Absyn_Decl*_tmp982;struct Cyc_Absyn_ExternC_d_struct _tmp981;
struct Cyc_Absyn_ExternC_d_struct*_tmp980;struct Cyc_List_List*_tmp97F;yyval=Cyc_YY18(((
_tmp97F=_cycalloc(sizeof(*_tmp97F)),((_tmp97F->hd=((_tmp982=_cycalloc(sizeof(*
_tmp982)),((_tmp982->r=(void*)((_tmp980=_cycalloc(sizeof(*_tmp980)),((_tmp980[0]=((
_tmp981.tag=12,((_tmp981.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmp981)))),_tmp980)))),((_tmp982->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmp982)))))),((
_tmp97F->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp97F)))))));}else{{const char*_tmp983;if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),((_tmp983="C include",_tag_dyneither(_tmp983,sizeof(char),10))))
!= 0){const char*_tmp984;Cyc_Parse_err(((_tmp984="expecting \"C\" or \"C include\"",
_tag_dyneither(_tmp984,sizeof(char),29))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_Absyn_Decl*
_tmp98E;struct Cyc_Absyn_ExternCinclude_d_struct _tmp98D;struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp98C;struct Cyc_List_List*_tmp98B;yyval=Cyc_YY18(((_tmp98B=_cycalloc(sizeof(*
_tmp98B)),((_tmp98B->hd=((_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E->r=(void*)((
_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C[0]=((_tmp98D.tag=13,((_tmp98D.f1=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmp98D.f2=0,_tmp98D)))))),_tmp98C)))),((_tmp98E->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp98E)))))),((
_tmp98B->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp98B)))))));}}}break;case 8: _LL217:{const char*_tmp98F;if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmp98F="C include",_tag_dyneither(_tmp98F,sizeof(
char),10))))!= 0){const char*_tmp990;Cyc_Parse_err(((_tmp990="expecting \"C include\"",
_tag_dyneither(_tmp990,sizeof(char),22))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Decl*_tmp99A;struct Cyc_Absyn_ExternCinclude_d_struct _tmp999;
struct Cyc_Absyn_ExternCinclude_d_struct*_tmp998;struct Cyc_List_List*_tmp997;
yyval=Cyc_YY18(((_tmp997=_cycalloc(sizeof(*_tmp997)),((_tmp997->hd=((_tmp99A=
_cycalloc(sizeof(*_tmp99A)),((_tmp99A->r=(void*)((_tmp998=_cycalloc(sizeof(*
_tmp998)),((_tmp998[0]=((_tmp999.tag=13,((_tmp999.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp999.f2=exs,_tmp999)))))),
_tmp998)))),((_tmp99A->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp99A)))))),((
_tmp997->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp997)))))));}break;}case 9: _LL218:{struct Cyc_Absyn_Decl*
_tmp99D;struct Cyc_List_List*_tmp99C;yyval=Cyc_YY18(((_tmp99C=_cycalloc(sizeof(*
_tmp99C)),((_tmp99C->hd=((_tmp99D=_cycalloc(sizeof(*_tmp99D)),((_tmp99D->r=(void*)&
Cyc_Absyn_Porton_d_val,((_tmp99D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmp99D)))))),((
_tmp99C->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp99C)))))));}break;case 10: _LL219:{struct Cyc_Absyn_Decl*
_tmp9A0;struct Cyc_List_List*_tmp99F;yyval=Cyc_YY18(((_tmp99F=_cycalloc(sizeof(*
_tmp99F)),((_tmp99F->hd=((_tmp9A0=_cycalloc(sizeof(*_tmp9A0)),((_tmp9A0->r=(void*)&
Cyc_Absyn_Portoff_d_val,((_tmp9A0->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmp9A0)))))),((
_tmp99F->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp99F)))))));}break;case 11: _LL21A: yyval=Cyc_YY18(0);break;case
12: _LL21B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 13: _LL21C: yyval=Cyc_YY52(0);break;case 14: _LL21D:{struct _tuple24*
_tmp9A3;struct Cyc_List_List*_tmp9A2;yyval=Cyc_YY52(((_tmp9A2=_cycalloc(sizeof(*
_tmp9A2)),((_tmp9A2->hd=((_tmp9A3=_cycalloc(sizeof(*_tmp9A3)),((_tmp9A3->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmp9A3->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmp9A3->f3=0,_tmp9A3)))))))),((_tmp9A2->tl=0,_tmp9A2)))))));}break;case 15:
_LL21E:{struct _tuple24*_tmp9A6;struct Cyc_List_List*_tmp9A5;yyval=Cyc_YY52(((
_tmp9A5=_cycalloc(sizeof(*_tmp9A5)),((_tmp9A5->hd=((_tmp9A6=_cycalloc(sizeof(*
_tmp9A6)),((_tmp9A6->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmp9A6->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmp9A6->f3=0,_tmp9A6)))))))),((_tmp9A5->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp9A5)))))));}break;case
16: _LL21F:{struct Cyc_Absyn_Fn_d_struct*_tmp9AC;struct Cyc_Absyn_Fn_d_struct
_tmp9AB;struct Cyc_List_List*_tmp9AA;yyval=Cyc_YY18(((_tmp9AA=_cycalloc(sizeof(*
_tmp9AA)),((_tmp9AA->hd=Cyc_Absyn_new_decl((void*)((_tmp9AC=_cycalloc(sizeof(*
_tmp9AC)),((_tmp9AC[0]=((_tmp9AB.tag=1,((_tmp9AB.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp9AB)))),_tmp9AC)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp9AA->tl=0,
_tmp9AA)))))));}break;case 17: _LL220: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 18: _LL221: yyval=Cyc_YY18(0);break;case 19: _LL222:
yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 20:
_LL223:{struct Cyc_Core_Opt*_tmp9AD;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmp9AD=_cycalloc(sizeof(*_tmp9AD)),((_tmp9AD->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp9AD)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 21:
_LL224: yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 22:
_LL225:{struct Cyc_Core_Opt*_tmp9AE;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmp9AE=_cycalloc(sizeof(*_tmp9AE)),((_tmp9AE->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp9AE)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 23:
_LL226:{struct Cyc_Core_Opt*_tmp9AF;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp9AF)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 24:
_LL227:{struct Cyc_Core_Opt*_tmp9B0;yyval=Cyc_YY17(Cyc_Parse_make_function(((
_tmp9B0=_cycalloc(sizeof(*_tmp9B0)),((_tmp9B0->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp9B0)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 25:
_LL228: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL229: Cyc_Lex_leave_using();
break;case 27: _LL22A:{struct _dyneither_ptr*_tmp9B1;Cyc_Lex_enter_namespace(((
_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp9B1)))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL22B: Cyc_Lex_leave_namespace();
break;case 29: _LL22C: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
30: _LL22D: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 31:
_LL22E:{struct Cyc_List_List*_tmp9B2;yyval=Cyc_YY18(((_tmp9B2=_cycalloc(sizeof(*
_tmp9B2)),((_tmp9B2->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp9B2->tl=0,
_tmp9B2)))))));}break;case 32: _LL22F: {struct Cyc_List_List*_tmp464=0;{struct Cyc_List_List*
_tmp465=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp465 != 0;_tmp465=_tmp465->tl){struct _dyneither_ptr*_tmp466=(struct
_dyneither_ptr*)_tmp465->hd;struct _tuple1*_tmp9B3;struct _tuple1*qv=(_tmp9B3=
_cycalloc(sizeof(*_tmp9B3)),((_tmp9B3->f1=Cyc_Absyn_Rel_n(0),((_tmp9B3->f2=
_tmp466,_tmp9B3)))));struct Cyc_Absyn_Vardecl*_tmp467=Cyc_Absyn_new_vardecl(qv,
Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*_tmp9B4;_tmp464=((_tmp9B4=_cycalloc(
sizeof(*_tmp9B4)),((_tmp9B4->hd=_tmp467,((_tmp9B4->tl=_tmp464,_tmp9B4))))));}}
_tmp464=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp464);{struct Cyc_List_List*_tmp9B5;yyval=Cyc_YY18(((_tmp9B5=_cycalloc(sizeof(*
_tmp9B5)),((_tmp9B5->hd=Cyc_Absyn_letv_decl(_tmp464,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp9B5->tl=0,
_tmp9B5)))))));}break;}case 33: _LL230:{const char*_tmp9B6;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmp9B6="`H",_tag_dyneither(_tmp9B6,sizeof(char),3))))== 0){
const char*_tmp9B9;void*_tmp9B8;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9B8=0,
Cyc_aprintf(((_tmp9B9="bad occurrence of heap region",_tag_dyneither(_tmp9B9,
sizeof(char),30))),_tag_dyneither(_tmp9B8,sizeof(void*),0)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{const char*
_tmp9BA;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp9BA="`U",
_tag_dyneither(_tmp9BA,sizeof(char),3))))== 0){const char*_tmp9BD;void*_tmp9BC;
Cyc_Parse_err((struct _dyneither_ptr)((_tmp9BC=0,Cyc_aprintf(((_tmp9BD="bad occurrence of unique region",
_tag_dyneither(_tmp9BD,sizeof(char),32))),_tag_dyneither(_tmp9BC,sizeof(void*),0)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{struct _dyneither_ptr*_tmp9C0;struct Cyc_Absyn_Tvar*
_tmp9BF;struct Cyc_Absyn_Tvar*tv=(_tmp9BF=_cycalloc(sizeof(*_tmp9BF)),((_tmp9BF->name=((
_tmp9C0=_cycalloc(sizeof(*_tmp9C0)),((_tmp9C0[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp9C0)))),((_tmp9BF->identity=
- 1,((_tmp9BF->kind=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_RgnKind),_tmp9BF)))))));
struct Cyc_Absyn_VarType_struct _tmp9C3;struct Cyc_Absyn_VarType_struct*_tmp9C2;
void*t=(void*)((_tmp9C2=_cycalloc(sizeof(*_tmp9C2)),((_tmp9C2[0]=((_tmp9C3.tag=2,((
_tmp9C3.f1=tv,_tmp9C3)))),_tmp9C2))));struct Cyc_Absyn_RgnHandleType_struct
_tmp9C9;struct Cyc_Absyn_RgnHandleType_struct*_tmp9C8;struct _dyneither_ptr*
_tmp9C6;struct _tuple1*_tmp9C5;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmp9C5=_cycalloc(sizeof(*_tmp9C5)),((_tmp9C5->f1=Cyc_Absyn_Loc_n,((_tmp9C5->f2=((
_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp9C6)))),_tmp9C5)))))),(
void*)((_tmp9C8=_cycalloc(sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9C9.tag=16,((
_tmp9C9.f1=(void*)t,_tmp9C9)))),_tmp9C8)))),0);{struct Cyc_List_List*_tmp9CA;
yyval=Cyc_YY18(((_tmp9CA=_cycalloc(sizeof(*_tmp9CA)),((_tmp9CA->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmp9CA->tl=0,_tmp9CA)))))));}break;}case 34: _LL231:{
const char*_tmp9CB;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp9CB="H",
_tag_dyneither(_tmp9CB,sizeof(char),2))))== 0){const char*_tmp9CC;Cyc_Parse_err(((
_tmp9CC="bad occurrence of heap region `H",_tag_dyneither(_tmp9CC,sizeof(char),
33))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}{const char*_tmp9CD;if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmp9CD="U",_tag_dyneither(_tmp9CD,sizeof(char),2))))== 0){
const char*_tmp9CE;Cyc_Parse_err(((_tmp9CE="bad occurrence of unique region `U",
_tag_dyneither(_tmp9CE,sizeof(char),35))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));}}if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){const char*
_tmp9CF;Cyc_Parse_err(((_tmp9CF="open regions cannot be @resetable",
_tag_dyneither(_tmp9CF,sizeof(char),34))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{struct
_dyneither_ptr*_tmp9DC;const char*_tmp9DB;void*_tmp9DA[1];struct Cyc_String_pa_struct
_tmp9D9;struct Cyc_Absyn_Tvar*_tmp9D8;struct Cyc_Absyn_Tvar*tv=(_tmp9D8=_cycalloc(
sizeof(*_tmp9D8)),((_tmp9D8->name=((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((
_tmp9DC[0]=(struct _dyneither_ptr)((_tmp9D9.tag=0,((_tmp9D9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmp9DA[0]=& _tmp9D9,
Cyc_aprintf(((_tmp9DB="`%s",_tag_dyneither(_tmp9DB,sizeof(char),4))),
_tag_dyneither(_tmp9DA,sizeof(void*),1)))))))),_tmp9DC)))),((_tmp9D8->identity=-
1,((_tmp9D8->kind=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_RgnKind),_tmp9D8)))))));
struct Cyc_Absyn_VarType_struct _tmp9DF;struct Cyc_Absyn_VarType_struct*_tmp9DE;
void*t=(void*)((_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE[0]=((_tmp9DF.tag=2,((
_tmp9DF.f1=tv,_tmp9DF)))),_tmp9DE))));struct Cyc_Absyn_RgnHandleType_struct
_tmp9E5;struct Cyc_Absyn_RgnHandleType_struct*_tmp9E4;struct _dyneither_ptr*
_tmp9E2;struct _tuple1*_tmp9E1;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((
_tmp9E1=_cycalloc(sizeof(*_tmp9E1)),((_tmp9E1->f1=Cyc_Absyn_Loc_n,((_tmp9E1->f2=((
_tmp9E2=_cycalloc(sizeof(*_tmp9E2)),((_tmp9E2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp9E2)))),_tmp9E1)))))),(
void*)((_tmp9E4=_cycalloc(sizeof(*_tmp9E4)),((_tmp9E4[0]=((_tmp9E5.tag=16,((
_tmp9E5.f1=(void*)t,_tmp9E5)))),_tmp9E4)))),0);{struct Cyc_List_List*_tmp9E6;
yyval=Cyc_YY18(((_tmp9E6=_cycalloc(sizeof(*_tmp9E6)),((_tmp9E6->hd=Cyc_Absyn_region_decl(
tv,vd,Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp9E6->tl=0,
_tmp9E6)))))));}break;}case 35: _LL232: {struct _dyneither_ptr*_tmp9EE;struct Cyc_Absyn_Eq_kb_struct*
_tmp9ED;struct Cyc_Absyn_Eq_kb_struct _tmp9EC;struct Cyc_Absyn_Tvar*_tmp9EB;struct
Cyc_Absyn_Tvar*tv=(_tmp9EB=_cycalloc(sizeof(*_tmp9EB)),((_tmp9EB->name=((_tmp9EE=
_cycalloc(sizeof(*_tmp9EE)),((_tmp9EE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmp9EE)))),((_tmp9EB->identity=
- 1,((_tmp9EB->kind=(void*)((_tmp9ED=_cycalloc_atomic(sizeof(*_tmp9ED)),((_tmp9ED[
0]=((_tmp9EC.tag=0,((_tmp9EC.f1=Cyc_Absyn_RgnKind,_tmp9EC)))),_tmp9ED)))),
_tmp9EB)))))));struct _dyneither_ptr*_tmp9F1;struct _tuple1*_tmp9F0;struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(((_tmp9F0=_cycalloc(sizeof(*_tmp9F0)),((_tmp9F0->f1=Cyc_Absyn_Loc_n,((
_tmp9F0->f2=((_tmp9F1=_cycalloc(sizeof(*_tmp9F1)),((_tmp9F1[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp9F1)))),_tmp9F0)))))),(
void*)& Cyc_Absyn_VoidType_val,0);{struct Cyc_List_List*_tmp9F2;yyval=Cyc_YY18(((
_tmp9F2=_cycalloc(sizeof(*_tmp9F2)),((_tmp9F2->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp9F2->tl=0,
_tmp9F2)))))));}break;}case 36: _LL233: yyval=Cyc_YY31(0);break;case 37: _LL234: yyval=
Cyc_YY31(1);break;case 38: _LL235: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 39: _LL236: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 40: _LL237:{
struct Cyc_Core_Opt*_tmp9F5;struct Cyc_Parse_Declaration_spec*_tmp9F4;yyval=Cyc_YY19(((
_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4->sc=((_tmp9F5=_cycalloc_atomic(
sizeof(*_tmp9F5)),((_tmp9F5->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp9F5)))),((_tmp9F4->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp9F4->type_specs=
0,((_tmp9F4->is_inline=0,((_tmp9F4->attributes=0,_tmp9F4)))))))))))));}break;
case 41: _LL238: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmp9F8;void*_tmp9F7;(_tmp9F7=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmp9F8="Only one storage class is allowed in a declaration",
_tag_dyneither(_tmp9F8,sizeof(char),51))),_tag_dyneither(_tmp9F7,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp9FB;struct Cyc_Parse_Declaration_spec*_tmp9FA;yyval=Cyc_YY19(((
_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA->sc=((_tmp9FB=_cycalloc_atomic(
sizeof(*_tmp9FB)),((_tmp9FB->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp9FB)))),((_tmp9FA->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmp9FA->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmp9FA->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmp9FA->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmp9FA)))))))))))));}break;case 42: _LL239:{struct Cyc_List_List*_tmp9FE;struct Cyc_Parse_Declaration_spec*
_tmp9FD;yyval=Cyc_YY19(((_tmp9FD=_cycalloc(sizeof(*_tmp9FD)),((_tmp9FD->sc=0,((
_tmp9FD->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmp9FD->type_specs=((
_tmp9FE=_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp9FE->tl=0,_tmp9FE)))))),((
_tmp9FD->is_inline=0,((_tmp9FD->attributes=0,_tmp9FD)))))))))))));}break;case 43:
_LL23A:{struct Cyc_List_List*_tmpA01;struct Cyc_Parse_Declaration_spec*_tmpA00;
yyval=Cyc_YY19(((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpA00->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpA00->type_specs=((_tmpA01=_cycalloc(sizeof(*_tmpA01)),((_tmpA01->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpA01->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,_tmpA01)))))),((
_tmpA00->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpA00->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpA00)))))))))))));}
break;case 44: _LL23B:{struct Cyc_Parse_Declaration_spec*_tmpA02;yyval=Cyc_YY19(((
_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02->sc=0,((_tmpA02->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA02->type_specs=
0,((_tmpA02->is_inline=0,((_tmpA02->attributes=0,_tmpA02)))))))))))));}break;
case 45: _LL23C:{struct Cyc_Parse_Declaration_spec*_tmpA03;yyval=Cyc_YY19(((_tmpA03=
_cycalloc(sizeof(*_tmpA03)),((_tmpA03->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpA03->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpA03->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpA03->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpA03->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpA03)))))))))))));}
break;case 46: _LL23D:{struct Cyc_Parse_Declaration_spec*_tmpA04;yyval=Cyc_YY19(((
_tmpA04=_cycalloc(sizeof(*_tmpA04)),((_tmpA04->sc=0,((_tmpA04->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpA04->type_specs=0,((_tmpA04->is_inline=1,((
_tmpA04->attributes=0,_tmpA04)))))))))))));}break;case 47: _LL23E:{struct Cyc_Parse_Declaration_spec*
_tmpA05;yyval=Cyc_YY19(((_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpA05->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpA05->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpA05->is_inline=1,((_tmpA05->attributes=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpA05)))))))))))));}
break;case 48: _LL23F:{struct Cyc_Parse_Declaration_spec*_tmpA06;yyval=Cyc_YY19(((
_tmpA06=_cycalloc(sizeof(*_tmpA06)),((_tmpA06->sc=0,((_tmpA06->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpA06->type_specs=0,((_tmpA06->is_inline=0,((
_tmpA06->attributes=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA06)))))))))))));}break;case 49: _LL240:{struct Cyc_Parse_Declaration_spec*
_tmpA07;yyval=Cyc_YY19(((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpA07->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpA07->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,((_tmpA07->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpA07->attributes=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes),_tmpA07)))))))))))));}
break;case 50: _LL241: yyval=Cyc_YY22(Cyc_Parse_Auto_sc);break;case 51: _LL242: yyval=
Cyc_YY22(Cyc_Parse_Register_sc);break;case 52: _LL243: yyval=Cyc_YY22(Cyc_Parse_Static_sc);
break;case 53: _LL244: yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 54: _LL245:{
const char*_tmpA08;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA08="C",
_tag_dyneither(_tmpA08,sizeof(char),2))))!= 0){const char*_tmpA09;Cyc_Parse_err(((
_tmpA09="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpA09,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}}yyval=Cyc_YY22(Cyc_Parse_ExternC_sc);break;case 55:
_LL246: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);break;case 56: _LL247: yyval=Cyc_YY22(
Cyc_Parse_Abstract_sc);break;case 57: _LL248: yyval=Cyc_YY45(0);break;case 58: _LL249:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 59:
_LL24A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 60: _LL24B:{struct Cyc_List_List*_tmpA0A;yyval=Cyc_YY45(((_tmpA0A=_cycalloc(
sizeof(*_tmpA0A)),((_tmpA0A->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA0A->tl=0,_tmpA0A)))))));}
break;case 61: _LL24C:{struct Cyc_List_List*_tmpA0B;yyval=Cyc_YY45(((_tmpA0B=
_cycalloc(sizeof(*_tmpA0B)),((_tmpA0B->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA0B->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA0B)))))));}break;
case 62: _LL24D: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};
static struct _tuple25 att_map[17]={{{_tmp4A8,_tmp4A8,_tmp4A8 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
_tmp4A9,_tmp4A9,_tmp4A9 + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4AA,_tmp4AA,
_tmp4AA + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4AB,_tmp4AB,_tmp4AB + 9},(
void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4AC,_tmp4AC,_tmp4AC + 6},(void*)& Cyc_Absyn_Const_att_val},{{
_tmp4AD,_tmp4AD,_tmp4AD + 8},(void*)& att_aligned},{{_tmp4AE,_tmp4AE,_tmp4AE + 7},(
void*)& Cyc_Absyn_Packed_att_val},{{_tmp4AF,_tmp4AF,_tmp4AF + 7},(void*)& Cyc_Absyn_Shared_att_val},{{
_tmp4B0,_tmp4B0,_tmp4B0 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4B1,_tmp4B1,
_tmp4B1 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4B2,_tmp4B2,_tmp4B2 + 10},(void*)&
Cyc_Absyn_Dllimport_att_val},{{_tmp4B3,_tmp4B3,_tmp4B3 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{
_tmp4B4,_tmp4B4,_tmp4B4 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{
_tmp4B5,_tmp4B5,_tmp4B5 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4B6,
_tmp4B6,_tmp4B6 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4B7,_tmp4B7,
_tmp4B7 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4B8,_tmp4B8,
_tmp4B8 + 5},(void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp4A6=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp4A6,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp4A6,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp4A6,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp4A6,sizeof(char),(int)(_get_dyneither_size(
_tmp4A6,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp4A6,sizeof(char),(int)(_get_dyneither_size(_tmp4A6,sizeof(char))- 3)))== '_')
_tmp4A6=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4A6,2,
_get_dyneither_size(_tmp4A6,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp4A6,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){{const char*_tmpA0C;Cyc_Parse_err(((
_tmpA0C="unrecognized attribute",_tag_dyneither(_tmpA0C,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=Cyc_YY46((
void*)& Cyc_Absyn_Cdecl_att_val);}break;}}case 63: _LL24E: yyval=Cyc_YY46((void*)&
Cyc_Absyn_Const_att_val);break;case 64: _LL24F: {struct _dyneither_ptr _tmp4BA=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp4BC;struct
_tuple6 _tmp4BB=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4BC=_tmp4BB.f2;{void*a;{const char*_tmpA0E;const char*
_tmpA0D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4BA,((_tmpA0E="regparm",
_tag_dyneither(_tmpA0E,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp4BA,((_tmpA0D="__regparm__",_tag_dyneither(_tmpA0D,sizeof(
char),12))))== 0){if(_tmp4BC < 0  || _tmp4BC > 3){const char*_tmpA0F;Cyc_Parse_err(((
_tmpA0F="regparm requires value between 0 and 3",_tag_dyneither(_tmpA0F,sizeof(
char),39))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct _tmpA12;
struct Cyc_Absyn_Regparm_att_struct*_tmpA11;a=(void*)((_tmpA11=_cycalloc_atomic(
sizeof(*_tmpA11)),((_tmpA11[0]=((_tmpA12.tag=0,((_tmpA12.f1=_tmp4BC,_tmpA12)))),
_tmpA11))));}}else{const char*_tmpA14;const char*_tmpA13;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp4BA,((_tmpA14="aligned",_tag_dyneither(_tmpA14,sizeof(char),8))))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4BA,((_tmpA13="__aligned__",
_tag_dyneither(_tmpA13,sizeof(char),12))))== 0){if(_tmp4BC < 0){const char*_tmpA15;
Cyc_Parse_err(((_tmpA15="aligned requires positive power of two",_tag_dyneither(
_tmpA15,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp4BC;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpA16;Cyc_Parse_err(((_tmpA16="aligned requires positive power of two",
_tag_dyneither(_tmpA16,sizeof(char),39))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpA19;struct Cyc_Absyn_Aligned_att_struct*_tmpA18;a=(void*)((_tmpA18=
_cycalloc_atomic(sizeof(*_tmpA18)),((_tmpA18[0]=((_tmpA19.tag=6,((_tmpA19.f1=
_tmp4BC,_tmpA19)))),_tmpA18))));}}}else{const char*_tmpA1B;const char*_tmpA1A;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp4BA,((_tmpA1B="initializes",_tag_dyneither(
_tmpA1B,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4BA,((
_tmpA1A="__initializes__",_tag_dyneither(_tmpA1A,sizeof(char),16))))== 0){struct
Cyc_Absyn_Initializes_att_struct _tmpA1E;struct Cyc_Absyn_Initializes_att_struct*
_tmpA1D;a=(void*)((_tmpA1D=_cycalloc_atomic(sizeof(*_tmpA1D)),((_tmpA1D[0]=((
_tmpA1E.tag=20,((_tmpA1E.f1=_tmp4BC,_tmpA1E)))),_tmpA1D))));}else{{const char*
_tmpA1F;Cyc_Parse_err(((_tmpA1F="unrecognized attribute",_tag_dyneither(_tmpA1F,
sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}}}yyval=Cyc_YY46(a);break;}}case 65: _LL250: {struct
_dyneither_ptr _tmp4CD=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp4CE=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpA21;const char*_tmpA20;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4CD,((_tmpA21="section",
_tag_dyneither(_tmpA21,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct
_dyneither_ptr)_tmp4CD,((_tmpA20="__section__",_tag_dyneither(_tmpA20,sizeof(
char),12))))== 0){struct Cyc_Absyn_Section_att_struct _tmpA24;struct Cyc_Absyn_Section_att_struct*
_tmpA23;a=(void*)((_tmpA23=_cycalloc(sizeof(*_tmpA23)),((_tmpA23[0]=((_tmpA24.tag=
8,((_tmpA24.f1=_tmp4CE,_tmpA24)))),_tmpA23))));}else{{const char*_tmpA25;Cyc_Parse_err(((
_tmpA25="unrecognized attribute",_tag_dyneither(_tmpA25,sizeof(char),23))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 66: _LL251: {struct
_dyneither_ptr _tmp4D4=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dyneither_ptr _tmp4D5=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpA26;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4D4,((_tmpA26="__mode__",
_tag_dyneither(_tmpA26,sizeof(char),9))))== 0){struct Cyc_Absyn_Mode_att_struct
_tmpA29;struct Cyc_Absyn_Mode_att_struct*_tmpA28;a=(void*)((_tmpA28=_cycalloc(
sizeof(*_tmpA28)),((_tmpA28[0]=((_tmpA29.tag=22,((_tmpA29.f1=_tmp4D5,_tmpA29)))),
_tmpA28))));}else{{const char*_tmpA2A;Cyc_Parse_err(((_tmpA2A="unrecognized attribute",
_tag_dyneither(_tmpA2A,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)&
Cyc_Absyn_Cdecl_att_val;}}yyval=Cyc_YY46(a);break;}case 67: _LL252: {struct
_dyneither_ptr _tmp4DA=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp4DB=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp4DD;struct _tuple6
_tmp4DC=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp4DD=_tmp4DC.f2;{int _tmp4DF;struct _tuple6 _tmp4DE=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4DF=_tmp4DE.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;{const char*_tmpA2C;const char*_tmpA2B;if(
Cyc_zstrcmp((struct _dyneither_ptr)_tmp4DA,((_tmpA2C="format",_tag_dyneither(
_tmpA2C,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4DA,((
_tmpA2B="__format__",_tag_dyneither(_tmpA2B,sizeof(char),11))))== 0){const char*
_tmpA2D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4DB,((_tmpA2D="printf",
_tag_dyneither(_tmpA2D,sizeof(char),7))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpA30;struct Cyc_Absyn_Format_att_struct*_tmpA2F;a=(void*)((_tmpA2F=
_cycalloc_atomic(sizeof(*_tmpA2F)),((_tmpA2F[0]=((_tmpA30.tag=19,((_tmpA30.f1=
Cyc_Absyn_Printf_ft,((_tmpA30.f2=_tmp4DD,((_tmpA30.f3=_tmp4DF,_tmpA30)))))))),
_tmpA2F))));}else{const char*_tmpA31;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4DB,((
_tmpA31="scanf",_tag_dyneither(_tmpA31,sizeof(char),6))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpA34;struct Cyc_Absyn_Format_att_struct*_tmpA33;a=(void*)((_tmpA33=
_cycalloc_atomic(sizeof(*_tmpA33)),((_tmpA33[0]=((_tmpA34.tag=19,((_tmpA34.f1=
Cyc_Absyn_Scanf_ft,((_tmpA34.f2=_tmp4DD,((_tmpA34.f3=_tmp4DF,_tmpA34)))))))),
_tmpA33))));}else{const char*_tmpA35;Cyc_Parse_err(((_tmpA35="unrecognized format type",
_tag_dyneither(_tmpA35,sizeof(char),25))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpA36;Cyc_Parse_err(((_tmpA36="unrecognized attribute",
_tag_dyneither(_tmpA36,sizeof(char),23))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY46(
a);break;}}}case 68: _LL253: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 69: _LL254:{struct Cyc_Absyn_TypedefType_struct _tmpA39;
struct Cyc_Absyn_TypedefType_struct*_tmpA38;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=((_tmpA39.tag=18,((
_tmpA39.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpA39.f2=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA39.f3=0,((_tmpA39.f4=
0,_tmpA39)))))))))),_tmpA38)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 70:
_LL255: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 71:
_LL256: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 72:
_LL257: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 73:
_LL258: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 74:
_LL259:{struct Cyc_Parse_Short_spec_struct _tmpA3C;struct Cyc_Parse_Short_spec_struct*
_tmpA3B;yyval=Cyc_YY23((void*)((_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=((
_tmpA3C.tag=2,((_tmpA3C.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA3C)))),
_tmpA3B)))));}break;case 75: _LL25A: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 76: _LL25B:{struct Cyc_Parse_Long_spec_struct
_tmpA3F;struct Cyc_Parse_Long_spec_struct*_tmpA3E;yyval=Cyc_YY23((void*)((_tmpA3E=
_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=((_tmpA3F.tag=3,((_tmpA3F.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA3F)))),
_tmpA3E)))));}break;case 77: _LL25C: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 78: _LL25D: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 79:
_LL25E:{struct Cyc_Parse_Signed_spec_struct _tmpA42;struct Cyc_Parse_Signed_spec_struct*
_tmpA41;yyval=Cyc_YY23((void*)((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41[0]=((
_tmpA42.tag=0,((_tmpA42.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA42)))),
_tmpA41)))));}break;case 80: _LL25F:{struct Cyc_Parse_Unsigned_spec_struct _tmpA45;
struct Cyc_Parse_Unsigned_spec_struct*_tmpA44;yyval=Cyc_YY23((void*)((_tmpA44=
_cycalloc(sizeof(*_tmpA44)),((_tmpA44[0]=((_tmpA45.tag=1,((_tmpA45.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA45)))),
_tmpA44)))));}break;case 81: _LL260: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 82: _LL261: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 83: _LL262: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL263:
yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 85:
_LL264:{struct Cyc_Absyn_TupleType_struct _tmpA48;struct Cyc_Absyn_TupleType_struct*
_tmpA47;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA47=_cycalloc(sizeof(*
_tmpA47)),((_tmpA47[0]=((_tmpA48.tag=11,((_tmpA48.f1=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpA48)))),_tmpA47)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 86: _LL265:{struct Cyc_Absyn_RgnHandleType_struct
_tmpA4B;struct Cyc_Absyn_RgnHandleType_struct*_tmpA4A;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=((_tmpA4B.tag=16,((
_tmpA4B.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpA4B)))),_tmpA4A)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 87:
_LL266: {void*_tmp4F8=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);{
struct Cyc_Absyn_DynRgnType_struct _tmpA4E;struct Cyc_Absyn_DynRgnType_struct*
_tmpA4D;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA4D=_cycalloc(sizeof(*
_tmpA4D)),((_tmpA4D[0]=((_tmpA4E.tag=17,((_tmpA4E.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpA4E.f2=(void*)
_tmp4F8,_tmpA4E)))))),_tmpA4D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case 88:
_LL267:{struct Cyc_Absyn_DynRgnType_struct _tmpA51;struct Cyc_Absyn_DynRgnType_struct*
_tmpA50;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA50=_cycalloc(sizeof(*
_tmpA50)),((_tmpA50[0]=((_tmpA51.tag=17,((_tmpA51.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA51.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA51)))))),
_tmpA50)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 89: _LL268:{struct Cyc_Absyn_TagType_struct
_tmpA54;struct Cyc_Absyn_TagType_struct*_tmpA53;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpA53=_cycalloc(sizeof(*_tmpA53)),((_tmpA53[0]=((_tmpA54.tag=20,((
_tmpA54.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpA54)))),_tmpA53)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 90:
_LL269:{struct Cyc_Absyn_TagType_struct _tmpA57;struct Cyc_Absyn_TagType_struct*
_tmpA56;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA56=_cycalloc(sizeof(*
_tmpA56)),((_tmpA56[0]=((_tmpA57.tag=20,((_tmpA57.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,0),_tmpA57)))),_tmpA56)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 91:
_LL26A:{struct Cyc_Absyn_ValueofType_struct _tmpA5A;struct Cyc_Absyn_ValueofType_struct*
_tmpA59;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA59=_cycalloc(sizeof(*
_tmpA59)),((_tmpA59[0]=((_tmpA5A.tag=19,((_tmpA5A.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA5A)))),_tmpA59)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 92: _LL26B: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 93: _LL26C: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;{struct Cyc_Absyn_Tqual
_tmpA5B;yyval=Cyc_YY25(((_tmpA5B.print_const=1,((_tmpA5B.q_volatile=0,((_tmpA5B.q_restrict=
0,((_tmpA5B.real_const=1,((_tmpA5B.loc=loc,_tmpA5B)))))))))));}break;}case 94:
_LL26D:{struct Cyc_Absyn_Tqual _tmpA5C;yyval=Cyc_YY25(((_tmpA5C.print_const=0,((
_tmpA5C.q_volatile=1,((_tmpA5C.q_restrict=0,((_tmpA5C.real_const=0,((_tmpA5C.loc=
0,_tmpA5C)))))))))));}break;case 95: _LL26E:{struct Cyc_Absyn_Tqual _tmpA5D;yyval=
Cyc_YY25(((_tmpA5D.print_const=0,((_tmpA5D.q_volatile=0,((_tmpA5D.q_restrict=1,((
_tmpA5D.real_const=0,((_tmpA5D.loc=0,_tmpA5D)))))))))));}break;case 96: _LL26F:{
struct Cyc_Parse_Decl_spec_struct _tmpA79;struct Cyc_Absyn_Enum_d_struct*_tmpA78;
struct Cyc_Absyn_Enumdecl*_tmpA77;struct Cyc_Core_Opt*_tmpA76;struct Cyc_Absyn_Enum_d_struct
_tmpA75;struct Cyc_Absyn_Decl*_tmpA74;struct Cyc_Parse_Decl_spec_struct*_tmpA73;
yyval=Cyc_YY23((void*)((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73[0]=((
_tmpA79.tag=5,((_tmpA79.f1=((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74->r=(
void*)((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78[0]=((_tmpA75.tag=8,((
_tmpA75.f1=((_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77->sc=Cyc_Absyn_Public,((
_tmpA77->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmpA77->fields=((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((
_tmpA76->v=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),_tmpA76)))),_tmpA77)))))))),_tmpA75)))),_tmpA78)))),((_tmpA74->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA74)))))),
_tmpA79)))),_tmpA73)))));}break;case 97: _LL270:{struct Cyc_Absyn_EnumType_struct
_tmpA7C;struct Cyc_Absyn_EnumType_struct*_tmpA7B;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B[0]=((_tmpA7C.tag=14,((
_tmpA7C.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpA7C.f2=0,_tmpA7C)))))),_tmpA7B)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 98:
_LL271:{struct Cyc_Parse_Type_spec_struct _tmpA86;struct Cyc_Absyn_AnonEnumType_struct
_tmpA85;struct Cyc_Absyn_AnonEnumType_struct*_tmpA84;struct Cyc_Parse_Type_spec_struct*
_tmpA83;yyval=Cyc_YY23((void*)((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=((
_tmpA86.tag=4,((_tmpA86.f1=(void*)((void*)((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((
_tmpA84[0]=((_tmpA85.tag=15,((_tmpA85.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA85)))),_tmpA84))))),((
_tmpA86.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA86)))))),_tmpA83)))));}break;case 99: _LL272:{
struct Cyc_Absyn_Enumfield*_tmpA87;yyval=Cyc_YY47(((_tmpA87=_cycalloc(sizeof(*
_tmpA87)),((_tmpA87->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA87->tag=0,((_tmpA87->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA87)))))))));}break;case 100: _LL273:{struct Cyc_Absyn_Enumfield*
_tmpA88;yyval=Cyc_YY47(((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA88->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpA88->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpA88)))))))));}
break;case 101: _LL274:{struct Cyc_List_List*_tmpA89;yyval=Cyc_YY48(((_tmpA89=
_cycalloc(sizeof(*_tmpA89)),((_tmpA89->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpA89->tl=0,_tmpA89)))))));}
break;case 102: _LL275:{struct Cyc_List_List*_tmpA8A;yyval=Cyc_YY48(((_tmpA8A=
_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpA8A->tl=0,_tmpA8A)))))));}
break;case 103: _LL276:{struct Cyc_List_List*_tmpA8B;yyval=Cyc_YY48(((_tmpA8B=
_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA8B->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA8B)))))));}break;
case 104: _LL277:{struct Cyc_Absyn_AnonAggrType_struct _tmpA8E;struct Cyc_Absyn_AnonAggrType_struct*
_tmpA8D;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA8D=_cycalloc(sizeof(*
_tmpA8D)),((_tmpA8D[0]=((_tmpA8E.tag=13,((_tmpA8E.f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA8E.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpA8E)))))),
_tmpA8D)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 105: _LL278: {struct Cyc_List_List*_tmp51A=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp51B=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct
_tmpA91;struct Cyc_Parse_Decl_spec_struct*_tmpA90;yyval=Cyc_YY23((void*)((_tmpA90=
_cycalloc(sizeof(*_tmpA90)),((_tmpA90[0]=((_tmpA91.tag=5,((_tmpA91.f1=Cyc_Absyn_aggr_decl(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),
_tmp51A,(struct Cyc_Absyn_AggrdeclImpl*)Cyc_Absyn_aggrdecl_impl(_tmp51B,Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpA91)))),
_tmpA90)))));}break;}case 106: _LL279: {struct Cyc_List_List*_tmp51E=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp51F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct _tmpA94;struct Cyc_Parse_Decl_spec_struct*
_tmpA93;yyval=Cyc_YY23((void*)((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((_tmpA93[0]=((
_tmpA94.tag=5,((_tmpA94.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp51E,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp51F,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpA94)))),
_tmpA93)))));}break;}case 107: _LL27A:{struct Cyc_Absyn_AggrType_struct _tmpA9E;
struct Cyc_Core_Opt*_tmpA9D;struct Cyc_Absyn_AggrInfo _tmpA9C;struct Cyc_Absyn_AggrType_struct*
_tmpA9B;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((_tmpA9B=_cycalloc(sizeof(*
_tmpA9B)),((_tmpA9B[0]=((_tmpA9E.tag=12,((_tmpA9E.f1=((_tmpA9C.aggr_info=Cyc_Absyn_UnknownAggr(
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpA9D=
_cycalloc_atomic(sizeof(*_tmpA9D)),((_tmpA9D->v=(void*)1,_tmpA9D))))),((_tmpA9C.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA9C)))),
_tmpA9E)))),_tmpA9B)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 108:
_LL27B:{struct Cyc_Absyn_AggrType_struct _tmpAA4;struct Cyc_Absyn_AggrInfo _tmpAA3;
struct Cyc_Absyn_AggrType_struct*_tmpAA2;yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)((
_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2[0]=((_tmpAA4.tag=12,((_tmpAA4.f1=((
_tmpAA3.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),((_tmpAA3.targs=
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAA3)))),
_tmpAA4)))),_tmpAA2)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 109:
_LL27C: yyval=Cyc_YY40(0);break;case 110: _LL27D: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 111: _LL27E:
yyval=Cyc_YY24(Cyc_Absyn_StructA);break;case 112: _LL27F: yyval=Cyc_YY24(Cyc_Absyn_UnionA);
break;case 113: _LL280: yyval=Cyc_YY26(0);break;case 114: _LL281: yyval=Cyc_YY26(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))));break;case 115: _LL282:{
struct Cyc_List_List*_tmpAA5;yyval=Cyc_YY27(((_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((
_tmpAA5->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpAA5->tl=0,_tmpAA5)))))));}break;case 116: _LL283:{struct Cyc_List_List*
_tmpAA6;yyval=Cyc_YY27(((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->hd=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAA6->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpAA6)))))));}
break;case 117: _LL284: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 118: _LL285:{struct Cyc_List_List*_tmpAA7;yyval=Cyc_YY21(((
_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAA7->tl=0,_tmpAA7)))))));}
break;case 119: _LL286:{struct Cyc_List_List*_tmpAA8;yyval=Cyc_YY21(((_tmpAA8=
_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAA8->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAA8)))))));}
break;case 120: _LL287:{struct _tuple21*_tmpAA9;yyval=Cyc_YY20(((_tmpAA9=_cycalloc(
sizeof(*_tmpAA9)),((_tmpAA9->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAA9->f2=0,_tmpAA9)))))));}
break;case 121: _LL288:{struct _tuple21*_tmpAAA;yyval=Cyc_YY20(((_tmpAAA=_cycalloc(
sizeof(*_tmpAAA)),((_tmpAAA->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAAA->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAAA)))))));}
break;case 122: _LL289: {struct _RegionHandle _tmp52F=_new_region("temp");struct
_RegionHandle*temp=& _tmp52F;_push_region(temp);{struct _tuple22 _tmp531;struct Cyc_Absyn_Tqual
_tmp532;struct Cyc_List_List*_tmp533;struct Cyc_List_List*_tmp534;struct _tuple22*
_tmp530=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp531=*_tmp530;_tmp532=_tmp531.f1;_tmp533=_tmp531.f2;_tmp534=_tmp531.f3;if(
_tmp532.loc == 0)_tmp532.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);{struct Cyc_List_List*
_tmp536;struct Cyc_List_List*_tmp537;struct _tuple0 _tmp535=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
temp,temp,Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]));_tmp536=_tmp535.f1;_tmp537=_tmp535.f2;{void*_tmp538=Cyc_Parse_speclist2typ(
_tmp533,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));struct Cyc_List_List*_tmp539=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(temp,_tmp532,_tmp538,_tmp536,
_tmp534),_tmp537);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Position_Segment*,struct _tuple15*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp539));}}}
_npop_handler(0);break;;_pop_region(temp);}case 123: _LL28A:{struct Cyc_List_List*
_tmpAAD;struct _tuple22*_tmpAAC;yyval=Cyc_YY35(((_tmpAAC=_cycalloc(sizeof(*
_tmpAAC)),((_tmpAAC->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAAC->f2=((
_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAAD->tl=0,_tmpAAD)))))),((
_tmpAAC->f3=0,_tmpAAC)))))))));}break;case 124: _LL28B:{struct Cyc_List_List*
_tmpAB0;struct _tuple22*_tmpAAF;yyval=Cyc_YY35(((_tmpAAF=_cycalloc(sizeof(*
_tmpAAF)),((_tmpAAF->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpAAF->f2=((_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((
_tmpAB0->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpAB0->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpAB0)))))),((_tmpAAF->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpAAF)))))))));}break;case 125: _LL28C:{struct _tuple22*_tmpAB1;yyval=Cyc_YY35(((
_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAB1->f2=0,((_tmpAB1->f3=
0,_tmpAB1)))))))));}break;case 126: _LL28D:{struct _tuple22*_tmpAB2;yyval=Cyc_YY35(((
_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpAB2->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpAB2->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpAB2)))))))));}break;case 127: _LL28E:{struct _tuple22*
_tmpAB3;yyval=Cyc_YY35(((_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAB3->f2=0,((_tmpAB3->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAB3)))))))));}break;
case 128: _LL28F:{struct _tuple22*_tmpAB4;yyval=Cyc_YY35(((_tmpAB4=_cycalloc(
sizeof(*_tmpAB4)),((_tmpAB4->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpAB4->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpAB4->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpAB4)))))))));}break;case 129: _LL290:{struct Cyc_List_List*_tmpAB7;struct
_tuple22*_tmpAB6;yyval=Cyc_YY35(((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAB6->f2=((
_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAB7->tl=0,_tmpAB7)))))),((
_tmpAB6->f3=0,_tmpAB6)))))))));}break;case 130: _LL291:{struct Cyc_List_List*
_tmpABA;struct _tuple22*_tmpAB9;yyval=Cyc_YY35(((_tmpAB9=_cycalloc(sizeof(*
_tmpAB9)),((_tmpAB9->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])).f1,((_tmpAB9->f2=((_tmpABA=_cycalloc(sizeof(*_tmpABA)),((
_tmpABA->hd=Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpABA->tl=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpABA)))))),((_tmpAB9->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,
_tmpAB9)))))))));}break;case 131: _LL292:{struct _tuple22*_tmpABB;yyval=Cyc_YY35(((
_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpABB->f2=0,((_tmpABB->f3=
0,_tmpABB)))))))));}break;case 132: _LL293:{struct _tuple22*_tmpABC;yyval=Cyc_YY35(((
_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpABC->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpABC->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpABC)))))))));}break;case 133: _LL294:{struct _tuple22*
_tmpABD;yyval=Cyc_YY35(((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpABD->f2=0,((_tmpABD->f3=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpABD)))))))));}break;
case 134: _LL295:{struct _tuple22*_tmpABE;yyval=Cyc_YY35(((_tmpABE=_cycalloc(
sizeof(*_tmpABE)),((_tmpABE->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpABE->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpABE->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpABE)))))))));}break;case 135: _LL296: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 136: _LL297:{
struct Cyc_List_List*_tmpABF;yyval=Cyc_YY21(((_tmpABF=_cycalloc(sizeof(*_tmpABF)),((
_tmpABF->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpABF->tl=0,_tmpABF)))))));}break;case 137: _LL298:{struct Cyc_List_List*
_tmpAC0;yyval=Cyc_YY21(((_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAC0->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAC0)))))));}
break;case 138: _LL299:{struct _tuple21*_tmpAC1;yyval=Cyc_YY20(((_tmpAC1=_cycalloc(
sizeof(*_tmpAC1)),((_tmpAC1->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAC1->f2=0,_tmpAC1)))))));}
break;case 139: _LL29A:{struct Cyc_Parse_Declarator*_tmpAC7;struct _tuple1*_tmpAC6;
struct _tuple21*_tmpAC5;yyval=Cyc_YY20(((_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((
_tmpAC5->f1=((_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7->id=((_tmpAC6=
_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6->f1=Cyc_Absyn_Rel_n(0),((_tmpAC6->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),
_tmpAC6)))))),((_tmpAC7->tms=0,_tmpAC7)))))),((_tmpAC5->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAC5)))))));}
break;case 140: _LL29B:{struct _tuple21*_tmpAC8;yyval=Cyc_YY20(((_tmpAC8=_cycalloc(
sizeof(*_tmpAC8)),((_tmpAC8->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAC8->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAC8)))))));}
break;case 141: _LL29C: {int _tmp552=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);struct Cyc_List_List*
_tmp553=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct _tmpACE;struct Cyc_Core_Opt*
_tmpACD;struct Cyc_Parse_Decl_spec_struct*_tmpACC;yyval=Cyc_YY23((void*)((_tmpACC=
_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=((_tmpACE.tag=5,((_tmpACE.f1=Cyc_Absyn_datatype_decl(
Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),_tmp553,((_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD->v=
Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpACD)))),_tmp552,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpACE)))),
_tmpACC)))));}break;}case 142: _LL29D: {int _tmp557=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);{struct Cyc_Absyn_DatatypeType_struct
_tmpAD8;struct Cyc_Absyn_UnknownDatatypeInfo _tmpAD7;struct Cyc_Absyn_DatatypeInfo
_tmpAD6;struct Cyc_Absyn_DatatypeType_struct*_tmpAD5;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5[0]=((_tmpAD8.tag=3,((
_tmpAD8.f1=((_tmpAD6.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpAD7.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpAD7.is_extensible=
_tmp557,_tmpAD7))))),((_tmpAD6.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAD6)))),_tmpAD8)))),
_tmpAD5)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;}case 143: _LL29E: {int _tmp55C=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);{struct Cyc_Absyn_DatatypeFieldType_struct
_tmpAE2;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpAE1;struct Cyc_Absyn_DatatypeFieldInfo
_tmpAE0;struct Cyc_Absyn_DatatypeFieldType_struct*_tmpADF;yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)((_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF[0]=((_tmpAE2.tag=4,((
_tmpAE2.f1=((_tmpAE0.field_info=Cyc_Absyn_UnknownDatatypefield(((_tmpAE1.datatype_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpAE1.field_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpAE1.is_extensible=_tmp55C,_tmpAE1))))))),((
_tmpAE0.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpAE0)))),_tmpAE2)))),_tmpADF)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
144: _LL29F: yyval=Cyc_YY31(0);break;case 145: _LL2A0: yyval=Cyc_YY31(1);break;case
146: _LL2A1:{struct Cyc_List_List*_tmpAE3;yyval=Cyc_YY34(((_tmpAE3=_cycalloc(
sizeof(*_tmpAE3)),((_tmpAE3->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAE3->tl=0,_tmpAE3)))))));}
break;case 147: _LL2A2:{struct Cyc_List_List*_tmpAE4;yyval=Cyc_YY34(((_tmpAE4=
_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpAE4->tl=0,_tmpAE4)))))));}
break;case 148: _LL2A3:{struct Cyc_List_List*_tmpAE5;yyval=Cyc_YY34(((_tmpAE5=
_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAE5->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAE5)))))));}break;
case 149: _LL2A4:{struct Cyc_List_List*_tmpAE6;yyval=Cyc_YY34(((_tmpAE6=_cycalloc(
sizeof(*_tmpAE6)),((_tmpAE6->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpAE6->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAE6)))))));}break;
case 150: _LL2A5: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 151: _LL2A6: yyval=Cyc_YY32(
Cyc_Absyn_Extern);break;case 152: _LL2A7: yyval=Cyc_YY32(Cyc_Absyn_Static);break;
case 153: _LL2A8:{struct Cyc_Absyn_Datatypefield*_tmpAE7;yyval=Cyc_YY33(((_tmpAE7=
_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAE7->typs=0,((_tmpAE7->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpAE7->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpAE7)))))))))));}
break;case 154: _LL2A9: {struct Cyc_List_List*_tmp566=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Datatypefield*
_tmpAE8;yyval=Cyc_YY33(((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpAE8->typs=
_tmp566,((_tmpAE8->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpAE8->sc=
Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpAE8)))))))))));}break;}case 155: _LL2AA: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 156: _LL2AB:{struct
Cyc_Parse_Declarator*_tmpAE9;yyval=Cyc_YY29(((_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((
_tmpAE9->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpAE9->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpAE9)))))));}
break;case 157: _LL2AC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 158: _LL2AD:{struct Cyc_Parse_Declarator*_tmpAEA;yyval=
Cyc_YY29(((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpAEA->tms=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpAEA)))))));}
break;case 159: _LL2AE:{struct Cyc_Parse_Declarator*_tmpAEB;yyval=Cyc_YY29(((
_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAEB->tms=0,_tmpAEB)))))));}
break;case 160: _LL2AF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 161: _LL2B0: {struct Cyc_Parse_Declarator*_tmp56B=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpAF1;struct Cyc_Absyn_Attributes_mod_struct _tmpAF0;struct Cyc_List_List*_tmpAEF;
_tmp56B->tms=((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->hd=(void*)((
_tmpAF1=_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1[0]=((_tmpAF0.tag=5,((_tmpAF0.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpAF0.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAF0)))))),_tmpAF1)))),((
_tmpAEF->tl=_tmp56B->tms,_tmpAEF))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 162: _LL2B1:{
struct Cyc_List_List*_tmpAFB;struct Cyc_Absyn_Carray_mod_struct _tmpAFA;struct Cyc_Absyn_Carray_mod_struct*
_tmpAF9;struct Cyc_Parse_Declarator*_tmpAF8;yyval=Cyc_YY29(((_tmpAF8=_cycalloc(
sizeof(*_tmpAF8)),((_tmpAF8->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpAF8->tms=((
_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->hd=(void*)((_tmpAF9=_cycalloc(
sizeof(*_tmpAF9)),((_tmpAF9[0]=((_tmpAFA.tag=0,((_tmpAFA.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAFA.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpAFA)))))),
_tmpAF9)))),((_tmpAFB->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpAFB)))))),_tmpAF8)))))));}break;case 163:
_LL2B2:{struct Cyc_List_List*_tmpB05;struct Cyc_Absyn_ConstArray_mod_struct _tmpB04;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpB03;struct Cyc_Parse_Declarator*_tmpB02;
yyval=Cyc_YY29(((_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpB02->tms=((
_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->hd=(void*)((_tmpB03=_cycalloc(
sizeof(*_tmpB03)),((_tmpB03[0]=((_tmpB04.tag=1,((_tmpB04.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB04.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB04.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB04)))))))),
_tmpB03)))),((_tmpB05->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpB05)))))),_tmpB02)))))));}break;case 164:
_LL2B3: {struct _tuple23 _tmp578;struct Cyc_List_List*_tmp579;int _tmp57A;struct Cyc_Absyn_VarargInfo*
_tmp57B;struct Cyc_Core_Opt*_tmp57C;struct Cyc_List_List*_tmp57D;struct _tuple23*
_tmp577=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp578=*_tmp577;_tmp579=_tmp578.f1;_tmp57A=_tmp578.f2;_tmp57B=_tmp578.f3;
_tmp57C=_tmp578.f4;_tmp57D=_tmp578.f5;{struct Cyc_List_List*_tmpB1A;struct Cyc_Absyn_Function_mod_struct
_tmpB19;struct Cyc_Absyn_WithTypes_struct _tmpB18;struct Cyc_Absyn_WithTypes_struct*
_tmpB17;struct Cyc_Absyn_Function_mod_struct*_tmpB16;struct Cyc_Parse_Declarator*
_tmpB15;yyval=Cyc_YY29(((_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpB15->tms=((
_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A->hd=(void*)((_tmpB16=_cycalloc(
sizeof(*_tmpB16)),((_tmpB16[0]=((_tmpB19.tag=3,((_tmpB19.f1=(void*)((void*)((
_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17[0]=((_tmpB18.tag=1,((_tmpB18.f1=
_tmp579,((_tmpB18.f2=_tmp57A,((_tmpB18.f3=_tmp57B,((_tmpB18.f4=_tmp57C,((_tmpB18.f5=
_tmp57D,_tmpB18)))))))))))),_tmpB17))))),_tmpB19)))),_tmpB16)))),((_tmpB1A->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpB1A)))))),_tmpB15)))))));}break;}case 165: _LL2B4:{struct Cyc_List_List*_tmpB2F;
struct Cyc_Absyn_Function_mod_struct _tmpB2E;struct Cyc_Absyn_WithTypes_struct
_tmpB2D;struct Cyc_Absyn_WithTypes_struct*_tmpB2C;struct Cyc_Absyn_Function_mod_struct*
_tmpB2B;struct Cyc_Parse_Declarator*_tmpB2A;yyval=Cyc_YY29(((_tmpB2A=_cycalloc(
sizeof(*_tmpB2A)),((_tmpB2A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpB2A->tms=((
_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->hd=(void*)((_tmpB2B=_cycalloc(
sizeof(*_tmpB2B)),((_tmpB2B[0]=((_tmpB2E.tag=3,((_tmpB2E.f1=(void*)((void*)((
_tmpB2C=_cycalloc(sizeof(*_tmpB2C)),((_tmpB2C[0]=((_tmpB2D.tag=1,((_tmpB2D.f1=0,((
_tmpB2D.f2=0,((_tmpB2D.f3=0,((_tmpB2D.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB2D.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB2D)))))))))))),
_tmpB2C))))),_tmpB2E)))),_tmpB2B)))),((_tmpB2F->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpB2F)))))),
_tmpB2A)))))));}break;case 166: _LL2B5:{struct Cyc_List_List*_tmpB44;struct Cyc_Absyn_Function_mod_struct
_tmpB43;struct Cyc_Absyn_NoTypes_struct _tmpB42;struct Cyc_Absyn_NoTypes_struct*
_tmpB41;struct Cyc_Absyn_Function_mod_struct*_tmpB40;struct Cyc_Parse_Declarator*
_tmpB3F;yyval=Cyc_YY29(((_tmpB3F=_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpB3F->tms=((
_tmpB44=_cycalloc(sizeof(*_tmpB44)),((_tmpB44->hd=(void*)((_tmpB40=_cycalloc(
sizeof(*_tmpB40)),((_tmpB40[0]=((_tmpB43.tag=3,((_tmpB43.f1=(void*)((void*)((
_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41[0]=((_tmpB42.tag=0,((_tmpB42.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpB42.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB42)))))),_tmpB41))))),_tmpB43)))),_tmpB40)))),((
_tmpB44->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpB44)))))),_tmpB3F)))))));}break;case 167: _LL2B6: {
struct Cyc_List_List*_tmp590=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpB4E;struct Cyc_Absyn_TypeParams_mod_struct _tmpB4D;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpB4C;struct Cyc_Parse_Declarator*_tmpB4B;yyval=Cyc_YY29(((_tmpB4B=_cycalloc(
sizeof(*_tmpB4B)),((_tmpB4B->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpB4B->tms=((
_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->hd=(void*)((_tmpB4C=_cycalloc(
sizeof(*_tmpB4C)),((_tmpB4C[0]=((_tmpB4D.tag=4,((_tmpB4D.f1=_tmp590,((_tmpB4D.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpB4D.f3=0,_tmpB4D)))))))),_tmpB4C)))),((_tmpB4E->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpB4E)))))),_tmpB4B)))))));}break;}case 168: _LL2B7:{struct Cyc_List_List*_tmpB58;
struct Cyc_Absyn_Attributes_mod_struct _tmpB57;struct Cyc_Absyn_Attributes_mod_struct*
_tmpB56;struct Cyc_Parse_Declarator*_tmpB55;yyval=Cyc_YY29(((_tmpB55=_cycalloc(
sizeof(*_tmpB55)),((_tmpB55->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpB55->tms=((
_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58->hd=(void*)((_tmpB56=_cycalloc(
sizeof(*_tmpB56)),((_tmpB56[0]=((_tmpB57.tag=5,((_tmpB57.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpB57.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB57)))))),_tmpB56)))),((
_tmpB58->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpB58)))))),_tmpB55)))))));}break;case 169: _LL2B8:{
struct Cyc_Parse_Declarator*_tmpB59;yyval=Cyc_YY29(((_tmpB59=_cycalloc(sizeof(*
_tmpB59)),((_tmpB59->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpB59->tms=0,_tmpB59)))))));}break;case 170: _LL2B9:{
struct Cyc_Parse_Declarator*_tmpB5A;yyval=Cyc_YY29(((_tmpB5A=_cycalloc(sizeof(*
_tmpB5A)),((_tmpB5A->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpB5A->tms=0,_tmpB5A)))))));}break;case 171: _LL2BA:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 172:
_LL2BB: {struct Cyc_Parse_Declarator*_tmp59B=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpB60;struct Cyc_Absyn_Attributes_mod_struct _tmpB5F;struct Cyc_List_List*_tmpB5E;
_tmp59B->tms=((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((_tmpB5E->hd=(void*)((
_tmpB60=_cycalloc(sizeof(*_tmpB60)),((_tmpB60[0]=((_tmpB5F.tag=5,((_tmpB5F.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpB5F.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpB5F)))))),_tmpB60)))),((
_tmpB5E->tl=_tmp59B->tms,_tmpB5E))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 173: _LL2BC:{
struct Cyc_List_List*_tmpB6A;struct Cyc_Absyn_Carray_mod_struct _tmpB69;struct Cyc_Absyn_Carray_mod_struct*
_tmpB68;struct Cyc_Parse_Declarator*_tmpB67;yyval=Cyc_YY29(((_tmpB67=_cycalloc(
sizeof(*_tmpB67)),((_tmpB67->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpB67->tms=((
_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((_tmpB6A->hd=(void*)((_tmpB68=_cycalloc(
sizeof(*_tmpB68)),((_tmpB68[0]=((_tmpB69.tag=0,((_tmpB69.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB69.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB69)))))),
_tmpB68)))),((_tmpB6A->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpB6A)))))),_tmpB67)))))));}break;case 174:
_LL2BD:{struct Cyc_List_List*_tmpB74;struct Cyc_Absyn_ConstArray_mod_struct _tmpB73;
struct Cyc_Absyn_ConstArray_mod_struct*_tmpB72;struct Cyc_Parse_Declarator*_tmpB71;
yyval=Cyc_YY29(((_tmpB71=_cycalloc(sizeof(*_tmpB71)),((_tmpB71->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpB71->tms=((
_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->hd=(void*)((_tmpB72=_cycalloc(
sizeof(*_tmpB72)),((_tmpB72[0]=((_tmpB73.tag=1,((_tmpB73.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB73.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB73.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB73)))))))),
_tmpB72)))),((_tmpB74->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpB74)))))),_tmpB71)))))));}break;case 175:
_LL2BE: {struct _tuple23 _tmp5A8;struct Cyc_List_List*_tmp5A9;int _tmp5AA;struct Cyc_Absyn_VarargInfo*
_tmp5AB;struct Cyc_Core_Opt*_tmp5AC;struct Cyc_List_List*_tmp5AD;struct _tuple23*
_tmp5A7=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5A8=*_tmp5A7;_tmp5A9=_tmp5A8.f1;_tmp5AA=_tmp5A8.f2;_tmp5AB=_tmp5A8.f3;
_tmp5AC=_tmp5A8.f4;_tmp5AD=_tmp5A8.f5;{struct Cyc_List_List*_tmpB89;struct Cyc_Absyn_Function_mod_struct
_tmpB88;struct Cyc_Absyn_WithTypes_struct _tmpB87;struct Cyc_Absyn_WithTypes_struct*
_tmpB86;struct Cyc_Absyn_Function_mod_struct*_tmpB85;struct Cyc_Parse_Declarator*
_tmpB84;yyval=Cyc_YY29(((_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpB84->tms=((
_tmpB89=_cycalloc(sizeof(*_tmpB89)),((_tmpB89->hd=(void*)((_tmpB85=_cycalloc(
sizeof(*_tmpB85)),((_tmpB85[0]=((_tmpB88.tag=3,((_tmpB88.f1=(void*)((void*)((
_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86[0]=((_tmpB87.tag=1,((_tmpB87.f1=
_tmp5A9,((_tmpB87.f2=_tmp5AA,((_tmpB87.f3=_tmp5AB,((_tmpB87.f4=_tmp5AC,((_tmpB87.f5=
_tmp5AD,_tmpB87)))))))))))),_tmpB86))))),_tmpB88)))),_tmpB85)))),((_tmpB89->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpB89)))))),_tmpB84)))))));}break;}case 176: _LL2BF:{struct Cyc_List_List*_tmpB9E;
struct Cyc_Absyn_Function_mod_struct _tmpB9D;struct Cyc_Absyn_WithTypes_struct
_tmpB9C;struct Cyc_Absyn_WithTypes_struct*_tmpB9B;struct Cyc_Absyn_Function_mod_struct*
_tmpB9A;struct Cyc_Parse_Declarator*_tmpB99;yyval=Cyc_YY29(((_tmpB99=_cycalloc(
sizeof(*_tmpB99)),((_tmpB99->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpB99->tms=((
_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E->hd=(void*)((_tmpB9A=_cycalloc(
sizeof(*_tmpB9A)),((_tmpB9A[0]=((_tmpB9D.tag=3,((_tmpB9D.f1=(void*)((void*)((
_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B[0]=((_tmpB9C.tag=1,((_tmpB9C.f1=0,((
_tmpB9C.f2=0,((_tmpB9C.f3=0,((_tmpB9C.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB9C.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB9C)))))))))))),
_tmpB9B))))),_tmpB9D)))),_tmpB9A)))),((_tmpB9E->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpB9E)))))),
_tmpB99)))))));}break;case 177: _LL2C0:{struct Cyc_List_List*_tmpBB3;struct Cyc_Absyn_Function_mod_struct
_tmpBB2;struct Cyc_Absyn_NoTypes_struct _tmpBB1;struct Cyc_Absyn_NoTypes_struct*
_tmpBB0;struct Cyc_Absyn_Function_mod_struct*_tmpBAF;struct Cyc_Parse_Declarator*
_tmpBAE;yyval=Cyc_YY29(((_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((_tmpBAE->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBAE->tms=((
_tmpBB3=_cycalloc(sizeof(*_tmpBB3)),((_tmpBB3->hd=(void*)((_tmpBAF=_cycalloc(
sizeof(*_tmpBAF)),((_tmpBAF[0]=((_tmpBB2.tag=3,((_tmpBB2.f1=(void*)((void*)((
_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0[0]=((_tmpBB1.tag=0,((_tmpBB1.f1=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpBB1.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpBB1)))))),_tmpBB0))))),_tmpBB2)))),_tmpBAF)))),((
_tmpBB3->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms,_tmpBB3)))))),_tmpBAE)))))));}break;case 178: _LL2C1: {
struct Cyc_List_List*_tmp5C0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_List_List*
_tmpBBD;struct Cyc_Absyn_TypeParams_mod_struct _tmpBBC;struct Cyc_Absyn_TypeParams_mod_struct*
_tmpBBB;struct Cyc_Parse_Declarator*_tmpBBA;yyval=Cyc_YY29(((_tmpBBA=_cycalloc(
sizeof(*_tmpBBA)),((_tmpBBA->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpBBA->tms=((
_tmpBBD=_cycalloc(sizeof(*_tmpBBD)),((_tmpBBD->hd=(void*)((_tmpBBB=_cycalloc(
sizeof(*_tmpBBB)),((_tmpBBB[0]=((_tmpBBC.tag=4,((_tmpBBC.f1=_tmp5C0,((_tmpBBC.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpBBC.f3=0,_tmpBBC)))))))),_tmpBBB)))),((_tmpBBD->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpBBD)))))),_tmpBBA)))))));}break;}case 179: _LL2C2:{struct Cyc_List_List*_tmpBC7;
struct Cyc_Absyn_Attributes_mod_struct _tmpBC6;struct Cyc_Absyn_Attributes_mod_struct*
_tmpBC5;struct Cyc_Parse_Declarator*_tmpBC4;yyval=Cyc_YY29(((_tmpBC4=_cycalloc(
sizeof(*_tmpBC4)),((_tmpBC4->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpBC4->tms=((
_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7->hd=(void*)((_tmpBC5=_cycalloc(
sizeof(*_tmpBC5)),((_tmpBC5[0]=((_tmpBC6.tag=5,((_tmpBC6.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpBC6.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBC6)))))),_tmpBC5)))),((
_tmpBC7->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpBC7)))))),_tmpBC4)))))));}break;case 180: _LL2C3:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 181:
_LL2C4: yyval=Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 182: _LL2C5: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpBCD;struct Cyc_Absyn_Attributes_mod_struct _tmpBCC;struct Cyc_List_List*_tmpBCB;
ans=((_tmpBCB=_cycalloc(sizeof(*_tmpBCB)),((_tmpBCB->hd=(void*)((_tmpBCD=
_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD[0]=((_tmpBCC.tag=5,((_tmpBCC.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpBCC.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpBCC)))))),_tmpBCD)))),((_tmpBCB->tl=ans,_tmpBCB))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;struct Cyc_Position_Segment*_tmp5CD;union Cyc_Absyn_Constraint*_tmp5CE;
union Cyc_Absyn_Constraint*_tmp5CF;struct _tuple18 _tmp5CC=*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp5CD=_tmp5CC.f1;
_tmp5CE=_tmp5CC.f2;_tmp5CF=_tmp5CC.f3;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*
_tmpBCE;ptrloc=((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE->ptr_loc=_tmp5CD,((
_tmpBCE->rgn_loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpBCE->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),_tmpBCE))))))));}{union Cyc_Absyn_Constraint*
_tmp5D2;union Cyc_Absyn_Constraint*_tmp5D3;union Cyc_Absyn_Constraint*_tmp5D4;void*
_tmp5D5;struct _tuple13 _tmp5D1=Cyc_Parse_collapse_pointer_quals(_tmp5CD,_tmp5CE,
_tmp5CF,Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY54(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
_tmp5D2=_tmp5D1.f1;_tmp5D3=_tmp5D1.f2;_tmp5D4=_tmp5D1.f3;_tmp5D5=_tmp5D1.f4;{
struct Cyc_Absyn_Pointer_mod_struct*_tmpBD8;struct Cyc_Absyn_PtrAtts _tmpBD7;struct
Cyc_Absyn_Pointer_mod_struct _tmpBD6;struct Cyc_List_List*_tmpBD5;ans=((_tmpBD5=
_cycalloc(sizeof(*_tmpBD5)),((_tmpBD5->hd=(void*)((_tmpBD8=_cycalloc(sizeof(*
_tmpBD8)),((_tmpBD8[0]=((_tmpBD6.tag=2,((_tmpBD6.f1=((_tmpBD7.rgn=_tmp5D5,((
_tmpBD7.nullable=_tmp5D2,((_tmpBD7.bounds=_tmp5D3,((_tmpBD7.zero_term=_tmp5D4,((
_tmpBD7.ptrloc=ptrloc,_tmpBD7)))))))))),((_tmpBD6.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBD6)))))),_tmpBD8)))),((
_tmpBD5->tl=ans,_tmpBD5))))));}yyval=Cyc_YY28(ans);break;}}}case 183: _LL2C6: yyval=
Cyc_YY54(0);break;case 184: _LL2C7:{struct Cyc_List_List*_tmpBD9;yyval=Cyc_YY54(((
_tmpBD9=_cycalloc(sizeof(*_tmpBD9)),((_tmpBD9->hd=Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBD9->tl=Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBD9)))))));}break;
case 185: _LL2C8:{struct Cyc_Absyn_Numelts_ptrqual_struct _tmpBDC;struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpBDB;yyval=Cyc_YY53((void*)((_tmpBDB=_cycalloc(sizeof(*_tmpBDB)),((_tmpBDB[0]=((
_tmpBDC.tag=0,((_tmpBDC.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpBDC)))),_tmpBDB)))));}break;case 186: _LL2C9:{struct
Cyc_Absyn_Region_ptrqual_struct _tmpBDF;struct Cyc_Absyn_Region_ptrqual_struct*
_tmpBDE;yyval=Cyc_YY53((void*)((_tmpBDE=_cycalloc(sizeof(*_tmpBDE)),((_tmpBDE[0]=((
_tmpBDF.tag=1,((_tmpBDF.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBDF)))),_tmpBDE)))));}
break;case 187: _LL2CA:{struct Cyc_Absyn_Thin_ptrqual_struct _tmpBE2;struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpBE1;yyval=Cyc_YY53((void*)((_tmpBE1=_cycalloc_atomic(sizeof(*_tmpBE1)),((
_tmpBE1[0]=((_tmpBE2.tag=2,_tmpBE2)),_tmpBE1)))));}break;case 188: _LL2CB:{struct
Cyc_Absyn_Fat_ptrqual_struct _tmpBE5;struct Cyc_Absyn_Fat_ptrqual_struct*_tmpBE4;
yyval=Cyc_YY53((void*)((_tmpBE4=_cycalloc_atomic(sizeof(*_tmpBE4)),((_tmpBE4[0]=((
_tmpBE5.tag=3,_tmpBE5)),_tmpBE4)))));}break;case 189: _LL2CC:{struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmpBE8;struct Cyc_Absyn_Zeroterm_ptrqual_struct*_tmpBE7;yyval=Cyc_YY53((void*)((
_tmpBE7=_cycalloc_atomic(sizeof(*_tmpBE7)),((_tmpBE7[0]=((_tmpBE8.tag=4,_tmpBE8)),
_tmpBE7)))));}break;case 190: _LL2CD:{struct Cyc_Absyn_Nozeroterm_ptrqual_struct
_tmpBEB;struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmpBEA;yyval=Cyc_YY53((void*)((
_tmpBEA=_cycalloc_atomic(sizeof(*_tmpBEA)),((_tmpBEA[0]=((_tmpBEB.tag=5,_tmpBEB)),
_tmpBEA)))));}break;case 191: _LL2CE:{struct Cyc_Absyn_Notnull_ptrqual_struct
_tmpBEE;struct Cyc_Absyn_Notnull_ptrqual_struct*_tmpBED;yyval=Cyc_YY53((void*)((
_tmpBED=_cycalloc_atomic(sizeof(*_tmpBED)),((_tmpBED[0]=((_tmpBEE.tag=6,_tmpBEE)),
_tmpBED)))));}break;case 192: _LL2CF:{struct Cyc_Absyn_Nullable_ptrqual_struct
_tmpBF1;struct Cyc_Absyn_Nullable_ptrqual_struct*_tmpBF0;yyval=Cyc_YY53((void*)((
_tmpBF0=_cycalloc_atomic(sizeof(*_tmpBF0)),((_tmpBF0[0]=((_tmpBF1.tag=7,_tmpBF1)),
_tmpBF0)))));}break;case 193: _LL2D0: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpBF2;yyval=Cyc_YY1(((_tmpBF2=_cycalloc(sizeof(*_tmpBF2)),((_tmpBF2->f1=
loc,((_tmpBF2->f2=Cyc_Absyn_true_conref,((_tmpBF2->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBF2)))))))));}break;}
case 194: _LL2D1: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct
_tuple18*_tmpBF3;yyval=Cyc_YY1(((_tmpBF3=_cycalloc(sizeof(*_tmpBF3)),((_tmpBF3->f1=
loc,((_tmpBF3->f2=Cyc_Absyn_false_conref,((_tmpBF3->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBF3)))))))));}break;}
case 195: _LL2D2: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{struct _tuple18*
_tmpBF4;yyval=Cyc_YY1(((_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4->f1=loc,((
_tmpBF4->f2=Cyc_Absyn_true_conref,((_tmpBF4->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpBF4)))))))));}break;}case 196: _LL2D3: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 197: _LL2D4:{struct Cyc_Absyn_Upper_b_struct _tmpBF7;struct Cyc_Absyn_Upper_b_struct*
_tmpBF6;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)((_tmpBF6=_cycalloc(sizeof(*_tmpBF6)),((_tmpBF6[0]=((_tmpBF7.tag=1,((
_tmpBF7.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpBF7)))),_tmpBF6))))));}break;case 198: _LL2D5: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)());break;case 199: _LL2D6: yyval=Cyc_YY51(Cyc_Absyn_true_conref);
break;case 200: _LL2D7: yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 201: _LL2D8:
yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));
break;case 202: _LL2D9: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_TopRgnKind,1);
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 203:
_LL2DA: yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));
break;case 204: _LL2DB: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
205: _LL2DC: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 206: _LL2DD:{
struct _tuple23*_tmpBF8;yyval=Cyc_YY39(((_tmpBF8=_cycalloc(sizeof(*_tmpBF8)),((
_tmpBF8->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpBF8->f2=0,((_tmpBF8->f3=0,((_tmpBF8->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBF8->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBF8)))))))))))));}
break;case 207: _LL2DE:{struct _tuple23*_tmpBF9;yyval=Cyc_YY39(((_tmpBF9=_cycalloc(
sizeof(*_tmpBF9)),((_tmpBF9->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),((_tmpBF9->f2=1,((_tmpBF9->f3=0,((_tmpBF9->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBF9->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBF9)))))))))))));}
break;case 208: _LL2DF: {struct _tuple8 _tmp5F3;struct Cyc_Core_Opt*_tmp5F4;struct Cyc_Absyn_Tqual
_tmp5F5;void*_tmp5F6;struct _tuple8*_tmp5F2=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5F3=*_tmp5F2;_tmp5F4=
_tmp5F3.f1;_tmp5F5=_tmp5F3.f2;_tmp5F6=_tmp5F3.f3;{struct Cyc_Absyn_VarargInfo*
_tmpBFA;struct Cyc_Absyn_VarargInfo*_tmp5F7=(_tmpBFA=_cycalloc(sizeof(*_tmpBFA)),((
_tmpBFA->name=_tmp5F4,((_tmpBFA->tq=_tmp5F5,((_tmpBFA->type=_tmp5F6,((_tmpBFA->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpBFA)))))))));{struct _tuple23*_tmpBFB;yyval=Cyc_YY39(((_tmpBFB=_cycalloc(
sizeof(*_tmpBFB)),((_tmpBFB->f1=0,((_tmpBFB->f2=0,((_tmpBFB->f3=_tmp5F7,((
_tmpBFB->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpBFB->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBFB)))))))))))));}
break;}}case 209: _LL2E0: {struct _tuple8 _tmp5FB;struct Cyc_Core_Opt*_tmp5FC;struct
Cyc_Absyn_Tqual _tmp5FD;void*_tmp5FE;struct _tuple8*_tmp5FA=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5FB=*_tmp5FA;_tmp5FC=
_tmp5FB.f1;_tmp5FD=_tmp5FB.f2;_tmp5FE=_tmp5FB.f3;{struct Cyc_Absyn_VarargInfo*
_tmpBFC;struct Cyc_Absyn_VarargInfo*_tmp5FF=(_tmpBFC=_cycalloc(sizeof(*_tmpBFC)),((
_tmpBFC->name=_tmp5FC,((_tmpBFC->tq=_tmp5FD,((_tmpBFC->type=_tmp5FE,((_tmpBFC->inject=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpBFC)))))))));{struct _tuple23*_tmpBFD;yyval=Cyc_YY39(((_tmpBFD=_cycalloc(
sizeof(*_tmpBFD)),((_tmpBFD->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)])),((_tmpBFD->f2=0,((_tmpBFD->f3=_tmp5FF,((_tmpBFD->f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBFD->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBFD)))))))))))));}
break;}}case 210: _LL2E1:{struct Cyc_Absyn_Unknown_kb_struct _tmpC00;struct Cyc_Absyn_Unknown_kb_struct*
_tmpBFF;yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)((_tmpBFF=_cycalloc(
sizeof(*_tmpBFF)),((_tmpBFF[0]=((_tmpC00.tag=1,((_tmpC00.f1=0,_tmpC00)))),
_tmpBFF))))));}break;case 211: _LL2E2: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 212: _LL2E3: yyval=Cyc_YY49(0);break;case 213: _LL2E4:{struct Cyc_Absyn_JoinEff_struct*
_tmpC06;struct Cyc_Absyn_JoinEff_struct _tmpC05;struct Cyc_Core_Opt*_tmpC04;yyval=
Cyc_YY49(((_tmpC04=_cycalloc(sizeof(*_tmpC04)),((_tmpC04->v=(void*)((_tmpC06=
_cycalloc(sizeof(*_tmpC06)),((_tmpC06[0]=((_tmpC05.tag=24,((_tmpC05.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC05)))),_tmpC06)))),
_tmpC04)))));}break;case 214: _LL2E5: yyval=Cyc_YY50(0);break;case 215: _LL2E6: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 216: _LL2E7: {
struct Cyc_Absyn_Less_kb_struct _tmpC09;struct Cyc_Absyn_Less_kb_struct*_tmpC08;
struct Cyc_Absyn_Less_kb_struct*_tmp607=(_tmpC08=_cycalloc(sizeof(*_tmpC08)),((
_tmpC08[0]=((_tmpC09.tag=2,((_tmpC09.f1=0,((_tmpC09.f2=Cyc_Absyn_TopRgnKind,
_tmpC09)))))),_tmpC08)));struct _dyneither_ptr _tmp608=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp609=Cyc_Parse_id2type(
_tmp608,(void*)_tmp607);{struct _tuple10*_tmpC13;struct Cyc_Absyn_JoinEff_struct
_tmpC12;struct Cyc_Absyn_JoinEff_struct*_tmpC11;struct Cyc_List_List*_tmpC10;yyval=
Cyc_YY50(((_tmpC10=_cycalloc(sizeof(*_tmpC10)),((_tmpC10->hd=((_tmpC13=_cycalloc(
sizeof(*_tmpC13)),((_tmpC13->f1=(void*)((_tmpC11=_cycalloc(sizeof(*_tmpC11)),((
_tmpC11[0]=((_tmpC12.tag=24,((_tmpC12.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC12)))),_tmpC11)))),((
_tmpC13->f2=_tmp609,_tmpC13)))))),((_tmpC10->tl=0,_tmpC10)))))));}break;}case 217:
_LL2E8: {struct Cyc_Absyn_Less_kb_struct _tmpC16;struct Cyc_Absyn_Less_kb_struct*
_tmpC15;struct Cyc_Absyn_Less_kb_struct*_tmp610=(_tmpC15=_cycalloc(sizeof(*
_tmpC15)),((_tmpC15[0]=((_tmpC16.tag=2,((_tmpC16.f1=0,((_tmpC16.f2=Cyc_Absyn_TopRgnKind,
_tmpC16)))))),_tmpC15)));struct _dyneither_ptr _tmp611=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp612=Cyc_Parse_id2type(
_tmp611,(void*)_tmp610);{struct _tuple10*_tmpC20;struct Cyc_Absyn_JoinEff_struct
_tmpC1F;struct Cyc_Absyn_JoinEff_struct*_tmpC1E;struct Cyc_List_List*_tmpC1D;yyval=
Cyc_YY50(((_tmpC1D=_cycalloc(sizeof(*_tmpC1D)),((_tmpC1D->hd=((_tmpC20=_cycalloc(
sizeof(*_tmpC20)),((_tmpC20->f1=(void*)((_tmpC1E=_cycalloc(sizeof(*_tmpC1E)),((
_tmpC1E[0]=((_tmpC1F.tag=24,((_tmpC1F.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpC1F)))),_tmpC1E)))),((
_tmpC20->f2=_tmp612,_tmpC20)))))),((_tmpC1D->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC1D)))))));}break;}case
218: _LL2E9: yyval=Cyc_YY31(0);break;case 219: _LL2EA:{const char*_tmpC21;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC21="inject",_tag_dyneither(_tmpC21,sizeof(char),7))))
!= 0){const char*_tmpC22;Cyc_Parse_err(((_tmpC22="missing type in function declaration",
_tag_dyneither(_tmpC22,sizeof(char),37))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}yyval=Cyc_YY31(
1);break;case 220: _LL2EB: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 221: _LL2EC: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 222: _LL2ED:
yyval=Cyc_YY40(0);break;case 223: _LL2EE: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 224: _LL2EF:{struct Cyc_Absyn_RgnsEff_struct*
_tmpC28;struct Cyc_Absyn_RgnsEff_struct _tmpC27;struct Cyc_List_List*_tmpC26;yyval=
Cyc_YY40(((_tmpC26=_cycalloc(sizeof(*_tmpC26)),((_tmpC26->hd=(void*)((_tmpC28=
_cycalloc(sizeof(*_tmpC28)),((_tmpC28[0]=((_tmpC27.tag=25,((_tmpC27.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpC27)))),_tmpC28)))),((_tmpC26->tl=0,_tmpC26)))))));}break;case 225: _LL2F0: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_EffKind,
0);{struct Cyc_List_List*_tmpC29;yyval=Cyc_YY40(((_tmpC29=_cycalloc(sizeof(*
_tmpC29)),((_tmpC29->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpC29->tl=0,_tmpC29)))))));}break;case 226: _LL2F1: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_TopRgnKind,
1);{struct Cyc_Absyn_AccessEff_struct*_tmpC2F;struct Cyc_Absyn_AccessEff_struct
_tmpC2E;struct Cyc_List_List*_tmpC2D;yyval=Cyc_YY40(((_tmpC2D=_cycalloc(sizeof(*
_tmpC2D)),((_tmpC2D->hd=(void*)((_tmpC2F=_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F[0]=((
_tmpC2E.tag=23,((_tmpC2E.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC2E)))),_tmpC2F)))),((
_tmpC2D->tl=0,_tmpC2D)))))));}break;case 227: _LL2F2: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Absyn_TopRgnKind,
1);{struct Cyc_Absyn_AccessEff_struct*_tmpC35;struct Cyc_Absyn_AccessEff_struct
_tmpC34;struct Cyc_List_List*_tmpC33;yyval=Cyc_YY40(((_tmpC33=_cycalloc(sizeof(*
_tmpC33)),((_tmpC33->hd=(void*)((_tmpC35=_cycalloc(sizeof(*_tmpC35)),((_tmpC35[0]=((
_tmpC34.tag=23,((_tmpC34.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC34)))),_tmpC35)))),((
_tmpC33->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpC33)))))));}break;case 228: _LL2F3:{struct Cyc_List_List*
_tmpC36;yyval=Cyc_YY38(((_tmpC36=_cycalloc(sizeof(*_tmpC36)),((_tmpC36->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC36->tl=0,
_tmpC36)))))));}break;case 229: _LL2F4:{struct Cyc_List_List*_tmpC37;yyval=Cyc_YY38(((
_tmpC37=_cycalloc(sizeof(*_tmpC37)),((_tmpC37->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC37->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC37)))))));}
break;case 230: _LL2F5: {struct _tuple22 _tmp628;struct Cyc_Absyn_Tqual _tmp629;struct
Cyc_List_List*_tmp62A;struct Cyc_List_List*_tmp62B;struct _tuple22*_tmp627=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp628=*_tmp627;
_tmp629=_tmp628.f1;_tmp62A=_tmp628.f2;_tmp62B=_tmp628.f3;if(_tmp629.loc == 0)
_tmp629.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp62D;struct _tuple1*
_tmp62E;struct Cyc_List_List*_tmp62F;struct Cyc_Parse_Declarator*_tmp62C=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp62D=*_tmp62C;
_tmp62E=_tmp62D.id;_tmp62F=_tmp62D.tms;{void*_tmp630=Cyc_Parse_speclist2typ(
_tmp62A,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp632;void*_tmp633;struct
Cyc_List_List*_tmp634;struct Cyc_List_List*_tmp635;struct _tuple12 _tmp631=Cyc_Parse_apply_tms(
_tmp629,_tmp630,_tmp62B,_tmp62F);_tmp632=_tmp631.f1;_tmp633=_tmp631.f2;_tmp634=
_tmp631.f3;_tmp635=_tmp631.f4;if(_tmp634 != 0){const char*_tmpC38;Cyc_Parse_err(((
_tmpC38="parameter with bad type params",_tag_dyneither(_tmpC38,sizeof(char),31))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(_tmp62E)){const char*
_tmpC39;Cyc_Parse_err(((_tmpC39="parameter cannot be qualified with a namespace",
_tag_dyneither(_tmpC39,sizeof(char),47))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpC3A;struct Cyc_Core_Opt*_tmp638=(struct Cyc_Core_Opt*)((_tmpC3A=_cycalloc(
sizeof(*_tmpC3A)),((_tmpC3A->v=(*_tmp62E).f2,_tmpC3A))));if(_tmp635 != 0){const
char*_tmpC3D;void*_tmpC3C;(_tmpC3C=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC3D="extra attributes on parameter, ignoring",
_tag_dyneither(_tmpC3D,sizeof(char),40))),_tag_dyneither(_tmpC3C,sizeof(void*),0)));}{
struct _tuple8*_tmpC3E;yyval=Cyc_YY37(((_tmpC3E=_cycalloc(sizeof(*_tmpC3E)),((
_tmpC3E->f1=_tmp638,((_tmpC3E->f2=_tmp632,((_tmpC3E->f3=_tmp633,_tmpC3E)))))))));}
break;}}}}case 231: _LL2F6: {struct _tuple22 _tmp63E;struct Cyc_Absyn_Tqual _tmp63F;
struct Cyc_List_List*_tmp640;struct Cyc_List_List*_tmp641;struct _tuple22*_tmp63D=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63E=*
_tmp63D;_tmp63F=_tmp63E.f1;_tmp640=_tmp63E.f2;_tmp641=_tmp63E.f3;if(_tmp63F.loc
== 0)_tmp63F.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);{void*_tmp642=
Cyc_Parse_speclist2typ(_tmp640,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp641 != 0){
const char*_tmpC41;void*_tmpC40;(_tmpC40=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC41="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpC41,sizeof(char),38))),_tag_dyneither(_tmpC40,sizeof(void*),0)));}{
struct _tuple8*_tmpC42;yyval=Cyc_YY37(((_tmpC42=_cycalloc(sizeof(*_tmpC42)),((
_tmpC42->f1=0,((_tmpC42->f2=_tmp63F,((_tmpC42->f3=_tmp642,_tmpC42)))))))));}
break;}}case 232: _LL2F7: {struct _tuple22 _tmp647;struct Cyc_Absyn_Tqual _tmp648;
struct Cyc_List_List*_tmp649;struct Cyc_List_List*_tmp64A;struct _tuple22*_tmp646=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp647=*_tmp646;_tmp648=_tmp647.f1;_tmp649=_tmp647.f2;_tmp64A=_tmp647.f3;if(
_tmp648.loc == 0)_tmp648.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp64B=
Cyc_Parse_speclist2typ(_tmp649,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp64C=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp64E;void*_tmp64F;struct Cyc_List_List*_tmp650;struct Cyc_List_List*
_tmp651;struct _tuple12 _tmp64D=Cyc_Parse_apply_tms(_tmp648,_tmp64B,_tmp64A,
_tmp64C);_tmp64E=_tmp64D.f1;_tmp64F=_tmp64D.f2;_tmp650=_tmp64D.f3;_tmp651=
_tmp64D.f4;if(_tmp650 != 0){const char*_tmpC45;void*_tmpC44;(_tmpC44=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpC45="bad type parameters on formal argument, ignoring",
_tag_dyneither(_tmpC45,sizeof(char),49))),_tag_dyneither(_tmpC44,sizeof(void*),0)));}
if(_tmp651 != 0){const char*_tmpC48;void*_tmpC47;(_tmpC47=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC48="bad attributes on parameter, ignoring",
_tag_dyneither(_tmpC48,sizeof(char),38))),_tag_dyneither(_tmpC47,sizeof(void*),0)));}{
struct _tuple8*_tmpC49;yyval=Cyc_YY37(((_tmpC49=_cycalloc(sizeof(*_tmpC49)),((
_tmpC49->f1=0,((_tmpC49->f2=_tmp64E,((_tmpC49->f3=_tmp64F,_tmpC49)))))))));}
break;}}case 233: _LL2F8: yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 234: _LL2F9:{struct _dyneither_ptr*_tmpC4C;struct Cyc_List_List*
_tmpC4B;yyval=Cyc_YY36(((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((_tmpC4B->hd=((
_tmpC4C=_cycalloc(sizeof(*_tmpC4C)),((_tmpC4C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC4C)))),((_tmpC4B->tl=0,
_tmpC4B)))))));}break;case 235: _LL2FA:{struct _dyneither_ptr*_tmpC4F;struct Cyc_List_List*
_tmpC4E;yyval=Cyc_YY36(((_tmpC4E=_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E->hd=((
_tmpC4F=_cycalloc(sizeof(*_tmpC4F)),((_tmpC4F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC4F)))),((_tmpC4E->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpC4E)))))));}break;case 236: _LL2FB: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 237: _LL2FC: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 238: _LL2FD:{struct
Cyc_Absyn_UnresolvedMem_e_struct _tmpC52;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpC51;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpC51=_cycalloc(sizeof(*
_tmpC51)),((_tmpC51[0]=((_tmpC52.tag=37,((_tmpC52.f1=0,((_tmpC52.f2=0,_tmpC52)))))),
_tmpC51)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 239: _LL2FE:{struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpC55;struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpC54;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC55.tag=37,((
_tmpC55.f1=0,((_tmpC55.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),
_tmpC55)))))),_tmpC54)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 240:
_LL2FF:{struct Cyc_Absyn_UnresolvedMem_e_struct _tmpC58;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpC57;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpC57=_cycalloc(sizeof(*
_tmpC57)),((_tmpC57[0]=((_tmpC58.tag=37,((_tmpC58.f1=0,((_tmpC58.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpC58)))))),_tmpC57)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 241: _LL300: {struct _dyneither_ptr*
_tmpC5B;struct _tuple1*_tmpC5A;struct Cyc_Absyn_Vardecl*_tmp661=Cyc_Absyn_new_vardecl(((
_tmpC5A=_cycalloc(sizeof(*_tmpC5A)),((_tmpC5A->f1=Cyc_Absyn_Loc_n,((_tmpC5A->f2=((
_tmpC5B=_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpC5B)))),_tmpC5A)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp661->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct _tmpC5E;struct Cyc_Absyn_Comprehension_e_struct*
_tmpC5D;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpC5D=_cycalloc(sizeof(*
_tmpC5D)),((_tmpC5D[0]=((_tmpC5E.tag=29,((_tmpC5E.f1=_tmp661,((_tmpC5E.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpC5E.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC5E.f4=0,
_tmpC5E)))))))))),_tmpC5D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
242: _LL301:{struct _tuple26*_tmpC61;struct Cyc_List_List*_tmpC60;yyval=Cyc_YY6(((
_tmpC60=_cycalloc(sizeof(*_tmpC60)),((_tmpC60->hd=((_tmpC61=_cycalloc(sizeof(*
_tmpC61)),((_tmpC61->f1=0,((_tmpC61->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC61)))))),((_tmpC60->tl=
0,_tmpC60)))))));}break;case 243: _LL302:{struct _tuple26*_tmpC64;struct Cyc_List_List*
_tmpC63;yyval=Cyc_YY6(((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63->hd=((
_tmpC64=_cycalloc(sizeof(*_tmpC64)),((_tmpC64->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC64->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC64)))))),((
_tmpC63->tl=0,_tmpC63)))))));}break;case 244: _LL303:{struct _tuple26*_tmpC67;
struct Cyc_List_List*_tmpC66;yyval=Cyc_YY6(((_tmpC66=_cycalloc(sizeof(*_tmpC66)),((
_tmpC66->hd=((_tmpC67=_cycalloc(sizeof(*_tmpC67)),((_tmpC67->f1=0,((_tmpC67->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC67)))))),((
_tmpC66->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpC66)))))));}break;case 245: _LL304:{struct _tuple26*_tmpC6A;struct Cyc_List_List*
_tmpC69;yyval=Cyc_YY6(((_tmpC69=_cycalloc(sizeof(*_tmpC69)),((_tmpC69->hd=((
_tmpC6A=_cycalloc(sizeof(*_tmpC6A)),((_tmpC6A->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC6A->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC6A)))))),((
_tmpC69->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpC69)))))));}break;case 246: _LL305: yyval=Cyc_YY41(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 247: _LL306:{
struct Cyc_List_List*_tmpC6B;yyval=Cyc_YY41(((_tmpC6B=_cycalloc(sizeof(*_tmpC6B)),((
_tmpC6B->hd=Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpC6B->tl=0,_tmpC6B)))))));}break;case 248: _LL307:{struct Cyc_List_List*
_tmpC6C;yyval=Cyc_YY41(((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->hd=Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC6C->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC6C)))))));}
break;case 249: _LL308:{struct Cyc_Absyn_ArrayElement_struct _tmpC6F;struct Cyc_Absyn_ArrayElement_struct*
_tmpC6E;yyval=Cyc_YY42((void*)((_tmpC6E=_cycalloc(sizeof(*_tmpC6E)),((_tmpC6E[0]=((
_tmpC6F.tag=0,((_tmpC6F.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpC6F)))),_tmpC6E)))));}break;case 250: _LL309:{struct
Cyc_Absyn_FieldName_struct _tmpC75;struct _dyneither_ptr*_tmpC74;struct Cyc_Absyn_FieldName_struct*
_tmpC73;yyval=Cyc_YY42((void*)((_tmpC73=_cycalloc(sizeof(*_tmpC73)),((_tmpC73[0]=((
_tmpC75.tag=1,((_tmpC75.f1=((_tmpC74=_cycalloc(sizeof(*_tmpC74)),((_tmpC74[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpC74)))),_tmpC75)))),_tmpC73)))));}break;case 251: _LL30A: {struct _tuple22
_tmp676;struct Cyc_Absyn_Tqual _tmp677;struct Cyc_List_List*_tmp678;struct Cyc_List_List*
_tmp679;struct _tuple22*_tmp675=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp676=*_tmp675;_tmp677=
_tmp676.f1;_tmp678=_tmp676.f2;_tmp679=_tmp676.f3;{void*_tmp67A=Cyc_Parse_speclist2typ(
_tmp678,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp679 != 0){const char*_tmpC78;void*_tmpC77;(
_tmpC77=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC78="ignoring attributes in type",
_tag_dyneither(_tmpC78,sizeof(char),28))),_tag_dyneither(_tmpC77,sizeof(void*),0)));}{
struct _tuple8*_tmpC79;yyval=Cyc_YY37(((_tmpC79=_cycalloc(sizeof(*_tmpC79)),((
_tmpC79->f1=0,((_tmpC79->f2=_tmp677,((_tmpC79->f3=_tmp67A,_tmpC79)))))))));}
break;}}case 252: _LL30B: {struct _tuple22 _tmp67F;struct Cyc_Absyn_Tqual _tmp680;
struct Cyc_List_List*_tmp681;struct Cyc_List_List*_tmp682;struct _tuple22*_tmp67E=
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp67F=*_tmp67E;_tmp680=_tmp67F.f1;_tmp681=_tmp67F.f2;_tmp682=_tmp67F.f3;{void*
_tmp683=Cyc_Parse_speclist2typ(_tmp681,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp684=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple12 _tmp685=Cyc_Parse_apply_tms(_tmp680,_tmp683,_tmp682,_tmp684);if(
_tmp685.f3 != 0){const char*_tmpC7C;void*_tmpC7B;(_tmpC7B=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC7C="bad type params, ignoring",
_tag_dyneither(_tmpC7C,sizeof(char),26))),_tag_dyneither(_tmpC7B,sizeof(void*),0)));}
if(_tmp685.f4 != 0){const char*_tmpC7F;void*_tmpC7E;(_tmpC7E=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpC7F="bad specifiers, ignoring",
_tag_dyneither(_tmpC7F,sizeof(char),25))),_tag_dyneither(_tmpC7E,sizeof(void*),0)));}{
struct _tuple8*_tmpC80;yyval=Cyc_YY37(((_tmpC80=_cycalloc(sizeof(*_tmpC80)),((
_tmpC80->f1=0,((_tmpC80->f2=_tmp685.f1,((_tmpC80->f3=_tmp685.f2,_tmpC80)))))))));}
break;}}case 253: _LL30C: yyval=Cyc_YY44((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);break;case 254: _LL30D:{
struct Cyc_Absyn_JoinEff_struct _tmpC83;struct Cyc_Absyn_JoinEff_struct*_tmpC82;
yyval=Cyc_YY44((void*)((_tmpC82=_cycalloc(sizeof(*_tmpC82)),((_tmpC82[0]=((
_tmpC83.tag=24,((_tmpC83.f1=0,_tmpC83)))),_tmpC82)))));}break;case 255: _LL30E:{
struct Cyc_Absyn_JoinEff_struct _tmpC86;struct Cyc_Absyn_JoinEff_struct*_tmpC85;
yyval=Cyc_YY44((void*)((_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85[0]=((
_tmpC86.tag=24,((_tmpC86.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpC86)))),_tmpC85)))));}break;case 256: _LL30F:{struct
Cyc_Absyn_RgnsEff_struct _tmpC89;struct Cyc_Absyn_RgnsEff_struct*_tmpC88;yyval=Cyc_YY44((
void*)((_tmpC88=_cycalloc(sizeof(*_tmpC88)),((_tmpC88[0]=((_tmpC89.tag=25,((
_tmpC89.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpC89)))),_tmpC88)))));}break;case 257: _LL310:{struct Cyc_Absyn_JoinEff_struct
_tmpC8F;struct Cyc_List_List*_tmpC8E;struct Cyc_Absyn_JoinEff_struct*_tmpC8D;yyval=
Cyc_YY44((void*)((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((_tmpC8D[0]=((_tmpC8F.tag=
24,((_tmpC8F.f1=((_tmpC8E=_cycalloc(sizeof(*_tmpC8E)),((_tmpC8E->hd=Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC8E->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC8E)))))),_tmpC8F)))),
_tmpC8D)))));}break;case 258: _LL311:{struct Cyc_List_List*_tmpC90;yyval=Cyc_YY40(((
_tmpC90=_cycalloc(sizeof(*_tmpC90)),((_tmpC90->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC90->tl=0,_tmpC90)))))));}
break;case 259: _LL312:{struct Cyc_List_List*_tmpC91;yyval=Cyc_YY40(((_tmpC91=
_cycalloc(sizeof(*_tmpC91)),((_tmpC91->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC91->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC91)))))));}
break;case 260: _LL313:{struct Cyc_Parse_Abstractdeclarator*_tmpC92;yyval=Cyc_YY30(((
_tmpC92=_cycalloc(sizeof(*_tmpC92)),((_tmpC92->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC92)))));}break;case 261:
_LL314: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
262: _LL315:{struct Cyc_Parse_Abstractdeclarator*_tmpC93;yyval=Cyc_YY30(((_tmpC93=
_cycalloc(sizeof(*_tmpC93)),((_tmpC93->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpC93)))));}break;
case 263: _LL316: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 264: _LL317:{struct Cyc_List_List*_tmpC9D;struct Cyc_Absyn_Carray_mod_struct
_tmpC9C;struct Cyc_Absyn_Carray_mod_struct*_tmpC9B;struct Cyc_Parse_Abstractdeclarator*
_tmpC9A;yyval=Cyc_YY30(((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A->tms=((
_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->hd=(void*)((_tmpC9B=_cycalloc(
sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9C.tag=0,((_tmpC9C.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC9C.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpC9C)))))),
_tmpC9B)))),((_tmpC9D->tl=0,_tmpC9D)))))),_tmpC9A)))));}break;case 265: _LL318:{
struct Cyc_List_List*_tmpCA7;struct Cyc_Absyn_Carray_mod_struct _tmpCA6;struct Cyc_Absyn_Carray_mod_struct*
_tmpCA5;struct Cyc_Parse_Abstractdeclarator*_tmpCA4;yyval=Cyc_YY30(((_tmpCA4=
_cycalloc(sizeof(*_tmpCA4)),((_tmpCA4->tms=((_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((
_tmpCA7->hd=(void*)((_tmpCA5=_cycalloc(sizeof(*_tmpCA5)),((_tmpCA5[0]=((_tmpCA6.tag=
0,((_tmpCA6.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCA6.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCA6)))))),
_tmpCA5)))),((_tmpCA7->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpCA7)))))),_tmpCA4)))));}break;case 266: _LL319:{
struct Cyc_List_List*_tmpCB1;struct Cyc_Absyn_ConstArray_mod_struct _tmpCB0;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpCAF;struct Cyc_Parse_Abstractdeclarator*
_tmpCAE;yyval=Cyc_YY30(((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->tms=((
_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->hd=(void*)((_tmpCAF=_cycalloc(
sizeof(*_tmpCAF)),((_tmpCAF[0]=((_tmpCB0.tag=1,((_tmpCB0.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCB0.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCB0.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCB0)))))))),
_tmpCAF)))),((_tmpCB1->tl=0,_tmpCB1)))))),_tmpCAE)))));}break;case 267: _LL31A:{
struct Cyc_List_List*_tmpCBB;struct Cyc_Absyn_ConstArray_mod_struct _tmpCBA;struct
Cyc_Absyn_ConstArray_mod_struct*_tmpCB9;struct Cyc_Parse_Abstractdeclarator*
_tmpCB8;yyval=Cyc_YY30(((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8->tms=((
_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((_tmpCBB->hd=(void*)((_tmpCB9=_cycalloc(
sizeof(*_tmpCB9)),((_tmpCB9[0]=((_tmpCBA.tag=1,((_tmpCBA.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCBA.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCBA.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCBA)))))))),
_tmpCB9)))),((_tmpCBB->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpCBB)))))),_tmpCB8)))));}break;case 268: _LL31B:{
struct Cyc_List_List*_tmpCD0;struct Cyc_Absyn_Function_mod_struct _tmpCCF;struct Cyc_Absyn_WithTypes_struct
_tmpCCE;struct Cyc_Absyn_WithTypes_struct*_tmpCCD;struct Cyc_Absyn_Function_mod_struct*
_tmpCCC;struct Cyc_Parse_Abstractdeclarator*_tmpCCB;yyval=Cyc_YY30(((_tmpCCB=
_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->tms=((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((
_tmpCD0->hd=(void*)((_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC[0]=((_tmpCCF.tag=
3,((_tmpCCF.f1=(void*)((void*)((_tmpCCD=_cycalloc(sizeof(*_tmpCCD)),((_tmpCCD[0]=((
_tmpCCE.tag=1,((_tmpCCE.f1=0,((_tmpCCE.f2=0,((_tmpCCE.f3=0,((_tmpCCE.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCCE.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCCE)))))))))))),
_tmpCCD))))),_tmpCCF)))),_tmpCCC)))),((_tmpCD0->tl=0,_tmpCD0)))))),_tmpCCB)))));}
break;case 269: _LL31C: {struct _tuple23 _tmp6AF;struct Cyc_List_List*_tmp6B0;int
_tmp6B1;struct Cyc_Absyn_VarargInfo*_tmp6B2;struct Cyc_Core_Opt*_tmp6B3;struct Cyc_List_List*
_tmp6B4;struct _tuple23*_tmp6AE=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6AF=*_tmp6AE;_tmp6B0=
_tmp6AF.f1;_tmp6B1=_tmp6AF.f2;_tmp6B2=_tmp6AF.f3;_tmp6B3=_tmp6AF.f4;_tmp6B4=
_tmp6AF.f5;{struct Cyc_List_List*_tmpCE5;struct Cyc_Absyn_Function_mod_struct
_tmpCE4;struct Cyc_Absyn_WithTypes_struct _tmpCE3;struct Cyc_Absyn_WithTypes_struct*
_tmpCE2;struct Cyc_Absyn_Function_mod_struct*_tmpCE1;struct Cyc_Parse_Abstractdeclarator*
_tmpCE0;yyval=Cyc_YY30(((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->tms=((
_tmpCE5=_cycalloc(sizeof(*_tmpCE5)),((_tmpCE5->hd=(void*)((_tmpCE1=_cycalloc(
sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE4.tag=3,((_tmpCE4.f1=(void*)((void*)((
_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((_tmpCE2[0]=((_tmpCE3.tag=1,((_tmpCE3.f1=
_tmp6B0,((_tmpCE3.f2=_tmp6B1,((_tmpCE3.f3=_tmp6B2,((_tmpCE3.f4=_tmp6B3,((_tmpCE3.f5=
_tmp6B4,_tmpCE3)))))))))))),_tmpCE2))))),_tmpCE4)))),_tmpCE1)))),((_tmpCE5->tl=0,
_tmpCE5)))))),_tmpCE0)))));}break;}case 270: _LL31D:{struct Cyc_List_List*_tmpCFA;
struct Cyc_Absyn_Function_mod_struct _tmpCF9;struct Cyc_Absyn_WithTypes_struct
_tmpCF8;struct Cyc_Absyn_WithTypes_struct*_tmpCF7;struct Cyc_Absyn_Function_mod_struct*
_tmpCF6;struct Cyc_Parse_Abstractdeclarator*_tmpCF5;yyval=Cyc_YY30(((_tmpCF5=
_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5->tms=((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((
_tmpCFA->hd=(void*)((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6[0]=((_tmpCF9.tag=
3,((_tmpCF9.f1=(void*)((void*)((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7[0]=((
_tmpCF8.tag=1,((_tmpCF8.f1=0,((_tmpCF8.f2=0,((_tmpCF8.f3=0,((_tmpCF8.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCF8.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCF8)))))))))))),
_tmpCF7))))),_tmpCF9)))),_tmpCF6)))),((_tmpCFA->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpCFA)))))),
_tmpCF5)))));}break;case 271: _LL31E: {struct _tuple23 _tmp6C2;struct Cyc_List_List*
_tmp6C3;int _tmp6C4;struct Cyc_Absyn_VarargInfo*_tmp6C5;struct Cyc_Core_Opt*_tmp6C6;
struct Cyc_List_List*_tmp6C7;struct _tuple23*_tmp6C1=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C2=*_tmp6C1;_tmp6C3=
_tmp6C2.f1;_tmp6C4=_tmp6C2.f2;_tmp6C5=_tmp6C2.f3;_tmp6C6=_tmp6C2.f4;_tmp6C7=
_tmp6C2.f5;{struct Cyc_List_List*_tmpD0F;struct Cyc_Absyn_Function_mod_struct
_tmpD0E;struct Cyc_Absyn_WithTypes_struct _tmpD0D;struct Cyc_Absyn_WithTypes_struct*
_tmpD0C;struct Cyc_Absyn_Function_mod_struct*_tmpD0B;struct Cyc_Parse_Abstractdeclarator*
_tmpD0A;yyval=Cyc_YY30(((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A->tms=((
_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->hd=(void*)((_tmpD0B=_cycalloc(
sizeof(*_tmpD0B)),((_tmpD0B[0]=((_tmpD0E.tag=3,((_tmpD0E.f1=(void*)((void*)((
_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C[0]=((_tmpD0D.tag=1,((_tmpD0D.f1=
_tmp6C3,((_tmpD0D.f2=_tmp6C4,((_tmpD0D.f3=_tmp6C5,((_tmpD0D.f4=_tmp6C6,((_tmpD0D.f5=
_tmp6C7,_tmpD0D)))))))))))),_tmpD0C))))),_tmpD0E)))),_tmpD0B)))),((_tmpD0F->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpD0F)))))),_tmpD0A)))));}break;}case 272: _LL31F: {struct Cyc_List_List*_tmp6CE=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_List_List*_tmpD19;struct Cyc_Absyn_TypeParams_mod_struct _tmpD18;struct
Cyc_Absyn_TypeParams_mod_struct*_tmpD17;struct Cyc_Parse_Abstractdeclarator*
_tmpD16;yyval=Cyc_YY30(((_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16->tms=((
_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19->hd=(void*)((_tmpD17=_cycalloc(
sizeof(*_tmpD17)),((_tmpD17[0]=((_tmpD18.tag=4,((_tmpD18.f1=_tmp6CE,((_tmpD18.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpD18.f3=0,_tmpD18)))))))),_tmpD17)))),((_tmpD19->tl=(
Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpD19)))))),_tmpD16)))));}break;}case 273: _LL320:{struct Cyc_List_List*_tmpD23;
struct Cyc_Absyn_Attributes_mod_struct _tmpD22;struct Cyc_Absyn_Attributes_mod_struct*
_tmpD21;struct Cyc_Parse_Abstractdeclarator*_tmpD20;yyval=Cyc_YY30(((_tmpD20=
_cycalloc(sizeof(*_tmpD20)),((_tmpD20->tms=((_tmpD23=_cycalloc(sizeof(*_tmpD23)),((
_tmpD23->hd=(void*)((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21[0]=((_tmpD22.tag=
5,((_tmpD22.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpD22.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD22)))))),_tmpD21)))),((
_tmpD23->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpD23)))))),_tmpD20)))));}break;case 274: _LL321: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 275: _LL322:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 276:
_LL323: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
277: _LL324: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 278: _LL325: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 279: _LL326: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 280: _LL327:{struct Cyc_Absyn_ResetRegion_s_struct _tmpD26;
struct Cyc_Absyn_ResetRegion_s_struct*_tmpD25;yyval=Cyc_YY9(Cyc_Absyn_new_stmt((
void*)((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25[0]=((_tmpD26.tag=16,((
_tmpD26.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]),_tmpD26)))),_tmpD25)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 281:
_LL328: yyval=Cyc_YY4(0);break;case 282: _LL329:{const char*_tmpD27;if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmpD27="open",_tag_dyneither(_tmpD27,sizeof(char),5))))
!= 0){const char*_tmpD28;Cyc_Parse_err(((_tmpD28="expecting `open'",
_tag_dyneither(_tmpD28,sizeof(char),17))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 283: _LL32A:{struct Cyc_Absyn_Label_s_struct _tmpD2E;
struct _dyneither_ptr*_tmpD2D;struct Cyc_Absyn_Label_s_struct*_tmpD2C;yyval=Cyc_YY9(
Cyc_Absyn_new_stmt((void*)((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C[0]=((
_tmpD2E.tag=13,((_tmpD2E.f1=((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpD2D)))),((_tmpD2E.f2=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpD2E)))))),_tmpD2C)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 284:
_LL32B: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 285:
_LL32C: yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 286:
_LL32D: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 287:
_LL32E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 288: _LL32F: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0)));
break;case 289: _LL330: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 290: _LL331:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 291:
_LL332: yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 292:
_LL333:{struct Cyc_Absyn_Fn_d_struct _tmpD31;struct Cyc_Absyn_Fn_d_struct*_tmpD30;
yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD30=
_cycalloc(sizeof(*_tmpD30)),((_tmpD30[0]=((_tmpD31.tag=1,((_tmpD31.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD31)))),_tmpD30)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0)));}break;case 293: _LL334:{
struct Cyc_Absyn_Fn_d_struct _tmpD34;struct Cyc_Absyn_Fn_d_struct*_tmpD33;yyval=Cyc_YY9(
Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpD33=_cycalloc(sizeof(*
_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=1,((_tmpD34.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD34)))),_tmpD33)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));}break;case 294: _LL335: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 295: _LL336: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 296:
_LL337: yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 297:
_LL338: {struct Cyc_Absyn_UnknownId_e_struct _tmpD37;struct Cyc_Absyn_UnknownId_e_struct*
_tmpD36;struct Cyc_Absyn_Exp*_tmp6E2=Cyc_Absyn_new_exp((void*)((_tmpD36=_cycalloc(
sizeof(*_tmpD36)),((_tmpD36[0]=((_tmpD37.tag=2,((_tmpD37.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpD37)))),_tmpD36)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp6E2,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 298:
_LL339: {struct Cyc_Absyn_Exp*_tmp6E5=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp6E5,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 299:
_LL33A: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 300:
_LL33B: yyval=Cyc_YY10(0);break;case 301: _LL33C:{struct Cyc_Absyn_Switch_clause*
_tmpD3A;struct Cyc_List_List*_tmpD39;yyval=Cyc_YY10(((_tmpD39=_cycalloc(sizeof(*
_tmpD39)),((_tmpD39->hd=((_tmpD3A=_cycalloc(sizeof(*_tmpD3A)),((_tmpD3A->pattern=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmpD3A->pat_vars=
0,((_tmpD3A->where_clause=0,((_tmpD3A->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD3A->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD3A)))))))))))),((
_tmpD39->tl=0,_tmpD39)))))));}break;case 302: _LL33D:{struct Cyc_Absyn_Switch_clause*
_tmpD3D;struct Cyc_List_List*_tmpD3C;yyval=Cyc_YY10(((_tmpD3C=_cycalloc(sizeof(*
_tmpD3C)),((_tmpD3C->hd=((_tmpD3D=_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpD3D->pat_vars=0,((_tmpD3D->where_clause=0,((_tmpD3D->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpD3D->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD3D)))))))))))),((
_tmpD3C->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpD3C)))))));}break;case 303: _LL33E:{struct Cyc_Absyn_Switch_clause*
_tmpD40;struct Cyc_List_List*_tmpD3F;yyval=Cyc_YY10(((_tmpD3F=_cycalloc(sizeof(*
_tmpD3F)),((_tmpD3F->hd=((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpD40->pat_vars=0,((_tmpD40->where_clause=0,((_tmpD40->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD40->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpD40)))))))))))),((
_tmpD3F->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpD3F)))))));}break;case 304: _LL33F:{struct Cyc_Absyn_Switch_clause*
_tmpD43;struct Cyc_List_List*_tmpD42;yyval=Cyc_YY10(((_tmpD42=_cycalloc(sizeof(*
_tmpD42)),((_tmpD42->hd=((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((
_tmpD43->pat_vars=0,((_tmpD43->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD43->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmpD43->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD43)))))))))))),((
_tmpD42->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpD42)))))));}break;case 305: _LL340:{struct Cyc_Absyn_Switch_clause*
_tmpD46;struct Cyc_List_List*_tmpD45;yyval=Cyc_YY10(((_tmpD45=_cycalloc(sizeof(*
_tmpD45)),((_tmpD45->hd=((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->pattern=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((
_tmpD46->pat_vars=0,((_tmpD46->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD46->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD46->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD46)))))))))))),((
_tmpD45->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpD45)))))));}break;case 306: _LL341: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 307:
_LL342: yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 308:
_LL343: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 309:
_LL344: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 310:
_LL345: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 311:
_LL346: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
312: _LL347: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 313: _LL348: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 314:
_LL349: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 315:
_LL34A: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 316:
_LL34B: {struct Cyc_List_List*_tmp6F0=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp6F1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6F0,_tmp6F1));break;}case 317: _LL34C: {struct
Cyc_List_List*_tmp6F2=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp6F3=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp6F2,
_tmp6F3));break;}case 318: _LL34D: {struct Cyc_List_List*_tmp6F4=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp6F5=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6F4,_tmp6F5));break;}case 319: _LL34E: {struct
Cyc_List_List*_tmp6F6=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp6F7=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp6F6,_tmp6F7));break;}case 320: _LL34F:{struct
_dyneither_ptr*_tmpD47;yyval=Cyc_YY9(Cyc_Absyn_goto_stmt(((_tmpD47=_cycalloc(
sizeof(*_tmpD47)),((_tmpD47[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD47)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));}break;case
321: _LL350: yyval=Cyc_YY9(Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
322: _LL351: yyval=Cyc_YY9(Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
323: _LL352: yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
324: _LL353: yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
325: _LL354: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
326: _LL355: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)));break;case
327: _LL356: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)));break;case
328: _LL357: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 329: _LL358: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 330: _LL359: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 331: _LL35A: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 332: _LL35B: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 333:
_LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
334: _LL35D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 335:
_LL35E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
336: _LL35F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 337: _LL360: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 338: _LL361: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 339:
_LL362: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
340: _LL363: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 341: _LL364: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 342: _LL365: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 343:
_LL366: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 344:
_LL367: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
345: _LL368: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 346:
_LL369: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 347:
_LL36A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 348:
_LL36B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 349:
_LL36C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
350: _LL36D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 351: _LL36E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 352:
_LL36F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
353: _LL370: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 354: _LL371: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 355:
_LL372: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
356: _LL373: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 357: _LL374: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 358:
_LL375: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 359:
_LL376: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
360: _LL377: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 361: _LL378: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 362: _LL379: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 363: _LL37A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 364: _LL37B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 365:
_LL37C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 366:
_LL37D:{struct Cyc_Absyn_StructField_struct _tmpD4D;struct _dyneither_ptr*_tmpD4C;
struct Cyc_Absyn_StructField_struct*_tmpD4B;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B[0]=((_tmpD4D.tag=0,((
_tmpD4D.f1=((_tmpD4C=_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD4C)))),_tmpD4D)))),
_tmpD4B)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 367: _LL37E:{struct Cyc_Absyn_TupleIndex_struct
_tmpD50;struct Cyc_Absyn_TupleIndex_struct*_tmpD4F;yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)((_tmpD4F=_cycalloc_atomic(sizeof(*_tmpD4F)),((
_tmpD4F[0]=((_tmpD50.tag=1,((_tmpD50.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2,_tmpD50)))),_tmpD4F)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));}break;case 368: _LL37F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 369: _LL380: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 370: _LL381:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 371:
_LL382: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 372: _LL383: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp6FE=e->r;union Cyc_Absyn_Cnst _tmp700;struct _tuple4
_tmp701;enum Cyc_Absyn_Sign _tmp702;char _tmp703;union Cyc_Absyn_Cnst _tmp705;struct
_tuple5 _tmp706;enum Cyc_Absyn_Sign _tmp707;short _tmp708;union Cyc_Absyn_Cnst
_tmp70A;struct _tuple6 _tmp70B;enum Cyc_Absyn_Sign _tmp70C;int _tmp70D;union Cyc_Absyn_Cnst
_tmp70F;struct _dyneither_ptr _tmp710;union Cyc_Absyn_Cnst _tmp712;int _tmp713;union
Cyc_Absyn_Cnst _tmp715;struct _dyneither_ptr _tmp716;union Cyc_Absyn_Cnst _tmp718;
struct _tuple7 _tmp719;_LL386: {struct Cyc_Absyn_Const_e_struct*_tmp6FF=(struct Cyc_Absyn_Const_e_struct*)
_tmp6FE;if(_tmp6FF->tag != 0)goto _LL388;else{_tmp700=_tmp6FF->f1;if((_tmp700.Char_c).tag
!= 2)goto _LL388;_tmp701=(struct _tuple4)(_tmp700.Char_c).val;_tmp702=_tmp701.f1;
_tmp703=_tmp701.f2;}}_LL387:{struct Cyc_Absyn_Char_p_struct _tmpD53;struct Cyc_Absyn_Char_p_struct*
_tmpD52;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD52=_cycalloc_atomic(
sizeof(*_tmpD52)),((_tmpD52[0]=((_tmpD53.tag=10,((_tmpD53.f1=_tmp703,_tmpD53)))),
_tmpD52)))),e->loc));}goto _LL385;_LL388: {struct Cyc_Absyn_Const_e_struct*_tmp704=(
struct Cyc_Absyn_Const_e_struct*)_tmp6FE;if(_tmp704->tag != 0)goto _LL38A;else{
_tmp705=_tmp704->f1;if((_tmp705.Short_c).tag != 3)goto _LL38A;_tmp706=(struct
_tuple5)(_tmp705.Short_c).val;_tmp707=_tmp706.f1;_tmp708=_tmp706.f2;}}_LL389:{
struct Cyc_Absyn_Int_p_struct _tmpD56;struct Cyc_Absyn_Int_p_struct*_tmpD55;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD55=_cycalloc_atomic(sizeof(*_tmpD55)),((
_tmpD55[0]=((_tmpD56.tag=9,((_tmpD56.f1=_tmp707,((_tmpD56.f2=(int)_tmp708,
_tmpD56)))))),_tmpD55)))),e->loc));}goto _LL385;_LL38A: {struct Cyc_Absyn_Const_e_struct*
_tmp709=(struct Cyc_Absyn_Const_e_struct*)_tmp6FE;if(_tmp709->tag != 0)goto _LL38C;
else{_tmp70A=_tmp709->f1;if((_tmp70A.Int_c).tag != 4)goto _LL38C;_tmp70B=(struct
_tuple6)(_tmp70A.Int_c).val;_tmp70C=_tmp70B.f1;_tmp70D=_tmp70B.f2;}}_LL38B:{
struct Cyc_Absyn_Int_p_struct _tmpD59;struct Cyc_Absyn_Int_p_struct*_tmpD58;yyval=
Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD58=_cycalloc_atomic(sizeof(*_tmpD58)),((
_tmpD58[0]=((_tmpD59.tag=9,((_tmpD59.f1=_tmp70C,((_tmpD59.f2=_tmp70D,_tmpD59)))))),
_tmpD58)))),e->loc));}goto _LL385;_LL38C: {struct Cyc_Absyn_Const_e_struct*_tmp70E=(
struct Cyc_Absyn_Const_e_struct*)_tmp6FE;if(_tmp70E->tag != 0)goto _LL38E;else{
_tmp70F=_tmp70E->f1;if((_tmp70F.Float_c).tag != 6)goto _LL38E;_tmp710=(struct
_dyneither_ptr)(_tmp70F.Float_c).val;}}_LL38D:{struct Cyc_Absyn_Float_p_struct
_tmpD5C;struct Cyc_Absyn_Float_p_struct*_tmpD5B;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpD5B=_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B[0]=((_tmpD5C.tag=11,((
_tmpD5C.f1=_tmp710,_tmpD5C)))),_tmpD5B)))),e->loc));}goto _LL385;_LL38E: {struct
Cyc_Absyn_Const_e_struct*_tmp711=(struct Cyc_Absyn_Const_e_struct*)_tmp6FE;if(
_tmp711->tag != 0)goto _LL390;else{_tmp712=_tmp711->f1;if((_tmp712.Null_c).tag != 1)
goto _LL390;_tmp713=(int)(_tmp712.Null_c).val;}}_LL38F: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL385;_LL390: {struct Cyc_Absyn_Const_e_struct*
_tmp714=(struct Cyc_Absyn_Const_e_struct*)_tmp6FE;if(_tmp714->tag != 0)goto _LL392;
else{_tmp715=_tmp714->f1;if((_tmp715.String_c).tag != 7)goto _LL392;_tmp716=(
struct _dyneither_ptr)(_tmp715.String_c).val;}}_LL391:{const char*_tmpD5D;Cyc_Parse_err(((
_tmpD5D="strings cannot occur within patterns",_tag_dyneither(_tmpD5D,sizeof(
char),37))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL385;_LL392: {struct Cyc_Absyn_Const_e_struct*
_tmp717=(struct Cyc_Absyn_Const_e_struct*)_tmp6FE;if(_tmp717->tag != 0)goto _LL394;
else{_tmp718=_tmp717->f1;if((_tmp718.LongLong_c).tag != 5)goto _LL394;_tmp719=(
struct _tuple7)(_tmp718.LongLong_c).val;}}_LL393:{const char*_tmpD5E;Cyc_Parse_unimp(((
_tmpD5E="long long's in patterns",_tag_dyneither(_tmpD5E,sizeof(char),24))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}goto _LL385;
_LL394:;_LL395: {const char*_tmpD5F;Cyc_Parse_err(((_tmpD5F="bad constant in case",
_tag_dyneither(_tmpD5F,sizeof(char),21))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL385:;}
break;}case 373: _LL384:{struct Cyc_Absyn_UnknownId_p_struct _tmpD62;struct Cyc_Absyn_UnknownId_p_struct*
_tmpD61;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD61=_cycalloc(sizeof(*
_tmpD61)),((_tmpD61[0]=((_tmpD62.tag=14,((_tmpD62.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD62)))),_tmpD61)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 374:
_LL396:{const char*_tmpD63;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD63="as",
_tag_dyneither(_tmpD63,sizeof(char),3))))!= 0){const char*_tmpD64;Cyc_Parse_err(((
_tmpD64="expecting `as'",_tag_dyneither(_tmpD64,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct
_tmpD6E;struct _dyneither_ptr*_tmpD6D;struct _tuple1*_tmpD6C;struct Cyc_Absyn_Var_p_struct*
_tmpD6B;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD6B=_cycalloc(sizeof(*
_tmpD6B)),((_tmpD6B[0]=((_tmpD6E.tag=1,((_tmpD6E.f1=Cyc_Absyn_new_vardecl(((
_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->f1=Cyc_Absyn_Loc_n,((_tmpD6C->f2=((
_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD6D)))),_tmpD6C)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpD6E.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD6E)))))),_tmpD6B)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 375: _LL397: {struct Cyc_List_List*
_tmp72E;int _tmp72F;struct _tuple19 _tmp72D=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp72E=_tmp72D.f1;
_tmp72F=_tmp72D.f2;{struct Cyc_Absyn_Tuple_p_struct _tmpD71;struct Cyc_Absyn_Tuple_p_struct*
_tmpD70;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD70=_cycalloc(sizeof(*
_tmpD70)),((_tmpD70[0]=((_tmpD71.tag=4,((_tmpD71.f1=_tmp72E,((_tmpD71.f2=_tmp72F,
_tmpD71)))))),_tmpD70)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
376: _LL398: {struct Cyc_List_List*_tmp733;int _tmp734;struct _tuple19 _tmp732=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp733=_tmp732.f1;
_tmp734=_tmp732.f2;{struct Cyc_Absyn_UnknownCall_p_struct _tmpD74;struct Cyc_Absyn_UnknownCall_p_struct*
_tmpD73;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD73=_cycalloc(sizeof(*
_tmpD73)),((_tmpD73[0]=((_tmpD74.tag=15,((_tmpD74.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpD74.f2=_tmp733,((
_tmpD74.f3=_tmp734,_tmpD74)))))))),_tmpD73)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
377: _LL399: {struct Cyc_List_List*_tmp738;int _tmp739;struct _tuple19 _tmp737=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp738=_tmp737.f1;
_tmp739=_tmp737.f2;{struct Cyc_List_List*_tmp73A=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpD7A;struct Cyc_Absyn_AggrInfo*_tmpD79;struct Cyc_Absyn_Aggr_p_struct*_tmpD78;
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD78=_cycalloc(sizeof(*_tmpD78)),((
_tmpD78[0]=((_tmpD7A.tag=6,((_tmpD7A.f1=((_tmpD79=_cycalloc(sizeof(*_tmpD79)),((
_tmpD79->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0),((_tmpD79->targs=
0,_tmpD79)))))),((_tmpD7A.f2=_tmp73A,((_tmpD7A.f3=_tmp738,((_tmpD7A.f4=_tmp739,
_tmpD7A)))))))))),_tmpD78)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}}case
378: _LL39A: {struct Cyc_List_List*_tmp73F;int _tmp740;struct _tuple19 _tmp73E=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp73F=_tmp73E.f1;
_tmp740=_tmp73E.f2;{struct Cyc_List_List*_tmp741=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct
_tmpD7D;struct Cyc_Absyn_Aggr_p_struct*_tmpD7C;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpD7C=_cycalloc(sizeof(*_tmpD7C)),((_tmpD7C[0]=((_tmpD7D.tag=6,((
_tmpD7D.f1=0,((_tmpD7D.f2=_tmp741,((_tmpD7D.f3=_tmp73F,((_tmpD7D.f4=_tmp740,
_tmpD7D)))))))))),_tmpD7C)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}}case
379: _LL39B:{struct Cyc_Absyn_Pointer_p_struct _tmpD80;struct Cyc_Absyn_Pointer_p_struct*
_tmpD7F;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD7F=_cycalloc(sizeof(*
_tmpD7F)),((_tmpD7F[0]=((_tmpD80.tag=5,((_tmpD80.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD80)))),_tmpD7F)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 380:
_LL39C:{struct Cyc_Absyn_Pointer_p_struct _tmpD8A;struct Cyc_Absyn_Pointer_p_struct
_tmpD89;struct Cyc_Absyn_Pointer_p_struct*_tmpD88;struct Cyc_Absyn_Pointer_p_struct*
_tmpD87;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD87=_cycalloc(sizeof(*
_tmpD87)),((_tmpD87[0]=((_tmpD8A.tag=5,((_tmpD8A.f1=Cyc_Absyn_new_pat((void*)((
_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88[0]=((_tmpD89.tag=5,((_tmpD89.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD89)))),
_tmpD88)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpD8A)))),_tmpD87)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 381:
_LL39D:{struct Cyc_Absyn_Reference_p_struct _tmpD94;struct _dyneither_ptr*_tmpD93;
struct _tuple1*_tmpD92;struct Cyc_Absyn_Reference_p_struct*_tmpD91;yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)((_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91[0]=((
_tmpD94.tag=2,((_tmpD94.f1=Cyc_Absyn_new_vardecl(((_tmpD92=_cycalloc(sizeof(*
_tmpD92)),((_tmpD92->f1=Cyc_Absyn_Loc_n,((_tmpD92->f2=((_tmpD93=_cycalloc(
sizeof(*_tmpD93)),((_tmpD93[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD93)))),_tmpD92)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpD94.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpD94)))))),_tmpD91)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 382:
_LL39E:{const char*_tmpD95;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpD95="as",
_tag_dyneither(_tmpD95,sizeof(char),3))))!= 0){const char*_tmpD96;Cyc_Parse_err(((
_tmpD96="expecting `as'",_tag_dyneither(_tmpD96,sizeof(char),15))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct
_tmpDA0;struct _dyneither_ptr*_tmpD9F;struct _tuple1*_tmpD9E;struct Cyc_Absyn_Reference_p_struct*
_tmpD9D;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpD9D=_cycalloc(sizeof(*
_tmpD9D)),((_tmpD9D[0]=((_tmpDA0.tag=2,((_tmpDA0.f1=Cyc_Absyn_new_vardecl(((
_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->f1=Cyc_Absyn_Loc_n,((_tmpD9E->f2=((
_tmpD9F=_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpD9F)))),_tmpD9E)))))),(
void*)& Cyc_Absyn_VoidType_val,0),((_tmpDA0.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDA0)))))),_tmpD9D)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)));}break;case 383: _LL39F: {void*_tmp754=Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
Cyc_Tcutil_kind_to_bound(Cyc_Absyn_IntKind));{struct Cyc_Absyn_TagInt_p_struct
_tmpDB1;struct Cyc_Absyn_TagType_struct _tmpDB0;struct Cyc_Absyn_TagType_struct*
_tmpDAF;struct _dyneither_ptr*_tmpDAE;struct _tuple1*_tmpDAD;struct Cyc_Absyn_TagInt_p_struct*
_tmpDAC;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)((_tmpDAC=_cycalloc(sizeof(*
_tmpDAC)),((_tmpDAC[0]=((_tmpDB1.tag=3,((_tmpDB1.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp754),((
_tmpDB1.f2=Cyc_Absyn_new_vardecl(((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->f1=
Cyc_Absyn_Loc_n,((_tmpDAD->f2=((_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmpDAE)))),_tmpDAD)))))),(void*)((_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF[
0]=((_tmpDB0.tag=20,((_tmpDB0.f1=(void*)_tmp754,_tmpDB0)))),_tmpDAF)))),0),
_tmpDB1)))))),_tmpDAC)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
384: _LL3A0: {struct Cyc_Absyn_Tvar*_tmp75B=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_IntKind));{struct Cyc_Absyn_TagInt_p_struct _tmpDCD;struct Cyc_Absyn_TagType_struct
_tmpDCC;struct Cyc_Absyn_VarType_struct _tmpDCB;struct Cyc_Absyn_VarType_struct*
_tmpDCA;struct Cyc_Absyn_TagType_struct*_tmpDC9;struct _dyneither_ptr*_tmpDC8;
struct _tuple1*_tmpDC7;struct Cyc_Absyn_TagInt_p_struct*_tmpDC6;yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)((_tmpDC6=_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6[0]=((_tmpDCD.tag=3,((
_tmpDCD.f1=_tmp75B,((_tmpDCD.f2=Cyc_Absyn_new_vardecl(((_tmpDC7=_cycalloc(
sizeof(*_tmpDC7)),((_tmpDC7->f1=Cyc_Absyn_Loc_n,((_tmpDC7->f2=((_tmpDC8=
_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpDC8)))),_tmpDC7)))))),(
void*)((_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9[0]=((_tmpDCC.tag=20,((
_tmpDCC.f1=(void*)((void*)((_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA[0]=((
_tmpDCB.tag=2,((_tmpDCB.f1=_tmp75B,_tmpDCB)))),_tmpDCA))))),_tmpDCC)))),_tmpDC9)))),
0),_tmpDCD)))))),_tmpDC6)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
385: _LL3A1:{struct _tuple19*_tmpDCE;yyval=Cyc_YY12(((_tmpDCE=_cycalloc(sizeof(*
_tmpDCE)),((_tmpDCE->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpDCE->f2=0,_tmpDCE)))))));}break;case 386: _LL3A2:{struct _tuple19*_tmpDCF;yyval=
Cyc_YY12(((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpDCF->f2=1,_tmpDCF)))))));}
break;case 387: _LL3A3:{struct _tuple19*_tmpDD0;yyval=Cyc_YY12(((_tmpDD0=_cycalloc(
sizeof(*_tmpDD0)),((_tmpDD0->f1=0,((_tmpDD0->f2=1,_tmpDD0)))))));}break;case 388:
_LL3A4:{struct Cyc_List_List*_tmpDD1;yyval=Cyc_YY13(((_tmpDD1=_cycalloc(sizeof(*
_tmpDD1)),((_tmpDD1->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpDD1->tl=0,_tmpDD1)))))));}break;case 389: _LL3A5:{struct Cyc_List_List*
_tmpDD2;yyval=Cyc_YY13(((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDD2->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpDD2)))))));}
break;case 390: _LL3A6:{struct _tuple20*_tmpDD3;yyval=Cyc_YY14(((_tmpDD3=_cycalloc(
sizeof(*_tmpDD3)),((_tmpDD3->f1=0,((_tmpDD3->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDD3)))))));}break;case
391: _LL3A7:{struct _tuple20*_tmpDD4;yyval=Cyc_YY14(((_tmpDD4=_cycalloc(sizeof(*
_tmpDD4)),((_tmpDD4->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpDD4->f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDD4)))))));}break;case
392: _LL3A8:{struct _tuple19*_tmpDD5;yyval=Cyc_YY16(((_tmpDD5=_cycalloc(sizeof(*
_tmpDD5)),((_tmpDD5->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),((
_tmpDD5->f2=0,_tmpDD5)))))));}break;case 393: _LL3A9:{struct _tuple19*_tmpDD6;yyval=
Cyc_YY16(((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmpDD6->f2=1,_tmpDD6)))))));}
break;case 394: _LL3AA:{struct _tuple19*_tmpDD7;yyval=Cyc_YY16(((_tmpDD7=_cycalloc(
sizeof(*_tmpDD7)),((_tmpDD7->f1=0,((_tmpDD7->f2=1,_tmpDD7)))))));}break;case 395:
_LL3AB:{struct Cyc_List_List*_tmpDD8;yyval=Cyc_YY15(((_tmpDD8=_cycalloc(sizeof(*
_tmpDD8)),((_tmpDD8->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpDD8->tl=0,_tmpDD8)))))));}break;case 396: _LL3AC:{struct Cyc_List_List*
_tmpDD9;yyval=Cyc_YY15(((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpDD9->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpDD9)))))));}
break;case 397: _LL3AD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 398: _LL3AE: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 399:
_LL3AF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
400: _LL3B0: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 401:
_LL3B1:{struct Cyc_Absyn_Swap_e_struct _tmpDDC;struct Cyc_Absyn_Swap_e_struct*
_tmpDDB;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpDDB=_cycalloc(sizeof(*
_tmpDDB)),((_tmpDDB[0]=((_tmpDDC.tag=36,((_tmpDDC.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDDC.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDDC)))))),_tmpDDB)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 402: _LL3B2: yyval=Cyc_YY8(0);break;case
403: _LL3B3:{struct Cyc_Core_Opt*_tmpDDD;yyval=Cyc_YY8(((_tmpDDD=_cycalloc_atomic(
sizeof(*_tmpDDD)),((_tmpDDD->v=(void*)Cyc_Absyn_Times,_tmpDDD)))));}break;case
404: _LL3B4:{struct Cyc_Core_Opt*_tmpDDE;yyval=Cyc_YY8(((_tmpDDE=_cycalloc_atomic(
sizeof(*_tmpDDE)),((_tmpDDE->v=(void*)Cyc_Absyn_Div,_tmpDDE)))));}break;case 405:
_LL3B5:{struct Cyc_Core_Opt*_tmpDDF;yyval=Cyc_YY8(((_tmpDDF=_cycalloc_atomic(
sizeof(*_tmpDDF)),((_tmpDDF->v=(void*)Cyc_Absyn_Mod,_tmpDDF)))));}break;case 406:
_LL3B6:{struct Cyc_Core_Opt*_tmpDE0;yyval=Cyc_YY8(((_tmpDE0=_cycalloc_atomic(
sizeof(*_tmpDE0)),((_tmpDE0->v=(void*)Cyc_Absyn_Plus,_tmpDE0)))));}break;case 407:
_LL3B7:{struct Cyc_Core_Opt*_tmpDE1;yyval=Cyc_YY8(((_tmpDE1=_cycalloc_atomic(
sizeof(*_tmpDE1)),((_tmpDE1->v=(void*)Cyc_Absyn_Minus,_tmpDE1)))));}break;case
408: _LL3B8:{struct Cyc_Core_Opt*_tmpDE2;yyval=Cyc_YY8(((_tmpDE2=_cycalloc_atomic(
sizeof(*_tmpDE2)),((_tmpDE2->v=(void*)Cyc_Absyn_Bitlshift,_tmpDE2)))));}break;
case 409: _LL3B9:{struct Cyc_Core_Opt*_tmpDE3;yyval=Cyc_YY8(((_tmpDE3=
_cycalloc_atomic(sizeof(*_tmpDE3)),((_tmpDE3->v=(void*)Cyc_Absyn_Bitlrshift,
_tmpDE3)))));}break;case 410: _LL3BA:{struct Cyc_Core_Opt*_tmpDE4;yyval=Cyc_YY8(((
_tmpDE4=_cycalloc_atomic(sizeof(*_tmpDE4)),((_tmpDE4->v=(void*)Cyc_Absyn_Bitand,
_tmpDE4)))));}break;case 411: _LL3BB:{struct Cyc_Core_Opt*_tmpDE5;yyval=Cyc_YY8(((
_tmpDE5=_cycalloc_atomic(sizeof(*_tmpDE5)),((_tmpDE5->v=(void*)Cyc_Absyn_Bitxor,
_tmpDE5)))));}break;case 412: _LL3BC:{struct Cyc_Core_Opt*_tmpDE6;yyval=Cyc_YY8(((
_tmpDE6=_cycalloc_atomic(sizeof(*_tmpDE6)),((_tmpDE6->v=(void*)Cyc_Absyn_Bitor,
_tmpDE6)))));}break;case 413: _LL3BD: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 414: _LL3BE: yyval=Cyc_YY3(Cyc_Absyn_conditional_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 415:
_LL3BF: yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 416:
_LL3C0: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 417:
_LL3C1: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 418:
_LL3C2: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 419:
_LL3C3: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 420:
_LL3C4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
421: _LL3C5: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 422: _LL3C6: yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 423:
_LL3C7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
424: _LL3C8: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 425:
_LL3C9: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
426: _LL3CA: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 427:
_LL3CB: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
428: _LL3CC: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 429:
_LL3CD: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
430: _LL3CE: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 431:
_LL3CF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
432: _LL3D0: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 433:
_LL3D1: yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 434:
_LL3D2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
435: _LL3D3: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 436:
_LL3D4: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 437:
_LL3D5: yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 438:
_LL3D6: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 439:
_LL3D7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
440: _LL3D8: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 441:
_LL3D9: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 442:
_LL3DA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
443: _LL3DB: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 444:
_LL3DC: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 445:
_LL3DD: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
446: _LL3DE: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 447:
_LL3DF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 448:
_LL3E0: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 449:
_LL3E1: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
450: _LL3E2: yyval=Cyc_YY3(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 451: _LL3E3: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 452: _LL3E4: yyval=
Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 453:
_LL3E5: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 454:
_LL3E6: yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 455:
_LL3E7: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 456:
_LL3E8: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
457: _LL3E9: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 458:
_LL3EA: yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 459:
_LL3EB: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 460:
_LL3EC:{struct Cyc_Absyn_StructField_struct _tmpDEC;struct _dyneither_ptr*_tmpDEB;
struct Cyc_Absyn_StructField_struct*_tmpDEA;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA[0]=((_tmpDEC.tag=0,((
_tmpDEC.f1=((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDEB)))),_tmpDEC)))),
_tmpDEA)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 461: _LL3ED:{struct Cyc_Absyn_TupleIndex_struct
_tmpDEF;struct Cyc_Absyn_TupleIndex_struct*_tmpDEE;yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)((_tmpDEE=_cycalloc_atomic(sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDEF.tag=1,((
_tmpDEF.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2,_tmpDEF)))),_tmpDEE)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 462:
_LL3EE: {struct Cyc_Position_Segment*_tmp781=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp782=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp781,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=Cyc_YY3(Cyc_Absyn_gentyp_exp(_tmp782,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 463:
_LL3EF:{struct Cyc_Absyn_Malloc_e_struct _tmpDF5;struct Cyc_Absyn_MallocInfo _tmpDF4;
struct Cyc_Absyn_Malloc_e_struct*_tmpDF3;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((_tmpDF3[0]=((_tmpDF5.tag=35,((_tmpDF5.f1=((
_tmpDF4.is_calloc=0,((_tmpDF4.rgn=0,((_tmpDF4.elt_type=0,((_tmpDF4.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpDF4.fat_result=
0,_tmpDF4)))))))))),_tmpDF5)))),_tmpDF3)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 464:
_LL3F0:{struct Cyc_Absyn_Malloc_e_struct _tmpDFB;struct Cyc_Absyn_MallocInfo _tmpDFA;
struct Cyc_Absyn_Malloc_e_struct*_tmpDF9;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9[0]=((_tmpDFB.tag=35,((_tmpDFB.f1=((
_tmpDFA.is_calloc=0,((_tmpDFA.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpDFA.elt_type=0,((
_tmpDFA.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpDFA.fat_result=0,_tmpDFA)))))))))),_tmpDFB)))),_tmpDF9)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 465: _LL3F1: {void*_tmp78A;struct _tuple8
_tmp789=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
2)]);_tmp78A=_tmp789.f3;{struct Cyc_Absyn_Malloc_e_struct _tmpE05;void**_tmpE04;
struct Cyc_Absyn_MallocInfo _tmpE03;struct Cyc_Absyn_Malloc_e_struct*_tmpE02;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02[
0]=((_tmpE05.tag=35,((_tmpE05.f1=((_tmpE03.is_calloc=1,((_tmpE03.rgn=0,((_tmpE03.elt_type=((
_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04[0]=_tmp78A,_tmpE04)))),((_tmpE03.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpE03.fat_result=0,_tmpE03)))))))))),_tmpE05)))),_tmpE02)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
466: _LL3F2: {void*_tmp790;struct _tuple8 _tmp78F=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp790=_tmp78F.f3;{
struct Cyc_Absyn_Malloc_e_struct _tmpE0F;void**_tmpE0E;struct Cyc_Absyn_MallocInfo
_tmpE0D;struct Cyc_Absyn_Malloc_e_struct*_tmpE0C;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C[0]=((_tmpE0F.tag=35,((
_tmpE0F.f1=((_tmpE0D.is_calloc=1,((_tmpE0D.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmpE0D.elt_type=((
_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=_tmp790,_tmpE0E)))),((_tmpE0D.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((
_tmpE0D.fat_result=0,_tmpE0D)))))))))),_tmpE0F)))),_tmpE0C)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;}case
467: _LL3F3:{struct Cyc_Absyn_Exp*_tmpE10[1];yyval=Cyc_YY3(Cyc_Absyn_primop_exp(
Cyc_Absyn_Numelts,((_tmpE10[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpE10,sizeof(struct Cyc_Absyn_Exp*),1)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 468:
_LL3F4:{struct Cyc_Absyn_Tagcheck_e_struct _tmpE16;struct _dyneither_ptr*_tmpE15;
struct Cyc_Absyn_Tagcheck_e_struct*_tmpE14;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((
_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE16.tag=39,((_tmpE16.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpE16.f2=((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE15)))),_tmpE16)))))),
_tmpE14)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 469: _LL3F5:{struct Cyc_Absyn_Tagcheck_e_struct
_tmpE1C;struct _dyneither_ptr*_tmpE1B;struct Cyc_Absyn_Tagcheck_e_struct*_tmpE1A;
yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((
_tmpE1A[0]=((_tmpE1C.tag=39,((_tmpE1C.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),((_tmpE1C.f2=((
_tmpE1B=_cycalloc(sizeof(*_tmpE1B)),((_tmpE1B[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE1B)))),_tmpE1C)))))),
_tmpE1A)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 470: _LL3F6: {void*_tmp79D;struct _tuple8
_tmp79C=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp79D=_tmp79C.f3;yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp79D,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 471:
_LL3F7: yyval=Cyc_YY7(Cyc_Absyn_Bitnot);break;case 472: _LL3F8: yyval=Cyc_YY7(Cyc_Absyn_Not);
break;case 473: _LL3F9: yyval=Cyc_YY7(Cyc_Absyn_Minus);break;case 474: _LL3FA: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 475: _LL3FB:
yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 476:
_LL3FC: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 477:
_LL3FD: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 478:
_LL3FE:{struct _dyneither_ptr*_tmpE1D;yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpE1D=_cycalloc(
sizeof(*_tmpE1D)),((_tmpE1D[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE1D)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 479:
_LL3FF:{struct _dyneither_ptr*_tmpE1E;yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpE1E=_cycalloc(
sizeof(*_tmpE1E)),((_tmpE1E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE1E)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 480:
_LL400: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 481:
_LL401: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 482:
_LL402:{struct Cyc_Absyn_CompoundLit_e_struct _tmpE21;struct Cyc_Absyn_CompoundLit_e_struct*
_tmpE20;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE20=_cycalloc(sizeof(*
_tmpE20)),((_tmpE20[0]=((_tmpE21.tag=27,((_tmpE21.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpE21.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpE21)))))),_tmpE20)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));}break;case 483: _LL403:{struct Cyc_Absyn_CompoundLit_e_struct
_tmpE24;struct Cyc_Absyn_CompoundLit_e_struct*_tmpE23;yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23[0]=((_tmpE24.tag=27,((
_tmpE24.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 5)]),((_tmpE24.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),
_tmpE24)))))),_tmpE23)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 484:
_LL404:{struct Cyc_Absyn_UnknownId_e_struct _tmpE27;struct Cyc_Absyn_UnknownId_e_struct*
_tmpE26;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE26=_cycalloc(sizeof(*
_tmpE26)),((_tmpE26[0]=((_tmpE27.tag=2,((_tmpE27.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE27)))),_tmpE26)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 485:
_LL405: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
486: _LL406: yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 487:
_LL407: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 488: _LL408: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 489:
_LL409: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 490:
_LL40A: yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 491:
_LL40B:{struct Cyc_Absyn_Aggregate_e_struct _tmpE2A;struct Cyc_Absyn_Aggregate_e_struct*
_tmpE29;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)((_tmpE29=_cycalloc(sizeof(*
_tmpE29)),((_tmpE29[0]=((_tmpE2A.tag=30,((_tmpE2A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpE2A.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpE2A.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmpE2A.f4=0,
_tmpE2A)))))))))),_tmpE29)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));}break;case 492:
_LL40C: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 493:
_LL40D: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 494: _LL40E:{struct Cyc_List_List*_tmpE2B;yyval=Cyc_YY5(((_tmpE2B=_cycalloc(
sizeof(*_tmpE2B)),((_tmpE2B->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE2B->tl=0,_tmpE2B)))))));}
break;case 495: _LL40F:{struct Cyc_List_List*_tmpE2C;yyval=Cyc_YY5(((_tmpE2C=
_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE2C->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpE2C)))))));}
break;case 496: _LL410: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 497:
_LL411: yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 498:
_LL412: yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 499:
_LL413: yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 500:
_LL414:{struct _dyneither_ptr*_tmpE2F;struct _tuple1*_tmpE2E;yyval=Cyc_QualId_tok(((
_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->f1=Cyc_Absyn_Rel_n(0),((_tmpE2E->f2=((
_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE2F)))),_tmpE2E)))))));}
break;case 501: _LL415: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 502: _LL416:{struct _dyneither_ptr*_tmpE32;struct _tuple1*
_tmpE31;yyval=Cyc_QualId_tok(((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31->f1=
Cyc_Absyn_Rel_n(0),((_tmpE31->f2=((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[
0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpE32)))),_tmpE31)))))));}break;case 503: _LL417: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 504: _LL418: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 505: _LL419:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 506:
_LL41A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
507: _LL41B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 508: _LL41C: break;case 509: _LL41D: yylex_buf->lex_curr_pos -=1;break;default:
_LL41E: break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(510,yyn)];yystate=Cyc_yypgoto[
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
_check_known_subscript_notnull(287,x)])+ 15,count ++);}{unsigned int _tmpE37;
unsigned int _tmpE36;struct _dyneither_ptr _tmpE35;char*_tmpE34;unsigned int _tmpE33;
msg=((_tmpE33=(unsigned int)(sze + 15),((_tmpE34=(char*)_region_malloc(yyregion,
_check_times(sizeof(char),_tmpE33 + 1)),((_tmpE35=_tag_dyneither(_tmpE34,sizeof(
char),_tmpE33 + 1),((((_tmpE36=_tmpE33,((_tmpE38(& _tmpE37,& _tmpE36,& _tmpE34),
_tmpE34[_tmpE36]=(char)0)))),_tmpE35))))))));}{const char*_tmpE39;Cyc_strcpy(msg,((
_tmpE39="parse error",_tag_dyneither(_tmpE39,sizeof(char),12))));}if(count < 5){
count=0;for(x=yyn < 0?- yyn: 0;x < 287 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6372,x + yyn)]== x){{const char*_tmpE3B;const char*
_tmpE3A;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)((
_tmpE3B=", expecting `",_tag_dyneither(_tmpE3B,sizeof(char),14))):(struct
_dyneither_ptr)((_tmpE3A=" or `",_tag_dyneither(_tmpE3A,sizeof(char),6)))));}Cyc_strcat(
msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(287,x)]);{
const char*_tmpE3C;Cyc_strcat(msg,((_tmpE3C="'",_tag_dyneither(_tmpE3C,sizeof(
char),2))));}++ count;}}}Cyc_yyerror((struct _dyneither_ptr)msg);}else{const char*
_tmpE3D;Cyc_yyerror(((_tmpE3D="parse error",_tag_dyneither(_tmpE3D,sizeof(char),
12))));}}goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int
_tmp7B7=1;_npop_handler(0);return _tmp7B7;}Cyc_yychar=- 2;}yyerrstatus=3;goto
yyerrhandle;yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp7B8=1;
_npop_handler(0);return _tmp7B8;}-- yyvsp_offset;yystate=(int)yyss[
_check_known_subscript_notnull(10000,-- yyssp_offset)];-- yylsp_offset;yyerrhandle:
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,yystate)];if(yyn == -
32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6371) || Cyc_yycheck[
_check_known_subscript_notnull(6372,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6372,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1031){int
_tmp7B9=0;_npop_handler(0);return _tmp7B9;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE v);void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE
_tmp7C3=v;struct _tuple6 _tmp7C4;int _tmp7C5;char _tmp7C6;struct _dyneither_ptr
_tmp7C7;struct _tuple1*_tmp7C8;struct _tuple1 _tmp7C9;union Cyc_Absyn_Nmspace _tmp7CA;
struct _dyneither_ptr*_tmp7CB;_LL421: if((_tmp7C3.Int_tok).tag != 1)goto _LL423;
_tmp7C4=(struct _tuple6)(_tmp7C3.Int_tok).val;_tmp7C5=_tmp7C4.f2;_LL422:{const
char*_tmpE41;void*_tmpE40[1];struct Cyc_Int_pa_struct _tmpE3F;(_tmpE3F.tag=1,((
_tmpE3F.f1=(unsigned long)_tmp7C5,((_tmpE40[0]=& _tmpE3F,Cyc_fprintf(Cyc_stderr,((
_tmpE41="%d",_tag_dyneither(_tmpE41,sizeof(char),3))),_tag_dyneither(_tmpE40,
sizeof(void*),1)))))));}goto _LL420;_LL423: if((_tmp7C3.Char_tok).tag != 2)goto
_LL425;_tmp7C6=(char)(_tmp7C3.Char_tok).val;_LL424:{const char*_tmpE45;void*
_tmpE44[1];struct Cyc_Int_pa_struct _tmpE43;(_tmpE43.tag=1,((_tmpE43.f1=(
unsigned long)((int)_tmp7C6),((_tmpE44[0]=& _tmpE43,Cyc_fprintf(Cyc_stderr,((
_tmpE45="%c",_tag_dyneither(_tmpE45,sizeof(char),3))),_tag_dyneither(_tmpE44,
sizeof(void*),1)))))));}goto _LL420;_LL425: if((_tmp7C3.String_tok).tag != 3)goto
_LL427;_tmp7C7=(struct _dyneither_ptr)(_tmp7C3.String_tok).val;_LL426:{const char*
_tmpE49;void*_tmpE48[1];struct Cyc_String_pa_struct _tmpE47;(_tmpE47.tag=0,((
_tmpE47.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7C7),((_tmpE48[0]=&
_tmpE47,Cyc_fprintf(Cyc_stderr,((_tmpE49="\"%s\"",_tag_dyneither(_tmpE49,sizeof(
char),5))),_tag_dyneither(_tmpE48,sizeof(void*),1)))))));}goto _LL420;_LL427: if((
_tmp7C3.QualId_tok).tag != 5)goto _LL429;_tmp7C8=(struct _tuple1*)(_tmp7C3.QualId_tok).val;
_tmp7C9=*_tmp7C8;_tmp7CA=_tmp7C9.f1;_tmp7CB=_tmp7C9.f2;_LL428: {struct Cyc_List_List*
_tmp7D5=0;{union Cyc_Absyn_Nmspace _tmp7D6=_tmp7CA;struct Cyc_List_List*_tmp7D7;
struct Cyc_List_List*_tmp7D8;int _tmp7D9;_LL42C: if((_tmp7D6.Rel_n).tag != 1)goto
_LL42E;_tmp7D7=(struct Cyc_List_List*)(_tmp7D6.Rel_n).val;_LL42D: _tmp7D5=_tmp7D7;
goto _LL42B;_LL42E: if((_tmp7D6.Abs_n).tag != 2)goto _LL430;_tmp7D8=(struct Cyc_List_List*)(
_tmp7D6.Abs_n).val;_LL42F: _tmp7D5=_tmp7D8;goto _LL42B;_LL430: if((_tmp7D6.Loc_n).tag
!= 3)goto _LL42B;_tmp7D9=(int)(_tmp7D6.Loc_n).val;_LL431: goto _LL42B;_LL42B:;}for(
0;_tmp7D5 != 0;_tmp7D5=_tmp7D5->tl){const char*_tmpE4D;void*_tmpE4C[1];struct Cyc_String_pa_struct
_tmpE4B;(_tmpE4B.tag=0,((_tmpE4B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)_tmp7D5->hd)),((_tmpE4C[0]=& _tmpE4B,Cyc_fprintf(Cyc_stderr,((
_tmpE4D="%s::",_tag_dyneither(_tmpE4D,sizeof(char),5))),_tag_dyneither(_tmpE4C,
sizeof(void*),1)))))));}{const char*_tmpE51;void*_tmpE50[1];struct Cyc_String_pa_struct
_tmpE4F;(_tmpE4F.tag=0,((_tmpE4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp7CB),((_tmpE50[0]=& _tmpE4F,Cyc_fprintf(Cyc_stderr,((_tmpE51="%s::",
_tag_dyneither(_tmpE51,sizeof(char),5))),_tag_dyneither(_tmpE50,sizeof(void*),1)))))));}
goto _LL420;}_LL429:;_LL42A:{const char*_tmpE54;void*_tmpE53;(_tmpE53=0,Cyc_fprintf(
Cyc_stderr,((_tmpE54="?",_tag_dyneither(_tmpE54,sizeof(char),2))),_tag_dyneither(
_tmpE53,sizeof(void*),0)));}goto _LL420;_LL420:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*
f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(f));return Cyc_Parse_parse_result;}
