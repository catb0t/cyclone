#include <setjmp.h>
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
extern int _throw_match();
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
_zero_arr_inplace_plus_char(char **x, int orig_i) {
  *x = _zero_arr_plus_char(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_short(short **x, int orig_i) {
  *x = _zero_arr_plus_short(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_int(int **x, int orig_i) {
  *x = _zero_arr_plus_int(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_float(float **x, int orig_i) {
  *x = _zero_arr_plus_float(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_double(double **x, int orig_i) {
  *x = _zero_arr_plus_double(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_longdouble(long double **x, int orig_i) {
  *x = _zero_arr_plus_longdouble(*x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_voidstar(void ***x, int orig_i) {
  *x = _zero_arr_plus_voidstar(*x,1,orig_i);
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
_zero_arr_inplace_plus_post_char(char **x, int orig_i){
  char * _zap_res = *x;
  *x = _zero_arr_plus_char(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE short *
_zero_arr_inplace_plus_post_short(short **x, int orig_i){
  short * _zap_res = *x;
  *x = _zero_arr_plus_short(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE int *
_zero_arr_inplace_plus_post_int(int **x, int orig_i){
  int * _zap_res = *x;
  *x = _zero_arr_plus_int(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE float *
_zero_arr_inplace_plus_post_float(float **x, int orig_i){
  float * _zap_res = *x;
  *x = _zero_arr_plus_float(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE double *
_zero_arr_inplace_plus_post_double(double **x, int orig_i){
  double * _zap_res = *x;
  *x = _zero_arr_plus_double(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble(long double **x, int orig_i){
  long double * _zap_res = *x;
  *x = _zero_arr_plus_longdouble(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar(void ***x, int orig_i){
  void ** _zap_res = *x;
  *x = _zero_arr_plus_voidstar(_zap_res,1,orig_i);
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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};int Cyc_file_string_write(struct Cyc___cycFILE*,struct
_dyneither_ptr src,int src_offset,int max_count);struct Cyc_Core_Opt{void*v;};extern
char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * 
sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*
f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{
char*tag;};struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
4];struct Cyc_List_Nth_struct{char*tag;};int Cyc_List_exists(int(*pred)(void*),
struct Cyc_List_List*x);unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};struct _dyneither_ptr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*
loc);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n
Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(
struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*
ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public
 = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct
Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;struct
Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz
 = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};enum 
Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top
 = 2};enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind
 = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct
Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None
 = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct
_union_Constraint_Eq_constr{int tag;void*val;};struct
_union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct
_union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr
Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_struct{
int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct
Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int
identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{int tag;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*
ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};
struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*
bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};
struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_struct{
int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{
int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{int tag;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{
struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
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
int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};extern
struct Cyc_Absyn_Stdcall_att_struct Cyc_Absyn_Stdcall_att_val;extern struct Cyc_Absyn_Cdecl_att_struct
Cyc_Absyn_Cdecl_att_val;extern struct Cyc_Absyn_Fastcall_att_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Unused_att_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_struct{
int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
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
f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
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
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_struct{
int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*
topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Skip_s_struct{
int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_struct{
int tag;};struct Cyc_Absyn_Var_p_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct
Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{int tag;struct Cyc_Absyn_Tvar*
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
char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
_tmp2=(*v).f1;int _tmp3;struct Cyc_List_List*_tmp4;struct Cyc_List_List*_tmp5;
struct Cyc_List_List*_tmp6;_LL11: if((_tmp2.Loc_n).tag != 4)goto _LL13;_tmp3=(int)(
_tmp2.Loc_n).val;_LL12: _tmp4=0;goto _LL14;_LL13: if((_tmp2.Rel_n).tag != 1)goto
_LL15;_tmp4=(struct Cyc_List_List*)(_tmp2.Rel_n).val;_LL14: _tmp1=_tmp4;goto _LL10;
_LL15: if((_tmp2.C_n).tag != 3)goto _LL17;_tmp5=(struct Cyc_List_List*)(_tmp2.C_n).val;
_LL16: Cyc_Absyndump_dump_char((int)' ');goto _LL10;_LL17: if((_tmp2.Abs_n).tag != 2)
goto _LL10;_tmp6=(struct Cyc_List_List*)(_tmp2.Abs_n).val;_LL18: if(Cyc_Absyndump_qvar_to_Cids
 && Cyc_Absyndump_add_cyc_prefix)prefix=(struct _dyneither_ptr**)& Cyc_Absynpp_cyc_stringptr;
_tmp1=_tmp6;goto _LL10;_LL10:;}if(prefix != 0){Cyc_Absyndump_dump_str(*prefix);if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{const char*
_tmp3A6;Cyc_Absyndump_dump_nospace(((_tmp3A6="::",_tag_dyneither(_tmp3A6,sizeof(
char),3))));}}if(_tmp1 != 0){Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)
_tmp1->hd));for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(Cyc_Absyndump_qvar_to_Cids)
Cyc_Absyndump_dump_char((int)'_');else{const char*_tmp3A7;Cyc_Absyndump_dump_nospace(((
_tmp3A7="::",_tag_dyneither(_tmp3A7,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*((
struct _dyneither_ptr*)_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids){const char*
_tmp3A8;Cyc_Absyndump_dump_nospace(((_tmp3A8="_",_tag_dyneither(_tmp3A8,sizeof(
char),2))));}else{const char*_tmp3A9;Cyc_Absyndump_dump_nospace(((_tmp3A9="::",
_tag_dyneither(_tmp3A9,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*(*v).f2);}
else{if(prefix != 0)Cyc_Absyndump_dump_nospace(*(*v).f2);else{Cyc_Absyndump_dump_str((*
v).f2);}}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){
const char*_tmp3AA;Cyc_Absyndump_dump(((_tmp3AA="restrict",_tag_dyneither(_tmp3AA,
sizeof(char),9))));}if(tq.q_volatile){const char*_tmp3AB;Cyc_Absyndump_dump(((
_tmp3AB="volatile",_tag_dyneither(_tmp3AB,sizeof(char),9))));}if(tq.print_const){
const char*_tmp3AC;Cyc_Absyndump_dump(((_tmp3AC="const",_tag_dyneither(_tmp3AC,
sizeof(char),6))));}}void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){switch(
sc){case Cyc_Absyn_Static: _LL19:{const char*_tmp3AD;Cyc_Absyndump_dump(((_tmp3AD="static",
_tag_dyneither(_tmp3AD,sizeof(char),7))));}return;case Cyc_Absyn_Public: _LL1A:
return;case Cyc_Absyn_Extern: _LL1B:{const char*_tmp3AE;Cyc_Absyndump_dump(((
_tmp3AE="extern",_tag_dyneither(_tmp3AE,sizeof(char),7))));}return;case Cyc_Absyn_ExternC:
_LL1C:{const char*_tmp3AF;Cyc_Absyndump_dump(((_tmp3AF="extern \"C\"",
_tag_dyneither(_tmp3AF,sizeof(char),11))));}return;case Cyc_Absyn_Abstract: _LL1D:{
const char*_tmp3B0;Cyc_Absyndump_dump(((_tmp3B0="abstract",_tag_dyneither(_tmp3B0,
sizeof(char),9))));}return;case Cyc_Absyn_Register: _LL1E:{const char*_tmp3B1;Cyc_Absyndump_dump(((
_tmp3B1="register",_tag_dyneither(_tmp3B1,sizeof(char),9))));}return;}}void Cyc_Absyndump_dumpkind(
struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp14;enum Cyc_Absyn_KindQual
_tmp15;enum Cyc_Absyn_AliasQual _tmp16;struct Cyc_Absyn_Kind*_tmp13=ka;_tmp14=*
_tmp13;_tmp15=_tmp14.kind;_tmp16=_tmp14.aliasqual;switch(_tmp15){case Cyc_Absyn_AnyKind:
_LL20: switch(_tmp16){case Cyc_Absyn_Aliasable: _LL22:{const char*_tmp3B2;Cyc_Absyndump_dump(((
_tmp3B2="A",_tag_dyneither(_tmp3B2,sizeof(char),2))));}return;case Cyc_Absyn_Unique:
_LL23:{const char*_tmp3B3;Cyc_Absyndump_dump(((_tmp3B3="UA",_tag_dyneither(
_tmp3B3,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL24:{const char*_tmp3B4;
Cyc_Absyndump_dump(((_tmp3B4="TA",_tag_dyneither(_tmp3B4,sizeof(char),3))));}
return;}case Cyc_Absyn_MemKind: _LL21: switch(_tmp16){case Cyc_Absyn_Aliasable: _LL27:{
const char*_tmp3B5;Cyc_Absyndump_dump(((_tmp3B5="M",_tag_dyneither(_tmp3B5,
sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL28:{const char*_tmp3B6;Cyc_Absyndump_dump(((
_tmp3B6="UM",_tag_dyneither(_tmp3B6,sizeof(char),3))));}return;case Cyc_Absyn_Top:
_LL29:{const char*_tmp3B7;Cyc_Absyndump_dump(((_tmp3B7="TM",_tag_dyneither(
_tmp3B7,sizeof(char),3))));}return;}case Cyc_Absyn_BoxKind: _LL26: switch(_tmp16){
case Cyc_Absyn_Aliasable: _LL2C:{const char*_tmp3B8;Cyc_Absyndump_dump(((_tmp3B8="B",
_tag_dyneither(_tmp3B8,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL2D:{
const char*_tmp3B9;Cyc_Absyndump_dump(((_tmp3B9="UB",_tag_dyneither(_tmp3B9,
sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL2E:{const char*_tmp3BA;Cyc_Absyndump_dump(((
_tmp3BA="TB",_tag_dyneither(_tmp3BA,sizeof(char),3))));}return;}case Cyc_Absyn_RgnKind:
_LL2B: switch(_tmp16){case Cyc_Absyn_Aliasable: _LL31:{const char*_tmp3BB;Cyc_Absyndump_dump(((
_tmp3BB="R",_tag_dyneither(_tmp3BB,sizeof(char),2))));}return;case Cyc_Absyn_Unique:
_LL32:{const char*_tmp3BC;Cyc_Absyndump_dump(((_tmp3BC="UR",_tag_dyneither(
_tmp3BC,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL33:{const char*_tmp3BD;
Cyc_Absyndump_dump(((_tmp3BD="TR",_tag_dyneither(_tmp3BD,sizeof(char),3))));}
return;}case Cyc_Absyn_EffKind: _LL30:{const char*_tmp3BE;Cyc_Absyndump_dump(((
_tmp3BE="E",_tag_dyneither(_tmp3BE,sizeof(char),2))));}return;case Cyc_Absyn_IntKind:
_LL35:{const char*_tmp3BF;Cyc_Absyndump_dump(((_tmp3BF="I",_tag_dyneither(_tmp3BF,
sizeof(char),2))));}return;}}void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind
k){switch(k){case Cyc_Absyn_StructA: _LL37:{const char*_tmp3C0;Cyc_Absyndump_dump(((
_tmp3C0="struct",_tag_dyneither(_tmp3C0,sizeof(char),7))));}return;case Cyc_Absyn_UnionA:
_LL38:{const char*_tmp3C1;Cyc_Absyndump_dump(((_tmp3C1="union",_tag_dyneither(
_tmp3C1,sizeof(char),6))));}return;}}void Cyc_Absyndump_dumptps(struct Cyc_List_List*
ts){const char*_tmp3C4;const char*_tmp3C3;const char*_tmp3C2;((void(*)(void(*f)(
void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,
struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,ts,((
_tmp3C2="<",_tag_dyneither(_tmp3C2,sizeof(char),2))),((_tmp3C3=">",
_tag_dyneither(_tmp3C3,sizeof(char),2))),((_tmp3C4=",",_tag_dyneither(_tmp3C4,
sizeof(char),2))));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);{void*_tmp2A=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*
_tmp2C;struct Cyc_Absyn_Kind _tmp2D;enum Cyc_Absyn_KindQual _tmp2E;enum Cyc_Absyn_AliasQual
_tmp2F;struct Cyc_Absyn_Kind*_tmp33;_LL3B: {struct Cyc_Absyn_Eq_kb_struct*_tmp2B=(
struct Cyc_Absyn_Eq_kb_struct*)_tmp2A;if(_tmp2B->tag != 0)goto _LL3D;else{_tmp2C=
_tmp2B->f1;_tmp2D=*_tmp2C;_tmp2E=_tmp2D.kind;if(_tmp2E != Cyc_Absyn_BoxKind)goto
_LL3D;_tmp2F=_tmp2D.aliasqual;if(_tmp2F != Cyc_Absyn_Aliasable)goto _LL3D;}}_LL3C:
goto _LL3E;_LL3D: {struct Cyc_Absyn_Less_kb_struct*_tmp30=(struct Cyc_Absyn_Less_kb_struct*)
_tmp2A;if(_tmp30->tag != 2)goto _LL3F;}_LL3E: goto _LL40;_LL3F: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp31=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp2A;if(_tmp31->tag != 1)goto _LL41;}
_LL40:{const char*_tmp3C5;Cyc_Absyndump_dump(((_tmp3C5="::?",_tag_dyneither(
_tmp3C5,sizeof(char),4))));}goto _LL3A;_LL41: {struct Cyc_Absyn_Eq_kb_struct*
_tmp32=(struct Cyc_Absyn_Eq_kb_struct*)_tmp2A;if(_tmp32->tag != 0)goto _LL3A;else{
_tmp33=_tmp32->f1;}}_LL42:{const char*_tmp3C6;Cyc_Absyndump_dump(((_tmp3C6="::",
_tag_dyneither(_tmp3C6,sizeof(char),3))));}Cyc_Absyndump_dumpkind(_tmp33);goto
_LL3A;_LL3A:;};}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){const char*
_tmp3C9;const char*_tmp3C8;const char*_tmp3C7;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,((_tmp3C7="<",
_tag_dyneither(_tmp3C7,sizeof(char),2))),((_tmp3C8=">",_tag_dyneither(_tmp3C8,
sizeof(char),2))),((_tmp3C9=",",_tag_dyneither(_tmp3C9,sizeof(char),2))));}void
Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){const char*_tmp3CC;const
char*_tmp3CB;const char*_tmp3CA;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,((
_tmp3CA="<",_tag_dyneither(_tmp3CA,sizeof(char),2))),((_tmp3CB=">",
_tag_dyneither(_tmp3CB,sizeof(char),2))),((_tmp3CC=",",_tag_dyneither(_tmp3CC,
sizeof(char),2))));}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(
struct _tuple13*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){const char*_tmp3CF;const char*_tmp3CE;const char*_tmp3CD;((
void(*)(void(*f)(struct _tuple13*),struct Cyc_List_List*l,struct _dyneither_ptr
start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,
ts,((_tmp3CD="(",_tag_dyneither(_tmp3CD,sizeof(char),2))),((_tmp3CE=")",
_tag_dyneither(_tmp3CE,sizeof(char),2))),((_tmp3CF=",",_tag_dyneither(_tmp3CF,
sizeof(char),2))));}void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp3F=(void*)atts->hd;_LL44: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp40=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp3F;if(_tmp40->tag != 1)goto _LL46;}
_LL45:{const char*_tmp3D0;Cyc_Absyndump_dump(((_tmp3D0="_stdcall",_tag_dyneither(
_tmp3D0,sizeof(char),9))));}return;_LL46: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp41=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp3F;if(_tmp41->tag != 2)goto _LL48;}
_LL47:{const char*_tmp3D1;Cyc_Absyndump_dump(((_tmp3D1="_cdecl",_tag_dyneither(
_tmp3D1,sizeof(char),7))));}return;_LL48: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp42=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp3F;if(_tmp42->tag != 3)goto _LL4A;}
_LL49:{const char*_tmp3D2;Cyc_Absyndump_dump(((_tmp3D2="_fastcall",_tag_dyneither(
_tmp3D2,sizeof(char),10))));}return;_LL4A:;_LL4B: goto _LL43;_LL43:;}}void Cyc_Absyndump_dump_noncallconv(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmp46=(void*)((struct Cyc_List_List*)_check_null(
atts))->hd;_LL4D: {struct Cyc_Absyn_Stdcall_att_struct*_tmp47=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp46;if(_tmp47->tag != 1)goto _LL4F;}_LL4E: goto _LL50;_LL4F: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp48=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp46;if(_tmp48->tag != 2)goto _LL51;}
_LL50: goto _LL52;_LL51: {struct Cyc_Absyn_Fastcall_att_struct*_tmp49=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp46;if(_tmp49->tag != 3)goto _LL53;}_LL52: goto _LL4C;_LL53:;_LL54: hasatt=1;goto
_LL4C;_LL4C:;}}if(!hasatt)return;{const char*_tmp3D3;Cyc_Absyndump_dump(((_tmp3D3="__declspec(",
_tag_dyneither(_tmp3D3,sizeof(char),12))));}for(0;atts != 0;atts=atts->tl){void*
_tmp4B=(void*)atts->hd;_LL56: {struct Cyc_Absyn_Stdcall_att_struct*_tmp4C=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmp4B;if(_tmp4C->tag != 1)goto _LL58;}_LL57: goto
_LL59;_LL58: {struct Cyc_Absyn_Cdecl_att_struct*_tmp4D=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp4B;if(_tmp4D->tag != 2)goto _LL5A;}_LL59: goto _LL5B;_LL5A: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp4E=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp4B;if(_tmp4E->tag != 3)goto _LL5C;}
_LL5B: goto _LL55;_LL5C:;_LL5D: Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)
atts->hd));goto _LL55;_LL55:;}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL5E:{const char*_tmp3D4;Cyc_Absyndump_dump(((_tmp3D4=" __attribute__((",
_tag_dyneither(_tmp3D4,sizeof(char),17))));}for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0){const char*_tmp3D5;
Cyc_Absyndump_dump(((_tmp3D5=",",_tag_dyneither(_tmp3D5,sizeof(char),2))));}}{
const char*_tmp3D6;Cyc_Absyndump_dump(((_tmp3D6=")) ",_tag_dyneither(_tmp3D6,
sizeof(char),4))));}return;case Cyc_Cyclone_Vc_c: _LL5F: Cyc_Absyndump_dump_noncallconv(
atts);return;}}int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){if(tms
== 0)return 0;{void*_tmp52=(void*)tms->hd;_LL62: {struct Cyc_Absyn_Pointer_mod_struct*
_tmp53=(struct Cyc_Absyn_Pointer_mod_struct*)_tmp52;if(_tmp53->tag != 2)goto _LL64;}
_LL63: return 1;_LL64:;_LL65: return 0;_LL61:;};}static void Cyc_Absyndump_dumprgn(
void*t){void*_tmp54=Cyc_Tcutil_compress(t);_LL67: {struct Cyc_Absyn_HeapRgn_struct*
_tmp55=(struct Cyc_Absyn_HeapRgn_struct*)_tmp54;if(_tmp55->tag != 21)goto _LL69;}
_LL68:{const char*_tmp3D7;Cyc_Absyndump_dump(((_tmp3D7="`H",_tag_dyneither(
_tmp3D7,sizeof(char),3))));}goto _LL66;_LL69:;_LL6A: Cyc_Absyndump_dumpntyp(t);
goto _LL66;_LL66:;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;}
;static struct _tuple14 Cyc_Absyndump_effects_split(void*t){struct Cyc_List_List*
rgions=0;struct Cyc_List_List*effects=0;{void*_tmp57=Cyc_Tcutil_compress(t);void*
_tmp59;struct Cyc_List_List*_tmp5B;_LL6C: {struct Cyc_Absyn_AccessEff_struct*
_tmp58=(struct Cyc_Absyn_AccessEff_struct*)_tmp57;if(_tmp58->tag != 23)goto _LL6E;
else{_tmp59=(void*)_tmp58->f1;}}_LL6D:{struct Cyc_List_List*_tmp3D8;rgions=((
_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8->hd=_tmp59,((_tmp3D8->tl=rgions,
_tmp3D8))))));}goto _LL6B;_LL6E: {struct Cyc_Absyn_JoinEff_struct*_tmp5A=(struct
Cyc_Absyn_JoinEff_struct*)_tmp57;if(_tmp5A->tag != 24)goto _LL70;else{_tmp5B=
_tmp5A->f1;}}_LL6F: for(0;_tmp5B != 0;_tmp5B=_tmp5B->tl){struct Cyc_List_List*
_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple14 _tmp5D=Cyc_Absyndump_effects_split((
void*)_tmp5B->hd);_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;rgions=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp5E,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp5F,effects);}goto _LL6B;_LL70:;_LL71:{struct Cyc_List_List*
_tmp3D9;effects=((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9->hd=t,((_tmp3D9->tl=
effects,_tmp3D9))))));}goto _LL6B;_LL6B:;}{struct _tuple14 _tmp3DA;return(_tmp3DA.f1=
rgions,((_tmp3DA.f2=effects,_tmp3DA)));};}static void Cyc_Absyndump_dumpeff(void*t){
struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp64;struct _tuple14 _tmp62=Cyc_Absyndump_effects_split(
t);_tmp63=_tmp62.f1;_tmp64=_tmp62.f2;_tmp63=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp63);_tmp64=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp64);for(0;_tmp64 != 0;_tmp64=_tmp64->tl){Cyc_Absyndump_dumpntyp((
void*)_tmp64->hd);Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp63 != 0;_tmp63=_tmp63->tl){Cyc_Absyndump_dumprgn((void*)_tmp63->hd);
if(_tmp63->tl != 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((
int)'}');}void Cyc_Absyndump_dumpntyp(void*t){void*_tmp65=t;struct Cyc_Absyn_Tvar*
_tmp6B;struct Cyc_Core_Opt*_tmp6D;struct Cyc_Core_Opt*_tmp6E;int _tmp6F;struct Cyc_Core_Opt*
_tmp71;struct Cyc_Core_Opt*_tmp72;struct Cyc_Core_Opt _tmp73;void*_tmp74;int _tmp75;
struct Cyc_Absyn_DatatypeInfo _tmp77;union Cyc_Absyn_DatatypeInfoU _tmp78;struct Cyc_List_List*
_tmp79;struct Cyc_Absyn_DatatypeFieldInfo _tmp7B;union Cyc_Absyn_DatatypeFieldInfoU
_tmp7C;struct Cyc_List_List*_tmp7D;enum Cyc_Absyn_Sign _tmp7F;enum Cyc_Absyn_Size_of
_tmp80;enum Cyc_Absyn_Sign _tmp82;enum Cyc_Absyn_Size_of _tmp83;enum Cyc_Absyn_Sign
_tmp85;enum Cyc_Absyn_Size_of _tmp86;enum Cyc_Absyn_Sign _tmp88;enum Cyc_Absyn_Size_of
_tmp89;enum Cyc_Absyn_Sign _tmp8B;enum Cyc_Absyn_Size_of _tmp8C;enum Cyc_Absyn_Sign
_tmp8E;enum Cyc_Absyn_Size_of _tmp8F;enum Cyc_Absyn_Sign _tmp91;enum Cyc_Absyn_Size_of
_tmp92;enum Cyc_Absyn_Sign _tmp94;enum Cyc_Absyn_Size_of _tmp95;enum Cyc_Absyn_Sign
_tmp97;enum Cyc_Absyn_Size_of _tmp98;enum Cyc_Absyn_Sign _tmp9A;enum Cyc_Absyn_Size_of
_tmp9B;enum Cyc_Absyn_Sign _tmp9D;enum Cyc_Absyn_Size_of _tmp9E;enum Cyc_Absyn_Sign
_tmpA0;enum Cyc_Absyn_Size_of _tmpA1;enum Cyc_Absyn_Sign _tmpA3;enum Cyc_Absyn_Size_of
_tmpA4;enum Cyc_Absyn_Sign _tmpA6;enum Cyc_Absyn_Size_of _tmpA7;enum Cyc_Absyn_Sign
_tmpA9;enum Cyc_Absyn_Size_of _tmpAA;int _tmpAD;struct Cyc_List_List*_tmpAF;struct
Cyc_Absyn_AggrInfo _tmpB1;union Cyc_Absyn_AggrInfoU _tmpB2;struct Cyc_List_List*
_tmpB3;enum Cyc_Absyn_AggrKind _tmpB5;struct Cyc_List_List*_tmpB6;struct _tuple0*
_tmpB8;struct Cyc_List_List*_tmpBA;struct _tuple0*_tmpBC;struct Cyc_List_List*
_tmpBD;struct Cyc_Absyn_Exp*_tmpBF;void*_tmpC1;void*_tmpC3;void*_tmpC4;void*
_tmpC6;_LL73: {struct Cyc_Absyn_ArrayType_struct*_tmp66=(struct Cyc_Absyn_ArrayType_struct*)
_tmp65;if(_tmp66->tag != 9)goto _LL75;}_LL74: goto _LL76;_LL75: {struct Cyc_Absyn_FnType_struct*
_tmp67=(struct Cyc_Absyn_FnType_struct*)_tmp65;if(_tmp67->tag != 10)goto _LL77;}
_LL76: goto _LL78;_LL77: {struct Cyc_Absyn_PointerType_struct*_tmp68=(struct Cyc_Absyn_PointerType_struct*)
_tmp65;if(_tmp68->tag != 5)goto _LL79;}_LL78: return;_LL79: {struct Cyc_Absyn_VoidType_struct*
_tmp69=(struct Cyc_Absyn_VoidType_struct*)_tmp65;if(_tmp69->tag != 0)goto _LL7B;}
_LL7A:{const char*_tmp3DB;Cyc_Absyndump_dump(((_tmp3DB="void",_tag_dyneither(
_tmp3DB,sizeof(char),5))));}return;_LL7B: {struct Cyc_Absyn_VarType_struct*_tmp6A=(
struct Cyc_Absyn_VarType_struct*)_tmp65;if(_tmp6A->tag != 2)goto _LL7D;else{_tmp6B=
_tmp6A->f1;}}_LL7C: Cyc_Absyndump_dump_str(_tmp6B->name);return;_LL7D: {struct Cyc_Absyn_Evar_struct*
_tmp6C=(struct Cyc_Absyn_Evar_struct*)_tmp65;if(_tmp6C->tag != 1)goto _LL7F;else{
_tmp6D=_tmp6C->f1;_tmp6E=_tmp6C->f2;if(_tmp6E != 0)goto _LL7F;_tmp6F=_tmp6C->f3;}}
_LL7E:{const char*_tmp3DC;Cyc_Absyndump_dump(((_tmp3DC="%",_tag_dyneither(_tmp3DC,
sizeof(char),2))));}if(_tmp6D == 0){const char*_tmp3DD;Cyc_Absyndump_dump(((
_tmp3DD="?",_tag_dyneither(_tmp3DD,sizeof(char),2))));}else{Cyc_Absyndump_dumpkind((
struct Cyc_Absyn_Kind*)_tmp6D->v);}{const char*_tmp3E1;void*_tmp3E0[1];struct Cyc_Int_pa_struct
_tmp3DF;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp3DF.tag=1,((_tmp3DF.f1=(
unsigned long)_tmp6F,((_tmp3E0[0]=& _tmp3DF,Cyc_aprintf(((_tmp3E1="(%d)",
_tag_dyneither(_tmp3E1,sizeof(char),5))),_tag_dyneither(_tmp3E0,sizeof(void*),1)))))))));}
return;_LL7F: {struct Cyc_Absyn_Evar_struct*_tmp70=(struct Cyc_Absyn_Evar_struct*)
_tmp65;if(_tmp70->tag != 1)goto _LL81;else{_tmp71=_tmp70->f1;_tmp72=_tmp70->f2;if(
_tmp72 == 0)goto _LL81;_tmp73=*_tmp72;_tmp74=(void*)_tmp73.v;_tmp75=_tmp70->f3;}}
_LL80: Cyc_Absyndump_dumpntyp(_tmp74);return;_LL81: {struct Cyc_Absyn_DatatypeType_struct*
_tmp76=(struct Cyc_Absyn_DatatypeType_struct*)_tmp65;if(_tmp76->tag != 3)goto _LL83;
else{_tmp77=_tmp76->f1;_tmp78=_tmp77.datatype_info;_tmp79=_tmp77.targs;}}_LL82:{
union Cyc_Absyn_DatatypeInfoU _tmpD2=_tmp78;struct Cyc_Absyn_UnknownDatatypeInfo
_tmpD3;struct _tuple0*_tmpD4;int _tmpD5;struct Cyc_Absyn_Datatypedecl**_tmpD6;
struct Cyc_Absyn_Datatypedecl*_tmpD7;struct Cyc_Absyn_Datatypedecl _tmpD8;struct
_tuple0*_tmpD9;int _tmpDA;_LLC6: if((_tmpD2.UnknownDatatype).tag != 1)goto _LLC8;
_tmpD3=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD2.UnknownDatatype).val;_tmpD4=
_tmpD3.name;_tmpD5=_tmpD3.is_extensible;_LLC7: _tmpD9=_tmpD4;_tmpDA=_tmpD5;goto
_LLC9;_LLC8: if((_tmpD2.KnownDatatype).tag != 2)goto _LLC5;_tmpD6=(struct Cyc_Absyn_Datatypedecl**)(
_tmpD2.KnownDatatype).val;_tmpD7=*_tmpD6;_tmpD8=*_tmpD7;_tmpD9=_tmpD8.name;
_tmpDA=_tmpD8.is_extensible;_LLC9: if(_tmpDA){const char*_tmp3E2;Cyc_Absyndump_dump(((
_tmp3E2="@extensible ",_tag_dyneither(_tmp3E2,sizeof(char),13))));}{const char*
_tmp3E3;Cyc_Absyndump_dump(((_tmp3E3="datatype ",_tag_dyneither(_tmp3E3,sizeof(
char),10))));}Cyc_Absyndump_dumpqvar(_tmpD9);Cyc_Absyndump_dumptps(_tmp79);goto
_LLC5;_LLC5:;}goto _LL72;_LL83: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp7A=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp65;if(_tmp7A->tag != 4)goto _LL85;
else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.field_info;_tmp7D=_tmp7B.targs;}}_LL84:{
union Cyc_Absyn_DatatypeFieldInfoU _tmpDD=_tmp7C;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpDE;struct _tuple0*_tmpDF;struct _tuple0*_tmpE0;int _tmpE1;struct _tuple1 _tmpE2;
struct Cyc_Absyn_Datatypedecl*_tmpE3;struct Cyc_Absyn_Datatypedecl _tmpE4;struct
_tuple0*_tmpE5;int _tmpE6;struct Cyc_Absyn_Datatypefield*_tmpE7;struct Cyc_Absyn_Datatypefield
_tmpE8;struct _tuple0*_tmpE9;_LLCB: if((_tmpDD.UnknownDatatypefield).tag != 1)goto
_LLCD;_tmpDE=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpDD.UnknownDatatypefield).val;
_tmpDF=_tmpDE.datatype_name;_tmpE0=_tmpDE.field_name;_tmpE1=_tmpDE.is_extensible;
_LLCC: _tmpE5=_tmpDF;_tmpE6=_tmpE1;_tmpE9=_tmpE0;goto _LLCE;_LLCD: if((_tmpDD.KnownDatatypefield).tag
!= 2)goto _LLCA;_tmpE2=(struct _tuple1)(_tmpDD.KnownDatatypefield).val;_tmpE3=
_tmpE2.f1;_tmpE4=*_tmpE3;_tmpE5=_tmpE4.name;_tmpE6=_tmpE4.is_extensible;_tmpE7=
_tmpE2.f2;_tmpE8=*_tmpE7;_tmpE9=_tmpE8.name;_LLCE: if(_tmpE6){const char*_tmp3E4;
Cyc_Absyndump_dump(((_tmp3E4="@extensible ",_tag_dyneither(_tmp3E4,sizeof(char),
13))));}{const char*_tmp3E5;Cyc_Absyndump_dump(((_tmp3E5="datatype ",
_tag_dyneither(_tmp3E5,sizeof(char),10))));}Cyc_Absyndump_dumpqvar(_tmpE5);{
const char*_tmp3E6;Cyc_Absyndump_dump(((_tmp3E6=".",_tag_dyneither(_tmp3E6,
sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmpE9);Cyc_Absyndump_dumptps(_tmp7D);
goto _LLCA;_LLCA:;}goto _LL72;_LL85: {struct Cyc_Absyn_IntType_struct*_tmp7E=(
struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp7E->tag != 6)goto _LL87;else{_tmp7F=
_tmp7E->f1;if(_tmp7F != Cyc_Absyn_None)goto _LL87;_tmp80=_tmp7E->f2;if(_tmp80 != 
Cyc_Absyn_Int_sz)goto _LL87;}}_LL86: goto _LL88;_LL87: {struct Cyc_Absyn_IntType_struct*
_tmp81=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp81->tag != 6)goto _LL89;
else{_tmp82=_tmp81->f1;if(_tmp82 != Cyc_Absyn_Signed)goto _LL89;_tmp83=_tmp81->f2;
if(_tmp83 != Cyc_Absyn_Int_sz)goto _LL89;}}_LL88:{const char*_tmp3E7;Cyc_Absyndump_dump(((
_tmp3E7="int",_tag_dyneither(_tmp3E7,sizeof(char),4))));}return;_LL89: {struct
Cyc_Absyn_IntType_struct*_tmp84=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(
_tmp84->tag != 6)goto _LL8B;else{_tmp85=_tmp84->f1;if(_tmp85 != Cyc_Absyn_None)goto
_LL8B;_tmp86=_tmp84->f2;if(_tmp86 != Cyc_Absyn_Long_sz)goto _LL8B;}}_LL8A: goto
_LL8C;_LL8B: {struct Cyc_Absyn_IntType_struct*_tmp87=(struct Cyc_Absyn_IntType_struct*)
_tmp65;if(_tmp87->tag != 6)goto _LL8D;else{_tmp88=_tmp87->f1;if(_tmp88 != Cyc_Absyn_Signed)
goto _LL8D;_tmp89=_tmp87->f2;if(_tmp89 != Cyc_Absyn_Long_sz)goto _LL8D;}}_LL8C:{
const char*_tmp3E8;Cyc_Absyndump_dump(((_tmp3E8="long",_tag_dyneither(_tmp3E8,
sizeof(char),5))));}return;_LL8D: {struct Cyc_Absyn_IntType_struct*_tmp8A=(struct
Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp8A->tag != 6)goto _LL8F;else{_tmp8B=_tmp8A->f1;
if(_tmp8B != Cyc_Absyn_None)goto _LL8F;_tmp8C=_tmp8A->f2;if(_tmp8C != Cyc_Absyn_Char_sz)
goto _LL8F;}}_LL8E:{const char*_tmp3E9;Cyc_Absyndump_dump(((_tmp3E9="char",
_tag_dyneither(_tmp3E9,sizeof(char),5))));}return;_LL8F: {struct Cyc_Absyn_IntType_struct*
_tmp8D=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp8D->tag != 6)goto _LL91;
else{_tmp8E=_tmp8D->f1;if(_tmp8E != Cyc_Absyn_Signed)goto _LL91;_tmp8F=_tmp8D->f2;
if(_tmp8F != Cyc_Absyn_Char_sz)goto _LL91;}}_LL90:{const char*_tmp3EA;Cyc_Absyndump_dump(((
_tmp3EA="signed char",_tag_dyneither(_tmp3EA,sizeof(char),12))));}return;_LL91: {
struct Cyc_Absyn_IntType_struct*_tmp90=(struct Cyc_Absyn_IntType_struct*)_tmp65;
if(_tmp90->tag != 6)goto _LL93;else{_tmp91=_tmp90->f1;if(_tmp91 != Cyc_Absyn_Unsigned)
goto _LL93;_tmp92=_tmp90->f2;if(_tmp92 != Cyc_Absyn_Char_sz)goto _LL93;}}_LL92:{
const char*_tmp3EB;Cyc_Absyndump_dump(((_tmp3EB="unsigned char",_tag_dyneither(
_tmp3EB,sizeof(char),14))));}return;_LL93: {struct Cyc_Absyn_IntType_struct*
_tmp93=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp93->tag != 6)goto _LL95;
else{_tmp94=_tmp93->f1;if(_tmp94 != Cyc_Absyn_None)goto _LL95;_tmp95=_tmp93->f2;
if(_tmp95 != Cyc_Absyn_Short_sz)goto _LL95;}}_LL94: goto _LL96;_LL95: {struct Cyc_Absyn_IntType_struct*
_tmp96=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp96->tag != 6)goto _LL97;
else{_tmp97=_tmp96->f1;if(_tmp97 != Cyc_Absyn_Signed)goto _LL97;_tmp98=_tmp96->f2;
if(_tmp98 != Cyc_Absyn_Short_sz)goto _LL97;}}_LL96:{const char*_tmp3EC;Cyc_Absyndump_dump(((
_tmp3EC="short",_tag_dyneither(_tmp3EC,sizeof(char),6))));}return;_LL97: {struct
Cyc_Absyn_IntType_struct*_tmp99=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(
_tmp99->tag != 6)goto _LL99;else{_tmp9A=_tmp99->f1;if(_tmp9A != Cyc_Absyn_Unsigned)
goto _LL99;_tmp9B=_tmp99->f2;if(_tmp9B != Cyc_Absyn_Short_sz)goto _LL99;}}_LL98:{
const char*_tmp3ED;Cyc_Absyndump_dump(((_tmp3ED="unsigned short",_tag_dyneither(
_tmp3ED,sizeof(char),15))));}return;_LL99: {struct Cyc_Absyn_IntType_struct*
_tmp9C=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmp9C->tag != 6)goto _LL9B;
else{_tmp9D=_tmp9C->f1;if(_tmp9D != Cyc_Absyn_Unsigned)goto _LL9B;_tmp9E=_tmp9C->f2;
if(_tmp9E != Cyc_Absyn_Int_sz)goto _LL9B;}}_LL9A:{const char*_tmp3EE;Cyc_Absyndump_dump(((
_tmp3EE="unsigned int",_tag_dyneither(_tmp3EE,sizeof(char),13))));}return;_LL9B: {
struct Cyc_Absyn_IntType_struct*_tmp9F=(struct Cyc_Absyn_IntType_struct*)_tmp65;
if(_tmp9F->tag != 6)goto _LL9D;else{_tmpA0=_tmp9F->f1;if(_tmpA0 != Cyc_Absyn_Unsigned)
goto _LL9D;_tmpA1=_tmp9F->f2;if(_tmpA1 != Cyc_Absyn_Long_sz)goto _LL9D;}}_LL9C:{
const char*_tmp3EF;Cyc_Absyndump_dump(((_tmp3EF="unsigned long",_tag_dyneither(
_tmp3EF,sizeof(char),14))));}return;_LL9D: {struct Cyc_Absyn_IntType_struct*
_tmpA2=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmpA2->tag != 6)goto _LL9F;
else{_tmpA3=_tmpA2->f1;if(_tmpA3 != Cyc_Absyn_None)goto _LL9F;_tmpA4=_tmpA2->f2;
if(_tmpA4 != Cyc_Absyn_LongLong_sz)goto _LL9F;}}_LL9E: goto _LLA0;_LL9F: {struct Cyc_Absyn_IntType_struct*
_tmpA5=(struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmpA5->tag != 6)goto _LLA1;
else{_tmpA6=_tmpA5->f1;if(_tmpA6 != Cyc_Absyn_Signed)goto _LLA1;_tmpA7=_tmpA5->f2;
if(_tmpA7 != Cyc_Absyn_LongLong_sz)goto _LLA1;}}_LLA0: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLCF:{const char*_tmp3F0;Cyc_Absyndump_dump(((_tmp3F0="long long",
_tag_dyneither(_tmp3F0,sizeof(char),10))));}return;case Cyc_Cyclone_Vc_c: _LLD0:{
const char*_tmp3F1;Cyc_Absyndump_dump(((_tmp3F1="__int64",_tag_dyneither(_tmp3F1,
sizeof(char),8))));}return;}_LLA1: {struct Cyc_Absyn_IntType_struct*_tmpA8=(
struct Cyc_Absyn_IntType_struct*)_tmp65;if(_tmpA8->tag != 6)goto _LLA3;else{_tmpA9=
_tmpA8->f1;if(_tmpA9 != Cyc_Absyn_Unsigned)goto _LLA3;_tmpAA=_tmpA8->f2;if(_tmpAA
!= Cyc_Absyn_LongLong_sz)goto _LLA3;}}_LLA2: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Vc_c: _LLD2:{const char*_tmp3F2;Cyc_Absyndump_dump(((_tmp3F2="unsigned __int64",
_tag_dyneither(_tmp3F2,sizeof(char),17))));}return;case Cyc_Cyclone_Gcc_c: _LLD3:{
const char*_tmp3F3;Cyc_Absyndump_dump(((_tmp3F3="unsigned long long",
_tag_dyneither(_tmp3F3,sizeof(char),19))));}return;}_LLA3: {struct Cyc_Absyn_FloatType_struct*
_tmpAB=(struct Cyc_Absyn_FloatType_struct*)_tmp65;if(_tmpAB->tag != 7)goto _LLA5;}
_LLA4:{const char*_tmp3F4;Cyc_Absyndump_dump(((_tmp3F4="float",_tag_dyneither(
_tmp3F4,sizeof(char),6))));}return;_LLA5: {struct Cyc_Absyn_DoubleType_struct*
_tmpAC=(struct Cyc_Absyn_DoubleType_struct*)_tmp65;if(_tmpAC->tag != 8)goto _LLA7;
else{_tmpAD=_tmpAC->f1;}}_LLA6: if(_tmpAD){const char*_tmp3F5;Cyc_Absyndump_dump(((
_tmp3F5="long double",_tag_dyneither(_tmp3F5,sizeof(char),12))));}else{const char*
_tmp3F6;Cyc_Absyndump_dump(((_tmp3F6="double",_tag_dyneither(_tmp3F6,sizeof(char),
7))));}return;_LLA7: {struct Cyc_Absyn_TupleType_struct*_tmpAE=(struct Cyc_Absyn_TupleType_struct*)
_tmp65;if(_tmpAE->tag != 11)goto _LLA9;else{_tmpAF=_tmpAE->f1;}}_LLA8: Cyc_Absyndump_dump_char((
int)'$');Cyc_Absyndump_dumpargs(_tmpAF);return;_LLA9: {struct Cyc_Absyn_AggrType_struct*
_tmpB0=(struct Cyc_Absyn_AggrType_struct*)_tmp65;if(_tmpB0->tag != 12)goto _LLAB;
else{_tmpB1=_tmpB0->f1;_tmpB2=_tmpB1.aggr_info;_tmpB3=_tmpB1.targs;}}_LLAA: {
enum Cyc_Absyn_AggrKind _tmpFE;struct _tuple0*_tmpFF;struct _tuple9 _tmpFD=Cyc_Absyn_aggr_kinded_name(
_tmpB2);_tmpFE=_tmpFD.f1;_tmpFF=_tmpFD.f2;Cyc_Absyndump_dumpaggr_kind(_tmpFE);
Cyc_Absyndump_dumpqvar(_tmpFF);Cyc_Absyndump_dumptps(_tmpB3);return;}_LLAB: {
struct Cyc_Absyn_AnonAggrType_struct*_tmpB4=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp65;if(_tmpB4->tag != 13)goto _LLAD;else{_tmpB5=_tmpB4->f1;_tmpB6=_tmpB4->f2;}}
_LLAC: Cyc_Absyndump_dumpaggr_kind(_tmpB5);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(
_tmpB6);Cyc_Absyndump_dump_char((int)'}');return;_LLAD: {struct Cyc_Absyn_EnumType_struct*
_tmpB7=(struct Cyc_Absyn_EnumType_struct*)_tmp65;if(_tmpB7->tag != 14)goto _LLAF;
else{_tmpB8=_tmpB7->f1;}}_LLAE:{const char*_tmp3F7;Cyc_Absyndump_dump(((_tmp3F7="enum ",
_tag_dyneither(_tmp3F7,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmpB8);
return;_LLAF: {struct Cyc_Absyn_AnonEnumType_struct*_tmpB9=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp65;if(_tmpB9->tag != 15)goto _LLB1;else{_tmpBA=_tmpB9->f1;}}_LLB0:{const char*
_tmp3F8;Cyc_Absyndump_dump(((_tmp3F8="enum {",_tag_dyneither(_tmp3F8,sizeof(char),
7))));}Cyc_Absyndump_dumpenumfields(_tmpBA);{const char*_tmp3F9;Cyc_Absyndump_dump(((
_tmp3F9="}",_tag_dyneither(_tmp3F9,sizeof(char),2))));}return;_LLB1: {struct Cyc_Absyn_TypedefType_struct*
_tmpBB=(struct Cyc_Absyn_TypedefType_struct*)_tmp65;if(_tmpBB->tag != 18)goto _LLB3;
else{_tmpBC=_tmpBB->f1;_tmpBD=_tmpBB->f2;}}_LLB2:(Cyc_Absyndump_dumpqvar(_tmpBC),
Cyc_Absyndump_dumptps(_tmpBD));return;_LLB3: {struct Cyc_Absyn_ValueofType_struct*
_tmpBE=(struct Cyc_Absyn_ValueofType_struct*)_tmp65;if(_tmpBE->tag != 19)goto _LLB5;
else{_tmpBF=_tmpBE->f1;}}_LLB4:{const char*_tmp3FA;Cyc_Absyndump_dump(((_tmp3FA="valueof_t(",
_tag_dyneither(_tmp3FA,sizeof(char),11))));}Cyc_Absyndump_dumpexp(_tmpBF);{const
char*_tmp3FB;Cyc_Absyndump_dump(((_tmp3FB=")",_tag_dyneither(_tmp3FB,sizeof(char),
2))));}return;_LLB5: {struct Cyc_Absyn_RgnHandleType_struct*_tmpC0=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp65;if(_tmpC0->tag != 16)goto _LLB7;else{_tmpC1=(void*)_tmpC0->f1;}}_LLB6:{
const char*_tmp3FC;Cyc_Absyndump_dump(((_tmp3FC="region_t<",_tag_dyneither(
_tmp3FC,sizeof(char),10))));}Cyc_Absyndump_dumprgn(_tmpC1);{const char*_tmp3FD;
Cyc_Absyndump_dump(((_tmp3FD=">",_tag_dyneither(_tmp3FD,sizeof(char),2))));}
return;_LLB7: {struct Cyc_Absyn_DynRgnType_struct*_tmpC2=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp65;if(_tmpC2->tag != 17)goto _LLB9;else{_tmpC3=(void*)_tmpC2->f1;_tmpC4=(void*)
_tmpC2->f2;}}_LLB8:{const char*_tmp3FE;Cyc_Absyndump_dump(((_tmp3FE="dynregion_t<",
_tag_dyneither(_tmp3FE,sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmpC3);{const
char*_tmp3FF;Cyc_Absyndump_dump(((_tmp3FF=",",_tag_dyneither(_tmp3FF,sizeof(char),
2))));}Cyc_Absyndump_dumprgn(_tmpC4);{const char*_tmp400;Cyc_Absyndump_dump(((
_tmp400=">",_tag_dyneither(_tmp400,sizeof(char),2))));}return;_LLB9: {struct Cyc_Absyn_TagType_struct*
_tmpC5=(struct Cyc_Absyn_TagType_struct*)_tmp65;if(_tmpC5->tag != 20)goto _LLBB;
else{_tmpC6=(void*)_tmpC5->f1;}}_LLBA:{const char*_tmp401;Cyc_Absyndump_dump(((
_tmp401="tag_t<",_tag_dyneither(_tmp401,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(
_tmpC6);{const char*_tmp402;Cyc_Absyndump_dump(((_tmp402=">",_tag_dyneither(
_tmp402,sizeof(char),2))));}return;_LLBB: {struct Cyc_Absyn_UniqueRgn_struct*
_tmpC7=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp65;if(_tmpC7->tag != 22)goto _LLBD;}
_LLBC:{const char*_tmp403;Cyc_Absyndump_dump(((_tmp403="`U",_tag_dyneither(
_tmp403,sizeof(char),3))));}goto _LL72;_LLBD: {struct Cyc_Absyn_HeapRgn_struct*
_tmpC8=(struct Cyc_Absyn_HeapRgn_struct*)_tmp65;if(_tmpC8->tag != 21)goto _LLBF;}
_LLBE: goto _LLC0;_LLBF: {struct Cyc_Absyn_AccessEff_struct*_tmpC9=(struct Cyc_Absyn_AccessEff_struct*)
_tmp65;if(_tmpC9->tag != 23)goto _LLC1;}_LLC0: goto _LLC2;_LLC1: {struct Cyc_Absyn_RgnsEff_struct*
_tmpCA=(struct Cyc_Absyn_RgnsEff_struct*)_tmp65;if(_tmpCA->tag != 25)goto _LLC3;}
_LLC2: goto _LLC4;_LLC3: {struct Cyc_Absyn_JoinEff_struct*_tmpCB=(struct Cyc_Absyn_JoinEff_struct*)
_tmp65;if(_tmpCB->tag != 24)goto _LL72;}_LLC4: return;_LL72:;}void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt*vo){if(vo != 0)Cyc_Absyndump_dump_str((struct _dyneither_ptr*)
vo->v);}void Cyc_Absyndump_dumpfunarg(struct _tuple7*t){((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*
t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple15{void*f1;void*f2;
};void Cyc_Absyndump_dump_rgncmp(struct _tuple15*cmp){struct _tuple15 _tmp10E;void*
_tmp10F;void*_tmp110;struct _tuple15*_tmp10D=cmp;_tmp10E=*_tmp10D;_tmp10F=_tmp10E.f1;
_tmp110=_tmp10E.f2;Cyc_Absyndump_dumpeff(_tmp10F);Cyc_Absyndump_dump_char((int)'>');
Cyc_Absyndump_dumprgn(_tmp110);}void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*
rgn_po){const char*_tmp404;((void(*)(void(*f)(struct _tuple15*),struct Cyc_List_List*
l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,
rgn_po,((_tmp404=",",_tag_dyneither(_tmp404,sizeof(char),2))));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple7*)
args->hd);if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs){const char*_tmp405;Cyc_Absyndump_dump(((_tmp405="...",
_tag_dyneither(_tmp405,sizeof(char),4))));}else{if(cyc_varargs != 0){struct
_tuple7*_tmp406;struct _tuple7*_tmp113=(_tmp406=_cycalloc(sizeof(*_tmp406)),((
_tmp406->f1=cyc_varargs->name,((_tmp406->f2=cyc_varargs->tq,((_tmp406->f3=
cyc_varargs->type,_tmp406)))))));{const char*_tmp407;Cyc_Absyndump_dump(((_tmp407="...",
_tag_dyneither(_tmp407,sizeof(char),4))));}if(cyc_varargs->inject){const char*
_tmp408;Cyc_Absyndump_dump(((_tmp408=" inject ",_tag_dyneither(_tmp408,sizeof(
char),9))));}Cyc_Absyndump_dumpfunarg(_tmp113);}}if(effopt != 0){Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((int)')');}
void Cyc_Absyndump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,
0);}void Cyc_Absyndump_dumpdesignator(void*d){void*_tmp117=d;struct Cyc_Absyn_Exp*
_tmp119;struct _dyneither_ptr*_tmp11B;_LLD6: {struct Cyc_Absyn_ArrayElement_struct*
_tmp118=(struct Cyc_Absyn_ArrayElement_struct*)_tmp117;if(_tmp118->tag != 0)goto
_LLD8;else{_tmp119=_tmp118->f1;}}_LLD7:{const char*_tmp409;Cyc_Absyndump_dump(((
_tmp409=".[",_tag_dyneither(_tmp409,sizeof(char),3))));}Cyc_Absyndump_dumpexp(
_tmp119);Cyc_Absyndump_dump_char((int)']');goto _LLD5;_LLD8: {struct Cyc_Absyn_FieldName_struct*
_tmp11A=(struct Cyc_Absyn_FieldName_struct*)_tmp117;if(_tmp11A->tag != 1)goto _LLD5;
else{_tmp11B=_tmp11A->f1;}}_LLD9: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp11B);goto _LLD5;_LLD5:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple16*de){{const char*_tmp40C;const char*
_tmp40B;const char*_tmp40A;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(*de).f1,((_tmp40A="",_tag_dyneither(_tmp40A,
sizeof(char),1))),((_tmp40B="=",_tag_dyneither(_tmp40B,sizeof(char),2))),((
_tmp40C="=",_tag_dyneither(_tmp40C,sizeof(char),2))));}Cyc_Absyndump_dumpexp((*
de).f2);}void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){const
char*_tmp40F;const char*_tmp40E;const char*_tmp40D;((void(*)(void(*f)(int,struct
Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,
inprec,es,((_tmp40D="",_tag_dyneither(_tmp40D,sizeof(char),1))),((_tmp40E="",
_tag_dyneither(_tmp40E,sizeof(char),1))),((_tmp40F=",",_tag_dyneither(_tmp40F,
sizeof(char),2))));}void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*
e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec){const char*_tmp410;Cyc_Absyndump_dump_nospace(((
_tmp410="(",_tag_dyneither(_tmp410,sizeof(char),2))));}{void*_tmp124=e->r;union
Cyc_Absyn_Cnst _tmp126;struct _tuple3 _tmp127;enum Cyc_Absyn_Sign _tmp128;char
_tmp129;union Cyc_Absyn_Cnst _tmp12B;struct _tuple4 _tmp12C;enum Cyc_Absyn_Sign
_tmp12D;short _tmp12E;union Cyc_Absyn_Cnst _tmp130;struct _tuple5 _tmp131;enum Cyc_Absyn_Sign
_tmp132;int _tmp133;union Cyc_Absyn_Cnst _tmp135;struct _tuple5 _tmp136;enum Cyc_Absyn_Sign
_tmp137;int _tmp138;union Cyc_Absyn_Cnst _tmp13A;struct _tuple5 _tmp13B;enum Cyc_Absyn_Sign
_tmp13C;int _tmp13D;union Cyc_Absyn_Cnst _tmp13F;struct _tuple6 _tmp140;enum Cyc_Absyn_Sign
_tmp141;long long _tmp142;union Cyc_Absyn_Cnst _tmp144;struct _dyneither_ptr _tmp145;
union Cyc_Absyn_Cnst _tmp147;int _tmp148;union Cyc_Absyn_Cnst _tmp14A;struct
_dyneither_ptr _tmp14B;struct _tuple0*_tmp14D;struct _tuple0*_tmp14F;enum Cyc_Absyn_Primop
_tmp151;struct Cyc_List_List*_tmp152;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_Core_Opt*
_tmp155;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp158;enum Cyc_Absyn_Incrementor
_tmp159;struct Cyc_Absyn_Exp*_tmp15B;enum Cyc_Absyn_Incrementor _tmp15C;struct Cyc_Absyn_Exp*
_tmp15E;enum Cyc_Absyn_Incrementor _tmp15F;struct Cyc_Absyn_Exp*_tmp161;enum Cyc_Absyn_Incrementor
_tmp162;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*
_tmp166;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*
_tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Exp*_tmp16E;struct Cyc_Absyn_Exp*
_tmp16F;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_List_List*_tmp172;struct Cyc_Absyn_Exp*
_tmp174;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Exp*_tmp177;struct Cyc_Absyn_Exp*
_tmp179;struct Cyc_Absyn_Exp*_tmp17B;void*_tmp17D;struct Cyc_Absyn_Exp*_tmp17E;
struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_Absyn_Exp*
_tmp183;void*_tmp185;struct Cyc_Absyn_Exp*_tmp187;void*_tmp189;int _tmp18B;struct
_dyneither_ptr _tmp18C;struct Cyc_Absyn_Exp*_tmp18E;struct _dyneither_ptr*_tmp18F;
void*_tmp191;void*_tmp192;struct _dyneither_ptr*_tmp194;void*_tmp196;void*_tmp197;
unsigned int _tmp199;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19D;
struct _dyneither_ptr*_tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;struct _dyneither_ptr*
_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_List_List*
_tmp1A6;struct _tuple7*_tmp1A8;struct Cyc_List_List*_tmp1A9;struct Cyc_List_List*
_tmp1AB;struct Cyc_Absyn_Vardecl*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AE;struct Cyc_Absyn_Exp*
_tmp1AF;struct _tuple0*_tmp1B1;struct Cyc_List_List*_tmp1B2;struct Cyc_List_List*
_tmp1B3;struct Cyc_List_List*_tmp1B5;struct Cyc_List_List*_tmp1B7;struct Cyc_Absyn_Datatypefield*
_tmp1B8;struct _tuple0*_tmp1BA;struct _tuple0*_tmp1BC;struct Cyc_Absyn_MallocInfo
_tmp1BE;int _tmp1BF;struct Cyc_Absyn_Exp*_tmp1C0;void**_tmp1C1;struct Cyc_Absyn_Exp*
_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Core_Opt*
_tmp1C7;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_Stmt*_tmp1CA;_LLDB: {struct
Cyc_Absyn_Const_e_struct*_tmp125=(struct Cyc_Absyn_Const_e_struct*)_tmp124;if(
_tmp125->tag != 0)goto _LLDD;else{_tmp126=_tmp125->f1;if((_tmp126.Char_c).tag != 2)
goto _LLDD;_tmp127=(struct _tuple3)(_tmp126.Char_c).val;_tmp128=_tmp127.f1;_tmp129=
_tmp127.f2;}}_LLDC: Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape(_tmp129));Cyc_Absyndump_dump_char((int)'\'');goto _LLDA;
_LLDD: {struct Cyc_Absyn_Const_e_struct*_tmp12A=(struct Cyc_Absyn_Const_e_struct*)
_tmp124;if(_tmp12A->tag != 0)goto _LLDF;else{_tmp12B=_tmp12A->f1;if((_tmp12B.Short_c).tag
!= 3)goto _LLDF;_tmp12C=(struct _tuple4)(_tmp12B.Short_c).val;_tmp12D=_tmp12C.f1;
_tmp12E=_tmp12C.f2;}}_LLDE:{const char*_tmp414;void*_tmp413[1];struct Cyc_Int_pa_struct
_tmp412;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp412.tag=1,((_tmp412.f1=(
unsigned long)((int)_tmp12E),((_tmp413[0]=& _tmp412,Cyc_aprintf(((_tmp414="%d",
_tag_dyneither(_tmp414,sizeof(char),3))),_tag_dyneither(_tmp413,sizeof(void*),1)))))))));}
goto _LLDA;_LLDF: {struct Cyc_Absyn_Const_e_struct*_tmp12F=(struct Cyc_Absyn_Const_e_struct*)
_tmp124;if(_tmp12F->tag != 0)goto _LLE1;else{_tmp130=_tmp12F->f1;if((_tmp130.Int_c).tag
!= 4)goto _LLE1;_tmp131=(struct _tuple5)(_tmp130.Int_c).val;_tmp132=_tmp131.f1;if(
_tmp132 != Cyc_Absyn_None)goto _LLE1;_tmp133=_tmp131.f2;}}_LLE0: _tmp138=_tmp133;
goto _LLE2;_LLE1: {struct Cyc_Absyn_Const_e_struct*_tmp134=(struct Cyc_Absyn_Const_e_struct*)
_tmp124;if(_tmp134->tag != 0)goto _LLE3;else{_tmp135=_tmp134->f1;if((_tmp135.Int_c).tag
!= 4)goto _LLE3;_tmp136=(struct _tuple5)(_tmp135.Int_c).val;_tmp137=_tmp136.f1;if(
_tmp137 != Cyc_Absyn_Signed)goto _LLE3;_tmp138=_tmp136.f2;}}_LLE2:{const char*
_tmp418;void*_tmp417[1];struct Cyc_Int_pa_struct _tmp416;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp416.tag=1,((_tmp416.f1=(unsigned long)_tmp138,((
_tmp417[0]=& _tmp416,Cyc_aprintf(((_tmp418="%d",_tag_dyneither(_tmp418,sizeof(
char),3))),_tag_dyneither(_tmp417,sizeof(void*),1)))))))));}goto _LLDA;_LLE3: {
struct Cyc_Absyn_Const_e_struct*_tmp139=(struct Cyc_Absyn_Const_e_struct*)_tmp124;
if(_tmp139->tag != 0)goto _LLE5;else{_tmp13A=_tmp139->f1;if((_tmp13A.Int_c).tag != 
4)goto _LLE5;_tmp13B=(struct _tuple5)(_tmp13A.Int_c).val;_tmp13C=_tmp13B.f1;if(
_tmp13C != Cyc_Absyn_Unsigned)goto _LLE5;_tmp13D=_tmp13B.f2;}}_LLE4:{const char*
_tmp41C;void*_tmp41B[1];struct Cyc_Int_pa_struct _tmp41A;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp41A.tag=1,((_tmp41A.f1=(unsigned int)_tmp13D,((
_tmp41B[0]=& _tmp41A,Cyc_aprintf(((_tmp41C="%u",_tag_dyneither(_tmp41C,sizeof(
char),3))),_tag_dyneither(_tmp41B,sizeof(void*),1)))))))));}goto _LLDA;_LLE5: {
struct Cyc_Absyn_Const_e_struct*_tmp13E=(struct Cyc_Absyn_Const_e_struct*)_tmp124;
if(_tmp13E->tag != 0)goto _LLE7;else{_tmp13F=_tmp13E->f1;if((_tmp13F.LongLong_c).tag
!= 5)goto _LLE7;_tmp140=(struct _tuple6)(_tmp13F.LongLong_c).val;_tmp141=_tmp140.f1;
_tmp142=_tmp140.f2;}}_LLE6:{const char*_tmp41D;Cyc_Absyndump_dump(((_tmp41D="<<FIX LONG LONG CONSTANT>>",
_tag_dyneither(_tmp41D,sizeof(char),27))));}goto _LLDA;_LLE7: {struct Cyc_Absyn_Const_e_struct*
_tmp143=(struct Cyc_Absyn_Const_e_struct*)_tmp124;if(_tmp143->tag != 0)goto _LLE9;
else{_tmp144=_tmp143->f1;if((_tmp144.Float_c).tag != 6)goto _LLE9;_tmp145=(struct
_dyneither_ptr)(_tmp144.Float_c).val;}}_LLE8: Cyc_Absyndump_dump(_tmp145);goto
_LLDA;_LLE9: {struct Cyc_Absyn_Const_e_struct*_tmp146=(struct Cyc_Absyn_Const_e_struct*)
_tmp124;if(_tmp146->tag != 0)goto _LLEB;else{_tmp147=_tmp146->f1;if((_tmp147.Null_c).tag
!= 1)goto _LLEB;_tmp148=(int)(_tmp147.Null_c).val;}}_LLEA:{const char*_tmp41E;Cyc_Absyndump_dump(((
_tmp41E="NULL",_tag_dyneither(_tmp41E,sizeof(char),5))));}goto _LLDA;_LLEB: {
struct Cyc_Absyn_Const_e_struct*_tmp149=(struct Cyc_Absyn_Const_e_struct*)_tmp124;
if(_tmp149->tag != 0)goto _LLED;else{_tmp14A=_tmp149->f1;if((_tmp14A.String_c).tag
!= 7)goto _LLED;_tmp14B=(struct _dyneither_ptr)(_tmp14A.String_c).val;}}_LLEC: Cyc_Absyndump_dump_char((
int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp14B));Cyc_Absyndump_dump_char((
int)'"');goto _LLDA;_LLED: {struct Cyc_Absyn_UnknownId_e_struct*_tmp14C=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmp124;if(_tmp14C->tag != 2)goto _LLEF;else{_tmp14D=_tmp14C->f1;}}_LLEE: _tmp14F=
_tmp14D;goto _LLF0;_LLEF: {struct Cyc_Absyn_Var_e_struct*_tmp14E=(struct Cyc_Absyn_Var_e_struct*)
_tmp124;if(_tmp14E->tag != 1)goto _LLF1;else{_tmp14F=_tmp14E->f1;}}_LLF0: Cyc_Absyndump_dumpqvar(
_tmp14F);goto _LLDA;_LLF1: {struct Cyc_Absyn_Primop_e_struct*_tmp150=(struct Cyc_Absyn_Primop_e_struct*)
_tmp124;if(_tmp150->tag != 3)goto _LLF3;else{_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;}}
_LLF2: {struct _dyneither_ptr _tmp1D6=Cyc_Absynpp_prim2str(_tmp151);switch(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp152)){case 1: _LL145: if(_tmp151 == (
enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){{const char*_tmp41F;Cyc_Absyndump_dump(((
_tmp41F="numelts(",_tag_dyneither(_tmp41F,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp152))->hd);{const
char*_tmp420;Cyc_Absyndump_dump(((_tmp420=")",_tag_dyneither(_tmp420,sizeof(char),
2))));};}else{Cyc_Absyndump_dump(_tmp1D6);Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp152))->hd);}break;
case 2: _LL146: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp152))->hd);Cyc_Absyndump_dump(_tmp1D6);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp152->tl))->hd);break;default: _LL147: {struct Cyc_Core_Failure_struct
_tmp426;const char*_tmp425;struct Cyc_Core_Failure_struct*_tmp424;(int)_throw((
void*)((_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424[0]=((_tmp426.tag=Cyc_Core_Failure,((
_tmp426.f1=((_tmp425="Absyndump -- Bad number of arguments to primop",
_tag_dyneither(_tmp425,sizeof(char),47))),_tmp426)))),_tmp424)))));}}goto _LLDA;}
_LLF3: {struct Cyc_Absyn_AssignOp_e_struct*_tmp153=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp124;if(_tmp153->tag != 4)goto _LLF5;else{_tmp154=_tmp153->f1;_tmp155=_tmp153->f2;
_tmp156=_tmp153->f3;}}_LLF4: Cyc_Absyndump_dumpexp_prec(myprec,_tmp154);if(
_tmp155 != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)
_tmp155->v));{const char*_tmp427;Cyc_Absyndump_dump_nospace(((_tmp427="=",
_tag_dyneither(_tmp427,sizeof(char),2))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp156);goto _LLDA;_LLF5: {struct Cyc_Absyn_Increment_e_struct*_tmp157=(struct Cyc_Absyn_Increment_e_struct*)
_tmp124;if(_tmp157->tag != 5)goto _LLF7;else{_tmp158=_tmp157->f1;_tmp159=_tmp157->f2;
if(_tmp159 != Cyc_Absyn_PreInc)goto _LLF7;}}_LLF6:{const char*_tmp428;Cyc_Absyndump_dump(((
_tmp428="++",_tag_dyneither(_tmp428,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp158);goto _LLDA;_LLF7: {struct Cyc_Absyn_Increment_e_struct*_tmp15A=(
struct Cyc_Absyn_Increment_e_struct*)_tmp124;if(_tmp15A->tag != 5)goto _LLF9;else{
_tmp15B=_tmp15A->f1;_tmp15C=_tmp15A->f2;if(_tmp15C != Cyc_Absyn_PreDec)goto _LLF9;}}
_LLF8:{const char*_tmp429;Cyc_Absyndump_dump(((_tmp429="--",_tag_dyneither(
_tmp429,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp15B);goto _LLDA;
_LLF9: {struct Cyc_Absyn_Increment_e_struct*_tmp15D=(struct Cyc_Absyn_Increment_e_struct*)
_tmp124;if(_tmp15D->tag != 5)goto _LLFB;else{_tmp15E=_tmp15D->f1;_tmp15F=_tmp15D->f2;
if(_tmp15F != Cyc_Absyn_PostInc)goto _LLFB;}}_LLFA: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp15E);{const char*_tmp42A;Cyc_Absyndump_dump(((_tmp42A="++",
_tag_dyneither(_tmp42A,sizeof(char),3))));}goto _LLDA;_LLFB: {struct Cyc_Absyn_Increment_e_struct*
_tmp160=(struct Cyc_Absyn_Increment_e_struct*)_tmp124;if(_tmp160->tag != 5)goto
_LLFD;else{_tmp161=_tmp160->f1;_tmp162=_tmp160->f2;if(_tmp162 != Cyc_Absyn_PostDec)
goto _LLFD;}}_LLFC: Cyc_Absyndump_dumpexp_prec(myprec,_tmp161);{const char*_tmp42B;
Cyc_Absyndump_dump(((_tmp42B="--",_tag_dyneither(_tmp42B,sizeof(char),3))));}
goto _LLDA;_LLFD: {struct Cyc_Absyn_Conditional_e_struct*_tmp163=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp124;if(_tmp163->tag != 6)goto _LLFF;else{_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;
_tmp166=_tmp163->f3;}}_LLFE: Cyc_Absyndump_dumpexp_prec(myprec,_tmp164);Cyc_Absyndump_dump_char((
int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp165);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp166);goto _LLDA;_LLFF: {struct Cyc_Absyn_And_e_struct*
_tmp167=(struct Cyc_Absyn_And_e_struct*)_tmp124;if(_tmp167->tag != 7)goto _LL101;
else{_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;}}_LL100: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp168);{const char*_tmp42C;Cyc_Absyndump_dump(((_tmp42C=" && ",
_tag_dyneither(_tmp42C,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,
_tmp169);goto _LLDA;_LL101: {struct Cyc_Absyn_Or_e_struct*_tmp16A=(struct Cyc_Absyn_Or_e_struct*)
_tmp124;if(_tmp16A->tag != 8)goto _LL103;else{_tmp16B=_tmp16A->f1;_tmp16C=_tmp16A->f2;}}
_LL102: Cyc_Absyndump_dumpexp_prec(myprec,_tmp16B);{const char*_tmp42D;Cyc_Absyndump_dump(((
_tmp42D=" || ",_tag_dyneither(_tmp42D,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp16C);goto _LLDA;_LL103: {struct Cyc_Absyn_SeqExp_e_struct*_tmp16D=(
struct Cyc_Absyn_SeqExp_e_struct*)_tmp124;if(_tmp16D->tag != 9)goto _LL105;else{
_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;}}_LL104: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp16E);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp16F);Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL105: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp170=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp124;if(_tmp170->tag != 10)goto
_LL107;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;}}_LL106: _tmp174=_tmp171;
_tmp175=_tmp172;goto _LL108;_LL107: {struct Cyc_Absyn_FnCall_e_struct*_tmp173=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp124;if(_tmp173->tag != 11)goto _LL109;else{
_tmp174=_tmp173->f1;_tmp175=_tmp173->f2;}}_LL108: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp174);{const char*_tmp42E;Cyc_Absyndump_dump_nospace(((_tmp42E="(",
_tag_dyneither(_tmp42E,sizeof(char),2))));}Cyc_Absyndump_dumpexps_prec(20,
_tmp175);{const char*_tmp42F;Cyc_Absyndump_dump_nospace(((_tmp42F=")",
_tag_dyneither(_tmp42F,sizeof(char),2))));}goto _LLDA;_LL109: {struct Cyc_Absyn_Throw_e_struct*
_tmp176=(struct Cyc_Absyn_Throw_e_struct*)_tmp124;if(_tmp176->tag != 12)goto _LL10B;
else{_tmp177=_tmp176->f1;}}_LL10A:{const char*_tmp430;Cyc_Absyndump_dump(((
_tmp430="throw",_tag_dyneither(_tmp430,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp177);goto _LLDA;_LL10B: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp178=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp124;if(_tmp178->tag != 13)
goto _LL10D;else{_tmp179=_tmp178->f1;}}_LL10C: _tmp17B=_tmp179;goto _LL10E;_LL10D: {
struct Cyc_Absyn_Instantiate_e_struct*_tmp17A=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp124;if(_tmp17A->tag != 14)goto _LL10F;else{_tmp17B=_tmp17A->f1;}}_LL10E: Cyc_Absyndump_dumpexp_prec(
inprec,_tmp17B);goto _LLDA;_LL10F: {struct Cyc_Absyn_Cast_e_struct*_tmp17C=(struct
Cyc_Absyn_Cast_e_struct*)_tmp124;if(_tmp17C->tag != 15)goto _LL111;else{_tmp17D=(
void*)_tmp17C->f1;_tmp17E=_tmp17C->f2;}}_LL110: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp(_tmp17D);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp17E);goto _LLDA;_LL111: {struct Cyc_Absyn_Address_e_struct*_tmp17F=(
struct Cyc_Absyn_Address_e_struct*)_tmp124;if(_tmp17F->tag != 16)goto _LL113;else{
_tmp180=_tmp17F->f1;}}_LL112: Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp180);goto _LLDA;_LL113: {struct Cyc_Absyn_New_e_struct*_tmp181=(struct
Cyc_Absyn_New_e_struct*)_tmp124;if(_tmp181->tag != 17)goto _LL115;else{_tmp182=
_tmp181->f1;_tmp183=_tmp181->f2;}}_LL114:{const char*_tmp431;Cyc_Absyndump_dump(((
_tmp431="new ",_tag_dyneither(_tmp431,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp183);goto _LLDA;_LL115: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp184=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp124;if(_tmp184->tag != 18)goto _LL117;
else{_tmp185=(void*)_tmp184->f1;}}_LL116:{const char*_tmp432;Cyc_Absyndump_dump(((
_tmp432="sizeof(",_tag_dyneither(_tmp432,sizeof(char),8))));}Cyc_Absyndump_dumptyp(
_tmp185);Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL117: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp186=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp124;if(_tmp186->tag != 19)goto
_LL119;else{_tmp187=_tmp186->f1;}}_LL118:{const char*_tmp433;Cyc_Absyndump_dump(((
_tmp433="sizeof(",_tag_dyneither(_tmp433,sizeof(char),8))));}Cyc_Absyndump_dumpexp(
_tmp187);Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL119: {struct Cyc_Absyn_Valueof_e_struct*
_tmp188=(struct Cyc_Absyn_Valueof_e_struct*)_tmp124;if(_tmp188->tag != 39)goto
_LL11B;else{_tmp189=(void*)_tmp188->f1;}}_LL11A:{const char*_tmp434;Cyc_Absyndump_dump(((
_tmp434="valueof(",_tag_dyneither(_tmp434,sizeof(char),9))));}Cyc_Absyndump_dumptyp(
_tmp189);Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL11B: {struct Cyc_Absyn_Asm_e_struct*
_tmp18A=(struct Cyc_Absyn_Asm_e_struct*)_tmp124;if(_tmp18A->tag != 40)goto _LL11D;
else{_tmp18B=_tmp18A->f1;_tmp18C=_tmp18A->f2;}}_LL11C:{const char*_tmp435;Cyc_Absyndump_dump(((
_tmp435="__asm__",_tag_dyneither(_tmp435,sizeof(char),8))));}if(_tmp18B){const
char*_tmp436;Cyc_Absyndump_dump(((_tmp436=" volatile ",_tag_dyneither(_tmp436,
sizeof(char),11))));}Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dump_nospace(
_tmp18C);Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL11D: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmp18D=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmp124;if(_tmp18D->tag != 38)goto
_LL11F;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;}}_LL11E:{const char*_tmp437;
Cyc_Absyndump_dump(((_tmp437="tagcheck(",_tag_dyneither(_tmp437,sizeof(char),10))));}
Cyc_Absyndump_dumpexp(_tmp18E);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp18F);Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL11F: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp190=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp124;if(_tmp190->tag != 20)goto
_LL121;else{_tmp191=(void*)_tmp190->f1;_tmp192=(void*)_tmp190->f2;{struct Cyc_Absyn_StructField_struct*
_tmp193=(struct Cyc_Absyn_StructField_struct*)_tmp192;if(_tmp193->tag != 0)goto
_LL121;else{_tmp194=_tmp193->f1;}};}}_LL120:{const char*_tmp438;Cyc_Absyndump_dump(((
_tmp438="offsetof(",_tag_dyneither(_tmp438,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp191);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp194);
Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL121: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp195=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp124;if(_tmp195->tag != 20)goto
_LL123;else{_tmp196=(void*)_tmp195->f1;_tmp197=(void*)_tmp195->f2;{struct Cyc_Absyn_TupleIndex_struct*
_tmp198=(struct Cyc_Absyn_TupleIndex_struct*)_tmp197;if(_tmp198->tag != 1)goto
_LL123;else{_tmp199=_tmp198->f1;}};}}_LL122:{const char*_tmp439;Cyc_Absyndump_dump(((
_tmp439="offsetof(",_tag_dyneither(_tmp439,sizeof(char),10))));}Cyc_Absyndump_dumptyp(
_tmp196);Cyc_Absyndump_dump_char((int)',');{const char*_tmp43D;void*_tmp43C[1];
struct Cyc_Int_pa_struct _tmp43B;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp43B.tag=1,((_tmp43B.f1=(unsigned long)((int)_tmp199),((_tmp43C[0]=& _tmp43B,
Cyc_aprintf(((_tmp43D="%d",_tag_dyneither(_tmp43D,sizeof(char),3))),
_tag_dyneither(_tmp43C,sizeof(void*),1)))))))));}Cyc_Absyndump_dump_char((int)')');
goto _LLDA;_LL123: {struct Cyc_Absyn_Deref_e_struct*_tmp19A=(struct Cyc_Absyn_Deref_e_struct*)
_tmp124;if(_tmp19A->tag != 21)goto _LL125;else{_tmp19B=_tmp19A->f1;}}_LL124: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp19B);goto _LLDA;_LL125: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp19C=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp124;if(_tmp19C->tag != 22)goto
_LL127;else{_tmp19D=_tmp19C->f1;_tmp19E=_tmp19C->f2;}}_LL126: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp19D);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp19E);goto _LLDA;_LL127: {struct Cyc_Absyn_AggrArrow_e_struct*_tmp19F=(struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp124;if(_tmp19F->tag != 23)goto _LL129;else{
_tmp1A0=_tmp19F->f1;_tmp1A1=_tmp19F->f2;}}_LL128: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1A0);{const char*_tmp43E;Cyc_Absyndump_dump_nospace(((_tmp43E="->",
_tag_dyneither(_tmp43E,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp1A1);
goto _LLDA;_LL129: {struct Cyc_Absyn_Subscript_e_struct*_tmp1A2=(struct Cyc_Absyn_Subscript_e_struct*)
_tmp124;if(_tmp1A2->tag != 24)goto _LL12B;else{_tmp1A3=_tmp1A2->f1;_tmp1A4=_tmp1A2->f2;}}
_LL12A: Cyc_Absyndump_dumpexp_prec(myprec,_tmp1A3);Cyc_Absyndump_dump_char((int)'[');
Cyc_Absyndump_dumpexp(_tmp1A4);Cyc_Absyndump_dump_char((int)']');goto _LLDA;
_LL12B: {struct Cyc_Absyn_Tuple_e_struct*_tmp1A5=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp124;if(_tmp1A5->tag != 25)goto _LL12D;else{_tmp1A6=_tmp1A5->f1;}}_LL12C:{const
char*_tmp43F;Cyc_Absyndump_dump(((_tmp43F="$(",_tag_dyneither(_tmp43F,sizeof(
char),3))));}Cyc_Absyndump_dumpexps_prec(20,_tmp1A6);Cyc_Absyndump_dump_char((
int)')');goto _LLDA;_LL12D: {struct Cyc_Absyn_CompoundLit_e_struct*_tmp1A7=(struct
Cyc_Absyn_CompoundLit_e_struct*)_tmp124;if(_tmp1A7->tag != 26)goto _LL12F;else{
_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_LL12E: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp1A8).f3);Cyc_Absyndump_dump_char((int)')');{const
char*_tmp442;const char*_tmp441;const char*_tmp440;((void(*)(void(*f)(struct
_tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr
end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1A9,((
_tmp440="{",_tag_dyneither(_tmp440,sizeof(char),2))),((_tmp441="}",
_tag_dyneither(_tmp441,sizeof(char),2))),((_tmp442=",",_tag_dyneither(_tmp442,
sizeof(char),2))));}goto _LLDA;_LL12F: {struct Cyc_Absyn_Array_e_struct*_tmp1AA=(
struct Cyc_Absyn_Array_e_struct*)_tmp124;if(_tmp1AA->tag != 27)goto _LL131;else{
_tmp1AB=_tmp1AA->f1;}}_LL130:{const char*_tmp445;const char*_tmp444;const char*
_tmp443;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1AB,((_tmp443="{",_tag_dyneither(_tmp443,sizeof(char),2))),((
_tmp444="}",_tag_dyneither(_tmp444,sizeof(char),2))),((_tmp445=",",
_tag_dyneither(_tmp445,sizeof(char),2))));}goto _LLDA;_LL131: {struct Cyc_Absyn_Comprehension_e_struct*
_tmp1AC=(struct Cyc_Absyn_Comprehension_e_struct*)_tmp124;if(_tmp1AC->tag != 28)
goto _LL133;else{_tmp1AD=_tmp1AC->f1;_tmp1AE=_tmp1AC->f2;_tmp1AF=_tmp1AC->f3;}}
_LL132:{const char*_tmp446;Cyc_Absyndump_dump(((_tmp446="new {for",_tag_dyneither(
_tmp446,sizeof(char),9))));}Cyc_Absyndump_dump_str((*_tmp1AD->name).f2);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumpexp(_tmp1AE);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(
_tmp1AF);Cyc_Absyndump_dump_char((int)'}');goto _LLDA;_LL133: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp1B0=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp124;if(_tmp1B0->tag != 29)goto
_LL135;else{_tmp1B1=_tmp1B0->f1;_tmp1B2=_tmp1B0->f2;_tmp1B3=_tmp1B0->f3;}}_LL134:
Cyc_Absyndump_dumpqvar(_tmp1B1);Cyc_Absyndump_dump_char((int)'{');if(_tmp1B2 != 0)
Cyc_Absyndump_dumptps(_tmp1B2);{const char*_tmp449;const char*_tmp448;const char*
_tmp447;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1B3,((_tmp447="",_tag_dyneither(_tmp447,sizeof(char),1))),((
_tmp448="}",_tag_dyneither(_tmp448,sizeof(char),2))),((_tmp449=",",
_tag_dyneither(_tmp449,sizeof(char),2))));}goto _LLDA;_LL135: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp1B4=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp124;if(_tmp1B4->tag != 30)goto
_LL137;else{_tmp1B5=_tmp1B4->f2;}}_LL136:{const char*_tmp44C;const char*_tmp44B;
const char*_tmp44A;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,
struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp1B5,((_tmp44A="{",_tag_dyneither(_tmp44A,sizeof(char),2))),((
_tmp44B="}",_tag_dyneither(_tmp44B,sizeof(char),2))),((_tmp44C=",",
_tag_dyneither(_tmp44C,sizeof(char),2))));}goto _LLDA;_LL137: {struct Cyc_Absyn_Datatype_e_struct*
_tmp1B6=(struct Cyc_Absyn_Datatype_e_struct*)_tmp124;if(_tmp1B6->tag != 31)goto
_LL139;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f3;}}_LL138: Cyc_Absyndump_dumpqvar(
_tmp1B8->name);if(_tmp1B7 != 0){const char*_tmp44F;const char*_tmp44E;const char*
_tmp44D;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmp1B7,((_tmp44D="(",_tag_dyneither(_tmp44D,sizeof(char),
2))),((_tmp44E=")",_tag_dyneither(_tmp44E,sizeof(char),2))),((_tmp44F=",",
_tag_dyneither(_tmp44F,sizeof(char),2))));}goto _LLDA;_LL139: {struct Cyc_Absyn_Enum_e_struct*
_tmp1B9=(struct Cyc_Absyn_Enum_e_struct*)_tmp124;if(_tmp1B9->tag != 32)goto _LL13B;
else{_tmp1BA=_tmp1B9->f1;}}_LL13A: Cyc_Absyndump_dumpqvar(_tmp1BA);goto _LLDA;
_LL13B: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp1BB=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp124;if(_tmp1BB->tag != 33)goto _LL13D;else{_tmp1BC=_tmp1BB->f1;}}_LL13C: Cyc_Absyndump_dumpqvar(
_tmp1BC);goto _LLDA;_LL13D: {struct Cyc_Absyn_Malloc_e_struct*_tmp1BD=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp124;if(_tmp1BD->tag != 34)goto _LL13F;else{_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BE.is_calloc;
_tmp1C0=_tmp1BE.rgn;_tmp1C1=_tmp1BE.elt_type;_tmp1C2=_tmp1BE.num_elts;}}_LL13E:
if(_tmp1BF){if(_tmp1C0 != 0){{const char*_tmp450;Cyc_Absyndump_dump(((_tmp450="rcalloc(",
_tag_dyneither(_tmp450,sizeof(char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp1C0);{const char*_tmp451;Cyc_Absyndump_dump(((_tmp451=",",_tag_dyneither(
_tmp451,sizeof(char),2))));};}else{const char*_tmp452;Cyc_Absyndump_dump(((
_tmp452="calloc",_tag_dyneither(_tmp452,sizeof(char),7))));}Cyc_Absyndump_dumpexp(
_tmp1C2);{const char*_tmp453;Cyc_Absyndump_dump(((_tmp453=",",_tag_dyneither(
_tmp453,sizeof(char),2))));}Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp1C1)),0));{const char*_tmp454;Cyc_Absyndump_dump(((_tmp454=")",
_tag_dyneither(_tmp454,sizeof(char),2))));};}else{if(_tmp1C0 != 0){{const char*
_tmp455;Cyc_Absyndump_dump(((_tmp455="rmalloc(",_tag_dyneither(_tmp455,sizeof(
char),9))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp1C0);{const char*
_tmp456;Cyc_Absyndump_dump(((_tmp456=",",_tag_dyneither(_tmp456,sizeof(char),2))));};}
else{const char*_tmp457;Cyc_Absyndump_dump(((_tmp457="malloc(",_tag_dyneither(
_tmp457,sizeof(char),8))));}if(_tmp1C1 != 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp1C1,0),_tmp1C2,0));else{Cyc_Absyndump_dumpexp(
_tmp1C2);}{const char*_tmp458;Cyc_Absyndump_dump(((_tmp458=")",_tag_dyneither(
_tmp458,sizeof(char),2))));};}goto _LLDA;_LL13F: {struct Cyc_Absyn_Swap_e_struct*
_tmp1C3=(struct Cyc_Absyn_Swap_e_struct*)_tmp124;if(_tmp1C3->tag != 35)goto _LL141;
else{_tmp1C4=_tmp1C3->f1;_tmp1C5=_tmp1C3->f2;}}_LL140:{const char*_tmp459;Cyc_Absyndump_dump(((
_tmp459="cycswap(",_tag_dyneither(_tmp459,sizeof(char),9))));}Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1C4);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp1C5);Cyc_Absyndump_dump_char((int)')');goto _LLDA;_LL141: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp1C6=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp124;if(_tmp1C6->tag != 36)
goto _LL143;else{_tmp1C7=_tmp1C6->f1;_tmp1C8=_tmp1C6->f2;}}_LL142:{const char*
_tmp45C;const char*_tmp45B;const char*_tmp45A;((void(*)(void(*f)(struct _tuple16*),
struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp1C8,((_tmp45A="{",
_tag_dyneither(_tmp45A,sizeof(char),2))),((_tmp45B="}",_tag_dyneither(_tmp45B,
sizeof(char),2))),((_tmp45C=",",_tag_dyneither(_tmp45C,sizeof(char),2))));}goto
_LLDA;_LL143: {struct Cyc_Absyn_StmtExp_e_struct*_tmp1C9=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmp124;if(_tmp1C9->tag != 37)goto _LLDA;else{_tmp1CA=_tmp1C9->f1;}}_LL144:{const
char*_tmp45D;Cyc_Absyndump_dump_nospace(((_tmp45D="({",_tag_dyneither(_tmp45D,
sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp1CA,1);{const char*_tmp45E;Cyc_Absyndump_dump_nospace(((
_tmp45E="})",_tag_dyneither(_tmp45E,sizeof(char),3))));}goto _LLDA;_LLDA:;}if(
inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp213=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp213->where_clause == 0  && (
_tmp213->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){const char*_tmp45F;Cyc_Absyndump_dump(((
_tmp45F="default:",_tag_dyneither(_tmp45F,sizeof(char),9))));}else{{const char*
_tmp460;Cyc_Absyndump_dump(((_tmp460="case",_tag_dyneither(_tmp460,sizeof(char),
5))));}Cyc_Absyndump_dumppat(_tmp213->pattern);if(_tmp213->where_clause != 0){{
const char*_tmp461;Cyc_Absyndump_dump(((_tmp461="&&",_tag_dyneither(_tmp461,
sizeof(char),3))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp213->where_clause));}{const char*_tmp462;Cyc_Absyndump_dump_nospace(((_tmp462=":",
_tag_dyneither(_tmp462,sizeof(char),2))));};}Cyc_Absyndump_dumpstmt(_tmp213->body,
0);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){void*_tmp218=
s->r;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_Absyn_Stmt*_tmp21D;struct Cyc_Absyn_Stmt*
_tmp21E;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Exp*_tmp222;struct Cyc_Absyn_Exp*
_tmp224;struct Cyc_Absyn_Stmt*_tmp225;struct Cyc_Absyn_Stmt*_tmp226;struct _tuple8
_tmp228;struct Cyc_Absyn_Exp*_tmp229;struct Cyc_Absyn_Stmt*_tmp22A;struct
_dyneither_ptr*_tmp22E;struct Cyc_Absyn_Exp*_tmp230;struct _tuple8 _tmp231;struct
Cyc_Absyn_Exp*_tmp232;struct _tuple8 _tmp233;struct Cyc_Absyn_Exp*_tmp234;struct Cyc_Absyn_Stmt*
_tmp235;struct Cyc_Absyn_Exp*_tmp237;struct Cyc_List_List*_tmp238;struct Cyc_Absyn_Decl*
_tmp23A;struct Cyc_Absyn_Stmt*_tmp23B;struct _dyneither_ptr*_tmp23D;struct Cyc_Absyn_Stmt*
_tmp23E;struct Cyc_Absyn_Stmt*_tmp240;struct _tuple8 _tmp241;struct Cyc_Absyn_Exp*
_tmp242;struct Cyc_List_List*_tmp244;struct Cyc_List_List*_tmp246;struct Cyc_Absyn_Stmt*
_tmp248;struct Cyc_List_List*_tmp249;struct Cyc_Absyn_Exp*_tmp24B;_LL14A: {struct
Cyc_Absyn_Skip_s_struct*_tmp219=(struct Cyc_Absyn_Skip_s_struct*)_tmp218;if(
_tmp219->tag != 0)goto _LL14C;}_LL14B: Cyc_Absyndump_dump_semi();goto _LL149;_LL14C: {
struct Cyc_Absyn_Exp_s_struct*_tmp21A=(struct Cyc_Absyn_Exp_s_struct*)_tmp218;if(
_tmp21A->tag != 1)goto _LL14E;else{_tmp21B=_tmp21A->f1;}}_LL14D: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dumpexp(_tmp21B);Cyc_Absyndump_dump_semi();goto _LL149;
_LL14E: {struct Cyc_Absyn_Seq_s_struct*_tmp21C=(struct Cyc_Absyn_Seq_s_struct*)
_tmp218;if(_tmp21C->tag != 2)goto _LL150;else{_tmp21D=_tmp21C->f1;_tmp21E=_tmp21C->f2;}}
_LL14F: if(Cyc_Absynpp_is_declaration(_tmp21D)){Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp21D,0);Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dumpstmt(
_tmp21D,0);}if(Cyc_Absynpp_is_declaration(_tmp21E)){if(expstmt)Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp21E,
expstmt);Cyc_Absyndump_dump_char((int)'}');if(expstmt)Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dump_semi();}else{Cyc_Absyndump_dumpstmt(_tmp21E,expstmt);}
goto _LL149;_LL150: {struct Cyc_Absyn_Return_s_struct*_tmp21F=(struct Cyc_Absyn_Return_s_struct*)
_tmp218;if(_tmp21F->tag != 3)goto _LL152;else{_tmp220=_tmp21F->f1;if(_tmp220 != 0)
goto _LL152;}}_LL151: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp463;Cyc_Absyndump_dump(((
_tmp463="return;",_tag_dyneither(_tmp463,sizeof(char),8))));}goto _LL149;_LL152: {
struct Cyc_Absyn_Return_s_struct*_tmp221=(struct Cyc_Absyn_Return_s_struct*)
_tmp218;if(_tmp221->tag != 3)goto _LL154;else{_tmp222=_tmp221->f1;}}_LL153: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp464;Cyc_Absyndump_dump(((_tmp464="return",_tag_dyneither(
_tmp464,sizeof(char),7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp222));Cyc_Absyndump_dump_semi();goto _LL149;_LL154: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp223=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp218;if(_tmp223->tag != 4)goto
_LL156;else{_tmp224=_tmp223->f1;_tmp225=_tmp223->f2;_tmp226=_tmp223->f3;}}_LL155:
Cyc_Absyndump_dumploc(s->loc);{const char*_tmp465;Cyc_Absyndump_dump(((_tmp465="if(",
_tag_dyneither(_tmp465,sizeof(char),4))));}Cyc_Absyndump_dumpexp(_tmp224);{void*
_tmp24F=_tmp225->r;_LL171: {struct Cyc_Absyn_Seq_s_struct*_tmp250=(struct Cyc_Absyn_Seq_s_struct*)
_tmp24F;if(_tmp250->tag != 2)goto _LL173;}_LL172: goto _LL174;_LL173: {struct Cyc_Absyn_Decl_s_struct*
_tmp251=(struct Cyc_Absyn_Decl_s_struct*)_tmp24F;if(_tmp251->tag != 12)goto _LL175;}
_LL174: goto _LL176;_LL175: {struct Cyc_Absyn_IfThenElse_s_struct*_tmp252=(struct
Cyc_Absyn_IfThenElse_s_struct*)_tmp24F;if(_tmp252->tag != 4)goto _LL177;}_LL176:
goto _LL178;_LL177: {struct Cyc_Absyn_Label_s_struct*_tmp253=(struct Cyc_Absyn_Label_s_struct*)
_tmp24F;if(_tmp253->tag != 13)goto _LL179;}_LL178:{const char*_tmp466;Cyc_Absyndump_dump_nospace(((
_tmp466="){",_tag_dyneither(_tmp466,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(
_tmp225,0);Cyc_Absyndump_dump_char((int)'}');goto _LL170;_LL179:;_LL17A: Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dumpstmt(_tmp225,0);_LL170:;}{void*_tmp255=_tmp226->r;
_LL17C: {struct Cyc_Absyn_Skip_s_struct*_tmp256=(struct Cyc_Absyn_Skip_s_struct*)
_tmp255;if(_tmp256->tag != 0)goto _LL17E;}_LL17D: goto _LL17B;_LL17E:;_LL17F:{const
char*_tmp467;Cyc_Absyndump_dump(((_tmp467="else{",_tag_dyneither(_tmp467,sizeof(
char),6))));}Cyc_Absyndump_dumpstmt(_tmp226,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL17B;_LL17B:;}goto _LL149;_LL156: {struct Cyc_Absyn_While_s_struct*_tmp227=(
struct Cyc_Absyn_While_s_struct*)_tmp218;if(_tmp227->tag != 5)goto _LL158;else{
_tmp228=_tmp227->f1;_tmp229=_tmp228.f1;_tmp22A=_tmp227->f2;}}_LL157: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp468;Cyc_Absyndump_dump(((_tmp468="while(",_tag_dyneither(
_tmp468,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp229);{const char*_tmp469;
Cyc_Absyndump_dump_nospace(((_tmp469="){",_tag_dyneither(_tmp469,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp22A,0);Cyc_Absyndump_dump_char((int)'}');goto _LL149;
_LL158: {struct Cyc_Absyn_Break_s_struct*_tmp22B=(struct Cyc_Absyn_Break_s_struct*)
_tmp218;if(_tmp22B->tag != 6)goto _LL15A;}_LL159: Cyc_Absyndump_dumploc(s->loc);{
const char*_tmp46A;Cyc_Absyndump_dump(((_tmp46A="break;",_tag_dyneither(_tmp46A,
sizeof(char),7))));}goto _LL149;_LL15A: {struct Cyc_Absyn_Continue_s_struct*
_tmp22C=(struct Cyc_Absyn_Continue_s_struct*)_tmp218;if(_tmp22C->tag != 7)goto
_LL15C;}_LL15B: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp46B;Cyc_Absyndump_dump(((
_tmp46B="continue;",_tag_dyneither(_tmp46B,sizeof(char),10))));}goto _LL149;
_LL15C: {struct Cyc_Absyn_Goto_s_struct*_tmp22D=(struct Cyc_Absyn_Goto_s_struct*)
_tmp218;if(_tmp22D->tag != 8)goto _LL15E;else{_tmp22E=_tmp22D->f1;}}_LL15D: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp46C;Cyc_Absyndump_dump(((_tmp46C="goto",_tag_dyneither(
_tmp46C,sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp22E);Cyc_Absyndump_dump_semi();
goto _LL149;_LL15E: {struct Cyc_Absyn_For_s_struct*_tmp22F=(struct Cyc_Absyn_For_s_struct*)
_tmp218;if(_tmp22F->tag != 9)goto _LL160;else{_tmp230=_tmp22F->f1;_tmp231=_tmp22F->f2;
_tmp232=_tmp231.f1;_tmp233=_tmp22F->f3;_tmp234=_tmp233.f1;_tmp235=_tmp22F->f4;}}
_LL15F: Cyc_Absyndump_dumploc(s->loc);{const char*_tmp46D;Cyc_Absyndump_dump(((
_tmp46D="for(",_tag_dyneither(_tmp46D,sizeof(char),5))));}Cyc_Absyndump_dumpexp(
_tmp230);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp232);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp234);{const char*_tmp46E;Cyc_Absyndump_dump_nospace(((
_tmp46E="){",_tag_dyneither(_tmp46E,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(
_tmp235,0);Cyc_Absyndump_dump_char((int)'}');goto _LL149;_LL160: {struct Cyc_Absyn_Switch_s_struct*
_tmp236=(struct Cyc_Absyn_Switch_s_struct*)_tmp218;if(_tmp236->tag != 10)goto
_LL162;else{_tmp237=_tmp236->f1;_tmp238=_tmp236->f2;}}_LL161: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp46F;Cyc_Absyndump_dump(((_tmp46F="switch(",_tag_dyneither(
_tmp46F,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp237);{const char*_tmp470;
Cyc_Absyndump_dump_nospace(((_tmp470="){",_tag_dyneither(_tmp470,sizeof(char),3))));}
Cyc_Absyndump_dumpswitchclauses(_tmp238);Cyc_Absyndump_dump_char((int)'}');goto
_LL149;_LL162: {struct Cyc_Absyn_Decl_s_struct*_tmp239=(struct Cyc_Absyn_Decl_s_struct*)
_tmp218;if(_tmp239->tag != 12)goto _LL164;else{_tmp23A=_tmp239->f1;_tmp23B=_tmp239->f2;}}
_LL163: Cyc_Absyndump_dumpdecl(_tmp23A);Cyc_Absyndump_dumpstmt(_tmp23B,expstmt);
goto _LL149;_LL164: {struct Cyc_Absyn_Label_s_struct*_tmp23C=(struct Cyc_Absyn_Label_s_struct*)
_tmp218;if(_tmp23C->tag != 13)goto _LL166;else{_tmp23D=_tmp23C->f1;_tmp23E=_tmp23C->f2;}}
_LL165: if(Cyc_Absynpp_is_declaration(_tmp23E)){Cyc_Absyndump_dump_str(_tmp23D);
if(expstmt){const char*_tmp471;Cyc_Absyndump_dump_nospace(((_tmp471=": ({",
_tag_dyneither(_tmp471,sizeof(char),5))));}else{const char*_tmp472;Cyc_Absyndump_dump_nospace(((
_tmp472=": {",_tag_dyneither(_tmp472,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(
_tmp23E,expstmt);if(expstmt){const char*_tmp473;Cyc_Absyndump_dump_nospace(((
_tmp473="});}",_tag_dyneither(_tmp473,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((
int)'}');}}else{Cyc_Absyndump_dump_str(_tmp23D);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp23E,expstmt);}goto _LL149;_LL166: {struct Cyc_Absyn_Do_s_struct*
_tmp23F=(struct Cyc_Absyn_Do_s_struct*)_tmp218;if(_tmp23F->tag != 14)goto _LL168;
else{_tmp240=_tmp23F->f1;_tmp241=_tmp23F->f2;_tmp242=_tmp241.f1;}}_LL167: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp474;Cyc_Absyndump_dump(((_tmp474="do{",_tag_dyneither(
_tmp474,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp240,0);{const char*_tmp475;
Cyc_Absyndump_dump_nospace(((_tmp475="}while(",_tag_dyneither(_tmp475,sizeof(
char),8))));}Cyc_Absyndump_dumpexp(_tmp242);{const char*_tmp476;Cyc_Absyndump_dump_nospace(((
_tmp476=");",_tag_dyneither(_tmp476,sizeof(char),3))));}goto _LL149;_LL168: {
struct Cyc_Absyn_Fallthru_s_struct*_tmp243=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp218;if(_tmp243->tag != 11)goto _LL16A;else{_tmp244=_tmp243->f1;if(_tmp244 != 0)
goto _LL16A;}}_LL169:{const char*_tmp477;Cyc_Absyndump_dump(((_tmp477="fallthru;",
_tag_dyneither(_tmp477,sizeof(char),10))));}goto _LL149;_LL16A: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp245=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp218;if(_tmp245->tag != 11)goto
_LL16C;else{_tmp246=_tmp245->f1;}}_LL16B:{const char*_tmp478;Cyc_Absyndump_dump(((
_tmp478="fallthru(",_tag_dyneither(_tmp478,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(
20,_tmp246);{const char*_tmp479;Cyc_Absyndump_dump_nospace(((_tmp479=");",
_tag_dyneither(_tmp479,sizeof(char),3))));}goto _LL149;_LL16C: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp247=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp218;if(_tmp247->tag != 15)goto
_LL16E;else{_tmp248=_tmp247->f1;_tmp249=_tmp247->f2;}}_LL16D: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp47A;Cyc_Absyndump_dump(((_tmp47A="try",_tag_dyneither(
_tmp47A,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp248,0);{const char*_tmp47B;
Cyc_Absyndump_dump(((_tmp47B="catch{",_tag_dyneither(_tmp47B,sizeof(char),7))));}
Cyc_Absyndump_dumpswitchclauses(_tmp249);Cyc_Absyndump_dump_char((int)'}');goto
_LL149;_LL16E: {struct Cyc_Absyn_ResetRegion_s_struct*_tmp24A=(struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp218;if(_tmp24A->tag != 16)goto _LL149;else{_tmp24B=_tmp24A->f1;}}_LL16F: Cyc_Absyndump_dumploc(
s->loc);{const char*_tmp47C;Cyc_Absyndump_dump(((_tmp47C="reset_region(",
_tag_dyneither(_tmp47C,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp24B);{
const char*_tmp47D;Cyc_Absyndump_dump(((_tmp47D=");",_tag_dyneither(_tmp47D,
sizeof(char),3))));}goto _LL149;_LL149:;}struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(struct _tuple17*dp){{const char*
_tmp480;const char*_tmp47F;const char*_tmp47E;((void(*)(void(*f)(void*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpdesignator,(*dp).f1,((_tmp47E="",_tag_dyneither(_tmp47E,
sizeof(char),1))),((_tmp47F="=",_tag_dyneither(_tmp47F,sizeof(char),2))),((
_tmp480="=",_tag_dyneither(_tmp480,sizeof(char),2))));}Cyc_Absyndump_dumppat((*
dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*_tmp271=p->r;
enum Cyc_Absyn_Sign _tmp275;int _tmp276;enum Cyc_Absyn_Sign _tmp278;int _tmp279;
enum Cyc_Absyn_Sign _tmp27B;int _tmp27C;char _tmp27E;struct _dyneither_ptr _tmp280;
struct Cyc_Absyn_Vardecl*_tmp282;struct Cyc_Absyn_Pat*_tmp283;struct Cyc_Absyn_Pat
_tmp284;void*_tmp285;struct Cyc_Absyn_Vardecl*_tmp288;struct Cyc_Absyn_Pat*_tmp289;
struct Cyc_List_List*_tmp28B;int _tmp28C;struct Cyc_Absyn_Pat*_tmp28E;struct Cyc_Absyn_Vardecl*
_tmp290;struct Cyc_Absyn_Pat*_tmp291;struct Cyc_Absyn_Pat _tmp292;void*_tmp293;
struct Cyc_Absyn_Vardecl*_tmp296;struct Cyc_Absyn_Pat*_tmp297;struct Cyc_Absyn_Tvar*
_tmp299;struct Cyc_Absyn_Vardecl*_tmp29A;struct _tuple0*_tmp29C;struct _tuple0*
_tmp29E;struct Cyc_List_List*_tmp29F;int _tmp2A0;struct Cyc_Absyn_AggrInfo*_tmp2A2;
struct Cyc_Absyn_AggrInfo _tmp2A3;union Cyc_Absyn_AggrInfoU _tmp2A4;struct Cyc_List_List*
_tmp2A5;struct Cyc_List_List*_tmp2A6;int _tmp2A7;struct Cyc_Absyn_AggrInfo*_tmp2A9;
struct Cyc_List_List*_tmp2AA;struct Cyc_List_List*_tmp2AB;int _tmp2AC;struct Cyc_Absyn_Datatypefield*
_tmp2AE;struct Cyc_List_List*_tmp2AF;int _tmp2B0;struct Cyc_Absyn_Enumfield*_tmp2B2;
struct Cyc_Absyn_Enumfield*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B6;_LL181: {struct Cyc_Absyn_Wild_p_struct*
_tmp272=(struct Cyc_Absyn_Wild_p_struct*)_tmp271;if(_tmp272->tag != 0)goto _LL183;}
_LL182: Cyc_Absyndump_dump_char((int)'_');goto _LL180;_LL183: {struct Cyc_Absyn_Null_p_struct*
_tmp273=(struct Cyc_Absyn_Null_p_struct*)_tmp271;if(_tmp273->tag != 8)goto _LL185;}
_LL184:{const char*_tmp481;Cyc_Absyndump_dump(((_tmp481="NULL",_tag_dyneither(
_tmp481,sizeof(char),5))));}goto _LL180;_LL185: {struct Cyc_Absyn_Int_p_struct*
_tmp274=(struct Cyc_Absyn_Int_p_struct*)_tmp271;if(_tmp274->tag != 9)goto _LL187;
else{_tmp275=_tmp274->f1;if(_tmp275 != Cyc_Absyn_None)goto _LL187;_tmp276=_tmp274->f2;}}
_LL186: _tmp279=_tmp276;goto _LL188;_LL187: {struct Cyc_Absyn_Int_p_struct*_tmp277=(
struct Cyc_Absyn_Int_p_struct*)_tmp271;if(_tmp277->tag != 9)goto _LL189;else{
_tmp278=_tmp277->f1;if(_tmp278 != Cyc_Absyn_Signed)goto _LL189;_tmp279=_tmp277->f2;}}
_LL188:{const char*_tmp485;void*_tmp484[1];struct Cyc_Int_pa_struct _tmp483;Cyc_Absyndump_dump((
struct _dyneither_ptr)((_tmp483.tag=1,((_tmp483.f1=(unsigned long)_tmp279,((
_tmp484[0]=& _tmp483,Cyc_aprintf(((_tmp485="%d",_tag_dyneither(_tmp485,sizeof(
char),3))),_tag_dyneither(_tmp484,sizeof(void*),1)))))))));}goto _LL180;_LL189: {
struct Cyc_Absyn_Int_p_struct*_tmp27A=(struct Cyc_Absyn_Int_p_struct*)_tmp271;if(
_tmp27A->tag != 9)goto _LL18B;else{_tmp27B=_tmp27A->f1;if(_tmp27B != Cyc_Absyn_Unsigned)
goto _LL18B;_tmp27C=_tmp27A->f2;}}_LL18A:{const char*_tmp489;void*_tmp488[1];
struct Cyc_Int_pa_struct _tmp487;Cyc_Absyndump_dump((struct _dyneither_ptr)((
_tmp487.tag=1,((_tmp487.f1=(unsigned int)_tmp27C,((_tmp488[0]=& _tmp487,Cyc_aprintf(((
_tmp489="%u",_tag_dyneither(_tmp489,sizeof(char),3))),_tag_dyneither(_tmp488,
sizeof(void*),1)))))))));}goto _LL180;_LL18B: {struct Cyc_Absyn_Char_p_struct*
_tmp27D=(struct Cyc_Absyn_Char_p_struct*)_tmp271;if(_tmp27D->tag != 10)goto _LL18D;
else{_tmp27E=_tmp27D->f1;}}_LL18C:{const char*_tmp48A;Cyc_Absyndump_dump(((
_tmp48A="'",_tag_dyneither(_tmp48A,sizeof(char),2))));}Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape(_tmp27E));{const char*_tmp48B;Cyc_Absyndump_dump_nospace(((
_tmp48B="'",_tag_dyneither(_tmp48B,sizeof(char),2))));}goto _LL180;_LL18D: {
struct Cyc_Absyn_Float_p_struct*_tmp27F=(struct Cyc_Absyn_Float_p_struct*)_tmp271;
if(_tmp27F->tag != 11)goto _LL18F;else{_tmp280=_tmp27F->f1;}}_LL18E: Cyc_Absyndump_dump(
_tmp280);goto _LL180;_LL18F: {struct Cyc_Absyn_Var_p_struct*_tmp281=(struct Cyc_Absyn_Var_p_struct*)
_tmp271;if(_tmp281->tag != 1)goto _LL191;else{_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;
_tmp284=*_tmp283;_tmp285=_tmp284.r;{struct Cyc_Absyn_Wild_p_struct*_tmp286=(
struct Cyc_Absyn_Wild_p_struct*)_tmp285;if(_tmp286->tag != 0)goto _LL191;};}}_LL190:
Cyc_Absyndump_dumpqvar(_tmp282->name);goto _LL180;_LL191: {struct Cyc_Absyn_Var_p_struct*
_tmp287=(struct Cyc_Absyn_Var_p_struct*)_tmp271;if(_tmp287->tag != 1)goto _LL193;
else{_tmp288=_tmp287->f1;_tmp289=_tmp287->f2;}}_LL192: Cyc_Absyndump_dumpqvar(
_tmp288->name);{const char*_tmp48C;Cyc_Absyndump_dump(((_tmp48C=" as ",
_tag_dyneither(_tmp48C,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp289);goto
_LL180;_LL193: {struct Cyc_Absyn_Tuple_p_struct*_tmp28A=(struct Cyc_Absyn_Tuple_p_struct*)
_tmp271;if(_tmp28A->tag != 4)goto _LL195;else{_tmp28B=_tmp28A->f1;_tmp28C=_tmp28A->f2;}}
_LL194: {const char*_tmp48E;const char*_tmp48D;struct _dyneither_ptr term=_tmp28C?(
_tmp48E=", ...)",_tag_dyneither(_tmp48E,sizeof(char),7)):((_tmp48D=")",
_tag_dyneither(_tmp48D,sizeof(char),2)));{const char*_tmp490;const char*_tmp48F;((
void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp28B,((_tmp48F="$(",_tag_dyneither(_tmp48F,sizeof(char),
3))),term,((_tmp490=",",_tag_dyneither(_tmp490,sizeof(char),2))));}goto _LL180;}
_LL195: {struct Cyc_Absyn_Pointer_p_struct*_tmp28D=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp271;if(_tmp28D->tag != 5)goto _LL197;else{_tmp28E=_tmp28D->f1;}}_LL196:{const
char*_tmp491;Cyc_Absyndump_dump(((_tmp491="&",_tag_dyneither(_tmp491,sizeof(char),
2))));}Cyc_Absyndump_dumppat(_tmp28E);goto _LL180;_LL197: {struct Cyc_Absyn_Reference_p_struct*
_tmp28F=(struct Cyc_Absyn_Reference_p_struct*)_tmp271;if(_tmp28F->tag != 2)goto
_LL199;else{_tmp290=_tmp28F->f1;_tmp291=_tmp28F->f2;_tmp292=*_tmp291;_tmp293=
_tmp292.r;{struct Cyc_Absyn_Wild_p_struct*_tmp294=(struct Cyc_Absyn_Wild_p_struct*)
_tmp293;if(_tmp294->tag != 0)goto _LL199;};}}_LL198:{const char*_tmp492;Cyc_Absyndump_dump(((
_tmp492="*",_tag_dyneither(_tmp492,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(
_tmp290->name);goto _LL180;_LL199: {struct Cyc_Absyn_Reference_p_struct*_tmp295=(
struct Cyc_Absyn_Reference_p_struct*)_tmp271;if(_tmp295->tag != 2)goto _LL19B;else{
_tmp296=_tmp295->f1;_tmp297=_tmp295->f2;}}_LL19A:{const char*_tmp493;Cyc_Absyndump_dump(((
_tmp493="*",_tag_dyneither(_tmp493,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(
_tmp296->name);{const char*_tmp494;Cyc_Absyndump_dump(((_tmp494=" as ",
_tag_dyneither(_tmp494,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp297);goto
_LL180;_LL19B: {struct Cyc_Absyn_TagInt_p_struct*_tmp298=(struct Cyc_Absyn_TagInt_p_struct*)
_tmp271;if(_tmp298->tag != 3)goto _LL19D;else{_tmp299=_tmp298->f1;_tmp29A=_tmp298->f2;}}
_LL19C: Cyc_Absyndump_dumpqvar(_tmp29A->name);Cyc_Absyndump_dump_char((int)'<');
Cyc_Absyndump_dumptvar(_tmp299);Cyc_Absyndump_dump_char((int)'>');goto _LL180;
_LL19D: {struct Cyc_Absyn_UnknownId_p_struct*_tmp29B=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp271;if(_tmp29B->tag != 14)goto _LL19F;else{_tmp29C=_tmp29B->f1;}}_LL19E: Cyc_Absyndump_dumpqvar(
_tmp29C);goto _LL180;_LL19F: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp29D=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp271;if(_tmp29D->tag != 15)goto _LL1A1;
else{_tmp29E=_tmp29D->f1;_tmp29F=_tmp29D->f2;_tmp2A0=_tmp29D->f3;}}_LL1A0: {
const char*_tmp496;const char*_tmp495;struct _dyneither_ptr term=_tmp2A0?(_tmp496=", ...)",
_tag_dyneither(_tmp496,sizeof(char),7)):((_tmp495=")",_tag_dyneither(_tmp495,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp29E);{const char*_tmp498;const char*
_tmp497;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp29F,((_tmp497="(",_tag_dyneither(_tmp497,sizeof(char),
2))),term,((_tmp498=",",_tag_dyneither(_tmp498,sizeof(char),2))));}goto _LL180;}
_LL1A1: {struct Cyc_Absyn_Aggr_p_struct*_tmp2A1=(struct Cyc_Absyn_Aggr_p_struct*)
_tmp271;if(_tmp2A1->tag != 6)goto _LL1A3;else{_tmp2A2=_tmp2A1->f1;if(_tmp2A2 == 0)
goto _LL1A3;_tmp2A3=*_tmp2A2;_tmp2A4=_tmp2A3.aggr_info;_tmp2A5=_tmp2A1->f2;
_tmp2A6=_tmp2A1->f3;_tmp2A7=_tmp2A1->f4;}}_LL1A2: {struct _tuple0*_tmp2CE;struct
_tuple9 _tmp2CD=Cyc_Absyn_aggr_kinded_name(_tmp2A4);_tmp2CE=_tmp2CD.f2;{const char*
_tmp49A;const char*_tmp499;struct _dyneither_ptr term=_tmp2A7?(_tmp49A=", ...)",
_tag_dyneither(_tmp49A,sizeof(char),7)):((_tmp499=")",_tag_dyneither(_tmp499,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2CE);Cyc_Absyndump_dump_char((int)'{');{
const char*_tmp49D;const char*_tmp49C;const char*_tmp49B;((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp2A5,((_tmp49B="[",_tag_dyneither(_tmp49B,sizeof(char),2))),((_tmp49C="]",
_tag_dyneither(_tmp49C,sizeof(char),2))),((_tmp49D=",",_tag_dyneither(_tmp49D,
sizeof(char),2))));}{const char*_tmp49F;const char*_tmp49E;((void(*)(void(*f)(
struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp2A6,((_tmp49E="",_tag_dyneither(_tmp49E,sizeof(char),1))),term,((_tmp49F=",",
_tag_dyneither(_tmp49F,sizeof(char),2))));}goto _LL180;};}_LL1A3: {struct Cyc_Absyn_Aggr_p_struct*
_tmp2A8=(struct Cyc_Absyn_Aggr_p_struct*)_tmp271;if(_tmp2A8->tag != 6)goto _LL1A5;
else{_tmp2A9=_tmp2A8->f1;if(_tmp2A9 != 0)goto _LL1A5;_tmp2AA=_tmp2A8->f2;_tmp2AB=
_tmp2A8->f3;_tmp2AC=_tmp2A8->f4;}}_LL1A4: {const char*_tmp4A1;const char*_tmp4A0;
struct _dyneither_ptr term=_tmp2AC?(_tmp4A1=", ...)",_tag_dyneither(_tmp4A1,
sizeof(char),7)):((_tmp4A0=")",_tag_dyneither(_tmp4A0,sizeof(char),2)));Cyc_Absyndump_dump_char((
int)'{');{const char*_tmp4A4;const char*_tmp4A3;const char*_tmp4A2;((void(*)(void(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp2AA,((_tmp4A2="[",_tag_dyneither(_tmp4A2,sizeof(char),2))),((_tmp4A3="]",
_tag_dyneither(_tmp4A3,sizeof(char),2))),((_tmp4A4=",",_tag_dyneither(_tmp4A4,
sizeof(char),2))));}{const char*_tmp4A6;const char*_tmp4A5;((void(*)(void(*f)(
struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp2AB,((_tmp4A5="",_tag_dyneither(_tmp4A5,sizeof(char),1))),term,((_tmp4A6=",",
_tag_dyneither(_tmp4A6,sizeof(char),2))));}goto _LL180;}_LL1A5: {struct Cyc_Absyn_Datatype_p_struct*
_tmp2AD=(struct Cyc_Absyn_Datatype_p_struct*)_tmp271;if(_tmp2AD->tag != 7)goto
_LL1A7;else{_tmp2AE=_tmp2AD->f2;_tmp2AF=_tmp2AD->f3;_tmp2B0=_tmp2AD->f4;}}_LL1A6: {
const char*_tmp4A8;const char*_tmp4A7;struct _dyneither_ptr term=_tmp2B0?(_tmp4A8=", ...)",
_tag_dyneither(_tmp4A8,sizeof(char),7)):((_tmp4A7=")",_tag_dyneither(_tmp4A7,
sizeof(char),2)));Cyc_Absyndump_dumpqvar(_tmp2AE->name);if(_tmp2AF != 0){const
char*_tmp4AA;const char*_tmp4A9;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct
Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct
_dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp2AF,((_tmp4A9="(",
_tag_dyneither(_tmp4A9,sizeof(char),2))),term,((_tmp4AA=",",_tag_dyneither(
_tmp4AA,sizeof(char),2))));}goto _LL180;}_LL1A7: {struct Cyc_Absyn_Enum_p_struct*
_tmp2B1=(struct Cyc_Absyn_Enum_p_struct*)_tmp271;if(_tmp2B1->tag != 12)goto _LL1A9;
else{_tmp2B2=_tmp2B1->f2;}}_LL1A8: _tmp2B4=_tmp2B2;goto _LL1AA;_LL1A9: {struct Cyc_Absyn_AnonEnum_p_struct*
_tmp2B3=(struct Cyc_Absyn_AnonEnum_p_struct*)_tmp271;if(_tmp2B3->tag != 13)goto
_LL1AB;else{_tmp2B4=_tmp2B3->f2;}}_LL1AA: Cyc_Absyndump_dumpqvar(_tmp2B4->name);
goto _LL180;_LL1AB: {struct Cyc_Absyn_Exp_p_struct*_tmp2B5=(struct Cyc_Absyn_Exp_p_struct*)
_tmp271;if(_tmp2B5->tag != 16)goto _LL180;else{_tmp2B6=_tmp2B5->f1;}}_LL1AC: Cyc_Absyndump_dumpexp(
_tmp2B6);goto _LL180;_LL180:;}void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){const char*
_tmp4AB;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,
struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,
fields,((_tmp4AB=",",_tag_dyneither(_tmp4AB,sizeof(char),2))));}void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){{
const char*_tmp4AC;Cyc_Absyndump_dump(((_tmp4AC=" = ",_tag_dyneither(_tmp4AC,
sizeof(char),4))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){const char*_tmp4AD;((
void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct
_dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,fields,((
_tmp4AD=",",_tag_dyneither(_tmp4AD,sizeof(char),2))));}void Cyc_Absyndump_dumpaggrfields(
struct Cyc_List_List*fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield
_tmp2E5;struct _dyneither_ptr*_tmp2E6;struct Cyc_Absyn_Tqual _tmp2E7;void*_tmp2E8;
struct Cyc_Absyn_Exp*_tmp2E9;struct Cyc_List_List*_tmp2EA;struct Cyc_Absyn_Aggrfield*
_tmp2E4=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp2E5=*_tmp2E4;_tmp2E6=_tmp2E5.name;
_tmp2E7=_tmp2E5.tq;_tmp2E8=_tmp2E5.type;_tmp2E9=_tmp2E5.width;_tmp2EA=_tmp2E5.attributes;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1AD:((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(
_tmp2E7,_tmp2E8,Cyc_Absyndump_dump_str,_tmp2E6);Cyc_Absyndump_dumpatts(_tmp2EA);
break;case Cyc_Cyclone_Vc_c: _LL1AE: Cyc_Absyndump_dumpatts(_tmp2EA);((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))
Cyc_Absyndump_dumptqtd)(_tmp2E7,_tmp2E8,Cyc_Absyndump_dump_str,_tmp2E6);break;}
if(_tmp2E9 != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp((struct
Cyc_Absyn_Exp*)_tmp2E9);}Cyc_Absyndump_dump_semi();}}void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple18{void*f1;struct
_tuple0*f2;};static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple18*pr){{void*
_tmp2EB=(*pr).f1;_LL1B1: {struct Cyc_Absyn_Unused_att_struct*_tmp2EC=(struct Cyc_Absyn_Unused_att_struct*)
_tmp2EB;if(_tmp2EC->tag != 11)goto _LL1B3;}_LL1B2: goto _LL1B0;_LL1B3: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp2ED=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp2EB;if(_tmp2ED->tag != 1)goto
_LL1B5;}_LL1B4:{const char*_tmp4AE;Cyc_Absyndump_dump(((_tmp4AE="_stdcall",
_tag_dyneither(_tmp4AE,sizeof(char),9))));}goto _LL1B0;_LL1B5: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp2EE=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp2EB;if(_tmp2EE->tag != 2)goto
_LL1B7;}_LL1B6:{const char*_tmp4AF;Cyc_Absyndump_dump(((_tmp4AF="_cdecl",
_tag_dyneither(_tmp4AF,sizeof(char),7))));}goto _LL1B0;_LL1B7: {struct Cyc_Absyn_Fastcall_att_struct*
_tmp2EF=(struct Cyc_Absyn_Fastcall_att_struct*)_tmp2EB;if(_tmp2EF->tag != 3)goto
_LL1B9;}_LL1B8:{const char*_tmp4B0;Cyc_Absyndump_dump(((_tmp4B0="_fastcall",
_tag_dyneither(_tmp4B0,sizeof(char),10))));}goto _LL1B0;_LL1B9:;_LL1BA: goto _LL1B0;
_LL1B0:;}Cyc_Absyndump_dumpqvar((*pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 
0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Vardecl
_tmp2F4;enum Cyc_Absyn_Scope _tmp2F5;struct _tuple0*_tmp2F6;struct Cyc_Absyn_Tqual
_tmp2F7;void*_tmp2F8;struct Cyc_Absyn_Exp*_tmp2F9;struct Cyc_List_List*_tmp2FA;
struct Cyc_Absyn_Vardecl*_tmp2F3=vd;_tmp2F4=*_tmp2F3;_tmp2F5=_tmp2F4.sc;_tmp2F6=
_tmp2F4.name;_tmp2F7=_tmp2F4.tq;_tmp2F8=_tmp2F4.type;_tmp2F9=_tmp2F4.initializer;
_tmp2FA=_tmp2F4.attributes;Cyc_Absyndump_dumploc(loc);switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL1BB: if(_tmp2F5 == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp2FB=Cyc_Tcutil_compress(_tmp2F8);_LL1BE: {struct Cyc_Absyn_FnType_struct*
_tmp2FC=(struct Cyc_Absyn_FnType_struct*)_tmp2FB;if(_tmp2FC->tag != 10)goto _LL1C0;}
_LL1BF: goto _LL1BD;_LL1C0:;_LL1C1: Cyc_Absyndump_dumpscope(_tmp2F5);_LL1BD:;}else{
Cyc_Absyndump_dumpscope(_tmp2F5);}((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp2F7,_tmp2F8,Cyc_Absyndump_dumpqvar,
_tmp2F6);Cyc_Absyndump_dumpatts(_tmp2FA);break;case Cyc_Cyclone_Vc_c: _LL1BC: Cyc_Absyndump_dumpatts(
_tmp2FA);Cyc_Absyndump_dumpscope(_tmp2F5);{struct _RegionHandle _tmp2FD=
_new_region("temp");struct _RegionHandle*temp=& _tmp2FD;_push_region(temp);{struct
Cyc_Absyn_Tqual _tmp2FF;void*_tmp300;struct Cyc_List_List*_tmp301;struct _tuple11
_tmp2FE=Cyc_Absynpp_to_tms(temp,_tmp2F7,_tmp2F8);_tmp2FF=_tmp2FE.f1;_tmp300=
_tmp2FE.f2;_tmp301=_tmp2FE.f3;{void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;{
struct Cyc_List_List*tms2=_tmp301;for(0;tms2 != 0;tms2=tms2->tl){void*_tmp302=(
void*)tms2->hd;struct Cyc_List_List*_tmp304;_LL1C4: {struct Cyc_Absyn_Attributes_mod_struct*
_tmp303=(struct Cyc_Absyn_Attributes_mod_struct*)_tmp302;if(_tmp303->tag != 5)goto
_LL1C6;else{_tmp304=_tmp303->f2;}}_LL1C5: for(0;_tmp304 != 0;_tmp304=_tmp304->tl){
void*_tmp305=(void*)_tmp304->hd;_LL1C9: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp306=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp305;if(_tmp306->tag != 1)goto
_LL1CB;}_LL1CA: call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL1C8;_LL1CB: {
struct Cyc_Absyn_Cdecl_att_struct*_tmp307=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmp305;if(_tmp307->tag != 2)goto _LL1CD;}_LL1CC: call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;
goto _LL1C8;_LL1CD: {struct Cyc_Absyn_Fastcall_att_struct*_tmp308=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp305;if(_tmp308->tag != 3)goto _LL1CF;}_LL1CE: call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;
goto _LL1C8;_LL1CF:;_LL1D0: goto _LL1C8;_LL1C8:;}goto _LL1C3;_LL1C6:;_LL1C7: goto
_LL1C3;_LL1C3:;}}Cyc_Absyndump_dumptq(_tmp2FF);Cyc_Absyndump_dumpntyp(_tmp300);{
struct _tuple18 _tmp4B1;struct _tuple18 _tmp309=(_tmp4B1.f1=call_conv,((_tmp4B1.f2=
_tmp2F6,_tmp4B1)));((void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple18*),
struct _tuple18*a))Cyc_Absyndump_dumptms)(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp301),Cyc_Absyndump_dump_callconv_qvar,& _tmp309);};};}
_npop_handler(0);break;;_pop_region(temp);};}if(_tmp2F9 != 0){Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2F9);}Cyc_Absyndump_dump_semi();}
struct _tuple19{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d){void*_tmp30B=d->r;struct Cyc_Absyn_Vardecl*_tmp30D;struct
Cyc_Absyn_Fndecl*_tmp30F;struct Cyc_Absyn_Aggrdecl*_tmp311;struct Cyc_Absyn_Datatypedecl*
_tmp313;struct Cyc_Absyn_Datatypedecl _tmp314;enum Cyc_Absyn_Scope _tmp315;struct
_tuple0*_tmp316;struct Cyc_List_List*_tmp317;struct Cyc_Core_Opt*_tmp318;int
_tmp319;struct Cyc_Absyn_Enumdecl*_tmp31B;struct Cyc_Absyn_Enumdecl _tmp31C;enum 
Cyc_Absyn_Scope _tmp31D;struct _tuple0*_tmp31E;struct Cyc_Core_Opt*_tmp31F;struct
Cyc_Absyn_Pat*_tmp321;struct Cyc_Absyn_Exp*_tmp322;struct Cyc_List_List*_tmp324;
struct Cyc_Absyn_Tvar*_tmp326;struct Cyc_Absyn_Vardecl*_tmp327;int _tmp328;struct
Cyc_Absyn_Exp*_tmp329;struct Cyc_Absyn_Exp*_tmp32B;struct Cyc_Absyn_Tvar*_tmp32C;
struct Cyc_Absyn_Vardecl*_tmp32D;struct Cyc_Absyn_Typedefdecl*_tmp32F;struct
_dyneither_ptr*_tmp331;struct Cyc_List_List*_tmp332;struct _tuple0*_tmp334;struct
Cyc_List_List*_tmp335;struct Cyc_List_List*_tmp337;struct Cyc_List_List*_tmp339;
struct Cyc_List_List*_tmp33A;_LL1D2: {struct Cyc_Absyn_Var_d_struct*_tmp30C=(
struct Cyc_Absyn_Var_d_struct*)_tmp30B;if(_tmp30C->tag != 0)goto _LL1D4;else{
_tmp30D=_tmp30C->f1;}}_LL1D3: Cyc_Absyndump_dumpvardecl(_tmp30D,d->loc);goto
_LL1D1;_LL1D4: {struct Cyc_Absyn_Fn_d_struct*_tmp30E=(struct Cyc_Absyn_Fn_d_struct*)
_tmp30B;if(_tmp30E->tag != 1)goto _LL1D6;else{_tmp30F=_tmp30E->f1;}}_LL1D5: Cyc_Absyndump_dumploc(
d->loc);switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL1F2: Cyc_Absyndump_dumpatts(
_tmp30F->attributes);break;case Cyc_Cyclone_Gcc_c: _LL1F3: break;}if(_tmp30F->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp33D=Cyc_Cyclone_c_compiler;_LL1F6: if(_tmp33D != 
Cyc_Cyclone_Vc_c)goto _LL1F8;_LL1F7:{const char*_tmp4B2;Cyc_Absyndump_dump(((
_tmp4B2="__inline",_tag_dyneither(_tmp4B2,sizeof(char),9))));}goto _LL1F5;_LL1F8:;
_LL1F9:{const char*_tmp4B3;Cyc_Absyndump_dump(((_tmp4B3="inline",_tag_dyneither(
_tmp4B3,sizeof(char),7))));}goto _LL1F5;_LL1F5:;}Cyc_Absyndump_dumpscope(_tmp30F->sc);{
struct Cyc_Absyn_FnType_struct _tmp4B9;struct Cyc_Absyn_FnInfo _tmp4B8;struct Cyc_Absyn_FnType_struct*
_tmp4B7;void*t=(void*)((_tmp4B7=_cycalloc(sizeof(*_tmp4B7)),((_tmp4B7[0]=((
_tmp4B9.tag=10,((_tmp4B9.f1=((_tmp4B8.tvars=_tmp30F->tvs,((_tmp4B8.effect=
_tmp30F->effect,((_tmp4B8.ret_typ=_tmp30F->ret_type,((_tmp4B8.args=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp30F->args),((_tmp4B8.c_varargs=_tmp30F->c_varargs,((_tmp4B8.cyc_varargs=
_tmp30F->cyc_varargs,((_tmp4B8.rgn_po=_tmp30F->rgn_po,((_tmp4B8.attributes=0,
_tmp4B8)))))))))))))))),_tmp4B9)))),_tmp4B7))));switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL1FA:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(
0),t,Cyc_Absyndump_dump_atts_qvar,_tmp30F);break;case Cyc_Cyclone_Vc_c: _LL1FB:((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,
_tmp30F);break;}Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp30F->body,
0);Cyc_Absyndump_dump_char((int)'}');goto _LL1D1;};_LL1D6: {struct Cyc_Absyn_Aggr_d_struct*
_tmp310=(struct Cyc_Absyn_Aggr_d_struct*)_tmp30B;if(_tmp310->tag != 6)goto _LL1D8;
else{_tmp311=_tmp310->f1;}}_LL1D7: Cyc_Absyndump_dumpscope(_tmp311->sc);Cyc_Absyndump_dumpaggr_kind(
_tmp311->kind);Cyc_Absyndump_dumpqvar(_tmp311->name);Cyc_Absyndump_dumptvars(
_tmp311->tvs);if(_tmp311->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp311->impl))->exist_vars
!= 0){const char*_tmp4BC;const char*_tmp4BB;const char*_tmp4BA;((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp311->impl))->exist_vars,((_tmp4BA="<",
_tag_dyneither(_tmp4BA,sizeof(char),2))),((_tmp4BB=">",_tag_dyneither(_tmp4BB,
sizeof(char),2))),((_tmp4BC=",",_tag_dyneither(_tmp4BC,sizeof(char),2))));}if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp311->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp311->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp311->impl))->fields);{const char*_tmp4BD;Cyc_Absyndump_dump(((
_tmp4BD="}",_tag_dyneither(_tmp4BD,sizeof(char),2))));}Cyc_Absyndump_dumpatts(
_tmp311->attributes);{const char*_tmp4BE;Cyc_Absyndump_dump(((_tmp4BE=";",
_tag_dyneither(_tmp4BE,sizeof(char),2))));};}goto _LL1D1;_LL1D8: {struct Cyc_Absyn_Datatype_d_struct*
_tmp312=(struct Cyc_Absyn_Datatype_d_struct*)_tmp30B;if(_tmp312->tag != 7)goto
_LL1DA;else{_tmp313=_tmp312->f1;_tmp314=*_tmp313;_tmp315=_tmp314.sc;_tmp316=
_tmp314.name;_tmp317=_tmp314.tvs;_tmp318=_tmp314.fields;_tmp319=_tmp314.is_extensible;}}
_LL1D9: Cyc_Absyndump_dumpscope(_tmp315);if(_tmp319){const char*_tmp4BF;Cyc_Absyndump_dump(((
_tmp4BF="@extensible ",_tag_dyneither(_tmp4BF,sizeof(char),13))));}{const char*
_tmp4C0;Cyc_Absyndump_dump(((_tmp4C0="datatype ",_tag_dyneither(_tmp4C0,sizeof(
char),10))));}Cyc_Absyndump_dumpqvar(_tmp316);Cyc_Absyndump_dumptvars(_tmp317);
if(_tmp318 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp318->v);{const char*
_tmp4C1;Cyc_Absyndump_dump_nospace(((_tmp4C1="};",_tag_dyneither(_tmp4C1,sizeof(
char),3))));};}goto _LL1D1;_LL1DA: {struct Cyc_Absyn_Enum_d_struct*_tmp31A=(struct
Cyc_Absyn_Enum_d_struct*)_tmp30B;if(_tmp31A->tag != 8)goto _LL1DC;else{_tmp31B=
_tmp31A->f1;_tmp31C=*_tmp31B;_tmp31D=_tmp31C.sc;_tmp31E=_tmp31C.name;_tmp31F=
_tmp31C.fields;}}_LL1DB: Cyc_Absyndump_dumpscope(_tmp31D);{const char*_tmp4C2;Cyc_Absyndump_dump(((
_tmp4C2="enum ",_tag_dyneither(_tmp4C2,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(
_tmp31E);if(_tmp31F == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp31F->v);{const
char*_tmp4C3;Cyc_Absyndump_dump_nospace(((_tmp4C3="};",_tag_dyneither(_tmp4C3,
sizeof(char),3))));};}return;_LL1DC: {struct Cyc_Absyn_Let_d_struct*_tmp320=(
struct Cyc_Absyn_Let_d_struct*)_tmp30B;if(_tmp320->tag != 2)goto _LL1DE;else{
_tmp321=_tmp320->f1;_tmp322=_tmp320->f3;}}_LL1DD:{const char*_tmp4C4;Cyc_Absyndump_dump(((
_tmp4C4="let",_tag_dyneither(_tmp4C4,sizeof(char),4))));}Cyc_Absyndump_dumppat(
_tmp321);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp322);Cyc_Absyndump_dump_semi();
goto _LL1D1;_LL1DE: {struct Cyc_Absyn_Letv_d_struct*_tmp323=(struct Cyc_Absyn_Letv_d_struct*)
_tmp30B;if(_tmp323->tag != 3)goto _LL1E0;else{_tmp324=_tmp323->f1;}}_LL1DF:{const
char*_tmp4C5;Cyc_Absyndump_dump(((_tmp4C5="let ",_tag_dyneither(_tmp4C5,sizeof(
char),5))));}Cyc_Absyndump_dumpids(_tmp324);Cyc_Absyndump_dump_semi();goto _LL1D1;
_LL1E0: {struct Cyc_Absyn_Region_d_struct*_tmp325=(struct Cyc_Absyn_Region_d_struct*)
_tmp30B;if(_tmp325->tag != 4)goto _LL1E2;else{_tmp326=_tmp325->f1;_tmp327=_tmp325->f2;
_tmp328=_tmp325->f3;_tmp329=_tmp325->f4;}}_LL1E1:{const char*_tmp4C6;Cyc_Absyndump_dump(((
_tmp4C6="region",_tag_dyneither(_tmp4C6,sizeof(char),7))));}{const char*_tmp4C7;
Cyc_Absyndump_dump(((_tmp4C7="<",_tag_dyneither(_tmp4C7,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp326);{const char*_tmp4C8;Cyc_Absyndump_dump(((_tmp4C8="> ",_tag_dyneither(
_tmp4C8,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp327->name);if(_tmp328){
const char*_tmp4C9;Cyc_Absyndump_dump(((_tmp4C9="@resetable",_tag_dyneither(
_tmp4C9,sizeof(char),11))));}if(_tmp329 != 0){{const char*_tmp4CA;Cyc_Absyndump_dump(((
_tmp4CA=" = open(",_tag_dyneither(_tmp4CA,sizeof(char),9))));}Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp329);{const char*_tmp4CB;Cyc_Absyndump_dump(((_tmp4CB=")",
_tag_dyneither(_tmp4CB,sizeof(char),2))));};}Cyc_Absyndump_dump_semi();goto
_LL1D1;_LL1E2: {struct Cyc_Absyn_Alias_d_struct*_tmp32A=(struct Cyc_Absyn_Alias_d_struct*)
_tmp30B;if(_tmp32A->tag != 5)goto _LL1E4;else{_tmp32B=_tmp32A->f1;_tmp32C=_tmp32A->f2;
_tmp32D=_tmp32A->f3;}}_LL1E3:{const char*_tmp4CC;Cyc_Absyndump_dump(((_tmp4CC="alias ",
_tag_dyneither(_tmp4CC,sizeof(char),7))));}{const char*_tmp4CD;Cyc_Absyndump_dump(((
_tmp4CD="<",_tag_dyneither(_tmp4CD,sizeof(char),2))));}Cyc_Absyndump_dumptvar(
_tmp32C);{const char*_tmp4CE;Cyc_Absyndump_dump(((_tmp4CE=">",_tag_dyneither(
_tmp4CE,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp32D->name);{const char*
_tmp4CF;Cyc_Absyndump_dump(((_tmp4CF=" = ",_tag_dyneither(_tmp4CF,sizeof(char),4))));}
Cyc_Absyndump_dumpexp(_tmp32B);Cyc_Absyndump_dump_semi();goto _LL1D1;_LL1E4: {
struct Cyc_Absyn_Typedef_d_struct*_tmp32E=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp30B;if(_tmp32E->tag != 9)goto _LL1E6;else{_tmp32F=_tmp32E->f1;}}_LL1E5: if(!Cyc_Absyndump_expand_typedefs
 || _tmp32F->defn != 0  && Cyc_Absynpp_is_anon_aggrtype((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp32F->defn))->v)){{const char*_tmp4D0;Cyc_Absyndump_dump(((_tmp4D0="typedef",
_tag_dyneither(_tmp4D0,sizeof(char),8))));}{void*t;if(_tmp32F->defn == 0)t=Cyc_Absyn_new_evar(
_tmp32F->kind,0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp32F->defn))->v;}((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp32F->tq,t,Cyc_Absyndump_dumptypedefname,
_tmp32F);Cyc_Absyndump_dumpatts(_tmp32F->atts);Cyc_Absyndump_dump_semi();};}goto
_LL1D1;_LL1E6: {struct Cyc_Absyn_Namespace_d_struct*_tmp330=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp30B;if(_tmp330->tag != 10)goto _LL1E8;else{_tmp331=_tmp330->f1;_tmp332=_tmp330->f2;}}
_LL1E7:{const char*_tmp4D1;Cyc_Absyndump_dump(((_tmp4D1="namespace",
_tag_dyneither(_tmp4D1,sizeof(char),10))));}Cyc_Absyndump_dump_str(_tmp331);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp332 != 0;_tmp332=_tmp332->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp332->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1D1;
_LL1E8: {struct Cyc_Absyn_Using_d_struct*_tmp333=(struct Cyc_Absyn_Using_d_struct*)
_tmp30B;if(_tmp333->tag != 11)goto _LL1EA;else{_tmp334=_tmp333->f1;_tmp335=_tmp333->f2;}}
_LL1E9:{const char*_tmp4D2;Cyc_Absyndump_dump(((_tmp4D2="using",_tag_dyneither(
_tmp4D2,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmp334);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp335 != 0;_tmp335=_tmp335->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp335->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1D1;
_LL1EA: {struct Cyc_Absyn_ExternC_d_struct*_tmp336=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp30B;if(_tmp336->tag != 12)goto _LL1EC;else{_tmp337=_tmp336->f1;}}_LL1EB:{const
char*_tmp4D3;Cyc_Absyndump_dump(((_tmp4D3="extern \"C\" {",_tag_dyneither(
_tmp4D3,sizeof(char),13))));}for(0;_tmp337 != 0;_tmp337=_tmp337->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp337->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1D1;
_LL1EC: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp338=(struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp30B;if(_tmp338->tag != 13)goto _LL1EE;else{_tmp339=_tmp338->f1;_tmp33A=_tmp338->f2;}}
_LL1ED:{const char*_tmp4D4;Cyc_Absyndump_dump(((_tmp4D4="extern \"C include\" {",
_tag_dyneither(_tmp4D4,sizeof(char),21))));}for(0;_tmp339 != 0;_tmp339=_tmp339->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp339->hd);}Cyc_Absyndump_dump_char((
int)'}');if(_tmp33A != 0){{const char*_tmp4D5;Cyc_Absyndump_dump(((_tmp4D5=" export {",
_tag_dyneither(_tmp4D5,sizeof(char),10))));}for(0;_tmp33A != 0;_tmp33A=_tmp33A->tl){
struct _tuple0*_tmp360;struct _tuple19 _tmp35F=*((struct _tuple19*)_tmp33A->hd);
_tmp360=_tmp35F.f2;Cyc_Absyndump_dumpqvar(_tmp360);if(_tmp33A->tl != 0)Cyc_Absyndump_dump_char((
int)',');}{const char*_tmp4D6;Cyc_Absyndump_dump(((_tmp4D6="}",_tag_dyneither(
_tmp4D6,sizeof(char),2))));};}goto _LL1D1;_LL1EE: {struct Cyc_Absyn_Porton_d_struct*
_tmp33B=(struct Cyc_Absyn_Porton_d_struct*)_tmp30B;if(_tmp33B->tag != 14)goto
_LL1F0;}_LL1EF:{const char*_tmp4D7;Cyc_Absyndump_dump(((_tmp4D7=" __cyclone_port_on__; ",
_tag_dyneither(_tmp4D7,sizeof(char),23))));}goto _LL1D1;_LL1F0: {struct Cyc_Absyn_Portoff_d_struct*
_tmp33C=(struct Cyc_Absyn_Portoff_d_struct*)_tmp30B;if(_tmp33C->tag != 15)goto
_LL1D1;}_LL1F1:{const char*_tmp4D8;Cyc_Absyndump_dump(((_tmp4D8=" __cyclone_port_off__; ",
_tag_dyneither(_tmp4D8,sizeof(char),24))));}goto _LL1D1;_LL1D1:;}static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp*e){struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1
!= 1  || !pr.f2){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((
int)'}');}}void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(void*),
void*a){if(tms == 0){f(a);return;}{void*_tmp364=(void*)tms->hd;struct Cyc_Absyn_PtrAtts
_tmp366;void*_tmp367;union Cyc_Absyn_Constraint*_tmp368;union Cyc_Absyn_Constraint*
_tmp369;union Cyc_Absyn_Constraint*_tmp36A;struct Cyc_Absyn_Tqual _tmp36B;_LL1FE: {
struct Cyc_Absyn_Pointer_mod_struct*_tmp365=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp364;if(_tmp365->tag != 2)goto _LL200;else{_tmp366=_tmp365->f1;_tmp367=_tmp366.rgn;
_tmp368=_tmp366.nullable;_tmp369=_tmp366.bounds;_tmp36A=_tmp366.zero_term;
_tmp36B=_tmp365->f2;}}_LL1FF:{void*_tmp36C=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp369);struct Cyc_Absyn_Exp*
_tmp36F;_LL203: {struct Cyc_Absyn_DynEither_b_struct*_tmp36D=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp36C;if(_tmp36D->tag != 0)goto _LL205;}_LL204: Cyc_Absyndump_dump_char((int)'?');
goto _LL202;_LL205: {struct Cyc_Absyn_Upper_b_struct*_tmp36E=(struct Cyc_Absyn_Upper_b_struct*)
_tmp36C;if(_tmp36E->tag != 1)goto _LL202;else{_tmp36F=_tmp36E->f1;}}_LL206: Cyc_Absyndump_dump_char((
int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp368)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp36F);goto _LL202;_LL202:;}if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp36A)){const char*_tmp4D9;
Cyc_Absyndump_dump(((_tmp4D9="@zeroterm",_tag_dyneither(_tmp4D9,sizeof(char),10))));}{
void*_tmp371=Cyc_Tcutil_compress(_tmp367);struct Cyc_Absyn_Tvar*_tmp375;struct Cyc_Core_Opt*
_tmp377;_LL208: {struct Cyc_Absyn_HeapRgn_struct*_tmp372=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp371;if(_tmp372->tag != 21)goto _LL20A;}_LL209: goto _LL207;_LL20A: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp373=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp371;if(_tmp373->tag != 22)goto
_LL20C;}_LL20B:{const char*_tmp4DA;Cyc_Absyndump_dump(((_tmp4DA="`U",
_tag_dyneither(_tmp4DA,sizeof(char),3))));}goto _LL207;_LL20C: {struct Cyc_Absyn_VarType_struct*
_tmp374=(struct Cyc_Absyn_VarType_struct*)_tmp371;if(_tmp374->tag != 2)goto _LL20E;
else{_tmp375=_tmp374->f1;}}_LL20D: Cyc_Absyndump_dump_str(_tmp375->name);goto
_LL207;_LL20E: {struct Cyc_Absyn_Evar_struct*_tmp376=(struct Cyc_Absyn_Evar_struct*)
_tmp371;if(_tmp376->tag != 1)goto _LL210;else{_tmp377=_tmp376->f2;if(_tmp377 != 0)
goto _LL210;}}_LL20F: Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp367));goto
_LL207;_LL210:;_LL211: {const char*_tmp4DD;void*_tmp4DC;(_tmp4DC=0,Cyc_Tcutil_impos(((
_tmp4DD="dumptms: bad rgn type in Pointer_mod",_tag_dyneither(_tmp4DD,sizeof(
char),37))),_tag_dyneither(_tmp4DC,sizeof(void*),0)));}_LL207:;}Cyc_Absyndump_dumptq(
_tmp36B);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL200:;_LL201: {int
next_is_pointer=0;if(tms->tl != 0){void*_tmp37B=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;_LL213: {struct Cyc_Absyn_Pointer_mod_struct*_tmp37C=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp37B;if(_tmp37C->tag != 2)goto _LL215;}
_LL214: next_is_pointer=1;goto _LL212;_LL215:;_LL216: goto _LL212;_LL212:;}if(
next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp37D=(void*)
tms->hd;union Cyc_Absyn_Constraint*_tmp37F;struct Cyc_Absyn_Exp*_tmp381;union Cyc_Absyn_Constraint*
_tmp382;void*_tmp384;struct Cyc_List_List*_tmp386;int _tmp387;struct Cyc_Absyn_VarargInfo*
_tmp388;struct Cyc_Core_Opt*_tmp389;struct Cyc_List_List*_tmp38A;void*_tmp38C;
struct Cyc_List_List*_tmp38E;struct Cyc_Position_Segment*_tmp38F;struct Cyc_List_List*
_tmp391;struct Cyc_Position_Segment*_tmp392;int _tmp393;struct Cyc_List_List*
_tmp395;_LL218: {struct Cyc_Absyn_Carray_mod_struct*_tmp37E=(struct Cyc_Absyn_Carray_mod_struct*)
_tmp37D;if(_tmp37E->tag != 0)goto _LL21A;else{_tmp37F=_tmp37E->f1;}}_LL219:{const
char*_tmp4DE;Cyc_Absyndump_dump(((_tmp4DE="[]",_tag_dyneither(_tmp4DE,sizeof(
char),3))));}if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp37F)){const char*_tmp4DF;Cyc_Absyndump_dump(((_tmp4DF="@zeroterm",
_tag_dyneither(_tmp4DF,sizeof(char),10))));}goto _LL217;_LL21A: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmp380=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmp37D;if(_tmp380->tag != 1)goto
_LL21C;else{_tmp381=_tmp380->f1;_tmp382=_tmp380->f2;}}_LL21B: Cyc_Absyndump_dump_char((
int)'[');Cyc_Absyndump_dumpexp(_tmp381);Cyc_Absyndump_dump_char((int)']');if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp382)){const
char*_tmp4E0;Cyc_Absyndump_dump(((_tmp4E0="@zeroterm",_tag_dyneither(_tmp4E0,
sizeof(char),10))));}goto _LL217;_LL21C: {struct Cyc_Absyn_Function_mod_struct*
_tmp383=(struct Cyc_Absyn_Function_mod_struct*)_tmp37D;if(_tmp383->tag != 3)goto
_LL21E;else{_tmp384=(void*)_tmp383->f1;{struct Cyc_Absyn_WithTypes_struct*_tmp385=(
struct Cyc_Absyn_WithTypes_struct*)_tmp384;if(_tmp385->tag != 1)goto _LL21E;else{
_tmp386=_tmp385->f1;_tmp387=_tmp385->f2;_tmp388=_tmp385->f3;_tmp389=_tmp385->f4;
_tmp38A=_tmp385->f5;}};}}_LL21D: Cyc_Absyndump_dumpfunargs(_tmp386,_tmp387,
_tmp388,_tmp389,_tmp38A);goto _LL217;_LL21E: {struct Cyc_Absyn_Function_mod_struct*
_tmp38B=(struct Cyc_Absyn_Function_mod_struct*)_tmp37D;if(_tmp38B->tag != 3)goto
_LL220;else{_tmp38C=(void*)_tmp38B->f1;{struct Cyc_Absyn_NoTypes_struct*_tmp38D=(
struct Cyc_Absyn_NoTypes_struct*)_tmp38C;if(_tmp38D->tag != 0)goto _LL220;else{
_tmp38E=_tmp38D->f1;_tmp38F=_tmp38D->f2;}};}}_LL21F:{const char*_tmp4E3;const char*
_tmp4E2;const char*_tmp4E1;((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str,_tmp38E,((_tmp4E1="(",_tag_dyneither(_tmp4E1,sizeof(char),
2))),((_tmp4E2=")",_tag_dyneither(_tmp4E2,sizeof(char),2))),((_tmp4E3=",",
_tag_dyneither(_tmp4E3,sizeof(char),2))));}goto _LL217;_LL220: {struct Cyc_Absyn_TypeParams_mod_struct*
_tmp390=(struct Cyc_Absyn_TypeParams_mod_struct*)_tmp37D;if(_tmp390->tag != 4)goto
_LL222;else{_tmp391=_tmp390->f1;_tmp392=_tmp390->f2;_tmp393=_tmp390->f3;}}_LL221:
if(_tmp393)Cyc_Absyndump_dumpkindedtvars(_tmp391);else{Cyc_Absyndump_dumptvars(
_tmp391);}goto _LL217;_LL222: {struct Cyc_Absyn_Attributes_mod_struct*_tmp394=(
struct Cyc_Absyn_Attributes_mod_struct*)_tmp37D;if(_tmp394->tag != 5)goto _LL224;
else{_tmp395=_tmp394->f2;}}_LL223: Cyc_Absyndump_dumpatts(_tmp395);goto _LL217;
_LL224: {struct Cyc_Absyn_Pointer_mod_struct*_tmp396=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmp37D;if(_tmp396->tag != 2)goto _LL217;}_LL225: {const char*_tmp4E6;void*_tmp4E5;(
_tmp4E5=0,Cyc_Tcutil_impos(((_tmp4E6="dumptms",_tag_dyneither(_tmp4E6,sizeof(
char),8))),_tag_dyneither(_tmp4E5,sizeof(void*),0)));}_LL217:;}return;}_LL1FD:;};}
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
struct _RegionHandle _tmp39F=_new_region("temp");struct _RegionHandle*temp=& _tmp39F;
_push_region(temp);{struct Cyc_Absyn_Tqual _tmp3A1;void*_tmp3A2;struct Cyc_List_List*
_tmp3A3;struct _tuple11 _tmp3A0=Cyc_Absynpp_to_tms(temp,tq,t);_tmp3A1=_tmp3A0.f1;
_tmp3A2=_tmp3A0.f2;_tmp3A3=_tmp3A0.f3;Cyc_Absyndump_dumptq(_tmp3A1);Cyc_Absyndump_dumpntyp(
_tmp3A2);Cyc_Absyndump_dumptms(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp3A3),f,a);};_pop_region(temp);}void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=
f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
const char*_tmp4E9;void*_tmp4E8;(_tmp4E8=0,Cyc_fprintf(f,((_tmp4E9="\n",
_tag_dyneither(_tmp4E9,sizeof(char),2))),_tag_dyneither(_tmp4E8,sizeof(void*),0)));};}
