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
void*v;};int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);int Cyc_List_length(
struct Cyc_List_List*x);void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_tl(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rcopy(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};void
Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);int Cyc_List_mem(int(*
compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_List_List*Cyc_List_delete_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
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
char*tag;};struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(
void*,void*));int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**
ans);void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);void Cyc_Dict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict
Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2);struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,
void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct
_tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct
_dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;int Cyc_Position_segment_equals(struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*);struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
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
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
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
char*tag;};extern struct Cyc_Absyn_EmptyAnnot_struct Cyc_Absyn_EmptyAnnot_val;int
Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_nontagged_union_type(
void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
int Cyc_Tcutil_typecmp(void*,void*);int Cyc_Tcutil_bits_only(void*t);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct _tuple10{unsigned int f1;int f2;};struct _tuple10 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
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
Cyc_CfFlowInfo_EqualConst(unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*,void*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int);union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(struct Cyc_Absyn_Vardecl*);
struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp
rop;};struct Cyc_CfFlowInfo_TagCmp{enum Cyc_Absyn_Primop cmp;void*bd;};char Cyc_CfFlowInfo_HasTagCmps[
11]="HasTagCmps";struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*
f1;};char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_struct{
char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_struct{
char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";
struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};extern
struct Cyc_CfFlowInfo_IsZero_struct Cyc_CfFlowInfo_IsZero_val;struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);struct
_union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct
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
set,void*place,struct Cyc_Position_Segment*loc);int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);int Cyc_CfFlowInfo_place_set_equals(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(
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
fs,struct _dyneither_ptr*f);int Cyc_CfFlowInfo_root_cmp(void*,void*);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);enum Cyc_CfFlowInfo_InitLevel
Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*
r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,
union Cyc_CfFlowInfo_FlowInfo f2);void Cyc_CfFlowInfo_print_absrval(void*rval);void
Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);void Cyc_CfFlowInfo_print_root(
void*root);void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);void Cyc_CfFlowInfo_print_dict_set(
struct Cyc_Dict_Dict p,void(*pr)(void*));void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*
p,void(*pr)(void*));void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);void
Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
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
int tag;struct Cyc_Absyn_Tvar*f1;};extern struct Cyc_CfFlowInfo_UniqueRgn_k_struct
Cyc_CfFlowInfo_UniqueRgn_k_val;int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);void Cyc_CfFlowInfo_check_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f);union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f);struct _tuple14{struct Cyc_CfFlowInfo_ConsumeInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple14 Cyc_CfFlowInfo_save_consume_info(
struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo f,int clear);union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo
c,int may_consume_only);struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_IsZero_struct
Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};struct Cyc_CfFlowInfo_UniqueRgn_k_struct
Cyc_CfFlowInfo_UniqueRgn_k_val={0};union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(
struct Cyc_CfFlowInfo_Place*x){return({union Cyc_CfFlowInfo_AbsLVal _tmp2;(_tmp2.PlaceL).val=
x;(_tmp2.PlaceL).tag=1;_tmp2;});}union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
return({union Cyc_CfFlowInfo_AbsLVal _tmp3;(_tmp3.UnknownL).val=0;(_tmp3.UnknownL).tag=
2;_tmp3;});}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){return({union
Cyc_CfFlowInfo_FlowInfo _tmp4;(_tmp4.BottomFL).val=0;(_tmp4.BottomFL).tag=1;_tmp4;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,
struct Cyc_List_List*r,struct Cyc_CfFlowInfo_ConsumeInfo c){return({union Cyc_CfFlowInfo_FlowInfo
_tmp5;(_tmp5.ReachableFL).val=({struct _tuple12 _tmp6;_tmp6.f1=fd;_tmp6.f2=r;_tmp6.f3=
c;_tmp6;});(_tmp5.ReachableFL).tag=2;_tmp5;});}int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);int Cyc_CfFlowInfo_root_cmp(
void*r1,void*r2);struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r){static struct Cyc_Absyn_Const_e_struct dummy_rawexp={0,{.Null_c={1,
0}}};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp7=_region_malloc(r,sizeof(*_tmp7));
_tmp7->r=r;_tmp7->zero=(void*)({struct Cyc_CfFlowInfo_Zero_struct*_tmp1B=
_region_malloc(r,sizeof(*_tmp1B));_tmp1B[0]=({struct Cyc_CfFlowInfo_Zero_struct
_tmp1C;_tmp1C.tag=0;_tmp1C;});_tmp1B;});_tmp7->notzeroall=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp19=_region_malloc(r,sizeof(*_tmp19));_tmp19[0]=({struct Cyc_CfFlowInfo_NotZeroAll_struct
_tmp1A;_tmp1A.tag=1;_tmp1A;});_tmp19;});_tmp7->notzerothis=(void*)({struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp17=_region_malloc(r,sizeof(*_tmp17));_tmp17[0]=({struct Cyc_CfFlowInfo_NotZeroThis_struct
_tmp18;_tmp18.tag=2;_tmp18;});_tmp17;});_tmp7->unknown_none=(void*)({struct Cyc_CfFlowInfo_UnknownR_struct*
_tmp15=_region_malloc(r,sizeof(*_tmp15));_tmp15[0]=({struct Cyc_CfFlowInfo_UnknownR_struct
_tmp16;_tmp16.tag=3;_tmp16.f1=Cyc_CfFlowInfo_NoneIL;_tmp16;});_tmp15;});_tmp7->unknown_this=(
void*)({struct Cyc_CfFlowInfo_UnknownR_struct*_tmp13=_region_malloc(r,sizeof(*
_tmp13));_tmp13[0]=({struct Cyc_CfFlowInfo_UnknownR_struct _tmp14;_tmp14.tag=3;
_tmp14.f1=Cyc_CfFlowInfo_ThisIL;_tmp14;});_tmp13;});_tmp7->unknown_all=(void*)({
struct Cyc_CfFlowInfo_UnknownR_struct*_tmp11=_region_malloc(r,sizeof(*_tmp11));
_tmp11[0]=({struct Cyc_CfFlowInfo_UnknownR_struct _tmp12;_tmp12.tag=3;_tmp12.f1=
Cyc_CfFlowInfo_AllIL;_tmp12;});_tmp11;});_tmp7->esc_none=(void*)({struct Cyc_CfFlowInfo_Esc_struct*
_tmpF=_region_malloc(r,sizeof(*_tmpF));_tmpF[0]=({struct Cyc_CfFlowInfo_Esc_struct
_tmp10;_tmp10.tag=4;_tmp10.f1=Cyc_CfFlowInfo_NoneIL;_tmp10;});_tmpF;});_tmp7->esc_this=(
void*)({struct Cyc_CfFlowInfo_Esc_struct*_tmpD=_region_malloc(r,sizeof(*_tmpD));
_tmpD[0]=({struct Cyc_CfFlowInfo_Esc_struct _tmpE;_tmpE.tag=4;_tmpE.f1=Cyc_CfFlowInfo_ThisIL;
_tmpE;});_tmpD;});_tmp7->esc_all=(void*)({struct Cyc_CfFlowInfo_Esc_struct*_tmpB=
_region_malloc(r,sizeof(*_tmpB));_tmpB[0]=({struct Cyc_CfFlowInfo_Esc_struct _tmpC;
_tmpC.tag=4;_tmpC.f1=Cyc_CfFlowInfo_AllIL;_tmpC;});_tmpB;});_tmp7->mt_flowdict=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(
r,Cyc_CfFlowInfo_root_cmp);_tmp7->mt_place_set=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))
Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp);_tmp7->dummy_place=({struct Cyc_CfFlowInfo_Place*
_tmp8=_region_malloc(r,sizeof(*_tmp8));_tmp8->root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp9=_region_malloc(r,sizeof(*_tmp9));_tmp9[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmpA;_tmpA.tag=1;_tmpA.f1=& dummy_exp;_tmpA.f2=(void*)((void*)& Cyc_Absyn_VoidType_val);
_tmpA;});_tmp9;});_tmp8->fields=0;_tmp8;});_tmp7;});}int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,void*place,struct Cyc_Position_Segment*loc){if(set != 0){
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,place))return 1;
else{*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Position_Segment*
v))Cyc_Dict_insert)(*set,place,loc);}}return 0;}struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place){void*_tmp1F;struct Cyc_List_List*_tmp20;struct
Cyc_CfFlowInfo_Place _tmp1E=*place;_tmp1F=_tmp1E.root;_tmp20=_tmp1E.fields;{void*
_tmp21=_tmp1F;struct Cyc_Absyn_Vardecl*_tmp23;_LL1: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp22=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp21;if(_tmp22->tag != 0)goto _LL3;
else{_tmp23=_tmp22->f1;}}_LL2: if(_tmp20 == 0)return Cyc_Absynpp_qvar2string(_tmp23->name);
else{return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp26;_tmp26.tag=0;
_tmp26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp23->name));({void*_tmp24[1]={& _tmp26};Cyc_aprintf(({const char*_tmp25="reachable from %s";
_tag_dyneither(_tmp25,sizeof(char),18);}),_tag_dyneither(_tmp24,sizeof(void*),1));});});}
_LL3:;_LL4:({void*_tmp27=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp28="error locations not for VarRoots";
_tag_dyneither(_tmp28,sizeof(char),33);}),_tag_dyneither(_tmp27,sizeof(void*),0));});
_LL0:;};}struct _dyneither_ptr Cyc_CfFlowInfo_uplace_err_string(struct Cyc_CfFlowInfo_UniquePlace*
place){return Cyc_CfFlowInfo_place_err_string(& place->place);}static void Cyc_CfFlowInfo_unique_err(
void*place,struct _dyneither_ptr(*toString)(void*),struct _dyneither_ptr err_msg1,
struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*consumed_loc,struct Cyc_Position_Segment*
loc){struct _dyneither_ptr _tmp29=toString(place);if(!Cyc_Position_segment_equals(
consumed_loc,loc)){struct _dyneither_ptr _tmp2A=Cyc_Position_string_of_segment(
consumed_loc);({struct Cyc_String_pa_struct _tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct
_dyneither_ptr)_tmp2A;({struct Cyc_String_pa_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=(
struct _dyneither_ptr)_tmp29;({void*_tmp2B[2]={& _tmp2C,& _tmp2D};Cyc_Tcutil_terr(
loc,err_msg1,_tag_dyneither(_tmp2B,sizeof(void*),2));});});});}else{({struct Cyc_String_pa_struct
_tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _dyneither_ptr)_tmp29;({void*_tmp2E[1]={&
_tmp2F};Cyc_Tcutil_terr(loc,err_msg2,_tag_dyneither(_tmp2E,sizeof(void*),1));});});}}
struct _tuple15{int f1;struct _dyneither_ptr(*f2)(void*);};static struct Cyc_Position_Segment*
Cyc_CfFlowInfo_combine_consume_f(struct _tuple15*env,void*place,struct Cyc_Position_Segment*
loc1,struct Cyc_Position_Segment*loc2){struct _tuple15 _tmp31;int _tmp32;struct
_dyneither_ptr(*_tmp33)(void*);struct _tuple15*_tmp30=env;_tmp31=*_tmp30;_tmp32=
_tmp31.f1;_tmp33=_tmp31.f2;if(_tmp32)Cyc_CfFlowInfo_unique_err(place,_tmp33,({
const char*_tmp34="May consume non-aliased pointer %s more than once (cf. %s)";
_tag_dyneither(_tmp34,sizeof(char),59);}),({const char*_tmp35="May consume non-aliased pointer %s more than once";
_tag_dyneither(_tmp35,sizeof(char),50);}),loc1,loc2);return loc1;}struct _tuple16{
int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_Place*place);};struct Cyc_Dict_Dict
Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int
disjoint){struct _tuple16 _tmp36=({struct _tuple16 _tmp38;_tmp38.f1=disjoint;_tmp38.f2=
Cyc_CfFlowInfo_place_err_string;_tmp38;});struct Cyc_Dict_Dict _tmp37=((struct Cyc_Dict_Dict(*)(
struct Cyc_Position_Segment*(*f)(struct _tuple16*,struct Cyc_CfFlowInfo_Place*,
struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),struct _tuple16*env,
struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((struct Cyc_Position_Segment*(*)(
struct _tuple16*env,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc1,struct Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp36,
s1,s2);return _tmp37;}struct _tuple17{int f1;struct _dyneither_ptr(*f2)(struct Cyc_CfFlowInfo_UniquePlace*
place);};struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_uplace_set(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2,int disjoint){struct _tuple17 _tmp39=({struct _tuple17
_tmp3A;_tmp3A.f1=disjoint;_tmp3A.f2=Cyc_CfFlowInfo_uplace_err_string;_tmp3A;});
return((struct Cyc_Dict_Dict(*)(struct Cyc_Position_Segment*(*f)(struct _tuple17*,
struct Cyc_CfFlowInfo_UniquePlace*,struct Cyc_Position_Segment*,struct Cyc_Position_Segment*),
struct _tuple17*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((
struct Cyc_Position_Segment*(*)(struct _tuple17*env,struct Cyc_CfFlowInfo_UniquePlace*
place,struct Cyc_Position_Segment*loc1,struct Cyc_Position_Segment*loc2))Cyc_CfFlowInfo_combine_consume_f,&
_tmp39,s1,s2);}struct _tuple18{void*f1;struct Cyc_Position_Segment*f2;};int Cyc_CfFlowInfo_place_set_subset(
struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp3B;_push_handler(& _tmp3B);{int _tmp3D=0;if(setjmp(_tmp3B.handler))
_tmp3D=1;if(!_tmp3D){{struct _RegionHandle _tmp3E=_new_region("temp");struct
_RegionHandle*temp=& _tmp3E;_push_region(temp);{struct Cyc_Iter_Iter iter=((struct
Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
temp,s1);struct _tuple18 _tmp3F=*((struct _tuple18*(*)(struct _RegionHandle*,struct
Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple18*))Cyc_Iter_next)(iter,& _tmp3F)){void*_tmp40=_tmp3F.f1;if(!((int(*)(
struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp40)){int _tmp41=0;
_npop_handler(1);return _tmp41;}}{int _tmp42=1;_npop_handler(1);return _tmp42;};};
_pop_region(temp);};_pop_handler();}else{void*_tmp3C=(void*)_exn_thrown;void*
_tmp44=_tmp3C;_LL6:;_LL7: return 1;_LL8:;_LL9:(void)_throw(_tmp44);_LL5:;}};};}int
Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(
s2,s1);}int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct
_dyneither_ptr*f){int n=0;for(0;(unsigned int)fs;fs=fs->tl){struct _dyneither_ptr*
_tmp45=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;if(Cyc_strptrcmp(_tmp45,f)== 0)
return n;n=n + 1;}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp46=
_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Core_Impossible_struct _tmp47;
_tmp47.tag=Cyc_Core_Impossible;_tmp47.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp4A;_tmp4A.tag=0;_tmp4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({
void*_tmp48[1]={& _tmp4A};Cyc_aprintf(({const char*_tmp49="get_field_index_fs failed: %s";
_tag_dyneither(_tmp49,sizeof(char),30);}),_tag_dyneither(_tmp48,sizeof(void*),1));});});
_tmp47;});_tmp46;}));}int Cyc_CfFlowInfo_get_field_index(void*t,struct
_dyneither_ptr*f){void*_tmp4B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp4D;union Cyc_Absyn_AggrInfoU _tmp4E;struct Cyc_List_List*_tmp50;_LLB: {struct
Cyc_Absyn_AggrType_struct*_tmp4C=(struct Cyc_Absyn_AggrType_struct*)_tmp4B;if(
_tmp4C->tag != 12)goto _LLD;else{_tmp4D=_tmp4C->f1;_tmp4E=_tmp4D.aggr_info;}}_LLC: {
struct Cyc_List_List*_tmp51=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(
_tmp4E))->impl))->fields;_tmp50=_tmp51;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp4F=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp4B;if(_tmp4F->tag != 13)goto _LLF;
else{_tmp50=_tmp4F->f2;}}_LLE: return Cyc_CfFlowInfo_get_field_index_fs(_tmp50,f);
_LLF:;_LL10:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp52=
_cycalloc(sizeof(*_tmp52));_tmp52[0]=({struct Cyc_Core_Impossible_struct _tmp53;
_tmp53.tag=Cyc_Core_Impossible;_tmp53.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));({void*_tmp54[1]={& _tmp56};Cyc_aprintf(({const char*_tmp55="get_field_index failed: %s";
_tag_dyneither(_tmp55,sizeof(char),27);}),_tag_dyneither(_tmp54,sizeof(void*),1));});});
_tmp53;});_tmp52;}));_LLA:;}int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)
r1 == (int)r2)return 0;{struct _tuple0 _tmp58=({struct _tuple0 _tmp57;_tmp57.f1=r1;
_tmp57.f2=r2;_tmp57;});void*_tmp59;struct Cyc_Absyn_Vardecl*_tmp5B;void*_tmp5C;
struct Cyc_Absyn_Vardecl*_tmp5E;void*_tmp5F;void*_tmp61;void*_tmp63;struct Cyc_Absyn_Exp*
_tmp65;void*_tmp66;struct Cyc_Absyn_Exp*_tmp68;void*_tmp69;void*_tmp6B;void*
_tmp6D;int _tmp6F;void*_tmp70;int _tmp72;_LL12: _tmp59=_tmp58.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5A=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp59;if(_tmp5A->tag != 0)goto _LL14;
else{_tmp5B=_tmp5A->f1;}};_tmp5C=_tmp58.f2;{struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp5D=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp5C;if(_tmp5D->tag != 0)goto _LL14;
else{_tmp5E=_tmp5D->f1;}};_LL13: return(int)_tmp5B - (int)_tmp5E;_LL14: _tmp5F=
_tmp58.f1;{struct Cyc_CfFlowInfo_VarRoot_struct*_tmp60=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp5F;if(_tmp60->tag != 0)goto _LL16;};_LL15: return - 1;_LL16: _tmp61=_tmp58.f2;{
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp62=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp61;if(_tmp62->tag != 0)goto _LL18;};_LL17: return 1;_LL18: _tmp63=_tmp58.f1;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp64=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp63;if(_tmp64->tag != 1)goto _LL1A;else{_tmp65=_tmp64->f1;}};_tmp66=_tmp58.f2;{
struct Cyc_CfFlowInfo_MallocPt_struct*_tmp67=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp66;if(_tmp67->tag != 1)goto _LL1A;else{_tmp68=_tmp67->f1;}};_LL19: return(int)
_tmp65 - (int)_tmp68;_LL1A: _tmp69=_tmp58.f1;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp6A=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp69;if(_tmp6A->tag != 1)goto
_LL1C;};_LL1B: return - 1;_LL1C: _tmp6B=_tmp58.f2;{struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp6C=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp6B;if(_tmp6C->tag != 1)goto
_LL1E;};_LL1D: return 1;_LL1E: _tmp6D=_tmp58.f1;{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp6E=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp6D;if(_tmp6E->tag != 2)goto
_LL11;else{_tmp6F=_tmp6E->f1;}};_tmp70=_tmp58.f2;{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp71=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp70;if(_tmp71->tag != 2)goto
_LL11;else{_tmp72=_tmp71->f1;}};_LL1F: return _tmp6F - _tmp72;_LL11:;};}static int
Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2){
return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))
Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)
p2)return 0;{void*_tmp74;struct Cyc_List_List*_tmp75;struct Cyc_CfFlowInfo_Place
_tmp73=*p1;_tmp74=_tmp73.root;_tmp75=_tmp73.fields;{void*_tmp77;struct Cyc_List_List*
_tmp78;struct Cyc_CfFlowInfo_Place _tmp76=*p2;_tmp77=_tmp76.root;_tmp78=_tmp76.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp74,_tmp77);if(i != 0)return i;return Cyc_CfFlowInfo_intlist_cmp(
_tmp75,_tmp78);};};};}int Cyc_CfFlowInfo_uplace_cmp(struct Cyc_CfFlowInfo_UniquePlace*
p1,struct Cyc_CfFlowInfo_UniquePlace*p2){if((int)p1 == (int)p2)return 0;{struct Cyc_CfFlowInfo_Place
_tmp7A;struct Cyc_List_List*_tmp7B;struct Cyc_CfFlowInfo_UniquePlace _tmp79=*p1;
_tmp7A=_tmp79.place;_tmp7B=_tmp79.path;{struct Cyc_CfFlowInfo_Place _tmp7D;struct
Cyc_List_List*_tmp7E;struct Cyc_CfFlowInfo_UniquePlace _tmp7C=*p2;_tmp7D=_tmp7C.place;
_tmp7E=_tmp7C.path;{int i=Cyc_CfFlowInfo_place_cmp(& _tmp7A,& _tmp7D);if(i != 0)
return i;return((int(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_intlist_cmp,
_tmp7B,_tmp7E);};};};}static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*sl=0;void*_tmp80;struct Cyc_List_List*
_tmp81;struct Cyc_CfFlowInfo_Place _tmp7F=*p;_tmp80=_tmp7F.root;_tmp81=_tmp7F.fields;{
void*_tmp82=_tmp80;struct Cyc_Absyn_Vardecl*_tmp84;struct Cyc_Absyn_Exp*_tmp86;int
_tmp88;_LL21: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp83=(struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp82;if(_tmp83->tag != 0)goto _LL23;else{_tmp84=_tmp83->f1;}}_LL22: sl=({struct
Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->hd=({struct
_dyneither_ptr*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_String_pa_struct
_tmp8D;_tmp8D.tag=0;_tmp8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp84->name).f2);({void*_tmp8B[1]={& _tmp8D};Cyc_aprintf(({const char*_tmp8C="%s";
_tag_dyneither(_tmp8C,sizeof(char),3);}),_tag_dyneither(_tmp8B,sizeof(void*),1));});});
_tmp8A;});_tmp89->tl=sl;_tmp89;});goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp85=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp82;if(_tmp85->tag != 1)goto
_LL25;else{_tmp86=_tmp85->f1;}}_LL24: sl=({struct Cyc_List_List*_tmp8E=_cycalloc(
sizeof(*_tmp8E));_tmp8E->hd=({struct _dyneither_ptr*_tmp8F=_cycalloc(sizeof(*
_tmp8F));_tmp8F[0]=({struct Cyc_Int_pa_struct _tmp92;_tmp92.tag=1;_tmp92.f1=(
unsigned long)((int)_tmp86);({void*_tmp90[1]={& _tmp92};Cyc_aprintf(({const char*
_tmp91="mpt%d";_tag_dyneither(_tmp91,sizeof(char),6);}),_tag_dyneither(_tmp90,
sizeof(void*),1));});});_tmp8F;});_tmp8E->tl=sl;_tmp8E;});goto _LL20;_LL25: {
struct Cyc_CfFlowInfo_InitParam_struct*_tmp87=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp82;if(_tmp87->tag != 2)goto _LL20;else{_tmp88=_tmp87->f1;}}_LL26: sl=({struct
Cyc_List_List*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93->hd=({struct
_dyneither_ptr*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94[0]=({struct Cyc_Int_pa_struct
_tmp97;_tmp97.tag=1;_tmp97.f1=(unsigned long)_tmp88;({void*_tmp95[1]={& _tmp97};
Cyc_aprintf(({const char*_tmp96="param%d";_tag_dyneither(_tmp96,sizeof(char),8);}),
_tag_dyneither(_tmp95,sizeof(void*),1));});});_tmp94;});_tmp93->tl=sl;_tmp93;});
goto _LL20;_LL20:;}for(0;_tmp81 != 0;_tmp81=_tmp81->tl){sl=({struct Cyc_List_List*
_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->hd=({struct _dyneither_ptr*_tmp99=
_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Int_pa_struct _tmp9C;_tmp9C.tag=
1;_tmp9C.f1=(unsigned long)((int)_tmp81->hd);({void*_tmp9A[1]={& _tmp9C};Cyc_aprintf(({
const char*_tmp9B="%d";_tag_dyneither(_tmp9B,sizeof(char),3);}),_tag_dyneither(
_tmp9A,sizeof(void*),1));});});_tmp99;});_tmp98->tl=sl;_tmp98;});}{struct
_dyneither_ptr*_tmp9D=({struct _dyneither_ptr*_tmpA2=_cycalloc(sizeof(*_tmpA2));
_tmpA2[0]=({struct Cyc_String_pa_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct
_dyneither_ptr)({const char*_tmpA6="";_tag_dyneither(_tmpA6,sizeof(char),1);});({
void*_tmpA3[1]={& _tmpA5};Cyc_aprintf(({const char*_tmpA4="%s";_tag_dyneither(
_tmpA4,sizeof(char),3);}),_tag_dyneither(_tmpA3,sizeof(void*),1));});});_tmpA2;});
for(0;sl != 0;sl=sl->tl){*_tmp9D=({struct Cyc_String_pa_struct _tmpA1;_tmpA1.tag=0;
_tmpA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9D);({struct Cyc_String_pa_struct
_tmpA0;_tmpA0.tag=0;_tmpA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)sl->hd));({void*_tmp9E[2]={& _tmpA0,& _tmpA1};Cyc_aprintf(({
const char*_tmp9F="%s.%s";_tag_dyneither(_tmp9F,sizeof(char),6);}),_tag_dyneither(
_tmp9E,sizeof(void*),2));});});});}return _tmp9D;};}static void*Cyc_CfFlowInfo_i_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,void*t,void*leafval);struct
_dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_List_List*fs,void*leafval){unsigned int sz=(unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(fs);struct _dyneither_ptr d=({unsigned int
_tmpAB=sz;void**_tmpAC=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),
_tmpAB));struct _dyneither_ptr _tmpAE=_tag_dyneither(_tmpAC,sizeof(void*),_tmpAB);{
unsigned int _tmpAD=_tmpAB;unsigned int i;for(i=0;i < _tmpAD;i ++){_tmpAC[i]=fenv->unknown_all;}}
_tmpAE;});{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){struct Cyc_Absyn_Aggrfield _tmpA8;
struct _dyneither_ptr*_tmpA9;void*_tmpAA;struct Cyc_Absyn_Aggrfield*_tmpA7=(struct
Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;_tmpA8=*_tmpA7;
_tmpA9=_tmpA8.name;_tmpAA=_tmpA8.type;if(_get_dyneither_size(*_tmpA9,sizeof(char))
!= 1)*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(
fenv,0,_tmpAA,leafval);}}return d;}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;
};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
int allow_zeroterm,void*t,void*leafval){if(Cyc_Absyn_is_nontagged_union_type(t))
return fenv->unknown_all;{void*_tmpAF=Cyc_Tcutil_compress(t);struct Cyc_Absyn_DatatypeFieldInfo
_tmpB1;union Cyc_Absyn_DatatypeFieldInfoU _tmpB2;struct _tuple2 _tmpB3;struct Cyc_Absyn_Datatypefield*
_tmpB4;struct Cyc_List_List*_tmpB6;struct Cyc_Absyn_AggrInfo _tmpB8;union Cyc_Absyn_AggrInfoU
_tmpB9;enum Cyc_Absyn_AggrKind _tmpBB;struct Cyc_List_List*_tmpBC;struct Cyc_Absyn_ArrayInfo
_tmpBE;void*_tmpBF;union Cyc_Absyn_Constraint*_tmpC0;void*_tmpC2;struct Cyc_Absyn_PtrInfo
_tmpC4;struct Cyc_Absyn_PtrAtts _tmpC5;union Cyc_Absyn_Constraint*_tmpC6;_LL28: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmpB0=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmpAF;if(_tmpB0->tag != 4)goto _LL2A;else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB1.field_info;
if((_tmpB2.KnownDatatypefield).tag != 2)goto _LL2A;_tmpB3=(struct _tuple2)(_tmpB2.KnownDatatypefield).val;
_tmpB4=_tmpB3.f2;}}_LL29: if(_tmpB4->typs == 0)return leafval;_tmpB6=_tmpB4->typs;
goto _LL2B;_LL2A: {struct Cyc_Absyn_TupleType_struct*_tmpB5=(struct Cyc_Absyn_TupleType_struct*)
_tmpAF;if(_tmpB5->tag != 11)goto _LL2C;else{_tmpB6=_tmpB5->f1;}}_LL2B: {
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpB6);struct _dyneither_ptr d=({unsigned int _tmpC9=sz;void**_tmpCA=(void**)
_region_malloc(fenv->r,_check_times(sizeof(void*),_tmpC9));struct _dyneither_ptr
_tmpCC=_tag_dyneither(_tmpCA,sizeof(void*),_tmpC9);{unsigned int _tmpCB=_tmpC9;
unsigned int i;for(i=0;i < _tmpCB;i ++){_tmpCA[i]=fenv->unknown_all;}}_tmpCC;});{
int i=0;for(0;i < sz;++ i){*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=
Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,(*((struct _tuple19*)((struct Cyc_List_List*)
_check_null(_tmpB6))->hd)).f2,leafval);_tmpB6=_tmpB6->tl;}}return(void*)({struct
Cyc_CfFlowInfo_Aggregate_struct*_tmpC7=_region_malloc(fenv->r,sizeof(*_tmpC7));
_tmpC7[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpC8;_tmpC8.tag=7;_tmpC8.f1=0;
_tmpC8.f2=d;_tmpC8;});_tmpC7;});}_LL2C: {struct Cyc_Absyn_AggrType_struct*_tmpB7=(
struct Cyc_Absyn_AggrType_struct*)_tmpAF;if(_tmpB7->tag != 12)goto _LL2E;else{
_tmpB8=_tmpB7->f1;_tmpB9=_tmpB8.aggr_info;}}_LL2D: {struct Cyc_Absyn_Aggrdecl*
_tmpCD=Cyc_Absyn_get_known_aggrdecl(_tmpB9);if(_tmpCD->impl == 0)goto _LL27;_tmpBB=
_tmpCD->kind;_tmpBC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCD->impl))->fields;
goto _LL2F;}_LL2E: {struct Cyc_Absyn_AnonAggrType_struct*_tmpBA=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpAF;if(_tmpBA->tag != 13)goto _LL30;else{_tmpBB=_tmpBA->f1;_tmpBC=_tmpBA->f2;}}
_LL2F: return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmpCE=_region_malloc(
fenv->r,sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmpCF;
_tmpCF.tag=7;_tmpCF.f1=_tmpBB == Cyc_Absyn_UnionA;_tmpCF.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(
fenv,_tmpBC,leafval);_tmpCF;});_tmpCE;});_LL30:{struct Cyc_Absyn_ArrayType_struct*
_tmpBD=(struct Cyc_Absyn_ArrayType_struct*)_tmpAF;if(_tmpBD->tag != 9)goto _LL32;
else{_tmpBE=_tmpBD->f1;_tmpBF=_tmpBE.elt_type;_tmpC0=_tmpBE.zero_term;}}if(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC0))goto
_LL32;_LL31: return allow_zeroterm  && Cyc_Tcutil_bits_only(_tmpBF)?fenv->unknown_all:
leafval;_LL32: {struct Cyc_Absyn_TagType_struct*_tmpC1=(struct Cyc_Absyn_TagType_struct*)
_tmpAF;if(_tmpC1->tag != 20)goto _LL34;else{_tmpC2=(void*)_tmpC1->f1;}}_LL33: {
void*_tmpD0=leafval;enum Cyc_CfFlowInfo_InitLevel _tmpD2;enum Cyc_CfFlowInfo_InitLevel
_tmpD4;_LL39: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpD1=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpD0;if(_tmpD1->tag != 3)goto _LL3B;else{_tmpD2=_tmpD1->f1;if(_tmpD2 != Cyc_CfFlowInfo_AllIL)
goto _LL3B;}}_LL3A: goto _LL3C;_LL3B: {struct Cyc_CfFlowInfo_Esc_struct*_tmpD3=(
struct Cyc_CfFlowInfo_Esc_struct*)_tmpD0;if(_tmpD3->tag != 4)goto _LL3D;else{_tmpD4=
_tmpD3->f1;if(_tmpD4 != Cyc_CfFlowInfo_AllIL)goto _LL3D;}}_LL3C: goto _LL3E;_LL3D: {
struct Cyc_CfFlowInfo_Zero_struct*_tmpD5=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmpD0;if(_tmpD5->tag != 0)goto _LL3F;}_LL3E: goto _LL40;_LL3F: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmpD6=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmpD0;if(_tmpD6->tag != 1)goto
_LL41;}_LL40: return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*_tmpD7=
_region_malloc(fenv->r,sizeof(*_tmpD7));_tmpD7[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmpD8;_tmpD8.tag=6;_tmpD8.f1=({struct Cyc_CfFlowInfo_TagCmp*_tmpD9[1];_tmpD9[0]=({
struct Cyc_CfFlowInfo_TagCmp*_tmpDA=_region_malloc(fenv->r,sizeof(*_tmpDA));
_tmpDA->cmp=Cyc_Absyn_Eq;_tmpDA->bd=_tmpC2;_tmpDA;});((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(fenv->r,
_tag_dyneither(_tmpD9,sizeof(struct Cyc_CfFlowInfo_TagCmp*),1));});_tmpD8;});
_tmpD7;});_LL41:;_LL42: return leafval;_LL38:;}_LL34:{struct Cyc_Absyn_PointerType_struct*
_tmpC3=(struct Cyc_Absyn_PointerType_struct*)_tmpAF;if(_tmpC3->tag != 5)goto _LL36;
else{_tmpC4=_tmpC3->f1;_tmpC5=_tmpC4.ptr_atts;_tmpC6=_tmpC5.nullable;}}if(!(!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC6)))goto
_LL36;_LL35:{void*_tmpDB=leafval;enum Cyc_CfFlowInfo_InitLevel _tmpDD;enum Cyc_CfFlowInfo_InitLevel
_tmpDF;_LL44: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmpDC=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpDB;if(_tmpDC->tag != 3)goto _LL46;else{_tmpDD=_tmpDC->f1;if(_tmpDD != Cyc_CfFlowInfo_ThisIL)
goto _LL46;}}_LL45: return fenv->notzerothis;_LL46: {struct Cyc_CfFlowInfo_UnknownR_struct*
_tmpDE=(struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpDB;if(_tmpDE->tag != 3)goto
_LL48;else{_tmpDF=_tmpDE->f1;if(_tmpDF != Cyc_CfFlowInfo_AllIL)goto _LL48;}}_LL47:
return fenv->notzeroall;_LL48:;_LL49: goto _LL43;_LL43:;}goto _LL27;_LL36:;_LL37:
goto _LL27;_LL27:;}return Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}void*
Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,void*
leafval){return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,t,leafval);}struct _tuple20{
struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*f2;};static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){struct _RegionHandle
_tmpE0=_new_region("r");struct _RegionHandle*r=& _tmpE0;_push_region(r);{struct
_tuple20 _tmpE1=({struct _tuple20 _tmpEB;_tmpEB.f1=place;_tmpEB.f2=0;_tmpEB;});
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct
Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);while(((int(*)(struct Cyc_Iter_Iter,
struct _tuple20*))Cyc_Iter_next)(iter,& _tmpE1)){struct Cyc_CfFlowInfo_Place*_tmpE2=
_tmpE1.f1;void*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_CfFlowInfo_Place
_tmpE3=*place;_tmpE4=_tmpE3.root;_tmpE5=_tmpE3.fields;{void*_tmpE7;struct Cyc_List_List*
_tmpE8;struct Cyc_CfFlowInfo_Place _tmpE6=*_tmpE2;_tmpE7=_tmpE6.root;_tmpE8=_tmpE6.fields;
if(Cyc_CfFlowInfo_root_cmp(_tmpE4,_tmpE7)!= 0)continue;for(0;_tmpE5 != 0  && 
_tmpE8 != 0;(_tmpE5=_tmpE5->tl,_tmpE8=_tmpE8->tl)){if((int)_tmpE5->hd != (int)
_tmpE8->hd)break;}if(_tmpE5 == 0){int _tmpE9=1;_npop_handler(0);return _tmpE9;}};}{
int _tmpEA=0;_npop_handler(0);return _tmpEA;};};_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{
struct _RegionHandle*rgn;struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(
struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,
place))pile->places=({struct Cyc_List_List*_tmpEC=_region_malloc(pile->rgn,
sizeof(*_tmpEC));_tmpEC->hd=place;_tmpEC->tl=pile->places;_tmpEC;});}static void
Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_tmpED=
r;struct Cyc_CfFlowInfo_Place*_tmpEF;int _tmpF1;struct _dyneither_ptr _tmpF2;_LL4B: {
struct Cyc_CfFlowInfo_AddressOf_struct*_tmpEE=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpED;if(_tmpEE->tag != 5)goto _LL4D;else{_tmpEF=_tmpEE->f1;}}_LL4C: Cyc_CfFlowInfo_add_place(
pile,_tmpEF);return;_LL4D: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmpF0=(struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmpED;if(_tmpF0->tag != 7)goto _LL4F;else{_tmpF1=
_tmpF0->f1;_tmpF2=_tmpF0->f2;}}_LL4E:{int i=0;for(0;i < _get_dyneither_size(_tmpF2,
sizeof(void*));++ i){Cyc_CfFlowInfo_add_places(pile,((void**)_tmpF2.curr)[i]);}}
return;_LL4F:;_LL50: return;_LL4A:;}static void*Cyc_CfFlowInfo_insert_place_inner(
struct _RegionHandle*r,void*new_val,void*old_val){void*_tmpF3=old_val;int _tmpF5;
struct _dyneither_ptr _tmpF6;_LL52: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmpF4=(
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpF3;if(_tmpF4->tag != 7)goto _LL54;else{
_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;}}_LL53: {struct _dyneither_ptr d2=({
unsigned int _tmpF9=_get_dyneither_size(_tmpF6,sizeof(void*));void**_tmpFA=(void**)
_region_malloc(r,_check_times(sizeof(void*),_tmpF9));struct _dyneither_ptr _tmpFC=
_tag_dyneither(_tmpFA,sizeof(void*),_tmpF9);{unsigned int _tmpFB=_tmpF9;
unsigned int i;for(i=0;i < _tmpFB;i ++){_tmpFA[i]=Cyc_CfFlowInfo_insert_place_inner(
r,new_val,((void**)_tmpF6.curr)[(int)i]);}}_tmpFC;});return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmpF7=_region_malloc(r,sizeof(*_tmpF7));_tmpF7[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmpF8;_tmpF8.tag=7;_tmpF8.f1=_tmpF5;_tmpF8.f2=d2;_tmpF8;});_tmpF7;});}_LL54:;
_LL55: return new_val;_LL51:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(
struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){void*_tmpFD=*((void**)
_check_dyneither_subscript(d,sizeof(void*),n));if(_tmpFD == rval)return d;{struct
_dyneither_ptr res=({unsigned int _tmpFE=_get_dyneither_size(d,sizeof(void*));void**
_tmpFF=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmpFE));struct
_dyneither_ptr _tmp101=_tag_dyneither(_tmpFF,sizeof(void*),_tmpFE);{unsigned int
_tmp100=_tmpFE;unsigned int i;for(i=0;i < _tmp100;i ++){_tmpFF[i]=((void**)d.curr)[(
int)i];}}_tmp101;});*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=
rval;return res;};}struct _tuple21{struct Cyc_List_List*f1;void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(
struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){if(fs == 
0)return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{struct _tuple21
_tmp103=({struct _tuple21 _tmp102;_tmp102.f1=fs;_tmp102.f2=old_val;_tmp102;});
struct Cyc_List_List*_tmp104;struct Cyc_List_List _tmp105;int _tmp106;struct Cyc_List_List*
_tmp107;void*_tmp108;int _tmp10A;struct _dyneither_ptr _tmp10B;_LL57: _tmp104=
_tmp103.f1;if(_tmp104 == 0)goto _LL59;_tmp105=*_tmp104;_tmp106=(int)_tmp105.hd;
_tmp107=_tmp105.tl;_tmp108=_tmp103.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp109=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp108;if(_tmp109->tag != 7)goto
_LL59;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}};_LL58: {void*_tmp10C=Cyc_CfFlowInfo_insert_place_outer(
r,_tmp107,*((void**)_check_dyneither_subscript(_tmp10B,sizeof(void*),_tmp106)),
new_val);struct _dyneither_ptr _tmp10D=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp10B,
_tmp106,_tmp10C);if(_tmp10D.curr == _tmp10B.curr)return old_val;else{return(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp10E=_region_malloc(r,sizeof(*_tmp10E));
_tmp10E[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp10F;_tmp10F.tag=7;_tmp10F.f1=
_tmp10A;_tmp10F.f2=_tmp10D;_tmp10F;});_tmp10E;});}}_LL59:;_LL5A:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp110=_cycalloc(sizeof(*_tmp110));
_tmp110[0]=({struct Cyc_Core_Impossible_struct _tmp111;_tmp111.tag=Cyc_Core_Impossible;
_tmp111.f1=({const char*_tmp112="bad insert place";_tag_dyneither(_tmp112,sizeof(
char),17);});_tmp111;});_tmp110;}));_LL56:;};}static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*
all_changed,struct Cyc_Dict_Dict d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*
_tmp113=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;((int(*)(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp113,0);{void*oldval;void*
newval;{struct _handler_cons _tmp114;_push_handler(& _tmp114);{int _tmp116=0;if(
setjmp(_tmp114.handler))_tmp116=1;if(!_tmp116){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp113);;_pop_handler();}else{void*_tmp115=(void*)_exn_thrown;void*_tmp118=
_tmp115;_LL5C: {struct Cyc_Dict_Absent_struct*_tmp119=(struct Cyc_Dict_Absent_struct*)
_tmp118;if(_tmp119->tag != Cyc_Dict_Absent)goto _LL5E;}_LL5D: continue;_LL5E:;_LL5F:(
void)_throw(_tmp118);_LL5B:;}};}switch(Cyc_CfFlowInfo_initlevel(fenv,d,oldval)){
case Cyc_CfFlowInfo_AllIL: _LL60: newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL:
_LL61: newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL62: newval=fenv->esc_none;
break;}Cyc_CfFlowInfo_add_places(pile,oldval);{void*_tmp11B;struct Cyc_List_List*
_tmp11C;struct Cyc_CfFlowInfo_Place _tmp11A=*_tmp113;_tmp11B=_tmp11A.root;_tmp11C=
_tmp11A.fields;d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))
Cyc_Dict_insert)(d,_tmp11B,Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp11C,((
void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp11B),newval));};};}
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict
d;struct Cyc_List_List*seen;};static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(
void*r){void*_tmp11D=r;enum Cyc_CfFlowInfo_InitLevel _tmp11F;enum Cyc_CfFlowInfo_InitLevel
_tmp121;_LL65: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp11E=(struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp11D;if(_tmp11E->tag != 3)goto _LL67;else{_tmp11F=_tmp11E->f1;}}_LL66: return
_tmp11F;_LL67: {struct Cyc_CfFlowInfo_Esc_struct*_tmp120=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp11D;if(_tmp120->tag != 4)goto _LL69;else{_tmp121=_tmp120->f1;}}_LL68: return
_tmp121;_LL69: {struct Cyc_CfFlowInfo_Zero_struct*_tmp122=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp11D;if(_tmp122->tag != 0)goto _LL6B;}_LL6A: goto _LL6C;_LL6B: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp123=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp11D;if(_tmp123->tag != 1)
goto _LL6D;}_LL6C: return Cyc_CfFlowInfo_AllIL;_LL6D: {struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp124=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp11D;if(_tmp124->tag != 2)
goto _LL6F;}_LL6E: return Cyc_CfFlowInfo_ThisIL;_LL6F: {struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp125=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp11D;if(_tmp125->tag != 6)goto
_LL71;}_LL70: return Cyc_CfFlowInfo_AllIL;_LL71:;_LL72:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126[0]=({
struct Cyc_Core_Impossible_struct _tmp127;_tmp127.tag=Cyc_Core_Impossible;_tmp127.f1=({
const char*_tmp128="initlevel_approx";_tag_dyneither(_tmp128,sizeof(char),17);});
_tmp127;});_tmp126;}));_LL64:;}static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(
struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
enum Cyc_CfFlowInfo_InitLevel this_ans;if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;{
void*_tmp129=r;int _tmp12B;struct _dyneither_ptr _tmp12C;int _tmp12E;struct
_dyneither_ptr _tmp12F;struct Cyc_CfFlowInfo_Place*_tmp131;_LL74: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp12A=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp129;if(_tmp12A->tag != 7)goto
_LL76;else{_tmp12B=_tmp12A->f1;if(_tmp12B != 1)goto _LL76;_tmp12C=_tmp12A->f2;}}
_LL75: {unsigned int _tmp132=_get_dyneither_size(_tmp12C,sizeof(void*));this_ans=
Cyc_CfFlowInfo_NoneIL;{int i=0;for(0;i < _tmp132;++ i){if(Cyc_CfFlowInfo_initlevel_rec(
env,*((void**)_check_dyneither_subscript(_tmp12C,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)
== Cyc_CfFlowInfo_AllIL){this_ans=Cyc_CfFlowInfo_AllIL;break;}}}goto _LL73;}_LL76: {
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp12D=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp129;if(_tmp12D->tag != 7)goto _LL78;else{_tmp12E=_tmp12D->f1;if(_tmp12E != 0)
goto _LL78;_tmp12F=_tmp12D->f2;}}_LL77: {unsigned int _tmp133=_get_dyneither_size(
_tmp12F,sizeof(void*));this_ans=Cyc_CfFlowInfo_AllIL;{int i=0;for(0;i < _tmp133;++
i){this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)
_check_dyneither_subscript(_tmp12F,sizeof(void*),i)),this_ans);}}goto _LL73;}
_LL78: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp130=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp129;if(_tmp130->tag != 5)goto _LL7A;else{_tmp131=_tmp130->f1;}}_LL79: if(((int(*)(
int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,
_tmp131))this_ans=Cyc_CfFlowInfo_AllIL;else{env->seen=({struct Cyc_List_List*
_tmp134=_region_malloc(env->e,sizeof(*_tmp134));_tmp134->hd=_tmp131;_tmp134->tl=
env->seen;_tmp134;});this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(
env->d,_tmp131),Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == Cyc_CfFlowInfo_NoneIL)this_ans=Cyc_CfFlowInfo_ThisIL;}
goto _LL73;_LL7A:;_LL7B: this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL73:;}if(
this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;if(this_ans == Cyc_CfFlowInfo_ThisIL
 || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;return Cyc_CfFlowInfo_AllIL;}
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*
env,struct Cyc_Dict_Dict d,void*r){struct _RegionHandle*_tmp135=env->r;struct Cyc_CfFlowInfo_InitlevelEnv
_tmp136=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp137;_tmp137.e=_tmp135;_tmp137.d=d;
_tmp137.seen=0;_tmp137;});return Cyc_CfFlowInfo_initlevel_rec(& _tmp136,r,Cyc_CfFlowInfo_AllIL);}
struct _tuple22{void*f1;int f2;};void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place){void*_tmp139;struct Cyc_List_List*_tmp13A;
struct Cyc_CfFlowInfo_Place _tmp138=*place;_tmp139=_tmp138.root;_tmp13A=_tmp138.fields;{
void*_tmp13B=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp139);
for(0;_tmp13A != 0;_tmp13A=_tmp13A->tl){struct _tuple22 _tmp13D=({struct _tuple22
_tmp13C;_tmp13C.f1=_tmp13B;_tmp13C.f2=(int)_tmp13A->hd;_tmp13C;});void*_tmp13E;
int _tmp140;struct _dyneither_ptr _tmp141;int _tmp142;_LL7D: _tmp13E=_tmp13D.f1;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp13F=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp13E;if(_tmp13F->tag != 7)goto _LL7F;else{_tmp140=_tmp13F->f1;_tmp141=_tmp13F->f2;}};
_tmp142=_tmp13D.f2;_LL7E: _tmp13B=*((void**)_check_dyneither_subscript(_tmp141,
sizeof(void*),_tmp142));goto _LL7C;_LL7F:;_LL80:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143[0]=({struct Cyc_Core_Impossible_struct
_tmp144;_tmp144.tag=Cyc_Core_Impossible;_tmp144.f1=({const char*_tmp145="bad lookup_place";
_tag_dyneither(_tmp145,sizeof(char),17);});_tmp144;});_tmp143;}));_LL7C:;}return
_tmp13B;};}static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_tmp146=
rval;int _tmp149;struct _dyneither_ptr _tmp14A;_LL82: {struct Cyc_CfFlowInfo_Esc_struct*
_tmp147=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp146;if(_tmp147->tag != 4)goto _LL84;}
_LL83: return 0;_LL84: {struct Cyc_CfFlowInfo_Aggregate_struct*_tmp148=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp146;if(_tmp148->tag != 7)goto _LL86;else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}}
_LL85: {unsigned int sz=_get_dyneither_size(_tmp14A,sizeof(void*));{int i=0;for(0;
i < sz;++ i){if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp14A.curr)[i]))
return 0;}}return 1;}_LL86:;_LL87: return 1;_LL81:;}int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){return Cyc_CfFlowInfo_is_rval_unescaped(
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,void*r){struct _RegionHandle*_tmp14B=fenv->r;struct Cyc_CfFlowInfo_EscPile*
pile=({struct Cyc_CfFlowInfo_EscPile*_tmp14C=_region_malloc(_tmp14B,sizeof(*
_tmp14C));_tmp14C->rgn=_tmp14B;_tmp14C->places=0;_tmp14C;});Cyc_CfFlowInfo_add_places(
pile,r);return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict
d;struct Cyc_Position_Segment*loc;};static void*Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){struct _tuple0 _tmp14E=({
struct _tuple0 _tmp14D;_tmp14D.f1=oldval;_tmp14D.f2=newval;_tmp14D;});void*_tmp14F;
void*_tmp151;struct Cyc_CfFlowInfo_Place*_tmp153;void*_tmp154;void*_tmp156;int
_tmp158;struct _dyneither_ptr _tmp159;void*_tmp15A;int _tmp15C;struct _dyneither_ptr
_tmp15D;void*_tmp15E;enum Cyc_CfFlowInfo_InitLevel _tmp160;_LL89: _tmp14F=_tmp14E.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp150=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp14F;if(_tmp150->tag != 4)goto _LL8B;};_tmp151=_tmp14E.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp152=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp151;if(_tmp152->tag != 5)goto
_LL8B;else{_tmp153=_tmp152->f1;}};_LL8A: Cyc_CfFlowInfo_add_place(env->pile,
_tmp153);goto _LL8C;_LL8B: _tmp154=_tmp14E.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp155=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp154;if(_tmp155->tag != 4)goto _LL8D;};
_LL8C: if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL)({
void*_tmp161=0;Cyc_Tcutil_terr(env->loc,({const char*_tmp162="assignment puts possibly-uninitialized data in an escaped location";
_tag_dyneither(_tmp162,sizeof(char),67);}),_tag_dyneither(_tmp161,sizeof(void*),
0));});return(env->fenv)->esc_all;_LL8D: _tmp156=_tmp14E.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp157=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp156;if(_tmp157->tag != 7)goto
_LL8F;else{_tmp158=_tmp157->f1;_tmp159=_tmp157->f2;}};_tmp15A=_tmp14E.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp15B=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp15A;if(_tmp15B->tag != 7)goto _LL8F;else{_tmp15C=_tmp15B->f1;_tmp15D=_tmp15B->f2;}};
_LL8E: {struct _dyneither_ptr new_d=({unsigned int _tmp165=_get_dyneither_size(
_tmp159,sizeof(void*));void**_tmp166=(void**)_region_malloc((env->fenv)->r,
_check_times(sizeof(void*),_tmp165));struct _dyneither_ptr _tmp168=_tag_dyneither(
_tmp166,sizeof(void*),_tmp165);{unsigned int _tmp167=_tmp165;unsigned int i;for(i=
0;i < _tmp167;i ++){_tmp166[i]=Cyc_CfFlowInfo_assign_place_inner(env,((void**)
_tmp159.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp15D,sizeof(void*),(
int)i)));}}_tmp168;});int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp159,
sizeof(void*));++ i){if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),
i))!= ((void**)_tmp159.curr)[i]){change=1;break;}}}if(!change)return oldval;
change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp159,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp15D,sizeof(void*),i))){change=1;break;}}}if(!
change)return newval;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp163=
_region_malloc((env->fenv)->r,sizeof(*_tmp163));_tmp163[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp164;_tmp164.tag=7;_tmp164.f1=_tmp158;_tmp164.f2=new_d;_tmp164;});_tmp163;});}
_LL8F: _tmp15E=_tmp14E.f2;{struct Cyc_CfFlowInfo_Esc_struct*_tmp15F=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp15E;if(_tmp15F->tag != 4)goto _LL91;else{_tmp160=_tmp15F->f1;}};_LL90: switch(
_tmp160){case Cyc_CfFlowInfo_NoneIL: _LL93: return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL:
_LL94: return(env->fenv)->unknown_this;case Cyc_CfFlowInfo_AllIL: _LL95: return(env->fenv)->unknown_all;}
_LL91:;_LL92: return newval;_LL88:;}static void*Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,void*oldval,void*
newval){if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple21 _tmp16A=({struct _tuple21 _tmp169;_tmp169.f1=fs;_tmp169.f2=oldval;
_tmp169;});struct Cyc_List_List*_tmp16B;struct Cyc_List_List _tmp16C;int _tmp16D;
struct Cyc_List_List*_tmp16E;void*_tmp16F;int _tmp171;struct _dyneither_ptr _tmp172;
_LL98: _tmp16B=_tmp16A.f1;if(_tmp16B == 0)goto _LL9A;_tmp16C=*_tmp16B;_tmp16D=(int)
_tmp16C.hd;_tmp16E=_tmp16C.tl;_tmp16F=_tmp16A.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp170=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp16F;if(_tmp170->tag != 7)goto
_LL9A;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;}};_LL99: {void*_tmp173=Cyc_CfFlowInfo_assign_place_outer(
env,_tmp16E,*((void**)_check_dyneither_subscript(_tmp172,sizeof(void*),_tmp16D)),
newval);struct _dyneither_ptr _tmp174=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,
_tmp172,_tmp16D,_tmp173);if(_tmp174.curr == _tmp172.curr)return oldval;else{return(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp175=_region_malloc((env->fenv)->r,
sizeof(*_tmp175));_tmp175[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp176;
_tmp176.tag=7;_tmp176.f1=_tmp171;_tmp176.f2=_tmp174;_tmp176;});_tmp175;});}}
_LL9A:;_LL9B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp177=
_cycalloc(sizeof(*_tmp177));_tmp177[0]=({struct Cyc_Core_Impossible_struct _tmp178;
_tmp178.tag=Cyc_Core_Impossible;_tmp178.f1=({const char*_tmp179="bad assign place";
_tag_dyneither(_tmp179,sizeof(char),17);});_tmp178;});_tmp177;}));_LL97:;};}
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)(
all_changed,place,0);{struct _RegionHandle*_tmp17A=fenv->r;struct Cyc_CfFlowInfo_Place
_tmp17C;void*_tmp17D;struct Cyc_List_List*_tmp17E;struct Cyc_CfFlowInfo_Place*
_tmp17B=place;_tmp17C=*_tmp17B;_tmp17D=_tmp17C.root;_tmp17E=_tmp17C.fields;{
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp17F;
_tmp17F.fenv=fenv;_tmp17F.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp180=
_region_malloc(_tmp17A,sizeof(*_tmp180));_tmp180->rgn=_tmp17A;_tmp180->places=0;
_tmp180;});_tmp17F.d=d;_tmp17F.loc=loc;_tmp17F;});void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp17E,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp17D),
r);return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp17D,newval));};};}
struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict
chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void*ignore,void*r1,void*r2);static struct Cyc_List_List*Cyc_CfFlowInfo_join_tag_cmps(
struct _RegionHandle*r,struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2)
return l1;{struct Cyc_List_List*_tmp181=0;for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp183;enum Cyc_Absyn_Primop _tmp184;void*_tmp185;struct Cyc_CfFlowInfo_TagCmp*
_tmp182=(struct Cyc_CfFlowInfo_TagCmp*)l2->hd;_tmp183=*_tmp182;_tmp184=_tmp183.cmp;
_tmp185=_tmp183.bd;{int found=0;enum Cyc_Absyn_Primop joined_cmp=Cyc_Absyn_Lte;{
struct Cyc_List_List*_tmp186=l1;for(0;_tmp186 != 0;_tmp186=_tmp186->tl){struct Cyc_CfFlowInfo_TagCmp
_tmp188;enum Cyc_Absyn_Primop _tmp189;void*_tmp18A;struct Cyc_CfFlowInfo_TagCmp*
_tmp187=(struct Cyc_CfFlowInfo_TagCmp*)_tmp186->hd;_tmp188=*_tmp187;_tmp189=
_tmp188.cmp;_tmp18A=_tmp188.bd;if(Cyc_Tcutil_typecmp(_tmp185,_tmp18A)== 0){found=
1;if(_tmp189 == _tmp184){joined_cmp=_tmp189;break;}}}}if(found)_tmp181=({struct
Cyc_List_List*_tmp18B=_region_malloc(r,sizeof(*_tmp18B));_tmp18B->hd=({struct Cyc_CfFlowInfo_TagCmp*
_tmp18C=_region_malloc(r,sizeof(*_tmp18C));_tmp18C->cmp=joined_cmp;_tmp18C->bd=
_tmp185;_tmp18C;});_tmp18B->tl=_tmp181;_tmp18B;});};}return _tmp181;};}struct
_tuple23{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,
void*r1,void*r2){if(r1 == r2)return r1;{struct _RegionHandle*_tmp18D=(env->fenv)->r;{
struct _tuple0 _tmp18F=({struct _tuple0 _tmp18E;_tmp18E.f1=r1;_tmp18E.f2=r2;_tmp18E;});
void*_tmp190;struct Cyc_CfFlowInfo_Place*_tmp192;void*_tmp193;struct Cyc_CfFlowInfo_Place*
_tmp195;void*_tmp196;struct Cyc_CfFlowInfo_Place*_tmp198;void*_tmp199;struct Cyc_CfFlowInfo_Place*
_tmp19B;void*_tmp19C;void*_tmp19E;void*_tmp1A0;void*_tmp1A2;void*_tmp1A4;int
_tmp1A6;struct _dyneither_ptr _tmp1A7;void*_tmp1A8;int _tmp1AA;struct _dyneither_ptr
_tmp1AB;void*_tmp1AC;struct Cyc_List_List*_tmp1AE;void*_tmp1AF;struct Cyc_List_List*
_tmp1B1;void*_tmp1B2;void*_tmp1B4;_LL9D: _tmp190=_tmp18F.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp191=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp190;if(_tmp191->tag != 5)goto
_LL9F;else{_tmp192=_tmp191->f1;}};_tmp193=_tmp18F.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp194=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp193;if(_tmp194->tag != 5)goto
_LL9F;else{_tmp195=_tmp194->f1;}};_LL9E: if(Cyc_CfFlowInfo_place_cmp(_tmp192,
_tmp195)== 0)return r1;Cyc_CfFlowInfo_add_place(env->pile,_tmp192);Cyc_CfFlowInfo_add_place(
env->pile,_tmp195);goto _LL9C;_LL9F: _tmp196=_tmp18F.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp197=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp196;if(_tmp197->tag != 5)goto
_LLA1;else{_tmp198=_tmp197->f1;}};_LLA0: Cyc_CfFlowInfo_add_place(env->pile,
_tmp198);goto _LL9C;_LLA1: _tmp199=_tmp18F.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp19A=(struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp199;if(_tmp19A->tag != 5)goto
_LLA3;else{_tmp19B=_tmp19A->f1;}};_LLA2: Cyc_CfFlowInfo_add_place(env->pile,
_tmp19B);goto _LL9C;_LLA3: _tmp19C=_tmp18F.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp19D=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp19C;if(_tmp19D->tag != 1)
goto _LLA5;};_tmp19E=_tmp18F.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp19F=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp19E;if(_tmp19F->tag != 2)goto _LLA5;};
_LLA4: goto _LLA6;_LLA5: _tmp1A0=_tmp18F.f1;{struct Cyc_CfFlowInfo_NotZeroThis_struct*
_tmp1A1=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp1A0;if(_tmp1A1->tag != 2)
goto _LLA7;};_tmp1A2=_tmp18F.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp1A3=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp1A2;if(_tmp1A3->tag != 1)goto _LLA7;};
_LLA6: return(env->fenv)->notzerothis;_LLA7: _tmp1A4=_tmp18F.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp1A5=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp1A4;if(_tmp1A5->tag != 7)goto
_LLA9;else{_tmp1A6=_tmp1A5->f1;_tmp1A7=_tmp1A5->f2;}};_tmp1A8=_tmp18F.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp1A9=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp1A8;if(_tmp1A9->tag != 7)goto _LLA9;else{_tmp1AA=_tmp1A9->f1;_tmp1AB=_tmp1A9->f2;}};
_LLA8: {struct _dyneither_ptr new_d=({unsigned int _tmp1B8=_get_dyneither_size(
_tmp1A7,sizeof(void*));void**_tmp1B9=(void**)_region_malloc(_tmp18D,_check_times(
sizeof(void*),_tmp1B8));struct _dyneither_ptr _tmp1BB=_tag_dyneither(_tmp1B9,
sizeof(void*),_tmp1B8);{unsigned int _tmp1BA=_tmp1B8;unsigned int i;for(i=0;i < 
_tmp1BA;i ++){_tmp1B9[i]=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,
void*r2))Cyc_CfFlowInfo_join_absRval)(env,0,((void**)_tmp1A7.curr)[(int)i],*((
void**)_check_dyneither_subscript(_tmp1AB,sizeof(void*),(int)i)));}}_tmp1BB;});
int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp1A7,sizeof(void*));++ i){if(*((
void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1A7.curr)[
i]){change=1;break;}}}if(!change)return r1;change=0;{int i=0;for(0;i < 
_get_dyneither_size(_tmp1A7,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp1AB,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp1B6=
_region_malloc(_tmp18D,sizeof(*_tmp1B6));_tmp1B6[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp1B7;_tmp1B7.tag=7;_tmp1B7.f1=_tmp1A6;_tmp1B7.f2=new_d;_tmp1B7;});_tmp1B6;});}
_LLA9: _tmp1AC=_tmp18F.f1;{struct Cyc_CfFlowInfo_TagCmps_struct*_tmp1AD=(struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp1AC;if(_tmp1AD->tag != 6)goto _LLAB;else{_tmp1AE=_tmp1AD->f1;}};_tmp1AF=
_tmp18F.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*_tmp1B0=(struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp1AF;if(_tmp1B0->tag != 6)goto _LLAB;else{_tmp1B1=_tmp1B0->f1;}};_LLAA: {struct
Cyc_List_List*_tmp1BC=Cyc_CfFlowInfo_join_tag_cmps(_tmp18D,_tmp1AE,_tmp1B1);if(
_tmp1BC == _tmp1AE)return r1;return(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp1BD=_region_malloc(_tmp18D,sizeof(*_tmp1BD));_tmp1BD[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp1BE;_tmp1BE.tag=6;_tmp1BE.f1=_tmp1BC;_tmp1BE;});_tmp1BD;});}_LLAB: _tmp1B2=
_tmp18F.f1;{struct Cyc_CfFlowInfo_TagCmps_struct*_tmp1B3=(struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp1B2;if(_tmp1B3->tag != 6)goto _LLAD;};_LLAC: return r2;_LLAD: _tmp1B4=_tmp18F.f2;{
struct Cyc_CfFlowInfo_TagCmps_struct*_tmp1B5=(struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp1B4;if(_tmp1B5->tag != 6)goto _LLAF;};_LLAE: return r1;_LLAF:;_LLB0: goto _LL9C;
_LL9C:;}{enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,
r1);enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,
r2);struct _tuple0 _tmp1C0=({struct _tuple0 _tmp1BF;_tmp1BF.f1=r1;_tmp1BF.f2=r2;
_tmp1BF;});void*_tmp1C1;void*_tmp1C3;_LLB2: _tmp1C1=_tmp1C0.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp1C2=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp1C1;if(_tmp1C2->tag != 4)goto _LLB4;};
_LLB3: goto _LLB5;_LLB4: _tmp1C3=_tmp1C0.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp1C4=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp1C3;if(_tmp1C4->tag != 4)goto _LLB6;};
_LLB5: {struct _tuple23 _tmp1C6=({struct _tuple23 _tmp1C5;_tmp1C5.f1=il1;_tmp1C5.f2=
il2;_tmp1C5;});enum Cyc_CfFlowInfo_InitLevel _tmp1C7;enum Cyc_CfFlowInfo_InitLevel
_tmp1C8;enum Cyc_CfFlowInfo_InitLevel _tmp1C9;enum Cyc_CfFlowInfo_InitLevel
_tmp1CA;_LLB9: _tmp1C7=_tmp1C6.f2;if(_tmp1C7 != Cyc_CfFlowInfo_NoneIL)goto _LLBB;
_LLBA: goto _LLBC;_LLBB: _tmp1C8=_tmp1C6.f1;if(_tmp1C8 != Cyc_CfFlowInfo_NoneIL)goto
_LLBD;_LLBC: return(env->fenv)->esc_none;_LLBD: _tmp1C9=_tmp1C6.f2;if(_tmp1C9 != 
Cyc_CfFlowInfo_ThisIL)goto _LLBF;_LLBE: goto _LLC0;_LLBF: _tmp1CA=_tmp1C6.f1;if(
_tmp1CA != Cyc_CfFlowInfo_ThisIL)goto _LLC1;_LLC0: return(env->fenv)->esc_this;
_LLC1:;_LLC2: return(env->fenv)->esc_all;_LLB8:;}_LLB6:;_LLB7: {struct _tuple23
_tmp1CC=({struct _tuple23 _tmp1CB;_tmp1CB.f1=il1;_tmp1CB.f2=il2;_tmp1CB;});enum 
Cyc_CfFlowInfo_InitLevel _tmp1CD;enum Cyc_CfFlowInfo_InitLevel _tmp1CE;enum Cyc_CfFlowInfo_InitLevel
_tmp1CF;enum Cyc_CfFlowInfo_InitLevel _tmp1D0;_LLC4: _tmp1CD=_tmp1CC.f2;if(_tmp1CD
!= Cyc_CfFlowInfo_NoneIL)goto _LLC6;_LLC5: goto _LLC7;_LLC6: _tmp1CE=_tmp1CC.f1;if(
_tmp1CE != Cyc_CfFlowInfo_NoneIL)goto _LLC8;_LLC7: return(env->fenv)->unknown_none;
_LLC8: _tmp1CF=_tmp1CC.f2;if(_tmp1CF != Cyc_CfFlowInfo_ThisIL)goto _LLCA;_LLC9: goto
_LLCB;_LLCA: _tmp1D0=_tmp1CC.f1;if(_tmp1D0 != Cyc_CfFlowInfo_ThisIL)goto _LLCC;
_LLCB: return(env->fenv)->unknown_this;_LLCC:;_LLCD: return(env->fenv)->unknown_all;
_LLC3:;}_LLB1:;};};}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_EqualConst(
unsigned int i){return({union Cyc_CfFlowInfo_RelnOp _tmp1D1;(_tmp1D1.EqualConst).val=
i;(_tmp1D1.EqualConst).tag=1;_tmp1D1;});}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessVar(
struct Cyc_Absyn_Vardecl*v,void*t){return({union Cyc_CfFlowInfo_RelnOp _tmp1D2;(
_tmp1D2.LessVar).val=({struct _tuple11 _tmp1D3;_tmp1D3.f1=v;_tmp1D3.f2=t;_tmp1D3;});(
_tmp1D2.LessVar).tag=2;_tmp1D2;});}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessNumelts(
struct Cyc_Absyn_Vardecl*v){return({union Cyc_CfFlowInfo_RelnOp _tmp1D4;(_tmp1D4.LessNumelts).val=
v;(_tmp1D4.LessNumelts).tag=3;_tmp1D4;});}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessConst(
unsigned int i){return({union Cyc_CfFlowInfo_RelnOp _tmp1D5;(_tmp1D5.LessConst).val=
i;(_tmp1D5.LessConst).tag=4;_tmp1D5;});}union Cyc_CfFlowInfo_RelnOp Cyc_CfFlowInfo_LessEqNumelts(
struct Cyc_Absyn_Vardecl*v){return({union Cyc_CfFlowInfo_RelnOp _tmp1D6;(_tmp1D6.LessEqNumelts).val=
v;(_tmp1D6.LessEqNumelts).tag=5;_tmp1D6;});}struct _tuple24{union Cyc_CfFlowInfo_RelnOp
f1;union Cyc_CfFlowInfo_RelnOp f2;};static int Cyc_CfFlowInfo_same_relop(union Cyc_CfFlowInfo_RelnOp
r1,union Cyc_CfFlowInfo_RelnOp r2){struct _tuple24 _tmp1D8=({struct _tuple24 _tmp1D7;
_tmp1D7.f1=r1;_tmp1D7.f2=r2;_tmp1D7;});union Cyc_CfFlowInfo_RelnOp _tmp1D9;
unsigned int _tmp1DA;union Cyc_CfFlowInfo_RelnOp _tmp1DB;unsigned int _tmp1DC;union
Cyc_CfFlowInfo_RelnOp _tmp1DD;struct _tuple11 _tmp1DE;struct Cyc_Absyn_Vardecl*
_tmp1DF;union Cyc_CfFlowInfo_RelnOp _tmp1E0;struct _tuple11 _tmp1E1;struct Cyc_Absyn_Vardecl*
_tmp1E2;union Cyc_CfFlowInfo_RelnOp _tmp1E3;struct Cyc_Absyn_Vardecl*_tmp1E4;union
Cyc_CfFlowInfo_RelnOp _tmp1E5;struct Cyc_Absyn_Vardecl*_tmp1E6;union Cyc_CfFlowInfo_RelnOp
_tmp1E7;unsigned int _tmp1E8;union Cyc_CfFlowInfo_RelnOp _tmp1E9;unsigned int
_tmp1EA;union Cyc_CfFlowInfo_RelnOp _tmp1EB;struct Cyc_Absyn_Vardecl*_tmp1EC;union
Cyc_CfFlowInfo_RelnOp _tmp1ED;struct Cyc_Absyn_Vardecl*_tmp1EE;_LLCF: _tmp1D9=
_tmp1D8.f1;if((_tmp1D9.EqualConst).tag != 1)goto _LLD1;_tmp1DA=(unsigned int)(
_tmp1D9.EqualConst).val;_tmp1DB=_tmp1D8.f2;if((_tmp1DB.EqualConst).tag != 1)goto
_LLD1;_tmp1DC=(unsigned int)(_tmp1DB.EqualConst).val;_LLD0: return _tmp1DA == 
_tmp1DC;_LLD1: _tmp1DD=_tmp1D8.f1;if((_tmp1DD.LessVar).tag != 2)goto _LLD3;_tmp1DE=(
struct _tuple11)(_tmp1DD.LessVar).val;_tmp1DF=_tmp1DE.f1;_tmp1E0=_tmp1D8.f2;if((
_tmp1E0.LessVar).tag != 2)goto _LLD3;_tmp1E1=(struct _tuple11)(_tmp1E0.LessVar).val;
_tmp1E2=_tmp1E1.f1;_LLD2: return _tmp1DF == _tmp1E2;_LLD3: _tmp1E3=_tmp1D8.f1;if((
_tmp1E3.LessNumelts).tag != 3)goto _LLD5;_tmp1E4=(struct Cyc_Absyn_Vardecl*)(
_tmp1E3.LessNumelts).val;_tmp1E5=_tmp1D8.f2;if((_tmp1E5.LessNumelts).tag != 3)
goto _LLD5;_tmp1E6=(struct Cyc_Absyn_Vardecl*)(_tmp1E5.LessNumelts).val;_LLD4:
return _tmp1E4 == _tmp1E6;_LLD5: _tmp1E7=_tmp1D8.f1;if((_tmp1E7.LessConst).tag != 4)
goto _LLD7;_tmp1E8=(unsigned int)(_tmp1E7.LessConst).val;_tmp1E9=_tmp1D8.f2;if((
_tmp1E9.LessConst).tag != 4)goto _LLD7;_tmp1EA=(unsigned int)(_tmp1E9.LessConst).val;
_LLD6: return _tmp1E8 == _tmp1EA;_LLD7: _tmp1EB=_tmp1D8.f1;if((_tmp1EB.LessEqNumelts).tag
!= 5)goto _LLD9;_tmp1EC=(struct Cyc_Absyn_Vardecl*)(_tmp1EB.LessEqNumelts).val;
_tmp1ED=_tmp1D8.f2;if((_tmp1ED.LessEqNumelts).tag != 5)goto _LLD9;_tmp1EE=(struct
Cyc_Absyn_Vardecl*)(_tmp1ED.LessEqNumelts).val;_LLD8: return _tmp1EC == _tmp1EE;
_LLD9:;_LLDA: return 0;_LLCE:;}static struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == 
r2s)return r1s;{struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp1EF=
r1s;for(0;_tmp1EF != 0;_tmp1EF=_tmp1EF->tl){struct Cyc_CfFlowInfo_Reln*_tmp1F0=(
struct Cyc_CfFlowInfo_Reln*)_tmp1EF->hd;int found=0;{struct Cyc_List_List*_tmp1F1=
r2s;for(0;_tmp1F1 != 0;_tmp1F1=_tmp1F1->tl){struct Cyc_CfFlowInfo_Reln*_tmp1F2=(
struct Cyc_CfFlowInfo_Reln*)_tmp1F1->hd;if(_tmp1F0 == _tmp1F2  || _tmp1F0->vd == 
_tmp1F2->vd  && Cyc_CfFlowInfo_same_relop(_tmp1F0->rop,_tmp1F2->rop)){res=({
struct Cyc_List_List*_tmp1F3=_region_malloc(r,sizeof(*_tmp1F3));_tmp1F3->hd=
_tmp1F0;_tmp1F3->tl=res;_tmp1F3;});found=1;break;}}}if(!found)diff=1;}}if(!diff)
res=r1s;return res;};}static void Cyc_CfFlowInfo_remove_f(struct Cyc_List_List**l,
struct Cyc_CfFlowInfo_Place*place){struct _handler_cons _tmp1F4;_push_handler(&
_tmp1F4);{int _tmp1F6=0;if(setjmp(_tmp1F4.handler))_tmp1F6=1;if(!_tmp1F6){*l=((
struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_delete_cmp)(Cyc_CfFlowInfo_place_cmp,*
l,place);;_pop_handler();}else{void*_tmp1F5=(void*)_exn_thrown;void*_tmp1F8=
_tmp1F5;_LLDC: {struct Cyc_Core_Not_found_struct*_tmp1F9=(struct Cyc_Core_Not_found_struct*)
_tmp1F8;if(_tmp1F9->tag != Cyc_Core_Not_found)goto _LLDE;}_LLDD: goto _LLDB;_LLDE:;
_LLDF:(void)_throw(_tmp1F8);_LLDB:;}};}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_or_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*mc=c2.may_consume;struct _RegionHandle*_tmp1FA=r;{struct
Cyc_List_List*_tmp1FB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(_tmp1FA,c1.may_consume);((void(*)(void(*f)(struct Cyc_List_List**,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List**env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_CfFlowInfo_remove_f,& _tmp1FB,c2.may_consume);while(_tmp1FB != 0){mc=({struct
Cyc_List_List*_tmp1FC=_region_malloc(r,sizeof(*_tmp1FC));_tmp1FC->hd=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmp1FB))->hd;_tmp1FC->tl=mc;_tmp1FC;});_tmp1FB=((
struct Cyc_List_List*)_check_null(_tmp1FB))->tl;}}return({struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1FD;_tmp1FD.consumed=Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,
0);_tmp1FD.may_consume=mc;_tmp1FD;});}struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*r,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2){struct Cyc_List_List*_tmp1FE=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,c1.may_consume,
c2.may_consume);return({struct Cyc_CfFlowInfo_ConsumeInfo _tmp1FF;_tmp1FF.consumed=
Cyc_CfFlowInfo_union_place_set(c1.consumed,c2.consumed,1);_tmp1FF.may_consume=
_tmp1FE;_tmp1FF;});}struct _tuple25{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo
f2;};union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2,int or_consumed){struct _tuple25 _tmp201=({struct _tuple25 _tmp200;_tmp200.f1=f1;
_tmp200.f2=f2;_tmp200;});union Cyc_CfFlowInfo_FlowInfo _tmp202;int _tmp203;union Cyc_CfFlowInfo_FlowInfo
_tmp204;int _tmp205;union Cyc_CfFlowInfo_FlowInfo _tmp206;struct _tuple12 _tmp207;
struct Cyc_Dict_Dict _tmp208;struct Cyc_List_List*_tmp209;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp20A;union Cyc_CfFlowInfo_FlowInfo _tmp20B;struct _tuple12 _tmp20C;struct Cyc_Dict_Dict
_tmp20D;struct Cyc_List_List*_tmp20E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp20F;
_LLE1: _tmp202=_tmp201.f1;if((_tmp202.BottomFL).tag != 1)goto _LLE3;_tmp203=(int)(
_tmp202.BottomFL).val;_LLE2: return f2;_LLE3: _tmp204=_tmp201.f2;if((_tmp204.BottomFL).tag
!= 1)goto _LLE5;_tmp205=(int)(_tmp204.BottomFL).val;_LLE4: return f1;_LLE5: _tmp206=
_tmp201.f1;if((_tmp206.ReachableFL).tag != 2)goto _LLE0;_tmp207=(struct _tuple12)(
_tmp206.ReachableFL).val;_tmp208=_tmp207.f1;_tmp209=_tmp207.f2;_tmp20A=_tmp207.f3;
_tmp20B=_tmp201.f2;if((_tmp20B.ReachableFL).tag != 2)goto _LLE0;_tmp20C=(struct
_tuple12)(_tmp20B.ReachableFL).val;_tmp20D=_tmp20C.f1;_tmp20E=_tmp20C.f2;_tmp20F=
_tmp20C.f3;_LLE6: if(((_tmp208.t == _tmp20D.t  && _tmp209 == _tmp20E) && _tmp20A.may_consume
== _tmp20F.may_consume) && (_tmp20A.consumed).t == (_tmp20F.consumed).t)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;if(Cyc_CfFlowInfo_flow_lessthan_approx(
f2,f1))return f1;{struct _RegionHandle*_tmp210=fenv->r;struct Cyc_CfFlowInfo_JoinEnv
_tmp211=({struct Cyc_CfFlowInfo_JoinEnv _tmp215;_tmp215.fenv=fenv;_tmp215.pile=({
struct Cyc_CfFlowInfo_EscPile*_tmp216=_region_malloc(_tmp210,sizeof(*_tmp216));
_tmp216->rgn=_tmp210;_tmp216->places=0;_tmp216;});_tmp215.d1=_tmp208;_tmp215.d2=
_tmp20D;_tmp215;});struct Cyc_Dict_Dict _tmp212=((struct Cyc_Dict_Dict(*)(void*(*f)(
struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*
env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,&
_tmp211,_tmp208,_tmp20D);struct Cyc_List_List*_tmp213=Cyc_CfFlowInfo_join_relns(
fenv->r,_tmp209,_tmp20E);struct Cyc_CfFlowInfo_ConsumeInfo _tmp214=or_consumed?Cyc_CfFlowInfo_or_consume(
fenv->r,_tmp20A,_tmp20F): Cyc_CfFlowInfo_and_consume(fenv->r,_tmp20A,_tmp20F);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp211.pile,
all_changed,_tmp212),_tmp213,_tmp214);};_LLE0:;}struct _tuple26{union Cyc_CfFlowInfo_FlowInfo
f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};struct
_tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2,int
or_consumed){union Cyc_CfFlowInfo_FlowInfo _tmp218;void*_tmp219;struct _tuple13
_tmp217=pr1;_tmp218=_tmp217.f1;_tmp219=_tmp217.f2;{union Cyc_CfFlowInfo_FlowInfo
_tmp21B;void*_tmp21C;struct _tuple13 _tmp21A=pr2;_tmp21B=_tmp21A.f1;_tmp21C=
_tmp21A.f2;{union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,
all_changed,_tmp218,_tmp21B,or_consumed);struct _tuple26 _tmp21E=({struct _tuple26
_tmp21D;_tmp21D.f1=_tmp218;_tmp21D.f2=_tmp21B;_tmp21D.f3=outflow;_tmp21D;});
union Cyc_CfFlowInfo_FlowInfo _tmp21F;int _tmp220;union Cyc_CfFlowInfo_FlowInfo
_tmp221;int _tmp222;union Cyc_CfFlowInfo_FlowInfo _tmp223;struct _tuple12 _tmp224;
struct Cyc_Dict_Dict _tmp225;union Cyc_CfFlowInfo_FlowInfo _tmp226;struct _tuple12
_tmp227;struct Cyc_Dict_Dict _tmp228;union Cyc_CfFlowInfo_FlowInfo _tmp229;struct
_tuple12 _tmp22A;struct Cyc_Dict_Dict _tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp22D;_LLE8: _tmp21F=_tmp21E.f1;if((_tmp21F.BottomFL).tag != 1)goto _LLEA;_tmp220=(
int)(_tmp21F.BottomFL).val;_LLE9: return({struct _tuple13 _tmp22E;_tmp22E.f1=
outflow;_tmp22E.f2=_tmp21C;_tmp22E;});_LLEA: _tmp221=_tmp21E.f2;if((_tmp221.BottomFL).tag
!= 1)goto _LLEC;_tmp222=(int)(_tmp221.BottomFL).val;_LLEB: return({struct _tuple13
_tmp22F;_tmp22F.f1=outflow;_tmp22F.f2=_tmp219;_tmp22F;});_LLEC: _tmp223=_tmp21E.f1;
if((_tmp223.ReachableFL).tag != 2)goto _LLEE;_tmp224=(struct _tuple12)(_tmp223.ReachableFL).val;
_tmp225=_tmp224.f1;_tmp226=_tmp21E.f2;if((_tmp226.ReachableFL).tag != 2)goto _LLEE;
_tmp227=(struct _tuple12)(_tmp226.ReachableFL).val;_tmp228=_tmp227.f1;_tmp229=
_tmp21E.f3;if((_tmp229.ReachableFL).tag != 2)goto _LLEE;_tmp22A=(struct _tuple12)(
_tmp229.ReachableFL).val;_tmp22B=_tmp22A.f1;_tmp22C=_tmp22A.f2;_tmp22D=_tmp22A.f3;
_LLED: if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp219,_tmp21C))return({struct _tuple13 _tmp230;_tmp230.f1=outflow;_tmp230.f2=
_tmp21C;_tmp230;});if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(
0,_tmp21C,_tmp219))return({struct _tuple13 _tmp231;_tmp231.f1=outflow;_tmp231.f2=
_tmp219;_tmp231;});{struct _RegionHandle*_tmp232=fenv->r;struct Cyc_CfFlowInfo_JoinEnv
_tmp233=({struct Cyc_CfFlowInfo_JoinEnv _tmp236;_tmp236.fenv=fenv;_tmp236.pile=({
struct Cyc_CfFlowInfo_EscPile*_tmp237=_region_malloc(_tmp232,sizeof(*_tmp237));
_tmp237->rgn=_tmp232;_tmp237->places=0;_tmp237;});_tmp236.d1=_tmp225;_tmp236.d2=
_tmp228;_tmp236;});void*_tmp234=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,
void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp233,0,_tmp219,_tmp21C);return({
struct _tuple13 _tmp235;_tmp235.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,_tmp233.pile,all_changed,_tmp22B),_tmp22C,_tmp22D);_tmp235.f2=_tmp234;
_tmp235;});};_LLEE:;_LLEF:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238[0]=({struct Cyc_Core_Impossible_struct
_tmp239;_tmp239.tag=Cyc_Core_Impossible;_tmp239.f1=({const char*_tmp23A="join_flow_and_rval: BottomFL outflow";
_tag_dyneither(_tmp23A,sizeof(char),37);});_tmp239;});_tmp238;}));_LLE7:;};};}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,
void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*
env,void*r1,void*r2){int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,
env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,
env->curr_place);if(changed1  && changed2)return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
if(changed1){if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(!Cyc_CfFlowInfo_prefix_of_member(
env->curr_place,env->chg1))return r2;env->changed=Cyc_CfFlowInfo_Two;}{struct
_tuple0 _tmp23C=({struct _tuple0 _tmp23B;_tmp23B.f1=r1;_tmp23B.f2=r2;_tmp23B;});
void*_tmp23D;int _tmp23F;struct _dyneither_ptr _tmp240;void*_tmp241;int _tmp243;
struct _dyneither_ptr _tmp244;_LLF1: _tmp23D=_tmp23C.f1;{struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp23E=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp23D;if(_tmp23E->tag != 7)goto
_LLF3;else{_tmp23F=_tmp23E->f1;_tmp240=_tmp23E->f2;}};_tmp241=_tmp23C.f2;{struct
Cyc_CfFlowInfo_Aggregate_struct*_tmp242=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp241;if(_tmp242->tag != 7)goto _LLF3;else{_tmp243=_tmp242->f1;_tmp244=_tmp242->f2;}};
_LLF2: {struct _dyneither_ptr new_d=({unsigned int _tmp247=_get_dyneither_size(
_tmp240,sizeof(void*));void**_tmp248=(void**)_region_malloc(((env->joinenv).fenv)->r,
_check_times(sizeof(void*),_tmp247));struct _dyneither_ptr _tmp24A=_tag_dyneither(
_tmp248,sizeof(void*),_tmp247);{unsigned int _tmp249=_tmp247;unsigned int i;for(i=
0;i < _tmp249;i ++){_tmp248[i]=Cyc_CfFlowInfo_after_absRval_d(env,(int)i,((void**)
_tmp240.curr)[(int)i],*((void**)_check_dyneither_subscript(_tmp244,sizeof(void*),(
int)i)));}}_tmp24A;});int change=0;{int i=0;for(0;i < _get_dyneither_size(_tmp240,
sizeof(void*));++ i){if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),
i))!= ((void**)_tmp240.curr)[i]){change=1;break;}}}if(!change)return r1;change=0;{
int i=0;for(0;i < _get_dyneither_size(_tmp240,sizeof(void*));++ i){if(*((void**)
_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)
_check_dyneither_subscript(_tmp244,sizeof(void*),i))){change=1;break;}}}if(!
change)return r2;return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp245=
_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp245));_tmp245[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp246;_tmp246.tag=7;_tmp246.f1=_tmp23F;_tmp246.f2=new_d;_tmp246;});_tmp245;});}
_LLF3:;_LLF4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp24B=
_cycalloc(sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_Core_Impossible_struct _tmp24C;
_tmp24C.tag=Cyc_Core_Impossible;_tmp24C.f1=({const char*_tmp24D="after_absRval -- non-aggregates!";
_tag_dyneither(_tmp24D,sizeof(char),33);});_tmp24C;});_tmp24B;}));_LLF0:;};}
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int
key,void*r1,void*r2){if(r1 == r2)return r1;{struct Cyc_List_List**_tmp24E=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp24F=env->changed;*env->last_field_cell=({
struct Cyc_List_List*_tmp250=_region_malloc(((env->joinenv).fenv)->r,sizeof(*
_tmp250));_tmp250->hd=(void*)key;_tmp250->tl=0;_tmp250;});env->last_field_cell=(
struct Cyc_List_List**)&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp251=Cyc_CfFlowInfo_after_absRval(env,r1,r2);env->last_field_cell=
_tmp24E;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;env->changed=
_tmp24F;return _tmp251;};};}static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*
env,void*root,void*r1,void*r2){if(r1 == r2)return r1;*env->curr_place=({struct Cyc_CfFlowInfo_Place
_tmp252;_tmp252.root=root;_tmp252.fields=0;_tmp252;});env->last_field_cell=(
struct Cyc_List_List**)&(env->curr_place)->fields;env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(
struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo
f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict
chg2){struct _tuple25 _tmp254=({struct _tuple25 _tmp253;_tmp253.f1=f1;_tmp253.f2=f2;
_tmp253;});union Cyc_CfFlowInfo_FlowInfo _tmp255;int _tmp256;union Cyc_CfFlowInfo_FlowInfo
_tmp257;int _tmp258;union Cyc_CfFlowInfo_FlowInfo _tmp259;struct _tuple12 _tmp25A;
struct Cyc_Dict_Dict _tmp25B;struct Cyc_List_List*_tmp25C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp25D;union Cyc_CfFlowInfo_FlowInfo _tmp25E;struct _tuple12 _tmp25F;struct Cyc_Dict_Dict
_tmp260;struct Cyc_List_List*_tmp261;struct Cyc_CfFlowInfo_ConsumeInfo _tmp262;
_LLF6: _tmp255=_tmp254.f1;if((_tmp255.BottomFL).tag != 1)goto _LLF8;_tmp256=(int)(
_tmp255.BottomFL).val;_LLF7: goto _LLF9;_LLF8: _tmp257=_tmp254.f2;if((_tmp257.BottomFL).tag
!= 1)goto _LLFA;_tmp258=(int)(_tmp257.BottomFL).val;_LLF9: return Cyc_CfFlowInfo_BottomFL();
_LLFA: _tmp259=_tmp254.f1;if((_tmp259.ReachableFL).tag != 2)goto _LLF5;_tmp25A=(
struct _tuple12)(_tmp259.ReachableFL).val;_tmp25B=_tmp25A.f1;_tmp25C=_tmp25A.f2;
_tmp25D=_tmp25A.f3;_tmp25E=_tmp254.f2;if((_tmp25E.ReachableFL).tag != 2)goto _LLF5;
_tmp25F=(struct _tuple12)(_tmp25E.ReachableFL).val;_tmp260=_tmp25F.f1;_tmp261=
_tmp25F.f2;_tmp262=_tmp25F.f3;_LLFB: if(((_tmp25B.t == _tmp260.t  && _tmp25C == 
_tmp261) && _tmp25D.may_consume == _tmp262.may_consume) && (_tmp25D.consumed).t == (
_tmp262.consumed).t)return f1;{struct _RegionHandle*_tmp263=fenv->r;struct Cyc_CfFlowInfo_Place*
_tmp264=fenv->dummy_place;struct Cyc_CfFlowInfo_AfterEnv _tmp265=({struct Cyc_CfFlowInfo_AfterEnv
_tmp269;_tmp269.joinenv=({struct Cyc_CfFlowInfo_JoinEnv _tmp26A;_tmp26A.fenv=fenv;
_tmp26A.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp26B=_region_malloc(_tmp263,
sizeof(*_tmp26B));_tmp26B->rgn=_tmp263;_tmp26B->places=0;_tmp26B;});_tmp26A.d1=
_tmp25B;_tmp26A.d2=_tmp260;_tmp26A;});_tmp269.chg1=chg1;_tmp269.chg2=chg2;
_tmp269.curr_place=_tmp264;_tmp269.last_field_cell=(struct Cyc_List_List**)&
_tmp264->fields;_tmp269.changed=Cyc_CfFlowInfo_Neither;_tmp269;});struct Cyc_Dict_Dict
_tmp266=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,
void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict
d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp265,_tmp25B,_tmp260);
struct Cyc_List_List*_tmp267=Cyc_CfFlowInfo_join_relns(fenv->r,_tmp25C,_tmp261);
struct Cyc_CfFlowInfo_ConsumeInfo _tmp268=Cyc_CfFlowInfo_and_consume(fenv->r,
_tmp25D,_tmp262);return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(
fenv,(_tmp265.joinenv).pile,all_changed,_tmp266),_tmp267,_tmp268);};_LLF5:;}
static int Cyc_CfFlowInfo_tag_cmps_lessthan_approx(struct Cyc_List_List*l1,struct
Cyc_List_List*l2){for(0;l2 != 0;l2=l2->tl){struct Cyc_CfFlowInfo_TagCmp _tmp26D;
enum Cyc_Absyn_Primop _tmp26E;void*_tmp26F;struct Cyc_CfFlowInfo_TagCmp*_tmp26C=(
struct Cyc_CfFlowInfo_TagCmp*)l2->hd;_tmp26D=*_tmp26C;_tmp26E=_tmp26D.cmp;_tmp26F=
_tmp26D.bd;{struct Cyc_List_List*_tmp270=l1;for(0;_tmp270 != 0;_tmp270=_tmp270->tl){
struct Cyc_CfFlowInfo_TagCmp _tmp272;enum Cyc_Absyn_Primop _tmp273;void*_tmp274;
struct Cyc_CfFlowInfo_TagCmp*_tmp271=(struct Cyc_CfFlowInfo_TagCmp*)_tmp270->hd;
_tmp272=*_tmp271;_tmp273=_tmp272.cmp;_tmp274=_tmp272.bd;if(_tmp273 == _tmp26E  && 
Cyc_Tcutil_typecmp(_tmp274,_tmp26F)== 0)break;}if(_tmp270 == 0)return 0;};}return 1;}
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){if(
r1 == r2)return 1;{struct _tuple0 _tmp276=({struct _tuple0 _tmp275;_tmp275.f1=r1;
_tmp275.f2=r2;_tmp275;});void*_tmp277;struct Cyc_CfFlowInfo_Place*_tmp279;void*
_tmp27A;struct Cyc_CfFlowInfo_Place*_tmp27C;void*_tmp27D;void*_tmp27F;void*
_tmp281;int _tmp283;struct _dyneither_ptr _tmp284;void*_tmp285;int _tmp287;struct
_dyneither_ptr _tmp288;void*_tmp289;void*_tmp28B;void*_tmp28D;void*_tmp28F;void*
_tmp291;void*_tmp293;void*_tmp295;void*_tmp297;void*_tmp299;struct Cyc_List_List*
_tmp29B;void*_tmp29C;struct Cyc_List_List*_tmp29E;void*_tmp29F;_LLFD: _tmp277=
_tmp276.f1;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp278=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp277;if(_tmp278->tag != 5)goto _LLFF;else{_tmp279=_tmp278->f1;}};_tmp27A=
_tmp276.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp27B=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp27A;if(_tmp27B->tag != 5)goto _LLFF;else{_tmp27C=_tmp27B->f1;}};_LLFE: return
Cyc_CfFlowInfo_place_cmp(_tmp279,_tmp27C)== 0;_LLFF: _tmp27D=_tmp276.f1;{struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp27E=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp27D;if(_tmp27E->tag != 5)goto _LL101;};_LL100: goto _LL102;_LL101: _tmp27F=
_tmp276.f2;{struct Cyc_CfFlowInfo_AddressOf_struct*_tmp280=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp27F;if(_tmp280->tag != 5)goto _LL103;};_LL102: return 0;_LL103: _tmp281=_tmp276.f1;{
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp282=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp281;if(_tmp282->tag != 7)goto _LL105;else{_tmp283=_tmp282->f1;_tmp284=_tmp282->f2;}};
_tmp285=_tmp276.f2;{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp286=(struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp285;if(_tmp286->tag != 7)goto _LL105;else{_tmp287=_tmp286->f1;_tmp288=_tmp286->f2;}};
_LL104: if(_tmp284.curr == _tmp288.curr)return 1;{int i=0;for(0;i < 
_get_dyneither_size(_tmp284,sizeof(void*));++ i){if(!((int(*)(int ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp284.curr)[i],*((
void**)_check_dyneither_subscript(_tmp288,sizeof(void*),i))))return 0;}}return 1;
_LL105: _tmp289=_tmp276.f1;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp28A=(
struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp289;if(_tmp28A->tag != 1)goto _LL107;};
_tmp28B=_tmp276.f2;{struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp28C=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp28B;if(_tmp28C->tag != 2)goto _LL107;};_LL106: return 1;_LL107: _tmp28D=_tmp276.f2;{
struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp28E=(struct Cyc_CfFlowInfo_NotZeroThis_struct*)
_tmp28D;if(_tmp28E->tag != 2)goto _LL109;};_LL108: return 0;_LL109: _tmp28F=_tmp276.f2;{
struct Cyc_CfFlowInfo_Zero_struct*_tmp290=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp28F;if(_tmp290->tag != 0)goto _LL10B;};_LL10A: goto _LL10C;_LL10B: _tmp291=
_tmp276.f2;{struct Cyc_CfFlowInfo_NotZeroAll_struct*_tmp292=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)
_tmp291;if(_tmp292->tag != 1)goto _LL10D;};_LL10C: return 0;_LL10D: _tmp293=_tmp276.f1;{
struct Cyc_CfFlowInfo_Esc_struct*_tmp294=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp293;if(_tmp294->tag != 4)goto _LL10F;};_tmp295=_tmp276.f2;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp296=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp295;if(_tmp296->tag != 4)goto _LL10F;};
_LL10E: goto _LLFC;_LL10F: _tmp297=_tmp276.f1;{struct Cyc_CfFlowInfo_Esc_struct*
_tmp298=(struct Cyc_CfFlowInfo_Esc_struct*)_tmp297;if(_tmp298->tag != 4)goto _LL111;};
_LL110: return 0;_LL111: _tmp299=_tmp276.f1;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp29A=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp299;if(_tmp29A->tag != 6)goto
_LL113;else{_tmp29B=_tmp29A->f1;}};_tmp29C=_tmp276.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp29D=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp29C;if(_tmp29D->tag != 6)goto
_LL113;else{_tmp29E=_tmp29D->f1;}};_LL112: return Cyc_CfFlowInfo_tag_cmps_lessthan_approx(
_tmp29B,_tmp29E);_LL113: _tmp29F=_tmp276.f2;{struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp2A0=(struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp29F;if(_tmp2A0->tag != 6)goto
_LL115;};_LL114: return 0;_LL115:;_LL116: goto _LLFC;_LLFC:;}{struct _tuple23 _tmp2A2=({
struct _tuple23 _tmp2A1;_tmp2A1.f1=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp2A1.f2=
Cyc_CfFlowInfo_initlevel_approx(r2);_tmp2A1;});enum Cyc_CfFlowInfo_InitLevel
_tmp2A3;enum Cyc_CfFlowInfo_InitLevel _tmp2A4;enum Cyc_CfFlowInfo_InitLevel
_tmp2A5;enum Cyc_CfFlowInfo_InitLevel _tmp2A6;enum Cyc_CfFlowInfo_InitLevel
_tmp2A7;enum Cyc_CfFlowInfo_InitLevel _tmp2A8;_LL118: _tmp2A3=_tmp2A2.f1;if(
_tmp2A3 != Cyc_CfFlowInfo_AllIL)goto _LL11A;_tmp2A4=_tmp2A2.f2;if(_tmp2A4 != Cyc_CfFlowInfo_AllIL)
goto _LL11A;_LL119: return 1;_LL11A: _tmp2A5=_tmp2A2.f2;if(_tmp2A5 != Cyc_CfFlowInfo_NoneIL)
goto _LL11C;_LL11B: return 1;_LL11C: _tmp2A6=_tmp2A2.f1;if(_tmp2A6 != Cyc_CfFlowInfo_NoneIL)
goto _LL11E;_LL11D: return 0;_LL11E: _tmp2A7=_tmp2A2.f2;if(_tmp2A7 != Cyc_CfFlowInfo_ThisIL)
goto _LL120;_LL11F: return 1;_LL120: _tmp2A8=_tmp2A2.f1;if(_tmp2A8 != Cyc_CfFlowInfo_ThisIL)
goto _LL117;_LL121: return 0;_LL117:;};}static int Cyc_CfFlowInfo_relns_approx(struct
Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s)return 1;for(0;r1s != 0;
r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp2A9=(struct Cyc_CfFlowInfo_Reln*)r1s->hd;
int found=0;{struct Cyc_List_List*_tmp2AA=r2s;for(0;_tmp2AA != 0;_tmp2AA=_tmp2AA->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2AB=(struct Cyc_CfFlowInfo_Reln*)_tmp2AA->hd;if(
_tmp2A9 == _tmp2AB  || _tmp2A9->vd == _tmp2AB->vd  && Cyc_CfFlowInfo_same_relop(
_tmp2A9->rop,_tmp2AB->rop)){found=1;break;}}}if(!found)return 0;}return 1;}int Cyc_CfFlowInfo_consume_approx(
struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo c2){if(c1.may_consume
!= c2.may_consume){struct Cyc_List_List*_tmp2AC=c1.may_consume;struct Cyc_List_List*
_tmp2AD=c2.may_consume;for(0;_tmp2AD != 0  && _tmp2AC != 0;_tmp2AD=_tmp2AD->tl){if((
unsigned int)_tmp2AC == (unsigned int)_tmp2AD)goto compare_consumed_set;{int
_tmp2AE=Cyc_CfFlowInfo_place_cmp((struct Cyc_CfFlowInfo_Place*)_tmp2AC->hd,(
struct Cyc_CfFlowInfo_Place*)_tmp2AD->hd);if(_tmp2AE == 0)_tmp2AC=_tmp2AC->tl;};}
if(_tmp2AC != 0)return 0;}compare_consumed_set: return((int(*)(struct Cyc_Dict_Dict
s1,struct Cyc_Dict_Dict s2))Cyc_CfFlowInfo_place_set_subset)(c1.consumed,c2.consumed);}
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo
f2){struct _tuple25 _tmp2B0=({struct _tuple25 _tmp2AF;_tmp2AF.f1=f1;_tmp2AF.f2=f2;
_tmp2AF;});union Cyc_CfFlowInfo_FlowInfo _tmp2B1;int _tmp2B2;union Cyc_CfFlowInfo_FlowInfo
_tmp2B3;int _tmp2B4;union Cyc_CfFlowInfo_FlowInfo _tmp2B5;struct _tuple12 _tmp2B6;
struct Cyc_Dict_Dict _tmp2B7;struct Cyc_List_List*_tmp2B8;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2B9;union Cyc_CfFlowInfo_FlowInfo _tmp2BA;struct _tuple12 _tmp2BB;struct Cyc_Dict_Dict
_tmp2BC;struct Cyc_List_List*_tmp2BD;struct Cyc_CfFlowInfo_ConsumeInfo _tmp2BE;
_LL123: _tmp2B1=_tmp2B0.f1;if((_tmp2B1.BottomFL).tag != 1)goto _LL125;_tmp2B2=(int)(
_tmp2B1.BottomFL).val;_LL124: return 1;_LL125: _tmp2B3=_tmp2B0.f2;if((_tmp2B3.BottomFL).tag
!= 1)goto _LL127;_tmp2B4=(int)(_tmp2B3.BottomFL).val;_LL126: return 0;_LL127:
_tmp2B5=_tmp2B0.f1;if((_tmp2B5.ReachableFL).tag != 2)goto _LL122;_tmp2B6=(struct
_tuple12)(_tmp2B5.ReachableFL).val;_tmp2B7=_tmp2B6.f1;_tmp2B8=_tmp2B6.f2;_tmp2B9=
_tmp2B6.f3;_tmp2BA=_tmp2B0.f2;if((_tmp2BA.ReachableFL).tag != 2)goto _LL122;
_tmp2BB=(struct _tuple12)(_tmp2BA.ReachableFL).val;_tmp2BC=_tmp2BB.f1;_tmp2BD=
_tmp2BB.f2;_tmp2BE=_tmp2BB.f3;_LL128: if(((_tmp2B7.t == _tmp2BC.t  && _tmp2B8 == 
_tmp2BD) && _tmp2B9.may_consume == _tmp2BE.may_consume) && (_tmp2B9.consumed).t == (
_tmp2BE.consumed).t)return 1;return(((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict
d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,
void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp2B7,_tmp2BC) && Cyc_CfFlowInfo_relns_approx(
_tmp2B8,_tmp2BD)) && Cyc_CfFlowInfo_consume_approx(_tmp2B9,_tmp2BE);_LL122:;}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(struct _RegionHandle*rgn,struct
Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=0;
for(p=rs;!found  && p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp2BF=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp2BF->vd == v){found=1;break;}{union Cyc_CfFlowInfo_RelnOp _tmp2C0=
_tmp2BF->rop;struct _tuple11 _tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C2;struct Cyc_Absyn_Vardecl*
_tmp2C3;struct Cyc_Absyn_Vardecl*_tmp2C4;_LL12A: if((_tmp2C0.LessVar).tag != 2)goto
_LL12C;_tmp2C1=(struct _tuple11)(_tmp2C0.LessVar).val;_tmp2C2=_tmp2C1.f1;_LL12B:
_tmp2C3=_tmp2C2;goto _LL12D;_LL12C: if((_tmp2C0.LessNumelts).tag != 3)goto _LL12E;
_tmp2C3=(struct Cyc_Absyn_Vardecl*)(_tmp2C0.LessNumelts).val;_LL12D: _tmp2C4=
_tmp2C3;goto _LL12F;_LL12E: if((_tmp2C0.LessEqNumelts).tag != 5)goto _LL130;_tmp2C4=(
struct Cyc_Absyn_Vardecl*)(_tmp2C0.LessEqNumelts).val;_LL12F: if(v == _tmp2C4)found=
1;goto _LL129;_LL130:;_LL131: goto _LL129;_LL129:;};}if(!found)return rs;{struct Cyc_List_List*
_tmp2C5=0;for(p=rs;p != 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp2C6=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp2C6->vd != v){{union Cyc_CfFlowInfo_RelnOp _tmp2C7=_tmp2C6->rop;struct
_tuple11 _tmp2C8;struct Cyc_Absyn_Vardecl*_tmp2C9;struct Cyc_Absyn_Vardecl*_tmp2CA;
struct Cyc_Absyn_Vardecl*_tmp2CB;_LL133: if((_tmp2C7.LessVar).tag != 2)goto _LL135;
_tmp2C8=(struct _tuple11)(_tmp2C7.LessVar).val;_tmp2C9=_tmp2C8.f1;_LL134: _tmp2CA=
_tmp2C9;goto _LL136;_LL135: if((_tmp2C7.LessNumelts).tag != 3)goto _LL137;_tmp2CA=(
struct Cyc_Absyn_Vardecl*)(_tmp2C7.LessNumelts).val;_LL136: _tmp2CB=_tmp2CA;goto
_LL138;_LL137: if((_tmp2C7.LessEqNumelts).tag != 5)goto _LL139;_tmp2CB=(struct Cyc_Absyn_Vardecl*)(
_tmp2C7.LessEqNumelts).val;_LL138: if(v == _tmp2CB)continue;goto _LL132;_LL139:;
_LL13A: goto _LL132;_LL132:;}_tmp2C5=({struct Cyc_List_List*_tmp2CC=_region_malloc(
rgn,sizeof(*_tmp2CC));_tmp2CC->hd=_tmp2C6;_tmp2CC->tl=_tmp2C5;_tmp2CC;});}}
return _tmp2C5;};}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct
_RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){{void*_tmp2CD=e->r;
void*_tmp2CF;struct Cyc_Absyn_Vardecl*_tmp2D1;void*_tmp2D3;struct Cyc_Absyn_Vardecl*
_tmp2D5;void*_tmp2D7;struct Cyc_Absyn_Vardecl*_tmp2D9;void*_tmp2DB;struct Cyc_Absyn_Vardecl*
_tmp2DD;_LL13C: {struct Cyc_Absyn_Var_e_struct*_tmp2CE=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CD;if(_tmp2CE->tag != 1)goto _LL13E;else{_tmp2CF=(void*)_tmp2CE->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp2D0=(struct Cyc_Absyn_Global_b_struct*)_tmp2CF;if(
_tmp2D0->tag != 1)goto _LL13E;else{_tmp2D1=_tmp2D0->f1;}};}}_LL13D: _tmp2D5=_tmp2D1;
goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_struct*_tmp2D2=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CD;if(_tmp2D2->tag != 1)goto _LL140;else{_tmp2D3=(void*)_tmp2D2->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp2D4=(struct Cyc_Absyn_Param_b_struct*)_tmp2D3;if(
_tmp2D4->tag != 3)goto _LL140;else{_tmp2D5=_tmp2D4->f1;}};}}_LL13F: _tmp2D9=_tmp2D5;
goto _LL141;_LL140: {struct Cyc_Absyn_Var_e_struct*_tmp2D6=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CD;if(_tmp2D6->tag != 1)goto _LL142;else{_tmp2D7=(void*)_tmp2D6->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp2D8=(struct Cyc_Absyn_Local_b_struct*)_tmp2D7;if(
_tmp2D8->tag != 4)goto _LL142;else{_tmp2D9=_tmp2D8->f1;}};}}_LL141: _tmp2DD=_tmp2D9;
goto _LL143;_LL142: {struct Cyc_Absyn_Var_e_struct*_tmp2DA=(struct Cyc_Absyn_Var_e_struct*)
_tmp2CD;if(_tmp2DA->tag != 1)goto _LL144;else{_tmp2DB=(void*)_tmp2DA->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp2DC=(struct Cyc_Absyn_Pat_b_struct*)_tmp2DB;if(_tmp2DC->tag
!= 5)goto _LL144;else{_tmp2DD=_tmp2DC->f1;}};}}_LL143: if(!_tmp2DD->escapes)return
Cyc_CfFlowInfo_reln_kill_var(rgn,r,_tmp2DD);goto _LL13B;_LL144:;_LL145: goto _LL13B;
_LL13B:;}return r;}struct Cyc_CfFlowInfo_TagCmp*Cyc_CfFlowInfo_copy_tagcmp(struct
_RegionHandle*r2,struct Cyc_CfFlowInfo_TagCmp*t){return({struct Cyc_CfFlowInfo_TagCmp*
_tmp2DE=_region_malloc(r2,sizeof(*_tmp2DE));_tmp2DE[0]=*t;_tmp2DE;});}struct Cyc_List_List*
Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*r2,struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*(*
f)(struct _RegionHandle*,struct Cyc_CfFlowInfo_TagCmp*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_CfFlowInfo_copy_tagcmp,r2,ts);}
struct Cyc_CfFlowInfo_Reln*Cyc_CfFlowInfo_copy_reln(struct _RegionHandle*r2,struct
Cyc_CfFlowInfo_Reln*r){return({struct Cyc_CfFlowInfo_Reln*_tmp2DF=_region_malloc(
r2,sizeof(*_tmp2DF));_tmp2DF[0]=*r;_tmp2DF;});}struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(
struct _RegionHandle*r2,struct Cyc_List_List*relns){return((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_CfFlowInfo_Reln*(*f)(struct _RegionHandle*,struct
Cyc_CfFlowInfo_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
r2,Cyc_CfFlowInfo_copy_reln,r2,relns);}int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*
r1,struct Cyc_List_List*r2){for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){struct
Cyc_CfFlowInfo_Reln*_tmp2E0=(struct Cyc_CfFlowInfo_Reln*)r1->hd;struct Cyc_CfFlowInfo_Reln*
_tmp2E1=(struct Cyc_CfFlowInfo_Reln*)r2->hd;if(_tmp2E0->vd != _tmp2E1->vd  || !Cyc_CfFlowInfo_same_relop(
_tmp2E0->rop,_tmp2E1->rop))return 0;}if(r1 != 0  || r2 != 0)return 0;else{return 1;}}
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*rgn,
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes)
return r;r=Cyc_CfFlowInfo_reln_kill_var(rgn,r,v);{void*_tmp2E2=e->r;struct Cyc_Absyn_MallocInfo
_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E5;int _tmp2E6;_LL147: {struct Cyc_Absyn_Malloc_e_struct*
_tmp2E3=(struct Cyc_Absyn_Malloc_e_struct*)_tmp2E2;if(_tmp2E3->tag != 34)goto
_LL149;else{_tmp2E4=_tmp2E3->f1;_tmp2E5=_tmp2E4.num_elts;_tmp2E6=_tmp2E4.fat_result;
if(_tmp2E6 != 1)goto _LL149;}}_LL148: malloc_loop: {void*_tmp2E7=_tmp2E5->r;struct
Cyc_Absyn_Exp*_tmp2E9;void*_tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2ED;void*_tmp2EF;
struct Cyc_Absyn_Vardecl*_tmp2F1;void*_tmp2F3;struct Cyc_Absyn_Vardecl*_tmp2F5;
void*_tmp2F7;struct Cyc_Absyn_Vardecl*_tmp2F9;_LL14C: {struct Cyc_Absyn_Cast_e_struct*
_tmp2E8=(struct Cyc_Absyn_Cast_e_struct*)_tmp2E7;if(_tmp2E8->tag != 15)goto _LL14E;
else{_tmp2E9=_tmp2E8->f2;}}_LL14D: _tmp2E5=_tmp2E9;goto malloc_loop;_LL14E: {
struct Cyc_Absyn_Var_e_struct*_tmp2EA=(struct Cyc_Absyn_Var_e_struct*)_tmp2E7;if(
_tmp2EA->tag != 1)goto _LL150;else{_tmp2EB=(void*)_tmp2EA->f2;{struct Cyc_Absyn_Pat_b_struct*
_tmp2EC=(struct Cyc_Absyn_Pat_b_struct*)_tmp2EB;if(_tmp2EC->tag != 5)goto _LL150;
else{_tmp2ED=_tmp2EC->f1;}};}}_LL14F: _tmp2F1=_tmp2ED;goto _LL151;_LL150: {struct
Cyc_Absyn_Var_e_struct*_tmp2EE=(struct Cyc_Absyn_Var_e_struct*)_tmp2E7;if(_tmp2EE->tag
!= 1)goto _LL152;else{_tmp2EF=(void*)_tmp2EE->f2;{struct Cyc_Absyn_Local_b_struct*
_tmp2F0=(struct Cyc_Absyn_Local_b_struct*)_tmp2EF;if(_tmp2F0->tag != 4)goto _LL152;
else{_tmp2F1=_tmp2F0->f1;}};}}_LL151: _tmp2F5=_tmp2F1;goto _LL153;_LL152: {struct
Cyc_Absyn_Var_e_struct*_tmp2F2=(struct Cyc_Absyn_Var_e_struct*)_tmp2E7;if(_tmp2F2->tag
!= 1)goto _LL154;else{_tmp2F3=(void*)_tmp2F2->f2;{struct Cyc_Absyn_Param_b_struct*
_tmp2F4=(struct Cyc_Absyn_Param_b_struct*)_tmp2F3;if(_tmp2F4->tag != 3)goto _LL154;
else{_tmp2F5=_tmp2F4->f1;}};}}_LL153: _tmp2F9=_tmp2F5;goto _LL155;_LL154: {struct
Cyc_Absyn_Var_e_struct*_tmp2F6=(struct Cyc_Absyn_Var_e_struct*)_tmp2E7;if(_tmp2F6->tag
!= 1)goto _LL156;else{_tmp2F7=(void*)_tmp2F6->f2;{struct Cyc_Absyn_Global_b_struct*
_tmp2F8=(struct Cyc_Absyn_Global_b_struct*)_tmp2F7;if(_tmp2F8->tag != 1)goto _LL156;
else{_tmp2F9=_tmp2F8->f1;}};}}_LL155: if(_tmp2F9->escapes)return r;return({struct
Cyc_List_List*_tmp2FA=_region_malloc(rgn,sizeof(*_tmp2FA));_tmp2FA->hd=({struct
Cyc_CfFlowInfo_Reln*_tmp2FB=_region_malloc(rgn,sizeof(*_tmp2FB));_tmp2FB->vd=
_tmp2F9;_tmp2FB->rop=Cyc_CfFlowInfo_LessEqNumelts(v);_tmp2FB;});_tmp2FA->tl=r;
_tmp2FA;});_LL156:;_LL157: return r;_LL14B:;}_LL149:;_LL14A: goto _LL146;_LL146:;}{
void*_tmp2FC=Cyc_Tcutil_compress(v->type);_LL159: {struct Cyc_Absyn_IntType_struct*
_tmp2FD=(struct Cyc_Absyn_IntType_struct*)_tmp2FC;if(_tmp2FD->tag != 6)goto _LL15B;}
_LL15A: goto _LL158;_LL15B:;_LL15C: return r;_LL158:;}loop: {void*_tmp2FE=e->r;
struct Cyc_Absyn_Exp*_tmp300;union Cyc_Absyn_Cnst _tmp302;struct _tuple6 _tmp303;int
_tmp304;enum Cyc_Absyn_Primop _tmp306;struct Cyc_List_List*_tmp307;struct Cyc_List_List
_tmp308;struct Cyc_List_List*_tmp309;struct Cyc_List_List _tmp30A;struct Cyc_Absyn_Exp*
_tmp30B;enum Cyc_Absyn_Primop _tmp30D;struct Cyc_List_List*_tmp30E;struct Cyc_List_List
_tmp30F;struct Cyc_Absyn_Exp*_tmp310;_LL15E: {struct Cyc_Absyn_Cast_e_struct*
_tmp2FF=(struct Cyc_Absyn_Cast_e_struct*)_tmp2FE;if(_tmp2FF->tag != 15)goto _LL160;
else{_tmp300=_tmp2FF->f2;}}_LL15F: e=_tmp300;goto loop;_LL160: {struct Cyc_Absyn_Const_e_struct*
_tmp301=(struct Cyc_Absyn_Const_e_struct*)_tmp2FE;if(_tmp301->tag != 0)goto _LL162;
else{_tmp302=_tmp301->f1;if((_tmp302.Int_c).tag != 4)goto _LL162;_tmp303=(struct
_tuple6)(_tmp302.Int_c).val;_tmp304=_tmp303.f2;}}_LL161: return({struct Cyc_List_List*
_tmp311=_region_malloc(rgn,sizeof(*_tmp311));_tmp311->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp312=_region_malloc(rgn,sizeof(*_tmp312));_tmp312->vd=v;_tmp312->rop=Cyc_CfFlowInfo_EqualConst((
unsigned int)_tmp304);_tmp312;});_tmp311->tl=r;_tmp311;});_LL162: {struct Cyc_Absyn_Primop_e_struct*
_tmp305=(struct Cyc_Absyn_Primop_e_struct*)_tmp2FE;if(_tmp305->tag != 3)goto _LL164;
else{_tmp306=_tmp305->f1;if(_tmp306 != Cyc_Absyn_Mod)goto _LL164;_tmp307=_tmp305->f2;
if(_tmp307 == 0)goto _LL164;_tmp308=*_tmp307;_tmp309=_tmp308.tl;if(_tmp309 == 0)
goto _LL164;_tmp30A=*_tmp309;_tmp30B=(struct Cyc_Absyn_Exp*)_tmp30A.hd;}}_LL163:{
void*_tmp313=_tmp30B->r;enum Cyc_Absyn_Primop _tmp315;struct Cyc_List_List*_tmp316;
struct Cyc_List_List _tmp317;struct Cyc_Absyn_Exp*_tmp318;_LL169: {struct Cyc_Absyn_Primop_e_struct*
_tmp314=(struct Cyc_Absyn_Primop_e_struct*)_tmp313;if(_tmp314->tag != 3)goto _LL16B;
else{_tmp315=_tmp314->f1;_tmp316=_tmp314->f2;if(_tmp316 == 0)goto _LL16B;_tmp317=*
_tmp316;_tmp318=(struct Cyc_Absyn_Exp*)_tmp317.hd;}}_LL16A:{void*_tmp319=_tmp318->r;
void*_tmp31B;struct Cyc_Absyn_Vardecl*_tmp31D;void*_tmp31F;struct Cyc_Absyn_Vardecl*
_tmp321;void*_tmp323;struct Cyc_Absyn_Vardecl*_tmp325;void*_tmp327;struct Cyc_Absyn_Vardecl*
_tmp329;_LL16E: {struct Cyc_Absyn_Var_e_struct*_tmp31A=(struct Cyc_Absyn_Var_e_struct*)
_tmp319;if(_tmp31A->tag != 1)goto _LL170;else{_tmp31B=(void*)_tmp31A->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp31C=(struct Cyc_Absyn_Global_b_struct*)_tmp31B;if(
_tmp31C->tag != 1)goto _LL170;else{_tmp31D=_tmp31C->f1;}};}}_LL16F: _tmp321=_tmp31D;
goto _LL171;_LL170: {struct Cyc_Absyn_Var_e_struct*_tmp31E=(struct Cyc_Absyn_Var_e_struct*)
_tmp319;if(_tmp31E->tag != 1)goto _LL172;else{_tmp31F=(void*)_tmp31E->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp320=(struct Cyc_Absyn_Local_b_struct*)_tmp31F;if(
_tmp320->tag != 4)goto _LL172;else{_tmp321=_tmp320->f1;}};}}_LL171: _tmp325=_tmp321;
goto _LL173;_LL172: {struct Cyc_Absyn_Var_e_struct*_tmp322=(struct Cyc_Absyn_Var_e_struct*)
_tmp319;if(_tmp322->tag != 1)goto _LL174;else{_tmp323=(void*)_tmp322->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp324=(struct Cyc_Absyn_Param_b_struct*)_tmp323;if(
_tmp324->tag != 3)goto _LL174;else{_tmp325=_tmp324->f1;}};}}_LL173: _tmp329=_tmp325;
goto _LL175;_LL174: {struct Cyc_Absyn_Var_e_struct*_tmp326=(struct Cyc_Absyn_Var_e_struct*)
_tmp319;if(_tmp326->tag != 1)goto _LL176;else{_tmp327=(void*)_tmp326->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp328=(struct Cyc_Absyn_Pat_b_struct*)_tmp327;if(_tmp328->tag
!= 5)goto _LL176;else{_tmp329=_tmp328->f1;}};}}_LL175: if(_tmp329->escapes)return r;
return({struct Cyc_List_List*_tmp32A=_region_malloc(rgn,sizeof(*_tmp32A));_tmp32A->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp32B=_region_malloc(rgn,sizeof(*_tmp32B));_tmp32B->vd=
v;_tmp32B->rop=Cyc_CfFlowInfo_LessNumelts(_tmp329);_tmp32B;});_tmp32A->tl=r;
_tmp32A;});_LL176:;_LL177: goto _LL16D;_LL16D:;}goto _LL168;_LL16B:;_LL16C: goto
_LL168;_LL168:;}goto _LL15D;_LL164: {struct Cyc_Absyn_Primop_e_struct*_tmp30C=(
struct Cyc_Absyn_Primop_e_struct*)_tmp2FE;if(_tmp30C->tag != 3)goto _LL166;else{
_tmp30D=_tmp30C->f1;_tmp30E=_tmp30C->f2;if(_tmp30E == 0)goto _LL166;_tmp30F=*
_tmp30E;_tmp310=(struct Cyc_Absyn_Exp*)_tmp30F.hd;}}_LL165:{void*_tmp32C=_tmp310->r;
void*_tmp32E;struct Cyc_Absyn_Vardecl*_tmp330;void*_tmp332;struct Cyc_Absyn_Vardecl*
_tmp334;void*_tmp336;struct Cyc_Absyn_Vardecl*_tmp338;void*_tmp33A;struct Cyc_Absyn_Vardecl*
_tmp33C;_LL179: {struct Cyc_Absyn_Var_e_struct*_tmp32D=(struct Cyc_Absyn_Var_e_struct*)
_tmp32C;if(_tmp32D->tag != 1)goto _LL17B;else{_tmp32E=(void*)_tmp32D->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp32F=(struct Cyc_Absyn_Global_b_struct*)_tmp32E;if(
_tmp32F->tag != 1)goto _LL17B;else{_tmp330=_tmp32F->f1;}};}}_LL17A: _tmp334=_tmp330;
goto _LL17C;_LL17B: {struct Cyc_Absyn_Var_e_struct*_tmp331=(struct Cyc_Absyn_Var_e_struct*)
_tmp32C;if(_tmp331->tag != 1)goto _LL17D;else{_tmp332=(void*)_tmp331->f2;{struct
Cyc_Absyn_Local_b_struct*_tmp333=(struct Cyc_Absyn_Local_b_struct*)_tmp332;if(
_tmp333->tag != 4)goto _LL17D;else{_tmp334=_tmp333->f1;}};}}_LL17C: _tmp338=_tmp334;
goto _LL17E;_LL17D: {struct Cyc_Absyn_Var_e_struct*_tmp335=(struct Cyc_Absyn_Var_e_struct*)
_tmp32C;if(_tmp335->tag != 1)goto _LL17F;else{_tmp336=(void*)_tmp335->f2;{struct
Cyc_Absyn_Param_b_struct*_tmp337=(struct Cyc_Absyn_Param_b_struct*)_tmp336;if(
_tmp337->tag != 3)goto _LL17F;else{_tmp338=_tmp337->f1;}};}}_LL17E: _tmp33C=_tmp338;
goto _LL180;_LL17F: {struct Cyc_Absyn_Var_e_struct*_tmp339=(struct Cyc_Absyn_Var_e_struct*)
_tmp32C;if(_tmp339->tag != 1)goto _LL181;else{_tmp33A=(void*)_tmp339->f2;{struct
Cyc_Absyn_Pat_b_struct*_tmp33B=(struct Cyc_Absyn_Pat_b_struct*)_tmp33A;if(_tmp33B->tag
!= 5)goto _LL181;else{_tmp33C=_tmp33B->f1;}};}}_LL180: if(_tmp33C->escapes)return r;
return({struct Cyc_List_List*_tmp33D=_region_malloc(rgn,sizeof(*_tmp33D));_tmp33D->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp33E=_region_malloc(rgn,sizeof(*_tmp33E));_tmp33E->vd=
v;_tmp33E->rop=Cyc_CfFlowInfo_LessEqNumelts(_tmp33C);_tmp33E;});_tmp33D->tl=r;
_tmp33D;});_LL181:;_LL182: goto _LL178;_LL178:;}goto _LL15D;_LL166:;_LL167: goto
_LL15D;_LL15D:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){{void*_tmp33F=e1->r;void*_tmp341;struct Cyc_Absyn_Vardecl*_tmp343;void*
_tmp345;struct Cyc_Absyn_Vardecl*_tmp347;void*_tmp349;struct Cyc_Absyn_Vardecl*
_tmp34B;void*_tmp34D;struct Cyc_Absyn_Vardecl*_tmp34F;_LL184: {struct Cyc_Absyn_Var_e_struct*
_tmp340=(struct Cyc_Absyn_Var_e_struct*)_tmp33F;if(_tmp340->tag != 1)goto _LL186;
else{_tmp341=(void*)_tmp340->f2;{struct Cyc_Absyn_Global_b_struct*_tmp342=(struct
Cyc_Absyn_Global_b_struct*)_tmp341;if(_tmp342->tag != 1)goto _LL186;else{_tmp343=
_tmp342->f1;}};}}_LL185: _tmp347=_tmp343;goto _LL187;_LL186: {struct Cyc_Absyn_Var_e_struct*
_tmp344=(struct Cyc_Absyn_Var_e_struct*)_tmp33F;if(_tmp344->tag != 1)goto _LL188;
else{_tmp345=(void*)_tmp344->f2;{struct Cyc_Absyn_Param_b_struct*_tmp346=(struct
Cyc_Absyn_Param_b_struct*)_tmp345;if(_tmp346->tag != 3)goto _LL188;else{_tmp347=
_tmp346->f1;}};}}_LL187: _tmp34B=_tmp347;goto _LL189;_LL188: {struct Cyc_Absyn_Var_e_struct*
_tmp348=(struct Cyc_Absyn_Var_e_struct*)_tmp33F;if(_tmp348->tag != 1)goto _LL18A;
else{_tmp349=(void*)_tmp348->f2;{struct Cyc_Absyn_Local_b_struct*_tmp34A=(struct
Cyc_Absyn_Local_b_struct*)_tmp349;if(_tmp34A->tag != 4)goto _LL18A;else{_tmp34B=
_tmp34A->f1;}};}}_LL189: _tmp34F=_tmp34B;goto _LL18B;_LL18A: {struct Cyc_Absyn_Var_e_struct*
_tmp34C=(struct Cyc_Absyn_Var_e_struct*)_tmp33F;if(_tmp34C->tag != 1)goto _LL18C;
else{_tmp34D=(void*)_tmp34C->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp34E=(struct Cyc_Absyn_Pat_b_struct*)
_tmp34D;if(_tmp34E->tag != 5)goto _LL18C;else{_tmp34F=_tmp34E->f1;}};}}_LL18B: if(!
_tmp34F->escapes)return Cyc_CfFlowInfo_reln_assign_var(rgn,r,_tmp34F,e2);goto
_LL183;_LL18C:;_LL18D: goto _LL183;_LL183:;}return r;}static void Cyc_CfFlowInfo_print_reln(
struct Cyc_CfFlowInfo_Reln*r){({struct Cyc_String_pa_struct _tmp352;_tmp352.tag=0;
_tmp352.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((
r->vd)->name));({void*_tmp350[1]={& _tmp352};Cyc_fprintf(Cyc_stderr,({const char*
_tmp351="%s";_tag_dyneither(_tmp351,sizeof(char),3);}),_tag_dyneither(_tmp350,
sizeof(void*),1));});});{union Cyc_CfFlowInfo_RelnOp _tmp353=r->rop;unsigned int
_tmp354;struct _tuple11 _tmp355;struct Cyc_Absyn_Vardecl*_tmp356;struct Cyc_Absyn_Vardecl*
_tmp357;unsigned int _tmp358;struct Cyc_Absyn_Vardecl*_tmp359;_LL18F: if((_tmp353.EqualConst).tag
!= 1)goto _LL191;_tmp354=(unsigned int)(_tmp353.EqualConst).val;_LL190:({struct
Cyc_Int_pa_struct _tmp35C;_tmp35C.tag=1;_tmp35C.f1=(unsigned long)((int)_tmp354);({
void*_tmp35A[1]={& _tmp35C};Cyc_fprintf(Cyc_stderr,({const char*_tmp35B="==%d";
_tag_dyneither(_tmp35B,sizeof(char),5);}),_tag_dyneither(_tmp35A,sizeof(void*),1));});});
goto _LL18E;_LL191: if((_tmp353.LessVar).tag != 2)goto _LL193;_tmp355=(struct
_tuple11)(_tmp353.LessVar).val;_tmp356=_tmp355.f1;_LL192:({struct Cyc_String_pa_struct
_tmp35F;_tmp35F.tag=0;_tmp35F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp356->name));({void*_tmp35D[1]={& _tmp35F};Cyc_fprintf(
Cyc_stderr,({const char*_tmp35E="<%s";_tag_dyneither(_tmp35E,sizeof(char),4);}),
_tag_dyneither(_tmp35D,sizeof(void*),1));});});goto _LL18E;_LL193: if((_tmp353.LessNumelts).tag
!= 3)goto _LL195;_tmp357=(struct Cyc_Absyn_Vardecl*)(_tmp353.LessNumelts).val;
_LL194:({struct Cyc_String_pa_struct _tmp362;_tmp362.tag=0;_tmp362.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp357->name));({
void*_tmp360[1]={& _tmp362};Cyc_fprintf(Cyc_stderr,({const char*_tmp361="<numelts(%s)";
_tag_dyneither(_tmp361,sizeof(char),13);}),_tag_dyneither(_tmp360,sizeof(void*),
1));});});goto _LL18E;_LL195: if((_tmp353.LessConst).tag != 4)goto _LL197;_tmp358=(
unsigned int)(_tmp353.LessConst).val;_LL196:({struct Cyc_Int_pa_struct _tmp365;
_tmp365.tag=1;_tmp365.f1=(unsigned long)((int)_tmp358);({void*_tmp363[1]={&
_tmp365};Cyc_fprintf(Cyc_stderr,({const char*_tmp364="<%d";_tag_dyneither(_tmp364,
sizeof(char),4);}),_tag_dyneither(_tmp363,sizeof(void*),1));});});goto _LL18E;
_LL197: if((_tmp353.LessEqNumelts).tag != 5)goto _LL18E;_tmp359=(struct Cyc_Absyn_Vardecl*)(
_tmp353.LessEqNumelts).val;_LL198:({struct Cyc_String_pa_struct _tmp368;_tmp368.tag=
0;_tmp368.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp359->name));({void*_tmp366[1]={& _tmp368};Cyc_fprintf(Cyc_stderr,({const char*
_tmp367="<=numelts(%s)";_tag_dyneither(_tmp367,sizeof(char),14);}),
_tag_dyneither(_tmp366,sizeof(void*),1));});});goto _LL18E;_LL18E:;};}void Cyc_CfFlowInfo_print_relns(
struct Cyc_List_List*r){for(0;r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)
r->hd);if(r->tl != 0)({void*_tmp369=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp36A=",";
_tag_dyneither(_tmp36A,sizeof(char),2);}),_tag_dyneither(_tmp369,sizeof(void*),0));});}}
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){switch(il){
case Cyc_CfFlowInfo_NoneIL: _LL199:({void*_tmp36B=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp36C="uninitialized";_tag_dyneither(_tmp36C,sizeof(char),14);}),
_tag_dyneither(_tmp36B,sizeof(void*),0));});break;case Cyc_CfFlowInfo_ThisIL:
_LL19A:({void*_tmp36D=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp36E="this-initialized";
_tag_dyneither(_tmp36E,sizeof(char),17);}),_tag_dyneither(_tmp36D,sizeof(void*),
0));});break;case Cyc_CfFlowInfo_AllIL: _LL19B:({void*_tmp36F=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp370="all-initialized";_tag_dyneither(_tmp370,sizeof(char),16);}),
_tag_dyneither(_tmp36F,sizeof(void*),0));});break;}}void Cyc_CfFlowInfo_print_root(
void*root){void*_tmp371=root;struct Cyc_Absyn_Vardecl*_tmp373;struct Cyc_Absyn_Exp*
_tmp375;void*_tmp376;_LL19E: {struct Cyc_CfFlowInfo_VarRoot_struct*_tmp372=(
struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp371;if(_tmp372->tag != 0)goto _LL1A0;
else{_tmp373=_tmp372->f1;}}_LL19F:({struct Cyc_String_pa_struct _tmp37A;_tmp37A.tag=
0;_tmp37A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp373->name));({void*_tmp378[1]={& _tmp37A};Cyc_fprintf(Cyc_stderr,({const char*
_tmp379="Root(%s)";_tag_dyneither(_tmp379,sizeof(char),9);}),_tag_dyneither(
_tmp378,sizeof(void*),1));});});goto _LL19D;_LL1A0: {struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp374=(struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp371;if(_tmp374->tag != 1)goto
_LL1A2;else{_tmp375=_tmp374->f1;_tmp376=(void*)_tmp374->f2;}}_LL1A1:({struct Cyc_String_pa_struct
_tmp37E;_tmp37E.tag=0;_tmp37E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp376));({struct Cyc_String_pa_struct _tmp37D;_tmp37D.tag=
0;_tmp37D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp375));({void*_tmp37B[2]={& _tmp37D,& _tmp37E};Cyc_fprintf(Cyc_stderr,({const
char*_tmp37C="MallocPt(%s,%s)";_tag_dyneither(_tmp37C,sizeof(char),16);}),
_tag_dyneither(_tmp37B,sizeof(void*),2));});});});goto _LL19D;_LL1A2: {struct Cyc_CfFlowInfo_InitParam_struct*
_tmp377=(struct Cyc_CfFlowInfo_InitParam_struct*)_tmp371;if(_tmp377->tag != 2)goto
_LL19D;}_LL1A3:({void*_tmp37F=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp380="InitParam(_,_)";
_tag_dyneither(_tmp380,sizeof(char),15);}),_tag_dyneither(_tmp37F,sizeof(void*),
0));});goto _LL19D;_LL19D:;}void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*
p){Cyc_CfFlowInfo_print_root(p->root);{struct Cyc_List_List*_tmp381=p->fields;
while(_tmp381 != 0){({struct Cyc_Int_pa_struct _tmp384;_tmp384.tag=1;_tmp384.f1=(
unsigned long)((int)_tmp381->hd);({void*_tmp382[1]={& _tmp384};Cyc_fprintf(Cyc_stderr,({
const char*_tmp383=".%d";_tag_dyneither(_tmp383,sizeof(char),4);}),_tag_dyneither(
_tmp382,sizeof(void*),1));});});_tmp381=_tmp381->tl;}({void*_tmp385=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp386=" ";_tag_dyneither(_tmp386,sizeof(char),2);}),
_tag_dyneither(_tmp385,sizeof(void*),0));});};}void Cyc_CfFlowInfo_print_list(
struct Cyc_List_List*x,void(*pr)(void*)){int _tmp387=1;while(x != 0){if(!_tmp387){({
void*_tmp388=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp389=", ";_tag_dyneither(
_tmp389,sizeof(char),3);}),_tag_dyneither(_tmp388,sizeof(void*),0));});_tmp387=0;}
pr((void*)x->hd);x=x->tl;}({void*_tmp38A=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp38B="\n";_tag_dyneither(_tmp38B,sizeof(char),2);}),_tag_dyneither(_tmp38A,
sizeof(void*),0));});}void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,
void(*pr)(void*)){struct _RegionHandle _tmp38C=_new_region("r");struct
_RegionHandle*r=& _tmp38C;_push_region(r);{struct _handler_cons _tmp38D;
_push_handler(& _tmp38D);{int _tmp38F=0;if(setjmp(_tmp38D.handler))_tmp38F=1;if(!
_tmp38F){{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);struct _tuple18 _tmp390=*((
struct _tuple18*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,p);int first=1;({void*_tmp391=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp392="{ ";
_tag_dyneither(_tmp392,sizeof(char),3);}),_tag_dyneither(_tmp391,sizeof(void*),0));});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& _tmp390)){
void*_tmp393=_tmp390.f1;if(!first){({void*_tmp394=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp395=", ";_tag_dyneither(_tmp395,sizeof(char),3);}),_tag_dyneither(
_tmp394,sizeof(void*),0));});first=0;}pr(_tmp393);}({void*_tmp396=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp397="}\n";_tag_dyneither(_tmp397,sizeof(char),3);}),
_tag_dyneither(_tmp396,sizeof(void*),0));});};_pop_handler();}else{void*_tmp38E=(
void*)_exn_thrown;void*_tmp399=_tmp38E;_LL1A5:;_LL1A6:({void*_tmp39A=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp39B="{ }\n";_tag_dyneither(_tmp39B,sizeof(char),5);}),
_tag_dyneither(_tmp39A,sizeof(void*),0));});_npop_handler(0);return;_LL1A7:;
_LL1A8:(void)_throw(_tmp399);_LL1A4:;}};};_pop_region(r);}void Cyc_CfFlowInfo_print_absrval(
void*rval){void*_tmp39C=rval;enum Cyc_CfFlowInfo_InitLevel _tmp3A1;enum Cyc_CfFlowInfo_InitLevel
_tmp3A3;struct Cyc_CfFlowInfo_Place*_tmp3A5;int _tmp3A8;struct _dyneither_ptr
_tmp3A9;_LL1AA: {struct Cyc_CfFlowInfo_Zero_struct*_tmp39D=(struct Cyc_CfFlowInfo_Zero_struct*)
_tmp39C;if(_tmp39D->tag != 0)goto _LL1AC;}_LL1AB:({void*_tmp3AA=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp3AB="Zero";_tag_dyneither(_tmp3AB,sizeof(char),5);}),
_tag_dyneither(_tmp3AA,sizeof(void*),0));});goto _LL1A9;_LL1AC: {struct Cyc_CfFlowInfo_NotZeroAll_struct*
_tmp39E=(struct Cyc_CfFlowInfo_NotZeroAll_struct*)_tmp39C;if(_tmp39E->tag != 1)
goto _LL1AE;}_LL1AD:({void*_tmp3AC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3AD="NotZeroAll";
_tag_dyneither(_tmp3AD,sizeof(char),11);}),_tag_dyneither(_tmp3AC,sizeof(void*),
0));});goto _LL1A9;_LL1AE: {struct Cyc_CfFlowInfo_NotZeroThis_struct*_tmp39F=(
struct Cyc_CfFlowInfo_NotZeroThis_struct*)_tmp39C;if(_tmp39F->tag != 2)goto _LL1B0;}
_LL1AF:({void*_tmp3AE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3AF="NotZeroThis";
_tag_dyneither(_tmp3AF,sizeof(char),12);}),_tag_dyneither(_tmp3AE,sizeof(void*),
0));});goto _LL1A9;_LL1B0: {struct Cyc_CfFlowInfo_UnknownR_struct*_tmp3A0=(struct
Cyc_CfFlowInfo_UnknownR_struct*)_tmp39C;if(_tmp3A0->tag != 3)goto _LL1B2;else{
_tmp3A1=_tmp3A0->f1;}}_LL1B1:({void*_tmp3B0=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp3B1="Unknown(";_tag_dyneither(_tmp3B1,sizeof(char),9);}),_tag_dyneither(
_tmp3B0,sizeof(void*),0));});Cyc_CfFlowInfo_print_initlevel(_tmp3A1);({void*
_tmp3B2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3B3=")";_tag_dyneither(_tmp3B3,
sizeof(char),2);}),_tag_dyneither(_tmp3B2,sizeof(void*),0));});goto _LL1A9;_LL1B2: {
struct Cyc_CfFlowInfo_Esc_struct*_tmp3A2=(struct Cyc_CfFlowInfo_Esc_struct*)
_tmp39C;if(_tmp3A2->tag != 4)goto _LL1B4;else{_tmp3A3=_tmp3A2->f1;}}_LL1B3:({void*
_tmp3B4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3B5="Esc(";_tag_dyneither(
_tmp3B5,sizeof(char),5);}),_tag_dyneither(_tmp3B4,sizeof(void*),0));});Cyc_CfFlowInfo_print_initlevel(
_tmp3A3);({void*_tmp3B6=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3B7=")";
_tag_dyneither(_tmp3B7,sizeof(char),2);}),_tag_dyneither(_tmp3B6,sizeof(void*),0));});
goto _LL1A9;_LL1B4: {struct Cyc_CfFlowInfo_AddressOf_struct*_tmp3A4=(struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp39C;if(_tmp3A4->tag != 5)goto _LL1B6;else{_tmp3A5=_tmp3A4->f1;}}_LL1B5:({void*
_tmp3B8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3B9="AddrOf(";_tag_dyneither(
_tmp3B9,sizeof(char),8);}),_tag_dyneither(_tmp3B8,sizeof(void*),0));});Cyc_CfFlowInfo_print_place(
_tmp3A5);({void*_tmp3BA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3BB=")";
_tag_dyneither(_tmp3BB,sizeof(char),2);}),_tag_dyneither(_tmp3BA,sizeof(void*),0));});
goto _LL1A9;_LL1B6: {struct Cyc_CfFlowInfo_TagCmps_struct*_tmp3A6=(struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp39C;if(_tmp3A6->tag != 6)goto _LL1B8;}_LL1B7:({void*_tmp3BC=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp3BD="TagCmps(?)";_tag_dyneither(_tmp3BD,sizeof(char),11);}),
_tag_dyneither(_tmp3BC,sizeof(void*),0));});goto _LL1A9;_LL1B8: {struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp3A7=(struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp39C;if(_tmp3A7->tag != 7)goto
_LL1A9;else{_tmp3A8=_tmp3A7->f1;_tmp3A9=_tmp3A7->f2;}}_LL1B9: if(_tmp3A8)({void*
_tmp3BE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3BF="AggrUnion{";_tag_dyneither(
_tmp3BF,sizeof(char),11);}),_tag_dyneither(_tmp3BE,sizeof(void*),0));});else{({
void*_tmp3C0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3C1="AggrStruct{";
_tag_dyneither(_tmp3C1,sizeof(char),12);}),_tag_dyneither(_tmp3C0,sizeof(void*),
0));});}{int i=0;for(0;i < _get_dyneither_size(_tmp3A9,sizeof(void*));++ i){Cyc_CfFlowInfo_print_absrval(((
void**)_tmp3A9.curr)[i]);if(i + 1 < _get_dyneither_size(_tmp3A9,sizeof(void*)))({
void*_tmp3C2=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3C3=",";_tag_dyneither(
_tmp3C3,sizeof(char),2);}),_tag_dyneither(_tmp3C2,sizeof(void*),0));});}}({void*
_tmp3C4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3C5="}";_tag_dyneither(_tmp3C5,
sizeof(char),2);}),_tag_dyneither(_tmp3C4,sizeof(void*),0));});goto _LL1A9;_LL1A9:;}
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){({void*_tmp3C6=
0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3C7="    ";_tag_dyneither(_tmp3C7,
sizeof(char),5);}),_tag_dyneither(_tmp3C6,sizeof(void*),0));});Cyc_CfFlowInfo_print_root(
root);({void*_tmp3C8=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3C9=" --> ";
_tag_dyneither(_tmp3C9,sizeof(char),6);}),_tag_dyneither(_tmp3C8,sizeof(void*),0));});
Cyc_CfFlowInfo_print_absrval(rval);({void*_tmp3CA=0;Cyc_fprintf(Cyc_stderr,({
const char*_tmp3CB="\n";_tag_dyneither(_tmp3CB,sizeof(char),2);}),_tag_dyneither(
_tmp3CA,sizeof(void*),0));});}void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict
d){((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,
d);}void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo
_tmp3CC=f;int _tmp3CD;struct _tuple12 _tmp3CE;struct Cyc_Dict_Dict _tmp3CF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3D0;_LL1BB: if((_tmp3CC.BottomFL).tag != 1)goto _LL1BD;_tmp3CD=(int)(_tmp3CC.BottomFL).val;
_LL1BC:({void*_tmp3D1=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3D2="  BottomFL\n";
_tag_dyneither(_tmp3D2,sizeof(char),12);}),_tag_dyneither(_tmp3D1,sizeof(void*),
0));});goto _LL1BA;_LL1BD: if((_tmp3CC.ReachableFL).tag != 2)goto _LL1BA;_tmp3CE=(
struct _tuple12)(_tmp3CC.ReachableFL).val;_tmp3CF=_tmp3CE.f1;_tmp3D0=_tmp3CE.f3;
_LL1BE:({void*_tmp3D3=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3D4="  ReachableFL:\n";
_tag_dyneither(_tmp3D4,sizeof(char),16);}),_tag_dyneither(_tmp3D3,sizeof(void*),
0));});((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,
_tmp3CF);({void*_tmp3D5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3D6="\n    consumed: ";
_tag_dyneither(_tmp3D6,sizeof(char),16);}),_tag_dyneither(_tmp3D5,sizeof(void*),
0));});((void(*)(struct Cyc_Dict_Dict p,void(*pr)(struct Cyc_CfFlowInfo_Place*)))
Cyc_CfFlowInfo_print_dict_set)(_tmp3D0.consumed,Cyc_CfFlowInfo_print_place);({
void*_tmp3D7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3D8="\n    may_consume: ";
_tag_dyneither(_tmp3D8,sizeof(char),19);}),_tag_dyneither(_tmp3D7,sizeof(void*),
0));});((void(*)(struct Cyc_List_List*x,void(*pr)(struct Cyc_CfFlowInfo_Place*)))
Cyc_CfFlowInfo_print_list)(_tmp3D0.may_consume,Cyc_CfFlowInfo_print_place);({
void*_tmp3D9=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp3DA="\n";_tag_dyneither(
_tmp3DA,sizeof(char),2);}),_tag_dyneither(_tmp3D9,sizeof(void*),0));});goto
_LL1BA;_LL1BA:;}int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){void*_tmp3DB=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3E7;struct Cyc_Absyn_DatatypeInfo
_tmp3E9;struct Cyc_List_List*_tmp3EA;struct Cyc_List_List*_tmp3EC;struct Cyc_Absyn_AggrInfo
_tmp3EE;struct Cyc_List_List*_tmp3EF;struct Cyc_Absyn_DatatypeFieldInfo _tmp3F1;
struct Cyc_List_List*_tmp3F2;struct Cyc_List_List*_tmp3F4;struct Cyc_Absyn_PtrInfo
_tmp3F6;void*_tmp3F7;struct Cyc_Absyn_PtrAtts _tmp3F8;void*_tmp3F9;struct Cyc_List_List*
_tmp3FC;struct Cyc_List_List*_tmp3FE;struct Cyc_Absyn_ArrayInfo _tmp400;void*
_tmp401;void*_tmp403;void*_tmp405;void*_tmp407;void*_tmp408;void*_tmp40A;_LL1C0: {
struct Cyc_Absyn_VoidType_struct*_tmp3DC=(struct Cyc_Absyn_VoidType_struct*)
_tmp3DB;if(_tmp3DC->tag != 0)goto _LL1C2;}_LL1C1: goto _LL1C3;_LL1C2: {struct Cyc_Absyn_IntType_struct*
_tmp3DD=(struct Cyc_Absyn_IntType_struct*)_tmp3DB;if(_tmp3DD->tag != 6)goto _LL1C4;}
_LL1C3: goto _LL1C5;_LL1C4: {struct Cyc_Absyn_FloatType_struct*_tmp3DE=(struct Cyc_Absyn_FloatType_struct*)
_tmp3DB;if(_tmp3DE->tag != 7)goto _LL1C6;}_LL1C5: goto _LL1C7;_LL1C6: {struct Cyc_Absyn_DoubleType_struct*
_tmp3DF=(struct Cyc_Absyn_DoubleType_struct*)_tmp3DB;if(_tmp3DF->tag != 8)goto
_LL1C8;}_LL1C7: goto _LL1C9;_LL1C8: {struct Cyc_Absyn_EnumType_struct*_tmp3E0=(
struct Cyc_Absyn_EnumType_struct*)_tmp3DB;if(_tmp3E0->tag != 14)goto _LL1CA;}_LL1C9:
goto _LL1CB;_LL1CA: {struct Cyc_Absyn_AnonEnumType_struct*_tmp3E1=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3DB;if(_tmp3E1->tag != 15)goto _LL1CC;}_LL1CB: goto _LL1CD;_LL1CC: {struct Cyc_Absyn_TagType_struct*
_tmp3E2=(struct Cyc_Absyn_TagType_struct*)_tmp3DB;if(_tmp3E2->tag != 20)goto _LL1CE;}
_LL1CD: goto _LL1CF;_LL1CE: {struct Cyc_Absyn_HeapRgn_struct*_tmp3E3=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp3DB;if(_tmp3E3->tag != 21)goto _LL1D0;}_LL1CF: goto _LL1D1;_LL1D0: {struct Cyc_Absyn_Evar_struct*
_tmp3E4=(struct Cyc_Absyn_Evar_struct*)_tmp3DB;if(_tmp3E4->tag != 1)goto _LL1D2;}
_LL1D1: return 0;_LL1D2: {struct Cyc_Absyn_UniqueRgn_struct*_tmp3E5=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp3DB;if(_tmp3E5->tag != 22)goto _LL1D4;}_LL1D3: {void*_tmp40C=rgn;_LL1F5: {
struct Cyc_CfFlowInfo_UniqueRgn_k_struct*_tmp40D=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)
_tmp40C;if(_tmp40D->tag != 0)goto _LL1F7;}_LL1F6: return 1;_LL1F7:;_LL1F8: return 0;
_LL1F4:;}_LL1D4: {struct Cyc_Absyn_VarType_struct*_tmp3E6=(struct Cyc_Absyn_VarType_struct*)
_tmp3DB;if(_tmp3E6->tag != 2)goto _LL1D6;else{_tmp3E7=_tmp3E6->f1;}}_LL1D5: {void*
_tmp40E=rgn;struct Cyc_Absyn_Tvar*_tmp411;_LL1FA: {struct Cyc_CfFlowInfo_UniqueRgn_k_struct*
_tmp40F=(struct Cyc_CfFlowInfo_UniqueRgn_k_struct*)_tmp40E;if(_tmp40F->tag != 0)
goto _LL1FC;}_LL1FB: return 0;_LL1FC: {struct Cyc_CfFlowInfo_Region_k_struct*_tmp410=(
struct Cyc_CfFlowInfo_Region_k_struct*)_tmp40E;if(_tmp410->tag != 1)goto _LL1F9;
else{_tmp411=_tmp410->f1;}}_LL1FD: return Cyc_Absyn_tvar_cmp(_tmp3E7,_tmp411)== 0;
_LL1F9:;}_LL1D6: {struct Cyc_Absyn_DatatypeType_struct*_tmp3E8=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp3DB;if(_tmp3E8->tag != 3)goto _LL1D8;else{_tmp3E9=_tmp3E8->f1;_tmp3EA=_tmp3E9.targs;}}
_LL1D7: _tmp3EC=_tmp3EA;goto _LL1D9;_LL1D8: {struct Cyc_Absyn_TypedefType_struct*
_tmp3EB=(struct Cyc_Absyn_TypedefType_struct*)_tmp3DB;if(_tmp3EB->tag != 18)goto
_LL1DA;else{_tmp3EC=_tmp3EB->f2;}}_LL1D9: _tmp3EF=_tmp3EC;goto _LL1DB;_LL1DA: {
struct Cyc_Absyn_AggrType_struct*_tmp3ED=(struct Cyc_Absyn_AggrType_struct*)
_tmp3DB;if(_tmp3ED->tag != 12)goto _LL1DC;else{_tmp3EE=_tmp3ED->f1;_tmp3EF=_tmp3EE.targs;}}
_LL1DB: _tmp3F2=_tmp3EF;goto _LL1DD;_LL1DC: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp3F0=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp3DB;if(_tmp3F0->tag != 4)
goto _LL1DE;else{_tmp3F1=_tmp3F0->f1;_tmp3F2=_tmp3F1.targs;}}_LL1DD: _tmp3F4=
_tmp3F2;goto _LL1DF;_LL1DE: {struct Cyc_Absyn_JoinEff_struct*_tmp3F3=(struct Cyc_Absyn_JoinEff_struct*)
_tmp3DB;if(_tmp3F3->tag != 24)goto _LL1E0;else{_tmp3F4=_tmp3F3->f1;}}_LL1DF: return((
int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(
Cyc_CfFlowInfo_contains_region,rgn,_tmp3F4);_LL1E0: {struct Cyc_Absyn_PointerType_struct*
_tmp3F5=(struct Cyc_Absyn_PointerType_struct*)_tmp3DB;if(_tmp3F5->tag != 5)goto
_LL1E2;else{_tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F6.elt_typ;_tmp3F8=_tmp3F6.ptr_atts;
_tmp3F9=_tmp3F8.rgn;}}_LL1E1: return Cyc_CfFlowInfo_contains_region(rgn,_tmp3F9)
 || Cyc_CfFlowInfo_contains_region(rgn,_tmp3F7);_LL1E2: {struct Cyc_Absyn_FnType_struct*
_tmp3FA=(struct Cyc_Absyn_FnType_struct*)_tmp3DB;if(_tmp3FA->tag != 10)goto _LL1E4;}
_LL1E3: return 0;_LL1E4: {struct Cyc_Absyn_TupleType_struct*_tmp3FB=(struct Cyc_Absyn_TupleType_struct*)
_tmp3DB;if(_tmp3FB->tag != 11)goto _LL1E6;else{_tmp3FC=_tmp3FB->f1;}}_LL1E5: for(0;
_tmp3FC != 0;_tmp3FC=_tmp3FC->tl){if(Cyc_CfFlowInfo_contains_region(rgn,(*((
struct _tuple19*)_tmp3FC->hd)).f2))return 1;}return 0;_LL1E6: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp3FD=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp3DB;if(_tmp3FD->tag != 13)goto
_LL1E8;else{_tmp3FE=_tmp3FD->f2;}}_LL1E7: for(0;_tmp3FE != 0;_tmp3FE=_tmp3FE->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp3FE->hd)->type))
return 1;}return 0;_LL1E8: {struct Cyc_Absyn_ArrayType_struct*_tmp3FF=(struct Cyc_Absyn_ArrayType_struct*)
_tmp3DB;if(_tmp3FF->tag != 9)goto _LL1EA;else{_tmp400=_tmp3FF->f1;_tmp401=_tmp400.elt_type;}}
_LL1E9: _tmp403=_tmp401;goto _LL1EB;_LL1EA: {struct Cyc_Absyn_AccessEff_struct*
_tmp402=(struct Cyc_Absyn_AccessEff_struct*)_tmp3DB;if(_tmp402->tag != 23)goto
_LL1EC;else{_tmp403=(void*)_tmp402->f1;}}_LL1EB: _tmp405=_tmp403;goto _LL1ED;
_LL1EC: {struct Cyc_Absyn_RgnsEff_struct*_tmp404=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp3DB;if(_tmp404->tag != 25)goto _LL1EE;else{_tmp405=(void*)_tmp404->f1;}}_LL1ED:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp405);_LL1EE: {struct Cyc_Absyn_DynRgnType_struct*
_tmp406=(struct Cyc_Absyn_DynRgnType_struct*)_tmp3DB;if(_tmp406->tag != 17)goto
_LL1F0;else{_tmp407=(void*)_tmp406->f1;_tmp408=(void*)_tmp406->f2;}}_LL1EF:
return Cyc_CfFlowInfo_contains_region(rgn,_tmp407) || Cyc_CfFlowInfo_contains_region(
rgn,_tmp408);_LL1F0: {struct Cyc_Absyn_RgnHandleType_struct*_tmp409=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3DB;if(_tmp409->tag != 16)goto _LL1F2;else{_tmp40A=(void*)_tmp409->f1;}}_LL1F1:
return 0;_LL1F2: {struct Cyc_Absyn_ValueofType_struct*_tmp40B=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3DB;if(_tmp40B->tag != 19)goto _LL1BF;}_LL1F3: return 0;_LL1BF:;}struct _tuple27{
struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};static void Cyc_CfFlowInfo_kill_root(
struct _tuple27*env,void*root,void*rval){struct _tuple27 _tmp413;struct Cyc_CfFlowInfo_FlowEnv*
_tmp414;struct Cyc_Dict_Dict _tmp415;struct Cyc_Dict_Dict*_tmp416;void*_tmp417;
struct _tuple27*_tmp412=env;_tmp413=*_tmp412;_tmp414=_tmp413.f1;_tmp415=_tmp413.f2;
_tmp416=(struct Cyc_Dict_Dict*)&(*_tmp412).f2;_tmp417=_tmp413.f3;{void*_tmp418=
root;struct Cyc_Absyn_Vardecl*_tmp41A;void*_tmp41C;_LL1FF: {struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp419=(struct Cyc_CfFlowInfo_VarRoot_struct*)_tmp418;if(_tmp419->tag != 0)goto
_LL201;else{_tmp41A=_tmp419->f1;}}_LL200: if(Cyc_CfFlowInfo_contains_region(
_tmp417,_tmp41A->type))rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp414,_tmp41A->type,
_tmp414->unknown_none);*_tmp416=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
void*k,void*v))Cyc_Dict_insert)(*_tmp416,root,rval);goto _LL1FE;_LL201: {struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp41B=(struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp418;if(_tmp41B->tag != 1)goto _LL203;else{_tmp41C=(void*)_tmp41B->f2;}}_LL202:
if(!Cyc_CfFlowInfo_contains_region(_tmp417,_tmp41C))*_tmp416=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp416,root,rval);goto
_LL1FE;_LL203: {struct Cyc_CfFlowInfo_InitParam_struct*_tmp41D=(struct Cyc_CfFlowInfo_InitParam_struct*)
_tmp418;if(_tmp41D->tag != 2)goto _LL1FE;}_LL204: goto _LL1FE;_LL1FE:;};}static
struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict fd,void*rgn){void*_tmp41E=Cyc_Tcutil_compress(rgn);
struct Cyc_Absyn_Tvar*_tmp420;_LL206: {struct Cyc_Absyn_VarType_struct*_tmp41F=(
struct Cyc_Absyn_VarType_struct*)_tmp41E;if(_tmp41F->tag != 2)goto _LL208;else{
_tmp420=_tmp41F->f1;}}_LL207: {struct Cyc_CfFlowInfo_Region_k_struct v=({struct Cyc_CfFlowInfo_Region_k_struct
_tmp423;_tmp423.tag=1;_tmp423.f1=_tmp420;_tmp423;});void*v2=(void*)& v;struct
_tuple27 env=({struct _tuple27 _tmp422;_tmp422.f1=fenv;_tmp422.f2=fenv->mt_flowdict;
_tmp422.f3=v2;_tmp422;});((void(*)(void(*f)(struct _tuple27*,void*,void*),struct
_tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,&
env,fd);return env.f2;}_LL208: {struct Cyc_Absyn_UniqueRgn_struct*_tmp421=(struct
Cyc_Absyn_UniqueRgn_struct*)_tmp41E;if(_tmp421->tag != 22)goto _LL20A;}_LL209: {
struct _tuple27 env=({struct _tuple27 _tmp424;_tmp424.f1=fenv;_tmp424.f2=fenv->mt_flowdict;
_tmp424.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val;_tmp424;});((void(*)(void(*f)(
struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(
Cyc_CfFlowInfo_kill_root,& env,fd);return env.f2;}_LL20A:;_LL20B:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425[0]=({
struct Cyc_Core_Impossible_struct _tmp426;_tmp426.tag=Cyc_Core_Impossible;_tmp426.f1=({
const char*_tmp427="kill_flowdict_region";_tag_dyneither(_tmp427,sizeof(char),21);});
_tmp426;});_tmp425;}));_LL205:;}static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(
struct Cyc_List_List*relns,void*rgn){return 0;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){union
Cyc_CfFlowInfo_FlowInfo _tmp428=f;int _tmp429;struct _tuple12 _tmp42A;struct Cyc_Dict_Dict
_tmp42B;struct Cyc_List_List*_tmp42C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp42D;
_LL20D: if((_tmp428.BottomFL).tag != 1)goto _LL20F;_tmp429=(int)(_tmp428.BottomFL).val;
_LL20E: return f;_LL20F: if((_tmp428.ReachableFL).tag != 2)goto _LL20C;_tmp42A=(
struct _tuple12)(_tmp428.ReachableFL).val;_tmp42B=_tmp42A.f1;_tmp42C=_tmp42A.f2;
_tmp42D=_tmp42A.f3;_LL210: {struct Cyc_Dict_Dict _tmp42E=Cyc_CfFlowInfo_kill_flowdict_region(
fenv,_tmp42B,rgn);struct Cyc_List_List*_tmp42F=Cyc_CfFlowInfo_kill_relns_region(
_tmp42C,rgn);return Cyc_CfFlowInfo_ReachableFL(_tmp42E,_tmp42F,_tmp42D);}_LL20C:;}
static struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of_rec(struct
_RegionHandle*r,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds,struct Cyc_List_List*
path){void*_tmp430=e->r;struct Cyc_Absyn_Exp*_tmp432;struct Cyc_Absyn_Exp*_tmp434;
struct Cyc_Absyn_Exp*_tmp436;void*_tmp438;struct Cyc_Absyn_Vardecl*_tmp43A;void*
_tmp43C;struct Cyc_Absyn_Vardecl*_tmp43E;void*_tmp440;struct Cyc_Absyn_Vardecl*
_tmp442;void*_tmp444;struct Cyc_Absyn_Vardecl*_tmp446;struct Cyc_Absyn_Exp*_tmp448;
struct _dyneither_ptr*_tmp449;struct Cyc_Absyn_Exp*_tmp44B;struct Cyc_Absyn_Exp*
_tmp44D;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp450;struct
_dyneither_ptr*_tmp451;_LL212: {struct Cyc_Absyn_Cast_e_struct*_tmp431=(struct Cyc_Absyn_Cast_e_struct*)
_tmp430;if(_tmp431->tag != 15)goto _LL214;else{_tmp432=_tmp431->f2;}}_LL213:
_tmp434=_tmp432;goto _LL215;_LL214: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp433=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp430;if(_tmp433->tag != 13)
goto _LL216;else{_tmp434=_tmp433->f1;}}_LL215: _tmp436=_tmp434;goto _LL217;_LL216: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp435=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp430;if(_tmp435->tag != 14)goto _LL218;else{_tmp436=_tmp435->f1;}}_LL217: return
Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp436,flds,path);_LL218: {struct Cyc_Absyn_Var_e_struct*
_tmp437=(struct Cyc_Absyn_Var_e_struct*)_tmp430;if(_tmp437->tag != 1)goto _LL21A;
else{_tmp438=(void*)_tmp437->f2;{struct Cyc_Absyn_Param_b_struct*_tmp439=(struct
Cyc_Absyn_Param_b_struct*)_tmp438;if(_tmp439->tag != 3)goto _LL21A;else{_tmp43A=
_tmp439->f1;}};}}_LL219: _tmp43E=_tmp43A;goto _LL21B;_LL21A: {struct Cyc_Absyn_Var_e_struct*
_tmp43B=(struct Cyc_Absyn_Var_e_struct*)_tmp430;if(_tmp43B->tag != 1)goto _LL21C;
else{_tmp43C=(void*)_tmp43B->f2;{struct Cyc_Absyn_Local_b_struct*_tmp43D=(struct
Cyc_Absyn_Local_b_struct*)_tmp43C;if(_tmp43D->tag != 4)goto _LL21C;else{_tmp43E=
_tmp43D->f1;}};}}_LL21B: _tmp442=_tmp43E;goto _LL21D;_LL21C: {struct Cyc_Absyn_Var_e_struct*
_tmp43F=(struct Cyc_Absyn_Var_e_struct*)_tmp430;if(_tmp43F->tag != 1)goto _LL21E;
else{_tmp440=(void*)_tmp43F->f2;{struct Cyc_Absyn_Pat_b_struct*_tmp441=(struct Cyc_Absyn_Pat_b_struct*)
_tmp440;if(_tmp441->tag != 5)goto _LL21E;else{_tmp442=_tmp441->f1;}};}}_LL21D:
return({struct Cyc_CfFlowInfo_UniquePlace*_tmp452=_region_malloc(r,sizeof(*
_tmp452));_tmp452->place=({struct Cyc_CfFlowInfo_Place _tmp453;_tmp453.root=(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp454=_region_malloc(r,sizeof(*_tmp454));
_tmp454[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp455;_tmp455.tag=0;_tmp455.f1=
_tmp442;_tmp455;});_tmp454;});_tmp453.fields=flds;_tmp453;});_tmp452->path=path;
_tmp452;});_LL21E: {struct Cyc_Absyn_Var_e_struct*_tmp443=(struct Cyc_Absyn_Var_e_struct*)
_tmp430;if(_tmp443->tag != 1)goto _LL220;else{_tmp444=(void*)_tmp443->f2;{struct
Cyc_Absyn_Global_b_struct*_tmp445=(struct Cyc_Absyn_Global_b_struct*)_tmp444;if(
_tmp445->tag != 1)goto _LL220;else{_tmp446=_tmp445->f1;}};}}_LL21F:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp456=_cycalloc(sizeof(*_tmp456));
_tmp456[0]=({struct Cyc_Core_Impossible_struct _tmp457;_tmp457.tag=Cyc_Core_Impossible;
_tmp457.f1=({const char*_tmp458="non-unique path";_tag_dyneither(_tmp458,sizeof(
char),16);});_tmp457;});_tmp456;}));_LL220: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp447=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp430;if(_tmp447->tag != 23)goto
_LL222;else{_tmp448=_tmp447->f1;_tmp449=_tmp447->f2;}}_LL221:{void*_tmp459=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp448->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp45B;void*_tmp45C;_LL22B: {struct Cyc_Absyn_PointerType_struct*_tmp45A=(struct
Cyc_Absyn_PointerType_struct*)_tmp459;if(_tmp45A->tag != 5)goto _LL22D;else{
_tmp45B=_tmp45A->f1;_tmp45C=_tmp45B.elt_typ;}}_LL22C: if(!Cyc_Absyn_is_union_type(
_tmp45C))flds=({struct Cyc_List_List*_tmp45D=_region_malloc(r,sizeof(*_tmp45D));
_tmp45D->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp45C,_tmp449);_tmp45D->tl=
flds;_tmp45D;});goto _LL22A;_LL22D:;_LL22E:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E[0]=({struct Cyc_Core_Impossible_struct
_tmp45F;_tmp45F.tag=Cyc_Core_Impossible;_tmp45F.f1=({const char*_tmp460="anal_Rexp: AggrArrow ptr";
_tag_dyneither(_tmp460,sizeof(char),25);});_tmp45F;});_tmp45E;}));_LL22A:;}
_tmp44B=_tmp448;goto _LL223;_LL222: {struct Cyc_Absyn_Deref_e_struct*_tmp44A=(
struct Cyc_Absyn_Deref_e_struct*)_tmp430;if(_tmp44A->tag != 21)goto _LL224;else{
_tmp44B=_tmp44A->f1;}}_LL223: return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp44B,
0,({struct Cyc_List_List*_tmp461=_region_malloc(r,sizeof(*_tmp461));_tmp461->hd=
flds;_tmp461->tl=path;_tmp461;}));_LL224: {struct Cyc_Absyn_Subscript_e_struct*
_tmp44C=(struct Cyc_Absyn_Subscript_e_struct*)_tmp430;if(_tmp44C->tag != 24)goto
_LL226;else{_tmp44D=_tmp44C->f1;_tmp44E=_tmp44C->f2;}}_LL225: {void*_tmp462=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp44D->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp465;void*_tmp466;struct Cyc_Absyn_PtrAtts _tmp467;union Cyc_Absyn_Constraint*
_tmp468;_LL230: {struct Cyc_Absyn_TupleType_struct*_tmp463=(struct Cyc_Absyn_TupleType_struct*)
_tmp462;if(_tmp463->tag != 11)goto _LL232;}_LL231: {unsigned int _tmp469=(Cyc_Evexp_eval_const_uint_exp(
_tmp44E)).f1;return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp44D,({struct Cyc_List_List*
_tmp46A=_region_malloc(r,sizeof(*_tmp46A));_tmp46A->hd=(void*)((int)_tmp469);
_tmp46A->tl=flds;_tmp46A;}),path);}_LL232: {struct Cyc_Absyn_PointerType_struct*
_tmp464=(struct Cyc_Absyn_PointerType_struct*)_tmp462;if(_tmp464->tag != 5)goto
_LL234;else{_tmp465=_tmp464->f1;_tmp466=_tmp465.elt_typ;_tmp467=_tmp465.ptr_atts;
_tmp468=_tmp467.bounds;}}_LL233:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B[0]=({struct Cyc_Core_Impossible_struct
_tmp46C;_tmp46C.tag=Cyc_Core_Impossible;_tmp46C.f1=({const char*_tmp46D="subscripts unsupported for unique places";
_tag_dyneither(_tmp46D,sizeof(char),41);});_tmp46C;});_tmp46B;}));_LL234:;_LL235:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp46E=_cycalloc(sizeof(*
_tmp46E));_tmp46E[0]=({struct Cyc_Core_Impossible_struct _tmp46F;_tmp46F.tag=Cyc_Core_Impossible;
_tmp46F.f1=({const char*_tmp470="unique place subscript -- bad type";
_tag_dyneither(_tmp470,sizeof(char),35);});_tmp46F;});_tmp46E;}));_LL22F:;}
_LL226: {struct Cyc_Absyn_AggrMember_e_struct*_tmp44F=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp430;if(_tmp44F->tag != 22)goto _LL228;else{_tmp450=_tmp44F->f1;_tmp451=_tmp44F->f2;}}
_LL227: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp450->topt))->v))(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471[0]=({struct Cyc_Core_Impossible_struct
_tmp472;_tmp472.tag=Cyc_Core_Impossible;_tmp472.f1=({const char*_tmp473="unions can't contain unique places";
_tag_dyneither(_tmp473,sizeof(char),35);});_tmp472;});_tmp471;}));{int _tmp474=
Cyc_CfFlowInfo_get_field_index((void*)((struct Cyc_Core_Opt*)_check_null(_tmp450->topt))->v,
_tmp451);return Cyc_CfFlowInfo_unique_place_of_rec(r,_tmp450,({struct Cyc_List_List*
_tmp475=_region_malloc(r,sizeof(*_tmp475));_tmp475->hd=(void*)_tmp474;_tmp475->tl=
flds;_tmp475;}),path);};_LL228:;_LL229:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476[0]=({struct Cyc_Core_Impossible_struct
_tmp477;_tmp477.tag=Cyc_Core_Impossible;_tmp477.f1=({const char*_tmp478="bad unique path";
_tag_dyneither(_tmp478,sizeof(char),16);});_tmp477;});_tmp476;}));_LL211:;}
struct Cyc_CfFlowInfo_UniquePlace*Cyc_CfFlowInfo_unique_place_of(struct
_RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_CfFlowInfo_unique_place_of_rec(
r,e,0,0);}struct _tuple28{struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict*f2;
struct Cyc_Dict_Dict f3;struct Cyc_Position_Segment*f4;};static void Cyc_CfFlowInfo_consume_f(
struct _tuple28*env,struct Cyc_CfFlowInfo_Place*place){struct _tuple28 _tmp47A;
struct Cyc_CfFlowInfo_FlowEnv*_tmp47B;struct Cyc_Dict_Dict*_tmp47C;struct Cyc_Dict_Dict
_tmp47D;struct Cyc_Position_Segment*_tmp47E;struct _tuple28*_tmp479=env;_tmp47A=*
_tmp479;_tmp47B=_tmp47A.f1;_tmp47C=_tmp47A.f2;_tmp47D=_tmp47A.f3;_tmp47E=_tmp47A.f4;{
void*rval=_tmp47B->zero;if(((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(
_tmp47D,place->root,& rval) && rval == _tmp47B->zero)return;if(((int(*)(struct Cyc_Dict_Dict*
set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_update_place_set)((
struct Cyc_Dict_Dict*)_tmp47C,place,_tmp47E)){struct Cyc_Position_Segment*_tmp47F=((
struct Cyc_Position_Segment*(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))
Cyc_Dict_lookup)(*_tmp47C,place);((void(*)(struct Cyc_CfFlowInfo_Place*place,
struct _dyneither_ptr(*toString)(struct Cyc_CfFlowInfo_Place*),struct
_dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)(place,
Cyc_CfFlowInfo_place_err_string,({const char*_tmp480="May consume non-aliased pointer %s more than once (cf. %s)";
_tag_dyneither(_tmp480,sizeof(char),59);}),({const char*_tmp481="May consume non-aliased pointer %s more than once";
_tag_dyneither(_tmp481,sizeof(char),50);}),_tmp47F,_tmp47E);}};}union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _tmp482=f;int
_tmp483;struct _tuple12 _tmp484;struct Cyc_Dict_Dict _tmp485;struct Cyc_List_List*
_tmp486;struct Cyc_CfFlowInfo_ConsumeInfo _tmp487;_LL237: if((_tmp482.BottomFL).tag
!= 1)goto _LL239;_tmp483=(int)(_tmp482.BottomFL).val;_LL238: return f;_LL239: if((
_tmp482.ReachableFL).tag != 2)goto _LL236;_tmp484=(struct _tuple12)(_tmp482.ReachableFL).val;
_tmp485=_tmp484.f1;_tmp486=_tmp484.f2;_tmp487=_tmp484.f3;_LL23A: if(_tmp487.may_consume
== 0)return f;{struct Cyc_Dict_Dict _tmp488=_tmp487.consumed;struct _tuple28 _tmp489=({
struct _tuple28 _tmp48B;_tmp48B.f1=fenv;_tmp48B.f2=& _tmp488;_tmp48B.f3=_tmp485;
_tmp48B.f4=loc;_tmp48B;});((void(*)(void(*f)(struct _tuple28*,struct Cyc_CfFlowInfo_Place*),
struct _tuple28*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_CfFlowInfo_consume_f,&
_tmp489,_tmp487.may_consume);return Cyc_CfFlowInfo_ReachableFL(_tmp485,_tmp486,({
struct Cyc_CfFlowInfo_ConsumeInfo _tmp48A;_tmp48A.consumed=_tmp488;_tmp48A.may_consume=
0;_tmp48A;}));};_LL236:;}void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*
loc,union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _tmp48C=f;int
_tmp48D;struct _tuple12 _tmp48E;struct Cyc_Dict_Dict _tmp48F;struct Cyc_List_List*
_tmp490;struct Cyc_CfFlowInfo_ConsumeInfo _tmp491;_LL23C: if((_tmp48C.BottomFL).tag
!= 1)goto _LL23E;_tmp48D=(int)(_tmp48C.BottomFL).val;_LL23D: return;_LL23E: if((
_tmp48C.ReachableFL).tag != 2)goto _LL23B;_tmp48E=(struct _tuple12)(_tmp48C.ReachableFL).val;
_tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;_tmp491=_tmp48E.f3;_LL23F: {struct Cyc_List_List*
_tmp492=_tmp491.may_consume;while(_tmp492 != 0){if(((int(*)(struct Cyc_Dict_Dict d,
struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp491.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp492->hd)){struct Cyc_Position_Segment*_tmp493=((struct Cyc_Position_Segment*(*)(
struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_lookup)(_tmp491.consumed,(
struct Cyc_CfFlowInfo_Place*)_tmp492->hd);((void(*)(struct Cyc_CfFlowInfo_Place*
place,struct _dyneither_ptr(*toString)(struct Cyc_CfFlowInfo_Place*),struct
_dyneither_ptr err_msg1,struct _dyneither_ptr err_msg2,struct Cyc_Position_Segment*
consumed_loc,struct Cyc_Position_Segment*loc))Cyc_CfFlowInfo_unique_err)((struct
Cyc_CfFlowInfo_Place*)_tmp492->hd,Cyc_CfFlowInfo_place_err_string,({const char*
_tmp494="Read through possibly consumed unique pointer %s (cf. %s)";
_tag_dyneither(_tmp494,sizeof(char),58);}),({const char*_tmp495="Read through possibly consumed unique pointer %s";
_tag_dyneither(_tmp495,sizeof(char),49);}),_tmp493,loc);break;}_tmp492=_tmp492->tl;}
return;}_LL23B:;}union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_drop_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo
_tmp496=f;int _tmp497;struct _tuple12 _tmp498;struct Cyc_Dict_Dict _tmp499;struct Cyc_List_List*
_tmp49A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp49B;_LL241: if((_tmp496.BottomFL).tag
!= 1)goto _LL243;_tmp497=(int)(_tmp496.BottomFL).val;_LL242: return f;_LL243: if((
_tmp496.ReachableFL).tag != 2)goto _LL240;_tmp498=(struct _tuple12)(_tmp496.ReachableFL).val;
_tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;_tmp49B=_tmp498.f3;_LL244: _tmp49B.may_consume=
0;return Cyc_CfFlowInfo_ReachableFL(_tmp499,_tmp49A,_tmp49B);_LL240:;}union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_readthrough_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo
f){Cyc_CfFlowInfo_check_unique_rvals(loc,f);{union Cyc_CfFlowInfo_FlowInfo _tmp49C=
f;int _tmp49D;struct _tuple12 _tmp49E;struct Cyc_Dict_Dict _tmp49F;struct Cyc_List_List*
_tmp4A0;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4A1;_LL246: if((_tmp49C.BottomFL).tag
!= 1)goto _LL248;_tmp49D=(int)(_tmp49C.BottomFL).val;_LL247: return f;_LL248: if((
_tmp49C.ReachableFL).tag != 2)goto _LL245;_tmp49E=(struct _tuple12)(_tmp49C.ReachableFL).val;
_tmp49F=_tmp49E.f1;_tmp4A0=_tmp49E.f2;_tmp4A1=_tmp49E.f3;_LL249: _tmp4A1.may_consume=
0;return Cyc_CfFlowInfo_ReachableFL(_tmp49F,_tmp4A0,_tmp4A1);_LL245:;};}struct
_tuple14 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*fenv,union
Cyc_CfFlowInfo_FlowInfo f,int clear){union Cyc_CfFlowInfo_FlowInfo _tmp4A2=f;int
_tmp4A3;struct _tuple12 _tmp4A4;struct Cyc_Dict_Dict _tmp4A5;struct Cyc_List_List*
_tmp4A6;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4A7;_LL24B: if((_tmp4A2.BottomFL).tag
!= 1)goto _LL24D;_tmp4A3=(int)(_tmp4A2.BottomFL).val;_LL24C: return({struct
_tuple14 _tmp4A8;_tmp4A8.f1=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp4A9;_tmp4A9.consumed=
fenv->mt_place_set;_tmp4A9.may_consume=0;_tmp4A9;});_tmp4A8.f2=f;_tmp4A8;});
_LL24D: if((_tmp4A2.ReachableFL).tag != 2)goto _LL24A;_tmp4A4=(struct _tuple12)(
_tmp4A2.ReachableFL).val;_tmp4A5=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;_tmp4A7=_tmp4A4.f3;
_LL24E: if(clear)return({struct _tuple14 _tmp4AA;_tmp4AA.f1=_tmp4A7;_tmp4AA.f2=Cyc_CfFlowInfo_ReachableFL(
_tmp4A5,_tmp4A6,({struct Cyc_CfFlowInfo_ConsumeInfo _tmp4AB;_tmp4AB.consumed=fenv->mt_place_set;
_tmp4AB.may_consume=0;_tmp4AB;}));_tmp4AA;});else{return({struct _tuple14 _tmp4AC;
_tmp4AC.f1=_tmp4A7;_tmp4AC.f2=f;_tmp4AC;});}_LL24A:;}union Cyc_CfFlowInfo_FlowInfo
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_CfFlowInfo_ConsumeInfo
c,int may_consume_only){union Cyc_CfFlowInfo_FlowInfo _tmp4AD=f;int _tmp4AE;struct
_tuple12 _tmp4AF;struct Cyc_Dict_Dict _tmp4B0;struct Cyc_List_List*_tmp4B1;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp4B2;_LL250: if((_tmp4AD.BottomFL).tag != 1)goto _LL252;_tmp4AE=(int)(_tmp4AD.BottomFL).val;
_LL251: return f;_LL252: if((_tmp4AD.ReachableFL).tag != 2)goto _LL24F;_tmp4AF=(
struct _tuple12)(_tmp4AD.ReachableFL).val;_tmp4B0=_tmp4AF.f1;_tmp4B1=_tmp4AF.f2;
_tmp4B2=_tmp4AF.f3;_LL253: if(may_consume_only)return Cyc_CfFlowInfo_ReachableFL(
_tmp4B0,_tmp4B1,({struct Cyc_CfFlowInfo_ConsumeInfo _tmp4B3;_tmp4B3.consumed=
_tmp4B2.consumed;_tmp4B3.may_consume=c.may_consume;_tmp4B3;}));return Cyc_CfFlowInfo_ReachableFL(
_tmp4B0,_tmp4B1,c);_LL24F:;}
