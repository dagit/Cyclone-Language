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
void*v;};struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[
17];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};
extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_NewRegion Cyc_Core_new_dynregion();
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
void Cyc_Core_free_dynregion(struct _DynRegionHandle*);struct Cyc___cycFILE;struct
Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr
f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char
Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(
struct Cyc_Lexing_lexbuf*);int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);void Cyc_Set_iter(void(*f)(void*),
struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};unsigned int Cyc_strlen(struct _dyneither_ptr s);int Cyc_zstrptrcmp(
struct _dyneither_ptr*,struct _dyneither_ptr*);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _dyneither_ptr Cyc_zstrncpy(
struct _dyneither_ptr,struct _dyneither_ptr,unsigned int);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned int n);struct Cyc_Xarray_Xarray{struct
_RegionHandle*r;struct _dyneither_ptr elmts;int num_elmts;};void*Cyc_Xarray_get(
struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct
_RegionHandle*,int,void*);void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);int
Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*,struct _dyneither_ptr);struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[
10];struct Cyc_Position_Nocontext_struct{char*tag;};void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
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
char*tag;};int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,
Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple9{
struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple9*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple10{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple10*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple11{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple11*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct
_tuple12{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope
val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};
struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple13{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple7*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple14{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple14*val;};
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
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};extern
struct Cyc_Yyltype Cyc_yylloc;extern union Cyc_YYSTYPE Cyc_yylval;struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict
Cyc_Dict_empty(int(*cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_insert(struct
Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
struct _tuple15{void*f1;void*f2;};struct _tuple15*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple15*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_yyerror(struct _dyneither_ptr s){Cyc_Position_post_error(Cyc_Position_mk_err_parse(
Cyc_Position_segment_of_abs(Cyc_yylloc.first_line,Cyc_yylloc.last_line),s));}
struct Cyc_Lex_Trie;struct _union_TrieChildren_Zero{int tag;int val;};struct _tuple16{
int f1;struct Cyc_Lex_Trie*f2;};struct _union_TrieChildren_One{int tag;struct
_tuple16 val;};struct _union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};
union Cyc_Lex_TrieChildren{struct _union_TrieChildren_Zero Zero;struct
_union_TrieChildren_One One;struct _union_TrieChildren_Many Many;};union Cyc_Lex_TrieChildren
Cyc_Lex_Zero(){return({union Cyc_Lex_TrieChildren _tmp0;(_tmp0.Zero).val=0;(_tmp0.Zero).tag=
1;_tmp0;});}union Cyc_Lex_TrieChildren Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){
return({union Cyc_Lex_TrieChildren _tmp1;(_tmp1.One).val=({struct _tuple16 _tmp2;
_tmp2.f1=i;_tmp2.f2=t;_tmp2;});(_tmp1.One).tag=2;_tmp1;});}union Cyc_Lex_TrieChildren
Cyc_Lex_Many(struct Cyc_Lex_Trie**ts){return({union Cyc_Lex_TrieChildren _tmp3;(
_tmp3.Many).val=ts;(_tmp3.Many).tag=3;_tmp3;});}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren
children;int shared_str;};static int Cyc_Lex_num_kws=0;static struct _dyneither_ptr
Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};struct Cyc_Lex_DynTrie{struct
_DynRegionHandle*dyn;struct Cyc_Lex_Trie*t;};struct Cyc_Lex_DynTrieSymbols{struct
_DynRegionHandle*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};
static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=0;static struct Cyc_Lex_DynTrie*
Cyc_Lex_typedefs_trie=0;static int Cyc_Lex_comment_depth=0;static struct _tuple5 Cyc_Lex_token_int={
Cyc_Absyn_Signed,0};static char _tmp4[8]="*bogus*";static struct _dyneither_ptr Cyc_Lex_bogus_string={
_tmp4,_tmp4,_tmp4 + 8};static struct _tuple0 Cyc_Lex_token_id_pair={{.Loc_n={3,0}},&
Cyc_Lex_bogus_string};static char Cyc_Lex_token_char='\000';static char _tmp5[1]="";
static struct _dyneither_ptr Cyc_Lex_token_string={_tmp5,_tmp5,_tmp5 + 1};static
struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;static int Cyc_Lex_runaway_start=
0;static void Cyc_Lex_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*lb){
struct Cyc_Position_Segment*s=Cyc_Position_segment_of_abs(Cyc_Lexing_lexeme_start(
lb),Cyc_Lexing_lexeme_end(lb));Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,
msg));}static void Cyc_Lex_runaway_err(struct _dyneither_ptr msg,struct Cyc_Lexing_lexbuf*
lb){struct Cyc_Position_Segment*s=Cyc_Position_segment_of_abs(Cyc_Lex_runaway_start,
Cyc_Lexing_lexeme_start(lb));Cyc_Position_post_error(Cyc_Position_mk_err_lex(s,
msg));}struct _tuple17{struct _dyneither_ptr f1;short f2;int f3;};static char _tmp6[14]="__attribute__";
static char _tmp7[9]="abstract";static char _tmp8[6]="alias";static char _tmp9[5]="auto";
static char _tmpA[6]="break";static char _tmpB[7]="calloc";static char _tmpC[5]="case";
static char _tmpD[6]="catch";static char _tmpE[5]="char";static char _tmpF[6]="const";
static char _tmp10[9]="continue";static char _tmp11[9]="datatype";static char _tmp12[8]="default";
static char _tmp13[3]="do";static char _tmp14[7]="double";static char _tmp15[12]="dynregion_t";
static char _tmp16[5]="else";static char _tmp17[5]="enum";static char _tmp18[7]="export";
static char _tmp19[7]="extern";static char _tmp1A[9]="fallthru";static char _tmp1B[6]="float";
static char _tmp1C[4]="for";static char _tmp1D[6]="__gen";static char _tmp1E[5]="goto";
static char _tmp1F[3]="if";static char _tmp20[7]="inline";static char _tmp21[11]="__inline__";
static char _tmp22[4]="int";static char _tmp23[4]="let";static char _tmp24[5]="long";
static char _tmp25[7]="malloc";static char _tmp26[10]="namespace";static char _tmp27[4]="new";
static char _tmp28[11]="NOZEROTERM";static char _tmp29[5]="NULL";static char _tmp2A[8]="numelts";
static char _tmp2B[9]="offsetof";static char _tmp2C[20]="__cyclone_port_on__";static
char _tmp2D[21]="__cyclone_port_off__";static char _tmp2E[8]="rcalloc";static char
_tmp2F[9]="region_t";static char _tmp30[7]="region";static char _tmp31[8]="regions";
static char _tmp32[9]="register";static char _tmp33[13]="reset_region";static char
_tmp34[9]="restrict";static char _tmp35[7]="return";static char _tmp36[8]="rmalloc";
static char _tmp37[5]="rnew";static char _tmp38[6]="short";static char _tmp39[7]="signed";
static char _tmp3A[7]="sizeof";static char _tmp3B[7]="static";static char _tmp3C[7]="struct";
static char _tmp3D[7]="switch";static char _tmp3E[9]="tagcheck";static char _tmp3F[6]="tag_t";
static char _tmp40[6]="throw";static char _tmp41[4]="try";static char _tmp42[7]="tunion";
static char _tmp43[8]="typedef";static char _tmp44[6]="union";static char _tmp45[9]="unsigned";
static char _tmp46[6]="using";static char _tmp47[8]="valueof";static char _tmp48[10]="valueof_t";
static char _tmp49[5]="void";static char _tmp4A[9]="volatile";static char _tmp4B[6]="while";
static char _tmp4C[8]="xtunion";static char _tmp4D[9]="ZEROTERM";static struct
_tuple17 Cyc_Lex_rw_array[72]={{{_tmp6,_tmp6,_tmp6 + 14},371,1},{{_tmp7,_tmp7,
_tmp7 + 9},300,0},{{_tmp8,_tmp8,_tmp8 + 6},325,0},{{_tmp9,_tmp9,_tmp9 + 5},258,1},{{
_tmpA,_tmpA,_tmpA + 6},290,1},{{_tmpB,_tmpB,_tmpB + 7},309,0},{{_tmpC,_tmpC,_tmpC + 
5},277,1},{{_tmpD,_tmpD,_tmpD + 6},297,1},{{_tmpE,_tmpE,_tmpE + 5},264,1},{{_tmpF,
_tmpF,_tmpF + 6},272,1},{{_tmp10,_tmp10,_tmp10 + 9},289,1},{{_tmp11,_tmp11,_tmp11 + 
9},304,0},{{_tmp12,_tmp12,_tmp12 + 8},278,1},{{_tmp13,_tmp13,_tmp13 + 3},286,1},{{
_tmp14,_tmp14,_tmp14 + 7},269,1},{{_tmp15,_tmp15,_tmp15 + 12},324,0},{{_tmp16,
_tmp16,_tmp16 + 5},283,1},{{_tmp17,_tmp17,_tmp17 + 5},292,1},{{_tmp18,_tmp18,
_tmp18 + 7},298,0},{{_tmp19,_tmp19,_tmp19 + 7},261,1},{{_tmp1A,_tmp1A,_tmp1A + 9},
301,0},{{_tmp1B,_tmp1B,_tmp1B + 6},268,1},{{_tmp1C,_tmp1C,_tmp1C + 4},287,1},{{
_tmp1D,_tmp1D,_tmp1D + 6},318,0},{{_tmp1E,_tmp1E,_tmp1E + 5},288,1},{{_tmp1F,
_tmp1F,_tmp1F + 3},282,1},{{_tmp20,_tmp20,_tmp20 + 7},279,1},{{_tmp21,_tmp21,
_tmp21 + 11},279,1},{{_tmp22,_tmp22,_tmp22 + 4},266,1},{{_tmp23,_tmp23,_tmp23 + 4},
294,0},{{_tmp24,_tmp24,_tmp24 + 5},267,1},{{_tmp25,_tmp25,_tmp25 + 7},307,0},{{
_tmp26,_tmp26,_tmp26 + 10},303,0},{{_tmp27,_tmp27,_tmp27 + 4},299,0},{{_tmp28,
_tmp28,_tmp28 + 11},319,0},{{_tmp29,_tmp29,_tmp29 + 5},293,0},{{_tmp2A,_tmp2A,
_tmp2A + 8},326,0},{{_tmp2B,_tmp2B,_tmp2B + 9},281,1},{{_tmp2C,_tmp2C,_tmp2C + 20},
322,0},{{_tmp2D,_tmp2D,_tmp2D + 21},323,0},{{_tmp2E,_tmp2E,_tmp2E + 8},310,0},{{
_tmp2F,_tmp2F,_tmp2F + 9},312,0},{{_tmp30,_tmp30,_tmp30 + 7},314,0},{{_tmp31,
_tmp31,_tmp31 + 8},316,0},{{_tmp32,_tmp32,_tmp32 + 9},259,1},{{_tmp33,_tmp33,
_tmp33 + 13},317,0},{{_tmp34,_tmp34,_tmp34 + 9},274,1},{{_tmp35,_tmp35,_tmp35 + 7},
291,1},{{_tmp36,_tmp36,_tmp36 + 8},308,0},{{_tmp37,_tmp37,_tmp37 + 5},315,0},{{
_tmp38,_tmp38,_tmp38 + 6},265,1},{{_tmp39,_tmp39,_tmp39 + 7},270,1},{{_tmp3A,
_tmp3A,_tmp3A + 7},280,1},{{_tmp3B,_tmp3B,_tmp3B + 7},260,1},{{_tmp3C,_tmp3C,
_tmp3C + 7},275,1},{{_tmp3D,_tmp3D,_tmp3D + 7},284,1},{{_tmp3E,_tmp3E,_tmp3E + 9},
329,0},{{_tmp3F,_tmp3F,_tmp3F + 6},313,0},{{_tmp40,_tmp40,_tmp40 + 6},295,0},{{
_tmp41,_tmp41,_tmp41 + 4},296,0},{{_tmp42,_tmp42,_tmp42 + 7},306,0},{{_tmp43,
_tmp43,_tmp43 + 8},262,1},{{_tmp44,_tmp44,_tmp44 + 6},276,1},{{_tmp45,_tmp45,
_tmp45 + 9},271,1},{{_tmp46,_tmp46,_tmp46 + 6},302,0},{{_tmp47,_tmp47,_tmp47 + 8},
327,0},{{_tmp48,_tmp48,_tmp48 + 10},328,0},{{_tmp49,_tmp49,_tmp49 + 5},263,1},{{
_tmp4A,_tmp4A,_tmp4A + 9},273,1},{{_tmp4B,_tmp4B,_tmp4B + 6},285,1},{{_tmp4C,
_tmp4C,_tmp4C + 8},305,0},{{_tmp4D,_tmp4D,_tmp4D + 9},320,0}};static int Cyc_Lex_num_keywords(
int include_cyclone_keywords){int sum=0;{unsigned int i=0;for(0;i < 72;++ i){if(
include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)i]).f3)++ sum;}}return sum;}
static int Cyc_Lex_trie_char(int c){if(c >= 95)return c - 59;else{if(c > 64)return c - 55;}
return c - 48;}static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,
struct Cyc_Lex_Trie*t,struct _dyneither_ptr buff,int offset,int len){int i=offset;int
last=(offset + len)- 1;while(i <= last){union Cyc_Lex_TrieChildren _tmp4E=((struct
Cyc_Lex_Trie*)_check_null(t))->children;struct Cyc_Lex_Trie**_tmp4F;struct
_tuple16 _tmp50;int _tmp51;struct Cyc_Lex_Trie*_tmp52;int _tmp53;_LL1: if((_tmp4E.Many).tag
!= 3)goto _LL3;_tmp4F=(struct Cyc_Lex_Trie**)(_tmp4E.Many).val;_LL2: {int ch=Cyc_Lex_trie_char((
int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),i)));if(_tmp4F[
_check_known_subscript_notnull(64,ch)]== 0)_tmp4F[_check_known_subscript_notnull(
64,ch)]=({struct Cyc_Lex_Trie*_tmp54=_region_malloc(r,sizeof(*_tmp54));_tmp54->children=
Cyc_Lex_Zero();_tmp54->shared_str=0;_tmp54;});t=_tmp4F[
_check_known_subscript_notnull(64,ch)];++ i;goto _LL0;}_LL3: if((_tmp4E.One).tag != 
2)goto _LL5;_tmp50=(struct _tuple16)(_tmp4E.One).val;_tmp51=_tmp50.f1;_tmp52=
_tmp50.f2;_LL4: if(_tmp51 == *((const char*)_check_dyneither_subscript(buff,sizeof(
char),i)))t=_tmp52;else{struct Cyc_Lex_Trie**_tmp55=({unsigned int _tmp58=64;
struct Cyc_Lex_Trie**_tmp59=(struct Cyc_Lex_Trie**)_region_malloc(r,_check_times(
sizeof(struct Cyc_Lex_Trie*),_tmp58));{unsigned int _tmp5A=_tmp58;unsigned int j;
for(j=0;j < _tmp5A;j ++){_tmp59[j]=0;}}_tmp59;});_tmp55[
_check_known_subscript_notnull(64,Cyc_Lex_trie_char(_tmp51))]=_tmp52;{int _tmp56=
Cyc_Lex_trie_char((int)*((const char*)_check_dyneither_subscript(buff,sizeof(char),
i)));_tmp55[_check_known_subscript_notnull(64,_tmp56)]=({struct Cyc_Lex_Trie*
_tmp57=_region_malloc(r,sizeof(*_tmp57));_tmp57->children=Cyc_Lex_Zero();_tmp57->shared_str=
0;_tmp57;});t->children=Cyc_Lex_Many(_tmp55);t=_tmp55[
_check_known_subscript_notnull(64,_tmp56)];}}++ i;goto _LL0;_LL5: if((_tmp4E.Zero).tag
!= 1)goto _LL0;_tmp53=(int)(_tmp4E.Zero).val;_LL6: while(i <= last){struct Cyc_Lex_Trie*
_tmp5B=({struct Cyc_Lex_Trie*_tmp5C=_region_malloc(r,sizeof(*_tmp5C));_tmp5C->children=
Cyc_Lex_Zero();_tmp5C->shared_str=0;_tmp5C;});((struct Cyc_Lex_Trie*)_check_null(
t))->children=Cyc_Lex_One((int)*((const char*)_check_dyneither_subscript(buff,
sizeof(char),i ++)),_tmp5B);t=_tmp5B;}return t;_LL0:;}return t;}static int Cyc_Lex_str_index(
struct _dyneither_ptr buff,int offset,int len){struct _DynRegionHandle*_tmp5E;struct
Cyc_Lex_Trie*_tmp5F;struct Cyc_Xarray_Xarray*_tmp60;struct Cyc_Lex_DynTrieSymbols
_tmp5D=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(Cyc_Lex_ids_trie));_tmp5E=
_tmp5D.dyn;_tmp5F=_tmp5D.t;_tmp60=_tmp5D.symbols;{struct _DynRegionFrame _tmp61;
struct _RegionHandle*d=_open_dynregion(& _tmp61,_tmp5E);{struct Cyc_Lex_Trie*_tmp62=
Cyc_Lex_trie_lookup(d,_tmp5F,buff,offset,len);if(_tmp62->shared_str == 0){struct
_dyneither_ptr _tmp63=Cyc_Core_new_string((unsigned int)(len + 1));Cyc_zstrncpy(
_dyneither_ptr_decrease_size(_tmp63,sizeof(char),1),(struct _dyneither_ptr)
_dyneither_ptr_plus(buff,sizeof(char),offset),(unsigned int)len);{int ans=((int(*)(
struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp60,({
struct _dyneither_ptr*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=(struct
_dyneither_ptr)_tmp63;_tmp64;}));_tmp62->shared_str=ans;}}{int _tmp65=_tmp62->shared_str;
_npop_handler(0);return _tmp65;}};_pop_dynregion(d);}}static int Cyc_Lex_str_index_lbuf(
struct Cyc_Lexing_lexbuf*lbuf){return Cyc_Lex_str_index((struct _dyneither_ptr)lbuf->lex_buffer,
lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}static void Cyc_Lex_insert_typedef(
struct _dyneither_ptr*sptr){struct _dyneither_ptr _tmp66=*sptr;struct
_DynRegionHandle*_tmp68;struct Cyc_Lex_Trie*_tmp69;struct Cyc_Lex_DynTrie _tmp67=*((
struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));_tmp68=_tmp67.dyn;
_tmp69=_tmp67.t;{struct _DynRegionFrame _tmp6A;struct _RegionHandle*d=
_open_dynregion(& _tmp6A,_tmp68);{struct Cyc_Lex_Trie*_tmp6B=Cyc_Lex_trie_lookup(d,
_tmp69,_tmp66,0,(int)(_get_dyneither_size(_tmp66,sizeof(char))- 1));((struct Cyc_Lex_Trie*)
_check_null(_tmp6B))->shared_str=1;}_npop_handler(0);return;;_pop_dynregion(d);}}
static struct _dyneither_ptr*Cyc_Lex_get_symbol(int symbol_num){struct
_DynRegionHandle*_tmp6D;struct Cyc_Lex_Trie*_tmp6E;struct Cyc_Xarray_Xarray*_tmp6F;
struct Cyc_Lex_DynTrieSymbols _tmp6C=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(
Cyc_Lex_ids_trie));_tmp6D=_tmp6C.dyn;_tmp6E=_tmp6C.t;_tmp6F=_tmp6C.symbols;{
struct _DynRegionFrame _tmp70;struct _RegionHandle*d=_open_dynregion(& _tmp70,_tmp6D);{
struct _dyneither_ptr*_tmp71=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp6F,symbol_num);_npop_handler(0);return _tmp71;};
_pop_dynregion(d);}}static int Cyc_Lex_int_of_char(char c){if('0' <= c  && c <= '9')
return c - '0';else{if('a' <= c  && c <= 'f')return(10 + c)- 'a';else{if('A' <= c  && c
<= 'F')return(10 + c)- 'A';else{(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp73;_tmp73.tag=Cyc_Core_Invalid_argument;_tmp73.f1=({const char*_tmp74="string to integer conversion";
_tag_dyneither(_tmp74,sizeof(char),29);});_tmp73;});_tmp72;}));}}}}static struct
_tuple5 Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int base){
unsigned int n=0;int end2=lbuf->lex_curr_pos - end;struct _dyneither_ptr buff=lbuf->lex_buffer;
int i=start + lbuf->lex_start_pos;{int i=start + lbuf->lex_start_pos;for(0;i < end2;
++ i){char c=*((char*)_check_dyneither_subscript(buff,sizeof(char),i));switch(c){
case 'u': _LL7: goto _LL8;case 'U': _LL8: return({struct _tuple5 _tmp75;_tmp75.f1=Cyc_Absyn_Unsigned;
_tmp75.f2=(int)n;_tmp75;});case 'l': _LL9: break;case 'L': _LLA: break;default: _LLB: n=
n * base + (unsigned int)Cyc_Lex_int_of_char(c);break;}}}return({struct _tuple5
_tmp76;_tmp76.f1=Cyc_Absyn_Signed;_tmp76.f2=(int)n;_tmp76;});}char Cyc_Lex_string_buffer_v[
11]={'x','x','x','x','x','x','x','x','x','x','\000'};struct _dyneither_ptr Cyc_Lex_string_buffer={(
void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v
+ 11)};int Cyc_Lex_string_pos=0;void Cyc_Lex_store_string_char(char c){int sz=(int)(
_get_dyneither_size(Cyc_Lex_string_buffer,sizeof(char))- 1);if(Cyc_Lex_string_pos
>= sz){int newsz=sz;while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{struct
_dyneither_ptr str=({unsigned int _tmp77=(unsigned int)newsz;char*_tmp78=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp77 + 1));struct _dyneither_ptr
_tmp7A=_tag_dyneither(_tmp78,sizeof(char),_tmp77 + 1);{unsigned int _tmp79=_tmp77;
unsigned int i;for(i=0;i < _tmp79;i ++){_tmp78[i]=i < sz?*((char*)
_check_dyneither_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)i)):'\000';}
_tmp78[_tmp79]=(char)0;}_tmp7A;});Cyc_Lex_string_buffer=str;}}({struct
_dyneither_ptr _tmp7B=_dyneither_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos);
char _tmp7C=*((char*)_check_dyneither_subscript(_tmp7B,sizeof(char),0));char
_tmp7D=c;if(_get_dyneither_size(_tmp7B,sizeof(char))== 1  && (_tmp7C == '\000'  && 
_tmp7D != '\000'))_throw_arraybounds();*((char*)_tmp7B.curr)=_tmp7D;});++ Cyc_Lex_string_pos;}
struct _dyneither_ptr Cyc_Lex_get_stored_string(){struct _dyneither_ptr str=Cyc_substring((
struct _dyneither_ptr)Cyc_Lex_string_buffer,0,(unsigned int)Cyc_Lex_string_pos);
Cyc_Lex_string_pos=0;return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;
struct Cyc_Set_Set*namespaces;};struct Cyc_Lex_Lvis{struct Cyc_List_List*
current_namespace;struct Cyc_List_List*imported_namespaces;};struct Cyc_Lex_Lstate{
struct Cyc_List_List*lstack;struct Cyc_Dict_Dict decls;};static struct Cyc_Core_Opt*
Cyc_Lex_lstate=0;static void Cyc_Lex_typedef_init(){struct Cyc_Lex_Lvis*_tmp7E=({
struct Cyc_Lex_Lvis*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->current_namespace=0;
_tmp84->imported_namespaces=0;_tmp84;});struct Cyc_List_List*_tmp7F=({struct Cyc_List_List*
_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->hd=_tmp7E;_tmp83->tl=0;_tmp83;});
struct Cyc_Dict_Dict init_decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(
int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),
0,({struct Cyc_Lex_Ldecls*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->typedefs=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_empty)(Cyc_zstrptrcmp);_tmp82->namespaces=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);
_tmp82;}));Cyc_Lex_lstate=({struct Cyc_Core_Opt*_tmp80=_cycalloc(sizeof(*_tmp80));
_tmp80->v=({struct Cyc_Lex_Lstate*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->lstack=
_tmp7F;_tmp81->decls=init_decls;_tmp81;});_tmp80;});}static struct Cyc_List_List*
Cyc_Lex_get_absolute_namespace(struct Cyc_List_List*ns){struct _dyneither_ptr*n=(
struct _dyneither_ptr*)ns->hd;{struct Cyc_List_List*ls=(struct Cyc_List_List*)((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;
for(0;ls != 0;ls=ls->tl){struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct
Cyc_List_List*x=({struct Cyc_List_List*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->hd=
lv->current_namespace;_tmp85->tl=lv->imported_namespaces;_tmp85;});for(0;x != 0;x=((
struct Cyc_List_List*)_check_null(x))->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd);if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(ld->namespaces,n))return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd,(struct Cyc_List_List*)ns);}}}Cyc_yyerror((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp88;_tmp88.tag=0;_tmp88.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_str_sepstr((struct Cyc_List_List*)
ns,({const char*_tmp89="::";_tag_dyneither(_tmp89,sizeof(char),3);})));{void*
_tmp86[1]={& _tmp88};Cyc_aprintf(({const char*_tmp87="undeclared namespace %s";
_tag_dyneither(_tmp87,sizeof(char),24);}),_tag_dyneither(_tmp86,sizeof(void*),1));}}));
return 0;}static void Cyc_Lex_recompute_typedefs(){struct _DynRegionHandle*_tmp8B;
struct Cyc_Lex_Trie*_tmp8C;struct Cyc_Lex_DynTrie _tmp8A=*((struct Cyc_Lex_DynTrie*)
_check_null(Cyc_Lex_typedefs_trie));_tmp8B=_tmp8A.dyn;_tmp8C=_tmp8A.t;Cyc_Core_free_dynregion(
_tmp8B);{struct _DynRegionHandle*_tmp8E;struct Cyc_Core_NewRegion _tmp8D=Cyc_Core_new_dynregion();
_tmp8E=_tmp8D.dynregion;{struct _DynRegionFrame _tmp8F;struct _RegionHandle*d2=
_open_dynregion(& _tmp8F,_tmp8E);Cyc_Lex_typedefs_trie=({struct Cyc_Lex_DynTrie*
_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->dyn=(struct _DynRegionHandle*)_tmp8E;
_tmp90->t=(struct Cyc_Lex_Trie*)({struct Cyc_Lex_Trie*_tmp91=_region_malloc(d2,
sizeof(*_tmp91));_tmp91->children=Cyc_Lex_Zero();_tmp91->shared_str=0;_tmp91;});
_tmp90;});{struct Cyc_List_List*ls=(struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack;for(0;ls != 0;ls=ls->tl){
struct Cyc_Lex_Lvis*lv=(struct Cyc_Lex_Lvis*)ls->hd;struct Cyc_List_List*x=({struct
Cyc_List_List*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->hd=lv->current_namespace;
_tmp92->tl=lv->imported_namespaces;_tmp92;});for(0;x != 0;x=((struct Cyc_List_List*)
_check_null(x))->tl){struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,(struct Cyc_List_List*)((
struct Cyc_List_List*)_check_null(x))->hd);((void(*)(void(*f)(struct
_dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ld->typedefs);}}};
_pop_dynregion(d2);}}}static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*
ns,struct _dyneither_ptr*v){struct Cyc_List_List*ans=Cyc_Lex_get_absolute_namespace(
ns);struct _handler_cons _tmp93;_push_handler(& _tmp93);{int _tmp95=0;if(setjmp(
_tmp93.handler))_tmp95=1;if(!_tmp95){{struct Cyc_Lex_Ldecls*ld=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,ans);int _tmp96=((int(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ld->typedefs,v);
_npop_handler(0);return _tmp96;};_pop_handler();}else{void*_tmp94=(void*)
_exn_thrown;void*_tmp98=_tmp94;_LLE: {struct Cyc_Dict_Absent_struct*_tmp99=(
struct Cyc_Dict_Absent_struct*)_tmp98;if(_tmp99->tag != Cyc_Dict_Absent)goto _LL10;}
_LLF: return 0;_LL10:;_LL11:(void)_throw(_tmp98);_LLD:;}}}static int Cyc_Lex_is_typedef(
struct Cyc_List_List*ns,struct _dyneither_ptr*v){if(ns != 0)return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*)ns,v);{struct _dyneither_ptr _tmp9A=*v;int len=(int)(
_get_dyneither_size(_tmp9A,sizeof(char))- 1);struct _DynRegionHandle*_tmp9C;
struct Cyc_Lex_Trie*_tmp9D;struct Cyc_Lex_DynTrie _tmp9B=*((struct Cyc_Lex_DynTrie*)
_check_null(Cyc_Lex_typedefs_trie));_tmp9C=_tmp9B.dyn;_tmp9D=_tmp9B.t;{struct
_DynRegionFrame _tmp9E;struct _RegionHandle*d=_open_dynregion(& _tmp9E,_tmp9C);{int
i=0;for(0;i < len;++ i){union Cyc_Lex_TrieChildren _tmp9F=((struct Cyc_Lex_Trie*)
_check_null(_tmp9D))->children;int _tmpA0;struct _tuple16 _tmpA1;int _tmpA2;struct
Cyc_Lex_Trie*_tmpA3;struct _tuple16 _tmpA4;struct Cyc_Lex_Trie*_tmpA5;struct Cyc_Lex_Trie**
_tmpA6;_LL13: if((_tmp9F.Zero).tag != 1)goto _LL15;_tmpA0=(int)(_tmp9F.Zero).val;
_LL14: {int _tmpA7=0;_npop_handler(0);return _tmpA7;}_LL15: if((_tmp9F.One).tag != 2)
goto _LL17;_tmpA1=(struct _tuple16)(_tmp9F.One).val;_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;
if(!(_tmpA2 != *((const char*)_check_dyneither_subscript(_tmp9A,sizeof(char),i))))
goto _LL17;_LL16: {int _tmpA8=0;_npop_handler(0);return _tmpA8;}_LL17: if((_tmp9F.One).tag
!= 2)goto _LL19;_tmpA4=(struct _tuple16)(_tmp9F.One).val;_tmpA5=_tmpA4.f2;_LL18:
_tmp9D=_tmpA5;goto _LL12;_LL19: if((_tmp9F.Many).tag != 3)goto _LL12;_tmpA6=(struct
Cyc_Lex_Trie**)(_tmp9F.Many).val;_LL1A: {struct Cyc_Lex_Trie*_tmpA9=_tmpA6[
_check_known_subscript_notnull(64,Cyc_Lex_trie_char((int)*((const char*)
_check_dyneither_subscript(_tmp9A,sizeof(char),i))))];if(_tmpA9 == 0){int _tmpAA=0;
_npop_handler(0);return _tmpAA;}_tmp9D=_tmpA9;goto _LL12;}_LL12:;}}{int _tmpAB=((
struct Cyc_Lex_Trie*)_check_null(_tmp9D))->shared_str;_npop_handler(0);return
_tmpAB;};_pop_dynregion(d);}}}void Cyc_Lex_enter_namespace(struct _dyneither_ptr*s){
struct Cyc_List_List*ns=((struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct
Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List*new_ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(ns,({struct Cyc_List_List*_tmpB0=
_cycalloc(sizeof(*_tmpB0));_tmpB0->hd=s;_tmpB0->tl=0;_tmpB0;}));((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=({struct Cyc_List_List*
_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->hd=({struct Cyc_Lex_Lvis*_tmpAD=
_cycalloc(sizeof(*_tmpAD));_tmpAD->current_namespace=new_ns;_tmpAD->imported_namespaces=
0;_tmpAD;});_tmpAC->tl=(struct Cyc_List_List*)((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->lstack;_tmpAC;});{struct Cyc_Lex_Ldecls*ld=((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
ns);if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
ld->namespaces,s)){((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,ns,({struct Cyc_Lex_Ldecls*_tmpAE=_cycalloc(sizeof(*
_tmpAE));_tmpAE->typedefs=ld->typedefs;_tmpAE->namespaces=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(ld->namespaces,s);
_tmpAE;}));((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*
v))Cyc_Dict_insert)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->decls,new_ns,({struct Cyc_Lex_Ldecls*_tmpAF=_cycalloc(
sizeof(*_tmpAF));_tmpAF->typedefs=((struct Cyc_Set_Set*(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);_tmpAF->namespaces=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))
Cyc_Set_empty)(Cyc_zstrptrcmp);_tmpAF;}));}((void(*)(void(*f)(struct
_dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
new_ns))->typedefs);}}void Cyc_Lex_leave_namespace(){((struct Cyc_Lex_Lstate*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack=(struct Cyc_List_List*)
_check_null((((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->tl);
Cyc_Lex_recompute_typedefs();}void Cyc_Lex_enter_using(struct _tuple0*q){struct Cyc_List_List*
ns;{union Cyc_Absyn_Nmspace _tmpB1=(*q).f1;int _tmpB2;struct Cyc_List_List*_tmpB3;
struct Cyc_List_List*_tmpB4;_LL1C: if((_tmpB1.Loc_n).tag != 3)goto _LL1E;_tmpB2=(int)(
_tmpB1.Loc_n).val;_LL1D: ns=(struct Cyc_List_List*)({struct Cyc_List_List*_tmpB5=
_cycalloc(sizeof(*_tmpB5));_tmpB5->hd=(*q).f2;_tmpB5->tl=0;_tmpB5;});goto _LL1B;
_LL1E: if((_tmpB1.Rel_n).tag != 1)goto _LL20;_tmpB3=(struct Cyc_List_List*)(_tmpB1.Rel_n).val;
_LL1F: _tmpB4=_tmpB3;goto _LL21;_LL20: if((_tmpB1.Abs_n).tag != 2)goto _LL1B;_tmpB4=(
struct Cyc_List_List*)(_tmpB1.Abs_n).val;_LL21: ns=(struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmpB4,({struct Cyc_List_List*_tmpB6=_cycalloc(sizeof(*_tmpB6));
_tmpB6->hd=(*q).f2;_tmpB6->tl=0;_tmpB6;})));goto _LL1B;_LL1B:;}{struct Cyc_List_List*
_tmpB7=Cyc_Lex_get_absolute_namespace(ns);struct Cyc_List_List*_tmpB8=((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=({struct Cyc_List_List*
_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->hd=_tmpB7;_tmpB9->tl=_tmpB8;_tmpB9;});((
void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,(((
struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->decls,
_tmpB7))->typedefs);}}void Cyc_Lex_leave_using(){struct Cyc_List_List*_tmpBA=((
struct Cyc_Lex_Lvis*)(((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(
Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces;((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->imported_namespaces=((
struct Cyc_List_List*)_check_null(_tmpBA))->tl;Cyc_Lex_recompute_typedefs();}void
Cyc_Lex_register_typedef(struct _tuple0*q){struct Cyc_List_List*_tmpBB=((struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict _tmpBC=((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls;struct Cyc_Lex_Ldecls*_tmpBD=((struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpBC,_tmpBB);
struct Cyc_Lex_Ldecls*_tmpBE=({struct Cyc_Lex_Ldecls*_tmpBF=_cycalloc(sizeof(*
_tmpBF));_tmpBF->typedefs=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_insert)(_tmpBD->typedefs,(*q).f2);_tmpBF->namespaces=
_tmpBD->namespaces;_tmpBF;});((struct Cyc_Lex_Lstate*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Lex_lstate))->v)->decls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Lex_Ldecls*v))Cyc_Dict_insert)(_tmpBC,_tmpBB,
_tmpBE);Cyc_Lex_insert_typedef((*q).f2);}static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*
lbuf){int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);if(symbol_num <= Cyc_Lex_num_kws){
short res=(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),
symbol_num - 1));switch(res){case 320: _LL22:({void*_tmpC0=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs(
Cyc_yylloc.first_line,Cyc_yylloc.last_line),({const char*_tmpC1="deprecated ZEROTERM, use @zeroterm";
_tag_dyneither(_tmpC1,sizeof(char),35);}),_tag_dyneither(_tmpC0,sizeof(void*),0));});
break;case 319: _LL23:({void*_tmpC2=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs(
Cyc_yylloc.first_line,Cyc_yylloc.last_line),({const char*_tmpC3="deprecated NOZEROTERM, use @nozeroterm";
_tag_dyneither(_tmpC3,sizeof(char),39);}),_tag_dyneither(_tmpC2,sizeof(void*),0));});
break;case 306: _LL24:({void*_tmpC4=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs(
Cyc_yylloc.first_line,Cyc_yylloc.last_line),({const char*_tmpC5="deprecated tunion, use datatype";
_tag_dyneither(_tmpC5,sizeof(char),32);}),_tag_dyneither(_tmpC4,sizeof(void*),0));});
break;case 305: _LL25:({void*_tmpC6=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs(
Cyc_yylloc.first_line,Cyc_yylloc.last_line),({const char*_tmpC7="deprecated xtunion, use @extensible datatype";
_tag_dyneither(_tmpC7,sizeof(char),45);}),_tag_dyneither(_tmpC6,sizeof(void*),0));});
break;default: _LL26: break;}return(short)*((int*)_check_dyneither_subscript(Cyc_Lex_kw_nums,
sizeof(int),symbol_num - 1));}{struct _dyneither_ptr*_tmpC8=Cyc_Lex_get_symbol(
symbol_num);Cyc_Lex_token_string=*_tmpC8;if(Cyc_Lex_is_typedef(0,_tmpC8))return
368;return 362;}}static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
int i=lbuf->lex_start_pos;int end=lbuf->lex_curr_pos;struct _dyneither_ptr s=lbuf->lex_buffer;
struct Cyc_List_List*rev_vs=0;while(i < end){int start=i;for(0;i < end  && *((char*)
_check_dyneither_subscript(s,sizeof(char),i))!= ':';++ i){;}if(start == i)(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));
_tmpC9[0]=({struct Cyc_Core_Impossible_struct _tmpCA;_tmpCA.tag=Cyc_Core_Impossible;
_tmpCA.f1=({const char*_tmpCB="bad namespace";_tag_dyneither(_tmpCB,sizeof(char),
14);});_tmpCA;});_tmpC9;}));{int vlen=i - start;struct _dyneither_ptr*v=Cyc_Lex_get_symbol(
Cyc_Lex_str_index((struct _dyneither_ptr)s,start,vlen));rev_vs=({struct Cyc_List_List*
_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->hd=v;_tmpCC->tl=rev_vs;_tmpCC;});i +=2;}}
if(rev_vs == 0)(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpCD=
_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Core_Impossible_struct _tmpCE;
_tmpCE.tag=Cyc_Core_Impossible;_tmpCE.f1=({const char*_tmpCF="bad namespace";
_tag_dyneither(_tmpCF,sizeof(char),14);});_tmpCE;});_tmpCD;}));{struct
_dyneither_ptr*v=(struct _dyneither_ptr*)rev_vs->hd;struct Cyc_List_List*vs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs->tl);Cyc_Lex_token_qvar=({
struct _tuple0*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->f1=Cyc_Absyn_Rel_n(vs);
_tmpD0->f2=v;_tmpD0;});if(Cyc_Lex_is_typedef(vs,v))return 370;return 369;}}int Cyc_Lex_token(
struct Cyc_Lexing_lexbuf*);int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*);int Cyc_Lex_cyc_qual(struct Cyc_Lexing_lexbuf*);static
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr s){return({union Cyc_YYSTYPE
_tmpD1;(_tmpD1.String_tok).val=s;(_tmpD1.String_tok).tag=3;_tmpD1;});}static
union Cyc_YYSTYPE Cyc_Stringopt_tok(struct Cyc_Core_Opt*s){return({union Cyc_YYSTYPE
_tmpD2;(_tmpD2.Stringopt_tok).val=s;(_tmpD2.Stringopt_tok).tag=4;_tmpD2;});}
static union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 s){return({union Cyc_YYSTYPE _tmpD3;(
_tmpD3.Int_tok).val=s;(_tmpD3.Int_tok).tag=1;_tmpD3;});}static union Cyc_YYSTYPE
Cyc_Char_tok(char s){return({union Cyc_YYSTYPE _tmpD4;(_tmpD4.Char_tok).val=s;(
_tmpD4.Char_tok).tag=2;_tmpD4;});}static union Cyc_YYSTYPE Cyc_QualId_tok(struct
_tuple0*s){return({union Cyc_YYSTYPE _tmpD5;(_tmpD5.QualId_tok).val=s;(_tmpD5.QualId_tok).tag=
5;_tmpD5;});}int Cyc_yylex(struct Cyc_Lexing_lexbuf*lbuf){int ans=((int(*)(struct
Cyc_Lexing_lexbuf*))Cyc_Lex_token)(lbuf);Cyc_yylloc.first_line=((int(*)(struct
Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(lbuf);Cyc_yylloc.last_line=((int(*)(
struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(lbuf);switch(ans){case 362: _LL28:
Cyc_yylval=Cyc_String_tok(Cyc_Lex_token_string);break;case 368: _LL29: Cyc_yylval=
Cyc_String_tok(Cyc_Lex_token_string);break;case 369: _LL2A: Cyc_yylval=Cyc_QualId_tok(
Cyc_Lex_token_qvar);break;case 370: _LL2B: Cyc_yylval=Cyc_QualId_tok(Cyc_Lex_token_qvar);
break;case 367: _LL2C: Cyc_yylval=Cyc_String_tok(Cyc_Lex_token_string);break;case
363: _LL2D: Cyc_yylval=Cyc_Int_tok(Cyc_Lex_token_int);break;case 365: _LL2E: Cyc_yylval=
Cyc_Char_tok(Cyc_Lex_token_char);break;case 366: _LL2F: Cyc_yylval=Cyc_String_tok(
Cyc_Lex_token_string);break;case 364: _LL30: Cyc_yylval=Cyc_String_tok(Cyc_Lex_token_string);
break;default: _LL31: break;}return ans;}const int Cyc_Lex_lex_base[225]=(const int[
225]){0,24,117,81,89,12,2,- 3,- 1,- 2,- 17,- 18,4,132,119,- 19,5,- 13,- 12,135,- 14,- 11,-
4,- 5,- 6,- 7,- 8,- 9,- 10,192,216,143,- 15,146,- 18,- 23,13,38,26,38,50,43,34,38,34,51,
43,51,309,58,57,53,384,62,74,76,459,97,72,78,65,102,103,94,102,534,93,95,609,91,
106,98,107,102,102,104,684,123,124,115,123,759,115,125,113,117,114,130,118,125,
834,119,130,142,909,135,984,136,152,144,140,158,176,167,175,1059,- 57,8,2,- 41,294,
30,239,1111,31,235,250,273,259,1136,1179,241,248,32,226,- 5,1271,218,1346,1404,
244,- 56,- 29,1479,1554,- 24,1629,239,1687,1762,1837,240,242,- 27,270,- 32,- 26,- 35,
274,- 36,1912,1941,298,250,262,1217,1951,1981,1249,1259,2014,2045,2083,254,264,
2153,2191,284,367,359,369,361,372,- 7,- 40,957,- 21,- 39,6,287,2123,- 34,- 17,- 33,- 16,
- 20,2,2231,295,297,2261,303,305,317,361,363,364,366,403,408,2304,2388,- 54,- 48,-
47,- 46,- 45,- 44,- 43,- 42,- 49,- 52,- 53,2472,478,- 50,- 51,- 55,- 28,- 25,- 22,958,- 37,12,
1185};const int Cyc_Lex_lex_backtrk[225]=(const int[225]){- 1,- 1,- 1,- 1,5,3,4,- 1,- 1,-
1,- 1,- 1,16,1,19,- 1,2,- 1,- 1,14,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,14,- 1,- 1,- 1,- 1,22,
22,22,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,12,- 1,- 1,- 1,18,- 1,- 1,- 1,11,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,15,- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,13,
- 1,- 1,- 1,20,- 1,19,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,14,- 1,37,56,- 1,56,56,56,56,56,56,56,56,
56,6,8,56,56,56,56,- 1,0,56,56,56,56,- 1,- 1,3,1,- 1,0,- 1,- 1,2,2,- 1,30,- 1,29,- 1,- 1,-
1,- 1,- 1,10,8,- 1,8,8,- 1,9,10,- 1,- 1,10,6,7,6,6,- 1,5,5,5,7,7,6,6,- 1,- 1,- 1,- 1,- 1,38,
- 1,10,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,36,37};const int Cyc_Lex_lex_default[
225]=(const int[225]){106,35,10,4,4,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,0,- 1,0,0,- 1,0,0,0,0,
0,0,0,0,0,- 1,- 1,- 1,0,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,
- 1,- 1,0,221,- 1,- 1,186,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,0,0,- 1,-
1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,175,0,0,- 1,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,221,0,- 1,- 1};
const int Cyc_Lex_lex_trans[2729]=(const int[2729]){0,0,0,0,0,0,0,0,0,107,107,107,
107,107,34,7,177,224,224,224,224,224,222,0,0,0,0,0,0,0,0,0,107,108,109,110,0,111,
112,113,224,217,114,115,8,116,117,118,119,120,120,120,120,120,120,120,120,120,
121,9,122,123,124,17,125,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,126,126,220,185,18,127,128,129,126,126,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,5,130,36,37,11,6,7,12,- 1,16,97,38,95,- 1,69,39,57,40,33,33,70,42,
33,41,49,43,44,45,13,46,17,50,33,33,47,18,33,48,53,51,52,33,54,8,19,19,19,19,19,
19,19,19,55,56,66,33,60,8,61,20,31,31,31,31,31,31,31,31,32,32,32,32,32,32,32,32,
62,58,63,64,65,67,68,77,82,73,14,74,21,59,83,71,72,22,23,75,76,78,24,79,80,81,91,
84,85,86,25,87,88,89,26,90,27,92,28,93,29,30,30,30,30,30,30,30,30,30,30,94,96,98,
99,100,101,131,30,30,30,30,30,30,102,30,30,30,30,30,30,30,30,30,30,103,104,105,
218,184,135,- 1,30,30,30,30,30,30,32,142,30,30,30,30,30,30,182,34,138,138,147,219,
174,148,143,222,35,175,223,144,20,145,15,183,30,30,30,30,30,30,179,176,180,180,
180,180,180,180,180,180,180,180,146,149,181,216,27,215,7,27,25,25,155,211,155,
210,- 1,156,156,156,156,156,156,156,156,156,156,209,9,9,9,9,9,9,9,9,9,9,27,132,24,
27,25,25,15,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,208,24,207,206,9,
205,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,26,26,26,26,26,26,26,26,
26,26,204,24,26,26,173,203,173,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,
26,26,26,26,26,26,26,26,26,26,24,26,26,173,26,173,26,26,26,26,26,26,26,26,26,26,
26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,26,8,8,8,8,8,8,8,8,8,8,212,0,0,0,0,
0,0,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,- 1,0,0,0,8,0,8,8,8,8,8,8,
8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,23,23,23,23,23,23,23,23,23,23,0,0,0,0,0,
0,0,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,0,0,0,0,23,0,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,
23,23,23,23,23,24,24,24,24,24,24,24,24,24,24,0,0,0,0,0,0,0,24,24,24,24,24,24,24,
24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,0,0,0,0,24,0,24,24,24,
24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,25,25,25,25,
25,25,25,25,25,25,0,0,0,0,0,0,0,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,0,0,0,0,25,0,25,25,25,25,25,25,25,25,25,25,25,25,
25,25,25,25,25,25,25,25,25,25,25,25,25,25,21,21,21,21,21,21,21,21,21,21,0,0,0,0,
0,0,0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,
21,0,0,0,0,21,0,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,
21,21,21,21,21,7,7,7,7,7,7,7,7,7,7,0,0,0,0,0,0,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7,0,0,0,0,7,0,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
7,7,28,28,28,28,28,28,28,28,28,28,177,222,0,178,223,0,0,28,28,28,28,28,28,28,28,
28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,0,0,0,0,28,0,28,28,28,28,
28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,28,27,27,27,27,27,
27,27,27,27,27,0,0,0,0,0,0,0,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,
27,27,27,27,27,27,27,27,27,0,0,0,0,27,0,27,27,27,27,27,27,27,27,27,27,27,27,27,
27,27,27,27,27,27,27,27,27,27,27,27,27,22,22,22,22,22,22,22,22,22,22,0,0,0,0,- 1,
0,0,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,
22,- 1,0,0,0,22,0,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,22,
22,22,22,22,22,150,0,161,161,161,161,161,161,161,161,162,162,224,224,224,224,224,
0,0,0,0,187,0,152,0,0,0,0,0,0,163,- 1,- 1,0,0,224,0,0,0,164,0,0,165,150,0,151,151,
151,151,151,151,151,151,151,151,152,0,0,0,0,0,0,163,0,0,0,152,0,0,0,0,164,0,153,
165,0,0,0,0,0,0,0,154,156,156,156,156,156,156,156,156,156,156,0,0,0,0,0,152,0,0,
0,0,0,0,153,0,0,0,0,159,0,159,0,154,160,160,160,160,160,160,160,160,160,160,160,
160,160,160,160,160,160,160,160,160,0,0,136,136,136,136,136,136,136,136,136,136,
137,0,0,0,0,0,0,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
136,136,136,136,136,136,136,136,136,136,0,0,0,0,136,- 1,136,136,136,136,136,136,
136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
134,134,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,134,134,134,134,134,134,
134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
0,0,0,0,134,0,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
134,134,134,134,134,134,134,134,134,134,133,133,133,133,133,133,133,133,133,133,
133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,0,0,0,0,133,0,
133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,0,0,0,0,0,0,0,
133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
133,133,133,133,133,133,0,0,0,0,133,0,133,133,133,133,133,133,133,133,133,133,
133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,134,134,134,134,
134,134,134,134,134,134,0,0,0,0,0,0,0,134,134,134,134,134,134,134,134,134,134,
134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,0,0,0,0,134,0,
134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
134,134,134,134,134,134,136,136,136,136,136,136,136,136,136,136,137,0,0,0,0,0,0,
136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
136,136,136,136,136,136,0,0,0,0,136,0,136,136,136,136,136,136,136,136,136,136,
136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,139,139,139,139,
139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
139,139,0,0,0,0,140,0,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
139,139,141,0,0,0,0,0,0,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
139,139,139,139,139,139,139,139,139,139,139,139,0,0,0,0,139,0,139,139,139,139,
139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
139,139,140,140,140,140,140,140,140,140,140,140,0,0,0,0,0,0,0,140,140,140,140,
140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,
140,140,0,0,0,0,140,0,140,140,140,140,140,140,140,140,140,140,140,140,140,140,
140,140,140,140,140,140,140,140,140,140,140,140,157,157,157,157,157,157,157,157,
157,157,0,0,0,0,0,0,0,0,0,0,0,158,21,0,0,0,0,150,21,151,151,151,151,151,151,151,
151,151,151,156,156,156,156,156,156,156,156,156,156,0,152,0,0,158,21,0,0,153,0,0,
21,28,0,0,0,0,154,28,0,157,157,157,157,157,157,157,157,157,157,0,0,0,152,0,0,0,0,
0,0,153,158,21,0,28,0,0,0,21,154,28,0,0,160,160,160,160,160,160,160,160,160,160,
0,0,0,0,0,0,0,0,0,0,158,21,21,0,0,0,0,21,21,150,0,161,161,161,161,161,161,161,
161,162,162,0,0,0,0,0,0,0,0,0,0,0,152,0,21,0,0,0,0,171,21,0,0,0,0,0,0,150,172,
162,162,162,162,162,162,162,162,162,162,0,0,0,0,0,152,0,0,0,0,0,152,171,0,0,0,0,
0,169,0,0,172,0,0,0,0,0,170,0,0,180,180,180,180,180,180,180,180,180,180,0,0,0,
152,0,0,0,0,0,0,169,158,21,0,0,0,0,0,21,170,166,166,166,166,166,166,166,166,166,
166,0,0,0,0,0,0,0,166,166,166,166,166,166,158,21,0,0,0,0,0,21,0,0,0,0,0,0,0,166,
166,166,166,166,166,166,166,166,166,0,166,166,166,166,166,166,166,166,166,166,
166,166,0,0,0,188,0,167,0,0,189,0,0,0,0,0,168,0,0,190,190,190,190,190,190,190,
190,0,166,166,166,166,166,166,191,0,0,0,0,167,212,0,0,0,0,0,0,0,168,213,213,213,
213,213,213,213,213,0,0,0,0,0,0,192,0,0,0,0,193,194,0,0,0,195,0,0,0,0,0,0,0,196,
0,0,0,197,0,198,0,199,0,200,201,201,201,201,201,201,201,201,201,201,0,0,0,0,0,0,
0,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,
201,201,201,201,201,201,201,0,0,0,0,0,0,201,201,201,201,201,201,201,201,201,201,
201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,202,0,0,0,0,0,0,
0,0,201,201,201,201,201,201,201,201,201,201,0,0,0,0,0,0,0,201,201,201,201,201,
201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,
201,0,0,0,0,0,0,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,
201,201,201,201,201,201,201,201,201,201,212,0,0,0,0,0,0,0,0,214,214,214,214,214,
214,214,214,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0};const int Cyc_Lex_lex_check[2729]=(const int[2729]){- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,0,0,0,0,0,12,16,178,107,107,107,107,107,223,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,0,0,0,0,- 1,0,0,0,107,186,0,0,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,0,0,0,108,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,111,114,123,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,1,1,2,3,14,2,4,14,36,1,37,4,38,1,39,1,13,
13,38,41,13,1,40,42,43,44,2,45,14,40,33,33,46,14,33,47,49,50,51,13,53,13,14,14,
14,14,14,14,14,14,54,55,58,33,59,33,60,14,19,19,19,19,19,19,19,19,31,31,31,31,31,
31,31,31,61,57,62,63,64,66,67,71,69,72,2,73,14,57,69,70,70,14,14,74,75,77,14,78,
79,80,82,83,84,85,14,86,87,88,14,89,14,91,14,92,14,29,29,29,29,29,29,29,29,29,29,
93,95,97,98,99,100,0,29,29,29,29,29,29,101,30,30,30,30,30,30,30,30,30,30,102,103,
104,112,115,127,1,30,30,30,30,30,30,124,124,29,29,29,29,29,29,116,115,137,141,
121,112,118,121,142,110,130,118,110,122,122,122,116,116,30,30,30,30,30,30,117,
118,117,117,117,117,117,117,117,117,117,117,144,148,179,188,153,189,3,154,163,
164,152,191,152,192,4,152,152,152,152,152,152,152,152,152,152,193,48,48,48,48,48,
48,48,48,48,48,153,130,167,154,163,164,2,48,48,48,48,48,48,48,48,48,48,48,48,48,
48,48,48,48,48,48,48,48,48,48,48,48,48,194,167,195,196,48,197,48,48,48,48,48,48,
48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,48,52,52,52,52,52,52,52,
52,52,52,198,168,169,170,171,199,172,52,52,52,52,52,52,52,52,52,52,52,52,52,52,
52,52,52,52,52,52,52,52,52,52,52,52,168,169,170,171,52,172,52,52,52,52,52,52,52,
52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,52,56,56,56,56,56,56,56,56,
56,56,214,- 1,- 1,- 1,- 1,- 1,- 1,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,
56,56,56,56,56,56,56,56,56,110,- 1,- 1,- 1,56,- 1,56,56,56,56,56,56,56,56,56,56,56,
56,56,56,56,56,56,56,56,56,56,56,56,56,56,56,65,65,65,65,65,65,65,65,65,65,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,
65,65,65,65,- 1,- 1,- 1,- 1,65,- 1,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,65,
65,65,65,65,65,65,65,65,65,68,68,68,68,68,68,68,68,68,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,
68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,- 1,- 1,
- 1,- 1,68,- 1,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,68,
68,68,68,76,76,76,76,76,76,76,76,76,76,- 1,- 1,- 1,- 1,- 1,- 1,- 1,76,76,76,76,76,76,76,
76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,- 1,- 1,- 1,- 1,76,- 1,76,76,
76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,76,81,81,81,
81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,
81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,
81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,90,90,90,90,90,90,90,90,90,
90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,94,94,94,94,94,94,94,94,94,94,175,221,- 1,175,
221,- 1,- 1,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,
94,94,94,- 1,- 1,- 1,- 1,94,- 1,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,94,
94,94,94,94,94,94,94,94,96,96,96,96,96,96,96,96,96,96,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,96,
96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,- 1,- 1,- 1,
- 1,96,- 1,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,
96,96,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,113,- 1,- 1,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,113,- 1,- 1,- 1,105,- 1,105,105,105,105,105,105,105,105,105,105,105,105,
105,105,105,105,105,105,105,105,105,105,105,105,105,105,119,- 1,119,119,119,119,
119,119,119,119,119,119,224,224,224,224,224,- 1,- 1,- 1,- 1,113,- 1,119,- 1,- 1,- 1,- 1,-
1,- 1,119,175,221,- 1,- 1,224,- 1,- 1,- 1,119,- 1,- 1,119,120,- 1,120,120,120,120,120,120,
120,120,120,120,119,- 1,- 1,- 1,- 1,- 1,- 1,119,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,119,- 1,120,
119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,155,155,155,155,155,155,155,155,155,155,- 1,- 1,- 1,- 1,
- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,158,- 1,158,- 1,120,158,158,158,158,158,
158,158,158,158,158,159,159,159,159,159,159,159,159,159,159,- 1,- 1,126,126,126,
126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,- 1,-
1,- 1,- 1,126,113,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,126,
126,126,126,126,126,126,126,126,126,126,128,128,128,128,128,128,128,128,128,128,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
128,128,128,128,128,128,128,128,128,128,128,- 1,- 1,- 1,- 1,128,- 1,128,128,128,128,
128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
128,128,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,129,- 1,129,129,129,129,129,129,129,
129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,133,
133,133,133,133,133,133,133,133,133,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,133,133,133,133,133,
133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
- 1,- 1,- 1,- 1,133,- 1,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
133,133,133,133,133,133,133,133,133,133,133,134,134,134,134,134,134,134,134,134,
134,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
134,134,134,134,134,134,134,134,134,134,134,134,- 1,- 1,- 1,- 1,134,- 1,134,134,134,
134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
134,134,134,136,136,136,136,136,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,136,
136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
136,136,136,136,136,- 1,- 1,- 1,- 1,136,- 1,136,136,136,136,136,136,136,136,136,136,
136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,138,138,138,138,
138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,
138,138,- 1,- 1,- 1,- 1,138,- 1,138,138,138,138,138,138,138,138,138,138,138,138,138,
138,138,138,138,138,138,138,138,138,138,138,138,138,139,139,139,139,139,139,139,
139,139,139,139,- 1,- 1,- 1,- 1,- 1,- 1,139,139,139,139,139,139,139,139,139,139,139,
139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,139,- 1,
139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
139,139,139,139,139,139,140,140,140,140,140,140,140,140,140,140,- 1,- 1,- 1,- 1,- 1,-
1,- 1,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,
140,140,140,140,140,140,140,- 1,- 1,- 1,- 1,140,- 1,140,140,140,140,140,140,140,140,
140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,150,150,
150,150,150,150,150,150,150,150,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,150,150,- 1,- 1,-
1,- 1,151,150,151,151,151,151,151,151,151,151,151,151,156,156,156,156,156,156,156,
156,156,156,- 1,151,- 1,- 1,150,150,- 1,- 1,151,- 1,- 1,150,156,- 1,- 1,- 1,- 1,151,156,- 1,
157,157,157,157,157,157,157,157,157,157,- 1,- 1,- 1,151,- 1,- 1,- 1,- 1,- 1,- 1,151,157,
157,- 1,156,- 1,- 1,- 1,157,151,156,- 1,- 1,160,160,160,160,160,160,160,160,160,160,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,157,157,160,- 1,- 1,- 1,- 1,157,160,161,- 1,161,161,161,
161,161,161,161,161,161,161,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,160,- 1,- 1,- 1,
- 1,161,160,- 1,- 1,- 1,- 1,- 1,- 1,162,161,162,162,162,162,162,162,162,162,162,162,- 1,
- 1,- 1,- 1,- 1,161,- 1,- 1,- 1,- 1,- 1,162,161,- 1,- 1,- 1,- 1,- 1,162,- 1,- 1,161,- 1,- 1,- 1,- 1,
- 1,162,- 1,- 1,180,180,180,180,180,180,180,180,180,180,- 1,- 1,- 1,162,- 1,- 1,- 1,- 1,- 1,
- 1,162,180,180,- 1,- 1,- 1,- 1,- 1,180,162,165,165,165,165,165,165,165,165,165,165,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,165,165,165,165,165,165,180,180,- 1,- 1,- 1,- 1,- 1,180,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,166,166,166,166,166,166,166,166,166,166,- 1,165,165,165,165,165,165,166,
166,166,166,166,166,- 1,- 1,- 1,187,- 1,166,- 1,- 1,187,- 1,- 1,- 1,- 1,- 1,166,- 1,- 1,187,
187,187,187,187,187,187,187,- 1,166,166,166,166,166,166,187,- 1,- 1,- 1,- 1,166,190,-
1,- 1,- 1,- 1,- 1,- 1,- 1,166,190,190,190,190,190,190,190,190,- 1,- 1,- 1,- 1,- 1,- 1,187,- 1,
- 1,- 1,- 1,187,187,- 1,- 1,- 1,187,- 1,- 1,- 1,- 1,- 1,- 1,- 1,187,- 1,- 1,- 1,187,- 1,187,- 1,
187,- 1,187,200,200,200,200,200,200,200,200,200,200,- 1,- 1,- 1,- 1,- 1,- 1,- 1,200,200,
200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,
200,200,200,200,- 1,- 1,- 1,- 1,- 1,- 1,200,200,200,200,200,200,200,200,200,200,200,
200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,201,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,- 1,- 1,- 1,201,201,201,
201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,
201,201,201,- 1,- 1,- 1,- 1,- 1,- 1,201,201,201,201,201,201,201,201,201,201,201,201,
201,201,201,201,201,201,201,201,201,201,201,201,201,201,213,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,213,213,213,213,213,213,213,213,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_Lex_lex_engine(
int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;
state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){base=Cyc_Lex_lex_base[
_check_known_subscript_notnull(225,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_Lex_lex_backtrk[_check_known_subscript_notnull(225,state)];if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos
>= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}
else{c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),
lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_Lex_lex_check[
_check_known_subscript_notnull(2729,base + c)]== state)state=Cyc_Lex_lex_trans[
_check_known_subscript_notnull(2729,base + c)];else{state=Cyc_Lex_lex_default[
_check_known_subscript_notnull(225,state)];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Lexing_Error_struct
_tmpD7;_tmpD7.tag=Cyc_Lexing_Error;_tmpD7.f1=({const char*_tmpD8="empty token";
_tag_dyneither(_tmpD8,sizeof(char),12);});_tmpD7;});_tmpD6;}));else{return lbuf->lex_last_action;}}
else{if(c == 256)lbuf->lex_eof_reached=0;}}}int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL33: return(int)Cyc_Lex_process_id(lexbuf);case 1: _LL34: return(int)Cyc_Lex_process_id(
lexbuf);case 2: _LL35: return(int)Cyc_Lex_process_qual_id(lexbuf);case 3: _LL36: Cyc_Lex_token_string=*
Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));return 367;case 4: _LL37: return
Cyc_Lex_cyc_qual(lexbuf);case 5: _LL38: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,
0,16);return 363;case 6: _LL39: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);
return 363;case 7: _LL3A: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);return
363;case 8: _LL3B: Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);return 363;case
9: _LL3C: Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
return 366;case 10: _LL3D: Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf);return 366;case 11: _LL3E: return 345;case 12: _LL3F: return 346;case 13: _LL40:
return 343;case 14: _LL41: return 344;case 15: _LL42: return 339;case 16: _LL43: return 340;
case 17: _LL44: return 352;case 18: _LL45: return 353;case 19: _LL46: return 349;case 20:
_LL47: return 350;case 21: _LL48: return 351;case 22: _LL49: return 358;case 23: _LL4A:
return 357;case 24: _LL4B: return 356;case 25: _LL4C: return 354;case 26: _LL4D: return 355;
case 27: _LL4E: return 347;case 28: _LL4F: return 348;case 29: _LL50: return 341;case 30:
_LL51: return 342;case 31: _LL52: return 360;case 32: _LL53: return 338;case 33: _LL54:
return 359;case 34: _LL55: return 361;case 35: _LL56: return 311;case 36: _LL57: return Cyc_Lex_token(
lexbuf);case 37: _LL58: return Cyc_Lex_token(lexbuf);case 38: _LL59: return Cyc_Lex_token(
lexbuf);case 39: _LL5A: Cyc_Lex_comment_depth=1;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(
lexbuf);Cyc_Lex_comment(lexbuf);return Cyc_Lex_token(lexbuf);case 40: _LL5B: Cyc_Lex_string_pos=
0;Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);while(Cyc_Lex_strng(
lexbuf)){;}Cyc_Lex_token_string=(struct _dyneither_ptr)Cyc_Lex_get_stored_string();
return 364;case 41: _LL5C: Cyc_Lex_token_char='\a';return 365;case 42: _LL5D: Cyc_Lex_token_char='\b';
return 365;case 43: _LL5E: Cyc_Lex_token_char='\f';return 365;case 44: _LL5F: Cyc_Lex_token_char='\n';
return 365;case 45: _LL60: Cyc_Lex_token_char='\r';return 365;case 46: _LL61: Cyc_Lex_token_char='\t';
return 365;case 47: _LL62: Cyc_Lex_token_char='\v';return 365;case 48: _LL63: Cyc_Lex_token_char='\\';
return 365;case 49: _LL64: Cyc_Lex_token_char='\'';return 365;case 50: _LL65: Cyc_Lex_token_char='"';
return 365;case 51: _LL66: Cyc_Lex_token_char='?';return 365;case 52: _LL67: Cyc_Lex_token_char=(
char)(Cyc_Lex_intconst(lexbuf,2,1,8)).f2;return 365;case 53: _LL68: Cyc_Lex_token_char=(
char)(Cyc_Lex_intconst(lexbuf,3,1,16)).f2;return 365;case 54: _LL69: Cyc_Lex_token_char=
Cyc_Lexing_lexeme_char(lexbuf,1);return 365;case 55: _LL6A: return - 1;case 56: _LL6B:
return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL6C:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_token_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9[0]=({struct Cyc_Lexing_Error_struct
_tmpDA;_tmpDA.tag=Cyc_Lexing_Error;_tmpDA.f1=({const char*_tmpDB="some action didn't return!";
_tag_dyneither(_tmpDB,sizeof(char),27);});_tmpDA;});_tmpD9;}));}int Cyc_Lex_token(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}int Cyc_Lex_cyc_qual_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL6E: lexbuf->lex_curr_pos -=Cyc_strlen(({const
char*_tmpDC="taggedZ";_tag_dyneither(_tmpDC,sizeof(char),8);}));return(int)'@';
case 1: _LL6F: lexbuf->lex_curr_pos -=Cyc_strlen(({const char*_tmpDD="zerotermZ";
_tag_dyneither(_tmpDD,sizeof(char),10);}));return(int)'@';case 2: _LL70: lexbuf->lex_curr_pos
-=Cyc_strlen(({const char*_tmpDE="nozerotermZ";_tag_dyneither(_tmpDE,sizeof(char),
12);}));return(int)'@';case 3: _LL71: lexbuf->lex_curr_pos -=Cyc_strlen(({const char*
_tmpDF="extensibleZ";_tag_dyneither(_tmpDF,sizeof(char),12);}));return(int)'@';
case 4: _LL72: lexbuf->lex_curr_pos -=Cyc_strlen(({const char*_tmpE0="resetableZ";
_tag_dyneither(_tmpE0,sizeof(char),11);}));return(int)'@';case 5: _LL73: lexbuf->lex_curr_pos
-=Cyc_strlen(({const char*_tmpE1="regionZ";_tag_dyneither(_tmpE1,sizeof(char),8);}));
return(int)'@';case 6: _LL74: lexbuf->lex_curr_pos -=Cyc_strlen(({const char*_tmpE2="numeltsZ";
_tag_dyneither(_tmpE2,sizeof(char),9);}));return(int)'@';case 7: _LL75: lexbuf->lex_curr_pos
-=Cyc_strlen(({const char*_tmpE3="thinZ";_tag_dyneither(_tmpE3,sizeof(char),6);}));
return(int)'@';case 8: _LL76: lexbuf->lex_curr_pos -=Cyc_strlen(({const char*_tmpE4="fatZ";
_tag_dyneither(_tmpE4,sizeof(char),5);}));return(int)'@';case 9: _LL77: lexbuf->lex_curr_pos
-=Cyc_strlen(({const char*_tmpE5="notnullZ";_tag_dyneither(_tmpE5,sizeof(char),9);}));
return(int)'@';case 10: _LL78: lexbuf->lex_curr_pos -=Cyc_strlen(({const char*_tmpE6="nullableZ";
_tag_dyneither(_tmpE6,sizeof(char),10);}));return(int)'@';case 11: _LL79: return 335;
case 12: _LL7A: return 320;case 13: _LL7B: return 319;case 14: _LL7C: return 336;case 15:
_LL7D: return 337;case 16: _LL7E: return 321;case 17: _LL7F: return 330;case 18: _LL80:
return 331;case 19: _LL81: return 332;case 20: _LL82: return 333;case 21: _LL83: return 334;
case 22: _LL84: lexbuf->lex_curr_pos -=1;return(int)'@';default: _LL85:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_cyc_qual_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({struct Cyc_Lexing_Error_struct
_tmpE8;_tmpE8.tag=Cyc_Lexing_Error;_tmpE8.f1=({const char*_tmpE9="some action didn't return!";
_tag_dyneither(_tmpE9,sizeof(char),27);});_tmpE8;});_tmpE7;}));}int Cyc_Lex_cyc_qual(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_cyc_qual_rec(lexbuf,1);}int Cyc_Lex_strng_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL87: return 1;case 1: _LL88: return 0;case 2: _LL89:
return 1;case 3: _LL8A: Cyc_Lex_store_string_char('\a');return 1;case 4: _LL8B: Cyc_Lex_store_string_char('\b');
return 1;case 5: _LL8C: Cyc_Lex_store_string_char('\f');return 1;case 6: _LL8D: Cyc_Lex_store_string_char('\n');
return 1;case 7: _LL8E: Cyc_Lex_store_string_char('\r');return 1;case 8: _LL8F: Cyc_Lex_store_string_char('\t');
return 1;case 9: _LL90: Cyc_Lex_store_string_char('\v');return 1;case 10: _LL91: Cyc_Lex_store_string_char('\\');
return 1;case 11: _LL92: Cyc_Lex_store_string_char('\'');return 1;case 12: _LL93: Cyc_Lex_store_string_char('"');
return 1;case 13: _LL94: Cyc_Lex_store_string_char('?');return 1;case 14: _LL95: Cyc_Lex_store_string_char((
char)(Cyc_Lex_intconst(lexbuf,1,0,8)).f2);return 1;case 15: _LL96: Cyc_Lex_store_string_char((
char)(Cyc_Lex_intconst(lexbuf,2,0,16)).f2);return 1;case 16: _LL97: Cyc_Lex_store_string_char(
Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 17: _LL98: Cyc_Lex_runaway_err(({
const char*_tmpEA="string ends in newline";_tag_dyneither(_tmpEA,sizeof(char),23);}),
lexbuf);return 0;case 18: _LL99: Cyc_Lex_runaway_err(({const char*_tmpEB="unterminated string";
_tag_dyneither(_tmpEB,sizeof(char),20);}),lexbuf);return 0;case 19: _LL9A: Cyc_Lex_err(({
const char*_tmpEC="bad character following backslash in string";_tag_dyneither(
_tmpEC,sizeof(char),44);}),lexbuf);return 1;default: _LL9B:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_strng_rec(lexbuf,lexstate);}(int)_throw((void*)({struct Cyc_Lexing_Error_struct*
_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Lexing_Error_struct
_tmpEE;_tmpEE.tag=Cyc_Lexing_Error;_tmpEE.f1=({const char*_tmpEF="some action didn't return!";
_tag_dyneither(_tmpEF,sizeof(char),27);});_tmpEE;});_tmpED;}));}int Cyc_Lex_strng(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,2);}int Cyc_Lex_comment_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_Lex_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL9D: ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(
lexbuf);case 1: _LL9E: -- Cyc_Lex_comment_depth;if(Cyc_Lex_comment_depth > 0)return
Cyc_Lex_comment(lexbuf);return 0;case 2: _LL9F: Cyc_Lex_runaway_err(({const char*
_tmpF0="unterminated comment";_tag_dyneither(_tmpF0,sizeof(char),21);}),lexbuf);
return 0;case 3: _LLA0: return Cyc_Lex_comment(lexbuf);case 4: _LLA1: return Cyc_Lex_comment(
lexbuf);case 5: _LLA2: return Cyc_Lex_comment(lexbuf);default: _LLA3:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lex_comment_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Lexing_Error_struct
_tmpF2;_tmpF2.tag=Cyc_Lexing_Error;_tmpF2.f1=({const char*_tmpF3="some action didn't return!";
_tag_dyneither(_tmpF3,sizeof(char),27);});_tmpF2;});_tmpF1;}));}int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,3);}void Cyc_Lex_lex_init(
int include_cyclone_keywords){if(Cyc_Lex_ids_trie != 0){struct _DynRegionHandle*
_tmpF5;struct Cyc_Lex_DynTrieSymbols _tmpF4=*((struct Cyc_Lex_DynTrieSymbols*)
_check_null(Cyc_Lex_ids_trie));_tmpF5=_tmpF4.dyn;Cyc_Core_free_dynregion(_tmpF5);}
if(Cyc_Lex_typedefs_trie != 0){struct _DynRegionHandle*_tmpF7;struct Cyc_Lex_DynTrie
_tmpF6=*((struct Cyc_Lex_DynTrie*)_check_null(Cyc_Lex_typedefs_trie));_tmpF7=
_tmpF6.dyn;Cyc_Core_free_dynregion(_tmpF7);}{struct _DynRegionHandle*_tmpF9;
struct Cyc_Core_NewRegion _tmpF8=Cyc_Core_new_dynregion();_tmpF9=_tmpF8.dynregion;{
struct _DynRegionFrame _tmpFA;struct _RegionHandle*id_rgn=_open_dynregion(& _tmpFA,
_tmpF9);{struct Cyc_Xarray_Xarray*symbols=((struct Cyc_Xarray_Xarray*(*)(struct
_RegionHandle*,int,struct _dyneither_ptr*))Cyc_Xarray_rcreate)(id_rgn,101,({
struct _dyneither_ptr*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=(struct
_dyneither_ptr)({const char*_tmpFE="";_tag_dyneither(_tmpFE,sizeof(char),1);});
_tmpFD;}));((void(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add)(
symbols,& Cyc_Lex_bogus_string);Cyc_Lex_ids_trie=({struct Cyc_Lex_DynTrieSymbols*
_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->dyn=(struct _DynRegionHandle*)_tmpF9;
_tmpFB->t=(struct Cyc_Lex_Trie*)({struct Cyc_Lex_Trie*_tmpFC=_region_malloc(id_rgn,
sizeof(*_tmpFC));_tmpFC->children=Cyc_Lex_Zero();_tmpFC->shared_str=0;_tmpFC;});
_tmpFB->symbols=(struct Cyc_Xarray_Xarray*)symbols;_tmpFB;});}{struct
_DynRegionHandle*_tmp100;struct Cyc_Core_NewRegion _tmpFF=Cyc_Core_new_dynregion();
_tmp100=_tmpFF.dynregion;{struct _DynRegionFrame _tmp101;struct _RegionHandle*
typedefs_rgn=_open_dynregion(& _tmp101,_tmp100);Cyc_Lex_typedefs_trie=({struct Cyc_Lex_DynTrie*
_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->dyn=(struct _DynRegionHandle*)
_tmp100;_tmp102->t=(struct Cyc_Lex_Trie*)({struct Cyc_Lex_Trie*_tmp103=
_region_malloc(typedefs_rgn,sizeof(*_tmp103));_tmp103->children=Cyc_Lex_Zero();
_tmp103->shared_str=0;_tmp103;});_tmp102;});Cyc_Lex_num_kws=Cyc_Lex_num_keywords(
include_cyclone_keywords);Cyc_Lex_kw_nums=({unsigned int _tmp104=(unsigned int)
Cyc_Lex_num_kws;int*_tmp105=(int*)_cycalloc_atomic(_check_times(sizeof(int),
_tmp104));struct _dyneither_ptr _tmp107=_tag_dyneither(_tmp105,sizeof(int),_tmp104);{
unsigned int _tmp106=_tmp104;unsigned int i;for(i=0;i < _tmp106;i ++){_tmp105[i]=0;}}
_tmp107;});{unsigned int i=0;unsigned int rwsze=72;{unsigned int j=0;for(0;j < rwsze;
++ j){if(include_cyclone_keywords  || (Cyc_Lex_rw_array[(int)j]).f3){struct
_dyneither_ptr _tmp108=(Cyc_Lex_rw_array[(int)j]).f1;Cyc_Lex_str_index(_tmp108,0,(
int)Cyc_strlen((struct _dyneither_ptr)_tmp108));*((int*)
_check_dyneither_subscript(Cyc_Lex_kw_nums,sizeof(int),(int)i))=(int)(Cyc_Lex_rw_array[(
int)j]).f2;++ i;}}}Cyc_Lex_typedef_init();Cyc_Lex_comment_depth=0;};
_pop_dynregion(typedefs_rgn);}};_pop_dynregion(id_rgn);}}}
