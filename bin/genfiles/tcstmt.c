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
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 113
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 299 "core.h"
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
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Relations_Reln;
# 73 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 79
typedef void*Cyc_Tcpat_decision_opt_t;
# 87
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 99
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 103
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 106
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
# 111
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
# 162
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 183
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 188
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 195
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 215
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 217
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 226
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 287
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 365
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 386
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 391
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 446 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 462
typedef void*Cyc_Absyn_funcparams_t;
# 465
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 529
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 555
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 562
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 580
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 743 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 944
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 946
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 960
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 969
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 988
void*Cyc_Absyn_exn_typ();
# 1010
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1026
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple2*,unsigned int);
# 1101
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1114
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1125
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1130
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1134
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1186
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 65 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
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
# 43 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 54
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 73
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 77
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 88
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 107 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 124
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 135
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 137
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 141
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 145
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 150
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct _tuple12{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 168
const struct _tuple12*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 171
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple13**oldtv,unsigned int loc);
# 179
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 181
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 185
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 197
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 73
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 120
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 164
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 192
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 227 "tcutil.h"
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 235
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 286
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 291
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 308
int Cyc_Tcutil_new_tvar_id();
# 325
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 328
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 346
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 350
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 367
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 54 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 58
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 60
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};
# 75
typedef void*Cyc_Tcpat_pat_test_t;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};
# 83
typedef void*Cyc_Tcpat_access_t;struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};
# 93
typedef struct Cyc_Tcpat_PathNode*Cyc_Tcpat_path_node_t;
# 95
typedef void*Cyc_Tcpat_term_desc_t;
typedef struct Cyc_List_List*Cyc_Tcpat_path_t;struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};
# 103
typedef struct Cyc_Tcpat_Rhs*Cyc_Tcpat_rhs_t;
# 105
typedef void*Cyc_Tcpat_decision_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 112
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 114
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p,void**);
# 116
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 30 "jump_analysis.h"
typedef struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_table_t;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 45 "jump_analysis.h"
typedef struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_anal_res_t;
# 47
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 43 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 54
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 65
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 67
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 72
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 87
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 92
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 100
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 121
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 128
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 56 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp2;struct Cyc_Absyn_Exp*_tmp3;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 4)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2:
# 59
 e->r=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4[0]=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp5;_tmp5.tag=4;_tmp5.f1=_tmp3;_tmp5.f2=Cyc_Absyn_PreInc;_tmp5;});_tmp4;});goto _LL0;case Cyc_Absyn_PostDec: _LL3: _tmp2=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4:
# 61
 e->r=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp7;_tmp7.tag=4;_tmp7.f1=_tmp2;_tmp7.f2=Cyc_Absyn_PreDec;_tmp7;});_tmp6;});goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}struct _tuple15{struct Cyc_Absyn_Tvar*f1;int f2;};
# 67
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 69
struct Cyc_Tcpat_TcPatResult _tmp8=pat_res;struct _tuple1*_tmp9;struct Cyc_List_List*_tmpA;_LL8: _tmp9=_tmp8.tvars_and_bounds_opt;_tmpA=_tmp8.patvars;_LL9:;{
struct _tuple1 _tmpB=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpA);struct _tuple1 _tmpC=_tmpB;struct Cyc_List_List*_tmpD;_LLB: _tmpD=_tmpC.f1;_LLC:;{
struct Cyc_List_List*_tmpE=_tmp9 == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmp9).f1);
struct Cyc_List_List*_tmpF=_tmp9 == 0?0:(*_tmp9).f2;
struct Cyc_List_List*_tmp10=_tmp9 == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple15*))Cyc_Core_snd,(*_tmp9).f1));
struct _RegionHandle _tmp11=_new_region("r");struct _RegionHandle*r=& _tmp11;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,loc,te,_tmpE);
for(0;_tmp10 != 0;_tmp10=_tmp10->tl){
te2=Cyc_Tcenv_add_region(r,te2,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp13;_tmp13.tag=2;_tmp13.f1=(struct Cyc_Absyn_Tvar*)_tmp10->hd;_tmp13;});_tmp12;}),0,1);}
te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmpF,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(r,loc,te2);{
void*_tmp14=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp15=_tmpD;for(0;_tmp15 != 0;_tmp15=_tmp15->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp15->hd != 0){
te2=Cyc_Tcenv_add_pat_var(r,loc,te2,*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp15->hd)));
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp15->hd)))->rgn=_tmp14;}}}
# 88
if(where_opt != 0)
Cyc_Tcexp_tcTest(te2,where_opt,({const char*_tmp16="switch clause guard";_tag_dyneither(_tmp16,sizeof(char),20);}));
Cyc_Tcstmt_tcStmt(te2,s,0);};}
# 75
;_pop_region(r);};};}
# 94
static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Tcenv_CList*const x){
# 96
if(x == (const struct Cyc_Tcenv_CList*)0)return 0;else{
return({struct Cyc_List_List*_tmp17=_region_malloc(r,sizeof(*_tmp17));_tmp17->hd=f(env,x->hd);_tmp17->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl);_tmp17;});}}
# 102
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){
# 105
struct _RegionHandle _tmp18=_new_region("r");struct _RegionHandle*r=& _tmp18;_push_region(r);
{struct _tuple13*tk=0;
struct Cyc_Tcenv_Tenv*_tmp19=Cyc_Tcenv_add_region_equality(r,te,r1,r2,& tk,s0->loc);
Cyc_Tcstmt_tcStmt(_tmp19,s0,new_block);
if(tk != 0)
((*((struct _tuple13*)_check_null(tk))).f1)->kind=(*((struct _tuple13*)_check_null(tk))).f2;}
# 106
;_pop_region(r);}
# 114
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp1A=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _tuple2*res=({struct _tuple2*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->f1=Cyc_Absyn_Loc_n;_tmp1B->f2=({struct _dyneither_ptr*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1F;_tmp1F.tag=1;_tmp1F.f1=(unsigned int)_tmp1A;({void*_tmp1D[1]={& _tmp1F};Cyc_aprintf(({const char*_tmp1E="_stmttmp%X";_tag_dyneither(_tmp1E,sizeof(char),11);}),_tag_dyneither(_tmp1D,sizeof(void*),1));});});_tmp1C;});_tmp1B;});
return res;}
# 128 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 130
void*_tmp20=s0->r;void*_tmp21=_tmp20;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Decl*_tmp23;struct Cyc_Absyn_Stmt*_tmp24;struct Cyc_Absyn_Stmt*_tmp25;struct Cyc_List_List**_tmp26;void**_tmp27;struct Cyc_Absyn_Exp**_tmp28;struct Cyc_List_List*_tmp29;void**_tmp2A;struct _dyneither_ptr*_tmp2B;struct Cyc_Absyn_Stmt*_tmp2C;struct Cyc_List_List*_tmp2D;struct Cyc_Absyn_Switch_clause***_tmp2E;struct Cyc_Absyn_Stmt*_tmp2F;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Stmt*_tmp31;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Exp*_tmp33;struct Cyc_Absyn_Stmt*_tmp34;struct Cyc_Absyn_Exp*_tmp35;struct Cyc_Absyn_Stmt*_tmp36;struct Cyc_Absyn_Stmt*_tmp37;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Stmt*_tmp3A;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Stmt*_tmp3C;struct Cyc_Absyn_Stmt*_tmp3D;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Stmt*_tmp3F;struct Cyc_Absyn_Stmt*_tmp40;struct Cyc_Absyn_Exp*_tmp41;switch(*((int*)_tmp21)){case 0: _LLE: _LLF:
# 132
 return;case 1: _LL10: _tmp41=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_LL11:
# 135
 Cyc_Tcexp_tcExp(te,0,_tmp41);
if(!Cyc_Tcexp_in_stmt_exp)
Cyc_Tcstmt_simplify_unused_result_exp(_tmp41);
return;case 2: _LL12: _tmp3F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp40=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL13:
# 141
 Cyc_Tcstmt_tcStmt(te,_tmp3F,1);
Cyc_Tcstmt_tcStmt(te,_tmp40,1);
return;case 3: _LL14: _tmp3E=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_LL15: {
# 146
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp3E == 0){
void*_tmp42=Cyc_Tcutil_compress(t);void*_tmp43=_tmp42;switch(*((int*)_tmp43)){case 0: _LL31: _LL32:
 goto _LL30;case 7: _LL33: _LL34:
 goto _LL36;case 6: _LL35: _LL36:
# 152
({struct Cyc_String_pa_PrintArg_struct _tmp46;_tmp46.tag=0;_tmp46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp44[1]={& _tmp46};Cyc_Tcutil_warn(s0->loc,({const char*_tmp45="should return a value of type %s";_tag_dyneither(_tmp45,sizeof(char),33);}),_tag_dyneither(_tmp44,sizeof(void*),1));});});goto _LL30;default: _LL37: _LL38:
# 154
({struct Cyc_String_pa_PrintArg_struct _tmp49;_tmp49.tag=0;_tmp49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp47[1]={& _tmp49};Cyc_Tcutil_terr(s0->loc,({const char*_tmp48="must return a value of type %s";_tag_dyneither(_tmp48,sizeof(char),31);}),_tag_dyneither(_tmp47,sizeof(void*),1));});});goto _LL30;}_LL30:;}else{
# 158
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp3E;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({void*_tmp4A[2]={& _tmp4C,& _tmp4D};Cyc_Tcutil_terr(s0->loc,({const char*_tmp4B="returns value of type %s but requires %s";_tag_dyneither(_tmp4B,sizeof(char),41);}),_tag_dyneither(_tmp4A,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}{
# 166
struct _RegionHandle _tmp4E=_new_region("temp");struct _RegionHandle*temp=& _tmp4E;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e))
({void*_tmp4F=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp50="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp50,sizeof(char),49);}),_tag_dyneither(_tmp4F,sizeof(void*),0));});
# 167
;_pop_region(temp);};}
# 171
return;}case 4: _LL16: _tmp3B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp3C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_tmp3D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp21)->f3;_LL17: {
# 174
struct Cyc_Tcexp_TestEnv _tmp51=Cyc_Tcexp_tcTest(te,_tmp3B,({const char*_tmp58="if statement";_tag_dyneither(_tmp58,sizeof(char),13);}));struct Cyc_Tcexp_TestEnv _tmp52=_tmp51;struct _tuple0*_tmp53;int _tmp54;_LL3A: _tmp53=_tmp52.eq;_tmp54=_tmp52.isTrue;_LL3B:;
{struct _tuple0*_tmp55=_tmp53;void*_tmp56;void*_tmp57;if(_tmp55 != 0){_LL3D: _tmp56=_tmp55->f1;_tmp57=_tmp55->f2;_LL3E:
# 177
 if(_tmp54){
Cyc_Tcstmt_tcStmtRefine(te,_tmp3C,_tmp56,_tmp57,1);
Cyc_Tcstmt_tcStmt(te,_tmp3D,1);}else{
# 182
Cyc_Tcstmt_tcStmt(te,_tmp3C,1);
Cyc_Tcstmt_tcStmtRefine(te,_tmp3D,_tmp56,_tmp57,1);}
# 185
goto _LL3C;}else{_LL3F: _LL40:
# 187
 Cyc_Tcstmt_tcStmt(te,_tmp3C,1);
Cyc_Tcstmt_tcStmt(te,_tmp3D,1);
goto _LL3C;}_LL3C:;}
# 191
return;}case 5: _LL18: _tmp38=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp21)->f1).f1;_tmp39=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp21)->f1).f2;_tmp3A=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL19:
# 194
 Cyc_Tcexp_tcTest(te,_tmp38,({const char*_tmp59="while loop";_tag_dyneither(_tmp59,sizeof(char),11);}));
Cyc_Tcstmt_tcStmt(te,_tmp3A,1);
return;case 9: _LL1A: _tmp32=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp33=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f1;_tmp34=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f2;_tmp35=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f3).f1;_tmp36=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f3).f2;_tmp37=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp21)->f4;_LL1B:
# 199
 Cyc_Tcexp_tcExp(te,0,_tmp32);
Cyc_Tcexp_tcTest(te,_tmp33,({const char*_tmp5A="for loop";_tag_dyneither(_tmp5A,sizeof(char),9);}));{
struct _RegionHandle _tmp5B=_new_region("r");struct _RegionHandle*r=& _tmp5B;_push_region(r);
Cyc_Tcstmt_tcStmt(te,_tmp37,1);
Cyc_Tcexp_tcExp(te,0,_tmp35);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp35);
# 206
_npop_handler(0);return;
# 201
;_pop_region(r);};case 14: _LL1C: _tmp2F=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp30=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f1;_tmp31=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp21)->f2).f2;_LL1D:
# 209
 Cyc_Tcstmt_tcStmt(te,_tmp2F,1);
Cyc_Tcexp_tcTest(te,_tmp30,({const char*_tmp5C="do loop";_tag_dyneither(_tmp5C,sizeof(char),8);}));
return;case 6: _LL1E: _LL1F:
# 213
 goto _LL21;case 7: _LL20: _LL21:
 goto _LL23;case 8: _LL22: _LL23:
 return;case 11: _LL24: _tmp2D=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp2E=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL25: {
# 218
const struct _tuple12*_tmp5D=Cyc_Tcenv_process_fallthru(te,s0,_tmp2E);
if(_tmp5D == (const struct _tuple12*)0){
({void*_tmp5E=0;Cyc_Tcutil_terr(s0->loc,({const char*_tmp5F="fallthru not in a non-last case";_tag_dyneither(_tmp5F,sizeof(char),32);}),_tag_dyneither(_tmp5E,sizeof(void*),0));});
return;}{
# 223
struct Cyc_List_List*_tmp60=(*_tmp5D).f2;
const struct Cyc_Tcenv_CList*_tmp61=(*_tmp5D).f3;
struct Cyc_List_List*instantiation=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp60);
struct Cyc_List_List*_tmp62=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,const struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(Cyc_Core_heap_region,Cyc_Tcutil_substitute,instantiation,_tmp61);
for(0;_tmp62 != 0  && _tmp2D != 0;(_tmp62=_tmp62->tl,_tmp2D=_tmp2D->tl)){
# 230
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp2D->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp2D->hd,(void*)_tmp62->hd,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 235
Cyc_Absynpp_typ2string((void*)_tmp62->hd));({struct Cyc_String_pa_PrintArg_struct _tmp65;_tmp65.tag=0;_tmp65.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp2D->hd)->topt)));({void*_tmp63[2]={& _tmp65,& _tmp66};Cyc_Tcutil_terr(s0->loc,({const char*_tmp64="fallthru argument has type %s but pattern variable has type %s";_tag_dyneither(_tmp64,sizeof(char),63);}),_tag_dyneither(_tmp63,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}{
# 239
struct _RegionHandle _tmp67=_new_region("temp");struct _RegionHandle*temp=& _tmp67;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_tmp62->hd) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmp2D->hd))
({void*_tmp68=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp2D->hd)->loc,({const char*_tmp69="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp69,sizeof(char),49);}),_tag_dyneither(_tmp68,sizeof(void*),0));});
# 240
;_pop_region(temp);};}
# 245
if(_tmp2D != 0)
({void*_tmp6A=0;Cyc_Tcutil_terr(s0->loc,({const char*_tmp6B="too many arguments in explicit fallthru";_tag_dyneither(_tmp6B,sizeof(char),40);}),_tag_dyneither(_tmp6A,sizeof(void*),0));});
if(_tmp62 != 0)
({void*_tmp6C=0;Cyc_Tcutil_terr(s0->loc,({const char*_tmp6D="too few arguments in explicit fallthru";_tag_dyneither(_tmp6D,sizeof(char),39);}),_tag_dyneither(_tmp6C,sizeof(void*),0));});
return;};}case 13: _LL26: _tmp2B=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp2C=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL27: {
# 255
struct _RegionHandle _tmp6E=_new_region("r");struct _RegionHandle*r=& _tmp6E;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(r,s0->loc,te,({struct Cyc_Absyn_Tvar*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->name=({struct _dyneither_ptr*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2B);({void*_tmp71[1]={& _tmp73};Cyc_aprintf(({const char*_tmp72="`%s";_tag_dyneither(_tmp72,sizeof(char),4);}),_tag_dyneither(_tmp71,sizeof(void*),1));});});_tmp70;});_tmp6F->identity=
# 258
Cyc_Tcutil_new_tvar_id();_tmp6F->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp6F;})),_tmp2C,0);
# 261
_npop_handler(0);return;
# 255
;_pop_region(r);}case 10: _LL28: _tmp28=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp29=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_tmp2A=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp21)->f3;_LL29: {
# 267
struct Cyc_Absyn_Exp*_tmp74=*_tmp28;
if(!Cyc_Tcutil_is_var_exp(_tmp74)){
struct _tuple2*_tmp75=Cyc_Tcstmt_stmt_temp_var();
struct Cyc_Absyn_Stmt*_tmp76=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmp77=Cyc_Absyn_let_decl(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp7A;_tmp7A.tag=1;_tmp7A.f1=Cyc_Absyn_new_vardecl(_tmp75,
Cyc_Absyn_new_evar(0,0),0);_tmp7A.f2=
# 274
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp74->loc);_tmp7A;});_tmp79;}),_tmp74->loc),_tmp74,s0->loc);
# 277
struct Cyc_Absyn_Stmt*_tmp78=Cyc_Absyn_decl_stmt(_tmp77,_tmp76,s0->loc);
# 279
s0->r=_tmp78->r;
*_tmp28=Cyc_Absyn_var_exp(_tmp75,_tmp74->loc);
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}
# 285
Cyc_Tcexp_tcExp(te,0,_tmp74);{
void*_tmp7B=(void*)_check_null(_tmp74->topt);
# 290
struct _RegionHandle _tmp7C=_new_region("r");struct _RegionHandle*r=& _tmp7C;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmp7B) && !Cyc_Tcutil_is_noalias_path(r,_tmp74))
({void*_tmp7D=0;Cyc_Tcutil_terr(_tmp74->loc,({const char*_tmp7E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp7E,sizeof(char),49);}),_tag_dyneither(_tmp7D,sizeof(void*),0));});{
struct Cyc_Tcenv_Tenv*_tmp7F=Cyc_Tcenv_clear_fallthru(r,te);
struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp29);for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Pat*_tmp80=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
struct Cyc_Tcpat_TcPatResult _tmp81=Cyc_Tcpat_tcPat(_tmp7F,_tmp80,& _tmp7B,_tmp74);
struct Cyc_List_List*_tmp82=_tmp81.tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmp81.tvars_and_bounds_opt).f1);
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp80->topt),_tmp7B)){
({struct Cyc_String_pa_PrintArg_struct _tmp86;_tmp86.tag=0;_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp80->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp85;_tmp85.tag=0;_tmp85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7B));({void*_tmp83[2]={& _tmp85,& _tmp86};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,({const char*_tmp84="switch on type %s, but case expects type %s";_tag_dyneither(_tmp84,sizeof(char),44);}),_tag_dyneither(_tmp83,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}else{
# 305
Cyc_Tcpat_check_pat_regions(_tmp7F,_tmp80,_tmp81.patvars);}
((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=({struct Cyc_Core_Opt*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->v=_tmp81.patvars;_tmp87;});
# 308
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp7F,_tmp81,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);
# 310
if(_tmp81.tvars_and_bounds_opt != 0  && (*_tmp81.tvars_and_bounds_opt).f2 != 0)
# 312
_tmp7F=Cyc_Tcenv_clear_fallthru(r,_tmp7F);else{
# 314
struct Cyc_List_List*_tmp88=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp81.patvars)).f1);
_tmp88=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp88);
_tmp7F=Cyc_Tcenv_set_fallthru(r,_tmp7F,_tmp82,_tmp88,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}};
# 320
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp29,_tmp2A);
_npop_handler(0);return;
# 290
;_pop_region(r);};}case 15: _LL2A: _tmp25=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp26=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_tmp27=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp21)->f3;_LL2B:
# 329
 _tmp25->r=(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp8A;_tmp8A.tag=2;_tmp8A.f1=Cyc_Absyn_new_stmt(_tmp25->r,_tmp25->loc);_tmp8A.f2=Cyc_Absyn_skip_stmt(_tmp25->loc);_tmp8A;});_tmp89;}),_tmp25->loc))->r;
# 332
{struct _RegionHandle _tmp8B=_new_region("r");struct _RegionHandle*r=& _tmp8B;_push_region(r);
Cyc_Tcstmt_tcStmt(te,_tmp25,1);;_pop_region(r);}{
# 337
struct _tuple2*def_v=({struct _tuple2*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->f1=Cyc_Absyn_Loc_n;_tmpA1->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"exn",sizeof(char),4);_tmpA1;});
struct Cyc_Absyn_Pat*_tmp8C=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpA0;_tmpA0.tag=1;_tmpA0.f1=Cyc_Absyn_new_vardecl(def_v,(void*)& Cyc_Absyn_VoidType_val,0);_tmpA0.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmpA0;});_tmp9F;}),0);
struct Cyc_Absyn_Stmt*_tmp8D=Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp9E;_tmp9E.tag=10;_tmp9E.f1=Cyc_Absyn_var_exp(def_v,0);_tmp9E.f2=1;_tmp9E;});_tmp9D;}),0),0);
struct Cyc_Absyn_Switch_clause*_tmp8E=({struct Cyc_Absyn_Switch_clause*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->pattern=_tmp8C;_tmp9C->pat_vars=0;_tmp9C->where_clause=0;_tmp9C->body=_tmp8D;_tmp9C->loc=0;_tmp9C;});
*_tmp26=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*_tmp26,({struct Cyc_Absyn_Switch_clause*_tmp8F[1];_tmp8F[0]=_tmp8E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp8F,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));
# 344
{struct _RegionHandle _tmp90=_new_region("r2");struct _RegionHandle*r2=& _tmp90;_push_region(r2);
# 346
{struct Cyc_Tcenv_Tenv*_tmp91=Cyc_Tcenv_clear_fallthru(r2,te);
struct Cyc_List_List*_tmp92=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(*_tmp26);for(0;_tmp92 != 0;_tmp92=_tmp92->tl){
_tmp91=Cyc_Tcenv_copy_tenv(r2,_tmp91);{
struct Cyc_Absyn_Pat*_tmp93=((struct Cyc_Absyn_Switch_clause*)_tmp92->hd)->pattern;
void*_tmp94=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult _tmp95=Cyc_Tcpat_tcPat(_tmp91,_tmp93,& _tmp94,0);
# 354
struct Cyc_List_List*_tmp96=_tmp95.tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmp95.tvars_and_bounds_opt).f1);
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp93->topt),_tmp94))
({struct Cyc_String_pa_PrintArg_struct _tmp99;_tmp99.tag=0;_tmp99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp93->topt)));({void*_tmp97[1]={& _tmp99};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmp92->hd)->loc,({const char*_tmp98="expected datatype exn@ but found %s";_tag_dyneither(_tmp98,sizeof(char),36);}),_tag_dyneither(_tmp97,sizeof(void*),1));});});else{
# 360
Cyc_Tcpat_check_pat_regions(_tmp91,_tmp93,_tmp95.patvars);}
((struct Cyc_Absyn_Switch_clause*)_tmp92->hd)->pat_vars=({struct Cyc_Core_Opt*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->v=_tmp95.patvars;_tmp9A;});
# 363
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp92->hd)->loc,_tmp91,_tmp95,((struct Cyc_Absyn_Switch_clause*)_tmp92->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp92->hd)->where_clause,1);
# 365
if(_tmp95.tvars_and_bounds_opt != 0  && (*_tmp95.tvars_and_bounds_opt).f2 != 0)
# 367
_tmp91=Cyc_Tcenv_clear_fallthru(r2,_tmp91);else{
# 369
struct Cyc_List_List*_tmp9B=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp95.patvars)).f1);
_tmp9B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9B);
_tmp91=Cyc_Tcenv_set_fallthru(r2,_tmp91,_tmp96,_tmp9B,(struct Cyc_Absyn_Switch_clause*)_tmp92->hd);}};}}
# 346
;_pop_region(r2);}
# 375
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*_tmp26,_tmp27);
return;};case 12: _LL2C: _tmp23=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_tmp24=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp21)->f2;_LL2D: {
# 379
struct _dyneither_ptr unimp_msg_part;
struct _RegionHandle _tmpA3=_new_region("decl_rgn");struct _RegionHandle*decl_rgn=& _tmpA3;_push_region(decl_rgn);{
struct Cyc_Tcenv_Tenv*te2;
if(new_block)
te2=Cyc_Tcenv_new_block(decl_rgn,s0->loc,te);else{
# 385
te2=Cyc_Tcenv_copy_tenv(decl_rgn,te);}{
struct Cyc_Tcenv_Tenv*_tmpA4=te2;
void*_tmpA5=_tmp23->r;void*_tmpA6=_tmpA5;struct _tuple2*_tmpA7;struct Cyc_List_List*_tmpA8;struct _dyneither_ptr*_tmpA9;struct Cyc_List_List*_tmpAA;struct Cyc_Absyn_Fndecl*_tmpAB;struct Cyc_Absyn_Tvar*_tmpAC;struct Cyc_Absyn_Vardecl*_tmpAD;int _tmpAE;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_List_List*_tmpB0;struct Cyc_Absyn_Pat*_tmpB1;struct Cyc_Core_Opt**_tmpB2;struct Cyc_Absyn_Exp**_tmpB3;void**_tmpB4;struct Cyc_Absyn_Vardecl*_tmpB5;switch(*((int*)_tmpA6)){case 0: _LL42: _tmpB5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA6)->f1;_LL43: {
# 389
struct Cyc_Absyn_Vardecl*_tmpB6=_tmpB5;enum Cyc_Absyn_Scope _tmpB7;union Cyc_Absyn_Nmspace _tmpB8;struct _dyneither_ptr*_tmpB9;struct Cyc_Absyn_Tqual _tmpBA;void*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;void**_tmpBD;struct Cyc_List_List*_tmpBE;_LL61: _tmpB7=_tmpB6->sc;_tmpB8=(_tmpB6->name)->f1;_tmpB9=(_tmpB6->name)->f2;_tmpBA=_tmpB6->tq;_tmpBB=_tmpB6->type;_tmpBC=_tmpB6->initializer;_tmpBD=(void**)& _tmpB6->rgn;_tmpBE=_tmpB6->attributes;_LL62:;{
void*_tmpBF=Cyc_Tcenv_curr_rgn(_tmpA4);
int is_local;
{enum Cyc_Absyn_Scope _tmpC0=_tmpB7;switch(_tmpC0){case Cyc_Absyn_Static: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Extern: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_ExternC: _LL68: _LL69:
# 396
 _tmpB5->escapes=1;
is_local=0;goto _LL63;case Cyc_Absyn_Abstract: _LL6A: _LL6B:
# 399
({void*_tmpC1=0;Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmpC2="bad abstract scope for local variable";_tag_dyneither(_tmpC2,sizeof(char),38);}),_tag_dyneither(_tmpC1,sizeof(void*),0));});
goto _LL6D;case Cyc_Absyn_Register: _LL6C: _LL6D:
 goto _LL6F;default: _LL6E: _LL6F:
 is_local=1;goto _LL63;}_LL63:;}
# 405
*_tmpBD=is_local?_tmpBF:(void*)& Cyc_Absyn_HeapRgn_val;
{union Cyc_Absyn_Nmspace _tmpC3=_tmpB8;switch((_tmpC3.C_n).tag){case 4: _LL71: _LL72:
 goto _LL70;case 1: if((_tmpC3.Rel_n).val == 0){_LL73: _LL74:
# 409
(*_tmpB5->name).f1=Cyc_Absyn_Loc_n;
goto _LL70;}else{_LL79: _LL7A:
# 416
({void*_tmpC4=0;Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmpC5="cannot declare a qualified local variable";_tag_dyneither(_tmpC5,sizeof(char),42);}),_tag_dyneither(_tmpC4,sizeof(void*),0));});
(*_tmpB5->name).f1=Cyc_Absyn_Loc_n;
goto _LL70;}case 3: _LL75: _LL76:
# 413
 goto _LL78;default: _LL77: _LL78:
 goto _LL7A;}_LL70:;}
# 421
{void*_tmpC6=Cyc_Tcutil_compress(_tmpBB);void*_tmpC7=_tmpC6;void*_tmpC8;struct Cyc_Absyn_Tqual _tmpC9;union Cyc_Absyn_Constraint*_tmpCA;unsigned int _tmpCB;switch(*((int*)_tmpC7)){case 9: _LL7C: if(is_local){_LL7D:
# 424
 _tmpB5->escapes=1;
_tmpB7=Cyc_Absyn_Extern;
is_local=0;
goto _LL7B;}else{goto _LL80;}case 8: if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC7)->f1).num_elts == 0){_LL7E: _tmpC8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC7)->f1).elt_type;_tmpC9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC7)->f1).tq;_tmpCA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC7)->f1).zero_term;_tmpCB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC7)->f1).zt_loc;if(_tmpB5->initializer != 0){_LL7F:
# 429
{void*_tmpCC=((struct Cyc_Absyn_Exp*)_check_null(_tmpB5->initializer))->r;void*_tmpCD=_tmpCC;struct Cyc_List_List*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;struct _dyneither_ptr _tmpD2;struct _dyneither_ptr _tmpD3;switch(*((int*)_tmpCD)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpCD)->f1).Wstring_c).tag){case 8: _LL83: _tmpD3=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpCD)->f1).String_c).val;_LL84:
# 431
 _tmpBB=(_tmpB5->type=Cyc_Absyn_array_typ(_tmpC8,_tmpC9,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpD3,sizeof(char)),0),_tmpCA,_tmpCB));
goto _LL82;case 9: _LL85: _tmpD2=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpCD)->f1).Wstring_c).val;_LL86:
# 434
 _tmpBB=(_tmpB5->type=Cyc_Absyn_array_typ(_tmpC8,_tmpC9,Cyc_Absyn_uint_exp(1,0),_tmpCA,_tmpCB));
goto _LL82;default: goto _LL8F;}case 26: _LL87: _tmpD1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpCD)->f2;_LL88:
 _tmpD0=_tmpD1;goto _LL8A;case 27: _LL89: _tmpD0=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpCD)->f1;_LL8A:
# 439
 _tmpBB=(_tmpB5->type=Cyc_Absyn_array_typ(_tmpC8,_tmpC9,_tmpD0,_tmpCA,_tmpCB));
goto _LL82;case 35: _LL8B: _tmpCF=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpCD)->f2;_LL8C:
 _tmpCE=_tmpCF;goto _LL8E;case 25: _LL8D: _tmpCE=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpCD)->f1;_LL8E:
# 443
 _tmpBB=(_tmpB5->type=Cyc_Absyn_array_typ(_tmpC8,_tmpC9,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpCE),0),_tmpCA,_tmpCB));
# 445
goto _LL82;default: _LL8F: _LL90:
 goto _LL82;}_LL82:;}
# 448
goto _LL7B;}else{goto _LL80;}}else{goto _LL80;}default: _LL80: _LL81:
 goto _LL7B;}_LL7B:;}{
# 455
struct Cyc_List_List*_tmpD4=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpA4);
int _tmpD5=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmpA4,_tmpD4,& Cyc_Tcutil_tmk,_tmpD5,1,_tmpBB);
(_tmpB5->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmpB5->tq).print_const,_tmpBB);{
# 460
struct Cyc_Tcenv_Tenv*_tmpD6=Cyc_Tcenv_add_local_var(decl_rgn,_tmp24->loc,_tmpA4,_tmpB5);
if(_tmpB7 == Cyc_Absyn_Extern  || _tmpB7 == Cyc_Absyn_ExternC)
({void*_tmpD7=0;Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmpD8="extern declarations are not yet supported within functions";_tag_dyneither(_tmpD8,sizeof(char),59);}),_tag_dyneither(_tmpD7,sizeof(void*),0));});
if(_tmpBC != 0){
Cyc_Tcexp_tcExpInitializer(_tmpD6,& _tmpBB,_tmpBC);
# 466
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpBC))
({void*_tmpD9=0;Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmpDA="initializer for static variable needs to be a constant expression";_tag_dyneither(_tmpDA,sizeof(char),66);}),_tag_dyneither(_tmpD9,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_assign(_tmpD6,_tmpBC,_tmpBB)){
struct _dyneither_ptr _tmpDB=*_tmpB9;
struct _dyneither_ptr _tmpDC=Cyc_Absynpp_typ2string(_tmpBB);
struct _dyneither_ptr _tmpDD=Cyc_Absynpp_typ2string((void*)_check_null(_tmpBC->topt));
if(((_get_dyneither_size(_tmpDB,sizeof(char))+ _get_dyneither_size(_tmpDC,sizeof(char)))+ _get_dyneither_size(_tmpDD,sizeof(char)))+ 50 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpE2;_tmpE2.tag=0;_tmpE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDD);({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDC);({struct Cyc_String_pa_PrintArg_struct _tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDB);({void*_tmpDE[3]={& _tmpE0,& _tmpE1,& _tmpE2};Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmpDF="%s was declared with type %s but initialized with type %s.";_tag_dyneither(_tmpDF,sizeof(char),59);}),_tag_dyneither(_tmpDE,sizeof(void*),3));});});});});else{
# 475
if((_get_dyneither_size(_tmpDB,sizeof(char))+ _get_dyneither_size(_tmpDC,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmpDD,sizeof(char))+ 25 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpE7;_tmpE7.tag=0;_tmpE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDD);({struct Cyc_String_pa_PrintArg_struct _tmpE6;_tmpE6.tag=0;_tmpE6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDC);({struct Cyc_String_pa_PrintArg_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDB);({void*_tmpE3[3]={& _tmpE5,& _tmpE6,& _tmpE7};Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmpE4="%s was declared with type %s\n but initialized with type %s.";_tag_dyneither(_tmpE4,sizeof(char),60);}),_tag_dyneither(_tmpE3,sizeof(void*),3));});});});});else{
# 480
({struct Cyc_String_pa_PrintArg_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDD);({struct Cyc_String_pa_PrintArg_struct _tmpEB;_tmpEB.tag=0;_tmpEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDC);({struct Cyc_String_pa_PrintArg_struct _tmpEA;_tmpEA.tag=0;_tmpEA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDB);({void*_tmpE8[3]={& _tmpEA,& _tmpEB,& _tmpEC};Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmpE9="%s declared with type \n%s\n but initialized with type \n%s.";_tag_dyneither(_tmpE9,sizeof(char),58);}),_tag_dyneither(_tmpE8,sizeof(void*),3));});});});});}}
# 482
Cyc_Tcutil_unify(_tmpBB,(void*)_check_null(_tmpBC->topt));
Cyc_Tcutil_explain_failure();}}
# 486
Cyc_Tcstmt_tcStmt(_tmpD6,_tmp24,0);
_npop_handler(0);return;};};};}case 2: _LL44: _tmpB1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA6)->f1;_tmpB2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA6)->f2;_tmpB3=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA6)->f3;_tmpB4=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA6)->f4;_LL45: {
# 494
struct Cyc_Absyn_Exp*_tmpED=*_tmpB3;
{void*_tmpEE=_tmpB1->r;void*_tmpEF=_tmpEE;switch(*((int*)_tmpEF)){case 1: _LL92: _LL93:
 goto _LL95;case 2: _LL94: _LL95:
 goto _LL97;case 15: _LL96: _LL97:
 goto _LL91;default: _LL98: _LL99:
# 500
 if(!Cyc_Tcutil_is_var_exp(_tmpED)){
struct _tuple2*_tmpF0=Cyc_Tcstmt_stmt_temp_var();
struct Cyc_Absyn_Stmt*_tmpF1=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmpF2=Cyc_Absyn_let_decl(Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpF5;_tmpF5.tag=1;_tmpF5.f1=Cyc_Absyn_new_vardecl(_tmpF0,
Cyc_Absyn_new_evar(0,0),0);_tmpF5.f2=
# 506
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmpED->loc);_tmpF5;});_tmpF4;}),_tmpED->loc),_tmpED,s0->loc);
# 509
struct Cyc_Absyn_Stmt*_tmpF3=Cyc_Absyn_decl_stmt(_tmpF2,_tmpF1,s0->loc);
# 511
s0->r=_tmpF3->r;
*_tmpB3=Cyc_Absyn_var_exp(_tmpF0,_tmpED->loc);
Cyc_Tcstmt_tcStmt(_tmpA4,s0,new_block);
_npop_handler(0);return;}}_LL91:;}
# 517
Cyc_Tcexp_tcExpInitializer(_tmpA4,0,_tmpED);{
# 519
void*pat_type=(void*)_check_null(_tmpED->topt);
# 521
struct Cyc_Tcpat_TcPatResult _tmpF6=Cyc_Tcpat_tcPat(_tmpA4,_tmpB1,& pat_type,_tmpED);
*_tmpB2=({struct Cyc_Core_Opt*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->v=_tmpF6.patvars;_tmpF7;});
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpB1->topt),(void*)_check_null(_tmpED->topt)) && !
Cyc_Tcutil_coerce_assign(_tmpA4,_tmpED,(void*)_check_null(_tmpB1->topt))){
({struct Cyc_String_pa_PrintArg_struct _tmpFB;_tmpFB.tag=0;_tmpFB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpED->topt)));({struct Cyc_String_pa_PrintArg_struct _tmpFA;_tmpFA.tag=0;_tmpFA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpB1->topt)));({void*_tmpF8[2]={& _tmpFA,& _tmpFB};Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmpF9="pattern type %s does not match definition type %s";_tag_dyneither(_tmpF9,sizeof(char),50);}),_tag_dyneither(_tmpF8,sizeof(void*),2));});});});
Cyc_Tcutil_unify((void*)_check_null(_tmpB1->topt),(void*)_check_null(_tmpED->topt));
Cyc_Tcutil_explain_failure();}else{
# 531
Cyc_Tcpat_check_pat_regions(_tmpA4,_tmpB1,_tmpF6.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmpB1->loc,_tmpA4,_tmpB1,_tmpB4);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmpA4,_tmpF6,_tmp24,0,0);
_npop_handler(0);return;};}case 3: _LL46: _tmpB0=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA6)->f1;_LL47: {
# 537
void*_tmpFC=Cyc_Tcenv_curr_rgn(_tmpA4);
struct Cyc_Tcenv_Tenv*_tmpFD=_tmpA4;
for(0;_tmpB0 != 0;_tmpB0=_tmpB0->tl){
struct Cyc_Absyn_Vardecl*_tmpFE=(struct Cyc_Absyn_Vardecl*)_tmpB0->hd;
struct Cyc_Absyn_Vardecl*_tmpFF=_tmpFE;union Cyc_Absyn_Nmspace _tmp100;void*_tmp101;void**_tmp102;_LL9B: _tmp100=(_tmpFF->name)->f1;_tmp101=_tmpFF->type;_tmp102=(void**)& _tmpFF->rgn;_LL9C:;
*_tmp102=_tmpFC;
{union Cyc_Absyn_Nmspace _tmp103=_tmp100;struct Cyc_List_List*_tmp104;switch((_tmp103.Abs_n).tag){case 4: _LL9E: _LL9F:
 goto _LL9D;case 1: if((_tmp103.Rel_n).val == 0){_LLA0: _LLA1:
# 546
(*_tmpFE->name).f1=Cyc_Absyn_Loc_n;
goto _LL9D;}else{goto _LLA4;}case 2: _LLA2: _tmp104=(_tmp103.Abs_n).val;_LLA3:
(int)_throw(({void*_tmp105=0;((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp106="tcstmt: Abs_n declaration";_tag_dyneither(_tmp106,sizeof(char),26);}),_tag_dyneither(_tmp105,sizeof(void*),0));}));default: _LLA4: _LLA5:
# 552
({void*_tmp107=0;Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmp108="cannot declare a qualified local variable";_tag_dyneither(_tmp108,sizeof(char),42);}),_tag_dyneither(_tmp107,sizeof(void*),0));});
goto _LL9D;}_LL9D:;}
# 555
Cyc_Tcutil_check_type(s0->loc,_tmpFD,Cyc_Tcenv_lookup_type_vars(_tmpFD),& Cyc_Tcutil_tmk,1,1,_tmp101);
_tmpFD=Cyc_Tcenv_add_local_var(decl_rgn,_tmp24->loc,_tmpFD,_tmpFE);}
# 558
Cyc_Tcstmt_tcStmt(_tmpFD,_tmp24,0);
_npop_handler(0);return;}case 4: _LL48: _tmpAC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA6)->f1;_tmpAD=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA6)->f2;_tmpAE=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA6)->f3;_tmpAF=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA6)->f4;_LL49:
# 564
 _tmp24->r=(Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp10A;_tmp10A.tag=2;_tmp10A.f1=Cyc_Absyn_new_stmt(_tmp24->r,_tmp24->loc);_tmp10A.f2=Cyc_Absyn_skip_stmt(_tmp24->loc);_tmp10A;});_tmp109;}),_tmp24->loc))->r;{
# 566
struct Cyc_Tcenv_Tenv*_tmp10B=_tmpA4;
struct Cyc_Absyn_Vardecl*_tmp10C=_tmpAD;void**_tmp10D;void**_tmp10E;_LLA7: _tmp10D=(void**)& _tmp10C->type;_tmp10E=(void**)& _tmp10C->rgn;_LLA8:;{
void*_tmp10F=Cyc_Tcenv_curr_rgn(_tmpA4);
*_tmp10E=_tmp10F;{
void*rgn_typ;
if((unsigned int)_tmpAF){
# 574
struct _tuple2*drname=({struct _tuple2*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=({union Cyc_Absyn_Nmspace _tmp124;(_tmp124.Abs_n).val=({struct _dyneither_ptr*_tmp125[1];_tmp125[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"Core",sizeof(char),5);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp125,sizeof(struct _dyneither_ptr*),1));});(_tmp124.Abs_n).tag=2;_tmp124;});_tmp122->f2=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"DynamicRegion",sizeof(char),14);_tmp122;});
void*_tmp110=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->v=& Cyc_Tcutil_trk;_tmp121;}),0);
rgn_typ=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->v=& Cyc_Tcutil_rk;_tmp111;}),0);{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp112=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp11E;_tmp11E.tag=11;_tmp11E.f1=({struct Cyc_Absyn_AggrInfo _tmp11F;_tmp11F.aggr_info=
Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);_tmp11F.targs=({void*_tmp120[1];_tmp120[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp120,sizeof(void*),1));});_tmp11F;});_tmp11E;});_tmp11D;});
# 581
void*_tmp113=Cyc_Absyn_at_typ((void*)_tmp112,_tmp110,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);
Cyc_Tcutil_check_type(s0->loc,_tmpA4,Cyc_Tcenv_lookup_type_vars(_tmpA4),& Cyc_Tcutil_tmk,1,0,_tmp113);{
void*_tmp114=Cyc_Tcexp_tcExp(_tmpA4,& _tmp113,_tmpAF);
if(!Cyc_Tcutil_unify(_tmp113,_tmp114) && !
Cyc_Tcutil_coerce_assign(_tmpA4,_tmpAF,_tmp113))
({struct Cyc_String_pa_PrintArg_struct _tmp118;_tmp118.tag=0;_tmp118.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp114));({struct Cyc_String_pa_PrintArg_struct _tmp117;_tmp117.tag=0;_tmp117.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp113));({void*_tmp115[2]={& _tmp117,& _tmp118};Cyc_Tcutil_terr(s0->loc,({const char*_tmp116="expected %s but found %s";_tag_dyneither(_tmp116,sizeof(char),25);}),_tag_dyneither(_tmp115,sizeof(void*),2));});});});
if(!Cyc_Tcutil_unify(_tmp110,(void*)& Cyc_Absyn_UniqueRgn_val) && !
Cyc_Tcutil_unify(_tmp110,(void*)& Cyc_Absyn_RefCntRgn_val))
({void*_tmp119=0;Cyc_Tcutil_terr(s0->loc,({const char*_tmp11A="open is only allowed on unique or reference-counted keys";_tag_dyneither(_tmp11A,sizeof(char),57);}),_tag_dyneither(_tmp119,sizeof(void*),0));});
*_tmp10D=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp11C;_tmp11C.tag=15;_tmp11C.f1=rgn_typ;_tmp11C;});_tmp11B;});};};}else{
# 594
rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp128;_tmp128.tag=2;_tmp128.f1=_tmpAC;_tmp128;});_tmp127;});
_tmp10B=Cyc_Tcenv_add_type_vars(decl_rgn,s0->loc,_tmp10B,({struct Cyc_List_List*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->hd=_tmpAC;_tmp129->tl=0;_tmp129;}));}
# 597
_tmp10B=Cyc_Tcenv_add_region(decl_rgn,_tmp10B,rgn_typ,_tmpAE,1);
Cyc_Tcutil_check_type(s0->loc,_tmpA4,Cyc_Tcenv_lookup_type_vars(_tmp10B),& Cyc_Tcutil_bk,1,0,*_tmp10D);
if(!Cyc_Tcutil_unify(*_tmp10D,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp12B;_tmp12B.tag=15;_tmp12B.f1=rgn_typ;_tmp12B;});_tmp12A;})))
({void*_tmp12C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp12D="region stmt: type of region handle is wrong!";_tag_dyneither(_tmp12D,sizeof(char),45);}),_tag_dyneither(_tmp12C,sizeof(void*),0));});
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_add_local_var(decl_rgn,_tmp24->loc,_tmp10B,_tmpAD),_tmp24,0);
_npop_handler(0);return;};};};case 1: _LL4A: _tmpAB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA6)->f1;_LL4B: {
# 606
void*_tmp12E=Cyc_Tcenv_curr_rgn(_tmpA4);
if(_tmpAB->sc != Cyc_Absyn_Public)
({void*_tmp12F=0;Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmp130="bad storage class for inner function";_tag_dyneither(_tmp130,sizeof(char),37);}),_tag_dyneither(_tmp12F,sizeof(void*),0));});
# 610
{union Cyc_Absyn_Nmspace _tmp131=(*_tmpAB->name).f1;union Cyc_Absyn_Nmspace _tmp132=_tmp131;struct Cyc_List_List*_tmp133;switch((_tmp132.Abs_n).tag){case 1: if((_tmp132.Rel_n).val == 0){_LLAA: _LLAB:
 goto _LLA9;}else{goto _LLAE;}case 2: _LLAC: _tmp133=(_tmp132.Abs_n).val;_LLAD:
({void*_tmp134=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp135="tc: Abs_n in tcStmt var decl";_tag_dyneither(_tmp135,sizeof(char),29);}),_tag_dyneither(_tmp134,sizeof(void*),0));});default: _LLAE: _LLAF:
({void*_tmp136=0;Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmp137="explicit namespace not allowed on inner function declaration";_tag_dyneither(_tmp137,sizeof(char),61);}),_tag_dyneither(_tmp136,sizeof(void*),0));});}_LLA9:;}
# 615
(*_tmpAB->name).f1=Cyc_Absyn_Loc_n;
Cyc_Tcutil_check_fndecl_valid_type(_tmp23->loc,_tmpA4,_tmpAB);{
void*t=Cyc_Tcutil_fndecl2typ(_tmpAB);
_tmpAB->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,_tmpAB->attributes);
# 620
{struct Cyc_List_List*atts=_tmpAB->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_tmp138=(void*)atts->hd;void*_tmp139=_tmp138;switch(*((int*)_tmp139)){case 7: _LLB1: _LLB2:
 goto _LLB4;case 6: _LLB3: _LLB4:
({struct Cyc_String_pa_PrintArg_struct _tmp13C;_tmp13C.tag=0;_tmp13C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp13A[1]={& _tmp13C};Cyc_Tcutil_terr(_tmp23->loc,({const char*_tmp13B="bad attribute %s for function";_tag_dyneither(_tmp13B,sizeof(char),30);}),_tag_dyneither(_tmp13A,sizeof(void*),1));});});
goto _LLB0;default: _LLB5: _LLB6:
 goto _LLB0;}_LLB0:;}}{
# 628
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->sc=_tmpAB->sc;_tmp13D->name=_tmpAB->name;_tmp13D->tq=Cyc_Absyn_const_tqual(0);_tmp13D->type=
Cyc_Absyn_at_typ(t,_tmp12E,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmp13D->initializer=0;_tmp13D->rgn=_tmp12E;_tmp13D->attributes=0;_tmp13D->escapes=0;_tmp13D;});
# 632
_tmpAB->fn_vardecl=vd;
_tmpA4=Cyc_Tcenv_add_local_var(decl_rgn,_tmp23->loc,_tmpA4,vd);{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpA4->le);
_tmpA4->le=Cyc_Tcenv_nested_fenv(_tmp23->loc,old_fenv,_tmpAB);
Cyc_Tcstmt_tcStmt(_tmpA4,_tmpAB->body,0);
Cyc_Tcenv_check_delayed_effects(_tmpA4);
Cyc_Tcenv_check_delayed_constraints(_tmpA4);
_tmpA4->le=old_fenv;
Cyc_Tcstmt_tcStmt(_tmpA4,_tmp24,0);
_npop_handler(0);return;};};};}case 9: _LL4C: _tmpA9=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA6)->f1;_tmpAA=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA6)->f2;_LL4D:
 unimp_msg_part=({const char*_tmp13E="namespace";_tag_dyneither(_tmp13E,sizeof(char),10);});goto _LL41;case 10: _LL4E: _tmpA7=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA6)->f1;_tmpA8=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA6)->f2;_LL4F:
 unimp_msg_part=({const char*_tmp13F="using";_tag_dyneither(_tmp13F,sizeof(char),6);});goto _LL41;case 5: _LL50: _LL51:
 unimp_msg_part=({const char*_tmp140="type";_tag_dyneither(_tmp140,sizeof(char),5);});goto _LL41;case 6: _LL52: _LL53:
 unimp_msg_part=({const char*_tmp141="datatype";_tag_dyneither(_tmp141,sizeof(char),9);});goto _LL41;case 7: _LL54: _LL55:
 unimp_msg_part=({const char*_tmp142="enum";_tag_dyneither(_tmp142,sizeof(char),5);});goto _LL41;case 8: _LL56: _LL57:
 unimp_msg_part=({const char*_tmp143="typedef";_tag_dyneither(_tmp143,sizeof(char),8);});goto _LL41;case 11: _LL58: _LL59:
 unimp_msg_part=({const char*_tmp144="extern \"C\"";_tag_dyneither(_tmp144,sizeof(char),11);});goto _LL41;case 12: _LL5A: _LL5B:
# 650
 unimp_msg_part=({const char*_tmp145="extern \"C include\"";_tag_dyneither(_tmp145,sizeof(char),19);});goto _LL41;case 13: _LL5C: _LL5D:
 unimp_msg_part=({const char*_tmp146="__cyclone_port_on__";_tag_dyneither(_tmp146,sizeof(char),20);});goto _LL41;default: _LL5E: _LL5F:
 unimp_msg_part=({const char*_tmp147="__cyclone_port_off__";_tag_dyneither(_tmp147,sizeof(char),21);});goto _LL41;}_LL41:;};}
# 655
(int)_throw(({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0;_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part);({void*_tmp148[1]={& _tmp14A};((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp149="tcStmt: nested %s declarations unimplemented";_tag_dyneither(_tmp149,sizeof(char),45);}),_tag_dyneither(_tmp148,sizeof(void*),1));});}));
# 380
;_pop_region(decl_rgn);}default: _LL2E: _tmp22=((struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp21)->f1;_LL2F: {
# 659
void*rgn_type=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));
void*etype=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp14F;_tmp14F.tag=15;_tmp14F.f1=rgn_type;_tmp14F;});_tmp14E;});
if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,& etype,_tmp22),etype))
({struct Cyc_String_pa_PrintArg_struct _tmp14D;_tmp14D.tag=0;_tmp14D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp22->topt)));({void*_tmp14B[1]={& _tmp14D};Cyc_Tcutil_terr(_tmp22->loc,({const char*_tmp14C="expecting region_t but found %s";_tag_dyneither(_tmp14C,sizeof(char),32);}),_tag_dyneither(_tmp14B,sizeof(void*),1));});});
# 664
Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);
return;}}_LLD:;}
