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
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
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
tag;struct _dyneither_ptr f1;};extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(
struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
14];struct Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(
void*,void*),struct Cyc_List_List*x,void*accum);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0
Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x);unsigned int Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);int Cyc_zstrcmp(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned int n);
struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;
int line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct _dyneither_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct
Cyc_Position_Segment*,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
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
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct
Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct
Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;
struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3
val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};
union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct
_union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
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
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
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
int tag;struct _dyneither_ptr f1;};extern struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_struct Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct
Cyc_Absyn_Fastcall_att_val;extern struct Cyc_Absyn_Noreturn_att_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_struct Cyc_Absyn_Const_att_val;extern struct Cyc_Absyn_Packed_att_struct
Cyc_Absyn_Packed_att_val;extern struct Cyc_Absyn_Shared_att_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val;extern struct Cyc_Absyn_Weak_att_struct
Cyc_Absyn_Weak_att_val;extern struct Cyc_Absyn_Dllimport_att_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_struct Cyc_Absyn_Dllexport_att_val;extern
struct Cyc_Absyn_No_instrument_function_att_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_struct Cyc_Absyn_Constructor_att_val;extern
struct Cyc_Absyn_Destructor_att_struct Cyc_Absyn_Destructor_att_val;extern struct
Cyc_Absyn_No_check_memory_usage_att_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_struct{
int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
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
struct Cyc_Absyn_Exp*f1;};extern struct Cyc_Absyn_Wild_p_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*
r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_Unresolved_b_struct{int tag;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;
struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;
};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope
sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;
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
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};extern struct Cyc_Absyn_Porton_d_struct
Cyc_Absyn_Porton_d_val;extern struct Cyc_Absyn_Portoff_d_struct Cyc_Absyn_Portoff_d_val;
struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_is_qvar_qualified(struct _tuple1*);struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(
void*x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(int);void*Cyc_Absyn_starb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
enum Cyc_Absyn_Sign,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(
enum Cyc_Absyn_Primop,struct Cyc_List_List*es,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*
exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);struct Cyc_Absyn_Decl*
Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple1*
n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(
enum Cyc_Absyn_Scope s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*
Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(void*t);extern struct Cyc_Core_Opt
Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ik;struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(
enum Cyc_Absyn_Kind k);void*Cyc_Tcutil_kind_to_bound(enum Cyc_Absyn_Kind k);struct
Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Lex_register_typedef(struct _tuple1*s);void Cyc_Lex_enter_namespace(
struct _dyneither_ptr*);void Cyc_Lex_leave_namespace();void Cyc_Lex_enter_using(
struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=0;struct Cyc_Parse_Signed_spec_struct{
int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Unsigned_spec_struct{int
tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Short_spec_struct{int tag;
struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Long_spec_struct{int tag;struct
Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{int tag;void*f1;struct
Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{int tag;struct Cyc_Absyn_Decl*
f1;};enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc
 = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc
 = 5,Cyc_Parse_Abstract_sc  = 6};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple10{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple10 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple11{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple11
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Parse_err((struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)({
struct Cyc_Position_Exit_struct*_tmp0=_cycalloc_atomic(sizeof(*_tmp0));_tmp0[0]=({
struct Cyc_Position_Exit_struct _tmp1;_tmp1.tag=Cyc_Position_Exit;_tmp1;});_tmp0;}));}
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,struct Cyc_Position_Segment*sg){({
struct Cyc_String_pa_struct _tmp5;_tmp5.tag=0;_tmp5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg);({struct Cyc_String_pa_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(sg));({
void*_tmp2[2]={& _tmp4,& _tmp5};Cyc_fprintf(Cyc_stderr,({const char*_tmp3="%s: Warning: Cyclone does not yet support %s\n";
_tag_dyneither(_tmp3,sizeof(char),46);}),_tag_dyneither(_tmp2,sizeof(void*),2));});});});
return;}struct _tuple12{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;void*f4;};static struct _tuple12 Cyc_Parse_collapse_pointer_quals(
struct Cyc_Position_Segment*loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*
bound,void*rgn,struct Cyc_List_List*pqs){union Cyc_Absyn_Constraint*zeroterm=((
union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();for(0;pqs != 0;pqs=pqs->tl){
void*_tmp6=(void*)pqs->hd;struct Cyc_Absyn_Exp*_tmpE;void*_tmp10;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_struct*
_tmp7=(struct Cyc_Absyn_Zeroterm_ptrqual_struct*)_tmp6;if(_tmp7->tag != 4)goto _LL3;}
_LL2: zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_struct*
_tmp8=(struct Cyc_Absyn_Nozeroterm_ptrqual_struct*)_tmp6;if(_tmp8->tag != 5)goto
_LL5;}_LL4: zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_struct*
_tmp9=(struct Cyc_Absyn_Nullable_ptrqual_struct*)_tmp6;if(_tmp9->tag != 7)goto _LL7;}
_LL6: nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_struct*
_tmpA=(struct Cyc_Absyn_Notnull_ptrqual_struct*)_tmp6;if(_tmpA->tag != 6)goto _LL9;}
_LL8: nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_struct*
_tmpB=(struct Cyc_Absyn_Fat_ptrqual_struct*)_tmp6;if(_tmpB->tag != 3)goto _LLB;}
_LLA: bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_struct*
_tmpC=(struct Cyc_Absyn_Thin_ptrqual_struct*)_tmp6;if(_tmpC->tag != 2)goto _LLD;}
_LLC: bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmpD=(struct Cyc_Absyn_Numelts_ptrqual_struct*)_tmp6;if(_tmpD->tag != 0)goto _LLF;
else{_tmpE=_tmpD->f1;}}_LLE: bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp12;_tmp12.tag=1;_tmp12.f1=_tmpE;_tmp12;});
_tmp11;}));goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_struct*_tmpF=(struct
Cyc_Absyn_Region_ptrqual_struct*)_tmp6;if(_tmpF->tag != 1)goto _LL0;else{_tmp10=(
void*)_tmpF->f1;}}_LL10: rgn=_tmp10;goto _LL0;_LL0:;}return({struct _tuple12 _tmp13;
_tmp13.f1=nullable;_tmp13.f2=bound;_tmp13.f3=zeroterm;_tmp13.f4=rgn;_tmp13;});}
static int Cyc_Parse_enum_counter=0;struct _tuple1*Cyc_Parse_gensym_enum(){return({
struct _tuple1*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->f1=Cyc_Absyn_Rel_n(0);
_tmp14->f2=({struct _dyneither_ptr*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=(
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp18;_tmp18.tag=1;_tmp18.f1=(
unsigned long)Cyc_Parse_enum_counter ++;({void*_tmp16[1]={& _tmp18};Cyc_aprintf(({
const char*_tmp17="__anonymous_enum_%d__";_tag_dyneither(_tmp17,sizeof(char),22);}),
_tag_dyneither(_tmp16,sizeof(void*),1));});});_tmp15;});_tmp14;});}struct
_tuple13{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;
struct Cyc_List_List*f5;};struct _tuple14{struct _tuple13*f1;struct Cyc_Absyn_Exp*f2;
};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple14*field_info){struct _tuple14 _tmp1A;struct _tuple13*_tmp1B;struct
_tuple13 _tmp1C;struct _tuple1*_tmp1D;struct Cyc_Absyn_Tqual _tmp1E;void*_tmp1F;
struct Cyc_List_List*_tmp20;struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Exp*_tmp22;
struct _tuple14*_tmp19=field_info;_tmp1A=*_tmp19;_tmp1B=_tmp1A.f1;_tmp1C=*_tmp1B;
_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;_tmp1F=_tmp1C.f3;_tmp20=_tmp1C.f4;_tmp21=
_tmp1C.f5;_tmp22=_tmp1A.f2;if(_tmp20 != 0)Cyc_Parse_err(({const char*_tmp23="bad type params in struct field";
_tag_dyneither(_tmp23,sizeof(char),32);}),loc);if(Cyc_Absyn_is_qvar_qualified(
_tmp1D))Cyc_Parse_err(({const char*_tmp24="struct or union field cannot be qualified with a namespace";
_tag_dyneither(_tmp24,sizeof(char),59);}),loc);return({struct Cyc_Absyn_Aggrfield*
_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->name=(*_tmp1D).f2;_tmp25->tq=_tmp1E;
_tmp25->type=_tmp1F;_tmp25->width=_tmp22;_tmp25->attributes=_tmp21;_tmp25;});}
static void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc){return(
void*)({struct Cyc_Parse_Type_spec_struct*_tmp26=_cycalloc(sizeof(*_tmp26));
_tmp26[0]=({struct Cyc_Parse_Type_spec_struct _tmp27;_tmp27.tag=4;_tmp27.f1=(void*)
t;_tmp27.f2=loc;_tmp27;});_tmp26;});}static void*Cyc_Parse_array2ptr(void*t,int
argposn){void*_tmp28=t;struct Cyc_Absyn_ArrayInfo _tmp2A;void*_tmp2B;struct Cyc_Absyn_Tqual
_tmp2C;struct Cyc_Absyn_Exp*_tmp2D;union Cyc_Absyn_Constraint*_tmp2E;struct Cyc_Position_Segment*
_tmp2F;_LL12: {struct Cyc_Absyn_ArrayType_struct*_tmp29=(struct Cyc_Absyn_ArrayType_struct*)
_tmp28;if(_tmp29->tag != 9)goto _LL14;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp2A.elt_type;
_tmp2C=_tmp2A.tq;_tmp2D=_tmp2A.num_elts;_tmp2E=_tmp2A.zero_term;_tmp2F=_tmp2A.zt_loc;}}
_LL13: return Cyc_Absyn_starb_typ(_tmp2B,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0):(void*)& Cyc_Absyn_HeapRgn_val,_tmp2C,_tmp2D == 0?(void*)& Cyc_Absyn_DynEither_b_val:(
void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp30=_cycalloc(sizeof(*_tmp30));
_tmp30[0]=({struct Cyc_Absyn_Upper_b_struct _tmp31;_tmp31.tag=1;_tmp31.f1=(struct
Cyc_Absyn_Exp*)_tmp2D;_tmp31;});_tmp30;})),_tmp2E);_LL14:;_LL15: return t;_LL11:;}
struct _tuple15{struct _dyneither_ptr*f1;void*f2;};static struct Cyc_List_List*Cyc_Parse_get_arg_tags(
struct Cyc_List_List*x){struct Cyc_List_List*_tmp32=0;for(0;x != 0;x=x->tl){struct
_tuple8*_tmp33=(struct _tuple8*)x->hd;struct _tuple8 _tmp34;struct Cyc_Core_Opt*
_tmp35;struct Cyc_Core_Opt _tmp36;struct _dyneither_ptr*_tmp37;void*_tmp38;void*
_tmp3A;struct _tuple8 _tmp3B;struct Cyc_Core_Opt*_tmp3C;struct Cyc_Core_Opt _tmp3D;
struct _dyneither_ptr*_tmp3E;void*_tmp3F;void*_tmp41;struct Cyc_Core_Opt*_tmp43;
struct Cyc_Core_Opt**_tmp44;_LL17: _tmp34=*_tmp33;_tmp35=_tmp34.f1;if(_tmp35 == 0)
goto _LL19;_tmp36=*_tmp35;_tmp37=(struct _dyneither_ptr*)_tmp36.v;_tmp38=_tmp34.f3;{
struct Cyc_Absyn_TagType_struct*_tmp39=(struct Cyc_Absyn_TagType_struct*)_tmp38;
if(_tmp39->tag != 20)goto _LL19;else{_tmp3A=(void*)_tmp39->f1;}};_LL18:{void*
_tmp45=_tmp3A;struct Cyc_Core_Opt*_tmp47;struct Cyc_Core_Opt**_tmp48;_LL1E: {
struct Cyc_Absyn_Evar_struct*_tmp46=(struct Cyc_Absyn_Evar_struct*)_tmp45;if(
_tmp46->tag != 1)goto _LL20;else{_tmp47=_tmp46->f2;_tmp48=(struct Cyc_Core_Opt**)&
_tmp46->f2;}}_LL1F: {struct _dyneither_ptr*nm=(struct _dyneither_ptr*)({struct
_dyneither_ptr*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp37);({void*_tmp50[1]={& _tmp52};Cyc_aprintf(({const char*_tmp51="`%s";
_tag_dyneither(_tmp51,sizeof(char),4);}),_tag_dyneither(_tmp50,sizeof(void*),1));});});
_tmp4F;});*_tmp48=({struct Cyc_Core_Opt*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->v=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[
0]=({struct Cyc_Absyn_VarType_struct _tmp4B;_tmp4B.tag=2;_tmp4B.f1=({struct Cyc_Absyn_Tvar*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->name=nm;_tmp4C->identity=- 1;_tmp4C->kind=(
void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp4D=_cycalloc_atomic(sizeof(*_tmp4D));
_tmp4D[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp4E;_tmp4E.tag=0;_tmp4E.f1=Cyc_Absyn_IntKind;
_tmp4E;});_tmp4D;});_tmp4C;});_tmp4B;});_tmp4A;});_tmp49;});goto _LL1D;}_LL20:;
_LL21: goto _LL1D;_LL1D:;}_tmp32=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*
_tmp53));_tmp53->hd=({struct _tuple15*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->f1=
_tmp37;_tmp54->f2=_tmp3A;_tmp54;});_tmp53->tl=_tmp32;_tmp53;});goto _LL16;_LL19:
_tmp3B=*_tmp33;_tmp3C=_tmp3B.f1;if(_tmp3C == 0)goto _LL1B;_tmp3D=*_tmp3C;_tmp3E=(
struct _dyneither_ptr*)_tmp3D.v;_tmp3F=_tmp3B.f3;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp40=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp3F;if(_tmp40->tag != 16)goto
_LL1B;else{_tmp41=(void*)_tmp40->f1;{struct Cyc_Absyn_Evar_struct*_tmp42=(struct
Cyc_Absyn_Evar_struct*)_tmp41;if(_tmp42->tag != 1)goto _LL1B;else{_tmp43=_tmp42->f2;
_tmp44=(struct Cyc_Core_Opt**)& _tmp42->f2;}};}};_LL1A: {struct _dyneither_ptr*nm=(
struct _dyneither_ptr*)({struct _dyneither_ptr*_tmp5B=_cycalloc(sizeof(*_tmp5B));
_tmp5B[0]=({struct Cyc_String_pa_struct _tmp5E;_tmp5E.tag=0;_tmp5E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3E);({void*_tmp5C[1]={& _tmp5E};Cyc_aprintf(({
const char*_tmp5D="`%s";_tag_dyneither(_tmp5D,sizeof(char),4);}),_tag_dyneither(
_tmp5C,sizeof(void*),1));});});_tmp5B;});*_tmp44=({struct Cyc_Core_Opt*_tmp55=
_cycalloc(sizeof(*_tmp55));_tmp55->v=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56[0]=({struct Cyc_Absyn_VarType_struct
_tmp57;_tmp57.tag=2;_tmp57.f1=({struct Cyc_Absyn_Tvar*_tmp58=_cycalloc(sizeof(*
_tmp58));_tmp58->name=nm;_tmp58->identity=- 1;_tmp58->kind=(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp59=_cycalloc_atomic(sizeof(*_tmp59));_tmp59[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp5A;_tmp5A.tag=0;_tmp5A.f1=Cyc_Absyn_RgnKind;_tmp5A;});_tmp59;});_tmp58;});
_tmp57;});_tmp56;});_tmp55;});goto _LL16;}_LL1B:;_LL1C: goto _LL16;_LL16:;}return
_tmp32;}static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*
x){struct Cyc_List_List*_tmp5F=0;for(0;x != 0;x=x->tl){void*_tmp60=((struct Cyc_Absyn_Aggrfield*)
x->hd)->type;void*_tmp62;_LL23: {struct Cyc_Absyn_TagType_struct*_tmp61=(struct
Cyc_Absyn_TagType_struct*)_tmp60;if(_tmp61->tag != 20)goto _LL25;else{_tmp62=(void*)
_tmp61->f1;}}_LL24: _tmp5F=({struct Cyc_List_List*_tmp63=_cycalloc(sizeof(*_tmp63));
_tmp63->hd=({struct _tuple15*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64->f1=((
struct Cyc_Absyn_Aggrfield*)x->hd)->name;_tmp64->f2=_tmp62;_tmp64;});_tmp63->tl=
_tmp5F;_tmp63;});goto _LL22;_LL25:;_LL26: goto _LL22;_LL22:;}return _tmp5F;}static
struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,
struct Cyc_Absyn_Exp*e){{void*_tmp65=e->r;struct _tuple1*_tmp67;struct _tuple1
_tmp68;union Cyc_Absyn_Nmspace _tmp69;struct Cyc_List_List*_tmp6A;struct
_dyneither_ptr*_tmp6B;struct _tuple1*_tmp6D;struct _tuple1 _tmp6E;union Cyc_Absyn_Nmspace
_tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr*_tmp71;_LL28: {struct Cyc_Absyn_Var_e_struct*
_tmp66=(struct Cyc_Absyn_Var_e_struct*)_tmp65;if(_tmp66->tag != 1)goto _LL2A;else{
_tmp67=_tmp66->f1;_tmp68=*_tmp67;_tmp69=_tmp68.f1;if((_tmp69.Rel_n).tag != 1)goto
_LL2A;_tmp6A=(struct Cyc_List_List*)(_tmp69.Rel_n).val;if(_tmp6A != 0)goto _LL2A;
_tmp6B=_tmp68.f2;}}_LL29: _tmp71=_tmp6B;goto _LL2B;_LL2A: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp6C=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp65;if(_tmp6C->tag != 2)goto _LL2C;
else{_tmp6D=_tmp6C->f1;_tmp6E=*_tmp6D;_tmp6F=_tmp6E.f1;if((_tmp6F.Rel_n).tag != 1)
goto _LL2C;_tmp70=(struct Cyc_List_List*)(_tmp6F.Rel_n).val;if(_tmp70 != 0)goto
_LL2C;_tmp71=_tmp6E.f2;}}_LL2B:{struct Cyc_List_List*_tmp72=tags;for(0;_tmp72 != 0;
_tmp72=_tmp72->tl){struct _tuple15 _tmp74;struct _dyneither_ptr*_tmp75;void*_tmp76;
struct _tuple15*_tmp73=(struct _tuple15*)_tmp72->hd;_tmp74=*_tmp73;_tmp75=_tmp74.f1;
_tmp76=_tmp74.f2;if(Cyc_strptrcmp(_tmp75,_tmp71)== 0)return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Valueof_e_struct*_tmp77=_cycalloc(sizeof(*_tmp77));
_tmp77[0]=({struct Cyc_Absyn_Valueof_e_struct _tmp78;_tmp78.tag=39;_tmp78.f1=(void*)
Cyc_Tcutil_copy_type(_tmp76);_tmp78;});_tmp77;}),e->loc);}}goto _LL27;_LL2C:;
_LL2D: goto _LL27;_LL27:;}return e;}static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*
tags,void*t){{void*_tmp79=t;struct Cyc_Absyn_ArrayInfo _tmp7B;void*_tmp7C;struct
Cyc_Absyn_Tqual _tmp7D;struct Cyc_Absyn_Exp*_tmp7E;union Cyc_Absyn_Constraint*
_tmp7F;struct Cyc_Position_Segment*_tmp80;struct Cyc_Absyn_PtrInfo _tmp82;void*
_tmp83;struct Cyc_Absyn_Tqual _tmp84;struct Cyc_Absyn_PtrAtts _tmp85;void*_tmp86;
union Cyc_Absyn_Constraint*_tmp87;union Cyc_Absyn_Constraint*_tmp88;union Cyc_Absyn_Constraint*
_tmp89;struct Cyc_Absyn_PtrLoc*_tmp8A;_LL2F: {struct Cyc_Absyn_ArrayType_struct*
_tmp7A=(struct Cyc_Absyn_ArrayType_struct*)_tmp79;if(_tmp7A->tag != 9)goto _LL31;
else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.elt_type;_tmp7D=_tmp7B.tq;_tmp7E=_tmp7B.num_elts;
_tmp7F=_tmp7B.zero_term;_tmp80=_tmp7B.zt_loc;}}_LL30: {struct Cyc_Absyn_Exp*
nelts2=_tmp7E;if(_tmp7E != 0)nelts2=(struct Cyc_Absyn_Exp*)Cyc_Parse_substitute_tags_exp(
tags,(struct Cyc_Absyn_Exp*)_tmp7E);{void*_tmp8B=Cyc_Parse_substitute_tags(tags,
_tmp7C);if(_tmp7E != nelts2  || _tmp7C != _tmp8B)return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp8D;_tmp8D.tag=9;_tmp8D.f1=({struct Cyc_Absyn_ArrayInfo _tmp8E;_tmp8E.elt_type=
_tmp8B;_tmp8E.tq=_tmp7D;_tmp8E.num_elts=nelts2;_tmp8E.zero_term=_tmp7F;_tmp8E.zt_loc=
_tmp80;_tmp8E;});_tmp8D;});_tmp8C;});goto _LL2E;};}_LL31: {struct Cyc_Absyn_PointerType_struct*
_tmp81=(struct Cyc_Absyn_PointerType_struct*)_tmp79;if(_tmp81->tag != 5)goto _LL33;
else{_tmp82=_tmp81->f1;_tmp83=_tmp82.elt_typ;_tmp84=_tmp82.elt_tq;_tmp85=_tmp82.ptr_atts;
_tmp86=_tmp85.rgn;_tmp87=_tmp85.nullable;_tmp88=_tmp85.bounds;_tmp89=_tmp85.zero_term;
_tmp8A=_tmp85.ptrloc;}}_LL32: {void*_tmp8F=Cyc_Parse_substitute_tags(tags,_tmp83);
union Cyc_Absyn_Constraint*_tmp90=_tmp88;{union Cyc_Absyn_Constraint _tmp91=*_tmp88;
void*_tmp92;struct Cyc_Absyn_Exp*_tmp94;_LL36: if((_tmp91.Eq_constr).tag != 1)goto
_LL38;_tmp92=(void*)(_tmp91.Eq_constr).val;{struct Cyc_Absyn_Upper_b_struct*
_tmp93=(struct Cyc_Absyn_Upper_b_struct*)_tmp92;if(_tmp93->tag != 1)goto _LL38;
else{_tmp94=_tmp93->f1;}};_LL37: _tmp94=Cyc_Parse_substitute_tags_exp(tags,_tmp94);
_tmp90=({union Cyc_Absyn_Constraint*_tmp95=_cycalloc(sizeof(*_tmp95));(_tmp95->Eq_constr).val=(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[
0]=({struct Cyc_Absyn_Upper_b_struct _tmp97;_tmp97.tag=1;_tmp97.f1=_tmp94;_tmp97;});
_tmp96;});(_tmp95->Eq_constr).tag=1;_tmp95;});goto _LL35;_LL38:;_LL39: goto _LL35;
_LL35:;}if(_tmp8F != _tmp83  || _tmp90 != _tmp88)return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98[0]=({struct Cyc_Absyn_PointerType_struct
_tmp99;_tmp99.tag=5;_tmp99.f1=({struct Cyc_Absyn_PtrInfo _tmp9A;_tmp9A.elt_typ=
_tmp8F;_tmp9A.elt_tq=_tmp84;_tmp9A.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9B;
_tmp9B.rgn=_tmp86;_tmp9B.nullable=_tmp87;_tmp9B.bounds=_tmp90;_tmp9B.zero_term=
_tmp89;_tmp9B.ptrloc=_tmp8A;_tmp9B;});_tmp9A;});_tmp99;});_tmp98;});goto _LL2E;}
_LL33:;_LL34: goto _LL2E;_LL2E:;}return t;}static void Cyc_Parse_substitute_aggrfield_tags(
struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){x->type=Cyc_Parse_substitute_tags(
tags,x->type);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static struct
_tuple16*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple8*t){
return({struct _tuple16*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->f1=(*t).f2;
_tmp9C->f2=(*t).f3;_tmp9C;});}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp9D=d->r;struct Cyc_Absyn_Vardecl*_tmp9F;_LL3B: {
struct Cyc_Absyn_Var_d_struct*_tmp9E=(struct Cyc_Absyn_Var_d_struct*)_tmp9D;if(
_tmp9E->tag != 0)goto _LL3D;else{_tmp9F=_tmp9E->f1;}}_LL3C: return _tmp9F;_LL3D:;
_LL3E:({void*_tmpA0=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(d->loc,({const char*
_tmpA1="bad declaration in `forarray' statement";_tag_dyneither(_tmpA1,sizeof(
char),40);}),_tag_dyneither(_tmpA0,sizeof(void*),0));});_LL3A:;}static int Cyc_Parse_is_typeparam(
void*tm){void*_tmpA2=tm;_LL40: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpA3=(
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpA2;if(_tmpA3->tag != 4)goto _LL42;}
_LL41: return 1;_LL42:;_LL43: return 0;_LL3F:;}static void*Cyc_Parse_id2type(struct
_dyneither_ptr s,void*k){if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*
_tmpA4="`H";_tag_dyneither(_tmpA4,sizeof(char),3);}))== 0)return(void*)& Cyc_Absyn_HeapRgn_val;
else{if(Cyc_zstrcmp((struct _dyneither_ptr)s,({const char*_tmpA5="`U";
_tag_dyneither(_tmpA5,sizeof(char),3);}))== 0)return(void*)& Cyc_Absyn_UniqueRgn_val;
else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmpA6=_cycalloc(sizeof(*
_tmpA6));_tmpA6[0]=({struct Cyc_Absyn_VarType_struct _tmpA7;_tmpA7.tag=2;_tmpA7.f1=({
struct Cyc_Absyn_Tvar*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->name=({struct
_dyneither_ptr*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=s;_tmpA9;});_tmpA8->identity=
- 1;_tmpA8->kind=k;_tmpA8;});_tmpA7;});_tmpA6;});}}}static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{void*_tmpAA=Cyc_Absyn_compress_kb(
t->kind);enum Cyc_Absyn_Kind _tmpAD;_LL45: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpAB=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpAA;if(_tmpAB->tag != 1)goto _LL47;}
_LL46: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmpAE=_cycalloc(sizeof(*
_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmpAF;_tmpAF.tag=1;_tmpAF.f1=
0;_tmpAF;});_tmpAE;});goto _LL44;_LL47: {struct Cyc_Absyn_Less_kb_struct*_tmpAC=(
struct Cyc_Absyn_Less_kb_struct*)_tmpAA;if(_tmpAC->tag != 2)goto _LL49;else{_tmpAD=
_tmpAC->f2;}}_LL48: k=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmpB0=_cycalloc(
sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_Less_kb_struct _tmpB1;_tmpB1.tag=2;
_tmpB1.f1=0;_tmpB1.f2=_tmpAD;_tmpB1;});_tmpB0;});goto _LL44;_LL49:;_LL4A: k=_tmpAA;
goto _LL44;_LL44:;}return({struct Cyc_Absyn_Tvar*_tmpB2=_cycalloc(sizeof(*_tmpB2));
_tmpB2->name=t->name;_tmpB2->identity=- 1;_tmpB2->kind=k;_tmpB2;});}static struct
Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,void*t){void*
_tmpB3=t;struct Cyc_Absyn_Tvar*_tmpB5;_LL4C: {struct Cyc_Absyn_VarType_struct*
_tmpB4=(struct Cyc_Absyn_VarType_struct*)_tmpB3;if(_tmpB4->tag != 2)goto _LL4E;
else{_tmpB5=_tmpB4->f1;}}_LL4D: return _tmpB5;_LL4E:;_LL4F:({void*_tmpB6=0;((int(*)(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Parse_abort)(loc,({const char*_tmpB7="expecting a list of type variables, not types";
_tag_dyneither(_tmpB7,sizeof(char),46);}),_tag_dyneither(_tmpB6,sizeof(void*),0));});
_LL4B:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){return(void*)({
struct Cyc_Absyn_VarType_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({
struct Cyc_Absyn_VarType_struct _tmpB9;_tmpB9.tag=2;_tmpB9.f1=pr;_tmpB9;});_tmpB8;});}
static void Cyc_Parse_set_vartyp_kind(void*t,enum Cyc_Absyn_Kind k,int leq){void*
_tmpBA=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpBC;struct Cyc_Absyn_Tvar
_tmpBD;void*_tmpBE;void**_tmpBF;_LL51: {struct Cyc_Absyn_VarType_struct*_tmpBB=(
struct Cyc_Absyn_VarType_struct*)_tmpBA;if(_tmpBB->tag != 2)goto _LL53;else{_tmpBC=
_tmpBB->f1;_tmpBD=*_tmpBC;_tmpBE=_tmpBD.kind;_tmpBF=(void**)&(*_tmpBB->f1).kind;}}
_LL52: {void*_tmpC0=Cyc_Absyn_compress_kb(*_tmpBF);_LL56: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpC1=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpC0;if(_tmpC1->tag != 1)goto _LL58;}
_LL57: if(!leq)*_tmpBF=Cyc_Tcutil_kind_to_bound(k);else{*_tmpBF=(void*)({struct
Cyc_Absyn_Less_kb_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_Less_kb_struct
_tmpC3;_tmpC3.tag=2;_tmpC3.f1=0;_tmpC3.f2=k;_tmpC3;});_tmpC2;});}return;_LL58:;
_LL59: return;_LL55:;}_LL53:;_LL54: return;_LL50:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmpC4=(void*)tms->hd;void*_tmpC6;
_LL5B: {struct Cyc_Absyn_Function_mod_struct*_tmpC5=(struct Cyc_Absyn_Function_mod_struct*)
_tmpC4;if(_tmpC5->tag != 3)goto _LL5D;else{_tmpC6=(void*)_tmpC5->f1;}}_LL5C: if(tms->tl
== 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)
 && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){void*_tmpC7=_tmpC6;
struct Cyc_List_List*_tmpCA;_LL60: {struct Cyc_Absyn_WithTypes_struct*_tmpC8=(
struct Cyc_Absyn_WithTypes_struct*)_tmpC7;if(_tmpC8->tag != 1)goto _LL62;}_LL61:({
void*_tmpCB=0;Cyc_Tcutil_warn(loc,({const char*_tmpCC="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dyneither(_tmpCC,sizeof(char),93);}),_tag_dyneither(_tmpCB,sizeof(void*),0));});
return tms;_LL62: {struct Cyc_Absyn_NoTypes_struct*_tmpC9=(struct Cyc_Absyn_NoTypes_struct*)
_tmpC7;if(_tmpC9->tag != 0)goto _LL5F;else{_tmpCA=_tmpC9->f1;}}_LL63: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpCA)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds))({void*_tmpCD=0;((int(*)(struct Cyc_Position_Segment*sg,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmpCE="wrong number of parameter declarations in old-style function declaration";
_tag_dyneither(_tmpCE,sizeof(char),73);}),_tag_dyneither(_tmpCD,sizeof(void*),0));});{
struct Cyc_List_List*_tmpCF=0;for(0;_tmpCA != 0;_tmpCA=_tmpCA->tl){struct Cyc_List_List*
_tmpD0=tds;for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){struct Cyc_Absyn_Decl*_tmpD1=(
struct Cyc_Absyn_Decl*)_tmpD0->hd;void*_tmpD2=_tmpD1->r;struct Cyc_Absyn_Vardecl*
_tmpD4;_LL65: {struct Cyc_Absyn_Var_d_struct*_tmpD3=(struct Cyc_Absyn_Var_d_struct*)
_tmpD2;if(_tmpD3->tag != 0)goto _LL67;else{_tmpD4=_tmpD3->f1;}}_LL66: if(Cyc_zstrptrcmp((*
_tmpD4->name).f2,(struct _dyneither_ptr*)_tmpCA->hd)!= 0)continue;if(_tmpD4->initializer
!= 0)({void*_tmpD5=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,({const char*_tmpD6="initializer found in parameter declaration";
_tag_dyneither(_tmpD6,sizeof(char),43);}),_tag_dyneither(_tmpD5,sizeof(void*),0));});
if(Cyc_Absyn_is_qvar_qualified(_tmpD4->name))({void*_tmpD7=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,({
const char*_tmpD8="namespaces forbidden in parameter declarations";_tag_dyneither(
_tmpD8,sizeof(char),47);}),_tag_dyneither(_tmpD7,sizeof(void*),0));});_tmpCF=({
struct Cyc_List_List*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->hd=({struct _tuple8*
_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->f1=({struct Cyc_Core_Opt*_tmpDB=
_cycalloc(sizeof(*_tmpDB));_tmpDB->v=(*_tmpD4->name).f2;_tmpDB;});_tmpDA->f2=
_tmpD4->tq;_tmpDA->f3=_tmpD4->type;_tmpDA;});_tmpD9->tl=_tmpCF;_tmpD9;});goto L;
_LL67:;_LL68:({void*_tmpDC=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmpD1->loc,({const
char*_tmpDD="nonvariable declaration in parameter type";_tag_dyneither(_tmpDD,
sizeof(char),42);}),_tag_dyneither(_tmpDC,sizeof(void*),0));});_LL64:;}L: if(
_tmpD0 == 0)({struct Cyc_String_pa_struct _tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpCA->hd));({
void*_tmpDE[1]={& _tmpE0};((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmpDF="%s is not given a type";_tag_dyneither(_tmpDF,sizeof(char),23);}),
_tag_dyneither(_tmpDE,sizeof(void*),1));});});}return({struct Cyc_List_List*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[0]=({struct Cyc_Absyn_Function_mod_struct
_tmpE3;_tmpE3.tag=3;_tmpE3.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({struct Cyc_Absyn_WithTypes_struct
_tmpE5;_tmpE5.tag=1;_tmpE5.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmpCF);_tmpE5.f2=0;_tmpE5.f3=0;_tmpE5.f4=0;_tmpE5.f5=0;_tmpE5;});
_tmpE4;}));_tmpE3;});_tmpE2;});_tmpE1->tl=0;_tmpE1;});};_LL5F:;}goto _LL5E;_LL5D:;
_LL5E: return({struct Cyc_List_List*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->hd=(
void*)tms->hd;_tmpE6->tl=Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmpE6;});
_LL5A:;};}struct _tuple17{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;
};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,
struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,
struct Cyc_Position_Segment*loc){if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmpE7=
_cycalloc(sizeof(*_tmpE7));_tmpE7->id=d->id;_tmpE7->tms=Cyc_Parse_oldstyle2newstyle(
d->tms,tds,loc);_tmpE7;});{enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;struct Cyc_List_List*
tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*
atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;
tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)
dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;
if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0)switch((enum Cyc_Parse_Storage_class)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc))->v){
case Cyc_Parse_Extern_sc: _LL69: sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc:
_LL6A: sc=Cyc_Absyn_Static;break;default: _LL6B: Cyc_Parse_err(({const char*_tmpE8="bad storage class on function";
_tag_dyneither(_tmpE8,sizeof(char),30);}),loc);break;}}{void*_tmpEA;struct Cyc_Core_Opt*
_tmpEB;struct _tuple10 _tmpE9=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpEA=
_tmpE9.f1;_tmpEB=_tmpE9.f2;{struct Cyc_Absyn_Tqual _tmpED;void*_tmpEE;struct Cyc_List_List*
_tmpEF;struct Cyc_List_List*_tmpF0;struct _tuple11 _tmpEC=Cyc_Parse_apply_tms(tq,
_tmpEA,atts,d->tms);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;_tmpEF=_tmpEC.f3;_tmpF0=
_tmpEC.f4;if(_tmpEB != 0)({void*_tmpF1=0;Cyc_Tcutil_warn(loc,({const char*_tmpF2="nested type declaration within function prototype";
_tag_dyneither(_tmpF2,sizeof(char),50);}),_tag_dyneither(_tmpF1,sizeof(void*),0));});
if(_tmpEF != 0)({void*_tmpF3=0;Cyc_Tcutil_warn(loc,({const char*_tmpF4="bad type params, ignoring";
_tag_dyneither(_tmpF4,sizeof(char),26);}),_tag_dyneither(_tmpF3,sizeof(void*),0));});{
void*_tmpF5=_tmpEE;struct Cyc_Absyn_FnInfo _tmpF7;struct Cyc_List_List*_tmpF8;
struct Cyc_Core_Opt*_tmpF9;void*_tmpFA;struct Cyc_List_List*_tmpFB;int _tmpFC;
struct Cyc_Absyn_VarargInfo*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_List_List*
_tmpFF;_LL6E: {struct Cyc_Absyn_FnType_struct*_tmpF6=(struct Cyc_Absyn_FnType_struct*)
_tmpF5;if(_tmpF6->tag != 10)goto _LL70;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF7.tvars;
_tmpF9=_tmpF7.effect;_tmpFA=_tmpF7.ret_typ;_tmpFB=_tmpF7.args;_tmpFC=_tmpF7.c_varargs;
_tmpFD=_tmpF7.cyc_varargs;_tmpFE=_tmpF7.rgn_po;_tmpFF=_tmpF7.attributes;}}_LL6F: {
struct Cyc_List_List*_tmp100=0;{struct Cyc_List_List*_tmp101=_tmpFB;for(0;_tmp101
!= 0;_tmp101=_tmp101->tl){struct _tuple8 _tmp103;struct Cyc_Core_Opt*_tmp104;struct
Cyc_Absyn_Tqual _tmp105;void*_tmp106;struct _tuple8*_tmp102=(struct _tuple8*)
_tmp101->hd;_tmp103=*_tmp102;_tmp104=_tmp103.f1;_tmp105=_tmp103.f2;_tmp106=
_tmp103.f3;if(_tmp104 == 0){Cyc_Parse_err(({const char*_tmp107="missing argument variable in function prototype";
_tag_dyneither(_tmp107,sizeof(char),48);}),loc);_tmp100=({struct Cyc_List_List*
_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->hd=({struct _tuple17*_tmp109=
_cycalloc(sizeof(*_tmp109));_tmp109->f1=_init_dyneither_ptr(_cycalloc(sizeof(
struct _dyneither_ptr)),"?",sizeof(char),2);_tmp109->f2=_tmp105;_tmp109->f3=
_tmp106;_tmp109;});_tmp108->tl=_tmp100;_tmp108;});}else{_tmp100=({struct Cyc_List_List*
_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->hd=({struct _tuple17*_tmp10C=
_cycalloc(sizeof(*_tmp10C));_tmp10C->f1=(struct _dyneither_ptr*)_tmp104->v;
_tmp10C->f2=_tmp105;_tmp10C->f3=_tmp106;_tmp10C;});_tmp10B->tl=_tmp100;_tmp10B;});}}}
return({struct Cyc_Absyn_Fndecl*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->sc=
sc;_tmp10D->name=d->id;_tmp10D->tvs=_tmpF8;_tmp10D->is_inline=is_inline;_tmp10D->effect=
_tmpF9;_tmp10D->ret_type=_tmpFA;_tmp10D->args=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp100);_tmp10D->c_varargs=_tmpFC;_tmp10D->cyc_varargs=
_tmpFD;_tmp10D->rgn_po=_tmpFE;_tmp10D->body=body;_tmp10D->cached_typ=0;_tmp10D->param_vardecls=
0;_tmp10D->fn_vardecl=0;_tmp10D->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmpFF,_tmpF0);_tmp10D;});}_LL70:;
_LL71:({void*_tmp10E=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmp10F="declarator is not a function prototype";_tag_dyneither(_tmp10F,sizeof(
char),39);}),_tag_dyneither(_tmp10E,sizeof(void*),0));});_LL6D:;};};};};}static
char _tmp110[52]="at most one type may appear within a type specifier";static
struct _dyneither_ptr Cyc_Parse_msg1={_tmp110,_tmp110,_tmp110 + 52};static char
_tmp111[63]="const or volatile may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg2={_tmp111,_tmp111,_tmp111 + 63};static
char _tmp112[50]="type specifier includes more than one declaration";static struct
_dyneither_ptr Cyc_Parse_msg3={_tmp112,_tmp112,_tmp112 + 50};static char _tmp113[60]="sign specifier may appear only once within a type specifier";
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp113,_tmp113,_tmp113 + 60};static
struct _tuple10 Cyc_Parse_collapse_type_specifiers(struct Cyc_List_List*ts,struct
Cyc_Position_Segment*loc){struct Cyc_Core_Opt*declopt=0;int seen_type=0;int
seen_sign=0;int seen_size=0;void*t=(void*)& Cyc_Absyn_VoidType_val;enum Cyc_Absyn_Size_of
sz=Cyc_Absyn_Int_sz;enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;struct Cyc_Position_Segment*
last_loc=loc;for(0;ts != 0;ts=ts->tl){void*_tmp114=(void*)ts->hd;void*_tmp116;
struct Cyc_Position_Segment*_tmp117;struct Cyc_Position_Segment*_tmp119;struct Cyc_Position_Segment*
_tmp11B;struct Cyc_Position_Segment*_tmp11D;struct Cyc_Position_Segment*_tmp11F;
struct Cyc_Absyn_Decl*_tmp121;_LL73: {struct Cyc_Parse_Type_spec_struct*_tmp115=(
struct Cyc_Parse_Type_spec_struct*)_tmp114;if(_tmp115->tag != 4)goto _LL75;else{
_tmp116=(void*)_tmp115->f1;_tmp117=_tmp115->f2;}}_LL74: if(seen_type)Cyc_Parse_err(
Cyc_Parse_msg1,_tmp117);last_loc=_tmp117;seen_type=1;t=_tmp116;goto _LL72;_LL75: {
struct Cyc_Parse_Signed_spec_struct*_tmp118=(struct Cyc_Parse_Signed_spec_struct*)
_tmp114;if(_tmp118->tag != 0)goto _LL77;else{_tmp119=_tmp118->f1;}}_LL76: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp119);if(seen_type)Cyc_Parse_err(({
const char*_tmp122="signed qualifier must come before type";_tag_dyneither(
_tmp122,sizeof(char),39);}),_tmp119);last_loc=_tmp119;seen_sign=1;sgn=Cyc_Absyn_Signed;
goto _LL72;_LL77: {struct Cyc_Parse_Unsigned_spec_struct*_tmp11A=(struct Cyc_Parse_Unsigned_spec_struct*)
_tmp114;if(_tmp11A->tag != 1)goto _LL79;else{_tmp11B=_tmp11A->f1;}}_LL78: if(
seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp11B);if(seen_type)Cyc_Parse_err(({
const char*_tmp123="signed qualifier must come before type";_tag_dyneither(
_tmp123,sizeof(char),39);}),_tmp11B);last_loc=_tmp11B;seen_sign=1;sgn=Cyc_Absyn_Unsigned;
goto _LL72;_LL79: {struct Cyc_Parse_Short_spec_struct*_tmp11C=(struct Cyc_Parse_Short_spec_struct*)
_tmp114;if(_tmp11C->tag != 2)goto _LL7B;else{_tmp11D=_tmp11C->f1;}}_LL7A: if(
seen_size)Cyc_Parse_err(({const char*_tmp124="integral size may appear only once within a type specifier";
_tag_dyneither(_tmp124,sizeof(char),59);}),_tmp11D);if(seen_type)Cyc_Parse_err(({
const char*_tmp125="short modifier must come before base type";_tag_dyneither(
_tmp125,sizeof(char),42);}),_tmp11D);last_loc=_tmp11D;seen_size=1;sz=Cyc_Absyn_Short_sz;
goto _LL72;_LL7B: {struct Cyc_Parse_Long_spec_struct*_tmp11E=(struct Cyc_Parse_Long_spec_struct*)
_tmp114;if(_tmp11E->tag != 3)goto _LL7D;else{_tmp11F=_tmp11E->f1;}}_LL7C: if(
seen_type)Cyc_Parse_err(({const char*_tmp126="long modifier must come before base type";
_tag_dyneither(_tmp126,sizeof(char),41);}),_tmp11F);if(seen_size)switch(sz){case
Cyc_Absyn_Long_sz: _LL7F: sz=Cyc_Absyn_LongLong_sz;break;default: _LL80: Cyc_Parse_err(({
const char*_tmp127="extra long in type specifier";_tag_dyneither(_tmp127,sizeof(
char),29);}),_tmp11F);break;}else{sz=Cyc_Absyn_Long_sz;}last_loc=_tmp11F;
seen_size=1;goto _LL72;_LL7D: {struct Cyc_Parse_Decl_spec_struct*_tmp120=(struct
Cyc_Parse_Decl_spec_struct*)_tmp114;if(_tmp120->tag != 5)goto _LL72;else{_tmp121=
_tmp120->f1;}}_LL7E: last_loc=_tmp121->loc;if(declopt == 0  && !seen_type){
seen_type=1;{void*_tmp128=_tmp121->r;struct Cyc_Absyn_Aggrdecl*_tmp12A;struct Cyc_Absyn_Datatypedecl*
_tmp12C;struct Cyc_Absyn_Enumdecl*_tmp12E;_LL83: {struct Cyc_Absyn_Aggr_d_struct*
_tmp129=(struct Cyc_Absyn_Aggr_d_struct*)_tmp128;if(_tmp129->tag != 6)goto _LL85;
else{_tmp12A=_tmp129->f1;}}_LL84: {struct Cyc_List_List*_tmp12F=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp12A->tvs));t=(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130[0]=({
struct Cyc_Absyn_AggrType_struct _tmp131;_tmp131.tag=12;_tmp131.f1=({struct Cyc_Absyn_AggrInfo
_tmp132;_tmp132.aggr_info=Cyc_Absyn_UnknownAggr(_tmp12A->kind,_tmp12A->name,0);
_tmp132.targs=_tmp12F;_tmp132;});_tmp131;});_tmp130;});if(_tmp12A->impl != 0)
declopt=({struct Cyc_Core_Opt*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->v=
_tmp121;_tmp133;});goto _LL82;}_LL85: {struct Cyc_Absyn_Datatype_d_struct*_tmp12B=(
struct Cyc_Absyn_Datatype_d_struct*)_tmp128;if(_tmp12B->tag != 7)goto _LL87;else{
_tmp12C=_tmp12B->f1;}}_LL86: {struct Cyc_List_List*_tmp134=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmp12C->tvs));t=(void*)({
struct Cyc_Absyn_DatatypeType_struct*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135[
0]=({struct Cyc_Absyn_DatatypeType_struct _tmp136;_tmp136.tag=3;_tmp136.f1=({
struct Cyc_Absyn_DatatypeInfo _tmp137;_tmp137.datatype_info=Cyc_Absyn_KnownDatatype(({
struct Cyc_Absyn_Datatypedecl**_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138[0]=
_tmp12C;_tmp138;}));_tmp137.targs=_tmp134;_tmp137;});_tmp136;});_tmp135;});if(
_tmp12C->fields != 0)declopt=({struct Cyc_Core_Opt*_tmp139=_cycalloc(sizeof(*
_tmp139));_tmp139->v=_tmp121;_tmp139;});goto _LL82;}_LL87: {struct Cyc_Absyn_Enum_d_struct*
_tmp12D=(struct Cyc_Absyn_Enum_d_struct*)_tmp128;if(_tmp12D->tag != 8)goto _LL89;
else{_tmp12E=_tmp12D->f1;}}_LL88: t=(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=({struct Cyc_Absyn_EnumType_struct
_tmp13B;_tmp13B.tag=14;_tmp13B.f1=_tmp12E->name;_tmp13B.f2=0;_tmp13B;});_tmp13A;});
declopt=({struct Cyc_Core_Opt*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->v=
_tmp121;_tmp13C;});goto _LL82;_LL89:;_LL8A:({void*_tmp13D=0;((int(*)(struct Cyc_Position_Segment*
sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(_tmp121->loc,({
const char*_tmp13E="bad declaration within type specifier";_tag_dyneither(_tmp13E,
sizeof(char),38);}),_tag_dyneither(_tmp13D,sizeof(void*),0));});_LL82:;};}else{
Cyc_Parse_err(Cyc_Parse_msg3,_tmp121->loc);}goto _LL72;_LL72:;}if(!seen_type){if(
!seen_sign  && !seen_size)({void*_tmp13F=0;Cyc_Tcutil_warn(last_loc,({const char*
_tmp140="missing type within specifier";_tag_dyneither(_tmp140,sizeof(char),30);}),
_tag_dyneither(_tmp13F,sizeof(void*),0));});t=Cyc_Absyn_int_typ(sgn,sz);}else{
if(seen_sign){void*_tmp141=t;enum Cyc_Absyn_Sign _tmp143;enum Cyc_Absyn_Size_of
_tmp144;_LL8C: {struct Cyc_Absyn_IntType_struct*_tmp142=(struct Cyc_Absyn_IntType_struct*)
_tmp141;if(_tmp142->tag != 6)goto _LL8E;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;}}
_LL8D: if(_tmp143 != sgn)t=Cyc_Absyn_int_typ(sgn,_tmp144);goto _LL8B;_LL8E:;_LL8F:
Cyc_Parse_err(({const char*_tmp145="sign specification on non-integral type";
_tag_dyneither(_tmp145,sizeof(char),40);}),last_loc);goto _LL8B;_LL8B:;}if(
seen_size){void*_tmp146=t;enum Cyc_Absyn_Sign _tmp148;enum Cyc_Absyn_Size_of
_tmp149;_LL91: {struct Cyc_Absyn_IntType_struct*_tmp147=(struct Cyc_Absyn_IntType_struct*)
_tmp146;if(_tmp147->tag != 6)goto _LL93;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}
_LL92: if(_tmp149 != sz)t=Cyc_Absyn_int_typ(_tmp148,sz);goto _LL90;_LL93: {struct
Cyc_Absyn_DoubleType_struct*_tmp14A=(struct Cyc_Absyn_DoubleType_struct*)_tmp146;
if(_tmp14A->tag != 8)goto _LL95;}_LL94: t=Cyc_Absyn_double_typ(1);goto _LL90;_LL95:;
_LL96: Cyc_Parse_err(({const char*_tmp14B="size qualifier on non-integral type";
_tag_dyneither(_tmp14B,sizeof(char),36);}),last_loc);goto _LL90;_LL90:;}}return({
struct _tuple10 _tmp14C;_tmp14C.f1=t;_tmp14C.f2=declopt;_tmp14C;});}static struct
Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{
struct Cyc_Parse_Declarator*_tmp14D=(struct Cyc_Parse_Declarator*)ds->hd;struct
_tuple1*_tmp14E=_tmp14D->id;struct Cyc_Absyn_Tqual _tmp150;void*_tmp151;struct Cyc_List_List*
_tmp152;struct Cyc_List_List*_tmp153;struct _tuple11 _tmp14F=Cyc_Parse_apply_tms(tq,
t,shared_atts,_tmp14D->tms);_tmp150=_tmp14F.f1;_tmp151=_tmp14F.f2;_tmp152=
_tmp14F.f3;_tmp153=_tmp14F.f4;if(ds->tl == 0)return({struct Cyc_List_List*_tmp154=
_region_malloc(r,sizeof(*_tmp154));_tmp154->hd=({struct _tuple13*_tmp155=
_region_malloc(r,sizeof(*_tmp155));_tmp155->f1=_tmp14E;_tmp155->f2=_tmp150;
_tmp155->f3=_tmp151;_tmp155->f4=_tmp152;_tmp155->f5=_tmp153;_tmp155;});_tmp154->tl=
0;_tmp154;});else{return({struct Cyc_List_List*_tmp156=_region_malloc(r,sizeof(*
_tmp156));_tmp156->hd=({struct _tuple13*_tmp157=_region_malloc(r,sizeof(*_tmp157));
_tmp157->f1=_tmp14E;_tmp157->f2=_tmp150;_tmp157->f3=_tmp151;_tmp157->f4=_tmp152;
_tmp157->f5=_tmp153;_tmp157;});_tmp156->tl=Cyc_Parse_apply_tmss(r,_tmp150,Cyc_Tcutil_copy_type(
t),ds->tl,shared_atts);_tmp156;});}};}static struct _tuple11 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
if(tms == 0)return({struct _tuple11 _tmp158;_tmp158.f1=tq;_tmp158.f2=t;_tmp158.f3=0;
_tmp158.f4=atts;_tmp158;});{void*_tmp159=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmp15B;struct Cyc_Position_Segment*_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;union Cyc_Absyn_Constraint*
_tmp15F;struct Cyc_Position_Segment*_tmp160;void*_tmp162;struct Cyc_List_List*
_tmp164;struct Cyc_Position_Segment*_tmp165;struct Cyc_Absyn_PtrAtts _tmp167;struct
Cyc_Absyn_Tqual _tmp168;struct Cyc_Position_Segment*_tmp16A;struct Cyc_List_List*
_tmp16B;_LL98: {struct Cyc_Absyn_Carray_mod_struct*_tmp15A=(struct Cyc_Absyn_Carray_mod_struct*)
_tmp159;if(_tmp15A->tag != 0)goto _LL9A;else{_tmp15B=_tmp15A->f1;_tmp15C=_tmp15A->f2;}}
_LL99: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,
tq,0,_tmp15B,_tmp15C),atts,tms->tl);_LL9A: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmp15D=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmp159;if(_tmp15D->tag != 1)goto
_LL9C;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;_tmp160=_tmp15D->f3;}}_LL9B:
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(
struct Cyc_Absyn_Exp*)_tmp15E,_tmp15F,_tmp160),atts,tms->tl);_LL9C: {struct Cyc_Absyn_Function_mod_struct*
_tmp161=(struct Cyc_Absyn_Function_mod_struct*)_tmp159;if(_tmp161->tag != 3)goto
_LL9E;else{_tmp162=(void*)_tmp161->f1;}}_LL9D: {void*_tmp16C=_tmp162;struct Cyc_List_List*
_tmp16E;int _tmp16F;struct Cyc_Absyn_VarargInfo*_tmp170;struct Cyc_Core_Opt*_tmp171;
struct Cyc_List_List*_tmp172;struct Cyc_Position_Segment*_tmp174;_LLA5: {struct Cyc_Absyn_WithTypes_struct*
_tmp16D=(struct Cyc_Absyn_WithTypes_struct*)_tmp16C;if(_tmp16D->tag != 1)goto _LLA7;
else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;_tmp170=_tmp16D->f3;_tmp171=_tmp16D->f4;
_tmp172=_tmp16D->f5;}}_LLA6: {struct Cyc_List_List*typvars=0;struct Cyc_List_List*
fn_atts=0;struct Cyc_List_List*new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 
0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))fn_atts=({struct Cyc_List_List*
_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->hd=(void*)as->hd;_tmp175->tl=
fn_atts;_tmp175;});else{new_atts=({struct Cyc_List_List*_tmp176=_cycalloc(sizeof(*
_tmp176));_tmp176->hd=(void*)as->hd;_tmp176->tl=new_atts;_tmp176;});}}}if(tms->tl
!= 0){void*_tmp177=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
struct Cyc_List_List*_tmp179;_LLAA: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp178=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp177;if(_tmp178->tag != 4)goto
_LLAC;else{_tmp179=_tmp178->f1;}}_LLAB: typvars=_tmp179;tms=tms->tl;goto _LLA9;
_LLAC:;_LLAD: goto _LLA9;_LLA9:;}if(((((!_tmp16F  && _tmp170 == 0) && _tmp16E != 0)
 && _tmp16E->tl == 0) && (*((struct _tuple8*)_tmp16E->hd)).f1 == 0) && (*((struct
_tuple8*)_tmp16E->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)_tmp16E=0;{struct Cyc_List_List*
_tmp17A=Cyc_Parse_get_arg_tags(_tmp16E);if(_tmp17A != 0)t=Cyc_Parse_substitute_tags(
_tmp17A,t);t=Cyc_Parse_array2ptr(t,0);{struct Cyc_List_List*_tmp17B=_tmp16E;for(0;
_tmp17B != 0;_tmp17B=_tmp17B->tl){struct _tuple8 _tmp17D;struct Cyc_Core_Opt*_tmp17E;
struct Cyc_Absyn_Tqual _tmp17F;void*_tmp180;void**_tmp181;struct _tuple8*_tmp17C=(
struct _tuple8*)_tmp17B->hd;_tmp17D=*_tmp17C;_tmp17E=_tmp17D.f1;_tmp17F=_tmp17D.f2;
_tmp180=_tmp17D.f3;_tmp181=(void**)&(*_tmp17C).f3;if(_tmp17A != 0)*_tmp181=Cyc_Parse_substitute_tags(
_tmp17A,*_tmp181);*_tmp181=Cyc_Parse_array2ptr(*_tmp181,1);}}return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(typvars,_tmp171,t,_tmp16E,
_tmp16F,_tmp170,_tmp172,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}
_LLA7: {struct Cyc_Absyn_NoTypes_struct*_tmp173=(struct Cyc_Absyn_NoTypes_struct*)
_tmp16C;if(_tmp173->tag != 0)goto _LLA4;else{_tmp174=_tmp173->f2;}}_LLA8:({void*
_tmp182=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(_tmp174,({const char*_tmp183="function declaration without parameter types";
_tag_dyneither(_tmp183,sizeof(char),45);}),_tag_dyneither(_tmp182,sizeof(void*),
0));});_LLA4:;}_LL9E: {struct Cyc_Absyn_TypeParams_mod_struct*_tmp163=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp159;if(_tmp163->tag != 4)goto _LLA0;else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;}}
_LL9F: if(tms->tl == 0)return({struct _tuple11 _tmp184;_tmp184.f1=tq;_tmp184.f2=t;
_tmp184.f3=_tmp164;_tmp184.f4=atts;_tmp184;});({void*_tmp185=0;((int(*)(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_abort)(
_tmp165,({const char*_tmp186="type parameters must appear before function arguments in declarator";
_tag_dyneither(_tmp186,sizeof(char),68);}),_tag_dyneither(_tmp185,sizeof(void*),
0));});_LLA0: {struct Cyc_Absyn_Pointer_mod_struct*_tmp166=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp159;if(_tmp166->tag != 2)goto _LLA2;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}
_LLA1: return Cyc_Parse_apply_tms(_tmp168,(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_PointerType_struct
_tmp188;_tmp188.tag=5;_tmp188.f1=({struct Cyc_Absyn_PtrInfo _tmp189;_tmp189.elt_typ=
t;_tmp189.elt_tq=tq;_tmp189.ptr_atts=_tmp167;_tmp189;});_tmp188;});_tmp187;}),
atts,tms->tl);_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*_tmp169=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmp159;if(_tmp169->tag != 5)goto _LL97;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}
_LLA3: return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp16B),tms->tl);_LL97:;};}void*
Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){
void*_tmp18B;struct Cyc_Core_Opt*_tmp18C;struct _tuple10 _tmp18A=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp18B=_tmp18A.f1;_tmp18C=_tmp18A.f2;if(_tmp18C != 0)({void*_tmp18D=0;
Cyc_Tcutil_warn(loc,({const char*_tmp18E="ignoring nested type declaration(s) in specifier list";
_tag_dyneither(_tmp18E,sizeof(char),54);}),_tag_dyneither(_tmp18D,sizeof(void*),
0));});return _tmp18B;}static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment*loc,struct _tuple13*t){struct _tuple13 _tmp190;struct
_tuple1*_tmp191;struct Cyc_Absyn_Tqual _tmp192;void*_tmp193;struct Cyc_List_List*
_tmp194;struct Cyc_List_List*_tmp195;struct _tuple13*_tmp18F=t;_tmp190=*_tmp18F;
_tmp191=_tmp190.f1;_tmp192=_tmp190.f2;_tmp193=_tmp190.f3;_tmp194=_tmp190.f4;
_tmp195=_tmp190.f5;Cyc_Lex_register_typedef(_tmp191);{struct Cyc_Core_Opt*kind;
struct Cyc_Core_Opt*type;{void*_tmp196=_tmp193;struct Cyc_Core_Opt*_tmp198;_LLAF: {
struct Cyc_Absyn_Evar_struct*_tmp197=(struct Cyc_Absyn_Evar_struct*)_tmp196;if(
_tmp197->tag != 1)goto _LLB1;else{_tmp198=_tmp197->f1;}}_LLB0: type=0;if(_tmp198 == 
0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;else{kind=_tmp198;}goto _LLAE;_LLB1:;
_LLB2: kind=0;type=({struct Cyc_Core_Opt*_tmp199=_cycalloc(sizeof(*_tmp199));
_tmp199->v=_tmp193;_tmp199;});goto _LLAE;_LLAE:;}return Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Typedef_d_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A[0]=({
struct Cyc_Absyn_Typedef_d_struct _tmp19B;_tmp19B.tag=9;_tmp19B.f1=({struct Cyc_Absyn_Typedefdecl*
_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->name=_tmp191;_tmp19C->tvs=_tmp194;
_tmp19C->kind=kind;_tmp19C->defn=type;_tmp19C->atts=_tmp195;_tmp19C->tq=_tmp192;
_tmp19C;});_tmp19B;});_tmp19A;}),loc);};}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Decl_s_struct*_tmp19D=_cycalloc(sizeof(*_tmp19D));_tmp19D[0]=({
struct Cyc_Absyn_Decl_s_struct _tmp19E;_tmp19E.tag=12;_tmp19E.f1=d;_tmp19E.f2=s;
_tmp19E;});_tmp19D;}),d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct _RegionHandle _tmp19F=_new_region("mkrgn");struct _RegionHandle*mkrgn=&
_tmp19F;_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp1A1;struct Cyc_Absyn_Tqual
_tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_List_List*_tmp1A4;struct Cyc_Parse_Declaration_spec*
_tmp1A0=ds;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.tq;_tmp1A3=_tmp1A1.type_specs;
_tmp1A4=_tmp1A1.attributes;if(_tmp1A2.loc == 0)_tmp1A2.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp1A5="inline is allowed only on function definitions";
_tag_dyneither(_tmp1A5,sizeof(char),47);}),loc);if(_tmp1A3 == 0){Cyc_Parse_err(({
const char*_tmp1A6="missing type specifiers in declaration";_tag_dyneither(
_tmp1A6,sizeof(char),39);}),loc);{struct Cyc_List_List*_tmp1A7=0;_npop_handler(0);
return _tmp1A7;};}{enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;int istypedef=0;if(ds->sc
!= 0)switch((enum Cyc_Parse_Storage_class)((struct Cyc_Core_Opt*)_check_null(ds->sc))->v){
case Cyc_Parse_Typedef_sc: _LLB3: istypedef=1;break;case Cyc_Parse_Extern_sc: _LLB4: s=
Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc: _LLB5: s=Cyc_Absyn_ExternC;break;
case Cyc_Parse_Static_sc: _LLB6: s=Cyc_Absyn_Static;break;case Cyc_Parse_Auto_sc:
_LLB7: s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc: _LLB8: if(Cyc_Parse_no_register)
s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}break;case Cyc_Parse_Abstract_sc:
_LLB9: s=Cyc_Absyn_Abstract;break;}{struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*
_tmp1AA;struct _tuple0 _tmp1A8=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(mkrgn,mkrgn,ids);
_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmp1AA;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=
0;break;}}}{struct _tuple10 _tmp1AB=Cyc_Parse_collapse_type_specifiers(_tmp1A3,loc);
if(_tmp1A9 == 0){void*_tmp1AD;struct Cyc_Core_Opt*_tmp1AE;struct _tuple10 _tmp1AC=
_tmp1AB;_tmp1AD=_tmp1AC.f1;_tmp1AE=_tmp1AC.f2;if(_tmp1AE != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmp1AE->v;{void*_tmp1AF=d->r;struct Cyc_Absyn_Enumdecl*
_tmp1B1;struct Cyc_Absyn_Aggrdecl*_tmp1B3;struct Cyc_Absyn_Datatypedecl*_tmp1B5;
_LLBC: {struct Cyc_Absyn_Enum_d_struct*_tmp1B0=(struct Cyc_Absyn_Enum_d_struct*)
_tmp1AF;if(_tmp1B0->tag != 8)goto _LLBE;else{_tmp1B1=_tmp1B0->f1;}}_LLBD: _tmp1B1->sc=
s;if(_tmp1A4 != 0)Cyc_Parse_err(({const char*_tmp1B6="bad attributes on enum";
_tag_dyneither(_tmp1B6,sizeof(char),23);}),loc);goto _LLBB;_LLBE: {struct Cyc_Absyn_Aggr_d_struct*
_tmp1B2=(struct Cyc_Absyn_Aggr_d_struct*)_tmp1AF;if(_tmp1B2->tag != 6)goto _LLC0;
else{_tmp1B3=_tmp1B2->f1;}}_LLBF: _tmp1B3->sc=s;_tmp1B3->attributes=_tmp1A4;goto
_LLBB;_LLC0: {struct Cyc_Absyn_Datatype_d_struct*_tmp1B4=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1AF;if(_tmp1B4->tag != 7)goto _LLC2;else{_tmp1B5=_tmp1B4->f1;}}_LLC1: _tmp1B5->sc=
s;if(_tmp1A4 != 0)Cyc_Parse_err(({const char*_tmp1B7="bad attributes on datatype";
_tag_dyneither(_tmp1B7,sizeof(char),27);}),loc);goto _LLBB;_LLC2:;_LLC3: Cyc_Parse_err(({
const char*_tmp1B8="bad declaration";_tag_dyneither(_tmp1B8,sizeof(char),16);}),
loc);{struct Cyc_List_List*_tmp1B9=0;_npop_handler(0);return _tmp1B9;};_LLBB:;}{
struct Cyc_List_List*_tmp1BB=({struct Cyc_List_List*_tmp1BA=_cycalloc(sizeof(*
_tmp1BA));_tmp1BA->hd=d;_tmp1BA->tl=0;_tmp1BA;});_npop_handler(0);return _tmp1BB;};}
else{void*_tmp1BC=_tmp1AD;struct Cyc_Absyn_AggrInfo _tmp1BE;union Cyc_Absyn_AggrInfoU
_tmp1BF;struct _tuple3 _tmp1C0;enum Cyc_Absyn_AggrKind _tmp1C1;struct _tuple1*
_tmp1C2;struct Cyc_List_List*_tmp1C3;struct Cyc_Absyn_DatatypeInfo _tmp1C5;union Cyc_Absyn_DatatypeInfoU
_tmp1C6;struct Cyc_Absyn_Datatypedecl**_tmp1C7;struct Cyc_Absyn_DatatypeInfo
_tmp1C9;union Cyc_Absyn_DatatypeInfoU _tmp1CA;struct Cyc_Absyn_UnknownDatatypeInfo
_tmp1CB;struct _tuple1*_tmp1CC;int _tmp1CD;struct Cyc_List_List*_tmp1CE;struct
_tuple1*_tmp1D0;struct Cyc_List_List*_tmp1D2;_LLC5: {struct Cyc_Absyn_AggrType_struct*
_tmp1BD=(struct Cyc_Absyn_AggrType_struct*)_tmp1BC;if(_tmp1BD->tag != 12)goto _LLC7;
else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BE.aggr_info;if((_tmp1BF.UnknownAggr).tag
!= 1)goto _LLC7;_tmp1C0=(struct _tuple3)(_tmp1BF.UnknownAggr).val;_tmp1C1=_tmp1C0.f1;
_tmp1C2=_tmp1C0.f2;_tmp1C3=_tmp1BE.targs;}}_LLC6: {struct Cyc_List_List*_tmp1D3=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp1C3);struct Cyc_Absyn_Aggrdecl*_tmp1D4=({struct Cyc_Absyn_Aggrdecl*_tmp1DA=
_cycalloc(sizeof(*_tmp1DA));_tmp1DA->kind=_tmp1C1;_tmp1DA->sc=s;_tmp1DA->name=
_tmp1C2;_tmp1DA->tvs=_tmp1D3;_tmp1DA->impl=0;_tmp1DA->attributes=0;_tmp1DA;});
if(_tmp1A4 != 0)Cyc_Parse_err(({const char*_tmp1D5="bad attributes on type declaration";
_tag_dyneither(_tmp1D5,sizeof(char),35);}),loc);{struct Cyc_List_List*_tmp1D9=({
struct Cyc_List_List*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));
_tmp1D7[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp1D8;_tmp1D8.tag=6;_tmp1D8.f1=
_tmp1D4;_tmp1D8;});_tmp1D7;}),loc);_tmp1D6->tl=0;_tmp1D6;});_npop_handler(0);
return _tmp1D9;};}_LLC7: {struct Cyc_Absyn_DatatypeType_struct*_tmp1C4=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp1BC;if(_tmp1C4->tag != 3)goto _LLC9;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C5.datatype_info;
if((_tmp1C6.KnownDatatype).tag != 2)goto _LLC9;_tmp1C7=(struct Cyc_Absyn_Datatypedecl**)(
_tmp1C6.KnownDatatype).val;}}_LLC8: if(_tmp1A4 != 0)Cyc_Parse_err(({const char*
_tmp1DB="bad attributes on datatype";_tag_dyneither(_tmp1DB,sizeof(char),27);}),
loc);{struct Cyc_List_List*_tmp1DF=({struct Cyc_List_List*_tmp1DC=_cycalloc(
sizeof(*_tmp1DC));_tmp1DC->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_struct*
_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD[0]=({struct Cyc_Absyn_Datatype_d_struct
_tmp1DE;_tmp1DE.tag=7;_tmp1DE.f1=*_tmp1C7;_tmp1DE;});_tmp1DD;}),loc);_tmp1DC->tl=
0;_tmp1DC;});_npop_handler(0);return _tmp1DF;};_LLC9: {struct Cyc_Absyn_DatatypeType_struct*
_tmp1C8=(struct Cyc_Absyn_DatatypeType_struct*)_tmp1BC;if(_tmp1C8->tag != 3)goto
_LLCB;else{_tmp1C9=_tmp1C8->f1;_tmp1CA=_tmp1C9.datatype_info;if((_tmp1CA.UnknownDatatype).tag
!= 1)goto _LLCB;_tmp1CB=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1CA.UnknownDatatype).val;
_tmp1CC=_tmp1CB.name;_tmp1CD=_tmp1CB.is_extensible;_tmp1CE=_tmp1C9.targs;}}_LLCA: {
struct Cyc_List_List*_tmp1E0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp1CE);struct Cyc_Absyn_Decl*_tmp1E1=
Cyc_Absyn_datatype_decl(s,_tmp1CC,_tmp1E0,0,_tmp1CD,loc);if(_tmp1A4 != 0)Cyc_Parse_err(({
const char*_tmp1E2="bad attributes on datatype";_tag_dyneither(_tmp1E2,sizeof(
char),27);}),loc);{struct Cyc_List_List*_tmp1E4=({struct Cyc_List_List*_tmp1E3=
_cycalloc(sizeof(*_tmp1E3));_tmp1E3->hd=_tmp1E1;_tmp1E3->tl=0;_tmp1E3;});
_npop_handler(0);return _tmp1E4;};}_LLCB: {struct Cyc_Absyn_EnumType_struct*
_tmp1CF=(struct Cyc_Absyn_EnumType_struct*)_tmp1BC;if(_tmp1CF->tag != 14)goto _LLCD;
else{_tmp1D0=_tmp1CF->f1;}}_LLCC: {struct Cyc_Absyn_Enumdecl*_tmp1E5=({struct Cyc_Absyn_Enumdecl*
_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->sc=s;_tmp1EC->name=_tmp1D0;_tmp1EC->fields=
0;_tmp1EC;});if(_tmp1A4 != 0)Cyc_Parse_err(({const char*_tmp1E6="bad attributes on enum";
_tag_dyneither(_tmp1E6,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp1EB=({
struct Cyc_List_List*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->hd=({struct Cyc_Absyn_Decl*
_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->r=(void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp1EA;_tmp1EA.tag=8;_tmp1EA.f1=_tmp1E5;_tmp1EA;});_tmp1E9;});_tmp1E8->loc=loc;
_tmp1E8;});_tmp1E7->tl=0;_tmp1E7;});_npop_handler(0);return _tmp1EB;};}_LLCD: {
struct Cyc_Absyn_AnonEnumType_struct*_tmp1D1=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1BC;if(_tmp1D1->tag != 15)goto _LLCF;else{_tmp1D2=_tmp1D1->f1;}}_LLCE: {struct
Cyc_Absyn_Enumdecl*_tmp1ED=({struct Cyc_Absyn_Enumdecl*_tmp1F4=_cycalloc(sizeof(*
_tmp1F4));_tmp1F4->sc=s;_tmp1F4->name=Cyc_Parse_gensym_enum();_tmp1F4->fields=({
struct Cyc_Core_Opt*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->v=_tmp1D2;
_tmp1F5;});_tmp1F4;});if(_tmp1A4 != 0)Cyc_Parse_err(({const char*_tmp1EE="bad attributes on enum";
_tag_dyneither(_tmp1EE,sizeof(char),23);}),loc);{struct Cyc_List_List*_tmp1F3=({
struct Cyc_List_List*_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF->hd=({struct Cyc_Absyn_Decl*
_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0->r=(void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp1F2;_tmp1F2.tag=8;_tmp1F2.f1=_tmp1ED;_tmp1F2;});_tmp1F1;});_tmp1F0->loc=loc;
_tmp1F0;});_tmp1EF->tl=0;_tmp1EF;});_npop_handler(0);return _tmp1F3;};}_LLCF:;
_LLD0: Cyc_Parse_err(({const char*_tmp1F6="missing declarator";_tag_dyneither(
_tmp1F6,sizeof(char),19);}),loc);{struct Cyc_List_List*_tmp1F7=0;_npop_handler(0);
return _tmp1F7;};_LLC4:;}}else{void*t=_tmp1AB.f1;struct Cyc_List_List*_tmp1F8=Cyc_Parse_apply_tmss(
mkrgn,_tmp1A2,t,_tmp1A9,_tmp1A4);if(istypedef){if(!exps_empty)Cyc_Parse_err(({
const char*_tmp1F9="initializer in typedef declaration";_tag_dyneither(_tmp1F9,
sizeof(char),35);}),loc);{struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,
_tmp1F8);if(_tmp1AB.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(
_tmp1AB.f2)->v;{void*_tmp1FA=d->r;struct Cyc_Absyn_Aggrdecl*_tmp1FC;struct Cyc_Absyn_Datatypedecl*
_tmp1FE;struct Cyc_Absyn_Enumdecl*_tmp200;_LLD2: {struct Cyc_Absyn_Aggr_d_struct*
_tmp1FB=(struct Cyc_Absyn_Aggr_d_struct*)_tmp1FA;if(_tmp1FB->tag != 6)goto _LLD4;
else{_tmp1FC=_tmp1FB->f1;}}_LLD3: _tmp1FC->sc=s;_tmp1FC->attributes=_tmp1A4;
_tmp1A4=0;goto _LLD1;_LLD4: {struct Cyc_Absyn_Datatype_d_struct*_tmp1FD=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp1FA;if(_tmp1FD->tag != 7)goto _LLD6;else{_tmp1FE=_tmp1FD->f1;}}_LLD5: _tmp1FE->sc=
s;goto _LLD1;_LLD6: {struct Cyc_Absyn_Enum_d_struct*_tmp1FF=(struct Cyc_Absyn_Enum_d_struct*)
_tmp1FA;if(_tmp1FF->tag != 8)goto _LLD8;else{_tmp200=_tmp1FF->f1;}}_LLD7: _tmp200->sc=
s;goto _LLD1;_LLD8:;_LLD9: Cyc_Parse_err(({const char*_tmp201="declaration within typedef is not a struct, union, or datatype";
_tag_dyneither(_tmp201,sizeof(char),63);}),loc);goto _LLD1;_LLD1:;}decls=({struct
Cyc_List_List*_tmp202=_cycalloc(sizeof(*_tmp202));_tmp202->hd=d;_tmp202->tl=
decls;_tmp202;});}{struct Cyc_List_List*_tmp203=decls;_npop_handler(0);return
_tmp203;};};}else{if(_tmp1AB.f2 != 0)Cyc_Parse_unimp(({const char*_tmp204="nested type declaration within declarator";
_tag_dyneither(_tmp204,sizeof(char),42);}),loc);{struct Cyc_List_List*decls=0;{
struct Cyc_List_List*_tmp205=_tmp1F8;for(0;_tmp205 != 0;(_tmp205=_tmp205->tl,
_tmp1AA=_tmp1AA->tl)){struct _tuple13 _tmp207;struct _tuple1*_tmp208;struct Cyc_Absyn_Tqual
_tmp209;void*_tmp20A;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20C;
struct _tuple13*_tmp206=(struct _tuple13*)_tmp205->hd;_tmp207=*_tmp206;_tmp208=
_tmp207.f1;_tmp209=_tmp207.f2;_tmp20A=_tmp207.f3;_tmp20B=_tmp207.f4;_tmp20C=
_tmp207.f5;if(_tmp20B != 0)({void*_tmp20D=0;Cyc_Tcutil_warn(loc,({const char*
_tmp20E="bad type params, ignoring";_tag_dyneither(_tmp20E,sizeof(char),26);}),
_tag_dyneither(_tmp20D,sizeof(void*),0));});if(_tmp1AA == 0)({void*_tmp20F=0;((
int(*)(struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp210="unexpected NULL in parse!";
_tag_dyneither(_tmp210,sizeof(char),26);}),_tag_dyneither(_tmp20F,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp211=(struct Cyc_Absyn_Exp*)_tmp1AA->hd;struct Cyc_Absyn_Vardecl*
_tmp212=Cyc_Absyn_new_vardecl(_tmp208,_tmp20A,_tmp211);_tmp212->tq=_tmp209;
_tmp212->sc=s;_tmp212->attributes=_tmp20C;{struct Cyc_Absyn_Decl*_tmp213=({struct
Cyc_Absyn_Decl*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_Var_d_struct
_tmp217;_tmp217.tag=0;_tmp217.f1=_tmp212;_tmp217;});_tmp216;});_tmp215->loc=loc;
_tmp215;});decls=({struct Cyc_List_List*_tmp214=_cycalloc(sizeof(*_tmp214));
_tmp214->hd=_tmp213;_tmp214->tl=decls;_tmp214;});};};}}{struct Cyc_List_List*
_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);
_npop_handler(0);return _tmp218;};};}}};};};};};_pop_region(mkrgn);}static enum 
Cyc_Absyn_Kind Cyc_Parse_id_to_kind(struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct
_dyneither_ptr)s)== 2)switch(*((const char*)_check_dyneither_subscript(s,sizeof(
char),0))){case 'A': _LLDA: return Cyc_Absyn_AnyKind;case 'M': _LLDB: return Cyc_Absyn_MemKind;
case 'B': _LLDC: return Cyc_Absyn_BoxKind;case 'R': _LLDD: return Cyc_Absyn_RgnKind;case
'U': _LLDE: if(*((const char*)_check_dyneither_subscript(s,sizeof(char),1))== 'R')
return Cyc_Absyn_UniqueRgnKind;else{break;}case 'T': _LLDF: if(*((const char*)
_check_dyneither_subscript(s,sizeof(char),1))== 'R')return Cyc_Absyn_TopRgnKind;
else{break;}case 'E': _LLE0: return Cyc_Absyn_EffKind;case 'I': _LLE1: return Cyc_Absyn_IntKind;
default: _LLE2: break;}Cyc_Parse_err((struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp21C;_tmp21C.tag=1;_tmp21C.f1=(unsigned long)((int)Cyc_strlen((struct
_dyneither_ptr)s));({struct Cyc_String_pa_struct _tmp21B;_tmp21B.tag=0;_tmp21B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp219[2]={& _tmp21B,&
_tmp21C};Cyc_aprintf(({const char*_tmp21A="bad kind: %s; strlen=%d";
_tag_dyneither(_tmp21A,sizeof(char),24);}),_tag_dyneither(_tmp219,sizeof(void*),
2));});});}),loc);return Cyc_Absyn_BoxKind;}static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(
struct Cyc_Absyn_Pat*p){void*_tmp21D=p->r;struct _tuple1*_tmp21F;struct Cyc_Absyn_Vardecl*
_tmp221;struct Cyc_Absyn_Pat*_tmp222;struct Cyc_Absyn_Pat _tmp223;void*_tmp224;
struct Cyc_Absyn_Pat*_tmp227;enum Cyc_Absyn_Sign _tmp22A;int _tmp22B;char _tmp22D;
struct _dyneither_ptr _tmp22F;struct _tuple1*_tmp231;struct Cyc_List_List*_tmp232;
int _tmp233;struct Cyc_Absyn_Exp*_tmp235;_LLE5: {struct Cyc_Absyn_UnknownId_p_struct*
_tmp21E=(struct Cyc_Absyn_UnknownId_p_struct*)_tmp21D;if(_tmp21E->tag != 14)goto
_LLE7;else{_tmp21F=_tmp21E->f1;}}_LLE6: return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp237;_tmp237.tag=2;_tmp237.f1=_tmp21F;
_tmp237;});_tmp236;}),p->loc);_LLE7: {struct Cyc_Absyn_Reference_p_struct*_tmp220=(
struct Cyc_Absyn_Reference_p_struct*)_tmp21D;if(_tmp220->tag != 2)goto _LLE9;else{
_tmp221=_tmp220->f1;_tmp222=_tmp220->f2;_tmp223=*_tmp222;_tmp224=_tmp223.r;{
struct Cyc_Absyn_Wild_p_struct*_tmp225=(struct Cyc_Absyn_Wild_p_struct*)_tmp224;
if(_tmp225->tag != 0)goto _LLE9;};}}_LLE8: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp238=_cycalloc(sizeof(*_tmp238));
_tmp238[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp239;_tmp239.tag=2;_tmp239.f1=
_tmp221->name;_tmp239;});_tmp238;}),p->loc),p->loc);_LLE9: {struct Cyc_Absyn_Pointer_p_struct*
_tmp226=(struct Cyc_Absyn_Pointer_p_struct*)_tmp21D;if(_tmp226->tag != 5)goto _LLEB;
else{_tmp227=_tmp226->f1;}}_LLEA: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(
_tmp227),p->loc);_LLEB: {struct Cyc_Absyn_Null_p_struct*_tmp228=(struct Cyc_Absyn_Null_p_struct*)
_tmp21D;if(_tmp228->tag != 8)goto _LLED;}_LLEC: return Cyc_Absyn_null_exp(p->loc);
_LLED: {struct Cyc_Absyn_Int_p_struct*_tmp229=(struct Cyc_Absyn_Int_p_struct*)
_tmp21D;if(_tmp229->tag != 9)goto _LLEF;else{_tmp22A=_tmp229->f1;_tmp22B=_tmp229->f2;}}
_LLEE: return Cyc_Absyn_int_exp(_tmp22A,_tmp22B,p->loc);_LLEF: {struct Cyc_Absyn_Char_p_struct*
_tmp22C=(struct Cyc_Absyn_Char_p_struct*)_tmp21D;if(_tmp22C->tag != 10)goto _LLF1;
else{_tmp22D=_tmp22C->f1;}}_LLF0: return Cyc_Absyn_char_exp(_tmp22D,p->loc);_LLF1: {
struct Cyc_Absyn_Float_p_struct*_tmp22E=(struct Cyc_Absyn_Float_p_struct*)_tmp21D;
if(_tmp22E->tag != 11)goto _LLF3;else{_tmp22F=_tmp22E->f1;}}_LLF2: return Cyc_Absyn_float_exp(
_tmp22F,p->loc);_LLF3: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp230=(struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp21D;if(_tmp230->tag != 15)goto _LLF5;else{_tmp231=_tmp230->f1;_tmp232=_tmp230->f2;
_tmp233=_tmp230->f3;if(_tmp233 != 0)goto _LLF5;}}_LLF4: {struct Cyc_Absyn_Exp*e1=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp23A=_cycalloc(
sizeof(*_tmp23A));_tmp23A[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp23B;_tmp23B.tag=
2;_tmp23B.f1=_tmp231;_tmp23B;});_tmp23A;}),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp232);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLF5: {struct Cyc_Absyn_Exp_p_struct*_tmp234=(struct Cyc_Absyn_Exp_p_struct*)
_tmp21D;if(_tmp234->tag != 16)goto _LLF7;else{_tmp235=_tmp234->f1;}}_LLF6: return
_tmp235;_LLF7:;_LLF8: Cyc_Parse_err(({const char*_tmp23C="cannot mix patterns and expressions in case";
_tag_dyneither(_tmp23C,sizeof(char),44);}),p->loc);return Cyc_Absyn_null_exp(p->loc);
_LLE4:;}union Cyc_YYSTYPE;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple6 val;};
struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{
int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;
struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*
val;};struct _tuple18{struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;
union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple18*
val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple19{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple19*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple20*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Parse_Declaration_spec*val;};
struct _tuple21{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Parse_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator*val;};
struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum 
Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*
val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple22{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple23{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple23*val;};
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
YY54;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp23E[14]="$(sign_t,int)";
static struct Cyc_Core_Failure_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp23E,
_tmp23E,_tmp23E + 14}};struct _tuple6 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){struct
_tuple6 yyzzz;{union Cyc_YYSTYPE _tmp23F=yy1;struct _tuple6 _tmp240;_LLFA: if((_tmp23F.Int_tok).tag
!= 1)goto _LLFC;_tmp240=(struct _tuple6)(_tmp23F.Int_tok).val;_LLFB: yyzzz=_tmp240;
goto _LLF9;_LLFC:;_LLFD:(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLF9:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple6 yy1){return({union Cyc_YYSTYPE
_tmp241;(_tmp241.Int_tok).val=yy1;(_tmp241.Int_tok).tag=1;_tmp241;});}static char
_tmp243[5]="char";static struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{
_tmp243,_tmp243,_tmp243 + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){char
yyzzz;{union Cyc_YYSTYPE _tmp244=yy1;char _tmp245;_LLFF: if((_tmp244.Char_tok).tag != 
2)goto _LL101;_tmp245=(char)(_tmp244.Char_tok).val;_LL100: yyzzz=_tmp245;goto _LLFE;
_LL101:;_LL102:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLFE:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp246;(_tmp246.Char_tok).val=
yy1;(_tmp246.Char_tok).tag=2;_tmp246;});}static char _tmp248[13]="string_t<`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{
_tmp248,_tmp248,_tmp248 + 13}};struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE
yy1){struct _dyneither_ptr yyzzz;{union Cyc_YYSTYPE _tmp249=yy1;struct _dyneither_ptr
_tmp24A;_LL104: if((_tmp249.String_tok).tag != 3)goto _LL106;_tmp24A=(struct
_dyneither_ptr)(_tmp249.String_tok).val;_LL105: yyzzz=_tmp24A;goto _LL103;_LL106:;
_LL107:(int)_throw((void*)& Cyc_yyfail_String_tok);_LL103:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp24B;(_tmp24B.String_tok).val=
yy1;(_tmp24B.String_tok).tag=3;_tmp24B;});}static char _tmp24D[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp24D,
_tmp24D,_tmp24D + 56}};struct _tuple18*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){struct
_tuple18*yyzzz;{union Cyc_YYSTYPE _tmp24E=yy1;struct _tuple18*_tmp24F;_LL109: if((
_tmp24E.YY1).tag != 6)goto _LL10B;_tmp24F=(struct _tuple18*)(_tmp24E.YY1).val;
_LL10A: yyzzz=_tmp24F;goto _LL108;_LL10B:;_LL10C:(int)_throw((void*)& Cyc_yyfail_YY1);
_LL108:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY1(struct _tuple18*yy1){return({union
Cyc_YYSTYPE _tmp250;(_tmp250.YY1).val=yy1;(_tmp250.YY1).tag=6;_tmp250;});}static
char _tmp252[19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={
Cyc_Core_Failure,{_tmp252,_tmp252,_tmp252 + 19}};union Cyc_Absyn_Constraint*Cyc_yyget_YY2(
union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE _tmp253=
yy1;union Cyc_Absyn_Constraint*_tmp254;_LL10E: if((_tmp253.YY2).tag != 7)goto _LL110;
_tmp254=(union Cyc_Absyn_Constraint*)(_tmp253.YY2).val;_LL10F: yyzzz=_tmp254;goto
_LL10D;_LL110:;_LL111:(int)_throw((void*)& Cyc_yyfail_YY2);_LL10D:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE
_tmp255;(_tmp255.YY2).val=yy1;(_tmp255.YY2).tag=7;_tmp255;});}static char _tmp257[
6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{
_tmp257,_tmp257,_tmp257 + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE _tmp258=yy1;struct Cyc_Absyn_Exp*
_tmp259;_LL113: if((_tmp258.YY3).tag != 8)goto _LL115;_tmp259=(struct Cyc_Absyn_Exp*)(
_tmp258.YY3).val;_LL114: yyzzz=_tmp259;goto _LL112;_LL115:;_LL116:(int)_throw((
void*)& Cyc_yyfail_YY3);_LL112:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY3(struct Cyc_Absyn_Exp*
yy1){return({union Cyc_YYSTYPE _tmp25A;(_tmp25A.YY3).val=yy1;(_tmp25A.YY3).tag=8;
_tmp25A;});}static char _tmp25C[10]="exp_opt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp25C,_tmp25C,_tmp25C + 10}};struct Cyc_Absyn_Exp*
Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE
_tmp25D=yy1;struct Cyc_Absyn_Exp*_tmp25E;_LL118: if((_tmp25D.YY4).tag != 9)goto
_LL11A;_tmp25E=(struct Cyc_Absyn_Exp*)(_tmp25D.YY4).val;_LL119: yyzzz=_tmp25E;goto
_LL117;_LL11A:;_LL11B:(int)_throw((void*)& Cyc_yyfail_YY4);_LL117:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp25F;(
_tmp25F.YY4).val=yy1;(_tmp25F.YY4).tag=9;_tmp25F;});}static char _tmp261[17]="list_t<exp_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp261,
_tmp261,_tmp261 + 17}};struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp262=yy1;struct Cyc_List_List*
_tmp263;_LL11D: if((_tmp262.YY5).tag != 10)goto _LL11F;_tmp263=(struct Cyc_List_List*)(
_tmp262.YY5).val;_LL11E: yyzzz=_tmp263;goto _LL11C;_LL11F:;_LL120:(int)_throw((
void*)& Cyc_yyfail_YY5);_LL11C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp264;(_tmp264.YY5).val=yy1;(_tmp264.YY5).tag=10;
_tmp264;});}static char _tmp266[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp266,
_tmp266,_tmp266 + 47}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp267=yy1;struct Cyc_List_List*
_tmp268;_LL122: if((_tmp267.YY6).tag != 11)goto _LL124;_tmp268=(struct Cyc_List_List*)(
_tmp267.YY6).val;_LL123: yyzzz=_tmp268;goto _LL121;_LL124:;_LL125:(int)_throw((
void*)& Cyc_yyfail_YY6);_LL121:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp269;(_tmp269.YY6).val=yy1;(_tmp269.YY6).tag=11;
_tmp269;});}static char _tmp26B[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp26B,_tmp26B,_tmp26B + 9}};enum Cyc_Absyn_Primop
Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){enum Cyc_Absyn_Primop yyzzz;{union Cyc_YYSTYPE
_tmp26C=yy1;enum Cyc_Absyn_Primop _tmp26D;_LL127: if((_tmp26C.YY7).tag != 12)goto
_LL129;_tmp26D=(enum Cyc_Absyn_Primop)(_tmp26C.YY7).val;_LL128: yyzzz=_tmp26D;
goto _LL126;_LL129:;_LL12A:(int)_throw((void*)& Cyc_yyfail_YY7);_LL126:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE
_tmp26E;(_tmp26E.YY7).val=yy1;(_tmp26E.YY7).tag=12;_tmp26E;});}static char _tmp270[
19]="opt_t<primop_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{
_tmp270,_tmp270,_tmp270 + 19}};struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE
yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp271=yy1;struct Cyc_Core_Opt*
_tmp272;_LL12C: if((_tmp271.YY8).tag != 13)goto _LL12E;_tmp272=(struct Cyc_Core_Opt*)(
_tmp271.YY8).val;_LL12D: yyzzz=_tmp272;goto _LL12B;_LL12E:;_LL12F:(int)_throw((
void*)& Cyc_yyfail_YY8);_LL12B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*
yy1){return({union Cyc_YYSTYPE _tmp273;(_tmp273.YY8).val=yy1;(_tmp273.YY8).tag=13;
_tmp273;});}static char _tmp275[7]="qvar_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{_tmp275,_tmp275,_tmp275 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(
union Cyc_YYSTYPE yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE _tmp276=yy1;struct
_tuple1*_tmp277;_LL131: if((_tmp276.QualId_tok).tag != 5)goto _LL133;_tmp277=(
struct _tuple1*)(_tmp276.QualId_tok).val;_LL132: yyzzz=_tmp277;goto _LL130;_LL133:;
_LL134:(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL130:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_QualId_tok(struct _tuple1*yy1){return({union Cyc_YYSTYPE _tmp278;(_tmp278.QualId_tok).val=
yy1;(_tmp278.QualId_tok).tag=5;_tmp278;});}static char _tmp27A[7]="stmt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp27A,_tmp27A,
_tmp27A + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Stmt*
yyzzz;{union Cyc_YYSTYPE _tmp27B=yy1;struct Cyc_Absyn_Stmt*_tmp27C;_LL136: if((
_tmp27B.YY9).tag != 14)goto _LL138;_tmp27C=(struct Cyc_Absyn_Stmt*)(_tmp27B.YY9).val;
_LL137: yyzzz=_tmp27C;goto _LL135;_LL138:;_LL139:(int)_throw((void*)& Cyc_yyfail_YY9);
_LL135:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Stmt*yy1){return({
union Cyc_YYSTYPE _tmp27D;(_tmp27D.YY9).val=yy1;(_tmp27D.YY9).tag=14;_tmp27D;});}
static char _tmp27F[27]="list_t<switch_clause_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp27F,_tmp27F,_tmp27F + 27}};struct Cyc_List_List*
Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp280=yy1;struct Cyc_List_List*_tmp281;_LL13B: if((_tmp280.YY10).tag != 15)goto
_LL13D;_tmp281=(struct Cyc_List_List*)(_tmp280.YY10).val;_LL13C: yyzzz=_tmp281;
goto _LL13A;_LL13D:;_LL13E:(int)_throw((void*)& Cyc_yyfail_YY10);_LL13A:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY10(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp282;(_tmp282.YY10).val=yy1;(_tmp282.YY10).tag=15;_tmp282;});}static char
_tmp284[6]="pat_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{
_tmp284,_tmp284,_tmp284 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE _tmp285=yy1;struct Cyc_Absyn_Pat*
_tmp286;_LL140: if((_tmp285.YY11).tag != 16)goto _LL142;_tmp286=(struct Cyc_Absyn_Pat*)(
_tmp285.YY11).val;_LL141: yyzzz=_tmp286;goto _LL13F;_LL142:;_LL143:(int)_throw((
void*)& Cyc_yyfail_YY11);_LL13F:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY11(struct Cyc_Absyn_Pat*
yy1){return({union Cyc_YYSTYPE _tmp287;(_tmp287.YY11).val=yy1;(_tmp287.YY11).tag=
16;_tmp287;});}static char _tmp289[28]="$(list_t<pat_t,`H>,bool)@`H";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp289,_tmp289,
_tmp289 + 28}};struct _tuple19*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){struct _tuple19*
yyzzz;{union Cyc_YYSTYPE _tmp28A=yy1;struct _tuple19*_tmp28B;_LL145: if((_tmp28A.YY12).tag
!= 17)goto _LL147;_tmp28B=(struct _tuple19*)(_tmp28A.YY12).val;_LL146: yyzzz=
_tmp28B;goto _LL144;_LL147:;_LL148:(int)_throw((void*)& Cyc_yyfail_YY12);_LL144:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY12(struct _tuple19*yy1){return({union Cyc_YYSTYPE
_tmp28C;(_tmp28C.YY12).val=yy1;(_tmp28C.YY12).tag=17;_tmp28C;});}static char
_tmp28E[17]="list_t<pat_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY13={
Cyc_Core_Failure,{_tmp28E,_tmp28E,_tmp28E + 17}};struct Cyc_List_List*Cyc_yyget_YY13(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp28F=yy1;
struct Cyc_List_List*_tmp290;_LL14A: if((_tmp28F.YY13).tag != 18)goto _LL14C;_tmp290=(
struct Cyc_List_List*)(_tmp28F.YY13).val;_LL14B: yyzzz=_tmp290;goto _LL149;_LL14C:;
_LL14D:(int)_throw((void*)& Cyc_yyfail_YY13);_LL149:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp291;(_tmp291.YY13).val=
yy1;(_tmp291.YY13).tag=18;_tmp291;});}static char _tmp293[36]="$(list_t<designator_t,`H>,pat_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp293,
_tmp293,_tmp293 + 36}};struct _tuple20*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){struct
_tuple20*yyzzz;{union Cyc_YYSTYPE _tmp294=yy1;struct _tuple20*_tmp295;_LL14F: if((
_tmp294.YY14).tag != 19)goto _LL151;_tmp295=(struct _tuple20*)(_tmp294.YY14).val;
_LL150: yyzzz=_tmp295;goto _LL14E;_LL151:;_LL152:(int)_throw((void*)& Cyc_yyfail_YY14);
_LL14E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY14(struct _tuple20*yy1){return({union
Cyc_YYSTYPE _tmp296;(_tmp296.YY14).val=yy1;(_tmp296.YY14).tag=19;_tmp296;});}
static char _tmp298[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp298,_tmp298,
_tmp298 + 47}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp299=yy1;struct Cyc_List_List*_tmp29A;_LL154: if((
_tmp299.YY15).tag != 20)goto _LL156;_tmp29A=(struct Cyc_List_List*)(_tmp299.YY15).val;
_LL155: yyzzz=_tmp29A;goto _LL153;_LL156:;_LL157:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL153:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY15(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp29B;(_tmp29B.YY15).val=yy1;(_tmp29B.YY15).tag=20;_tmp29B;});}
static char _tmp29D[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp29D,
_tmp29D,_tmp29D + 58}};struct _tuple19*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){struct
_tuple19*yyzzz;{union Cyc_YYSTYPE _tmp29E=yy1;struct _tuple19*_tmp29F;_LL159: if((
_tmp29E.YY16).tag != 21)goto _LL15B;_tmp29F=(struct _tuple19*)(_tmp29E.YY16).val;
_LL15A: yyzzz=_tmp29F;goto _LL158;_LL15B:;_LL15C:(int)_throw((void*)& Cyc_yyfail_YY16);
_LL158:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY16(struct _tuple19*yy1){return({union
Cyc_YYSTYPE _tmp2A0;(_tmp2A0.YY16).val=yy1;(_tmp2A0.YY16).tag=21;_tmp2A0;});}
static char _tmp2A2[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp2A2,_tmp2A2,_tmp2A2 + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE _tmp2A3=yy1;
struct Cyc_Absyn_Fndecl*_tmp2A4;_LL15E: if((_tmp2A3.YY17).tag != 22)goto _LL160;
_tmp2A4=(struct Cyc_Absyn_Fndecl*)(_tmp2A3.YY17).val;_LL15F: yyzzz=_tmp2A4;goto
_LL15D;_LL160:;_LL161:(int)_throw((void*)& Cyc_yyfail_YY17);_LL15D:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE
_tmp2A5;(_tmp2A5.YY17).val=yy1;(_tmp2A5.YY17).tag=22;_tmp2A5;});}static char
_tmp2A7[18]="list_t<decl_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp2A7,_tmp2A7,_tmp2A7 + 18}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2A8=yy1;
struct Cyc_List_List*_tmp2A9;_LL163: if((_tmp2A8.YY18).tag != 23)goto _LL165;_tmp2A9=(
struct Cyc_List_List*)(_tmp2A8.YY18).val;_LL164: yyzzz=_tmp2A9;goto _LL162;_LL165:;
_LL166:(int)_throw((void*)& Cyc_yyfail_YY18);_LL162:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY18(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp2AA;(_tmp2AA.YY18).val=
yy1;(_tmp2AA.YY18).tag=23;_tmp2AA;});}static char _tmp2AC[12]="decl_spec_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp2AC,_tmp2AC,
_tmp2AC + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE _tmp2AD=yy1;struct
Cyc_Parse_Declaration_spec*_tmp2AE;_LL168: if((_tmp2AD.YY19).tag != 24)goto _LL16A;
_tmp2AE=(struct Cyc_Parse_Declaration_spec*)(_tmp2AD.YY19).val;_LL169: yyzzz=
_tmp2AE;goto _LL167;_LL16A:;_LL16B:(int)_throw((void*)& Cyc_yyfail_YY19);_LL167:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY19(struct Cyc_Parse_Declaration_spec*yy1){
return({union Cyc_YYSTYPE _tmp2AF;(_tmp2AF.YY19).val=yy1;(_tmp2AF.YY19).tag=24;
_tmp2AF;});}static char _tmp2B1[29]="$(declarator_t,exp_opt_t)@`H";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp2B1,_tmp2B1,_tmp2B1 + 29}};struct _tuple21*
Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){struct _tuple21*yyzzz;{union Cyc_YYSTYPE
_tmp2B2=yy1;struct _tuple21*_tmp2B3;_LL16D: if((_tmp2B2.YY20).tag != 25)goto _LL16F;
_tmp2B3=(struct _tuple21*)(_tmp2B2.YY20).val;_LL16E: yyzzz=_tmp2B3;goto _LL16C;
_LL16F:;_LL170:(int)_throw((void*)& Cyc_yyfail_YY20);_LL16C:;}return yyzzz;}union
Cyc_YYSTYPE Cyc_YY20(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp2B4;(_tmp2B4.YY20).val=
yy1;(_tmp2B4.YY20).tag=25;_tmp2B4;});}static char _tmp2B6[40]="list_t<$(declarator_t,exp_opt_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp2B6,
_tmp2B6,_tmp2B6 + 40}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp2B7=yy1;struct Cyc_List_List*
_tmp2B8;_LL172: if((_tmp2B7.YY21).tag != 26)goto _LL174;_tmp2B8=(struct Cyc_List_List*)(
_tmp2B7.YY21).val;_LL173: yyzzz=_tmp2B8;goto _LL171;_LL174:;_LL175:(int)_throw((
void*)& Cyc_yyfail_YY21);_LL171:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY21(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp2B9;(_tmp2B9.YY21).val=yy1;(_tmp2B9.YY21).tag=
26;_tmp2B9;});}static char _tmp2BB[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp2BB,_tmp2BB,_tmp2BB + 16}};enum Cyc_Parse_Storage_class
Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){enum Cyc_Parse_Storage_class yyzzz;{union Cyc_YYSTYPE
_tmp2BC=yy1;enum Cyc_Parse_Storage_class _tmp2BD;_LL177: if((_tmp2BC.YY22).tag != 
27)goto _LL179;_tmp2BD=(enum Cyc_Parse_Storage_class)(_tmp2BC.YY22).val;_LL178:
yyzzz=_tmp2BD;goto _LL176;_LL179:;_LL17A:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL176:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Parse_Storage_class yy1){
return({union Cyc_YYSTYPE _tmp2BE;(_tmp2BE.YY22).val=yy1;(_tmp2BE.YY22).tag=27;
_tmp2BE;});}static char _tmp2C0[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp2C0,_tmp2C0,_tmp2C0 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp2C1=yy1;void*_tmp2C2;_LL17C:
if((_tmp2C1.YY23).tag != 28)goto _LL17E;_tmp2C2=(void*)(_tmp2C1.YY23).val;_LL17D:
yyzzz=_tmp2C2;goto _LL17B;_LL17E:;_LL17F:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL17B:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY23(void*yy1){return({union Cyc_YYSTYPE
_tmp2C3;(_tmp2C3.YY23).val=yy1;(_tmp2C3.YY23).tag=28;_tmp2C3;});}static char
_tmp2C5[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={
Cyc_Core_Failure,{_tmp2C5,_tmp2C5,_tmp2C5 + 12}};enum Cyc_Absyn_AggrKind Cyc_yyget_YY24(
union Cyc_YYSTYPE yy1){enum Cyc_Absyn_AggrKind yyzzz;{union Cyc_YYSTYPE _tmp2C6=yy1;
enum Cyc_Absyn_AggrKind _tmp2C7;_LL181: if((_tmp2C6.YY24).tag != 29)goto _LL183;
_tmp2C7=(enum Cyc_Absyn_AggrKind)(_tmp2C6.YY24).val;_LL182: yyzzz=_tmp2C7;goto
_LL180;_LL183:;_LL184:(int)_throw((void*)& Cyc_yyfail_YY24);_LL180:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY24(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE
_tmp2C8;(_tmp2C8.YY24).val=yy1;(_tmp2C8.YY24).tag=29;_tmp2C8;});}static char
_tmp2CA[8]="tqual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{
_tmp2CA,_tmp2CA,_tmp2CA + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE _tmp2CB=yy1;struct Cyc_Absyn_Tqual
_tmp2CC;_LL186: if((_tmp2CB.YY25).tag != 30)goto _LL188;_tmp2CC=(struct Cyc_Absyn_Tqual)(
_tmp2CB.YY25).val;_LL187: yyzzz=_tmp2CC;goto _LL185;_LL188:;_LL189:(int)_throw((
void*)& Cyc_yyfail_YY25);_LL185:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual
yy1){return({union Cyc_YYSTYPE _tmp2CD;(_tmp2CD.YY25).val=yy1;(_tmp2CD.YY25).tag=
30;_tmp2CD;});}static char _tmp2CF[23]="list_t<aggrfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp2CF,_tmp2CF,_tmp2CF + 23}};struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2D0=yy1;struct Cyc_List_List*_tmp2D1;_LL18B: if((_tmp2D0.YY26).tag != 31)goto
_LL18D;_tmp2D1=(struct Cyc_List_List*)(_tmp2D0.YY26).val;_LL18C: yyzzz=_tmp2D1;
goto _LL18A;_LL18D:;_LL18E:(int)_throw((void*)& Cyc_yyfail_YY26);_LL18A:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp2D2;(_tmp2D2.YY26).val=yy1;(_tmp2D2.YY26).tag=31;_tmp2D2;});}static char
_tmp2D4[34]="list_t<list_t<aggrfield_t,`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp2D4,_tmp2D4,_tmp2D4 + 34}};struct Cyc_List_List*
Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2D5=yy1;struct Cyc_List_List*_tmp2D6;_LL190: if((_tmp2D5.YY27).tag != 32)goto
_LL192;_tmp2D6=(struct Cyc_List_List*)(_tmp2D5.YY27).val;_LL191: yyzzz=_tmp2D6;
goto _LL18F;_LL192:;_LL193:(int)_throw((void*)& Cyc_yyfail_YY27);_LL18F:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp2D7;(_tmp2D7.YY27).val=yy1;(_tmp2D7.YY27).tag=32;_tmp2D7;});}static char
_tmp2D9[31]="list_t<type_modifier_t<`H>,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp2D9,_tmp2D9,_tmp2D9 + 31}};struct Cyc_List_List*
Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2DA=yy1;struct Cyc_List_List*_tmp2DB;_LL195: if((_tmp2DA.YY28).tag != 33)goto
_LL197;_tmp2DB=(struct Cyc_List_List*)(_tmp2DA.YY28).val;_LL196: yyzzz=_tmp2DB;
goto _LL194;_LL197:;_LL198:(int)_throw((void*)& Cyc_yyfail_YY28);_LL194:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp2DC;(_tmp2DC.YY28).val=yy1;(_tmp2DC.YY28).tag=33;_tmp2DC;});}static char
_tmp2DE[13]="declarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={
Cyc_Core_Failure,{_tmp2DE,_tmp2DE,_tmp2DE + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE _tmp2DF=
yy1;struct Cyc_Parse_Declarator*_tmp2E0;_LL19A: if((_tmp2DF.YY29).tag != 34)goto
_LL19C;_tmp2E0=(struct Cyc_Parse_Declarator*)(_tmp2DF.YY29).val;_LL19B: yyzzz=
_tmp2E0;goto _LL199;_LL19C:;_LL19D:(int)_throw((void*)& Cyc_yyfail_YY29);_LL199:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator*yy1){return({
union Cyc_YYSTYPE _tmp2E1;(_tmp2E1.YY29).val=yy1;(_tmp2E1.YY29).tag=34;_tmp2E1;});}
static char _tmp2E3[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp2E3,_tmp2E3,_tmp2E3 + 21}};struct Cyc_Parse_Abstractdeclarator*
Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{
union Cyc_YYSTYPE _tmp2E4=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp2E5;_LL19F:
if((_tmp2E4.YY30).tag != 35)goto _LL1A1;_tmp2E5=(struct Cyc_Parse_Abstractdeclarator*)(
_tmp2E4.YY30).val;_LL1A0: yyzzz=_tmp2E5;goto _LL19E;_LL1A1:;_LL1A2:(int)_throw((
void*)& Cyc_yyfail_YY30);_LL19E:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator*
yy1){return({union Cyc_YYSTYPE _tmp2E6;(_tmp2E6.YY30).val=yy1;(_tmp2E6.YY30).tag=
35;_tmp2E6;});}static char _tmp2E8[5]="bool";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp2E8,_tmp2E8,_tmp2E8 + 5}};int Cyc_yyget_YY31(
union Cyc_YYSTYPE yy1){int yyzzz;{union Cyc_YYSTYPE _tmp2E9=yy1;int _tmp2EA;_LL1A4: if((
_tmp2E9.YY31).tag != 36)goto _LL1A6;_tmp2EA=(int)(_tmp2E9.YY31).val;_LL1A5: yyzzz=
_tmp2EA;goto _LL1A3;_LL1A6:;_LL1A7:(int)_throw((void*)& Cyc_yyfail_YY31);_LL1A3:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp2EB;(
_tmp2EB.YY31).val=yy1;(_tmp2EB.YY31).tag=36;_tmp2EB;});}static char _tmp2ED[8]="scope_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp2ED,
_tmp2ED,_tmp2ED + 8}};enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;{union Cyc_YYSTYPE _tmp2EE=yy1;enum Cyc_Absyn_Scope
_tmp2EF;_LL1A9: if((_tmp2EE.YY32).tag != 37)goto _LL1AB;_tmp2EF=(enum Cyc_Absyn_Scope)(
_tmp2EE.YY32).val;_LL1AA: yyzzz=_tmp2EF;goto _LL1A8;_LL1AB:;_LL1AC:(int)_throw((
void*)& Cyc_yyfail_YY32);_LL1A8:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope
yy1){return({union Cyc_YYSTYPE _tmp2F0;(_tmp2F0.YY32).val=yy1;(_tmp2F0.YY32).tag=
37;_tmp2F0;});}static char _tmp2F2[16]="datatypefield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp2F2,_tmp2F2,_tmp2F2 + 16}};struct Cyc_Absyn_Datatypefield*
Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Datatypefield*yyzzz;{union
Cyc_YYSTYPE _tmp2F3=yy1;struct Cyc_Absyn_Datatypefield*_tmp2F4;_LL1AE: if((_tmp2F3.YY33).tag
!= 38)goto _LL1B0;_tmp2F4=(struct Cyc_Absyn_Datatypefield*)(_tmp2F3.YY33).val;
_LL1AF: yyzzz=_tmp2F4;goto _LL1AD;_LL1B0:;_LL1B1:(int)_throw((void*)& Cyc_yyfail_YY33);
_LL1AD:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){
return({union Cyc_YYSTYPE _tmp2F5;(_tmp2F5.YY33).val=yy1;(_tmp2F5.YY33).tag=38;
_tmp2F5;});}static char _tmp2F7[27]="list_t<datatypefield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2F7,_tmp2F7,_tmp2F7 + 27}};struct Cyc_List_List*
Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp2F8=yy1;struct Cyc_List_List*_tmp2F9;_LL1B3: if((_tmp2F8.YY34).tag != 39)goto
_LL1B5;_tmp2F9=(struct Cyc_List_List*)(_tmp2F8.YY34).val;_LL1B4: yyzzz=_tmp2F9;
goto _LL1B2;_LL1B5:;_LL1B6:(int)_throw((void*)& Cyc_yyfail_YY34);_LL1B2:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp2FA;(_tmp2FA.YY34).val=yy1;(_tmp2FA.YY34).tag=39;_tmp2FA;});}static char
_tmp2FC[55]="$(tqual_t,list_t<type_specifier_t,`H>,attributes_t)@`H";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2FC,_tmp2FC,
_tmp2FC + 55}};struct _tuple22*Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){struct _tuple22*
yyzzz;{union Cyc_YYSTYPE _tmp2FD=yy1;struct _tuple22*_tmp2FE;_LL1B8: if((_tmp2FD.YY35).tag
!= 40)goto _LL1BA;_tmp2FE=(struct _tuple22*)(_tmp2FD.YY35).val;_LL1B9: yyzzz=
_tmp2FE;goto _LL1B7;_LL1BA:;_LL1BB:(int)_throw((void*)& Cyc_yyfail_YY35);_LL1B7:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY35(struct _tuple22*yy1){return({union Cyc_YYSTYPE
_tmp2FF;(_tmp2FF.YY35).val=yy1;(_tmp2FF.YY35).tag=40;_tmp2FF;});}static char
_tmp301[17]="list_t<var_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp301,_tmp301,_tmp301 + 17}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp302=yy1;
struct Cyc_List_List*_tmp303;_LL1BD: if((_tmp302.YY36).tag != 41)goto _LL1BF;_tmp303=(
struct Cyc_List_List*)(_tmp302.YY36).val;_LL1BE: yyzzz=_tmp303;goto _LL1BC;_LL1BF:;
_LL1C0:(int)_throw((void*)& Cyc_yyfail_YY36);_LL1BC:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp304;(_tmp304.YY36).val=
yy1;(_tmp304.YY36).tag=41;_tmp304;});}static char _tmp306[37]="$(opt_t<var_t,`H>,tqual_t,type_t)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp306,
_tmp306,_tmp306 + 37}};struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){struct
_tuple8*yyzzz;{union Cyc_YYSTYPE _tmp307=yy1;struct _tuple8*_tmp308;_LL1C2: if((
_tmp307.YY37).tag != 42)goto _LL1C4;_tmp308=(struct _tuple8*)(_tmp307.YY37).val;
_LL1C3: yyzzz=_tmp308;goto _LL1C1;_LL1C4:;_LL1C5:(int)_throw((void*)& Cyc_yyfail_YY37);
_LL1C1:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union
Cyc_YYSTYPE _tmp309;(_tmp309.YY37).val=yy1;(_tmp309.YY37).tag=42;_tmp309;});}
static char _tmp30B[48]="list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp30B,_tmp30B,
_tmp30B + 48}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp30C=yy1;struct Cyc_List_List*_tmp30D;_LL1C7: if((
_tmp30C.YY38).tag != 43)goto _LL1C9;_tmp30D=(struct Cyc_List_List*)(_tmp30C.YY38).val;
_LL1C8: yyzzz=_tmp30D;goto _LL1C6;_LL1C9:;_LL1CA:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL1C6:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp30E;(_tmp30E.YY38).val=yy1;(_tmp30E.YY38).tag=43;_tmp30E;});}
static char _tmp310[127]="$(list_t<$(opt_t<var_t,`H>,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,opt_t<type_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp310,
_tmp310,_tmp310 + 127}};struct _tuple23*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){struct
_tuple23*yyzzz;{union Cyc_YYSTYPE _tmp311=yy1;struct _tuple23*_tmp312;_LL1CC: if((
_tmp311.YY39).tag != 44)goto _LL1CE;_tmp312=(struct _tuple23*)(_tmp311.YY39).val;
_LL1CD: yyzzz=_tmp312;goto _LL1CB;_LL1CE:;_LL1CF:(int)_throw((void*)& Cyc_yyfail_YY39);
_LL1CB:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY39(struct _tuple23*yy1){return({union
Cyc_YYSTYPE _tmp313;(_tmp313.YY39).val=yy1;(_tmp313.YY39).tag=44;_tmp313;});}
static char _tmp315[18]="list_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp315,_tmp315,_tmp315 + 18}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp316=yy1;
struct Cyc_List_List*_tmp317;_LL1D1: if((_tmp316.YY40).tag != 45)goto _LL1D3;_tmp317=(
struct Cyc_List_List*)(_tmp316.YY40).val;_LL1D2: yyzzz=_tmp317;goto _LL1D0;_LL1D3:;
_LL1D4:(int)_throw((void*)& Cyc_yyfail_YY40);_LL1D0:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp318;(_tmp318.YY40).val=
yy1;(_tmp318.YY40).tag=45;_tmp318;});}static char _tmp31A[24]="list_t<designator_t,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp31A,
_tmp31A,_tmp31A + 24}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp31B=yy1;struct Cyc_List_List*
_tmp31C;_LL1D6: if((_tmp31B.YY41).tag != 46)goto _LL1D8;_tmp31C=(struct Cyc_List_List*)(
_tmp31B.YY41).val;_LL1D7: yyzzz=_tmp31C;goto _LL1D5;_LL1D8:;_LL1D9:(int)_throw((
void*)& Cyc_yyfail_YY41);_LL1D5:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp31D;(_tmp31D.YY41).val=yy1;(_tmp31D.YY41).tag=
46;_tmp31D;});}static char _tmp31F[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp31F,_tmp31F,_tmp31F + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp320=yy1;void*_tmp321;_LL1DB:
if((_tmp320.YY42).tag != 47)goto _LL1DD;_tmp321=(void*)(_tmp320.YY42).val;_LL1DC:
yyzzz=_tmp321;goto _LL1DA;_LL1DD:;_LL1DE:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1DA:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE
_tmp322;(_tmp322.YY42).val=yy1;(_tmp322.YY42).tag=47;_tmp322;});}static char
_tmp324[7]="kind_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{
_tmp324,_tmp324,_tmp324 + 7}};enum Cyc_Absyn_Kind Cyc_yyget_YY43(union Cyc_YYSTYPE
yy1){enum Cyc_Absyn_Kind yyzzz;{union Cyc_YYSTYPE _tmp325=yy1;enum Cyc_Absyn_Kind
_tmp326;_LL1E0: if((_tmp325.YY43).tag != 48)goto _LL1E2;_tmp326=(enum Cyc_Absyn_Kind)(
_tmp325.YY43).val;_LL1E1: yyzzz=_tmp326;goto _LL1DF;_LL1E2:;_LL1E3:(int)_throw((
void*)& Cyc_yyfail_YY43);_LL1DF:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY43(enum Cyc_Absyn_Kind
yy1){return({union Cyc_YYSTYPE _tmp327;(_tmp327.YY43).val=yy1;(_tmp327.YY43).tag=
48;_tmp327;});}static char _tmp329[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp329,_tmp329,_tmp329 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp32A=yy1;void*_tmp32B;_LL1E5:
if((_tmp32A.YY44).tag != 49)goto _LL1E7;_tmp32B=(void*)(_tmp32A.YY44).val;_LL1E6:
yyzzz=_tmp32B;goto _LL1E4;_LL1E7:;_LL1E8:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1E4:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE
_tmp32C;(_tmp32C.YY44).val=yy1;(_tmp32C.YY44).tag=49;_tmp32C;});}static char
_tmp32E[23]="list_t<attribute_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY45={
Cyc_Core_Failure,{_tmp32E,_tmp32E,_tmp32E + 23}};struct Cyc_List_List*Cyc_yyget_YY45(
union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp32F=yy1;
struct Cyc_List_List*_tmp330;_LL1EA: if((_tmp32F.YY45).tag != 50)goto _LL1EC;_tmp330=(
struct Cyc_List_List*)(_tmp32F.YY45).val;_LL1EB: yyzzz=_tmp330;goto _LL1E9;_LL1EC:;
_LL1ED:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1E9:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp331;(_tmp331.YY45).val=
yy1;(_tmp331.YY45).tag=50;_tmp331;});}static char _tmp333[12]="attribute_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp333,_tmp333,
_tmp333 + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){void*yyzzz;{union Cyc_YYSTYPE
_tmp334=yy1;void*_tmp335;_LL1EF: if((_tmp334.YY46).tag != 51)goto _LL1F1;_tmp335=(
void*)(_tmp334.YY46).val;_LL1F0: yyzzz=_tmp335;goto _LL1EE;_LL1F1:;_LL1F2:(int)
_throw((void*)& Cyc_yyfail_YY46);_LL1EE:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY46(
void*yy1){return({union Cyc_YYSTYPE _tmp336;(_tmp336.YY46).val=yy1;(_tmp336.YY46).tag=
51;_tmp336;});}static char _tmp338[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp338,_tmp338,_tmp338 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE
_tmp339=yy1;struct Cyc_Absyn_Enumfield*_tmp33A;_LL1F4: if((_tmp339.YY47).tag != 52)
goto _LL1F6;_tmp33A=(struct Cyc_Absyn_Enumfield*)(_tmp339.YY47).val;_LL1F5: yyzzz=
_tmp33A;goto _LL1F3;_LL1F6:;_LL1F7:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1F3:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({
union Cyc_YYSTYPE _tmp33B;(_tmp33B.YY47).val=yy1;(_tmp33B.YY47).tag=52;_tmp33B;});}
static char _tmp33D[23]="list_t<enumfield_t,`H>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp33D,_tmp33D,_tmp33D + 23}};struct Cyc_List_List*
Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE
_tmp33E=yy1;struct Cyc_List_List*_tmp33F;_LL1F9: if((_tmp33E.YY48).tag != 53)goto
_LL1FB;_tmp33F=(struct Cyc_List_List*)(_tmp33E.YY48).val;_LL1FA: yyzzz=_tmp33F;
goto _LL1F8;_LL1FB:;_LL1FC:(int)_throw((void*)& Cyc_yyfail_YY48);_LL1F8:;}return
yyzzz;}union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE
_tmp340;(_tmp340.YY48).val=yy1;(_tmp340.YY48).tag=53;_tmp340;});}static char
_tmp342[17]="opt_t<type_t,`H>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={
Cyc_Core_Failure,{_tmp342,_tmp342,_tmp342 + 17}};struct Cyc_Core_Opt*Cyc_yyget_YY49(
union Cyc_YYSTYPE yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE _tmp343=yy1;
struct Cyc_Core_Opt*_tmp344;_LL1FE: if((_tmp343.YY49).tag != 54)goto _LL200;_tmp344=(
struct Cyc_Core_Opt*)(_tmp343.YY49).val;_LL1FF: yyzzz=_tmp344;goto _LL1FD;_LL200:;
_LL201:(int)_throw((void*)& Cyc_yyfail_YY49);_LL1FD:;}return yyzzz;}union Cyc_YYSTYPE
Cyc_YY49(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp345;(_tmp345.YY49).val=
yy1;(_tmp345.YY49).tag=54;_tmp345;});}static char _tmp347[31]="list_t<$(type_t,type_t)@`H,`H>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp347,
_tmp347,_tmp347 + 31}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp348=yy1;struct Cyc_List_List*
_tmp349;_LL203: if((_tmp348.YY50).tag != 55)goto _LL205;_tmp349=(struct Cyc_List_List*)(
_tmp348.YY50).val;_LL204: yyzzz=_tmp349;goto _LL202;_LL205:;_LL206:(int)_throw((
void*)& Cyc_yyfail_YY50);_LL202:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp34A;(_tmp34A.YY50).val=yy1;(_tmp34A.YY50).tag=
55;_tmp34A;});}static char _tmp34C[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp34C,_tmp34C,_tmp34C + 15}};union Cyc_Absyn_Constraint*
Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){union Cyc_Absyn_Constraint*yyzzz;{union Cyc_YYSTYPE
_tmp34D=yy1;union Cyc_Absyn_Constraint*_tmp34E;_LL208: if((_tmp34D.YY51).tag != 56)
goto _LL20A;_tmp34E=(union Cyc_Absyn_Constraint*)(_tmp34D.YY51).val;_LL209: yyzzz=
_tmp34E;goto _LL207;_LL20A:;_LL20B:(int)_throw((void*)& Cyc_yyfail_YY51);_LL207:;}
return yyzzz;}union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){return({
union Cyc_YYSTYPE _tmp34F;(_tmp34F.YY51).val=yy1;(_tmp34F.YY51).tag=56;_tmp34F;});}
static char _tmp351[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp351,_tmp351,
_tmp351 + 45}};struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE _tmp352=yy1;struct Cyc_List_List*_tmp353;_LL20D: if((
_tmp352.YY52).tag != 57)goto _LL20F;_tmp353=(struct Cyc_List_List*)(_tmp352.YY52).val;
_LL20E: yyzzz=_tmp353;goto _LL20C;_LL20F:;_LL210:(int)_throw((void*)& Cyc_yyfail_YY52);
_LL20C:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({
union Cyc_YYSTYPE _tmp354;(_tmp354.YY52).val=yy1;(_tmp354.YY52).tag=57;_tmp354;});}
static char _tmp356[15]="pointer_qual_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY53={
Cyc_Core_Failure,{_tmp356,_tmp356,_tmp356 + 15}};void*Cyc_yyget_YY53(union Cyc_YYSTYPE
yy1){void*yyzzz;{union Cyc_YYSTYPE _tmp357=yy1;void*_tmp358;_LL212: if((_tmp357.YY53).tag
!= 58)goto _LL214;_tmp358=(void*)(_tmp357.YY53).val;_LL213: yyzzz=_tmp358;goto
_LL211;_LL214:;_LL215:(int)_throw((void*)& Cyc_yyfail_YY53);_LL211:;}return yyzzz;}
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp359;(_tmp359.YY53).val=
yy1;(_tmp359.YY53).tag=58;_tmp359;});}static char _tmp35B[16]="pointer_quals_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp35B,
_tmp35B,_tmp35B + 16}};struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE _tmp35C=yy1;struct Cyc_List_List*
_tmp35D;_LL217: if((_tmp35C.YY54).tag != 59)goto _LL219;_tmp35D=(struct Cyc_List_List*)(
_tmp35C.YY54).val;_LL218: yyzzz=_tmp35D;goto _LL216;_LL219:;_LL21A:(int)_throw((
void*)& Cyc_yyfail_YY54);_LL216:;}return yyzzz;}union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*
yy1){return({union Cyc_YYSTYPE _tmp35E;(_tmp35E.YY54).val=yy1;(_tmp35E.YY54).tag=
59;_tmp35E;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int
last_line;int last_column;};struct Cyc_Yyltype Cyc_yynewloc(){return({struct Cyc_Yyltype
_tmp35F;_tmp35F.timestamp=0;_tmp35F.first_line=0;_tmp35F.first_column=0;_tmp35F.last_line=
0;_tmp35F.last_column=0;_tmp35F;});}struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
static short Cyc_yytranslate[371]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,142,2,2,126,140,137,2,123,124,131,134,119,138,128,139,2,2,2,2,
2,2,2,2,2,2,127,116,121,120,122,133,132,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,129,2,130,136,125,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,117,135,118,141,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,
45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,
72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,
99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115};static char
_tmp360[2]="$";static char _tmp361[6]="error";static char _tmp362[12]="$undefined.";
static char _tmp363[5]="AUTO";static char _tmp364[9]="REGISTER";static char _tmp365[7]="STATIC";
static char _tmp366[7]="EXTERN";static char _tmp367[8]="TYPEDEF";static char _tmp368[5]="VOID";
static char _tmp369[5]="CHAR";static char _tmp36A[6]="SHORT";static char _tmp36B[4]="INT";
static char _tmp36C[5]="LONG";static char _tmp36D[6]="FLOAT";static char _tmp36E[7]="DOUBLE";
static char _tmp36F[7]="SIGNED";static char _tmp370[9]="UNSIGNED";static char _tmp371[
6]="CONST";static char _tmp372[9]="VOLATILE";static char _tmp373[9]="RESTRICT";
static char _tmp374[7]="STRUCT";static char _tmp375[6]="UNION";static char _tmp376[5]="CASE";
static char _tmp377[8]="DEFAULT";static char _tmp378[7]="INLINE";static char _tmp379[7]="SIZEOF";
static char _tmp37A[9]="OFFSETOF";static char _tmp37B[3]="IF";static char _tmp37C[5]="ELSE";
static char _tmp37D[7]="SWITCH";static char _tmp37E[6]="WHILE";static char _tmp37F[3]="DO";
static char _tmp380[4]="FOR";static char _tmp381[5]="GOTO";static char _tmp382[9]="CONTINUE";
static char _tmp383[6]="BREAK";static char _tmp384[7]="RETURN";static char _tmp385[5]="ENUM";
static char _tmp386[8]="NULL_kw";static char _tmp387[4]="LET";static char _tmp388[6]="THROW";
static char _tmp389[4]="TRY";static char _tmp38A[6]="CATCH";static char _tmp38B[7]="EXPORT";
static char _tmp38C[4]="NEW";static char _tmp38D[9]="ABSTRACT";static char _tmp38E[9]="FALLTHRU";
static char _tmp38F[6]="USING";static char _tmp390[10]="NAMESPACE";static char _tmp391[
9]="DATATYPE";static char _tmp392[8]="XTUNION";static char _tmp393[7]="TUNION";
static char _tmp394[7]="MALLOC";static char _tmp395[8]="RMALLOC";static char _tmp396[7]="CALLOC";
static char _tmp397[8]="RCALLOC";static char _tmp398[5]="SWAP";static char _tmp399[9]="REGION_T";
static char _tmp39A[6]="TAG_T";static char _tmp39B[7]="REGION";static char _tmp39C[5]="RNEW";
static char _tmp39D[8]="REGIONS";static char _tmp39E[13]="RESET_REGION";static char
_tmp39F[16]="NOZEROTERM_QUAL";static char _tmp3A0[14]="ZEROTERM_QUAL";static char
_tmp3A1[12]="REGION_QUAL";static char _tmp3A2[7]="PORTON";static char _tmp3A3[8]="PORTOFF";
static char _tmp3A4[12]="DYNREGION_T";static char _tmp3A5[6]="ALIAS";static char
_tmp3A6[8]="NUMELTS";static char _tmp3A7[8]="VALUEOF";static char _tmp3A8[10]="VALUEOF_T";
static char _tmp3A9[9]="TAGCHECK";static char _tmp3AA[13]="NUMELTS_QUAL";static char
_tmp3AB[10]="THIN_QUAL";static char _tmp3AC[9]="FAT_QUAL";static char _tmp3AD[13]="NOTNULL_QUAL";
static char _tmp3AE[14]="NULLABLE_QUAL";static char _tmp3AF[12]="TAGGED_QUAL";static
char _tmp3B0[16]="EXTENSIBLE_QUAL";static char _tmp3B1[15]="RESETABLE_QUAL";static
char _tmp3B2[7]="PTR_OP";static char _tmp3B3[7]="INC_OP";static char _tmp3B4[7]="DEC_OP";
static char _tmp3B5[8]="LEFT_OP";static char _tmp3B6[9]="RIGHT_OP";static char _tmp3B7[
6]="LE_OP";static char _tmp3B8[6]="GE_OP";static char _tmp3B9[6]="EQ_OP";static char
_tmp3BA[6]="NE_OP";static char _tmp3BB[7]="AND_OP";static char _tmp3BC[6]="OR_OP";
static char _tmp3BD[11]="MUL_ASSIGN";static char _tmp3BE[11]="DIV_ASSIGN";static char
_tmp3BF[11]="MOD_ASSIGN";static char _tmp3C0[11]="ADD_ASSIGN";static char _tmp3C1[11]="SUB_ASSIGN";
static char _tmp3C2[12]="LEFT_ASSIGN";static char _tmp3C3[13]="RIGHT_ASSIGN";static
char _tmp3C4[11]="AND_ASSIGN";static char _tmp3C5[11]="XOR_ASSIGN";static char
_tmp3C6[10]="OR_ASSIGN";static char _tmp3C7[9]="ELLIPSIS";static char _tmp3C8[11]="LEFT_RIGHT";
static char _tmp3C9[12]="COLON_COLON";static char _tmp3CA[11]="IDENTIFIER";static
char _tmp3CB[17]="INTEGER_CONSTANT";static char _tmp3CC[7]="STRING";static char
_tmp3CD[19]="CHARACTER_CONSTANT";static char _tmp3CE[18]="FLOATING_CONSTANT";
static char _tmp3CF[9]="TYPE_VAR";static char _tmp3D0[13]="TYPEDEF_NAME";static char
_tmp3D1[16]="QUAL_IDENTIFIER";static char _tmp3D2[18]="QUAL_TYPEDEF_NAME";static
char _tmp3D3[10]="ATTRIBUTE";static char _tmp3D4[4]="';'";static char _tmp3D5[4]="'{'";
static char _tmp3D6[4]="'}'";static char _tmp3D7[4]="','";static char _tmp3D8[4]="'='";
static char _tmp3D9[4]="'<'";static char _tmp3DA[4]="'>'";static char _tmp3DB[4]="'('";
static char _tmp3DC[4]="')'";static char _tmp3DD[4]="'_'";static char _tmp3DE[4]="'$'";
static char _tmp3DF[4]="':'";static char _tmp3E0[4]="'.'";static char _tmp3E1[4]="'['";
static char _tmp3E2[4]="']'";static char _tmp3E3[4]="'*'";static char _tmp3E4[4]="'@'";
static char _tmp3E5[4]="'?'";static char _tmp3E6[4]="'+'";static char _tmp3E7[4]="'|'";
static char _tmp3E8[4]="'^'";static char _tmp3E9[4]="'&'";static char _tmp3EA[4]="'-'";
static char _tmp3EB[4]="'/'";static char _tmp3EC[4]="'%'";static char _tmp3ED[4]="'~'";
static char _tmp3EE[4]="'!'";static char _tmp3EF[5]="prog";static char _tmp3F0[17]="translation_unit";
static char _tmp3F1[12]="export_list";static char _tmp3F2[19]="export_list_values";
static char _tmp3F3[21]="external_declaration";static char _tmp3F4[20]="function_definition";
static char _tmp3F5[21]="function_definition2";static char _tmp3F6[13]="using_action";
static char _tmp3F7[15]="unusing_action";static char _tmp3F8[17]="namespace_action";
static char _tmp3F9[19]="unnamespace_action";static char _tmp3FA[12]="declaration";
static char _tmp3FB[19]="resetable_qual_opt";static char _tmp3FC[17]="declaration_list";
static char _tmp3FD[23]="declaration_specifiers";static char _tmp3FE[24]="storage_class_specifier";
static char _tmp3FF[15]="attributes_opt";static char _tmp400[11]="attributes";static
char _tmp401[15]="attribute_list";static char _tmp402[10]="attribute";static char
_tmp403[15]="type_specifier";static char _tmp404[25]="type_specifier_notypedef";
static char _tmp405[5]="kind";static char _tmp406[15]="type_qualifier";static char
_tmp407[15]="enum_specifier";static char _tmp408[11]="enum_field";static char
_tmp409[22]="enum_declaration_list";static char _tmp40A[26]="struct_or_union_specifier";
static char _tmp40B[16]="type_params_opt";static char _tmp40C[16]="struct_or_union";
static char _tmp40D[24]="struct_declaration_list";static char _tmp40E[25]="struct_declaration_list0";
static char _tmp40F[21]="init_declarator_list";static char _tmp410[22]="init_declarator_list0";
static char _tmp411[16]="init_declarator";static char _tmp412[19]="struct_declaration";
static char _tmp413[25]="specifier_qualifier_list";static char _tmp414[35]="notypedef_specifier_qualifier_list";
static char _tmp415[23]="struct_declarator_list";static char _tmp416[24]="struct_declarator_list0";
static char _tmp417[18]="struct_declarator";static char _tmp418[19]="datatype_specifier";
static char _tmp419[14]="qual_datatype";static char _tmp41A[19]="datatypefield_list";
static char _tmp41B[20]="datatypefield_scope";static char _tmp41C[14]="datatypefield";
static char _tmp41D[11]="declarator";static char _tmp41E[23]="declarator_withtypedef";
static char _tmp41F[18]="direct_declarator";static char _tmp420[30]="direct_declarator_withtypedef";
static char _tmp421[8]="pointer";static char _tmp422[12]="one_pointer";static char
_tmp423[14]="pointer_quals";static char _tmp424[13]="pointer_qual";static char
_tmp425[23]="pointer_null_and_bound";static char _tmp426[14]="pointer_bound";
static char _tmp427[18]="zeroterm_qual_opt";static char _tmp428[8]="rgn_opt";static
char _tmp429[11]="tqual_list";static char _tmp42A[20]="parameter_type_list";static
char _tmp42B[9]="type_var";static char _tmp42C[16]="optional_effect";static char
_tmp42D[19]="optional_rgn_order";static char _tmp42E[10]="rgn_order";static char
_tmp42F[16]="optional_inject";static char _tmp430[11]="effect_set";static char
_tmp431[14]="atomic_effect";static char _tmp432[11]="region_set";static char _tmp433[
15]="parameter_list";static char _tmp434[22]="parameter_declaration";static char
_tmp435[16]="identifier_list";static char _tmp436[17]="identifier_list0";static
char _tmp437[12]="initializer";static char _tmp438[18]="array_initializer";static
char _tmp439[17]="initializer_list";static char _tmp43A[12]="designation";static
char _tmp43B[16]="designator_list";static char _tmp43C[11]="designator";static char
_tmp43D[10]="type_name";static char _tmp43E[14]="any_type_name";static char _tmp43F[
15]="type_name_list";static char _tmp440[20]="abstract_declarator";static char
_tmp441[27]="direct_abstract_declarator";static char _tmp442[10]="statement";
static char _tmp443[13]="open_exp_opt";static char _tmp444[18]="labeled_statement";
static char _tmp445[21]="expression_statement";static char _tmp446[19]="compound_statement";
static char _tmp447[16]="block_item_list";static char _tmp448[20]="selection_statement";
static char _tmp449[15]="switch_clauses";static char _tmp44A[20]="iteration_statement";
static char _tmp44B[15]="jump_statement";static char _tmp44C[12]="exp_pattern";
static char _tmp44D[20]="conditional_pattern";static char _tmp44E[19]="logical_or_pattern";
static char _tmp44F[20]="logical_and_pattern";static char _tmp450[21]="inclusive_or_pattern";
static char _tmp451[21]="exclusive_or_pattern";static char _tmp452[12]="and_pattern";
static char _tmp453[17]="equality_pattern";static char _tmp454[19]="relational_pattern";
static char _tmp455[14]="shift_pattern";static char _tmp456[17]="additive_pattern";
static char _tmp457[23]="multiplicative_pattern";static char _tmp458[13]="cast_pattern";
static char _tmp459[14]="unary_pattern";static char _tmp45A[16]="postfix_pattern";
static char _tmp45B[16]="primary_pattern";static char _tmp45C[8]="pattern";static
char _tmp45D[19]="tuple_pattern_list";static char _tmp45E[20]="tuple_pattern_list0";
static char _tmp45F[14]="field_pattern";static char _tmp460[19]="field_pattern_list";
static char _tmp461[20]="field_pattern_list0";static char _tmp462[11]="expression";
static char _tmp463[22]="assignment_expression";static char _tmp464[20]="assignment_operator";
static char _tmp465[23]="conditional_expression";static char _tmp466[20]="constant_expression";
static char _tmp467[22]="logical_or_expression";static char _tmp468[23]="logical_and_expression";
static char _tmp469[24]="inclusive_or_expression";static char _tmp46A[24]="exclusive_or_expression";
static char _tmp46B[15]="and_expression";static char _tmp46C[20]="equality_expression";
static char _tmp46D[22]="relational_expression";static char _tmp46E[17]="shift_expression";
static char _tmp46F[20]="additive_expression";static char _tmp470[26]="multiplicative_expression";
static char _tmp471[16]="cast_expression";static char _tmp472[17]="unary_expression";
static char _tmp473[15]="unary_operator";static char _tmp474[19]="postfix_expression";
static char _tmp475[19]="primary_expression";static char _tmp476[25]="argument_expression_list";
static char _tmp477[26]="argument_expression_list0";static char _tmp478[9]="constant";
static char _tmp479[20]="qual_opt_identifier";static char _tmp47A[17]="qual_opt_typedef";
static char _tmp47B[18]="struct_union_name";static char _tmp47C[11]="field_name";
static char _tmp47D[12]="right_angle";static struct _dyneither_ptr Cyc_yytname[286]={{
_tmp360,_tmp360,_tmp360 + 2},{_tmp361,_tmp361,_tmp361 + 6},{_tmp362,_tmp362,
_tmp362 + 12},{_tmp363,_tmp363,_tmp363 + 5},{_tmp364,_tmp364,_tmp364 + 9},{_tmp365,
_tmp365,_tmp365 + 7},{_tmp366,_tmp366,_tmp366 + 7},{_tmp367,_tmp367,_tmp367 + 8},{
_tmp368,_tmp368,_tmp368 + 5},{_tmp369,_tmp369,_tmp369 + 5},{_tmp36A,_tmp36A,
_tmp36A + 6},{_tmp36B,_tmp36B,_tmp36B + 4},{_tmp36C,_tmp36C,_tmp36C + 5},{_tmp36D,
_tmp36D,_tmp36D + 6},{_tmp36E,_tmp36E,_tmp36E + 7},{_tmp36F,_tmp36F,_tmp36F + 7},{
_tmp370,_tmp370,_tmp370 + 9},{_tmp371,_tmp371,_tmp371 + 6},{_tmp372,_tmp372,
_tmp372 + 9},{_tmp373,_tmp373,_tmp373 + 9},{_tmp374,_tmp374,_tmp374 + 7},{_tmp375,
_tmp375,_tmp375 + 6},{_tmp376,_tmp376,_tmp376 + 5},{_tmp377,_tmp377,_tmp377 + 8},{
_tmp378,_tmp378,_tmp378 + 7},{_tmp379,_tmp379,_tmp379 + 7},{_tmp37A,_tmp37A,
_tmp37A + 9},{_tmp37B,_tmp37B,_tmp37B + 3},{_tmp37C,_tmp37C,_tmp37C + 5},{_tmp37D,
_tmp37D,_tmp37D + 7},{_tmp37E,_tmp37E,_tmp37E + 6},{_tmp37F,_tmp37F,_tmp37F + 3},{
_tmp380,_tmp380,_tmp380 + 4},{_tmp381,_tmp381,_tmp381 + 5},{_tmp382,_tmp382,
_tmp382 + 9},{_tmp383,_tmp383,_tmp383 + 6},{_tmp384,_tmp384,_tmp384 + 7},{_tmp385,
_tmp385,_tmp385 + 5},{_tmp386,_tmp386,_tmp386 + 8},{_tmp387,_tmp387,_tmp387 + 4},{
_tmp388,_tmp388,_tmp388 + 6},{_tmp389,_tmp389,_tmp389 + 4},{_tmp38A,_tmp38A,
_tmp38A + 6},{_tmp38B,_tmp38B,_tmp38B + 7},{_tmp38C,_tmp38C,_tmp38C + 4},{_tmp38D,
_tmp38D,_tmp38D + 9},{_tmp38E,_tmp38E,_tmp38E + 9},{_tmp38F,_tmp38F,_tmp38F + 6},{
_tmp390,_tmp390,_tmp390 + 10},{_tmp391,_tmp391,_tmp391 + 9},{_tmp392,_tmp392,
_tmp392 + 8},{_tmp393,_tmp393,_tmp393 + 7},{_tmp394,_tmp394,_tmp394 + 7},{_tmp395,
_tmp395,_tmp395 + 8},{_tmp396,_tmp396,_tmp396 + 7},{_tmp397,_tmp397,_tmp397 + 8},{
_tmp398,_tmp398,_tmp398 + 5},{_tmp399,_tmp399,_tmp399 + 9},{_tmp39A,_tmp39A,
_tmp39A + 6},{_tmp39B,_tmp39B,_tmp39B + 7},{_tmp39C,_tmp39C,_tmp39C + 5},{_tmp39D,
_tmp39D,_tmp39D + 8},{_tmp39E,_tmp39E,_tmp39E + 13},{_tmp39F,_tmp39F,_tmp39F + 16},{
_tmp3A0,_tmp3A0,_tmp3A0 + 14},{_tmp3A1,_tmp3A1,_tmp3A1 + 12},{_tmp3A2,_tmp3A2,
_tmp3A2 + 7},{_tmp3A3,_tmp3A3,_tmp3A3 + 8},{_tmp3A4,_tmp3A4,_tmp3A4 + 12},{_tmp3A5,
_tmp3A5,_tmp3A5 + 6},{_tmp3A6,_tmp3A6,_tmp3A6 + 8},{_tmp3A7,_tmp3A7,_tmp3A7 + 8},{
_tmp3A8,_tmp3A8,_tmp3A8 + 10},{_tmp3A9,_tmp3A9,_tmp3A9 + 9},{_tmp3AA,_tmp3AA,
_tmp3AA + 13},{_tmp3AB,_tmp3AB,_tmp3AB + 10},{_tmp3AC,_tmp3AC,_tmp3AC + 9},{_tmp3AD,
_tmp3AD,_tmp3AD + 13},{_tmp3AE,_tmp3AE,_tmp3AE + 14},{_tmp3AF,_tmp3AF,_tmp3AF + 12},{
_tmp3B0,_tmp3B0,_tmp3B0 + 16},{_tmp3B1,_tmp3B1,_tmp3B1 + 15},{_tmp3B2,_tmp3B2,
_tmp3B2 + 7},{_tmp3B3,_tmp3B3,_tmp3B3 + 7},{_tmp3B4,_tmp3B4,_tmp3B4 + 7},{_tmp3B5,
_tmp3B5,_tmp3B5 + 8},{_tmp3B6,_tmp3B6,_tmp3B6 + 9},{_tmp3B7,_tmp3B7,_tmp3B7 + 6},{
_tmp3B8,_tmp3B8,_tmp3B8 + 6},{_tmp3B9,_tmp3B9,_tmp3B9 + 6},{_tmp3BA,_tmp3BA,
_tmp3BA + 6},{_tmp3BB,_tmp3BB,_tmp3BB + 7},{_tmp3BC,_tmp3BC,_tmp3BC + 6},{_tmp3BD,
_tmp3BD,_tmp3BD + 11},{_tmp3BE,_tmp3BE,_tmp3BE + 11},{_tmp3BF,_tmp3BF,_tmp3BF + 11},{
_tmp3C0,_tmp3C0,_tmp3C0 + 11},{_tmp3C1,_tmp3C1,_tmp3C1 + 11},{_tmp3C2,_tmp3C2,
_tmp3C2 + 12},{_tmp3C3,_tmp3C3,_tmp3C3 + 13},{_tmp3C4,_tmp3C4,_tmp3C4 + 11},{
_tmp3C5,_tmp3C5,_tmp3C5 + 11},{_tmp3C6,_tmp3C6,_tmp3C6 + 10},{_tmp3C7,_tmp3C7,
_tmp3C7 + 9},{_tmp3C8,_tmp3C8,_tmp3C8 + 11},{_tmp3C9,_tmp3C9,_tmp3C9 + 12},{_tmp3CA,
_tmp3CA,_tmp3CA + 11},{_tmp3CB,_tmp3CB,_tmp3CB + 17},{_tmp3CC,_tmp3CC,_tmp3CC + 7},{
_tmp3CD,_tmp3CD,_tmp3CD + 19},{_tmp3CE,_tmp3CE,_tmp3CE + 18},{_tmp3CF,_tmp3CF,
_tmp3CF + 9},{_tmp3D0,_tmp3D0,_tmp3D0 + 13},{_tmp3D1,_tmp3D1,_tmp3D1 + 16},{_tmp3D2,
_tmp3D2,_tmp3D2 + 18},{_tmp3D3,_tmp3D3,_tmp3D3 + 10},{_tmp3D4,_tmp3D4,_tmp3D4 + 4},{
_tmp3D5,_tmp3D5,_tmp3D5 + 4},{_tmp3D6,_tmp3D6,_tmp3D6 + 4},{_tmp3D7,_tmp3D7,
_tmp3D7 + 4},{_tmp3D8,_tmp3D8,_tmp3D8 + 4},{_tmp3D9,_tmp3D9,_tmp3D9 + 4},{_tmp3DA,
_tmp3DA,_tmp3DA + 4},{_tmp3DB,_tmp3DB,_tmp3DB + 4},{_tmp3DC,_tmp3DC,_tmp3DC + 4},{
_tmp3DD,_tmp3DD,_tmp3DD + 4},{_tmp3DE,_tmp3DE,_tmp3DE + 4},{_tmp3DF,_tmp3DF,
_tmp3DF + 4},{_tmp3E0,_tmp3E0,_tmp3E0 + 4},{_tmp3E1,_tmp3E1,_tmp3E1 + 4},{_tmp3E2,
_tmp3E2,_tmp3E2 + 4},{_tmp3E3,_tmp3E3,_tmp3E3 + 4},{_tmp3E4,_tmp3E4,_tmp3E4 + 4},{
_tmp3E5,_tmp3E5,_tmp3E5 + 4},{_tmp3E6,_tmp3E6,_tmp3E6 + 4},{_tmp3E7,_tmp3E7,
_tmp3E7 + 4},{_tmp3E8,_tmp3E8,_tmp3E8 + 4},{_tmp3E9,_tmp3E9,_tmp3E9 + 4},{_tmp3EA,
_tmp3EA,_tmp3EA + 4},{_tmp3EB,_tmp3EB,_tmp3EB + 4},{_tmp3EC,_tmp3EC,_tmp3EC + 4},{
_tmp3ED,_tmp3ED,_tmp3ED + 4},{_tmp3EE,_tmp3EE,_tmp3EE + 4},{_tmp3EF,_tmp3EF,
_tmp3EF + 5},{_tmp3F0,_tmp3F0,_tmp3F0 + 17},{_tmp3F1,_tmp3F1,_tmp3F1 + 12},{_tmp3F2,
_tmp3F2,_tmp3F2 + 19},{_tmp3F3,_tmp3F3,_tmp3F3 + 21},{_tmp3F4,_tmp3F4,_tmp3F4 + 20},{
_tmp3F5,_tmp3F5,_tmp3F5 + 21},{_tmp3F6,_tmp3F6,_tmp3F6 + 13},{_tmp3F7,_tmp3F7,
_tmp3F7 + 15},{_tmp3F8,_tmp3F8,_tmp3F8 + 17},{_tmp3F9,_tmp3F9,_tmp3F9 + 19},{
_tmp3FA,_tmp3FA,_tmp3FA + 12},{_tmp3FB,_tmp3FB,_tmp3FB + 19},{_tmp3FC,_tmp3FC,
_tmp3FC + 17},{_tmp3FD,_tmp3FD,_tmp3FD + 23},{_tmp3FE,_tmp3FE,_tmp3FE + 24},{
_tmp3FF,_tmp3FF,_tmp3FF + 15},{_tmp400,_tmp400,_tmp400 + 11},{_tmp401,_tmp401,
_tmp401 + 15},{_tmp402,_tmp402,_tmp402 + 10},{_tmp403,_tmp403,_tmp403 + 15},{
_tmp404,_tmp404,_tmp404 + 25},{_tmp405,_tmp405,_tmp405 + 5},{_tmp406,_tmp406,
_tmp406 + 15},{_tmp407,_tmp407,_tmp407 + 15},{_tmp408,_tmp408,_tmp408 + 11},{
_tmp409,_tmp409,_tmp409 + 22},{_tmp40A,_tmp40A,_tmp40A + 26},{_tmp40B,_tmp40B,
_tmp40B + 16},{_tmp40C,_tmp40C,_tmp40C + 16},{_tmp40D,_tmp40D,_tmp40D + 24},{
_tmp40E,_tmp40E,_tmp40E + 25},{_tmp40F,_tmp40F,_tmp40F + 21},{_tmp410,_tmp410,
_tmp410 + 22},{_tmp411,_tmp411,_tmp411 + 16},{_tmp412,_tmp412,_tmp412 + 19},{
_tmp413,_tmp413,_tmp413 + 25},{_tmp414,_tmp414,_tmp414 + 35},{_tmp415,_tmp415,
_tmp415 + 23},{_tmp416,_tmp416,_tmp416 + 24},{_tmp417,_tmp417,_tmp417 + 18},{
_tmp418,_tmp418,_tmp418 + 19},{_tmp419,_tmp419,_tmp419 + 14},{_tmp41A,_tmp41A,
_tmp41A + 19},{_tmp41B,_tmp41B,_tmp41B + 20},{_tmp41C,_tmp41C,_tmp41C + 14},{
_tmp41D,_tmp41D,_tmp41D + 11},{_tmp41E,_tmp41E,_tmp41E + 23},{_tmp41F,_tmp41F,
_tmp41F + 18},{_tmp420,_tmp420,_tmp420 + 30},{_tmp421,_tmp421,_tmp421 + 8},{_tmp422,
_tmp422,_tmp422 + 12},{_tmp423,_tmp423,_tmp423 + 14},{_tmp424,_tmp424,_tmp424 + 13},{
_tmp425,_tmp425,_tmp425 + 23},{_tmp426,_tmp426,_tmp426 + 14},{_tmp427,_tmp427,
_tmp427 + 18},{_tmp428,_tmp428,_tmp428 + 8},{_tmp429,_tmp429,_tmp429 + 11},{_tmp42A,
_tmp42A,_tmp42A + 20},{_tmp42B,_tmp42B,_tmp42B + 9},{_tmp42C,_tmp42C,_tmp42C + 16},{
_tmp42D,_tmp42D,_tmp42D + 19},{_tmp42E,_tmp42E,_tmp42E + 10},{_tmp42F,_tmp42F,
_tmp42F + 16},{_tmp430,_tmp430,_tmp430 + 11},{_tmp431,_tmp431,_tmp431 + 14},{
_tmp432,_tmp432,_tmp432 + 11},{_tmp433,_tmp433,_tmp433 + 15},{_tmp434,_tmp434,
_tmp434 + 22},{_tmp435,_tmp435,_tmp435 + 16},{_tmp436,_tmp436,_tmp436 + 17},{
_tmp437,_tmp437,_tmp437 + 12},{_tmp438,_tmp438,_tmp438 + 18},{_tmp439,_tmp439,
_tmp439 + 17},{_tmp43A,_tmp43A,_tmp43A + 12},{_tmp43B,_tmp43B,_tmp43B + 16},{
_tmp43C,_tmp43C,_tmp43C + 11},{_tmp43D,_tmp43D,_tmp43D + 10},{_tmp43E,_tmp43E,
_tmp43E + 14},{_tmp43F,_tmp43F,_tmp43F + 15},{_tmp440,_tmp440,_tmp440 + 20},{
_tmp441,_tmp441,_tmp441 + 27},{_tmp442,_tmp442,_tmp442 + 10},{_tmp443,_tmp443,
_tmp443 + 13},{_tmp444,_tmp444,_tmp444 + 18},{_tmp445,_tmp445,_tmp445 + 21},{
_tmp446,_tmp446,_tmp446 + 19},{_tmp447,_tmp447,_tmp447 + 16},{_tmp448,_tmp448,
_tmp448 + 20},{_tmp449,_tmp449,_tmp449 + 15},{_tmp44A,_tmp44A,_tmp44A + 20},{
_tmp44B,_tmp44B,_tmp44B + 15},{_tmp44C,_tmp44C,_tmp44C + 12},{_tmp44D,_tmp44D,
_tmp44D + 20},{_tmp44E,_tmp44E,_tmp44E + 19},{_tmp44F,_tmp44F,_tmp44F + 20},{
_tmp450,_tmp450,_tmp450 + 21},{_tmp451,_tmp451,_tmp451 + 21},{_tmp452,_tmp452,
_tmp452 + 12},{_tmp453,_tmp453,_tmp453 + 17},{_tmp454,_tmp454,_tmp454 + 19},{
_tmp455,_tmp455,_tmp455 + 14},{_tmp456,_tmp456,_tmp456 + 17},{_tmp457,_tmp457,
_tmp457 + 23},{_tmp458,_tmp458,_tmp458 + 13},{_tmp459,_tmp459,_tmp459 + 14},{
_tmp45A,_tmp45A,_tmp45A + 16},{_tmp45B,_tmp45B,_tmp45B + 16},{_tmp45C,_tmp45C,
_tmp45C + 8},{_tmp45D,_tmp45D,_tmp45D + 19},{_tmp45E,_tmp45E,_tmp45E + 20},{_tmp45F,
_tmp45F,_tmp45F + 14},{_tmp460,_tmp460,_tmp460 + 19},{_tmp461,_tmp461,_tmp461 + 20},{
_tmp462,_tmp462,_tmp462 + 11},{_tmp463,_tmp463,_tmp463 + 22},{_tmp464,_tmp464,
_tmp464 + 20},{_tmp465,_tmp465,_tmp465 + 23},{_tmp466,_tmp466,_tmp466 + 20},{
_tmp467,_tmp467,_tmp467 + 22},{_tmp468,_tmp468,_tmp468 + 23},{_tmp469,_tmp469,
_tmp469 + 24},{_tmp46A,_tmp46A,_tmp46A + 24},{_tmp46B,_tmp46B,_tmp46B + 15},{
_tmp46C,_tmp46C,_tmp46C + 20},{_tmp46D,_tmp46D,_tmp46D + 22},{_tmp46E,_tmp46E,
_tmp46E + 17},{_tmp46F,_tmp46F,_tmp46F + 20},{_tmp470,_tmp470,_tmp470 + 26},{
_tmp471,_tmp471,_tmp471 + 16},{_tmp472,_tmp472,_tmp472 + 17},{_tmp473,_tmp473,
_tmp473 + 15},{_tmp474,_tmp474,_tmp474 + 19},{_tmp475,_tmp475,_tmp475 + 19},{
_tmp476,_tmp476,_tmp476 + 25},{_tmp477,_tmp477,_tmp477 + 26},{_tmp478,_tmp478,
_tmp478 + 9},{_tmp479,_tmp479,_tmp479 + 20},{_tmp47A,_tmp47A,_tmp47A + 17},{_tmp47B,
_tmp47B,_tmp47B + 18},{_tmp47C,_tmp47C,_tmp47C + 11},{_tmp47D,_tmp47D,_tmp47D + 12}};
static short Cyc_yyr1[510]={0,143,144,144,144,144,144,144,144,144,144,144,145,145,
146,146,147,147,147,148,148,148,148,149,149,150,151,152,153,154,154,154,154,154,
154,154,155,155,156,156,157,157,157,157,157,157,157,157,157,157,158,158,158,158,
158,158,158,159,159,160,161,161,162,162,162,162,162,162,163,163,164,164,164,164,
164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,165,
166,166,166,167,167,167,168,168,169,169,169,170,170,170,170,170,171,171,172,172,
173,173,174,174,175,176,176,177,177,178,179,179,179,179,179,179,180,180,180,180,
180,180,181,182,182,183,183,183,184,184,184,185,185,186,186,186,186,187,187,187,
188,188,189,189,190,190,191,191,191,191,191,191,191,191,191,191,192,192,192,192,
192,192,192,192,192,192,192,193,193,194,195,195,196,196,196,196,196,196,196,196,
197,197,197,198,198,199,199,199,200,200,200,201,201,202,202,202,202,203,203,204,
204,205,205,206,206,207,207,208,208,209,209,209,209,210,210,211,211,212,212,212,
213,214,214,215,215,216,216,216,216,217,217,217,217,218,219,219,220,220,221,221,
222,222,222,222,222,223,223,224,224,224,225,225,225,225,225,225,225,225,225,225,
225,226,226,226,226,226,226,226,227,227,228,229,229,230,230,231,231,231,231,231,
231,232,232,232,232,232,232,233,233,233,233,233,233,234,234,234,234,234,234,234,
234,234,234,234,234,234,234,235,235,235,235,235,235,235,235,236,237,237,238,238,
239,239,240,240,241,241,242,242,243,243,243,244,244,244,244,244,245,245,245,246,
246,246,247,247,247,247,248,248,249,249,249,249,249,249,249,250,251,252,252,252,
252,252,252,252,252,252,252,252,252,252,252,252,253,253,253,254,254,255,255,256,
256,256,257,257,258,258,259,259,259,260,260,260,260,260,260,260,260,260,260,260,
261,261,261,261,261,261,261,262,263,263,264,264,265,265,266,266,267,267,268,268,
268,269,269,269,269,269,270,270,270,271,271,271,272,272,272,272,273,273,274,274,
274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,274,275,275,275,
276,276,276,276,276,276,276,276,276,276,277,277,277,277,277,277,277,277,277,278,
279,279,280,280,280,280,281,281,282,282,283,283,284,284,285,285};static short Cyc_yyr2[
510]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,3,5,3,7,5,8,0,
1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,
1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,
2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,
2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,
0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,
1,1,2,3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,
1,1,1,1,5,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,
6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,
3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,
3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,
3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,4,6,9,11,4,6,6,4,1,1,1,1,4,3,
4,3,3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short
Cyc_yydefact[1027]={0,18,50,51,52,53,55,70,73,74,75,76,77,78,79,80,94,95,96,112,
113,46,0,0,56,0,0,145,87,91,0,0,0,0,0,0,0,37,500,211,502,501,503,0,0,71,0,197,
197,196,1,0,16,0,0,17,0,0,40,48,42,68,44,81,82,0,83,0,36,156,0,181,184,84,160,
110,54,53,47,0,98,499,0,500,496,497,498,110,0,371,0,0,0,0,234,0,373,374,25,27,0,
0,0,0,0,0,0,0,146,0,0,0,0,0,0,0,194,195,2,0,0,0,0,0,29,0,118,119,121,41,49,43,45,
114,504,505,110,110,36,38,36,0,19,0,213,0,169,157,182,191,190,0,0,188,189,192,
193,202,184,0,69,0,54,102,0,100,0,500,381,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,486,
0,0,0,0,0,473,471,472,0,398,400,414,422,424,426,428,430,432,435,440,443,446,450,
0,452,474,485,484,0,382,380,32,0,0,110,0,0,0,128,124,126,252,254,0,0,9,10,0,0,0,
110,506,507,212,93,0,0,161,72,232,0,229,0,3,0,5,0,282,0,30,0,0,36,20,0,115,116,0,
109,0,143,0,0,0,0,0,0,0,0,0,0,0,0,500,285,287,0,293,289,0,291,275,276,277,0,278,
279,280,0,39,21,121,259,0,219,235,0,0,215,213,0,199,0,0,0,204,57,203,185,0,0,103,
99,0,0,375,0,0,395,0,0,0,0,248,391,396,0,393,0,460,0,416,450,0,417,418,0,0,0,0,0,
0,0,0,0,453,454,36,0,0,0,456,457,455,0,372,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,404,405,406,407,408,409,410,411,412,413,403,0,458,0,480,481,0,0,0,488,0,110,
388,389,0,386,0,236,0,0,0,0,255,227,0,129,134,130,132,125,127,213,0,261,253,262,
509,508,0,86,90,0,88,0,92,108,63,62,0,60,162,213,231,158,261,233,170,171,0,85,
198,26,0,28,0,0,0,0,120,122,238,237,22,105,117,0,0,0,136,137,139,0,110,110,151,0,
0,0,0,0,0,36,0,322,323,324,0,0,326,0,0,0,0,294,290,121,292,288,286,0,168,220,0,0,
0,226,214,221,165,0,0,0,215,167,201,200,163,199,0,0,205,58,111,0,104,421,101,97,
384,385,251,0,392,247,249,379,0,0,0,0,239,243,0,0,0,0,0,0,0,0,0,0,0,0,0,487,494,
0,493,399,423,0,425,427,429,431,433,434,438,439,436,437,441,442,444,445,447,448,
449,402,401,479,476,0,478,0,0,0,376,0,383,31,0,377,0,0,256,135,131,133,0,215,0,
199,0,263,0,213,0,274,258,0,0,110,0,0,0,128,0,110,0,213,0,180,159,230,4,6,0,34,0,
0,140,123,0,0,215,144,153,152,0,0,147,0,0,0,301,0,0,0,0,0,321,325,0,0,0,0,284,36,
23,260,213,0,223,0,0,216,0,166,219,207,164,187,186,205,183,0,7,0,250,394,397,459,
0,0,240,0,244,463,0,0,0,0,467,470,0,0,0,0,0,451,490,0,0,477,475,0,0,387,390,378,
257,228,270,0,264,265,199,0,0,215,199,0,89,0,215,0,0,0,59,61,0,172,0,0,215,0,199,
0,0,0,138,141,114,142,154,151,151,0,0,0,0,0,0,0,0,0,0,0,0,0,301,327,0,0,24,215,0,
224,222,0,215,0,206,0,8,0,0,0,241,245,0,0,0,0,419,420,479,478,492,0,495,415,489,
491,0,269,267,273,272,0,266,199,0,114,0,66,64,65,173,179,176,0,178,174,199,0,33,
0,0,150,149,295,301,0,0,0,0,0,0,329,330,332,334,336,338,340,342,345,350,353,356,
360,362,369,370,0,36,298,307,0,0,0,0,0,0,0,0,0,0,328,281,209,225,217,208,213,13,
0,0,462,461,0,246,464,0,0,469,468,482,0,271,268,35,0,0,177,175,283,107,0,0,0,301,
0,366,0,0,363,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,364,302,0,309,0,0,0,317,
0,0,0,0,0,0,300,0,215,12,14,0,0,0,0,483,106,0,155,296,297,0,0,0,0,301,303,333,0,
335,337,339,341,343,344,348,349,346,347,351,352,354,355,357,358,359,0,308,310,
311,0,319,318,0,313,0,0,0,218,210,15,0,0,0,0,299,365,0,361,304,0,36,312,320,314,
315,0,242,465,0,67,0,0,331,301,305,316,0,368,367,306,466,0,0,0};static short Cyc_yydefgoto[
143]={1024,50,682,872,51,52,284,53,455,54,457,55,56,140,57,58,523,225,441,442,
226,61,240,227,63,163,164,64,160,65,261,262,125,126,127,263,228,422,470,471,472,
66,67,643,644,645,68,473,69,446,70,71,157,158,72,116,519,313,679,603,73,604,513,
670,505,509,510,417,306,248,93,94,544,463,545,328,329,330,229,299,300,605,428,
287,459,288,289,290,291,292,757,293,294,833,834,835,836,837,838,839,840,841,842,
843,844,845,846,847,848,331,407,408,332,333,334,295,196,394,197,529,198,199,200,
201,202,203,204,205,206,207,208,209,210,211,212,560,561,213,214,75,873,241,432};
static short Cyc_yypact[1027]={1197,- -32768,- -32768,- -32768,- -32768,29,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,3236,215,3637,- -32768,154,62,- -32768,137,153,166,
175,172,212,223,15,306,- -32768,- -32768,276,- -32768,- -32768,- -32768,266,889,305,
294,318,318,- -32768,- -32768,2625,- -32768,493,597,- -32768,391,731,3236,3236,3236,
- -32768,3236,- -32768,- -32768,643,- -32768,154,3157,241,131,737,670,- -32768,-
-32768,327,371,387,- -32768,154,398,- -32768,4675,272,- -32768,- -32768,- -32768,327,
5300,- -32768,394,413,4675,421,437,440,- -32768,193,- -32768,- -32768,3309,3309,2625,
2625,3309,475,5300,747,- -32768,280,448,267,468,280,3528,5300,- -32768,- -32768,-
-32768,2625,2758,2625,2758,50,- -32768,486,506,- -32768,3078,- -32768,- -32768,-
-32768,- -32768,3528,- -32768,- -32768,327,191,1658,- -32768,3157,731,- -32768,3309,
1506,4036,- -32768,241,- -32768,- -32768,- -32768,525,532,- -32768,- -32768,- -32768,-
-32768,113,670,3309,- -32768,2758,- -32768,520,543,549,154,114,- -32768,4675,130,
1235,5476,571,5300,5341,578,601,603,619,631,649,652,660,5567,5567,- -32768,2213,
662,5608,5608,5608,- -32768,- -32768,- -32768,72,- -32768,- -32768,- 35,594,659,674,
661,629,140,678,81,155,- -32768,1075,5608,194,40,- -32768,697,874,711,- -32768,-
-32768,720,5300,327,874,713,226,3528,3601,3528,702,- -32768,- 68,- 68,- -32768,-
-32768,95,717,124,327,- -32768,- -32768,- -32768,- -32768,58,718,- -32768,- -32768,664,
319,- -32768,739,- -32768,772,- -32768,783,735,754,- -32768,267,4077,3157,- -32768,
795,3528,- -32768,598,805,154,806,803,99,804,3669,811,823,819,821,4172,3669,261,
815,812,- -32768,- -32768,820,1798,1798,731,1798,- -32768,- -32768,- -32768,826,-
-32768,- -32768,- -32768,20,- -32768,- -32768,825,813,32,840,- -32768,24,824,827,299,
829,716,830,3309,5300,- -32768,841,- -32768,- -32768,32,837,154,- -32768,5300,843,-
-32768,845,846,- -32768,280,5300,4675,240,- -32768,- -32768,- -32768,851,852,2213,-
-32768,3528,- -32768,- -32768,3764,- -32768,870,5300,5300,5300,5300,5300,5300,3528,
491,2213,- -32768,- -32768,1938,839,337,5300,- -32768,- -32768,- -32768,5300,- -32768,
5608,5300,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,
5608,5608,5608,5300,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,5300,- -32768,280,- -32768,- -32768,4213,280,5300,-
-32768,854,327,- -32768,- -32768,848,860,4675,- -32768,389,1235,858,3309,- -32768,
866,871,- -32768,3601,3601,3601,- -32768,- -32768,2918,4305,285,- -32768,419,- -32768,
- -32768,24,- -32768,- -32768,3309,- -32768,864,- -32768,873,- -32768,865,877,875,-
-32768,2837,- -32768,574,699,- -32768,- -32768,- -32768,3528,- -32768,- -32768,- -32768,
2625,- -32768,2625,892,891,888,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,676,5300,897,895,- -32768,890,531,327,327,844,5300,5300,896,898,5300,988,
2078,907,- -32768,- -32768,- -32768,428,982,- -32768,4346,5300,3669,2348,- -32768,-
-32768,3078,- -32768,- -32768,- -32768,3309,- -32768,- -32768,3528,904,293,- -32768,-
-32768,894,- -32768,24,906,3455,827,- -32768,- -32768,- -32768,- -32768,716,89,908,
784,- -32768,- -32768,2491,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
901,- -32768,- -32768,- -32768,- -32768,5725,909,915,929,- -32768,- -32768,705,4077,
339,917,919,920,361,363,916,537,918,923,5435,- -32768,- -32768,922,924,- -32768,594,
177,659,674,661,629,140,140,678,678,678,678,81,81,155,155,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,927,- -32768,149,3309,3941,- -32768,5390,-
-32768,- -32768,926,- -32768,115,941,- -32768,- -32768,- -32768,- -32768,931,827,936,
716,932,419,3309,3382,4438,- -32768,- -32768,- 68,933,327,769,939,58,2999,940,327,
3309,1506,4479,- -32768,574,- -32768,- -32768,- -32768,942,- -32768,960,818,- -32768,-
-32768,598,5300,827,- -32768,- -32768,- -32768,949,154,487,377,380,5300,849,383,945,
4574,4615,561,- -32768,- -32768,952,954,948,397,- -32768,3157,- -32768,813,957,3309,
- -32768,958,24,- -32768,955,- -32768,202,- -32768,- -32768,- -32768,- -32768,784,-
-32768,961,- -32768,2625,- -32768,- -32768,- -32768,962,364,963,- -32768,3805,- -32768,
- -32768,5300,1056,5300,5341,- -32768,- -32768,280,280,962,959,3941,- -32768,- -32768,
5300,5300,- -32768,- -32768,32,767,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
964,- -32768,- -32768,716,32,965,827,716,956,- -32768,5300,827,399,966,967,- -32768,
- -32768,968,- -32768,32,970,827,989,716,985,5300,969,- -32768,- -32768,3528,- -32768,
993,91,844,3669,1000,994,1023,995,1002,3669,5300,4707,562,4799,583,4891,849,-
-32768,971,1005,- -32768,827,116,- -32768,- -32768,1012,827,3528,- -32768,510,-
-32768,1001,1004,5300,- -32768,- -32768,4077,408,1003,1015,- -32768,870,1011,1014,-
-32768,774,- -32768,- -32768,- -32768,- -32768,3941,- -32768,- -32768,- -32768,- -32768,
1017,- -32768,716,589,3528,975,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,
1018,- -32768,- -32768,716,427,- -32768,1021,3528,- -32768,- -32768,1115,849,1027,
5699,1024,2348,5608,1026,- -32768,77,- -32768,1064,1028,751,807,178,814,197,291,-
-32768,- -32768,- -32768,- -32768,1065,5608,1938,- -32768,- -32768,431,3669,460,4932,
3669,507,5024,5116,590,1049,- -32768,- -32768,- -32768,- -32768,1059,- -32768,957,-
-32768,1061,634,- -32768,- -32768,211,- -32768,- -32768,3528,1155,- -32768,- -32768,-
-32768,3900,- -32768,- -32768,- -32768,1063,1066,- -32768,- -32768,- -32768,- -32768,
516,3669,1068,849,2213,- -32768,3528,1060,- -32768,1370,5608,5300,5608,5608,5608,
5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5608,5300,-
-32768,- -32768,1067,- -32768,3669,3669,523,- -32768,3669,3669,527,3669,533,5208,-
-32768,24,827,- -32768,- -32768,747,5300,1069,1071,- -32768,- -32768,1080,- -32768,-
-32768,- -32768,1070,1072,1073,5608,849,- -32768,594,234,659,674,674,629,140,140,
678,678,678,678,81,81,155,155,- -32768,- -32768,- -32768,239,- -32768,- -32768,-
-32768,3669,- -32768,- -32768,3669,- -32768,3669,3669,540,- -32768,- -32768,- -32768,
791,1095,3528,1096,- -32768,962,366,- -32768,- -32768,5300,1370,- -32768,- -32768,-
-32768,- -32768,3669,- -32768,- -32768,1098,- -32768,1099,1101,- -32768,849,- -32768,-
-32768,1102,- -32768,- -32768,- -32768,- -32768,1189,1190,- -32768};static short Cyc_yypgoto[
143]={- -32768,150,- -32768,248,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- 51,- -32768,- 96,22,- -32768,- -32768,0,608,- -32768,55,- 174,1117,5,- -32768,-
-32768,- 128,- -32768,120,1193,- 707,- -32768,- -32768,- -32768,974,973,542,463,-
-32768,- -32768,595,- -32768,- -32768,167,- -32768,- -32768,48,- 193,1163,- 274,- 61,-
-32768,1081,- -32768,- -32768,1195,- 374,- -32768,560,- 124,- 133,- 143,119,302,575,580,
- 370,- 412,- 108,- 405,- 110,- -32768,- 231,- 171,- 497,- 254,- -32768,921,- 157,- 89,- 112,-
94,- 377,- 167,- -32768,- -32768,- -32768,- 29,- 271,- -32768,- 715,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 4,1025,- -32768,712,847,- -32768,
214,534,- -32768,- 172,- 290,- 144,- 330,- 336,- 341,885,- 320,- 347,- 242,- 334,- 306,- 182,
730,498,910,- -32768,- 305,- -32768,181,372,- 62,9,- 247,49};static short Cyc_yytable[
5863]={59,305,338,135,341,62,247,358,359,360,148,230,231,496,497,234,499,139,429,
95,304,59,569,570,314,566,62,462,395,565,355,342,259,563,307,19,20,534,321,142,
822,575,576,78,111,135,628,316,568,608,863,59,420,445,430,60,62,363,59,59,59,613,
59,62,62,62,431,62,59,146,608,577,578,62,136,439,60,139,168,533,129,130,131,556,
132,506,546,285,217,296,141,416,112,711,586,668,641,642,364,260,665,888,59,59,
483,128,60,62,62,628,490,297,896,60,60,60,237,60,429,59,59,59,59,60,62,62,62,62,
59,571,572,573,574,62,427,39,501,76,59,361,59,507,671,62,402,62,675,146,528,584,
141,502,448,587,430,528,254,60,60,243,286,59,141,515,440,322,62,426,99,904,508,
255,403,627,60,60,60,60,540,635,541,429,953,60,450,447,716,659,958,298,527,361,
553,60,555,60,362,579,580,581,627,118,450,474,96,38,794,171,296,- 148,905,406,41,
676,434,377,60,430,406,378,169,521,479,431,39,480,419,371,372,431,465,421,720,
285,285,170,285,38,312,715,867,323,1001,361,41,420,420,420,437,431,431,621,232,
233,44,324,265,267,100,59,38,373,374,96,62,912,913,41,361,250,251,252,253,96,101,
621,396,397,398,709,433,141,102,435,59,59,379,59,1017,62,62,103,62,104,380,381,
361,528,508,914,915,1022,195,285,707,298,286,286,504,286,221,317,159,524,60,691,
222,399,303,266,236,38,400,401,535,597,- 213,661,41,- 213,361,918,79,105,498,919,
546,39,944,60,60,412,60,754,415,614,106,801,746,503,96,805,96,361,59,108,43,438,
361,62,536,1002,143,426,144,525,1003,818,326,327,145,870,74,38,416,704,286,491,
169,508,41,109,622,447,492,450,238,247,- 235,110,44,- 235,239,170,80,97,96,98,47,
48,49,356,662,96,39,593,450,474,424,60,113,667,450,664,425,303,74,114,514,419,
419,419,920,74,421,421,421,735,612,74,921,922,886,653,411,115,785,134,451,137,
781,735,74,452,620,891,626,139,159,546,123,165,791,792,97,59,361,59,361,784,62,
558,62,692,97,416,528,724,634,663,238,780,238,1013,74,74,239,710,239,134,361,739,
361,74,59,696,723,697,161,62,489,74,74,74,74,162,361,722,59,361,738,752,361,62,
753,594,141,758,361,96,60,737,60,74,740,166,361,215,809,216,141,768,770,810,590,
789,59,361,678,81,775,62,878,74,43,796,508,218,165,60,609,97,610,97,656,785,361,
361,611,356,361,892,790,60,877,926,219,547,622,474,220,551,552,965,966,356,962,
963,784,671,961,242,450,474,959,450,474,771,564,361,925,60,971,972,928,826,235,
97,713,964,96,853,244,96,97,639,640,38,84,186,85,86,256,750,41,629,751,630,612,
119,120,296,973,974,351,588,38,188,449,699,397,398,40,41,42,257,361,626,871,74,
785,932,957,769,674,451,449,38,476,318,950,481,361,40,41,42,361,982,310,249,902,
985,361,784,468,311,246,987,74,361,399,319,59,727,1008,700,401,62,924,320,967,
968,969,970,901,264,681,764,857,309,361,361,59,678,141,365,246,62,927,43,165,931,
646,647,337,623,650,624,654,860,97,343,361,625,38,887,937,660,361,361,40,41,42,
121,122,894,135,60,369,370,96,468,945,718,344,469,345,940,951,47,48,49,1016,149,
150,151,729,60,975,976,977,954,346,955,152,153,154,155,156,38,942,849,1014,943,
347,40,41,42,747,797,133,980,981,375,376,983,984,418,986,423,38,802,348,96,1000,
349,40,41,42,517,518,97,38,350,97,357,814,444,40,41,42,43,464,425,366,47,48,49,
368,468,285,16,17,18,264,38,508,47,48,49,367,40,41,42,404,1004,449,409,1005,449,
1006,1007,444,689,690,424,410,74,425,74,1015,425,779,47,48,49,414,38,1011,436,
449,1018,443,804,41,522,449,124,808,641,642,59,285,38,44,458,62,453,816,40,41,42,
47,48,49,460,761,763,47,48,49,755,756,286,464,730,731,732,548,549,550,135,600,
601,602,798,799,908,909,866,454,559,883,884,869,562,910,911,74,916,917,456,336,
59,339,339,60,786,62,1009,361,97,81,466,352,353,582,824,825,339,339,339,475,477,
38,286,478,482,583,485,40,41,42,559,484,486,96,487,493,494,339,468,807,495,500,
258,504,431,511,47,48,49,285,516,512,526,43,820,60,607,520,530,363,557,97,82,246,
531,532,538,615,539,591,854,856,589,859,405,862,592,167,84,596,85,86,598,246,41,
617,599,616,87,449,246,619,38,876,88,631,89,90,618,41,59,43,91,449,632,62,449,
633,92,44,636,637,649,749,638,651,648,47,48,49,655,657,286,559,666,669,559,672,
683,677,686,687,688,693,246,694,695,698,702,701,706,714,195,705,246,829,830,339,
708,39,728,74,717,246,339,60,991,719,81,721,733,736,743,744,748,759,765,766,930,
767,303,934,936,772,774,778,703,464,787,889,793,782,821,806,864,800,803,811,812,
813,339,815,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,
356,817,82,819,823,827,828,960,852,865,851,868,464,874,879,97,875,167,84,382,85,
86,880,881,41,978,882,893,87,885,890,895,897,726,831,900,89,90,134,989,246,903,
91,906,923,832,993,742,92,192,418,907,193,194,246,938,383,384,385,386,387,388,
389,390,391,392,939,941,946,948,559,979,956,949,952,996,997,1025,1026,992,999,
994,995,393,998,- 11,1,339,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,1010,
1012,21,1019,1020,464,1021,1023,734,107,788,245,461,745,147,22,467,23,464,777,
315,795,990,24,117,25,26,27,413,776,773,537,685,567,850,28,29,- 36,0,0,595,554,0,
0,30,31,32,33,0,0,34,0,0,0,81,0,0,35,36,37,0,0,0,0,0,0,0,0,339,0,264,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,42,43,0,0,134,0,0,246,464,44,0,45,46,0,0,82,0,
47,48,49,0,0,464,0,0,0,0,325,0,0,167,84,0,85,86,0,0,41,0,264,0,87,0,0,0,0,0,88,0,
89,90,0,326,327,246,91,0,339,0,0,0,92,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,755,756,21,172,173,268,0,269,270,271,272,273,274,275,276,22,81,23,174,
277,0,0,175,24,278,0,464,27,0,0,176,177,178,179,339,28,29,0,180,0,279,0,0,0,0,
339,32,33,181,182,34,183,0,0,0,0,0,35,36,37,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,280,84,186,85,86,39,40,41,42,43,281,138,- 301,0,0,0,0,187,0,45,
283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,27,0,0,0,899,0,0,
339,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,339,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,301,0,0,302,0,0,0,0,39,40,0,42,43,303,0,0,0,0,0,0,0,0,45,
46,0,339,0,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,339,0,
0,0,0,0,0,0,0,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,
268,339,269,270,271,272,273,274,275,276,22,81,23,174,277,0,0,175,24,278,0,0,27,0,
0,176,177,178,179,0,28,29,0,180,0,279,0,0,0,0,0,32,33,181,182,34,183,339,0,0,0,0,
35,36,37,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,84,186,85,86,39,
40,41,42,43,281,138,282,0,0,0,0,187,0,45,283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,
193,194,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,268,0,
269,270,271,272,273,274,275,276,22,81,23,174,277,0,0,175,24,278,0,0,27,0,0,176,
177,178,179,0,28,29,- 36,180,0,279,0,0,0,0,0,32,33,181,182,34,183,0,0,0,0,0,35,36,
37,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,84,186,85,86,39,40,41,
42,43,281,138,0,0,0,0,0,187,0,45,283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,
2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,268,0,269,270,
271,272,273,274,275,276,22,81,23,174,277,0,0,175,24,278,0,0,27,0,0,176,177,178,
179,0,28,29,0,180,0,279,0,0,0,0,0,32,33,181,182,34,183,0,0,0,0,0,35,36,37,0,184,
185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,84,186,85,86,39,40,41,42,43,
281,138,0,0,0,0,0,187,0,45,283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,2,3,4,
77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,172,173,0,0,0,0,0,0,0,0,0,0,
22,81,23,174,0,0,0,175,24,0,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,0,0,0,0,0,
0,32,33,181,182,34,183,0,0,0,0,0,35,36,37,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,84,186,85,86,39,40,41,42,43,652,0,0,0,0,0,0,187,0,45,283,0,0,0,
0,189,0,0,190,0,0,191,192,0,0,193,194,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,
0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,0,0,0,0,27,0,0,176,177,178,
179,0,28,29,0,180,0,0,0,0,0,0,0,32,0,181,182,34,183,0,0,0,0,0,35,36,0,0,184,185,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,39,40,41,42,43,0,354,0,
0,0,0,0,187,0,45,283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,0,172,173,0,0,0,0,0,0,0,0,0,0,22,81,0,174,0,0,0,175,
0,0,0,0,27,0,0,176,177,178,179,0,28,29,0,180,0,0,0,0,0,0,0,32,0,181,182,34,183,0,
0,0,0,0,35,36,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,
86,39,40,41,42,43,0,0,0,0,0,0,0,187,0,45,283,0,0,0,0,189,0,0,190,0,0,191,192,0,0,
193,194,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,
0,0,0,0,0,0,0,0,22,0,23,0,0,0,680,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,
0,0,0,30,31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,0,0,0,0,39,40,41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,
49,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,
0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,30,
31,32,33,0,0,34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,38,0,0,0,0,39,40,41,42,43,0,0,- 11,0,0,0,0,44,0,45,46,0,0,0,0,47,48,49,1,0,2,
3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,
0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 36,0,0,0,0,0,0,30,31,32,33,0,0,
34,0,0,0,0,0,0,35,36,37,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,
0,38,0,0,0,0,39,40,41,42,43,22,0,- 11,0,0,0,0,44,0,45,46,0,27,0,0,47,48,49,0,0,28,
29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,0,0,0,0,0,0,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,301,0,0,38,0,0,0,0,39,40,41,42,43,303,0,22,0,0,0,0,
444,0,45,46,0,0,425,27,47,48,49,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,
0,0,0,0,35,36,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,301,0,0,0,0,
0,0,0,39,40,0,42,43,303,0,22,0,0,0,0,424,0,45,46,0,0,425,27,47,48,49,0,0,0,0,28,
29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,0,2,3,4,77,6,7,8,9,10,11,12,
13,14,15,16,17,18,19,20,0,0,21,0,0,38,0,0,0,0,39,40,41,42,43,22,0,23,0,0,0,0,468,
24,45,46,0,27,0,0,47,48,49,0,0,28,29,- 36,0,0,0,0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,
0,35,36,37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,
0,39,40,0,42,43,22,138,23,0,258,0,0,0,24,45,46,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,
0,0,0,0,0,32,33,0,0,34,0,0,0,0,0,0,35,36,37,2,3,4,77,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,39,40,0,42,43,22,138,0,0,0,0,0,0,24,45,46,0,
27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,39,40,0,42,43,0,0,
0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,223,0,0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,
36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,39,40,
0,42,43,0,224,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,0,
0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,301,0,0,0,0,
0,0,22,39,40,0,42,43,303,0,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,
0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,
0,673,0,0,0,0,0,0,22,39,40,0,42,43,0,0,0,0,0,0,27,0,0,45,46,0,0,0,28,29,0,0,0,0,
0,0,0,0,0,32,0,0,0,34,0,0,0,0,0,0,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,39,40,0,42,43,0,0,0,0,0,0,27,0,0,45,46,0,0,0,28,
29,0,0,0,0,0,0,0,0,0,32,0,0,0,34,0,81,0,0,0,0,35,36,0,0,0,0,0,0,0,0,0,0,0,0,172,
173,268,0,269,270,271,272,273,274,275,276,0,81,0,174,277,0,39,175,0,278,43,0,0,0,
0,176,177,178,179,0,45,46,82,180,0,279,0,0,0,0,0,0,0,181,182,0,183,83,84,0,85,86,
0,0,41,0,184,185,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92,280,84,186,85,
86,0,0,41,0,0,281,138,0,0,172,173,0,187,0,0,188,542,0,0,0,189,0,81,190,174,0,191,
192,175,0,193,194,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,
182,0,183,0,0,0,0,0,81,0,174,0,184,185,175,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,
180,0,0,0,0,38,84,186,85,86,181,182,41,183,0,0,340,543,0,0,0,0,187,184,185,188,0,
326,327,0,189,0,0,190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,
0,340,783,0,172,173,0,187,0,0,188,0,326,327,0,189,0,81,190,174,0,191,192,175,0,
193,194,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,183,
0,0,0,0,0,81,0,174,0,184,185,175,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,
0,38,84,186,85,86,181,182,41,183,0,0,340,947,0,0,0,0,187,184,185,188,0,326,327,0,
189,0,0,190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,340,0,0,
172,173,0,187,0,0,188,0,326,327,0,189,0,81,190,174,0,191,192,175,0,193,194,0,0,0,
0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,183,0,0,0,0,0,81,0,
174,0,184,185,175,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,38,84,186,85,
86,181,182,41,183,0,0,0,0,0,0,0,0,187,184,185,188,0,0,0,308,189,0,0,190,0,0,191,
192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,340,0,0,172,173,0,187,0,0,
188,0,0,0,0,189,0,81,190,174,0,191,192,175,0,193,194,0,0,0,0,176,177,178,179,0,0,
0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,183,0,0,0,0,0,81,0,174,0,184,185,175,0,0,
0,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,38,84,186,85,86,181,182,41,183,0,
488,0,0,0,0,0,0,187,184,185,188,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,0,0,
0,0,38,84,186,85,86,0,0,41,0,0,0,172,173,0,0,0,0,187,585,0,188,0,0,0,81,189,174,
0,190,0,175,191,192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,
0,0,181,182,0,183,0,0,0,0,0,81,0,174,0,184,185,175,0,0,0,0,0,0,0,176,177,178,179,
0,0,0,0,180,0,0,0,0,38,84,186,85,86,181,182,41,183,0,0,0,0,0,0,0,0,187,184,185,
188,0,0,0,606,189,0,0,190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,
0,0,0,172,173,0,0,0,0,187,658,0,188,0,0,0,81,189,174,0,190,0,175,191,192,0,0,193,
194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,183,0,0,0,0,0,
81,0,174,0,184,185,175,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,38,84,
186,85,86,181,182,41,183,0,0,0,0,0,0,0,0,187,184,185,188,0,0,0,725,189,0,0,190,0,
0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,0,0,0,172,173,0,187,0,
0,188,0,0,0,741,189,0,81,190,174,0,191,192,175,0,193,194,0,0,0,0,176,177,178,179,
0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,183,0,0,0,0,0,81,0,174,0,184,185,175,
0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,180,0,0,0,0,38,84,186,85,86,181,182,41,183,
0,760,0,0,0,0,0,0,187,184,185,188,0,0,0,0,189,0,0,190,0,0,191,192,81,0,193,194,0,
0,0,0,38,84,186,85,86,0,0,41,0,0,762,172,173,0,0,0,0,187,0,0,188,0,0,0,81,189,
174,0,190,0,175,191,192,0,0,193,194,0,176,177,178,179,0,0,0,82,180,0,0,0,0,0,0,0,
0,0,181,182,0,183,167,84,0,85,86,0,0,41,0,184,185,87,0,0,0,0,0,88,0,89,90,0,0,0,
0,91,0,0,0,0,0,92,38,84,186,85,86,0,0,41,0,0,0,172,173,0,0,0,0,187,855,0,188,0,0,
0,81,189,174,0,190,0,175,191,192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,
0,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,172,173,0,0,0,0,187,858,0,188,0,0,0,
81,189,174,0,190,0,175,191,192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,
0,172,173,0,0,181,182,0,183,0,0,0,0,0,81,0,174,0,184,185,175,0,0,0,0,0,0,0,176,
177,178,179,0,0,0,0,180,0,0,0,0,38,84,186,85,86,181,182,41,183,0,861,0,0,0,0,0,0,
187,184,185,188,0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,
86,0,0,41,0,0,0,172,173,0,0,0,0,187,929,0,188,0,0,0,81,189,174,0,190,0,175,191,
192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,
0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,
86,0,0,41,0,0,0,172,173,0,0,0,0,187,933,0,188,0,0,0,81,189,174,0,190,0,175,191,
192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,
0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,
86,0,0,41,0,0,0,172,173,0,0,0,0,187,935,0,188,0,0,0,81,189,174,0,190,0,175,191,
192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,0,0,0,0,181,182,0,183,0,
0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,
86,0,0,41,0,0,0,172,173,0,0,0,0,187,988,0,188,0,0,0,81,189,174,0,190,0,175,191,
192,0,0,193,194,0,176,177,178,179,0,0,0,0,180,0,0,0,0,0,172,173,0,0,181,182,0,
183,0,0,0,0,0,81,0,0,0,184,185,0,0,0,0,0,0,0,0,176,177,178,179,0,0,0,0,0,0,0,0,0,
38,84,186,85,86,181,182,41,183,0,0,0,0,0,0,0,0,187,184,185,188,0,81,0,0,189,0,0,
190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,340,0,172,173,0,
0,187,0,0,188,0,0,0,0,189,81,0,190,0,0,191,192,0,82,193,194,0,0,0,176,177,178,
179,0,0,712,0,0,167,84,0,85,86,172,173,41,0,181,182,87,183,0,0,0,0,88,81,89,90,0,
184,185,0,91,0,0,0,0,0,92,176,177,178,179,0,0,0,0,0,0,0,0,0,38,84,186,85,86,181,
182,41,183,0,0,703,0,0,0,0,0,187,184,185,188,0,0,0,0,189,0,0,190,0,0,191,192,0,0,
193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,172,173,0,0,0,0,0,335,0,0,188,0,0,81,
0,189,0,0,190,0,0,191,192,0,0,193,194,176,177,178,179,0,0,0,0,0,0,0,0,0,0,172,
173,0,0,181,182,0,183,0,0,0,0,0,81,0,0,0,184,185,0,0,0,0,0,0,0,0,176,177,178,179,
0,0,0,0,0,0,0,0,0,38,84,186,85,86,181,182,41,183,0,0,0,0,0,0,0,0,351,184,185,188,
0,0,0,0,189,0,0,190,0,0,191,192,0,0,193,194,0,0,0,0,38,84,186,85,86,0,0,41,0,0,
172,173,0,0,0,0,0,187,0,0,188,0,0,81,0,189,0,0,190,0,0,191,192,0,0,193,194,176,
177,178,179,0,0,0,0,0,0,0,0,81,0,0,0,0,0,181,182,0,183,0,0,0,0,0,0,0,0,0,184,185,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38,84,186,85,86,0,0,41,0,0,0,82,0,0,0,
0,0,898,0,0,188,0,0,684,0,189,167,84,190,85,86,191,192,41,0,193,194,87,0,0,0,0,0,
88,0,89,90,0,326,327,0,91,0,0,0,0,0,92};static short Cyc_yycheck[5863]={0,144,174,
65,175,0,114,189,190,191,71,100,101,284,285,104,287,68,86,23,144,21,369,370,157,
366,21,258,210,365,187,175,128,363,144,20,21,327,166,68,747,375,376,21,44,107,
451,159,368,426,765,51,226,246,122,0,51,92,58,59,60,431,62,58,59,60,134,62,68,69,
447,377,378,68,65,17,21,128,82,326,58,59,60,354,62,61,340,138,92,140,68,224,44,
590,399,507,5,6,133,128,505,808,102,103,271,57,51,102,103,514,277,140,827,58,59,
60,107,62,86,119,120,121,122,68,119,120,121,122,128,371,372,373,374,128,228,111,
116,108,138,119,140,117,512,138,104,140,520,147,320,396,128,119,246,400,122,327,
106,102,103,111,138,161,140,306,106,169,161,228,106,92,303,121,132,447,119,120,
121,122,335,469,337,86,897,128,246,246,598,492,903,141,318,119,349,138,351,140,
124,379,380,381,474,51,264,264,23,106,703,87,259,118,133,215,113,124,119,134,161,
122,222,138,106,310,123,134,111,126,226,87,88,134,259,226,606,284,285,121,287,
106,125,124,124,111,957,119,113,419,420,421,124,134,134,444,102,103,123,125,136,
137,121,259,106,121,122,82,259,87,88,113,119,119,120,121,122,92,121,468,82,83,84,
130,231,259,116,234,284,285,131,287,1003,284,285,116,287,121,139,140,119,469,431,
121,122,1016,88,354,127,257,284,285,106,287,117,161,121,313,259,546,123,123,116,
128,106,106,128,129,328,414,124,494,113,127,119,134,117,121,286,138,590,111,127,
284,285,221,287,648,118,434,123,721,638,300,169,725,171,119,354,49,115,237,119,
354,120,127,121,424,123,316,127,741,128,129,129,776,0,106,507,557,354,116,106,
512,113,105,444,444,123,447,106,495,116,123,123,119,112,121,22,23,215,25,131,132,
133,187,498,222,111,409,468,468,123,354,105,118,474,502,129,116,44,123,119,419,
420,421,131,51,419,420,421,620,428,57,139,140,806,484,220,117,690,65,119,67,687,
634,70,124,444,819,446,498,121,703,59,79,699,700,82,455,119,457,119,690,455,124,
457,124,92,598,638,610,468,498,106,107,106,107,102,103,112,589,112,107,119,624,
119,111,484,124,610,124,117,484,276,119,120,121,122,108,119,609,498,119,624,124,
119,498,124,116,484,124,119,328,455,623,457,141,624,117,119,123,119,106,498,124,
665,124,404,696,526,119,523,38,673,526,124,161,115,707,669,116,166,484,121,169,
123,171,116,799,119,119,129,335,119,124,696,498,785,124,119,343,620,620,120,347,
348,910,911,351,907,908,799,939,906,123,634,634,904,637,637,666,364,119,851,526,
916,917,124,752,111,215,592,909,409,758,124,412,222,475,476,106,107,108,109,110,
116,116,113,455,119,457,608,116,117,662,918,919,123,401,106,126,246,82,83,84,112,
113,114,119,119,627,118,257,884,124,903,662,515,119,264,106,266,119,124,269,119,
112,113,114,119,124,123,115,832,124,119,884,123,123,114,124,286,119,123,118,662,
614,124,128,129,662,850,120,912,913,914,915,831,133,526,116,116,145,119,119,682,
678,662,91,144,682,855,115,318,858,478,479,123,121,482,123,484,116,328,123,119,
129,106,116,116,493,119,119,112,113,114,116,117,823,778,662,89,90,539,123,879,
604,123,127,123,870,895,131,132,133,1003,63,64,65,616,682,920,921,922,898,123,
900,74,75,76,77,78,106,116,755,999,119,123,112,113,114,639,710,117,928,929,85,86,
932,933,225,935,227,106,722,123,592,956,123,112,113,114,63,64,409,106,123,412,
123,737,123,112,113,114,115,258,129,135,131,132,133,137,123,851,17,18,19,262,106,
939,131,132,133,136,112,113,114,117,982,444,106,985,447,987,988,123,118,119,123,
106,455,129,457,1002,129,682,131,132,133,123,106,995,122,468,1008,124,724,113,
311,474,116,729,5,6,851,903,106,123,120,851,118,739,112,113,114,131,132,133,111,
652,653,131,132,133,22,23,851,340,106,107,108,344,345,346,943,419,420,421,118,
119,136,137,770,118,357,118,119,775,361,89,90,526,85,86,118,172,903,174,175,851,
693,903,118,119,539,38,118,184,185,382,750,751,189,190,191,117,117,106,903,123,
123,394,106,112,113,114,399,123,116,755,116,123,127,210,123,728,123,118,120,106,
134,124,131,132,133,1003,124,127,118,115,743,903,425,130,118,92,124,592,91,424,
122,122,118,106,119,124,759,760,121,762,103,764,119,106,107,124,109,110,119,444,
113,123,118,117,117,620,451,119,106,782,123,106,125,126,124,113,1003,115,131,634,
116,1003,637,122,137,123,116,119,117,644,127,30,123,131,132,133,116,42,1003,492,
123,134,495,124,130,124,124,119,106,119,495,119,119,124,118,124,119,118,831,124,
505,25,26,320,124,111,120,682,124,514,327,1003,940,124,38,130,124,124,123,106,
118,123,117,116,857,124,116,860,861,118,122,117,117,546,25,107,124,121,116,130,
116,124,124,124,124,124,363,124,365,366,367,368,369,370,371,372,373,374,375,376,
377,378,379,380,381,898,124,91,130,123,117,124,905,118,116,127,111,590,124,123,
755,124,106,107,56,109,110,119,124,113,923,124,118,117,124,124,28,117,611,123,
123,125,126,778,937,610,127,131,91,91,134,944,625,137,138,620,135,141,142,624,
118,93,94,95,96,97,98,99,100,101,102,119,118,25,118,648,116,124,119,118,107,118,
0,0,943,119,124,123,120,124,0,1,469,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,124,124,24,124,124,690,124,124,619,35,695,113,257,637,70,37,262,39,703,678,
158,706,939,45,48,47,48,49,222,673,669,329,539,367,755,57,58,59,- 1,- 1,412,350,- 1,
- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,38,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,557,-
1,747,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,
- 1,- 1,943,- 1,- 1,776,785,123,- 1,125,126,- 1,- 1,91,- 1,131,132,133,- 1,- 1,799,- 1,- 1,-
1,- 1,103,- 1,- 1,106,107,- 1,109,110,- 1,- 1,113,- 1,808,- 1,117,- 1,- 1,- 1,- 1,- 1,123,- 1,
125,126,- 1,128,129,823,131,- 1,638,- 1,- 1,- 1,137,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,
- 1,44,45,46,- 1,884,49,- 1,- 1,52,53,54,55,696,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,707,68,
69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,
118,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,
142,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,829,- 1,- 1,832,57,58,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,850,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,- 1,
111,112,- 1,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,- 1,904,- 1,906,907,908,909,
910,911,912,913,914,915,916,917,918,919,920,921,922,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,
5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,956,29,30,31,32,
33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,
60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,1002,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,
111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,
134,- 1,- 1,137,138,- 1,- 1,141,142,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,
- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,
- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,123,-
1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,3,4,5,6,7,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,
38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,
- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,
115,116,117,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,
138,- 1,- 1,141,142,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,
26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,
53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,
80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
106,107,108,109,110,111,112,113,114,115,116,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,
- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,
- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,70,71,
72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,- 1,117,- 1,- 1,- 1,- 1,
- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,68,- 1,70,71,72,73,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,
115,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,
- 1,- 1,141,142,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,
79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
106,- 1,- 1,- 1,- 1,111,112,113,114,115,- 1,- 1,118,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,-
1,- 1,131,132,133,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,
- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,- 1,- 1,118,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,-
1,- 1,- 1,131,132,133,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,
- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,-
1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,37,- 1,118,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,49,
- 1,- 1,131,132,133,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,
- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
103,- 1,- 1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,116,- 1,37,- 1,- 1,- 1,- 1,123,- 1,125,
126,- 1,- 1,129,49,131,132,133,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,
- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,115,116,- 1,37,- 1,- 1,- 1,
- 1,123,- 1,125,126,- 1,- 1,129,49,131,132,133,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,106,- 1,- 1,- 1,- 1,111,112,113,114,115,37,- 1,39,
- 1,- 1,- 1,- 1,123,45,125,126,- 1,49,- 1,- 1,131,132,133,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,3,4,5,6,7,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,115,37,117,39,
- 1,120,- 1,- 1,- 1,45,125,126,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,115,37,117,- 1,- 1,
- 1,- 1,- 1,- 1,45,125,126,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,
21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,111,112,- 1,114,115,- 1,- 1,- 1,-
1,- 1,- 1,49,- 1,- 1,125,126,- 1,- 1,- 1,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,
72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,111,112,- 1,114,115,- 1,117,- 1,- 1,- 1,- 1,49,- 1,-
1,125,126,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,-
1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,-
1,- 1,- 1,- 1,- 1,- 1,37,111,112,- 1,114,115,116,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,125,126,- 1,- 1,
- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,-
1,37,111,112,- 1,114,115,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,125,126,- 1,- 1,- 1,57,58,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,111,112,- 1,
114,115,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,125,126,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,68,- 1,- 1,- 1,72,- 1,38,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,111,44,- 1,46,115,- 1,- 1,- 1,
- 1,52,53,54,55,- 1,125,126,91,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,106,107,-
1,109,110,- 1,- 1,113,- 1,83,84,117,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,-
1,- 1,- 1,- 1,- 1,137,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,117,- 1,- 1,25,26,- 1,123,
- 1,- 1,126,32,- 1,- 1,- 1,131,- 1,38,134,40,- 1,137,138,44,- 1,141,142,- 1,- 1,- 1,- 1,52,
53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,
- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,
106,107,108,109,110,70,71,113,73,- 1,- 1,117,118,- 1,- 1,- 1,- 1,123,83,84,126,- 1,128,
129,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,
- 1,- 1,113,- 1,- 1,- 1,117,118,- 1,25,26,- 1,123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,38,134,
40,- 1,137,138,44,- 1,141,142,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,-
1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,- 1,- 1,
117,118,- 1,- 1,- 1,- 1,123,83,84,126,- 1,128,129,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,-
1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,117,- 1,- 1,25,26,- 1,
123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,38,134,40,- 1,137,138,44,- 1,141,142,- 1,- 1,- 1,-
1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,
- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,
- 1,106,107,108,109,110,70,71,113,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,
- 1,130,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,
- 1,- 1,113,- 1,- 1,- 1,117,- 1,- 1,25,26,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,38,134,40,
- 1,137,138,44,- 1,141,142,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,
25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,- 1,116,-
1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,
142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,
- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,-
1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,
84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,106,107,108,
109,110,70,71,113,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,- 1,130,131,- 1,-
1,134,- 1,- 1,137,138,- 1,- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,-
1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,
- 1,141,142,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,
- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,83,
84,126,- 1,- 1,- 1,130,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,- 1,- 1,- 1,- 1,106,
107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,123,- 1,- 1,126,- 1,- 1,- 1,130,
131,- 1,38,134,40,- 1,137,138,44,- 1,141,142,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,
113,73,- 1,116,- 1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,
137,138,38,- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,116,25,26,
- 1,- 1,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,142,
- 1,52,53,54,55,- 1,- 1,- 1,91,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,106,107,- 1,
109,110,- 1,- 1,113,- 1,83,84,117,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,- 1,- 1,- 1,131,- 1,
- 1,- 1,- 1,- 1,137,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,
- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,-
1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,
84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,- 1,126,- 1,- 1,- 1,38,131,40,-
1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,
25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,83,84,44,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,- 1,116,-
1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,
142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,
- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,-
1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,
84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,- 1,126,- 1,- 1,- 1,38,131,40,-
1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,25,
26,- 1,- 1,- 1,- 1,123,124,- 1,126,- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,
142,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,123,124,- 1,126,
- 1,- 1,- 1,38,131,40,- 1,134,- 1,44,137,138,- 1,- 1,141,142,- 1,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,83,84,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,70,
71,113,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,38,- 1,- 1,131,- 1,- 1,134,- 1,- 1,
137,138,- 1,- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,- 1,117,- 1,
25,26,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,38,- 1,134,- 1,- 1,137,138,- 1,91,141,142,
- 1,- 1,- 1,52,53,54,55,- 1,- 1,103,- 1,- 1,106,107,- 1,109,110,25,26,113,- 1,70,71,117,
73,- 1,- 1,- 1,- 1,123,38,125,126,- 1,83,84,- 1,131,- 1,- 1,- 1,- 1,- 1,137,52,53,54,55,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,- 1,- 1,117,- 1,- 1,- 1,- 1,-
1,123,83,84,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,- 1,- 1,- 1,-
1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,126,- 1,- 1,
38,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,25,26,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,70,71,113,73,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,83,84,126,- 1,- 1,- 1,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,
- 1,141,142,- 1,- 1,- 1,- 1,106,107,108,109,110,- 1,- 1,113,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,
123,- 1,- 1,126,- 1,- 1,38,- 1,131,- 1,- 1,134,- 1,- 1,137,138,- 1,- 1,141,142,52,53,54,55,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,- 1,- 1,- 1,70,71,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,
109,110,- 1,- 1,113,- 1,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,126,- 1,- 1,103,- 1,131,106,
107,134,109,110,137,138,113,- 1,141,142,117,- 1,- 1,- 1,- 1,- 1,123,- 1,125,126,- 1,128,
129,- 1,131,- 1,- 1,- 1,- 1,- 1,137};char Cyc_Yystack_overflow[17]="Yystack_overflow";
struct Cyc_Yystack_overflow_struct{char*tag;int f1;};struct Cyc_Yystack_overflow_struct
Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};void Cyc_yyerror(struct
_dyneither_ptr);int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(int)'\000';
union Cyc_YYSTYPE Cyc_yylval={.YYINITIALSVAL={60,0}};static int Cyc_yynerrs=0;struct
_tuple24{struct Cyc_Position_Segment*f1;struct _tuple1*f2;int f3;};struct _tuple25{
struct _dyneither_ptr f1;void*f2;};static char _tmp519[8]="stdcall";static char
_tmp51A[6]="cdecl";static char _tmp51B[9]="fastcall";static char _tmp51C[9]="noreturn";
static char _tmp51D[6]="const";static char _tmp51E[8]="aligned";static char _tmp51F[7]="packed";
static char _tmp520[7]="shared";static char _tmp521[7]="unused";static char _tmp522[5]="weak";
static char _tmp523[10]="dllimport";static char _tmp524[10]="dllexport";static char
_tmp525[23]="no_instrument_function";static char _tmp526[12]="constructor";static
char _tmp527[11]="destructor";static char _tmp528[22]="no_check_memory_usage";
static char _tmp529[5]="pure";struct _tuple26{void*f1;void*f2;};struct _tuple27{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*
yylex_buf){struct _RegionHandle _tmp47F=_new_region("yyregion");struct
_RegionHandle*yyregion=& _tmp47F;_push_region(yyregion);{int yystate;int yyn=0;int
yyerrstatus;int yychar1=0;int yyssp_offset;short*yyss=(short*)({unsigned int
_tmp833=10000;short*_tmp834=(short*)_region_malloc(yyregion,_check_times(sizeof(
short),_tmp833));{unsigned int _tmp835=_tmp833;unsigned int i;for(i=0;i < _tmp835;i
++){_tmp834[i]=(short)0;}}_tmp834;});int yyvsp_offset;union Cyc_YYSTYPE*yyvs=(
union Cyc_YYSTYPE*)({unsigned int _tmp830=10000;union Cyc_YYSTYPE*_tmp831=(union Cyc_YYSTYPE*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp830));{
unsigned int _tmp832=_tmp830;unsigned int i;for(i=0;i < _tmp832;i ++){_tmp831[i]=Cyc_yylval;}}
_tmp831;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp82D=10000;struct Cyc_Yyltype*_tmp82E=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp82D));{
unsigned int _tmp82F=_tmp82D;unsigned int i;for(i=0;i < _tmp82F;i ++){_tmp82E[i]=Cyc_yynewloc();}}
_tmp82E;});int yystacksize=10000;union Cyc_YYSTYPE yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp480="parser stack overflow";_tag_dyneither(
_tmp480,sizeof(char),22);}));(int)_throw((void*)& Cyc_Yystack_overflow_val);}}
goto yybackup;yybackup: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1027,
yystate)];if(yyn == - 32768)goto yydefault;if(Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(
yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=0;}else{yychar1=Cyc_yychar > 0
 && Cyc_yychar <= 370?(int)Cyc_yytranslate[_check_known_subscript_notnull(371,Cyc_yychar)]:
286;}yyn +=yychar1;if((yyn < 0  || yyn > 5862) || Cyc_yycheck[
_check_known_subscript_notnull(5863,yyn)]!= yychar1)goto yydefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(5863,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrlab;}if(yyn == 1026){int
_tmp481=0;_npop_handler(0);return _tmp481;}if(Cyc_yychar != 0)Cyc_yychar=- 2;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;if(yyerrstatus
!= 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)Cyc_yydefact[
_check_known_subscript_notnull(1027,yystate)];if(yyn == 0)goto yyerrlab;yyreduce:
yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(510,yyn)];if(yylen > 0)yyval=
yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- yylen)];switch(yyn){
case 1: _LL21B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];Cyc_Parse_parse_result=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);break;
case 2: _LL21C: {struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons
_tmp482;_push_handler(& _tmp482);{int _tmp484=0;if(setjmp(_tmp482.handler))_tmp484=
1;if(!_tmp484){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);;_pop_handler();}else{void*_tmp483=(void*)_exn_thrown;void*
_tmp486=_tmp483;_LL21F: {struct Cyc_Core_Failure_struct*_tmp487=(struct Cyc_Core_Failure_struct*)
_tmp486;if(_tmp487->tag != Cyc_Core_Failure)goto _LL221;}_LL220: x=0;goto _LL21E;
_LL221:;_LL222:(void)_throw(_tmp486);_LL21E:;}};}{struct _handler_cons _tmp488;
_push_handler(& _tmp488);{int _tmp48A=0;if(setjmp(_tmp488.handler))_tmp48A=1;if(!
_tmp48A){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp489=(void*)_exn_thrown;void*_tmp48C=_tmp489;_LL224: {
struct Cyc_Core_Failure_struct*_tmp48D=(struct Cyc_Core_Failure_struct*)_tmp48C;
if(_tmp48D->tag != Cyc_Core_Failure)goto _LL226;}_LL225: y=0;goto _LL223;_LL226:;
_LL227:(void)_throw(_tmp48C);_LL223:;}};}yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));break;}
case 3: _LL21D: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp48E=_cycalloc(sizeof(*
_tmp48E));_tmp48E->hd=({struct Cyc_Absyn_Decl*_tmp48F=_cycalloc(sizeof(*_tmp48F));
_tmp48F->r=(void*)({struct Cyc_Absyn_Using_d_struct*_tmp490=_cycalloc(sizeof(*
_tmp490));_tmp490[0]=({struct Cyc_Absyn_Using_d_struct _tmp491;_tmp491.tag=11;
_tmp491.f1=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp491.f2=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp491;});_tmp490;});_tmp48F->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp48F;});
_tmp48E->tl=0;_tmp48E;}));Cyc_Lex_leave_using();break;case 4: _LL228: yyval=Cyc_YY18(({
struct Cyc_List_List*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->hd=({struct Cyc_Absyn_Decl*
_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->r=(void*)({struct Cyc_Absyn_Using_d_struct*
_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494[0]=({struct Cyc_Absyn_Using_d_struct
_tmp495;_tmp495.tag=11;_tmp495.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp495.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp495;});_tmp494;});
_tmp493->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp493;});_tmp492->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp492;}));break;case 5:
_LL229: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp496=_cycalloc(sizeof(*_tmp496));
_tmp496->hd=({struct Cyc_Absyn_Decl*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->r=(
void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp498=_cycalloc(sizeof(*_tmp498));
_tmp498[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp499;_tmp499.tag=10;_tmp499.f1=({
struct _dyneither_ptr*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp49A;});_tmp499.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp499;});
_tmp498;});_tmp497->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp497;});
_tmp496->tl=0;_tmp496;}));Cyc_Lex_leave_namespace();break;case 6: _LL22A: {struct
_dyneither_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp49B;_push_handler(& _tmp49B);{int _tmp49D=0;if(setjmp(_tmp49B.handler))
_tmp49D=1;if(!_tmp49D){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp49C=(void*)_exn_thrown;void*_tmp49F=_tmp49C;_LL22D: {struct Cyc_Core_Failure_struct*
_tmp4A0=(struct Cyc_Core_Failure_struct*)_tmp49F;if(_tmp4A0->tag != Cyc_Core_Failure)
goto _LL22F;}_LL22E: nspace=({const char*_tmp4A1="";_tag_dyneither(_tmp4A1,sizeof(
char),1);});goto _LL22C;_LL22F:;_LL230:(void)_throw(_tmp49F);_LL22C:;}};}{struct
_handler_cons _tmp4A2;_push_handler(& _tmp4A2);{int _tmp4A4=0;if(setjmp(_tmp4A2.handler))
_tmp4A4=1;if(!_tmp4A4){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);;_pop_handler();}else{void*_tmp4A3=(void*)_exn_thrown;
void*_tmp4A6=_tmp4A3;_LL232: {struct Cyc_Core_Failure_struct*_tmp4A7=(struct Cyc_Core_Failure_struct*)
_tmp4A6;if(_tmp4A7->tag != Cyc_Core_Failure)goto _LL234;}_LL233: x=0;goto _LL231;
_LL234:;_LL235:(void)_throw(_tmp4A6);_LL231:;}};}{struct _handler_cons _tmp4A8;
_push_handler(& _tmp4A8);{int _tmp4AA=0;if(setjmp(_tmp4A8.handler))_tmp4AA=1;if(!
_tmp4AA){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;
_pop_handler();}else{void*_tmp4A9=(void*)_exn_thrown;void*_tmp4AC=_tmp4A9;_LL237: {
struct Cyc_Core_Failure_struct*_tmp4AD=(struct Cyc_Core_Failure_struct*)_tmp4AC;
if(_tmp4AD->tag != Cyc_Core_Failure)goto _LL239;}_LL238: y=0;goto _LL236;_LL239:;
_LL23A:(void)_throw(_tmp4AC);_LL236:;}};}yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE->hd=({struct Cyc_Absyn_Decl*_tmp4AF=
_cycalloc(sizeof(*_tmp4AF));_tmp4AF->r=(void*)({struct Cyc_Absyn_Namespace_d_struct*
_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp4B1;_tmp4B1.tag=10;_tmp4B1.f1=({struct _dyneither_ptr*_tmp4B2=_cycalloc(
sizeof(*_tmp4B2));_tmp4B2[0]=nspace;_tmp4B2;});_tmp4B1.f2=x;_tmp4B1;});_tmp4B0;});
_tmp4AF->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp4AF;});_tmp4AE->tl=y;_tmp4AE;}));break;}case 7:
_LL22B: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp4B3="C";
_tag_dyneither(_tmp4B3,sizeof(char),2);}))== 0)yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4->hd=({struct Cyc_Absyn_Decl*_tmp4B5=
_cycalloc(sizeof(*_tmp4B5));_tmp4B5->r=(void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp4B7;_tmp4B7.tag=12;_tmp4B7.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4B7;});_tmp4B6;});
_tmp4B5->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp4B5;});_tmp4B4->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4B4;}));else{if(Cyc_strcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),({const char*_tmp4B8="C include";_tag_dyneither(_tmp4B8,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp4B9="expecting \"C\" or \"C include\"";
_tag_dyneither(_tmp4B9,sizeof(char),29);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=Cyc_YY18(({
struct Cyc_List_List*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA->hd=({struct Cyc_Absyn_Decl*
_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->r=(void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp4BD;_tmp4BD.tag=13;_tmp4BD.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4BD.f2=0;_tmp4BD;});
_tmp4BC;});_tmp4BB->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp4BB;});
_tmp4BA->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4BA;}));}break;case 8: _LL23B: if(Cyc_strcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp4BE="C include";_tag_dyneither(_tmp4BE,
sizeof(char),10);}))!= 0)Cyc_Parse_err(({const char*_tmp4BF="expecting \"C include\"";
_tag_dyneither(_tmp4BF,sizeof(char),22);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->hd=({
struct Cyc_Absyn_Decl*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));_tmp4C1->r=(void*)({
struct Cyc_Absyn_ExternCinclude_d_struct*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));
_tmp4C2[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp4C3;_tmp4C3.tag=13;
_tmp4C3.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp4C3.f2=exs;_tmp4C3;});_tmp4C2;});_tmp4C1->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp4C1;});
_tmp4C0->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4C0;}));break;};case 9: _LL23C: yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp4C4=_cycalloc(sizeof(*_tmp4C4));_tmp4C4->hd=({struct Cyc_Absyn_Decl*_tmp4C5=
_cycalloc(sizeof(*_tmp4C5));_tmp4C5->r=(void*)& Cyc_Absyn_Porton_d_val;_tmp4C5->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp4C5;});_tmp4C4->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C4;}));break;case 10:
_LL23D: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));
_tmp4C6->hd=({struct Cyc_Absyn_Decl*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));_tmp4C7->r=(
void*)& Cyc_Absyn_Portoff_d_val;_tmp4C7->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp4C7;});
_tmp4C6->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4C6;}));break;case 11: _LL23E: yyval=Cyc_YY18(0);break;case 12:
_LL23F: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 13: _LL240: yyval=Cyc_YY52(0);break;case 14: _LL241: yyval=Cyc_YY52(({struct Cyc_List_List*
_tmp4C8=_cycalloc(sizeof(*_tmp4C8));_tmp4C8->hd=({struct _tuple24*_tmp4C9=
_cycalloc(sizeof(*_tmp4C9));_tmp4C9->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp4C9->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp4C9->f3=0;_tmp4C9;});_tmp4C8->tl=0;_tmp4C8;}));break;case 15: _LL242: yyval=Cyc_YY52(({
struct Cyc_List_List*_tmp4CA=_cycalloc(sizeof(*_tmp4CA));_tmp4CA->hd=({struct
_tuple24*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp4CB->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp4CB->f3=0;_tmp4CB;});_tmp4CA->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4CA;}));break;case 16:
_LL243: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));
_tmp4CC->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*_tmp4CD=
_cycalloc(sizeof(*_tmp4CD));_tmp4CD[0]=({struct Cyc_Absyn_Fn_d_struct _tmp4CE;
_tmp4CE.tag=1;_tmp4CE.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4CE;});_tmp4CD;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4CC->tl=0;
_tmp4CC;}));break;case 17: _LL244: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 18: _LL245: yyval=Cyc_YY18(0);break;case 19: _LL246: yyval=
Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 20:
_LL247: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp4CF=
_cycalloc(sizeof(*_tmp4CF));_tmp4CF->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4CF;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 21:
_LL248: yyval=Cyc_YY17(Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 22:
_LL249: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp4D0=
_cycalloc(sizeof(*_tmp4D0));_tmp4D0->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4D0;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 23:
_LL24A: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp4D1=
_cycalloc(sizeof(*_tmp4D1));_tmp4D1->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D1;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 24:
_LL24B: yyval=Cyc_YY17(Cyc_Parse_make_function(({struct Cyc_Core_Opt*_tmp4D2=
_cycalloc(sizeof(*_tmp4D2));_tmp4D2->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4D2;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 25:
_LL24C: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL24D: Cyc_Lex_leave_using();
break;case 27: _LL24E: Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp4D3=
_cycalloc(sizeof(*_tmp4D3));_tmp4D3[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D3;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL24F: Cyc_Lex_leave_namespace();
break;case 29: _LL250: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
30: _LL251: yyval=Cyc_YY18(Cyc_Parse_make_declarations(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 31:
_LL252: yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));
_tmp4D4->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4D4->tl=0;
_tmp4D4;}));break;case 32: _LL253: {struct Cyc_List_List*_tmp4D5=0;{struct Cyc_List_List*
_tmp4D6=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
for(0;_tmp4D6 != 0;_tmp4D6=_tmp4D6->tl){struct _dyneither_ptr*_tmp4D7=(struct
_dyneither_ptr*)_tmp4D6->hd;struct _tuple1*qv=({struct _tuple1*_tmp4DA=_cycalloc(
sizeof(*_tmp4DA));_tmp4DA->f1=Cyc_Absyn_Rel_n(0);_tmp4DA->f2=_tmp4D7;_tmp4DA;});
struct Cyc_Absyn_Vardecl*_tmp4D8=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
_tmp4D5=({struct Cyc_List_List*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->hd=
_tmp4D8;_tmp4D9->tl=_tmp4D5;_tmp4D9;});}}_tmp4D5=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4D5);yyval=Cyc_YY18(({struct Cyc_List_List*
_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB->hd=Cyc_Absyn_letv_decl(_tmp4D5,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4DB->tl=0;
_tmp4DB;}));break;}case 33: _LL254: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp4DC="`H";
_tag_dyneither(_tmp4DC,sizeof(char),3);}))== 0)Cyc_Parse_err((struct
_dyneither_ptr)({void*_tmp4DD=0;Cyc_aprintf(({const char*_tmp4DE="bad occurrence of heap region";
_tag_dyneither(_tmp4DE,sizeof(char),30);}),_tag_dyneither(_tmp4DD,sizeof(void*),
0));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp4DF="`U";
_tag_dyneither(_tmp4DF,sizeof(char),3);}))== 0)Cyc_Parse_err((struct
_dyneither_ptr)({void*_tmp4E0=0;Cyc_aprintf(({const char*_tmp4E1="bad occurrence of unique region";
_tag_dyneither(_tmp4E1,sizeof(char),32);}),_tag_dyneither(_tmp4E0,sizeof(void*),
0));}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->name=({struct _dyneither_ptr*_tmp4EA=
_cycalloc(sizeof(*_tmp4EA));_tmp4EA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4EA;});_tmp4E9->identity=
- 1;_tmp4E9->kind=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_RgnKind);_tmp4E9;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));
_tmp4E7[0]=({struct Cyc_Absyn_VarType_struct _tmp4E8;_tmp4E8.tag=2;_tmp4E8.f1=tv;
_tmp4E8;});_tmp4E7;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->f1=Cyc_Absyn_Loc_n;_tmp4E3->f2=({
struct _dyneither_ptr*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4E4;});_tmp4E3;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp4E5=_cycalloc(sizeof(*_tmp4E5));
_tmp4E5[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp4E6;_tmp4E6.tag=16;_tmp4E6.f1=(
void*)t;_tmp4E6;});_tmp4E5;}),0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4E2=
_cycalloc(sizeof(*_tmp4E2));_tmp4E2->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4E2->tl=0;
_tmp4E2;}));break;};case 34: _LL255: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({const char*_tmp4EB="H";
_tag_dyneither(_tmp4EB,sizeof(char),2);}))== 0)Cyc_Parse_err(({const char*_tmp4EC="bad occurrence of heap region `H";
_tag_dyneither(_tmp4EC,sizeof(char),33);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));if(Cyc_zstrcmp((
struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({const char*_tmp4ED="U";_tag_dyneither(_tmp4ED,sizeof(
char),2);}))== 0)Cyc_Parse_err(({const char*_tmp4EE="bad occurrence of unique region `U";
_tag_dyneither(_tmp4EE,sizeof(char),35);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));if(Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]) && Cyc_yyget_YY4(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)Cyc_Parse_err(({
const char*_tmp4EF="open regions cannot be @resetable";_tag_dyneither(_tmp4EF,
sizeof(char),34);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->name=({
struct _dyneither_ptr*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8[0]=(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp4FB;_tmp4FB.tag=0;_tmp4FB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));({void*_tmp4F9[1]={&
_tmp4FB};Cyc_aprintf(({const char*_tmp4FA="`%s";_tag_dyneither(_tmp4FA,sizeof(
char),4);}),_tag_dyneither(_tmp4F9,sizeof(void*),1));});});_tmp4F8;});_tmp4F7->identity=
- 1;_tmp4F7->kind=Cyc_Tcutil_kind_to_bound(Cyc_Absyn_RgnKind);_tmp4F7;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));
_tmp4F5[0]=({struct Cyc_Absyn_VarType_struct _tmp4F6;_tmp4F6.tag=2;_tmp4F6.f1=tv;
_tmp4F6;});_tmp4F5;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(({struct
_tuple1*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1->f1=Cyc_Absyn_Loc_n;_tmp4F1->f2=({
struct _dyneither_ptr*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4F2;});_tmp4F1;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));
_tmp4F3[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp4F4;_tmp4F4.tag=16;_tmp4F4.f1=(
void*)t;_tmp4F4;});_tmp4F3;}),0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4F0=
_cycalloc(sizeof(*_tmp4F0));_tmp4F0->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F0->tl=0;
_tmp4F0;}));break;};case 35: _LL256: {struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*
_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->name=({struct _dyneither_ptr*_tmp502=
_cycalloc(sizeof(*_tmp502));_tmp502[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp502;});_tmp4FF->identity=
- 1;_tmp4FF->kind=(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp500=_cycalloc_atomic(
sizeof(*_tmp500));_tmp500[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp501;_tmp501.tag=0;
_tmp501.f1=Cyc_Absyn_RgnKind;_tmp501;});_tmp500;});_tmp4FF;});struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));
_tmp4FD->f1=Cyc_Absyn_Loc_n;_tmp4FD->f2=({struct _dyneither_ptr*_tmp4FE=_cycalloc(
sizeof(*_tmp4FE));_tmp4FE[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4FE;});_tmp4FD;}),(
void*)& Cyc_Absyn_VoidType_val,0);yyval=Cyc_YY18(({struct Cyc_List_List*_tmp4FC=
_cycalloc(sizeof(*_tmp4FC));_tmp4FC->hd=Cyc_Absyn_alias_decl(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),tv,vd,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4FC->tl=0;
_tmp4FC;}));break;}case 36: _LL257: yyval=Cyc_YY31(0);break;case 37: _LL258: yyval=Cyc_YY31(
1);break;case 38: _LL259: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 39: _LL25A: yyval=Cyc_YY18(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 40: _LL25B: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp503=_cycalloc(sizeof(*_tmp503));
_tmp503->sc=({struct Cyc_Core_Opt*_tmp504=_cycalloc_atomic(sizeof(*_tmp504));
_tmp504->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp504;});_tmp503->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp503->type_specs=
0;_tmp503->is_inline=0;_tmp503->attributes=0;_tmp503;}));break;case 41: _LL25C: if((
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc != 
0)({void*_tmp505=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp506="Only one storage class is allowed in a declaration";_tag_dyneither(
_tmp506,sizeof(char),51);}),_tag_dyneither(_tmp505,sizeof(void*),0));});yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp507=_cycalloc(sizeof(*_tmp507));
_tmp507->sc=({struct Cyc_Core_Opt*_tmp508=_cycalloc_atomic(sizeof(*_tmp508));
_tmp508->v=(void*)Cyc_yyget_YY22(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp508;});_tmp507->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp507->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp507->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp507->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp507;}));
break;case 42: _LL25D: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp509=
_cycalloc(sizeof(*_tmp509));_tmp509->sc=0;_tmp509->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp509->type_specs=({
struct Cyc_List_List*_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50A->tl=0;_tmp50A;});
_tmp509->is_inline=0;_tmp509->attributes=0;_tmp509;}));break;case 43: _LL25E: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp50B=_cycalloc(sizeof(*_tmp50B));
_tmp50B->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp50B->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp50B->type_specs=({
struct Cyc_List_List*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp50C->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp50C;});
_tmp50B->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp50B->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp50B;}));
break;case 44: _LL25F: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp50D=
_cycalloc(sizeof(*_tmp50D));_tmp50D->sc=0;_tmp50D->tq=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50D->type_specs=0;
_tmp50D->is_inline=0;_tmp50D->attributes=0;_tmp50D;}));break;case 45: _LL260: yyval=
Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp50E=_cycalloc(sizeof(*_tmp50E));
_tmp50E->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp50E->tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp50E->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp50E->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp50E->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp50E;}));
break;case 46: _LL261: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp50F=
_cycalloc(sizeof(*_tmp50F));_tmp50F->sc=0;_tmp50F->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp50F->type_specs=
0;_tmp50F->is_inline=1;_tmp50F->attributes=0;_tmp50F;}));break;case 47: _LL262:
yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp510=_cycalloc(sizeof(*
_tmp510));_tmp510->sc=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc;_tmp510->tq=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp510->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp510->is_inline=1;_tmp510->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp510;}));
break;case 48: _LL263: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp511=
_cycalloc(sizeof(*_tmp511));_tmp511->sc=0;_tmp511->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp511->type_specs=
0;_tmp511->is_inline=0;_tmp511->attributes=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp511;}));break;case 49:
_LL264: yyval=Cyc_YY19(({struct Cyc_Parse_Declaration_spec*_tmp512=_cycalloc(
sizeof(*_tmp512));_tmp512->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp512->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp512->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp512->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp512->attributes=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp512;}));
break;case 50: _LL265: yyval=Cyc_YY22(Cyc_Parse_Auto_sc);break;case 51: _LL266: yyval=
Cyc_YY22(Cyc_Parse_Register_sc);break;case 52: _LL267: yyval=Cyc_YY22(Cyc_Parse_Static_sc);
break;case 53: _LL268: yyval=Cyc_YY22(Cyc_Parse_Extern_sc);break;case 54: _LL269: if(
Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp513="C";
_tag_dyneither(_tmp513,sizeof(char),2);}))!= 0)Cyc_Parse_err(({const char*_tmp514="only extern or extern \"C\" is allowed";
_tag_dyneither(_tmp514,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY22(
Cyc_Parse_ExternC_sc);break;case 55: _LL26A: yyval=Cyc_YY22(Cyc_Parse_Typedef_sc);
break;case 56: _LL26B: yyval=Cyc_YY22(Cyc_Parse_Abstract_sc);break;case 57: _LL26C:
yyval=Cyc_YY45(0);break;case 58: _LL26D: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 59: _LL26E: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;case 60: _LL26F: yyval=
Cyc_YY45(({struct Cyc_List_List*_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515->hd=
Cyc_yyget_YY46(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp515->tl=
0;_tmp515;}));break;case 61: _LL270: yyval=Cyc_YY45(({struct Cyc_List_List*_tmp516=
_cycalloc(sizeof(*_tmp516));_tmp516->hd=Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp516->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp516;}));break;case
62: _LL271: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={6,- 1};static
struct _tuple25 att_map[17]={{{_tmp519,_tmp519,_tmp519 + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{
_tmp51A,_tmp51A,_tmp51A + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp51B,_tmp51B,
_tmp51B + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp51C,_tmp51C,_tmp51C + 9},(
void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp51D,_tmp51D,_tmp51D + 6},(void*)& Cyc_Absyn_Const_att_val},{{
_tmp51E,_tmp51E,_tmp51E + 8},(void*)& att_aligned},{{_tmp51F,_tmp51F,_tmp51F + 7},(
void*)& Cyc_Absyn_Packed_att_val},{{_tmp520,_tmp520,_tmp520 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{
_tmp521,_tmp521,_tmp521 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp522,_tmp522,
_tmp522 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp523,_tmp523,_tmp523 + 10},(void*)&
Cyc_Absyn_Dllimport_att_val},{{_tmp524,_tmp524,_tmp524 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{
_tmp525,_tmp525,_tmp525 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{
_tmp526,_tmp526,_tmp526 + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp527,
_tmp527,_tmp527 + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp528,_tmp528,
_tmp528 + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp529,_tmp529,
_tmp529 + 5},(void*)& Cyc_Absyn_Pure_att_val}};struct _dyneither_ptr _tmp517=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);if((((
_get_dyneither_size(_tmp517,sizeof(char))> 4  && *((const char*)
_check_dyneither_subscript(_tmp517,sizeof(char),0))== '_') && *((const char*)
_check_dyneither_subscript(_tmp517,sizeof(char),1))== '_') && *((const char*)
_check_dyneither_subscript(_tmp517,sizeof(char),(int)(_get_dyneither_size(
_tmp517,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(
_tmp517,sizeof(char),(int)(_get_dyneither_size(_tmp517,sizeof(char))- 3)))== '_')
_tmp517=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp517,2,
_get_dyneither_size(_tmp517,sizeof(char))- 5);{int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((
struct _dyneither_ptr)_tmp517,(struct _dyneither_ptr)(att_map[i]).f1)== 0){yyval=
Cyc_YY46((att_map[i]).f2);break;}}if(i == 17){Cyc_Parse_err(({const char*_tmp518="unrecognized attribute";
_tag_dyneither(_tmp518,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY46((
void*)& Cyc_Absyn_Cdecl_att_val);}break;};}case 63: _LL272: yyval=Cyc_YY46((void*)&
Cyc_Absyn_Const_att_val);break;case 64: _LL273: {struct _dyneither_ptr _tmp52B=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp52D;struct
_tuple6 _tmp52C=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp52D=_tmp52C.f2;{void*a;if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp52B,({const char*_tmp52E="regparm";_tag_dyneither(_tmp52E,
sizeof(char),8);}))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,({const char*
_tmp52F="__regparm__";_tag_dyneither(_tmp52F,sizeof(char),12);}))== 0){if(
_tmp52D < 0  || _tmp52D > 3)Cyc_Parse_err(({const char*_tmp530="regparm requires value between 0 and 3";
_tag_dyneither(_tmp530,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp531=_cycalloc_atomic(sizeof(*_tmp531));
_tmp531[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp532;_tmp532.tag=0;_tmp532.f1=
_tmp52D;_tmp532;});_tmp531;});}else{if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,({
const char*_tmp533="aligned";_tag_dyneither(_tmp533,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,({const char*_tmp534="__aligned__";
_tag_dyneither(_tmp534,sizeof(char),12);}))== 0){if(_tmp52D < 0)Cyc_Parse_err(({
const char*_tmp535="aligned requires positive power of two";_tag_dyneither(
_tmp535,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp52D;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp536="aligned requires positive power of two";
_tag_dyneither(_tmp536,sizeof(char),39);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp537=_cycalloc_atomic(sizeof(*_tmp537));
_tmp537[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp538;_tmp538.tag=6;_tmp538.f1=
_tmp52D;_tmp538;});_tmp537;});};}else{if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp52B,({const char*_tmp539="initializes";_tag_dyneither(_tmp539,sizeof(char),12);}))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52B,({const char*_tmp53A="__initializes__";
_tag_dyneither(_tmp53A,sizeof(char),16);}))== 0)a=(void*)({struct Cyc_Absyn_Initializes_att_struct*
_tmp53B=_cycalloc_atomic(sizeof(*_tmp53B));_tmp53B[0]=({struct Cyc_Absyn_Initializes_att_struct
_tmp53C;_tmp53C.tag=20;_tmp53C.f1=_tmp52D;_tmp53C;});_tmp53B;});else{Cyc_Parse_err(({
const char*_tmp53D="unrecognized attribute";_tag_dyneither(_tmp53D,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}yyval=Cyc_YY46(
a);break;};}case 65: _LL274: {struct _dyneither_ptr _tmp53E=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct
_dyneither_ptr _tmp53F=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53E,({
const char*_tmp540="section";_tag_dyneither(_tmp540,sizeof(char),8);}))== 0  || 
Cyc_zstrcmp((struct _dyneither_ptr)_tmp53E,({const char*_tmp541="__section__";
_tag_dyneither(_tmp541,sizeof(char),12);}))== 0)a=(void*)({struct Cyc_Absyn_Section_att_struct*
_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542[0]=({struct Cyc_Absyn_Section_att_struct
_tmp543;_tmp543.tag=8;_tmp543.f1=_tmp53F;_tmp543;});_tmp542;});else{Cyc_Parse_err(({
const char*_tmp544="unrecognized attribute";_tag_dyneither(_tmp544,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));a=(void*)& Cyc_Absyn_Cdecl_att_val;}yyval=Cyc_YY46(
a);break;}case 66: _LL275: {struct _dyneither_ptr _tmp545=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct _dyneither_ptr
_tmp546=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp545,({const
char*_tmp547="__mode__";_tag_dyneither(_tmp547,sizeof(char),9);}))== 0)a=(void*)({
struct Cyc_Absyn_Mode_att_struct*_tmp548=_cycalloc(sizeof(*_tmp548));_tmp548[0]=({
struct Cyc_Absyn_Mode_att_struct _tmp549;_tmp549.tag=22;_tmp549.f1=_tmp546;_tmp549;});
_tmp548;});else{Cyc_Parse_err(({const char*_tmp54A="unrecognized attribute";
_tag_dyneither(_tmp54A,sizeof(char),23);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)&
Cyc_Absyn_Cdecl_att_val;}yyval=Cyc_YY46(a);break;}case 67: _LL276: {struct
_dyneither_ptr _tmp54B=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dyneither_ptr _tmp54C=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp54E;struct _tuple6
_tmp54D=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]);_tmp54E=_tmp54D.f2;{int _tmp550;struct _tuple6 _tmp54F=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp550=_tmp54F.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;if(Cyc_zstrcmp((struct _dyneither_ptr)
_tmp54B,({const char*_tmp551="format";_tag_dyneither(_tmp551,sizeof(char),7);}))
== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp54B,({const char*_tmp552="__format__";
_tag_dyneither(_tmp552,sizeof(char),11);}))== 0){if(Cyc_zstrcmp((struct
_dyneither_ptr)_tmp54C,({const char*_tmp553="printf";_tag_dyneither(_tmp553,
sizeof(char),7);}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*_tmp554=
_cycalloc_atomic(sizeof(*_tmp554));_tmp554[0]=({struct Cyc_Absyn_Format_att_struct
_tmp555;_tmp555.tag=19;_tmp555.f1=Cyc_Absyn_Printf_ft;_tmp555.f2=_tmp54E;_tmp555.f3=
_tmp550;_tmp555;});_tmp554;});else{if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp54C,({
const char*_tmp556="scanf";_tag_dyneither(_tmp556,sizeof(char),6);}))== 0)a=(void*)({
struct Cyc_Absyn_Format_att_struct*_tmp557=_cycalloc_atomic(sizeof(*_tmp557));
_tmp557[0]=({struct Cyc_Absyn_Format_att_struct _tmp558;_tmp558.tag=19;_tmp558.f1=
Cyc_Absyn_Scanf_ft;_tmp558.f2=_tmp54E;_tmp558.f3=_tmp550;_tmp558;});_tmp557;});
else{Cyc_Parse_err(({const char*_tmp559="unrecognized format type";_tag_dyneither(
_tmp559,sizeof(char),25);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp55A="unrecognized attribute";_tag_dyneither(_tmp55A,sizeof(char),
23);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 7)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}yyval=Cyc_YY46(a);break;};};}case 68: _LL277: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 69: _LL278:
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp55C;_tmp55C.tag=18;_tmp55C.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp55C.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp55C.f3=0;_tmp55C.f4=
0;_tmp55C;});_tmp55B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 70:
_LL279: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 71:
_LL27A: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 72:
_LL27B: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 73:
_LL27C: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 74:
_LL27D: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Short_spec_struct*_tmp55D=
_cycalloc(sizeof(*_tmp55D));_tmp55D[0]=({struct Cyc_Parse_Short_spec_struct
_tmp55E;_tmp55E.tag=2;_tmp55E.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp55E;});
_tmp55D;}));break;case 75: _LL27E: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 76: _LL27F: yyval=Cyc_YY23((void*)({struct
Cyc_Parse_Long_spec_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F[0]=({
struct Cyc_Parse_Long_spec_struct _tmp560;_tmp560.tag=3;_tmp560.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp560;});
_tmp55F;}));break;case 77: _LL280: yyval=Cyc_YY23(Cyc_Parse_type_spec(Cyc_Absyn_float_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 78: _LL281: yyval=Cyc_YY23(Cyc_Parse_type_spec(
Cyc_Absyn_double_typ(0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 79:
_LL282: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Signed_spec_struct*_tmp561=
_cycalloc(sizeof(*_tmp561));_tmp561[0]=({struct Cyc_Parse_Signed_spec_struct
_tmp562;_tmp562.tag=0;_tmp562.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp562;});
_tmp561;}));break;case 80: _LL283: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp564;_tmp564.tag=1;_tmp564.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp564;});
_tmp563;}));break;case 81: _LL284: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 82: _LL285: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 83: _LL286: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 84: _LL287: yyval=
Cyc_YY23(Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 85:
_LL288: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TupleType_struct*
_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565[0]=({struct Cyc_Absyn_TupleType_struct
_tmp566;_tmp566.tag=11;_tmp566.f1=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(
struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp566;});_tmp565;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 86: _LL289: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp567=_cycalloc(sizeof(*_tmp567));
_tmp567[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp568;_tmp568.tag=16;_tmp568.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp568;});_tmp567;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 87:
_LL28A: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp56A;_tmp56A.tag=16;_tmp56A.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0);_tmp56A;});_tmp569;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 88:
_LL28B: {void*_tmp56B=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);
yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp56D;_tmp56D.tag=17;_tmp56D.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp56D.f2=(void*)
_tmp56B;_tmp56D;});_tmp56C;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 89:
_LL28C: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp56F;_tmp56F.tag=17;_tmp56F.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp56F.f2=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp56F;});_tmp56E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 90: _LL28D: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp570=_cycalloc(sizeof(*_tmp570));
_tmp570[0]=({struct Cyc_Absyn_TagType_struct _tmp571;_tmp571.tag=20;_tmp571.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp571;});_tmp570;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 91:
_LL28E: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TagType_struct*
_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572[0]=({struct Cyc_Absyn_TagType_struct
_tmp573;_tmp573.tag=20;_tmp573.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ik,0);_tmp573;});_tmp572;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 92:
_LL28F: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_ValueofType_struct*
_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574[0]=({struct Cyc_Absyn_ValueofType_struct
_tmp575;_tmp575.tag=19;_tmp575.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp575;});_tmp574;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 93: _LL290: yyval=Cyc_YY43(Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 94: _LL291: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;yyval=Cyc_YY25(({
struct Cyc_Absyn_Tqual _tmp576;_tmp576.print_const=1;_tmp576.q_volatile=0;_tmp576.q_restrict=
0;_tmp576.real_const=1;_tmp576.loc=loc;_tmp576;}));break;}case 95: _LL292: yyval=
Cyc_YY25(({struct Cyc_Absyn_Tqual _tmp577;_tmp577.print_const=0;_tmp577.q_volatile=
1;_tmp577.q_restrict=0;_tmp577.real_const=0;_tmp577.loc=0;_tmp577;}));break;case
96: _LL293: yyval=Cyc_YY25(({struct Cyc_Absyn_Tqual _tmp578;_tmp578.print_const=0;
_tmp578.q_volatile=0;_tmp578.q_restrict=1;_tmp578.real_const=0;_tmp578.loc=0;
_tmp578;}));break;case 97: _LL294: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp57A;_tmp57A.tag=5;_tmp57A.f1=({struct Cyc_Absyn_Decl*_tmp57B=_cycalloc(
sizeof(*_tmp57B));_tmp57B->r=(void*)({struct Cyc_Absyn_Enum_d_struct*_tmp57C=
_cycalloc(sizeof(*_tmp57C));_tmp57C[0]=({struct Cyc_Absyn_Enum_d_struct _tmp57D;
_tmp57D.tag=8;_tmp57D.f1=({struct Cyc_Absyn_Enumdecl*_tmp57E=_cycalloc(sizeof(*
_tmp57E));_tmp57E->sc=Cyc_Absyn_Public;_tmp57E->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp57E->fields=({struct
Cyc_Core_Opt*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->v=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp57F;});_tmp57E;});
_tmp57D;});_tmp57C;});_tmp57B->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp57B;});
_tmp57A;});_tmp579;}));break;case 98: _LL295: yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp580=_cycalloc(sizeof(*_tmp580));
_tmp580[0]=({struct Cyc_Absyn_EnumType_struct _tmp581;_tmp581.tag=14;_tmp581.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp581.f2=0;_tmp581;});_tmp580;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 99:
_LL296: yyval=Cyc_YY23((void*)({struct Cyc_Parse_Type_spec_struct*_tmp582=
_cycalloc(sizeof(*_tmp582));_tmp582[0]=({struct Cyc_Parse_Type_spec_struct _tmp583;
_tmp583.tag=4;_tmp583.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp585;_tmp585.tag=15;_tmp585.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp585;});_tmp584;}));
_tmp583.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp583;});_tmp582;}));break;case 100: _LL297: yyval=Cyc_YY47(({
struct Cyc_Absyn_Enumfield*_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp586->tag=0;_tmp586->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp586;}));break;case 101: _LL298: yyval=Cyc_YY47(({
struct Cyc_Absyn_Enumfield*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp587->tag=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp587->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp587;}));
break;case 102: _LL299: yyval=Cyc_YY48(({struct Cyc_List_List*_tmp588=_cycalloc(
sizeof(*_tmp588));_tmp588->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp588->tl=0;_tmp588;}));break;case 103: _LL29A: yyval=Cyc_YY48(({
struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->hd=Cyc_yyget_YY47(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp589->tl=0;
_tmp589;}));break;case 104: _LL29B: yyval=Cyc_YY48(({struct Cyc_List_List*_tmp58A=
_cycalloc(sizeof(*_tmp58A));_tmp58A->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp58A->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp58A;}));break;case
105: _LL29C: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AnonAggrType_struct*
_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B[0]=({struct Cyc_Absyn_AnonAggrType_struct
_tmp58C;_tmp58C.tag=13;_tmp58C.f1=Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp58C.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp58C;});_tmp58B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 106: _LL29D: {struct Cyc_List_List*_tmp58D=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp58E=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp58F=_cycalloc(sizeof(*_tmp58F));
_tmp58F[0]=({struct Cyc_Parse_Decl_spec_struct _tmp590;_tmp590.tag=5;_tmp590.f1=
Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp58D,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp58E,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),1),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp590;});
_tmp58F;}));break;}case 107: _LL29E: {struct Cyc_List_List*_tmp591=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp592=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp594;_tmp594.tag=5;_tmp594.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp591,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp592,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp594;});
_tmp593;}));break;}case 108: _LL29F: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AggrType_struct*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595[0]=({
struct Cyc_Absyn_AggrType_struct _tmp596;_tmp596.tag=12;_tmp596.f1=({struct Cyc_Absyn_AggrInfo
_tmp597;_tmp597.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({struct Cyc_Core_Opt*
_tmp598=_cycalloc_atomic(sizeof(*_tmp598));_tmp598->v=(void*)1;_tmp598;}));
_tmp597.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp597;});_tmp596;});_tmp595;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 109:
_LL2A0: yyval=Cyc_YY23(Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_AggrType_struct*
_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599[0]=({struct Cyc_Absyn_AggrType_struct
_tmp59A;_tmp59A.tag=12;_tmp59A.f1=({struct Cyc_Absyn_AggrInfo _tmp59B;_tmp59B.aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),0);_tmp59B.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59B;});_tmp59A;});
_tmp599;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 110: _LL2A1: yyval=Cyc_YY40(0);break;case
111: _LL2A2: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
break;case 112: _LL2A3: yyval=Cyc_YY24(Cyc_Absyn_StructA);break;case 113: _LL2A4:
yyval=Cyc_YY24(Cyc_Absyn_UnionA);break;case 114: _LL2A5: yyval=Cyc_YY26(0);break;
case 115: _LL2A6: {struct Cyc_List_List*_tmp59C=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_flatten)(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));
struct Cyc_List_List*_tmp59D=Cyc_Parse_get_argrfield_tags(_tmp59C);if(_tmp59D != 0)((
void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,
_tmp59D,_tmp59C);yyval=Cyc_YY26(_tmp59C);break;}case 116: _LL2A7: yyval=Cyc_YY27(({
struct Cyc_List_List*_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E->hd=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp59E->tl=0;_tmp59E;}));
break;case 117: _LL2A8: yyval=Cyc_YY27(({struct Cyc_List_List*_tmp59F=_cycalloc(
sizeof(*_tmp59F));_tmp59F->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp59F->tl=Cyc_yyget_YY27(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp59F;}));break;case
118: _LL2A9: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));
break;case 119: _LL2AA: yyval=Cyc_YY21(({struct Cyc_List_List*_tmp5A0=_cycalloc(
sizeof(*_tmp5A0));_tmp5A0->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp5A0->tl=0;_tmp5A0;}));break;case 120: _LL2AB: yyval=Cyc_YY21(({
struct Cyc_List_List*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A1->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A1;}));break;
case 121: _LL2AC: yyval=Cyc_YY20(({struct _tuple21*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));
_tmp5A2->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5A2->f2=0;_tmp5A2;}));break;case 122: _LL2AD: yyval=Cyc_YY20(({
struct _tuple21*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A3->f2=(struct
Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5A3;}));break;case 123: _LL2AE: {struct _RegionHandle _tmp5A4=
_new_region("temp");struct _RegionHandle*temp=& _tmp5A4;_push_region(temp);{struct
_tuple22 _tmp5A6;struct Cyc_Absyn_Tqual _tmp5A7;struct Cyc_List_List*_tmp5A8;struct
Cyc_List_List*_tmp5A9;struct _tuple22*_tmp5A5=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5A6=*_tmp5A5;_tmp5A7=
_tmp5A6.f1;_tmp5A8=_tmp5A6.f2;_tmp5A9=_tmp5A6.f3;if(_tmp5A7.loc == 0)_tmp5A7.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp5AB;struct Cyc_List_List*
_tmp5AC;struct _tuple0 _tmp5AA=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp5AB=_tmp5AA.f1;
_tmp5AC=_tmp5AA.f2;{void*_tmp5AD=Cyc_Parse_speclist2typ(_tmp5A8,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp5AE=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp5A7,_tmp5AD,_tmp5AB,_tmp5A9),_tmp5AC);yyval=Cyc_YY26(((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,struct _tuple14*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmp5AE));};};}_npop_handler(0);break;;_pop_region(
temp);}case 124: _LL2AF: yyval=Cyc_YY35(({struct _tuple22*_tmp5AF=_cycalloc(sizeof(*
_tmp5AF));_tmp5AF->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp5AF->f2=({
struct Cyc_List_List*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B0->tl=0;_tmp5B0;});
_tmp5AF->f3=0;_tmp5AF;}));break;case 125: _LL2B0: yyval=Cyc_YY35(({struct _tuple22*
_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp5B1->f2=({struct Cyc_List_List*
_tmp5B2=_cycalloc(sizeof(*_tmp5B2));_tmp5B2->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5B2->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5B2;});_tmp5B1->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp5B1;}));break;case 126: _LL2B1: yyval=Cyc_YY35(({struct _tuple22*_tmp5B3=
_cycalloc(sizeof(*_tmp5B3));_tmp5B3->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B3->f2=0;_tmp5B3->f3=0;
_tmp5B3;}));break;case 127: _LL2B2: yyval=Cyc_YY35(({struct _tuple22*_tmp5B4=
_cycalloc(sizeof(*_tmp5B4));_tmp5B4->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp5B4->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5B4->f3=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp5B4;}));break;
case 128: _LL2B3: yyval=Cyc_YY35(({struct _tuple22*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));
_tmp5B5->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp5B5->f2=0;
_tmp5B5->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5B5;}));break;case 129: _LL2B4: yyval=Cyc_YY35(({struct _tuple22*
_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp5B6->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5B6->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp5B6;}));break;case 130: _LL2B5: yyval=Cyc_YY35(({struct _tuple22*_tmp5B7=
_cycalloc(sizeof(*_tmp5B7));_tmp5B7->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp5B7->f2=({
struct Cyc_List_List*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->hd=Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B8->tl=0;_tmp5B8;});
_tmp5B7->f3=0;_tmp5B7;}));break;case 131: _LL2B6: yyval=Cyc_YY35(({struct _tuple22*
_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp5B9->f2=({struct Cyc_List_List*
_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->hd=Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5BA->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5BA;});_tmp5B9->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp5B9;}));break;case 132: _LL2B7: yyval=Cyc_YY35(({struct _tuple22*_tmp5BB=
_cycalloc(sizeof(*_tmp5BB));_tmp5BB->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5BB->f2=0;_tmp5BB->f3=0;
_tmp5BB;}));break;case 133: _LL2B8: yyval=Cyc_YY35(({struct _tuple22*_tmp5BC=
_cycalloc(sizeof(*_tmp5BC));_tmp5BC->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);_tmp5BC->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5BC->f3=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp5BC;}));break;
case 134: _LL2B9: yyval=Cyc_YY35(({struct _tuple22*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));
_tmp5BD->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp5BD->f2=0;
_tmp5BD->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5BD;}));break;case 135: _LL2BA: yyval=Cyc_YY35(({struct _tuple22*
_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp5BE->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp5BE->f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp5BE;}));break;case 136: _LL2BB: yyval=Cyc_YY21(((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 137: _LL2BC:
yyval=Cyc_YY21(({struct Cyc_List_List*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->hd=
Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5BF->tl=
0;_tmp5BF;}));break;case 138: _LL2BD: yyval=Cyc_YY21(({struct Cyc_List_List*_tmp5C0=
_cycalloc(sizeof(*_tmp5C0));_tmp5C0->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C0->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5C0;}));break;
case 139: _LL2BE: yyval=Cyc_YY20(({struct _tuple21*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));
_tmp5C1->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5C1->f2=0;_tmp5C1;}));break;case 140: _LL2BF: yyval=Cyc_YY20(({
struct _tuple21*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->f1=({struct Cyc_Parse_Declarator*
_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->id=({struct _tuple1*_tmp5C4=
_cycalloc(sizeof(*_tmp5C4));_tmp5C4->f1=Cyc_Absyn_Rel_n(0);_tmp5C4->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
_tmp5C4;});_tmp5C3->tms=0;_tmp5C3;});_tmp5C2->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C2;}));break;case
141: _LL2C0: yyval=Cyc_YY20(({struct _tuple21*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));
_tmp5C6->f1=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp5C6->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C6;}));break;case 142:
_LL2C1: {int _tmp5C7=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_List_List*_tmp5C8=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=Cyc_YY23((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));
_tmp5C9[0]=({struct Cyc_Parse_Decl_spec_struct _tmp5CA;_tmp5CA.tag=5;_tmp5CA.f1=
Cyc_Absyn_datatype_decl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp5C8,({struct Cyc_Core_Opt*
_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CB;}),_tmp5C7,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp5CA;});
_tmp5C9;}));break;}case 143: _LL2C2: {int _tmp5CC=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_DatatypeType_struct*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));
_tmp5CD[0]=({struct Cyc_Absyn_DatatypeType_struct _tmp5CE;_tmp5CE.tag=3;_tmp5CE.f1=({
struct Cyc_Absyn_DatatypeInfo _tmp5CF;_tmp5CF.datatype_info=Cyc_Absyn_UnknownDatatype(({
struct Cyc_Absyn_UnknownDatatypeInfo _tmp5D0;_tmp5D0.name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D0.is_extensible=
_tmp5CC;_tmp5D0;}));_tmp5CF.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5CF;});_tmp5CE;});
_tmp5CD;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 144: _LL2C3: {int _tmp5D1=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);yyval=Cyc_YY23(Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_DatatypeFieldType_struct*_tmp5D2=_cycalloc(sizeof(*
_tmp5D2));_tmp5D2[0]=({struct Cyc_Absyn_DatatypeFieldType_struct _tmp5D3;_tmp5D3.tag=
4;_tmp5D3.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp5D4;_tmp5D4.field_info=Cyc_Absyn_UnknownDatatypefield(({
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp5D5;_tmp5D5.datatype_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5D5.field_name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5D5.is_extensible=_tmp5D1;_tmp5D5;}));_tmp5D4.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D4;});_tmp5D3;});
_tmp5D2;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 145: _LL2C4: yyval=Cyc_YY31(0);break;case
146: _LL2C5: yyval=Cyc_YY31(1);break;case 147: _LL2C6: yyval=Cyc_YY34(({struct Cyc_List_List*
_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D6->tl=0;_tmp5D6;}));
break;case 148: _LL2C7: yyval=Cyc_YY34(({struct Cyc_List_List*_tmp5D7=_cycalloc(
sizeof(*_tmp5D7));_tmp5D7->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp5D7->tl=0;_tmp5D7;}));break;case 149: _LL2C8: yyval=
Cyc_YY34(({struct Cyc_List_List*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->hd=
Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5D8->tl=Cyc_yyget_YY34(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5D8;}));break;case 150: _LL2C9: yyval=Cyc_YY34(({struct Cyc_List_List*
_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D9->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D9;}));break;case
151: _LL2CA: yyval=Cyc_YY32(Cyc_Absyn_Public);break;case 152: _LL2CB: yyval=Cyc_YY32(
Cyc_Absyn_Extern);break;case 153: _LL2CC: yyval=Cyc_YY32(Cyc_Absyn_Static);break;
case 154: _LL2CD: yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp5DA=_cycalloc(
sizeof(*_tmp5DA));_tmp5DA->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DA->typs=0;_tmp5DA->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5DA->sc=Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5DA;}));break;case
155: _LL2CE: {struct Cyc_List_List*_tmp5DB=((struct Cyc_List_List*(*)(struct
_tuple16*(*f)(struct Cyc_Position_Segment*,struct _tuple8*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY33(({
struct Cyc_Absyn_Datatypefield*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5DC->typs=_tmp5DB;_tmp5DC->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5DC->sc=Cyc_yyget_YY32(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp5DC;}));break;}
case 156: _LL2CF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 157: _LL2D0: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp5DD=
_cycalloc(sizeof(*_tmp5DD));_tmp5DD->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp5DD->tms=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp5DD;}));
break;case 158: _LL2D1: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 159: _LL2D2: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp5DE->tms=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp5DE;}));
break;case 160: _LL2D3: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp5DF=
_cycalloc(sizeof(*_tmp5DF));_tmp5DF->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5DF->tms=0;_tmp5DF;}));
break;case 161: _LL2D4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 162: _LL2D5: {struct Cyc_Parse_Declarator*_tmp5E0=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E0->tms=({
struct Cyc_List_List*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));
_tmp5E2[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5E3;_tmp5E3.tag=5;_tmp5E3.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp5E3.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5E3;});_tmp5E2;});
_tmp5E1->tl=_tmp5E0->tms;_tmp5E1;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 163: _LL2D6: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5E4->tms=({
struct Cyc_List_List*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5->hd=(void*)({
struct Cyc_Absyn_Carray_mod_struct*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6[0]=({
struct Cyc_Absyn_Carray_mod_struct _tmp5E7;_tmp5E7.tag=0;_tmp5E7.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E7.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5E7;});
_tmp5E6;});_tmp5E5->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp5E5;});_tmp5E4;}));break;case 164: _LL2D7: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5E8->tms=({
struct Cyc_List_List*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->hd=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));
_tmp5EA[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp5EB;_tmp5EB.tag=1;_tmp5EB.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5EB.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp5EB.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5EB;});_tmp5EA;});_tmp5E9->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5E9;});
_tmp5E8;}));break;case 165: _LL2D8: {struct _tuple23 _tmp5ED;struct Cyc_List_List*
_tmp5EE;int _tmp5EF;struct Cyc_Absyn_VarargInfo*_tmp5F0;struct Cyc_Core_Opt*_tmp5F1;
struct Cyc_List_List*_tmp5F2;struct _tuple23*_tmp5EC=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5ED=*_tmp5EC;_tmp5EE=
_tmp5ED.f1;_tmp5EF=_tmp5ED.f2;_tmp5F0=_tmp5ED.f3;_tmp5F1=_tmp5ED.f4;_tmp5F2=
_tmp5ED.f5;yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp5F3=_cycalloc(sizeof(*
_tmp5F3));_tmp5F3->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5F3->tms=({struct Cyc_List_List*_tmp5F4=_cycalloc(
sizeof(*_tmp5F4));_tmp5F4->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp5F6;_tmp5F6.tag=3;_tmp5F6.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp5F8;_tmp5F8.tag=1;_tmp5F8.f1=_tmp5EE;_tmp5F8.f2=_tmp5EF;_tmp5F8.f3=_tmp5F0;
_tmp5F8.f4=_tmp5F1;_tmp5F8.f5=_tmp5F2;_tmp5F8;});_tmp5F7;}));_tmp5F6;});_tmp5F5;});
_tmp5F4->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp5F4;});_tmp5F3;}));break;}case 166: _LL2D9: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp5F9->tms=({
struct Cyc_List_List*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp5FC;_tmp5FC.tag=3;_tmp5FC.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));
_tmp5FD[0]=({struct Cyc_Absyn_WithTypes_struct _tmp5FE;_tmp5FE.tag=1;_tmp5FE.f1=0;
_tmp5FE.f2=0;_tmp5FE.f3=0;_tmp5FE.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5FE.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5FE;});_tmp5FD;}));
_tmp5FC;});_tmp5FB;});_tmp5FA->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp5FA;});_tmp5F9;}));
break;case 167: _LL2DA: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp5FF=
_cycalloc(sizeof(*_tmp5FF));_tmp5FF->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp5FF->tms=({
struct Cyc_List_List*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp602;_tmp602.tag=3;_tmp602.f1=(void*)((
void*)({struct Cyc_Absyn_NoTypes_struct*_tmp603=_cycalloc(sizeof(*_tmp603));
_tmp603[0]=({struct Cyc_Absyn_NoTypes_struct _tmp604;_tmp604.tag=0;_tmp604.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp604.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp604;});
_tmp603;}));_tmp602;});_tmp601;});_tmp600->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp600;});_tmp5FF;}));
break;case 168: _LL2DB: {struct Cyc_List_List*_tmp605=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp606->tms=({
struct Cyc_List_List*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->hd=(void*)({
struct Cyc_Absyn_TypeParams_mod_struct*_tmp608=_cycalloc(sizeof(*_tmp608));
_tmp608[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp609;_tmp609.tag=4;_tmp609.f1=
_tmp605;_tmp609.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp609.f3=0;
_tmp609;});_tmp608;});_tmp607->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp607;});_tmp606;}));
break;}case 169: _LL2DC: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp60A=
_cycalloc(sizeof(*_tmp60A));_tmp60A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp60A->tms=({
struct Cyc_List_List*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));
_tmp60C[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp60D;_tmp60D.tag=5;_tmp60D.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp60D.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60D;});_tmp60C;});
_tmp60B->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp60B;});_tmp60A;}));break;case 170: _LL2DD: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->id=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60E->tms=0;_tmp60E;}));
break;case 171: _LL2DE: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp60F=
_cycalloc(sizeof(*_tmp60F));_tmp60F->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60F->tms=0;_tmp60F;}));
break;case 172: _LL2DF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 173: _LL2E0: {struct Cyc_Parse_Declarator*_tmp610=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp610->tms=({
struct Cyc_List_List*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp612=_cycalloc(sizeof(*_tmp612));
_tmp612[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp613;_tmp613.tag=5;_tmp613.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp613.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp613;});_tmp612;});
_tmp611->tl=_tmp610->tms;_tmp611;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 174: _LL2E1: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*
_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp614->tms=({
struct Cyc_List_List*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->hd=(void*)({
struct Cyc_Absyn_Carray_mod_struct*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616[0]=({
struct Cyc_Absyn_Carray_mod_struct _tmp617;_tmp617.tag=0;_tmp617.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp617.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp617;});
_tmp616;});_tmp615->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp615;});_tmp614;}));break;case 175: _LL2E2: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp618->tms=({
struct Cyc_List_List*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->hd=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));
_tmp61A[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp61B;_tmp61B.tag=1;_tmp61B.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp61B.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp61B.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp61B;});_tmp61A;});_tmp619->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp619;});
_tmp618;}));break;case 176: _LL2E3: {struct _tuple23 _tmp61D;struct Cyc_List_List*
_tmp61E;int _tmp61F;struct Cyc_Absyn_VarargInfo*_tmp620;struct Cyc_Core_Opt*_tmp621;
struct Cyc_List_List*_tmp622;struct _tuple23*_tmp61C=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp61D=*_tmp61C;_tmp61E=
_tmp61D.f1;_tmp61F=_tmp61D.f2;_tmp620=_tmp61D.f3;_tmp621=_tmp61D.f4;_tmp622=
_tmp61D.f5;yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp623=_cycalloc(sizeof(*
_tmp623));_tmp623->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp623->tms=({struct Cyc_List_List*_tmp624=_cycalloc(
sizeof(*_tmp624));_tmp624->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp626;_tmp626.tag=3;_tmp626.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp628;_tmp628.tag=1;_tmp628.f1=_tmp61E;_tmp628.f2=_tmp61F;_tmp628.f3=_tmp620;
_tmp628.f4=_tmp621;_tmp628.f5=_tmp622;_tmp628;});_tmp627;}));_tmp626;});_tmp625;});
_tmp624->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp624;});_tmp623;}));break;}case 177: _LL2E4: yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp629->tms=({
struct Cyc_List_List*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp62C;_tmp62C.tag=3;_tmp62C.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp62D=_cycalloc(sizeof(*_tmp62D));
_tmp62D[0]=({struct Cyc_Absyn_WithTypes_struct _tmp62E;_tmp62E.tag=1;_tmp62E.f1=0;
_tmp62E.f2=0;_tmp62E.f3=0;_tmp62E.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp62E.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp62E;});_tmp62D;}));
_tmp62C;});_tmp62B;});_tmp62A->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp62A;});_tmp629;}));
break;case 178: _LL2E5: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp62F=
_cycalloc(sizeof(*_tmp62F));_tmp62F->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp62F->tms=({
struct Cyc_List_List*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp632;_tmp632.tag=3;_tmp632.f1=(void*)((
void*)({struct Cyc_Absyn_NoTypes_struct*_tmp633=_cycalloc(sizeof(*_tmp633));
_tmp633[0]=({struct Cyc_Absyn_NoTypes_struct _tmp634;_tmp634.tag=0;_tmp634.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp634.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp634;});
_tmp633;}));_tmp632;});_tmp631;});_tmp630->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp630;});_tmp62F;}));
break;case 179: _LL2E6: {struct Cyc_List_List*_tmp635=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY29(({
struct Cyc_Parse_Declarator*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp636->tms=({
struct Cyc_List_List*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->hd=(void*)({
struct Cyc_Absyn_TypeParams_mod_struct*_tmp638=_cycalloc(sizeof(*_tmp638));
_tmp638[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp639;_tmp639.tag=4;_tmp639.f1=
_tmp635;_tmp639.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp639.f3=0;
_tmp639;});_tmp638;});_tmp637->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp637;});_tmp636;}));
break;}case 180: _LL2E7: yyval=Cyc_YY29(({struct Cyc_Parse_Declarator*_tmp63A=
_cycalloc(sizeof(*_tmp63A));_tmp63A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp63A->tms=({
struct Cyc_List_List*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->hd=(void*)({
struct Cyc_Absyn_Attributes_mod_struct*_tmp63C=_cycalloc(sizeof(*_tmp63C));
_tmp63C[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp63D;_tmp63D.tag=5;_tmp63D.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp63D.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp63D;});_tmp63C;});
_tmp63B->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp63B;});_tmp63A;}));break;case 181: _LL2E8: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 182: _LL2E9: yyval=
Cyc_YY28(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])));break;case 183: _LL2EA: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->hd=(void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp640;_tmp640.tag=5;_tmp640.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp640.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp640;});_tmp63F;});_tmp63E->tl=ans;_tmp63E;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;struct Cyc_Position_Segment*_tmp642;union Cyc_Absyn_Constraint*_tmp643;union Cyc_Absyn_Constraint*
_tmp644;struct _tuple18 _tmp641=*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp642=_tmp641.f1;_tmp643=_tmp641.f2;_tmp644=_tmp641.f3;
if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp645=_cycalloc(
sizeof(*_tmp645));_tmp645->ptr_loc=_tmp642;_tmp645->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp645->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp645;});{union Cyc_Absyn_Constraint*_tmp647;
union Cyc_Absyn_Constraint*_tmp648;union Cyc_Absyn_Constraint*_tmp649;void*_tmp64A;
struct _tuple12 _tmp646=Cyc_Parse_collapse_pointer_quals(_tmp642,_tmp643,_tmp644,
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));_tmp647=_tmp646.f1;
_tmp648=_tmp646.f2;_tmp649=_tmp646.f3;_tmp64A=_tmp646.f4;ans=({struct Cyc_List_List*
_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->hd=(void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp64D;_tmp64D.tag=2;_tmp64D.f1=({struct Cyc_Absyn_PtrAtts _tmp64E;_tmp64E.rgn=
_tmp64A;_tmp64E.nullable=_tmp647;_tmp64E.bounds=_tmp648;_tmp64E.zero_term=
_tmp649;_tmp64E.ptrloc=ptrloc;_tmp64E;});_tmp64D.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64D;});_tmp64C;});
_tmp64B->tl=ans;_tmp64B;});yyval=Cyc_YY28(ans);break;};};}case 184: _LL2EB: yyval=
Cyc_YY54(0);break;case 185: _LL2EC: yyval=Cyc_YY54(({struct Cyc_List_List*_tmp64F=
_cycalloc(sizeof(*_tmp64F));_tmp64F->hd=Cyc_yyget_YY53(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp64F->tl=Cyc_yyget_YY54(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64F;}));break;case
186: _LL2ED: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Numelts_ptrqual_struct*
_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650[0]=({struct Cyc_Absyn_Numelts_ptrqual_struct
_tmp651;_tmp651.tag=0;_tmp651.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp651;});_tmp650;}));
break;case 187: _LL2EE: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Region_ptrqual_struct*
_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652[0]=({struct Cyc_Absyn_Region_ptrqual_struct
_tmp653;_tmp653.tag=1;_tmp653.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp653;});_tmp652;}));
break;case 188: _LL2EF: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Thin_ptrqual_struct*
_tmp654=_cycalloc_atomic(sizeof(*_tmp654));_tmp654[0]=({struct Cyc_Absyn_Thin_ptrqual_struct
_tmp655;_tmp655.tag=2;_tmp655;});_tmp654;}));break;case 189: _LL2F0: yyval=Cyc_YY53((
void*)({struct Cyc_Absyn_Fat_ptrqual_struct*_tmp656=_cycalloc_atomic(sizeof(*
_tmp656));_tmp656[0]=({struct Cyc_Absyn_Fat_ptrqual_struct _tmp657;_tmp657.tag=3;
_tmp657;});_tmp656;}));break;case 190: _LL2F1: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Zeroterm_ptrqual_struct*
_tmp658=_cycalloc_atomic(sizeof(*_tmp658));_tmp658[0]=({struct Cyc_Absyn_Zeroterm_ptrqual_struct
_tmp659;_tmp659.tag=4;_tmp659;});_tmp658;}));break;case 191: _LL2F2: yyval=Cyc_YY53((
void*)({struct Cyc_Absyn_Nozeroterm_ptrqual_struct*_tmp65A=_cycalloc_atomic(
sizeof(*_tmp65A));_tmp65A[0]=({struct Cyc_Absyn_Nozeroterm_ptrqual_struct _tmp65B;
_tmp65B.tag=5;_tmp65B;});_tmp65A;}));break;case 192: _LL2F3: yyval=Cyc_YY53((void*)({
struct Cyc_Absyn_Notnull_ptrqual_struct*_tmp65C=_cycalloc_atomic(sizeof(*_tmp65C));
_tmp65C[0]=({struct Cyc_Absyn_Notnull_ptrqual_struct _tmp65D;_tmp65D.tag=6;_tmp65D;});
_tmp65C;}));break;case 193: _LL2F4: yyval=Cyc_YY53((void*)({struct Cyc_Absyn_Nullable_ptrqual_struct*
_tmp65E=_cycalloc_atomic(sizeof(*_tmp65E));_tmp65E[0]=({struct Cyc_Absyn_Nullable_ptrqual_struct
_tmp65F;_tmp65F.tag=7;_tmp65F;});_tmp65E;}));break;case 194: _LL2F5: {struct Cyc_Position_Segment*
loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);yyval=Cyc_YY1(({struct _tuple18*_tmp660=_cycalloc(
sizeof(*_tmp660));_tmp660->f1=loc;_tmp660->f2=Cyc_Absyn_true_conref;_tmp660->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp660;}));
break;}case 195: _LL2F6: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);yyval=Cyc_YY1(({
struct _tuple18*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->f1=loc;_tmp661->f2=
Cyc_Absyn_false_conref;_tmp661->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp661;}));break;}case 196:
_LL2F7: {struct Cyc_Position_Segment*loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);yyval=Cyc_YY1(({
struct _tuple18*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->f1=loc;_tmp662->f2=
Cyc_Absyn_true_conref;_tmp662->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp662;}));
break;}case 197: _LL2F8: yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);break;case 198:
_LL2F9: yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp663=_cycalloc(sizeof(*_tmp663));
_tmp663[0]=({struct Cyc_Absyn_Upper_b_struct _tmp664;_tmp664.tag=1;_tmp664.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp664;});_tmp663;})));
break;case 199: _LL2FA: yyval=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 200: _LL2FB: yyval=Cyc_YY51(Cyc_Absyn_true_conref);break;case 201: _LL2FC:
yyval=Cyc_YY51(Cyc_Absyn_false_conref);break;case 202: _LL2FD: yyval=Cyc_YY44(Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));break;case 203: _LL2FE: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_TopRgnKind,
1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 204:
_LL2FF: yyval=Cyc_YY44(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0));
break;case 205: _LL300: yyval=Cyc_YY25(Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)));break;case
206: _LL301: yyval=Cyc_YY25(Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 207: _LL302:
yyval=Cyc_YY39(({struct _tuple23*_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp665->f2=0;
_tmp665->f3=0;_tmp665->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp665->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp665;}));break;case 208:
_LL303: yyval=Cyc_YY39(({struct _tuple23*_tmp666=_cycalloc(sizeof(*_tmp666));
_tmp666->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]));
_tmp666->f2=1;_tmp666->f3=0;_tmp666->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp666->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp666;}));break;case
209: _LL304: {struct _tuple8 _tmp668;struct Cyc_Core_Opt*_tmp669;struct Cyc_Absyn_Tqual
_tmp66A;void*_tmp66B;struct _tuple8*_tmp667=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp668=*_tmp667;_tmp669=
_tmp668.f1;_tmp66A=_tmp668.f2;_tmp66B=_tmp668.f3;{struct Cyc_Absyn_VarargInfo*
_tmp66C=({struct Cyc_Absyn_VarargInfo*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->name=
_tmp669;_tmp66E->tq=_tmp66A;_tmp66E->type=_tmp66B;_tmp66E->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp66E;});yyval=
Cyc_YY39(({struct _tuple23*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->f1=0;
_tmp66D->f2=0;_tmp66D->f3=_tmp66C;_tmp66D->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp66D->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66D;}));break;};}
case 210: _LL305: {struct _tuple8 _tmp670;struct Cyc_Core_Opt*_tmp671;struct Cyc_Absyn_Tqual
_tmp672;void*_tmp673;struct _tuple8*_tmp66F=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp670=*_tmp66F;_tmp671=
_tmp670.f1;_tmp672=_tmp670.f2;_tmp673=_tmp670.f3;{struct Cyc_Absyn_VarargInfo*
_tmp674=({struct Cyc_Absyn_VarargInfo*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->name=
_tmp671;_tmp676->tq=_tmp672;_tmp676->type=_tmp673;_tmp676->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp676;});yyval=
Cyc_YY39(({struct _tuple23*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp675->f2=0;
_tmp675->f3=_tmp674;_tmp675->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp675->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp675;}));break;};}
case 211: _LL306: yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp678;_tmp678.tag=1;_tmp678.f1=0;_tmp678;});_tmp677;})));break;case 212: _LL307:
yyval=Cyc_YY44(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))));
break;case 213: _LL308: yyval=Cyc_YY49(0);break;case 214: _LL309: yyval=Cyc_YY49(({
struct Cyc_Core_Opt*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->v=(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp67B;_tmp67B.tag=24;_tmp67B.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp67B;});_tmp67A;});
_tmp679;}));break;case 215: _LL30A: yyval=Cyc_YY50(0);break;case 216: _LL30B: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 217: _LL30C: {
struct Cyc_Absyn_Less_kb_struct*_tmp67C=({struct Cyc_Absyn_Less_kb_struct*_tmp683=
_cycalloc(sizeof(*_tmp683));_tmp683[0]=({struct Cyc_Absyn_Less_kb_struct _tmp684;
_tmp684.tag=2;_tmp684.f1=0;_tmp684.f2=Cyc_Absyn_TopRgnKind;_tmp684;});_tmp683;});
struct _dyneither_ptr _tmp67D=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);void*_tmp67E=Cyc_Parse_id2type(
_tmp67D,(void*)_tmp67C);yyval=Cyc_YY50(({struct Cyc_List_List*_tmp67F=_cycalloc(
sizeof(*_tmp67F));_tmp67F->hd=({struct _tuple26*_tmp680=_cycalloc(sizeof(*_tmp680));
_tmp680->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp681=_cycalloc(sizeof(*
_tmp681));_tmp681[0]=({struct Cyc_Absyn_JoinEff_struct _tmp682;_tmp682.tag=24;
_tmp682.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]);_tmp682;});_tmp681;});_tmp680->f2=_tmp67E;_tmp680;});_tmp67F->tl=0;
_tmp67F;}));break;}case 218: _LL30D: {struct Cyc_Absyn_Less_kb_struct*_tmp685=({
struct Cyc_Absyn_Less_kb_struct*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C[0]=({
struct Cyc_Absyn_Less_kb_struct _tmp68D;_tmp68D.tag=2;_tmp68D.f1=0;_tmp68D.f2=Cyc_Absyn_TopRgnKind;
_tmp68D;});_tmp68C;});struct _dyneither_ptr _tmp686=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);void*_tmp687=Cyc_Parse_id2type(
_tmp686,(void*)_tmp685);yyval=Cyc_YY50(({struct Cyc_List_List*_tmp688=_cycalloc(
sizeof(*_tmp688));_tmp688->hd=({struct _tuple26*_tmp689=_cycalloc(sizeof(*_tmp689));
_tmp689->f1=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp68A=_cycalloc(sizeof(*
_tmp68A));_tmp68A[0]=({struct Cyc_Absyn_JoinEff_struct _tmp68B;_tmp68B.tag=24;
_tmp68B.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 4)]);_tmp68B;});_tmp68A;});_tmp689->f2=_tmp687;_tmp689;});_tmp688->tl=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp688;}));break;}
case 219: _LL30E: yyval=Cyc_YY31(0);break;case 220: _LL30F: if(Cyc_zstrcmp((struct
_dyneither_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),({const char*_tmp68E="inject";_tag_dyneither(_tmp68E,sizeof(char),
7);}))!= 0)Cyc_Parse_err(({const char*_tmp68F="missing type in function declaration";
_tag_dyneither(_tmp68F,sizeof(char),37);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY31(
1);break;case 221: _LL310: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 222: _LL311: yyval=Cyc_YY40(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 223: _LL312:
yyval=Cyc_YY40(0);break;case 224: _LL313: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 225: _LL314: yyval=Cyc_YY40(({struct Cyc_List_List*
_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->hd=(void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp692;_tmp692.tag=25;_tmp692.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp692;});_tmp691;});
_tmp690->tl=0;_tmp690;}));break;case 226: _LL315: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_EffKind,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->hd=
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp693->tl=
0;_tmp693;}));break;case 227: _LL316: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_TopRgnKind,1);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694->hd=(
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp695=_cycalloc(sizeof(*_tmp695));
_tmp695[0]=({struct Cyc_Absyn_AccessEff_struct _tmp696;_tmp696.tag=23;_tmp696.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp696;});_tmp695;});_tmp694->tl=0;_tmp694;}));break;case 228: _LL317: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Absyn_TopRgnKind,
1);yyval=Cyc_YY40(({struct Cyc_List_List*_tmp697=_cycalloc(sizeof(*_tmp697));
_tmp697->hd=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp698=_cycalloc(sizeof(*
_tmp698));_tmp698[0]=({struct Cyc_Absyn_AccessEff_struct _tmp699;_tmp699.tag=23;
_tmp699.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp699;});_tmp698;});_tmp697->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp697;}));break;case 229:
_LL318: yyval=Cyc_YY38(({struct Cyc_List_List*_tmp69A=_cycalloc(sizeof(*_tmp69A));
_tmp69A->hd=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp69A->tl=0;_tmp69A;}));break;case 230: _LL319: yyval=Cyc_YY38(({
struct Cyc_List_List*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69B->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp69B;}));break;
case 231: _LL31A: {struct _tuple22 _tmp69D;struct Cyc_Absyn_Tqual _tmp69E;struct Cyc_List_List*
_tmp69F;struct Cyc_List_List*_tmp6A0;struct _tuple22*_tmp69C=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp69D=*_tmp69C;_tmp69E=
_tmp69D.f1;_tmp69F=_tmp69D.f2;_tmp6A0=_tmp69D.f3;if(_tmp69E.loc == 0)_tmp69E.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp6A2;struct _tuple1*
_tmp6A3;struct Cyc_List_List*_tmp6A4;struct Cyc_Parse_Declarator*_tmp6A1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A2=*_tmp6A1;
_tmp6A3=_tmp6A2.id;_tmp6A4=_tmp6A2.tms;{void*_tmp6A5=Cyc_Parse_speclist2typ(
_tmp69F,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp6A7;void*_tmp6A8;struct
Cyc_List_List*_tmp6A9;struct Cyc_List_List*_tmp6AA;struct _tuple11 _tmp6A6=Cyc_Parse_apply_tms(
_tmp69E,_tmp6A5,_tmp6A0,_tmp6A4);_tmp6A7=_tmp6A6.f1;_tmp6A8=_tmp6A6.f2;_tmp6A9=
_tmp6A6.f3;_tmp6AA=_tmp6A6.f4;if(_tmp6A9 != 0)Cyc_Parse_err(({const char*_tmp6AB="parameter with bad type params";
_tag_dyneither(_tmp6AB,sizeof(char),31);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp6A3))Cyc_Parse_err(({const char*_tmp6AC="parameter cannot be qualified with a namespace";
_tag_dyneither(_tmp6AC,sizeof(char),47);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp6AD=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp6B1=_cycalloc(sizeof(*
_tmp6B1));_tmp6B1->v=(*_tmp6A3).f2;_tmp6B1;});if(_tmp6AA != 0)({void*_tmp6AE=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6AF="extra attributes on parameter, ignoring";
_tag_dyneither(_tmp6AF,sizeof(char),40);}),_tag_dyneither(_tmp6AE,sizeof(void*),
0));});yyval=Cyc_YY37(({struct _tuple8*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));
_tmp6B0->f1=_tmp6AD;_tmp6B0->f2=_tmp6A7;_tmp6B0->f3=_tmp6A8;_tmp6B0;}));break;};};};}
case 232: _LL31B: {struct _tuple22 _tmp6B3;struct Cyc_Absyn_Tqual _tmp6B4;struct Cyc_List_List*
_tmp6B5;struct Cyc_List_List*_tmp6B6;struct _tuple22*_tmp6B2=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B3=*_tmp6B2;_tmp6B4=
_tmp6B3.f1;_tmp6B5=_tmp6B3.f2;_tmp6B6=_tmp6B3.f3;if(_tmp6B4.loc == 0)_tmp6B4.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp6B7=Cyc_Parse_speclist2typ(_tmp6B5,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6B6 != 0)({
void*_tmp6B8=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6B9="bad attributes on parameter, ignoring";_tag_dyneither(_tmp6B9,sizeof(
char),38);}),_tag_dyneither(_tmp6B8,sizeof(void*),0));});yyval=Cyc_YY37(({struct
_tuple8*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));_tmp6BA->f1=0;_tmp6BA->f2=_tmp6B4;
_tmp6BA->f3=_tmp6B7;_tmp6BA;}));break;};}case 233: _LL31C: {struct _tuple22 _tmp6BC;
struct Cyc_Absyn_Tqual _tmp6BD;struct Cyc_List_List*_tmp6BE;struct Cyc_List_List*
_tmp6BF;struct _tuple22*_tmp6BB=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6BC=*_tmp6BB;_tmp6BD=
_tmp6BC.f1;_tmp6BE=_tmp6BC.f2;_tmp6BF=_tmp6BC.f3;if(_tmp6BD.loc == 0)_tmp6BD.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp6C0=Cyc_Parse_speclist2typ(_tmp6BE,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6C1=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp6C3;void*_tmp6C4;struct Cyc_List_List*_tmp6C5;struct Cyc_List_List*
_tmp6C6;struct _tuple11 _tmp6C2=Cyc_Parse_apply_tms(_tmp6BD,_tmp6C0,_tmp6BF,
_tmp6C1);_tmp6C3=_tmp6C2.f1;_tmp6C4=_tmp6C2.f2;_tmp6C5=_tmp6C2.f3;_tmp6C6=
_tmp6C2.f4;if(_tmp6C5 != 0)({void*_tmp6C7=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6C8="bad type parameters on formal argument, ignoring";_tag_dyneither(
_tmp6C8,sizeof(char),49);}),_tag_dyneither(_tmp6C7,sizeof(void*),0));});if(
_tmp6C6 != 0)({void*_tmp6C9=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6CA="bad attributes on parameter, ignoring";_tag_dyneither(_tmp6CA,sizeof(
char),38);}),_tag_dyneither(_tmp6C9,sizeof(void*),0));});yyval=Cyc_YY37(({struct
_tuple8*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB->f1=0;_tmp6CB->f2=_tmp6C3;
_tmp6CB->f3=_tmp6C4;_tmp6CB;}));break;};}case 234: _LL31D: yyval=Cyc_YY36(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 235: _LL31E:
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC->hd=({
struct _dyneither_ptr*_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6CD;});_tmp6CC->tl=
0;_tmp6CC;}));break;case 236: _LL31F: yyval=Cyc_YY36(({struct Cyc_List_List*_tmp6CE=
_cycalloc(sizeof(*_tmp6CE));_tmp6CE->hd=({struct _dyneither_ptr*_tmp6CF=_cycalloc(
sizeof(*_tmp6CF));_tmp6CF[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6CF;});_tmp6CE->tl=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6CE;}));break;
case 237: _LL320: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 238: _LL321: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 239: _LL322: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp6D0=_cycalloc(sizeof(*_tmp6D0));
_tmp6D0[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp6D1;_tmp6D1.tag=36;
_tmp6D1.f1=0;_tmp6D1.f2=0;_tmp6D1;});_tmp6D0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 240:
_LL323: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp6D3;_tmp6D3.tag=36;_tmp6D3.f1=0;_tmp6D3.f2=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp6D3;});_tmp6D2;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 241: _LL324: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));
_tmp6D4[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmp6D5;_tmp6D5.tag=36;
_tmp6D5.f1=0;_tmp6D5.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp6D5;});_tmp6D4;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 242:
_LL325: {struct Cyc_Absyn_Vardecl*_tmp6D6=Cyc_Absyn_new_vardecl(({struct _tuple1*
_tmp6D9=_cycalloc(sizeof(*_tmp6D9));_tmp6D9->f1=Cyc_Absyn_Loc_n;_tmp6D9->f2=({
struct _dyneither_ptr*_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp6DA;});_tmp6D9;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp6D6->tq).real_const=
1;yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp6D8;_tmp6D8.tag=28;_tmp6D8.f1=_tmp6D6;_tmp6D8.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6D8.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D8.f4=0;
_tmp6D8;});_tmp6D7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 243:
_LL326: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp6DB=_cycalloc(sizeof(*_tmp6DB));
_tmp6DB->hd=({struct _tuple27*_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC->f1=0;
_tmp6DC->f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6DC;});_tmp6DB->tl=0;_tmp6DB;}));break;case 244: _LL327: yyval=Cyc_YY6(({struct
Cyc_List_List*_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD->hd=({struct _tuple27*
_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6DE->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6DE;});_tmp6DD->tl=
0;_tmp6DD;}));break;case 245: _LL328: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp6DF=
_cycalloc(sizeof(*_tmp6DF));_tmp6DF->hd=({struct _tuple27*_tmp6E0=_cycalloc(
sizeof(*_tmp6E0));_tmp6E0->f1=0;_tmp6E0->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E0;});_tmp6DF->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6DF;}));break;
case 246: _LL329: yyval=Cyc_YY6(({struct Cyc_List_List*_tmp6E1=_cycalloc(sizeof(*
_tmp6E1));_tmp6E1->hd=({struct _tuple27*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));
_tmp6E2->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp6E2->f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6E2;});_tmp6E1->tl=Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6E1;}));break;case
247: _LL32A: yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
break;case 248: _LL32B: yyval=Cyc_YY41(({struct Cyc_List_List*_tmp6E3=_cycalloc(
sizeof(*_tmp6E3));_tmp6E3->hd=Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp6E3->tl=0;_tmp6E3;}));break;case 249: _LL32C: yyval=Cyc_YY41(({
struct Cyc_List_List*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4->hd=Cyc_yyget_YY42(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E4->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E4;}));break;
case 250: _LL32D: yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp6E6;_tmp6E6.tag=0;_tmp6E6.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6E6;});_tmp6E5;}));
break;case 251: _LL32E: yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7[0]=({struct Cyc_Absyn_FieldName_struct
_tmp6E8;_tmp6E8.tag=1;_tmp6E8.f1=({struct _dyneither_ptr*_tmp6E9=_cycalloc(
sizeof(*_tmp6E9));_tmp6E9[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6E9;});_tmp6E8;});
_tmp6E7;}));break;case 252: _LL32F: {struct _tuple22 _tmp6EB;struct Cyc_Absyn_Tqual
_tmp6EC;struct Cyc_List_List*_tmp6ED;struct Cyc_List_List*_tmp6EE;struct _tuple22*
_tmp6EA=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp6EB=*_tmp6EA;_tmp6EC=_tmp6EB.f1;_tmp6ED=_tmp6EB.f2;_tmp6EE=_tmp6EB.f3;{void*
_tmp6EF=Cyc_Parse_speclist2typ(_tmp6ED,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp6EE != 0)({
void*_tmp6F0=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6F1="ignoring attributes in type";_tag_dyneither(_tmp6F1,sizeof(char),28);}),
_tag_dyneither(_tmp6F0,sizeof(void*),0));});yyval=Cyc_YY37(({struct _tuple8*
_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2->f1=0;_tmp6F2->f2=_tmp6EC;_tmp6F2->f3=
_tmp6EF;_tmp6F2;}));break;};}case 253: _LL330: {struct _tuple22 _tmp6F4;struct Cyc_Absyn_Tqual
_tmp6F5;struct Cyc_List_List*_tmp6F6;struct Cyc_List_List*_tmp6F7;struct _tuple22*
_tmp6F3=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp6F4=*_tmp6F3;_tmp6F5=_tmp6F4.f1;_tmp6F6=_tmp6F4.f2;_tmp6F7=_tmp6F4.f3;{void*
_tmp6F8=Cyc_Parse_speclist2typ(_tmp6F6,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp6F9=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct _tuple11 _tmp6FA=Cyc_Parse_apply_tms(_tmp6F5,_tmp6F8,_tmp6F7,_tmp6F9);if(
_tmp6FA.f3 != 0)({void*_tmp6FB=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp6FC="bad type params, ignoring";_tag_dyneither(_tmp6FC,sizeof(char),26);}),
_tag_dyneither(_tmp6FB,sizeof(void*),0));});if(_tmp6FA.f4 != 0)({void*_tmp6FD=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp6FE="bad specifiers, ignoring";
_tag_dyneither(_tmp6FE,sizeof(char),25);}),_tag_dyneither(_tmp6FD,sizeof(void*),
0));});yyval=Cyc_YY37(({struct _tuple8*_tmp6FF=_cycalloc(sizeof(*_tmp6FF));
_tmp6FF->f1=0;_tmp6FF->f2=_tmp6FA.f1;_tmp6FF->f3=_tmp6FA.f2;_tmp6FF;}));break;};}
case 254: _LL331: yyval=Cyc_YY44((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);break;case 255: _LL332:
yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp700=_cycalloc(sizeof(*
_tmp700));_tmp700[0]=({struct Cyc_Absyn_JoinEff_struct _tmp701;_tmp701.tag=24;
_tmp701.f1=0;_tmp701;});_tmp700;}));break;case 256: _LL333: yyval=Cyc_YY44((void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp703;_tmp703.tag=24;_tmp703.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp703;});_tmp702;}));
break;case 257: _LL334: yyval=Cyc_YY44((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp704=_cycalloc(sizeof(*_tmp704));_tmp704[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp705;_tmp705.tag=25;_tmp705.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp705;});_tmp704;}));
break;case 258: _LL335: yyval=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp706=_cycalloc(sizeof(*_tmp706));_tmp706[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp707;_tmp707.tag=24;_tmp707.f1=({struct Cyc_List_List*_tmp708=_cycalloc(
sizeof(*_tmp708));_tmp708->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp708->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp708;});_tmp707;});
_tmp706;}));break;case 259: _LL336: yyval=Cyc_YY40(({struct Cyc_List_List*_tmp709=
_cycalloc(sizeof(*_tmp709));_tmp709->hd=Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp709->tl=0;_tmp709;}));
break;case 260: _LL337: yyval=Cyc_YY40(({struct Cyc_List_List*_tmp70A=_cycalloc(
sizeof(*_tmp70A));_tmp70A->hd=Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp70A->tl=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp70A;}));break;case
261: _LL338: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp70B=_cycalloc(
sizeof(*_tmp70B));_tmp70B->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp70B;}));break;case 262:
_LL339: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
263: _LL33A: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp70C=_cycalloc(
sizeof(*_tmp70C));_tmp70C->tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp70C;}));break;
case 264: _LL33B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];
break;case 265: _LL33C: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp70D=
_cycalloc(sizeof(*_tmp70D));_tmp70D->tms=({struct Cyc_List_List*_tmp70E=_cycalloc(
sizeof(*_tmp70E));_tmp70E->hd=(void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp70F=
_cycalloc(sizeof(*_tmp70F));_tmp70F[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp710;_tmp710.tag=0;_tmp710.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp710.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp710;});
_tmp70F;});_tmp70E->tl=0;_tmp70E;});_tmp70D;}));break;case 266: _LL33D: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp711=_cycalloc(sizeof(*_tmp711));_tmp711->tms=({
struct Cyc_List_List*_tmp712=_cycalloc(sizeof(*_tmp712));_tmp712->hd=(void*)({
struct Cyc_Absyn_Carray_mod_struct*_tmp713=_cycalloc(sizeof(*_tmp713));_tmp713[0]=({
struct Cyc_Absyn_Carray_mod_struct _tmp714;_tmp714.tag=0;_tmp714.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp714.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp714;});
_tmp713;});_tmp712->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp712;});_tmp711;}));break;case 267: _LL33E: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715->tms=({
struct Cyc_List_List*_tmp716=_cycalloc(sizeof(*_tmp716));_tmp716->hd=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp717=_cycalloc(sizeof(*_tmp717));
_tmp717[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp718;_tmp718.tag=1;_tmp718.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp718.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp718.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp718;});_tmp717;});_tmp716->tl=0;_tmp716;});
_tmp715;}));break;case 268: _LL33F: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp719=_cycalloc(sizeof(*_tmp719));_tmp719->tms=({struct Cyc_List_List*_tmp71A=
_cycalloc(sizeof(*_tmp71A));_tmp71A->hd=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp71B=_cycalloc(sizeof(*_tmp71B));_tmp71B[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp71C;_tmp71C.tag=1;_tmp71C.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp71C.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp71C.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp71C;});
_tmp71B;});_tmp71A->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->tms;_tmp71A;});_tmp719;}));break;case 269: _LL340: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp71D=_cycalloc(sizeof(*_tmp71D));_tmp71D->tms=({
struct Cyc_List_List*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp71F=_cycalloc(sizeof(*_tmp71F));_tmp71F[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp720;_tmp720.tag=3;_tmp720.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp721=_cycalloc(sizeof(*_tmp721));
_tmp721[0]=({struct Cyc_Absyn_WithTypes_struct _tmp722;_tmp722.tag=1;_tmp722.f1=0;
_tmp722.f2=0;_tmp722.f3=0;_tmp722.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp722.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp722;});_tmp721;}));
_tmp720;});_tmp71F;});_tmp71E->tl=0;_tmp71E;});_tmp71D;}));break;case 270: _LL341: {
struct _tuple23 _tmp724;struct Cyc_List_List*_tmp725;int _tmp726;struct Cyc_Absyn_VarargInfo*
_tmp727;struct Cyc_Core_Opt*_tmp728;struct Cyc_List_List*_tmp729;struct _tuple23*
_tmp723=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp724=*_tmp723;_tmp725=_tmp724.f1;_tmp726=_tmp724.f2;_tmp727=_tmp724.f3;
_tmp728=_tmp724.f4;_tmp729=_tmp724.f5;yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A->tms=({struct Cyc_List_List*_tmp72B=
_cycalloc(sizeof(*_tmp72B));_tmp72B->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp72D;_tmp72D.tag=3;_tmp72D.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp72E=_cycalloc(sizeof(*_tmp72E));_tmp72E[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp72F;_tmp72F.tag=1;_tmp72F.f1=_tmp725;_tmp72F.f2=_tmp726;_tmp72F.f3=_tmp727;
_tmp72F.f4=_tmp728;_tmp72F.f5=_tmp729;_tmp72F;});_tmp72E;}));_tmp72D;});_tmp72C;});
_tmp72B->tl=0;_tmp72B;});_tmp72A;}));break;}case 271: _LL342: yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730->tms=({
struct Cyc_List_List*_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731->hd=(void*)({
struct Cyc_Absyn_Function_mod_struct*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732[
0]=({struct Cyc_Absyn_Function_mod_struct _tmp733;_tmp733.tag=3;_tmp733.f1=(void*)((
void*)({struct Cyc_Absyn_WithTypes_struct*_tmp734=_cycalloc(sizeof(*_tmp734));
_tmp734[0]=({struct Cyc_Absyn_WithTypes_struct _tmp735;_tmp735.tag=1;_tmp735.f1=0;
_tmp735.f2=0;_tmp735.f3=0;_tmp735.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp735.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp735;});_tmp734;}));
_tmp733;});_tmp732;});_tmp731->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp731;});_tmp730;}));
break;case 272: _LL343: {struct _tuple23 _tmp737;struct Cyc_List_List*_tmp738;int
_tmp739;struct Cyc_Absyn_VarargInfo*_tmp73A;struct Cyc_Core_Opt*_tmp73B;struct Cyc_List_List*
_tmp73C;struct _tuple23*_tmp736=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp737=*_tmp736;_tmp738=
_tmp737.f1;_tmp739=_tmp737.f2;_tmp73A=_tmp737.f3;_tmp73B=_tmp737.f4;_tmp73C=
_tmp737.f5;yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*_tmp73D=_cycalloc(
sizeof(*_tmp73D));_tmp73D->tms=({struct Cyc_List_List*_tmp73E=_cycalloc(sizeof(*
_tmp73E));_tmp73E->hd=(void*)({struct Cyc_Absyn_Function_mod_struct*_tmp73F=
_cycalloc(sizeof(*_tmp73F));_tmp73F[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp740;_tmp740.tag=3;_tmp740.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp742;_tmp742.tag=1;_tmp742.f1=_tmp738;_tmp742.f2=_tmp739;_tmp742.f3=_tmp73A;
_tmp742.f4=_tmp73B;_tmp742.f5=_tmp73C;_tmp742;});_tmp741;}));_tmp740;});_tmp73F;});
_tmp73E->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp73E;});_tmp73D;}));break;}case 273: _LL344: {struct
Cyc_List_List*_tmp743=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct
Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=Cyc_YY30(({
struct Cyc_Parse_Abstractdeclarator*_tmp744=_cycalloc(sizeof(*_tmp744));_tmp744->tms=({
struct Cyc_List_List*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745->hd=(void*)({
struct Cyc_Absyn_TypeParams_mod_struct*_tmp746=_cycalloc(sizeof(*_tmp746));
_tmp746[0]=({struct Cyc_Absyn_TypeParams_mod_struct _tmp747;_tmp747.tag=4;_tmp747.f1=
_tmp743;_tmp747.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp747.f3=0;
_tmp747;});_tmp746;});_tmp745->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp745;});_tmp744;}));
break;}case 274: _LL345: yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator*
_tmp748=_cycalloc(sizeof(*_tmp748));_tmp748->tms=({struct Cyc_List_List*_tmp749=
_cycalloc(sizeof(*_tmp749));_tmp749->hd=(void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp74B;_tmp74B.tag=5;_tmp74B.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp74B.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp74B;});_tmp74A;});
_tmp749->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp749;});_tmp748;}));break;case 275: _LL346: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 276: _LL347: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 277: _LL348:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 278:
_LL349: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
279: _LL34A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 280: _LL34B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 281: _LL34C: yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_ResetRegion_s_struct*
_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C[0]=({struct Cyc_Absyn_ResetRegion_s_struct
_tmp74D;_tmp74D.tag=16;_tmp74D.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp74D;});_tmp74C;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 282: _LL34D: yyval=Cyc_YY4(0);break;case 283:
_LL34E: if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp74E="open";
_tag_dyneither(_tmp74E,sizeof(char),5);}))!= 0)Cyc_Parse_err(({const char*_tmp74F="expecting `open'";
_tag_dyneither(_tmp74F,sizeof(char),17);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY4((
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]));break;case 284: _LL34F: yyval=Cyc_YY9(Cyc_Absyn_new_stmt((void*)({
struct Cyc_Absyn_Label_s_struct*_tmp750=_cycalloc(sizeof(*_tmp750));_tmp750[0]=({
struct Cyc_Absyn_Label_s_struct _tmp751;_tmp751.tag=13;_tmp751.f1=({struct
_dyneither_ptr*_tmp752=_cycalloc(sizeof(*_tmp752));_tmp752[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp752;});_tmp751.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp751;});
_tmp750;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 285: _LL350: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 286: _LL351: yyval=Cyc_YY9(Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 287:
_LL352: yyval=Cyc_YY9(Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 288:
_LL353: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 289: _LL354: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0)));
break;case 290: _LL355: yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])));break;case 291: _LL356:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 292:
_LL357: yyval=Cyc_YY9(Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 293:
_LL358: yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct
Cyc_Absyn_Fn_d_struct*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp754;_tmp754.tag=1;_tmp754.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp754;});_tmp753;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(
0)));break;case 294: _LL359: yyval=Cyc_YY9(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp756;_tmp756.tag=1;_tmp756.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp756;});_tmp755;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 295: _LL35A: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 296: _LL35B: yyval=Cyc_YY9(Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 297:
_LL35C: yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 298:
_LL35D: {struct Cyc_Absyn_Exp*_tmp757=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp758=_cycalloc(sizeof(*_tmp758));_tmp758[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp759;_tmp759.tag=2;_tmp759.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp759;});_tmp758;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(Cyc_Absyn_switch_stmt(_tmp757,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 299:
_LL35E: {struct Cyc_Absyn_Exp*_tmp75A=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=Cyc_YY9(
Cyc_Absyn_switch_stmt(_tmp75A,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 300:
_LL35F: yyval=Cyc_YY9(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 301:
_LL360: yyval=Cyc_YY10(0);break;case 302: _LL361: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp75B=_cycalloc(sizeof(*_tmp75B));_tmp75B->hd=({struct Cyc_Absyn_Switch_clause*
_tmp75C=_cycalloc(sizeof(*_tmp75C));_tmp75C->pattern=Cyc_Absyn_new_pat((void*)&
Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp75C->pat_vars=
0;_tmp75C->where_clause=0;_tmp75C->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75C->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp75C;});
_tmp75B->tl=0;_tmp75B;}));break;case 303: _LL362: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D->hd=({struct Cyc_Absyn_Switch_clause*
_tmp75E=_cycalloc(sizeof(*_tmp75E));_tmp75E->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp75E->pat_vars=0;
_tmp75E->where_clause=0;_tmp75E->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp75E->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp75E;});_tmp75D->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp75D;}));break;case 304:
_LL363: yyval=Cyc_YY10(({struct Cyc_List_List*_tmp75F=_cycalloc(sizeof(*_tmp75F));
_tmp75F->hd=({struct Cyc_Absyn_Switch_clause*_tmp760=_cycalloc(sizeof(*_tmp760));
_tmp760->pattern=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp760->pat_vars=0;_tmp760->where_clause=0;_tmp760->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp760->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp760;});
_tmp75F->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp75F;}));break;case 305: _LL364: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp761=_cycalloc(sizeof(*_tmp761));_tmp761->hd=({struct Cyc_Absyn_Switch_clause*
_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp762->pat_vars=0;
_tmp762->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp762->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp762->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp762;});
_tmp761->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp761;}));break;case 306: _LL365: yyval=Cyc_YY10(({struct Cyc_List_List*
_tmp763=_cycalloc(sizeof(*_tmp763));_tmp763->hd=({struct Cyc_Absyn_Switch_clause*
_tmp764=_cycalloc(sizeof(*_tmp764));_tmp764->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp764->pat_vars=0;
_tmp764->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp764->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp764->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp764;});
_tmp763->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp763;}));break;case 307: _LL366: yyval=Cyc_YY9(Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 308:
_LL367: yyval=Cyc_YY9(Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 309:
_LL368: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 310:
_LL369: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 311:
_LL36A: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 312:
_LL36B: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
313: _LL36C: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 314: _LL36D: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 315:
_LL36E: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 316:
_LL36F: yyval=Cyc_YY9(Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 317:
_LL370: {struct Cyc_List_List*_tmp765=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp766=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp765,_tmp766));break;}case 318: _LL371: {struct
Cyc_List_List*_tmp767=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*_tmp768=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));yyval=Cyc_YY9(Cyc_Parse_flatten_declarations(_tmp767,
_tmp768));break;}case 319: _LL372: {struct Cyc_List_List*_tmp769=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp76A=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp769,_tmp76A));break;}case 320: _LL373: {struct
Cyc_List_List*_tmp76B=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*_tmp76C=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=Cyc_YY9(
Cyc_Parse_flatten_declarations(_tmp76B,_tmp76C));break;}case 321: _LL374: yyval=Cyc_YY9(
Cyc_Absyn_goto_stmt(({struct _dyneither_ptr*_tmp76D=_cycalloc(sizeof(*_tmp76D));
_tmp76D[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp76D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
322: _LL375: yyval=Cyc_YY9(Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
323: _LL376: yyval=Cyc_YY9(Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
324: _LL377: yyval=Cyc_YY9(Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
325: _LL378: yyval=Cyc_YY9(Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
326: _LL379: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)));break;case
327: _LL37A: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)));break;case
328: _LL37B: yyval=Cyc_YY9(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)));break;case
329: _LL37C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 330: _LL37D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 331: _LL37E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 332: _LL37F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 333: _LL380: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 334:
_LL381: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
335: _LL382: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 336:
_LL383: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
337: _LL384: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 338: _LL385: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 339: _LL386: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 340:
_LL387: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
341: _LL388: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 342: _LL389: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 343: _LL38A: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 344:
_LL38B: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 345:
_LL38C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
346: _LL38D: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 347:
_LL38E: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 348:
_LL38F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 349:
_LL390: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 350:
_LL391: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
351: _LL392: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 352: _LL393: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 353:
_LL394: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
354: _LL395: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 355: _LL396: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 356:
_LL397: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
357: _LL398: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 358: _LL399: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 359:
_LL39A: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 360:
_LL39B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
361: _LL39C: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 362: _LL39D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 363: _LL39E: yyval=
Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)])));break;case 364: _LL39F: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 365: _LL3A0: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 366:
_LL3A1: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))));break;case 367:
_LL3A2: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E[0]=({
struct Cyc_Absyn_StructField_struct _tmp76F;_tmp76F.tag=0;_tmp76F.f1=({struct
_dyneither_ptr*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp770;});_tmp76F;});
_tmp76E;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 368: _LL3A3: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp771=
_cycalloc_atomic(sizeof(*_tmp771));_tmp771[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp772;_tmp772.tag=1;_tmp772.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp772;});_tmp771;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))));break;case 369: _LL3A4: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 370: _LL3A5: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 371: _LL3A6:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 372:
_LL3A7: yyval=Cyc_YY11(Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));break;case 373: _LL3A8: {
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);{void*_tmp773=e->r;union Cyc_Absyn_Cnst _tmp775;struct _tuple4
_tmp776;enum Cyc_Absyn_Sign _tmp777;char _tmp778;union Cyc_Absyn_Cnst _tmp77A;struct
_tuple5 _tmp77B;enum Cyc_Absyn_Sign _tmp77C;short _tmp77D;union Cyc_Absyn_Cnst
_tmp77F;struct _tuple6 _tmp780;enum Cyc_Absyn_Sign _tmp781;int _tmp782;union Cyc_Absyn_Cnst
_tmp784;struct _dyneither_ptr _tmp785;union Cyc_Absyn_Cnst _tmp787;int _tmp788;union
Cyc_Absyn_Cnst _tmp78A;struct _dyneither_ptr _tmp78B;union Cyc_Absyn_Cnst _tmp78D;
struct _tuple7 _tmp78E;_LL3AB: {struct Cyc_Absyn_Const_e_struct*_tmp774=(struct Cyc_Absyn_Const_e_struct*)
_tmp773;if(_tmp774->tag != 0)goto _LL3AD;else{_tmp775=_tmp774->f1;if((_tmp775.Char_c).tag
!= 2)goto _LL3AD;_tmp776=(struct _tuple4)(_tmp775.Char_c).val;_tmp777=_tmp776.f1;
_tmp778=_tmp776.f2;}}_LL3AC: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp78F=_cycalloc_atomic(sizeof(*_tmp78F));_tmp78F[0]=({struct Cyc_Absyn_Char_p_struct
_tmp790;_tmp790.tag=10;_tmp790.f1=_tmp778;_tmp790;});_tmp78F;}),e->loc));goto
_LL3AA;_LL3AD: {struct Cyc_Absyn_Const_e_struct*_tmp779=(struct Cyc_Absyn_Const_e_struct*)
_tmp773;if(_tmp779->tag != 0)goto _LL3AF;else{_tmp77A=_tmp779->f1;if((_tmp77A.Short_c).tag
!= 3)goto _LL3AF;_tmp77B=(struct _tuple5)(_tmp77A.Short_c).val;_tmp77C=_tmp77B.f1;
_tmp77D=_tmp77B.f2;}}_LL3AE: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp791=_cycalloc_atomic(sizeof(*_tmp791));_tmp791[0]=({struct Cyc_Absyn_Int_p_struct
_tmp792;_tmp792.tag=9;_tmp792.f1=_tmp77C;_tmp792.f2=(int)_tmp77D;_tmp792;});
_tmp791;}),e->loc));goto _LL3AA;_LL3AF: {struct Cyc_Absyn_Const_e_struct*_tmp77E=(
struct Cyc_Absyn_Const_e_struct*)_tmp773;if(_tmp77E->tag != 0)goto _LL3B1;else{
_tmp77F=_tmp77E->f1;if((_tmp77F.Int_c).tag != 4)goto _LL3B1;_tmp780=(struct _tuple6)(
_tmp77F.Int_c).val;_tmp781=_tmp780.f1;_tmp782=_tmp780.f2;}}_LL3B0: yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*_tmp793=_cycalloc_atomic(
sizeof(*_tmp793));_tmp793[0]=({struct Cyc_Absyn_Int_p_struct _tmp794;_tmp794.tag=9;
_tmp794.f1=_tmp781;_tmp794.f2=_tmp782;_tmp794;});_tmp793;}),e->loc));goto _LL3AA;
_LL3B1: {struct Cyc_Absyn_Const_e_struct*_tmp783=(struct Cyc_Absyn_Const_e_struct*)
_tmp773;if(_tmp783->tag != 0)goto _LL3B3;else{_tmp784=_tmp783->f1;if((_tmp784.Float_c).tag
!= 6)goto _LL3B3;_tmp785=(struct _dyneither_ptr)(_tmp784.Float_c).val;}}_LL3B2:
yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Float_p_struct*_tmp795=
_cycalloc(sizeof(*_tmp795));_tmp795[0]=({struct Cyc_Absyn_Float_p_struct _tmp796;
_tmp796.tag=11;_tmp796.f1=_tmp785;_tmp796;});_tmp795;}),e->loc));goto _LL3AA;
_LL3B3: {struct Cyc_Absyn_Const_e_struct*_tmp786=(struct Cyc_Absyn_Const_e_struct*)
_tmp773;if(_tmp786->tag != 0)goto _LL3B5;else{_tmp787=_tmp786->f1;if((_tmp787.Null_c).tag
!= 1)goto _LL3B5;_tmp788=(int)(_tmp787.Null_c).val;}}_LL3B4: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3AA;_LL3B5: {struct Cyc_Absyn_Const_e_struct*
_tmp789=(struct Cyc_Absyn_Const_e_struct*)_tmp773;if(_tmp789->tag != 0)goto _LL3B7;
else{_tmp78A=_tmp789->f1;if((_tmp78A.String_c).tag != 7)goto _LL3B7;_tmp78B=(
struct _dyneither_ptr)(_tmp78A.String_c).val;}}_LL3B6: Cyc_Parse_err(({const char*
_tmp797="strings cannot occur within patterns";_tag_dyneither(_tmp797,sizeof(
char),37);}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));goto _LL3AA;_LL3B7: {struct Cyc_Absyn_Const_e_struct*
_tmp78C=(struct Cyc_Absyn_Const_e_struct*)_tmp773;if(_tmp78C->tag != 0)goto _LL3B9;
else{_tmp78D=_tmp78C->f1;if((_tmp78D.LongLong_c).tag != 5)goto _LL3B9;_tmp78E=(
struct _tuple7)(_tmp78D.LongLong_c).val;}}_LL3B8: Cyc_Parse_unimp(({const char*
_tmp798="long long's in patterns";_tag_dyneither(_tmp798,sizeof(char),24);}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL3AA;
_LL3B9:;_LL3BA: Cyc_Parse_err(({const char*_tmp799="bad constant in case";
_tag_dyneither(_tmp799,sizeof(char),21);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL3AA:;}break;}
case 374: _LL3A9: yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp79A=_cycalloc(sizeof(*_tmp79A));_tmp79A[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp79B;_tmp79B.tag=14;_tmp79B.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp79B;});_tmp79A;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 375:
_LL3BB: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp79C="as";
_tag_dyneither(_tmp79C,sizeof(char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp79D="expecting `as'";
_tag_dyneither(_tmp79D,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp79E=_cycalloc(
sizeof(*_tmp79E));_tmp79E[0]=({struct Cyc_Absyn_Var_p_struct _tmp79F;_tmp79F.tag=1;
_tmp79F.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7A0=_cycalloc(sizeof(*
_tmp7A0));_tmp7A0->f1=Cyc_Absyn_Loc_n;_tmp7A0->f2=({struct _dyneither_ptr*_tmp7A1=
_cycalloc(sizeof(*_tmp7A1));_tmp7A1[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7A1;});_tmp7A0;}),(
void*)& Cyc_Absyn_VoidType_val,0);_tmp79F.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp79F;});_tmp79E;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)));break;case
376: _LL3BC: {struct Cyc_List_List*_tmp7A3;int _tmp7A4;struct _tuple19 _tmp7A2=*Cyc_yyget_YY12(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A3=_tmp7A2.f1;
_tmp7A4=_tmp7A2.f2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Tuple_p_struct*
_tmp7A5=_cycalloc(sizeof(*_tmp7A5));_tmp7A5[0]=({struct Cyc_Absyn_Tuple_p_struct
_tmp7A6;_tmp7A6.tag=4;_tmp7A6.f1=_tmp7A3;_tmp7A6.f2=_tmp7A4;_tmp7A6;});_tmp7A5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 377: _LL3BD: {struct Cyc_List_List*_tmp7A8;
int _tmp7A9;struct _tuple19 _tmp7A7=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7A8=_tmp7A7.f1;
_tmp7A9=_tmp7A7.f2;yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownCall_p_struct*
_tmp7AA=_cycalloc(sizeof(*_tmp7AA));_tmp7AA[0]=({struct Cyc_Absyn_UnknownCall_p_struct
_tmp7AB;_tmp7AB.tag=15;_tmp7AB.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7AB.f2=_tmp7A8;
_tmp7AB.f3=_tmp7A9;_tmp7AB;});_tmp7AA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 378:
_LL3BE: {struct Cyc_List_List*_tmp7AD;int _tmp7AE;struct _tuple19 _tmp7AC=*Cyc_yyget_YY16(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AD=_tmp7AC.f1;
_tmp7AE=_tmp7AC.f2;{struct Cyc_List_List*_tmp7AF=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp7B0=_cycalloc(sizeof(*_tmp7B0));
_tmp7B0[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp7B1;_tmp7B1.tag=6;_tmp7B1.f1=({
struct Cyc_Absyn_AggrInfo*_tmp7B2=_cycalloc(sizeof(*_tmp7B2));_tmp7B2->aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),0);_tmp7B2->targs=0;
_tmp7B2;});_tmp7B1.f2=_tmp7AF;_tmp7B1.f3=_tmp7AD;_tmp7B1.f4=_tmp7AE;_tmp7B1;});
_tmp7B0;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;};}case 379: _LL3BF: {struct Cyc_List_List*
_tmp7B4;int _tmp7B5;struct _tuple19 _tmp7B3=*Cyc_yyget_YY16(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7B4=_tmp7B3.f1;
_tmp7B5=_tmp7B3.f2;{struct Cyc_List_List*_tmp7B6=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));
_tmp7B7[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp7B8;_tmp7B8.tag=6;_tmp7B8.f1=0;
_tmp7B8.f2=_tmp7B6;_tmp7B8.f3=_tmp7B4;_tmp7B8.f4=_tmp7B5;_tmp7B8;});_tmp7B7;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;};}case 380: _LL3C0: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7B9=_cycalloc(sizeof(*_tmp7B9));
_tmp7B9[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7BA;_tmp7BA.tag=5;_tmp7BA.f1=
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7BA;});
_tmp7B9;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 381: _LL3C1: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7BB=_cycalloc(sizeof(*_tmp7BB));
_tmp7BB[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7BC;_tmp7BC.tag=5;_tmp7BC.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7BD=_cycalloc(
sizeof(*_tmp7BD));_tmp7BD[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7BE;_tmp7BE.tag=
5;_tmp7BE.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7BE;});_tmp7BD;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7BC;});
_tmp7BB;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 382: _LL3C2: yyval=Cyc_YY11(Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7BF=_cycalloc(sizeof(*_tmp7BF));
_tmp7BF[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7C0;_tmp7C0.tag=2;_tmp7C0.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7C1=_cycalloc(sizeof(*_tmp7C1));
_tmp7C1->f1=Cyc_Absyn_Loc_n;_tmp7C1->f2=({struct _dyneither_ptr*_tmp7C2=_cycalloc(
sizeof(*_tmp7C2));_tmp7C2[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7C2;});_tmp7C1;}),(void*)&
Cyc_Absyn_VoidType_val,0);_tmp7C0.f2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7C0;});_tmp7BF;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 383:
_LL3C3: if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7C3="as";
_tag_dyneither(_tmp7C3,sizeof(char),3);}))!= 0)Cyc_Parse_err(({const char*_tmp7C4="expecting `as'";
_tag_dyneither(_tmp7C4,sizeof(char),15);}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=Cyc_YY11(
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7C5=_cycalloc(
sizeof(*_tmp7C5));_tmp7C5[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7C6;_tmp7C6.tag=
2;_tmp7C6.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7C7=_cycalloc(sizeof(*
_tmp7C7));_tmp7C7->f1=Cyc_Absyn_Loc_n;_tmp7C7->f2=({struct _dyneither_ptr*_tmp7C8=
_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7C8;});_tmp7C7;}),(
void*)& Cyc_Absyn_VoidType_val,0);_tmp7C6.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7C6;});_tmp7C5;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)));break;case
384: _LL3C4: {void*_tmp7C9=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_IntKind));yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp7CA=_cycalloc(sizeof(*_tmp7CA));_tmp7CA[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp7CB;_tmp7CB.tag=3;_tmp7CB.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp7C9);
_tmp7CB.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7CC=_cycalloc(sizeof(*
_tmp7CC));_tmp7CC->f1=Cyc_Absyn_Loc_n;_tmp7CC->f2=({struct _dyneither_ptr*_tmp7CD=
_cycalloc(sizeof(*_tmp7CD));_tmp7CD[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7CD;});_tmp7CC;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp7CE=_cycalloc(sizeof(*_tmp7CE));
_tmp7CE[0]=({struct Cyc_Absyn_TagType_struct _tmp7CF;_tmp7CF.tag=20;_tmp7CF.f1=(
void*)_tmp7C9;_tmp7CF;});_tmp7CE;}),0);_tmp7CB;});_tmp7CA;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 385:
_LL3C5: {struct Cyc_Absyn_Tvar*_tmp7D0=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(
Cyc_Absyn_IntKind));yyval=Cyc_YY11(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp7D1=_cycalloc(sizeof(*_tmp7D1));_tmp7D1[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp7D2;_tmp7D2.tag=3;_tmp7D2.f1=_tmp7D0;_tmp7D2.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp7D3=_cycalloc(sizeof(*_tmp7D3));_tmp7D3->f1=Cyc_Absyn_Loc_n;
_tmp7D3->f2=({struct _dyneither_ptr*_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7D4;});_tmp7D3;}),(void*)({struct Cyc_Absyn_TagType_struct*_tmp7D5=_cycalloc(
sizeof(*_tmp7D5));_tmp7D5[0]=({struct Cyc_Absyn_TagType_struct _tmp7D6;_tmp7D6.tag=
20;_tmp7D6.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp7D7=_cycalloc(
sizeof(*_tmp7D7));_tmp7D7[0]=({struct Cyc_Absyn_VarType_struct _tmp7D8;_tmp7D8.tag=
2;_tmp7D8.f1=_tmp7D0;_tmp7D8;});_tmp7D7;}));_tmp7D6;});_tmp7D5;}),0);_tmp7D2;});
_tmp7D1;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;}case 386: _LL3C6: yyval=Cyc_YY12(({struct
_tuple19*_tmp7D9=_cycalloc(sizeof(*_tmp7D9));_tmp7D9->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp7D9->f2=0;_tmp7D9;}));
break;case 387: _LL3C7: yyval=Cyc_YY12(({struct _tuple19*_tmp7DA=_cycalloc(sizeof(*
_tmp7DA));_tmp7DA->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp7DA->f2=1;_tmp7DA;}));break;case 388: _LL3C8: yyval=Cyc_YY12(({struct _tuple19*
_tmp7DB=_cycalloc(sizeof(*_tmp7DB));_tmp7DB->f1=0;_tmp7DB->f2=1;_tmp7DB;}));
break;case 389: _LL3C9: yyval=Cyc_YY13(({struct Cyc_List_List*_tmp7DC=_cycalloc(
sizeof(*_tmp7DC));_tmp7DC->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7DC->tl=0;_tmp7DC;}));break;case 390: _LL3CA: yyval=Cyc_YY13(({
struct Cyc_List_List*_tmp7DD=_cycalloc(sizeof(*_tmp7DD));_tmp7DD->hd=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7DD->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7DD;}));break;
case 391: _LL3CB: yyval=Cyc_YY14(({struct _tuple20*_tmp7DE=_cycalloc(sizeof(*_tmp7DE));
_tmp7DE->f1=0;_tmp7DE->f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7DE;}));break;case 392: _LL3CC: yyval=Cyc_YY14(({struct
_tuple20*_tmp7DF=_cycalloc(sizeof(*_tmp7DF));_tmp7DF->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7DF->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7DF;}));break;case
393: _LL3CD: yyval=Cyc_YY16(({struct _tuple19*_tmp7E0=_cycalloc(sizeof(*_tmp7E0));
_tmp7E0->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp7E0->f2=0;_tmp7E0;}));
break;case 394: _LL3CE: yyval=Cyc_YY16(({struct _tuple19*_tmp7E1=_cycalloc(sizeof(*
_tmp7E1));_tmp7E1->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp7E1->f2=1;_tmp7E1;}));break;case 395: _LL3CF: yyval=Cyc_YY16(({struct _tuple19*
_tmp7E2=_cycalloc(sizeof(*_tmp7E2));_tmp7E2->f1=0;_tmp7E2->f2=1;_tmp7E2;}));
break;case 396: _LL3D0: yyval=Cyc_YY15(({struct Cyc_List_List*_tmp7E3=_cycalloc(
sizeof(*_tmp7E3));_tmp7E3->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp7E3->tl=0;_tmp7E3;}));break;case 397: _LL3D1: yyval=Cyc_YY15(({
struct Cyc_List_List*_tmp7E4=_cycalloc(sizeof(*_tmp7E4));_tmp7E4->hd=Cyc_yyget_YY14(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E4->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7E4;}));break;
case 398: _LL3D2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 399: _LL3D3: yyval=Cyc_YY3(Cyc_Absyn_seq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 400:
_LL3D4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
401: _LL3D5: yyval=Cyc_YY3(Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 402:
_LL3D6: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Swap_e_struct*
_tmp7E5=_cycalloc(sizeof(*_tmp7E5));_tmp7E5[0]=({struct Cyc_Absyn_Swap_e_struct
_tmp7E6;_tmp7E6.tag=35;_tmp7E6.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7E6.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7E6;});_tmp7E5;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 403: _LL3D7: yyval=Cyc_YY8(0);break;case 404:
_LL3D8: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7E7=_cycalloc_atomic(sizeof(*
_tmp7E7));_tmp7E7->v=(void*)Cyc_Absyn_Times;_tmp7E7;}));break;case 405: _LL3D9:
yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7E8=_cycalloc_atomic(sizeof(*_tmp7E8));
_tmp7E8->v=(void*)Cyc_Absyn_Div;_tmp7E8;}));break;case 406: _LL3DA: yyval=Cyc_YY8(({
struct Cyc_Core_Opt*_tmp7E9=_cycalloc_atomic(sizeof(*_tmp7E9));_tmp7E9->v=(void*)
Cyc_Absyn_Mod;_tmp7E9;}));break;case 407: _LL3DB: yyval=Cyc_YY8(({struct Cyc_Core_Opt*
_tmp7EA=_cycalloc_atomic(sizeof(*_tmp7EA));_tmp7EA->v=(void*)Cyc_Absyn_Plus;
_tmp7EA;}));break;case 408: _LL3DC: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7EB=
_cycalloc_atomic(sizeof(*_tmp7EB));_tmp7EB->v=(void*)Cyc_Absyn_Minus;_tmp7EB;}));
break;case 409: _LL3DD: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7EC=_cycalloc_atomic(
sizeof(*_tmp7EC));_tmp7EC->v=(void*)Cyc_Absyn_Bitlshift;_tmp7EC;}));break;case
410: _LL3DE: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7ED=_cycalloc_atomic(sizeof(*
_tmp7ED));_tmp7ED->v=(void*)Cyc_Absyn_Bitlrshift;_tmp7ED;}));break;case 411:
_LL3DF: yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7EE=_cycalloc_atomic(sizeof(*
_tmp7EE));_tmp7EE->v=(void*)Cyc_Absyn_Bitand;_tmp7EE;}));break;case 412: _LL3E0:
yyval=Cyc_YY8(({struct Cyc_Core_Opt*_tmp7EF=_cycalloc_atomic(sizeof(*_tmp7EF));
_tmp7EF->v=(void*)Cyc_Absyn_Bitxor;_tmp7EF;}));break;case 413: _LL3E1: yyval=Cyc_YY8(({
struct Cyc_Core_Opt*_tmp7F0=_cycalloc_atomic(sizeof(*_tmp7F0));_tmp7F0->v=(void*)
Cyc_Absyn_Bitor;_tmp7F0;}));break;case 414: _LL3E2: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 415: _LL3E3: yyval=
Cyc_YY3(Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 416:
_LL3E4: yyval=Cyc_YY3(Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 417:
_LL3E5: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 418:
_LL3E6: yyval=Cyc_YY3(Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 419:
_LL3E7: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 420:
_LL3E8: yyval=Cyc_YY3(Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 421:
_LL3E9: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
422: _LL3EA: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 423: _LL3EB: yyval=Cyc_YY3(Cyc_Absyn_or_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 424:
_LL3EC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
425: _LL3ED: yyval=Cyc_YY3(Cyc_Absyn_and_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 426:
_LL3EE: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
427: _LL3EF: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 428:
_LL3F0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
429: _LL3F1: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 430:
_LL3F2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
431: _LL3F3: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 432:
_LL3F4: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
433: _LL3F5: yyval=Cyc_YY3(Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 434:
_LL3F6: yyval=Cyc_YY3(Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 435:
_LL3F7: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
436: _LL3F8: yyval=Cyc_YY3(Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 437:
_LL3F9: yyval=Cyc_YY3(Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 438:
_LL3FA: yyval=Cyc_YY3(Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 439:
_LL3FB: yyval=Cyc_YY3(Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 440:
_LL3FC: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
441: _LL3FD: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 442:
_LL3FE: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 443:
_LL3FF: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
444: _LL400: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 445:
_LL401: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 446:
_LL402: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
447: _LL403: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 448:
_LL404: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 449:
_LL405: yyval=Cyc_YY3(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 450:
_LL406: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
451: _LL407: yyval=Cyc_YY3(Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),1,Cyc_Absyn_Unknown_coercion,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 452: _LL408: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 453: _LL409: yyval=
Cyc_YY3(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 454:
_LL40A: yyval=Cyc_YY3(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 455:
_LL40B: yyval=Cyc_YY3(Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 456:
_LL40C: yyval=Cyc_YY3(Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 457:
_LL40D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
458: _LL40E: yyval=Cyc_YY3(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 459:
_LL40F: yyval=Cyc_YY3(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 460:
_LL410: yyval=Cyc_YY3(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 461:
_LL411: yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*
_tmp7F1=_cycalloc(sizeof(*_tmp7F1));_tmp7F1[0]=({struct Cyc_Absyn_StructField_struct
_tmp7F2;_tmp7F2.tag=0;_tmp7F2.f1=({struct _dyneither_ptr*_tmp7F3=_cycalloc(
sizeof(*_tmp7F3));_tmp7F3[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7F3;});_tmp7F2;});
_tmp7F1;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 462: _LL412: yyval=Cyc_YY3(Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7F4=_cycalloc_atomic(sizeof(*
_tmp7F4));_tmp7F4[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp7F5;_tmp7F5.tag=1;
_tmp7F5.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp7F5;});_tmp7F4;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 463:
_LL413: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp7F6=_cycalloc(sizeof(*_tmp7F6));_tmp7F6[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp7F7;_tmp7F7.tag=34;_tmp7F7.f1=({struct Cyc_Absyn_MallocInfo _tmp7F8;_tmp7F8.is_calloc=
0;_tmp7F8.rgn=0;_tmp7F8.elt_type=0;_tmp7F8.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7F8.fat_result=0;
_tmp7F8;});_tmp7F7;});_tmp7F6;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 464:
_LL414: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp7F9=_cycalloc(sizeof(*_tmp7F9));_tmp7F9[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp7FA;_tmp7FA.tag=34;_tmp7FA.f1=({struct Cyc_Absyn_MallocInfo _tmp7FB;_tmp7FB.is_calloc=
0;_tmp7FB.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp7FB.elt_type=0;
_tmp7FB.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp7FB.fat_result=0;_tmp7FB;});_tmp7FA;});_tmp7F9;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 465:
_LL415: {void*_tmp7FD;struct _tuple8 _tmp7FC=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7FD=_tmp7FC.f3;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp7FE=
_cycalloc(sizeof(*_tmp7FE));_tmp7FE[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp7FF;
_tmp7FF.tag=34;_tmp7FF.f1=({struct Cyc_Absyn_MallocInfo _tmp800;_tmp800.is_calloc=
1;_tmp800.rgn=0;_tmp800.elt_type=({void**_tmp801=_cycalloc(sizeof(*_tmp801));
_tmp801[0]=_tmp7FD;_tmp801;});_tmp800.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp800.fat_result=0;
_tmp800;});_tmp7FF;});_tmp7FE;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 466:
_LL416: {void*_tmp803;struct _tuple8 _tmp802=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp803=_tmp802.f3;yyval=
Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*_tmp804=
_cycalloc(sizeof(*_tmp804));_tmp804[0]=({struct Cyc_Absyn_Malloc_e_struct _tmp805;
_tmp805.tag=34;_tmp805.f1=({struct Cyc_Absyn_MallocInfo _tmp806;_tmp806.is_calloc=
1;_tmp806.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp806.elt_type=({void**
_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807[0]=_tmp803;_tmp807;});_tmp806.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp806.fat_result=0;_tmp806;});_tmp805;});_tmp804;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 467:
_LL417: yyval=Cyc_YY3(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,({struct Cyc_Absyn_Exp*
_tmp808[1];_tmp808[0]=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp808,sizeof(struct Cyc_Absyn_Exp*),1));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 468:
_LL418: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_struct*
_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Absyn_Tagcheck_e_struct
_tmp80A;_tmp80A.tag=38;_tmp80A.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp80A.f2=({struct
_dyneither_ptr*_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp80B;});_tmp80A;});
_tmp809;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 469: _LL419: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Tagcheck_e_struct*_tmp80C=_cycalloc(sizeof(*_tmp80C));
_tmp80C[0]=({struct Cyc_Absyn_Tagcheck_e_struct _tmp80D;_tmp80D.tag=38;_tmp80D.f1=
Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp80D.f2=({
struct _dyneither_ptr*_tmp80E=_cycalloc(sizeof(*_tmp80E));_tmp80E[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp80E;});_tmp80D;});
_tmp80C;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 470: _LL41A: {void*_tmp810;struct _tuple8
_tmp80F=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp810=_tmp80F.f3;yyval=Cyc_YY3(Cyc_Absyn_valueof_exp(_tmp810,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;}case 471:
_LL41B: yyval=Cyc_YY7(Cyc_Absyn_Bitnot);break;case 472: _LL41C: yyval=Cyc_YY7(Cyc_Absyn_Not);
break;case 473: _LL41D: yyval=Cyc_YY7(Cyc_Absyn_Minus);break;case 474: _LL41E: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 475: _LL41F:
yyval=Cyc_YY3(Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 476:
_LL420: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 477:
_LL421: yyval=Cyc_YY3(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 478:
_LL422: yyval=Cyc_YY3(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _dyneither_ptr*
_tmp811=_cycalloc(sizeof(*_tmp811));_tmp811[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp811;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 479:
_LL423: yyval=Cyc_YY3(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct _dyneither_ptr*
_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp812;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 480:
_LL424: yyval=Cyc_YY3(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 481:
_LL425: yyval=Cyc_YY3(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 482:
_LL426: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_struct*
_tmp813=_cycalloc(sizeof(*_tmp813));_tmp813[0]=({struct Cyc_Absyn_CompoundLit_e_struct
_tmp814;_tmp814.tag=26;_tmp814.f1=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp814.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp814;});_tmp813;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 483: _LL427: yyval=Cyc_YY3(Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp815=_cycalloc(sizeof(*_tmp815));
_tmp815[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp816;_tmp816.tag=26;_tmp816.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp816.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp816;});_tmp815;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 484:
_LL428: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp817=_cycalloc(sizeof(*_tmp817));_tmp817[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp818;_tmp818.tag=2;_tmp818.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp818;});_tmp817;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 485:
_LL429: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
486: _LL42A: yyval=Cyc_YY3(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 487:
_LL42B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;
case 488: _LL42C: yyval=Cyc_YY3(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 489:
_LL42D: yyval=Cyc_YY3(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 490:
_LL42E: yyval=Cyc_YY3(Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 491:
_LL42F: yyval=Cyc_YY3(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_struct*
_tmp819=_cycalloc(sizeof(*_tmp819));_tmp819[0]=({struct Cyc_Absyn_Aggregate_e_struct
_tmp81A;_tmp81A.tag=29;_tmp81A.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp81A.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp81A.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp81A.f4=0;_tmp81A;});
_tmp819;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));break;case 492: _LL430: yyval=Cyc_YY3(Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 493:
_LL431: yyval=Cyc_YY5(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));break;
case 494: _LL432: yyval=Cyc_YY5(({struct Cyc_List_List*_tmp81B=_cycalloc(sizeof(*
_tmp81B));_tmp81B->hd=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp81B->tl=0;_tmp81B;}));break;case 495: _LL433: yyval=Cyc_YY5(({
struct Cyc_List_List*_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C->hd=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81C->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp81C;}));break;
case 496: _LL434: yyval=Cyc_YY3(Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 497:
_LL435: yyval=Cyc_YY3(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 498:
_LL436: yyval=Cyc_YY3(Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 499:
_LL437: yyval=Cyc_YY3(Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));break;case 500:
_LL438: yyval=Cyc_QualId_tok(({struct _tuple1*_tmp81D=_cycalloc(sizeof(*_tmp81D));
_tmp81D->f1=Cyc_Absyn_Rel_n(0);_tmp81D->f2=({struct _dyneither_ptr*_tmp81E=
_cycalloc(sizeof(*_tmp81E));_tmp81E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp81E;});_tmp81D;}));
break;case 501: _LL439: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 502: _LL43A: yyval=Cyc_QualId_tok(({struct _tuple1*_tmp81F=
_cycalloc(sizeof(*_tmp81F));_tmp81F->f1=Cyc_Absyn_Rel_n(0);_tmp81F->f2=({struct
_dyneither_ptr*_tmp820=_cycalloc(sizeof(*_tmp820));_tmp820[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp820;});_tmp81F;}));
break;case 503: _LL43B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 504: _LL43C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 505: _LL43D: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 506: _LL43E: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 507: _LL43F:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 508:
_LL440: break;case 509: _LL441: yylex_buf->lex_curr_pos -=1;break;default: _LL442:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;++ yylsp_offset;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(510,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(143,yyn - 143)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
5862) && Cyc_yycheck[_check_known_subscript_notnull(5863,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(5863,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(143,yyn - 143)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1027,yystate)];if(yyn > - 32768  && yyn < 5862){int
sze=0;struct _dyneither_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 286 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(5863,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[
_check_known_subscript_notnull(286,x)])+ 15,count ++);}msg=({unsigned int _tmp821=(
unsigned int)(sze + 15);char*_tmp822=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp821 + 1));struct _dyneither_ptr _tmp824=_tag_dyneither(_tmp822,
sizeof(char),_tmp821 + 1);{unsigned int _tmp823=_tmp821;unsigned int i;for(i=0;i < 
_tmp823;i ++){_tmp822[i]='\000';}_tmp822[_tmp823]=(char)0;}_tmp824;});Cyc_strcpy(
msg,({const char*_tmp825="parse error";_tag_dyneither(_tmp825,sizeof(char),12);}));
if(count < 5){count=0;for(x=yyn < 0?- yyn: 0;x < 286 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(5863,x + yyn)]== x){Cyc_strcat(msg,(struct
_dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp826=", expecting `";
_tag_dyneither(_tmp826,sizeof(char),14);}):(struct _dyneither_ptr)({const char*
_tmp827=" or `";_tag_dyneither(_tmp827,sizeof(char),6);})));Cyc_strcat(msg,(
struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(286,x)]);Cyc_strcat(
msg,({const char*_tmp828="'";_tag_dyneither(_tmp828,sizeof(char),2);}));++ count;}}}
Cyc_yyerror((struct _dyneither_ptr)msg);}else{Cyc_yyerror(({const char*_tmp829="parse error";
_tag_dyneither(_tmp829,sizeof(char),12);}));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp82A=1;_npop_handler(0);return _tmp82A;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp82B=1;_npop_handler(0);return _tmp82B;}-- yyvsp_offset;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];--
yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1027,
yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 5862) || 
Cyc_yycheck[_check_known_subscript_notnull(5863,yyn)]!= 1)goto yyerrdefault;yyn=(
int)Cyc_yytable[_check_known_subscript_notnull(5863,yyn)];if(yyn < 0){if(yyn == -
32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 
1026){int _tmp82C=0;_npop_handler(0);return _tmp82C;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE
_tmp836=v;struct _tuple6 _tmp837;int _tmp838;char _tmp839;struct _dyneither_ptr
_tmp83A;struct _tuple1*_tmp83B;struct _tuple1 _tmp83C;union Cyc_Absyn_Nmspace _tmp83D;
struct _dyneither_ptr*_tmp83E;_LL445: if((_tmp836.Int_tok).tag != 1)goto _LL447;
_tmp837=(struct _tuple6)(_tmp836.Int_tok).val;_tmp838=_tmp837.f2;_LL446:({struct
Cyc_Int_pa_struct _tmp841;_tmp841.tag=1;_tmp841.f1=(unsigned long)_tmp838;({void*
_tmp83F[1]={& _tmp841};Cyc_fprintf(Cyc_stderr,({const char*_tmp840="%d";
_tag_dyneither(_tmp840,sizeof(char),3);}),_tag_dyneither(_tmp83F,sizeof(void*),1));});});
goto _LL444;_LL447: if((_tmp836.Char_tok).tag != 2)goto _LL449;_tmp839=(char)(
_tmp836.Char_tok).val;_LL448:({struct Cyc_Int_pa_struct _tmp844;_tmp844.tag=1;
_tmp844.f1=(unsigned long)((int)_tmp839);({void*_tmp842[1]={& _tmp844};Cyc_fprintf(
Cyc_stderr,({const char*_tmp843="%c";_tag_dyneither(_tmp843,sizeof(char),3);}),
_tag_dyneither(_tmp842,sizeof(void*),1));});});goto _LL444;_LL449: if((_tmp836.String_tok).tag
!= 3)goto _LL44B;_tmp83A=(struct _dyneither_ptr)(_tmp836.String_tok).val;_LL44A:({
struct Cyc_String_pa_struct _tmp847;_tmp847.tag=0;_tmp847.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp83A);({void*_tmp845[1]={& _tmp847};Cyc_fprintf(Cyc_stderr,({
const char*_tmp846="\"%s\"";_tag_dyneither(_tmp846,sizeof(char),5);}),
_tag_dyneither(_tmp845,sizeof(void*),1));});});goto _LL444;_LL44B: if((_tmp836.QualId_tok).tag
!= 5)goto _LL44D;_tmp83B=(struct _tuple1*)(_tmp836.QualId_tok).val;_tmp83C=*
_tmp83B;_tmp83D=_tmp83C.f1;_tmp83E=_tmp83C.f2;_LL44C: {struct Cyc_List_List*
_tmp848=0;{union Cyc_Absyn_Nmspace _tmp849=_tmp83D;struct Cyc_List_List*_tmp84A;
struct Cyc_List_List*_tmp84B;int _tmp84C;_LL450: if((_tmp849.Rel_n).tag != 1)goto
_LL452;_tmp84A=(struct Cyc_List_List*)(_tmp849.Rel_n).val;_LL451: _tmp848=_tmp84A;
goto _LL44F;_LL452: if((_tmp849.Abs_n).tag != 2)goto _LL454;_tmp84B=(struct Cyc_List_List*)(
_tmp849.Abs_n).val;_LL453: _tmp848=_tmp84B;goto _LL44F;_LL454: if((_tmp849.Loc_n).tag
!= 3)goto _LL44F;_tmp84C=(int)(_tmp849.Loc_n).val;_LL455: goto _LL44F;_LL44F:;}for(
0;_tmp848 != 0;_tmp848=_tmp848->tl){({struct Cyc_String_pa_struct _tmp84F;_tmp84F.tag=
0;_tmp84F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct
_dyneither_ptr*)_tmp848->hd));({void*_tmp84D[1]={& _tmp84F};Cyc_fprintf(Cyc_stderr,({
const char*_tmp84E="%s::";_tag_dyneither(_tmp84E,sizeof(char),5);}),
_tag_dyneither(_tmp84D,sizeof(void*),1));});});}({struct Cyc_String_pa_struct
_tmp852;_tmp852.tag=0;_tmp852.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp83E);({void*_tmp850[1]={& _tmp852};Cyc_fprintf(Cyc_stderr,({const char*_tmp851="%s::";
_tag_dyneither(_tmp851,sizeof(char),5);}),_tag_dyneither(_tmp850,sizeof(void*),1));});});
goto _LL444;}_LL44D:;_LL44E:({void*_tmp853=0;Cyc_fprintf(Cyc_stderr,({const char*
_tmp854="?";_tag_dyneither(_tmp854,sizeof(char),2);}),_tag_dyneither(_tmp853,
sizeof(void*),0));});goto _LL444;_LL444:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(
f));return Cyc_Parse_parse_result;}
