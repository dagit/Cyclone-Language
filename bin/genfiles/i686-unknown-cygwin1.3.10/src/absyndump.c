#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[52];
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
char*tag;};struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};int Cyc_file_string_write(struct Cyc___cycFILE*,struct
_dyneither_ptr src,int src_offset,int max_count);struct Cyc_List_List{void*hd;struct
Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
unsigned int Cyc_strlen(struct _dyneither_ptr s);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _dyneither_ptr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*
loc);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
int tag;struct _dyneither_ptr f1;};extern struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_struct Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct
Cyc_Absyn_Fastcall_att_val;extern struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val;
struct Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union
Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*f3;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _tuple4{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};
struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple6 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
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
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
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
char*tag;};struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct _dyneither_ptr
Cyc_Absyn_attribute2string(void*);struct _tuple9{enum Cyc_Absyn_AggrKind f1;struct
_tuple0*f2;};struct _tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);int Cyc_Absynpp_is_anon_aggrtype(void*t);extern
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple10{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple10*arg);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);struct _tuple12{unsigned int f1;int f2;};struct _tuple12 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};
extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};extern
char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct _tuple13{
void*f1;void*f2;};struct _tuple13*Cyc_Dict_rchoose(struct _RegionHandle*r,struct
Cyc_Dict_Dict d);struct _tuple13*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict
d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};extern
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;static int Cyc_Absyndump_expand_typedefs;
static int Cyc_Absyndump_qvar_to_Cids;static int Cyc_Absyndump_add_cyc_prefix;static
int Cyc_Absyndump_generate_line_directives;static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*);void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(
void*),void*a);void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(
void*),void*);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);void
Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);void Cyc_Absyndump_dumploc(
struct Cyc_Position_Segment*);struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
void Cyc_Absyndump_ignore(void*x){return;}static unsigned int Cyc_Absyndump_pos=0;
static char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0: goto _LL1;case '}': _LL1: goto _LL2;case
'(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;case '[': _LL4: goto _LL5;case ']': _LL5: goto
_LL6;case ';': _LL6: goto _LL7;case ',': _LL7: goto _LL8;case '=': _LL8: goto _LL9;case '?':
_LL9: goto _LLA;case '!': _LLA: goto _LLB;case ' ': _LLB: goto _LLC;case '\n': _LLC: goto _LLD;
case '*': _LLD: return 0;default: _LLE: return 1;}}void Cyc_Absyndump_dump(struct
_dyneither_ptr s){int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos
+=sz;if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_fputc((int)'\n',*
Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())Cyc_fputc((
int)' ',*Cyc_Absyndump_dump_file);}if(sz >= 1){Cyc_Absyndump_prev_char=*((const
char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));Cyc_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);}}void Cyc_Absyndump_dump_nospace(struct
_dyneither_ptr s){int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos
+=sz;if(sz >= 1){Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);Cyc_Absyndump_prev_char=*((
const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}void Cyc_Absyndump_dump_char(
int c){++ Cyc_Absyndump_pos;Cyc_fputc(c,*Cyc_Absyndump_dump_file);Cyc_Absyndump_prev_char=(
char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*loc){if(loc == 0)
return;if(!Cyc_Absyndump_generate_line_directives)return;{struct _dyneither_ptr
_tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(_tmp0);};}void Cyc_Absyndump_dump_str(
struct _dyneither_ptr*s){Cyc_Absyndump_dump(*s);}void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(void(*f)(void*),
struct Cyc_List_List*l,struct _dyneither_ptr sep){if(l == 0)return;for(0;l->tl != 0;l=
l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((void*)l->hd);}void Cyc_Absyndump_dump_sep_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)return;for(0;l->tl != 0;l=l->tl){f(env,(void*)l->hd);Cyc_Absyndump_dump_nospace(
sep);}f(env,(void*)l->hd);}void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep(f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,
struct _dyneither_ptr end,struct _dyneither_ptr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep_c(f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(struct _tuple0*
v){struct Cyc_List_List*_tmp1=0;struct _dyneither_ptr**prefix=0;{union Cyc_Absyn_Nmspace
_tmp2=(*v).f1;int _tmp3;struct Cyc_List_List*_tmp4;struct Cyc_List_List*_tmp5;_LL11:
if((_tmp2.Loc_n).tag != 3)goto _LL13;_tmp3=(int)(_tmp2.Loc_n).val;_LL12: _tmp4=0;
goto _LL14;_LL13: if((_tmp2.Rel_n).tag != 1)goto _LL15;_tmp4=(struct Cyc_List_List*)(
_tmp2.Rel_n).val;_LL14: _tmp1=_tmp4;goto _LL10;_LL15: if((_tmp2.Abs_n).tag != 2)goto
_LL10;_tmp5=(struct Cyc_List_List*)(_tmp2.Abs_n).val;_LL16: if(Cyc_Absyndump_qvar_to_Cids
 && Cyc_Absyndump_add_cyc_prefix)prefix=(struct _dyneither_ptr**)& Cyc_Absynpp_cyc_stringptr;
_tmp1=_tmp5;goto _LL10;_LL10:;}if(prefix != 0){Cyc_Absyndump_dump_str(*prefix);if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{Cyc_Absyndump_dump_nospace(({
const char*_tmp6="::";_tag_dyneither(_tmp6,sizeof(char),3);}));}}if(_tmp1 != 0){
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmp1->hd));for(_tmp1=_tmp1->tl;
_tmp1 != 0;_tmp1=_tmp1->tl){if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((
int)'_');else{Cyc_Absyndump_dump_nospace(({const char*_tmp7="::";_tag_dyneither(
_tmp7,sizeof(char),3);}));}Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)
_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_nospace(({const
char*_tmp8="_";_tag_dyneither(_tmp8,sizeof(char),2);}));else{Cyc_Absyndump_dump_nospace(({
const char*_tmp9="::";_tag_dyneither(_tmp9,sizeof(char),3);}));}Cyc_Absyndump_dump_nospace(*(*
v).f2);}else{if(prefix != 0)Cyc_Absyndump_dump_nospace(*(*v).f2);else{Cyc_Absyndump_dump_str((*
v).f2);}}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict)Cyc_Absyndump_dump(({
const char*_tmpA="restrict";_tag_dyneither(_tmpA,sizeof(char),9);}));if(tq.q_volatile)
Cyc_Absyndump_dump(({const char*_tmpB="volatile";_tag_dyneither(_tmpB,sizeof(char),
9);}));if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmpC="const";
_tag_dyneither(_tmpC,sizeof(char),6);}));}void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope
sc){switch(sc){case Cyc_Absyn_Static: _LL17: Cyc_Absyndump_dump(({const char*_tmpD="static";
_tag_dyneither(_tmpD,sizeof(char),7);}));return;case Cyc_Absyn_Public: _LL18:
return;case Cyc_Absyn_Extern: _LL19: Cyc_Absyndump_dump(({const char*_tmpE="extern";
_tag_dyneither(_tmpE,sizeof(char),7);}));return;case Cyc_Absyn_ExternC: _LL1A: Cyc_Absyndump_dump(({
const char*_tmpF="extern \"C\"";_tag_dyneither(_tmpF,sizeof(char),11);}));return;
case Cyc_Absyn_Abstract: _LL1B: Cyc_Absyndump_dump(({const char*_tmp10="abstract";
_tag_dyneither(_tmp10,sizeof(char),9);}));return;case Cyc_Absyn_Register: _LL1C:
Cyc_Absyndump_dump(({const char*_tmp11="register";_tag_dyneither(_tmp11,sizeof(
char),9);}));return;}}void Cyc_Absyndump_dumpkind(enum Cyc_Absyn_Kind k){switch(k){
case Cyc_Absyn_AnyKind: _LL1E: Cyc_Absyndump_dump(({const char*_tmp12="A";
_tag_dyneither(_tmp12,sizeof(char),2);}));return;case Cyc_Absyn_MemKind: _LL1F: Cyc_Absyndump_dump(({
const char*_tmp13="M";_tag_dyneither(_tmp13,sizeof(char),2);}));return;case Cyc_Absyn_BoxKind:
_LL20: Cyc_Absyndump_dump(({const char*_tmp14="B";_tag_dyneither(_tmp14,sizeof(
char),2);}));return;case Cyc_Absyn_RgnKind: _LL21: Cyc_Absyndump_dump(({const char*
_tmp15="R";_tag_dyneither(_tmp15,sizeof(char),2);}));return;case Cyc_Absyn_UniqueRgnKind:
_LL22: Cyc_Absyndump_dump(({const char*_tmp16="UR";_tag_dyneither(_tmp16,sizeof(
char),3);}));return;case Cyc_Absyn_TopRgnKind: _LL23: Cyc_Absyndump_dump(({const
char*_tmp17="TR";_tag_dyneither(_tmp17,sizeof(char),3);}));return;case Cyc_Absyn_EffKind:
_LL24: Cyc_Absyndump_dump(({const char*_tmp18="E";_tag_dyneither(_tmp18,sizeof(
char),2);}));return;case Cyc_Absyn_IntKind: _LL25: Cyc_Absyndump_dump(({const char*
_tmp19="I";_tag_dyneither(_tmp19,sizeof(char),2);}));return;}}void Cyc_Absyndump_dumpaggr_kind(
enum Cyc_Absyn_AggrKind k){switch(k){case Cyc_Absyn_StructA: _LL27: Cyc_Absyndump_dump(({
const char*_tmp1A="struct";_tag_dyneither(_tmp1A,sizeof(char),7);}));return;case
Cyc_Absyn_UnionA: _LL28: Cyc_Absyndump_dump(({const char*_tmp1B="union";
_tag_dyneither(_tmp1B,sizeof(char),6);}));return;}}void Cyc_Absyndump_dumptps(
struct Cyc_List_List*ts){((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptyp,ts,({const char*_tmp1C="<";_tag_dyneither(_tmp1C,sizeof(
char),2);}),({const char*_tmp1D=">";_tag_dyneither(_tmp1D,sizeof(char),2);}),({
const char*_tmp1E=",";_tag_dyneither(_tmp1E,sizeof(char),2);}));}void Cyc_Absyndump_dumptvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);}void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);{void*_tmp1F=Cyc_Absyn_compress_kb(
tv->kind);enum Cyc_Absyn_Kind _tmp21;enum Cyc_Absyn_Kind _tmp25;_LL2B: {struct Cyc_Absyn_Eq_kb_struct*
_tmp20=(struct Cyc_Absyn_Eq_kb_struct*)_tmp1F;if(_tmp20->tag != 0)goto _LL2D;else{
_tmp21=_tmp20->f1;if(_tmp21 != Cyc_Absyn_BoxKind)goto _LL2D;}}_LL2C: goto _LL2E;
_LL2D: {struct Cyc_Absyn_Less_kb_struct*_tmp22=(struct Cyc_Absyn_Less_kb_struct*)
_tmp1F;if(_tmp22->tag != 2)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp23=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp1F;if(_tmp23->tag != 1)goto _LL31;}
_LL30: Cyc_Absyndump_dump(({const char*_tmp26="::?";_tag_dyneither(_tmp26,sizeof(
char),4);}));goto _LL2A;_LL31: {struct Cyc_Absyn_Eq_kb_struct*_tmp24=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp1F;if(_tmp24->tag != 0)goto _LL2A;else{_tmp25=_tmp24->f1;}}_LL32: Cyc_Absyndump_dump(({
const char*_tmp27="::";_tag_dyneither(_tmp27,sizeof(char),3);}));Cyc_Absyndump_dumpkind(
_tmp25);goto _LL2A;_LL2A:;};}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,tvs,({const char*_tmp28="<";_tag_dyneither(_tmp28,sizeof(
char),2);}),({const char*_tmp29=">";_tag_dyneither(_tmp29,sizeof(char),2);}),({
const char*_tmp2A=",";_tag_dyneither(_tmp2A,sizeof(char),2);}));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,tvs,({const char*_tmp2B="<";_tag_dyneither(_tmp2B,
sizeof(char),2);}),({const char*_tmp2C=">";_tag_dyneither(_tmp2C,sizeof(char),2);}),({
const char*_tmp2D=",";_tag_dyneither(_tmp2D,sizeof(char),2);}));}struct _tuple14{
struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(struct _tuple14*pr){((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,({const char*_tmp2E="(";_tag_dyneither(_tmp2E,sizeof(
char),2);}),({const char*_tmp2F=")";_tag_dyneither(_tmp2F,sizeof(char),2);}),({
const char*_tmp30=",";_tag_dyneither(_tmp30,sizeof(char),2);}));}void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp31=(void*)atts->hd;
_LL34: {struct Cyc_Absyn_Stdcall_att_struct*_tmp32=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp31;if(_tmp32->tag != 1)goto _LL36;}_LL35: Cyc_Absyndump_dump(({const char*_tmp35="_stdcall";
_tag_dyneither(_tmp35,sizeof(char),9);}));return;_LL36: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp33=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp31;if(_tmp33->tag != 2)goto _LL38;}
_LL37: Cyc_Absyndump_dump(({const char*_tmp36="_cdecl";_tag_dyneither(_tmp36,
sizeof(char),7);}));return;_LL38: {struct Cyc_Absyn_Fastcall_att_struct*_tmp34=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp31;if(_tmp34->tag != 3)goto _LL3A;}_LL39:
Cyc_Absyndump_dump(({const char*_tmp37="_fastcall";_tag_dyneither(_tmp37,sizeof(
char),10);}));return;_LL3A:;_LL3B: goto _LL33;_LL33:;}}void Cyc_Absyndump_dump_noncallconv(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp38=(void*)((struct Cyc_List_List*)_check_null(
atts))->hd;_LL3D: {struct Cyc_Absyn_Stdcall_att_struct*_tmp39=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp38;if(_tmp39->tag != 1)goto _LL3F;}_LL3E: goto _LL40;_LL3F: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp3A=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp38;if(_tmp3A->tag != 2)goto _LL41;}
_LL40: goto _LL42;_LL41: {struct Cyc_Absyn_Fastcall_att_struct*_tmp3B=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp38;if(_tmp3B->tag != 3)goto _LL43;}_LL42: goto _LL3C;_LL43:;_LL44: hasatt=1;goto
_LL3C;_LL3C:;}}if(!hasatt)return;Cyc_Absyndump_dump(({const char*_tmp3C="__declspec(";
_tag_dyneither(_tmp3C,sizeof(char),12);}));for(0;atts != 0;atts=atts->tl){void*
_tmp3D=(void*)atts->hd;_LL46: {struct Cyc_Absyn_Stdcall_att_struct*_tmp3E=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL48;}_LL47: goto
_LL49;_LL48: {struct Cyc_Absyn_Cdecl_att_struct*_tmp3F=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp3D;if(_tmp3F->tag != 2)goto _LL4A;}_LL49: goto _LL4B;_LL4A: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp40=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp3D;if(_tmp40->tag != 3)goto _LL4C;}
_LL4B: goto _LL45;_LL4C:;_LL4D: Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)
atts->hd));goto _LL45;_LL45:;}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL4E: Cyc_Absyndump_dump(({const char*_tmp41=" __attribute__((";
_tag_dyneither(_tmp41,sizeof(char),17);}));for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0)Cyc_Absyndump_dump(({
const char*_tmp42=",";_tag_dyneither(_tmp42,sizeof(char),2);}));}Cyc_Absyndump_dump(({
const char*_tmp43=")) ";_tag_dyneither(_tmp43,sizeof(char),4);}));return;case Cyc_Cyclone_Vc_c:
_LL4F: Cyc_Absyndump_dump_noncallconv(atts);return;}}int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp44=(void*)tms->hd;_LL52: {
struct Cyc_Absyn_Pointer_mod_struct*_tmp45=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp44;if(_tmp45->tag != 2)goto _LL54;}_LL53: return 1;_LL54:;_LL55: return 0;_LL51:;};}
static void Cyc_Absyndump_dumprgn(void*t){void*_tmp46=Cyc_Tcutil_compress(t);_LL57: {
struct Cyc_Absyn_HeapRgn_struct*_tmp47=(struct Cyc_Absyn_HeapRgn_struct*)_tmp46;
if(_tmp47->tag != 21)goto _LL59;}_LL58: Cyc_Absyndump_dump(({const char*_tmp48="`H";
_tag_dyneither(_tmp48,sizeof(char),3);}));goto _LL56;_LL59:;_LL5A: Cyc_Absyndump_dumpntyp(
t);goto _LL56;_LL56:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};static struct _tuple15 Cyc_Absyndump_effects_split(void*t){struct Cyc_List_List*
rgions=0;struct Cyc_List_List*effects=0;{void*_tmp49=Cyc_Tcutil_compress(t);void*
_tmp4B;struct Cyc_List_List*_tmp4D;_LL5C: {struct Cyc_Absyn_AccessEff_struct*
_tmp4A=(struct Cyc_Absyn_AccessEff_struct*)_tmp49;if(_tmp4A->tag != 23)goto _LL5E;
else{_tmp4B=(void*)_tmp4A->f1;}}_LL5D: rgions=({struct Cyc_List_List*_tmp4E=
_cycalloc(sizeof(*_tmp4E));_tmp4E->hd=_tmp4B;_tmp4E->tl=rgions;_tmp4E;});goto
_LL5B;_LL5E: {struct Cyc_Absyn_JoinEff_struct*_tmp4C=(struct Cyc_Absyn_JoinEff_struct*)
_tmp49;if(_tmp4C->tag != 24)goto _LL60;else{_tmp4D=_tmp4C->f1;}}_LL5F: for(0;_tmp4D
!= 0;_tmp4D=_tmp4D->tl){struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp51;
struct _tuple15 _tmp4F=Cyc_Absyndump_effects_split((void*)_tmp4D->hd);_tmp50=
_tmp4F.f1;_tmp51=_tmp4F.f2;rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp50,rgions);effects=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp51,effects);}
goto _LL5B;_LL60:;_LL61: effects=({struct Cyc_List_List*_tmp52=_cycalloc(sizeof(*
_tmp52));_tmp52->hd=t;_tmp52->tl=effects;_tmp52;});goto _LL5B;_LL5B:;}return({
struct _tuple15 _tmp53;_tmp53.f1=rgions;_tmp53.f2=effects;_tmp53;});}static void Cyc_Absyndump_dumpeff(
void*t){struct Cyc_List_List*_tmp55;struct Cyc_List_List*_tmp56;struct _tuple15
_tmp54=Cyc_Absyndump_effects_split(t);_tmp55=_tmp54.f1;_tmp56=_tmp54.f2;_tmp55=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp55);_tmp56=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp56);for(0;
_tmp56 != 0;_tmp56=_tmp56->tl){Cyc_Absyndump_dumpntyp((void*)_tmp56->hd);Cyc_Absyndump_dump_char((
int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;_tmp55 != 0;_tmp55=_tmp55->tl){
Cyc_Absyndump_dumprgn((void*)_tmp55->hd);if(_tmp55->tl != 0)Cyc_Absyndump_dump_char((
int)',');}Cyc_Absyndump_dump_char((int)'}');}void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp57=t;struct Cyc_Absyn_Tvar*_tmp5D;struct Cyc_Core_Opt*_tmp5F;struct Cyc_Core_Opt*
_tmp60;int _tmp61;struct Cyc_Core_Opt*_tmp63;struct Cyc_Core_Opt*_tmp64;struct Cyc_Core_Opt
_tmp65;void*_tmp66;int _tmp67;struct Cyc_Absyn_DatatypeInfo _tmp69;union Cyc_Absyn_DatatypeInfoU
_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_Absyn_DatatypeFieldInfo _tmp6D;union
Cyc_Absyn_DatatypeFieldInfoU _tmp6E;struct Cyc_List_List*_tmp6F;enum Cyc_Absyn_Sign
_tmp71;enum Cyc_Absyn_Size_of _tmp72;enum Cyc_Absyn_Sign _tmp74;enum Cyc_Absyn_Size_of
_tmp75;enum Cyc_Absyn_Sign _tmp77;enum Cyc_Absyn_Size_of _tmp78;enum Cyc_Absyn_Sign
_tmp7A;enum Cyc_Absyn_Size_of _tmp7B;enum Cyc_Absyn_Sign _tmp7D;enum Cyc_Absyn_Size_of
_tmp7E;enum Cyc_Absyn_Sign _tmp80;enum Cyc_Absyn_Size_of _tmp81;enum Cyc_Absyn_Sign
_tmp83;enum Cyc_Absyn_Size_of _tmp84;enum Cyc_Absyn_Sign _tmp86;enum Cyc_Absyn_Size_of
_tmp87;enum Cyc_Absyn_Sign _tmp89;enum Cyc_Absyn_Size_of _tmp8A;enum Cyc_Absyn_Sign
_tmp8C;enum Cyc_Absyn_Size_of _tmp8D;enum Cyc_Absyn_Sign _tmp8F;enum Cyc_Absyn_Size_of
_tmp90;enum Cyc_Absyn_Sign _tmp92;enum Cyc_Absyn_Size_of _tmp93;enum Cyc_Absyn_Sign
_tmp95;enum Cyc_Absyn_Size_of _tmp96;enum Cyc_Absyn_Sign _tmp98;enum Cyc_Absyn_Size_of
_tmp99;enum Cyc_Absyn_Sign _tmp9B;enum Cyc_Absyn_Size_of _tmp9C;int _tmp9F;struct
Cyc_List_List*_tmpA1;struct Cyc_Absyn_AggrInfo _tmpA3;union Cyc_Absyn_AggrInfoU
_tmpA4;struct Cyc_List_List*_tmpA5;enum Cyc_Absyn_AggrKind _tmpA7;struct Cyc_List_List*
_tmpA8;struct _tuple0*_tmpAA;struct Cyc_List_List*_tmpAC;struct _tuple0*_tmpAE;
struct Cyc_List_List*_tmpAF;struct Cyc_Absyn_Exp*_tmpB1;void*_tmpB3;void*_tmpB5;
void*_tmpB6;void*_tmpB8;_LL63: {struct Cyc_Absyn_ArrayType_struct*_tmp58=(struct
Cyc_Absyn_ArrayType_struct*)_tmp57;if(_tmp58->tag != 9)goto _LL65;}_LL64: goto _LL66;
_LL65: {struct Cyc_Absyn_FnType_struct*_tmp59=(struct Cyc_Absyn_FnType_struct*)
_tmp57;if(_tmp59->tag != 10)goto _LL67;}_LL66: goto _LL68;_LL67: {struct Cyc_Absyn_PointerType_struct*
_tmp5A=(struct Cyc_Absyn_PointerType_struct*)_tmp57;if(_tmp5A->tag != 5)goto _LL69;}
_LL68: return;_LL69: {struct Cyc_Absyn_VoidType_struct*_tmp5B=(struct Cyc_Absyn_VoidType_struct*)
_tmp57;if(_tmp5B->tag != 0)goto _LL6B;}_LL6A: Cyc_Absyndump_dump(({const char*_tmpBE="void";
_tag_dyneither(_tmpBE,sizeof(char),5);}));return;_LL6B: {struct Cyc_Absyn_VarType_struct*
_tmp5C=(struct Cyc_Absyn_VarType_struct*)_tmp57;if(_tmp5C->tag != 2)goto _LL6D;
else{_tmp5D=_tmp5C->f1;}}_LL6C: Cyc_Absyndump_dump_str(_tmp5D->name);return;_LL6D: {
struct Cyc_Absyn_Evar_struct*_tmp5E=(struct Cyc_Absyn_Evar_struct*)_tmp57;if(
_tmp5E->tag != 1)goto _LL6F;else{_tmp5F=_tmp5E->f1;_tmp60=_tmp5E->f2;if(_tmp60 != 0)
goto _LL6F;_tmp61=_tmp5E->f3;}}_LL6E: Cyc_Absyndump_dump(({const char*_tmpBF="%";
_tag_dyneither(_tmpBF,sizeof(char),2);}));if(_tmp5F == 0)Cyc_Absyndump_dump(({
const char*_tmpC0="?";_tag_dyneither(_tmpC0,sizeof(char),2);}));else{Cyc_Absyndump_dumpkind((
enum Cyc_Absyn_Kind)_tmp5F->v);}Cyc_Absyndump_dump((struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmpC3;_tmpC3.tag=1;_tmpC3.f1=(unsigned long)_tmp61;({
void*_tmpC1[1]={& _tmpC3};Cyc_aprintf(({const char*_tmpC2="(%d)";_tag_dyneither(
_tmpC2,sizeof(char),5);}),_tag_dyneither(_tmpC1,sizeof(void*),1));});}));return;
_LL6F: {struct Cyc_Absyn_Evar_struct*_tmp62=(struct Cyc_Absyn_Evar_struct*)_tmp57;
if(_tmp62->tag != 1)goto _LL71;else{_tmp63=_tmp62->f1;_tmp64=_tmp62->f2;if(_tmp64
== 0)goto _LL71;_tmp65=*_tmp64;_tmp66=(void*)_tmp65.v;_tmp67=_tmp62->f3;}}_LL70:
Cyc_Absyndump_dumpntyp(_tmp66);return;_LL71: {struct Cyc_Absyn_DatatypeType_struct*
_tmp68=(struct Cyc_Absyn_DatatypeType_struct*)_tmp57;if(_tmp68->tag != 3)goto _LL73;
else{_tmp69=_tmp68->f1;_tmp6A=_tmp69.datatype_info;_tmp6B=_tmp69.targs;}}_LL72:{
union Cyc_Absyn_DatatypeInfoU _tmpC4=_tmp6A;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpC5;struct _tuple0*_tmpC6;int _tmpC7;struct Cyc_Absyn_Datatypedecl**_tmpC8;
struct Cyc_Absyn_Datatypedecl*_tmpC9;struct Cyc_Absyn_Datatypedecl _tmpCA;struct
_tuple0*_tmpCB;int _tmpCC;_LLB6: if((_tmpC4.UnknownDatatype).tag != 1)goto _LLB8;
_tmpC5=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC4.UnknownDatatype).val;_tmpC6=
_tmpC5.name;_tmpC7=_tmpC5.is_extensible;_LLB7: _tmpCB=_tmpC6;_tmpCC=_tmpC7;goto
_LLB9;_LLB8: if((_tmpC4.KnownDatatype).tag != 2)goto _LLB5;_tmpC8=(struct Cyc_Absyn_Datatypedecl**)(
_tmpC4.KnownDatatype).val;_tmpC9=*_tmpC8;_tmpCA=*_tmpC9;_tmpCB=_tmpCA.name;
_tmpCC=_tmpCA.is_extensible;_LLB9: if(_tmpCC)Cyc_Absyndump_dump(({const char*
_tmpCD="@extensible ";_tag_dyneither(_tmpCD,sizeof(char),13);}));Cyc_Absyndump_dump(({
const char*_tmpCE="datatype ";_tag_dyneither(_tmpCE,sizeof(char),10);}));Cyc_Absyndump_dumpqvar(
_tmpCB);Cyc_Absyndump_dumptps(_tmp6B);goto _LLB5;_LLB5:;}goto _LL62;_LL73: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp6C=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp57;if(_tmp6C->tag != 4)goto _LL75;else{_tmp6D=_tmp6C->f1;_tmp6E=_tmp6D.field_info;
_tmp6F=_tmp6D.targs;}}_LL74:{union Cyc_Absyn_DatatypeFieldInfoU _tmpCF=_tmp6E;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpD0;struct _tuple0*_tmpD1;struct
_tuple0*_tmpD2;int _tmpD3;struct _tuple1 _tmpD4;struct Cyc_Absyn_Datatypedecl*_tmpD5;
struct Cyc_Absyn_Datatypedecl _tmpD6;struct _tuple0*_tmpD7;int _tmpD8;struct Cyc_Absyn_Datatypefield*
_tmpD9;struct Cyc_Absyn_Datatypefield _tmpDA;struct _tuple0*_tmpDB;_LLBB: if((_tmpCF.UnknownDatatypefield).tag
!= 1)goto _LLBD;_tmpD0=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpCF.UnknownDatatypefield).val;
_tmpD1=_tmpD0.datatype_name;_tmpD2=_tmpD0.field_name;_tmpD3=_tmpD0.is_extensible;
_LLBC: _tmpD7=_tmpD1;_tmpD8=_tmpD3;_tmpDB=_tmpD2;goto _LLBE;_LLBD: if((_tmpCF.KnownDatatypefield).tag
!= 2)goto _LLBA;_tmpD4=(struct _tuple1)(_tmpCF.KnownDatatypefield).val;_tmpD5=
_tmpD4.f1;_tmpD6=*_tmpD5;_tmpD7=_tmpD6.name;_tmpD8=_tmpD6.is_extensible;_tmpD9=
_tmpD4.f2;_tmpDA=*_tmpD9;_tmpDB=_tmpDA.name;_LLBE: if(_tmpD8)Cyc_Absyndump_dump(({
const char*_tmpDC="@extensible ";_tag_dyneither(_tmpDC,sizeof(char),13);}));Cyc_Absyndump_dump(({
const char*_tmpDD="datatype ";_tag_dyneither(_tmpDD,sizeof(char),10);}));Cyc_Absyndump_dumpqvar(
_tmpD7);Cyc_Absyndump_dump(({const char*_tmpDE=".";_tag_dyneither(_tmpDE,sizeof(
char),2);}));Cyc_Absyndump_dumpqvar(_tmpDB);Cyc_Absyndump_dumptps(_tmp6F);goto
_LLBA;_LLBA:;}goto _LL62;_LL75: {struct Cyc_Absyn_IntType_struct*_tmp70=(struct Cyc_Absyn_IntType_struct*)
_tmp57;if(_tmp70->tag != 6)goto _LL77;else{_tmp71=_tmp70->f1;if(_tmp71 != Cyc_Absyn_None)
goto _LL77;_tmp72=_tmp70->f2;if(_tmp72 != Cyc_Absyn_Int_sz)goto _LL77;}}_LL76: goto
_LL78;_LL77: {struct Cyc_Absyn_IntType_struct*_tmp73=(struct Cyc_Absyn_IntType_struct*)
_tmp57;if(_tmp73->tag != 6)goto _LL79;else{_tmp74=_tmp73->f1;if(_tmp74 != Cyc_Absyn_Signed)
goto _LL79;_tmp75=_tmp73->f2;if(_tmp75 != Cyc_Absyn_Int_sz)goto _LL79;}}_LL78: Cyc_Absyndump_dump(({
const char*_tmpDF="int";_tag_dyneither(_tmpDF,sizeof(char),4);}));return;_LL79: {
struct Cyc_Absyn_IntType_struct*_tmp76=(struct Cyc_Absyn_IntType_struct*)_tmp57;
if(_tmp76->tag != 6)goto _LL7B;else{_tmp77=_tmp76->f1;if(_tmp77 != Cyc_Absyn_None)
goto _LL7B;_tmp78=_tmp76->f2;if(_tmp78 != Cyc_Absyn_Long_sz)goto _LL7B;}}_LL7A: goto
_LL7C;_LL7B: {struct Cyc_Absyn_IntType_struct*_tmp79=(struct Cyc_Absyn_IntType_struct*)
_tmp57;if(_tmp79->tag != 6)goto _LL7D;else{_tmp7A=_tmp79->f1;if(_tmp7A != Cyc_Absyn_Signed)
goto _LL7D;_tmp7B=_tmp79->f2;if(_tmp7B != Cyc_Absyn_Long_sz)goto _LL7D;}}_LL7C: Cyc_Absyndump_dump(({
const char*_tmpE0="long";_tag_dyneither(_tmpE0,sizeof(char),5);}));return;_LL7D: {
struct Cyc_Absyn_IntType_struct*_tmp7C=(struct Cyc_Absyn_IntType_struct*)_tmp57;
if(_tmp7C->tag != 6)goto _LL7F;else{_tmp7D=_tmp7C->f1;if(_tmp7D != Cyc_Absyn_None)
goto _LL7F;_tmp7E=_tmp7C->f2;if(_tmp7E != Cyc_Absyn_Char_sz)goto _LL7F;}}_LL7E: Cyc_Absyndump_dump(({
const char*_tmpE1="char";_tag_dyneither(_tmpE1,sizeof(char),5);}));return;_LL7F: {
struct Cyc_Absyn_IntType_struct*_tmp7F=(struct Cyc_Absyn_IntType_struct*)_tmp57;
if(_tmp7F->tag != 6)goto _LL81;else{_tmp80=_tmp7F->f1;if(_tmp80 != Cyc_Absyn_Signed)
goto _LL81;_tmp81=_tmp7F->f2;if(_tmp81 != Cyc_Absyn_Char_sz)goto _LL81;}}_LL80: Cyc_Absyndump_dump(({
const char*_tmpE2="signed char";_tag_dyneither(_tmpE2,sizeof(char),12);}));
return;_LL81: {struct Cyc_Absyn_IntType_struct*_tmp82=(struct Cyc_Absyn_IntType_struct*)
_tmp57;if(_tmp82->tag != 6)goto _LL83;else{_tmp83=_tmp82->f1;if(_tmp83 != Cyc_Absyn_Unsigned)
goto _LL83;_tmp84=_tmp82->f2;if(_tmp84 != Cyc_Absyn_Char_sz)goto _LL83;}}_LL82: Cyc_Absyndump_dump(({
const char*_tmpE3="unsigned char";_tag_dyneither(_tmpE3,sizeof(char),14);}));
return;_LL83: {struct Cyc_Absyn_IntType_struct*_tmp85=(struct Cyc_Absyn_IntType_struct*)
_tmp57;if(_tmp85->tag != 6)goto _LL85;else{_tmp86=_tmp85->f1;if(_tmp86 != Cyc_Absyn_None)
goto _LL85;_tmp87=_tmp85->f2;if(_tmp87 != Cyc_Absyn_Short_sz)goto _LL85;}}_LL84:
goto _LL86;_LL85: {struct Cyc_Absyn_IntType_struct*_tmp88=(struct Cyc_Absyn_IntType_struct*)
_tmp57;if(_tmp88->tag != 6)goto _LL87;else{_tmp89=_tmp88->f1;if(_tmp89 != Cyc_Absyn_Signed)
goto _LL87;_tmp8A=_tmp88->f2;if(_tmp8A != Cyc_Absyn_Short_sz)goto _LL87;}}_LL86: Cyc_Absyndump_dump(({
const char*_tmpE4="short";_tag_dyneither(_tmpE4,sizeof(char),6);}));return;_LL87: {
struct Cyc_Absyn_IntType_struct*_tmp8B=(struct Cyc_Absyn_IntType_struct*)_tmp57;
if(_tmp8B->tag != 6)goto _LL89;else{_tmp8C=_tmp8B->f1;if(_tmp8C != Cyc_Absyn_Unsigned)
goto _LL89;_tmp8D=_tmp8B->f2;if(_tmp8D != Cyc_Absyn_Short_sz)goto _LL89;}}_LL88: Cyc_Absyndump_dump(({
const char*_tmpE5="unsigned short";_tag_dyneither(_tmpE5,sizeof(char),15);}));
return;_LL89: {struct Cyc_Absyn_IntType_struct*_tmp8E=(struct Cyc_Absyn_IntType_struct*)
_tmp57;if(_tmp8E->tag != 6)goto _LL8B;else{_tmp8F=_tmp8E->f1;if(_tmp8F != Cyc_Absyn_Unsigned)
goto _LL8B;_tmp90=_tmp8E->f2;if(_tmp90 != Cyc_Absyn_Int_sz)goto _LL8B;}}_LL8A: Cyc_Absyndump_dump(({
const char*_tmpE6="unsigned int";_tag_dyneither(_tmpE6,sizeof(char),13);}));
return;_LL8B: {struct Cyc_Absyn_IntType_struct*_tmp91=(struct Cyc_Absyn_IntType_struct*)
_tmp57;if(_tmp91->tag != 6)goto _LL8D;else{_tmp92=_tmp91->f1;if(_tmp92 != Cyc_Absyn_Unsigned)
goto _LL8D;_tmp93=_tmp91->f2;if(_tmp93 != Cyc_Absyn_Long_sz)goto _LL8D;}}_LL8C: Cyc_Absyndump_dump(({
const char*_tmpE7="unsigned long";_tag_dyneither(_tmpE7,sizeof(char),14);}));
return;_LL8D: {struct Cyc_Absyn_IntType_struct*_tmp94=(struct Cyc_Absyn_IntType_struct*)
_tmp57;if(_tmp94->tag != 6)goto _LL8F;else{_tmp95=_tmp94->f1;if(_tmp95 != Cyc_Absyn_None)
goto _LL8F;_tmp96=_tmp94->f2;if(_tmp96 != Cyc_Absyn_LongLong_sz)goto _LL8F;}}_LL8E:
goto _LL90;_LL8F: {struct Cyc_Absyn_IntType_struct*_tmp97=(struct Cyc_Absyn_IntType_struct*)
_tmp57;if(_tmp97->tag != 6)goto _LL91;else{_tmp98=_tmp97->f1;if(_tmp98 != Cyc_Absyn_Signed)
goto _LL91;_tmp99=_tmp97->f2;if(_tmp99 != Cyc_Absyn_LongLong_sz)goto _LL91;}}_LL90:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLBF: Cyc_Absyndump_dump(({
const char*_tmpE8="long long";_tag_dyneither(_tmpE8,sizeof(char),10);}));return;
case Cyc_Cyclone_Vc_c: _LLC0: Cyc_Absyndump_dump(({const char*_tmpE9="__int64";
_tag_dyneither(_tmpE9,sizeof(char),8);}));return;}_LL91: {struct Cyc_Absyn_IntType_struct*
_tmp9A=(struct Cyc_Absyn_IntType_struct*)_tmp57;if(_tmp9A->tag != 6)goto _LL93;
else{_tmp9B=_tmp9A->f1;if(_tmp9B != Cyc_Absyn_Unsigned)goto _LL93;_tmp9C=_tmp9A->f2;
if(_tmp9C != Cyc_Absyn_LongLong_sz)goto _LL93;}}_LL92: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Vc_c: _LLC2: Cyc_Absyndump_dump(({const char*_tmpEA="unsigned __int64";
_tag_dyneither(_tmpEA,sizeof(char),17);}));return;case Cyc_Cyclone_Gcc_c: _LLC3:
Cyc_Absyndump_dump(({const char*_tmpEB="unsigned long long";_tag_dyneither(_tmpEB,
sizeof(char),19);}));return;}_LL93: {struct Cyc_Absyn_FloatType_struct*_tmp9D=(
struct Cyc_Absyn_FloatType_struct*)_tmp57;if(_tmp9D->tag != 7)goto _LL95;}_LL94: Cyc_Absyndump_dump(({
const char*_tmpEC="float";_tag_dyneither(_tmpEC,sizeof(char),6);}));return;_LL95: {
struct Cyc_Absyn_DoubleType_struct*_tmp9E=(struct Cyc_Absyn_DoubleType_struct*)
_tmp57;if(_tmp9E->tag != 8)goto _LL97;else{_tmp9F=_tmp9E->f1;}}_LL96: if(_tmp9F)Cyc_Absyndump_dump(({
const char*_tmpED="long double";_tag_dyneither(_tmpED,sizeof(char),12);}));else{
Cyc_Absyndump_dump(({const char*_tmpEE="double";_tag_dyneither(_tmpEE,sizeof(char),
7);}));}return;_LL97: {struct Cyc_Absyn_TupleType_struct*_tmpA0=(struct Cyc_Absyn_TupleType_struct*)
_tmp57;if(_tmpA0->tag != 11)goto _LL99;else{_tmpA1=_tmpA0->f1;}}_LL98: Cyc_Absyndump_dump_char((
int)'$');Cyc_Absyndump_dumpargs(_tmpA1);return;_LL99: {struct Cyc_Absyn_AggrType_struct*
_tmpA2=(struct Cyc_Absyn_AggrType_struct*)_tmp57;if(_tmpA2->tag != 12)goto _LL9B;
else{_tmpA3=_tmpA2->f1;_tmpA4=_tmpA3.aggr_info;_tmpA5=_tmpA3.targs;}}_LL9A: {
enum Cyc_Absyn_AggrKind _tmpF0;struct _tuple0*_tmpF1;struct _tuple9 _tmpEF=Cyc_Absyn_aggr_kinded_name(
_tmpA4);_tmpF0=_tmpEF.f1;_tmpF1=_tmpEF.f2;Cyc_Absyndump_dumpaggr_kind(_tmpF0);
Cyc_Absyndump_dumpqvar(_tmpF1);Cyc_Absyndump_dumptps(_tmpA5);return;}_LL9B: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpA6=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp57;if(_tmpA6->tag != 13)goto _LL9D;else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA6->f2;}}
_LL9C: Cyc_Absyndump_dumpaggr_kind(_tmpA7);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(
_tmpA8);Cyc_Absyndump_dump_char((int)'}');return;_LL9D: {struct Cyc_Absyn_EnumType_struct*
_tmpA9=(struct Cyc_Absyn_EnumType_struct*)_tmp57;if(_tmpA9->tag != 14)goto _LL9F;
else{_tmpAA=_tmpA9->f1;}}_LL9E: Cyc_Absyndump_dump(({const char*_tmpF2="enum ";
_tag_dyneither(_tmpF2,sizeof(char),6);}));Cyc_Absyndump_dumpqvar(_tmpAA);return;
_LL9F: {struct Cyc_Absyn_AnonEnumType_struct*_tmpAB=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp57;if(_tmpAB->tag != 15)goto _LLA1;else{_tmpAC=_tmpAB->f1;}}_LLA0: Cyc_Absyndump_dump(({
const char*_tmpF3="enum {";_tag_dyneither(_tmpF3,sizeof(char),7);}));Cyc_Absyndump_dumpenumfields(
_tmpAC);Cyc_Absyndump_dump(({const char*_tmpF4="}";_tag_dyneither(_tmpF4,sizeof(
char),2);}));return;_LLA1: {struct Cyc_Absyn_TypedefType_struct*_tmpAD=(struct Cyc_Absyn_TypedefType_struct*)
_tmp57;if(_tmpAD->tag != 18)goto _LLA3;else{_tmpAE=_tmpAD->f1;_tmpAF=_tmpAD->f2;}}
_LLA2:(Cyc_Absyndump_dumpqvar(_tmpAE),Cyc_Absyndump_dumptps(_tmpAF));return;
_LLA3: {struct Cyc_Absyn_ValueofType_struct*_tmpB0=(struct Cyc_Absyn_ValueofType_struct*)
_tmp57;if(_tmpB0->tag != 19)goto _LLA5;else{_tmpB1=_tmpB0->f1;}}_LLA4: Cyc_Absyndump_dump(({
const char*_tmpF5="valueof_t(";_tag_dyneither(_tmpF5,sizeof(char),11);}));Cyc_Absyndump_dumpexp(
_tmpB1);Cyc_Absyndump_dump(({const char*_tmpF6=")";_tag_dyneither(_tmpF6,sizeof(
char),2);}));return;_LLA5: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB2=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmp57;if(_tmpB2->tag != 16)goto _LLA7;else{_tmpB3=(
void*)_tmpB2->f1;}}_LLA6: Cyc_Absyndump_dump(({const char*_tmpF7="region_t<";
_tag_dyneither(_tmpF7,sizeof(char),10);}));Cyc_Absyndump_dumprgn(_tmpB3);Cyc_Absyndump_dump(({
const char*_tmpF8=">";_tag_dyneither(_tmpF8,sizeof(char),2);}));return;_LLA7: {
struct Cyc_Absyn_DynRgnType_struct*_tmpB4=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp57;if(_tmpB4->tag != 17)goto _LLA9;else{_tmpB5=(void*)_tmpB4->f1;_tmpB6=(void*)
_tmpB4->f2;}}_LLA8: Cyc_Absyndump_dump(({const char*_tmpF9="dynregion_t<";
_tag_dyneither(_tmpF9,sizeof(char),13);}));Cyc_Absyndump_dumprgn(_tmpB5);Cyc_Absyndump_dump(({
const char*_tmpFA=",";_tag_dyneither(_tmpFA,sizeof(char),2);}));Cyc_Absyndump_dumprgn(
_tmpB6);Cyc_Absyndump_dump(({const char*_tmpFB=">";_tag_dyneither(_tmpFB,sizeof(
char),2);}));return;_LLA9: {struct Cyc_Absyn_TagType_struct*_tmpB7=(struct Cyc_Absyn_TagType_struct*)
_tmp57;if(_tmpB7->tag != 20)goto _LLAB;else{_tmpB8=(void*)_tmpB7->f1;}}_LLAA: Cyc_Absyndump_dump(({
const char*_tmpFC="tag_t<";_tag_dyneither(_tmpFC,sizeof(char),7);}));Cyc_Absyndump_dumpntyp(
_tmpB8);Cyc_Absyndump_dump(({const char*_tmpFD=">";_tag_dyneither(_tmpFD,sizeof(
char),2);}));return;_LLAB: {struct Cyc_Absyn_UniqueRgn_struct*_tmpB9=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp57;if(_tmpB9->tag != 22)goto _LLAD;}_LLAC: Cyc_Absyndump_dump(({const char*
_tmpFE="`U";_tag_dyneither(_tmpFE,sizeof(char),3);}));goto _LL62;_LLAD: {struct
Cyc_Absyn_HeapRgn_struct*_tmpBA=(struct Cyc_Absyn_HeapRgn_struct*)_tmp57;if(
_tmpBA->tag != 21)goto _LLAF;}_LLAE: goto _LLB0;_LLAF: {struct Cyc_Absyn_AccessEff_struct*
_tmpBB=(struct Cyc_Absyn_AccessEff_struct*)_tmp57;if(_tmpBB->tag != 23)goto _LLB1;}
_LLB0: goto _LLB2;_LLB1: {struct Cyc_Absyn_RgnsEff_struct*_tmpBC=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp57;if(_tmpBC->tag != 25)goto _LLB3;}_LLB2: goto _LLB4;_LLB3: {struct Cyc_Absyn_JoinEff_struct*
_tmpBD=(struct Cyc_Absyn_JoinEff_struct*)_tmp57;if(_tmpBD->tag != 24)goto _LL62;}
_LLB4: return;_LL62:;}void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo != 
0)Cyc_Absyndump_dump_str((struct _dyneither_ptr*)vo->v);}void Cyc_Absyndump_dumpfunarg(
struct _tuple7*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*
t).f1);}void Cyc_Absyndump_dump_rgncmp(struct _tuple13*cmp){struct _tuple13 _tmp100;
void*_tmp101;void*_tmp102;struct _tuple13*_tmpFF=cmp;_tmp100=*_tmpFF;_tmp101=
_tmp100.f1;_tmp102=_tmp100.f2;Cyc_Absyndump_dumpeff(_tmp101);Cyc_Absyndump_dump_char((
int)'>');Cyc_Absyndump_dumprgn(_tmp102);}void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*
rgn_po){((void(*)(void(*f)(struct _tuple13*),struct Cyc_List_List*l,struct
_dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,({
const char*_tmp103=",";_tag_dyneither(_tmp103,sizeof(char),2);}));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple7*)
args->hd);if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs)Cyc_Absyndump_dump(({const char*_tmp104="...";
_tag_dyneither(_tmp104,sizeof(char),4);}));else{if(cyc_varargs != 0){struct
_tuple7*_tmp105=({struct _tuple7*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->f1=
cyc_varargs->name;_tmp108->f2=cyc_varargs->tq;_tmp108->f3=cyc_varargs->type;
_tmp108;});Cyc_Absyndump_dump(({const char*_tmp106="...";_tag_dyneither(_tmp106,
sizeof(char),4);}));if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*
_tmp107=" inject ";_tag_dyneither(_tmp107,sizeof(char),9);}));Cyc_Absyndump_dumpfunarg(
_tmp105);}}if(effopt != 0){Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff((void*)
effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(
rgn_po);}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumptyp(void*t){((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(
Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(
void*d){void*_tmp109=d;struct Cyc_Absyn_Exp*_tmp10B;struct _dyneither_ptr*_tmp10D;
_LLC6: {struct Cyc_Absyn_ArrayElement_struct*_tmp10A=(struct Cyc_Absyn_ArrayElement_struct*)
_tmp109;if(_tmp10A->tag != 0)goto _LLC8;else{_tmp10B=_tmp10A->f1;}}_LLC7: Cyc_Absyndump_dump(({
const char*_tmp10E=".[";_tag_dyneither(_tmp10E,sizeof(char),3);}));Cyc_Absyndump_dumpexp(
_tmp10B);Cyc_Absyndump_dump_char((int)']');goto _LLC5;_LLC8: {struct Cyc_Absyn_FieldName_struct*
_tmp10C=(struct Cyc_Absyn_FieldName_struct*)_tmp109;if(_tmp10C->tag != 1)goto _LLC5;
else{_tmp10D=_tmp10C->f1;}}_LLC9: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp10D);goto _LLC5;_LLC5:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple16*de){((void(*)(void(*f)(void*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*de).f1,({
const char*_tmp10F="";_tag_dyneither(_tmp10F,sizeof(char),1);}),({const char*
_tmp110="=";_tag_dyneither(_tmp110,sizeof(char),2);}),({const char*_tmp111="=";
_tag_dyneither(_tmp111,sizeof(char),2);}));Cyc_Absyndump_dumpexp((*de).f2);}void
Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){((void(*)(void(*f)(
int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,
struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,
inprec,es,({const char*_tmp112="";_tag_dyneither(_tmp112,sizeof(char),1);}),({
const char*_tmp113="";_tag_dyneither(_tmp113,sizeof(char),1);}),({const char*
_tmp114=",";_tag_dyneither(_tmp114,sizeof(char),2);}));}void Cyc_Absyndump_dumpexp_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= 
myprec)Cyc_Absyndump_dump_nospace(({const char*_tmp115="(";_tag_dyneither(_tmp115,
sizeof(char),2);}));{void*_tmp116=e->r;union Cyc_Absyn_Cnst _tmp118;struct _tuple3
_tmp119;enum Cyc_Absyn_Sign _tmp11A;char _tmp11B;union Cyc_Absyn_Cnst _tmp11D;struct
_tuple4 _tmp11E;enum Cyc_Absyn_Sign _tmp11F;short _tmp120;union Cyc_Absyn_Cnst
_tmp122;struct _tuple5 _tmp123;enum Cyc_Absyn_Sign _tmp124;int _tmp125;union Cyc_Absyn_Cnst
_tmp127;struct _tuple5 _tmp128;enum Cyc_Absyn_Sign _tmp129;int _tmp12A;union Cyc_Absyn_Cnst
_tmp12C;struct _tuple5 _tmp12D;enum Cyc_Absyn_Sign _tmp12E;int _tmp12F;union Cyc_Absyn_Cnst
_tmp131;struct _tuple6 _tmp132;enum Cyc_Absyn_Sign _tmp133;long long _tmp134;union
Cyc_Absyn_Cnst _tmp136;struct _dyneither_ptr _tmp137;union Cyc_Absyn_Cnst _tmp139;int
_tmp13A;union Cyc_Absyn_Cnst _tmp13C;struct _dyneither_ptr _tmp13D;struct _tuple0*
_tmp13F;struct _tuple0*_tmp141;enum Cyc_Absyn_Primop _tmp143;struct Cyc_List_List*
_tmp144;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Core_Opt*_tmp147;struct Cyc_Absyn_Exp*
_tmp148;struct Cyc_Absyn_Exp*_tmp14A;enum Cyc_Absyn_Incrementor _tmp14B;struct Cyc_Absyn_Exp*
_tmp14D;enum Cyc_Absyn_Incrementor _tmp14E;struct Cyc_Absyn_Exp*_tmp150;enum Cyc_Absyn_Incrementor
_tmp151;struct Cyc_Absyn_Exp*_tmp153;enum Cyc_Absyn_Incrementor _tmp154;struct Cyc_Absyn_Exp*
_tmp156;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*
_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*
_tmp15E;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*
_tmp163;struct Cyc_List_List*_tmp164;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_List_List*
_tmp167;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*
_tmp16D;void*_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*_tmp172;
struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*_tmp175;void*_tmp177;struct Cyc_Absyn_Exp*
_tmp179;void*_tmp17B;struct Cyc_Absyn_Exp*_tmp17D;struct _dyneither_ptr*_tmp17E;
void*_tmp180;void*_tmp181;struct _dyneither_ptr*_tmp183;void*_tmp185;void*_tmp186;
unsigned int _tmp188;struct Cyc_List_List*_tmp18A;void*_tmp18B;struct Cyc_Absyn_Exp*
_tmp18D;struct Cyc_Absyn_Exp*_tmp18F;struct _dyneither_ptr*_tmp190;struct Cyc_Absyn_Exp*
_tmp192;struct _dyneither_ptr*_tmp193;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*
_tmp196;struct Cyc_List_List*_tmp198;struct _tuple7*_tmp19A;struct Cyc_List_List*
_tmp19B;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_Vardecl*_tmp19F;struct Cyc_Absyn_Exp*
_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A1;struct _tuple0*_tmp1A3;struct Cyc_List_List*
_tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_List_List*_tmp1A7;struct Cyc_List_List*
_tmp1A9;struct Cyc_Absyn_Datatypefield*_tmp1AA;struct _tuple0*_tmp1AC;struct
_tuple0*_tmp1AE;struct Cyc_Absyn_MallocInfo _tmp1B0;int _tmp1B1;struct Cyc_Absyn_Exp*
_tmp1B2;void**_tmp1B3;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B6;
struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Core_Opt*_tmp1B9;struct Cyc_List_List*
_tmp1BA;struct Cyc_Absyn_Stmt*_tmp1BC;_LLCB: {struct Cyc_Absyn_Const_e_struct*
_tmp117=(struct Cyc_Absyn_Const_e_struct*)_tmp116;if(_tmp117->tag != 0)goto _LLCD;
else{_tmp118=_tmp117->f1;if((_tmp118.Char_c).tag != 2)goto _LLCD;_tmp119=(struct
_tuple3)(_tmp118.Char_c).val;_tmp11A=_tmp119.f1;_tmp11B=_tmp119.f2;}}_LLCC: Cyc_Absyndump_dump_char((
int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp11B));Cyc_Absyndump_dump_char((
int)'\'');goto _LLCA;_LLCD: {struct Cyc_Absyn_Const_e_struct*_tmp11C=(struct Cyc_Absyn_Const_e_struct*)
_tmp116;if(_tmp11C->tag != 0)goto _LLCF;else{_tmp11D=_tmp11C->f1;if((_tmp11D.Short_c).tag
!= 3)goto _LLCF;_tmp11E=(struct _tuple4)(_tmp11D.Short_c).val;_tmp11F=_tmp11E.f1;
_tmp120=_tmp11E.f2;}}_LLCE: Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp1BF;_tmp1BF.tag=1;_tmp1BF.f1=(unsigned long)((int)_tmp120);({void*_tmp1BD[1]={&
_tmp1BF};Cyc_aprintf(({const char*_tmp1BE="%d";_tag_dyneither(_tmp1BE,sizeof(char),
3);}),_tag_dyneither(_tmp1BD,sizeof(void*),1));});}));goto _LLCA;_LLCF: {struct
Cyc_Absyn_Const_e_struct*_tmp121=(struct Cyc_Absyn_Const_e_struct*)_tmp116;if(
_tmp121->tag != 0)goto _LLD1;else{_tmp122=_tmp121->f1;if((_tmp122.Int_c).tag != 4)
goto _LLD1;_tmp123=(struct _tuple5)(_tmp122.Int_c).val;_tmp124=_tmp123.f1;if(
_tmp124 != Cyc_Absyn_None)goto _LLD1;_tmp125=_tmp123.f2;}}_LLD0: _tmp12A=_tmp125;
goto _LLD2;_LLD1: {struct Cyc_Absyn_Const_e_struct*_tmp126=(struct Cyc_Absyn_Const_e_struct*)
_tmp116;if(_tmp126->tag != 0)goto _LLD3;else{_tmp127=_tmp126->f1;if((_tmp127.Int_c).tag
!= 4)goto _LLD3;_tmp128=(struct _tuple5)(_tmp127.Int_c).val;_tmp129=_tmp128.f1;if(
_tmp129 != Cyc_Absyn_Signed)goto _LLD3;_tmp12A=_tmp128.f2;}}_LLD2: Cyc_Absyndump_dump((
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp1C2;_tmp1C2.tag=1;_tmp1C2.f1=(
unsigned long)_tmp12A;({void*_tmp1C0[1]={& _tmp1C2};Cyc_aprintf(({const char*
_tmp1C1="%d";_tag_dyneither(_tmp1C1,sizeof(char),3);}),_tag_dyneither(_tmp1C0,
sizeof(void*),1));});}));goto _LLCA;_LLD3: {struct Cyc_Absyn_Const_e_struct*
_tmp12B=(struct Cyc_Absyn_Const_e_struct*)_tmp116;if(_tmp12B->tag != 0)goto _LLD5;
else{_tmp12C=_tmp12B->f1;if((_tmp12C.Int_c).tag != 4)goto _LLD5;_tmp12D=(struct
_tuple5)(_tmp12C.Int_c).val;_tmp12E=_tmp12D.f1;if(_tmp12E != Cyc_Absyn_Unsigned)
goto _LLD5;_tmp12F=_tmp12D.f2;}}_LLD4: Cyc_Absyndump_dump((struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp1C5;_tmp1C5.tag=1;_tmp1C5.f1=(unsigned int)_tmp12F;({
void*_tmp1C3[1]={& _tmp1C5};Cyc_aprintf(({const char*_tmp1C4="%u";_tag_dyneither(
_tmp1C4,sizeof(char),3);}),_tag_dyneither(_tmp1C3,sizeof(void*),1));});}));goto
_LLCA;_LLD5: {struct Cyc_Absyn_Const_e_struct*_tmp130=(struct Cyc_Absyn_Const_e_struct*)
_tmp116;if(_tmp130->tag != 0)goto _LLD7;else{_tmp131=_tmp130->f1;if((_tmp131.LongLong_c).tag
!= 5)goto _LLD7;_tmp132=(struct _tuple6)(_tmp131.LongLong_c).val;_tmp133=_tmp132.f1;
_tmp134=_tmp132.f2;}}_LLD6: Cyc_Absyndump_dump(({const char*_tmp1C6="<<FIX LONG LONG CONSTANT>>";
_tag_dyneither(_tmp1C6,sizeof(char),27);}));goto _LLCA;_LLD7: {struct Cyc_Absyn_Const_e_struct*
_tmp135=(struct Cyc_Absyn_Const_e_struct*)_tmp116;if(_tmp135->tag != 0)goto _LLD9;
else{_tmp136=_tmp135->f1;if((_tmp136.Float_c).tag != 6)goto _LLD9;_tmp137=(struct
_dyneither_ptr)(_tmp136.Float_c).val;}}_LLD8: Cyc_Absyndump_dump(_tmp137);goto
_LLCA;_LLD9: {struct Cyc_Absyn_Const_e_struct*_tmp138=(struct Cyc_Absyn_Const_e_struct*)
_tmp116;if(_tmp138->tag != 0)goto _LLDB;else{_tmp139=_tmp138->f1;if((_tmp139.Null_c).tag
!= 1)goto _LLDB;_tmp13A=(int)(_tmp139.Null_c).val;}}_LLDA: Cyc_Absyndump_dump(({
const char*_tmp1C7="NULL";_tag_dyneither(_tmp1C7,sizeof(char),5);}));goto _LLCA;
_LLDB: {struct Cyc_Absyn_Const_e_struct*_tmp13B=(struct Cyc_Absyn_Const_e_struct*)
_tmp116;if(_tmp13B->tag != 0)goto _LLDD;else{_tmp13C=_tmp13B->f1;if((_tmp13C.String_c).tag
!= 7)goto _LLDD;_tmp13D=(struct _dyneither_ptr)(_tmp13C.String_c).val;}}_LLDC: Cyc_Absyndump_dump_char((
int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp13D));Cyc_Absyndump_dump_char((
int)'"');goto _LLCA;_LLDD: {struct Cyc_Absyn_UnknownId_e_struct*_tmp13E=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp116;if(_tmp13E->tag != 2)goto _LLDF;else{_tmp13F=_tmp13E->f1;}}_LLDE: _tmp141=
_tmp13F;goto _LLE0;_LLDF: {struct Cyc_Absyn_Var_e_struct*_tmp140=(struct Cyc_Absyn_Var_e_struct*)
_tmp116;if(_tmp140->tag != 1)goto _LLE1;else{_tmp141=_tmp140->f1;}}_LLE0: Cyc_Absyndump_dumpqvar(
_tmp141);goto _LLCA;_LLE1: {struct Cyc_Absyn_Primop_e_struct*_tmp142=(struct Cyc_Absyn_Primop_e_struct*)
_tmp116;if(_tmp142->tag != 3)goto _LLE3;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}
_LLE2: {struct _dyneither_ptr _tmp1C8=Cyc_Absynpp_prim2str(_tmp143);switch(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp144)){case 1: _LL135: if(_tmp143 == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){Cyc_Absyndump_dump(({const char*_tmp1C9="numelts(";
_tag_dyneither(_tmp1C9,sizeof(char),9);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp144))->hd);Cyc_Absyndump_dump(({const char*
_tmp1CA=")";_tag_dyneither(_tmp1CA,sizeof(char),2);}));}else{Cyc_Absyndump_dump(
_tmp1C8);Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp144))->hd);}break;case 2: _LL136: Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp144))->hd);Cyc_Absyndump_dump(
_tmp1C8);Cyc_Absyndump_dump_char((int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp144->tl))->hd);
break;default: _LL137:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp1CB=
_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=({struct Cyc_Core_Failure_struct _tmp1CC;
_tmp1CC.tag=Cyc_Core_Failure;_tmp1CC.f1=({const char*_tmp1CD="Absyndump -- Bad number of arguments to primop";
_tag_dyneither(_tmp1CD,sizeof(char),47);});_tmp1CC;});_tmp1CB;}));}goto _LLCA;}
_LLE3: {struct Cyc_Absyn_AssignOp_e_struct*_tmp145=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp116;if(_tmp145->tag != 4)goto _LLE5;else{_tmp146=_tmp145->f1;_tmp147=_tmp145->f2;
_tmp148=_tmp145->f3;}}_LLE4: Cyc_Absyndump_dumpexp_prec(myprec,_tmp146);if(
_tmp147 != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)
_tmp147->v));Cyc_Absyndump_dump_nospace(({const char*_tmp1CE="=";_tag_dyneither(
_tmp1CE,sizeof(char),2);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp148);goto _LLCA;
_LLE5: {struct Cyc_Absyn_Increment_e_struct*_tmp149=(struct Cyc_Absyn_Increment_e_struct*)
_tmp116;if(_tmp149->tag != 5)goto _LLE7;else{_tmp14A=_tmp149->f1;_tmp14B=_tmp149->f2;
if(_tmp14B != Cyc_Absyn_PreInc)goto _LLE7;}}_LLE6: Cyc_Absyndump_dump(({const char*
_tmp1CF="++";_tag_dyneither(_tmp1CF,sizeof(char),3);}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp14A);goto _LLCA;_LLE7: {struct Cyc_Absyn_Increment_e_struct*_tmp14C=(
struct Cyc_Absyn_Increment_e_struct*)_tmp116;if(_tmp14C->tag != 5)goto _LLE9;else{
_tmp14D=_tmp14C->f1;_tmp14E=_tmp14C->f2;if(_tmp14E != Cyc_Absyn_PreDec)goto _LLE9;}}
_LLE8: Cyc_Absyndump_dump(({const char*_tmp1D0="--";_tag_dyneither(_tmp1D0,sizeof(
char),3);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp14D);goto _LLCA;_LLE9: {struct
Cyc_Absyn_Increment_e_struct*_tmp14F=(struct Cyc_Absyn_Increment_e_struct*)
_tmp116;if(_tmp14F->tag != 5)goto _LLEB;else{_tmp150=_tmp14F->f1;_tmp151=_tmp14F->f2;
if(_tmp151 != Cyc_Absyn_PostInc)goto _LLEB;}}_LLEA: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp150);Cyc_Absyndump_dump(({const char*_tmp1D1="++";_tag_dyneither(
_tmp1D1,sizeof(char),3);}));goto _LLCA;_LLEB: {struct Cyc_Absyn_Increment_e_struct*
_tmp152=(struct Cyc_Absyn_Increment_e_struct*)_tmp116;if(_tmp152->tag != 5)goto
_LLED;else{_tmp153=_tmp152->f1;_tmp154=_tmp152->f2;if(_tmp154 != Cyc_Absyn_PostDec)
goto _LLED;}}_LLEC: Cyc_Absyndump_dumpexp_prec(myprec,_tmp153);Cyc_Absyndump_dump(({
const char*_tmp1D2="--";_tag_dyneither(_tmp1D2,sizeof(char),3);}));goto _LLCA;
_LLED: {struct Cyc_Absyn_Conditional_e_struct*_tmp155=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp116;if(_tmp155->tag != 6)goto _LLEF;else{_tmp156=_tmp155->f1;_tmp157=_tmp155->f2;
_tmp158=_tmp155->f3;}}_LLEE: Cyc_Absyndump_dumpexp_prec(myprec,_tmp156);Cyc_Absyndump_dump_char((
int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp157);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp158);goto _LLCA;_LLEF: {struct Cyc_Absyn_And_e_struct*
_tmp159=(struct Cyc_Absyn_And_e_struct*)_tmp116;if(_tmp159->tag != 7)goto _LLF1;
else{_tmp15A=_tmp159->f1;_tmp15B=_tmp159->f2;}}_LLF0: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp15A);Cyc_Absyndump_dump(({const char*_tmp1D3=" && ";_tag_dyneither(
_tmp1D3,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp15B);goto _LLCA;
_LLF1: {struct Cyc_Absyn_Or_e_struct*_tmp15C=(struct Cyc_Absyn_Or_e_struct*)
_tmp116;if(_tmp15C->tag != 8)goto _LLF3;else{_tmp15D=_tmp15C->f1;_tmp15E=_tmp15C->f2;}}
_LLF2: Cyc_Absyndump_dumpexp_prec(myprec,_tmp15D);Cyc_Absyndump_dump(({const char*
_tmp1D4=" || ";_tag_dyneither(_tmp1D4,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp15E);goto _LLCA;_LLF3: {struct Cyc_Absyn_SeqExp_e_struct*_tmp15F=(
struct Cyc_Absyn_SeqExp_e_struct*)_tmp116;if(_tmp15F->tag != 9)goto _LLF5;else{
_tmp160=_tmp15F->f1;_tmp161=_tmp15F->f2;}}_LLF4: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp160);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp161);Cyc_Absyndump_dump_char((int)')');goto _LLCA;_LLF5: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp162=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp116;if(_tmp162->tag != 10)goto
_LLF7;else{_tmp163=_tmp162->f1;_tmp164=_tmp162->f2;}}_LLF6: _tmp166=_tmp163;
_tmp167=_tmp164;goto _LLF8;_LLF7: {struct Cyc_Absyn_FnCall_e_struct*_tmp165=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp116;if(_tmp165->tag != 11)goto _LLF9;else{
_tmp166=_tmp165->f1;_tmp167=_tmp165->f2;}}_LLF8: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp166);Cyc_Absyndump_dump_nospace(({const char*_tmp1D5="(";
_tag_dyneither(_tmp1D5,sizeof(char),2);}));Cyc_Absyndump_dumpexps_prec(20,
_tmp167);Cyc_Absyndump_dump_nospace(({const char*_tmp1D6=")";_tag_dyneither(
_tmp1D6,sizeof(char),2);}));goto _LLCA;_LLF9: {struct Cyc_Absyn_Throw_e_struct*
_tmp168=(struct Cyc_Absyn_Throw_e_struct*)_tmp116;if(_tmp168->tag != 12)goto _LLFB;
else{_tmp169=_tmp168->f1;}}_LLFA: Cyc_Absyndump_dump(({const char*_tmp1D7="throw";
_tag_dyneither(_tmp1D7,sizeof(char),6);}));Cyc_Absyndump_dumpexp_prec(myprec,
_tmp169);goto _LLCA;_LLFB: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp16A=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp116;if(_tmp16A->tag != 13)goto _LLFD;
else{_tmp16B=_tmp16A->f1;}}_LLFC: _tmp16D=_tmp16B;goto _LLFE;_LLFD: {struct Cyc_Absyn_Instantiate_e_struct*
_tmp16C=(struct Cyc_Absyn_Instantiate_e_struct*)_tmp116;if(_tmp16C->tag != 14)goto
_LLFF;else{_tmp16D=_tmp16C->f1;}}_LLFE: Cyc_Absyndump_dumpexp_prec(inprec,_tmp16D);
goto _LLCA;_LLFF: {struct Cyc_Absyn_Cast_e_struct*_tmp16E=(struct Cyc_Absyn_Cast_e_struct*)
_tmp116;if(_tmp16E->tag != 15)goto _LL101;else{_tmp16F=(void*)_tmp16E->f1;_tmp170=
_tmp16E->f2;}}_LL100: Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(
_tmp16F);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp170);goto _LLCA;_LL101: {struct Cyc_Absyn_Address_e_struct*_tmp171=(struct Cyc_Absyn_Address_e_struct*)
_tmp116;if(_tmp171->tag != 16)goto _LL103;else{_tmp172=_tmp171->f1;}}_LL102: Cyc_Absyndump_dump_char((
int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp172);goto _LLCA;_LL103: {struct Cyc_Absyn_New_e_struct*
_tmp173=(struct Cyc_Absyn_New_e_struct*)_tmp116;if(_tmp173->tag != 17)goto _LL105;
else{_tmp174=_tmp173->f1;_tmp175=_tmp173->f2;}}_LL104: Cyc_Absyndump_dump(({const
char*_tmp1D8="new ";_tag_dyneither(_tmp1D8,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp175);goto _LLCA;_LL105: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp176=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp116;if(_tmp176->tag != 18)goto _LL107;
else{_tmp177=(void*)_tmp176->f1;}}_LL106: Cyc_Absyndump_dump(({const char*_tmp1D9="sizeof(";
_tag_dyneither(_tmp1D9,sizeof(char),8);}));Cyc_Absyndump_dumptyp(_tmp177);Cyc_Absyndump_dump_char((
int)')');goto _LLCA;_LL107: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp178=(struct
Cyc_Absyn_Sizeofexp_e_struct*)_tmp116;if(_tmp178->tag != 19)goto _LL109;else{
_tmp179=_tmp178->f1;}}_LL108: Cyc_Absyndump_dump(({const char*_tmp1DA="sizeof(";
_tag_dyneither(_tmp1DA,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp179);Cyc_Absyndump_dump_char((
int)')');goto _LLCA;_LL109: {struct Cyc_Absyn_Valueof_e_struct*_tmp17A=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp116;if(_tmp17A->tag != 40)goto _LL10B;else{_tmp17B=(void*)_tmp17A->f1;}}_LL10A:
Cyc_Absyndump_dump(({const char*_tmp1DB="valueof(";_tag_dyneither(_tmp1DB,sizeof(
char),9);}));Cyc_Absyndump_dumptyp(_tmp17B);Cyc_Absyndump_dump_char((int)')');
goto _LLCA;_LL10B: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp17C=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp116;if(_tmp17C->tag != 39)goto _LL10D;else{_tmp17D=_tmp17C->f1;_tmp17E=_tmp17C->f2;}}
_LL10C: Cyc_Absyndump_dump(({const char*_tmp1DC="tagcheck(";_tag_dyneither(_tmp1DC,
sizeof(char),10);}));Cyc_Absyndump_dumpexp(_tmp17D);Cyc_Absyndump_dump_char((int)'.');
Cyc_Absyndump_dump_nospace(*_tmp17E);Cyc_Absyndump_dump_char((int)')');goto _LLCA;
_LL10D: {struct Cyc_Absyn_Offsetof_e_struct*_tmp17F=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp116;if(_tmp17F->tag != 20)goto _LL10F;else{_tmp180=(void*)_tmp17F->f1;_tmp181=(
void*)_tmp17F->f2;{struct Cyc_Absyn_StructField_struct*_tmp182=(struct Cyc_Absyn_StructField_struct*)
_tmp181;if(_tmp182->tag != 0)goto _LL10F;else{_tmp183=_tmp182->f1;}};}}_LL10E: Cyc_Absyndump_dump(({
const char*_tmp1DD="offsetof(";_tag_dyneither(_tmp1DD,sizeof(char),10);}));Cyc_Absyndump_dumptyp(
_tmp180);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp183);
Cyc_Absyndump_dump_char((int)')');goto _LLCA;_LL10F: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp184=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp116;if(_tmp184->tag != 20)goto
_LL111;else{_tmp185=(void*)_tmp184->f1;_tmp186=(void*)_tmp184->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp187=(struct Cyc_Absyn_TupleIndex_struct*)_tmp186;if(_tmp187->tag != 1)goto
_LL111;else{_tmp188=_tmp187->f1;}};}}_LL110: Cyc_Absyndump_dump(({const char*
_tmp1DE="offsetof(";_tag_dyneither(_tmp1DE,sizeof(char),10);}));Cyc_Absyndump_dumptyp(
_tmp185);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump((struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp1E1;_tmp1E1.tag=1;_tmp1E1.f1=(
unsigned long)((int)_tmp188);({void*_tmp1DF[1]={& _tmp1E1};Cyc_aprintf(({const
char*_tmp1E0="%d";_tag_dyneither(_tmp1E0,sizeof(char),3);}),_tag_dyneither(
_tmp1DF,sizeof(void*),1));});}));Cyc_Absyndump_dump_char((int)')');goto _LLCA;
_LL111: {struct Cyc_Absyn_Gentyp_e_struct*_tmp189=(struct Cyc_Absyn_Gentyp_e_struct*)
_tmp116;if(_tmp189->tag != 21)goto _LL113;else{_tmp18A=_tmp189->f1;_tmp18B=(void*)
_tmp189->f2;}}_LL112: Cyc_Absyndump_dump(({const char*_tmp1E2="__gen(";
_tag_dyneither(_tmp1E2,sizeof(char),7);}));Cyc_Absyndump_dumptvars(_tmp18A);Cyc_Absyndump_dumptyp(
_tmp18B);Cyc_Absyndump_dump_char((int)')');goto _LLCA;_LL113: {struct Cyc_Absyn_Deref_e_struct*
_tmp18C=(struct Cyc_Absyn_Deref_e_struct*)_tmp116;if(_tmp18C->tag != 22)goto _LL115;
else{_tmp18D=_tmp18C->f1;}}_LL114: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp18D);goto _LLCA;_LL115: {struct Cyc_Absyn_AggrMember_e_struct*_tmp18E=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmp116;if(_tmp18E->tag != 23)goto _LL117;
else{_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;}}_LL116: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp18F);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp190);goto _LLCA;_LL117: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp191=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp116;if(_tmp191->tag != 24)goto _LL119;else{
_tmp192=_tmp191->f1;_tmp193=_tmp191->f2;}}_LL118: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp192);Cyc_Absyndump_dump_nospace(({const char*_tmp1E3="->";
_tag_dyneither(_tmp1E3,sizeof(char),3);}));Cyc_Absyndump_dump_nospace(*_tmp193);
goto _LLCA;_LL119: {struct Cyc_Absyn_Subscript_e_struct*_tmp194=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp116;if(_tmp194->tag != 25)goto _LL11B;else{_tmp195=_tmp194->f1;_tmp196=_tmp194->f2;}}
_LL11A: Cyc_Absyndump_dumpexp_prec(myprec,_tmp195);Cyc_Absyndump_dump_char((int)'[');
Cyc_Absyndump_dumpexp(_tmp196);Cyc_Absyndump_dump_char((int)']');goto _LLCA;
_LL11B: {struct Cyc_Absyn_Tuple_e_struct*_tmp197=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp116;if(_tmp197->tag != 26)goto _LL11D;else{_tmp198=_tmp197->f1;}}_LL11C: Cyc_Absyndump_dump(({
const char*_tmp1E4="$(";_tag_dyneither(_tmp1E4,sizeof(char),3);}));Cyc_Absyndump_dumpexps_prec(
20,_tmp198);Cyc_Absyndump_dump_char((int)')');goto _LLCA;_LL11D: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp199=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp116;if(_tmp199->tag != 27)goto
_LL11F;else{_tmp19A=_tmp199->f1;_tmp19B=_tmp199->f2;}}_LL11E: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp((*_tmp19A).f3);Cyc_Absyndump_dump_char((int)')');((
void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr
start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp19B,({const char*_tmp1E5="{";_tag_dyneither(_tmp1E5,sizeof(char),2);}),({
const char*_tmp1E6="}";_tag_dyneither(_tmp1E6,sizeof(char),2);}),({const char*
_tmp1E7=",";_tag_dyneither(_tmp1E7,sizeof(char),2);}));goto _LLCA;_LL11F: {struct
Cyc_Absyn_Array_e_struct*_tmp19C=(struct Cyc_Absyn_Array_e_struct*)_tmp116;if(
_tmp19C->tag != 28)goto _LL121;else{_tmp19D=_tmp19C->f1;}}_LL120:((void(*)(void(*f)(
struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp19D,({const char*_tmp1E8="{";_tag_dyneither(_tmp1E8,sizeof(char),2);}),({
const char*_tmp1E9="}";_tag_dyneither(_tmp1E9,sizeof(char),2);}),({const char*
_tmp1EA=",";_tag_dyneither(_tmp1EA,sizeof(char),2);}));goto _LLCA;_LL121: {struct
Cyc_Absyn_Comprehension_e_struct*_tmp19E=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp116;if(_tmp19E->tag != 29)goto _LL123;else{_tmp19F=_tmp19E->f1;_tmp1A0=_tmp19E->f2;
_tmp1A1=_tmp19E->f3;}}_LL122: Cyc_Absyndump_dump(({const char*_tmp1EB="new {for";
_tag_dyneither(_tmp1EB,sizeof(char),9);}));Cyc_Absyndump_dump_str((*_tmp19F->name).f2);
Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp1A0);Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp(_tmp1A1);Cyc_Absyndump_dump_char((int)'}');goto
_LLCA;_LL123: {struct Cyc_Absyn_Aggregate_e_struct*_tmp1A2=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp116;if(_tmp1A2->tag != 30)goto _LL125;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;
_tmp1A5=_tmp1A2->f3;}}_LL124: Cyc_Absyndump_dumpqvar(_tmp1A3);Cyc_Absyndump_dump_char((
int)'{');if(_tmp1A4 != 0)Cyc_Absyndump_dumptps(_tmp1A4);((void(*)(void(*f)(struct
_tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr
end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1A5,({
const char*_tmp1EC="";_tag_dyneither(_tmp1EC,sizeof(char),1);}),({const char*
_tmp1ED="}";_tag_dyneither(_tmp1ED,sizeof(char),2);}),({const char*_tmp1EE=",";
_tag_dyneither(_tmp1EE,sizeof(char),2);}));goto _LLCA;_LL125: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1A6=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp116;if(_tmp1A6->tag != 31)goto
_LL127;else{_tmp1A7=_tmp1A6->f2;}}_LL126:((void(*)(void(*f)(struct _tuple16*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1A7,({const char*
_tmp1EF="{";_tag_dyneither(_tmp1EF,sizeof(char),2);}),({const char*_tmp1F0="}";
_tag_dyneither(_tmp1F0,sizeof(char),2);}),({const char*_tmp1F1=",";_tag_dyneither(
_tmp1F1,sizeof(char),2);}));goto _LLCA;_LL127: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1A8=(struct Cyc_Absyn_Datatype_e_struct*)_tmp116;if(_tmp1A8->tag != 32)goto
_LL129;else{_tmp1A9=_tmp1A8->f1;_tmp1AA=_tmp1A8->f3;}}_LL128: Cyc_Absyndump_dumpqvar(
_tmp1AA->name);if(_tmp1A9 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmp1A9,({const char*_tmp1F2="(";_tag_dyneither(_tmp1F2,
sizeof(char),2);}),({const char*_tmp1F3=")";_tag_dyneither(_tmp1F3,sizeof(char),2);}),({
const char*_tmp1F4=",";_tag_dyneither(_tmp1F4,sizeof(char),2);}));goto _LLCA;
_LL129: {struct Cyc_Absyn_Enum_e_struct*_tmp1AB=(struct Cyc_Absyn_Enum_e_struct*)
_tmp116;if(_tmp1AB->tag != 33)goto _LL12B;else{_tmp1AC=_tmp1AB->f1;}}_LL12A: Cyc_Absyndump_dumpqvar(
_tmp1AC);goto _LLCA;_LL12B: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1AD=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp116;if(_tmp1AD->tag != 34)goto _LL12D;else{_tmp1AE=_tmp1AD->f1;}}_LL12C: Cyc_Absyndump_dumpqvar(
_tmp1AE);goto _LLCA;_LL12D: {struct Cyc_Absyn_Malloc_e_struct*_tmp1AF=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp116;if(_tmp1AF->tag != 35)goto _LL12F;else{_tmp1B0=_tmp1AF->f1;_tmp1B1=_tmp1B0.is_calloc;
_tmp1B2=_tmp1B0.rgn;_tmp1B3=_tmp1B0.elt_type;_tmp1B4=_tmp1B0.num_elts;}}_LL12E:
if(_tmp1B1){if(_tmp1B2 != 0){Cyc_Absyndump_dump(({const char*_tmp1F5="rcalloc(";
_tag_dyneither(_tmp1F5,sizeof(char),9);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp1B2);Cyc_Absyndump_dump(({const char*_tmp1F6=",";_tag_dyneither(_tmp1F6,
sizeof(char),2);}));}else{Cyc_Absyndump_dump(({const char*_tmp1F7="calloc";
_tag_dyneither(_tmp1F7,sizeof(char),7);}));}Cyc_Absyndump_dumpexp(_tmp1B4);Cyc_Absyndump_dump(({
const char*_tmp1F8=",";_tag_dyneither(_tmp1F8,sizeof(char),2);}));Cyc_Absyndump_dumpexp(
Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp1B3)),0));Cyc_Absyndump_dump(({
const char*_tmp1F9=")";_tag_dyneither(_tmp1F9,sizeof(char),2);}));}else{if(
_tmp1B2 != 0){Cyc_Absyndump_dump(({const char*_tmp1FA="rmalloc(";_tag_dyneither(
_tmp1FA,sizeof(char),9);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp1B2);
Cyc_Absyndump_dump(({const char*_tmp1FB=",";_tag_dyneither(_tmp1FB,sizeof(char),2);}));}
else{Cyc_Absyndump_dump(({const char*_tmp1FC="malloc(";_tag_dyneither(_tmp1FC,
sizeof(char),8);}));}if(_tmp1B3 != 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp1B3,0),_tmp1B4,0));else{Cyc_Absyndump_dumpexp(
_tmp1B4);}Cyc_Absyndump_dump(({const char*_tmp1FD=")";_tag_dyneither(_tmp1FD,
sizeof(char),2);}));}goto _LLCA;_LL12F: {struct Cyc_Absyn_Swap_e_struct*_tmp1B5=(
struct Cyc_Absyn_Swap_e_struct*)_tmp116;if(_tmp1B5->tag != 36)goto _LL131;else{
_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B5->f2;}}_LL130: Cyc_Absyndump_dump(({const char*
_tmp1FE="cycswap(";_tag_dyneither(_tmp1FE,sizeof(char),9);}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1B6);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1B7);Cyc_Absyndump_dump_char((int)')');goto _LLCA;_LL131: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1B8=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp116;if(_tmp1B8->tag != 37)
goto _LL133;else{_tmp1B9=_tmp1B8->f1;_tmp1BA=_tmp1B8->f2;}}_LL132:((void(*)(void(*
f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp1BA,({const char*_tmp1FF="{";_tag_dyneither(_tmp1FF,sizeof(char),2);}),({
const char*_tmp200="}";_tag_dyneither(_tmp200,sizeof(char),2);}),({const char*
_tmp201=",";_tag_dyneither(_tmp201,sizeof(char),2);}));goto _LLCA;_LL133: {struct
Cyc_Absyn_StmtExp_e_struct*_tmp1BB=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp116;
if(_tmp1BB->tag != 38)goto _LLCA;else{_tmp1BC=_tmp1BB->f1;}}_LL134: Cyc_Absyndump_dump_nospace(({
const char*_tmp202="({";_tag_dyneither(_tmp202,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(
_tmp1BC,1);Cyc_Absyndump_dump_nospace(({const char*_tmp203="})";_tag_dyneither(
_tmp203,sizeof(char),3);}));goto _LLCA;_LLCA:;}if(inprec >= myprec)Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(
0,e);}void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){for(0;scs != 0;
scs=scs->tl){struct Cyc_Absyn_Switch_clause*_tmp204=(struct Cyc_Absyn_Switch_clause*)
scs->hd;if(_tmp204->where_clause == 0  && (_tmp204->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp205="default:";_tag_dyneither(_tmp205,sizeof(
char),9);}));else{Cyc_Absyndump_dump(({const char*_tmp206="case";_tag_dyneither(
_tmp206,sizeof(char),5);}));Cyc_Absyndump_dumppat(_tmp204->pattern);if(_tmp204->where_clause
!= 0){Cyc_Absyndump_dump(({const char*_tmp207="&&";_tag_dyneither(_tmp207,sizeof(
char),3);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp204->where_clause));}
Cyc_Absyndump_dump_nospace(({const char*_tmp208=":";_tag_dyneither(_tmp208,
sizeof(char),2);}));}Cyc_Absyndump_dumpstmt(_tmp204->body,0);}}void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*s,int expstmt){void*_tmp209=s->r;struct Cyc_Absyn_Exp*_tmp20C;
struct Cyc_Absyn_Stmt*_tmp20E;struct Cyc_Absyn_Stmt*_tmp20F;struct Cyc_Absyn_Exp*
_tmp211;struct Cyc_Absyn_Exp*_tmp213;struct Cyc_Absyn_Exp*_tmp215;struct Cyc_Absyn_Stmt*
_tmp216;struct Cyc_Absyn_Stmt*_tmp217;struct _tuple8 _tmp219;struct Cyc_Absyn_Exp*
_tmp21A;struct Cyc_Absyn_Stmt*_tmp21B;struct _dyneither_ptr*_tmp21F;struct Cyc_Absyn_Exp*
_tmp221;struct _tuple8 _tmp222;struct Cyc_Absyn_Exp*_tmp223;struct _tuple8 _tmp224;
struct Cyc_Absyn_Exp*_tmp225;struct Cyc_Absyn_Stmt*_tmp226;struct Cyc_Absyn_Exp*
_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_Absyn_Decl*_tmp22B;struct Cyc_Absyn_Stmt*
_tmp22C;struct _dyneither_ptr*_tmp22E;struct Cyc_Absyn_Stmt*_tmp22F;struct Cyc_Absyn_Stmt*
_tmp231;struct _tuple8 _tmp232;struct Cyc_Absyn_Exp*_tmp233;struct Cyc_List_List*
_tmp235;struct Cyc_List_List*_tmp237;struct Cyc_Absyn_Stmt*_tmp239;struct Cyc_List_List*
_tmp23A;struct Cyc_Absyn_Exp*_tmp23C;_LL13A: {struct Cyc_Absyn_Skip_s_struct*
_tmp20A=(struct Cyc_Absyn_Skip_s_struct*)_tmp209;if(_tmp20A->tag != 0)goto _LL13C;}
_LL13B: Cyc_Absyndump_dump_semi();goto _LL139;_LL13C: {struct Cyc_Absyn_Exp_s_struct*
_tmp20B=(struct Cyc_Absyn_Exp_s_struct*)_tmp209;if(_tmp20B->tag != 1)goto _LL13E;
else{_tmp20C=_tmp20B->f1;}}_LL13D: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dumpexp(
_tmp20C);Cyc_Absyndump_dump_semi();goto _LL139;_LL13E: {struct Cyc_Absyn_Seq_s_struct*
_tmp20D=(struct Cyc_Absyn_Seq_s_struct*)_tmp209;if(_tmp20D->tag != 2)goto _LL140;
else{_tmp20E=_tmp20D->f1;_tmp20F=_tmp20D->f2;}}_LL13F: if(Cyc_Absynpp_is_declaration(
_tmp20E)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp20E,0);
Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp20E,0);}if(
Cyc_Absynpp_is_declaration(_tmp20F)){if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp20F,expstmt);Cyc_Absyndump_dump_char((
int)'}');if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}
else{Cyc_Absyndump_dumpstmt(_tmp20F,expstmt);}goto _LL139;_LL140: {struct Cyc_Absyn_Return_s_struct*
_tmp210=(struct Cyc_Absyn_Return_s_struct*)_tmp209;if(_tmp210->tag != 3)goto _LL142;
else{_tmp211=_tmp210->f1;if(_tmp211 != 0)goto _LL142;}}_LL141: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp23D="return;";_tag_dyneither(_tmp23D,
sizeof(char),8);}));goto _LL139;_LL142: {struct Cyc_Absyn_Return_s_struct*_tmp212=(
struct Cyc_Absyn_Return_s_struct*)_tmp209;if(_tmp212->tag != 3)goto _LL144;else{
_tmp213=_tmp212->f1;}}_LL143: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp23E="return";_tag_dyneither(_tmp23E,sizeof(char),7);}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(_tmp213));Cyc_Absyndump_dump_semi();goto _LL139;
_LL144: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp214=(struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp209;if(_tmp214->tag != 4)goto _LL146;else{_tmp215=_tmp214->f1;_tmp216=_tmp214->f2;
_tmp217=_tmp214->f3;}}_LL145: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp23F="if(";_tag_dyneither(_tmp23F,sizeof(char),4);}));Cyc_Absyndump_dumpexp(
_tmp215);{void*_tmp240=_tmp216->r;_LL161: {struct Cyc_Absyn_Seq_s_struct*_tmp241=(
struct Cyc_Absyn_Seq_s_struct*)_tmp240;if(_tmp241->tag != 2)goto _LL163;}_LL162:
goto _LL164;_LL163: {struct Cyc_Absyn_Decl_s_struct*_tmp242=(struct Cyc_Absyn_Decl_s_struct*)
_tmp240;if(_tmp242->tag != 12)goto _LL165;}_LL164: goto _LL166;_LL165: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp243=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp240;if(_tmp243->tag != 4)goto
_LL167;}_LL166: goto _LL168;_LL167: {struct Cyc_Absyn_Label_s_struct*_tmp244=(
struct Cyc_Absyn_Label_s_struct*)_tmp240;if(_tmp244->tag != 13)goto _LL169;}_LL168:
Cyc_Absyndump_dump_nospace(({const char*_tmp245="){";_tag_dyneither(_tmp245,
sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp216,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL160;_LL169:;_LL16A: Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(
_tmp216,0);_LL160:;}{void*_tmp246=_tmp217->r;_LL16C: {struct Cyc_Absyn_Skip_s_struct*
_tmp247=(struct Cyc_Absyn_Skip_s_struct*)_tmp246;if(_tmp247->tag != 0)goto _LL16E;}
_LL16D: goto _LL16B;_LL16E:;_LL16F: Cyc_Absyndump_dump(({const char*_tmp248="else{";
_tag_dyneither(_tmp248,sizeof(char),6);}));Cyc_Absyndump_dumpstmt(_tmp217,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL16B;_LL16B:;}goto _LL139;_LL146: {struct Cyc_Absyn_While_s_struct*
_tmp218=(struct Cyc_Absyn_While_s_struct*)_tmp209;if(_tmp218->tag != 5)goto _LL148;
else{_tmp219=_tmp218->f1;_tmp21A=_tmp219.f1;_tmp21B=_tmp218->f2;}}_LL147: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp249="while(";_tag_dyneither(_tmp249,
sizeof(char),7);}));Cyc_Absyndump_dumpexp(_tmp21A);Cyc_Absyndump_dump_nospace(({
const char*_tmp24A="){";_tag_dyneither(_tmp24A,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(
_tmp21B,0);Cyc_Absyndump_dump_char((int)'}');goto _LL139;_LL148: {struct Cyc_Absyn_Break_s_struct*
_tmp21C=(struct Cyc_Absyn_Break_s_struct*)_tmp209;if(_tmp21C->tag != 6)goto _LL14A;}
_LL149: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp24B="break;";
_tag_dyneither(_tmp24B,sizeof(char),7);}));goto _LL139;_LL14A: {struct Cyc_Absyn_Continue_s_struct*
_tmp21D=(struct Cyc_Absyn_Continue_s_struct*)_tmp209;if(_tmp21D->tag != 7)goto
_LL14C;}_LL14B: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp24C="continue;";_tag_dyneither(_tmp24C,sizeof(char),10);}));goto _LL139;
_LL14C: {struct Cyc_Absyn_Goto_s_struct*_tmp21E=(struct Cyc_Absyn_Goto_s_struct*)
_tmp209;if(_tmp21E->tag != 8)goto _LL14E;else{_tmp21F=_tmp21E->f1;}}_LL14D: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp24D="goto";_tag_dyneither(_tmp24D,
sizeof(char),5);}));Cyc_Absyndump_dump_str(_tmp21F);Cyc_Absyndump_dump_semi();
goto _LL139;_LL14E: {struct Cyc_Absyn_For_s_struct*_tmp220=(struct Cyc_Absyn_For_s_struct*)
_tmp209;if(_tmp220->tag != 9)goto _LL150;else{_tmp221=_tmp220->f1;_tmp222=_tmp220->f2;
_tmp223=_tmp222.f1;_tmp224=_tmp220->f3;_tmp225=_tmp224.f1;_tmp226=_tmp220->f4;}}
_LL14F: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp24E="for(";
_tag_dyneither(_tmp24E,sizeof(char),5);}));Cyc_Absyndump_dumpexp(_tmp221);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp223);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmp225);Cyc_Absyndump_dump_nospace(({const char*_tmp24F="){";_tag_dyneither(
_tmp24F,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp226,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL139;_LL150: {struct Cyc_Absyn_Switch_s_struct*_tmp227=(struct Cyc_Absyn_Switch_s_struct*)
_tmp209;if(_tmp227->tag != 10)goto _LL152;else{_tmp228=_tmp227->f1;_tmp229=_tmp227->f2;}}
_LL151: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp250="switch(";
_tag_dyneither(_tmp250,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp228);Cyc_Absyndump_dump_nospace(({
const char*_tmp251="){";_tag_dyneither(_tmp251,sizeof(char),3);}));Cyc_Absyndump_dumpswitchclauses(
_tmp229);Cyc_Absyndump_dump_char((int)'}');goto _LL139;_LL152: {struct Cyc_Absyn_Decl_s_struct*
_tmp22A=(struct Cyc_Absyn_Decl_s_struct*)_tmp209;if(_tmp22A->tag != 12)goto _LL154;
else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22A->f2;}}_LL153: Cyc_Absyndump_dumpdecl(
_tmp22B);Cyc_Absyndump_dumpstmt(_tmp22C,expstmt);goto _LL139;_LL154: {struct Cyc_Absyn_Label_s_struct*
_tmp22D=(struct Cyc_Absyn_Label_s_struct*)_tmp209;if(_tmp22D->tag != 13)goto _LL156;
else{_tmp22E=_tmp22D->f1;_tmp22F=_tmp22D->f2;}}_LL155: if(Cyc_Absynpp_is_declaration(
_tmp22F)){Cyc_Absyndump_dump_str(_tmp22E);if(expstmt)Cyc_Absyndump_dump_nospace(({
const char*_tmp252=": ({";_tag_dyneither(_tmp252,sizeof(char),5);}));else{Cyc_Absyndump_dump_nospace(({
const char*_tmp253=": {";_tag_dyneither(_tmp253,sizeof(char),4);}));}Cyc_Absyndump_dumpstmt(
_tmp22F,expstmt);if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp254="});}";
_tag_dyneither(_tmp254,sizeof(char),5);}));else{Cyc_Absyndump_dump_char((int)'}');}}
else{Cyc_Absyndump_dump_str(_tmp22E);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(
_tmp22F,expstmt);}goto _LL139;_LL156: {struct Cyc_Absyn_Do_s_struct*_tmp230=(
struct Cyc_Absyn_Do_s_struct*)_tmp209;if(_tmp230->tag != 14)goto _LL158;else{
_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;_tmp233=_tmp232.f1;}}_LL157: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp255="do{";_tag_dyneither(_tmp255,
sizeof(char),4);}));Cyc_Absyndump_dumpstmt(_tmp231,0);Cyc_Absyndump_dump_nospace(({
const char*_tmp256="}while(";_tag_dyneither(_tmp256,sizeof(char),8);}));Cyc_Absyndump_dumpexp(
_tmp233);Cyc_Absyndump_dump_nospace(({const char*_tmp257=");";_tag_dyneither(
_tmp257,sizeof(char),3);}));goto _LL139;_LL158: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp234=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp209;if(_tmp234->tag != 11)goto
_LL15A;else{_tmp235=_tmp234->f1;if(_tmp235 != 0)goto _LL15A;}}_LL159: Cyc_Absyndump_dump(({
const char*_tmp258="fallthru;";_tag_dyneither(_tmp258,sizeof(char),10);}));goto
_LL139;_LL15A: {struct Cyc_Absyn_Fallthru_s_struct*_tmp236=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp209;if(_tmp236->tag != 11)goto _LL15C;else{_tmp237=_tmp236->f1;}}_LL15B: Cyc_Absyndump_dump(({
const char*_tmp259="fallthru(";_tag_dyneither(_tmp259,sizeof(char),10);}));Cyc_Absyndump_dumpexps_prec(
20,_tmp237);Cyc_Absyndump_dump_nospace(({const char*_tmp25A=");";_tag_dyneither(
_tmp25A,sizeof(char),3);}));goto _LL139;_LL15C: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp238=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp209;if(_tmp238->tag != 15)goto
_LL15E;else{_tmp239=_tmp238->f1;_tmp23A=_tmp238->f2;}}_LL15D: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp25B="try";_tag_dyneither(_tmp25B,
sizeof(char),4);}));Cyc_Absyndump_dumpstmt(_tmp239,0);Cyc_Absyndump_dump(({const
char*_tmp25C="catch{";_tag_dyneither(_tmp25C,sizeof(char),7);}));Cyc_Absyndump_dumpswitchclauses(
_tmp23A);Cyc_Absyndump_dump_char((int)'}');goto _LL139;_LL15E: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp23B=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp209;if(_tmp23B->tag != 16)goto
_LL139;else{_tmp23C=_tmp23B->f1;}}_LL15F: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp25D="reset_region(";_tag_dyneither(_tmp25D,sizeof(char),14);}));
Cyc_Absyndump_dumpexp(_tmp23C);Cyc_Absyndump_dump(({const char*_tmp25E=");";
_tag_dyneither(_tmp25E,sizeof(char),3);}));goto _LL139;_LL139:;}struct _tuple17{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(struct
_tuple17*dp){((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(*dp).f1,({const char*_tmp25F="";_tag_dyneither(
_tmp25F,sizeof(char),1);}),({const char*_tmp260="=";_tag_dyneither(_tmp260,
sizeof(char),2);}),({const char*_tmp261="=";_tag_dyneither(_tmp261,sizeof(char),2);}));
Cyc_Absyndump_dumppat((*dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp262=p->r;enum Cyc_Absyn_Sign _tmp266;int _tmp267;enum Cyc_Absyn_Sign
_tmp269;int _tmp26A;enum Cyc_Absyn_Sign _tmp26C;int _tmp26D;char _tmp26F;struct
_dyneither_ptr _tmp271;struct Cyc_Absyn_Vardecl*_tmp273;struct Cyc_Absyn_Pat*
_tmp274;struct Cyc_Absyn_Pat _tmp275;void*_tmp276;struct Cyc_Absyn_Vardecl*_tmp279;
struct Cyc_Absyn_Pat*_tmp27A;struct Cyc_List_List*_tmp27C;int _tmp27D;struct Cyc_Absyn_Pat*
_tmp27F;struct Cyc_Absyn_Vardecl*_tmp281;struct Cyc_Absyn_Pat*_tmp282;struct Cyc_Absyn_Pat
_tmp283;void*_tmp284;struct Cyc_Absyn_Vardecl*_tmp287;struct Cyc_Absyn_Pat*_tmp288;
struct Cyc_Absyn_Tvar*_tmp28A;struct Cyc_Absyn_Vardecl*_tmp28B;struct _tuple0*
_tmp28D;struct _tuple0*_tmp28F;struct Cyc_List_List*_tmp290;int _tmp291;struct Cyc_Absyn_AggrInfo*
_tmp293;struct Cyc_Absyn_AggrInfo _tmp294;union Cyc_Absyn_AggrInfoU _tmp295;struct
Cyc_List_List*_tmp296;struct Cyc_List_List*_tmp297;int _tmp298;struct Cyc_Absyn_AggrInfo*
_tmp29A;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*_tmp29C;int _tmp29D;
struct Cyc_Absyn_Datatypefield*_tmp29F;struct Cyc_List_List*_tmp2A0;int _tmp2A1;
struct Cyc_Absyn_Enumfield*_tmp2A3;struct Cyc_Absyn_Enumfield*_tmp2A5;struct Cyc_Absyn_Exp*
_tmp2A7;_LL171: {struct Cyc_Absyn_Wild_p_struct*_tmp263=(struct Cyc_Absyn_Wild_p_struct*)
_tmp262;if(_tmp263->tag != 0)goto _LL173;}_LL172: Cyc_Absyndump_dump_char((int)'_');
goto _LL170;_LL173: {struct Cyc_Absyn_Null_p_struct*_tmp264=(struct Cyc_Absyn_Null_p_struct*)
_tmp262;if(_tmp264->tag != 8)goto _LL175;}_LL174: Cyc_Absyndump_dump(({const char*
_tmp2A8="NULL";_tag_dyneither(_tmp2A8,sizeof(char),5);}));goto _LL170;_LL175: {
struct Cyc_Absyn_Int_p_struct*_tmp265=(struct Cyc_Absyn_Int_p_struct*)_tmp262;if(
_tmp265->tag != 9)goto _LL177;else{_tmp266=_tmp265->f1;if(_tmp266 != Cyc_Absyn_None)
goto _LL177;_tmp267=_tmp265->f2;}}_LL176: _tmp26A=_tmp267;goto _LL178;_LL177: {
struct Cyc_Absyn_Int_p_struct*_tmp268=(struct Cyc_Absyn_Int_p_struct*)_tmp262;if(
_tmp268->tag != 9)goto _LL179;else{_tmp269=_tmp268->f1;if(_tmp269 != Cyc_Absyn_Signed)
goto _LL179;_tmp26A=_tmp268->f2;}}_LL178: Cyc_Absyndump_dump((struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp2AB;_tmp2AB.tag=1;_tmp2AB.f1=(unsigned long)_tmp26A;({
void*_tmp2A9[1]={& _tmp2AB};Cyc_aprintf(({const char*_tmp2AA="%d";_tag_dyneither(
_tmp2AA,sizeof(char),3);}),_tag_dyneither(_tmp2A9,sizeof(void*),1));});}));goto
_LL170;_LL179: {struct Cyc_Absyn_Int_p_struct*_tmp26B=(struct Cyc_Absyn_Int_p_struct*)
_tmp262;if(_tmp26B->tag != 9)goto _LL17B;else{_tmp26C=_tmp26B->f1;if(_tmp26C != Cyc_Absyn_Unsigned)
goto _LL17B;_tmp26D=_tmp26B->f2;}}_LL17A: Cyc_Absyndump_dump((struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp2AE;_tmp2AE.tag=1;_tmp2AE.f1=(unsigned int)_tmp26D;({
void*_tmp2AC[1]={& _tmp2AE};Cyc_aprintf(({const char*_tmp2AD="%u";_tag_dyneither(
_tmp2AD,sizeof(char),3);}),_tag_dyneither(_tmp2AC,sizeof(void*),1));});}));goto
_LL170;_LL17B: {struct Cyc_Absyn_Char_p_struct*_tmp26E=(struct Cyc_Absyn_Char_p_struct*)
_tmp262;if(_tmp26E->tag != 10)goto _LL17D;else{_tmp26F=_tmp26E->f1;}}_LL17C: Cyc_Absyndump_dump(({
const char*_tmp2AF="'";_tag_dyneither(_tmp2AF,sizeof(char),2);}));Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape(_tmp26F));Cyc_Absyndump_dump_nospace(({const char*_tmp2B0="'";
_tag_dyneither(_tmp2B0,sizeof(char),2);}));goto _LL170;_LL17D: {struct Cyc_Absyn_Float_p_struct*
_tmp270=(struct Cyc_Absyn_Float_p_struct*)_tmp262;if(_tmp270->tag != 11)goto _LL17F;
else{_tmp271=_tmp270->f1;}}_LL17E: Cyc_Absyndump_dump(_tmp271);goto _LL170;_LL17F: {
struct Cyc_Absyn_Var_p_struct*_tmp272=(struct Cyc_Absyn_Var_p_struct*)_tmp262;if(
_tmp272->tag != 1)goto _LL181;else{_tmp273=_tmp272->f1;_tmp274=_tmp272->f2;_tmp275=*
_tmp274;_tmp276=_tmp275.r;{struct Cyc_Absyn_Wild_p_struct*_tmp277=(struct Cyc_Absyn_Wild_p_struct*)
_tmp276;if(_tmp277->tag != 0)goto _LL181;};}}_LL180: Cyc_Absyndump_dumpqvar(_tmp273->name);
goto _LL170;_LL181: {struct Cyc_Absyn_Var_p_struct*_tmp278=(struct Cyc_Absyn_Var_p_struct*)
_tmp262;if(_tmp278->tag != 1)goto _LL183;else{_tmp279=_tmp278->f1;_tmp27A=_tmp278->f2;}}
_LL182: Cyc_Absyndump_dumpqvar(_tmp279->name);Cyc_Absyndump_dump(({const char*
_tmp2B1=" as ";_tag_dyneither(_tmp2B1,sizeof(char),5);}));Cyc_Absyndump_dumppat(
_tmp27A);goto _LL170;_LL183: {struct Cyc_Absyn_Tuple_p_struct*_tmp27B=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp262;if(_tmp27B->tag != 4)goto _LL185;else{_tmp27C=_tmp27B->f1;_tmp27D=_tmp27B->f2;}}
_LL184: {struct _dyneither_ptr term=_tmp27D?({const char*_tmp2B4=", ...)";
_tag_dyneither(_tmp2B4,sizeof(char),7);}):({const char*_tmp2B5=")";_tag_dyneither(
_tmp2B5,sizeof(char),2);});((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp27C,({const char*_tmp2B2="$(";_tag_dyneither(_tmp2B2,
sizeof(char),3);}),term,({const char*_tmp2B3=",";_tag_dyneither(_tmp2B3,sizeof(
char),2);}));goto _LL170;}_LL185: {struct Cyc_Absyn_Pointer_p_struct*_tmp27E=(
struct Cyc_Absyn_Pointer_p_struct*)_tmp262;if(_tmp27E->tag != 5)goto _LL187;else{
_tmp27F=_tmp27E->f1;}}_LL186: Cyc_Absyndump_dump(({const char*_tmp2B6="&";
_tag_dyneither(_tmp2B6,sizeof(char),2);}));Cyc_Absyndump_dumppat(_tmp27F);goto
_LL170;_LL187: {struct Cyc_Absyn_Reference_p_struct*_tmp280=(struct Cyc_Absyn_Reference_p_struct*)
_tmp262;if(_tmp280->tag != 2)goto _LL189;else{_tmp281=_tmp280->f1;_tmp282=_tmp280->f2;
_tmp283=*_tmp282;_tmp284=_tmp283.r;{struct Cyc_Absyn_Wild_p_struct*_tmp285=(
struct Cyc_Absyn_Wild_p_struct*)_tmp284;if(_tmp285->tag != 0)goto _LL189;};}}_LL188:
Cyc_Absyndump_dump(({const char*_tmp2B7="*";_tag_dyneither(_tmp2B7,sizeof(char),2);}));
Cyc_Absyndump_dumpqvar(_tmp281->name);goto _LL170;_LL189: {struct Cyc_Absyn_Reference_p_struct*
_tmp286=(struct Cyc_Absyn_Reference_p_struct*)_tmp262;if(_tmp286->tag != 2)goto
_LL18B;else{_tmp287=_tmp286->f1;_tmp288=_tmp286->f2;}}_LL18A: Cyc_Absyndump_dump(({
const char*_tmp2B8="*";_tag_dyneither(_tmp2B8,sizeof(char),2);}));Cyc_Absyndump_dumpqvar(
_tmp287->name);Cyc_Absyndump_dump(({const char*_tmp2B9=" as ";_tag_dyneither(
_tmp2B9,sizeof(char),5);}));Cyc_Absyndump_dumppat(_tmp288);goto _LL170;_LL18B: {
struct Cyc_Absyn_TagInt_p_struct*_tmp289=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp262;if(_tmp289->tag != 3)goto _LL18D;else{_tmp28A=_tmp289->f1;_tmp28B=_tmp289->f2;}}
_LL18C: Cyc_Absyndump_dumpqvar(_tmp28B->name);Cyc_Absyndump_dump_char((int)'<');
Cyc_Absyndump_dumptvar(_tmp28A);Cyc_Absyndump_dump_char((int)'>');goto _LL170;
_LL18D: {struct Cyc_Absyn_UnknownId_p_struct*_tmp28C=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp262;if(_tmp28C->tag != 14)goto _LL18F;else{_tmp28D=_tmp28C->f1;}}_LL18E: Cyc_Absyndump_dumpqvar(
_tmp28D);goto _LL170;_LL18F: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp28E=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp262;if(_tmp28E->tag != 15)goto _LL191;
else{_tmp28F=_tmp28E->f1;_tmp290=_tmp28E->f2;_tmp291=_tmp28E->f3;}}_LL190: {
struct _dyneither_ptr term=_tmp291?({const char*_tmp2BC=", ...)";_tag_dyneither(
_tmp2BC,sizeof(char),7);}):({const char*_tmp2BD=")";_tag_dyneither(_tmp2BD,
sizeof(char),2);});Cyc_Absyndump_dumpqvar(_tmp28F);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp290,({const
char*_tmp2BA="(";_tag_dyneither(_tmp2BA,sizeof(char),2);}),term,({const char*
_tmp2BB=",";_tag_dyneither(_tmp2BB,sizeof(char),2);}));goto _LL170;}_LL191: {
struct Cyc_Absyn_Aggr_p_struct*_tmp292=(struct Cyc_Absyn_Aggr_p_struct*)_tmp262;
if(_tmp292->tag != 6)goto _LL193;else{_tmp293=_tmp292->f1;if(_tmp293 == 0)goto
_LL193;_tmp294=*_tmp293;_tmp295=_tmp294.aggr_info;_tmp296=_tmp292->f2;_tmp297=
_tmp292->f3;_tmp298=_tmp292->f4;}}_LL192: {struct _tuple0*_tmp2BF;struct _tuple9
_tmp2BE=Cyc_Absyn_aggr_kinded_name(_tmp295);_tmp2BF=_tmp2BE.f2;{struct
_dyneither_ptr term=_tmp298?({const char*_tmp2C5=", ...)";_tag_dyneither(_tmp2C5,
sizeof(char),7);}):({const char*_tmp2C6=")";_tag_dyneither(_tmp2C6,sizeof(char),2);});
Cyc_Absyndump_dumpqvar(_tmp2BF);Cyc_Absyndump_dump_char((int)'{');((void(*)(void(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp296,({const char*_tmp2C0="[";_tag_dyneither(_tmp2C0,sizeof(char),2);}),({
const char*_tmp2C1="]";_tag_dyneither(_tmp2C1,sizeof(char),2);}),({const char*
_tmp2C2=",";_tag_dyneither(_tmp2C2,sizeof(char),2);}));((void(*)(void(*f)(struct
_tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr
end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp297,({
const char*_tmp2C3="";_tag_dyneither(_tmp2C3,sizeof(char),1);}),term,({const char*
_tmp2C4=",";_tag_dyneither(_tmp2C4,sizeof(char),2);}));goto _LL170;};}_LL193: {
struct Cyc_Absyn_Aggr_p_struct*_tmp299=(struct Cyc_Absyn_Aggr_p_struct*)_tmp262;
if(_tmp299->tag != 6)goto _LL195;else{_tmp29A=_tmp299->f1;if(_tmp29A != 0)goto
_LL195;_tmp29B=_tmp299->f2;_tmp29C=_tmp299->f3;_tmp29D=_tmp299->f4;}}_LL194: {
struct _dyneither_ptr term=_tmp29D?({const char*_tmp2CC=", ...)";_tag_dyneither(
_tmp2CC,sizeof(char),7);}):({const char*_tmp2CD=")";_tag_dyneither(_tmp2CD,
sizeof(char),2);});Cyc_Absyndump_dump_char((int)'{');((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp29B,({const char*_tmp2C7="[";_tag_dyneither(_tmp2C7,sizeof(char),2);}),({
const char*_tmp2C8="]";_tag_dyneither(_tmp2C8,sizeof(char),2);}),({const char*
_tmp2C9=",";_tag_dyneither(_tmp2C9,sizeof(char),2);}));((void(*)(void(*f)(struct
_tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr
end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp29C,({
const char*_tmp2CA="";_tag_dyneither(_tmp2CA,sizeof(char),1);}),term,({const char*
_tmp2CB=",";_tag_dyneither(_tmp2CB,sizeof(char),2);}));goto _LL170;}_LL195: {
struct Cyc_Absyn_Datatype_p_struct*_tmp29E=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp262;if(_tmp29E->tag != 7)goto _LL197;else{_tmp29F=_tmp29E->f2;_tmp2A0=_tmp29E->f3;
_tmp2A1=_tmp29E->f4;}}_LL196: {struct _dyneither_ptr term=_tmp2A1?({const char*
_tmp2D0=", ...)";_tag_dyneither(_tmp2D0,sizeof(char),7);}):({const char*_tmp2D1=")";
_tag_dyneither(_tmp2D1,sizeof(char),2);});Cyc_Absyndump_dumpqvar(_tmp29F->name);
if(_tmp2A0 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,
struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp2A0,({const char*_tmp2CE="(";_tag_dyneither(_tmp2CE,
sizeof(char),2);}),term,({const char*_tmp2CF=",";_tag_dyneither(_tmp2CF,sizeof(
char),2);}));goto _LL170;}_LL197: {struct Cyc_Absyn_Enum_p_struct*_tmp2A2=(struct
Cyc_Absyn_Enum_p_struct*)_tmp262;if(_tmp2A2->tag != 12)goto _LL199;else{_tmp2A3=
_tmp2A2->f2;}}_LL198: _tmp2A5=_tmp2A3;goto _LL19A;_LL199: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp2A4=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp262;if(_tmp2A4->tag != 13)goto
_LL19B;else{_tmp2A5=_tmp2A4->f2;}}_LL19A: Cyc_Absyndump_dumpqvar(_tmp2A5->name);
goto _LL170;_LL19B: {struct Cyc_Absyn_Exp_p_struct*_tmp2A6=(struct Cyc_Absyn_Exp_p_struct*)
_tmp262;if(_tmp2A6->tag != 16)goto _LL170;else{_tmp2A7=_tmp2A6->f1;}}_LL19C: Cyc_Absyndump_dumpexp(
_tmp2A7);goto _LL170;_LL170:;}void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){((void(*)(void(*
f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))
Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,fields,({const char*
_tmp2D2=",";_tag_dyneither(_tmp2D2,sizeof(char),2);}));}void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp2D3=" = ";_tag_dyneither(_tmp2D3,sizeof(char),
4);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void
Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){((void(*)(void(*f)(
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumpenumfield,fields,({const char*_tmp2D4=",";_tag_dyneither(
_tmp2D4,sizeof(char),2);}));}void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*
fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield _tmp2D6;
struct _dyneither_ptr*_tmp2D7;struct Cyc_Absyn_Tqual _tmp2D8;void*_tmp2D9;struct Cyc_Absyn_Exp*
_tmp2DA;struct Cyc_List_List*_tmp2DB;struct Cyc_Absyn_Aggrfield*_tmp2D5=(struct Cyc_Absyn_Aggrfield*)
fields->hd;_tmp2D6=*_tmp2D5;_tmp2D7=_tmp2D6.name;_tmp2D8=_tmp2D6.tq;_tmp2D9=
_tmp2D6.type;_tmp2DA=_tmp2D6.width;_tmp2DB=_tmp2D6.attributes;switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL19D:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp2D8,
_tmp2D9,Cyc_Absyndump_dump_str,_tmp2D7);Cyc_Absyndump_dumpatts(_tmp2DB);break;
case Cyc_Cyclone_Vc_c: _LL19E: Cyc_Absyndump_dumpatts(_tmp2DB);((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(
_tmp2D8,_tmp2D9,Cyc_Absyndump_dump_str,_tmp2D7);break;}if(_tmp2DA != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2DA);}Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(
td->name);Cyc_Absyndump_dumptvars(td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}struct _tuple18{void*f1;struct _tuple0*f2;};static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple18*pr){{void*_tmp2DC=(*pr).f1;_LL1A1: {struct Cyc_Absyn_Unused_att_struct*
_tmp2DD=(struct Cyc_Absyn_Unused_att_struct*)_tmp2DC;if(_tmp2DD->tag != 11)goto
_LL1A3;}_LL1A2: goto _LL1A0;_LL1A3: {struct Cyc_Absyn_Stdcall_att_struct*_tmp2DE=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmp2DC;if(_tmp2DE->tag != 1)goto _LL1A5;}
_LL1A4: Cyc_Absyndump_dump(({const char*_tmp2E1="_stdcall";_tag_dyneither(_tmp2E1,
sizeof(char),9);}));goto _LL1A0;_LL1A5: {struct Cyc_Absyn_Cdecl_att_struct*_tmp2DF=(
struct Cyc_Absyn_Cdecl_att_struct*)_tmp2DC;if(_tmp2DF->tag != 2)goto _LL1A7;}_LL1A6:
Cyc_Absyndump_dump(({const char*_tmp2E2="_cdecl";_tag_dyneither(_tmp2E2,sizeof(
char),7);}));goto _LL1A0;_LL1A7: {struct Cyc_Absyn_Fastcall_att_struct*_tmp2E0=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp2DC;if(_tmp2E0->tag != 3)goto _LL1A9;}
_LL1A8: Cyc_Absyndump_dump(({const char*_tmp2E3="_fastcall";_tag_dyneither(_tmp2E3,
sizeof(char),10);}));goto _LL1A0;_LL1A9:;_LL1AA: goto _LL1A0;_LL1A0:;}Cyc_Absyndump_dumpqvar((*
pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)
Cyc_Absyndump_dump_char((int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Vardecl _tmp2E5;enum Cyc_Absyn_Scope
_tmp2E6;struct _tuple0*_tmp2E7;struct Cyc_Absyn_Tqual _tmp2E8;void*_tmp2E9;struct
Cyc_Absyn_Exp*_tmp2EA;struct Cyc_List_List*_tmp2EB;struct Cyc_Absyn_Vardecl*
_tmp2E4=vd;_tmp2E5=*_tmp2E4;_tmp2E6=_tmp2E5.sc;_tmp2E7=_tmp2E5.name;_tmp2E8=
_tmp2E5.tq;_tmp2E9=_tmp2E5.type;_tmp2EA=_tmp2E5.initializer;_tmp2EB=_tmp2E5.attributes;
Cyc_Absyndump_dumploc(loc);switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c:
_LL1AB: if(_tmp2E6 == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){void*_tmp2EC=
Cyc_Tcutil_compress(_tmp2E9);_LL1AE: {struct Cyc_Absyn_FnType_struct*_tmp2ED=(
struct Cyc_Absyn_FnType_struct*)_tmp2EC;if(_tmp2ED->tag != 10)goto _LL1B0;}_LL1AF:
goto _LL1AD;_LL1B0:;_LL1B1: Cyc_Absyndump_dumpscope(_tmp2E6);_LL1AD:;}else{Cyc_Absyndump_dumpscope(
_tmp2E6);}((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct
_tuple0*))Cyc_Absyndump_dumptqtd)(_tmp2E8,_tmp2E9,Cyc_Absyndump_dumpqvar,_tmp2E7);
Cyc_Absyndump_dumpatts(_tmp2EB);break;case Cyc_Cyclone_Vc_c: _LL1AC: Cyc_Absyndump_dumpatts(
_tmp2EB);Cyc_Absyndump_dumpscope(_tmp2E6);{struct _RegionHandle _tmp2EE=
_new_region("temp");struct _RegionHandle*temp=& _tmp2EE;_push_region(temp);{struct
Cyc_Absyn_Tqual _tmp2F0;void*_tmp2F1;struct Cyc_List_List*_tmp2F2;struct _tuple11
_tmp2EF=Cyc_Absynpp_to_tms(temp,_tmp2E8,_tmp2E9);_tmp2F0=_tmp2EF.f1;_tmp2F1=
_tmp2EF.f2;_tmp2F2=_tmp2EF.f3;{void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;{
struct Cyc_List_List*tms2=_tmp2F2;for(0;tms2 != 0;tms2=tms2->tl){void*_tmp2F3=(
void*)tms2->hd;struct Cyc_List_List*_tmp2F5;_LL1B4: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp2F4=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp2F3;if(_tmp2F4->tag != 5)goto
_LL1B6;else{_tmp2F5=_tmp2F4->f2;}}_LL1B5: for(0;_tmp2F5 != 0;_tmp2F5=_tmp2F5->tl){
void*_tmp2F6=(void*)_tmp2F5->hd;_LL1B9: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp2F7=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp2F6;if(_tmp2F7->tag != 1)goto
_LL1BB;}_LL1BA: call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL1B8;_LL1BB: {
struct Cyc_Absyn_Cdecl_att_struct*_tmp2F8=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp2F6;if(_tmp2F8->tag != 2)goto _LL1BD;}_LL1BC: call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;
goto _LL1B8;_LL1BD: {struct Cyc_Absyn_Fastcall_att_struct*_tmp2F9=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp2F6;if(_tmp2F9->tag != 3)goto _LL1BF;}_LL1BE: call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;
goto _LL1B8;_LL1BF:;_LL1C0: goto _LL1B8;_LL1B8:;}goto _LL1B3;_LL1B6:;_LL1B7: goto
_LL1B3;_LL1B3:;}}Cyc_Absyndump_dumptq(_tmp2F0);Cyc_Absyndump_dumpntyp(_tmp2F1);{
struct _tuple18 _tmp2FA=({struct _tuple18 _tmp2FB;_tmp2FB.f1=call_conv;_tmp2FB.f2=
_tmp2E7;_tmp2FB;});((void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple18*),
struct _tuple18*a))Cyc_Absyndump_dumptms)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp2F2),Cyc_Absyndump_dump_callconv_qvar,& _tmp2FA);};};}
_npop_handler(0);break;;_pop_region(temp);};}if(_tmp2EA != 0){Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2EA);}Cyc_Absyndump_dump_semi();}
struct _tuple19{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d){void*_tmp2FC=d->r;struct Cyc_Absyn_Vardecl*_tmp2FE;struct
Cyc_Absyn_Fndecl*_tmp300;struct Cyc_Absyn_Aggrdecl*_tmp302;struct Cyc_Absyn_Datatypedecl*
_tmp304;struct Cyc_Absyn_Datatypedecl _tmp305;enum Cyc_Absyn_Scope _tmp306;struct
_tuple0*_tmp307;struct Cyc_List_List*_tmp308;struct Cyc_Core_Opt*_tmp309;int
_tmp30A;struct Cyc_Absyn_Enumdecl*_tmp30C;struct Cyc_Absyn_Enumdecl _tmp30D;enum 
Cyc_Absyn_Scope _tmp30E;struct _tuple0*_tmp30F;struct Cyc_Core_Opt*_tmp310;struct
Cyc_Absyn_Pat*_tmp312;struct Cyc_Absyn_Exp*_tmp313;struct Cyc_List_List*_tmp315;
struct Cyc_Absyn_Tvar*_tmp317;struct Cyc_Absyn_Vardecl*_tmp318;int _tmp319;struct
Cyc_Absyn_Exp*_tmp31A;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_Absyn_Tvar*_tmp31D;
struct Cyc_Absyn_Vardecl*_tmp31E;struct Cyc_Absyn_Typedefdecl*_tmp320;struct
_dyneither_ptr*_tmp322;struct Cyc_List_List*_tmp323;struct _tuple0*_tmp325;struct
Cyc_List_List*_tmp326;struct Cyc_List_List*_tmp328;struct Cyc_List_List*_tmp32A;
struct Cyc_List_List*_tmp32B;_LL1C2: {struct Cyc_Absyn_Var_d_struct*_tmp2FD=(
struct Cyc_Absyn_Var_d_struct*)_tmp2FC;if(_tmp2FD->tag != 0)goto _LL1C4;else{
_tmp2FE=_tmp2FD->f1;}}_LL1C3: Cyc_Absyndump_dumpvardecl(_tmp2FE,d->loc);goto
_LL1C1;_LL1C4: {struct Cyc_Absyn_Fn_d_struct*_tmp2FF=(struct Cyc_Absyn_Fn_d_struct*)
_tmp2FC;if(_tmp2FF->tag != 1)goto _LL1C6;else{_tmp300=_tmp2FF->f1;}}_LL1C5: Cyc_Absyndump_dumploc(
d->loc);switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL1E2: Cyc_Absyndump_dumpatts(
_tmp300->attributes);break;case Cyc_Cyclone_Gcc_c: _LL1E3: break;}if(_tmp300->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp32E=Cyc_Cyclone_c_compiler;_LL1E6: if(_tmp32E != 
Cyc_Cyclone_Vc_c)goto _LL1E8;_LL1E7: Cyc_Absyndump_dump(({const char*_tmp32F="__inline";
_tag_dyneither(_tmp32F,sizeof(char),9);}));goto _LL1E5;_LL1E8:;_LL1E9: Cyc_Absyndump_dump(({
const char*_tmp330="inline";_tag_dyneither(_tmp330,sizeof(char),7);}));goto _LL1E5;
_LL1E5:;}Cyc_Absyndump_dumpscope(_tmp300->sc);{void*t=(void*)({struct Cyc_Absyn_FnType_struct*
_tmp331=_cycalloc(sizeof(*_tmp331));_tmp331[0]=({struct Cyc_Absyn_FnType_struct
_tmp332;_tmp332.tag=10;_tmp332.f1=({struct Cyc_Absyn_FnInfo _tmp333;_tmp333.tvars=
_tmp300->tvs;_tmp333.effect=_tmp300->effect;_tmp333.ret_typ=_tmp300->ret_type;
_tmp333.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple10*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp300->args);
_tmp333.c_varargs=_tmp300->c_varargs;_tmp333.cyc_varargs=_tmp300->cyc_varargs;
_tmp333.rgn_po=_tmp300->rgn_po;_tmp333.attributes=0;_tmp333;});_tmp332;});
_tmp331;});switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1EA:((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,
_tmp300);break;case Cyc_Cyclone_Vc_c: _LL1EB:((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(
Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,_tmp300);break;}
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp300->body,0);Cyc_Absyndump_dump_char((
int)'}');goto _LL1C1;};_LL1C6: {struct Cyc_Absyn_Aggr_d_struct*_tmp301=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp2FC;if(_tmp301->tag != 6)goto _LL1C8;else{_tmp302=_tmp301->f1;}}_LL1C7: Cyc_Absyndump_dumpscope(
_tmp302->sc);Cyc_Absyndump_dumpaggr_kind(_tmp302->kind);Cyc_Absyndump_dumpqvar(
_tmp302->name);Cyc_Absyndump_dumptvars(_tmp302->tvs);if(_tmp302->impl == 0)Cyc_Absyndump_dump_semi();
else{Cyc_Absyndump_dump_char((int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp302->impl))->exist_vars != 0)((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp302->impl))->exist_vars,({const char*
_tmp334="<";_tag_dyneither(_tmp334,sizeof(char),2);}),({const char*_tmp335=">";
_tag_dyneither(_tmp335,sizeof(char),2);}),({const char*_tmp336=",";_tag_dyneither(
_tmp336,sizeof(char),2);}));if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp302->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp302->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp302->impl))->fields);Cyc_Absyndump_dump(({
const char*_tmp337="}";_tag_dyneither(_tmp337,sizeof(char),2);}));Cyc_Absyndump_dumpatts(
_tmp302->attributes);Cyc_Absyndump_dump(({const char*_tmp338=";";_tag_dyneither(
_tmp338,sizeof(char),2);}));}goto _LL1C1;_LL1C8: {struct Cyc_Absyn_Datatype_d_struct*
_tmp303=(struct Cyc_Absyn_Datatype_d_struct*)_tmp2FC;if(_tmp303->tag != 7)goto
_LL1CA;else{_tmp304=_tmp303->f1;_tmp305=*_tmp304;_tmp306=_tmp305.sc;_tmp307=
_tmp305.name;_tmp308=_tmp305.tvs;_tmp309=_tmp305.fields;_tmp30A=_tmp305.is_extensible;}}
_LL1C9: Cyc_Absyndump_dumpscope(_tmp306);if(_tmp30A)Cyc_Absyndump_dump(({const
char*_tmp339="@extensible ";_tag_dyneither(_tmp339,sizeof(char),13);}));Cyc_Absyndump_dump(({
const char*_tmp33A="datatype ";_tag_dyneither(_tmp33A,sizeof(char),10);}));Cyc_Absyndump_dumpqvar(
_tmp307);Cyc_Absyndump_dumptvars(_tmp308);if(_tmp309 == 0)Cyc_Absyndump_dump_semi();
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpdatatypefields((struct
Cyc_List_List*)_tmp309->v);Cyc_Absyndump_dump_nospace(({const char*_tmp33B="};";
_tag_dyneither(_tmp33B,sizeof(char),3);}));}goto _LL1C1;_LL1CA: {struct Cyc_Absyn_Enum_d_struct*
_tmp30B=(struct Cyc_Absyn_Enum_d_struct*)_tmp2FC;if(_tmp30B->tag != 8)goto _LL1CC;
else{_tmp30C=_tmp30B->f1;_tmp30D=*_tmp30C;_tmp30E=_tmp30D.sc;_tmp30F=_tmp30D.name;
_tmp310=_tmp30D.fields;}}_LL1CB: Cyc_Absyndump_dumpscope(_tmp30E);Cyc_Absyndump_dump(({
const char*_tmp33C="enum ";_tag_dyneither(_tmp33C,sizeof(char),6);}));Cyc_Absyndump_dumpqvar(
_tmp30F);if(_tmp310 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp310->v);Cyc_Absyndump_dump_nospace(({
const char*_tmp33D="};";_tag_dyneither(_tmp33D,sizeof(char),3);}));}return;_LL1CC: {
struct Cyc_Absyn_Let_d_struct*_tmp311=(struct Cyc_Absyn_Let_d_struct*)_tmp2FC;if(
_tmp311->tag != 2)goto _LL1CE;else{_tmp312=_tmp311->f1;_tmp313=_tmp311->f3;}}
_LL1CD: Cyc_Absyndump_dump(({const char*_tmp33E="let";_tag_dyneither(_tmp33E,
sizeof(char),4);}));Cyc_Absyndump_dumppat(_tmp312);Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp313);Cyc_Absyndump_dump_semi();goto _LL1C1;_LL1CE: {
struct Cyc_Absyn_Letv_d_struct*_tmp314=(struct Cyc_Absyn_Letv_d_struct*)_tmp2FC;
if(_tmp314->tag != 3)goto _LL1D0;else{_tmp315=_tmp314->f1;}}_LL1CF: Cyc_Absyndump_dump(({
const char*_tmp33F="let ";_tag_dyneither(_tmp33F,sizeof(char),5);}));Cyc_Absyndump_dumpids(
_tmp315);Cyc_Absyndump_dump_semi();goto _LL1C1;_LL1D0: {struct Cyc_Absyn_Region_d_struct*
_tmp316=(struct Cyc_Absyn_Region_d_struct*)_tmp2FC;if(_tmp316->tag != 4)goto _LL1D2;
else{_tmp317=_tmp316->f1;_tmp318=_tmp316->f2;_tmp319=_tmp316->f3;_tmp31A=_tmp316->f4;}}
_LL1D1: Cyc_Absyndump_dump(({const char*_tmp340="region";_tag_dyneither(_tmp340,
sizeof(char),7);}));Cyc_Absyndump_dump(({const char*_tmp341="<";_tag_dyneither(
_tmp341,sizeof(char),2);}));Cyc_Absyndump_dumptvar(_tmp317);Cyc_Absyndump_dump(({
const char*_tmp342="> ";_tag_dyneither(_tmp342,sizeof(char),3);}));Cyc_Absyndump_dumpqvar(
_tmp318->name);if(_tmp319)Cyc_Absyndump_dump(({const char*_tmp343="@resetable";
_tag_dyneither(_tmp343,sizeof(char),11);}));if(_tmp31A != 0){Cyc_Absyndump_dump(({
const char*_tmp344=" = open(";_tag_dyneither(_tmp344,sizeof(char),9);}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp31A);Cyc_Absyndump_dump(({const char*_tmp345=")";
_tag_dyneither(_tmp345,sizeof(char),2);}));}Cyc_Absyndump_dump_semi();goto _LL1C1;
_LL1D2: {struct Cyc_Absyn_Alias_d_struct*_tmp31B=(struct Cyc_Absyn_Alias_d_struct*)
_tmp2FC;if(_tmp31B->tag != 5)goto _LL1D4;else{_tmp31C=_tmp31B->f1;_tmp31D=_tmp31B->f2;
_tmp31E=_tmp31B->f3;}}_LL1D3: Cyc_Absyndump_dump(({const char*_tmp346="alias ";
_tag_dyneither(_tmp346,sizeof(char),7);}));Cyc_Absyndump_dump(({const char*
_tmp347="<";_tag_dyneither(_tmp347,sizeof(char),2);}));Cyc_Absyndump_dumptvar(
_tmp31D);Cyc_Absyndump_dump(({const char*_tmp348=">";_tag_dyneither(_tmp348,
sizeof(char),2);}));Cyc_Absyndump_dumpqvar(_tmp31E->name);Cyc_Absyndump_dump(({
const char*_tmp349=" = ";_tag_dyneither(_tmp349,sizeof(char),4);}));Cyc_Absyndump_dumpexp(
_tmp31C);Cyc_Absyndump_dump_semi();goto _LL1C1;_LL1D4: {struct Cyc_Absyn_Typedef_d_struct*
_tmp31F=(struct Cyc_Absyn_Typedef_d_struct*)_tmp2FC;if(_tmp31F->tag != 9)goto
_LL1D6;else{_tmp320=_tmp31F->f1;}}_LL1D5: if(!Cyc_Absyndump_expand_typedefs  || 
_tmp320->defn != 0  && Cyc_Absynpp_is_anon_aggrtype((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp320->defn))->v)){Cyc_Absyndump_dump(({const char*_tmp34A="typedef";
_tag_dyneither(_tmp34A,sizeof(char),8);}));{void*t;if(_tmp320->defn == 0)t=Cyc_Absyn_new_evar(
_tmp320->kind,0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp320->defn))->v;}((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp320->tq,t,Cyc_Absyndump_dumptypedefname,
_tmp320);Cyc_Absyndump_dumpatts(_tmp320->atts);Cyc_Absyndump_dump_semi();};}goto
_LL1C1;_LL1D6: {struct Cyc_Absyn_Namespace_d_struct*_tmp321=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp2FC;if(_tmp321->tag != 10)goto _LL1D8;else{_tmp322=_tmp321->f1;_tmp323=_tmp321->f2;}}
_LL1D7: Cyc_Absyndump_dump(({const char*_tmp34B="namespace";_tag_dyneither(_tmp34B,
sizeof(char),10);}));Cyc_Absyndump_dump_str(_tmp322);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp323 != 0;_tmp323=_tmp323->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp323->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1C1;
_LL1D8: {struct Cyc_Absyn_Using_d_struct*_tmp324=(struct Cyc_Absyn_Using_d_struct*)
_tmp2FC;if(_tmp324->tag != 11)goto _LL1DA;else{_tmp325=_tmp324->f1;_tmp326=_tmp324->f2;}}
_LL1D9: Cyc_Absyndump_dump(({const char*_tmp34C="using";_tag_dyneither(_tmp34C,
sizeof(char),6);}));Cyc_Absyndump_dumpqvar(_tmp325);Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp326 != 0;_tmp326=_tmp326->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)
_tmp326->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1C1;_LL1DA: {struct Cyc_Absyn_ExternC_d_struct*
_tmp327=(struct Cyc_Absyn_ExternC_d_struct*)_tmp2FC;if(_tmp327->tag != 12)goto
_LL1DC;else{_tmp328=_tmp327->f1;}}_LL1DB: Cyc_Absyndump_dump(({const char*_tmp34D="extern \"C\" {";
_tag_dyneither(_tmp34D,sizeof(char),13);}));for(0;_tmp328 != 0;_tmp328=_tmp328->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp328->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1C1;_LL1DC: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp329=(
struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp2FC;if(_tmp329->tag != 13)goto _LL1DE;
else{_tmp32A=_tmp329->f1;_tmp32B=_tmp329->f2;}}_LL1DD: Cyc_Absyndump_dump(({const
char*_tmp34E="extern \"C include\" {";_tag_dyneither(_tmp34E,sizeof(char),21);}));
for(0;_tmp32A != 0;_tmp32A=_tmp32A->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)
_tmp32A->hd);}Cyc_Absyndump_dump_char((int)'}');if(_tmp32B != 0){Cyc_Absyndump_dump(({
const char*_tmp34F=" export {";_tag_dyneither(_tmp34F,sizeof(char),10);}));for(0;
_tmp32B != 0;_tmp32B=_tmp32B->tl){struct _tuple0*_tmp351;struct _tuple19 _tmp350=*((
struct _tuple19*)_tmp32B->hd);_tmp351=_tmp350.f2;Cyc_Absyndump_dumpqvar(_tmp351);
if(_tmp32B->tl != 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump(({const
char*_tmp352="}";_tag_dyneither(_tmp352,sizeof(char),2);}));}goto _LL1C1;_LL1DE: {
struct Cyc_Absyn_Porton_d_struct*_tmp32C=(struct Cyc_Absyn_Porton_d_struct*)
_tmp2FC;if(_tmp32C->tag != 14)goto _LL1E0;}_LL1DF: Cyc_Absyndump_dump(({const char*
_tmp353=" __cyclone_port_on__; ";_tag_dyneither(_tmp353,sizeof(char),23);}));
goto _LL1C1;_LL1E0: {struct Cyc_Absyn_Portoff_d_struct*_tmp32D=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp2FC;if(_tmp32D->tag != 15)goto _LL1C1;}_LL1E1: Cyc_Absyndump_dump(({const char*
_tmp354=" __cyclone_port_off__; ";_tag_dyneither(_tmp354,sizeof(char),24);}));
goto _LL1C1;_LL1C1:;}static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*
e){struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1 != 1  || !pr.f2){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmp355=(void*)tms->hd;struct Cyc_Absyn_PtrAtts _tmp357;void*_tmp358;union Cyc_Absyn_Constraint*
_tmp359;union Cyc_Absyn_Constraint*_tmp35A;union Cyc_Absyn_Constraint*_tmp35B;
struct Cyc_Absyn_Tqual _tmp35C;_LL1EE: {struct Cyc_Absyn_Pointer_mod_struct*_tmp356=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp355;if(_tmp356->tag != 2)goto _LL1F0;else{
_tmp357=_tmp356->f1;_tmp358=_tmp357.rgn;_tmp359=_tmp357.nullable;_tmp35A=_tmp357.bounds;
_tmp35B=_tmp357.zero_term;_tmp35C=_tmp356->f2;}}_LL1EF:{void*_tmp35D=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp35A);struct Cyc_Absyn_Exp*_tmp360;_LL1F3: {struct Cyc_Absyn_DynEither_b_struct*
_tmp35E=(struct Cyc_Absyn_DynEither_b_struct*)_tmp35D;if(_tmp35E->tag != 0)goto
_LL1F5;}_LL1F4: Cyc_Absyndump_dump_char((int)'?');goto _LL1F2;_LL1F5: {struct Cyc_Absyn_Upper_b_struct*
_tmp35F=(struct Cyc_Absyn_Upper_b_struct*)_tmp35D;if(_tmp35F->tag != 1)goto _LL1F2;
else{_tmp360=_tmp35F->f1;}}_LL1F6: Cyc_Absyndump_dump_char((int)(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp359)?'*':'@'));Cyc_Absyndump_dump_upperbound(
_tmp360);goto _LL1F2;_LL1F2:;}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp35B))Cyc_Absyndump_dump(({const char*_tmp361="@zeroterm";_tag_dyneither(
_tmp361,sizeof(char),10);}));{void*_tmp362=Cyc_Tcutil_compress(_tmp358);struct
Cyc_Absyn_Tvar*_tmp366;struct Cyc_Core_Opt*_tmp368;_LL1F8: {struct Cyc_Absyn_HeapRgn_struct*
_tmp363=(struct Cyc_Absyn_HeapRgn_struct*)_tmp362;if(_tmp363->tag != 21)goto _LL1FA;}
_LL1F9: goto _LL1F7;_LL1FA: {struct Cyc_Absyn_UniqueRgn_struct*_tmp364=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp362;if(_tmp364->tag != 22)goto _LL1FC;}_LL1FB: Cyc_Absyndump_dump(({const char*
_tmp369="`U";_tag_dyneither(_tmp369,sizeof(char),3);}));goto _LL1F7;_LL1FC: {
struct Cyc_Absyn_VarType_struct*_tmp365=(struct Cyc_Absyn_VarType_struct*)_tmp362;
if(_tmp365->tag != 2)goto _LL1FE;else{_tmp366=_tmp365->f1;}}_LL1FD: Cyc_Absyndump_dump_str(
_tmp366->name);goto _LL1F7;_LL1FE: {struct Cyc_Absyn_Evar_struct*_tmp367=(struct
Cyc_Absyn_Evar_struct*)_tmp362;if(_tmp367->tag != 1)goto _LL200;else{_tmp368=
_tmp367->f2;if(_tmp368 != 0)goto _LL200;}}_LL1FF: Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(
_tmp358));goto _LL1F7;_LL200:;_LL201:({void*_tmp36A=0;Cyc_Tcutil_impos(({const
char*_tmp36B="dumptms: bad rgn type in Pointer_mod";_tag_dyneither(_tmp36B,
sizeof(char),37);}),_tag_dyneither(_tmp36A,sizeof(void*),0));});_LL1F7:;}Cyc_Absyndump_dumptq(
_tmp35C);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1F0:;_LL1F1: {int
next_is_pointer=0;if(tms->tl != 0){void*_tmp36C=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;_LL203: {struct Cyc_Absyn_Pointer_mod_struct*_tmp36D=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp36C;if(_tmp36D->tag != 2)goto _LL205;}
_LL204: next_is_pointer=1;goto _LL202;_LL205:;_LL206: goto _LL202;_LL202:;}if(
next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp36E=(void*)
tms->hd;union Cyc_Absyn_Constraint*_tmp370;struct Cyc_Absyn_Exp*_tmp372;union Cyc_Absyn_Constraint*
_tmp373;void*_tmp375;struct Cyc_List_List*_tmp377;int _tmp378;struct Cyc_Absyn_VarargInfo*
_tmp379;struct Cyc_Core_Opt*_tmp37A;struct Cyc_List_List*_tmp37B;void*_tmp37D;
struct Cyc_List_List*_tmp37F;struct Cyc_Position_Segment*_tmp380;struct Cyc_List_List*
_tmp382;struct Cyc_Position_Segment*_tmp383;int _tmp384;struct Cyc_List_List*
_tmp386;_LL208: {struct Cyc_Absyn_Carray_mod_struct*_tmp36F=(struct Cyc_Absyn_Carray_mod_struct*)
_tmp36E;if(_tmp36F->tag != 0)goto _LL20A;else{_tmp370=_tmp36F->f1;}}_LL209: Cyc_Absyndump_dump(({
const char*_tmp388="[]";_tag_dyneither(_tmp388,sizeof(char),3);}));if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp370))Cyc_Absyndump_dump(({
const char*_tmp389="@zeroterm";_tag_dyneither(_tmp389,sizeof(char),10);}));goto
_LL207;_LL20A: {struct Cyc_Absyn_ConstArray_mod_struct*_tmp371=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp36E;if(_tmp371->tag != 1)goto _LL20C;else{_tmp372=_tmp371->f1;_tmp373=_tmp371->f2;}}
_LL20B: Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp372);Cyc_Absyndump_dump_char((
int)']');if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp373))Cyc_Absyndump_dump(({const char*_tmp38A="@zeroterm";_tag_dyneither(
_tmp38A,sizeof(char),10);}));goto _LL207;_LL20C: {struct Cyc_Absyn_Function_mod_struct*
_tmp374=(struct Cyc_Absyn_Function_mod_struct*)_tmp36E;if(_tmp374->tag != 3)goto
_LL20E;else{_tmp375=(void*)_tmp374->f1;{struct Cyc_Absyn_WithTypes_struct*_tmp376=(
struct Cyc_Absyn_WithTypes_struct*)_tmp375;if(_tmp376->tag != 1)goto _LL20E;else{
_tmp377=_tmp376->f1;_tmp378=_tmp376->f2;_tmp379=_tmp376->f3;_tmp37A=_tmp376->f4;
_tmp37B=_tmp376->f5;}};}}_LL20D: Cyc_Absyndump_dumpfunargs(_tmp377,_tmp378,
_tmp379,_tmp37A,_tmp37B);goto _LL207;_LL20E: {struct Cyc_Absyn_Function_mod_struct*
_tmp37C=(struct Cyc_Absyn_Function_mod_struct*)_tmp36E;if(_tmp37C->tag != 3)goto
_LL210;else{_tmp37D=(void*)_tmp37C->f1;{struct Cyc_Absyn_NoTypes_struct*_tmp37E=(
struct Cyc_Absyn_NoTypes_struct*)_tmp37D;if(_tmp37E->tag != 0)goto _LL210;else{
_tmp37F=_tmp37E->f1;_tmp380=_tmp37E->f2;}};}}_LL20F:((void(*)(void(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,
_tmp37F,({const char*_tmp38B="(";_tag_dyneither(_tmp38B,sizeof(char),2);}),({
const char*_tmp38C=")";_tag_dyneither(_tmp38C,sizeof(char),2);}),({const char*
_tmp38D=",";_tag_dyneither(_tmp38D,sizeof(char),2);}));goto _LL207;_LL210: {
struct Cyc_Absyn_TypeParams_mod_struct*_tmp381=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp36E;if(_tmp381->tag != 4)goto _LL212;else{_tmp382=_tmp381->f1;_tmp383=_tmp381->f2;
_tmp384=_tmp381->f3;}}_LL211: if(_tmp384)Cyc_Absyndump_dumpkindedtvars(_tmp382);
else{Cyc_Absyndump_dumptvars(_tmp382);}goto _LL207;_LL212: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp385=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp36E;if(_tmp385->tag != 5)goto
_LL214;else{_tmp386=_tmp385->f2;}}_LL213: Cyc_Absyndump_dumpatts(_tmp386);goto
_LL207;_LL214: {struct Cyc_Absyn_Pointer_mod_struct*_tmp387=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp36E;if(_tmp387->tag != 2)goto _LL207;}_LL215:({void*_tmp38E=0;Cyc_Tcutil_impos(({
const char*_tmp38F="dumptms";_tag_dyneither(_tmp38F,sizeof(char),8);}),
_tag_dyneither(_tmp38E,sizeof(void*),0));});_LL207:;}return;}_LL1ED:;};}void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){struct _RegionHandle
_tmp390=_new_region("temp");struct _RegionHandle*temp=& _tmp390;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp392;void*_tmp393;struct Cyc_List_List*_tmp394;struct
_tuple11 _tmp391=Cyc_Absynpp_to_tms(temp,tq,t);_tmp392=_tmp391.f1;_tmp393=_tmp391.f2;
_tmp394=_tmp391.f3;Cyc_Absyndump_dumptq(_tmp392);Cyc_Absyndump_dumpntyp(_tmp393);
Cyc_Absyndump_dumptms(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp394),f,a);};_pop_region(temp);}void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=f;for(0;
tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}({
void*_tmp395=0;Cyc_fprintf(f,({const char*_tmp396="\n";_tag_dyneither(_tmp396,
sizeof(char),2);}),_tag_dyneither(_tmp395,sizeof(void*),0));});}
