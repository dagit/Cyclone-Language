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
tag;struct _dyneither_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*
env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};void Cyc_Position_post_error(struct Cyc_Position_Error*);struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
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
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};extern
struct Cyc_Absyn_Unresolved_b_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{
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
Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);enum Cyc_Absyn_Kind Cyc_Absyn_force_kb(
void*kb);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);extern char Cyc_Tcdecl_Incompatible[
13];struct Cyc_Tcdecl_Incompatible_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{
struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap);struct _tuple9{enum Cyc_Absyn_Scope f1;int f2;};struct
_tuple9 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,
struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg,int allow_externC_extern_merge);struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Datatypedecl*
Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg);struct
Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*
d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct
Cyc_List_List*f,int*res,struct _dyneither_ptr*v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};int Cyc_strptrcmp(struct _dyneither_ptr*s1,
struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};int Cyc_Tcutil_unify_kindbound(
void*,void*);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_same_atts(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple0*);struct _dyneither_ptr Cyc_Absynpp_scope2string(
enum Cyc_Absyn_Scope sc);char Cyc_Tcdecl_Incompatible[13]="Incompatible";struct Cyc_Tcdecl_Incompatible_struct
Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};void Cyc_Tcdecl_merr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){if(msg1 == 0)(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
struct _dyneither_ptr fmt2=(struct _dyneither_ptr)Cyc_strconcat(({const char*_tmp7="%s ";
_tag_dyneither(_tmp7,sizeof(char),4);}),(struct _dyneither_ptr)fmt);struct
_dyneither_ptr ap2=({unsigned int _tmp1=_get_dyneither_size(ap,sizeof(void*))+ 1;
void**_tmp2=(void**)_cycalloc(_check_times(sizeof(void*),_tmp1));struct
_dyneither_ptr _tmp6=_tag_dyneither(_tmp2,sizeof(void*),_tmp1);{unsigned int _tmp3=
_tmp1;unsigned int i;for(i=0;i < _tmp3;i ++){_tmp2[i]=i == 0?(void*)((void*)({struct
Cyc_String_pa_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4[0]=({struct Cyc_String_pa_struct
_tmp5;_tmp5.tag=0;_tmp5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*msg1);
_tmp5;});_tmp4;})):*((void**)_check_dyneither_subscript(ap,sizeof(void*),(int)(i
- 1)));}}_tmp6;});Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct
_dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2)));};}static void Cyc_Tcdecl_merge_scope_err(
enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct
_dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){({
struct Cyc_String_pa_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_scope2string(s0));({struct Cyc_String_pa_struct
_tmpC;_tmpC.tag=0;_tmpC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_scope2string(
s1));({struct Cyc_String_pa_struct _tmpB;_tmpB.tag=0;_tmpB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_struct _tmpA;_tmpA.tag=
0;_tmpA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp8[4]={&
_tmpA,& _tmpB,& _tmpC,& _tmpD};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp9="%s %s is %s whereas expected scope is %s";
_tag_dyneither(_tmp9,sizeof(char),41);}),_tag_dyneither(_tmp8,sizeof(void*),4));});});});});});}
struct _tuple10{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};struct _tuple9 Cyc_Tcdecl_merge_scope(
enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct
_dyneither_ptr v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg,int
externCmerge){{struct _tuple10 _tmpF=({struct _tuple10 _tmpE;_tmpE.f1=s0;_tmpE.f2=s1;
_tmpE;});enum Cyc_Absyn_Scope _tmp10;enum Cyc_Absyn_Scope _tmp11;enum Cyc_Absyn_Scope
_tmp12;enum Cyc_Absyn_Scope _tmp13;enum Cyc_Absyn_Scope _tmp14;enum Cyc_Absyn_Scope
_tmp15;enum Cyc_Absyn_Scope _tmp16;enum Cyc_Absyn_Scope _tmp17;enum Cyc_Absyn_Scope
_tmp18;enum Cyc_Absyn_Scope _tmp19;enum Cyc_Absyn_Scope _tmp1A;enum Cyc_Absyn_Scope
_tmp1B;enum Cyc_Absyn_Scope _tmp1C;enum Cyc_Absyn_Scope _tmp1D;enum Cyc_Absyn_Scope
_tmp1E;enum Cyc_Absyn_Scope _tmp1F;enum Cyc_Absyn_Scope _tmp20;enum Cyc_Absyn_Scope
_tmp21;_LL1: _tmp10=_tmpF.f1;if(_tmp10 != Cyc_Absyn_ExternC)goto _LL3;_tmp11=_tmpF.f2;
if(_tmp11 != Cyc_Absyn_ExternC)goto _LL3;_LL2: goto _LL0;_LL3: _tmp12=_tmpF.f1;if(
_tmp12 != Cyc_Absyn_ExternC)goto _LL5;_tmp13=_tmpF.f2;if(_tmp13 != Cyc_Absyn_Extern)
goto _LL5;_LL4: goto _LL6;_LL5: _tmp14=_tmpF.f1;if(_tmp14 != Cyc_Absyn_Extern)goto
_LL7;_tmp15=_tmpF.f2;if(_tmp15 != Cyc_Absyn_ExternC)goto _LL7;_LL6: if(externCmerge)
goto _LL0;goto _LL8;_LL7: _tmp16=_tmpF.f1;if(_tmp16 != Cyc_Absyn_ExternC)goto _LL9;
_LL8: goto _LLA;_LL9: _tmp17=_tmpF.f2;if(_tmp17 != Cyc_Absyn_ExternC)goto _LLB;_LLA:
Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple9 _tmp22;_tmp22.f1=
s1;_tmp22.f2=0;_tmp22;});_LLB: _tmp18=_tmpF.f2;if(_tmp18 != Cyc_Absyn_Extern)goto
_LLD;_LLC: s1=s0;goto _LL0;_LLD: _tmp19=_tmpF.f1;if(_tmp19 != Cyc_Absyn_Extern)goto
_LLF;_LLE: goto _LL0;_LLF: _tmp1A=_tmpF.f1;if(_tmp1A != Cyc_Absyn_Static)goto _LL11;
_tmp1B=_tmpF.f2;if(_tmp1B != Cyc_Absyn_Static)goto _LL11;_LL10: goto _LL12;_LL11:
_tmp1C=_tmpF.f1;if(_tmp1C != Cyc_Absyn_Public)goto _LL13;_tmp1D=_tmpF.f2;if(_tmp1D
!= Cyc_Absyn_Public)goto _LL13;_LL12: goto _LL14;_LL13: _tmp1E=_tmpF.f1;if(_tmp1E != 
Cyc_Absyn_Abstract)goto _LL15;_tmp1F=_tmpF.f2;if(_tmp1F != Cyc_Absyn_Abstract)goto
_LL15;_LL14: goto _LL0;_LL15: _tmp20=_tmpF.f1;if(_tmp20 != Cyc_Absyn_Register)goto
_LL17;_tmp21=_tmpF.f2;if(_tmp21 != Cyc_Absyn_Register)goto _LL17;_LL16: goto _LL0;
_LL17:;_LL18: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple9
_tmp23;_tmp23.f1=s1;_tmp23.f2=0;_tmp23;});_LL0:;}return({struct _tuple9 _tmp24;
_tmp24.f1=s1;_tmp24.f2=1;_tmp24;});}static int Cyc_Tcdecl_check_type(void*t0,void*
t1){return Cyc_Tcutil_unify(t0,t1);}static unsigned int Cyc_Tcdecl_get_uint_const_value(
struct Cyc_Absyn_Exp*e){void*_tmp25=e->r;union Cyc_Absyn_Cnst _tmp27;struct _tuple5
_tmp28;int _tmp29;_LL1A: {struct Cyc_Absyn_Const_e_struct*_tmp26=(struct Cyc_Absyn_Const_e_struct*)
_tmp25;if(_tmp26->tag != 0)goto _LL1C;else{_tmp27=_tmp26->f1;if((_tmp27.Int_c).tag
!= 4)goto _LL1C;_tmp28=(struct _tuple5)(_tmp27.Int_c).val;_tmp29=_tmp28.f2;}}_LL1B:
return(unsigned int)_tmp29;_LL1C:;_LL1D:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp2B;_tmp2B.tag=Cyc_Core_Invalid_argument;_tmp2B.f1=({const char*_tmp2C="Tcdecl::get_uint_const_value";
_tag_dyneither(_tmp2C,sizeof(char),29);});_tmp2B;});_tmp2A;}));_LL19:;}inline
static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,
struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tvs0)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1)){({struct Cyc_String_pa_struct
_tmp30;_tmp30.tag=0;_tmp30.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({
struct Cyc_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);({void*_tmp2D[2]={& _tmp2F,& _tmp30};Cyc_Tcdecl_merr(loc,
msg,({const char*_tmp2E="%s %s has a different number of type parameters";
_tag_dyneither(_tmp2E,sizeof(char),48);}),_tag_dyneither(_tmp2D,sizeof(void*),2));});});});
return 0;}{struct Cyc_List_List*_tmp31=tvs0;struct Cyc_List_List*_tmp32=tvs1;for(0;
_tmp31 != 0;(_tmp31=_tmp31->tl,_tmp32=_tmp32->tl)){Cyc_Tcutil_unify_kindbound(((
struct Cyc_Absyn_Tvar*)_tmp31->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(_tmp32))->hd)->kind);{enum Cyc_Absyn_Kind _tmp33=Cyc_Absyn_force_kb(((
struct Cyc_Absyn_Tvar*)_tmp31->hd)->kind);enum Cyc_Absyn_Kind _tmp34=Cyc_Absyn_force_kb(((
struct Cyc_Absyn_Tvar*)_tmp32->hd)->kind);if(_tmp33 == _tmp34)continue;({struct Cyc_String_pa_struct
_tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp34));({struct Cyc_String_pa_struct _tmp3A;_tmp3A.tag=0;_tmp3A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp31->hd)->name);({
struct Cyc_String_pa_struct _tmp39;_tmp39.tag=0;_tmp39.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp33));({struct Cyc_String_pa_struct
_tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({
struct Cyc_String_pa_struct _tmp37;_tmp37.tag=0;_tmp37.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);({void*_tmp35[5]={& _tmp37,& _tmp38,& _tmp39,& _tmp3A,&
_tmp3B};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp36="%s %s has a different kind (%s) for type parameter %s (%s)";
_tag_dyneither(_tmp36,sizeof(char),59);}),_tag_dyneither(_tmp35,sizeof(void*),5));});});});});});});
return 0;};}return 1;};}static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,
struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*
loc,struct _dyneither_ptr*msg){if(!Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_String_pa_struct
_tmp3F;_tmp3F.tag=0;_tmp3F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({
struct Cyc_String_pa_struct _tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);({void*_tmp3C[2]={& _tmp3E,& _tmp3F};Cyc_Tcdecl_merr(loc,
msg,({const char*_tmp3D="%s %s has different attributes";_tag_dyneither(_tmp3D,
sizeof(char),31);}),_tag_dyneither(_tmp3C,sizeof(void*),2));});});});return 0;}
return 1;}struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};static struct Cyc_List_List*
Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
struct Cyc_List_List*inst=0;for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){inst=({
struct Cyc_List_List*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->hd=({struct
_tuple11*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->f1=(struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(tvs1))->hd;_tmp41->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42[0]=({struct Cyc_Absyn_VarType_struct
_tmp43;_tmp43.tag=2;_tmp43.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_tmp43;});_tmp42;});
_tmp41;});_tmp40->tl=inst;_tmp40;});}return inst;}struct _tuple12{struct Cyc_Absyn_AggrdeclImpl*
f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple13{void*f1;void*f2;};struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct _dyneither_ptr
_tmp44=Cyc_Absynpp_qvar2string(d0->name);int _tmp45=1;if(!(d0->kind == d1->kind))
return 0;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,({const char*_tmp46="type";
_tag_dyneither(_tmp46,sizeof(char),5);}),_tmp44,loc,msg))return 0;{enum Cyc_Absyn_Scope
_tmp49;int _tmp4A;struct _tuple9 _tmp48=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,({
const char*_tmp47="type";_tag_dyneither(_tmp47,sizeof(char),5);}),_tmp44,loc,msg,
1);_tmp49=_tmp48.f1;_tmp4A=_tmp48.f2;if(!_tmp4A)_tmp45=0;if(!Cyc_Tcdecl_check_atts(
d0->attributes,d1->attributes,({const char*_tmp4B="type";_tag_dyneither(_tmp4B,
sizeof(char),5);}),_tmp44,loc,msg))_tmp45=0;{struct Cyc_Absyn_Aggrdecl*d2;{struct
_tuple12 _tmp4D=({struct _tuple12 _tmp4C;_tmp4C.f1=d0->impl;_tmp4C.f2=d1->impl;
_tmp4C;});struct Cyc_Absyn_AggrdeclImpl*_tmp4E;struct Cyc_Absyn_AggrdeclImpl*
_tmp4F;struct Cyc_Absyn_AggrdeclImpl*_tmp50;struct Cyc_Absyn_AggrdeclImpl _tmp51;
struct Cyc_List_List*_tmp52;struct Cyc_List_List*_tmp53;struct Cyc_List_List*_tmp54;
int _tmp55;struct Cyc_Absyn_AggrdeclImpl*_tmp56;struct Cyc_Absyn_AggrdeclImpl _tmp57;
struct Cyc_List_List*_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;
int _tmp5B;_LL1F: _tmp4E=_tmp4D.f2;if(_tmp4E != 0)goto _LL21;_LL20: d2=d0;goto _LL1E;
_LL21: _tmp4F=_tmp4D.f1;if(_tmp4F != 0)goto _LL23;_LL22: d2=d1;goto _LL1E;_LL23:
_tmp50=_tmp4D.f1;if(_tmp50 == 0)goto _LL1E;_tmp51=*_tmp50;_tmp52=_tmp51.exist_vars;
_tmp53=_tmp51.rgn_po;_tmp54=_tmp51.fields;_tmp55=_tmp51.tagged;_tmp56=_tmp4D.f2;
if(_tmp56 == 0)goto _LL1E;_tmp57=*_tmp56;_tmp58=_tmp57.exist_vars;_tmp59=_tmp57.rgn_po;
_tmp5A=_tmp57.fields;_tmp5B=_tmp57.tagged;_LL24: if(!Cyc_Tcdecl_check_tvs(_tmp52,
_tmp58,({const char*_tmp5C="type";_tag_dyneither(_tmp5C,sizeof(char),5);}),_tmp44,
loc,msg))return 0;{struct Cyc_List_List*_tmp5D=Cyc_Tcdecl_build_tvs_map(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
d0->tvs,_tmp52),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(d1->tvs,_tmp58));for(0;_tmp53 != 0  && _tmp59 != 0;(_tmp53=
_tmp53->tl,_tmp59=_tmp59->tl)){Cyc_Tcdecl_check_type((*((struct _tuple13*)_tmp53->hd)).f1,(*((
struct _tuple13*)_tmp59->hd)).f1);Cyc_Tcdecl_check_type((*((struct _tuple13*)
_tmp53->hd)).f2,(*((struct _tuple13*)_tmp59->hd)).f2);}for(0;_tmp54 != 0  && _tmp5A
!= 0;(_tmp54=_tmp54->tl,_tmp5A=_tmp5A->tl)){struct Cyc_Absyn_Aggrfield _tmp5F;
struct _dyneither_ptr*_tmp60;struct Cyc_Absyn_Tqual _tmp61;void*_tmp62;struct Cyc_Absyn_Exp*
_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_Absyn_Aggrfield*_tmp5E=(struct Cyc_Absyn_Aggrfield*)
_tmp54->hd;_tmp5F=*_tmp5E;_tmp60=_tmp5F.name;_tmp61=_tmp5F.tq;_tmp62=_tmp5F.type;
_tmp63=_tmp5F.width;_tmp64=_tmp5F.attributes;{struct Cyc_Absyn_Aggrfield _tmp66;
struct _dyneither_ptr*_tmp67;struct Cyc_Absyn_Tqual _tmp68;void*_tmp69;struct Cyc_Absyn_Exp*
_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_Absyn_Aggrfield*_tmp65=(struct Cyc_Absyn_Aggrfield*)
_tmp5A->hd;_tmp66=*_tmp65;_tmp67=_tmp66.name;_tmp68=_tmp66.tq;_tmp69=_tmp66.type;
_tmp6A=_tmp66.width;_tmp6B=_tmp66.attributes;if(Cyc_strptrcmp(_tmp60,_tmp67)!= 0){({
struct Cyc_String_pa_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp67);({struct Cyc_String_pa_struct _tmp70;_tmp70.tag=0;
_tmp70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp60);({struct Cyc_String_pa_struct
_tmp6F;_tmp6F.tag=0;_tmp6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp44);({struct Cyc_String_pa_struct _tmp6E;_tmp6E.tag=0;_tmp6E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)({const char*_tmp72="type";_tag_dyneither(
_tmp72,sizeof(char),5);}));({void*_tmp6C[4]={& _tmp6E,& _tmp6F,& _tmp70,& _tmp71};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmp6D="%s %s : field name mismatch %s != %s";
_tag_dyneither(_tmp6D,sizeof(char),37);}),_tag_dyneither(_tmp6C,sizeof(void*),4));});});});});});
return 0;}if(!Cyc_Tcutil_same_atts(_tmp64,_tmp6B)){({struct Cyc_String_pa_struct
_tmp77;_tmp77.tag=0;_tmp77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp60);({struct Cyc_String_pa_struct _tmp76;_tmp76.tag=0;_tmp76.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp44);({struct Cyc_String_pa_struct _tmp75;
_tmp75.tag=0;_tmp75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({const char*
_tmp78="type";_tag_dyneither(_tmp78,sizeof(char),5);}));({void*_tmp73[3]={&
_tmp75,& _tmp76,& _tmp77};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp74="%s %s : attribute mismatch on field %s";
_tag_dyneither(_tmp74,sizeof(char),39);}),_tag_dyneither(_tmp73,sizeof(void*),3));});});});});
_tmp45=0;}if(!Cyc_Tcutil_equal_tqual(_tmp61,_tmp68)){({struct Cyc_String_pa_struct
_tmp7D;_tmp7D.tag=0;_tmp7D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp60);({struct Cyc_String_pa_struct _tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp44);({struct Cyc_String_pa_struct _tmp7B;
_tmp7B.tag=0;_tmp7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({const char*
_tmp7E="type";_tag_dyneither(_tmp7E,sizeof(char),5);}));({void*_tmp79[3]={&
_tmp7B,& _tmp7C,& _tmp7D};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp7A="%s %s : qualifier mismatch on field %s";
_tag_dyneither(_tmp7A,sizeof(char),39);}),_tag_dyneither(_tmp79,sizeof(void*),3));});});});});
_tmp45=0;}if(((_tmp63 != 0  && _tmp6A != 0) && Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_tmp63)!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_tmp6A) || _tmp63 == 0  && _tmp6A != 0) || _tmp63 != 0  && _tmp6A == 0){({struct Cyc_String_pa_struct
_tmp83;_tmp83.tag=0;_tmp83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp60);({struct Cyc_String_pa_struct _tmp82;_tmp82.tag=0;_tmp82.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp44);({struct Cyc_String_pa_struct _tmp81;
_tmp81.tag=0;_tmp81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)({const char*
_tmp84="type";_tag_dyneither(_tmp84,sizeof(char),5);}));({void*_tmp7F[3]={&
_tmp81,& _tmp82,& _tmp83};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp80="%s %s : bitfield mismatch on field %s";
_tag_dyneither(_tmp80,sizeof(char),38);}),_tag_dyneither(_tmp7F,sizeof(void*),3));});});});});
_tmp45=0;}{void*subst_t1=Cyc_Tcutil_substitute(_tmp5D,_tmp69);if(!Cyc_Tcdecl_check_type(
_tmp62,subst_t1)){({struct Cyc_String_pa_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1));({
struct Cyc_String_pa_struct _tmp89;_tmp89.tag=0;_tmp89.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp62));({struct Cyc_String_pa_struct
_tmp88;_tmp88.tag=0;_tmp88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp60);({struct Cyc_String_pa_struct _tmp87;_tmp87.tag=0;_tmp87.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp44);({void*_tmp85[4]={& _tmp87,& _tmp88,&
_tmp89,& _tmp8A};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp86="type %s : type mismatch on field %s: %s != %s";
_tag_dyneither(_tmp86,sizeof(char),46);}),_tag_dyneither(_tmp85,sizeof(void*),4));});});});});});
Cyc_Tcutil_explain_failure();_tmp45=0;}};};}if(_tmp54 != 0){({struct Cyc_String_pa_struct
_tmp8E;_tmp8E.tag=0;_tmp8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)_tmp54->hd)->name);({struct Cyc_String_pa_struct _tmp8D;
_tmp8D.tag=0;_tmp8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp44);({
void*_tmp8B[2]={& _tmp8D,& _tmp8E};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp8C="type %s is missing field %s";
_tag_dyneither(_tmp8C,sizeof(char),28);}),_tag_dyneither(_tmp8B,sizeof(void*),2));});});});
_tmp45=0;}if(_tmp5A != 0){({struct Cyc_String_pa_struct _tmp92;_tmp92.tag=0;_tmp92.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp5A->hd)->name);({
struct Cyc_String_pa_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp44);({void*_tmp8F[2]={& _tmp91,& _tmp92};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp90="type %s has extra field %s";_tag_dyneither(_tmp90,
sizeof(char),27);}),_tag_dyneither(_tmp8F,sizeof(void*),2));});});});_tmp45=0;}
if(_tmp55 != _tmp5B){({struct Cyc_String_pa_struct _tmp95;_tmp95.tag=0;_tmp95.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp44);({void*_tmp93[1]={& _tmp95};
Cyc_Tcdecl_merr(loc,msg,({const char*_tmp94="%s : previous declaration disagrees with @tagged qualifier";
_tag_dyneither(_tmp94,sizeof(char),59);}),_tag_dyneither(_tmp93,sizeof(void*),1));});});
_tmp45=0;}d2=d0;goto _LL1E;};_LL1E:;}if(!_tmp45)return 0;if(_tmp49 == d2->sc)return(
struct Cyc_Absyn_Aggrdecl*)d2;else{d2=({struct Cyc_Absyn_Aggrdecl*_tmp96=_cycalloc(
sizeof(*_tmp96));_tmp96[0]=*d2;_tmp96;});d2->sc=_tmp49;return(struct Cyc_Absyn_Aggrdecl*)
d2;}};};}inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){return
is_x?({const char*_tmp97="@extensible datatype";_tag_dyneither(_tmp97,sizeof(char),
21);}):({const char*_tmp98="datatype";_tag_dyneither(_tmp98,sizeof(char),9);});}
struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_Absyn_Datatypefield*
Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*
f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct
_dyneither_ptr*msg){struct Cyc_Position_Segment*loc=f1->loc;if(Cyc_strptrcmp((*f0->name).f2,(*
f1->name).f2)!= 0){({struct Cyc_String_pa_struct _tmp9E;_tmp9E.tag=0;_tmp9E.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2);({struct Cyc_String_pa_struct
_tmp9D;_tmp9D.tag=0;_tmp9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2);({
struct Cyc_String_pa_struct _tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)v);({struct Cyc_String_pa_struct _tmp9B;_tmp9B.tag=0;_tmp9B.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp99[4]={& _tmp9B,& _tmp9C,&
_tmp9D,& _tmp9E};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp9A="%s %s: field name mismatch %s != %s";
_tag_dyneither(_tmp9A,sizeof(char),36);}),_tag_dyneither(_tmp99,sizeof(void*),4));});});});});});
return 0;}{struct _dyneither_ptr _tmp9F=*(*f0->name).f2;enum Cyc_Absyn_Scope _tmpA5;
int _tmpA6;struct _tuple9 _tmpA4=Cyc_Tcdecl_merge_scope(f0->sc,f1->sc,(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_struct _tmpA2;
_tmpA2.tag=0;_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*
_tmpA0[2]={& _tmpA2,& _tmpA3};Cyc_aprintf(({const char*_tmpA1="in %s %s, field";
_tag_dyneither(_tmpA1,sizeof(char),16);}),_tag_dyneither(_tmpA0,sizeof(void*),2));});});}),
_tmp9F,loc,msg,0);_tmpA5=_tmpA4.f1;_tmpA6=_tmpA4.f2;{struct Cyc_List_List*_tmpA7=
f0->typs;struct Cyc_List_List*_tmpA8=f1->typs;if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmpA7)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpA8)){({struct Cyc_String_pa_struct _tmpAD;_tmpAD.tag=0;_tmpAD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp9F);({struct Cyc_String_pa_struct _tmpAC;
_tmpAC.tag=0;_tmpAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct
Cyc_String_pa_struct _tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)t);({void*_tmpA9[3]={& _tmpAB,& _tmpAC,& _tmpAD};Cyc_Tcdecl_merr(loc,
msg,({const char*_tmpAA="%s %s, field %s: parameter number mismatch";
_tag_dyneither(_tmpAA,sizeof(char),43);}),_tag_dyneither(_tmpA9,sizeof(void*),3));});});});});
_tmpA6=0;}for(0;_tmpA7 != 0;(_tmpA7=_tmpA7->tl,_tmpA8=_tmpA8->tl)){if(!Cyc_Tcutil_equal_tqual((*((
struct _tuple14*)_tmpA7->hd)).f1,(*((struct _tuple14*)((struct Cyc_List_List*)
_check_null(_tmpA8))->hd)).f1)){({struct Cyc_String_pa_struct _tmpB2;_tmpB2.tag=0;
_tmpB2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9F);({struct Cyc_String_pa_struct
_tmpB1;_tmpB1.tag=0;_tmpB1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({
struct Cyc_String_pa_struct _tmpB0;_tmpB0.tag=0;_tmpB0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);({void*_tmpAE[3]={& _tmpB0,& _tmpB1,& _tmpB2};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmpAF="%s %s, field %s: parameter qualifier";_tag_dyneither(
_tmpAF,sizeof(char),37);}),_tag_dyneither(_tmpAE,sizeof(void*),3));});});});});
_tmpA6=0;}{void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple14*)_tmpA8->hd)).f2);
if(!Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmpA7->hd)).f2,subst_t1)){({
struct Cyc_String_pa_struct _tmpB9;_tmpB9.tag=0;_tmpB9.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_t1));({struct Cyc_String_pa_struct
_tmpB8;_tmpB8.tag=0;_tmpB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((
struct _tuple14*)_tmpA7->hd)).f2));({struct Cyc_String_pa_struct _tmpB7;_tmpB7.tag=
0;_tmpB7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9F);({struct Cyc_String_pa_struct
_tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({
struct Cyc_String_pa_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);({void*_tmpB3[5]={& _tmpB5,& _tmpB6,& _tmpB7,& _tmpB8,&
_tmpB9};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpB4="%s %s, field %s: parameter type mismatch %s != %s";
_tag_dyneither(_tmpB4,sizeof(char),50);}),_tag_dyneither(_tmpB3,sizeof(void*),5));});});});});});});
Cyc_Tcutil_explain_failure();_tmpA6=0;}};}if(!_tmpA6)return 0;if(f0->sc != _tmpA5){
struct Cyc_Absyn_Datatypefield*_tmpBA=({struct Cyc_Absyn_Datatypefield*_tmpBB=
_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=*f0;_tmpBB;});((struct Cyc_Absyn_Datatypefield*)
_check_null(_tmpBA))->sc=_tmpA5;return _tmpBA;}else{return(struct Cyc_Absyn_Datatypefield*)
f0;}};};}static struct _tuple14*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*
inst,struct _tuple14*x){struct _tuple14 _tmpBD;struct Cyc_Absyn_Tqual _tmpBE;void*
_tmpBF;struct _tuple14*_tmpBC=x;_tmpBD=*_tmpBC;_tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;
return({struct _tuple14*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->f1=_tmpBE;
_tmpC0->f2=Cyc_Tcutil_substitute(inst,_tmpBF);_tmpC0;});}static struct Cyc_Absyn_Datatypefield*
Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*
f1){struct Cyc_Absyn_Datatypefield*_tmpC1=({struct Cyc_Absyn_Datatypefield*_tmpC2=
_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=*f1;_tmpC2;});_tmpC1->typs=((struct Cyc_List_List*(*)(
struct _tuple14*(*f)(struct Cyc_List_List*,struct _tuple14*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,
inst1,f1->typs);return _tmpC1;}static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(
struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct
Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct
_dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpCB=
_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=0;_tmpCB;});struct Cyc_List_List**_tmpC3=
f2sp;int cmp=- 1;for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)
f1s->hd)->name))< 0){struct Cyc_List_List*_tmpC4=({struct Cyc_List_List*_tmpC5=
_cycalloc(sizeof(*_tmpC5));_tmpC5->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd;
_tmpC5->tl=0;_tmpC5;});*_tmpC3=_tmpC4;_tmpC3=&((struct Cyc_List_List*)_check_null(
_tmpC4))->tl;f0s=f0s->tl;}if(f0s == 0  || cmp > 0){*incl=0;{struct Cyc_List_List*
_tmpC6=({struct Cyc_List_List*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->hd=Cyc_Tcdecl_substitute_datatypefield(
inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd);_tmpC7->tl=0;_tmpC7;});*_tmpC3=
_tmpC6;_tmpC3=&((struct Cyc_List_List*)_check_null(_tmpC6))->tl;};}else{struct Cyc_Absyn_Datatypefield*
_tmpC8=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(
struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);if(_tmpC8 != 0){if(_tmpC8 != (
struct Cyc_Absyn_Datatypefield*)((struct Cyc_Absyn_Datatypefield*)f0s->hd))*incl=0;{
struct Cyc_List_List*_tmpC9=({struct Cyc_List_List*_tmpCA=_cycalloc(sizeof(*_tmpCA));
_tmpCA->hd=(struct Cyc_Absyn_Datatypefield*)_tmpC8;_tmpCA->tl=0;_tmpCA;});*_tmpC3=
_tmpC9;_tmpC3=&((struct Cyc_List_List*)_check_null(_tmpC9))->tl;};}else{*res=0;}
f0s=f0s->tl;}}if(f1s != 0){*incl=0;*_tmpC3=f1s;}else{*_tmpC3=f0s;}return*f2sp;}
struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static struct
_tuple15 Cyc_Tcdecl_split(struct Cyc_List_List*f){if(f == 0)return({struct _tuple15
_tmpCC;_tmpCC.f1=0;_tmpCC.f2=0;_tmpCC;});if(f->tl == 0)return({struct _tuple15
_tmpCD;_tmpCD.f1=f;_tmpCD.f2=0;_tmpCD;});{struct Cyc_List_List*_tmpCF;struct Cyc_List_List*
_tmpD0;struct _tuple15 _tmpCE=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(
f->tl))->tl);_tmpCF=_tmpCE.f1;_tmpD0=_tmpCE.f2;return({struct _tuple15 _tmpD1;
_tmpD1.f1=({struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->hd=(
void*)((void*)f->hd);_tmpD3->tl=_tmpCF;_tmpD3;});_tmpD1.f2=({struct Cyc_List_List*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->hd=(void*)((void*)((struct Cyc_List_List*)
_check_null(f->tl))->hd);_tmpD2->tl=_tmpD0;_tmpD2;});_tmpD1;});};}struct Cyc_List_List*
Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct
_dyneither_ptr*v,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct
Cyc_List_List*_tmpD5;struct Cyc_List_List*_tmpD6;struct _tuple15 _tmpD4=((struct
_tuple15(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);_tmpD5=_tmpD4.f1;_tmpD6=
_tmpD4.f2;if(_tmpD5 != 0  && _tmpD5->tl != 0)_tmpD5=Cyc_Tcdecl_sort_xdatatype_fields(
_tmpD5,res,v,loc,msg);if(_tmpD6 != 0  && _tmpD6->tl != 0)_tmpD6=Cyc_Tcdecl_sort_xdatatype_fields(
_tmpD6,res,v,loc,msg);return Cyc_Tcdecl_merge_xdatatype_fields(_tmpD5,_tmpD6,0,0,
0,res,({int*_tmpD7=_cycalloc_atomic(sizeof(*_tmpD7));_tmpD7[0]=1;_tmpD7;}),({
const char*_tmpD8="@extensible datatype";_tag_dyneither(_tmpD8,sizeof(char),21);}),*
v,loc,msg);}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};struct
Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*
d0,struct Cyc_Absyn_Datatypedecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct _dyneither_ptr _tmpD9=Cyc_Absynpp_qvar2string(d0->name);
struct _dyneither_ptr t=({const char*_tmpFD="datatype";_tag_dyneither(_tmpFD,
sizeof(char),9);});int _tmpDA=1;if(d0->is_extensible != d1->is_extensible){({
struct Cyc_String_pa_struct _tmpDF;_tmpDF.tag=0;_tmpDF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d1->is_extensible));({struct Cyc_String_pa_struct
_tmpDE;_tmpDE.tag=0;_tmpDE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(
d0->is_extensible));({struct Cyc_String_pa_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD9);({void*_tmpDB[3]={& _tmpDD,&
_tmpDE,& _tmpDF};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpDC="expected %s to be a %s instead of a %s";
_tag_dyneither(_tmpDC,sizeof(char),39);}),_tag_dyneither(_tmpDB,sizeof(void*),3));});});});});
_tmpDA=0;}else{t=Cyc_Tcdecl_is_x2string(d0->is_extensible);}if(!Cyc_Tcdecl_check_tvs(
d0->tvs,d1->tvs,t,_tmpD9,loc,msg))return 0;{enum Cyc_Absyn_Scope _tmpE1;int _tmpE2;
struct _tuple9 _tmpE0=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpD9,loc,msg,0);
_tmpE1=_tmpE0.f1;_tmpE2=_tmpE0.f2;if(!_tmpE2)_tmpDA=0;{struct Cyc_Absyn_Datatypedecl*
d2;{struct _tuple16 _tmpE4=({struct _tuple16 _tmpE3;_tmpE3.f1=d0->fields;_tmpE3.f2=
d1->fields;_tmpE3;});struct Cyc_Core_Opt*_tmpE5;struct Cyc_Core_Opt*_tmpE6;struct
Cyc_Core_Opt*_tmpE7;struct Cyc_Core_Opt _tmpE8;struct Cyc_List_List*_tmpE9;struct
Cyc_Core_Opt*_tmpEA;struct Cyc_Core_Opt _tmpEB;struct Cyc_List_List*_tmpEC;_LL26:
_tmpE5=_tmpE4.f2;if(_tmpE5 != 0)goto _LL28;_LL27: d2=d0;goto _LL25;_LL28: _tmpE6=
_tmpE4.f1;if(_tmpE6 != 0)goto _LL2A;_LL29: d2=d1;goto _LL25;_LL2A: _tmpE7=_tmpE4.f1;
if(_tmpE7 == 0)goto _LL25;_tmpE8=*_tmpE7;_tmpE9=(struct Cyc_List_List*)_tmpE8.v;
_tmpEA=_tmpE4.f2;if(_tmpEA == 0)goto _LL25;_tmpEB=*_tmpEA;_tmpEC=(struct Cyc_List_List*)
_tmpEB.v;_LL2B: {struct Cyc_List_List*_tmpED=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
if(d0->is_extensible){int _tmpEE=1;struct Cyc_List_List*_tmpEF=Cyc_Tcdecl_merge_xdatatype_fields(
_tmpE9,_tmpEC,_tmpED,d0->tvs,d1->tvs,& _tmpDA,& _tmpEE,t,_tmpD9,loc,msg);if(_tmpEE)
d2=d0;else{d2=({struct Cyc_Absyn_Datatypedecl*_tmpF0=_cycalloc(sizeof(*_tmpF0));
_tmpF0[0]=*d0;_tmpF0;});d2->sc=_tmpE1;d2->fields=({struct Cyc_Core_Opt*_tmpF1=
_cycalloc(sizeof(*_tmpF1));_tmpF1->v=_tmpEF;_tmpF1;});}}else{for(0;_tmpE9 != 0
 && _tmpEC != 0;(_tmpE9=_tmpE9->tl,_tmpEC=_tmpEC->tl)){Cyc_Tcdecl_merge_datatypefield((
struct Cyc_Absyn_Datatypefield*)_tmpE9->hd,(struct Cyc_Absyn_Datatypefield*)_tmpEC->hd,
_tmpED,t,_tmpD9,msg);}if(_tmpE9 != 0){({struct Cyc_String_pa_struct _tmpF6;_tmpF6.tag=
0;_tmpF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)((
struct Cyc_List_List*)_check_null(_tmpEC))->hd)->name).f2);({struct Cyc_String_pa_struct
_tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpD9);({struct Cyc_String_pa_struct _tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpF2[3]={& _tmpF4,& _tmpF5,&
_tmpF6};Cyc_Tcdecl_merr(loc,msg,({const char*_tmpF3="%s %s has extra field %s";
_tag_dyneither(_tmpF3,sizeof(char),25);}),_tag_dyneither(_tmpF2,sizeof(void*),3));});});});});
_tmpDA=0;}if(_tmpEC != 0){({struct Cyc_String_pa_struct _tmpFB;_tmpFB.tag=0;_tmpFB.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)
_tmpEC->hd)->name).f2);({struct Cyc_String_pa_struct _tmpFA;_tmpFA.tag=0;_tmpFA.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD9);({struct Cyc_String_pa_struct
_tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({
void*_tmpF7[3]={& _tmpF9,& _tmpFA,& _tmpFB};Cyc_Tcdecl_merr(loc,msg,({const char*
_tmpF8="%s %s is missing field %s";_tag_dyneither(_tmpF8,sizeof(char),26);}),
_tag_dyneither(_tmpF7,sizeof(void*),3));});});});});_tmpDA=0;}d2=d0;}goto _LL25;}
_LL25:;}if(!_tmpDA)return 0;if(_tmpE1 == d2->sc)return(struct Cyc_Absyn_Datatypedecl*)
d2;else{d2=({struct Cyc_Absyn_Datatypedecl*_tmpFC=_cycalloc(sizeof(*_tmpFC));
_tmpFC[0]=*d2;_tmpFC;});d2->sc=_tmpE1;return(struct Cyc_Absyn_Datatypedecl*)d2;}};};}
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,
struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*
msg){struct _dyneither_ptr _tmpFE=Cyc_Absynpp_qvar2string(d0->name);int _tmpFF=1;
enum Cyc_Absyn_Scope _tmp102;int _tmp103;struct _tuple9 _tmp101=Cyc_Tcdecl_merge_scope(
d0->sc,d1->sc,({const char*_tmp100="enum";_tag_dyneither(_tmp100,sizeof(char),5);}),
_tmpFE,loc,msg,1);_tmp102=_tmp101.f1;_tmp103=_tmp101.f2;if(!_tmp103)_tmpFF=0;{
struct Cyc_Absyn_Enumdecl*d2;{struct _tuple16 _tmp105=({struct _tuple16 _tmp104;
_tmp104.f1=d0->fields;_tmp104.f2=d1->fields;_tmp104;});struct Cyc_Core_Opt*
_tmp106;struct Cyc_Core_Opt*_tmp107;struct Cyc_Core_Opt*_tmp108;struct Cyc_Core_Opt
_tmp109;struct Cyc_List_List*_tmp10A;struct Cyc_Core_Opt*_tmp10B;struct Cyc_Core_Opt
_tmp10C;struct Cyc_List_List*_tmp10D;_LL2D: _tmp106=_tmp105.f2;if(_tmp106 != 0)goto
_LL2F;_LL2E: d2=d0;goto _LL2C;_LL2F: _tmp107=_tmp105.f1;if(_tmp107 != 0)goto _LL31;
_LL30: d2=d1;goto _LL2C;_LL31: _tmp108=_tmp105.f1;if(_tmp108 == 0)goto _LL2C;_tmp109=*
_tmp108;_tmp10A=(struct Cyc_List_List*)_tmp109.v;_tmp10B=_tmp105.f2;if(_tmp10B == 
0)goto _LL2C;_tmp10C=*_tmp10B;_tmp10D=(struct Cyc_List_List*)_tmp10C.v;_LL32: for(0;
_tmp10A != 0  && _tmp10D != 0;(_tmp10A=_tmp10A->tl,_tmp10D=_tmp10D->tl)){struct Cyc_Absyn_Enumfield
_tmp10F;struct _tuple0*_tmp110;struct _tuple0 _tmp111;struct _dyneither_ptr*_tmp112;
struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Position_Segment*_tmp114;struct Cyc_Absyn_Enumfield*
_tmp10E=(struct Cyc_Absyn_Enumfield*)_tmp10A->hd;_tmp10F=*_tmp10E;_tmp110=_tmp10F.name;
_tmp111=*_tmp110;_tmp112=_tmp111.f2;_tmp113=_tmp10F.tag;_tmp114=_tmp10F.loc;{
struct Cyc_Absyn_Enumfield _tmp116;struct _tuple0*_tmp117;struct _tuple0 _tmp118;
struct _dyneither_ptr*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Position_Segment*
_tmp11B;struct Cyc_Absyn_Enumfield*_tmp115=(struct Cyc_Absyn_Enumfield*)_tmp10D->hd;
_tmp116=*_tmp115;_tmp117=_tmp116.name;_tmp118=*_tmp117;_tmp119=_tmp118.f2;
_tmp11A=_tmp116.tag;_tmp11B=_tmp116.loc;if(Cyc_strptrcmp(_tmp112,_tmp119)!= 0){({
struct Cyc_String_pa_struct _tmp120;_tmp120.tag=0;_tmp120.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp119);({struct Cyc_String_pa_struct _tmp11F;_tmp11F.tag=0;
_tmp11F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp112);({struct Cyc_String_pa_struct
_tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpFE);({void*_tmp11C[3]={& _tmp11E,& _tmp11F,& _tmp120};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp11D="enum %s: field name mismatch %s != %s";_tag_dyneither(_tmp11D,
sizeof(char),38);}),_tag_dyneither(_tmp11C,sizeof(void*),3));});});});});_tmpFF=
0;}if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp113))
!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp11A))){({
struct Cyc_String_pa_struct _tmp124;_tmp124.tag=0;_tmp124.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*_tmp119);({struct Cyc_String_pa_struct _tmp123;_tmp123.tag=0;
_tmp123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpFE);({void*_tmp121[2]={&
_tmp123,& _tmp124};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp122="enum %s, field %s, value mismatch";
_tag_dyneither(_tmp122,sizeof(char),34);}),_tag_dyneither(_tmp121,sizeof(void*),
2));});});});_tmpFF=0;}};}d2=d0;goto _LL2C;_LL2C:;}if(!_tmpFF)return 0;if(d2->sc == 
_tmp102)return(struct Cyc_Absyn_Enumdecl*)d2;else{d2=({struct Cyc_Absyn_Enumdecl*
_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125[0]=*d2;_tmp125;});d2->sc=_tmp102;
return(struct Cyc_Absyn_Enumdecl*)d2;}};}static struct _tuple9 Cyc_Tcdecl_check_var_or_fn_decl(
enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*
atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*
atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr*msg){int _tmp126=1;enum Cyc_Absyn_Scope _tmp128;int _tmp129;
struct _tuple9 _tmp127=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);_tmp128=
_tmp127.f1;_tmp129=_tmp127.f2;if(!_tmp129)_tmp126=0;if(!Cyc_Tcdecl_check_type(t0,
t1)){({struct Cyc_String_pa_struct _tmp12F;_tmp12F.tag=0;_tmp12F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t0));({struct Cyc_String_pa_struct
_tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));({struct Cyc_String_pa_struct _tmp12D;_tmp12D.tag=0;
_tmp12D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_struct
_tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({
void*_tmp12A[4]={& _tmp12C,& _tmp12D,& _tmp12E,& _tmp12F};Cyc_Tcdecl_merr(loc,msg,({
const char*_tmp12B="%s %s has type \n%s\n instead of \n%s\n";_tag_dyneither(
_tmp12B,sizeof(char),36);}),_tag_dyneither(_tmp12A,sizeof(void*),4));});});});});});
Cyc_Tcutil_explain_failure();_tmp126=0;}if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){({
struct Cyc_String_pa_struct _tmp133;_tmp133.tag=0;_tmp133.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)v);({struct Cyc_String_pa_struct _tmp132;_tmp132.tag=0;
_tmp132.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp130[2]={&
_tmp132,& _tmp133};Cyc_Tcdecl_merr(loc,msg,({const char*_tmp131="%s %s has different type qualifiers";
_tag_dyneither(_tmp131,sizeof(char),36);}),_tag_dyneither(_tmp130,sizeof(void*),
2));});});});_tmp126=0;}if(!Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_String_pa_struct
_tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({
struct Cyc_String_pa_struct _tmp136;_tmp136.tag=0;_tmp136.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)t);({void*_tmp134[2]={& _tmp136,& _tmp137};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp135="%s %s has different attributes";_tag_dyneither(
_tmp135,sizeof(char),31);}),_tag_dyneither(_tmp134,sizeof(void*),2));});});});({
void*_tmp138=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp139="previous attributes: ";
_tag_dyneither(_tmp139,sizeof(char),22);}),_tag_dyneither(_tmp138,sizeof(void*),
0));});for(0;atts0 != 0;atts0=atts0->tl){({struct Cyc_String_pa_struct _tmp13C;
_tmp13C.tag=0;_tmp13C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)atts0->hd));({void*_tmp13A[1]={& _tmp13C};Cyc_fprintf(Cyc_stderr,({const
char*_tmp13B="%s ";_tag_dyneither(_tmp13B,sizeof(char),4);}),_tag_dyneither(
_tmp13A,sizeof(void*),1));});});}({void*_tmp13D=0;Cyc_fprintf(Cyc_stderr,({const
char*_tmp13E="\ncurrent attributes: ";_tag_dyneither(_tmp13E,sizeof(char),22);}),
_tag_dyneither(_tmp13D,sizeof(void*),0));});for(0;atts1 != 0;atts1=atts1->tl){({
struct Cyc_String_pa_struct _tmp141;_tmp141.tag=0;_tmp141.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd));({void*
_tmp13F[1]={& _tmp141};Cyc_fprintf(Cyc_stderr,({const char*_tmp140="%s ";
_tag_dyneither(_tmp140,sizeof(char),4);}),_tag_dyneither(_tmp13F,sizeof(void*),1));});});}({
void*_tmp142=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp143="\n";_tag_dyneither(
_tmp143,sizeof(char),2);}),_tag_dyneither(_tmp142,sizeof(void*),0));});_tmp126=0;}
return({struct _tuple9 _tmp144;_tmp144.f1=_tmp128;_tmp144.f2=_tmp126;_tmp144;});}
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,
struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*
msg){struct _dyneither_ptr _tmp145=Cyc_Absynpp_qvar2string(d0->name);enum Cyc_Absyn_Scope
_tmp148;int _tmp149;struct _tuple9 _tmp147=Cyc_Tcdecl_check_var_or_fn_decl(d0->sc,
d0->type,d0->tq,d0->attributes,d1->sc,d1->type,d1->tq,d1->attributes,({const char*
_tmp146="variable";_tag_dyneither(_tmp146,sizeof(char),9);}),_tmp145,loc,msg);
_tmp148=_tmp147.f1;_tmp149=_tmp147.f2;if(!_tmp149)return 0;if(d0->sc == _tmp148)
return(struct Cyc_Absyn_Vardecl*)d0;else{struct Cyc_Absyn_Vardecl*_tmp14A=({struct
Cyc_Absyn_Vardecl*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=*d0;_tmp14B;});((
struct Cyc_Absyn_Vardecl*)_check_null(_tmp14A))->sc=_tmp148;return _tmp14A;}}
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct _dyneither_ptr _tmp14C=Cyc_Absynpp_qvar2string(d0->name);
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,({const char*_tmp14D="typedef";
_tag_dyneither(_tmp14D,sizeof(char),8);}),_tmp14C,loc,msg))return 0;{struct Cyc_List_List*
_tmp14E=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->defn != 0  && d1->defn != 
0){void*subst_defn1=Cyc_Tcutil_substitute(_tmp14E,(void*)((struct Cyc_Core_Opt*)
_check_null(d1->defn))->v);if(!Cyc_Tcdecl_check_type((void*)((struct Cyc_Core_Opt*)
_check_null(d0->defn))->v,subst_defn1)){({struct Cyc_String_pa_struct _tmp153;
_tmp153.tag=0;_tmp153.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(d0->defn))->v));({struct Cyc_String_pa_struct
_tmp152;_tmp152.tag=0;_tmp152.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_defn1));({struct Cyc_String_pa_struct _tmp151;_tmp151.tag=
0;_tmp151.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp14C);({void*
_tmp14F[3]={& _tmp151,& _tmp152,& _tmp153};Cyc_Tcdecl_merr(loc,msg,({const char*
_tmp150="typedef %s does not refer to the same type: %s != %s";_tag_dyneither(
_tmp150,sizeof(char),53);}),_tag_dyneither(_tmp14F,sizeof(void*),3));});});});});
return 0;}}return(struct Cyc_Absyn_Typedefdecl*)d0;};}void*Cyc_Tcdecl_merge_binding(
void*b0,void*b1,struct Cyc_Position_Segment*loc,struct _dyneither_ptr*msg){struct
_tuple13 _tmp155=({struct _tuple13 _tmp154;_tmp154.f1=b0;_tmp154.f2=b1;_tmp154;});
void*_tmp156;void*_tmp158;void*_tmp15A;struct Cyc_Absyn_Vardecl*_tmp15C;void*
_tmp15D;struct Cyc_Absyn_Vardecl*_tmp15F;void*_tmp160;struct Cyc_Absyn_Vardecl*
_tmp162;void*_tmp163;struct Cyc_Absyn_Fndecl*_tmp165;void*_tmp166;void*_tmp168;
struct Cyc_Absyn_Fndecl*_tmp16A;void*_tmp16B;struct Cyc_Absyn_Fndecl*_tmp16D;void*
_tmp16E;struct Cyc_Absyn_Vardecl*_tmp170;_LL34: _tmp156=_tmp155.f1;{struct Cyc_Absyn_Unresolved_b_struct*
_tmp157=(struct Cyc_Absyn_Unresolved_b_struct*)_tmp156;if(_tmp157->tag != 0)goto
_LL36;};_tmp158=_tmp155.f2;{struct Cyc_Absyn_Unresolved_b_struct*_tmp159=(struct
Cyc_Absyn_Unresolved_b_struct*)_tmp158;if(_tmp159->tag != 0)goto _LL36;};_LL35:
return(void*)& Cyc_Absyn_Unresolved_b_val;_LL36: _tmp15A=_tmp155.f1;{struct Cyc_Absyn_Global_b_struct*
_tmp15B=(struct Cyc_Absyn_Global_b_struct*)_tmp15A;if(_tmp15B->tag != 1)goto _LL38;
else{_tmp15C=_tmp15B->f1;}};_tmp15D=_tmp155.f2;{struct Cyc_Absyn_Global_b_struct*
_tmp15E=(struct Cyc_Absyn_Global_b_struct*)_tmp15D;if(_tmp15E->tag != 1)goto _LL38;
else{_tmp15F=_tmp15E->f1;}};_LL37: {struct Cyc_Absyn_Vardecl*_tmp171=Cyc_Tcdecl_merge_vardecl(
_tmp15C,_tmp15F,loc,msg);if(_tmp171 == 0)return(void*)& Cyc_Absyn_Unresolved_b_val;
if(_tmp171 == (struct Cyc_Absyn_Vardecl*)_tmp15C)return b0;if(_tmp171 == (struct Cyc_Absyn_Vardecl*)
_tmp15F)return b1;return(void*)({struct Cyc_Absyn_Global_b_struct*_tmp172=
_cycalloc(sizeof(*_tmp172));_tmp172[0]=({struct Cyc_Absyn_Global_b_struct _tmp173;
_tmp173.tag=1;_tmp173.f1=(struct Cyc_Absyn_Vardecl*)_tmp171;_tmp173;});_tmp172;});}
_LL38: _tmp160=_tmp155.f1;{struct Cyc_Absyn_Global_b_struct*_tmp161=(struct Cyc_Absyn_Global_b_struct*)
_tmp160;if(_tmp161->tag != 1)goto _LL3A;else{_tmp162=_tmp161->f1;}};_tmp163=
_tmp155.f2;{struct Cyc_Absyn_Funname_b_struct*_tmp164=(struct Cyc_Absyn_Funname_b_struct*)
_tmp163;if(_tmp164->tag != 2)goto _LL3A;else{_tmp165=_tmp164->f1;}};_LL39: {int
_tmp176;struct _tuple9 _tmp175=Cyc_Tcdecl_check_var_or_fn_decl(_tmp162->sc,_tmp162->type,
_tmp162->tq,_tmp162->attributes,_tmp165->sc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp165->cached_typ))->v,Cyc_Absyn_empty_tqual(0),_tmp165->attributes,({
const char*_tmp174="function";_tag_dyneither(_tmp174,sizeof(char),9);}),Cyc_Absynpp_qvar2string(
_tmp162->name),loc,msg);_tmp176=_tmp175.f2;return _tmp176?b1:(void*)& Cyc_Absyn_Unresolved_b_val;}
_LL3A: _tmp166=_tmp155.f1;{struct Cyc_Absyn_Funname_b_struct*_tmp167=(struct Cyc_Absyn_Funname_b_struct*)
_tmp166;if(_tmp167->tag != 2)goto _LL3C;};_tmp168=_tmp155.f2;{struct Cyc_Absyn_Funname_b_struct*
_tmp169=(struct Cyc_Absyn_Funname_b_struct*)_tmp168;if(_tmp169->tag != 2)goto _LL3C;
else{_tmp16A=_tmp169->f1;}};_LL3B:({struct Cyc_String_pa_struct _tmp179;_tmp179.tag=
0;_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp16A->name));({void*_tmp177[1]={& _tmp179};Cyc_Tcdecl_merr(loc,msg,({const char*
_tmp178="redefinition of function %s";_tag_dyneither(_tmp178,sizeof(char),28);}),
_tag_dyneither(_tmp177,sizeof(void*),1));});});return(void*)& Cyc_Absyn_Unresolved_b_val;
_LL3C: _tmp16B=_tmp155.f1;{struct Cyc_Absyn_Funname_b_struct*_tmp16C=(struct Cyc_Absyn_Funname_b_struct*)
_tmp16B;if(_tmp16C->tag != 2)goto _LL3E;else{_tmp16D=_tmp16C->f1;}};_tmp16E=
_tmp155.f2;{struct Cyc_Absyn_Global_b_struct*_tmp16F=(struct Cyc_Absyn_Global_b_struct*)
_tmp16E;if(_tmp16F->tag != 1)goto _LL3E;else{_tmp170=_tmp16F->f1;}};_LL3D: {int
_tmp17C;struct _tuple9 _tmp17B=Cyc_Tcdecl_check_var_or_fn_decl(_tmp16D->sc,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp16D->cached_typ))->v,Cyc_Absyn_empty_tqual(0),
_tmp16D->attributes,_tmp170->sc,_tmp170->type,_tmp170->tq,_tmp170->attributes,({
const char*_tmp17A="variable";_tag_dyneither(_tmp17A,sizeof(char),9);}),Cyc_Absynpp_qvar2string(
_tmp16D->name),loc,msg);_tmp17C=_tmp17B.f2;return _tmp17C?b0:(void*)& Cyc_Absyn_Unresolved_b_val;}
_LL3E:;_LL3F:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp17D=
_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp17E;_tmp17E.tag=Cyc_Core_Invalid_argument;_tmp17E.f1=({const char*_tmp17F="Tcdecl::merge_binding";
_tag_dyneither(_tmp17F,sizeof(char),22);});_tmp17E;});_tmp17D;}));_LL33:;}struct
Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*
d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*msg){struct Cyc_Tcdecl_Xdatatypefielddecl _tmp181;struct Cyc_Absyn_Datatypedecl*
_tmp182;struct Cyc_Absyn_Datatypefield*_tmp183;struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp180=d0;_tmp181=*_tmp180;_tmp182=_tmp181.base;_tmp183=_tmp181.field;{struct
Cyc_Tcdecl_Xdatatypefielddecl _tmp185;struct Cyc_Absyn_Datatypedecl*_tmp186;struct
Cyc_Absyn_Datatypefield*_tmp187;struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp184=d1;
_tmp185=*_tmp184;_tmp186=_tmp185.base;_tmp187=_tmp185.field;{struct
_dyneither_ptr _tmp188=Cyc_Absynpp_qvar2string(_tmp183->name);if(Cyc_Absyn_qvar_cmp(
_tmp182->name,_tmp186->name)!= 0){({struct Cyc_String_pa_struct _tmp18D;_tmp18D.tag=
0;_tmp18D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp188);({struct Cyc_String_pa_struct
_tmp18C;_tmp18C.tag=0;_tmp18C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp186->name));({struct Cyc_String_pa_struct _tmp18B;
_tmp18B.tag=0;_tmp18B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp182->name));({void*_tmp189[3]={& _tmp18B,& _tmp18C,& _tmp18D};Cyc_Tcdecl_merr(
loc,msg,({const char*_tmp18A="@extensible datatype %s and %s have both a field named %s";
_tag_dyneither(_tmp18A,sizeof(char),58);}),_tag_dyneither(_tmp189,sizeof(void*),
3));});});});});return 0;}if(!Cyc_Tcdecl_check_tvs(_tmp182->tvs,_tmp186->tvs,({
const char*_tmp18E="@extensible datatype";_tag_dyneither(_tmp18E,sizeof(char),21);}),
Cyc_Absynpp_qvar2string(_tmp182->name),loc,msg))return 0;{struct Cyc_List_List*
_tmp18F=Cyc_Tcdecl_build_tvs_map(_tmp182->tvs,_tmp186->tvs);struct Cyc_Absyn_Datatypefield*
_tmp190=Cyc_Tcdecl_merge_datatypefield(_tmp183,_tmp187,_tmp18F,({const char*
_tmp192="@extensible datatype field";_tag_dyneither(_tmp192,sizeof(char),27);}),
_tmp188,msg);if(_tmp190 == 0)return 0;if(_tmp190 == (struct Cyc_Absyn_Datatypefield*)
_tmp183)return(struct Cyc_Tcdecl_Xdatatypefielddecl*)d0;return({struct Cyc_Tcdecl_Xdatatypefielddecl*
_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->base=_tmp182;_tmp191->field=(struct
Cyc_Absyn_Datatypefield*)_tmp190;_tmp191;});};};};}
