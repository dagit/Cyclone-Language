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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_rcopy(
struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];
struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*
tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
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
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];
struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{
char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,
void*);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
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
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);extern struct _tuple0*Cyc_Absyn_exn_name;void*Cyc_Absyn_cstar_typ(
void*t,struct Cyc_Absyn_Tqual tq);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,
struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*
Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);extern char
Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Iter_Iter Cyc_Set_make_iter(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_compress(
void*t);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);int Cyc_Tovc_elim_array_initializers=
1;struct Cyc_Tovc_BoxingEnv{struct _RegionHandle*rgn;struct Cyc_List_List*
all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_List_List*boundvars;struct Cyc_Set_Set**
freevars;};struct Cyc_Tovc_ToExpEnv{struct _RegionHandle*rgn;struct Cyc_List_List*
all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**
gen_ds;};struct _tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static
void Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e);
static void Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*
e){while(1){void*_tmp0=e->r;struct _tuple0*_tmp2;struct _tuple0 _tmp3;union Cyc_Absyn_Nmspace
_tmp4;int _tmp5;struct _dyneither_ptr*_tmp6;struct Cyc_Absyn_Exp*_tmpA;struct Cyc_List_List*
_tmpB;struct Cyc_List_List*_tmpD;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*
_tmp10;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*
_tmp14;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*
_tmp19;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*
_tmp1D;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*
_tmp22;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*
_tmp28;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Exp*_tmp2C;struct Cyc_Absyn_Exp*
_tmp2E;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_List_List*
_tmp38;_LL1: {struct Cyc_Absyn_Var_e_struct*_tmp1=(struct Cyc_Absyn_Var_e_struct*)
_tmp0;if(_tmp1->tag != 1)goto _LL3;else{_tmp2=_tmp1->f1;_tmp3=*_tmp2;_tmp4=_tmp3.f1;
if((_tmp4.Loc_n).tag != 3)goto _LL3;_tmp5=(int)(_tmp4.Loc_n).val;_tmp6=_tmp3.f2;}}
_LL2: if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(
env.varmap,_tmp6)){e->r=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct
_dyneither_ptr*k))Cyc_Dict_lookup)(env.varmap,_tmp6))->r;continue;}{int i=0;for(0;
i < _get_dyneither_size(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*));++ i){if(
Cyc_strptrcmp(_tmp6,*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Toc_globals,
sizeof(struct _dyneither_ptr*),i)))== 0)return;}}for(0;env.boundvars != 0;env.boundvars=(
env.boundvars)->tl){if(Cyc_strptrcmp(_tmp6,(*((struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)
== 0)return;}for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
struct Cyc_Absyn_Vardecl*_tmp39=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;
if(Cyc_strptrcmp(_tmp6,(*_tmp39->name).f2)== 0){{void*_tmp3A=_tmp39->type;_LL36: {
struct Cyc_Absyn_ArrayType_struct*_tmp3B=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3A;if(_tmp3B->tag != 9)goto _LL38;}_LL37: goto _LL35;_LL38:;_LL39: e->r=(Cyc_Absyn_deref_exp(
Cyc_Absyn_var_exp(_tmp39->name,0),0))->r;goto _LL35;_LL35:;}*env.freevars=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Absyn_Vardecl*
elt))Cyc_Set_rinsert)(env.rgn,*env.freevars,_tmp39);return;}}for(0;env.all_locals
!= 0;env.all_locals=(env.all_locals)->tl){if(Cyc_strptrcmp(_tmp6,(*((struct Cyc_Absyn_Vardecl*)(
env.all_locals)->hd)->name).f2)== 0){struct Cyc_Core_Impossible_struct _tmp182;
const char*_tmp181;void*_tmp180[1];struct Cyc_String_pa_struct _tmp17F;struct Cyc_Core_Impossible_struct*
_tmp17E;(int)_throw((void*)((_tmp17E=_cycalloc(sizeof(*_tmp17E)),((_tmp17E[0]=((
_tmp182.tag=Cyc_Core_Impossible,((_tmp182.f1=(struct _dyneither_ptr)((_tmp17F.tag=
0,((_tmp17F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6),((_tmp180[0]=&
_tmp17F,Cyc_aprintf(((_tmp181="unbound variable %s found in box_free_vars_exp",
_tag_dyneither(_tmp181,sizeof(char),47))),_tag_dyneither(_tmp180,sizeof(void*),1)))))))),
_tmp182)))),_tmp17E)))));}}return;_LL3: {struct Cyc_Absyn_Var_e_struct*_tmp7=(
struct Cyc_Absyn_Var_e_struct*)_tmp0;if(_tmp7->tag != 1)goto _LL5;}_LL4: return;_LL5: {
struct Cyc_Absyn_Const_e_struct*_tmp8=(struct Cyc_Absyn_Const_e_struct*)_tmp0;if(
_tmp8->tag != 0)goto _LL7;}_LL6: return;_LL7: {struct Cyc_Absyn_FnCall_e_struct*
_tmp9=(struct Cyc_Absyn_FnCall_e_struct*)_tmp0;if(_tmp9->tag != 11)goto _LL9;else{
_tmpA=_tmp9->f1;_tmpB=_tmp9->f2;}}_LL8: Cyc_Tovc_box_free_vars_exp(env,_tmpA);
_tmpD=_tmpB;goto _LLA;_LL9: {struct Cyc_Absyn_Primop_e_struct*_tmpC=(struct Cyc_Absyn_Primop_e_struct*)
_tmp0;if(_tmpC->tag != 3)goto _LLB;else{_tmpD=_tmpC->f2;}}_LLA: for(0;_tmpD != 0;
_tmpD=_tmpD->tl){Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_tmpD->hd);}
return;_LLB: {struct Cyc_Absyn_Conditional_e_struct*_tmpE=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp0;if(_tmpE->tag != 6)goto _LLD;else{_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;_tmp11=
_tmpE->f3;}}_LLC: Cyc_Tovc_box_free_vars_exp(env,_tmp11);_tmp13=_tmpF;_tmp14=
_tmp10;goto _LLE;_LLD: {struct Cyc_Absyn_And_e_struct*_tmp12=(struct Cyc_Absyn_And_e_struct*)
_tmp0;if(_tmp12->tag != 7)goto _LLF;else{_tmp13=_tmp12->f1;_tmp14=_tmp12->f2;}}
_LLE: _tmp16=_tmp13;_tmp17=_tmp14;goto _LL10;_LLF: {struct Cyc_Absyn_Or_e_struct*
_tmp15=(struct Cyc_Absyn_Or_e_struct*)_tmp0;if(_tmp15->tag != 8)goto _LL11;else{
_tmp16=_tmp15->f1;_tmp17=_tmp15->f2;}}_LL10: _tmp19=_tmp16;_tmp1A=_tmp17;goto
_LL12;_LL11: {struct Cyc_Absyn_Subscript_e_struct*_tmp18=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp0;if(_tmp18->tag != 25)goto _LL13;else{_tmp19=_tmp18->f1;_tmp1A=_tmp18->f2;}}
_LL12: _tmp1C=_tmp19;_tmp1D=_tmp1A;goto _LL14;_LL13: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp1B=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp0;if(_tmp1B->tag != 9)goto _LL15;
else{_tmp1C=_tmp1B->f1;_tmp1D=_tmp1B->f2;}}_LL14: _tmp1F=_tmp1C;_tmp20=_tmp1D;
goto _LL16;_LL15: {struct Cyc_Absyn_AssignOp_e_struct*_tmp1E=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp0;if(_tmp1E->tag != 4)goto _LL17;else{_tmp1F=_tmp1E->f1;_tmp20=_tmp1E->f3;}}
_LL16: Cyc_Tovc_box_free_vars_exp(env,_tmp20);_tmp22=_tmp1F;goto _LL18;_LL17: {
struct Cyc_Absyn_NoInstantiate_e_struct*_tmp21=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp0;if(_tmp21->tag != 13)goto _LL19;else{_tmp22=_tmp21->f1;}}_LL18: _tmp24=_tmp22;
goto _LL1A;_LL19: {struct Cyc_Absyn_Instantiate_e_struct*_tmp23=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp0;if(_tmp23->tag != 14)goto _LL1B;else{_tmp24=_tmp23->f1;}}_LL1A: _tmp26=_tmp24;
goto _LL1C;_LL1B: {struct Cyc_Absyn_Cast_e_struct*_tmp25=(struct Cyc_Absyn_Cast_e_struct*)
_tmp0;if(_tmp25->tag != 15)goto _LL1D;else{_tmp26=_tmp25->f2;}}_LL1C: _tmp28=_tmp26;
goto _LL1E;_LL1D: {struct Cyc_Absyn_Address_e_struct*_tmp27=(struct Cyc_Absyn_Address_e_struct*)
_tmp0;if(_tmp27->tag != 16)goto _LL1F;else{_tmp28=_tmp27->f1;}}_LL1E: _tmp2A=_tmp28;
goto _LL20;_LL1F: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp29=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp0;if(_tmp29->tag != 19)goto _LL21;else{_tmp2A=_tmp29->f1;}}_LL20: _tmp2C=_tmp2A;
goto _LL22;_LL21: {struct Cyc_Absyn_Deref_e_struct*_tmp2B=(struct Cyc_Absyn_Deref_e_struct*)
_tmp0;if(_tmp2B->tag != 22)goto _LL23;else{_tmp2C=_tmp2B->f1;}}_LL22: _tmp2E=_tmp2C;
goto _LL24;_LL23: {struct Cyc_Absyn_AggrMember_e_struct*_tmp2D=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp0;if(_tmp2D->tag != 23)goto _LL25;else{_tmp2E=_tmp2D->f1;}}_LL24: _tmp30=_tmp2E;
goto _LL26;_LL25: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp2F=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp0;if(_tmp2F->tag != 24)goto _LL27;else{_tmp30=_tmp2F->f1;}}_LL26: _tmp32=_tmp30;
goto _LL28;_LL27: {struct Cyc_Absyn_Increment_e_struct*_tmp31=(struct Cyc_Absyn_Increment_e_struct*)
_tmp0;if(_tmp31->tag != 5)goto _LL29;else{_tmp32=_tmp31->f1;}}_LL28: Cyc_Tovc_box_free_vars_exp(
env,_tmp32);return;_LL29: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp33=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp0;if(_tmp33->tag != 18)goto _LL2B;}_LL2A: goto _LL2C;_LL2B: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp34=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp0;if(_tmp34->tag != 20)goto _LL2D;}
_LL2C: goto _LL2E;_LL2D: {struct Cyc_Absyn_Gentyp_e_struct*_tmp35=(struct Cyc_Absyn_Gentyp_e_struct*)
_tmp0;if(_tmp35->tag != 21)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_Enum_e_struct*
_tmp36=(struct Cyc_Absyn_Enum_e_struct*)_tmp0;if(_tmp36->tag != 33)goto _LL31;}
_LL30: return;_LL31: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp37=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp0;if(_tmp37->tag != 37)goto _LL33;else{_tmp38=_tmp37->f2;}}_LL32: for(0;_tmp38
!= 0;_tmp38=_tmp38->tl){Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple9*)_tmp38->hd)).f2);}
return;_LL33:;_LL34: {struct Cyc_Core_Impossible_struct _tmp188;const char*_tmp187;
struct Cyc_Core_Impossible_struct*_tmp186;(int)_throw((void*)((_tmp186=_cycalloc(
sizeof(*_tmp186)),((_tmp186[0]=((_tmp188.tag=Cyc_Core_Impossible,((_tmp188.f1=((
_tmp187="bad exp form in Tocv::box_free_vars_exp",_tag_dyneither(_tmp187,sizeof(
char),40))),_tmp188)))),_tmp186)))));}_LL0:;}}static void Cyc_Tovc_box_free_vars_stmt(
struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s);static void Cyc_Tovc_box_free_vars_stmt(
struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp44=s->r;
struct Cyc_Absyn_Stmt*_tmp4A;struct Cyc_Absyn_Exp*_tmp4C;struct Cyc_Absyn_Exp*
_tmp4E;struct Cyc_Absyn_Stmt*_tmp50;struct Cyc_Absyn_Stmt*_tmp51;struct Cyc_Absyn_Exp*
_tmp53;struct Cyc_Absyn_Stmt*_tmp54;struct Cyc_Absyn_Stmt*_tmp55;struct _tuple8
_tmp57;struct Cyc_Absyn_Exp*_tmp58;struct Cyc_Absyn_Stmt*_tmp59;struct Cyc_Absyn_Exp*
_tmp5B;struct _tuple8 _tmp5C;struct Cyc_Absyn_Exp*_tmp5D;struct _tuple8 _tmp5E;struct
Cyc_Absyn_Exp*_tmp5F;struct Cyc_Absyn_Stmt*_tmp60;struct Cyc_Absyn_Stmt*_tmp62;
struct _tuple8 _tmp63;struct Cyc_Absyn_Exp*_tmp64;struct Cyc_Absyn_Exp*_tmp66;struct
Cyc_List_List*_tmp67;struct Cyc_Absyn_Decl*_tmp69;struct Cyc_Absyn_Stmt*_tmp6A;
_LL3B: {struct Cyc_Absyn_Skip_s_struct*_tmp45=(struct Cyc_Absyn_Skip_s_struct*)
_tmp44;if(_tmp45->tag != 0)goto _LL3D;}_LL3C: goto _LL3E;_LL3D: {struct Cyc_Absyn_Break_s_struct*
_tmp46=(struct Cyc_Absyn_Break_s_struct*)_tmp44;if(_tmp46->tag != 6)goto _LL3F;}
_LL3E: goto _LL40;_LL3F: {struct Cyc_Absyn_Continue_s_struct*_tmp47=(struct Cyc_Absyn_Continue_s_struct*)
_tmp44;if(_tmp47->tag != 7)goto _LL41;}_LL40: return;_LL41: {struct Cyc_Absyn_Goto_s_struct*
_tmp48=(struct Cyc_Absyn_Goto_s_struct*)_tmp44;if(_tmp48->tag != 8)goto _LL43;}
_LL42: return;_LL43: {struct Cyc_Absyn_Label_s_struct*_tmp49=(struct Cyc_Absyn_Label_s_struct*)
_tmp44;if(_tmp49->tag != 13)goto _LL45;else{_tmp4A=_tmp49->f2;}}_LL44: s=_tmp4A;
continue;_LL45: {struct Cyc_Absyn_Return_s_struct*_tmp4B=(struct Cyc_Absyn_Return_s_struct*)
_tmp44;if(_tmp4B->tag != 3)goto _LL47;else{_tmp4C=_tmp4B->f1;}}_LL46: if(_tmp4C == 0)
return;_tmp4E=(struct Cyc_Absyn_Exp*)_tmp4C;goto _LL48;_LL47: {struct Cyc_Absyn_Exp_s_struct*
_tmp4D=(struct Cyc_Absyn_Exp_s_struct*)_tmp44;if(_tmp4D->tag != 1)goto _LL49;else{
_tmp4E=_tmp4D->f1;}}_LL48: Cyc_Tovc_box_free_vars_exp(env,_tmp4E);return;_LL49: {
struct Cyc_Absyn_Seq_s_struct*_tmp4F=(struct Cyc_Absyn_Seq_s_struct*)_tmp44;if(
_tmp4F->tag != 2)goto _LL4B;else{_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;}}_LL4A: Cyc_Tovc_box_free_vars_stmt(
env,_tmp50);s=_tmp51;continue;_LL4B: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp52=(
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp44;if(_tmp52->tag != 4)goto _LL4D;else{
_tmp53=_tmp52->f1;_tmp54=_tmp52->f2;_tmp55=_tmp52->f3;}}_LL4C: Cyc_Tovc_box_free_vars_exp(
env,_tmp53);Cyc_Tovc_box_free_vars_stmt(env,_tmp54);s=_tmp55;continue;_LL4D: {
struct Cyc_Absyn_While_s_struct*_tmp56=(struct Cyc_Absyn_While_s_struct*)_tmp44;
if(_tmp56->tag != 5)goto _LL4F;else{_tmp57=_tmp56->f1;_tmp58=_tmp57.f1;_tmp59=
_tmp56->f2;}}_LL4E: Cyc_Tovc_box_free_vars_exp(env,_tmp58);s=_tmp59;continue;
_LL4F: {struct Cyc_Absyn_For_s_struct*_tmp5A=(struct Cyc_Absyn_For_s_struct*)
_tmp44;if(_tmp5A->tag != 9)goto _LL51;else{_tmp5B=_tmp5A->f1;_tmp5C=_tmp5A->f2;
_tmp5D=_tmp5C.f1;_tmp5E=_tmp5A->f3;_tmp5F=_tmp5E.f1;_tmp60=_tmp5A->f4;}}_LL50:
Cyc_Tovc_box_free_vars_exp(env,_tmp5B);Cyc_Tovc_box_free_vars_exp(env,_tmp5D);
Cyc_Tovc_box_free_vars_exp(env,_tmp5F);s=_tmp60;continue;_LL51: {struct Cyc_Absyn_Do_s_struct*
_tmp61=(struct Cyc_Absyn_Do_s_struct*)_tmp44;if(_tmp61->tag != 14)goto _LL53;else{
_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;_tmp64=_tmp63.f1;}}_LL52: Cyc_Tovc_box_free_vars_exp(
env,_tmp64);s=_tmp62;continue;_LL53: {struct Cyc_Absyn_Switch_s_struct*_tmp65=(
struct Cyc_Absyn_Switch_s_struct*)_tmp44;if(_tmp65->tag != 10)goto _LL55;else{
_tmp66=_tmp65->f1;_tmp67=_tmp65->f2;}}_LL54: Cyc_Tovc_box_free_vars_exp(env,
_tmp66);for(0;_tmp67 != 0;_tmp67=_tmp67->tl){Cyc_Tovc_box_free_vars_stmt(env,((
struct Cyc_Absyn_Switch_clause*)_tmp67->hd)->body);}return;_LL55: {struct Cyc_Absyn_Decl_s_struct*
_tmp68=(struct Cyc_Absyn_Decl_s_struct*)_tmp44;if(_tmp68->tag != 12)goto _LL57;
else{_tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;}}_LL56:{void*_tmp6B=_tmp69->r;struct
Cyc_Absyn_Vardecl*_tmp6D;_LL5A: {struct Cyc_Absyn_Var_d_struct*_tmp6C=(struct Cyc_Absyn_Var_d_struct*)
_tmp6B;if(_tmp6C->tag != 0)goto _LL5C;else{_tmp6D=_tmp6C->f1;}}_LL5B:{struct Cyc_List_List*
_tmp189;env.boundvars=((_tmp189=_region_malloc(env.rgn,sizeof(*_tmp189)),((
_tmp189->hd=_tmp6D,((_tmp189->tl=env.boundvars,_tmp189))))));}env.varmap=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete_same)(
env.varmap,(*_tmp6D->name).f2);if(_tmp6D->initializer != 0)Cyc_Tovc_box_free_vars_exp(
env,(struct Cyc_Absyn_Exp*)_check_null(_tmp6D->initializer));s=_tmp6A;continue;
_LL5C:;_LL5D: goto _LL59;_LL59:;}goto _LL58;_LL57:;_LL58: {struct Cyc_Core_Impossible_struct
_tmp18F;const char*_tmp18E;struct Cyc_Core_Impossible_struct*_tmp18D;(int)_throw((
void*)((_tmp18D=_cycalloc(sizeof(*_tmp18D)),((_tmp18D[0]=((_tmp18F.tag=Cyc_Core_Impossible,((
_tmp18F.f1=((_tmp18E="bad stmt after xlation to C",_tag_dyneither(_tmp18E,
sizeof(char),28))),_tmp18F)))),_tmp18D)))));}_LL3A:;}}static int Cyc_Tovc_stmt_to_fun_cmp(
struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y);static int Cyc_Tovc_stmt_to_fun_cmp(
struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){return Cyc_Absyn_qvar_cmp(x->name,
y->name);}struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;
};static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,
struct Cyc_Absyn_Stmt*s,void*rettype);static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(
struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){struct Cyc_Set_Set*
freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty)(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
struct Cyc_Tovc_BoxingEnv _tmp190;struct Cyc_Tovc_BoxingEnv _tmp72=(_tmp190.rgn=env.rgn,((
_tmp190.all_locals=env.all_locals,((_tmp190.varmap=env.varmap,((_tmp190.boundvars=
0,((_tmp190.freevars=& freevars,_tmp190)))))))));Cyc_Tovc_box_free_vars_stmt(
_tmp72,s);{struct Cyc_List_List*params=0;struct Cyc_List_List*args=0;struct Cyc_Iter_Iter
iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(
Cyc_Core_heap_region,freevars);struct Cyc_Absyn_Vardecl*_tmp191;struct Cyc_Absyn_Vardecl*
_tmp73=(_tmp191=_cycalloc(sizeof(*_tmp191)),((_tmp191->sc=Cyc_Absyn_Public,((
_tmp191->name=Cyc_Absyn_exn_name,((_tmp191->tq=Cyc_Absyn_empty_tqual(0),((
_tmp191->type=(void*)& Cyc_Absyn_VoidType_val,((_tmp191->initializer=0,((_tmp191->rgn=
0,((_tmp191->attributes=0,((_tmp191->escapes=0,_tmp191)))))))))))))))));while(((
int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(iter,&
_tmp73)){void*_tmp74=_tmp73->type;struct Cyc_Absyn_ArrayInfo _tmp76;void*_tmp77;
_LL5F: {struct Cyc_Absyn_ArrayType_struct*_tmp75=(struct Cyc_Absyn_ArrayType_struct*)
_tmp74;if(_tmp75->tag != 9)goto _LL61;else{_tmp76=_tmp75->f1;_tmp77=_tmp76.elt_type;}}
_LL60:{struct _tuple10*_tmp194;struct Cyc_List_List*_tmp193;params=((_tmp193=
_cycalloc(sizeof(*_tmp193)),((_tmp193->hd=((_tmp194=_cycalloc(sizeof(*_tmp194)),((
_tmp194->f1=(*_tmp73->name).f2,((_tmp194->f2=_tmp73->tq,((_tmp194->f3=Cyc_Absyn_cstar_typ(
_tmp77,Cyc_Absyn_empty_tqual(0)),_tmp194)))))))),((_tmp193->tl=params,_tmp193))))));}{
struct Cyc_List_List*_tmp195;args=((_tmp195=_cycalloc(sizeof(*_tmp195)),((_tmp195->hd=
Cyc_Absyn_var_exp(_tmp73->name,0),((_tmp195->tl=args,_tmp195))))));}goto _LL5E;
_LL61:;_LL62:{struct _tuple10*_tmp198;struct Cyc_List_List*_tmp197;params=((
_tmp197=_cycalloc(sizeof(*_tmp197)),((_tmp197->hd=((_tmp198=_cycalloc(sizeof(*
_tmp198)),((_tmp198->f1=(*_tmp73->name).f2,((_tmp198->f2=_tmp73->tq,((_tmp198->f3=
Cyc_Absyn_cstar_typ(_tmp73->type,Cyc_Absyn_empty_tqual(0)),_tmp198)))))))),((
_tmp197->tl=params,_tmp197))))));}{struct Cyc_List_List*_tmp199;args=((_tmp199=
_cycalloc(sizeof(*_tmp199)),((_tmp199->hd=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(
_tmp73->name,0),0),((_tmp199->tl=args,_tmp199))))));}goto _LL5E;_LL5E:;}{struct
_tuple0*funname=Cyc_Toc_temp_var();{struct Cyc_Absyn_Fn_d_struct*_tmp1A3;struct
Cyc_Absyn_Fndecl*_tmp1A2;struct Cyc_Absyn_Fn_d_struct _tmp1A1;struct Cyc_List_List*
_tmp1A0;*env.gen_ds=((_tmp1A0=_cycalloc(sizeof(*_tmp1A0)),((_tmp1A0->hd=Cyc_Absyn_new_decl((
void*)((_tmp1A3=_cycalloc(sizeof(*_tmp1A3)),((_tmp1A3[0]=((_tmp1A1.tag=1,((
_tmp1A1.f1=((_tmp1A2=_cycalloc(sizeof(*_tmp1A2)),((_tmp1A2->sc=Cyc_Absyn_Static,((
_tmp1A2->is_inline=0,((_tmp1A2->name=funname,((_tmp1A2->tvs=0,((_tmp1A2->effect=
0,((_tmp1A2->ret_type=rettype,((_tmp1A2->args=params,((_tmp1A2->c_varargs=0,((
_tmp1A2->cyc_varargs=0,((_tmp1A2->rgn_po=0,((_tmp1A2->body=s,((_tmp1A2->cached_typ=
0,((_tmp1A2->param_vardecls=0,((_tmp1A2->fn_vardecl=0,((_tmp1A2->attributes=0,
_tmp1A2)))))))))))))))))))))))))))))))),_tmp1A1)))),_tmp1A3)))),0),((_tmp1A0->tl=*
env.gen_ds,_tmp1A0))))));}return Cyc_Absyn_fncall_exp(Cyc_Absyn_var_exp(funname,0),
args,0);};};}static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*
e);static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*
e){void*_tmp84=e->r;struct _tuple0*_tmp86;struct _tuple0 _tmp87;union Cyc_Absyn_Nmspace
_tmp88;int _tmp89;struct _dyneither_ptr*_tmp8A;struct Cyc_Absyn_Exp*_tmp8E;struct
Cyc_List_List*_tmp8F;struct Cyc_List_List*_tmp91;struct Cyc_Absyn_Exp*_tmp93;
struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp97;
struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp9B;
struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmpA0;
struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Exp*_tmpA4;
struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpAA;
struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpB0;
struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB6;
struct Cyc_List_List*_tmpBC;_LL64: {struct Cyc_Absyn_Var_e_struct*_tmp85=(struct
Cyc_Absyn_Var_e_struct*)_tmp84;if(_tmp85->tag != 1)goto _LL66;else{_tmp86=_tmp85->f1;
_tmp87=*_tmp86;_tmp88=_tmp87.f1;if((_tmp88.Loc_n).tag != 3)goto _LL66;_tmp89=(int)(
_tmp88.Loc_n).val;_tmp8A=_tmp87.f2;}}_LL65: if(((int(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k))Cyc_Dict_member)(varmap,_tmp8A))e->r=(((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(varmap,_tmp8A))->r;
return;_LL66: {struct Cyc_Absyn_Var_e_struct*_tmp8B=(struct Cyc_Absyn_Var_e_struct*)
_tmp84;if(_tmp8B->tag != 1)goto _LL68;}_LL67: return;_LL68: {struct Cyc_Absyn_Const_e_struct*
_tmp8C=(struct Cyc_Absyn_Const_e_struct*)_tmp84;if(_tmp8C->tag != 0)goto _LL6A;}
_LL69: return;_LL6A: {struct Cyc_Absyn_FnCall_e_struct*_tmp8D=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp84;if(_tmp8D->tag != 11)goto _LL6C;else{_tmp8E=_tmp8D->f1;_tmp8F=_tmp8D->f2;}}
_LL6B: Cyc_Tovc_apply_varmap(varmap,_tmp8E);_tmp91=_tmp8F;goto _LL6D;_LL6C: {
struct Cyc_Absyn_Primop_e_struct*_tmp90=(struct Cyc_Absyn_Primop_e_struct*)_tmp84;
if(_tmp90->tag != 3)goto _LL6E;else{_tmp91=_tmp90->f2;}}_LL6D: for(0;_tmp91 != 0;
_tmp91=_tmp91->tl){Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)_tmp91->hd);}
return;_LL6E: {struct Cyc_Absyn_Conditional_e_struct*_tmp92=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp84;if(_tmp92->tag != 6)goto _LL70;else{_tmp93=_tmp92->f1;_tmp94=_tmp92->f2;
_tmp95=_tmp92->f3;}}_LL6F: Cyc_Tovc_apply_varmap(varmap,_tmp95);_tmp97=_tmp93;
_tmp98=_tmp94;goto _LL71;_LL70: {struct Cyc_Absyn_And_e_struct*_tmp96=(struct Cyc_Absyn_And_e_struct*)
_tmp84;if(_tmp96->tag != 7)goto _LL72;else{_tmp97=_tmp96->f1;_tmp98=_tmp96->f2;}}
_LL71: _tmp9A=_tmp97;_tmp9B=_tmp98;goto _LL73;_LL72: {struct Cyc_Absyn_Or_e_struct*
_tmp99=(struct Cyc_Absyn_Or_e_struct*)_tmp84;if(_tmp99->tag != 8)goto _LL74;else{
_tmp9A=_tmp99->f1;_tmp9B=_tmp99->f2;}}_LL73: _tmp9D=_tmp9A;_tmp9E=_tmp9B;goto
_LL75;_LL74: {struct Cyc_Absyn_Subscript_e_struct*_tmp9C=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp84;if(_tmp9C->tag != 25)goto _LL76;else{_tmp9D=_tmp9C->f1;_tmp9E=_tmp9C->f2;}}
_LL75: _tmpA0=_tmp9D;_tmpA1=_tmp9E;goto _LL77;_LL76: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp9F=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp84;if(_tmp9F->tag != 9)goto _LL78;
else{_tmpA0=_tmp9F->f1;_tmpA1=_tmp9F->f2;}}_LL77: _tmpA3=_tmpA0;_tmpA4=_tmpA1;
goto _LL79;_LL78: {struct Cyc_Absyn_AssignOp_e_struct*_tmpA2=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp84;if(_tmpA2->tag != 4)goto _LL7A;else{_tmpA3=_tmpA2->f1;_tmpA4=_tmpA2->f3;}}
_LL79: Cyc_Tovc_apply_varmap(varmap,_tmpA4);_tmpA6=_tmpA3;goto _LL7B;_LL7A: {
struct Cyc_Absyn_NoInstantiate_e_struct*_tmpA5=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp84;if(_tmpA5->tag != 13)goto _LL7C;else{_tmpA6=_tmpA5->f1;}}_LL7B: _tmpA8=
_tmpA6;goto _LL7D;_LL7C: {struct Cyc_Absyn_Instantiate_e_struct*_tmpA7=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp84;if(_tmpA7->tag != 14)goto _LL7E;else{_tmpA8=_tmpA7->f1;}}_LL7D: _tmpAA=
_tmpA8;goto _LL7F;_LL7E: {struct Cyc_Absyn_Cast_e_struct*_tmpA9=(struct Cyc_Absyn_Cast_e_struct*)
_tmp84;if(_tmpA9->tag != 15)goto _LL80;else{_tmpAA=_tmpA9->f2;}}_LL7F: _tmpAC=
_tmpAA;goto _LL81;_LL80: {struct Cyc_Absyn_Address_e_struct*_tmpAB=(struct Cyc_Absyn_Address_e_struct*)
_tmp84;if(_tmpAB->tag != 16)goto _LL82;else{_tmpAC=_tmpAB->f1;}}_LL81: _tmpAE=
_tmpAC;goto _LL83;_LL82: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpAD=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp84;if(_tmpAD->tag != 19)goto _LL84;else{_tmpAE=_tmpAD->f1;}}_LL83: _tmpB0=
_tmpAE;goto _LL85;_LL84: {struct Cyc_Absyn_Deref_e_struct*_tmpAF=(struct Cyc_Absyn_Deref_e_struct*)
_tmp84;if(_tmpAF->tag != 22)goto _LL86;else{_tmpB0=_tmpAF->f1;}}_LL85: _tmpB2=
_tmpB0;goto _LL87;_LL86: {struct Cyc_Absyn_AggrMember_e_struct*_tmpB1=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp84;if(_tmpB1->tag != 23)goto _LL88;else{_tmpB2=_tmpB1->f1;}}_LL87: _tmpB4=
_tmpB2;goto _LL89;_LL88: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpB3=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp84;if(_tmpB3->tag != 24)goto _LL8A;else{_tmpB4=_tmpB3->f1;}}_LL89: _tmpB6=
_tmpB4;goto _LL8B;_LL8A: {struct Cyc_Absyn_Increment_e_struct*_tmpB5=(struct Cyc_Absyn_Increment_e_struct*)
_tmp84;if(_tmpB5->tag != 5)goto _LL8C;else{_tmpB6=_tmpB5->f1;}}_LL8B: Cyc_Tovc_apply_varmap(
varmap,_tmpB6);return;_LL8C: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpB7=(struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmp84;if(_tmpB7->tag != 18)goto _LL8E;}_LL8D: goto
_LL8F;_LL8E: {struct Cyc_Absyn_Offsetof_e_struct*_tmpB8=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp84;if(_tmpB8->tag != 20)goto _LL90;}_LL8F: goto _LL91;_LL90: {struct Cyc_Absyn_Gentyp_e_struct*
_tmpB9=(struct Cyc_Absyn_Gentyp_e_struct*)_tmp84;if(_tmpB9->tag != 21)goto _LL92;}
_LL91: goto _LL93;_LL92: {struct Cyc_Absyn_Enum_e_struct*_tmpBA=(struct Cyc_Absyn_Enum_e_struct*)
_tmp84;if(_tmpBA->tag != 33)goto _LL94;}_LL93: return;_LL94: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpBB=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp84;if(_tmpBB->tag != 37)goto
_LL96;else{_tmpBC=_tmpBB->f2;}}_LL95: for(0;_tmpBC != 0;_tmpBC=_tmpBC->tl){Cyc_Tovc_apply_varmap(
varmap,(*((struct _tuple9*)_tmpBC->hd)).f2);}return;_LL96:;_LL97: {struct Cyc_Core_Impossible_struct
_tmp1A9;const char*_tmp1A8;struct Cyc_Core_Impossible_struct*_tmp1A7;(int)_throw((
void*)((_tmp1A7=_cycalloc(sizeof(*_tmp1A7)),((_tmp1A7[0]=((_tmp1A9.tag=Cyc_Core_Impossible,((
_tmp1A9.f1=((_tmp1A8="bad exp form in Tocv::apply_varmap",_tag_dyneither(_tmp1A8,
sizeof(char),35))),_tmp1A9)))),_tmp1A7)))));}_LL63:;}static struct Cyc_Absyn_Exp*
Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*
s){void*_tmpC0=s->r;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Stmt*_tmpC5;
struct Cyc_Absyn_Stmt*_tmpC6;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Stmt*
_tmpC9;struct Cyc_Absyn_Stmt*_tmpCA;struct Cyc_Absyn_Decl*_tmpCC;struct Cyc_Absyn_Stmt*
_tmpCD;_LL99: {struct Cyc_Absyn_Skip_s_struct*_tmpC1=(struct Cyc_Absyn_Skip_s_struct*)
_tmpC0;if(_tmpC1->tag != 0)goto _LL9B;}_LL9A: return Cyc_Absyn_true_exp(0);_LL9B: {
struct Cyc_Absyn_Exp_s_struct*_tmpC2=(struct Cyc_Absyn_Exp_s_struct*)_tmpC0;if(
_tmpC2->tag != 1)goto _LL9D;else{_tmpC3=_tmpC2->f1;}}_LL9C: Cyc_Tovc_apply_varmap(
env.varmap,_tmpC3);return _tmpC3;_LL9D: {struct Cyc_Absyn_Seq_s_struct*_tmpC4=(
struct Cyc_Absyn_Seq_s_struct*)_tmpC0;if(_tmpC4->tag != 2)goto _LL9F;else{_tmpC5=
_tmpC4->f1;_tmpC6=_tmpC4->f2;}}_LL9E: return Cyc_Absyn_seq_exp(Cyc_Tovc_stmt_to_exp(
env,_tmpC5),Cyc_Tovc_stmt_to_exp(env,_tmpC6),0);_LL9F: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmpC7=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmpC0;if(_tmpC7->tag != 4)goto _LLA1;
else{_tmpC8=_tmpC7->f1;_tmpC9=_tmpC7->f2;_tmpCA=_tmpC7->f3;}}_LLA0: Cyc_Tovc_apply_varmap(
env.varmap,_tmpC8);return Cyc_Absyn_conditional_exp(_tmpC8,Cyc_Tovc_stmt_to_exp(
env,_tmpC9),Cyc_Tovc_stmt_to_exp(env,_tmpCA),0);_LLA1: {struct Cyc_Absyn_Decl_s_struct*
_tmpCB=(struct Cyc_Absyn_Decl_s_struct*)_tmpC0;if(_tmpCB->tag != 12)goto _LLA3;
else{_tmpCC=_tmpCB->f1;_tmpCD=_tmpCB->f2;}}_LLA2: {void*_tmpCE=_tmpCC->r;struct
Cyc_Absyn_Vardecl*_tmpD0;_LLA6: {struct Cyc_Absyn_Var_d_struct*_tmpCF=(struct Cyc_Absyn_Var_d_struct*)
_tmpCE;if(_tmpCF->tag != 0)goto _LLA8;else{_tmpD0=_tmpCF->f1;}}_LLA7: {struct
_dyneither_ptr*_tmpD1=(*_tmpD0->name).f2;struct _tuple0*_tmpD2=Cyc_Toc_temp_var();
_tmpD0->name=_tmpD2;env.varmap=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct _dyneither_ptr*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(env.varmap,_tmpD1,
Cyc_Absyn_var_exp(_tmpD2,0));{struct Cyc_List_List*_tmpD3=0;if(_tmpD0->initializer
!= 0){Cyc_Tovc_apply_varmap(env.varmap,(struct Cyc_Absyn_Exp*)_check_null(_tmpD0->initializer));{
void*_tmpD4=((struct Cyc_Absyn_Exp*)_check_null(_tmpD0->initializer))->r;struct
Cyc_List_List*_tmpD6;_LLAB: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpD5=(
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpD4;if(_tmpD5->tag != 37)goto _LLAD;
else{_tmpD6=_tmpD5->f2;}}_LLAC: {struct Cyc_Absyn_Exp*_tmpD7=Cyc_Absyn_var_exp(
_tmpD0->name,0);{unsigned int i=0;for(0;_tmpD6 != 0;(_tmpD6=_tmpD6->tl,++ i)){
struct Cyc_List_List*_tmp1AA;_tmpD3=((_tmp1AA=_cycalloc(sizeof(*_tmp1AA)),((
_tmp1AA->hd=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(_tmpD7,Cyc_Absyn_uint_exp(
i,0),0),(*((struct _tuple9*)_tmpD6->hd)).f2,0),((_tmp1AA->tl=_tmpD3,_tmp1AA))))));}}
_tmpD0->initializer=0;goto _LLAA;}_LLAD:;_LLAE: goto _LLAA;_LLAA:;};}{struct Cyc_List_List*
_tmp1AB;env.all_locals=((_tmp1AB=_region_malloc(env.rgn,sizeof(*_tmp1AB)),((
_tmp1AB->hd=_tmpD0,((_tmp1AB->tl=env.all_locals,_tmp1AB))))));}(env.encloser)->r=(
Cyc_Absyn_decl_stmt(_tmpCC,Cyc_Absyn_new_stmt((env.encloser)->r,0),0))->r;{
struct Cyc_Absyn_Exp*_tmpDA=Cyc_Tovc_stmt_to_exp(env,_tmpCD);if(_tmpD0->initializer
!= 0){_tmpDA=Cyc_Absyn_seq_exp(Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmpD0->name,
0),(struct Cyc_Absyn_Exp*)_check_null(_tmpD0->initializer),0),_tmpDA,0);_tmpD0->initializer=
0;}for(0;_tmpD3 != 0;_tmpD3=_tmpD3->tl){_tmpDA=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)
_tmpD3->hd,_tmpDA,0);}return _tmpDA;};};}_LLA8:;_LLA9: {struct Cyc_Core_Impossible_struct
_tmp1B1;const char*_tmp1B0;struct Cyc_Core_Impossible_struct*_tmp1AF;(int)_throw((
void*)((_tmp1AF=_cycalloc(sizeof(*_tmp1AF)),((_tmp1AF[0]=((_tmp1B1.tag=Cyc_Core_Impossible,((
_tmp1B1.f1=((_tmp1B0="bad local decl in Tovc::stmt_to_exp",_tag_dyneither(
_tmp1B0,sizeof(char),36))),_tmp1B1)))),_tmp1AF)))));}_LLA5:;}_LLA3:;_LLA4: return
Cyc_Tovc_stmt_to_fun(env,s,(void*)& Cyc_Absyn_VoidType_val);_LL98:;}static void Cyc_Tovc_stmt_to_vc(
struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);static void Cyc_Tovc_exp_to_vc(
struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e);static void Cyc_Tovc_exp_to_vc(
struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){void*_tmpDE=e->r;struct Cyc_Absyn_Exp*
_tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_List_List*_tmpE5;struct Cyc_Absyn_Exp*
_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*
_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*
_tmpEF;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*
_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*
_tmpF8;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*
_tmpFE;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*
_tmp104;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*
_tmp10A;struct Cyc_List_List*_tmp110;struct Cyc_Absyn_Stmt*_tmp112;_LLB0: {struct
Cyc_Absyn_Const_e_struct*_tmpDF=(struct Cyc_Absyn_Const_e_struct*)_tmpDE;if(
_tmpDF->tag != 0)goto _LLB2;}_LLB1: return;_LLB2: {struct Cyc_Absyn_Var_e_struct*
_tmpE0=(struct Cyc_Absyn_Var_e_struct*)_tmpDE;if(_tmpE0->tag != 1)goto _LLB4;}_LLB3:
return;_LLB4: {struct Cyc_Absyn_FnCall_e_struct*_tmpE1=(struct Cyc_Absyn_FnCall_e_struct*)
_tmpDE;if(_tmpE1->tag != 11)goto _LLB6;else{_tmpE2=_tmpE1->f1;_tmpE3=_tmpE1->f2;}}
_LLB5: Cyc_Tovc_exp_to_vc(env,_tmpE2);_tmpE5=_tmpE3;goto _LLB7;_LLB6: {struct Cyc_Absyn_Primop_e_struct*
_tmpE4=(struct Cyc_Absyn_Primop_e_struct*)_tmpDE;if(_tmpE4->tag != 3)goto _LLB8;
else{_tmpE5=_tmpE4->f2;}}_LLB7: for(0;_tmpE5 != 0;_tmpE5=_tmpE5->tl){Cyc_Tovc_exp_to_vc(
env,(struct Cyc_Absyn_Exp*)_tmpE5->hd);}return;_LLB8: {struct Cyc_Absyn_Conditional_e_struct*
_tmpE6=(struct Cyc_Absyn_Conditional_e_struct*)_tmpDE;if(_tmpE6->tag != 6)goto
_LLBA;else{_tmpE7=_tmpE6->f1;_tmpE8=_tmpE6->f2;_tmpE9=_tmpE6->f3;}}_LLB9: Cyc_Tovc_exp_to_vc(
env,_tmpE9);_tmpEB=_tmpE7;_tmpEC=_tmpE8;goto _LLBB;_LLBA: {struct Cyc_Absyn_And_e_struct*
_tmpEA=(struct Cyc_Absyn_And_e_struct*)_tmpDE;if(_tmpEA->tag != 7)goto _LLBC;else{
_tmpEB=_tmpEA->f1;_tmpEC=_tmpEA->f2;}}_LLBB: _tmpEE=_tmpEB;_tmpEF=_tmpEC;goto
_LLBD;_LLBC: {struct Cyc_Absyn_Or_e_struct*_tmpED=(struct Cyc_Absyn_Or_e_struct*)
_tmpDE;if(_tmpED->tag != 8)goto _LLBE;else{_tmpEE=_tmpED->f1;_tmpEF=_tmpED->f2;}}
_LLBD: _tmpF1=_tmpEE;_tmpF2=_tmpEF;goto _LLBF;_LLBE: {struct Cyc_Absyn_Subscript_e_struct*
_tmpF0=(struct Cyc_Absyn_Subscript_e_struct*)_tmpDE;if(_tmpF0->tag != 25)goto _LLC0;
else{_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;}}_LLBF: _tmpF4=_tmpF1;_tmpF5=_tmpF2;
goto _LLC1;_LLC0: {struct Cyc_Absyn_SeqExp_e_struct*_tmpF3=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpDE;if(_tmpF3->tag != 9)goto _LLC2;else{_tmpF4=_tmpF3->f1;_tmpF5=_tmpF3->f2;}}
_LLC1: _tmpF7=_tmpF4;_tmpF8=_tmpF5;goto _LLC3;_LLC2: {struct Cyc_Absyn_AssignOp_e_struct*
_tmpF6=(struct Cyc_Absyn_AssignOp_e_struct*)_tmpDE;if(_tmpF6->tag != 4)goto _LLC4;
else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f3;}}_LLC3: Cyc_Tovc_exp_to_vc(env,_tmpF8);
_tmpFA=_tmpF7;goto _LLC5;_LLC4: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmpF9=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpDE;if(_tmpF9->tag != 13)goto _LLC6;
else{_tmpFA=_tmpF9->f1;}}_LLC5: _tmpFC=_tmpFA;goto _LLC7;_LLC6: {struct Cyc_Absyn_Instantiate_e_struct*
_tmpFB=(struct Cyc_Absyn_Instantiate_e_struct*)_tmpDE;if(_tmpFB->tag != 14)goto
_LLC8;else{_tmpFC=_tmpFB->f1;}}_LLC7: _tmpFE=_tmpFC;goto _LLC9;_LLC8: {struct Cyc_Absyn_Cast_e_struct*
_tmpFD=(struct Cyc_Absyn_Cast_e_struct*)_tmpDE;if(_tmpFD->tag != 15)goto _LLCA;
else{_tmpFE=_tmpFD->f2;}}_LLC9: _tmp100=_tmpFE;goto _LLCB;_LLCA: {struct Cyc_Absyn_Address_e_struct*
_tmpFF=(struct Cyc_Absyn_Address_e_struct*)_tmpDE;if(_tmpFF->tag != 16)goto _LLCC;
else{_tmp100=_tmpFF->f1;}}_LLCB: _tmp102=_tmp100;goto _LLCD;_LLCC: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp101=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpDE;if(_tmp101->tag != 19)goto
_LLCE;else{_tmp102=_tmp101->f1;}}_LLCD: _tmp104=_tmp102;goto _LLCF;_LLCE: {struct
Cyc_Absyn_Deref_e_struct*_tmp103=(struct Cyc_Absyn_Deref_e_struct*)_tmpDE;if(
_tmp103->tag != 22)goto _LLD0;else{_tmp104=_tmp103->f1;}}_LLCF: _tmp106=_tmp104;
goto _LLD1;_LLD0: {struct Cyc_Absyn_AggrMember_e_struct*_tmp105=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpDE;if(_tmp105->tag != 23)goto _LLD2;else{_tmp106=_tmp105->f1;}}_LLD1: _tmp108=
_tmp106;goto _LLD3;_LLD2: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp107=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpDE;if(_tmp107->tag != 24)goto _LLD4;else{_tmp108=_tmp107->f1;}}_LLD3: _tmp10A=
_tmp108;goto _LLD5;_LLD4: {struct Cyc_Absyn_Increment_e_struct*_tmp109=(struct Cyc_Absyn_Increment_e_struct*)
_tmpDE;if(_tmp109->tag != 5)goto _LLD6;else{_tmp10A=_tmp109->f1;}}_LLD5: Cyc_Tovc_exp_to_vc(
env,_tmp10A);return;_LLD6: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp10B=(struct
Cyc_Absyn_Sizeoftyp_e_struct*)_tmpDE;if(_tmp10B->tag != 18)goto _LLD8;}_LLD7: goto
_LLD9;_LLD8: {struct Cyc_Absyn_Offsetof_e_struct*_tmp10C=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpDE;if(_tmp10C->tag != 20)goto _LLDA;}_LLD9: goto _LLDB;_LLDA: {struct Cyc_Absyn_Gentyp_e_struct*
_tmp10D=(struct Cyc_Absyn_Gentyp_e_struct*)_tmpDE;if(_tmp10D->tag != 21)goto _LLDC;}
_LLDB: goto _LLDD;_LLDC: {struct Cyc_Absyn_Enum_e_struct*_tmp10E=(struct Cyc_Absyn_Enum_e_struct*)
_tmpDE;if(_tmp10E->tag != 33)goto _LLDE;}_LLDD: return;_LLDE: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp10F=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpDE;if(_tmp10F->tag != 37)
goto _LLE0;else{_tmp110=_tmp10F->f2;}}_LLDF: for(0;_tmp110 != 0;_tmp110=_tmp110->tl){
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple9*)_tmp110->hd)).f2);}return;_LLE0: {
struct Cyc_Absyn_StmtExp_e_struct*_tmp111=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpDE;if(_tmp111->tag != 38)goto _LLE2;else{_tmp112=_tmp111->f1;}}_LLE1: Cyc_Tovc_stmt_to_vc(
env,_tmp112);e->r=(Cyc_Tovc_stmt_to_exp(env,_tmp112))->r;return;_LLE2:;_LLE3: {
struct Cyc_Core_Impossible_struct _tmp1B7;const char*_tmp1B6;struct Cyc_Core_Impossible_struct*
_tmp1B5;(int)_throw((void*)((_tmp1B5=_cycalloc(sizeof(*_tmp1B5)),((_tmp1B5[0]=((
_tmp1B7.tag=Cyc_Core_Impossible,((_tmp1B7.f1=((_tmp1B6="bad exp form after xlation to C",
_tag_dyneither(_tmp1B6,sizeof(char),32))),_tmp1B7)))),_tmp1B5)))));}_LLAF:;}
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
while(1){env.encloser=s;{void*_tmp116=s->r;struct Cyc_Absyn_Stmt*_tmp11C;struct
Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Stmt*_tmp122;
struct Cyc_Absyn_Stmt*_tmp123;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Stmt*
_tmp126;struct Cyc_Absyn_Stmt*_tmp127;struct _tuple8 _tmp129;struct Cyc_Absyn_Exp*
_tmp12A;struct Cyc_Absyn_Stmt*_tmp12B;struct Cyc_Absyn_Exp*_tmp12D;struct _tuple8
_tmp12E;struct Cyc_Absyn_Exp*_tmp12F;struct _tuple8 _tmp130;struct Cyc_Absyn_Exp*
_tmp131;struct Cyc_Absyn_Stmt*_tmp132;struct Cyc_Absyn_Stmt*_tmp134;struct _tuple8
_tmp135;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_List_List*
_tmp139;struct Cyc_Absyn_Decl*_tmp13B;struct Cyc_Absyn_Stmt*_tmp13C;_LLE5: {struct
Cyc_Absyn_Skip_s_struct*_tmp117=(struct Cyc_Absyn_Skip_s_struct*)_tmp116;if(
_tmp117->tag != 0)goto _LLE7;}_LLE6: goto _LLE8;_LLE7: {struct Cyc_Absyn_Break_s_struct*
_tmp118=(struct Cyc_Absyn_Break_s_struct*)_tmp116;if(_tmp118->tag != 6)goto _LLE9;}
_LLE8: goto _LLEA;_LLE9: {struct Cyc_Absyn_Continue_s_struct*_tmp119=(struct Cyc_Absyn_Continue_s_struct*)
_tmp116;if(_tmp119->tag != 7)goto _LLEB;}_LLEA: goto _LLEC;_LLEB: {struct Cyc_Absyn_Goto_s_struct*
_tmp11A=(struct Cyc_Absyn_Goto_s_struct*)_tmp116;if(_tmp11A->tag != 8)goto _LLED;}
_LLEC: return;_LLED: {struct Cyc_Absyn_Label_s_struct*_tmp11B=(struct Cyc_Absyn_Label_s_struct*)
_tmp116;if(_tmp11B->tag != 13)goto _LLEF;else{_tmp11C=_tmp11B->f2;}}_LLEE: s=
_tmp11C;continue;_LLEF: {struct Cyc_Absyn_Return_s_struct*_tmp11D=(struct Cyc_Absyn_Return_s_struct*)
_tmp116;if(_tmp11D->tag != 3)goto _LLF1;else{_tmp11E=_tmp11D->f1;}}_LLF0: if(
_tmp11E == 0)return;_tmp120=(struct Cyc_Absyn_Exp*)_tmp11E;goto _LLF2;_LLF1: {
struct Cyc_Absyn_Exp_s_struct*_tmp11F=(struct Cyc_Absyn_Exp_s_struct*)_tmp116;if(
_tmp11F->tag != 1)goto _LLF3;else{_tmp120=_tmp11F->f1;}}_LLF2: Cyc_Tovc_exp_to_vc(
env,_tmp120);return;_LLF3: {struct Cyc_Absyn_Seq_s_struct*_tmp121=(struct Cyc_Absyn_Seq_s_struct*)
_tmp116;if(_tmp121->tag != 2)goto _LLF5;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;}}
_LLF4: Cyc_Tovc_stmt_to_vc(env,_tmp122);s=_tmp123;continue;_LLF5: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp124=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp116;if(_tmp124->tag != 4)goto
_LLF7;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;_tmp127=_tmp124->f3;}}_LLF6:
Cyc_Tovc_exp_to_vc(env,_tmp125);Cyc_Tovc_stmt_to_vc(env,_tmp126);s=_tmp127;
continue;_LLF7: {struct Cyc_Absyn_While_s_struct*_tmp128=(struct Cyc_Absyn_While_s_struct*)
_tmp116;if(_tmp128->tag != 5)goto _LLF9;else{_tmp129=_tmp128->f1;_tmp12A=_tmp129.f1;
_tmp12B=_tmp128->f2;}}_LLF8: Cyc_Tovc_exp_to_vc(env,_tmp12A);s=_tmp12B;continue;
_LLF9: {struct Cyc_Absyn_For_s_struct*_tmp12C=(struct Cyc_Absyn_For_s_struct*)
_tmp116;if(_tmp12C->tag != 9)goto _LLFB;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;
_tmp12F=_tmp12E.f1;_tmp130=_tmp12C->f3;_tmp131=_tmp130.f1;_tmp132=_tmp12C->f4;}}
_LLFA: Cyc_Tovc_exp_to_vc(env,_tmp12D);Cyc_Tovc_exp_to_vc(env,_tmp12F);Cyc_Tovc_exp_to_vc(
env,_tmp131);s=_tmp132;continue;_LLFB: {struct Cyc_Absyn_Do_s_struct*_tmp133=(
struct Cyc_Absyn_Do_s_struct*)_tmp116;if(_tmp133->tag != 14)goto _LLFD;else{_tmp134=
_tmp133->f1;_tmp135=_tmp133->f2;_tmp136=_tmp135.f1;}}_LLFC: Cyc_Tovc_exp_to_vc(
env,_tmp136);s=_tmp134;continue;_LLFD: {struct Cyc_Absyn_Switch_s_struct*_tmp137=(
struct Cyc_Absyn_Switch_s_struct*)_tmp116;if(_tmp137->tag != 10)goto _LLFF;else{
_tmp138=_tmp137->f1;_tmp139=_tmp137->f2;}}_LLFE: Cyc_Tovc_exp_to_vc(env,_tmp138);
for(0;_tmp139 != 0;_tmp139=_tmp139->tl){Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)
_tmp139->hd)->body);}return;_LLFF: {struct Cyc_Absyn_Decl_s_struct*_tmp13A=(
struct Cyc_Absyn_Decl_s_struct*)_tmp116;if(_tmp13A->tag != 12)goto _LL101;else{
_tmp13B=_tmp13A->f1;_tmp13C=_tmp13A->f2;}}_LL100:{void*_tmp13D=_tmp13B->r;struct
Cyc_Absyn_Vardecl*_tmp13F;_LL104: {struct Cyc_Absyn_Var_d_struct*_tmp13E=(struct
Cyc_Absyn_Var_d_struct*)_tmp13D;if(_tmp13E->tag != 0)goto _LL106;else{_tmp13F=
_tmp13E->f1;}}_LL105: if(Cyc_Tovc_elim_array_initializers){void*_tmp140=Cyc_Tcutil_compress(
_tmp13F->type);_LL109: {struct Cyc_Absyn_ArrayType_struct*_tmp141=(struct Cyc_Absyn_ArrayType_struct*)
_tmp140;if(_tmp141->tag != 9)goto _LL10B;}_LL10A: if(_tmp13F->initializer != 0){void*
_tmp142=((struct Cyc_Absyn_Exp*)_check_null(_tmp13F->initializer))->r;struct Cyc_List_List*
_tmp144;_LL10E: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp143=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp142;if(_tmp143->tag != 37)goto _LL110;else{_tmp144=_tmp143->f2;}}_LL10F:
_tmp13F->initializer=0;{struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(_tmp13F->name,
0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(_tmp13C->r,0);{unsigned int i=0;
for(0;_tmp144 != 0;(_tmp144=_tmp144->tl,++ i)){s3=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(arr,Cyc_Absyn_uint_exp(i,0),0),(*((
struct _tuple9*)_tmp144->hd)).f2,0),0),s3,0);}}_tmp13C->r=s3->r;goto _LL10D;};
_LL110:;_LL111: {const char*_tmp1BB;void*_tmp1BA[1];struct Cyc_String_pa_struct
_tmp1B9;struct _dyneither_ptr _tmp145=(_tmp1B9.tag=0,((_tmp1B9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmp1BA[0]=&
_tmp1B9,Cyc_aprintf(((_tmp1BB="|%s|: array type bad initializer",_tag_dyneither(
_tmp1BB,sizeof(char),33))),_tag_dyneither(_tmp1BA,sizeof(void*),1)))))));struct
Cyc_Core_Impossible_struct _tmp1BE;struct Cyc_Core_Impossible_struct*_tmp1BD;(int)
_throw((void*)((_tmp1BD=_cycalloc(sizeof(*_tmp1BD)),((_tmp1BD[0]=((_tmp1BE.tag=
Cyc_Core_Impossible,((_tmp1BE.f1=(struct _dyneither_ptr)_tmp145,_tmp1BE)))),
_tmp1BD)))));}_LL10D:;}goto _LL108;_LL10B:;_LL10C: goto _LL108;_LL108:;}if(_tmp13F->initializer
!= 0)Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp13F->initializer));{
struct Cyc_List_List*_tmp1BF;env.all_locals=((_tmp1BF=_region_malloc(env.rgn,
sizeof(*_tmp1BF)),((_tmp1BF->hd=_tmp13F,((_tmp1BF->tl=env.all_locals,_tmp1BF))))));}
s=_tmp13C;continue;_LL106:;_LL107: goto _LL103;_LL103:;}goto _LL102;_LL101:;_LL102: {
struct Cyc_Core_Impossible_struct _tmp1C5;const char*_tmp1C4;struct Cyc_Core_Impossible_struct*
_tmp1C3;(int)_throw((void*)((_tmp1C3=_cycalloc(sizeof(*_tmp1C3)),((_tmp1C3[0]=((
_tmp1C5.tag=Cyc_Core_Impossible,((_tmp1C5.f1=((_tmp1C4="bad stmt after xlation to C",
_tag_dyneither(_tmp1C4,sizeof(char),28))),_tmp1C5)))),_tmp1C3)))));}_LLE4:;};}}
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds);struct Cyc_List_List*
Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){struct Cyc_List_List*_tmp14F=0;for(0;
old_ds != 0;old_ds=old_ds->tl){struct Cyc_Absyn_Decl*_tmp150=(struct Cyc_Absyn_Decl*)
old_ds->hd;void*_tmp151=_tmp150->r;struct Cyc_Absyn_Fndecl*_tmp153;struct Cyc_List_List*
_tmp15E;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp162;struct Cyc_List_List*
_tmp164;_LL113: {struct Cyc_Absyn_Fn_d_struct*_tmp152=(struct Cyc_Absyn_Fn_d_struct*)
_tmp151;if(_tmp152->tag != 1)goto _LL115;else{_tmp153=_tmp152->f1;}}_LL114: _tmp153->cached_typ=
0;{struct Cyc_Absyn_Vardecl*_tmp1C6;struct Cyc_Absyn_Vardecl*vd=(_tmp1C6=_cycalloc(
sizeof(*_tmp1C6)),((_tmp1C6->sc=_tmp153->sc,((_tmp1C6->name=_tmp153->name,((
_tmp1C6->tq=Cyc_Absyn_const_tqual(0),((_tmp1C6->type=Cyc_Tcutil_fndecl2typ(
_tmp153),((_tmp1C6->initializer=0,((_tmp1C6->rgn=0,((_tmp1C6->attributes=_tmp153->attributes,((
_tmp1C6->escapes=0,_tmp1C6)))))))))))))))));{struct Cyc_Absyn_Var_d_struct*
_tmp1CC;struct Cyc_Absyn_Var_d_struct _tmp1CB;struct Cyc_List_List*_tmp1CA;_tmp14F=((
_tmp1CA=_cycalloc(sizeof(*_tmp1CA)),((_tmp1CA->hd=Cyc_Absyn_new_decl((void*)((
_tmp1CC=_cycalloc(sizeof(*_tmp1CC)),((_tmp1CC[0]=((_tmp1CB.tag=0,((_tmp1CB.f1=vd,
_tmp1CB)))),_tmp1CC)))),0),((_tmp1CA->tl=_tmp14F,_tmp1CA))))));}if(_tmp153->param_vardecls
!= 0){struct _RegionHandle _tmp16A=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp16A;_push_region(rgn);{struct Cyc_List_List**_tmp1CD;struct Cyc_List_List**
_tmp16B=(_tmp1CD=_region_malloc(rgn,sizeof(*_tmp1CD)),((_tmp1CD[0]=0,_tmp1CD)));
if(_tmp153->param_vardecls == 0){{const char*_tmp1D1;void*_tmp1D0[1];struct Cyc_String_pa_struct
_tmp1CF;(_tmp1CF.tag=0,((_tmp1CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp153->name).f2),((_tmp1D0[0]=& _tmp1CF,Cyc_fprintf(Cyc_stderr,((_tmp1D1="No param vardecls for function %s\n",
_tag_dyneither(_tmp1D1,sizeof(char),35))),_tag_dyneither(_tmp1D0,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}{struct Cyc_Tovc_ToExpEnv _tmp1D2;
Cyc_Tovc_stmt_to_vc(((_tmp1D2.rgn=rgn,((_tmp1D2.all_locals=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp153->param_vardecls))->v),((_tmp1D2.varmap=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Dict_rempty)(rgn,Cyc_strptrcmp),((_tmp1D2.encloser=
_tmp153->body,((_tmp1D2.gen_ds=_tmp16B,_tmp1D2)))))))))),_tmp153->body);}_tmp14F=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*
_tmp16B,_tmp14F);};_pop_region(rgn);}goto _LL116;};_LL115: {struct Cyc_Absyn_Var_d_struct*
_tmp154=(struct Cyc_Absyn_Var_d_struct*)_tmp151;if(_tmp154->tag != 0)goto _LL117;}
_LL116: goto _LL118;_LL117: {struct Cyc_Absyn_Let_d_struct*_tmp155=(struct Cyc_Absyn_Let_d_struct*)
_tmp151;if(_tmp155->tag != 2)goto _LL119;}_LL118: goto _LL11A;_LL119: {struct Cyc_Absyn_Letv_d_struct*
_tmp156=(struct Cyc_Absyn_Letv_d_struct*)_tmp151;if(_tmp156->tag != 3)goto _LL11B;}
_LL11A: goto _LL11C;_LL11B: {struct Cyc_Absyn_Region_d_struct*_tmp157=(struct Cyc_Absyn_Region_d_struct*)
_tmp151;if(_tmp157->tag != 4)goto _LL11D;}_LL11C: goto _LL11E;_LL11D: {struct Cyc_Absyn_Alias_d_struct*
_tmp158=(struct Cyc_Absyn_Alias_d_struct*)_tmp151;if(_tmp158->tag != 5)goto _LL11F;}
_LL11E: goto _LL120;_LL11F: {struct Cyc_Absyn_Aggr_d_struct*_tmp159=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp151;if(_tmp159->tag != 6)goto _LL121;}_LL120: goto _LL122;_LL121: {struct Cyc_Absyn_Datatype_d_struct*
_tmp15A=(struct Cyc_Absyn_Datatype_d_struct*)_tmp151;if(_tmp15A->tag != 7)goto
_LL123;}_LL122: goto _LL124;_LL123: {struct Cyc_Absyn_Enum_d_struct*_tmp15B=(struct
Cyc_Absyn_Enum_d_struct*)_tmp151;if(_tmp15B->tag != 8)goto _LL125;}_LL124: goto
_LL126;_LL125: {struct Cyc_Absyn_Typedef_d_struct*_tmp15C=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp151;if(_tmp15C->tag != 9)goto _LL127;}_LL126:{struct Cyc_List_List*_tmp1D3;
_tmp14F=((_tmp1D3=_cycalloc(sizeof(*_tmp1D3)),((_tmp1D3->hd=_tmp150,((_tmp1D3->tl=
_tmp14F,_tmp1D3))))));}goto _LL112;_LL127: {struct Cyc_Absyn_Namespace_d_struct*
_tmp15D=(struct Cyc_Absyn_Namespace_d_struct*)_tmp151;if(_tmp15D->tag != 10)goto
_LL129;else{_tmp15E=_tmp15D->f2;}}_LL128: _tmp160=_tmp15E;goto _LL12A;_LL129: {
struct Cyc_Absyn_Using_d_struct*_tmp15F=(struct Cyc_Absyn_Using_d_struct*)_tmp151;
if(_tmp15F->tag != 11)goto _LL12B;else{_tmp160=_tmp15F->f2;}}_LL12A: _tmp162=
_tmp160;goto _LL12C;_LL12B: {struct Cyc_Absyn_ExternC_d_struct*_tmp161=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp151;if(_tmp161->tag != 12)goto _LL12D;else{_tmp162=_tmp161->f1;}}_LL12C:
_tmp164=_tmp162;goto _LL12E;_LL12D: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp163=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp151;if(_tmp163->tag != 13)
goto _LL12F;else{_tmp164=_tmp163->f1;}}_LL12E: goto _LL130;_LL12F: {struct Cyc_Absyn_Porton_d_struct*
_tmp165=(struct Cyc_Absyn_Porton_d_struct*)_tmp151;if(_tmp165->tag != 14)goto
_LL131;}_LL130: goto _LL132;_LL131: {struct Cyc_Absyn_Portoff_d_struct*_tmp166=(
struct Cyc_Absyn_Portoff_d_struct*)_tmp151;if(_tmp166->tag != 15)goto _LL112;}
_LL132: {struct Cyc_Core_Impossible_struct _tmp1D9;const char*_tmp1D8;struct Cyc_Core_Impossible_struct*
_tmp1D7;(int)_throw((void*)((_tmp1D7=_cycalloc(sizeof(*_tmp1D7)),((_tmp1D7[0]=((
_tmp1D9.tag=Cyc_Core_Impossible,((_tmp1D9.f1=((_tmp1D8="nested translation unit after translation to C",
_tag_dyneither(_tmp1D8,sizeof(char),47))),_tmp1D9)))),_tmp1D7)))));}_LL112:;}
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14F);}
