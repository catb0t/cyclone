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
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

// A dynamic region is just a region handle.  We have the
// wrapper struct for type abstraction reasons.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
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
extern int _throw_null_fn(const char *filename, unsigned lineno);
extern int _throw_arraybounds_fn(const char *filename, unsigned lineno);
extern int _throw_badalloc_fn(const char *filename, unsigned lineno);
extern int _throw_match_fn(const char *filename, unsigned lineno);
extern int _throw_fn(void* e, const char *filename, unsigned lineno);
extern int _rethrow(void* e);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

extern struct _xtunion_struct *_exn_thrown;

/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
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
_check_null_fn(const void *ptr, const char *filename, unsigned lineno) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null_fn(filename,lineno);
  return _check_null_temp;
}
#define _check_null(p) (_check_null_fn((p),__FILE__,__LINE__))
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
_check_known_subscript_null_fn(void *ptr, unsigned bound, unsigned elt_sz, unsigned index, const char *filename, unsigned lineno) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null_fn(filename,lineno);
  if (_cks_index >= _cks_bound) _throw_arraybounds_fn(filename,lineno);
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
#define _check_known_subscript_null(p,b,e) (_check_known_subscript_null_fn(p,b,e,__FILE__,__LINE__))
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
_check_known_subscript_notnull_fn(unsigned bound,unsigned index,const char *filename,unsigned lineno) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds_fn(filename,lineno); 
  return _cksnn_index;
}
#define _check_known_subscript_notnull(b,i) (_check_known_subscript_notnull_fn(b,i,__FILE__,__LINE__))
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
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char_fn(char *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short_fn(short *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int_fn(int *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float_fn(float *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double_fn(double *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble_fn(long double *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar_fn(void **orig_x, unsigned int orig_sz, int orig_i,const char *filename,unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
#endif

#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_short(x,s,i) \
  (_zero_arr_plus_short_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_int(x,s,i) \
  (_zero_arr_plus_int_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_float(x,s,i) \
  (_zero_arr_plus_float_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_double(x,s,i) \
  (_zero_arr_plus_double_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_longdouble(x,s,i) \
  (_zero_arr_plus_longdouble_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_voidstar(x,s,i) \
  (_zero_arr_plus_voidstar_fn(x,s,i,__FILE__,__LINE__))


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
   Note that this expands to call _zero_arr_plus_<type>_fn. */
static _INLINE char *
_zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_char_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_short_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_int_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_float_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_double_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_longdouble_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
static _INLINE void *
_zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_voidstar_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)

/* Does in-place increment of a zero-terminated pointer (e.g., x++). */
static _INLINE char *
_zero_arr_inplace_plus_post_char_fn(char **x, int orig_i,const char *filename,unsigned lineno){
  char * _zap_res = *x;
  *x = _zero_arr_plus_char_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_post_short_fn(short **x, int orig_i,const char *filename,unsigned lineno){
  short * _zap_res = *x;
  *x = _zero_arr_plus_short_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_post_int_fn(int **x, int orig_i,const char *filename, unsigned lineno){
  int * _zap_res = *x;
  *x = _zero_arr_plus_int_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_post_float_fn(float **x, int orig_i,const char *filename, unsigned lineno){
  float * _zap_res = *x;
  *x = _zero_arr_plus_float_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_post_double_fn(double **x, int orig_i,const char *filename,unsigned lineno){
  double * _zap_res = *x;
  *x = _zero_arr_plus_double_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno){
  long double * _zap_res = *x;
  *x = _zero_arr_plus_longdouble_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno){
  void ** _zap_res = *x;
  *x = _zero_arr_plus_voidstar_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

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
_check_dyneither_subscript_fn(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index,const char *filename, unsigned lineno) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _cus_ans;
}
#define _check_dyneither_subscript(a,s,i) \
  _check_dyneither_subscript_fn(a,s,i,__FILE__,__LINE__)
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
_untag_dyneither_ptr_fn(struct _dyneither_ptr arr, 
                        unsigned elt_sz,unsigned num_elts,
                        const char *filename, unsigned lineno) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
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

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static _INLINE void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST && r->curr != 0) { 
#ifdef CYC_NOALIGN
    unsigned s =  orig_s;
#else
    unsigned s =  (orig_s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT -1)); 
#endif
    char *result; 
    result = r->offset; 
    if (s <= (r->last_plus_one - result)) {
      r->offset = result + s; 
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
      return result;
    }
  } 
  return _region_malloc(r,orig_s); 
}

/* FIX?  Not sure if we want to pass filename and lineno in here... */
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
extern void* _profile_GC_malloc(int,const char *file,const char *func,
                                int lineno);
extern void* _profile_GC_malloc_atomic(int,const char *file,
                                       const char *func,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern void* _profile_region_calloc(struct _RegionHandle *, unsigned,
                                    unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						const char *file,
						const char *func,
                                                int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 const char *file,
                                 const char *func,
                                 int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
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

# 35 "core.h"
 typedef char*Cyc_Cstring;
typedef char*Cyc_CstringNN;
typedef struct _dyneither_ptr Cyc_string_t;
# 40
typedef struct _dyneither_ptr Cyc_mstring_t;
# 43
typedef struct _dyneither_ptr*Cyc_stringptr_t;
# 47
typedef struct _dyneither_ptr*Cyc_mstringptr_t;
# 50
typedef char*Cyc_Cbuffer_t;
# 52
typedef char*Cyc_CbufferNN_t;
# 54
typedef struct _dyneither_ptr Cyc_buffer_t;
# 56
typedef struct _dyneither_ptr Cyc_mbuffer_t;
# 59
typedef int Cyc_bool;
# 69 "core.h"
void exit(int);
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;
# 38
int Cyc_open(const char*,int,struct _dyneither_ptr);struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);
# 106
int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);
# 310
int toupper(int);
# 318
int system(const char*);
# 321
int mkdir(const char*pathname,unsigned short mode);
# 324
int close(int);
int chdir(const char*);
struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 288 "core.h"
struct _dyneither_ptr Cstring_to_string(char*);
# 299
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;
# 303
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 308
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 315
return 0;
return dyn.curr;};}extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 75
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 82
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 85
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 102 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 39
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 32 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 37
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 66
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;
# 49
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 57
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 70
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 81 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);struct Cyc_Relations_Reln;
# 77 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 83
typedef void*Cyc_Tcpat_decision_opt_t;
# 91
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 103
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 107
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 110
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 115
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
typedef void*Cyc_Absyn_kindbound_t;
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
typedef void*Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void*Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
typedef void*Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
typedef void*Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
typedef void*Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_opt_t;
typedef void*Cyc_Absyn_raw_decl_t;
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
typedef void*Cyc_Absyn_designator_t;
typedef void*Cyc_Absyn_absyn_annot_t;
typedef void*Cyc_Absyn_attribute_t;
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
typedef void*Cyc_Absyn_offsetof_field_t;
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
typedef struct Cyc_Absyn_PtrLoc*Cyc_Absyn_ptrloc_t;
# 166
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 187
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 192
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 199
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 219
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 221
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 230
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 291
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 390
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 395
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 466 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 469
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 533
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 559
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 566
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 584
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1137 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1200
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 32 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 36
typedef struct Cyc_FlatList*Cyc_flat_list_t;struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 47
typedef struct Cyc_Type_specifier Cyc_type_specifier_t;struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 52
typedef struct Cyc_Declarator Cyc_declarator_t;struct _tuple12{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};
typedef struct _tuple11*Cyc_declarator_list_t;
# 55
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};
# 59
typedef enum Cyc_Storage_class Cyc_storage_class_t;struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 67
typedef struct Cyc_Declaration_spec Cyc_decl_spec_t;struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};
# 71
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple14{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple17{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 78 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 33 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 35
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 42
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 48
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 51
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 44 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 55
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 74
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 78
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
# 89
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 101
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 119 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 139
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 36 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 81 "buildlib.cyl"
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 89
extern char*Ccomp;
# 91
static int Cyc_do_setjmp=0;
static int Cyc_verbose=0;
# 94
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 98
int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 101
if(Cyc_log_file == 0){
({void*_tmp0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1="Internal error: log file is NULL\n";_tag_dyneither(_tmp1,sizeof(char),34);}),_tag_dyneither(_tmp0,sizeof(void*),0));});
exit(1);}{
# 105
int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);
Cyc_fflush((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
return _tmp2;};}
# 110
static struct _dyneither_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _dyneither_ptr*sptr){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);_tmp3;});}struct _tuple20{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
# 117
typedef struct _tuple20*Cyc_line_t;
struct _tuple20*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 124
struct Cyc___cycFILE*Cyc_slurp_out=0;
int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple21{struct _dyneither_ptr f1;struct _dyneither_ptr*f2;};
# 131
typedef struct _tuple21*Cyc_suck_line_t;
struct _tuple21*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple22{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 139
typedef struct _tuple22*Cyc_ifdefined_t;struct _tuple23{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};
# 141
typedef struct _tuple23*Cyc_spec_t;
# 143
struct _tuple23*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile={(void*)0,(void*)0,(void*)(0 + 0)};
struct Cyc_List_List*Cyc_snarfed_symbols=0;
struct Cyc_List_List*Cyc_current_symbols=0;
struct Cyc_List_List*Cyc_current_cstubs=0;
struct Cyc_List_List*Cyc_current_cycstubs=0;
struct Cyc_List_List*Cyc_current_hstubs=0;
struct Cyc_List_List*Cyc_current_omit_symbols=0;
struct Cyc_List_List*Cyc_current_cpp=0;
struct Cyc_Buffer_t*Cyc_specbuf=0;
int Cyc_braces_to_match=0;
int Cyc_parens_to_match=0;
# 161
int Cyc_numdef=0;
# 163
static struct Cyc_List_List*Cyc_cppargs=0;
# 166
const int Cyc_lex_base[425]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,33,23,48,70,54,64,74,100,91,107,94,370,386,111,103,95,122,122,375,414,111,111,153,377,1024,1139,525,176,210,228,214,216,217,242,498,1253,1368,- 9,654,- 10,224,245,508,1482,1597,684,- 8,720,- 11,432,510,515,1674,1751,1828,1909,434,465,550,1984,249,249,249,248,244,254,0,13,4,2065,5,628,2073,2138,660,49,467,6,2099,7,705,2161,2199,820,- 22,1051,1056,261,314,242,251,258,250,271,281,274,275,278,288,295,282,- 20,292,296,300,313,321,326,- 15,309,325,319,315,326,365,400,417,418,404,400,400,427,431,- 17,422,421,415,436,433,449,427,449,453,441,445,436,436,- 19,444,438,442,453,464,447,449,482,489,490,4,6,21,491,492,505,504,516,516,524,554,24,556,557,24,20,570,571,53,627,644,- 13,647,594,596,583,584,598,598,605,606,1,677,623,624,654,647,654,658,659,623,624,639,645,648,703,704,705,654,655,710,720,721,679,680,735,737,765,713,714,770,797,798,746,747,803,814,815,- 12,762,763,1030,- 21,1165,762,763,760,773,772,767,769,772,773,771,827,1144,819,820,818,832,1258,1170,871,872,862,864,862,875,1370,893,894,892,905,1375,- 7,- 8,8,1263,2231,9,996,2255,2293,1341,1279,- 49,1150,- 2,945,- 4,974,998,1028,980,1010,1023,1448,981,2320,2363,994,1055,993,996,2433,994,1057,- 36,- 42,- 37,2508,- 28,1002,- 40,- 25,1021,- 27,- 45,- 39,- 48,2583,2612,1467,1081,1091,1563,2622,2652,1582,2281,2685,2716,2754,1083,1093,2824,2862,1118,1157,1195,1206,1198,1241,- 6,- 34,1040,2794,- 47,- 30,- 32,- 46,- 29,- 31,- 33,1048,2902,1112,1117,2128,1119,1124,1125,1133,1134,1138,1146,1147,1191,2975,3059,- 23,- 16,- 18,2239,1192,- 24,- 41,- 38,- 35,- 26,1394,3141,2,3224,1191,15,1170,1174,1175,1173,1171,1185,1255};
const int Cyc_lex_backtrk[425]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,- 1,21,21,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[425]={- 1,- 1,- 1,317,306,138,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,103,- 1,- 1,- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,386,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3481]={0,0,0,0,0,0,0,0,0,0,22,19,28,417,19,28,19,28,100,19,45,45,45,45,45,22,45,0,0,0,0,0,21,234,21,418,203,22,- 1,- 1,22,- 1,- 1,45,204,45,224,22,415,415,415,415,415,415,415,415,415,415,31,103,22,205,114,40,214,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,31,217,218,221,415,130,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,413,413,413,413,413,413,413,413,413,413,121,20,74,67,54,55,56,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,57,58,59,60,413,61,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,62,63,37,318,319,318,318,319,39,22,64,65,68,69,70,129,34,34,34,34,34,34,34,34,71,72,318,320,321,75,76,322,323,324,104,104,325,326,104,327,328,329,330,331,331,331,331,331,331,331,331,331,332,77,333,334,335,104,19,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,19,- 1,- 1,337,336,101,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,307,338,37,308,139,139,24,24,139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,309,88,95,139,96,22,26,26,21,21,115,116,117,140,118,119,120,142,192,193,26,35,35,35,35,35,35,35,35,178,172,163,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,150,78,151,152,153,154,66,66,310,155,66,157,158,66,132,123,141,39,73,22,78,159,143,144,145,160,146,161,27,66,147,31,162,21,73,73,164,165,73,148,166,167,168,113,113,113,113,113,113,113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,169,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,89,89,28,170,89,171,100,173,174,175,97,97,104,104,97,176,104,112,112,177,179,112,180,89,181,105,105,80,80,105,19,80,21,97,182,104,183,184,185,186,112,187,188,189,190,191,271,194,105,195,80,196,112,112,91,197,112,198,199,21,21,21,106,107,106,106,106,106,106,106,106,106,106,106,21,112,200,201,202,206,207,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,208,209,210,211,106,212,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,47,47,35,213,47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,219,220,92,222,47,33,33,33,33,33,33,128,128,128,128,128,128,128,128,48,223,92,225,- 1,226,- 1,227,228,99,99,229,49,99,230,231,232,233,33,33,33,33,33,33,35,35,35,35,35,35,35,35,99,235,236,237,265,- 1,238,- 1,41,41,239,260,41,101,101,255,250,101,243,240,50,241,244,245,246,51,52,247,248,249,46,41,53,251,252,253,101,137,137,137,137,137,137,137,137,254,93,42,42,42,42,42,42,42,42,42,42,242,256,257,258,93,259,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,102,100,261,262,42,263,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,264,91,43,266,267,102,268,33,33,33,33,33,33,33,33,33,33,269,270,272,273,288,283,279,33,33,33,33,33,33,35,35,35,35,35,35,35,35,280,281,282,36,284,285,286,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,287,289,290,291,44,292,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,301,296,297,28,298,299,300,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,302,303,304,305,44,411,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,410,408,78,- 1,273,273,385,376,273,315,315,315,315,315,315,315,315,351,383,346,341,78,343,344,409,293,293,273,345,293,275,275,- 1,380,275,274,384,79,79,79,79,79,79,79,79,79,79,350,293,381,382,379,407,275,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,347,348,349,339,79,387,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,80,294,155,80,287,287,276,191,287,274,412,295,412,412,404,102,277,100,100,35,35,278,80,270,223,275,275,287,46,275,293,293,340,412,293,19,162,403,79,79,79,79,79,79,79,79,79,79,275,100,100,35,35,293,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,177,138,21,31,79,31,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,276,31,89,292,292,294,419,292,316,316,277,420,316,421,422,295,36,423,36,375,- 1,89,424,21,316,316,292,0,316,0,0,316,0,21,31,0,0,90,90,90,90,90,90,90,90,90,90,316,36,21,36,375,0,375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,375,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,91,92,92,300,300,92,0,300,305,305,0,0,305,31,31,31,31,31,31,31,31,0,0,0,92,0,300,412,0,412,412,305,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,412,0,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,0,377,97,378,378,378,378,378,378,378,378,378,378,0,0,0,0,357,0,357,0,97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,36,99,99,0,0,99,358,358,358,358,358,358,358,358,358,358,0,0,0,0,361,0,361,0,99,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,0,0,- 1,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,111,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,28,0,0,124,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,0,0,28,0,0,133,125,125,125,125,125,125,125,125,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0,127,127,127,127,127,127,0,0,0,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,31,0,0,- 1,0,138,0,0,127,127,127,127,127,127,405,405,405,405,405,405,405,405,0,126,127,127,127,127,127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,127,127,127,127,136,136,136,136,136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,136,136,136,136,0,0,0,127,127,127,127,127,127,19,0,0,311,0,0,136,136,136,136,136,136,136,136,136,136,0,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,0,138,312,312,312,312,312,312,312,312,406,406,406,406,406,406,406,406,0,136,136,136,136,136,136,0,314,314,314,314,314,314,314,314,314,314,0,0,0,0,0,0,0,314,314,314,314,314,314,0,0,28,362,362,362,362,362,362,362,362,362,362,0,0,314,314,314,314,314,314,314,314,314,314,313,314,314,314,314,314,314,314,314,314,314,314,314,0,0,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,314,314,314,314,314,314,365,0,0,0,0,0,0,0,0,366,0,0,367,352,0,353,353,353,353,353,353,353,353,353,353,354,0,0,0,0,0,0,365,0,0,0,354,0,0,0,0,366,0,355,367,0,0,0,0,0,0,0,356,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,354,0,0,0,0,0,0,355,0,0,0,0,0,0,0,0,356,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,0,0,0,0,342,0,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,359,359,359,359,359,359,359,359,359,359,0,0,0,0,0,0,0,0,0,0,0,360,93,0,0,0,0,352,93,353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,0,354,0,0,360,93,0,0,355,0,0,93,91,0,0,0,0,356,91,0,359,359,359,359,359,359,359,359,359,359,0,0,0,354,0,0,0,0,0,0,355,360,93,0,91,0,0,0,93,356,91,0,0,362,362,362,362,362,362,362,362,362,362,0,0,0,0,0,0,0,0,0,0,360,93,93,0,0,0,0,93,93,352,0,363,363,363,363,363,363,363,363,364,364,0,0,0,0,0,0,0,0,0,0,0,354,0,93,0,0,0,0,373,93,0,0,0,0,0,0,352,374,364,364,364,364,364,364,364,364,364,364,0,0,0,0,0,354,0,0,0,0,0,354,373,0,0,0,0,0,371,0,0,374,0,0,0,0,0,372,0,0,378,378,378,378,378,378,378,378,378,378,0,0,0,354,0,0,0,0,0,0,371,360,93,0,0,0,0,0,93,372,368,368,368,368,368,368,368,368,368,368,0,0,0,0,0,0,0,368,368,368,368,368,368,360,93,0,0,0,0,0,93,0,0,0,0,0,0,0,368,368,368,368,368,368,368,368,368,368,0,368,368,368,368,368,368,368,368,368,368,368,368,0,0,0,388,0,369,0,0,389,0,0,0,0,0,370,0,0,390,390,390,390,390,390,390,390,0,368,368,368,368,368,368,391,0,0,0,0,369,0,0,0,0,0,0,0,0,370,0,0,0,0,0,0,0,0,0,0,0,0,0,0,392,0,0,0,0,393,394,0,0,0,395,0,0,0,0,0,0,0,396,0,0,0,397,0,398,0,399,0,400,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,402,0,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,0,0,0,0,0,0,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,19,0,0,414,0,0,0,413,413,413,413,413,413,413,413,413,413,0,0,0,0,0,0,0,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,0,0,0,0,413,0,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,416,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,0,0,0,0,0,0,0,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,0,0,415,0,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3481]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,122,124,131,133,308,311,41,41,45,45,41,417,45,- 1,- 1,- 1,- 1,- 1,120,233,414,0,202,10,12,40,10,12,40,41,202,45,203,20,1,1,1,1,1,1,1,1,1,1,38,48,121,204,10,38,213,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,129,216,217,220,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,61,62,7,3,3,3,3,3,7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,30,70,71,3,3,3,74,75,3,3,3,47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,50,85,27,86,17,27,50,17,4,87,94,5,95,5,6,8,9,18,114,115,116,5,117,118,119,141,143,143,17,34,34,34,34,34,34,34,34,144,145,146,27,27,27,27,27,27,27,27,65,65,147,148,65,72,72,77,77,72,149,77,150,151,152,153,66,66,4,154,66,156,157,65,6,8,5,16,72,16,77,158,142,142,142,159,142,160,17,66,142,27,161,7,73,73,163,164,73,142,165,166,167,11,11,11,11,11,11,11,11,11,11,103,27,110,103,73,110,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,111,168,130,111,11,130,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,88,88,66,169,88,170,171,172,173,174,96,96,104,104,96,175,104,105,105,176,178,105,179,88,180,13,13,80,80,13,73,80,16,96,181,104,182,183,184,185,105,186,187,188,189,190,192,193,13,194,80,195,112,112,4,196,112,197,198,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,112,199,200,201,205,206,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,207,208,209,210,13,211,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,80,212,14,214,215,32,32,32,32,32,32,32,32,32,32,92,92,218,219,92,221,14,32,32,32,32,32,32,125,125,125,125,125,125,125,125,14,222,92,224,103,225,110,226,227,99,99,228,14,99,229,230,231,232,32,32,32,32,32,32,128,128,128,128,128,128,128,128,99,234,235,236,238,111,237,130,15,15,237,239,15,101,101,240,241,101,242,237,14,237,243,244,245,14,14,246,247,248,249,15,14,250,251,252,101,134,134,134,134,134,134,134,134,253,254,15,15,15,15,15,15,15,15,15,15,237,255,256,257,92,258,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,259,99,260,261,15,262,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,263,264,15,265,266,101,267,33,33,33,33,33,33,33,33,33,33,268,269,271,272,276,277,278,33,33,33,33,33,33,137,137,137,137,137,137,137,137,279,280,281,282,283,284,285,42,42,42,42,42,42,42,42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,286,288,289,290,42,291,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,294,295,296,15,297,298,299,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,301,302,303,304,44,320,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,322,323,78,324,273,273,325,329,273,312,312,312,312,312,312,312,312,332,326,334,337,78,335,335,323,139,139,273,344,139,140,140,324,327,140,273,326,78,78,78,78,78,78,78,78,78,78,347,139,327,327,377,386,140,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,333,333,333,338,78,324,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,79,79,139,388,79,287,287,140,389,287,391,318,139,318,318,392,393,140,355,356,365,366,140,79,394,395,275,275,287,396,275,293,293,338,318,293,287,397,398,79,79,79,79,79,79,79,79,79,79,275,355,356,365,366,293,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,399,406,416,370,79,369,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,275,370,89,292,292,293,418,292,309,309,275,419,309,420,421,293,371,422,372,373,324,89,423,424,316,316,292,- 1,316,- 1,- 1,309,- 1,309,292,- 1,- 1,89,89,89,89,89,89,89,89,89,89,316,371,316,372,373,- 1,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,89,374,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,300,300,90,- 1,300,305,305,- 1,- 1,305,315,315,315,315,315,315,315,315,- 1,- 1,- 1,90,- 1,300,412,- 1,412,412,305,- 1,- 1,300,- 1,- 1,- 1,- 1,305,90,90,90,90,90,90,90,90,90,90,412,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,- 1,328,97,328,328,328,328,328,328,328,328,328,328,- 1,- 1,- 1,- 1,354,- 1,354,- 1,97,354,354,354,354,354,354,354,354,354,354,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,98,98,- 1,- 1,98,357,357,357,357,357,357,357,357,357,357,- 1,- 1,- 1,- 1,360,- 1,360,- 1,98,360,360,360,360,360,360,360,360,360,360,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,- 1,- 1,109,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,123,- 1,- 1,123,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,132,- 1,- 1,132,123,123,123,123,123,123,123,123,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,- 1,- 1,- 1,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,109,- 1,390,- 1,- 1,126,126,126,126,126,126,390,390,390,390,390,390,390,390,- 1,123,127,127,127,127,127,127,127,127,127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,135,135,135,135,135,135,135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,- 1,- 1,- 1,127,127,127,127,127,127,310,- 1,- 1,310,- 1,- 1,136,136,136,136,136,136,136,136,136,136,- 1,135,135,135,135,135,135,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,405,310,310,310,310,310,310,310,310,405,405,405,405,405,405,405,405,- 1,136,136,136,136,136,136,- 1,313,313,313,313,313,313,313,313,313,313,- 1,- 1,- 1,- 1,- 1,- 1,- 1,313,313,313,313,313,313,- 1,- 1,310,361,361,361,361,361,361,361,361,361,361,- 1,- 1,314,314,314,314,314,314,314,314,314,314,310,313,313,313,313,313,313,314,314,314,314,314,314,- 1,- 1,330,- 1,330,330,330,330,330,330,330,330,330,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,314,314,314,314,314,314,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,330,331,- 1,331,331,331,331,331,331,331,331,331,331,330,- 1,- 1,- 1,- 1,- 1,- 1,330,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,330,- 1,331,330,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,- 1,- 1,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,- 1,- 1,- 1,- 1,336,- 1,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,- 1,- 1,- 1,- 1,342,- 1,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,342,352,352,352,352,352,352,352,352,352,352,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,352,352,- 1,- 1,- 1,- 1,353,352,353,353,353,353,353,353,353,353,353,353,358,358,358,358,358,358,358,358,358,358,- 1,353,- 1,- 1,352,352,- 1,- 1,353,- 1,- 1,352,358,- 1,- 1,- 1,- 1,353,358,- 1,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,353,- 1,- 1,- 1,- 1,- 1,- 1,353,359,359,- 1,358,- 1,- 1,- 1,359,353,358,- 1,- 1,362,362,362,362,362,362,362,362,362,362,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,362,- 1,- 1,- 1,- 1,359,362,363,- 1,363,363,363,363,363,363,363,363,363,363,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,363,- 1,362,- 1,- 1,- 1,- 1,363,362,- 1,- 1,- 1,- 1,- 1,- 1,364,363,364,364,364,364,364,364,364,364,364,364,- 1,- 1,- 1,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,363,- 1,- 1,- 1,- 1,- 1,364,- 1,- 1,378,378,378,378,378,378,378,378,378,378,- 1,- 1,- 1,364,- 1,- 1,- 1,- 1,- 1,- 1,364,378,378,- 1,- 1,- 1,- 1,- 1,378,364,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,367,367,367,367,367,378,378,- 1,- 1,- 1,- 1,- 1,378,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,368,368,368,368,368,368,368,368,368,- 1,367,367,367,367,367,367,368,368,368,368,368,368,- 1,- 1,- 1,387,- 1,368,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,387,387,387,387,387,387,387,387,- 1,368,368,368,368,368,368,387,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,368,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,387,387,- 1,- 1,- 1,387,- 1,- 1,- 1,- 1,- 1,- 1,- 1,387,- 1,- 1,- 1,387,- 1,387,- 1,387,- 1,387,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,- 1,- 1,- 1,- 1,- 1,- 1,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,400,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,- 1,- 1,- 1,- 1,- 1,- 1,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,401,413,- 1,- 1,413,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,- 1,- 1,- 1,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,- 1,- 1,- 1,- 1,413,- 1,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,413,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,415,- 1,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,415,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 173
int state;int base;int backtrk;
int c;
state=start_state;
# 177
if(state >= 0){
lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);
lbuf->lex_last_action=- 1;}else{
# 181
state=(- state)- 1;}
# 183
while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(425,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[_check_known_subscript_notnull(425,state)];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 191
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 195
c=256;}}else{
# 197
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 200
if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]== state)
state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
# 203
state=Cyc_lex_default[_check_known_subscript_notnull(425,state)];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4[0]=({struct Cyc_Lexing_Error_exn_struct _tmp5;_tmp5.tag=Cyc_Lexing_Error;_tmp5.f1=({const char*_tmp6="empty token";_tag_dyneither(_tmp6,sizeof(char),12);});_tmp5;});_tmp4;}));else{
# 209
return lbuf->lex_last_action;}}else{
# 212
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 216
struct _tuple20*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp7=lexstate;switch(_tmp7){case 0: _LL1: _LL2:
# 175 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);_tmp8;});}
# 180
return({struct _tuple20*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp9->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp9;});case 1: _LL3: _LL4:
# 183 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2: _LL5: _LL6:
# 185
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL0:;}
# 189
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=({struct Cyc_Lexing_Error_exn_struct _tmpB;_tmpB.tag=Cyc_Lexing_Error;_tmpB.f1=({const char*_tmpC="some action didn't return!";_tag_dyneither(_tmpC,sizeof(char),27);});_tmpB;});_tmpA;}));}
# 191
struct _tuple20*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpD=lexstate;switch(_tmpD){case 0: _LLA: _LLB:
# 189 "buildlib.cyl"
 Cyc_current_source=({struct _dyneither_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmpE;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmpF;});
Cyc_token(lexbuf);
return 0;case 1: _LLC: _LLD:
# 197
 Cyc_current_source=({struct _dyneither_ptr*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmp10;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp11;});
Cyc_args(lexbuf);
return 0;case 2: _LLE: _LLF:
# 205
 Cyc_current_source=({struct _dyneither_ptr*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp12;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp13;});
Cyc_token(lexbuf);
return 0;default: _LL10: _LL11:
# 211
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL9:;}
# 214
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({struct Cyc_Lexing_Error_exn_struct _tmp15;_tmp15.tag=Cyc_Lexing_Error;_tmp15.f1=({const char*_tmp16="some action didn't return!";_tag_dyneither(_tmp16,sizeof(char),27);});_tmp15;});_tmp14;}));}
# 216
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp17=lexstate;switch(_tmp17){case 0: _LL13: _LL14: {
# 214 "buildlib.cyl"
struct _dyneither_ptr*_tmp18=({struct _dyneither_ptr*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2));_tmp1A;});
Cyc_current_args=({struct Cyc_List_List*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->hd=_tmp18;_tmp19->tl=Cyc_current_args;_tmp19;});
return Cyc_args(lexbuf);}case 1: _LL15: _LL16: {
# 220
struct _dyneither_ptr*_tmp1B=({struct _dyneither_ptr*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmp1D;});
Cyc_current_args=({struct Cyc_List_List*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->hd=_tmp1B;_tmp1C->tl=Cyc_current_args;_tmp1C;});
return Cyc_args(lexbuf);}case 2: _LL17: _LL18: {
# 226
struct _dyneither_ptr*_tmp1E=({struct _dyneither_ptr*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));_tmp20;});
Cyc_current_args=({struct Cyc_List_List*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->hd=_tmp1E;_tmp1F->tl=Cyc_current_args;_tmp1F;});
return Cyc_token(lexbuf);}default: _LL19: _LL1A:
# 231
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL12:;}
# 234
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=({struct Cyc_Lexing_Error_exn_struct _tmp22;_tmp22.tag=Cyc_Lexing_Error;_tmp22.f1=({const char*_tmp23="some action didn't return!";_tag_dyneither(_tmp23,sizeof(char),27);});_tmp22;});_tmp21;}));}
# 236
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp24=lexstate;switch(_tmp24){case 0: _LL1C: _LL1D:
# 235 "buildlib.cyl"
 Cyc_add_target(({struct _dyneither_ptr*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp25;}));return Cyc_token(lexbuf);case 1: _LL1E: _LL1F:
# 238 "buildlib.cyl"
 return 0;case 2: _LL20: _LL21:
# 241 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3: _LL22: _LL23:
# 244 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL24: _LL25:
# 247 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5: _LL26: _LL27:
# 249
 return Cyc_token(lexbuf);case 6: _LL28: _LL29:
# 251
 return Cyc_token(lexbuf);case 7: _LL2A: _LL2B:
# 253
 return Cyc_token(lexbuf);case 8: _LL2C: _LL2D:
# 256 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9: _LL2E: _LL2F:
# 259 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10: _LL30: _LL31:
# 262 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11: _LL32: _LL33:
# 264
 return Cyc_token(lexbuf);case 12: _LL34: _LL35:
# 266
 return Cyc_token(lexbuf);case 13: _LL36: _LL37:
# 268
 return Cyc_token(lexbuf);case 14: _LL38: _LL39:
# 270
 return Cyc_token(lexbuf);case 15: _LL3A: _LL3B:
# 272
 return Cyc_token(lexbuf);case 16: _LL3C: _LL3D:
# 274
 return Cyc_token(lexbuf);case 17: _LL3E: _LL3F:
# 276
 return Cyc_token(lexbuf);case 18: _LL40: _LL41:
# 278
 return Cyc_token(lexbuf);case 19: _LL42: _LL43:
# 280
 return Cyc_token(lexbuf);case 20: _LL44: _LL45:
# 282
 return Cyc_token(lexbuf);case 21: _LL46: _LL47:
# 284
 return Cyc_token(lexbuf);case 22: _LL48: _LL49:
# 286
 return Cyc_token(lexbuf);case 23: _LL4A: _LL4B:
# 288
 return Cyc_token(lexbuf);case 24: _LL4C: _LL4D:
# 291 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25: _LL4E: _LL4F:
# 293
 return Cyc_token(lexbuf);case 26: _LL50: _LL51:
# 295
 return Cyc_token(lexbuf);case 27: _LL52: _LL53:
# 297
 return Cyc_token(lexbuf);case 28: _LL54: _LL55:
# 299
 return Cyc_token(lexbuf);case 29: _LL56: _LL57:
# 301
 return Cyc_token(lexbuf);case 30: _LL58: _LL59:
# 303
 return Cyc_token(lexbuf);case 31: _LL5A: _LL5B:
# 305
 return Cyc_token(lexbuf);case 32: _LL5C: _LL5D:
# 307
 return Cyc_token(lexbuf);case 33: _LL5E: _LL5F:
# 309
 return Cyc_token(lexbuf);case 34: _LL60: _LL61:
# 311
 return Cyc_token(lexbuf);case 35: _LL62: _LL63:
# 313
 return Cyc_token(lexbuf);case 36: _LL64: _LL65:
# 315
 return Cyc_token(lexbuf);case 37: _LL66: _LL67:
# 317
 return Cyc_token(lexbuf);case 38: _LL68: _LL69:
# 319
 return Cyc_token(lexbuf);case 39: _LL6A: _LL6B:
# 321
 return Cyc_token(lexbuf);case 40: _LL6C: _LL6D:
# 323
 return Cyc_token(lexbuf);case 41: _LL6E: _LL6F:
# 325
 return Cyc_token(lexbuf);case 42: _LL70: _LL71:
# 327
 return Cyc_token(lexbuf);case 43: _LL72: _LL73:
# 329
 return Cyc_token(lexbuf);case 44: _LL74: _LL75:
# 331
 return Cyc_token(lexbuf);case 45: _LL76: _LL77:
# 333
 return Cyc_token(lexbuf);case 46: _LL78: _LL79:
# 335
 return Cyc_token(lexbuf);case 47: _LL7A: _LL7B:
# 337
 return Cyc_token(lexbuf);case 48: _LL7C: _LL7D:
# 340 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL7E: _LL7F:
(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}_LL1B:;}
# 344
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26[0]=({struct Cyc_Lexing_Error_exn_struct _tmp27;_tmp27.tag=Cyc_Lexing_Error;_tmp27.f1=({const char*_tmp28="some action didn't return!";_tag_dyneither(_tmp28,sizeof(char),27);});_tmp27;});_tmp26;}));}
# 346
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp29=lexstate;switch(_tmp29){case 0: _LL81: _LL82:
# 345 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1: _LL83: _LL84:
# 346 "buildlib.cyl"
 return 0;case 2: _LL85: _LL86:
# 347 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3: _LL87: _LL88:
# 348 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4: _LL89: _LL8A:
# 351 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5: _LL8B: _LL8C:
# 354 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6: _LL8D: _LL8E:
# 356
 return Cyc_string(lexbuf);case 7: _LL8F: _LL90:
# 357 "buildlib.cyl"
 return 0;case 8: _LL91: _LL92:
# 358 "buildlib.cyl"
 return 0;case 9: _LL93: _LL94:
# 359 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL95: _LL96:
(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}_LL80:;}
# 363
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Lexing_Error_exn_struct _tmp2B;_tmp2B.tag=Cyc_Lexing_Error;_tmp2B.f1=({const char*_tmp2C="some action didn't return!";_tag_dyneither(_tmp2C,sizeof(char),27);});_tmp2B;});_tmp2A;}));}
# 365
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp2D=lexstate;switch(_tmp2D){case 0: _LL98: _LL99:
# 368 "buildlib.cyl"
 return 0;case 1: _LL9A: _LL9B:
# 370
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL9C: _LL9D:
# 377 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp2E=0;Cyc_log(({const char*_tmp2F="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmp2F,sizeof(char),44);}),_tag_dyneither(_tmp2E,sizeof(void*),0));});
return 1;case 3: _LL9E: _LL9F:
# 383 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp30=0;Cyc_log(({const char*_tmp31="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmp31,sizeof(char),44);}),_tag_dyneither(_tmp30,sizeof(void*),0));});
return 1;case 4: _LLA0: _LLA1:
# 389 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp32=0;Cyc_log(({const char*_tmp33="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmp33,sizeof(char),44);}),_tag_dyneither(_tmp32,sizeof(void*),0));});
return 1;case 5: _LLA2: _LLA3:
# 395 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp34=0;Cyc_log(({const char*_tmp35="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmp35,sizeof(char),44);}),_tag_dyneither(_tmp34,sizeof(void*),0));});
return 1;case 6: _LLA4: _LLA5:
# 399
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp36=0;Cyc_log(({const char*_tmp37="Warning: use of region sidestepped\n";_tag_dyneither(_tmp37,sizeof(char),36);}),_tag_dyneither(_tmp36,sizeof(void*),0));});
return 1;case 7: _LLA6: _LLA7:
# 403
({void*_tmp38=0;Cyc_log(({const char*_tmp39="Warning: use of __extension__ deleted\n";_tag_dyneither(_tmp39,sizeof(char),39);}),_tag_dyneither(_tmp38,sizeof(void*),0));});
return 1;case 8: _LLA8: _LLA9:
# 408 "buildlib.cyl"
({void*_tmp3A=0;Cyc_log(({const char*_tmp3B="Warning: use of mode HI deleted\n";_tag_dyneither(_tmp3B,sizeof(char),33);}),_tag_dyneither(_tmp3A,sizeof(void*),0));});
return 1;case 9: _LLAA: _LLAB:
# 411
({void*_tmp3C=0;Cyc_log(({const char*_tmp3D="Warning: use of mode SI deleted\n";_tag_dyneither(_tmp3D,sizeof(char),33);}),_tag_dyneither(_tmp3C,sizeof(void*),0));});
return 1;case 10: _LLAC: _LLAD:
# 414
({void*_tmp3E=0;Cyc_log(({const char*_tmp3F="Warning: use of mode QI deleted\n";_tag_dyneither(_tmp3F,sizeof(char),33);}),_tag_dyneither(_tmp3E,sizeof(void*),0));});
return 1;case 11: _LLAE: _LLAF:
# 417
({void*_tmp40=0;Cyc_log(({const char*_tmp41="Warning: use of mode DI deleted\n";_tag_dyneither(_tmp41,sizeof(char),33);}),_tag_dyneither(_tmp40,sizeof(void*),0));});
return 1;case 12: _LLB0: _LLB1:
# 420
({void*_tmp42=0;Cyc_log(({const char*_tmp43="Warning: use of mode DI deleted\n";_tag_dyneither(_tmp43,sizeof(char),33);}),_tag_dyneither(_tmp42,sizeof(void*),0));});
return 1;case 13: _LLB2: _LLB3:
# 423
({void*_tmp44=0;Cyc_log(({const char*_tmp45="Warning: use of mode word deleted\n";_tag_dyneither(_tmp45,sizeof(char),35);}),_tag_dyneither(_tmp44,sizeof(void*),0));});
return 1;case 14: _LLB4: _LLB5:
# 426
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15: _LLB6: _LLB7:
# 428
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16: _LLB8: _LLB9:
# 430
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17: _LLBA: _LLBB:
# 432
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18: _LLBC: _LLBD:
# 437 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19: _LLBE: _LLBF:
# 439
 return 1;case 20: _LLC0: _LLC1:
# 441
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp46=0;Cyc_log(({const char*_tmp47="Warning: replacing use of __asm__ with 0\n";_tag_dyneither(_tmp47,sizeof(char),42);}),_tag_dyneither(_tmp46,sizeof(void*),0));});
return 1;case 21: _LLC2: _LLC3:
# 447
 Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;default: _LLC4: _LLC5:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LL97:;}
# 451
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48[0]=({struct Cyc_Lexing_Error_exn_struct _tmp49;_tmp49.tag=Cyc_Lexing_Error;_tmp49.f1=({const char*_tmp4A="some action didn't return!";_tag_dyneither(_tmp4A,sizeof(char),27);});_tmp49;});_tmp48;}));}
# 453
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp4B=lexstate;switch(_tmp4B){case 0: _LLC7: _LLC8:
# 451 "buildlib.cyl"
 return 0;case 1: _LLC9: _LLCA:
# 453
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LLCB: _LLCC:
# 455
({void*_tmp4C=0;Cyc_log(({const char*_tmp4D="Warning: unclosed string\n";_tag_dyneither(_tmp4D,sizeof(char),26);}),_tag_dyneither(_tmp4C,sizeof(void*),0));});
({struct Cyc_String_pa_PrintArg_struct _tmp50;_tmp50.tag=0;_tmp50.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp4E[1]={& _tmp50};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp4F="%s";_tag_dyneither(_tmp4F,sizeof(char),3);}),_tag_dyneither(_tmp4E,sizeof(void*),1));});});return 1;case 3: _LLCD: _LLCE:
# 458
({struct Cyc_String_pa_PrintArg_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp51[1]={& _tmp53};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp52="%s";_tag_dyneither(_tmp52,sizeof(char),3);}),_tag_dyneither(_tmp51,sizeof(void*),1));});});return 1;case 4: _LLCF: _LLD0:
# 460
({struct Cyc_String_pa_PrintArg_struct _tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp54[1]={& _tmp56};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp55="%s";_tag_dyneither(_tmp55,sizeof(char),3);}),_tag_dyneither(_tmp54,sizeof(void*),1));});});return 1;case 5: _LLD1: _LLD2:
# 462
({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp57[1]={& _tmp59};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp58="%s";_tag_dyneither(_tmp58,sizeof(char),3);}),_tag_dyneither(_tmp57,sizeof(void*),1));});});return 1;case 6: _LLD3: _LLD4:
# 464
({struct Cyc_String_pa_PrintArg_struct _tmp5C;_tmp5C.tag=0;_tmp5C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp5A[1]={& _tmp5C};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp5B="%s";_tag_dyneither(_tmp5B,sizeof(char),3);}),_tag_dyneither(_tmp5A,sizeof(void*),1));});});return 1;case 7: _LLD5: _LLD6:
# 466
({struct Cyc_String_pa_PrintArg_struct _tmp5F;_tmp5F.tag=0;_tmp5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp5D[1]={& _tmp5F};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp5E="%s";_tag_dyneither(_tmp5E,sizeof(char),3);}),_tag_dyneither(_tmp5D,sizeof(void*),1));});});return 1;case 8: _LLD7: _LLD8:
# 468
({struct Cyc_String_pa_PrintArg_struct _tmp62;_tmp62.tag=0;_tmp62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));({void*_tmp60[1]={& _tmp62};Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),({const char*_tmp61="%s";_tag_dyneither(_tmp61,sizeof(char),3);}),_tag_dyneither(_tmp60,sizeof(void*),1));});});return 1;default: _LLD9: _LLDA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LLC6:;}
# 472
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp63=_cycalloc(sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Lexing_Error_exn_struct _tmp64;_tmp64.tag=Cyc_Lexing_Error;_tmp64.f1=({const char*_tmp65="some action didn't return!";_tag_dyneither(_tmp65,sizeof(char),27);});_tmp64;});_tmp63;}));}
# 474
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp66=lexstate;switch(_tmp66){case 0: _LLDC: _LLDD:
# 478 "buildlib.cyl"
 return 0;case 1: _LLDE: _LLDF:
# 480
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2: _LLE0: _LLE1:
# 484
 ++ Cyc_parens_to_match;
return 1;case 3: _LLE2: _LLE3:
# 487
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4: _LLE4: _LLE5:
# 490
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5: _LLE6: _LLE7:
# 493
 return 1;case 6: _LLE8: _LLE9:
# 495
 return 1;default: _LLEA: _LLEB:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LLDB:;}
# 499
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Lexing_Error_exn_struct _tmp68;_tmp68.tag=Cyc_Lexing_Error;_tmp68.f1=({const char*_tmp69="some action didn't return!";_tag_dyneither(_tmp69,sizeof(char),27);});_tmp68;});_tmp67;}));}
# 501
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp6A=lexstate;switch(_tmp6A){case 0: _LLED: _LLEE:
# 499 "buildlib.cyl"
({void*_tmp6B=0;Cyc_log(({const char*_tmp6C="Warning: unclosed string\n";_tag_dyneither(_tmp6C,sizeof(char),26);}),_tag_dyneither(_tmp6B,sizeof(void*),0));});return 0;case 1: _LLEF: _LLF0:
# 501
 return 0;case 2: _LLF1: _LLF2:
# 503
({void*_tmp6D=0;Cyc_log(({const char*_tmp6E="Warning: unclosed string\n";_tag_dyneither(_tmp6E,sizeof(char),26);}),_tag_dyneither(_tmp6D,sizeof(void*),0));});return 1;case 3: _LLF3: _LLF4:
# 505
 return 1;case 4: _LLF5: _LLF6:
# 507
 return 1;case 5: _LLF7: _LLF8:
# 509
 return 1;case 6: _LLF9: _LLFA:
# 511
 return 1;case 7: _LLFB: _LLFC:
# 513
 return 1;case 8: _LLFD: _LLFE:
# 515
 return 1;default: _LLFF: _LL100:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LLEC:;}
# 519
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Lexing_Error_exn_struct _tmp70;_tmp70.tag=Cyc_Lexing_Error;_tmp70.f1=({const char*_tmp71="some action didn't return!";_tag_dyneither(_tmp71,sizeof(char),27);});_tmp70;});_tmp6F;}));}
# 521
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp72=lexstate;switch(_tmp72){case 0: _LL102: _LL103:
# 519 "buildlib.cyl"
({void*_tmp73=0;Cyc_log(({const char*_tmp74="Warning: unclosed comment\n";_tag_dyneither(_tmp74,sizeof(char),27);}),_tag_dyneither(_tmp73,sizeof(void*),0));});return 0;case 1: _LL104: _LL105:
# 521
 return 0;case 2: _LL106: _LL107:
# 523
 return 1;default: _LL108: _LL109:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL101:;}
# 527
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=({struct Cyc_Lexing_Error_exn_struct _tmp76;_tmp76.tag=Cyc_Lexing_Error;_tmp76.f1=({const char*_tmp77="some action didn't return!";_tag_dyneither(_tmp77,sizeof(char),27);});_tmp76;});_tmp75;}));}
# 529
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple21*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp78=lexstate;switch(_tmp78){case 0: _LL10B: _LL10C:
# 531 "buildlib.cyl"
 Cyc_current_line=({const char*_tmp79="#define ";_tag_dyneither(_tmp79,sizeof(char),9);});
Cyc_suck_macroname(lexbuf);
return({struct _tuple21*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=Cyc_current_line;_tmp7A->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp7A;});case 1: _LL10D: _LL10E:
# 535
 return Cyc_suck_line(lexbuf);case 2: _LL10F: _LL110:
# 537
 return 0;default: _LL111: _LL112:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL10A:;}
# 541
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({struct Cyc_Lexing_Error_exn_struct _tmp7C;_tmp7C.tag=Cyc_Lexing_Error;_tmp7C.f1=({const char*_tmp7D="some action didn't return!";_tag_dyneither(_tmp7D,sizeof(char),27);});_tmp7C;});_tmp7B;}));}
# 543
struct _tuple21*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp7E=lexstate;if(_tmp7E == 0){_LL114: _LL115:
# 541 "buildlib.cyl"
 Cyc_current_source=({struct _dyneither_ptr*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmp7F;});
Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);}else{_LL116: _LL117:
# 545
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL113:;}
# 548
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({struct Cyc_Lexing_Error_exn_struct _tmp81;_tmp81.tag=Cyc_Lexing_Error;_tmp81.f1=({const char*_tmp82="some action didn't return!";_tag_dyneither(_tmp82,sizeof(char),27);});_tmp81;});_tmp80;}));}
# 550
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp83=lexstate;if(_tmp83 == 0){_LL119: _LL11A:
# 548 "buildlib.cyl"
 Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;}else{_LL11B: _LL11C:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL118:;}
# 552
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct Cyc_Lexing_Error_exn_struct _tmp85;_tmp85.tag=Cyc_Lexing_Error;_tmp85.f1=({const char*_tmp86="some action didn't return!";_tag_dyneither(_tmp86,sizeof(char),27);});_tmp85;});_tmp84;}));}
# 554
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple23*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp87=lexstate;switch(_tmp87){case 0: _LL11E: _LL11F:
# 555 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1: _LL120: _LL121:
# 557
 Cyc_current_headerfile=(struct _dyneither_ptr)
Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((
Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));
Cyc_current_symbols=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
while(Cyc_commands(lexbuf)){;}
Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);
return({struct _tuple23*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->f1=Cyc_current_headerfile;_tmp88->f2=Cyc_current_symbols;_tmp88->f3=Cyc_current_omit_symbols;_tmp88->f4=Cyc_current_hstubs;_tmp88->f5=Cyc_current_cstubs;_tmp88->f6=Cyc_current_cycstubs;_tmp88;});case 2: _LL122: _LL123:
# 577
 return Cyc_spec(lexbuf);case 3: _LL124: _LL125:
# 579
 return 0;case 4: _LL126: _LL127:
# 581
({struct Cyc_Int_pa_PrintArg_struct _tmp8B;_tmp8B.tag=1;_tmp8B.f1=(unsigned long)((int)
# 583
Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmp89[1]={& _tmp8B};Cyc_fprintf(Cyc_stderr,({const char*_tmp8A="Error in .cys file: expected header file name, found '%c' instead\n";_tag_dyneither(_tmp8A,sizeof(char),67);}),_tag_dyneither(_tmp89,sizeof(void*),1));});});
return 0;default: _LL128: _LL129:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL11D:;}
# 588
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Lexing_Error_exn_struct _tmp8D;_tmp8D.tag=Cyc_Lexing_Error;_tmp8D.f1=({const char*_tmp8E="some action didn't return!";_tag_dyneither(_tmp8E,sizeof(char),27);});_tmp8D;});_tmp8C;}));}
# 590
struct _tuple23*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp8F=lexstate;switch(_tmp8F){case 0: _LL12B: _LL12C:
# 588 "buildlib.cyl"
 return 0;case 1: _LL12D: _LL12E:
# 590
 return 0;case 2: _LL12F: _LL130:
# 592
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL131: _LL132:
# 597
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;case 4: _LL133: _LL134:
# 602
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp91->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp91;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->hd=x;_tmp90->tl=Cyc_current_hstubs;_tmp90;});
return 1;};case 5: _LL135: _LL136: {
# 610
struct _dyneither_ptr _tmp92=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp92,sizeof(char),(int)Cyc_strlen(({const char*_tmp93="hstub";_tag_dyneither(_tmp93,sizeof(char),6);})));
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp92,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp92,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp92;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp94=Cyc_substring((struct _dyneither_ptr)_tmp92,0,(unsigned long)((t.curr - _tmp92.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->f1=(struct _dyneither_ptr)_tmp94;_tmp96->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp96;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->hd=x;_tmp95->tl=Cyc_current_hstubs;_tmp95;});
return 1;};};};}case 6: _LL137: _LL138:
# 624
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp98->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp98;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=x;_tmp97->tl=Cyc_current_cstubs;_tmp97;});
return 1;};case 7: _LL139: _LL13A: {
# 632
struct _dyneither_ptr _tmp99=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp99,sizeof(char),(int)Cyc_strlen(({const char*_tmp9A="cstub";_tag_dyneither(_tmp9A,sizeof(char),6);})));
while(isspace((int)*((char*)_check_dyneither_subscript(_tmp99,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmp99,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp99;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp9B=Cyc_substring((struct _dyneither_ptr)_tmp99,0,(unsigned long)((t.curr - _tmp99.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->f1=(struct _dyneither_ptr)_tmp9B;_tmp9D->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9D;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->hd=x;_tmp9C->tl=Cyc_current_cstubs;_tmp9C;});
return 1;};};};}case 8: _LL13B: _LL13C:
# 646
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp9F->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9F;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->hd=x;_tmp9E->tl=Cyc_current_cycstubs;_tmp9E;});
return 1;};case 9: _LL13D: _LL13E: {
# 654
struct _dyneither_ptr _tmpA0=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmpA0,sizeof(char),(int)Cyc_strlen(({const char*_tmpA1="cycstub";_tag_dyneither(_tmpA1,sizeof(char),8);})));
while(isspace((int)*((char*)_check_dyneither_subscript(_tmpA0,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& _tmpA0,sizeof(char),1);}{
struct _dyneither_ptr t=_tmpA0;
while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmpA2=Cyc_substring((struct _dyneither_ptr)_tmpA0,0,(unsigned long)((t.curr - _tmpA0.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _tuple22*x=({struct _tuple22*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->f1=(struct _dyneither_ptr)_tmpA2;_tmpA4->f2=(struct _dyneither_ptr)
Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA4;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->hd=x;_tmpA3->tl=Cyc_current_cycstubs;_tmpA3;});
return 1;};};};}case 10: _LL13F: _LL140:
# 668
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*x=({struct _dyneither_ptr*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA6;});
Cyc_current_cpp=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->hd=x;_tmpA5->tl=Cyc_current_cpp;_tmpA5;});
return 1;};case 11: _LL141: _LL142:
# 675
 return 1;case 12: _LL143: _LL144:
# 677
 return 1;case 13: _LL145: _LL146:
# 679
({struct Cyc_Int_pa_PrintArg_struct _tmpA9;_tmpA9.tag=1;_tmpA9.f1=(unsigned long)((int)
# 681
Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmpA7[1]={& _tmpA9};Cyc_fprintf(Cyc_stderr,({const char*_tmpA8="Error in .cys file: expected command, found '%c' instead\n";_tag_dyneither(_tmpA8,sizeof(char),58);}),_tag_dyneither(_tmpA7,sizeof(void*),1));});});
return 0;default: _LL147: _LL148:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL12A:;}
# 686
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA[0]=({struct Cyc_Lexing_Error_exn_struct _tmpAB;_tmpAB.tag=Cyc_Lexing_Error;_tmpAB.f1=({const char*_tmpAC="some action didn't return!";_tag_dyneither(_tmpAC,sizeof(char),27);});_tmpAB;});_tmpAA;}));}
# 688
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpAD=lexstate;switch(_tmpAD){case 0: _LL14A: _LL14B:
# 691 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->hd=({struct _dyneither_ptr*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);_tmpAF;});_tmpAE->tl=Cyc_snarfed_symbols;_tmpAE;});
return 1;case 1: _LL14C: _LL14D:
# 694
 return 1;case 2: _LL14E: _LL14F:
# 696
 return 0;case 3: _LL150: _LL151:
# 698
({void*_tmpB0=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpB1="Error in .cys file: unexpected end-of-file\n";_tag_dyneither(_tmpB1,sizeof(char),44);}),_tag_dyneither(_tmpB0,sizeof(void*),0));});
# 700
return 0;case 4: _LL152: _LL153:
# 702
({struct Cyc_Int_pa_PrintArg_struct _tmpB4;_tmpB4.tag=1;_tmpB4.f1=(unsigned long)((int)
# 704
Cyc_Lexing_lexeme_char(lexbuf,0));({void*_tmpB2[1]={& _tmpB4};Cyc_fprintf(Cyc_stderr,({const char*_tmpB3="Error in .cys file: expected symbol, found '%c' instead\n";_tag_dyneither(_tmpB3,sizeof(char),57);}),_tag_dyneither(_tmpB2,sizeof(void*),1));});});
return 0;default: _LL154: _LL155:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL149:;}
# 709
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Lexing_Error_exn_struct _tmpB6;_tmpB6.tag=Cyc_Lexing_Error;_tmpB6.f1=({const char*_tmpB7="some action didn't return!";_tag_dyneither(_tmpB7,sizeof(char),27);});_tmpB6;});_tmpB5;}));}
# 711
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpB8=lexstate;switch(_tmpB8){case 0: _LL157: _LL158:
# 715 "buildlib.cyl"
({void*_tmpB9=0;Cyc_log(({const char*_tmpBA="Warning: unclosed brace\n";_tag_dyneither(_tmpBA,sizeof(char),25);}),_tag_dyneither(_tmpB9,sizeof(void*),0));});return 0;case 1: _LL159: _LL15A:
# 717
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2: _LL15B: _LL15C:
# 722
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3: _LL15D: _LL15E:
# 726
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LL15F: _LL160:
# 730
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),({const char*_tmpBB="/*";_tag_dyneither(_tmpBB,sizeof(char),3);}));
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LL161: _LL162:
# 734
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LL163: _LL164:
# 737
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LL165: _LL166:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL156:;}
# 742
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Lexing_Error_exn_struct _tmpBD;_tmpBD.tag=Cyc_Lexing_Error;_tmpBD.f1=({const char*_tmpBE="some action didn't return!";_tag_dyneither(_tmpBE,sizeof(char),27);});_tmpBD;});_tmpBC;}));}
# 744
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpBF=lexstate;switch(_tmpBF){case 0: _LL168: _LL169:
# 742 "buildlib.cyl"
({void*_tmpC0=0;Cyc_log(({const char*_tmpC1="Warning: unclosed string\n";_tag_dyneither(_tmpC1,sizeof(char),26);}),_tag_dyneither(_tmpC0,sizeof(void*),0));});return 0;case 1: _LL16A: _LL16B:
# 744
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LL16C: _LL16D:
# 746
({void*_tmpC2=0;Cyc_log(({const char*_tmpC3="Warning: unclosed string\n";_tag_dyneither(_tmpC3,sizeof(char),26);}),_tag_dyneither(_tmpC2,sizeof(void*),0));});
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3: _LL16E: _LL16F:
# 750
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 4: _LL170: _LL171:
# 753
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 5: _LL172: _LL173:
# 756
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LL174: _LL175:
# 759
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 7: _LL176: _LL177:
# 762
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 8: _LL178: _LL179:
# 765
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LL17A: _LL17B:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL167:;}
# 770
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Lexing_Error_exn_struct _tmpC5;_tmpC5.tag=Cyc_Lexing_Error;_tmpC5.f1=({const char*_tmpC6="some action didn't return!";_tag_dyneither(_tmpC6,sizeof(char),27);});_tmpC5;});_tmpC4;}));}
# 772
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC7=lexstate;switch(_tmpC7){case 0: _LL17D: _LL17E:
# 770 "buildlib.cyl"
({void*_tmpC8=0;Cyc_log(({const char*_tmpC9="Warning: unclosed comment\n";_tag_dyneither(_tmpC9,sizeof(char),27);}),_tag_dyneither(_tmpC8,sizeof(void*),0));});return 0;case 1: _LL17F: _LL180:
# 772
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),({const char*_tmpCA="*/";_tag_dyneither(_tmpCA,sizeof(char),3);}));return 0;case 2: _LL181: _LL182:
# 774
 Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LL183: _LL184:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL17C:;}
# 779
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB[0]=({struct Cyc_Lexing_Error_exn_struct _tmpCC;_tmpCC.tag=Cyc_Lexing_Error;_tmpCC.f1=({const char*_tmpCD="some action didn't return!";_tag_dyneither(_tmpCD,sizeof(char),27);});_tmpCC;});_tmpCB;}));}
# 781
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 784 "buildlib.cyl"
typedef struct Cyc_Hashtable_Table*Cyc_dep_t;
# 787
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmpCE=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_tmpCF=_tmpCE;struct Cyc_List_List*_tmpD0;void*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct _dyneither_ptr*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;struct _dyneither_ptr*_tmpD6;void*_tmpD7;void*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;int _tmpDA;struct Cyc_Absyn_Exp*_tmpDB;void**_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;void*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_List_List*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_List_List*_tmpF3;struct _dyneither_ptr*_tmpF4;switch(*((int*)_tmpCF)){case 1: _LL186: _tmpF4=(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCF)->f1)->f2;_LL187:
# 791
 Cyc_add_target(_tmpF4);
return;case 2: _LL188: _tmpF3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL189:
# 794
 for(0;_tmpF3 != 0;_tmpF3=_tmpF3->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpF3->hd,dep);}
# 797
return;case 22: _LL18A: _tmpF1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpF2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL18B:
# 799
 _tmpEF=_tmpF1;_tmpF0=_tmpF2;goto _LL18D;case 8: _LL18C: _tmpEF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpF0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL18D:
# 801
 _tmpED=_tmpEF;_tmpEE=_tmpF0;goto _LL18F;case 3: _LL18E: _tmpED=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpEE=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpCF)->f3;_LL18F:
# 803
 Cyc_scan_exp(_tmpED,dep);
Cyc_scan_exp(_tmpEE,dep);
return;case 19: _LL190: _tmpEC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL191:
# 807
 _tmpEB=_tmpEC;goto _LL193;case 17: _LL192: _tmpEB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL193:
# 809
 _tmpEA=_tmpEB;goto _LL195;case 14: _LL194: _tmpEA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL195:
# 811
 _tmpE9=_tmpEA;goto _LL197;case 4: _LL196: _tmpE9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL197:
# 813
 Cyc_scan_exp(_tmpE9,dep);
return;case 5: _LL198: _tmpE6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpE7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_tmpE8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCF)->f3;_LL199:
# 816
 Cyc_scan_exp(_tmpE6,dep);
Cyc_scan_exp(_tmpE7,dep);
Cyc_scan_exp(_tmpE8,dep);
return;case 6: _LL19A: _tmpE4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpE5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL19B:
 _tmpE2=_tmpE4;_tmpE3=_tmpE5;goto _LL19D;case 7: _LL19C: _tmpE2=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpE3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL19D:
# 822
 Cyc_scan_exp(_tmpE2,dep);
Cyc_scan_exp(_tmpE3,dep);
return;case 9: _LL19E: _tmpE0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpE1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL19F:
# 826
 Cyc_scan_exp(_tmpE0,dep);
for(0;_tmpE1 != 0;_tmpE1=_tmpE1->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE1->hd,dep);}
# 830
return;case 13: _LL1A0: _tmpDE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpDF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL1A1:
# 832
 Cyc_scan_type(_tmpDE,dep);
Cyc_scan_exp(_tmpDF,dep);
return;case 33: _LL1A2: _tmpDA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpCF)->f1).is_calloc;_tmpDB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpCF)->f1).rgn;_tmpDC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpCF)->f1).elt_type;_tmpDD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpCF)->f1).num_elts;_LL1A3:
# 836
 if(_tmpDB != 0)Cyc_scan_exp(_tmpDB,dep);
if(_tmpDC != 0)Cyc_scan_type(*_tmpDC,dep);
Cyc_scan_exp(_tmpDD,dep);
return;case 37: _LL1A4: _tmpD9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL1A5:
# 841
 Cyc_scan_exp(_tmpD9,dep);return;case 38: _LL1A6: _tmpD8=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL1A7:
 _tmpD7=_tmpD8;goto _LL1A9;case 16: _LL1A8: _tmpD7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_LL1A9:
# 844
 Cyc_scan_type(_tmpD7,dep);
return;case 20: _LL1AA: _tmpD5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpD6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL1AB:
# 847
 _tmpD3=_tmpD5;_tmpD4=_tmpD6;goto _LL1AD;case 21: _LL1AC: _tmpD3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpD4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL1AD:
# 849
 Cyc_scan_exp(_tmpD3,dep);
Cyc_add_target(_tmpD4);
return;case 18: _LL1AE: _tmpD1=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCF)->f1;_tmpD2=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL1AF:
# 853
 Cyc_scan_type(_tmpD1,dep);
# 855
{void*_tmpF5=(void*)((struct Cyc_List_List*)_check_null(_tmpD2))->hd;void*_tmpF6=_tmpF5;struct _dyneither_ptr*_tmpF7;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpF6)->tag == 0){_LL1D7: _tmpF7=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpF6)->f1;_LL1D8:
 Cyc_add_target(_tmpF7);goto _LL1D6;}else{_LL1D9: _LL1DA:
 goto _LL1D6;}_LL1D6:;}
# 859
return;case 0: _LL1B0: _LL1B1:
# 861
 return;case 35: _LL1B2: _tmpD0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpCF)->f2;_LL1B3:
# 863
 for(0;_tmpD0 != 0;_tmpD0=_tmpD0->tl){
struct _tuple24*_tmpF8=(struct _tuple24*)_tmpD0->hd;struct _tuple24*_tmpF9=_tmpF8;struct Cyc_Absyn_Exp*_tmpFA;_LL1DC: _tmpFA=_tmpF9->f2;_LL1DD:;
Cyc_scan_exp(_tmpFA,dep);}
# 867
return;case 39: _LL1B4: _LL1B5:
 return;case 34: _LL1B6: _LL1B7:
# 870
({void*_tmpFB=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpFC="Error: unexpected Swap_e\n";_tag_dyneither(_tmpFC,sizeof(char),26);}),_tag_dyneither(_tmpFB,sizeof(void*),0));});
exit(1);return;case 36: _LL1B8: _LL1B9:
# 873
({void*_tmpFD=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpFE="Error: unexpected Stmt_e\n";_tag_dyneither(_tmpFE,sizeof(char),26);}),_tag_dyneither(_tmpFD,sizeof(void*),0));});
exit(1);return;case 10: _LL1BA: _LL1BB:
# 876
({void*_tmpFF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp100="Error: unexpected Throw_e\n";_tag_dyneither(_tmp100,sizeof(char),27);}),_tag_dyneither(_tmpFF,sizeof(void*),0));});
exit(1);return;case 11: _LL1BC: _LL1BD:
# 879
({void*_tmp101=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp102="Error: unexpected NoInstantiate_e\n";_tag_dyneither(_tmp102,sizeof(char),35);}),_tag_dyneither(_tmp101,sizeof(void*),0));});
exit(1);return;case 12: _LL1BE: _LL1BF:
# 882
({void*_tmp103=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp104="Error: unexpected Instantiate_e\n";_tag_dyneither(_tmp104,sizeof(char),33);}),_tag_dyneither(_tmp103,sizeof(void*),0));});
exit(1);return;case 15: _LL1C0: _LL1C1:
# 885
({void*_tmp105=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp106="Error: unexpected New_e\n";_tag_dyneither(_tmp106,sizeof(char),25);}),_tag_dyneither(_tmp105,sizeof(void*),0));});
exit(1);return;case 23: _LL1C2: _LL1C3:
# 888
({void*_tmp107=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp108="Error: unexpected Tuple_e\n";_tag_dyneither(_tmp108,sizeof(char),27);}),_tag_dyneither(_tmp107,sizeof(void*),0));});
exit(1);return;case 24: _LL1C4: _LL1C5:
# 891
({void*_tmp109=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp10A="Error: unexpected CompoundLit_e\n";_tag_dyneither(_tmp10A,sizeof(char),33);}),_tag_dyneither(_tmp109,sizeof(void*),0));});
exit(1);return;case 25: _LL1C6: _LL1C7:
# 894
({void*_tmp10B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp10C="Error: unexpected Array_e\n";_tag_dyneither(_tmp10C,sizeof(char),27);}),_tag_dyneither(_tmp10B,sizeof(void*),0));});
exit(1);return;case 26: _LL1C8: _LL1C9:
# 897
({void*_tmp10D=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp10E="Error: unexpected Comprehension_e\n";_tag_dyneither(_tmp10E,sizeof(char),35);}),_tag_dyneither(_tmp10D,sizeof(void*),0));});
exit(1);return;case 27: _LL1CA: _LL1CB:
# 900
({void*_tmp10F=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp110="Error: unexpected ComprehensionNoinit_e\n";_tag_dyneither(_tmp110,sizeof(char),41);}),_tag_dyneither(_tmp10F,sizeof(void*),0));});
exit(1);return;case 28: _LL1CC: _LL1CD:
# 903
({void*_tmp111=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp112="Error: unexpected Aggregate_e\n";_tag_dyneither(_tmp112,sizeof(char),31);}),_tag_dyneither(_tmp111,sizeof(void*),0));});
exit(1);return;case 29: _LL1CE: _LL1CF:
# 906
({void*_tmp113=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp114="Error: unexpected AnonStruct_e\n";_tag_dyneither(_tmp114,sizeof(char),32);}),_tag_dyneither(_tmp113,sizeof(void*),0));});
exit(1);return;case 30: _LL1D0: _LL1D1:
# 909
({void*_tmp115=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp116="Error: unexpected Datatype_e\n";_tag_dyneither(_tmp116,sizeof(char),30);}),_tag_dyneither(_tmp115,sizeof(void*),0));});
exit(1);return;case 31: _LL1D2: _LL1D3:
# 912
({void*_tmp117=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp118="Error: unexpected Enum_e\n";_tag_dyneither(_tmp118,sizeof(char),26);}),_tag_dyneither(_tmp117,sizeof(void*),0));});
exit(1);return;default: _LL1D4: _LL1D5:
# 915
({void*_tmp119=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp11A="Error: unexpected AnonEnum_e\n";_tag_dyneither(_tmp11A,sizeof(char),30);}),_tag_dyneither(_tmp119,sizeof(void*),0));});
exit(1);return;}_LL185:;}
# 920
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 925
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp11B=t;struct Cyc_Absyn_Datatypedecl*_tmp11C;struct Cyc_Absyn_Enumdecl*_tmp11D;struct Cyc_Absyn_Aggrdecl*_tmp11E;struct _dyneither_ptr*_tmp11F;struct _dyneither_ptr*_tmp120;union Cyc_Absyn_AggrInfoU _tmp121;struct Cyc_List_List*_tmp122;struct Cyc_Absyn_FnInfo _tmp123;struct Cyc_Absyn_Exp*_tmp124;void*_tmp125;struct Cyc_Absyn_Exp*_tmp126;union Cyc_Absyn_Constraint*_tmp127;struct Cyc_Absyn_PtrInfo _tmp128;switch(*((int*)_tmp11B)){case 0: _LL1DF: _LL1E0:
 goto _LL1E2;case 6: _LL1E1: _LL1E2:
 goto _LL1E4;case 28: _LL1E3: _LL1E4:
 goto _LL1E6;case 7: _LL1E5: _LL1E6:
# 932
 return;case 5: _LL1E7: _tmp128=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11B)->f1;_LL1E8:
# 935
 Cyc_scan_type(_tmp128.elt_typ,dep);
return;case 8: _LL1E9: _tmp125=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11B)->f1).elt_type;_tmp126=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11B)->f1).num_elts;_tmp127=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11B)->f1).zero_term;_LL1EA:
# 938
 Cyc_scan_type(_tmp125,dep);
Cyc_scan_exp_opt(_tmp126,dep);
return;case 27: _LL1EB: _tmp124=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp11B)->f1;_LL1EC:
# 942
 Cyc_scan_exp(_tmp124,dep);
return;case 9: _LL1ED: _tmp123=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp11B)->f1;_LL1EE:
# 945
 Cyc_scan_type(_tmp123.ret_typ,dep);
{struct Cyc_List_List*_tmp129=_tmp123.args;for(0;_tmp129 != 0;_tmp129=_tmp129->tl){
struct _tuple8*_tmp12A=(struct _tuple8*)_tmp129->hd;struct _tuple8*_tmp12B=_tmp12A;void*_tmp12C;_LL21E: _tmp12C=_tmp12B->f3;_LL21F:;
Cyc_scan_type(_tmp12C,dep);}}
# 950
if(_tmp123.cyc_varargs != 0)
Cyc_scan_type((_tmp123.cyc_varargs)->type,dep);
return;case 12: _LL1EF: _tmp122=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp11B)->f2;_LL1F0:
# 954
 for(0;_tmp122 != 0;_tmp122=_tmp122->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp122->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp122->hd)->width,dep);}
# 958
return;case 14: _LL1F1: _LL1F2:
# 960
 return;case 11: _LL1F3: _tmp121=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp11B)->f1).aggr_info;_LL1F4: {
# 962
struct _tuple10 _tmp12D=Cyc_Absyn_aggr_kinded_name(_tmp121);struct _tuple10 _tmp12E=_tmp12D;struct _dyneither_ptr*_tmp12F;_LL221: _tmp12F=(_tmp12E.f2)->f2;_LL222:;
_tmp120=_tmp12F;goto _LL1F6;}case 13: _LL1F5: _tmp120=(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp11B)->f1)->f2;_LL1F6:
# 965
 _tmp11F=_tmp120;goto _LL1F8;case 17: _LL1F7: _tmp11F=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp11B)->f1)->f2;_LL1F8:
# 967
 Cyc_add_target(_tmp11F);
return;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11B)->f1)->r)){case 0: _LL1F9: _tmp11E=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11B)->f1)->r)->f1;_LL1FA:
# 971
 Cyc_scan_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130[0]=({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp131;_tmp131.tag=5;_tmp131.f1=_tmp11E;_tmp131;});_tmp130;}),0),dep);{
struct _tuple0*_tmp132=_tmp11E->name;struct _tuple0*_tmp133=_tmp132;struct _dyneither_ptr*_tmp134;_LL224: _tmp134=_tmp133->f2;_LL225:;
Cyc_add_target(_tmp134);
return;};case 1: _LL1FB: _tmp11D=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11B)->f1)->r)->f1;_LL1FC:
# 977
 Cyc_scan_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135[0]=({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp136;_tmp136.tag=7;_tmp136.f1=_tmp11D;_tmp136;});_tmp135;}),0),dep);{
struct _tuple0*_tmp137=_tmp11D->name;struct _tuple0*_tmp138=_tmp137;struct _dyneither_ptr*_tmp139;_LL227: _tmp139=_tmp138->f2;_LL228:;
Cyc_add_target(_tmp139);
return;};default: _LL1FD: _tmp11C=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11B)->f1)->r)->f1;_LL1FE:
# 983
({void*_tmp13A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp13B="Error: unexpected Datatype declaration\n";_tag_dyneither(_tmp13B,sizeof(char),40);}),_tag_dyneither(_tmp13A,sizeof(void*),0));});
exit(1);return;}case 1: _LL1FF: _LL200:
# 986
({void*_tmp13C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp13D="Error: unexpected Evar\n";_tag_dyneither(_tmp13D,sizeof(char),24);}),_tag_dyneither(_tmp13C,sizeof(void*),0));});
exit(1);return;case 2: _LL201: _LL202:
# 989
({void*_tmp13E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp13F="Error: unexpected VarType\n";_tag_dyneither(_tmp13F,sizeof(char),27);}),_tag_dyneither(_tmp13E,sizeof(void*),0));});
exit(1);return;case 3: _LL203: _LL204:
# 992
({void*_tmp140=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp141="Error: unexpected DatatypeType\n";_tag_dyneither(_tmp141,sizeof(char),32);}),_tag_dyneither(_tmp140,sizeof(void*),0));});
exit(1);return;case 4: _LL205: _LL206:
# 995
({void*_tmp142=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp143="Error: unexpected DatatypeFieldType\n";_tag_dyneither(_tmp143,sizeof(char),37);}),_tag_dyneither(_tmp142,sizeof(void*),0));});
exit(1);return;case 10: _LL207: _LL208:
# 998
({void*_tmp144=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp145="Error: unexpected TupleType\n";_tag_dyneither(_tmp145,sizeof(char),29);}),_tag_dyneither(_tmp144,sizeof(void*),0));});
exit(1);return;case 15: _LL209: _LL20A:
# 1001
({void*_tmp146=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp147="Error: unexpected RgnHandleType\n";_tag_dyneither(_tmp147,sizeof(char),33);}),_tag_dyneither(_tmp146,sizeof(void*),0));});
exit(1);return;case 16: _LL20B: _LL20C:
# 1004
({void*_tmp148=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp149="Error: unexpected DynRgnType\n";_tag_dyneither(_tmp149,sizeof(char),30);}),_tag_dyneither(_tmp148,sizeof(void*),0));});
exit(1);return;case 20: _LL20D: _LL20E:
# 1007
({void*_tmp14A=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp14B="Error: unexpected HeapRgn\n";_tag_dyneither(_tmp14B,sizeof(char),27);}),_tag_dyneither(_tmp14A,sizeof(void*),0));});
exit(1);return;case 21: _LL20F: _LL210:
# 1010
({void*_tmp14C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp14D="Error: unexpected UniqueRgn\n";_tag_dyneither(_tmp14D,sizeof(char),29);}),_tag_dyneither(_tmp14C,sizeof(void*),0));});
exit(1);return;case 22: _LL211: _LL212:
# 1013
({void*_tmp14E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp14F="Error: unexpected RefCntRgn\n";_tag_dyneither(_tmp14F,sizeof(char),29);}),_tag_dyneither(_tmp14E,sizeof(void*),0));});
exit(1);return;case 23: _LL213: _LL214:
# 1016
({void*_tmp150=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp151="Error: unexpected AccessEff\n";_tag_dyneither(_tmp151,sizeof(char),29);}),_tag_dyneither(_tmp150,sizeof(void*),0));});
exit(1);return;case 24: _LL215: _LL216:
# 1019
({void*_tmp152=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp153="Error: unexpected JoinEff\n";_tag_dyneither(_tmp153,sizeof(char),27);}),_tag_dyneither(_tmp152,sizeof(void*),0));});
exit(1);return;case 25: _LL217: _LL218:
# 1022
({void*_tmp154=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp155="Error: unexpected RgnsEff\n";_tag_dyneither(_tmp155,sizeof(char),27);}),_tag_dyneither(_tmp154,sizeof(void*),0));});
exit(1);return;case 19: _LL219: _LL21A:
# 1025
({void*_tmp156=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp157="Error: unexpected tag_t\n";_tag_dyneither(_tmp157,sizeof(char),25);}),_tag_dyneither(_tmp156,sizeof(void*),0));});
exit(1);return;default: _LL21B: _LL21C:
# 1028
({void*_tmp158=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp159="Error: unexpected valueof_t\n";_tag_dyneither(_tmp159,sizeof(char),29);}),_tag_dyneither(_tmp158,sizeof(void*),0));});
exit(1);return;}_LL1DE:;}
# 1033
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp15A=Cyc_current_targets;
struct _dyneither_ptr*_tmp15B=Cyc_current_source;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_tmp15C;});
{void*_tmp15D=d->r;void*_tmp15E=_tmp15D;struct Cyc_Absyn_Typedefdecl*_tmp15F;struct Cyc_Absyn_Enumdecl*_tmp160;struct Cyc_Absyn_Aggrdecl*_tmp161;struct Cyc_Absyn_Fndecl*_tmp162;struct Cyc_Absyn_Vardecl*_tmp163;switch(*((int*)_tmp15E)){case 0: _LL22A: _tmp163=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp15E)->f1;_LL22B: {
# 1039
struct _tuple0*_tmp164=_tmp163->name;struct _tuple0*_tmp165=_tmp164;struct _dyneither_ptr*_tmp166;_LL249: _tmp166=_tmp165->f2;_LL24A:;
Cyc_current_source=_tmp166;
Cyc_scan_type(_tmp163->type,dep);
Cyc_scan_exp_opt(_tmp163->initializer,dep);
goto _LL229;}case 1: _LL22C: _tmp162=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp15E)->f1;_LL22D: {
# 1045
struct _tuple0*_tmp167=_tmp162->name;struct _tuple0*_tmp168=_tmp167;struct _dyneither_ptr*_tmp169;_LL24C: _tmp169=_tmp168->f2;_LL24D:;
Cyc_current_source=_tmp169;
Cyc_scan_type(_tmp162->ret_type,dep);
{struct Cyc_List_List*_tmp16A=_tmp162->args;for(0;_tmp16A != 0;_tmp16A=_tmp16A->tl){
struct _tuple8*_tmp16B=(struct _tuple8*)_tmp16A->hd;struct _tuple8*_tmp16C=_tmp16B;void*_tmp16D;_LL24F: _tmp16D=_tmp16C->f3;_LL250:;
Cyc_scan_type(_tmp16D,dep);}}
# 1052
if(_tmp162->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp162->cyc_varargs))->type,dep);
if(_tmp162->is_inline)
({void*_tmp16E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp16F="Warning: ignoring inline function\n";_tag_dyneither(_tmp16F,sizeof(char),35);}),_tag_dyneither(_tmp16E,sizeof(void*),0));});
# 1057
goto _LL229;}case 5: _LL22E: _tmp161=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp15E)->f1;_LL22F: {
# 1059
struct _tuple0*_tmp170=_tmp161->name;struct _tuple0*_tmp171=_tmp170;struct _dyneither_ptr*_tmp172;_LL252: _tmp172=_tmp171->f2;_LL253:;
Cyc_current_source=_tmp172;
if((unsigned int)_tmp161->impl){
{struct Cyc_List_List*_tmp173=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp161->impl))->fields;for(0;_tmp173 != 0;_tmp173=_tmp173->tl){
struct Cyc_Absyn_Aggrfield*_tmp174=(struct Cyc_Absyn_Aggrfield*)_tmp173->hd;
Cyc_scan_type(_tmp174->type,dep);
Cyc_scan_exp_opt(_tmp174->width,dep);}}{
# 1069
struct Cyc_List_List*_tmp175=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp161->impl))->fields;for(0;_tmp175 != 0;_tmp175=_tmp175->tl){;}};}
# 1073
goto _LL229;}case 7: _LL230: _tmp160=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp15E)->f1;_LL231: {
# 1075
struct _tuple0*_tmp176=_tmp160->name;struct _tuple0*_tmp177=_tmp176;struct _dyneither_ptr*_tmp178;_LL255: _tmp178=_tmp177->f2;_LL256:;
Cyc_current_source=_tmp178;
if((unsigned int)_tmp160->fields){
{struct Cyc_List_List*_tmp179=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp160->fields))->v;for(0;_tmp179 != 0;_tmp179=_tmp179->tl){
struct Cyc_Absyn_Enumfield*_tmp17A=(struct Cyc_Absyn_Enumfield*)_tmp179->hd;
Cyc_scan_exp_opt(_tmp17A->tag,dep);}}{
# 1084
struct Cyc_List_List*_tmp17B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp160->fields))->v;for(0;_tmp17B != 0;_tmp17B=_tmp17B->tl){;}};}
# 1088
goto _LL229;}case 8: _LL232: _tmp15F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp15E)->f1;_LL233: {
# 1090
struct _tuple0*_tmp17C=_tmp15F->name;struct _tuple0*_tmp17D=_tmp17C;struct _dyneither_ptr*_tmp17E;_LL258: _tmp17E=_tmp17D->f2;_LL259:;
Cyc_current_source=_tmp17E;
if((unsigned int)_tmp15F->defn)
Cyc_scan_type((void*)_check_null(_tmp15F->defn),dep);
goto _LL229;}case 4: _LL234: _LL235:
# 1096
({void*_tmp17F=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp180="Error: unexpected region declaration";_tag_dyneither(_tmp180,sizeof(char),37);}),_tag_dyneither(_tmp17F,sizeof(void*),0));});
exit(1);case 13: _LL236: _LL237:
# 1102
({void*_tmp181=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp182="Error: unexpected __cyclone_port_on__";_tag_dyneither(_tmp182,sizeof(char),38);}),_tag_dyneither(_tmp181,sizeof(void*),0));});
exit(1);case 14: _LL238: _LL239:
# 1105
({void*_tmp183=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp184="Error: unexpected __cyclone_port_off__";_tag_dyneither(_tmp184,sizeof(char),39);}),_tag_dyneither(_tmp183,sizeof(void*),0));});
exit(1);case 2: _LL23A: _LL23B:
# 1108
({void*_tmp185=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp186="Error: unexpected let declaration\n";_tag_dyneither(_tmp186,sizeof(char),35);}),_tag_dyneither(_tmp185,sizeof(void*),0));});
exit(1);case 6: _LL23C: _LL23D:
# 1111
({void*_tmp187=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp188="Error: unexpected datatype declaration\n";_tag_dyneither(_tmp188,sizeof(char),40);}),_tag_dyneither(_tmp187,sizeof(void*),0));});
exit(1);case 3: _LL23E: _LL23F:
# 1114
({void*_tmp189=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18A="Error: unexpected let declaration\n";_tag_dyneither(_tmp18A,sizeof(char),35);}),_tag_dyneither(_tmp189,sizeof(void*),0));});
exit(1);case 9: _LL240: _LL241:
# 1117
({void*_tmp18B=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18C="Error: unexpected namespace declaration\n";_tag_dyneither(_tmp18C,sizeof(char),41);}),_tag_dyneither(_tmp18B,sizeof(void*),0));});
exit(1);case 10: _LL242: _LL243:
# 1120
({void*_tmp18D=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18E="Error: unexpected using declaration\n";_tag_dyneither(_tmp18E,sizeof(char),37);}),_tag_dyneither(_tmp18D,sizeof(void*),0));});
exit(1);case 11: _LL244: _LL245:
# 1123
({void*_tmp18F=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp190="Error: unexpected extern \"C\" declaration\n";_tag_dyneither(_tmp190,sizeof(char),42);}),_tag_dyneither(_tmp18F,sizeof(void*),0));});
exit(1);default: _LL246: _LL247:
# 1126
({void*_tmp191=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp192="Error: unexpected extern \"C include\" declaration\n";_tag_dyneither(_tmp192,sizeof(char),50);}),_tag_dyneither(_tmp191,sizeof(void*),0));});
exit(1);}_LL229:;}{
# 1134
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp193=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp194;_push_handler(& _tmp194);{int _tmp196=0;if(setjmp(_tmp194.handler))_tmp196=1;if(!_tmp196){
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp193);;_pop_handler();}else{void*_tmp195=(void*)_exn_thrown;void*_tmp197=_tmp195;void*_tmp198;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp197)->tag == Cyc_Core_Not_found){_LL25B: _LL25C:
# 1139
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL25A;}else{_LL25D: _tmp198=_tmp197;_LL25E:(int)_rethrow(_tmp198);}_LL25A:;}};}{
# 1141
struct Cyc_Set_Set*_tmp199=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp193,_tmp199);
# 1144
Cyc_current_targets=_tmp15A;
Cyc_current_source=_tmp15B;};};}
# 1148
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1152
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp19A;_push_handler(& _tmp19A);{int _tmp19C=0;if(setjmp(_tmp19A.handler))_tmp19C=1;if(!_tmp19C){{struct Cyc_Set_Set*_tmp19D=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return _tmp19D;};_pop_handler();}else{void*_tmp19B=(void*)_exn_thrown;void*_tmp19E=_tmp19B;void*_tmp19F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp19E)->tag == Cyc_Core_Not_found){_LL260: _LL261:
# 1155
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL262: _tmp19F=_tmp19E;_LL263:(int)_rethrow(_tmp19F);}_LL25F:;}};}
# 1159
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1170 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
struct Cyc_Set_Set*_tmp1A0=curr;
# 1176
struct _dyneither_ptr*_tmp1A1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1A0)> 0){
struct Cyc_Set_Set*_tmp1A2=emptyset;
struct Cyc_Iter_Iter _tmp1A3=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp1A0);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp1A3,& _tmp1A1)){
_tmp1A2=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp1A2,Cyc_find(t,_tmp1A1));}
_tmp1A0=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp1A2,curr);
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp1A0);}
# 1185
return curr;};}
# 1188
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0,Cyc_GATHER  = 1,Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering(){
return Cyc_mode == Cyc_GATHER  || Cyc_mode == Cyc_GATHERSCRIPT;}
# 1194
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1198
if(Cyc_script_file == 0){
({void*_tmp1A5=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1A6="Internal error: script file is NULL\n";_tag_dyneither(_tmp1A6,sizeof(char),37);}),_tag_dyneither(_tmp1A5,sizeof(void*),0));});
exit(1);}
# 1202
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1205
int Cyc_force_directory(struct _dyneither_ptr d){
if(Cyc_mode == Cyc_GATHERSCRIPT)
({struct Cyc_String_pa_PrintArg_struct _tmp1AA;_tmp1AA.tag=0;_tmp1AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({struct Cyc_String_pa_PrintArg_struct _tmp1A9;_tmp1A9.tag=0;_tmp1A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp1A7[2]={& _tmp1A9,& _tmp1AA};Cyc_prscript(({const char*_tmp1A8="if ! test -e %s; then mkdir %s; fi\n";_tag_dyneither(_tmp1A8,sizeof(char),36);}),_tag_dyneither(_tmp1A7,sizeof(void*),2));});});});else{
# 1212
int _tmp1AB=({unsigned short _tmp1AF[0];Cyc_open((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp1AF,sizeof(unsigned short),0));});
if(_tmp1AB == - 1){
if(mkdir((const char*)_untag_dyneither_ptr(d,sizeof(char),1),448)== - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp1AE;_tmp1AE.tag=0;_tmp1AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);({void*_tmp1AC[1]={& _tmp1AE};Cyc_fprintf(Cyc_stderr,({const char*_tmp1AD="Error: could not create directory %s\n";_tag_dyneither(_tmp1AD,sizeof(char),38);}),_tag_dyneither(_tmp1AC,sizeof(void*),1));});});
return 1;}}else{
# 1219
close(_tmp1AB);}}
# 1221
return 0;}
# 1224
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1228
struct _dyneither_ptr _tmp1B0=Cyc_strdup((struct _dyneither_ptr)file);
# 1230
struct Cyc_List_List*_tmp1B1=0;
while(1){
_tmp1B0=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp1B0);
if(Cyc_strlen((struct _dyneither_ptr)_tmp1B0)== 0)break;
_tmp1B1=({struct Cyc_List_List*_tmp1B2=_cycalloc(sizeof(*_tmp1B2));_tmp1B2->hd=({struct _dyneither_ptr*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3[0]=(struct _dyneither_ptr)_tmp1B0;_tmp1B3;});_tmp1B2->tl=_tmp1B1;_tmp1B2;});}
# 1237
for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp1B1->hd)))return 1;}
# 1240
return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1247
static int Cyc_is_other_special(char c){
char _tmp1B4=c;switch(_tmp1B4){case 92: _LL265: _LL266:
 goto _LL268;case 34: _LL267: _LL268:
 goto _LL26A;case 59: _LL269: _LL26A:
 goto _LL26C;case 38: _LL26B: _LL26C:
 goto _LL26E;case 40: _LL26D: _LL26E:
 goto _LL270;case 41: _LL26F: _LL270:
 goto _LL272;case 124: _LL271: _LL272:
 goto _LL274;case 94: _LL273: _LL274:
 goto _LL276;case 60: _LL275: _LL276:
 goto _LL278;case 62: _LL277: _LL278:
 goto _LL27A;case 10: _LL279: _LL27A:
# 1262
 goto _LL27C;case 9: _LL27B: _LL27C:
 return 1;default: _LL27D: _LL27E:
 return 0;}_LL264:;}
# 1268
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp1B5=Cyc_strlen((struct _dyneither_ptr)s);
# 1272
int _tmp1B6=0;
int _tmp1B7=0;
{int i=0;for(0;i < _tmp1B5;++ i){
char _tmp1B8=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp1B8 == '\'')++ _tmp1B6;else{
if(Cyc_is_other_special(_tmp1B8))++ _tmp1B7;}}}
# 1281
if(_tmp1B6 == 0  && _tmp1B7 == 0)
return s;
# 1285
if(_tmp1B6 == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmp1B9[3];_tmp1B9[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);_tmp1B9[1]=({struct _dyneither_ptr*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));_tmp1BB[0]=(struct _dyneither_ptr)s;_tmp1BB;});_tmp1B9[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1B9,sizeof(struct _dyneither_ptr*),3));}));{
# 1289
unsigned long _tmp1BD=(_tmp1B5 + _tmp1B6)+ _tmp1B7;
struct _dyneither_ptr s2=({unsigned int _tmp1C7=_tmp1BD + 1;char*_tmp1C8=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp1C7 + 1));struct _dyneither_ptr _tmp1CA=_tag_dyneither(_tmp1C8,sizeof(char),_tmp1C7 + 1);{unsigned int _tmp1C9=_tmp1C7;unsigned int i;for(i=0;i < _tmp1C9;i ++){_tmp1C8[i]=(char)'\000';}_tmp1C8[_tmp1C9]=(char)0;}_tmp1CA;});
int _tmp1BE=0;
int _tmp1BF=0;
for(0;_tmp1BE < _tmp1B5;++ _tmp1BE){
char _tmp1C0=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp1BE));
if(_tmp1C0 == '\''  || Cyc_is_other_special(_tmp1C0))
({struct _dyneither_ptr _tmp1C1=_dyneither_ptr_plus(s2,sizeof(char),_tmp1BF ++);char _tmp1C2=*((char*)_check_dyneither_subscript(_tmp1C1,sizeof(char),0));char _tmp1C3='\\';if(_get_dyneither_size(_tmp1C1,sizeof(char))== 1  && (_tmp1C2 == '\000'  && _tmp1C3 != '\000'))_throw_arraybounds();*((char*)_tmp1C1.curr)=_tmp1C3;});
({struct _dyneither_ptr _tmp1C4=_dyneither_ptr_plus(s2,sizeof(char),_tmp1BF ++);char _tmp1C5=*((char*)_check_dyneither_subscript(_tmp1C4,sizeof(char),0));char _tmp1C6=_tmp1C0;if(_get_dyneither_size(_tmp1C4,sizeof(char))== 1  && (_tmp1C5 == '\000'  && _tmp1C6 != '\000'))_throw_arraybounds();*((char*)_tmp1C4.curr)=_tmp1C6;});}
# 1299
return(struct _dyneither_ptr)s2;};}
# 1301
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=Cyc_sh_escape_string(*sp);_tmp1CB;});}struct _tuple25{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 1306
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){
# 1312
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1317
({struct Cyc_String_pa_PrintArg_struct _tmp1CE;_tmp1CE.tag=0;_tmp1CE.f1=(struct _dyneither_ptr)({const char*_tmp1CF=filename;_tag_dyneither(_tmp1CF,sizeof(char),_get_zero_arr_size_char((void*)_tmp1CF,1));});({void*_tmp1CC[1]={& _tmp1CE};Cyc_fprintf(Cyc_stderr,({const char*_tmp1CD="********************************* %s...\n";_tag_dyneither(_tmp1CD,sizeof(char),41);}),_tag_dyneither(_tmp1CC,sizeof(void*),1));});});
# 1320
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp1D2;_tmp1D2.tag=0;_tmp1D2.f1=(struct _dyneither_ptr)({const char*_tmp1D3=filename;_tag_dyneither(_tmp1D3,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D3,1));});({void*_tmp1D0[1]={& _tmp1D2};Cyc_log(({const char*_tmp1D1="\n%s:\n";_tag_dyneither(_tmp1D1,sizeof(char),6);}),_tag_dyneither(_tmp1D0,sizeof(void*),1));});});{
# 1332 "buildlib.cyl"
struct _dyneither_ptr _tmp1D4=Cyc_Filename_basename(({const char*_tmp318=filename;_tag_dyneither(_tmp318,sizeof(char),_get_zero_arr_size_char((void*)_tmp318,1));}));
struct _dyneither_ptr _tmp1D5=Cyc_Filename_dirname(({const char*_tmp317=filename;_tag_dyneither(_tmp317,sizeof(char),_get_zero_arr_size_char((void*)_tmp317,1));}));
struct _dyneither_ptr _tmp1D6=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp1D4);
const char*_tmp1D7=(const char*)_untag_dyneither_ptr(Cyc_strconcat((struct _dyneither_ptr)_tmp1D6,({const char*_tmp316=".iA";_tag_dyneither(_tmp316,sizeof(char),4);})),sizeof(char),1);
const char*_tmp1D8=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp1D5,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp312;_tmp312.tag=0;_tmp312.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6);({void*_tmp310[1]={& _tmp312};Cyc_aprintf(({const char*_tmp311="%s.iB";_tag_dyneither(_tmp311,sizeof(char),6);}),_tag_dyneither(_tmp310,sizeof(void*),1));});}): Cyc_Filename_concat((struct _dyneither_ptr)_tmp1D5,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp315;_tmp315.tag=0;_tmp315.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6);({void*_tmp313[1]={& _tmp315};Cyc_aprintf(({const char*_tmp314="%s.iB";_tag_dyneither(_tmp314,sizeof(char),6);}),_tag_dyneither(_tmp313,sizeof(void*),1));});})),sizeof(char),1);
# 1340
const char*_tmp1D9=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp1D5,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp30C;_tmp30C.tag=0;_tmp30C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6);({void*_tmp30A[1]={& _tmp30C};Cyc_aprintf(({const char*_tmp30B="%s.iC";_tag_dyneither(_tmp30B,sizeof(char),6);}),_tag_dyneither(_tmp30A,sizeof(void*),1));});}): Cyc_Filename_concat((struct _dyneither_ptr)_tmp1D5,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30F;_tmp30F.tag=0;_tmp30F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6);({void*_tmp30D[1]={& _tmp30F};Cyc_aprintf(({const char*_tmp30E="%s.iC";_tag_dyneither(_tmp30E,sizeof(char),6);}),_tag_dyneither(_tmp30D,sizeof(void*),1));});})),sizeof(char),1);
# 1344
const char*_tmp1DA=(const char*)_untag_dyneither_ptr(_get_dyneither_size(_tmp1D5,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp306;_tmp306.tag=0;_tmp306.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6);({void*_tmp304[1]={& _tmp306};Cyc_aprintf(({const char*_tmp305="%s.iD";_tag_dyneither(_tmp305,sizeof(char),6);}),_tag_dyneither(_tmp304,sizeof(void*),1));});}): Cyc_Filename_concat((struct _dyneither_ptr)_tmp1D5,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp309;_tmp309.tag=0;_tmp309.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D6);({void*_tmp307[1]={& _tmp309};Cyc_aprintf(({const char*_tmp308="%s.iD";_tag_dyneither(_tmp308,sizeof(char),6);}),_tag_dyneither(_tmp307,sizeof(void*),1));});})),sizeof(char),1);
# 1349
{struct _handler_cons _tmp1DB;_push_handler(& _tmp1DB);{int _tmp1DD=0;if(setjmp(_tmp1DB.handler))_tmp1DD=1;if(!_tmp1DD){
# 1352
if(Cyc_force_directory_prefixes(({const char*_tmp1DE=filename;_tag_dyneither(_tmp1DE,sizeof(char),_get_zero_arr_size_char((void*)_tmp1DE,1));}))){
int _tmp1DF=1;_npop_handler(0);return _tmp1DF;}
# 1357
if(Cyc_mode != Cyc_FINISH){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp1E2;_tmp1E2.tag=0;_tmp1E2.f1=(struct _dyneither_ptr)({const char*_tmp1E3=_tmp1D7;_tag_dyneither(_tmp1E3,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E3,1));});({void*_tmp1E0[1]={& _tmp1E2};Cyc_prscript(({const char*_tmp1E1="cat >%s <<XXX\n";_tag_dyneither(_tmp1E1,sizeof(char),15);}),_tag_dyneither(_tmp1E0,sizeof(void*),1));});});
{struct Cyc_List_List*_tmp1E4=Cyc_current_cpp;for(0;_tmp1E4 != 0;_tmp1E4=_tmp1E4->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0;_tmp1E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp1E4->hd));({void*_tmp1E5[1]={& _tmp1E7};Cyc_prscript(({const char*_tmp1E6="%s";_tag_dyneither(_tmp1E6,sizeof(char),3);}),_tag_dyneither(_tmp1E5,sizeof(void*),1));});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp1EA;_tmp1EA.tag=0;_tmp1EA.f1=(struct _dyneither_ptr)({const char*_tmp1EB=filename;_tag_dyneither(_tmp1EB,sizeof(char),_get_zero_arr_size_char((void*)_tmp1EB,1));});({void*_tmp1E8[1]={& _tmp1EA};Cyc_prscript(({const char*_tmp1E9="#include <%s>\n";_tag_dyneither(_tmp1E9,sizeof(char),15);}),_tag_dyneither(_tmp1E8,sizeof(void*),1));});});
({void*_tmp1EC=0;Cyc_prscript(({const char*_tmp1ED="XXX\n";_tag_dyneither(_tmp1ED,sizeof(char),5);}),_tag_dyneither(_tmp1EC,sizeof(void*),0));});
({struct Cyc_String_pa_PrintArg_struct _tmp1F1;_tmp1F1.tag=0;_tmp1F1.f1=(struct _dyneither_ptr)({const char*_tmp1F3=_tmp1D7;_tag_dyneither(_tmp1F3,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F3,1));});({struct Cyc_String_pa_PrintArg_struct _tmp1F0;_tmp1F0.tag=0;_tmp1F0.f1=(struct _dyneither_ptr)({const char*_tmp1F2=_tmp1D8;_tag_dyneither(_tmp1F2,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F2,1));});({void*_tmp1EE[2]={& _tmp1F0,& _tmp1F1};Cyc_prscript(({const char*_tmp1EF="$GCC -E -dM -o %s -x c %s && \\\n";_tag_dyneither(_tmp1EF,sizeof(char),32);}),_tag_dyneither(_tmp1EE,sizeof(void*),2));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp1F7;_tmp1F7.tag=0;_tmp1F7.f1=(struct _dyneither_ptr)({const char*_tmp1F9=_tmp1D7;_tag_dyneither(_tmp1F9,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F9,1));});({struct Cyc_String_pa_PrintArg_struct _tmp1F6;_tmp1F6.tag=0;_tmp1F6.f1=(struct _dyneither_ptr)({const char*_tmp1F8=_tmp1D9;_tag_dyneither(_tmp1F8,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F8,1));});({void*_tmp1F4[2]={& _tmp1F6,& _tmp1F7};Cyc_prscript(({const char*_tmp1F5="$GCC -E     -o %s -x c %s;\n";_tag_dyneither(_tmp1F5,sizeof(char),28);}),_tag_dyneither(_tmp1F4,sizeof(void*),2));});});});
({struct Cyc_String_pa_PrintArg_struct _tmp1FC;_tmp1FC.tag=0;_tmp1FC.f1=(struct _dyneither_ptr)({const char*_tmp1FD=_tmp1D7;_tag_dyneither(_tmp1FD,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FD,1));});({void*_tmp1FA[1]={& _tmp1FC};Cyc_prscript(({const char*_tmp1FB="rm %s\n";_tag_dyneither(_tmp1FB,sizeof(char),7);}),_tag_dyneither(_tmp1FA,sizeof(void*),1));});});}else{
# 1370
maybe=Cyc_fopen(_tmp1D7,"w");
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp200;_tmp200.tag=0;_tmp200.f1=(struct _dyneither_ptr)({const char*_tmp201=_tmp1D7;_tag_dyneither(_tmp201,sizeof(char),_get_zero_arr_size_char((void*)_tmp201,1));});({void*_tmp1FE[1]={& _tmp200};Cyc_fprintf(Cyc_stderr,({const char*_tmp1FF="Error: could not create file %s\n";_tag_dyneither(_tmp1FF,sizeof(char),33);}),_tag_dyneither(_tmp1FE,sizeof(void*),1));});});{
int _tmp202=1;_npop_handler(0);return _tmp202;};}
# 1375
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp205;_tmp205.tag=0;_tmp205.f1=(struct _dyneither_ptr)({const char*_tmp206=_tmp1D7;_tag_dyneither(_tmp206,sizeof(char),_get_zero_arr_size_char((void*)_tmp206,1));});({void*_tmp203[1]={& _tmp205};Cyc_fprintf(Cyc_stderr,({const char*_tmp204="Creating %s\n";_tag_dyneither(_tmp204,sizeof(char),13);}),_tag_dyneither(_tmp203,sizeof(void*),1));});});
out_file=maybe;
{struct Cyc_List_List*_tmp207=Cyc_current_cpp;for(0;_tmp207 != 0;_tmp207=_tmp207->tl){
Cyc_fputs((const char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp207->hd),sizeof(char),1),out_file);}}
# 1381
({struct Cyc_String_pa_PrintArg_struct _tmp20A;_tmp20A.tag=0;_tmp20A.f1=(struct _dyneither_ptr)({const char*_tmp20B=filename;_tag_dyneither(_tmp20B,sizeof(char),_get_zero_arr_size_char((void*)_tmp20B,1));});({void*_tmp208[1]={& _tmp20A};Cyc_fprintf(out_file,({const char*_tmp209="#include <%s>\n";_tag_dyneither(_tmp209,sizeof(char),15);}),_tag_dyneither(_tmp208,sizeof(void*),1));});});
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp20C=Cstring_to_string(Ccomp);
struct _dyneither_ptr _tmp20D=
Cyc_str_sepstr(({struct Cyc_List_List*_tmp226=_cycalloc(sizeof(*_tmp226));_tmp226->hd=({struct _dyneither_ptr*_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227[0]=(struct _dyneither_ptr)({const char*_tmp228="";_tag_dyneither(_tmp228,sizeof(char),1);});_tmp227;});_tmp226->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp226;}),({const char*_tmp229=" ";_tag_dyneither(_tmp229,sizeof(char),2);}));
# 1388
char*cmd=(char*)_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp223;_tmp223.tag=0;_tmp223.f1=(struct _dyneither_ptr)({const char*_tmp225=_tmp1D7;_tag_dyneither(_tmp225,sizeof(char),_get_zero_arr_size_char((void*)_tmp225,1));});({struct Cyc_String_pa_PrintArg_struct _tmp222;_tmp222.tag=0;_tmp222.f1=(struct _dyneither_ptr)({const char*_tmp224=_tmp1D8;_tag_dyneither(_tmp224,sizeof(char),_get_zero_arr_size_char((void*)_tmp224,1));});({struct Cyc_String_pa_PrintArg_struct _tmp221;_tmp221.tag=0;_tmp221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20D);({struct Cyc_String_pa_PrintArg_struct _tmp220;_tmp220.tag=0;_tmp220.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20C);({void*_tmp21E[4]={& _tmp220,& _tmp221,& _tmp222,& _tmp223};Cyc_aprintf(({const char*_tmp21F="%s %s -E -dM -o %s -x c %s";_tag_dyneither(_tmp21F,sizeof(char),27);}),_tag_dyneither(_tmp21E,sizeof(void*),4));});});});});}),sizeof(char),1);
# 1390
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp210;_tmp210.tag=0;_tmp210.f1=(struct _dyneither_ptr)({char*_tmp211=cmd;_tag_dyneither(_tmp211,sizeof(char),_get_zero_arr_size_char((void*)_tmp211,1));});({void*_tmp20E[1]={& _tmp210};Cyc_fprintf(Cyc_stderr,({const char*_tmp20F="%s\n";_tag_dyneither(_tmp20F,sizeof(char),4);}),_tag_dyneither(_tmp20E,sizeof(void*),1));});});
if(!system((const char*)cmd)){
# 1395
cmd=(char*)_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp217;_tmp217.tag=0;_tmp217.f1=(struct _dyneither_ptr)({const char*_tmp219=_tmp1D7;_tag_dyneither(_tmp219,sizeof(char),_get_zero_arr_size_char((void*)_tmp219,1));});({struct Cyc_String_pa_PrintArg_struct _tmp216;_tmp216.tag=0;_tmp216.f1=(struct _dyneither_ptr)({const char*_tmp218=_tmp1D9;_tag_dyneither(_tmp218,sizeof(char),_get_zero_arr_size_char((void*)_tmp218,1));});({struct Cyc_String_pa_PrintArg_struct _tmp215;_tmp215.tag=0;_tmp215.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20D);({struct Cyc_String_pa_PrintArg_struct _tmp214;_tmp214.tag=0;_tmp214.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20C);({void*_tmp212[4]={& _tmp214,& _tmp215,& _tmp216,& _tmp217};Cyc_aprintf(({const char*_tmp213="%s %s -E -o %s -x c %s";_tag_dyneither(_tmp213,sizeof(char),23);}),_tag_dyneither(_tmp212,sizeof(void*),4));});});});});}),sizeof(char),1);
# 1397
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp21C;_tmp21C.tag=0;_tmp21C.f1=(struct _dyneither_ptr)({char*_tmp21D=cmd;_tag_dyneither(_tmp21D,sizeof(char),_get_zero_arr_size_char((void*)_tmp21D,1));});({void*_tmp21A[1]={& _tmp21C};Cyc_fprintf(Cyc_stderr,({const char*_tmp21B="%s\n";_tag_dyneither(_tmp21B,sizeof(char),4);}),_tag_dyneither(_tmp21A,sizeof(void*),1));});});
system((const char*)cmd);}};}}
# 1404
if(Cyc_gathering()){int _tmp22A=0;_npop_handler(0);return _tmp22A;}{
# 1407
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp1D8,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp22B=_cycalloc(sizeof(*_tmp22B));_tmp22B[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp22C;_tmp22C.tag=Cyc_NO_SUPPORT;_tmp22C.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp22F;_tmp22F.tag=0;_tmp22F.f1=(struct _dyneither_ptr)({const char*_tmp230=_tmp1D8;_tag_dyneither(_tmp230,sizeof(char),_get_zero_arr_size_char((void*)_tmp230,1));});({void*_tmp22D[1]={& _tmp22F};Cyc_aprintf(({const char*_tmp22E="can't open macrosfile %s";_tag_dyneither(_tmp22E,sizeof(char),25);}),_tag_dyneither(_tmp22D,sizeof(void*),1));});});_tmp22C;});_tmp22B;}));
# 1412
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp231=Cyc_Lexing_from_file(in_file);
struct _tuple20*entry;
while((entry=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp231))!= 0){
struct _tuple20*_tmp232=(struct _tuple20*)_check_null(entry);struct _tuple20*_tmp233=_tmp232;struct _dyneither_ptr*_tmp234;struct Cyc_Set_Set*_tmp235;_LL280: _tmp234=_tmp233->f1;_tmp235=_tmp233->f2;_LL281:;
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp234,_tmp235);}
# 1421
Cyc_fclose(in_file);
# 1424
maybe=Cyc_fopen(_tmp1D9,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp237;_tmp237.tag=Cyc_NO_SUPPORT;_tmp237.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp23A;_tmp23A.tag=0;_tmp23A.f1=(struct _dyneither_ptr)({const char*_tmp23B=_tmp1D9;_tag_dyneither(_tmp23B,sizeof(char),_get_zero_arr_size_char((void*)_tmp23B,1));});({void*_tmp238[1]={& _tmp23A};Cyc_aprintf(({const char*_tmp239="can't open declsfile %s";_tag_dyneither(_tmp239,sizeof(char),24);}),_tag_dyneither(_tmp238,sizeof(void*),1));});});_tmp237;});_tmp236;}));
# 1428
in_file=maybe;
_tmp231=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(_tmp1DA,"w");
if(!((unsigned int)Cyc_slurp_out)){int _tmp23C=1;_npop_handler(0);return _tmp23C;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp231)){;}
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if(Cyc_mode != Cyc_FINISH)
;
# 1438
maybe=Cyc_fopen(_tmp1DA,"r");
if(!((unsigned int)maybe)){int _tmp23D=1;_npop_handler(0);return _tmp23D;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp23E=_tmp1DA;_tag_dyneither(_tmp23E,sizeof(char),_get_zero_arr_size_char((void*)_tmp23E,1));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp23F=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1448
{struct Cyc_List_List*_tmp240=_tmp23F;for(0;_tmp240 != 0;_tmp240=_tmp240->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp240->hd,t);}}{
# 1452
struct Cyc_Set_Set*_tmp241=Cyc_reachable(start_symbols,t);
# 1455
struct Cyc_List_List*_tmp242=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp243=_tmp23F;for(0;_tmp243 != 0;_tmp243=_tmp243->tl){
struct Cyc_Absyn_Decl*_tmp244=(struct Cyc_Absyn_Decl*)_tmp243->hd;
struct _dyneither_ptr*name;
{void*_tmp245=_tmp244->r;void*_tmp246=_tmp245;struct Cyc_Absyn_Typedefdecl*_tmp247;struct Cyc_Absyn_Enumdecl*_tmp248;struct Cyc_Absyn_Aggrdecl*_tmp249;struct Cyc_Absyn_Fndecl*_tmp24A;struct Cyc_Absyn_Vardecl*_tmp24B;switch(*((int*)_tmp246)){case 0: _LL283: _tmp24B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp246)->f1;_LL284: {
# 1464
struct _tuple0*_tmp24C=_tmp24B->name;struct _tuple0*_tmp24D=_tmp24C;struct _dyneither_ptr*_tmp24E;_LL2A2: _tmp24E=_tmp24D->f2;_LL2A3:;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp24E);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp24E))name=0;else{
name=_tmp24E;}
goto _LL282;}case 1: _LL285: _tmp24A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp246)->f1;_LL286: {
# 1470
struct _tuple0*_tmp24F=_tmp24A->name;struct _tuple0*_tmp250=_tmp24F;struct _dyneither_ptr*_tmp251;_LL2A5: _tmp251=_tmp250->f2;_LL2A6:;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp251);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp251))name=0;else{
name=_tmp251;}
goto _LL282;}case 5: _LL287: _tmp249=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp246)->f1;_LL288: {
# 1476
struct _tuple0*_tmp252=_tmp249->name;struct _tuple0*_tmp253=_tmp252;struct _dyneither_ptr*_tmp254;_LL2A8: _tmp254=_tmp253->f2;_LL2A9:;
name=_tmp254;
goto _LL282;}case 7: _LL289: _tmp248=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp246)->f1;_LL28A: {
# 1480
struct _tuple0*_tmp255=_tmp248->name;struct _tuple0*_tmp256=_tmp255;struct _dyneither_ptr*_tmp257;_LL2AB: _tmp257=_tmp256->f2;_LL2AC:;
name=_tmp257;
# 1484
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp241,name))
_tmp242=({struct Cyc_List_List*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258->hd=_tmp244;_tmp258->tl=_tmp242;_tmp258;});else{
# 1487
if((unsigned int)_tmp248->fields){
struct Cyc_List_List*_tmp259=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp248->fields))->v;for(0;_tmp259 != 0;_tmp259=_tmp259->tl){
struct Cyc_Absyn_Enumfield*_tmp25A=(struct Cyc_Absyn_Enumfield*)_tmp259->hd;
struct _tuple0*_tmp25B=_tmp25A->name;struct _tuple0*_tmp25C=_tmp25B;struct _dyneither_ptr*_tmp25D;_LL2AE: _tmp25D=_tmp25C->f2;_LL2AF:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp241,_tmp25D)){
_tmp242=({struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->hd=_tmp244;_tmp25E->tl=_tmp242;_tmp25E;});
break;}}}}
# 1497
name=0;
goto _LL282;}case 8: _LL28B: _tmp247=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp246)->f1;_LL28C: {
# 1500
struct _tuple0*_tmp25F=_tmp247->name;struct _tuple0*_tmp260=_tmp25F;struct _dyneither_ptr*_tmp261;_LL2B1: _tmp261=_tmp260->f2;_LL2B2:;
name=_tmp261;
goto _LL282;}case 13: _LL28D: _LL28E:
 goto _LL290;case 14: _LL28F: _LL290:
 goto _LL292;case 2: _LL291: _LL292:
 goto _LL294;case 6: _LL293: _LL294:
 goto _LL296;case 3: _LL295: _LL296:
 goto _LL298;case 9: _LL297: _LL298:
 goto _LL29A;case 10: _LL299: _LL29A:
 goto _LL29C;case 11: _LL29B: _LL29C:
 goto _LL29E;case 12: _LL29D: _LL29E:
 goto _LL2A0;default: _LL29F: _LL2A0:
# 1514
 name=0;
goto _LL282;}_LL282:;}
# 1517
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp241,name))
_tmp242=({struct Cyc_List_List*_tmp262=_cycalloc(sizeof(*_tmp262));_tmp262->hd=_tmp244;_tmp262->tl=_tmp242;_tmp262;});}}
# 1522
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){int _tmp263=1;_npop_handler(0);return _tmp263;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _dyneither_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2DD;_tmp2DD.tag=0;_tmp2DD.f1=(struct _dyneither_ptr)({const char*_tmp2DE=filename;_tag_dyneither(_tmp2DE,sizeof(char),_get_zero_arr_size_char((void*)_tmp2DE,1));});({void*_tmp2DB[1]={& _tmp2DD};Cyc_aprintf(({const char*_tmp2DC="_%s_";_tag_dyneither(_tmp2DC,sizeof(char),5);}),_tag_dyneither(_tmp2DB,sizeof(void*),1));});});
{int _tmp264=0;for(0;_tmp264 < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp264){
if(((char*)ifdefmacro.curr)[_tmp264]== '.'  || ((char*)ifdefmacro.curr)[_tmp264]== '/')
({struct _dyneither_ptr _tmp265=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp264);char _tmp266=*((char*)_check_dyneither_subscript(_tmp265,sizeof(char),0));char _tmp267='_';if(_get_dyneither_size(_tmp265,sizeof(char))== 1  && (_tmp266 == '\000'  && _tmp267 != '\000'))_throw_arraybounds();*((char*)_tmp265.curr)=_tmp267;});else{
if(((char*)ifdefmacro.curr)[_tmp264]!= '_'  && ((char*)ifdefmacro.curr)[_tmp264]!= '/')
({struct _dyneither_ptr _tmp268=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp264);char _tmp269=*((char*)_check_dyneither_subscript(_tmp268,sizeof(char),0));char _tmp26A=(char)toupper((int)((char*)ifdefmacro.curr)[_tmp264]);if(_get_dyneither_size(_tmp268,sizeof(char))== 1  && (_tmp269 == '\000'  && _tmp26A != '\000'))_throw_arraybounds();*((char*)_tmp268.curr)=_tmp26A;});}}}
# 1534
({struct Cyc_String_pa_PrintArg_struct _tmp26E;_tmp26E.tag=0;_tmp26E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({struct Cyc_String_pa_PrintArg_struct _tmp26D;_tmp26D.tag=0;_tmp26D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp26B[2]={& _tmp26D,& _tmp26E};Cyc_fprintf(out_file,({const char*_tmp26C="#ifndef %s\n#define %s\n";_tag_dyneither(_tmp26C,sizeof(char),23);}),_tag_dyneither(_tmp26B,sizeof(void*),2));});});});{
# 1541
struct Cyc_List_List*_tmp26F=0;
struct Cyc_List_List*_tmp270=0;
{struct Cyc_List_List*_tmp271=_tmp242;for(0;_tmp271 != 0;_tmp271=_tmp271->tl){
struct Cyc_Absyn_Decl*_tmp272=(struct Cyc_Absyn_Decl*)_tmp271->hd;
int _tmp273=0;
struct _dyneither_ptr*name;
{void*_tmp274=_tmp272->r;void*_tmp275=_tmp274;struct Cyc_Absyn_Typedefdecl*_tmp276;struct Cyc_Absyn_Enumdecl*_tmp277;struct Cyc_Absyn_Aggrdecl*_tmp278;struct Cyc_Absyn_Fndecl*_tmp279;struct Cyc_Absyn_Vardecl*_tmp27A;switch(*((int*)_tmp275)){case 0: _LL2B4: _tmp27A=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp275)->f1;_LL2B5: {
# 1549
struct _tuple0*_tmp27B=_tmp27A->name;struct _tuple0*_tmp27C=_tmp27B;struct _dyneither_ptr*_tmp27D;_LL2D3: _tmp27D=_tmp27C->f2;_LL2D4:;
name=_tmp27D;
goto _LL2B3;}case 1: _LL2B6: _tmp279=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp275)->f1;_LL2B7: {
# 1553
struct _tuple0*_tmp27E=_tmp279->name;struct _tuple0*_tmp27F=_tmp27E;struct _dyneither_ptr*_tmp280;_LL2D6: _tmp280=_tmp27F->f2;_LL2D7:;
name=_tmp280;
goto _LL2B3;}case 5: _LL2B8: _tmp278=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp275)->f1;_LL2B9: {
# 1557
struct _tuple0*_tmp281=_tmp278->name;struct _tuple0*_tmp282=_tmp281;struct _dyneither_ptr*_tmp283;_LL2D9: _tmp283=_tmp282->f2;_LL2DA:;
name=_tmp283;
goto _LL2B3;}case 7: _LL2BA: _tmp277=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp275)->f1;_LL2BB: {
# 1561
struct _tuple0*_tmp284=_tmp277->name;struct _tuple0*_tmp285=_tmp284;struct _dyneither_ptr*_tmp286;_LL2DC: _tmp286=_tmp285->f2;_LL2DD:;
name=_tmp286;
goto _LL2B3;}case 8: _LL2BC: _tmp276=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp275)->f1;_LL2BD: {
# 1565
struct _tuple0*_tmp287=_tmp276->name;struct _tuple0*_tmp288=_tmp287;struct _dyneither_ptr*_tmp289;_LL2DF: _tmp289=_tmp288->f2;_LL2E0:;
name=_tmp289;
goto _LL2B3;}case 4: _LL2BE: _LL2BF:
 goto _LL2C1;case 13: _LL2C0: _LL2C1:
# 1570
 goto _LL2C3;case 14: _LL2C2: _LL2C3:
 goto _LL2C5;case 2: _LL2C4: _LL2C5:
 goto _LL2C7;case 6: _LL2C6: _LL2C7:
 goto _LL2C9;case 3: _LL2C8: _LL2C9:
 goto _LL2CB;case 9: _LL2CA: _LL2CB:
 goto _LL2CD;case 10: _LL2CC: _LL2CD:
 goto _LL2CF;case 11: _LL2CE: _LL2CF:
 goto _LL2D1;default: _LL2D0: _LL2D1:
# 1579
 name=0;
goto _LL2B3;}_LL2B3:;}
# 1582
if(!((unsigned int)name) && !_tmp273)continue;
_tmp26F=({struct Cyc_List_List*_tmp28A=_cycalloc(sizeof(*_tmp28A));_tmp28A->hd=_tmp272;_tmp28A->tl=_tmp26F;_tmp28A;});
_tmp270=({struct Cyc_List_List*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B->hd=name;_tmp28B->tl=_tmp270;_tmp28B;});}}
# 1588
{struct _handler_cons _tmp28C;_push_handler(& _tmp28C);{int _tmp28E=0;if(setjmp(_tmp28C.handler))_tmp28E=1;if(!_tmp28E){
{struct _RegionHandle _tmp28F=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp28F;_push_region(tc_rgn);
{struct Cyc_Tcenv_Tenv*_tmp290=Cyc_Tcenv_tc_init(tc_rgn);
Cyc_Tc_tc(tc_rgn,_tmp290,1,_tmp26F);}
# 1590
;_pop_region(tc_rgn);}
# 1589
;_pop_handler();}else{void*_tmp28D=(void*)_exn_thrown;void*_tmp291=_tmp28D;_LL2E2: _LL2E3:
# 1595
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp293;_tmp293.tag=Cyc_NO_SUPPORT;_tmp293.f1=({const char*_tmp294="can't typecheck acquired declarations";_tag_dyneither(_tmp294,sizeof(char),38);});_tmp293;});_tmp292;}));
goto _LL2E1;_LL2E1:;}};}
# 1600
{struct _tuple25 _tmp295=({struct _tuple25 _tmp2A9;_tmp2A9.f1=_tmp26F;_tmp2A9.f2=_tmp270;_tmp2A9;});struct _tuple25 _tmp296=_tmp295;struct Cyc_List_List*_tmp297;struct Cyc_List_List*_tmp298;_LL2E7: _tmp297=_tmp296.f1;_tmp298=_tmp296.f2;_LL2E8:;for(0;
_tmp297 != 0  && _tmp298 != 0;(_tmp297=_tmp297->tl,_tmp298=_tmp298->tl)){
struct Cyc_Absyn_Decl*_tmp299=(struct Cyc_Absyn_Decl*)_tmp297->hd;
struct _dyneither_ptr*_tmp29A=(struct _dyneither_ptr*)_tmp298->hd;
int _tmp29B=0;
if(!((unsigned int)_tmp29A))
_tmp29B=1;
# 1609
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned int)_tmp29A){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0;_tmp29E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp29A);({void*_tmp29C[1]={& _tmp29E};Cyc_aprintf(({const char*_tmp29D="_%s_def_";_tag_dyneither(_tmp29D,sizeof(char),9);}),_tag_dyneither(_tmp29C,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp29F[1]={& _tmp2A1};Cyc_fprintf(out_file,({const char*_tmp2A0="#ifndef %s\n";_tag_dyneither(_tmp2A0,sizeof(char),12);}),_tag_dyneither(_tmp29F,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);({void*_tmp2A2[1]={& _tmp2A4};Cyc_fprintf(out_file,({const char*_tmp2A3="#define %s\n";_tag_dyneither(_tmp2A3,sizeof(char),12);}),_tag_dyneither(_tmp2A2,sizeof(void*),1));});});
# 1615
Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp2A5[1];_tmp2A5[0]=_tmp299;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A5,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);
({void*_tmp2A6=0;Cyc_fprintf(out_file,({const char*_tmp2A7="#endif\n";_tag_dyneither(_tmp2A7,sizeof(char),8);}),_tag_dyneither(_tmp2A6,sizeof(void*),0));});}else{
# 1620
Cyc_Absynpp_decllist2file(({struct Cyc_Absyn_Decl*_tmp2A8[1];_tmp2A8[0]=_tmp299;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A8,sizeof(struct Cyc_Absyn_Decl*),1));}),out_file);}}}
# 1625
maybe=Cyc_fopen(_tmp1D8,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA[0]=({struct Cyc_NO_SUPPORT_exn_struct _tmp2AB;_tmp2AB.tag=Cyc_NO_SUPPORT;_tmp2AB.f1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(struct _dyneither_ptr)({const char*_tmp2AF=_tmp1D8;_tag_dyneither(_tmp2AF,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AF,1));});({void*_tmp2AC[1]={& _tmp2AE};Cyc_aprintf(({const char*_tmp2AD="can't open macrosfile %s";_tag_dyneither(_tmp2AD,sizeof(char),25);}),_tag_dyneither(_tmp2AC,sizeof(void*),1));});});_tmp2AB;});_tmp2AA;}));
# 1628
in_file=maybe;
_tmp231=Cyc_Lexing_from_file(in_file);{
struct _tuple21*entry2;
while((entry2=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp231))!= 0){
struct _tuple21*_tmp2B0=(struct _tuple21*)_check_null(entry2);struct _tuple21*_tmp2B1=_tmp2B0;struct _dyneither_ptr _tmp2B2;struct _dyneither_ptr*_tmp2B3;_LL2EA: _tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B1->f2;_LL2EB:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp241,_tmp2B3)){
({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B3);({void*_tmp2B4[1]={& _tmp2B6};Cyc_fprintf(out_file,({const char*_tmp2B5="#ifndef %s\n";_tag_dyneither(_tmp2B5,sizeof(char),12);}),_tag_dyneither(_tmp2B4,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=0;_tmp2B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2B2);({void*_tmp2B7[1]={& _tmp2B9};Cyc_fprintf(out_file,({const char*_tmp2B8="%s\n";_tag_dyneither(_tmp2B8,sizeof(char),4);}),_tag_dyneither(_tmp2B7,sizeof(void*),1));});});
({void*_tmp2BA=0;Cyc_fprintf(out_file,({const char*_tmp2BB="#endif\n";_tag_dyneither(_tmp2BB,sizeof(char),8);}),_tag_dyneither(_tmp2BA,sizeof(void*),0));});}}
# 1639
Cyc_fclose(in_file);
if(Cyc_mode != Cyc_FINISH);
# 1642
if(hstubs != 0){
struct Cyc_List_List*_tmp2BC=hstubs;for(0;_tmp2BC != 0;_tmp2BC=_tmp2BC->tl){
struct _tuple22*_tmp2BD=(struct _tuple22*)_tmp2BC->hd;struct _tuple22*_tmp2BE=_tmp2BD;struct _dyneither_ptr _tmp2BF;struct _dyneither_ptr _tmp2C0;_LL2ED: _tmp2BF=_tmp2BE->f1;_tmp2C0=_tmp2BE->f2;_LL2EE:;
if((char*)_tmp2C0.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp2BF.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1[0]=_tmp2BF;_tmp2C1;}))))
# 1648
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2C0,sizeof(char),1),out_file);else{
# 1650
({struct Cyc_String_pa_PrintArg_struct _tmp2C4;_tmp2C4.tag=0;_tmp2C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2BF);({void*_tmp2C2[1]={& _tmp2C4};Cyc_log(({const char*_tmp2C3="%s is not supported on this platform\n";_tag_dyneither(_tmp2C3,sizeof(char),38);}),_tag_dyneither(_tmp2C2,sizeof(void*),1));});});}}}
# 1653
({void*_tmp2C5=0;Cyc_fprintf(out_file,({const char*_tmp2C6="#endif\n";_tag_dyneither(_tmp2C6,sizeof(char),8);}),_tag_dyneither(_tmp2C5,sizeof(void*),0));});
if(Cyc_do_setjmp){int _tmp2C7=0;_npop_handler(0);return _tmp2C7;}else{
Cyc_fclose(out_file);}
# 1658
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2C8=cstubs;for(0;_tmp2C8 != 0;_tmp2C8=_tmp2C8->tl){
struct _tuple22*_tmp2C9=(struct _tuple22*)_tmp2C8->hd;struct _tuple22*_tmp2CA=_tmp2C9;struct _dyneither_ptr _tmp2CB;struct _dyneither_ptr _tmp2CC;_LL2F0: _tmp2CB=_tmp2CA->f1;_tmp2CC=_tmp2CA->f2;_LL2F1:;
if((char*)_tmp2CC.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp2CB.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));_tmp2CD[0]=_tmp2CB;_tmp2CD;}))))
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2CC,sizeof(char),1),out_file);}};}
# 1669
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1671
({struct Cyc_String_pa_PrintArg_struct _tmp2D0;_tmp2D0.tag=0;_tmp2D0.f1=(struct _dyneither_ptr)({const char*_tmp2D1=filename;_tag_dyneither(_tmp2D1,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D1,1));});({void*_tmp2CE[1]={& _tmp2D0};Cyc_fprintf(out_file,({const char*_tmp2CF="#include <%s>\n\n";_tag_dyneither(_tmp2CF,sizeof(char),16);}),_tag_dyneither(_tmp2CE,sizeof(void*),1));});});
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp2D2=cycstubs;for(0;_tmp2D2 != 0;_tmp2D2=_tmp2D2->tl){
struct _tuple22*_tmp2D3=(struct _tuple22*)_tmp2D2->hd;struct _tuple22*_tmp2D4=_tmp2D3;struct _dyneither_ptr _tmp2D5;struct _dyneither_ptr _tmp2D6;_LL2F3: _tmp2D5=_tmp2D4->f1;_tmp2D6=_tmp2D4->f2;_LL2F4:;
if((char*)_tmp2D6.curr != (char*)(_tag_dyneither(0,0,0)).curr  && (
(char*)_tmp2D5.curr == (char*)(_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,({struct _dyneither_ptr*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7[0]=_tmp2D5;_tmp2D7;}))))
Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2D6,sizeof(char),1),out_file);}}
# 1680
({void*_tmp2D8=0;Cyc_fprintf(out_file,({const char*_tmp2D9="\n";_tag_dyneither(_tmp2D9,sizeof(char),2);}),_tag_dyneither(_tmp2D8,sizeof(void*),0));});}{
# 1683
int _tmp2DA=0;_npop_handler(0);return _tmp2DA;};};};};};};};};
# 1352
;_pop_handler();}else{void*_tmp1DC=(void*)_exn_thrown;void*_tmp2DF=_tmp1DC;struct _dyneither_ptr _tmp2E0;struct _dyneither_ptr _tmp2E1;struct _dyneither_ptr _tmp2E2;struct _dyneither_ptr _tmp2E3;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp2DF)->tag == Cyc_Core_Impossible){_LL2F6: _tmp2E3=((struct Cyc_Core_Impossible_exn_struct*)_tmp2DF)->f1;_LL2F7:
# 1687
({struct Cyc_String_pa_PrintArg_struct _tmp2E6;_tmp2E6.tag=0;_tmp2E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E3);({void*_tmp2E4[1]={& _tmp2E6};Cyc_fprintf(Cyc_stderr,({const char*_tmp2E5="Got Core::Impossible(%s)\n";_tag_dyneither(_tmp2E5,sizeof(char),26);}),_tag_dyneither(_tmp2E4,sizeof(void*),1));});});goto _LL2F5;}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2DF)->tag == Cyc_Dict_Absent){_LL2F8: _LL2F9:
# 1689
({void*_tmp2E7=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2E8="Got Dict::Absent\n";_tag_dyneither(_tmp2E8,sizeof(char),18);}),_tag_dyneither(_tmp2E7,sizeof(void*),0));});goto _LL2F5;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp2DF)->tag == Cyc_Core_Failure){_LL2FA: _tmp2E2=((struct Cyc_Core_Failure_exn_struct*)_tmp2DF)->f1;_LL2FB:
# 1691
({struct Cyc_String_pa_PrintArg_struct _tmp2EB;_tmp2EB.tag=0;_tmp2EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E2);({void*_tmp2E9[1]={& _tmp2EB};Cyc_fprintf(Cyc_stderr,({const char*_tmp2EA="Got Core::Failure(%s)\n";_tag_dyneither(_tmp2EA,sizeof(char),23);}),_tag_dyneither(_tmp2E9,sizeof(void*),1));});});goto _LL2F5;}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2DF)->tag == Cyc_Core_Invalid_argument){_LL2FC: _tmp2E1=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2DF)->f1;_LL2FD:
# 1693
({struct Cyc_String_pa_PrintArg_struct _tmp2EE;_tmp2EE.tag=0;_tmp2EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E1);({void*_tmp2EC[1]={& _tmp2EE};Cyc_fprintf(Cyc_stderr,({const char*_tmp2ED="Got Invalid_argument(%s)\n";_tag_dyneither(_tmp2ED,sizeof(char),26);}),_tag_dyneither(_tmp2EC,sizeof(void*),1));});});goto _LL2F5;}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp2DF)->tag == Cyc_Core_Not_found){_LL2FE: _LL2FF:
# 1695
({void*_tmp2EF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F0="Got Not_found\n";_tag_dyneither(_tmp2F0,sizeof(char),15);}),_tag_dyneither(_tmp2EF,sizeof(void*),0));});goto _LL2F5;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2DF)->tag == Cyc_NO_SUPPORT){_LL300: _tmp2E0=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2DF)->f1;_LL301:
# 1697
({struct Cyc_String_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=0;_tmp2F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2E0);({void*_tmp2F1[1]={& _tmp2F3};Cyc_fprintf(Cyc_stderr,({const char*_tmp2F2="No support because %s\n";_tag_dyneither(_tmp2F2,sizeof(char),23);}),_tag_dyneither(_tmp2F1,sizeof(void*),1));});});goto _LL2F5;}else{_LL302: _LL303:
# 1699
({void*_tmp2F4=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2F5="Got unknown exception\n";_tag_dyneither(_tmp2F5,sizeof(char),23);}),_tag_dyneither(_tmp2F4,sizeof(void*),0));});goto _LL2F5;}}}}}}_LL2F5:;}};}
# 1704
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp2F8;_tmp2F8.tag=0;_tmp2F8.f1=(struct _dyneither_ptr)({const char*_tmp2F9=filename;_tag_dyneither(_tmp2F9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F9,1));});({void*_tmp2F6[1]={& _tmp2F8};Cyc_fprintf(Cyc_stderr,({const char*_tmp2F7="Error: could not create file %s\n";_tag_dyneither(_tmp2F7,sizeof(char),33);}),_tag_dyneither(_tmp2F6,sizeof(void*),1));});});
return 1;}
# 1709
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp2FC;_tmp2FC.tag=0;_tmp2FC.f1=(struct _dyneither_ptr)({const char*_tmp2FD=filename;_tag_dyneither(_tmp2FD,sizeof(char),_get_zero_arr_size_char((void*)_tmp2FD,1));});({void*_tmp2FA[1]={& _tmp2FC};Cyc_fprintf(out_file,({const char*_tmp2FB="#error -- %s is not supported on this platform\n";_tag_dyneither(_tmp2FB,sizeof(char),48);}),_tag_dyneither(_tmp2FA,sizeof(void*),1));});});
# 1713
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp300;_tmp300.tag=0;_tmp300.f1=(struct _dyneither_ptr)({const char*_tmp301=filename;_tag_dyneither(_tmp301,sizeof(char),_get_zero_arr_size_char((void*)_tmp301,1));});({void*_tmp2FE[1]={& _tmp300};Cyc_fprintf(Cyc_stderr,({const char*_tmp2FF="Warning: %s will not be supported on this platform\n";_tag_dyneither(_tmp2FF,sizeof(char),52);}),_tag_dyneither(_tmp2FE,sizeof(void*),1));});});
# 1716
({void*_tmp302=0;Cyc_log(({const char*_tmp303="Not supported on this platform\n";_tag_dyneither(_tmp303,sizeof(char),32);}),_tag_dyneither(_tmp302,sizeof(void*),0));});
# 1723
return 0;};}
# 1727
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp31B;_tmp31B.tag=0;_tmp31B.f1=(struct _dyneither_ptr)({const char*_tmp31C=file;_tag_dyneither(_tmp31C,sizeof(char),_get_zero_arr_size_char((void*)_tmp31C,1));});({void*_tmp319[1]={& _tmp31B};Cyc_fprintf(Cyc_stderr,({const char*_tmp31A="Processing %s\n";_tag_dyneither(_tmp31A,sizeof(char),15);}),_tag_dyneither(_tmp319,sizeof(void*),1));});});{
struct Cyc___cycFILE*_tmp31D=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp31D)){
({struct Cyc_String_pa_PrintArg_struct _tmp320;_tmp320.tag=0;_tmp320.f1=(struct _dyneither_ptr)({const char*_tmp321=file;_tag_dyneither(_tmp321,sizeof(char),_get_zero_arr_size_char((void*)_tmp321,1));});({void*_tmp31E[1]={& _tmp320};Cyc_fprintf(Cyc_stderr,({const char*_tmp31F="Error: could not open %s\n";_tag_dyneither(_tmp31F,sizeof(char),26);}),_tag_dyneither(_tmp31E,sizeof(void*),1));});});
return 1;}{
# 1735
struct Cyc___cycFILE*_tmp322=_tmp31D;
# 1739
struct _dyneither_ptr buf=({char*_tmp33F=({unsigned int _tmp33C=(unsigned int)1024;char*_tmp33D=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp33C + 1));{unsigned int _tmp33E=_tmp33C;unsigned int i;for(i=0;i < _tmp33E;i ++){_tmp33D[i]=(char)'\000';}_tmp33D[_tmp33E]=(char)0;}_tmp33D;});_tag_dyneither(_tmp33F,sizeof(char),_get_zero_arr_size_char((void*)_tmp33F,(unsigned int)1024 + 1));});
struct _dyneither_ptr _tmp323=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(Cyc_mode != Cyc_GATHERSCRIPT){
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp326;_tmp326.tag=0;_tmp326.f1=(struct _dyneither_ptr)({const char*_tmp327=dir;_tag_dyneither(_tmp327,sizeof(char),_get_zero_arr_size_char((void*)_tmp327,1));});({void*_tmp324[1]={& _tmp326};Cyc_fprintf(Cyc_stderr,({const char*_tmp325="Error: can't change directory to %s\n";_tag_dyneither(_tmp325,sizeof(char),37);}),_tag_dyneither(_tmp324,sizeof(void*),1));});});
return 1;}}
# 1747
if(Cyc_mode == Cyc_GATHER){
# 1749
struct _dyneither_ptr _tmp328=Cstring_to_string(Ccomp);
struct _dyneither_ptr _tmp329=({struct Cyc_String_pa_PrintArg_struct _tmp32F;_tmp32F.tag=0;_tmp32F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp328);({void*_tmp32D[1]={& _tmp32F};Cyc_aprintf(({const char*_tmp32E="echo | %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp32E,sizeof(char),36);}),_tag_dyneither(_tmp32D,sizeof(void*),1));});});
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp32C;_tmp32C.tag=0;_tmp32C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp329);({void*_tmp32A[1]={& _tmp32C};Cyc_fprintf(Cyc_stderr,({const char*_tmp32B="%s\n";_tag_dyneither(_tmp32B,sizeof(char),4);}),_tag_dyneither(_tmp32A,sizeof(void*),1));});});
system((const char*)_untag_dyneither_ptr(_tmp329,sizeof(char),1));}{
# 1756
struct Cyc_Lexing_lexbuf*_tmp330=Cyc_Lexing_from_file(_tmp322);
struct _tuple23*entry;
while((entry=((struct _tuple23*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp330))!= 0){
struct _tuple23*_tmp331=(struct _tuple23*)_check_null(entry);struct _tuple23*_tmp332=_tmp331;struct _dyneither_ptr _tmp333;struct Cyc_List_List*_tmp334;struct Cyc_List_List*_tmp335;struct Cyc_List_List*_tmp336;struct Cyc_List_List*_tmp337;struct Cyc_List_List*_tmp338;_LL307: _tmp333=_tmp332->f1;_tmp334=_tmp332->f2;_tmp335=_tmp332->f3;_tmp336=_tmp332->f4;_tmp337=_tmp332->f5;_tmp338=_tmp332->f6;_LL308:;
# 1761
if(Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp333,sizeof(char),1),_tmp334,_tmp335,_tmp336,_tmp337,_tmp338))
# 1763
return 1;}
# 1765
Cyc_fclose(_tmp322);
# 1767
if(Cyc_mode != Cyc_GATHERSCRIPT){
if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp323,sizeof(char),1)))){
({struct Cyc_String_pa_PrintArg_struct _tmp33B;_tmp33B.tag=0;_tmp33B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp323);({void*_tmp339[1]={& _tmp33B};Cyc_fprintf(Cyc_stderr,({const char*_tmp33A="Error: could not change directory to %s\n";_tag_dyneither(_tmp33A,sizeof(char),41);}),_tag_dyneither(_tmp339,sizeof(void*),1));});});
return 1;}}
# 1773
return 0;};};};}
# 1777
int Cyc_process_setjmp(const char*dir){
# 1780
struct _dyneither_ptr buf=({char*_tmp351=({unsigned int _tmp34E=(unsigned int)1024;char*_tmp34F=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp34E + 1));{unsigned int _tmp350=_tmp34E;unsigned int i;for(i=0;i < _tmp350;i ++){_tmp34F[i]=(char)'\000';}_tmp34F[_tmp350]=(char)0;}_tmp34F;});_tag_dyneither(_tmp351,sizeof(char),_get_zero_arr_size_char((void*)_tmp351,(unsigned int)1024 + 1));});
struct _dyneither_ptr _tmp340=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp343;_tmp343.tag=0;_tmp343.f1=(struct _dyneither_ptr)({const char*_tmp344=dir;_tag_dyneither(_tmp344,sizeof(char),_get_zero_arr_size_char((void*)_tmp344,1));});({void*_tmp341[1]={& _tmp343};Cyc_fprintf(Cyc_stderr,({const char*_tmp342="Error: can't change directory to %s\n";_tag_dyneither(_tmp342,sizeof(char),37);}),_tag_dyneither(_tmp341,sizeof(void*),1));});});
return 1;}
# 1786
if(Cyc_process_file("setjmp.h",({struct _dyneither_ptr*_tmp345[1];_tmp345[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",sizeof(char),8);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp345,sizeof(struct _dyneither_ptr*),1));}),0,({struct _tuple22*_tmp347[1];_tmp347[0]=({struct _tuple22*_tmp348=_cycalloc(sizeof(*_tmp348));_tmp348->f1=({const char*_tmp34A="setjmp";_tag_dyneither(_tmp34A,sizeof(char),7);});_tmp348->f2=({const char*_tmp349="extern int setjmp(jmp_buf);\n";_tag_dyneither(_tmp349,sizeof(char),29);});_tmp348;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp347,sizeof(struct _tuple22*),1));}),0,0))
# 1789
return 1;
if(chdir((const char*)((char*)_untag_dyneither_ptr(_tmp340,sizeof(char),1)))){
({struct Cyc_String_pa_PrintArg_struct _tmp34D;_tmp34D.tag=0;_tmp34D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp340);({void*_tmp34B[1]={& _tmp34D};Cyc_fprintf(Cyc_stderr,({const char*_tmp34C="Error: could not change directory to %s\n";_tag_dyneither(_tmp34C,sizeof(char),41);}),_tag_dyneither(_tmp34B,sizeof(void*),1));});});
return 1;}
# 1794
return 0;}static char _tmp352[13]="BUILDLIB.OUT";
# 1798
static struct _dyneither_ptr Cyc_output_dir={_tmp352,_tmp352,_tmp352 + 13};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 1802
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->hd=(const char*)_untag_dyneither_ptr(s,sizeof(char),1);_tmp353->tl=Cyc_spec_files;_tmp353;});}
# 1806
static int Cyc_no_other(struct _dyneither_ptr s){return 0;}
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 1810
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 1813
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 1816
static void Cyc_add_cpparg(struct _dyneither_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->hd=({struct _dyneither_ptr*_tmp355=_cycalloc(sizeof(*_tmp355));_tmp355[0]=s;_tmp355;});_tmp354->tl=Cyc_cppargs;_tmp354;});}
# 1819
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp358;_tmp358.tag=0;_tmp358.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp356[1]={& _tmp358};Cyc_fprintf(Cyc_stderr,({const char*_tmp357="Unsupported option %s\n";_tag_dyneither(_tmp357,sizeof(char),23);}),_tag_dyneither(_tmp356,sizeof(void*),1));});});
Cyc_badparse=1;}
# 1824
static void Cyc_set_machine(struct _dyneither_ptr s){
Cyc_add_cpparg(({const char*_tmp359="-b";_tag_dyneither(_tmp359,sizeof(char),3);}));
Cyc_add_cpparg(s);}
# 1832
void GC_blacklist_warn_clear();struct _tuple26{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
GC_blacklist_warn_clear();{
# 1836
struct Cyc_List_List*options=({struct _tuple26*_tmp37F[8];_tmp37F[7]=({struct _tuple26*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA->f1=({const char*_tmp3AF="-";_tag_dyneither(_tmp3AF,sizeof(char),2);});_tmp3AA->f2=1;_tmp3AA->f3=({const char*_tmp3AE="";_tag_dyneither(_tmp3AE,sizeof(char),1);});_tmp3AA->f4=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC[0]=({struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmp3AD;_tmp3AD.tag=1;_tmp3AD.f1=Cyc_add_cpparg;_tmp3AD;});_tmp3AC;});_tmp3AA->f5=({const char*_tmp3AB="";_tag_dyneither(_tmp3AB,sizeof(char),1);});_tmp3AA;});_tmp37F[6]=({struct _tuple26*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4->f1=({const char*_tmp3A9="-v";_tag_dyneither(_tmp3A9,sizeof(char),3);});_tmp3A4->f2=0;_tmp3A4->f3=({const char*_tmp3A8="";_tag_dyneither(_tmp3A8,sizeof(char),1);});_tmp3A4->f4=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6[0]=({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp3A7;_tmp3A7.tag=3;_tmp3A7.f1=& Cyc_verbose;_tmp3A7;});_tmp3A6;});_tmp3A4->f5=({const char*_tmp3A5="Verbose operation";_tag_dyneither(_tmp3A5,sizeof(char),18);});_tmp3A4;});_tmp37F[5]=({struct _tuple26*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->f1=({const char*_tmp3A3="-b";_tag_dyneither(_tmp3A3,sizeof(char),3);});_tmp39E->f2=0;_tmp39E->f3=({const char*_tmp3A2=" <machine>";_tag_dyneither(_tmp3A2,sizeof(char),11);});_tmp39E->f4=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0[0]=({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp3A1;_tmp3A1.tag=5;_tmp3A1.f1=Cyc_set_machine;_tmp3A1;});_tmp3A0;});_tmp39E->f5=({const char*_tmp39F="Set the target machine for compilation to <machine>";_tag_dyneither(_tmp39F,sizeof(char),52);});_tmp39E;});_tmp37F[4]=({struct _tuple26*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->f1=({const char*_tmp39D="-setjmp";_tag_dyneither(_tmp39D,sizeof(char),8);});_tmp398->f2=0;_tmp398->f3=({const char*_tmp39C="";_tag_dyneither(_tmp39C,sizeof(char),1);});_tmp398->f4=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A[0]=({struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp39B;_tmp39B.tag=3;_tmp39B.f1=& Cyc_do_setjmp;_tmp39B;});_tmp39A;});_tmp398->f5=({const char*_tmp399="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_dyneither(_tmp399,sizeof(char),186);});_tmp398;});_tmp37F[3]=({struct _tuple26*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392->f1=({const char*_tmp397="-finish";_tag_dyneither(_tmp397,sizeof(char),8);});_tmp392->f2=0;_tmp392->f3=({const char*_tmp396="";_tag_dyneither(_tmp396,sizeof(char),1);});_tmp392->f4=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394[0]=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp395;_tmp395.tag=0;_tmp395.f1=Cyc_set_FINISH;_tmp395;});_tmp394;});_tmp392->f5=({const char*_tmp393="Produce Cyclone headers from pre-gathered C library info";_tag_dyneither(_tmp393,sizeof(char),57);});_tmp392;});_tmp37F[2]=({struct _tuple26*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C->f1=({const char*_tmp391="-gatherscript";_tag_dyneither(_tmp391,sizeof(char),14);});_tmp38C->f2=0;_tmp38C->f3=({const char*_tmp390="";_tag_dyneither(_tmp390,sizeof(char),1);});_tmp38C->f4=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E[0]=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp38F;_tmp38F.tag=0;_tmp38F.f1=Cyc_set_GATHERSCRIPT;_tmp38F;});_tmp38E;});_tmp38C->f5=({const char*_tmp38D="Produce a script to gather C library info";_tag_dyneither(_tmp38D,sizeof(char),42);});_tmp38C;});_tmp37F[1]=({struct _tuple26*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386->f1=({const char*_tmp38B="-gather";_tag_dyneither(_tmp38B,sizeof(char),8);});_tmp386->f2=0;_tmp386->f3=({const char*_tmp38A="";_tag_dyneither(_tmp38A,sizeof(char),1);});_tmp386->f4=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388[0]=({struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp389;_tmp389.tag=0;_tmp389.f1=Cyc_set_GATHER;_tmp389;});_tmp388;});_tmp386->f5=({const char*_tmp387="Gather C library info but don't produce Cyclone headers";_tag_dyneither(_tmp387,sizeof(char),56);});_tmp386;});_tmp37F[0]=({struct _tuple26*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->f1=({const char*_tmp385="-d";_tag_dyneither(_tmp385,sizeof(char),3);});_tmp380->f2=0;_tmp380->f3=({const char*_tmp384=" <file>";_tag_dyneither(_tmp384,sizeof(char),8);});_tmp380->f4=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382[0]=({struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp383;_tmp383.tag=5;_tmp383.f1=Cyc_set_output_dir;_tmp383;});_tmp382;});_tmp380->f5=({const char*_tmp381="Set the output directory to <file>";_tag_dyneither(_tmp381,sizeof(char),35);});_tmp380;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp37F,sizeof(struct _tuple26*),8));});
# 1865
Cyc_Arg_parse(options,Cyc_add_spec_file,Cyc_no_other,({const char*_tmp35A="Options:";_tag_dyneither(_tmp35A,sizeof(char),9);}),argv);
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){
Cyc_Arg_usage(options,({const char*_tmp35B="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_dyneither(_tmp35B,sizeof(char),59);}));
# 1874
return 1;}
# 1877
if(Cyc_mode == Cyc_GATHERSCRIPT){
if(Cyc_verbose)
({void*_tmp35C=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp35D="Creating BUILDLIB.sh\n";_tag_dyneither(_tmp35D,sizeof(char),22);}),_tag_dyneither(_tmp35C,sizeof(void*),0));});
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned int)Cyc_script_file)){
({void*_tmp35E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp35F="Could not create file BUILDLIB.sh\n";_tag_dyneither(_tmp35F,sizeof(char),35);}),_tag_dyneither(_tmp35E,sizeof(void*),0));});
exit(1);}
# 1885
({void*_tmp360=0;Cyc_prscript(({const char*_tmp361="#!/bin/sh\n";_tag_dyneither(_tmp361,sizeof(char),11);}),_tag_dyneither(_tmp360,sizeof(void*),0));});
({void*_tmp362=0;Cyc_prscript(({const char*_tmp363="GCC=\"gcc\"\n";_tag_dyneither(_tmp363,sizeof(char),11);}),_tag_dyneither(_tmp362,sizeof(void*),0));});}
# 1890
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp366;_tmp366.tag=0;_tmp366.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp364[1]={& _tmp366};Cyc_fprintf(Cyc_stderr,({const char*_tmp365="Error: could not create directory %s\n";_tag_dyneither(_tmp365,sizeof(char),38);}),_tag_dyneither(_tmp364,sizeof(void*),1));});});
return 1;}
# 1894
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp369;_tmp369.tag=0;_tmp369.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp367[1]={& _tmp369};Cyc_fprintf(Cyc_stderr,({const char*_tmp368="Output directory is %s\n";_tag_dyneither(_tmp368,sizeof(char),24);}),_tag_dyneither(_tmp367,sizeof(void*),1));});});
# 1897
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp36C;_tmp36C.tag=0;_tmp36C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp36A[1]={& _tmp36C};Cyc_prscript(({const char*_tmp36B="cd %s\n";_tag_dyneither(_tmp36B,sizeof(char),7);}),_tag_dyneither(_tmp36A,sizeof(void*),1));});});
({void*_tmp36D=0;Cyc_prscript(({const char*_tmp36E="echo | $GCC -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp36E,sizeof(char),38);}),_tag_dyneither(_tmp36D,sizeof(void*),0));});}
# 1902
if(!Cyc_gathering()){
# 1905
Cyc_log_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp36F="BUILDLIB.LOG";_tag_dyneither(_tmp36F,sizeof(char),13);})),sizeof(char),1),"w");
if(!((unsigned int)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp372;_tmp372.tag=0;_tmp372.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp370[1]={& _tmp372};Cyc_fprintf(Cyc_stderr,({const char*_tmp371="Error: could not create log file in directory %s\n";_tag_dyneither(_tmp371,sizeof(char),50);}),_tag_dyneither(_tmp370,sizeof(void*),1));});});
return 1;}
# 1911
if(!Cyc_do_setjmp){
# 1913
Cyc_cstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp373="cstubs.c";_tag_dyneither(_tmp373,sizeof(char),9);})),sizeof(char),1),"w");
if(!((unsigned int)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp376;_tmp376.tag=0;_tmp376.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp374[1]={& _tmp376};Cyc_fprintf(Cyc_stderr,({const char*_tmp375="Error: could not create cstubs.c in directory %s\n";_tag_dyneither(_tmp375,sizeof(char),50);}),_tag_dyneither(_tmp374,sizeof(void*),1));});});
return 1;}
# 1920
Cyc_cycstubs_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_tmp377="cycstubs.cyc";_tag_dyneither(_tmp377,sizeof(char),13);})),sizeof(char),1),"w");
if(!((unsigned int)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp37A;_tmp37A.tag=0;_tmp37A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);({void*_tmp378[1]={& _tmp37A};Cyc_fprintf(Cyc_stderr,({const char*_tmp379="Error: could not create cycstubs.c in directory %s\n";_tag_dyneither(_tmp379,sizeof(char),52);}),_tag_dyneither(_tmp378,sizeof(void*),1));});});
# 1925
return 1;}
# 1927
({void*_tmp37B=0;Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file),({const char*_tmp37C="#include <core.h>\nusing Core;\n\n";_tag_dyneither(_tmp37C,sizeof(char),32);}),_tag_dyneither(_tmp37B,sizeof(void*),0));});}}{
# 1934
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 1940
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp37D=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp37E="FATAL ERROR -- QUIT!\n";_tag_dyneither(_tmp37E,sizeof(char),22);}),_tag_dyneither(_tmp37D,sizeof(void*),0));});
exit(1);}}}
# 1948
if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 1951
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 1959
return 0;};};}
