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
 struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_list(struct _dyneither_ptr);extern char Cyc_List_List_mismatch[14];struct
Cyc_List_List_mismatch_struct{char*tag;};void Cyc_List_iter(void(*f)(void*),
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict Cyc_Dict_insert(
struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,
void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Iter_Iter Cyc_Dict_make_iter(
struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct Cyc_Position_Segment;
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
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
char*tag;};int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_empty_effect;struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];
struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
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
struct Cyc_Absyn_Tvar*t,enum Cyc_Absyn_Kind def);enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(
void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_unify(void*,void*);void*
Cyc_Tcutil_normalize_effect(void*e);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);static struct
_dyneither_ptr Cyc_RgnOrder_tvar2string(struct Cyc_Absyn_Tvar*t){return*t->name;}
static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn);static void*Cyc_RgnOrder_filter_effect(
void*eff,enum Cyc_Absyn_Kind k);struct Cyc_RgnOrder_RgnInfo{void*these_outlive;int
resetable;};struct Cyc_RgnOrder_RgnPO{struct Cyc_Dict_Dict d;void*
these_outlive_heap;void*these_outlive_unique;struct Cyc_Absyn_Tvar*
youngest_unresettable;};static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn){
enum Cyc_Absyn_Kind _tmp0=Cyc_Tcutil_typ_kind(rgn);void*_tmp1=Cyc_Tcutil_compress(
eff);struct Cyc_List_List*_tmp3;void*_tmp7;void*_tmp9;_LL1: {struct Cyc_Absyn_JoinEff_struct*
_tmp2=(struct Cyc_Absyn_JoinEff_struct*)_tmp1;if(_tmp2->tag != 24)goto _LL3;else{
_tmp3=_tmp2->f1;}}_LL2: for(0;_tmp3 != 0;_tmp3=_tmp3->tl){if(!Cyc_RgnOrder_valid_constraint((
void*)_tmp3->hd,rgn))return 0;}return 1;_LL3: {struct Cyc_Absyn_Evar_struct*_tmp4=(
struct Cyc_Absyn_Evar_struct*)_tmp1;if(_tmp4->tag != 1)goto _LL5;}_LL4: goto _LL6;
_LL5: {struct Cyc_Absyn_VarType_struct*_tmp5=(struct Cyc_Absyn_VarType_struct*)
_tmp1;if(_tmp5->tag != 2)goto _LL7;}_LL6: return 1;_LL7: {struct Cyc_Absyn_RgnsEff_struct*
_tmp6=(struct Cyc_Absyn_RgnsEff_struct*)_tmp1;if(_tmp6->tag != 25)goto _LL9;else{
_tmp7=(void*)_tmp6->f1;}}_LL8: if(_tmp0 == Cyc_Absyn_RgnKind)return 1;else{return 0;}
_LL9: {struct Cyc_Absyn_AccessEff_struct*_tmp8=(struct Cyc_Absyn_AccessEff_struct*)
_tmp1;if(_tmp8->tag != 23)goto _LLB;else{_tmp9=(void*)_tmp8->f1;}}_LLA: {enum Cyc_Absyn_Kind
_tmpA=Cyc_Tcutil_typ_kind(_tmp9);return _tmp0 == Cyc_Absyn_RgnKind  || _tmpA == 
_tmp0;}_LLB:;_LLC:({struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(eff));{void*_tmpB[1]={&
_tmpD};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpC="bad effect |%s| passed to add_outlives_constraint";
_tag_dyneither(_tmpC,sizeof(char),50);}),_tag_dyneither(_tmpB,sizeof(void*),1));}});
_LL0:;}static void*Cyc_RgnOrder_filter_effect(void*eff,enum Cyc_Absyn_Kind k){void*
_tmpE=Cyc_Tcutil_compress(eff);void*_tmpF=_tmpE;struct Cyc_List_List*_tmp11;void*
_tmp15;_LLE: {struct Cyc_Absyn_JoinEff_struct*_tmp10=(struct Cyc_Absyn_JoinEff_struct*)
_tmpF;if(_tmp10->tag != 24)goto _LL10;else{_tmp11=_tmp10->f1;}}_LLF: {struct Cyc_List_List*
effs=0;for(0;_tmp11 != 0;_tmp11=_tmp11->tl){void*_tmp16=Cyc_RgnOrder_filter_effect((
void*)_tmp11->hd,k);if(_tmp16 != (void*)& Cyc_Absyn_VoidType_val)effs=({struct Cyc_List_List*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=_tmp16;_tmp17->tl=effs;_tmp17;});}
return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp18=_cycalloc(sizeof(*_tmp18));
_tmp18[0]=({struct Cyc_Absyn_JoinEff_struct _tmp19;_tmp19.tag=24;_tmp19.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effs);_tmp19;});
_tmp18;});}_LL10: {struct Cyc_Absyn_Evar_struct*_tmp12=(struct Cyc_Absyn_Evar_struct*)
_tmpF;if(_tmp12->tag != 1)goto _LL12;}_LL11: goto _LL13;_LL12: {struct Cyc_Absyn_RgnsEff_struct*
_tmp13=(struct Cyc_Absyn_RgnsEff_struct*)_tmpF;if(_tmp13->tag != 25)goto _LL14;}
_LL13: return _tmpE;_LL14: {struct Cyc_Absyn_AccessEff_struct*_tmp14=(struct Cyc_Absyn_AccessEff_struct*)
_tmpF;if(_tmp14->tag != 23)goto _LL16;else{_tmp15=(void*)_tmp14->f1;}}_LL15: if(Cyc_Tcutil_typ_kind(
_tmp15)!= k)return _tmpE;else{return(void*)& Cyc_Absyn_VoidType_val;}_LL16:;_LL17:({
void*_tmp1A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1B="bad effect passed to filter_effect";_tag_dyneither(_tmp1B,
sizeof(char),35);}),_tag_dyneither(_tmp1A,sizeof(void*),0));});_LLD:;}struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*r,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc){eff=Cyc_Tcutil_normalize_effect(
eff);{struct Cyc_RgnOrder_RgnPO*_tmp1C=({struct Cyc_RgnOrder_RgnPO*_tmp34=
_region_malloc(r,sizeof(*_tmp34));_tmp34[0]=*po;_tmp34;});if(!Cyc_RgnOrder_valid_constraint(
eff,rgn)){({void*_tmp1D=0;Cyc_Tcutil_terr(loc,({const char*_tmp1E="Invalid region ordering constraint; kind mismatch";
_tag_dyneither(_tmp1E,sizeof(char),50);}),_tag_dyneither(_tmp1D,sizeof(void*),0));});
return _tmp1C;}{void*_tmp1F=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp21;
_LL19: {struct Cyc_Absyn_VarType_struct*_tmp20=(struct Cyc_Absyn_VarType_struct*)
_tmp1F;if(_tmp20->tag != 2)goto _LL1B;else{_tmp21=_tmp20->f1;}}_LL1A: {struct Cyc_Dict_Dict
_tmp25=po->d;if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(
_tmp25,_tmp21)){void*_tmp26=(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict
d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(_tmp25,_tmp21))->these_outlive;_tmp25=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*
v))Cyc_Dict_insert)(_tmp25,_tmp21,({struct Cyc_RgnOrder_RgnInfo*_tmp27=
_region_malloc(r,sizeof(*_tmp27));_tmp27->these_outlive=(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp29;_tmp29.tag=24;_tmp29.f1=({void*_tmp2A[2];_tmp2A[1]=_tmp26;_tmp2A[0]=eff;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp2A,sizeof(void*),2));});_tmp29;});_tmp28;});_tmp27->resetable=0;_tmp27;}));}
else{_tmp25=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,
struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(_tmp25,_tmp21,({struct Cyc_RgnOrder_RgnInfo*
_tmp2B=_region_malloc(r,sizeof(*_tmp2B));_tmp2B->these_outlive=eff;_tmp2B->resetable=
0;_tmp2B;}));}_tmp1C->d=_tmp25;return _tmp1C;}_LL1B: {struct Cyc_Absyn_Evar_struct*
_tmp22=(struct Cyc_Absyn_Evar_struct*)_tmp1F;if(_tmp22->tag != 1)goto _LL1D;}_LL1C:
Cyc_Tcutil_unify(rgn,(void*)& Cyc_Absyn_HeapRgn_val);goto _LL1E;_LL1D: {struct Cyc_Absyn_HeapRgn_struct*
_tmp23=(struct Cyc_Absyn_HeapRgn_struct*)_tmp1F;if(_tmp23->tag != 21)goto _LL1F;}
_LL1E: _tmp1C->these_outlive_heap=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp2C=
_cycalloc(sizeof(*_tmp2C));_tmp2C[0]=({struct Cyc_Absyn_JoinEff_struct _tmp2D;
_tmp2D.tag=24;_tmp2D.f1=({void*_tmp2E[2];_tmp2E[1]=po->these_outlive_heap;_tmp2E[
0]=eff;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp2E,sizeof(void*),2));});_tmp2D;});_tmp2C;});return _tmp1C;
_LL1F: {struct Cyc_Absyn_UniqueRgn_struct*_tmp24=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp1F;if(_tmp24->tag != 22)goto _LL21;}_LL20: _tmp1C->these_outlive_unique=(void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=({
struct Cyc_Absyn_JoinEff_struct _tmp30;_tmp30.tag=24;_tmp30.f1=({void*_tmp31[2];
_tmp31[1]=po->these_outlive_unique;_tmp31[0]=eff;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp31,sizeof(void*),2));});
_tmp30;});_tmp2F;});return _tmp1C;_LL21:;_LL22:({void*_tmp32=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp33="RgnOrder::add_outlives_constraint passed a bad region";
_tag_dyneither(_tmp33,sizeof(char),54);}),_tag_dyneither(_tmp32,sizeof(void*),0));});
_LL18:;}}}struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*
r,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resettable,int opened){
if(!opened  && ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_member)(
po->d,rgn))({void*_tmp35=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp36="RgnOrder::add_youngest: repeated region";
_tag_dyneither(_tmp36,sizeof(char),40);}),_tag_dyneither(_tmp35,sizeof(void*),0));});{
struct Cyc_RgnOrder_RgnPO*_tmp37=({struct Cyc_RgnOrder_RgnPO*_tmp3E=_region_malloc(
r,sizeof(*_tmp3E));_tmp3E[0]=*po;_tmp3E;});struct Cyc_Absyn_AccessEff_struct*
_tmp38=({struct Cyc_Absyn_AccessEff_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));
_tmp3A[0]=({struct Cyc_Absyn_AccessEff_struct _tmp3B;_tmp3B.tag=23;_tmp3B.f1=(void*)((
void*)({struct Cyc_Absyn_VarType_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[
0]=({struct Cyc_Absyn_VarType_struct _tmp3D;_tmp3D.tag=2;_tmp3D.f1=po->youngest_unresettable;
_tmp3D;});_tmp3C;}));_tmp3B;});_tmp3A;});_tmp37->d=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(
po->d,rgn,({struct Cyc_RgnOrder_RgnInfo*_tmp39=_region_malloc(r,sizeof(*_tmp39));
_tmp39->these_outlive=(void*)_tmp38;_tmp39->resetable=resettable;_tmp39;}));if(!
resettable);_tmp37->youngest_unresettable=rgn;return _tmp37;}}struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*
loc){struct Cyc_Dict_Dict d=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*)))Cyc_Dict_rempty)(r,Cyc_Absyn_tvar_cmp);{
struct Cyc_List_List*_tmp3F=tvs;for(0;_tmp3F != 0;_tmp3F=_tmp3F->tl){switch(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp3F->hd,Cyc_Absyn_BoxKind)){case Cyc_Absyn_RgnKind: _LL23:
goto _LL24;case Cyc_Absyn_UniqueRgnKind: _LL24: goto _LL25;case Cyc_Absyn_TopRgnKind:
_LL25: d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,
struct Cyc_RgnOrder_RgnInfo*v))Cyc_Dict_insert)(d,(struct Cyc_Absyn_Tvar*)_tmp3F->hd,({
struct Cyc_RgnOrder_RgnInfo*_tmp40=_region_malloc(r,sizeof(*_tmp40));_tmp40->these_outlive=
Cyc_Absyn_empty_effect;_tmp40->resetable=0;_tmp40;}));break;break;default: _LL26:
break;}}}if(!Cyc_RgnOrder_valid_constraint(effect,(void*)({struct Cyc_Absyn_VarType_struct*
_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41[0]=({struct Cyc_Absyn_VarType_struct
_tmp42;_tmp42.tag=2;_tmp42.f1=fst_rgn;_tmp42;});_tmp41;}))){({void*_tmp43=0;Cyc_Tcutil_terr(
loc,({const char*_tmp44="Invalid region ordering constraint; kind mismatch";
_tag_dyneither(_tmp44,sizeof(char),50);}),_tag_dyneither(_tmp43,sizeof(void*),0));});
return({struct Cyc_RgnOrder_RgnPO*_tmp45=_region_malloc(r,sizeof(*_tmp45));_tmp45->d=
d;_tmp45->these_outlive_heap=Cyc_Absyn_empty_effect;_tmp45->these_outlive_unique=
Cyc_Absyn_empty_effect;_tmp45->youngest_unresettable=fst_rgn;_tmp45;});}d=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k,struct Cyc_RgnOrder_RgnInfo*
v))Cyc_Dict_insert)(d,fst_rgn,({struct Cyc_RgnOrder_RgnInfo*_tmp46=_region_malloc(
r,sizeof(*_tmp46));_tmp46->these_outlive=effect;_tmp46->resetable=0;_tmp46;}));{
struct Cyc_RgnOrder_RgnPO*_tmp47=({struct Cyc_RgnOrder_RgnPO*_tmp48=_region_malloc(
r,sizeof(*_tmp48));_tmp48->d=d;_tmp48->these_outlive_heap=Cyc_Absyn_empty_effect;
_tmp48->these_outlive_unique=Cyc_Absyn_empty_effect;_tmp48->youngest_unresettable=
fst_rgn;_tmp48;});for(0;po != 0;po=po->tl){_tmp47=Cyc_RgnOrder_add_outlives_constraint(
r,_tmp47,(*((struct _tuple0*)po->hd)).f1,(*((struct _tuple0*)po->hd)).f2,loc);}
return _tmp47;}}int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*r){return(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict
d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,r))->resetable;}static int Cyc_RgnOrder_contains_rgnseff(
struct Cyc_Absyn_Tvar*rgns_of_var,void*eff){void*_tmp49=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(eff));void*_tmp4B;struct Cyc_List_List*_tmp4D;_LL29: {struct
Cyc_Absyn_RgnsEff_struct*_tmp4A=(struct Cyc_Absyn_RgnsEff_struct*)_tmp49;if(
_tmp4A->tag != 25)goto _LL2B;else{_tmp4B=(void*)_tmp4A->f1;}}_LL2A: {void*_tmp4E=
Cyc_Tcutil_compress(_tmp4B);struct Cyc_Absyn_Tvar*_tmp50;_LL30: {struct Cyc_Absyn_VarType_struct*
_tmp4F=(struct Cyc_Absyn_VarType_struct*)_tmp4E;if(_tmp4F->tag != 2)goto _LL32;
else{_tmp50=_tmp4F->f1;}}_LL31: return Cyc_Absyn_tvar_cmp(_tmp50,rgns_of_var)== 0;
_LL32:;_LL33: return 0;_LL2F:;}_LL2B: {struct Cyc_Absyn_JoinEff_struct*_tmp4C=(
struct Cyc_Absyn_JoinEff_struct*)_tmp49;if(_tmp4C->tag != 24)goto _LL2D;else{_tmp4D=
_tmp4C->f1;}}_LL2C: for(0;_tmp4D != 0;_tmp4D=_tmp4D->tl){if(Cyc_RgnOrder_contains_rgnseff(
rgns_of_var,(void*)_tmp4D->hd))return 1;}return 0;_LL2D:;_LL2E: return 0;_LL28:;}
struct Cyc_RgnOrder_OutlivesEnv{struct _RegionHandle*r;struct Cyc_List_List*seen;
struct Cyc_List_List*todo;};static void Cyc_RgnOrder_add_to_search(struct Cyc_RgnOrder_OutlivesEnv*
env,void*eff){void*_tmp51=Cyc_Tcutil_compress(eff);void*_tmp53;struct Cyc_Absyn_Tvar*
_tmp55;struct Cyc_List_List*_tmp57;_LL35: {struct Cyc_Absyn_AccessEff_struct*
_tmp52=(struct Cyc_Absyn_AccessEff_struct*)_tmp51;if(_tmp52->tag != 23)goto _LL37;
else{_tmp53=(void*)_tmp52->f1;{struct Cyc_Absyn_VarType_struct*_tmp54=(struct Cyc_Absyn_VarType_struct*)
_tmp53;if(_tmp54->tag != 2)goto _LL37;else{_tmp55=_tmp54->f1;}}}}_LL36:{struct Cyc_List_List*
_tmp58=env->seen;for(0;_tmp58 != 0;_tmp58=_tmp58->tl){if(Cyc_Absyn_tvar_cmp(
_tmp55,(struct Cyc_Absyn_Tvar*)_tmp58->hd)== 0)return;}}env->seen=({struct Cyc_List_List*
_tmp59=_region_malloc(env->r,sizeof(*_tmp59));_tmp59->hd=_tmp55;_tmp59->tl=env->seen;
_tmp59;});env->todo=({struct Cyc_List_List*_tmp5A=_region_malloc(env->r,sizeof(*
_tmp5A));_tmp5A->hd=_tmp55;_tmp5A->tl=env->todo;_tmp5A;});return;_LL37: {struct
Cyc_Absyn_JoinEff_struct*_tmp56=(struct Cyc_Absyn_JoinEff_struct*)_tmp51;if(
_tmp56->tag != 24)goto _LL39;else{_tmp57=_tmp56->f1;}}_LL38: for(0;_tmp57 != 0;
_tmp57=_tmp57->tl){Cyc_RgnOrder_add_to_search(env,(void*)_tmp57->hd);}return;
_LL39:;_LL3A: return;_LL34:;}static struct Cyc_RgnOrder_OutlivesEnv Cyc_RgnOrder_initial_env(
struct _RegionHandle*listrgn,struct Cyc_RgnOrder_RgnPO*po,void*rgn){struct Cyc_RgnOrder_OutlivesEnv
_tmp5B=({struct Cyc_RgnOrder_OutlivesEnv _tmp63;_tmp63.r=listrgn;_tmp63.seen=0;
_tmp63.todo=0;_tmp63;});void*r=Cyc_Tcutil_compress(rgn);switch(Cyc_Tcutil_typ_kind(
r)){case Cyc_Absyn_RgnKind: _LL3B: Cyc_RgnOrder_add_to_search(& _tmp5B,po->these_outlive_heap);
break;case Cyc_Absyn_UniqueRgnKind: _LL3C: Cyc_RgnOrder_add_to_search(& _tmp5B,po->these_outlive_unique);
break;case Cyc_Absyn_TopRgnKind: _LL3D: Cyc_RgnOrder_add_to_search(& _tmp5B,po->these_outlive_heap);
Cyc_RgnOrder_add_to_search(& _tmp5B,po->these_outlive_unique);break;default: _LL3E:({
void*_tmp5C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp5D="RgnOrder: rgn not of correct kind";_tag_dyneither(_tmp5D,
sizeof(char),34);}),_tag_dyneither(_tmp5C,sizeof(void*),0));});}{void*_tmp5E=r;
struct Cyc_Absyn_Tvar*_tmp60;_LL41: {struct Cyc_Absyn_VarType_struct*_tmp5F=(
struct Cyc_Absyn_VarType_struct*)_tmp5E;if(_tmp5F->tag != 2)goto _LL43;else{_tmp60=
_tmp5F->f1;}}_LL42: _tmp5B.seen=({struct Cyc_List_List*_tmp61=_region_malloc(
listrgn,sizeof(*_tmp61));_tmp61->hd=_tmp60;_tmp61->tl=_tmp5B.seen;_tmp61;});
_tmp5B.todo=({struct Cyc_List_List*_tmp62=_region_malloc(listrgn,sizeof(*_tmp62));
_tmp62->hd=_tmp60;_tmp62->tl=_tmp5B.todo;_tmp62;});goto _LL44;_LL43:;_LL44: return
_tmp5B;_LL40:;}}static int Cyc_RgnOrder_atomic_effect_outlives(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn){struct _RegionHandle _tmp64=_new_region("listrgn");struct
_RegionHandle*listrgn=& _tmp64;_push_region(listrgn);{void*_tmp65=Cyc_Tcutil_compress(
eff);void*_tmp67;void*_tmp69;_LL46: {struct Cyc_Absyn_AccessEff_struct*_tmp66=(
struct Cyc_Absyn_AccessEff_struct*)_tmp65;if(_tmp66->tag != 23)goto _LL48;else{
_tmp67=(void*)_tmp66->f1;}}_LL47: {void*_tmp6A=Cyc_Tcutil_compress(_tmp67);
struct Cyc_Absyn_Tvar*_tmp6E;_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*_tmp6B=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp6A;if(_tmp6B->tag != 22)goto _LL4F;}_LL4E: {
int _tmp6F=1;_npop_handler(0);return _tmp6F;}_LL4F: {struct Cyc_Absyn_HeapRgn_struct*
_tmp6C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp6A;if(_tmp6C->tag != 21)goto _LL51;}
_LL50: {void*_tmp70=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*_tmp73;_LL56: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp71=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp70;if(_tmp71->tag != 22)goto _LL58;}_LL57: {int _tmp74=0;_npop_handler(0);
return _tmp74;}_LL58: {struct Cyc_Absyn_VarType_struct*_tmp72=(struct Cyc_Absyn_VarType_struct*)
_tmp70;if(_tmp72->tag != 2)goto _LL5A;else{_tmp73=_tmp72->f1;}}_LL59: if(Cyc_Tcutil_tvar_kind(
_tmp73,Cyc_Absyn_BoxKind)== Cyc_Absyn_RgnKind){int _tmp75=1;_npop_handler(0);
return _tmp75;}else{int _tmp76=0;_npop_handler(0);return _tmp76;}_LL5A:;_LL5B: {int
_tmp77=1;_npop_handler(0);return _tmp77;}_LL55:;}_LL51: {struct Cyc_Absyn_VarType_struct*
_tmp6D=(struct Cyc_Absyn_VarType_struct*)_tmp6A;if(_tmp6D->tag != 2)goto _LL53;
else{_tmp6E=_tmp6D->f1;}}_LL52: {struct Cyc_RgnOrder_OutlivesEnv _tmp78=Cyc_RgnOrder_initial_env(
listrgn,po,rgn);while(_tmp78.todo != 0){struct Cyc_Absyn_Tvar*_tmp79=(struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp78.todo))->hd;_tmp78.todo=((struct Cyc_List_List*)
_check_null(_tmp78.todo))->tl;if(Cyc_Absyn_tvar_cmp(_tmp79,_tmp6E)== 0){int
_tmp7A=1;_npop_handler(0);return _tmp7A;}if(((int(*)(struct Cyc_Dict_Dict d,struct
Cyc_Absyn_Tvar*k))Cyc_Dict_member)(po->d,_tmp79))Cyc_RgnOrder_add_to_search(&
_tmp78,(((struct Cyc_RgnOrder_RgnInfo*(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*
k))Cyc_Dict_lookup)(po->d,_tmp79))->these_outlive);else{({void*_tmp7B=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp7C="RgnOrder: type variable not in dict";_tag_dyneither(_tmp7C,sizeof(char),
36);}),_tag_dyneither(_tmp7B,sizeof(void*),0));});}}{int _tmp7D=0;_npop_handler(0);
return _tmp7D;}}_LL53:;_LL54: {int _tmp7E=0;_npop_handler(0);return _tmp7E;}_LL4C:;}
_LL48: {struct Cyc_Absyn_RgnsEff_struct*_tmp68=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp65;if(_tmp68->tag != 25)goto _LL4A;else{_tmp69=(void*)_tmp68->f1;}}_LL49: {
void*_tmp7F=Cyc_Tcutil_compress(_tmp69);struct Cyc_Absyn_Tvar*_tmp81;_LL5D: {
struct Cyc_Absyn_VarType_struct*_tmp80=(struct Cyc_Absyn_VarType_struct*)_tmp7F;
if(_tmp80->tag != 2)goto _LL5F;else{_tmp81=_tmp80->f1;}}_LL5E: {struct Cyc_RgnOrder_OutlivesEnv
_tmp82=Cyc_RgnOrder_initial_env(listrgn,po,rgn);enum Cyc_Absyn_Kind _tmp83=Cyc_Tcutil_typ_kind(
rgn);if(_tmp83 == Cyc_Absyn_RgnKind  || _tmp83 == Cyc_Absyn_TopRgnKind){if(Cyc_RgnOrder_contains_rgnseff(
_tmp81,po->these_outlive_heap)){int _tmp84=1;_npop_handler(0);return _tmp84;}}if(
_tmp83 == Cyc_Absyn_UniqueRgnKind  || _tmp83 == Cyc_Absyn_TopRgnKind){if(Cyc_RgnOrder_contains_rgnseff(
_tmp81,po->these_outlive_unique)){int _tmp85=1;_npop_handler(0);return _tmp85;}}
while(_tmp82.todo != 0){struct Cyc_Absyn_Tvar*_tmp86=(struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp82.todo))->hd;_tmp82.todo=((struct Cyc_List_List*)
_check_null(_tmp82.todo))->tl;if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*
k))Cyc_Dict_member)(po->d,_tmp86)){void*_tmp87=(((struct Cyc_RgnOrder_RgnInfo*(*)(
struct Cyc_Dict_Dict d,struct Cyc_Absyn_Tvar*k))Cyc_Dict_lookup)(po->d,_tmp86))->these_outlive;
if(Cyc_RgnOrder_contains_rgnseff(_tmp81,_tmp87)){int _tmp88=1;_npop_handler(0);
return _tmp88;}Cyc_RgnOrder_add_to_search(& _tmp82,_tmp87);}else{({void*_tmp89=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp8A="RgnOrder: type variable not in dict";_tag_dyneither(_tmp8A,sizeof(
char),36);}),_tag_dyneither(_tmp89,sizeof(void*),0));});}}{int _tmp8B=0;
_npop_handler(0);return _tmp8B;}}_LL5F:;_LL60: {int _tmp8C=0;_npop_handler(0);
return _tmp8C;}_LL5C:;}_LL4A:;_LL4B: {int _tmp8D=0;_npop_handler(0);return _tmp8D;}
_LL45:;};_pop_region(listrgn);}int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn){eff=Cyc_Tcutil_normalize_effect(eff);{void*_tmp8E=Cyc_Tcutil_compress(
eff);struct Cyc_List_List*_tmp90;_LL62: {struct Cyc_Absyn_JoinEff_struct*_tmp8F=(
struct Cyc_Absyn_JoinEff_struct*)_tmp8E;if(_tmp8F->tag != 24)goto _LL64;else{_tmp90=
_tmp8F->f1;}}_LL63: for(0;_tmp90 != 0;_tmp90=_tmp90->tl){if(!Cyc_RgnOrder_effect_outlives(
po,(void*)_tmp90->hd,rgn))return 0;}return 1;_LL64:;_LL65: return Cyc_RgnOrder_atomic_effect_outlives(
po,eff,rgn);_LL61:;}}static void Cyc_RgnOrder_pin_effect(void*eff,void*bd){eff=Cyc_Tcutil_normalize_effect(
eff);{void*_tmp91=Cyc_Tcutil_compress(eff);struct Cyc_List_List*_tmp93;void*
_tmp95;void*_tmp97;_LL67: {struct Cyc_Absyn_JoinEff_struct*_tmp92=(struct Cyc_Absyn_JoinEff_struct*)
_tmp91;if(_tmp92->tag != 24)goto _LL69;else{_tmp93=_tmp92->f1;}}_LL68: for(0;_tmp93
!= 0;_tmp93=_tmp93->tl){Cyc_RgnOrder_pin_effect((void*)_tmp93->hd,bd);}return;
_LL69: {struct Cyc_Absyn_RgnsEff_struct*_tmp94=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp91;if(_tmp94->tag != 25)goto _LL6B;else{_tmp95=(void*)_tmp94->f1;}}_LL6A: {
void*_tmp99=Cyc_Tcutil_compress(_tmp95);_LL72: {struct Cyc_Absyn_Evar_struct*
_tmp9A=(struct Cyc_Absyn_Evar_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL74;}_LL73:
Cyc_Tcutil_unify(_tmp95,Cyc_Absyn_uint_typ);return;_LL74:;_LL75: return;_LL71:;}
_LL6B: {struct Cyc_Absyn_AccessEff_struct*_tmp96=(struct Cyc_Absyn_AccessEff_struct*)
_tmp91;if(_tmp96->tag != 23)goto _LL6D;else{_tmp97=(void*)_tmp96->f1;}}_LL6C: {
void*_tmp9B=Cyc_Tcutil_compress(_tmp97);_LL77: {struct Cyc_Absyn_Evar_struct*
_tmp9C=(struct Cyc_Absyn_Evar_struct*)_tmp9B;if(_tmp9C->tag != 1)goto _LL79;}_LL78:
Cyc_Tcutil_unify(_tmp97,bd);return;_LL79:;_LL7A: return;_LL76:;}_LL6D: {struct Cyc_Absyn_Evar_struct*
_tmp98=(struct Cyc_Absyn_Evar_struct*)_tmp91;if(_tmp98->tag != 1)goto _LL6F;}_LL6E:
Cyc_Tcutil_unify(eff,Cyc_Absyn_empty_effect);return;_LL6F:;_LL70: return;_LL66:;}}
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin){{struct Cyc_List_List*_tmp9D=
constraints;for(0;_tmp9D != 0;_tmp9D=_tmp9D->tl){struct _tuple0 _tmp9F;void*_tmpA0;
struct _tuple0*_tmp9E=(struct _tuple0*)_tmp9D->hd;_tmp9F=*_tmp9E;_tmpA0=_tmp9F.f2;{
void*_tmpA1=Cyc_Tcutil_compress(_tmpA0);_LL7C: {struct Cyc_Absyn_Evar_struct*
_tmpA2=(struct Cyc_Absyn_Evar_struct*)_tmpA1;if(_tmpA2->tag != 1)goto _LL7E;}_LL7D:
if(do_pin)Cyc_Tcutil_unify(_tmpA0,default_bound);goto _LL7B;_LL7E:;_LL7F: goto
_LL7B;_LL7B:;}}}{struct Cyc_List_List*_tmpA3=constraints;for(0;_tmpA3 != 0;_tmpA3=
_tmpA3->tl){struct _tuple0 _tmpA5;void*_tmpA6;void*_tmpA7;struct _tuple0*_tmpA4=(
struct _tuple0*)_tmpA3->hd;_tmpA5=*_tmpA4;_tmpA6=_tmpA5.f1;_tmpA7=_tmpA5.f2;if(
do_pin)Cyc_RgnOrder_pin_effect(_tmpA6,_tmpA7);if(!Cyc_RgnOrder_effect_outlives(
po,_tmpA6,_tmpA7))return 0;}}return 1;}static int Cyc_RgnOrder_eff_outlives_atomic_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){eff2=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
eff2));{void*_tmpA8=eff2;struct Cyc_List_List*_tmpAA;void*_tmpAC;void*_tmpAE;
_LL81: {struct Cyc_Absyn_JoinEff_struct*_tmpA9=(struct Cyc_Absyn_JoinEff_struct*)
_tmpA8;if(_tmpA9->tag != 24)goto _LL83;else{_tmpAA=_tmpA9->f1;}}_LL82: for(0;_tmpAA
!= 0;_tmpAA=_tmpAA->tl){if(Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,(void*)
_tmpAA->hd))return 1;}return 0;_LL83: {struct Cyc_Absyn_AccessEff_struct*_tmpAB=(
struct Cyc_Absyn_AccessEff_struct*)_tmpA8;if(_tmpAB->tag != 23)goto _LL85;else{
_tmpAC=(void*)_tmpAB->f1;}}_LL84: return Cyc_RgnOrder_effect_outlives(po,eff1,
_tmpAC);_LL85: {struct Cyc_Absyn_RgnsEff_struct*_tmpAD=(struct Cyc_Absyn_RgnsEff_struct*)
_tmpA8;if(_tmpAD->tag != 25)goto _LL87;else{_tmpAE=(void*)_tmpAD->f1;}}_LL86: {
void*_tmpAF=eff1;void*_tmpB1;_LL8A: {struct Cyc_Absyn_RgnsEff_struct*_tmpB0=(
struct Cyc_Absyn_RgnsEff_struct*)_tmpAF;if(_tmpB0->tag != 25)goto _LL8C;else{_tmpB1=(
void*)_tmpB0->f1;}}_LL8B:{struct _tuple0 _tmpB3=({struct _tuple0 _tmpB2;_tmpB2.f1=
Cyc_Tcutil_compress(_tmpAE);_tmpB2.f2=Cyc_Tcutil_compress(_tmpB1);_tmpB2;});void*
_tmpB4;struct Cyc_Absyn_Tvar*_tmpB6;void*_tmpB7;struct Cyc_Absyn_Tvar*_tmpB9;_LL8F:
_tmpB4=_tmpB3.f1;{struct Cyc_Absyn_VarType_struct*_tmpB5=(struct Cyc_Absyn_VarType_struct*)
_tmpB4;if(_tmpB5->tag != 2)goto _LL91;else{_tmpB6=_tmpB5->f1;}}_tmpB7=_tmpB3.f2;{
struct Cyc_Absyn_VarType_struct*_tmpB8=(struct Cyc_Absyn_VarType_struct*)_tmpB7;
if(_tmpB8->tag != 2)goto _LL91;else{_tmpB9=_tmpB8->f1;}}_LL90: return Cyc_Absyn_tvar_cmp(
_tmpB6,_tmpB9)== 0;_LL91:;_LL92: goto _LL8E;_LL8E:;}goto _LL8D;_LL8C:;_LL8D: return
eff1 == (void*)& Cyc_Absyn_HeapRgn_val;_LL89:;}_LL87:;_LL88: return eff1 == (void*)&
Cyc_Absyn_HeapRgn_val;_LL80:;}}int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*
po,void*eff1,void*eff2){eff1=Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
eff1));{void*_tmpBA=eff1;struct Cyc_List_List*_tmpBC;_LL94: {struct Cyc_Absyn_JoinEff_struct*
_tmpBB=(struct Cyc_Absyn_JoinEff_struct*)_tmpBA;if(_tmpBB->tag != 24)goto _LL96;
else{_tmpBC=_tmpBB->f1;}}_LL95: for(0;_tmpBC != 0;_tmpBC=_tmpBC->tl){if(!Cyc_RgnOrder_eff_outlives_eff(
po,(void*)_tmpBC->hd,eff2))return 0;}return 1;_LL96:;_LL97: return Cyc_RgnOrder_eff_outlives_atomic_eff(
po,eff1,eff2);_LL93:;}}struct _tuple10{struct Cyc_Absyn_Tvar*f1;struct Cyc_RgnOrder_RgnInfo*
f2;};void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po){struct
_RegionHandle _tmpBD=_new_region("r");struct _RegionHandle*r=& _tmpBD;_push_region(
r);{struct Cyc_Iter_Iter _tmpBE=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,po->d);struct _tuple10 _tmpBF=*((
struct _tuple10*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
r,po->d);({void*_tmpC0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpC1="region po:\n";
_tag_dyneither(_tmpC1,sizeof(char),12);}),_tag_dyneither(_tmpC0,sizeof(void*),0));});
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(_tmpBE,&
_tmpBF)){({struct Cyc_String_pa_struct _tmpC5;_tmpC5.tag=0;_tmpC5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmpBF.f2)->these_outlive));{
struct Cyc_String_pa_struct _tmpC4;_tmpC4.tag=0;_tmpC4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*(_tmpBF.f1)->name);{void*_tmpC2[2]={& _tmpC4,& _tmpC5};Cyc_fprintf(
Cyc_stderr,({const char*_tmpC3="  %s outlived by %s\n";_tag_dyneither(_tmpC3,
sizeof(char),21);}),_tag_dyneither(_tmpC2,sizeof(void*),2));}}});}({struct Cyc_String_pa_struct
_tmpC8;_tmpC8.tag=0;_tmpC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
po->these_outlive_heap));{void*_tmpC6[1]={& _tmpC8};Cyc_fprintf(Cyc_stderr,({
const char*_tmpC7="  these outlive heap: %s\n";_tag_dyneither(_tmpC7,sizeof(char),
26);}),_tag_dyneither(_tmpC6,sizeof(void*),1));}});({struct Cyc_String_pa_struct
_tmpCB;_tmpCB.tag=0;_tmpCB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
po->these_outlive_unique));{void*_tmpC9[1]={& _tmpCB};Cyc_fprintf(Cyc_stderr,({
const char*_tmpCA="  these outlive unique: %s\n";_tag_dyneither(_tmpCA,sizeof(
char),28);}),_tag_dyneither(_tmpC9,sizeof(void*),1));}});};_pop_region(r);}
