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
_tmp5);};};}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};void*Cyc_Toc_aggrdecl_type(
struct _tuple1*q,void*(*type_maker)(struct _tuple1*));void*Cyc_Toc_aggrdecl_type(
struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){struct _DynRegionHandle*_tmp7;
struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState _tmp6=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;_tmp8=_tmp6.aggrs_so_far;{struct
_DynRegionFrame _tmp9;struct _RegionHandle*d=_open_dynregion(& _tmp9,_tmp7);{struct
_tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*
_tmp8,q);if(v == 0){void*_tmpA=type_maker(q);_npop_handler(0);return _tmpA;}else{
struct _tuple15 _tmpC;void*_tmpD;struct _tuple15*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{
void*_tmpE=_tmpD;_npop_handler(0);return _tmpE;};}};_pop_dynregion(d);};}static
int Cyc_Toc_tuple_type_counter=0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=
0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_struct{
char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_struct{
char*tag;};static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap);static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);{const char*_tmpA23;void*_tmpA22;(_tmpA22=0,Cyc_fprintf(Cyc_stderr,((
_tmpA23="\n",_tag_dyneither(_tmpA23,sizeof(char),2))),_tag_dyneither(_tmpA22,
sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Unimplemented_struct
_tmpA26;struct Cyc_Toc_Toc_Unimplemented_struct*_tmpA25;(int)_throw((void*)((
_tmpA25=_cycalloc_atomic(sizeof(*_tmpA25)),((_tmpA25[0]=((_tmpA26.tag=Cyc_Toc_Toc_Unimplemented,
_tmpA26)),_tmpA25)))));};}static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{const char*_tmpA29;void*
_tmpA28;(_tmpA28=0,Cyc_fprintf(Cyc_stderr,((_tmpA29="\n",_tag_dyneither(_tmpA29,
sizeof(char),2))),_tag_dyneither(_tmpA28,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Toc_Toc_Impossible_struct _tmpA2C;struct Cyc_Toc_Toc_Impossible_struct*
_tmpA2B;(int)_throw((void*)((_tmpA2B=_cycalloc_atomic(sizeof(*_tmpA2B)),((
_tmpA2B[0]=((_tmpA2C.tag=Cyc_Toc_Toc_Impossible,_tmpA2C)),_tmpA2B)))));};}char
Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_struct{char*tag;
};static char _tmp17[5]="curr";static struct _dyneither_ptr Cyc_Toc_curr_string={
_tmp17,_tmp17,_tmp17 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp18[4]="tag";static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp18,
_tmp18,_tmp18 + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static char _tmp19[4]="val";static struct _dyneither_ptr Cyc_Toc_val_string={_tmp19,
_tmp19,_tmp19 + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;
static char _tmp1A[14]="_handler_cons";static struct _dyneither_ptr Cyc_Toc__handler_cons_string={
_tmp1A,_tmp1A,_tmp1A + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=&
Cyc_Toc__handler_cons_string;static char _tmp1B[8]="handler";static struct
_dyneither_ptr Cyc_Toc_handler_string={_tmp1B,_tmp1B,_tmp1B + 8};static struct
_dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp1C[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp1C,_tmp1C,_tmp1C + 14};
static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp1D[17]="_DynRegionHandle";static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={
_tmp1D,_tmp1D,_tmp1D + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=&
Cyc_Toc__DynRegionHandle_string;static char _tmp1E[16]="_DynRegionFrame";static
struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp1E,_tmp1E,_tmp1E + 16};
static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char
_tmp1F[7]="_throw";static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp1F,_tmp1F,
_tmp1F + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={3,0}},& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((
void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp21[7]="setjmp";static struct
_dyneither_ptr Cyc_Toc_setjmp_str={_tmp21,_tmp21,_tmp21 + 7};static struct _tuple1
Cyc_Toc_setjmp_pr={{.Loc_n={3,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};
static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)&
Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;
static char _tmp23[14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={
_tmp23,_tmp23,_tmp23 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={
3,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1,& Cyc_Toc__push_handler_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,
0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp25[13]="_pop_handler";static struct
_dyneither_ptr Cyc_Toc__pop_handler_str={_tmp25,_tmp25,_tmp25 + 13};static struct
_tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={3,0}},& Cyc_Toc__pop_handler_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char
_tmp27[12]="_exn_thrown";static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27,_tmp27 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={3,
0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp29[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp29,_tmp29,_tmp29 + 14};static struct
_tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={3,0}},& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(
void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static
struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char
_tmp2B[12]="_check_null";static struct _dyneither_ptr Cyc_Toc__check_null_str={
_tmp2B,_tmp2B,_tmp2B + 12};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={3,
0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={
1,& Cyc_Toc__check_null_pr,(void*)((void*)& Cyc_Absyn_Unresolved_b_val)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(
void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp2D[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp2D,_tmp2D,
_tmp2D + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={
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
skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpA2D;skip_stmt_opt=((_tmpA2D=
_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D[0]=Cyc_Absyn_skip_stmt(0),_tmpA2D))));}
return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*
e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*
e){struct Cyc_Absyn_Cast_e_struct _tmpA30;struct Cyc_Absyn_Cast_e_struct*_tmpA2F;
return(void*)((_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F[0]=((_tmpA30.tag=15,((
_tmpA30.f1=(void*)t,((_tmpA30.f2=e,((_tmpA30.f3=0,((_tmpA30.f4=Cyc_Absyn_No_coercion,
_tmpA30)))))))))),_tmpA2F))));}static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*
e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Deref_e_struct
_tmpA33;struct Cyc_Absyn_Deref_e_struct*_tmpA32;return(void*)((_tmpA32=_cycalloc(
sizeof(*_tmpA32)),((_tmpA32[0]=((_tmpA33.tag=22,((_tmpA33.f1=e,_tmpA33)))),
_tmpA32))));}static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){struct Cyc_Absyn_Subscript_e_struct _tmpA36;struct Cyc_Absyn_Subscript_e_struct*
_tmpA35;return(void*)((_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35[0]=((
_tmpA36.tag=25,((_tmpA36.f1=e1,((_tmpA36.f2=e2,_tmpA36)))))),_tmpA35))));}static
void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_stmt_exp_r(
struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct _tmpA39;struct Cyc_Absyn_StmtExp_e_struct*
_tmpA38;return(void*)((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=((
_tmpA39.tag=38,((_tmpA39.f1=s,_tmpA39)))),_tmpA38))));}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t);static void*Cyc_Toc_sizeoftyp_exp_r(void*t){struct Cyc_Absyn_Sizeoftyp_e_struct
_tmpA3C;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpA3B;return(void*)((_tmpA3B=
_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B[0]=((_tmpA3C.tag=18,((_tmpA3C.f1=(void*)t,
_tmpA3C)))),_tmpA3B))));}static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,
struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct _tmpA3F;struct Cyc_Absyn_FnCall_e_struct*
_tmpA3E;return(void*)((_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=((
_tmpA3F.tag=11,((_tmpA3F.f1=e,((_tmpA3F.f2=es,((_tmpA3F.f3=0,_tmpA3F)))))))),
_tmpA3E))));}static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(
struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct _tmpA42;struct Cyc_Absyn_Exp_s_struct*
_tmpA41;return(void*)((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41[0]=((
_tmpA42.tag=1,((_tmpA42.f1=e,_tmpA42)))),_tmpA41))));}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2);static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){struct Cyc_Absyn_Seq_s_struct
_tmpA45;struct Cyc_Absyn_Seq_s_struct*_tmpA44;return(void*)((_tmpA44=_cycalloc(
sizeof(*_tmpA44)),((_tmpA44[0]=((_tmpA45.tag=2,((_tmpA45.f1=s1,((_tmpA45.f2=s2,
_tmpA45)))))),_tmpA44))));}static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct
_tmpA48;struct Cyc_Absyn_Conditional_e_struct*_tmpA47;return(void*)((_tmpA47=
_cycalloc(sizeof(*_tmpA47)),((_tmpA47[0]=((_tmpA48.tag=6,((_tmpA48.f1=e1,((
_tmpA48.f2=e2,((_tmpA48.f3=e3,_tmpA48)))))))),_tmpA47))));}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){struct Cyc_Absyn_AggrMember_e_struct
_tmpA4B;struct Cyc_Absyn_AggrMember_e_struct*_tmpA4A;return(void*)((_tmpA4A=
_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=((_tmpA4B.tag=23,((_tmpA4B.f1=e,((
_tmpA4B.f2=n,((_tmpA4B.f3=0,((_tmpA4B.f4=0,_tmpA4B)))))))))),_tmpA4A))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n);
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmpA4E;struct Cyc_Absyn_AggrArrow_e_struct*
_tmpA4D;return(void*)((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D[0]=((
_tmpA4E.tag=24,((_tmpA4E.f1=e,((_tmpA4E.f2=n,((_tmpA4E.f3=0,((_tmpA4E.f4=0,
_tmpA4E)))))))))),_tmpA4D))));}static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds);static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*
tdopt,struct Cyc_List_List*ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmpA51;
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpA50;return(void*)((_tmpA50=_cycalloc(
sizeof(*_tmpA50)),((_tmpA50[0]=((_tmpA51.tag=37,((_tmpA51.f1=tdopt,((_tmpA51.f2=
ds,_tmpA51)))))),_tmpA50))));}static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s);static void*Cyc_Toc_goto_stmt_r(struct
_dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Goto_s_struct _tmpA54;
struct Cyc_Absyn_Goto_s_struct*_tmpA53;return(void*)((_tmpA53=_cycalloc(sizeof(*
_tmpA53)),((_tmpA53[0]=((_tmpA54.tag=8,((_tmpA54.f1=v,((_tmpA54.f2=s,_tmpA54)))))),
_tmpA53))));}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={0,{.Int_c={4,{
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
_tmpA5A;const char*_tmpA59;struct Cyc_Core_Impossible_struct*_tmpA58;(int)_throw((
void*)((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA5A.tag=Cyc_Core_Impossible,((
_tmpA5A.f1=((_tmpA59="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpA59,sizeof(char),44))),_tmpA5A)))),_tmpA58)))));}}goto _LL0;
_LL3: {struct Cyc_Absyn_FloatType_struct*_tmpC7=(struct Cyc_Absyn_FloatType_struct*)
_tmpC4;if(_tmpC7->tag != 7)goto _LL5;}_LL4: function=fS->ffloat;goto _LL0;_LL5: {
struct Cyc_Absyn_DoubleType_struct*_tmpC8=(struct Cyc_Absyn_DoubleType_struct*)
_tmpC4;if(_tmpC8->tag != 8)goto _LL7;else{_tmpC9=_tmpC8->f1;}}_LL6: switch(_tmpC9){
case 1: _LL10: function=fS->flongdouble;break;default: _LL11: function=fS->fdouble;}
goto _LL0;_LL7: {struct Cyc_Absyn_PointerType_struct*_tmpCA=(struct Cyc_Absyn_PointerType_struct*)
_tmpC4;if(_tmpCA->tag != 5)goto _LL9;}_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;
_LLA: {struct Cyc_Core_Impossible_struct _tmpA60;const char*_tmpA5F;struct Cyc_Core_Impossible_struct*
_tmpA5E;(int)_throw((void*)((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E[0]=((
_tmpA60.tag=Cyc_Core_Impossible,((_tmpA60.f1=((_tmpA5F="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpA5F,sizeof(char),64))),_tmpA60)))),_tmpA5E)))));}_LL0:;}
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
_tmpA66;const char*_tmpA65;struct Cyc_Core_Impossible_struct*_tmpA64;(int)_throw((
void*)((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA66.tag=Cyc_Core_Impossible,((
_tmpA66.f1=((_tmpA65="impossible type (not pointer)",_tag_dyneither(_tmpA65,
sizeof(char),30))),_tmpA66)))),_tmpA64)))));}_LL13:;}struct _tuple16{struct Cyc_List_List*
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
_LL34:;_LL35: {const char*_tmpA69;void*_tmpA68;(_tmpA68=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA69="is_nullable",
_tag_dyneither(_tmpA69,sizeof(char),12))),_tag_dyneither(_tmpA68,sizeof(void*),0)));}
_LL31:;}static char _tmp10B[1]="";static char _tmp10C[8]="*bogus*";static struct
_tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct _dyneither_ptr tag);
static struct _tuple1*Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct
_dyneither_ptr tag){struct _DynRegionHandle*_tmp100;struct Cyc_Dict_Dict*_tmp101;
struct Cyc_Toc_TocState _tmpFF=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp100=_tmpFF.dyn;_tmp101=_tmpFF.qvar_tags;{static struct _dyneither_ptr
bogus_string={_tmp10C,_tmp10C,_tmp10C + 8};static struct _tuple1 bogus_qvar={{.Loc_n={
3,0}},& bogus_string};static struct _tuple14 pair={& bogus_qvar,{_tmp10B,_tmp10B,
_tmp10B + 1}};{struct _tuple14 _tmpA6A;pair=((_tmpA6A.f1=x,((_tmpA6A.f2=tag,_tmpA6A))));}{
struct _DynRegionFrame _tmp103;struct _RegionHandle*d=_open_dynregion(& _tmp103,
_tmp100);{struct _tuple1**_tmp104=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,
struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmp101,(struct _tuple14*)& pair);if(
_tmp104 != 0){struct _tuple1*_tmp105=*_tmp104;_npop_handler(0);return _tmp105;}{
struct _tuple14*_tmpA6B;struct _tuple14*_tmp106=(_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((
_tmpA6B->f1=x,((_tmpA6B->f2=tag,_tmpA6B)))));struct _dyneither_ptr*_tmpA6E;struct
_tuple1*_tmpA6D;struct _tuple1*res=(_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D->f1=(*
x).f1,((_tmpA6D->f2=((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag),_tmpA6E)))),_tmpA6D)))));*_tmp101=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp101,(
struct _tuple14*)_tmp106,res);{struct _tuple1*_tmp107=res;_npop_handler(0);return
_tmp107;};};};_pop_dynregion(d);};};}struct _tuple17{void*f1;struct Cyc_List_List*
f2;};static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0);static void*Cyc_Toc_add_tuple_type(
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
_npop_handler(0);return _tmp117;}};}}{struct Cyc_Int_pa_struct _tmpA76;void*_tmpA75[
1];const char*_tmpA74;struct _dyneither_ptr*_tmpA73;struct _dyneither_ptr*xname=(
_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73[0]=(struct _dyneither_ptr)((_tmpA76.tag=
1,((_tmpA76.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA75[0]=& _tmpA76,
Cyc_aprintf(((_tmpA74="_tuple%d",_tag_dyneither(_tmpA74,sizeof(char),9))),
_tag_dyneither(_tmpA75,sizeof(void*),1)))))))),_tmpA73)));void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp118=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmpA79;struct Cyc_List_List*
_tmpA78;_tmp118=((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78->hd=((_tmpA79=
_cycalloc(sizeof(*_tmpA79)),((_tmpA79->name=Cyc_Absyn_fieldname(i),((_tmpA79->tq=
Cyc_Toc_mt_tq,((_tmpA79->type=(void*)ts2->hd,((_tmpA79->width=0,((_tmpA79->attributes=
0,_tmpA79)))))))))))),((_tmpA78->tl=_tmp118,_tmpA78))))));}}_tmp118=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp118);{struct Cyc_Absyn_AggrdeclImpl*
_tmpA7E;struct _tuple1*_tmpA7D;struct Cyc_Absyn_Aggrdecl*_tmpA7C;struct Cyc_Absyn_Aggrdecl*
_tmp11B=(_tmpA7C=_cycalloc(sizeof(*_tmpA7C)),((_tmpA7C->kind=Cyc_Absyn_StructA,((
_tmpA7C->sc=Cyc_Absyn_Public,((_tmpA7C->name=((_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((
_tmpA7D->f1=Cyc_Absyn_Rel_n(0),((_tmpA7D->f2=xname,_tmpA7D)))))),((_tmpA7C->tvs=
0,((_tmpA7C->impl=((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E->exist_vars=0,((
_tmpA7E->rgn_po=0,((_tmpA7E->fields=_tmp118,((_tmpA7E->tagged=0,_tmpA7E)))))))))),((
_tmpA7C->attributes=0,_tmpA7C)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*
_tmpA84;struct Cyc_Absyn_Aggr_d_struct _tmpA83;struct Cyc_List_List*_tmpA82;Cyc_Toc_result_decls=((
_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82->hd=Cyc_Absyn_new_decl((void*)((
_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84[0]=((_tmpA83.tag=6,((_tmpA83.f1=
_tmp11B,_tmpA83)))),_tmpA84)))),0),((_tmpA82->tl=Cyc_Toc_result_decls,_tmpA82))))));}{
struct _tuple17*_tmpA87;struct Cyc_List_List*_tmpA86;*_tmp10F=((_tmpA86=
_region_malloc(d,sizeof(*_tmpA86)),((_tmpA86->hd=((_tmpA87=_region_malloc(d,
sizeof(*_tmpA87)),((_tmpA87->f1=x,((_tmpA87->f2=ts,_tmpA87)))))),((_tmpA86->tl=*
_tmp10F,_tmpA86))))));}{void*_tmp121=x;_npop_handler(0);return _tmp121;};};};;
_pop_dynregion(d);};}struct _tuple1*Cyc_Toc_temp_var();struct _tuple1*Cyc_Toc_temp_var(){
int _tmp129=Cyc_Toc_temp_var_counter ++;struct _dyneither_ptr*_tmpA94;const char*
_tmpA93;void*_tmpA92[1];struct Cyc_Int_pa_struct _tmpA91;struct _tuple1*_tmpA90;
struct _tuple1*res=(_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->f1=Cyc_Absyn_Loc_n,((
_tmpA90->f2=((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94[0]=(struct
_dyneither_ptr)((_tmpA91.tag=1,((_tmpA91.f1=(unsigned int)_tmp129,((_tmpA92[0]=&
_tmpA91,Cyc_aprintf(((_tmpA93="_tmp%X",_tag_dyneither(_tmpA93,sizeof(char),7))),
_tag_dyneither(_tmpA92,sizeof(void*),1)))))))),_tmpA94)))),_tmpA90)))));return
res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label();static struct _dyneither_ptr*
Cyc_Toc_fresh_label(){struct _DynRegionHandle*_tmp130;struct Cyc_Xarray_Xarray*
_tmp131;struct Cyc_Toc_TocState _tmp12F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp130=_tmp12F.dyn;_tmp131=_tmp12F.temp_labels;{struct _DynRegionFrame _tmp132;
struct _RegionHandle*d=_open_dynregion(& _tmp132,_tmp130);{int _tmp133=Cyc_Toc_fresh_label_counter
++;if(_tmp133 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp131)){
struct _dyneither_ptr*_tmp134=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp131,_tmp133);_npop_handler(0);return _tmp134;}{struct Cyc_Int_pa_struct
_tmpA9C;void*_tmpA9B[1];const char*_tmpA9A;struct _dyneither_ptr*_tmpA99;struct
_dyneither_ptr*res=(_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99[0]=(struct
_dyneither_ptr)((_tmpA9C.tag=1,((_tmpA9C.f1=(unsigned int)_tmp133,((_tmpA9B[0]=&
_tmpA9C,Cyc_aprintf(((_tmpA9A="_LL%X",_tag_dyneither(_tmpA9A,sizeof(char),6))),
_tag_dyneither(_tmpA9B,sizeof(void*),1)))))))),_tmpA99)));if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp131,res)!= _tmp133){const char*
_tmpA9F;void*_tmpA9E;(_tmpA9E=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9F="fresh_label: add_ind returned bad index...",
_tag_dyneither(_tmpA9F,sizeof(char),43))),_tag_dyneither(_tmpA9E,sizeof(void*),0)));}{
struct _dyneither_ptr*_tmp137=res;_npop_handler(0);return _tmp137;};};};
_pop_dynregion(d);};}static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple1*name);static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*
td,struct _tuple1*name){int ans=0;struct Cyc_List_List*_tmp13C=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(name,((
struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp13C))->hd)->name)
!= 0){++ ans;_tmp13C=_tmp13C->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple8*Cyc_Toc_arg_to_c(struct
_tuple8*a);static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a){struct Cyc_Core_Opt*
_tmp13E;struct Cyc_Absyn_Tqual _tmp13F;void*_tmp140;struct _tuple8 _tmp13D=*a;
_tmp13E=_tmp13D.f1;_tmp13F=_tmp13D.f2;_tmp140=_tmp13D.f3;{struct _tuple8*_tmpAA0;
return(_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0->f1=_tmp13E,((_tmpAA0->f2=
_tmp13F,((_tmpAA0->f3=Cyc_Toc_typ_to_c(_tmp140),_tmpAA0)))))));};}static struct
_tuple10*Cyc_Toc_typ_to_c_f(struct _tuple10*x);static struct _tuple10*Cyc_Toc_typ_to_c_f(
struct _tuple10*x){struct Cyc_Absyn_Tqual _tmp143;void*_tmp144;struct _tuple10
_tmp142=*x;_tmp143=_tmp142.f1;_tmp144=_tmp142.f2;{struct _tuple10*_tmpAA1;return(
_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->f1=_tmp143,((_tmpAA1->f2=Cyc_Toc_typ_to_c(
_tmp144),_tmpAA1)))));};}static void*Cyc_Toc_typ_to_c_array(void*t);static void*
Cyc_Toc_typ_to_c_array(void*t){void*_tmp146=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo
_tmp148;void*_tmp149;struct Cyc_Absyn_Tqual _tmp14A;struct Cyc_Absyn_Exp*_tmp14B;
union Cyc_Absyn_Constraint*_tmp14C;struct Cyc_Position_Segment*_tmp14D;struct Cyc_Core_Opt*
_tmp14F;struct Cyc_Core_Opt _tmp150;void*_tmp151;_LL37: {struct Cyc_Absyn_ArrayType_struct*
_tmp147=(struct Cyc_Absyn_ArrayType_struct*)_tmp146;if(_tmp147->tag != 9)goto _LL39;
else{_tmp148=_tmp147->f1;_tmp149=_tmp148.elt_type;_tmp14A=_tmp148.tq;_tmp14B=
_tmp148.num_elts;_tmp14C=_tmp148.zero_term;_tmp14D=_tmp148.zt_loc;}}_LL38: return
Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp149),_tmp14A,_tmp14B,Cyc_Absyn_false_conref,
_tmp14D);_LL39: {struct Cyc_Absyn_Evar_struct*_tmp14E=(struct Cyc_Absyn_Evar_struct*)
_tmp146;if(_tmp14E->tag != 1)goto _LL3B;else{_tmp14F=_tmp14E->f2;if(_tmp14F == 0)
goto _LL3B;_tmp150=*_tmp14F;_tmp151=(void*)_tmp150.v;}}_LL3A: return Cyc_Toc_typ_to_c_array(
_tmp151);_LL3B:;_LL3C: return Cyc_Toc_typ_to_c(t);_LL36:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmpAA2;return(_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2->name=f->name,((
_tmpAA2->tq=Cyc_Toc_mt_tq,((_tmpAA2->type=Cyc_Toc_typ_to_c(f->type),((_tmpAA2->width=
f->width,((_tmpAA2->attributes=f->attributes,_tmpAA2)))))))))));}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
return;}static void*Cyc_Toc_char_star_typ();static void*Cyc_Toc_char_star_typ(){
static void**cs=0;if(cs == 0){void**_tmpAA3;cs=((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((
_tmpAA3[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAA3))));}return*cs;}static void*Cyc_Toc_rgn_typ();
static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0){void**_tmpAA4;r=((_tmpAA4=
_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq),_tmpAA4))));}return*r;}static void*Cyc_Toc_dyn_rgn_typ();static
void*Cyc_Toc_dyn_rgn_typ(){static void**r=0;if(r == 0){void**_tmpAA5;r=((_tmpAA5=
_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),
Cyc_Toc_mt_tq),_tmpAA5))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static
void*Cyc_Toc_typ_to_c(void*t){void*_tmp156=t;struct Cyc_Core_Opt*_tmp159;struct
Cyc_Core_Opt*_tmp15B;struct Cyc_Core_Opt _tmp15C;void*_tmp15D;struct Cyc_Absyn_Tvar*
_tmp15F;struct Cyc_Absyn_DatatypeFieldInfo _tmp162;union Cyc_Absyn_DatatypeFieldInfoU
_tmp163;struct _tuple2 _tmp164;struct Cyc_Absyn_Datatypedecl*_tmp165;struct Cyc_Absyn_Datatypefield*
_tmp166;struct Cyc_Absyn_PtrInfo _tmp169;void*_tmp16A;struct Cyc_Absyn_Tqual _tmp16B;
struct Cyc_Absyn_PtrAtts _tmp16C;union Cyc_Absyn_Constraint*_tmp16D;struct Cyc_Absyn_ArrayInfo
_tmp172;void*_tmp173;struct Cyc_Absyn_Tqual _tmp174;struct Cyc_Absyn_Exp*_tmp175;
struct Cyc_Position_Segment*_tmp176;struct Cyc_Absyn_FnInfo _tmp178;void*_tmp179;
struct Cyc_List_List*_tmp17A;int _tmp17B;struct Cyc_Absyn_VarargInfo*_tmp17C;struct
Cyc_List_List*_tmp17D;struct Cyc_List_List*_tmp17F;enum Cyc_Absyn_AggrKind _tmp181;
struct Cyc_List_List*_tmp182;struct Cyc_Absyn_AggrInfo _tmp184;union Cyc_Absyn_AggrInfoU
_tmp185;struct Cyc_List_List*_tmp186;struct _tuple1*_tmp188;struct Cyc_List_List*
_tmp18A;struct _tuple1*_tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_Absyn_Typedefdecl*
_tmp18E;void**_tmp18F;void*_tmp192;_LL3E: {struct Cyc_Absyn_VoidType_struct*
_tmp157=(struct Cyc_Absyn_VoidType_struct*)_tmp156;if(_tmp157->tag != 0)goto _LL40;}
_LL3F: return t;_LL40: {struct Cyc_Absyn_Evar_struct*_tmp158=(struct Cyc_Absyn_Evar_struct*)
_tmp156;if(_tmp158->tag != 1)goto _LL42;else{_tmp159=_tmp158->f2;if(_tmp159 != 0)
goto _LL42;}}_LL41: return Cyc_Absyn_sint_typ;_LL42: {struct Cyc_Absyn_Evar_struct*
_tmp15A=(struct Cyc_Absyn_Evar_struct*)_tmp156;if(_tmp15A->tag != 1)goto _LL44;
else{_tmp15B=_tmp15A->f2;if(_tmp15B == 0)goto _LL44;_tmp15C=*_tmp15B;_tmp15D=(void*)
_tmp15C.v;}}_LL43: return Cyc_Toc_typ_to_c(_tmp15D);_LL44: {struct Cyc_Absyn_VarType_struct*
_tmp15E=(struct Cyc_Absyn_VarType_struct*)_tmp156;if(_tmp15E->tag != 2)goto _LL46;
else{_tmp15F=_tmp15E->f1;}}_LL45: if(Cyc_Tcutil_tvar_kind(_tmp15F,Cyc_Absyn_BoxKind)
== Cyc_Absyn_AnyKind)return(void*)& Cyc_Absyn_VoidType_val;else{return Cyc_Absyn_void_star_typ();}
_LL46: {struct Cyc_Absyn_DatatypeType_struct*_tmp160=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp156;if(_tmp160->tag != 3)goto _LL48;}_LL47: return(void*)& Cyc_Absyn_VoidType_val;
_LL48: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp161=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp156;if(_tmp161->tag != 4)goto _LL4A;else{_tmp162=_tmp161->f1;_tmp163=_tmp162.field_info;
if((_tmp163.KnownDatatypefield).tag != 2)goto _LL4A;_tmp164=(struct _tuple2)(
_tmp163.KnownDatatypefield).val;_tmp165=_tmp164.f1;_tmp166=_tmp164.f2;}}_LL49: {
const char*_tmpAA6;return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp166->name,((
_tmpAA6="_struct",_tag_dyneither(_tmpAA6,sizeof(char),8)))));}_LL4A: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp167=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp156;if(_tmp167->tag != 4)
goto _LL4C;}_LL4B: {const char*_tmpAA9;void*_tmpAA8;(_tmpAA8=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA9="unresolved DatatypeFieldType",
_tag_dyneither(_tmpAA9,sizeof(char),29))),_tag_dyneither(_tmpAA8,sizeof(void*),0)));}
_LL4C: {struct Cyc_Absyn_PointerType_struct*_tmp168=(struct Cyc_Absyn_PointerType_struct*)
_tmp156;if(_tmp168->tag != 5)goto _LL4E;else{_tmp169=_tmp168->f1;_tmp16A=_tmp169.elt_typ;
_tmp16B=_tmp169.elt_tq;_tmp16C=_tmp169.ptr_atts;_tmp16D=_tmp16C.bounds;}}_LL4D:
_tmp16A=Cyc_Toc_typ_to_c_array(_tmp16A);{void*_tmp19D=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp16D);_LL77: {
struct Cyc_Absyn_DynEither_b_struct*_tmp19E=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp19D;if(_tmp19E->tag != 0)goto _LL79;}_LL78: return Cyc_Toc_dyneither_ptr_typ;
_LL79:;_LL7A: return Cyc_Absyn_star_typ(_tmp16A,(void*)& Cyc_Absyn_HeapRgn_val,
_tmp16B,Cyc_Absyn_false_conref);_LL76:;};_LL4E: {struct Cyc_Absyn_IntType_struct*
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
goto _LL84;}_LL83: continue;_LL84:;_LL85:{struct Cyc_List_List*_tmpAAA;_tmp19F=((
_tmpAAA=_cycalloc(sizeof(*_tmpAAA)),((_tmpAAA->hd=(void*)_tmp17D->hd,((_tmpAAA->tl=
_tmp19F,_tmpAAA))))));}goto _LL7B;_LL7B:;}{struct Cyc_List_List*_tmp1A6=((struct
Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp17A);if(_tmp17C != 0){void*_tmp1A7=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp17C->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple8*_tmpAAB;struct _tuple8*_tmp1A8=(_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((
_tmpAAB->f1=_tmp17C->name,((_tmpAAB->f2=_tmp17C->tq,((_tmpAAB->f3=_tmp1A7,
_tmpAAB)))))));struct Cyc_List_List*_tmpAAC;_tmp1A6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1A6,((
_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->hd=_tmp1A8,((_tmpAAC->tl=0,
_tmpAAC)))))));}{struct Cyc_Absyn_FnType_struct _tmpAB2;struct Cyc_Absyn_FnInfo
_tmpAB1;struct Cyc_Absyn_FnType_struct*_tmpAB0;return(void*)((_tmpAB0=_cycalloc(
sizeof(*_tmpAB0)),((_tmpAB0[0]=((_tmpAB2.tag=10,((_tmpAB2.f1=((_tmpAB1.tvars=0,((
_tmpAB1.effect=0,((_tmpAB1.ret_typ=Cyc_Toc_typ_to_c(_tmp179),((_tmpAB1.args=
_tmp1A6,((_tmpAB1.c_varargs=_tmp17B,((_tmpAB1.cyc_varargs=0,((_tmpAB1.rgn_po=0,((
_tmpAB1.attributes=_tmp19F,_tmpAB1)))))))))))))))),_tmpAB2)))),_tmpAB0))));};};}
_LL58: {struct Cyc_Absyn_TupleType_struct*_tmp17E=(struct Cyc_Absyn_TupleType_struct*)
_tmp156;if(_tmp17E->tag != 11)goto _LL5A;else{_tmp17F=_tmp17E->f1;}}_LL59: _tmp17F=((
struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp17F);return Cyc_Toc_add_tuple_type(
_tmp17F);_LL5A: {struct Cyc_Absyn_AnonAggrType_struct*_tmp180=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp156;if(_tmp180->tag != 13)goto _LL5C;else{_tmp181=_tmp180->f1;_tmp182=_tmp180->f2;}}
_LL5B: {struct Cyc_Absyn_AnonAggrType_struct _tmpAB5;struct Cyc_Absyn_AnonAggrType_struct*
_tmpAB4;return(void*)((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((
_tmpAB5.tag=13,((_tmpAB5.f1=_tmp181,((_tmpAB5.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp182),_tmpAB5)))))),_tmpAB4))));}_LL5C: {
struct Cyc_Absyn_AggrType_struct*_tmp183=(struct Cyc_Absyn_AggrType_struct*)
_tmp156;if(_tmp183->tag != 12)goto _LL5E;else{_tmp184=_tmp183->f1;_tmp185=_tmp184.aggr_info;
_tmp186=_tmp184.targs;}}_LL5D:{union Cyc_Absyn_AggrInfoU _tmp1B0=_tmp185;struct
_tuple3 _tmp1B1;_LL87: if((_tmp1B0.UnknownAggr).tag != 1)goto _LL89;_tmp1B1=(struct
_tuple3)(_tmp1B0.UnknownAggr).val;_LL88: return t;_LL89:;_LL8A: goto _LL86;_LL86:;}{
struct Cyc_Absyn_Aggrdecl*_tmp1B2=Cyc_Absyn_get_known_aggrdecl(_tmp185);if(
_tmp1B2->kind == Cyc_Absyn_UnionA)return Cyc_Toc_aggrdecl_type(_tmp1B2->name,Cyc_Absyn_unionq_typ);
else{return Cyc_Toc_aggrdecl_type(_tmp1B2->name,Cyc_Absyn_strctq);}};_LL5E: {
struct Cyc_Absyn_EnumType_struct*_tmp187=(struct Cyc_Absyn_EnumType_struct*)
_tmp156;if(_tmp187->tag != 14)goto _LL60;else{_tmp188=_tmp187->f1;}}_LL5F: return t;
_LL60: {struct Cyc_Absyn_AnonEnumType_struct*_tmp189=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp156;if(_tmp189->tag != 15)goto _LL62;else{_tmp18A=_tmp189->f1;}}_LL61: Cyc_Toc_enumfields_to_c(
_tmp18A);return t;_LL62: {struct Cyc_Absyn_TypedefType_struct*_tmp18B=(struct Cyc_Absyn_TypedefType_struct*)
_tmp156;if(_tmp18B->tag != 18)goto _LL64;else{_tmp18C=_tmp18B->f1;_tmp18D=_tmp18B->f2;
_tmp18E=_tmp18B->f3;_tmp18F=_tmp18B->f4;}}_LL63: if(_tmp18F == 0  || Cyc_noexpand_r){
if(_tmp18D != 0){struct Cyc_Absyn_TypedefType_struct _tmpAB8;struct Cyc_Absyn_TypedefType_struct*
_tmpAB7;return(void*)((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7[0]=((
_tmpAB8.tag=18,((_tmpAB8.f1=_tmp18C,((_tmpAB8.f2=0,((_tmpAB8.f3=_tmp18E,((
_tmpAB8.f4=0,_tmpAB8)))))))))),_tmpAB7))));}else{return t;}}else{struct Cyc_Absyn_TypedefType_struct
_tmpABE;void**_tmpABD;struct Cyc_Absyn_TypedefType_struct*_tmpABC;return(void*)((
_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=((_tmpABE.tag=18,((_tmpABE.f1=
_tmp18C,((_tmpABE.f2=0,((_tmpABE.f3=_tmp18E,((_tmpABE.f4=((_tmpABD=_cycalloc(
sizeof(*_tmpABD)),((_tmpABD[0]=Cyc_Toc_typ_to_c_array(*_tmp18F),_tmpABD)))),
_tmpABE)))))))))),_tmpABC))));}_LL64: {struct Cyc_Absyn_TagType_struct*_tmp190=(
struct Cyc_Absyn_TagType_struct*)_tmp156;if(_tmp190->tag != 20)goto _LL66;}_LL65:
return Cyc_Absyn_uint_typ;_LL66: {struct Cyc_Absyn_RgnHandleType_struct*_tmp191=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp156;if(_tmp191->tag != 16)goto _LL68;
else{_tmp192=(void*)_tmp191->f1;}}_LL67: return Cyc_Toc_rgn_typ();_LL68: {struct
Cyc_Absyn_DynRgnType_struct*_tmp193=(struct Cyc_Absyn_DynRgnType_struct*)_tmp156;
if(_tmp193->tag != 17)goto _LL6A;}_LL69: return Cyc_Toc_dyn_rgn_typ();_LL6A: {struct
Cyc_Absyn_HeapRgn_struct*_tmp194=(struct Cyc_Absyn_HeapRgn_struct*)_tmp156;if(
_tmp194->tag != 21)goto _LL6C;}_LL6B: {const char*_tmpAC1;void*_tmpAC0;(_tmpAC0=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC1="Toc::typ_to_c: type translation passed the heap region",_tag_dyneither(
_tmpAC1,sizeof(char),55))),_tag_dyneither(_tmpAC0,sizeof(void*),0)));}_LL6C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp195=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp156;if(_tmp195->tag != 22)goto _LL6E;}_LL6D: {const char*_tmpAC4;void*_tmpAC3;(
_tmpAC3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC4="Toc::typ_to_c: type translation passed the unique region",
_tag_dyneither(_tmpAC4,sizeof(char),57))),_tag_dyneither(_tmpAC3,sizeof(void*),0)));}
_LL6E: {struct Cyc_Absyn_AccessEff_struct*_tmp196=(struct Cyc_Absyn_AccessEff_struct*)
_tmp156;if(_tmp196->tag != 23)goto _LL70;}_LL6F: goto _LL71;_LL70: {struct Cyc_Absyn_JoinEff_struct*
_tmp197=(struct Cyc_Absyn_JoinEff_struct*)_tmp156;if(_tmp197->tag != 24)goto _LL72;}
_LL71: goto _LL73;_LL72: {struct Cyc_Absyn_RgnsEff_struct*_tmp198=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp156;if(_tmp198->tag != 25)goto _LL74;}_LL73: {const char*_tmpAC7;void*_tmpAC6;(
_tmpAC6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAC7="Toc::typ_to_c: type translation passed an effect",_tag_dyneither(
_tmpAC7,sizeof(char),49))),_tag_dyneither(_tmpAC6,sizeof(void*),0)));}_LL74: {
struct Cyc_Absyn_ValueofType_struct*_tmp199=(struct Cyc_Absyn_ValueofType_struct*)
_tmp156;if(_tmp199->tag != 19)goto _LL3D;}_LL75: {const char*_tmpACA;void*_tmpAC9;(
_tmpAC9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpACA="Toc::typ_to_c: type translation passed a valueof_t",_tag_dyneither(
_tmpACA,sizeof(char),51))),_tag_dyneither(_tmpAC9,sizeof(void*),0)));}_LL3D:;}
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
_tmp1E6->hd)->type))return 0;}}return 1;};_LLA7: {struct Cyc_Absyn_AnonAggrType_struct*
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
_LLB5;}_LLB4: return 0;_LLB5:;_LLB6: {const char*_tmpACE;void*_tmpACD[1];struct Cyc_String_pa_struct
_tmpACC;(_tmpACC.tag=0,((_tmpACC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpACD[0]=& _tmpACC,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpACE="atomic_typ:  bad type %s",
_tag_dyneither(_tmpACE,sizeof(char),25))),_tag_dyneither(_tmpACD,sizeof(void*),1)))))));}
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
if(_tmp1F8->impl == 0){const char*_tmpAD1;void*_tmpAD0;(_tmpAD0=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD1="is_poly_field: type missing fields",
_tag_dyneither(_tmpAD1,sizeof(char),35))),_tag_dyneither(_tmpAD0,sizeof(void*),0)));}
_tmp1F7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F8->impl))->fields;goto
_LLCF;}_LLCE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1F6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1F2;if(_tmp1F6->tag != 13)goto _LLD0;else{_tmp1F7=_tmp1F6->f2;}}_LLCF: {struct
Cyc_Absyn_Aggrfield*_tmp1FB=Cyc_Absyn_lookup_field(_tmp1F7,f);if(_tmp1FB == 0){
const char*_tmpAD5;void*_tmpAD4[1];struct Cyc_String_pa_struct _tmpAD3;(_tmpAD3.tag=
0,((_tmpAD3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpAD4[0]=&
_tmpAD3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpAD5="is_poly_field: bad field %s",_tag_dyneither(_tmpAD5,sizeof(char),28))),
_tag_dyneither(_tmpAD4,sizeof(void*),1)))))));}return Cyc_Toc_is_void_star_or_tvar(
_tmp1FB->type);}_LLD0:;_LLD1: {const char*_tmpAD9;void*_tmpAD8[1];struct Cyc_String_pa_struct
_tmpAD7;(_tmpAD7.tag=0,((_tmpAD7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpAD8[0]=& _tmpAD7,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAD9="is_poly_field: bad type %s",
_tag_dyneither(_tmpAD9,sizeof(char),27))),_tag_dyneither(_tmpAD8,sizeof(void*),1)))))));}
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
_tmp208);_LLDC:;_LLDD: {const char*_tmpADD;void*_tmpADC[1];struct Cyc_String_pa_struct
_tmpADB;(_tmpADB.tag=0,((_tmpADB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp207->topt))->v)),((
_tmpADC[0]=& _tmpADB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpADD="is_poly_project: bad type %s",_tag_dyneither(_tmpADD,sizeof(char),29))),
_tag_dyneither(_tmpADC,sizeof(void*),1)))))));}_LLD9:;}_LLD7:;_LLD8: return 0;
_LLD2:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmpADE;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpADE=_cycalloc(
sizeof(*_tmpADE)),((_tmpADE->hd=s,((_tmpADE->tl=0,_tmpADE)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmpADF;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpADF=_cycalloc(sizeof(*
_tmpADF)),((_tmpADF->hd=s,((_tmpADF->tl=0,_tmpADF)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmpAE0[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpAE0[1]=s,((_tmpAE0[0]=
rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAE0,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmpAE1[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpAE1[1]=n,((
_tmpAE1[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAE1,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmpAE2[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpAE2[1]=n,((
_tmpAE2[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAE2,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct
Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmpAE3[3];return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,((_tmpAE3[2]=n,((_tmpAE3[1]=s,((_tmpAE3[0]=rgn,((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAE3,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*
e){struct Cyc_List_List*_tmpAE4;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((
_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4->hd=e,((_tmpAE4->tl=0,_tmpAE4)))))),
0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*
Cyc_Toc_throw_match_stmt(){static struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;
if(throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpAE5;throw_match_stmt_opt=((
_tmpAE5=_cycalloc(sizeof(*_tmpAE5)),((_tmpAE5[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmpAE5))));}return*throw_match_stmt_opt;}static
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
x,vd_typ,(struct Cyc_Absyn_Exp*)e);{struct Cyc_Absyn_Var_d_struct*_tmpAEB;struct
Cyc_Absyn_Var_d_struct _tmpAEA;struct Cyc_List_List*_tmpAE9;Cyc_Toc_result_decls=((
_tmpAE9=_cycalloc(sizeof(*_tmpAE9)),((_tmpAE9->hd=Cyc_Absyn_new_decl((void*)((
_tmpAEB=_cycalloc(sizeof(*_tmpAEB)),((_tmpAEB[0]=((_tmpAEA.tag=0,((_tmpAEA.f1=vd,
_tmpAEA)))),_tmpAEB)))),0),((_tmpAE9->tl=Cyc_Toc_result_decls,_tmpAE9))))));}
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(
Cyc_Absyn_void_star_typ(),e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0));}{struct Cyc_Absyn_Exp*urm_exp;{struct _tuple16*_tmpAF2;
struct _tuple16*_tmpAF1;struct _tuple16*_tmpAF0;struct _tuple16*_tmpAEF[3];urm_exp=
Cyc_Absyn_unresolvedmem_exp(0,((_tmpAEF[2]=((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((
_tmpAF0->f1=0,((_tmpAF0->f2=xplussz,_tmpAF0)))))),((_tmpAEF[1]=((_tmpAF1=
_cycalloc(sizeof(*_tmpAF1)),((_tmpAF1->f1=0,((_tmpAF1->f2=xexp,_tmpAF1)))))),((
_tmpAEF[0]=((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2->f1=0,((_tmpAF2->f2=
xexp,_tmpAF2)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpAEF,sizeof(struct _tuple16*),3)))))))),0);}return urm_exp;};};}
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
struct _RegionHandle*r){struct Cyc_Toc_Env*_tmpAF3;return(_tmpAF3=_region_malloc(r,
sizeof(*_tmpAF3)),((_tmpAF3->break_lab=(struct _dyneither_ptr**)0,((_tmpAF3->continue_lab=(
struct _dyneither_ptr**)0,((_tmpAF3->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((
_tmpAF3->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((
_tmpAF3->toplevel=(int)1,((_tmpAF3->rgn=(struct _RegionHandle*)r,_tmpAF3)))))))))))));}
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp22B;struct _dyneither_ptr**_tmp22C;struct _dyneither_ptr**
_tmp22D;struct Cyc_Toc_FallthruInfo*_tmp22E;struct Cyc_Dict_Dict _tmp22F;int _tmp230;
struct Cyc_Toc_Env*_tmp22A=e;_tmp22B=*_tmp22A;_tmp22C=_tmp22B.break_lab;_tmp22D=
_tmp22B.continue_lab;_tmp22E=_tmp22B.fallthru_info;_tmp22F=_tmp22B.varmap;
_tmp230=_tmp22B.toplevel;{struct Cyc_Toc_Env*_tmpAF4;return(_tmpAF4=
_region_malloc(r,sizeof(*_tmpAF4)),((_tmpAF4->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22C),((_tmpAF4->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp22D),((_tmpAF4->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp22E,((_tmpAF4->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp22F),((_tmpAF4->toplevel=(
int)_tmp230,((_tmpAF4->rgn=(struct _RegionHandle*)r,_tmpAF4)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e);static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct
Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp233;struct _dyneither_ptr**_tmp234;struct
_dyneither_ptr**_tmp235;struct Cyc_Toc_FallthruInfo*_tmp236;struct Cyc_Dict_Dict
_tmp237;int _tmp238;struct Cyc_Toc_Env*_tmp232=e;_tmp233=*_tmp232;_tmp234=_tmp233.break_lab;
_tmp235=_tmp233.continue_lab;_tmp236=_tmp233.fallthru_info;_tmp237=_tmp233.varmap;
_tmp238=_tmp233.toplevel;{struct Cyc_Toc_Env*_tmpAF5;return(_tmpAF5=
_region_malloc(r,sizeof(*_tmpAF5)),((_tmpAF5->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp234),((_tmpAF5->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp235),((_tmpAF5->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp236,((_tmpAF5->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp237),((_tmpAF5->toplevel=(
int)0,((_tmpAF5->rgn=(struct _RegionHandle*)r,_tmpAF5)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp23B;struct _dyneither_ptr**_tmp23C;struct _dyneither_ptr**
_tmp23D;struct Cyc_Toc_FallthruInfo*_tmp23E;struct Cyc_Dict_Dict _tmp23F;int _tmp240;
struct Cyc_Toc_Env*_tmp23A=e;_tmp23B=*_tmp23A;_tmp23C=_tmp23B.break_lab;_tmp23D=
_tmp23B.continue_lab;_tmp23E=_tmp23B.fallthru_info;_tmp23F=_tmp23B.varmap;
_tmp240=_tmp23B.toplevel;{struct Cyc_Toc_Env*_tmpAF6;return(_tmpAF6=
_region_malloc(r,sizeof(*_tmpAF6)),((_tmpAF6->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23C),((_tmpAF6->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23D),((_tmpAF6->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp23E,((_tmpAF6->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp23F),((_tmpAF6->toplevel=(
int)1,((_tmpAF6->rgn=(struct _RegionHandle*)r,_tmpAF6)))))))))))));};}static
struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,
struct _tuple1*x,struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace _tmp242=(*x).f1;struct Cyc_List_List*_tmp243;_LLE4: if((
_tmp242.Rel_n).tag != 1)goto _LLE6;_tmp243=(struct Cyc_List_List*)(_tmp242.Rel_n).val;
_LLE5: {const char*_tmpAFA;void*_tmpAF9[1];struct Cyc_String_pa_struct _tmpAF8;(
_tmpAF8.tag=0,((_tmpAF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x)),((_tmpAF9[0]=& _tmpAF8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpAFA="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(
_tmpAFA,sizeof(char),30))),_tag_dyneither(_tmpAF9,sizeof(void*),1)))))));}_LLE6:;
_LLE7: goto _LLE3;_LLE3:;}{struct Cyc_Toc_Env _tmp248;struct _dyneither_ptr**_tmp249;
struct _dyneither_ptr**_tmp24A;struct Cyc_Toc_FallthruInfo*_tmp24B;struct Cyc_Dict_Dict
_tmp24C;int _tmp24D;struct Cyc_Toc_Env*_tmp247=e;_tmp248=*_tmp247;_tmp249=_tmp248.break_lab;
_tmp24A=_tmp248.continue_lab;_tmp24B=_tmp248.fallthru_info;_tmp24C=_tmp248.varmap;
_tmp24D=_tmp248.toplevel;{struct Cyc_Dict_Dict _tmp24E=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24C),x,y);struct Cyc_Toc_Env*_tmpAFB;return(_tmpAFB=_region_malloc(r,
sizeof(*_tmpAFB)),((_tmpAFB->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp249),((_tmpAFB->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp24A),((_tmpAFB->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp24B,((_tmpAFB->varmap=(struct Cyc_Dict_Dict)_tmp24E,((_tmpAFB->toplevel=(int)
_tmp24D,((_tmpAFB->rgn=(struct _RegionHandle*)r,_tmpAFB)))))))))))));};};}static
struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e);
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp251;struct _dyneither_ptr**_tmp252;struct _dyneither_ptr**
_tmp253;struct Cyc_Toc_FallthruInfo*_tmp254;struct Cyc_Dict_Dict _tmp255;int _tmp256;
struct Cyc_Toc_Env*_tmp250=e;_tmp251=*_tmp250;_tmp252=_tmp251.break_lab;_tmp253=
_tmp251.continue_lab;_tmp254=_tmp251.fallthru_info;_tmp255=_tmp251.varmap;
_tmp256=_tmp251.toplevel;{struct Cyc_Toc_Env*_tmpAFC;return(_tmpAFC=
_region_malloc(r,sizeof(*_tmpAFC)),((_tmpAFC->break_lab=(struct _dyneither_ptr**)
0,((_tmpAFC->continue_lab=(struct _dyneither_ptr**)0,((_tmpAFC->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp254,((_tmpAFC->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp255),((_tmpAFC->toplevel=(int)_tmp256,((_tmpAFC->rgn=(struct _RegionHandle*)
r,_tmpAFC)))))))))))));};}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env);static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){struct Cyc_List_List*_tmpAFD;fallthru_vars=((
_tmpAFD=_region_malloc(r,sizeof(*_tmpAFD)),((_tmpAFD->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmpAFD->tl=fallthru_vars,_tmpAFD))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp25A;struct _dyneither_ptr**_tmp25B;struct
_dyneither_ptr**_tmp25C;struct Cyc_Toc_FallthruInfo*_tmp25D;struct Cyc_Dict_Dict
_tmp25E;int _tmp25F;struct Cyc_Toc_Env*_tmp259=e;_tmp25A=*_tmp259;_tmp25B=_tmp25A.break_lab;
_tmp25C=_tmp25A.continue_lab;_tmp25D=_tmp25A.fallthru_info;_tmp25E=_tmp25A.varmap;
_tmp25F=_tmp25A.toplevel;{struct Cyc_Toc_Env _tmp261;struct Cyc_Dict_Dict _tmp262;
struct Cyc_Toc_Env*_tmp260=next_case_env;_tmp261=*_tmp260;_tmp262=_tmp261.varmap;{
struct Cyc_Toc_FallthruInfo*_tmpAFE;struct Cyc_Toc_FallthruInfo*fi=(_tmpAFE=
_region_malloc(r,sizeof(*_tmpAFE)),((_tmpAFE->label=fallthru_l,((_tmpAFE->binders=
fallthru_vars,((_tmpAFE->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp262),_tmpAFE)))))));
struct _dyneither_ptr**_tmpB01;struct Cyc_Toc_Env*_tmpB00;return(_tmpB00=
_region_malloc(r,sizeof(*_tmpB00)),((_tmpB00->break_lab=(struct _dyneither_ptr**)((
_tmpB01=_region_malloc(r,sizeof(*_tmpB01)),((_tmpB01[0]=break_l,_tmpB01)))),((
_tmpB00->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp25C),((
_tmpB00->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB00->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp25E),((_tmpB00->toplevel=(int)_tmp25F,((_tmpB00->rgn=(
struct _RegionHandle*)r,_tmpB00)))))))))))));};};};}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l);static
struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*
e,struct _dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp267;struct _dyneither_ptr**
_tmp268;struct _dyneither_ptr**_tmp269;struct Cyc_Toc_FallthruInfo*_tmp26A;struct
Cyc_Dict_Dict _tmp26B;int _tmp26C;struct Cyc_Toc_Env*_tmp266=e;_tmp267=*_tmp266;
_tmp268=_tmp267.break_lab;_tmp269=_tmp267.continue_lab;_tmp26A=_tmp267.fallthru_info;
_tmp26B=_tmp267.varmap;_tmp26C=_tmp267.toplevel;{struct _dyneither_ptr**_tmpB04;
struct Cyc_Toc_Env*_tmpB03;return(_tmpB03=_region_malloc(r,sizeof(*_tmpB03)),((
_tmpB03->break_lab=(struct _dyneither_ptr**)((_tmpB04=_region_malloc(r,sizeof(*
_tmpB04)),((_tmpB04[0]=break_l,_tmpB04)))),((_tmpB03->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp269),((_tmpB03->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0,((_tmpB03->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp26B),((_tmpB03->toplevel=(
int)_tmp26C,((_tmpB03->rgn=(struct _RegionHandle*)r,_tmpB03)))))))))))));};}
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp270;struct _dyneither_ptr**_tmp271;struct _dyneither_ptr**_tmp272;struct Cyc_Toc_FallthruInfo*
_tmp273;struct Cyc_Dict_Dict _tmp274;int _tmp275;struct Cyc_Toc_Env*_tmp26F=e;
_tmp270=*_tmp26F;_tmp271=_tmp270.break_lab;_tmp272=_tmp270.continue_lab;_tmp273=
_tmp270.fallthru_info;_tmp274=_tmp270.varmap;_tmp275=_tmp270.toplevel;{struct Cyc_Toc_FallthruInfo*
_tmpB07;struct Cyc_Toc_Env*_tmpB06;return(_tmpB06=_region_malloc(r,sizeof(*
_tmpB06)),((_tmpB06->break_lab=(struct _dyneither_ptr**)0,((_tmpB06->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp272),((_tmpB06->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)((_tmpB07=_region_malloc(r,sizeof(*_tmpB07)),((
_tmpB07->label=next_l,((_tmpB07->binders=0,((_tmpB07->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp),_tmpB07)))))))),((_tmpB06->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp274),((_tmpB06->toplevel=(int)_tmp275,((_tmpB06->rgn=(struct _RegionHandle*)
r,_tmpB06)))))))))))));};}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_need_null_check(
struct Cyc_Absyn_Exp*e){void*_tmp278=e->annot;_LLE9: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp279=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp278;if(_tmp279->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLEB;}_LLEA: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLEB: {struct Cyc_CfFlowInfo_NotZero_struct*_tmp27A=(
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp278;if(_tmp27A->tag != Cyc_CfFlowInfo_NotZero)
goto _LLED;}_LLEC: return 0;_LLED: {struct Cyc_CfFlowInfo_IsZero_struct*_tmp27B=(
struct Cyc_CfFlowInfo_IsZero_struct*)_tmp278;if(_tmp27B->tag != Cyc_CfFlowInfo_IsZero)
goto _LLEF;}_LLEE:{const char*_tmpB0A;void*_tmpB09;(_tmpB09=0,Cyc_Tcutil_terr(e->loc,((
_tmpB0A="dereference of NULL pointer",_tag_dyneither(_tmpB0A,sizeof(char),28))),
_tag_dyneither(_tmpB09,sizeof(void*),0)));}return 0;_LLEF: {struct Cyc_Absyn_EmptyAnnot_struct*
_tmp27C=(struct Cyc_Absyn_EmptyAnnot_struct*)_tmp278;if(_tmp27C->tag != Cyc_Absyn_EmptyAnnot)
goto _LLF1;}_LLF0: return 0;_LLF1: {struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp27D=(
struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp278;if(_tmp27D->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLF3;}_LLF2:{const char*_tmpB0D;void*_tmpB0C;(_tmpB0C=0,Cyc_Tcutil_warn(e->loc,((
_tmpB0D="compiler oddity: pointer compared to tag type",_tag_dyneither(_tmpB0D,
sizeof(char),46))),_tag_dyneither(_tmpB0C,sizeof(void*),0)));}return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLF3:;_LLF4: {const char*
_tmpB10;void*_tmpB0F;(_tmpB0F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB10="need_null_check",_tag_dyneither(
_tmpB10,sizeof(char),16))),_tag_dyneither(_tmpB0F,sizeof(void*),0)));}_LLE8:;}
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e);static struct
Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*_tmp284=e->annot;
struct Cyc_List_List*_tmp286;struct Cyc_List_List*_tmp288;_LLF6: {struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp285=(struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp284;if(_tmp285->tag != Cyc_CfFlowInfo_UnknownZ)
goto _LLF8;else{_tmp286=_tmp285->f1;}}_LLF7: return _tmp286;_LLF8: {struct Cyc_CfFlowInfo_NotZero_struct*
_tmp287=(struct Cyc_CfFlowInfo_NotZero_struct*)_tmp284;if(_tmp287->tag != Cyc_CfFlowInfo_NotZero)
goto _LLFA;else{_tmp288=_tmp287->f1;}}_LLF9: return _tmp288;_LLFA: {struct Cyc_CfFlowInfo_IsZero_struct*
_tmp289=(struct Cyc_CfFlowInfo_IsZero_struct*)_tmp284;if(_tmp289->tag != Cyc_CfFlowInfo_IsZero)
goto _LLFC;}_LLFB:{const char*_tmpB13;void*_tmpB12;(_tmpB12=0,Cyc_Tcutil_terr(e->loc,((
_tmpB13="dereference of NULL pointer",_tag_dyneither(_tmpB13,sizeof(char),28))),
_tag_dyneither(_tmpB12,sizeof(void*),0)));}return 0;_LLFC: {struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp28A=(struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp284;if(_tmp28A->tag != Cyc_CfFlowInfo_HasTagCmps)
goto _LLFE;}_LLFD: goto _LLFF;_LLFE: {struct Cyc_Absyn_EmptyAnnot_struct*_tmp28B=(
struct Cyc_Absyn_EmptyAnnot_struct*)_tmp284;if(_tmp28B->tag != Cyc_Absyn_EmptyAnnot)
goto _LL100;}_LLFF: return 0;_LL100:;_LL101: {const char*_tmpB16;void*_tmpB15;(
_tmpB15=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB16="get_relns",_tag_dyneither(_tmpB16,sizeof(char),10))),_tag_dyneither(
_tmpB15,sizeof(void*),0)));}_LLF5:;}static int Cyc_Toc_check_const_array(
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
_tmp2A2  && i <= _tmp2A1;};_LL107:;_LL108: return 0;_LL102:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y);
static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*
_tmp2A3=(struct Cyc_CfFlowInfo_Reln*)relns->hd;if(_tmp2A3->vd != y)continue;{union
Cyc_CfFlowInfo_RelnOp _tmp2A4=_tmp2A3->rop;struct Cyc_Absyn_Vardecl*_tmp2A5;struct
Cyc_Absyn_Vardecl*_tmp2A6;_LL10F: if((_tmp2A4.LessNumelts).tag != 3)goto _LL111;
_tmp2A5=(struct Cyc_Absyn_Vardecl*)(_tmp2A4.LessNumelts).val;_LL110: _tmp2A6=
_tmp2A5;goto _LL112;_LL111: if((_tmp2A4.LessEqNumelts).tag != 5)goto _LL113;_tmp2A6=(
struct Cyc_Absyn_Vardecl*)(_tmp2A4.LessEqNumelts).val;_LL112: if(_tmp2A6 == v)
return 1;else{goto _LL10E;}_LL113:;_LL114: continue;_LL10E:;};}return 0;}static int
Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e);static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,
struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*_tmp2A7=e->r;void*_tmp2A9;
struct Cyc_Absyn_Vardecl*_tmp2AB;void*_tmp2AD;struct Cyc_Absyn_Vardecl*_tmp2AF;
void*_tmp2B1;struct Cyc_Absyn_Vardecl*_tmp2B3;void*_tmp2B5;struct Cyc_Absyn_Vardecl*
_tmp2B7;enum Cyc_Absyn_Primop _tmp2B9;struct Cyc_List_List*_tmp2BA;struct Cyc_List_List
_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;_LL116: {struct Cyc_Absyn_Var_e_struct*
_tmp2A8=(struct Cyc_Absyn_Var_e_struct*)_tmp2A7;if(_tmp2A8->tag != 1)goto _LL118;
else{_tmp2A9=(void*)_tmp2A8->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp2AA=(struct Cyc_Absyn_Pat_b_struct*)
_tmp2A9;if(_tmp2AA->tag != 5)goto _LL118;else{_tmp2AB=_tmp2AA->f1;}};}}_LL117:
_tmp2AF=_tmp2AB;goto _LL119;_LL118: {struct Cyc_Absyn_Var_e_struct*_tmp2AC=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A7;if(_tmp2AC->tag != 1)goto _LL11A;else{_tmp2AD=(
void*)_tmp2AC->f2;{struct Cyc_Absyn_Local_b_struct*_tmp2AE=(struct Cyc_Absyn_Local_b_struct*)
_tmp2AD;if(_tmp2AE->tag != 4)goto _LL11A;else{_tmp2AF=_tmp2AE->f1;}};}}_LL119:
_tmp2B3=_tmp2AF;goto _LL11B;_LL11A: {struct Cyc_Absyn_Var_e_struct*_tmp2B0=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A7;if(_tmp2B0->tag != 1)goto _LL11C;else{_tmp2B1=(
void*)_tmp2B0->f2;{struct Cyc_Absyn_Global_b_struct*_tmp2B2=(struct Cyc_Absyn_Global_b_struct*)
_tmp2B1;if(_tmp2B2->tag != 1)goto _LL11C;else{_tmp2B3=_tmp2B2->f1;}};}}_LL11B:
_tmp2B7=_tmp2B3;goto _LL11D;_LL11C: {struct Cyc_Absyn_Var_e_struct*_tmp2B4=(struct
Cyc_Absyn_Var_e_struct*)_tmp2A7;if(_tmp2B4->tag != 1)goto _LL11E;else{_tmp2B5=(
void*)_tmp2B4->f2;{struct Cyc_Absyn_Param_b_struct*_tmp2B6=(struct Cyc_Absyn_Param_b_struct*)
_tmp2B5;if(_tmp2B6->tag != 3)goto _LL11E;else{_tmp2B7=_tmp2B6->f1;}};}}_LL11D: if(
_tmp2B7->escapes)return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2B7))return 1;
goto _LL115;_LL11E: {struct Cyc_Absyn_Primop_e_struct*_tmp2B8=(struct Cyc_Absyn_Primop_e_struct*)
_tmp2A7;if(_tmp2B8->tag != 3)goto _LL120;else{_tmp2B9=_tmp2B8->f1;if(_tmp2B9 != Cyc_Absyn_Numelts)
goto _LL120;_tmp2BA=_tmp2B8->f2;if(_tmp2BA == 0)goto _LL120;_tmp2BB=*_tmp2BA;
_tmp2BC=(struct Cyc_Absyn_Exp*)_tmp2BB.hd;}}_LL11F:{void*_tmp2BD=_tmp2BC->r;void*
_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C1;void*_tmp2C3;struct Cyc_Absyn_Vardecl*
_tmp2C5;void*_tmp2C7;struct Cyc_Absyn_Vardecl*_tmp2C9;void*_tmp2CB;struct Cyc_Absyn_Vardecl*
_tmp2CD;_LL123: {struct Cyc_Absyn_Var_e_struct*_tmp2BE=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2BE->tag != 1)goto _LL125;else{_tmp2BF=(void*)_tmp2BE->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2C0=(struct Cyc_Absyn_Pat_b_struct*)_tmp2BF;if(_tmp2C0->tag
!= 5)goto _LL125;else{_tmp2C1=_tmp2C0->f1;}};}}_LL124: _tmp2C5=_tmp2C1;goto _LL126;
_LL125: {struct Cyc_Absyn_Var_e_struct*_tmp2C2=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2C2->tag != 1)goto _LL127;else{_tmp2C3=(void*)_tmp2C2->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2C4=(struct Cyc_Absyn_Local_b_struct*)_tmp2C3;if(
_tmp2C4->tag != 4)goto _LL127;else{_tmp2C5=_tmp2C4->f1;}};}}_LL126: _tmp2C9=_tmp2C5;
goto _LL128;_LL127: {struct Cyc_Absyn_Var_e_struct*_tmp2C6=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2C6->tag != 1)goto _LL129;else{_tmp2C7=(void*)_tmp2C6->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2C8=(struct Cyc_Absyn_Global_b_struct*)_tmp2C7;if(
_tmp2C8->tag != 1)goto _LL129;else{_tmp2C9=_tmp2C8->f1;}};}}_LL128: _tmp2CD=_tmp2C9;
goto _LL12A;_LL129: {struct Cyc_Absyn_Var_e_struct*_tmp2CA=(struct Cyc_Absyn_Var_e_struct*)
_tmp2BD;if(_tmp2CA->tag != 1)goto _LL12B;else{_tmp2CB=(void*)_tmp2CA->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2CC=(struct Cyc_Absyn_Param_b_struct*)_tmp2CB;if(
_tmp2CC->tag != 3)goto _LL12B;else{_tmp2CD=_tmp2CC->f1;}};}}_LL12A: return _tmp2CD == 
v;_LL12B:;_LL12C: goto _LL122;_LL122:;}goto _LL115;_LL120:;_LL121: goto _LL115;_LL115:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i);static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp2CE=a->r;void*_tmp2D0;
struct Cyc_Absyn_Vardecl*_tmp2D2;void*_tmp2D4;struct Cyc_Absyn_Vardecl*_tmp2D6;
void*_tmp2D8;struct Cyc_Absyn_Vardecl*_tmp2DA;void*_tmp2DC;struct Cyc_Absyn_Vardecl*
_tmp2DE;_LL12E: {struct Cyc_Absyn_Var_e_struct*_tmp2CF=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2CF->tag != 1)goto _LL130;else{_tmp2D0=(void*)_tmp2CF->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2D1=(struct Cyc_Absyn_Pat_b_struct*)_tmp2D0;if(_tmp2D1->tag
!= 5)goto _LL130;else{_tmp2D2=_tmp2D1->f1;}};}}_LL12F: _tmp2D6=_tmp2D2;goto _LL131;
_LL130: {struct Cyc_Absyn_Var_e_struct*_tmp2D3=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2D3->tag != 1)goto _LL132;else{_tmp2D4=(void*)_tmp2D3->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2D5=(struct Cyc_Absyn_Local_b_struct*)_tmp2D4;if(
_tmp2D5->tag != 4)goto _LL132;else{_tmp2D6=_tmp2D5->f1;}};}}_LL131: _tmp2DA=_tmp2D6;
goto _LL133;_LL132: {struct Cyc_Absyn_Var_e_struct*_tmp2D7=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2D7->tag != 1)goto _LL134;else{_tmp2D8=(void*)_tmp2D7->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2D9=(struct Cyc_Absyn_Global_b_struct*)_tmp2D8;if(
_tmp2D9->tag != 1)goto _LL134;else{_tmp2DA=_tmp2D9->f1;}};}}_LL133: _tmp2DE=_tmp2DA;
goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_struct*_tmp2DB=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CE;if(_tmp2DB->tag != 1)goto _LL136;else{_tmp2DC=(void*)_tmp2DB->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2DD=(struct Cyc_Absyn_Param_b_struct*)_tmp2DC;if(
_tmp2DD->tag != 3)goto _LL136;else{_tmp2DE=_tmp2DD->f1;}};}}_LL135: if(_tmp2DE->escapes)
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
else{_tmp2FD=_tmp2FC->f1;}};}}_LL144: _tmp301=_tmp2FD;goto _LL146;_LL145: {struct
Cyc_Absyn_Var_e_struct*_tmp2FE=(struct Cyc_Absyn_Var_e_struct*)_tmp2DF;if(_tmp2FE->tag
!= 1)goto _LL147;else{_tmp2FF=(void*)_tmp2FE->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp300=(struct Cyc_Absyn_Local_b_struct*)_tmp2FF;if(_tmp300->tag != 4)goto _LL147;
else{_tmp301=_tmp300->f1;}};}}_LL146: _tmp305=_tmp301;goto _LL148;_LL147: {struct
Cyc_Absyn_Var_e_struct*_tmp302=(struct Cyc_Absyn_Var_e_struct*)_tmp2DF;if(_tmp302->tag
!= 1)goto _LL149;else{_tmp303=(void*)_tmp302->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp304=(struct Cyc_Absyn_Global_b_struct*)_tmp303;if(_tmp304->tag != 1)goto _LL149;
else{_tmp305=_tmp304->f1;}};}}_LL148: _tmp309=_tmp305;goto _LL14A;_LL149: {struct
Cyc_Absyn_Var_e_struct*_tmp306=(struct Cyc_Absyn_Var_e_struct*)_tmp2DF;if(_tmp306->tag
!= 1)goto _LL14B;else{_tmp307=(void*)_tmp306->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp308=(struct Cyc_Absyn_Param_b_struct*)_tmp307;if(_tmp308->tag != 3)goto _LL14B;
else{_tmp309=_tmp308->f1;}};}}_LL14A: if(_tmp309->escapes)return 0;{struct Cyc_List_List*
_tmp30A=relns;for(0;_tmp30A != 0;_tmp30A=_tmp30A->tl){struct Cyc_CfFlowInfo_Reln*
_tmp30B=(struct Cyc_CfFlowInfo_Reln*)_tmp30A->hd;if(_tmp30B->vd == _tmp309){union
Cyc_CfFlowInfo_RelnOp _tmp30C=_tmp30B->rop;struct Cyc_Absyn_Vardecl*_tmp30D;struct
_tuple12 _tmp30E;struct Cyc_Absyn_Vardecl*_tmp30F;void*_tmp310;unsigned int _tmp311;
_LL14E: if((_tmp30C.LessNumelts).tag != 3)goto _LL150;_tmp30D=(struct Cyc_Absyn_Vardecl*)(
_tmp30C.LessNumelts).val;_LL14F: if(_tmp2DE == _tmp30D)return 1;else{goto _LL14D;}
_LL150: if((_tmp30C.LessVar).tag != 2)goto _LL152;_tmp30E=(struct _tuple12)(_tmp30C.LessVar).val;
_tmp30F=_tmp30E.f1;_tmp310=_tmp30E.f2;_LL151:{struct _tuple0 _tmpB17;struct _tuple0
_tmp313=(_tmpB17.f1=Cyc_Tcutil_compress(_tmp310),((_tmpB17.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(a->topt))->v),_tmpB17)));void*_tmp314;
void*_tmp316;void*_tmp317;struct Cyc_Absyn_PtrInfo _tmp319;struct Cyc_Absyn_PtrAtts
_tmp31A;union Cyc_Absyn_Constraint*_tmp31B;_LL157: _tmp314=_tmp313.f1;{struct Cyc_Absyn_TagType_struct*
_tmp315=(struct Cyc_Absyn_TagType_struct*)_tmp314;if(_tmp315->tag != 20)goto _LL159;
else{_tmp316=(void*)_tmp315->f1;}};_tmp317=_tmp313.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp318=(struct Cyc_Absyn_PointerType_struct*)_tmp317;if(_tmp318->tag != 5)goto
_LL159;else{_tmp319=_tmp318->f1;_tmp31A=_tmp319.ptr_atts;_tmp31B=_tmp31A.bounds;}};
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
struct Cyc_Absyn_Exp*e){if(e->topt == 0){const char*_tmpB1A;void*_tmpB19;(_tmpB19=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB1A="Missing type in primop ",_tag_dyneither(_tmpB1A,sizeof(char),24))),
_tag_dyneither(_tmpB19,sizeof(void*),0)));}return Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(
e->topt == 0){const char*_tmpB1D;void*_tmpB1C;(_tmpB1C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1D="Missing type in primop ",
_tag_dyneither(_tmpB1D,sizeof(char),24))),_tag_dyneither(_tmpB1C,sizeof(void*),0)));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct
_tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e);static struct _tuple10*Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp*e){struct _tuple10*_tmpB1E;return(_tmpB1E=_cycalloc(sizeof(*
_tmpB1E)),((_tmpB1E->f1=Cyc_Toc_mt_tq,((_tmpB1E->f2=Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmpB1E)))));}static struct _tuple16*
Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static struct
_tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple16*_tmpB1F;return(_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((
_tmpB1F->f1=0,((_tmpB1F->f2=e,_tmpB1F)))));};}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic);static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(
struct Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int
pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;
if(pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp32E=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp32E): Cyc_Toc_malloc_ptr(_tmp32E));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp32E);}};}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
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
_tmp332->tl != 0){const char*_tmpB22;void*_tmpB21;(_tmpB21=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB22="multiple designators in array",
_tag_dyneither(_tmpB22,sizeof(char),30))),_tag_dyneither(_tmpB21,sizeof(void*),0)));}{
void*_tmp336=(void*)_tmp332->hd;void*_tmp337=_tmp336;struct Cyc_Absyn_Exp*_tmp339;
_LL16C: {struct Cyc_Absyn_ArrayElement_struct*_tmp338=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp337;if(_tmp338->tag != 0)goto _LL16E;else{_tmp339=_tmp338->f1;}}_LL16D: Cyc_Toc_exp_to_c(
nv,_tmp339);e_index=_tmp339;goto _LL16B;_LL16E: {struct Cyc_Absyn_FieldName_struct*
_tmp33A=(struct Cyc_Absyn_FieldName_struct*)_tmp337;if(_tmp33A->tag != 1)goto
_LL16B;}_LL16F: {const char*_tmpB25;void*_tmpB24;(_tmpB24=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB25="field name designators in array",
_tag_dyneither(_tmpB25,sizeof(char),32))),_tag_dyneither(_tmpB24,sizeof(void*),0)));}
_LL16B:;};}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
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
_tmp333,0),s,0);goto _LL170;_LL170:;};};}}return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple1*_tmp348=vd->name;void*_tmp349=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp34B;struct _RegionHandle*_tmp34C;
struct Cyc_Toc_Env*_tmp34A=nv;_tmp34B=*_tmp34A;_tmp34C=_tmp34B.rgn;{struct Cyc_Absyn_Local_b_struct
_tmpB28;struct Cyc_Absyn_Local_b_struct*_tmpB27;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
_tmp34C,nv,_tmp348,Cyc_Absyn_varb_exp(_tmp348,(void*)((_tmpB27=_cycalloc(sizeof(*
_tmpB27)),((_tmpB27[0]=((_tmpB28.tag=4,((_tmpB28.f1=vd,_tmpB28)))),_tmpB27)))),0));
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
0);};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp35A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp35A != 0;_tmp35A=_tmp35A->tl){struct _tuple16 _tmp35C;
struct Cyc_List_List*_tmp35D;struct Cyc_Absyn_Exp*_tmp35E;struct _tuple16*_tmp35B=(
struct _tuple16*)_tmp35A->hd;_tmp35C=*_tmp35B;_tmp35D=_tmp35C.f1;_tmp35E=_tmp35C.f2;
if(_tmp35D == 0){const char*_tmpB2B;void*_tmpB2A;(_tmpB2A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2B="empty designator list",
_tag_dyneither(_tmpB2B,sizeof(char),22))),_tag_dyneither(_tmpB2A,sizeof(void*),0)));}
if(_tmp35D->tl != 0){const char*_tmpB2E;void*_tmpB2D;(_tmpB2D=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2E="too many designators in anonymous struct",
_tag_dyneither(_tmpB2E,sizeof(char),41))),_tag_dyneither(_tmpB2D,sizeof(void*),0)));}{
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
_tmpB31;void*_tmpB30;(_tmpB30=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB31="array designator in struct",
_tag_dyneither(_tmpB31,sizeof(char),27))),_tag_dyneither(_tmpB30,sizeof(void*),0)));}
_LL182:;};}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
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
_tmp379,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp37A,0);goto _LL190;_LL190:;};}}
return Cyc_Toc_make_struct(nv,_tmp378,_tmp377,_tmp37A,pointer,rgnopt,is_atomic);};}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f);static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,
struct _dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp385=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp385 != 0;_tmp385=_tmp385->tl){struct Cyc_Absyn_Aggrfield*
_tmp386=(struct Cyc_Absyn_Aggrfield*)_tmp385->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp386->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}{
struct Cyc_String_pa_struct _tmpB39;void*_tmpB38[1];const char*_tmpB37;void*_tmpB36;(
_tmpB36=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)((_tmpB39.tag=0,((_tmpB39.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmpB38[0]=& _tmpB39,Cyc_aprintf(((_tmpB37="get_member_offset %s failed",
_tag_dyneither(_tmpB37,sizeof(char),28))),_tag_dyneither(_tmpB38,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB36,sizeof(void*),0)));};}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
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
const char*_tmpB3C;void*_tmpB3B;(_tmpB3B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3C="init_struct: bad struct type",
_tag_dyneither(_tmpB3C,sizeof(char),29))),_tag_dyneither(_tmpB3B,sizeof(void*),0)));}
_LL197:;}{int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp397;struct _RegionHandle*_tmp398;struct Cyc_Toc_Env*_tmp396=
nv;_tmp397=*_tmp396;_tmp398=_tmp397.rgn;{struct Cyc_List_List*_tmp399=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp398,dles);for(0;
_tmp399 != 0;_tmp399=_tmp399->tl){struct _tuple16 _tmp39B;struct Cyc_List_List*
_tmp39C;struct Cyc_Absyn_Exp*_tmp39D;struct _tuple16*_tmp39A=(struct _tuple16*)
_tmp399->hd;_tmp39B=*_tmp39A;_tmp39C=_tmp39B.f1;_tmp39D=_tmp39B.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp39D->topt))->v);
if(_tmp39C == 0){const char*_tmpB3F;void*_tmpB3E;(_tmpB3E=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB3F="empty designator list",
_tag_dyneither(_tmpB3F,sizeof(char),22))),_tag_dyneither(_tmpB3E,sizeof(void*),0)));}
if(_tmp39C->tl != 0){struct _tuple1*_tmp3A0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp3A1=Cyc_Absyn_var_exp(_tmp3A0,0);for(0;_tmp39C != 0;_tmp39C=_tmp39C->tl){void*
_tmp3A2=(void*)_tmp39C->hd;struct _dyneither_ptr*_tmp3A4;_LL19D: {struct Cyc_Absyn_FieldName_struct*
_tmp3A3=(struct Cyc_Absyn_FieldName_struct*)_tmp3A2;if(_tmp3A3->tag != 1)goto
_LL19F;else{_tmp3A4=_tmp3A3->f1;}}_LL19E: if(Cyc_Toc_is_poly_field(struct_type,
_tmp3A4))_tmp3A1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3A1);_tmp38D=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38E(_tmp38C,_tmp3A4,0),_tmp3A1,0),0),
_tmp38D,0);goto _LL19C;_LL19F:;_LL1A0: {const char*_tmpB42;void*_tmpB41;(_tmpB41=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB42="array designator in struct",_tag_dyneither(_tmpB42,sizeof(char),27))),
_tag_dyneither(_tmpB41,sizeof(void*),0)));}_LL19C:;}Cyc_Toc_exp_to_c(nv,_tmp39D);
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
_tmp39D,0),0),_tmp38D,0);goto _LL1A6;};}_LL1A6:;}goto _LL1A1;}_LL1A4:;_LL1A5: {
const char*_tmpB45;void*_tmpB44;(_tmpB44=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB45="array designator in struct",
_tag_dyneither(_tmpB45,sizeof(char),27))),_tag_dyneither(_tmpB44,sizeof(void*),0)));}
_LL1A1:;}}}return Cyc_Toc_make_struct(nv,_tmp38B,_tmp38F,_tmp38D,pointer,rgnopt,
is_atomic);};}struct _tuple18{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple18*pr);static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*
el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int
ignore);static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,
int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr);static struct Cyc_Absyn_Exp*
Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_struct _tmpB48;struct Cyc_Absyn_Increment_e_struct*
_tmpB47;return Cyc_Absyn_new_exp((void*)((_tmpB47=_cycalloc(sizeof(*_tmpB47)),((
_tmpB47[0]=((_tmpB48.tag=5,((_tmpB48.f1=e1,((_tmpB48.f2=incr,_tmpB48)))))),
_tmpB47)))),0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,
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
_tmpB49;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmpB49=_cycalloc(
sizeof(*_tmpB49)),((_tmpB49->hd=_tmp3C2,((_tmpB49->tl=fs,_tmpB49)))))),f,f_env);}
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
_tmp3CD,fs,f,f_env);goto _LL1B8;_LL1BF:;_LL1C0: {const char*_tmpB4D;void*_tmpB4C[1];
struct Cyc_String_pa_struct _tmpB4B;(_tmpB4B.tag=0,((_tmpB4B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpB4C[0]=&
_tmpB4B,Cyc_Toc_toc_impos(((_tmpB4D="lvalue_assign_stmt: %s",_tag_dyneither(
_tmpB4D,sizeof(char),23))),_tag_dyneither(_tmpB4C,sizeof(void*),1)))))));}_LL1B8:;}
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
char*_tmpB51;void*_tmpB50[1];struct Cyc_String_pa_struct _tmpB4F;(_tmpB4F.tag=0,((
_tmpB4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpB50[0]=& _tmpB4F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB51="can't take & of %s",_tag_dyneither(_tmpB51,sizeof(char),19))),
_tag_dyneither(_tmpB50,sizeof(void*),1)))))));};_LL1C1:;}static struct Cyc_List_List*
Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct
Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*_tmpB52;result=((
_tmpB52=_region_malloc(r2,sizeof(*_tmpB52)),((_tmpB52->hd=(void*)f((void*)x->hd,
env),((_tmpB52->tl=0,_tmpB52))))));}prev=result;for(x=x->tl;x != 0;x=x->tl){{
struct Cyc_List_List*_tmpB53;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmpB53=_region_malloc(r2,sizeof(*_tmpB53)),((_tmpB53->hd=(void*)f((void*)x->hd,
env),((_tmpB53->tl=0,_tmpB53))))));}prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e);static
struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple16*_tmpB54;
return(_tmpB54=_cycalloc(sizeof(*_tmpB54)),((_tmpB54->f1=0,((_tmpB54->f2=e,
_tmpB54)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t);static
struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp3DF=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp3E1;_LL1C9: {struct Cyc_Absyn_PointerType_struct*
_tmp3E0=(struct Cyc_Absyn_PointerType_struct*)_tmp3DF;if(_tmp3E0->tag != 5)goto
_LL1CB;else{_tmp3E1=_tmp3E0->f1;}}_LL1CA: return _tmp3E1;_LL1CB:;_LL1CC: {const
char*_tmpB57;void*_tmpB56;(_tmpB56=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB57="get_ptr_typ: not a pointer!",
_tag_dyneither(_tmpB57,sizeof(char),28))),_tag_dyneither(_tmpB56,sizeof(void*),0)));}
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
_LL1E2;}_LL1E1:{const char*_tmpB58;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((
_tmpB58="0.0",_tag_dyneither(_tmpB58,sizeof(char),4)))),0);}goto _LL1CD;_LL1E2:;
_LL1E3: {const char*_tmpB5C;void*_tmpB5B[1];struct Cyc_String_pa_struct _tmpB5A;(
_tmpB5A.tag=0,((_tmpB5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpB5B[0]=& _tmpB5A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(((_tmpB5C="found non-zero type %s in generate_zero",
_tag_dyneither(_tmpB5C,sizeof(char),40))),_tag_dyneither(_tmpB5B,sizeof(void*),1)))))));}
_LL1CD:;}{struct Cyc_Core_Opt*_tmpB5D;res->topt=((_tmpB5D=_cycalloc(sizeof(*
_tmpB5D)),((_tmpB5D->v=t,_tmpB5D))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type);static
void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);void*
_tmp3FE=Cyc_Toc_typ_to_c(elt_type);void*_tmp3FF=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp400=Cyc_Absyn_cstar_typ(_tmp3FE,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmpB5E;struct Cyc_Core_Opt*_tmp401=(_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((
_tmpB5E->v=_tmp400,_tmpB5E)));struct Cyc_Absyn_Exp*xinit;{void*_tmp402=e1->r;
struct Cyc_Absyn_Exp*_tmp404;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*
_tmp407;_LL1E5: {struct Cyc_Absyn_Deref_e_struct*_tmp403=(struct Cyc_Absyn_Deref_e_struct*)
_tmp402;if(_tmp403->tag != 22)goto _LL1E7;else{_tmp404=_tmp403->f1;}}_LL1E6: if(!
is_dyneither){_tmp404=Cyc_Toc_cast_it(fat_ptr_type,_tmp404);{struct Cyc_Core_Opt*
_tmpB5F;_tmp404->topt=((_tmpB5F=_cycalloc(sizeof(*_tmpB5F)),((_tmpB5F->v=
fat_ptr_type,_tmpB5F))));};}Cyc_Toc_exp_to_c(nv,_tmp404);xinit=_tmp404;goto
_LL1E4;_LL1E7: {struct Cyc_Absyn_Subscript_e_struct*_tmp405=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp402;if(_tmp405->tag != 25)goto _LL1E9;else{_tmp406=_tmp405->f1;_tmp407=_tmp405->f2;}}
_LL1E8: if(!is_dyneither){_tmp406=Cyc_Toc_cast_it(fat_ptr_type,_tmp406);{struct
Cyc_Core_Opt*_tmpB60;_tmp406->topt=((_tmpB60=_cycalloc(sizeof(*_tmpB60)),((
_tmpB60->v=fat_ptr_type,_tmpB60))));};}Cyc_Toc_exp_to_c(nv,_tmp406);Cyc_Toc_exp_to_c(
nv,_tmp407);{struct Cyc_Absyn_Exp*_tmpB61[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmpB61[2]=_tmp407,((_tmpB61[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmpB61[0]=_tmp406,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpB61,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1E4;_LL1E9:;_LL1EA: {const char*_tmpB64;void*_tmpB63;(_tmpB63=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB64="found bad lhs for zero-terminated pointer assignment",
_tag_dyneither(_tmpB64,sizeof(char),53))),_tag_dyneither(_tmpB63,sizeof(void*),0)));}
_LL1E4:;}{struct _tuple1*_tmp40D=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp40F;
struct _RegionHandle*_tmp410;struct Cyc_Toc_Env*_tmp40E=nv;_tmp40F=*_tmp40E;
_tmp410=_tmp40F.rgn;{struct Cyc_Toc_Env*_tmp411=Cyc_Toc_add_varmap(_tmp410,nv,
_tmp40D,Cyc_Absyn_var_exp(_tmp40D,0));struct Cyc_Absyn_Vardecl*_tmpB65;struct Cyc_Absyn_Vardecl*
_tmp412=(_tmpB65=_cycalloc(sizeof(*_tmpB65)),((_tmpB65->sc=Cyc_Absyn_Public,((
_tmpB65->name=_tmp40D,((_tmpB65->tq=Cyc_Toc_mt_tq,((_tmpB65->type=_tmp3FF,((
_tmpB65->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmpB65->rgn=0,((_tmpB65->attributes=
0,((_tmpB65->escapes=0,_tmpB65)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmpB68;struct Cyc_Absyn_Local_b_struct*_tmpB67;struct Cyc_Absyn_Local_b_struct*
_tmp413=(_tmpB67=_cycalloc(sizeof(*_tmpB67)),((_tmpB67[0]=((_tmpB68.tag=4,((
_tmpB68.f1=_tmp412,_tmpB68)))),_tmpB67)));struct Cyc_Absyn_Exp*_tmp414=Cyc_Absyn_varb_exp(
_tmp40D,(void*)_tmp413,0);{struct Cyc_Core_Opt*_tmpB69;_tmp414->topt=((_tmpB69=
_cycalloc(sizeof(*_tmpB69)),((_tmpB69->v=fat_ptr_type,_tmpB69))));}{struct Cyc_Absyn_Exp*
_tmp416=Cyc_Absyn_deref_exp(_tmp414,0);{struct Cyc_Core_Opt*_tmpB6A;_tmp416->topt=((
_tmpB6A=_cycalloc(sizeof(*_tmpB6A)),((_tmpB6A->v=elt_type,_tmpB6A))));}Cyc_Toc_exp_to_c(
_tmp411,_tmp416);{struct _tuple1*_tmp418=Cyc_Toc_temp_var();_tmp411=Cyc_Toc_add_varmap(
_tmp410,_tmp411,_tmp418,Cyc_Absyn_var_exp(_tmp418,0));{struct Cyc_Absyn_Vardecl*
_tmpB6B;struct Cyc_Absyn_Vardecl*_tmp419=(_tmpB6B=_cycalloc(sizeof(*_tmpB6B)),((
_tmpB6B->sc=Cyc_Absyn_Public,((_tmpB6B->name=_tmp418,((_tmpB6B->tq=Cyc_Toc_mt_tq,((
_tmpB6B->type=_tmp3FE,((_tmpB6B->initializer=(struct Cyc_Absyn_Exp*)_tmp416,((
_tmpB6B->rgn=0,((_tmpB6B->attributes=0,((_tmpB6B->escapes=0,_tmpB6B)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB6E;struct Cyc_Absyn_Local_b_struct*_tmpB6D;
struct Cyc_Absyn_Local_b_struct*_tmp41A=(_tmpB6D=_cycalloc(sizeof(*_tmpB6D)),((
_tmpB6D[0]=((_tmpB6E.tag=4,((_tmpB6E.f1=_tmp419,_tmpB6E)))),_tmpB6D)));struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp41B=Cyc_Absyn_varb_exp(_tmp418,(
void*)_tmp41A,0);_tmp41B->topt=_tmp416->topt;z_init=Cyc_Absyn_prim2_exp((enum 
Cyc_Absyn_Primop)popt->v,_tmp41B,e2,0);z_init->topt=_tmp41B->topt;}Cyc_Toc_exp_to_c(
_tmp411,z_init);{struct _tuple1*_tmp41C=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmpB6F;struct Cyc_Absyn_Vardecl*_tmp41D=(_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((
_tmpB6F->sc=Cyc_Absyn_Public,((_tmpB6F->name=_tmp41C,((_tmpB6F->tq=Cyc_Toc_mt_tq,((
_tmpB6F->type=_tmp3FE,((_tmpB6F->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmpB6F->rgn=0,((_tmpB6F->attributes=0,((_tmpB6F->escapes=0,_tmpB6F)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmpB72;struct Cyc_Absyn_Local_b_struct*_tmpB71;
struct Cyc_Absyn_Local_b_struct*_tmp41E=(_tmpB71=_cycalloc(sizeof(*_tmpB71)),((
_tmpB71[0]=((_tmpB72.tag=4,((_tmpB72.f1=_tmp41D,_tmpB72)))),_tmpB71)));_tmp411=
Cyc_Toc_add_varmap(_tmp410,_tmp411,_tmp41C,Cyc_Absyn_var_exp(_tmp41C,0));{struct
Cyc_Absyn_Exp*_tmp41F=Cyc_Absyn_varb_exp(_tmp418,(void*)_tmp41A,0);_tmp41F->topt=
_tmp416->topt;{struct Cyc_Absyn_Exp*_tmp420=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp421=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp41F,_tmp420,0);{
struct Cyc_Core_Opt*_tmpB73;_tmp421->topt=((_tmpB73=_cycalloc(sizeof(*_tmpB73)),((
_tmpB73->v=Cyc_Absyn_sint_typ,_tmpB73))));}Cyc_Toc_exp_to_c(_tmp411,_tmp421);{
struct Cyc_Absyn_Exp*_tmp423=Cyc_Absyn_varb_exp(_tmp41C,(void*)_tmp41E,0);_tmp423->topt=
_tmp416->topt;{struct Cyc_Absyn_Exp*_tmp424=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp425=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp423,_tmp424,0);{
struct Cyc_Core_Opt*_tmpB74;_tmp425->topt=((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((
_tmpB74->v=Cyc_Absyn_sint_typ,_tmpB74))));}Cyc_Toc_exp_to_c(_tmp411,_tmp425);{
struct Cyc_Absyn_Exp*_tmpB75[2];struct Cyc_List_List*_tmp427=(_tmpB75[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmpB75[0]=Cyc_Absyn_varb_exp(_tmp40D,(void*)
_tmp413,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpB75,sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*
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
0),0),_tmp42E,0);{struct Cyc_Absyn_Var_d_struct*_tmpB7B;struct Cyc_Absyn_Var_d_struct
_tmpB7A;struct Cyc_Absyn_Decl*_tmpB79;_tmp42E=Cyc_Absyn_decl_stmt(((_tmpB79=
_cycalloc(sizeof(*_tmpB79)),((_tmpB79->r=(void*)((_tmpB7B=_cycalloc(sizeof(*
_tmpB7B)),((_tmpB7B[0]=((_tmpB7A.tag=0,((_tmpB7A.f1=_tmp41D,_tmpB7A)))),_tmpB7B)))),((
_tmpB79->loc=0,_tmpB79)))))),_tmp42E,0);}{struct Cyc_Absyn_Var_d_struct*_tmpB81;
struct Cyc_Absyn_Var_d_struct _tmpB80;struct Cyc_Absyn_Decl*_tmpB7F;_tmp42E=Cyc_Absyn_decl_stmt(((
_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F->r=(void*)((_tmpB81=_cycalloc(
sizeof(*_tmpB81)),((_tmpB81[0]=((_tmpB80.tag=0,((_tmpB80.f1=_tmp419,_tmpB80)))),
_tmpB81)))),((_tmpB7F->loc=0,_tmpB7F)))))),_tmp42E,0);}{struct Cyc_Absyn_Var_d_struct*
_tmpB87;struct Cyc_Absyn_Var_d_struct _tmpB86;struct Cyc_Absyn_Decl*_tmpB85;_tmp42E=
Cyc_Absyn_decl_stmt(((_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85->r=(void*)((
_tmpB87=_cycalloc(sizeof(*_tmpB87)),((_tmpB87[0]=((_tmpB86.tag=0,((_tmpB86.f1=
_tmp412,_tmpB86)))),_tmpB87)))),((_tmpB85->loc=0,_tmpB85)))))),_tmp42E,0);}e->r=
Cyc_Toc_stmt_exp_r(_tmp42E);};};};};};};};};};};};};};}static void*Cyc_Toc_check_tagged_union(
struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,
struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*));static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*
e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*
aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*)){
struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp443=Cyc_Tcutil_compress(aggrtype);struct
Cyc_Absyn_AggrInfo _tmp445;union Cyc_Absyn_AggrInfoU _tmp446;_LL1EC: {struct Cyc_Absyn_AggrType_struct*
_tmp444=(struct Cyc_Absyn_AggrType_struct*)_tmp443;if(_tmp444->tag != 12)goto
_LL1EE;else{_tmp445=_tmp444->f1;_tmp446=_tmp445.aggr_info;}}_LL1ED: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp446);goto _LL1EB;_LL1EE:;_LL1EF: {struct Cyc_String_pa_struct _tmpB8F;void*
_tmpB8E[1];const char*_tmpB8D;void*_tmpB8C;(_tmpB8C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct
_dyneither_ptr)((_tmpB8F.tag=0,((_tmpB8F.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpB8E[0]=& _tmpB8F,Cyc_aprintf(((
_tmpB8D="expecting union but found %s in check_tagged_union",_tag_dyneither(
_tmpB8D,sizeof(char),51))),_tag_dyneither(_tmpB8E,sizeof(void*),1)))))))),
_tag_dyneither(_tmpB8C,sizeof(void*),0)));}_LL1EB:;}{struct _tuple1*_tmp44B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp44C=Cyc_Absyn_var_exp(_tmp44B,0);struct Cyc_Absyn_Exp*
_tmp44D=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44C,f,0),Cyc_Toc_tag_sp,0);struct
Cyc_Absyn_Exp*_tmp44E=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
struct Cyc_Absyn_Exp*_tmp44F=Cyc_Absyn_neq_exp(_tmp44D,_tmp44E,0);struct Cyc_Absyn_Exp*
_tmp450=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp44C,f,0),Cyc_Toc_val_sp,0);struct
Cyc_Absyn_Stmt*_tmp451=Cyc_Absyn_exp_stmt(_tmp450,0);struct Cyc_Absyn_Stmt*
_tmp452=Cyc_Absyn_ifthenelse_stmt(_tmp44F,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp453=Cyc_Absyn_declare_stmt(_tmp44B,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp452,_tmp451,0),0);return Cyc_Toc_stmt_exp_r(
_tmp453);};}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read);static int Cyc_Toc_is_tagged_union_project(
struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){void*
_tmp454=e->r;struct Cyc_Absyn_Exp*_tmp456;struct Cyc_Absyn_Exp*_tmp458;struct
_dyneither_ptr*_tmp459;int _tmp45A;int*_tmp45B;struct Cyc_Absyn_Exp*_tmp45D;struct
_dyneither_ptr*_tmp45E;int _tmp45F;int*_tmp460;_LL1F1: {struct Cyc_Absyn_Cast_e_struct*
_tmp455=(struct Cyc_Absyn_Cast_e_struct*)_tmp454;if(_tmp455->tag != 15)goto _LL1F3;
else{_tmp456=_tmp455->f2;}}_LL1F2: {const char*_tmpB92;void*_tmpB91;(_tmpB91=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB92="cast on lhs!",_tag_dyneither(_tmpB92,sizeof(char),13))),_tag_dyneither(
_tmpB91,sizeof(void*),0)));}_LL1F3: {struct Cyc_Absyn_AggrMember_e_struct*_tmp457=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp454;if(_tmp457->tag != 23)goto _LL1F5;
else{_tmp458=_tmp457->f1;_tmp459=_tmp457->f2;_tmp45A=_tmp457->f4;_tmp45B=(int*)&
_tmp457->f4;}}_LL1F4: {void*_tmp463=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp458->topt))->v);struct Cyc_Absyn_AggrInfo _tmp465;union Cyc_Absyn_AggrInfoU
_tmp466;_LL1FA: {struct Cyc_Absyn_AggrType_struct*_tmp464=(struct Cyc_Absyn_AggrType_struct*)
_tmp463;if(_tmp464->tag != 12)goto _LL1FC;else{_tmp465=_tmp464->f1;_tmp466=_tmp465.aggr_info;}}
_LL1FB: {struct Cyc_Absyn_Aggrdecl*_tmp467=Cyc_Absyn_get_known_aggrdecl(_tmp466);*
f_tag=Cyc_Toc_get_member_offset(_tmp467,_tmp459);{const char*_tmpB97;void*_tmpB96[
2];struct Cyc_String_pa_struct _tmpB95;struct Cyc_String_pa_struct _tmpB94;struct
_dyneither_ptr str=(struct _dyneither_ptr)((_tmpB94.tag=0,((_tmpB94.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp459),((_tmpB95.tag=0,((_tmpB95.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp467->name).f2),((_tmpB96[0]=&
_tmpB95,((_tmpB96[1]=& _tmpB94,Cyc_aprintf(((_tmpB97="_union_%s_%s",
_tag_dyneither(_tmpB97,sizeof(char),13))),_tag_dyneither(_tmpB96,sizeof(void*),2))))))))))))));{
struct _dyneither_ptr*_tmpB98;*tagged_member_type=Cyc_Absyn_strct(((_tmpB98=
_cycalloc(sizeof(*_tmpB98)),((_tmpB98[0]=str,_tmpB98)))));}if(clear_read)*
_tmp45B=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp467->impl))->tagged;};}
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
_tmp475,_tmp45E);{const char*_tmpB9D;void*_tmpB9C[2];struct Cyc_String_pa_struct
_tmpB9B;struct Cyc_String_pa_struct _tmpB9A;struct _dyneither_ptr str=(struct
_dyneither_ptr)((_tmpB9A.tag=0,((_tmpB9A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp45E),((_tmpB9B.tag=0,((_tmpB9B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(*_tmp475->name).f2),((_tmpB9C[0]=& _tmpB9B,((_tmpB9C[1]=&
_tmpB9A,Cyc_aprintf(((_tmpB9D="_union_%s_%s",_tag_dyneither(_tmpB9D,sizeof(char),
13))),_tag_dyneither(_tmpB9C,sizeof(void*),2))))))))))))));{struct _dyneither_ptr*
_tmpB9E;*tagged_member_type=Cyc_Absyn_strct(((_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((
_tmpB9E[0]=str,_tmpB9E)))));}if(clear_read)*_tmp460=0;return((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp475->impl))->tagged;};}_LL206:;_LL207: return 0;_LL203:;}_LL201:;
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
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple19{struct _tuple1*f1;void*f2;struct
Cyc_Absyn_Exp*f3;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void _tmpBFD(unsigned int*_tmpBFC,unsigned int*_tmpBFB,struct _tuple1***
_tmpBF9){for(*_tmpBFC=0;*_tmpBFC < *_tmpBFB;(*_tmpBFC)++){(*_tmpBF9)[*_tmpBFC]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp47D=e->r;if(e->topt == 0){const char*_tmpBA2;void*_tmpBA1[1];struct Cyc_String_pa_struct
_tmpBA0;(_tmpBA0.tag=0,((_tmpBA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e)),((_tmpBA1[0]=& _tmpBA0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA2="exp_to_c: no type for %s",
_tag_dyneither(_tmpBA2,sizeof(char),25))),_tag_dyneither(_tmpBA1,sizeof(void*),1)))))));}{
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
_tmp50A))->r;else{struct Cyc_Absyn_Exp*_tmpBA3[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((
_tmpBA3[2]=_tmp50A,((_tmpBA3[1]=_tmp50A,((_tmpBA3[0]=_tmp50A,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBA3,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}}else{e->r=(void*)& Cyc_Toc_zero_exp;}goto _LL208;}_LL20B: {struct Cyc_Absyn_Const_e_struct*
_tmp485=(struct Cyc_Absyn_Const_e_struct*)_tmp481;if(_tmp485->tag != 0)goto _LL20D;}
_LL20C: goto _LL208;_LL20D: {struct Cyc_Absyn_Var_e_struct*_tmp486=(struct Cyc_Absyn_Var_e_struct*)
_tmp481;if(_tmp486->tag != 1)goto _LL20F;else{_tmp487=_tmp486->f1;_tmp488=(void*)
_tmp486->f2;}}_LL20E:{struct _handler_cons _tmp50C;_push_handler(& _tmp50C);{int
_tmp50E=0;if(setjmp(_tmp50C.handler))_tmp50E=1;if(!_tmp50E){e->r=(Cyc_Toc_lookup_varmap(
nv,_tmp487))->r;;_pop_handler();}else{void*_tmp50D=(void*)_exn_thrown;void*
_tmp510=_tmp50D;_LL262: {struct Cyc_Dict_Absent_struct*_tmp511=(struct Cyc_Dict_Absent_struct*)
_tmp510;if(_tmp511->tag != Cyc_Dict_Absent)goto _LL264;}_LL263: {const char*_tmpBA7;
void*_tmpBA6[1];struct Cyc_String_pa_struct _tmpBA5;(_tmpBA5.tag=0,((_tmpBA5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp487)),((
_tmpBA6[0]=& _tmpBA5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBA7="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpBA7,sizeof(char),
32))),_tag_dyneither(_tmpBA6,sizeof(void*),1)))))));}_LL264:;_LL265:(void)_throw(
_tmp510);_LL261:;}};}goto _LL208;_LL20F: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp489=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp481;if(_tmp489->tag != 2)goto
_LL211;else{_tmp48A=_tmp489->f1;}}_LL210: {const char*_tmpBAA;void*_tmpBA9;(
_tmpBA9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBAA="unknownid",_tag_dyneither(_tmpBAA,sizeof(char),10))),_tag_dyneither(
_tmpBA9,sizeof(void*),0)));}_LL211: {struct Cyc_Absyn_Primop_e_struct*_tmp48B=(
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
struct Cyc_Absyn_Exp*)_check_null(_tmp51B))){const char*_tmpBAD;void*_tmpBAC;(
_tmpBAC=0,Cyc_Tcutil_terr(e->loc,((_tmpBAD="can't calculate numelts",
_tag_dyneither(_tmpBAD,sizeof(char),24))),_tag_dyneither(_tmpBAC,sizeof(void*),0)));}
e->r=_tmp51B->r;goto _LL268;_LL26B: {struct Cyc_Absyn_PointerType_struct*_tmp51C=(
struct Cyc_Absyn_PointerType_struct*)_tmp518;if(_tmp51C->tag != 5)goto _LL26D;else{
_tmp51D=_tmp51C->f1;_tmp51E=_tmp51D.elt_typ;_tmp51F=_tmp51D.ptr_atts;_tmp520=
_tmp51F.nullable;_tmp521=_tmp51F.bounds;_tmp522=_tmp51F.zero_term;}}_LL26C:{void*
_tmp525=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp521);struct Cyc_Absyn_Exp*_tmp528;_LL270: {struct Cyc_Absyn_DynEither_b_struct*
_tmp526=(struct Cyc_Absyn_DynEither_b_struct*)_tmp525;if(_tmp526->tag != 0)goto
_LL272;}_LL271:{struct Cyc_Absyn_Exp*_tmpBAE[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmpBAE[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp51E),0),((_tmpBAE[0]=(
struct Cyc_Absyn_Exp*)_tmp48D->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpBAE,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL26F;_LL272: {struct Cyc_Absyn_Upper_b_struct*_tmp527=(struct Cyc_Absyn_Upper_b_struct*)
_tmp525;if(_tmp527->tag != 1)goto _LL26F;else{_tmp528=_tmp527->f1;}}_LL273: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp522)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp48D->hd);struct Cyc_Absyn_Exp*_tmpBAF[2];e->r=Cyc_Toc_fncall_exp_r(
function_e,((_tmpBAF[1]=_tmp528,((_tmpBAF[0]=(struct Cyc_Absyn_Exp*)_tmp48D->hd,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpBAF,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{if(((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp520)){if(!Cyc_Evexp_c_can_eval(_tmp528)){const
char*_tmpBB2;void*_tmpBB1;(_tmpBB1=0,Cyc_Tcutil_terr(e->loc,((_tmpBB2="can't calculate numelts",
_tag_dyneither(_tmpBB2,sizeof(char),24))),_tag_dyneither(_tmpBB1,sizeof(void*),0)));}
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp528,Cyc_Absyn_uint_exp(0,0));}else{e->r=
_tmp528->r;goto _LL26F;}}goto _LL26F;_LL26F:;}goto _LL268;_LL26D:;_LL26E: {const
char*_tmpBB7;void*_tmpBB6[2];struct Cyc_String_pa_struct _tmpBB5;struct Cyc_String_pa_struct
_tmpBB4;(_tmpBB4.tag=0,((_tmpBB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((
_tmpBB5.tag=0,((_tmpBB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmpBB6[0]=& _tmpBB5,((
_tmpBB6[1]=& _tmpBB4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpBB7="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpBB7,
sizeof(char),41))),_tag_dyneither(_tmpBB6,sizeof(void*),2)))))))))))));}_LL268:;}
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
Cyc_List_List*)_check_null(_tmp48D->tl))->hd;{struct Cyc_Absyn_Exp*_tmpBB8[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBB8[2]=e2,((_tmpBB8[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp534),0),((_tmpBB8[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB8,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));}goto _LL27A;}_LL27D: {struct Cyc_Absyn_Upper_b_struct*_tmp53A=(struct
Cyc_Absyn_Upper_b_struct*)_tmp538;if(_tmp53A->tag != 1)goto _LL27A;else{_tmp53B=
_tmp53A->f1;}}_LL27E: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp537)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp48D))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp48D->tl))->hd;struct Cyc_Absyn_Exp*_tmpBB9[3];e->r=(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),((
_tmpBB9[2]=e2,((_tmpBB9[1]=_tmp53B,((_tmpBB9[0]=e1,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBB9,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r;}goto _LL27A;_LL27A:;}goto _LL275;_LL278:;_LL279: goto _LL275;
_LL275:;}break;case Cyc_Absyn_Minus: _LL274: {void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp517))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48D))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp48D->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp517->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{struct Cyc_Absyn_Exp*_tmpBBA[3];e->r=
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpBBA[2]=Cyc_Absyn_prim1_exp(
Cyc_Absyn_Minus,e2,0),((_tmpBBA[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_typ),0),((_tmpBBA[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBBA,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}break;}case Cyc_Absyn_Eq:
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
is_dyneither=0;const char*_tmpBBB;struct _dyneither_ptr incr_str=(_tmpBBB="increment",
_tag_dyneither(_tmpBBB,sizeof(char),10));if(_tmp490 == Cyc_Absyn_PreDec  || 
_tmp490 == Cyc_Absyn_PostDec){const char*_tmpBBC;incr_str=((_tmpBBC="decrement",
_tag_dyneither(_tmpBBC,sizeof(char),10)));}if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp48F,& ptr_type,& is_dyneither,& elt_type)){{const char*_tmpBC0;void*_tmpBBF[1];
struct Cyc_String_pa_struct _tmpBBE;(_tmpBBE.tag=0,((_tmpBBE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBBF[0]=& _tmpBBE,Cyc_Tcutil_terr(
e->loc,((_tmpBC0="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dyneither(_tmpBC0,sizeof(char),74))),_tag_dyneither(_tmpBBF,sizeof(void*),1)))))));}{
const char*_tmpBC3;void*_tmpBC2;(_tmpBC2=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC3="in-place inc/dec on zero-term",
_tag_dyneither(_tmpBC3,sizeof(char),30))),_tag_dyneither(_tmpBC2,sizeof(void*),0)));};}{
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp48F,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp545=Cyc_Absyn_signed_int_exp(
1,0);{struct Cyc_Core_Opt*_tmpBC4;_tmp545->topt=((_tmpBC4=_cycalloc(sizeof(*
_tmpBC4)),((_tmpBC4->v=Cyc_Absyn_sint_typ,_tmpBC4))));}switch(_tmp490){case Cyc_Absyn_PreInc:
_LL287:{struct Cyc_Absyn_AssignOp_e_struct _tmpBCA;struct Cyc_Core_Opt*_tmpBC9;
struct Cyc_Absyn_AssignOp_e_struct*_tmpBC8;e->r=(void*)((_tmpBC8=_cycalloc(
sizeof(*_tmpBC8)),((_tmpBC8[0]=((_tmpBCA.tag=4,((_tmpBCA.f1=_tmp48F,((_tmpBCA.f2=((
_tmpBC9=_cycalloc_atomic(sizeof(*_tmpBC9)),((_tmpBC9->v=(void*)Cyc_Absyn_Plus,
_tmpBC9)))),((_tmpBCA.f3=_tmp545,_tmpBCA)))))))),_tmpBC8))));}Cyc_Toc_exp_to_c(
nv,e);return;case Cyc_Absyn_PreDec: _LL288:{struct Cyc_Absyn_AssignOp_e_struct
_tmpBD0;struct Cyc_Core_Opt*_tmpBCF;struct Cyc_Absyn_AssignOp_e_struct*_tmpBCE;e->r=(
void*)((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE[0]=((_tmpBD0.tag=4,((
_tmpBD0.f1=_tmp48F,((_tmpBD0.f2=((_tmpBCF=_cycalloc_atomic(sizeof(*_tmpBCF)),((
_tmpBCF->v=(void*)Cyc_Absyn_Minus,_tmpBCF)))),((_tmpBD0.f3=_tmp545,_tmpBD0)))))))),
_tmpBCE))));}Cyc_Toc_exp_to_c(nv,e);return;default: _LL289:{const char*_tmpBD4;
void*_tmpBD3[1];struct Cyc_String_pa_struct _tmpBD2;(_tmpBD2.tag=0,((_tmpBD2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpBD3[0]=& _tmpBD2,Cyc_Tcutil_terr(
e->loc,((_tmpBD4="in-place post-%s is not supported on @tagged union members",
_tag_dyneither(_tmpBD4,sizeof(char),59))),_tag_dyneither(_tmpBD3,sizeof(void*),1)))))));}{
const char*_tmpBD7;void*_tmpBD6;(_tmpBD6=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD7="in-place inc/dec on @tagged union",
_tag_dyneither(_tmpBD7,sizeof(char),34))),_tag_dyneither(_tmpBD6,sizeof(void*),0)));};}}
Cyc_Toc_exp_to_c(nv,_tmp48F);{void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp490 == Cyc_Absyn_PostInc
 || _tmp490 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp490 == Cyc_Absyn_PreDec  || _tmp490 == Cyc_Absyn_PostDec)change=- 1;{struct
Cyc_Absyn_Exp*_tmpBD8[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBD8[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpBD8[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpBD8[0]=Cyc_Toc_push_address_exp(_tmp48F),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD8,sizeof(struct Cyc_Absyn_Exp*),
3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;{void*_tmp553=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp48F->topt))->v);enum Cyc_Absyn_Size_of _tmp555;int _tmp558;_LL28C: {
struct Cyc_Absyn_IntType_struct*_tmp554=(struct Cyc_Absyn_IntType_struct*)_tmp553;
if(_tmp554->tag != 6)goto _LL28E;else{_tmp555=_tmp554->f2;}}_LL28D: switch(_tmp555){
case Cyc_Absyn_Char_sz: _LL296: fn_e=_tmp490 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_char_e:
Cyc_Toc__zero_arr_inplace_plus_char_e;break;case Cyc_Absyn_Short_sz: _LL297: fn_e=
_tmp490 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
break;case Cyc_Absyn_Int_sz: _LL298: fn_e=_tmp490 == Cyc_Absyn_PostInc?Cyc_Toc__zero_arr_inplace_plus_post_int_e:
Cyc_Toc__zero_arr_inplace_plus_int_e;break;default: _LL299: {struct Cyc_Core_Impossible_struct
_tmpBDE;const char*_tmpBDD;struct Cyc_Core_Impossible_struct*_tmpBDC;(int)_throw((
void*)((_tmpBDC=_cycalloc(sizeof(*_tmpBDC)),((_tmpBDC[0]=((_tmpBDE.tag=Cyc_Core_Impossible,((
_tmpBDE.f1=((_tmpBDD="impossible IntType (not char, short or int)",
_tag_dyneither(_tmpBDD,sizeof(char),44))),_tmpBDE)))),_tmpBDC)))));}}goto _LL28B;
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
goto _LL28B;_LL294:;_LL295: {struct Cyc_Core_Impossible_struct _tmpBE4;const char*
_tmpBE3;struct Cyc_Core_Impossible_struct*_tmpBE2;(int)_throw((void*)((_tmpBE2=
_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2[0]=((_tmpBE4.tag=Cyc_Core_Impossible,((
_tmpBE4.f1=((_tmpBE3="impossible expression type (not int, float, double, or pointer)",
_tag_dyneither(_tmpBE3,sizeof(char),64))),_tmpBE4)))),_tmpBE2)))));}_LL28B:;}{
struct Cyc_Absyn_Exp*_tmpBE5[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpBE5[1]=Cyc_Absyn_signed_int_exp(
1,0),((_tmpBE5[0]=_tmp48F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE5,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}else{if(elt_typ
== (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp48F)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(
_tmp48F,0,Cyc_Toc_incr_lvalue,_tmp490);e->r=_tmp48F->r;}}}goto _LL208;};};}_LL215: {
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
_LL2A0: {const char*_tmpBE8;void*_tmpBE7;(_tmpBE7=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE8="bad t ? pointer arithmetic",
_tag_dyneither(_tmpBE8,sizeof(char),27))),_tag_dyneither(_tmpBE7,sizeof(void*),0)));}}
done=1;{struct Cyc_Absyn_Exp*_tmp564=Cyc_Toc__dyneither_ptr_inplace_plus_e;struct
Cyc_Absyn_Exp*_tmpBE9[3];e->r=Cyc_Toc_fncall_exp_r(_tmp564,((_tmpBE9[2]=change,((
_tmpBE9[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpBE9[0]=Cyc_Toc_push_address_exp(
_tmp492),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBE9,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ))switch((enum Cyc_Absyn_Primop)_tmp493->v){case Cyc_Absyn_Plus:
_LL2A2: done=1;{struct Cyc_Absyn_Exp*_tmpBEA[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp492),((_tmpBEA[1]=_tmp494,((
_tmpBEA[0]=_tmp492,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBEA,sizeof(struct Cyc_Absyn_Exp*),2)))))));}break;default:
_LL2A3: {const char*_tmpBED;void*_tmpBEC;(_tmpBEC=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBED="bad zero-terminated pointer arithmetic",
_tag_dyneither(_tmpBED,sizeof(char),39))),_tag_dyneither(_tmpBEC,sizeof(void*),0)));}}}}
if(!done){if(e1_poly)_tmp494->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp494->r,0));if(!Cyc_Absyn_is_lvalue(_tmp492)){{struct _tuple18 _tmpBF0;struct
_tuple18*_tmpBEF;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct
Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple18*),struct _tuple18*f_env))
Cyc_Toc_lvalue_assign)(_tmp492,0,Cyc_Toc_assignop_lvalue,((_tmpBEF=_cycalloc(
sizeof(struct _tuple18)* 1),((_tmpBEF[0]=((_tmpBF0.f1=_tmp493,((_tmpBF0.f2=
_tmp494,_tmpBF0)))),_tmpBEF)))));}e->r=_tmp492->r;}}goto _LL208;};};};}_LL217: {
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
struct Cyc_List_List*_tmpBF1;new_args=((_tmpBF1=_cycalloc(sizeof(*_tmpBF1)),((
_tmpBF1->hd=(struct Cyc_Absyn_Exp*)_tmp4AB->hd,((_tmpBF1->tl=new_args,_tmpBF1))))));};}}{
struct Cyc_Absyn_Exp*_tmpBF4[3];struct Cyc_List_List*_tmpBF3;new_args=((_tmpBF3=
_cycalloc(sizeof(*_tmpBF3)),((_tmpBF3->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((
_tmpBF4[2]=num_varargs_exp,((_tmpBF4[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpBF4[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpBF4,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpBF3->tl=
new_args,_tmpBF3))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp4AA);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4AA,new_args,0),0);if(_tmp4B0->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp571=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(
_tmp4B0->type));struct Cyc_Absyn_DatatypeInfo _tmp573;union Cyc_Absyn_DatatypeInfoU
_tmp574;struct Cyc_Absyn_Datatypedecl**_tmp575;struct Cyc_Absyn_Datatypedecl*
_tmp576;_LL2A6: {struct Cyc_Absyn_DatatypeType_struct*_tmp572=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp571;if(_tmp572->tag != 3)goto _LL2A8;else{_tmp573=_tmp572->f1;_tmp574=_tmp573.datatype_info;
if((_tmp574.KnownDatatype).tag != 2)goto _LL2A8;_tmp575=(struct Cyc_Absyn_Datatypedecl**)(
_tmp574.KnownDatatype).val;_tmp576=*_tmp575;}}_LL2A7: tud=_tmp576;goto _LL2A5;
_LL2A8:;_LL2A9: {const char*_tmpBF7;void*_tmpBF6;(_tmpBF6=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF7="toc: unknown datatype in vararg with inject",
_tag_dyneither(_tmpBF7,sizeof(char),44))),_tag_dyneither(_tmpBF6,sizeof(void*),0)));}
_LL2A5:;}{unsigned int _tmpBFC;unsigned int _tmpBFB;struct _dyneither_ptr _tmpBFA;
struct _tuple1**_tmpBF9;unsigned int _tmpBF8;struct _dyneither_ptr vs=(_tmpBF8=(
unsigned int)_tmp4AE,((_tmpBF9=(struct _tuple1**)_region_malloc(_tmp56D,
_check_times(sizeof(struct _tuple1*),_tmpBF8)),((_tmpBFA=_tag_dyneither(_tmpBF9,
sizeof(struct _tuple1*),_tmpBF8),((((_tmpBFB=_tmpBF8,_tmpBFD(& _tmpBFC,& _tmpBFB,&
_tmpBF9))),_tmpBFA)))))));if(_tmp4AE != 0){struct Cyc_List_List*_tmp579=0;{int i=
_tmp4AE - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpBFE;_tmp579=((_tmpBFE=
_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct
_tuple1*),i)),0),0)),((_tmpBFE->tl=_tmp579,_tmpBFE))))));}}s=Cyc_Absyn_declare_stmt(
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
tud,_tmp57D),0),s,0);{const char*_tmpBFF;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp57D,((_tmpBFF="_struct",_tag_dyneither(_tmpBFF,
sizeof(char),8))))),0,s,0);};};}};}else{struct _tuple16*_tmpC00[3];struct Cyc_List_List*
_tmp580=(_tmpC00[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC00[1]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((_tmpC00[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC00,sizeof(struct _tuple16*),3)))))));s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}};}else{{int i=0;for(0;_tmp4AB
!= 0;(_tmp4AB=_tmp4AB->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4AB->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4AB->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL208;}_LL225: {struct Cyc_Absyn_Throw_e_struct*_tmp4B1=(struct Cyc_Absyn_Throw_e_struct*)
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
new_typ);Cyc_Toc_exp_to_c(nv,_tmp4BB);{struct _tuple0 _tmpC01;struct _tuple0 _tmp58A=(
_tmpC01.f1=Cyc_Tcutil_compress(old_t2),((_tmpC01.f2=Cyc_Tcutil_compress(new_typ),
_tmpC01)));void*_tmp58B;struct Cyc_Absyn_PtrInfo _tmp58D;void*_tmp58E;struct Cyc_Absyn_PtrInfo
_tmp590;void*_tmp591;struct Cyc_Absyn_PtrInfo _tmp593;void*_tmp594;_LL2B2: _tmp58B=
_tmp58A.f1;{struct Cyc_Absyn_PointerType_struct*_tmp58C=(struct Cyc_Absyn_PointerType_struct*)
_tmp58B;if(_tmp58C->tag != 5)goto _LL2B4;else{_tmp58D=_tmp58C->f1;}};_tmp58E=
_tmp58A.f2;{struct Cyc_Absyn_PointerType_struct*_tmp58F=(struct Cyc_Absyn_PointerType_struct*)
_tmp58E;if(_tmp58F->tag != 5)goto _LL2B4;else{_tmp590=_tmp58F->f1;}};_LL2B3: {int
_tmp596=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp58D.ptr_atts).nullable);int _tmp597=((int(*)(int y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_def)(0,(_tmp590.ptr_atts).nullable);void*_tmp598=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(
_tmp58D.ptr_atts).bounds);void*_tmp599=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp590.ptr_atts).bounds);int
_tmp59A=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp58D.ptr_atts).zero_term);int _tmp59B=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp590.ptr_atts).zero_term);{struct _tuple0 _tmpC02;
struct _tuple0 _tmp59D=(_tmpC02.f1=_tmp598,((_tmpC02.f2=_tmp599,_tmpC02)));void*
_tmp59E;struct Cyc_Absyn_Exp*_tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A3;
void*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A6;void*_tmp5A7;void*_tmp5A9;void*_tmp5AB;
struct Cyc_Absyn_Exp*_tmp5AD;void*_tmp5AE;void*_tmp5B0;_LL2B9: _tmp59E=_tmp59D.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp59F=(struct Cyc_Absyn_Upper_b_struct*)_tmp59E;
if(_tmp59F->tag != 1)goto _LL2BB;else{_tmp5A0=_tmp59F->f1;}};_tmp5A1=_tmp59D.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp5A2=(struct Cyc_Absyn_Upper_b_struct*)_tmp5A1;
if(_tmp5A2->tag != 1)goto _LL2BB;else{_tmp5A3=_tmp5A2->f1;}};_LL2BA: if((!Cyc_Evexp_c_can_eval(
_tmp5A0) || !Cyc_Evexp_c_can_eval(_tmp5A3)) && !Cyc_Evexp_same_const_exp(_tmp5A0,
_tmp5A3)){const char*_tmpC05;void*_tmpC04;(_tmpC04=0,Cyc_Tcutil_terr(e->loc,((
_tmpC05="can't validate cast due to potential size differences",_tag_dyneither(
_tmpC05,sizeof(char),54))),_tag_dyneither(_tmpC04,sizeof(void*),0)));}if(_tmp596
 && !_tmp597){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC08;void*_tmpC07;(
_tmpC07=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC08="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC08,
sizeof(char),44))),_tag_dyneither(_tmpC07,sizeof(void*),0)));}if(_tmp4BD != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC0C;void*_tmpC0B[1];struct Cyc_String_pa_struct _tmpC0A;(_tmpC0A.tag=
0,((_tmpC0A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpC0B[0]=& _tmpC0A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpC0C="null-check conversion mis-classified: %s",
_tag_dyneither(_tmpC0C,sizeof(char),41))),_tag_dyneither(_tmpC0B,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp4BB);if(do_null_check){if(!_tmp4BC){
const char*_tmpC0F;void*_tmpC0E;(_tmpC0E=0,Cyc_Tcutil_warn(e->loc,((_tmpC0F="inserted null check due to implicit cast from * to @ type",
_tag_dyneither(_tmpC0F,sizeof(char),58))),_tag_dyneither(_tmpC0E,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC10;e->r=Cyc_Toc_cast_it_r(*_tmp4BA,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC10=_cycalloc(sizeof(*_tmpC10)),((_tmpC10->hd=
_tmp4BB,((_tmpC10->tl=0,_tmpC10)))))),0));};}};}goto _LL2B8;_LL2BB: _tmp5A4=
_tmp59D.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp5A5=(struct Cyc_Absyn_Upper_b_struct*)
_tmp5A4;if(_tmp5A5->tag != 1)goto _LL2BD;else{_tmp5A6=_tmp5A5->f1;}};_tmp5A7=
_tmp59D.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5A8=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5A7;if(_tmp5A8->tag != 0)goto _LL2BD;};_LL2BC: if(!Cyc_Evexp_c_can_eval(_tmp5A6)){
const char*_tmpC13;void*_tmpC12;(_tmpC12=0,Cyc_Tcutil_terr(e->loc,((_tmpC13="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC13,sizeof(char),71))),_tag_dyneither(_tmpC12,sizeof(void*),0)));}
if(_tmp4BD == Cyc_Absyn_NonNull_to_Null){const char*_tmpC17;void*_tmpC16[1];struct
Cyc_String_pa_struct _tmpC15;(_tmpC15.tag=0,((_tmpC15.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC16[0]=& _tmpC15,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC17="conversion mis-classified as null-check: %s",
_tag_dyneither(_tmpC17,sizeof(char),44))),_tag_dyneither(_tmpC16,sizeof(void*),1)))))));}
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
_LL2C5:{struct Cyc_Absyn_Exp*_tmpC18[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp4BB),((_tmpC18[1]=_tmp5A6,((_tmpC18[0]=
_tmp5C3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC18,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}goto _LL2C1;
_LL2C1:;}if(!_tmp59B  && !(_tmp590.elt_tq).real_const)arg3=Cyc_Absyn_prim2_exp(
Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5C9=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp590.elt_typ),0);struct Cyc_Absyn_Exp*_tmpC19[3];struct Cyc_Absyn_Exp*
_tmp5CA=Cyc_Absyn_fncall_exp(_tmp5C1,((_tmpC19[2]=arg3,((_tmpC19[1]=_tmp5C9,((
_tmpC19[0]=_tmp5C3,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC19,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);struct Cyc_Absyn_Stmt*
_tmp5CB=Cyc_Absyn_exp_stmt(_tmp5CA,0);_tmp5CB=Cyc_Absyn_declare_stmt(_tmp5C2,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp4BB,_tmp5CB,0);e->r=Cyc_Toc_stmt_exp_r(_tmp5CB);};}
else{struct Cyc_Absyn_Exp*_tmpC1A[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5C1,((_tmpC1A[
2]=_tmp5A6,((_tmpC1A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp590.elt_typ),
0),((_tmpC1A[0]=_tmp4BB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC1A,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}goto _LL2B8;
_LL2BD: _tmp5A9=_tmp59D.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp5AA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp5A9;if(_tmp5AA->tag != 0)goto _LL2BF;};_tmp5AB=_tmp59D.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp5AC=(struct Cyc_Absyn_Upper_b_struct*)_tmp5AB;if(_tmp5AC->tag != 1)goto _LL2BF;
else{_tmp5AD=_tmp5AC->f1;}};_LL2BE: if(!Cyc_Evexp_c_can_eval(_tmp5AD)){const char*
_tmpC1D;void*_tmpC1C;(_tmpC1C=0,Cyc_Tcutil_terr(e->loc,((_tmpC1D="cannot perform coercion since numelts cannot be determined statically.",
_tag_dyneither(_tmpC1D,sizeof(char),71))),_tag_dyneither(_tmpC1C,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC20;void*_tmpC1F;(_tmpC1F=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC20="can't coerce t? to t* or t@ at the top-level",
_tag_dyneither(_tmpC20,sizeof(char),45))),_tag_dyneither(_tmpC1F,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5D2=_tmp5AD;if(_tmp59A  && !_tmp59B)_tmp5D2=Cyc_Absyn_add_exp(
_tmp5AD,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp5D3=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC21[3];struct Cyc_Absyn_Exp*_tmp5D4=Cyc_Absyn_fncall_exp(
_tmp5D3,((_tmpC21[2]=_tmp5D2,((_tmpC21[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp58D.elt_typ),0),((_tmpC21[0]=_tmp4BB,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC21,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp597){struct Cyc_List_List*_tmpC22;_tmp5D4->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc__check_null_e,((_tmpC22=_cycalloc(sizeof(*_tmpC22)),((_tmpC22->hd=Cyc_Absyn_copy_exp(
_tmp5D4),((_tmpC22->tl=0,_tmpC22)))))));}e->r=Cyc_Toc_cast_it_r(*_tmp4BA,_tmp5D4);
goto _LL2B8;};};_LL2BF: _tmp5AE=_tmp59D.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp5AF=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5AE;if(_tmp5AF->tag != 0)goto
_LL2B8;};_tmp5B0=_tmp59D.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp5B1=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp5B0;if(_tmp5B1->tag != 0)goto _LL2B8;};_LL2C0:
DynCast: if((_tmp59A  && !_tmp59B) && !(_tmp590.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpC25;void*_tmpC24;(_tmpC24=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC25="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dyneither(_tmpC25,sizeof(char),70))),_tag_dyneither(_tmpC24,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp5D9=Cyc_Toc__dyneither_ptr_decrease_size_e;struct Cyc_Absyn_Exp*
_tmpC26[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5D9,((_tmpC26[2]=Cyc_Absyn_uint_exp(1,0),((
_tmpC26[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58D.elt_typ),0),((
_tmpC26[0]=_tmp4BB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC26,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}goto _LL2B8;
_LL2B8:;}goto _LL2B1;}_LL2B4: _tmp591=_tmp58A.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp592=(struct Cyc_Absyn_PointerType_struct*)_tmp591;if(_tmp592->tag != 5)goto
_LL2B6;else{_tmp593=_tmp592->f1;}};_tmp594=_tmp58A.f2;{struct Cyc_Absyn_IntType_struct*
_tmp595=(struct Cyc_Absyn_IntType_struct*)_tmp594;if(_tmp595->tag != 6)goto _LL2B6;};
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
_tmp5E1=_tmp5DE->f3;}}_LL2CD: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC2A;void*
_tmpC29[1];struct Cyc_String_pa_struct _tmpC28;(_tmpC28.tag=0,((_tmpC28.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4BF->loc)),((
_tmpC29[0]=& _tmpC28,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC2A="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC2A,
sizeof(char),42))),_tag_dyneither(_tmpC29,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4BF->topt))->v,_tmp5E0 != 0,1,0,
_tmp5E1,_tmp5DF))->r;goto _LL2CB;_LL2CE: {struct Cyc_Absyn_Tuple_e_struct*_tmp5E2=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp5DD;if(_tmp5E2->tag != 26)goto _LL2D0;else{
_tmp5E3=_tmp5E2->f1;}}_LL2CF: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC2E;void*
_tmpC2D[1];struct Cyc_String_pa_struct _tmpC2C;(_tmpC2C.tag=0,((_tmpC2C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp4BF->loc)),((
_tmpC2D[0]=& _tmpC2C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpC2E="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC2E,
sizeof(char),42))),_tag_dyneither(_tmpC2D,sizeof(void*),1)))))));}e->r=(Cyc_Toc_init_tuple(
nv,1,0,_tmp5E3))->r;goto _LL2CB;_LL2D0:;_LL2D1: Cyc_Toc_exp_to_c(nv,_tmp4BF);if(!
Cyc_Absyn_is_lvalue(_tmp4BF)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp4BF,0,Cyc_Toc_address_lvalue,1);e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp4BF);}goto _LL2CB;
_LL2CB:;}goto _LL208;_LL22F: {struct Cyc_Absyn_New_e_struct*_tmp4C0=(struct Cyc_Absyn_New_e_struct*)
_tmp481;if(_tmp4C0->tag != 17)goto _LL231;else{_tmp4C1=_tmp4C0->f1;_tmp4C2=_tmp4C0->f2;}}
_LL230: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpC32;void*_tmpC31[1];struct Cyc_String_pa_struct
_tmpC30;(_tmpC30.tag=0,((_tmpC30.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp4C2->loc)),((_tmpC31[0]=& _tmpC30,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC32="%s: new at top-level",
_tag_dyneither(_tmpC32,sizeof(char),21))),_tag_dyneither(_tmpC31,sizeof(void*),1)))))));}{
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
goto _LL2DD;_LL2E0:;_LL2E1: {const char*_tmpC35;void*_tmpC34;old_elt_typ=((_tmpC34=
0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC35="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(
_tmpC35,sizeof(char),52))),_tag_dyneither(_tmpC34,sizeof(void*),0))));}_LL2DD:;}{
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp608=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp609=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5EF),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4C1 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp609);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp4C1;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp609);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp5FC,_tmp608,(struct Cyc_Absyn_Exp*)e1,_tmp5FE,0));goto
_LL2D2;};}_LL2D5: {struct Cyc_Absyn_Comprehension_e_struct*_tmp5F0=(struct Cyc_Absyn_Comprehension_e_struct*)
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
_LL2E5:;_LL2E6: {const char*_tmpC38;void*_tmpC37;(_tmpC37=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC38="exp_to_c: comprehension not an array type",
_tag_dyneither(_tmpC38,sizeof(char),42))),_tag_dyneither(_tmpC37,sizeof(void*),0)));}
_LL2E2:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5F3->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp5F2);{struct Cyc_Absyn_Exp*_tmp614=
Cyc_Absyn_var_exp(max,0);if(_tmp5F4)_tmp614=Cyc_Absyn_add_exp(_tmp614,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp5F1,Cyc_Absyn_var_exp(max,0),_tmp5F3,_tmp5F4,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp616;struct _RegionHandle*_tmp617;struct Cyc_Toc_Env*_tmp615=
nv;_tmp616=*_tmp615;_tmp617=_tmp616.rgn;{struct _tuple19*_tmpC3B;struct Cyc_List_List*
_tmpC3A;struct Cyc_List_List*decls=(_tmpC3A=_region_malloc(_tmp617,sizeof(*
_tmpC3A)),((_tmpC3A->hd=((_tmpC3B=_region_malloc(_tmp617,sizeof(*_tmpC3B)),((
_tmpC3B->f1=max,((_tmpC3B->f2=Cyc_Absyn_uint_typ,((_tmpC3B->f3=(struct Cyc_Absyn_Exp*)
_tmp5F2,_tmpC3B)))))))),((_tmpC3A->tl=0,_tmpC3A)))));struct Cyc_Absyn_Exp*ai;if(
_tmp4C1 == 0  || Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*_tmpC3C[2];ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((_tmpC3C[1]=_tmp614,((
_tmpC3C[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3C,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp4C1;Cyc_Toc_exp_to_c(
nv,r);{struct Cyc_Absyn_Exp*_tmpC3D[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpC3D[1]=_tmp614,((_tmpC3D[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpC3D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{struct Cyc_Absyn_Exp*
ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct _tuple19*_tmpC40;struct Cyc_List_List*
_tmpC3F;decls=((_tmpC3F=_region_malloc(_tmp617,sizeof(*_tmpC3F)),((_tmpC3F->hd=((
_tmpC40=_region_malloc(_tmp617,sizeof(*_tmpC40)),((_tmpC40->f1=a,((_tmpC40->f2=
ptr_typ,((_tmpC40->f3=(struct Cyc_Absyn_Exp*)ainit,_tmpC40)))))))),((_tmpC3F->tl=
decls,_tmpC3F))))));}if(is_dyneither_ptr){struct _tuple1*_tmp61C=Cyc_Toc_temp_var();
void*_tmp61D=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp61E=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC41[3];struct Cyc_Absyn_Exp*_tmp61F=Cyc_Absyn_fncall_exp(
_tmp61E,((_tmpC41[2]=_tmp614,((_tmpC41[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((
_tmpC41[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmpC41,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);{
struct _tuple19*_tmpC44;struct Cyc_List_List*_tmpC43;decls=((_tmpC43=
_region_malloc(_tmp617,sizeof(*_tmpC43)),((_tmpC43->hd=((_tmpC44=_region_malloc(
_tmp617,sizeof(*_tmpC44)),((_tmpC44->f1=_tmp61C,((_tmpC44->f2=_tmp61D,((_tmpC44->f3=(
struct Cyc_Absyn_Exp*)_tmp61F,_tmpC44)))))))),((_tmpC43->tl=decls,_tmpC43))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp61C,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp623=decls;for(0;_tmp623 != 0;_tmp623=_tmp623->tl){struct
_tuple1*_tmp625;void*_tmp626;struct Cyc_Absyn_Exp*_tmp627;struct _tuple19 _tmp624=*((
struct _tuple19*)_tmp623->hd);_tmp625=_tmp624.f1;_tmp626=_tmp624.f2;_tmp627=
_tmp624.f3;s=Cyc_Absyn_declare_stmt(_tmp625,_tmp626,_tmp627,s,0);}}e->r=Cyc_Toc_stmt_exp_r(
s);};}goto _LL2D2;};};};}_LL2D7: {struct Cyc_Absyn_Aggregate_e_struct*_tmp5F5=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp5ED;if(_tmp5F5->tag != 30)goto _LL2D9;
else{_tmp5F6=_tmp5F5->f1;_tmp5F7=_tmp5F5->f2;_tmp5F8=_tmp5F5->f3;_tmp5F9=_tmp5F5->f4;}}
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
_tmp62E=(void*)_tmp62D->f1;_tmp62F=_tmp62D->f2;}}_LL2E9:{struct _tuple0 _tmpC45;
struct _tuple0 _tmp631=(_tmpC45.f1=Cyc_Tcutil_compress(_tmp62E),((_tmpC45.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp62F->topt))->v),_tmpC45)));void*
_tmp632;struct Cyc_Absyn_PtrInfo _tmp634;void*_tmp635;struct Cyc_Absyn_PtrAtts
_tmp636;union Cyc_Absyn_Constraint*_tmp637;void*_tmp638;struct Cyc_Absyn_PtrInfo
_tmp63A;struct Cyc_Absyn_PtrAtts _tmp63B;union Cyc_Absyn_Constraint*_tmp63C;_LL2ED:
_tmp632=_tmp631.f1;{struct Cyc_Absyn_PointerType_struct*_tmp633=(struct Cyc_Absyn_PointerType_struct*)
_tmp632;if(_tmp633->tag != 5)goto _LL2EF;else{_tmp634=_tmp633->f1;_tmp635=_tmp634.elt_typ;
_tmp636=_tmp634.ptr_atts;_tmp637=_tmp636.bounds;}};_tmp638=_tmp631.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp639=(struct Cyc_Absyn_PointerType_struct*)_tmp638;if(_tmp639->tag != 5)goto
_LL2EF;else{_tmp63A=_tmp639->f1;_tmp63B=_tmp63A.ptr_atts;_tmp63C=_tmp63B.bounds;}};
_LL2EE:{struct _tuple0 _tmpC46;struct _tuple0 _tmp63E=(_tmpC46.f1=((void*(*)(void*y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp637),((
_tmpC46.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp63C),_tmpC46)));void*_tmp63F;void*_tmp641;struct Cyc_Absyn_Exp*
_tmp643;_LL2F2: _tmp63F=_tmp63E.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp640=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp63F;if(_tmp640->tag != 0)goto _LL2F4;};
_tmp641=_tmp63E.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp642=(struct Cyc_Absyn_Upper_b_struct*)
_tmp641;if(_tmp642->tag != 1)goto _LL2F4;else{_tmp643=_tmp642->f1;}};_LL2F3: Cyc_Toc_exp_to_c(
nv,_tmp62F);inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp644=Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpC47[4];e->r=Cyc_Toc_fncall_exp_r(
_tmp644,((_tmpC47[3]=_tmp643,((_tmpC47[2]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp635),0),((_tmpC47[1]=_tmp62F,((_tmpC47[0]=mexp,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC47,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))));}goto _LL2F1;};_LL2F4:;_LL2F5: goto _LL2F1;_LL2F1:;}goto _LL2EC;_LL2EF:;
_LL2F0: goto _LL2EC;_LL2EC:;}goto _LL2E7;_LL2EA:;_LL2EB: goto _LL2E7;_LL2E7:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp646=Cyc_Absyn_exp_stmt(_tmp62B,0);struct Cyc_Absyn_Exp*
_tmp647=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp4C2);_tmp646=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp62B,_tmp647,0),_tmp4C2,0),
_tmp646,0);{void*_tmp648=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp62A,_tmp648,(struct Cyc_Absyn_Exp*)mexp,_tmp646,0));};}
goto _LL2D2;};}_LL2D2:;}goto _LL208;_LL231: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp4C3=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp481;if(_tmp4C3->tag != 19)goto
_LL233;else{_tmp4C4=_tmp4C3->f1;}}_LL232: Cyc_Toc_exp_to_c(nv,_tmp4C4);goto _LL208;
_LL233: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp4C5=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp481;if(_tmp4C5->tag != 18)goto _LL235;else{_tmp4C6=(void*)_tmp4C5->f1;}}_LL234:{
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpC4A;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpC49;e->r=(void*)((_tmpC49=_cycalloc(sizeof(*_tmpC49)),((_tmpC49[0]=((_tmpC4A.tag=
18,((_tmpC4A.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4C6),_tmpC4A)))),_tmpC49))));}
goto _LL208;_LL235: {struct Cyc_Absyn_Gentyp_e_struct*_tmp4C7=(struct Cyc_Absyn_Gentyp_e_struct*)
_tmp481;if(_tmp4C7->tag != 21)goto _LL237;}_LL236: {const char*_tmpC4D;void*_tmpC4C;(
_tmpC4C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpC4D="__gen() in code generator",_tag_dyneither(_tmpC4D,sizeof(char),26))),
_tag_dyneither(_tmpC4C,sizeof(void*),0)));}_LL237: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp4C8=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp481;if(_tmp4C8->tag != 20)goto
_LL239;else{_tmp4C9=(void*)_tmp4C8->f1;_tmp4CA=(void*)_tmp4C8->f2;{struct Cyc_Absyn_StructField_struct*
_tmp4CB=(struct Cyc_Absyn_StructField_struct*)_tmp4CA;if(_tmp4CB->tag != 0)goto
_LL239;else{_tmp4CC=_tmp4CB->f1;}};}}_LL238:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC57;struct Cyc_Absyn_StructField_struct _tmpC56;struct Cyc_Absyn_StructField_struct*
_tmpC55;struct Cyc_Absyn_Offsetof_e_struct*_tmpC54;e->r=(void*)((_tmpC54=
_cycalloc(sizeof(*_tmpC54)),((_tmpC54[0]=((_tmpC57.tag=20,((_tmpC57.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4C9),((_tmpC57.f2=(void*)((void*)((_tmpC55=_cycalloc(
sizeof(*_tmpC55)),((_tmpC55[0]=((_tmpC56.tag=0,((_tmpC56.f1=_tmp4CC,_tmpC56)))),
_tmpC55))))),_tmpC57)))))),_tmpC54))));}goto _LL208;_LL239: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp4CD=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp481;if(_tmp4CD->tag != 20)goto
_LL23B;else{_tmp4CE=(void*)_tmp4CD->f1;_tmp4CF=(void*)_tmp4CD->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp4D0=(struct Cyc_Absyn_TupleIndex_struct*)_tmp4CF;if(_tmp4D0->tag != 1)goto
_LL23B;else{_tmp4D1=_tmp4D0->f1;}};}}_LL23A:{void*_tmp651=Cyc_Tcutil_compress(
_tmp4CE);struct Cyc_Absyn_AggrInfo _tmp653;union Cyc_Absyn_AggrInfoU _tmp654;struct
Cyc_List_List*_tmp656;_LL2F7: {struct Cyc_Absyn_AggrType_struct*_tmp652=(struct
Cyc_Absyn_AggrType_struct*)_tmp651;if(_tmp652->tag != 12)goto _LL2F9;else{_tmp653=
_tmp652->f1;_tmp654=_tmp653.aggr_info;}}_LL2F8: {struct Cyc_Absyn_Aggrdecl*
_tmp659=Cyc_Absyn_get_known_aggrdecl(_tmp654);if(_tmp659->impl == 0){const char*
_tmpC5A;void*_tmpC59;(_tmpC59=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5A="struct fields must be known",
_tag_dyneither(_tmpC5A,sizeof(char),28))),_tag_dyneither(_tmpC59,sizeof(void*),0)));}
_tmp656=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp659->impl))->fields;goto
_LL2FA;}_LL2F9: {struct Cyc_Absyn_AnonAggrType_struct*_tmp655=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp651;if(_tmp655->tag != 13)goto _LL2FB;else{_tmp656=_tmp655->f2;}}_LL2FA: {
struct Cyc_Absyn_Aggrfield*_tmp65C=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp656,(int)_tmp4D1);{struct Cyc_Absyn_Offsetof_e_struct
_tmpC64;struct Cyc_Absyn_StructField_struct _tmpC63;struct Cyc_Absyn_StructField_struct*
_tmpC62;struct Cyc_Absyn_Offsetof_e_struct*_tmpC61;e->r=(void*)((_tmpC61=
_cycalloc(sizeof(*_tmpC61)),((_tmpC61[0]=((_tmpC64.tag=20,((_tmpC64.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC64.f2=(void*)((void*)((_tmpC62=_cycalloc(
sizeof(*_tmpC62)),((_tmpC62[0]=((_tmpC63.tag=0,((_tmpC63.f1=_tmp65C->name,
_tmpC63)))),_tmpC62))))),_tmpC64)))))),_tmpC61))));}goto _LL2F6;}_LL2FB: {struct
Cyc_Absyn_TupleType_struct*_tmp657=(struct Cyc_Absyn_TupleType_struct*)_tmp651;
if(_tmp657->tag != 11)goto _LL2FD;}_LL2FC:{struct Cyc_Absyn_Offsetof_e_struct
_tmpC6E;struct Cyc_Absyn_StructField_struct _tmpC6D;struct Cyc_Absyn_StructField_struct*
_tmpC6C;struct Cyc_Absyn_Offsetof_e_struct*_tmpC6B;e->r=(void*)((_tmpC6B=
_cycalloc(sizeof(*_tmpC6B)),((_tmpC6B[0]=((_tmpC6E.tag=20,((_tmpC6E.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC6E.f2=(void*)((void*)((_tmpC6C=_cycalloc(
sizeof(*_tmpC6C)),((_tmpC6C[0]=((_tmpC6D.tag=0,((_tmpC6D.f1=Cyc_Absyn_fieldname((
int)(_tmp4D1 + 1)),_tmpC6D)))),_tmpC6C))))),_tmpC6E)))))),_tmpC6B))));}goto _LL2F6;
_LL2FD: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp658=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp651;if(_tmp658->tag != 4)goto _LL2FF;}_LL2FE: if(_tmp4D1 == 0){struct Cyc_Absyn_Offsetof_e_struct
_tmpC78;struct Cyc_Absyn_StructField_struct _tmpC77;struct Cyc_Absyn_StructField_struct*
_tmpC76;struct Cyc_Absyn_Offsetof_e_struct*_tmpC75;e->r=(void*)((_tmpC75=
_cycalloc(sizeof(*_tmpC75)),((_tmpC75[0]=((_tmpC78.tag=20,((_tmpC78.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC78.f2=(void*)((void*)((_tmpC76=_cycalloc(
sizeof(*_tmpC76)),((_tmpC76[0]=((_tmpC77.tag=0,((_tmpC77.f1=Cyc_Toc_tag_sp,
_tmpC77)))),_tmpC76))))),_tmpC78)))))),_tmpC75))));}else{struct Cyc_Absyn_Offsetof_e_struct
_tmpC82;struct Cyc_Absyn_StructField_struct _tmpC81;struct Cyc_Absyn_StructField_struct*
_tmpC80;struct Cyc_Absyn_Offsetof_e_struct*_tmpC7F;e->r=(void*)((_tmpC7F=
_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F[0]=((_tmpC82.tag=20,((_tmpC82.f1=(void*)
Cyc_Toc_typ_to_c_array(_tmp4CE),((_tmpC82.f2=(void*)((void*)((_tmpC80=_cycalloc(
sizeof(*_tmpC80)),((_tmpC80[0]=((_tmpC81.tag=0,((_tmpC81.f1=Cyc_Absyn_fieldname((
int)_tmp4D1),_tmpC81)))),_tmpC80))))),_tmpC82)))))),_tmpC7F))));}goto _LL2F6;
_LL2FF:;_LL300: {const char*_tmpC85;void*_tmpC84;(_tmpC84=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC85="impossible type for offsetof tuple index",
_tag_dyneither(_tmpC85,sizeof(char),41))),_tag_dyneither(_tmpC84,sizeof(void*),0)));}
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
const char*_tmpC88;void*_tmpC87;(_tmpC87=0,Cyc_Tcutil_warn(e->loc,((_tmpC88="inserted null check due to dereference",
_tag_dyneither(_tmpC88,sizeof(char),39))),_tag_dyneither(_tmpC87,sizeof(void*),0)));}{
struct Cyc_List_List*_tmpC89;_tmp4D3->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
_tmp66F),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((_tmpC89=_cycalloc(sizeof(*
_tmpC89)),((_tmpC89->hd=Cyc_Absyn_copy_exp(_tmp4D3),((_tmpC89->tl=0,_tmpC89)))))),
0));};}if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp679)){unsigned int _tmp682;int _tmp683;struct _tuple11 _tmp681=Cyc_Evexp_eval_const_uint_exp(
_tmp67C);_tmp682=_tmp681.f1;_tmp683=_tmp681.f2;if(!_tmp683  || _tmp682 <= 0){const
char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,Cyc_Tcutil_terr(e->loc,((_tmpC8C="cannot determine dereference is in bounds",
_tag_dyneither(_tmpC8C,sizeof(char),42))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}}
goto _LL306;}_LL309: {struct Cyc_Absyn_DynEither_b_struct*_tmp67D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp67A;if(_tmp67D->tag != 0)goto _LL306;}_LL30A: {struct Cyc_Absyn_Exp*_tmp686=Cyc_Absyn_uint_exp(
0,0);{struct Cyc_Core_Opt*_tmpC8D;_tmp686->topt=((_tmpC8D=_cycalloc(sizeof(*
_tmpC8D)),((_tmpC8D->v=Cyc_Absyn_uint_typ,_tmpC8D))));}e->r=Cyc_Toc_subscript_exp_r(
_tmp4D3,_tmp686);Cyc_Toc_exp_to_c(nv,e);goto _LL306;}_LL306:;}goto _LL301;_LL304:;
_LL305: {const char*_tmpC90;void*_tmpC8F;(_tmpC8F=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC90="exp_to_c: Deref: non-pointer",
_tag_dyneither(_tmpC90,sizeof(char),29))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}
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
_tmp698=_tmp696.f2;if(_tmp698){if(_tmp697 < 1){const char*_tmpC93;void*_tmpC92;(
_tmpC92=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpC93="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpC93,
sizeof(char),44))),_tag_dyneither(_tmpC92,sizeof(void*),0)));}if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){const char*_tmpC96;void*_tmpC95;(_tmpC95=0,Cyc_Tcutil_warn(
e->loc,((_tmpC96="inserted null check due to dereference",_tag_dyneither(_tmpC96,
sizeof(char),39))),_tag_dyneither(_tmpC95,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpC97[1];_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpC97[0]=Cyc_Absyn_new_exp(_tmp4DA->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC97,sizeof(struct Cyc_Absyn_Exp*),
1)))),0));};}}else{if(!Cyc_Evexp_c_can_eval(_tmp694)){const char*_tmpC9A;void*
_tmpC99;(_tmpC99=0,Cyc_Tcutil_terr(e->loc,((_tmpC9A="cannot determine pointer dereference in bounds",
_tag_dyneither(_tmpC9A,sizeof(char),47))),_tag_dyneither(_tmpC99,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpC9B[4];_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(
e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((_tmpC9B[3]=
Cyc_Absyn_uint_exp(0,0),((_tmpC9B[2]=Cyc_Absyn_sizeoftyp_exp(_tmp68B,0),((
_tmpC9B[1]=_tmp694,((_tmpC9B[0]=Cyc_Absyn_new_exp(_tmp4DA->r,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC9B,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0));};}goto _LL30B;}_LL30E: {struct Cyc_Absyn_DynEither_b_struct*
_tmp695=(struct Cyc_Absyn_DynEither_b_struct*)_tmp692;if(_tmp695->tag != 0)goto
_LL30B;}_LL30F: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp68B);{struct Cyc_Absyn_Exp*
_tmpC9C[3];_tmp4DA->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp68C),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_dyneither_subscript_e,((_tmpC9C[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpC9C[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpC9C[0]=Cyc_Absyn_new_exp(_tmp4DA->r,
0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC9C,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}goto _LL30B;}_LL30B:;}if(
_tmp4DC  && _tmp4DD)e->r=Cyc_Toc_check_tagged_union(_tmp4DA,Cyc_Toc_typ_to_c(
e1typ),_tmp68B,_tmp4DB,Cyc_Absyn_aggrarrow_exp);if(is_poly  && _tmp4DD)e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(
e->r,0),0))->r;goto _LL208;};}_LL241: {struct Cyc_Absyn_Subscript_e_struct*_tmp4DE=(
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
_tmp6B1=_tmp6AF.f2;if(!_tmp6B1){const char*_tmpC9F;void*_tmpC9E;(_tmpC9E=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC9F="unknown tuple subscript in translation to C",
_tag_dyneither(_tmpC9F,sizeof(char),44))),_tag_dyneither(_tmpC9E,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp4DF,Cyc_Absyn_fieldname((int)(_tmp6B0 + 1)));
goto _LL310;};_LL313: {struct Cyc_Absyn_PointerType_struct*_tmp6A6=(struct Cyc_Absyn_PointerType_struct*)
_tmp6A3;if(_tmp6A6->tag != 5)goto _LL315;else{_tmp6A7=_tmp6A6->f1;_tmp6A8=_tmp6A7.elt_typ;
_tmp6A9=_tmp6A7.elt_tq;_tmp6AA=_tmp6A7.ptr_atts;_tmp6AB=_tmp6AA.rgn;_tmp6AC=
_tmp6AA.nullable;_tmp6AD=_tmp6AA.bounds;_tmp6AE=_tmp6AA.zero_term;}}_LL314: {
struct Cyc_List_List*_tmp6B4=Cyc_Toc_get_relns(_tmp4DF);int in_bnds=0;{void*
_tmp6B5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp6AD);_LL318:;_LL319: in_bnds=Cyc_Toc_check_bounds(_tmp6B4,_tmp4DF,_tmp4E0);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpCA3;void*_tmpCA2[1];
struct Cyc_String_pa_struct _tmpCA1;(_tmpCA1.tag=0,((_tmpCA1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCA2[0]=&
_tmpCA1,Cyc_Tcutil_warn(e->loc,((_tmpCA3="bounds check necessary for %s",
_tag_dyneither(_tmpCA3,sizeof(char),30))),_tag_dyneither(_tmpCA2,sizeof(void*),1)))))));}
_LL317:;}Cyc_Toc_exp_to_c(nv,_tmp4DF);Cyc_Toc_exp_to_c(nv,_tmp4E0);++ Cyc_Toc_total_bounds_checks;{
void*_tmp6B9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp6AD);struct Cyc_Absyn_Exp*_tmp6BB;_LL31B: {struct Cyc_Absyn_Upper_b_struct*
_tmp6BA=(struct Cyc_Absyn_Upper_b_struct*)_tmp6B9;if(_tmp6BA->tag != 1)goto _LL31D;
else{_tmp6BB=_tmp6BA->f1;}}_LL31C: {int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp6AC);void*ta1=Cyc_Toc_typ_to_c(_tmp6A8);void*ta2=
Cyc_Absyn_cstar_typ(ta1,_tmp6A9);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;
else{if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6AE)){if(!Cyc_Evexp_c_can_eval(_tmp6BB)){const char*_tmpCA6;void*_tmpCA5;(
_tmpCA5=0,Cyc_Tcutil_terr(e->loc,((_tmpCA6="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCA6,sizeof(char),40))),_tag_dyneither(_tmpCA5,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp4DF);struct Cyc_Absyn_Exp*_tmpCA7[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpCA7[2]=_tmp4E0,((_tmpCA7[1]=_tmp6BB,((
_tmpCA7[0]=_tmp4DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCA7,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp6BB)){const char*_tmpCAA;void*_tmpCA9;(
_tmpCA9=0,Cyc_Tcutil_terr(e->loc,((_tmpCAA="cannot determine subscript is in bounds",
_tag_dyneither(_tmpCAA,sizeof(char),40))),_tag_dyneither(_tmpCA9,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){const char*_tmpCAD;void*_tmpCAC;(_tmpCAC=0,Cyc_Tcutil_warn(
e->loc,((_tmpCAD="inserted null check due to dereference",_tag_dyneither(_tmpCAD,
sizeof(char),39))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCAE[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpCAE[3]=_tmp4E0,((_tmpCAE[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCAE[1]=
_tmp6BB,((_tmpCAE[0]=_tmp4DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCAE,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{if(
!Cyc_Evexp_c_can_eval(_tmp6BB)){const char*_tmpCB1;void*_tmpCB0;(_tmpCB0=0,Cyc_Tcutil_terr(
e->loc,((_tmpCB1="cannot determine subscript is in bounds",_tag_dyneither(
_tmpCB1,sizeof(char),40))),_tag_dyneither(_tmpCB0,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*
_tmpCB2[2];_tmp4E0->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((
_tmpCB2[1]=Cyc_Absyn_copy_exp(_tmp4E0),((_tmpCB2[0]=_tmp6BB,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB2,sizeof(struct Cyc_Absyn_Exp*),
2)))))));};}}}goto _LL31A;}_LL31D: {struct Cyc_Absyn_DynEither_b_struct*_tmp6BC=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp6B9;if(_tmp6BC->tag != 0)goto _LL31A;}
_LL31E: {void*ta1=Cyc_Toc_typ_to_c_array(_tmp6A8);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A9),
Cyc_Absyn_aggrmember_exp(_tmp4DF,Cyc_Toc_curr_sp,0)),_tmp4E0);}else{struct Cyc_Absyn_Exp*
_tmp6C8=Cyc_Toc__check_dyneither_subscript_e;struct Cyc_Absyn_Exp*_tmpCB3[3];e->r=
Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6A9),Cyc_Absyn_fncall_exp(
_tmp6C8,((_tmpCB3[2]=_tmp4E0,((_tmpCB3[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((
_tmpCB3[0]=_tmp4DF,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCB3,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL31A;}
_LL31A:;}goto _LL310;}_LL315:;_LL316: {const char*_tmpCB6;void*_tmpCB5;(_tmpCB5=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCB6="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(
_tmpCB6,sizeof(char),49))),_tag_dyneither(_tmpCB5,sizeof(void*),0)));}_LL310:;}
goto _LL208;}_LL243: {struct Cyc_Absyn_Tuple_e_struct*_tmp4E1=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp481;if(_tmp4E1->tag != 26)goto _LL245;else{_tmp4E2=_tmp4E1->f1;}}_LL244: if(!
Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp4E2))->r;else{struct
Cyc_List_List*_tmp6CC=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4E2);void*_tmp6CD=Cyc_Toc_add_tuple_type(
_tmp6CC);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp4E2 != 0;(_tmp4E2=_tmp4E2->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4E2->hd);{struct _tuple16*
_tmpCB9;struct Cyc_List_List*_tmpCB8;dles=((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((
_tmpCB8->hd=((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9->f1=0,((_tmpCB9->f2=(
struct Cyc_Absyn_Exp*)_tmp4E2->hd,_tmpCB9)))))),((_tmpCB8->tl=dles,_tmpCB8))))));};}}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=
Cyc_Toc_unresolvedmem_exp_r(0,dles);}goto _LL208;_LL245: {struct Cyc_Absyn_Array_e_struct*
_tmp4E3=(struct Cyc_Absyn_Array_e_struct*)_tmp481;if(_tmp4E3->tag != 28)goto _LL247;
else{_tmp4E4=_tmp4E3->f1;}}_LL246: e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp4E4);{
struct Cyc_List_List*_tmp6D0=_tmp4E4;for(0;_tmp6D0 != 0;_tmp6D0=_tmp6D0->tl){
struct _tuple16 _tmp6D2;struct Cyc_Absyn_Exp*_tmp6D3;struct _tuple16*_tmp6D1=(struct
_tuple16*)_tmp6D0->hd;_tmp6D2=*_tmp6D1;_tmp6D3=_tmp6D2.f2;Cyc_Toc_exp_to_c(nv,
_tmp6D3);}}goto _LL208;_LL247: {struct Cyc_Absyn_Comprehension_e_struct*_tmp4E5=(
struct Cyc_Absyn_Comprehension_e_struct*)_tmp481;if(_tmp4E5->tag != 29)goto _LL249;
else{_tmp4E6=_tmp4E5->f1;_tmp4E7=_tmp4E5->f2;_tmp4E8=_tmp4E5->f3;_tmp4E9=_tmp4E5->f4;}}
_LL248: {unsigned int _tmp6D5;int _tmp6D6;struct _tuple11 _tmp6D4=Cyc_Evexp_eval_const_uint_exp(
_tmp4E7);_tmp6D5=_tmp6D4.f1;_tmp6D6=_tmp6D4.f2;{void*_tmp6D7=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E8->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp4E8);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp4E8)){if(!_tmp6D6){
const char*_tmpCBC;void*_tmpCBB;(_tmpCBB=0,Cyc_Tcutil_terr(_tmp4E7->loc,((_tmpCBC="cannot determine value of constant",
_tag_dyneither(_tmpCBC,sizeof(char),35))),_tag_dyneither(_tmpCBB,sizeof(void*),0)));}{
unsigned int i=0;for(0;i < _tmp6D5;++ i){struct _tuple16*_tmpCBF;struct Cyc_List_List*
_tmpCBE;es=((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE->hd=((_tmpCBF=
_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF->f1=0,((_tmpCBF->f2=_tmp4E8,_tmpCBF)))))),((
_tmpCBE->tl=es,_tmpCBE))))));}}if(_tmp4E9){struct Cyc_Absyn_Exp*_tmp6DC=Cyc_Toc_cast_it(
_tmp6D7,Cyc_Absyn_uint_exp(0,0));struct _tuple16*_tmpCC2;struct Cyc_List_List*
_tmpCC1;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=((
_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2->f1=0,((_tmpCC2->f2=_tmp6DC,
_tmpCC2)))))),((_tmpCC1->tl=0,_tmpCC1)))))));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL208;};};}_LL249: {struct Cyc_Absyn_Aggregate_e_struct*_tmp4EA=(
struct Cyc_Absyn_Aggregate_e_struct*)_tmp481;if(_tmp4EA->tag != 30)goto _LL24B;
else{_tmp4EB=_tmp4EA->f1;_tmp4EC=_tmp4EA->f2;_tmp4ED=_tmp4EA->f3;_tmp4EE=_tmp4EA->f4;}}
_LL24A: if(!Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp4EC
!= 0,0,0,_tmp4ED,_tmp4EB))->r;else{if(_tmp4EE == 0){const char*_tmpCC5;void*
_tmpCC4;(_tmpCC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpCC5="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpCC5,sizeof(
char),38))),_tag_dyneither(_tmpCC4,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp4EE;struct Cyc_Toc_Env _tmp6E2;struct
_RegionHandle*_tmp6E3;struct Cyc_Toc_Env*_tmp6E1=nv;_tmp6E2=*_tmp6E1;_tmp6E3=
_tmp6E2.rgn;{struct Cyc_List_List*_tmp6E4=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,enum 
Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp6E3,e->loc,_tmp4ED,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4EE->impl))->tagged){struct
_tuple20 _tmp6E6;struct Cyc_Absyn_Aggrfield*_tmp6E7;struct Cyc_Absyn_Exp*_tmp6E8;
struct _tuple20*_tmp6E5=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp6E4))->hd;_tmp6E6=*_tmp6E5;_tmp6E7=_tmp6E6.f1;_tmp6E8=_tmp6E6.f2;{void*
_tmp6E9=_tmp6E7->type;Cyc_Toc_exp_to_c(nv,_tmp6E8);if(Cyc_Toc_is_void_star_or_tvar(
_tmp6E9))_tmp6E8->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp6E8->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp4EE,_tmp6E7->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct _tuple16*_tmpCCA;struct _tuple16*_tmpCC9;struct _tuple16*_tmpCC8[2];
struct Cyc_List_List*_tmp6EA=(_tmpCC8[1]=((_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((
_tmpCC9->f1=0,((_tmpCC9->f2=_tmp6E8,_tmpCC9)))))),((_tmpCC8[0]=((_tmpCCA=
_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->f1=0,((_tmpCCA->f2=field_tag_exp,_tmpCCA)))))),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCC8,sizeof(struct _tuple16*),2)))));struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(
0,_tmp6EA,0);struct Cyc_Absyn_FieldName_struct*_tmpCD0;struct Cyc_Absyn_FieldName_struct
_tmpCCF;void*_tmpCCE[1];struct Cyc_List_List*ds=(_tmpCCE[0]=(void*)((_tmpCD0=
_cycalloc(sizeof(*_tmpCD0)),((_tmpCD0[0]=((_tmpCCF.tag=1,((_tmpCCF.f1=_tmp6E7->name,
_tmpCCF)))),_tmpCD0)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCCE,sizeof(void*),1)));struct _tuple16*_tmpCD3;struct _tuple16*
_tmpCD2[1];struct Cyc_List_List*dles=(_tmpCD2[0]=((_tmpCD3=_cycalloc(sizeof(*
_tmpCD3)),((_tmpCD3->f1=ds,((_tmpCD3->f2=umem,_tmpCD3)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD2,sizeof(struct
_tuple16*),1)));e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{struct Cyc_List_List*
_tmp6F3=0;struct Cyc_List_List*_tmp6F4=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp6F4 != 0;_tmp6F4=_tmp6F4->tl){struct Cyc_List_List*
_tmp6F5=_tmp6E4;for(0;_tmp6F5 != 0;_tmp6F5=_tmp6F5->tl){if((*((struct _tuple20*)
_tmp6F5->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp6F4->hd){struct _tuple20
_tmp6F7;struct Cyc_Absyn_Aggrfield*_tmp6F8;struct Cyc_Absyn_Exp*_tmp6F9;struct
_tuple20*_tmp6F6=(struct _tuple20*)_tmp6F5->hd;_tmp6F7=*_tmp6F6;_tmp6F8=_tmp6F7.f1;
_tmp6F9=_tmp6F7.f2;{void*_tmp6FA=_tmp6F8->type;Cyc_Toc_exp_to_c(nv,_tmp6F9);if(
Cyc_Toc_is_void_star_or_tvar(_tmp6FA))_tmp6F9->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp6F9->r,0));{struct _tuple16*_tmpCD6;struct Cyc_List_List*
_tmpCD5;_tmp6F3=((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5->hd=((_tmpCD6=
_cycalloc(sizeof(*_tmpCD6)),((_tmpCD6->f1=0,((_tmpCD6->f2=_tmp6F9,_tmpCD6)))))),((
_tmpCD5->tl=_tmp6F3,_tmpCD5))))));}break;};}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6F3));}};};}
goto _LL208;_LL24B: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp4EF=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp481;if(_tmp4EF->tag != 31)goto _LL24D;else{_tmp4F0=(void*)_tmp4EF->f1;_tmp4F1=
_tmp4EF->f2;}}_LL24C: {struct Cyc_List_List*fs;{void*_tmp6FD=Cyc_Tcutil_compress(
_tmp4F0);struct Cyc_List_List*_tmp6FF;_LL320: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp6FE=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp6FD;if(_tmp6FE->tag != 13)goto
_LL322;else{_tmp6FF=_tmp6FE->f2;}}_LL321: fs=_tmp6FF;goto _LL31F;_LL322:;_LL323: {
const char*_tmpCDA;void*_tmpCD9[1];struct Cyc_String_pa_struct _tmpCD8;(_tmpCD8.tag=
0,((_tmpCD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp4F0)),((_tmpCD9[0]=& _tmpCD8,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCDA="anon struct has type %s",
_tag_dyneither(_tmpCDA,sizeof(char),24))),_tag_dyneither(_tmpCD9,sizeof(void*),1)))))));}
_LL31F:;}{struct Cyc_Toc_Env _tmp704;struct _RegionHandle*_tmp705;struct Cyc_Toc_Env*
_tmp703=nv;_tmp704=*_tmp703;_tmp705=_tmp704.rgn;{struct Cyc_List_List*_tmp706=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp705,e->loc,_tmp4F1,Cyc_Absyn_StructA,fs);for(0;_tmp706 != 0;_tmp706=_tmp706->tl){
struct _tuple20 _tmp708;struct Cyc_Absyn_Aggrfield*_tmp709;struct Cyc_Absyn_Exp*
_tmp70A;struct _tuple20*_tmp707=(struct _tuple20*)_tmp706->hd;_tmp708=*_tmp707;
_tmp709=_tmp708.f1;_tmp70A=_tmp708.f2;{void*_tmp70B=_tmp709->type;Cyc_Toc_exp_to_c(
nv,_tmp70A);if(Cyc_Toc_is_void_star_or_tvar(_tmp70B))_tmp70A->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp70A->r,0));};}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp4F1);}goto _LL208;};}_LL24D: {struct Cyc_Absyn_Datatype_e_struct*_tmp4F2=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp481;if(_tmp4F2->tag != 32)goto _LL24F;else{
_tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;_tmp4F5=_tmp4F2->f3;}}_LL24E: {void*
datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp70C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp70D=Cyc_Absyn_var_exp(_tmp70C,0);struct Cyc_Absyn_Exp*
member_exp;{const char*_tmpCDB;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp4F5->name,((_tmpCDB="_struct",_tag_dyneither(_tmpCDB,sizeof(char),8)))));}
tag_exp=_tmp4F4->is_extensible?Cyc_Absyn_var_exp(_tmp4F5->name,0): Cyc_Toc_datatype_tag(
_tmp4F4,_tmp4F5->name);member_exp=_tmp70D;{struct Cyc_List_List*_tmp70F=_tmp4F5->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp4F3 != 0;(
_tmp4F3=_tmp4F3->tl,_tmp70F=_tmp70F->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp4F3->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp70F))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct _tuple16*_tmpCDE;struct
Cyc_List_List*_tmpCDD;dles=((_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->hd=((
_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE->f1=0,((_tmpCDE->f2=cur_e,_tmpCDE)))))),((
_tmpCDD->tl=dles,_tmpCDD))))));};}{struct _tuple16*_tmpCE1;struct Cyc_List_List*
_tmpCE0;dles=((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0->hd=((_tmpCE1=
_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->f1=0,((_tmpCE1->f2=tag_exp,_tmpCE1)))))),((
_tmpCE0->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
dles),_tmpCE0))))));}e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{struct Cyc_List_List*
_tmpCE2;struct Cyc_List_List*_tmp714=(_tmpCE2=_cycalloc(sizeof(*_tmpCE2)),((
_tmpCE2->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpCE2->tl=0,_tmpCE2)))));{int i=1;for(0;_tmp4F3 != 0;(((_tmp4F3=
_tmp4F3->tl,i ++)),_tmp70F=_tmp70F->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp4F3->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp70F))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp715=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpCE3;_tmp714=((_tmpCE3=_cycalloc(sizeof(*
_tmpCE3)),((_tmpCE3->hd=_tmp715,((_tmpCE3->tl=_tmp714,_tmpCE3))))));};}}{struct
Cyc_Absyn_Stmt*_tmp717=Cyc_Absyn_exp_stmt(_tmp70D,0);struct Cyc_List_List*_tmpCE4;
struct Cyc_Absyn_Stmt*_tmp718=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((
_tmpCE4->hd=_tmp717,((_tmpCE4->tl=_tmp714,_tmpCE4))))))),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp70C,datatype_ctype,0,_tmp718,0));};}goto _LL208;};}
_LL24F: {struct Cyc_Absyn_Enum_e_struct*_tmp4F6=(struct Cyc_Absyn_Enum_e_struct*)
_tmp481;if(_tmp4F6->tag != 33)goto _LL251;}_LL250: goto _LL252;_LL251: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmp4F7=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmp481;if(_tmp4F7->tag != 34)goto
_LL253;}_LL252: goto _LL208;_LL253: {struct Cyc_Absyn_Malloc_e_struct*_tmp4F8=(
struct Cyc_Absyn_Malloc_e_struct*)_tmp481;if(_tmp4F8->tag != 35)goto _LL255;else{
_tmp4F9=_tmp4F8->f1;_tmp4FA=_tmp4F9.is_calloc;_tmp4FB=_tmp4F9.rgn;_tmp4FC=
_tmp4F9.elt_type;_tmp4FD=_tmp4F9.num_elts;_tmp4FE=_tmp4F9.fat_result;}}_LL254: {
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp4FC)));Cyc_Toc_exp_to_c(nv,
_tmp4FD);if(_tmp4FE){struct _tuple1*_tmp71B=Cyc_Toc_temp_var();struct _tuple1*
_tmp71C=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*rexp;if(_tmp4FA){xexp=_tmp4FD;if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp71B,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp4FC,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp71B,0));}{struct Cyc_Absyn_Exp*_tmpCE5[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCE5[2]=Cyc_Absyn_var_exp(_tmp71B,0),((_tmpCE5[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpCE5[0]=Cyc_Absyn_var_exp(_tmp71C,0),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpCE5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp4FD,0);if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp71B,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp4FC,Cyc_Absyn_var_exp(_tmp71B,0));}{struct Cyc_Absyn_Exp*_tmpCE6[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,((_tmpCE6[2]=Cyc_Absyn_var_exp(_tmp71B,0),((_tmpCE6[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpCE6[0]=Cyc_Absyn_var_exp(_tmp71C,0),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE6,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);};}{struct Cyc_Absyn_Stmt*_tmp71F=Cyc_Absyn_declare_stmt(_tmp71B,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp71C,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp71F);};}else{struct Cyc_Absyn_Exp*_tmp720=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp721=_tmp4FD->r;union Cyc_Absyn_Cnst _tmp723;struct _tuple6 _tmp724;
int _tmp725;_LL325: {struct Cyc_Absyn_Const_e_struct*_tmp722=(struct Cyc_Absyn_Const_e_struct*)
_tmp721;if(_tmp722->tag != 0)goto _LL327;else{_tmp723=_tmp722->f1;if((_tmp723.Int_c).tag
!= 4)goto _LL327;_tmp724=(struct _tuple6)(_tmp723.Int_c).val;_tmp725=_tmp724.f2;
if(_tmp725 != 1)goto _LL327;}}_LL326: goto _LL324;_LL327:;_LL328: _tmp720=Cyc_Absyn_times_exp(
_tmp720,_tmp4FD,0);goto _LL324;_LL324:;}if(_tmp4FB != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp4FB;Cyc_Toc_exp_to_c(nv,rgn);e->r=(
Cyc_Toc_rmalloc_exp(rgn,_tmp720))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp4FC,
_tmp720))->r;}}goto _LL208;}_LL255: {struct Cyc_Absyn_Swap_e_struct*_tmp4FF=(
struct Cyc_Absyn_Swap_e_struct*)_tmp481;if(_tmp4FF->tag != 36)goto _LL257;else{
_tmp500=_tmp4FF->f1;_tmp501=_tmp4FF->f2;}}_LL256: {int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp500->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp501->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dyneither_ptr)){const char*_tmpCE9;void*_tmpCE8;(_tmpCE8=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE9="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dyneither(_tmpCE9,sizeof(char),57))),_tag_dyneither(_tmpCE8,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}Cyc_Toc_exp_to_c(nv,_tmp500);Cyc_Toc_exp_to_c(
nv,_tmp501);{struct Cyc_Absyn_Exp*_tmpCEA[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((
_tmpCEA[1]=Cyc_Toc_push_address_exp(_tmp501),((_tmpCEA[0]=Cyc_Toc_push_address_exp(
_tmp500),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpCEA,sizeof(struct Cyc_Absyn_Exp*),2)))))));}goto _LL208;};}
_LL257: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp502=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp481;if(_tmp502->tag != 39)goto _LL259;else{_tmp503=_tmp502->f1;_tmp504=_tmp502->f2;}}
_LL258: {void*_tmp729=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp503->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp503);{void*_tmp72A=
_tmp729;struct Cyc_Absyn_AggrInfo _tmp72C;union Cyc_Absyn_AggrInfoU _tmp72D;struct
Cyc_Absyn_Aggrdecl**_tmp72E;struct Cyc_Absyn_Aggrdecl*_tmp72F;_LL32A: {struct Cyc_Absyn_AggrType_struct*
_tmp72B=(struct Cyc_Absyn_AggrType_struct*)_tmp72A;if(_tmp72B->tag != 12)goto
_LL32C;else{_tmp72C=_tmp72B->f1;_tmp72D=_tmp72C.aggr_info;if((_tmp72D.KnownAggr).tag
!= 2)goto _LL32C;_tmp72E=(struct Cyc_Absyn_Aggrdecl**)(_tmp72D.KnownAggr).val;
_tmp72F=*_tmp72E;}}_LL32B: {struct Cyc_Absyn_Exp*_tmp730=Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp72F,_tmp504),0);struct Cyc_Absyn_Exp*_tmp731=Cyc_Absyn_aggrmember_exp(
_tmp503,_tmp504,0);struct Cyc_Absyn_Exp*_tmp732=Cyc_Absyn_aggrmember_exp(_tmp731,
Cyc_Toc_tag_sp,0);e->r=(Cyc_Absyn_eq_exp(_tmp732,_tmp730,0))->r;goto _LL329;}
_LL32C:;_LL32D: {const char*_tmpCED;void*_tmpCEC;(_tmpCEC=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCED="non-aggregate type in tagcheck",
_tag_dyneither(_tmpCED,sizeof(char),31))),_tag_dyneither(_tmpCEC,sizeof(void*),0)));}
_LL329:;}goto _LL208;}_LL259: {struct Cyc_Absyn_StmtExp_e_struct*_tmp505=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp481;if(_tmp505->tag != 38)goto _LL25B;else{_tmp506=
_tmp505->f1;}}_LL25A: Cyc_Toc_stmt_to_c(nv,_tmp506);goto _LL208;_LL25B: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp507=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp481;if(_tmp507->tag != 37)
goto _LL25D;}_LL25C: {const char*_tmpCF0;void*_tmpCEF;(_tmpCEF=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF0="UnresolvedMem",
_tag_dyneither(_tmpCF0,sizeof(char),14))),_tag_dyneither(_tmpCEF,sizeof(void*),0)));}
_LL25D: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp508=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp481;if(_tmp508->tag != 27)goto _LL25F;}_LL25E: {const char*_tmpCF3;void*_tmpCF2;(
_tmpCF2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((
_tmpCF3="compoundlit",_tag_dyneither(_tmpCF3,sizeof(char),12))),_tag_dyneither(
_tmpCF2,sizeof(void*),0)));}_LL25F: {struct Cyc_Absyn_Valueof_e_struct*_tmp509=(
struct Cyc_Absyn_Valueof_e_struct*)_tmp481;if(_tmp509->tag != 40)goto _LL208;}
_LL260: {const char*_tmpCF6;void*_tmpCF5;(_tmpCF5=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF6="valueof(-)",
_tag_dyneither(_tmpCF6,sizeof(char),11))),_tag_dyneither(_tmpCF5,sizeof(void*),0)));}
_LL208:;};}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
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
struct Cyc_Absyn_Stmt*s;{void*_tmp73B=p->r;struct Cyc_Absyn_Vardecl*_tmp73D;struct
Cyc_Absyn_Vardecl _tmp73E;struct _tuple1*_tmp73F;struct Cyc_Absyn_Pat*_tmp740;
struct Cyc_Absyn_Vardecl*_tmp742;struct Cyc_Absyn_Vardecl _tmp743;struct _tuple1*
_tmp744;struct Cyc_Absyn_Vardecl*_tmp747;struct Cyc_Absyn_Pat*_tmp748;enum Cyc_Absyn_Sign
_tmp74B;int _tmp74C;char _tmp74E;struct _dyneither_ptr _tmp750;struct Cyc_Absyn_Enumdecl*
_tmp752;struct Cyc_Absyn_Enumfield*_tmp753;void*_tmp755;struct Cyc_Absyn_Enumfield*
_tmp756;struct Cyc_Absyn_Pat*_tmp758;struct Cyc_Absyn_Pat _tmp759;void*_tmp75A;
struct Cyc_Absyn_Datatypedecl*_tmp75C;struct Cyc_Absyn_Datatypefield*_tmp75D;
struct Cyc_List_List*_tmp75E;struct Cyc_List_List*_tmp760;struct Cyc_List_List*
_tmp762;struct Cyc_Absyn_AggrInfo*_tmp764;struct Cyc_Absyn_AggrInfo*_tmp766;struct
Cyc_Absyn_AggrInfo _tmp767;union Cyc_Absyn_AggrInfoU _tmp768;struct Cyc_List_List*
_tmp769;struct Cyc_Absyn_Pat*_tmp76B;_LL32F: {struct Cyc_Absyn_Var_p_struct*
_tmp73C=(struct Cyc_Absyn_Var_p_struct*)_tmp73B;if(_tmp73C->tag != 1)goto _LL331;
else{_tmp73D=_tmp73C->f1;_tmp73E=*_tmp73D;_tmp73F=_tmp73E.name;_tmp740=_tmp73C->f2;}}
_LL330: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp73F,r),rgn,t,r,path,
_tmp740,fail_stmt,decls);_LL331: {struct Cyc_Absyn_TagInt_p_struct*_tmp741=(
struct Cyc_Absyn_TagInt_p_struct*)_tmp73B;if(_tmp741->tag != 3)goto _LL333;else{
_tmp742=_tmp741->f2;_tmp743=*_tmp742;_tmp744=_tmp743.name;}}_LL332: nv=Cyc_Toc_add_varmap(
rgn,nv,_tmp744,r);goto _LL334;_LL333: {struct Cyc_Absyn_Wild_p_struct*_tmp745=(
struct Cyc_Absyn_Wild_p_struct*)_tmp73B;if(_tmp745->tag != 0)goto _LL335;}_LL334: s=
Cyc_Toc_skip_stmt_dl();goto _LL32E;_LL335: {struct Cyc_Absyn_Reference_p_struct*
_tmp746=(struct Cyc_Absyn_Reference_p_struct*)_tmp73B;if(_tmp746->tag != 2)goto
_LL337;else{_tmp747=_tmp746->f1;_tmp748=_tmp746->f2;}}_LL336: {struct _tuple1*
_tmp76F=Cyc_Toc_temp_var();{struct _tuple22*_tmpCF9;struct Cyc_List_List*_tmpCF8;
decls=((_tmpCF8=_region_malloc(rgn,sizeof(*_tmpCF8)),((_tmpCF8->hd=((_tmpCF9=
_region_malloc(rgn,sizeof(*_tmpCF9)),((_tmpCF9->f1=_tmp76F,((_tmpCF9->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpCF9)))))),((_tmpCF8->tl=decls,_tmpCF8))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp747->name,Cyc_Absyn_var_exp(_tmp76F,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp76F,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Toc_push_address_exp(path)),0);{struct _tuple21 _tmp772=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp748,fail_stmt,decls);_tmp772.f3=Cyc_Absyn_seq_stmt(s,_tmp772.f3,
0);return _tmp772;};}_LL337: {struct Cyc_Absyn_Null_p_struct*_tmp749=(struct Cyc_Absyn_Null_p_struct*)
_tmp73B;if(_tmp749->tag != 8)goto _LL339;}_LL338: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(
0,0),fail_stmt);goto _LL32E;_LL339: {struct Cyc_Absyn_Int_p_struct*_tmp74A=(struct
Cyc_Absyn_Int_p_struct*)_tmp73B;if(_tmp74A->tag != 9)goto _LL33B;else{_tmp74B=
_tmp74A->f1;_tmp74C=_tmp74A->f2;}}_LL33A: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(
_tmp74B,_tmp74C,0),fail_stmt);goto _LL32E;_LL33B: {struct Cyc_Absyn_Char_p_struct*
_tmp74D=(struct Cyc_Absyn_Char_p_struct*)_tmp73B;if(_tmp74D->tag != 10)goto _LL33D;
else{_tmp74E=_tmp74D->f1;}}_LL33C: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(
_tmp74E,0),fail_stmt);goto _LL32E;_LL33D: {struct Cyc_Absyn_Float_p_struct*_tmp74F=(
struct Cyc_Absyn_Float_p_struct*)_tmp73B;if(_tmp74F->tag != 11)goto _LL33F;else{
_tmp750=_tmp74F->f1;}}_LL33E: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp750,
0),fail_stmt);goto _LL32E;_LL33F: {struct Cyc_Absyn_Enum_p_struct*_tmp751=(struct
Cyc_Absyn_Enum_p_struct*)_tmp73B;if(_tmp751->tag != 12)goto _LL341;else{_tmp752=
_tmp751->f1;_tmp753=_tmp751->f2;}}_LL340:{struct Cyc_Absyn_Enum_e_struct _tmpCFC;
struct Cyc_Absyn_Enum_e_struct*_tmpCFB;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((_tmpCFB[0]=((_tmpCFC.tag=33,((
_tmpCFC.f1=_tmp753->name,((_tmpCFC.f2=(struct Cyc_Absyn_Enumdecl*)_tmp752,((
_tmpCFC.f3=(struct Cyc_Absyn_Enumfield*)_tmp753,_tmpCFC)))))))),_tmpCFB)))),0),
fail_stmt);}goto _LL32E;_LL341: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp754=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp73B;if(_tmp754->tag != 13)goto _LL343;else{
_tmp755=(void*)_tmp754->f1;_tmp756=_tmp754->f2;}}_LL342:{struct Cyc_Absyn_AnonEnum_e_struct
_tmpCFF;struct Cyc_Absyn_AnonEnum_e_struct*_tmpCFE;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE[0]=((_tmpCFF.tag=34,((
_tmpCFF.f1=_tmp756->name,((_tmpCFF.f2=(void*)_tmp755,((_tmpCFF.f3=(struct Cyc_Absyn_Enumfield*)
_tmp756,_tmpCFF)))))))),_tmpCFE)))),0),fail_stmt);}goto _LL32E;_LL343: {struct Cyc_Absyn_Pointer_p_struct*
_tmp757=(struct Cyc_Absyn_Pointer_p_struct*)_tmp73B;if(_tmp757->tag != 5)goto
_LL345;else{_tmp758=_tmp757->f1;_tmp759=*_tmp758;_tmp75A=_tmp759.r;{struct Cyc_Absyn_Datatype_p_struct*
_tmp75B=(struct Cyc_Absyn_Datatype_p_struct*)_tmp75A;if(_tmp75B->tag != 7)goto
_LL345;else{_tmp75C=_tmp75B->f1;_tmp75D=_tmp75B->f2;_tmp75E=_tmp75B->f3;}};}}
_LL344: s=Cyc_Toc_skip_stmt_dl();{struct _tuple1*_tmp777=Cyc_Toc_temp_var();const
char*_tmpD00;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp75D->name,((
_tmpD00="_struct",_tag_dyneither(_tmpD00,sizeof(char),8))));void*_tmp778=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);int cnt=1;struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
_tmp778,r);struct Cyc_List_List*_tmp779=_tmp75D->typs;for(0;_tmp75E != 0;(((
_tmp75E=_tmp75E->tl,_tmp779=((struct Cyc_List_List*)_check_null(_tmp779))->tl)),
++ cnt)){struct Cyc_Absyn_Pat*_tmp77A=(struct Cyc_Absyn_Pat*)_tmp75E->hd;if(_tmp77A->r
== (void*)& Cyc_Absyn_Wild_p_val)continue;{void*_tmp77B=(*((struct _tuple10*)((
struct Cyc_List_List*)_check_null(_tmp779))->hd)).f2;struct _tuple1*_tmp77C=Cyc_Toc_temp_var();
void*_tmp77D=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp77A->topt))->v;void*
_tmp77E=Cyc_Toc_typ_to_c(_tmp77D);struct Cyc_Absyn_Exp*_tmp77F=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp777,0),Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(
Cyc_Toc_typ_to_c(_tmp77B)))_tmp77F=Cyc_Toc_cast_it(_tmp77E,_tmp77F);{struct
_tuple22*_tmpD03;struct Cyc_List_List*_tmpD02;decls=((_tmpD02=_region_malloc(rgn,
sizeof(*_tmpD02)),((_tmpD02->hd=((_tmpD03=_region_malloc(rgn,sizeof(*_tmpD03)),((
_tmpD03->f1=_tmp77C,((_tmpD03->f2=_tmp77E,_tmpD03)))))),((_tmpD02->tl=decls,
_tmpD02))))));}{struct _tuple21 _tmp782=Cyc_Toc_xlate_pat(nv,rgn,_tmp77D,Cyc_Absyn_var_exp(
_tmp77C,0),_tmp77F,_tmp77A,fail_stmt,decls);nv=_tmp782.f1;decls=_tmp782.f2;{
struct Cyc_Absyn_Stmt*_tmp783=_tmp782.f3;struct Cyc_Absyn_Stmt*_tmp784=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp77C,0),_tmp77F,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp784,_tmp783,0),0);};};};}{struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
Cyc_Absyn_var_exp(_tmp777,0),Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*tag_exp=
_tmp75C->is_extensible?Cyc_Absyn_var_exp(_tmp75D->name,0): Cyc_Toc_datatype_tag(
_tmp75C,_tmp75D->name);struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,
tag_exp,0);s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);if(Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v))s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp777,0),Cyc_Absyn_uint_exp(0,0),0),
fail_stmt,s,0);s=Cyc_Absyn_declare_stmt(_tmp777,_tmp778,(struct Cyc_Absyn_Exp*)
rcast,s,0);goto _LL32E;};};_LL345: {struct Cyc_Absyn_Datatype_p_struct*_tmp75F=(
struct Cyc_Absyn_Datatype_p_struct*)_tmp73B;if(_tmp75F->tag != 7)goto _LL347;else{
_tmp760=_tmp75F->f3;}}_LL346: _tmp762=_tmp760;goto _LL348;_LL347: {struct Cyc_Absyn_Tuple_p_struct*
_tmp761=(struct Cyc_Absyn_Tuple_p_struct*)_tmp73B;if(_tmp761->tag != 4)goto _LL349;
else{_tmp762=_tmp761->f1;}}_LL348: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;
_tmp762 != 0;(_tmp762=_tmp762->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp786=(struct Cyc_Absyn_Pat*)
_tmp762->hd;if(_tmp786->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{struct _tuple1*
_tmp787=Cyc_Toc_temp_var();void*_tmp788=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp786->topt))->v;{struct _tuple22*_tmpD06;struct Cyc_List_List*_tmpD05;decls=((
_tmpD05=_region_malloc(rgn,sizeof(*_tmpD05)),((_tmpD05->hd=((_tmpD06=
_region_malloc(rgn,sizeof(*_tmpD06)),((_tmpD06->f1=_tmp787,((_tmpD06->f2=Cyc_Toc_typ_to_c(
_tmp788),_tmpD06)))))),((_tmpD05->tl=decls,_tmpD05))))));}{struct _tuple21 _tmp78B=
Cyc_Toc_xlate_pat(nv,rgn,_tmp788,Cyc_Absyn_var_exp(_tmp787,0),Cyc_Absyn_aggrmember_exp(
path,Cyc_Absyn_fieldname(cnt),0),_tmp786,fail_stmt,decls);nv=_tmp78B.f1;decls=
_tmp78B.f2;{struct Cyc_Absyn_Stmt*_tmp78C=_tmp78B.f3;struct Cyc_Absyn_Stmt*_tmp78D=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp787,0),Cyc_Absyn_aggrmember_exp(r,
Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp78D,
_tmp78C,0),0);};};};}goto _LL32E;};_LL349: {struct Cyc_Absyn_Aggr_p_struct*_tmp763=(
struct Cyc_Absyn_Aggr_p_struct*)_tmp73B;if(_tmp763->tag != 6)goto _LL34B;else{
_tmp764=_tmp763->f1;if(_tmp764 != 0)goto _LL34B;}}_LL34A: {const char*_tmpD09;void*
_tmpD08;(_tmpD08=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD09="unresolved aggregate pattern!",_tag_dyneither(_tmpD09,sizeof(char),30))),
_tag_dyneither(_tmpD08,sizeof(void*),0)));}_LL34B: {struct Cyc_Absyn_Aggr_p_struct*
_tmp765=(struct Cyc_Absyn_Aggr_p_struct*)_tmp73B;if(_tmp765->tag != 6)goto _LL34D;
else{_tmp766=_tmp765->f1;if(_tmp766 == 0)goto _LL34D;_tmp767=*_tmp766;_tmp768=
_tmp767.aggr_info;_tmp769=_tmp765->f3;}}_LL34C: {struct Cyc_Absyn_Aggrdecl*
_tmp790=Cyc_Absyn_get_known_aggrdecl(_tmp768);if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp790->impl))->tagged){struct _tuple23 _tmp792;struct Cyc_List_List*
_tmp793;struct Cyc_Absyn_Pat*_tmp794;struct _tuple23*_tmp791=(struct _tuple23*)((
struct Cyc_List_List*)_check_null(_tmp769))->hd;_tmp792=*_tmp791;_tmp793=_tmp792.f1;
_tmp794=_tmp792.f2;{struct _dyneither_ptr*f;{void*_tmp795=(void*)((struct Cyc_List_List*)
_check_null(_tmp793))->hd;struct _dyneither_ptr*_tmp797;_LL356: {struct Cyc_Absyn_FieldName_struct*
_tmp796=(struct Cyc_Absyn_FieldName_struct*)_tmp795;if(_tmp796->tag != 1)goto
_LL358;else{_tmp797=_tmp796->f1;}}_LL357: f=_tmp797;goto _LL355;_LL358:;_LL359: {
const char*_tmpD0C;void*_tmpD0B;(_tmpD0B=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0C="no field name in tagged union pattern",
_tag_dyneither(_tmpD0C,sizeof(char),38))),_tag_dyneither(_tmpD0B,sizeof(void*),0)));}
_LL355:;}{struct _tuple1*_tmp79A=Cyc_Toc_temp_var();void*_tmp79B=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp794->topt))->v;void*_tmp79C=Cyc_Toc_typ_to_c(
_tmp79B);{struct _tuple22*_tmpD0F;struct Cyc_List_List*_tmpD0E;decls=((_tmpD0E=
_region_malloc(rgn,sizeof(*_tmpD0E)),((_tmpD0E->hd=((_tmpD0F=_region_malloc(rgn,
sizeof(*_tmpD0F)),((_tmpD0F->f1=_tmp79A,((_tmpD0F->f2=_tmp79C,_tmpD0F)))))),((
_tmpD0E->tl=decls,_tmpD0E))))));}{struct Cyc_Absyn_Exp*_tmp79F=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp7A0=
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7A0=Cyc_Toc_cast_it(_tmp79C,_tmp7A0);_tmp79F=Cyc_Toc_cast_it(_tmp79C,_tmp79F);{
struct _tuple21 _tmp7A1=Cyc_Toc_xlate_pat(nv,rgn,_tmp79B,Cyc_Absyn_var_exp(_tmp79A,
0),_tmp79F,_tmp794,fail_stmt,decls);nv=_tmp7A1.f1;decls=_tmp7A1.f2;{struct Cyc_Absyn_Stmt*
_tmp7A2=_tmp7A1.f3;struct Cyc_Absyn_Stmt*_tmp7A3=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp790,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp7A4=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp79A,0),_tmp7A0,0);s=Cyc_Absyn_seq_stmt(_tmp7A3,Cyc_Absyn_seq_stmt(_tmp7A4,
_tmp7A2,0),0);};};};};};}else{s=Cyc_Toc_skip_stmt_dl();for(0;_tmp769 != 0;_tmp769=
_tmp769->tl){struct _tuple23*_tmp7A5=(struct _tuple23*)_tmp769->hd;struct Cyc_Absyn_Pat*
_tmp7A6=(*_tmp7A5).f2;if(_tmp7A6->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _dyneither_ptr*f;{void*_tmp7A7=(void*)((struct Cyc_List_List*)_check_null((*
_tmp7A5).f1))->hd;struct _dyneither_ptr*_tmp7A9;_LL35B: {struct Cyc_Absyn_FieldName_struct*
_tmp7A8=(struct Cyc_Absyn_FieldName_struct*)_tmp7A7;if(_tmp7A8->tag != 1)goto
_LL35D;else{_tmp7A9=_tmp7A8->f1;}}_LL35C: f=_tmp7A9;goto _LL35A;_LL35D:;_LL35E: {
struct Cyc_Toc_Match_error_struct _tmpD12;struct Cyc_Toc_Match_error_struct*_tmpD11;(
int)_throw((void*)((_tmpD11=_cycalloc_atomic(sizeof(*_tmpD11)),((_tmpD11[0]=((
_tmpD12.tag=Cyc_Toc_Match_error,_tmpD12)),_tmpD11)))));}_LL35A:;}{struct _tuple1*
_tmp7AC=Cyc_Toc_temp_var();void*_tmp7AD=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7A6->topt))->v;void*_tmp7AE=Cyc_Toc_typ_to_c(_tmp7AD);{struct _tuple22*
_tmpD15;struct Cyc_List_List*_tmpD14;decls=((_tmpD14=_region_malloc(rgn,sizeof(*
_tmpD14)),((_tmpD14->hd=((_tmpD15=_region_malloc(rgn,sizeof(*_tmpD15)),((_tmpD15->f1=
_tmp7AC,((_tmpD15->f2=_tmp7AE,_tmpD15)))))),((_tmpD14->tl=decls,_tmpD14))))));}{
struct _tuple21 _tmp7B1=Cyc_Toc_xlate_pat(nv,rgn,_tmp7AD,Cyc_Absyn_var_exp(_tmp7AC,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp7A6,fail_stmt,decls);nv=_tmp7B1.f1;
decls=_tmp7B1.f2;{struct Cyc_Absyn_Exp*_tmp7B2=Cyc_Absyn_aggrmember_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp790->impl))->fields,f)))->type))
_tmp7B2=Cyc_Toc_cast_it(_tmp7AE,_tmp7B2);{struct Cyc_Absyn_Stmt*_tmp7B3=_tmp7B1.f3;
struct Cyc_Absyn_Stmt*_tmp7B4=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7AC,0),
_tmp7B2,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp7B4,_tmp7B3,0),0);};};};};};}}
goto _LL32E;}_LL34D: {struct Cyc_Absyn_Pointer_p_struct*_tmp76A=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp73B;if(_tmp76A->tag != 5)goto _LL34F;else{_tmp76B=_tmp76A->f1;}}_LL34E: {
struct _tuple1*_tmp7B5=Cyc_Toc_temp_var();void*_tmp7B6=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp76B->topt))->v;{struct _tuple22*_tmpD18;struct Cyc_List_List*
_tmpD17;decls=((_tmpD17=_region_malloc(rgn,sizeof(*_tmpD17)),((_tmpD17->hd=((
_tmpD18=_region_malloc(rgn,sizeof(*_tmpD18)),((_tmpD18->f1=_tmp7B5,((_tmpD18->f2=
Cyc_Toc_typ_to_c(_tmp7B6),_tmpD18)))))),((_tmpD17->tl=decls,_tmpD17))))));}{
struct _tuple21 _tmp7B9=Cyc_Toc_xlate_pat(nv,rgn,_tmp7B6,Cyc_Absyn_var_exp(_tmp7B5,
0),Cyc_Absyn_deref_exp(path,0),_tmp76B,fail_stmt,decls);nv=_tmp7B9.f1;decls=
_tmp7B9.f2;{struct Cyc_Absyn_Stmt*_tmp7BA=_tmp7B9.f3;struct Cyc_Absyn_Stmt*_tmp7BB=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B5,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7BA,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp7BB,0);else{s=_tmp7BB;}goto _LL32E;};};}_LL34F: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp76C=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp73B;if(_tmp76C->tag != 14)goto
_LL351;}_LL350: {const char*_tmpD1B;void*_tmpD1A;(_tmpD1A=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1B="unknownid pat",
_tag_dyneither(_tmpD1B,sizeof(char),14))),_tag_dyneither(_tmpD1A,sizeof(void*),0)));}
_LL351: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp76D=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp73B;if(_tmp76D->tag != 15)goto _LL353;}_LL352: {const char*_tmpD1E;void*_tmpD1D;(
_tmpD1D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD1E="unknowncall pat",_tag_dyneither(_tmpD1E,sizeof(char),16))),
_tag_dyneither(_tmpD1D,sizeof(void*),0)));}_LL353: {struct Cyc_Absyn_Exp_p_struct*
_tmp76E=(struct Cyc_Absyn_Exp_p_struct*)_tmp73B;if(_tmp76E->tag != 16)goto _LL32E;}
_LL354: {const char*_tmpD21;void*_tmpD20;(_tmpD20=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD21="exp pat",
_tag_dyneither(_tmpD21,sizeof(char),8))),_tag_dyneither(_tmpD20,sizeof(void*),0)));}
_LL32E:;}{struct _tuple21 _tmpD22;return(_tmpD22.f1=nv,((_tmpD22.f2=decls,((
_tmpD22.f3=s,_tmpD22)))));};}struct _tuple24{struct _dyneither_ptr*f1;struct
_dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc);static struct _tuple24*Cyc_Toc_gen_label(
struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){struct _tuple24*_tmpD23;
return(_tmpD23=_region_malloc(r,sizeof(*_tmpD23)),((_tmpD23->f1=Cyc_Toc_fresh_label(),((
_tmpD23->f2=Cyc_Toc_fresh_label(),((_tmpD23->f3=sc,_tmpD23)))))));}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs);static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7C4=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp7C5=Cyc_Tcutil_compress(_tmp7C4);_LL360: {struct Cyc_Absyn_IntType_struct*
_tmp7C6=(struct Cyc_Absyn_IntType_struct*)_tmp7C5;if(_tmp7C6->tag != 6)goto _LL362;}
_LL361: goto _LL363;_LL362: {struct Cyc_Absyn_EnumType_struct*_tmp7C7=(struct Cyc_Absyn_EnumType_struct*)
_tmp7C5;if(_tmp7C7->tag != 14)goto _LL364;}_LL363: leave_as_switch=1;goto _LL35F;
_LL364:;_LL365: leave_as_switch=0;goto _LL35F;_LL35F:;}{struct Cyc_List_List*
_tmp7C8=scs;for(0;_tmp7C8 != 0;_tmp7C8=_tmp7C8->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7C8->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7C8->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp7C9=scs;for(0;_tmp7C9 != 0;_tmp7C9=_tmp7C9->tl){struct Cyc_Absyn_Stmt*
_tmp7CA=((struct Cyc_Absyn_Switch_clause*)_tmp7C9->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp7C9->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7CA,0);next_l=Cyc_Toc_fresh_label();{
struct Cyc_Toc_Env _tmp7CC;struct _RegionHandle*_tmp7CD;struct Cyc_Toc_Env*_tmp7CB=
nv;_tmp7CC=*_tmp7CB;_tmp7CD=_tmp7CC.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
_tmp7CD,nv,next_l),_tmp7CA);};}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp7CF;
struct _RegionHandle*_tmp7D0;struct Cyc_Toc_Env*_tmp7CE=nv;_tmp7CF=*_tmp7CE;
_tmp7D0=_tmp7CF.rgn;{struct Cyc_Toc_Env*_tmp7D1=Cyc_Toc_share_env(_tmp7D0,nv);
struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp7D0,Cyc_Toc_gen_label,
_tmp7D0,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct
Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp7D2=lscs;for(0;_tmp7D2 != 0;
_tmp7D2=_tmp7D2->tl){struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)
_tmp7D2->hd)).f3;struct _dyneither_ptr*fail_lab=_tmp7D2->tl == 0?end_l:(*((struct
_tuple24*)((struct Cyc_List_List*)_check_null(_tmp7D2->tl))->hd)).f1;struct Cyc_Toc_Env*
_tmp7D4;struct Cyc_List_List*_tmp7D5;struct Cyc_Absyn_Stmt*_tmp7D6;struct _tuple21
_tmp7D3=Cyc_Toc_xlate_pat(_tmp7D1,_tmp7D0,_tmp7C4,r,path,sc->pattern,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp7D4=_tmp7D3.f1;_tmp7D5=_tmp7D3.f2;_tmp7D6=_tmp7D3.f3;if(
sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp7D7=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp7D4,_tmp7D7);_tmp7D6=Cyc_Absyn_seq_stmt(
_tmp7D6,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp7D7,0),
Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp7D5;{
struct Cyc_List_List*_tmpD24;nvs=((_tmpD24=_region_malloc(_tmp7D0,sizeof(*_tmpD24)),((
_tmpD24->hd=_tmp7D4,((_tmpD24->tl=nvs,_tmpD24))))));}{struct Cyc_List_List*
_tmpD25;test_stmts=((_tmpD25=_region_malloc(_tmp7D0,sizeof(*_tmpD25)),((_tmpD25->hd=
_tmp7D6,((_tmpD25->tl=test_stmts,_tmpD25))))));};}}nvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{struct Cyc_List_List*stmts=
0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple24 _tmp7DB;struct _dyneither_ptr*_tmp7DC;struct _dyneither_ptr*_tmp7DD;
struct Cyc_Absyn_Switch_clause*_tmp7DE;struct _tuple24*_tmp7DA=(struct _tuple24*)
lscs->hd;_tmp7DB=*_tmp7DA;_tmp7DC=_tmp7DB.f1;_tmp7DD=_tmp7DB.f2;_tmp7DE=_tmp7DB.f3;{
struct Cyc_Toc_Env*_tmp7DF=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp7DE->body;struct Cyc_Toc_Env _tmp7E1;struct
_RegionHandle*_tmp7E2;struct Cyc_Toc_Env*_tmp7E0=_tmp7D1;_tmp7E1=*_tmp7E0;_tmp7E2=
_tmp7E1.rgn;if(lscs->tl != 0){struct _tuple24 _tmp7E4;struct _dyneither_ptr*_tmp7E5;
struct Cyc_Absyn_Switch_clause*_tmp7E6;struct _tuple24*_tmp7E3=(struct _tuple24*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp7E4=*_tmp7E3;_tmp7E5=_tmp7E4.f2;
_tmp7E6=_tmp7E4.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(_tmp7E2,
_tmp7DF,end_l,_tmp7E5,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp7E6->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(
nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(_tmp7E2,
_tmp7DF,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp7DC,(struct Cyc_Absyn_Stmt*)((
struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp7DD,s,0),0);{struct Cyc_List_List*_tmpD26;stmts=((_tmpD26=_region_malloc(
_tmp7D0,sizeof(*_tmpD26)),((_tmpD26->hd=s,((_tmpD26->tl=stmts,_tmpD26))))));};};}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22
_tmp7E9;struct _tuple1*_tmp7EA;void*_tmp7EB;struct _tuple22*_tmp7E8=(struct
_tuple22*)decls->hd;_tmp7E9=*_tmp7E8;_tmp7EA=_tmp7E9.f1;_tmp7EB=_tmp7E9.f2;res=
Cyc_Absyn_declare_stmt(_tmp7EA,_tmp7EB,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(
v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),(
struct Cyc_Absyn_Exp*)e,res,0))->r;};};};};};}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){struct Cyc_List_List*_tmpD27;return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((
_tmpD27->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD27->tl=0,_tmpD27)))))),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s){while(1){void*_tmp7ED=s->r;struct Cyc_Absyn_Exp*_tmp7F0;struct
Cyc_Absyn_Stmt*_tmp7F2;struct Cyc_Absyn_Stmt*_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F5;
struct Cyc_Absyn_Exp*_tmp7F7;struct Cyc_Absyn_Stmt*_tmp7F8;struct Cyc_Absyn_Stmt*
_tmp7F9;struct _tuple9 _tmp7FB;struct Cyc_Absyn_Exp*_tmp7FC;struct Cyc_Absyn_Stmt*
_tmp7FD;struct Cyc_Absyn_Stmt*_tmp7FF;struct Cyc_Absyn_Stmt*_tmp801;struct Cyc_Absyn_Stmt*
_tmp803;struct Cyc_Absyn_Exp*_tmp805;struct _tuple9 _tmp806;struct Cyc_Absyn_Exp*
_tmp807;struct _tuple9 _tmp808;struct Cyc_Absyn_Exp*_tmp809;struct Cyc_Absyn_Stmt*
_tmp80A;struct Cyc_Absyn_Exp*_tmp80C;struct Cyc_List_List*_tmp80D;struct Cyc_List_List*
_tmp80F;struct Cyc_Absyn_Switch_clause**_tmp810;struct Cyc_Absyn_Decl*_tmp812;
struct Cyc_Absyn_Stmt*_tmp813;struct _dyneither_ptr*_tmp815;struct Cyc_Absyn_Stmt*
_tmp816;struct Cyc_Absyn_Stmt*_tmp818;struct _tuple9 _tmp819;struct Cyc_Absyn_Exp*
_tmp81A;struct Cyc_Absyn_Stmt*_tmp81C;struct Cyc_List_List*_tmp81D;struct Cyc_Absyn_Exp*
_tmp81F;_LL367: {struct Cyc_Absyn_Skip_s_struct*_tmp7EE=(struct Cyc_Absyn_Skip_s_struct*)
_tmp7ED;if(_tmp7EE->tag != 0)goto _LL369;}_LL368: return;_LL369: {struct Cyc_Absyn_Exp_s_struct*
_tmp7EF=(struct Cyc_Absyn_Exp_s_struct*)_tmp7ED;if(_tmp7EF->tag != 1)goto _LL36B;
else{_tmp7F0=_tmp7EF->f1;}}_LL36A: Cyc_Toc_exp_to_c(nv,_tmp7F0);return;_LL36B: {
struct Cyc_Absyn_Seq_s_struct*_tmp7F1=(struct Cyc_Absyn_Seq_s_struct*)_tmp7ED;if(
_tmp7F1->tag != 2)goto _LL36D;else{_tmp7F2=_tmp7F1->f1;_tmp7F3=_tmp7F1->f2;}}
_LL36C: Cyc_Toc_stmt_to_c(nv,_tmp7F2);s=_tmp7F3;continue;_LL36D: {struct Cyc_Absyn_Return_s_struct*
_tmp7F4=(struct Cyc_Absyn_Return_s_struct*)_tmp7ED;if(_tmp7F4->tag != 3)goto _LL36F;
else{_tmp7F5=_tmp7F4->f1;}}_LL36E: {struct Cyc_Core_Opt*topt=0;if(_tmp7F5 != 0){{
struct Cyc_Core_Opt*_tmpD28;topt=((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28->v=
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7F5->topt))->v),
_tmpD28))));}Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp7F5);}if(s->try_depth
> 0){if(topt != 0){struct _tuple1*_tmp821=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*
_tmp822=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp821,0),
0);s->r=(Cyc_Absyn_declare_stmt(_tmp821,(void*)topt->v,_tmp7F5,Cyc_Absyn_seq_stmt(
Cyc_Toc_make_npop_handler(s->try_depth),_tmp822,0),0))->r;}else{Cyc_Toc_do_npop_before(
s->try_depth,s);}}return;}_LL36F: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp7F6=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7ED;if(_tmp7F6->tag != 4)goto _LL371;
else{_tmp7F7=_tmp7F6->f1;_tmp7F8=_tmp7F6->f2;_tmp7F9=_tmp7F6->f3;}}_LL370: Cyc_Toc_exp_to_c(
nv,_tmp7F7);Cyc_Toc_stmt_to_c(nv,_tmp7F8);s=_tmp7F9;continue;_LL371: {struct Cyc_Absyn_While_s_struct*
_tmp7FA=(struct Cyc_Absyn_While_s_struct*)_tmp7ED;if(_tmp7FA->tag != 5)goto _LL373;
else{_tmp7FB=_tmp7FA->f1;_tmp7FC=_tmp7FB.f1;_tmp7FD=_tmp7FA->f2;}}_LL372: Cyc_Toc_exp_to_c(
nv,_tmp7FC);{struct Cyc_Toc_Env _tmp824;struct _RegionHandle*_tmp825;struct Cyc_Toc_Env*
_tmp823=nv;_tmp824=*_tmp823;_tmp825=_tmp824.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp825,nv),_tmp7FD);return;};_LL373: {struct Cyc_Absyn_Break_s_struct*_tmp7FE=(
struct Cyc_Absyn_Break_s_struct*)_tmp7ED;if(_tmp7FE->tag != 6)goto _LL375;else{
_tmp7FF=_tmp7FE->f1;}}_LL374: {struct Cyc_Toc_Env _tmp827;struct _dyneither_ptr**
_tmp828;struct Cyc_Toc_Env*_tmp826=nv;_tmp827=*_tmp826;_tmp828=_tmp827.break_lab;
if(_tmp828 != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp828,0);{int dest_depth=_tmp7FF == 0?0:
_tmp7FF->try_depth;Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;};}
_LL375: {struct Cyc_Absyn_Continue_s_struct*_tmp800=(struct Cyc_Absyn_Continue_s_struct*)
_tmp7ED;if(_tmp800->tag != 7)goto _LL377;else{_tmp801=_tmp800->f1;}}_LL376: {
struct Cyc_Toc_Env _tmp82A;struct _dyneither_ptr**_tmp82B;struct Cyc_Toc_Env*_tmp829=
nv;_tmp82A=*_tmp829;_tmp82B=_tmp82A.continue_lab;if(_tmp82B != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp82B,0);_tmp803=_tmp801;goto _LL378;}_LL377: {struct Cyc_Absyn_Goto_s_struct*
_tmp802=(struct Cyc_Absyn_Goto_s_struct*)_tmp7ED;if(_tmp802->tag != 8)goto _LL379;
else{_tmp803=_tmp802->f2;}}_LL378: Cyc_Toc_do_npop_before(s->try_depth - ((struct
Cyc_Absyn_Stmt*)_check_null(_tmp803))->try_depth,s);return;_LL379: {struct Cyc_Absyn_For_s_struct*
_tmp804=(struct Cyc_Absyn_For_s_struct*)_tmp7ED;if(_tmp804->tag != 9)goto _LL37B;
else{_tmp805=_tmp804->f1;_tmp806=_tmp804->f2;_tmp807=_tmp806.f1;_tmp808=_tmp804->f3;
_tmp809=_tmp808.f1;_tmp80A=_tmp804->f4;}}_LL37A: Cyc_Toc_exp_to_c(nv,_tmp805);Cyc_Toc_exp_to_c(
nv,_tmp807);Cyc_Toc_exp_to_c(nv,_tmp809);{struct Cyc_Toc_Env _tmp82D;struct
_RegionHandle*_tmp82E;struct Cyc_Toc_Env*_tmp82C=nv;_tmp82D=*_tmp82C;_tmp82E=
_tmp82D.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp82E,nv),_tmp80A);return;};
_LL37B: {struct Cyc_Absyn_Switch_s_struct*_tmp80B=(struct Cyc_Absyn_Switch_s_struct*)
_tmp7ED;if(_tmp80B->tag != 10)goto _LL37D;else{_tmp80C=_tmp80B->f1;_tmp80D=_tmp80B->f2;}}
_LL37C: Cyc_Toc_xlate_switch(nv,s,_tmp80C,_tmp80D);return;_LL37D: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp80E=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp7ED;if(_tmp80E->tag != 11)goto
_LL37F;else{_tmp80F=_tmp80E->f1;_tmp810=_tmp80E->f2;}}_LL37E: {struct Cyc_Toc_Env
_tmp830;struct Cyc_Toc_FallthruInfo*_tmp831;struct Cyc_Toc_Env*_tmp82F=nv;_tmp830=*
_tmp82F;_tmp831=_tmp830.fallthru_info;if(_tmp831 == 0){const char*_tmpD2B;void*
_tmpD2A;(_tmpD2A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpD2B="fallthru in unexpected place",_tag_dyneither(_tmpD2B,sizeof(char),29))),
_tag_dyneither(_tmpD2A,sizeof(void*),0)));}{struct _dyneither_ptr*_tmp835;struct
Cyc_List_List*_tmp836;struct Cyc_Dict_Dict _tmp837;struct Cyc_Toc_FallthruInfo
_tmp834=*_tmp831;_tmp835=_tmp834.label;_tmp836=_tmp834.binders;_tmp837=_tmp834.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp835,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp810)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp838=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp836);struct Cyc_List_List*_tmp839=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp80F);for(0;_tmp838 != 0;(_tmp838=_tmp838->tl,
_tmp839=_tmp839->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp839))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp837,(
struct _tuple1*)_tmp838->hd),(struct Cyc_Absyn_Exp*)_tmp839->hd,0),s2,0);}s->r=s2->r;
return;};};};}_LL37F: {struct Cyc_Absyn_Decl_s_struct*_tmp811=(struct Cyc_Absyn_Decl_s_struct*)
_tmp7ED;if(_tmp811->tag != 12)goto _LL381;else{_tmp812=_tmp811->f1;_tmp813=_tmp811->f2;}}
_LL380:{void*_tmp83A=_tmp812->r;struct Cyc_Absyn_Vardecl*_tmp83C;struct Cyc_Absyn_Pat*
_tmp83E;struct Cyc_Absyn_Exp*_tmp83F;struct Cyc_List_List*_tmp841;struct Cyc_Absyn_Fndecl*
_tmp843;struct Cyc_Absyn_Tvar*_tmp845;struct Cyc_Absyn_Vardecl*_tmp846;int _tmp847;
struct Cyc_Absyn_Exp*_tmp848;struct Cyc_Absyn_Exp*_tmp84A;struct Cyc_Absyn_Tvar*
_tmp84B;struct Cyc_Absyn_Vardecl*_tmp84C;_LL38A: {struct Cyc_Absyn_Var_d_struct*
_tmp83B=(struct Cyc_Absyn_Var_d_struct*)_tmp83A;if(_tmp83B->tag != 0)goto _LL38C;
else{_tmp83C=_tmp83B->f1;}}_LL38B: {struct Cyc_Toc_Env _tmp84E;struct _RegionHandle*
_tmp84F;struct Cyc_Toc_Env*_tmp84D=nv;_tmp84E=*_tmp84D;_tmp84F=_tmp84E.rgn;{
struct Cyc_Absyn_Local_b_struct _tmpD2E;struct Cyc_Absyn_Local_b_struct*_tmpD2D;
struct Cyc_Toc_Env*_tmp850=Cyc_Toc_add_varmap(_tmp84F,nv,_tmp83C->name,Cyc_Absyn_varb_exp(
_tmp83C->name,(void*)((_tmpD2D=_cycalloc(sizeof(*_tmpD2D)),((_tmpD2D[0]=((
_tmpD2E.tag=4,((_tmpD2E.f1=_tmp83C,_tmpD2E)))),_tmpD2D)))),0));Cyc_Toc_local_decl_to_c(
_tmp850,_tmp850,_tmp83C,_tmp813);}goto _LL389;}_LL38C: {struct Cyc_Absyn_Let_d_struct*
_tmp83D=(struct Cyc_Absyn_Let_d_struct*)_tmp83A;if(_tmp83D->tag != 2)goto _LL38E;
else{_tmp83E=_tmp83D->f1;_tmp83F=_tmp83D->f3;}}_LL38D:{void*_tmp853=_tmp83E->r;
struct Cyc_Absyn_Vardecl*_tmp855;struct Cyc_Absyn_Pat*_tmp856;struct Cyc_Absyn_Pat
_tmp857;void*_tmp858;_LL399: {struct Cyc_Absyn_Var_p_struct*_tmp854=(struct Cyc_Absyn_Var_p_struct*)
_tmp853;if(_tmp854->tag != 1)goto _LL39B;else{_tmp855=_tmp854->f1;_tmp856=_tmp854->f2;
_tmp857=*_tmp856;_tmp858=_tmp857.r;{struct Cyc_Absyn_Wild_p_struct*_tmp859=(
struct Cyc_Absyn_Wild_p_struct*)_tmp858;if(_tmp859->tag != 0)goto _LL39B;};}}_LL39A: {
struct _tuple1*old_name=_tmp855->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp855->name=new_name;_tmp855->initializer=(struct Cyc_Absyn_Exp*)_tmp83F;{
struct Cyc_Absyn_Var_d_struct _tmpD31;struct Cyc_Absyn_Var_d_struct*_tmpD30;_tmp812->r=(
void*)((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30[0]=((_tmpD31.tag=0,((
_tmpD31.f1=_tmp855,_tmpD31)))),_tmpD30))));}{struct Cyc_Toc_Env _tmp85D;struct
_RegionHandle*_tmp85E;struct Cyc_Toc_Env*_tmp85C=nv;_tmp85D=*_tmp85C;_tmp85E=
_tmp85D.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD34;struct Cyc_Absyn_Local_b_struct*
_tmpD33;struct Cyc_Toc_Env*_tmp85F=Cyc_Toc_add_varmap(_tmp85E,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33[0]=((_tmpD34.tag=
4,((_tmpD34.f1=_tmp855,_tmpD34)))),_tmpD33)))),0));Cyc_Toc_local_decl_to_c(
_tmp85F,nv,_tmp855,_tmp813);}goto _LL398;};}_LL39B:;_LL39C: s->r=(Cyc_Toc_letdecl_to_c(
nv,_tmp83E,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp83F->topt))->v,_tmp83F,
_tmp813))->r;goto _LL398;_LL398:;}goto _LL389;_LL38E: {struct Cyc_Absyn_Letv_d_struct*
_tmp840=(struct Cyc_Absyn_Letv_d_struct*)_tmp83A;if(_tmp840->tag != 3)goto _LL390;
else{_tmp841=_tmp840->f1;}}_LL38F: {struct Cyc_List_List*_tmp862=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp841);if(_tmp862 == 0){const char*_tmpD37;
void*_tmpD36;(_tmpD36=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmpD37="empty Letv_d",_tag_dyneither(_tmpD37,sizeof(char),
13))),_tag_dyneither(_tmpD36,sizeof(void*),0)));}{struct Cyc_Absyn_Var_d_struct
_tmpD3A;struct Cyc_Absyn_Var_d_struct*_tmpD39;_tmp812->r=(void*)((_tmpD39=
_cycalloc(sizeof(*_tmpD39)),((_tmpD39[0]=((_tmpD3A.tag=0,((_tmpD3A.f1=(struct Cyc_Absyn_Vardecl*)
_tmp862->hd,_tmpD3A)))),_tmpD39))));}_tmp862=_tmp862->tl;for(0;_tmp862 != 0;
_tmp862=_tmp862->tl){struct Cyc_Absyn_Var_d_struct _tmpD3D;struct Cyc_Absyn_Var_d_struct*
_tmpD3C;struct Cyc_Absyn_Decl*_tmp867=Cyc_Absyn_new_decl((void*)((_tmpD3C=
_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C[0]=((_tmpD3D.tag=0,((_tmpD3D.f1=(struct Cyc_Absyn_Vardecl*)
_tmp862->hd,_tmpD3D)))),_tmpD3C)))),0);s->r=(Cyc_Absyn_decl_stmt(_tmp867,Cyc_Absyn_new_stmt(
s->r,0),0))->r;}Cyc_Toc_stmt_to_c(nv,s);goto _LL389;}_LL390: {struct Cyc_Absyn_Fn_d_struct*
_tmp842=(struct Cyc_Absyn_Fn_d_struct*)_tmp83A;if(_tmp842->tag != 1)goto _LL392;
else{_tmp843=_tmp842->f1;}}_LL391: {struct _tuple1*_tmp86A=_tmp843->name;struct
Cyc_Toc_Env _tmp86C;struct _RegionHandle*_tmp86D;struct Cyc_Toc_Env*_tmp86B=nv;
_tmp86C=*_tmp86B;_tmp86D=_tmp86C.rgn;{struct Cyc_Toc_Env*_tmp86E=Cyc_Toc_add_varmap(
_tmp86D,nv,_tmp843->name,Cyc_Absyn_var_exp(_tmp86A,0));Cyc_Toc_fndecl_to_c(
_tmp86E,_tmp843,0);Cyc_Toc_stmt_to_c(_tmp86E,_tmp813);}goto _LL389;}_LL392: {
struct Cyc_Absyn_Region_d_struct*_tmp844=(struct Cyc_Absyn_Region_d_struct*)
_tmp83A;if(_tmp844->tag != 4)goto _LL394;else{_tmp845=_tmp844->f1;_tmp846=_tmp844->f2;
_tmp847=_tmp844->f3;_tmp848=_tmp844->f4;}}_LL393: {struct Cyc_Absyn_Stmt*_tmp86F=
_tmp813;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);void*
rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*
rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp846->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp871;struct _RegionHandle*_tmp872;struct Cyc_Toc_Env*
_tmp870=nv;_tmp871=*_tmp870;_tmp872=_tmp871.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp872,nv,x_var,x_exp),_tmp86F);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp86F,0))->r;
else{if(_tmp848 == 0){struct Cyc_Absyn_Exp*_tmpD40[1];struct Cyc_Absyn_Exp*_tmpD3F[
1];struct Cyc_List_List*_tmpD3E;s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpD3E=
_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0),((_tmpD3E->tl=0,_tmpD3E)))))),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD3F[0]=
x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD3F,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),Cyc_Absyn_seq_stmt(
_tmp86F,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD40[
0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD40,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp848);{struct Cyc_Absyn_Exp*
_tmpD42[1];struct Cyc_Absyn_Exp*_tmpD41[2];s->r=(Cyc_Absyn_declare_stmt(rh_var,
Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,((
_tmpD41[1]=(struct Cyc_Absyn_Exp*)_tmp848,((_tmpD41[0]=Cyc_Absyn_address_exp(
rh_exp,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD41,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),Cyc_Absyn_seq_stmt(
_tmp86F,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpD42[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmpD42,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r;};}}
return;}_LL394: {struct Cyc_Absyn_Alias_d_struct*_tmp849=(struct Cyc_Absyn_Alias_d_struct*)
_tmp83A;if(_tmp849->tag != 5)goto _LL396;else{_tmp84A=_tmp849->f1;_tmp84B=_tmp849->f2;
_tmp84C=_tmp849->f3;}}_LL395: {struct _tuple1*old_name=_tmp84C->name;struct
_tuple1*new_name=Cyc_Toc_temp_var();_tmp84C->name=new_name;_tmp84C->initializer=(
struct Cyc_Absyn_Exp*)_tmp84A;{struct Cyc_Absyn_Decl_s_struct _tmpD51;struct Cyc_Absyn_Var_d_struct*
_tmpD50;struct Cyc_Absyn_Var_d_struct _tmpD4F;struct Cyc_Absyn_Decl*_tmpD4E;struct
Cyc_Absyn_Decl_s_struct*_tmpD4D;s->r=(void*)((_tmpD4D=_cycalloc(sizeof(*_tmpD4D)),((
_tmpD4D[0]=((_tmpD51.tag=12,((_tmpD51.f1=((_tmpD4E=_cycalloc(sizeof(*_tmpD4E)),((
_tmpD4E->r=(void*)((_tmpD50=_cycalloc(sizeof(*_tmpD50)),((_tmpD50[0]=((_tmpD4F.tag=
0,((_tmpD4F.f1=_tmp84C,_tmpD4F)))),_tmpD50)))),((_tmpD4E->loc=0,_tmpD4E)))))),((
_tmpD51.f2=_tmp813,_tmpD51)))))),_tmpD4D))));}{struct Cyc_Toc_Env _tmp87E;struct
_RegionHandle*_tmp87F;struct Cyc_Toc_Env*_tmp87D=nv;_tmp87E=*_tmp87D;_tmp87F=
_tmp87E.rgn;{struct Cyc_Absyn_Local_b_struct _tmpD54;struct Cyc_Absyn_Local_b_struct*
_tmpD53;struct Cyc_Toc_Env*_tmp880=Cyc_Toc_add_varmap(_tmp87F,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpD53=_cycalloc(sizeof(*_tmpD53)),((_tmpD53[0]=((_tmpD54.tag=
4,((_tmpD54.f1=_tmp84C,_tmpD54)))),_tmpD53)))),0));Cyc_Toc_local_decl_to_c(
_tmp880,nv,_tmp84C,_tmp813);}return;};}_LL396:;_LL397: {const char*_tmpD57;void*
_tmpD56;(_tmpD56=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpD57="bad nested declaration within function",_tag_dyneither(_tmpD57,sizeof(
char),39))),_tag_dyneither(_tmpD56,sizeof(void*),0)));}_LL389:;}return;_LL381: {
struct Cyc_Absyn_Label_s_struct*_tmp814=(struct Cyc_Absyn_Label_s_struct*)_tmp7ED;
if(_tmp814->tag != 13)goto _LL383;else{_tmp815=_tmp814->f1;_tmp816=_tmp814->f2;}}
_LL382: s=_tmp816;continue;_LL383: {struct Cyc_Absyn_Do_s_struct*_tmp817=(struct
Cyc_Absyn_Do_s_struct*)_tmp7ED;if(_tmp817->tag != 14)goto _LL385;else{_tmp818=
_tmp817->f1;_tmp819=_tmp817->f2;_tmp81A=_tmp819.f1;}}_LL384: {struct Cyc_Toc_Env
_tmp886;struct _RegionHandle*_tmp887;struct Cyc_Toc_Env*_tmp885=nv;_tmp886=*
_tmp885;_tmp887=_tmp886.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(_tmp887,nv),
_tmp818);Cyc_Toc_exp_to_c(nv,_tmp81A);return;}_LL385: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp81B=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp7ED;if(_tmp81B->tag != 15)goto
_LL387;else{_tmp81C=_tmp81B->f1;_tmp81D=_tmp81B->f2;}}_LL386: {struct _tuple1*
h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();struct _tuple1*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpD58;
e_exp->topt=((_tmpD58=_cycalloc(sizeof(*_tmpD58)),((_tmpD58->v=e_typ,_tmpD58))));}{
struct Cyc_Toc_Env _tmp88A;struct _RegionHandle*_tmp88B;struct Cyc_Toc_Env*_tmp889=
nv;_tmp88A=*_tmp889;_tmp88B=_tmp88A.rgn;{struct Cyc_Toc_Env*_tmp88C=Cyc_Toc_add_varmap(
_tmp88B,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp88C,_tmp81C);{struct Cyc_Absyn_Stmt*
_tmp88D=Cyc_Absyn_seq_stmt(_tmp81C,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp88E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp88F=
Cyc_Absyn_var_exp(_tmp88E,0);struct Cyc_Absyn_Vardecl*_tmp890=Cyc_Absyn_new_vardecl(
_tmp88E,Cyc_Absyn_exn_typ(),0);{struct Cyc_Core_Opt*_tmpD59;_tmp88F->topt=((
_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59->v=Cyc_Absyn_exn_typ(),_tmpD59))));}{
struct Cyc_Core_Opt*_tmpD6A;struct Cyc_Absyn_Var_p_struct*_tmpD69;struct Cyc_Absyn_Pat*
_tmpD68;struct Cyc_Core_Opt*_tmpD67;struct Cyc_Absyn_Var_p_struct _tmpD66;struct Cyc_Absyn_Pat*
_tmpD65;struct Cyc_Absyn_Pat*_tmp892=(_tmpD65=_cycalloc(sizeof(*_tmpD65)),((
_tmpD65->r=(void*)((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69[0]=((_tmpD66.tag=
1,((_tmpD66.f1=_tmp890,((_tmpD66.f2=((_tmpD68=_cycalloc(sizeof(*_tmpD68)),((
_tmpD68->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD68->topt=((_tmpD67=_cycalloc(
sizeof(*_tmpD67)),((_tmpD67->v=Cyc_Absyn_exn_typ(),_tmpD67)))),((_tmpD68->loc=0,
_tmpD68)))))))),_tmpD66)))))),_tmpD69)))),((_tmpD65->topt=((_tmpD6A=_cycalloc(
sizeof(*_tmpD6A)),((_tmpD6A->v=Cyc_Absyn_exn_typ(),_tmpD6A)))),((_tmpD65->loc=0,
_tmpD65)))))));struct Cyc_Absyn_Exp*_tmp893=Cyc_Absyn_throw_exp(_tmp88F,0);{
struct Cyc_Core_Opt*_tmpD6B;_tmp893->topt=((_tmpD6B=_cycalloc(sizeof(*_tmpD6B)),((
_tmpD6B->v=(void*)& Cyc_Absyn_VoidType_val,_tmpD6B))));}{struct Cyc_Absyn_Stmt*
_tmp895=Cyc_Absyn_exp_stmt(_tmp893,0);struct Cyc_Core_Opt*_tmpD71;struct Cyc_List_List*
_tmpD70;struct Cyc_Absyn_Switch_clause*_tmpD6F;struct Cyc_Absyn_Switch_clause*
_tmp896=(_tmpD6F=_cycalloc(sizeof(*_tmpD6F)),((_tmpD6F->pattern=_tmp892,((
_tmpD6F->pat_vars=((_tmpD71=_cycalloc(sizeof(*_tmpD71)),((_tmpD71->v=((_tmpD70=
_cycalloc(sizeof(*_tmpD70)),((_tmpD70->hd=_tmp890,((_tmpD70->tl=0,_tmpD70)))))),
_tmpD71)))),((_tmpD6F->where_clause=0,((_tmpD6F->body=_tmp895,((_tmpD6F->loc=0,
_tmpD6F)))))))))));struct Cyc_List_List*_tmpD72;struct Cyc_Absyn_Stmt*_tmp897=Cyc_Absyn_switch_stmt(
e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp81D,((_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72->hd=_tmp896,((_tmpD72->tl=
0,_tmpD72))))))),0);Cyc_Toc_stmt_to_c(_tmp88C,_tmp897);{struct Cyc_List_List*
_tmpD73;struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,((
_tmpD73=_cycalloc(sizeof(*_tmpD73)),((_tmpD73->hd=Cyc_Absyn_aggrmember_exp(h_exp,
Cyc_Toc_handler_sp,0),((_tmpD73->tl=0,_tmpD73)))))),0);struct Cyc_List_List*
_tmpD74;struct Cyc_Absyn_Stmt*_tmp899=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpD74->tl=0,_tmpD74)))))),0),0);struct Cyc_Absyn_Exp*_tmp89A=Cyc_Absyn_int_exp(
Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp89B=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,
1,0);s->r=(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp899,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp89A,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp898,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp89B,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,
was_thrown_exp,0),_tmp88D,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp897,0),0),0),0),0),0))->r;};};};};}
return;};}_LL387: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp81E=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp7ED;if(_tmp81E->tag != 16)goto _LL366;else{_tmp81F=_tmp81E->f1;}}_LL388: if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{Cyc_Toc_exp_to_c(nv,_tmp81F);{struct Cyc_List_List*
_tmpD75;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((
_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75->hd=_tmp81F,((_tmpD75->tl=0,
_tmpD75)))))),0));};}return;_LL366:;}}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int
cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8A9=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8A9;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp8AA=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp8AB=f->args;for(0;_tmp8AB != 0;_tmp8AB=_tmp8AB->tl){struct
_tuple1*_tmpD76;struct _tuple1*_tmp8AC=(_tmpD76=_cycalloc(sizeof(*_tmpD76)),((
_tmpD76->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD76->f2=(*((struct
_tuple25*)_tmp8AB->hd)).f1,_tmpD76)))));(*((struct _tuple25*)_tmp8AB->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp8AB->hd)).f3);_tmp8AA=Cyc_Toc_add_varmap(frgn,_tmp8AA,
_tmp8AC,Cyc_Absyn_var_exp(_tmp8AC,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp8AF;struct Cyc_Absyn_Tqual
_tmp8B0;void*_tmp8B1;int _tmp8B2;struct Cyc_Absyn_VarargInfo _tmp8AE=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp8AF=_tmp8AE.name;_tmp8B0=_tmp8AE.tq;_tmp8B1=
_tmp8AE.type;_tmp8B2=_tmp8AE.inject;{void*_tmp8B3=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp8B1,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8B0,Cyc_Absyn_false_conref));struct
_tuple1*_tmpD77;struct _tuple1*_tmp8B4=(_tmpD77=_cycalloc(sizeof(*_tmpD77)),((
_tmpD77->f1=(union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n,((_tmpD77->f2=(struct
_dyneither_ptr*)((struct Cyc_Core_Opt*)_check_null(_tmp8AF))->v,_tmpD77)))));{
struct _tuple25*_tmpD7A;struct Cyc_List_List*_tmpD79;f->args=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD79=
_cycalloc(sizeof(*_tmpD79)),((_tmpD79->hd=((_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((
_tmpD7A->f1=(struct _dyneither_ptr*)_tmp8AF->v,((_tmpD7A->f2=_tmp8B0,((_tmpD7A->f3=
_tmp8B3,_tmpD7A)))))))),((_tmpD79->tl=0,_tmpD79)))))));}_tmp8AA=Cyc_Toc_add_varmap(
frgn,_tmp8AA,_tmp8B4,Cyc_Absyn_var_exp(_tmp8B4,0));f->cyc_varargs=0;};}{struct
Cyc_List_List*_tmp8B8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp8B8 != 0;_tmp8B8=_tmp8B8->tl){((struct Cyc_Absyn_Vardecl*)_tmp8B8->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8B8->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp8AA),f->body);};_pop_region(frgn);};}static enum 
Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s);static enum Cyc_Absyn_Scope
Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){switch(s){case Cyc_Absyn_Abstract:
_LL39D: return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL39E: return Cyc_Absyn_Extern;
default: _LL39F: return s;}}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad);static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp8B9=ad->name;struct _DynRegionHandle*_tmp8BB;struct Cyc_Dict_Dict*_tmp8BC;
struct Cyc_Toc_TocState _tmp8BA=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8BB=_tmp8BA.dyn;_tmp8BC=_tmp8BA.aggrs_so_far;{struct _DynRegionFrame _tmp8BD;
struct _RegionHandle*d=_open_dynregion(& _tmp8BD,_tmp8BB);{int seen_defn_before;
struct _tuple15**_tmp8BE=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*
k))Cyc_Dict_lookup_opt)(*_tmp8BC,_tmp8B9);if(_tmp8BE == 0){seen_defn_before=0;{
struct _tuple15*v;if(ad->kind == Cyc_Absyn_StructA){struct _tuple15*_tmpD7B;v=((
_tmpD7B=_region_malloc(d,sizeof(*_tmpD7B)),((_tmpD7B->f1=ad,((_tmpD7B->f2=Cyc_Absyn_strctq(
_tmp8B9),_tmpD7B))))));}else{struct _tuple15*_tmpD7C;v=((_tmpD7C=_region_malloc(d,
sizeof(*_tmpD7C)),((_tmpD7C->f1=ad,((_tmpD7C->f2=Cyc_Absyn_unionq_typ(_tmp8B9),
_tmpD7C))))));}*_tmp8BC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp8BC,_tmp8B9,v);};}else{struct
_tuple15 _tmp8C2;struct Cyc_Absyn_Aggrdecl*_tmp8C3;void*_tmp8C4;struct _tuple15*
_tmp8C1=*_tmp8BE;_tmp8C2=*_tmp8C1;_tmp8C3=_tmp8C2.f1;_tmp8C4=_tmp8C2.f2;if(
_tmp8C3->impl == 0){{struct _tuple15*_tmpD7D;*_tmp8BC=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple15*v))Cyc_Dict_insert)(*_tmp8BC,
_tmp8B9,((_tmpD7D=_region_malloc(d,sizeof(*_tmpD7D)),((_tmpD7D->f1=ad,((_tmpD7D->f2=
_tmp8C4,_tmpD7D)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*
_tmpD7E;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((
_tmpD7E->kind=ad->kind,((_tmpD7E->sc=Cyc_Absyn_Public,((_tmpD7E->name=ad->name,((
_tmpD7E->tvs=0,((_tmpD7E->impl=0,((_tmpD7E->attributes=ad->attributes,_tmpD7E)))))))))))));
if(ad->impl != 0  && !seen_defn_before){{struct Cyc_Absyn_AggrdeclImpl*_tmpD7F;
new_ad->impl=((_tmpD7F=_cycalloc(sizeof(*_tmpD7F)),((_tmpD7F->exist_vars=0,((
_tmpD7F->rgn_po=0,((_tmpD7F->fields=0,((_tmpD7F->tagged=0,_tmpD7F))))))))));}{
struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*_tmp8C7=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp8C7 != 0;_tmp8C7=_tmp8C7->tl){struct Cyc_Absyn_Aggrfield*
_tmp8C8=(struct Cyc_Absyn_Aggrfield*)_tmp8C7->hd;struct Cyc_Absyn_Aggrfield*
_tmpD80;struct Cyc_Absyn_Aggrfield*_tmp8C9=(_tmpD80=_cycalloc(sizeof(*_tmpD80)),((
_tmpD80->name=_tmp8C8->name,((_tmpD80->tq=Cyc_Toc_mt_tq,((_tmpD80->type=Cyc_Toc_typ_to_c_array(
_tmp8C8->type),((_tmpD80->width=_tmp8C8->width,((_tmpD80->attributes=_tmp8C8->attributes,
_tmpD80)))))))))));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
void*_tmp8CA=_tmp8C9->type;struct _dyneither_ptr*_tmp8CB=_tmp8C9->name;const char*
_tmpD85;void*_tmpD84[2];struct Cyc_String_pa_struct _tmpD83;struct Cyc_String_pa_struct
_tmpD82;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpD82.tag=0,((_tmpD82.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8CB),((_tmpD83.tag=0,((_tmpD83.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2),((_tmpD84[0]=&
_tmpD83,((_tmpD84[1]=& _tmpD82,Cyc_aprintf(((_tmpD85="_union_%s_%s",
_tag_dyneither(_tmpD85,sizeof(char),13))),_tag_dyneither(_tmpD84,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpD86;struct _dyneither_ptr*str=(_tmpD86=_cycalloc(sizeof(*
_tmpD86)),((_tmpD86[0]=s,_tmpD86)));struct Cyc_Absyn_Aggrfield*_tmpD87;struct Cyc_Absyn_Aggrfield*
_tmp8CC=(_tmpD87=_cycalloc(sizeof(*_tmpD87)),((_tmpD87->name=Cyc_Toc_val_sp,((
_tmpD87->tq=Cyc_Toc_mt_tq,((_tmpD87->type=_tmp8CA,((_tmpD87->width=0,((_tmpD87->attributes=
0,_tmpD87)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD88;struct Cyc_Absyn_Aggrfield*
_tmp8CD=(_tmpD88=_cycalloc(sizeof(*_tmpD88)),((_tmpD88->name=Cyc_Toc_tag_sp,((
_tmpD88->tq=Cyc_Toc_mt_tq,((_tmpD88->type=Cyc_Absyn_sint_typ,((_tmpD88->width=0,((
_tmpD88->attributes=0,_tmpD88)))))))))));struct Cyc_Absyn_Aggrfield*_tmpD89[2];
struct Cyc_List_List*_tmp8CE=(_tmpD89[1]=_tmp8CC,((_tmpD89[0]=_tmp8CD,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD89,sizeof(struct Cyc_Absyn_Aggrfield*),
2)))));struct Cyc_Absyn_AggrdeclImpl*_tmpD8E;struct _tuple1*_tmpD8D;struct Cyc_Absyn_Aggrdecl*
_tmpD8C;struct Cyc_Absyn_Aggrdecl*_tmp8CF=(_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((
_tmpD8C->kind=Cyc_Absyn_StructA,((_tmpD8C->sc=Cyc_Absyn_Public,((_tmpD8C->name=((
_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D->f1=Cyc_Absyn_Loc_n,((_tmpD8D->f2=
str,_tmpD8D)))))),((_tmpD8C->tvs=0,((_tmpD8C->impl=((_tmpD8E=_cycalloc(sizeof(*
_tmpD8E)),((_tmpD8E->exist_vars=0,((_tmpD8E->rgn_po=0,((_tmpD8E->fields=_tmp8CE,((
_tmpD8E->tagged=0,_tmpD8E)))))))))),((_tmpD8C->attributes=0,_tmpD8C)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmpD94;struct Cyc_Absyn_Aggr_d_struct _tmpD93;
struct Cyc_List_List*_tmpD92;Cyc_Toc_result_decls=((_tmpD92=_cycalloc(sizeof(*
_tmpD92)),((_tmpD92->hd=Cyc_Absyn_new_decl((void*)((_tmpD94=_cycalloc(sizeof(*
_tmpD94)),((_tmpD94[0]=((_tmpD93.tag=6,((_tmpD93.f1=_tmp8CF,_tmpD93)))),_tmpD94)))),
0),((_tmpD92->tl=Cyc_Toc_result_decls,_tmpD92))))));}_tmp8C9->type=Cyc_Absyn_strct(
str);}{struct Cyc_List_List*_tmpD95;new_fields=((_tmpD95=_cycalloc(sizeof(*
_tmpD95)),((_tmpD95->hd=_tmp8C9,((_tmpD95->tl=new_fields,_tmpD95))))));};}}(
new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_fields);};}{struct Cyc_Absyn_Decl*_tmpD9F;struct Cyc_Absyn_Aggr_d_struct
_tmpD9E;struct Cyc_Absyn_Aggr_d_struct*_tmpD9D;struct Cyc_List_List*_tmpD9C;Cyc_Toc_result_decls=((
_tmpD9C=_cycalloc(sizeof(*_tmpD9C)),((_tmpD9C->hd=((_tmpD9F=_cycalloc(sizeof(*
_tmpD9F)),((_tmpD9F->r=(void*)((_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D[0]=((
_tmpD9E.tag=6,((_tmpD9E.f1=new_ad,_tmpD9E)))),_tmpD9D)))),((_tmpD9F->loc=0,
_tmpD9F)))))),((_tmpD9C->tl=Cyc_Toc_result_decls,_tmpD9C))))));};};};
_pop_dynregion(d);};}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud);static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
struct _DynRegionHandle*_tmp8E6;struct Cyc_Set_Set**_tmp8E7;struct Cyc_Toc_TocState
_tmp8E5=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp8E6=
_tmp8E5.dyn;_tmp8E7=_tmp8E5.datatypes_so_far;{struct _DynRegionFrame _tmp8E8;
struct _RegionHandle*d=_open_dynregion(& _tmp8E8,_tmp8E6);{struct _tuple1*_tmp8E9=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))
Cyc_Set_member)(*_tmp8E7,_tmp8E9)){_npop_handler(0);return;}*_tmp8E7=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp8E7,_tmp8E9);}{struct Cyc_List_List*_tmp8EA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp8EA != 0;_tmp8EA=_tmp8EA->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8EA->hd;struct Cyc_List_List*_tmp8EB=0;int i=
1;{struct Cyc_List_List*_tmp8EC=f->typs;for(0;_tmp8EC != 0;(_tmp8EC=_tmp8EC->tl,i
++)){struct _dyneither_ptr*_tmp8ED=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmpDA0;struct Cyc_Absyn_Aggrfield*_tmp8EE=(_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((
_tmpDA0->name=_tmp8ED,((_tmpDA0->tq=(*((struct _tuple10*)_tmp8EC->hd)).f1,((
_tmpDA0->type=Cyc_Toc_typ_to_c_array((*((struct _tuple10*)_tmp8EC->hd)).f2),((
_tmpDA0->width=0,((_tmpDA0->attributes=0,_tmpDA0)))))))))));struct Cyc_List_List*
_tmpDA1;_tmp8EB=((_tmpDA1=_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1->hd=_tmp8EE,((
_tmpDA1->tl=_tmp8EB,_tmpDA1))))));}}{struct Cyc_Absyn_Aggrfield*_tmpDA4;struct Cyc_List_List*
_tmpDA3;_tmp8EB=((_tmpDA3=_cycalloc(sizeof(*_tmpDA3)),((_tmpDA3->hd=((_tmpDA4=
_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4->name=Cyc_Toc_tag_sp,((_tmpDA4->tq=Cyc_Toc_mt_tq,((
_tmpDA4->type=Cyc_Absyn_sint_typ,((_tmpDA4->width=0,((_tmpDA4->attributes=0,
_tmpDA4)))))))))))),((_tmpDA3->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp8EB),_tmpDA3))))));}{struct Cyc_Absyn_AggrdeclImpl*
_tmpDA9;const char*_tmpDA8;struct Cyc_Absyn_Aggrdecl*_tmpDA7;struct Cyc_Absyn_Aggrdecl*
_tmp8F3=(_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7->kind=Cyc_Absyn_StructA,((
_tmpDA7->sc=Cyc_Absyn_Public,((_tmpDA7->name=Cyc_Toc_collapse_qvar_tag(f->name,((
_tmpDA8="_struct",_tag_dyneither(_tmpDA8,sizeof(char),8)))),((_tmpDA7->tvs=0,((
_tmpDA7->impl=((_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9->exist_vars=0,((
_tmpDA9->rgn_po=0,((_tmpDA9->fields=_tmp8EB,((_tmpDA9->tagged=0,_tmpDA9)))))))))),((
_tmpDA7->attributes=0,_tmpDA7)))))))))))));struct Cyc_Absyn_Aggr_d_struct*_tmpDAF;
struct Cyc_Absyn_Aggr_d_struct _tmpDAE;struct Cyc_List_List*_tmpDAD;Cyc_Toc_result_decls=((
_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->hd=Cyc_Absyn_new_decl((void*)((
_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF[0]=((_tmpDAE.tag=6,((_tmpDAE.f1=
_tmp8F3,_tmpDAE)))),_tmpDAF)))),0),((_tmpDAD->tl=Cyc_Toc_result_decls,_tmpDAD))))));};}};;
_pop_dynregion(d);};}static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
xd);static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields
== 0)return;{struct _DynRegionHandle*_tmp8FB;struct Cyc_Dict_Dict*_tmp8FC;struct
Cyc_Toc_TocState _tmp8FA=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp8FB=_tmp8FA.dyn;_tmp8FC=_tmp8FA.xdatatypes_so_far;{struct _DynRegionFrame
_tmp8FD;struct _RegionHandle*d=_open_dynregion(& _tmp8FD,_tmp8FB);{struct _tuple1*
_tmp8FE=xd->name;struct Cyc_List_List*_tmp8FF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp8FF != 0;_tmp8FF=_tmp8FF->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp8FF->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp900=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char)),0);void*_tmp901=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp900,Cyc_Absyn_false_conref,0);int*_tmp902=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8FC,f->name);int
_tmp903;_LL3A2: if(_tmp902 != 0)goto _LL3A4;_LL3A3: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp(*
fn,0);{struct Cyc_Absyn_Vardecl*_tmp904=Cyc_Absyn_new_vardecl(f->name,_tmp901,
initopt);_tmp904->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*_tmpDB5;struct Cyc_Absyn_Var_d_struct
_tmpDB4;struct Cyc_List_List*_tmpDB3;Cyc_Toc_result_decls=((_tmpDB3=_cycalloc(
sizeof(*_tmpDB3)),((_tmpDB3->hd=Cyc_Absyn_new_decl((void*)((_tmpDB5=_cycalloc(
sizeof(*_tmpDB5)),((_tmpDB5[0]=((_tmpDB4.tag=0,((_tmpDB4.f1=_tmp904,_tmpDB4)))),
_tmpDB5)))),0),((_tmpDB3->tl=Cyc_Toc_result_decls,_tmpDB3))))));}*_tmp8FC=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp8FC,f->name,f->sc != Cyc_Absyn_Extern);{struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp908=f->typs;for(0;_tmp908 != 0;(_tmp908=_tmp908->tl,i ++)){
struct Cyc_Int_pa_struct _tmpDBD;void*_tmpDBC[1];const char*_tmpDBB;struct
_dyneither_ptr*_tmpDBA;struct _dyneither_ptr*_tmp909=(_tmpDBA=_cycalloc(sizeof(*
_tmpDBA)),((_tmpDBA[0]=(struct _dyneither_ptr)((_tmpDBD.tag=1,((_tmpDBD.f1=(
unsigned long)i,((_tmpDBC[0]=& _tmpDBD,Cyc_aprintf(((_tmpDBB="f%d",_tag_dyneither(
_tmpDBB,sizeof(char),4))),_tag_dyneither(_tmpDBC,sizeof(void*),1)))))))),_tmpDBA)));
struct Cyc_Absyn_Aggrfield*_tmpDBE;struct Cyc_Absyn_Aggrfield*_tmp90A=(_tmpDBE=
_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->name=_tmp909,((_tmpDBE->tq=(*((struct
_tuple10*)_tmp908->hd)).f1,((_tmpDBE->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple10*)_tmp908->hd)).f2),((_tmpDBE->width=0,((_tmpDBE->attributes=0,_tmpDBE)))))))))));
struct Cyc_List_List*_tmpDBF;fields=((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((
_tmpDBF->hd=_tmp90A,((_tmpDBF->tl=fields,_tmpDBF))))));}}{struct Cyc_Absyn_Aggrfield*
_tmpDC2;struct Cyc_List_List*_tmpDC1;fields=((_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((
_tmpDC1->hd=((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2->name=Cyc_Toc_tag_sp,((
_tmpDC2->tq=Cyc_Toc_mt_tq,((_tmpDC2->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq),((_tmpDC2->width=0,((_tmpDC2->attributes=0,_tmpDC2)))))))))))),((
_tmpDC1->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmpDC1))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpDC7;const char*_tmpDC6;
struct Cyc_Absyn_Aggrdecl*_tmpDC5;struct Cyc_Absyn_Aggrdecl*_tmp913=(_tmpDC5=
_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5->kind=Cyc_Absyn_StructA,((_tmpDC5->sc=Cyc_Absyn_Public,((
_tmpDC5->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpDC6="_struct",
_tag_dyneither(_tmpDC6,sizeof(char),8)))),((_tmpDC5->tvs=0,((_tmpDC5->impl=((
_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->exist_vars=0,((_tmpDC7->rgn_po=0,((
_tmpDC7->fields=fields,((_tmpDC7->tagged=0,_tmpDC7)))))))))),((_tmpDC5->attributes=
0,_tmpDC5)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpDCD;struct Cyc_Absyn_Aggr_d_struct
_tmpDCC;struct Cyc_List_List*_tmpDCB;Cyc_Toc_result_decls=((_tmpDCB=_cycalloc(
sizeof(*_tmpDCB)),((_tmpDCB->hd=Cyc_Absyn_new_decl((void*)((_tmpDCD=_cycalloc(
sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDCC.tag=6,((_tmpDCC.f1=_tmp913,_tmpDCC)))),
_tmpDCD)))),0),((_tmpDCB->tl=Cyc_Toc_result_decls,_tmpDCB))))));}goto _LL3A1;};};};}
_LL3A4: if(_tmp902 == 0)goto _LL3A6;_tmp903=*_tmp902;if(_tmp903 != 0)goto _LL3A6;
_LL3A5: if(f->sc != Cyc_Absyn_Extern){struct Cyc_Absyn_Exp*_tmp91A=Cyc_Absyn_string_exp(*
fn,0);struct Cyc_Absyn_Vardecl*_tmp91B=Cyc_Absyn_new_vardecl(f->name,_tmp901,(
struct Cyc_Absyn_Exp*)_tmp91A);_tmp91B->sc=f->sc;{struct Cyc_Absyn_Var_d_struct*
_tmpDD3;struct Cyc_Absyn_Var_d_struct _tmpDD2;struct Cyc_List_List*_tmpDD1;Cyc_Toc_result_decls=((
_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1->hd=Cyc_Absyn_new_decl((void*)((
_tmpDD3=_cycalloc(sizeof(*_tmpDD3)),((_tmpDD3[0]=((_tmpDD2.tag=0,((_tmpDD2.f1=
_tmp91B,_tmpDD2)))),_tmpDD3)))),0),((_tmpDD1->tl=Cyc_Toc_result_decls,_tmpDD1))))));}*
_tmp8FC=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp8FC,f->name,1);}goto _LL3A1;_LL3A6:;_LL3A7: goto _LL3A1;_LL3A1:;}};
_pop_dynregion(d);};};}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Enumdecl*ed){ed->sc=Cyc_Absyn_Public;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=Cyc_Toc_typ_to_c_array(
old_typ);if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=Cyc_Absyn_Public;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp91F=init->r;struct Cyc_Absyn_Vardecl*_tmp921;struct Cyc_Absyn_Exp*_tmp922;
struct Cyc_Absyn_Exp*_tmp923;int _tmp924;_LL3A9: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp920=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp91F;if(_tmp920->tag != 29)
goto _LL3AB;else{_tmp921=_tmp920->f1;_tmp922=_tmp920->f2;_tmp923=_tmp920->f3;
_tmp924=_tmp920->f4;}}_LL3AA: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp921,_tmp922,_tmp923,_tmp924,Cyc_Absyn_new_stmt(
s->r,0),0))->r;goto _LL3A8;_LL3AB:;_LL3AC: if(vd->sc == Cyc_Absyn_Static){struct Cyc_Toc_Env
_tmp926;struct _RegionHandle*_tmp927;struct Cyc_Toc_Env*_tmp925=init_nv;_tmp926=*
_tmp925;_tmp927=_tmp926.rgn;{struct Cyc_Toc_Env*_tmp928=Cyc_Toc_set_toplevel(
_tmp927,init_nv);Cyc_Toc_exp_to_c(_tmp928,init);};}else{Cyc_Toc_exp_to_c(init_nv,
init);}goto _LL3A8;_LL3A8:;}else{void*_tmp929=Cyc_Tcutil_compress(old_typ);struct
Cyc_Absyn_ArrayInfo _tmp92B;void*_tmp92C;struct Cyc_Absyn_Exp*_tmp92D;union Cyc_Absyn_Constraint*
_tmp92E;_LL3AE:{struct Cyc_Absyn_ArrayType_struct*_tmp92A=(struct Cyc_Absyn_ArrayType_struct*)
_tmp929;if(_tmp92A->tag != 9)goto _LL3B0;else{_tmp92B=_tmp92A->f1;_tmp92C=_tmp92B.elt_type;
_tmp92D=_tmp92B.num_elts;_tmp92E=_tmp92B.zero_term;}}if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp92E))goto _LL3B0;_LL3AF: if(_tmp92D == 0){const char*
_tmpDD6;void*_tmpDD5;(_tmpDD5=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDD6="can't initialize zero-terminated array -- size unknown",
_tag_dyneither(_tmpDD6,sizeof(char),55))),_tag_dyneither(_tmpDD5,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp92D;struct Cyc_Absyn_Exp*
_tmp931=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp932=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp931,
_tmp932,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3AD;};_LL3B0:;_LL3B1: goto _LL3AD;
_LL3AD:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp934;struct _RegionHandle*
_tmp935;struct Cyc_Toc_Env*_tmp933=nv;_tmp934=*_tmp933;_tmp935=_tmp934.rgn;{
struct Cyc_Absyn_Stmt*_tmp936=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp937=Cyc_Toc_share_env(_tmp935,nv);struct Cyc_Toc_Env*_tmp939;struct Cyc_List_List*
_tmp93A;struct Cyc_Absyn_Stmt*_tmp93B;struct _tuple21 _tmp938=Cyc_Toc_xlate_pat(
_tmp937,_tmp935,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,Cyc_Toc_throw_match_stmt(),
0);_tmp939=_tmp938.f1;_tmp93A=_tmp938.f2;_tmp93B=_tmp938.f3;Cyc_Toc_stmt_to_c(
_tmp939,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp93B,s,0),0);for(0;_tmp93A != 0;_tmp93A=_tmp93A->tl){
struct _tuple22 _tmp93D;struct _tuple1*_tmp93E;void*_tmp93F;struct _tuple22*_tmp93C=(
struct _tuple22*)_tmp93A->hd;_tmp93D=*_tmp93C;_tmp93E=_tmp93D.f1;_tmp93F=_tmp93D.f2;
s=Cyc_Absyn_declare_stmt(_tmp93E,_tmp93F,0,s,0);}}return s;};}static void Cyc_Toc_exptypes_to_c(
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp940=e->r;struct Cyc_Absyn_Exp*_tmp942;struct Cyc_Absyn_Exp*_tmp944;struct
Cyc_Absyn_Exp*_tmp946;struct Cyc_Absyn_Exp*_tmp948;struct Cyc_Absyn_Exp*_tmp94A;
struct Cyc_Absyn_Exp*_tmp94C;struct Cyc_Absyn_Exp*_tmp94E;struct Cyc_Absyn_Exp*
_tmp950;struct Cyc_List_List*_tmp952;struct Cyc_Absyn_Exp*_tmp954;struct Cyc_Absyn_Exp*
_tmp955;struct Cyc_Absyn_Exp*_tmp957;struct Cyc_Absyn_Exp*_tmp958;struct Cyc_Absyn_Exp*
_tmp95A;struct Cyc_Absyn_Exp*_tmp95B;struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_Absyn_Exp*
_tmp95E;struct Cyc_Absyn_Exp*_tmp960;struct Cyc_Absyn_Exp*_tmp961;struct Cyc_Absyn_Exp*
_tmp963;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*
_tmp967;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_List_List*
_tmp96B;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_List_List*_tmp96E;void*_tmp970;
void**_tmp971;struct Cyc_Absyn_Exp*_tmp972;struct _tuple8*_tmp974;struct _tuple8
_tmp975;void*_tmp976;void**_tmp977;struct Cyc_List_List*_tmp978;struct Cyc_List_List*
_tmp97A;struct Cyc_List_List*_tmp97C;void*_tmp97E;void**_tmp97F;void*_tmp981;void**
_tmp982;struct Cyc_Absyn_Stmt*_tmp984;struct Cyc_Absyn_MallocInfo _tmp986;struct Cyc_Absyn_MallocInfo*
_tmp987;_LL3B3: {struct Cyc_Absyn_Deref_e_struct*_tmp941=(struct Cyc_Absyn_Deref_e_struct*)
_tmp940;if(_tmp941->tag != 22)goto _LL3B5;else{_tmp942=_tmp941->f1;}}_LL3B4:
_tmp944=_tmp942;goto _LL3B6;_LL3B5: {struct Cyc_Absyn_AggrMember_e_struct*_tmp943=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp940;if(_tmp943->tag != 23)goto _LL3B7;
else{_tmp944=_tmp943->f1;}}_LL3B6: _tmp946=_tmp944;goto _LL3B8;_LL3B7: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp945=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp940;if(_tmp945->tag != 24)goto
_LL3B9;else{_tmp946=_tmp945->f1;}}_LL3B8: _tmp948=_tmp946;goto _LL3BA;_LL3B9: {
struct Cyc_Absyn_Address_e_struct*_tmp947=(struct Cyc_Absyn_Address_e_struct*)
_tmp940;if(_tmp947->tag != 16)goto _LL3BB;else{_tmp948=_tmp947->f1;}}_LL3BA:
_tmp94A=_tmp948;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_Throw_e_struct*_tmp949=(
struct Cyc_Absyn_Throw_e_struct*)_tmp940;if(_tmp949->tag != 12)goto _LL3BD;else{
_tmp94A=_tmp949->f1;}}_LL3BC: _tmp94C=_tmp94A;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp94B=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp940;if(_tmp94B->tag != 13)
goto _LL3BF;else{_tmp94C=_tmp94B->f1;}}_LL3BE: _tmp94E=_tmp94C;goto _LL3C0;_LL3BF: {
struct Cyc_Absyn_Sizeofexp_e_struct*_tmp94D=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp940;if(_tmp94D->tag != 19)goto _LL3C1;else{_tmp94E=_tmp94D->f1;}}_LL3C0:
_tmp950=_tmp94E;goto _LL3C2;_LL3C1: {struct Cyc_Absyn_Increment_e_struct*_tmp94F=(
struct Cyc_Absyn_Increment_e_struct*)_tmp940;if(_tmp94F->tag != 5)goto _LL3C3;else{
_tmp950=_tmp94F->f1;}}_LL3C2: Cyc_Toc_exptypes_to_c(_tmp950);goto _LL3B2;_LL3C3: {
struct Cyc_Absyn_Primop_e_struct*_tmp951=(struct Cyc_Absyn_Primop_e_struct*)
_tmp940;if(_tmp951->tag != 3)goto _LL3C5;else{_tmp952=_tmp951->f2;}}_LL3C4:((void(*)(
void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,
_tmp952);goto _LL3B2;_LL3C5: {struct Cyc_Absyn_And_e_struct*_tmp953=(struct Cyc_Absyn_And_e_struct*)
_tmp940;if(_tmp953->tag != 7)goto _LL3C7;else{_tmp954=_tmp953->f1;_tmp955=_tmp953->f2;}}
_LL3C6: _tmp957=_tmp954;_tmp958=_tmp955;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_Or_e_struct*
_tmp956=(struct Cyc_Absyn_Or_e_struct*)_tmp940;if(_tmp956->tag != 8)goto _LL3C9;
else{_tmp957=_tmp956->f1;_tmp958=_tmp956->f2;}}_LL3C8: _tmp95A=_tmp957;_tmp95B=
_tmp958;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_SeqExp_e_struct*_tmp959=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp940;if(_tmp959->tag != 9)goto _LL3CB;else{_tmp95A=_tmp959->f1;_tmp95B=_tmp959->f2;}}
_LL3CA: _tmp95D=_tmp95A;_tmp95E=_tmp95B;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Subscript_e_struct*
_tmp95C=(struct Cyc_Absyn_Subscript_e_struct*)_tmp940;if(_tmp95C->tag != 25)goto
_LL3CD;else{_tmp95D=_tmp95C->f1;_tmp95E=_tmp95C->f2;}}_LL3CC: _tmp960=_tmp95D;
_tmp961=_tmp95E;goto _LL3CE;_LL3CD: {struct Cyc_Absyn_Swap_e_struct*_tmp95F=(
struct Cyc_Absyn_Swap_e_struct*)_tmp940;if(_tmp95F->tag != 36)goto _LL3CF;else{
_tmp960=_tmp95F->f1;_tmp961=_tmp95F->f2;}}_LL3CE: _tmp963=_tmp960;_tmp964=_tmp961;
goto _LL3D0;_LL3CF: {struct Cyc_Absyn_AssignOp_e_struct*_tmp962=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp940;if(_tmp962->tag != 4)goto _LL3D1;else{_tmp963=_tmp962->f1;_tmp964=_tmp962->f3;}}
_LL3D0: Cyc_Toc_exptypes_to_c(_tmp963);Cyc_Toc_exptypes_to_c(_tmp964);goto _LL3B2;
_LL3D1: {struct Cyc_Absyn_Conditional_e_struct*_tmp965=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp940;if(_tmp965->tag != 6)goto _LL3D3;else{_tmp966=_tmp965->f1;_tmp967=_tmp965->f2;
_tmp968=_tmp965->f3;}}_LL3D2: Cyc_Toc_exptypes_to_c(_tmp966);Cyc_Toc_exptypes_to_c(
_tmp967);Cyc_Toc_exptypes_to_c(_tmp968);goto _LL3B2;_LL3D3: {struct Cyc_Absyn_FnCall_e_struct*
_tmp969=(struct Cyc_Absyn_FnCall_e_struct*)_tmp940;if(_tmp969->tag != 11)goto
_LL3D5;else{_tmp96A=_tmp969->f1;_tmp96B=_tmp969->f2;}}_LL3D4: _tmp96D=_tmp96A;
_tmp96E=_tmp96B;goto _LL3D6;_LL3D5: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp96C=(
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp940;if(_tmp96C->tag != 10)goto _LL3D7;
else{_tmp96D=_tmp96C->f1;_tmp96E=_tmp96C->f2;}}_LL3D6: Cyc_Toc_exptypes_to_c(
_tmp96D);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp96E);goto _LL3B2;_LL3D7: {struct Cyc_Absyn_Cast_e_struct*
_tmp96F=(struct Cyc_Absyn_Cast_e_struct*)_tmp940;if(_tmp96F->tag != 15)goto _LL3D9;
else{_tmp970=(void**)& _tmp96F->f1;_tmp971=(void**)((void**)& _tmp96F->f1);_tmp972=
_tmp96F->f2;}}_LL3D8:*_tmp971=Cyc_Toc_typ_to_c(*_tmp971);Cyc_Toc_exptypes_to_c(
_tmp972);goto _LL3B2;_LL3D9: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp973=(
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp940;if(_tmp973->tag != 27)goto _LL3DB;
else{_tmp974=_tmp973->f1;_tmp975=*_tmp974;_tmp976=_tmp975.f3;_tmp977=(void**)&(*
_tmp973->f1).f3;_tmp978=_tmp973->f2;}}_LL3DA:*_tmp977=Cyc_Toc_typ_to_c(*_tmp977);
_tmp97A=_tmp978;goto _LL3DC;_LL3DB: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp979=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp940;if(_tmp979->tag != 37)
goto _LL3DD;else{_tmp97A=_tmp979->f2;}}_LL3DC: _tmp97C=_tmp97A;goto _LL3DE;_LL3DD: {
struct Cyc_Absyn_Array_e_struct*_tmp97B=(struct Cyc_Absyn_Array_e_struct*)_tmp940;
if(_tmp97B->tag != 28)goto _LL3DF;else{_tmp97C=_tmp97B->f1;}}_LL3DE: for(0;_tmp97C
!= 0;_tmp97C=_tmp97C->tl){struct Cyc_Absyn_Exp*_tmp998;struct _tuple16 _tmp997=*((
struct _tuple16*)_tmp97C->hd);_tmp998=_tmp997.f2;Cyc_Toc_exptypes_to_c(_tmp998);}
goto _LL3B2;_LL3DF: {struct Cyc_Absyn_Offsetof_e_struct*_tmp97D=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp940;if(_tmp97D->tag != 20)goto _LL3E1;else{_tmp97E=(void**)& _tmp97D->f1;
_tmp97F=(void**)((void**)& _tmp97D->f1);}}_LL3E0: _tmp982=_tmp97F;goto _LL3E2;
_LL3E1: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp980=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp940;if(_tmp980->tag != 18)goto _LL3E3;else{_tmp981=(void**)& _tmp980->f1;
_tmp982=(void**)((void**)& _tmp980->f1);}}_LL3E2:*_tmp982=Cyc_Toc_typ_to_c(*
_tmp982);goto _LL3B2;_LL3E3: {struct Cyc_Absyn_StmtExp_e_struct*_tmp983=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp940;if(_tmp983->tag != 38)goto _LL3E5;else{_tmp984=_tmp983->f1;}}_LL3E4: Cyc_Toc_stmttypes_to_c(
_tmp984);goto _LL3B2;_LL3E5: {struct Cyc_Absyn_Malloc_e_struct*_tmp985=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp940;if(_tmp985->tag != 35)goto _LL3E7;else{_tmp986=_tmp985->f1;_tmp987=(struct
Cyc_Absyn_MallocInfo*)& _tmp985->f1;}}_LL3E6: if(_tmp987->elt_type != 0){void**
_tmpDD7;_tmp987->elt_type=((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7[0]=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp987->elt_type))),_tmpDD7))));}Cyc_Toc_exptypes_to_c(
_tmp987->num_elts);goto _LL3B2;_LL3E7: {struct Cyc_Absyn_Const_e_struct*_tmp988=(
struct Cyc_Absyn_Const_e_struct*)_tmp940;if(_tmp988->tag != 0)goto _LL3E9;}_LL3E8:
goto _LL3EA;_LL3E9: {struct Cyc_Absyn_Var_e_struct*_tmp989=(struct Cyc_Absyn_Var_e_struct*)
_tmp940;if(_tmp989->tag != 1)goto _LL3EB;}_LL3EA: goto _LL3EC;_LL3EB: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp98A=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp940;if(_tmp98A->tag != 2)goto
_LL3ED;}_LL3EC: goto _LL3EE;_LL3ED: {struct Cyc_Absyn_Enum_e_struct*_tmp98B=(struct
Cyc_Absyn_Enum_e_struct*)_tmp940;if(_tmp98B->tag != 33)goto _LL3EF;}_LL3EE: goto
_LL3F0;_LL3EF: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp98C=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp940;if(_tmp98C->tag != 34)goto _LL3F1;}_LL3F0: goto _LL3B2;_LL3F1: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp98D=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp940;if(_tmp98D->tag != 31)goto
_LL3F3;}_LL3F2: goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Datatype_e_struct*_tmp98E=(
struct Cyc_Absyn_Datatype_e_struct*)_tmp940;if(_tmp98E->tag != 32)goto _LL3F5;}
_LL3F4: goto _LL3F6;_LL3F5: {struct Cyc_Absyn_Aggregate_e_struct*_tmp98F=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp940;if(_tmp98F->tag != 30)goto _LL3F7;}_LL3F6: goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp990=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp940;if(_tmp990->tag != 29)
goto _LL3F9;}_LL3F8: goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Tuple_e_struct*_tmp991=(
struct Cyc_Absyn_Tuple_e_struct*)_tmp940;if(_tmp991->tag != 26)goto _LL3FB;}_LL3FA:
goto _LL3FC;_LL3FB: {struct Cyc_Absyn_Instantiate_e_struct*_tmp992=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp940;if(_tmp992->tag != 14)goto _LL3FD;}_LL3FC: goto _LL3FE;_LL3FD: {struct Cyc_Absyn_New_e_struct*
_tmp993=(struct Cyc_Absyn_New_e_struct*)_tmp940;if(_tmp993->tag != 17)goto _LL3FF;}
_LL3FE: goto _LL400;_LL3FF: {struct Cyc_Absyn_Valueof_e_struct*_tmp994=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp940;if(_tmp994->tag != 40)goto _LL401;}_LL400: goto _LL402;_LL401: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp995=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp940;if(_tmp995->tag != 39)goto
_LL403;}_LL402: goto _LL404;_LL403: {struct Cyc_Absyn_Gentyp_e_struct*_tmp996=(
struct Cyc_Absyn_Gentyp_e_struct*)_tmp940;if(_tmp996->tag != 21)goto _LL3B2;}_LL404:{
const char*_tmpDDA;void*_tmpDD9;(_tmpDD9=0,Cyc_Tcutil_terr(e->loc,((_tmpDDA="Cyclone expression within C code",
_tag_dyneither(_tmpDDA,sizeof(char),33))),_tag_dyneither(_tmpDD9,sizeof(void*),0)));}
goto _LL3B2;_LL3B2:;}static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d);
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp99C=d->r;
struct Cyc_Absyn_Vardecl*_tmp99E;struct Cyc_Absyn_Fndecl*_tmp9A0;struct Cyc_Absyn_Aggrdecl*
_tmp9A2;struct Cyc_Absyn_Enumdecl*_tmp9A4;struct Cyc_Absyn_Typedefdecl*_tmp9A6;
_LL406: {struct Cyc_Absyn_Var_d_struct*_tmp99D=(struct Cyc_Absyn_Var_d_struct*)
_tmp99C;if(_tmp99D->tag != 0)goto _LL408;else{_tmp99E=_tmp99D->f1;}}_LL407: _tmp99E->type=
Cyc_Toc_typ_to_c(_tmp99E->type);if(_tmp99E->initializer != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp99E->initializer));goto _LL405;_LL408: {
struct Cyc_Absyn_Fn_d_struct*_tmp99F=(struct Cyc_Absyn_Fn_d_struct*)_tmp99C;if(
_tmp99F->tag != 1)goto _LL40A;else{_tmp9A0=_tmp99F->f1;}}_LL409: _tmp9A0->ret_type=
Cyc_Toc_typ_to_c(_tmp9A0->ret_type);{struct Cyc_List_List*_tmp9B2=_tmp9A0->args;
for(0;_tmp9B2 != 0;_tmp9B2=_tmp9B2->tl){(*((struct _tuple25*)_tmp9B2->hd)).f3=Cyc_Toc_typ_to_c((*((
struct _tuple25*)_tmp9B2->hd)).f3);}}goto _LL405;_LL40A: {struct Cyc_Absyn_Aggr_d_struct*
_tmp9A1=(struct Cyc_Absyn_Aggr_d_struct*)_tmp99C;if(_tmp9A1->tag != 6)goto _LL40C;
else{_tmp9A2=_tmp9A1->f1;}}_LL40B: Cyc_Toc_aggrdecl_to_c(_tmp9A2);goto _LL405;
_LL40C: {struct Cyc_Absyn_Enum_d_struct*_tmp9A3=(struct Cyc_Absyn_Enum_d_struct*)
_tmp99C;if(_tmp9A3->tag != 8)goto _LL40E;else{_tmp9A4=_tmp9A3->f1;}}_LL40D: if(
_tmp9A4->fields != 0){struct Cyc_List_List*_tmp9B3=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp9A4->fields))->v;for(0;_tmp9B3 != 0;_tmp9B3=_tmp9B3->tl){
struct Cyc_Absyn_Enumfield*_tmp9B4=(struct Cyc_Absyn_Enumfield*)_tmp9B3->hd;if(
_tmp9B4->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9B4->tag));}}
goto _LL405;_LL40E: {struct Cyc_Absyn_Typedef_d_struct*_tmp9A5=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp99C;if(_tmp9A5->tag != 9)goto _LL410;else{_tmp9A6=_tmp9A5->f1;}}_LL40F:{struct
Cyc_Core_Opt*_tmpDDB;_tmp9A6->defn=((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((
_tmpDDB->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp9A6->defn))->v),_tmpDDB))));}goto _LL405;_LL410: {struct Cyc_Absyn_Let_d_struct*
_tmp9A7=(struct Cyc_Absyn_Let_d_struct*)_tmp99C;if(_tmp9A7->tag != 2)goto _LL412;}
_LL411: goto _LL413;_LL412: {struct Cyc_Absyn_Letv_d_struct*_tmp9A8=(struct Cyc_Absyn_Letv_d_struct*)
_tmp99C;if(_tmp9A8->tag != 3)goto _LL414;}_LL413: goto _LL415;_LL414: {struct Cyc_Absyn_Datatype_d_struct*
_tmp9A9=(struct Cyc_Absyn_Datatype_d_struct*)_tmp99C;if(_tmp9A9->tag != 7)goto
_LL416;}_LL415: goto _LL417;_LL416: {struct Cyc_Absyn_Namespace_d_struct*_tmp9AA=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp99C;if(_tmp9AA->tag != 10)goto _LL418;}
_LL417: goto _LL419;_LL418: {struct Cyc_Absyn_Using_d_struct*_tmp9AB=(struct Cyc_Absyn_Using_d_struct*)
_tmp99C;if(_tmp9AB->tag != 11)goto _LL41A;}_LL419: goto _LL41B;_LL41A: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9AC=(struct Cyc_Absyn_ExternC_d_struct*)_tmp99C;if(_tmp9AC->tag != 12)goto
_LL41C;}_LL41B: goto _LL41D;_LL41C: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp9AD=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp99C;if(_tmp9AD->tag != 13)
goto _LL41E;}_LL41D: goto _LL41F;_LL41E: {struct Cyc_Absyn_Region_d_struct*_tmp9AE=(
struct Cyc_Absyn_Region_d_struct*)_tmp99C;if(_tmp9AE->tag != 4)goto _LL420;}_LL41F:
goto _LL421;_LL420: {struct Cyc_Absyn_Alias_d_struct*_tmp9AF=(struct Cyc_Absyn_Alias_d_struct*)
_tmp99C;if(_tmp9AF->tag != 5)goto _LL422;}_LL421:{const char*_tmpDDE;void*_tmpDDD;(
_tmpDDD=0,Cyc_Tcutil_terr(d->loc,((_tmpDDE="Cyclone declaration within C code",
_tag_dyneither(_tmpDDE,sizeof(char),34))),_tag_dyneither(_tmpDDD,sizeof(void*),0)));}
goto _LL405;_LL422: {struct Cyc_Absyn_Porton_d_struct*_tmp9B0=(struct Cyc_Absyn_Porton_d_struct*)
_tmp99C;if(_tmp9B0->tag != 14)goto _LL424;}_LL423: goto _LL425;_LL424: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9B1=(struct Cyc_Absyn_Portoff_d_struct*)_tmp99C;if(_tmp9B1->tag != 15)goto
_LL405;}_LL425: goto _LL405;_LL405:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){void*_tmp9B8=s->r;
struct Cyc_Absyn_Exp*_tmp9BA;struct Cyc_Absyn_Stmt*_tmp9BC;struct Cyc_Absyn_Stmt*
_tmp9BD;struct Cyc_Absyn_Exp*_tmp9BF;struct Cyc_Absyn_Exp*_tmp9C1;struct Cyc_Absyn_Stmt*
_tmp9C2;struct Cyc_Absyn_Stmt*_tmp9C3;struct _tuple9 _tmp9C5;struct Cyc_Absyn_Exp*
_tmp9C6;struct Cyc_Absyn_Stmt*_tmp9C7;struct Cyc_Absyn_Exp*_tmp9C9;struct _tuple9
_tmp9CA;struct Cyc_Absyn_Exp*_tmp9CB;struct _tuple9 _tmp9CC;struct Cyc_Absyn_Exp*
_tmp9CD;struct Cyc_Absyn_Stmt*_tmp9CE;struct Cyc_Absyn_Exp*_tmp9D0;struct Cyc_List_List*
_tmp9D1;struct Cyc_Absyn_Decl*_tmp9D3;struct Cyc_Absyn_Stmt*_tmp9D4;struct Cyc_Absyn_Stmt*
_tmp9D6;struct _tuple9 _tmp9D7;struct Cyc_Absyn_Exp*_tmp9D8;_LL427: {struct Cyc_Absyn_Exp_s_struct*
_tmp9B9=(struct Cyc_Absyn_Exp_s_struct*)_tmp9B8;if(_tmp9B9->tag != 1)goto _LL429;
else{_tmp9BA=_tmp9B9->f1;}}_LL428: Cyc_Toc_exptypes_to_c(_tmp9BA);goto _LL426;
_LL429: {struct Cyc_Absyn_Seq_s_struct*_tmp9BB=(struct Cyc_Absyn_Seq_s_struct*)
_tmp9B8;if(_tmp9BB->tag != 2)goto _LL42B;else{_tmp9BC=_tmp9BB->f1;_tmp9BD=_tmp9BB->f2;}}
_LL42A: Cyc_Toc_stmttypes_to_c(_tmp9BC);Cyc_Toc_stmttypes_to_c(_tmp9BD);goto
_LL426;_LL42B: {struct Cyc_Absyn_Return_s_struct*_tmp9BE=(struct Cyc_Absyn_Return_s_struct*)
_tmp9B8;if(_tmp9BE->tag != 3)goto _LL42D;else{_tmp9BF=_tmp9BE->f1;}}_LL42C: if(
_tmp9BF != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_tmp9BF);goto _LL426;
_LL42D: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp9C0=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp9B8;if(_tmp9C0->tag != 4)goto _LL42F;else{_tmp9C1=_tmp9C0->f1;_tmp9C2=_tmp9C0->f2;
_tmp9C3=_tmp9C0->f3;}}_LL42E: Cyc_Toc_exptypes_to_c(_tmp9C1);Cyc_Toc_stmttypes_to_c(
_tmp9C2);Cyc_Toc_stmttypes_to_c(_tmp9C3);goto _LL426;_LL42F: {struct Cyc_Absyn_While_s_struct*
_tmp9C4=(struct Cyc_Absyn_While_s_struct*)_tmp9B8;if(_tmp9C4->tag != 5)goto _LL431;
else{_tmp9C5=_tmp9C4->f1;_tmp9C6=_tmp9C5.f1;_tmp9C7=_tmp9C4->f2;}}_LL430: Cyc_Toc_exptypes_to_c(
_tmp9C6);Cyc_Toc_stmttypes_to_c(_tmp9C7);goto _LL426;_LL431: {struct Cyc_Absyn_For_s_struct*
_tmp9C8=(struct Cyc_Absyn_For_s_struct*)_tmp9B8;if(_tmp9C8->tag != 9)goto _LL433;
else{_tmp9C9=_tmp9C8->f1;_tmp9CA=_tmp9C8->f2;_tmp9CB=_tmp9CA.f1;_tmp9CC=_tmp9C8->f3;
_tmp9CD=_tmp9CC.f1;_tmp9CE=_tmp9C8->f4;}}_LL432: Cyc_Toc_exptypes_to_c(_tmp9C9);
Cyc_Toc_exptypes_to_c(_tmp9CB);Cyc_Toc_exptypes_to_c(_tmp9CD);Cyc_Toc_stmttypes_to_c(
_tmp9CE);goto _LL426;_LL433: {struct Cyc_Absyn_Switch_s_struct*_tmp9CF=(struct Cyc_Absyn_Switch_s_struct*)
_tmp9B8;if(_tmp9CF->tag != 10)goto _LL435;else{_tmp9D0=_tmp9CF->f1;_tmp9D1=_tmp9CF->f2;}}
_LL434: Cyc_Toc_exptypes_to_c(_tmp9D0);for(0;_tmp9D1 != 0;_tmp9D1=_tmp9D1->tl){Cyc_Toc_stmttypes_to_c(((
struct Cyc_Absyn_Switch_clause*)_tmp9D1->hd)->body);}goto _LL426;_LL435: {struct
Cyc_Absyn_Decl_s_struct*_tmp9D2=(struct Cyc_Absyn_Decl_s_struct*)_tmp9B8;if(
_tmp9D2->tag != 12)goto _LL437;else{_tmp9D3=_tmp9D2->f1;_tmp9D4=_tmp9D2->f2;}}
_LL436: Cyc_Toc_decltypes_to_c(_tmp9D3);Cyc_Toc_stmttypes_to_c(_tmp9D4);goto
_LL426;_LL437: {struct Cyc_Absyn_Do_s_struct*_tmp9D5=(struct Cyc_Absyn_Do_s_struct*)
_tmp9B8;if(_tmp9D5->tag != 14)goto _LL439;else{_tmp9D6=_tmp9D5->f1;_tmp9D7=_tmp9D5->f2;
_tmp9D8=_tmp9D7.f1;}}_LL438: Cyc_Toc_stmttypes_to_c(_tmp9D6);Cyc_Toc_exptypes_to_c(
_tmp9D8);goto _LL426;_LL439: {struct Cyc_Absyn_Skip_s_struct*_tmp9D9=(struct Cyc_Absyn_Skip_s_struct*)
_tmp9B8;if(_tmp9D9->tag != 0)goto _LL43B;}_LL43A: goto _LL43C;_LL43B: {struct Cyc_Absyn_Break_s_struct*
_tmp9DA=(struct Cyc_Absyn_Break_s_struct*)_tmp9B8;if(_tmp9DA->tag != 6)goto _LL43D;}
_LL43C: goto _LL43E;_LL43D: {struct Cyc_Absyn_Continue_s_struct*_tmp9DB=(struct Cyc_Absyn_Continue_s_struct*)
_tmp9B8;if(_tmp9DB->tag != 7)goto _LL43F;}_LL43E: goto _LL440;_LL43F: {struct Cyc_Absyn_Goto_s_struct*
_tmp9DC=(struct Cyc_Absyn_Goto_s_struct*)_tmp9B8;if(_tmp9DC->tag != 8)goto _LL441;}
_LL440: goto _LL426;_LL441: {struct Cyc_Absyn_Fallthru_s_struct*_tmp9DD=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp9B8;if(_tmp9DD->tag != 11)goto _LL443;}_LL442: goto _LL444;_LL443: {struct Cyc_Absyn_Label_s_struct*
_tmp9DE=(struct Cyc_Absyn_Label_s_struct*)_tmp9B8;if(_tmp9DE->tag != 13)goto _LL445;}
_LL444: goto _LL446;_LL445: {struct Cyc_Absyn_TryCatch_s_struct*_tmp9DF=(struct Cyc_Absyn_TryCatch_s_struct*)
_tmp9B8;if(_tmp9DF->tag != 15)goto _LL447;}_LL446: goto _LL448;_LL447: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp9E0=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp9B8;if(_tmp9E0->tag != 16)goto
_LL426;}_LL448:{const char*_tmpDE1;void*_tmpDE0;(_tmpDE0=0,Cyc_Tcutil_terr(s->loc,((
_tmpDE1="Cyclone statement in C code",_tag_dyneither(_tmpDE1,sizeof(char),28))),
_tag_dyneither(_tmpDE0,sizeof(void*),0)));}goto _LL426;_LL426:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude);static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){const char*_tmpDE4;void*
_tmpDE3;(_tmpDE3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpDE4="decls_to_c: not at toplevel!",_tag_dyneither(_tmpDE4,sizeof(char),29))),
_tag_dyneither(_tmpDE3,sizeof(void*),0)));}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp9E5=d->r;struct Cyc_Absyn_Vardecl*_tmp9E7;struct Cyc_Absyn_Fndecl*
_tmp9E9;struct Cyc_Absyn_Aggrdecl*_tmp9EF;struct Cyc_Absyn_Datatypedecl*_tmp9F1;
struct Cyc_Absyn_Enumdecl*_tmp9F3;struct Cyc_Absyn_Typedefdecl*_tmp9F5;struct Cyc_List_List*
_tmp9F9;struct Cyc_List_List*_tmp9FB;struct Cyc_List_List*_tmp9FD;struct Cyc_List_List*
_tmp9FF;_LL44A: {struct Cyc_Absyn_Var_d_struct*_tmp9E6=(struct Cyc_Absyn_Var_d_struct*)
_tmp9E5;if(_tmp9E6->tag != 0)goto _LL44C;else{_tmp9E7=_tmp9E6->f1;}}_LL44B: {
struct _tuple1*_tmpA00=_tmp9E7->name;if(_tmp9E7->sc == Cyc_Absyn_ExternC){struct
_tuple1*_tmpDE5;_tmpA00=((_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->f1=Cyc_Absyn_Rel_n(
0),((_tmpDE5->f2=(*_tmpA00).f2,_tmpDE5))))));}if(_tmp9E7->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9E7->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9E7->initializer));}}{
struct Cyc_Absyn_Global_b_struct _tmpDE8;struct Cyc_Absyn_Global_b_struct*_tmpDE7;
nv=Cyc_Toc_add_varmap(r,nv,_tmp9E7->name,Cyc_Absyn_varb_exp(_tmpA00,(void*)((
_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7[0]=((_tmpDE8.tag=1,((_tmpDE8.f1=
_tmp9E7,_tmpDE8)))),_tmpDE7)))),0));}_tmp9E7->name=_tmpA00;_tmp9E7->sc=Cyc_Toc_scope_to_c(
_tmp9E7->sc);_tmp9E7->type=Cyc_Toc_typ_to_c_array(_tmp9E7->type);{struct Cyc_List_List*
_tmpDE9;Cyc_Toc_result_decls=((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9->hd=
d,((_tmpDE9->tl=Cyc_Toc_result_decls,_tmpDE9))))));}goto _LL449;}_LL44C: {struct
Cyc_Absyn_Fn_d_struct*_tmp9E8=(struct Cyc_Absyn_Fn_d_struct*)_tmp9E5;if(_tmp9E8->tag
!= 1)goto _LL44E;else{_tmp9E9=_tmp9E8->f1;}}_LL44D: {struct _tuple1*_tmpA05=
_tmp9E9->name;if(_tmp9E9->sc == Cyc_Absyn_ExternC){{struct _tuple1*_tmpDEA;_tmpA05=((
_tmpDEA=_cycalloc(sizeof(*_tmpDEA)),((_tmpDEA->f1=Cyc_Absyn_Rel_n(0),((_tmpDEA->f2=(*
_tmpA05).f2,_tmpDEA))))));}_tmp9E9->sc=Cyc_Absyn_Public;}nv=Cyc_Toc_add_varmap(r,
nv,_tmp9E9->name,Cyc_Absyn_var_exp(_tmpA05,0));_tmp9E9->name=_tmpA05;Cyc_Toc_fndecl_to_c(
nv,_tmp9E9,cinclude);{struct Cyc_List_List*_tmpDEB;Cyc_Toc_result_decls=((_tmpDEB=
_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->hd=d,((_tmpDEB->tl=Cyc_Toc_result_decls,
_tmpDEB))))));}goto _LL449;}_LL44E: {struct Cyc_Absyn_Let_d_struct*_tmp9EA=(struct
Cyc_Absyn_Let_d_struct*)_tmp9E5;if(_tmp9EA->tag != 2)goto _LL450;}_LL44F: goto
_LL451;_LL450: {struct Cyc_Absyn_Letv_d_struct*_tmp9EB=(struct Cyc_Absyn_Letv_d_struct*)
_tmp9E5;if(_tmp9EB->tag != 3)goto _LL452;}_LL451: {const char*_tmpDEE;void*_tmpDED;(
_tmpDED=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDEE="letdecl at toplevel",_tag_dyneither(_tmpDEE,sizeof(char),20))),
_tag_dyneither(_tmpDED,sizeof(void*),0)));}_LL452: {struct Cyc_Absyn_Region_d_struct*
_tmp9EC=(struct Cyc_Absyn_Region_d_struct*)_tmp9E5;if(_tmp9EC->tag != 4)goto _LL454;}
_LL453: {const char*_tmpDF1;void*_tmpDF0;(_tmpDF0=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDF1="region decl at toplevel",
_tag_dyneither(_tmpDF1,sizeof(char),24))),_tag_dyneither(_tmpDF0,sizeof(void*),0)));}
_LL454: {struct Cyc_Absyn_Alias_d_struct*_tmp9ED=(struct Cyc_Absyn_Alias_d_struct*)
_tmp9E5;if(_tmp9ED->tag != 5)goto _LL456;}_LL455: {const char*_tmpDF4;void*_tmpDF3;(
_tmpDF3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((
_tmpDF4="alias decl at toplevel",_tag_dyneither(_tmpDF4,sizeof(char),23))),
_tag_dyneither(_tmpDF3,sizeof(void*),0)));}_LL456: {struct Cyc_Absyn_Aggr_d_struct*
_tmp9EE=(struct Cyc_Absyn_Aggr_d_struct*)_tmp9E5;if(_tmp9EE->tag != 6)goto _LL458;
else{_tmp9EF=_tmp9EE->f1;}}_LL457: Cyc_Toc_aggrdecl_to_c(_tmp9EF);goto _LL449;
_LL458: {struct Cyc_Absyn_Datatype_d_struct*_tmp9F0=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp9E5;if(_tmp9F0->tag != 7)goto _LL45A;else{_tmp9F1=_tmp9F0->f1;}}_LL459: if(
_tmp9F1->is_extensible)Cyc_Toc_xdatatypedecl_to_c(_tmp9F1);else{Cyc_Toc_datatypedecl_to_c(
_tmp9F1);}goto _LL449;_LL45A: {struct Cyc_Absyn_Enum_d_struct*_tmp9F2=(struct Cyc_Absyn_Enum_d_struct*)
_tmp9E5;if(_tmp9F2->tag != 8)goto _LL45C;else{_tmp9F3=_tmp9F2->f1;}}_LL45B: Cyc_Toc_enumdecl_to_c(
nv,_tmp9F3);{struct Cyc_List_List*_tmpDF5;Cyc_Toc_result_decls=((_tmpDF5=
_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->hd=d,((_tmpDF5->tl=Cyc_Toc_result_decls,
_tmpDF5))))));}goto _LL449;_LL45C: {struct Cyc_Absyn_Typedef_d_struct*_tmp9F4=(
struct Cyc_Absyn_Typedef_d_struct*)_tmp9E5;if(_tmp9F4->tag != 9)goto _LL45E;else{
_tmp9F5=_tmp9F4->f1;}}_LL45D: _tmp9F5->name=_tmp9F5->name;_tmp9F5->tvs=0;if(
_tmp9F5->defn != 0){struct Cyc_Core_Opt*_tmpDF6;_tmp9F5->defn=((_tmpDF6=_cycalloc(
sizeof(*_tmpDF6)),((_tmpDF6->v=Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp9F5->defn))->v),_tmpDF6))));}else{switch((enum Cyc_Absyn_Kind)((
struct Cyc_Core_Opt*)_check_null(_tmp9F5->kind))->v){case Cyc_Absyn_BoxKind: _LL46A:{
struct Cyc_Core_Opt*_tmpDF7;_tmp9F5->defn=((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((
_tmpDF7->v=Cyc_Absyn_void_star_typ(),_tmpDF7))));}break;default: _LL46B:{struct
Cyc_Core_Opt*_tmpDF8;_tmp9F5->defn=((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((
_tmpDF8->v=(void*)& Cyc_Absyn_VoidType_val,_tmpDF8))));}break;}}{struct Cyc_List_List*
_tmpDF9;Cyc_Toc_result_decls=((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->hd=
d,((_tmpDF9->tl=Cyc_Toc_result_decls,_tmpDF9))))));}goto _LL449;_LL45E: {struct
Cyc_Absyn_Porton_d_struct*_tmp9F6=(struct Cyc_Absyn_Porton_d_struct*)_tmp9E5;if(
_tmp9F6->tag != 14)goto _LL460;}_LL45F: goto _LL461;_LL460: {struct Cyc_Absyn_Portoff_d_struct*
_tmp9F7=(struct Cyc_Absyn_Portoff_d_struct*)_tmp9E5;if(_tmp9F7->tag != 15)goto
_LL462;}_LL461: goto _LL449;_LL462: {struct Cyc_Absyn_Namespace_d_struct*_tmp9F8=(
struct Cyc_Absyn_Namespace_d_struct*)_tmp9E5;if(_tmp9F8->tag != 10)goto _LL464;
else{_tmp9F9=_tmp9F8->f2;}}_LL463: _tmp9FB=_tmp9F9;goto _LL465;_LL464: {struct Cyc_Absyn_Using_d_struct*
_tmp9FA=(struct Cyc_Absyn_Using_d_struct*)_tmp9E5;if(_tmp9FA->tag != 11)goto _LL466;
else{_tmp9FB=_tmp9FA->f2;}}_LL465: _tmp9FD=_tmp9FB;goto _LL467;_LL466: {struct Cyc_Absyn_ExternC_d_struct*
_tmp9FC=(struct Cyc_Absyn_ExternC_d_struct*)_tmp9E5;if(_tmp9FC->tag != 12)goto
_LL468;else{_tmp9FD=_tmp9FC->f1;}}_LL467: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FD,top,
cinclude);goto _LL449;_LL468: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp9FE=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp9E5;if(_tmp9FE->tag != 13)goto _LL449;
else{_tmp9FF=_tmp9FE->f1;}}_LL469: nv=Cyc_Toc_decls_to_c(r,nv,_tmp9FF,top,1);goto
_LL449;_LL449:;};}return nv;}static void Cyc_Toc_init();static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmpA14;struct Cyc_Core_NewRegion _tmpA13=Cyc_Core_new_dynregion();
_tmpA14=_tmpA13.dynregion;{struct _DynRegionFrame _tmpA15;struct _RegionHandle*d=
_open_dynregion(& _tmpA15,_tmpA14);{struct Cyc_Dict_Dict*_tmpE04;struct Cyc_Dict_Dict*
_tmpE03;struct Cyc_Set_Set**_tmpE02;struct Cyc_Dict_Dict*_tmpE01;struct Cyc_List_List**
_tmpE00;struct Cyc_Toc_TocState*_tmpDFF;Cyc_Toc_toc_state=((_tmpDFF=_cycalloc(
sizeof(*_tmpDFF)),((_tmpDFF->dyn=(struct _DynRegionHandle*)_tmpA14,((_tmpDFF->tuple_types=(
struct Cyc_List_List**)((_tmpE00=_region_malloc(d,sizeof(*_tmpE00)),((_tmpE00[0]=
0,_tmpE00)))),((_tmpDFF->aggrs_so_far=(struct Cyc_Dict_Dict*)((_tmpE01=
_region_malloc(d,sizeof(*_tmpE01)),((_tmpE01[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE01)))),((_tmpDFF->datatypes_so_far=(struct Cyc_Set_Set**)((_tmpE02=
_region_malloc(d,sizeof(*_tmpE02)),((_tmpE02[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE02)))),((_tmpDFF->xdatatypes_so_far=(struct Cyc_Dict_Dict*)((_tmpE03=
_region_malloc(d,sizeof(*_tmpE03)),((_tmpE03[0]=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),
_tmpE03)))),((_tmpDFF->qvar_tags=(struct Cyc_Dict_Dict*)((_tmpE04=_region_malloc(
d,sizeof(*_tmpE04)),((_tmpE04[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,
int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),
_tmpE04)))),((_tmpDFF->temp_labels=(struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(
struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpDFF))))))))))))))));}Cyc_Toc_result_decls=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;{struct
_dyneither_ptr**_tmpE05;Cyc_Toc_globals=_tag_dyneither(((_tmpE05=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38),((_tmpE05[0]=& Cyc_Toc__throw_str,((_tmpE05[1]=&
Cyc_Toc_setjmp_str,((_tmpE05[2]=& Cyc_Toc__push_handler_str,((_tmpE05[3]=& Cyc_Toc__pop_handler_str,((
_tmpE05[4]=& Cyc_Toc__exn_thrown_str,((_tmpE05[5]=& Cyc_Toc__npop_handler_str,((
_tmpE05[6]=& Cyc_Toc__check_null_str,((_tmpE05[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpE05[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE05[9]=& Cyc_Toc__check_dyneither_subscript_str,((
_tmpE05[10]=& Cyc_Toc__dyneither_ptr_str,((_tmpE05[11]=& Cyc_Toc__tag_dyneither_str,((
_tmpE05[12]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpE05[13]=& Cyc_Toc__untag_dyneither_ptr_str,((
_tmpE05[14]=& Cyc_Toc__get_dyneither_size_str,((_tmpE05[15]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpE05[16]=& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE05[17]=& Cyc_Toc__zero_arr_plus_str,((
_tmpE05[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE05[19]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpE05[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE05[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((
_tmpE05[22]=& Cyc_Toc__cycalloc_str,((_tmpE05[23]=& Cyc_Toc__cyccalloc_str,((
_tmpE05[24]=& Cyc_Toc__cycalloc_atomic_str,((_tmpE05[25]=& Cyc_Toc__cyccalloc_atomic_str,((
_tmpE05[26]=& Cyc_Toc__region_malloc_str,((_tmpE05[27]=& Cyc_Toc__region_calloc_str,((
_tmpE05[28]=& Cyc_Toc__check_times_str,((_tmpE05[29]=& Cyc_Toc__new_region_str,((
_tmpE05[30]=& Cyc_Toc__push_region_str,((_tmpE05[31]=& Cyc_Toc__pop_region_str,((
_tmpE05[32]=& Cyc_Toc__open_dynregion_str,((_tmpE05[33]=& Cyc_Toc__push_dynregion_str,((
_tmpE05[34]=& Cyc_Toc__pop_dynregion_str,((_tmpE05[35]=& Cyc_Toc__reset_region_str,((
_tmpE05[36]=& Cyc_Toc__throw_arraybounds_str,((_tmpE05[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str,
_tmpE05)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dyneither_ptr*),38);};;_pop_dynregion(d);};}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds){Cyc_Toc_init();{struct _RegionHandle _tmpA1D=_new_region("start");struct
_RegionHandle*start=& _tmpA1D;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmpA1F;struct Cyc_Toc_TocState _tmpA1E=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpA1F=_tmpA1E.dyn;Cyc_Core_free_dynregion(
_tmpA1F);};{struct Cyc_List_List*_tmpA20=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA20;};;
_pop_region(start);};}
