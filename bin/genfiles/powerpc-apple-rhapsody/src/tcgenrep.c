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
void*v;};void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};int isalnum(int);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*
env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*
x);void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*,struct Cyc_List_List*
x,void*accum);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*
x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_forall(
int(*pred)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_zip3(struct
Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_split(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int));
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct
Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int
tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_insert(struct
Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);void Cyc_Dict_iter(void(*f)(
void*,void*),struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,
void*);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strdup(struct
_dyneither_ptr src);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*Cyc_Hashtable_create(
int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct
Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*
t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_Float_struct{
int tag;};struct Cyc_Typerep_Double_struct{int tag;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple1{unsigned int f1;struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct
_tuple2{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dyneither_ptr f2;};struct _tuple3{unsigned int f1;struct
_dyneither_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;
struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple4{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);struct Cyc_timespec{long tv_sec;int tv_nsec;};struct Cyc_tm{int tm_sec;int
tm_min;int tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int tm_yday;int
tm_isdst;long tm_gmtoff;char*tm_zone;};struct _union_Nmspace_Rel_n{int tag;struct
Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};
struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple5{union Cyc_Absyn_Nmspace f1;struct
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple5*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple5*datatype_name;struct _tuple5*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple6{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple6 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct
Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct
Cyc_List_List*targs;};struct _tuple7{enum Cyc_Absyn_AggrKind f1;struct _tuple5*f2;
struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple7
val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};
union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct
_union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(
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
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple5*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple5*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
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
_union_Cnst_Null_c{int tag;int val;};struct _tuple8{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple8 val;};struct _tuple9{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple9 val;};struct _tuple10{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple10 val;}
;struct _tuple11{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple11 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);enum Cyc_Absyn_Primop{
Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod
 = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte
 = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand
 = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift
 = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};enum Cyc_Absyn_Incrementor{
Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec
 = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*
injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple5*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple5*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple12*f1;
struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple5*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple5*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple5*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple13{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple13 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple13 f2;struct _tuple13 f3;struct Cyc_Absyn_Stmt*f4;
};struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple13 f2;
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
int tag;struct _tuple5*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple5*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple5*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple5*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple5*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple5*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple5*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple5*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple5*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple5*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple5*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple5*,struct _tuple5*);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct
_tuple5*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple5*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*);
struct _tuple14{unsigned int f1;int f2;};struct _tuple14 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcgenrep_RepInfo{struct Cyc_List_List*
decls;struct Cyc_Absyn_Exp*exp;struct Cyc_List_List*dependencies;struct Cyc_Core_Opt*
fwd_decl;int emitted;int is_extern;};struct Cyc_Dict_Dict Cyc_Tcgenrep_empty_typerep_dict(){
return((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(Cyc_Tcutil_typecmp);}
void Cyc_Tcgenrep_print_dict_entry(void*type,struct Cyc_Tcgenrep_RepInfo*info){({
struct Cyc_Int_pa_struct _tmp4;_tmp4.tag=1;_tmp4.f1=(unsigned long)info->emitted;({
struct Cyc_Int_pa_struct _tmp3;_tmp3.tag=1;_tmp3.f1=(unsigned int)info;({struct Cyc_String_pa_struct
_tmp2;_tmp2.tag=0;_tmp2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
type));({void*_tmp0[3]={& _tmp2,& _tmp3,& _tmp4};Cyc_printf(({const char*_tmp1="(%s,%x:%d,";
_tag_dyneither(_tmp1,sizeof(char),11);}),_tag_dyneither(_tmp0,sizeof(void*),3));});});});});{
struct Cyc_List_List*p=info->dependencies;for(0;p != 0;p=p->tl){({struct Cyc_Int_pa_struct
_tmp7;_tmp7.tag=1;_tmp7.f1=(unsigned int)((struct Cyc_Tcgenrep_RepInfo*)p->hd);({
void*_tmp5[1]={& _tmp7};Cyc_printf(({const char*_tmp6="%x";_tag_dyneither(_tmp6,
sizeof(char),3);}),_tag_dyneither(_tmp5,sizeof(void*),1));});});if(p->tl != 0)({
void*_tmp8=0;Cyc_printf(({const char*_tmp9=",";_tag_dyneither(_tmp9,sizeof(char),
2);}),_tag_dyneither(_tmp8,sizeof(void*),0));});}}({void*_tmpA=0;Cyc_printf(({
const char*_tmpB=")\n";_tag_dyneither(_tmpB,sizeof(char),3);}),_tag_dyneither(
_tmpA,sizeof(void*),0));});}void Cyc_Tcgenrep_print_typerep_dict(struct Cyc_Dict_Dict
dict){((void(*)(void(*f)(void*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Dict_Dict d))
Cyc_Dict_iter)(Cyc_Tcgenrep_print_dict_entry,dict);}static int Cyc_Tcgenrep_rephash(
struct Cyc_Tcgenrep_RepInfo*ri){return(int)ri;}static int Cyc_Tcgenrep_repcmp(
struct Cyc_Tcgenrep_RepInfo*r1,struct Cyc_Tcgenrep_RepInfo*r2){unsigned int r1p;
unsigned int r2p;r1p=(unsigned int)r1;r2p=(unsigned int)r2;if(r1 < r2)return - 1;
else{if(r1 == r2)return 0;else{return 1;}}}static struct Cyc_Absyn_Datatypefield*Cyc_Tcgenrep_getField(
struct Cyc_Absyn_Datatypedecl*td,struct _tuple5*fieldname){if(td->fields == 0)({
void*_tmpC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpD="Could not find field in datatypedecl";_tag_dyneither(_tmpD,
sizeof(char),37);}),_tag_dyneither(_tmpC,sizeof(void*),0));});else{struct Cyc_List_List*
l=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;
l != 0;l=l->tl){if(!Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)l->hd)->name,
fieldname))return(struct Cyc_Absyn_Datatypefield*)l->hd;}}({void*_tmpE=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpF="Could not find field in datatypedecl";_tag_dyneither(_tmpF,sizeof(char),
37);}),_tag_dyneither(_tmpE,sizeof(void*),0));});}static char _tmp10[8]="Typerep";
static struct _dyneither_ptr Cyc_Tcgenrep_typerep_nm={_tmp10,_tmp10,_tmp10 + 8};
static struct Cyc_List_List Cyc_Tcgenrep_l2={(void*)& Cyc_Tcgenrep_typerep_nm,0};
static struct Cyc_List_List*Cyc_Tcgenrep_dfsvisit(struct Cyc_Tcgenrep_RepInfo*ri,
struct Cyc_Hashtable_Table*visited){struct _handler_cons _tmp11;_push_handler(&
_tmp11);{int _tmp13=0;if(setjmp(_tmp11.handler))_tmp13=1;if(!_tmp13){((int(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key))Cyc_Hashtable_lookup)(
visited,ri);{struct Cyc_List_List*_tmp14=0;_npop_handler(0);return _tmp14;};;
_pop_handler();}else{void*_tmp12=(void*)_exn_thrown;void*_tmp16=_tmp12;_LL1: {
struct Cyc_Core_Not_found_struct*_tmp17=(struct Cyc_Core_Not_found_struct*)_tmp16;
if(_tmp17->tag != Cyc_Core_Not_found)goto _LL3;}_LL2:((void(*)(struct Cyc_Hashtable_Table*
t,struct Cyc_Tcgenrep_RepInfo*key,int val))Cyc_Hashtable_insert)(visited,ri,1);{
struct Cyc_List_List*ds=0;{struct Cyc_List_List*l=ri->dependencies;for(0;l != 0;l=l->tl){
ds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
ds,Cyc_Tcgenrep_dfsvisit((struct Cyc_Tcgenrep_RepInfo*)l->hd,visited));}}if(ri->emitted
== 0){if(ri->fwd_decl != 0)ds=({struct Cyc_List_List*_tmp18=_cycalloc(sizeof(*
_tmp18));_tmp18->hd=({struct Cyc_Absyn_Decl*_tmp19[1];_tmp19[0]=(struct Cyc_Absyn_Decl*)((
struct Cyc_Core_Opt*)_check_null(ri->fwd_decl))->v;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp19,sizeof(struct Cyc_Absyn_Decl*),
1));});_tmp18->tl=ds;_tmp18;});{struct Cyc_List_List*_tmp1A=({struct Cyc_List_List*
_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->hd=ri->decls;_tmp1B->tl=0;_tmp1B;});ri->emitted=
1;return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(ds,_tmp1A);};}else{return ds;}};_LL3:;_LL4:(void)_throw(
_tmp16);_LL0:;}};}static struct Cyc_List_List*Cyc_Tcgenrep_dfs(struct Cyc_Tcgenrep_RepInfo*
ri){struct Cyc_Hashtable_Table*tab=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(
struct Cyc_Tcgenrep_RepInfo*,struct Cyc_Tcgenrep_RepInfo*),int(*hash)(struct Cyc_Tcgenrep_RepInfo*)))
Cyc_Hashtable_create)(53,Cyc_Tcgenrep_repcmp,Cyc_Tcgenrep_rephash);struct Cyc_List_List*
ds=Cyc_Tcgenrep_dfsvisit(ri,tab);return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_flatten)(ds);}static char _tmp1C[11]="Typestruct";static struct
_dyneither_ptr Cyc_Tcgenrep_typestruct_str={_tmp1C,_tmp1C,_tmp1C + 11};static char
_tmp1D[4]="Var";static struct _dyneither_ptr Cyc_Tcgenrep_var_str={_tmp1D,_tmp1D,
_tmp1D + 4};static char _tmp1E[4]="Int";static struct _dyneither_ptr Cyc_Tcgenrep_int_str={
_tmp1E,_tmp1E,_tmp1E + 4};static char _tmp1F[6]="Float";static struct _dyneither_ptr
Cyc_Tcgenrep_float_str={_tmp1F,_tmp1F,_tmp1F + 6};static char _tmp20[7]="Double";
static struct _dyneither_ptr Cyc_Tcgenrep_double_str={_tmp20,_tmp20,_tmp20 + 7};
static char _tmp21[8]="ThinPtr";static struct _dyneither_ptr Cyc_Tcgenrep_thinptr_str={
_tmp21,_tmp21,_tmp21 + 8};static char _tmp22[7]="FatPtr";static struct _dyneither_ptr
Cyc_Tcgenrep_fatptr_str={_tmp22,_tmp22,_tmp22 + 7};static char _tmp23[6]="Tuple";
static struct _dyneither_ptr Cyc_Tcgenrep_tuple_str={_tmp23,_tmp23,_tmp23 + 6};
static char _tmp24[12]="TUnionField";static struct _dyneither_ptr Cyc_Tcgenrep_datatypefield_str={
_tmp24,_tmp24,_tmp24 + 12};static char _tmp25[7]="Struct";static struct
_dyneither_ptr Cyc_Tcgenrep_struct_str={_tmp25,_tmp25,_tmp25 + 7};static char _tmp26[
7]="TUnion";static struct _dyneither_ptr Cyc_Tcgenrep_datatype_str={_tmp26,_tmp26,
_tmp26 + 7};static char _tmp27[8]="XTUnion";static struct _dyneither_ptr Cyc_Tcgenrep_xdatatype_str={
_tmp27,_tmp27,_tmp27 + 8};static char _tmp28[6]="Union";static struct _dyneither_ptr
Cyc_Tcgenrep_union_str={_tmp28,_tmp28,_tmp28 + 6};static char _tmp29[5]="Enum";
static struct _dyneither_ptr Cyc_Tcgenrep_enum_str={_tmp29,_tmp29,_tmp29 + 5};static
char _tmp2A[7]="name_t";static struct _dyneither_ptr Cyc_Tcgenrep_name_t_str={_tmp2A,
_tmp2A,_tmp2A + 7};static struct _tuple5*Cyc_Tcgenrep_typerep_name(struct
_dyneither_ptr*name){return({struct _tuple5*_tmp2B=_cycalloc(sizeof(*_tmp2B));
_tmp2B->f1=Cyc_Absyn_Abs_n((struct Cyc_List_List*)& Cyc_Tcgenrep_l2);_tmp2B->f2=
name;_tmp2B;});}static int Cyc_Tcgenrep_gen_id_counter=0;static struct
_dyneither_ptr*Cyc_Tcgenrep_new_gen_id(struct _dyneither_ptr name){return({struct
_dyneither_ptr*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C[0]=(struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp30;_tmp30.tag=1;_tmp30.f1=(unsigned long)Cyc_Tcgenrep_gen_id_counter
++;({struct Cyc_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)name);({void*_tmp2D[2]={& _tmp2F,& _tmp30};
Cyc_aprintf(({const char*_tmp2E="_gen%s_%d";_tag_dyneither(_tmp2E,sizeof(char),10);}),
_tag_dyneither(_tmp2D,sizeof(void*),2));});});});_tmp2C;});}static void Cyc_Tcgenrep_print_params(
struct Cyc_List_List*l){({void*_tmp31=0;Cyc_printf(({const char*_tmp32="<";
_tag_dyneither(_tmp32,sizeof(char),2);}),_tag_dyneither(_tmp31,sizeof(void*),0));});{
struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){({struct Cyc_Int_pa_struct _tmp36;
_tmp36.tag=1;_tmp36.f1=(unsigned long)((int)(p->tl != 0?',':' '));({struct Cyc_String_pa_struct
_tmp35;_tmp35.tag=0;_tmp35.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)p->hd));({void*_tmp33[2]={& _tmp35,& _tmp36};Cyc_printf(({const char*_tmp34="%s%c";
_tag_dyneither(_tmp34,sizeof(char),5);}),_tag_dyneither(_tmp33,sizeof(void*),2));});});});}}({
void*_tmp37=0;Cyc_printf(({const char*_tmp38=">\n";_tag_dyneither(_tmp38,sizeof(
char),3);}),_tag_dyneither(_tmp37,sizeof(void*),0));});}static void Cyc_Tcgenrep_print_tvars(
struct Cyc_List_List*l){({void*_tmp39=0;Cyc_printf(({const char*_tmp3A="<";
_tag_dyneither(_tmp3A,sizeof(char),2);}),_tag_dyneither(_tmp39,sizeof(void*),0));});{
struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){({struct Cyc_Int_pa_struct _tmp3E;
_tmp3E.tag=1;_tmp3E.f1=(unsigned long)((int)(p->tl != 0?',':' '));({struct Cyc_String_pa_struct
_tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Tvar*)p->hd)->name);({void*_tmp3B[2]={& _tmp3D,& _tmp3E};Cyc_printf(({
const char*_tmp3C="%s%c";_tag_dyneither(_tmp3C,sizeof(char),5);}),_tag_dyneither(
_tmp3B,sizeof(void*),2));});});});}}({void*_tmp3F=0;Cyc_printf(({const char*
_tmp40=">\n";_tag_dyneither(_tmp40,sizeof(char),3);}),_tag_dyneither(_tmp3F,
sizeof(void*),0));});}static struct _tuple5*Cyc_Tcgenrep_toplevel_name(struct
_dyneither_ptr*name){return({struct _tuple5*_tmp41=_cycalloc(sizeof(*_tmp41));
_tmp41->f1=Cyc_Absyn_Rel_n(0);_tmp41->f2=(struct _dyneither_ptr*)name;_tmp41;});}
static struct _tuple5*Cyc_Tcgenrep_relative_name(struct Cyc_List_List*pathl,struct
_dyneither_ptr*name){return({struct _tuple5*_tmp42=_cycalloc(sizeof(*_tmp42));
_tmp42->f1=Cyc_Absyn_Rel_n(pathl);_tmp42->f2=(struct _dyneither_ptr*)name;_tmp42;});}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl(struct _dyneither_ptr*name,void*
type,struct Cyc_Absyn_Exp*init,struct Cyc_Position_Segment*seg){struct _tuple5*qvar=
Cyc_Tcgenrep_toplevel_name(name);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(
qvar,type,init);vd->sc=Cyc_Absyn_Static;{struct Cyc_Absyn_Var_d_struct*r1=({
struct Cyc_Absyn_Var_d_struct*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=({
struct Cyc_Absyn_Var_d_struct _tmp44;_tmp44.tag=0;_tmp44.f1=vd;_tmp44;});_tmp43;});
void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,seg);};}static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_gen_vardecl(struct _tuple5*name,void*type,struct Cyc_Absyn_Exp*init,
enum Cyc_Absyn_Scope sc,struct Cyc_Position_Segment*seg){struct _tuple5*topname=Cyc_Tcgenrep_toplevel_name((*
name).f2);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(topname,type,init);vd->sc=
sc;{struct Cyc_Absyn_Var_d_struct*r1=({struct Cyc_Absyn_Var_d_struct*_tmp45=
_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Absyn_Var_d_struct _tmp46;_tmp46.tag=
0;_tmp46.f1=vd;_tmp46;});_tmp45;});void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,
seg);};}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string(struct _dyneither_ptr s,
struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(s),
seg);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string_cls(struct Cyc_Position_Segment*
seg,struct _dyneither_ptr*s){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(*s),seg);}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_qvar_string_cls(struct Cyc_Position_Segment*
seg,struct _tuple5*s){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(*(*s).f2),seg);}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int(int i,struct Cyc_Position_Segment*
seg){return Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i),seg);}static
struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int_cls(struct Cyc_Position_Segment*seg,int
i){return Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i),seg);}static int
Cyc_Tcgenrep_size_of2int(enum Cyc_Absyn_Size_of sz){switch(sz){case Cyc_Absyn_Char_sz:
_LL5: return 8;case Cyc_Absyn_Short_sz: _LL6: return 16;case Cyc_Absyn_Int_sz: _LL7:
return 32;case Cyc_Absyn_Long_sz: _LL8: return 32;case Cyc_Absyn_LongLong_sz: _LL9:
return 64;}}static void*Cyc_Tcgenrep_datatype_typ(struct _tuple5*name){return(void*)({
struct Cyc_Absyn_DatatypeType_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47[0]=({
struct Cyc_Absyn_DatatypeType_struct _tmp48;_tmp48.tag=3;_tmp48.f1=({struct Cyc_Absyn_DatatypeInfo
_tmp49;_tmp49.datatype_info=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo
_tmp4A;_tmp4A.name=name;_tmp4A.is_extensible=0;_tmp4A;}));_tmp49.targs=0;_tmp49;});
_tmp48;});_tmp47;});}static void*Cyc_Tcgenrep_datatypefield_typ(struct _tuple5*
name,struct _tuple5*fieldname){return(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B[0]=({struct Cyc_Absyn_DatatypeFieldType_struct
_tmp4C;_tmp4C.tag=4;_tmp4C.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp4D;_tmp4D.field_info=
Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp4E;
_tmp4E.datatype_name=name;_tmp4E.field_name=fieldname;_tmp4E.is_extensible=0;
_tmp4E;}));_tmp4D.targs=0;_tmp4D;});_tmp4C;});_tmp4B;});}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_call_exp(struct _tuple5*name,struct Cyc_List_List*args,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_unknowncall_exp(Cyc_Absyn_unknownid_exp(name,loc),args,loc);}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_datatype_constructor_decl(struct _tuple5*
datatypename,struct _tuple5*fieldname,struct _tuple5*varname,struct Cyc_List_List*
args,enum Cyc_Absyn_Scope sc,struct Cyc_Position_Segment*loc){void*_tmp4F=Cyc_Tcgenrep_datatypefield_typ(
datatypename,fieldname);struct Cyc_Absyn_Exp*_tmp50=Cyc_Tcgenrep_call_exp(
fieldname,args,loc);struct Cyc_Absyn_Decl*_tmp51=Cyc_Tcgenrep_gen_vardecl(varname,
_tmp4F,(struct Cyc_Absyn_Exp*)_tmp50,sc,loc);return _tmp51;}struct _tuple15{struct
Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_Tcgenrep_get_second(struct _tuple15*
pair){return(*pair).f2;}struct _tuple16{struct _dyneither_ptr*f1;struct Cyc_Absyn_Exp*
f2;};static struct _dyneither_ptr*Cyc_Tcgenrep_get_first(struct _tuple16*pair){
return(*pair).f1;}static char _tmp52[5]="list";static struct _dyneither_ptr Cyc_Tcgenrep_list_str={
_tmp52,_tmp52,_tmp52 + 5};static char _tmp53[5]="List";static struct _dyneither_ptr
Cyc_Tcgenrep_List_str={_tmp53,_tmp53,_tmp53 + 5};struct _tuple17{struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple2_exp_cls(
struct Cyc_Position_Segment*loc,struct _tuple17*es){struct _tuple17 _tmp55;struct Cyc_Absyn_Exp*
_tmp56;struct Cyc_Absyn_Exp*_tmp57;struct _tuple17*_tmp54=es;_tmp55=*_tmp54;_tmp56=
_tmp55.f1;_tmp57=_tmp55.f2;return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*
_tmp58[2];_tmp58[1]=_tmp57;_tmp58[0]=_tmp56;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp58,sizeof(struct Cyc_Absyn_Exp*),
2));}),loc);}struct _tuple18{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct
Cyc_Absyn_Exp*f3;};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple3_exp_cls(struct
Cyc_Position_Segment*loc,struct _tuple18*es){struct _tuple18 _tmp5A;struct Cyc_Absyn_Exp*
_tmp5B;struct Cyc_Absyn_Exp*_tmp5C;struct Cyc_Absyn_Exp*_tmp5D;struct _tuple18*
_tmp59=es;_tmp5A=*_tmp59;_tmp5B=_tmp5A.f1;_tmp5C=_tmp5A.f2;_tmp5D=_tmp5A.f3;
return Cyc_Absyn_tuple_exp(({struct Cyc_Absyn_Exp*_tmp5E[3];_tmp5E[2]=_tmp5D;
_tmp5E[1]=_tmp5C;_tmp5E[0]=_tmp5B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp5E,sizeof(struct Cyc_Absyn_Exp*),3));}),loc);}
static char _tmp5F[5]="NULL";static struct _dyneither_ptr Cyc_Tcgenrep_null_str={
_tmp5F,_tmp5F,_tmp5F + 5};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(struct
Cyc_List_List*l,struct Cyc_Position_Segment*loc){if(l == 0)return Cyc_Absyn_null_exp(
loc);return Cyc_Tcgenrep_call_exp(Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_list_str),
l,loc);}struct _tuple19{void*f1;struct Cyc_Position_Segment*f2;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_make_offsetof_exp(struct _tuple19*typeloc,int index){struct _tuple19
_tmp61;void*_tmp62;struct Cyc_Position_Segment*_tmp63;struct _tuple19*_tmp60=
typeloc;_tmp61=*_tmp60;_tmp62=_tmp61.f1;_tmp63=_tmp61.f2;return Cyc_Absyn_offsetof_exp(
_tmp62,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp64=_cycalloc_atomic(
sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp65;_tmp65.tag=
1;_tmp65.f1=(unsigned int)index;_tmp65;});_tmp64;}),_tmp63);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_get_and_cast_ri_exp(struct Cyc_Position_Segment*loc,struct Cyc_Tcgenrep_RepInfo*
info){return Cyc_Absyn_cast_exp(Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str)),(struct Cyc_Absyn_Exp*)_check_null(info->exp),1,Cyc_Absyn_Unknown_coercion,
loc);}static struct _tuple16*Cyc_Tcgenrep_gen_id_for_exp(struct Cyc_Absyn_Exp*e){
return({struct _tuple16*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=Cyc_Tcgenrep_new_gen_id(({
const char*_tmp67="tuple";_tag_dyneither(_tmp67,sizeof(char),6);}));_tmp66->f2=e;
_tmp66;});}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl_cls(struct _tuple19*
env,struct _tuple16*name_exp){struct _tuple19 _tmp69;void*_tmp6A;struct Cyc_Position_Segment*
_tmp6B;struct _tuple19*_tmp68=env;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{
struct _tuple16 _tmp6D;struct _dyneither_ptr*_tmp6E;struct Cyc_Absyn_Exp*_tmp6F;
struct _tuple16*_tmp6C=name_exp;_tmp6D=*_tmp6C;_tmp6E=_tmp6D.f1;_tmp6F=_tmp6D.f2;
return Cyc_Tcgenrep_gen_decl(_tmp6E,_tmp6A,(struct Cyc_Absyn_Exp*)_tmp6F,_tmp6B);};}
struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct
_tuple20*Cyc_Tcgenrep_null_designator_exp(struct Cyc_Absyn_Exp*e){return({struct
_tuple20*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->f1=0;_tmp70->f2=e;_tmp70;});}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_arr_init_exp(struct Cyc_List_List*l,struct
Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp71=((struct Cyc_List_List*(*)(
struct _tuple20*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_null_designator_exp,l);return Cyc_Absyn_unresolvedmem_exp(0,_tmp71,
loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_address_exp(e,loc);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_unknownid_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple5*e){
return Cyc_Absyn_unknownid_exp(e,loc);}static int Cyc_Tcgenrep_has_bitfield(struct
Cyc_Absyn_Aggrfield*sf){return sf->width != 0;}static int Cyc_Tcgenrep_add_bitfield_sizes(
int total,struct Cyc_Absyn_Aggrfield*sf){unsigned int _tmp73;int _tmp74;struct
_tuple14 _tmp72=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
sf->width));_tmp73=_tmp72.f1;_tmp74=_tmp72.f2;if(!_tmp74)({void*_tmp75=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp76="add_bitfield_sizes: sizeof or offsetof in bitfield size";_tag_dyneither(
_tmp76,sizeof(char),56);}),_tag_dyneither(_tmp75,sizeof(void*),0));});return(int)(
_tmp73 + total);}static void*Cyc_Tcgenrep_select_structfield_type(struct Cyc_Absyn_Aggrfield*
sf){if(Cyc_strcmp((struct _dyneither_ptr)*sf->name,({const char*_tmp77="";
_tag_dyneither(_tmp77,sizeof(char),1);}))== 0)({void*_tmp78=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp79="gen(): anonymous (padding) structfield not handled yet";
_tag_dyneither(_tmp79,sizeof(char),55);}),_tag_dyneither(_tmp78,sizeof(void*),0));});
if(Cyc_Tcgenrep_has_bitfield(sf))({void*_tmp7A=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7B="gen(): mixed struct bitfields and union bitfields not handled";
_tag_dyneither(_tmp7B,sizeof(char),62);}),_tag_dyneither(_tmp7A,sizeof(void*),0));});
return sf->type;}struct _tuple21{struct _dyneither_ptr*f1;void*f2;};static struct
_tuple21*Cyc_Tcgenrep_select_structfield_nmtype(struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*sf->name,({const char*_tmp7C="";
_tag_dyneither(_tmp7C,sizeof(char),1);}))== 0)({void*_tmp7D=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7E="gen(): anonymous (padding) structfield not handled yet";
_tag_dyneither(_tmp7E,sizeof(char),55);}),_tag_dyneither(_tmp7D,sizeof(void*),0));});
if(Cyc_Tcgenrep_has_bitfield(sf))({void*_tmp7F=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp80="gen(): mixed struct bitfields and union bitfields not handled";
_tag_dyneither(_tmp80,sizeof(char),62);}),_tag_dyneither(_tmp7F,sizeof(void*),0));});
return({struct _tuple21*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->f1=sf->name;
_tmp81->f2=sf->type;_tmp81;});}struct _tuple22{int f1;struct _tuple5*f2;};struct
_tuple22*Cyc_Tcgenrep_select_enumfield_tagnm(struct Cyc_Absyn_Enumfield*ef){if(ef->tag
== 0)({void*_tmp82=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp83="Enum tag exp should be filled in by now";
_tag_dyneither(_tmp83,sizeof(char),40);}),_tag_dyneither(_tmp82,sizeof(void*),0));});
return({struct _tuple22*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->f1=(int)(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag))).f1;_tmp84->f2=ef->name;_tmp84;});}
static struct Cyc_Dict_Dict Cyc_Tcgenrep_update_info(struct Cyc_Dict_Dict dict,void*
type,struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*exp,struct Cyc_List_List*
dependencies,struct Cyc_Core_Opt*fwd_decl){struct Cyc_Tcgenrep_RepInfo**_tmp85=((
struct Cyc_Tcgenrep_RepInfo**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(
dict,type);if(_tmp85 != 0){if((*_tmp85)->decls != 0){Cyc_Tcgenrep_print_typerep_dict(
dict);({void*_tmp86=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp87="Updating non-forward declaration";
_tag_dyneither(_tmp87,sizeof(char),33);}),_tag_dyneither(_tmp86,sizeof(void*),0));});}(*
_tmp85)->decls=decls;(*_tmp85)->exp=exp;(*_tmp85)->dependencies=dependencies;
return dict;}else{return((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,type,({struct Cyc_Tcgenrep_RepInfo*
_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->decls=decls;_tmp88->exp=exp;_tmp88->dependencies=
dependencies;_tmp88->fwd_decl=fwd_decl;_tmp88->emitted=0;_tmp88->is_extern=0;
_tmp88;}));}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict
dict,void*type,struct Cyc_Absyn_Exp*exp,struct Cyc_Core_Opt*fwd_decl,int is_extern){
struct Cyc_Tcgenrep_RepInfo**_tmp89=((struct Cyc_Tcgenrep_RepInfo**(*)(struct Cyc_Dict_Dict
d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(_tmp89 != 0){({struct Cyc_String_pa_struct
_tmp8C;_tmp8C.tag=0;_tmp8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
type));({void*_tmp8A[1]={& _tmp8C};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp8B="Repinfo for fwd declaration of %s already exists";
_tag_dyneither(_tmp8B,sizeof(char),49);}),_tag_dyneither(_tmp8A,sizeof(void*),1));});});
return dict;}else{return((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,
struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(dict,type,({struct Cyc_Tcgenrep_RepInfo*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->decls=0;_tmp8D->exp=exp;_tmp8D->dependencies=
0;_tmp8D->fwd_decl=fwd_decl;_tmp8D->emitted=0;_tmp8D->is_extern=is_extern;_tmp8D;}));}}
static struct Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={0,0,0,0,0};static struct _tuple15*
Cyc_Tcgenrep_tqual_type(struct Cyc_Absyn_Tqual*tq,void*type){return({struct
_tuple15*_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E->f1=*((struct Cyc_Absyn_Tqual*)
_check_null(tq));_tmp8E->f2=type;_tmp8E;});}static void*Cyc_Tcgenrep_tuple_typ(
struct Cyc_List_List*types){struct Cyc_List_List*_tmp8F=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Tqual*,void*),struct Cyc_Absyn_Tqual*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tqual_type,(struct Cyc_Absyn_Tqual*)&
Cyc_Tcgenrep_tq_none,types);struct Cyc_Absyn_TupleType_struct*tuple_type_base=({
struct Cyc_Absyn_TupleType_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({
struct Cyc_Absyn_TupleType_struct _tmp91;_tmp91.tag=11;_tmp91.f1=_tmp8F;_tmp91;});
_tmp90;});void*tuple_type=(void*)tuple_type_base;return tuple_type;}static struct
_tuple20*Cyc_Tcgenrep_array_decls(void*type,struct Cyc_List_List*exps,struct Cyc_Position_Segment*
loc){struct Cyc_List_List*_tmp92=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_gen_id_for_exp,
exps);struct Cyc_List_List*_tmp93=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*
f)(struct _tuple19*,struct _tuple16*),struct _tuple19*env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Tcgenrep_gen_decl_cls,({struct _tuple19*_tmpA1=_cycalloc(
sizeof(*_tmpA1));_tmpA1->f1=type;_tmpA1->f2=loc;_tmpA1;}),_tmp92);struct Cyc_List_List*
_tmp94=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple16*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_first,_tmp92);struct Cyc_List_List*
_tmp95=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct _dyneither_ptr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_toplevel_name,_tmp94);struct
Cyc_List_List*_tmp96=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct
Cyc_Position_Segment*,struct _tuple5*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_unknownid_exp_cls,loc,_tmp95);struct Cyc_List_List*
_tmp97=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Exp*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_address_exp_cls,loc,_tmp96);struct Cyc_Absyn_Exp*_tmp98=Cyc_Tcgenrep_arr_init_exp(
_tmp97,loc);struct _dyneither_ptr*_tmp99=Cyc_Tcgenrep_new_gen_id(({const char*
_tmpA0="arr";_tag_dyneither(_tmpA0,sizeof(char),4);}));void*_tmp9A=Cyc_Absyn_at_typ(
type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Tcgenrep_tq_none,Cyc_Absyn_false_conref);
void*_tmp9B=Cyc_Absyn_array_typ(_tmp9A,Cyc_Tcgenrep_tq_none,0,Cyc_Absyn_false_conref,
0);struct Cyc_Absyn_Decl*_tmp9C=Cyc_Tcgenrep_gen_decl(_tmp99,_tmp9B,(struct Cyc_Absyn_Exp*)
_tmp98,loc);struct Cyc_Absyn_Exp*_tmp9D=Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp99),loc);return({struct _tuple20*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp93,({struct Cyc_Absyn_Decl*_tmp9F[1];_tmp9F[0]=_tmp9C;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp9F,sizeof(struct Cyc_Absyn_Decl*),
1));}));_tmp9E->f2=_tmp9D;_tmp9E;});}static void*Cyc_Tcgenrep_check_datatypefield_and_get_type(
struct Cyc_Absyn_Datatypefield*tuf){return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_TupleType_struct
_tmpA3;_tmpA3.tag=11;_tmpA3.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple15*_tmpA4[1];_tmpA4[0]=({
struct _tuple15*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->f1=Cyc_Tcgenrep_tq_none;
_tmpA5->f2=Cyc_Absyn_uint_typ;_tmpA5;});((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA4,sizeof(struct _tuple15*),1));}),
tuf->typs);_tmpA3;});_tmpA2;});}static struct _tuple5*Cyc_Tcgenrep_check_datatypefield_and_get_name(
struct Cyc_Absyn_Datatypefield*tuf){return tuf->name;}struct _tuple23{struct _tuple5*
f1;void*f2;};static struct _tuple23*Cyc_Tcgenrep_check_datatypefield_and_get_nmtype(
struct Cyc_Absyn_Datatypefield*tuf){return({struct _tuple23*_tmpA6=_cycalloc(
sizeof(*_tmpA6));_tmpA6->f1=tuf->name;_tmpA6->f2=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_TupleType_struct
_tmpA8;_tmpA8.tag=11;_tmpA8.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple15*_tmpA9[1];_tmpA9[0]=({
struct _tuple15*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->f1=Cyc_Tcgenrep_tq_none;
_tmpAA->f2=Cyc_Absyn_uint_typ;_tmpAA;});((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpA9,sizeof(struct _tuple15*),1));}),
tuf->typs);_tmpA8;});_tmpA7;});_tmpA6;});}static struct _tuple21*Cyc_Tcgenrep_check_xdatatypefield_and_get_name_type(
struct Cyc_Absyn_Datatypefield*tuf){return({struct _tuple21*_tmpAB=_cycalloc(
sizeof(*_tmpAB));_tmpAB->f1=(*tuf->name).f2;_tmpAB->f2=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC[0]=({struct Cyc_Absyn_TupleType_struct
_tmpAD;_tmpAD.tag=11;_tmpAD.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(({struct _tuple15*_tmpAE[1];_tmpAE[0]=({
struct _tuple15*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->f1=Cyc_Tcgenrep_tq_none;
_tmpAF->f2=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Tcgenrep_tq_none,Cyc_Absyn_true_conref);_tmpAF;});((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAE,sizeof(struct _tuple15*),
1));}),tuf->typs);_tmpAD;});_tmpAC;});_tmpAB;});}static int Cyc_Tcgenrep_filter_empty_datatypefield(
struct Cyc_Absyn_Datatypefield*tuf){return tuf->typs != 0;}static int Cyc_Tcgenrep_filter_nonempty_datatypefield(
struct Cyc_Absyn_Datatypefield*tuf){return tuf->typs == 0;}static struct
_dyneither_ptr*Cyc_Tcgenrep_get_datatypefield_name(struct Cyc_Absyn_Datatypefield*
tuf){return(*tuf->name).f2;}struct Cyc_Absyn_Aggrfield*Cyc_Tcgenrep_substitute_structfield_type(
struct Cyc_List_List*subst,struct Cyc_Absyn_Aggrfield*sf){return({struct Cyc_Absyn_Aggrfield*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->name=sf->name;_tmpB0->tq=sf->tq;_tmpB0->type=
Cyc_Tcutil_substitute(subst,sf->type);_tmpB0->width=sf->width;_tmpB0->attributes=
sf->attributes;_tmpB0;});}struct _tuple15*Cyc_Tcgenrep_substitute_tqual_type(
struct Cyc_List_List*subst,struct _tuple15*pair){struct _tuple15 _tmpB2;struct Cyc_Absyn_Tqual
_tmpB3;void*_tmpB4;struct _tuple15*_tmpB1=pair;_tmpB2=*_tmpB1;_tmpB3=_tmpB2.f1;
_tmpB4=_tmpB2.f2;return({struct _tuple15*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->f1=
_tmpB3;_tmpB5->f2=Cyc_Tcutil_substitute(subst,_tmpB4);_tmpB5;});}struct Cyc_Absyn_Datatypefield*
Cyc_Tcgenrep_substitute_datatypefield_type(struct Cyc_List_List*subst,struct Cyc_Absyn_Datatypefield*
tf){return({struct Cyc_Absyn_Datatypefield*_tmpB6=_cycalloc(sizeof(*_tmpB6));
_tmpB6->name=tf->name;_tmpB6->typs=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(
struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_tqual_type,subst,tf->typs);_tmpB6->loc=
tf->loc;_tmpB6->sc=tf->sc;_tmpB6;});}void*Cyc_Tcgenrep_monomorphize_type(void*
type){void*_tmpB7=Cyc_Tcutil_compress(type);struct Cyc_Absyn_AggrInfo _tmpB9;union
Cyc_Absyn_AggrInfoU _tmpBA;struct Cyc_List_List*_tmpBB;struct Cyc_Absyn_DatatypeInfo
_tmpBD;union Cyc_Absyn_DatatypeInfoU _tmpBE;struct Cyc_Absyn_Datatypedecl**_tmpBF;
struct Cyc_Absyn_Datatypedecl*_tmpC0;struct Cyc_List_List*_tmpC1;_LLC: {struct Cyc_Absyn_AggrType_struct*
_tmpB8=(struct Cyc_Absyn_AggrType_struct*)_tmpB7;if(_tmpB8->tag != 12)goto _LLE;
else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB9.aggr_info;_tmpBB=_tmpB9.targs;}}_LLD: {
struct Cyc_Absyn_Aggrdecl*_tmpC2=Cyc_Absyn_get_known_aggrdecl(_tmpBA);struct Cyc_List_List*
_tmpC3=_tmpC2->tvs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBB)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC3))({struct Cyc_Int_pa_struct
_tmpC7;_tmpC7.tag=1;_tmpC7.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpC3);({struct Cyc_Int_pa_struct _tmpC6;_tmpC6.tag=1;_tmpC6.f1=(unsigned long)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBB);({void*_tmpC4[2]={& _tmpC6,&
_tmpC7};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpC5="gen(): number of params %d differs from number of tyvars %d";
_tag_dyneither(_tmpC5,sizeof(char),60);}),_tag_dyneither(_tmpC4,sizeof(void*),2));});});});{
struct Cyc_List_List*_tmpC8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmpC3,_tmpBB);struct Cyc_List_List*fields=0;
if(_tmpC2->impl != 0)fields=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_structfield_type,
_tmpC8,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC2->impl))->fields);{
struct Cyc_Absyn_AggrdeclImpl*_tmpC9=_tmpC2->impl == 0?0:({struct Cyc_Absyn_AggrdeclImpl*
_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->exist_vars=0;_tmpCF->rgn_po=0;_tmpCF->fields=
fields;_tmpCF->tagged=0;_tmpCF;});struct Cyc_Absyn_Aggrdecl*ad2=({struct Cyc_Absyn_Aggrdecl*
_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->kind=_tmpC2->kind;_tmpCE->sc=_tmpC2->sc;
_tmpCE->name=_tmpC2->name;_tmpCE->tvs=0;_tmpCE->impl=_tmpC9;_tmpCE->attributes=
_tmpC2->attributes;_tmpCE;});return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_AggrType_struct
_tmpCB;_tmpCB.tag=12;_tmpCB.f1=({struct Cyc_Absyn_AggrInfo _tmpCC;_tmpCC.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpCD=_cycalloc(sizeof(*_tmpCD));
_tmpCD[0]=ad2;_tmpCD;}));_tmpCC.targs=0;_tmpCC;});_tmpCB;});_tmpCA;});};};}_LLE: {
struct Cyc_Absyn_DatatypeType_struct*_tmpBC=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpB7;if(_tmpBC->tag != 3)goto _LL10;else{_tmpBD=_tmpBC->f1;_tmpBE=_tmpBD.datatype_info;
if((_tmpBE.KnownDatatype).tag != 2)goto _LL10;_tmpBF=(struct Cyc_Absyn_Datatypedecl**)(
_tmpBE.KnownDatatype).val;_tmpC0=*_tmpBF;_tmpC1=_tmpBD.targs;}}_LLF: {struct Cyc_List_List*
_tmpD0=_tmpC0->tvs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC1)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD0))({struct Cyc_Int_pa_struct
_tmpD4;_tmpD4.tag=1;_tmpD4.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpD0);({struct Cyc_Int_pa_struct _tmpD3;_tmpD3.tag=1;_tmpD3.f1=(unsigned long)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC1);({void*_tmpD1[2]={& _tmpD3,&
_tmpD4};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpD2="gen(): number of params %d differs from number of tyvars %d";
_tag_dyneither(_tmpD2,sizeof(char),60);}),_tag_dyneither(_tmpD1,sizeof(void*),2));});});});{
struct Cyc_List_List*_tmpD5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmpD0,_tmpC1);struct Cyc_Core_Opt*fields=0;
if(_tmpC0->fields != 0)fields=({struct Cyc_Core_Opt*_tmpD6=_cycalloc(sizeof(*
_tmpD6));_tmpD6->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Datatypefield*(*f)(
struct Cyc_List_List*,struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_datatypefield_type,
_tmpD5,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpC0->fields))->v);
_tmpD6;});{struct Cyc_Absyn_Datatypedecl*_tmpD7=({struct Cyc_Absyn_Datatypedecl*
_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->sc=_tmpC0->sc;_tmpDE->name=_tmpC0->name;
_tmpDE->tvs=0;_tmpDE->fields=fields;_tmpDE->is_extensible=_tmpC0->is_extensible;
_tmpDE;});struct Cyc_Absyn_DatatypeType_struct*_tmpD8=({struct Cyc_Absyn_DatatypeType_struct*
_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_DatatypeType_struct
_tmpDB;_tmpDB.tag=3;_tmpDB.f1=({struct Cyc_Absyn_DatatypeInfo _tmpDC;_tmpDC.datatype_info=
Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmpDD=_cycalloc(sizeof(*
_tmpDD));_tmpDD[0]=_tmpD7;_tmpDD;}));_tmpDC.targs=_tmpC1;_tmpDC;});_tmpDB;});
_tmpDA;});struct Cyc_Position_Segment*_tmpD9=Cyc_Position_segment_of_abs(0,0);
return(void*)_tmpD8;};};}_LL10:;_LL11: return type;_LLB:;}struct _dyneither_ptr Cyc_Tcgenrep_make_type_cstring(
void*t){struct _dyneither_ptr s=Cyc_strdup((struct _dyneither_ptr)Cyc_Absynpp_typ2cstring(
t));{int i=0;for(0;i < Cyc_strlen((struct _dyneither_ptr)s);++ i){if(*((char*)
_check_dyneither_subscript(s,sizeof(char),i))== ' ')({struct _dyneither_ptr _tmpDF=
_dyneither_ptr_plus(s,sizeof(char),i);char _tmpE0=*((char*)
_check_dyneither_subscript(_tmpDF,sizeof(char),0));char _tmpE1='_';if(
_get_dyneither_size(_tmpDF,sizeof(char))== 1  && (_tmpE0 == '\000'  && _tmpE1 != '\000'))
_throw_arraybounds();*((char*)_tmpDF.curr)=_tmpE1;});else{if(!isalnum((int)*((
char*)_check_dyneither_subscript(s,sizeof(char),i))) && *((char*)
_check_dyneither_subscript(s,sizeof(char),i))!= '_')({struct _dyneither_ptr _tmpE2=
_dyneither_ptr_plus(s,sizeof(char),i);char _tmpE3=*((char*)
_check_dyneither_subscript(_tmpE2,sizeof(char),0));char _tmpE4=(char)('0' + *((
char*)_check_dyneither_subscript(s,sizeof(char),i))% 10);if(_get_dyneither_size(
_tmpE2,sizeof(char))== 1  && (_tmpE3 == '\000'  && _tmpE4 != '\000'))
_throw_arraybounds();*((char*)_tmpE2.curr)=_tmpE4;});}}}return(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)s,({const char*_tmpE5="_rep";
_tag_dyneither(_tmpE5,sizeof(char),5);}));}struct _tuple24{struct Cyc_Dict_Dict f1;
struct Cyc_Tcgenrep_RepInfo*f2;};static struct _tuple24*Cyc_Tcgenrep_lookupRep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,
void*type);struct _tuple25{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct
_tuple26{struct Cyc_Tcenv_Tenv*f1;struct Cyc_Position_Segment*f2;};static struct
_tuple25*Cyc_Tcgenrep_lookupRepsCls(struct _tuple26*env,void*type,struct _tuple25*
carry){struct _tuple24 _tmpE7;struct Cyc_Dict_Dict _tmpE8;struct Cyc_Tcgenrep_RepInfo*
_tmpE9;struct _tuple24*_tmpE6=Cyc_Tcgenrep_lookupRep((*env).f1,(*carry).f1,(*env).f2,
type);_tmpE7=*_tmpE6;_tmpE8=_tmpE7.f1;_tmpE9=_tmpE7.f2;return({struct _tuple25*
_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->f1=_tmpE8;_tmpEA->f2=({struct Cyc_List_List*
_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=_tmpE9;_tmpEB->tl=(*carry).f2;
_tmpEB;});_tmpEA;});}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepTuple(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,struct
_tuple5*varname,enum Cyc_Absyn_Scope sc,void*type,struct Cyc_List_List*types){
struct Cyc_Absyn_Exp*_tmpEC=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple26*
_tmpED=({struct _tuple26*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->f1=te;
_tmp102->f2=loc;_tmp102;});struct _tuple25*_tmpEE=({struct _tuple25*_tmp101=
_cycalloc(sizeof(*_tmp101));_tmp101->f1=dict;_tmp101->f2=0;_tmp101;});struct
_tuple25 _tmpF0;struct Cyc_Dict_Dict _tmpF1;struct Cyc_List_List*_tmpF2;struct
_tuple25*_tmpEF=((struct _tuple25*(*)(struct _tuple25*(*f)(struct _tuple26*,void*,
struct _tuple25*),struct _tuple26*,struct Cyc_List_List*x,struct _tuple25*accum))Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls,_tmpED,types,_tmpEE);_tmpF0=*_tmpEF;_tmpF1=_tmpF0.f1;
_tmpF2=_tmpF0.f2;dict=_tmpF1;{struct Cyc_List_List*_tmpF3=((struct Cyc_List_List*(*)(
int n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmpF4=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple19*,int),struct _tuple19*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({struct _tuple19*_tmp100=
_cycalloc(sizeof(*_tmp100));_tmp100->f1=type;_tmp100->f2=loc;_tmp100;}),_tmpF3);
struct Cyc_List_List*_tmpF5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmpF2);struct Cyc_List_List*_tmpF6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple17*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpF4,_tmpF5));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmpFF[2];_tmpFF[1]=
Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmpFF[0]=Cyc_Absyn_uint_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpFF,sizeof(void*),2));}));struct _tuple20 _tmpF8;struct Cyc_List_List*
_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;struct _tuple20*_tmpF7=Cyc_Tcgenrep_array_decls(
tuple_type,_tmpF6,loc);_tmpF8=*_tmpF7;_tmpF9=_tmpF8.f1;_tmpFA=_tmpF8.f2;{struct
Cyc_Absyn_Decl*_tmpFB=Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str),
varname,({struct Cyc_Absyn_Exp*_tmpFE[2];_tmpFE[1]=_tmpFA;_tmpFE[0]=_tmpEC;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpFE,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);struct Cyc_List_List*_tmpFC=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmpF9,({struct Cyc_Absyn_Decl*_tmpFD[1];_tmpFD[0]=_tmpFB;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFD,sizeof(struct Cyc_Absyn_Decl*),
1));}));return Cyc_Tcgenrep_update_info(dict,type,_tmpFC,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmpF2,0);};};}
static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepDatatypefield(struct _tuple5*tname,
struct _tuple5*fname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple5*varname,enum Cyc_Absyn_Scope sc,void*type,struct Cyc_List_List*
types){struct Cyc_Absyn_Exp*_tmp103=Cyc_Absyn_sizeoftyp_exp(type,loc);struct
_tuple26*_tmp104=({struct _tuple26*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->f1=
te;_tmp11B->f2=loc;_tmp11B;});struct _tuple25*_tmp105=({struct _tuple25*_tmp11A=
_cycalloc(sizeof(*_tmp11A));_tmp11A->f1=dict;_tmp11A->f2=0;_tmp11A;});struct
_tuple25 _tmp107;struct Cyc_Dict_Dict _tmp108;struct Cyc_List_List*_tmp109;struct
_tuple25*_tmp106=((struct _tuple25*(*)(struct _tuple25*(*f)(struct _tuple26*,void*,
struct _tuple25*),struct _tuple26*,struct Cyc_List_List*x,struct _tuple25*accum))Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls,_tmp104,types,_tmp105);_tmp107=*_tmp106;_tmp108=
_tmp107.f1;_tmp109=_tmp107.f2;dict=_tmp108;{struct Cyc_List_List*_tmp10A=((struct
Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(types),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*
_tmp10B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple19*,int),
struct _tuple19*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple19*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->f1=type;_tmp119->f2=
loc;_tmp119;}),_tmp10A);struct Cyc_List_List*_tmp10C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,
loc,_tmp109);struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple17*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp10B,_tmp10C));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({void*_tmp118[2];
_tmp118[1]=Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp118[0]=Cyc_Absyn_uint_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp118,sizeof(void*),2));}));struct Cyc_Absyn_Exp*
_tmp10E=Cyc_Tcgenrep_cnst_string(*(*tname).f2,loc);struct Cyc_Absyn_Exp*_tmp10F=
Cyc_Tcgenrep_cnst_string(*(*fname).f2,loc);struct _tuple20 _tmp111;struct Cyc_List_List*
_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct _tuple20*_tmp110=Cyc_Tcgenrep_array_decls(
tuple_type,_tmp10D,loc);_tmp111=*_tmp110;_tmp112=_tmp111.f1;_tmp113=_tmp111.f2;{
struct Cyc_Absyn_Decl*_tmp114=Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_datatypefield_str),
varname,({struct Cyc_Absyn_Exp*_tmp117[4];_tmp117[3]=_tmp113;_tmp117[2]=_tmp103;
_tmp117[1]=_tmp10F;_tmp117[0]=_tmp10E;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp117,sizeof(struct Cyc_Absyn_Exp*),
4));}),sc,loc);struct Cyc_List_List*_tmp115=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp112,({struct Cyc_Absyn_Decl*
_tmp116[1];_tmp116[0]=_tmp114;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp116,sizeof(struct Cyc_Absyn_Decl*),1));}));
return Cyc_Tcgenrep_update_info(dict,type,_tmp115,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp109,0);};};}static struct Cyc_Dict_Dict
Cyc_Tcgenrep_buildRepStruct(struct Cyc_Core_Opt*sname,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,struct _tuple5*varname,
enum Cyc_Absyn_Scope sc,void*type,struct Cyc_List_List*nmtypes){struct Cyc_Absyn_Exp*
_tmp11C=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple26*_tmp11D=({struct
_tuple26*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->f1=te;_tmp13A->f2=loc;
_tmp13A;});struct _tuple25*_tmp11E=({struct _tuple25*_tmp139=_cycalloc(sizeof(*
_tmp139));_tmp139->f1=dict;_tmp139->f2=0;_tmp139;});struct Cyc_List_List*_tmp120;
struct Cyc_List_List*_tmp121;struct _tuple0 _tmp11F=((struct _tuple0(*)(struct Cyc_List_List*
x))Cyc_List_split)(nmtypes);_tmp120=_tmp11F.f1;_tmp121=_tmp11F.f2;{struct
_tuple25 _tmp123;struct Cyc_Dict_Dict _tmp124;struct Cyc_List_List*_tmp125;struct
_tuple25*_tmp122=((struct _tuple25*(*)(struct _tuple25*(*f)(struct _tuple26*,void*,
struct _tuple25*),struct _tuple26*,struct Cyc_List_List*x,struct _tuple25*accum))Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls,_tmp11D,_tmp121,_tmp11E);_tmp123=*_tmp122;_tmp124=
_tmp123.f1;_tmp125=_tmp123.f2;dict=_tmp124;{struct Cyc_List_List*_tmp126=((struct
Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp121),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*
_tmp127=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple19*,int),
struct _tuple19*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple19*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->f1=type;_tmp138->f2=
loc;_tmp138;}),_tmp126);struct Cyc_List_List*_tmp128=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct _dyneither_ptr*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,
loc,_tmp120);struct Cyc_List_List*_tmp129=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp125);struct Cyc_List_List*_tmp12A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple18*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*
z))Cyc_List_zip3)(_tmp127,_tmp128,_tmp129));void*tuple_type=Cyc_Tcgenrep_tuple_typ(({
void*_tmp137[3];_tmp137[2]=Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str));_tmp137[1]=Cyc_Absyn_const_string_typ((void*)& Cyc_Absyn_HeapRgn_val);
_tmp137[0]=Cyc_Absyn_uint_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp137,sizeof(void*),3));}));struct _tuple20
_tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;struct _tuple20*
_tmp12B=Cyc_Tcgenrep_array_decls(tuple_type,_tmp12A,loc);_tmp12C=*_tmp12B;
_tmp12D=_tmp12C.f1;_tmp12E=_tmp12C.f2;{struct Cyc_Absyn_Exp*name;if(sname == 0)
name=Cyc_Absyn_null_exp(loc);else{struct _dyneither_ptr*_tmp12F=Cyc_Tcgenrep_new_gen_id(({
const char*_tmp132="name";_tag_dyneither(_tmp132,sizeof(char),5);}));struct Cyc_Absyn_Decl*
_tmp130=Cyc_Tcgenrep_gen_decl(_tmp12F,Cyc_Absyn_const_string_typ((void*)& Cyc_Absyn_HeapRgn_val),(
struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((struct _tuple5*)sname->v)).f2,
loc),loc);_tmp12D=({struct Cyc_List_List*_tmp131=_cycalloc(sizeof(*_tmp131));
_tmp131->hd=_tmp130;_tmp131->tl=_tmp12D;_tmp131;});name=Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(_tmp12F),loc),loc);}{struct
Cyc_Absyn_Decl*_tmp133=Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_struct_str),
varname,({struct Cyc_Absyn_Exp*_tmp136[3];_tmp136[2]=_tmp12E;_tmp136[1]=_tmp11C;
_tmp136[0]=name;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp136,sizeof(struct Cyc_Absyn_Exp*),3));}),sc,loc);struct Cyc_List_List*
_tmp134=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp12D,({struct Cyc_Absyn_Decl*_tmp135[1];_tmp135[0]=
_tmp133;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp135,sizeof(struct Cyc_Absyn_Decl*),1));}));return Cyc_Tcgenrep_update_info(
dict,type,_tmp134,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp125,0);};};};};}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Core_Opt*uname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct
Cyc_Position_Segment*loc,struct _tuple5*varname,enum Cyc_Absyn_Scope sc,void*type,
struct Cyc_List_List*nmtypes){struct Cyc_Absyn_Exp*_tmp13B=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple26*_tmp13C=({struct _tuple26*_tmp156=_cycalloc(sizeof(*
_tmp156));_tmp156->f1=te;_tmp156->f2=loc;_tmp156;});struct _tuple25*_tmp13D=({
struct _tuple25*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->f1=dict;_tmp155->f2=
0;_tmp155;});struct Cyc_List_List*_tmp13F;struct Cyc_List_List*_tmp140;struct
_tuple0 _tmp13E=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nmtypes);
_tmp13F=_tmp13E.f1;_tmp140=_tmp13E.f2;{struct _tuple25 _tmp142;struct Cyc_Dict_Dict
_tmp143;struct Cyc_List_List*_tmp144;struct _tuple25*_tmp141=((struct _tuple25*(*)(
struct _tuple25*(*f)(struct _tuple26*,void*,struct _tuple25*),struct _tuple26*,
struct Cyc_List_List*x,struct _tuple25*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp13C,_tmp140,_tmp13D);_tmp142=*_tmp141;_tmp143=_tmp142.f1;_tmp144=_tmp142.f2;
dict=_tmp143;{struct Cyc_List_List*_tmp145=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp13F);struct Cyc_List_List*_tmp146=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp144);struct Cyc_List_List*_tmp147=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple17*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp145,_tmp146));void*_tmp148=Cyc_Tcgenrep_tuple_typ(({void*_tmp154[2];_tmp154[
1]=Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));
_tmp154[0]=Cyc_Absyn_const_string_typ((void*)& Cyc_Absyn_HeapRgn_val);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp154,sizeof(void*),2));}));
struct _tuple20 _tmp14A;struct Cyc_List_List*_tmp14B;struct Cyc_Absyn_Exp*_tmp14C;
struct _tuple20*_tmp149=Cyc_Tcgenrep_array_decls(_tmp148,_tmp147,loc);_tmp14A=*
_tmp149;_tmp14B=_tmp14A.f1;_tmp14C=_tmp14A.f2;{struct Cyc_Absyn_Exp*name;if(uname
== 0)name=Cyc_Absyn_null_exp(loc);else{struct _dyneither_ptr*_tmp14D=Cyc_Tcgenrep_new_gen_id(({
const char*_tmp150="name";_tag_dyneither(_tmp150,sizeof(char),5);}));struct Cyc_Absyn_Decl*
_tmp14E=Cyc_Tcgenrep_gen_decl(_tmp14D,Cyc_Absyn_const_string_typ((void*)& Cyc_Absyn_HeapRgn_val),(
struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((struct _tuple5*)uname->v)).f2,
loc),loc);_tmp14B=({struct Cyc_List_List*_tmp14F=_cycalloc(sizeof(*_tmp14F));
_tmp14F->hd=_tmp14E;_tmp14F->tl=_tmp14B;_tmp14F;});name=Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(_tmp14D),loc),loc);}{struct
Cyc_Absyn_Decl*_tmp151=Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_union_str),
varname,({struct Cyc_Absyn_Exp*_tmp153[3];_tmp153[2]=_tmp14C;_tmp153[1]=_tmp13B;
_tmp153[0]=name;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp153,sizeof(struct Cyc_Absyn_Exp*),3));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp14B,({struct Cyc_Absyn_Decl*_tmp152[1];_tmp152[0]=
_tmp151;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp152,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp144,0);};};};};}
static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepEnum(struct _tuple5**ename,struct
Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,struct
_tuple5*varname,enum Cyc_Absyn_Scope sc,void*type,struct Cyc_List_List*tagnms){
struct Cyc_Absyn_Exp*_tmp157=Cyc_Absyn_sizeoftyp_exp(type,loc);struct Cyc_List_List*
_tmp159;struct Cyc_List_List*_tmp15A;struct _tuple0 _tmp158=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(tagnms);_tmp159=_tmp158.f1;_tmp15A=
_tmp158.f2;{struct Cyc_List_List*_tmp15B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple5*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp15A);struct Cyc_List_List*_tmp15C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp159);struct Cyc_List_List*
_tmp15D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple17*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmp15C,_tmp15B));void*_tmp15E=Cyc_Tcgenrep_tuple_typ(({
void*_tmp16A[2];_tmp16A[1]=Cyc_Absyn_const_string_typ((void*)& Cyc_Absyn_HeapRgn_val);
_tmp16A[0]=Cyc_Absyn_uint_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp16A,sizeof(void*),2));}));struct _tuple20
_tmp160;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct _tuple20*
_tmp15F=Cyc_Tcgenrep_array_decls(_tmp15E,_tmp15D,loc);_tmp160=*_tmp15F;_tmp161=
_tmp160.f1;_tmp162=_tmp160.f2;{struct Cyc_Absyn_Exp*name;if(ename == 0)name=Cyc_Absyn_null_exp(
loc);else{struct _dyneither_ptr*_tmp163=Cyc_Tcgenrep_new_gen_id(({const char*
_tmp166="name";_tag_dyneither(_tmp166,sizeof(char),5);}));struct Cyc_Absyn_Decl*
_tmp164=Cyc_Tcgenrep_gen_decl(_tmp163,Cyc_Absyn_const_string_typ((void*)& Cyc_Absyn_HeapRgn_val),(
struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*(*ename)).f2,loc),loc);_tmp161=({
struct Cyc_List_List*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->hd=_tmp164;
_tmp165->tl=_tmp161;_tmp165;});name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name(_tmp163),loc),loc);}{struct Cyc_Absyn_Decl*_tmp167=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str),varname,({struct Cyc_Absyn_Exp*_tmp169[3];_tmp169[2]=
_tmp162;_tmp169[1]=_tmp157;_tmp169[0]=name;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp169,sizeof(struct Cyc_Absyn_Exp*),
3));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp161,({
struct Cyc_Absyn_Decl*_tmp168[1];_tmp168[0]=_tmp167;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp168,sizeof(struct Cyc_Absyn_Decl*),
1));})),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),0,0);};};};}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepDatatype(
struct _tuple5*tname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple5*varname,enum Cyc_Absyn_Scope sc,void*type,struct Cyc_List_List*
tonames,struct Cyc_List_List*nmtypes){struct Cyc_List_List*_tmp16B=((struct Cyc_List_List*(*)(
int n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tonames),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp16C=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp16B);
struct Cyc_List_List*_tmp16D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple5*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
tonames);struct Cyc_List_List*_tmp16E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp16C,_tmp16D);struct Cyc_List_List*
_tmp16F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple17*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp16E);void*_tmp170=Cyc_Tcgenrep_tuple_typ(({
void*_tmp18F[2];_tmp18F[1]=Cyc_Absyn_const_string_typ((void*)& Cyc_Absyn_HeapRgn_val);
_tmp18F[0]=Cyc_Absyn_uint_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp18F,sizeof(void*),2));}));struct _tuple20
_tmp172;struct Cyc_List_List*_tmp173;struct Cyc_Absyn_Exp*_tmp174;struct _tuple20*
_tmp171=Cyc_Tcgenrep_array_decls(_tmp170,_tmp16F,loc);_tmp172=*_tmp171;_tmp173=
_tmp172.f1;_tmp174=_tmp172.f2;{struct _tuple26*_tmp175=({struct _tuple26*_tmp18E=
_cycalloc(sizeof(*_tmp18E));_tmp18E->f1=te;_tmp18E->f2=loc;_tmp18E;});struct
_tuple25*_tmp176=({struct _tuple25*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->f1=
dict;_tmp18D->f2=0;_tmp18D;});struct Cyc_List_List*_tmp178;struct Cyc_List_List*
_tmp179;struct _tuple0 _tmp177=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nmtypes);_tmp178=_tmp177.f1;_tmp179=_tmp177.f2;{struct _tuple25 _tmp17B;struct Cyc_Dict_Dict
_tmp17C;struct Cyc_List_List*_tmp17D;struct _tuple25*_tmp17A=((struct _tuple25*(*)(
struct _tuple25*(*f)(struct _tuple26*,void*,struct _tuple25*),struct _tuple26*,
struct Cyc_List_List*x,struct _tuple25*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp175,_tmp179,_tmp176);_tmp17B=*_tmp17A;_tmp17C=_tmp17B.f1;_tmp17D=_tmp17B.f2;
dict=_tmp17C;{struct Cyc_List_List*_tmp17E=((struct Cyc_List_List*(*)(int n,int(*f)(
int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp17D),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp17F=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp17E);
struct Cyc_List_List*_tmp180=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple5*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp178);struct Cyc_List_List*_tmp181=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp17D);struct Cyc_List_List*_tmp182=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp17F,_tmp180,
_tmp181);struct Cyc_List_List*_tmp183=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple18*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,_tmp182);
void*tuple_type2=Cyc_Tcgenrep_tuple_typ(({void*_tmp18C[3];_tmp18C[2]=Cyc_Tcgenrep_datatype_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str));_tmp18C[1]=Cyc_Absyn_const_string_typ((
void*)& Cyc_Absyn_HeapRgn_val);_tmp18C[0]=Cyc_Absyn_uint_typ;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp18C,sizeof(void*),3));}));
struct _tuple20 _tmp185;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Exp*_tmp187;
struct _tuple20*_tmp184=Cyc_Tcgenrep_array_decls(tuple_type2,_tmp183,loc);_tmp185=*
_tmp184;_tmp186=_tmp185.f1;_tmp187=_tmp185.f2;{struct Cyc_Absyn_Exp*_tmp188=Cyc_Tcgenrep_cnst_string(*(*
tname).f2,loc);struct Cyc_Absyn_Decl*_tmp189=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_datatype_str),varname,({struct Cyc_Absyn_Exp*_tmp18B[3];_tmp18B[2]=
_tmp187;_tmp18B[1]=_tmp174;_tmp18B[0]=_tmp188;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp18B,sizeof(struct Cyc_Absyn_Exp*),
3));}),sc,loc);return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp173,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp186,({struct Cyc_Absyn_Decl*_tmp18A[1];_tmp18A[0]=_tmp189;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp18A,sizeof(struct Cyc_Absyn_Decl*),
1));}))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp17D,0);return dict;};};};};}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepXDatatype(
struct _tuple5*xname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple5*varname,enum Cyc_Absyn_Scope sc,void*type,struct Cyc_List_List*
nametypes){struct Cyc_List_List*_tmp191;struct Cyc_List_List*_tmp192;struct _tuple0
_tmp190=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nametypes);
_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;{struct _tuple26*_tmp193=({struct _tuple26*
_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->f1=te;_tmp1A8->f2=loc;_tmp1A8;});
struct _tuple25*_tmp194=({struct _tuple25*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));
_tmp1A7->f1=dict;_tmp1A7->f2=0;_tmp1A7;});struct _tuple25 _tmp196;struct Cyc_Dict_Dict
_tmp197;struct Cyc_List_List*_tmp198;struct _tuple25*_tmp195=((struct _tuple25*(*)(
struct _tuple25*(*f)(struct _tuple26*,void*,struct _tuple25*),struct _tuple26*,
struct Cyc_List_List*x,struct _tuple25*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp193,_tmp192,_tmp194);_tmp196=*_tmp195;_tmp197=_tmp196.f1;_tmp198=_tmp196.f2;
dict=_tmp197;{struct Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _dyneither_ptr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp191);struct Cyc_List_List*_tmp19A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp198);struct Cyc_List_List*_tmp19B=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp199,_tmp19A);struct Cyc_List_List*
_tmp19C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple17*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp19B);void*name_type=Cyc_Absyn_dyneither_typ(
Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,({struct Cyc_Absyn_Tqual _tmp1A6;
_tmp1A6.print_const=1;_tmp1A6.q_volatile=0;_tmp1A6.q_restrict=0;_tmp1A6.real_const=
1;_tmp1A6.loc=0;_tmp1A6;}),Cyc_Absyn_true_conref);void*tuple_type=Cyc_Tcgenrep_tuple_typ(({
void*_tmp1A5[2];_tmp1A5[1]=Cyc_Tcgenrep_datatype_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str));_tmp1A5[0]=name_type;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1A5,sizeof(void*),2));}));
struct _tuple20 _tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_Absyn_Exp*_tmp1A0;
struct _tuple20*_tmp19D=Cyc_Tcgenrep_array_decls(tuple_type,_tmp19C,loc);_tmp19E=*
_tmp19D;_tmp19F=_tmp19E.f1;_tmp1A0=_tmp19E.f2;{struct Cyc_Absyn_Exp*_tmp1A1=Cyc_Tcgenrep_cnst_string(*(*
xname).f2,loc);struct Cyc_Absyn_Decl*_tmp1A2=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xdatatype_str),varname,({struct Cyc_Absyn_Exp*_tmp1A4[2];_tmp1A4[1]=
_tmp1A0;_tmp1A4[0]=_tmp1A1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1A4,sizeof(struct Cyc_Absyn_Exp*),2));}),sc,loc);return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp19F,({struct Cyc_Absyn_Decl*_tmp1A3[1];_tmp1A3[0]=
_tmp1A2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1A3,sizeof(struct Cyc_Absyn_Decl*),1));})),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp198,0);
return dict;};};};}static struct _tuple24*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,void*type){struct Cyc_Tcgenrep_RepInfo**
info=((struct Cyc_Tcgenrep_RepInfo**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(
dict,type);if(info != 0){if((*info)->is_extern)dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict,void*))Cyc_Dict_delete)(dict,type);else{struct _tuple24*
_tmp1A9=({struct _tuple24*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA->f1=dict;
_tmp1AA->f2=*info;_tmp1AA;});return _tmp1A9;}}{void*_tmp1AB=Cyc_Tcutil_compress(
Cyc_Tcgenrep_monomorphize_type(type));enum Cyc_Absyn_Sign _tmp1AD;enum Cyc_Absyn_Size_of
_tmp1AE;int _tmp1B1;struct Cyc_Absyn_PtrInfo _tmp1B3;struct Cyc_Absyn_ArrayInfo
_tmp1B5;void*_tmp1B6;struct Cyc_Absyn_Tqual _tmp1B7;struct Cyc_Absyn_Exp*_tmp1B8;
union Cyc_Absyn_Constraint*_tmp1B9;struct Cyc_List_List*_tmp1BB;struct _tuple5*
_tmp1BD;struct Cyc_List_List*_tmp1BE;struct Cyc_Absyn_Typedefdecl*_tmp1BF;void**
_tmp1C0;struct Cyc_Absyn_Tvar*_tmp1C4;struct Cyc_Absyn_FnInfo _tmp1C6;struct Cyc_Absyn_DatatypeInfo
_tmp1CF;union Cyc_Absyn_DatatypeInfoU _tmp1D0;struct Cyc_Absyn_Datatypedecl**
_tmp1D1;struct Cyc_Absyn_Datatypedecl*_tmp1D2;struct Cyc_List_List*_tmp1D3;struct
Cyc_Absyn_DatatypeFieldInfo _tmp1D6;struct Cyc_Absyn_AggrInfo _tmp1D8;union Cyc_Absyn_AggrInfoU
_tmp1D9;struct Cyc_List_List*_tmp1DA;enum Cyc_Absyn_AggrKind _tmp1DC;struct Cyc_List_List*
_tmp1DD;enum Cyc_Absyn_AggrKind _tmp1DF;struct Cyc_List_List*_tmp1E0;struct Cyc_Absyn_Enumdecl*
_tmp1E2;struct Cyc_List_List*_tmp1E4;_LL13: {struct Cyc_Absyn_IntType_struct*
_tmp1AC=(struct Cyc_Absyn_IntType_struct*)_tmp1AB;if(_tmp1AC->tag != 6)goto _LL15;
else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;}}_LL14: {struct _tuple5*_tmp1E7=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp1EB="rep";_tag_dyneither(_tmp1EB,sizeof(
char),4);})));struct Cyc_Absyn_Decl*_tmp1E8=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_int_str),_tmp1E7,({struct Cyc_Absyn_Exp*_tmp1EA[2];_tmp1EA[1]=Cyc_Tcgenrep_cnst_int(
Cyc_Tcgenrep_size_of2int(_tmp1AE),loc);_tmp1EA[0]=Cyc_Tcgenrep_cnst_int(_tmp1AD
== Cyc_Absyn_Signed?1: 0,loc);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1EA,sizeof(struct Cyc_Absyn_Exp*),2));}),Cyc_Absyn_Static,loc);
dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp1E9[1];
_tmp1E9[0]=_tmp1E8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1E9,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1E7,loc),loc),0,0);goto _LL12;}
_LL15: {struct Cyc_Absyn_FloatType_struct*_tmp1AF=(struct Cyc_Absyn_FloatType_struct*)
_tmp1AB;if(_tmp1AF->tag != 7)goto _LL17;}_LL16: dict=Cyc_Tcgenrep_update_info(dict,
type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_float_str),loc),0,0);goto _LL12;_LL17: {struct Cyc_Absyn_DoubleType_struct*
_tmp1B0=(struct Cyc_Absyn_DoubleType_struct*)_tmp1AB;if(_tmp1B0->tag != 8)goto
_LL19;else{_tmp1B1=_tmp1B0->f1;}}_LL18: dict=Cyc_Tcgenrep_update_info(dict,type,0,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_double_str),
loc),0,0);goto _LL12;_LL19: {struct Cyc_Absyn_PointerType_struct*_tmp1B2=(struct
Cyc_Absyn_PointerType_struct*)_tmp1AB;if(_tmp1B2->tag != 5)goto _LL1B;else{_tmp1B3=
_tmp1B2->f1;}}_LL1A:{void*_tmp1EC=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((
_tmp1B3.ptr_atts).bounds);struct Cyc_Absyn_Exp*_tmp1EE;_LL4C: {struct Cyc_Absyn_Upper_b_struct*
_tmp1ED=(struct Cyc_Absyn_Upper_b_struct*)_tmp1EC;if(_tmp1ED->tag != 1)goto _LL4E;
else{_tmp1EE=_tmp1ED->f1;}}_LL4D: {struct _tuple5*_tmp1F0=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp1FB="rep";_tag_dyneither(_tmp1FB,sizeof(
char),4);})));struct Cyc_Core_Opt*_tmp1F1=({struct Cyc_Core_Opt*_tmp1FA=_cycalloc(
sizeof(*_tmp1FA));_tmp1FA->v=Cyc_Tcgenrep_gen_vardecl(_tmp1F0,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str)),0,Cyc_Absyn_Extern,loc);_tmp1FA;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1F0,loc),loc),_tmp1F1,0);{struct _tuple24 _tmp1F3;struct Cyc_Dict_Dict _tmp1F4;
struct Cyc_Tcgenrep_RepInfo*_tmp1F5;struct _tuple24*_tmp1F2=Cyc_Tcgenrep_lookupRep(
te,dict,loc,_tmp1B3.elt_typ);_tmp1F3=*_tmp1F2;_tmp1F4=_tmp1F3.f1;_tmp1F5=_tmp1F3.f2;
dict=_tmp1F4;{struct Cyc_Absyn_Decl*_tmp1F6=Cyc_Tcgenrep_datatype_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp1F0,({struct Cyc_Absyn_Exp*_tmp1F9[2];_tmp1F9[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp1F5->exp);_tmp1F9[0]=_tmp1EE;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1F9,sizeof(struct Cyc_Absyn_Exp*),
2));}),Cyc_Absyn_Static,loc);dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*
_tmp1F7[1];_tmp1F7[0]=_tmp1F6;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp1F7,sizeof(struct Cyc_Absyn_Decl*),1));}),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1F0,loc),
loc),({struct Cyc_Tcgenrep_RepInfo*_tmp1F8[1];_tmp1F8[0]=_tmp1F5;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1F8,sizeof(struct Cyc_Tcgenrep_RepInfo*),
1));}),0);goto _LL4B;};};}_LL4E: {struct Cyc_Absyn_DynEither_b_struct*_tmp1EF=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp1EC;if(_tmp1EF->tag != 0)goto _LL4B;}_LL4F: {
struct _tuple5*_tmp1FC=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp207="rep";_tag_dyneither(_tmp207,sizeof(char),4);})));struct Cyc_Core_Opt*
_tmp1FD=({struct Cyc_Core_Opt*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->v=Cyc_Tcgenrep_gen_vardecl(
_tmp1FC,Cyc_Tcgenrep_datatypefield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str)),0,Cyc_Absyn_Extern,loc);
_tmp206;});dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp1FC,loc),loc),_tmp1FD,0);{
struct _tuple24 _tmp1FF;struct Cyc_Dict_Dict _tmp200;struct Cyc_Tcgenrep_RepInfo*
_tmp201;struct _tuple24*_tmp1FE=Cyc_Tcgenrep_lookupRep(te,dict,loc,_tmp1B3.elt_typ);
_tmp1FF=*_tmp1FE;_tmp200=_tmp1FF.f1;_tmp201=_tmp1FF.f2;dict=_tmp200;{struct Cyc_Absyn_Decl*
_tmp202=Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str),_tmp1FC,({struct Cyc_Absyn_Exp*
_tmp205[1];_tmp205[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp201->exp);((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp205,
sizeof(struct Cyc_Absyn_Exp*),1));}),Cyc_Absyn_Static,loc);dict=Cyc_Tcgenrep_update_info(
dict,type,({struct Cyc_Absyn_Decl*_tmp203[1];_tmp203[0]=_tmp202;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp203,sizeof(struct Cyc_Absyn_Decl*),
1));}),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1FC,loc),loc),({struct Cyc_Tcgenrep_RepInfo*_tmp204[1];_tmp204[0]=_tmp201;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp204,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),0);goto _LL4B;};};}_LL4B:;}
goto _LL12;_LL1B: {struct Cyc_Absyn_ArrayType_struct*_tmp1B4=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1AB;if(_tmp1B4->tag != 9)goto _LL1D;else{_tmp1B5=_tmp1B4->f1;_tmp1B6=_tmp1B5.elt_type;
_tmp1B7=_tmp1B5.tq;_tmp1B8=_tmp1B5.num_elts;_tmp1B9=_tmp1B5.zero_term;}}_LL1C:
if(_tmp1B8 == 0)({void*_tmp208=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp209="tcgenrep:At this point, array bounds must be constants";
_tag_dyneither(_tmp209,sizeof(char),55);}),_tag_dyneither(_tmp208,sizeof(void*),
0));});{struct _tuple24 _tmp20B;struct Cyc_Dict_Dict _tmp20C;struct Cyc_Tcgenrep_RepInfo*
_tmp20D;struct _tuple24*_tmp20A=Cyc_Tcgenrep_lookupRep(te,dict,loc,_tmp1B6);
_tmp20B=*_tmp20A;_tmp20C=_tmp20B.f1;_tmp20D=_tmp20B.f2;dict=_tmp20C;{struct
_tuple5*_tmp20E=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const char*
_tmp213="rep";_tag_dyneither(_tmp213,sizeof(char),4);})));struct Cyc_Absyn_Decl*
_tmp20F=Cyc_Tcgenrep_datatype_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str),_tmp20E,({struct Cyc_Absyn_Exp*
_tmp212[2];_tmp212[1]=(struct Cyc_Absyn_Exp*)_check_null(_tmp20D->exp);_tmp212[0]=(
struct Cyc_Absyn_Exp*)_tmp1B8;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp212,sizeof(struct Cyc_Absyn_Exp*),2));}),Cyc_Absyn_Static,loc);
dict=Cyc_Tcgenrep_update_info(dict,type,({struct Cyc_Absyn_Decl*_tmp210[1];
_tmp210[0]=_tmp20F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp210,sizeof(struct Cyc_Absyn_Decl*),1));}),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp20E,loc),loc),({struct Cyc_Tcgenrep_RepInfo*
_tmp211[1];_tmp211[0]=_tmp20D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp211,sizeof(struct Cyc_Tcgenrep_RepInfo*),1));}),
0);goto _LL12;};};_LL1D: {struct Cyc_Absyn_TupleType_struct*_tmp1BA=(struct Cyc_Absyn_TupleType_struct*)
_tmp1AB;if(_tmp1BA->tag != 11)goto _LL1F;else{_tmp1BB=_tmp1BA->f1;}}_LL1E: {struct
_tuple5*_tmp214=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const char*
_tmp218="rep";_tag_dyneither(_tmp218,sizeof(char),4);})));struct Cyc_Core_Opt*
_tmp215=({struct Cyc_Core_Opt*_tmp217=_cycalloc(sizeof(*_tmp217));_tmp217->v=Cyc_Tcgenrep_gen_vardecl(
_tmp214,Cyc_Tcgenrep_datatypefield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tuple_str)),0,Cyc_Absyn_Extern,loc);
_tmp217;});dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp214,loc),loc),_tmp215,0);{
struct Cyc_List_List*_tmp216=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple15*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp1BB);dict=Cyc_Tcgenrep_buildRepTuple(
te,dict,loc,_tmp214,Cyc_Absyn_Static,type,_tmp216);goto _LL12;};}_LL1F: {struct
Cyc_Absyn_TypedefType_struct*_tmp1BC=(struct Cyc_Absyn_TypedefType_struct*)
_tmp1AB;if(_tmp1BC->tag != 18)goto _LL21;else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;
_tmp1BF=_tmp1BC->f3;_tmp1C0=_tmp1BC->f4;}}_LL20: if(_tmp1C0 == 0)({void*_tmp219=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp21A="gen(): can't gen for abstract typedef";_tag_dyneither(_tmp21A,
sizeof(char),38);}),_tag_dyneither(_tmp219,sizeof(void*),0));});{struct _tuple24
_tmp21C;struct Cyc_Dict_Dict _tmp21D;struct Cyc_Tcgenrep_RepInfo*_tmp21E;struct
_tuple24*_tmp21B=Cyc_Tcgenrep_lookupRep(te,dict,loc,*_tmp1C0);_tmp21C=*_tmp21B;
_tmp21D=_tmp21C.f1;_tmp21E=_tmp21C.f2;dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(_tmp21D,type,_tmp21E);
goto _LL12;};_LL21: {struct Cyc_Absyn_VoidType_struct*_tmp1C1=(struct Cyc_Absyn_VoidType_struct*)
_tmp1AB;if(_tmp1C1->tag != 0)goto _LL23;}_LL22:({void*_tmp21F=0;Cyc_Tcutil_terr(
loc,({const char*_tmp220="found void in gen() expression";_tag_dyneither(_tmp220,
sizeof(char),31);}),_tag_dyneither(_tmp21F,sizeof(void*),0));});goto _LL12;_LL23: {
struct Cyc_Absyn_Evar_struct*_tmp1C2=(struct Cyc_Absyn_Evar_struct*)_tmp1AB;if(
_tmp1C2->tag != 1)goto _LL25;}_LL24:({void*_tmp221=0;Cyc_Tcutil_terr(loc,({const
char*_tmp222="found evar in gen() expression";_tag_dyneither(_tmp222,sizeof(char),
31);}),_tag_dyneither(_tmp221,sizeof(void*),0));});goto _LL12;_LL25: {struct Cyc_Absyn_VarType_struct*
_tmp1C3=(struct Cyc_Absyn_VarType_struct*)_tmp1AB;if(_tmp1C3->tag != 2)goto _LL27;
else{_tmp1C4=_tmp1C3->f1;}}_LL26:({void*_tmp223=0;Cyc_Tcutil_terr(loc,({const
char*_tmp224="found tyvar in gen() expression";_tag_dyneither(_tmp224,sizeof(
char),32);}),_tag_dyneither(_tmp223,sizeof(void*),0));});goto _LL12;_LL27: {
struct Cyc_Absyn_FnType_struct*_tmp1C5=(struct Cyc_Absyn_FnType_struct*)_tmp1AB;
if(_tmp1C5->tag != 10)goto _LL29;else{_tmp1C6=_tmp1C5->f1;}}_LL28:({void*_tmp225=0;
Cyc_Tcutil_terr(loc,({const char*_tmp226="found function type in gen() expression";
_tag_dyneither(_tmp226,sizeof(char),40);}),_tag_dyneither(_tmp225,sizeof(void*),
0));});goto _LL12;_LL29: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1C7=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1AB;if(_tmp1C7->tag != 16)goto _LL2B;}_LL2A: goto _LL2C;_LL2B: {struct Cyc_Absyn_DynRgnType_struct*
_tmp1C8=(struct Cyc_Absyn_DynRgnType_struct*)_tmp1AB;if(_tmp1C8->tag != 17)goto
_LL2D;}_LL2C: goto _LL2E;_LL2D: {struct Cyc_Absyn_UniqueRgn_struct*_tmp1C9=(struct
Cyc_Absyn_UniqueRgn_struct*)_tmp1AB;if(_tmp1C9->tag != 22)goto _LL2F;}_LL2E: goto
_LL30;_LL2F: {struct Cyc_Absyn_HeapRgn_struct*_tmp1CA=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp1AB;if(_tmp1CA->tag != 21)goto _LL31;}_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_AccessEff_struct*
_tmp1CB=(struct Cyc_Absyn_AccessEff_struct*)_tmp1AB;if(_tmp1CB->tag != 23)goto
_LL33;}_LL32: goto _LL34;_LL33: {struct Cyc_Absyn_JoinEff_struct*_tmp1CC=(struct Cyc_Absyn_JoinEff_struct*)
_tmp1AB;if(_tmp1CC->tag != 24)goto _LL35;}_LL34: goto _LL36;_LL35: {struct Cyc_Absyn_RgnsEff_struct*
_tmp1CD=(struct Cyc_Absyn_RgnsEff_struct*)_tmp1AB;if(_tmp1CD->tag != 25)goto _LL37;}
_LL36:({void*_tmp227=0;Cyc_Tcutil_terr(loc,({const char*_tmp228="gen(): unhandled region, handle or effect type";
_tag_dyneither(_tmp228,sizeof(char),47);}),_tag_dyneither(_tmp227,sizeof(void*),
0));});goto _LL12;_LL37: {struct Cyc_Absyn_DatatypeType_struct*_tmp1CE=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp1AB;if(_tmp1CE->tag != 3)goto _LL39;else{_tmp1CF=_tmp1CE->f1;_tmp1D0=_tmp1CF.datatype_info;
if((_tmp1D0.KnownDatatype).tag != 2)goto _LL39;_tmp1D1=(struct Cyc_Absyn_Datatypedecl**)(
_tmp1D0.KnownDatatype).val;_tmp1D2=*_tmp1D1;_tmp1D3=_tmp1CF.targs;}}_LL38: if(
_tmp1D2->tvs != 0)({void*_tmp229=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp22A="gen(): datatype type variables not handled yet";
_tag_dyneither(_tmp22A,sizeof(char),47);}),_tag_dyneither(_tmp229,sizeof(void*),
0));});if(_tmp1D2->fields == 0){struct _tuple5*_tmp22B=Cyc_Tcgenrep_toplevel_name(({
struct _dyneither_ptr*_tmp22E=_cycalloc(sizeof(*_tmp22E));_tmp22E[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp22E;}));struct Cyc_Core_Opt*_tmp22C=({struct Cyc_Core_Opt*_tmp22D=
_cycalloc(sizeof(*_tmp22D));_tmp22D->v=Cyc_Tcgenrep_gen_vardecl(_tmp22B,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_datatype_str)),0,Cyc_Absyn_Extern,loc);_tmp22D;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp22B,loc),loc),_tmp22C,1);goto _LL12;}if(!_tmp1D2->is_extensible){struct
_tuple5*_tmp22F=Cyc_Tcgenrep_toplevel_name(({struct _dyneither_ptr*_tmp237=
_cycalloc(sizeof(*_tmp237));_tmp237[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp237;}));struct Cyc_Core_Opt*_tmp230=({struct Cyc_Core_Opt*_tmp236=_cycalloc(
sizeof(*_tmp236));_tmp236->v=Cyc_Tcgenrep_gen_vardecl(_tmp22F,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_datatype_str)),0,Cyc_Absyn_Extern,loc);_tmp236;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp22F,loc),loc),_tmp230,0);{struct Cyc_List_List*_tmp231=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D2->fields))->v;struct Cyc_List_List*_tmp232=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_datatypefield,_tmp231);struct Cyc_List_List*
_tmp233=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Datatypefield*),struct
Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_nonempty_datatypefield,
_tmp231);struct Cyc_List_List*_tmp234=((struct Cyc_List_List*(*)(struct _tuple23*(*
f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_datatypefield_and_get_nmtype,
_tmp232);struct Cyc_List_List*_tmp235=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(
struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_datatypefield_and_get_name,
_tmp233);dict=Cyc_Tcgenrep_buildRepDatatype(_tmp1D2->name,te,dict,loc,_tmp22F,
Cyc_Absyn_Public,type,_tmp235,_tmp234);};}else{struct _tuple5*_tmp238=Cyc_Tcgenrep_toplevel_name(({
struct _dyneither_ptr*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp23E;}));struct Cyc_Core_Opt*_tmp239=({struct Cyc_Core_Opt*_tmp23D=
_cycalloc(sizeof(*_tmp23D));_tmp23D->v=Cyc_Tcgenrep_gen_vardecl(_tmp238,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xdatatype_str)),0,Cyc_Absyn_Extern,loc);_tmp23D;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp238,loc),loc),_tmp239,0);{struct Cyc_List_List*_tmp23A=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D2->fields))->v;struct Cyc_List_List*_tmp23B=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_datatypefield,_tmp23A);struct Cyc_List_List*
_tmp23C=((struct Cyc_List_List*(*)(struct _tuple21*(*f)(struct Cyc_Absyn_Datatypefield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_xdatatypefield_and_get_name_type,
_tmp23B);dict=Cyc_Tcgenrep_buildRepXDatatype(_tmp1D2->name,te,dict,loc,_tmp238,
Cyc_Absyn_Public,type,_tmp23C);};}goto _LL12;_LL39: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1D4=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1AB;if(_tmp1D4->tag != 3)goto
_LL3B;}_LL3A:({void*_tmp23F=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp240="gen(): datatype must be resolved by now";
_tag_dyneither(_tmp240,sizeof(char),40);}),_tag_dyneither(_tmp23F,sizeof(void*),
0));});_LL3B: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1D5=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1AB;if(_tmp1D5->tag != 4)goto _LL3D;else{_tmp1D6=_tmp1D5->f1;}}_LL3C: if(
_tmp1D6.targs != 0)({void*_tmp241=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp242="gen(): datatypefield type parameters not handled yet";
_tag_dyneither(_tmp242,sizeof(char),53);}),_tag_dyneither(_tmp241,sizeof(void*),
0));});{union Cyc_Absyn_DatatypeFieldInfoU _tmp243=_tmp1D6.field_info;struct
_tuple6 _tmp244;struct Cyc_Absyn_Datatypedecl*_tmp245;struct Cyc_Absyn_Datatypefield*
_tmp246;_LL51: if((_tmp243.KnownDatatypefield).tag != 2)goto _LL53;_tmp244=(struct
_tuple6)(_tmp243.KnownDatatypefield).val;_tmp245=_tmp244.f1;_tmp246=_tmp244.f2;
_LL52: {struct Cyc_List_List*_tmp247=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,_tmp246->typs);
struct Cyc_List_List*_tmp248=({struct Cyc_List_List*_tmp24B=_cycalloc(sizeof(*
_tmp24B));_tmp24B->hd=Cyc_Absyn_uint_typ;_tmp24B->tl=_tmp247;_tmp24B;});struct
_tuple5*_tmp249=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const char*
_tmp24A="rep";_tag_dyneither(_tmp24A,sizeof(char),4);})));dict=Cyc_Tcgenrep_buildRepDatatypefield(
_tmp245->name,_tmp246->name,te,dict,loc,_tmp249,Cyc_Absyn_Static,type,_tmp248);
goto _LL50;}_LL53:;_LL54:({void*_tmp24C=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp24D="gen(): datatypefield must be known at this point";
_tag_dyneither(_tmp24D,sizeof(char),49);}),_tag_dyneither(_tmp24C,sizeof(void*),
0));});_LL50:;}goto _LL12;_LL3D: {struct Cyc_Absyn_AggrType_struct*_tmp1D7=(struct
Cyc_Absyn_AggrType_struct*)_tmp1AB;if(_tmp1D7->tag != 12)goto _LL3F;else{_tmp1D8=
_tmp1D7->f1;_tmp1D9=_tmp1D8.aggr_info;_tmp1DA=_tmp1D8.targs;}}_LL3E: {struct Cyc_Absyn_Aggrdecl*
_tmp24E=Cyc_Absyn_get_known_aggrdecl(_tmp1D9);if(_tmp24E->impl != 0  && ((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24E->impl))->exist_vars != 0)({void*
_tmp24F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp250="gen(): existential type variables not yet supported";
_tag_dyneither(_tmp250,sizeof(char),52);}),_tag_dyneither(_tmp24F,sizeof(void*),
0));});if(_tmp24E->kind == Cyc_Absyn_StructA){struct Cyc_Absyn_Aggrdecl*_tmp251=
_tmp24E;if(_tmp251->impl == 0){struct _tuple5*_tmp252=Cyc_Tcgenrep_toplevel_name(({
struct _dyneither_ptr*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp255;}));struct Cyc_Core_Opt*_tmp253=({struct Cyc_Core_Opt*_tmp254=
_cycalloc(sizeof(*_tmp254));_tmp254->v=Cyc_Tcgenrep_gen_vardecl(_tmp252,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,Cyc_Absyn_Extern,loc);_tmp254;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp252,loc),loc),_tmp253,1);goto _LL12;}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp251->impl))->fields != 0  && ((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp251->impl))->fields)){int _tmp256=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),
int accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,
0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp251->impl))->fields);int
_tmp257=_tmp256 / 8 + (_tmp256 % 8 == 0?0: 1);struct Cyc_List_List*chars=0;{int i=0;
for(0;i < _tmp257;++ i){chars=({struct Cyc_List_List*_tmp258=_cycalloc(sizeof(*
_tmp258));_tmp258->hd=({struct _tuple15*_tmp259=_cycalloc(sizeof(*_tmp259));
_tmp259->f1=Cyc_Tcgenrep_tq_none;_tmp259->f2=Cyc_Absyn_char_typ;_tmp259;});
_tmp258->tl=chars;_tmp258;});}}{void*base_type=(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E[0]=({struct Cyc_Absyn_TupleType_struct
_tmp25F;_tmp25F.tag=11;_tmp25F.f1=chars;_tmp25F;});_tmp25E;});struct _tuple24
_tmp25B;struct Cyc_Dict_Dict _tmp25C;struct Cyc_Tcgenrep_RepInfo*_tmp25D;struct
_tuple24*_tmp25A=Cyc_Tcgenrep_lookupRep(te,dict,loc,base_type);_tmp25B=*_tmp25A;
_tmp25C=_tmp25B.f1;_tmp25D=_tmp25B.f2;dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,void*k,struct Cyc_Tcgenrep_RepInfo*v))Cyc_Dict_insert)(_tmp25C,type,_tmp25D);};}
else{struct _tuple5*_tmp260=Cyc_Tcgenrep_toplevel_name(({struct _dyneither_ptr*
_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp265;}));struct Cyc_Core_Opt*_tmp261=({struct Cyc_Core_Opt*_tmp264=
_cycalloc(sizeof(*_tmp264));_tmp264->v=Cyc_Tcgenrep_gen_vardecl(_tmp260,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,Cyc_Absyn_Extern,loc);_tmp264;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp260,loc),loc),_tmp261,0);{struct Cyc_List_List*_tmp262=((struct Cyc_List_List*(*)(
struct _tuple21*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp251->impl))->fields);dict=Cyc_Tcgenrep_buildRepStruct(({struct
Cyc_Core_Opt*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->v=_tmp251->name;
_tmp263;}),te,dict,loc,_tmp260,Cyc_Absyn_Public,type,_tmp262);};}}else{struct Cyc_Absyn_Aggrdecl*
_tmp266=_tmp24E;if(_tmp266->tvs != 0)({void*_tmp267=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp268="gen: unions with parameters not yet supported";
_tag_dyneither(_tmp268,sizeof(char),46);}),_tag_dyneither(_tmp267,sizeof(void*),
0));});if(_tmp266->impl == 0){struct _tuple5*_tmp269=Cyc_Tcgenrep_toplevel_name(({
struct _dyneither_ptr*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp26C;}));struct Cyc_Core_Opt*_tmp26A=({struct Cyc_Core_Opt*_tmp26B=
_cycalloc(sizeof(*_tmp26B));_tmp26B->v=Cyc_Tcgenrep_gen_vardecl(_tmp269,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_union_str)),0,Cyc_Absyn_Extern,loc);_tmp26B;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp269,loc),loc),_tmp26A,1);goto _LL12;}{struct Cyc_List_List*_tmp26D=((struct Cyc_List_List*(*)(
struct _tuple21*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp266->impl))->fields);struct _tuple5*_tmp26E=Cyc_Tcgenrep_toplevel_name(({
struct _dyneither_ptr*_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270[0]=Cyc_Tcgenrep_make_type_cstring(
type);_tmp270;}));dict=Cyc_Tcgenrep_buildRepUnion(({struct Cyc_Core_Opt*_tmp26F=
_cycalloc(sizeof(*_tmp26F));_tmp26F->v=_tmp266->name;_tmp26F;}),te,dict,loc,
_tmp26E,Cyc_Absyn_Public,type,_tmp26D);};}goto _LL12;}_LL3F: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp1DB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp1AB;if(_tmp1DB->tag != 13)goto
_LL41;else{_tmp1DC=_tmp1DB->f1;if(_tmp1DC != Cyc_Absyn_StructA)goto _LL41;_tmp1DD=
_tmp1DB->f2;}}_LL40: if(_tmp1DD != 0  && ((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,_tmp1DD)){int
_tmp271=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,_tmp1DD);int _tmp272=
_tmp271 / 8 + (_tmp271 % 8 == 0?0: 1);void*base_type=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Tcgenrep_tq_none,(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_int(_tmp272,loc),
Cyc_Absyn_true_conref,0);struct _tuple24 _tmp274;struct Cyc_Dict_Dict _tmp275;struct
Cyc_Tcgenrep_RepInfo*_tmp276;struct _tuple24*_tmp273=Cyc_Tcgenrep_lookupRep(te,
dict,loc,base_type);_tmp274=*_tmp273;_tmp275=_tmp274.f1;_tmp276=_tmp274.f2;dict=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Tcgenrep_RepInfo*v))
Cyc_Dict_insert)(_tmp275,type,_tmp276);}else{struct Cyc_List_List*_tmp277=((
struct Cyc_List_List*(*)(struct _tuple21*(*f)(struct Cyc_Absyn_Aggrfield*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,_tmp1DD);
struct _tuple5*_tmp278=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(({const
char*_tmp279="rep";_tag_dyneither(_tmp279,sizeof(char),4);})));dict=Cyc_Tcgenrep_buildRepStruct(
0,te,dict,loc,_tmp278,Cyc_Absyn_Static,type,_tmp277);}goto _LL12;_LL41: {struct
Cyc_Absyn_AnonAggrType_struct*_tmp1DE=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1AB;if(_tmp1DE->tag != 13)goto _LL43;else{_tmp1DF=_tmp1DE->f1;if(_tmp1DF != Cyc_Absyn_UnionA)
goto _LL43;_tmp1E0=_tmp1DE->f2;}}_LL42: {struct Cyc_List_List*_tmp27A=((struct Cyc_List_List*(*)(
struct _tuple21*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,_tmp1E0);struct _tuple5*_tmp27B=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp27C="rep";_tag_dyneither(_tmp27C,sizeof(
char),4);})));dict=Cyc_Tcgenrep_buildRepUnion(0,te,dict,loc,_tmp27B,Cyc_Absyn_Static,
type,_tmp27A);goto _LL12;}_LL43: {struct Cyc_Absyn_EnumType_struct*_tmp1E1=(struct
Cyc_Absyn_EnumType_struct*)_tmp1AB;if(_tmp1E1->tag != 14)goto _LL45;else{_tmp1E2=
_tmp1E1->f2;}}_LL44: if(_tmp1E2 == 0)({void*_tmp27D=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp27E="gen(): enum declaration must be present by now";
_tag_dyneither(_tmp27E,sizeof(char),47);}),_tag_dyneither(_tmp27D,sizeof(void*),
0));});{struct Cyc_Absyn_Enumdecl _tmp27F=*_tmp1E2;if(_tmp27F.fields == 0){struct
_tuple5*_tmp280=Cyc_Tcgenrep_toplevel_name(({struct _dyneither_ptr*_tmp283=
_cycalloc(sizeof(*_tmp283));_tmp283[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp283;}));struct Cyc_Core_Opt*_tmp281=({struct Cyc_Core_Opt*_tmp282=_cycalloc(
sizeof(*_tmp282));_tmp282->v=Cyc_Tcgenrep_gen_vardecl(_tmp280,Cyc_Tcgenrep_datatypefield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_enum_str)),0,Cyc_Absyn_Extern,loc);_tmp282;});dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp280,loc),loc),_tmp281,1);goto _LL12;}{struct Cyc_List_List*_tmp284=((struct Cyc_List_List*(*)(
struct _tuple22*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,(struct Cyc_List_List*)(_tmp27F.fields)->v);
struct _tuple5*_tmp285=Cyc_Tcgenrep_toplevel_name(({struct _dyneither_ptr*_tmp286=
_cycalloc(sizeof(*_tmp286));_tmp286[0]=Cyc_Tcgenrep_make_type_cstring(type);
_tmp286;}));dict=Cyc_Tcgenrep_buildRepEnum((struct _tuple5**)& _tmp27F.name,te,
dict,loc,_tmp285,Cyc_Absyn_Public,type,_tmp284);goto _LL12;};};_LL45: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp1E3=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp1AB;if(_tmp1E3->tag != 15)goto
_LL47;else{_tmp1E4=_tmp1E3->f1;}}_LL46: {struct Cyc_List_List*_tmp287=((struct Cyc_List_List*(*)(
struct _tuple22*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,_tmp1E4);struct _tuple5*_tmp288=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(({const char*_tmp289="rep";_tag_dyneither(_tmp289,sizeof(
char),4);})));dict=Cyc_Tcgenrep_buildRepEnum(0,te,dict,loc,_tmp288,Cyc_Absyn_Static,
type,_tmp287);goto _LL12;}_LL47: {struct Cyc_Absyn_TagType_struct*_tmp1E5=(struct
Cyc_Absyn_TagType_struct*)_tmp1AB;if(_tmp1E5->tag != 20)goto _LL49;}_LL48:({void*
_tmp28A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp28B="gen() for tag_t<-> not yet supported";_tag_dyneither(_tmp28B,
sizeof(char),37);}),_tag_dyneither(_tmp28A,sizeof(void*),0));});_LL49: {struct
Cyc_Absyn_ValueofType_struct*_tmp1E6=(struct Cyc_Absyn_ValueofType_struct*)
_tmp1AB;if(_tmp1E6->tag != 19)goto _LL12;}_LL4A:({void*_tmp28C=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp28D="gen() for valueof_t<-> not yet supported";
_tag_dyneither(_tmp28D,sizeof(char),41);}),_tag_dyneither(_tmp28C,sizeof(void*),
0));});_LL12:;}return({struct _tuple24*_tmp28E=_cycalloc(sizeof(*_tmp28E));
_tmp28E->f1=dict;_tmp28E->f2=((struct Cyc_Tcgenrep_RepInfo*(*)(struct Cyc_Dict_Dict
d,void*k))Cyc_Dict_lookup)(dict,type);_tmp28E;});}static int Cyc_Tcgenrep_not_emitted_filter(
struct Cyc_Tcgenrep_RepInfo*ri){return ri->emitted == 0;}static void Cyc_Tcgenrep_mark_emitted(
struct Cyc_Tcgenrep_RepInfo*ri){ri->emitted=1;}struct _tuple27{struct Cyc_Dict_Dict
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple27 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
void*type,struct Cyc_Dict_Dict dict){Cyc_Tcutil_check_valid_toplevel_type(loc,te,
type);Cyc_Tcutil_check_valid_toplevel_type(loc,te,type);{struct _tuple24 _tmp290;
struct Cyc_Dict_Dict _tmp291;struct Cyc_Tcgenrep_RepInfo*_tmp292;struct _tuple24*
_tmp28F=Cyc_Tcgenrep_lookupRep(te,dict,loc,type);_tmp290=*_tmp28F;_tmp291=
_tmp290.f1;_tmp292=_tmp290.f2;{struct Cyc_List_List*_tmp293=Cyc_Tcgenrep_dfs(
_tmp292);return({struct _tuple27 _tmp294;_tmp294.f1=_tmp291;_tmp294.f2=_tmp293;
_tmp294.f3=(struct Cyc_Absyn_Exp*)_check_null(_tmp292->exp);_tmp294;});};};}
