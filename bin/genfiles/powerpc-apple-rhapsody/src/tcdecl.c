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
tag;struct _dyneither_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*
env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};void Cyc_Position_post_error(struct Cyc_Position_Error*);struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple0{union
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
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
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
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);enum Cyc_Absyn_Kind Cyc_Absyn_force_kb(
void*kb);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);extern char Cyc_Tcdecl_Incompatible[
13];struct Cyc_Tcdecl_Incompatible_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{
struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);struct _tuple9{enum Cyc_Absyn_Scope f1;int f2;};struct
_tuple9 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,
struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg,int allow_externC_extern_merge);struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*
Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct
Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*
d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct
Cyc_List_List*f,int*res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};int Cyc_strptrcmp(struct _dyneither_ptr*s1,
struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};int Cyc_Tcutil_unify_kindbound(
void*,void*);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_same_atts(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple0*);struct _dyneither_ptr Cyc_Absynpp_scope2string(
enum Cyc_Absyn_Scope sc);char Cyc_Tcdecl_Incompatible[13]="Incompatible";struct Cyc_Tcdecl_Incompatible_struct
Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};void Cyc_Tcdecl_merr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);static void _tmp19C(struct _dyneither_ptr**msg1,struct
_dyneither_ptr*ap,unsigned int*_tmp19B,unsigned int*_tmp19A,void***_tmp198){for(*
_tmp19B=0;*_tmp19B < *_tmp19A;(*_tmp19B)++){struct Cyc_String_pa_struct _tmp196;
struct Cyc_String_pa_struct*_tmp195;(*_tmp198)[*_tmp19B]=*_tmp19B == 0?(void*)((
void*)((_tmp195=_cycalloc(sizeof(*_tmp195)),((_tmp195[0]=((_tmp196.tag=0,((
_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*msg1)),_tmp196)))),
_tmp195))))):*((void**)_check_dyneither_subscript(*ap,sizeof(void*),(int)(*
_tmp19B - 1)));}}void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){if(msg1 == 0)(
int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{const char*_tmp193;struct
_dyneither_ptr fmt2=(struct _dyneither_ptr)Cyc_strconcat(((_tmp193="%s ",
_tag_dyneither(_tmp193,sizeof(char),4))),(struct _dyneither_ptr)fmt);unsigned int
_tmp19B;unsigned int _tmp19A;struct _dyneither_ptr _tmp199;void**_tmp198;
unsigned int _tmp197;struct _dyneither_ptr ap2=(_tmp197=_get_dyneither_size(ap,
sizeof(void*))+ 1,((_tmp198=(void**)_cycalloc(_check_times(sizeof(void*),_tmp197)),((
_tmp199=_tag_dyneither(_tmp198,sizeof(void*),_tmp197),((((_tmp19A=_tmp197,
_tmp19C(& msg1,& ap,& _tmp19B,& _tmp19A,& _tmp198))),_tmp199)))))));Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt2,ap2)));};}static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum 
Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope
s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){const char*_tmp1A3;void*_tmp1A2[4];struct Cyc_String_pa_struct
_tmp1A1;struct Cyc_String_pa_struct _tmp1A0;struct Cyc_String_pa_struct _tmp19F;
struct Cyc_String_pa_struct _tmp19E;(_tmp19E.tag=0,((_tmp19E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_scope2string(s0)),((_tmp19F.tag=
0,((_tmp19F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_scope2string(
s1)),((_tmp1A0.tag=0,((_tmp1A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((
_tmp1A1.tag=0,((_tmp1A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp1A2[0]=& _tmp1A1,((_tmp1A2[1]=& _tmp1A0,((_tmp1A2[2]=& _tmp19F,((_tmp1A2[3]=&
_tmp19E,Cyc_Tcdecl_merr(loc,msg,((_tmp1A3="%s %s is %s whereas expected scope is %s",
_tag_dyneither(_tmp1A3,sizeof(char),41))),_tag_dyneither(_tmp1A2,sizeof(void*),4)))))))))))))))))))))))));}
struct _tuple10{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};struct _tuple9 Cyc_Tcdecl_merge_scope(
enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct
_dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg,int
externCmerge);struct _tuple9 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum 
Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg,int externCmerge){{struct _tuple10 _tmp1A4;struct
_tuple10 _tmpF=(_tmp1A4.f1=s0,((_tmp1A4.f2=s1,_tmp1A4)));enum Cyc_Absyn_Scope
_tmp10;enum Cyc_Absyn_Scope _tmp11;enum Cyc_Absyn_Scope _tmp12;enum Cyc_Absyn_Scope
_tmp13;enum Cyc_Absyn_Scope _tmp14;enum Cyc_Absyn_Scope _tmp15;enum Cyc_Absyn_Scope
_tmp16;enum Cyc_Absyn_Scope _tmp17;enum Cyc_Absyn_Scope _tmp18;enum Cyc_Absyn_Scope
_tmp19;enum Cyc_Absyn_Scope _tmp1A;enum Cyc_Absyn_Scope _tmp1B;enum Cyc_Absyn_Scope
_tmp1C;enum Cyc_Absyn_Scope _tmp1D;enum Cyc_Absyn_Scope _tmp1E;enum Cyc_Absyn_Scope
_tmp1F;enum Cyc_Absyn_Scope _tmp20;enum Cyc_Absyn_Scope _tmp21;_LL1: _tmp10=_tmpF.f1;
if(_tmp10 != Cyc_Absyn_ExternC)goto _LL3;_tmp11=_tmpF.f2;if(_tmp11 != Cyc_Absyn_ExternC)
goto _LL3;_LL2: goto _LL0;_LL3: _tmp12=_tmpF.f1;if(_tmp12 != Cyc_Absyn_ExternC)goto
_LL5;_tmp13=_tmpF.f2;if(_tmp13 != Cyc_Absyn_Extern)goto _LL5;_LL4: goto _LL6;_LL5:
_tmp14=_tmpF.f1;if(_tmp14 != Cyc_Absyn_Extern)goto _LL7;_tmp15=_tmpF.f2;if(_tmp15
!= Cyc_Absyn_ExternC)goto _LL7;_LL6: if(externCmerge)goto _LL0;goto _LL8;_LL7: _tmp16=
_tmpF.f1;if(_tmp16 != Cyc_Absyn_ExternC)goto _LL9;_LL8: goto _LLA;_LL9: _tmp17=_tmpF.f2;
if(_tmp17 != Cyc_Absyn_ExternC)goto _LLB;_LLA: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,
loc,msg);{struct _tuple9 _tmp1A5;return(_tmp1A5.f1=s1,((_tmp1A5.f2=0,_tmp1A5)));};
_LLB: _tmp18=_tmpF.f2;if(_tmp18 != Cyc_Absyn_Extern)goto _LLD;_LLC: s1=s0;goto _LL0;
_LLD: _tmp19=_tmpF.f1;if(_tmp19 != Cyc_Absyn_Extern)goto _LLF;_LLE: goto _LL0;_LLF:
_tmp1A=_tmpF.f1;if(_tmp1A != Cyc_Absyn_Static)goto _LL11;_tmp1B=_tmpF.f2;if(_tmp1B
!= Cyc_Absyn_Static)goto _LL11;_LL10: goto _LL12;_LL11: _tmp1C=_tmpF.f1;if(_tmp1C != 
Cyc_Absyn_Public)goto _LL13;_tmp1D=_tmpF.f2;if(_tmp1D != Cyc_Absyn_Public)goto
_LL13;_LL12: goto _LL14;_LL13: _tmp1E=_tmpF.f1;if(_tmp1E != Cyc_Absyn_Abstract)goto
_LL15;_tmp1F=_tmpF.f2;if(_tmp1F != Cyc_Absyn_Abstract)goto _LL15;_LL14: goto _LL0;
_LL15: _tmp20=_tmpF.f1;if(_tmp20 != Cyc_Absyn_Register)goto _LL17;_tmp21=_tmpF.f2;
if(_tmp21 != Cyc_Absyn_Register)goto _LL17;_LL16: goto _LL0;_LL17:;_LL18: Cyc_Tcdecl_merge_scope_err(
s0,s1,t,v,loc,msg);{struct _tuple9 _tmp1A6;return(_tmp1A6.f1=s1,((_tmp1A6.f2=0,
_tmp1A6)));};_LL0:;}{struct _tuple9 _tmp1A7;return(_tmp1A7.f1=s1,((_tmp1A7.f2=1,
_tmp1A7)));};}static int Cyc_Tcdecl_check_type(void*t0,void*t1);static int Cyc_Tcdecl_check_type(
void*t0,void*t1){return Cyc_Tcutil_unify(t0,t1);}static unsigned int Cyc_Tcdecl_get_uint_const_value(
struct Cyc_Absyn_Exp*e);static unsigned int Cyc_Tcdecl_get_uint_const_value(struct
Cyc_Absyn_Exp*e){void*_tmp25=e->r;union Cyc_Absyn_Cnst _tmp27;struct _tuple5 _tmp28;
int _tmp29;_LL1A: {struct Cyc_Absyn_Const_e_struct*_tmp26=(struct Cyc_Absyn_Const_e_struct*)
_tmp25;if(_tmp26->tag != 0)goto _LL1C;else{_tmp27=_tmp26->f1;if((_tmp27.Int_c).tag
!= 4)goto _LL1C;_tmp28=(struct _tuple5)(_tmp27.Int_c).val;_tmp29=_tmp28.f2;}}_LL1B:
return(unsigned int)_tmp29;_LL1C:;_LL1D: {struct Cyc_Core_Invalid_argument_struct
_tmp1AD;const char*_tmp1AC;struct Cyc_Core_Invalid_argument_struct*_tmp1AB;(int)
_throw((void*)((_tmp1AB=_cycalloc(sizeof(*_tmp1AB)),((_tmp1AB[0]=((_tmp1AD.tag=
Cyc_Core_Invalid_argument,((_tmp1AD.f1=((_tmp1AC="Tcdecl::get_uint_const_value",
_tag_dyneither(_tmp1AC,sizeof(char),29))),_tmp1AD)))),_tmp1AB)))));}_LL19:;}
static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,
struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg);inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*
tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct
Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tvs1)){{const char*_tmp1B2;void*_tmp1B1[2];struct Cyc_String_pa_struct _tmp1B0;
struct Cyc_String_pa_struct _tmp1AF;(_tmp1AF.tag=0,((_tmp1AF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1B0.tag=0,((_tmp1B0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1B1[0]=& _tmp1B0,((_tmp1B1[1]=&
_tmp1AF,Cyc_Tcdecl_merr(loc,msg,((_tmp1B2="%s %s has a different number of type parameters",
_tag_dyneither(_tmp1B2,sizeof(char),48))),_tag_dyneither(_tmp1B1,sizeof(void*),2)))))))))))));}
return 0;}{struct Cyc_List_List*_tmp31=tvs0;struct Cyc_List_List*_tmp32=tvs1;for(0;
_tmp31 != 0;(_tmp31=_tmp31->tl,_tmp32=_tmp32->tl)){Cyc_Tcutil_unify_kindbound(((
struct Cyc_Absyn_Tvar*)_tmp31->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmp32))->hd)->kind);{enum Cyc_Absyn_Kind _tmp33=Cyc_Absyn_force_kb(((
struct Cyc_Absyn_Tvar*)_tmp31->hd)->kind);enum Cyc_Absyn_Kind _tmp34=Cyc_Absyn_force_kb(((
struct Cyc_Absyn_Tvar*)_tmp32->hd)->kind);if(_tmp33 == _tmp34)continue;{const char*
_tmp1BA;void*_tmp1B9[5];struct Cyc_String_pa_struct _tmp1B8;struct Cyc_String_pa_struct
_tmp1B7;struct Cyc_String_pa_struct _tmp1B6;struct Cyc_String_pa_struct _tmp1B5;
struct Cyc_String_pa_struct _tmp1B4;(_tmp1B4.tag=0,((_tmp1B4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp34)),((_tmp1B5.tag=
0,((_tmp1B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
_tmp31->hd)->name),((_tmp1B6.tag=0,((_tmp1B6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmp33)),((_tmp1B7.tag=0,((_tmp1B7.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1B8.tag=0,((_tmp1B8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1B9[0]=& _tmp1B8,((_tmp1B9[1]=&
_tmp1B7,((_tmp1B9[2]=& _tmp1B6,((_tmp1B9[3]=& _tmp1B5,((_tmp1B9[4]=& _tmp1B4,Cyc_Tcdecl_merr(
loc,msg,((_tmp1BA="%s %s has a different kind (%s) for type parameter %s (%s)",
_tag_dyneither(_tmp1BA,sizeof(char),59))),_tag_dyneither(_tmp1B9,sizeof(void*),5)))))))))))))))))))))))))))))));}
return 0;};}return 1;};}static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,
struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*
atts0,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){if(!Cyc_Tcutil_same_atts(
atts0,atts1)){{const char*_tmp1BF;void*_tmp1BE[2];struct Cyc_String_pa_struct
_tmp1BD;struct Cyc_String_pa_struct _tmp1BC;(_tmp1BC.tag=0,((_tmp1BC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v),((_tmp1BD.tag=0,((_tmp1BD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t),((_tmp1BE[0]=& _tmp1BD,((_tmp1BE[1]=&
_tmp1BC,Cyc_Tcdecl_merr(loc,msg,((_tmp1BF="%s %s has different attributes",
_tag_dyneither(_tmp1BF,sizeof(char),31))),_tag_dyneither(_tmp1BE,sizeof(void*),2)))))))))))));}
return 0;}return 1;}struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};static struct
Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*
tvs1);static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*
tvs0,struct Cyc_List_List*tvs1){struct Cyc_List_List*inst=0;for(0;tvs0 != 0;(tvs0=
tvs0->tl,tvs1=tvs1->tl)){struct _tuple11*_tmp1C9;struct Cyc_Absyn_VarType_struct
_tmp1C8;struct Cyc_Absyn_VarType_struct*_tmp1C7;struct Cyc_List_List*_tmp1C6;inst=((
_tmp1C6=_cycalloc(sizeof(*_tmp1C6)),((_tmp1C6->hd=((_tmp1C9=_cycalloc(sizeof(*
_tmp1C9)),((_tmp1C9->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(tvs1))->hd,((_tmp1C9->f2=(void*)((_tmp1C7=_cycalloc(sizeof(*_tmp1C7)),((
_tmp1C7[0]=((_tmp1C8.tag=2,((_tmp1C8.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd,_tmp1C8)))),
_tmp1C7)))),_tmp1C9)))))),((_tmp1C6->tl=inst,_tmp1C6))))));}return inst;}struct
_tuple12{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
struct _tuple13{void*f1;void*f2;};struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(
struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){struct _dyneither_ptr _tmp44=Cyc_Absynpp_qvar2string(
d0->name);int _tmp45=1;if(!(d0->kind == d1->kind))return 0;{const char*_tmp1CA;if(!
Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp1CA="type",_tag_dyneither(_tmp1CA,
sizeof(char),5))),_tmp44,loc,msg))return 0;}{enum Cyc_Absyn_Scope _tmp49;int _tmp4A;
const char*_tmp1CB;struct _tuple9 _tmp48=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((
_tmp1CB="type",_tag_dyneither(_tmp1CB,sizeof(char),5))),_tmp44,loc,msg,1);_tmp49=
_tmp48.f1;_tmp4A=_tmp48.f2;if(!_tmp4A)_tmp45=0;{const char*_tmp1CC;if(!Cyc_Tcdecl_check_atts(
d0->attributes,d1->attributes,((_tmp1CC="type",_tag_dyneither(_tmp1CC,sizeof(
char),5))),_tmp44,loc,msg))_tmp45=0;}{struct Cyc_Absyn_Aggrdecl*d2;{struct
_tuple12 _tmp1CD;struct _tuple12 _tmp4D=(_tmp1CD.f1=d0->impl,((_tmp1CD.f2=d1->impl,
_tmp1CD)));struct Cyc_Absyn_AggrdeclImpl*_tmp4E;struct Cyc_Absyn_AggrdeclImpl*
_tmp4F;struct Cyc_Absyn_AggrdeclImpl*_tmp50;struct Cyc_Absyn_AggrdeclImpl _tmp51;
struct Cyc_List_List*_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_List_List*_tmp54;
int _tmp55;struct Cyc_Absyn_AggrdeclImpl*_tmp56;struct Cyc_Absyn_AggrdeclImpl _tmp57;
struct Cyc_List_List*_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;
int _tmp5B;_LL1F: _tmp4E=_tmp4D.f2;if(_tmp4E != 0)goto _LL21;_LL20: d2=d0;goto _LL1E;
_LL21: _tmp4F=_tmp4D.f1;if(_tmp4F != 0)goto _LL23;_LL22: d2=d1;goto _LL1E;_LL23:
_tmp50=_tmp4D.f1;if(_tmp50 == 0)goto _LL1E;_tmp51=*_tmp50;_tmp52=_tmp51.exist_vars;
_tmp53=_tmp51.rgn_po;_tmp54=_tmp51.fields;_tmp55=_tmp51.tagged;_tmp56=_tmp4D.f2;
if(_tmp56 == 0)goto _LL1E;_tmp57=*_tmp56;_tmp58=_tmp57.exist_vars;_tmp59=_tmp57.rgn_po;
_tmp5A=_tmp57.fields;_tmp5B=_tmp57.tagged;_LL24:{const char*_tmp1CE;if(!Cyc_Tcdecl_check_tvs(
_tmp52,_tmp58,((_tmp1CE="type",_tag_dyneither(_tmp1CE,sizeof(char),5))),_tmp44,
loc,msg))return 0;}{struct Cyc_List_List*_tmp5D=Cyc_Tcdecl_build_tvs_map(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
d0->tvs,_tmp52),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(d1->tvs,_tmp58));for(0;_tmp53 != 0  && _tmp59 != 0;(_tmp53=
_tmp53->tl,_tmp59=_tmp59->tl)){Cyc_Tcdecl_check_type((*((struct _tuple13*)_tmp53->hd)).f1,(*((
struct _tuple13*)_tmp59->hd)).f1);Cyc_Tcdecl_check_type((*((struct _tuple13*)
_tmp53->hd)).f2,(*((struct _tuple13*)_tmp59->hd)).f2);}for(0;_tmp54 != 0  && _tmp5A
!= 0;(_tmp54=_tmp54->tl,_tmp5A=_tmp5A->tl)){struct Cyc_Absyn_Aggrfield _tmp5F;
struct _dyneither_ptr*_tmp60;struct Cyc_Absyn_Tqual _tmp61;void*_tmp62;struct Cyc_Absyn_Exp*
_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_Absyn_Aggrfield*_tmp5E=(struct Cyc_Absyn_Aggrfield*)
_tmp54->hd;_tmp5F=*_tmp5E;_tmp60=_tmp5F.name;_tmp61=_tmp5F.tq;_tmp62=_tmp5F.type;
_tmp63=_tmp5F.width;_tmp64=_tmp5F.attributes;{struct Cyc_Absyn_Aggrfield _tmp66;
struct _dyneither_ptr*_tmp67;struct Cyc_Absyn_Tqual _tmp68;void*_tmp69;struct Cyc_Absyn_Exp*
_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_Absyn_Aggrfield*_tmp65=(struct Cyc_Absyn_Aggrfield*)
_tmp5A->hd;_tmp66=*_tmp65;_tmp67=_tmp66.name;_tmp68=_tmp66.tq;_tmp69=_tmp66.type;
_tmp6A=_tmp66.width;_tmp6B=_tmp66.attributes;if(Cyc_strptrcmp(_tmp60,_tmp67)!= 0){{
const char*_tmp1D7;void*_tmp1D6[4];const char*_tmp1D5;struct Cyc_String_pa_struct
_tmp1D4;struct Cyc_String_pa_struct _tmp1D3;struct Cyc_String_pa_struct _tmp1D2;
struct Cyc_String_pa_struct _tmp1D1;(_tmp1D1.tag=0,((_tmp1D1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp67),((_tmp1D2.tag=0,((_tmp1D2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp60),((_tmp1D3.tag=0,((_tmp1D3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp44),((_tmp1D4.tag=0,((_tmp1D4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1D5="type",_tag_dyneither(
_tmp1D5,sizeof(char),5)))),((_tmp1D6[0]=& _tmp1D4,((_tmp1D6[1]=& _tmp1D3,((_tmp1D6[
2]=& _tmp1D2,((_tmp1D6[3]=& _tmp1D1,Cyc_Tcdecl_merr(loc,msg,((_tmp1D7="%s %s : field name mismatch %s != %s",
_tag_dyneither(_tmp1D7,sizeof(char),37))),_tag_dyneither(_tmp1D6,sizeof(void*),4)))))))))))))))))))))))));}
return 0;}if(!Cyc_Tcutil_same_atts(_tmp64,_tmp6B)){{const char*_tmp1DF;void*
_tmp1DE[3];const char*_tmp1DD;struct Cyc_String_pa_struct _tmp1DC;struct Cyc_String_pa_struct
_tmp1DB;struct Cyc_String_pa_struct _tmp1DA;(_tmp1DA.tag=0,((_tmp1DA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp60),((_tmp1DB.tag=0,((_tmp1DB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp44),((_tmp1DC.tag=0,((_tmp1DC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1DD="type",_tag_dyneither(
_tmp1DD,sizeof(char),5)))),((_tmp1DE[0]=& _tmp1DC,((_tmp1DE[1]=& _tmp1DB,((_tmp1DE[
2]=& _tmp1DA,Cyc_Tcdecl_merr(loc,msg,((_tmp1DF="%s %s : attribute mismatch on field %s",
_tag_dyneither(_tmp1DF,sizeof(char),39))),_tag_dyneither(_tmp1DE,sizeof(void*),3)))))))))))))))))));}
_tmp45=0;}if(!Cyc_Tcutil_equal_tqual(_tmp61,_tmp68)){{const char*_tmp1E7;void*
_tmp1E6[3];const char*_tmp1E5;struct Cyc_String_pa_struct _tmp1E4;struct Cyc_String_pa_struct
_tmp1E3;struct Cyc_String_pa_struct _tmp1E2;(_tmp1E2.tag=0,((_tmp1E2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp60),((_tmp1E3.tag=0,((_tmp1E3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp44),((_tmp1E4.tag=0,((_tmp1E4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1E5="type",_tag_dyneither(
_tmp1E5,sizeof(char),5)))),((_tmp1E6[0]=& _tmp1E4,((_tmp1E6[1]=& _tmp1E3,((_tmp1E6[
2]=& _tmp1E2,Cyc_Tcdecl_merr(loc,msg,((_tmp1E7="%s %s : qualifier mismatch on field %s",
_tag_dyneither(_tmp1E7,sizeof(char),39))),_tag_dyneither(_tmp1E6,sizeof(void*),3)))))))))))))))))));}
_tmp45=0;}if(((_tmp63 != 0  && _tmp6A != 0) && Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_tmp63)!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_tmp6A) || _tmp63 == 0  && _tmp6A != 0) || _tmp63 != 0  && _tmp6A == 0){{const char*
_tmp1EF;void*_tmp1EE[3];const char*_tmp1ED;struct Cyc_String_pa_struct _tmp1EC;
struct Cyc_String_pa_struct _tmp1EB;struct Cyc_String_pa_struct _tmp1EA;(_tmp1EA.tag=
0,((_tmp1EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp60),((_tmp1EB.tag=
0,((_tmp1EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp44),((_tmp1EC.tag=
0,((_tmp1EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)((_tmp1ED="type",
_tag_dyneither(_tmp1ED,sizeof(char),5)))),((_tmp1EE[0]=& _tmp1EC,((_tmp1EE[1]=&
_tmp1EB,((_tmp1EE[2]=& _tmp1EA,Cyc_Tcdecl_merr(loc,msg,((_tmp1EF="%s %s : bitfield mismatch on field %s",
_tag_dyneither(_tmp1EF,sizeof(char),38))),_tag_dyneither(_tmp1EE,sizeof(void*),3)))))))))))))))))));}
_tmp45=0;}{void*subst_t1=Cyc_Tcutil_substitute(_tmp5D,_tmp69);if(!Cyc_Tcdecl_check_type(
_tmp62,subst_t1)){{const char*_tmp1F6;void*_tmp1F5[4];struct Cyc_String_pa_struct
_tmp1F4;struct Cyc_String_pa_struct _tmp1F3;struct Cyc_String_pa_struct _tmp1F2;
struct Cyc_String_pa_struct _tmp1F1;(_tmp1F1.tag=0,((_tmp1F1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1)),((
_tmp1F2.tag=0,((_tmp1F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp62)),((_tmp1F3.tag=0,((_tmp1F3.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp60),((_tmp1F4.tag=0,((_tmp1F4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp44),((_tmp1F5[0]=& _tmp1F4,((_tmp1F5[1]=& _tmp1F3,((
_tmp1F5[2]=& _tmp1F2,((_tmp1F5[3]=& _tmp1F1,Cyc_Tcdecl_merr(loc,msg,((_tmp1F6="type %s : type mismatch on field %s: %s != %s",
_tag_dyneither(_tmp1F6,sizeof(char),46))),_tag_dyneither(_tmp1F5,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();_tmp45=0;}};};}if(_tmp54 != 0){{const char*_tmp1FB;
void*_tmp1FA[2];struct Cyc_String_pa_struct _tmp1F9;struct Cyc_String_pa_struct
_tmp1F8;(_tmp1F8.tag=0,((_tmp1F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)_tmp54->hd)->name),((_tmp1F9.tag=0,((_tmp1F9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp44),((_tmp1FA[0]=& _tmp1F9,((
_tmp1FA[1]=& _tmp1F8,Cyc_Tcdecl_merr(loc,msg,((_tmp1FB="type %s is missing field %s",
_tag_dyneither(_tmp1FB,sizeof(char),28))),_tag_dyneither(_tmp1FA,sizeof(void*),2)))))))))))));}
_tmp45=0;}if(_tmp5A != 0){{const char*_tmp200;void*_tmp1FF[2];struct Cyc_String_pa_struct
_tmp1FE;struct Cyc_String_pa_struct _tmp1FD;(_tmp1FD.tag=0,((_tmp1FD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp5A->hd)->name),((
_tmp1FE.tag=0,((_tmp1FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp44),((
_tmp1FF[0]=& _tmp1FE,((_tmp1FF[1]=& _tmp1FD,Cyc_Tcdecl_merr(loc,msg,((_tmp200="type %s has extra field %s",
_tag_dyneither(_tmp200,sizeof(char),27))),_tag_dyneither(_tmp1FF,sizeof(void*),2)))))))))))));}
_tmp45=0;}if(_tmp55 != _tmp5B){{const char*_tmp204;void*_tmp203[1];struct Cyc_String_pa_struct
_tmp202;(_tmp202.tag=0,((_tmp202.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp44),((_tmp203[0]=& _tmp202,Cyc_Tcdecl_merr(loc,msg,((_tmp204="%s : previous declaration disagrees with @tagged qualifier",
_tag_dyneither(_tmp204,sizeof(char),59))),_tag_dyneither(_tmp203,sizeof(void*),1)))))));}
_tmp45=0;}d2=d0;goto _LL1E;};_LL1E:;}if(!_tmp45)return 0;if(_tmp49 == d2->sc)return(
struct Cyc_Absyn_Aggrdecl*)d2;else{{struct Cyc_Absyn_Aggrdecl*_tmp205;d2=((_tmp205=
_cycalloc(sizeof(*_tmp205)),((_tmp205[0]=*d2,_tmp205))));}d2->sc=_tmp49;return(
struct Cyc_Absyn_Aggrdecl*)d2;}};};}static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(
int is_x);inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){const
char*_tmp207;const char*_tmp206;return is_x?(_tmp207="@extensible datatype",
_tag_dyneither(_tmp207,sizeof(char),21)):((_tmp206="datatype",_tag_dyneither(
_tmp206,sizeof(char),9)));}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*
f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg);static struct Cyc_Absyn_Datatypefield*
Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*
f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct
_dyneither_ptr*msg){struct Cyc_Position_Segment*loc=f1->loc;if(Cyc_strptrcmp((*f0->name).f2,(*
f1->name).f2)!= 0){{const char*_tmp20E;void*_tmp20D[4];struct Cyc_String_pa_struct
_tmp20C;struct Cyc_String_pa_struct _tmp20B;struct Cyc_String_pa_struct _tmp20A;
struct Cyc_String_pa_struct _tmp209;(_tmp209.tag=0,((_tmp209.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2),((_tmp20A.tag=0,((_tmp20A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2),((_tmp20B.tag=0,((
_tmp20B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp20C.tag=0,((
_tmp20C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp20D[0]=& _tmp20C,((
_tmp20D[1]=& _tmp20B,((_tmp20D[2]=& _tmp20A,((_tmp20D[3]=& _tmp209,Cyc_Tcdecl_merr(
loc,msg,((_tmp20E="%s %s: field name mismatch %s != %s",_tag_dyneither(_tmp20E,
sizeof(char),36))),_tag_dyneither(_tmp20D,sizeof(void*),4)))))))))))))))))))))))));}
return 0;}{struct _dyneither_ptr _tmp9F=*(*f0->name).f2;enum Cyc_Absyn_Scope _tmpA5;
int _tmpA6;const char*_tmp213;void*_tmp212[2];struct Cyc_String_pa_struct _tmp211;
struct Cyc_String_pa_struct _tmp210;struct _tuple9 _tmpA4=Cyc_Tcdecl_merge_scope(f0->sc,
f1->sc,(struct _dyneither_ptr)((_tmp210.tag=0,((_tmp210.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)v),((_tmp211.tag=0,((_tmp211.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t),((_tmp212[0]=& _tmp211,((_tmp212[1]=& _tmp210,Cyc_aprintf(((
_tmp213="in %s %s, field",_tag_dyneither(_tmp213,sizeof(char),16))),
_tag_dyneither(_tmp212,sizeof(void*),2)))))))))))))),_tmp9F,loc,msg,0);_tmpA5=
_tmpA4.f1;_tmpA6=_tmpA4.f2;{struct Cyc_List_List*_tmpA7=f0->typs;struct Cyc_List_List*
_tmpA8=f1->typs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA7)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpA8)){{const char*_tmp219;void*
_tmp218[3];struct Cyc_String_pa_struct _tmp217;struct Cyc_String_pa_struct _tmp216;
struct Cyc_String_pa_struct _tmp215;(_tmp215.tag=0,((_tmp215.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp9F),((_tmp216.tag=0,((_tmp216.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp217.tag=0,((_tmp217.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp218[0]=& _tmp217,((_tmp218[1]=&
_tmp216,((_tmp218[2]=& _tmp215,Cyc_Tcdecl_merr(loc,msg,((_tmp219="%s %s, field %s: parameter number mismatch",
_tag_dyneither(_tmp219,sizeof(char),43))),_tag_dyneither(_tmp218,sizeof(void*),3)))))))))))))))))));}
_tmpA6=0;}for(0;_tmpA7 != 0;(_tmpA7=_tmpA7->tl,_tmpA8=_tmpA8->tl)){if(!Cyc_Tcutil_equal_tqual((*((
struct _tuple14*)_tmpA7->hd)).f1,(*((struct _tuple14*)((struct Cyc_List_List*)
_check_null(_tmpA8))->hd)).f1)){{const char*_tmp21F;void*_tmp21E[3];struct Cyc_String_pa_struct
_tmp21D;struct Cyc_String_pa_struct _tmp21C;struct Cyc_String_pa_struct _tmp21B;(
_tmp21B.tag=0,((_tmp21B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9F),((
_tmp21C.tag=0,((_tmp21C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((
_tmp21D.tag=0,((_tmp21D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp21E[0]=& _tmp21D,((_tmp21E[1]=& _tmp21C,((_tmp21E[2]=& _tmp21B,Cyc_Tcdecl_merr(
loc,msg,((_tmp21F="%s %s, field %s: parameter qualifier",_tag_dyneither(_tmp21F,
sizeof(char),37))),_tag_dyneither(_tmp21E,sizeof(void*),3)))))))))))))))))));}
_tmpA6=0;}{void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple14*)_tmpA8->hd)).f2);
if(!Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmpA7->hd)).f2,subst_t1)){{const
char*_tmp227;void*_tmp226[5];struct Cyc_String_pa_struct _tmp225;struct Cyc_String_pa_struct
_tmp224;struct Cyc_String_pa_struct _tmp223;struct Cyc_String_pa_struct _tmp222;
struct Cyc_String_pa_struct _tmp221;(_tmp221.tag=0,((_tmp221.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1)),((
_tmp222.tag=0,((_tmp222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((
struct _tuple14*)_tmpA7->hd)).f2)),((_tmp223.tag=0,((_tmp223.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp9F),((_tmp224.tag=0,((_tmp224.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp225.tag=0,((_tmp225.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp226[0]=& _tmp225,((_tmp226[1]=&
_tmp224,((_tmp226[2]=& _tmp223,((_tmp226[3]=& _tmp222,((_tmp226[4]=& _tmp221,Cyc_Tcdecl_merr(
loc,msg,((_tmp227="%s %s, field %s: parameter type mismatch %s != %s",
_tag_dyneither(_tmp227,sizeof(char),50))),_tag_dyneither(_tmp226,sizeof(void*),5)))))))))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();_tmpA6=0;}};}if(!_tmpA6)return 0;if(f0->sc != _tmpA5){
struct Cyc_Absyn_Datatypefield*_tmp228;struct Cyc_Absyn_Datatypefield*_tmpBA=(
_tmp228=_cycalloc(sizeof(*_tmp228)),((_tmp228[0]=*f0,_tmp228)));((struct Cyc_Absyn_Datatypefield*)
_check_null(_tmpBA))->sc=_tmpA5;return _tmpBA;}else{return(struct Cyc_Absyn_Datatypefield*)
f0;}};};}static struct _tuple14*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*
inst,struct _tuple14*x);static struct _tuple14*Cyc_Tcdecl_substitute_datatypefield_f2(
struct Cyc_List_List*inst,struct _tuple14*x){struct _tuple14 _tmpBD;struct Cyc_Absyn_Tqual
_tmpBE;void*_tmpBF;struct _tuple14*_tmpBC=x;_tmpBD=*_tmpBC;_tmpBE=_tmpBD.f1;
_tmpBF=_tmpBD.f2;{struct _tuple14*_tmp229;return(_tmp229=_cycalloc(sizeof(*
_tmp229)),((_tmp229->f1=_tmpBE,((_tmp229->f2=Cyc_Tcutil_substitute(inst,_tmpBF),
_tmp229)))));};}static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(
struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1);static struct Cyc_Absyn_Datatypefield*
Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*
f1){struct Cyc_Absyn_Datatypefield*_tmp22A;struct Cyc_Absyn_Datatypefield*_tmpC1=(
_tmp22A=_cycalloc(sizeof(*_tmp22A)),((_tmp22A[0]=*f1,_tmp22A)));_tmpC1->typs=((
struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct Cyc_List_List*,struct _tuple14*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,
inst1,f1->typs);return _tmpC1;}static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(
struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct
Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(
struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct
Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct Cyc_List_List**_tmp22B;struct Cyc_List_List**f2sp=(
_tmp22B=_cycalloc(sizeof(*_tmp22B)),((_tmp22B[0]=0,_tmp22B)));struct Cyc_List_List**
_tmpC3=f2sp;int cmp=- 1;for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){while(f0s != 0  && (
cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct
Cyc_Absyn_Datatypefield*)f1s->hd)->name))< 0){struct Cyc_List_List*_tmp22C;struct
Cyc_List_List*_tmpC4=(_tmp22C=_cycalloc(sizeof(*_tmp22C)),((_tmp22C->hd=(struct
Cyc_Absyn_Datatypefield*)f0s->hd,((_tmp22C->tl=0,_tmp22C)))));*_tmpC3=_tmpC4;
_tmpC3=&((struct Cyc_List_List*)_check_null(_tmpC4))->tl;f0s=f0s->tl;}if(f0s == 0
 || cmp > 0){*incl=0;{struct Cyc_List_List*_tmp22D;struct Cyc_List_List*_tmpC6=(
_tmp22D=_cycalloc(sizeof(*_tmp22D)),((_tmp22D->hd=Cyc_Tcdecl_substitute_datatypefield(
inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd),((_tmp22D->tl=0,_tmp22D)))));*
_tmpC3=_tmpC6;_tmpC3=&((struct Cyc_List_List*)_check_null(_tmpC6))->tl;};}else{
struct Cyc_Absyn_Datatypefield*_tmpC8=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)
f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);if(_tmpC8 != 0){if(
_tmpC8 != (struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)f0s->hd))*
incl=0;{struct Cyc_List_List*_tmp22E;struct Cyc_List_List*_tmpC9=(_tmp22E=
_cycalloc(sizeof(*_tmp22E)),((_tmp22E->hd=(struct Cyc_Absyn_Datatypefield*)_tmpC8,((
_tmp22E->tl=0,_tmp22E)))));*_tmpC3=_tmpC9;_tmpC3=&((struct Cyc_List_List*)
_check_null(_tmpC9))->tl;};}else{*res=0;}f0s=f0s->tl;}}if(f1s != 0){*incl=0;*
_tmpC3=f1s;}else{*_tmpC3=f0s;}return*f2sp;}struct _tuple15{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;};static struct _tuple15 Cyc_Tcdecl_split(struct Cyc_List_List*
f);static struct _tuple15 Cyc_Tcdecl_split(struct Cyc_List_List*f){if(f == 0){struct
_tuple15 _tmp22F;return(_tmp22F.f1=0,((_tmp22F.f2=0,_tmp22F)));}if(f->tl == 0){
struct _tuple15 _tmp230;return(_tmp230.f1=f,((_tmp230.f2=0,_tmp230)));}{struct Cyc_List_List*
_tmpCF;struct Cyc_List_List*_tmpD0;struct _tuple15 _tmpCE=Cyc_Tcdecl_split(((struct
Cyc_List_List*)_check_null(f->tl))->tl);_tmpCF=_tmpCE.f1;_tmpD0=_tmpCE.f2;{
struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp234;struct _tuple15 _tmp233;
return(_tmp233.f1=((_tmp234=_cycalloc(sizeof(*_tmp234)),((_tmp234->hd=(void*)((
void*)f->hd),((_tmp234->tl=_tmpCF,_tmp234)))))),((_tmp233.f2=((_tmp235=_cycalloc(
sizeof(*_tmp235)),((_tmp235->hd=(void*)((void*)((struct Cyc_List_List*)
_check_null(f->tl))->hd),((_tmp235->tl=_tmpD0,_tmp235)))))),_tmp233)));};};}
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*
res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*
msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,
int*res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct Cyc_List_List*_tmpD5;struct Cyc_List_List*_tmpD6;struct
_tuple15 _tmpD4=((struct _tuple15(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);
_tmpD5=_tmpD4.f1;_tmpD6=_tmpD4.f2;if(_tmpD5 != 0  && _tmpD5->tl != 0)_tmpD5=Cyc_Tcdecl_sort_xdatatype_fields(
_tmpD5,res,v,loc,msg);if(_tmpD6 != 0  && _tmpD6->tl != 0)_tmpD6=Cyc_Tcdecl_sort_xdatatype_fields(
_tmpD6,res,v,loc,msg);{const char*_tmp237;int*_tmp236;return Cyc_Tcdecl_merge_xdatatype_fields(
_tmpD5,_tmpD6,0,0,0,res,((_tmp236=_cycalloc_atomic(sizeof(*_tmp236)),((_tmp236[0]=
1,_tmp236)))),((_tmp237="@extensible datatype",_tag_dyneither(_tmp237,sizeof(
char),21))),*v,loc,msg);};}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;};struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(
struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){struct _dyneither_ptr _tmpD9=Cyc_Absynpp_qvar2string(
d0->name);const char*_tmp238;struct _dyneither_ptr t=(_tmp238="datatype",
_tag_dyneither(_tmp238,sizeof(char),9));int _tmpDA=1;if(d0->is_extensible != d1->is_extensible){{
const char*_tmp23E;void*_tmp23D[3];struct Cyc_String_pa_struct _tmp23C;struct Cyc_String_pa_struct
_tmp23B;struct Cyc_String_pa_struct _tmp23A;(_tmp23A.tag=0,((_tmp23A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d1->is_extensible)),((
_tmp23B.tag=0,((_tmp23B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(
d0->is_extensible)),((_tmp23C.tag=0,((_tmp23C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmpD9),((_tmp23D[0]=& _tmp23C,((_tmp23D[1]=& _tmp23B,((_tmp23D[2]=&
_tmp23A,Cyc_Tcdecl_merr(loc,msg,((_tmp23E="expected %s to be a %s instead of a %s",
_tag_dyneither(_tmp23E,sizeof(char),39))),_tag_dyneither(_tmp23D,sizeof(void*),3)))))))))))))))))));}
_tmpDA=0;}else{t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}if(!Cyc_Tcdecl_check_tvs(
d0->tvs,d1->tvs,t,_tmpD9,loc,msg))return 0;{enum Cyc_Absyn_Scope _tmpE1;int _tmpE2;
struct _tuple9 _tmpE0=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpD9,loc,msg,0);
_tmpE1=_tmpE0.f1;_tmpE2=_tmpE0.f2;if(!_tmpE2)_tmpDA=0;{struct Cyc_Absyn_Datatypedecl*
d2;{struct _tuple16 _tmp23F;struct _tuple16 _tmpE4=(_tmp23F.f1=d0->fields,((_tmp23F.f2=
d1->fields,_tmp23F)));struct Cyc_Core_Opt*_tmpE5;struct Cyc_Core_Opt*_tmpE6;struct
Cyc_Core_Opt*_tmpE7;struct Cyc_Core_Opt _tmpE8;struct Cyc_List_List*_tmpE9;struct
Cyc_Core_Opt*_tmpEA;struct Cyc_Core_Opt _tmpEB;struct Cyc_List_List*_tmpEC;_LL26:
_tmpE5=_tmpE4.f2;if(_tmpE5 != 0)goto _LL28;_LL27: d2=d0;goto _LL25;_LL28: _tmpE6=
_tmpE4.f1;if(_tmpE6 != 0)goto _LL2A;_LL29: d2=d1;goto _LL25;_LL2A: _tmpE7=_tmpE4.f1;
if(_tmpE7 == 0)goto _LL25;_tmpE8=*_tmpE7;_tmpE9=(struct Cyc_List_List*)_tmpE8.v;
_tmpEA=_tmpE4.f2;if(_tmpEA == 0)goto _LL25;_tmpEB=*_tmpEA;_tmpEC=(struct Cyc_List_List*)
_tmpEB.v;_LL2B: {struct Cyc_List_List*_tmpED=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
if(d0->is_extensible){int _tmpEE=1;struct Cyc_List_List*_tmpEF=Cyc_Tcdecl_merge_xdatatype_fields(
_tmpE9,_tmpEC,_tmpED,d0->tvs,d1->tvs,& _tmpDA,& _tmpEE,t,_tmpD9,loc,msg);if(_tmpEE)
d2=d0;else{{struct Cyc_Absyn_Datatypedecl*_tmp240;d2=((_tmp240=_cycalloc(sizeof(*
_tmp240)),((_tmp240[0]=*d0,_tmp240))));}d2->sc=_tmpE1;{struct Cyc_Core_Opt*
_tmp241;d2->fields=((_tmp241=_cycalloc(sizeof(*_tmp241)),((_tmp241->v=_tmpEF,
_tmp241))));};}}else{for(0;_tmpE9 != 0  && _tmpEC != 0;(_tmpE9=_tmpE9->tl,_tmpEC=
_tmpEC->tl)){Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)
_tmpE9->hd,(struct Cyc_Absyn_Datatypefield*)_tmpEC->hd,_tmpED,t,_tmpD9,msg);}if(
_tmpE9 != 0){{const char*_tmp247;void*_tmp246[3];struct Cyc_String_pa_struct _tmp245;
struct Cyc_String_pa_struct _tmp244;struct Cyc_String_pa_struct _tmp243;(_tmp243.tag=
0,((_tmp243.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)((
struct Cyc_List_List*)_check_null(_tmpEC))->hd)->name).f2),((_tmp244.tag=0,((
_tmp244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD9),((_tmp245.tag=0,((
_tmp245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp246[0]=& _tmp245,((
_tmp246[1]=& _tmp244,((_tmp246[2]=& _tmp243,Cyc_Tcdecl_merr(loc,msg,((_tmp247="%s %s has extra field %s",
_tag_dyneither(_tmp247,sizeof(char),25))),_tag_dyneither(_tmp246,sizeof(void*),3)))))))))))))))))));}
_tmpDA=0;}if(_tmpEC != 0){{const char*_tmp24D;void*_tmp24C[3];struct Cyc_String_pa_struct
_tmp24B;struct Cyc_String_pa_struct _tmp24A;struct Cyc_String_pa_struct _tmp249;(
_tmp249.tag=0,((_tmp249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((
struct Cyc_Absyn_Datatypefield*)_tmpEC->hd)->name).f2),((_tmp24A.tag=0,((_tmp24A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD9),((_tmp24B.tag=0,((_tmp24B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp24C[0]=& _tmp24B,((_tmp24C[1]=&
_tmp24A,((_tmp24C[2]=& _tmp249,Cyc_Tcdecl_merr(loc,msg,((_tmp24D="%s %s is missing field %s",
_tag_dyneither(_tmp24D,sizeof(char),26))),_tag_dyneither(_tmp24C,sizeof(void*),3)))))))))))))))))));}
_tmpDA=0;}d2=d0;}goto _LL25;}_LL25:;}if(!_tmpDA)return 0;if(_tmpE1 == d2->sc)return(
struct Cyc_Absyn_Datatypedecl*)d2;else{{struct Cyc_Absyn_Datatypedecl*_tmp24E;d2=((
_tmp24E=_cycalloc(sizeof(*_tmp24E)),((_tmp24E[0]=*d2,_tmp24E))));}d2->sc=_tmpE1;
return(struct Cyc_Absyn_Datatypedecl*)d2;}};};}struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){struct _dyneither_ptr _tmpFE=Cyc_Absynpp_qvar2string(
d0->name);int _tmpFF=1;enum Cyc_Absyn_Scope _tmp102;int _tmp103;const char*_tmp24F;
struct _tuple9 _tmp101=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,((_tmp24F="enum",
_tag_dyneither(_tmp24F,sizeof(char),5))),_tmpFE,loc,msg,1);_tmp102=_tmp101.f1;
_tmp103=_tmp101.f2;if(!_tmp103)_tmpFF=0;{struct Cyc_Absyn_Enumdecl*d2;{struct
_tuple16 _tmp250;struct _tuple16 _tmp105=(_tmp250.f1=d0->fields,((_tmp250.f2=d1->fields,
_tmp250)));struct Cyc_Core_Opt*_tmp106;struct Cyc_Core_Opt*_tmp107;struct Cyc_Core_Opt*
_tmp108;struct Cyc_Core_Opt _tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_Core_Opt*
_tmp10B;struct Cyc_Core_Opt _tmp10C;struct Cyc_List_List*_tmp10D;_LL2D: _tmp106=
_tmp105.f2;if(_tmp106 != 0)goto _LL2F;_LL2E: d2=d0;goto _LL2C;_LL2F: _tmp107=_tmp105.f1;
if(_tmp107 != 0)goto _LL31;_LL30: d2=d1;goto _LL2C;_LL31: _tmp108=_tmp105.f1;if(
_tmp108 == 0)goto _LL2C;_tmp109=*_tmp108;_tmp10A=(struct Cyc_List_List*)_tmp109.v;
_tmp10B=_tmp105.f2;if(_tmp10B == 0)goto _LL2C;_tmp10C=*_tmp10B;_tmp10D=(struct Cyc_List_List*)
_tmp10C.v;_LL32: for(0;_tmp10A != 0  && _tmp10D != 0;(_tmp10A=_tmp10A->tl,_tmp10D=
_tmp10D->tl)){struct Cyc_Absyn_Enumfield _tmp10F;struct _tuple0*_tmp110;struct
_tuple0 _tmp111;struct _dyneither_ptr*_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct
Cyc_Position_Segment*_tmp114;struct Cyc_Absyn_Enumfield*_tmp10E=(struct Cyc_Absyn_Enumfield*)
_tmp10A->hd;_tmp10F=*_tmp10E;_tmp110=_tmp10F.name;_tmp111=*_tmp110;_tmp112=
_tmp111.f2;_tmp113=_tmp10F.tag;_tmp114=_tmp10F.loc;{struct Cyc_Absyn_Enumfield
_tmp116;struct _tuple0*_tmp117;struct _tuple0 _tmp118;struct _dyneither_ptr*_tmp119;
struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Position_Segment*_tmp11B;struct Cyc_Absyn_Enumfield*
_tmp115=(struct Cyc_Absyn_Enumfield*)_tmp10D->hd;_tmp116=*_tmp115;_tmp117=_tmp116.name;
_tmp118=*_tmp117;_tmp119=_tmp118.f2;_tmp11A=_tmp116.tag;_tmp11B=_tmp116.loc;if(
Cyc_strptrcmp(_tmp112,_tmp119)!= 0){{const char*_tmp256;void*_tmp255[3];struct Cyc_String_pa_struct
_tmp254;struct Cyc_String_pa_struct _tmp253;struct Cyc_String_pa_struct _tmp252;(
_tmp252.tag=0,((_tmp252.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp119),((
_tmp253.tag=0,((_tmp253.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp112),((
_tmp254.tag=0,((_tmp254.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFE),((
_tmp255[0]=& _tmp254,((_tmp255[1]=& _tmp253,((_tmp255[2]=& _tmp252,Cyc_Tcdecl_merr(
loc,msg,((_tmp256="enum %s: field name mismatch %s != %s",_tag_dyneither(_tmp256,
sizeof(char),38))),_tag_dyneither(_tmp255,sizeof(void*),3)))))))))))))))))));}
_tmpFF=0;}if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(
_tmp113))!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(
_tmp11A))){{const char*_tmp25B;void*_tmp25A[2];struct Cyc_String_pa_struct _tmp259;
struct Cyc_String_pa_struct _tmp258;(_tmp258.tag=0,((_tmp258.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp119),((_tmp259.tag=0,((_tmp259.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFE),((_tmp25A[0]=& _tmp259,((
_tmp25A[1]=& _tmp258,Cyc_Tcdecl_merr(loc,msg,((_tmp25B="enum %s, field %s, value mismatch",
_tag_dyneither(_tmp25B,sizeof(char),34))),_tag_dyneither(_tmp25A,sizeof(void*),2)))))))))))));}
_tmpFF=0;}};}d2=d0;goto _LL2C;_LL2C:;}if(!_tmpFF)return 0;if(d2->sc == _tmp102)
return(struct Cyc_Absyn_Enumdecl*)d2;else{{struct Cyc_Absyn_Enumdecl*_tmp25C;d2=((
_tmp25C=_cycalloc(sizeof(*_tmp25C)),((_tmp25C[0]=*d2,_tmp25C))));}d2->sc=_tmp102;
return(struct Cyc_Absyn_Enumdecl*)d2;}};}static struct _tuple9 Cyc_Tcdecl_check_var_or_fn_decl(
enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*
atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*
atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg);static struct _tuple9 Cyc_Tcdecl_check_var_or_fn_decl(
enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*
atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*
atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg){int _tmp126=1;enum Cyc_Absyn_Scope _tmp128;int _tmp129;
struct _tuple9 _tmp127=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);_tmp128=
_tmp127.f1;_tmp129=_tmp127.f2;if(!_tmp129)_tmp126=0;if(!Cyc_Tcdecl_check_type(t0,
t1)){{const char*_tmp263;void*_tmp262[4];struct Cyc_String_pa_struct _tmp261;struct
Cyc_String_pa_struct _tmp260;struct Cyc_String_pa_struct _tmp25F;struct Cyc_String_pa_struct
_tmp25E;(_tmp25E.tag=0,((_tmp25E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t0)),((_tmp25F.tag=0,((_tmp25F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp260.tag=0,((_tmp260.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp261.tag=0,((_tmp261.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp262[0]=& _tmp261,((_tmp262[1]=&
_tmp260,((_tmp262[2]=& _tmp25F,((_tmp262[3]=& _tmp25E,Cyc_Tcdecl_merr(loc,msg,((
_tmp263="%s %s has type \n%s\n instead of \n%s\n",_tag_dyneither(_tmp263,sizeof(
char),36))),_tag_dyneither(_tmp262,sizeof(void*),4)))))))))))))))))))))))));}Cyc_Tcutil_explain_failure();
_tmp126=0;}if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){{const char*_tmp268;void*_tmp267[
2];struct Cyc_String_pa_struct _tmp266;struct Cyc_String_pa_struct _tmp265;(_tmp265.tag=
0,((_tmp265.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((_tmp266.tag=0,((
_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((_tmp267[0]=& _tmp266,((
_tmp267[1]=& _tmp265,Cyc_Tcdecl_merr(loc,msg,((_tmp268="%s %s has different type qualifiers",
_tag_dyneither(_tmp268,sizeof(char),36))),_tag_dyneither(_tmp267,sizeof(void*),2)))))))))))));}
_tmp126=0;}if(!Cyc_Tcutil_same_atts(atts0,atts1)){{const char*_tmp26D;void*
_tmp26C[2];struct Cyc_String_pa_struct _tmp26B;struct Cyc_String_pa_struct _tmp26A;(
_tmp26A.tag=0,((_tmp26A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v),((
_tmp26B.tag=0,((_tmp26B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t),((
_tmp26C[0]=& _tmp26B,((_tmp26C[1]=& _tmp26A,Cyc_Tcdecl_merr(loc,msg,((_tmp26D="%s %s has different attributes",
_tag_dyneither(_tmp26D,sizeof(char),31))),_tag_dyneither(_tmp26C,sizeof(void*),2)))))))))))));}{
const char*_tmp270;void*_tmp26F;(_tmp26F=0,Cyc_fprintf(Cyc_stderr,((_tmp270="previous attributes: ",
_tag_dyneither(_tmp270,sizeof(char),22))),_tag_dyneither(_tmp26F,sizeof(void*),0)));}
for(0;atts0 != 0;atts0=atts0->tl){const char*_tmp274;void*_tmp273[1];struct Cyc_String_pa_struct
_tmp272;(_tmp272.tag=0,((_tmp272.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts0->hd)),((_tmp273[0]=& _tmp272,Cyc_fprintf(
Cyc_stderr,((_tmp274="%s ",_tag_dyneither(_tmp274,sizeof(char),4))),
_tag_dyneither(_tmp273,sizeof(void*),1)))))));}{const char*_tmp277;void*_tmp276;(
_tmp276=0,Cyc_fprintf(Cyc_stderr,((_tmp277="\ncurrent attributes: ",
_tag_dyneither(_tmp277,sizeof(char),22))),_tag_dyneither(_tmp276,sizeof(void*),0)));}
for(0;atts1 != 0;atts1=atts1->tl){const char*_tmp27B;void*_tmp27A[1];struct Cyc_String_pa_struct
_tmp279;(_tmp279.tag=0,((_tmp279.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts1->hd)),((_tmp27A[0]=& _tmp279,Cyc_fprintf(
Cyc_stderr,((_tmp27B="%s ",_tag_dyneither(_tmp27B,sizeof(char),4))),
_tag_dyneither(_tmp27A,sizeof(void*),1)))))));}{const char*_tmp27E;void*_tmp27D;(
_tmp27D=0,Cyc_fprintf(Cyc_stderr,((_tmp27E="\n",_tag_dyneither(_tmp27E,sizeof(
char),2))),_tag_dyneither(_tmp27D,sizeof(void*),0)));}_tmp126=0;}{struct _tuple9
_tmp27F;return(_tmp27F.f1=_tmp128,((_tmp27F.f2=_tmp126,_tmp27F)));};}struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct _dyneither_ptr
_tmp145=Cyc_Absynpp_qvar2string(d0->name);enum Cyc_Absyn_Scope _tmp148;int _tmp149;
const char*_tmp280;struct _tuple9 _tmp147=Cyc_Tcdecl_check_var_or_fn_decl(d0->sc,d0->type,
d0->tq,d0->attributes,d1->sc,d1->type,d1->tq,d1->attributes,((_tmp280="variable",
_tag_dyneither(_tmp280,sizeof(char),9))),_tmp145,loc,msg);_tmp148=_tmp147.f1;
_tmp149=_tmp147.f2;if(!_tmp149)return 0;if(d0->sc == _tmp148)return(struct Cyc_Absyn_Vardecl*)
d0;else{struct Cyc_Absyn_Vardecl*_tmp281;struct Cyc_Absyn_Vardecl*_tmp14A=(_tmp281=
_cycalloc(sizeof(*_tmp281)),((_tmp281[0]=*d0,_tmp281)));((struct Cyc_Absyn_Vardecl*)
_check_null(_tmp14A))->sc=_tmp148;return _tmp14A;}}struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct _dyneither_ptr
_tmp14C=Cyc_Absynpp_qvar2string(d0->name);{const char*_tmp282;if(!Cyc_Tcdecl_check_tvs(
d0->tvs,d1->tvs,((_tmp282="typedef",_tag_dyneither(_tmp282,sizeof(char),8))),
_tmp14C,loc,msg))return 0;}{struct Cyc_List_List*_tmp14E=Cyc_Tcdecl_build_tvs_map(
d0->tvs,d1->tvs);if(d0->defn != 0  && d1->defn != 0){void*subst_defn1=Cyc_Tcutil_substitute(
_tmp14E,(void*)((struct Cyc_Core_Opt*)_check_null(d1->defn))->v);if(!Cyc_Tcdecl_check_type((
void*)((struct Cyc_Core_Opt*)_check_null(d0->defn))->v,subst_defn1)){{const char*
_tmp288;void*_tmp287[3];struct Cyc_String_pa_struct _tmp286;struct Cyc_String_pa_struct
_tmp285;struct Cyc_String_pa_struct _tmp284;(_tmp284.tag=0,((_tmp284.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(d0->defn))->v)),((_tmp285.tag=0,((_tmp285.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1)),((_tmp286.tag=0,((
_tmp286.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp14C),((_tmp287[0]=&
_tmp286,((_tmp287[1]=& _tmp285,((_tmp287[2]=& _tmp284,Cyc_Tcdecl_merr(loc,msg,((
_tmp288="typedef %s does not refer to the same type: %s != %s",_tag_dyneither(
_tmp288,sizeof(char),53))),_tag_dyneither(_tmp287,sizeof(void*),3)))))))))))))))))));}
return 0;}}return(struct Cyc_Absyn_Typedefdecl*)d0;};}void*Cyc_Tcdecl_merge_binding(
void*b0,void*b1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);void*
Cyc_Tcdecl_merge_binding(void*b0,void*b1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct _tuple13 _tmp289;struct _tuple13 _tmp155=(_tmp289.f1=b0,((
_tmp289.f2=b1,_tmp289)));void*_tmp156;void*_tmp158;void*_tmp15A;struct Cyc_Absyn_Vardecl*
_tmp15C;void*_tmp15D;struct Cyc_Absyn_Vardecl*_tmp15F;void*_tmp160;struct Cyc_Absyn_Vardecl*
_tmp162;void*_tmp163;struct Cyc_Absyn_Fndecl*_tmp165;void*_tmp166;void*_tmp168;
struct Cyc_Absyn_Fndecl*_tmp16A;void*_tmp16B;struct Cyc_Absyn_Fndecl*_tmp16D;void*
_tmp16E;struct Cyc_Absyn_Vardecl*_tmp170;_LL34: _tmp156=_tmp155.f1;{struct Cyc_Absyn_Unresolved_b_struct*
_tmp157=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp156;if(_tmp157->tag != 0)goto
_LL36;};_tmp158=_tmp155.f2;{struct Cyc_Absyn_Unresolved_b_struct*_tmp159=(struct
Cyc_Absyn_Unresolved_b_struct*)_tmp158;if(_tmp159->tag != 0)goto _LL36;};_LL35:
return(void*)& Cyc_Absyn_Unresolved_b_val;_LL36: _tmp15A=_tmp155.f1;{struct Cyc_Absyn_Global_b_struct*
_tmp15B=(struct Cyc_Absyn_Global_b_struct*)_tmp15A;if(_tmp15B->tag != 1)goto _LL38;
else{_tmp15C=_tmp15B->f1;}};_tmp15D=_tmp155.f2;{struct Cyc_Absyn_Global_b_struct*
_tmp15E=(struct Cyc_Absyn_Global_b_struct*)_tmp15D;if(_tmp15E->tag != 1)goto _LL38;
else{_tmp15F=_tmp15E->f1;}};_LL37: {struct Cyc_Absyn_Vardecl*_tmp171=Cyc_Tcdecl_merge_vardecl(
_tmp15C,_tmp15F,loc,msg);if(_tmp171 == 0)return(void*)& Cyc_Absyn_Unresolved_b_val;
if(_tmp171 == (struct Cyc_Absyn_Vardecl*)_tmp15C)return b0;if(_tmp171 == (struct Cyc_Absyn_Vardecl*)
_tmp15F)return b1;{struct Cyc_Absyn_Global_b_struct _tmp28C;struct Cyc_Absyn_Global_b_struct*
_tmp28B;return(void*)((_tmp28B=_cycalloc(sizeof(*_tmp28B)),((_tmp28B[0]=((
_tmp28C.tag=1,((_tmp28C.f1=(struct Cyc_Absyn_Vardecl*)_tmp171,_tmp28C)))),_tmp28B))));};}
_LL38: _tmp160=_tmp155.f1;{struct Cyc_Absyn_Global_b_struct*_tmp161=(struct Cyc_Absyn_Global_b_struct*)
_tmp160;if(_tmp161->tag != 1)goto _LL3A;else{_tmp162=_tmp161->f1;}};_tmp163=
_tmp155.f2;{struct Cyc_Absyn_Funname_b_struct*_tmp164=(struct Cyc_Absyn_Funname_b_struct*)
_tmp163;if(_tmp164->tag != 2)goto _LL3A;else{_tmp165=_tmp164->f1;}};_LL39: {int
_tmp176;const char*_tmp28D;struct _tuple9 _tmp175=Cyc_Tcdecl_check_var_or_fn_decl(
_tmp162->sc,_tmp162->type,_tmp162->tq,_tmp162->attributes,_tmp165->sc,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp165->cached_typ))->v,Cyc_Absyn_empty_tqual(0),
_tmp165->attributes,((_tmp28D="function",_tag_dyneither(_tmp28D,sizeof(char),9))),
Cyc_Absynpp_qvar2string(_tmp162->name),loc,msg);_tmp176=_tmp175.f2;return _tmp176?
b1:(void*)& Cyc_Absyn_Unresolved_b_val;}_LL3A: _tmp166=_tmp155.f1;{struct Cyc_Absyn_Funname_b_struct*
_tmp167=(struct Cyc_Absyn_Funname_b_struct*)_tmp166;if(_tmp167->tag != 2)goto _LL3C;};
_tmp168=_tmp155.f2;{struct Cyc_Absyn_Funname_b_struct*_tmp169=(struct Cyc_Absyn_Funname_b_struct*)
_tmp168;if(_tmp169->tag != 2)goto _LL3C;else{_tmp16A=_tmp169->f1;}};_LL3B:{const
char*_tmp291;void*_tmp290[1];struct Cyc_String_pa_struct _tmp28F;(_tmp28F.tag=0,((
_tmp28F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp16A->name)),((_tmp290[0]=& _tmp28F,Cyc_Tcdecl_merr(loc,msg,((_tmp291="redefinition of function %s",
_tag_dyneither(_tmp291,sizeof(char),28))),_tag_dyneither(_tmp290,sizeof(void*),1)))))));}
return(void*)& Cyc_Absyn_Unresolved_b_val;_LL3C: _tmp16B=_tmp155.f1;{struct Cyc_Absyn_Funname_b_struct*
_tmp16C=(struct Cyc_Absyn_Funname_b_struct*)_tmp16B;if(_tmp16C->tag != 2)goto _LL3E;
else{_tmp16D=_tmp16C->f1;}};_tmp16E=_tmp155.f2;{struct Cyc_Absyn_Global_b_struct*
_tmp16F=(struct Cyc_Absyn_Global_b_struct*)_tmp16E;if(_tmp16F->tag != 1)goto _LL3E;
else{_tmp170=_tmp16F->f1;}};_LL3D: {int _tmp17C;const char*_tmp292;struct _tuple9
_tmp17B=Cyc_Tcdecl_check_var_or_fn_decl(_tmp16D->sc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp16D->cached_typ))->v,Cyc_Absyn_empty_tqual(0),_tmp16D->attributes,
_tmp170->sc,_tmp170->type,_tmp170->tq,_tmp170->attributes,((_tmp292="variable",
_tag_dyneither(_tmp292,sizeof(char),9))),Cyc_Absynpp_qvar2string(_tmp16D->name),
loc,msg);_tmp17C=_tmp17B.f2;return _tmp17C?b0:(void*)& Cyc_Absyn_Unresolved_b_val;}
_LL3E:;_LL3F: {struct Cyc_Core_Invalid_argument_struct _tmp298;const char*_tmp297;
struct Cyc_Core_Invalid_argument_struct*_tmp296;(int)_throw((void*)((_tmp296=
_cycalloc(sizeof(*_tmp296)),((_tmp296[0]=((_tmp298.tag=Cyc_Core_Invalid_argument,((
_tmp298.f1=((_tmp297="Tcdecl::merge_binding",_tag_dyneither(_tmp297,sizeof(char),
22))),_tmp298)))),_tmp296)))));}_LL33:;}struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(
struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Tcdecl_Xdatatypefielddecl*
Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,
struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl _tmp181;struct Cyc_Absyn_Datatypedecl*
_tmp182;struct Cyc_Absyn_Datatypefield*_tmp183;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp180=d0;_tmp181=*_tmp180;_tmp182=_tmp181.base;_tmp183=_tmp181.field;{struct
Cyc_Tcdecl_Xdatatypefielddecl _tmp185;struct Cyc_Absyn_Datatypedecl*_tmp186;struct
Cyc_Absyn_Datatypefield*_tmp187;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp184=d1;
_tmp185=*_tmp184;_tmp186=_tmp185.base;_tmp187=_tmp185.field;{struct
_dyneither_ptr _tmp188=Cyc_Absynpp_qvar2string(_tmp183->name);if(Cyc_Absyn_qvar_cmp(
_tmp182->name,_tmp186->name)!= 0){{const char*_tmp29E;void*_tmp29D[3];struct Cyc_String_pa_struct
_tmp29C;struct Cyc_String_pa_struct _tmp29B;struct Cyc_String_pa_struct _tmp29A;(
_tmp29A.tag=0,((_tmp29A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp188),((
_tmp29B.tag=0,((_tmp29B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp186->name)),((_tmp29C.tag=0,((_tmp29C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp182->name)),((_tmp29D[0]=& _tmp29C,((
_tmp29D[1]=& _tmp29B,((_tmp29D[2]=& _tmp29A,Cyc_Tcdecl_merr(loc,msg,((_tmp29E="@extensible datatype %s and %s have both a field named %s",
_tag_dyneither(_tmp29E,sizeof(char),58))),_tag_dyneither(_tmp29D,sizeof(void*),3)))))))))))))))))));}
return 0;}{const char*_tmp29F;if(!Cyc_Tcdecl_check_tvs(_tmp182->tvs,_tmp186->tvs,((
_tmp29F="@extensible datatype",_tag_dyneither(_tmp29F,sizeof(char),21))),Cyc_Absynpp_qvar2string(
_tmp182->name),loc,msg))return 0;}{struct Cyc_List_List*_tmp18F=Cyc_Tcdecl_build_tvs_map(
_tmp182->tvs,_tmp186->tvs);const char*_tmp2A0;struct Cyc_Absyn_Datatypefield*
_tmp190=Cyc_Tcdecl_merge_datatypefield(_tmp183,_tmp187,_tmp18F,((_tmp2A0="@extensible datatype field",
_tag_dyneither(_tmp2A0,sizeof(char),27))),_tmp188,msg);if(_tmp190 == 0)return 0;
if(_tmp190 == (struct Cyc_Absyn_Datatypefield*)_tmp183)return(struct Cyc_Tcdecl_Xdatatypefielddecl*)
d0;{struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp2A1;return(_tmp2A1=_cycalloc(sizeof(*
_tmp2A1)),((_tmp2A1->base=_tmp182,((_tmp2A1->field=(struct Cyc_Absyn_Datatypefield*)
_tmp190,_tmp2A1)))));};};};};}
