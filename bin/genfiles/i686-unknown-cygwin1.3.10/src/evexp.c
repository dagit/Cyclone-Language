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
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};extern char Cyc_List_Nth[4];
struct Cyc_List_Nth_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr
logical_file;struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;struct
Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};struct
Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct _dyneither_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};
extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{char*
tag;};struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
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
_dyneither_ptr f1;};int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*
s2);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
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
_union_Cnst_String_c String_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,
int);enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus
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
char*tag;};struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t,enum Cyc_Absyn_Kind def);void*Cyc_Tcutil_compress(void*t);
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);struct
_tuple9{unsigned int f1;int f2;};struct _tuple9 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);struct _tuple10{
union Cyc_Absyn_Cnst f1;int f2;};static struct _tuple10 Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*);static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst
cn){union Cyc_Absyn_Cnst _tmp0=cn;struct _tuple3 _tmp1;enum Cyc_Absyn_Sign _tmp2;char
_tmp3;struct _tuple4 _tmp4;enum Cyc_Absyn_Sign _tmp5;short _tmp6;_LL1: if((_tmp0.Char_c).tag
!= 2)goto _LL3;_tmp1=(struct _tuple3)(_tmp0.Char_c).val;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;
_LL2: return Cyc_Absyn_Int_c(_tmp2,(int)_tmp3);_LL3: if((_tmp0.Short_c).tag != 3)
goto _LL5;_tmp4=(struct _tuple4)(_tmp0.Short_c).val;_tmp5=_tmp4.f1;_tmp6=_tmp4.f2;
_LL4: return Cyc_Absyn_Int_c(_tmp5,(int)_tmp6);_LL5:;_LL6: return cn;_LL0:;}struct
_tuple9 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){union Cyc_Absyn_Cnst
_tmp8;int _tmp9;struct _tuple10 _tmp7=Cyc_Evexp_eval_const_exp(e);_tmp8=_tmp7.f1;
_tmp9=_tmp7.f2;if(!_tmp9)return({struct _tuple9 _tmpA;_tmpA.f1=0;_tmpA.f2=0;_tmpA;});{
union Cyc_Absyn_Cnst _tmpB=Cyc_Evexp_promote_const(_tmp8);struct _tuple5 _tmpC;int
_tmpD;_LL8: if((_tmpB.Int_c).tag != 4)goto _LLA;_tmpC=(struct _tuple5)(_tmpB.Int_c).val;
_tmpD=_tmpC.f2;_LL9: return({struct _tuple9 _tmpE;_tmpE.f1=(unsigned int)_tmpD;
_tmpE.f2=1;_tmpE;});_LLA:;_LLB:({void*_tmpF=0;Cyc_Tcutil_terr(e->loc,({const char*
_tmp10="expecting unsigned int";_tag_dyneither(_tmp10,sizeof(char),23);}),
_tag_dyneither(_tmpF,sizeof(void*),0));});return({struct _tuple9 _tmp11;_tmp11.f1=
0;_tmp11.f2=1;_tmp11;});_LL7:;};}struct _tuple11{int f1;int f2;};static struct
_tuple11 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){union Cyc_Absyn_Cnst
_tmp13;int _tmp14;struct _tuple10 _tmp12=Cyc_Evexp_eval_const_exp(e);_tmp13=_tmp12.f1;
_tmp14=_tmp12.f2;if(!_tmp14)return({struct _tuple11 _tmp15;_tmp15.f1=0;_tmp15.f2=0;
_tmp15;});{union Cyc_Absyn_Cnst _tmp16=Cyc_Evexp_promote_const(_tmp13);struct
_tuple5 _tmp17;int _tmp18;struct _tuple6 _tmp19;long long _tmp1A;int _tmp1B;_LLD: if((
_tmp16.Int_c).tag != 4)goto _LLF;_tmp17=(struct _tuple5)(_tmp16.Int_c).val;_tmp18=
_tmp17.f2;_LLE: return({struct _tuple11 _tmp1C;_tmp1C.f1=_tmp18 != 0;_tmp1C.f2=1;
_tmp1C;});_LLF: if((_tmp16.LongLong_c).tag != 5)goto _LL11;_tmp19=(struct _tuple6)(
_tmp16.LongLong_c).val;_tmp1A=_tmp19.f2;_LL10: return({struct _tuple11 _tmp1D;
_tmp1D.f1=_tmp1A != 0;_tmp1D.f2=1;_tmp1D;});_LL11: if((_tmp16.Null_c).tag != 1)goto
_LL13;_tmp1B=(int)(_tmp16.Null_c).val;_LL12: return({struct _tuple11 _tmp1E;_tmp1E.f1=
0;_tmp1E.f2=0;_tmp1E;});_LL13:;_LL14:({void*_tmp1F=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp20="expecting bool";_tag_dyneither(_tmp20,sizeof(char),15);}),
_tag_dyneither(_tmp1F,sizeof(void*),0));});return({struct _tuple11 _tmp21;_tmp21.f1=
0;_tmp21.f2=0;_tmp21;});_LLC:;};}struct _tuple12{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst
f2;};static struct _tuple10 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,
struct Cyc_Absyn_Exp*e){union Cyc_Absyn_Cnst _tmp23;int _tmp24;struct _tuple10 _tmp22=
Cyc_Evexp_eval_const_exp(e);_tmp23=_tmp22.f1;_tmp24=_tmp22.f2;if(!_tmp24)return({
struct _tuple10 _tmp25;_tmp25.f1=_tmp23;_tmp25.f2=0;_tmp25;});{struct _tuple12
_tmp27=({struct _tuple12 _tmp26;_tmp26.f1=p;_tmp26.f2=_tmp23;_tmp26;});enum Cyc_Absyn_Primop
_tmp28;enum Cyc_Absyn_Primop _tmp29;union Cyc_Absyn_Cnst _tmp2A;struct _tuple5 _tmp2B;
enum Cyc_Absyn_Sign _tmp2C;int _tmp2D;enum Cyc_Absyn_Primop _tmp2E;union Cyc_Absyn_Cnst
_tmp2F;struct _tuple5 _tmp30;enum Cyc_Absyn_Sign _tmp31;int _tmp32;enum Cyc_Absyn_Primop
_tmp33;union Cyc_Absyn_Cnst _tmp34;struct _tuple5 _tmp35;int _tmp36;enum Cyc_Absyn_Primop
_tmp37;union Cyc_Absyn_Cnst _tmp38;int _tmp39;_LL16: _tmp28=_tmp27.f1;if(_tmp28 != 
Cyc_Absyn_Plus)goto _LL18;_LL17: goto _LL15;_LL18: _tmp29=_tmp27.f1;if(_tmp29 != Cyc_Absyn_Minus)
goto _LL1A;_tmp2A=_tmp27.f2;if((_tmp2A.Int_c).tag != 4)goto _LL1A;_tmp2B=(struct
_tuple5)(_tmp2A.Int_c).val;_tmp2C=_tmp2B.f1;_tmp2D=_tmp2B.f2;_LL19: _tmp23=Cyc_Absyn_Int_c(
Cyc_Absyn_Signed,- _tmp2D);goto _LL15;_LL1A: _tmp2E=_tmp27.f1;if(_tmp2E != Cyc_Absyn_Bitnot)
goto _LL1C;_tmp2F=_tmp27.f2;if((_tmp2F.Int_c).tag != 4)goto _LL1C;_tmp30=(struct
_tuple5)(_tmp2F.Int_c).val;_tmp31=_tmp30.f1;_tmp32=_tmp30.f2;_LL1B: _tmp23=Cyc_Absyn_Int_c(
Cyc_Absyn_Unsigned,~ _tmp32);goto _LL15;_LL1C: _tmp33=_tmp27.f1;if(_tmp33 != Cyc_Absyn_Not)
goto _LL1E;_tmp34=_tmp27.f2;if((_tmp34.Int_c).tag != 4)goto _LL1E;_tmp35=(struct
_tuple5)(_tmp34.Int_c).val;_tmp36=_tmp35.f2;_LL1D: _tmp23=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,
_tmp36 == 0?1: 0);goto _LL15;_LL1E: _tmp37=_tmp27.f1;if(_tmp37 != Cyc_Absyn_Not)goto
_LL20;_tmp38=_tmp27.f2;if((_tmp38.Null_c).tag != 1)goto _LL20;_tmp39=(int)(_tmp38.Null_c).val;
_LL1F: _tmp23=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL15;_LL20:;_LL21:({void*
_tmp3A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp3B="bad constant expression";
_tag_dyneither(_tmp3B,sizeof(char),24);}),_tag_dyneither(_tmp3A,sizeof(void*),0));});
goto _LL15;_LL15:;}return({struct _tuple10 _tmp3C;_tmp3C.f1=_tmp23;_tmp3C.f2=1;
_tmp3C;});}struct _tuple13{enum Cyc_Absyn_Primop f1;int f2;};static struct _tuple10
Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){union Cyc_Absyn_Cnst _tmp3E;int _tmp3F;struct _tuple10 _tmp3D=
Cyc_Evexp_eval_const_exp(e1);_tmp3E=_tmp3D.f1;_tmp3F=_tmp3D.f2;{union Cyc_Absyn_Cnst
_tmp41;int _tmp42;struct _tuple10 _tmp40=Cyc_Evexp_eval_const_exp(e2);_tmp41=_tmp40.f1;
_tmp42=_tmp40.f2;if(!_tmp3F  || !_tmp42)return({struct _tuple10 _tmp43;_tmp43.f1=
_tmp3E;_tmp43.f2=0;_tmp43;});_tmp3E=Cyc_Evexp_promote_const(_tmp3E);_tmp41=Cyc_Evexp_promote_const(
_tmp41);{enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;int i1;int i2;{union Cyc_Absyn_Cnst
_tmp44=_tmp3E;struct _tuple5 _tmp45;enum Cyc_Absyn_Sign _tmp46;int _tmp47;_LL23: if((
_tmp44.Int_c).tag != 4)goto _LL25;_tmp45=(struct _tuple5)(_tmp44.Int_c).val;_tmp46=
_tmp45.f1;_tmp47=_tmp45.f2;_LL24: s1=_tmp46;i1=_tmp47;goto _LL22;_LL25:;_LL26:({
void*_tmp48=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp49="bad constant expression";
_tag_dyneither(_tmp49,sizeof(char),24);}),_tag_dyneither(_tmp48,sizeof(void*),0));});
return({struct _tuple10 _tmp4A;_tmp4A.f1=_tmp3E;_tmp4A.f2=1;_tmp4A;});_LL22:;}{
union Cyc_Absyn_Cnst _tmp4B=_tmp41;struct _tuple5 _tmp4C;enum Cyc_Absyn_Sign _tmp4D;
int _tmp4E;_LL28: if((_tmp4B.Int_c).tag != 4)goto _LL2A;_tmp4C=(struct _tuple5)(
_tmp4B.Int_c).val;_tmp4D=_tmp4C.f1;_tmp4E=_tmp4C.f2;_LL29: s2=_tmp4D;i2=_tmp4E;
goto _LL27;_LL2A:;_LL2B:({void*_tmp4F=0;Cyc_Tcutil_terr(e2->loc,({const char*
_tmp50="bad constant expression";_tag_dyneither(_tmp50,sizeof(char),24);}),
_tag_dyneither(_tmp4F,sizeof(void*),0));});return({struct _tuple10 _tmp51;_tmp51.f1=
_tmp3E;_tmp51.f2=1;_tmp51;});_LL27:;}switch(p){case Cyc_Absyn_Div: _LL2C: goto _LL2D;
case Cyc_Absyn_Mod: _LL2D: if(i2 == 0){({void*_tmp52=0;Cyc_Tcutil_terr(e2->loc,({
const char*_tmp53="division by zero in constant expression";_tag_dyneither(_tmp53,
sizeof(char),40);}),_tag_dyneither(_tmp52,sizeof(void*),0));});return({struct
_tuple10 _tmp54;_tmp54.f1=_tmp3E;_tmp54.f2=1;_tmp54;});}break;default: _LL2E:
break;}{int has_u_arg=s1 == Cyc_Absyn_Unsigned  || s2 == Cyc_Absyn_Unsigned;
unsigned int u1=(unsigned int)i1;unsigned int u2=(unsigned int)i2;int i3=0;
unsigned int u3=0;int b3=1;int use_i3=0;int use_u3=0;int use_b3=0;{struct _tuple13
_tmp56=({struct _tuple13 _tmp55;_tmp55.f1=p;_tmp55.f2=has_u_arg;_tmp55;});enum Cyc_Absyn_Primop
_tmp57;int _tmp58;enum Cyc_Absyn_Primop _tmp59;int _tmp5A;enum Cyc_Absyn_Primop
_tmp5B;int _tmp5C;enum Cyc_Absyn_Primop _tmp5D;int _tmp5E;enum Cyc_Absyn_Primop
_tmp5F;int _tmp60;enum Cyc_Absyn_Primop _tmp61;int _tmp62;enum Cyc_Absyn_Primop
_tmp63;int _tmp64;enum Cyc_Absyn_Primop _tmp65;int _tmp66;enum Cyc_Absyn_Primop
_tmp67;int _tmp68;enum Cyc_Absyn_Primop _tmp69;int _tmp6A;enum Cyc_Absyn_Primop
_tmp6B;enum Cyc_Absyn_Primop _tmp6C;enum Cyc_Absyn_Primop _tmp6D;int _tmp6E;enum 
Cyc_Absyn_Primop _tmp6F;int _tmp70;enum Cyc_Absyn_Primop _tmp71;int _tmp72;enum Cyc_Absyn_Primop
_tmp73;int _tmp74;enum Cyc_Absyn_Primop _tmp75;int _tmp76;enum Cyc_Absyn_Primop
_tmp77;int _tmp78;enum Cyc_Absyn_Primop _tmp79;int _tmp7A;enum Cyc_Absyn_Primop
_tmp7B;int _tmp7C;enum Cyc_Absyn_Primop _tmp7D;enum Cyc_Absyn_Primop _tmp7E;enum 
Cyc_Absyn_Primop _tmp7F;enum Cyc_Absyn_Primop _tmp80;enum Cyc_Absyn_Primop _tmp81;
enum Cyc_Absyn_Primop _tmp82;_LL31: _tmp57=_tmp56.f1;if(_tmp57 != Cyc_Absyn_Plus)
goto _LL33;_tmp58=_tmp56.f2;if(_tmp58 != 0)goto _LL33;_LL32: i3=i1 + i2;use_i3=1;goto
_LL30;_LL33: _tmp59=_tmp56.f1;if(_tmp59 != Cyc_Absyn_Times)goto _LL35;_tmp5A=_tmp56.f2;
if(_tmp5A != 0)goto _LL35;_LL34: i3=i1 * i2;use_i3=1;goto _LL30;_LL35: _tmp5B=_tmp56.f1;
if(_tmp5B != Cyc_Absyn_Minus)goto _LL37;_tmp5C=_tmp56.f2;if(_tmp5C != 0)goto _LL37;
_LL36: i3=i1 - i2;use_i3=1;goto _LL30;_LL37: _tmp5D=_tmp56.f1;if(_tmp5D != Cyc_Absyn_Div)
goto _LL39;_tmp5E=_tmp56.f2;if(_tmp5E != 0)goto _LL39;_LL38: i3=i1 / i2;use_i3=1;goto
_LL30;_LL39: _tmp5F=_tmp56.f1;if(_tmp5F != Cyc_Absyn_Mod)goto _LL3B;_tmp60=_tmp56.f2;
if(_tmp60 != 0)goto _LL3B;_LL3A: i3=i1 % i2;use_i3=1;goto _LL30;_LL3B: _tmp61=_tmp56.f1;
if(_tmp61 != Cyc_Absyn_Plus)goto _LL3D;_tmp62=_tmp56.f2;if(_tmp62 != 1)goto _LL3D;
_LL3C: u3=u1 + u2;use_u3=1;goto _LL30;_LL3D: _tmp63=_tmp56.f1;if(_tmp63 != Cyc_Absyn_Times)
goto _LL3F;_tmp64=_tmp56.f2;if(_tmp64 != 1)goto _LL3F;_LL3E: u3=u1 * u2;use_u3=1;goto
_LL30;_LL3F: _tmp65=_tmp56.f1;if(_tmp65 != Cyc_Absyn_Minus)goto _LL41;_tmp66=_tmp56.f2;
if(_tmp66 != 1)goto _LL41;_LL40: u3=u1 - u2;use_u3=1;goto _LL30;_LL41: _tmp67=_tmp56.f1;
if(_tmp67 != Cyc_Absyn_Div)goto _LL43;_tmp68=_tmp56.f2;if(_tmp68 != 1)goto _LL43;
_LL42: u3=u1 / u2;use_u3=1;goto _LL30;_LL43: _tmp69=_tmp56.f1;if(_tmp69 != Cyc_Absyn_Mod)
goto _LL45;_tmp6A=_tmp56.f2;if(_tmp6A != 1)goto _LL45;_LL44: u3=u1 % u2;use_u3=1;goto
_LL30;_LL45: _tmp6B=_tmp56.f1;if(_tmp6B != Cyc_Absyn_Eq)goto _LL47;_LL46: b3=i1 == i2;
use_b3=1;goto _LL30;_LL47: _tmp6C=_tmp56.f1;if(_tmp6C != Cyc_Absyn_Neq)goto _LL49;
_LL48: b3=i1 != i2;use_b3=1;goto _LL30;_LL49: _tmp6D=_tmp56.f1;if(_tmp6D != Cyc_Absyn_Gt)
goto _LL4B;_tmp6E=_tmp56.f2;if(_tmp6E != 0)goto _LL4B;_LL4A: b3=i1 > i2;use_b3=1;goto
_LL30;_LL4B: _tmp6F=_tmp56.f1;if(_tmp6F != Cyc_Absyn_Lt)goto _LL4D;_tmp70=_tmp56.f2;
if(_tmp70 != 0)goto _LL4D;_LL4C: b3=i1 < i2;use_b3=1;goto _LL30;_LL4D: _tmp71=_tmp56.f1;
if(_tmp71 != Cyc_Absyn_Gte)goto _LL4F;_tmp72=_tmp56.f2;if(_tmp72 != 0)goto _LL4F;
_LL4E: b3=i1 >= i2;use_b3=1;goto _LL30;_LL4F: _tmp73=_tmp56.f1;if(_tmp73 != Cyc_Absyn_Lte)
goto _LL51;_tmp74=_tmp56.f2;if(_tmp74 != 0)goto _LL51;_LL50: b3=i1 <= i2;use_b3=1;
goto _LL30;_LL51: _tmp75=_tmp56.f1;if(_tmp75 != Cyc_Absyn_Gt)goto _LL53;_tmp76=
_tmp56.f2;if(_tmp76 != 1)goto _LL53;_LL52: b3=u1 > u2;use_b3=1;goto _LL30;_LL53:
_tmp77=_tmp56.f1;if(_tmp77 != Cyc_Absyn_Lt)goto _LL55;_tmp78=_tmp56.f2;if(_tmp78 != 
1)goto _LL55;_LL54: b3=u1 < u2;use_b3=1;goto _LL30;_LL55: _tmp79=_tmp56.f1;if(_tmp79
!= Cyc_Absyn_Gte)goto _LL57;_tmp7A=_tmp56.f2;if(_tmp7A != 1)goto _LL57;_LL56: b3=u1
>= u2;use_b3=1;goto _LL30;_LL57: _tmp7B=_tmp56.f1;if(_tmp7B != Cyc_Absyn_Lte)goto
_LL59;_tmp7C=_tmp56.f2;if(_tmp7C != 1)goto _LL59;_LL58: b3=u1 <= u2;use_b3=1;goto
_LL30;_LL59: _tmp7D=_tmp56.f1;if(_tmp7D != Cyc_Absyn_Bitand)goto _LL5B;_LL5A: u3=u1 & 
u2;use_u3=1;goto _LL30;_LL5B: _tmp7E=_tmp56.f1;if(_tmp7E != Cyc_Absyn_Bitor)goto
_LL5D;_LL5C: u3=u1 | u2;use_u3=1;goto _LL30;_LL5D: _tmp7F=_tmp56.f1;if(_tmp7F != Cyc_Absyn_Bitxor)
goto _LL5F;_LL5E: u3=u1 ^ u2;use_u3=1;goto _LL30;_LL5F: _tmp80=_tmp56.f1;if(_tmp80 != 
Cyc_Absyn_Bitlshift)goto _LL61;_LL60: u3=u1 << u2;use_u3=1;goto _LL30;_LL61: _tmp81=
_tmp56.f1;if(_tmp81 != Cyc_Absyn_Bitlrshift)goto _LL63;_LL62: u3=u1 >> u2;use_u3=1;
goto _LL30;_LL63: _tmp82=_tmp56.f1;if(_tmp82 != Cyc_Absyn_Bitarshift)goto _LL65;
_LL64:({void*_tmp83=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp84=">>> NOT IMPLEMENTED";
_tag_dyneither(_tmp84,sizeof(char),20);}),_tag_dyneither(_tmp83,sizeof(void*),0));});
goto _LL66;_LL65:;_LL66:({void*_tmp85=0;Cyc_Tcutil_terr(e1->loc,({const char*
_tmp86="bad constant expression";_tag_dyneither(_tmp86,sizeof(char),24);}),
_tag_dyneither(_tmp85,sizeof(void*),0));});return({struct _tuple10 _tmp87;_tmp87.f1=
_tmp3E;_tmp87.f2=1;_tmp87;});_LL30:;}if(use_i3)return({struct _tuple10 _tmp88;
_tmp88.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3);_tmp88.f2=1;_tmp88;});if(use_u3)
return({struct _tuple10 _tmp89;_tmp89.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)
u3);_tmp89.f2=1;_tmp89;});if(use_b3)return({struct _tuple10 _tmp8A;_tmp8A.f1=Cyc_Absyn_Int_c(
Cyc_Absyn_Signed,b3?1: 0);_tmp8A.f2=1;_tmp8A;});({void*_tmp8B=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp8C="Evexp::eval_const_binop";
_tag_dyneither(_tmp8C,sizeof(char),24);}),_tag_dyneither(_tmp8B,sizeof(void*),0));});};};};}
struct _tuple14{void*f1;union Cyc_Absyn_Cnst f2;};static struct _tuple10 Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*e){struct _tuple10 ans;{void*_tmp8D=e->r;union Cyc_Absyn_Cnst
_tmp8F;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*
_tmp93;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*
_tmp98;struct Cyc_Absyn_Exp*_tmp99;enum Cyc_Absyn_Primop _tmp9B;struct Cyc_List_List*
_tmp9C;void*_tmp9E;void*_tmpA3;struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_Absyn_Enumfield*
_tmpA6;struct Cyc_Absyn_Enumfield*_tmpA8;_LL68: {struct Cyc_Absyn_Const_e_struct*
_tmp8E=(struct Cyc_Absyn_Const_e_struct*)_tmp8D;if(_tmp8E->tag != 0)goto _LL6A;
else{_tmp8F=_tmp8E->f1;}}_LL69: return({struct _tuple10 _tmpA9;_tmpA9.f1=_tmp8F;
_tmpA9.f2=1;_tmpA9;});_LL6A: {struct Cyc_Absyn_Conditional_e_struct*_tmp90=(
struct Cyc_Absyn_Conditional_e_struct*)_tmp8D;if(_tmp90->tag != 6)goto _LL6C;else{
_tmp91=_tmp90->f1;_tmp92=_tmp90->f2;_tmp93=_tmp90->f3;}}_LL6B: {int _tmpAB;int
_tmpAC;struct _tuple11 _tmpAA=Cyc_Evexp_eval_const_bool_exp(_tmp91);_tmpAB=_tmpAA.f1;
_tmpAC=_tmpAA.f2;if(!_tmpAC){Cyc_Evexp_eval_const_exp(_tmp92);Cyc_Evexp_eval_const_exp(
_tmp93);return({struct _tuple10 _tmpAD;_tmpAD.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,
0);_tmpAD.f2=0;_tmpAD;});}ans=_tmpAB?Cyc_Evexp_eval_const_exp(_tmp92): Cyc_Evexp_eval_const_exp(
_tmp93);goto _LL67;}_LL6C: {struct Cyc_Absyn_And_e_struct*_tmp94=(struct Cyc_Absyn_And_e_struct*)
_tmp8D;if(_tmp94->tag != 7)goto _LL6E;else{_tmp95=_tmp94->f1;_tmp96=_tmp94->f2;}}
_LL6D: {int _tmpAF;int _tmpB0;struct _tuple11 _tmpAE=Cyc_Evexp_eval_const_bool_exp(
_tmp95);_tmpAF=_tmpAE.f1;_tmpB0=_tmpAE.f2;if(!_tmpB0){Cyc_Evexp_eval_const_exp(
_tmp96);return({struct _tuple10 _tmpB1;_tmpB1.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,
0);_tmpB1.f2=0;_tmpB1;});}if(_tmpAF)ans=Cyc_Evexp_eval_const_exp(_tmp96);else{
ans=({struct _tuple10 _tmpB2;_tmpB2.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);_tmpB2.f2=
1;_tmpB2;});}goto _LL67;}_LL6E: {struct Cyc_Absyn_Or_e_struct*_tmp97=(struct Cyc_Absyn_Or_e_struct*)
_tmp8D;if(_tmp97->tag != 8)goto _LL70;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}
_LL6F: {int _tmpB4;int _tmpB5;struct _tuple11 _tmpB3=Cyc_Evexp_eval_const_bool_exp(
_tmp98);_tmpB4=_tmpB3.f1;_tmpB5=_tmpB3.f2;if(!_tmpB5){Cyc_Evexp_eval_const_exp(
_tmp99);return({struct _tuple10 _tmpB6;_tmpB6.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,
0);_tmpB6.f2=0;_tmpB6;});}if(!_tmpB4)ans=Cyc_Evexp_eval_const_exp(_tmp99);else{
ans=({struct _tuple10 _tmpB7;_tmpB7.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);_tmpB7.f2=
1;_tmpB7;});}goto _LL67;}_LL70: {struct Cyc_Absyn_Primop_e_struct*_tmp9A=(struct
Cyc_Absyn_Primop_e_struct*)_tmp8D;if(_tmp9A->tag != 3)goto _LL72;else{_tmp9B=
_tmp9A->f1;_tmp9C=_tmp9A->f2;}}_LL71: if(_tmp9C == 0){({void*_tmpB8=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmpB9="bad static expression (no args to primop)";
_tag_dyneither(_tmpB9,sizeof(char),42);}),_tag_dyneither(_tmpB8,sizeof(void*),0));});
return({struct _tuple10 _tmpBA;_tmpBA.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);
_tmpBA.f2=1;_tmpBA;});}if(_tmp9C->tl == 0){ans=Cyc_Evexp_eval_const_unprimop(
_tmp9B,(struct Cyc_Absyn_Exp*)_tmp9C->hd);goto _LL67;}if(((struct Cyc_List_List*)
_check_null(_tmp9C->tl))->tl == 0){ans=Cyc_Evexp_eval_const_binprimop(_tmp9B,(
struct Cyc_Absyn_Exp*)_tmp9C->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp9C->tl))->hd);goto _LL67;}({void*_tmpBB=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmpBC="bad static expression (too many args to primop)";
_tag_dyneither(_tmpBC,sizeof(char),48);}),_tag_dyneither(_tmpBB,sizeof(void*),0));});
return({struct _tuple10 _tmpBD;_tmpBD.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);
_tmpBD.f2=1;_tmpBD;});_LL72: {struct Cyc_Absyn_Valueof_e_struct*_tmp9D=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp8D;if(_tmp9D->tag != 40)goto _LL74;else{_tmp9E=(void*)_tmp9D->f1;}}_LL73:{void*
_tmpBE=Cyc_Tcutil_compress(_tmp9E);struct Cyc_Absyn_Exp*_tmpC0;_LL83: {struct Cyc_Absyn_ValueofType_struct*
_tmpBF=(struct Cyc_Absyn_ValueofType_struct*)_tmpBE;if(_tmpBF->tag != 19)goto _LL85;
else{_tmpC0=_tmpBF->f1;}}_LL84: return Cyc_Evexp_eval_const_exp(_tmpC0);_LL85:;
_LL86: goto _LL82;_LL82:;}goto _LL75;_LL74: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp9F=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp8D;if(_tmp9F->tag != 18)goto _LL76;}
_LL75: goto _LL77;_LL76: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpA0=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp8D;if(_tmpA0->tag != 19)goto _LL78;}_LL77: goto _LL79;_LL78: {struct Cyc_Absyn_Offsetof_e_struct*
_tmpA1=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp8D;if(_tmpA1->tag != 20)goto _LL7A;}
_LL79: ans=({struct _tuple10 _tmpC1;_tmpC1.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);
_tmpC1.f2=0;_tmpC1;});goto _LL67;_LL7A: {struct Cyc_Absyn_Cast_e_struct*_tmpA2=(
struct Cyc_Absyn_Cast_e_struct*)_tmp8D;if(_tmpA2->tag != 15)goto _LL7C;else{_tmpA3=(
void*)_tmpA2->f1;_tmpA4=_tmpA2->f2;}}_LL7B: ans=Cyc_Evexp_eval_const_exp(_tmpA4);
if(ans.f2){struct _tuple14 _tmpC3=({struct _tuple14 _tmpC2;_tmpC2.f1=Cyc_Tcutil_compress(
_tmpA3);_tmpC2.f2=ans.f1;_tmpC2;});void*_tmpC4;enum Cyc_Absyn_Sign _tmpC6;enum 
Cyc_Absyn_Size_of _tmpC7;union Cyc_Absyn_Cnst _tmpC8;struct _tuple3 _tmpC9;enum Cyc_Absyn_Sign
_tmpCA;char _tmpCB;void*_tmpCC;enum Cyc_Absyn_Sign _tmpCE;enum Cyc_Absyn_Size_of
_tmpCF;union Cyc_Absyn_Cnst _tmpD0;struct _tuple4 _tmpD1;enum Cyc_Absyn_Sign _tmpD2;
short _tmpD3;void*_tmpD4;enum Cyc_Absyn_Sign _tmpD6;enum Cyc_Absyn_Size_of _tmpD7;
union Cyc_Absyn_Cnst _tmpD8;struct _tuple5 _tmpD9;enum Cyc_Absyn_Sign _tmpDA;int
_tmpDB;void*_tmpDC;void*_tmpDE;union Cyc_Absyn_Cnst _tmpDF;struct _tuple3 _tmpE0;
enum Cyc_Absyn_Sign _tmpE1;char _tmpE2;void*_tmpE3;void*_tmpE5;union Cyc_Absyn_Cnst
_tmpE6;struct _tuple4 _tmpE7;enum Cyc_Absyn_Sign _tmpE8;short _tmpE9;void*_tmpEA;
void*_tmpEC;union Cyc_Absyn_Cnst _tmpED;struct _tuple5 _tmpEE;enum Cyc_Absyn_Sign
_tmpEF;int _tmpF0;_LL88: _tmpC4=_tmpC3.f1;{struct Cyc_Absyn_IntType_struct*_tmpC5=(
struct Cyc_Absyn_IntType_struct*)_tmpC4;if(_tmpC5->tag != 6)goto _LL8A;else{_tmpC6=
_tmpC5->f1;_tmpC7=_tmpC5->f2;}};_tmpC8=_tmpC3.f2;if((_tmpC8.Char_c).tag != 2)goto
_LL8A;_tmpC9=(struct _tuple3)(_tmpC8.Char_c).val;_tmpCA=_tmpC9.f1;_tmpCB=_tmpC9.f2;
_LL89: _tmpCE=_tmpC6;_tmpCF=_tmpC7;_tmpD2=_tmpCA;_tmpD3=(short)_tmpCB;goto _LL8B;
_LL8A: _tmpCC=_tmpC3.f1;{struct Cyc_Absyn_IntType_struct*_tmpCD=(struct Cyc_Absyn_IntType_struct*)
_tmpCC;if(_tmpCD->tag != 6)goto _LL8C;else{_tmpCE=_tmpCD->f1;_tmpCF=_tmpCD->f2;}};
_tmpD0=_tmpC3.f2;if((_tmpD0.Short_c).tag != 3)goto _LL8C;_tmpD1=(struct _tuple4)(
_tmpD0.Short_c).val;_tmpD2=_tmpD1.f1;_tmpD3=_tmpD1.f2;_LL8B: _tmpD6=_tmpCE;_tmpD7=
_tmpCF;_tmpDA=_tmpD2;_tmpDB=(int)_tmpD3;goto _LL8D;_LL8C: _tmpD4=_tmpC3.f1;{struct
Cyc_Absyn_IntType_struct*_tmpD5=(struct Cyc_Absyn_IntType_struct*)_tmpD4;if(
_tmpD5->tag != 6)goto _LL8E;else{_tmpD6=_tmpD5->f1;_tmpD7=_tmpD5->f2;}};_tmpD8=
_tmpC3.f2;if((_tmpD8.Int_c).tag != 4)goto _LL8E;_tmpD9=(struct _tuple5)(_tmpD8.Int_c).val;
_tmpDA=_tmpD9.f1;_tmpDB=_tmpD9.f2;_LL8D: if(_tmpD6 != _tmpDA)ans.f1=Cyc_Absyn_Int_c(
_tmpD6,_tmpDB);goto _LL87;_LL8E: _tmpDC=_tmpC3.f1;{struct Cyc_Absyn_TagType_struct*
_tmpDD=(struct Cyc_Absyn_TagType_struct*)_tmpDC;if(_tmpDD->tag != 20)goto _LL90;
else{_tmpDE=(void*)_tmpDD->f1;}};_tmpDF=_tmpC3.f2;if((_tmpDF.Char_c).tag != 2)
goto _LL90;_tmpE0=(struct _tuple3)(_tmpDF.Char_c).val;_tmpE1=_tmpE0.f1;_tmpE2=
_tmpE0.f2;_LL8F: _tmpE5=_tmpDE;_tmpE8=_tmpE1;_tmpE9=(short)_tmpE2;goto _LL91;_LL90:
_tmpE3=_tmpC3.f1;{struct Cyc_Absyn_TagType_struct*_tmpE4=(struct Cyc_Absyn_TagType_struct*)
_tmpE3;if(_tmpE4->tag != 20)goto _LL92;else{_tmpE5=(void*)_tmpE4->f1;}};_tmpE6=
_tmpC3.f2;if((_tmpE6.Short_c).tag != 3)goto _LL92;_tmpE7=(struct _tuple4)(_tmpE6.Short_c).val;
_tmpE8=_tmpE7.f1;_tmpE9=_tmpE7.f2;_LL91: _tmpEC=_tmpE5;_tmpEF=_tmpE8;_tmpF0=(int)
_tmpE9;goto _LL93;_LL92: _tmpEA=_tmpC3.f1;{struct Cyc_Absyn_TagType_struct*_tmpEB=(
struct Cyc_Absyn_TagType_struct*)_tmpEA;if(_tmpEB->tag != 20)goto _LL94;else{_tmpEC=(
void*)_tmpEB->f1;}};_tmpED=_tmpC3.f2;if((_tmpED.Int_c).tag != 4)goto _LL94;_tmpEE=(
struct _tuple5)(_tmpED.Int_c).val;_tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;_LL93: if(
_tmpF0 < 0)({void*_tmpF1=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpF2="cannot cast negative number to a tag type";
_tag_dyneither(_tmpF2,sizeof(char),42);}),_tag_dyneither(_tmpF1,sizeof(void*),0));});
Cyc_Tcutil_unify(_tmpEC,(void*)({struct Cyc_Absyn_ValueofType_struct*_tmpF3=
_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_ValueofType_struct _tmpF4;
_tmpF4.tag=19;_tmpF4.f1=Cyc_Absyn_const_exp(ans.f1,0);_tmpF4;});_tmpF3;}));ans.f1=
Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmpF0);goto _LL87;_LL94:;_LL95:({struct Cyc_String_pa_struct
_tmpF7;_tmpF7.tag=0;_tmpF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmpA3));({void*_tmpF5[1]={& _tmpF7};Cyc_Tcutil_terr(e->loc,({const char*_tmpF6="cannot cast to %s";
_tag_dyneither(_tmpF6,sizeof(char),18);}),_tag_dyneither(_tmpF5,sizeof(void*),1));});});
goto _LL87;_LL87:;}goto _LL67;_LL7C: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpA5=(
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp8D;if(_tmpA5->tag != 34)goto _LL7E;else{
_tmpA6=_tmpA5->f3;}}_LL7D: _tmpA8=_tmpA6;goto _LL7F;_LL7E: {struct Cyc_Absyn_Enum_e_struct*
_tmpA7=(struct Cyc_Absyn_Enum_e_struct*)_tmp8D;if(_tmpA7->tag != 33)goto _LL80;
else{_tmpA8=_tmpA7->f3;}}_LL7F: ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)
_check_null(((struct Cyc_Absyn_Enumfield*)_check_null(_tmpA8))->tag));goto _LL67;
_LL80:;_LL81:({struct Cyc_String_pa_struct _tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmpF8[1]={&
_tmpFA};Cyc_Tcutil_terr(e->loc,({const char*_tmpF9="bad static expression %s";
_tag_dyneither(_tmpF9,sizeof(char),25);}),_tag_dyneither(_tmpF8,sizeof(void*),1));});});
return({struct _tuple10 _tmpFB;_tmpFB.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0);
_tmpFB.f2=1;_tmpFB;});_LL67:;}if(ans.f2)e->r=(void*)({struct Cyc_Absyn_Const_e_struct*
_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=({struct Cyc_Absyn_Const_e_struct
_tmpFD;_tmpFD.tag=0;_tmpFD.f1=ans.f1;_tmpFD;});_tmpFC;});return ans;}int Cyc_Evexp_c_can_eval(
struct Cyc_Absyn_Exp*e){void*_tmpFE=e->r;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*
_tmp107;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*
_tmp10B;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;enum Cyc_Absyn_Primop
_tmp110;struct Cyc_List_List*_tmp111;struct Cyc_Absyn_Exp*_tmp114;_LL97: {struct
Cyc_Absyn_AnonEnum_e_struct*_tmpFF=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmpFE;
if(_tmpFF->tag != 34)goto _LL99;}_LL98: goto _LL9A;_LL99: {struct Cyc_Absyn_Enum_e_struct*
_tmp100=(struct Cyc_Absyn_Enum_e_struct*)_tmpFE;if(_tmp100->tag != 33)goto _LL9B;}
_LL9A: goto _LL9C;_LL9B: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp101=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpFE;if(_tmp101->tag != 18)goto _LL9D;}_LL9C: goto _LL9E;_LL9D: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp102=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpFE;if(_tmp102->tag != 19)goto
_LL9F;}_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_Offsetof_e_struct*_tmp103=(struct
Cyc_Absyn_Offsetof_e_struct*)_tmpFE;if(_tmp103->tag != 20)goto _LLA1;}_LLA0: goto
_LLA2;_LLA1: {struct Cyc_Absyn_Const_e_struct*_tmp104=(struct Cyc_Absyn_Const_e_struct*)
_tmpFE;if(_tmp104->tag != 0)goto _LLA3;}_LLA2: return 1;_LLA3: {struct Cyc_Absyn_Conditional_e_struct*
_tmp105=(struct Cyc_Absyn_Conditional_e_struct*)_tmpFE;if(_tmp105->tag != 6)goto
_LLA5;else{_tmp106=_tmp105->f1;_tmp107=_tmp105->f2;_tmp108=_tmp105->f3;}}_LLA4:
return(Cyc_Evexp_c_can_eval(_tmp106) && Cyc_Evexp_c_can_eval(_tmp107)) && Cyc_Evexp_c_can_eval(
_tmp108);_LLA5: {struct Cyc_Absyn_And_e_struct*_tmp109=(struct Cyc_Absyn_And_e_struct*)
_tmpFE;if(_tmp109->tag != 7)goto _LLA7;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}}
_LLA6: _tmp10D=_tmp10A;_tmp10E=_tmp10B;goto _LLA8;_LLA7: {struct Cyc_Absyn_Or_e_struct*
_tmp10C=(struct Cyc_Absyn_Or_e_struct*)_tmpFE;if(_tmp10C->tag != 8)goto _LLA9;else{
_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}}_LLA8: return Cyc_Evexp_c_can_eval(
_tmp10D) && Cyc_Evexp_c_can_eval(_tmp10E);_LLA9: {struct Cyc_Absyn_Primop_e_struct*
_tmp10F=(struct Cyc_Absyn_Primop_e_struct*)_tmpFE;if(_tmp10F->tag != 3)goto _LLAB;
else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;}}_LLAA: for(0;_tmp111 != 0;_tmp111=
_tmp111->tl){if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_tmp111->hd))return 0;}
return 1;_LLAB: {struct Cyc_Absyn_Valueof_e_struct*_tmp112=(struct Cyc_Absyn_Valueof_e_struct*)
_tmpFE;if(_tmp112->tag != 40)goto _LLAD;}_LLAC: return 0;_LLAD: {struct Cyc_Absyn_Cast_e_struct*
_tmp113=(struct Cyc_Absyn_Cast_e_struct*)_tmpFE;if(_tmp113->tag != 15)goto _LLAF;
else{_tmp114=_tmp113->f2;}}_LLAE: return Cyc_Evexp_c_can_eval(_tmp114);_LLAF:;
_LLB0: return 0;_LL96:;}static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*
e){void*_tmp115=e->r;_LLB2: {struct Cyc_Absyn_Const_e_struct*_tmp116=(struct Cyc_Absyn_Const_e_struct*)
_tmp115;if(_tmp116->tag != 0)goto _LLB4;}_LLB3: return 1;_LLB4: {struct Cyc_Absyn_Conditional_e_struct*
_tmp117=(struct Cyc_Absyn_Conditional_e_struct*)_tmp115;if(_tmp117->tag != 6)goto
_LLB6;}_LLB5: return 2;_LLB6: {struct Cyc_Absyn_Primop_e_struct*_tmp118=(struct Cyc_Absyn_Primop_e_struct*)
_tmp115;if(_tmp118->tag != 3)goto _LLB8;}_LLB7: return 3;_LLB8: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp119=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp115;if(_tmp119->tag != 18)goto
_LLBA;}_LLB9: goto _LLBB;_LLBA: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp11A=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp115;if(_tmp11A->tag != 19)goto _LLBC;}
_LLBB: return 4;_LLBC: {struct Cyc_Absyn_Offsetof_e_struct*_tmp11B=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp115;if(_tmp11B->tag != 20)goto _LLBE;}_LLBD: return 5;_LLBE: {struct Cyc_Absyn_Cast_e_struct*
_tmp11C=(struct Cyc_Absyn_Cast_e_struct*)_tmp115;if(_tmp11C->tag != 15)goto _LLC0;}
_LLBF: return 6;_LLC0: {struct Cyc_Absyn_And_e_struct*_tmp11D=(struct Cyc_Absyn_And_e_struct*)
_tmp115;if(_tmp11D->tag != 7)goto _LLC2;}_LLC1: return 7;_LLC2: {struct Cyc_Absyn_Or_e_struct*
_tmp11E=(struct Cyc_Absyn_Or_e_struct*)_tmp115;if(_tmp11E->tag != 8)goto _LLC4;}
_LLC3: return 8;_LLC4: {struct Cyc_Absyn_Valueof_e_struct*_tmp11F=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp115;if(_tmp11F->tag != 40)goto _LLC6;}_LLC5: return 9;_LLC6:;_LLC7:({struct Cyc_String_pa_struct
_tmp122;_tmp122.tag=0;_tmp122.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e));({void*_tmp120[1]={& _tmp122};Cyc_Tcutil_terr(e->loc,({
const char*_tmp121="bad static expression %s";_tag_dyneither(_tmp121,sizeof(char),
25);}),_tag_dyneither(_tmp120,sizeof(void*),1));});});return 0;_LLB1:;}struct
_tuple15{void*f1;void*f2;};int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){unsigned int _tmp124;int _tmp125;struct _tuple9 _tmp123=Cyc_Evexp_eval_const_uint_exp(
e1);_tmp124=_tmp123.f1;_tmp125=_tmp123.f2;{unsigned int _tmp127;int _tmp128;struct
_tuple9 _tmp126=Cyc_Evexp_eval_const_uint_exp(e2);_tmp127=_tmp126.f1;_tmp128=
_tmp126.f2;if(_tmp125  && _tmp128)return(int)(_tmp124 - _tmp127);{int _tmp129=Cyc_Evexp_const_exp_case_number(
e1);int _tmp12A=Cyc_Evexp_const_exp_case_number(e2);if(_tmp129 != _tmp12A)return
_tmp129 - _tmp12A;{struct _tuple15 _tmp12C=({struct _tuple15 _tmp12B;_tmp12B.f1=e1->r;
_tmp12B.f2=e2->r;_tmp12B;});void*_tmp12D;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*
_tmp130;struct Cyc_Absyn_Exp*_tmp131;void*_tmp132;struct Cyc_Absyn_Exp*_tmp134;
struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp136;void*_tmp137;struct Cyc_Absyn_Exp*
_tmp139;struct Cyc_Absyn_Exp*_tmp13A;void*_tmp13B;struct Cyc_Absyn_Exp*_tmp13D;
struct Cyc_Absyn_Exp*_tmp13E;void*_tmp13F;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*
_tmp142;void*_tmp143;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp146;
void*_tmp147;enum Cyc_Absyn_Primop _tmp149;struct Cyc_List_List*_tmp14A;void*
_tmp14B;enum Cyc_Absyn_Primop _tmp14D;struct Cyc_List_List*_tmp14E;void*_tmp14F;
void*_tmp151;void*_tmp152;void*_tmp154;void*_tmp155;void*_tmp157;void*_tmp158;
struct Cyc_Absyn_Exp*_tmp15A;void*_tmp15B;struct Cyc_Absyn_Exp*_tmp15D;void*
_tmp15E;void*_tmp160;void*_tmp161;struct Cyc_Absyn_Exp*_tmp163;void*_tmp164;
struct Cyc_Absyn_Exp*_tmp166;void*_tmp167;void*_tmp169;void*_tmp16A;void*_tmp16B;
void*_tmp16D;void*_tmp16E;void*_tmp16F;void*_tmp171;struct Cyc_Absyn_Exp*_tmp172;
void*_tmp173;void*_tmp175;struct Cyc_Absyn_Exp*_tmp176;void*_tmp177;void*_tmp179;
void*_tmp17A;void*_tmp17C;_LLC9: _tmp12D=_tmp12C.f1;{struct Cyc_Absyn_Conditional_e_struct*
_tmp12E=(struct Cyc_Absyn_Conditional_e_struct*)_tmp12D;if(_tmp12E->tag != 6)goto
_LLCB;else{_tmp12F=_tmp12E->f1;_tmp130=_tmp12E->f2;_tmp131=_tmp12E->f3;}};
_tmp132=_tmp12C.f2;{struct Cyc_Absyn_Conditional_e_struct*_tmp133=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp132;if(_tmp133->tag != 6)goto _LLCB;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;
_tmp136=_tmp133->f3;}};_LLCA: {int _tmp17D=Cyc_Evexp_const_exp_cmp(_tmp131,
_tmp136);if(_tmp17D != 0)return _tmp17D;_tmp139=_tmp12F;_tmp13A=_tmp130;_tmp13D=
_tmp134;_tmp13E=_tmp135;goto _LLCC;}_LLCB: _tmp137=_tmp12C.f1;{struct Cyc_Absyn_And_e_struct*
_tmp138=(struct Cyc_Absyn_And_e_struct*)_tmp137;if(_tmp138->tag != 7)goto _LLCD;
else{_tmp139=_tmp138->f1;_tmp13A=_tmp138->f2;}};_tmp13B=_tmp12C.f2;{struct Cyc_Absyn_And_e_struct*
_tmp13C=(struct Cyc_Absyn_And_e_struct*)_tmp13B;if(_tmp13C->tag != 7)goto _LLCD;
else{_tmp13D=_tmp13C->f1;_tmp13E=_tmp13C->f2;}};_LLCC: _tmp141=_tmp139;_tmp142=
_tmp13A;_tmp145=_tmp13D;_tmp146=_tmp13E;goto _LLCE;_LLCD: _tmp13F=_tmp12C.f1;{
struct Cyc_Absyn_Or_e_struct*_tmp140=(struct Cyc_Absyn_Or_e_struct*)_tmp13F;if(
_tmp140->tag != 8)goto _LLCF;else{_tmp141=_tmp140->f1;_tmp142=_tmp140->f2;}};
_tmp143=_tmp12C.f2;{struct Cyc_Absyn_Or_e_struct*_tmp144=(struct Cyc_Absyn_Or_e_struct*)
_tmp143;if(_tmp144->tag != 8)goto _LLCF;else{_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;}};
_LLCE: {int _tmp17E=Cyc_Evexp_const_exp_cmp(_tmp141,_tmp145);if(_tmp17E != 0)
return _tmp17E;return Cyc_Evexp_const_exp_cmp(_tmp142,_tmp146);}_LLCF: _tmp147=
_tmp12C.f1;{struct Cyc_Absyn_Primop_e_struct*_tmp148=(struct Cyc_Absyn_Primop_e_struct*)
_tmp147;if(_tmp148->tag != 3)goto _LLD1;else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;}};
_tmp14B=_tmp12C.f2;{struct Cyc_Absyn_Primop_e_struct*_tmp14C=(struct Cyc_Absyn_Primop_e_struct*)
_tmp14B;if(_tmp14C->tag != 3)goto _LLD1;else{_tmp14D=_tmp14C->f1;_tmp14E=_tmp14C->f2;}};
_LLD0: {int _tmp17F=(int)_tmp149 - (int)_tmp14D;if(_tmp17F != 0)return _tmp17F;for(0;
_tmp14A != 0  && _tmp14E != 0;(_tmp14A=_tmp14A->tl,_tmp14E=_tmp14E->tl)){int _tmp180=
Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)_tmp14A->hd,(struct Cyc_Absyn_Exp*)
_tmp14E->hd);if(_tmp180 != 0)return _tmp180;}return 0;}_LLD1: _tmp14F=_tmp12C.f1;{
struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp150=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp14F;if(_tmp150->tag != 18)goto _LLD3;else{_tmp151=(void*)_tmp150->f1;}};
_tmp152=_tmp12C.f2;{struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp153=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp152;if(_tmp153->tag != 18)goto _LLD3;else{_tmp154=(void*)_tmp153->f1;}};_LLD2:
return Cyc_Tcutil_typecmp(_tmp151,_tmp154);_LLD3: _tmp155=_tmp12C.f1;{struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp156=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp155;if(_tmp156->tag != 18)goto
_LLD5;else{_tmp157=(void*)_tmp156->f1;}};_tmp158=_tmp12C.f2;{struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp159=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp158;if(_tmp159->tag != 19)goto
_LLD5;else{_tmp15A=_tmp159->f1;}};_LLD4: if(_tmp15A->topt == 0)({void*_tmp181=0;
Cyc_Tcutil_terr(e2->loc,({const char*_tmp182="cannot handle sizeof(exp) here -- use sizeof(type)";
_tag_dyneither(_tmp182,sizeof(char),51);}),_tag_dyneither(_tmp181,sizeof(void*),
0));});return Cyc_Tcutil_typecmp(_tmp157,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp15A->topt))->v);_LLD5: _tmp15B=_tmp12C.f1;{struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp15C=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp15B;if(_tmp15C->tag != 19)goto
_LLD7;else{_tmp15D=_tmp15C->f1;}};_tmp15E=_tmp12C.f2;{struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp15F=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp15E;if(_tmp15F->tag != 18)goto
_LLD7;else{_tmp160=(void*)_tmp15F->f1;}};_LLD6: if(_tmp15D->topt == 0)({void*
_tmp183=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp184="cannot handle sizeof(exp) here -- use sizeof(type)";
_tag_dyneither(_tmp184,sizeof(char),51);}),_tag_dyneither(_tmp183,sizeof(void*),
0));});return Cyc_Tcutil_typecmp((void*)((struct Cyc_Core_Opt*)_check_null(_tmp15D->topt))->v,
_tmp160);_LLD7: _tmp161=_tmp12C.f1;{struct Cyc_Absyn_Sizeofexp_e_struct*_tmp162=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp161;if(_tmp162->tag != 19)goto _LLD9;else{
_tmp163=_tmp162->f1;}};_tmp164=_tmp12C.f2;{struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp165=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp164;if(_tmp165->tag != 19)goto
_LLD9;else{_tmp166=_tmp165->f1;}};_LLD8: if(_tmp163->topt == 0)({void*_tmp185=0;
Cyc_Tcutil_terr(e1->loc,({const char*_tmp186="cannot handle sizeof(exp) here -- use sizeof(type)";
_tag_dyneither(_tmp186,sizeof(char),51);}),_tag_dyneither(_tmp185,sizeof(void*),
0));});if(_tmp166->topt == 0)({void*_tmp187=0;Cyc_Tcutil_terr(e2->loc,({const char*
_tmp188="cannot handle sizeof(exp) here -- use sizeof(type)";_tag_dyneither(
_tmp188,sizeof(char),51);}),_tag_dyneither(_tmp187,sizeof(void*),0));});return
Cyc_Tcutil_typecmp((void*)((struct Cyc_Core_Opt*)_check_null(_tmp163->topt))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp166->topt))->v);_LLD9: _tmp167=
_tmp12C.f1;{struct Cyc_Absyn_Offsetof_e_struct*_tmp168=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp167;if(_tmp168->tag != 20)goto _LLDB;else{_tmp169=(void*)_tmp168->f1;_tmp16A=(
void*)_tmp168->f2;}};_tmp16B=_tmp12C.f2;{struct Cyc_Absyn_Offsetof_e_struct*
_tmp16C=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp16B;if(_tmp16C->tag != 20)goto
_LLDB;else{_tmp16D=(void*)_tmp16C->f1;_tmp16E=(void*)_tmp16C->f2;}};_LLDA: {int
_tmp189=Cyc_Tcutil_typecmp(_tmp169,_tmp16D);if(_tmp189 != 0)return _tmp189;{struct
_tuple15 _tmp18B=({struct _tuple15 _tmp18A;_tmp18A.f1=_tmp16A;_tmp18A.f2=_tmp16E;
_tmp18A;});void*_tmp18C;void*_tmp18E;void*_tmp190;void*_tmp192;void*_tmp194;
struct _dyneither_ptr*_tmp196;void*_tmp197;struct _dyneither_ptr*_tmp199;void*
_tmp19A;unsigned int _tmp19C;void*_tmp19D;unsigned int _tmp19F;_LLE2: _tmp18C=
_tmp18B.f1;{struct Cyc_Absyn_StructField_struct*_tmp18D=(struct Cyc_Absyn_StructField_struct*)
_tmp18C;if(_tmp18D->tag != 0)goto _LLE4;};_tmp18E=_tmp18B.f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp18F=(struct Cyc_Absyn_TupleIndex_struct*)_tmp18E;if(_tmp18F->tag != 1)goto
_LLE4;};_LLE3: return - 1;_LLE4: _tmp190=_tmp18B.f1;{struct Cyc_Absyn_TupleIndex_struct*
_tmp191=(struct Cyc_Absyn_TupleIndex_struct*)_tmp190;if(_tmp191->tag != 1)goto
_LLE6;};_tmp192=_tmp18B.f2;{struct Cyc_Absyn_StructField_struct*_tmp193=(struct
Cyc_Absyn_StructField_struct*)_tmp192;if(_tmp193->tag != 0)goto _LLE6;};_LLE5:
return 1;_LLE6: _tmp194=_tmp18B.f1;{struct Cyc_Absyn_StructField_struct*_tmp195=(
struct Cyc_Absyn_StructField_struct*)_tmp194;if(_tmp195->tag != 0)goto _LLE8;else{
_tmp196=_tmp195->f1;}};_tmp197=_tmp18B.f2;{struct Cyc_Absyn_StructField_struct*
_tmp198=(struct Cyc_Absyn_StructField_struct*)_tmp197;if(_tmp198->tag != 0)goto
_LLE8;else{_tmp199=_tmp198->f1;}};_LLE7: return Cyc_strptrcmp(_tmp196,_tmp199);
_LLE8: _tmp19A=_tmp18B.f1;{struct Cyc_Absyn_TupleIndex_struct*_tmp19B=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp19A;if(_tmp19B->tag != 1)goto _LLE1;else{_tmp19C=_tmp19B->f1;}};_tmp19D=
_tmp18B.f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp19E=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp19D;if(_tmp19E->tag != 1)goto _LLE1;else{_tmp19F=_tmp19E->f1;}};_LLE9: return(
int)(_tmp19C - _tmp19F);_LLE1:;};}_LLDB: _tmp16F=_tmp12C.f1;{struct Cyc_Absyn_Cast_e_struct*
_tmp170=(struct Cyc_Absyn_Cast_e_struct*)_tmp16F;if(_tmp170->tag != 15)goto _LLDD;
else{_tmp171=(void*)_tmp170->f1;_tmp172=_tmp170->f2;}};_tmp173=_tmp12C.f2;{
struct Cyc_Absyn_Cast_e_struct*_tmp174=(struct Cyc_Absyn_Cast_e_struct*)_tmp173;
if(_tmp174->tag != 15)goto _LLDD;else{_tmp175=(void*)_tmp174->f1;_tmp176=_tmp174->f2;}};
_LLDC: {int _tmp1A0=Cyc_Tcutil_typecmp(_tmp171,_tmp175);if(_tmp1A0 != 0)return
_tmp1A0;return Cyc_Evexp_const_exp_cmp(_tmp172,_tmp176);}_LLDD: _tmp177=_tmp12C.f1;{
struct Cyc_Absyn_Valueof_e_struct*_tmp178=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp177;if(_tmp178->tag != 40)goto _LLDF;else{_tmp179=(void*)_tmp178->f1;}};
_tmp17A=_tmp12C.f2;{struct Cyc_Absyn_Valueof_e_struct*_tmp17B=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp17A;if(_tmp17B->tag != 40)goto _LLDF;else{_tmp17C=(void*)_tmp17B->f1;}};_LLDE:
if(Cyc_Tcutil_unify(_tmp179,_tmp17C))return 0;return Cyc_Tcutil_typecmp(_tmp179,
_tmp17C);_LLDF:;_LLE0:({void*_tmp1A1=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A2="Evexp::const_exp_cmp, unexpected case";
_tag_dyneither(_tmp1A2,sizeof(char),38);}),_tag_dyneither(_tmp1A1,sizeof(void*),
0));});_LLC8:;};};};}int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}int Cyc_Evexp_lte_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){unsigned int _tmp1A4;int _tmp1A5;
struct _tuple9 _tmp1A3=Cyc_Evexp_eval_const_uint_exp(e1);_tmp1A4=_tmp1A3.f1;
_tmp1A5=_tmp1A3.f2;{unsigned int _tmp1A7;int _tmp1A8;struct _tuple9 _tmp1A6=Cyc_Evexp_eval_const_uint_exp(
e2);_tmp1A7=_tmp1A6.f1;_tmp1A8=_tmp1A6.f2;if(_tmp1A5  && _tmp1A8)return _tmp1A4 <= 
_tmp1A7;return Cyc_Evexp_same_const_exp(e1,e2);};}struct _tuple16{struct Cyc_Absyn_Tqual
f1;void*f2;};int Cyc_Evexp_okay_szofarg(void*t){void*_tmp1A9=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp1AC;struct Cyc_Core_Opt*_tmp1AE;struct Cyc_Core_Opt
_tmp1AF;enum Cyc_Absyn_Kind _tmp1B0;struct Cyc_Absyn_DatatypeFieldInfo _tmp1B4;
union Cyc_Absyn_DatatypeFieldInfoU _tmp1B5;struct _tuple1 _tmp1B6;struct Cyc_Absyn_Datatypefield*
_tmp1B7;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_ArrayInfo _tmp1C1;void*
_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_AggrInfo _tmp1C8;union Cyc_Absyn_AggrInfoU
_tmp1C9;struct Cyc_List_List*_tmp1CB;struct Cyc_Absyn_Typedefdecl*_tmp1D5;_LLEB: {
struct Cyc_Absyn_VoidType_struct*_tmp1AA=(struct Cyc_Absyn_VoidType_struct*)
_tmp1A9;if(_tmp1AA->tag != 0)goto _LLED;}_LLEC: return 0;_LLED: {struct Cyc_Absyn_VarType_struct*
_tmp1AB=(struct Cyc_Absyn_VarType_struct*)_tmp1A9;if(_tmp1AB->tag != 2)goto _LLEF;
else{_tmp1AC=_tmp1AB->f1;}}_LLEE: switch(Cyc_Tcutil_tvar_kind(_tmp1AC,Cyc_Absyn_BoxKind)){
case Cyc_Absyn_BoxKind: _LL123: return 1;default: _LL124: return 0;}_LLEF: {struct Cyc_Absyn_Evar_struct*
_tmp1AD=(struct Cyc_Absyn_Evar_struct*)_tmp1A9;if(_tmp1AD->tag != 1)goto _LLF1;
else{_tmp1AE=_tmp1AD->f1;if(_tmp1AE == 0)goto _LLF1;_tmp1AF=*_tmp1AE;_tmp1B0=(
enum Cyc_Absyn_Kind)_tmp1AF.v;if(_tmp1B0 != Cyc_Absyn_BoxKind)goto _LLF1;}}_LLF0:
return 1;_LLF1: {struct Cyc_Absyn_Evar_struct*_tmp1B1=(struct Cyc_Absyn_Evar_struct*)
_tmp1A9;if(_tmp1B1->tag != 1)goto _LLF3;}_LLF2: return 0;_LLF3: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1B2=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1A9;if(_tmp1B2->tag != 3)goto
_LLF5;}_LLF4: return 1;_LLF5: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1B3=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp1A9;if(_tmp1B3->tag != 4)goto _LLF7;
else{_tmp1B4=_tmp1B3->f1;_tmp1B5=_tmp1B4.field_info;if((_tmp1B5.KnownDatatypefield).tag
!= 2)goto _LLF7;_tmp1B6=(struct _tuple1)(_tmp1B5.KnownDatatypefield).val;_tmp1B7=
_tmp1B6.f2;}}_LLF6: _tmp1B9=_tmp1B7->typs;goto _LLF8;_LLF7: {struct Cyc_Absyn_TupleType_struct*
_tmp1B8=(struct Cyc_Absyn_TupleType_struct*)_tmp1A9;if(_tmp1B8->tag != 11)goto
_LLF9;else{_tmp1B9=_tmp1B8->f1;}}_LLF8: for(0;_tmp1B9 != 0;_tmp1B9=_tmp1B9->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple16*)_tmp1B9->hd)).f2))return 0;}return
1;_LLF9: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1BA=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp1A9;if(_tmp1BA->tag != 4)goto _LLFB;}_LLFA: return 0;_LLFB: {struct Cyc_Absyn_PointerType_struct*
_tmp1BB=(struct Cyc_Absyn_PointerType_struct*)_tmp1A9;if(_tmp1BB->tag != 5)goto
_LLFD;}_LLFC: return 1;_LLFD: {struct Cyc_Absyn_TagType_struct*_tmp1BC=(struct Cyc_Absyn_TagType_struct*)
_tmp1A9;if(_tmp1BC->tag != 20)goto _LLFF;}_LLFE: goto _LL100;_LLFF: {struct Cyc_Absyn_IntType_struct*
_tmp1BD=(struct Cyc_Absyn_IntType_struct*)_tmp1A9;if(_tmp1BD->tag != 6)goto _LL101;}
_LL100: goto _LL102;_LL101: {struct Cyc_Absyn_FloatType_struct*_tmp1BE=(struct Cyc_Absyn_FloatType_struct*)
_tmp1A9;if(_tmp1BE->tag != 7)goto _LL103;}_LL102: goto _LL104;_LL103: {struct Cyc_Absyn_DoubleType_struct*
_tmp1BF=(struct Cyc_Absyn_DoubleType_struct*)_tmp1A9;if(_tmp1BF->tag != 8)goto
_LL105;}_LL104: return 1;_LL105: {struct Cyc_Absyn_ArrayType_struct*_tmp1C0=(struct
Cyc_Absyn_ArrayType_struct*)_tmp1A9;if(_tmp1C0->tag != 9)goto _LL107;else{_tmp1C1=
_tmp1C0->f1;_tmp1C2=_tmp1C1.elt_type;_tmp1C3=_tmp1C1.num_elts;}}_LL106: return
_tmp1C3 != 0;_LL107: {struct Cyc_Absyn_FnType_struct*_tmp1C4=(struct Cyc_Absyn_FnType_struct*)
_tmp1A9;if(_tmp1C4->tag != 10)goto _LL109;}_LL108: return 0;_LL109: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp1C5=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp1A9;if(_tmp1C5->tag != 16)goto
_LL10B;}_LL10A: return 1;_LL10B: {struct Cyc_Absyn_DynRgnType_struct*_tmp1C6=(
struct Cyc_Absyn_DynRgnType_struct*)_tmp1A9;if(_tmp1C6->tag != 17)goto _LL10D;}
_LL10C: return 1;_LL10D: {struct Cyc_Absyn_AggrType_struct*_tmp1C7=(struct Cyc_Absyn_AggrType_struct*)
_tmp1A9;if(_tmp1C7->tag != 12)goto _LL10F;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C8.aggr_info;}}
_LL10E: {struct Cyc_Absyn_Aggrdecl*ad;{union Cyc_Absyn_AggrInfoU _tmp1D6=_tmp1C9;
struct _tuple2 _tmp1D7;struct Cyc_Absyn_Aggrdecl**_tmp1D8;struct Cyc_Absyn_Aggrdecl*
_tmp1D9;_LL127: if((_tmp1D6.UnknownAggr).tag != 1)goto _LL129;_tmp1D7=(struct
_tuple2)(_tmp1D6.UnknownAggr).val;_LL128:({void*_tmp1DA=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1DB="szof on unchecked StructType";
_tag_dyneither(_tmp1DB,sizeof(char),29);}),_tag_dyneither(_tmp1DA,sizeof(void*),
0));});_LL129: if((_tmp1D6.KnownAggr).tag != 2)goto _LL126;_tmp1D8=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1D6.KnownAggr).val;_tmp1D9=*_tmp1D8;_LL12A: ad=_tmp1D9;_LL126:;}if(ad->impl == 
0)return 0;_tmp1CB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
goto _LL110;}_LL10F: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1CA=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1A9;if(_tmp1CA->tag != 13)goto _LL111;else{_tmp1CB=_tmp1CA->f2;}}_LL110: for(0;
_tmp1CB != 0;_tmp1CB=_tmp1CB->tl){if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)
_tmp1CB->hd)->type))return 0;}return 1;_LL111: {struct Cyc_Absyn_EnumType_struct*
_tmp1CC=(struct Cyc_Absyn_EnumType_struct*)_tmp1A9;if(_tmp1CC->tag != 14)goto
_LL113;}_LL112: goto _LL114;_LL113: {struct Cyc_Absyn_AnonEnumType_struct*_tmp1CD=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp1A9;if(_tmp1CD->tag != 15)goto _LL115;}
_LL114: return 1;_LL115: {struct Cyc_Absyn_HeapRgn_struct*_tmp1CE=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp1A9;if(_tmp1CE->tag != 21)goto _LL117;}_LL116: goto _LL118;_LL117: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp1CF=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp1A9;if(_tmp1CF->tag != 22)goto
_LL119;}_LL118: goto _LL11A;_LL119: {struct Cyc_Absyn_JoinEff_struct*_tmp1D0=(
struct Cyc_Absyn_JoinEff_struct*)_tmp1A9;if(_tmp1D0->tag != 24)goto _LL11B;}_LL11A:
goto _LL11C;_LL11B: {struct Cyc_Absyn_AccessEff_struct*_tmp1D1=(struct Cyc_Absyn_AccessEff_struct*)
_tmp1A9;if(_tmp1D1->tag != 23)goto _LL11D;}_LL11C: goto _LL11E;_LL11D: {struct Cyc_Absyn_ValueofType_struct*
_tmp1D2=(struct Cyc_Absyn_ValueofType_struct*)_tmp1A9;if(_tmp1D2->tag != 19)goto
_LL11F;}_LL11E: goto _LL120;_LL11F: {struct Cyc_Absyn_RgnsEff_struct*_tmp1D3=(
struct Cyc_Absyn_RgnsEff_struct*)_tmp1A9;if(_tmp1D3->tag != 25)goto _LL121;}_LL120:
return 0;_LL121: {struct Cyc_Absyn_TypedefType_struct*_tmp1D4=(struct Cyc_Absyn_TypedefType_struct*)
_tmp1A9;if(_tmp1D4->tag != 18)goto _LLEA;else{_tmp1D5=_tmp1D4->f3;}}_LL122: if(
_tmp1D5 == 0  || _tmp1D5->kind == 0)({void*_tmp1DC=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp1DF;_tmp1DF.tag=0;_tmp1DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmp1DD[1]={& _tmp1DF};Cyc_aprintf(({const char*
_tmp1DE="szof typedeftype %s";_tag_dyneither(_tmp1DE,sizeof(char),20);}),
_tag_dyneither(_tmp1DD,sizeof(void*),1));});}),_tag_dyneither(_tmp1DC,sizeof(
void*),0));});return(enum Cyc_Absyn_Kind)((struct Cyc_Core_Opt*)_check_null(
_tmp1D5->kind))->v == Cyc_Absyn_BoxKind;_LLEA:;}
