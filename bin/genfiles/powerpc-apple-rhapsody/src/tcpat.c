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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct
_RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
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
char*tag;};struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct
_RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(struct Cyc_Set_Set*
s);int Cyc_Set_is_empty(struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*
s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};
void*Cyc_Set_choose(struct Cyc_Set_Set*s);int Cyc_strcmp(struct _dyneither_ptr s1,
struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;}
;struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};struct _union_Nmspace_Rel_n{int tag;
struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
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
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
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
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_VoidType_struct
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
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
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
Cyc_Absyn_empty_conref();void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_false_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(enum 
Cyc_Absyn_Kind);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_Tcutil_kind_leq(enum Cyc_Absyn_Kind
k1,enum Cyc_Absyn_Kind k2);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(
void*);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int
Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);struct _tuple10{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};struct
_tuple11*Cyc_Tcutil_r_make_inst_var(struct _tuple10*,struct Cyc_Absyn_Tvar*);void
Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr err_msg);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;
};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr msg_part);struct _tuple12{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple12*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct
_tuple13{unsigned int f1;int f2;};struct _tuple13 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct
Cyc_Absyn_Pat*p){void*_tmp0=p->r;struct _tuple1*_tmp2;struct _tuple1*_tmp4;struct
Cyc_List_List*_tmp5;int _tmp6;struct Cyc_Absyn_AggrInfo*_tmp8;struct Cyc_List_List*
_tmp9;struct Cyc_List_List*_tmpA;int _tmpB;struct Cyc_Absyn_AggrInfo*_tmpD;struct
Cyc_Absyn_AggrInfo _tmpE;union Cyc_Absyn_AggrInfoU _tmpF;struct _tuple3 _tmp10;enum 
Cyc_Absyn_AggrKind _tmp11;struct _tuple1*_tmp12;struct Cyc_List_List*_tmp13;struct
Cyc_List_List*_tmp14;struct Cyc_List_List**_tmp15;struct Cyc_List_List*_tmp16;int
_tmp17;struct Cyc_Absyn_Exp*_tmp19;_LL1: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp1=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp0;if(_tmp1->tag != 14)goto _LL3;
else{_tmp2=_tmp1->f1;}}_LL2:{struct _handler_cons _tmp1A;_push_handler(& _tmp1A);{
int _tmp1C=0;if(setjmp(_tmp1A.handler))_tmp1C=1;if(!_tmp1C){{struct _RegionHandle*
_tmp1D=Cyc_Tcenv_get_fnrgn(te);void*_tmp1E=Cyc_Tcenv_lookup_ordinary(_tmp1D,te,p->loc,
_tmp2);struct Cyc_Absyn_Datatypedecl*_tmp21;struct Cyc_Absyn_Datatypefield*_tmp22;
struct Cyc_Absyn_Enumdecl*_tmp24;struct Cyc_Absyn_Enumfield*_tmp25;void*_tmp27;
struct Cyc_Absyn_Enumfield*_tmp28;_LLE: {struct Cyc_Tcenv_AggrRes_struct*_tmp1F=(
struct Cyc_Tcenv_AggrRes_struct*)_tmp1E;if(_tmp1F->tag != 1)goto _LL10;}_LLF: p->r=(
void*)& Cyc_Absyn_Wild_p_val;({void*_tmp2A=0;Cyc_Tcutil_terr(p->loc,({const char*
_tmp2B="struct tag used without arguments in pattern";_tag_dyneither(_tmp2B,
sizeof(char),45);}),_tag_dyneither(_tmp2A,sizeof(void*),0));});_npop_handler(0);
return;_LL10: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp20=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp1E;if(_tmp20->tag != 2)goto _LL12;else{_tmp21=_tmp20->f1;_tmp22=_tmp20->f2;}}
_LL11: p->r=(void*)({struct Cyc_Absyn_Datatype_p_struct*_tmp2C=_cycalloc(sizeof(*
_tmp2C));_tmp2C[0]=({struct Cyc_Absyn_Datatype_p_struct _tmp2D;_tmp2D.tag=7;_tmp2D.f1=
_tmp21;_tmp2D.f2=_tmp22;_tmp2D.f3=0;_tmp2D.f4=0;_tmp2D;});_tmp2C;});
_npop_handler(0);return;_LL12: {struct Cyc_Tcenv_EnumRes_struct*_tmp23=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp1E;if(_tmp23->tag != 3)goto _LL14;else{_tmp24=_tmp23->f1;_tmp25=_tmp23->f2;}}
_LL13: p->r=(void*)({struct Cyc_Absyn_Enum_p_struct*_tmp2E=_cycalloc(sizeof(*
_tmp2E));_tmp2E[0]=({struct Cyc_Absyn_Enum_p_struct _tmp2F;_tmp2F.tag=12;_tmp2F.f1=
_tmp24;_tmp2F.f2=_tmp25;_tmp2F;});_tmp2E;});_npop_handler(0);return;_LL14: {
struct Cyc_Tcenv_AnonEnumRes_struct*_tmp26=(struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp1E;if(_tmp26->tag != 4)goto _LL16;else{_tmp27=(void*)_tmp26->f1;_tmp28=_tmp26->f2;}}
_LL15: p->r=(void*)({struct Cyc_Absyn_AnonEnum_p_struct*_tmp30=_cycalloc(sizeof(*
_tmp30));_tmp30[0]=({struct Cyc_Absyn_AnonEnum_p_struct _tmp31;_tmp31.tag=13;
_tmp31.f1=(void*)_tmp27;_tmp31.f2=_tmp28;_tmp31;});_tmp30;});_npop_handler(0);
return;_LL16: {struct Cyc_Tcenv_VarRes_struct*_tmp29=(struct Cyc_Tcenv_VarRes_struct*)
_tmp1E;if(_tmp29->tag != 0)goto _LLD;}_LL17: goto _LLD;_LLD:;};_pop_handler();}else{
void*_tmp1B=(void*)_exn_thrown;void*_tmp33=_tmp1B;_LL19: {struct Cyc_Dict_Absent_struct*
_tmp34=(struct Cyc_Dict_Absent_struct*)_tmp33;if(_tmp34->tag != Cyc_Dict_Absent)
goto _LL1B;}_LL1A: goto _LL18;_LL1B:;_LL1C:(void)_throw(_tmp33);_LL18:;}};}{union
Cyc_Absyn_Nmspace _tmp35=(*_tmp2).f1;int _tmp36;struct Cyc_List_List*_tmp37;_LL1E:
if((_tmp35.Loc_n).tag != 3)goto _LL20;_tmp36=(int)(_tmp35.Loc_n).val;_LL1F: goto
_LL21;_LL20: if((_tmp35.Rel_n).tag != 1)goto _LL22;_tmp37=(struct Cyc_List_List*)(
_tmp35.Rel_n).val;if(_tmp37 != 0)goto _LL22;_LL21:(*_tmp2).f1=Cyc_Absyn_Loc_n;p->r=(
void*)({struct Cyc_Absyn_Var_p_struct*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38[0]=({
struct Cyc_Absyn_Var_p_struct _tmp39;_tmp39.tag=1;_tmp39.f1=Cyc_Absyn_new_vardecl(
_tmp2,(void*)& Cyc_Absyn_VoidType_val,0);_tmp39.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
0);_tmp39;});_tmp38;});return;_LL22:;_LL23:({void*_tmp3A=0;Cyc_Tcutil_terr(p->loc,({
const char*_tmp3B="qualified variable in pattern";_tag_dyneither(_tmp3B,sizeof(
char),30);}),_tag_dyneither(_tmp3A,sizeof(void*),0));});return;_LL1D:;};_LL3: {
struct Cyc_Absyn_UnknownCall_p_struct*_tmp3=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0;if(_tmp3->tag != 15)goto _LL5;else{_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;_tmp6=
_tmp3->f3;}}_LL4:{struct _handler_cons _tmp3C;_push_handler(& _tmp3C);{int _tmp3E=0;
if(setjmp(_tmp3C.handler))_tmp3E=1;if(!_tmp3E){{struct _RegionHandle*_tmp3F=Cyc_Tcenv_get_fnrgn(
te);void*_tmp40=Cyc_Tcenv_lookup_ordinary(_tmp3F,te,p->loc,_tmp4);struct Cyc_Absyn_Aggrdecl*
_tmp42;struct Cyc_Absyn_Datatypedecl*_tmp44;struct Cyc_Absyn_Datatypefield*_tmp45;
_LL25: {struct Cyc_Tcenv_AggrRes_struct*_tmp41=(struct Cyc_Tcenv_AggrRes_struct*)
_tmp40;if(_tmp41->tag != 1)goto _LL27;else{_tmp42=_tmp41->f1;}}_LL26: {struct Cyc_List_List*
_tmp49=0;for(0;_tmp5 != 0;_tmp5=_tmp5->tl){_tmp49=({struct Cyc_List_List*_tmp4A=
_cycalloc(sizeof(*_tmp4A));_tmp4A->hd=({struct _tuple14*_tmp4B=_cycalloc(sizeof(*
_tmp4B));_tmp4B->f1=0;_tmp4B->f2=(struct Cyc_Absyn_Pat*)_tmp5->hd;_tmp4B;});
_tmp4A->tl=_tmp49;_tmp4A;});}p->r=(void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp4C=
_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp4D;
_tmp4D.tag=6;_tmp4D.f1=({struct Cyc_Absyn_AggrInfo*_tmp4E=_cycalloc(sizeof(*
_tmp4E));_tmp4E->aggr_info=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**
_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=_tmp42;_tmp4F;}));_tmp4E->targs=0;
_tmp4E;});_tmp4D.f2=0;_tmp4D.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp49);_tmp4D.f4=_tmp6;_tmp4D;});_tmp4C;});_npop_handler(0);
return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_struct*_tmp43=(struct Cyc_Tcenv_DatatypeRes_struct*)
_tmp40;if(_tmp43->tag != 2)goto _LL29;else{_tmp44=_tmp43->f1;_tmp45=_tmp43->f2;}}
_LL28: p->r=(void*)({struct Cyc_Absyn_Datatype_p_struct*_tmp50=_cycalloc(sizeof(*
_tmp50));_tmp50[0]=({struct Cyc_Absyn_Datatype_p_struct _tmp51;_tmp51.tag=7;_tmp51.f1=
_tmp44;_tmp51.f2=_tmp45;_tmp51.f3=_tmp5;_tmp51.f4=_tmp6;_tmp51;});_tmp50;});
_npop_handler(0);return;_LL29: {struct Cyc_Tcenv_EnumRes_struct*_tmp46=(struct Cyc_Tcenv_EnumRes_struct*)
_tmp40;if(_tmp46->tag != 3)goto _LL2B;}_LL2A: goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp47=(struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp40;if(_tmp47->tag != 4)goto _LL2D;}
_LL2C:({void*_tmp52=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp53="enum tag used with arguments in pattern";
_tag_dyneither(_tmp53,sizeof(char),40);}),_tag_dyneither(_tmp52,sizeof(void*),0));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;_npop_handler(0);return;_LL2D: {struct Cyc_Tcenv_VarRes_struct*
_tmp48=(struct Cyc_Tcenv_VarRes_struct*)_tmp40;if(_tmp48->tag != 0)goto _LL24;}
_LL2E: goto _LL24;_LL24:;};_pop_handler();}else{void*_tmp3D=(void*)_exn_thrown;
void*_tmp55=_tmp3D;_LL30: {struct Cyc_Dict_Absent_struct*_tmp56=(struct Cyc_Dict_Absent_struct*)
_tmp55;if(_tmp56->tag != Cyc_Dict_Absent)goto _LL32;}_LL31: goto _LL2F;_LL32:;_LL33:(
void)_throw(_tmp55);_LL2F:;}};}({struct Cyc_String_pa_struct _tmp59;_tmp59.tag=0;
_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp4));({void*_tmp57[1]={& _tmp59};Cyc_Tcutil_terr(p->loc,({const char*_tmp58="%s is not a constructor in pattern";
_tag_dyneither(_tmp58,sizeof(char),35);}),_tag_dyneither(_tmp57,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;return;_LL5: {struct Cyc_Absyn_Aggr_p_struct*
_tmp7=(struct Cyc_Absyn_Aggr_p_struct*)_tmp0;if(_tmp7->tag != 6)goto _LL7;else{
_tmp8=_tmp7->f1;if(_tmp8 != 0)goto _LL7;_tmp9=_tmp7->f2;_tmpA=_tmp7->f3;_tmpB=
_tmp7->f4;}}_LL6: if(topt == 0)({void*_tmp5A=0;Cyc_Tcutil_terr(p->loc,({const char*
_tmp5B="cannot determine pattern type";_tag_dyneither(_tmp5B,sizeof(char),30);}),
_tag_dyneither(_tmp5A,sizeof(void*),0));});{void*t=Cyc_Tcutil_compress(*((void**)
_check_null(topt)));{void*_tmp5C=t;struct Cyc_Absyn_AggrInfo _tmp5E;_LL35: {struct
Cyc_Absyn_AggrType_struct*_tmp5D=(struct Cyc_Absyn_AggrType_struct*)_tmp5C;if(
_tmp5D->tag != 12)goto _LL37;else{_tmp5E=_tmp5D->f1;}}_LL36: p->r=(void*)({struct
Cyc_Absyn_Aggr_p_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F[0]=({struct Cyc_Absyn_Aggr_p_struct
_tmp60;_tmp60.tag=6;_tmp60.f1=({struct Cyc_Absyn_AggrInfo*_tmp61=_cycalloc(
sizeof(*_tmp61));_tmp61[0]=_tmp5E;_tmp61;});_tmp60.f2=_tmp9;_tmp60.f3=_tmpA;
_tmp60.f4=_tmpB;_tmp60;});_tmp5F;});Cyc_Tcpat_resolve_pat(te,topt,p);goto _LL34;
_LL37:;_LL38:({struct Cyc_String_pa_struct _tmp64;_tmp64.tag=0;_tmp64.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp62[1]={&
_tmp64};Cyc_Tcutil_terr(p->loc,({const char*_tmp63="pattern expects aggregate type instead of %s";
_tag_dyneither(_tmp63,sizeof(char),45);}),_tag_dyneither(_tmp62,sizeof(void*),1));});});
goto _LL34;_LL34:;}return;};_LL7: {struct Cyc_Absyn_Aggr_p_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp0;if(_tmpC->tag != 6)goto _LL9;else{_tmpD=_tmpC->f1;if(_tmpD == 0)goto _LL9;
_tmpE=*_tmpD;_tmpF=_tmpE.aggr_info;if((_tmpF.UnknownAggr).tag != 1)goto _LL9;
_tmp10=(struct _tuple3)(_tmpF.UnknownAggr).val;_tmp11=_tmp10.f1;_tmp12=_tmp10.f2;
_tmp13=_tmpE.targs;_tmp14=_tmpC->f2;_tmp15=(struct Cyc_List_List**)& _tmpC->f2;
_tmp16=_tmpC->f3;_tmp17=_tmpC->f4;}}_LL8:{struct _handler_cons _tmp65;
_push_handler(& _tmp65);{int _tmp67=0;if(setjmp(_tmp65.handler))_tmp67=1;if(!
_tmp67){{struct Cyc_Absyn_Aggrdecl**_tmp68=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,
_tmp12);struct Cyc_Absyn_Aggrdecl*_tmp69=*_tmp68;if(_tmp69->impl == 0){({struct Cyc_String_pa_struct
_tmp6C;_tmp6C.tag=0;_tmp6C.f1=(struct _dyneither_ptr)(_tmp69->kind == Cyc_Absyn_StructA?({
const char*_tmp6D="struct";_tag_dyneither(_tmp6D,sizeof(char),7);}):({const char*
_tmp6E="union";_tag_dyneither(_tmp6E,sizeof(char),6);}));({void*_tmp6A[1]={&
_tmp6C};Cyc_Tcutil_terr(p->loc,({const char*_tmp6B="can't destructure an abstract %s";
_tag_dyneither(_tmp6B,sizeof(char),33);}),_tag_dyneither(_tmp6A,sizeof(void*),1));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;_npop_handler(0);return;}{int more_exists=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp69->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*
_tmp15);if(more_exists < 0){({void*_tmp6F=0;Cyc_Tcutil_terr(p->loc,({const char*
_tmp70="too many existentially bound type variables in pattern";_tag_dyneither(
_tmp70,sizeof(char),55);}),_tag_dyneither(_tmp6F,sizeof(void*),0));});{struct Cyc_List_List**
_tmp71=_tmp15;{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp69->impl))->exist_vars);for(0;n != 0;-- n){_tmp71=&((struct Cyc_List_List*)
_check_null(*_tmp71))->tl;}}*_tmp71=0;};}else{if(more_exists > 0){struct Cyc_List_List*
_tmp72=0;for(0;more_exists != 0;-- more_exists){_tmp72=({struct Cyc_List_List*
_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->hd=Cyc_Tcutil_new_tvar((void*)({struct
Cyc_Absyn_Unknown_kb_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct
Cyc_Absyn_Unknown_kb_struct _tmp75;_tmp75.tag=1;_tmp75.f1=0;_tmp75;});_tmp74;}));
_tmp73->tl=_tmp72;_tmp73;});}*_tmp15=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp15,_tmp72);}}p->r=(void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp77;_tmp77.tag=6;_tmp77.f1=({struct Cyc_Absyn_AggrInfo*
_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->aggr_info=Cyc_Absyn_KnownAggr(_tmp68);
_tmp78->targs=_tmp13;_tmp78;});_tmp77.f2=*_tmp15;_tmp77.f3=_tmp16;_tmp77.f4=
_tmp17;_tmp77;});_tmp76;});};};_pop_handler();}else{void*_tmp66=(void*)
_exn_thrown;void*_tmp7A=_tmp66;_LL3A: {struct Cyc_Dict_Absent_struct*_tmp7B=(
struct Cyc_Dict_Absent_struct*)_tmp7A;if(_tmp7B->tag != Cyc_Dict_Absent)goto _LL3C;}
_LL3B:({void*_tmp7C=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp7D="Non-aggregate name has designator patterns";
_tag_dyneither(_tmp7D,sizeof(char),43);}),_tag_dyneither(_tmp7C,sizeof(void*),0));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;goto _LL39;_LL3C:;_LL3D:(void)_throw(_tmp7A);
_LL39:;}};}return;_LL9: {struct Cyc_Absyn_Exp_p_struct*_tmp18=(struct Cyc_Absyn_Exp_p_struct*)
_tmp0;if(_tmp18->tag != 16)goto _LLB;else{_tmp19=_tmp18->f1;}}_LLA: Cyc_Tcexp_tcExp(
te,0,_tmp19);if(!Cyc_Tcutil_is_const_exp(te,_tmp19)){({void*_tmp7E=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmp7F="non-constant expression in case pattern";
_tag_dyneither(_tmp7F,sizeof(char),40);}),_tag_dyneither(_tmp7E,sizeof(void*),0));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{unsigned int _tmp81;int _tmp82;struct _tuple13
_tmp80=Cyc_Evexp_eval_const_uint_exp(_tmp19);_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;p->r=(
void*)({struct Cyc_Absyn_Int_p_struct*_tmp83=_cycalloc_atomic(sizeof(*_tmp83));
_tmp83[0]=({struct Cyc_Absyn_Int_p_struct _tmp84;_tmp84.tag=9;_tmp84.f1=Cyc_Absyn_None;
_tmp84.f2=(int)_tmp81;_tmp84;});_tmp83;});return;};_LLB:;_LLC: return;_LL0:;}
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){return(*
vd->name).f2;}static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)return*topt;return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->v=s;_tmp85;}));}
static void*Cyc_Tcpat_num_type(void**topt,void*numt){if(topt != 0  && Cyc_Tcutil_coerceable(*
topt))return*topt;{void*_tmp86=Cyc_Tcutil_compress(numt);_LL3F: {struct Cyc_Absyn_EnumType_struct*
_tmp87=(struct Cyc_Absyn_EnumType_struct*)_tmp86;if(_tmp87->tag != 14)goto _LL41;}
_LL40: if(topt != 0)return*topt;goto _LL3E;_LL41:;_LL42: goto _LL3E;_LL3E:;}return
numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){vd->type=t;vd->tq=Cyc_Absyn_empty_tqual(0);*v_result_ptr=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp88=_cycalloc(sizeof(*_tmp88));
_tmp88->hd=vd;_tmp88->tl=*v_result_ptr;_tmp88;});}static struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult
res2){struct _tuple12*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Tcpat_TcPatResult
_tmp89=res1;_tmp8A=_tmp89.tvars_and_bounds_opt;_tmp8B=_tmp89.patvars;{struct
_tuple12*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Tcpat_TcPatResult _tmp8C=
res2;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=_tmp8C.patvars;if(_tmp8A != 0  || 
_tmp8D != 0){if(_tmp8A == 0)_tmp8A=({struct _tuple12*_tmp8F=_cycalloc(sizeof(*
_tmp8F));_tmp8F->f1=0;_tmp8F->f2=0;_tmp8F;});if(_tmp8D == 0)_tmp8D=({struct
_tuple12*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->f1=0;_tmp90->f2=0;_tmp90;});
return({struct Cyc_Tcpat_TcPatResult _tmp91;_tmp91.tvars_and_bounds_opt=({struct
_tuple12*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple12*)
_check_null(_tmp8A))).f1,(*((struct _tuple12*)_check_null(_tmp8D))).f1);_tmp92->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple12*)_check_null(_tmp8A))).f2,(*((struct _tuple12*)_check_null(_tmp8D))).f2);
_tmp92;});_tmp91.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp8B,_tmp8E);_tmp91;});}return({struct Cyc_Tcpat_TcPatResult
_tmp93;_tmp93.tvars_and_bounds_opt=0;_tmp93.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8B,_tmp8E);
_tmp93;});};}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Pat*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->loc=
loc;_tmp94->topt=0;_tmp94->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp94;});}struct
_tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple16{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(
te,topt,p);{void*t;struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult
_tmp17D;_tmp17D.tvars_and_bounds_opt=0;_tmp17D.patvars=0;_tmp17D;});{void*_tmp95=
p->r;struct Cyc_Absyn_Vardecl*_tmp98;struct Cyc_Absyn_Pat*_tmp99;struct Cyc_Absyn_Vardecl*
_tmp9B;struct Cyc_Absyn_Pat*_tmp9C;struct Cyc_Absyn_Tvar*_tmp9E;struct Cyc_Absyn_Vardecl*
_tmp9F;enum Cyc_Absyn_Sign _tmpA1;enum Cyc_Absyn_Sign _tmpA3;enum Cyc_Absyn_Sign
_tmpA5;struct Cyc_Absyn_Enumdecl*_tmpA9;void*_tmpAB;struct Cyc_Absyn_Pat*_tmpAE;
struct Cyc_List_List*_tmpB0;struct Cyc_List_List**_tmpB1;int _tmpB2;struct Cyc_Absyn_AggrInfo*
_tmpB4;struct Cyc_Absyn_AggrInfo _tmpB5;union Cyc_Absyn_AggrInfoU _tmpB6;struct Cyc_Absyn_Aggrdecl**
_tmpB7;struct Cyc_Absyn_Aggrdecl*_tmpB8;struct Cyc_List_List*_tmpB9;struct Cyc_List_List**
_tmpBA;struct Cyc_List_List*_tmpBB;struct Cyc_List_List*_tmpBC;struct Cyc_List_List**
_tmpBD;int _tmpBE;struct Cyc_Absyn_Datatypedecl*_tmpC0;struct Cyc_Absyn_Datatypefield*
_tmpC1;struct Cyc_List_List*_tmpC2;struct Cyc_List_List**_tmpC3;int _tmpC4;struct
Cyc_Absyn_AggrInfo*_tmpC6;struct Cyc_Absyn_AggrInfo*_tmpC8;struct Cyc_Absyn_AggrInfo
_tmpC9;union Cyc_Absyn_AggrInfoU _tmpCA;struct _tuple3 _tmpCB;_LL44: {struct Cyc_Absyn_Wild_p_struct*
_tmp96=(struct Cyc_Absyn_Wild_p_struct*)_tmp95;if(_tmp96->tag != 0)goto _LL46;}
_LL45: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);goto _LL43;_LL46: {
struct Cyc_Absyn_Var_p_struct*_tmp97=(struct Cyc_Absyn_Var_p_struct*)_tmp95;if(
_tmp97->tag != 1)goto _LL48;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL47: res=
Cyc_Tcpat_tcPatRec(te,_tmp99,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp99->topt))->v;Cyc_Tcpat_set_vd(_tmp98,& res.patvars,t);goto _LL43;
_LL48: {struct Cyc_Absyn_Reference_p_struct*_tmp9A=(struct Cyc_Absyn_Reference_p_struct*)
_tmp95;if(_tmp9A->tag != 2)goto _LL4A;else{_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;}}
_LL49: res=Cyc_Tcpat_tcPatRec(te,_tmp9C,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9C->topt))->v;if(rgn_opt == 0){({void*_tmpCF=0;Cyc_Tcutil_terr(p->loc,({
const char*_tmpD0="* pattern would point into an unknown/unallowed region";
_tag_dyneither(_tmpD0,sizeof(char),55);}),_tag_dyneither(_tmpCF,sizeof(void*),0));});
goto _LL43;}else{if(Cyc_Tcutil_is_noalias_pointer(t))({void*_tmpD1=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmpD2="* pattern cannot take the address of an alias-free path";_tag_dyneither(
_tmpD2,sizeof(char),56);}),_tag_dyneither(_tmpD1,sizeof(void*),0));});}Cyc_Tcpat_set_vd(
_tmp9B,& res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmpD3=_cycalloc(
sizeof(*_tmpD3));_tmpD3[0]=({struct Cyc_Absyn_PointerType_struct _tmpD4;_tmpD4.tag=
5;_tmpD4.f1=({struct Cyc_Absyn_PtrInfo _tmpD5;_tmpD5.elt_typ=t;_tmpD5.elt_tq=Cyc_Absyn_empty_tqual(
0);_tmpD5.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpD6;_tmpD6.rgn=*rgn_opt;_tmpD6.nullable=
Cyc_Absyn_false_conref;_tmpD6.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmpD6.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmpD6.ptrloc=0;_tmpD6;});_tmpD5;});_tmpD4;});_tmpD3;}));goto _LL43;_LL4A: {
struct Cyc_Absyn_TagInt_p_struct*_tmp9D=(struct Cyc_Absyn_TagInt_p_struct*)_tmp95;
if(_tmp9D->tag != 3)goto _LL4C;else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}_LL4B:
Cyc_Tcpat_set_vd(_tmp9F,& res.patvars,_tmp9F->type);{struct _RegionHandle*_tmpD7=
Cyc_Tcenv_get_fnrgn(te);Cyc_Tcenv_add_type_vars(_tmpD7,p->loc,te,({struct Cyc_Absyn_Tvar*
_tmpD8[1];_tmpD8[0]=_tmp9E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD8,sizeof(struct Cyc_Absyn_Tvar*),1));}));if(res.tvars_and_bounds_opt
== 0)res.tvars_and_bounds_opt=({struct _tuple12*_tmpD9=_cycalloc(sizeof(*_tmpD9));
_tmpD9->f1=0;_tmpD9->f2=0;_tmpD9;});(*res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*
_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->hd=_tmp9E;_tmpDA->tl=(*res.tvars_and_bounds_opt).f1;
_tmpDA;});t=Cyc_Absyn_uint_typ;goto _LL43;};_LL4C: {struct Cyc_Absyn_Int_p_struct*
_tmpA0=(struct Cyc_Absyn_Int_p_struct*)_tmp95;if(_tmpA0->tag != 9)goto _LL4E;else{
_tmpA1=_tmpA0->f1;if(_tmpA1 != Cyc_Absyn_Unsigned)goto _LL4E;}}_LL4D: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_uint_typ);goto _LL43;_LL4E: {struct Cyc_Absyn_Int_p_struct*_tmpA2=(
struct Cyc_Absyn_Int_p_struct*)_tmp95;if(_tmpA2->tag != 9)goto _LL50;else{_tmpA3=
_tmpA2->f1;if(_tmpA3 != Cyc_Absyn_None)goto _LL50;}}_LL4F: goto _LL51;_LL50: {struct
Cyc_Absyn_Int_p_struct*_tmpA4=(struct Cyc_Absyn_Int_p_struct*)_tmp95;if(_tmpA4->tag
!= 9)goto _LL52;else{_tmpA5=_tmpA4->f1;if(_tmpA5 != Cyc_Absyn_Signed)goto _LL52;}}
_LL51: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL43;_LL52: {struct Cyc_Absyn_Char_p_struct*
_tmpA6=(struct Cyc_Absyn_Char_p_struct*)_tmp95;if(_tmpA6->tag != 10)goto _LL54;}
_LL53: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL43;_LL54: {struct Cyc_Absyn_Float_p_struct*
_tmpA7=(struct Cyc_Absyn_Float_p_struct*)_tmp95;if(_tmpA7->tag != 11)goto _LL56;}
_LL55: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ);goto _LL43;_LL56: {struct Cyc_Absyn_Enum_p_struct*
_tmpA8=(struct Cyc_Absyn_Enum_p_struct*)_tmp95;if(_tmpA8->tag != 12)goto _LL58;
else{_tmpA9=_tmpA8->f1;}}_LL57: t=Cyc_Tcpat_num_type(topt,(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Absyn_EnumType_struct
_tmpDC;_tmpDC.tag=14;_tmpDC.f1=_tmpA9->name;_tmpDC.f2=(struct Cyc_Absyn_Enumdecl*)
_tmpA9;_tmpDC;});_tmpDB;}));goto _LL43;_LL58: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmpAA=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp95;if(_tmpAA->tag != 13)goto _LL5A;
else{_tmpAB=(void*)_tmpAA->f1;}}_LL59: t=Cyc_Tcpat_num_type(topt,_tmpAB);goto
_LL43;_LL5A: {struct Cyc_Absyn_Null_p_struct*_tmpAC=(struct Cyc_Absyn_Null_p_struct*)
_tmp95;if(_tmpAC->tag != 8)goto _LL5C;}_LL5B: if(topt != 0){void*_tmpDD=Cyc_Tcutil_compress(*
topt);_LL6F: {struct Cyc_Absyn_PointerType_struct*_tmpDE=(struct Cyc_Absyn_PointerType_struct*)
_tmpDD;if(_tmpDE->tag != 5)goto _LL71;}_LL70: t=*topt;goto tcpat_end;_LL71:;_LL72:
goto _LL6E;_LL6E:;}{struct Cyc_Core_Opt*_tmpDF=Cyc_Tcenv_lookup_opt_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));
_tmpE0[0]=({struct Cyc_Absyn_PointerType_struct _tmpE1;_tmpE1.tag=5;_tmpE1.f1=({
struct Cyc_Absyn_PtrInfo _tmpE2;_tmpE2.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ak,_tmpDF);_tmpE2.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpE2.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpE3;_tmpE3.rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,_tmpDF);_tmpE3.nullable=Cyc_Absyn_true_conref;_tmpE3.bounds=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmpE3.zero_term=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmpE3.ptrloc=0;_tmpE3;});
_tmpE2;});_tmpE1;});_tmpE0;});goto _LL43;};_LL5C: {struct Cyc_Absyn_Pointer_p_struct*
_tmpAD=(struct Cyc_Absyn_Pointer_p_struct*)_tmp95;if(_tmpAD->tag != 5)goto _LL5E;
else{_tmpAE=_tmpAD->f1;}}_LL5D: {void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpE4=0;int elt_const=0;if(topt != 0){void*_tmpE5=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmpE7;void*_tmpE8;struct Cyc_Absyn_Tqual _tmpE9;_LL74: {
struct Cyc_Absyn_PointerType_struct*_tmpE6=(struct Cyc_Absyn_PointerType_struct*)
_tmpE5;if(_tmpE6->tag != 5)goto _LL76;else{_tmpE7=_tmpE6->f1;_tmpE8=_tmpE7.elt_typ;
_tmpE9=_tmpE7.elt_tq;}}_LL75: inner_typ=_tmpE8;_tmpE4=(void**)& inner_typ;
elt_const=_tmpE9.real_const;goto _LL73;_LL76:;_LL77: goto _LL73;_LL73:;}{void*
ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(
te));res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpAE,_tmpE4,(void**)&
ptr_rgn));{void*_tmpEA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpAE->topt))->v);struct Cyc_Absyn_DatatypeFieldInfo _tmpEC;union Cyc_Absyn_DatatypeFieldInfoU
_tmpED;struct _tuple2 _tmpEE;struct Cyc_Absyn_Datatypedecl*_tmpEF;struct Cyc_Absyn_Datatypefield*
_tmpF0;struct Cyc_List_List*_tmpF1;_LL79: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpEB=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpEA;if(_tmpEB->tag != 4)goto
_LL7B;else{_tmpEC=_tmpEB->f1;_tmpED=_tmpEC.field_info;if((_tmpED.KnownDatatypefield).tag
!= 2)goto _LL7B;_tmpEE=(struct _tuple2)(_tmpED.KnownDatatypefield).val;_tmpEF=
_tmpEE.f1;_tmpF0=_tmpEE.f2;_tmpF1=_tmpEC.targs;}}_LL7A:{void*_tmpF2=Cyc_Tcutil_compress(
inner_typ);_LL7E: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmpF3=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpF2;if(_tmpF3->tag != 4)goto _LL80;}_LL7F: goto DONT_PROMOTE;_LL80:;_LL81: goto
_LL7D;_LL7D:;}{struct Cyc_Absyn_DatatypeType_struct*_tmpF4=({struct Cyc_Absyn_DatatypeType_struct*
_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_DatatypeType_struct
_tmpFA;_tmpFA.tag=3;_tmpFA.f1=({struct Cyc_Absyn_DatatypeInfo _tmpFB;_tmpFB.datatype_info=
Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmpFC=_cycalloc(sizeof(*
_tmpFC));_tmpFC[0]=_tmpEF;_tmpFC;}));_tmpFB.targs=_tmpF1;_tmpFB;});_tmpFA;});
_tmpF9;});((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v=(void*)((void*)
_tmpF4);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmpF5=_cycalloc(sizeof(*
_tmpF5));_tmpF5[0]=({struct Cyc_Absyn_PointerType_struct _tmpF6;_tmpF6.tag=5;
_tmpF6.f1=({struct Cyc_Absyn_PtrInfo _tmpF7;_tmpF7.elt_typ=(void*)_tmpF4;_tmpF7.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0): Cyc_Absyn_empty_tqual(0);_tmpF7.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpF8;_tmpF8.rgn=ptr_rgn;_tmpF8.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmpF8.bounds=Cyc_Absyn_bounds_one_conref;_tmpF8.zero_term=
Cyc_Absyn_false_conref;_tmpF8.ptrloc=0;_tmpF8;});_tmpF7;});_tmpF6;});_tmpF5;});
goto _LL78;};_LL7B:;_LL7C: DONT_PROMOTE: t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_PointerType_struct
_tmpFE;_tmpFE.tag=5;_tmpFE.f1=({struct Cyc_Absyn_PtrInfo _tmpFF;_tmpFF.elt_typ=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAE->topt))->v;_tmpFF.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0): Cyc_Absyn_empty_tqual(0);_tmpFF.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmp100;_tmp100.rgn=ptr_rgn;_tmp100.nullable=((union Cyc_Absyn_Constraint*(*)())
Cyc_Absyn_empty_conref)();_tmp100.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmp100.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
_tmp100.ptrloc=0;_tmp100;});_tmpFF;});_tmpFE;});_tmpFD;});_LL78:;}goto _LL43;};}
_LL5E: {struct Cyc_Absyn_Tuple_p_struct*_tmpAF=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp95;if(_tmpAF->tag != 4)goto _LL60;else{_tmpB0=_tmpAF->f1;_tmpB1=(struct Cyc_List_List**)&
_tmpAF->f1;_tmpB2=_tmpAF->f2;}}_LL5F: {struct Cyc_List_List*_tmp101=*_tmpB1;
struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*
_tmp102=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp104;_LL83: {struct Cyc_Absyn_TupleType_struct*
_tmp103=(struct Cyc_Absyn_TupleType_struct*)_tmp102;if(_tmp103->tag != 11)goto
_LL85;else{_tmp104=_tmp103->f1;}}_LL84: topt_ts=_tmp104;if(_tmpB2){int _tmp105=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp101);int _tmp106=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp104);if(_tmp105 < _tmp106){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp106 - _tmp105;++ i){wild_ps=({struct Cyc_List_List*
_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->hd=Cyc_Tcpat_wild_pat(p->loc);
_tmp107->tl=wild_ps;_tmp107;});}}*_tmpB1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp101,wild_ps);_tmp101=*_tmpB1;}
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp101)== ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp104))({void*_tmp108=0;Cyc_Tcutil_warn(
p->loc,({const char*_tmp109="unnecessary ... in tuple pattern";_tag_dyneither(
_tmp109,sizeof(char),33);}),_tag_dyneither(_tmp108,sizeof(void*),0));});}}goto
_LL82;_LL85:;_LL86: goto _LL82;_LL82:;}else{if(_tmpB2)({void*_tmp10A=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmp10B="cannot determine missing fields for ... in tuple pattern";
_tag_dyneither(_tmp10B,sizeof(char),57);}),_tag_dyneither(_tmp10A,sizeof(void*),
0));});}for(0;_tmp101 != 0;_tmp101=_tmp101->tl){void**_tmp10C=0;if(topt_ts != 0){
_tmp10C=(void**)&(*((struct _tuple15*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=
Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)
_tmp101->hd,_tmp10C,rgn_opt));pat_ts=({struct Cyc_List_List*_tmp10D=_cycalloc(
sizeof(*_tmp10D));_tmp10D->hd=({struct _tuple15*_tmp10E=_cycalloc(sizeof(*_tmp10E));
_tmp10E->f1=Cyc_Absyn_empty_tqual(0);_tmp10E->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Pat*)_tmp101->hd)->topt))->v;_tmp10E;});_tmp10D->tl=
pat_ts;_tmp10D;});}t=(void*)({struct Cyc_Absyn_TupleType_struct*_tmp10F=_cycalloc(
sizeof(*_tmp10F));_tmp10F[0]=({struct Cyc_Absyn_TupleType_struct _tmp110;_tmp110.tag=
11;_tmp110.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
pat_ts);_tmp110;});_tmp10F;});goto _LL43;}_LL60: {struct Cyc_Absyn_Aggr_p_struct*
_tmpB3=(struct Cyc_Absyn_Aggr_p_struct*)_tmp95;if(_tmpB3->tag != 6)goto _LL62;else{
_tmpB4=_tmpB3->f1;if(_tmpB4 == 0)goto _LL62;_tmpB5=*_tmpB4;_tmpB6=_tmpB5.aggr_info;
if((_tmpB6.KnownAggr).tag != 2)goto _LL62;_tmpB7=(struct Cyc_Absyn_Aggrdecl**)(
_tmpB6.KnownAggr).val;_tmpB8=*_tmpB7;_tmpB9=_tmpB5.targs;_tmpBA=(struct Cyc_List_List**)&(*
_tmpB3->f1).targs;_tmpBB=_tmpB3->f2;_tmpBC=_tmpB3->f3;_tmpBD=(struct Cyc_List_List**)&
_tmpB3->f3;_tmpBE=_tmpB3->f4;}}_LL61: {struct Cyc_List_List*_tmp111=*_tmpBD;
struct _dyneither_ptr aggr_str=_tmpB8->kind == Cyc_Absyn_StructA?({const char*
_tmp15D="struct";_tag_dyneither(_tmp15D,sizeof(char),7);}):({const char*_tmp15E="union";
_tag_dyneither(_tmp15E,sizeof(char),6);});if(_tmpB8->impl == 0){({struct Cyc_String_pa_struct
_tmp114;_tmp114.tag=0;_tmp114.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
aggr_str);({void*_tmp112[1]={& _tmp114};Cyc_Tcutil_terr(p->loc,({const char*
_tmp113="can't destructure an abstract %s";_tag_dyneither(_tmp113,sizeof(char),
33);}),_tag_dyneither(_tmp112,sizeof(void*),1));});});t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));goto _LL43;}if(_tmpBB != 0  || _tmpB8->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->tagged)rgn_opt=0;{struct _RegionHandle _tmp115=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp115;_push_region(rgn);{struct Cyc_List_List*
_tmp116=0;struct Cyc_List_List*outlives_constraints=0;struct Cyc_List_List*_tmp117=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->exist_vars;{struct Cyc_List_List*
t=_tmpBB;for(0;t != 0;t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmp117))->hd;_tmp117=_tmp117->tl;{void*_tmp118=Cyc_Absyn_compress_kb(
tv->kind);void*_tmp119=Cyc_Absyn_compress_kb(uv->kind);int error=0;enum Cyc_Absyn_Kind
k2;{void*_tmp11A=_tmp119;enum Cyc_Absyn_Kind _tmp11C;enum Cyc_Absyn_Kind _tmp11E;
_LL88: {struct Cyc_Absyn_Less_kb_struct*_tmp11B=(struct Cyc_Absyn_Less_kb_struct*)
_tmp11A;if(_tmp11B->tag != 2)goto _LL8A;else{_tmp11C=_tmp11B->f2;}}_LL89: _tmp11E=
_tmp11C;goto _LL8B;_LL8A: {struct Cyc_Absyn_Eq_kb_struct*_tmp11D=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp11A;if(_tmp11D->tag != 0)goto _LL8C;else{_tmp11E=_tmp11D->f1;}}_LL8B: k2=
_tmp11E;goto _LL87;_LL8C:;_LL8D:({void*_tmp11F=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp120="unconstrained existential type variable in aggregate";
_tag_dyneither(_tmp120,sizeof(char),53);}),_tag_dyneither(_tmp11F,sizeof(void*),
0));});_LL87:;}{void*_tmp121=_tmp118;enum Cyc_Absyn_Kind _tmp123;struct Cyc_Core_Opt*
_tmp125;struct Cyc_Core_Opt**_tmp126;enum Cyc_Absyn_Kind _tmp127;struct Cyc_Core_Opt*
_tmp129;struct Cyc_Core_Opt**_tmp12A;_LL8F: {struct Cyc_Absyn_Eq_kb_struct*_tmp122=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp121;if(_tmp122->tag != 0)goto _LL91;else{_tmp123=
_tmp122->f1;}}_LL90: if(!Cyc_Tcutil_kind_leq(k2,_tmp123))error=1;goto _LL8E;_LL91: {
struct Cyc_Absyn_Less_kb_struct*_tmp124=(struct Cyc_Absyn_Less_kb_struct*)_tmp121;
if(_tmp124->tag != 2)goto _LL93;else{_tmp125=_tmp124->f1;_tmp126=(struct Cyc_Core_Opt**)&
_tmp124->f1;_tmp127=_tmp124->f2;}}_LL92: _tmp12A=_tmp126;goto _LL94;_LL93: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp128=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp121;
if(_tmp128->tag != 1)goto _LL8E;else{_tmp129=_tmp128->f1;_tmp12A=(struct Cyc_Core_Opt**)&
_tmp128->f1;}}_LL94:*_tmp12A=({struct Cyc_Core_Opt*_tmp12B=_cycalloc(sizeof(*
_tmp12B));_tmp12B->v=_tmp119;_tmp12B;});goto _LL8E;_LL8E:;}if(error)({struct Cyc_String_pa_struct
_tmp130;_tmp130.tag=0;_tmp130.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(k2));({struct Cyc_String_pa_struct _tmp12F;_tmp12F.tag=0;
_tmp12F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
_tmp118));({struct Cyc_String_pa_struct _tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp12C[3]={& _tmp12E,&
_tmp12F,& _tmp130};Cyc_Tcutil_terr(p->loc,({const char*_tmp12D="type variable %s has kind %s but must have at least kind %s";
_tag_dyneither(_tmp12D,sizeof(char),60);}),_tag_dyneither(_tmp12C,sizeof(void*),
3));});});});});{void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*_tmp136=
_cycalloc(sizeof(*_tmp136));_tmp136[0]=({struct Cyc_Absyn_VarType_struct _tmp137;
_tmp137.tag=2;_tmp137.f1=tv;_tmp137;});_tmp136;});_tmp116=({struct Cyc_List_List*
_tmp131=_region_malloc(rgn,sizeof(*_tmp131));_tmp131->hd=(void*)vartype;_tmp131->tl=
_tmp116;_tmp131;});if(k2 == Cyc_Absyn_RgnKind)outlives_constraints=({struct Cyc_List_List*
_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->hd=({struct _tuple0*_tmp133=
_cycalloc(sizeof(*_tmp133));_tmp133->f1=Cyc_Absyn_empty_effect;_tmp133->f2=
vartype;_tmp133;});_tmp132->tl=outlives_constraints;_tmp132;});else{if(k2 == Cyc_Absyn_UniqueRgnKind
 || k2 == Cyc_Absyn_TopRgnKind)({void*_tmp134=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp135="opened existential had unique or top region kind";
_tag_dyneither(_tmp135,sizeof(char),49);}),_tag_dyneither(_tmp134,sizeof(void*),
0));});}};};}}_tmp116=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp116);{struct _RegionHandle*_tmp138=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcenv_Tenv*
te2=Cyc_Tcenv_add_type_vars(_tmp138,p->loc,te,_tmpBB);struct Cyc_List_List*
_tmp139=Cyc_Tcenv_lookup_type_vars(te2);struct _tuple10 _tmp13A=({struct _tuple10
_tmp15C;_tmp15C.f1=_tmp139;_tmp15C.f2=rgn;_tmp15C;});struct Cyc_List_List*_tmp13B=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,
struct Cyc_Absyn_Tvar*),struct _tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp13A,_tmpB8->tvs);struct Cyc_List_List*_tmp13C=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->exist_vars,_tmp116);struct Cyc_List_List*_tmp13D=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple11*))Cyc_Core_snd,_tmp13B);struct Cyc_List_List*_tmp13E=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple11*))Cyc_Core_snd,_tmp13C);struct Cyc_List_List*_tmp13F=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmp13B,_tmp13C);if(_tmpBB != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->rgn_po != 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple12*_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140->f1=0;_tmp140->f2=0;
_tmp140;});(*res.tvars_and_bounds_opt).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,_tmpBB);(*
res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmp141=0;{struct Cyc_List_List*
_tmp142=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->rgn_po;for(0;
_tmp142 != 0;_tmp142=_tmp142->tl){_tmp141=({struct Cyc_List_List*_tmp143=_cycalloc(
sizeof(*_tmp143));_tmp143->hd=({struct _tuple0*_tmp144=_cycalloc(sizeof(*_tmp144));
_tmp144->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp13F,(*((struct _tuple0*)_tmp142->hd)).f1);
_tmp144->f2=Cyc_Tcutil_rsubstitute(rgn,_tmp13F,(*((struct _tuple0*)_tmp142->hd)).f2);
_tmp144;});_tmp143->tl=_tmp141;_tmp143;});}}_tmp141=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp141);(*res.tvars_and_bounds_opt).f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f2,_tmp141);};}*_tmpBA=_tmp13D;t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145[0]=({struct Cyc_Absyn_AggrType_struct
_tmp146;_tmp146.tag=12;_tmp146.f1=({struct Cyc_Absyn_AggrInfo _tmp147;_tmp147.aggr_info=
Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp148=_cycalloc(sizeof(*
_tmp148));_tmp148[0]=_tmpB8;_tmp148;}));_tmp147.targs=*_tmpBA;_tmp147;});_tmp146;});
_tmp145;});if(_tmpBE  && _tmpB8->kind == Cyc_Absyn_UnionA)({void*_tmp149=0;Cyc_Tcutil_warn(
p->loc,({const char*_tmp14A="`...' pattern not allowed in union pattern";
_tag_dyneither(_tmp14A,sizeof(char),43);}),_tag_dyneither(_tmp149,sizeof(void*),
0));});else{if(_tmpBE){int _tmp14B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp111);int _tmp14C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB8->impl))->fields);if(_tmp14B < _tmp14C){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp14C - _tmp14B;++ i){wild_dps=({struct Cyc_List_List*
_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->hd=({struct _tuple14*_tmp14E=
_cycalloc(sizeof(*_tmp14E));_tmp14E->f1=0;_tmp14E->f2=Cyc_Tcpat_wild_pat(p->loc);
_tmp14E;});_tmp14D->tl=wild_dps;_tmp14D;});}}*_tmpBD=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp111,
wild_dps);_tmp111=*_tmpBD;}else{if(_tmp14B == _tmp14C)({void*_tmp14F=0;Cyc_Tcutil_warn(
p->loc,({const char*_tmp150="unnecessary ... in struct pattern";_tag_dyneither(
_tmp150,sizeof(char),34);}),_tag_dyneither(_tmp14F,sizeof(void*),0));});}}}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,
struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,
_tmp111,_tmpB8->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB8->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple16 _tmp152;struct Cyc_Absyn_Aggrfield*
_tmp153;struct Cyc_Absyn_Pat*_tmp154;struct _tuple16*_tmp151=(struct _tuple16*)
fields->hd;_tmp152=*_tmp151;_tmp153=_tmp152.f1;_tmp154=_tmp152.f2;{void*_tmp155=
Cyc_Tcutil_rsubstitute(rgn,_tmp13F,_tmp153->type);res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te2,_tmp154,(void**)& _tmp155,rgn_opt));if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp154->topt))->v,_tmp155))({struct Cyc_String_pa_struct
_tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp154->topt))->v));({
struct Cyc_String_pa_struct _tmp15A;_tmp15A.tag=0;_tmp15A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp155));({struct Cyc_String_pa_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
aggr_str);({struct Cyc_String_pa_struct _tmp158;_tmp158.tag=0;_tmp158.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp153->name);({void*_tmp156[4]={&
_tmp158,& _tmp159,& _tmp15A,& _tmp15B};Cyc_Tcutil_terr(p->loc,({const char*_tmp157="field %s of %s pattern expects type %s != %s";
_tag_dyneither(_tmp157,sizeof(char),45);}),_tag_dyneither(_tmp156,sizeof(void*),
4));});});});});});};}};};}_npop_handler(0);goto _LL43;;_pop_region(rgn);};}_LL62: {
struct Cyc_Absyn_Datatype_p_struct*_tmpBF=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp95;if(_tmpBF->tag != 7)goto _LL64;else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;
_tmpC2=_tmpBF->f3;_tmpC3=(struct Cyc_List_List**)& _tmpBF->f3;_tmpC4=_tmpBF->f4;}}
_LL63: {struct Cyc_List_List*_tmp15F=*_tmpC3;struct _RegionHandle*_tmp160=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*tqts=_tmpC1->typs;struct Cyc_List_List*_tmp161=Cyc_Tcenv_lookup_type_vars(
te);struct _tuple10 _tmp162=({struct _tuple10 _tmp17B;_tmp17B.f1=_tmp161;_tmp17B.f2=
_tmp160;_tmp17B;});struct Cyc_List_List*_tmp163=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple10*,struct Cyc_Absyn_Tvar*),struct
_tuple10*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp160,Cyc_Tcutil_r_make_inst_var,&
_tmp162,_tmpC0->tvs);struct Cyc_List_List*_tmp164=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple11*))Cyc_Core_snd,_tmp163);t=(void*)({struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165[0]=({struct Cyc_Absyn_DatatypeFieldType_struct
_tmp166;_tmp166.tag=4;_tmp166.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp167;
_tmp167.field_info=Cyc_Absyn_KnownDatatypefield(_tmpC0,_tmpC1);_tmp167.targs=
_tmp164;_tmp167;});_tmp166;});_tmp165;});if(_tmpC4){int _tmp168=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp15F);int _tmp169=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(tqts);if(_tmp168 < _tmp169){struct Cyc_List_List*wild_ps=0;{
int i=0;for(0;i < _tmp169 - _tmp168;++ i){wild_ps=({struct Cyc_List_List*_tmp16A=
_cycalloc(sizeof(*_tmp16A));_tmp16A->hd=Cyc_Tcpat_wild_pat(p->loc);_tmp16A->tl=
wild_ps;_tmp16A;});}}*_tmpC3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp15F,wild_ps);_tmp15F=*_tmpC3;}
else{if(_tmp168 == _tmp169)({struct Cyc_String_pa_struct _tmp16D;_tmp16D.tag=0;
_tmp16D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpC0->name));({void*_tmp16B[1]={& _tmp16D};Cyc_Tcutil_warn(p->loc,({const char*
_tmp16C="unnecessary ... in datatype field %s";_tag_dyneither(_tmp16C,sizeof(
char),37);}),_tag_dyneither(_tmp16B,sizeof(void*),1));});});}}for(0;_tmp15F != 0
 && tqts != 0;(_tmp15F=_tmp15F->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*_tmp16E=(
struct Cyc_Absyn_Pat*)_tmp15F->hd;void*_tmp16F=Cyc_Tcutil_rsubstitute(_tmp160,
_tmp163,(*((struct _tuple15*)tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(
te,_tmp16E,(void**)& _tmp16F,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp16E->topt))->v,_tmp16F))({struct Cyc_String_pa_struct _tmp174;
_tmp174.tag=0;_tmp174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16E->topt))->v));({struct Cyc_String_pa_struct
_tmp173;_tmp173.tag=0;_tmp173.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp16F));({struct Cyc_String_pa_struct _tmp172;_tmp172.tag=
0;_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpC1->name));({void*_tmp170[3]={& _tmp172,& _tmp173,& _tmp174};Cyc_Tcutil_terr(
_tmp16E->loc,({const char*_tmp171="%s expects argument type %s, not %s";
_tag_dyneither(_tmp171,sizeof(char),36);}),_tag_dyneither(_tmp170,sizeof(void*),
3));});});});});}if(_tmp15F != 0)({struct Cyc_String_pa_struct _tmp177;_tmp177.tag=
0;_tmp177.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpC1->name));({void*_tmp175[1]={& _tmp177};Cyc_Tcutil_terr(p->loc,({const char*
_tmp176="too many arguments for datatype constructor %s";_tag_dyneither(_tmp176,
sizeof(char),47);}),_tag_dyneither(_tmp175,sizeof(void*),1));});});if(tqts != 0)({
struct Cyc_String_pa_struct _tmp17A;_tmp17A.tag=0;_tmp17A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC1->name));({void*_tmp178[1]={&
_tmp17A};Cyc_Tcutil_terr(p->loc,({const char*_tmp179="too few arguments for datatype constructor %s";
_tag_dyneither(_tmp179,sizeof(char),46);}),_tag_dyneither(_tmp178,sizeof(void*),
1));});});}goto _LL43;}_LL64: {struct Cyc_Absyn_Aggr_p_struct*_tmpC5=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp95;if(_tmpC5->tag != 6)goto _LL66;else{_tmpC6=_tmpC5->f1;if(_tmpC6 != 0)goto
_LL66;}}_LL65: goto _LL67;_LL66: {struct Cyc_Absyn_Aggr_p_struct*_tmpC7=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp95;if(_tmpC7->tag != 6)goto _LL68;else{_tmpC8=_tmpC7->f1;if(_tmpC8 == 0)goto
_LL68;_tmpC9=*_tmpC8;_tmpCA=_tmpC9.aggr_info;if((_tmpCA.UnknownAggr).tag != 1)
goto _LL68;_tmpCB=(struct _tuple3)(_tmpCA.UnknownAggr).val;}}_LL67: goto _LL69;_LL68: {
struct Cyc_Absyn_UnknownId_p_struct*_tmpCC=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp95;if(_tmpCC->tag != 14)goto _LL6A;}_LL69: goto _LL6B;_LL6A: {struct Cyc_Absyn_Exp_p_struct*
_tmpCD=(struct Cyc_Absyn_Exp_p_struct*)_tmp95;if(_tmpCD->tag != 16)goto _LL6C;}
_LL6B: goto _LL6D;_LL6C: {struct Cyc_Absyn_UnknownCall_p_struct*_tmpCE=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp95;if(_tmpCE->tag != 15)goto _LL43;}_LL6D: t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));goto _LL43;_LL43:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp17C=_cycalloc(
sizeof(*_tmp17C));_tmp17C->v=t;_tmp17C;});return res;};}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp17E=Cyc_Tcpat_tcPatRec(te,p,(void**)topt,0);struct
_RegionHandle*_tmp17F=Cyc_Tcenv_get_fnrgn(te);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmp17F,Cyc_Tcpat_get_name,_tmp17E.patvars),
p->loc,({const char*_tmp180="pattern contains a repeated variable";_tag_dyneither(
_tmp180,sizeof(char),37);}));return _tmp17E;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp181=p->r;struct Cyc_Absyn_Pat*
_tmp183;struct Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp187;struct Cyc_List_List*
_tmp189;_LL96: {struct Cyc_Absyn_Pointer_p_struct*_tmp182=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp181;if(_tmp182->tag != 5)goto _LL98;else{_tmp183=_tmp182->f1;}}_LL97: Cyc_Tcpat_check_pat_regions(
te,_tmp183);{struct _RegionHandle*_tmp18A=Cyc_Tcenv_get_fnrgn(te);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmp18A,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp183->topt))->v)){({void*
_tmp18B=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp18C="Pattern dereferences to a non-aliased pointer; use swap";
_tag_dyneither(_tmp18C,sizeof(char),56);}),_tag_dyneither(_tmp18B,sizeof(void*),
0));});return;}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp18A,(void*)((struct
Cyc_Core_Opt*)_check_null(p->topt))->v)){({void*_tmp18D=0;Cyc_Tcutil_terr(p->loc,({
const char*_tmp18E="Pattern dereferences a non-aliased pointer; use swap";
_tag_dyneither(_tmp18E,sizeof(char),53);}),_tag_dyneither(_tmp18D,sizeof(void*),
0));});return;}{void*_tmp18F=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp191;struct Cyc_Absyn_PtrAtts _tmp192;void*_tmp193;_LLA1: {
struct Cyc_Absyn_PointerType_struct*_tmp190=(struct Cyc_Absyn_PointerType_struct*)
_tmp18F;if(_tmp190->tag != 5)goto _LLA3;else{_tmp191=_tmp190->f1;_tmp192=_tmp191.ptr_atts;
_tmp193=_tmp192.rgn;}}_LLA2: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp193);
return;_LLA3:;_LLA4:({void*_tmp194=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp195="check_pat_regions: bad pointer type";
_tag_dyneither(_tmp195,sizeof(char),36);}),_tag_dyneither(_tmp194,sizeof(void*),
0));});_LLA0:;};};_LL98: {struct Cyc_Absyn_Aggr_p_struct*_tmp184=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp181;if(_tmp184->tag != 6)goto _LL9A;else{_tmp185=_tmp184->f3;}}_LL99: for(0;
_tmp185 != 0;_tmp185=_tmp185->tl){Cyc_Tcpat_check_pat_regions(te,(*((struct
_tuple14*)_tmp185->hd)).f2);}return;_LL9A: {struct Cyc_Absyn_Datatype_p_struct*
_tmp186=(struct Cyc_Absyn_Datatype_p_struct*)_tmp181;if(_tmp186->tag != 7)goto
_LL9C;else{_tmp187=_tmp186->f3;}}_LL9B: _tmp189=_tmp187;goto _LL9D;_LL9C: {struct
Cyc_Absyn_Tuple_p_struct*_tmp188=(struct Cyc_Absyn_Tuple_p_struct*)_tmp181;if(
_tmp188->tag != 4)goto _LL9E;else{_tmp189=_tmp188->f1;}}_LL9D: for(0;_tmp189 != 0;
_tmp189=_tmp189->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp189->hd);}return;_LL9E:;_LL9F: return;_LL95:;}struct _union_Name_value_Name_v{
int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};
union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct
_union_Name_value_Int_v Int_v;};union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct
_dyneither_ptr s){return({union Cyc_Tcpat_Name_value _tmp196;(_tmp196.Name_v).val=s;(
_tmp196.Name_v).tag=1;_tmp196;});}union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){
return({union Cyc_Tcpat_Name_value _tmp197;(_tmp197.Int_v).val=i;(_tmp197.Int_v).tag=
2;_tmp197;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*
span;struct Cyc_Absyn_Pat*orig_pat;};struct Cyc_Tcpat_Any_struct{int tag;};struct
Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp198=c1->name;struct _dyneither_ptr _tmp199;int _tmp19A;
_LLA6: if((_tmp198.Name_v).tag != 1)goto _LLA8;_tmp199=(struct _dyneither_ptr)(
_tmp198.Name_v).val;_LLA7: {union Cyc_Tcpat_Name_value _tmp19B=c2->name;struct
_dyneither_ptr _tmp19C;int _tmp19D;_LLAB: if((_tmp19B.Name_v).tag != 1)goto _LLAD;
_tmp19C=(struct _dyneither_ptr)(_tmp19B.Name_v).val;_LLAC: return Cyc_strcmp((
struct _dyneither_ptr)_tmp199,(struct _dyneither_ptr)_tmp19C);_LLAD: if((_tmp19B.Int_v).tag
!= 2)goto _LLAA;_tmp19D=(int)(_tmp19B.Int_v).val;_LLAE: return - 1;_LLAA:;}_LLA8: if((
_tmp198.Int_v).tag != 2)goto _LLA5;_tmp19A=(int)(_tmp198.Int_v).val;_LLA9: {union
Cyc_Tcpat_Name_value _tmp19E=c2->name;struct _dyneither_ptr _tmp19F;int _tmp1A0;
_LLB0: if((_tmp19E.Name_v).tag != 1)goto _LLB2;_tmp19F=(struct _dyneither_ptr)(
_tmp19E.Name_v).val;_LLB1: return 1;_LLB2: if((_tmp19E.Int_v).tag != 2)goto _LLAF;
_tmp1A0=(int)(_tmp19E.Int_v).val;_LLB3: return _tmp19A - _tmp1A0;_LLAF:;}_LLA5:;}
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){return((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,
struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}static int Cyc_Tcpat_one_opt=
1;static int Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=256;static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp1A1=_region_malloc(r,sizeof(*_tmp1A1));
_tmp1A1->name=Cyc_Tcpat_Name_v(({const char*_tmp1A2="NULL";_tag_dyneither(_tmp1A2,
sizeof(char),5);}));_tmp1A1->arity=0;_tmp1A1->span=(int*)& Cyc_Tcpat_two_opt;
_tmp1A1->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp1A1;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp1A3=_region_malloc(r,sizeof(*_tmp1A3));_tmp1A3->name=
Cyc_Tcpat_Name_v(({const char*_tmp1A4="&";_tag_dyneither(_tmp1A4,sizeof(char),2);}));
_tmp1A3->arity=1;_tmp1A3->span=(int*)& Cyc_Tcpat_two_opt;_tmp1A3->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp1A3;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(
struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp1A5=_region_malloc(r,sizeof(*_tmp1A5));_tmp1A5->name=Cyc_Tcpat_Name_v(({
const char*_tmp1A6="&";_tag_dyneither(_tmp1A6,sizeof(char),2);}));_tmp1A5->arity=
1;_tmp1A5->span=(int*)& Cyc_Tcpat_one_opt;_tmp1A5->orig_pat=(struct Cyc_Absyn_Pat*)
p;_tmp1A5;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*
r,int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp1A7=
_region_malloc(r,sizeof(*_tmp1A7));_tmp1A7->name=Cyc_Tcpat_Int_v(i);_tmp1A7->arity=
0;_tmp1A7->span=0;_tmp1A7->orig_pat=p;_tmp1A7;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp1A8=_region_malloc(r,sizeof(*_tmp1A8));
_tmp1A8->name=Cyc_Tcpat_Name_v(f);_tmp1A8->arity=0;_tmp1A8->span=0;_tmp1A8->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp1A8;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(
struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp1A9=_region_malloc(r,sizeof(*_tmp1A9));_tmp1A9->name=Cyc_Tcpat_Int_v((int)c);
_tmp1A9->arity=0;_tmp1A9->span=(int*)& Cyc_Tcpat_twofiftysix_opt;_tmp1A9->orig_pat=(
struct Cyc_Absyn_Pat*)p;_tmp1A9;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(
struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp1AA=_region_malloc(r,sizeof(*_tmp1AA));_tmp1AA->name=Cyc_Tcpat_Name_v(({
const char*_tmp1AB="$";_tag_dyneither(_tmp1AB,sizeof(char),2);}));_tmp1AA->arity=
i;_tmp1AA->span=(int*)& Cyc_Tcpat_one_opt;_tmp1AA->orig_pat=p;_tmp1AA;});}static
void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp1AC=_region_malloc(r,sizeof(*_tmp1AC));_tmp1AC[0]=({
struct Cyc_Tcpat_Con_struct _tmp1AD;_tmp1AD.tag=1;_tmp1AD.f1=Cyc_Tcpat_null_con(r,
p);_tmp1AD.f2=0;_tmp1AD;});_tmp1AC;});}static void*Cyc_Tcpat_int_pat(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp1AE=_region_malloc(r,sizeof(*_tmp1AE));_tmp1AE[0]=({struct Cyc_Tcpat_Con_struct
_tmp1AF;_tmp1AF.tag=1;_tmp1AF.f1=Cyc_Tcpat_int_con(r,i,p);_tmp1AF.f2=0;_tmp1AF;});
_tmp1AE;});}static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*
p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp1B0=_region_malloc(r,sizeof(*
_tmp1B0));_tmp1B0[0]=({struct Cyc_Tcpat_Con_struct _tmp1B1;_tmp1B1.tag=1;_tmp1B1.f1=
Cyc_Tcpat_char_con(r,c,p);_tmp1B1.f2=0;_tmp1B1;});_tmp1B0;});}static void*Cyc_Tcpat_float_pat(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp1B2=_region_malloc(r,sizeof(*_tmp1B2));_tmp1B2[0]=({
struct Cyc_Tcpat_Con_struct _tmp1B3;_tmp1B3.tag=1;_tmp1B3.f1=Cyc_Tcpat_float_con(r,
f,p);_tmp1B3.f2=0;_tmp1B3;});_tmp1B2;});}static void*Cyc_Tcpat_null_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp1B4=_region_malloc(r,sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_Tcpat_Con_struct
_tmp1B5;_tmp1B5.tag=1;_tmp1B5.f1=Cyc_Tcpat_null_ptr_con(r,p0);_tmp1B5.f2=({
struct Cyc_List_List*_tmp1B6=_region_malloc(r,sizeof(*_tmp1B6));_tmp1B6->hd=p;
_tmp1B6->tl=0;_tmp1B6;});_tmp1B5;});_tmp1B4;});}static void*Cyc_Tcpat_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp1B7=_region_malloc(r,sizeof(*_tmp1B7));_tmp1B7[0]=({struct Cyc_Tcpat_Con_struct
_tmp1B8;_tmp1B8.tag=1;_tmp1B8.f1=Cyc_Tcpat_ptr_con(r,p0);_tmp1B8.f2=({struct Cyc_List_List*
_tmp1B9=_region_malloc(r,sizeof(*_tmp1B9));_tmp1B9->hd=p;_tmp1B9->tl=0;_tmp1B9;});
_tmp1B8;});_tmp1B7;});}static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,
struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp1BA=_region_malloc(r,sizeof(*_tmp1BA));_tmp1BA[0]=({struct Cyc_Tcpat_Con_struct
_tmp1BB;_tmp1BB.tag=1;_tmp1BB.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(ss),p);_tmp1BB.f2=ss;_tmp1BB;});_tmp1BA;});}static void*Cyc_Tcpat_con_pat(
struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*
ps,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*
_tmp1BE=_region_malloc(r,sizeof(*_tmp1BE));_tmp1BE->name=Cyc_Tcpat_Name_v(
con_name);_tmp1BE->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);
_tmp1BE->span=span;_tmp1BE->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp1BE;});return(
void*)({struct Cyc_Tcpat_Con_struct*_tmp1BC=_region_malloc(r,sizeof(*_tmp1BC));
_tmp1BC[0]=({struct Cyc_Tcpat_Con_struct _tmp1BD;_tmp1BD.tag=1;_tmp1BD.f1=c;
_tmp1BD.f2=ps;_tmp1BD;});_tmp1BC;});}static void*Cyc_Tcpat_compile_pat(struct
_RegionHandle*r,struct Cyc_Absyn_Pat*p){void*s;{void*_tmp1BF=p->r;enum Cyc_Absyn_Sign
_tmp1C4;int _tmp1C5;char _tmp1C7;struct _dyneither_ptr _tmp1C9;struct Cyc_Absyn_Pat*
_tmp1CB;struct Cyc_Absyn_Pat*_tmp1CD;struct Cyc_Absyn_Pat*_tmp1CF;struct Cyc_Absyn_Datatypedecl*
_tmp1D1;struct Cyc_Absyn_Datatypefield*_tmp1D2;struct Cyc_List_List*_tmp1D3;struct
Cyc_List_List*_tmp1D5;struct Cyc_Absyn_AggrInfo*_tmp1D7;struct Cyc_Absyn_AggrInfo
_tmp1D8;union Cyc_Absyn_AggrInfoU _tmp1D9;struct Cyc_Absyn_Aggrdecl**_tmp1DA;struct
Cyc_Absyn_Aggrdecl*_tmp1DB;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_Enumdecl*
_tmp1DE;struct Cyc_Absyn_Enumfield*_tmp1DF;void*_tmp1E1;struct Cyc_Absyn_Enumfield*
_tmp1E2;_LLB5: {struct Cyc_Absyn_Wild_p_struct*_tmp1C0=(struct Cyc_Absyn_Wild_p_struct*)
_tmp1BF;if(_tmp1C0->tag != 0)goto _LLB7;}_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_TagInt_p_struct*
_tmp1C1=(struct Cyc_Absyn_TagInt_p_struct*)_tmp1BF;if(_tmp1C1->tag != 3)goto _LLB9;}
_LLB8: s=(void*)({struct Cyc_Tcpat_Any_struct*_tmp1E7=_region_malloc(r,sizeof(*
_tmp1E7));_tmp1E7[0]=({struct Cyc_Tcpat_Any_struct _tmp1E8;_tmp1E8.tag=0;_tmp1E8;});
_tmp1E7;});goto _LLB4;_LLB9: {struct Cyc_Absyn_Null_p_struct*_tmp1C2=(struct Cyc_Absyn_Null_p_struct*)
_tmp1BF;if(_tmp1C2->tag != 8)goto _LLBB;}_LLBA: s=Cyc_Tcpat_null_pat(r,p);goto _LLB4;
_LLBB: {struct Cyc_Absyn_Int_p_struct*_tmp1C3=(struct Cyc_Absyn_Int_p_struct*)
_tmp1BF;if(_tmp1C3->tag != 9)goto _LLBD;else{_tmp1C4=_tmp1C3->f1;_tmp1C5=_tmp1C3->f2;}}
_LLBC: s=Cyc_Tcpat_int_pat(r,_tmp1C5,(struct Cyc_Absyn_Pat*)p);goto _LLB4;_LLBD: {
struct Cyc_Absyn_Char_p_struct*_tmp1C6=(struct Cyc_Absyn_Char_p_struct*)_tmp1BF;
if(_tmp1C6->tag != 10)goto _LLBF;else{_tmp1C7=_tmp1C6->f1;}}_LLBE: s=Cyc_Tcpat_char_pat(
r,_tmp1C7,p);goto _LLB4;_LLBF: {struct Cyc_Absyn_Float_p_struct*_tmp1C8=(struct Cyc_Absyn_Float_p_struct*)
_tmp1BF;if(_tmp1C8->tag != 11)goto _LLC1;else{_tmp1C9=_tmp1C8->f1;}}_LLC0: s=Cyc_Tcpat_float_pat(
r,_tmp1C9,p);goto _LLB4;_LLC1: {struct Cyc_Absyn_Var_p_struct*_tmp1CA=(struct Cyc_Absyn_Var_p_struct*)
_tmp1BF;if(_tmp1CA->tag != 1)goto _LLC3;else{_tmp1CB=_tmp1CA->f2;}}_LLC2: s=Cyc_Tcpat_compile_pat(
r,_tmp1CB);goto _LLB4;_LLC3: {struct Cyc_Absyn_Reference_p_struct*_tmp1CC=(struct
Cyc_Absyn_Reference_p_struct*)_tmp1BF;if(_tmp1CC->tag != 2)goto _LLC5;else{_tmp1CD=
_tmp1CC->f2;}}_LLC4: s=Cyc_Tcpat_compile_pat(r,_tmp1CD);goto _LLB4;_LLC5: {struct
Cyc_Absyn_Pointer_p_struct*_tmp1CE=(struct Cyc_Absyn_Pointer_p_struct*)_tmp1BF;
if(_tmp1CE->tag != 5)goto _LLC7;else{_tmp1CF=_tmp1CE->f1;}}_LLC6:{void*_tmp1E9=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1EB;struct Cyc_Absyn_PtrAtts _tmp1EC;union Cyc_Absyn_Constraint*_tmp1ED;_LLDA: {
struct Cyc_Absyn_PointerType_struct*_tmp1EA=(struct Cyc_Absyn_PointerType_struct*)
_tmp1E9;if(_tmp1EA->tag != 5)goto _LLDC;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EB.ptr_atts;
_tmp1ED=_tmp1EC.nullable;}}_LLDB: {int is_nullable=0;int still_working=1;while(
still_working){union Cyc_Absyn_Constraint*_tmp1EE=_tmp1ED;union Cyc_Absyn_Constraint
_tmp1EF;union Cyc_Absyn_Constraint*_tmp1F0;union Cyc_Absyn_Constraint _tmp1F1;int
_tmp1F2;union Cyc_Absyn_Constraint _tmp1F3;int _tmp1F4;_LLDF: _tmp1EF=*_tmp1EE;if((
_tmp1EF.Forward_constr).tag != 2)goto _LLE1;_tmp1F0=(union Cyc_Absyn_Constraint*)(
_tmp1EF.Forward_constr).val;_LLE0:*_tmp1ED=*_tmp1F0;continue;_LLE1: _tmp1F1=*
_tmp1EE;if((_tmp1F1.No_constr).tag != 3)goto _LLE3;_tmp1F2=(int)(_tmp1F1.No_constr).val;
_LLE2:({struct _union_Constraint_Eq_constr*_tmp1F5=& _tmp1ED->Eq_constr;_tmp1F5->tag=
1;_tmp1F5->val=0;});is_nullable=0;still_working=0;goto _LLDE;_LLE3: _tmp1F3=*
_tmp1EE;if((_tmp1F3.Eq_constr).tag != 1)goto _LLDE;_tmp1F4=(int)(_tmp1F3.Eq_constr).val;
_LLE4: is_nullable=(int)_tmp1F4;still_working=0;goto _LLDE;_LLDE:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp1CF);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLD9;};}_LLDC:;_LLDD:({void*_tmp1F6=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F7="expecting pointertype for pattern!";
_tag_dyneither(_tmp1F7,sizeof(char),35);}),_tag_dyneither(_tmp1F6,sizeof(void*),
0));});_LLD9:;}goto _LLB4;_LLC7: {struct Cyc_Absyn_Datatype_p_struct*_tmp1D0=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp1BF;if(_tmp1D0->tag != 7)goto _LLC9;else{
_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;_tmp1D3=_tmp1D0->f3;}}_LLC8: {int*span;{
void*_tmp1F8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);
_LLE6: {struct Cyc_Absyn_DatatypeType_struct*_tmp1F9=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp1F8;if(_tmp1F9->tag != 3)goto _LLE8;}_LLE7: if(_tmp1D1->is_extensible)span=0;
else{span=({int*_tmp1FB=_region_malloc(r,sizeof(*_tmp1FB));_tmp1FB[0]=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1D1->fields))->v);_tmp1FB;});}goto _LLE5;_LLE8: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp1FA=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp1F8;if(_tmp1FA->tag != 4)
goto _LLEA;}_LLE9: span=(int*)& Cyc_Tcpat_one_opt;goto _LLE5;_LLEA:;_LLEB: span=({
void*_tmp1FC=0;((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1FD="void datatype pattern has bad type";_tag_dyneither(_tmp1FD,
sizeof(char),35);}),_tag_dyneither(_tmp1FC,sizeof(void*),0));});goto _LLE5;_LLE5:;}
s=Cyc_Tcpat_con_pat(r,*(*_tmp1D2->name).f2,span,((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1D3),p);goto _LLB4;}_LLC9: {struct Cyc_Absyn_Tuple_p_struct*_tmp1D4=(struct
Cyc_Absyn_Tuple_p_struct*)_tmp1BF;if(_tmp1D4->tag != 4)goto _LLCB;else{_tmp1D5=
_tmp1D4->f1;}}_LLCA: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp1D5),(struct Cyc_Absyn_Pat*)p);goto _LLB4;_LLCB: {struct Cyc_Absyn_Aggr_p_struct*
_tmp1D6=(struct Cyc_Absyn_Aggr_p_struct*)_tmp1BF;if(_tmp1D6->tag != 6)goto _LLCD;
else{_tmp1D7=_tmp1D6->f1;if(_tmp1D7 == 0)goto _LLCD;_tmp1D8=*_tmp1D7;_tmp1D9=
_tmp1D8.aggr_info;if((_tmp1D9.KnownAggr).tag != 2)goto _LLCD;_tmp1DA=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1D9.KnownAggr).val;_tmp1DB=*_tmp1DA;_tmp1DC=_tmp1D6->f3;}}_LLCC: if(_tmp1DB->kind
== Cyc_Absyn_StructA){struct Cyc_List_List*ps=0;{struct Cyc_List_List*fields=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DB->impl))->fields;for(0;fields != 
0;fields=fields->tl){int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name,({const char*_tmp20D="";_tag_dyneither(_tmp20D,sizeof(char),1);}))
== 0;{struct Cyc_List_List*dlps0=_tmp1DC;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple14 _tmp1FF;struct Cyc_List_List*_tmp200;struct Cyc_Absyn_Pat*_tmp201;
struct _tuple14*_tmp1FE=(struct _tuple14*)dlps0->hd;_tmp1FF=*_tmp1FE;_tmp200=
_tmp1FF.f1;_tmp201=_tmp1FF.f2;{struct Cyc_List_List*_tmp202=_tmp200;struct Cyc_List_List
_tmp203;void*_tmp204;struct _dyneither_ptr*_tmp206;struct Cyc_List_List*_tmp207;
_LLED: if(_tmp202 == 0)goto _LLEF;_tmp203=*_tmp202;_tmp204=(void*)_tmp203.hd;{
struct Cyc_Absyn_FieldName_struct*_tmp205=(struct Cyc_Absyn_FieldName_struct*)
_tmp204;if(_tmp205->tag != 1)goto _LLEF;else{_tmp206=_tmp205->f1;}};_tmp207=
_tmp203.tl;if(_tmp207 != 0)goto _LLEF;_LLEE: if(Cyc_strptrcmp(_tmp206,((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name)== 0){ps=({struct Cyc_List_List*_tmp208=_region_malloc(r,
sizeof(*_tmp208));_tmp208->hd=Cyc_Tcpat_compile_pat(r,_tmp201);_tmp208->tl=ps;
_tmp208;});found=1;}goto _LLEC;_LLEF:;_LLF0:({void*_tmp209=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp20A="bad designator(s)";
_tag_dyneither(_tmp20A,sizeof(char),18);}),_tag_dyneither(_tmp209,sizeof(void*),
0));});_LLEC:;};}}if(!found)({void*_tmp20B=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp20C="bad designator";
_tag_dyneither(_tmp20C,sizeof(char),15);}),_tag_dyneither(_tmp20B,sizeof(void*),
0));});}}s=Cyc_Tcpat_tuple_pat(r,ps,(struct Cyc_Absyn_Pat*)p);}else{if(!((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1DB->impl))->tagged)({void*_tmp20E=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmp20F="patterns on untagged unions not yet supported.";
_tag_dyneither(_tmp20F,sizeof(char),47);}),_tag_dyneither(_tmp20E,sizeof(void*),
0));});{int*span=({int*_tmp21F=_region_malloc(r,sizeof(*_tmp21F));_tmp21F[0]=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1DB->impl))->fields);_tmp21F;});int field_name;struct Cyc_List_List*
_tmp210=_tmp1DC;struct Cyc_List_List _tmp211;struct _tuple14*_tmp212;struct _tuple14
_tmp213;struct Cyc_List_List*_tmp214;struct Cyc_List_List _tmp215;void*_tmp216;
struct _dyneither_ptr*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_Absyn_Pat*
_tmp21A;struct Cyc_List_List*_tmp21B;_LLF2: if(_tmp210 == 0)goto _LLF4;_tmp211=*
_tmp210;_tmp212=(struct _tuple14*)_tmp211.hd;_tmp213=*_tmp212;_tmp214=_tmp213.f1;
if(_tmp214 == 0)goto _LLF4;_tmp215=*_tmp214;_tmp216=(void*)_tmp215.hd;{struct Cyc_Absyn_FieldName_struct*
_tmp217=(struct Cyc_Absyn_FieldName_struct*)_tmp216;if(_tmp217->tag != 1)goto _LLF4;
else{_tmp218=_tmp217->f1;}};_tmp219=_tmp215.tl;if(_tmp219 != 0)goto _LLF4;_tmp21A=
_tmp213.f2;_tmp21B=_tmp211.tl;if(_tmp21B != 0)goto _LLF4;_LLF3: s=Cyc_Tcpat_con_pat(
r,*_tmp218,span,({struct Cyc_List_List*_tmp21C=_region_malloc(r,sizeof(*_tmp21C));
_tmp21C->hd=Cyc_Tcpat_compile_pat(r,_tmp21A);_tmp21C->tl=0;_tmp21C;}),p);goto
_LLF1;_LLF4:;_LLF5:({void*_tmp21D=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp21E="bad union pattern";
_tag_dyneither(_tmp21E,sizeof(char),18);}),_tag_dyneither(_tmp21D,sizeof(void*),
0));});_LLF1:;};}goto _LLB4;_LLCD: {struct Cyc_Absyn_Enum_p_struct*_tmp1DD=(struct
Cyc_Absyn_Enum_p_struct*)_tmp1BF;if(_tmp1DD->tag != 12)goto _LLCF;else{_tmp1DE=
_tmp1DD->f1;_tmp1DF=_tmp1DD->f2;}}_LLCE: {int span=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1DE->fields))->v);
s=Cyc_Tcpat_con_pat(r,*(*_tmp1DF->name).f2,({int*_tmp220=_region_malloc(r,
sizeof(*_tmp220));_tmp220[0]=span;_tmp220;}),0,p);goto _LLB4;}_LLCF: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp1E0=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1BF;if(_tmp1E0->tag != 13)goto
_LLD1;else{_tmp1E1=(void*)_tmp1E0->f1;_tmp1E2=_tmp1E0->f2;}}_LLD0: {struct Cyc_List_List*
fields;{void*_tmp221=Cyc_Tcutil_compress(_tmp1E1);struct Cyc_List_List*_tmp223;
_LLF7: {struct Cyc_Absyn_AnonEnumType_struct*_tmp222=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp221;if(_tmp222->tag != 15)goto _LLF9;else{_tmp223=_tmp222->f1;}}_LLF8: fields=
_tmp223;goto _LLF6;_LLF9:;_LLFA:({void*_tmp224=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp225="bad type in AnonEnum_p";
_tag_dyneither(_tmp225,sizeof(char),23);}),_tag_dyneither(_tmp224,sizeof(void*),
0));});_LLF6:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
s=Cyc_Tcpat_con_pat(r,*(*_tmp1E2->name).f2,({int*_tmp226=_region_malloc(r,
sizeof(*_tmp226));_tmp226[0]=span;_tmp226;}),0,p);goto _LLB4;};}_LLD1: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp1E3=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp1BF;if(_tmp1E3->tag != 14)goto
_LLD3;}_LLD2: goto _LLD4;_LLD3: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp1E4=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1BF;if(_tmp1E4->tag != 15)goto _LLD5;}
_LLD4: goto _LLD6;_LLD5: {struct Cyc_Absyn_Aggr_p_struct*_tmp1E5=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp1BF;if(_tmp1E5->tag != 6)goto _LLD7;}_LLD6: goto _LLD8;_LLD7: {struct Cyc_Absyn_Exp_p_struct*
_tmp1E6=(struct Cyc_Absyn_Exp_p_struct*)_tmp1BF;if(_tmp1E6->tag != 16)goto _LLB4;}
_LLD8: s=(void*)({struct Cyc_Tcpat_Any_struct*_tmp227=_region_malloc(r,sizeof(*
_tmp227));_tmp227[0]=({struct Cyc_Tcpat_Any_struct _tmp228;_tmp228.tag=0;_tmp228;});
_tmp227;});_LLB4:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;struct Cyc_Set_Set*
f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_struct{
int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes
 = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};static void*Cyc_Tcpat_add_neg(struct
_RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp229=td;struct Cyc_Set_Set*
_tmp22B;_LLFC: {struct Cyc_Tcpat_Neg_struct*_tmp22A=(struct Cyc_Tcpat_Neg_struct*)
_tmp229;if(_tmp22A->tag != 1)goto _LLFE;else{_tmp22B=_tmp22A->f1;}}_LLFD: return(
void*)({struct Cyc_Tcpat_Neg_struct*_tmp22D=_region_malloc(r,sizeof(*_tmp22D));
_tmp22D[0]=({struct Cyc_Tcpat_Neg_struct _tmp22E;_tmp22E.tag=1;_tmp22E.f1=((struct
Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_rinsert)(r,_tmp22B,c);_tmp22E;});_tmp22D;});_LLFE: {struct Cyc_Tcpat_Pos_struct*
_tmp22C=(struct Cyc_Tcpat_Pos_struct*)_tmp229;if(_tmp22C->tag != 0)goto _LLFB;}
_LLFF:({void*_tmp22F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp230="add_neg called when td is Positive";
_tag_dyneither(_tmp230,sizeof(char),35);}),_tag_dyneither(_tmp22F,sizeof(void*),
0));});_LLFB:;}static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*
c,void*td){void*_tmp231=td;struct Cyc_Tcpat_Con_s*_tmp233;struct Cyc_Set_Set*
_tmp235;_LL101: {struct Cyc_Tcpat_Pos_struct*_tmp232=(struct Cyc_Tcpat_Pos_struct*)
_tmp231;if(_tmp232->tag != 0)goto _LL103;else{_tmp233=_tmp232->f1;}}_LL102: if(Cyc_Tcpat_compare_con(
c,_tmp233)== 0)return Cyc_Tcpat_Yes;else{return Cyc_Tcpat_No;}_LL103: {struct Cyc_Tcpat_Neg_struct*
_tmp234=(struct Cyc_Tcpat_Neg_struct*)_tmp231;if(_tmp234->tag != 1)goto _LL100;
else{_tmp235=_tmp234->f1;}}_LL104: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp235,c))return Cyc_Tcpat_No;else{if(c->span != 0  && *((int*)
_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp235)+ 1)return Cyc_Tcpat_Yes;else{return Cyc_Tcpat_Maybe;}}_LL100:;}struct
_tuple18{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp236=ctxt;struct Cyc_List_List _tmp237;struct _tuple18*
_tmp238;struct _tuple18 _tmp239;struct Cyc_Tcpat_Con_s*_tmp23A;struct Cyc_List_List*
_tmp23B;struct Cyc_List_List*_tmp23C;_LL106: if(_tmp236 != 0)goto _LL108;_LL107:
return 0;_LL108: if(_tmp236 == 0)goto _LL105;_tmp237=*_tmp236;_tmp238=(struct
_tuple18*)_tmp237.hd;_tmp239=*_tmp238;_tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;
_tmp23C=_tmp237.tl;_LL109: return({struct Cyc_List_List*_tmp23D=_region_malloc(r,
sizeof(*_tmp23D));_tmp23D->hd=({struct _tuple18*_tmp23E=_region_malloc(r,sizeof(*
_tmp23E));_tmp23E->f1=_tmp23A;_tmp23E->f2=(struct Cyc_List_List*)({struct Cyc_List_List*
_tmp23F=_region_malloc(r,sizeof(*_tmp23F));_tmp23F->hd=dsc;_tmp23F->tl=_tmp23B;
_tmp23F;});_tmp23E;});_tmp23D->tl=_tmp23C;_tmp23D;});_LL105:;}static struct Cyc_List_List*
Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){struct Cyc_List_List*
_tmp240=ctxt;struct Cyc_List_List _tmp241;struct _tuple18*_tmp242;struct _tuple18
_tmp243;struct Cyc_Tcpat_Con_s*_tmp244;struct Cyc_List_List*_tmp245;struct Cyc_List_List*
_tmp246;_LL10B: if(_tmp240 != 0)goto _LL10D;_LL10C:({void*_tmp247=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp248="norm_context: empty context";
_tag_dyneither(_tmp248,sizeof(char),28);}),_tag_dyneither(_tmp247,sizeof(void*),
0));});_LL10D: if(_tmp240 == 0)goto _LL10A;_tmp241=*_tmp240;_tmp242=(struct _tuple18*)
_tmp241.hd;_tmp243=*_tmp242;_tmp244=_tmp243.f1;_tmp245=_tmp243.f2;_tmp246=
_tmp241.tl;_LL10E: return Cyc_Tcpat_augment(r,_tmp246,(void*)({struct Cyc_Tcpat_Pos_struct*
_tmp249=_region_malloc(r,sizeof(*_tmp249));_tmp249[0]=({struct Cyc_Tcpat_Pos_struct
_tmp24A;_tmp24A.tag=0;_tmp24A.f1=_tmp244;_tmp24A.f2=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp245);_tmp24A;});
_tmp249;}));_LL10A:;}static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,
struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){struct _tuple12
_tmp24C=({struct _tuple12 _tmp24B;_tmp24B.f1=ctxt;_tmp24B.f2=work;_tmp24B;});
struct Cyc_List_List*_tmp24D;struct Cyc_List_List*_tmp24E;struct Cyc_List_List*
_tmp24F;struct Cyc_List_List*_tmp250;struct Cyc_List_List*_tmp251;struct Cyc_List_List
_tmp252;struct _tuple18*_tmp253;struct _tuple18 _tmp254;struct Cyc_Tcpat_Con_s*
_tmp255;struct Cyc_List_List*_tmp256;struct Cyc_List_List*_tmp257;struct Cyc_List_List*
_tmp258;struct Cyc_List_List _tmp259;struct _tuple17*_tmp25A;struct _tuple17 _tmp25B;
struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25D;_LL110: _tmp24D=_tmp24C.f1;
if(_tmp24D != 0)goto _LL112;_tmp24E=_tmp24C.f2;if(_tmp24E != 0)goto _LL112;_LL111:
return dsc;_LL112: _tmp24F=_tmp24C.f1;if(_tmp24F != 0)goto _LL114;_LL113: goto _LL115;
_LL114: _tmp250=_tmp24C.f2;if(_tmp250 != 0)goto _LL116;_LL115:({void*_tmp25E=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp25F="build_desc: ctxt and work don't match";_tag_dyneither(_tmp25F,
sizeof(char),38);}),_tag_dyneither(_tmp25E,sizeof(void*),0));});_LL116: _tmp251=
_tmp24C.f1;if(_tmp251 == 0)goto _LL10F;_tmp252=*_tmp251;_tmp253=(struct _tuple18*)
_tmp252.hd;_tmp254=*_tmp253;_tmp255=_tmp254.f1;_tmp256=_tmp254.f2;_tmp257=
_tmp252.tl;_tmp258=_tmp24C.f2;if(_tmp258 == 0)goto _LL10F;_tmp259=*_tmp258;_tmp25A=(
struct _tuple17*)_tmp259.hd;_tmp25B=*_tmp25A;_tmp25C=_tmp25B.f3;_tmp25D=_tmp259.tl;
_LL117: {struct Cyc_Tcpat_Pos_struct*_tmp260=({struct Cyc_Tcpat_Pos_struct*_tmp261=
_region_malloc(r,sizeof(*_tmp261));_tmp261[0]=({struct Cyc_Tcpat_Pos_struct
_tmp262;_tmp262.tag=0;_tmp262.f1=_tmp255;_tmp262.f2=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
r,_tmp256),({struct Cyc_List_List*_tmp263=_region_malloc(r,sizeof(*_tmp263));
_tmp263->hd=dsc;_tmp263->tl=_tmp25C;_tmp263;}));_tmp262;});_tmp261;});return Cyc_Tcpat_build_desc(
r,_tmp257,(void*)_tmp260,_tmp25D);}_LL10F:;}static void*Cyc_Tcpat_match(struct
_RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,
struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);static
void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*
allmrules){struct Cyc_List_List*_tmp264=allmrules;struct Cyc_List_List _tmp265;
struct _tuple0*_tmp266;struct _tuple0 _tmp267;void*_tmp268;void*_tmp269;struct Cyc_List_List*
_tmp26A;_LL119: if(_tmp264 != 0)goto _LL11B;_LL11A: return(void*)({struct Cyc_Tcpat_Failure_struct*
_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B[0]=({struct Cyc_Tcpat_Failure_struct
_tmp26C;_tmp26C.tag=0;_tmp26C.f1=(void*)dsc;_tmp26C;});_tmp26B;});_LL11B: if(
_tmp264 == 0)goto _LL118;_tmp265=*_tmp264;_tmp266=(struct _tuple0*)_tmp265.hd;
_tmp267=*_tmp266;_tmp268=_tmp267.f1;_tmp269=_tmp267.f2;_tmp26A=_tmp265.tl;_LL11C:
return Cyc_Tcpat_match(r,_tmp268,0,dsc,0,0,_tmp269,_tmp26A);_LL118:;}static void*
Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
return Cyc_Tcpat_or_match(r,(void*)({struct Cyc_Tcpat_Neg_struct*_tmp26D=
_region_malloc(r,sizeof(*_tmp26D));_tmp26D[0]=({struct Cyc_Tcpat_Neg_struct
_tmp26E;_tmp26E.tag=1;_tmp26E.f1=Cyc_Tcpat_empty_con_set(r);_tmp26E;});_tmp26D;}),
allmrules);}static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*
ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
struct Cyc_List_List*_tmp26F=work;struct Cyc_List_List _tmp270;struct _tuple17*
_tmp271;struct _tuple17 _tmp272;struct Cyc_List_List*_tmp273;struct Cyc_List_List*
_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_List_List*_tmp276;struct Cyc_List_List
_tmp277;struct _tuple17*_tmp278;struct _tuple17 _tmp279;struct Cyc_List_List*_tmp27A;
struct Cyc_List_List*_tmp27B;struct Cyc_List_List*_tmp27C;struct Cyc_List_List*
_tmp27D;_LL11E: if(_tmp26F != 0)goto _LL120;_LL11F: return(void*)({struct Cyc_Tcpat_Success_struct*
_tmp27E=_region_malloc(r,sizeof(*_tmp27E));_tmp27E[0]=({struct Cyc_Tcpat_Success_struct
_tmp27F;_tmp27F.tag=1;_tmp27F.f1=(void*)right_hand_side;_tmp27F;});_tmp27E;});
_LL120: if(_tmp26F == 0)goto _LL122;_tmp270=*_tmp26F;_tmp271=(struct _tuple17*)
_tmp270.hd;_tmp272=*_tmp271;_tmp273=_tmp272.f1;if(_tmp273 != 0)goto _LL122;_tmp274=
_tmp272.f2;if(_tmp274 != 0)goto _LL122;_tmp275=_tmp272.f3;if(_tmp275 != 0)goto
_LL122;_tmp276=_tmp270.tl;_LL121: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp276,right_hand_side,rules);_LL122: if(_tmp26F == 0)goto _LL11D;_tmp277=*
_tmp26F;_tmp278=(struct _tuple17*)_tmp277.hd;_tmp279=*_tmp278;_tmp27A=_tmp279.f1;
_tmp27B=_tmp279.f2;_tmp27C=_tmp279.f3;_tmp27D=_tmp277.tl;_LL123: if((_tmp27A == 0
 || _tmp27B == 0) || _tmp27C == 0)({void*_tmp280=0;Cyc_Tcutil_impos(({const char*
_tmp281="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp281,sizeof(
char),38);}),_tag_dyneither(_tmp280,sizeof(void*),0));});{struct Cyc_List_List
_tmp283;void*_tmp284;struct Cyc_List_List*_tmp285;struct Cyc_List_List*_tmp282=(
struct Cyc_List_List*)_tmp27A;_tmp283=*_tmp282;_tmp284=(void*)_tmp283.hd;_tmp285=
_tmp283.tl;{struct Cyc_List_List _tmp287;struct Cyc_List_List*_tmp288;struct Cyc_List_List*
_tmp289;struct Cyc_List_List*_tmp286=(struct Cyc_List_List*)_tmp27B;_tmp287=*
_tmp286;_tmp288=(struct Cyc_List_List*)_tmp287.hd;_tmp289=_tmp287.tl;{struct Cyc_List_List
_tmp28B;void*_tmp28C;struct Cyc_List_List*_tmp28D;struct Cyc_List_List*_tmp28A=(
struct Cyc_List_List*)_tmp27C;_tmp28B=*_tmp28A;_tmp28C=(void*)_tmp28B.hd;_tmp28D=
_tmp28B.tl;{struct _tuple17*_tmp28E=({struct _tuple17*_tmp290=_region_malloc(r,
sizeof(*_tmp290));_tmp290->f1=_tmp285;_tmp290->f2=_tmp289;_tmp290->f3=_tmp28D;
_tmp290;});return Cyc_Tcpat_match(r,_tmp284,_tmp288,_tmp28C,ctx,({struct Cyc_List_List*
_tmp28F=_region_malloc(r,sizeof(*_tmp28F));_tmp28F->hd=_tmp28E;_tmp28F->tl=
_tmp27D;_tmp28F;}),right_hand_side,rules);};};};};_LL11D:;}static struct Cyc_List_List*
Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp291=dsc;struct Cyc_Set_Set*_tmp293;struct Cyc_List_List*_tmp295;_LL125: {
struct Cyc_Tcpat_Neg_struct*_tmp292=(struct Cyc_Tcpat_Neg_struct*)_tmp291;if(
_tmp292->tag != 1)goto _LL127;else{_tmp293=_tmp292->f1;}}_LL126: {void*any=(void*)({
struct Cyc_Tcpat_Neg_struct*_tmp298=_region_malloc(r,sizeof(*_tmp298));_tmp298[0]=({
struct Cyc_Tcpat_Neg_struct _tmp299;_tmp299.tag=1;_tmp299.f1=Cyc_Tcpat_empty_con_set(
r);_tmp299;});_tmp298;});struct Cyc_List_List*_tmp296=0;{int i=0;for(0;i < pcon->arity;
++ i){_tmp296=({struct Cyc_List_List*_tmp297=_region_malloc(r,sizeof(*_tmp297));
_tmp297->hd=any;_tmp297->tl=_tmp296;_tmp297;});}}return _tmp296;}_LL127: {struct
Cyc_Tcpat_Pos_struct*_tmp294=(struct Cyc_Tcpat_Pos_struct*)_tmp291;if(_tmp294->tag
!= 0)goto _LL124;else{_tmp295=_tmp294->f2;}}_LL128: return _tmp295;_LL124:;}struct
_tuple19{struct _RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_getoarg(struct _tuple19*env,int i){struct _tuple19 _tmp29B;struct
_RegionHandle*_tmp29C;struct Cyc_List_List*_tmp29D;struct _tuple19*_tmp29A=env;
_tmp29B=*_tmp29A;_tmp29C=_tmp29B.f1;_tmp29D=_tmp29B.f2;return({struct Cyc_List_List*
_tmp29E=_region_malloc(_tmp29C,sizeof(*_tmp29E));_tmp29E->hd=(void*)(i + 1);
_tmp29E->tl=_tmp29D;_tmp29E;});}static struct Cyc_List_List*Cyc_Tcpat_getoargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){struct
_tuple19 _tmp29F=({struct _tuple19 _tmp2A0;_tmp2A0.f1=r;_tmp2A0.f2=obj;_tmp2A0;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple19*,int),struct _tuple19*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp29F);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp2A1=p;struct Cyc_Tcpat_Con_s*
_tmp2A4;struct Cyc_List_List*_tmp2A5;_LL12A: {struct Cyc_Tcpat_Any_struct*_tmp2A2=(
struct Cyc_Tcpat_Any_struct*)_tmp2A1;if(_tmp2A2->tag != 0)goto _LL12C;}_LL12B:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL12C: {struct Cyc_Tcpat_Con_struct*_tmp2A3=(struct Cyc_Tcpat_Con_struct*)
_tmp2A1;if(_tmp2A3->tag != 1)goto _LL129;else{_tmp2A4=_tmp2A3->f1;_tmp2A5=_tmp2A3->f2;}}
_LL12D: switch(Cyc_Tcpat_static_match(_tmp2A4,dsc)){case Cyc_Tcpat_Yes: _LL12E: {
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp2A8=_region_malloc(r,sizeof(*
_tmp2A8));_tmp2A8->hd=({struct _tuple18*_tmp2A9=_region_malloc(r,sizeof(*_tmp2A9));
_tmp2A9->f1=_tmp2A4;_tmp2A9->f2=0;_tmp2A9;});_tmp2A8->tl=ctx;_tmp2A8;});struct
_tuple17*work_frame=({struct _tuple17*_tmp2A7=_region_malloc(r,sizeof(*_tmp2A7));
_tmp2A7->f1=_tmp2A5;_tmp2A7->f2=Cyc_Tcpat_getoargs(r,_tmp2A4,obj);_tmp2A7->f3=
Cyc_Tcpat_getdargs(r,_tmp2A4,dsc);_tmp2A7;});struct Cyc_List_List*work2=({struct
Cyc_List_List*_tmp2A6=_region_malloc(r,sizeof(*_tmp2A6));_tmp2A6->hd=work_frame;
_tmp2A6->tl=work;_tmp2A6;});return Cyc_Tcpat_and_match(r,ctx2,work2,
right_hand_side,rules);}case Cyc_Tcpat_No: _LL12F: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(
r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL130: {struct Cyc_List_List*ctx2=({
struct Cyc_List_List*_tmp2B0=_region_malloc(r,sizeof(*_tmp2B0));_tmp2B0->hd=({
struct _tuple18*_tmp2B1=_region_malloc(r,sizeof(*_tmp2B1));_tmp2B1->f1=_tmp2A4;
_tmp2B1->f2=0;_tmp2B1;});_tmp2B0->tl=ctx;_tmp2B0;});struct _tuple17*work_frame=({
struct _tuple17*_tmp2AF=_region_malloc(r,sizeof(*_tmp2AF));_tmp2AF->f1=_tmp2A5;
_tmp2AF->f2=Cyc_Tcpat_getoargs(r,_tmp2A4,obj);_tmp2AF->f3=Cyc_Tcpat_getdargs(r,
_tmp2A4,dsc);_tmp2AF;});struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp2AE=
_region_malloc(r,sizeof(*_tmp2AE));_tmp2AE->hd=work_frame;_tmp2AE->tl=work;
_tmp2AE;});void*_tmp2AA=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp2AB=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,
dsc,_tmp2A4),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp2AC=
_region_malloc(r,sizeof(*_tmp2AC));_tmp2AC[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp2AD;_tmp2AD.tag=2;_tmp2AD.f1=obj;_tmp2AD.f2=_tmp2A4;_tmp2AD.f3=(void*)
_tmp2AA;_tmp2AD.f4=(void*)_tmp2AB;_tmp2AD;});_tmp2AC;});}}_LL129:;}static void Cyc_Tcpat_check_exhaust_overlap(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),
void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){void*
_tmp2B2=d;void*_tmp2B4;void*_tmp2B6;void*_tmp2B8;void*_tmp2B9;_LL133: {struct Cyc_Tcpat_Failure_struct*
_tmp2B3=(struct Cyc_Tcpat_Failure_struct*)_tmp2B2;if(_tmp2B3->tag != 0)goto _LL135;
else{_tmp2B4=(void*)_tmp2B3->f1;}}_LL134: if(!(*exhaust_done)){not_exhaust(r,env1,
_tmp2B4);*exhaust_done=1;}goto _LL132;_LL135: {struct Cyc_Tcpat_Success_struct*
_tmp2B5=(struct Cyc_Tcpat_Success_struct*)_tmp2B2;if(_tmp2B5->tag != 1)goto _LL137;
else{_tmp2B6=(void*)_tmp2B5->f1;}}_LL136: rhs_appears(env2,_tmp2B6);goto _LL132;
_LL137: {struct Cyc_Tcpat_IfEq_struct*_tmp2B7=(struct Cyc_Tcpat_IfEq_struct*)
_tmp2B2;if(_tmp2B7->tag != 2)goto _LL132;else{_tmp2B8=(void*)_tmp2B7->f3;_tmp2B9=(
void*)_tmp2B7->f4;}}_LL138: Cyc_Tcpat_check_exhaust_overlap(r,_tmp2B8,not_exhaust,
env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(r,_tmp2B9,
not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL132;_LL132:;}struct
_tuple20{int f1;struct Cyc_Position_Segment*f2;};struct _tuple21{void*f1;struct
_tuple20*f2;};struct _tuple22{struct _RegionHandle*f1;int*f2;};static struct
_tuple21*Cyc_Tcpat_get_match(struct _tuple22*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple22 _tmp2BB;struct _RegionHandle*_tmp2BC;int*_tmp2BD;struct
_tuple22*_tmp2BA=env;_tmp2BB=*_tmp2BA;_tmp2BC=_tmp2BB.f1;_tmp2BD=_tmp2BB.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp2BC,swc->pattern);struct _tuple20*rhs=({struct
_tuple20*_tmp2C5=_region_malloc(_tmp2BC,sizeof(*_tmp2C5));_tmp2C5->f1=0;_tmp2C5->f2=(
swc->pattern)->loc;_tmp2C5;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(
_tmp2BC,({struct Cyc_List_List*_tmp2BE=_region_malloc(_tmp2BC,sizeof(*_tmp2BE));
_tmp2BE->hd=sp0;_tmp2BE->tl=({struct Cyc_List_List*_tmp2BF=_region_malloc(_tmp2BC,
sizeof(*_tmp2BF));_tmp2BF->hd=Cyc_Tcpat_int_pat(_tmp2BC,*_tmp2BD,0);_tmp2BF->tl=
0;_tmp2BF;});_tmp2BE;}),0);*_tmp2BD=*_tmp2BD + 1;}else{sp=Cyc_Tcpat_tuple_pat(
_tmp2BC,({struct Cyc_List_List*_tmp2C0=_region_malloc(_tmp2BC,sizeof(*_tmp2C0));
_tmp2C0->hd=sp0;_tmp2C0->tl=({struct Cyc_List_List*_tmp2C1=_region_malloc(_tmp2BC,
sizeof(*_tmp2C1));_tmp2C1->hd=(void*)({struct Cyc_Tcpat_Any_struct*_tmp2C2=
_region_malloc(_tmp2BC,sizeof(*_tmp2C2));_tmp2C2[0]=({struct Cyc_Tcpat_Any_struct
_tmp2C3;_tmp2C3.tag=0;_tmp2C3;});_tmp2C2;});_tmp2C1->tl=0;_tmp2C1;});_tmp2C0;}),
0);}return({struct _tuple21*_tmp2C4=_region_malloc(_tmp2BC,sizeof(*_tmp2C4));
_tmp2C4->f1=sp;_tmp2C4->f2=rhs;_tmp2C4;});};}char Cyc_Tcpat_Desc2string[12]="Desc2string";
struct Cyc_Tcpat_Desc2string_struct{char*tag;};struct Cyc_Tcpat_Desc2string_struct
Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};static struct _dyneither_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*);static struct _dyneither_ptr Cyc_Tcpat_desc2string(
struct _RegionHandle*r,void*d){void*_tmp2C7=d;struct Cyc_Tcpat_Con_s*_tmp2C9;
struct Cyc_List_List*_tmp2CA;struct Cyc_Set_Set*_tmp2CC;_LL13A: {struct Cyc_Tcpat_Pos_struct*
_tmp2C8=(struct Cyc_Tcpat_Pos_struct*)_tmp2C7;if(_tmp2C8->tag != 0)goto _LL13C;
else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C8->f2;}}_LL13B: {union Cyc_Tcpat_Name_value
_tmp2CD=_tmp2C9->name;struct Cyc_Absyn_Pat*_tmp2CE=_tmp2C9->orig_pat;if(_tmp2CE == 
0)return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(
_tmp2CA))->hd);{void*_tmp2CF=_tmp2CE->r;struct Cyc_Absyn_Vardecl*_tmp2D2;struct
Cyc_Absyn_Vardecl*_tmp2D4;struct Cyc_Absyn_Tvar*_tmp2D6;struct Cyc_Absyn_Vardecl*
_tmp2D7;struct Cyc_Absyn_Pat*_tmp2DA;struct Cyc_Absyn_Pat _tmp2DB;void*_tmp2DC;
struct Cyc_Absyn_Datatypefield*_tmp2DE;struct Cyc_Absyn_AggrInfo*_tmp2E1;struct Cyc_Absyn_AggrInfo
_tmp2E2;union Cyc_Absyn_AggrInfoU _tmp2E3;struct Cyc_Absyn_Aggrdecl**_tmp2E4;struct
Cyc_Absyn_Aggrdecl*_tmp2E5;struct Cyc_List_List*_tmp2E6;struct Cyc_Absyn_Datatypefield*
_tmp2E8;int _tmp2EB;char _tmp2ED;struct _dyneither_ptr _tmp2EF;struct Cyc_Absyn_Enumfield*
_tmp2F1;struct Cyc_Absyn_Enumfield*_tmp2F3;struct Cyc_Absyn_Exp*_tmp2F5;_LL13F: {
struct Cyc_Absyn_Wild_p_struct*_tmp2D0=(struct Cyc_Absyn_Wild_p_struct*)_tmp2CF;
if(_tmp2D0->tag != 0)goto _LL141;}_LL140: return({const char*_tmp2F6="_";
_tag_dyneither(_tmp2F6,sizeof(char),2);});_LL141: {struct Cyc_Absyn_Var_p_struct*
_tmp2D1=(struct Cyc_Absyn_Var_p_struct*)_tmp2CF;if(_tmp2D1->tag != 1)goto _LL143;
else{_tmp2D2=_tmp2D1->f1;}}_LL142: return Cyc_Absynpp_qvar2string(_tmp2D2->name);
_LL143: {struct Cyc_Absyn_Reference_p_struct*_tmp2D3=(struct Cyc_Absyn_Reference_p_struct*)
_tmp2CF;if(_tmp2D3->tag != 2)goto _LL145;else{_tmp2D4=_tmp2D3->f1;}}_LL144: return(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2F9;_tmp2F9.tag=0;_tmp2F9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D4->name));({
void*_tmp2F7[1]={& _tmp2F9};Cyc_aprintf(({const char*_tmp2F8="*%s";_tag_dyneither(
_tmp2F8,sizeof(char),4);}),_tag_dyneither(_tmp2F7,sizeof(void*),1));});});_LL145: {
struct Cyc_Absyn_TagInt_p_struct*_tmp2D5=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp2CF;if(_tmp2D5->tag != 3)goto _LL147;else{_tmp2D6=_tmp2D5->f1;_tmp2D7=_tmp2D5->f2;}}
_LL146: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2FD;_tmp2FD.tag=
0;_tmp2FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2D6->name);({
struct Cyc_String_pa_struct _tmp2FC;_tmp2FC.tag=0;_tmp2FC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2D7->name));({void*_tmp2FA[2]={&
_tmp2FC,& _tmp2FD};Cyc_aprintf(({const char*_tmp2FB="%s<`%s>";_tag_dyneither(
_tmp2FB,sizeof(char),8);}),_tag_dyneither(_tmp2FA,sizeof(void*),2));});});});
_LL147: {struct Cyc_Absyn_Tuple_p_struct*_tmp2D8=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp2CF;if(_tmp2D8->tag != 4)goto _LL149;}_LL148: return(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp300;_tmp300.tag=0;_tmp300.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CA));({void*_tmp2FE[1]={&
_tmp300};Cyc_aprintf(({const char*_tmp2FF="$(%s)";_tag_dyneither(_tmp2FF,sizeof(
char),6);}),_tag_dyneither(_tmp2FE,sizeof(void*),1));});});_LL149: {struct Cyc_Absyn_Pointer_p_struct*
_tmp2D9=(struct Cyc_Absyn_Pointer_p_struct*)_tmp2CF;if(_tmp2D9->tag != 5)goto
_LL14B;else{_tmp2DA=_tmp2D9->f1;_tmp2DB=*_tmp2DA;_tmp2DC=_tmp2DB.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp2DD=(struct Cyc_Absyn_Datatype_p_struct*)_tmp2DC;if(_tmp2DD->tag != 7)goto
_LL14B;else{_tmp2DE=_tmp2DD->f2;}};}}_LL14A: return(struct _dyneither_ptr)({struct
Cyc_String_pa_struct _tmp304;_tmp304.tag=0;_tmp304.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CA));({struct Cyc_String_pa_struct
_tmp303;_tmp303.tag=0;_tmp303.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2DE->name));({void*_tmp301[2]={& _tmp303,& _tmp304};
Cyc_aprintf(({const char*_tmp302="&%s(%s)";_tag_dyneither(_tmp302,sizeof(char),8);}),
_tag_dyneither(_tmp301,sizeof(void*),2));});});});_LL14B: {struct Cyc_Absyn_Pointer_p_struct*
_tmp2DF=(struct Cyc_Absyn_Pointer_p_struct*)_tmp2CF;if(_tmp2DF->tag != 5)goto
_LL14D;}_LL14C: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp307;
_tmp307.tag=0;_tmp307.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2CA));({void*_tmp305[1]={& _tmp307};Cyc_aprintf(({const char*_tmp306="&%s";
_tag_dyneither(_tmp306,sizeof(char),4);}),_tag_dyneither(_tmp305,sizeof(void*),1));});});
_LL14D: {struct Cyc_Absyn_Aggr_p_struct*_tmp2E0=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp2CF;if(_tmp2E0->tag != 6)goto _LL14F;else{_tmp2E1=_tmp2E0->f1;if(_tmp2E1 == 0)
goto _LL14F;_tmp2E2=*_tmp2E1;_tmp2E3=_tmp2E2.aggr_info;if((_tmp2E3.KnownAggr).tag
!= 2)goto _LL14F;_tmp2E4=(struct Cyc_Absyn_Aggrdecl**)(_tmp2E3.KnownAggr).val;
_tmp2E5=*_tmp2E4;_tmp2E6=_tmp2E0->f3;}}_LL14E: if(_tmp2E5->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp308=_tmp2E6;struct Cyc_List_List _tmp309;struct _tuple14*
_tmp30A;struct _tuple14 _tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_List_List
_tmp30D;void*_tmp30E;struct _dyneither_ptr*_tmp310;_LL162: if(_tmp308 == 0)goto
_LL164;_tmp309=*_tmp308;_tmp30A=(struct _tuple14*)_tmp309.hd;_tmp30B=*_tmp30A;
_tmp30C=_tmp30B.f1;if(_tmp30C == 0)goto _LL164;_tmp30D=*_tmp30C;_tmp30E=(void*)
_tmp30D.hd;{struct Cyc_Absyn_FieldName_struct*_tmp30F=(struct Cyc_Absyn_FieldName_struct*)
_tmp30E;if(_tmp30F->tag != 1)goto _LL164;else{_tmp310=_tmp30F->f1;}};_LL163: return(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp315;_tmp315.tag=0;_tmp315.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp2CA));({
struct Cyc_String_pa_struct _tmp314;_tmp314.tag=0;_tmp314.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp310);({struct Cyc_String_pa_struct _tmp313;_tmp313.tag=0;
_tmp313.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp2E5->name));({void*_tmp311[3]={& _tmp313,& _tmp314,& _tmp315};Cyc_aprintf(({
const char*_tmp312="%s{.%s = %s}";_tag_dyneither(_tmp312,sizeof(char),13);}),
_tag_dyneither(_tmp311,sizeof(void*),3));});});});});_LL164:;_LL165: goto _LL161;
_LL161:;}return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp319;_tmp319.tag=
0;_tmp319.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp2CA));({struct Cyc_String_pa_struct _tmp318;_tmp318.tag=0;_tmp318.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2E5->name));({
void*_tmp316[2]={& _tmp318,& _tmp319};Cyc_aprintf(({const char*_tmp317="%s{%s}";
_tag_dyneither(_tmp317,sizeof(char),7);}),_tag_dyneither(_tmp316,sizeof(void*),2));});});});
_LL14F: {struct Cyc_Absyn_Datatype_p_struct*_tmp2E7=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp2CF;if(_tmp2E7->tag != 7)goto _LL151;else{_tmp2E8=_tmp2E7->f2;}}_LL150: return(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp31C;_tmp31C.tag=0;_tmp31C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2E8->name));({
void*_tmp31A[1]={& _tmp31C};Cyc_aprintf(({const char*_tmp31B="%s";_tag_dyneither(
_tmp31B,sizeof(char),3);}),_tag_dyneither(_tmp31A,sizeof(void*),1));});});_LL151: {
struct Cyc_Absyn_Null_p_struct*_tmp2E9=(struct Cyc_Absyn_Null_p_struct*)_tmp2CF;
if(_tmp2E9->tag != 8)goto _LL153;}_LL152: return({const char*_tmp31D="NULL";
_tag_dyneither(_tmp31D,sizeof(char),5);});_LL153: {struct Cyc_Absyn_Int_p_struct*
_tmp2EA=(struct Cyc_Absyn_Int_p_struct*)_tmp2CF;if(_tmp2EA->tag != 9)goto _LL155;
else{_tmp2EB=_tmp2EA->f2;}}_LL154: return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp320;_tmp320.tag=1;_tmp320.f1=(unsigned long)_tmp2EB;({void*_tmp31E[1]={&
_tmp320};Cyc_aprintf(({const char*_tmp31F="%d";_tag_dyneither(_tmp31F,sizeof(char),
3);}),_tag_dyneither(_tmp31E,sizeof(void*),1));});});_LL155: {struct Cyc_Absyn_Char_p_struct*
_tmp2EC=(struct Cyc_Absyn_Char_p_struct*)_tmp2CF;if(_tmp2EC->tag != 10)goto _LL157;
else{_tmp2ED=_tmp2EC->f1;}}_LL156: return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp323;_tmp323.tag=1;_tmp323.f1=(unsigned long)((int)_tmp2ED);({void*_tmp321[1]={&
_tmp323};Cyc_aprintf(({const char*_tmp322="%d";_tag_dyneither(_tmp322,sizeof(char),
3);}),_tag_dyneither(_tmp321,sizeof(void*),1));});});_LL157: {struct Cyc_Absyn_Float_p_struct*
_tmp2EE=(struct Cyc_Absyn_Float_p_struct*)_tmp2CF;if(_tmp2EE->tag != 11)goto _LL159;
else{_tmp2EF=_tmp2EE->f1;}}_LL158: return _tmp2EF;_LL159: {struct Cyc_Absyn_Enum_p_struct*
_tmp2F0=(struct Cyc_Absyn_Enum_p_struct*)_tmp2CF;if(_tmp2F0->tag != 12)goto _LL15B;
else{_tmp2F1=_tmp2F0->f2;}}_LL15A: _tmp2F3=_tmp2F1;goto _LL15C;_LL15B: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp2F2=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp2CF;if(_tmp2F2->tag != 13)goto
_LL15D;else{_tmp2F3=_tmp2F2->f2;}}_LL15C: return Cyc_Absynpp_qvar2string(_tmp2F3->name);
_LL15D: {struct Cyc_Absyn_Exp_p_struct*_tmp2F4=(struct Cyc_Absyn_Exp_p_struct*)
_tmp2CF;if(_tmp2F4->tag != 16)goto _LL15F;else{_tmp2F5=_tmp2F4->f1;}}_LL15E: return
Cyc_Absynpp_exp2string(_tmp2F5);_LL15F:;_LL160:(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);
_LL13E:;};}_LL13C: {struct Cyc_Tcpat_Neg_struct*_tmp2CB=(struct Cyc_Tcpat_Neg_struct*)
_tmp2C7;if(_tmp2CB->tag != 1)goto _LL139;else{_tmp2CC=_tmp2CB->f1;}}_LL13D: if(((
int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp2CC))return({const char*_tmp324="_";
_tag_dyneither(_tmp324,sizeof(char),2);});{struct Cyc_Tcpat_Con_s*_tmp325=((
struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp2CC);struct
Cyc_Absyn_Pat*_tmp326=_tmp325->orig_pat;if(_tmp326 == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_tmp327=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp326->topt))->v);enum Cyc_Absyn_Size_of _tmp329;struct Cyc_Absyn_PtrInfo _tmp32C;
struct Cyc_Absyn_PtrAtts _tmp32D;struct Cyc_Absyn_DatatypeInfo _tmp32F;union Cyc_Absyn_DatatypeInfoU
_tmp330;struct Cyc_Absyn_Datatypedecl**_tmp331;struct Cyc_Absyn_Datatypedecl*
_tmp332;struct Cyc_Absyn_AggrInfo _tmp334;union Cyc_Absyn_AggrInfoU _tmp335;struct
Cyc_Absyn_Aggrdecl**_tmp336;struct Cyc_Absyn_Aggrdecl*_tmp337;_LL167: {struct Cyc_Absyn_IntType_struct*
_tmp328=(struct Cyc_Absyn_IntType_struct*)_tmp327;if(_tmp328->tag != 6)goto _LL169;
else{_tmp329=_tmp328->f2;if(_tmp329 != Cyc_Absyn_Char_sz)goto _LL169;}}_LL168:{int
i=0;for(0;i < 256;++ i){struct Cyc_Tcpat_Con_s*_tmp338=Cyc_Tcpat_char_con(r,(char)i,(
struct Cyc_Absyn_Pat*)_tmp326);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp2CC,_tmp338))return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp33B;_tmp33B.tag=1;_tmp33B.f1=(unsigned long)i;({void*_tmp339[1]={& _tmp33B};
Cyc_aprintf(({const char*_tmp33A="%d";_tag_dyneither(_tmp33A,sizeof(char),3);}),
_tag_dyneither(_tmp339,sizeof(void*),1));});});}}(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);
_LL169: {struct Cyc_Absyn_IntType_struct*_tmp32A=(struct Cyc_Absyn_IntType_struct*)
_tmp327;if(_tmp32A->tag != 6)goto _LL16B;}_LL16A:{unsigned int i=0;for(0;i < 0 - 1;++
i){struct Cyc_Tcpat_Con_s*_tmp33C=Cyc_Tcpat_int_con(r,(int)i,_tmp326);if(!((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2CC,_tmp33C))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp33F;_tmp33F.tag=1;
_tmp33F.f1=(unsigned long)((int)i);({void*_tmp33D[1]={& _tmp33F};Cyc_aprintf(({
const char*_tmp33E="%d";_tag_dyneither(_tmp33E,sizeof(char),3);}),_tag_dyneither(
_tmp33D,sizeof(void*),1));});});}}(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);
_LL16B: {struct Cyc_Absyn_PointerType_struct*_tmp32B=(struct Cyc_Absyn_PointerType_struct*)
_tmp327;if(_tmp32B->tag != 5)goto _LL16D;else{_tmp32C=_tmp32B->f1;_tmp32D=_tmp32C.ptr_atts;}}
_LL16C: {union Cyc_Absyn_Constraint*_tmp340=_tmp32D.nullable;int is_nullable=((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp340);if(
is_nullable){if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2CC,Cyc_Tcpat_null_con(r,(struct Cyc_Absyn_Pat*)_tmp326)))return({const char*
_tmp341="NULL";_tag_dyneither(_tmp341,sizeof(char),5);});}return({const char*
_tmp342="&_";_tag_dyneither(_tmp342,sizeof(char),3);});}_LL16D: {struct Cyc_Absyn_DatatypeType_struct*
_tmp32E=(struct Cyc_Absyn_DatatypeType_struct*)_tmp327;if(_tmp32E->tag != 3)goto
_LL16F;else{_tmp32F=_tmp32E->f1;_tmp330=_tmp32F.datatype_info;if((_tmp330.KnownDatatype).tag
!= 2)goto _LL16F;_tmp331=(struct Cyc_Absyn_Datatypedecl**)(_tmp330.KnownDatatype).val;
_tmp332=*_tmp331;}}_LL16E: if(_tmp332->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp343=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp332->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp343);for(0;(unsigned int)_tmp343;_tmp343=_tmp343->tl){struct _dyneither_ptr n=*(*((
struct Cyc_Absyn_Datatypefield*)_tmp343->hd)->name).f2;struct Cyc_List_List*
_tmp344=((struct Cyc_Absyn_Datatypefield*)_tmp343->hd)->typs;if(!((int(*)(struct
Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2CC,(struct Cyc_Tcpat_Con_s*)({
struct Cyc_Tcpat_Con_s*_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345->name=Cyc_Tcpat_Name_v(
n);_tmp345->arity=0;_tmp345->span=0;_tmp345->orig_pat=_tmp326;_tmp345;}))){if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp344)== 0)return n;else{return(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp348;_tmp348.tag=0;_tmp348.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)n);({void*_tmp346[1]={& _tmp348};Cyc_aprintf(({
const char*_tmp347="&%s(...)";_tag_dyneither(_tmp347,sizeof(char),9);}),
_tag_dyneither(_tmp346,sizeof(void*),1));});});}}}(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};
_LL16F:{struct Cyc_Absyn_AggrType_struct*_tmp333=(struct Cyc_Absyn_AggrType_struct*)
_tmp327;if(_tmp333->tag != 12)goto _LL171;else{_tmp334=_tmp333->f1;_tmp335=_tmp334.aggr_info;
if((_tmp335.KnownAggr).tag != 2)goto _LL171;_tmp336=(struct Cyc_Absyn_Aggrdecl**)(
_tmp335.KnownAggr).val;_tmp337=*_tmp336;}}if(!(_tmp337->kind == Cyc_Absyn_UnionA))
goto _LL171;_LL170: {struct Cyc_List_List*_tmp349=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp337->impl))->fields;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp349);struct _tuple1 _tmp34B;struct _dyneither_ptr*_tmp34C;struct _dyneither_ptr
_tmp34D;struct _tuple1*_tmp34A=_tmp337->name;_tmp34B=*_tmp34A;_tmp34C=_tmp34B.f2;
_tmp34D=*_tmp34C;for(0;(unsigned int)_tmp349;_tmp349=_tmp349->tl){struct
_dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp349->hd)->name;if(!((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2CC,(struct
Cyc_Tcpat_Con_s*)({struct Cyc_Tcpat_Con_s*_tmp34E=_cycalloc(sizeof(*_tmp34E));
_tmp34E->name=Cyc_Tcpat_Name_v(n);_tmp34E->arity=0;_tmp34E->span=0;_tmp34E->orig_pat=
_tmp326;_tmp34E;})))return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp352;_tmp352.tag=0;_tmp352.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({
struct Cyc_String_pa_struct _tmp351;_tmp351.tag=0;_tmp351.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp34D);({void*_tmp34F[2]={& _tmp351,& _tmp352};Cyc_aprintf(({
const char*_tmp350="%s{.%s = _}";_tag_dyneither(_tmp350,sizeof(char),12);}),
_tag_dyneither(_tmp34F,sizeof(void*),2));});});});}(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}
_LL171:;_LL172:(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL166:;};};_LL139:;}
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
return({struct _dyneither_ptr*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353[0]=Cyc_Tcpat_desc2string(
r,d);_tmp353;});}static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct
_RegionHandle*r,struct Cyc_List_List*ds){struct Cyc_List_List*_tmp354=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,
r,ds);struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),",",sizeof(char),2);{struct Cyc_List_List*_tmp355=_tmp354;for(0;
_tmp355 != 0;_tmp355=((struct Cyc_List_List*)_check_null(_tmp355))->tl){if(_tmp355->tl
!= 0){_tmp355->tl=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp356=_cycalloc(
sizeof(*_tmp356));_tmp356->hd=comma;_tmp356->tl=_tmp355->tl;_tmp356;});_tmp355=
_tmp355->tl;}}}return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp354);}static void
Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
void*desc){struct _handler_cons _tmp358;_push_handler(& _tmp358);{int _tmp35A=0;if(
setjmp(_tmp358.handler))_tmp35A=1;if(!_tmp35A){{struct _dyneither_ptr _tmp35B=Cyc_Tcpat_desc2string(
r,desc);({struct Cyc_String_pa_struct _tmp35E;_tmp35E.tag=0;_tmp35E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp35B);({void*_tmp35C[1]={& _tmp35E};Cyc_Tcutil_terr(
loc,({const char*_tmp35D="patterns may not be exhaustive.\n\tmissing case for %s";
_tag_dyneither(_tmp35D,sizeof(char),53);}),_tag_dyneither(_tmp35C,sizeof(void*),
1));});});};_pop_handler();}else{void*_tmp359=(void*)_exn_thrown;void*_tmp360=
_tmp359;_LL174: {struct Cyc_Tcpat_Desc2string_struct*_tmp361=(struct Cyc_Tcpat_Desc2string_struct*)
_tmp360;if(_tmp361->tag != Cyc_Tcpat_Desc2string)goto _LL176;}_LL175:({void*
_tmp362=0;Cyc_Tcutil_terr(loc,({const char*_tmp363="patterns may not be exhaustive.";
_tag_dyneither(_tmp363,sizeof(char),32);}),_tag_dyneither(_tmp362,sizeof(void*),
0));});goto _LL173;_LL176:;_LL177:(void)_throw(_tmp360);_LL173:;}};}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple20*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp364=Cyc_Tcenv_get_fnrgn(te);int _tmp365=0;struct _tuple22
_tmp366=({struct _tuple22 _tmp372;_tmp372.f1=_tmp364;_tmp372.f2=& _tmp365;_tmp372;});
struct Cyc_List_List*_tmp367=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple21*(*f)(struct _tuple22*,struct Cyc_Absyn_Switch_clause*),struct
_tuple22*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp364,Cyc_Tcpat_get_match,&
_tmp366,swcs);void*_tmp368=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(_tmp364,_tmp367);int _tmp369=0;((void(*)(
struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple20*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp364,_tmp368,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp369);for(0;_tmp367 != 0;_tmp367=_tmp367->tl){
struct _tuple21 _tmp36B;struct _tuple20*_tmp36C;struct _tuple20 _tmp36D;int _tmp36E;
struct Cyc_Position_Segment*_tmp36F;struct _tuple21*_tmp36A=(struct _tuple21*)
_tmp367->hd;_tmp36B=*_tmp36A;_tmp36C=_tmp36B.f2;_tmp36D=*_tmp36C;_tmp36E=_tmp36D.f1;
_tmp36F=_tmp36D.f2;if(!_tmp36E)({void*_tmp370=0;Cyc_Tcutil_terr(_tmp36F,({const
char*_tmp371="redundant pattern";_tag_dyneither(_tmp371,sizeof(char),18);}),
_tag_dyneither(_tmp370,sizeof(void*),0));});}}struct _tuple23{struct Cyc_Position_Segment*
f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct
_tuple23*pr,void*desc){(*pr).f2=0;{struct _handler_cons _tmp373;_push_handler(&
_tmp373);{int _tmp375=0;if(setjmp(_tmp373.handler))_tmp375=1;if(!_tmp375){{struct
_dyneither_ptr _tmp376=Cyc_Tcpat_desc2string(r,desc);({struct Cyc_String_pa_struct
_tmp379;_tmp379.tag=0;_tmp379.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp376);({void*_tmp377[1]={& _tmp379};Cyc_Tcutil_warn((*pr).f1,({const char*
_tmp378="pattern not exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp378,
sizeof(char),45);}),_tag_dyneither(_tmp377,sizeof(void*),1));});});};
_pop_handler();}else{void*_tmp374=(void*)_exn_thrown;void*_tmp37B=_tmp374;_LL179: {
struct Cyc_Tcpat_Desc2string_struct*_tmp37C=(struct Cyc_Tcpat_Desc2string_struct*)
_tmp37B;if(_tmp37C->tag != Cyc_Tcpat_Desc2string)goto _LL17B;}_LL17A:({void*
_tmp37D=0;Cyc_Tcutil_warn((*pr).f1,({const char*_tmp37E="pattern not exhaustive.";
_tag_dyneither(_tmp37E,sizeof(char),24);}),_tag_dyneither(_tmp37D,sizeof(void*),
0));});goto _LL178;_LL17B:;_LL17C:(void)_throw(_tmp37B);_LL178:;}};};}static void
Cyc_Tcpat_dummy_fn(int i,int j){return;}struct _tuple24{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle*_tmp37F=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp380=({
struct Cyc_List_List*_tmp385=_region_malloc(_tmp37F,sizeof(*_tmp385));_tmp385->hd=({
struct _tuple24*_tmp386=_region_malloc(_tmp37F,sizeof(*_tmp386));_tmp386->f1=Cyc_Tcpat_compile_pat(
_tmp37F,p);_tmp386->f2=0;_tmp386;});_tmp385->tl=0;_tmp385;});void*_tmp381=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(
_tmp37F,_tmp380);struct _tuple23 _tmp382=({struct _tuple23 _tmp384;_tmp384.f1=loc;
_tmp384.f2=1;_tmp384;});int _tmp383=0;((void(*)(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,struct _tuple23*,void*),struct _tuple23*env1,
void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
_tmp37F,_tmp381,Cyc_Tcpat_not_exhaust_warn,& _tmp382,Cyc_Tcpat_dummy_fn,0,&
_tmp383);return _tmp382.f2;}static struct _tuple21*Cyc_Tcpat_get_match2(struct
_RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){void*sp0=Cyc_Tcpat_compile_pat(
r,swc->pattern);struct _tuple20*rhs=({struct _tuple20*_tmp388=_region_malloc(r,
sizeof(*_tmp388));_tmp388->f1=0;_tmp388->f2=(swc->pattern)->loc;_tmp388;});
return({struct _tuple21*_tmp387=_region_malloc(r,sizeof(*_tmp387));_tmp387->f1=
sp0;_tmp387->f2=rhs;_tmp387;});}static void Cyc_Tcpat_not_exhaust_err2(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
struct _RegionHandle*_tmp389=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*_tmp38A=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple21*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(_tmp389,Cyc_Tcpat_get_match2,_tmp389,swcs);void*_tmp38B=((
void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(
_tmp389,_tmp38A);int _tmp38C=0;((void(*)(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env1,void(*rhs_appears)(int,struct _tuple20*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
_tmp389,_tmp38B,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp38C);
for(0;_tmp38A != 0;_tmp38A=_tmp38A->tl){struct _tuple21 _tmp38E;struct _tuple20*
_tmp38F;struct _tuple20 _tmp390;int _tmp391;struct Cyc_Position_Segment*_tmp392;
struct _tuple21*_tmp38D=(struct _tuple21*)_tmp38A->hd;_tmp38E=*_tmp38D;_tmp38F=
_tmp38E.f2;_tmp390=*_tmp38F;_tmp391=_tmp390.f1;_tmp392=_tmp390.f2;if(!_tmp391)({
void*_tmp393=0;Cyc_Tcutil_terr(_tmp392,({const char*_tmp394="redundant pattern";
_tag_dyneither(_tmp394,sizeof(char),18);}),_tag_dyneither(_tmp393,sizeof(void*),
0));});}}
