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
struct Cyc_Core_NewRegion Cyc_Core_new_dynregion();extern char Cyc_Core_Open_Region[
12];struct Cyc_Core_Open_Region_struct{char*tag;};extern char Cyc_Core_Free_Region[
12];struct Cyc_Core_Free_Region_struct{char*tag;};void Cyc_Core_free_dynregion(
struct _DynRegionHandle*);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};int Cyc_vfprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;
};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};void
Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct
_dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,
int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,
struct Cyc_Set_Set*s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));struct Cyc_Dict_Dict
Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct _tuple0{
void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
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
x,struct _tuple14*y);int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple1*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple14 _tmp0=*x;_tmp1=_tmp0.f1;
_tmp2=_tmp0.f2;{struct _tuple1*_tmp4;struct _dyneither_ptr _tmp5;struct _tuple14
_tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);if(
i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(struct _dyneither_ptr)
_tmp5);}}}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};void*Cyc_Toc_aggrdecl_type(
struct _tuple1*q,void*(*type_maker)(struct _tuple1*));void*Cyc_Toc_aggrdecl_type(
struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){struct _DynRegionHandle*_tmp7;
struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct
_DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct
_tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*
_tmp8,q);if(v == 0){void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
struct _tuple15 _tmpC;void*_tmpD;struct _tuple15*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;}}};_pop_dynregion(d);}}static int
Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=
0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap);static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA21;void*_tmpA20;(_tmpA20=0,Cyc_fprintf(Cyc_stderr,((
_tmpA21="\n",_tag_dyneither(_tmpA21,sizeof(char),2))),_tag_dyneither(_tmpA20,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Unimplemented_struct
_tmpA24;struct Cyc_Toc_Toc_Unimplemented_struct*_tmpA23;(int)_throw((void*)((
_tmpA23=_cycalloc_atomic(sizeof(*_tmpA23)),((_tmpA23[0]=((_tmpA24.tag=Cyc_Toc_Toc_Unimplemented,
_tmpA24)),_tmpA23)))));}}static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA27;void*
_tmpA26;(_tmpA26=0,Cyc_fprintf(Cyc_stderr,((_tmpA27="\n",_tag_dyneither(_tmpA27,
sizeof(char),2))),_tag_dyneither(_tmpA26,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct _tmpA2A;struct Cyc_Toc_Toc_Impossible_struct*
_tmpA29;(int)_throw((void*)((_tmpA29=_cycalloc_atomic(sizeof(*_tmpA29)),((
_tmpA29[0]=((_tmpA2A.tag=Cyc_Toc_Toc_Impossible,_tmpA2A)),_tmpA29)))));}}char Cyc_Toc_Match_error[
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
0,0,0};static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl();static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(
skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpA2B;skip_stmt_opt=((_tmpA2B=
_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B[0]=Cyc_Absyn_skip_stmt(0),_tmpA2B))));}
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*
e){struct Cyc_Absyn_Cast_e_struct _tmpA2E;struct Cyc_Absyn_Cast_e_struct*_tmpA2D;
return(void*)((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D[0]=((_tmpA2E.tag=15,((
_tmpA2E.f1=(void*)t,((_tmpA2E.f2=e,((_tmpA2E.f3=0,((_tmpA2E.f4=Cyc_Absyn_No_coercion,
_tmpA2E)))))))))),_tmpA2D))));}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA31;struct Cyc_Absyn_Deref_e_struct*_tmpA30;return(void*)((_tmpA30=_cycalloc(
sizeof(*_tmpA30)),((_tmpA30[0]=((_tmpA31.tag=22,((_tmpA31.f1=e,_tmpA31)))),
_tmpA30))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA34;struct Cyc_Absyn_Subscript_e_struct*
_tmpA33;return(void*)((_tmpA33=_cycalloc(sizeof(*_tmpA33)),((_tmpA33[0]=((
_tmpA34.tag=25,((_tmpA34.f1=e1,((_tmpA34.f2=e2,_tmpA34)))))),_tmpA33))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct _tmpA37;struct Cyc_Absyn_StmtExp_e_struct*
_tmpA36;return(void*)((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36[0]=((
_tmpA37.tag=38,((_tmpA37.f1=s,_tmpA37)))),_tmpA36))));}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t);static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA3A;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA39;return(void*)((_tmpA39=
_cycalloc(sizeof(*_tmpA39)),((_tmpA39[0]=((_tmpA3A.tag=18,((_tmpA3A.f1=(void*)t,
_tmpA3A)))),_tmpA39))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA3D;struct Cyc_Absyn_FnCall_e_struct*
_tmpA3C;return(void*)((_tmpA3C=_cycalloc(sizeof(*_tmpA3C)),((_tmpA3C[0]=((
_tmpA3D.tag=11,((_tmpA3D.f1=e,((_tmpA3D.f2=es,((_tmpA3D.f3=0,_tmpA3D)))))))),
_tmpA3C))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(
struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct _tmpA40;struct Cyc_Absyn_Exp_s_struct*
_tmpA3F;return(void*)((_tmpA3F=_cycalloc(sizeof(*_tmpA3F)),((_tmpA3F[0]=((
_tmpA40.tag=1,((_tmpA40.f1=e,_tmpA40)))),_tmpA3F))));}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2);static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct
_tmpA43;struct Cyc_Absyn_Seq_s_struct*_tmpA42;return(void*)((_tmpA42=_cycalloc(
sizeof(*_tmpA42)),((_tmpA42[0]=((_tmpA43.tag=2,((_tmpA43.f1=s1,((_tmpA43.f2=s2,
_tmpA43)))))),_tmpA42))));}static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA46;struct Cyc_Absyn_Conditional_e_struct*_tmpA45;return(void*)((_tmpA45=
_cycalloc(sizeof(*_tmpA45)),((_tmpA45[0]=((_tmpA46.tag=6,((_tmpA46.f1=e1,((
_tmpA46.f2=e2,((_tmpA46.f3=e3,_tmpA46)))))))),_tmpA45))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA49;struct Cyc_Absyn_AggrMember_e_struct*_tmpA48;return(void*)((_tmpA48=
_cycalloc(sizeof(*_tmpA48)),((_tmpA48[0]=((_tmpA49.tag=23,((_tmpA49.f1=e,((
_tmpA49.f2=n,((_tmpA49.f3=0,((_tmpA49.f4=0,_tmpA49)))))))))),_tmpA48))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA4C;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA4B;return(void*)((_tmpA4B=_cycalloc(sizeof(*_tmpA4B)),((_tmpA4B[0]=((
_tmpA4C.tag=24,((_tmpA4C.f1=e,((_tmpA4C.f2=n,((_tmpA4C.f3=0,((_tmpA4C.f4=0,
_tmpA4C)))))))))),_tmpA4B))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA4F;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA4E;return(void*)((_tmpA4E=_cycalloc(
sizeof(*_tmpA4E)),((_tmpA4E[0]=((_tmpA4F.tag=37,((_tmpA4F.f1=tdopt,((_tmpA4F.f2=
ds,_tmpA4F)))))),_tmpA4E))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA52;
struct Cyc_Absyn_Goto_s_struct*_tmpA51;return(void*)((_tmpA51=_cycalloc(sizeof(*
_tmpA51)),((_tmpA51[0]=((_tmpA52.tag=8,((_tmpA52.f1=v,((_tmpA52.f2=s,_tmpA52)))))),
_tmpA51))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
Cyc_Absyn_Signed,0}}}};struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;
struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;
struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*
fvoidstar;};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,&
Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,&
Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,&
Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,&
Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t);static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmpC4=t;
enum Cyc_Absyn_Size_of _tmpC6;int _tmpC9;_LL1: {struct Cyc_Absyn_IntType_struct*
_tmpC5=(struct Cyc_Absyn_IntType_struct*)_tmpC4;if(_tmpC5->tag != 6)goto _LL3;else{
_tmpC6=_tmpC5->f2;}}_LL2: switch(_tmpC6){case Cyc_Absyn_Char_sz: _LLB: function=fS->fchar;
break;case Cyc_Absyn_Short_sz: _LLC: function=fS->fshort;break;case Cyc_Absyn_Int_sz:
_LLD: function=fS->fint;break;default: _LLE: {struct Cyc_Core_Impossible_struct
_tmpA58;const char*_tmpA57;struct Cyc_Core_Impossible_struct*_tmpA56;(int)_throw((
void*)((_tmpA56=_cycalloc(sizeof(*_tmpA56)),((_tmpA56[0]=((_tmpA58.tag=Cyc_Core_Impossible,((
_tmpA58.f1=((_tmpA57="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA57,sizeof(char),44))),_tmpA58)))),_tmpA56)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA5E;const char*_tmpA5D;struct Cyc_Core_Impossible_struct*
_tmpA5C;(int)_throw((void*)((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C[0]=((
_tmpA5E.tag=Cyc_Core_Impossible,((_tmpA5E.f1=((_tmpA5D="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpA5D,sizeof(char),64))),_tmpA5E)))),_tmpA5C)))));}_LL0:;}
return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(fS,Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr){void*_tmpD1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD3;void*_tmpD4;_LL14: {
struct Cyc_Absyn_PointerType_struct*_tmpD2=(struct Cyc_Absyn_PointerType_struct*)
_tmpD1;if(_tmpD2->tag != 5)goto _LL16;else{_tmpD3=_tmpD2->f1;_tmpD4=_tmpD3.elt_typ;}}
_LL15: return Cyc_Toc_getFunctionType(fS,_tmpD4);_LL16:;_LL17: {struct Cyc_Core_Impossible_struct
_tmpA64;const char*_tmpA63;struct Cyc_Core_Impossible_struct*_tmpA62;(int)_throw((
void*)((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=((_tmpA64.tag=Cyc_Core_Impossible,((
_tmpA64.f1=((_tmpA63="impossible type (not pointer)",_tag_dyneither(_tmpA63,
sizeof(char),30))),_tmpA64)))),_tmpA62)))));}_LL13:;}struct _tuple16{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e);
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*_tmpD8=e->r;union Cyc_Absyn_Cnst
_tmpDA;struct _tuple4 _tmpDB;char _tmpDC;union Cyc_Absyn_Cnst _tmpDE;struct _tuple5
_tmpDF;short _tmpE0;union Cyc_Absyn_Cnst _tmpE2;struct _tuple6 _tmpE3;int _tmpE4;union
Cyc_Absyn_Cnst _tmpE6;struct _tuple7 _tmpE7;long long _tmpE8;union Cyc_Absyn_Cnst
_tmpEA;int _tmpEB;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_List_List*_tmpEF;struct Cyc_List_List*
_tmpF1;struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*
_tmpF7;_LL19: {struct Cyc_Absyn_Const_e_struct*_tmpD9=(struct Cyc_Absyn_Const_e_struct*)
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
static int Cyc_Toc_is_nullable(void*t);static int Cyc_Toc_is_nullable(void*t){void*
_tmpF8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpFA;struct Cyc_Absyn_PtrAtts
_tmpFB;union Cyc_Absyn_Constraint*_tmpFC;_LL32: {struct Cyc_Absyn_PointerType_struct*
_tmpF9=(struct Cyc_Absyn_PointerType_struct*)_tmpF8;if(_tmpF9->tag != 5)goto _LL34;
else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpFA.ptr_atts;_tmpFC=_tmpFB.nullable;}}_LL33:
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFC);
_LL34:;_LL35: {const char*_tmpA67;void*_tmpA66;(_tmpA66=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA67="is_nullable",
_tag_dyneither(_tmpA67,sizeof(char),12))),_tag_dyneither(_tmpA66,sizeof(void*),0)));}
_LL31:;}static char _tmp10B[1]="";static char _tmp10C[8]="*bogus*";static struct
_tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct _dyneither_ptr tag);
static struct _tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp100;struct Cyc_Dict_Dict*_tmp101;
struct Cyc_Toc_TocState _tmpFF=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp100=_tmpFF.dyn;_tmp101=_tmpFF.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp10C,_tmp10C,_tmp10C + 8};static struct _tuple1 bogus_qvar={{.Loc_n={
3,0}},& bogus_string};static struct _tuple14 pair={& bogus_qvar,{_tmp10B,_tmp10B,
_tmp10B + 1}};{struct _tuple14 _tmpA68;pair=((_tmpA68.f1=x,((_tmpA68.f2=tag,_tmpA68))));}{
struct _DynRegionFrame _tmp103;struct _RegionHandle*d=_open_dynregion(& _tmp103,
_tmp100);{struct _tuple1**_tmp104=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,
struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp101,(struct _tuple14*)& pair);if(
_tmp104 != 0){struct _tuple1*_tmp105=*_tmp104;_npop_handler(0);return _tmp105;}{
struct _tuple14*_tmpA69;struct _tuple14*_tmp106=(_tmpA69=_cycalloc(sizeof(*_tmpA69)),((
_tmpA69->f1=x,((_tmpA69->f2=tag,_tmpA69)))));struct _dyneither_ptr*_tmpA6C;struct
_tuple1*_tmpA6B;struct _tuple1*res=(_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B->f1=(*
x).f1,((_tmpA6B->f2=((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA6C)))),_tmpA6B)))));*_tmp101=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp101,(
struct _tuple14*)_tmp106,res);{struct _tuple1*_tmp107=res;_npop_handler(0);return
_tmp107;}}};_pop_dynregion(d);}}}struct _tuple17{void*f1;struct Cyc_List_List*f2;}
;static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0);static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmp10E;struct Cyc_List_List**
_tmp10F;struct Cyc_Toc_TocState _tmp10D=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp10E=_tmp10D.dyn;_tmp10F=_tmp10D.tuple_types;{struct _DynRegionFrame _tmp110;
struct _RegionHandle*d=_open_dynregion(& _tmp110,_tmp10E);{struct Cyc_List_List*
_tmp111=*_tmp10F;for(0;_tmp111 != 0;_tmp111=_tmp111->tl){struct _tuple17 _tmp113;
void*_tmp114;struct Cyc_List_List*_tmp115;struct _tuple17*_tmp112=(struct _tuple17*)
_tmp111->hd;_tmp113=*_tmp112;_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;{struct Cyc_List_List*
_tmp116=tqs0;for(0;_tmp116 != 0  && _tmp115 != 0;(_tmp116=_tmp116->tl,_tmp115=
_tmp115->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple10*)_tmp116->hd)).f2,(void*)
_tmp115->hd))break;}if(_tmp116 == 0  && _tmp115 == 0){void*_tmp117=_tmp114;
_npop_handler(0);return _tmp117;}}}}{struct Cyc_Int_pa_struct _tmpA74;void*_tmpA73[
1];const char*_tmpA72;struct _dyneither_ptr*_tmpA71;struct _dyneither_ptr*xname=(
_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71[0]=(struct _dyneither_ptr)((_tmpA74.tag=
1,((_tmpA74.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA73[0]=& _tmpA74,
Cyc_aprintf(((_tmpA72="_tuple%d",_tag_dyneither(_tmpA72,sizeof(char),9))),
_tag_dyneither(_tmpA73,sizeof(void*),1)))))))),_tmpA71)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp118=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA77;struct Cyc_List_List*
_tmpA76;_tmp118=((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->hd=((_tmpA77=
_cycalloc(sizeof(*_tmpA77)),((_tmpA77->name=Cyc_Absyn_fieldname(i),((_tmpA77->tq=
Cyc_Toc_mt_tq,((_tmpA77->type=(void*)ts2->hd,((_tmpA77->width=0,((_tmpA77->attributes=
0,_tmpA77)))))))))))),((_tmpA76->tl=_tmp118,_tmpA76))))));}}_tmp118=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp118);{struct _tuple1*_tmpA7C;struct
Cyc_Absyn_AggrdeclImpl*_tmpA7B;struct Cyc_Absyn_Aggrdecl*_tmpA7A;struct Cyc_Absyn_Aggrdecl*
_tmp11B=(_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A->kind=Cyc_Absyn_StructA,((
_tmpA7A->sc=Cyc_Absyn_Public,((_tmpA7A->name=((_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((
_tmpA7C->f1=Cyc_Absyn_Rel_n(0),((_tmpA7C->f2=xname,_tmpA7C)))))),((_tmpA7A->tvs=
0,((_tmpA7A->impl=((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->exist_vars=0,((
_tmpA7B->rgn_po=0,((_tmpA7B->fields=_tmp118,((_tmpA7B->tagged=0,_tmpA7B)))))))))),((
_tmpA7A->attributes=0,_tmpA7A)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpA82;struct Cyc_Absyn_Aggr_d_struct _tmpA81;struct Cyc_List_List*_tmpA80;Cyc_Toc_result_decls=((
_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80->hd=Cyc_Absyn_new_decl((void*)((
_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA81.tag=6,((_tmpA81.f1=
_tmp11B,_tmpA81)))),_tmpA82)))),0),((_tmpA80->tl=Cyc_Toc_result_decls,_tmpA80))))));}{
struct _tuple17*_tmpA85;struct Cyc_List_List*_tmpA84;*_tmp10F=((_tmpA84=
_region_malloc(d,sizeof(*_tmpA84)),((_tmpA84->hd=((_tmpA85=_region_malloc(d,
sizeof(*_tmpA85)),((_tmpA85->f1=x,((_tmpA85->f2=ts,_tmpA85)))))),((_tmpA84->tl=*
_tmp10F,_tmpA84))))));}{void*_tmp121=x;_npop_handler(0);return _tmp121;}}};
_pop_dynregion(d);}}struct _tuple1*Cyc_Toc_temp_var();struct _tuple1*Cyc_Toc_temp_var(){
int _tmp129=Cyc_Toc_temp_var_counter ++;struct _dyneither_ptr*_tmpA92;const char*
_tmpA91;void*_tmpA90[1];struct Cyc_Int_pa_struct _tmpA8F;struct _tuple1*_tmpA8E;
struct _tuple1*res=(_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E->f1=Cyc_Absyn_Loc_n,((
_tmpA8E->f2=((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=(struct
_dyneither_ptr)((_tmpA8F.tag=1,((_tmpA8F.f1=(unsigned int)_tmp129,((_tmpA90[0]=&
_tmpA8F,Cyc_aprintf(((_tmpA91="_tmp%X",_tag_dyneither(_tmpA91,sizeof(char),7))),
_tag_dyneither(_tmpA90,sizeof(void*),1)))))))),_tmpA92)))),_tmpA8E)))));return
res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();static struct _dyneither_ptr*
Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp130;struct Cyc_Xarray_Xarray*
_tmp131;struct Cyc_Toc_TocState _tmp12F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp130=_tmp12F.dyn;_tmp131=_tmp12F.temp_labels;{struct _DynRegionFrame _tmp132;
struct _RegionHandle*d=_open_dynregion(& _tmp132,_tmp130);{int _tmp133=Cyc_Toc_fresh_label_counter
++;if(_tmp133 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp131)){
struct _dyneither_ptr*_tmp134=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp131,_tmp133);_npop_handler(0);return _tmp134;}{struct Cyc_Int_pa_struct
_tmpA9A;void*_tmpA99[1];const char*_tmpA98;struct _dyneither_ptr*_tmpA97;struct
_dyneither_ptr*res=(_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97[0]=(struct
_dyneither_ptr)((_tmpA9A.tag=1,((_tmpA9A.f1=(unsigned int)_tmp133,((_tmpA99[0]=&
_tmpA9A,Cyc_aprintf(((_tmpA98="_LL%X",_tag_dyneither(_tmpA98,sizeof(char),6))),
_tag_dyneither(_tmpA99,sizeof(void*),1)))))))),_tmpA97)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp131,res)!= _tmp133){const char*
_tmpA9D;void*_tmpA9C;(_tmpA9C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9D="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpA9D,sizeof(char),43))),_tag_dyneither(_tmpA9C,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp137=res;_npop_handler(0);return _tmp137;}}};
_pop_dynregion(d);}}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple1*name);static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple1*name){int ans=0;struct Cyc_List_List*_tmp13C=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp13C))->hd)->name)
!= 0){++ ans;_tmp13C=_tmp13C->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple8*Cyc_Toc_arg_to_c(struct
_tuple8*a);static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a){struct Cyc_Core_Opt*
_tmp13E;struct Cyc_Absyn_Tqual _tmp13F;void*_tmp140;struct _tuple8 _tmp13D=*a;
_tmp13E=_tmp13D.f1;_tmp13F=_tmp13D.f2;_tmp140=_tmp13D.f3;{struct _tuple8*_tmpA9E;
return(_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((_tmpA9E->f1=_tmp13E,((_tmpA9E->f2=
_tmp13F,((_tmpA9E->f3=Cyc_Toc_typ_to_c(_tmp140),_tmpA9E)))))));}}static struct
_tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x);static struct _tuple10*Cyc_Toc_typ_to_c_f(
struct _tuple10*x){struct Cyc_Absyn_Tqual _tmp143;void*_tmp144;struct _tuple10
_tmp142=*x;_tmp143=_tmp142.f1;_tmp144=_tmp142.f2;{struct _tuple10*_tmpA9F;return(
_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->f1=_tmp143,((_tmpA9F->f2=Cyc_Toc_typ_to_c(
_tmp144),_tmpA9F)))));}}static void*Cyc_Toc_typ_to_c_array(void*t);static void*Cyc_Toc_typ_to_c_array(
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
f);static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*
f){struct Cyc_Absyn_Aggrfield*_tmpAA0;return(_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((
_tmpAA0->name=f->name,((_tmpAA0->tq=Cyc_Toc_mt_tq,((_tmpAA0->type=Cyc_Toc_typ_to_c(
f->type),((_tmpAA0->width=f->width,((_tmpAA0->attributes=f->attributes,_tmpAA0)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ();static void*
Cyc_Toc_char_star_typ(){static void**cs=0;if(cs == 0){void**_tmpAA1;cs=((_tmpAA1=
_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAA1))));}
return*cs;}static void*Cyc_Toc_rgn_typ();static void*Cyc_Toc_rgn_typ(){static void**
r=0;if(r == 0){void**_tmpAA2;r=((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2[0]=
Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),
_tmpAA2))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ();static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;if(r == 0){void**_tmpAA3;r=((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((
_tmpAA3[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),
_tmpAA3))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static void*Cyc_Toc_typ_to_c(
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
_tmp164.f1;_tmp166=_tmp164.f2;}}_LL49: {const char*_tmpAA4;return Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp166->name,((_tmpAA4="_struct",_tag_dyneither(
_tmpAA4,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp167=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp156;if(_tmp167->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAA7;void*_tmpAA6;(_tmpAA6=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA7="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAA7,sizeof(char),29))),_tag_dyneither(_tmpAA6,sizeof(void*),0)));}
_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp168=(struct Cyc_Absyn_PointerType_struct*)
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
goto _LL84;}_LL83: continue;_LL84:;_LL85:{struct Cyc_List_List*_tmpAA8;_tmp19F=((
_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->hd=(void*)_tmp17D->hd,((_tmpAA8->tl=
_tmp19F,_tmpAA8))))));}goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A6=((struct
Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17A);if(_tmp17C != 0){void*_tmp1A7=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17C->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpAA9;struct _tuple8*_tmp1A8=(_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((
_tmpAA9->f1=_tmp17C->name,((_tmpAA9->f2=_tmp17C->tq,((_tmpAA9->f3=_tmp1A7,
_tmpAA9)))))));struct Cyc_List_List*_tmpAAA;_tmp1A6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1A6,((
_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA->hd=_tmp1A8,((_tmpAAA->tl=0,
_tmpAAA)))))));}{struct Cyc_Absyn_FnType_struct _tmpAB0;struct Cyc_Absyn_FnInfo
_tmpAAF;struct Cyc_Absyn_FnType_struct*_tmpAAE;return(void*)((_tmpAAE=_cycalloc(
sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAB0.tag=10,((_tmpAB0.f1=((_tmpAAF.tvars=0,((
_tmpAAF.effect=0,((_tmpAAF.ret_typ=Cyc_Toc_typ_to_c(_tmp179),((_tmpAAF.args=
_tmp1A6,((_tmpAAF.c_varargs=_tmp17B,((_tmpAAF.cyc_varargs=0,((_tmpAAF.rgn_po=0,((
_tmpAAF.attributes=_tmp19F,_tmpAAF)))))))))))))))),_tmpAB0)))),_tmpAAE))));}}}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp17E=(struct Cyc_Absyn_TupleType_struct*)
_tmp156;if(_tmp17E->tag != 11)goto _LL5A;else{_tmp17F=_tmp17E->f1;}}_LL59: _tmp17F=((
struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp17F);return Cyc_Toc_add_tuple_type(
_tmp17F);_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp180=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp156;if(_tmp180->tag != 13)goto _LL5C;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpAB3;struct Cyc_Absyn_AnonAggrType_struct*
_tmpAB2;return(void*)((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2[0]=((
_tmpAB3.tag=13,((_tmpAB3.f1=_tmp181,((_tmpAB3.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp182),_tmpAB3)))))),_tmpAB2))));}_LL5C: {
struct Cyc_Absyn_AggrType_struct*_tmp183=(struct Cyc_Absyn_AggrType_struct*)
_tmp156;if(_tmp183->tag != 12)goto _LL5E;else{_tmp184=_tmp183->f1;_tmp185=_tmp184.aggr_info;
_tmp186=_tmp184.targs;}}_LL5D:{union Cyc_Absyn_AggrInfoU _tmp1B0=_tmp185;struct
_tuple3 _tmp1B1;_LL87: if((_tmp1B0.UnknownAggr).tag != 1)goto _LL89;_tmp1B1=(struct
_tuple3)(_tmp1B0.UnknownAggr).val;_LL88: return t;_LL89:;_LL8A: goto _LL86;_LL86:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1B2=Cyc_Absyn_get_known_aggrdecl(_tmp185);if(
_tmp1B2->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1B2->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1B2->name,Cyc_Absyn_strctq);}}_LL5E: {
struct Cyc_Absyn_EnumType_struct*_tmp187=(struct Cyc_Absyn_EnumType_struct*)
_tmp156;if(_tmp187->tag != 14)goto _LL60;else{_tmp188=_tmp187->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp189=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp156;if(_tmp189->tag != 15)goto _LL62;else{_tmp18A=_tmp189->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18A);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp18B=(struct Cyc_Absyn_TypedefType_struct*)
_tmp156;if(_tmp18B->tag != 18)goto _LL64;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;
_tmp18E=_tmp18B->f3;_tmp18F=_tmp18B->f4;}}_LL63: if(_tmp18F == 0  || Cyc_noexpand_r){
if(_tmp18D != 0){struct Cyc_Absyn_TypedefType_struct _tmpAB6;struct Cyc_Absyn_TypedefType_struct*
_tmpAB5;return(void*)((_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5[0]=((
_tmpAB6.tag=18,((_tmpAB6.f1=_tmp18C,((_tmpAB6.f2=0,((_tmpAB6.f3=_tmp18E,((
_tmpAB6.f4=0,_tmpAB6)))))))))),_tmpAB5))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpABC;void**_tmpABB;struct Cyc_Absyn_TypedefType_struct*_tmpABA;return(void*)((
_tmpABA=_cycalloc(sizeof(*_tmpABA)),((_tmpABA[0]=((_tmpABC.tag=18,((_tmpABC.f1=
_tmp18C,((_tmpABC.f2=0,((_tmpABC.f3=_tmp18E,((_tmpABC.f4=((_tmpABB=_cycalloc(
sizeof(*_tmpABB)),((_tmpABB[0]=Cyc_Toc_typ_to_c_array(*_tmp18F),_tmpABB)))),
_tmpABC)))))))))),_tmpABA))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp190=(
struct Cyc_Absyn_TagType_struct*)_tmp156;if(_tmp190->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp191=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp156;if(_tmp191->tag != 16)goto _LL68;
else{_tmp192=(void*)_tmp191->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp193=(struct Cyc_Absyn_DynRgnType_struct*)_tmp156;
if(_tmp193->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp194=(struct Cyc_Absyn_HeapRgn_struct*)_tmp156;if(
_tmp194->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpABF;void*_tmpABE;(_tmpABE=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpABF="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpABF,sizeof(char),55))),_tag_dyneither(_tmpABE,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp195=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp156;if(_tmp195->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpAC2;void*_tmpAC1;(
_tmpAC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC2="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpAC2,sizeof(char),57))),_tag_dyneither(_tmpAC1,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp196=(struct Cyc_Absyn_AccessEff_struct*)
_tmp156;if(_tmp196->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp197=(struct Cyc_Absyn_JoinEff_struct*)_tmp156;if(_tmp197->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp198=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp156;if(_tmp198->tag != 25)goto _LL74;}_LL73: {const char*_tmpAC5;void*_tmpAC4;(
_tmpAC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC5="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpAC5,sizeof(char),49))),_tag_dyneither(_tmpAC4,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp199=(struct Cyc_Absyn_ValueofType_struct*)
_tmp156;if(_tmp199->tag != 19)goto _LL3D;}_LL75: {const char*_tmpAC8;void*_tmpAC7;(
_tmpAC7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC8="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpAC8,sizeof(char),51))),_tag_dyneither(_tmpAC7,sizeof(void*),0)));}_LL3D:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*l);static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(
void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1C0=t;struct
Cyc_Absyn_ArrayInfo _tmp1C2;void*_tmp1C3;struct Cyc_Absyn_Tqual _tmp1C4;_LL8C: {
struct Cyc_Absyn_ArrayType_struct*_tmp1C1=(struct Cyc_Absyn_ArrayType_struct*)
_tmp1C0;if(_tmp1C1->tag != 9)goto _LL8E;else{_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C2.elt_type;
_tmp1C4=_tmp1C2.tq;}}_LL8D: return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp1C3,(
void*)& Cyc_Absyn_HeapRgn_val,_tmp1C4,Cyc_Absyn_false_conref),e);_LL8E:;_LL8F:
return Cyc_Toc_cast_it(t,e);_LL8B:;}static int Cyc_Toc_atomic_typ(void*t);static int
Cyc_Toc_atomic_typ(void*t){void*_tmp1C5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp1D0;void*_tmp1D1;struct Cyc_Absyn_AggrInfo _tmp1D3;union Cyc_Absyn_AggrInfoU
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
_LLA6:{union Cyc_Absyn_AggrInfoU _tmp1E3=_tmp1D4;struct _tuple3 _tmp1E4;_LLB8: if((
_tmp1E3.UnknownAggr).tag != 1)goto _LLBA;_tmp1E4=(struct _tuple3)(_tmp1E3.UnknownAggr).val;
_LLB9: return 0;_LLBA:;_LLBB: goto _LLB7;_LLB7:;}{struct Cyc_Absyn_Aggrdecl*_tmp1E5=
Cyc_Absyn_get_known_aggrdecl(_tmp1D4);if(_tmp1E5->impl == 0)return 0;{struct Cyc_List_List*
_tmp1E6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E5->impl))->fields;for(
0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1E6->hd)->type))return 0;}}return 1;}_LLA7: {struct Cyc_Absyn_AnonAggrType_struct*
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
goto _LLB2;_LLB1: {struct Cyc_Absyn_DynRgnType_struct*_tmp1E1=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp1C5;if(_tmp1E1->tag != 17)goto _LLB3;}_LLB2: goto _LLB4;_LLB3: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1E2=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1C5;if(_tmp1E2->tag != 16)goto
_LLB5;}_LLB4: return 0;_LLB5:;_LLB6: {const char*_tmpACC;void*_tmpACB[1];struct Cyc_String_pa_struct
_tmpACA;(_tmpACA.tag=0,((_tmpACA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpACB[0]=& _tmpACA,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACC="atomic_typ:  bad type %s",
_tag_dyneither(_tmpACC,sizeof(char),25))),_tag_dyneither(_tmpACB,sizeof(void*),1)))))));}
_LL90:;}static int Cyc_Toc_is_void_star(void*t);static int Cyc_Toc_is_void_star(void*
t){void*_tmp1EA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1EC;void*
_tmp1ED;_LLBD: {struct Cyc_Absyn_PointerType_struct*_tmp1EB=(struct Cyc_Absyn_PointerType_struct*)
_tmp1EA;if(_tmp1EB->tag != 5)goto _LLBF;else{_tmp1EC=_tmp1EB->f1;_tmp1ED=_tmp1EC.elt_typ;}}
_LLBE: {void*_tmp1EE=Cyc_Tcutil_compress(_tmp1ED);_LLC2: {struct Cyc_Absyn_VoidType_struct*
_tmp1EF=(struct Cyc_Absyn_VoidType_struct*)_tmp1EE;if(_tmp1EF->tag != 0)goto _LLC4;}
_LLC3: return 1;_LLC4:;_LLC5: return 0;_LLC1:;}_LLBF:;_LLC0: return 0;_LLBC:;}static
int Cyc_Toc_is_tvar(void*t);static int Cyc_Toc_is_tvar(void*t){void*_tmp1F0=Cyc_Tcutil_compress(
t);_LLC7: {struct Cyc_Absyn_VarType_struct*_tmp1F1=(struct Cyc_Absyn_VarType_struct*)
_tmp1F0;if(_tmp1F1->tag != 2)goto _LLC9;}_LLC8: return 1;_LLC9:;_LLCA: return 0;_LLC6:;}
static int Cyc_Toc_is_void_star_or_tvar(void*t);static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f);static int Cyc_Toc_is_poly_field(void*t,struct
_dyneither_ptr*f){void*_tmp1F2=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1F4;union Cyc_Absyn_AggrInfoU _tmp1F5;struct Cyc_List_List*_tmp1F7;_LLCC: {
struct Cyc_Absyn_AggrType_struct*_tmp1F3=(struct Cyc_Absyn_AggrType_struct*)
_tmp1F2;if(_tmp1F3->tag != 12)goto _LLCE;else{_tmp1F4=_tmp1F3->f1;_tmp1F5=_tmp1F4.aggr_info;}}
_LLCD: {struct Cyc_Absyn_Aggrdecl*_tmp1F8=Cyc_Absyn_get_known_aggrdecl(_tmp1F5);
if(_tmp1F8->impl == 0){const char*_tmpACF;void*_tmpACE;(_tmpACE=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACF="is_poly_field: type missing fields",
_tag_dyneither(_tmpACF,sizeof(char),35))),_tag_dyneither(_tmpACE,sizeof(void*),0)));}
_tmp1F7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F8->impl))->fields;goto
_LLCF;}_LLCE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F2;if(_tmp1F6->tag != 13)goto _LLD0;else{_tmp1F7=_tmp1F6->f2;}}_LLCF: {struct
Cyc_Absyn_Aggrfield*_tmp1FB=Cyc_Absyn_lookup_field(_tmp1F7,f);if(_tmp1FB == 0){
const char*_tmpAD3;void*_tmpAD2[1];struct Cyc_String_pa_struct _tmpAD1;(_tmpAD1.tag=
0,((_tmpAD1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAD2[0]=&
_tmpAD1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD3="is_poly_field: bad field %s",_tag_dyneither(_tmpAD3,sizeof(char),28))),
_tag_dyneither(_tmpAD2,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp1FB->type);}_LLD0:;_LLD1: {const char*_tmpAD7;void*_tmpAD6[1];struct Cyc_String_pa_struct
_tmpAD5;(_tmpAD5.tag=0,((_tmpAD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAD6[0]=& _tmpAD5,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD7="is_poly_field: bad type %s",
_tag_dyneither(_tmpAD7,sizeof(char),27))),_tag_dyneither(_tmpAD6,sizeof(void*),1)))))));}
_LLCB:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e){void*_tmp202=e->r;struct Cyc_Absyn_Exp*_tmp204;struct
_dyneither_ptr*_tmp205;struct Cyc_Absyn_Exp*_tmp207;struct _dyneither_ptr*_tmp208;
_LLD3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp203=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp202;if(_tmp203->tag != 23)goto _LLD5;else{_tmp204=_tmp203->f1;_tmp205=_tmp203->f2;}}
_LLD4: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp204->topt))->v,_tmp205);_LLD5: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp206=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp202;if(_tmp206->tag != 24)goto _LLD7;else{
_tmp207=_tmp206->f1;_tmp208=_tmp206->f2;}}_LLD6: {void*_tmp209=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp207->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp20B;void*_tmp20C;_LLDA: {struct Cyc_Absyn_PointerType_struct*_tmp20A=(struct
Cyc_Absyn_PointerType_struct*)_tmp209;if(_tmp20A->tag != 5)goto _LLDC;else{_tmp20B=
_tmp20A->f1;_tmp20C=_tmp20B.elt_typ;}}_LLDB: return Cyc_Toc_is_poly_field(_tmp20C,
_tmp208);_LLDC:;_LLDD: {const char*_tmpADB;void*_tmpADA[1];struct Cyc_String_pa_struct
_tmpAD9;(_tmpAD9.tag=0,((_tmpAD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp207->topt))->v)),((
_tmpADA[0]=& _tmpAD9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADB="is_poly_project: bad type %s",_tag_dyneither(_tmpADB,sizeof(char),29))),
_tag_dyneither(_tmpADA,sizeof(void*),1)))))));}_LLD9:;}_LLD7:;_LLD8: return 0;
_LLD2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmpADC;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpADC=_cycalloc(
sizeof(*_tmpADC)),((_tmpADC->hd=s,((_tmpADC->tl=0,_tmpADC)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpADD;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpADD=_cycalloc(sizeof(*
_tmpADD)),((_tmpADD->hd=s,((_tmpADD->tl=0,_tmpADD)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpADE[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpADE[1]=s,((_tmpADE[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpADE,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpADF[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpADF[1]=n,((
_tmpADF[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpADF,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpAE0[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAE0[1]=n,((
_tmpAE0[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAE0,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmpAE1[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmpAE1[2]=n,((_tmpAE1[1]=s,((_tmpAE1[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAE1,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpAE2;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2->hd=e,((_tmpAE2->tl=0,_tmpAE2)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpAE3;throw_match_stmt_opt=((
_tmpAE3=_cycalloc(sizeof(*_tmpAE3)),((_tmpAE3[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmpAE3))));}return*throw_match_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp218=e->r;union Cyc_Absyn_Cnst _tmp21A;struct _dyneither_ptr _tmp21B;_LLDF: {
struct Cyc_Absyn_Const_e_struct*_tmp219=(struct Cyc_Absyn_Const_e_struct*)_tmp218;
if(_tmp219->tag != 0)goto _LLE1;else{_tmp21A=_tmp219->f1;if((_tmp21A.String_c).tag
!= 7)goto _LLE1;_tmp21B=(struct _dyneither_ptr)(_tmp21A.String_c).val;}}_LLE0:
is_string=1;goto _LLDE;_LLE1:;_LLE2: goto _LLDE;_LLDE:;}{struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpAE9;struct
Cyc_Absyn_Var_d_struct _tmpAE8;struct Cyc_List_List*_tmpAE7;Cyc_Toc_result_decls=((
_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7->hd=Cyc_Absyn_new_decl((void*)((
_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9[0]=((_tmpAE8.tag=0,((_tmpAE8.f1=vd,
_tmpAE8)))),_tmpAE9)))),0),((_tmpAE7->tl=Cyc_Toc_result_decls,_tmpAE7))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple16*_tmpAF0;
struct _tuple16*_tmpAEF;struct _tuple16*_tmpAEE;struct _tuple16*_tmpAED[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpAED[2]=((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((
_tmpAF0->f1=0,((_tmpAF0->f2=xplussz,_tmpAF0)))))),((_tmpAED[1]=((_tmpAEF=
_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->f1=0,((_tmpAEF->f2=xexp,_tmpAEF)))))),((
_tmpAED[0]=((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->f1=0,((_tmpAEE->f2=
xexp,_tmpAEE)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAED,sizeof(struct _tuple16*),3)))))))),0);}return urm_exp;}}}
struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv);static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp224;int _tmp225;struct Cyc_Toc_Env*
_tmp223=nv;_tmp224=*_tmp223;_tmp225=_tmp224.toplevel;return _tmp225;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x);static
struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Toc_Env _tmp227;struct Cyc_Dict_Dict _tmp228;struct Cyc_Toc_Env*_tmp226=nv;
_tmp227=*_tmp226;_tmp228=_tmp227.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp228,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r);static struct Cyc_Toc_Env*Cyc_Toc_empty_env(
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpAF1;return(_tmpAF1=_region_malloc(r,
sizeof(*_tmpAF1)),((_tmpAF1->break_lab=(struct _dyneither_ptr**)0,((_tmpAF1->continue_lab=(
struct _dyneither_ptr**)0,((_tmpAF1->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpAF1->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpAF1->toplevel=(int)1,((_tmpAF1->rgn=(struct _RegionHandle*)r,_tmpAF1)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp22B;struct _dyneither_ptr**_tmp22C;struct _dyneither_ptr**
_tmp22D;struct Cyc_Toc_FallthruInfo*_tmp22E;struct Cyc_Dict_Dict _tmp22F;int _tmp230;
struct Cyc_Toc_Env*_tmp22A=e;_tmp22B=*_tmp22A;_tmp22C=_tmp22B.break_lab;_tmp22D=
_tmp22B.continue_lab;_tmp22E=_tmp22B.fallthru_info;_tmp22F=_tmp22B.varmap;
_tmp230=_tmp22B.toplevel;{struct Cyc_Toc_Env*_tmpAF2;return(_tmpAF2=
_region_malloc(r,sizeof(*_tmpAF2)),((_tmpAF2->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22C),((_tmpAF2->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22D),((_tmpAF2->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp22E,((_tmpAF2->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp22F),((_tmpAF2->toplevel=(
int)_tmp230,((_tmpAF2->rgn=(struct _RegionHandle*)r,_tmpAF2)))))))))))));}}static
struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp233;struct _dyneither_ptr**_tmp234;struct
_dyneither_ptr**_tmp235;struct Cyc_Toc_FallthruInfo*_tmp236;struct Cyc_Dict_Dict
_tmp237;int _tmp238;struct Cyc_Toc_Env*_tmp232=e;_tmp233=*_tmp232;_tmp234=_tmp233.break_lab;
_tmp235=_tmp233.continue_lab;_tmp236=_tmp233.fallthru_info;_tmp237=_tmp233.varmap;
_tmp238=_tmp233.toplevel;{struct Cyc_Toc_Env*_tmpAF3;return(_tmpAF3=
_region_malloc(r,sizeof(*_tmpAF3)),((_tmpAF3->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp234),((_tmpAF3->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp235),((_tmpAF3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp236,((_tmpAF3->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp237),((_tmpAF3->toplevel=(
int)0,((_tmpAF3->rgn=(struct _RegionHandle*)r,_tmpAF3)))))))))))));}}static struct
Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23B;struct _dyneither_ptr**_tmp23C;struct _dyneither_ptr**
_tmp23D;struct Cyc_Toc_FallthruInfo*_tmp23E;struct Cyc_Dict_Dict _tmp23F;int _tmp240;
struct Cyc_Toc_Env*_tmp23A=e;_tmp23B=*_tmp23A;_tmp23C=_tmp23B.break_lab;_tmp23D=
_tmp23B.continue_lab;_tmp23E=_tmp23B.fallthru_info;_tmp23F=_tmp23B.varmap;
_tmp240=_tmp23B.toplevel;{struct Cyc_Toc_Env*_tmpAF4;return(_tmpAF4=
_region_malloc(r,sizeof(*_tmpAF4)),((_tmpAF4->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23C),((_tmpAF4->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23D),((_tmpAF4->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp23E,((_tmpAF4->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp23F),((_tmpAF4->toplevel=(
int)1,((_tmpAF4->rgn=(struct _RegionHandle*)r,_tmpAF4)))))))))))));}}static struct
Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct
_tuple1*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp242=(*x).f1;struct Cyc_List_List*_tmp243;_LLE4: if((
_tmp242.Rel_n).tag != 1)goto _LLE6;_tmp243=(struct Cyc_List_List*)(_tmp242.Rel_n).val;
_LLE5: {const char*_tmpAF8;void*_tmpAF7[1];struct Cyc_String_pa_struct _tmpAF6;(
_tmpAF6.tag=0,((_tmpAF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpAF7[0]=& _tmpAF6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpAF8="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpAF8,sizeof(char),30))),_tag_dyneither(_tmpAF7,sizeof(void*),1)))))));}_LLE6:;
_LLE7: goto _LLE3;_LLE3:;}{struct Cyc_Toc_Env _tmp248;struct _dyneither_ptr**_tmp249;
struct _dyneither_ptr**_tmp24A;struct Cyc_Toc_FallthruInfo*_tmp24B;struct Cyc_Dict_Dict
_tmp24C;int _tmp24D;struct Cyc_Toc_Env*_tmp247=e;_tmp248=*_tmp247;_tmp249=_tmp248.break_lab;
_tmp24A=_tmp248.continue_lab;_tmp24B=_tmp248.fallthru_info;_tmp24C=_tmp248.varmap;
_tmp24D=_tmp248.toplevel;{struct Cyc_Dict_Dict _tmp24E=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24C),x,y);struct Cyc_Toc_Env*_tmpAF9;return(_tmpAF9=_region_malloc(r,
sizeof(*_tmpAF9)),((_tmpAF9->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp249),((_tmpAF9->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24A),((_tmpAF9->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24B,((_tmpAF9->varmap=(struct Cyc_Dict_Dict)_tmp24E,((_tmpAF9->toplevel=(int)
_tmp24D,((_tmpAF9->rgn=(struct _RegionHandle*)r,_tmpAF9)))))))))))));}}}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp251;struct _dyneither_ptr**_tmp252;struct _dyneither_ptr**
_tmp253;struct Cyc_Toc_FallthruInfo*_tmp254;struct Cyc_Dict_Dict _tmp255;int _tmp256;
struct Cyc_Toc_Env*_tmp250=e;_tmp251=*_tmp250;_tmp252=_tmp251.break_lab;_tmp253=
_tmp251.continue_lab;_tmp254=_tmp251.fallthru_info;_tmp255=_tmp251.varmap;
_tmp256=_tmp251.toplevel;{struct Cyc_Toc_Env*_tmpAFA;return(_tmpAFA=
_region_malloc(r,sizeof(*_tmpAFA)),((_tmpAFA->break_lab=(struct _dyneither_ptr**)
0,((_tmpAFA->continue_lab=(struct _dyneither_ptr**)0,((_tmpAFA->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp254,((_tmpAFA->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp255),((_tmpAFA->toplevel=(int)_tmp256,((_tmpAFA->rgn=(struct _RegionHandle*)
r,_tmpAFA)))))))))))));}}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpAFB;fallthru_vars=((
_tmpAFB=_region_malloc(r,sizeof(*_tmpAFB)),((_tmpAFB->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpAFB->tl=fallthru_vars,_tmpAFB))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp25A;struct _dyneither_ptr**_tmp25B;struct
_dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict
_tmp25E;int _tmp25F;struct Cyc_Toc_Env*_tmp259=e;_tmp25A=*_tmp259;_tmp25B=_tmp25A.break_lab;
_tmp25C=_tmp25A.continue_lab;_tmp25D=_tmp25A.fallthru_info;_tmp25E=_tmp25A.varmap;
_tmp25F=_tmp25A.toplevel;{struct Cyc_Toc_Env _tmp261;struct Cyc_Dict_Dict _tmp262;
struct Cyc_Toc_Env*_tmp260=next_case_env;_tmp261=*_tmp260;_tmp262=_tmp261.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpAFC;struct Cyc_Toc_FallthruInfo*fi=(_tmpAFC=
_region_malloc(r,sizeof(*_tmpAFC)),((_tmpAFC->label=fallthru_l,((_tmpAFC->binders=
fallthru_vars,((_tmpAFC->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp262),_tmpAFC)))))));
struct _dyneither_ptr**_tmpAFF;struct Cyc_Toc_Env*_tmpAFE;return(_tmpAFE=
_region_malloc(r,sizeof(*_tmpAFE)),((_tmpAFE->break_lab=(struct _dyneither_ptr**)((
_tmpAFF=_region_malloc(r,sizeof(*_tmpAFF)),((_tmpAFF[0]=break_l,_tmpAFF)))),((
_tmpAFE->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25C),((
_tmpAFE->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpAFE->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp25E),((_tmpAFE->toplevel=(int)_tmp25F,((_tmpAFE->rgn=(
struct _RegionHandle*)r,_tmpAFE)))))))))))));}}}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp267;struct _dyneither_ptr**
_tmp268;struct _dyneither_ptr**_tmp269;struct Cyc_Toc_FallthruInfo*_tmp26A;struct
Cyc_Dict_Dict _tmp26B;int _tmp26C;struct Cyc_Toc_Env*_tmp266=e;_tmp267=*_tmp266;
_tmp268=_tmp267.break_lab;_tmp269=_tmp267.continue_lab;_tmp26A=_tmp267.fallthru_info;
_tmp26B=_tmp267.varmap;_tmp26C=_tmp267.toplevel;{struct _dyneither_ptr**_tmpB02;
struct Cyc_Toc_Env*_tmpB01;return(_tmpB01=_region_malloc(r,sizeof(*_tmpB01)),((
_tmpB01->break_lab=(struct _dyneither_ptr**)((_tmpB02=_region_malloc(r,sizeof(*
_tmpB02)),((_tmpB02[0]=break_l,_tmpB02)))),((_tmpB01->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp269),((_tmpB01->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB01->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26B),((_tmpB01->toplevel=(
int)_tmp26C,((_tmpB01->rgn=(struct _RegionHandle*)r,_tmpB01)))))))))))));}}static
struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp270;struct _dyneither_ptr**_tmp271;struct _dyneither_ptr**_tmp272;struct Cyc_Toc_FallthruInfo*
_tmp273;struct Cyc_Dict_Dict _tmp274;int _tmp275;struct Cyc_Toc_Env*_tmp26F=e;
_tmp270=*_tmp26F;_tmp271=_tmp270.break_lab;_tmp272=_tmp270.continue_lab;_tmp273=
_tmp270.fallthru_info;_tmp274=_tmp270.varmap;_tmp275=_tmp270.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB05;struct Cyc_Toc_Env*_tmpB04;return(_tmpB04=_region_malloc(r,sizeof(*
_tmpB04)),((_tmpB04->break_lab=(struct _dyneither_ptr**)0,((_tmpB04->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp272),((_tmpB04->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB05=_region_malloc(r,sizeof(*_tmpB05)),((
_tmpB05->label=next_l,((_tmpB05->binders=0,((_tmpB05->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB05)))))))),((_tmpB04->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp274),((_tmpB04->toplevel=(int)_tmp275,((_tmpB04->rgn=(struct _RegionHandle*)
r,_tmpB04)))))))))))));}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp278=e->annot;_LLE9: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp279=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp278;if(_tmp279->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLEB;}_LLEA: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLEB: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp27A=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp278;if(_tmp27A->tag != Cyc_CfFlowInfo_NotZero)
goto _LLED;}_LLEC: return 0;_LLED: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27B=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp278;if(_tmp27B->tag != Cyc_CfFlowInfo_IsZero)
goto _LLEF;}_LLEE:{const char*_tmpB08;void*_tmpB07;(_tmpB07=0,Cyc_Tcutil_terr(e->loc,((
_tmpB08="dereference of NULL pointer",_tag_dyneither(_tmpB08,sizeof(char),28))),
_tag_dyneither(_tmpB07,sizeof(void*),0)));}return 0;_LLEF: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp27C=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp278;if(_tmp27C->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF1;}_LLF0: return 0;_LLF1: {struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp27D=(
struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp278;if(_tmp27D->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLF3;}_LLF2:{const char*_tmpB0B;void*_tmpB0A;(_tmpB0A=0,Cyc_Tcutil_warn(e->loc,((
_tmpB0B="compiler oddity: pointer compared to tag type",_tag_dyneither(_tmpB0B,
sizeof(char),46))),_tag_dyneither(_tmpB0A,sizeof(void*),0)));}return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLF3:;_LLF4: {const char*
_tmpB0E;void*_tmpB0D;(_tmpB0D=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB0E="need_null_check",_tag_dyneither(
_tmpB0E,sizeof(char),16))),_tag_dyneither(_tmpB0D,sizeof(void*),0)));}_LLE8:;}
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e);static struct
Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp284=e->annot;
struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp288;_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp285=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp284;if(_tmp285->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF8;else{_tmp286=_tmp285->f1;}}_LLF7: return _tmp286;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp287=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp284;if(_tmp287->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp288=_tmp287->f1;}}_LLF9: return _tmp288;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp284;if(_tmp289->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:{const char*_tmpB11;void*_tmpB10;(_tmpB10=0,Cyc_Tcutil_terr(e->loc,((
_tmpB11="dereference of NULL pointer",_tag_dyneither(_tmpB11,sizeof(char),28))),
_tag_dyneither(_tmpB10,sizeof(void*),0)));}return 0;_LLFC: {struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp28A=(struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp284;if(_tmp28A->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLFE;}_LLFD: goto _LLFF;_LLFE: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp28B=(
struct Cyc_Absyn_EmptyAnnot_struct*)_tmp284;if(_tmp28B->tag != Cyc_Absyn_EmptyAnnot)
goto _LL100;}_LLFF: return 0;_LL100:;_LL101: {const char*_tmpB14;void*_tmpB13;(
_tmpB13=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB14="get_relns",_tag_dyneither(_tmpB14,sizeof(char),10))),_tag_dyneither(
_tmpB13,sizeof(void*),0)));}_LLF5:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t);static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp290=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp292;struct Cyc_Absyn_PtrAtts
_tmp293;union Cyc_Absyn_Constraint*_tmp294;union Cyc_Absyn_Constraint*_tmp295;
struct Cyc_Absyn_ArrayInfo _tmp297;struct Cyc_Absyn_Exp*_tmp298;_LL103: {struct Cyc_Absyn_PointerType_struct*
_tmp291=(struct Cyc_Absyn_PointerType_struct*)_tmp290;if(_tmp291->tag != 5)goto
_LL105;else{_tmp292=_tmp291->f1;_tmp293=_tmp292.ptr_atts;_tmp294=_tmp293.bounds;
_tmp295=_tmp293.zero_term;}}_LL104: {void*_tmp299=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp294);struct Cyc_Absyn_Exp*
_tmp29C;_LL10A: {struct Cyc_Absyn_DynEither_b_struct*_tmp29A=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp299;if(_tmp29A->tag != 0)goto _LL10C;}_LL10B: return 0;_LL10C: {struct Cyc_Absyn_Upper_b_struct*
_tmp29B=(struct Cyc_Absyn_Upper_b_struct*)_tmp299;if(_tmp29B->tag != 1)goto _LL109;
else{_tmp29C=_tmp29B->f1;}}_LL10D: {unsigned int _tmp29E;int _tmp29F;struct
_tuple11 _tmp29D=Cyc_Evexp_eval_const_uint_exp(_tmp29C);_tmp29E=_tmp29D.f1;
_tmp29F=_tmp29D.f2;return _tmp29F  && i <= _tmp29E;}_LL109:;}_LL105: {struct Cyc_Absyn_ArrayType_struct*
_tmp296=(struct Cyc_Absyn_ArrayType_struct*)_tmp290;if(_tmp296->tag != 9)goto
_LL107;else{_tmp297=_tmp296->f1;_tmp298=_tmp297.num_elts;}}_LL106: if(_tmp298 == 0)
return 0;{unsigned int _tmp2A1;int _tmp2A2;struct _tuple11 _tmp2A0=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp298);_tmp2A1=_tmp2A0.f1;_tmp2A2=_tmp2A0.f2;return
_tmp2A2  && i <= _tmp2A1;}_LL107:;_LL108: return 0;_LL102:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y);
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2A3=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(_tmp2A3->vd != y)continue;{union
Cyc_CfFlowInfo_RelnOp _tmp2A4=_tmp2A3->rop;struct Cyc_Absyn_Vardecl*_tmp2A5;struct
Cyc_Absyn_Vardecl*_tmp2A6;_LL10F: if((_tmp2A4.LessNumelts).tag != 3)goto _LL111;
_tmp2A5=(struct Cyc_Absyn_Vardecl*)(_tmp2A4.LessNumelts).val;_LL110: _tmp2A6=
_tmp2A5;goto _LL112;_LL111: if((_tmp2A4.LessEqNumelts).tag != 5)goto _LL113;_tmp2A6=(
struct Cyc_Absyn_Vardecl*)(_tmp2A4.LessEqNumelts).val;_LL112: if(_tmp2A6 == v)
return 1;else{goto _LL10E;}_LL113:;_LL114: continue;_LL10E:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e);
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e){{void*_tmp2A7=e->r;void*_tmp2A9;struct Cyc_Absyn_Vardecl*
_tmp2AB;void*_tmp2AD;struct Cyc_Absyn_Vardecl*_tmp2AF;void*_tmp2B1;struct Cyc_Absyn_Vardecl*
_tmp2B3;void*_tmp2B5;struct Cyc_Absyn_Vardecl*_tmp2B7;enum Cyc_Absyn_Primop
_tmp2B9;struct Cyc_List_List*_tmp2BA;struct Cyc_List_List _tmp2BB;struct Cyc_Absyn_Exp*
_tmp2BC;_LL116: {struct Cyc_Absyn_Var_e_struct*_tmp2A8=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A7;if(_tmp2A8->tag != 1)goto _LL118;else{_tmp2A9=(void*)_tmp2A8->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2AA=(struct Cyc_Absyn_Pat_b_struct*)_tmp2A9;if(_tmp2AA->tag
!= 5)goto _LL118;else{_tmp2AB=_tmp2AA->f1;}}}}_LL117: _tmp2AF=_tmp2AB;goto _LL119;
_LL118: {struct Cyc_Absyn_Var_e_struct*_tmp2AC=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A7;if(_tmp2AC->tag != 1)goto _LL11A;else{_tmp2AD=(void*)_tmp2AC->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2AE=(struct Cyc_Absyn_Local_b_struct*)_tmp2AD;if(
_tmp2AE->tag != 4)goto _LL11A;else{_tmp2AF=_tmp2AE->f1;}}}}_LL119: _tmp2B3=_tmp2AF;
goto _LL11B;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2B0=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A7;if(_tmp2B0->tag != 1)goto _LL11C;else{_tmp2B1=(void*)_tmp2B0->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2B2=(struct Cyc_Absyn_Global_b_struct*)_tmp2B1;if(
_tmp2B2->tag != 1)goto _LL11C;else{_tmp2B3=_tmp2B2->f1;}}}}_LL11B: _tmp2B7=_tmp2B3;
goto _LL11D;_LL11C: {struct Cyc_Absyn_Var_e_struct*_tmp2B4=(struct Cyc_Absyn_Var_e_struct*)
_tmp2A7;if(_tmp2B4->tag != 1)goto _LL11E;else{_tmp2B5=(void*)_tmp2B4->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2B6=(struct Cyc_Absyn_Param_b_struct*)_tmp2B5;if(
_tmp2B6->tag != 3)goto _LL11E;else{_tmp2B7=_tmp2B6->f1;}}}}_LL11D: if(_tmp2B7->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2B7))return 1;goto _LL115;_LL11E: {
struct Cyc_Absyn_Primop_e_struct*_tmp2B8=(struct Cyc_Absyn_Primop_e_struct*)
_tmp2A7;if(_tmp2B8->tag != 3)goto _LL120;else{_tmp2B9=_tmp2B8->f1;if(_tmp2B9 != Cyc_Absyn_Numelts)
goto _LL120;_tmp2BA=_tmp2B8->f2;if(_tmp2BA == 0)goto _LL120;_tmp2BB=*_tmp2BA;
_tmp2BC=(struct Cyc_Absyn_Exp*)_tmp2BB.hd;}}_LL11F:{void*_tmp2BD=_tmp2BC->r;void*
_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C1;void*_tmp2C3;struct Cyc_Absyn_Vardecl*
_tmp2C5;void*_tmp2C7;struct Cyc_Absyn_Vardecl*_tmp2C9;void*_tmp2CB;struct Cyc_Absyn_Vardecl*
_tmp2CD;_LL123: {struct Cyc_Absyn_Var_e_struct*_tmp2BE=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2BE->tag != 1)goto _LL125;else{_tmp2BF=(void*)_tmp2BE->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2C0=(struct Cyc_Absyn_Pat_b_struct*)_tmp2BF;if(_tmp2C0->tag
!= 5)goto _LL125;else{_tmp2C1=_tmp2C0->f1;}}}}_LL124: _tmp2C5=_tmp2C1;goto _LL126;
_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp2C2=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2C2->tag != 1)goto _LL127;else{_tmp2C3=(void*)_tmp2C2->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2C4=(struct Cyc_Absyn_Local_b_struct*)_tmp2C3;if(
_tmp2C4->tag != 4)goto _LL127;else{_tmp2C5=_tmp2C4->f1;}}}}_LL126: _tmp2C9=_tmp2C5;
goto _LL128;_LL127: {struct Cyc_Absyn_Var_e_struct*_tmp2C6=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2C6->tag != 1)goto _LL129;else{_tmp2C7=(void*)_tmp2C6->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2C8=(struct Cyc_Absyn_Global_b_struct*)_tmp2C7;if(
_tmp2C8->tag != 1)goto _LL129;else{_tmp2C9=_tmp2C8->f1;}}}}_LL128: _tmp2CD=_tmp2C9;
goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*_tmp2CA=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2CA->tag != 1)goto _LL12B;else{_tmp2CB=(void*)_tmp2CA->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2CC=(struct Cyc_Absyn_Param_b_struct*)_tmp2CB;if(
_tmp2CC->tag != 3)goto _LL12B;else{_tmp2CD=_tmp2CC->f1;}}}}_LL12A: return _tmp2CD == 
v;_LL12B:;_LL12C: goto _LL122;_LL122:;}goto _LL115;_LL120:;_LL121: goto _LL115;_LL115:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i);static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp2CE=a->r;void*_tmp2D0;
struct Cyc_Absyn_Vardecl*_tmp2D2;void*_tmp2D4;struct Cyc_Absyn_Vardecl*_tmp2D6;
void*_tmp2D8;struct Cyc_Absyn_Vardecl*_tmp2DA;void*_tmp2DC;struct Cyc_Absyn_Vardecl*
_tmp2DE;_LL12E: {struct Cyc_Absyn_Var_e_struct*_tmp2CF=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2CF->tag != 1)goto _LL130;else{_tmp2D0=(void*)_tmp2CF->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2D1=(struct Cyc_Absyn_Pat_b_struct*)_tmp2D0;if(_tmp2D1->tag
!= 5)goto _LL130;else{_tmp2D2=_tmp2D1->f1;}}}}_LL12F: _tmp2D6=_tmp2D2;goto _LL131;
_LL130: {struct Cyc_Absyn_Var_e_struct*_tmp2D3=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2D3->tag != 1)goto _LL132;else{_tmp2D4=(void*)_tmp2D3->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2D5=(struct Cyc_Absyn_Local_b_struct*)_tmp2D4;if(
_tmp2D5->tag != 4)goto _LL132;else{_tmp2D6=_tmp2D5->f1;}}}}_LL131: _tmp2DA=_tmp2D6;
goto _LL133;_LL132: {struct Cyc_Absyn_Var_e_struct*_tmp2D7=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2D7->tag != 1)goto _LL134;else{_tmp2D8=(void*)_tmp2D7->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2D9=(struct Cyc_Absyn_Global_b_struct*)_tmp2D8;if(
_tmp2D9->tag != 1)goto _LL134;else{_tmp2DA=_tmp2D9->f1;}}}}_LL133: _tmp2DE=_tmp2DA;
goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_struct*_tmp2DB=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2DB->tag != 1)goto _LL136;else{_tmp2DC=(void*)_tmp2DB->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2DD=(struct Cyc_Absyn_Param_b_struct*)_tmp2DC;if(
_tmp2DD->tag != 3)goto _LL136;else{_tmp2DE=_tmp2DD->f1;}}}}_LL135: if(_tmp2DE->escapes)
return 0;inner_loop: {void*_tmp2DF=i->r;void*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E2;
union Cyc_Absyn_Cnst _tmp2E4;struct _tuple6 _tmp2E5;enum Cyc_Absyn_Sign _tmp2E6;int
_tmp2E7;union Cyc_Absyn_Cnst _tmp2E9;struct _tuple6 _tmp2EA;enum Cyc_Absyn_Sign
_tmp2EB;int _tmp2EC;union Cyc_Absyn_Cnst _tmp2EE;struct _tuple6 _tmp2EF;enum Cyc_Absyn_Sign
_tmp2F0;int _tmp2F1;enum Cyc_Absyn_Primop _tmp2F3;struct Cyc_List_List*_tmp2F4;
struct Cyc_List_List _tmp2F5;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_List_List*
_tmp2F7;struct Cyc_List_List _tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;void*_tmp2FB;
struct Cyc_Absyn_Vardecl*_tmp2FD;void*_tmp2FF;struct Cyc_Absyn_Vardecl*_tmp301;
void*_tmp303;struct Cyc_Absyn_Vardecl*_tmp305;void*_tmp307;struct Cyc_Absyn_Vardecl*
_tmp309;_LL139: {struct Cyc_Absyn_Cast_e_struct*_tmp2E0=(struct Cyc_Absyn_Cast_e_struct*)
_tmp2DF;if(_tmp2E0->tag != 15)goto _LL13B;else{_tmp2E1=(void*)_tmp2E0->f1;_tmp2E2=
_tmp2E0->f2;}}_LL13A: i=_tmp2E2;goto inner_loop;_LL13B: {struct Cyc_Absyn_Const_e_struct*
_tmp2E3=(struct Cyc_Absyn_Const_e_struct*)_tmp2DF;if(_tmp2E3->tag != 0)goto _LL13D;
else{_tmp2E4=_tmp2E3->f1;if((_tmp2E4.Int_c).tag != 4)goto _LL13D;_tmp2E5=(struct
_tuple6)(_tmp2E4.Int_c).val;_tmp2E6=_tmp2E5.f1;if(_tmp2E6 != Cyc_Absyn_None)goto
_LL13D;_tmp2E7=_tmp2E5.f2;}}_LL13C: _tmp2EC=_tmp2E7;goto _LL13E;_LL13D: {struct Cyc_Absyn_Const_e_struct*
_tmp2E8=(struct Cyc_Absyn_Const_e_struct*)_tmp2DF;if(_tmp2E8->tag != 0)goto _LL13F;
else{_tmp2E9=_tmp2E8->f1;if((_tmp2E9.Int_c).tag != 4)goto _LL13F;_tmp2EA=(struct
_tuple6)(_tmp2E9.Int_c).val;_tmp2EB=_tmp2EA.f1;if(_tmp2EB != Cyc_Absyn_Signed)
goto _LL13F;_tmp2EC=_tmp2EA.f2;}}_LL13E: return _tmp2EC >= 0  && Cyc_Toc_check_const_array((
unsigned int)(_tmp2EC + 1),_tmp2DE->type);_LL13F: {struct Cyc_Absyn_Const_e_struct*
_tmp2ED=(struct Cyc_Absyn_Const_e_struct*)_tmp2DF;if(_tmp2ED->tag != 0)goto _LL141;
else{_tmp2EE=_tmp2ED->f1;if((_tmp2EE.Int_c).tag != 4)goto _LL141;_tmp2EF=(struct
_tuple6)(_tmp2EE.Int_c).val;_tmp2F0=_tmp2EF.f1;if(_tmp2F0 != Cyc_Absyn_Unsigned)
goto _LL141;_tmp2F1=_tmp2EF.f2;}}_LL140: return Cyc_Toc_check_const_array((
unsigned int)(_tmp2F1 + 1),_tmp2DE->type);_LL141: {struct Cyc_Absyn_Primop_e_struct*
_tmp2F2=(struct Cyc_Absyn_Primop_e_struct*)_tmp2DF;if(_tmp2F2->tag != 3)goto _LL143;
else{_tmp2F3=_tmp2F2->f1;if(_tmp2F3 != Cyc_Absyn_Mod)goto _LL143;_tmp2F4=_tmp2F2->f2;
if(_tmp2F4 == 0)goto _LL143;_tmp2F5=*_tmp2F4;_tmp2F6=(struct Cyc_Absyn_Exp*)_tmp2F5.hd;
_tmp2F7=_tmp2F5.tl;if(_tmp2F7 == 0)goto _LL143;_tmp2F8=*_tmp2F7;_tmp2F9=(struct Cyc_Absyn_Exp*)
_tmp2F8.hd;}}_LL142: return Cyc_Toc_check_leq_size(relns,_tmp2DE,_tmp2F9);_LL143: {
struct Cyc_Absyn_Var_e_struct*_tmp2FA=(struct Cyc_Absyn_Var_e_struct*)_tmp2DF;if(
_tmp2FA->tag != 1)goto _LL145;else{_tmp2FB=(void*)_tmp2FA->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp2FC=(struct Cyc_Absyn_Pat_b_struct*)_tmp2FB;if(_tmp2FC->tag != 5)goto _LL145;
else{_tmp2FD=_tmp2FC->f1;}}}}_LL144: _tmp301=_tmp2FD;goto _LL146;_LL145: {struct
Cyc_Absyn_Var_e_struct*_tmp2FE=(struct Cyc_Absyn_Var_e_struct*)_tmp2DF;if(_tmp2FE->tag
!= 1)goto _LL147;else{_tmp2FF=(void*)_tmp2FE->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp300=(struct Cyc_Absyn_Local_b_struct*)_tmp2FF;if(_tmp300->tag != 4)goto _LL147;
else{_tmp301=_tmp300->f1;}}}}_LL146: _tmp305=_tmp301;goto _LL148;_LL147: {struct
Cyc_Absyn_Var_e_struct*_tmp302=(struct Cyc_Absyn_Var_e_struct*)_tmp2DF;if(_tmp302->tag
!= 1)goto _LL149;else{_tmp303=(void*)_tmp302->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp304=(struct Cyc_Absyn_Global_b_struct*)_tmp303;if(_tmp304->tag != 1)goto _LL149;
else{_tmp305=_tmp304->f1;}}}}_LL148: _tmp309=_tmp305;goto _LL14A;_LL149: {struct
Cyc_Absyn_Var_e_struct*_tmp306=(struct Cyc_Absyn_Var_e_struct*)_tmp2DF;if(_tmp306->tag
!= 1)goto _LL14B;else{_tmp307=(void*)_tmp306->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp308=(struct Cyc_Absyn_Param_b_struct*)_tmp307;if(_tmp308->tag != 3)goto _LL14B;
else{_tmp309=_tmp308->f1;}}}}_LL14A: if(_tmp309->escapes)return 0;{struct Cyc_List_List*
_tmp30A=relns;for(0;_tmp30A != 0;_tmp30A=_tmp30A->tl){struct Cyc_CfFlowInfo_Reln*
_tmp30B=(struct Cyc_CfFlowInfo_Reln*)_tmp30A->hd;if(_tmp30B->vd == _tmp309){union
Cyc_CfFlowInfo_RelnOp _tmp30C=_tmp30B->rop;struct Cyc_Absyn_Vardecl*_tmp30D;struct
_tuple12 _tmp30E;struct Cyc_Absyn_Vardecl*_tmp30F;void*_tmp310;unsigned int _tmp311;
_LL14E: if((_tmp30C.LessNumelts).tag != 3)goto _LL150;_tmp30D=(struct Cyc_Absyn_Vardecl*)(
_tmp30C.LessNumelts).val;_LL14F: if(_tmp2DE == _tmp30D)return 1;else{goto _LL14D;}
_LL150: if((_tmp30C.LessVar).tag != 2)goto _LL152;_tmp30E=(struct _tuple12)(_tmp30C.LessVar).val;
_tmp30F=_tmp30E.f1;_tmp310=_tmp30E.f2;_LL151:{struct _tuple0 _tmpB15;struct _tuple0
_tmp313=(_tmpB15.f1=Cyc_Tcutil_compress(_tmp310),((_tmpB15.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(a->topt))->v),_tmpB15)));void*_tmp314;
void*_tmp316;void*_tmp317;struct Cyc_Absyn_PtrInfo _tmp319;struct Cyc_Absyn_PtrAtts
_tmp31A;union Cyc_Absyn_Constraint*_tmp31B;_LL157: _tmp314=_tmp313.f1;{struct Cyc_Absyn_TagType_struct*
_tmp315=(struct Cyc_Absyn_TagType_struct*)_tmp314;if(_tmp315->tag != 20)goto _LL159;
else{_tmp316=(void*)_tmp315->f1;}}_tmp317=_tmp313.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp318=(struct Cyc_Absyn_PointerType_struct*)_tmp317;if(_tmp318->tag != 5)goto
_LL159;else{_tmp319=_tmp318->f1;_tmp31A=_tmp319.ptr_atts;_tmp31B=_tmp31A.bounds;}}
_LL158:{void*_tmp31C=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(
_tmp31B);struct Cyc_Absyn_Exp*_tmp31E;_LL15C: {struct Cyc_Absyn_Upper_b_struct*
_tmp31D=(struct Cyc_Absyn_Upper_b_struct*)_tmp31C;if(_tmp31D->tag != 1)goto _LL15E;
else{_tmp31E=_tmp31D->f1;}}_LL15D: {struct Cyc_Absyn_Exp*_tmp31F=Cyc_Absyn_cast_exp(
Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp316,0),0,Cyc_Absyn_No_coercion,0);
if(Cyc_Evexp_lte_const_exp(_tmp31F,_tmp31E))return 1;goto _LL15B;}_LL15E:;_LL15F:
goto _LL15B;_LL15B:;}goto _LL156;_LL159:;_LL15A: goto _LL156;_LL156:;}{struct Cyc_List_List*
_tmp320=relns;for(0;_tmp320 != 0;_tmp320=_tmp320->tl){struct Cyc_CfFlowInfo_Reln*
_tmp321=(struct Cyc_CfFlowInfo_Reln*)_tmp320->hd;if(_tmp321->vd == _tmp30F){union
Cyc_CfFlowInfo_RelnOp _tmp322=_tmp321->rop;struct Cyc_Absyn_Vardecl*_tmp323;struct
Cyc_Absyn_Vardecl*_tmp324;unsigned int _tmp325;struct _tuple12 _tmp326;struct Cyc_Absyn_Vardecl*
_tmp327;_LL161: if((_tmp322.LessEqNumelts).tag != 5)goto _LL163;_tmp323=(struct Cyc_Absyn_Vardecl*)(
_tmp322.LessEqNumelts).val;_LL162: _tmp324=_tmp323;goto _LL164;_LL163: if((_tmp322.LessNumelts).tag
!= 3)goto _LL165;_tmp324=(struct Cyc_Absyn_Vardecl*)(_tmp322.LessNumelts).val;
_LL164: if(_tmp2DE == _tmp324)return 1;goto _LL160;_LL165: if((_tmp322.EqualConst).tag
!= 1)goto _LL167;_tmp325=(unsigned int)(_tmp322.EqualConst).val;_LL166: return Cyc_Toc_check_const_array(
_tmp325,_tmp2DE->type);_LL167: if((_tmp322.LessVar).tag != 2)goto _LL169;_tmp326=(
struct _tuple12)(_tmp322.LessVar).val;_tmp327=_tmp326.f1;_LL168: if(Cyc_Toc_check_leq_size_var(
relns,_tmp2DE,_tmp327))return 1;goto _LL160;_LL169:;_LL16A: goto _LL160;_LL160:;}}}
goto _LL14D;_LL152: if((_tmp30C.LessConst).tag != 4)goto _LL154;_tmp311=(
unsigned int)(_tmp30C.LessConst).val;_LL153: return Cyc_Toc_check_const_array(
_tmp311,_tmp2DE->type);_LL154:;_LL155: goto _LL14D;_LL14D:;}}}goto _LL138;_LL14B:;
_LL14C: goto _LL138;_LL138:;}goto _LL12D;_LL136:;_LL137: goto _LL12D;_LL12D:;}return 0;}
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB18;void*_tmpB17;(_tmpB17=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB18="Missing type in primop ",_tag_dyneither(_tmpB18,sizeof(char),24))),
_tag_dyneither(_tmpB17,sizeof(void*),0)));}return Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(
e->topt == 0){const char*_tmpB1B;void*_tmpB1A;(_tmpB1A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1B="Missing type in primop ",
_tag_dyneither(_tmpB1B,sizeof(char),24))),_tag_dyneither(_tmpB1A,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct
_tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e);static struct _tuple10*Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp*e){struct _tuple10*_tmpB1C;return(_tmpB1C=_cycalloc(sizeof(*
_tmpB1C)),((_tmpB1C->f1=Cyc_Toc_mt_tq,((_tmpB1C->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpB1C)))));}static struct _tuple16*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static struct
_tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple16*_tmpB1D;return(_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((
_tmpB1D->f1=0,((_tmpB1D->f2=e,_tmpB1D)))));}}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic);static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp32E=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp32E): Cyc_Toc_malloc_ptr(_tmp32E));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp32E);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp32F=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp32F != 0;_tmp32F=_tmp32F->tl){struct _tuple16 _tmp331;struct Cyc_List_List*
_tmp332;struct Cyc_Absyn_Exp*_tmp333;struct _tuple16*_tmp330=(struct _tuple16*)
_tmp32F->hd;_tmp331=*_tmp330;_tmp332=_tmp331.f1;_tmp333=_tmp331.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp332 == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp332->tl != 0){const char*_tmpB20;void*_tmpB1F;(_tmpB1F=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB20="multiple designators in array",
_tag_dyneither(_tmpB20,sizeof(char),30))),_tag_dyneither(_tmpB1F,sizeof(void*),0)));}{
void*_tmp336=(void*)_tmp332->hd;void*_tmp337=_tmp336;struct Cyc_Absyn_Exp*_tmp339;
_LL16C: {struct Cyc_Absyn_ArrayElement_struct*_tmp338=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp337;if(_tmp338->tag != 0)goto _LL16E;else{_tmp339=_tmp338->f1;}}_LL16D: Cyc_Toc_exp_to_c(
nv,_tmp339);e_index=_tmp339;goto _LL16B;_LL16E: {struct Cyc_Absyn_FieldName_struct*
_tmp33A=(struct Cyc_Absyn_FieldName_struct*)_tmp337;if(_tmp33A->tag != 1)goto
_LL16B;}_LL16F: {const char*_tmpB23;void*_tmpB22;(_tmpB22=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB23="field name designators in array",
_tag_dyneither(_tmpB23,sizeof(char),32))),_tag_dyneither(_tmpB22,sizeof(void*),0)));}
_LL16B:;}}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp33D=_tmp333->r;struct Cyc_List_List*_tmp33F;struct Cyc_Absyn_Vardecl*_tmp341;
struct Cyc_Absyn_Exp*_tmp342;struct Cyc_Absyn_Exp*_tmp343;int _tmp344;void*_tmp346;
struct Cyc_List_List*_tmp347;_LL171: {struct Cyc_Absyn_Array_e_struct*_tmp33E=(
struct Cyc_Absyn_Array_e_struct*)_tmp33D;if(_tmp33E->tag != 28)goto _LL173;else{
_tmp33F=_tmp33E->f1;}}_LL172: s=Cyc_Toc_init_array(nv,lval,_tmp33F,s);goto _LL170;
_LL173: {struct Cyc_Absyn_Comprehension_e_struct*_tmp340=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp33D;if(_tmp340->tag != 29)goto _LL175;else{_tmp341=_tmp340->f1;_tmp342=_tmp340->f2;
_tmp343=_tmp340->f3;_tmp344=_tmp340->f4;}}_LL174: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp341,_tmp342,_tmp343,_tmp344,s,0);goto _LL170;_LL175: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp345=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp33D;if(_tmp345->tag != 31)goto
_LL177;else{_tmp346=(void*)_tmp345->f1;_tmp347=_tmp345->f2;}}_LL176: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp346,_tmp347,s);goto _LL170;_LL177:;_LL178: Cyc_Toc_exp_to_c(nv,_tmp333);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
_tmp333,0),s,0);goto _LL170;_LL170:;}}}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple1*_tmp348=vd->name;void*_tmp349=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp34B;struct _RegionHandle*_tmp34C;
struct Cyc_Toc_Env*_tmp34A=nv;_tmp34B=*_tmp34A;_tmp34C=_tmp34B.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB26;struct Cyc_Absyn_Local_b_struct*_tmpB25;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp34C,nv,_tmp348,Cyc_Absyn_varb_exp(_tmp348,(void*)((_tmpB25=_cycalloc(sizeof(*
_tmpB25)),((_tmpB25[0]=((_tmpB26.tag=4,((_tmpB26.f1=vd,_tmpB26)))),_tmpB25)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp348,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp348,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp348,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp348,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp34D=e2->r;struct Cyc_List_List*_tmp34F;
struct Cyc_Absyn_Vardecl*_tmp351;struct Cyc_Absyn_Exp*_tmp352;struct Cyc_Absyn_Exp*
_tmp353;int _tmp354;void*_tmp356;struct Cyc_List_List*_tmp357;_LL17A: {struct Cyc_Absyn_Array_e_struct*
_tmp34E=(struct Cyc_Absyn_Array_e_struct*)_tmp34D;if(_tmp34E->tag != 28)goto _LL17C;
else{_tmp34F=_tmp34E->f1;}}_LL17B: body=Cyc_Toc_init_array(nv2,lval,_tmp34F,Cyc_Toc_skip_stmt_dl());
goto _LL179;_LL17C: {struct Cyc_Absyn_Comprehension_e_struct*_tmp350=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp34D;if(_tmp350->tag != 29)goto _LL17E;else{_tmp351=_tmp350->f1;_tmp352=_tmp350->f2;
_tmp353=_tmp350->f3;_tmp354=_tmp350->f4;}}_LL17D: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp351,_tmp352,_tmp353,_tmp354,Cyc_Toc_skip_stmt_dl(),0);goto _LL179;
_LL17E: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp355=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp34D;if(_tmp355->tag != 31)goto _LL180;else{_tmp356=(void*)_tmp355->f1;_tmp357=
_tmp355->f2;}}_LL17F: body=Cyc_Toc_init_anon_struct(nv,lval,_tmp356,_tmp357,Cyc_Toc_skip_stmt_dl());
goto _LL179;_LL180:;_LL181: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL179;_LL179:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp349,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp348,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);}}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp35A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp35A != 0;_tmp35A=_tmp35A->tl){struct _tuple16 _tmp35C;
struct Cyc_List_List*_tmp35D;struct Cyc_Absyn_Exp*_tmp35E;struct _tuple16*_tmp35B=(
struct _tuple16*)_tmp35A->hd;_tmp35C=*_tmp35B;_tmp35D=_tmp35C.f1;_tmp35E=_tmp35C.f2;
if(_tmp35D == 0){const char*_tmpB29;void*_tmpB28;(_tmpB28=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB29="empty designator list",
_tag_dyneither(_tmpB29,sizeof(char),22))),_tag_dyneither(_tmpB28,sizeof(void*),0)));}
if(_tmp35D->tl != 0){const char*_tmpB2C;void*_tmpB2B;(_tmpB2B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2C="too many designators in anonymous struct",
_tag_dyneither(_tmpB2C,sizeof(char),41))),_tag_dyneither(_tmpB2B,sizeof(void*),0)));}{
void*_tmp363=(void*)_tmp35D->hd;struct _dyneither_ptr*_tmp365;_LL183: {struct Cyc_Absyn_FieldName_struct*
_tmp364=(struct Cyc_Absyn_FieldName_struct*)_tmp363;if(_tmp364->tag != 1)goto
_LL185;else{_tmp365=_tmp364->f1;}}_LL184: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(
lhs,_tmp365,0);{void*_tmp366=_tmp35E->r;struct Cyc_List_List*_tmp368;struct Cyc_Absyn_Vardecl*
_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;struct Cyc_Absyn_Exp*_tmp36C;int _tmp36D;void*
_tmp36F;struct Cyc_List_List*_tmp370;_LL188: {struct Cyc_Absyn_Array_e_struct*
_tmp367=(struct Cyc_Absyn_Array_e_struct*)_tmp366;if(_tmp367->tag != 28)goto _LL18A;
else{_tmp368=_tmp367->f1;}}_LL189: s=Cyc_Toc_init_array(nv,lval,_tmp368,s);goto
_LL187;_LL18A: {struct Cyc_Absyn_Comprehension_e_struct*_tmp369=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp366;if(_tmp369->tag != 29)goto _LL18C;else{_tmp36A=_tmp369->f1;_tmp36B=_tmp369->f2;
_tmp36C=_tmp369->f3;_tmp36D=_tmp369->f4;}}_LL18B: s=Cyc_Toc_init_comprehension(nv,
lval,_tmp36A,_tmp36B,_tmp36C,_tmp36D,s,0);goto _LL187;_LL18C: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp36E=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp366;if(_tmp36E->tag != 31)goto
_LL18E;else{_tmp36F=(void*)_tmp36E->f1;_tmp370=_tmp36E->f2;}}_LL18D: s=Cyc_Toc_init_anon_struct(
nv,lval,_tmp36F,_tmp370,s);goto _LL187;_LL18E:;_LL18F: Cyc_Toc_exp_to_c(nv,_tmp35E);
if(Cyc_Toc_is_poly_field(struct_type,_tmp365))_tmp35E=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp35E);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp35E,0),0),s,0);goto _LL187;_LL187:;}goto _LL182;}_LL185:;_LL186: {const char*
_tmpB2F;void*_tmpB2E;(_tmpB2E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2F="array designator in struct",
_tag_dyneither(_tmpB2F,sizeof(char),27))),_tag_dyneither(_tmpB2E,sizeof(void*),0)));}
_LL182:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es);static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*es){struct Cyc_Toc_Env _tmp374;struct _RegionHandle*_tmp375;
struct Cyc_Toc_Env*_tmp373=nv;_tmp374=*_tmp373;_tmp375=_tmp374.rgn;{struct Cyc_List_List*
_tmp376=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple10*(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp375,Cyc_Toc_tup_to_c,
es);void*_tmp377=Cyc_Toc_add_tuple_type(_tmp376);struct _tuple1*_tmp378=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp379=Cyc_Absyn_var_exp(_tmp378,0);struct Cyc_Absyn_Stmt*
_tmp37A=Cyc_Absyn_exp_stmt(_tmp379,0);struct Cyc_Absyn_Exp*(*_tmp37B)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp37C=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp375,es);{int i=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp37C);for(0;_tmp37C != 0;(
_tmp37C=_tmp37C->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp37C->hd;
struct Cyc_Absyn_Exp*lval=_tmp37B(_tmp379,Cyc_Absyn_fieldname(i),0);is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp37D=e->r;struct Cyc_List_List*_tmp37F;struct Cyc_Absyn_Vardecl*_tmp381;
struct Cyc_Absyn_Exp*_tmp382;struct Cyc_Absyn_Exp*_tmp383;int _tmp384;_LL191: {
struct Cyc_Absyn_Array_e_struct*_tmp37E=(struct Cyc_Absyn_Array_e_struct*)_tmp37D;
if(_tmp37E->tag != 28)goto _LL193;else{_tmp37F=_tmp37E->f1;}}_LL192: _tmp37A=Cyc_Toc_init_array(
nv,lval,_tmp37F,_tmp37A);goto _LL190;_LL193: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp380=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp37D;if(_tmp380->tag != 29)
goto _LL195;else{_tmp381=_tmp380->f1;_tmp382=_tmp380->f2;_tmp383=_tmp380->f3;
_tmp384=_tmp380->f4;}}_LL194: _tmp37A=Cyc_Toc_init_comprehension(nv,lval,_tmp381,
_tmp382,_tmp383,_tmp384,_tmp37A,0);goto _LL190;_LL195:;_LL196: Cyc_Toc_exp_to_c(nv,
e);_tmp37A=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp37B(
_tmp379,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp37A,0);goto _LL190;_LL190:;}}}
return Cyc_Toc_make_struct(nv,_tmp378,_tmp377,_tmp37A,pointer,rgnopt,is_atomic);}}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f);static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,
struct _dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp385=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp385 != 0;_tmp385=_tmp385->tl){struct Cyc_Absyn_Aggrfield*
_tmp386=(struct Cyc_Absyn_Aggrfield*)_tmp385->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp386->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB37;void*_tmpB36[1];const char*_tmpB35;void*_tmpB34;(
_tmpB34=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB37.tag=0,((_tmpB37.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB36[0]=& _tmpB37,Cyc_aprintf(((_tmpB35="get_member_offset %s failed",
_tag_dyneither(_tmpB35,sizeof(char),28))),_tag_dyneither(_tmpB36,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB34,sizeof(void*),0)));}}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn);static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*
rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp38B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp38C=Cyc_Absyn_var_exp(_tmp38B,0);struct Cyc_Absyn_Stmt*
_tmp38D=Cyc_Absyn_exp_stmt(_tmp38C,0);struct Cyc_Absyn_Exp*(*_tmp38E)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp38F=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp390=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp392;union Cyc_Absyn_AggrInfoU _tmp393;
_LL198: {struct Cyc_Absyn_AggrType_struct*_tmp391=(struct Cyc_Absyn_AggrType_struct*)
_tmp390;if(_tmp391->tag != 12)goto _LL19A;else{_tmp392=_tmp391->f1;_tmp393=_tmp392.aggr_info;}}
_LL199: ad=Cyc_Absyn_get_known_aggrdecl(_tmp393);goto _LL197;_LL19A:;_LL19B: {
const char*_tmpB3A;void*_tmpB39;(_tmpB39=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3A="init_struct: bad struct type",
_tag_dyneither(_tmpB3A,sizeof(char),29))),_tag_dyneither(_tmpB39,sizeof(void*),0)));}
_LL197:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp397;struct _RegionHandle*_tmp398;struct Cyc_Toc_Env*_tmp396=
nv;_tmp397=*_tmp396;_tmp398=_tmp397.rgn;{struct Cyc_List_List*_tmp399=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp398,dles);for(0;
_tmp399 != 0;_tmp399=_tmp399->tl){struct _tuple16 _tmp39B;struct Cyc_List_List*
_tmp39C;struct Cyc_Absyn_Exp*_tmp39D;struct _tuple16*_tmp39A=(struct _tuple16*)
_tmp399->hd;_tmp39B=*_tmp39A;_tmp39C=_tmp39B.f1;_tmp39D=_tmp39B.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp39D->topt))->v);
if(_tmp39C == 0){const char*_tmpB3D;void*_tmpB3C;(_tmpB3C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3D="empty designator list",
_tag_dyneither(_tmpB3D,sizeof(char),22))),_tag_dyneither(_tmpB3C,sizeof(void*),0)));}
if(_tmp39C->tl != 0){struct _tuple1*_tmp3A0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3A1=Cyc_Absyn_var_exp(_tmp3A0,0);for(0;_tmp39C != 0;_tmp39C=_tmp39C->tl){void*
_tmp3A2=(void*)_tmp39C->hd;struct _dyneither_ptr*_tmp3A4;_LL19D: {struct Cyc_Absyn_FieldName_struct*
_tmp3A3=(struct Cyc_Absyn_FieldName_struct*)_tmp3A2;if(_tmp3A3->tag != 1)goto
_LL19F;else{_tmp3A4=_tmp3A3->f1;}}_LL19E: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3A4))_tmp3A1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A1);_tmp38D=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38E(_tmp38C,_tmp3A4,0),_tmp3A1,0),0),
_tmp38D,0);goto _LL19C;_LL19F:;_LL1A0: {const char*_tmpB40;void*_tmpB3F;(_tmpB3F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB40="array designator in struct",_tag_dyneither(_tmpB40,sizeof(char),27))),
_tag_dyneither(_tmpB3F,sizeof(void*),0)));}_LL19C:;}Cyc_Toc_exp_to_c(nv,_tmp39D);
_tmp38D=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3A1,
_tmp39D,0),0),_tmp38D,0);}else{void*_tmp3A7=(void*)_tmp39C->hd;struct
_dyneither_ptr*_tmp3A9;_LL1A2: {struct Cyc_Absyn_FieldName_struct*_tmp3A8=(struct
Cyc_Absyn_FieldName_struct*)_tmp3A7;if(_tmp3A8->tag != 1)goto _LL1A4;else{_tmp3A9=
_tmp3A8->f1;}}_LL1A3: {struct Cyc_Absyn_Exp*lval=_tmp38E(_tmp38C,_tmp3A9,0);if(
is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,_tmp3A9);struct Cyc_Absyn_Exp*
f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*lhs=Cyc_Absyn_aggrmember_exp(
lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,
f_tag_exp,0);_tmp38D=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp38D,0);
lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{void*_tmp3AA=_tmp39D->r;
struct Cyc_List_List*_tmp3AC;struct Cyc_Absyn_Vardecl*_tmp3AE;struct Cyc_Absyn_Exp*
_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;int _tmp3B1;void*_tmp3B3;struct Cyc_List_List*
_tmp3B4;_LL1A7: {struct Cyc_Absyn_Array_e_struct*_tmp3AB=(struct Cyc_Absyn_Array_e_struct*)
_tmp3AA;if(_tmp3AB->tag != 28)goto _LL1A9;else{_tmp3AC=_tmp3AB->f1;}}_LL1A8:
_tmp38D=Cyc_Toc_init_array(nv,lval,_tmp3AC,_tmp38D);goto _LL1A6;_LL1A9: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp3AD=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3AA;if(_tmp3AD->tag != 29)goto _LL1AB;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;
_tmp3B0=_tmp3AD->f3;_tmp3B1=_tmp3AD->f4;}}_LL1AA: _tmp38D=Cyc_Toc_init_comprehension(
nv,lval,_tmp3AE,_tmp3AF,_tmp3B0,_tmp3B1,_tmp38D,0);goto _LL1A6;_LL1AB: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp3B2=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3AA;if(_tmp3B2->tag != 31)goto
_LL1AD;else{_tmp3B3=(void*)_tmp3B2->f1;_tmp3B4=_tmp3B2->f2;}}_LL1AC: _tmp38D=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3B3,_tmp3B4,_tmp38D);goto _LL1A6;_LL1AD:;_LL1AE: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp39D->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp39D);{struct Cyc_Absyn_Aggrfield*_tmp3B5=Cyc_Absyn_lookup_decl_field(ad,
_tmp3A9);if(Cyc_Toc_is_poly_field(struct_type,_tmp3A9) && !was_ptr_type)_tmp39D=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp39D);if(has_exists)_tmp39D=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp3B5))->type),
_tmp39D);_tmp38D=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp39D,0),0),_tmp38D,0);goto _LL1A6;}}_LL1A6:;}goto _LL1A1;}_LL1A4:;_LL1A5: {
const char*_tmpB43;void*_tmpB42;(_tmpB42=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB43="array designator in struct",
_tag_dyneither(_tmpB43,sizeof(char),27))),_tag_dyneither(_tmpB42,sizeof(void*),0)));}
_LL1A1:;}}}return Cyc_Toc_make_struct(nv,_tmp38B,_tmp38F,_tmp38D,pointer,rgnopt,
is_atomic);}}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr);static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*
el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int
ignore);static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,
int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr);static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_struct _tmpB46;struct Cyc_Absyn_Increment_e_struct*
_tmpB45;return Cyc_Absyn_new_exp((void*)((_tmpB45=_cycalloc(sizeof(*_tmpB45)),((
_tmpB45[0]=((_tmpB46.tag=5,((_tmpB46.f1=e1,((_tmpB46.f2=incr,_tmpB46)))))),
_tmpB45)))),0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*
f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3BA=e1->r;struct Cyc_Absyn_Stmt*
_tmp3BC;void*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C1;
struct _dyneither_ptr*_tmp3C2;int _tmp3C3;int _tmp3C4;_LL1B0: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp3BB=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp3BA;if(_tmp3BB->tag != 38)goto
_LL1B2;else{_tmp3BC=_tmp3BB->f1;}}_LL1B1: Cyc_Toc_lvalue_assign_stmt(_tmp3BC,fs,f,
f_env);goto _LL1AF;_LL1B2: {struct Cyc_Absyn_Cast_e_struct*_tmp3BD=(struct Cyc_Absyn_Cast_e_struct*)
_tmp3BA;if(_tmp3BD->tag != 15)goto _LL1B4;else{_tmp3BE=(void*)_tmp3BD->f1;_tmp3BF=
_tmp3BD->f2;}}_LL1B3: Cyc_Toc_lvalue_assign(_tmp3BF,fs,f,f_env);goto _LL1AF;_LL1B4: {
struct Cyc_Absyn_AggrMember_e_struct*_tmp3C0=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3BA;if(_tmp3C0->tag != 23)goto _LL1B6;else{_tmp3C1=_tmp3C0->f1;_tmp3C2=_tmp3C0->f2;
_tmp3C3=_tmp3C0->f3;_tmp3C4=_tmp3C0->f4;}}_LL1B5: e1->r=_tmp3C1->r;{struct Cyc_List_List*
_tmpB47;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpB47=_cycalloc(
sizeof(*_tmpB47)),((_tmpB47->hd=_tmp3C2,((_tmpB47->tl=fs,_tmpB47)))))),f,f_env);}
goto _LL1AF;_LL1B6:;_LL1B7: {struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct
_dyneither_ptr*)fs->hd,e1_copy->loc);}e1->r=(f(e1_copy,f_env))->r;goto _LL1AF;}
_LL1AF:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp3C6=s->r;
struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Decl*_tmp3CA;struct Cyc_Absyn_Stmt*
_tmp3CB;struct Cyc_Absyn_Stmt*_tmp3CD;_LL1B9: {struct Cyc_Absyn_Exp_s_struct*
_tmp3C7=(struct Cyc_Absyn_Exp_s_struct*)_tmp3C6;if(_tmp3C7->tag != 1)goto _LL1BB;
else{_tmp3C8=_tmp3C7->f1;}}_LL1BA: Cyc_Toc_lvalue_assign(_tmp3C8,fs,f,f_env);goto
_LL1B8;_LL1BB: {struct Cyc_Absyn_Decl_s_struct*_tmp3C9=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3C6;if(_tmp3C9->tag != 12)goto _LL1BD;else{_tmp3CA=_tmp3C9->f1;_tmp3CB=_tmp3C9->f2;}}
_LL1BC: Cyc_Toc_lvalue_assign_stmt(_tmp3CB,fs,f,f_env);goto _LL1B8;_LL1BD: {struct
Cyc_Absyn_Seq_s_struct*_tmp3CC=(struct Cyc_Absyn_Seq_s_struct*)_tmp3C6;if(_tmp3CC->tag
!= 2)goto _LL1BF;else{_tmp3CD=_tmp3CC->f2;}}_LL1BE: Cyc_Toc_lvalue_assign_stmt(
_tmp3CD,fs,f,f_env);goto _LL1B8;_LL1BF:;_LL1C0: {const char*_tmpB4B;void*_tmpB4A[1];
struct Cyc_String_pa_struct _tmpB49;(_tmpB49.tag=0,((_tmpB49.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB4A[0]=&
_tmpB49,Cyc_Toc_toc_impos(((_tmpB4B="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpB4B,sizeof(char),23))),_tag_dyneither(_tmpB4A,sizeof(void*),1)))))));}_LL1B8:;}
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e);static
struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){void*_tmp3D1=
e->r;void*_tmp3D3;void**_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Exp**
_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D8;_LL1C2: {struct Cyc_Absyn_Cast_e_struct*
_tmp3D2=(struct Cyc_Absyn_Cast_e_struct*)_tmp3D1;if(_tmp3D2->tag != 15)goto _LL1C4;
else{_tmp3D3=(void**)& _tmp3D2->f1;_tmp3D4=(void**)((void**)& _tmp3D2->f1);_tmp3D5=
_tmp3D2->f2;_tmp3D6=(struct Cyc_Absyn_Exp**)& _tmp3D2->f2;}}_LL1C3:*_tmp3D6=Cyc_Toc_push_address_exp(*
_tmp3D6);*_tmp3D4=Cyc_Absyn_cstar_typ(*_tmp3D4,Cyc_Toc_mt_tq);return e;_LL1C4: {
struct Cyc_Absyn_Deref_e_struct*_tmp3D7=(struct Cyc_Absyn_Deref_e_struct*)_tmp3D1;
if(_tmp3D7->tag != 22)goto _LL1C6;else{_tmp3D8=_tmp3D7->f1;}}_LL1C5: return _tmp3D8;
_LL1C6:;_LL1C7: if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{const
char*_tmpB4F;void*_tmpB4E[1];struct Cyc_String_pa_struct _tmpB4D;(_tmpB4D.tag=0,((
_tmpB4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpB4E[0]=& _tmpB4D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB4F="can't take & of %s",_tag_dyneither(_tmpB4F,sizeof(char),19))),
_tag_dyneither(_tmpB4E,sizeof(void*),1)))))));}_LL1C1:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct
Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpB50;result=((
_tmpB50=_region_malloc(r2,sizeof(*_tmpB50)),((_tmpB50->hd=(void*)f((void*)x->hd,
env),((_tmpB50->tl=0,_tmpB50))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpB51;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpB51=_region_malloc(r2,sizeof(*_tmpB51)),((_tmpB51->hd=(void*)f((void*)x->hd,
env),((_tmpB51->tl=0,_tmpB51))))));}prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e);static
struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple16*_tmpB52;
return(_tmpB52=_cycalloc(sizeof(*_tmpB52)),((_tmpB52->f1=0,((_tmpB52->f2=e,
_tmpB52)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t);static
struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3DF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp3E1;_LL1C9: {struct Cyc_Absyn_PointerType_struct*
_tmp3E0=(struct Cyc_Absyn_PointerType_struct*)_tmp3DF;if(_tmp3E0->tag != 5)goto
_LL1CB;else{_tmp3E1=_tmp3E0->f1;}}_LL1CA: return _tmp3E1;_LL1CB:;_LL1CC: {const
char*_tmpB55;void*_tmpB54;(_tmpB54=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB55="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB55,sizeof(char),28))),_tag_dyneither(_tmpB54,sizeof(void*),0)));}
_LL1C8:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t);static struct Cyc_Absyn_Exp*
Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*res;{void*_tmp3E4=Cyc_Tcutil_compress(
t);enum Cyc_Absyn_Sign _tmp3E7;enum Cyc_Absyn_Size_of _tmp3E8;enum Cyc_Absyn_Sign
_tmp3EA;enum Cyc_Absyn_Size_of _tmp3EB;enum Cyc_Absyn_Sign _tmp3EF;enum Cyc_Absyn_Size_of
_tmp3F0;enum Cyc_Absyn_Sign _tmp3F2;enum Cyc_Absyn_Size_of _tmp3F3;enum Cyc_Absyn_Sign
_tmp3F5;enum Cyc_Absyn_Size_of _tmp3F6;_LL1CE: {struct Cyc_Absyn_PointerType_struct*
_tmp3E5=(struct Cyc_Absyn_PointerType_struct*)_tmp3E4;if(_tmp3E5->tag != 5)goto
_LL1D0;}_LL1CF: res=Cyc_Absyn_null_exp(0);goto _LL1CD;_LL1D0: {struct Cyc_Absyn_IntType_struct*
_tmp3E6=(struct Cyc_Absyn_IntType_struct*)_tmp3E4;if(_tmp3E6->tag != 6)goto _LL1D2;
else{_tmp3E7=_tmp3E6->f1;_tmp3E8=_tmp3E6->f2;if(_tmp3E8 != Cyc_Absyn_Char_sz)goto
_LL1D2;}}_LL1D1: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp3E7,'\000'),0);goto
_LL1CD;_LL1D2: {struct Cyc_Absyn_IntType_struct*_tmp3E9=(struct Cyc_Absyn_IntType_struct*)
_tmp3E4;if(_tmp3E9->tag != 6)goto _LL1D4;else{_tmp3EA=_tmp3E9->f1;_tmp3EB=_tmp3E9->f2;
if(_tmp3EB != Cyc_Absyn_Short_sz)goto _LL1D4;}}_LL1D3: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp3EA,0),0);goto _LL1CD;_LL1D4: {struct Cyc_Absyn_EnumType_struct*_tmp3EC=(
struct Cyc_Absyn_EnumType_struct*)_tmp3E4;if(_tmp3EC->tag != 14)goto _LL1D6;}_LL1D5:
goto _LL1D7;_LL1D6: {struct Cyc_Absyn_AnonEnumType_struct*_tmp3ED=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3E4;if(_tmp3ED->tag != 15)goto _LL1D8;}_LL1D7: _tmp3EF=Cyc_Absyn_Unsigned;goto
_LL1D9;_LL1D8: {struct Cyc_Absyn_IntType_struct*_tmp3EE=(struct Cyc_Absyn_IntType_struct*)
_tmp3E4;if(_tmp3EE->tag != 6)goto _LL1DA;else{_tmp3EF=_tmp3EE->f1;_tmp3F0=_tmp3EE->f2;
if(_tmp3F0 != Cyc_Absyn_Int_sz)goto _LL1DA;}}_LL1D9: _tmp3F2=_tmp3EF;goto _LL1DB;
_LL1DA: {struct Cyc_Absyn_IntType_struct*_tmp3F1=(struct Cyc_Absyn_IntType_struct*)
_tmp3E4;if(_tmp3F1->tag != 6)goto _LL1DC;else{_tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;
if(_tmp3F3 != Cyc_Absyn_Long_sz)goto _LL1DC;}}_LL1DB: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(
_tmp3F2,0),0);goto _LL1CD;_LL1DC: {struct Cyc_Absyn_IntType_struct*_tmp3F4=(struct
Cyc_Absyn_IntType_struct*)_tmp3E4;if(_tmp3F4->tag != 6)goto _LL1DE;else{_tmp3F5=
_tmp3F4->f1;_tmp3F6=_tmp3F4->f2;if(_tmp3F6 != Cyc_Absyn_LongLong_sz)goto _LL1DE;}}
_LL1DD: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp3F5,(long long)0),0);
goto _LL1CD;_LL1DE: {struct Cyc_Absyn_FloatType_struct*_tmp3F7=(struct Cyc_Absyn_FloatType_struct*)
_tmp3E4;if(_tmp3F7->tag != 7)goto _LL1E0;}_LL1DF: goto _LL1E1;_LL1E0: {struct Cyc_Absyn_DoubleType_struct*
_tmp3F8=(struct Cyc_Absyn_DoubleType_struct*)_tmp3E4;if(_tmp3F8->tag != 8)goto
_LL1E2;}_LL1E1:{const char*_tmpB56;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpB56="0.0",_tag_dyneither(_tmpB56,sizeof(char),4)))),0);}goto _LL1CD;_LL1E2:;
_LL1E3: {const char*_tmpB5A;void*_tmpB59[1];struct Cyc_String_pa_struct _tmpB58;(
_tmpB58.tag=0,((_tmpB58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpB59[0]=& _tmpB58,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB5A="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpB5A,sizeof(char),40))),_tag_dyneither(_tmpB59,sizeof(void*),1)))))));}
_LL1CD:;}{struct Cyc_Core_Opt*_tmpB5B;res->topt=((_tmpB5B=_cycalloc(sizeof(*
_tmpB5B)),((_tmpB5B->v=t,_tmpB5B))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static
void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp3FE=Cyc_Toc_typ_to_c(elt_type);void*_tmp3FF=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp400=Cyc_Absyn_cstar_typ(_tmp3FE,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB5C;struct Cyc_Core_Opt*_tmp401=(_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((
_tmpB5C->v=_tmp400,_tmpB5C)));struct Cyc_Absyn_Exp*xinit;{void*_tmp402=e1->r;
struct Cyc_Absyn_Exp*_tmp404;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*
_tmp407;_LL1E5: {struct Cyc_Absyn_Deref_e_struct*_tmp403=(struct Cyc_Absyn_Deref_e_struct*)
_tmp402;if(_tmp403->tag != 22)goto _LL1E7;else{_tmp404=_tmp403->f1;}}_LL1E6: if(!
is_dyneither){_tmp404=Cyc_Toc_cast_it(fat_ptr_type,_tmp404);{struct Cyc_Core_Opt*
_tmpB5D;_tmp404->topt=((_tmpB5D=_cycalloc(sizeof(*_tmpB5D)),((_tmpB5D->v=
fat_ptr_type,_tmpB5D))));}}Cyc_Toc_exp_to_c(nv,_tmp404);xinit=_tmp404;goto _LL1E4;
_LL1E7: {struct Cyc_Absyn_Subscript_e_struct*_tmp405=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp402;if(_tmp405->tag != 25)goto _LL1E9;else{_tmp406=_tmp405->f1;_tmp407=_tmp405->f2;}}
_LL1E8: if(!is_dyneither){_tmp406=Cyc_Toc_cast_it(fat_ptr_type,_tmp406);{struct
Cyc_Core_Opt*_tmpB5E;_tmp406->topt=((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((
_tmpB5E->v=fat_ptr_type,_tmpB5E))));}}Cyc_Toc_exp_to_c(nv,_tmp406);Cyc_Toc_exp_to_c(
nv,_tmp407);{struct Cyc_Absyn_Exp*_tmpB5F[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB5F[2]=_tmp407,((_tmpB5F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB5F[0]=_tmp406,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB5F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1E4;_LL1E9:;_LL1EA: {const char*_tmpB62;void*_tmpB61;(_tmpB61=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB62="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB62,sizeof(char),53))),_tag_dyneither(_tmpB61,sizeof(void*),0)));}
_LL1E4:;}{struct _tuple1*_tmp40D=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp40F;
struct _RegionHandle*_tmp410;struct Cyc_Toc_Env*_tmp40E=nv;_tmp40F=*_tmp40E;
_tmp410=_tmp40F.rgn;{struct Cyc_Toc_Env*_tmp411=Cyc_Toc_add_varmap(_tmp410,nv,
_tmp40D,Cyc_Absyn_var_exp(_tmp40D,0));struct Cyc_Absyn_Vardecl*_tmpB63;struct Cyc_Absyn_Vardecl*
_tmp412=(_tmpB63=_cycalloc(sizeof(*_tmpB63)),((_tmpB63->sc=Cyc_Absyn_Public,((
_tmpB63->name=_tmp40D,((_tmpB63->tq=Cyc_Toc_mt_tq,((_tmpB63->type=_tmp3FF,((
_tmpB63->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB63->rgn=0,((_tmpB63->attributes=
0,((_tmpB63->escapes=0,_tmpB63)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB66;struct Cyc_Absyn_Local_b_struct*_tmpB65;struct Cyc_Absyn_Local_b_struct*
_tmp413=(_tmpB65=_cycalloc(sizeof(*_tmpB65)),((_tmpB65[0]=((_tmpB66.tag=4,((
_tmpB66.f1=_tmp412,_tmpB66)))),_tmpB65)));struct Cyc_Absyn_Exp*_tmp414=Cyc_Absyn_varb_exp(
_tmp40D,(void*)_tmp413,0);{struct Cyc_Core_Opt*_tmpB67;_tmp414->topt=((_tmpB67=
_cycalloc(sizeof(*_tmpB67)),((_tmpB67->v=fat_ptr_type,_tmpB67))));}{struct Cyc_Absyn_Exp*
_tmp416=Cyc_Absyn_deref_exp(_tmp414,0);{struct Cyc_Core_Opt*_tmpB68;_tmp416->topt=((
_tmpB68=_cycalloc(sizeof(*_tmpB68)),((_tmpB68->v=elt_type,_tmpB68))));}Cyc_Toc_exp_to_c(
_tmp411,_tmp416);{struct _tuple1*_tmp418=Cyc_Toc_temp_var();_tmp411=Cyc_Toc_add_varmap(
_tmp410,_tmp411,_tmp418,Cyc_Absyn_var_exp(_tmp418,0));{struct Cyc_Absyn_Vardecl*
_tmpB69;struct Cyc_Absyn_Vardecl*_tmp419=(_tmpB69=_cycalloc(sizeof(*_tmpB69)),((
_tmpB69->sc=Cyc_Absyn_Public,((_tmpB69->name=_tmp418,((_tmpB69->tq=Cyc_Toc_mt_tq,((
_tmpB69->type=_tmp3FE,((_tmpB69->initializer=(struct Cyc_Absyn_Exp*)_tmp416,((
_tmpB69->rgn=0,((_tmpB69->attributes=0,((_tmpB69->escapes=0,_tmpB69)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB6C;struct Cyc_Absyn_Local_b_struct*_tmpB6B;
struct Cyc_Absyn_Local_b_struct*_tmp41A=(_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((
_tmpB6B[0]=((_tmpB6C.tag=4,((_tmpB6C.f1=_tmp419,_tmpB6C)))),_tmpB6B)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp41B=Cyc_Absyn_varb_exp(_tmp418,(
void*)_tmp41A,0);_tmp41B->topt=_tmp416->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp41B,e2,0);z_init->topt=_tmp41B->topt;}Cyc_Toc_exp_to_c(
_tmp411,z_init);{struct _tuple1*_tmp41C=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB6D;struct Cyc_Absyn_Vardecl*_tmp41D=(_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((
_tmpB6D->sc=Cyc_Absyn_Public,((_tmpB6D->name=_tmp41C,((_tmpB6D->tq=Cyc_Toc_mt_tq,((
_tmpB6D->type=_tmp3FE,((_tmpB6D->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB6D->rgn=0,((_tmpB6D->attributes=0,((_tmpB6D->escapes=0,_tmpB6D)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB70;struct Cyc_Absyn_Local_b_struct*_tmpB6F;
struct Cyc_Absyn_Local_b_struct*_tmp41E=(_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((
_tmpB6F[0]=((_tmpB70.tag=4,((_tmpB70.f1=_tmp41D,_tmpB70)))),_tmpB6F)));_tmp411=
Cyc_Toc_add_varmap(_tmp410,_tmp411,_tmp41C,Cyc_Absyn_var_exp(_tmp41C,0));{struct
Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_varb_exp(_tmp418,(void*)_tmp41A,0);_tmp41F->topt=
_tmp416->topt;{struct Cyc_Absyn_Exp*_tmp420=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp421=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp41F,_tmp420,0);{
struct Cyc_Core_Opt*_tmpB71;_tmp421->topt=((_tmpB71=_cycalloc(sizeof(*_tmpB71)),((
_tmpB71->v=Cyc_Absyn_sint_typ,_tmpB71))));}Cyc_Toc_exp_to_c(_tmp411,_tmp421);{
struct Cyc_Absyn_Exp*_tmp423=Cyc_Absyn_varb_exp(_tmp41C,(void*)_tmp41E,0);_tmp423->topt=
_tmp416->topt;{struct Cyc_Absyn_Exp*_tmp424=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp425=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp423,_tmp424,0);{
struct Cyc_Core_Opt*_tmpB72;_tmp425->topt=((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((
_tmpB72->v=Cyc_Absyn_sint_typ,_tmpB72))));}Cyc_Toc_exp_to_c(_tmp411,_tmp425);{
struct Cyc_Absyn_Exp*_tmpB73[2];struct Cyc_List_List*_tmp427=(_tmpB73[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB73[0]=Cyc_Absyn_varb_exp(_tmp40D,(void*)
_tmp413,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB73,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
_tmp428=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp427,0),
_tmp428,0);{struct Cyc_Absyn_Exp*_tmp429=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp421,_tmp425,0),0);struct Cyc_Absyn_Stmt*_tmp42A=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp42B=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp40D,(void*)_tmp413,0),Cyc_Toc_curr_sp,0);_tmp42B=Cyc_Toc_cast_it(
_tmp400,_tmp42B);{struct Cyc_Absyn_Exp*_tmp42C=Cyc_Absyn_deref_exp(_tmp42B,0);
struct Cyc_Absyn_Exp*_tmp42D=Cyc_Absyn_assign_exp(_tmp42C,Cyc_Absyn_var_exp(
_tmp41C,0),0);struct Cyc_Absyn_Stmt*_tmp42E=Cyc_Absyn_exp_stmt(_tmp42D,0);_tmp42E=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp429,_tmp42A,Cyc_Absyn_skip_stmt(
0),0),_tmp42E,0);{struct Cyc_Absyn_Var_d_struct*_tmpB79;struct Cyc_Absyn_Var_d_struct
_tmpB78;struct Cyc_Absyn_Decl*_tmpB77;_tmp42E=Cyc_Absyn_decl_stmt(((_tmpB77=
_cycalloc(sizeof(*_tmpB77)),((_tmpB77->r=(void*)((_tmpB79=_cycalloc(sizeof(*
_tmpB79)),((_tmpB79[0]=((_tmpB78.tag=0,((_tmpB78.f1=_tmp41D,_tmpB78)))),_tmpB79)))),((
_tmpB77->loc=0,_tmpB77)))))),_tmp42E,0);}{struct Cyc_Absyn_Var_d_struct*_tmpB7F;
struct Cyc_Absyn_Var_d_struct _tmpB7E;struct Cyc_Absyn_Decl*_tmpB7D;_tmp42E=Cyc_Absyn_decl_stmt(((
_tmpB7D=_cycalloc(sizeof(*_tmpB7D)),((_tmpB7D->r=(void*)((_tmpB7F=_cycalloc(
sizeof(*_tmpB7F)),((_tmpB7F[0]=((_tmpB7E.tag=0,((_tmpB7E.f1=_tmp419,_tmpB7E)))),
_tmpB7F)))),((_tmpB7D->loc=0,_tmpB7D)))))),_tmp42E,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpB85;struct Cyc_Absyn_Var_d_struct _tmpB84;struct Cyc_Absyn_Decl*_tmpB83;_tmp42E=
Cyc_Absyn_decl_stmt(((_tmpB83=_cycalloc(sizeof(*_tmpB83)),((_tmpB83->r=(void*)((
_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85[0]=((_tmpB84.tag=0,((_tmpB84.f1=
_tmp412,_tmpB84)))),_tmpB85)))),((_tmpB83->loc=0,_tmpB83)))))),_tmp42E,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp42E);}}}}}}}}}}}}}}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*));static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)){
struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp443=Cyc_Tcutil_compress(aggrtype);struct
Cyc_Absyn_AggrInfo _tmp445;union Cyc_Absyn_AggrInfoU _tmp446;_LL1EC: {struct Cyc_Absyn_AggrType_struct*
_tmp444=(struct Cyc_Absyn_AggrType_struct*)_tmp443;if(_tmp444->tag != 12)goto
_LL1EE;else{_tmp445=_tmp444->f1;_tmp446=_tmp445.aggr_info;}}_LL1ED: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp446);goto _LL1EB;_LL1EE:;_LL1EF: {struct Cyc_String_pa_struct _tmpB8D;void*
_tmpB8C[1];const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpB8D.tag=0,((_tmpB8D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB8C[0]=& _tmpB8D,Cyc_aprintf(((
_tmpB8B="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpB8B,sizeof(char),51))),_tag_dyneither(_tmpB8C,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB8A,sizeof(void*),0)));}_LL1EB:;}{struct _tuple1*_tmp44B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_var_exp(_tmp44B,0);struct Cyc_Absyn_Exp*
_tmp44D=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44C,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp44E=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_neq_exp(_tmp44D,_tmp44E,0);struct Cyc_Absyn_Exp*
_tmp450=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44C,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp451=Cyc_Absyn_exp_stmt(_tmp450,0);struct Cyc_Absyn_Stmt*
_tmp452=Cyc_Absyn_ifthenelse_stmt(_tmp44F,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp453=Cyc_Absyn_declare_stmt(_tmp44B,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp452,_tmp451,0),0);return Cyc_Toc_stmt_exp_r(
_tmp453);}}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read);static int Cyc_Toc_is_tagged_union_project(
struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){void*
_tmp454=e->r;struct Cyc_Absyn_Exp*_tmp456;struct Cyc_Absyn_Exp*_tmp458;struct
_dyneither_ptr*_tmp459;int _tmp45A;int*_tmp45B;struct Cyc_Absyn_Exp*_tmp45D;struct
_dyneither_ptr*_tmp45E;int _tmp45F;int*_tmp460;_LL1F1: {struct Cyc_Absyn_Cast_e_struct*
_tmp455=(struct Cyc_Absyn_Cast_e_struct*)_tmp454;if(_tmp455->tag != 15)goto _LL1F3;
else{_tmp456=_tmp455->f2;}}_LL1F2: {const char*_tmpB90;void*_tmpB8F;(_tmpB8F=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB90="cast on lhs!",_tag_dyneither(_tmpB90,sizeof(char),13))),_tag_dyneither(
_tmpB8F,sizeof(void*),0)));}_LL1F3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp457=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp454;if(_tmp457->tag != 23)goto _LL1F5;
else{_tmp458=_tmp457->f1;_tmp459=_tmp457->f2;_tmp45A=_tmp457->f4;_tmp45B=(int*)&
_tmp457->f4;}}_LL1F4: {void*_tmp463=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp458->topt))->v);struct Cyc_Absyn_AggrInfo _tmp465;union Cyc_Absyn_AggrInfoU
_tmp466;_LL1FA: {struct Cyc_Absyn_AggrType_struct*_tmp464=(struct Cyc_Absyn_AggrType_struct*)
_tmp463;if(_tmp464->tag != 12)goto _LL1FC;else{_tmp465=_tmp464->f1;_tmp466=_tmp465.aggr_info;}}
_LL1FB: {struct Cyc_Absyn_Aggrdecl*_tmp467=Cyc_Absyn_get_known_aggrdecl(_tmp466);*
f_tag=Cyc_Toc_get_member_offset(_tmp467,_tmp459);{const char*_tmpB95;void*_tmpB94[
2];struct Cyc_String_pa_struct _tmpB93;struct Cyc_String_pa_struct _tmpB92;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpB92.tag=0,((_tmpB92.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp459),((_tmpB93.tag=0,((_tmpB93.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp467->name).f2),((_tmpB94[0]=&
_tmpB93,((_tmpB94[1]=& _tmpB92,Cyc_aprintf(((_tmpB95="_union_%s_%s",
_tag_dyneither(_tmpB95,sizeof(char),13))),_tag_dyneither(_tmpB94,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpB96;*tagged_member_type=Cyc_Absyn_strct(((_tmpB96=
_cycalloc(sizeof(*_tmpB96)),((_tmpB96[0]=str,_tmpB96)))));}if(clear_read)*
_tmp45B=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp467->impl))->tagged;}}
_LL1FC:;_LL1FD: return 0;_LL1F9:;}_LL1F5: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp45C=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp454;if(_tmp45C->tag != 24)goto
_LL1F7;else{_tmp45D=_tmp45C->f1;_tmp45E=_tmp45C->f2;_tmp45F=_tmp45C->f4;_tmp460=(
int*)& _tmp45C->f4;}}_LL1F6: {void*_tmp46D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp45D->topt))->v);struct Cyc_Absyn_PtrInfo _tmp46F;void*_tmp470;
_LL1FF: {struct Cyc_Absyn_PointerType_struct*_tmp46E=(struct Cyc_Absyn_PointerType_struct*)
_tmp46D;if(_tmp46E->tag != 5)goto _LL201;else{_tmp46F=_tmp46E->f1;_tmp470=_tmp46F.elt_typ;}}
_LL200: {void*_tmp471=Cyc_Tcutil_compress(_tmp470);struct Cyc_Absyn_AggrInfo
_tmp473;union Cyc_Absyn_AggrInfoU _tmp474;_LL204: {struct Cyc_Absyn_AggrType_struct*
_tmp472=(struct Cyc_Absyn_AggrType_struct*)_tmp471;if(_tmp472->tag != 12)goto
_LL206;else{_tmp473=_tmp472->f1;_tmp474=_tmp473.aggr_info;}}_LL205: {struct Cyc_Absyn_Aggrdecl*
_tmp475=Cyc_Absyn_get_known_aggrdecl(_tmp474);*f_tag=Cyc_Toc_get_member_offset(
_tmp475,_tmp45E);{const char*_tmpB9B;void*_tmpB9A[2];struct Cyc_String_pa_struct
_tmpB99;struct Cyc_String_pa_struct _tmpB98;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpB98.tag=0,((_tmpB98.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp45E),((_tmpB99.tag=0,((_tmpB99.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp475->name).f2),((_tmpB9A[0]=& _tmpB99,((_tmpB9A[1]=&
_tmpB98,Cyc_aprintf(((_tmpB9B="_union_%s_%s",_tag_dyneither(_tmpB9B,sizeof(char),
13))),_tag_dyneither(_tmpB9A,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpB9C;*tagged_member_type=Cyc_Absyn_strct(((_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((
_tmpB9C[0]=str,_tmpB9C)))));}if(clear_read)*_tmp460=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp475->impl))->tagged;}}_LL206:;_LL207: return 0;_LL203:;}_LL201:;
_LL202: return 0;_LL1FE:;}_LL1F7:;_LL1F8: return 0;_LL1F0:;}static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type);static void*Cyc_Toc_tagged_union_assignop(
struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*
e2,void*e2_cyc_type,int tag_num,void*member_type){struct _tuple1*_tmp47B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp47B,0);struct Cyc_Absyn_Exp*
temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*
temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*
f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 
0)s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*
_tmp47C=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp47C,
Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=
Cyc_Absyn_declare_stmt(_tmp47B,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(
struct Cyc_Absyn_Exp*)Cyc_Toc_push_address_exp(e1),Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);}}struct _tuple19{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*
f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static
void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void
_tmpBFB(unsigned int*_tmpBFA,unsigned int*_tmpBF9,struct _tuple1***_tmpBF7){for(*
_tmpBFA=0;*_tmpBFA < *_tmpBF9;(*_tmpBFA)++){(*_tmpBF7)[*_tmpBFA]=Cyc_Toc_temp_var();}}
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*
_tmp47D=e->r;if(e->topt == 0){const char*_tmpBA0;void*_tmpB9F[1];struct Cyc_String_pa_struct
_tmpB9E;(_tmpB9E.tag=0,((_tmpB9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpB9F[0]=& _tmpB9E,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA0="exp_to_c: no type for %s",
_tag_dyneither(_tmpBA0,sizeof(char),25))),_tag_dyneither(_tmpB9F,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp481=
_tmp47D;union Cyc_Absyn_Cnst _tmp483;int _tmp484;struct _tuple1*_tmp487;void*_tmp488;
struct _tuple1*_tmp48A;enum Cyc_Absyn_Primop _tmp48C;struct Cyc_List_List*_tmp48D;
struct Cyc_Absyn_Exp*_tmp48F;enum Cyc_Absyn_Incrementor _tmp490;struct Cyc_Absyn_Exp*
_tmp492;struct Cyc_Core_Opt*_tmp493;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*
_tmp496;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*
_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_Absyn_Exp*
_tmp49E;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*
_tmp4A3;struct Cyc_List_List*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_List_List*
_tmp4A7;struct Cyc_Absyn_VarargCallInfo*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4AA;
struct Cyc_List_List*_tmp4AB;struct Cyc_Absyn_VarargCallInfo*_tmp4AC;struct Cyc_Absyn_VarargCallInfo
_tmp4AD;int _tmp4AE;struct Cyc_List_List*_tmp4AF;struct Cyc_Absyn_VarargInfo*
_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*
_tmp4B6;struct Cyc_List_List*_tmp4B7;void*_tmp4B9;void**_tmp4BA;struct Cyc_Absyn_Exp*
_tmp4BB;int _tmp4BC;enum Cyc_Absyn_Coercion _tmp4BD;struct Cyc_Absyn_Exp*_tmp4BF;
struct Cyc_Absyn_Exp*_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C2;struct Cyc_Absyn_Exp*
_tmp4C4;void*_tmp4C6;void*_tmp4C9;void*_tmp4CA;struct _dyneither_ptr*_tmp4CC;void*
_tmp4CE;void*_tmp4CF;unsigned int _tmp4D1;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*
_tmp4D5;struct _dyneither_ptr*_tmp4D6;int _tmp4D7;int _tmp4D8;struct Cyc_Absyn_Exp*
_tmp4DA;struct _dyneither_ptr*_tmp4DB;int _tmp4DC;int _tmp4DD;struct Cyc_Absyn_Exp*
_tmp4DF;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_List_List*_tmp4E2;struct Cyc_List_List*
_tmp4E4;struct Cyc_Absyn_Vardecl*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Exp*
_tmp4E8;int _tmp4E9;struct _tuple1*_tmp4EB;struct Cyc_List_List*_tmp4EC;struct Cyc_List_List*
_tmp4ED;struct Cyc_Absyn_Aggrdecl*_tmp4EE;void*_tmp4F0;struct Cyc_List_List*
_tmp4F1;struct Cyc_List_List*_tmp4F3;struct Cyc_Absyn_Datatypedecl*_tmp4F4;struct
Cyc_Absyn_Datatypefield*_tmp4F5;struct Cyc_Absyn_MallocInfo _tmp4F9;int _tmp4FA;
struct Cyc_Absyn_Exp*_tmp4FB;void**_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;int _tmp4FE;
struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*
_tmp503;struct _dyneither_ptr*_tmp504;struct Cyc_Absyn_Stmt*_tmp506;_LL209: {
struct Cyc_Absyn_Const_e_struct*_tmp482=(struct Cyc_Absyn_Const_e_struct*)_tmp481;
if(_tmp482->tag != 0)goto _LL20B;else{_tmp483=_tmp482->f1;if((_tmp483.Null_c).tag
!= 1)goto _LL20B;_tmp484=(int)(_tmp483.Null_c).val;}}_LL20A: {struct Cyc_Absyn_Exp*
_tmp50A=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp50A,
_tmp50A))->r;else{struct Cyc_Absyn_Exp*_tmpBA1[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBA1[2]=_tmp50A,((_tmpBA1[1]=_tmp50A,((_tmpBA1[0]=_tmp50A,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA1,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL208;}_LL20B: {struct Cyc_Absyn_Const_e_struct*
_tmp485=(struct Cyc_Absyn_Const_e_struct*)_tmp481;if(_tmp485->tag != 0)goto _LL20D;}
_LL20C: goto _LL208;_LL20D: {struct Cyc_Absyn_Var_e_struct*_tmp486=(struct Cyc_Absyn_Var_e_struct*)
_tmp481;if(_tmp486->tag != 1)goto _LL20F;else{_tmp487=_tmp486->f1;_tmp488=(void*)
_tmp486->f2;}}_LL20E:{struct _handler_cons _tmp50C;_push_handler(& _tmp50C);{int
_tmp50E=0;if(setjmp(_tmp50C.handler))_tmp50E=1;if(!_tmp50E){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp487))->r;;_pop_handler();}else{void*_tmp50D=(void*)_exn_thrown;void*
_tmp510=_tmp50D;_LL262: {struct Cyc_Dict_Absent_struct*_tmp511=(struct Cyc_Dict_Absent_struct*)
_tmp510;if(_tmp511->tag != Cyc_Dict_Absent)goto _LL264;}_LL263: {const char*_tmpBA5;
void*_tmpBA4[1];struct Cyc_String_pa_struct _tmpBA3;(_tmpBA3.tag=0,((_tmpBA3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp487)),((
_tmpBA4[0]=& _tmpBA3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBA5="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBA5,sizeof(char),
32))),_tag_dyneither(_tmpBA4,sizeof(void*),1)))))));}_LL264:;_LL265:(void)_throw(
_tmp510);_LL261:;}}}goto _LL208;_LL20F: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp489=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp481;if(_tmp489->tag != 2)goto
_LL211;else{_tmp48A=_tmp489->f1;}}_LL210: {const char*_tmpBA8;void*_tmpBA7;(
_tmpBA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBA8="unknownid",_tag_dyneither(_tmpBA8,sizeof(char),10))),_tag_dyneither(
_tmpBA7,sizeof(void*),0)));}_LL211: {struct Cyc_Absyn_Primop_e_struct*_tmp48B=(
struct Cyc_Absyn_Primop_e_struct*)_tmp481;if(_tmp48B->tag != 3)goto _LL213;else{
_tmp48C=_tmp48B->f1;_tmp48D=_tmp48B->f2;}}_LL212: {struct Cyc_List_List*_tmp517=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp48D);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp48D);switch(_tmp48C){case Cyc_Absyn_Numelts: _LL266: {
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp48D))->hd;{void*_tmp518=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo _tmp51A;struct Cyc_Absyn_Exp*
_tmp51B;struct Cyc_Absyn_PtrInfo _tmp51D;void*_tmp51E;struct Cyc_Absyn_PtrAtts
_tmp51F;union Cyc_Absyn_Constraint*_tmp520;union Cyc_Absyn_Constraint*_tmp521;
union Cyc_Absyn_Constraint*_tmp522;_LL269: {struct Cyc_Absyn_ArrayType_struct*
_tmp519=(struct Cyc_Absyn_ArrayType_struct*)_tmp518;if(_tmp519->tag != 9)goto
_LL26B;else{_tmp51A=_tmp519->f1;_tmp51B=_tmp51A.num_elts;}}_LL26A: if(!Cyc_Evexp_c_can_eval((
struct Cyc_Absyn_Exp*)_check_null(_tmp51B))){const char*_tmpBAB;void*_tmpBAA;(
_tmpBAA=0,Cyc_Tcutil_terr(e->loc,((_tmpBAB="can't calculate numelts",
_tag_dyneither(_tmpBAB,sizeof(char),24))),_tag_dyneither(_tmpBAA,sizeof(void*),0)));}
e->r=_tmp51B->r;goto _LL268;_LL26B: {struct Cyc_Absyn_PointerType_struct*_tmp51C=(
struct Cyc_Absyn_PointerType_struct*)_tmp518;if(_tmp51C->tag != 5)goto _LL26D;else{
_tmp51D=_tmp51C->f1;_tmp51E=_tmp51D.elt_typ;_tmp51F=_tmp51D.ptr_atts;_tmp520=
_tmp51F.nullable;_tmp521=_tmp51F.bounds;_tmp522=_tmp51F.zero_term;}}_LL26C:{void*
_tmp525=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp521);struct Cyc_Absyn_Exp*_tmp528;_LL270: {struct Cyc_Absyn_DynEither_b_struct*
_tmp526=(struct Cyc_Absyn_DynEither_b_struct*)_tmp525;if(_tmp526->tag != 0)goto
_LL272;}_LL271:{struct Cyc_Absyn_Exp*_tmpBAC[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBAC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp51E),0),((_tmpBAC[0]=(
struct Cyc_Absyn_Exp*)_tmp48D->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBAC,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL26F;_LL272: {struct Cyc_Absyn_Upper_b_struct*_tmp527=(struct Cyc_Absyn_Upper_b_struct*)
_tmp525;if(_tmp527->tag != 1)goto _LL26F;else{_tmp528=_tmp527->f1;}}_LL273: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp522)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp48D->hd);struct Cyc_Absyn_Exp*_tmpBAD[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBAD[1]=_tmp528,((_tmpBAD[0]=(struct Cyc_Absyn_Exp*)_tmp48D->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBAD,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp520)){if(!Cyc_Evexp_c_can_eval(_tmp528)){const
char*_tmpBB0;void*_tmpBAF;(_tmpBAF=0,Cyc_Tcutil_terr(e->loc,((_tmpBB0="can't calculate numelts",
_tag_dyneither(_tmpBB0,sizeof(char),24))),_tag_dyneither(_tmpBAF,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp528,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp528->r;goto _LL26F;}}goto _LL26F;_LL26F:;}goto _LL268;_LL26D:;_LL26E: {const
char*_tmpBB5;void*_tmpBB4[2];struct Cyc_String_pa_struct _tmpBB3;struct Cyc_String_pa_struct
_tmpBB2;(_tmpBB2.tag=0,((_tmpBB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBB3.tag=0,((_tmpBB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBB4[0]=& _tmpBB3,((
_tmpBB4[1]=& _tmpBB2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB5="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBB5,
sizeof(char),41))),_tag_dyneither(_tmpBB4,sizeof(void*),2)))))))))))));}_LL268:;}
break;}case Cyc_Absyn_Plus: _LL267:{void*_tmp531=Cyc_Tcutil_compress((void*)((
struct Cyc_List_List*)_check_null(_tmp517))->hd);struct Cyc_Absyn_PtrInfo _tmp533;
void*_tmp534;struct Cyc_Absyn_PtrAtts _tmp535;union Cyc_Absyn_Constraint*_tmp536;
union Cyc_Absyn_Constraint*_tmp537;_LL276: {struct Cyc_Absyn_PointerType_struct*
_tmp532=(struct Cyc_Absyn_PointerType_struct*)_tmp531;if(_tmp532->tag != 5)goto
_LL278;else{_tmp533=_tmp532->f1;_tmp534=_tmp533.elt_typ;_tmp535=_tmp533.ptr_atts;
_tmp536=_tmp535.bounds;_tmp537=_tmp535.zero_term;}}_LL277:{void*_tmp538=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp536);struct Cyc_Absyn_Exp*_tmp53B;_LL27B: {struct Cyc_Absyn_DynEither_b_struct*
_tmp539=(struct Cyc_Absyn_DynEither_b_struct*)_tmp538;if(_tmp539->tag != 0)goto
_LL27D;}_LL27C: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48D))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp48D->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBB6[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBB6[2]=e2,((_tmpBB6[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp534),0),((_tmpBB6[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB6,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL27A;}_LL27D: {struct Cyc_Absyn_Upper_b_struct*_tmp53A=(struct
Cyc_Absyn_Upper_b_struct*)_tmp538;if(_tmp53A->tag != 1)goto _LL27A;else{_tmp53B=
_tmp53A->f1;}}_LL27E: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp537)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48D))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp48D->tl))->hd;struct Cyc_Absyn_Exp*_tmpBB7[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),((
_tmpBB7[2]=e2,((_tmpBB7[1]=_tmp53B,((_tmpBB7[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB7,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL27A;_LL27A:;}goto _LL275;_LL278:;_LL279: goto _LL275;
_LL275:;}break;case Cyc_Absyn_Minus: _LL274: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp517))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48D))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48D->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp517->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBB8[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBB8[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBB8[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBB8[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBB8,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
_LL27F: goto _LL280;case Cyc_Absyn_Neq: _LL280: goto _LL281;case Cyc_Absyn_Gt: _LL281:
goto _LL282;case Cyc_Absyn_Gte: _LL282: goto _LL283;case Cyc_Absyn_Lt: _LL283: goto
_LL284;case Cyc_Absyn_Lte: _LL284: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48D))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48D->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp517))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp517->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);if(Cyc_Tcutil_is_tagged_pointer_typ(
t2))e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
break;}default: _LL285: break;}goto _LL208;}_LL213: {struct Cyc_Absyn_Increment_e_struct*
_tmp48E=(struct Cyc_Absyn_Increment_e_struct*)_tmp481;if(_tmp48E->tag != 5)goto
_LL215;else{_tmp48F=_tmp48E->f1;_tmp490=_tmp48E->f2;}}_LL214: {void*e2_cyc_typ=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp48F->topt))->v;void*ptr_type=(void*)&
Cyc_Absyn_VoidType_val;void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int
is_dyneither=0;const char*_tmpBB9;struct _dyneither_ptr incr_str=(_tmpBB9="increment",
_tag_dyneither(_tmpBB9,sizeof(char),10));if(_tmp490 == Cyc_Absyn_PreDec  || 
_tmp490 == Cyc_Absyn_PostDec){const char*_tmpBBA;incr_str=((_tmpBBA="decrement",
_tag_dyneither(_tmpBBA,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp48F,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBBE;void*_tmpBBD[1];
struct Cyc_String_pa_struct _tmpBBC;(_tmpBBC.tag=0,((_tmpBBC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBBD[0]=& _tmpBBC,Cyc_Tcutil_terr(
e->loc,((_tmpBBE="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBBE,sizeof(char),74))),_tag_dyneither(_tmpBBD,sizeof(void*),1)))))));}{
const char*_tmpBC1;void*_tmpBC0;(_tmpBC0=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC1="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBC1,sizeof(char),30))),_tag_dyneither(_tmpBC0,sizeof(void*),0)));}}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp48F,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp545=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBC2;_tmp545->topt=((_tmpBC2=_cycalloc(sizeof(*
_tmpBC2)),((_tmpBC2->v=Cyc_Absyn_sint_typ,_tmpBC2))));}switch(_tmp490){case Cyc_Absyn_PreInc:
_LL287:{struct Cyc_Absyn_AssignOp_e_struct _tmpBC8;struct Cyc_Core_Opt*_tmpBC7;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBC6;e->r=(void*)((_tmpBC6=_cycalloc(
sizeof(*_tmpBC6)),((_tmpBC6[0]=((_tmpBC8.tag=4,((_tmpBC8.f1=_tmp48F,((_tmpBC8.f2=((
_tmpBC7=_cycalloc_atomic(sizeof(*_tmpBC7)),((_tmpBC7->v=(void*)Cyc_Absyn_Plus,
_tmpBC7)))),((_tmpBC8.f3=_tmp545,_tmpBC8)))))))),_tmpBC6))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL288:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBCE;struct Cyc_Core_Opt*_tmpBCD;struct Cyc_Absyn_AssignOp_e_struct*_tmpBCC;e->r=(
void*)((_tmpBCC=_cycalloc(sizeof(*_tmpBCC)),((_tmpBCC[0]=((_tmpBCE.tag=4,((
_tmpBCE.f1=_tmp48F,((_tmpBCE.f2=((_tmpBCD=_cycalloc_atomic(sizeof(*_tmpBCD)),((
_tmpBCD->v=(void*)Cyc_Absyn_Minus,_tmpBCD)))),((_tmpBCE.f3=_tmp545,_tmpBCE)))))))),
_tmpBCC))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL289:{const char*_tmpBD2;
void*_tmpBD1[1];struct Cyc_String_pa_struct _tmpBD0;(_tmpBD0.tag=0,((_tmpBD0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD1[0]=& _tmpBD0,Cyc_Tcutil_terr(
e->loc,((_tmpBD2="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBD2,sizeof(char),59))),_tag_dyneither(_tmpBD1,sizeof(void*),1)))))));}{
const char*_tmpBD5;void*_tmpBD4;(_tmpBD4=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD5="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBD5,sizeof(char),34))),_tag_dyneither(_tmpBD4,sizeof(void*),0)));}}}
Cyc_Toc_exp_to_c(nv,_tmp48F);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp490 == Cyc_Absyn_PostInc
 || _tmp490 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp490 == Cyc_Absyn_PreDec  || _tmp490 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBD6[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBD6[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBD6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBD6[0]=Cyc_Toc_push_address_exp(_tmp48F),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD6,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;{void*_tmp553=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp48F->topt))->v);enum Cyc_Absyn_Size_of _tmp555;int _tmp558;_LL28C: {
struct Cyc_Absyn_IntType_struct*_tmp554=(struct Cyc_Absyn_IntType_struct*)_tmp553;
if(_tmp554->tag != 6)goto _LL28E;else{_tmp555=_tmp554->f2;}}_LL28D: switch(_tmp555){
case Cyc_Absyn_Char_sz: _LL296: fn_e=_tmp490 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_char_e:
Cyc_Toc__zero_arr_inplace_plus_char_e;break;case Cyc_Absyn_Short_sz: _LL297: fn_e=
_tmp490 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
break;case Cyc_Absyn_Int_sz: _LL298: fn_e=_tmp490 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_int_e:
Cyc_Toc__zero_arr_inplace_plus_int_e;break;default: _LL299: {struct Cyc_Core_Impossible_struct
_tmpBDC;const char*_tmpBDB;struct Cyc_Core_Impossible_struct*_tmpBDA;(int)_throw((
void*)((_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((_tmpBDA[0]=((_tmpBDC.tag=Cyc_Core_Impossible,((
_tmpBDC.f1=((_tmpBDB="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpBDB,sizeof(char),44))),_tmpBDC)))),_tmpBDA)))));}}goto _LL28B;
_LL28E: {struct Cyc_Absyn_FloatType_struct*_tmp556=(struct Cyc_Absyn_FloatType_struct*)
_tmp553;if(_tmp556->tag != 7)goto _LL290;}_LL28F: fn_e=_tmp490 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_float_e: Cyc_Toc__zero_arr_inplace_plus_float_e;
goto _LL28B;_LL290: {struct Cyc_Absyn_DoubleType_struct*_tmp557=(struct Cyc_Absyn_DoubleType_struct*)
_tmp553;if(_tmp557->tag != 8)goto _LL292;else{_tmp558=_tmp557->f1;}}_LL291: switch(
_tmp558){case 1: _LL29B: fn_e=_tmp490 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL29C: fn_e=_tmp490 == 
Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL28B;_LL292: {struct Cyc_Absyn_PointerType_struct*_tmp559=(struct Cyc_Absyn_PointerType_struct*)
_tmp553;if(_tmp559->tag != 5)goto _LL294;}_LL293: fn_e=_tmp490 == Cyc_Absyn_PostInc?
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL28B;_LL294:;_LL295: {struct Cyc_Core_Impossible_struct _tmpBE2;const char*
_tmpBE1;struct Cyc_Core_Impossible_struct*_tmpBE0;(int)_throw((void*)((_tmpBE0=
_cycalloc(sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE2.tag=Cyc_Core_Impossible,((
_tmpBE2.f1=((_tmpBE1="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpBE1,sizeof(char),64))),_tmpBE2)))),_tmpBE0)))));}_LL28B:;}{
struct Cyc_Absyn_Exp*_tmpBE3[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBE3[1]=Cyc_Absyn_signed_int_exp(
1,0),((_tmpBE3[0]=_tmp48F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE3,sizeof(struct Cyc_Absyn_Exp*),2)))))));}}else{if(elt_typ == (
void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp48F)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp48F,0,Cyc_Toc_incr_lvalue,_tmp490);e->r=_tmp48F->r;}}}goto _LL208;}}}_LL215: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp491=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp481;if(_tmp491->tag != 4)goto _LL217;else{_tmp492=_tmp491->f1;_tmp493=_tmp491->f2;
_tmp494=_tmp491->f3;}}_LL216: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp492->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp494->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)&
Cyc_Absyn_VoidType_val;if(Cyc_Toc_is_tagged_union_project(_tmp492,& f_tag,&
tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp492);Cyc_Toc_exp_to_c(nv,
_tmp494);e->r=Cyc_Toc_tagged_union_assignop(_tmp492,e1_old_typ,_tmp493,_tmp494,
e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp492,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp492,_tmp493,_tmp494,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp492);Cyc_Toc_exp_to_c(nv,_tmp492);Cyc_Toc_exp_to_c(nv,_tmp494);{int done=0;
if(_tmp493 != 0){void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;switch((enum Cyc_Absyn_Primop)
_tmp493->v){case Cyc_Absyn_Plus: _LL29E: change=_tmp494;break;case Cyc_Absyn_Minus:
_LL29F: change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp494,0);break;default:
_LL2A0: {const char*_tmpBE6;void*_tmpBE5;(_tmpBE5=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE6="bad t ? pointer arithmetic",
_tag_dyneither(_tmpBE6,sizeof(char),27))),_tag_dyneither(_tmpBE5,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp564=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpBE7[3];e->r=Cyc_Toc_fncall_exp_r(_tmp564,((_tmpBE7[2]=change,((
_tmpBE7[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBE7[0]=Cyc_Toc_push_address_exp(
_tmp492),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE7,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp493->v){case Cyc_Absyn_Plus:
_LL2A2: done=1;{struct Cyc_Absyn_Exp*_tmpBE8[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp492),((_tmpBE8[1]=_tmp494,((
_tmpBE8[0]=_tmp492,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE8,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2A3: {const char*_tmpBEB;void*_tmpBEA;(_tmpBEA=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEB="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpBEB,sizeof(char),39))),_tag_dyneither(_tmpBEA,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp494->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp494->r,0));if(!Cyc_Absyn_is_lvalue(_tmp492)){{struct _tuple18 _tmpBEE;struct
_tuple18*_tmpBED;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp492,0,Cyc_Toc_assignop_lvalue,((_tmpBED=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpBED[0]=((_tmpBEE.f1=_tmp493,((_tmpBEE.f2=
_tmp494,_tmpBEE)))),_tmpBED)))));}e->r=_tmp492->r;}}goto _LL208;}}}}_LL217: {
struct Cyc_Absyn_Conditional_e_struct*_tmp495=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp481;if(_tmp495->tag != 6)goto _LL219;else{_tmp496=_tmp495->f1;_tmp497=_tmp495->f2;
_tmp498=_tmp495->f3;}}_LL218: Cyc_Toc_exp_to_c(nv,_tmp496);Cyc_Toc_exp_to_c(nv,
_tmp497);Cyc_Toc_exp_to_c(nv,_tmp498);goto _LL208;_LL219: {struct Cyc_Absyn_And_e_struct*
_tmp499=(struct Cyc_Absyn_And_e_struct*)_tmp481;if(_tmp499->tag != 7)goto _LL21B;
else{_tmp49A=_tmp499->f1;_tmp49B=_tmp499->f2;}}_LL21A: Cyc_Toc_exp_to_c(nv,
_tmp49A);Cyc_Toc_exp_to_c(nv,_tmp49B);goto _LL208;_LL21B: {struct Cyc_Absyn_Or_e_struct*
_tmp49C=(struct Cyc_Absyn_Or_e_struct*)_tmp481;if(_tmp49C->tag != 8)goto _LL21D;
else{_tmp49D=_tmp49C->f1;_tmp49E=_tmp49C->f2;}}_LL21C: Cyc_Toc_exp_to_c(nv,
_tmp49D);Cyc_Toc_exp_to_c(nv,_tmp49E);goto _LL208;_LL21D: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp49F=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp481;if(_tmp49F->tag != 9)goto _LL21F;
else{_tmp4A0=_tmp49F->f1;_tmp4A1=_tmp49F->f2;}}_LL21E: Cyc_Toc_exp_to_c(nv,
_tmp4A0);Cyc_Toc_exp_to_c(nv,_tmp4A1);goto _LL208;_LL21F: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp4A2=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp481;if(_tmp4A2->tag != 10)goto
_LL221;else{_tmp4A3=_tmp4A2->f1;_tmp4A4=_tmp4A2->f2;}}_LL220: _tmp4A6=_tmp4A3;
_tmp4A7=_tmp4A4;goto _LL222;_LL221: {struct Cyc_Absyn_FnCall_e_struct*_tmp4A5=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp481;if(_tmp4A5->tag != 11)goto _LL223;else{
_tmp4A6=_tmp4A5->f1;_tmp4A7=_tmp4A5->f2;_tmp4A8=_tmp4A5->f3;if(_tmp4A8 != 0)goto
_LL223;}}_LL222: Cyc_Toc_exp_to_c(nv,_tmp4A6);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp4A7);goto _LL208;_LL223: {struct Cyc_Absyn_FnCall_e_struct*
_tmp4A9=(struct Cyc_Absyn_FnCall_e_struct*)_tmp481;if(_tmp4A9->tag != 11)goto
_LL225;else{_tmp4AA=_tmp4A9->f1;_tmp4AB=_tmp4A9->f2;_tmp4AC=_tmp4A9->f3;if(
_tmp4AC == 0)goto _LL225;_tmp4AD=*_tmp4AC;_tmp4AE=_tmp4AD.num_varargs;_tmp4AF=
_tmp4AD.injectors;_tmp4B0=_tmp4AD.vai;}}_LL224: {struct Cyc_Toc_Env _tmp56C;struct
_RegionHandle*_tmp56D;struct Cyc_Toc_Env*_tmp56B=nv;_tmp56C=*_tmp56B;_tmp56D=
_tmp56C.rgn;{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*argvexp=
Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp4AE,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp4B0->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp4AB);int num_normargs=num_args - _tmp4AE;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4AB=_tmp4AB->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AB))->hd);{
struct Cyc_List_List*_tmpBEF;new_args=((_tmpBEF=_cycalloc(sizeof(*_tmpBEF)),((
_tmpBEF->hd=(struct Cyc_Absyn_Exp*)_tmp4AB->hd,((_tmpBEF->tl=new_args,_tmpBEF))))));}}}{
struct Cyc_Absyn_Exp*_tmpBF2[3];struct Cyc_List_List*_tmpBF1;new_args=((_tmpBF1=
_cycalloc(sizeof(*_tmpBF1)),((_tmpBF1->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpBF2[2]=num_varargs_exp,((_tmpBF2[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpBF2[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF2,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBF1->tl=
new_args,_tmpBF1))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4AA);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4AA,new_args,0),0);if(_tmp4B0->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp571=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4B0->type));struct Cyc_Absyn_DatatypeInfo _tmp573;union Cyc_Absyn_DatatypeInfoU
_tmp574;struct Cyc_Absyn_Datatypedecl**_tmp575;struct Cyc_Absyn_Datatypedecl*
_tmp576;_LL2A6: {struct Cyc_Absyn_DatatypeType_struct*_tmp572=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp571;if(_tmp572->tag != 3)goto _LL2A8;else{_tmp573=_tmp572->f1;_tmp574=_tmp573.datatype_info;
if((_tmp574.KnownDatatype).tag != 2)goto _LL2A8;_tmp575=(struct Cyc_Absyn_Datatypedecl**)(
_tmp574.KnownDatatype).val;_tmp576=*_tmp575;}}_LL2A7: tud=_tmp576;goto _LL2A5;
_LL2A8:;_LL2A9: {const char*_tmpBF5;void*_tmpBF4;(_tmpBF4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF5="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpBF5,sizeof(char),44))),_tag_dyneither(_tmpBF4,sizeof(void*),0)));}
_LL2A5:;}{unsigned int _tmpBFA;unsigned int _tmpBF9;struct _dyneither_ptr _tmpBF8;
struct _tuple1**_tmpBF7;unsigned int _tmpBF6;struct _dyneither_ptr vs=(_tmpBF6=(
unsigned int)_tmp4AE,((_tmpBF7=(struct _tuple1**)_region_malloc(_tmp56D,
_check_times(sizeof(struct _tuple1*),_tmpBF6)),((_tmpBF8=_tag_dyneither(_tmpBF7,
sizeof(struct _tuple1*),_tmpBF6),((((_tmpBF9=_tmpBF6,_tmpBFB(& _tmpBFA,& _tmpBF9,&
_tmpBF7))),_tmpBF8)))))));if(_tmp4AE != 0){struct Cyc_List_List*_tmp579=0;{int i=
_tmp4AE - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpBFC;_tmp579=((_tmpBFC=
_cycalloc(sizeof(*_tmpBFC)),((_tmpBFC->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct
_tuple1*),i)),0),0)),((_tmpBFC->tl=_tmp579,_tmpBFC))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp579,0),s,0);{
int i=0;for(0;_tmp4AB != 0;(((_tmp4AB=_tmp4AB->tl,_tmp4AF=_tmp4AF->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4AB->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple1*var=*((struct
_tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp57C;struct
_tuple1*_tmp57D;struct Cyc_List_List*_tmp57E;struct Cyc_Absyn_Datatypefield*
_tmp57B=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp4AF))->hd;_tmp57C=*_tmp57B;_tmp57D=_tmp57C.name;_tmp57E=_tmp57C.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp57E))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp57D),0),s,0);{const char*_tmpBFD;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp57D,((_tmpBFD="_struct",_tag_dyneither(_tmpBFD,
sizeof(char),8))))),0,s,0);}}}}}else{struct _tuple16*_tmpBFE[3];struct Cyc_List_List*
_tmp580=(_tmpBFE[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpBFE[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpBFE[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBFE,sizeof(struct _tuple16*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}}}else{{int i=0;for(0;_tmp4AB
!= 0;(_tmp4AB=_tmp4AB->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4AB->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4AB->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);}}goto _LL208;}_LL225: {struct Cyc_Absyn_Throw_e_struct*_tmp4B1=(struct Cyc_Absyn_Throw_e_struct*)
_tmp481;if(_tmp4B1->tag != 12)goto _LL227;else{_tmp4B2=_tmp4B1->f1;}}_LL226: Cyc_Toc_exp_to_c(
nv,_tmp4B2);e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(
_tmp4B2),0))->r;goto _LL208;_LL227: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp4B3=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp481;if(_tmp4B3->tag != 13)
goto _LL229;else{_tmp4B4=_tmp4B3->f1;}}_LL228: Cyc_Toc_exp_to_c(nv,_tmp4B4);goto
_LL208;_LL229: {struct Cyc_Absyn_Instantiate_e_struct*_tmp4B5=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp481;if(_tmp4B5->tag != 14)goto _LL22B;else{_tmp4B6=_tmp4B5->f1;_tmp4B7=_tmp4B5->f2;}}
_LL22A: Cyc_Toc_exp_to_c(nv,_tmp4B6);for(0;_tmp4B7 != 0;_tmp4B7=_tmp4B7->tl){enum 
Cyc_Absyn_Kind k=Cyc_Tcutil_typ_kind((void*)_tmp4B7->hd);if(((k != Cyc_Absyn_EffKind
 && k != Cyc_Absyn_RgnKind) && k != Cyc_Absyn_UniqueRgnKind) && k != Cyc_Absyn_TopRgnKind){{
void*_tmp586=Cyc_Tcutil_compress((void*)_tmp4B7->hd);_LL2AB: {struct Cyc_Absyn_VarType_struct*
_tmp587=(struct Cyc_Absyn_VarType_struct*)_tmp586;if(_tmp587->tag != 2)goto _LL2AD;}
_LL2AC: goto _LL2AE;_LL2AD: {struct Cyc_Absyn_DatatypeType_struct*_tmp588=(struct
Cyc_Absyn_DatatypeType_struct*)_tmp586;if(_tmp588->tag != 3)goto _LL2AF;}_LL2AE:
continue;_LL2AF:;_LL2B0: e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4B6,0))->r;goto _LL2AA;_LL2AA:;}
break;}}goto _LL208;_LL22B: {struct Cyc_Absyn_Cast_e_struct*_tmp4B8=(struct Cyc_Absyn_Cast_e_struct*)
_tmp481;if(_tmp4B8->tag != 15)goto _LL22D;else{_tmp4B9=(void**)& _tmp4B8->f1;
_tmp4BA=(void**)((void**)& _tmp4B8->f1);_tmp4BB=_tmp4B8->f2;_tmp4BC=_tmp4B8->f3;
_tmp4BD=_tmp4B8->f4;}}_LL22C: {void*old_t2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4BB->topt))->v;void*new_typ=*_tmp4BA;*_tmp4BA=Cyc_Toc_typ_to_c(
new_typ);Cyc_Toc_exp_to_c(nv,_tmp4BB);{struct _tuple0 _tmpBFF;struct _tuple0 _tmp58A=(
_tmpBFF.f1=Cyc_Tcutil_compress(old_t2),((_tmpBFF.f2=Cyc_Tcutil_compress(new_typ),
_tmpBFF)));void*_tmp58B;struct Cyc_Absyn_PtrInfo _tmp58D;void*_tmp58E;struct Cyc_Absyn_PtrInfo
_tmp590;void*_tmp591;struct Cyc_Absyn_PtrInfo _tmp593;void*_tmp594;_LL2B2: _tmp58B=
_tmp58A.f1;{struct Cyc_Absyn_PointerType_struct*_tmp58C=(struct Cyc_Absyn_PointerType_struct*)
_tmp58B;if(_tmp58C->tag != 5)goto _LL2B4;else{_tmp58D=_tmp58C->f1;}}_tmp58E=
_tmp58A.f2;{struct Cyc_Absyn_PointerType_struct*_tmp58F=(struct Cyc_Absyn_PointerType_struct*)
_tmp58E;if(_tmp58F->tag != 5)goto _LL2B4;else{_tmp590=_tmp58F->f1;}}_LL2B3: {int
_tmp596=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp58D.ptr_atts).nullable);int _tmp597=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,(_tmp590.ptr_atts).nullable);void*_tmp598=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(
_tmp58D.ptr_atts).bounds);void*_tmp599=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp590.ptr_atts).bounds);int
_tmp59A=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp58D.ptr_atts).zero_term);int _tmp59B=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp590.ptr_atts).zero_term);{struct _tuple0 _tmpC00;
struct _tuple0 _tmp59D=(_tmpC00.f1=_tmp598,((_tmpC00.f2=_tmp599,_tmpC00)));void*
_tmp59E;struct Cyc_Absyn_Exp*_tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A3;
void*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A6;void*_tmp5A7;void*_tmp5A9;void*_tmp5AB;
struct Cyc_Absyn_Exp*_tmp5AD;void*_tmp5AE;void*_tmp5B0;_LL2B9: _tmp59E=_tmp59D.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp59F=(struct Cyc_Absyn_Upper_b_struct*)_tmp59E;
if(_tmp59F->tag != 1)goto _LL2BB;else{_tmp5A0=_tmp59F->f1;}}_tmp5A1=_tmp59D.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5A2=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A1;
if(_tmp5A2->tag != 1)goto _LL2BB;else{_tmp5A3=_tmp5A2->f1;}}_LL2BA: if((!Cyc_Evexp_c_can_eval(
_tmp5A0) || !Cyc_Evexp_c_can_eval(_tmp5A3)) && !Cyc_Evexp_same_const_exp(_tmp5A0,
_tmp5A3)){const char*_tmpC03;void*_tmpC02;(_tmpC02=0,Cyc_Tcutil_terr(e->loc,((
_tmpC03="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC03,sizeof(char),54))),_tag_dyneither(_tmpC02,sizeof(void*),0)));}if(_tmp596
 && !_tmp597){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC06;void*_tmpC05;(
_tmpC05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC06="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC06,
sizeof(char),44))),_tag_dyneither(_tmpC05,sizeof(void*),0)));}if(_tmp4BD != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC0A;void*_tmpC09[1];struct Cyc_String_pa_struct _tmpC08;(_tmpC08.tag=
0,((_tmpC08.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC09[0]=& _tmpC08,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC0A="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC0A,sizeof(char),41))),_tag_dyneither(_tmpC09,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4BB);if(do_null_check){if(!_tmp4BC){
const char*_tmpC0D;void*_tmpC0C;(_tmpC0C=0,Cyc_Tcutil_warn(e->loc,((_tmpC0D="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC0D,sizeof(char),58))),_tag_dyneither(_tmpC0C,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC0E;e->r=Cyc_Toc_cast_it_r(*_tmp4BA,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC0E=_cycalloc(sizeof(*_tmpC0E)),((_tmpC0E->hd=
_tmp4BB,((_tmpC0E->tl=0,_tmpC0E)))))),0));}}}}goto _LL2B8;_LL2BB: _tmp5A4=_tmp59D.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp5A5=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A4;
if(_tmp5A5->tag != 1)goto _LL2BD;else{_tmp5A6=_tmp5A5->f1;}}_tmp5A7=_tmp59D.f2;{
struct Cyc_Absyn_DynEither_b_struct*_tmp5A8=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5A7;if(_tmp5A8->tag != 0)goto _LL2BD;}_LL2BC: if(!Cyc_Evexp_c_can_eval(_tmp5A6)){
const char*_tmpC11;void*_tmpC10;(_tmpC10=0,Cyc_Tcutil_terr(e->loc,((_tmpC11="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC11,sizeof(char),71))),_tag_dyneither(_tmpC10,sizeof(void*),0)));}
if(_tmp4BD == Cyc_Absyn_NonNull_to_Null){const char*_tmpC15;void*_tmpC14[1];struct
Cyc_String_pa_struct _tmpC13;(_tmpC13.tag=0,((_tmpC13.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC14[0]=& _tmpC13,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC15="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC15,sizeof(char),44))),_tag_dyneither(_tmpC14,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){if((_tmp59A  && !(_tmp590.elt_tq).real_const) && !
_tmp59B)_tmp5A6=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5A6,Cyc_Absyn_uint_exp(1,
0),0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5A6,_tmp4BB))->r;}else{
struct Cyc_Absyn_Exp*_tmp5C1=Cyc_Toc__tag_dyneither_e;if(_tmp59A){struct _tuple1*
_tmp5C2=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5C3=Cyc_Absyn_var_exp(_tmp5C2,
0);struct Cyc_Absyn_Exp*arg3;{void*_tmp5C4=_tmp4BB->r;union Cyc_Absyn_Cnst _tmp5C6;
struct _dyneither_ptr _tmp5C7;_LL2C2: {struct Cyc_Absyn_Const_e_struct*_tmp5C5=(
struct Cyc_Absyn_Const_e_struct*)_tmp5C4;if(_tmp5C5->tag != 0)goto _LL2C4;else{
_tmp5C6=_tmp5C5->f1;if((_tmp5C6.String_c).tag != 7)goto _LL2C4;_tmp5C7=(struct
_dyneither_ptr)(_tmp5C6.String_c).val;}}_LL2C3: arg3=_tmp5A6;goto _LL2C1;_LL2C4:;
_LL2C5:{struct Cyc_Absyn_Exp*_tmpC16[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4BB),((_tmpC16[1]=_tmp5A6,((_tmpC16[0]=
_tmp5C3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC16,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}goto _LL2C1;
_LL2C1:;}if(!_tmp59B  && !(_tmp590.elt_tq).real_const)arg3=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5C9=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp590.elt_typ),0);struct Cyc_Absyn_Exp*_tmpC17[3];struct Cyc_Absyn_Exp*
_tmp5CA=Cyc_Absyn_fncall_exp(_tmp5C1,((_tmpC17[2]=arg3,((_tmpC17[1]=_tmp5C9,((
_tmpC17[0]=_tmp5C3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC17,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);struct Cyc_Absyn_Stmt*
_tmp5CB=Cyc_Absyn_exp_stmt(_tmp5CA,0);_tmp5CB=Cyc_Absyn_declare_stmt(_tmp5C2,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp4BB,_tmp5CB,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5CB);}}
else{struct Cyc_Absyn_Exp*_tmpC18[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5C1,((_tmpC18[
2]=_tmp5A6,((_tmpC18[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp590.elt_typ),
0),((_tmpC18[0]=_tmp4BB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC18,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2B8;
_LL2BD: _tmp5A9=_tmp59D.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5AA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5A9;if(_tmp5AA->tag != 0)goto _LL2BF;}_tmp5AB=_tmp59D.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5AC=(struct Cyc_Absyn_Upper_b_struct*)_tmp5AB;if(_tmp5AC->tag != 1)goto _LL2BF;
else{_tmp5AD=_tmp5AC->f1;}}_LL2BE: if(!Cyc_Evexp_c_can_eval(_tmp5AD)){const char*
_tmpC1B;void*_tmpC1A;(_tmpC1A=0,Cyc_Tcutil_terr(e->loc,((_tmpC1B="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC1B,sizeof(char),71))),_tag_dyneither(_tmpC1A,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC1E;void*_tmpC1D;(_tmpC1D=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC1E="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC1E,sizeof(char),45))),_tag_dyneither(_tmpC1D,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5D2=_tmp5AD;if(_tmp59A  && !_tmp59B)_tmp5D2=Cyc_Absyn_add_exp(
_tmp5AD,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5D3=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC1F[3];struct Cyc_Absyn_Exp*_tmp5D4=Cyc_Absyn_fncall_exp(
_tmp5D3,((_tmpC1F[2]=_tmp5D2,((_tmpC1F[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp58D.elt_typ),0),((_tmpC1F[0]=_tmp4BB,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1F,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp597){struct Cyc_List_List*_tmpC20;_tmp5D4->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC20=_cycalloc(sizeof(*_tmpC20)),((_tmpC20->hd=Cyc_Absyn_copy_exp(
_tmp5D4),((_tmpC20->tl=0,_tmpC20)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4BA,_tmp5D4);
goto _LL2B8;}}_LL2BF: _tmp5AE=_tmp59D.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5AF=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5AE;if(_tmp5AF->tag != 0)goto
_LL2B8;}_tmp5B0=_tmp59D.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5B1=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5B0;if(_tmp5B1->tag != 0)goto _LL2B8;}_LL2C0:
DynCast: if((_tmp59A  && !_tmp59B) && !(_tmp590.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC23;void*_tmpC22;(_tmpC22=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC23="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC23,sizeof(char),70))),_tag_dyneither(_tmpC22,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5D9=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC24[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5D9,((_tmpC24[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC24[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58D.elt_typ),0),((
_tmpC24[0]=_tmp4BB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC24,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2B8;
_LL2B8:;}goto _LL2B1;}_LL2B4: _tmp591=_tmp58A.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp592=(struct Cyc_Absyn_PointerType_struct*)_tmp591;if(_tmp592->tag != 5)goto
_LL2B6;else{_tmp593=_tmp592->f1;}}_tmp594=_tmp58A.f2;{struct Cyc_Absyn_IntType_struct*
_tmp595=(struct Cyc_Absyn_IntType_struct*)_tmp594;if(_tmp595->tag != 6)goto _LL2B6;}
_LL2B5:{void*_tmp5DB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,(_tmp593.ptr_atts).bounds);_LL2C7: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5DC=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5DB;if(_tmp5DC->tag != 0)goto
_LL2C9;}_LL2C8: _tmp4BB->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp4BB->r,
_tmp4BB->loc),Cyc_Toc_curr_sp);goto _LL2C6;_LL2C9:;_LL2CA: goto _LL2C6;_LL2C6:;}
goto _LL2B1;_LL2B6:;_LL2B7: goto _LL2B1;_LL2B1:;}goto _LL208;}_LL22D: {struct Cyc_Absyn_Address_e_struct*
_tmp4BE=(struct Cyc_Absyn_Address_e_struct*)_tmp481;if(_tmp4BE->tag != 16)goto
_LL22F;else{_tmp4BF=_tmp4BE->f1;}}_LL22E:{void*_tmp5DD=_tmp4BF->r;struct _tuple1*
_tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_List_List*_tmp5E1;struct Cyc_List_List*
_tmp5E3;_LL2CC: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5DE=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp5DD;if(_tmp5DE->tag != 30)goto _LL2CE;else{_tmp5DF=_tmp5DE->f1;_tmp5E0=_tmp5DE->f2;
_tmp5E1=_tmp5DE->f3;}}_LL2CD: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC28;void*
_tmpC27[1];struct Cyc_String_pa_struct _tmpC26;(_tmpC26.tag=0,((_tmpC26.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4BF->loc)),((
_tmpC27[0]=& _tmpC26,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC28="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC28,
sizeof(char),42))),_tag_dyneither(_tmpC27,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4BF->topt))->v,_tmp5E0 != 0,1,0,
_tmp5E1,_tmp5DF))->r;goto _LL2CB;_LL2CE: {struct Cyc_Absyn_Tuple_e_struct*_tmp5E2=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5DD;if(_tmp5E2->tag != 26)goto _LL2D0;else{
_tmp5E3=_tmp5E2->f1;}}_LL2CF: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC2C;void*
_tmpC2B[1];struct Cyc_String_pa_struct _tmpC2A;(_tmpC2A.tag=0,((_tmpC2A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4BF->loc)),((
_tmpC2B[0]=& _tmpC2A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC2C="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC2C,
sizeof(char),42))),_tag_dyneither(_tmpC2B,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5E3))->r;goto _LL2CB;_LL2D0:;_LL2D1: Cyc_Toc_exp_to_c(nv,_tmp4BF);if(!
Cyc_Absyn_is_lvalue(_tmp4BF)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4BF,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4BF);}goto _LL2CB;
_LL2CB:;}goto _LL208;_LL22F: {struct Cyc_Absyn_New_e_struct*_tmp4C0=(struct Cyc_Absyn_New_e_struct*)
_tmp481;if(_tmp4C0->tag != 17)goto _LL231;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=_tmp4C0->f2;}}
_LL230: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC30;void*_tmpC2F[1];struct Cyc_String_pa_struct
_tmpC2E;(_tmpC2E.tag=0,((_tmpC2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4C2->loc)),((_tmpC2F[0]=& _tmpC2E,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC30="%s: new at top-level",
_tag_dyneither(_tmpC30,sizeof(char),21))),_tag_dyneither(_tmpC2F,sizeof(void*),1)))))));}{
void*_tmp5ED=_tmp4C2->r;struct Cyc_List_List*_tmp5EF;struct Cyc_Absyn_Vardecl*
_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F3;int _tmp5F4;
struct _tuple1*_tmp5F6;struct Cyc_List_List*_tmp5F7;struct Cyc_List_List*_tmp5F8;
struct Cyc_Absyn_Aggrdecl*_tmp5F9;struct Cyc_List_List*_tmp5FB;_LL2D3: {struct Cyc_Absyn_Array_e_struct*
_tmp5EE=(struct Cyc_Absyn_Array_e_struct*)_tmp5ED;if(_tmp5EE->tag != 28)goto _LL2D5;
else{_tmp5EF=_tmp5EE->f1;}}_LL2D4: {struct _tuple1*_tmp5FC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5FD=Cyc_Absyn_var_exp(_tmp5FC,0);struct Cyc_Absyn_Stmt*
_tmp5FE=Cyc_Toc_init_array(nv,_tmp5FD,_tmp5EF,Cyc_Absyn_exp_stmt(_tmp5FD,0));
void*old_elt_typ;{void*_tmp5FF=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp601;void*_tmp602;struct Cyc_Absyn_Tqual _tmp603;struct Cyc_Absyn_PtrAtts _tmp604;
union Cyc_Absyn_Constraint*_tmp605;_LL2DE: {struct Cyc_Absyn_PointerType_struct*
_tmp600=(struct Cyc_Absyn_PointerType_struct*)_tmp5FF;if(_tmp600->tag != 5)goto
_LL2E0;else{_tmp601=_tmp600->f1;_tmp602=_tmp601.elt_typ;_tmp603=_tmp601.elt_tq;
_tmp604=_tmp601.ptr_atts;_tmp605=_tmp604.zero_term;}}_LL2DF: old_elt_typ=_tmp602;
goto _LL2DD;_LL2E0:;_LL2E1: {const char*_tmpC33;void*_tmpC32;old_elt_typ=((_tmpC32=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC33="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC33,sizeof(char),52))),_tag_dyneither(_tmpC32,sizeof(void*),0))));}_LL2DD:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp608=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp609=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5EF),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4C1 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp609);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4C1;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp609);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp5FC,_tmp608,(struct Cyc_Absyn_Exp*)e1,_tmp5FE,0));goto
_LL2D2;}}_LL2D5: {struct Cyc_Absyn_Comprehension_e_struct*_tmp5F0=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp5ED;if(_tmp5F0->tag != 29)goto _LL2D7;else{_tmp5F1=_tmp5F0->f1;_tmp5F2=_tmp5F0->f2;
_tmp5F3=_tmp5F0->f3;_tmp5F4=_tmp5F0->f4;}}_LL2D6: {int is_dyneither_ptr=0;{void*
_tmp60A=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp60C;void*_tmp60D;
struct Cyc_Absyn_Tqual _tmp60E;struct Cyc_Absyn_PtrAtts _tmp60F;union Cyc_Absyn_Constraint*
_tmp610;union Cyc_Absyn_Constraint*_tmp611;_LL2E3: {struct Cyc_Absyn_PointerType_struct*
_tmp60B=(struct Cyc_Absyn_PointerType_struct*)_tmp60A;if(_tmp60B->tag != 5)goto
_LL2E5;else{_tmp60C=_tmp60B->f1;_tmp60D=_tmp60C.elt_typ;_tmp60E=_tmp60C.elt_tq;
_tmp60F=_tmp60C.ptr_atts;_tmp610=_tmp60F.bounds;_tmp611=_tmp60F.zero_term;}}
_LL2E4: is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp610)== (void*)& Cyc_Absyn_DynEither_b_val;goto _LL2E2;
_LL2E5:;_LL2E6: {const char*_tmpC36;void*_tmpC35;(_tmpC35=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC36="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC36,sizeof(char),42))),_tag_dyneither(_tmpC35,sizeof(void*),0)));}
_LL2E2:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5F3->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp5F2);{struct Cyc_Absyn_Exp*_tmp614=
Cyc_Absyn_var_exp(max,0);if(_tmp5F4)_tmp614=Cyc_Absyn_add_exp(_tmp614,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp5F1,Cyc_Absyn_var_exp(max,0),_tmp5F3,_tmp5F4,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp616;struct _RegionHandle*_tmp617;struct Cyc_Toc_Env*_tmp615=
nv;_tmp616=*_tmp615;_tmp617=_tmp616.rgn;{struct _tuple19*_tmpC39;struct Cyc_List_List*
_tmpC38;struct Cyc_List_List*decls=(_tmpC38=_region_malloc(_tmp617,sizeof(*
_tmpC38)),((_tmpC38->hd=((_tmpC39=_region_malloc(_tmp617,sizeof(*_tmpC39)),((
_tmpC39->f1=max,((_tmpC39->f2=Cyc_Absyn_uint_typ,((_tmpC39->f3=(struct Cyc_Absyn_Exp*)
_tmp5F2,_tmpC39)))))))),((_tmpC38->tl=0,_tmpC38)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4C1 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC3A[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC3A[1]=_tmp614,((
_tmpC3A[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3A,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4C1;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC3B[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC3B[1]=_tmp614,((_tmpC3B[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC3B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC3E;struct Cyc_List_List*
_tmpC3D;decls=((_tmpC3D=_region_malloc(_tmp617,sizeof(*_tmpC3D)),((_tmpC3D->hd=((
_tmpC3E=_region_malloc(_tmp617,sizeof(*_tmpC3E)),((_tmpC3E->f1=a,((_tmpC3E->f2=
ptr_typ,((_tmpC3E->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC3E)))))))),((_tmpC3D->tl=
decls,_tmpC3D))))));}if(is_dyneither_ptr){struct _tuple1*_tmp61C=Cyc_Toc_temp_var();
void*_tmp61D=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp61E=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC3F[3];struct Cyc_Absyn_Exp*_tmp61F=Cyc_Absyn_fncall_exp(
_tmp61E,((_tmpC3F[2]=_tmp614,((_tmpC3F[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC3F[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC3F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC42;struct Cyc_List_List*_tmpC41;decls=((_tmpC41=
_region_malloc(_tmp617,sizeof(*_tmpC41)),((_tmpC41->hd=((_tmpC42=_region_malloc(
_tmp617,sizeof(*_tmpC42)),((_tmpC42->f1=_tmp61C,((_tmpC42->f2=_tmp61D,((_tmpC42->f3=(
struct Cyc_Absyn_Exp*)_tmp61F,_tmpC42)))))))),((_tmpC41->tl=decls,_tmpC41))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp61C,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp623=decls;for(0;_tmp623 != 0;_tmp623=_tmp623->tl){struct
_tuple1*_tmp625;void*_tmp626;struct Cyc_Absyn_Exp*_tmp627;struct _tuple19 _tmp624=*((
struct _tuple19*)_tmp623->hd);_tmp625=_tmp624.f1;_tmp626=_tmp624.f2;_tmp627=
_tmp624.f3;s=Cyc_Absyn_declare_stmt(_tmp625,_tmp626,_tmp627,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);}}goto _LL2D2;}}}}_LL2D7: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5F5=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp5ED;if(_tmp5F5->tag != 30)goto _LL2D9;else{
_tmp5F6=_tmp5F5->f1;_tmp5F7=_tmp5F5->f2;_tmp5F8=_tmp5F5->f3;_tmp5F9=_tmp5F5->f4;}}
_LL2D8: e->r=(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4C2->topt))->v,_tmp5F7 != 0,1,_tmp4C1,_tmp5F8,_tmp5F6))->r;goto _LL2D2;_LL2D9: {
struct Cyc_Absyn_Tuple_e_struct*_tmp5FA=(struct Cyc_Absyn_Tuple_e_struct*)_tmp5ED;
if(_tmp5FA->tag != 26)goto _LL2DB;else{_tmp5FB=_tmp5FA->f1;}}_LL2DA: e->r=(Cyc_Toc_init_tuple(
nv,1,_tmp4C1,_tmp5FB))->r;goto _LL2D2;_LL2DB:;_LL2DC: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4C2->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct _tuple1*_tmp62A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp62B=Cyc_Absyn_var_exp(_tmp62A,0);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(_tmp62B,0),0);struct Cyc_Absyn_Exp*inner_mexp=mexp;if(_tmp4C1
== 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4C1;Cyc_Toc_exp_to_c(nv,r);mexp=
Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp62C=_tmp4C2->r;void*_tmp62E;
struct Cyc_Absyn_Exp*_tmp62F;_LL2E8: {struct Cyc_Absyn_Cast_e_struct*_tmp62D=(
struct Cyc_Absyn_Cast_e_struct*)_tmp62C;if(_tmp62D->tag != 15)goto _LL2EA;else{
_tmp62E=(void*)_tmp62D->f1;_tmp62F=_tmp62D->f2;}}_LL2E9:{struct _tuple0 _tmpC43;
struct _tuple0 _tmp631=(_tmpC43.f1=Cyc_Tcutil_compress(_tmp62E),((_tmpC43.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp62F->topt))->v),_tmpC43)));void*
_tmp632;struct Cyc_Absyn_PtrInfo _tmp634;void*_tmp635;struct Cyc_Absyn_PtrAtts
_tmp636;union Cyc_Absyn_Constraint*_tmp637;void*_tmp638;struct Cyc_Absyn_PtrInfo
_tmp63A;struct Cyc_Absyn_PtrAtts _tmp63B;union Cyc_Absyn_Constraint*_tmp63C;_LL2ED:
_tmp632=_tmp631.f1;{struct Cyc_Absyn_PointerType_struct*_tmp633=(struct Cyc_Absyn_PointerType_struct*)
_tmp632;if(_tmp633->tag != 5)goto _LL2EF;else{_tmp634=_tmp633->f1;_tmp635=_tmp634.elt_typ;
_tmp636=_tmp634.ptr_atts;_tmp637=_tmp636.bounds;}}_tmp638=_tmp631.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp639=(struct Cyc_Absyn_PointerType_struct*)_tmp638;if(_tmp639->tag != 5)goto
_LL2EF;else{_tmp63A=_tmp639->f1;_tmp63B=_tmp63A.ptr_atts;_tmp63C=_tmp63B.bounds;}}
_LL2EE:{struct _tuple0 _tmpC44;struct _tuple0 _tmp63E=(_tmpC44.f1=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp637),((
_tmpC44.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp63C),_tmpC44)));void*_tmp63F;void*_tmp641;struct Cyc_Absyn_Exp*
_tmp643;_LL2F2: _tmp63F=_tmp63E.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp640=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp63F;if(_tmp640->tag != 0)goto _LL2F4;}
_tmp641=_tmp63E.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp642=(struct Cyc_Absyn_Upper_b_struct*)
_tmp641;if(_tmp642->tag != 1)goto _LL2F4;else{_tmp643=_tmp642->f1;}}_LL2F3: Cyc_Toc_exp_to_c(
nv,_tmp62F);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp644=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC45[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp644,((_tmpC45[3]=_tmp643,((_tmpC45[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp635),0),((_tmpC45[1]=_tmp62F,((_tmpC45[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC45,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2F1;}_LL2F4:;_LL2F5: goto _LL2F1;_LL2F1:;}goto _LL2EC;_LL2EF:;
_LL2F0: goto _LL2EC;_LL2EC:;}goto _LL2E7;_LL2EA:;_LL2EB: goto _LL2E7;_LL2E7:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp646=Cyc_Absyn_exp_stmt(_tmp62B,0);struct Cyc_Absyn_Exp*
_tmp647=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4C2);_tmp646=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp62B,_tmp647,0),_tmp4C2,0),
_tmp646,0);{void*_tmp648=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp62A,_tmp648,(struct Cyc_Absyn_Exp*)mexp,_tmp646,0));}}
goto _LL2D2;}}_LL2D2:;}goto _LL208;_LL231: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4C3=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp481;if(_tmp4C3->tag != 19)goto
_LL233;else{_tmp4C4=_tmp4C3->f1;}}_LL232: Cyc_Toc_exp_to_c(nv,_tmp4C4);goto _LL208;
_LL233: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4C5=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp481;if(_tmp4C5->tag != 18)goto _LL235;else{_tmp4C6=(void*)_tmp4C5->f1;}}_LL234:{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC48;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC47;e->r=(void*)((_tmpC47=_cycalloc(sizeof(*_tmpC47)),((_tmpC47[0]=((_tmpC48.tag=
18,((_tmpC48.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4C6),_tmpC48)))),_tmpC47))));}
goto _LL208;_LL235: {struct Cyc_Absyn_Gentyp_e_struct*_tmp4C7=(struct Cyc_Absyn_Gentyp_e_struct*)
_tmp481;if(_tmp4C7->tag != 21)goto _LL237;}_LL236: {const char*_tmpC4B;void*_tmpC4A;(
_tmpC4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC4B="__gen() in code generator",_tag_dyneither(_tmpC4B,sizeof(char),26))),
_tag_dyneither(_tmpC4A,sizeof(void*),0)));}_LL237: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp4C8=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp481;if(_tmp4C8->tag != 20)goto
_LL239;else{_tmp4C9=(void*)_tmp4C8->f1;_tmp4CA=(void*)_tmp4C8->f2;{struct Cyc_Absyn_StructField_struct*
_tmp4CB=(struct Cyc_Absyn_StructField_struct*)_tmp4CA;if(_tmp4CB->tag != 0)goto
_LL239;else{_tmp4CC=_tmp4CB->f1;}}}}_LL238:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC55;struct Cyc_Absyn_StructField_struct _tmpC54;struct Cyc_Absyn_StructField_struct*
_tmpC53;struct Cyc_Absyn_Offsetof_e_struct*_tmpC52;e->r=(void*)((_tmpC52=
_cycalloc(sizeof(*_tmpC52)),((_tmpC52[0]=((_tmpC55.tag=20,((_tmpC55.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4C9),((_tmpC55.f2=(void*)((void*)((_tmpC53=_cycalloc(
sizeof(*_tmpC53)),((_tmpC53[0]=((_tmpC54.tag=0,((_tmpC54.f1=_tmp4CC,_tmpC54)))),
_tmpC53))))),_tmpC55)))))),_tmpC52))));}goto _LL208;_LL239: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp4CD=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp481;if(_tmp4CD->tag != 20)goto
_LL23B;else{_tmp4CE=(void*)_tmp4CD->f1;_tmp4CF=(void*)_tmp4CD->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp4D0=(struct Cyc_Absyn_TupleIndex_struct*)_tmp4CF;if(_tmp4D0->tag != 1)goto
_LL23B;else{_tmp4D1=_tmp4D0->f1;}}}}_LL23A:{void*_tmp651=Cyc_Tcutil_compress(
_tmp4CE);struct Cyc_Absyn_AggrInfo _tmp653;union Cyc_Absyn_AggrInfoU _tmp654;struct
Cyc_List_List*_tmp656;_LL2F7: {struct Cyc_Absyn_AggrType_struct*_tmp652=(struct
Cyc_Absyn_AggrType_struct*)_tmp651;if(_tmp652->tag != 12)goto _LL2F9;else{_tmp653=
_tmp652->f1;_tmp654=_tmp653.aggr_info;}}_LL2F8: {struct Cyc_Absyn_Aggrdecl*
_tmp659=Cyc_Absyn_get_known_aggrdecl(_tmp654);if(_tmp659->impl == 0){const char*
_tmpC58;void*_tmpC57;(_tmpC57=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC58="struct fields must be known",
_tag_dyneither(_tmpC58,sizeof(char),28))),_tag_dyneither(_tmpC57,sizeof(void*),0)));}
_tmp656=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp659->impl))->fields;goto
_LL2FA;}_LL2F9: {struct Cyc_Absyn_AnonAggrType_struct*_tmp655=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp651;if(_tmp655->tag != 13)goto _LL2FB;else{_tmp656=_tmp655->f2;}}_LL2FA: {
struct Cyc_Absyn_Aggrfield*_tmp65C=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp656,(int)_tmp4D1);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC62;struct Cyc_Absyn_StructField_struct _tmpC61;struct Cyc_Absyn_StructField_struct*
_tmpC60;struct Cyc_Absyn_Offsetof_e_struct*_tmpC5F;e->r=(void*)((_tmpC5F=
_cycalloc(sizeof(*_tmpC5F)),((_tmpC5F[0]=((_tmpC62.tag=20,((_tmpC62.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC62.f2=(void*)((void*)((_tmpC60=_cycalloc(
sizeof(*_tmpC60)),((_tmpC60[0]=((_tmpC61.tag=0,((_tmpC61.f1=_tmp65C->name,
_tmpC61)))),_tmpC60))))),_tmpC62)))))),_tmpC5F))));}goto _LL2F6;}_LL2FB: {struct
Cyc_Absyn_TupleType_struct*_tmp657=(struct Cyc_Absyn_TupleType_struct*)_tmp651;
if(_tmp657->tag != 11)goto _LL2FD;}_LL2FC:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC6C;struct Cyc_Absyn_StructField_struct _tmpC6B;struct Cyc_Absyn_StructField_struct*
_tmpC6A;struct Cyc_Absyn_Offsetof_e_struct*_tmpC69;e->r=(void*)((_tmpC69=
_cycalloc(sizeof(*_tmpC69)),((_tmpC69[0]=((_tmpC6C.tag=20,((_tmpC6C.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC6C.f2=(void*)((void*)((_tmpC6A=_cycalloc(
sizeof(*_tmpC6A)),((_tmpC6A[0]=((_tmpC6B.tag=0,((_tmpC6B.f1=Cyc_Absyn_fieldname((
int)(_tmp4D1 + 1)),_tmpC6B)))),_tmpC6A))))),_tmpC6C)))))),_tmpC69))));}goto _LL2F6;
_LL2FD: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp658=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp651;if(_tmp658->tag != 4)goto _LL2FF;}_LL2FE: if(_tmp4D1 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC76;struct Cyc_Absyn_StructField_struct _tmpC75;struct Cyc_Absyn_StructField_struct*
_tmpC74;struct Cyc_Absyn_Offsetof_e_struct*_tmpC73;e->r=(void*)((_tmpC73=
_cycalloc(sizeof(*_tmpC73)),((_tmpC73[0]=((_tmpC76.tag=20,((_tmpC76.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC76.f2=(void*)((void*)((_tmpC74=_cycalloc(
sizeof(*_tmpC74)),((_tmpC74[0]=((_tmpC75.tag=0,((_tmpC75.f1=Cyc_Toc_tag_sp,
_tmpC75)))),_tmpC74))))),_tmpC76)))))),_tmpC73))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC80;struct Cyc_Absyn_StructField_struct _tmpC7F;struct Cyc_Absyn_StructField_struct*
_tmpC7E;struct Cyc_Absyn_Offsetof_e_struct*_tmpC7D;e->r=(void*)((_tmpC7D=
_cycalloc(sizeof(*_tmpC7D)),((_tmpC7D[0]=((_tmpC80.tag=20,((_tmpC80.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC80.f2=(void*)((void*)((_tmpC7E=_cycalloc(
sizeof(*_tmpC7E)),((_tmpC7E[0]=((_tmpC7F.tag=0,((_tmpC7F.f1=Cyc_Absyn_fieldname((
int)_tmp4D1),_tmpC7F)))),_tmpC7E))))),_tmpC80)))))),_tmpC7D))));}goto _LL2F6;
_LL2FF:;_LL300: {const char*_tmpC83;void*_tmpC82;(_tmpC82=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC83="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC83,sizeof(char),41))),_tag_dyneither(_tmpC82,sizeof(void*),0)));}
_LL2F6:;}goto _LL208;_LL23B: {struct Cyc_Absyn_Deref_e_struct*_tmp4D2=(struct Cyc_Absyn_Deref_e_struct*)
_tmp481;if(_tmp4D2->tag != 22)goto _LL23D;else{_tmp4D3=_tmp4D2->f1;}}_LL23C: {void*
_tmp66F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D3->topt))->v);{
void*_tmp670=_tmp66F;struct Cyc_Absyn_PtrInfo _tmp672;void*_tmp673;struct Cyc_Absyn_Tqual
_tmp674;struct Cyc_Absyn_PtrAtts _tmp675;void*_tmp676;union Cyc_Absyn_Constraint*
_tmp677;union Cyc_Absyn_Constraint*_tmp678;union Cyc_Absyn_Constraint*_tmp679;
_LL302: {struct Cyc_Absyn_PointerType_struct*_tmp671=(struct Cyc_Absyn_PointerType_struct*)
_tmp670;if(_tmp671->tag != 5)goto _LL304;else{_tmp672=_tmp671->f1;_tmp673=_tmp672.elt_typ;
_tmp674=_tmp672.elt_tq;_tmp675=_tmp672.ptr_atts;_tmp676=_tmp675.rgn;_tmp677=
_tmp675.nullable;_tmp678=_tmp675.bounds;_tmp679=_tmp675.zero_term;}}_LL303:{void*
_tmp67A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp678);struct Cyc_Absyn_Exp*_tmp67C;_LL307: {struct Cyc_Absyn_Upper_b_struct*
_tmp67B=(struct Cyc_Absyn_Upper_b_struct*)_tmp67A;if(_tmp67B->tag != 1)goto _LL309;
else{_tmp67C=_tmp67B->f1;}}_LL308: {int do_null_check=Cyc_Toc_need_null_check(
_tmp4D3);Cyc_Toc_exp_to_c(nv,_tmp4D3);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpC86;void*_tmpC85;(_tmpC85=0,Cyc_Tcutil_warn(e->loc,((_tmpC86="inserted null check due to dereference",
_tag_dyneither(_tmpC86,sizeof(char),39))),_tag_dyneither(_tmpC85,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC87;_tmp4D3->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp66F),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC87=_cycalloc(sizeof(*
_tmpC87)),((_tmpC87->hd=Cyc_Absyn_copy_exp(_tmp4D3),((_tmpC87->tl=0,_tmpC87)))))),
0));}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp679)){unsigned int _tmp682;int _tmp683;struct _tuple11 _tmp681=Cyc_Evexp_eval_const_uint_exp(
_tmp67C);_tmp682=_tmp681.f1;_tmp683=_tmp681.f2;if(!_tmp683  || _tmp682 <= 0){const
char*_tmpC8A;void*_tmpC89;(_tmpC89=0,Cyc_Tcutil_terr(e->loc,((_tmpC8A="cannot determine dereference is in bounds",
_tag_dyneither(_tmpC8A,sizeof(char),42))),_tag_dyneither(_tmpC89,sizeof(void*),0)));}}
goto _LL306;}_LL309: {struct Cyc_Absyn_DynEither_b_struct*_tmp67D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp67A;if(_tmp67D->tag != 0)goto _LL306;}_LL30A: {struct Cyc_Absyn_Exp*_tmp686=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpC8B;_tmp686->topt=((_tmpC8B=_cycalloc(sizeof(*
_tmpC8B)),((_tmpC8B->v=Cyc_Absyn_uint_typ,_tmpC8B))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4D3,_tmp686);Cyc_Toc_exp_to_c(nv,e);goto _LL306;}_LL306:;}goto _LL301;_LL304:;
_LL305: {const char*_tmpC8E;void*_tmpC8D;(_tmpC8D=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC8E="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpC8E,sizeof(char),29))),_tag_dyneither(_tmpC8D,sizeof(void*),0)));}
_LL301:;}goto _LL208;}_LL23D: {struct Cyc_Absyn_AggrMember_e_struct*_tmp4D4=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp481;if(_tmp4D4->tag != 23)goto _LL23F;
else{_tmp4D5=_tmp4D4->f1;_tmp4D6=_tmp4D4->f2;_tmp4D7=_tmp4D4->f3;_tmp4D8=_tmp4D4->f4;}}
_LL23E: {int is_poly=Cyc_Toc_is_poly_project(e);void*e1_cyc_type=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp4D5->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4D5);if(
_tmp4D7  && _tmp4D8)e->r=Cyc_Toc_check_tagged_union(_tmp4D5,Cyc_Toc_typ_to_c(
e1_cyc_type),e1_cyc_type,_tmp4D6,Cyc_Absyn_aggrmember_exp);if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL208;}_LL23F: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp4D9=(
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp481;if(_tmp4D9->tag != 24)goto _LL241;
else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4D9->f2;_tmp4DC=_tmp4D9->f3;_tmp4DD=_tmp4D9->f4;}}
_LL240: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp4DA->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp4DA);Cyc_Toc_exp_to_c(
nv,_tmp4DA);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp68B;struct Cyc_Absyn_Tqual
_tmp68C;struct Cyc_Absyn_PtrAtts _tmp68D;void*_tmp68E;union Cyc_Absyn_Constraint*
_tmp68F;union Cyc_Absyn_Constraint*_tmp690;union Cyc_Absyn_Constraint*_tmp691;
struct Cyc_Absyn_PtrInfo _tmp68A=Cyc_Toc_get_ptr_type(e1typ);_tmp68B=_tmp68A.elt_typ;
_tmp68C=_tmp68A.elt_tq;_tmp68D=_tmp68A.ptr_atts;_tmp68E=_tmp68D.rgn;_tmp68F=
_tmp68D.nullable;_tmp690=_tmp68D.bounds;_tmp691=_tmp68D.zero_term;{void*_tmp692=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp690);struct Cyc_Absyn_Exp*_tmp694;_LL30C: {struct Cyc_Absyn_Upper_b_struct*
_tmp693=(struct Cyc_Absyn_Upper_b_struct*)_tmp692;if(_tmp693->tag != 1)goto _LL30E;
else{_tmp694=_tmp693->f1;}}_LL30D: {unsigned int _tmp697;int _tmp698;struct
_tuple11 _tmp696=Cyc_Evexp_eval_const_uint_exp(_tmp694);_tmp697=_tmp696.f1;
_tmp698=_tmp696.f2;if(_tmp698){if(_tmp697 < 1){const char*_tmpC91;void*_tmpC90;(
_tmpC90=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC91="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC91,
sizeof(char),44))),_tag_dyneither(_tmpC90,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpC94;void*_tmpC93;(_tmpC93=0,Cyc_Tcutil_warn(
e->loc,((_tmpC94="inserted null check due to dereference",_tag_dyneither(_tmpC94,
sizeof(char),39))),_tag_dyneither(_tmpC93,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpC95[1];_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC95[0]=Cyc_Absyn_new_exp(_tmp4DA->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC95,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));}}}else{if(!Cyc_Evexp_c_can_eval(_tmp694)){const char*_tmpC98;void*
_tmpC97;(_tmpC97=0,Cyc_Tcutil_terr(e->loc,((_tmpC98="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpC98,sizeof(char),47))),_tag_dyneither(_tmpC97,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC99[4];_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpC99[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpC99[2]=Cyc_Absyn_sizeoftyp_exp(_tmp68B,0),((
_tmpC99[1]=_tmp694,((_tmpC99[0]=Cyc_Absyn_new_exp(_tmp4DA->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC99,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));}}goto _LL30B;}_LL30E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp695=(struct Cyc_Absyn_DynEither_b_struct*)_tmp692;if(_tmp695->tag != 0)goto
_LL30B;}_LL30F: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp68B);{struct Cyc_Absyn_Exp*
_tmpC9A[3];_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp68C),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpC9A[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpC9A[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC9A[0]=Cyc_Absyn_new_exp(_tmp4DA->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC9A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL30B;}_LL30B:;}if(
_tmp4DC  && _tmp4DD)e->r=Cyc_Toc_check_tagged_union(_tmp4DA,Cyc_Toc_typ_to_c(
e1typ),_tmp68B,_tmp4DB,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp4DD)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL208;}}_LL241: {struct Cyc_Absyn_Subscript_e_struct*_tmp4DE=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp481;if(_tmp4DE->tag != 25)goto _LL243;
else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}_LL242: {void*_tmp6A2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4DF->topt))->v);{void*_tmp6A3=
_tmp6A2;struct Cyc_List_List*_tmp6A5;struct Cyc_Absyn_PtrInfo _tmp6A7;void*_tmp6A8;
struct Cyc_Absyn_Tqual _tmp6A9;struct Cyc_Absyn_PtrAtts _tmp6AA;void*_tmp6AB;union
Cyc_Absyn_Constraint*_tmp6AC;union Cyc_Absyn_Constraint*_tmp6AD;union Cyc_Absyn_Constraint*
_tmp6AE;_LL311: {struct Cyc_Absyn_TupleType_struct*_tmp6A4=(struct Cyc_Absyn_TupleType_struct*)
_tmp6A3;if(_tmp6A4->tag != 11)goto _LL313;else{_tmp6A5=_tmp6A4->f1;}}_LL312: Cyc_Toc_exp_to_c(
nv,_tmp4DF);Cyc_Toc_exp_to_c(nv,_tmp4E0);{unsigned int _tmp6B0;int _tmp6B1;struct
_tuple11 _tmp6AF=Cyc_Evexp_eval_const_uint_exp(_tmp4E0);_tmp6B0=_tmp6AF.f1;
_tmp6B1=_tmp6AF.f2;if(!_tmp6B1){const char*_tmpC9D;void*_tmpC9C;(_tmpC9C=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC9D="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpC9D,sizeof(char),44))),_tag_dyneither(_tmpC9C,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4DF,Cyc_Absyn_fieldname((int)(_tmp6B0 + 1)));
goto _LL310;}_LL313: {struct Cyc_Absyn_PointerType_struct*_tmp6A6=(struct Cyc_Absyn_PointerType_struct*)
_tmp6A3;if(_tmp6A6->tag != 5)goto _LL315;else{_tmp6A7=_tmp6A6->f1;_tmp6A8=_tmp6A7.elt_typ;
_tmp6A9=_tmp6A7.elt_tq;_tmp6AA=_tmp6A7.ptr_atts;_tmp6AB=_tmp6AA.rgn;_tmp6AC=
_tmp6AA.nullable;_tmp6AD=_tmp6AA.bounds;_tmp6AE=_tmp6AA.zero_term;}}_LL314: {
struct Cyc_List_List*_tmp6B4=Cyc_Toc_get_relns(_tmp4DF);int in_bnds=0;{void*
_tmp6B5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6AD);_LL318:;_LL319: in_bnds=Cyc_Toc_check_bounds(_tmp6B4,_tmp4DF,_tmp4E0);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCA1;void*_tmpCA0[1];
struct Cyc_String_pa_struct _tmpC9F;(_tmpC9F.tag=0,((_tmpC9F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCA0[0]=&
_tmpC9F,Cyc_Tcutil_warn(e->loc,((_tmpCA1="bounds check necessary for %s",
_tag_dyneither(_tmpCA1,sizeof(char),30))),_tag_dyneither(_tmpCA0,sizeof(void*),1)))))));}
_LL317:;}Cyc_Toc_exp_to_c(nv,_tmp4DF);Cyc_Toc_exp_to_c(nv,_tmp4E0);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6B9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6AD);struct Cyc_Absyn_Exp*_tmp6BB;_LL31B: {struct Cyc_Absyn_Upper_b_struct*
_tmp6BA=(struct Cyc_Absyn_Upper_b_struct*)_tmp6B9;if(_tmp6BA->tag != 1)goto _LL31D;
else{_tmp6BB=_tmp6BA->f1;}}_LL31C: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6AC);void*ta1=Cyc_Toc_typ_to_c(_tmp6A8);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6A9);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6AE)){if(!Cyc_Evexp_c_can_eval(_tmp6BB)){const char*_tmpCA4;void*_tmpCA3;(
_tmpCA3=0,Cyc_Tcutil_terr(e->loc,((_tmpCA4="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCA4,sizeof(char),40))),_tag_dyneither(_tmpCA3,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4DF);struct Cyc_Absyn_Exp*_tmpCA5[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCA5[2]=_tmp4E0,((_tmpCA5[1]=_tmp6BB,((
_tmpCA5[0]=_tmp4DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCA5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6BB)){const char*_tmpCA8;void*_tmpCA7;(
_tmpCA7=0,Cyc_Tcutil_terr(e->loc,((_tmpCA8="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCA8,sizeof(char),40))),_tag_dyneither(_tmpCA7,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCAB;void*_tmpCAA;(_tmpCAA=0,Cyc_Tcutil_warn(
e->loc,((_tmpCAB="inserted null check due to dereference",_tag_dyneither(_tmpCAB,
sizeof(char),39))),_tag_dyneither(_tmpCAA,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCAC[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCAC[3]=_tmp4E0,((_tmpCAC[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCAC[1]=
_tmp6BB,((_tmpCAC[0]=_tmp4DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCAC,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));}}else{
struct Cyc_Absyn_Exp*_tmpCAD[2];_tmp4E0->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCAD[1]=Cyc_Absyn_copy_exp(_tmp4E0),((_tmpCAD[0]=_tmp6BB,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAD,sizeof(struct Cyc_Absyn_Exp*),
2)))))));}}}goto _LL31A;}_LL31D: {struct Cyc_Absyn_DynEither_b_struct*_tmp6BC=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6B9;if(_tmp6BC->tag != 0)goto _LL31A;}
_LL31E: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6A8);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A9),
Cyc_Absyn_aggrmember_exp(_tmp4DF,Cyc_Toc_curr_sp,0)),_tmp4E0);}else{struct Cyc_Absyn_Exp*
_tmp6C6=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCAE[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A9),Cyc_Absyn_fncall_exp(
_tmp6C6,((_tmpCAE[2]=_tmp4E0,((_tmpCAE[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCAE[0]=_tmp4DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCAE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL31A;}
_LL31A:;}goto _LL310;}_LL315:;_LL316: {const char*_tmpCB1;void*_tmpCB0;(_tmpCB0=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCB1="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCB1,sizeof(char),49))),_tag_dyneither(_tmpCB0,sizeof(void*),0)));}_LL310:;}
goto _LL208;}_LL243: {struct Cyc_Absyn_Tuple_e_struct*_tmp4E1=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp481;if(_tmp4E1->tag != 26)goto _LL245;else{_tmp4E2=_tmp4E1->f1;}}_LL244: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4E2))->r;else{struct
Cyc_List_List*_tmp6CA=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4E2);void*_tmp6CB=Cyc_Toc_add_tuple_type(
_tmp6CA);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp4E2 != 0;(_tmp4E2=_tmp4E2->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4E2->hd);{struct _tuple16*
_tmpCB4;struct Cyc_List_List*_tmpCB3;dles=((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((
_tmpCB3->hd=((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->f1=0,((_tmpCB4->f2=(
struct Cyc_Absyn_Exp*)_tmp4E2->hd,_tmpCB4)))))),((_tmpCB3->tl=dles,_tmpCB3))))));}}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL208;_LL245: {struct Cyc_Absyn_Array_e_struct*
_tmp4E3=(struct Cyc_Absyn_Array_e_struct*)_tmp481;if(_tmp4E3->tag != 28)goto _LL247;
else{_tmp4E4=_tmp4E3->f1;}}_LL246: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4E4);{
struct Cyc_List_List*_tmp6CE=_tmp4E4;for(0;_tmp6CE != 0;_tmp6CE=_tmp6CE->tl){
struct _tuple16 _tmp6D0;struct Cyc_Absyn_Exp*_tmp6D1;struct _tuple16*_tmp6CF=(struct
_tuple16*)_tmp6CE->hd;_tmp6D0=*_tmp6CF;_tmp6D1=_tmp6D0.f2;Cyc_Toc_exp_to_c(nv,
_tmp6D1);}}goto _LL208;_LL247: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4E5=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp481;if(_tmp4E5->tag != 29)goto _LL249;
else{_tmp4E6=_tmp4E5->f1;_tmp4E7=_tmp4E5->f2;_tmp4E8=_tmp4E5->f3;_tmp4E9=_tmp4E5->f4;}}
_LL248: {unsigned int _tmp6D3;int _tmp6D4;struct _tuple11 _tmp6D2=Cyc_Evexp_eval_const_uint_exp(
_tmp4E7);_tmp6D3=_tmp6D2.f1;_tmp6D4=_tmp6D2.f2;{void*_tmp6D5=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E8->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4E8);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4E8)){if(!_tmp6D4){
const char*_tmpCB7;void*_tmpCB6;(_tmpCB6=0,Cyc_Tcutil_terr(_tmp4E7->loc,((_tmpCB7="cannot determine value of constant",
_tag_dyneither(_tmpCB7,sizeof(char),35))),_tag_dyneither(_tmpCB6,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6D3;++ i){struct _tuple16*_tmpCBA;struct Cyc_List_List*
_tmpCB9;es=((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->hd=((_tmpCBA=
_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->f1=0,((_tmpCBA->f2=_tmp4E8,_tmpCBA)))))),((
_tmpCB9->tl=es,_tmpCB9))))));}}if(_tmp4E9){struct Cyc_Absyn_Exp*_tmp6DA=Cyc_Toc_cast_it(
_tmp6D5,Cyc_Absyn_uint_exp(0,0));struct _tuple16*_tmpCBD;struct Cyc_List_List*
_tmpCBC;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC->hd=((
_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD->f1=0,((_tmpCBD->f2=_tmp6DA,
_tmpCBD)))))),((_tmpCBC->tl=0,_tmpCBC)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL208;}}}_LL249: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4EA=(struct
Cyc_Absyn_Aggregate_e_struct*)_tmp481;if(_tmp4EA->tag != 30)goto _LL24B;else{
_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;_tmp4ED=_tmp4EA->f3;_tmp4EE=_tmp4EA->f4;}}
_LL24A: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp4EC
!= 0,0,0,_tmp4ED,_tmp4EB))->r;else{if(_tmp4EE == 0){const char*_tmpCC0;void*
_tmpCBF;(_tmpCBF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCC0="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCC0,sizeof(
char),38))),_tag_dyneither(_tmpCBF,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp4EE;struct Cyc_Toc_Env _tmp6E0;struct
_RegionHandle*_tmp6E1;struct Cyc_Toc_Env*_tmp6DF=nv;_tmp6E0=*_tmp6DF;_tmp6E1=
_tmp6E0.rgn;{struct Cyc_List_List*_tmp6E2=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6E1,e->loc,_tmp4ED,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4EE->impl))->tagged){struct
_tuple20 _tmp6E4;struct Cyc_Absyn_Aggrfield*_tmp6E5;struct Cyc_Absyn_Exp*_tmp6E6;
struct _tuple20*_tmp6E3=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6E2))->hd;_tmp6E4=*_tmp6E3;_tmp6E5=_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;{void*
_tmp6E7=_tmp6E5->type;Cyc_Toc_exp_to_c(nv,_tmp6E6);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6E7))_tmp6E6->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6E6->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp4EE,_tmp6E5->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple16*_tmpCC5;struct _tuple16*_tmpCC4;struct _tuple16*_tmpCC3[2];
struct Cyc_List_List*_tmp6E8=(_tmpCC3[1]=((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((
_tmpCC5->f1=0,((_tmpCC5->f2=_tmp6E6,_tmpCC5)))))),((_tmpCC3[0]=((_tmpCC4=
_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->f1=0,((_tmpCC4->f2=field_tag_exp,_tmpCC4)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCC3,sizeof(struct _tuple16*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp6E8,0);struct Cyc_Absyn_FieldName_struct*_tmpCCB;struct Cyc_Absyn_FieldName_struct
_tmpCCA;void*_tmpCC9[1];struct Cyc_List_List*ds=(_tmpCC9[0]=(void*)((_tmpCCB=
_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB[0]=((_tmpCCA.tag=1,((_tmpCCA.f1=_tmp6E5->name,
_tmpCCA)))),_tmpCCB)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCC9,sizeof(void*),1)));struct _tuple16*_tmpCCE;struct _tuple16*
_tmpCCD[1];struct Cyc_List_List*dles=(_tmpCCD[0]=((_tmpCCE=_cycalloc(sizeof(*
_tmpCCE)),((_tmpCCE->f1=ds,((_tmpCCE->f2=umem,_tmpCCE)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCCD,sizeof(struct
_tuple16*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}}}else{struct Cyc_List_List*
_tmp6F1=0;struct Cyc_List_List*_tmp6F2=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp6F2 != 0;_tmp6F2=_tmp6F2->tl){struct Cyc_List_List*
_tmp6F3=_tmp6E2;for(0;_tmp6F3 != 0;_tmp6F3=_tmp6F3->tl){if((*((struct _tuple20*)
_tmp6F3->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6F2->hd){struct _tuple20
_tmp6F5;struct Cyc_Absyn_Aggrfield*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F7;struct
_tuple20*_tmp6F4=(struct _tuple20*)_tmp6F3->hd;_tmp6F5=*_tmp6F4;_tmp6F6=_tmp6F5.f1;
_tmp6F7=_tmp6F5.f2;{void*_tmp6F8=_tmp6F6->type;Cyc_Toc_exp_to_c(nv,_tmp6F7);if(
Cyc_Toc_is_void_star_or_tvar(_tmp6F8))_tmp6F7->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp6F7->r,0));{struct _tuple16*_tmpCD1;struct Cyc_List_List*
_tmpCD0;_tmp6F1=((_tmpCD0=_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0->hd=((_tmpCD1=
_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->f1=0,((_tmpCD1->f2=_tmp6F7,_tmpCD1)))))),((
_tmpCD0->tl=_tmp6F1,_tmpCD0))))));}break;}}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6F1));}}}}
goto _LL208;_LL24B: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp4EF=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp481;if(_tmp4EF->tag != 31)goto _LL24D;else{_tmp4F0=(void*)_tmp4EF->f1;_tmp4F1=
_tmp4EF->f2;}}_LL24C: {struct Cyc_List_List*fs;{void*_tmp6FB=Cyc_Tcutil_compress(
_tmp4F0);struct Cyc_List_List*_tmp6FD;_LL320: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp6FC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp6FB;if(_tmp6FC->tag != 13)goto
_LL322;else{_tmp6FD=_tmp6FC->f2;}}_LL321: fs=_tmp6FD;goto _LL31F;_LL322:;_LL323: {
const char*_tmpCD5;void*_tmpCD4[1];struct Cyc_String_pa_struct _tmpCD3;(_tmpCD3.tag=
0,((_tmpCD3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp4F0)),((_tmpCD4[0]=& _tmpCD3,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD5="anon struct has type %s",
_tag_dyneither(_tmpCD5,sizeof(char),24))),_tag_dyneither(_tmpCD4,sizeof(void*),1)))))));}
_LL31F:;}{struct Cyc_Toc_Env _tmp702;struct _RegionHandle*_tmp703;struct Cyc_Toc_Env*
_tmp701=nv;_tmp702=*_tmp701;_tmp703=_tmp702.rgn;{struct Cyc_List_List*_tmp704=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp703,e->loc,_tmp4F1,Cyc_Absyn_StructA,fs);for(0;_tmp704 != 0;_tmp704=_tmp704->tl){
struct _tuple20 _tmp706;struct Cyc_Absyn_Aggrfield*_tmp707;struct Cyc_Absyn_Exp*
_tmp708;struct _tuple20*_tmp705=(struct _tuple20*)_tmp704->hd;_tmp706=*_tmp705;
_tmp707=_tmp706.f1;_tmp708=_tmp706.f2;{void*_tmp709=_tmp707->type;Cyc_Toc_exp_to_c(
nv,_tmp708);if(Cyc_Toc_is_void_star_or_tvar(_tmp709))_tmp708->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp708->r,0));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp4F1);}goto _LL208;}}_LL24D: {struct Cyc_Absyn_Datatype_e_struct*_tmp4F2=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp481;if(_tmp4F2->tag != 32)goto _LL24F;else{
_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;_tmp4F5=_tmp4F2->f3;}}_LL24E: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp70A=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp70B=Cyc_Absyn_var_exp(_tmp70A,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCD6;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp4F5->name,((_tmpCD6="_struct",_tag_dyneither(_tmpCD6,sizeof(char),8)))));}
tag_exp=_tmp4F4->is_extensible?Cyc_Absyn_var_exp(_tmp4F5->name,0): Cyc_Toc_datatype_tag(
_tmp4F4,_tmp4F5->name);member_exp=_tmp70B;{struct Cyc_List_List*_tmp70D=_tmp4F5->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp4F3 != 0;(
_tmp4F3=_tmp4F3->tl,_tmp70D=_tmp70D->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp4F3->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp70D))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple16*_tmpCD9;struct
Cyc_List_List*_tmpCD8;dles=((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->hd=((
_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((_tmpCD9->f1=0,((_tmpCD9->f2=cur_e,_tmpCD9)))))),((
_tmpCD8->tl=dles,_tmpCD8))))));}}{struct _tuple16*_tmpCDC;struct Cyc_List_List*
_tmpCDB;dles=((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB->hd=((_tmpCDC=
_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC->f1=0,((_tmpCDC->f2=tag_exp,_tmpCDC)))))),((
_tmpCDB->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCDB))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCDD;struct Cyc_List_List*_tmp712=(_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((
_tmpCDD->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCDD->tl=0,_tmpCDD)))));{int i=1;for(0;_tmp4F3 != 0;(((_tmp4F3=
_tmp4F3->tl,i ++)),_tmp70D=_tmp70D->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp4F3->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp70D))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp713=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCDE;_tmp712=((_tmpCDE=_cycalloc(sizeof(*
_tmpCDE)),((_tmpCDE->hd=_tmp713,((_tmpCDE->tl=_tmp712,_tmpCDE))))));}}}{struct
Cyc_Absyn_Stmt*_tmp715=Cyc_Absyn_exp_stmt(_tmp70B,0);struct Cyc_List_List*_tmpCDF;
struct Cyc_Absyn_Stmt*_tmp716=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCDF=_cycalloc(sizeof(*_tmpCDF)),((
_tmpCDF->hd=_tmp715,((_tmpCDF->tl=_tmp712,_tmpCDF))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp70A,datatype_ctype,0,_tmp716,0));}}goto _LL208;}}
_LL24F: {struct Cyc_Absyn_Enum_e_struct*_tmp4F6=(struct Cyc_Absyn_Enum_e_struct*)
_tmp481;if(_tmp4F6->tag != 33)goto _LL251;}_LL250: goto _LL252;_LL251: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp4F7=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp481;if(_tmp4F7->tag != 34)goto
_LL253;}_LL252: goto _LL208;_LL253: {struct Cyc_Absyn_Malloc_e_struct*_tmp4F8=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp481;if(_tmp4F8->tag != 35)goto _LL255;else{
_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F9.is_calloc;_tmp4FB=_tmp4F9.rgn;_tmp4FC=
_tmp4F9.elt_type;_tmp4FD=_tmp4F9.num_elts;_tmp4FE=_tmp4F9.fat_result;}}_LL254: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4FC)));Cyc_Toc_exp_to_c(nv,
_tmp4FD);if(_tmp4FE){struct _tuple1*_tmp719=Cyc_Toc_temp_var();struct _tuple1*
_tmp71A=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp4FA){xexp=_tmp4FD;if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp719,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp4FC,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp719,0));}{struct Cyc_Absyn_Exp*_tmpCE0[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCE0[2]=Cyc_Absyn_var_exp(_tmp719,0),((_tmpCE0[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCE0[0]=Cyc_Absyn_var_exp(_tmp71A,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCE0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp4FD,0);if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp719,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp4FC,Cyc_Absyn_var_exp(_tmp719,0));}{struct Cyc_Absyn_Exp*_tmpCE1[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCE1[2]=Cyc_Absyn_var_exp(_tmp719,0),((_tmpCE1[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCE1[0]=Cyc_Absyn_var_exp(_tmp71A,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE1,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}}{struct Cyc_Absyn_Stmt*_tmp71D=Cyc_Absyn_declare_stmt(_tmp719,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp71A,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp71D);}}else{struct Cyc_Absyn_Exp*_tmp71E=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp71F=_tmp4FD->r;union Cyc_Absyn_Cnst _tmp721;struct _tuple6 _tmp722;
int _tmp723;_LL325: {struct Cyc_Absyn_Const_e_struct*_tmp720=(struct Cyc_Absyn_Const_e_struct*)
_tmp71F;if(_tmp720->tag != 0)goto _LL327;else{_tmp721=_tmp720->f1;if((_tmp721.Int_c).tag
!= 4)goto _LL327;_tmp722=(struct _tuple6)(_tmp721.Int_c).val;_tmp723=_tmp722.f2;
if(_tmp723 != 1)goto _LL327;}}_LL326: goto _LL324;_LL327:;_LL328: _tmp71E=Cyc_Absyn_times_exp(
_tmp71E,_tmp4FD,0);goto _LL324;_LL324:;}if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp71E))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp4FC,
_tmp71E))->r;}}goto _LL208;}_LL255: {struct Cyc_Absyn_Swap_e_struct*_tmp4FF=(
struct Cyc_Absyn_Swap_e_struct*)_tmp481;if(_tmp4FF->tag != 36)goto _LL257;else{
_tmp500=_tmp4FF->f1;_tmp501=_tmp4FF->f2;}}_LL256: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp500->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp501->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCE4;void*_tmpCE3;(_tmpCE3=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE4="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCE4,sizeof(char),57))),_tag_dyneither(_tmpCE3,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp500);Cyc_Toc_exp_to_c(
nv,_tmp501);{struct Cyc_Absyn_Exp*_tmpCE5[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((
_tmpCE5[1]=Cyc_Toc_push_address_exp(_tmp501),((_tmpCE5[0]=Cyc_Toc_push_address_exp(
_tmp500),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCE5,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL208;}}
_LL257: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp502=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp481;if(_tmp502->tag != 39)goto _LL259;else{_tmp503=_tmp502->f1;_tmp504=_tmp502->f2;}}
_LL258: {void*_tmp727=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp503->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp503);{void*_tmp728=
_tmp727;struct Cyc_Absyn_AggrInfo _tmp72A;union Cyc_Absyn_AggrInfoU _tmp72B;struct
Cyc_Absyn_Aggrdecl**_tmp72C;struct Cyc_Absyn_Aggrdecl*_tmp72D;_LL32A: {struct Cyc_Absyn_AggrType_struct*
_tmp729=(struct Cyc_Absyn_AggrType_struct*)_tmp728;if(_tmp729->tag != 12)goto
_LL32C;else{_tmp72A=_tmp729->f1;_tmp72B=_tmp72A.aggr_info;if((_tmp72B.KnownAggr).tag
!= 2)goto _LL32C;_tmp72C=(struct Cyc_Absyn_Aggrdecl**)(_tmp72B.KnownAggr).val;
_tmp72D=*_tmp72C;}}_LL32B: {struct Cyc_Absyn_Exp*_tmp72E=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp72D,_tmp504),0);struct Cyc_Absyn_Exp*_tmp72F=Cyc_Absyn_aggrmember_exp(
_tmp503,_tmp504,0);struct Cyc_Absyn_Exp*_tmp730=Cyc_Absyn_aggrmember_exp(_tmp72F,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp730,_tmp72E,0))->r;goto _LL329;}
_LL32C:;_LL32D: {const char*_tmpCE8;void*_tmpCE7;(_tmpCE7=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE8="non-aggregate type in tagcheck",
_tag_dyneither(_tmpCE8,sizeof(char),31))),_tag_dyneither(_tmpCE7,sizeof(void*),0)));}
_LL329:;}goto _LL208;}_LL259: {struct Cyc_Absyn_StmtExp_e_struct*_tmp505=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp481;if(_tmp505->tag != 38)goto _LL25B;else{_tmp506=
_tmp505->f1;}}_LL25A: Cyc_Toc_stmt_to_c(nv,_tmp506);goto _LL208;_LL25B: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp507=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp481;if(_tmp507->tag != 37)
goto _LL25D;}_LL25C: {const char*_tmpCEB;void*_tmpCEA;(_tmpCEA=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCEB="UnresolvedMem",
_tag_dyneither(_tmpCEB,sizeof(char),14))),_tag_dyneither(_tmpCEA,sizeof(void*),0)));}
_LL25D: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp508=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp481;if(_tmp508->tag != 27)goto _LL25F;}_LL25E: {const char*_tmpCEE;void*_tmpCED;(
_tmpCED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCEE="compoundlit",_tag_dyneither(_tmpCEE,sizeof(char),12))),_tag_dyneither(
_tmpCED,sizeof(void*),0)));}_LL25F: {struct Cyc_Absyn_Valueof_e_struct*_tmp509=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp481;if(_tmp509->tag != 40)goto _LL208;}
_LL260: {const char*_tmpCF1;void*_tmpCF0;(_tmpCF0=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF1="valueof(-)",
_tag_dyneither(_tmpCF1,sizeof(char),11))),_tag_dyneither(_tmpCF0,sizeof(void*),0)));}
_LL208:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt);static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple21{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple22{struct _tuple1*f1;void*f2;};struct
_tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct
Cyc_List_List*decls);static struct _tuple21 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,
struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,
struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp739=p->r;struct Cyc_Absyn_Vardecl*_tmp73B;struct
Cyc_Absyn_Vardecl _tmp73C;struct _tuple1*_tmp73D;struct Cyc_Absyn_Pat*_tmp73E;
struct Cyc_Absyn_Vardecl*_tmp740;struct Cyc_Absyn_Vardecl _tmp741;struct _tuple1*
_tmp742;struct Cyc_Absyn_Vardecl*_tmp745;struct Cyc_Absyn_Pat*_tmp746;enum Cyc_Absyn_Sign
_tmp749;int _tmp74A;char _tmp74C;struct _dyneither_ptr _tmp74E;struct Cyc_Absyn_Enumdecl*
_tmp750;struct Cyc_Absyn_Enumfield*_tmp751;void*_tmp753;struct Cyc_Absyn_Enumfield*
_tmp754;struct Cyc_Absyn_Pat*_tmp756;struct Cyc_Absyn_Pat _tmp757;void*_tmp758;
struct Cyc_Absyn_Datatypedecl*_tmp75A;struct Cyc_Absyn_Datatypefield*_tmp75B;
struct Cyc_List_List*_tmp75C;struct Cyc_List_List*_tmp75E;struct Cyc_List_List*
_tmp760;struct Cyc_Absyn_AggrInfo*_tmp762;struct Cyc_Absyn_AggrInfo*_tmp764;struct
Cyc_Absyn_AggrInfo _tmp765;union Cyc_Absyn_AggrInfoU _tmp766;struct Cyc_List_List*
_tmp767;struct Cyc_Absyn_Pat*_tmp769;_LL32F: {struct Cyc_Absyn_Var_p_struct*
_tmp73A=(struct Cyc_Absyn_Var_p_struct*)_tmp739;if(_tmp73A->tag != 1)goto _LL331;
else{_tmp73B=_tmp73A->f1;_tmp73C=*_tmp73B;_tmp73D=_tmp73C.name;_tmp73E=_tmp73A->f2;}}
_LL330: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp73D,r),rgn,t,r,path,
_tmp73E,fail_stmt,decls);_LL331: {struct Cyc_Absyn_TagInt_p_struct*_tmp73F=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp739;if(_tmp73F->tag != 3)goto _LL333;else{
_tmp740=_tmp73F->f2;_tmp741=*_tmp740;_tmp742=_tmp741.name;}}_LL332: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp742,r);goto _LL334;_LL333: {struct Cyc_Absyn_Wild_p_struct*_tmp743=(
struct Cyc_Absyn_Wild_p_struct*)_tmp739;if(_tmp743->tag != 0)goto _LL335;}_LL334: s=
Cyc_Toc_skip_stmt_dl();goto _LL32E;_LL335: {struct Cyc_Absyn_Reference_p_struct*
_tmp744=(struct Cyc_Absyn_Reference_p_struct*)_tmp739;if(_tmp744->tag != 2)goto
_LL337;else{_tmp745=_tmp744->f1;_tmp746=_tmp744->f2;}}_LL336: {struct _tuple1*
_tmp76D=Cyc_Toc_temp_var();{struct _tuple22*_tmpCF4;struct Cyc_List_List*_tmpCF3;
decls=((_tmpCF3=_region_malloc(rgn,sizeof(*_tmpCF3)),((_tmpCF3->hd=((_tmpCF4=
_region_malloc(rgn,sizeof(*_tmpCF4)),((_tmpCF4->f1=_tmp76D,((_tmpCF4->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpCF4)))))),((_tmpCF3->tl=decls,_tmpCF3))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp745->name,Cyc_Absyn_var_exp(_tmp76D,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp76D,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp770=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp746,fail_stmt,decls);_tmp770.f3=Cyc_Absyn_seq_stmt(s,_tmp770.f3,
0);return _tmp770;}}_LL337: {struct Cyc_Absyn_Null_p_struct*_tmp747=(struct Cyc_Absyn_Null_p_struct*)
_tmp739;if(_tmp747->tag != 8)goto _LL339;}_LL338: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL32E;_LL339: {struct Cyc_Absyn_Int_p_struct*_tmp748=(struct
Cyc_Absyn_Int_p_struct*)_tmp739;if(_tmp748->tag != 9)goto _LL33B;else{_tmp749=
_tmp748->f1;_tmp74A=_tmp748->f2;}}_LL33A: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp749,_tmp74A,0),fail_stmt);goto _LL32E;_LL33B: {struct Cyc_Absyn_Char_p_struct*
_tmp74B=(struct Cyc_Absyn_Char_p_struct*)_tmp739;if(_tmp74B->tag != 10)goto _LL33D;
else{_tmp74C=_tmp74B->f1;}}_LL33C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp74C,0),fail_stmt);goto _LL32E;_LL33D: {struct Cyc_Absyn_Float_p_struct*_tmp74D=(
struct Cyc_Absyn_Float_p_struct*)_tmp739;if(_tmp74D->tag != 11)goto _LL33F;else{
_tmp74E=_tmp74D->f1;}}_LL33E: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp74E,
0),fail_stmt);goto _LL32E;_LL33F: {struct Cyc_Absyn_Enum_p_struct*_tmp74F=(struct
Cyc_Absyn_Enum_p_struct*)_tmp739;if(_tmp74F->tag != 12)goto _LL341;else{_tmp750=
_tmp74F->f1;_tmp751=_tmp74F->f2;}}_LL340:{struct Cyc_Absyn_Enum_e_struct _tmpCF7;
struct Cyc_Absyn_Enum_e_struct*_tmpCF6;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6[0]=((_tmpCF7.tag=33,((
_tmpCF7.f1=_tmp751->name,((_tmpCF7.f2=(struct Cyc_Absyn_Enumdecl*)_tmp750,((
_tmpCF7.f3=(struct Cyc_Absyn_Enumfield*)_tmp751,_tmpCF7)))))))),_tmpCF6)))),0),
fail_stmt);}goto _LL32E;_LL341: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp752=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp739;if(_tmp752->tag != 13)goto _LL343;else{
_tmp753=(void*)_tmp752->f1;_tmp754=_tmp752->f2;}}_LL342:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpCFA;struct Cyc_Absyn_AnonEnum_e_struct*_tmpCF9;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpCF9=_cycalloc(sizeof(*_tmpCF9)),((_tmpCF9[0]=((_tmpCFA.tag=34,((
_tmpCFA.f1=_tmp754->name,((_tmpCFA.f2=(void*)_tmp753,((_tmpCFA.f3=(struct Cyc_Absyn_Enumfield*)
_tmp754,_tmpCFA)))))))),_tmpCF9)))),0),fail_stmt);}goto _LL32E;_LL343: {struct Cyc_Absyn_Pointer_p_struct*
_tmp755=(struct Cyc_Absyn_Pointer_p_struct*)_tmp739;if(_tmp755->tag != 5)goto
_LL345;else{_tmp756=_tmp755->f1;_tmp757=*_tmp756;_tmp758=_tmp757.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp759=(struct Cyc_Absyn_Datatype_p_struct*)_tmp758;if(_tmp759->tag != 7)goto
_LL345;else{_tmp75A=_tmp759->f1;_tmp75B=_tmp759->f2;_tmp75C=_tmp759->f3;}}}}
_LL344: s=Cyc_Toc_skip_stmt_dl();{struct _tuple1*_tmp775=Cyc_Toc_temp_var();const
char*_tmpCFB;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp75B->name,((
_tmpCFB="_struct",_tag_dyneither(_tmpCFB,sizeof(char),8))));void*_tmp776=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp776,r);struct Cyc_List_List*_tmp777=_tmp75B->typs;for(0;_tmp75C != 0;(((
_tmp75C=_tmp75C->tl,_tmp777=((struct Cyc_List_List*)_check_null(_tmp777))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp778=(struct Cyc_Absyn_Pat*)_tmp75C->hd;if(_tmp778->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp779=(*((struct _tuple10*)((
struct Cyc_List_List*)_check_null(_tmp777))->hd)).f2;struct _tuple1*_tmp77A=Cyc_Toc_temp_var();
void*_tmp77B=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp778->topt))->v;void*
_tmp77C=Cyc_Toc_typ_to_c(_tmp77B);struct Cyc_Absyn_Exp*_tmp77D=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp775,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp779)))_tmp77D=Cyc_Toc_cast_it(_tmp77C,_tmp77D);{struct
_tuple22*_tmpCFE;struct Cyc_List_List*_tmpCFD;decls=((_tmpCFD=_region_malloc(rgn,
sizeof(*_tmpCFD)),((_tmpCFD->hd=((_tmpCFE=_region_malloc(rgn,sizeof(*_tmpCFE)),((
_tmpCFE->f1=_tmp77A,((_tmpCFE->f2=_tmp77C,_tmpCFE)))))),((_tmpCFD->tl=decls,
_tmpCFD))))));}{struct _tuple21 _tmp780=Cyc_Toc_xlate_pat(nv,rgn,_tmp77B,Cyc_Absyn_var_exp(
_tmp77A,0),_tmp77D,_tmp778,fail_stmt,decls);nv=_tmp780.f1;decls=_tmp780.f2;{
struct Cyc_Absyn_Stmt*_tmp781=_tmp780.f3;struct Cyc_Absyn_Stmt*_tmp782=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp77A,0),_tmp77D,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp782,_tmp781,0),0);}}}}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp775,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp75A->is_extensible?Cyc_Absyn_var_exp(_tmp75B->name,0): Cyc_Toc_datatype_tag(
_tmp75A,_tmp75B->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp775,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp775,_tmp776,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL32E;}}_LL345: {struct Cyc_Absyn_Datatype_p_struct*_tmp75D=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp739;if(_tmp75D->tag != 7)goto _LL347;else{
_tmp75E=_tmp75D->f3;}}_LL346: _tmp760=_tmp75E;goto _LL348;_LL347: {struct Cyc_Absyn_Tuple_p_struct*
_tmp75F=(struct Cyc_Absyn_Tuple_p_struct*)_tmp739;if(_tmp75F->tag != 4)goto _LL349;
else{_tmp760=_tmp75F->f1;}}_LL348: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp760 != 0;(_tmp760=_tmp760->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp784=(struct Cyc_Absyn_Pat*)
_tmp760->hd;if(_tmp784->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple1*
_tmp785=Cyc_Toc_temp_var();void*_tmp786=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp784->topt))->v;{struct _tuple22*_tmpD01;struct Cyc_List_List*_tmpD00;decls=((
_tmpD00=_region_malloc(rgn,sizeof(*_tmpD00)),((_tmpD00->hd=((_tmpD01=
_region_malloc(rgn,sizeof(*_tmpD01)),((_tmpD01->f1=_tmp785,((_tmpD01->f2=Cyc_Toc_typ_to_c(
_tmp786),_tmpD01)))))),((_tmpD00->tl=decls,_tmpD00))))));}{struct _tuple21 _tmp789=
Cyc_Toc_xlate_pat(nv,rgn,_tmp786,Cyc_Absyn_var_exp(_tmp785,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp784,fail_stmt,decls);nv=_tmp789.f1;decls=
_tmp789.f2;{struct Cyc_Absyn_Stmt*_tmp78A=_tmp789.f3;struct Cyc_Absyn_Stmt*_tmp78B=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp785,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp78B,
_tmp78A,0),0);}}}}goto _LL32E;}_LL349: {struct Cyc_Absyn_Aggr_p_struct*_tmp761=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp739;if(_tmp761->tag != 6)goto _LL34B;else{
_tmp762=_tmp761->f1;if(_tmp762 != 0)goto _LL34B;}}_LL34A: {const char*_tmpD04;void*
_tmpD03;(_tmpD03=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD04="unresolved aggregate pattern!",_tag_dyneither(_tmpD04,sizeof(char),30))),
_tag_dyneither(_tmpD03,sizeof(void*),0)));}_LL34B: {struct Cyc_Absyn_Aggr_p_struct*
_tmp763=(struct Cyc_Absyn_Aggr_p_struct*)_tmp739;if(_tmp763->tag != 6)goto _LL34D;
else{_tmp764=_tmp763->f1;if(_tmp764 == 0)goto _LL34D;_tmp765=*_tmp764;_tmp766=
_tmp765.aggr_info;_tmp767=_tmp763->f3;}}_LL34C: {struct Cyc_Absyn_Aggrdecl*
_tmp78E=Cyc_Absyn_get_known_aggrdecl(_tmp766);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp78E->impl))->tagged){struct _tuple23 _tmp790;struct Cyc_List_List*
_tmp791;struct Cyc_Absyn_Pat*_tmp792;struct _tuple23*_tmp78F=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp767))->hd;_tmp790=*_tmp78F;_tmp791=_tmp790.f1;
_tmp792=_tmp790.f2;{struct _dyneither_ptr*f;{void*_tmp793=(void*)((struct Cyc_List_List*)
_check_null(_tmp791))->hd;struct _dyneither_ptr*_tmp795;_LL356: {struct Cyc_Absyn_FieldName_struct*
_tmp794=(struct Cyc_Absyn_FieldName_struct*)_tmp793;if(_tmp794->tag != 1)goto
_LL358;else{_tmp795=_tmp794->f1;}}_LL357: f=_tmp795;goto _LL355;_LL358:;_LL359: {
const char*_tmpD07;void*_tmpD06;(_tmpD06=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD07="no field name in tagged union pattern",
_tag_dyneither(_tmpD07,sizeof(char),38))),_tag_dyneither(_tmpD06,sizeof(void*),0)));}
_LL355:;}{struct _tuple1*_tmp798=Cyc_Toc_temp_var();void*_tmp799=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp792->topt))->v;void*_tmp79A=Cyc_Toc_typ_to_c(
_tmp799);{struct _tuple22*_tmpD0A;struct Cyc_List_List*_tmpD09;decls=((_tmpD09=
_region_malloc(rgn,sizeof(*_tmpD09)),((_tmpD09->hd=((_tmpD0A=_region_malloc(rgn,
sizeof(*_tmpD0A)),((_tmpD0A->f1=_tmp798,((_tmpD0A->f2=_tmp79A,_tmpD0A)))))),((
_tmpD09->tl=decls,_tmpD09))))));}{struct Cyc_Absyn_Exp*_tmp79D=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp79E=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp79E=Cyc_Toc_cast_it(_tmp79A,_tmp79E);_tmp79D=Cyc_Toc_cast_it(_tmp79A,_tmp79D);{
struct _tuple21 _tmp79F=Cyc_Toc_xlate_pat(nv,rgn,_tmp799,Cyc_Absyn_var_exp(_tmp798,
0),_tmp79D,_tmp792,fail_stmt,decls);nv=_tmp79F.f1;decls=_tmp79F.f2;{struct Cyc_Absyn_Stmt*
_tmp7A0=_tmp79F.f3;struct Cyc_Absyn_Stmt*_tmp7A1=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp78E,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7A2=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp798,0),_tmp79E,0);s=Cyc_Absyn_seq_stmt(_tmp7A1,Cyc_Absyn_seq_stmt(_tmp7A2,
_tmp7A0,0),0);}}}}}}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp767 != 0;_tmp767=
_tmp767->tl){struct _tuple23*_tmp7A3=(struct _tuple23*)_tmp767->hd;struct Cyc_Absyn_Pat*
_tmp7A4=(*_tmp7A3).f2;if(_tmp7A4->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7A5=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7A3).f1))->hd;struct _dyneither_ptr*_tmp7A7;_LL35B: {struct Cyc_Absyn_FieldName_struct*
_tmp7A6=(struct Cyc_Absyn_FieldName_struct*)_tmp7A5;if(_tmp7A6->tag != 1)goto
_LL35D;else{_tmp7A7=_tmp7A6->f1;}}_LL35C: f=_tmp7A7;goto _LL35A;_LL35D:;_LL35E: {
struct Cyc_Toc_Match_error_struct _tmpD0D;struct Cyc_Toc_Match_error_struct*_tmpD0C;(
int)_throw((void*)((_tmpD0C=_cycalloc_atomic(sizeof(*_tmpD0C)),((_tmpD0C[0]=((
_tmpD0D.tag=Cyc_Toc_Match_error,_tmpD0D)),_tmpD0C)))));}_LL35A:;}{struct _tuple1*
_tmp7AA=Cyc_Toc_temp_var();void*_tmp7AB=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7A4->topt))->v;void*_tmp7AC=Cyc_Toc_typ_to_c(_tmp7AB);{struct _tuple22*
_tmpD10;struct Cyc_List_List*_tmpD0F;decls=((_tmpD0F=_region_malloc(rgn,sizeof(*
_tmpD0F)),((_tmpD0F->hd=((_tmpD10=_region_malloc(rgn,sizeof(*_tmpD10)),((_tmpD10->f1=
_tmp7AA,((_tmpD10->f2=_tmp7AC,_tmpD10)))))),((_tmpD0F->tl=decls,_tmpD0F))))));}{
struct _tuple21 _tmp7AF=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AB,Cyc_Absyn_var_exp(_tmp7AA,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7A4,fail_stmt,decls);nv=_tmp7AF.f1;
decls=_tmp7AF.f2;{struct Cyc_Absyn_Exp*_tmp7B0=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp78E->impl))->fields,f)))->type))
_tmp7B0=Cyc_Toc_cast_it(_tmp7AC,_tmp7B0);{struct Cyc_Absyn_Stmt*_tmp7B1=_tmp7AF.f3;
struct Cyc_Absyn_Stmt*_tmp7B2=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7AA,0),
_tmp7B0,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7B2,_tmp7B1,0),0);}}}}}}}
goto _LL32E;}_LL34D: {struct Cyc_Absyn_Pointer_p_struct*_tmp768=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp739;if(_tmp768->tag != 5)goto _LL34F;else{_tmp769=_tmp768->f1;}}_LL34E: {
struct _tuple1*_tmp7B3=Cyc_Toc_temp_var();void*_tmp7B4=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp769->topt))->v;{struct _tuple22*_tmpD13;struct Cyc_List_List*
_tmpD12;decls=((_tmpD12=_region_malloc(rgn,sizeof(*_tmpD12)),((_tmpD12->hd=((
_tmpD13=_region_malloc(rgn,sizeof(*_tmpD13)),((_tmpD13->f1=_tmp7B3,((_tmpD13->f2=
Cyc_Toc_typ_to_c(_tmp7B4),_tmpD13)))))),((_tmpD12->tl=decls,_tmpD12))))));}{
struct _tuple21 _tmp7B7=Cyc_Toc_xlate_pat(nv,rgn,_tmp7B4,Cyc_Absyn_var_exp(_tmp7B3,
0),Cyc_Absyn_deref_exp(path,0),_tmp769,fail_stmt,decls);nv=_tmp7B7.f1;decls=
_tmp7B7.f2;{struct Cyc_Absyn_Stmt*_tmp7B8=_tmp7B7.f3;struct Cyc_Absyn_Stmt*_tmp7B9=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B3,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7B8,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7B9,0);else{s=_tmp7B9;}goto _LL32E;}}}_LL34F: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp76A=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp739;if(_tmp76A->tag != 14)goto
_LL351;}_LL350: {const char*_tmpD16;void*_tmpD15;(_tmpD15=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD16="unknownid pat",
_tag_dyneither(_tmpD16,sizeof(char),14))),_tag_dyneither(_tmpD15,sizeof(void*),0)));}
_LL351: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp76B=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp739;if(_tmp76B->tag != 15)goto _LL353;}_LL352: {const char*_tmpD19;void*_tmpD18;(
_tmpD18=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD19="unknowncall pat",_tag_dyneither(_tmpD19,sizeof(char),16))),
_tag_dyneither(_tmpD18,sizeof(void*),0)));}_LL353: {struct Cyc_Absyn_Exp_p_struct*
_tmp76C=(struct Cyc_Absyn_Exp_p_struct*)_tmp739;if(_tmp76C->tag != 16)goto _LL32E;}
_LL354: {const char*_tmpD1C;void*_tmpD1B;(_tmpD1B=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1C="exp pat",
_tag_dyneither(_tmpD1C,sizeof(char),8))),_tag_dyneither(_tmpD1B,sizeof(void*),0)));}
_LL32E:;}{struct _tuple21 _tmpD1D;return(_tmpD1D.f1=nv,((_tmpD1D.f2=decls,((
_tmpD1D.f3=s,_tmpD1D)))));}}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD1E;
return(_tmpD1E=_region_malloc(r,sizeof(*_tmpD1E)),((_tmpD1E->f1=Cyc_Toc_fresh_label(),((
_tmpD1E->f2=Cyc_Toc_fresh_label(),((_tmpD1E->f3=sc,_tmpD1E)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs);static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7C2=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp7C3=Cyc_Tcutil_compress(_tmp7C2);_LL360: {struct Cyc_Absyn_IntType_struct*
_tmp7C4=(struct Cyc_Absyn_IntType_struct*)_tmp7C3;if(_tmp7C4->tag != 6)goto _LL362;}
_LL361: goto _LL363;_LL362: {struct Cyc_Absyn_EnumType_struct*_tmp7C5=(struct Cyc_Absyn_EnumType_struct*)
_tmp7C3;if(_tmp7C5->tag != 14)goto _LL364;}_LL363: leave_as_switch=1;goto _LL35F;
_LL364:;_LL365: leave_as_switch=0;goto _LL35F;_LL35F:;}{struct Cyc_List_List*
_tmp7C6=scs;for(0;_tmp7C6 != 0;_tmp7C6=_tmp7C6->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7C6->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7C6->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp7C7=scs;for(0;_tmp7C7 != 0;_tmp7C7=_tmp7C7->tl){struct Cyc_Absyn_Stmt*
_tmp7C8=((struct Cyc_Absyn_Switch_clause*)_tmp7C7->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7C7->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7C8,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7CA;struct _RegionHandle*_tmp7CB;struct Cyc_Toc_Env*_tmp7C9=
nv;_tmp7CA=*_tmp7C9;_tmp7CB=_tmp7CA.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7CB,nv,next_l),_tmp7C8);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7CD;
struct _RegionHandle*_tmp7CE;struct Cyc_Toc_Env*_tmp7CC=nv;_tmp7CD=*_tmp7CC;
_tmp7CE=_tmp7CD.rgn;{struct Cyc_Toc_Env*_tmp7CF=Cyc_Toc_share_env(_tmp7CE,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7CE,Cyc_Toc_gen_label,
_tmp7CE,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7D0=lscs;for(0;_tmp7D0 != 0;
_tmp7D0=_tmp7D0->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7D0->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7D0->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7D0->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7D2;struct Cyc_List_List*_tmp7D3;struct Cyc_Absyn_Stmt*_tmp7D4;struct _tuple21
_tmp7D1=Cyc_Toc_xlate_pat(_tmp7CF,_tmp7CE,_tmp7C2,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7D2=_tmp7D1.f1;_tmp7D3=_tmp7D1.f2;_tmp7D4=_tmp7D1.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7D5=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7D2,_tmp7D5);_tmp7D4=Cyc_Absyn_seq_stmt(
_tmp7D4,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7D5,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7D3;{
struct Cyc_List_List*_tmpD1F;nvs=((_tmpD1F=_region_malloc(_tmp7CE,sizeof(*_tmpD1F)),((
_tmpD1F->hd=_tmp7D2,((_tmpD1F->tl=nvs,_tmpD1F))))));}{struct Cyc_List_List*
_tmpD20;test_stmts=((_tmpD20=_region_malloc(_tmp7CE,sizeof(*_tmpD20)),((_tmpD20->hd=
_tmp7D4,((_tmpD20->tl=test_stmts,_tmpD20))))));}}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7D9;struct _dyneither_ptr*_tmp7DA;struct _dyneither_ptr*_tmp7DB;
struct Cyc_Absyn_Switch_clause*_tmp7DC;struct _tuple24*_tmp7D8=(struct _tuple24*)
lscs->hd;_tmp7D9=*_tmp7D8;_tmp7DA=_tmp7D9.f1;_tmp7DB=_tmp7D9.f2;_tmp7DC=_tmp7D9.f3;{
struct Cyc_Toc_Env*_tmp7DD=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7DC->body;struct Cyc_Toc_Env _tmp7DF;struct
_RegionHandle*_tmp7E0;struct Cyc_Toc_Env*_tmp7DE=_tmp7CF;_tmp7DF=*_tmp7DE;_tmp7E0=
_tmp7DF.rgn;if(lscs->tl != 0){struct _tuple24 _tmp7E2;struct _dyneither_ptr*_tmp7E3;
struct Cyc_Absyn_Switch_clause*_tmp7E4;struct _tuple24*_tmp7E1=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7E2=*_tmp7E1;_tmp7E3=_tmp7E2.f2;
_tmp7E4=_tmp7E2.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp7E0,
_tmp7DD,end_l,_tmp7E3,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7E4->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp7E0,
_tmp7DD,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7DA,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7DB,s,0),0);{struct Cyc_List_List*_tmpD21;stmts=((_tmpD21=_region_malloc(
_tmp7CE,sizeof(*_tmpD21)),((_tmpD21->hd=s,((_tmpD21->tl=stmts,_tmpD21))))));}}}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp7E7;struct _tuple1*_tmp7E8;void*_tmp7E9;struct _tuple22*_tmp7E6=(struct
_tuple22*)decls->hd;_tmp7E7=*_tmp7E6;_tmp7E8=_tmp7E7.f1;_tmp7E9=_tmp7E7.f2;res=
Cyc_Absyn_declare_stmt(_tmp7E8,_tmp7E9,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;}}}}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD22;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((
_tmpD22->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD22->tl=0,_tmpD22)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp7EB=s->r;struct Cyc_Absyn_Exp*_tmp7EE;struct
Cyc_Absyn_Stmt*_tmp7F0;struct Cyc_Absyn_Stmt*_tmp7F1;struct Cyc_Absyn_Exp*_tmp7F3;
struct Cyc_Absyn_Exp*_tmp7F5;struct Cyc_Absyn_Stmt*_tmp7F6;struct Cyc_Absyn_Stmt*
_tmp7F7;struct _tuple9 _tmp7F9;struct Cyc_Absyn_Exp*_tmp7FA;struct Cyc_Absyn_Stmt*
_tmp7FB;struct Cyc_Absyn_Stmt*_tmp7FD;struct Cyc_Absyn_Stmt*_tmp7FF;struct Cyc_Absyn_Stmt*
_tmp801;struct Cyc_Absyn_Exp*_tmp803;struct _tuple9 _tmp804;struct Cyc_Absyn_Exp*
_tmp805;struct _tuple9 _tmp806;struct Cyc_Absyn_Exp*_tmp807;struct Cyc_Absyn_Stmt*
_tmp808;struct Cyc_Absyn_Exp*_tmp80A;struct Cyc_List_List*_tmp80B;struct Cyc_List_List*
_tmp80D;struct Cyc_Absyn_Switch_clause**_tmp80E;struct Cyc_Absyn_Decl*_tmp810;
struct Cyc_Absyn_Stmt*_tmp811;struct _dyneither_ptr*_tmp813;struct Cyc_Absyn_Stmt*
_tmp814;struct Cyc_Absyn_Stmt*_tmp816;struct _tuple9 _tmp817;struct Cyc_Absyn_Exp*
_tmp818;struct Cyc_Absyn_Stmt*_tmp81A;struct Cyc_List_List*_tmp81B;struct Cyc_Absyn_Exp*
_tmp81D;_LL367: {struct Cyc_Absyn_Skip_s_struct*_tmp7EC=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7EB;if(_tmp7EC->tag != 0)goto _LL369;}_LL368: return;_LL369: {struct Cyc_Absyn_Exp_s_struct*
_tmp7ED=(struct Cyc_Absyn_Exp_s_struct*)_tmp7EB;if(_tmp7ED->tag != 1)goto _LL36B;
else{_tmp7EE=_tmp7ED->f1;}}_LL36A: Cyc_Toc_exp_to_c(nv,_tmp7EE);return;_LL36B: {
struct Cyc_Absyn_Seq_s_struct*_tmp7EF=(struct Cyc_Absyn_Seq_s_struct*)_tmp7EB;if(
_tmp7EF->tag != 2)goto _LL36D;else{_tmp7F0=_tmp7EF->f1;_tmp7F1=_tmp7EF->f2;}}
_LL36C: Cyc_Toc_stmt_to_c(nv,_tmp7F0);s=_tmp7F1;continue;_LL36D: {struct Cyc_Absyn_Return_s_struct*
_tmp7F2=(struct Cyc_Absyn_Return_s_struct*)_tmp7EB;if(_tmp7F2->tag != 3)goto _LL36F;
else{_tmp7F3=_tmp7F2->f1;}}_LL36E: {struct Cyc_Core_Opt*topt=0;if(_tmp7F3 != 0){{
struct Cyc_Core_Opt*_tmpD23;topt=((_tmpD23=_cycalloc(sizeof(*_tmpD23)),((_tmpD23->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F3->topt))->v),
_tmpD23))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp7F3);}if(s->try_depth
> 0){if(topt != 0){struct _tuple1*_tmp81F=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp820=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp81F,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp81F,(void*)topt->v,_tmp7F3,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp820,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL36F: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp7F4=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7EB;if(_tmp7F4->tag != 4)goto _LL371;
else{_tmp7F5=_tmp7F4->f1;_tmp7F6=_tmp7F4->f2;_tmp7F7=_tmp7F4->f3;}}_LL370: Cyc_Toc_exp_to_c(
nv,_tmp7F5);Cyc_Toc_stmt_to_c(nv,_tmp7F6);s=_tmp7F7;continue;_LL371: {struct Cyc_Absyn_While_s_struct*
_tmp7F8=(struct Cyc_Absyn_While_s_struct*)_tmp7EB;if(_tmp7F8->tag != 5)goto _LL373;
else{_tmp7F9=_tmp7F8->f1;_tmp7FA=_tmp7F9.f1;_tmp7FB=_tmp7F8->f2;}}_LL372: Cyc_Toc_exp_to_c(
nv,_tmp7FA);{struct Cyc_Toc_Env _tmp822;struct _RegionHandle*_tmp823;struct Cyc_Toc_Env*
_tmp821=nv;_tmp822=*_tmp821;_tmp823=_tmp822.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp823,nv),_tmp7FB);return;}_LL373: {struct Cyc_Absyn_Break_s_struct*_tmp7FC=(
struct Cyc_Absyn_Break_s_struct*)_tmp7EB;if(_tmp7FC->tag != 6)goto _LL375;else{
_tmp7FD=_tmp7FC->f1;}}_LL374: {struct Cyc_Toc_Env _tmp825;struct _dyneither_ptr**
_tmp826;struct Cyc_Toc_Env*_tmp824=nv;_tmp825=*_tmp824;_tmp826=_tmp825.break_lab;
if(_tmp826 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp826,0);{int dest_depth=_tmp7FD == 0?0:
_tmp7FD->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}
_LL375: {struct Cyc_Absyn_Continue_s_struct*_tmp7FE=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7EB;if(_tmp7FE->tag != 7)goto _LL377;else{_tmp7FF=_tmp7FE->f1;}}_LL376: {
struct Cyc_Toc_Env _tmp828;struct _dyneither_ptr**_tmp829;struct Cyc_Toc_Env*_tmp827=
nv;_tmp828=*_tmp827;_tmp829=_tmp828.continue_lab;if(_tmp829 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp829,0);_tmp801=_tmp7FF;goto _LL378;}_LL377: {struct Cyc_Absyn_Goto_s_struct*
_tmp800=(struct Cyc_Absyn_Goto_s_struct*)_tmp7EB;if(_tmp800->tag != 8)goto _LL379;
else{_tmp801=_tmp800->f2;}}_LL378: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp801))->try_depth,s);return;_LL379: {struct Cyc_Absyn_For_s_struct*
_tmp802=(struct Cyc_Absyn_For_s_struct*)_tmp7EB;if(_tmp802->tag != 9)goto _LL37B;
else{_tmp803=_tmp802->f1;_tmp804=_tmp802->f2;_tmp805=_tmp804.f1;_tmp806=_tmp802->f3;
_tmp807=_tmp806.f1;_tmp808=_tmp802->f4;}}_LL37A: Cyc_Toc_exp_to_c(nv,_tmp803);Cyc_Toc_exp_to_c(
nv,_tmp805);Cyc_Toc_exp_to_c(nv,_tmp807);{struct Cyc_Toc_Env _tmp82B;struct
_RegionHandle*_tmp82C;struct Cyc_Toc_Env*_tmp82A=nv;_tmp82B=*_tmp82A;_tmp82C=
_tmp82B.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp82C,nv),_tmp808);return;}
_LL37B: {struct Cyc_Absyn_Switch_s_struct*_tmp809=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7EB;if(_tmp809->tag != 10)goto _LL37D;else{_tmp80A=_tmp809->f1;_tmp80B=_tmp809->f2;}}
_LL37C: Cyc_Toc_xlate_switch(nv,s,_tmp80A,_tmp80B);return;_LL37D: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp80C=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7EB;if(_tmp80C->tag != 11)goto
_LL37F;else{_tmp80D=_tmp80C->f1;_tmp80E=_tmp80C->f2;}}_LL37E: {struct Cyc_Toc_Env
_tmp82E;struct Cyc_Toc_FallthruInfo*_tmp82F;struct Cyc_Toc_Env*_tmp82D=nv;_tmp82E=*
_tmp82D;_tmp82F=_tmp82E.fallthru_info;if(_tmp82F == 0){const char*_tmpD26;void*
_tmpD25;(_tmpD25=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD26="fallthru in unexpected place",_tag_dyneither(_tmpD26,sizeof(char),29))),
_tag_dyneither(_tmpD25,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp833;struct
Cyc_List_List*_tmp834;struct Cyc_Dict_Dict _tmp835;struct Cyc_Toc_FallthruInfo
_tmp832=*_tmp82F;_tmp833=_tmp832.label;_tmp834=_tmp832.binders;_tmp835=_tmp832.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp833,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp80E)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp836=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp834);struct Cyc_List_List*_tmp837=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp80D);for(0;_tmp836 != 0;(_tmp836=_tmp836->tl,
_tmp837=_tmp837->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp837))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp835,(
struct _tuple1*)_tmp836->hd),(struct Cyc_Absyn_Exp*)_tmp837->hd,0),s2,0);}s->r=s2->r;
return;}}}}_LL37F: {struct Cyc_Absyn_Decl_s_struct*_tmp80F=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7EB;if(_tmp80F->tag != 12)goto _LL381;else{_tmp810=_tmp80F->f1;_tmp811=_tmp80F->f2;}}
_LL380:{void*_tmp838=_tmp810->r;struct Cyc_Absyn_Vardecl*_tmp83A;struct Cyc_Absyn_Pat*
_tmp83C;struct Cyc_Absyn_Exp*_tmp83D;struct Cyc_List_List*_tmp83F;struct Cyc_Absyn_Fndecl*
_tmp841;struct Cyc_Absyn_Tvar*_tmp843;struct Cyc_Absyn_Vardecl*_tmp844;int _tmp845;
struct Cyc_Absyn_Exp*_tmp846;struct Cyc_Absyn_Exp*_tmp848;struct Cyc_Absyn_Tvar*
_tmp849;struct Cyc_Absyn_Vardecl*_tmp84A;_LL38A: {struct Cyc_Absyn_Var_d_struct*
_tmp839=(struct Cyc_Absyn_Var_d_struct*)_tmp838;if(_tmp839->tag != 0)goto _LL38C;
else{_tmp83A=_tmp839->f1;}}_LL38B: {struct Cyc_Toc_Env _tmp84C;struct _RegionHandle*
_tmp84D;struct Cyc_Toc_Env*_tmp84B=nv;_tmp84C=*_tmp84B;_tmp84D=_tmp84C.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD29;struct Cyc_Absyn_Local_b_struct*_tmpD28;
struct Cyc_Toc_Env*_tmp84E=Cyc_Toc_add_varmap(_tmp84D,nv,_tmp83A->name,Cyc_Absyn_varb_exp(
_tmp83A->name,(void*)((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28[0]=((
_tmpD29.tag=4,((_tmpD29.f1=_tmp83A,_tmpD29)))),_tmpD28)))),0));Cyc_Toc_local_decl_to_c(
_tmp84E,_tmp84E,_tmp83A,_tmp811);}goto _LL389;}_LL38C: {struct Cyc_Absyn_Let_d_struct*
_tmp83B=(struct Cyc_Absyn_Let_d_struct*)_tmp838;if(_tmp83B->tag != 2)goto _LL38E;
else{_tmp83C=_tmp83B->f1;_tmp83D=_tmp83B->f3;}}_LL38D:{void*_tmp851=_tmp83C->r;
struct Cyc_Absyn_Vardecl*_tmp853;struct Cyc_Absyn_Pat*_tmp854;struct Cyc_Absyn_Pat
_tmp855;void*_tmp856;_LL399: {struct Cyc_Absyn_Var_p_struct*_tmp852=(struct Cyc_Absyn_Var_p_struct*)
_tmp851;if(_tmp852->tag != 1)goto _LL39B;else{_tmp853=_tmp852->f1;_tmp854=_tmp852->f2;
_tmp855=*_tmp854;_tmp856=_tmp855.r;{struct Cyc_Absyn_Wild_p_struct*_tmp857=(
struct Cyc_Absyn_Wild_p_struct*)_tmp856;if(_tmp857->tag != 0)goto _LL39B;}}}_LL39A: {
struct _tuple1*old_name=_tmp853->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp853->name=new_name;_tmp853->initializer=(struct Cyc_Absyn_Exp*)_tmp83D;{
struct Cyc_Absyn_Var_d_struct _tmpD2C;struct Cyc_Absyn_Var_d_struct*_tmpD2B;_tmp810->r=(
void*)((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B[0]=((_tmpD2C.tag=0,((
_tmpD2C.f1=_tmp853,_tmpD2C)))),_tmpD2B))));}{struct Cyc_Toc_Env _tmp85B;struct
_RegionHandle*_tmp85C;struct Cyc_Toc_Env*_tmp85A=nv;_tmp85B=*_tmp85A;_tmp85C=
_tmp85B.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD2F;struct Cyc_Absyn_Local_b_struct*
_tmpD2E;struct Cyc_Toc_Env*_tmp85D=Cyc_Toc_add_varmap(_tmp85C,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD2E=_cycalloc(sizeof(*_tmpD2E)),((_tmpD2E[0]=((_tmpD2F.tag=
4,((_tmpD2F.f1=_tmp853,_tmpD2F)))),_tmpD2E)))),0));Cyc_Toc_local_decl_to_c(
_tmp85D,nv,_tmp853,_tmp811);}goto _LL398;}}_LL39B:;_LL39C: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp83C,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp83D->topt))->v,_tmp83D,
_tmp811))->r;goto _LL398;_LL398:;}goto _LL389;_LL38E: {struct Cyc_Absyn_Letv_d_struct*
_tmp83E=(struct Cyc_Absyn_Letv_d_struct*)_tmp838;if(_tmp83E->tag != 3)goto _LL390;
else{_tmp83F=_tmp83E->f1;}}_LL38F: {struct Cyc_List_List*_tmp860=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp83F);if(_tmp860 == 0){const char*_tmpD32;
void*_tmpD31;(_tmpD31=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD32="empty Letv_d",_tag_dyneither(_tmpD32,sizeof(char),
13))),_tag_dyneither(_tmpD31,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD35;struct Cyc_Absyn_Var_d_struct*_tmpD34;_tmp810->r=(void*)((_tmpD34=
_cycalloc(sizeof(*_tmpD34)),((_tmpD34[0]=((_tmpD35.tag=0,((_tmpD35.f1=(struct Cyc_Absyn_Vardecl*)
_tmp860->hd,_tmpD35)))),_tmpD34))));}_tmp860=_tmp860->tl;for(0;_tmp860 != 0;
_tmp860=_tmp860->tl){struct Cyc_Absyn_Var_d_struct _tmpD38;struct Cyc_Absyn_Var_d_struct*
_tmpD37;struct Cyc_Absyn_Decl*_tmp865=Cyc_Absyn_new_decl((void*)((_tmpD37=
_cycalloc(sizeof(*_tmpD37)),((_tmpD37[0]=((_tmpD38.tag=0,((_tmpD38.f1=(struct Cyc_Absyn_Vardecl*)
_tmp860->hd,_tmpD38)))),_tmpD37)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp865,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL389;}_LL390: {struct Cyc_Absyn_Fn_d_struct*
_tmp840=(struct Cyc_Absyn_Fn_d_struct*)_tmp838;if(_tmp840->tag != 1)goto _LL392;
else{_tmp841=_tmp840->f1;}}_LL391: {struct _tuple1*_tmp868=_tmp841->name;struct
Cyc_Toc_Env _tmp86A;struct _RegionHandle*_tmp86B;struct Cyc_Toc_Env*_tmp869=nv;
_tmp86A=*_tmp869;_tmp86B=_tmp86A.rgn;{struct Cyc_Toc_Env*_tmp86C=Cyc_Toc_add_varmap(
_tmp86B,nv,_tmp841->name,Cyc_Absyn_var_exp(_tmp868,0));Cyc_Toc_fndecl_to_c(
_tmp86C,_tmp841,0);Cyc_Toc_stmt_to_c(_tmp86C,_tmp811);}goto _LL389;}_LL392: {
struct Cyc_Absyn_Region_d_struct*_tmp842=(struct Cyc_Absyn_Region_d_struct*)
_tmp838;if(_tmp842->tag != 4)goto _LL394;else{_tmp843=_tmp842->f1;_tmp844=_tmp842->f2;
_tmp845=_tmp842->f3;_tmp846=_tmp842->f4;}}_LL393: {struct Cyc_Absyn_Stmt*_tmp86D=
_tmp811;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*
rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp844->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp86F;struct _RegionHandle*_tmp870;struct Cyc_Toc_Env*
_tmp86E=nv;_tmp86F=*_tmp86E;_tmp870=_tmp86F.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp870,nv,x_var,x_exp),_tmp86D);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp86D,0))->r;
else{if(_tmp846 == 0){struct Cyc_Absyn_Exp*_tmpD3B[1];struct Cyc_Absyn_Exp*_tmpD3A[
1];struct Cyc_List_List*_tmpD39;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD39=
_cycalloc(sizeof(*_tmpD39)),((_tmpD39->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD39->tl=0,_tmpD39)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD3A[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3A,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp86D,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD3B[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3B,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp846);{struct Cyc_Absyn_Exp*
_tmpD3D[1];struct Cyc_Absyn_Exp*_tmpD3C[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD3C[1]=(struct Cyc_Absyn_Exp*)_tmp846,((_tmpD3C[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp86D,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD3D[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3D,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;}}}
return;}_LL394: {struct Cyc_Absyn_Alias_d_struct*_tmp847=(struct Cyc_Absyn_Alias_d_struct*)
_tmp838;if(_tmp847->tag != 5)goto _LL396;else{_tmp848=_tmp847->f1;_tmp849=_tmp847->f2;
_tmp84A=_tmp847->f3;}}_LL395: {struct _tuple1*old_name=_tmp84A->name;struct
_tuple1*new_name=Cyc_Toc_temp_var();_tmp84A->name=new_name;_tmp84A->initializer=(
struct Cyc_Absyn_Exp*)_tmp848;{struct Cyc_Absyn_Decl_s_struct _tmpD4C;struct Cyc_Absyn_Var_d_struct*
_tmpD4B;struct Cyc_Absyn_Var_d_struct _tmpD4A;struct Cyc_Absyn_Decl*_tmpD49;struct
Cyc_Absyn_Decl_s_struct*_tmpD48;s->r=(void*)((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((
_tmpD48[0]=((_tmpD4C.tag=12,((_tmpD4C.f1=((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((
_tmpD49->r=(void*)((_tmpD4B=_cycalloc(sizeof(*_tmpD4B)),((_tmpD4B[0]=((_tmpD4A.tag=
0,((_tmpD4A.f1=_tmp84A,_tmpD4A)))),_tmpD4B)))),((_tmpD49->loc=0,_tmpD49)))))),((
_tmpD4C.f2=_tmp811,_tmpD4C)))))),_tmpD48))));}{struct Cyc_Toc_Env _tmp87C;struct
_RegionHandle*_tmp87D;struct Cyc_Toc_Env*_tmp87B=nv;_tmp87C=*_tmp87B;_tmp87D=
_tmp87C.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD4F;struct Cyc_Absyn_Local_b_struct*
_tmpD4E;struct Cyc_Toc_Env*_tmp87E=Cyc_Toc_add_varmap(_tmp87D,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((_tmpD4E[0]=((_tmpD4F.tag=
4,((_tmpD4F.f1=_tmp84A,_tmpD4F)))),_tmpD4E)))),0));Cyc_Toc_local_decl_to_c(
_tmp87E,nv,_tmp84A,_tmp811);}return;}}_LL396:;_LL397: {const char*_tmpD52;void*
_tmpD51;(_tmpD51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD52="bad nested declaration within function",_tag_dyneither(_tmpD52,sizeof(
char),39))),_tag_dyneither(_tmpD51,sizeof(void*),0)));}_LL389:;}return;_LL381: {
struct Cyc_Absyn_Label_s_struct*_tmp812=(struct Cyc_Absyn_Label_s_struct*)_tmp7EB;
if(_tmp812->tag != 13)goto _LL383;else{_tmp813=_tmp812->f1;_tmp814=_tmp812->f2;}}
_LL382: s=_tmp814;continue;_LL383: {struct Cyc_Absyn_Do_s_struct*_tmp815=(struct
Cyc_Absyn_Do_s_struct*)_tmp7EB;if(_tmp815->tag != 14)goto _LL385;else{_tmp816=
_tmp815->f1;_tmp817=_tmp815->f2;_tmp818=_tmp817.f1;}}_LL384: {struct Cyc_Toc_Env
_tmp884;struct _RegionHandle*_tmp885;struct Cyc_Toc_Env*_tmp883=nv;_tmp884=*
_tmp883;_tmp885=_tmp884.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp885,nv),
_tmp816);Cyc_Toc_exp_to_c(nv,_tmp818);return;}_LL385: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp819=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7EB;if(_tmp819->tag != 15)goto
_LL387;else{_tmp81A=_tmp819->f1;_tmp81B=_tmp819->f2;}}_LL386: {struct _tuple1*
h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();struct _tuple1*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD53;
e_exp->topt=((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53->v=e_typ,_tmpD53))));}{
struct Cyc_Toc_Env _tmp888;struct _RegionHandle*_tmp889;struct Cyc_Toc_Env*_tmp887=
nv;_tmp888=*_tmp887;_tmp889=_tmp888.rgn;{struct Cyc_Toc_Env*_tmp88A=Cyc_Toc_add_varmap(
_tmp889,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp88A,_tmp81A);{struct Cyc_Absyn_Stmt*
_tmp88B=Cyc_Absyn_seq_stmt(_tmp81A,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp88C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp88D=
Cyc_Absyn_var_exp(_tmp88C,0);struct Cyc_Absyn_Vardecl*_tmp88E=Cyc_Absyn_new_vardecl(
_tmp88C,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD54;_tmp88D->topt=((
_tmpD54=_cycalloc(sizeof(*_tmpD54)),((_tmpD54->v=Cyc_Absyn_exn_typ(),_tmpD54))));}{
struct Cyc_Absyn_Var_p_struct*_tmpD65;struct Cyc_Absyn_Pat*_tmpD64;struct Cyc_Core_Opt*
_tmpD63;struct Cyc_Absyn_Var_p_struct _tmpD62;struct Cyc_Core_Opt*_tmpD61;struct Cyc_Absyn_Pat*
_tmpD60;struct Cyc_Absyn_Pat*_tmp890=(_tmpD60=_cycalloc(sizeof(*_tmpD60)),((
_tmpD60->r=(void*)((_tmpD65=_cycalloc(sizeof(*_tmpD65)),((_tmpD65[0]=((_tmpD62.tag=
1,((_tmpD62.f1=_tmp88E,((_tmpD62.f2=((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((
_tmpD64->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD64->topt=((_tmpD63=_cycalloc(
sizeof(*_tmpD63)),((_tmpD63->v=Cyc_Absyn_exn_typ(),_tmpD63)))),((_tmpD64->loc=0,
_tmpD64)))))))),_tmpD62)))))),_tmpD65)))),((_tmpD60->topt=((_tmpD61=_cycalloc(
sizeof(*_tmpD61)),((_tmpD61->v=Cyc_Absyn_exn_typ(),_tmpD61)))),((_tmpD60->loc=0,
_tmpD60)))))));struct Cyc_Absyn_Exp*_tmp891=Cyc_Absyn_throw_exp(_tmp88D,0);{
struct Cyc_Core_Opt*_tmpD66;_tmp891->topt=((_tmpD66=_cycalloc(sizeof(*_tmpD66)),((
_tmpD66->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD66))));}{struct Cyc_Absyn_Stmt*
_tmp893=Cyc_Absyn_exp_stmt(_tmp891,0);struct Cyc_Core_Opt*_tmpD6C;struct Cyc_List_List*
_tmpD6B;struct Cyc_Absyn_Switch_clause*_tmpD6A;struct Cyc_Absyn_Switch_clause*
_tmp894=(_tmpD6A=_cycalloc(sizeof(*_tmpD6A)),((_tmpD6A->pattern=_tmp890,((
_tmpD6A->pat_vars=((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C->v=((_tmpD6B=
_cycalloc(sizeof(*_tmpD6B)),((_tmpD6B->hd=_tmp88E,((_tmpD6B->tl=0,_tmpD6B)))))),
_tmpD6C)))),((_tmpD6A->where_clause=0,((_tmpD6A->body=_tmp893,((_tmpD6A->loc=0,
_tmpD6A)))))))))));struct Cyc_List_List*_tmpD6D;struct Cyc_Absyn_Stmt*_tmp895=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp81B,((_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D->hd=_tmp894,((_tmpD6D->tl=
0,_tmpD6D))))))),0);Cyc_Toc_stmt_to_c(_tmp88A,_tmp895);{struct Cyc_List_List*
_tmpD6E;struct Cyc_Absyn_Exp*_tmp896=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD6E=_cycalloc(sizeof(*_tmpD6E)),((_tmpD6E->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD6E->tl=0,_tmpD6E)))))),0);struct Cyc_List_List*
_tmpD6F;struct Cyc_Absyn_Stmt*_tmp897=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD6F->tl=0,_tmpD6F)))))),0),0);struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp899=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp897,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp898,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp896,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp899,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp88B,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp895,0),0),0),0),0),0))->r;}}}}}
return;}}_LL387: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp81C=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7EB;if(_tmp81C->tag != 16)goto _LL366;else{_tmp81D=_tmp81C->f1;}}_LL388: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp81D);{struct Cyc_List_List*
_tmpD70;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70->hd=_tmp81D,((_tmpD70->tl=0,
_tmpD70)))))),0));}}return;_LL366:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8A7=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8A7;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8A8=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8A9=f->args;for(0;_tmp8A9 != 0;_tmp8A9=_tmp8A9->tl){struct
_tuple1*_tmpD71;struct _tuple1*_tmp8AA=(_tmpD71=_cycalloc(sizeof(*_tmpD71)),((
_tmpD71->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD71->f2=(*((struct
_tuple25*)_tmp8A9->hd)).f1,_tmpD71)))));(*((struct _tuple25*)_tmp8A9->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8A9->hd)).f3);_tmp8A8=Cyc_Toc_add_varmap(frgn,_tmp8A8,
_tmp8AA,Cyc_Absyn_var_exp(_tmp8AA,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8AD;struct Cyc_Absyn_Tqual
_tmp8AE;void*_tmp8AF;int _tmp8B0;struct Cyc_Absyn_VarargInfo _tmp8AC=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8AD=_tmp8AC.name;_tmp8AE=_tmp8AC.tq;_tmp8AF=
_tmp8AC.type;_tmp8B0=_tmp8AC.inject;{void*_tmp8B1=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8AF,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8AE,Cyc_Absyn_false_conref));struct
_tuple1*_tmpD72;struct _tuple1*_tmp8B2=(_tmpD72=_cycalloc(sizeof(*_tmpD72)),((
_tmpD72->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD72->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8AD))->v,_tmpD72)))));{
struct _tuple25*_tmpD75;struct Cyc_List_List*_tmpD74;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD74=
_cycalloc(sizeof(*_tmpD74)),((_tmpD74->hd=((_tmpD75=_cycalloc(sizeof(*_tmpD75)),((
_tmpD75->f1=(struct _dyneither_ptr*)_tmp8AD->v,((_tmpD75->f2=_tmp8AE,((_tmpD75->f3=
_tmp8B1,_tmpD75)))))))),((_tmpD74->tl=0,_tmpD74)))))));}_tmp8A8=Cyc_Toc_add_varmap(
frgn,_tmp8A8,_tmp8B2,Cyc_Absyn_var_exp(_tmp8B2,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp8B6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8B6 != 0;_tmp8B6=_tmp8B6->tl){((struct Cyc_Absyn_Vardecl*)_tmp8B6->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8B6->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8A8),f->body);};_pop_region(frgn);}}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s);static enum Cyc_Absyn_Scope
Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL39D: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL39E: return Cyc_Absyn_Extern;
default: _LL39F: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp8B7=ad->name;struct _DynRegionHandle*_tmp8B9;struct Cyc_Dict_Dict*_tmp8BA;
struct Cyc_Toc_TocState _tmp8B8=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8B9=_tmp8B8.dyn;_tmp8BA=_tmp8B8.aggrs_so_far;{struct _DynRegionFrame _tmp8BB;
struct _RegionHandle*d=_open_dynregion(& _tmp8BB,_tmp8B9);{int seen_defn_before;
struct _tuple15**_tmp8BC=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*
k))Cyc_Dict_lookup_opt)(*_tmp8BA,_tmp8B7);if(_tmp8BC == 0){seen_defn_before=0;{
struct _tuple15*v;if(ad->kind == Cyc_Absyn_StructA){struct _tuple15*_tmpD76;v=((
_tmpD76=_region_malloc(d,sizeof(*_tmpD76)),((_tmpD76->f1=ad,((_tmpD76->f2=Cyc_Absyn_strctq(
_tmp8B7),_tmpD76))))));}else{struct _tuple15*_tmpD77;v=((_tmpD77=_region_malloc(d,
sizeof(*_tmpD77)),((_tmpD77->f1=ad,((_tmpD77->f2=Cyc_Absyn_unionq_typ(_tmp8B7),
_tmpD77))))));}*_tmp8BA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp8BA,_tmp8B7,v);}}else{struct
_tuple15 _tmp8C0;struct Cyc_Absyn_Aggrdecl*_tmp8C1;void*_tmp8C2;struct _tuple15*
_tmp8BF=*_tmp8BC;_tmp8C0=*_tmp8BF;_tmp8C1=_tmp8C0.f1;_tmp8C2=_tmp8C0.f2;if(
_tmp8C1->impl == 0){{struct _tuple15*_tmpD78;*_tmp8BA=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp8BA,
_tmp8B7,((_tmpD78=_region_malloc(d,sizeof(*_tmpD78)),((_tmpD78->f1=ad,((_tmpD78->f2=
_tmp8C2,_tmpD78)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpD79;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD79=_cycalloc(sizeof(*_tmpD79)),((
_tmpD79->kind=ad->kind,((_tmpD79->sc=Cyc_Absyn_Public,((_tmpD79->name=ad->name,((
_tmpD79->tvs=0,((_tmpD79->impl=0,((_tmpD79->attributes=ad->attributes,_tmpD79)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD7A;
new_ad->impl=((_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A->exist_vars=0,((
_tmpD7A->rgn_po=0,((_tmpD7A->fields=0,((_tmpD7A->tagged=0,_tmpD7A))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp8C5=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8C5 != 0;_tmp8C5=_tmp8C5->tl){struct Cyc_Absyn_Aggrfield*
_tmp8C6=(struct Cyc_Absyn_Aggrfield*)_tmp8C5->hd;struct Cyc_Absyn_Aggrfield*
_tmpD7B;struct Cyc_Absyn_Aggrfield*_tmp8C7=(_tmpD7B=_cycalloc(sizeof(*_tmpD7B)),((
_tmpD7B->name=_tmp8C6->name,((_tmpD7B->tq=Cyc_Toc_mt_tq,((_tmpD7B->type=Cyc_Toc_typ_to_c_array(
_tmp8C6->type),((_tmpD7B->width=_tmp8C6->width,((_tmpD7B->attributes=_tmp8C6->attributes,
_tmpD7B)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8C8=_tmp8C7->type;struct _dyneither_ptr*_tmp8C9=_tmp8C7->name;const char*
_tmpD80;void*_tmpD7F[2];struct Cyc_String_pa_struct _tmpD7E;struct Cyc_String_pa_struct
_tmpD7D;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD7D.tag=0,((_tmpD7D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8C9),((_tmpD7E.tag=0,((_tmpD7E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD7F[0]=&
_tmpD7E,((_tmpD7F[1]=& _tmpD7D,Cyc_aprintf(((_tmpD80="_union_%s_%s",
_tag_dyneither(_tmpD80,sizeof(char),13))),_tag_dyneither(_tmpD7F,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD81;struct _dyneither_ptr*str=(_tmpD81=_cycalloc(sizeof(*
_tmpD81)),((_tmpD81[0]=s,_tmpD81)));struct Cyc_Absyn_Aggrfield*_tmpD82;struct Cyc_Absyn_Aggrfield*
_tmp8CA=(_tmpD82=_cycalloc(sizeof(*_tmpD82)),((_tmpD82->name=Cyc_Toc_val_sp,((
_tmpD82->tq=Cyc_Toc_mt_tq,((_tmpD82->type=_tmp8C8,((_tmpD82->width=0,((_tmpD82->attributes=
0,_tmpD82)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD83;struct Cyc_Absyn_Aggrfield*
_tmp8CB=(_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83->name=Cyc_Toc_tag_sp,((
_tmpD83->tq=Cyc_Toc_mt_tq,((_tmpD83->type=Cyc_Absyn_sint_typ,((_tmpD83->width=0,((
_tmpD83->attributes=0,_tmpD83)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD84[2];
struct Cyc_List_List*_tmp8CC=(_tmpD84[1]=_tmp8CA,((_tmpD84[0]=_tmp8CB,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD84,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct _tuple1*_tmpD89;struct Cyc_Absyn_AggrdeclImpl*_tmpD88;struct Cyc_Absyn_Aggrdecl*
_tmpD87;struct Cyc_Absyn_Aggrdecl*_tmp8CD=(_tmpD87=_cycalloc(sizeof(*_tmpD87)),((
_tmpD87->kind=Cyc_Absyn_StructA,((_tmpD87->sc=Cyc_Absyn_Public,((_tmpD87->name=((
_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89->f1=Cyc_Absyn_Loc_n,((_tmpD89->f2=
str,_tmpD89)))))),((_tmpD87->tvs=0,((_tmpD87->impl=((_tmpD88=_cycalloc(sizeof(*
_tmpD88)),((_tmpD88->exist_vars=0,((_tmpD88->rgn_po=0,((_tmpD88->fields=_tmp8CC,((
_tmpD88->tagged=0,_tmpD88)))))))))),((_tmpD87->attributes=0,_tmpD87)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpD8F;struct Cyc_Absyn_Aggr_d_struct _tmpD8E;
struct Cyc_List_List*_tmpD8D;Cyc_Toc_result_decls=((_tmpD8D=_cycalloc(sizeof(*
_tmpD8D)),((_tmpD8D->hd=Cyc_Absyn_new_decl((void*)((_tmpD8F=_cycalloc(sizeof(*
_tmpD8F)),((_tmpD8F[0]=((_tmpD8E.tag=6,((_tmpD8E.f1=_tmp8CD,_tmpD8E)))),_tmpD8F)))),
0),((_tmpD8D->tl=Cyc_Toc_result_decls,_tmpD8D))))));}_tmp8C7->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpD90;new_fields=((_tmpD90=_cycalloc(sizeof(*
_tmpD90)),((_tmpD90->hd=_tmp8C7,((_tmpD90->tl=new_fields,_tmpD90))))));}}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);}}{struct Cyc_Absyn_Decl*_tmpD9A;struct Cyc_Absyn_Aggr_d_struct _tmpD99;
struct Cyc_Absyn_Aggr_d_struct*_tmpD98;struct Cyc_List_List*_tmpD97;Cyc_Toc_result_decls=((
_tmpD97=_cycalloc(sizeof(*_tmpD97)),((_tmpD97->hd=((_tmpD9A=_cycalloc(sizeof(*
_tmpD9A)),((_tmpD9A->r=(void*)((_tmpD98=_cycalloc(sizeof(*_tmpD98)),((_tmpD98[0]=((
_tmpD99.tag=6,((_tmpD99.f1=new_ad,_tmpD99)))),_tmpD98)))),((_tmpD9A->loc=0,
_tmpD9A)))))),((_tmpD97->tl=Cyc_Toc_result_decls,_tmpD97))))));}}};
_pop_dynregion(d);}}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8E4;struct Cyc_Set_Set**_tmp8E5;struct Cyc_Toc_TocState
_tmp8E3=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8E4=
_tmp8E3.dyn;_tmp8E5=_tmp8E3.datatypes_so_far;{struct _DynRegionFrame _tmp8E6;
struct _RegionHandle*d=_open_dynregion(& _tmp8E6,_tmp8E4);{struct _tuple1*_tmp8E7=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))
Cyc_Set_member)(*_tmp8E5,_tmp8E7)){_npop_handler(0);return;}*_tmp8E5=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp8E5,_tmp8E7);}{struct Cyc_List_List*_tmp8E8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp8E8 != 0;_tmp8E8=_tmp8E8->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8E8->hd;struct Cyc_List_List*_tmp8E9=0;int i=
1;{struct Cyc_List_List*_tmp8EA=f->typs;for(0;_tmp8EA != 0;(_tmp8EA=_tmp8EA->tl,i
++)){struct _dyneither_ptr*_tmp8EB=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpD9B;struct Cyc_Absyn_Aggrfield*_tmp8EC=(_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((
_tmpD9B->name=_tmp8EB,((_tmpD9B->tq=(*((struct _tuple10*)_tmp8EA->hd)).f1,((
_tmpD9B->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp8EA->hd)).f2),((
_tmpD9B->width=0,((_tmpD9B->attributes=0,_tmpD9B)))))))))));struct Cyc_List_List*
_tmpD9C;_tmp8E9=((_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C->hd=_tmp8EC,((
_tmpD9C->tl=_tmp8E9,_tmpD9C))))));}}{struct Cyc_Absyn_Aggrfield*_tmpD9F;struct Cyc_List_List*
_tmpD9E;_tmp8E9=((_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->hd=((_tmpD9F=
_cycalloc(sizeof(*_tmpD9F)),((_tmpD9F->name=Cyc_Toc_tag_sp,((_tmpD9F->tq=Cyc_Toc_mt_tq,((
_tmpD9F->type=Cyc_Absyn_sint_typ,((_tmpD9F->width=0,((_tmpD9F->attributes=0,
_tmpD9F)))))))))))),((_tmpD9E->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp8E9),_tmpD9E))))));}{const char*_tmpDA4;struct Cyc_Absyn_AggrdeclImpl*
_tmpDA3;struct Cyc_Absyn_Aggrdecl*_tmpDA2;struct Cyc_Absyn_Aggrdecl*_tmp8F1=(
_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2->kind=Cyc_Absyn_StructA,((_tmpDA2->sc=
Cyc_Absyn_Public,((_tmpDA2->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDA4="_struct",
_tag_dyneither(_tmpDA4,sizeof(char),8)))),((_tmpDA2->tvs=0,((_tmpDA2->impl=((
_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3->exist_vars=0,((_tmpDA3->rgn_po=0,((
_tmpDA3->fields=_tmp8E9,((_tmpDA3->tagged=0,_tmpDA3)))))))))),((_tmpDA2->attributes=
0,_tmpDA2)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDAA;struct Cyc_Absyn_Aggr_d_struct
_tmpDA9;struct Cyc_List_List*_tmpDA8;Cyc_Toc_result_decls=((_tmpDA8=_cycalloc(
sizeof(*_tmpDA8)),((_tmpDA8->hd=Cyc_Absyn_new_decl((void*)((_tmpDAA=_cycalloc(
sizeof(*_tmpDAA)),((_tmpDAA[0]=((_tmpDA9.tag=6,((_tmpDA9.f1=_tmp8F1,_tmpDA9)))),
_tmpDAA)))),0),((_tmpDA8->tl=Cyc_Toc_result_decls,_tmpDA8))))));}}};
_pop_dynregion(d);}}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp8F9;struct Cyc_Dict_Dict*_tmp8FA;struct
Cyc_Toc_TocState _tmp8F8=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8F9=_tmp8F8.dyn;_tmp8FA=_tmp8F8.xdatatypes_so_far;{struct _DynRegionFrame
_tmp8FB;struct _RegionHandle*d=_open_dynregion(& _tmp8FB,_tmp8F9);{struct _tuple1*
_tmp8FC=xd->name;struct Cyc_List_List*_tmp8FD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp8FD != 0;_tmp8FD=_tmp8FD->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8FD->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp8FE=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp8FF=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp8FE,Cyc_Absyn_false_conref,0);int*_tmp900=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8FA,f->name);int
_tmp901;_LL3A2: if(_tmp900 != 0)goto _LL3A4;_LL3A3: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp902=Cyc_Absyn_new_vardecl(f->name,_tmp8FF,
initopt);_tmp902->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDB0;struct Cyc_Absyn_Var_d_struct
_tmpDAF;struct Cyc_List_List*_tmpDAE;Cyc_Toc_result_decls=((_tmpDAE=_cycalloc(
sizeof(*_tmpDAE)),((_tmpDAE->hd=Cyc_Absyn_new_decl((void*)((_tmpDB0=_cycalloc(
sizeof(*_tmpDB0)),((_tmpDB0[0]=((_tmpDAF.tag=0,((_tmpDAF.f1=_tmp902,_tmpDAF)))),
_tmpDB0)))),0),((_tmpDAE->tl=Cyc_Toc_result_decls,_tmpDAE))))));}*_tmp8FA=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp8FA,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp906=f->typs;for(0;_tmp906 != 0;(_tmp906=_tmp906->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDB8;void*_tmpDB7[1];const char*_tmpDB6;struct
_dyneither_ptr*_tmpDB5;struct _dyneither_ptr*_tmp907=(_tmpDB5=_cycalloc(sizeof(*
_tmpDB5)),((_tmpDB5[0]=(struct _dyneither_ptr)((_tmpDB8.tag=1,((_tmpDB8.f1=(
unsigned long)i,((_tmpDB7[0]=& _tmpDB8,Cyc_aprintf(((_tmpDB6="f%d",_tag_dyneither(
_tmpDB6,sizeof(char),4))),_tag_dyneither(_tmpDB7,sizeof(void*),1)))))))),_tmpDB5)));
struct Cyc_Absyn_Aggrfield*_tmpDB9;struct Cyc_Absyn_Aggrfield*_tmp908=(_tmpDB9=
_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->name=_tmp907,((_tmpDB9->tq=(*((struct
_tuple10*)_tmp906->hd)).f1,((_tmpDB9->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple10*)_tmp906->hd)).f2),((_tmpDB9->width=0,((_tmpDB9->attributes=0,_tmpDB9)))))))))));
struct Cyc_List_List*_tmpDBA;fields=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((
_tmpDBA->hd=_tmp908,((_tmpDBA->tl=fields,_tmpDBA))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDBD;struct Cyc_List_List*_tmpDBC;fields=((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((
_tmpDBC->hd=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->name=Cyc_Toc_tag_sp,((
_tmpDBD->tq=Cyc_Toc_mt_tq,((_tmpDBD->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDBD->width=0,((_tmpDBD->attributes=0,_tmpDBD)))))))))))),((
_tmpDBC->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDBC))))));}{const char*_tmpDC2;struct Cyc_Absyn_AggrdeclImpl*_tmpDC1;
struct Cyc_Absyn_Aggrdecl*_tmpDC0;struct Cyc_Absyn_Aggrdecl*_tmp911=(_tmpDC0=
_cycalloc(sizeof(*_tmpDC0)),((_tmpDC0->kind=Cyc_Absyn_StructA,((_tmpDC0->sc=Cyc_Absyn_Public,((
_tmpDC0->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDC2="_struct",
_tag_dyneither(_tmpDC2,sizeof(char),8)))),((_tmpDC0->tvs=0,((_tmpDC0->impl=((
_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->exist_vars=0,((_tmpDC1->rgn_po=0,((
_tmpDC1->fields=fields,((_tmpDC1->tagged=0,_tmpDC1)))))))))),((_tmpDC0->attributes=
0,_tmpDC0)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDC8;struct Cyc_Absyn_Aggr_d_struct
_tmpDC7;struct Cyc_List_List*_tmpDC6;Cyc_Toc_result_decls=((_tmpDC6=_cycalloc(
sizeof(*_tmpDC6)),((_tmpDC6->hd=Cyc_Absyn_new_decl((void*)((_tmpDC8=_cycalloc(
sizeof(*_tmpDC8)),((_tmpDC8[0]=((_tmpDC7.tag=6,((_tmpDC7.f1=_tmp911,_tmpDC7)))),
_tmpDC8)))),0),((_tmpDC6->tl=Cyc_Toc_result_decls,_tmpDC6))))));}goto _LL3A1;}}}}
_LL3A4: if(_tmp900 == 0)goto _LL3A6;_tmp901=*_tmp900;if(_tmp901 != 0)goto _LL3A6;
_LL3A5: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp918=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp919=Cyc_Absyn_new_vardecl(f->name,_tmp8FF,(
struct Cyc_Absyn_Exp*)_tmp918);_tmp919->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDCE;struct Cyc_Absyn_Var_d_struct _tmpDCD;struct Cyc_List_List*_tmpDCC;Cyc_Toc_result_decls=((
_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC->hd=Cyc_Absyn_new_decl((void*)((
_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE[0]=((_tmpDCD.tag=0,((_tmpDCD.f1=
_tmp919,_tmpDCD)))),_tmpDCE)))),0),((_tmpDCC->tl=Cyc_Toc_result_decls,_tmpDCC))))));}*
_tmp8FA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp8FA,f->name,1);}goto _LL3A1;_LL3A6:;_LL3A7: goto _LL3A1;_LL3A1:;}};
_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Enumdecl*ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp91D=init->r;struct Cyc_Absyn_Vardecl*_tmp91F;struct Cyc_Absyn_Exp*_tmp920;
struct Cyc_Absyn_Exp*_tmp921;int _tmp922;_LL3A9: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp91E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp91D;if(_tmp91E->tag != 29)
goto _LL3AB;else{_tmp91F=_tmp91E->f1;_tmp920=_tmp91E->f2;_tmp921=_tmp91E->f3;
_tmp922=_tmp91E->f4;}}_LL3AA: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp91F,_tmp920,_tmp921,_tmp922,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3A8;_LL3AB:;_LL3AC: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp924;struct _RegionHandle*_tmp925;struct Cyc_Toc_Env*_tmp923=init_nv;_tmp924=*
_tmp923;_tmp925=_tmp924.rgn;{struct Cyc_Toc_Env*_tmp926=Cyc_Toc_set_toplevel(
_tmp925,init_nv);Cyc_Toc_exp_to_c(_tmp926,init);}}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3A8;_LL3A8:;}else{void*_tmp927=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp929;void*_tmp92A;struct Cyc_Absyn_Exp*_tmp92B;union Cyc_Absyn_Constraint*
_tmp92C;_LL3AE:{struct Cyc_Absyn_ArrayType_struct*_tmp928=(struct Cyc_Absyn_ArrayType_struct*)
_tmp927;if(_tmp928->tag != 9)goto _LL3B0;else{_tmp929=_tmp928->f1;_tmp92A=_tmp929.elt_type;
_tmp92B=_tmp929.num_elts;_tmp92C=_tmp929.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp92C))goto _LL3B0;_LL3AF: if(_tmp92B == 0){const char*
_tmpDD1;void*_tmpDD0;(_tmpDD0=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDD1="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDD1,sizeof(char),55))),_tag_dyneither(_tmpDD0,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp92B;struct Cyc_Absyn_Exp*
_tmp92F=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp930=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp92F,
_tmp930,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3AD;}_LL3B0:;_LL3B1: goto _LL3AD;
_LL3AD:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp932;struct _RegionHandle*
_tmp933;struct Cyc_Toc_Env*_tmp931=nv;_tmp932=*_tmp931;_tmp933=_tmp932.rgn;{
struct Cyc_Absyn_Stmt*_tmp934=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp935=Cyc_Toc_share_env(_tmp933,nv);struct Cyc_Toc_Env*_tmp937;struct Cyc_List_List*
_tmp938;struct Cyc_Absyn_Stmt*_tmp939;struct _tuple21 _tmp936=Cyc_Toc_xlate_pat(
_tmp935,_tmp933,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp937=_tmp936.f1;_tmp938=_tmp936.f2;_tmp939=_tmp936.f3;Cyc_Toc_stmt_to_c(
_tmp937,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp939,s,0),0);for(0;_tmp938 != 0;_tmp938=_tmp938->tl){
struct _tuple22 _tmp93B;struct _tuple1*_tmp93C;void*_tmp93D;struct _tuple22*_tmp93A=(
struct _tuple22*)_tmp938->hd;_tmp93B=*_tmp93A;_tmp93C=_tmp93B.f1;_tmp93D=_tmp93B.f2;
s=Cyc_Absyn_declare_stmt(_tmp93C,_tmp93D,0,s,0);}}return s;}}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp93E=e->r;struct Cyc_Absyn_Exp*_tmp940;struct Cyc_Absyn_Exp*_tmp942;struct
Cyc_Absyn_Exp*_tmp944;struct Cyc_Absyn_Exp*_tmp946;struct Cyc_Absyn_Exp*_tmp948;
struct Cyc_Absyn_Exp*_tmp94A;struct Cyc_Absyn_Exp*_tmp94C;struct Cyc_Absyn_Exp*
_tmp94E;struct Cyc_List_List*_tmp950;struct Cyc_Absyn_Exp*_tmp952;struct Cyc_Absyn_Exp*
_tmp953;struct Cyc_Absyn_Exp*_tmp955;struct Cyc_Absyn_Exp*_tmp956;struct Cyc_Absyn_Exp*
_tmp958;struct Cyc_Absyn_Exp*_tmp959;struct Cyc_Absyn_Exp*_tmp95B;struct Cyc_Absyn_Exp*
_tmp95C;struct Cyc_Absyn_Exp*_tmp95E;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*
_tmp961;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*
_tmp965;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_List_List*
_tmp969;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_List_List*_tmp96C;void*_tmp96E;
void**_tmp96F;struct Cyc_Absyn_Exp*_tmp970;struct _tuple8*_tmp972;struct _tuple8
_tmp973;void*_tmp974;void**_tmp975;struct Cyc_List_List*_tmp976;struct Cyc_List_List*
_tmp978;struct Cyc_List_List*_tmp97A;void*_tmp97C;void**_tmp97D;void*_tmp97F;void**
_tmp980;struct Cyc_Absyn_Stmt*_tmp982;struct Cyc_Absyn_MallocInfo _tmp984;struct Cyc_Absyn_MallocInfo*
_tmp985;_LL3B3: {struct Cyc_Absyn_Deref_e_struct*_tmp93F=(struct Cyc_Absyn_Deref_e_struct*)
_tmp93E;if(_tmp93F->tag != 22)goto _LL3B5;else{_tmp940=_tmp93F->f1;}}_LL3B4:
_tmp942=_tmp940;goto _LL3B6;_LL3B5: {struct Cyc_Absyn_AggrMember_e_struct*_tmp941=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp93E;if(_tmp941->tag != 23)goto _LL3B7;
else{_tmp942=_tmp941->f1;}}_LL3B6: _tmp944=_tmp942;goto _LL3B8;_LL3B7: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp943=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp93E;if(_tmp943->tag != 24)goto
_LL3B9;else{_tmp944=_tmp943->f1;}}_LL3B8: _tmp946=_tmp944;goto _LL3BA;_LL3B9: {
struct Cyc_Absyn_Address_e_struct*_tmp945=(struct Cyc_Absyn_Address_e_struct*)
_tmp93E;if(_tmp945->tag != 16)goto _LL3BB;else{_tmp946=_tmp945->f1;}}_LL3BA:
_tmp948=_tmp946;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_Throw_e_struct*_tmp947=(
struct Cyc_Absyn_Throw_e_struct*)_tmp93E;if(_tmp947->tag != 12)goto _LL3BD;else{
_tmp948=_tmp947->f1;}}_LL3BC: _tmp94A=_tmp948;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp949=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp93E;if(_tmp949->tag != 13)
goto _LL3BF;else{_tmp94A=_tmp949->f1;}}_LL3BE: _tmp94C=_tmp94A;goto _LL3C0;_LL3BF: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp94B=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp93E;if(_tmp94B->tag != 19)goto _LL3C1;else{_tmp94C=_tmp94B->f1;}}_LL3C0:
_tmp94E=_tmp94C;goto _LL3C2;_LL3C1: {struct Cyc_Absyn_Increment_e_struct*_tmp94D=(
struct Cyc_Absyn_Increment_e_struct*)_tmp93E;if(_tmp94D->tag != 5)goto _LL3C3;else{
_tmp94E=_tmp94D->f1;}}_LL3C2: Cyc_Toc_exptypes_to_c(_tmp94E);goto _LL3B2;_LL3C3: {
struct Cyc_Absyn_Primop_e_struct*_tmp94F=(struct Cyc_Absyn_Primop_e_struct*)
_tmp93E;if(_tmp94F->tag != 3)goto _LL3C5;else{_tmp950=_tmp94F->f2;}}_LL3C4:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp950);goto _LL3B2;_LL3C5: {struct Cyc_Absyn_And_e_struct*_tmp951=(struct Cyc_Absyn_And_e_struct*)
_tmp93E;if(_tmp951->tag != 7)goto _LL3C7;else{_tmp952=_tmp951->f1;_tmp953=_tmp951->f2;}}
_LL3C6: _tmp955=_tmp952;_tmp956=_tmp953;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_Or_e_struct*
_tmp954=(struct Cyc_Absyn_Or_e_struct*)_tmp93E;if(_tmp954->tag != 8)goto _LL3C9;
else{_tmp955=_tmp954->f1;_tmp956=_tmp954->f2;}}_LL3C8: _tmp958=_tmp955;_tmp959=
_tmp956;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_SeqExp_e_struct*_tmp957=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp93E;if(_tmp957->tag != 9)goto _LL3CB;else{_tmp958=_tmp957->f1;_tmp959=_tmp957->f2;}}
_LL3CA: _tmp95B=_tmp958;_tmp95C=_tmp959;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Subscript_e_struct*
_tmp95A=(struct Cyc_Absyn_Subscript_e_struct*)_tmp93E;if(_tmp95A->tag != 25)goto
_LL3CD;else{_tmp95B=_tmp95A->f1;_tmp95C=_tmp95A->f2;}}_LL3CC: _tmp95E=_tmp95B;
_tmp95F=_tmp95C;goto _LL3CE;_LL3CD: {struct Cyc_Absyn_Swap_e_struct*_tmp95D=(
struct Cyc_Absyn_Swap_e_struct*)_tmp93E;if(_tmp95D->tag != 36)goto _LL3CF;else{
_tmp95E=_tmp95D->f1;_tmp95F=_tmp95D->f2;}}_LL3CE: _tmp961=_tmp95E;_tmp962=_tmp95F;
goto _LL3D0;_LL3CF: {struct Cyc_Absyn_AssignOp_e_struct*_tmp960=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp93E;if(_tmp960->tag != 4)goto _LL3D1;else{_tmp961=_tmp960->f1;_tmp962=_tmp960->f3;}}
_LL3D0: Cyc_Toc_exptypes_to_c(_tmp961);Cyc_Toc_exptypes_to_c(_tmp962);goto _LL3B2;
_LL3D1: {struct Cyc_Absyn_Conditional_e_struct*_tmp963=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp93E;if(_tmp963->tag != 6)goto _LL3D3;else{_tmp964=_tmp963->f1;_tmp965=_tmp963->f2;
_tmp966=_tmp963->f3;}}_LL3D2: Cyc_Toc_exptypes_to_c(_tmp964);Cyc_Toc_exptypes_to_c(
_tmp965);Cyc_Toc_exptypes_to_c(_tmp966);goto _LL3B2;_LL3D3: {struct Cyc_Absyn_FnCall_e_struct*
_tmp967=(struct Cyc_Absyn_FnCall_e_struct*)_tmp93E;if(_tmp967->tag != 11)goto
_LL3D5;else{_tmp968=_tmp967->f1;_tmp969=_tmp967->f2;}}_LL3D4: _tmp96B=_tmp968;
_tmp96C=_tmp969;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp96A=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp93E;if(_tmp96A->tag != 10)goto _LL3D7;
else{_tmp96B=_tmp96A->f1;_tmp96C=_tmp96A->f2;}}_LL3D6: Cyc_Toc_exptypes_to_c(
_tmp96B);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp96C);goto _LL3B2;_LL3D7: {struct Cyc_Absyn_Cast_e_struct*
_tmp96D=(struct Cyc_Absyn_Cast_e_struct*)_tmp93E;if(_tmp96D->tag != 15)goto _LL3D9;
else{_tmp96E=(void**)& _tmp96D->f1;_tmp96F=(void**)((void**)& _tmp96D->f1);_tmp970=
_tmp96D->f2;}}_LL3D8:*_tmp96F=Cyc_Toc_typ_to_c(*_tmp96F);Cyc_Toc_exptypes_to_c(
_tmp970);goto _LL3B2;_LL3D9: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp971=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp93E;if(_tmp971->tag != 27)goto _LL3DB;
else{_tmp972=_tmp971->f1;_tmp973=*_tmp972;_tmp974=_tmp973.f3;_tmp975=(void**)&(*
_tmp971->f1).f3;_tmp976=_tmp971->f2;}}_LL3DA:*_tmp975=Cyc_Toc_typ_to_c(*_tmp975);
_tmp978=_tmp976;goto _LL3DC;_LL3DB: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp977=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp93E;if(_tmp977->tag != 37)
goto _LL3DD;else{_tmp978=_tmp977->f2;}}_LL3DC: _tmp97A=_tmp978;goto _LL3DE;_LL3DD: {
struct Cyc_Absyn_Array_e_struct*_tmp979=(struct Cyc_Absyn_Array_e_struct*)_tmp93E;
if(_tmp979->tag != 28)goto _LL3DF;else{_tmp97A=_tmp979->f1;}}_LL3DE: for(0;_tmp97A
!= 0;_tmp97A=_tmp97A->tl){struct Cyc_Absyn_Exp*_tmp996;struct _tuple16 _tmp995=*((
struct _tuple16*)_tmp97A->hd);_tmp996=_tmp995.f2;Cyc_Toc_exptypes_to_c(_tmp996);}
goto _LL3B2;_LL3DF: {struct Cyc_Absyn_Offsetof_e_struct*_tmp97B=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp93E;if(_tmp97B->tag != 20)goto _LL3E1;else{_tmp97C=(void**)& _tmp97B->f1;
_tmp97D=(void**)((void**)& _tmp97B->f1);}}_LL3E0: _tmp980=_tmp97D;goto _LL3E2;
_LL3E1: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp97E=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp93E;if(_tmp97E->tag != 18)goto _LL3E3;else{_tmp97F=(void**)& _tmp97E->f1;
_tmp980=(void**)((void**)& _tmp97E->f1);}}_LL3E2:*_tmp980=Cyc_Toc_typ_to_c(*
_tmp980);goto _LL3B2;_LL3E3: {struct Cyc_Absyn_StmtExp_e_struct*_tmp981=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp93E;if(_tmp981->tag != 38)goto _LL3E5;else{_tmp982=_tmp981->f1;}}_LL3E4: Cyc_Toc_stmttypes_to_c(
_tmp982);goto _LL3B2;_LL3E5: {struct Cyc_Absyn_Malloc_e_struct*_tmp983=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp93E;if(_tmp983->tag != 35)goto _LL3E7;else{_tmp984=_tmp983->f1;_tmp985=(struct
Cyc_Absyn_MallocInfo*)& _tmp983->f1;}}_LL3E6: if(_tmp985->elt_type != 0){void**
_tmpDD2;_tmp985->elt_type=((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp985->elt_type))),_tmpDD2))));}Cyc_Toc_exptypes_to_c(
_tmp985->num_elts);goto _LL3B2;_LL3E7: {struct Cyc_Absyn_Const_e_struct*_tmp986=(
struct Cyc_Absyn_Const_e_struct*)_tmp93E;if(_tmp986->tag != 0)goto _LL3E9;}_LL3E8:
goto _LL3EA;_LL3E9: {struct Cyc_Absyn_Var_e_struct*_tmp987=(struct Cyc_Absyn_Var_e_struct*)
_tmp93E;if(_tmp987->tag != 1)goto _LL3EB;}_LL3EA: goto _LL3EC;_LL3EB: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp988=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp93E;if(_tmp988->tag != 2)goto
_LL3ED;}_LL3EC: goto _LL3EE;_LL3ED: {struct Cyc_Absyn_Enum_e_struct*_tmp989=(struct
Cyc_Absyn_Enum_e_struct*)_tmp93E;if(_tmp989->tag != 33)goto _LL3EF;}_LL3EE: goto
_LL3F0;_LL3EF: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp98A=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp93E;if(_tmp98A->tag != 34)goto _LL3F1;}_LL3F0: goto _LL3B2;_LL3F1: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp98B=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp93E;if(_tmp98B->tag != 31)goto
_LL3F3;}_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Datatype_e_struct*_tmp98C=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp93E;if(_tmp98C->tag != 32)goto _LL3F5;}
_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Aggregate_e_struct*_tmp98D=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp93E;if(_tmp98D->tag != 30)goto _LL3F7;}_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp98E=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp93E;if(_tmp98E->tag != 29)
goto _LL3F9;}_LL3F8: goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Tuple_e_struct*_tmp98F=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp93E;if(_tmp98F->tag != 26)goto _LL3FB;}_LL3FA:
goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Instantiate_e_struct*_tmp990=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp93E;if(_tmp990->tag != 14)goto _LL3FD;}_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_New_e_struct*
_tmp991=(struct Cyc_Absyn_New_e_struct*)_tmp93E;if(_tmp991->tag != 17)goto _LL3FF;}
_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Valueof_e_struct*_tmp992=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp93E;if(_tmp992->tag != 40)goto _LL401;}_LL400: goto _LL402;_LL401: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp993=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp93E;if(_tmp993->tag != 39)goto
_LL403;}_LL402: goto _LL404;_LL403: {struct Cyc_Absyn_Gentyp_e_struct*_tmp994=(
struct Cyc_Absyn_Gentyp_e_struct*)_tmp93E;if(_tmp994->tag != 21)goto _LL3B2;}_LL404:{
const char*_tmpDD5;void*_tmpDD4;(_tmpDD4=0,Cyc_Tcutil_terr(e->loc,((_tmpDD5="Cyclone expression within C code",
_tag_dyneither(_tmpDD5,sizeof(char),33))),_tag_dyneither(_tmpDD4,sizeof(void*),0)));}
goto _LL3B2;_LL3B2:;}static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp99A=d->r;
struct Cyc_Absyn_Vardecl*_tmp99C;struct Cyc_Absyn_Fndecl*_tmp99E;struct Cyc_Absyn_Aggrdecl*
_tmp9A0;struct Cyc_Absyn_Enumdecl*_tmp9A2;struct Cyc_Absyn_Typedefdecl*_tmp9A4;
_LL406: {struct Cyc_Absyn_Var_d_struct*_tmp99B=(struct Cyc_Absyn_Var_d_struct*)
_tmp99A;if(_tmp99B->tag != 0)goto _LL408;else{_tmp99C=_tmp99B->f1;}}_LL407: _tmp99C->type=
Cyc_Toc_typ_to_c(_tmp99C->type);if(_tmp99C->initializer != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp99C->initializer));goto _LL405;_LL408: {
struct Cyc_Absyn_Fn_d_struct*_tmp99D=(struct Cyc_Absyn_Fn_d_struct*)_tmp99A;if(
_tmp99D->tag != 1)goto _LL40A;else{_tmp99E=_tmp99D->f1;}}_LL409: _tmp99E->ret_type=
Cyc_Toc_typ_to_c(_tmp99E->ret_type);{struct Cyc_List_List*_tmp9B0=_tmp99E->args;
for(0;_tmp9B0 != 0;_tmp9B0=_tmp9B0->tl){(*((struct _tuple25*)_tmp9B0->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp9B0->hd)).f3);}}goto _LL405;_LL40A: {struct Cyc_Absyn_Aggr_d_struct*
_tmp99F=(struct Cyc_Absyn_Aggr_d_struct*)_tmp99A;if(_tmp99F->tag != 6)goto _LL40C;
else{_tmp9A0=_tmp99F->f1;}}_LL40B: Cyc_Toc_aggrdecl_to_c(_tmp9A0);goto _LL405;
_LL40C: {struct Cyc_Absyn_Enum_d_struct*_tmp9A1=(struct Cyc_Absyn_Enum_d_struct*)
_tmp99A;if(_tmp9A1->tag != 8)goto _LL40E;else{_tmp9A2=_tmp9A1->f1;}}_LL40D: if(
_tmp9A2->fields != 0){struct Cyc_List_List*_tmp9B1=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9A2->fields))->v;for(0;_tmp9B1 != 0;_tmp9B1=_tmp9B1->tl){
struct Cyc_Absyn_Enumfield*_tmp9B2=(struct Cyc_Absyn_Enumfield*)_tmp9B1->hd;if(
_tmp9B2->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9B2->tag));}}
goto _LL405;_LL40E: {struct Cyc_Absyn_Typedef_d_struct*_tmp9A3=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp99A;if(_tmp9A3->tag != 9)goto _LL410;else{_tmp9A4=_tmp9A3->f1;}}_LL40F:{struct
Cyc_Core_Opt*_tmpDD6;_tmp9A4->defn=((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((
_tmpDD6->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9A4->defn))->v),_tmpDD6))));}goto _LL405;_LL410: {struct Cyc_Absyn_Let_d_struct*
_tmp9A5=(struct Cyc_Absyn_Let_d_struct*)_tmp99A;if(_tmp9A5->tag != 2)goto _LL412;}
_LL411: goto _LL413;_LL412: {struct Cyc_Absyn_Letv_d_struct*_tmp9A6=(struct Cyc_Absyn_Letv_d_struct*)
_tmp99A;if(_tmp9A6->tag != 3)goto _LL414;}_LL413: goto _LL415;_LL414: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9A7=(struct Cyc_Absyn_Datatype_d_struct*)_tmp99A;if(_tmp9A7->tag != 7)goto
_LL416;}_LL415: goto _LL417;_LL416: {struct Cyc_Absyn_Namespace_d_struct*_tmp9A8=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp99A;if(_tmp9A8->tag != 10)goto _LL418;}
_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_Using_d_struct*_tmp9A9=(struct Cyc_Absyn_Using_d_struct*)
_tmp99A;if(_tmp9A9->tag != 11)goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9AA=(struct Cyc_Absyn_ExternC_d_struct*)_tmp99A;if(_tmp9AA->tag != 12)goto
_LL41C;}_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9AB=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp99A;if(_tmp9AB->tag != 13)
goto _LL41E;}_LL41D: goto _LL41F;_LL41E: {struct Cyc_Absyn_Region_d_struct*_tmp9AC=(
struct Cyc_Absyn_Region_d_struct*)_tmp99A;if(_tmp9AC->tag != 4)goto _LL420;}_LL41F:
goto _LL421;_LL420: {struct Cyc_Absyn_Alias_d_struct*_tmp9AD=(struct Cyc_Absyn_Alias_d_struct*)
_tmp99A;if(_tmp9AD->tag != 5)goto _LL422;}_LL421:{const char*_tmpDD9;void*_tmpDD8;(
_tmpDD8=0,Cyc_Tcutil_terr(d->loc,((_tmpDD9="Cyclone declaration within C code",
_tag_dyneither(_tmpDD9,sizeof(char),34))),_tag_dyneither(_tmpDD8,sizeof(void*),0)));}
goto _LL405;_LL422: {struct Cyc_Absyn_Porton_d_struct*_tmp9AE=(struct Cyc_Absyn_Porton_d_struct*)
_tmp99A;if(_tmp9AE->tag != 14)goto _LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9AF=(struct Cyc_Absyn_Portoff_d_struct*)_tmp99A;if(_tmp9AF->tag != 15)goto
_LL405;}_LL425: goto _LL405;_LL405:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){void*_tmp9B6=s->r;
struct Cyc_Absyn_Exp*_tmp9B8;struct Cyc_Absyn_Stmt*_tmp9BA;struct Cyc_Absyn_Stmt*
_tmp9BB;struct Cyc_Absyn_Exp*_tmp9BD;struct Cyc_Absyn_Exp*_tmp9BF;struct Cyc_Absyn_Stmt*
_tmp9C0;struct Cyc_Absyn_Stmt*_tmp9C1;struct _tuple9 _tmp9C3;struct Cyc_Absyn_Exp*
_tmp9C4;struct Cyc_Absyn_Stmt*_tmp9C5;struct Cyc_Absyn_Exp*_tmp9C7;struct _tuple9
_tmp9C8;struct Cyc_Absyn_Exp*_tmp9C9;struct _tuple9 _tmp9CA;struct Cyc_Absyn_Exp*
_tmp9CB;struct Cyc_Absyn_Stmt*_tmp9CC;struct Cyc_Absyn_Exp*_tmp9CE;struct Cyc_List_List*
_tmp9CF;struct Cyc_Absyn_Decl*_tmp9D1;struct Cyc_Absyn_Stmt*_tmp9D2;struct Cyc_Absyn_Stmt*
_tmp9D4;struct _tuple9 _tmp9D5;struct Cyc_Absyn_Exp*_tmp9D6;_LL427: {struct Cyc_Absyn_Exp_s_struct*
_tmp9B7=(struct Cyc_Absyn_Exp_s_struct*)_tmp9B6;if(_tmp9B7->tag != 1)goto _LL429;
else{_tmp9B8=_tmp9B7->f1;}}_LL428: Cyc_Toc_exptypes_to_c(_tmp9B8);goto _LL426;
_LL429: {struct Cyc_Absyn_Seq_s_struct*_tmp9B9=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9B6;if(_tmp9B9->tag != 2)goto _LL42B;else{_tmp9BA=_tmp9B9->f1;_tmp9BB=_tmp9B9->f2;}}
_LL42A: Cyc_Toc_stmttypes_to_c(_tmp9BA);Cyc_Toc_stmttypes_to_c(_tmp9BB);goto
_LL426;_LL42B: {struct Cyc_Absyn_Return_s_struct*_tmp9BC=(struct Cyc_Absyn_Return_s_struct*)
_tmp9B6;if(_tmp9BC->tag != 3)goto _LL42D;else{_tmp9BD=_tmp9BC->f1;}}_LL42C: if(
_tmp9BD != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9BD);goto _LL426;
_LL42D: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9BE=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9B6;if(_tmp9BE->tag != 4)goto _LL42F;else{_tmp9BF=_tmp9BE->f1;_tmp9C0=_tmp9BE->f2;
_tmp9C1=_tmp9BE->f3;}}_LL42E: Cyc_Toc_exptypes_to_c(_tmp9BF);Cyc_Toc_stmttypes_to_c(
_tmp9C0);Cyc_Toc_stmttypes_to_c(_tmp9C1);goto _LL426;_LL42F: {struct Cyc_Absyn_While_s_struct*
_tmp9C2=(struct Cyc_Absyn_While_s_struct*)_tmp9B6;if(_tmp9C2->tag != 5)goto _LL431;
else{_tmp9C3=_tmp9C2->f1;_tmp9C4=_tmp9C3.f1;_tmp9C5=_tmp9C2->f2;}}_LL430: Cyc_Toc_exptypes_to_c(
_tmp9C4);Cyc_Toc_stmttypes_to_c(_tmp9C5);goto _LL426;_LL431: {struct Cyc_Absyn_For_s_struct*
_tmp9C6=(struct Cyc_Absyn_For_s_struct*)_tmp9B6;if(_tmp9C6->tag != 9)goto _LL433;
else{_tmp9C7=_tmp9C6->f1;_tmp9C8=_tmp9C6->f2;_tmp9C9=_tmp9C8.f1;_tmp9CA=_tmp9C6->f3;
_tmp9CB=_tmp9CA.f1;_tmp9CC=_tmp9C6->f4;}}_LL432: Cyc_Toc_exptypes_to_c(_tmp9C7);
Cyc_Toc_exptypes_to_c(_tmp9C9);Cyc_Toc_exptypes_to_c(_tmp9CB);Cyc_Toc_stmttypes_to_c(
_tmp9CC);goto _LL426;_LL433: {struct Cyc_Absyn_Switch_s_struct*_tmp9CD=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9B6;if(_tmp9CD->tag != 10)goto _LL435;else{_tmp9CE=_tmp9CD->f1;_tmp9CF=_tmp9CD->f2;}}
_LL434: Cyc_Toc_exptypes_to_c(_tmp9CE);for(0;_tmp9CF != 0;_tmp9CF=_tmp9CF->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9CF->hd)->body);}goto _LL426;_LL435: {struct
Cyc_Absyn_Decl_s_struct*_tmp9D0=(struct Cyc_Absyn_Decl_s_struct*)_tmp9B6;if(
_tmp9D0->tag != 12)goto _LL437;else{_tmp9D1=_tmp9D0->f1;_tmp9D2=_tmp9D0->f2;}}
_LL436: Cyc_Toc_decltypes_to_c(_tmp9D1);Cyc_Toc_stmttypes_to_c(_tmp9D2);goto
_LL426;_LL437: {struct Cyc_Absyn_Do_s_struct*_tmp9D3=(struct Cyc_Absyn_Do_s_struct*)
_tmp9B6;if(_tmp9D3->tag != 14)goto _LL439;else{_tmp9D4=_tmp9D3->f1;_tmp9D5=_tmp9D3->f2;
_tmp9D6=_tmp9D5.f1;}}_LL438: Cyc_Toc_stmttypes_to_c(_tmp9D4);Cyc_Toc_exptypes_to_c(
_tmp9D6);goto _LL426;_LL439: {struct Cyc_Absyn_Skip_s_struct*_tmp9D7=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9B6;if(_tmp9D7->tag != 0)goto _LL43B;}_LL43A: goto _LL43C;_LL43B: {struct Cyc_Absyn_Break_s_struct*
_tmp9D8=(struct Cyc_Absyn_Break_s_struct*)_tmp9B6;if(_tmp9D8->tag != 6)goto _LL43D;}
_LL43C: goto _LL43E;_LL43D: {struct Cyc_Absyn_Continue_s_struct*_tmp9D9=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9B6;if(_tmp9D9->tag != 7)goto _LL43F;}_LL43E: goto _LL440;_LL43F: {struct Cyc_Absyn_Goto_s_struct*
_tmp9DA=(struct Cyc_Absyn_Goto_s_struct*)_tmp9B6;if(_tmp9DA->tag != 8)goto _LL441;}
_LL440: goto _LL426;_LL441: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9DB=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9B6;if(_tmp9DB->tag != 11)goto _LL443;}_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_Label_s_struct*
_tmp9DC=(struct Cyc_Absyn_Label_s_struct*)_tmp9B6;if(_tmp9DC->tag != 13)goto _LL445;}
_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9DD=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9B6;if(_tmp9DD->tag != 15)goto _LL447;}_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9DE=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9B6;if(_tmp9DE->tag != 16)goto
_LL426;}_LL448:{const char*_tmpDDC;void*_tmpDDB;(_tmpDDB=0,Cyc_Tcutil_terr(s->loc,((
_tmpDDC="Cyclone statement in C code",_tag_dyneither(_tmpDDC,sizeof(char),28))),
_tag_dyneither(_tmpDDB,sizeof(void*),0)));}goto _LL426;_LL426:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude);static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){const char*_tmpDDF;void*
_tmpDDE;(_tmpDDE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDDF="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDDF,sizeof(char),29))),
_tag_dyneither(_tmpDDE,sizeof(void*),0)));}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9E3=d->r;struct Cyc_Absyn_Vardecl*_tmp9E5;struct Cyc_Absyn_Fndecl*
_tmp9E7;struct Cyc_Absyn_Aggrdecl*_tmp9ED;struct Cyc_Absyn_Datatypedecl*_tmp9EF;
struct Cyc_Absyn_Enumdecl*_tmp9F1;struct Cyc_Absyn_Typedefdecl*_tmp9F3;struct Cyc_List_List*
_tmp9F7;struct Cyc_List_List*_tmp9F9;struct Cyc_List_List*_tmp9FB;struct Cyc_List_List*
_tmp9FD;_LL44A: {struct Cyc_Absyn_Var_d_struct*_tmp9E4=(struct Cyc_Absyn_Var_d_struct*)
_tmp9E3;if(_tmp9E4->tag != 0)goto _LL44C;else{_tmp9E5=_tmp9E4->f1;}}_LL44B: {
struct _tuple1*_tmp9FE=_tmp9E5->name;if(_tmp9E5->sc == Cyc_Absyn_ExternC){struct
_tuple1*_tmpDE0;_tmp9FE=((_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0->f1=Cyc_Absyn_Rel_n(
0),((_tmpDE0->f2=(*_tmp9FE).f2,_tmpDE0))))));}if(_tmp9E5->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9E5->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9E5->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpDE3;struct Cyc_Absyn_Global_b_struct*_tmpDE2;
nv=Cyc_Toc_add_varmap(r,nv,_tmp9E5->name,Cyc_Absyn_varb_exp(_tmp9FE,(void*)((
_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE3.tag=1,((_tmpDE3.f1=
_tmp9E5,_tmpDE3)))),_tmpDE2)))),0));}_tmp9E5->name=_tmp9FE;_tmp9E5->sc=Cyc_Toc_scope_to_c(
_tmp9E5->sc);_tmp9E5->type=Cyc_Toc_typ_to_c_array(_tmp9E5->type);{struct Cyc_List_List*
_tmpDE4;Cyc_Toc_result_decls=((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4->hd=
d,((_tmpDE4->tl=Cyc_Toc_result_decls,_tmpDE4))))));}goto _LL449;}_LL44C: {struct
Cyc_Absyn_Fn_d_struct*_tmp9E6=(struct Cyc_Absyn_Fn_d_struct*)_tmp9E3;if(_tmp9E6->tag
!= 1)goto _LL44E;else{_tmp9E7=_tmp9E6->f1;}}_LL44D: {struct _tuple1*_tmpA03=
_tmp9E7->name;if(_tmp9E7->sc == Cyc_Absyn_ExternC){{struct _tuple1*_tmpDE5;_tmpA03=((
_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->f1=Cyc_Absyn_Rel_n(0),((_tmpDE5->f2=(*
_tmpA03).f2,_tmpDE5))))));}_tmp9E7->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,
nv,_tmp9E7->name,Cyc_Absyn_var_exp(_tmpA03,0));_tmp9E7->name=_tmpA03;Cyc_Toc_fndecl_to_c(
nv,_tmp9E7,cinclude);{struct Cyc_List_List*_tmpDE6;Cyc_Toc_result_decls=((_tmpDE6=
_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->hd=d,((_tmpDE6->tl=Cyc_Toc_result_decls,
_tmpDE6))))));}goto _LL449;}_LL44E: {struct Cyc_Absyn_Let_d_struct*_tmp9E8=(struct
Cyc_Absyn_Let_d_struct*)_tmp9E3;if(_tmp9E8->tag != 2)goto _LL450;}_LL44F: goto
_LL451;_LL450: {struct Cyc_Absyn_Letv_d_struct*_tmp9E9=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9E3;if(_tmp9E9->tag != 3)goto _LL452;}_LL451: {const char*_tmpDE9;void*_tmpDE8;(
_tmpDE8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDE9="letdecl at toplevel",_tag_dyneither(_tmpDE9,sizeof(char),20))),
_tag_dyneither(_tmpDE8,sizeof(void*),0)));}_LL452: {struct Cyc_Absyn_Region_d_struct*
_tmp9EA=(struct Cyc_Absyn_Region_d_struct*)_tmp9E3;if(_tmp9EA->tag != 4)goto _LL454;}
_LL453: {const char*_tmpDEC;void*_tmpDEB;(_tmpDEB=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDEC="region decl at toplevel",
_tag_dyneither(_tmpDEC,sizeof(char),24))),_tag_dyneither(_tmpDEB,sizeof(void*),0)));}
_LL454: {struct Cyc_Absyn_Alias_d_struct*_tmp9EB=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9E3;if(_tmp9EB->tag != 5)goto _LL456;}_LL455: {const char*_tmpDEF;void*_tmpDEE;(
_tmpDEE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDEF="alias decl at toplevel",_tag_dyneither(_tmpDEF,sizeof(char),23))),
_tag_dyneither(_tmpDEE,sizeof(void*),0)));}_LL456: {struct Cyc_Absyn_Aggr_d_struct*
_tmp9EC=(struct Cyc_Absyn_Aggr_d_struct*)_tmp9E3;if(_tmp9EC->tag != 6)goto _LL458;
else{_tmp9ED=_tmp9EC->f1;}}_LL457: Cyc_Toc_aggrdecl_to_c(_tmp9ED);goto _LL449;
_LL458: {struct Cyc_Absyn_Datatype_d_struct*_tmp9EE=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9E3;if(_tmp9EE->tag != 7)goto _LL45A;else{_tmp9EF=_tmp9EE->f1;}}_LL459: if(
_tmp9EF->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmp9EF);else{Cyc_Toc_datatypedecl_to_c(
_tmp9EF);}goto _LL449;_LL45A: {struct Cyc_Absyn_Enum_d_struct*_tmp9F0=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9E3;if(_tmp9F0->tag != 8)goto _LL45C;else{_tmp9F1=_tmp9F0->f1;}}_LL45B: Cyc_Toc_enumdecl_to_c(
nv,_tmp9F1);{struct Cyc_List_List*_tmpDF0;Cyc_Toc_result_decls=((_tmpDF0=
_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0->hd=d,((_tmpDF0->tl=Cyc_Toc_result_decls,
_tmpDF0))))));}goto _LL449;_LL45C: {struct Cyc_Absyn_Typedef_d_struct*_tmp9F2=(
struct Cyc_Absyn_Typedef_d_struct*)_tmp9E3;if(_tmp9F2->tag != 9)goto _LL45E;else{
_tmp9F3=_tmp9F2->f1;}}_LL45D: _tmp9F3->name=_tmp9F3->name;_tmp9F3->tvs=0;if(
_tmp9F3->defn != 0){struct Cyc_Core_Opt*_tmpDF1;_tmp9F3->defn=((_tmpDF1=_cycalloc(
sizeof(*_tmpDF1)),((_tmpDF1->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9F3->defn))->v),_tmpDF1))));}else{switch((enum Cyc_Absyn_Kind)((
struct Cyc_Core_Opt*)_check_null(_tmp9F3->kind))->v){case Cyc_Absyn_BoxKind: _LL46A:{
struct Cyc_Core_Opt*_tmpDF2;_tmp9F3->defn=((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((
_tmpDF2->v=Cyc_Absyn_void_star_typ(),_tmpDF2))));}break;default: _LL46B:{struct
Cyc_Core_Opt*_tmpDF3;_tmp9F3->defn=((_tmpDF3=_cycalloc(sizeof(*_tmpDF3)),((
_tmpDF3->v=(void*)& Cyc_Absyn_VoidType_val,_tmpDF3))));}break;}}{struct Cyc_List_List*
_tmpDF4;Cyc_Toc_result_decls=((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4->hd=
d,((_tmpDF4->tl=Cyc_Toc_result_decls,_tmpDF4))))));}goto _LL449;_LL45E: {struct
Cyc_Absyn_Porton_d_struct*_tmp9F4=(struct Cyc_Absyn_Porton_d_struct*)_tmp9E3;if(
_tmp9F4->tag != 14)goto _LL460;}_LL45F: goto _LL461;_LL460: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9F5=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9E3;if(_tmp9F5->tag != 15)goto
_LL462;}_LL461: goto _LL449;_LL462: {struct Cyc_Absyn_Namespace_d_struct*_tmp9F6=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9E3;if(_tmp9F6->tag != 10)goto _LL464;
else{_tmp9F7=_tmp9F6->f2;}}_LL463: _tmp9F9=_tmp9F7;goto _LL465;_LL464: {struct Cyc_Absyn_Using_d_struct*
_tmp9F8=(struct Cyc_Absyn_Using_d_struct*)_tmp9E3;if(_tmp9F8->tag != 11)goto _LL466;
else{_tmp9F9=_tmp9F8->f2;}}_LL465: _tmp9FB=_tmp9F9;goto _LL467;_LL466: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9FA=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9E3;if(_tmp9FA->tag != 12)goto
_LL468;else{_tmp9FB=_tmp9FA->f1;}}_LL467: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FB,top,
cinclude);goto _LL449;_LL468: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp9FC=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9E3;if(_tmp9FC->tag != 13)goto _LL449;
else{_tmp9FD=_tmp9FC->f1;}}_LL469: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FD,top,1);goto
_LL449;_LL449:;}}return nv;}static void Cyc_Toc_init();static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA12;struct Cyc_Core_NewRegion _tmpA11=Cyc_Core_new_dynregion();
_tmpA12=_tmpA11.dynregion;{struct _DynRegionFrame _tmpA13;struct _RegionHandle*d=
_open_dynregion(& _tmpA13,_tmpA12);{struct Cyc_List_List**_tmpDFF;struct Cyc_Dict_Dict*
_tmpDFE;struct Cyc_Set_Set**_tmpDFD;struct Cyc_Dict_Dict*_tmpDFC;struct Cyc_Dict_Dict*
_tmpDFB;struct Cyc_Toc_TocState*_tmpDFA;Cyc_Toc_toc_state=((_tmpDFA=_cycalloc(
sizeof(*_tmpDFA)),((_tmpDFA->dyn=(struct _DynRegionHandle*)_tmpA12,((_tmpDFA->tuple_types=(
struct Cyc_List_List**)((_tmpDFF=_region_malloc(d,sizeof(*_tmpDFF)),((_tmpDFF[0]=
0,_tmpDFF)))),((_tmpDFA->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpDFE=
_region_malloc(d,sizeof(*_tmpDFE)),((_tmpDFE[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpDFE)))),((_tmpDFA->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpDFD=
_region_malloc(d,sizeof(*_tmpDFD)),((_tmpDFD[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpDFD)))),((_tmpDFA->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpDFC=
_region_malloc(d,sizeof(*_tmpDFC)),((_tmpDFC[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpDFC)))),((_tmpDFA->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpDFB=_region_malloc(
d,sizeof(*_tmpDFB)),((_tmpDFB[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpDFB)))),((_tmpDFA->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpDFA))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE00;Cyc_Toc_globals=_tag_dyneither(((_tmpE00=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38),((_tmpE00[0]=& Cyc_Toc__throw_str,((_tmpE00[1]=&
Cyc_Toc_setjmp_str,((_tmpE00[2]=& Cyc_Toc__push_handler_str,((_tmpE00[3]=& Cyc_Toc__pop_handler_str,((
_tmpE00[4]=& Cyc_Toc__exn_thrown_str,((_tmpE00[5]=& Cyc_Toc__npop_handler_str,((
_tmpE00[6]=& Cyc_Toc__check_null_str,((_tmpE00[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE00[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE00[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE00[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE00[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE00[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE00[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE00[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE00[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE00[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE00[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE00[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE00[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE00[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE00[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE00[22]=& Cyc_Toc__cycalloc_str,((_tmpE00[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE00[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE00[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE00[26]=& Cyc_Toc__region_malloc_str,((_tmpE00[27]=& Cyc_Toc__region_calloc_str,((
_tmpE00[28]=& Cyc_Toc__check_times_str,((_tmpE00[29]=& Cyc_Toc__new_region_str,((
_tmpE00[30]=& Cyc_Toc__push_region_str,((_tmpE00[31]=& Cyc_Toc__pop_region_str,((
_tmpE00[32]=& Cyc_Toc__open_dynregion_str,((_tmpE00[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE00[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE00[35]=& Cyc_Toc__reset_region_str,((
_tmpE00[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE00[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,
_tmpE00)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};_pop_dynregion(d);}}struct Cyc_List_List*Cyc_Toc_toc(
struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{struct _RegionHandle _tmpA1B=_new_region("start");struct
_RegionHandle*start=& _tmpA1B;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmpA1D;struct Cyc_Toc_TocState _tmpA1C=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA1D=_tmpA1C.dyn;Cyc_Core_free_dynregion(
_tmpA1D);}{struct Cyc_List_List*_tmpA1E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA1E;};
_pop_region(start);}}
