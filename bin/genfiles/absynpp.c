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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct
_dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};
extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _dyneither_ptr Cyc_Core_new_string(
unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,
unsigned int sz){if(bd >> 20  || sz >> 12)return 0;{unsigned char*ptrbd=dyn.curr + bd * 
sz;if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
return 0;return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);int Cyc_List_list_prefix(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);extern int
Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);struct _dyneither_ptr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();
struct Cyc_PP_Doc*Cyc_PP_line_doc();struct Cyc_PP_Doc*Cyc_PP_text(struct
_dyneither_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);struct
Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);struct Cyc_PP_Doc*Cyc_PP_nest(
int k,struct Cyc_PP_Doc*d);struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr
sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,
struct Cyc_List_List*l0);struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(
void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(
struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,
struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,
struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{int tag;};
struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{int tag;}
;struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};struct _union_Nmspace_Rel_n{int tag;
struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct
_tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{
Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern
 = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz
 = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_AliasQual{
Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};enum Cyc_Absyn_KindQual{
Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind
 = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual
kind;enum Cyc_Absyn_AliasQual aliasqual;};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 
0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA
 = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};
struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};
struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{
struct _union_Constraint_Eq_constr Eq_constr;struct
_union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr
No_constr;};struct Cyc_Absyn_Eq_kb_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct
Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{
struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_struct{
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
int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};struct
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
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
struct Cyc_Buffer_t;unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strptrcmp(
struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple10{unsigned int f1;int f2;}
;struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int
qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_for_cycdoc;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _dyneither_ptr
Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct _dyneither_ptr Cyc_Absynpp_prim2string(
enum Cyc_Absyn_Primop p);struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*
p);struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;extern struct
_dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop
p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple11{struct
_dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(
struct _tuple11*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual
tq,void*t);enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c
 = 1};extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple13*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);struct
Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum 
Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*
dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static char _tmp0[4]="Cyc";struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,
_tmp0 + 4};struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_all_effects;
static int Cyc_Absynpp_print_using_stmts;static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars;static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace;static int Cyc_Absynpp_print_zeroterm;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;int Cyc_Absynpp_print_for_cycdoc=
0;struct Cyc_Absynpp_Params;void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=
fs->decls_first;Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_all_effects=
fs->print_all_effects;Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=
fs->print_externC_stmts;Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=
fs->print_zeroterm;Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=
fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,
1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,0,
0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,
0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,
0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_dyneither_ptr*v){struct Cyc_List_List*_tmp5F5;Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,((_tmp5F5=_cycalloc(sizeof(*_tmp5F5)),((_tmp5F5->hd=v,((
_tmp5F5->tl=0,_tmp5F5)))))));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**
l){if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0: {
const char*_tmp5F6;return(_tmp5F6="\\a",_tag_dyneither(_tmp5F6,sizeof(char),3));}
case '\b': _LL1: {const char*_tmp5F7;return(_tmp5F7="\\b",_tag_dyneither(_tmp5F7,
sizeof(char),3));}case '\f': _LL2: {const char*_tmp5F8;return(_tmp5F8="\\f",
_tag_dyneither(_tmp5F8,sizeof(char),3));}case '\n': _LL3: {const char*_tmp5F9;
return(_tmp5F9="\\n",_tag_dyneither(_tmp5F9,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp5FA;return(_tmp5FA="\\r",_tag_dyneither(_tmp5FA,sizeof(char),3));}
case '\t': _LL5: {const char*_tmp5FB;return(_tmp5FB="\\t",_tag_dyneither(_tmp5FB,
sizeof(char),3));}case '\v': _LL6: {const char*_tmp5FC;return(_tmp5FC="\\v",
_tag_dyneither(_tmp5FC,sizeof(char),3));}case '\\': _LL7: {const char*_tmp5FD;
return(_tmp5FD="\\\\",_tag_dyneither(_tmp5FD,sizeof(char),3));}case '"': _LL8: {
const char*_tmp5FE;return(_tmp5FE="\"",_tag_dyneither(_tmp5FE,sizeof(char),2));}
case '\'': _LL9: {const char*_tmp5FF;return(_tmp5FF="\\'",_tag_dyneither(_tmp5FF,
sizeof(char),3));}default: _LLA: if(c >= ' '  && c <= '~'){struct _dyneither_ptr _tmpC=
Cyc_Core_new_string(2);{char _tmp602;char _tmp601;struct _dyneither_ptr _tmp600;(
_tmp600=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp601=*((char*)
_check_dyneither_subscript(_tmp600,sizeof(char),0)),((_tmp602=c,((
_get_dyneither_size(_tmp600,sizeof(char))== 1  && (_tmp601 == '\000'  && _tmp602 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp600.curr)=_tmp602)))))));}return(struct
_dyneither_ptr)_tmpC;}else{struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);int
j=0;{char _tmp605;char _tmp604;struct _dyneither_ptr _tmp603;(_tmp603=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp604=*((char*)
_check_dyneither_subscript(_tmp603,sizeof(char),0)),((_tmp605='\\',((
_get_dyneither_size(_tmp603,sizeof(char))== 1  && (_tmp604 == '\000'  && _tmp605 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp603.curr)=_tmp605)))))));}{char _tmp608;char
_tmp607;struct _dyneither_ptr _tmp606;(_tmp606=_dyneither_ptr_plus(_tmp10,sizeof(
char),j ++),((_tmp607=*((char*)_check_dyneither_subscript(_tmp606,sizeof(char),0)),((
_tmp608=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp606,
sizeof(char))== 1  && (_tmp607 == '\000'  && _tmp608 != '\000')?_throw_arraybounds():
1,*((char*)_tmp606.curr)=_tmp608)))))));}{char _tmp60B;char _tmp60A;struct
_dyneither_ptr _tmp609;(_tmp609=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp60A=*((char*)_check_dyneither_subscript(_tmp609,sizeof(char),0)),((_tmp60B=(
char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp609,sizeof(char))== 1  && (
_tmp60A == '\000'  && _tmp60B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp609.curr)=
_tmp60B)))))));}{char _tmp60E;char _tmp60D;struct _dyneither_ptr _tmp60C;(_tmp60C=
_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp60D=*((char*)
_check_dyneither_subscript(_tmp60C,sizeof(char),0)),((_tmp60E=(char)('0' + (c & 7)),((
_get_dyneither_size(_tmp60C,sizeof(char))== 1  && (_tmp60D == '\000'  && _tmp60E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp60C.curr)=_tmp60E)))))));}return(struct
_dyneither_ptr)_tmp10;}}}static int Cyc_Absynpp_special(struct _dyneither_ptr s){int
sz=(int)(_get_dyneither_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;++ i){char c=*((
const char*)_check_dyneither_subscript(s,sizeof(char),i));if(((c <= ' '  || c >= '~')
 || c == '"') || c == '\\')return 1;}}return 0;}struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(
_get_dyneither_size(s,sizeof(char))- 1);if(n > 0  && *((const char*)
_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{int len=0;{int i=0;
for(0;i <= n;++ i){char _tmp1D=*((const char*)_check_dyneither_subscript(s,sizeof(
char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')
goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12: goto _LL14;_LL13:
if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;
_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp1D
!= '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C: goto
_LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(
_tmp1D >= ' '  && _tmp1D <= '~')++ len;else{len +=4;}goto _LLC;_LLC:;}}{struct
_dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{int i=0;for(0;
i <= n;++ i){char _tmp1E=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp611;char _tmp610;struct
_dyneither_ptr _tmp60F;(_tmp60F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp610=*((
char*)_check_dyneither_subscript(_tmp60F,sizeof(char),0)),((_tmp611='\\',((
_get_dyneither_size(_tmp60F,sizeof(char))== 1  && (_tmp610 == '\000'  && _tmp611 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp60F.curr)=_tmp611)))))));}{char _tmp614;char
_tmp613;struct _dyneither_ptr _tmp612;(_tmp612=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp613=*((char*)_check_dyneither_subscript(_tmp612,sizeof(char),0)),((
_tmp614='a',((_get_dyneither_size(_tmp612,sizeof(char))== 1  && (_tmp613 == '\000'
 && _tmp614 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp612.curr)=_tmp614)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp617;char _tmp616;struct
_dyneither_ptr _tmp615;(_tmp615=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp616=*((
char*)_check_dyneither_subscript(_tmp615,sizeof(char),0)),((_tmp617='\\',((
_get_dyneither_size(_tmp615,sizeof(char))== 1  && (_tmp616 == '\000'  && _tmp617 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp615.curr)=_tmp617)))))));}{char _tmp61A;char
_tmp619;struct _dyneither_ptr _tmp618;(_tmp618=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp619=*((char*)_check_dyneither_subscript(_tmp618,sizeof(char),0)),((
_tmp61A='b',((_get_dyneither_size(_tmp618,sizeof(char))== 1  && (_tmp619 == '\000'
 && _tmp61A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp618.curr)=_tmp61A)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp61D;char _tmp61C;struct
_dyneither_ptr _tmp61B;(_tmp61B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61C=*((
char*)_check_dyneither_subscript(_tmp61B,sizeof(char),0)),((_tmp61D='\\',((
_get_dyneither_size(_tmp61B,sizeof(char))== 1  && (_tmp61C == '\000'  && _tmp61D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp61B.curr)=_tmp61D)))))));}{char _tmp620;char
_tmp61F;struct _dyneither_ptr _tmp61E;(_tmp61E=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp61F=*((char*)_check_dyneither_subscript(_tmp61E,sizeof(char),0)),((
_tmp620='f',((_get_dyneither_size(_tmp61E,sizeof(char))== 1  && (_tmp61F == '\000'
 && _tmp620 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61E.curr)=_tmp620)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp623;char _tmp622;struct
_dyneither_ptr _tmp621;(_tmp621=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp622=*((
char*)_check_dyneither_subscript(_tmp621,sizeof(char),0)),((_tmp623='\\',((
_get_dyneither_size(_tmp621,sizeof(char))== 1  && (_tmp622 == '\000'  && _tmp623 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp621.curr)=_tmp623)))))));}{char _tmp626;char
_tmp625;struct _dyneither_ptr _tmp624;(_tmp624=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp625=*((char*)_check_dyneither_subscript(_tmp624,sizeof(char),0)),((
_tmp626='n',((_get_dyneither_size(_tmp624,sizeof(char))== 1  && (_tmp625 == '\000'
 && _tmp626 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp624.curr)=_tmp626)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp629;char _tmp628;struct
_dyneither_ptr _tmp627;(_tmp627=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp628=*((
char*)_check_dyneither_subscript(_tmp627,sizeof(char),0)),((_tmp629='\\',((
_get_dyneither_size(_tmp627,sizeof(char))== 1  && (_tmp628 == '\000'  && _tmp629 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp627.curr)=_tmp629)))))));}{char _tmp62C;char
_tmp62B;struct _dyneither_ptr _tmp62A;(_tmp62A=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp62B=*((char*)_check_dyneither_subscript(_tmp62A,sizeof(char),0)),((
_tmp62C='r',((_get_dyneither_size(_tmp62A,sizeof(char))== 1  && (_tmp62B == '\000'
 && _tmp62C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62A.curr)=_tmp62C)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp62F;char _tmp62E;struct
_dyneither_ptr _tmp62D;(_tmp62D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62E=*((
char*)_check_dyneither_subscript(_tmp62D,sizeof(char),0)),((_tmp62F='\\',((
_get_dyneither_size(_tmp62D,sizeof(char))== 1  && (_tmp62E == '\000'  && _tmp62F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp62D.curr)=_tmp62F)))))));}{char _tmp632;char
_tmp631;struct _dyneither_ptr _tmp630;(_tmp630=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp631=*((char*)_check_dyneither_subscript(_tmp630,sizeof(char),0)),((
_tmp632='t',((_get_dyneither_size(_tmp630,sizeof(char))== 1  && (_tmp631 == '\000'
 && _tmp632 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp630.curr)=_tmp632)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp635;char _tmp634;struct
_dyneither_ptr _tmp633;(_tmp633=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp634=*((
char*)_check_dyneither_subscript(_tmp633,sizeof(char),0)),((_tmp635='\\',((
_get_dyneither_size(_tmp633,sizeof(char))== 1  && (_tmp634 == '\000'  && _tmp635 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp633.curr)=_tmp635)))))));}{char _tmp638;char
_tmp637;struct _dyneither_ptr _tmp636;(_tmp636=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp637=*((char*)_check_dyneither_subscript(_tmp636,sizeof(char),0)),((
_tmp638='v',((_get_dyneither_size(_tmp636,sizeof(char))== 1  && (_tmp637 == '\000'
 && _tmp638 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp636.curr)=_tmp638)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp63B;char _tmp63A;struct
_dyneither_ptr _tmp639;(_tmp639=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63A=*((
char*)_check_dyneither_subscript(_tmp639,sizeof(char),0)),((_tmp63B='\\',((
_get_dyneither_size(_tmp639,sizeof(char))== 1  && (_tmp63A == '\000'  && _tmp63B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp639.curr)=_tmp63B)))))));}{char _tmp63E;char
_tmp63D;struct _dyneither_ptr _tmp63C;(_tmp63C=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp63D=*((char*)_check_dyneither_subscript(_tmp63C,sizeof(char),0)),((
_tmp63E='\\',((_get_dyneither_size(_tmp63C,sizeof(char))== 1  && (_tmp63D == '\000'
 && _tmp63E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63C.curr)=_tmp63E)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp641;char _tmp640;struct
_dyneither_ptr _tmp63F;(_tmp63F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp640=*((
char*)_check_dyneither_subscript(_tmp63F,sizeof(char),0)),((_tmp641='\\',((
_get_dyneither_size(_tmp63F,sizeof(char))== 1  && (_tmp640 == '\000'  && _tmp641 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp63F.curr)=_tmp641)))))));}{char _tmp644;char
_tmp643;struct _dyneither_ptr _tmp642;(_tmp642=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp643=*((char*)_check_dyneither_subscript(_tmp642,sizeof(char),0)),((
_tmp644='"',((_get_dyneither_size(_tmp642,sizeof(char))== 1  && (_tmp643 == '\000'
 && _tmp644 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp642.curr)=_tmp644)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp647;char _tmp646;
struct _dyneither_ptr _tmp645;(_tmp645=_dyneither_ptr_plus(t,sizeof(char),j ++),((
_tmp646=*((char*)_check_dyneither_subscript(_tmp645,sizeof(char),0)),((_tmp647=
_tmp1E,((_get_dyneither_size(_tmp645,sizeof(char))== 1  && (_tmp646 == '\000'  && 
_tmp647 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp645.curr)=_tmp647)))))));}
else{{char _tmp64A;char _tmp649;struct _dyneither_ptr _tmp648;(_tmp648=
_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp649=*((char*)
_check_dyneither_subscript(_tmp648,sizeof(char),0)),((_tmp64A='\\',((
_get_dyneither_size(_tmp648,sizeof(char))== 1  && (_tmp649 == '\000'  && _tmp64A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp648.curr)=_tmp64A)))))));}{char _tmp64D;char
_tmp64C;struct _dyneither_ptr _tmp64B;(_tmp64B=_dyneither_ptr_plus(t,sizeof(char),
j ++),((_tmp64C=*((char*)_check_dyneither_subscript(_tmp64B,sizeof(char),0)),((
_tmp64D=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dyneither_size(_tmp64B,sizeof(char))
== 1  && (_tmp64C == '\000'  && _tmp64D != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp64B.curr)=_tmp64D)))))));}{char _tmp650;char _tmp64F;struct _dyneither_ptr
_tmp64E;(_tmp64E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64F=*((char*)
_check_dyneither_subscript(_tmp64E,sizeof(char),0)),((_tmp650=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dyneither_size(_tmp64E,sizeof(char))== 1  && (_tmp64F == '\000'
 && _tmp650 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64E.curr)=_tmp650)))))));}{
char _tmp653;char _tmp652;struct _dyneither_ptr _tmp651;(_tmp651=_dyneither_ptr_plus(
t,sizeof(char),j ++),((_tmp652=*((char*)_check_dyneither_subscript(_tmp651,
sizeof(char),0)),((_tmp653=(char)('0' + (_tmp1E & 7)),((_get_dyneither_size(
_tmp651,sizeof(char))== 1  && (_tmp652 == '\000'  && _tmp653 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp651.curr)=_tmp653)))))));};}goto _LL21;_LL21:;}}
return(struct _dyneither_ptr)t;};};};}static char _tmp64[9]="restrict";static struct
_dyneither_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64,_tmp64 + 9};static char
_tmp65[9]="volatile";static struct _dyneither_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dyneither_ptr
Cyc_Absynpp_const_str={_tmp66,_tmp66,_tmp66 + 6};static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=&
Cyc_Absynpp_restrict_string;static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*
l=0;if(tq.q_restrict){struct Cyc_List_List*_tmp654;l=((_tmp654=_cycalloc(sizeof(*
_tmp654)),((_tmp654->hd=Cyc_Absynpp_restrict_sp,((_tmp654->tl=l,_tmp654))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp655;l=((_tmp655=_cycalloc(sizeof(*
_tmp655)),((_tmp655->hd=Cyc_Absynpp_volatile_sp,((_tmp655->tl=l,_tmp655))))));}
if(tq.print_const){struct Cyc_List_List*_tmp656;l=((_tmp656=_cycalloc(sizeof(*
_tmp656)),((_tmp656->hd=Cyc_Absynpp_const_sp,((_tmp656->tl=l,_tmp656))))));}{
const char*_tmp659;const char*_tmp658;const char*_tmp657;return Cyc_PP_egroup(((
_tmp657="",_tag_dyneither(_tmp657,sizeof(char),1))),((_tmp658=" ",_tag_dyneither(
_tmp658,sizeof(char),2))),((_tmp659=" ",_tag_dyneither(_tmp659,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,l));};}struct _dyneither_ptr Cyc_Absynpp_kind2string(
struct Cyc_Absyn_Kind*ka){struct Cyc_Absyn_Kind _tmp6E;enum Cyc_Absyn_KindQual
_tmp6F;enum Cyc_Absyn_AliasQual _tmp70;struct Cyc_Absyn_Kind*_tmp6D=ka;_tmp6E=*
_tmp6D;_tmp6F=_tmp6E.kind;_tmp70=_tmp6E.aliasqual;switch(_tmp6F){case Cyc_Absyn_AnyKind:
_LL36: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL38: {const char*_tmp65A;return(
_tmp65A="A",_tag_dyneither(_tmp65A,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp65B;return(_tmp65B="UA",_tag_dyneither(_tmp65B,sizeof(char),3));}
case Cyc_Absyn_Top: _LL3A: {const char*_tmp65C;return(_tmp65C="TA",_tag_dyneither(
_tmp65C,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL3D: {const char*_tmp65D;return(_tmp65D="M",_tag_dyneither(_tmp65D,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL3E: {const char*_tmp65E;return(_tmp65E="UM",
_tag_dyneither(_tmp65E,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {const char*
_tmp65F;return(_tmp65F="TM",_tag_dyneither(_tmp65F,sizeof(char),3));}}case Cyc_Absyn_BoxKind:
_LL3C: switch(_tmp70){case Cyc_Absyn_Aliasable: _LL42: {const char*_tmp660;return(
_tmp660="B",_tag_dyneither(_tmp660,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp661;return(_tmp661="UB",_tag_dyneither(_tmp661,sizeof(char),3));}
case Cyc_Absyn_Top: _LL44: {const char*_tmp662;return(_tmp662="TB",_tag_dyneither(
_tmp662,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41: switch(_tmp70){case Cyc_Absyn_Aliasable:
_LL47: {const char*_tmp663;return(_tmp663="R",_tag_dyneither(_tmp663,sizeof(char),
2));}case Cyc_Absyn_Unique: _LL48: {const char*_tmp664;return(_tmp664="UR",
_tag_dyneither(_tmp664,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {const char*
_tmp665;return(_tmp665="TR",_tag_dyneither(_tmp665,sizeof(char),3));}}case Cyc_Absyn_EffKind:
_LL46: {const char*_tmp666;return(_tmp666="E",_tag_dyneither(_tmp666,sizeof(char),
2));}case Cyc_Absyn_IntKind: _LL4B: {const char*_tmp667;return(_tmp667="I",
_tag_dyneither(_tmp667,sizeof(char),2));}}}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct
_dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp7F=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp81;struct Cyc_Absyn_Kind*_tmp84;_LL4E: {struct Cyc_Absyn_Eq_kb_struct*
_tmp80=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7F;if(_tmp80->tag != 0)goto _LL50;else{
_tmp81=_tmp80->f1;}}_LL4F: return Cyc_Absynpp_kind2string(_tmp81);_LL50: {struct
Cyc_Absyn_Unknown_kb_struct*_tmp82=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7F;
if(_tmp82->tag != 1)goto _LL52;}_LL51: if(Cyc_PP_tex_output){const char*_tmp668;
return(_tmp668="{?}",_tag_dyneither(_tmp668,sizeof(char),4));}else{const char*
_tmp669;return(_tmp669="?",_tag_dyneither(_tmp669,sizeof(char),2));}_LL52: {
struct Cyc_Absyn_Less_kb_struct*_tmp83=(struct Cyc_Absyn_Less_kb_struct*)_tmp7F;
if(_tmp83->tag != 2)goto _LL4D;else{_tmp84=_tmp83->f2;}}_LL53: {const char*_tmp66D;
void*_tmp66C[1];struct Cyc_String_pa_struct _tmp66B;return(struct _dyneither_ptr)((
_tmp66B.tag=0,((_tmp66B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
_tmp84)),((_tmp66C[0]=& _tmp66B,Cyc_aprintf(((_tmp66D="<=%s",_tag_dyneither(
_tmp66D,sizeof(char),5))),_tag_dyneither(_tmp66C,sizeof(void*),1))))))));}_LL4D:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){void*_tmp8A=Cyc_Absyn_compress_kb(
c);struct Cyc_Absyn_Kind*_tmp8C;struct Cyc_Absyn_Kind*_tmp8F;_LL55: {struct Cyc_Absyn_Eq_kb_struct*
_tmp8B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL57;else{
_tmp8C=_tmp8B->f1;}}_LL56: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8C));
_LL57: {struct Cyc_Absyn_Unknown_kb_struct*_tmp8D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp8A;if(_tmp8D->tag != 1)goto _LL59;}_LL58: if(Cyc_PP_tex_output){const char*
_tmp66E;return Cyc_PP_text_width(((_tmp66E="{?}",_tag_dyneither(_tmp66E,sizeof(
char),4))),1);}else{const char*_tmp66F;return Cyc_PP_text(((_tmp66F="?",
_tag_dyneither(_tmp66F,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_struct*
_tmp8E=(struct Cyc_Absyn_Less_kb_struct*)_tmp8A;if(_tmp8E->tag != 2)goto _LL54;
else{_tmp8F=_tmp8E->f2;}}_LL5A: return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8F));
_LL54:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){const char*
_tmp672;const char*_tmp671;const char*_tmp670;return Cyc_PP_egroup(((_tmp670="<",
_tag_dyneither(_tmp670,sizeof(char),2))),((_tmp671=">",_tag_dyneither(_tmp671,
sizeof(char),2))),((_tmp672=",",_tag_dyneither(_tmp672,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_typ2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*
tv){void*_tmp95=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp98;
struct Cyc_Absyn_Kind _tmp99;enum Cyc_Absyn_KindQual _tmp9A;enum Cyc_Absyn_AliasQual
_tmp9B;struct Cyc_Absyn_Kind*_tmp9D;struct Cyc_Absyn_Kind*_tmp9F;_LL5C: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp96=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp95;if(_tmp96->tag != 1)goto _LL5E;}
_LL5D: goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_struct*_tmp97=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp95;if(_tmp97->tag != 0)goto _LL60;else{_tmp98=_tmp97->f1;_tmp99=*_tmp98;_tmp9A=
_tmp99.kind;if(_tmp9A != Cyc_Absyn_BoxKind)goto _LL60;_tmp9B=_tmp99.aliasqual;if(
_tmp9B != Cyc_Absyn_Aliasable)goto _LL60;}}_LL5F: return Cyc_PP_textptr(tv->name);
_LL60: {struct Cyc_Absyn_Less_kb_struct*_tmp9C=(struct Cyc_Absyn_Less_kb_struct*)
_tmp95;if(_tmp9C->tag != 2)goto _LL62;else{_tmp9D=_tmp9C->f2;}}_LL61: _tmp9F=_tmp9D;
goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_struct*_tmp9E=(struct Cyc_Absyn_Eq_kb_struct*)
_tmp95;if(_tmp9E->tag != 0)goto _LL5B;else{_tmp9F=_tmp9E->f1;}}_LL63: {const char*
_tmp675;struct Cyc_PP_Doc*_tmp674[3];return(_tmp674[2]=Cyc_Absynpp_kind2doc(
_tmp9F),((_tmp674[1]=Cyc_PP_text(((_tmp675="::",_tag_dyneither(_tmp675,sizeof(
char),3)))),((_tmp674[0]=Cyc_PP_textptr(tv->name),Cyc_PP_cat(_tag_dyneither(
_tmp674,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){const char*_tmp678;const char*_tmp677;const char*_tmp676;
return Cyc_PP_egroup(((_tmp676="<",_tag_dyneither(_tmp676,sizeof(char),2))),((
_tmp677=">",_tag_dyneither(_tmp677,sizeof(char),2))),((_tmp678=",",
_tag_dyneither(_tmp678,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(tvs);{const char*
_tmp67B;const char*_tmp67A;const char*_tmp679;return Cyc_PP_egroup(((_tmp679="<",
_tag_dyneither(_tmp679,sizeof(char),2))),((_tmp67A=">",_tag_dyneither(_tmp67A,
sizeof(char),2))),((_tmp67B=",",_tag_dyneither(_tmp67B,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple15*t){return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*
Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){const char*_tmp67E;const char*_tmp67D;
const char*_tmp67C;return Cyc_PP_group(((_tmp67C="(",_tag_dyneither(_tmp67C,
sizeof(char),2))),((_tmp67D=")",_tag_dyneither(_tmp67D,sizeof(char),2))),((
_tmp67E=",",_tag_dyneither(_tmp67E,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmpAB=att;
_LL65: {struct Cyc_Absyn_Stdcall_att_struct*_tmpAC=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmpAB;if(_tmpAC->tag != 1)goto _LL67;}_LL66: goto _LL68;_LL67: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpAD=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAB;if(_tmpAD->tag != 2)goto _LL69;}
_LL68: goto _LL6A;_LL69: {struct Cyc_Absyn_Fastcall_att_struct*_tmpAE=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpAB;if(_tmpAE->tag != 3)goto _LL6B;}_LL6A: return Cyc_PP_nil_doc();_LL6B:;_LL6C:
return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL64:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmpAF=(void*)atts->hd;
_LL6E: {struct Cyc_Absyn_Stdcall_att_struct*_tmpB0=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmpAF;if(_tmpB0->tag != 1)goto _LL70;}_LL6F: {const char*_tmp67F;return Cyc_PP_text(((
_tmp67F=" _stdcall ",_tag_dyneither(_tmp67F,sizeof(char),11))));}_LL70: {struct
Cyc_Absyn_Cdecl_att_struct*_tmpB1=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpAF;if(
_tmpB1->tag != 2)goto _LL72;}_LL71: {const char*_tmp680;return Cyc_PP_text(((_tmp680=" _cdecl ",
_tag_dyneither(_tmp680,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB2=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}
_LL73: {const char*_tmp681;return Cyc_PP_text(((_tmp681=" _fastcall ",
_tag_dyneither(_tmp681,sizeof(char),12))));}_LL74:;_LL75: goto _LL6D;_LL6D:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB6=(void*)atts2->hd;_LL77: {struct Cyc_Absyn_Stdcall_att_struct*_tmpB7=(
struct Cyc_Absyn_Stdcall_att_struct*)_tmpB6;if(_tmpB7->tag != 1)goto _LL79;}_LL78:
goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_struct*_tmpB8=(struct Cyc_Absyn_Cdecl_att_struct*)
_tmpB6;if(_tmpB8->tag != 2)goto _LL7B;}_LL7A: goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_struct*
_tmpB9=(struct Cyc_Absyn_Fastcall_att_struct*)_tmpB6;if(_tmpB9->tag != 3)goto _LL7D;}
_LL7C: goto _LL76;_LL7D:;_LL7E: hasatt=1;goto _LL76;_LL76:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp68C;const char*_tmp68B;const char*_tmp68A;const char*_tmp689;const
char*_tmp688;struct Cyc_PP_Doc*_tmp687[3];return(_tmp687[2]=Cyc_PP_text(((_tmp688=")",
_tag_dyneither(_tmp688,sizeof(char),2)))),((_tmp687[1]=Cyc_PP_group(((_tmp68B="",
_tag_dyneither(_tmp68B,sizeof(char),1))),((_tmp68A="",_tag_dyneither(_tmp68A,
sizeof(char),1))),((_tmp689=" ",_tag_dyneither(_tmp689,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts)),((_tmp687[0]=Cyc_PP_text(((_tmp68C=" __declspec(",
_tag_dyneither(_tmp68C,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp687,
sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*
a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Vc_c: _LL7F: return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c:
_LL80: {const char*_tmp695;const char*_tmp694;const char*_tmp693;const char*_tmp692;
struct Cyc_PP_Doc*_tmp691[2];return(_tmp691[1]=Cyc_PP_group(((_tmp694="((",
_tag_dyneither(_tmp694,sizeof(char),3))),((_tmp693="))",_tag_dyneither(_tmp693,
sizeof(char),3))),((_tmp692=",",_tag_dyneither(_tmp692,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_att2doc,atts)),((_tmp691[0]=Cyc_PP_text(((_tmp695=" __attribute__",
_tag_dyneither(_tmp695,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp691,
sizeof(struct Cyc_PP_Doc*),2)))));}}}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms){if(tms == 0)return 0;{void*_tmpC5=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_struct*
_tmpC6=(struct Cyc_Absyn_Pointer_mod_struct*)_tmpC5;if(_tmpC6->tag != 2)goto _LL85;}
_LL84: return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_struct*_tmpC7=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpC5;if(_tmpC7->tag != 5)goto _LL87;}_LL86: switch(Cyc_Cyclone_c_compiler){case
Cyc_Cyclone_Gcc_c: _LL89: return 0;default: _LL8A: return Cyc_Absynpp_next_is_pointer(
tms->tl);}_LL87:;_LL88: return 0;_LL82:;};}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t);static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*
Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){if(Cyc_PP_tex_output){
const char*_tmp696;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((
_tmp696="{?}",_tag_dyneither(_tmp696,sizeof(char),4))),1);}else{const char*
_tmp697;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp697="?",
_tag_dyneither(_tmp697,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){const char*_tmp698;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp698="{\\lb}",_tag_dyneither(_tmp698,sizeof(char),6))),1);}
else{const char*_tmp699;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp699="{",_tag_dyneither(_tmp699,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_lb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){const char*_tmp69A;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp69A="{\\rb}",_tag_dyneither(_tmp69A,sizeof(char),6))),1);}
else{const char*_tmp69B;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp69B="}",_tag_dyneither(_tmp69B,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=
0;static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){const char*_tmp69C;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp69C="\\$",_tag_dyneither(_tmp69C,sizeof(char),3))),1);}
else{const char*_tmp69D;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp69D="$",_tag_dyneither(_tmp69D,sizeof(char),2))));}}return(struct Cyc_PP_Doc*)
_check_null(Cyc_Absynpp_cache_dollar);}struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(
struct _dyneither_ptr sep,struct Cyc_List_List*ss){struct Cyc_PP_Doc*_tmp69E[3];
return(_tmp69E[2]=Cyc_Absynpp_rb(),((_tmp69E[1]=Cyc_PP_seq(sep,ss),((_tmp69E[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp69E,sizeof(struct Cyc_PP_Doc*),3)))))));}
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){for(0;tms != 0;tms=tms->tl){
void*_tmpD1=(void*)tms->hd;void*_tmpD5;struct Cyc_List_List*_tmpD7;_LL8D: {struct
Cyc_Absyn_Carray_mod_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_struct*)_tmpD1;
if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:{const char*_tmp6A1;void*_tmp6A0;(_tmp6A0=0,
Cyc_fprintf(Cyc_stderr,((_tmp6A1="Carray_mod ",_tag_dyneither(_tmp6A1,sizeof(
char),12))),_tag_dyneither(_tmp6A0,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct
Cyc_Absyn_ConstArray_mod_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:{const char*_tmp6A4;void*_tmp6A3;(
_tmp6A3=0,Cyc_fprintf(Cyc_stderr,((_tmp6A4="ConstArray_mod ",_tag_dyneither(
_tmp6A4,sizeof(char),16))),_tag_dyneither(_tmp6A3,sizeof(void*),0)));}goto _LL8C;
_LL91: {struct Cyc_Absyn_Function_mod_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{_tmpD5=(void*)_tmpD4->f1;{struct Cyc_Absyn_WithTypes_struct*
_tmpD6=(struct Cyc_Absyn_WithTypes_struct*)_tmpD5;if(_tmpD6->tag != 1)goto _LL93;
else{_tmpD7=_tmpD6->f1;}};}}_LL92:{const char*_tmp6A7;void*_tmp6A6;(_tmp6A6=0,Cyc_fprintf(
Cyc_stderr,((_tmp6A7="Function_mod(",_tag_dyneither(_tmp6A7,sizeof(char),14))),
_tag_dyneither(_tmp6A6,sizeof(void*),0)));}for(0;_tmpD7 != 0;_tmpD7=_tmpD7->tl){
struct Cyc_Core_Opt*_tmpE2=(*((struct _tuple7*)_tmpD7->hd)).f1;if(_tmpE2 == 0){
const char*_tmp6AA;void*_tmp6A9;(_tmp6A9=0,Cyc_fprintf(Cyc_stderr,((_tmp6AA="?",
_tag_dyneither(_tmp6AA,sizeof(char),2))),_tag_dyneither(_tmp6A9,sizeof(void*),0)));}
else{void*_tmp6AB;(_tmp6AB=0,Cyc_fprintf(Cyc_stderr,*((struct _dyneither_ptr*)
_tmpE2->v),_tag_dyneither(_tmp6AB,sizeof(void*),0)));}if(_tmpD7->tl != 0){const
char*_tmp6AE;void*_tmp6AD;(_tmp6AD=0,Cyc_fprintf(Cyc_stderr,((_tmp6AE=",",
_tag_dyneither(_tmp6AE,sizeof(char),2))),_tag_dyneither(_tmp6AD,sizeof(void*),0)));}}{
const char*_tmp6B1;void*_tmp6B0;(_tmp6B0=0,Cyc_fprintf(Cyc_stderr,((_tmp6B1=") ",
_tag_dyneither(_tmp6B1,sizeof(char),3))),_tag_dyneither(_tmp6B0,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_struct*_tmpD8=(struct Cyc_Absyn_Function_mod_struct*)
_tmpD1;if(_tmpD8->tag != 3)goto _LL95;}_LL94:{const char*_tmp6B4;void*_tmp6B3;(
_tmp6B3=0,Cyc_fprintf(Cyc_stderr,((_tmp6B4="Function_mod()",_tag_dyneither(
_tmp6B4,sizeof(char),15))),_tag_dyneither(_tmp6B3,sizeof(void*),0)));}goto _LL8C;
_LL95: {struct Cyc_Absyn_Attributes_mod_struct*_tmpD9=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpD1;if(_tmpD9->tag != 5)goto _LL97;}_LL96:{const char*_tmp6B7;void*_tmp6B6;(
_tmp6B6=0,Cyc_fprintf(Cyc_stderr,((_tmp6B7="Attributes_mod ",_tag_dyneither(
_tmp6B7,sizeof(char),16))),_tag_dyneither(_tmp6B6,sizeof(void*),0)));}goto _LL8C;
_LL97: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpDA=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpD1;if(_tmpDA->tag != 4)goto _LL99;}_LL98:{const char*_tmp6BA;void*_tmp6B9;(
_tmp6B9=0,Cyc_fprintf(Cyc_stderr,((_tmp6BA="TypeParams_mod ",_tag_dyneither(
_tmp6BA,sizeof(char),16))),_tag_dyneither(_tmp6B9,sizeof(void*),0)));}goto _LL8C;
_LL99: {struct Cyc_Absyn_Pointer_mod_struct*_tmpDB=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpD1;if(_tmpDB->tag != 2)goto _LL8C;}_LL9A:{const char*_tmp6BD;void*_tmp6BC;(
_tmp6BC=0,Cyc_fprintf(Cyc_stderr,((_tmp6BD="Pointer_mod ",_tag_dyneither(_tmp6BD,
sizeof(char),13))),_tag_dyneither(_tmp6BC,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
const char*_tmp6C0;void*_tmp6BF;(_tmp6BF=0,Cyc_fprintf(Cyc_stderr,((_tmp6C0="\n",
_tag_dyneither(_tmp6C0,sizeof(char),2))),_tag_dyneither(_tmp6BF,sizeof(void*),0)));};}
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct
Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(
0,d,tms->tl);const char*_tmp6C5;const char*_tmp6C4;struct Cyc_PP_Doc*_tmp6C3[3];
struct Cyc_PP_Doc*p_rest=(_tmp6C3[2]=Cyc_PP_text(((_tmp6C4=")",_tag_dyneither(
_tmp6C4,sizeof(char),2)))),((_tmp6C3[1]=rest,((_tmp6C3[0]=Cyc_PP_text(((_tmp6C5="(",
_tag_dyneither(_tmp6C5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6C3,
sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpF4=(void*)tms->hd;union Cyc_Absyn_Constraint*
_tmpF6;struct Cyc_Absyn_Exp*_tmpF8;union Cyc_Absyn_Constraint*_tmpF9;void*_tmpFB;
struct Cyc_List_List*_tmpFD;struct Cyc_List_List*_tmpFF;struct Cyc_Position_Segment*
_tmp100;int _tmp101;struct Cyc_Absyn_PtrAtts _tmp103;void*_tmp104;union Cyc_Absyn_Constraint*
_tmp105;union Cyc_Absyn_Constraint*_tmp106;union Cyc_Absyn_Constraint*_tmp107;
struct Cyc_Absyn_Tqual _tmp108;_LL9C: {struct Cyc_Absyn_Carray_mod_struct*_tmpF5=(
struct Cyc_Absyn_Carray_mod_struct*)_tmpF4;if(_tmpF5->tag != 0)goto _LL9E;else{
_tmpF6=_tmpF5->f1;}}_LL9D: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6CA;const char*_tmp6C9;struct Cyc_PP_Doc*_tmp6C8[2];return(_tmp6C8[1]=((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF6)?Cyc_PP_text(((
_tmp6C9="[]@zeroterm",_tag_dyneither(_tmp6C9,sizeof(char),12)))): Cyc_PP_text(((
_tmp6CA="[]",_tag_dyneither(_tmp6CA,sizeof(char),3)))),((_tmp6C8[0]=rest,Cyc_PP_cat(
_tag_dyneither(_tmp6C8,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF7=(struct Cyc_Absyn_ConstArray_mod_struct*)_tmpF4;if(_tmpF7->tag != 1)goto
_LLA0;else{_tmpF8=_tmpF7->f1;_tmpF9=_tmpF7->f2;}}_LL9F: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp6D1;const char*_tmp6D0;const char*_tmp6CF;
struct Cyc_PP_Doc*_tmp6CE[4];return(_tmp6CE[3]=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpF9)?Cyc_PP_text(((_tmp6CF="]@zeroterm",
_tag_dyneither(_tmp6CF,sizeof(char),11)))): Cyc_PP_text(((_tmp6D0="]",
_tag_dyneither(_tmp6D0,sizeof(char),2)))),((_tmp6CE[2]=Cyc_Absynpp_exp2doc(
_tmpF8),((_tmp6CE[1]=Cyc_PP_text(((_tmp6D1="[",_tag_dyneither(_tmp6D1,sizeof(
char),2)))),((_tmp6CE[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6CE,sizeof(struct Cyc_PP_Doc*),
4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_struct*_tmpFA=(struct Cyc_Absyn_Function_mod_struct*)
_tmpF4;if(_tmpFA->tag != 3)goto _LLA2;else{_tmpFB=(void*)_tmpFA->f1;}}_LLA1: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp110=_tmpFB;struct Cyc_List_List*_tmp112;int _tmp113;
struct Cyc_Absyn_VarargInfo*_tmp114;struct Cyc_Core_Opt*_tmp115;struct Cyc_List_List*
_tmp116;struct Cyc_List_List*_tmp118;struct Cyc_Position_Segment*_tmp119;_LLA9: {
struct Cyc_Absyn_WithTypes_struct*_tmp111=(struct Cyc_Absyn_WithTypes_struct*)
_tmp110;if(_tmp111->tag != 1)goto _LLAB;else{_tmp112=_tmp111->f1;_tmp113=_tmp111->f2;
_tmp114=_tmp111->f3;_tmp115=_tmp111->f4;_tmp116=_tmp111->f5;}}_LLAA: {struct Cyc_PP_Doc*
_tmp6D2[2];return(_tmp6D2[1]=Cyc_Absynpp_funargs2doc(_tmp112,_tmp113,_tmp114,
_tmp115,_tmp116),((_tmp6D2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6D2,sizeof(
struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_struct*_tmp117=(
struct Cyc_Absyn_NoTypes_struct*)_tmp110;if(_tmp117->tag != 0)goto _LLA8;else{
_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;}}_LLAC: {const char*_tmp6D9;const char*
_tmp6D8;const char*_tmp6D7;struct Cyc_PP_Doc*_tmp6D6[2];return(_tmp6D6[1]=Cyc_PP_group(((
_tmp6D9="(",_tag_dyneither(_tmp6D9,sizeof(char),2))),((_tmp6D8=")",
_tag_dyneither(_tmp6D8,sizeof(char),2))),((_tmp6D7=",",_tag_dyneither(_tmp6D7,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp118)),((
_tmp6D6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6D6,sizeof(struct Cyc_PP_Doc*),2)))));}
_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_struct*_tmpFC=(struct Cyc_Absyn_Attributes_mod_struct*)
_tmpF4;if(_tmpFC->tag != 5)goto _LLA4;else{_tmpFD=_tmpFC->f2;}}_LLA3: switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6DA[2];return(_tmp6DA[1]=Cyc_Absynpp_atts2doc(_tmpFD),((
_tmp6DA[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6DA,sizeof(struct Cyc_PP_Doc*),2)))));};
case Cyc_Cyclone_Vc_c: _LLAE: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp6DB[2];return(_tmp6DB[1]=rest,((_tmp6DB[0]=Cyc_Absynpp_callconv2doc(_tmpFD),
Cyc_PP_cat(_tag_dyneither(_tmp6DB,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;}
_LLA4: {struct Cyc_Absyn_TypeParams_mod_struct*_tmpFE=(struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpF4;if(_tmpFE->tag != 4)goto _LLA6;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;
_tmp101=_tmpFE->f3;}}_LLA5: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp101){struct Cyc_PP_Doc*_tmp6DC[2];return(_tmp6DC[1]=Cyc_Absynpp_ktvars2doc(
_tmpFF),((_tmp6DC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6DC,sizeof(struct Cyc_PP_Doc*),
2)))));}else{struct Cyc_PP_Doc*_tmp6DD[2];return(_tmp6DD[1]=Cyc_Absynpp_tvars2doc(
_tmpFF),((_tmp6DD[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6DD,sizeof(struct Cyc_PP_Doc*),
2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_struct*_tmp102=(struct Cyc_Absyn_Pointer_mod_struct*)
_tmpF4;if(_tmp102->tag != 2)goto _LL9B;else{_tmp103=_tmp102->f1;_tmp104=_tmp103.rgn;
_tmp105=_tmp103.nullable;_tmp106=_tmp103.bounds;_tmp107=_tmp103.zero_term;
_tmp108=_tmp102->f2;}}_LLA7: {struct Cyc_PP_Doc*ptr;{void*_tmp123=((void*(*)(void*
y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,
_tmp106);struct Cyc_Absyn_Exp*_tmp126;_LLB1: {struct Cyc_Absyn_DynEither_b_struct*
_tmp124=(struct Cyc_Absyn_DynEither_b_struct*)_tmp123;if(_tmp124->tag != 0)goto
_LLB3;}_LLB2: ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_struct*
_tmp125=(struct Cyc_Absyn_Upper_b_struct*)_tmp123;if(_tmp125->tag != 1)goto _LLB0;
else{_tmp126=_tmp125->f1;}}_LLB4:{const char*_tmp6DF;const char*_tmp6DE;ptr=Cyc_PP_text(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp105)?(
_tmp6DF="*",_tag_dyneither(_tmp6DF,sizeof(char),2)):((_tmp6DE="@",_tag_dyneither(
_tmp6DE,sizeof(char),2))));}{unsigned int _tmp12A;int _tmp12B;struct _tuple10
_tmp129=Cyc_Evexp_eval_const_uint_exp(_tmp126);_tmp12A=_tmp129.f1;_tmp12B=
_tmp129.f2;if(!_tmp12B  || _tmp12A != 1){struct Cyc_PP_Doc*_tmp6E0[4];ptr=((_tmp6E0[
3]=Cyc_Absynpp_rb(),((_tmp6E0[2]=Cyc_Absynpp_exp2doc(_tmp126),((_tmp6E0[1]=Cyc_Absynpp_lb(),((
_tmp6E0[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6E0,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}if(Cyc_Absynpp_print_zeroterm){if(!is_char_ptr  && ((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){const char*
_tmp6E3;struct Cyc_PP_Doc*_tmp6E2[2];ptr=((_tmp6E2[1]=Cyc_PP_text(((_tmp6E3="@zeroterm ",
_tag_dyneither(_tmp6E3,sizeof(char),11)))),((_tmp6E2[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6E2,sizeof(struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr
 && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp107)){
const char*_tmp6E6;struct Cyc_PP_Doc*_tmp6E5[2];ptr=((_tmp6E5[1]=Cyc_PP_text(((
_tmp6E6="@nozeroterm ",_tag_dyneither(_tmp6E6,sizeof(char),13)))),((_tmp6E5[0]=
ptr,Cyc_PP_cat(_tag_dyneither(_tmp6E5,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*
_tmp131=Cyc_Tcutil_compress(_tmp104);_LLB6: {struct Cyc_Absyn_HeapRgn_struct*
_tmp132=(struct Cyc_Absyn_HeapRgn_struct*)_tmp131;if(_tmp132->tag != 21)goto _LLB8;}
_LLB7: goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_struct*_tmp133=(struct Cyc_Absyn_Evar_struct*)
_tmp131;if(_tmp133->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto
_LLBA;_LLB9: goto _LLB5;_LLBA:;_LLBB: {const char*_tmp6E9;struct Cyc_PP_Doc*_tmp6E8[
3];ptr=((_tmp6E8[2]=Cyc_PP_text(((_tmp6E9=" ",_tag_dyneither(_tmp6E9,sizeof(char),
2)))),((_tmp6E8[1]=Cyc_Absynpp_typ2doc(_tmp104),((_tmp6E8[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6E8,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}{struct Cyc_PP_Doc*
_tmp6EA[2];ptr=((_tmp6EA[1]=Cyc_Absynpp_tqual2doc(_tmp108),((_tmp6EA[0]=ptr,Cyc_PP_cat(
_tag_dyneither(_tmp6EA,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp6EB[2];return(_tmp6EB[1]=rest,((_tmp6EB[0]=ptr,Cyc_PP_cat(_tag_dyneither(
_tmp6EB,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp13B=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_struct*
_tmp13C=(struct Cyc_Absyn_HeapRgn_struct*)_tmp13B;if(_tmp13C->tag != 21)goto _LLBF;}
_LLBE: {const char*_tmp6EC;return Cyc_PP_text(((_tmp6EC="`H",_tag_dyneither(
_tmp6EC,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_struct*_tmp13D=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp13B;if(_tmp13D->tag != 22)goto _LLC1;}_LLC0: {
const char*_tmp6ED;return Cyc_PP_text(((_tmp6ED="`U",_tag_dyneither(_tmp6ED,
sizeof(char),3))));}_LLC1:;_LLC2: return Cyc_Absynpp_ntyp2doc(t);_LLBC:;}static
void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t){void*_tmp140=Cyc_Tcutil_compress(t);void*_tmp142;struct Cyc_List_List*
_tmp144;_LLC4: {struct Cyc_Absyn_AccessEff_struct*_tmp141=(struct Cyc_Absyn_AccessEff_struct*)
_tmp140;if(_tmp141->tag != 23)goto _LLC6;else{_tmp142=(void*)_tmp141->f1;}}_LLC5:{
struct Cyc_List_List*_tmp6EE;*rgions=((_tmp6EE=_cycalloc(sizeof(*_tmp6EE)),((
_tmp6EE->hd=Cyc_Absynpp_rgn2doc(_tmp142),((_tmp6EE->tl=*rgions,_tmp6EE))))));}
goto _LLC3;_LLC6: {struct Cyc_Absyn_JoinEff_struct*_tmp143=(struct Cyc_Absyn_JoinEff_struct*)
_tmp140;if(_tmp143->tag != 24)goto _LLC8;else{_tmp144=_tmp143->f1;}}_LLC7: for(0;
_tmp144 != 0;_tmp144=_tmp144->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)
_tmp144->hd);}goto _LLC3;_LLC8:;_LLC9:{struct Cyc_List_List*_tmp6EF;*effects=((
_tmp6EF=_cycalloc(sizeof(*_tmp6EF)),((_tmp6EF->hd=Cyc_Absynpp_typ2doc(t),((
_tmp6EF->tl=*effects,_tmp6EF))))));}goto _LLC3;_LLC3:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(
void*t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0  && effects != 0){const char*_tmp6F2;const char*_tmp6F1;const
char*_tmp6F0;return Cyc_PP_group(((_tmp6F0="",_tag_dyneither(_tmp6F0,sizeof(char),
1))),((_tmp6F1="",_tag_dyneither(_tmp6F1,sizeof(char),1))),((_tmp6F2="+",
_tag_dyneither(_tmp6F2,sizeof(char),2))),effects);}else{const char*_tmp6F3;struct
Cyc_PP_Doc*_tmp14A=Cyc_Absynpp_group_braces(((_tmp6F3=",",_tag_dyneither(_tmp6F3,
sizeof(char),2))),rgions);struct Cyc_List_List*_tmp6F7;const char*_tmp6F6;const
char*_tmp6F5;const char*_tmp6F4;return Cyc_PP_group(((_tmp6F4="",_tag_dyneither(
_tmp6F4,sizeof(char),1))),((_tmp6F5="",_tag_dyneither(_tmp6F5,sizeof(char),1))),((
_tmp6F6="+",_tag_dyneither(_tmp6F6,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp6F7=_cycalloc(sizeof(*_tmp6F7)),((_tmp6F7->hd=_tmp14A,((_tmp6F7->tl=0,
_tmp6F7))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind
k){switch(k){case Cyc_Absyn_StructA: _LLCA: {const char*_tmp6F8;return Cyc_PP_text(((
_tmp6F8="struct ",_tag_dyneither(_tmp6F8,sizeof(char),8))));}case Cyc_Absyn_UnionA:
_LLCB: {const char*_tmp6F9;return Cyc_PP_text(((_tmp6F9="union ",_tag_dyneither(
_tmp6F9,sizeof(char),7))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;{void*_tmp152=t;struct Cyc_Core_Opt*_tmp158;struct Cyc_Core_Opt*
_tmp159;int _tmp15A;struct Cyc_Core_Opt*_tmp15B;struct Cyc_Absyn_Tvar*_tmp15D;
struct Cyc_Absyn_DatatypeInfo _tmp15F;union Cyc_Absyn_DatatypeInfoU _tmp160;struct
Cyc_List_List*_tmp161;struct Cyc_Absyn_DatatypeFieldInfo _tmp163;union Cyc_Absyn_DatatypeFieldInfoU
_tmp164;struct Cyc_List_List*_tmp165;enum Cyc_Absyn_Sign _tmp167;enum Cyc_Absyn_Size_of
_tmp168;int _tmp16B;struct Cyc_List_List*_tmp16D;struct Cyc_Absyn_AggrInfo _tmp16F;
union Cyc_Absyn_AggrInfoU _tmp170;struct Cyc_List_List*_tmp171;enum Cyc_Absyn_AggrKind
_tmp173;struct Cyc_List_List*_tmp174;struct Cyc_List_List*_tmp176;struct _tuple0*
_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct _tuple0*_tmp17C;struct Cyc_List_List*
_tmp17D;struct Cyc_Absyn_Typedefdecl*_tmp17E;void*_tmp180;void*_tmp182;void*
_tmp183;void*_tmp185;void*_tmp189;_LLCE: {struct Cyc_Absyn_ArrayType_struct*
_tmp153=(struct Cyc_Absyn_ArrayType_struct*)_tmp152;if(_tmp153->tag != 9)goto _LLD0;}
_LLCF: {const char*_tmp6FA;return Cyc_PP_text(((_tmp6FA="[[[array]]]",
_tag_dyneither(_tmp6FA,sizeof(char),12))));}_LLD0: {struct Cyc_Absyn_FnType_struct*
_tmp154=(struct Cyc_Absyn_FnType_struct*)_tmp152;if(_tmp154->tag != 10)goto _LLD2;}
_LLD1: return Cyc_PP_nil_doc();_LLD2: {struct Cyc_Absyn_PointerType_struct*_tmp155=(
struct Cyc_Absyn_PointerType_struct*)_tmp152;if(_tmp155->tag != 5)goto _LLD4;}_LLD3:
return Cyc_PP_nil_doc();_LLD4: {struct Cyc_Absyn_VoidType_struct*_tmp156=(struct
Cyc_Absyn_VoidType_struct*)_tmp152;if(_tmp156->tag != 0)goto _LLD6;}_LLD5:{const
char*_tmp6FB;s=Cyc_PP_text(((_tmp6FB="void",_tag_dyneither(_tmp6FB,sizeof(char),
5))));}goto _LLCD;_LLD6: {struct Cyc_Absyn_Evar_struct*_tmp157=(struct Cyc_Absyn_Evar_struct*)
_tmp152;if(_tmp157->tag != 1)goto _LLD8;else{_tmp158=_tmp157->f1;_tmp159=_tmp157->f2;
_tmp15A=_tmp157->f3;_tmp15B=_tmp157->f4;}}_LLD7: if(_tmp159 != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp159->v);else{const char*_tmp70B;const char*_tmp70A;const char*_tmp709;
struct Cyc_Int_pa_struct _tmp708;void*_tmp707[1];const char*_tmp706;const char*
_tmp705;struct Cyc_PP_Doc*_tmp704[6];s=((_tmp704[5]=_tmp158 == 0?Cyc_Absynpp_question():
Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp158->v),((_tmp704[4]=Cyc_PP_text(((
_tmp705=")::",_tag_dyneither(_tmp705,sizeof(char),4)))),((_tmp704[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp15B == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp15B->v),((_tmp704[2]=Cyc_PP_text((struct _dyneither_ptr)((_tmp708.tag=1,((
_tmp708.f1=(unsigned long)_tmp15A,((_tmp707[0]=& _tmp708,Cyc_aprintf(((_tmp706="%d",
_tag_dyneither(_tmp706,sizeof(char),3))),_tag_dyneither(_tmp707,sizeof(void*),1))))))))),((
_tmp704[1]=Cyc_PP_text(((_tmp709="(",_tag_dyneither(_tmp709,sizeof(char),2)))),((
_tmp704[0]=Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp70A="\\%",
_tag_dyneither(_tmp70A,sizeof(char),3)))): Cyc_PP_text(((_tmp70B="%",
_tag_dyneither(_tmp70B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp704,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLCD;_LLD8: {struct Cyc_Absyn_VarType_struct*
_tmp15C=(struct Cyc_Absyn_VarType_struct*)_tmp152;if(_tmp15C->tag != 2)goto _LLDA;
else{_tmp15D=_tmp15C->f1;}}_LLD9: s=Cyc_PP_textptr(_tmp15D->name);if(Cyc_Absynpp_print_all_kinds){
const char*_tmp70E;struct Cyc_PP_Doc*_tmp70D[3];s=((_tmp70D[2]=Cyc_Absynpp_kindbound2doc(
_tmp15D->kind),((_tmp70D[1]=Cyc_PP_text(((_tmp70E="::",_tag_dyneither(_tmp70E,
sizeof(char),3)))),((_tmp70D[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp70D,sizeof(
struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(
_tmp15D)){const char*_tmp713;const char*_tmp712;struct Cyc_PP_Doc*_tmp711[3];s=((
_tmp711[2]=Cyc_PP_text(((_tmp712=" */",_tag_dyneither(_tmp712,sizeof(char),4)))),((
_tmp711[1]=s,((_tmp711[0]=Cyc_PP_text(((_tmp713="_ /* ",_tag_dyneither(_tmp713,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp711,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLDA: {struct Cyc_Absyn_DatatypeType_struct*_tmp15E=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp152;if(_tmp15E->tag != 3)goto _LLDC;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15F.datatype_info;
_tmp161=_tmp15F.targs;}}_LLDB:{union Cyc_Absyn_DatatypeInfoU _tmp19B=_tmp160;
struct Cyc_Absyn_UnknownDatatypeInfo _tmp19C;struct _tuple0*_tmp19D;int _tmp19E;
struct Cyc_Absyn_Datatypedecl**_tmp19F;struct Cyc_Absyn_Datatypedecl*_tmp1A0;
struct Cyc_Absyn_Datatypedecl _tmp1A1;struct _tuple0*_tmp1A2;int _tmp1A3;_LL103: if((
_tmp19B.UnknownDatatype).tag != 1)goto _LL105;_tmp19C=(struct Cyc_Absyn_UnknownDatatypeInfo)(
_tmp19B.UnknownDatatype).val;_tmp19D=_tmp19C.name;_tmp19E=_tmp19C.is_extensible;
_LL104: _tmp1A2=_tmp19D;_tmp1A3=_tmp19E;goto _LL106;_LL105: if((_tmp19B.KnownDatatype).tag
!= 2)goto _LL102;_tmp19F=(struct Cyc_Absyn_Datatypedecl**)(_tmp19B.KnownDatatype).val;
_tmp1A0=*_tmp19F;_tmp1A1=*_tmp1A0;_tmp1A2=_tmp1A1.name;_tmp1A3=_tmp1A1.is_extensible;
_LL106: {const char*_tmp714;struct Cyc_PP_Doc*_tmp1A4=Cyc_PP_text(((_tmp714="datatype ",
_tag_dyneither(_tmp714,sizeof(char),10))));const char*_tmp715;struct Cyc_PP_Doc*
_tmp1A5=_tmp1A3?Cyc_PP_text(((_tmp715="@extensible ",_tag_dyneither(_tmp715,
sizeof(char),13)))): Cyc_PP_nil_doc();{struct Cyc_PP_Doc*_tmp716[4];s=((_tmp716[3]=
Cyc_Absynpp_tps2doc(_tmp161),((_tmp716[2]=Cyc_Absynpp_qvar2doc(_tmp1A2),((
_tmp716[1]=_tmp1A4,((_tmp716[0]=_tmp1A5,Cyc_PP_cat(_tag_dyneither(_tmp716,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL102;}_LL102:;}goto _LLCD;_LLDC: {
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp162=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp152;if(_tmp162->tag != 4)goto _LLDE;else{_tmp163=_tmp162->f1;_tmp164=_tmp163.field_info;
_tmp165=_tmp163.targs;}}_LLDD:{union Cyc_Absyn_DatatypeFieldInfoU _tmp1A9=_tmp164;
struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp1AA;struct _tuple0*_tmp1AB;struct
_tuple0*_tmp1AC;int _tmp1AD;struct _tuple1 _tmp1AE;struct Cyc_Absyn_Datatypedecl*
_tmp1AF;struct Cyc_Absyn_Datatypedecl _tmp1B0;struct _tuple0*_tmp1B1;int _tmp1B2;
struct Cyc_Absyn_Datatypefield*_tmp1B3;struct Cyc_Absyn_Datatypefield _tmp1B4;
struct _tuple0*_tmp1B5;_LL108: if((_tmp1A9.UnknownDatatypefield).tag != 1)goto
_LL10A;_tmp1AA=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1A9.UnknownDatatypefield).val;
_tmp1AB=_tmp1AA.datatype_name;_tmp1AC=_tmp1AA.field_name;_tmp1AD=_tmp1AA.is_extensible;
_LL109: _tmp1B1=_tmp1AB;_tmp1B2=_tmp1AD;_tmp1B5=_tmp1AC;goto _LL10B;_LL10A: if((
_tmp1A9.KnownDatatypefield).tag != 2)goto _LL107;_tmp1AE=(struct _tuple1)(_tmp1A9.KnownDatatypefield).val;
_tmp1AF=_tmp1AE.f1;_tmp1B0=*_tmp1AF;_tmp1B1=_tmp1B0.name;_tmp1B2=_tmp1B0.is_extensible;
_tmp1B3=_tmp1AE.f2;_tmp1B4=*_tmp1B3;_tmp1B5=_tmp1B4.name;_LL10B: {const char*
_tmp718;const char*_tmp717;struct Cyc_PP_Doc*_tmp1B6=Cyc_PP_text(_tmp1B2?(_tmp718="@extensible datatype ",
_tag_dyneither(_tmp718,sizeof(char),22)):((_tmp717="datatype ",_tag_dyneither(
_tmp717,sizeof(char),10))));{const char*_tmp71B;struct Cyc_PP_Doc*_tmp71A[4];s=((
_tmp71A[3]=Cyc_Absynpp_qvar2doc(_tmp1B5),((_tmp71A[2]=Cyc_PP_text(((_tmp71B=".",
_tag_dyneither(_tmp71B,sizeof(char),2)))),((_tmp71A[1]=Cyc_Absynpp_qvar2doc(
_tmp1B1),((_tmp71A[0]=_tmp1B6,Cyc_PP_cat(_tag_dyneither(_tmp71A,sizeof(struct Cyc_PP_Doc*),
4))))))))));}goto _LL107;}_LL107:;}goto _LLCD;_LLDE: {struct Cyc_Absyn_IntType_struct*
_tmp166=(struct Cyc_Absyn_IntType_struct*)_tmp152;if(_tmp166->tag != 6)goto _LLE0;
else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_LLDF: {struct _dyneither_ptr sns;
struct _dyneither_ptr ts;switch(_tmp167){case Cyc_Absyn_None: _LL10C: goto _LL10D;case
Cyc_Absyn_Signed: _LL10D:{const char*_tmp71C;sns=((_tmp71C="",_tag_dyneither(
_tmp71C,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL10E:{const char*
_tmp71D;sns=((_tmp71D="unsigned ",_tag_dyneither(_tmp71D,sizeof(char),10)));}
break;}switch(_tmp168){case Cyc_Absyn_Char_sz: _LL110: switch(_tmp167){case Cyc_Absyn_None:
_LL112:{const char*_tmp71E;sns=((_tmp71E="",_tag_dyneither(_tmp71E,sizeof(char),1)));}
break;case Cyc_Absyn_Signed: _LL113:{const char*_tmp71F;sns=((_tmp71F="signed ",
_tag_dyneither(_tmp71F,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL114:{
const char*_tmp720;sns=((_tmp720="unsigned ",_tag_dyneither(_tmp720,sizeof(char),
10)));}break;}{const char*_tmp721;ts=((_tmp721="char",_tag_dyneither(_tmp721,
sizeof(char),5)));}break;case Cyc_Absyn_Short_sz: _LL111:{const char*_tmp722;ts=((
_tmp722="short",_tag_dyneither(_tmp722,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz:
_LL116:{const char*_tmp723;ts=((_tmp723="int",_tag_dyneither(_tmp723,sizeof(char),
4)));}break;case Cyc_Absyn_Long_sz: _LL117:{const char*_tmp724;ts=((_tmp724="long",
_tag_dyneither(_tmp724,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL118:
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL11A:{const char*_tmp725;
ts=((_tmp725="long long",_tag_dyneither(_tmp725,sizeof(char),10)));}break;case
Cyc_Cyclone_Vc_c: _LL11B:{const char*_tmp726;ts=((_tmp726="__int64",_tag_dyneither(
_tmp726,sizeof(char),8)));}break;}break;}{const char*_tmp72B;void*_tmp72A[2];
struct Cyc_String_pa_struct _tmp729;struct Cyc_String_pa_struct _tmp728;s=Cyc_PP_text((
struct _dyneither_ptr)((_tmp728.tag=0,((_tmp728.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ts),((_tmp729.tag=0,((_tmp729.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)sns),((_tmp72A[0]=& _tmp729,((_tmp72A[1]=& _tmp728,Cyc_aprintf(((
_tmp72B="%s%s",_tag_dyneither(_tmp72B,sizeof(char),5))),_tag_dyneither(_tmp72A,
sizeof(void*),2)))))))))))))));}goto _LLCD;}_LLE0: {struct Cyc_Absyn_FloatType_struct*
_tmp169=(struct Cyc_Absyn_FloatType_struct*)_tmp152;if(_tmp169->tag != 7)goto _LLE2;}
_LLE1:{const char*_tmp72C;s=Cyc_PP_text(((_tmp72C="float",_tag_dyneither(_tmp72C,
sizeof(char),6))));}goto _LLCD;_LLE2: {struct Cyc_Absyn_DoubleType_struct*_tmp16A=(
struct Cyc_Absyn_DoubleType_struct*)_tmp152;if(_tmp16A->tag != 8)goto _LLE4;else{
_tmp16B=_tmp16A->f1;}}_LLE3: if(_tmp16B){const char*_tmp72D;s=Cyc_PP_text(((
_tmp72D="long double",_tag_dyneither(_tmp72D,sizeof(char),12))));}else{const char*
_tmp72E;s=Cyc_PP_text(((_tmp72E="double",_tag_dyneither(_tmp72E,sizeof(char),7))));}
goto _LLCD;_LLE4: {struct Cyc_Absyn_TupleType_struct*_tmp16C=(struct Cyc_Absyn_TupleType_struct*)
_tmp152;if(_tmp16C->tag != 11)goto _LLE6;else{_tmp16D=_tmp16C->f1;}}_LLE5:{struct
Cyc_PP_Doc*_tmp72F[2];s=((_tmp72F[1]=Cyc_Absynpp_args2doc(_tmp16D),((_tmp72F[0]=
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp72F,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LLCD;_LLE6: {struct Cyc_Absyn_AggrType_struct*_tmp16E=(struct Cyc_Absyn_AggrType_struct*)
_tmp152;if(_tmp16E->tag != 12)goto _LLE8;else{_tmp16F=_tmp16E->f1;_tmp170=_tmp16F.aggr_info;
_tmp171=_tmp16F.targs;}}_LLE7: {enum Cyc_Absyn_AggrKind _tmp1CF;struct _tuple0*
_tmp1D0;struct _tuple9 _tmp1CE=Cyc_Absyn_aggr_kinded_name(_tmp170);_tmp1CF=_tmp1CE.f1;
_tmp1D0=_tmp1CE.f2;{struct Cyc_PP_Doc*_tmp730[3];s=((_tmp730[2]=Cyc_Absynpp_tps2doc(
_tmp171),((_tmp730[1]=Cyc_Absynpp_qvar2doc(_tmp1D0),((_tmp730[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp1CF),Cyc_PP_cat(_tag_dyneither(_tmp730,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;}_LLE8: {struct Cyc_Absyn_AnonAggrType_struct*_tmp172=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp152;if(_tmp172->tag != 13)goto _LLEA;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}
_LLE9:{struct Cyc_PP_Doc*_tmp731[4];s=((_tmp731[3]=Cyc_Absynpp_rb(),((_tmp731[2]=
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp174)),((_tmp731[1]=Cyc_Absynpp_lb(),((
_tmp731[0]=Cyc_Absynpp_aggr_kind2doc(_tmp173),Cyc_PP_cat(_tag_dyneither(_tmp731,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEA: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp175=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp152;if(_tmp175->tag != 15)goto
_LLEC;else{_tmp176=_tmp175->f1;}}_LLEB:{const char*_tmp734;struct Cyc_PP_Doc*
_tmp733[4];s=((_tmp733[3]=Cyc_Absynpp_rb(),((_tmp733[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp176)),((_tmp733[1]=Cyc_Absynpp_lb(),((_tmp733[0]=Cyc_PP_text(((_tmp734="enum ",
_tag_dyneither(_tmp734,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp733,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLCD;_LLEC: {struct Cyc_Absyn_EnumType_struct*
_tmp177=(struct Cyc_Absyn_EnumType_struct*)_tmp152;if(_tmp177->tag != 14)goto _LLEE;
else{_tmp178=_tmp177->f1;}}_LLED:{const char*_tmp737;struct Cyc_PP_Doc*_tmp736[2];
s=((_tmp736[1]=Cyc_Absynpp_qvar2doc(_tmp178),((_tmp736[0]=Cyc_PP_text(((_tmp737="enum ",
_tag_dyneither(_tmp737,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp736,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLCD;_LLEE: {struct Cyc_Absyn_ValueofType_struct*
_tmp179=(struct Cyc_Absyn_ValueofType_struct*)_tmp152;if(_tmp179->tag != 19)goto
_LLF0;else{_tmp17A=_tmp179->f1;}}_LLEF:{const char*_tmp73C;const char*_tmp73B;
struct Cyc_PP_Doc*_tmp73A[3];s=((_tmp73A[2]=Cyc_PP_text(((_tmp73B=")",
_tag_dyneither(_tmp73B,sizeof(char),2)))),((_tmp73A[1]=Cyc_Absynpp_exp2doc(
_tmp17A),((_tmp73A[0]=Cyc_PP_text(((_tmp73C="valueof_t(",_tag_dyneither(_tmp73C,
sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp73A,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LLCD;_LLF0: {struct Cyc_Absyn_TypedefType_struct*_tmp17B=(struct
Cyc_Absyn_TypedefType_struct*)_tmp152;if(_tmp17B->tag != 18)goto _LLF2;else{
_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;_tmp17E=_tmp17B->f3;}}_LLF1:{struct Cyc_PP_Doc*
_tmp73D[2];s=((_tmp73D[1]=Cyc_Absynpp_tps2doc(_tmp17D),((_tmp73D[0]=Cyc_Absynpp_qvar2doc(
_tmp17C),Cyc_PP_cat(_tag_dyneither(_tmp73D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLCD;_LLF2: {struct Cyc_Absyn_RgnHandleType_struct*_tmp17F=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp152;if(_tmp17F->tag != 16)goto _LLF4;else{_tmp180=(void*)_tmp17F->f1;}}_LLF3:{
const char*_tmp742;const char*_tmp741;struct Cyc_PP_Doc*_tmp740[3];s=((_tmp740[2]=
Cyc_PP_text(((_tmp741=">",_tag_dyneither(_tmp741,sizeof(char),2)))),((_tmp740[1]=
Cyc_Absynpp_rgn2doc(_tmp180),((_tmp740[0]=Cyc_PP_text(((_tmp742="region_t<",
_tag_dyneither(_tmp742,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp740,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF4: {struct Cyc_Absyn_DynRgnType_struct*
_tmp181=(struct Cyc_Absyn_DynRgnType_struct*)_tmp152;if(_tmp181->tag != 17)goto
_LLF6;else{_tmp182=(void*)_tmp181->f1;_tmp183=(void*)_tmp181->f2;}}_LLF5:{const
char*_tmp749;const char*_tmp748;const char*_tmp747;struct Cyc_PP_Doc*_tmp746[5];s=((
_tmp746[4]=Cyc_PP_text(((_tmp747=">",_tag_dyneither(_tmp747,sizeof(char),2)))),((
_tmp746[3]=Cyc_Absynpp_rgn2doc(_tmp183),((_tmp746[2]=Cyc_PP_text(((_tmp748=",",
_tag_dyneither(_tmp748,sizeof(char),2)))),((_tmp746[1]=Cyc_Absynpp_rgn2doc(
_tmp182),((_tmp746[0]=Cyc_PP_text(((_tmp749="dynregion_t<",_tag_dyneither(
_tmp749,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp746,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LLCD;_LLF6: {struct Cyc_Absyn_TagType_struct*_tmp184=(struct
Cyc_Absyn_TagType_struct*)_tmp152;if(_tmp184->tag != 20)goto _LLF8;else{_tmp185=(
void*)_tmp184->f1;}}_LLF7:{const char*_tmp74E;const char*_tmp74D;struct Cyc_PP_Doc*
_tmp74C[3];s=((_tmp74C[2]=Cyc_PP_text(((_tmp74D=">",_tag_dyneither(_tmp74D,
sizeof(char),2)))),((_tmp74C[1]=Cyc_Absynpp_typ2doc(_tmp185),((_tmp74C[0]=Cyc_PP_text(((
_tmp74E="tag_t<",_tag_dyneither(_tmp74E,sizeof(char),7)))),Cyc_PP_cat(
_tag_dyneither(_tmp74C,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLCD;_LLF8: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp186=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp152;if(_tmp186->tag != 22)goto _LLFA;}_LLF9: goto _LLFB;_LLFA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp187=(struct Cyc_Absyn_HeapRgn_struct*)_tmp152;if(_tmp187->tag != 21)goto _LLFC;}
_LLFB: s=Cyc_Absynpp_rgn2doc(t);goto _LLCD;_LLFC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp188=(struct Cyc_Absyn_RgnsEff_struct*)_tmp152;if(_tmp188->tag != 25)goto _LLFE;
else{_tmp189=(void*)_tmp188->f1;}}_LLFD:{const char*_tmp753;const char*_tmp752;
struct Cyc_PP_Doc*_tmp751[3];s=((_tmp751[2]=Cyc_PP_text(((_tmp752=")",
_tag_dyneither(_tmp752,sizeof(char),2)))),((_tmp751[1]=Cyc_Absynpp_typ2doc(
_tmp189),((_tmp751[0]=Cyc_PP_text(((_tmp753="regions(",_tag_dyneither(_tmp753,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp751,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLCD;_LLFE: {struct Cyc_Absyn_AccessEff_struct*_tmp18A=(struct Cyc_Absyn_AccessEff_struct*)
_tmp152;if(_tmp18A->tag != 23)goto _LL100;}_LLFF: goto _LL101;_LL100: {struct Cyc_Absyn_JoinEff_struct*
_tmp18B=(struct Cyc_Absyn_JoinEff_struct*)_tmp152;if(_tmp18B->tag != 24)goto _LLCD;}
_LL101: s=Cyc_Absynpp_eff2doc(t);goto _LLCD;_LLCD:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_dyneither_ptr*)vo->v));}struct _tuple16{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple16*cmp){struct _tuple16 _tmp1E9;void*_tmp1EA;void*_tmp1EB;struct
_tuple16*_tmp1E8=cmp;_tmp1E9=*_tmp1E8;_tmp1EA=_tmp1E9.f1;_tmp1EB=_tmp1E9.f2;{
const char*_tmp756;struct Cyc_PP_Doc*_tmp755[3];return(_tmp755[2]=Cyc_Absynpp_rgn2doc(
_tmp1EB),((_tmp755[1]=Cyc_PP_text(((_tmp756=" > ",_tag_dyneither(_tmp756,sizeof(
char),4)))),((_tmp755[0]=Cyc_Absynpp_rgn2doc(_tmp1EA),Cyc_PP_cat(_tag_dyneither(
_tmp755,sizeof(struct Cyc_PP_Doc*),3)))))));};}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){const char*_tmp759;const char*_tmp758;const char*_tmp757;
return Cyc_PP_group(((_tmp757="",_tag_dyneither(_tmp757,sizeof(char),1))),((
_tmp758="",_tag_dyneither(_tmp758,sizeof(char),1))),((_tmp759=",",_tag_dyneither(
_tmp759,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple7*t){struct Cyc_Core_Opt*
_tmp75A;struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:((_tmp75A=_cycalloc(sizeof(*
_tmp75A)),((_tmp75A->v=Cyc_PP_text(*((struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null((*t).f1))->v)),_tmp75A))));return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,
dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,
struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp1F2=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){const char*_tmp75D;struct Cyc_List_List*
_tmp75C;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1F2,((_tmp75C=_cycalloc(sizeof(*_tmp75C)),((_tmp75C->hd=
Cyc_PP_text(((_tmp75D="...",_tag_dyneither(_tmp75D,sizeof(char),4)))),((_tmp75C->tl=
0,_tmp75C)))))));}else{if(cyc_varargs != 0){const char*_tmp766;const char*_tmp765;
const char*_tmp764;struct _tuple7*_tmp763;struct Cyc_PP_Doc*_tmp762[3];struct Cyc_PP_Doc*
_tmp1F5=(_tmp762[2]=Cyc_Absynpp_funarg2doc(((_tmp763=_cycalloc(sizeof(*_tmp763)),((
_tmp763->f1=cyc_varargs->name,((_tmp763->f2=cyc_varargs->tq,((_tmp763->f3=
cyc_varargs->type,_tmp763))))))))),((_tmp762[1]=cyc_varargs->inject?Cyc_PP_text(((
_tmp764=" inject ",_tag_dyneither(_tmp764,sizeof(char),9)))): Cyc_PP_text(((
_tmp765=" ",_tag_dyneither(_tmp765,sizeof(char),2)))),((_tmp762[0]=Cyc_PP_text(((
_tmp766="...",_tag_dyneither(_tmp766,sizeof(char),4)))),Cyc_PP_cat(
_tag_dyneither(_tmp762,sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*
_tmp767;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp1F2,((_tmp767=_cycalloc(sizeof(*_tmp767)),((_tmp767->hd=
_tmp1F5,((_tmp767->tl=0,_tmp767)))))));}}{const char*_tmp76A;const char*_tmp769;
const char*_tmp768;struct Cyc_PP_Doc*_tmp1FC=Cyc_PP_group(((_tmp768="",
_tag_dyneither(_tmp768,sizeof(char),1))),((_tmp769="",_tag_dyneither(_tmp769,
sizeof(char),1))),((_tmp76A=",",_tag_dyneither(_tmp76A,sizeof(char),2))),_tmp1F2);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){const char*_tmp76D;struct Cyc_PP_Doc*
_tmp76C[3];_tmp1FC=((_tmp76C[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp76C[
1]=Cyc_PP_text(((_tmp76D=";",_tag_dyneither(_tmp76D,sizeof(char),2)))),((_tmp76C[
0]=_tmp1FC,Cyc_PP_cat(_tag_dyneither(_tmp76C,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){const char*_tmp770;struct Cyc_PP_Doc*_tmp76F[3];_tmp1FC=((_tmp76F[2]=
Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp76F[1]=Cyc_PP_text(((_tmp770=":",
_tag_dyneither(_tmp770,sizeof(char),2)))),((_tmp76F[0]=_tmp1FC,Cyc_PP_cat(
_tag_dyneither(_tmp76F,sizeof(struct Cyc_PP_Doc*),3))))))));}{const char*_tmp775;
const char*_tmp774;struct Cyc_PP_Doc*_tmp773[3];return(_tmp773[2]=Cyc_PP_text(((
_tmp774=")",_tag_dyneither(_tmp774,sizeof(char),2)))),((_tmp773[1]=_tmp1FC,((
_tmp773[0]=Cyc_PP_text(((_tmp775="(",_tag_dyneither(_tmp775,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp773,sizeof(struct Cyc_PP_Doc*),3)))))));};};}struct
_tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple11*arg){struct Cyc_Core_Opt*_tmp778;
struct _tuple7*_tmp777;return(_tmp777=_cycalloc(sizeof(*_tmp777)),((_tmp777->f1=((
_tmp778=_cycalloc(sizeof(*_tmp778)),((_tmp778->v=(*arg).f1,_tmp778)))),((_tmp777->f2=(*
arg).f2,((_tmp777->f3=(*arg).f3,_tmp777)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(
struct _dyneither_ptr*v){return Cyc_PP_text(*v);}struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple0*q){struct Cyc_List_List*_tmp209=0;int match;{union Cyc_Absyn_Nmspace
_tmp20A=(*q).f1;int _tmp20B;struct Cyc_List_List*_tmp20C;struct Cyc_List_List*
_tmp20D;struct Cyc_List_List*_tmp20E;_LL11E: if((_tmp20A.Loc_n).tag != 4)goto _LL120;
_tmp20B=(int)(_tmp20A.Loc_n).val;_LL11F: _tmp20C=0;goto _LL121;_LL120: if((_tmp20A.Rel_n).tag
!= 1)goto _LL122;_tmp20C=(struct Cyc_List_List*)(_tmp20A.Rel_n).val;_LL121: match=0;
_tmp209=_tmp20C;goto _LL11D;_LL122: if((_tmp20A.C_n).tag != 3)goto _LL124;_tmp20D=(
struct Cyc_List_List*)(_tmp20A.C_n).val;_LL123: match=Cyc_Absynpp_use_curr_namespace
 && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20D,Cyc_Absynpp_curr_namespace);
goto _LL11D;_LL124: if((_tmp20A.Abs_n).tag != 2)goto _LL11D;_tmp20E=(struct Cyc_List_List*)(
_tmp20A.Abs_n).val;_LL125: match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20E,Cyc_Absynpp_curr_namespace);{
struct Cyc_List_List*_tmp779;_tmp209=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(
_tmp779=_cycalloc(sizeof(*_tmp779)),((_tmp779->hd=Cyc_Absynpp_cyc_stringptr,((
_tmp779->tl=_tmp20E,_tmp779))))): _tmp20E;}goto _LL11D;_LL11D:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp77B;struct Cyc_List_List*_tmp77A;return(struct _dyneither_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp209,((_tmp77A=_cycalloc(sizeof(*_tmp77A)),((_tmp77A->hd=(*q).f2,((_tmp77A->tl=
0,_tmp77A))))))),((_tmp77B="_",_tag_dyneither(_tmp77B,sizeof(char),2))));}else{
if(match)return*(*q).f2;else{const char*_tmp77D;struct Cyc_List_List*_tmp77C;
return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp209,((_tmp77C=_cycalloc(sizeof(*
_tmp77C)),((_tmp77C->hd=(*q).f2,((_tmp77C->tl=0,_tmp77C))))))),((_tmp77D="::",
_tag_dyneither(_tmp77D,sizeof(char),3))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){struct _dyneither_ptr _tmp214=Cyc_Absynpp_qvar2string(
q);if(Cyc_PP_tex_output){const char*_tmp77F;const char*_tmp77E;return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp77E="\\textbf{",
_tag_dyneither(_tmp77E,sizeof(char),9))),(struct _dyneither_ptr)_tmp214),((
_tmp77F="}",_tag_dyneither(_tmp77F,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp214));}else{return Cyc_PP_text(_tmp214);}}struct _dyneither_ptr
Cyc_Absynpp_typedef_name2string(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)
return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){union Cyc_Absyn_Nmspace
_tmp217=(*v).f1;int _tmp218;struct Cyc_List_List*_tmp219;struct Cyc_List_List*
_tmp21A;struct Cyc_List_List*_tmp21B;_LL127: if((_tmp217.Loc_n).tag != 4)goto _LL129;
_tmp218=(int)(_tmp217.Loc_n).val;_LL128: goto _LL12A;_LL129: if((_tmp217.Rel_n).tag
!= 1)goto _LL12B;_tmp219=(struct Cyc_List_List*)(_tmp217.Rel_n).val;if(_tmp219 != 0)
goto _LL12B;_LL12A: return*(*v).f2;_LL12B: if((_tmp217.C_n).tag != 3)goto _LL12D;
_tmp21A=(struct Cyc_List_List*)(_tmp217.C_n).val;_LL12C: _tmp21B=_tmp21A;goto
_LL12E;_LL12D: if((_tmp217.Abs_n).tag != 2)goto _LL12F;_tmp21B=(struct Cyc_List_List*)(
_tmp217.Abs_n).val;_LL12E: if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_strptrcmp,_tmp21B,Cyc_Absynpp_curr_namespace)== 0)return*(*v).f2;else{goto
_LL130;}_LL12F:;_LL130: {const char*_tmp780;return(struct _dyneither_ptr)Cyc_strconcat(((
_tmp780="/* bad namespace : */ ",_tag_dyneither(_tmp780,sizeof(char),23))),(
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL126:;}else{return*(*v).f2;}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(
Cyc_Absynpp_typedef_name2string(v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(
struct _tuple0*v){struct _dyneither_ptr _tmp21D=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){const char*_tmp782;const char*_tmp781;return Cyc_PP_text_width((
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp781="\\textbf{",
_tag_dyneither(_tmp781,sizeof(char),9))),(struct _dyneither_ptr)_tmp21D),((
_tmp782="}",_tag_dyneither(_tmp782,sizeof(char),2)))),(int)Cyc_strlen((struct
_dyneither_ptr)_tmp21D));}else{return Cyc_PP_text(_tmp21D);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmp220=e->r;enum Cyc_Absyn_Primop _tmp225;struct Cyc_Absyn_Exp*
_tmp230;struct Cyc_Absyn_Exp*_tmp232;_LL132: {struct Cyc_Absyn_Const_e_struct*
_tmp221=(struct Cyc_Absyn_Const_e_struct*)_tmp220;if(_tmp221->tag != 0)goto _LL134;}
_LL133: goto _LL135;_LL134: {struct Cyc_Absyn_Var_e_struct*_tmp222=(struct Cyc_Absyn_Var_e_struct*)
_tmp220;if(_tmp222->tag != 1)goto _LL136;}_LL135: goto _LL137;_LL136: {struct Cyc_Absyn_UnknownId_e_struct*
_tmp223=(struct Cyc_Absyn_UnknownId_e_struct*)_tmp220;if(_tmp223->tag != 2)goto
_LL138;}_LL137: return 10000;_LL138: {struct Cyc_Absyn_Primop_e_struct*_tmp224=(
struct Cyc_Absyn_Primop_e_struct*)_tmp220;if(_tmp224->tag != 3)goto _LL13A;else{
_tmp225=_tmp224->f1;}}_LL139: switch(_tmp225){case Cyc_Absyn_Plus: _LL184: return 100;
case Cyc_Absyn_Times: _LL185: return 110;case Cyc_Absyn_Minus: _LL186: return 100;case
Cyc_Absyn_Div: _LL187: goto _LL188;case Cyc_Absyn_Mod: _LL188: return 110;case Cyc_Absyn_Eq:
_LL189: goto _LL18A;case Cyc_Absyn_Neq: _LL18A: return 70;case Cyc_Absyn_Gt: _LL18B: goto
_LL18C;case Cyc_Absyn_Lt: _LL18C: goto _LL18D;case Cyc_Absyn_Gte: _LL18D: goto _LL18E;
case Cyc_Absyn_Lte: _LL18E: return 80;case Cyc_Absyn_Not: _LL18F: goto _LL190;case Cyc_Absyn_Bitnot:
_LL190: return 130;case Cyc_Absyn_Bitand: _LL191: return 60;case Cyc_Absyn_Bitor: _LL192:
return 40;case Cyc_Absyn_Bitxor: _LL193: return 50;case Cyc_Absyn_Bitlshift: _LL194:
return 90;case Cyc_Absyn_Bitlrshift: _LL195: return 80;case Cyc_Absyn_Bitarshift:
_LL196: return 80;case Cyc_Absyn_Numelts: _LL197: return 140;}_LL13A: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp226=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp220;if(_tmp226->tag != 4)goto
_LL13C;}_LL13B: return 20;_LL13C: {struct Cyc_Absyn_Increment_e_struct*_tmp227=(
struct Cyc_Absyn_Increment_e_struct*)_tmp220;if(_tmp227->tag != 5)goto _LL13E;}
_LL13D: return 130;_LL13E: {struct Cyc_Absyn_Conditional_e_struct*_tmp228=(struct
Cyc_Absyn_Conditional_e_struct*)_tmp220;if(_tmp228->tag != 6)goto _LL140;}_LL13F:
return 30;_LL140: {struct Cyc_Absyn_And_e_struct*_tmp229=(struct Cyc_Absyn_And_e_struct*)
_tmp220;if(_tmp229->tag != 7)goto _LL142;}_LL141: return 35;_LL142: {struct Cyc_Absyn_Or_e_struct*
_tmp22A=(struct Cyc_Absyn_Or_e_struct*)_tmp220;if(_tmp22A->tag != 8)goto _LL144;}
_LL143: return 30;_LL144: {struct Cyc_Absyn_SeqExp_e_struct*_tmp22B=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp220;if(_tmp22B->tag != 9)goto _LL146;}_LL145: return 10;_LL146: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmp22C=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmp220;if(_tmp22C->tag != 10)goto
_LL148;}_LL147: goto _LL149;_LL148: {struct Cyc_Absyn_FnCall_e_struct*_tmp22D=(
struct Cyc_Absyn_FnCall_e_struct*)_tmp220;if(_tmp22D->tag != 11)goto _LL14A;}_LL149:
return 140;_LL14A: {struct Cyc_Absyn_Throw_e_struct*_tmp22E=(struct Cyc_Absyn_Throw_e_struct*)
_tmp220;if(_tmp22E->tag != 12)goto _LL14C;}_LL14B: return 130;_LL14C: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmp22F=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp220;if(_tmp22F->tag != 13)
goto _LL14E;else{_tmp230=_tmp22F->f1;}}_LL14D: return Cyc_Absynpp_exp_prec(_tmp230);
_LL14E: {struct Cyc_Absyn_Instantiate_e_struct*_tmp231=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp220;if(_tmp231->tag != 14)goto _LL150;else{_tmp232=_tmp231->f1;}}_LL14F: return
Cyc_Absynpp_exp_prec(_tmp232);_LL150: {struct Cyc_Absyn_Cast_e_struct*_tmp233=(
struct Cyc_Absyn_Cast_e_struct*)_tmp220;if(_tmp233->tag != 15)goto _LL152;}_LL151:
return 120;_LL152: {struct Cyc_Absyn_New_e_struct*_tmp234=(struct Cyc_Absyn_New_e_struct*)
_tmp220;if(_tmp234->tag != 17)goto _LL154;}_LL153: return 15;_LL154: {struct Cyc_Absyn_Address_e_struct*
_tmp235=(struct Cyc_Absyn_Address_e_struct*)_tmp220;if(_tmp235->tag != 16)goto
_LL156;}_LL155: goto _LL157;_LL156: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp236=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp220;if(_tmp236->tag != 18)goto _LL158;}
_LL157: goto _LL159;_LL158: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmp237=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp220;if(_tmp237->tag != 19)goto _LL15A;}_LL159: goto _LL15B;_LL15A: {struct Cyc_Absyn_Valueof_e_struct*
_tmp238=(struct Cyc_Absyn_Valueof_e_struct*)_tmp220;if(_tmp238->tag != 39)goto
_LL15C;}_LL15B: goto _LL15D;_LL15C: {struct Cyc_Absyn_Asm_e_struct*_tmp239=(struct
Cyc_Absyn_Asm_e_struct*)_tmp220;if(_tmp239->tag != 40)goto _LL15E;}_LL15D: goto
_LL15F;_LL15E: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp23A=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp220;if(_tmp23A->tag != 38)goto _LL160;}_LL15F: goto _LL161;_LL160: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp23B=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp220;if(_tmp23B->tag != 20)goto
_LL162;}_LL161: goto _LL163;_LL162: {struct Cyc_Absyn_Deref_e_struct*_tmp23C=(
struct Cyc_Absyn_Deref_e_struct*)_tmp220;if(_tmp23C->tag != 21)goto _LL164;}_LL163:
return 130;_LL164: {struct Cyc_Absyn_AggrMember_e_struct*_tmp23D=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmp220;if(_tmp23D->tag != 22)goto _LL166;}_LL165: goto _LL167;_LL166: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp23E=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp220;if(_tmp23E->tag != 23)goto
_LL168;}_LL167: goto _LL169;_LL168: {struct Cyc_Absyn_Subscript_e_struct*_tmp23F=(
struct Cyc_Absyn_Subscript_e_struct*)_tmp220;if(_tmp23F->tag != 24)goto _LL16A;}
_LL169: return 140;_LL16A: {struct Cyc_Absyn_Tuple_e_struct*_tmp240=(struct Cyc_Absyn_Tuple_e_struct*)
_tmp220;if(_tmp240->tag != 25)goto _LL16C;}_LL16B: return 150;_LL16C: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp241=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp220;if(_tmp241->tag != 26)goto
_LL16E;}_LL16D: goto _LL16F;_LL16E: {struct Cyc_Absyn_Array_e_struct*_tmp242=(
struct Cyc_Absyn_Array_e_struct*)_tmp220;if(_tmp242->tag != 27)goto _LL170;}_LL16F:
goto _LL171;_LL170: {struct Cyc_Absyn_Comprehension_e_struct*_tmp243=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp220;if(_tmp243->tag != 28)goto _LL172;}_LL171: goto _LL173;_LL172: {struct Cyc_Absyn_Aggregate_e_struct*
_tmp244=(struct Cyc_Absyn_Aggregate_e_struct*)_tmp220;if(_tmp244->tag != 29)goto
_LL174;}_LL173: goto _LL175;_LL174: {struct Cyc_Absyn_AnonStruct_e_struct*_tmp245=(
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp220;if(_tmp245->tag != 30)goto _LL176;}
_LL175: goto _LL177;_LL176: {struct Cyc_Absyn_Datatype_e_struct*_tmp246=(struct Cyc_Absyn_Datatype_e_struct*)
_tmp220;if(_tmp246->tag != 31)goto _LL178;}_LL177: goto _LL179;_LL178: {struct Cyc_Absyn_Enum_e_struct*
_tmp247=(struct Cyc_Absyn_Enum_e_struct*)_tmp220;if(_tmp247->tag != 32)goto _LL17A;}
_LL179: goto _LL17B;_LL17A: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp248=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp220;if(_tmp248->tag != 33)goto _LL17C;}_LL17B: goto _LL17D;_LL17C: {struct Cyc_Absyn_Malloc_e_struct*
_tmp249=(struct Cyc_Absyn_Malloc_e_struct*)_tmp220;if(_tmp249->tag != 34)goto
_LL17E;}_LL17D: goto _LL17F;_LL17E: {struct Cyc_Absyn_Swap_e_struct*_tmp24A=(struct
Cyc_Absyn_Swap_e_struct*)_tmp220;if(_tmp24A->tag != 35)goto _LL180;}_LL17F: goto
_LL181;_LL180: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp24B=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp220;if(_tmp24B->tag != 36)goto _LL182;}_LL181: return 140;_LL182: {struct Cyc_Absyn_StmtExp_e_struct*
_tmp24C=(struct Cyc_Absyn_StmtExp_e_struct*)_tmp220;if(_tmp24C->tag != 37)goto
_LL131;}_LL183: return 10000;_LL131:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct
Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp24D=e->r;union Cyc_Absyn_Cnst _tmp24F;struct _tuple0*_tmp251;struct
_tuple0*_tmp253;enum Cyc_Absyn_Primop _tmp255;struct Cyc_List_List*_tmp256;struct
Cyc_Absyn_Exp*_tmp258;struct Cyc_Core_Opt*_tmp259;struct Cyc_Absyn_Exp*_tmp25A;
struct Cyc_Absyn_Exp*_tmp25C;enum Cyc_Absyn_Incrementor _tmp25D;struct Cyc_Absyn_Exp*
_tmp25F;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*
_tmp263;struct Cyc_Absyn_Exp*_tmp264;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_Absyn_Exp*
_tmp267;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Exp*_tmp26A;struct Cyc_Absyn_Exp*
_tmp26C;struct Cyc_List_List*_tmp26D;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_List_List*
_tmp270;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*
_tmp276;void*_tmp278;struct Cyc_Absyn_Exp*_tmp279;struct Cyc_Absyn_Exp*_tmp27B;
struct Cyc_Absyn_Exp*_tmp27D;struct Cyc_Absyn_Exp*_tmp27E;void*_tmp280;struct Cyc_Absyn_Exp*
_tmp282;void*_tmp284;int _tmp286;struct _dyneither_ptr _tmp287;struct Cyc_Absyn_Exp*
_tmp289;struct _dyneither_ptr*_tmp28A;void*_tmp28C;void*_tmp28D;struct
_dyneither_ptr*_tmp28F;void*_tmp291;void*_tmp292;unsigned int _tmp294;struct Cyc_Absyn_Exp*
_tmp296;struct Cyc_Absyn_Exp*_tmp298;struct _dyneither_ptr*_tmp299;struct Cyc_Absyn_Exp*
_tmp29B;struct _dyneither_ptr*_tmp29C;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*
_tmp29F;struct Cyc_List_List*_tmp2A1;struct _tuple7*_tmp2A3;struct Cyc_List_List*
_tmp2A4;struct Cyc_List_List*_tmp2A6;struct Cyc_Absyn_Vardecl*_tmp2A8;struct Cyc_Absyn_Exp*
_tmp2A9;struct Cyc_Absyn_Exp*_tmp2AA;struct _tuple0*_tmp2AC;struct Cyc_List_List*
_tmp2AD;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List*_tmp2B0;struct Cyc_List_List*
_tmp2B2;struct Cyc_Absyn_Datatypefield*_tmp2B3;struct _tuple0*_tmp2B5;struct
_tuple0*_tmp2B7;struct Cyc_Absyn_MallocInfo _tmp2B9;int _tmp2BA;struct Cyc_Absyn_Exp*
_tmp2BB;void**_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BF;
struct Cyc_Absyn_Exp*_tmp2C0;struct Cyc_Core_Opt*_tmp2C2;struct Cyc_List_List*
_tmp2C3;struct Cyc_Absyn_Stmt*_tmp2C5;_LL19A: {struct Cyc_Absyn_Const_e_struct*
_tmp24E=(struct Cyc_Absyn_Const_e_struct*)_tmp24D;if(_tmp24E->tag != 0)goto _LL19C;
else{_tmp24F=_tmp24E->f1;}}_LL19B: s=Cyc_Absynpp_cnst2doc(_tmp24F);goto _LL199;
_LL19C: {struct Cyc_Absyn_Var_e_struct*_tmp250=(struct Cyc_Absyn_Var_e_struct*)
_tmp24D;if(_tmp250->tag != 1)goto _LL19E;else{_tmp251=_tmp250->f1;}}_LL19D: _tmp253=
_tmp251;goto _LL19F;_LL19E: {struct Cyc_Absyn_UnknownId_e_struct*_tmp252=(struct
Cyc_Absyn_UnknownId_e_struct*)_tmp24D;if(_tmp252->tag != 2)goto _LL1A0;else{
_tmp253=_tmp252->f1;}}_LL19F: s=Cyc_Absynpp_qvar2doc(_tmp253);goto _LL199;_LL1A0: {
struct Cyc_Absyn_Primop_e_struct*_tmp254=(struct Cyc_Absyn_Primop_e_struct*)
_tmp24D;if(_tmp254->tag != 3)goto _LL1A2;else{_tmp255=_tmp254->f1;_tmp256=_tmp254->f2;}}
_LL1A1: s=Cyc_Absynpp_primapp2doc(myprec,_tmp255,_tmp256);goto _LL199;_LL1A2: {
struct Cyc_Absyn_AssignOp_e_struct*_tmp257=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmp24D;if(_tmp257->tag != 4)goto _LL1A4;else{_tmp258=_tmp257->f1;_tmp259=_tmp257->f2;
_tmp25A=_tmp257->f3;}}_LL1A3:{const char*_tmp787;const char*_tmp786;struct Cyc_PP_Doc*
_tmp785[5];s=((_tmp785[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25A),((_tmp785[3]=
Cyc_PP_text(((_tmp786="= ",_tag_dyneither(_tmp786,sizeof(char),3)))),((_tmp785[2]=
_tmp259 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp259->v),((
_tmp785[1]=Cyc_PP_text(((_tmp787=" ",_tag_dyneither(_tmp787,sizeof(char),2)))),((
_tmp785[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp258),Cyc_PP_cat(_tag_dyneither(
_tmp785,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL199;_LL1A4: {struct Cyc_Absyn_Increment_e_struct*
_tmp25B=(struct Cyc_Absyn_Increment_e_struct*)_tmp24D;if(_tmp25B->tag != 5)goto
_LL1A6;else{_tmp25C=_tmp25B->f1;_tmp25D=_tmp25B->f2;}}_LL1A5: {struct Cyc_PP_Doc*
_tmp2C9=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25C);switch(_tmp25D){case Cyc_Absyn_PreInc:
_LL1EE:{const char*_tmp78A;struct Cyc_PP_Doc*_tmp789[2];s=((_tmp789[1]=_tmp2C9,((
_tmp789[0]=Cyc_PP_text(((_tmp78A="++",_tag_dyneither(_tmp78A,sizeof(char),3)))),
Cyc_PP_cat(_tag_dyneither(_tmp789,sizeof(struct Cyc_PP_Doc*),2))))));}break;case
Cyc_Absyn_PreDec: _LL1EF:{const char*_tmp78D;struct Cyc_PP_Doc*_tmp78C[2];s=((
_tmp78C[1]=_tmp2C9,((_tmp78C[0]=Cyc_PP_text(((_tmp78D="--",_tag_dyneither(
_tmp78D,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp78C,sizeof(struct Cyc_PP_Doc*),
2))))));}break;case Cyc_Absyn_PostInc: _LL1F0:{const char*_tmp790;struct Cyc_PP_Doc*
_tmp78F[2];s=((_tmp78F[1]=Cyc_PP_text(((_tmp790="++",_tag_dyneither(_tmp790,
sizeof(char),3)))),((_tmp78F[0]=_tmp2C9,Cyc_PP_cat(_tag_dyneither(_tmp78F,
sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1F1:{const char*
_tmp793;struct Cyc_PP_Doc*_tmp792[2];s=((_tmp792[1]=Cyc_PP_text(((_tmp793="--",
_tag_dyneither(_tmp793,sizeof(char),3)))),((_tmp792[0]=_tmp2C9,Cyc_PP_cat(
_tag_dyneither(_tmp792,sizeof(struct Cyc_PP_Doc*),2))))));}break;}goto _LL199;}
_LL1A6: {struct Cyc_Absyn_Conditional_e_struct*_tmp25E=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp24D;if(_tmp25E->tag != 6)goto _LL1A8;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25E->f2;
_tmp261=_tmp25E->f3;}}_LL1A7:{const char*_tmp798;const char*_tmp797;struct Cyc_PP_Doc*
_tmp796[5];s=((_tmp796[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp261),((_tmp796[3]=
Cyc_PP_text(((_tmp797=" : ",_tag_dyneither(_tmp797,sizeof(char),4)))),((_tmp796[
2]=Cyc_Absynpp_exp2doc_prec(0,_tmp260),((_tmp796[1]=Cyc_PP_text(((_tmp798=" ? ",
_tag_dyneither(_tmp798,sizeof(char),4)))),((_tmp796[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp25F),Cyc_PP_cat(_tag_dyneither(_tmp796,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL199;_LL1A8: {struct Cyc_Absyn_And_e_struct*_tmp262=(struct Cyc_Absyn_And_e_struct*)
_tmp24D;if(_tmp262->tag != 7)goto _LL1AA;else{_tmp263=_tmp262->f1;_tmp264=_tmp262->f2;}}
_LL1A9:{const char*_tmp79B;struct Cyc_PP_Doc*_tmp79A[3];s=((_tmp79A[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp264),((_tmp79A[1]=Cyc_PP_text(((_tmp79B=" && ",_tag_dyneither(_tmp79B,
sizeof(char),5)))),((_tmp79A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp263),Cyc_PP_cat(
_tag_dyneither(_tmp79A,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL199;_LL1AA: {
struct Cyc_Absyn_Or_e_struct*_tmp265=(struct Cyc_Absyn_Or_e_struct*)_tmp24D;if(
_tmp265->tag != 8)goto _LL1AC;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;}}
_LL1AB:{const char*_tmp79E;struct Cyc_PP_Doc*_tmp79D[3];s=((_tmp79D[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp267),((_tmp79D[1]=Cyc_PP_text(((_tmp79E=" || ",_tag_dyneither(_tmp79E,
sizeof(char),5)))),((_tmp79D[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp266),Cyc_PP_cat(
_tag_dyneither(_tmp79D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL199;_LL1AC: {
struct Cyc_Absyn_SeqExp_e_struct*_tmp268=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmp24D;if(_tmp268->tag != 9)goto _LL1AE;else{_tmp269=_tmp268->f1;_tmp26A=_tmp268->f2;}}
_LL1AD:{const char*_tmp7A5;const char*_tmp7A4;const char*_tmp7A3;struct Cyc_PP_Doc*
_tmp7A2[5];s=((_tmp7A2[4]=Cyc_PP_text(((_tmp7A3=")",_tag_dyneither(_tmp7A3,
sizeof(char),2)))),((_tmp7A2[3]=Cyc_Absynpp_exp2doc(_tmp26A),((_tmp7A2[2]=Cyc_PP_text(((
_tmp7A4=", ",_tag_dyneither(_tmp7A4,sizeof(char),3)))),((_tmp7A2[1]=Cyc_Absynpp_exp2doc(
_tmp269),((_tmp7A2[0]=Cyc_PP_text(((_tmp7A5="(",_tag_dyneither(_tmp7A5,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7A2,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL199;_LL1AE: {struct Cyc_Absyn_UnknownCall_e_struct*_tmp26B=(struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp24D;if(_tmp26B->tag != 10)goto _LL1B0;else{_tmp26C=_tmp26B->f1;_tmp26D=_tmp26B->f2;}}
_LL1AF:{const char*_tmp7AA;const char*_tmp7A9;struct Cyc_PP_Doc*_tmp7A8[4];s=((
_tmp7A8[3]=Cyc_PP_text(((_tmp7A9=")",_tag_dyneither(_tmp7A9,sizeof(char),2)))),((
_tmp7A8[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp26D),((_tmp7A8[1]=Cyc_PP_text(((
_tmp7AA="(",_tag_dyneither(_tmp7AA,sizeof(char),2)))),((_tmp7A8[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp26C),Cyc_PP_cat(_tag_dyneither(_tmp7A8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL199;_LL1B0: {struct Cyc_Absyn_FnCall_e_struct*_tmp26E=(struct Cyc_Absyn_FnCall_e_struct*)
_tmp24D;if(_tmp26E->tag != 11)goto _LL1B2;else{_tmp26F=_tmp26E->f1;_tmp270=_tmp26E->f2;}}
_LL1B1:{const char*_tmp7AF;const char*_tmp7AE;struct Cyc_PP_Doc*_tmp7AD[4];s=((
_tmp7AD[3]=Cyc_PP_text(((_tmp7AE=")",_tag_dyneither(_tmp7AE,sizeof(char),2)))),((
_tmp7AD[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp270),((_tmp7AD[1]=Cyc_PP_text(((
_tmp7AF="(",_tag_dyneither(_tmp7AF,sizeof(char),2)))),((_tmp7AD[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp26F),Cyc_PP_cat(_tag_dyneither(_tmp7AD,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL199;_LL1B2: {struct Cyc_Absyn_Throw_e_struct*_tmp271=(struct Cyc_Absyn_Throw_e_struct*)
_tmp24D;if(_tmp271->tag != 12)goto _LL1B4;else{_tmp272=_tmp271->f1;}}_LL1B3:{const
char*_tmp7B2;struct Cyc_PP_Doc*_tmp7B1[2];s=((_tmp7B1[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp272),((_tmp7B1[0]=Cyc_PP_text(((_tmp7B2="throw ",_tag_dyneither(
_tmp7B2,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp7B1,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL199;_LL1B4: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmp273=(
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp24D;if(_tmp273->tag != 13)goto _LL1B6;
else{_tmp274=_tmp273->f1;}}_LL1B5: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp274);
goto _LL199;_LL1B6: {struct Cyc_Absyn_Instantiate_e_struct*_tmp275=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmp24D;if(_tmp275->tag != 14)goto _LL1B8;else{_tmp276=_tmp275->f1;}}_LL1B7: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmp276);goto _LL199;_LL1B8: {struct Cyc_Absyn_Cast_e_struct*_tmp277=(
struct Cyc_Absyn_Cast_e_struct*)_tmp24D;if(_tmp277->tag != 15)goto _LL1BA;else{
_tmp278=(void*)_tmp277->f1;_tmp279=_tmp277->f2;}}_LL1B9:{const char*_tmp7B7;const
char*_tmp7B6;struct Cyc_PP_Doc*_tmp7B5[4];s=((_tmp7B5[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp279),((_tmp7B5[2]=Cyc_PP_text(((_tmp7B6=")",_tag_dyneither(_tmp7B6,
sizeof(char),2)))),((_tmp7B5[1]=Cyc_Absynpp_typ2doc(_tmp278),((_tmp7B5[0]=Cyc_PP_text(((
_tmp7B7="(",_tag_dyneither(_tmp7B7,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7B5,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL199;_LL1BA: {struct Cyc_Absyn_Address_e_struct*
_tmp27A=(struct Cyc_Absyn_Address_e_struct*)_tmp24D;if(_tmp27A->tag != 16)goto
_LL1BC;else{_tmp27B=_tmp27A->f1;}}_LL1BB:{const char*_tmp7BA;struct Cyc_PP_Doc*
_tmp7B9[2];s=((_tmp7B9[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27B),((_tmp7B9[0]=
Cyc_PP_text(((_tmp7BA="&",_tag_dyneither(_tmp7BA,sizeof(char),2)))),Cyc_PP_cat(
_tag_dyneither(_tmp7B9,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL199;_LL1BC: {
struct Cyc_Absyn_New_e_struct*_tmp27C=(struct Cyc_Absyn_New_e_struct*)_tmp24D;if(
_tmp27C->tag != 17)goto _LL1BE;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27C->f2;}}
_LL1BD: if(_tmp27D == 0){const char*_tmp7BD;struct Cyc_PP_Doc*_tmp7BC[2];s=((_tmp7BC[
1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27E),((_tmp7BC[0]=Cyc_PP_text(((_tmp7BD="new ",
_tag_dyneither(_tmp7BD,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7BC,
sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*_tmp7C2;const char*_tmp7C1;
struct Cyc_PP_Doc*_tmp7C0[4];s=((_tmp7C0[3]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmp27E),((_tmp7C0[2]=Cyc_PP_text(((_tmp7C1=") ",_tag_dyneither(_tmp7C1,sizeof(
char),3)))),((_tmp7C0[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp27D),((
_tmp7C0[0]=Cyc_PP_text(((_tmp7C2="rnew(",_tag_dyneither(_tmp7C2,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp7C0,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL199;_LL1BE: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp27F=(struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp24D;if(_tmp27F->tag != 18)goto _LL1C0;else{_tmp280=(void*)_tmp27F->f1;}}_LL1BF:{
const char*_tmp7C7;const char*_tmp7C6;struct Cyc_PP_Doc*_tmp7C5[3];s=((_tmp7C5[2]=
Cyc_PP_text(((_tmp7C6=")",_tag_dyneither(_tmp7C6,sizeof(char),2)))),((_tmp7C5[1]=
Cyc_Absynpp_typ2doc(_tmp280),((_tmp7C5[0]=Cyc_PP_text(((_tmp7C7="sizeof(",
_tag_dyneither(_tmp7C7,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7C5,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL199;_LL1C0: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp281=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp24D;if(_tmp281->tag != 19)goto
_LL1C2;else{_tmp282=_tmp281->f1;}}_LL1C1:{const char*_tmp7CC;const char*_tmp7CB;
struct Cyc_PP_Doc*_tmp7CA[3];s=((_tmp7CA[2]=Cyc_PP_text(((_tmp7CB=")",
_tag_dyneither(_tmp7CB,sizeof(char),2)))),((_tmp7CA[1]=Cyc_Absynpp_exp2doc(
_tmp282),((_tmp7CA[0]=Cyc_PP_text(((_tmp7CC="sizeof(",_tag_dyneither(_tmp7CC,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7CA,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL199;_LL1C2: {struct Cyc_Absyn_Valueof_e_struct*_tmp283=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp24D;if(_tmp283->tag != 39)goto _LL1C4;else{_tmp284=(void*)_tmp283->f1;}}_LL1C3:{
const char*_tmp7D1;const char*_tmp7D0;struct Cyc_PP_Doc*_tmp7CF[3];s=((_tmp7CF[2]=
Cyc_PP_text(((_tmp7D0=")",_tag_dyneither(_tmp7D0,sizeof(char),2)))),((_tmp7CF[1]=
Cyc_Absynpp_typ2doc(_tmp284),((_tmp7CF[0]=Cyc_PP_text(((_tmp7D1="valueof(",
_tag_dyneither(_tmp7D1,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7CF,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL199;_LL1C4: {struct Cyc_Absyn_Asm_e_struct*
_tmp285=(struct Cyc_Absyn_Asm_e_struct*)_tmp24D;if(_tmp285->tag != 40)goto _LL1C6;
else{_tmp286=_tmp285->f1;_tmp287=_tmp285->f2;}}_LL1C5: if(_tmp286){const char*
_tmp7D8;const char*_tmp7D7;const char*_tmp7D6;struct Cyc_PP_Doc*_tmp7D5[4];s=((
_tmp7D5[3]=Cyc_PP_text(((_tmp7D6=")",_tag_dyneither(_tmp7D6,sizeof(char),2)))),((
_tmp7D5[2]=Cyc_PP_text(_tmp287),((_tmp7D5[1]=Cyc_PP_text(((_tmp7D7=" volatile (",
_tag_dyneither(_tmp7D7,sizeof(char),12)))),((_tmp7D5[0]=Cyc_PP_text(((_tmp7D8="__asm__",
_tag_dyneither(_tmp7D8,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7D5,
sizeof(struct Cyc_PP_Doc*),4))))))))));}else{const char*_tmp7DD;const char*_tmp7DC;
struct Cyc_PP_Doc*_tmp7DB[3];s=((_tmp7DB[2]=Cyc_PP_text(((_tmp7DC=")",
_tag_dyneither(_tmp7DC,sizeof(char),2)))),((_tmp7DB[1]=Cyc_PP_text(_tmp287),((
_tmp7DB[0]=Cyc_PP_text(((_tmp7DD="__asm__(",_tag_dyneither(_tmp7DD,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp7DB,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL199;_LL1C6: {struct Cyc_Absyn_Tagcheck_e_struct*_tmp288=(struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp24D;if(_tmp288->tag != 38)goto _LL1C8;else{_tmp289=_tmp288->f1;_tmp28A=_tmp288->f2;}}
_LL1C7:{const char*_tmp7E4;const char*_tmp7E3;const char*_tmp7E2;struct Cyc_PP_Doc*
_tmp7E1[5];s=((_tmp7E1[4]=Cyc_PP_text(((_tmp7E2=")",_tag_dyneither(_tmp7E2,
sizeof(char),2)))),((_tmp7E1[3]=Cyc_PP_textptr(_tmp28A),((_tmp7E1[2]=Cyc_PP_text(((
_tmp7E3=".",_tag_dyneither(_tmp7E3,sizeof(char),2)))),((_tmp7E1[1]=Cyc_Absynpp_exp2doc(
_tmp289),((_tmp7E1[0]=Cyc_PP_text(((_tmp7E4="tagcheck(",_tag_dyneither(_tmp7E4,
sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7E1,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL199;_LL1C8: {struct Cyc_Absyn_Offsetof_e_struct*_tmp28B=(
struct Cyc_Absyn_Offsetof_e_struct*)_tmp24D;if(_tmp28B->tag != 20)goto _LL1CA;else{
_tmp28C=(void*)_tmp28B->f1;_tmp28D=(void*)_tmp28B->f2;{struct Cyc_Absyn_StructField_struct*
_tmp28E=(struct Cyc_Absyn_StructField_struct*)_tmp28D;if(_tmp28E->tag != 0)goto
_LL1CA;else{_tmp28F=_tmp28E->f1;}};}}_LL1C9:{const char*_tmp7EB;const char*_tmp7EA;
const char*_tmp7E9;struct Cyc_PP_Doc*_tmp7E8[5];s=((_tmp7E8[4]=Cyc_PP_text(((
_tmp7E9=")",_tag_dyneither(_tmp7E9,sizeof(char),2)))),((_tmp7E8[3]=Cyc_PP_textptr(
_tmp28F),((_tmp7E8[2]=Cyc_PP_text(((_tmp7EA=",",_tag_dyneither(_tmp7EA,sizeof(
char),2)))),((_tmp7E8[1]=Cyc_Absynpp_typ2doc(_tmp28C),((_tmp7E8[0]=Cyc_PP_text(((
_tmp7EB="offsetof(",_tag_dyneither(_tmp7EB,sizeof(char),10)))),Cyc_PP_cat(
_tag_dyneither(_tmp7E8,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL199;
_LL1CA: {struct Cyc_Absyn_Offsetof_e_struct*_tmp290=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmp24D;if(_tmp290->tag != 20)goto _LL1CC;else{_tmp291=(void*)_tmp290->f1;_tmp292=(
void*)_tmp290->f2;{struct Cyc_Absyn_TupleIndex_struct*_tmp293=(struct Cyc_Absyn_TupleIndex_struct*)
_tmp292;if(_tmp293->tag != 1)goto _LL1CC;else{_tmp294=_tmp293->f1;}};}}_LL1CB:{
const char*_tmp7F9;const char*_tmp7F8;struct Cyc_Int_pa_struct _tmp7F7;void*_tmp7F6[
1];const char*_tmp7F5;const char*_tmp7F4;struct Cyc_PP_Doc*_tmp7F3[5];s=((_tmp7F3[4]=
Cyc_PP_text(((_tmp7F4=")",_tag_dyneither(_tmp7F4,sizeof(char),2)))),((_tmp7F3[3]=
Cyc_PP_text((struct _dyneither_ptr)((_tmp7F7.tag=1,((_tmp7F7.f1=(unsigned long)((
int)_tmp294),((_tmp7F6[0]=& _tmp7F7,Cyc_aprintf(((_tmp7F5="%d",_tag_dyneither(
_tmp7F5,sizeof(char),3))),_tag_dyneither(_tmp7F6,sizeof(void*),1))))))))),((
_tmp7F3[2]=Cyc_PP_text(((_tmp7F8=",",_tag_dyneither(_tmp7F8,sizeof(char),2)))),((
_tmp7F3[1]=Cyc_Absynpp_typ2doc(_tmp291),((_tmp7F3[0]=Cyc_PP_text(((_tmp7F9="offsetof(",
_tag_dyneither(_tmp7F9,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7F3,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL199;_LL1CC: {struct Cyc_Absyn_Deref_e_struct*
_tmp295=(struct Cyc_Absyn_Deref_e_struct*)_tmp24D;if(_tmp295->tag != 21)goto _LL1CE;
else{_tmp296=_tmp295->f1;}}_LL1CD:{const char*_tmp7FC;struct Cyc_PP_Doc*_tmp7FB[2];
s=((_tmp7FB[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp296),((_tmp7FB[0]=Cyc_PP_text(((
_tmp7FC="*",_tag_dyneither(_tmp7FC,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(
_tmp7FB,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL199;_LL1CE: {struct Cyc_Absyn_AggrMember_e_struct*
_tmp297=(struct Cyc_Absyn_AggrMember_e_struct*)_tmp24D;if(_tmp297->tag != 22)goto
_LL1D0;else{_tmp298=_tmp297->f1;_tmp299=_tmp297->f2;}}_LL1CF:{const char*_tmp7FF;
struct Cyc_PP_Doc*_tmp7FE[3];s=((_tmp7FE[2]=Cyc_PP_textptr(_tmp299),((_tmp7FE[1]=
Cyc_PP_text(((_tmp7FF=".",_tag_dyneither(_tmp7FF,sizeof(char),2)))),((_tmp7FE[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp298),Cyc_PP_cat(_tag_dyneither(_tmp7FE,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL199;_LL1D0: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmp29A=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmp24D;if(_tmp29A->tag != 23)goto
_LL1D2;else{_tmp29B=_tmp29A->f1;_tmp29C=_tmp29A->f2;}}_LL1D1:{const char*_tmp802;
struct Cyc_PP_Doc*_tmp801[3];s=((_tmp801[2]=Cyc_PP_textptr(_tmp29C),((_tmp801[1]=
Cyc_PP_text(((_tmp802="->",_tag_dyneither(_tmp802,sizeof(char),3)))),((_tmp801[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp29B),Cyc_PP_cat(_tag_dyneither(_tmp801,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL199;_LL1D2: {struct Cyc_Absyn_Subscript_e_struct*
_tmp29D=(struct Cyc_Absyn_Subscript_e_struct*)_tmp24D;if(_tmp29D->tag != 24)goto
_LL1D4;else{_tmp29E=_tmp29D->f1;_tmp29F=_tmp29D->f2;}}_LL1D3:{const char*_tmp807;
const char*_tmp806;struct Cyc_PP_Doc*_tmp805[4];s=((_tmp805[3]=Cyc_PP_text(((
_tmp806="]",_tag_dyneither(_tmp806,sizeof(char),2)))),((_tmp805[2]=Cyc_Absynpp_exp2doc(
_tmp29F),((_tmp805[1]=Cyc_PP_text(((_tmp807="[",_tag_dyneither(_tmp807,sizeof(
char),2)))),((_tmp805[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29E),Cyc_PP_cat(
_tag_dyneither(_tmp805,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL199;_LL1D4: {
struct Cyc_Absyn_Tuple_e_struct*_tmp2A0=(struct Cyc_Absyn_Tuple_e_struct*)_tmp24D;
if(_tmp2A0->tag != 25)goto _LL1D6;else{_tmp2A1=_tmp2A0->f1;}}_LL1D5:{const char*
_tmp80C;const char*_tmp80B;struct Cyc_PP_Doc*_tmp80A[4];s=((_tmp80A[3]=Cyc_PP_text(((
_tmp80B=")",_tag_dyneither(_tmp80B,sizeof(char),2)))),((_tmp80A[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp2A1),((_tmp80A[1]=Cyc_PP_text(((_tmp80C="(",_tag_dyneither(_tmp80C,
sizeof(char),2)))),((_tmp80A[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(
_tmp80A,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL199;_LL1D6: {struct Cyc_Absyn_CompoundLit_e_struct*
_tmp2A2=(struct Cyc_Absyn_CompoundLit_e_struct*)_tmp24D;if(_tmp2A2->tag != 26)goto
_LL1D8;else{_tmp2A3=_tmp2A2->f1;_tmp2A4=_tmp2A2->f2;}}_LL1D7:{const char*_tmp813;
const char*_tmp812;const char*_tmp811;struct Cyc_PP_Doc*_tmp810[4];s=((_tmp810[3]=
Cyc_Absynpp_group_braces(((_tmp811=",",_tag_dyneither(_tmp811,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A4)),((_tmp810[2]=Cyc_PP_text(((_tmp812=")",
_tag_dyneither(_tmp812,sizeof(char),2)))),((_tmp810[1]=Cyc_Absynpp_typ2doc((*
_tmp2A3).f3),((_tmp810[0]=Cyc_PP_text(((_tmp813="(",_tag_dyneither(_tmp813,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp810,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL199;_LL1D8: {struct Cyc_Absyn_Array_e_struct*_tmp2A5=(struct Cyc_Absyn_Array_e_struct*)
_tmp24D;if(_tmp2A5->tag != 27)goto _LL1DA;else{_tmp2A6=_tmp2A5->f1;}}_LL1D9:{const
char*_tmp814;s=Cyc_Absynpp_group_braces(((_tmp814=",",_tag_dyneither(_tmp814,
sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A6));}goto _LL199;
_LL1DA: {struct Cyc_Absyn_Comprehension_e_struct*_tmp2A7=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmp24D;if(_tmp2A7->tag != 28)goto _LL1DC;else{_tmp2A8=_tmp2A7->f1;_tmp2A9=_tmp2A7->f2;
_tmp2AA=_tmp2A7->f3;}}_LL1DB:{const char*_tmp81B;const char*_tmp81A;const char*
_tmp819;struct Cyc_PP_Doc*_tmp818[8];s=((_tmp818[7]=Cyc_Absynpp_rb(),((_tmp818[6]=
Cyc_Absynpp_exp2doc(_tmp2AA),((_tmp818[5]=Cyc_PP_text(((_tmp819=" : ",
_tag_dyneither(_tmp819,sizeof(char),4)))),((_tmp818[4]=Cyc_Absynpp_exp2doc(
_tmp2A9),((_tmp818[3]=Cyc_PP_text(((_tmp81A=" < ",_tag_dyneither(_tmp81A,sizeof(
char),4)))),((_tmp818[2]=Cyc_PP_text(*(*_tmp2A8->name).f2),((_tmp818[1]=Cyc_PP_text(((
_tmp81B="for ",_tag_dyneither(_tmp81B,sizeof(char),5)))),((_tmp818[0]=Cyc_Absynpp_lb(),
Cyc_PP_cat(_tag_dyneither(_tmp818,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL199;_LL1DC: {struct Cyc_Absyn_Aggregate_e_struct*_tmp2AB=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmp24D;if(_tmp2AB->tag != 29)goto _LL1DE;else{_tmp2AC=_tmp2AB->f1;_tmp2AD=_tmp2AB->f2;
_tmp2AE=_tmp2AB->f3;}}_LL1DD: {struct Cyc_List_List*_tmp323=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2AE);{const char*_tmp820;struct Cyc_List_List*_tmp81F;struct Cyc_PP_Doc*_tmp81E[
2];s=((_tmp81E[1]=Cyc_Absynpp_group_braces(((_tmp820=",",_tag_dyneither(_tmp820,
sizeof(char),2))),_tmp2AD != 0?(_tmp81F=_cycalloc(sizeof(*_tmp81F)),((_tmp81F->hd=
Cyc_Absynpp_tps2doc(_tmp2AD),((_tmp81F->tl=_tmp323,_tmp81F))))): _tmp323),((
_tmp81E[0]=Cyc_Absynpp_qvar2doc(_tmp2AC),Cyc_PP_cat(_tag_dyneither(_tmp81E,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL199;}_LL1DE: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmp2AF=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmp24D;if(_tmp2AF->tag != 30)goto
_LL1E0;else{_tmp2B0=_tmp2AF->f2;}}_LL1DF:{const char*_tmp821;s=Cyc_Absynpp_group_braces(((
_tmp821=",",_tag_dyneither(_tmp821,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2B0));}goto _LL199;_LL1E0: {struct Cyc_Absyn_Datatype_e_struct*_tmp2B1=(struct
Cyc_Absyn_Datatype_e_struct*)_tmp24D;if(_tmp2B1->tag != 31)goto _LL1E2;else{
_tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B1->f3;}}_LL1E1: if(_tmp2B2 == 0)s=Cyc_Absynpp_qvar2doc(
_tmp2B3->name);else{const char*_tmp828;const char*_tmp827;const char*_tmp826;struct
Cyc_PP_Doc*_tmp825[2];s=((_tmp825[1]=Cyc_PP_egroup(((_tmp828="(",_tag_dyneither(
_tmp828,sizeof(char),2))),((_tmp827=")",_tag_dyneither(_tmp827,sizeof(char),2))),((
_tmp826=",",_tag_dyneither(_tmp826,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_exp2doc,_tmp2B2)),((_tmp825[0]=Cyc_Absynpp_qvar2doc(_tmp2B3->name),
Cyc_PP_cat(_tag_dyneither(_tmp825,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL199;
_LL1E2: {struct Cyc_Absyn_Enum_e_struct*_tmp2B4=(struct Cyc_Absyn_Enum_e_struct*)
_tmp24D;if(_tmp2B4->tag != 32)goto _LL1E4;else{_tmp2B5=_tmp2B4->f1;}}_LL1E3: s=Cyc_Absynpp_qvar2doc(
_tmp2B5);goto _LL199;_LL1E4: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp2B6=(struct
Cyc_Absyn_AnonEnum_e_struct*)_tmp24D;if(_tmp2B6->tag != 33)goto _LL1E6;else{
_tmp2B7=_tmp2B6->f1;}}_LL1E5: s=Cyc_Absynpp_qvar2doc(_tmp2B7);goto _LL199;_LL1E6: {
struct Cyc_Absyn_Malloc_e_struct*_tmp2B8=(struct Cyc_Absyn_Malloc_e_struct*)
_tmp24D;if(_tmp2B8->tag != 34)goto _LL1E8;else{_tmp2B9=_tmp2B8->f1;_tmp2BA=_tmp2B9.is_calloc;
_tmp2BB=_tmp2B9.rgn;_tmp2BC=_tmp2B9.elt_type;_tmp2BD=_tmp2B9.num_elts;}}_LL1E7:
if(_tmp2BA){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp2BC)),0);if(_tmp2BB == 0){const char*_tmp82F;const char*_tmp82E;const char*
_tmp82D;struct Cyc_PP_Doc*_tmp82C[5];s=((_tmp82C[4]=Cyc_PP_text(((_tmp82D=")",
_tag_dyneither(_tmp82D,sizeof(char),2)))),((_tmp82C[3]=Cyc_Absynpp_exp2doc(st),((
_tmp82C[2]=Cyc_PP_text(((_tmp82E=",",_tag_dyneither(_tmp82E,sizeof(char),2)))),((
_tmp82C[1]=Cyc_Absynpp_exp2doc(_tmp2BD),((_tmp82C[0]=Cyc_PP_text(((_tmp82F="calloc(",
_tag_dyneither(_tmp82F,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp82C,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp838;const char*
_tmp837;const char*_tmp836;const char*_tmp835;struct Cyc_PP_Doc*_tmp834[7];s=((
_tmp834[6]=Cyc_PP_text(((_tmp835=")",_tag_dyneither(_tmp835,sizeof(char),2)))),((
_tmp834[5]=Cyc_Absynpp_exp2doc(st),((_tmp834[4]=Cyc_PP_text(((_tmp836=",",
_tag_dyneither(_tmp836,sizeof(char),2)))),((_tmp834[3]=Cyc_Absynpp_exp2doc(
_tmp2BD),((_tmp834[2]=Cyc_PP_text(((_tmp837=",",_tag_dyneither(_tmp837,sizeof(
char),2)))),((_tmp834[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2BB),((
_tmp834[0]=Cyc_PP_text(((_tmp838="rcalloc(",_tag_dyneither(_tmp838,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp834,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp2BC == 0)new_e=_tmp2BD;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp2BC,0),_tmp2BD,0);}if(_tmp2BB == 0){const char*
_tmp83D;const char*_tmp83C;struct Cyc_PP_Doc*_tmp83B[3];s=((_tmp83B[2]=Cyc_PP_text(((
_tmp83C=")",_tag_dyneither(_tmp83C,sizeof(char),2)))),((_tmp83B[1]=Cyc_Absynpp_exp2doc(
new_e),((_tmp83B[0]=Cyc_PP_text(((_tmp83D="malloc(",_tag_dyneither(_tmp83D,
sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp83B,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp844;const char*_tmp843;const char*_tmp842;struct Cyc_PP_Doc*
_tmp841[5];s=((_tmp841[4]=Cyc_PP_text(((_tmp842=")",_tag_dyneither(_tmp842,
sizeof(char),2)))),((_tmp841[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp841[2]=Cyc_PP_text(((
_tmp843=",",_tag_dyneither(_tmp843,sizeof(char),2)))),((_tmp841[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp2BB),((_tmp841[0]=Cyc_PP_text(((_tmp844="rmalloc(",
_tag_dyneither(_tmp844,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp841,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL199;_LL1E8: {struct Cyc_Absyn_Swap_e_struct*
_tmp2BE=(struct Cyc_Absyn_Swap_e_struct*)_tmp24D;if(_tmp2BE->tag != 35)goto _LL1EA;
else{_tmp2BF=_tmp2BE->f1;_tmp2C0=_tmp2BE->f2;}}_LL1E9:{const char*_tmp84B;const
char*_tmp84A;const char*_tmp849;struct Cyc_PP_Doc*_tmp848[5];s=((_tmp848[4]=Cyc_PP_text(((
_tmp849=")",_tag_dyneither(_tmp849,sizeof(char),2)))),((_tmp848[3]=Cyc_Absynpp_exp2doc(
_tmp2C0),((_tmp848[2]=Cyc_PP_text(((_tmp84A=",",_tag_dyneither(_tmp84A,sizeof(
char),2)))),((_tmp848[1]=Cyc_Absynpp_exp2doc(_tmp2BF),((_tmp848[0]=Cyc_PP_text(((
_tmp84B="cycswap(",_tag_dyneither(_tmp84B,sizeof(char),9)))),Cyc_PP_cat(
_tag_dyneither(_tmp848,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL199;
_LL1EA: {struct Cyc_Absyn_UnresolvedMem_e_struct*_tmp2C1=(struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp24D;if(_tmp2C1->tag != 36)goto _LL1EC;else{_tmp2C2=_tmp2C1->f1;_tmp2C3=_tmp2C1->f2;}}
_LL1EB:{const char*_tmp84C;s=Cyc_Absynpp_group_braces(((_tmp84C=",",
_tag_dyneither(_tmp84C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp2C3));}goto _LL199;_LL1EC: {struct Cyc_Absyn_StmtExp_e_struct*_tmp2C4=(struct
Cyc_Absyn_StmtExp_e_struct*)_tmp24D;if(_tmp2C4->tag != 37)goto _LL199;else{_tmp2C5=
_tmp2C4->f1;}}_LL1ED:{const char*_tmp851;const char*_tmp850;struct Cyc_PP_Doc*
_tmp84F[7];s=((_tmp84F[6]=Cyc_PP_text(((_tmp850=")",_tag_dyneither(_tmp850,
sizeof(char),2)))),((_tmp84F[5]=Cyc_Absynpp_rb(),((_tmp84F[4]=Cyc_PP_blank_doc(),((
_tmp84F[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2C5,1)),((_tmp84F[2]=Cyc_PP_blank_doc(),((
_tmp84F[1]=Cyc_Absynpp_lb(),((_tmp84F[0]=Cyc_PP_text(((_tmp851="(",
_tag_dyneither(_tmp851,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp84F,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL199;_LL199:;}if(inprec >= 
myprec){const char*_tmp856;const char*_tmp855;struct Cyc_PP_Doc*_tmp854[3];s=((
_tmp854[2]=Cyc_PP_text(((_tmp855=")",_tag_dyneither(_tmp855,sizeof(char),2)))),((
_tmp854[1]=s,((_tmp854[0]=Cyc_PP_text(((_tmp856="(",_tag_dyneither(_tmp856,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp854,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp347=d;
struct Cyc_Absyn_Exp*_tmp349;struct _dyneither_ptr*_tmp34B;_LL1F4: {struct Cyc_Absyn_ArrayElement_struct*
_tmp348=(struct Cyc_Absyn_ArrayElement_struct*)_tmp347;if(_tmp348->tag != 0)goto
_LL1F6;else{_tmp349=_tmp348->f1;}}_LL1F5: {const char*_tmp85B;const char*_tmp85A;
struct Cyc_PP_Doc*_tmp859[3];return(_tmp859[2]=Cyc_PP_text(((_tmp85A="]",
_tag_dyneither(_tmp85A,sizeof(char),2)))),((_tmp859[1]=Cyc_Absynpp_exp2doc(
_tmp349),((_tmp859[0]=Cyc_PP_text(((_tmp85B=".[",_tag_dyneither(_tmp85B,sizeof(
char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp859,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL1F6: {struct Cyc_Absyn_FieldName_struct*_tmp34A=(struct Cyc_Absyn_FieldName_struct*)
_tmp347;if(_tmp34A->tag != 1)goto _LL1F3;else{_tmp34B=_tmp34A->f1;}}_LL1F7: {const
char*_tmp85E;struct Cyc_PP_Doc*_tmp85D[2];return(_tmp85D[1]=Cyc_PP_textptr(
_tmp34B),((_tmp85D[0]=Cyc_PP_text(((_tmp85E=".",_tag_dyneither(_tmp85E,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp85D,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL1F3:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){if((*de).f1 == 0)
return Cyc_Absynpp_exp2doc((*de).f2);else{const char*_tmp865;const char*_tmp864;
const char*_tmp863;struct Cyc_PP_Doc*_tmp862[2];return(_tmp862[1]=Cyc_Absynpp_exp2doc((*
de).f2),((_tmp862[0]=Cyc_PP_egroup(((_tmp865="",_tag_dyneither(_tmp865,sizeof(
char),1))),((_tmp864="=",_tag_dyneither(_tmp864,sizeof(char),2))),((_tmp863="=",
_tag_dyneither(_tmp863,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),
Cyc_PP_cat(_tag_dyneither(_tmp862,sizeof(struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){const char*_tmp868;
const char*_tmp867;const char*_tmp866;return Cyc_PP_group(((_tmp866="",
_tag_dyneither(_tmp866,sizeof(char),1))),((_tmp867="",_tag_dyneither(_tmp867,
sizeof(char),1))),((_tmp868=",",_tag_dyneither(_tmp868,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst
_tmp358=c;struct _tuple3 _tmp359;enum Cyc_Absyn_Sign _tmp35A;char _tmp35B;struct
_tuple4 _tmp35C;enum Cyc_Absyn_Sign _tmp35D;short _tmp35E;struct _tuple5 _tmp35F;
enum Cyc_Absyn_Sign _tmp360;int _tmp361;struct _tuple5 _tmp362;enum Cyc_Absyn_Sign
_tmp363;int _tmp364;struct _tuple5 _tmp365;enum Cyc_Absyn_Sign _tmp366;int _tmp367;
struct _tuple6 _tmp368;enum Cyc_Absyn_Sign _tmp369;long long _tmp36A;struct
_dyneither_ptr _tmp36B;int _tmp36C;struct _dyneither_ptr _tmp36D;_LL1F9: if((_tmp358.Char_c).tag
!= 2)goto _LL1FB;_tmp359=(struct _tuple3)(_tmp358.Char_c).val;_tmp35A=_tmp359.f1;
_tmp35B=_tmp359.f2;_LL1FA: {const char*_tmp86C;void*_tmp86B[1];struct Cyc_String_pa_struct
_tmp86A;return Cyc_PP_text((struct _dyneither_ptr)((_tmp86A.tag=0,((_tmp86A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp35B)),((
_tmp86B[0]=& _tmp86A,Cyc_aprintf(((_tmp86C="'%s'",_tag_dyneither(_tmp86C,sizeof(
char),5))),_tag_dyneither(_tmp86B,sizeof(void*),1)))))))));}_LL1FB: if((_tmp358.Short_c).tag
!= 3)goto _LL1FD;_tmp35C=(struct _tuple4)(_tmp358.Short_c).val;_tmp35D=_tmp35C.f1;
_tmp35E=_tmp35C.f2;_LL1FC: {const char*_tmp870;void*_tmp86F[1];struct Cyc_Int_pa_struct
_tmp86E;return Cyc_PP_text((struct _dyneither_ptr)((_tmp86E.tag=1,((_tmp86E.f1=(
unsigned long)((int)_tmp35E),((_tmp86F[0]=& _tmp86E,Cyc_aprintf(((_tmp870="%d",
_tag_dyneither(_tmp870,sizeof(char),3))),_tag_dyneither(_tmp86F,sizeof(void*),1)))))))));}
_LL1FD: if((_tmp358.Int_c).tag != 4)goto _LL1FF;_tmp35F=(struct _tuple5)(_tmp358.Int_c).val;
_tmp360=_tmp35F.f1;if(_tmp360 != Cyc_Absyn_None)goto _LL1FF;_tmp361=_tmp35F.f2;
_LL1FE: _tmp364=_tmp361;goto _LL200;_LL1FF: if((_tmp358.Int_c).tag != 4)goto _LL201;
_tmp362=(struct _tuple5)(_tmp358.Int_c).val;_tmp363=_tmp362.f1;if(_tmp363 != Cyc_Absyn_Signed)
goto _LL201;_tmp364=_tmp362.f2;_LL200: {const char*_tmp874;void*_tmp873[1];struct
Cyc_Int_pa_struct _tmp872;return Cyc_PP_text((struct _dyneither_ptr)((_tmp872.tag=1,((
_tmp872.f1=(unsigned long)_tmp364,((_tmp873[0]=& _tmp872,Cyc_aprintf(((_tmp874="%d",
_tag_dyneither(_tmp874,sizeof(char),3))),_tag_dyneither(_tmp873,sizeof(void*),1)))))))));}
_LL201: if((_tmp358.Int_c).tag != 4)goto _LL203;_tmp365=(struct _tuple5)(_tmp358.Int_c).val;
_tmp366=_tmp365.f1;if(_tmp366 != Cyc_Absyn_Unsigned)goto _LL203;_tmp367=_tmp365.f2;
_LL202: {const char*_tmp878;void*_tmp877[1];struct Cyc_Int_pa_struct _tmp876;return
Cyc_PP_text((struct _dyneither_ptr)((_tmp876.tag=1,((_tmp876.f1=(unsigned int)
_tmp367,((_tmp877[0]=& _tmp876,Cyc_aprintf(((_tmp878="%u",_tag_dyneither(_tmp878,
sizeof(char),3))),_tag_dyneither(_tmp877,sizeof(void*),1)))))))));}_LL203: if((
_tmp358.LongLong_c).tag != 5)goto _LL205;_tmp368=(struct _tuple6)(_tmp358.LongLong_c).val;
_tmp369=_tmp368.f1;_tmp36A=_tmp368.f2;_LL204: {const char*_tmp879;return Cyc_PP_text(((
_tmp879="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp879,sizeof(char),27))));}
_LL205: if((_tmp358.Float_c).tag != 6)goto _LL207;_tmp36B=(struct _dyneither_ptr)(
_tmp358.Float_c).val;_LL206: return Cyc_PP_text(_tmp36B);_LL207: if((_tmp358.Null_c).tag
!= 1)goto _LL209;_tmp36C=(int)(_tmp358.Null_c).val;_LL208: {const char*_tmp87A;
return Cyc_PP_text(((_tmp87A="NULL",_tag_dyneither(_tmp87A,sizeof(char),5))));}
_LL209: if((_tmp358.String_c).tag != 7)goto _LL1F8;_tmp36D=(struct _dyneither_ptr)(
_tmp358.String_c).val;_LL20A: {const char*_tmp87F;const char*_tmp87E;struct Cyc_PP_Doc*
_tmp87D[3];return(_tmp87D[2]=Cyc_PP_text(((_tmp87E="\"",_tag_dyneither(_tmp87E,
sizeof(char),2)))),((_tmp87D[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp36D)),((
_tmp87D[0]=Cyc_PP_text(((_tmp87F="\"",_tag_dyneither(_tmp87F,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp87D,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1F8:;}
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct
Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (enum Cyc_Absyn_Primop)
Cyc_Absyn_Numelts){if(es == 0  || es->tl != 0){struct Cyc_Core_Failure_struct _tmp88C;
const char*_tmp88B;void*_tmp88A[1];struct Cyc_String_pa_struct _tmp889;struct Cyc_Core_Failure_struct*
_tmp888;(int)_throw((void*)((_tmp888=_cycalloc(sizeof(*_tmp888)),((_tmp888[0]=((
_tmp88C.tag=Cyc_Core_Failure,((_tmp88C.f1=(struct _dyneither_ptr)((_tmp889.tag=0,((
_tmp889.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp88A[0]=& _tmp889,Cyc_aprintf(((_tmp88B="Absynpp::primapp2doc Numelts: %s with bad args",
_tag_dyneither(_tmp88B,sizeof(char),47))),_tag_dyneither(_tmp88A,sizeof(void*),1)))))))),
_tmp88C)))),_tmp888)))));}{const char*_tmp891;const char*_tmp890;struct Cyc_PP_Doc*
_tmp88F[3];return(_tmp88F[2]=Cyc_PP_text(((_tmp890=")",_tag_dyneither(_tmp890,
sizeof(char),2)))),((_tmp88F[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((
_tmp88F[0]=Cyc_PP_text(((_tmp891="numelts(",_tag_dyneither(_tmp891,sizeof(char),
9)))),Cyc_PP_cat(_tag_dyneither(_tmp88F,sizeof(struct Cyc_PP_Doc*),3)))))));};}
else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,
struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0){struct Cyc_Core_Failure_struct _tmp89E;const char*_tmp89D;
void*_tmp89C[1];struct Cyc_String_pa_struct _tmp89B;struct Cyc_Core_Failure_struct*
_tmp89A;(int)_throw((void*)((_tmp89A=_cycalloc(sizeof(*_tmp89A)),((_tmp89A[0]=((
_tmp89E.tag=Cyc_Core_Failure,((_tmp89E.f1=(struct _dyneither_ptr)((_tmp89B.tag=0,((
_tmp89B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp89C[0]=& _tmp89B,Cyc_aprintf(((_tmp89D="Absynpp::primapp2doc: %s with no args",
_tag_dyneither(_tmp89D,sizeof(char),38))),_tag_dyneither(_tmp89C,sizeof(void*),1)))))))),
_tmp89E)))),_tmp89A)))));}else{if(ds->tl == 0){const char*_tmp8A1;struct Cyc_PP_Doc*
_tmp8A0[3];return(_tmp8A0[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp8A0[1]=Cyc_PP_text(((
_tmp8A1=" ",_tag_dyneither(_tmp8A1,sizeof(char),2)))),((_tmp8A0[0]=ps,Cyc_PP_cat(
_tag_dyneither(_tmp8A0,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp8AE;const char*
_tmp8AD;void*_tmp8AC[1];struct Cyc_String_pa_struct _tmp8AB;struct Cyc_Core_Failure_struct*
_tmp8AA;(int)_throw((void*)((_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA[0]=((
_tmp8AE.tag=Cyc_Core_Failure,((_tmp8AE.f1=(struct _dyneither_ptr)((_tmp8AB.tag=0,((
_tmp8AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,
72)),((_tmp8AC[0]=& _tmp8AB,Cyc_aprintf(((_tmp8AD="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dyneither(_tmp8AD,sizeof(char),47))),_tag_dyneither(_tmp8AC,sizeof(void*),1)))))))),
_tmp8AE)))),_tmp8AA)))));}else{const char*_tmp8B3;const char*_tmp8B2;struct Cyc_PP_Doc*
_tmp8B1[5];return(_tmp8B1[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)
_check_null(ds->tl))->hd,((_tmp8B1[3]=Cyc_PP_text(((_tmp8B2=" ",_tag_dyneither(
_tmp8B2,sizeof(char),2)))),((_tmp8B1[2]=ps,((_tmp8B1[1]=Cyc_PP_text(((_tmp8B3=" ",
_tag_dyneither(_tmp8B3,sizeof(char),2)))),((_tmp8B1[0]=(struct Cyc_PP_Doc*)ds->hd,
Cyc_PP_cat(_tag_dyneither(_tmp8B1,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){switch(p){case
Cyc_Absyn_Plus: _LL20B: {const char*_tmp8B4;return(_tmp8B4="+",_tag_dyneither(
_tmp8B4,sizeof(char),2));}case Cyc_Absyn_Times: _LL20C: {const char*_tmp8B5;return(
_tmp8B5="*",_tag_dyneither(_tmp8B5,sizeof(char),2));}case Cyc_Absyn_Minus: _LL20D: {
const char*_tmp8B6;return(_tmp8B6="-",_tag_dyneither(_tmp8B6,sizeof(char),2));}
case Cyc_Absyn_Div: _LL20E: {const char*_tmp8B7;return(_tmp8B7="/",_tag_dyneither(
_tmp8B7,sizeof(char),2));}case Cyc_Absyn_Mod: _LL20F: {const char*_tmp8B9;const char*
_tmp8B8;return Cyc_Absynpp_print_for_cycdoc?(_tmp8B9="\\%",_tag_dyneither(_tmp8B9,
sizeof(char),3)):((_tmp8B8="%",_tag_dyneither(_tmp8B8,sizeof(char),2)));}case Cyc_Absyn_Eq:
_LL210: {const char*_tmp8BA;return(_tmp8BA="==",_tag_dyneither(_tmp8BA,sizeof(
char),3));}case Cyc_Absyn_Neq: _LL211: {const char*_tmp8BB;return(_tmp8BB="!=",
_tag_dyneither(_tmp8BB,sizeof(char),3));}case Cyc_Absyn_Gt: _LL212: {const char*
_tmp8BC;return(_tmp8BC=">",_tag_dyneither(_tmp8BC,sizeof(char),2));}case Cyc_Absyn_Lt:
_LL213: {const char*_tmp8BD;return(_tmp8BD="<",_tag_dyneither(_tmp8BD,sizeof(char),
2));}case Cyc_Absyn_Gte: _LL214: {const char*_tmp8BE;return(_tmp8BE=">=",
_tag_dyneither(_tmp8BE,sizeof(char),3));}case Cyc_Absyn_Lte: _LL215: {const char*
_tmp8BF;return(_tmp8BF="<=",_tag_dyneither(_tmp8BF,sizeof(char),3));}case Cyc_Absyn_Not:
_LL216: {const char*_tmp8C0;return(_tmp8C0="!",_tag_dyneither(_tmp8C0,sizeof(char),
2));}case Cyc_Absyn_Bitnot: _LL217: {const char*_tmp8C1;return(_tmp8C1="~",
_tag_dyneither(_tmp8C1,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL218: {const char*
_tmp8C2;return(_tmp8C2="&",_tag_dyneither(_tmp8C2,sizeof(char),2));}case Cyc_Absyn_Bitor:
_LL219: {const char*_tmp8C3;return(_tmp8C3="|",_tag_dyneither(_tmp8C3,sizeof(char),
2));}case Cyc_Absyn_Bitxor: _LL21A: {const char*_tmp8C4;return(_tmp8C4="^",
_tag_dyneither(_tmp8C4,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL21B: {const
char*_tmp8C5;return(_tmp8C5="<<",_tag_dyneither(_tmp8C5,sizeof(char),3));}case
Cyc_Absyn_Bitlrshift: _LL21C: {const char*_tmp8C6;return(_tmp8C6=">>",
_tag_dyneither(_tmp8C6,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL21D: {const
char*_tmp8C7;return(_tmp8C7=">>>",_tag_dyneither(_tmp8C7,sizeof(char),4));}case
Cyc_Absyn_Numelts: _LL21E: {const char*_tmp8C8;return(_tmp8C8="numelts",
_tag_dyneither(_tmp8C8,sizeof(char),8));}}}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
enum Cyc_Absyn_Primop p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp3AB=s->r;_LL221: {struct Cyc_Absyn_Decl_s_struct*
_tmp3AC=(struct Cyc_Absyn_Decl_s_struct*)_tmp3AB;if(_tmp3AC->tag != 12)goto _LL223;}
_LL222: return 1;_LL223:;_LL224: return 0;_LL220:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st,int stmtexp){struct Cyc_PP_Doc*s;{void*_tmp3AD=st->r;
struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Stmt*_tmp3B2;struct Cyc_Absyn_Stmt*
_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Stmt*
_tmp3B8;struct Cyc_Absyn_Stmt*_tmp3B9;struct _tuple8 _tmp3BB;struct Cyc_Absyn_Exp*
_tmp3BC;struct Cyc_Absyn_Stmt*_tmp3BD;struct _dyneither_ptr*_tmp3C1;struct Cyc_Absyn_Exp*
_tmp3C3;struct _tuple8 _tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;struct _tuple8 _tmp3C6;
struct Cyc_Absyn_Exp*_tmp3C7;struct Cyc_Absyn_Stmt*_tmp3C8;struct Cyc_Absyn_Exp*
_tmp3CA;struct Cyc_List_List*_tmp3CB;struct Cyc_List_List*_tmp3CD;struct Cyc_List_List*
_tmp3CF;struct Cyc_Absyn_Decl*_tmp3D1;struct Cyc_Absyn_Stmt*_tmp3D2;struct
_dyneither_ptr*_tmp3D4;struct Cyc_Absyn_Stmt*_tmp3D5;struct Cyc_Absyn_Stmt*_tmp3D7;
struct _tuple8 _tmp3D8;struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Stmt*_tmp3DB;
struct Cyc_List_List*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DE;_LL226: {struct Cyc_Absyn_Skip_s_struct*
_tmp3AE=(struct Cyc_Absyn_Skip_s_struct*)_tmp3AD;if(_tmp3AE->tag != 0)goto _LL228;}
_LL227:{const char*_tmp8C9;s=Cyc_PP_text(((_tmp8C9=";",_tag_dyneither(_tmp8C9,
sizeof(char),2))));}goto _LL225;_LL228: {struct Cyc_Absyn_Exp_s_struct*_tmp3AF=(
struct Cyc_Absyn_Exp_s_struct*)_tmp3AD;if(_tmp3AF->tag != 1)goto _LL22A;else{
_tmp3B0=_tmp3AF->f1;}}_LL229:{const char*_tmp8CC;struct Cyc_PP_Doc*_tmp8CB[2];s=((
_tmp8CB[1]=Cyc_PP_text(((_tmp8CC=";",_tag_dyneither(_tmp8CC,sizeof(char),2)))),((
_tmp8CB[0]=Cyc_Absynpp_exp2doc(_tmp3B0),Cyc_PP_cat(_tag_dyneither(_tmp8CB,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL225;_LL22A: {struct Cyc_Absyn_Seq_s_struct*
_tmp3B1=(struct Cyc_Absyn_Seq_s_struct*)_tmp3AD;if(_tmp3B1->tag != 2)goto _LL22C;
else{_tmp3B2=_tmp3B1->f1;_tmp3B3=_tmp3B1->f2;}}_LL22B: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3B2)){struct Cyc_PP_Doc*_tmp8D7[5];struct Cyc_PP_Doc*
_tmp8D6[7];const char*_tmp8D5;const char*_tmp8D4;struct Cyc_PP_Doc*_tmp8D3[7];s=((
_tmp8D3[6]=Cyc_Absynpp_is_declaration(_tmp3B3)?stmtexp?(_tmp8D6[6]=Cyc_PP_line_doc(),((
_tmp8D6[5]=Cyc_PP_text(((_tmp8D5=");",_tag_dyneither(_tmp8D5,sizeof(char),3)))),((
_tmp8D6[4]=Cyc_Absynpp_rb(),((_tmp8D6[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3B3,stmtexp)),((_tmp8D6[2]=Cyc_PP_blank_doc(),((_tmp8D6[1]=Cyc_Absynpp_lb(),((
_tmp8D6[0]=Cyc_PP_text(((_tmp8D4="(",_tag_dyneither(_tmp8D4,sizeof(char),2)))),
Cyc_PP_cat(_tag_dyneither(_tmp8D6,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):((
_tmp8D7[4]=Cyc_PP_line_doc(),((_tmp8D7[3]=Cyc_Absynpp_rb(),((_tmp8D7[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3B3,stmtexp)),((_tmp8D7[1]=Cyc_PP_blank_doc(),((
_tmp8D7[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8D7,sizeof(struct Cyc_PP_Doc*),
5)))))))))))): Cyc_Absynpp_stmt2doc(_tmp3B3,stmtexp),((_tmp8D3[5]=Cyc_PP_line_doc(),((
_tmp8D3[4]=Cyc_Absynpp_rb(),((_tmp8D3[3]=Cyc_PP_line_doc(),((_tmp8D3[2]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3B2,0)),((_tmp8D3[1]=Cyc_PP_blank_doc(),((_tmp8D3[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8D3,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
else{if(Cyc_Absynpp_is_declaration(_tmp3B3)){struct Cyc_PP_Doc*_tmp8E2[4];struct
Cyc_PP_Doc*_tmp8E1[6];const char*_tmp8E0;const char*_tmp8DF;struct Cyc_PP_Doc*
_tmp8DE[4];s=((_tmp8DE[3]=Cyc_PP_line_doc(),((_tmp8DE[2]=stmtexp?(_tmp8E1[5]=Cyc_PP_text(((
_tmp8E0=");",_tag_dyneither(_tmp8E0,sizeof(char),3)))),((_tmp8E1[4]=Cyc_Absynpp_rb(),((
_tmp8E1[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3B3,stmtexp)),((_tmp8E1[2]=Cyc_PP_blank_doc(),((
_tmp8E1[1]=Cyc_Absynpp_lb(),((_tmp8E1[0]=Cyc_PP_text(((_tmp8DF="(",
_tag_dyneither(_tmp8DF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8E1,
sizeof(struct Cyc_PP_Doc*),6))))))))))))):((_tmp8E2[3]=Cyc_Absynpp_rb(),((_tmp8E2[
2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3B3,stmtexp)),((_tmp8E2[1]=Cyc_PP_blank_doc(),((
_tmp8E2[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8E2,sizeof(struct Cyc_PP_Doc*),
4)))))))))),((_tmp8DE[1]=Cyc_PP_line_doc(),((_tmp8DE[0]=Cyc_Absynpp_stmt2doc(
_tmp3B2,0),Cyc_PP_cat(_tag_dyneither(_tmp8DE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{struct Cyc_PP_Doc*_tmp8E3[3];s=((_tmp8E3[2]=Cyc_Absynpp_stmt2doc(_tmp3B3,
stmtexp),((_tmp8E3[1]=Cyc_PP_line_doc(),((_tmp8E3[0]=Cyc_Absynpp_stmt2doc(
_tmp3B2,0),Cyc_PP_cat(_tag_dyneither(_tmp8E3,sizeof(struct Cyc_PP_Doc*),3))))))));}}}
else{struct Cyc_PP_Doc*_tmp8E4[3];s=((_tmp8E4[2]=Cyc_Absynpp_stmt2doc(_tmp3B3,
stmtexp),((_tmp8E4[1]=Cyc_PP_line_doc(),((_tmp8E4[0]=Cyc_Absynpp_stmt2doc(
_tmp3B2,0),Cyc_PP_cat(_tag_dyneither(_tmp8E4,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL225;_LL22C: {struct Cyc_Absyn_Return_s_struct*_tmp3B4=(struct Cyc_Absyn_Return_s_struct*)
_tmp3AD;if(_tmp3B4->tag != 3)goto _LL22E;else{_tmp3B5=_tmp3B4->f1;}}_LL22D: if(
_tmp3B5 == 0){const char*_tmp8E5;s=Cyc_PP_text(((_tmp8E5="return;",_tag_dyneither(
_tmp8E5,sizeof(char),8))));}else{const char*_tmp8EA;const char*_tmp8E9;struct Cyc_PP_Doc*
_tmp8E8[3];s=((_tmp8E8[2]=Cyc_PP_text(((_tmp8E9=";",_tag_dyneither(_tmp8E9,
sizeof(char),2)))),((_tmp8E8[1]=_tmp3B5 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3B5),((_tmp8E8[0]=Cyc_PP_text(((_tmp8EA="return ",
_tag_dyneither(_tmp8EA,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8E8,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL225;_LL22E: {struct Cyc_Absyn_IfThenElse_s_struct*
_tmp3B6=(struct Cyc_Absyn_IfThenElse_s_struct*)_tmp3AD;if(_tmp3B6->tag != 4)goto
_LL230;else{_tmp3B7=_tmp3B6->f1;_tmp3B8=_tmp3B6->f2;_tmp3B9=_tmp3B6->f3;}}_LL22F: {
int print_else;{void*_tmp3F2=_tmp3B9->r;_LL24B: {struct Cyc_Absyn_Skip_s_struct*
_tmp3F3=(struct Cyc_Absyn_Skip_s_struct*)_tmp3F2;if(_tmp3F3->tag != 0)goto _LL24D;}
_LL24C: print_else=0;goto _LL24A;_LL24D:;_LL24E: print_else=1;goto _LL24A;_LL24A:;}{
const char*_tmp8F9;const char*_tmp8F8;struct Cyc_PP_Doc*_tmp8F7[2];struct Cyc_PP_Doc*
_tmp8F6[6];struct Cyc_PP_Doc*_tmp8F5[2];const char*_tmp8F4;struct Cyc_PP_Doc*
_tmp8F3[8];s=((_tmp8F3[7]=print_else?(_tmp8F6[5]=Cyc_Absynpp_rb(),((_tmp8F6[4]=
Cyc_PP_line_doc(),((_tmp8F6[3]=Cyc_PP_nest(2,((_tmp8F5[1]=Cyc_Absynpp_stmt2doc(
_tmp3B9,0),((_tmp8F5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8F5,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8F6[2]=Cyc_Absynpp_lb(),((_tmp8F6[1]=
Cyc_PP_text(((_tmp8F4="else ",_tag_dyneither(_tmp8F4,sizeof(char),6)))),((
_tmp8F6[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8F6,sizeof(struct Cyc_PP_Doc*),
6))))))))))))): Cyc_PP_nil_doc(),((_tmp8F3[6]=Cyc_Absynpp_rb(),((_tmp8F3[5]=Cyc_PP_line_doc(),((
_tmp8F3[4]=Cyc_PP_nest(2,((_tmp8F7[1]=Cyc_Absynpp_stmt2doc(_tmp3B8,0),((_tmp8F7[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8F7,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp8F3[3]=Cyc_Absynpp_lb(),((_tmp8F3[2]=Cyc_PP_text(((_tmp8F8=") ",
_tag_dyneither(_tmp8F8,sizeof(char),3)))),((_tmp8F3[1]=Cyc_Absynpp_exp2doc(
_tmp3B7),((_tmp8F3[0]=Cyc_PP_text(((_tmp8F9="if (",_tag_dyneither(_tmp8F9,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL225;}_LL230: {struct Cyc_Absyn_While_s_struct*_tmp3BA=(struct Cyc_Absyn_While_s_struct*)
_tmp3AD;if(_tmp3BA->tag != 5)goto _LL232;else{_tmp3BB=_tmp3BA->f1;_tmp3BC=_tmp3BB.f1;
_tmp3BD=_tmp3BA->f2;}}_LL231:{const char*_tmp900;const char*_tmp8FF;struct Cyc_PP_Doc*
_tmp8FE[2];struct Cyc_PP_Doc*_tmp8FD[7];s=((_tmp8FD[6]=Cyc_Absynpp_rb(),((_tmp8FD[
5]=Cyc_PP_line_doc(),((_tmp8FD[4]=Cyc_PP_nest(2,((_tmp8FE[1]=Cyc_Absynpp_stmt2doc(
_tmp3BD,0),((_tmp8FE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8FE,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8FD[3]=Cyc_Absynpp_lb(),((_tmp8FD[2]=
Cyc_PP_text(((_tmp8FF=") ",_tag_dyneither(_tmp8FF,sizeof(char),3)))),((_tmp8FD[1]=
Cyc_Absynpp_exp2doc(_tmp3BC),((_tmp8FD[0]=Cyc_PP_text(((_tmp900="while (",
_tag_dyneither(_tmp900,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8FD,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL225;_LL232: {struct Cyc_Absyn_Break_s_struct*
_tmp3BE=(struct Cyc_Absyn_Break_s_struct*)_tmp3AD;if(_tmp3BE->tag != 6)goto _LL234;}
_LL233:{const char*_tmp901;s=Cyc_PP_text(((_tmp901="break;",_tag_dyneither(
_tmp901,sizeof(char),7))));}goto _LL225;_LL234: {struct Cyc_Absyn_Continue_s_struct*
_tmp3BF=(struct Cyc_Absyn_Continue_s_struct*)_tmp3AD;if(_tmp3BF->tag != 7)goto
_LL236;}_LL235:{const char*_tmp902;s=Cyc_PP_text(((_tmp902="continue;",
_tag_dyneither(_tmp902,sizeof(char),10))));}goto _LL225;_LL236: {struct Cyc_Absyn_Goto_s_struct*
_tmp3C0=(struct Cyc_Absyn_Goto_s_struct*)_tmp3AD;if(_tmp3C0->tag != 8)goto _LL238;
else{_tmp3C1=_tmp3C0->f1;}}_LL237:{const char*_tmp906;void*_tmp905[1];struct Cyc_String_pa_struct
_tmp904;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp904.tag=0,((_tmp904.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmp3C1),((_tmp905[0]=& _tmp904,Cyc_aprintf(((
_tmp906="goto %s;",_tag_dyneither(_tmp906,sizeof(char),9))),_tag_dyneither(
_tmp905,sizeof(void*),1)))))))));}goto _LL225;_LL238: {struct Cyc_Absyn_For_s_struct*
_tmp3C2=(struct Cyc_Absyn_For_s_struct*)_tmp3AD;if(_tmp3C2->tag != 9)goto _LL23A;
else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C2->f2;_tmp3C5=_tmp3C4.f1;_tmp3C6=_tmp3C2->f3;
_tmp3C7=_tmp3C6.f1;_tmp3C8=_tmp3C2->f4;}}_LL239:{const char*_tmp911;const char*
_tmp910;const char*_tmp90F;const char*_tmp90E;struct Cyc_PP_Doc*_tmp90D[2];struct
Cyc_PP_Doc*_tmp90C[11];s=((_tmp90C[10]=Cyc_Absynpp_rb(),((_tmp90C[9]=Cyc_PP_line_doc(),((
_tmp90C[8]=Cyc_PP_nest(2,((_tmp90D[1]=Cyc_Absynpp_stmt2doc(_tmp3C8,0),((_tmp90D[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp90D,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp90C[7]=Cyc_Absynpp_lb(),((_tmp90C[6]=Cyc_PP_text(((_tmp90E=") ",
_tag_dyneither(_tmp90E,sizeof(char),3)))),((_tmp90C[5]=Cyc_Absynpp_exp2doc(
_tmp3C7),((_tmp90C[4]=Cyc_PP_text(((_tmp90F="; ",_tag_dyneither(_tmp90F,sizeof(
char),3)))),((_tmp90C[3]=Cyc_Absynpp_exp2doc(_tmp3C5),((_tmp90C[2]=Cyc_PP_text(((
_tmp910="; ",_tag_dyneither(_tmp910,sizeof(char),3)))),((_tmp90C[1]=Cyc_Absynpp_exp2doc(
_tmp3C3),((_tmp90C[0]=Cyc_PP_text(((_tmp911="for(",_tag_dyneither(_tmp911,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp90C,sizeof(struct Cyc_PP_Doc*),
11))))))))))))))))))))))));}goto _LL225;_LL23A: {struct Cyc_Absyn_Switch_s_struct*
_tmp3C9=(struct Cyc_Absyn_Switch_s_struct*)_tmp3AD;if(_tmp3C9->tag != 10)goto
_LL23C;else{_tmp3CA=_tmp3C9->f1;_tmp3CB=_tmp3C9->f2;}}_LL23B:{const char*_tmp916;
const char*_tmp915;struct Cyc_PP_Doc*_tmp914[8];s=((_tmp914[7]=Cyc_Absynpp_rb(),((
_tmp914[6]=Cyc_PP_line_doc(),((_tmp914[5]=Cyc_Absynpp_switchclauses2doc(_tmp3CB),((
_tmp914[4]=Cyc_PP_line_doc(),((_tmp914[3]=Cyc_Absynpp_lb(),((_tmp914[2]=Cyc_PP_text(((
_tmp915=") ",_tag_dyneither(_tmp915,sizeof(char),3)))),((_tmp914[1]=Cyc_Absynpp_exp2doc(
_tmp3CA),((_tmp914[0]=Cyc_PP_text(((_tmp916="switch (",_tag_dyneither(_tmp916,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp914,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL225;_LL23C: {struct Cyc_Absyn_Fallthru_s_struct*_tmp3CC=(struct Cyc_Absyn_Fallthru_s_struct*)
_tmp3AD;if(_tmp3CC->tag != 11)goto _LL23E;else{_tmp3CD=_tmp3CC->f1;if(_tmp3CD != 0)
goto _LL23E;}}_LL23D:{const char*_tmp917;s=Cyc_PP_text(((_tmp917="fallthru;",
_tag_dyneither(_tmp917,sizeof(char),10))));}goto _LL225;_LL23E: {struct Cyc_Absyn_Fallthru_s_struct*
_tmp3CE=(struct Cyc_Absyn_Fallthru_s_struct*)_tmp3AD;if(_tmp3CE->tag != 11)goto
_LL240;else{_tmp3CF=_tmp3CE->f1;}}_LL23F:{const char*_tmp91C;const char*_tmp91B;
struct Cyc_PP_Doc*_tmp91A[3];s=((_tmp91A[2]=Cyc_PP_text(((_tmp91B=");",
_tag_dyneither(_tmp91B,sizeof(char),3)))),((_tmp91A[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp3CF),((_tmp91A[0]=Cyc_PP_text(((_tmp91C="fallthru(",_tag_dyneither(
_tmp91C,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp91A,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL225;_LL240: {struct Cyc_Absyn_Decl_s_struct*_tmp3D0=(struct Cyc_Absyn_Decl_s_struct*)
_tmp3AD;if(_tmp3D0->tag != 12)goto _LL242;else{_tmp3D1=_tmp3D0->f1;_tmp3D2=_tmp3D0->f2;}}
_LL241:{struct Cyc_PP_Doc*_tmp91D[3];s=((_tmp91D[2]=Cyc_Absynpp_stmt2doc(_tmp3D2,
stmtexp),((_tmp91D[1]=Cyc_PP_line_doc(),((_tmp91D[0]=Cyc_Absynpp_decl2doc(
_tmp3D1),Cyc_PP_cat(_tag_dyneither(_tmp91D,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL225;_LL242: {struct Cyc_Absyn_Label_s_struct*_tmp3D3=(struct Cyc_Absyn_Label_s_struct*)
_tmp3AD;if(_tmp3D3->tag != 13)goto _LL244;else{_tmp3D4=_tmp3D3->f1;_tmp3D5=_tmp3D3->f2;}}
_LL243: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3D5)){if(
stmtexp){const char*_tmp922;const char*_tmp921;struct Cyc_PP_Doc*_tmp920[8];s=((
_tmp920[7]=Cyc_PP_text(((_tmp921=");",_tag_dyneither(_tmp921,sizeof(char),3)))),((
_tmp920[6]=Cyc_Absynpp_rb(),((_tmp920[5]=Cyc_PP_line_doc(),((_tmp920[4]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3D5,1)),((_tmp920[3]=Cyc_PP_line_doc(),((_tmp920[2]=
Cyc_Absynpp_lb(),((_tmp920[1]=Cyc_PP_text(((_tmp922=": (",_tag_dyneither(_tmp922,
sizeof(char),4)))),((_tmp920[0]=Cyc_PP_textptr(_tmp3D4),Cyc_PP_cat(
_tag_dyneither(_tmp920,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const
char*_tmp925;struct Cyc_PP_Doc*_tmp924[7];s=((_tmp924[6]=Cyc_Absynpp_rb(),((
_tmp924[5]=Cyc_PP_line_doc(),((_tmp924[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp3D5,0)),((_tmp924[3]=Cyc_PP_line_doc(),((_tmp924[2]=Cyc_Absynpp_lb(),((
_tmp924[1]=Cyc_PP_text(((_tmp925=": ",_tag_dyneither(_tmp925,sizeof(char),3)))),((
_tmp924[0]=Cyc_PP_textptr(_tmp3D4),Cyc_PP_cat(_tag_dyneither(_tmp924,sizeof(
struct Cyc_PP_Doc*),7))))))))))))))));}}else{const char*_tmp928;struct Cyc_PP_Doc*
_tmp927[3];s=((_tmp927[2]=Cyc_Absynpp_stmt2doc(_tmp3D5,stmtexp),((_tmp927[1]=Cyc_PP_text(((
_tmp928=": ",_tag_dyneither(_tmp928,sizeof(char),3)))),((_tmp927[0]=Cyc_PP_textptr(
_tmp3D4),Cyc_PP_cat(_tag_dyneither(_tmp927,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL225;_LL244: {struct Cyc_Absyn_Do_s_struct*_tmp3D6=(struct Cyc_Absyn_Do_s_struct*)
_tmp3AD;if(_tmp3D6->tag != 14)goto _LL246;else{_tmp3D7=_tmp3D6->f1;_tmp3D8=_tmp3D6->f2;
_tmp3D9=_tmp3D8.f1;}}_LL245:{const char*_tmp92F;const char*_tmp92E;const char*
_tmp92D;struct Cyc_PP_Doc*_tmp92C[9];s=((_tmp92C[8]=Cyc_PP_text(((_tmp92D=");",
_tag_dyneither(_tmp92D,sizeof(char),3)))),((_tmp92C[7]=Cyc_Absynpp_exp2doc(
_tmp3D9),((_tmp92C[6]=Cyc_PP_text(((_tmp92E=" while (",_tag_dyneither(_tmp92E,
sizeof(char),9)))),((_tmp92C[5]=Cyc_Absynpp_rb(),((_tmp92C[4]=Cyc_PP_line_doc(),((
_tmp92C[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3D7,0)),((_tmp92C[2]=Cyc_PP_line_doc(),((
_tmp92C[1]=Cyc_Absynpp_lb(),((_tmp92C[0]=Cyc_PP_text(((_tmp92F="do ",
_tag_dyneither(_tmp92F,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp92C,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL225;_LL246: {struct Cyc_Absyn_TryCatch_s_struct*
_tmp3DA=(struct Cyc_Absyn_TryCatch_s_struct*)_tmp3AD;if(_tmp3DA->tag != 15)goto
_LL248;else{_tmp3DB=_tmp3DA->f1;_tmp3DC=_tmp3DA->f2;}}_LL247:{const char*_tmp934;
const char*_tmp933;struct Cyc_PP_Doc*_tmp932[12];s=((_tmp932[11]=Cyc_Absynpp_rb(),((
_tmp932[10]=Cyc_PP_line_doc(),((_tmp932[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp3DC)),((_tmp932[8]=Cyc_PP_line_doc(),((_tmp932[7]=Cyc_Absynpp_lb(),((_tmp932[
6]=Cyc_PP_text(((_tmp933=" catch ",_tag_dyneither(_tmp933,sizeof(char),8)))),((
_tmp932[5]=Cyc_Absynpp_rb(),((_tmp932[4]=Cyc_PP_line_doc(),((_tmp932[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp3DB,0)),((_tmp932[2]=Cyc_PP_line_doc(),((_tmp932[1]=
Cyc_Absynpp_lb(),((_tmp932[0]=Cyc_PP_text(((_tmp934="try ",_tag_dyneither(
_tmp934,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp932,sizeof(struct Cyc_PP_Doc*),
12))))))))))))))))))))))))));}goto _LL225;_LL248: {struct Cyc_Absyn_ResetRegion_s_struct*
_tmp3DD=(struct Cyc_Absyn_ResetRegion_s_struct*)_tmp3AD;if(_tmp3DD->tag != 16)goto
_LL225;else{_tmp3DE=_tmp3DD->f1;}}_LL249:{const char*_tmp939;const char*_tmp938;
struct Cyc_PP_Doc*_tmp937[3];s=((_tmp937[2]=Cyc_PP_text(((_tmp938=");",
_tag_dyneither(_tmp938,sizeof(char),3)))),((_tmp937[1]=Cyc_Absynpp_exp2doc(
_tmp3DE),((_tmp937[0]=Cyc_PP_text(((_tmp939="reset_region(",_tag_dyneither(
_tmp939,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp937,sizeof(struct Cyc_PP_Doc*),
3))))))));}goto _LL225;_LL225:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp423=p->r;enum Cyc_Absyn_Sign
_tmp427;int _tmp428;char _tmp42A;struct _dyneither_ptr _tmp42C;struct Cyc_Absyn_Vardecl*
_tmp42E;struct Cyc_Absyn_Pat*_tmp42F;struct Cyc_Absyn_Pat _tmp430;void*_tmp431;
struct Cyc_Absyn_Vardecl*_tmp434;struct Cyc_Absyn_Pat*_tmp435;struct Cyc_Absyn_Tvar*
_tmp437;struct Cyc_Absyn_Vardecl*_tmp438;struct Cyc_List_List*_tmp43A;int _tmp43B;
struct Cyc_Absyn_Pat*_tmp43D;struct Cyc_Absyn_Vardecl*_tmp43F;struct Cyc_Absyn_Pat*
_tmp440;struct Cyc_Absyn_Pat _tmp441;void*_tmp442;struct Cyc_Absyn_Vardecl*_tmp445;
struct Cyc_Absyn_Pat*_tmp446;struct _tuple0*_tmp448;struct _tuple0*_tmp44A;struct
Cyc_List_List*_tmp44B;int _tmp44C;struct Cyc_Absyn_AggrInfo*_tmp44E;struct Cyc_Absyn_AggrInfo
_tmp44F;union Cyc_Absyn_AggrInfoU _tmp450;struct Cyc_List_List*_tmp451;struct Cyc_List_List*
_tmp452;int _tmp453;struct Cyc_Absyn_AggrInfo*_tmp455;struct Cyc_List_List*_tmp456;
struct Cyc_List_List*_tmp457;int _tmp458;struct Cyc_Absyn_Enumfield*_tmp45A;struct
Cyc_Absyn_Enumfield*_tmp45C;struct Cyc_Absyn_Datatypefield*_tmp45E;struct Cyc_List_List*
_tmp45F;struct Cyc_Absyn_Datatypefield*_tmp461;struct Cyc_List_List*_tmp462;int
_tmp463;struct Cyc_Absyn_Exp*_tmp465;_LL250: {struct Cyc_Absyn_Wild_p_struct*
_tmp424=(struct Cyc_Absyn_Wild_p_struct*)_tmp423;if(_tmp424->tag != 0)goto _LL252;}
_LL251:{const char*_tmp93A;s=Cyc_PP_text(((_tmp93A="_",_tag_dyneither(_tmp93A,
sizeof(char),2))));}goto _LL24F;_LL252: {struct Cyc_Absyn_Null_p_struct*_tmp425=(
struct Cyc_Absyn_Null_p_struct*)_tmp423;if(_tmp425->tag != 8)goto _LL254;}_LL253:{
const char*_tmp93B;s=Cyc_PP_text(((_tmp93B="NULL",_tag_dyneither(_tmp93B,sizeof(
char),5))));}goto _LL24F;_LL254: {struct Cyc_Absyn_Int_p_struct*_tmp426=(struct Cyc_Absyn_Int_p_struct*)
_tmp423;if(_tmp426->tag != 9)goto _LL256;else{_tmp427=_tmp426->f1;_tmp428=_tmp426->f2;}}
_LL255: if(_tmp427 != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){const char*_tmp93F;
void*_tmp93E[1];struct Cyc_Int_pa_struct _tmp93D;s=Cyc_PP_text((struct
_dyneither_ptr)((_tmp93D.tag=1,((_tmp93D.f1=(unsigned long)_tmp428,((_tmp93E[0]=&
_tmp93D,Cyc_aprintf(((_tmp93F="%d",_tag_dyneither(_tmp93F,sizeof(char),3))),
_tag_dyneither(_tmp93E,sizeof(void*),1)))))))));}else{const char*_tmp943;void*
_tmp942[1];struct Cyc_Int_pa_struct _tmp941;s=Cyc_PP_text((struct _dyneither_ptr)((
_tmp941.tag=1,((_tmp941.f1=(unsigned int)_tmp428,((_tmp942[0]=& _tmp941,Cyc_aprintf(((
_tmp943="%u",_tag_dyneither(_tmp943,sizeof(char),3))),_tag_dyneither(_tmp942,
sizeof(void*),1)))))))));}goto _LL24F;_LL256: {struct Cyc_Absyn_Char_p_struct*
_tmp429=(struct Cyc_Absyn_Char_p_struct*)_tmp423;if(_tmp429->tag != 10)goto _LL258;
else{_tmp42A=_tmp429->f1;}}_LL257:{const char*_tmp947;void*_tmp946[1];struct Cyc_String_pa_struct
_tmp945;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp945.tag=0,((_tmp945.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp42A)),((
_tmp946[0]=& _tmp945,Cyc_aprintf(((_tmp947="'%s'",_tag_dyneither(_tmp947,sizeof(
char),5))),_tag_dyneither(_tmp946,sizeof(void*),1)))))))));}goto _LL24F;_LL258: {
struct Cyc_Absyn_Float_p_struct*_tmp42B=(struct Cyc_Absyn_Float_p_struct*)_tmp423;
if(_tmp42B->tag != 11)goto _LL25A;else{_tmp42C=_tmp42B->f1;}}_LL259: s=Cyc_PP_text(
_tmp42C);goto _LL24F;_LL25A: {struct Cyc_Absyn_Var_p_struct*_tmp42D=(struct Cyc_Absyn_Var_p_struct*)
_tmp423;if(_tmp42D->tag != 1)goto _LL25C;else{_tmp42E=_tmp42D->f1;_tmp42F=_tmp42D->f2;
_tmp430=*_tmp42F;_tmp431=_tmp430.r;{struct Cyc_Absyn_Wild_p_struct*_tmp432=(
struct Cyc_Absyn_Wild_p_struct*)_tmp431;if(_tmp432->tag != 0)goto _LL25C;};}}_LL25B:
s=Cyc_Absynpp_qvar2doc(_tmp42E->name);goto _LL24F;_LL25C: {struct Cyc_Absyn_Var_p_struct*
_tmp433=(struct Cyc_Absyn_Var_p_struct*)_tmp423;if(_tmp433->tag != 1)goto _LL25E;
else{_tmp434=_tmp433->f1;_tmp435=_tmp433->f2;}}_LL25D:{const char*_tmp94A;struct
Cyc_PP_Doc*_tmp949[3];s=((_tmp949[2]=Cyc_Absynpp_pat2doc(_tmp435),((_tmp949[1]=
Cyc_PP_text(((_tmp94A=" as ",_tag_dyneither(_tmp94A,sizeof(char),5)))),((_tmp949[
0]=Cyc_Absynpp_qvar2doc(_tmp434->name),Cyc_PP_cat(_tag_dyneither(_tmp949,sizeof(
struct Cyc_PP_Doc*),3))))))));}goto _LL24F;_LL25E: {struct Cyc_Absyn_TagInt_p_struct*
_tmp436=(struct Cyc_Absyn_TagInt_p_struct*)_tmp423;if(_tmp436->tag != 3)goto _LL260;
else{_tmp437=_tmp436->f1;_tmp438=_tmp436->f2;}}_LL25F:{const char*_tmp94F;const
char*_tmp94E;struct Cyc_PP_Doc*_tmp94D[4];s=((_tmp94D[3]=Cyc_PP_text(((_tmp94E=">",
_tag_dyneither(_tmp94E,sizeof(char),2)))),((_tmp94D[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(
_tmp437)),((_tmp94D[1]=Cyc_PP_text(((_tmp94F="<",_tag_dyneither(_tmp94F,sizeof(
char),2)))),((_tmp94D[0]=Cyc_Absynpp_qvar2doc(_tmp438->name),Cyc_PP_cat(
_tag_dyneither(_tmp94D,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL24F;_LL260: {
struct Cyc_Absyn_Tuple_p_struct*_tmp439=(struct Cyc_Absyn_Tuple_p_struct*)_tmp423;
if(_tmp439->tag != 4)goto _LL262;else{_tmp43A=_tmp439->f1;_tmp43B=_tmp439->f2;}}
_LL261:{const char*_tmp958;const char*_tmp957;const char*_tmp956;const char*_tmp955;
struct Cyc_PP_Doc*_tmp954[4];s=((_tmp954[3]=_tmp43B?Cyc_PP_text(((_tmp955=", ...)",
_tag_dyneither(_tmp955,sizeof(char),7)))): Cyc_PP_text(((_tmp956=")",
_tag_dyneither(_tmp956,sizeof(char),2)))),((_tmp954[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp957=",",_tag_dyneither(_tmp957,
sizeof(char),2))),_tmp43A),((_tmp954[1]=Cyc_PP_text(((_tmp958="(",_tag_dyneither(
_tmp958,sizeof(char),2)))),((_tmp954[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(
_tag_dyneither(_tmp954,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL24F;_LL262: {
struct Cyc_Absyn_Pointer_p_struct*_tmp43C=(struct Cyc_Absyn_Pointer_p_struct*)
_tmp423;if(_tmp43C->tag != 5)goto _LL264;else{_tmp43D=_tmp43C->f1;}}_LL263:{const
char*_tmp95B;struct Cyc_PP_Doc*_tmp95A[2];s=((_tmp95A[1]=Cyc_Absynpp_pat2doc(
_tmp43D),((_tmp95A[0]=Cyc_PP_text(((_tmp95B="&",_tag_dyneither(_tmp95B,sizeof(
char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp95A,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL24F;_LL264: {struct Cyc_Absyn_Reference_p_struct*_tmp43E=(struct Cyc_Absyn_Reference_p_struct*)
_tmp423;if(_tmp43E->tag != 2)goto _LL266;else{_tmp43F=_tmp43E->f1;_tmp440=_tmp43E->f2;
_tmp441=*_tmp440;_tmp442=_tmp441.r;{struct Cyc_Absyn_Wild_p_struct*_tmp443=(
struct Cyc_Absyn_Wild_p_struct*)_tmp442;if(_tmp443->tag != 0)goto _LL266;};}}_LL265:{
const char*_tmp95E;struct Cyc_PP_Doc*_tmp95D[2];s=((_tmp95D[1]=Cyc_Absynpp_qvar2doc(
_tmp43F->name),((_tmp95D[0]=Cyc_PP_text(((_tmp95E="*",_tag_dyneither(_tmp95E,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp95D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL24F;_LL266: {struct Cyc_Absyn_Reference_p_struct*_tmp444=(struct Cyc_Absyn_Reference_p_struct*)
_tmp423;if(_tmp444->tag != 2)goto _LL268;else{_tmp445=_tmp444->f1;_tmp446=_tmp444->f2;}}
_LL267:{const char*_tmp963;const char*_tmp962;struct Cyc_PP_Doc*_tmp961[4];s=((
_tmp961[3]=Cyc_Absynpp_pat2doc(_tmp446),((_tmp961[2]=Cyc_PP_text(((_tmp962=" as ",
_tag_dyneither(_tmp962,sizeof(char),5)))),((_tmp961[1]=Cyc_Absynpp_qvar2doc(
_tmp445->name),((_tmp961[0]=Cyc_PP_text(((_tmp963="*",_tag_dyneither(_tmp963,
sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp961,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL24F;_LL268: {struct Cyc_Absyn_UnknownId_p_struct*_tmp447=(struct Cyc_Absyn_UnknownId_p_struct*)
_tmp423;if(_tmp447->tag != 14)goto _LL26A;else{_tmp448=_tmp447->f1;}}_LL269: s=Cyc_Absynpp_qvar2doc(
_tmp448);goto _LL24F;_LL26A: {struct Cyc_Absyn_UnknownCall_p_struct*_tmp449=(
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp423;if(_tmp449->tag != 15)goto _LL26C;
else{_tmp44A=_tmp449->f1;_tmp44B=_tmp449->f2;_tmp44C=_tmp449->f3;}}_LL26B: {
const char*_tmp965;const char*_tmp964;struct _dyneither_ptr term=_tmp44C?(_tmp965=", ...)",
_tag_dyneither(_tmp965,sizeof(char),7)):((_tmp964=")",_tag_dyneither(_tmp964,
sizeof(char),2)));{const char*_tmp96A;const char*_tmp969;struct Cyc_PP_Doc*_tmp968[
2];s=((_tmp968[1]=Cyc_PP_group(((_tmp96A="(",_tag_dyneither(_tmp96A,sizeof(char),
2))),term,((_tmp969=",",_tag_dyneither(_tmp969,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp44B)),((_tmp968[0]=Cyc_Absynpp_qvar2doc(_tmp44A),Cyc_PP_cat(
_tag_dyneither(_tmp968,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL24F;}_LL26C: {
struct Cyc_Absyn_Aggr_p_struct*_tmp44D=(struct Cyc_Absyn_Aggr_p_struct*)_tmp423;
if(_tmp44D->tag != 6)goto _LL26E;else{_tmp44E=_tmp44D->f1;if(_tmp44E == 0)goto
_LL26E;_tmp44F=*_tmp44E;_tmp450=_tmp44F.aggr_info;_tmp451=_tmp44D->f2;_tmp452=
_tmp44D->f3;_tmp453=_tmp44D->f4;}}_LL26D: {const char*_tmp96C;const char*_tmp96B;
struct _dyneither_ptr term=_tmp453?(_tmp96C=", ...}",_tag_dyneither(_tmp96C,
sizeof(char),7)):((_tmp96B="}",_tag_dyneither(_tmp96B,sizeof(char),2)));struct
_tuple0*_tmp488;struct _tuple9 _tmp487=Cyc_Absyn_aggr_kinded_name(_tmp450);_tmp488=
_tmp487.f2;{const char*_tmp977;const char*_tmp976;const char*_tmp975;const char*
_tmp974;const char*_tmp973;struct Cyc_PP_Doc*_tmp972[4];s=((_tmp972[3]=Cyc_PP_group(((
_tmp974="",_tag_dyneither(_tmp974,sizeof(char),1))),term,((_tmp973=",",
_tag_dyneither(_tmp973,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp452)),((_tmp972[2]=Cyc_PP_egroup(((_tmp977="[",_tag_dyneither(_tmp977,
sizeof(char),2))),((_tmp976="]",_tag_dyneither(_tmp976,sizeof(char),2))),((
_tmp975=",",_tag_dyneither(_tmp975,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp451))),((_tmp972[1]=
Cyc_Absynpp_lb(),((_tmp972[0]=Cyc_Absynpp_qvar2doc(_tmp488),Cyc_PP_cat(
_tag_dyneither(_tmp972,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL24F;}_LL26E: {
struct Cyc_Absyn_Aggr_p_struct*_tmp454=(struct Cyc_Absyn_Aggr_p_struct*)_tmp423;
if(_tmp454->tag != 6)goto _LL270;else{_tmp455=_tmp454->f1;if(_tmp455 != 0)goto
_LL270;_tmp456=_tmp454->f2;_tmp457=_tmp454->f3;_tmp458=_tmp454->f4;}}_LL26F: {
const char*_tmp979;const char*_tmp978;struct _dyneither_ptr term=_tmp458?(_tmp979=", ...}",
_tag_dyneither(_tmp979,sizeof(char),7)):((_tmp978="}",_tag_dyneither(_tmp978,
sizeof(char),2)));{const char*_tmp984;const char*_tmp983;const char*_tmp982;const
char*_tmp981;const char*_tmp980;struct Cyc_PP_Doc*_tmp97F[3];s=((_tmp97F[2]=Cyc_PP_group(((
_tmp981="",_tag_dyneither(_tmp981,sizeof(char),1))),term,((_tmp980=",",
_tag_dyneither(_tmp980,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp457)),((_tmp97F[1]=Cyc_PP_egroup(((_tmp984="[",_tag_dyneither(_tmp984,
sizeof(char),2))),((_tmp983="]",_tag_dyneither(_tmp983,sizeof(char),2))),((
_tmp982=",",_tag_dyneither(_tmp982,sizeof(char),2))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp456))),((_tmp97F[0]=
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp97F,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL24F;}_LL270: {struct Cyc_Absyn_Enum_p_struct*_tmp459=(struct Cyc_Absyn_Enum_p_struct*)
_tmp423;if(_tmp459->tag != 12)goto _LL272;else{_tmp45A=_tmp459->f2;}}_LL271:
_tmp45C=_tmp45A;goto _LL273;_LL272: {struct Cyc_Absyn_AnonEnum_p_struct*_tmp45B=(
struct Cyc_Absyn_AnonEnum_p_struct*)_tmp423;if(_tmp45B->tag != 13)goto _LL274;else{
_tmp45C=_tmp45B->f2;}}_LL273: s=Cyc_Absynpp_qvar2doc(_tmp45C->name);goto _LL24F;
_LL274: {struct Cyc_Absyn_Datatype_p_struct*_tmp45D=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp423;if(_tmp45D->tag != 7)goto _LL276;else{_tmp45E=_tmp45D->f2;_tmp45F=_tmp45D->f3;
if(_tmp45F != 0)goto _LL276;}}_LL275: s=Cyc_Absynpp_qvar2doc(_tmp45E->name);goto
_LL24F;_LL276: {struct Cyc_Absyn_Datatype_p_struct*_tmp460=(struct Cyc_Absyn_Datatype_p_struct*)
_tmp423;if(_tmp460->tag != 7)goto _LL278;else{_tmp461=_tmp460->f2;_tmp462=_tmp460->f3;
_tmp463=_tmp460->f4;}}_LL277: {const char*_tmp986;const char*_tmp985;struct
_dyneither_ptr term=_tmp463?(_tmp986=", ...)",_tag_dyneither(_tmp986,sizeof(char),
7)):((_tmp985=")",_tag_dyneither(_tmp985,sizeof(char),2)));{const char*_tmp98B;
const char*_tmp98A;struct Cyc_PP_Doc*_tmp989[2];s=((_tmp989[1]=Cyc_PP_egroup(((
_tmp98B="(",_tag_dyneither(_tmp98B,sizeof(char),2))),term,((_tmp98A=",",
_tag_dyneither(_tmp98A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp462)),((_tmp989[0]=Cyc_Absynpp_qvar2doc(_tmp461->name),Cyc_PP_cat(
_tag_dyneither(_tmp989,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL24F;}_LL278: {
struct Cyc_Absyn_Exp_p_struct*_tmp464=(struct Cyc_Absyn_Exp_p_struct*)_tmp423;if(
_tmp464->tag != 16)goto _LL24F;else{_tmp465=_tmp464->f1;}}_LL279: s=Cyc_Absynpp_exp2doc(
_tmp465);goto _LL24F;_LL24F:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct
_tuple13*dp){const char*_tmp992;const char*_tmp991;const char*_tmp990;struct Cyc_PP_Doc*
_tmp98F[2];return(_tmp98F[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp98F[0]=Cyc_PP_egroup(((
_tmp992="",_tag_dyneither(_tmp992,sizeof(char),1))),((_tmp991="=",_tag_dyneither(
_tmp991,sizeof(char),2))),((_tmp990="=",_tag_dyneither(_tmp990,sizeof(char),2))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp98F,sizeof(
struct Cyc_PP_Doc*),2)))));}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct
Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0  && (c->pattern)->r == (void*)&
Cyc_Absyn_Wild_p_val){const char*_tmp997;struct Cyc_PP_Doc*_tmp996[2];struct Cyc_PP_Doc*
_tmp995[2];return(_tmp995[1]=Cyc_PP_nest(2,((_tmp996[1]=Cyc_Absynpp_stmt2doc(c->body,
0),((_tmp996[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp996,sizeof(
struct Cyc_PP_Doc*),2))))))),((_tmp995[0]=Cyc_PP_text(((_tmp997="default: ",
_tag_dyneither(_tmp997,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp995,
sizeof(struct Cyc_PP_Doc*),2)))));}else{if(c->where_clause == 0){const char*_tmp99E;
const char*_tmp99D;struct Cyc_PP_Doc*_tmp99C[2];struct Cyc_PP_Doc*_tmp99B[4];return(
_tmp99B[3]=Cyc_PP_nest(2,((_tmp99C[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp99C[
0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp99C,sizeof(struct Cyc_PP_Doc*),
2))))))),((_tmp99B[2]=Cyc_PP_text(((_tmp99D=": ",_tag_dyneither(_tmp99D,sizeof(
char),3)))),((_tmp99B[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp99B[0]=Cyc_PP_text(((
_tmp99E="case ",_tag_dyneither(_tmp99E,sizeof(char),6)))),Cyc_PP_cat(
_tag_dyneither(_tmp99B,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{const char*
_tmp9A7;const char*_tmp9A6;const char*_tmp9A5;struct Cyc_PP_Doc*_tmp9A4[2];struct
Cyc_PP_Doc*_tmp9A3[6];return(_tmp9A3[5]=Cyc_PP_nest(2,((_tmp9A4[1]=Cyc_Absynpp_stmt2doc(
c->body,0),((_tmp9A4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9A4,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9A3[4]=Cyc_PP_text(((_tmp9A5=": ",
_tag_dyneither(_tmp9A5,sizeof(char),3)))),((_tmp9A3[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp9A3[2]=Cyc_PP_text(((
_tmp9A6=" && ",_tag_dyneither(_tmp9A6,sizeof(char),5)))),((_tmp9A3[1]=Cyc_Absynpp_pat2doc(
c->pattern),((_tmp9A3[0]=Cyc_PP_text(((_tmp9A7="case ",_tag_dyneither(_tmp9A7,
sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9A3,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){const char*
_tmp9A8;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((
_tmp9A8="",_tag_dyneither(_tmp9A8,sizeof(char),1))),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);
else{const char*_tmp9AB;struct Cyc_PP_Doc*_tmp9AA[3];return(_tmp9AA[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp9AA[1]=Cyc_PP_text(((_tmp9AB=" = ",
_tag_dyneither(_tmp9AB,sizeof(char),4)))),((_tmp9AA[0]=Cyc_Absynpp_qvar2doc(f->name),
Cyc_PP_cat(_tag_dyneither(_tmp9AA,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){const char*_tmp9AC;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((
_tmp9AC=",",_tag_dyneither(_tmp9AC,sizeof(char),2))),fs);}static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){const char*
_tmp9AD;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((
_tmp9AD=",",_tag_dyneither(_tmp9AD,sizeof(char),2))),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp4B4;enum Cyc_Absyn_Scope
_tmp4B5;struct _tuple0*_tmp4B6;struct Cyc_Absyn_Tqual _tmp4B7;void*_tmp4B8;struct
Cyc_Absyn_Exp*_tmp4B9;struct Cyc_List_List*_tmp4BA;struct Cyc_Absyn_Vardecl*
_tmp4B3=vd;_tmp4B4=*_tmp4B3;_tmp4B5=_tmp4B4.sc;_tmp4B6=_tmp4B4.name;_tmp4B7=
_tmp4B4.tq;_tmp4B8=_tmp4B4.type;_tmp4B9=_tmp4B4.initializer;_tmp4BA=_tmp4B4.attributes;{
struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4B6);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4BA);struct Cyc_PP_Doc*
beforenamedoc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL27A:
beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL27B:{void*_tmp4BB=Cyc_Tcutil_compress(
_tmp4B8);struct Cyc_Absyn_FnInfo _tmp4BD;struct Cyc_List_List*_tmp4BE;_LL27E: {
struct Cyc_Absyn_FnType_struct*_tmp4BC=(struct Cyc_Absyn_FnType_struct*)_tmp4BB;
if(_tmp4BC->tag != 10)goto _LL280;else{_tmp4BD=_tmp4BC->f1;_tmp4BE=_tmp4BD.attributes;}}
_LL27F: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4BE);goto _LL27D;_LL280:;_LL281:
beforenamedoc=Cyc_PP_nil_doc();goto _LL27D;_LL27D:;}break;}{struct Cyc_PP_Doc*
tmp_doc;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL282: tmp_doc=Cyc_PP_nil_doc();
break;case Cyc_Cyclone_Vc_c: _LL283: tmp_doc=attsdoc;break;}{struct Cyc_Core_Opt*
_tmp9BA;struct Cyc_PP_Doc*_tmp9B9[2];struct Cyc_PP_Doc*_tmp9B8[2];const char*
_tmp9B7;const char*_tmp9B6;struct Cyc_PP_Doc*_tmp9B5[5];s=((_tmp9B5[4]=Cyc_PP_text(((
_tmp9B6=";",_tag_dyneither(_tmp9B6,sizeof(char),2)))),((_tmp9B5[3]=_tmp4B9 == 0?
Cyc_PP_nil_doc():((_tmp9B8[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4B9),((
_tmp9B8[0]=Cyc_PP_text(((_tmp9B7=" = ",_tag_dyneither(_tmp9B7,sizeof(char),4)))),
Cyc_PP_cat(_tag_dyneither(_tmp9B8,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9B5[2]=
Cyc_Absynpp_tqtd2doc(_tmp4B7,_tmp4B8,((_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((
_tmp9BA->v=((_tmp9B9[1]=sn,((_tmp9B9[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(
_tmp9B9,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9BA))))),((_tmp9B5[1]=Cyc_Absynpp_scope2doc(
_tmp4B5),((_tmp9B5[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp9B5,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;};};}struct _tuple17{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple0*_tmp4C6;struct _tuple17 _tmp4C5=*x;_tmp4C6=_tmp4C5.f2;return Cyc_Absynpp_qvar2doc(
_tmp4C6);}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct
Cyc_PP_Doc*s;{void*_tmp4C7=d->r;struct Cyc_Absyn_Fndecl*_tmp4C9;struct Cyc_Absyn_Aggrdecl*
_tmp4CB;struct Cyc_Absyn_Vardecl*_tmp4CD;struct Cyc_Absyn_Tvar*_tmp4CF;struct Cyc_Absyn_Vardecl*
_tmp4D0;int _tmp4D1;struct Cyc_Absyn_Exp*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D4;
struct Cyc_Absyn_Tvar*_tmp4D5;struct Cyc_Absyn_Vardecl*_tmp4D6;struct Cyc_Absyn_Datatypedecl*
_tmp4D8;struct Cyc_Absyn_Datatypedecl _tmp4D9;enum Cyc_Absyn_Scope _tmp4DA;struct
_tuple0*_tmp4DB;struct Cyc_List_List*_tmp4DC;struct Cyc_Core_Opt*_tmp4DD;int
_tmp4DE;struct Cyc_Absyn_Pat*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_List_List*
_tmp4E3;struct Cyc_Absyn_Enumdecl*_tmp4E5;struct Cyc_Absyn_Enumdecl _tmp4E6;enum 
Cyc_Absyn_Scope _tmp4E7;struct _tuple0*_tmp4E8;struct Cyc_Core_Opt*_tmp4E9;struct
Cyc_Absyn_Typedefdecl*_tmp4EB;struct _dyneither_ptr*_tmp4ED;struct Cyc_List_List*
_tmp4EE;struct _tuple0*_tmp4F0;struct Cyc_List_List*_tmp4F1;struct Cyc_List_List*
_tmp4F3;struct Cyc_List_List*_tmp4F5;struct Cyc_List_List*_tmp4F6;_LL286: {struct
Cyc_Absyn_Fn_d_struct*_tmp4C8=(struct Cyc_Absyn_Fn_d_struct*)_tmp4C7;if(_tmp4C8->tag
!= 1)goto _LL288;else{_tmp4C9=_tmp4C8->f1;}}_LL287: {struct Cyc_Absyn_FnType_struct
_tmp9C0;struct Cyc_Absyn_FnInfo _tmp9BF;struct Cyc_Absyn_FnType_struct*_tmp9BE;void*
t=(void*)((_tmp9BE=_cycalloc(sizeof(*_tmp9BE)),((_tmp9BE[0]=((_tmp9C0.tag=10,((
_tmp9C0.f1=((_tmp9BF.tvars=_tmp4C9->tvs,((_tmp9BF.effect=_tmp4C9->effect,((
_tmp9BF.ret_typ=_tmp4C9->ret_type,((_tmp9BF.args=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp4C9->args),((_tmp9BF.c_varargs=_tmp4C9->c_varargs,((_tmp9BF.cyc_varargs=
_tmp4C9->cyc_varargs,((_tmp9BF.rgn_po=_tmp4C9->rgn_po,((_tmp9BF.attributes=0,
_tmp9BF)))))))))))))))),_tmp9C0)))),_tmp9BE))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp4C9->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp4C9->is_inline)switch(Cyc_Cyclone_c_compiler){
case Cyc_Cyclone_Gcc_c: _LL2A6:{const char*_tmp9C1;inlinedoc=Cyc_PP_text(((_tmp9C1="inline ",
_tag_dyneither(_tmp9C1,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2A7:{
const char*_tmp9C2;inlinedoc=Cyc_PP_text(((_tmp9C2="__inline ",_tag_dyneither(
_tmp9C2,sizeof(char),10))));}break;}else{inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*
scopedoc=Cyc_Absynpp_scope2doc(_tmp4C9->sc);struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2A9: beforenamedoc=
attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2AA: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp4C9->attributes);break;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp4C9->name);struct Cyc_PP_Doc*_tmp9C5[2];struct Cyc_Core_Opt*_tmp9C4;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp9C4=_cycalloc(
sizeof(*_tmp9C4)),((_tmp9C4->v=((_tmp9C5[1]=namedoc,((_tmp9C5[0]=beforenamedoc,
Cyc_PP_cat(_tag_dyneither(_tmp9C5,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9C4)))));
struct Cyc_PP_Doc*_tmp9C8[2];struct Cyc_PP_Doc*_tmp9C7[5];struct Cyc_PP_Doc*bodydoc=(
_tmp9C7[4]=Cyc_Absynpp_rb(),((_tmp9C7[3]=Cyc_PP_line_doc(),((_tmp9C7[2]=Cyc_PP_nest(
2,((_tmp9C8[1]=Cyc_Absynpp_stmt2doc(_tmp4C9->body,0),((_tmp9C8[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dyneither(_tmp9C8,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9C7[1]=
Cyc_Absynpp_lb(),((_tmp9C7[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(
_tmp9C7,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp9C9[4];s=((
_tmp9C9[3]=bodydoc,((_tmp9C9[2]=tqtddoc,((_tmp9C9[1]=scopedoc,((_tmp9C9[0]=
inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9C9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2AC:{struct Cyc_PP_Doc*
_tmp9CA[2];s=((_tmp9CA[1]=s,((_tmp9CA[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(
_tmp9CA,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2AD: break;}goto
_LL285;};};}_LL288: {struct Cyc_Absyn_Aggr_d_struct*_tmp4CA=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp4C7;if(_tmp4CA->tag != 6)goto _LL28A;else{_tmp4CB=_tmp4CA->f1;}}_LL289: if(
_tmp4CB->impl == 0){const char*_tmp9CD;struct Cyc_PP_Doc*_tmp9CC[5];s=((_tmp9CC[4]=
Cyc_PP_text(((_tmp9CD=";",_tag_dyneither(_tmp9CD,sizeof(char),2)))),((_tmp9CC[3]=
Cyc_Absynpp_ktvars2doc(_tmp4CB->tvs),((_tmp9CC[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp4CB->name),((_tmp9CC[1]=Cyc_Absynpp_aggr_kind2doc(_tmp4CB->kind),((_tmp9CC[0]=
Cyc_Absynpp_scope2doc(_tmp4CB->sc),Cyc_PP_cat(_tag_dyneither(_tmp9CC,sizeof(
struct Cyc_PP_Doc*),5))))))))))));}else{struct Cyc_PP_Doc*_tmp9D7[2];const char*
_tmp9D6;struct Cyc_PP_Doc*_tmp9D5[2];const char*_tmp9D4;struct Cyc_PP_Doc*_tmp9D3[
13];s=((_tmp9D3[12]=Cyc_PP_text(((_tmp9D4=";",_tag_dyneither(_tmp9D4,sizeof(char),
2)))),((_tmp9D3[11]=Cyc_Absynpp_atts2doc(_tmp4CB->attributes),((_tmp9D3[10]=Cyc_Absynpp_rb(),((
_tmp9D3[9]=Cyc_PP_line_doc(),((_tmp9D3[8]=Cyc_PP_nest(2,((_tmp9D5[1]=Cyc_Absynpp_aggrfields2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CB->impl))->fields),((_tmp9D5[0]=
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9D5,sizeof(struct Cyc_PP_Doc*),2))))))),((
_tmp9D3[7]=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CB->impl))->rgn_po == 
0?Cyc_PP_nil_doc():((_tmp9D7[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4CB->impl))->rgn_po),((_tmp9D7[0]=Cyc_PP_text(((_tmp9D6=":",
_tag_dyneither(_tmp9D6,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9D7,
sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9D3[6]=Cyc_Absynpp_ktvars2doc(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CB->impl))->exist_vars),((_tmp9D3[5]=
Cyc_Absynpp_lb(),((_tmp9D3[4]=Cyc_PP_blank_doc(),((_tmp9D3[3]=Cyc_Absynpp_ktvars2doc(
_tmp4CB->tvs),((_tmp9D3[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4CB->name),((_tmp9D3[1]=
Cyc_Absynpp_aggr_kind2doc(_tmp4CB->kind),((_tmp9D3[0]=Cyc_Absynpp_scope2doc(
_tmp4CB->sc),Cyc_PP_cat(_tag_dyneither(_tmp9D3,sizeof(struct Cyc_PP_Doc*),13))))))))))))))))))))))))))));}
goto _LL285;_LL28A: {struct Cyc_Absyn_Var_d_struct*_tmp4CC=(struct Cyc_Absyn_Var_d_struct*)
_tmp4C7;if(_tmp4CC->tag != 0)goto _LL28C;else{_tmp4CD=_tmp4CC->f1;}}_LL28B: s=Cyc_Absynpp_vardecl2doc(
_tmp4CD);goto _LL285;_LL28C: {struct Cyc_Absyn_Region_d_struct*_tmp4CE=(struct Cyc_Absyn_Region_d_struct*)
_tmp4C7;if(_tmp4CE->tag != 4)goto _LL28E;else{_tmp4CF=_tmp4CE->f1;_tmp4D0=_tmp4CE->f2;
_tmp4D1=_tmp4CE->f3;_tmp4D2=_tmp4CE->f4;}}_LL28D:{const char*_tmp9EA;const char*
_tmp9E9;const char*_tmp9E8;const char*_tmp9E7;struct Cyc_PP_Doc*_tmp9E6[3];const
char*_tmp9E5;const char*_tmp9E4;const char*_tmp9E3;struct Cyc_PP_Doc*_tmp9E2[8];s=((
_tmp9E2[7]=Cyc_PP_text(((_tmp9E3=";",_tag_dyneither(_tmp9E3,sizeof(char),2)))),((
_tmp9E2[6]=_tmp4D2 != 0?(_tmp9E6[2]=Cyc_PP_text(((_tmp9E5=")",_tag_dyneither(
_tmp9E5,sizeof(char),2)))),((_tmp9E6[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_tmp4D2),((_tmp9E6[0]=Cyc_PP_text(((_tmp9E4=" = open(",_tag_dyneither(_tmp9E4,
sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9E6,sizeof(struct Cyc_PP_Doc*),3))))))):
Cyc_PP_nil_doc(),((_tmp9E2[5]=_tmp4D1?Cyc_PP_text(((_tmp9E7=" @resetable",
_tag_dyneither(_tmp9E7,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9E2[4]=Cyc_Absynpp_qvar2doc(
_tmp4D0->name),((_tmp9E2[3]=Cyc_PP_text(((_tmp9E8=">",_tag_dyneither(_tmp9E8,
sizeof(char),2)))),((_tmp9E2[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4CF)),((
_tmp9E2[1]=Cyc_PP_text(((_tmp9E9="<",_tag_dyneither(_tmp9E9,sizeof(char),2)))),((
_tmp9E2[0]=Cyc_PP_text(((_tmp9EA="region",_tag_dyneither(_tmp9EA,sizeof(char),7)))),
Cyc_PP_cat(_tag_dyneither(_tmp9E2,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL285;_LL28E: {struct Cyc_Absyn_Alias_d_struct*_tmp4D3=(struct Cyc_Absyn_Alias_d_struct*)
_tmp4C7;if(_tmp4D3->tag != 5)goto _LL290;else{_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;
_tmp4D6=_tmp4D3->f3;}}_LL28F:{const char*_tmp9F5;const char*_tmp9F4;const char*
_tmp9F3;const char*_tmp9F2;const char*_tmp9F1;struct Cyc_PP_Doc*_tmp9F0[8];s=((
_tmp9F0[7]=Cyc_PP_text(((_tmp9F1=";",_tag_dyneither(_tmp9F1,sizeof(char),2)))),((
_tmp9F0[6]=Cyc_Absynpp_exp2doc(_tmp4D4),((_tmp9F0[5]=Cyc_PP_text(((_tmp9F2=" = ",
_tag_dyneither(_tmp9F2,sizeof(char),4)))),((_tmp9F0[4]=Cyc_Absynpp_qvar2doc(
_tmp4D6->name),((_tmp9F0[3]=Cyc_PP_text(((_tmp9F3="> ",_tag_dyneither(_tmp9F3,
sizeof(char),3)))),((_tmp9F0[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4D5)),((
_tmp9F0[1]=Cyc_PP_text(((_tmp9F4=" <",_tag_dyneither(_tmp9F4,sizeof(char),3)))),((
_tmp9F0[0]=Cyc_PP_text(((_tmp9F5="alias",_tag_dyneither(_tmp9F5,sizeof(char),6)))),
Cyc_PP_cat(_tag_dyneither(_tmp9F0,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
goto _LL285;_LL290: {struct Cyc_Absyn_Datatype_d_struct*_tmp4D7=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp4C7;if(_tmp4D7->tag != 7)goto _LL292;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=*_tmp4D8;
_tmp4DA=_tmp4D9.sc;_tmp4DB=_tmp4D9.name;_tmp4DC=_tmp4D9.tvs;_tmp4DD=_tmp4D9.fields;
_tmp4DE=_tmp4D9.is_extensible;}}_LL291: if(_tmp4DD == 0){const char*_tmp9FC;const
char*_tmp9FB;const char*_tmp9FA;struct Cyc_PP_Doc*_tmp9F9[6];s=((_tmp9F9[5]=Cyc_PP_text(((
_tmp9FA=";",_tag_dyneither(_tmp9FA,sizeof(char),2)))),((_tmp9F9[4]=Cyc_Absynpp_ktvars2doc(
_tmp4DC),((_tmp9F9[3]=_tmp4DE?Cyc_Absynpp_qvar2bolddoc(_tmp4DB): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4DB),((_tmp9F9[2]=Cyc_PP_text(((_tmp9FB="datatype ",_tag_dyneither(_tmp9FB,
sizeof(char),10)))),((_tmp9F9[1]=_tmp4DE?Cyc_PP_text(((_tmp9FC="@extensible",
_tag_dyneither(_tmp9FC,sizeof(char),12)))): Cyc_PP_blank_doc(),((_tmp9F9[0]=Cyc_Absynpp_scope2doc(
_tmp4DA),Cyc_PP_cat(_tag_dyneither(_tmp9F9,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA05;const char*_tmpA04;struct Cyc_PP_Doc*_tmpA03[2];const char*
_tmpA02;struct Cyc_PP_Doc*_tmpA01[11];s=((_tmpA01[10]=Cyc_PP_text(((_tmpA02=";",
_tag_dyneither(_tmpA02,sizeof(char),2)))),((_tmpA01[9]=Cyc_Absynpp_rb(),((
_tmpA01[8]=Cyc_PP_line_doc(),((_tmpA01[7]=Cyc_PP_nest(2,((_tmpA03[1]=Cyc_Absynpp_datatypefields2doc((
struct Cyc_List_List*)_tmp4DD->v),((_tmpA03[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dyneither(_tmpA03,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA01[6]=Cyc_Absynpp_lb(),((
_tmpA01[5]=Cyc_PP_blank_doc(),((_tmpA01[4]=Cyc_Absynpp_ktvars2doc(_tmp4DC),((
_tmpA01[3]=_tmp4DE?Cyc_Absynpp_qvar2bolddoc(_tmp4DB): Cyc_Absynpp_typedef_name2bolddoc(
_tmp4DB),((_tmpA01[2]=Cyc_PP_text(((_tmpA04="datatype ",_tag_dyneither(_tmpA04,
sizeof(char),10)))),((_tmpA01[1]=_tmp4DE?Cyc_PP_text(((_tmpA05="@extensible ",
_tag_dyneither(_tmpA05,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmpA01[0]=Cyc_Absynpp_scope2doc(
_tmp4DA),Cyc_PP_cat(_tag_dyneither(_tmpA01,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL285;_LL292: {struct Cyc_Absyn_Let_d_struct*_tmp4DF=(struct Cyc_Absyn_Let_d_struct*)
_tmp4C7;if(_tmp4DF->tag != 2)goto _LL294;else{_tmp4E0=_tmp4DF->f1;_tmp4E1=_tmp4DF->f3;}}
_LL293:{const char*_tmpA0C;const char*_tmpA0B;const char*_tmpA0A;struct Cyc_PP_Doc*
_tmpA09[5];s=((_tmpA09[4]=Cyc_PP_text(((_tmpA0A=";",_tag_dyneither(_tmpA0A,
sizeof(char),2)))),((_tmpA09[3]=Cyc_Absynpp_exp2doc(_tmp4E1),((_tmpA09[2]=Cyc_PP_text(((
_tmpA0B=" = ",_tag_dyneither(_tmpA0B,sizeof(char),4)))),((_tmpA09[1]=Cyc_Absynpp_pat2doc(
_tmp4E0),((_tmpA09[0]=Cyc_PP_text(((_tmpA0C="let ",_tag_dyneither(_tmpA0C,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA09,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL285;_LL294: {struct Cyc_Absyn_Letv_d_struct*_tmp4E2=(struct Cyc_Absyn_Letv_d_struct*)
_tmp4C7;if(_tmp4E2->tag != 3)goto _LL296;else{_tmp4E3=_tmp4E2->f1;}}_LL295:{const
char*_tmpA11;const char*_tmpA10;struct Cyc_PP_Doc*_tmpA0F[3];s=((_tmpA0F[2]=Cyc_PP_text(((
_tmpA10=";",_tag_dyneither(_tmpA10,sizeof(char),2)))),((_tmpA0F[1]=Cyc_Absynpp_ids2doc(
_tmp4E3),((_tmpA0F[0]=Cyc_PP_text(((_tmpA11="let ",_tag_dyneither(_tmpA11,
sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA0F,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL285;_LL296: {struct Cyc_Absyn_Enum_d_struct*_tmp4E4=(struct Cyc_Absyn_Enum_d_struct*)
_tmp4C7;if(_tmp4E4->tag != 8)goto _LL298;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=*_tmp4E5;
_tmp4E7=_tmp4E6.sc;_tmp4E8=_tmp4E6.name;_tmp4E9=_tmp4E6.fields;}}_LL297: if(
_tmp4E9 == 0){const char*_tmpA16;const char*_tmpA15;struct Cyc_PP_Doc*_tmpA14[4];s=((
_tmpA14[3]=Cyc_PP_text(((_tmpA15=";",_tag_dyneither(_tmpA15,sizeof(char),2)))),((
_tmpA14[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4E8),((_tmpA14[1]=Cyc_PP_text(((
_tmpA16="enum ",_tag_dyneither(_tmpA16,sizeof(char),6)))),((_tmpA14[0]=Cyc_Absynpp_scope2doc(
_tmp4E7),Cyc_PP_cat(_tag_dyneither(_tmpA14,sizeof(struct Cyc_PP_Doc*),4))))))))));}
else{const char*_tmpA1D;struct Cyc_PP_Doc*_tmpA1C[2];const char*_tmpA1B;struct Cyc_PP_Doc*
_tmpA1A[9];s=((_tmpA1A[8]=Cyc_PP_text(((_tmpA1B=";",_tag_dyneither(_tmpA1B,
sizeof(char),2)))),((_tmpA1A[7]=Cyc_Absynpp_rb(),((_tmpA1A[6]=Cyc_PP_line_doc(),((
_tmpA1A[5]=Cyc_PP_nest(2,((_tmpA1C[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp4E9->v),((_tmpA1C[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmpA1C,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmpA1A[4]=Cyc_Absynpp_lb(),((_tmpA1A[3]=
Cyc_PP_blank_doc(),((_tmpA1A[2]=Cyc_Absynpp_qvar2bolddoc(_tmp4E8),((_tmpA1A[1]=
Cyc_PP_text(((_tmpA1D="enum ",_tag_dyneither(_tmpA1D,sizeof(char),6)))),((
_tmpA1A[0]=Cyc_Absynpp_scope2doc(_tmp4E7),Cyc_PP_cat(_tag_dyneither(_tmpA1A,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL285;_LL298: {struct Cyc_Absyn_Typedef_d_struct*
_tmp4EA=(struct Cyc_Absyn_Typedef_d_struct*)_tmp4C7;if(_tmp4EA->tag != 9)goto
_LL29A;else{_tmp4EB=_tmp4EA->f1;}}_LL299: {void*t;if(_tmp4EB->defn != 0)t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4EB->defn))->v;else{t=Cyc_Absyn_new_evar(
_tmp4EB->kind,0);}{const char*_tmpA27;struct Cyc_Core_Opt*_tmpA26;struct Cyc_PP_Doc*
_tmpA25[2];const char*_tmpA24;struct Cyc_PP_Doc*_tmpA23[4];s=((_tmpA23[3]=Cyc_PP_text(((
_tmpA24=";",_tag_dyneither(_tmpA24,sizeof(char),2)))),((_tmpA23[2]=Cyc_Absynpp_atts2doc(
_tmp4EB->atts),((_tmpA23[1]=Cyc_Absynpp_tqtd2doc(_tmp4EB->tq,t,((_tmpA26=
_cycalloc(sizeof(*_tmpA26)),((_tmpA26->v=((_tmpA25[1]=Cyc_Absynpp_tvars2doc(
_tmp4EB->tvs),((_tmpA25[0]=Cyc_Absynpp_typedef_name2bolddoc(_tmp4EB->name),Cyc_PP_cat(
_tag_dyneither(_tmpA25,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA26))))),((_tmpA23[
0]=Cyc_PP_text(((_tmpA27="typedef ",_tag_dyneither(_tmpA27,sizeof(char),9)))),
Cyc_PP_cat(_tag_dyneither(_tmpA23,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL285;}_LL29A: {struct Cyc_Absyn_Namespace_d_struct*_tmp4EC=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp4C7;if(_tmp4EC->tag != 10)goto _LL29C;else{_tmp4ED=_tmp4EC->f1;_tmp4EE=_tmp4EC->f2;}}
_LL29B: if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4ED);{
const char*_tmpA2C;const char*_tmpA2B;struct Cyc_PP_Doc*_tmpA2A[8];s=((_tmpA2A[7]=
Cyc_Absynpp_rb(),((_tmpA2A[6]=Cyc_PP_line_doc(),((_tmpA2A[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA2B="",_tag_dyneither(_tmpA2B,
sizeof(char),1))),_tmp4EE),((_tmpA2A[4]=Cyc_PP_line_doc(),((_tmpA2A[3]=Cyc_Absynpp_lb(),((
_tmpA2A[2]=Cyc_PP_blank_doc(),((_tmpA2A[1]=Cyc_PP_textptr(_tmp4ED),((_tmpA2A[0]=
Cyc_PP_text(((_tmpA2C="namespace ",_tag_dyneither(_tmpA2C,sizeof(char),11)))),
Cyc_PP_cat(_tag_dyneither(_tmpA2A,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();goto _LL285;
_LL29C: {struct Cyc_Absyn_Using_d_struct*_tmp4EF=(struct Cyc_Absyn_Using_d_struct*)
_tmp4C7;if(_tmp4EF->tag != 11)goto _LL29E;else{_tmp4F0=_tmp4EF->f1;_tmp4F1=_tmp4EF->f2;}}
_LL29D: if(Cyc_Absynpp_print_using_stmts){const char*_tmpA31;const char*_tmpA30;
struct Cyc_PP_Doc*_tmpA2F[8];s=((_tmpA2F[7]=Cyc_Absynpp_rb(),((_tmpA2F[6]=Cyc_PP_line_doc(),((
_tmpA2F[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA30="",_tag_dyneither(_tmpA30,sizeof(char),1))),_tmp4F1),((_tmpA2F[4]=Cyc_PP_line_doc(),((
_tmpA2F[3]=Cyc_Absynpp_lb(),((_tmpA2F[2]=Cyc_PP_blank_doc(),((_tmpA2F[1]=Cyc_Absynpp_qvar2doc(
_tmp4F0),((_tmpA2F[0]=Cyc_PP_text(((_tmpA31="using ",_tag_dyneither(_tmpA31,
sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA2F,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
else{const char*_tmpA3C;const char*_tmpA3B;const char*_tmpA3A;const char*_tmpA39;
const char*_tmpA38;struct Cyc_PP_Doc*_tmpA37[11];s=((_tmpA37[10]=Cyc_PP_text(((
_tmpA38=" */",_tag_dyneither(_tmpA38,sizeof(char),4)))),((_tmpA37[9]=Cyc_Absynpp_rb(),((
_tmpA37[8]=Cyc_PP_text(((_tmpA39="/* ",_tag_dyneither(_tmpA39,sizeof(char),4)))),((
_tmpA37[7]=Cyc_PP_line_doc(),((_tmpA37[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA3A="",_tag_dyneither(_tmpA3A,sizeof(char),1))),
_tmp4F1),((_tmpA37[5]=Cyc_PP_line_doc(),((_tmpA37[4]=Cyc_PP_text(((_tmpA3B=" */",
_tag_dyneither(_tmpA3B,sizeof(char),4)))),((_tmpA37[3]=Cyc_Absynpp_lb(),((
_tmpA37[2]=Cyc_PP_blank_doc(),((_tmpA37[1]=Cyc_Absynpp_qvar2doc(_tmp4F0),((
_tmpA37[0]=Cyc_PP_text(((_tmpA3C="/* using ",_tag_dyneither(_tmpA3C,sizeof(char),
10)))),Cyc_PP_cat(_tag_dyneither(_tmpA37,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL285;_LL29E: {struct Cyc_Absyn_ExternC_d_struct*_tmp4F2=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp4C7;if(_tmp4F2->tag != 12)goto _LL2A0;else{_tmp4F3=_tmp4F2->f1;}}_LL29F: if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA41;const char*_tmpA40;struct Cyc_PP_Doc*_tmpA3F[6];s=((_tmpA3F[5]=
Cyc_Absynpp_rb(),((_tmpA3F[4]=Cyc_PP_line_doc(),((_tmpA3F[3]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA40="",_tag_dyneither(_tmpA40,
sizeof(char),1))),_tmp4F3),((_tmpA3F[2]=Cyc_PP_line_doc(),((_tmpA3F[1]=Cyc_Absynpp_lb(),((
_tmpA3F[0]=Cyc_PP_text(((_tmpA41="extern \"C\" ",_tag_dyneither(_tmpA41,sizeof(
char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA3F,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
else{const char*_tmpA4C;const char*_tmpA4B;const char*_tmpA4A;const char*_tmpA49;
const char*_tmpA48;struct Cyc_PP_Doc*_tmpA47[9];s=((_tmpA47[8]=Cyc_PP_text(((
_tmpA48=" */",_tag_dyneither(_tmpA48,sizeof(char),4)))),((_tmpA47[7]=Cyc_Absynpp_rb(),((
_tmpA47[6]=Cyc_PP_text(((_tmpA49="/* ",_tag_dyneither(_tmpA49,sizeof(char),4)))),((
_tmpA47[5]=Cyc_PP_line_doc(),((_tmpA47[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmpA4A="",_tag_dyneither(_tmpA4A,sizeof(char),1))),
_tmp4F3),((_tmpA47[3]=Cyc_PP_line_doc(),((_tmpA47[2]=Cyc_PP_text(((_tmpA4B=" */",
_tag_dyneither(_tmpA4B,sizeof(char),4)))),((_tmpA47[1]=Cyc_Absynpp_lb(),((
_tmpA47[0]=Cyc_PP_text(((_tmpA4C="/* extern \"C\" ",_tag_dyneither(_tmpA4C,
sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA47,sizeof(struct Cyc_PP_Doc*),
9))))))))))))))))))));}goto _LL285;_LL2A0: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp4F4=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp4C7;if(_tmp4F4->tag != 13)
goto _LL2A2;else{_tmp4F5=_tmp4F4->f1;_tmp4F6=_tmp4F4->f2;}}_LL2A1: if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;if(_tmp4F6 != 0){const char*_tmpA51;const char*_tmpA50;
struct Cyc_PP_Doc*_tmpA4F[7];exs_doc=((_tmpA4F[6]=Cyc_Absynpp_rb(),((_tmpA4F[5]=
Cyc_PP_line_doc(),((_tmpA4F[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_export2doc,((_tmpA50=",",_tag_dyneither(_tmpA50,sizeof(char),2))),
_tmp4F6),((_tmpA4F[3]=Cyc_PP_line_doc(),((_tmpA4F[2]=Cyc_Absynpp_lb(),((_tmpA4F[
1]=Cyc_PP_text(((_tmpA51=" export ",_tag_dyneither(_tmpA51,sizeof(char),9)))),((
_tmpA4F[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA4F,sizeof(struct Cyc_PP_Doc*),
7))))))))))))))));}else{exs_doc=Cyc_Absynpp_rb();}{const char*_tmpA56;const char*
_tmpA55;struct Cyc_PP_Doc*_tmpA54[6];s=((_tmpA54[5]=exs_doc,((_tmpA54[4]=Cyc_PP_line_doc(),((
_tmpA54[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA55="",_tag_dyneither(_tmpA55,sizeof(char),1))),_tmp4F5),((_tmpA54[2]=Cyc_PP_line_doc(),((
_tmpA54[1]=Cyc_Absynpp_lb(),((_tmpA54[0]=Cyc_PP_text(((_tmpA56="extern \"C include\" ",
_tag_dyneither(_tmpA56,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA54,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{const char*_tmpA61;const char*
_tmpA60;const char*_tmpA5F;const char*_tmpA5E;const char*_tmpA5D;struct Cyc_PP_Doc*
_tmpA5C[9];s=((_tmpA5C[8]=Cyc_PP_text(((_tmpA5D=" */",_tag_dyneither(_tmpA5D,
sizeof(char),4)))),((_tmpA5C[7]=Cyc_Absynpp_rb(),((_tmpA5C[6]=Cyc_PP_text(((
_tmpA5E="/* ",_tag_dyneither(_tmpA5E,sizeof(char),4)))),((_tmpA5C[5]=Cyc_PP_line_doc(),((
_tmpA5C[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmpA5F="",_tag_dyneither(_tmpA5F,sizeof(char),1))),_tmp4F5),((_tmpA5C[3]=Cyc_PP_line_doc(),((
_tmpA5C[2]=Cyc_PP_text(((_tmpA60=" */",_tag_dyneither(_tmpA60,sizeof(char),4)))),((
_tmpA5C[1]=Cyc_Absynpp_lb(),((_tmpA5C[0]=Cyc_PP_text(((_tmpA61="/* extern \"C include\" ",
_tag_dyneither(_tmpA61,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA5C,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL285;_LL2A2: {struct Cyc_Absyn_Porton_d_struct*
_tmp4F7=(struct Cyc_Absyn_Porton_d_struct*)_tmp4C7;if(_tmp4F7->tag != 14)goto
_LL2A4;}_LL2A3:{const char*_tmpA64;struct Cyc_PP_Doc*_tmpA63[2];s=((_tmpA63[1]=Cyc_Absynpp_lb(),((
_tmpA63[0]=Cyc_PP_text(((_tmpA64="__cyclone_port_on__;",_tag_dyneither(_tmpA64,
sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA63,sizeof(struct Cyc_PP_Doc*),
2))))));}goto _LL285;_LL2A4: {struct Cyc_Absyn_Portoff_d_struct*_tmp4F8=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp4C7;if(_tmp4F8->tag != 15)goto _LL285;}_LL2A5:{const char*_tmpA67;struct Cyc_PP_Doc*
_tmpA66[2];s=((_tmpA66[1]=Cyc_Absynpp_lb(),((_tmpA66[0]=Cyc_PP_text(((_tmpA67="__cyclone_port_off__;",
_tag_dyneither(_tmpA67,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA66,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL285;_LL285:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){if(Cyc_Absynpp_print_for_cycdoc)
return Cyc_PP_nil_doc();switch(sc){case Cyc_Absyn_Static: _LL2AF: {const char*
_tmpA68;return Cyc_PP_text(((_tmpA68="static ",_tag_dyneither(_tmpA68,sizeof(char),
8))));}case Cyc_Absyn_Public: _LL2B0: return Cyc_PP_nil_doc();case Cyc_Absyn_Extern:
_LL2B1: {const char*_tmpA69;return Cyc_PP_text(((_tmpA69="extern ",_tag_dyneither(
_tmpA69,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2B2: {const char*_tmpA6A;
return Cyc_PP_text(((_tmpA6A="extern \"C\" ",_tag_dyneither(_tmpA6A,sizeof(char),
12))));}case Cyc_Absyn_Abstract: _LL2B3: {const char*_tmpA6B;return Cyc_PP_text(((
_tmpA6B="abstract ",_tag_dyneither(_tmpA6B,sizeof(char),10))));}case Cyc_Absyn_Register:
_LL2B4: {const char*_tmpA6C;return Cyc_PP_text(((_tmpA6C="register ",
_tag_dyneither(_tmpA6C,sizeof(char),10))));}}}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp560=t;struct Cyc_Absyn_Tvar*_tmp562;struct Cyc_List_List*_tmp564;
_LL2B7: {struct Cyc_Absyn_VarType_struct*_tmp561=(struct Cyc_Absyn_VarType_struct*)
_tmp560;if(_tmp561->tag != 2)goto _LL2B9;else{_tmp562=_tmp561->f1;}}_LL2B8: return
Cyc_Tcutil_is_temp_tvar(_tmp562);_LL2B9: {struct Cyc_Absyn_JoinEff_struct*_tmp563=(
struct Cyc_Absyn_JoinEff_struct*)_tmp560;if(_tmp563->tag != 24)goto _LL2BB;else{
_tmp564=_tmp563->f1;}}_LL2BA: return((int(*)(int(*pred)(void*),struct Cyc_List_List*
x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp564);_LL2BB:;
_LL2BC: return 0;_LL2B6:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){void*_tmp565=t;
void**_tmp569;void*_tmp56A;_LL2BE: {struct Cyc_Absyn_AnonAggrType_struct*_tmp566=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp565;if(_tmp566->tag != 13)goto _LL2C0;}
_LL2BF: return 1;_LL2C0: {struct Cyc_Absyn_AnonEnumType_struct*_tmp567=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp565;if(_tmp567->tag != 15)goto _LL2C2;}_LL2C1: return 1;_LL2C2: {struct Cyc_Absyn_TypedefType_struct*
_tmp568=(struct Cyc_Absyn_TypedefType_struct*)_tmp565;if(_tmp568->tag != 18)goto
_LL2C4;else{_tmp569=_tmp568->f4;if(_tmp569 == 0)goto _LL2C4;_tmp56A=*_tmp569;}}
_LL2C3: return Cyc_Absynpp_is_anon_aggrtype(_tmp56A);_LL2C4:;_LL2C5: return 0;_LL2BD:;}
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,
void*atts,struct Cyc_List_List*tms){if(tms != 0  && tms->tl != 0){struct _tuple16
_tmpA6D;struct _tuple16 _tmp56C=(_tmpA6D.f1=(void*)tms->hd,((_tmpA6D.f2=(void*)((
struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA6D)));void*_tmp56D;void*
_tmp56F;_LL2C7: _tmp56D=_tmp56C.f1;{struct Cyc_Absyn_Pointer_mod_struct*_tmp56E=(
struct Cyc_Absyn_Pointer_mod_struct*)_tmp56D;if(_tmp56E->tag != 2)goto _LL2C9;};
_tmp56F=_tmp56C.f2;{struct Cyc_Absyn_Function_mod_struct*_tmp570=(struct Cyc_Absyn_Function_mod_struct*)
_tmp56F;if(_tmp570->tag != 3)goto _LL2C9;};_LL2C8: {struct Cyc_List_List*_tmpA70;
struct Cyc_List_List*_tmpA6F;return(_tmpA6F=_region_malloc(r,sizeof(*_tmpA6F)),((
_tmpA6F->hd=(void*)tms->hd,((_tmpA6F->tl=((_tmpA70=_region_malloc(r,sizeof(*
_tmpA70)),((_tmpA70->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((
_tmpA70->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)
_check_null(tms->tl))->tl),_tmpA70)))))),_tmpA6F)))));}_LL2C9:;_LL2CA: {struct
Cyc_List_List*_tmpA71;return(_tmpA71=_region_malloc(r,sizeof(*_tmpA71)),((
_tmpA71->hd=atts,((_tmpA71->tl=tms,_tmpA71)))));}_LL2C6:;}else{struct Cyc_List_List*
_tmpA72;return(_tmpA72=_region_malloc(r,sizeof(*_tmpA72)),((_tmpA72->hd=atts,((
_tmpA72->tl=tms,_tmpA72)))));}}struct _tuple12 Cyc_Absynpp_to_tms(struct
_RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){void*_tmp575=t;struct Cyc_Absyn_ArrayInfo
_tmp577;void*_tmp578;struct Cyc_Absyn_Tqual _tmp579;struct Cyc_Absyn_Exp*_tmp57A;
union Cyc_Absyn_Constraint*_tmp57B;struct Cyc_Position_Segment*_tmp57C;struct Cyc_Absyn_PtrInfo
_tmp57E;void*_tmp57F;struct Cyc_Absyn_Tqual _tmp580;struct Cyc_Absyn_PtrAtts _tmp581;
struct Cyc_Absyn_FnInfo _tmp583;struct Cyc_List_List*_tmp584;struct Cyc_Core_Opt*
_tmp585;void*_tmp586;struct Cyc_List_List*_tmp587;int _tmp588;struct Cyc_Absyn_VarargInfo*
_tmp589;struct Cyc_List_List*_tmp58A;struct Cyc_List_List*_tmp58B;struct Cyc_Core_Opt*
_tmp58D;struct Cyc_Core_Opt*_tmp58E;int _tmp58F;struct _tuple0*_tmp591;struct Cyc_List_List*
_tmp592;void**_tmp593;_LL2CC: {struct Cyc_Absyn_ArrayType_struct*_tmp576=(struct
Cyc_Absyn_ArrayType_struct*)_tmp575;if(_tmp576->tag != 9)goto _LL2CE;else{_tmp577=
_tmp576->f1;_tmp578=_tmp577.elt_type;_tmp579=_tmp577.tq;_tmp57A=_tmp577.num_elts;
_tmp57B=_tmp577.zero_term;_tmp57C=_tmp577.zt_loc;}}_LL2CD: {struct Cyc_Absyn_Tqual
_tmp595;void*_tmp596;struct Cyc_List_List*_tmp597;struct _tuple12 _tmp594=Cyc_Absynpp_to_tms(
r,_tmp579,_tmp578);_tmp595=_tmp594.f1;_tmp596=_tmp594.f2;_tmp597=_tmp594.f3;{
void*tm;if(_tmp57A == 0){struct Cyc_Absyn_Carray_mod_struct _tmpA75;struct Cyc_Absyn_Carray_mod_struct*
_tmpA74;tm=(void*)((_tmpA74=_region_malloc(r,sizeof(*_tmpA74)),((_tmpA74[0]=((
_tmpA75.tag=0,((_tmpA75.f1=_tmp57B,((_tmpA75.f2=_tmp57C,_tmpA75)))))),_tmpA74))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmpA78;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpA77;tm=(void*)((_tmpA77=_region_malloc(r,sizeof(*_tmpA77)),((_tmpA77[0]=((
_tmpA78.tag=1,((_tmpA78.f1=(struct Cyc_Absyn_Exp*)_tmp57A,((_tmpA78.f2=_tmp57B,((
_tmpA78.f3=_tmp57C,_tmpA78)))))))),_tmpA77))));}{struct Cyc_List_List*_tmpA7B;
struct _tuple12 _tmpA7A;return(_tmpA7A.f1=_tmp595,((_tmpA7A.f2=_tmp596,((_tmpA7A.f3=((
_tmpA7B=_region_malloc(r,sizeof(*_tmpA7B)),((_tmpA7B->hd=tm,((_tmpA7B->tl=
_tmp597,_tmpA7B)))))),_tmpA7A)))));};};}_LL2CE: {struct Cyc_Absyn_PointerType_struct*
_tmp57D=(struct Cyc_Absyn_PointerType_struct*)_tmp575;if(_tmp57D->tag != 5)goto
_LL2D0;else{_tmp57E=_tmp57D->f1;_tmp57F=_tmp57E.elt_typ;_tmp580=_tmp57E.elt_tq;
_tmp581=_tmp57E.ptr_atts;}}_LL2CF: {struct Cyc_Absyn_Tqual _tmp59F;void*_tmp5A0;
struct Cyc_List_List*_tmp5A1;struct _tuple12 _tmp59E=Cyc_Absynpp_to_tms(r,_tmp580,
_tmp57F);_tmp59F=_tmp59E.f1;_tmp5A0=_tmp59E.f2;_tmp5A1=_tmp59E.f3;{struct Cyc_Absyn_Pointer_mod_struct*
_tmpA81;struct Cyc_Absyn_Pointer_mod_struct _tmpA80;struct Cyc_List_List*_tmpA7F;
_tmp5A1=((_tmpA7F=_region_malloc(r,sizeof(*_tmpA7F)),((_tmpA7F->hd=(void*)((
_tmpA81=_region_malloc(r,sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA80.tag=2,((
_tmpA80.f1=_tmp581,((_tmpA80.f2=tq,_tmpA80)))))),_tmpA81)))),((_tmpA7F->tl=
_tmp5A1,_tmpA7F))))));}{struct _tuple12 _tmpA82;return(_tmpA82.f1=_tmp59F,((
_tmpA82.f2=_tmp5A0,((_tmpA82.f3=_tmp5A1,_tmpA82)))));};}_LL2D0: {struct Cyc_Absyn_FnType_struct*
_tmp582=(struct Cyc_Absyn_FnType_struct*)_tmp575;if(_tmp582->tag != 10)goto _LL2D2;
else{_tmp583=_tmp582->f1;_tmp584=_tmp583.tvars;_tmp585=_tmp583.effect;_tmp586=
_tmp583.ret_typ;_tmp587=_tmp583.args;_tmp588=_tmp583.c_varargs;_tmp589=_tmp583.cyc_varargs;
_tmp58A=_tmp583.rgn_po;_tmp58B=_tmp583.attributes;}}_LL2D1: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp585 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp585->v)){
_tmp585=0;_tmp584=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp584);}{struct Cyc_Absyn_Tqual _tmp5A7;void*_tmp5A8;struct Cyc_List_List*_tmp5A9;
struct _tuple12 _tmp5A6=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp586);
_tmp5A7=_tmp5A6.f1;_tmp5A8=_tmp5A6.f2;_tmp5A9=_tmp5A6.f3;{struct Cyc_List_List*
tms=_tmp5A9;switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2D8: if(
_tmp58B != 0){struct Cyc_Absyn_Attributes_mod_struct _tmpA85;struct Cyc_Absyn_Attributes_mod_struct*
_tmpA84;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA84=_region_malloc(r,
sizeof(*_tmpA84)),((_tmpA84[0]=((_tmpA85.tag=5,((_tmpA85.f1=0,((_tmpA85.f2=
_tmp58B,_tmpA85)))))),_tmpA84)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmpA94;struct Cyc_Absyn_WithTypes_struct*_tmpA93;struct Cyc_Absyn_WithTypes_struct
_tmpA92;struct Cyc_Absyn_Function_mod_struct _tmpA91;struct Cyc_List_List*_tmpA90;
tms=((_tmpA90=_region_malloc(r,sizeof(*_tmpA90)),((_tmpA90->hd=(void*)((_tmpA94=
_region_malloc(r,sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA91.tag=3,((_tmpA91.f1=(
void*)((void*)((_tmpA93=_region_malloc(r,sizeof(*_tmpA93)),((_tmpA93[0]=((
_tmpA92.tag=1,((_tmpA92.f1=_tmp587,((_tmpA92.f2=_tmp588,((_tmpA92.f3=_tmp589,((
_tmpA92.f4=_tmp585,((_tmpA92.f5=_tmp58A,_tmpA92)))))))))))),_tmpA93))))),_tmpA91)))),
_tmpA94)))),((_tmpA90->tl=tms,_tmpA90))))));}break;case Cyc_Cyclone_Vc_c: _LL2D9:{
struct Cyc_Absyn_Function_mod_struct*_tmpAA3;struct Cyc_Absyn_WithTypes_struct*
_tmpAA2;struct Cyc_Absyn_WithTypes_struct _tmpAA1;struct Cyc_Absyn_Function_mod_struct
_tmpAA0;struct Cyc_List_List*_tmpA9F;tms=((_tmpA9F=_region_malloc(r,sizeof(*
_tmpA9F)),((_tmpA9F->hd=(void*)((_tmpAA3=_region_malloc(r,sizeof(*_tmpAA3)),((
_tmpAA3[0]=((_tmpAA0.tag=3,((_tmpAA0.f1=(void*)((void*)((_tmpAA2=_region_malloc(
r,sizeof(*_tmpAA2)),((_tmpAA2[0]=((_tmpAA1.tag=1,((_tmpAA1.f1=_tmp587,((_tmpAA1.f2=
_tmp588,((_tmpAA1.f3=_tmp589,((_tmpAA1.f4=_tmp585,((_tmpAA1.f5=_tmp58A,_tmpAA1)))))))))))),
_tmpAA2))))),_tmpAA0)))),_tmpAA3)))),((_tmpA9F->tl=tms,_tmpA9F))))));}for(0;
_tmp58B != 0;_tmp58B=_tmp58B->tl){void*_tmp5B6=(void*)_tmp58B->hd;_LL2DC: {struct
Cyc_Absyn_Stdcall_att_struct*_tmp5B7=(struct Cyc_Absyn_Stdcall_att_struct*)
_tmp5B6;if(_tmp5B7->tag != 1)goto _LL2DE;}_LL2DD: goto _LL2DF;_LL2DE: {struct Cyc_Absyn_Cdecl_att_struct*
_tmp5B8=(struct Cyc_Absyn_Cdecl_att_struct*)_tmp5B6;if(_tmp5B8->tag != 2)goto
_LL2E0;}_LL2DF: goto _LL2E1;_LL2E0: {struct Cyc_Absyn_Fastcall_att_struct*_tmp5B9=(
struct Cyc_Absyn_Fastcall_att_struct*)_tmp5B6;if(_tmp5B9->tag != 3)goto _LL2E2;}
_LL2E1:{struct Cyc_Absyn_Attributes_mod_struct*_tmpAAD;struct Cyc_List_List*
_tmpAAC;struct Cyc_Absyn_Attributes_mod_struct _tmpAAB;struct Cyc_List_List*_tmpAAA;
tms=((_tmpAAA=_region_malloc(r,sizeof(*_tmpAAA)),((_tmpAAA->hd=(void*)((_tmpAAD=
_region_malloc(r,sizeof(*_tmpAAD)),((_tmpAAD[0]=((_tmpAAB.tag=5,((_tmpAAB.f1=0,((
_tmpAAB.f2=((_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->hd=(void*)_tmp58B->hd,((
_tmpAAC->tl=0,_tmpAAC)))))),_tmpAAB)))))),_tmpAAD)))),((_tmpAAA->tl=tms,_tmpAAA))))));}
goto AfterAtts;_LL2E2:;_LL2E3: goto _LL2DB;_LL2DB:;}break;}AfterAtts: if(_tmp584 != 0){
struct Cyc_Absyn_TypeParams_mod_struct*_tmpAB3;struct Cyc_Absyn_TypeParams_mod_struct
_tmpAB2;struct Cyc_List_List*_tmpAB1;tms=((_tmpAB1=_region_malloc(r,sizeof(*
_tmpAB1)),((_tmpAB1->hd=(void*)((_tmpAB3=_region_malloc(r,sizeof(*_tmpAB3)),((
_tmpAB3[0]=((_tmpAB2.tag=4,((_tmpAB2.f1=_tmp584,((_tmpAB2.f2=0,((_tmpAB2.f3=1,
_tmpAB2)))))))),_tmpAB3)))),((_tmpAB1->tl=tms,_tmpAB1))))));}{struct _tuple12
_tmpAB4;return(_tmpAB4.f1=_tmp5A7,((_tmpAB4.f2=_tmp5A8,((_tmpAB4.f3=tms,_tmpAB4)))));};};};
_LL2D2: {struct Cyc_Absyn_Evar_struct*_tmp58C=(struct Cyc_Absyn_Evar_struct*)
_tmp575;if(_tmp58C->tag != 1)goto _LL2D4;else{_tmp58D=_tmp58C->f1;_tmp58E=_tmp58C->f2;
_tmp58F=_tmp58C->f3;}}_LL2D3: if(_tmp58E == 0){struct _tuple12 _tmpAB5;return(
_tmpAB5.f1=tq,((_tmpAB5.f2=t,((_tmpAB5.f3=0,_tmpAB5)))));}else{return Cyc_Absynpp_to_tms(
r,tq,(void*)_tmp58E->v);}_LL2D4: {struct Cyc_Absyn_TypedefType_struct*_tmp590=(
struct Cyc_Absyn_TypedefType_struct*)_tmp575;if(_tmp590->tag != 18)goto _LL2D6;
else{_tmp591=_tmp590->f1;_tmp592=_tmp590->f2;_tmp593=_tmp590->f4;}}_LL2D5: if((
_tmp593 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(*
_tmp593)){struct _tuple12 _tmpAB6;return(_tmpAB6.f1=tq,((_tmpAB6.f2=t,((_tmpAB6.f3=
0,_tmpAB6)))));}else{return Cyc_Absynpp_to_tms(r,tq,*_tmp593);}_LL2D6:;_LL2D7: {
struct _tuple12 _tmpAB7;return(_tmpAB7.f1=tq,((_tmpAB7.f2=t,((_tmpAB7.f3=0,_tmpAB7)))));}
_LL2CB:;}static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp5C5=t;struct Cyc_Core_Opt*
_tmp5C7;struct Cyc_Core_Opt _tmp5C8;void*_tmp5C9;struct Cyc_Absyn_PtrInfo _tmp5CB;
void*_tmp5CC;_LL2E5: {struct Cyc_Absyn_Evar_struct*_tmp5C6=(struct Cyc_Absyn_Evar_struct*)
_tmp5C5;if(_tmp5C6->tag != 1)goto _LL2E7;else{_tmp5C7=_tmp5C6->f2;if(_tmp5C7 == 0)
goto _LL2E7;_tmp5C8=*_tmp5C7;_tmp5C9=(void*)_tmp5C8.v;}}_LL2E6: return Cyc_Absynpp_is_char_ptr(
_tmp5C9);_LL2E7: {struct Cyc_Absyn_PointerType_struct*_tmp5CA=(struct Cyc_Absyn_PointerType_struct*)
_tmp5C5;if(_tmp5CA->tag != 5)goto _LL2E9;else{_tmp5CB=_tmp5CA->f1;_tmp5CC=_tmp5CB.elt_typ;}}
_LL2E8: L: {void*_tmp5CD=_tmp5CC;struct Cyc_Core_Opt*_tmp5CF;struct Cyc_Core_Opt
_tmp5D0;void*_tmp5D1;void**_tmp5D3;void*_tmp5D4;enum Cyc_Absyn_Size_of _tmp5D6;
_LL2EC: {struct Cyc_Absyn_Evar_struct*_tmp5CE=(struct Cyc_Absyn_Evar_struct*)
_tmp5CD;if(_tmp5CE->tag != 1)goto _LL2EE;else{_tmp5CF=_tmp5CE->f2;if(_tmp5CF == 0)
goto _LL2EE;_tmp5D0=*_tmp5CF;_tmp5D1=(void*)_tmp5D0.v;}}_LL2ED: _tmp5CC=_tmp5D1;
goto L;_LL2EE: {struct Cyc_Absyn_TypedefType_struct*_tmp5D2=(struct Cyc_Absyn_TypedefType_struct*)
_tmp5CD;if(_tmp5D2->tag != 18)goto _LL2F0;else{_tmp5D3=_tmp5D2->f4;if(_tmp5D3 == 0)
goto _LL2F0;_tmp5D4=*_tmp5D3;}}_LL2EF: _tmp5CC=_tmp5D4;goto L;_LL2F0: {struct Cyc_Absyn_IntType_struct*
_tmp5D5=(struct Cyc_Absyn_IntType_struct*)_tmp5CD;if(_tmp5D5->tag != 6)goto _LL2F2;
else{_tmp5D6=_tmp5D5->f2;if(_tmp5D6 != Cyc_Absyn_Char_sz)goto _LL2F2;}}_LL2F1:
return 1;_LL2F2:;_LL2F3: return 0;_LL2EB:;}_LL2E9:;_LL2EA: return 0;_LL2E4:;}struct
Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*
dopt){struct _RegionHandle _tmp5D7=_new_region("temp");struct _RegionHandle*temp=&
_tmp5D7;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp5D9;void*_tmp5DA;struct Cyc_List_List*
_tmp5DB;struct _tuple12 _tmp5D8=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5D9=_tmp5D8.f1;
_tmp5DA=_tmp5D8.f2;_tmp5DB=_tmp5D8.f3;_tmp5DB=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5DB);if(_tmp5DB == 0  && dopt == 0){struct
Cyc_PP_Doc*_tmpAB8[2];struct Cyc_PP_Doc*_tmp5DD=(_tmpAB8[1]=Cyc_Absynpp_ntyp2doc(
_tmp5DA),((_tmpAB8[0]=Cyc_Absynpp_tqual2doc(_tmp5D9),Cyc_PP_cat(_tag_dyneither(
_tmpAB8,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5DD;}else{
const char*_tmpABB;struct Cyc_PP_Doc*_tmpABA[4];struct Cyc_PP_Doc*_tmp5E0=(_tmpABA[
3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(
struct Cyc_PP_Doc*)dopt->v,_tmp5DB),((_tmpABA[2]=Cyc_PP_text(((_tmpABB=" ",
_tag_dyneither(_tmpABB,sizeof(char),2)))),((_tmpABA[1]=Cyc_Absynpp_ntyp2doc(
_tmp5DA),((_tmpABA[0]=Cyc_Absynpp_tqual2doc(_tmp5D9),Cyc_PP_cat(_tag_dyneither(
_tmpABA,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5E0;}};
_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f){switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2F4: if(f->width != 0){
struct Cyc_Core_Opt*_tmpAC2;const char*_tmpAC1;const char*_tmpAC0;struct Cyc_PP_Doc*
_tmpABF[5];return(_tmpABF[4]=Cyc_PP_text(((_tmpAC0=";",_tag_dyneither(_tmpAC0,
sizeof(char),2)))),((_tmpABF[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpABF[2]=
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpABF[1]=
Cyc_PP_text(((_tmpAC1=":",_tag_dyneither(_tmpAC1,sizeof(char),2)))),((_tmpABF[0]=
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAC2=_cycalloc(sizeof(*_tmpAC2)),((
_tmpAC2->v=Cyc_PP_textptr(f->name),_tmpAC2))))),Cyc_PP_cat(_tag_dyneither(
_tmpABF,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*_tmpAC7;
const char*_tmpAC6;struct Cyc_PP_Doc*_tmpAC5[3];return(_tmpAC5[2]=Cyc_PP_text(((
_tmpAC6=";",_tag_dyneither(_tmpAC6,sizeof(char),2)))),((_tmpAC5[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmpAC5[0]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAC7=
_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7->v=Cyc_PP_textptr(f->name),_tmpAC7))))),
Cyc_PP_cat(_tag_dyneither(_tmpAC5,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c:
_LL2F5: if(f->width != 0){struct Cyc_Core_Opt*_tmpACE;const char*_tmpACD;const char*
_tmpACC;struct Cyc_PP_Doc*_tmpACB[5];return(_tmpACB[4]=Cyc_PP_text(((_tmpACC=";",
_tag_dyneither(_tmpACC,sizeof(char),2)))),((_tmpACB[3]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpACB[2]=Cyc_PP_text(((_tmpACD=":",
_tag_dyneither(_tmpACD,sizeof(char),2)))),((_tmpACB[1]=Cyc_Absynpp_tqtd2doc(f->tq,
f->type,((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE->v=Cyc_PP_textptr(f->name),
_tmpACE))))),((_tmpACB[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpACB,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{struct Cyc_Core_Opt*
_tmpAD3;const char*_tmpAD2;struct Cyc_PP_Doc*_tmpAD1[3];return(_tmpAD1[2]=Cyc_PP_text(((
_tmpAD2=";",_tag_dyneither(_tmpAD2,sizeof(char),2)))),((_tmpAD1[1]=Cyc_Absynpp_tqtd2doc(
f->tq,f->type,((_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3->v=Cyc_PP_textptr(
f->name),_tmpAD3))))),((_tmpAD1[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dyneither(_tmpAD1,sizeof(struct Cyc_PP_Doc*),3)))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){const char*_tmpAD4;return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct
_dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((
_tmpAD4="",_tag_dyneither(_tmpAD4,sizeof(char),1))),fields);}struct Cyc_PP_Doc*
Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){struct Cyc_PP_Doc*
_tmpAD5[3];return(_tmpAD5[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((
_tmpAD5[1]=Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAD5[0]=Cyc_Absynpp_scope2doc(
f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAD5,sizeof(struct Cyc_PP_Doc*),3)))))));}
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAD6;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),
struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((
_tmpAD6=",",_tag_dyneither(_tmpAD6,sizeof(char),2))),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(
Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{const char*_tmpAD9;
void*_tmpAD8;(_tmpAD8=0,Cyc_fprintf(f,((_tmpAD9="\n",_tag_dyneither(_tmpAD9,
sizeof(char),2))),_tag_dyneither(_tmpAD8,sizeof(void*),0)));};}}struct
_dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmpADA;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpADA="",_tag_dyneither(
_tmpADA,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),
72);}struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(
Cyc_Absynpp_exp2doc(e),72);}struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct
Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(
Cyc_Absynpp_typ2doc(t),72);}struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dyneither_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;};}struct _dyneither_ptr Cyc_Absynpp_prim2string(enum 
Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc(p),72);}struct _dyneither_ptr Cyc_Absynpp_scope2string(enum 
Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
