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
extern int _throw_null_fn(const char *filename, unsigned lineno);
extern int _throw_arraybounds_fn(const char *filename, unsigned lineno);
extern int _throw_badalloc_fn(const char *filename, unsigned lineno);
extern int _throw_match_fn(const char *filename, unsigned lineno);
extern int _throw_fn(void* e, const char *filename, unsigned lineno);
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
typedef int Cyc_bool;struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 120 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 131
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 256 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 261
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 268
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
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 62
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);
# 80 "absyn.h"
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
# 104
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
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
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
# 155
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 176
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 181
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 188
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 208
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 210
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 219
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 245
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 523
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 908 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 910
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 921
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 928
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 930
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 936
void*Cyc_Absyn_compress_kb(void*);
# 941
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 946
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 948
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 952
extern void*Cyc_Absyn_empty_effect;
# 962
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 971
extern void*Cyc_Absyn_bounds_one;
# 976
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1149
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1151
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1153
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1155
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1157
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1161
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1163
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 125 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 129
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 151
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 218
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 44
void Cyc_Tcutil_flush_warnings();extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 53
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 80
int Cyc_Tcutil_coerceable(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
# 83
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 85
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 87
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 91
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 93
extern int Cyc_Tcutil_warn_alias_coerce;
# 96
extern int Cyc_Tcutil_warn_region_coerce;
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 109
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 116
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 121
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 126
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 133
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 138
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 143
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 147
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 152
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 154
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 158
void Cyc_Tcutil_explain_failure();
# 160
int Cyc_Tcutil_unify(void*,void*);
# 162
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 165
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 170
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 174
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 178
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 181
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 185
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 187
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 191
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 197
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 201
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 227 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 229
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 237
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 240
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 247
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 249
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 251
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 253
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 255
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 261
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 263
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 265
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 269
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 275
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 280
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 283
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 288
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 293
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple18{int f1;void*f2;};
# 297
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 305
void*Cyc_Tcutil_normalize_effect(void*e);
# 308
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 310
int Cyc_Tcutil_new_tvar_id();
# 312
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 315
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 317
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 320
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 324
int Cyc_Tcutil_bits_only(void*t);
# 327
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 330
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 333
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 337
int Cyc_Tcutil_supports_default(void*);
# 341
int Cyc_Tcutil_admits_zero(void*t);
# 344
int Cyc_Tcutil_is_noreturn(void*);
# 348
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 352
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 356
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 359
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 362
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 366
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 369
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 372
int Cyc_Tcutil_is_array(void*t);
# 376
void*Cyc_Tcutil_promote_array(void*t,void*rgn);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 41 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 46 "tcutil.cyc"
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 48
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 52
int Cyc_Tcutil_warn_region_coerce=0;
# 55
void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;
# 60
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 64
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 69
{const char*_tmpD76;if(Cyc_strcmp(((_tmpD76="(qualifiers don't match)",_tag_dyneither(_tmpD76,sizeof(char),25))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
{const char*_tmpD7A;void*_tmpD79[1];struct Cyc_String_pa_PrintArg_struct _tmpD78;(_tmpD78.tag=0,((_tmpD78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpD79[0]=& _tmpD78,Cyc_fprintf(Cyc_stderr,((_tmpD7A="  %s\n",_tag_dyneither(_tmpD7A,sizeof(char),6))),_tag_dyneither(_tmpD79,sizeof(void*),1)))))));}
return;}}{
# 73
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
{const char*_tmpD7E;void*_tmpD7D[1];struct Cyc_String_pa_PrintArg_struct _tmpD7C;(_tmpD7C.tag=0,((_tmpD7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpD7D[0]=& _tmpD7C,Cyc_fprintf(Cyc_stderr,((_tmpD7E="  %s",_tag_dyneither(_tmpD7E,sizeof(char),5))),_tag_dyneither(_tmpD7D,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpD81;void*_tmpD80;(_tmpD80=0,Cyc_fprintf(Cyc_stderr,((_tmpD81="\n\t",_tag_dyneither(_tmpD81,sizeof(char),3))),_tag_dyneither(_tmpD80,sizeof(void*),0)));}
pos=8;}else{
# 82
{const char*_tmpD84;void*_tmpD83;(_tmpD83=0,Cyc_fprintf(Cyc_stderr,((_tmpD84=" ",_tag_dyneither(_tmpD84,sizeof(char),2))),_tag_dyneither(_tmpD83,sizeof(void*),0)));}
++ pos;}
# 85
{const char*_tmpD87;void*_tmpD86;(_tmpD86=0,Cyc_fprintf(Cyc_stderr,((_tmpD87="and ",_tag_dyneither(_tmpD87,sizeof(char),5))),_tag_dyneither(_tmpD86,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpD8A;void*_tmpD89;(_tmpD89=0,Cyc_fprintf(Cyc_stderr,((_tmpD8A="\n\t",_tag_dyneither(_tmpD8A,sizeof(char),3))),_tag_dyneither(_tmpD89,sizeof(void*),0)));}
pos=8;}
# 91
{const char*_tmpD8E;void*_tmpD8D[1];struct Cyc_String_pa_PrintArg_struct _tmpD8C;(_tmpD8C.tag=0,((_tmpD8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpD8D[0]=& _tmpD8C,Cyc_fprintf(Cyc_stderr,((_tmpD8E="%s ",_tag_dyneither(_tmpD8E,sizeof(char),4))),_tag_dyneither(_tmpD8D,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpD91;void*_tmpD90;(_tmpD90=0,Cyc_fprintf(Cyc_stderr,((_tmpD91="\n\t",_tag_dyneither(_tmpD91,sizeof(char),3))),_tag_dyneither(_tmpD90,sizeof(void*),0)));}
pos=8;}
# 97
{const char*_tmpD94;void*_tmpD93;(_tmpD93=0,Cyc_fprintf(Cyc_stderr,((_tmpD94="are not compatible. ",_tag_dyneither(_tmpD94,sizeof(char),21))),_tag_dyneither(_tmpD93,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpD97;void*_tmpD96;(_tmpD96=0,Cyc_fprintf(Cyc_stderr,((_tmpD97="\n\t",_tag_dyneither(_tmpD97,sizeof(char),3))),_tag_dyneither(_tmpD96,sizeof(void*),0)));}{
# 103
const char*_tmpD9B;void*_tmpD9A[1];struct Cyc_String_pa_PrintArg_struct _tmpD99;(_tmpD99.tag=0,((_tmpD99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpD9A[0]=& _tmpD99,Cyc_fprintf(Cyc_stderr,((_tmpD9B="%s",_tag_dyneither(_tmpD9B,sizeof(char),3))),_tag_dyneither(_tmpD9A,sizeof(void*),1)))))));};}
# 105
{const char*_tmpD9E;void*_tmpD9D;(_tmpD9D=0,Cyc_fprintf(Cyc_stderr,((_tmpD9E="\n",_tag_dyneither(_tmpD9E,sizeof(char),2))),_tag_dyneither(_tmpD9D,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 109
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 112
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 115
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 118
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpDA2;void*_tmpDA1[1];struct Cyc_String_pa_PrintArg_struct _tmpDA0;(_tmpDA0.tag=0,((_tmpDA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpDA1[0]=& _tmpDA0,Cyc_fprintf(Cyc_stderr,((_tmpDA2="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpDA2,sizeof(char),36))),_tag_dyneither(_tmpDA1,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpDA5;struct Cyc_Core_Impossible_exn_struct*_tmpDA4;(int)_throw((void*)((_tmpDA4=_cycalloc(sizeof(*_tmpDA4)),((_tmpDA4[0]=((_tmpDA5.tag=Cyc_Core_Impossible,((_tmpDA5.f1=msg,_tmpDA5)))),_tmpDA4)))));};}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 126
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 130
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpDAA;void*_tmpDA9[2];struct Cyc_String_pa_PrintArg_struct _tmpDA8;struct Cyc_String_pa_PrintArg_struct _tmpDA7;(_tmpDA7.tag=0,((_tmpDA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpDA8.tag=0,((_tmpDA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpDA9[0]=& _tmpDA8,((_tmpDA9[1]=& _tmpDA7,Cyc_fprintf(Cyc_stderr,((_tmpDAA="%s::%s ",_tag_dyneither(_tmpDAA,sizeof(char),8))),_tag_dyneither(_tmpDA9,sizeof(void*),2)))))))))))));}
{const char*_tmpDAD;void*_tmpDAC;(_tmpDAC=0,Cyc_fprintf(Cyc_stderr,((_tmpDAD="\n",_tag_dyneither(_tmpDAD,sizeof(char),2))),_tag_dyneither(_tmpDAC,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 137
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 142
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 145
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpDAE;Cyc_Tcutil_warning_segs=((_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->hd=(void*)sg,((_tmpDAE->tl=Cyc_Tcutil_warning_segs,_tmpDAE))))));}{
struct _dyneither_ptr*_tmpDB1;struct Cyc_List_List*_tmpDB0;Cyc_Tcutil_warning_msgs=((_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->hd=((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1[0]=msg,_tmpDB1)))),((_tmpDB0->tl=Cyc_Tcutil_warning_msgs,_tmpDB0))))));};}
# 149
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpDB4;void*_tmpDB3;(_tmpDB3=0,Cyc_fprintf(Cyc_stderr,((_tmpDB4="***Warnings***\n",_tag_dyneither(_tmpDB4,sizeof(char),16))),_tag_dyneither(_tmpDB3,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2E=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpDB9;void*_tmpDB8[2];struct Cyc_String_pa_PrintArg_struct _tmpDB7;struct Cyc_String_pa_PrintArg_struct _tmpDB6;(_tmpDB6.tag=0,((_tmpDB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpDB7.tag=0,((_tmpDB7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp2E))->hd)),((_tmpDB8[0]=& _tmpDB7,((_tmpDB8[1]=& _tmpDB6,Cyc_fprintf(Cyc_stderr,((_tmpDB9="%s: %s\n",_tag_dyneither(_tmpDB9,sizeof(char),8))),_tag_dyneither(_tmpDB8,sizeof(void*),2)))))))))))));}
_tmp2E=_tmp2E->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 161
{const char*_tmpDBC;void*_tmpDBB;(_tmpDBB=0,Cyc_fprintf(Cyc_stderr,((_tmpDBC="**************\n",_tag_dyneither(_tmpDBC,sizeof(char),16))),_tag_dyneither(_tmpDBB,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 166
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 170
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 175
void*Cyc_Tcutil_compress(void*t){
void*_tmp35=t;void**_tmp39;void**_tmp3B;struct Cyc_Absyn_Exp*_tmp3D;void*_tmp3F;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp36=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35;if(_tmp36->tag != 1)goto _LL3;else{if((void*)_tmp36->f2 != 0)goto _LL3;}}_LL2:
 goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp35;if(_tmp37->tag != 17)goto _LL5;else{if((void*)_tmp37->f4 != 0)goto _LL5;}}_LL4:
 return t;_LL5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp38=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp35;if(_tmp38->tag != 17)goto _LL7;else{_tmp39=(void**)((void**)& _tmp38->f4);}}_LL6: {
# 180
void*ta=(void*)_check_null(*_tmp39);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp39=t2;
return t2;}_LL7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35;if(_tmp3A->tag != 1)goto _LL9;else{_tmp3B=(void**)((void**)& _tmp3A->f2);}}_LL8: {
# 186
void*ta=(void*)_check_null(*_tmp3B);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3B=t2;
return t2;}_LL9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp35;if(_tmp3C->tag != 18)goto _LLB;else{_tmp3D=_tmp3C->f1;}}_LLA:
# 192
 Cyc_Evexp_eval_const_uint_exp(_tmp3D);{
void*_stmttmp0=_tmp3D->r;void*_tmp40=_stmttmp0;void*_tmp42;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp41=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp40;if(_tmp41->tag != 38)goto _LL12;else{_tmp42=(void*)_tmp41->f1;}}_LL11:
 return Cyc_Tcutil_compress(_tmp42);_LL12:;_LL13:
 return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp35;if(_tmp3E->tag != 26)goto _LLD;else{if(_tmp3E->f2 == 0)goto _LLD;_tmp3F=*_tmp3E->f2;}}_LLC:
# 198
 return Cyc_Tcutil_compress(_tmp3F);_LLD:;_LLE:
 return t;_LL0:;}
# 206
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 210
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp43=cptr;void*_tmp44;union Cyc_Absyn_Constraint*_tmp45;_LL15: if((_tmp43->No_constr).tag != 3)goto _LL17;_LL16:
 return Cyc_Absyn_empty_conref();_LL17: if((_tmp43->Eq_constr).tag != 1)goto _LL19;_tmp44=(void*)(_tmp43->Eq_constr).val;_LL18:
 return Cyc_Absyn_new_conref(_tmp44);_LL19: if((_tmp43->Forward_constr).tag != 2)goto _LL14;_tmp45=(union Cyc_Absyn_Constraint*)(_tmp43->Forward_constr).val;_LL1A:
 return Cyc_Tcutil_copy_conref(_tmp45);_LL14:;}
# 217
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_stmttmp1=Cyc_Absyn_compress_kb(kb);void*_tmp46=_stmttmp1;struct Cyc_Absyn_Kind*_tmp49;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp47=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp46;if(_tmp47->tag != 1)goto _LL1E;}_LL1D: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpDBF;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpDBE;return(void*)((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE[0]=((_tmpDBF.tag=1,((_tmpDBF.f1=0,_tmpDBF)))),_tmpDBE))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp48=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp46;if(_tmp48->tag != 2)goto _LL20;else{_tmp49=_tmp48->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDC2;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpDC1;return(void*)((_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1[0]=((_tmpDC2.tag=2,((_tmpDC2.f1=0,((_tmpDC2.f2=_tmp49,_tmpDC2)))))),_tmpDC1))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 224
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 226
struct Cyc_Absyn_Tvar*_tmpDC3;return(_tmpDC3=_cycalloc(sizeof(*_tmpDC3)),((_tmpDC3->name=tv->name,((_tmpDC3->identity=- 1,((_tmpDC3->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpDC3)))))));}
# 228
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 230
struct _dyneither_ptr*_tmp50;struct Cyc_Absyn_Tqual _tmp51;void*_tmp52;struct _tuple10*_tmp4F=arg;_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;_tmp52=_tmp4F->f3;{
struct _tuple10*_tmpDC4;return(_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->f1=_tmp50,((_tmpDC4->f2=_tmp51,((_tmpDC4->f3=Cyc_Tcutil_copy_type(_tmp52),_tmpDC4)))))));};}
# 233
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;struct _tuple12*_tmp54=arg;_tmp55=_tmp54->f1;_tmp56=_tmp54->f2;{
struct _tuple12*_tmpDC5;return(_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5->f1=_tmp55,((_tmpDC5->f2=Cyc_Tcutil_copy_type(_tmp56),_tmpDC5)))));};}
# 237
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpDC6;return(_tmpDC6=_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6->name=f->name,((_tmpDC6->tq=f->tq,((_tmpDC6->type=Cyc_Tcutil_copy_type(f->type),((_tmpDC6->width=f->width,((_tmpDC6->attributes=f->attributes,((_tmpDC6->requires_clause=f->requires_clause,_tmpDC6)))))))))))));}
# 242
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp5A;void*_tmp5B;struct _tuple0*_tmp59=x;_tmp5A=_tmp59->f1;_tmp5B=_tmp59->f2;{
struct _tuple0*_tmpDC7;return(_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->f1=Cyc_Tcutil_copy_type(_tmp5A),((_tmpDC7->f2=Cyc_Tcutil_copy_type(_tmp5B),_tmpDC7)))));};}
# 246
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpDC8;return(_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8->name=f->name,((_tmpDC8->tag=f->tag,((_tmpDC8->loc=f->loc,_tmpDC8)))))));}
# 249
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDCB;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDCA;return(void*)((_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA[0]=((_tmpDCB.tag=2,((_tmpDCB.f1=Cyc_Tcutil_copy_tvar(t),_tmpDCB)))),_tmpDCA))));}
# 252
void*Cyc_Tcutil_copy_type(void*t){
void*_stmttmp2=Cyc_Tcutil_compress(t);void*_tmp60=_stmttmp2;struct Cyc_Absyn_Tvar*_tmp64;union Cyc_Absyn_DatatypeInfoU _tmp66;struct Cyc_List_List*_tmp67;union Cyc_Absyn_DatatypeFieldInfoU _tmp69;struct Cyc_List_List*_tmp6A;void*_tmp6C;struct Cyc_Absyn_Tqual _tmp6D;void*_tmp6E;union Cyc_Absyn_Constraint*_tmp6F;union Cyc_Absyn_Constraint*_tmp70;union Cyc_Absyn_Constraint*_tmp71;struct Cyc_Absyn_PtrLoc*_tmp72;void*_tmp76;struct Cyc_Absyn_Tqual _tmp77;struct Cyc_Absyn_Exp*_tmp78;union Cyc_Absyn_Constraint*_tmp79;unsigned int _tmp7A;struct Cyc_List_List*_tmp7C;void*_tmp7D;struct Cyc_Absyn_Tqual _tmp7E;void*_tmp7F;struct Cyc_List_List*_tmp80;int _tmp81;struct Cyc_Absyn_VarargInfo*_tmp82;struct Cyc_List_List*_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp86;enum Cyc_Absyn_AggrKind _tmp88;struct _tuple2*_tmp89;struct Cyc_Core_Opt*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Absyn_Aggrdecl**_tmp8D;struct Cyc_List_List*_tmp8E;enum Cyc_Absyn_AggrKind _tmp90;struct Cyc_List_List*_tmp91;struct _tuple2*_tmp93;struct Cyc_Absyn_Enumdecl*_tmp94;struct Cyc_List_List*_tmp96;void*_tmp98;struct Cyc_Absyn_Exp*_tmp9A;void*_tmp9C;void*_tmp9E;void*_tmp9F;struct _tuple2*_tmpA1;struct Cyc_List_List*_tmpA2;struct Cyc_Absyn_Typedefdecl*_tmpA3;void*_tmpA8;struct Cyc_List_List*_tmpAA;void*_tmpAC;struct Cyc_Absyn_Aggrdecl*_tmpAF;struct Cyc_Absyn_Enumdecl*_tmpB2;struct Cyc_Absyn_Datatypedecl*_tmpB5;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp61=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp60;if(_tmp61->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp62=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp60;if(_tmp62->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp63=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp60;if(_tmp63->tag != 2)goto _LL29;else{_tmp64=_tmp63->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDCE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpDCD;return(void*)((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDCE.tag=2,((_tmpDCE.f1=Cyc_Tcutil_copy_tvar(_tmp64),_tmpDCE)))),_tmpDCD))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp65=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp60;if(_tmp65->tag != 3)goto _LL2B;else{_tmp66=(_tmp65->f1).datatype_info;_tmp67=(_tmp65->f1).targs;}}_LL2A: {
# 258
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpDD4;struct Cyc_Absyn_DatatypeInfo _tmpDD3;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpDD2;return(void*)((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2[0]=((_tmpDD4.tag=3,((_tmpDD4.f1=((_tmpDD3.datatype_info=_tmp66,((_tmpDD3.targs=Cyc_Tcutil_copy_types(_tmp67),_tmpDD3)))),_tmpDD4)))),_tmpDD2))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp68=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp60;if(_tmp68->tag != 4)goto _LL2D;else{_tmp69=(_tmp68->f1).field_info;_tmp6A=(_tmp68->f1).targs;}}_LL2C: {
# 260
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpDDA;struct Cyc_Absyn_DatatypeFieldInfo _tmpDD9;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpDD8;return(void*)((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8[0]=((_tmpDDA.tag=4,((_tmpDDA.f1=((_tmpDD9.field_info=_tmp69,((_tmpDD9.targs=Cyc_Tcutil_copy_types(_tmp6A),_tmpDD9)))),_tmpDDA)))),_tmpDD8))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60;if(_tmp6B->tag != 5)goto _LL2F;else{_tmp6C=(_tmp6B->f1).elt_typ;_tmp6D=(_tmp6B->f1).elt_tq;_tmp6E=((_tmp6B->f1).ptr_atts).rgn;_tmp6F=((_tmp6B->f1).ptr_atts).nullable;_tmp70=((_tmp6B->f1).ptr_atts).bounds;_tmp71=((_tmp6B->f1).ptr_atts).zero_term;_tmp72=((_tmp6B->f1).ptr_atts).ptrloc;}}_LL2E: {
# 262
void*_tmpBE=Cyc_Tcutil_copy_type(_tmp6C);
void*_tmpBF=Cyc_Tcutil_copy_type(_tmp6E);
union Cyc_Absyn_Constraint*_tmpC0=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp6F);
struct Cyc_Absyn_Tqual _tmpC1=_tmp6D;
union Cyc_Absyn_Constraint*_tmpC2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp70);
union Cyc_Absyn_Constraint*_tmpC3=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp71);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpDE0;struct Cyc_Absyn_PtrInfo _tmpDDF;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDDE;return(void*)((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE[0]=((_tmpDE0.tag=5,((_tmpDE0.f1=((_tmpDDF.elt_typ=_tmpBE,((_tmpDDF.elt_tq=_tmpC1,((_tmpDDF.ptr_atts=(((_tmpDDF.ptr_atts).rgn=_tmpBF,(((_tmpDDF.ptr_atts).nullable=_tmpC0,(((_tmpDDF.ptr_atts).bounds=_tmpC2,(((_tmpDDF.ptr_atts).zero_term=_tmpC3,(((_tmpDDF.ptr_atts).ptrloc=_tmp72,_tmpDDF.ptr_atts)))))))))),_tmpDDF)))))),_tmpDE0)))),_tmpDDE))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp60;if(_tmp73->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp60;if(_tmp74->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp75=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp60;if(_tmp75->tag != 8)goto _LL35;else{_tmp76=(_tmp75->f1).elt_type;_tmp77=(_tmp75->f1).tq;_tmp78=(_tmp75->f1).num_elts;_tmp79=(_tmp75->f1).zero_term;_tmp7A=(_tmp75->f1).zt_loc;}}_LL34: {
# 273
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDE6;struct Cyc_Absyn_ArrayInfo _tmpDE5;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDE4;return(void*)((_tmpDE4=_cycalloc(sizeof(*_tmpDE4)),((_tmpDE4[0]=((_tmpDE6.tag=8,((_tmpDE6.f1=((_tmpDE5.elt_type=Cyc_Tcutil_copy_type(_tmp76),((_tmpDE5.tq=_tmp77,((_tmpDE5.num_elts=_tmp78,((_tmpDE5.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp79),((_tmpDE5.zt_loc=_tmp7A,_tmpDE5)))))))))),_tmpDE6)))),_tmpDE4))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp60;if(_tmp7B->tag != 9)goto _LL37;else{_tmp7C=(_tmp7B->f1).tvars;_tmp7D=(_tmp7B->f1).effect;_tmp7E=(_tmp7B->f1).ret_tqual;_tmp7F=(_tmp7B->f1).ret_typ;_tmp80=(_tmp7B->f1).args;_tmp81=(_tmp7B->f1).c_varargs;_tmp82=(_tmp7B->f1).cyc_varargs;_tmp83=(_tmp7B->f1).rgn_po;_tmp84=(_tmp7B->f1).attributes;}}_LL36: {
# 275
struct Cyc_List_List*_tmpCA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp7C);
void*effopt2=_tmp7D == 0?0: Cyc_Tcutil_copy_type(_tmp7D);
void*_tmpCB=Cyc_Tcutil_copy_type(_tmp7F);
struct Cyc_List_List*_tmpCC=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp80);
int _tmpCD=_tmp81;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp82 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp82;
struct Cyc_Absyn_VarargInfo*_tmpDE7;cyc_varargs2=((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->name=cv->name,((_tmpDE7->tq=cv->tq,((_tmpDE7->type=Cyc_Tcutil_copy_type(cv->type),((_tmpDE7->inject=cv->inject,_tmpDE7))))))))));}{
# 286
struct Cyc_List_List*_tmpCF=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp83);
struct Cyc_List_List*_tmpD0=_tmp84;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpDED;struct Cyc_Absyn_FnInfo _tmpDEC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDEB;return(void*)((_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB[0]=((_tmpDED.tag=9,((_tmpDED.f1=((_tmpDEC.tvars=_tmpCA,((_tmpDEC.effect=effopt2,((_tmpDEC.ret_tqual=_tmp7E,((_tmpDEC.ret_typ=_tmpCB,((_tmpDEC.args=_tmpCC,((_tmpDEC.c_varargs=_tmpCD,((_tmpDEC.cyc_varargs=cyc_varargs2,((_tmpDEC.rgn_po=_tmpCF,((_tmpDEC.attributes=_tmpD0,_tmpDEC)))))))))))))))))),_tmpDED)))),_tmpDEB))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp85=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp60;if(_tmp85->tag != 10)goto _LL39;else{_tmp86=_tmp85->f1;}}_LL38: {
# 290
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpDF0;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDEF;return(void*)((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF[0]=((_tmpDF0.tag=10,((_tmpDF0.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp86),_tmpDF0)))),_tmpDEF))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp87=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp60;if(_tmp87->tag != 11)goto _LL3B;else{if((((_tmp87->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp88=((struct _tuple4)(((_tmp87->f1).aggr_info).UnknownAggr).val).f1;_tmp89=((struct _tuple4)(((_tmp87->f1).aggr_info).UnknownAggr).val).f2;_tmp8A=((struct _tuple4)(((_tmp87->f1).aggr_info).UnknownAggr).val).f3;_tmp8B=(_tmp87->f1).targs;}}_LL3A: {
# 292
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDF6;struct Cyc_Absyn_AggrInfo _tmpDF5;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDF4;return(void*)((_tmpDF4=_cycalloc(sizeof(*_tmpDF4)),((_tmpDF4[0]=((_tmpDF6.tag=11,((_tmpDF6.f1=((_tmpDF5.aggr_info=Cyc_Absyn_UnknownAggr(_tmp88,_tmp89,_tmp8A),((_tmpDF5.targs=Cyc_Tcutil_copy_types(_tmp8B),_tmpDF5)))),_tmpDF6)))),_tmpDF4))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp60;if(_tmp8C->tag != 11)goto _LL3D;else{if((((_tmp8C->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp8D=(struct Cyc_Absyn_Aggrdecl**)(((_tmp8C->f1).aggr_info).KnownAggr).val;_tmp8E=(_tmp8C->f1).targs;}}_LL3C: {
# 294
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDFC;struct Cyc_Absyn_AggrInfo _tmpDFB;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDFA;return(void*)((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA[0]=((_tmpDFC.tag=11,((_tmpDFC.f1=((_tmpDFB.aggr_info=Cyc_Absyn_KnownAggr(_tmp8D),((_tmpDFB.targs=Cyc_Tcutil_copy_types(_tmp8E),_tmpDFB)))),_tmpDFC)))),_tmpDFA))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp60;if(_tmp8F->tag != 12)goto _LL3F;else{_tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpDFF;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDFE;return(void*)((_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE[0]=((_tmpDFF.tag=12,((_tmpDFF.f1=_tmp90,((_tmpDFF.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp91),_tmpDFF)))))),_tmpDFE))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp92=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp60;if(_tmp92->tag != 13)goto _LL41;else{_tmp93=_tmp92->f1;_tmp94=_tmp92->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE02;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE01;return(void*)((_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01[0]=((_tmpE02.tag=13,((_tmpE02.f1=_tmp93,((_tmpE02.f2=_tmp94,_tmpE02)))))),_tmpE01))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp95=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp60;if(_tmp95->tag != 14)goto _LL43;else{_tmp96=_tmp95->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpE05;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpE04;return(void*)((_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04[0]=((_tmpE05.tag=14,((_tmpE05.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp96),_tmpE05)))),_tmpE04))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp60;if(_tmp97->tag != 19)goto _LL45;else{_tmp98=(void*)_tmp97->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE08;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE07;return(void*)((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07[0]=((_tmpE08.tag=19,((_tmpE08.f1=Cyc_Tcutil_copy_type(_tmp98),_tmpE08)))),_tmpE07))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp99=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp60;if(_tmp99->tag != 18)goto _LL47;else{_tmp9A=_tmp99->f1;}}_LL46: {
# 301
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpE0B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpE0A;return(void*)((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A[0]=((_tmpE0B.tag=18,((_tmpE0B.f1=_tmp9A,_tmpE0B)))),_tmpE0A))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp60;if(_tmp9B->tag != 15)goto _LL49;else{_tmp9C=(void*)_tmp9B->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpE0E;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpE0D;return(void*)((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D[0]=((_tmpE0E.tag=15,((_tmpE0E.f1=Cyc_Tcutil_copy_type(_tmp9C),_tmpE0E)))),_tmpE0D))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp60;if(_tmp9D->tag != 16)goto _LL4B;else{_tmp9E=(void*)_tmp9D->f1;_tmp9F=(void*)_tmp9D->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpE11;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpE10;return(void*)((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10[0]=((_tmpE11.tag=16,((_tmpE11.f1=Cyc_Tcutil_copy_type(_tmp9E),((_tmpE11.f2=Cyc_Tcutil_copy_type(_tmp9F),_tmpE11)))))),_tmpE10))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp60;if(_tmpA0->tag != 17)goto _LL4D;else{_tmpA1=_tmpA0->f1;_tmpA2=_tmpA0->f2;_tmpA3=_tmpA0->f3;}}_LL4C: {
# 305
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpE14;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE13;return(void*)((_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13[0]=((_tmpE14.tag=17,((_tmpE14.f1=_tmpA1,((_tmpE14.f2=Cyc_Tcutil_copy_types(_tmpA2),((_tmpE14.f3=_tmpA3,((_tmpE14.f4=0,_tmpE14)))))))))),_tmpE13))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp60;if(_tmpA4->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp60;if(_tmpA5->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp60;if(_tmpA6->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpA7=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp60;if(_tmpA7->tag != 23)goto _LL55;else{_tmpA8=(void*)_tmpA7->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE17;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE16;return(void*)((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16[0]=((_tmpE17.tag=23,((_tmpE17.f1=Cyc_Tcutil_copy_type(_tmpA8),_tmpE17)))),_tmpE16))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp60;if(_tmpA9->tag != 24)goto _LL57;else{_tmpAA=_tmpA9->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE1A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpE19;return(void*)((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19[0]=((_tmpE1A.tag=24,((_tmpE1A.f1=Cyc_Tcutil_copy_types(_tmpAA),_tmpE1A)))),_tmpE19))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp60;if(_tmpAB->tag != 25)goto _LL59;else{_tmpAC=(void*)_tmpAB->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE1D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpE1C;return(void*)((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C[0]=((_tmpE1D.tag=25,((_tmpE1D.f1=Cyc_Tcutil_copy_type(_tmpAC),_tmpE1D)))),_tmpE1C))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp60;if(_tmpAD->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpAE=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpAD->f1)->r;if(_tmpAE->tag != 0)goto _LL5B;else{_tmpAF=_tmpAE->f1;}}}_LL5A: {
# 314
struct Cyc_List_List*_tmpF2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpAF->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpE23;struct Cyc_Absyn_AggrInfo _tmpE22;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE21;return(void*)((_tmpE21=_cycalloc(sizeof(*_tmpE21)),((_tmpE21[0]=((_tmpE23.tag=11,((_tmpE23.f1=((_tmpE22.aggr_info=Cyc_Absyn_UnknownAggr(_tmpAF->kind,_tmpAF->name,0),((_tmpE22.targs=_tmpF2,_tmpE22)))),_tmpE23)))),_tmpE21))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp60;if(_tmpB0->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB1=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB0->f1)->r;if(_tmpB1->tag != 1)goto _LL5D;else{_tmpB2=_tmpB1->f1;}}}_LL5C: {
# 317
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpE26;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE25;return(void*)((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25[0]=((_tmpE26.tag=13,((_tmpE26.f1=_tmpB2->name,((_tmpE26.f2=0,_tmpE26)))))),_tmpE25))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB3=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp60;if(_tmpB3->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB4=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpB3->f1)->r;if(_tmpB4->tag != 2)goto _LL22;else{_tmpB5=_tmpB4->f1;}}}_LL5E: {
# 319
struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB5->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpE30;struct Cyc_Absyn_UnknownDatatypeInfo _tmpE2F;struct Cyc_Absyn_DatatypeInfo _tmpE2E;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpE2D;return(void*)((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=((_tmpE30.tag=3,((_tmpE30.f1=((_tmpE2E.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpE2F.name=_tmpB5->name,((_tmpE2F.is_extensible=0,_tmpE2F))))),((_tmpE2E.targs=_tmpF8,_tmpE2E)))),_tmpE30)))),_tmpE2D))));}_LL22:;}
# 334 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmpFD=d;struct Cyc_Absyn_Exp*_tmpFF;struct _dyneither_ptr*_tmp101;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpFE=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpFD;if(_tmpFE->tag != 0)goto _LL62;else{_tmpFF=_tmpFE->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpE33;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpE32;return(void*)((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=((_tmpE33.tag=0,((_tmpE33.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmpFF),_tmpE33)))),_tmpE32))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp100=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpFD;if(_tmp100->tag != 1)goto _LL5F;else{_tmp101=_tmp100->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 340
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 342
struct _tuple19*_tmpE34;return(_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1),((_tmpE34->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2),_tmpE34)))));}
# 345
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmp105=preserve_types;
{void*_stmttmp3=e->r;void*_tmp106=_stmttmp3;union Cyc_Absyn_Cnst _tmp108;struct _tuple2*_tmp10A;void*_tmp10B;enum Cyc_Absyn_Primop _tmp10D;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Core_Opt*_tmp111;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp114;enum Cyc_Absyn_Incrementor _tmp115;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_List_List*_tmp125;struct Cyc_Absyn_VarargCallInfo*_tmp126;int _tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_List_List*_tmp12E;void*_tmp130;struct Cyc_Absyn_Exp*_tmp131;int _tmp132;enum Cyc_Absyn_Coercion _tmp133;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp138;void*_tmp13A;struct Cyc_Absyn_Exp*_tmp13C;void*_tmp13E;struct Cyc_List_List*_tmp13F;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp143;struct _dyneither_ptr*_tmp144;int _tmp145;int _tmp146;struct Cyc_Absyn_Exp*_tmp148;struct _dyneither_ptr*_tmp149;int _tmp14A;int _tmp14B;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_List_List*_tmp150;struct _dyneither_ptr*_tmp152;struct Cyc_Absyn_Tqual _tmp153;void*_tmp154;struct Cyc_List_List*_tmp155;struct Cyc_List_List*_tmp157;struct Cyc_Absyn_Vardecl*_tmp159;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;int _tmp15C;struct Cyc_Absyn_Exp*_tmp15E;void*_tmp15F;int _tmp160;struct _tuple2*_tmp162;struct Cyc_List_List*_tmp163;struct Cyc_List_List*_tmp164;struct Cyc_Absyn_Aggrdecl*_tmp165;void*_tmp167;struct Cyc_List_List*_tmp168;struct Cyc_List_List*_tmp16A;struct Cyc_Absyn_Datatypedecl*_tmp16B;struct Cyc_Absyn_Datatypefield*_tmp16C;struct _tuple2*_tmp16E;struct Cyc_Absyn_Enumdecl*_tmp16F;struct Cyc_Absyn_Enumfield*_tmp170;struct _tuple2*_tmp172;void*_tmp173;struct Cyc_Absyn_Enumfield*_tmp174;int _tmp176;struct Cyc_Absyn_Exp*_tmp177;void**_tmp178;struct Cyc_Absyn_Exp*_tmp179;int _tmp17A;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;struct Cyc_Core_Opt*_tmp17F;struct Cyc_List_List*_tmp180;struct Cyc_Absyn_Exp*_tmp183;struct _dyneither_ptr*_tmp184;void*_tmp186;int _tmp188;struct _dyneither_ptr _tmp189;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp107=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp107->tag != 0)goto _LL67;else{_tmp108=_tmp107->f1;}}_LL66:
 new_e=Cyc_Absyn_const_exp(_tmp108,e->loc);goto _LL64;_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp109=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp109->tag != 1)goto _LL69;else{_tmp10A=_tmp109->f1;_tmp10B=(void*)_tmp109->f2;}}_LL68:
 new_e=Cyc_Absyn_varb_exp(_tmp10A,_tmp10B,e->loc);goto _LL64;_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp10C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp10C->tag != 2)goto _LL6B;else{_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}}_LL6A:
 new_e=Cyc_Absyn_primop_exp(_tmp10D,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp105,_tmp10E),e->loc);goto _LL64;_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp10F->tag != 3)goto _LL6D;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;_tmp112=_tmp10F->f3;}}_LL6C:
# 353
{struct Cyc_Core_Opt*_tmpE35;new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp110),(unsigned int)_tmp111?(_tmpE35=_cycalloc_atomic(sizeof(*_tmpE35)),((_tmpE35->v=(void*)((enum Cyc_Absyn_Primop)_tmp111->v),_tmpE35))): 0,Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp112),e->loc);}
goto _LL64;_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp113->tag != 4)goto _LL6F;else{_tmp114=_tmp113->f1;_tmp115=_tmp113->f2;}}_LL6E:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp114),_tmp115,e->loc);goto _LL64;_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp116=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp116->tag != 5)goto _LL71;else{_tmp117=_tmp116->f1;_tmp118=_tmp116->f2;_tmp119=_tmp116->f3;}}_LL70:
# 357
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp117),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp118),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp119),e->loc);goto _LL64;_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp11A->tag != 6)goto _LL73;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;}}_LL72:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp11B),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp11C),e->loc);goto _LL64;_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp11D->tag != 7)goto _LL75;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;}}_LL74:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp11E),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp11F),e->loc);goto _LL64;_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp120=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp120->tag != 8)goto _LL77;else{_tmp121=_tmp120->f1;_tmp122=_tmp120->f2;}}_LL76:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp121),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp122),e->loc);goto _LL64;_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp123=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp123->tag != 9)goto _LL79;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;_tmp126=_tmp123->f3;_tmp127=_tmp123->f4;}}_LL78:
# 362
{struct Cyc_Absyn_VarargCallInfo*_tmp18B=_tmp126;int _tmp18C;struct Cyc_List_List*_tmp18D;struct Cyc_Absyn_VarargInfo*_tmp18E;_LLB6: if(_tmp18B == 0)goto _LLB8;_tmp18C=_tmp18B->num_varargs;_tmp18D=_tmp18B->injectors;_tmp18E=_tmp18B->vai;_LLB7: {
# 364
struct _dyneither_ptr*_tmp190;struct Cyc_Absyn_Tqual _tmp191;void*_tmp192;int _tmp193;struct Cyc_Absyn_VarargInfo*_tmp18F=_tmp18E;_tmp190=_tmp18F->name;_tmp191=_tmp18F->tq;_tmp192=_tmp18F->type;_tmp193=_tmp18F->inject;
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE3F;struct Cyc_Absyn_VarargInfo*_tmpE3E;struct Cyc_Absyn_VarargCallInfo*_tmpE3D;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE3C;new_e=Cyc_Absyn_new_exp((void*)((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C[0]=((_tmpE3F.tag=9,((_tmpE3F.f1=
Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp124),((_tmpE3F.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp105,_tmp125),((_tmpE3F.f3=(
(_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D->num_varargs=_tmp18C,((_tmpE3D->injectors=_tmp18D,((_tmpE3D->vai=(
(_tmpE3E=_cycalloc(sizeof(*_tmpE3E)),((_tmpE3E->name=_tmp190,((_tmpE3E->tq=_tmp191,((_tmpE3E->type=Cyc_Tcutil_copy_type(_tmp192),((_tmpE3E->inject=_tmp193,_tmpE3E)))))))))),_tmpE3D)))))))),((_tmpE3F.f4=_tmp127,_tmpE3F)))))))))),_tmpE3C)))),e->loc);}
# 370
goto _LLB5;}_LLB8:;_LLB9:
# 372
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE42;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE41;new_e=Cyc_Absyn_new_exp((void*)((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41[0]=((_tmpE42.tag=9,((_tmpE42.f1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp124),((_tmpE42.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp105,_tmp125),((_tmpE42.f3=_tmp126,((_tmpE42.f4=_tmp127,_tmpE42)))))))))),_tmpE41)))),e->loc);}
goto _LLB5;_LLB5:;}
# 375
goto _LL64;_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp128->tag != 10)goto _LL7B;else{_tmp129=_tmp128->f1;}}_LL7A:
 new_e=Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp129),e->loc);goto _LL64;_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp12A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp12A->tag != 11)goto _LL7D;else{_tmp12B=_tmp12A->f1;}}_LL7C:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp12B),e->loc);
goto _LL64;_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp12C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp12C->tag != 12)goto _LL7F;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;}}_LL7E:
# 380
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp12D),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp12E),e->loc);
goto _LL64;_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp12F->tag != 13)goto _LL81;else{_tmp130=(void*)_tmp12F->f1;_tmp131=_tmp12F->f2;_tmp132=_tmp12F->f3;_tmp133=_tmp12F->f4;}}_LL80:
# 383
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp130),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp131),_tmp132,_tmp133,e->loc);goto _LL64;_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp134->tag != 14)goto _LL83;else{_tmp135=_tmp134->f1;}}_LL82:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp135),e->loc);goto _LL64;_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp136->tag != 15)goto _LL85;else{_tmp137=_tmp136->f1;_tmp138=_tmp136->f2;}}_LL84: {
# 386
struct Cyc_Absyn_Exp*eo1=_tmp137;if(_tmp137 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp137);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp138),e->loc);
goto _LL64;}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp139=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp139->tag != 16)goto _LL87;else{_tmp13A=(void*)_tmp139->f1;}}_LL86:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp13A),e->loc);
goto _LL64;_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp13B=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp13B->tag != 17)goto _LL89;else{_tmp13C=_tmp13B->f1;}}_LL88:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp13C),e->loc);goto _LL64;_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp13D=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp13D->tag != 18)goto _LL8B;else{_tmp13E=(void*)_tmp13D->f1;_tmp13F=_tmp13D->f2;}}_LL8A:
# 393
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp13E),_tmp13F,e->loc);goto _LL64;_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp140=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp140->tag != 19)goto _LL8D;else{_tmp141=_tmp140->f1;}}_LL8C:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp141),e->loc);goto _LL64;_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp142=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp142->tag != 20)goto _LL8F;else{_tmp143=_tmp142->f1;_tmp144=_tmp142->f2;_tmp145=_tmp142->f3;_tmp146=_tmp142->f4;}}_LL8E:
# 396
{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpE45;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpE44;new_e=Cyc_Absyn_new_exp((void*)((_tmpE44=_cycalloc(sizeof(*_tmpE44)),((_tmpE44[0]=((_tmpE45.tag=20,((_tmpE45.f1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp143),((_tmpE45.f2=_tmp144,((_tmpE45.f3=_tmp145,((_tmpE45.f4=_tmp146,_tmpE45)))))))))),_tmpE44)))),e->loc);}goto _LL64;_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp147=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp147->tag != 21)goto _LL91;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;_tmp14A=_tmp147->f3;_tmp14B=_tmp147->f4;}}_LL90:
# 398
{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpE48;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpE47;new_e=Cyc_Absyn_new_exp((void*)((_tmpE47=_cycalloc(sizeof(*_tmpE47)),((_tmpE47[0]=((_tmpE48.tag=21,((_tmpE48.f1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp148),((_tmpE48.f2=_tmp149,((_tmpE48.f3=_tmp14A,((_tmpE48.f4=_tmp14B,_tmpE48)))))))))),_tmpE47)))),e->loc);}goto _LL64;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp14C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp14C->tag != 22)goto _LL93;else{_tmp14D=_tmp14C->f1;_tmp14E=_tmp14C->f2;}}_LL92:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp14D),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp14E),e->loc);
goto _LL64;_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp14F->tag != 23)goto _LL95;else{_tmp150=_tmp14F->f1;}}_LL94:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp105,_tmp150),e->loc);goto _LL64;_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp151=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp151->tag != 24)goto _LL97;else{_tmp152=(_tmp151->f1)->f1;_tmp153=(_tmp151->f1)->f2;_tmp154=(_tmp151->f1)->f3;_tmp155=_tmp151->f2;}}_LL96: {
# 403
struct _dyneither_ptr*vopt1=_tmp152;
if(_tmp152 != 0)vopt1=_tmp152;
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpE4E;struct _tuple10*_tmpE4D;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpE4C;new_e=Cyc_Absyn_new_exp((void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((_tmpE4E.tag=24,((_tmpE4E.f1=((_tmpE4D=_cycalloc(sizeof(*_tmpE4D)),((_tmpE4D->f1=vopt1,((_tmpE4D->f2=_tmp153,((_tmpE4D->f3=Cyc_Tcutil_copy_type(_tmp154),_tmpE4D)))))))),((_tmpE4E.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp105,_tmp155),_tmpE4E)))))),_tmpE4C)))),e->loc);}
goto _LL64;}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp156->tag != 25)goto _LL99;else{_tmp157=_tmp156->f1;}}_LL98:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpE51;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE50;new_e=Cyc_Absyn_new_exp((void*)((_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50[0]=((_tmpE51.tag=25,((_tmpE51.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp105,_tmp157),_tmpE51)))),_tmpE50)))),e->loc);}
goto _LL64;_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp158->tag != 26)goto _LL9B;else{_tmp159=_tmp158->f1;_tmp15A=_tmp158->f2;_tmp15B=_tmp158->f3;_tmp15C=_tmp158->f4;}}_LL9A:
# 411
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpE54;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE53;new_e=Cyc_Absyn_new_exp((void*)((_tmpE53=_cycalloc(sizeof(*_tmpE53)),((_tmpE53[0]=((_tmpE54.tag=26,((_tmpE54.f1=_tmp159,((_tmpE54.f2=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp15A),((_tmpE54.f3=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp15B),((_tmpE54.f4=_tmp15C,_tmpE54)))))))))),_tmpE53)))),e->loc);}
goto _LL64;_LL9B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp15D->tag != 27)goto _LL9D;else{_tmp15E=_tmp15D->f1;_tmp15F=(void*)_tmp15D->f2;_tmp160=_tmp15D->f3;}}_LL9C:
# 414
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpE57;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpE56;new_e=Cyc_Absyn_new_exp((void*)((_tmpE56=_cycalloc(sizeof(*_tmpE56)),((_tmpE56[0]=((_tmpE57.tag=27,((_tmpE57.f1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp15E),((_tmpE57.f2=Cyc_Tcutil_copy_type(_tmp15F),((_tmpE57.f3=_tmp160,_tmpE57)))))))),_tmpE56)))),_tmp15E->loc);}
# 416
goto _LL64;_LL9D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp161->tag != 28)goto _LL9F;else{_tmp162=_tmp161->f1;_tmp163=_tmp161->f2;_tmp164=_tmp161->f3;_tmp165=_tmp161->f4;}}_LL9E:
# 418
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpE5A;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE59;new_e=Cyc_Absyn_new_exp((void*)((_tmpE59=_cycalloc(sizeof(*_tmpE59)),((_tmpE59[0]=((_tmpE5A.tag=28,((_tmpE5A.f1=_tmp162,((_tmpE5A.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp163),((_tmpE5A.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp105,_tmp164),((_tmpE5A.f4=_tmp165,_tmpE5A)))))))))),_tmpE59)))),e->loc);}
# 420
goto _LL64;_LL9F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp166->tag != 29)goto _LLA1;else{_tmp167=(void*)_tmp166->f1;_tmp168=_tmp166->f2;}}_LLA0:
# 422
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE5D;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE5C;new_e=Cyc_Absyn_new_exp((void*)((_tmpE5C=_cycalloc(sizeof(*_tmpE5C)),((_tmpE5C[0]=((_tmpE5D.tag=29,((_tmpE5D.f1=Cyc_Tcutil_copy_type(_tmp167),((_tmpE5D.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp105,_tmp168),_tmpE5D)))))),_tmpE5C)))),e->loc);}
goto _LL64;_LLA1: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp169=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp169->tag != 30)goto _LLA3;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;_tmp16C=_tmp169->f3;}}_LLA2:
# 425
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpE60;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE5F;new_e=Cyc_Absyn_new_exp((void*)((_tmpE5F=_cycalloc(sizeof(*_tmpE5F)),((_tmpE5F[0]=((_tmpE60.tag=30,((_tmpE60.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp105,_tmp16A),((_tmpE60.f2=_tmp16B,((_tmpE60.f3=_tmp16C,_tmpE60)))))))),_tmpE5F)))),e->loc);}
goto _LL64;_LLA3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp16D->tag != 31)goto _LLA5;else{_tmp16E=_tmp16D->f1;_tmp16F=_tmp16D->f2;_tmp170=_tmp16D->f3;}}_LLA4:
 new_e=e;goto _LL64;_LLA5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp171->tag != 32)goto _LLA7;else{_tmp172=_tmp171->f1;_tmp173=(void*)_tmp171->f2;_tmp174=_tmp171->f3;}}_LLA6:
# 429
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpE63;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpE62;new_e=Cyc_Absyn_new_exp((void*)((_tmpE62=_cycalloc(sizeof(*_tmpE62)),((_tmpE62[0]=((_tmpE63.tag=32,((_tmpE63.f1=_tmp172,((_tmpE63.f2=Cyc_Tcutil_copy_type(_tmp173),((_tmpE63.f3=_tmp174,_tmpE63)))))))),_tmpE62)))),e->loc);}
goto _LL64;_LLA7: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp175->tag != 33)goto _LLA9;else{_tmp176=(_tmp175->f1).is_calloc;_tmp177=(_tmp175->f1).rgn;_tmp178=(_tmp175->f1).elt_type;_tmp179=(_tmp175->f1).num_elts;_tmp17A=(_tmp175->f1).fat_result;}}_LLA8: {
# 432
struct Cyc_Absyn_Exp*_tmp1AF=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp177;if(_tmp177 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp177);{
void**t1=_tmp178;if(_tmp178 != 0){void**_tmpE64;t1=((_tmpE64=_cycalloc(sizeof(*_tmpE64)),((_tmpE64[0]=Cyc_Tcutil_copy_type(*_tmp178),_tmpE64))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE6A;struct Cyc_Absyn_MallocInfo _tmpE69;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE68;_tmp1AF->r=(void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE6A.tag=33,((_tmpE6A.f1=((_tmpE69.is_calloc=_tmp176,((_tmpE69.rgn=r1,((_tmpE69.elt_type=t1,((_tmpE69.num_elts=_tmp179,((_tmpE69.fat_result=_tmp17A,_tmpE69)))))))))),_tmpE6A)))),_tmpE68))));}
new_e=_tmp1AF;
goto _LL64;};}_LLA9: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp17B->tag != 34)goto _LLAB;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;}}_LLAA:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp17C),Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp17D),e->loc);goto _LL64;_LLAB: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp17E=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp17E->tag != 35)goto _LLAD;else{_tmp17F=_tmp17E->f1;_tmp180=_tmp17E->f2;}}_LLAC: {
# 440
struct Cyc_Core_Opt*nopt1=_tmp17F;
if(_tmp17F != 0){struct Cyc_Core_Opt*_tmpE6B;nopt1=((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B->v=(struct _tuple2*)_tmp17F->v,_tmpE6B))));}
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpE6E;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpE6D;new_e=Cyc_Absyn_new_exp((void*)((_tmpE6D=_cycalloc(sizeof(*_tmpE6D)),((_tmpE6D[0]=((_tmpE6E.tag=35,((_tmpE6E.f1=nopt1,((_tmpE6E.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp105,_tmp180),_tmpE6E)))))),_tmpE6D)))),e->loc);}
goto _LL64;}_LLAD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp181=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp181->tag != 36)goto _LLAF;}_LLAE: {
# 445
struct Cyc_Core_Failure_exn_struct _tmpE74;const char*_tmpE73;struct Cyc_Core_Failure_exn_struct*_tmpE72;(int)_throw((void*)((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72[0]=((_tmpE74.tag=Cyc_Core_Failure,((_tmpE74.f1=((_tmpE73="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpE73,sizeof(char),45))),_tmpE74)))),_tmpE72)))));}_LLAF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp182->tag != 37)goto _LLB1;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;}}_LLB0:
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpE77;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpE76;new_e=Cyc_Absyn_new_exp((void*)((_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76[0]=((_tmpE77.tag=37,((_tmpE77.f1=Cyc_Tcutil_deep_copy_exp(_tmp105,_tmp183),((_tmpE77.f2=_tmp184,_tmpE77)))))),_tmpE76)))),e->loc);}
goto _LL64;_LLB1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp185=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp185->tag != 38)goto _LLB3;else{_tmp186=(void*)_tmp185->f1;}}_LLB2:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp186),e->loc);
goto _LL64;_LLB3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp106;if(_tmp187->tag != 39)goto _LL64;else{_tmp188=_tmp187->f1;_tmp189=_tmp187->f2;}}_LLB4:
 new_e=Cyc_Absyn_asm_exp(_tmp188,_tmp189,e->loc);goto _LL64;_LL64:;}
# 453
if(preserve_types)new_e->topt=e->topt;
return new_e;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 465 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1BD;enum Cyc_Absyn_AliasQual _tmp1BE;struct Cyc_Absyn_Kind*_tmp1BC=ka1;_tmp1BD=_tmp1BC->kind;_tmp1BE=_tmp1BC->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C0;enum Cyc_Absyn_AliasQual _tmp1C1;struct Cyc_Absyn_Kind*_tmp1BF=ka2;_tmp1C0=_tmp1BF->kind;_tmp1C1=_tmp1BF->aliasqual;
# 469
if(_tmp1BD != _tmp1C0){
struct _tuple20 _tmpE78;struct _tuple20 _stmttmp4=(_tmpE78.f1=_tmp1BD,((_tmpE78.f2=_tmp1C0,_tmpE78)));struct _tuple20 _tmp1C2=_stmttmp4;_LLBB: if(_tmp1C2.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1C2.f2 != Cyc_Absyn_MemKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1C2.f1 != Cyc_Absyn_BoxKind)goto _LLBF;if(_tmp1C2.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLC0;_LLBF: if(_tmp1C2.f1 != Cyc_Absyn_MemKind)goto _LLC1;if(_tmp1C2.f2 != Cyc_Absyn_AnyKind)goto _LLC1;_LLC0:
 goto _LLBA;_LLC1:;_LLC2:
 return 0;_LLBA:;}
# 478
if(_tmp1BE != _tmp1C1){
struct _tuple21 _tmpE79;struct _tuple21 _stmttmp5=(_tmpE79.f1=_tmp1BE,((_tmpE79.f2=_tmp1C1,_tmpE79)));struct _tuple21 _tmp1C4=_stmttmp5;_LLC4: if(_tmp1C4.f1 != Cyc_Absyn_Aliasable)goto _LLC6;if(_tmp1C4.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 goto _LLC7;_LLC6: if(_tmp1C4.f1 != Cyc_Absyn_Unique)goto _LLC8;if(_tmp1C4.f2 != Cyc_Absyn_Top)goto _LLC8;_LLC7:
 return 1;_LLC8:;_LLC9:
 return 0;_LLC3:;}
# 485
return 1;};}
# 488
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_stmttmp6=Cyc_Absyn_compress_kb(tv->kind);void*_tmp1C6=_stmttmp6;struct Cyc_Absyn_Kind*_tmp1C8;struct Cyc_Absyn_Kind*_tmp1CA;_LLCB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1C7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1C6;if(_tmp1C7->tag != 0)goto _LLCD;else{_tmp1C8=_tmp1C7->f1;}}_LLCC:
 return _tmp1C8;_LLCD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1C6;if(_tmp1C9->tag != 2)goto _LLCF;else{_tmp1CA=_tmp1C9->f2;}}_LLCE:
 return _tmp1CA;_LLCF:;_LLD0:
# 493
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE7C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE7B;tv->kind=(void*)((_tmpE7B=_cycalloc(sizeof(*_tmpE7B)),((_tmpE7B[0]=((_tmpE7C.tag=2,((_tmpE7C.f1=0,((_tmpE7C.f2=def,_tmpE7C)))))),_tmpE7B))));}
return def;_LLCA:;}
# 498
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpE7D;struct _tuple0 _stmttmp7=(_tmpE7D.f1=Cyc_Absyn_compress_kb(kb1),((_tmpE7D.f2=Cyc_Absyn_compress_kb(kb2),_tmpE7D)));struct _tuple0 _tmp1CD=_stmttmp7;struct Cyc_Absyn_Kind*_tmp1CF;struct Cyc_Absyn_Kind*_tmp1D1;struct Cyc_Core_Opt**_tmp1D3;struct Cyc_Absyn_Kind*_tmp1D4;struct Cyc_Absyn_Kind*_tmp1D6;struct Cyc_Absyn_Kind*_tmp1D8;struct Cyc_Core_Opt**_tmp1DA;struct Cyc_Absyn_Kind*_tmp1DB;struct Cyc_Core_Opt**_tmp1DD;struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Core_Opt**_tmp1E0;struct Cyc_Absyn_Kind*_tmp1E1;struct Cyc_Core_Opt**_tmp1E3;void*_tmp1E4;void*_tmp1E5;struct Cyc_Core_Opt**_tmp1E7;_LLD2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1CE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD.f1;if(_tmp1CE->tag != 0)goto _LLD4;else{_tmp1CF=_tmp1CE->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD.f2;if(_tmp1D0->tag != 0)goto _LLD4;else{_tmp1D1=_tmp1D0->f1;}};_LLD3:
 return _tmp1CF == _tmp1D1;_LLD4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD.f1;if(_tmp1D2->tag != 2)goto _LLD6;else{_tmp1D3=(struct Cyc_Core_Opt**)& _tmp1D2->f1;_tmp1D4=_tmp1D2->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D5=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD.f2;if(_tmp1D5->tag != 0)goto _LLD6;else{_tmp1D6=_tmp1D5->f1;}};_LLD5:
# 502
 if(Cyc_Tcutil_kind_leq(_tmp1D6,_tmp1D4)){
{struct Cyc_Core_Opt*_tmpE7E;*_tmp1D3=((_tmpE7E=_cycalloc(sizeof(*_tmpE7E)),((_tmpE7E->v=kb2,_tmpE7E))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CD.f1;if(_tmp1D7->tag != 0)goto _LLD8;else{_tmp1D8=_tmp1D7->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD.f2;if(_tmp1D9->tag != 2)goto _LLD8;else{_tmp1DA=(struct Cyc_Core_Opt**)& _tmp1D9->f1;_tmp1DB=_tmp1D9->f2;}};_LLD7:
# 507
 if(Cyc_Tcutil_kind_leq(_tmp1D8,_tmp1DB)){
{struct Cyc_Core_Opt*_tmpE7F;*_tmp1DA=((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F->v=kb1,_tmpE7F))));}
return 1;}else{
return 0;}_LLD8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD.f1;if(_tmp1DC->tag != 2)goto _LLDA;else{_tmp1DD=(struct Cyc_Core_Opt**)& _tmp1DC->f1;_tmp1DE=_tmp1DC->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CD.f2;if(_tmp1DF->tag != 2)goto _LLDA;else{_tmp1E0=(struct Cyc_Core_Opt**)& _tmp1DF->f1;_tmp1E1=_tmp1DF->f2;}};_LLD9:
# 512
 if(Cyc_Tcutil_kind_leq(_tmp1DE,_tmp1E1)){
{struct Cyc_Core_Opt*_tmpE80;*_tmp1E0=((_tmpE80=_cycalloc(sizeof(*_tmpE80)),((_tmpE80->v=kb1,_tmpE80))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1E1,_tmp1DE)){
{struct Cyc_Core_Opt*_tmpE81;*_tmp1DD=((_tmpE81=_cycalloc(sizeof(*_tmpE81)),((_tmpE81->v=kb2,_tmpE81))));}
return 1;}else{
return 0;}}_LLDA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E2=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1CD.f1;if(_tmp1E2->tag != 1)goto _LLDC;else{_tmp1E3=(struct Cyc_Core_Opt**)& _tmp1E2->f1;}}_tmp1E4=_tmp1CD.f2;_LLDB:
 _tmp1E5=_tmp1E4;_tmp1E7=_tmp1E3;goto _LLDD;_LLDC: _tmp1E5=_tmp1CD.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1E6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1CD.f2;if(_tmp1E6->tag != 1)goto _LLD1;else{_tmp1E7=(struct Cyc_Core_Opt**)& _tmp1E6->f1;}};_LLDD:
# 521
{struct Cyc_Core_Opt*_tmpE82;*_tmp1E7=((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82->v=_tmp1E5,_tmpE82))));}
return 1;_LLD1:;}
# 526
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_stmttmp8=Cyc_Tcutil_compress(t);void*_tmp1EE=_stmttmp8;struct Cyc_Absyn_Tvar*_tmp1F0;_LLDF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1EF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EE;if(_tmp1EF->tag != 2)goto _LLE1;else{_tmp1F0=_tmp1EF->f1;}}_LLE0: {
# 529
void*_tmp1F1=_tmp1F0->kind;
_tmp1F0->kind=kb;{
struct _tuple16 _tmpE83;return(_tmpE83.f1=_tmp1F0,((_tmpE83.f2=_tmp1F1,_tmpE83)));};}_LLE1:;_LLE2: {
# 533
const char*_tmpE87;void*_tmpE86[1];struct Cyc_String_pa_PrintArg_struct _tmpE85;(_tmpE85.tag=0,((_tmpE85.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE86[0]=& _tmpE85,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE87="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpE87,sizeof(char),40))),_tag_dyneither(_tmpE86,sizeof(void*),1)))))));}_LLDE:;}
# 539
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 541
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 545
struct _RegionHandle _tmp1F6=_new_region("temp");struct _RegionHandle*temp=& _tmp1F6;_push_region(temp);
{struct Cyc_List_List*_tmp1F7=0;
# 548
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp1F8=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp1F9=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_stmttmp9=Cyc_Tcutil_tvar_kind(_tmp1F8,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1FA=_stmttmp9;_LLE4: if(_tmp1FA->kind != Cyc_Absyn_IntKind)goto _LLE6;_LLE5:
 goto _LLE7;_LLE6: if(_tmp1FA->kind != Cyc_Absyn_AnyKind)goto _LLE8;_LLE7:
# 554
{struct _tuple16*_tmpE8A;struct Cyc_List_List*_tmpE89;_tmp1F7=((_tmpE89=_region_malloc(temp,sizeof(*_tmpE89)),((_tmpE89->hd=((_tmpE8A=_region_malloc(temp,sizeof(*_tmpE8A)),((_tmpE8A->f1=_tmp1F8,((_tmpE8A->f2=_tmp1F9,_tmpE8A)))))),((_tmpE89->tl=_tmp1F7,_tmpE89))))));}goto _LLE3;_LLE8:;_LLE9:
 goto _LLE3;_LLE3:;}
# 558
if(_tmp1F7 != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1F7),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 546
;_pop_region(temp);}
# 563
return k;}
# 570
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_stmttmpA=Cyc_Tcutil_compress(t);void*_tmp1FD=_stmttmpA;struct Cyc_Core_Opt*_tmp1FF;struct Cyc_Absyn_Tvar*_tmp201;enum Cyc_Absyn_Size_of _tmp204;struct Cyc_Absyn_Datatypedecl*_tmp210;struct Cyc_Absyn_Datatypefield*_tmp211;enum Cyc_Absyn_AggrKind _tmp215;struct Cyc_List_List*_tmp216;struct Cyc_Absyn_AggrdeclImpl*_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_Absyn_PtrInfo _tmp21B;struct Cyc_Absyn_Exp*_tmp21F;struct Cyc_Absyn_Typedefdecl*_tmp222;_LLEB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1FE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1FD;if(_tmp1FE->tag != 1)goto _LLED;else{_tmp1FF=_tmp1FE->f1;}}_LLEC:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1FF))->v;_LLED: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp200=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1FD;if(_tmp200->tag != 2)goto _LLEF;else{_tmp201=_tmp200->f1;}}_LLEE:
 return Cyc_Tcutil_tvar_kind(_tmp201,& Cyc_Tcutil_bk);_LLEF: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1FD;if(_tmp202->tag != 0)goto _LLF1;}_LLF0:
 return& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp203=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1FD;if(_tmp203->tag != 6)goto _LLF3;else{_tmp204=_tmp203->f2;}}_LLF2:
# 576
 return(_tmp204 == Cyc_Absyn_Int_sz  || _tmp204 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp205=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1FD;if(_tmp205->tag != 7)goto _LLF5;}_LLF4:
# 578
 goto _LLF6;_LLF5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp206=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FD;if(_tmp206->tag != 9)goto _LLF7;}_LLF6:
 return& Cyc_Tcutil_mk;_LLF7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp207=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1FD;if(_tmp207->tag != 16)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp208=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1FD;if(_tmp208->tag != 13)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp209=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1FD;if(_tmp209->tag != 14)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp20A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1FD;if(_tmp20A->tag != 15)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_bk;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1FD;if(_tmp20B->tag != 21)goto _LL101;}_LL100:
 return& Cyc_Tcutil_urk;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp20C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1FD;if(_tmp20C->tag != 20)goto _LL103;}_LL102:
 return& Cyc_Tcutil_rk;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1FD;if(_tmp20D->tag != 22)goto _LL105;}_LL104:
 return& Cyc_Tcutil_trk;_LL105: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp20E=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1FD;if(_tmp20E->tag != 3)goto _LL107;}_LL106:
# 589
 return& Cyc_Tcutil_ak;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1FD;if(_tmp20F->tag != 4)goto _LL109;else{if((((_tmp20F->f1).field_info).KnownDatatypefield).tag != 2)goto _LL109;_tmp210=((struct _tuple3)(((_tmp20F->f1).field_info).KnownDatatypefield).val).f1;_tmp211=((struct _tuple3)(((_tmp20F->f1).field_info).KnownDatatypefield).val).f2;}}_LL108:
# 591
 return& Cyc_Tcutil_mk;_LL109: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp212=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1FD;if(_tmp212->tag != 4)goto _LL10B;else{if((((_tmp212->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL10B;}}_LL10A: {
# 593
const char*_tmpE8D;void*_tmpE8C;(_tmpE8C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE8D="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpE8D,sizeof(char),39))),_tag_dyneither(_tmpE8C,sizeof(void*),0)));}_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1FD;if(_tmp213->tag != 11)goto _LL10D;else{if((((_tmp213->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10D;}}_LL10C:
# 596
 return& Cyc_Tcutil_ak;_LL10D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1FD;if(_tmp214->tag != 11)goto _LL10F;else{if((((_tmp214->f1).aggr_info).KnownAggr).tag != 2)goto _LL10F;_tmp215=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp214->f1).aggr_info).KnownAggr).val))->kind;_tmp216=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp214->f1).aggr_info).KnownAggr).val))->tvs;_tmp217=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp214->f1).aggr_info).KnownAggr).val))->impl;_tmp218=(_tmp214->f1).targs;}}_LL10E:
# 598
 if(_tmp217 == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp22E=_tmp217->fields;
if(_tmp22E == 0)return& Cyc_Tcutil_mk;
# 602
if(_tmp215 == Cyc_Absyn_StructA){
for(0;_tmp22E->tl != 0;_tmp22E=_tmp22E->tl){;}{
void*_tmp22F=((struct Cyc_Absyn_Aggrfield*)_tmp22E->hd)->type;
struct Cyc_Absyn_Kind*_tmp230=Cyc_Tcutil_field_kind(_tmp22F,_tmp218,_tmp216);
if(_tmp230 == & Cyc_Tcutil_ak  || _tmp230 == & Cyc_Tcutil_tak)return _tmp230;};}else{
# 610
for(0;_tmp22E != 0;_tmp22E=_tmp22E->tl){
void*_tmp231=((struct Cyc_Absyn_Aggrfield*)_tmp22E->hd)->type;
struct Cyc_Absyn_Kind*_tmp232=Cyc_Tcutil_field_kind(_tmp231,_tmp218,_tmp216);
if(_tmp232 == & Cyc_Tcutil_ak  || _tmp232 == & Cyc_Tcutil_tak)return _tmp232;}}
# 616
return& Cyc_Tcutil_mk;};_LL10F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1FD;if(_tmp219->tag != 12)goto _LL111;}_LL110:
 return& Cyc_Tcutil_mk;_LL111: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FD;if(_tmp21A->tag != 5)goto _LL113;else{_tmp21B=_tmp21A->f1;}}_LL112: {
# 619
void*_stmttmpB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp21B.ptr_atts).bounds);void*_tmp233=_stmttmpB;_LL12A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp234=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp233;if(_tmp234->tag != 0)goto _LL12C;}_LL12B: {
# 621
enum Cyc_Absyn_AliasQual _stmttmpD=(Cyc_Tcutil_typ_kind((_tmp21B.ptr_atts).rgn))->aliasqual;switch(_stmttmpD){case Cyc_Absyn_Aliasable: _LL12E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12F:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL130:
 return& Cyc_Tcutil_tmk;}}_LL12C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp235=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp233;if(_tmp235->tag != 1)goto _LL129;}_LL12D: {
# 627
enum Cyc_Absyn_AliasQual _stmttmpC=(Cyc_Tcutil_typ_kind((_tmp21B.ptr_atts).rgn))->aliasqual;switch(_stmttmpC){case Cyc_Absyn_Aliasable: _LL132:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL133:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL134:
 return& Cyc_Tcutil_tbk;}}_LL129:;}_LL113: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1FD;if(_tmp21C->tag != 18)goto _LL115;}_LL114:
# 633
 return& Cyc_Tcutil_ik;_LL115: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1FD;if(_tmp21D->tag != 19)goto _LL117;}_LL116:
 return& Cyc_Tcutil_bk;_LL117: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1FD;if(_tmp21E->tag != 8)goto _LL119;else{_tmp21F=(_tmp21E->f1).num_elts;}}_LL118:
# 636
 if(_tmp21F == 0  || Cyc_Tcutil_is_const_exp(_tmp21F))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL119: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1FD;if(_tmp220->tag != 10)goto _LL11B;}_LL11A:
 return& Cyc_Tcutil_mk;_LL11B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1FD;if(_tmp221->tag != 17)goto _LL11D;else{_tmp222=_tmp221->f3;}}_LL11C:
# 640
 if(_tmp222 == 0  || _tmp222->kind == 0){
const char*_tmpE91;void*_tmpE90[1];struct Cyc_String_pa_PrintArg_struct _tmpE8F;(_tmpE8F.tag=0,((_tmpE8F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpE90[0]=& _tmpE8F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE91="typ_kind: typedef found: %s",_tag_dyneither(_tmpE91,sizeof(char),28))),_tag_dyneither(_tmpE90,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp222->kind))->v;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FD;if(_tmp223->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp224=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp223->f1)->r;if(_tmp224->tag != 0)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FD;if(_tmp225->tag != 26)goto _LL121;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp226=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp225->f1)->r;if(_tmp226->tag != 1)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_bk;_LL121: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1FD;if(_tmp227->tag != 26)goto _LL123;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp228=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp227->f1)->r;if(_tmp228->tag != 2)goto _LL123;}}_LL122:
 return& Cyc_Tcutil_ak;_LL123: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1FD;if(_tmp229->tag != 23)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1FD;if(_tmp22A->tag != 24)goto _LL127;}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1FD;if(_tmp22B->tag != 25)goto _LLEA;}_LL128:
 return& Cyc_Tcutil_ek;_LLEA:;}
# 652
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 657
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp239;_push_handler(& _tmp239);{int _tmp23B=0;if(setjmp(_tmp239.handler))_tmp23B=1;if(!_tmp23B){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp23C=1;_npop_handler(0);return _tmp23C;};
# 659
;_pop_handler();}else{void*_tmp23A=(void*)_exn_thrown;void*_tmp23E=_tmp23A;void*_tmp240;_LL137: {struct Cyc_Tcutil_Unify_exn_struct*_tmp23F=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp23E;if(_tmp23F->tag != Cyc_Tcutil_Unify)goto _LL139;}_LL138:
# 661
 return 0;_LL139: _tmp240=_tmp23E;_LL13A:(void)_throw(_tmp240);_LL136:;}};}
# 666
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp241=t;struct Cyc_Absyn_Tvar*_tmp243;void*_tmp245;struct Cyc_Core_Opt**_tmp246;struct Cyc_Absyn_PtrInfo _tmp248;void*_tmp24A;struct Cyc_List_List*_tmp24C;void*_tmp24D;struct Cyc_Absyn_Tqual _tmp24E;void*_tmp24F;struct Cyc_List_List*_tmp250;int _tmp251;struct Cyc_Absyn_VarargInfo*_tmp252;struct Cyc_List_List*_tmp253;struct Cyc_List_List*_tmp254;struct Cyc_List_List*_tmp256;struct Cyc_List_List*_tmp258;struct Cyc_List_List*_tmp25A;struct Cyc_List_List*_tmp25C;struct Cyc_List_List*_tmp25E;struct Cyc_List_List*_tmp260;void*_tmp262;void*_tmp264;void*_tmp266;void*_tmp268;struct Cyc_List_List*_tmp26A;_LL13C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp242=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp241;if(_tmp242->tag != 2)goto _LL13E;else{_tmp243=_tmp242->f1;}}_LL13D:
# 671
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp243)){
{const char*_tmpE92;Cyc_Tcutil_failure_reason=((_tmpE92="(type variable would escape scope)",_tag_dyneither(_tmpE92,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 675
goto _LL13B;_LL13E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp244=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp241;if(_tmp244->tag != 1)goto _LL140;else{_tmp245=(void*)_tmp244->f2;_tmp246=(struct Cyc_Core_Opt**)& _tmp244->f4;}}_LL13F:
# 677
 if(t == evar){
{const char*_tmpE93;Cyc_Tcutil_failure_reason=((_tmpE93="(occurs check)",_tag_dyneither(_tmpE93,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 681
if(_tmp245 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp245);else{
# 684
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp246))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 691
if(problem){
struct Cyc_List_List*_tmp26D=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp246))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpE94;_tmp26D=((_tmpE94=_cycalloc(sizeof(*_tmpE94)),((_tmpE94->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpE94->tl=_tmp26D,_tmpE94))))));}}}{
# 697
struct Cyc_Core_Opt*_tmpE95;*_tmp246=((_tmpE95=_cycalloc(sizeof(*_tmpE95)),((_tmpE95->v=_tmp26D,_tmpE95))));};}}}
# 700
goto _LL13B;_LL140: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp247=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp241;if(_tmp247->tag != 5)goto _LL142;else{_tmp248=_tmp247->f1;}}_LL141:
# 703
 Cyc_Tcutil_occurs(evar,r,env,_tmp248.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp248.ptr_atts).rgn);
goto _LL13B;_LL142: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp249=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp241;if(_tmp249->tag != 8)goto _LL144;else{_tmp24A=(_tmp249->f1).elt_type;}}_LL143:
# 707
 Cyc_Tcutil_occurs(evar,r,env,_tmp24A);goto _LL13B;_LL144: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp24B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp241;if(_tmp24B->tag != 9)goto _LL146;else{_tmp24C=(_tmp24B->f1).tvars;_tmp24D=(_tmp24B->f1).effect;_tmp24E=(_tmp24B->f1).ret_tqual;_tmp24F=(_tmp24B->f1).ret_typ;_tmp250=(_tmp24B->f1).args;_tmp251=(_tmp24B->f1).c_varargs;_tmp252=(_tmp24B->f1).cyc_varargs;_tmp253=(_tmp24B->f1).rgn_po;_tmp254=(_tmp24B->f1).attributes;}}_LL145:
# 709
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp24C,env);
if(_tmp24D != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp24D);
Cyc_Tcutil_occurs(evar,r,env,_tmp24F);
for(0;_tmp250 != 0;_tmp250=_tmp250->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp250->hd)).f3);}
if(_tmp252 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp252->type);
for(0;_tmp253 != 0;_tmp253=_tmp253->tl){
struct _tuple0*_stmttmpE=(struct _tuple0*)_tmp253->hd;void*_tmp271;void*_tmp272;struct _tuple0*_tmp270=_stmttmpE;_tmp271=_tmp270->f1;_tmp272=_tmp270->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp271);
Cyc_Tcutil_occurs(evar,r,env,_tmp272);}
# 721
goto _LL13B;_LL146: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp255=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp241;if(_tmp255->tag != 10)goto _LL148;else{_tmp256=_tmp255->f1;}}_LL147:
# 723
 for(0;_tmp256 != 0;_tmp256=_tmp256->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp256->hd)).f2);}
goto _LL13B;_LL148: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp257=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp241;if(_tmp257->tag != 3)goto _LL14A;else{_tmp258=(_tmp257->f1).targs;}}_LL149:
# 727
 Cyc_Tcutil_occurslist(evar,r,env,_tmp258);goto _LL13B;_LL14A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp259=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp241;if(_tmp259->tag != 17)goto _LL14C;else{_tmp25A=_tmp259->f2;}}_LL14B:
 _tmp25C=_tmp25A;goto _LL14D;_LL14C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp25B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp241;if(_tmp25B->tag != 4)goto _LL14E;else{_tmp25C=(_tmp25B->f1).targs;}}_LL14D:
 _tmp25E=_tmp25C;goto _LL14F;_LL14E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp25D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp241;if(_tmp25D->tag != 11)goto _LL150;else{_tmp25E=(_tmp25D->f1).targs;}}_LL14F:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp25E);goto _LL13B;_LL150: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp25F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp241;if(_tmp25F->tag != 12)goto _LL152;else{_tmp260=_tmp25F->f2;}}_LL151:
# 733
 for(0;_tmp260 != 0;_tmp260=_tmp260->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp260->hd)->type);}
goto _LL13B;_LL152: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp261=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp241;if(_tmp261->tag != 19)goto _LL154;else{_tmp262=(void*)_tmp261->f1;}}_LL153:
 _tmp264=_tmp262;goto _LL155;_LL154: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp263=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp241;if(_tmp263->tag != 23)goto _LL156;else{_tmp264=(void*)_tmp263->f1;}}_LL155:
 _tmp266=_tmp264;goto _LL157;_LL156: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp241;if(_tmp265->tag != 15)goto _LL158;else{_tmp266=(void*)_tmp265->f1;}}_LL157:
 _tmp268=_tmp266;goto _LL159;_LL158: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp267=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp241;if(_tmp267->tag != 25)goto _LL15A;else{_tmp268=(void*)_tmp267->f1;}}_LL159:
 Cyc_Tcutil_occurs(evar,r,env,_tmp268);goto _LL13B;_LL15A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp241;if(_tmp269->tag != 24)goto _LL15C;else{_tmp26A=_tmp269->f1;}}_LL15B:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp26A);goto _LL13B;_LL15C:;_LL15D:
# 742
 goto _LL13B;_LL13B:;};}
# 745
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 747
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 752
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 760
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpE98;void*_tmpE97;(_tmpE97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE98="tq1 real_const not set.",_tag_dyneither(_tmpE98,sizeof(char),24))),_tag_dyneither(_tmpE97,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpE9B;void*_tmpE9A;(_tmpE9A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE9B="tq2 real_const not set.",_tag_dyneither(_tmpE9B,sizeof(char),24))),_tag_dyneither(_tmpE9A,sizeof(void*),0)));}
# 766
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 769
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpE9C;Cyc_Tcutil_failure_reason=((_tmpE9C="(qualifiers don't match)",_tag_dyneither(_tmpE9C,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 777
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 781
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 788
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 790
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp278=x;void*_tmp279;_LL15F: if((_tmp278->No_constr).tag != 3)goto _LL161;_LL160:
# 795
{union Cyc_Absyn_Constraint _tmpE9D;*x=(((_tmpE9D.Forward_constr).val=y,(((_tmpE9D.Forward_constr).tag=2,_tmpE9D))));}return;_LL161: if((_tmp278->Eq_constr).tag != 1)goto _LL163;_tmp279=(void*)(_tmp278->Eq_constr).val;_LL162: {
# 797
union Cyc_Absyn_Constraint*_tmp27B=y;void*_tmp27C;_LL166: if((_tmp27B->No_constr).tag != 3)goto _LL168;_LL167:
*y=*x;return;_LL168: if((_tmp27B->Eq_constr).tag != 1)goto _LL16A;_tmp27C=(void*)(_tmp27B->Eq_constr).val;_LL169:
# 800
 if(cmp(_tmp279,_tmp27C)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 804
return;_LL16A: if((_tmp27B->Forward_constr).tag != 2)goto _LL165;_LL16B: {
const char*_tmpEA0;void*_tmpE9F;(_tmpE9F=0,Cyc_Tcutil_impos(((_tmpEA0="unify_conref: forward after compress(2)",_tag_dyneither(_tmpEA0,sizeof(char),40))),_tag_dyneither(_tmpE9F,sizeof(void*),0)));}_LL165:;}_LL163: if((_tmp278->Forward_constr).tag != 2)goto _LL15E;_LL164: {
# 807
const char*_tmpEA3;void*_tmpEA2;(_tmpEA2=0,Cyc_Tcutil_impos(((_tmpEA3="unify_conref: forward after compress",_tag_dyneither(_tmpEA3,sizeof(char),37))),_tag_dyneither(_tmpEA2,sizeof(void*),0)));}_LL15E:;};}
# 811
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp281;_push_handler(& _tmp281);{int _tmp283=0;if(setjmp(_tmp281.handler))_tmp283=1;if(!_tmp283){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp284=1;_npop_handler(0);return _tmp284;};
# 813
;_pop_handler();}else{void*_tmp282=(void*)_exn_thrown;void*_tmp286=_tmp282;void*_tmp288;_LL16D: {struct Cyc_Tcutil_Unify_exn_struct*_tmp287=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp286;if(_tmp287->tag != Cyc_Tcutil_Unify)goto _LL16F;}_LL16E:
# 815
 return 0;_LL16F: _tmp288=_tmp286;_LL170:(void)_throw(_tmp288);_LL16C:;}};}
# 818
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpEA4;struct _tuple0 _stmttmpF=(_tmpEA4.f1=b1,((_tmpEA4.f2=b2,_tmpEA4)));struct _tuple0 _tmp289=_stmttmpF;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp291;_LL172:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28A->tag != 0)goto _LL174;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp28B->tag != 0)goto _LL174;};_LL173:
 return 0;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28C->tag != 0)goto _LL176;}_LL175:
 return - 1;_LL176: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp28D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp28D->tag != 0)goto _LL178;}_LL177:
 return 1;_LL178:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp28E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp289.f1;if(_tmp28E->tag != 1)goto _LL171;else{_tmp28F=_tmp28E->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp290=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp289.f2;if(_tmp290->tag != 1)goto _LL171;else{_tmp291=_tmp290->f1;}};_LL179:
# 824
 return Cyc_Evexp_const_exp_cmp(_tmp28F,_tmp291);_LL171:;}
# 828
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpEA5;struct _tuple0 _stmttmp10=(_tmpEA5.f1=b1,((_tmpEA5.f2=b2,_tmpEA5)));struct _tuple0 _tmp293=_stmttmp10;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp29B;_LL17B:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp294=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp294->tag != 0)goto _LL17D;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp295=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp295->tag != 0)goto _LL17D;};_LL17C:
 return 0;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp296=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp296->tag != 0)goto _LL17F;}_LL17E:
 return - 1;_LL17F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp297=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp297->tag != 0)goto _LL181;}_LL180:
 return 1;_LL181:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp298=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp293.f1;if(_tmp298->tag != 1)goto _LL17A;else{_tmp299=_tmp298->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp29A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp293.f2;if(_tmp29A->tag != 1)goto _LL17A;else{_tmp29B=_tmp29A->f1;}};_LL182:
 return Cyc_Evexp_const_exp_cmp(_tmp299,_tmp29B);_LL17A:;}
# 837
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp29D=att;_LL184: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp29E=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp29E->tag != 0)goto _LL186;}_LL185:
 return 0;_LL186: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp29F=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp29F->tag != 1)goto _LL188;}_LL187:
 return 1;_LL188: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2A0=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A0->tag != 2)goto _LL18A;}_LL189:
 return 2;_LL18A: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2A1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A1->tag != 3)goto _LL18C;}_LL18B:
 return 3;_LL18C: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2A2=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A2->tag != 4)goto _LL18E;}_LL18D:
 return 4;_LL18E: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2A3=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A3->tag != 5)goto _LL190;}_LL18F:
 return 5;_LL190: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2A4=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A4->tag != 6)goto _LL192;}_LL191:
 return 6;_LL192: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2A5=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A5->tag != 7)goto _LL194;}_LL193:
 return 7;_LL194: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2A6=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A6->tag != 8)goto _LL196;}_LL195:
 return 8;_LL196: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2A7=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A7->tag != 9)goto _LL198;}_LL197:
 return 9;_LL198: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2A8=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A8->tag != 10)goto _LL19A;}_LL199:
 return 10;_LL19A: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2A9=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2A9->tag != 11)goto _LL19C;}_LL19B:
 return 11;_LL19C: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2AA=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AA->tag != 12)goto _LL19E;}_LL19D:
 return 12;_LL19E: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2AB=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AB->tag != 13)goto _LL1A0;}_LL19F:
 return 13;_LL1A0: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2AC=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AC->tag != 14)goto _LL1A2;}_LL1A1:
 return 14;_LL1A2: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2AD=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AD->tag != 15)goto _LL1A4;}_LL1A3:
 return 15;_LL1A4: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2AE=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AE->tag != 16)goto _LL1A6;}_LL1A5:
 return 16;_LL1A6: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2AF=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2AF->tag != 17)goto _LL1A8;}_LL1A7:
 return 17;_LL1A8: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2B0=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2B0->tag != 18)goto _LL1AA;}_LL1A9:
 return 18;_LL1AA: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2B1->tag != 19)goto _LL1AC;}_LL1AB:
 return 19;_LL1AC: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B2=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp29D;if(_tmp2B2->tag != 20)goto _LL1AE;}_LL1AD:
 return 20;_LL1AE:;_LL1AF:
 return 21;_LL183:;}
# 864
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpEA6;struct _tuple0 _stmttmp11=(_tmpEA6.f1=att1,((_tmpEA6.f2=att2,_tmpEA6)));struct _tuple0 _tmp2B3=_stmttmp11;int _tmp2B5;int _tmp2B7;int _tmp2B9;int _tmp2BB;int _tmp2BD;int _tmp2BF;struct _dyneither_ptr _tmp2C1;struct _dyneither_ptr _tmp2C3;enum Cyc_Absyn_Format_Type _tmp2C5;int _tmp2C6;int _tmp2C7;enum Cyc_Absyn_Format_Type _tmp2C9;int _tmp2CA;int _tmp2CB;_LL1B1:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B4=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2B4->tag != 0)goto _LL1B3;else{_tmp2B5=_tmp2B4->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B6=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2B6->tag != 0)goto _LL1B3;else{_tmp2B7=_tmp2B6->f1;}};_LL1B2:
 _tmp2B9=_tmp2B5;_tmp2BB=_tmp2B7;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2B8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2B8->tag != 20)goto _LL1B5;else{_tmp2B9=_tmp2B8->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2BA=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2BA->tag != 20)goto _LL1B5;else{_tmp2BB=_tmp2BA->f1;}};_LL1B4:
 _tmp2BD=_tmp2B9;_tmp2BF=_tmp2BB;goto _LL1B6;_LL1B5:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2BC->tag != 6)goto _LL1B7;else{_tmp2BD=_tmp2BC->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2BE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2BE->tag != 6)goto _LL1B7;else{_tmp2BF=_tmp2BE->f1;}};_LL1B6:
 return Cyc_Core_intcmp(_tmp2BD,_tmp2BF);_LL1B7:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C0=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2C0->tag != 8)goto _LL1B9;else{_tmp2C1=_tmp2C0->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2C2=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2C2->tag != 8)goto _LL1B9;else{_tmp2C3=_tmp2C2->f1;}};_LL1B8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2C1,(struct _dyneither_ptr)_tmp2C3);_LL1B9:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C4=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B3.f1;if(_tmp2C4->tag != 19)goto _LL1BB;else{_tmp2C5=_tmp2C4->f1;_tmp2C6=_tmp2C4->f2;_tmp2C7=_tmp2C4->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C8=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B3.f2;if(_tmp2C8->tag != 19)goto _LL1BB;else{_tmp2C9=_tmp2C8->f1;_tmp2CA=_tmp2C8->f2;_tmp2CB=_tmp2C8->f3;}};_LL1BA: {
# 871
int _tmp2CC=Cyc_Core_intcmp((int)((unsigned int)_tmp2C5),(int)((unsigned int)_tmp2C9));
if(_tmp2CC != 0)return _tmp2CC;{
int _tmp2CD=Cyc_Core_intcmp(_tmp2C6,_tmp2CA);
if(_tmp2CD != 0)return _tmp2CD;
return Cyc_Core_intcmp(_tmp2C7,_tmp2CB);};}_LL1BB:;_LL1BC:
# 877
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1B0:;}
# 881
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 885
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 894
static void*Cyc_Tcutil_rgns_of(void*t);
# 896
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 900
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_stmttmp12=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp2CF=_stmttmp12;_LL1BE: if(_tmp2CF->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2CF->aliasqual != Cyc_Absyn_Unique)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BD;_LL1C0: if(_tmp2CF->kind != Cyc_Absyn_RgnKind)goto _LL1C2;if(_tmp2CF->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C2;_LL1C1:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BD;_LL1C2: if(_tmp2CF->kind != Cyc_Absyn_EffKind)goto _LL1C4;_LL1C3:
 t=Cyc_Absyn_empty_effect;goto _LL1BD;_LL1C4: if(_tmp2CF->kind != Cyc_Absyn_IntKind)goto _LL1C6;_LL1C5:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpEA9;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpEA8;t=(void*)((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((_tmpEA8[0]=((_tmpEA9.tag=18,((_tmpEA9.f1=Cyc_Absyn_uint_exp(0,0),_tmpEA9)))),_tmpEA8))));}goto _LL1BD;_LL1C6:;_LL1C7:
 t=Cyc_Absyn_sint_typ;goto _LL1BD;_LL1BD:;}{
# 909
struct _tuple16*_tmpEAA;return(_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA->f1=tv,((_tmpEAA->f2=t,_tmpEAA)))));};}
# 916
static void*Cyc_Tcutil_rgns_of(void*t){
void*_stmttmp13=Cyc_Tcutil_compress(t);void*_tmp2D3=_stmttmp13;void*_tmp2DD;void*_tmp2DF;void*_tmp2E0;struct Cyc_List_List*_tmp2E2;void*_tmp2E4;void*_tmp2E5;void*_tmp2E7;struct Cyc_List_List*_tmp2E9;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*_tmp2ED;struct Cyc_List_List*_tmp2EF;struct Cyc_List_List*_tmp2F2;void*_tmp2F3;struct Cyc_Absyn_Tqual _tmp2F4;void*_tmp2F5;struct Cyc_List_List*_tmp2F6;struct Cyc_Absyn_VarargInfo*_tmp2F7;struct Cyc_List_List*_tmp2F8;void*_tmp2FF;struct Cyc_List_List*_tmp301;_LL1C9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2D4=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D4->tag != 0)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2D5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D5->tag != 7)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2D6=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D6->tag != 13)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2D7=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D7->tag != 14)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2D8=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D8->tag != 18)goto _LL1D3;}_LL1D2:
 goto _LL1D4;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2D9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2D3;if(_tmp2D9->tag != 6)goto _LL1D5;}_LL1D4:
 return Cyc_Absyn_empty_effect;_LL1D5: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2D3;if(_tmp2DA->tag != 1)goto _LL1D7;}_LL1D6:
 goto _LL1D8;_LL1D7: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D3;if(_tmp2DB->tag != 2)goto _LL1D9;}_LL1D8: {
# 926
struct Cyc_Absyn_Kind*_stmttmp14=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp303=_stmttmp14;_LL200: if(_tmp303->kind != Cyc_Absyn_RgnKind)goto _LL202;_LL201: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEAD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEAC;return(void*)((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC[0]=((_tmpEAD.tag=23,((_tmpEAD.f1=t,_tmpEAD)))),_tmpEAC))));}_LL202: if(_tmp303->kind != Cyc_Absyn_EffKind)goto _LL204;_LL203:
 return t;_LL204: if(_tmp303->kind != Cyc_Absyn_IntKind)goto _LL206;_LL205:
 return Cyc_Absyn_empty_effect;_LL206:;_LL207: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpEB0;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpEAF;return(void*)((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF[0]=((_tmpEB0.tag=25,((_tmpEB0.f1=t,_tmpEB0)))),_tmpEAF))));}_LL1FF:;}_LL1D9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2DC=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2D3;if(_tmp2DC->tag != 15)goto _LL1DB;else{_tmp2DD=(void*)_tmp2DC->f1;}}_LL1DA: {
# 932
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEB3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEB2;return(void*)((_tmpEB2=_cycalloc(sizeof(*_tmpEB2)),((_tmpEB2[0]=((_tmpEB3.tag=23,((_tmpEB3.f1=_tmp2DD,_tmpEB3)))),_tmpEB2))));}_LL1DB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2DE=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2D3;if(_tmp2DE->tag != 16)goto _LL1DD;else{_tmp2DF=(void*)_tmp2DE->f1;_tmp2E0=(void*)_tmp2DE->f2;}}_LL1DC: {
# 936
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEB6;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEB5;return(void*)((_tmpEB5=_cycalloc(sizeof(*_tmpEB5)),((_tmpEB5[0]=((_tmpEB6.tag=23,((_tmpEB6.f1=_tmp2E0,_tmpEB6)))),_tmpEB5))));}_LL1DD: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2E1=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2D3;if(_tmp2E1->tag != 3)goto _LL1DF;else{_tmp2E2=(_tmp2E1->f1).targs;}}_LL1DE: {
# 938
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2E2);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEB9;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEB8;return Cyc_Tcutil_normalize_effect((void*)((_tmpEB8=_cycalloc(sizeof(*_tmpEB8)),((_tmpEB8[0]=((_tmpEB9.tag=24,((_tmpEB9.f1=ts,_tmpEB9)))),_tmpEB8)))));}_LL1DF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3;if(_tmp2E3->tag != 5)goto _LL1E1;else{_tmp2E4=(_tmp2E3->f1).elt_typ;_tmp2E5=((_tmp2E3->f1).ptr_atts).rgn;}}_LL1E0: {
# 942
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEC8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEC7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEC6;void*_tmpEC5[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEC4;return Cyc_Tcutil_normalize_effect((void*)((_tmpEC4=_cycalloc(sizeof(*_tmpEC4)),((_tmpEC4[0]=((_tmpEC8.tag=24,((_tmpEC8.f1=((_tmpEC5[1]=Cyc_Tcutil_rgns_of(_tmp2E4),((_tmpEC5[0]=(void*)((_tmpEC7=_cycalloc(sizeof(*_tmpEC7)),((_tmpEC7[0]=((_tmpEC6.tag=23,((_tmpEC6.f1=_tmp2E5,_tmpEC6)))),_tmpEC7)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEC5,sizeof(void*),2)))))),_tmpEC8)))),_tmpEC4)))));}_LL1E1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2E6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3;if(_tmp2E6->tag != 8)goto _LL1E3;else{_tmp2E7=(_tmp2E6->f1).elt_type;}}_LL1E2:
# 944
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2E7));_LL1E3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2E8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D3;if(_tmp2E8->tag != 10)goto _LL1E5;else{_tmp2E9=_tmp2E8->f1;}}_LL1E4: {
# 946
struct Cyc_List_List*_tmp313=0;
for(0;_tmp2E9 != 0;_tmp2E9=_tmp2E9->tl){
struct Cyc_List_List*_tmpEC9;_tmp313=((_tmpEC9=_cycalloc(sizeof(*_tmpEC9)),((_tmpEC9->hd=(*((struct _tuple12*)_tmp2E9->hd)).f2,((_tmpEC9->tl=_tmp313,_tmpEC9))))));}
_tmp2EB=_tmp313;goto _LL1E6;}_LL1E5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp2EA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2D3;if(_tmp2EA->tag != 4)goto _LL1E7;else{_tmp2EB=(_tmp2EA->f1).targs;}}_LL1E6:
 _tmp2ED=_tmp2EB;goto _LL1E8;_LL1E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp2EC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D3;if(_tmp2EC->tag != 11)goto _LL1E9;else{_tmp2ED=(_tmp2EC->f1).targs;}}_LL1E8: {
# 952
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpECC;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpECB;return Cyc_Tcutil_normalize_effect((void*)((_tmpECB=_cycalloc(sizeof(*_tmpECB)),((_tmpECB[0]=((_tmpECC.tag=24,((_tmpECC.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2ED),_tmpECC)))),_tmpECB)))));}_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp2EE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3;if(_tmp2EE->tag != 12)goto _LL1EB;else{_tmp2EF=_tmp2EE->f2;}}_LL1EA: {
# 954
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpECF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpECE;return Cyc_Tcutil_normalize_effect((void*)((_tmpECE=_cycalloc(sizeof(*_tmpECE)),((_tmpECE[0]=((_tmpECF.tag=24,((_tmpECF.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp2EF),_tmpECF)))),_tmpECE)))));}_LL1EB: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2D3;if(_tmp2F0->tag != 19)goto _LL1ED;}_LL1EC:
 return Cyc_Absyn_empty_effect;_LL1ED: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3;if(_tmp2F1->tag != 9)goto _LL1EF;else{_tmp2F2=(_tmp2F1->f1).tvars;_tmp2F3=(_tmp2F1->f1).effect;_tmp2F4=(_tmp2F1->f1).ret_tqual;_tmp2F5=(_tmp2F1->f1).ret_typ;_tmp2F6=(_tmp2F1->f1).args;_tmp2F7=(_tmp2F1->f1).cyc_varargs;_tmp2F8=(_tmp2F1->f1).rgn_po;}}_LL1EE: {
# 964
void*_tmp319=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp2F2),(void*)_check_null(_tmp2F3));
return Cyc_Tcutil_normalize_effect(_tmp319);}_LL1EF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2F9=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2D3;if(_tmp2F9->tag != 21)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2D3;if(_tmp2FA->tag != 22)goto _LL1F3;}_LL1F2:
 goto _LL1F4;_LL1F3: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2D3;if(_tmp2FB->tag != 20)goto _LL1F5;}_LL1F4:
 return Cyc_Absyn_empty_effect;_LL1F5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2D3;if(_tmp2FC->tag != 23)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2FD=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2D3;if(_tmp2FD->tag != 24)goto _LL1F9;}_LL1F8:
 return t;_LL1F9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2FE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2D3;if(_tmp2FE->tag != 25)goto _LL1FB;else{_tmp2FF=(void*)_tmp2FE->f1;}}_LL1FA:
 return Cyc_Tcutil_rgns_of(_tmp2FF);_LL1FB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3;if(_tmp300->tag != 17)goto _LL1FD;else{_tmp301=_tmp300->f2;}}_LL1FC: {
# 973
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpED2;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpED1;return Cyc_Tcutil_normalize_effect((void*)((_tmpED1=_cycalloc(sizeof(*_tmpED1)),((_tmpED1[0]=((_tmpED2.tag=24,((_tmpED2.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp301),_tmpED2)))),_tmpED1)))));}_LL1FD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2D3;if(_tmp302->tag != 26)goto _LL1C8;}_LL1FE: {
const char*_tmpED5;void*_tmpED4;(_tmpED4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpED5="typedecl in rgns_of",_tag_dyneither(_tmpED5,sizeof(char),20))),_tag_dyneither(_tmpED4,sizeof(void*),0)));}_LL1C8:;}
# 981
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp31E=e;struct Cyc_List_List**_tmp320;void*_tmp322;_LL209: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp31F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp31E;if(_tmp31F->tag != 24)goto _LL20B;else{_tmp320=(struct Cyc_List_List**)& _tmp31F->f1;}}_LL20A: {
# 985
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp320;for(0;effs != 0;effs=effs->tl){
void*_tmp323=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp323));{
void*_stmttmp16=(void*)effs->hd;void*_tmp324=_stmttmp16;_LL210: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp325=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp324;if(_tmp325->tag != 24)goto _LL212;}_LL211:
 goto _LL213;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp324;if(_tmp326->tag != 23)goto _LL214;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp326->f1);if(_tmp327->tag != 20)goto _LL214;}}_LL213:
 redo_join=1;goto _LL20F;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp324;if(_tmp328->tag != 23)goto _LL216;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp329=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp328->f1);if(_tmp329->tag != 22)goto _LL216;}}_LL215:
 redo_join=1;goto _LL20F;_LL216: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp324;if(_tmp32A->tag != 23)goto _LL218;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp32B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp32A->f1);if(_tmp32B->tag != 21)goto _LL218;}}_LL217:
 redo_join=1;goto _LL20F;_LL218:;_LL219:
 goto _LL20F;_LL20F:;};}}
# 997
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp320;for(0;effs != 0;effs=effs->tl){
void*_stmttmp17=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp32C=_stmttmp17;struct Cyc_List_List*_tmp32E;void*_tmp335;_LL21B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp32D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp32C;if(_tmp32D->tag != 24)goto _LL21D;else{_tmp32E=_tmp32D->f1;}}_LL21C:
# 1002
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp32E,effects);
goto _LL21A;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32C;if(_tmp32F->tag != 23)goto _LL21F;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp32F->f1);if(_tmp330->tag != 20)goto _LL21F;}}_LL21E:
 goto _LL21A;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp331=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32C;if(_tmp331->tag != 23)goto _LL221;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp331->f1);if(_tmp332->tag != 22)goto _LL221;}}_LL220:
 goto _LL21A;_LL221: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp333=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp32C;if(_tmp333->tag != 23)goto _LL223;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp334=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp333->f1);if(_tmp334->tag != 21)goto _LL223;}}_LL222:
 goto _LL21A;_LL223: _tmp335=_tmp32C;_LL224:
{struct Cyc_List_List*_tmpED6;effects=((_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6->hd=_tmp335,((_tmpED6->tl=effects,_tmpED6))))));}goto _LL21A;_LL21A:;}}
# 1010
*_tmp320=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL20B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp321=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp31E;if(_tmp321->tag != 25)goto _LL20D;else{_tmp322=(void*)_tmp321->f1;}}_LL20C: {
# 1013
void*_stmttmp15=Cyc_Tcutil_compress(_tmp322);void*_tmp337=_stmttmp15;_LL226: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp338=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp337;if(_tmp338->tag != 1)goto _LL228;}_LL227:
 goto _LL229;_LL228: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp337;if(_tmp339->tag != 2)goto _LL22A;}_LL229:
 return e;_LL22A:;_LL22B:
 return Cyc_Tcutil_rgns_of(_tmp322);_LL225:;}_LL20D:;_LL20E:
# 1018
 return e;_LL208:;};}
# 1023
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpEDC;struct Cyc_Absyn_FnInfo _tmpEDB;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpEDA;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp33A=(_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA[0]=((_tmpEDC.tag=9,((_tmpEDC.f1=((_tmpEDB.tvars=0,((_tmpEDB.effect=eff,((_tmpEDB.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpEDB.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpEDB.args=0,((_tmpEDB.c_varargs=0,((_tmpEDB.cyc_varargs=0,((_tmpEDB.rgn_po=0,((_tmpEDB.attributes=0,_tmpEDB)))))))))))))))))),_tmpEDC)))),_tmpEDA)));
# 1029
return Cyc_Absyn_atb_typ((void*)_tmp33A,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1036
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_stmttmp18=Cyc_Tcutil_compress(e);void*_tmp33E=_stmttmp18;void*_tmp340;struct Cyc_List_List*_tmp342;void*_tmp344;struct Cyc_Core_Opt*_tmp346;void**_tmp347;struct Cyc_Core_Opt*_tmp348;_LL22D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33E;if(_tmp33F->tag != 23)goto _LL22F;else{_tmp340=(void*)_tmp33F->f1;}}_LL22E:
# 1043
 if(constrain)return Cyc_Tcutil_unify(r,_tmp340);
_tmp340=Cyc_Tcutil_compress(_tmp340);
if(r == _tmp340)return 1;{
struct _tuple0 _tmpEDD;struct _tuple0 _stmttmp1B=(_tmpEDD.f1=r,((_tmpEDD.f2=_tmp340,_tmpEDD)));struct _tuple0 _tmp349=_stmttmp1B;struct Cyc_Absyn_Tvar*_tmp34B;struct Cyc_Absyn_Tvar*_tmp34D;_LL238:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp349.f1;if(_tmp34A->tag != 2)goto _LL23A;else{_tmp34B=_tmp34A->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp349.f2;if(_tmp34C->tag != 2)goto _LL23A;else{_tmp34D=_tmp34C->f1;}};_LL239:
 return Cyc_Absyn_tvar_cmp(_tmp34B,_tmp34D)== 0;_LL23A:;_LL23B:
 return 0;_LL237:;};_LL22F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp341=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp33E;if(_tmp341->tag != 24)goto _LL231;else{_tmp342=_tmp341->f1;}}_LL230:
# 1051
 for(0;_tmp342 != 0;_tmp342=_tmp342->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp342->hd))return 1;}
return 0;_LL231: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp343=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp33E;if(_tmp343->tag != 25)goto _LL233;else{_tmp344=(void*)_tmp343->f1;}}_LL232: {
# 1055
void*_stmttmp19=Cyc_Tcutil_rgns_of(_tmp344);void*_tmp34F=_stmttmp19;void*_tmp351;void*_tmp352;_LL23D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp350=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp34F;if(_tmp350->tag != 25)goto _LL23F;else{_tmp351=(void*)_tmp350->f1;}}_LL23E:
# 1057
 if(!constrain)return 0;{
void*_stmttmp1A=Cyc_Tcutil_compress(_tmp351);void*_tmp353=_stmttmp1A;struct Cyc_Core_Opt*_tmp355;void**_tmp356;struct Cyc_Core_Opt*_tmp357;_LL242: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp354=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp353;if(_tmp354->tag != 1)goto _LL244;else{_tmp355=_tmp354->f1;_tmp356=(void**)((void**)& _tmp354->f2);_tmp357=_tmp354->f4;}}_LL243: {
# 1062
void*_tmp358=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp357);
# 1065
Cyc_Tcutil_occurs(_tmp358,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp357))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEEC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEEB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEEA;void*_tmpEE9[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEE8;void*_tmp359=Cyc_Tcutil_dummy_fntype((void*)((_tmpEE8=_cycalloc(sizeof(*_tmpEE8)),((_tmpEE8[0]=((_tmpEEC.tag=24,((_tmpEEC.f1=((_tmpEE9[1]=(void*)((_tmpEEB=_cycalloc(sizeof(*_tmpEEB)),((_tmpEEB[0]=((_tmpEEA.tag=23,((_tmpEEA.f1=r,_tmpEEA)))),_tmpEEB)))),((_tmpEE9[0]=_tmp358,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEE9,sizeof(void*),2)))))),_tmpEEC)))),_tmpEE8)))));
*_tmp356=_tmp359;
return 1;};}_LL244:;_LL245:
 return 0;_LL241:;};_LL23F: _tmp352=_tmp34F;_LL240:
# 1071
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp352);_LL23C:;}_LL233: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp345=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33E;if(_tmp345->tag != 1)goto _LL235;else{_tmp346=_tmp345->f1;_tmp347=(void**)((void**)& _tmp345->f2);_tmp348=_tmp345->f4;}}_LL234:
# 1074
 if(_tmp346 == 0  || ((struct Cyc_Absyn_Kind*)_tmp346->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpEEF;void*_tmpEEE;(_tmpEEE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpEEF="effect evar has wrong kind",_tag_dyneither(_tmpEEF,sizeof(char),27))),_tag_dyneither(_tmpEEE,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1079
void*_tmp361=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp348);
# 1082
Cyc_Tcutil_occurs(_tmp361,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp348))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF04;struct Cyc_List_List*_tmpF03;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF02;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF01;struct Cyc_List_List*_tmpF00;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEFF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp362=(_tmpEFF=_cycalloc(sizeof(*_tmpEFF)),((_tmpEFF[0]=((_tmpF04.tag=24,((_tmpF04.f1=((_tmpF00=_cycalloc(sizeof(*_tmpF00)),((_tmpF00->hd=_tmp361,((_tmpF00->tl=((_tmpF03=_cycalloc(sizeof(*_tmpF03)),((_tmpF03->hd=(void*)((_tmpF01=_cycalloc(sizeof(*_tmpF01)),((_tmpF01[0]=((_tmpF02.tag=23,((_tmpF02.f1=r,_tmpF02)))),_tmpF01)))),((_tmpF03->tl=0,_tmpF03)))))),_tmpF00)))))),_tmpF04)))),_tmpEFF)));
*_tmp347=(void*)_tmp362;
return 1;};};_LL235:;_LL236:
 return 0;_LL22C:;};}
# 1093
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_stmttmp1C=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp369=_stmttmp1C;struct Cyc_List_List*_tmp36C;void*_tmp36E;struct Cyc_Core_Opt*_tmp370;void**_tmp371;struct Cyc_Core_Opt*_tmp372;_LL247: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp36A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp369;if(_tmp36A->tag != 23)goto _LL249;}_LL248:
 return 0;_LL249: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp369;if(_tmp36B->tag != 24)goto _LL24B;else{_tmp36C=_tmp36B->f1;}}_LL24A:
# 1098
 for(0;_tmp36C != 0;_tmp36C=_tmp36C->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp36C->hd))
return 1;}
return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp36D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp369;if(_tmp36D->tag != 25)goto _LL24D;else{_tmp36E=(void*)_tmp36D->f1;}}_LL24C:
# 1103
 _tmp36E=Cyc_Tcutil_compress(_tmp36E);
if(t == _tmp36E)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp36E);{
void*_stmttmp1D=Cyc_Tcutil_rgns_of(t);void*_tmp373=_stmttmp1D;void*_tmp375;void*_tmp376;_LL252: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp373;if(_tmp374->tag != 25)goto _LL254;else{_tmp375=(void*)_tmp374->f1;}}_LL253: {
# 1108
struct _tuple0 _tmpF05;struct _tuple0 _stmttmp1E=(_tmpF05.f1=Cyc_Tcutil_compress(_tmp375),((_tmpF05.f2=_tmp36E,_tmpF05)));struct _tuple0 _tmp377=_stmttmp1E;struct Cyc_Absyn_Tvar*_tmp379;struct Cyc_Absyn_Tvar*_tmp37B;_LL257:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp378=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp377.f1;if(_tmp378->tag != 2)goto _LL259;else{_tmp379=_tmp378->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp377.f2;if(_tmp37A->tag != 2)goto _LL259;else{_tmp37B=_tmp37A->f1;}};_LL258:
 return Cyc_Tcutil_unify(t,_tmp36E);_LL259:;_LL25A:
 return _tmp375 == _tmp36E;_LL256:;}_LL254: _tmp376=_tmp373;_LL255:
# 1112
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp376);_LL251:;};_LL24D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp36F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp369;if(_tmp36F->tag != 1)goto _LL24F;else{_tmp370=_tmp36F->f1;_tmp371=(void**)((void**)& _tmp36F->f2);_tmp372=_tmp36F->f4;}}_LL24E:
# 1115
 if(_tmp370 == 0  || ((struct Cyc_Absyn_Kind*)_tmp370->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF08;void*_tmpF07;(_tmpF07=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF08="effect evar has wrong kind",_tag_dyneither(_tmpF08,sizeof(char),27))),_tag_dyneither(_tmpF07,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1120
void*_tmp37F=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp372);
# 1123
Cyc_Tcutil_occurs(_tmp37F,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp372))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF1D;struct Cyc_List_List*_tmpF1C;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF1B;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF1A;struct Cyc_List_List*_tmpF19;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF18;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp380=(_tmpF18=_cycalloc(sizeof(*_tmpF18)),((_tmpF18[0]=((_tmpF1D.tag=24,((_tmpF1D.f1=((_tmpF19=_cycalloc(sizeof(*_tmpF19)),((_tmpF19->hd=_tmp37F,((_tmpF19->tl=((_tmpF1C=_cycalloc(sizeof(*_tmpF1C)),((_tmpF1C->hd=(void*)((_tmpF1A=_cycalloc(sizeof(*_tmpF1A)),((_tmpF1A[0]=((_tmpF1B.tag=25,((_tmpF1B.f1=t,_tmpF1B)))),_tmpF1A)))),((_tmpF1C->tl=0,_tmpF1C)))))),_tmpF19)))))),_tmpF1D)))),_tmpF18)));
*_tmp371=(void*)_tmp380;
return 1;};};_LL24F:;_LL250:
 return 0;_LL246:;};}
# 1134
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp387=e;struct Cyc_Absyn_Tvar*_tmp389;struct Cyc_List_List*_tmp38B;void*_tmp38D;struct Cyc_Core_Opt*_tmp38F;void**_tmp390;struct Cyc_Core_Opt*_tmp391;_LL25C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp388=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp387;if(_tmp388->tag != 2)goto _LL25E;else{_tmp389=_tmp388->f1;}}_LL25D:
 return Cyc_Absyn_tvar_cmp(v,_tmp389)== 0;_LL25E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp387;if(_tmp38A->tag != 24)goto _LL260;else{_tmp38B=_tmp38A->f1;}}_LL25F:
# 1139
 for(0;_tmp38B != 0;_tmp38B=_tmp38B->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp38B->hd))
return 1;}
return 0;_LL260: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp387;if(_tmp38C->tag != 25)goto _LL262;else{_tmp38D=(void*)_tmp38C->f1;}}_LL261: {
# 1144
void*_stmttmp1F=Cyc_Tcutil_rgns_of(_tmp38D);void*_tmp392=_stmttmp1F;void*_tmp394;void*_tmp395;_LL267: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp393=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp392;if(_tmp393->tag != 25)goto _LL269;else{_tmp394=(void*)_tmp393->f1;}}_LL268:
# 1146
 if(!may_constrain_evars)return 0;{
void*_stmttmp20=Cyc_Tcutil_compress(_tmp394);void*_tmp396=_stmttmp20;struct Cyc_Core_Opt*_tmp398;void**_tmp399;struct Cyc_Core_Opt*_tmp39A;_LL26C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp397=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp396;if(_tmp397->tag != 1)goto _LL26E;else{_tmp398=_tmp397->f1;_tmp399=(void**)((void**)& _tmp397->f2);_tmp39A=_tmp397->f4;}}_LL26D: {
# 1152
void*_tmp39B=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp39A);
# 1154
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39A))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF2C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF2B;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF2A;void*_tmpF29[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF28;void*_tmp39C=Cyc_Tcutil_dummy_fntype((void*)((_tmpF28=_cycalloc(sizeof(*_tmpF28)),((_tmpF28[0]=((_tmpF2C.tag=24,((_tmpF2C.f1=((_tmpF29[1]=(void*)((_tmpF2B=_cycalloc(sizeof(*_tmpF2B)),((_tmpF2B[0]=((_tmpF2A.tag=2,((_tmpF2A.f1=v,_tmpF2A)))),_tmpF2B)))),((_tmpF29[0]=_tmp39B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF29,sizeof(void*),2)))))),_tmpF2C)))),_tmpF28)))));
*_tmp399=_tmp39C;
return 1;};}_LL26E:;_LL26F:
 return 0;_LL26B:;};_LL269: _tmp395=_tmp392;_LL26A:
# 1160
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp395);_LL266:;}_LL262: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp387;if(_tmp38E->tag != 1)goto _LL264;else{_tmp38F=_tmp38E->f1;_tmp390=(void**)((void**)& _tmp38E->f2);_tmp391=_tmp38E->f4;}}_LL263:
# 1163
 if(_tmp38F == 0  || ((struct Cyc_Absyn_Kind*)_tmp38F->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpF2F;void*_tmpF2E;(_tmpF2E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF2F="effect evar has wrong kind",_tag_dyneither(_tmpF2F,sizeof(char),27))),_tag_dyneither(_tmpF2E,sizeof(void*),0)));}{
# 1167
void*_tmp3A4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp391);
# 1169
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp391))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF44;struct Cyc_List_List*_tmpF43;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF42;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF41;struct Cyc_List_List*_tmpF40;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF3F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3A5=(_tmpF3F=_cycalloc(sizeof(*_tmpF3F)),((_tmpF3F[0]=((_tmpF44.tag=24,((_tmpF44.f1=((_tmpF40=_cycalloc(sizeof(*_tmpF40)),((_tmpF40->hd=_tmp3A4,((_tmpF40->tl=((_tmpF43=_cycalloc(sizeof(*_tmpF43)),((_tmpF43->hd=(void*)((_tmpF41=_cycalloc(sizeof(*_tmpF41)),((_tmpF41[0]=((_tmpF42.tag=2,((_tmpF42.f1=v,_tmpF42)))),_tmpF41)))),((_tmpF43->tl=0,_tmpF43)))))),_tmpF40)))))),_tmpF44)))),_tmpF3F)));
*_tmp390=(void*)_tmp3A5;
return 1;};};_LL264:;_LL265:
 return 0;_LL25B:;};}
# 1179
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3AC=e;struct Cyc_List_List*_tmp3AE;void*_tmp3B0;_LL271: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AD=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3AC;if(_tmp3AD->tag != 24)goto _LL273;else{_tmp3AE=_tmp3AD->f1;}}_LL272:
# 1183
 for(0;_tmp3AE != 0;_tmp3AE=_tmp3AE->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3AE->hd))
return 1;}
return 0;_LL273: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3AF=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3AC;if(_tmp3AF->tag != 25)goto _LL275;else{_tmp3B0=(void*)_tmp3AF->f1;}}_LL274: {
# 1188
void*_stmttmp21=Cyc_Tcutil_rgns_of(_tmp3B0);void*_tmp3B2=_stmttmp21;void*_tmp3B4;void*_tmp3B5;_LL27A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B2;if(_tmp3B3->tag != 25)goto _LL27C;else{_tmp3B4=(void*)_tmp3B3->f1;}}_LL27B:
 return 0;_LL27C: _tmp3B5=_tmp3B2;_LL27D:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3B5);_LL279:;}_LL275: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3AC;if(_tmp3B1->tag != 1)goto _LL277;}_LL276:
# 1192
 return evar == e;_LL277:;_LL278:
 return 0;_LL270:;};}
# 1206 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1209
void*_stmttmp22=Cyc_Tcutil_compress(e1);void*_tmp3B6=_stmttmp22;struct Cyc_List_List*_tmp3B8;void*_tmp3BA;struct Cyc_Absyn_Tvar*_tmp3BC;void*_tmp3BE;void**_tmp3C0;struct Cyc_Core_Opt*_tmp3C1;_LL27F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B7=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B6;if(_tmp3B7->tag != 24)goto _LL281;else{_tmp3B8=_tmp3B7->f1;}}_LL280:
# 1211
 for(0;_tmp3B8 != 0;_tmp3B8=_tmp3B8->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3B8->hd,e2))
return 0;}
return 1;_LL281: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3B9=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B6;if(_tmp3B9->tag != 23)goto _LL283;else{_tmp3BA=(void*)_tmp3B9->f1;}}_LL282:
# 1222
 return Cyc_Tcutil_region_in_effect(0,_tmp3BA,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3BA,(void*)& Cyc_Absyn_HeapRgn_val);_LL283: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3BB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3B6;if(_tmp3BB->tag != 2)goto _LL285;else{_tmp3BC=_tmp3BB->f1;}}_LL284:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3BC,e2);_LL285: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B6;if(_tmp3BD->tag != 25)goto _LL287;else{_tmp3BE=(void*)_tmp3BD->f1;}}_LL286: {
# 1226
void*_stmttmp23=Cyc_Tcutil_rgns_of(_tmp3BE);void*_tmp3C2=_stmttmp23;void*_tmp3C4;void*_tmp3C5;_LL28C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3C3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3C2;if(_tmp3C3->tag != 25)goto _LL28E;else{_tmp3C4=(void*)_tmp3C3->f1;}}_LL28D:
# 1231
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3C4,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3C4,Cyc_Absyn_sint_typ);_LL28E: _tmp3C5=_tmp3C2;_LL28F:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3C5,e2);_LL28B:;}_LL287: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3BF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B6;if(_tmp3BF->tag != 1)goto _LL289;else{_tmp3C0=(void**)((void**)& _tmp3BF->f2);_tmp3C1=_tmp3BF->f4;}}_LL288:
# 1236
 if(!Cyc_Tcutil_evar_in_effect(e1,e2))
# 1240
*_tmp3C0=Cyc_Absyn_empty_effect;
# 1244
return 1;_LL289:;_LL28A: {
const char*_tmpF48;void*_tmpF47[1];struct Cyc_String_pa_PrintArg_struct _tmpF46;(_tmpF46.tag=0,((_tmpF46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpF47[0]=& _tmpF46,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF48="subset_effect: bad effect: %s",_tag_dyneither(_tmpF48,sizeof(char),30))),_tag_dyneither(_tmpF47,sizeof(void*),1)))))));}_LL27E:;}
# 1260 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1276
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1278
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_stmttmp24=(struct _tuple0*)r1->hd;void*_tmp3CA;void*_tmp3CB;struct _tuple0*_tmp3C9=_stmttmp24;_tmp3CA=_tmp3C9->f1;_tmp3CB=_tmp3C9->f2;{
int found=_tmp3CA == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_stmttmp25=(struct _tuple0*)r2->hd;void*_tmp3CD;void*_tmp3CE;struct _tuple0*_tmp3CC=_stmttmp25;_tmp3CD=_tmp3CC->f1;_tmp3CE=_tmp3CC->f2;
if(Cyc_Tcutil_unify(_tmp3CA,_tmp3CD) && Cyc_Tcutil_unify(_tmp3CB,_tmp3CE)){
found=1;
break;}}}
# 1288
if(!found)return 0;};}}
# 1290
return 1;}
# 1294
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1296
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1300
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp3CF=t1;struct Cyc_Core_Opt*_tmp3D1;void**_tmp3D2;struct Cyc_Core_Opt*_tmp3D3;_LL291: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3CF;if(_tmp3D0->tag != 1)goto _LL293;else{_tmp3D1=_tmp3D0->f1;_tmp3D2=(void**)((void**)& _tmp3D0->f2);_tmp3D3=_tmp3D0->f4;}}_LL292:
# 1311
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D3))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3D4=Cyc_Tcutil_typ_kind(t2);
# 1316
if(Cyc_Tcutil_kind_leq(_tmp3D4,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3D1))->v)){
*_tmp3D2=t2;
return;}else{
# 1320
{void*_tmp3D5=t2;void**_tmp3D7;struct Cyc_Core_Opt*_tmp3D8;struct Cyc_Absyn_PtrInfo _tmp3DA;_LL296: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D5;if(_tmp3D6->tag != 1)goto _LL298;else{_tmp3D7=(void**)((void**)& _tmp3D6->f2);_tmp3D8=_tmp3D6->f4;}}_LL297: {
# 1323
struct Cyc_List_List*_tmp3DB=(struct Cyc_List_List*)_tmp3D3->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3D8))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp3DB,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmpF49;Cyc_Tcutil_failure_reason=((_tmpF49="(type variable would escape scope)",_tag_dyneither(_tmpF49,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1330
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3D1->v,_tmp3D4)){
*_tmp3D7=t1;return;}
# 1333
{const char*_tmpF4A;Cyc_Tcutil_failure_reason=((_tmpF4A="(kinds are incompatible)",_tag_dyneither(_tmpF4A,sizeof(char),25)));}
goto _LL295;}_LL298:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3D9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D5;if(_tmp3D9->tag != 5)goto _LL29A;else{_tmp3DA=_tmp3D9->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3D1->v)->kind == Cyc_Absyn_BoxKind))goto _LL29A;_LL299: {
# 1339
union Cyc_Absyn_Constraint*_tmp3DE=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp3DA.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp3DF=_tmp3DE;_LL29D: if((_tmp3DF->No_constr).tag != 3)goto _LL29F;_LL29E:
# 1343
{struct _union_Constraint_Eq_constr*_tmpF4B;(_tmpF4B=& _tmp3DE->Eq_constr,((_tmpF4B->tag=1,_tmpF4B->val=Cyc_Absyn_bounds_one)));}
*_tmp3D2=t2;
return;_LL29F:;_LL2A0:
 goto _LL29C;_LL29C:;}
# 1348
goto _LL295;}_LL29A:;_LL29B:
 goto _LL295;_LL295:;}
# 1351
{const char*_tmpF4C;Cyc_Tcutil_failure_reason=((_tmpF4C="(kinds are incompatible)",_tag_dyneither(_tmpF4C,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL293:;_LL294:
# 1354
 goto _LL290;_LL290:;}
# 1359
{struct _tuple0 _tmpF4D;struct _tuple0 _stmttmp26=(_tmpF4D.f1=t2,((_tmpF4D.f2=t1,_tmpF4D)));struct _tuple0 _tmp3E2=_stmttmp26;struct Cyc_Absyn_Tvar*_tmp3E7;struct Cyc_Absyn_Tvar*_tmp3E9;union Cyc_Absyn_AggrInfoU _tmp3EB;struct Cyc_List_List*_tmp3EC;union Cyc_Absyn_AggrInfoU _tmp3EE;struct Cyc_List_List*_tmp3EF;struct _tuple2*_tmp3F1;struct _tuple2*_tmp3F3;struct Cyc_List_List*_tmp3F5;struct Cyc_List_List*_tmp3F7;struct Cyc_Absyn_Datatypedecl*_tmp3F9;struct Cyc_List_List*_tmp3FA;struct Cyc_Absyn_Datatypedecl*_tmp3FC;struct Cyc_List_List*_tmp3FD;struct Cyc_Absyn_Datatypedecl*_tmp3FF;struct Cyc_Absyn_Datatypefield*_tmp400;struct Cyc_List_List*_tmp401;struct Cyc_Absyn_Datatypedecl*_tmp403;struct Cyc_Absyn_Datatypefield*_tmp404;struct Cyc_List_List*_tmp405;void*_tmp407;struct Cyc_Absyn_Tqual _tmp408;void*_tmp409;union Cyc_Absyn_Constraint*_tmp40A;union Cyc_Absyn_Constraint*_tmp40B;union Cyc_Absyn_Constraint*_tmp40C;void*_tmp40E;struct Cyc_Absyn_Tqual _tmp40F;void*_tmp410;union Cyc_Absyn_Constraint*_tmp411;union Cyc_Absyn_Constraint*_tmp412;union Cyc_Absyn_Constraint*_tmp413;enum Cyc_Absyn_Sign _tmp415;enum Cyc_Absyn_Size_of _tmp416;enum Cyc_Absyn_Sign _tmp418;enum Cyc_Absyn_Size_of _tmp419;int _tmp41B;int _tmp41D;void*_tmp41F;void*_tmp421;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp425;void*_tmp427;struct Cyc_Absyn_Tqual _tmp428;struct Cyc_Absyn_Exp*_tmp429;union Cyc_Absyn_Constraint*_tmp42A;void*_tmp42C;struct Cyc_Absyn_Tqual _tmp42D;struct Cyc_Absyn_Exp*_tmp42E;union Cyc_Absyn_Constraint*_tmp42F;struct Cyc_List_List*_tmp431;void*_tmp432;struct Cyc_Absyn_Tqual _tmp433;void*_tmp434;struct Cyc_List_List*_tmp435;int _tmp436;struct Cyc_Absyn_VarargInfo*_tmp437;struct Cyc_List_List*_tmp438;struct Cyc_List_List*_tmp439;struct Cyc_List_List*_tmp43B;void*_tmp43C;struct Cyc_Absyn_Tqual _tmp43D;void*_tmp43E;struct Cyc_List_List*_tmp43F;int _tmp440;struct Cyc_Absyn_VarargInfo*_tmp441;struct Cyc_List_List*_tmp442;struct Cyc_List_List*_tmp443;struct Cyc_List_List*_tmp445;struct Cyc_List_List*_tmp447;enum Cyc_Absyn_AggrKind _tmp449;struct Cyc_List_List*_tmp44A;enum Cyc_Absyn_AggrKind _tmp44C;struct Cyc_List_List*_tmp44D;struct Cyc_List_List*_tmp44F;struct Cyc_Absyn_Typedefdecl*_tmp450;struct Cyc_List_List*_tmp452;struct Cyc_Absyn_Typedefdecl*_tmp453;void*_tmp45B;void*_tmp45D;void*_tmp45F;void*_tmp460;void*_tmp462;void*_tmp463;_LL2A2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E3->tag != 1)goto _LL2A4;}_LL2A3:
# 1362
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A4:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E4=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E4->tag != 0)goto _LL2A6;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp3E5=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3E5->tag != 0)goto _LL2A6;};_LL2A5:
# 1365
 return;_LL2A6:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E6=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3E6->tag != 2)goto _LL2A8;else{_tmp3E7=_tmp3E6->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3E8->tag != 2)goto _LL2A8;else{_tmp3E9=_tmp3E8->f1;}};_LL2A7: {
# 1368
struct _dyneither_ptr*_tmp46A=_tmp3E7->name;
struct _dyneither_ptr*_tmp46B=_tmp3E9->name;
# 1371
int _tmp46C=_tmp3E7->identity;
int _tmp46D=_tmp3E9->identity;
if(_tmp46D == _tmp46C)return;
{const char*_tmpF4E;Cyc_Tcutil_failure_reason=((_tmpF4E="(variable types are not the same)",_tag_dyneither(_tmpF4E,sizeof(char),34)));}
goto _LL2A1;}_LL2A8:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3EA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3EA->tag != 11)goto _LL2AA;else{_tmp3EB=(_tmp3EA->f1).aggr_info;_tmp3EC=(_tmp3EA->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3ED=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3ED->tag != 11)goto _LL2AA;else{_tmp3EE=(_tmp3ED->f1).aggr_info;_tmp3EF=(_tmp3ED->f1).targs;}};_LL2A9: {
# 1378
struct _tuple13 _stmttmp2E=Cyc_Absyn_aggr_kinded_name(_tmp3EE);enum Cyc_Absyn_AggrKind _tmp470;struct _tuple2*_tmp471;struct _tuple13 _tmp46F=_stmttmp2E;_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;{
struct _tuple13 _stmttmp2F=Cyc_Absyn_aggr_kinded_name(_tmp3EB);enum Cyc_Absyn_AggrKind _tmp473;struct _tuple2*_tmp474;struct _tuple13 _tmp472=_stmttmp2F;_tmp473=_tmp472.f1;_tmp474=_tmp472.f2;
if(_tmp470 != _tmp473){{const char*_tmpF4F;Cyc_Tcutil_failure_reason=((_tmpF4F="(struct and union type)",_tag_dyneither(_tmpF4F,sizeof(char),24)));}goto _LL2A1;}
if(Cyc_Absyn_qvar_cmp(_tmp471,_tmp474)!= 0){{const char*_tmpF50;Cyc_Tcutil_failure_reason=((_tmpF50="(different type name)",_tag_dyneither(_tmpF50,sizeof(char),22)));}goto _LL2A1;}
Cyc_Tcutil_unify_list(_tmp3EF,_tmp3EC);
return;};}_LL2AA:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F0->tag != 13)goto _LL2AC;else{_tmp3F1=_tmp3F0->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp3F2=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3F2->tag != 13)goto _LL2AC;else{_tmp3F3=_tmp3F2->f1;}};_LL2AB:
# 1387
 if(Cyc_Absyn_qvar_cmp(_tmp3F1,_tmp3F3)== 0)return;
{const char*_tmpF51;Cyc_Tcutil_failure_reason=((_tmpF51="(different enum types)",_tag_dyneither(_tmpF51,sizeof(char),23)));}
goto _LL2A1;_LL2AC:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F4->tag != 14)goto _LL2AE;else{_tmp3F5=_tmp3F4->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3F6->tag != 14)goto _LL2AE;else{_tmp3F7=_tmp3F6->f1;}};_LL2AD: {
# 1392
int bad=0;
for(0;_tmp3F5 != 0  && _tmp3F7 != 0;(_tmp3F5=_tmp3F5->tl,_tmp3F7=_tmp3F7->tl)){
struct Cyc_Absyn_Enumfield*_tmp478=(struct Cyc_Absyn_Enumfield*)_tmp3F5->hd;
struct Cyc_Absyn_Enumfield*_tmp479=(struct Cyc_Absyn_Enumfield*)_tmp3F7->hd;
if(Cyc_Absyn_qvar_cmp(_tmp478->name,_tmp479->name)!= 0){
{const char*_tmpF52;Cyc_Tcutil_failure_reason=((_tmpF52="(different names for enum fields)",_tag_dyneither(_tmpF52,sizeof(char),34)));}
bad=1;
break;}
# 1401
if(_tmp478->tag == _tmp479->tag)continue;
if(_tmp478->tag == 0  || _tmp479->tag == 0){
{const char*_tmpF53;Cyc_Tcutil_failure_reason=((_tmpF53="(different tag values for enum fields)",_tag_dyneither(_tmpF53,sizeof(char),39)));}
bad=1;
break;}
# 1407
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp478->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp479->tag))){
{const char*_tmpF54;Cyc_Tcutil_failure_reason=((_tmpF54="(different tag values for enum fields)",_tag_dyneither(_tmpF54,sizeof(char),39)));}
bad=1;
break;}}
# 1413
if(bad)goto _LL2A1;
if(_tmp3F5 == 0  && _tmp3F7 == 0)return;
{const char*_tmpF55;Cyc_Tcutil_failure_reason=((_tmpF55="(different number of fields for enums)",_tag_dyneither(_tmpF55,sizeof(char),39)));}
goto _LL2A1;}_LL2AE:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3F8->tag != 3)goto _LL2B0;else{if((((_tmp3F8->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp3F9=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3F8->f1).datatype_info).KnownDatatype).val);_tmp3FA=(_tmp3F8->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp3FB->tag != 3)goto _LL2B0;else{if((((_tmp3FB->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp3FC=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp3FB->f1).datatype_info).KnownDatatype).val);_tmp3FD=(_tmp3FB->f1).targs;}};_LL2AF:
# 1420
 if(_tmp3F9 == _tmp3FC  || Cyc_Absyn_qvar_cmp(_tmp3F9->name,_tmp3FC->name)== 0){
Cyc_Tcutil_unify_list(_tmp3FD,_tmp3FA);
return;}
# 1424
goto _LL2A1;_LL2B0:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp3FE->tag != 4)goto _LL2B2;else{if((((_tmp3FE->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp3FF=((struct _tuple3)(((_tmp3FE->f1).field_info).KnownDatatypefield).val).f1;_tmp400=((struct _tuple3)(((_tmp3FE->f1).field_info).KnownDatatypefield).val).f2;_tmp401=(_tmp3FE->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp402=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp402->tag != 4)goto _LL2B2;else{if((((_tmp402->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp403=((struct _tuple3)(((_tmp402->f1).field_info).KnownDatatypefield).val).f1;_tmp404=((struct _tuple3)(((_tmp402->f1).field_info).KnownDatatypefield).val).f2;_tmp405=(_tmp402->f1).targs;}};_LL2B1:
# 1428
 if((_tmp3FF == _tmp403  || Cyc_Absyn_qvar_cmp(_tmp3FF->name,_tmp403->name)== 0) && (
_tmp400 == _tmp404  || Cyc_Absyn_qvar_cmp(_tmp400->name,_tmp404->name)== 0)){
Cyc_Tcutil_unify_list(_tmp405,_tmp401);
return;}
# 1433
{const char*_tmpF56;Cyc_Tcutil_failure_reason=((_tmpF56="(different datatype field types)",_tag_dyneither(_tmpF56,sizeof(char),33)));}
goto _LL2A1;_LL2B2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp406->tag != 5)goto _LL2B4;else{_tmp407=(_tmp406->f1).elt_typ;_tmp408=(_tmp406->f1).elt_tq;_tmp409=((_tmp406->f1).ptr_atts).rgn;_tmp40A=((_tmp406->f1).ptr_atts).nullable;_tmp40B=((_tmp406->f1).ptr_atts).bounds;_tmp40C=((_tmp406->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp40D->tag != 5)goto _LL2B4;else{_tmp40E=(_tmp40D->f1).elt_typ;_tmp40F=(_tmp40D->f1).elt_tq;_tmp410=((_tmp40D->f1).ptr_atts).rgn;_tmp411=((_tmp40D->f1).ptr_atts).nullable;_tmp412=((_tmp40D->f1).ptr_atts).bounds;_tmp413=((_tmp40D->f1).ptr_atts).zero_term;}};_LL2B3:
# 1438
 Cyc_Tcutil_unify_it(_tmp40E,_tmp407);
Cyc_Tcutil_unify_it(_tmp409,_tmp410);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF57;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp413,_tmp40C,((_tmpF57="(not both zero terminated)",_tag_dyneither(_tmpF57,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp40F,_tmp40E,_tmp408,_tmp407);
{const char*_tmpF58;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp412,_tmp40B,((_tmpF58="(different pointer bounds)",_tag_dyneither(_tmpF58,sizeof(char),27))));}
# 1446
{void*_stmttmp2D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp412);void*_tmp481=_stmttmp2D;_LL2DF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp482=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp481;if(_tmp482->tag != 0)goto _LL2E1;}_LL2E0:
 return;_LL2E1:;_LL2E2:
 goto _LL2DE;_LL2DE:;}
# 1450
{const char*_tmpF59;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp411,_tmp40A,((_tmpF59="(incompatible pointer types)",_tag_dyneither(_tmpF59,sizeof(char),29))));}
return;_LL2B4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp414=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp414->tag != 6)goto _LL2B6;else{_tmp415=_tmp414->f1;_tmp416=_tmp414->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp417->tag != 6)goto _LL2B6;else{_tmp418=_tmp417->f1;_tmp419=_tmp417->f2;}};_LL2B5:
# 1455
 if(_tmp418 == _tmp415  && ((_tmp419 == _tmp416  || _tmp419 == Cyc_Absyn_Int_sz  && _tmp416 == Cyc_Absyn_Long_sz) || 
_tmp419 == Cyc_Absyn_Long_sz  && _tmp416 == Cyc_Absyn_Int_sz))return;
{const char*_tmpF5A;Cyc_Tcutil_failure_reason=((_tmpF5A="(different integral types)",_tag_dyneither(_tmpF5A,sizeof(char),27)));}
goto _LL2A1;_LL2B6:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp41A->tag != 7)goto _LL2B8;else{_tmp41B=_tmp41A->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp41C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp41C->tag != 7)goto _LL2B8;else{_tmp41D=_tmp41C->f1;}};_LL2B7:
# 1461
 if(_tmp41D == 0  && _tmp41B == 0)return;else{
if(_tmp41D == 1  && _tmp41B == 1)return;else{
# 1464
if(((_tmp41D != 0  && _tmp41D != 1) && _tmp41B != 0) && _tmp41B != 1)return;}}
goto _LL2A1;_LL2B8:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp41E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp41E->tag != 19)goto _LL2BA;else{_tmp41F=(void*)_tmp41E->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp420=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp420->tag != 19)goto _LL2BA;else{_tmp421=(void*)_tmp420->f1;}};_LL2B9:
# 1467
 Cyc_Tcutil_unify_it(_tmp41F,_tmp421);return;_LL2BA:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp422=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp422->tag != 18)goto _LL2BC;else{_tmp423=_tmp422->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp424->tag != 18)goto _LL2BC;else{_tmp425=_tmp424->f1;}};_LL2BB:
# 1470
 if(!Cyc_Evexp_same_const_exp(_tmp423,_tmp425)){
{const char*_tmpF5B;Cyc_Tcutil_failure_reason=((_tmpF5B="(cannot prove expressions are the same)",_tag_dyneither(_tmpF5B,sizeof(char),40)));}
goto _LL2A1;}
# 1474
return;_LL2BC:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp426=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp426->tag != 8)goto _LL2BE;else{_tmp427=(_tmp426->f1).elt_type;_tmp428=(_tmp426->f1).tq;_tmp429=(_tmp426->f1).num_elts;_tmp42A=(_tmp426->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp42B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp42B->tag != 8)goto _LL2BE;else{_tmp42C=(_tmp42B->f1).elt_type;_tmp42D=(_tmp42B->f1).tq;_tmp42E=(_tmp42B->f1).num_elts;_tmp42F=(_tmp42B->f1).zero_term;}};_LL2BD:
# 1478
 Cyc_Tcutil_unify_it(_tmp42C,_tmp427);
Cyc_Tcutil_unify_tqual(_tmp42D,_tmp42C,_tmp428,_tmp427);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF5C;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp42A,_tmp42F,((_tmpF5C="(not both zero terminated)",_tag_dyneither(_tmpF5C,sizeof(char),27))));}
if(_tmp429 == _tmp42E)return;
if(_tmp429 == 0  || _tmp42E == 0)goto _LL2A1;
if(Cyc_Evexp_same_const_exp(_tmp429,_tmp42E))
return;
{const char*_tmpF5D;Cyc_Tcutil_failure_reason=((_tmpF5D="(different array sizes)",_tag_dyneither(_tmpF5D,sizeof(char),24)));}
goto _LL2A1;_LL2BE:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp430=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp430->tag != 9)goto _LL2C0;else{_tmp431=(_tmp430->f1).tvars;_tmp432=(_tmp430->f1).effect;_tmp433=(_tmp430->f1).ret_tqual;_tmp434=(_tmp430->f1).ret_typ;_tmp435=(_tmp430->f1).args;_tmp436=(_tmp430->f1).c_varargs;_tmp437=(_tmp430->f1).cyc_varargs;_tmp438=(_tmp430->f1).rgn_po;_tmp439=(_tmp430->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp43A->tag != 9)goto _LL2C0;else{_tmp43B=(_tmp43A->f1).tvars;_tmp43C=(_tmp43A->f1).effect;_tmp43D=(_tmp43A->f1).ret_tqual;_tmp43E=(_tmp43A->f1).ret_typ;_tmp43F=(_tmp43A->f1).args;_tmp440=(_tmp43A->f1).c_varargs;_tmp441=(_tmp43A->f1).cyc_varargs;_tmp442=(_tmp43A->f1).rgn_po;_tmp443=(_tmp43A->f1).attributes;}};_LL2BF: {
# 1492
int done=0;
struct _RegionHandle _tmp488=_new_region("rgn");struct _RegionHandle*rgn=& _tmp488;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp43B != 0){
if(_tmp431 == 0){
{const char*_tmpF5E;Cyc_Tcutil_failure_reason=((_tmpF5E="(second function type has too few type variables)",_tag_dyneither(_tmpF5E,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1500
void*_tmp48A=((struct Cyc_Absyn_Tvar*)_tmp43B->hd)->kind;
void*_tmp48B=((struct Cyc_Absyn_Tvar*)_tmp431->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp48A,_tmp48B)){
{const char*_tmpF64;void*_tmpF63[3];struct Cyc_String_pa_PrintArg_struct _tmpF62;struct Cyc_String_pa_PrintArg_struct _tmpF61;struct Cyc_String_pa_PrintArg_struct _tmpF60;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmpF60.tag=0,((_tmpF60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1506
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp431->hd,& Cyc_Tcutil_bk))),((_tmpF61.tag=0,((_tmpF61.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1505
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp43B->hd,& Cyc_Tcutil_bk))),((_tmpF62.tag=0,((_tmpF62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1504
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp43B->hd)),((_tmpF63[0]=& _tmpF62,((_tmpF63[1]=& _tmpF61,((_tmpF63[2]=& _tmpF60,Cyc_aprintf(((_tmpF64="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmpF64,sizeof(char),44))),_tag_dyneither(_tmpF63,sizeof(void*),3))))))))))))))))))));}
# 1507
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1509
{struct _tuple16*_tmpF6E;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF6D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF6C;struct Cyc_List_List*_tmpF6B;inst=((_tmpF6B=_region_malloc(rgn,sizeof(*_tmpF6B)),((_tmpF6B->hd=((_tmpF6E=_region_malloc(rgn,sizeof(*_tmpF6E)),((_tmpF6E->f1=(struct Cyc_Absyn_Tvar*)_tmp431->hd,((_tmpF6E->f2=(void*)((_tmpF6C=_cycalloc(sizeof(*_tmpF6C)),((_tmpF6C[0]=((_tmpF6D.tag=2,((_tmpF6D.f1=(struct Cyc_Absyn_Tvar*)_tmp43B->hd,_tmpF6D)))),_tmpF6C)))),_tmpF6E)))))),((_tmpF6B->tl=inst,_tmpF6B))))));}
_tmp43B=_tmp43B->tl;
_tmp431=_tmp431->tl;};}
# 1513
if(_tmp431 != 0){
{const char*_tmpF6F;Cyc_Tcutil_failure_reason=((_tmpF6F="(second function type has too many type variables)",_tag_dyneither(_tmpF6F,sizeof(char),51)));}
_npop_handler(0);goto _LL2A1;}
# 1517
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF7B;struct Cyc_Absyn_FnInfo _tmpF7A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF79;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF75;struct Cyc_Absyn_FnInfo _tmpF74;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF73;Cyc_Tcutil_unify_it((void*)((_tmpF73=_cycalloc(sizeof(*_tmpF73)),((_tmpF73[0]=((_tmpF75.tag=9,((_tmpF75.f1=((_tmpF74.tvars=0,((_tmpF74.effect=_tmp43C,((_tmpF74.ret_tqual=_tmp43D,((_tmpF74.ret_typ=_tmp43E,((_tmpF74.args=_tmp43F,((_tmpF74.c_varargs=_tmp440,((_tmpF74.cyc_varargs=_tmp441,((_tmpF74.rgn_po=_tmp442,((_tmpF74.attributes=_tmp443,_tmpF74)))))))))))))))))),_tmpF75)))),_tmpF73)))),
# 1520
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpF79=_cycalloc(sizeof(*_tmpF79)),((_tmpF79[0]=((_tmpF7B.tag=9,((_tmpF7B.f1=((_tmpF7A.tvars=0,((_tmpF7A.effect=_tmp432,((_tmpF7A.ret_tqual=_tmp433,((_tmpF7A.ret_typ=_tmp434,((_tmpF7A.args=_tmp435,((_tmpF7A.c_varargs=_tmp436,((_tmpF7A.cyc_varargs=_tmp437,((_tmpF7A.rgn_po=_tmp438,((_tmpF7A.attributes=_tmp439,_tmpF7A)))))))))))))))))),_tmpF7B)))),_tmpF79))))));}
# 1523
done=1;}}
# 1526
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp43E,_tmp434);
Cyc_Tcutil_unify_tqual(_tmp43D,_tmp43E,_tmp433,_tmp434);
for(0;_tmp43F != 0  && _tmp435 != 0;(_tmp43F=_tmp43F->tl,_tmp435=_tmp435->tl)){
struct _tuple10 _stmttmp29=*((struct _tuple10*)_tmp43F->hd);struct Cyc_Absyn_Tqual _tmp49D;void*_tmp49E;struct _tuple10 _tmp49C=_stmttmp29;_tmp49D=_tmp49C.f2;_tmp49E=_tmp49C.f3;{
struct _tuple10 _stmttmp2A=*((struct _tuple10*)_tmp435->hd);struct Cyc_Absyn_Tqual _tmp4A0;void*_tmp4A1;struct _tuple10 _tmp49F=_stmttmp2A;_tmp4A0=_tmp49F.f2;_tmp4A1=_tmp49F.f3;
Cyc_Tcutil_unify_it(_tmp49E,_tmp4A1);
Cyc_Tcutil_unify_tqual(_tmp49D,_tmp49E,_tmp4A0,_tmp4A1);};}
# 1536
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp43F != 0  || _tmp435 != 0){
{const char*_tmpF7C;Cyc_Tcutil_failure_reason=((_tmpF7C="(function types have different number of arguments)",_tag_dyneither(_tmpF7C,sizeof(char),52)));}
_npop_handler(0);goto _LL2A1;}
# 1542
if(_tmp440 != _tmp436){
{const char*_tmpF7D;Cyc_Tcutil_failure_reason=((_tmpF7D="(only one function type takes C varargs)",_tag_dyneither(_tmpF7D,sizeof(char),41)));}
_npop_handler(0);goto _LL2A1;}{
# 1547
int bad_cyc_vararg=0;
{struct _tuple22 _tmpF7E;struct _tuple22 _stmttmp2B=(_tmpF7E.f1=_tmp441,((_tmpF7E.f2=_tmp437,_tmpF7E)));struct _tuple22 _tmp4A4=_stmttmp2B;struct _dyneither_ptr*_tmp4A5;struct Cyc_Absyn_Tqual _tmp4A6;void*_tmp4A7;int _tmp4A8;struct _dyneither_ptr*_tmp4A9;struct Cyc_Absyn_Tqual _tmp4AA;void*_tmp4AB;int _tmp4AC;_LL2E4: if(_tmp4A4.f1 != 0)goto _LL2E6;if(_tmp4A4.f2 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E3;_LL2E6: if(_tmp4A4.f1 != 0)goto _LL2E8;_LL2E7:
 goto _LL2E9;_LL2E8: if(_tmp4A4.f2 != 0)goto _LL2EA;_LL2E9:
# 1552
 bad_cyc_vararg=1;
{const char*_tmpF7F;Cyc_Tcutil_failure_reason=((_tmpF7F="(only one function type takes varargs)",_tag_dyneither(_tmpF7F,sizeof(char),39)));}
goto _LL2E3;_LL2EA: if(_tmp4A4.f1 == 0)goto _LL2E3;_tmp4A5=(_tmp4A4.f1)->name;_tmp4A6=(_tmp4A4.f1)->tq;_tmp4A7=(_tmp4A4.f1)->type;_tmp4A8=(_tmp4A4.f1)->inject;if(_tmp4A4.f2 == 0)goto _LL2E3;_tmp4A9=(_tmp4A4.f2)->name;_tmp4AA=(_tmp4A4.f2)->tq;_tmp4AB=(_tmp4A4.f2)->type;_tmp4AC=(_tmp4A4.f2)->inject;_LL2EB:
# 1556
 Cyc_Tcutil_unify_it(_tmp4A7,_tmp4AB);
Cyc_Tcutil_unify_tqual(_tmp4A6,_tmp4A7,_tmp4AA,_tmp4AB);
if(_tmp4A8 != _tmp4AC){
bad_cyc_vararg=1;{
const char*_tmpF80;Cyc_Tcutil_failure_reason=((_tmpF80="(only one function type injects varargs)",_tag_dyneither(_tmpF80,sizeof(char),41)));};}
# 1562
goto _LL2E3;_LL2E3:;}
# 1564
if(bad_cyc_vararg){_npop_handler(0);goto _LL2A1;}{
# 1567
int bad_effect=0;
{struct _tuple0 _tmpF81;struct _tuple0 _stmttmp2C=(_tmpF81.f1=_tmp43C,((_tmpF81.f2=_tmp432,_tmpF81)));struct _tuple0 _tmp4B0=_stmttmp2C;_LL2ED: if(_tmp4B0.f1 != 0)goto _LL2EF;if(_tmp4B0.f2 != 0)goto _LL2EF;_LL2EE:
 goto _LL2EC;_LL2EF: if(_tmp4B0.f1 != 0)goto _LL2F1;_LL2F0:
 goto _LL2F2;_LL2F1: if(_tmp4B0.f2 != 0)goto _LL2F3;_LL2F2:
 bad_effect=1;goto _LL2EC;_LL2F3:;_LL2F4:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp43C),(void*)_check_null(_tmp432));goto _LL2EC;_LL2EC:;}
# 1574
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmpF82;Cyc_Tcutil_failure_reason=((_tmpF82="(function type effects do not unify)",_tag_dyneither(_tmpF82,sizeof(char),37)));}
_npop_handler(0);goto _LL2A1;}
# 1580
if(!Cyc_Tcutil_same_atts(_tmp439,_tmp443)){
{const char*_tmpF83;Cyc_Tcutil_failure_reason=((_tmpF83="(function types have different attributes)",_tag_dyneither(_tmpF83,sizeof(char),43)));}
_npop_handler(0);goto _LL2A1;}
# 1584
if(!Cyc_Tcutil_same_rgn_po(_tmp438,_tmp442)){
{const char*_tmpF84;Cyc_Tcutil_failure_reason=((_tmpF84="(function types have different region lifetime orderings)",_tag_dyneither(_tmpF84,sizeof(char),58)));}
_npop_handler(0);goto _LL2A1;}
# 1588
_npop_handler(0);return;};};
# 1493
;_pop_region(rgn);}_LL2C0:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp444=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp444->tag != 10)goto _LL2C2;else{_tmp445=_tmp444->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp446=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp446->tag != 10)goto _LL2C2;else{_tmp447=_tmp446->f1;}};_LL2C1:
# 1591
 for(0;_tmp447 != 0  && _tmp445 != 0;(_tmp447=_tmp447->tl,_tmp445=_tmp445->tl)){
struct _tuple12 _stmttmp27=*((struct _tuple12*)_tmp447->hd);struct Cyc_Absyn_Tqual _tmp4B6;void*_tmp4B7;struct _tuple12 _tmp4B5=_stmttmp27;_tmp4B6=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;{
struct _tuple12 _stmttmp28=*((struct _tuple12*)_tmp445->hd);struct Cyc_Absyn_Tqual _tmp4B9;void*_tmp4BA;struct _tuple12 _tmp4B8=_stmttmp28;_tmp4B9=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;
Cyc_Tcutil_unify_it(_tmp4B7,_tmp4BA);
Cyc_Tcutil_unify_tqual(_tmp4B6,_tmp4B7,_tmp4B9,_tmp4BA);};}
# 1597
if(_tmp447 == 0  && _tmp445 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF85;Cyc_Tcutil_failure_reason=((_tmpF85="(tuple types have different numbers of components)",_tag_dyneither(_tmpF85,sizeof(char),51)));}
goto _LL2A1;_LL2C2:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp448=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp448->tag != 12)goto _LL2C4;else{_tmp449=_tmp448->f1;_tmp44A=_tmp448->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp44B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp44B->tag != 12)goto _LL2C4;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;}};_LL2C3:
# 1604
 if(_tmp44C != _tmp449){{const char*_tmpF86;Cyc_Tcutil_failure_reason=((_tmpF86="(struct and union type)",_tag_dyneither(_tmpF86,sizeof(char),24)));}goto _LL2A1;}
for(0;_tmp44D != 0  && _tmp44A != 0;(_tmp44D=_tmp44D->tl,_tmp44A=_tmp44A->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4BD=(struct Cyc_Absyn_Aggrfield*)_tmp44D->hd;
struct Cyc_Absyn_Aggrfield*_tmp4BE=(struct Cyc_Absyn_Aggrfield*)_tmp44A->hd;
if(Cyc_strptrcmp(_tmp4BD->name,_tmp4BE->name)!= 0){
{const char*_tmpF87;Cyc_Tcutil_failure_reason=((_tmpF87="(different member names)",_tag_dyneither(_tmpF87,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1612
Cyc_Tcutil_unify_it(_tmp4BD->type,_tmp4BE->type);
Cyc_Tcutil_unify_tqual(_tmp4BD->tq,_tmp4BD->type,_tmp4BE->tq,_tmp4BE->type);
if(!Cyc_Tcutil_same_atts(_tmp4BD->attributes,_tmp4BE->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF88;Cyc_Tcutil_failure_reason=((_tmpF88="(different attributes on member)",_tag_dyneither(_tmpF88,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1620
if((_tmp4BD->width != 0  && _tmp4BE->width == 0  || 
_tmp4BE->width != 0  && _tmp4BD->width == 0) || 
(_tmp4BD->width != 0  && _tmp4BE->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4BD->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4BE->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF89;Cyc_Tcutil_failure_reason=((_tmpF89="(different bitfield widths on member)",_tag_dyneither(_tmpF89,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1629
if((_tmp4BD->requires_clause != 0  && _tmp4BE->requires_clause == 0  || 
_tmp4BD->requires_clause == 0  && _tmp4BE->requires_clause != 0) || 
(_tmp4BD->requires_clause == 0  && _tmp4BE->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4BD->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp4BE->requires_clause))){
# 1634
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF8A;Cyc_Tcutil_failure_reason=((_tmpF8A="(different @requires clauses on member)",_tag_dyneither(_tmpF8A,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1640
if(_tmp44D == 0  && _tmp44A == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmpF8B;Cyc_Tcutil_failure_reason=((_tmpF8B="(different number of members)",_tag_dyneither(_tmpF8B,sizeof(char),30)));}
goto _LL2A1;_LL2C4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp44E->tag != 17)goto _LL2C6;else{_tmp44F=_tmp44E->f2;_tmp450=_tmp44E->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp451=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp451->tag != 17)goto _LL2C6;else{_tmp452=_tmp451->f2;_tmp453=_tmp451->f3;}};_LL2C5:
# 1646
 if(_tmp450 != _tmp453){
{const char*_tmpF8C;Cyc_Tcutil_failure_reason=((_tmpF8C="(different abstract typedefs)",_tag_dyneither(_tmpF8C,sizeof(char),30)));}
goto _LL2A1;}
# 1650
{const char*_tmpF8D;Cyc_Tcutil_failure_reason=((_tmpF8D="(type parameters to typedef differ)",_tag_dyneither(_tmpF8D,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp44F,_tmp452);
return;_LL2C6:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp454->tag != 20)goto _LL2C8;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp455->tag != 20)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp456->tag != 21)goto _LL2CA;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp457->tag != 21)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp458->tag != 22)goto _LL2CC;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp459->tag != 22)goto _LL2CC;};_LL2CB:
 return;_LL2CC:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp45A->tag != 15)goto _LL2CE;else{_tmp45B=(void*)_tmp45A->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp45C->tag != 15)goto _LL2CE;else{_tmp45D=(void*)_tmp45C->f1;}};_LL2CD:
# 1657
 Cyc_Tcutil_unify_it(_tmp45B,_tmp45D);
return;_LL2CE:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp45E->tag != 16)goto _LL2D0;else{_tmp45F=(void*)_tmp45E->f1;_tmp460=(void*)_tmp45E->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp461->tag != 16)goto _LL2D0;else{_tmp462=(void*)_tmp461->f1;_tmp463=(void*)_tmp461->f2;}};_LL2CF:
# 1660
 Cyc_Tcutil_unify_it(_tmp45F,_tmp462);
Cyc_Tcutil_unify_it(_tmp460,_tmp463);
return;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp464->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp465=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp465->tag != 24)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp466->tag != 23)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp467=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E2.f1;if(_tmp467->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp468->tag != 25)goto _LL2DA;}_LL2D9:
 goto _LL2DB;_LL2DA: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3E2.f2;if(_tmp469->tag != 23)goto _LL2DC;}_LL2DB:
# 1669
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmpF8E;Cyc_Tcutil_failure_reason=((_tmpF8E="(effects don't unify)",_tag_dyneither(_tmpF8E,sizeof(char),22)));}
goto _LL2A1;_LL2DC:;_LL2DD:
 goto _LL2A1;_LL2A1:;}
# 1674
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1677
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1684
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp4C8=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4C9=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4C8,_tmp4C9);}
# 1691
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4CA=x;void*_tmp4CB;_LL2F6: if((_tmp4CA->No_constr).tag != 3)goto _LL2F8;_LL2F7:
 return - 1;_LL2F8: if((_tmp4CA->Eq_constr).tag != 1)goto _LL2FA;_tmp4CB=(void*)(_tmp4CA->Eq_constr).val;_LL2F9: {
# 1698
union Cyc_Absyn_Constraint*_tmp4CC=y;void*_tmp4CD;_LL2FD: if((_tmp4CC->No_constr).tag != 3)goto _LL2FF;_LL2FE:
 return 1;_LL2FF: if((_tmp4CC->Eq_constr).tag != 1)goto _LL301;_tmp4CD=(void*)(_tmp4CC->Eq_constr).val;_LL300:
 return cmp(_tmp4CB,_tmp4CD);_LL301: if((_tmp4CC->Forward_constr).tag != 2)goto _LL2FC;_LL302: {
const char*_tmpF91;void*_tmpF90;(_tmpF90=0,Cyc_Tcutil_impos(((_tmpF91="unify_conref: forward after compress(2)",_tag_dyneither(_tmpF91,sizeof(char),40))),_tag_dyneither(_tmpF90,sizeof(void*),0)));}_LL2FC:;}_LL2FA: if((_tmp4CA->Forward_constr).tag != 2)goto _LL2F5;_LL2FB: {
# 1703
const char*_tmpF94;void*_tmpF93;(_tmpF93=0,Cyc_Tcutil_impos(((_tmpF94="unify_conref: forward after compress",_tag_dyneither(_tmpF94,sizeof(char),37))),_tag_dyneither(_tmpF93,sizeof(void*),0)));}_LL2F5:;};}
# 1707
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp4D3;void*_tmp4D4;struct _tuple12*_tmp4D2=tqt1;_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;{
struct Cyc_Absyn_Tqual _tmp4D6;void*_tmp4D7;struct _tuple12*_tmp4D5=tqt2;_tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;{
int _tmp4D8=Cyc_Tcutil_tqual_cmp(_tmp4D3,_tmp4D6);
if(_tmp4D8 != 0)return _tmp4D8;
return Cyc_Tcutil_typecmp(_tmp4D4,_tmp4D7);};};}
# 1715
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp4D9=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp4D9 != 0)return _tmp4D9;{
int _tmp4DA=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp4DA != 0)return _tmp4DA;{
int _tmp4DB=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp4DB != 0)return _tmp4DB;{
int _tmp4DC=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp4DC != 0)return _tmp4DC;
_tmp4DC=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp4DC != 0)return _tmp4DC;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1729
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp4DD=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp4DD != 0)return _tmp4DD;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1735
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp4DE=t;_LL304: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4DF=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4DE;if(_tmp4DF->tag != 0)goto _LL306;}_LL305:
 return 0;_LL306: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4E0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4DE;if(_tmp4E0->tag != 1)goto _LL308;}_LL307:
 return 1;_LL308: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E1->tag != 2)goto _LL30A;}_LL309:
 return 2;_LL30A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4E2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E2->tag != 3)goto _LL30C;}_LL30B:
 return 3;_LL30C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E3->tag != 4)goto _LL30E;}_LL30D:
 return 4;_LL30E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4E4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E4->tag != 5)goto _LL310;}_LL30F:
 return 5;_LL310: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4E5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E5->tag != 6)goto _LL312;}_LL311:
 return 6;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E6->tag != 7)goto _LL314;else{if(_tmp4E6->f1 != 0)goto _LL314;}}_LL313:
 return 7;_LL314: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4E7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E7->tag != 7)goto _LL316;}_LL315:
 return 8;_LL316: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4E8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E8->tag != 8)goto _LL318;}_LL317:
 return 9;_LL318: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4DE;if(_tmp4E9->tag != 9)goto _LL31A;}_LL319:
 return 10;_LL31A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4EA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EA->tag != 10)goto _LL31C;}_LL31B:
 return 11;_LL31C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4EB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EB->tag != 11)goto _LL31E;}_LL31D:
 return 12;_LL31E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EC->tag != 12)goto _LL320;}_LL31F:
 return 14;_LL320: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4DE;if(_tmp4ED->tag != 13)goto _LL322;}_LL321:
 return 16;_LL322: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EE->tag != 14)goto _LL324;}_LL323:
 return 17;_LL324: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4DE;if(_tmp4EF->tag != 15)goto _LL326;}_LL325:
 return 18;_LL326: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4F0=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F0->tag != 17)goto _LL328;}_LL327:
 return 19;_LL328: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4F1=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4DE;if(_tmp4F1->tag != 21)goto _LL32A;}_LL329:
 return 20;_LL32A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4DE;if(_tmp4F2->tag != 20)goto _LL32C;}_LL32B:
 return 21;_LL32C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4DE;if(_tmp4F3->tag != 23)goto _LL32E;}_LL32D:
 return 22;_LL32E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4DE;if(_tmp4F4->tag != 24)goto _LL330;}_LL32F:
 return 23;_LL330: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4DE;if(_tmp4F5->tag != 25)goto _LL332;}_LL331:
 return 24;_LL332: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4DE;if(_tmp4F6->tag != 22)goto _LL334;}_LL333:
 return 25;_LL334: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F7->tag != 19)goto _LL336;}_LL335:
 return 26;_LL336: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F8->tag != 16)goto _LL338;}_LL337:
 return 27;_LL338: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4DE;if(_tmp4F9->tag != 18)goto _LL33A;}_LL339:
 return 28;_LL33A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4DE;if(_tmp4FA->tag != 26)goto _LL303;}_LL33B:
 return 29;_LL303:;}
# 1770
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp4FB=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp4FB != 0)
return _tmp4FB;{
# 1779
struct _tuple0 _tmpF95;struct _tuple0 _stmttmp30=(_tmpF95.f1=t2,((_tmpF95.f2=t1,_tmpF95)));struct _tuple0 _tmp4FC=_stmttmp30;struct Cyc_Absyn_Tvar*_tmp500;struct Cyc_Absyn_Tvar*_tmp502;union Cyc_Absyn_AggrInfoU _tmp504;struct Cyc_List_List*_tmp505;union Cyc_Absyn_AggrInfoU _tmp507;struct Cyc_List_List*_tmp508;struct _tuple2*_tmp50A;struct _tuple2*_tmp50C;struct Cyc_List_List*_tmp50E;struct Cyc_List_List*_tmp510;struct Cyc_Absyn_Datatypedecl*_tmp512;struct Cyc_List_List*_tmp513;struct Cyc_Absyn_Datatypedecl*_tmp515;struct Cyc_List_List*_tmp516;struct Cyc_Absyn_Datatypedecl*_tmp518;struct Cyc_Absyn_Datatypefield*_tmp519;struct Cyc_List_List*_tmp51A;struct Cyc_Absyn_Datatypedecl*_tmp51C;struct Cyc_Absyn_Datatypefield*_tmp51D;struct Cyc_List_List*_tmp51E;void*_tmp520;struct Cyc_Absyn_Tqual _tmp521;void*_tmp522;union Cyc_Absyn_Constraint*_tmp523;union Cyc_Absyn_Constraint*_tmp524;union Cyc_Absyn_Constraint*_tmp525;void*_tmp527;struct Cyc_Absyn_Tqual _tmp528;void*_tmp529;union Cyc_Absyn_Constraint*_tmp52A;union Cyc_Absyn_Constraint*_tmp52B;union Cyc_Absyn_Constraint*_tmp52C;enum Cyc_Absyn_Sign _tmp52E;enum Cyc_Absyn_Size_of _tmp52F;enum Cyc_Absyn_Sign _tmp531;enum Cyc_Absyn_Size_of _tmp532;int _tmp534;int _tmp536;void*_tmp538;struct Cyc_Absyn_Tqual _tmp539;struct Cyc_Absyn_Exp*_tmp53A;union Cyc_Absyn_Constraint*_tmp53B;void*_tmp53D;struct Cyc_Absyn_Tqual _tmp53E;struct Cyc_Absyn_Exp*_tmp53F;union Cyc_Absyn_Constraint*_tmp540;struct Cyc_List_List*_tmp542;void*_tmp543;struct Cyc_Absyn_Tqual _tmp544;void*_tmp545;struct Cyc_List_List*_tmp546;int _tmp547;struct Cyc_Absyn_VarargInfo*_tmp548;struct Cyc_List_List*_tmp549;struct Cyc_List_List*_tmp54A;struct Cyc_List_List*_tmp54C;void*_tmp54D;struct Cyc_Absyn_Tqual _tmp54E;void*_tmp54F;struct Cyc_List_List*_tmp550;int _tmp551;struct Cyc_Absyn_VarargInfo*_tmp552;struct Cyc_List_List*_tmp553;struct Cyc_List_List*_tmp554;struct Cyc_List_List*_tmp556;struct Cyc_List_List*_tmp558;enum Cyc_Absyn_AggrKind _tmp55A;struct Cyc_List_List*_tmp55B;enum Cyc_Absyn_AggrKind _tmp55D;struct Cyc_List_List*_tmp55E;void*_tmp560;void*_tmp562;void*_tmp564;void*_tmp565;void*_tmp567;void*_tmp568;void*_tmp56A;void*_tmp56C;struct Cyc_Absyn_Exp*_tmp56E;struct Cyc_Absyn_Exp*_tmp570;_LL33D:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp4FD->tag != 1)goto _LL33F;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp4FE->tag != 1)goto _LL33F;};_LL33E: {
# 1781
const char*_tmpF98;void*_tmpF97;(_tmpF97=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF98="typecmp: can only compare closed types",_tag_dyneither(_tmpF98,sizeof(char),39))),_tag_dyneither(_tmpF97,sizeof(void*),0)));}_LL33F:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp4FF->tag != 2)goto _LL341;else{_tmp500=_tmp4FF->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp501=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp501->tag != 2)goto _LL341;else{_tmp502=_tmp501->f1;}};_LL340:
# 1785
 return Cyc_Core_intcmp(_tmp502->identity,_tmp500->identity);_LL341:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp503=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp503->tag != 11)goto _LL343;else{_tmp504=(_tmp503->f1).aggr_info;_tmp505=(_tmp503->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp506->tag != 11)goto _LL343;else{_tmp507=(_tmp506->f1).aggr_info;_tmp508=(_tmp506->f1).targs;}};_LL342: {
# 1789
struct _tuple13 _stmttmp32=Cyc_Absyn_aggr_kinded_name(_tmp504);struct _tuple2*_tmp57A;struct _tuple13 _tmp579=_stmttmp32;_tmp57A=_tmp579.f2;{
struct _tuple13 _stmttmp33=Cyc_Absyn_aggr_kinded_name(_tmp507);struct _tuple2*_tmp57C;struct _tuple13 _tmp57B=_stmttmp33;_tmp57C=_tmp57B.f2;{
int _tmp57D=Cyc_Absyn_qvar_cmp(_tmp57A,_tmp57C);
if(_tmp57D != 0)return _tmp57D;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp505,_tmp508);}};};}_LL343:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp509=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp509->tag != 13)goto _LL345;else{_tmp50A=_tmp509->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp50B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp50B->tag != 13)goto _LL345;else{_tmp50C=_tmp50B->f1;}};_LL344:
# 1796
 return Cyc_Absyn_qvar_cmp(_tmp50A,_tmp50C);_LL345:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp50D->tag != 14)goto _LL347;else{_tmp50E=_tmp50D->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp50F->tag != 14)goto _LL347;else{_tmp510=_tmp50F->f1;}};_LL346:
# 1799
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp50E,_tmp510);_LL347:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp511->tag != 3)goto _LL349;else{if((((_tmp511->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp512=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp511->f1).datatype_info).KnownDatatype).val);_tmp513=(_tmp511->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp514=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp514->tag != 3)goto _LL349;else{if((((_tmp514->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp515=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp514->f1).datatype_info).KnownDatatype).val);_tmp516=(_tmp514->f1).targs;}};_LL348:
# 1803
 if(_tmp515 == _tmp512)return 0;{
int _tmp57E=Cyc_Absyn_qvar_cmp(_tmp515->name,_tmp512->name);
if(_tmp57E != 0)return _tmp57E;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp516,_tmp513);};_LL349:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp517=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp517->tag != 4)goto _LL34B;else{if((((_tmp517->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp518=((struct _tuple3)(((_tmp517->f1).field_info).KnownDatatypefield).val).f1;_tmp519=((struct _tuple3)(((_tmp517->f1).field_info).KnownDatatypefield).val).f2;_tmp51A=(_tmp517->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp51B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp51B->tag != 4)goto _LL34B;else{if((((_tmp51B->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp51C=((struct _tuple3)(((_tmp51B->f1).field_info).KnownDatatypefield).val).f1;_tmp51D=((struct _tuple3)(((_tmp51B->f1).field_info).KnownDatatypefield).val).f2;_tmp51E=(_tmp51B->f1).targs;}};_LL34A:
# 1810
 if(_tmp51C == _tmp518)return 0;{
int _tmp57F=Cyc_Absyn_qvar_cmp(_tmp518->name,_tmp51C->name);
if(_tmp57F != 0)return _tmp57F;{
int _tmp580=Cyc_Absyn_qvar_cmp(_tmp519->name,_tmp51D->name);
if(_tmp580 != 0)return _tmp580;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp51E,_tmp51A);};};_LL34B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp51F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp51F->tag != 5)goto _LL34D;else{_tmp520=(_tmp51F->f1).elt_typ;_tmp521=(_tmp51F->f1).elt_tq;_tmp522=((_tmp51F->f1).ptr_atts).rgn;_tmp523=((_tmp51F->f1).ptr_atts).nullable;_tmp524=((_tmp51F->f1).ptr_atts).bounds;_tmp525=((_tmp51F->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp526=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp526->tag != 5)goto _LL34D;else{_tmp527=(_tmp526->f1).elt_typ;_tmp528=(_tmp526->f1).elt_tq;_tmp529=((_tmp526->f1).ptr_atts).rgn;_tmp52A=((_tmp526->f1).ptr_atts).nullable;_tmp52B=((_tmp526->f1).ptr_atts).bounds;_tmp52C=((_tmp526->f1).ptr_atts).zero_term;}};_LL34C: {
# 1819
int _tmp581=Cyc_Tcutil_typecmp(_tmp527,_tmp520);
if(_tmp581 != 0)return _tmp581;{
int _tmp582=Cyc_Tcutil_typecmp(_tmp529,_tmp522);
if(_tmp582 != 0)return _tmp582;{
int _tmp583=Cyc_Tcutil_tqual_cmp(_tmp528,_tmp521);
if(_tmp583 != 0)return _tmp583;{
int _tmp584=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp52B,_tmp524);
if(_tmp584 != 0)return _tmp584;{
int _tmp585=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52C,_tmp525);
if(_tmp585 != 0)return _tmp585;
{void*_stmttmp31=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp52B);void*_tmp586=_stmttmp31;_LL370: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp587=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp586;if(_tmp587->tag != 0)goto _LL372;}_LL371:
 return 0;_LL372:;_LL373:
 goto _LL36F;_LL36F:;}
# 1833
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52A,_tmp523);};};};};}_LL34D:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp52D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp52D->tag != 6)goto _LL34F;else{_tmp52E=_tmp52D->f1;_tmp52F=_tmp52D->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp530=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp530->tag != 6)goto _LL34F;else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}};_LL34E:
# 1836
 if(_tmp531 != _tmp52E)return Cyc_Core_intcmp((int)((unsigned int)_tmp531),(int)((unsigned int)_tmp52E));
if(_tmp532 != _tmp52F)return Cyc_Core_intcmp((int)((unsigned int)_tmp532),(int)((unsigned int)_tmp52F));
return 0;_LL34F:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp533=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp533->tag != 7)goto _LL351;else{_tmp534=_tmp533->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp535=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp535->tag != 7)goto _LL351;else{_tmp536=_tmp535->f1;}};_LL350:
# 1841
 if(_tmp534 == _tmp536)return 0;else{
if(_tmp536 == 0)return - 1;else{
if(_tmp536 == 1  && _tmp534 == 0)return - 1;else{
return 1;}}}_LL351:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp537=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp537->tag != 8)goto _LL353;else{_tmp538=(_tmp537->f1).elt_type;_tmp539=(_tmp537->f1).tq;_tmp53A=(_tmp537->f1).num_elts;_tmp53B=(_tmp537->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp53C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp53C->tag != 8)goto _LL353;else{_tmp53D=(_tmp53C->f1).elt_type;_tmp53E=(_tmp53C->f1).tq;_tmp53F=(_tmp53C->f1).num_elts;_tmp540=(_tmp53C->f1).zero_term;}};_LL352: {
# 1848
int _tmp588=Cyc_Tcutil_tqual_cmp(_tmp53E,_tmp539);
if(_tmp588 != 0)return _tmp588;{
int _tmp589=Cyc_Tcutil_typecmp(_tmp53D,_tmp538);
if(_tmp589 != 0)return _tmp589;{
int _tmp58A=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp53B,_tmp540);
if(_tmp58A != 0)return _tmp58A;
if(_tmp53A == _tmp53F)return 0;
if(_tmp53A == 0  || _tmp53F == 0){
const char*_tmpF9B;void*_tmpF9A;(_tmpF9A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF9B="missing expression in array index",_tag_dyneither(_tmpF9B,sizeof(char),34))),_tag_dyneither(_tmpF9A,sizeof(void*),0)));}
# 1858
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp53A,_tmp53F);};};}_LL353:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp541=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp541->tag != 9)goto _LL355;else{_tmp542=(_tmp541->f1).tvars;_tmp543=(_tmp541->f1).effect;_tmp544=(_tmp541->f1).ret_tqual;_tmp545=(_tmp541->f1).ret_typ;_tmp546=(_tmp541->f1).args;_tmp547=(_tmp541->f1).c_varargs;_tmp548=(_tmp541->f1).cyc_varargs;_tmp549=(_tmp541->f1).rgn_po;_tmp54A=(_tmp541->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp54B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp54B->tag != 9)goto _LL355;else{_tmp54C=(_tmp54B->f1).tvars;_tmp54D=(_tmp54B->f1).effect;_tmp54E=(_tmp54B->f1).ret_tqual;_tmp54F=(_tmp54B->f1).ret_typ;_tmp550=(_tmp54B->f1).args;_tmp551=(_tmp54B->f1).c_varargs;_tmp552=(_tmp54B->f1).cyc_varargs;_tmp553=(_tmp54B->f1).rgn_po;_tmp554=(_tmp54B->f1).attributes;}};_LL354: {
# 1862
const char*_tmpF9E;void*_tmpF9D;(_tmpF9D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF9E="typecmp: function types not handled",_tag_dyneither(_tmpF9E,sizeof(char),36))),_tag_dyneither(_tmpF9D,sizeof(void*),0)));}_LL355:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp555=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp555->tag != 10)goto _LL357;else{_tmp556=_tmp555->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp557=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp557->tag != 10)goto _LL357;else{_tmp558=_tmp557->f1;}};_LL356:
# 1865
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp558,_tmp556);_LL357:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp559->tag != 12)goto _LL359;else{_tmp55A=_tmp559->f1;_tmp55B=_tmp559->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp55C->tag != 12)goto _LL359;else{_tmp55D=_tmp55C->f1;_tmp55E=_tmp55C->f2;}};_LL358:
# 1868
 if(_tmp55D != _tmp55A){
if(_tmp55D == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp55E,_tmp55B);_LL359:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp55F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp55F->tag != 15)goto _LL35B;else{_tmp560=(void*)_tmp55F->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp561->tag != 15)goto _LL35B;else{_tmp562=(void*)_tmp561->f1;}};_LL35A:
# 1873
 return Cyc_Tcutil_typecmp(_tmp560,_tmp562);_LL35B:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp563->tag != 16)goto _LL35D;else{_tmp564=(void*)_tmp563->f1;_tmp565=(void*)_tmp563->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp566->tag != 16)goto _LL35D;else{_tmp567=(void*)_tmp566->f1;_tmp568=(void*)_tmp566->f2;}};_LL35C: {
# 1875
int _tmp58F=Cyc_Tcutil_typecmp(_tmp564,_tmp567);
if(_tmp58F != 0)return _tmp58F;else{
return Cyc_Tcutil_typecmp(_tmp565,_tmp568);}}_LL35D:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp569=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp569->tag != 19)goto _LL35F;else{_tmp56A=(void*)_tmp569->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp56B->tag != 19)goto _LL35F;else{_tmp56C=(void*)_tmp56B->f1;}};_LL35E:
 return Cyc_Tcutil_typecmp(_tmp56A,_tmp56C);_LL35F:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp56D->tag != 18)goto _LL361;else{_tmp56E=_tmp56D->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp56F=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp56F->tag != 18)goto _LL361;else{_tmp570=_tmp56F->f1;}};_LL360:
# 1880
 return Cyc_Evexp_const_exp_cmp(_tmp56E,_tmp570);_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp571=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp571->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp572->tag != 24)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp573->tag != 23)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FC.f1;if(_tmp574->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp575=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp575->tag != 25)goto _LL36B;}_LL36A:
 goto _LL36C;_LL36B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp576=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4FC.f2;if(_tmp576->tag != 23)goto _LL36D;}_LL36C: {
const char*_tmpFA1;void*_tmpFA0;(_tmpFA0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFA1="typecmp: effects not handled",_tag_dyneither(_tmpFA1,sizeof(char),29))),_tag_dyneither(_tmpFA0,sizeof(void*),0)));}_LL36D:;_LL36E: {
const char*_tmpFA4;void*_tmpFA3;(_tmpFA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFA4="Unmatched case in typecmp",_tag_dyneither(_tmpFA4,sizeof(char),26))),_tag_dyneither(_tmpFA3,sizeof(void*),0)));}_LL33C:;};};}
# 1891
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_stmttmp34=Cyc_Tcutil_compress(t);void*_tmp595=_stmttmp34;_LL375: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp595;if(_tmp596->tag != 6)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp597=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp595;if(_tmp597->tag != 7)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp598=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp595;if(_tmp598->tag != 13)goto _LL37B;}_LL37A:
 goto _LL37C;_LL37B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp599=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp595;if(_tmp599->tag != 14)goto _LL37D;}_LL37C:
 return 1;_LL37D:;_LL37E:
 return 0;_LL374:;}
# 1903
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFA5;struct _tuple0 _stmttmp35=(_tmpFA5.f1=t1,((_tmpFA5.f2=t2,_tmpFA5)));struct _tuple0 _tmp59A=_stmttmp35;int _tmp59C;int _tmp59E;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp59B->tag != 7)goto _LL382;else{_tmp59C=_tmp59B->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp59D->tag != 7)goto _LL382;else{_tmp59E=_tmp59D->f1;}};_LL381:
# 1908
 return _tmp59E == 0  && _tmp59C != 0  || (_tmp59E == 1  && _tmp59C != 0) && _tmp59C != 1;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp59F->tag != 7)goto _LL384;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5A0->tag != 6)goto _LL384;};_LL383:
 goto _LL385;_LL384:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5A1->tag != 7)goto _LL386;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5A2->tag != 19)goto _LL386;};_LL385:
 return 1;_LL386:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5A3->tag != 6)goto _LL388;else{if(_tmp5A3->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5A4->tag != 6)goto _LL388;else{if(_tmp5A4->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}};_LL387:
 return 0;_LL388: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5A5->tag != 6)goto _LL38A;else{if(_tmp5A5->f2 != Cyc_Absyn_LongLong_sz)goto _LL38A;}}_LL389:
 return 1;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5A6->tag != 6)goto _LL38C;else{if(_tmp5A6->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5A7->tag != 6)goto _LL38C;else{if(_tmp5A7->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}};_LL38B:
# 1915
 goto _LL38D;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5A8->tag != 6)goto _LL38E;else{if(_tmp5A8->f2 != Cyc_Absyn_Int_sz)goto _LL38E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5A9->tag != 6)goto _LL38E;else{if(_tmp5A9->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}};_LL38D:
 return 0;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5AA->tag != 6)goto _LL390;else{if(_tmp5AA->f2 != Cyc_Absyn_Long_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5AB->tag != 7)goto _LL390;else{if(_tmp5AB->f1 != 0)goto _LL390;}};_LL38F:
# 1918
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5AC->tag != 6)goto _LL392;else{if(_tmp5AC->f2 != Cyc_Absyn_Int_sz)goto _LL392;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5AD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5AD->tag != 7)goto _LL392;else{if(_tmp5AD->f1 != 0)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5AE->tag != 6)goto _LL394;else{if(_tmp5AE->f2 != Cyc_Absyn_Long_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5AF->tag != 6)goto _LL394;else{if(_tmp5AF->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5B0->tag != 6)goto _LL396;else{if(_tmp5B0->f2 != Cyc_Absyn_Int_sz)goto _LL396;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5B1->tag != 6)goto _LL396;else{if(_tmp5B1->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5B2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5B2->tag != 19)goto _LL398;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5B3->tag != 6)goto _LL398;else{if(_tmp5B3->f2 != Cyc_Absyn_Short_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5B4->tag != 6)goto _LL39A;else{if(_tmp5B4->f2 != Cyc_Absyn_Long_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5B5->tag != 6)goto _LL39A;else{if(_tmp5B5->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5B6->tag != 6)goto _LL39C;else{if(_tmp5B6->f2 != Cyc_Absyn_Int_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5B7->tag != 6)goto _LL39C;else{if(_tmp5B7->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5B8->tag != 6)goto _LL39E;else{if(_tmp5B8->f2 != Cyc_Absyn_Short_sz)goto _LL39E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5B9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5B9->tag != 6)goto _LL39E;else{if(_tmp5B9->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 goto _LL39F;_LL39E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59A.f1;if(_tmp5BA->tag != 19)goto _LL3A0;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5BB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp59A.f2;if(_tmp5BB->tag != 6)goto _LL3A0;else{if(_tmp5BB->f2 != Cyc_Absyn_Char_sz)goto _LL3A0;}};_LL39F:
 return 1;_LL3A0:;_LL3A1:
# 1928
 return 0;_LL37F:;};}
# 1934
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1937
struct _RegionHandle _tmp5BD=_new_region("r");struct _RegionHandle*r=& _tmp5BD;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmpFA6;max_arith_type=((_tmpFA6=_region_malloc(r,sizeof(*_tmpFA6)),((_tmpFA6->v=t1,_tmpFA6))));}}}}
# 1947
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5BF=0;_npop_handler(0);return _tmp5BF;}}}
# 1952
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmpFAB;void*_tmpFAA[2];struct Cyc_String_pa_PrintArg_struct _tmpFA9;struct Cyc_String_pa_PrintArg_struct _tmpFA8;(_tmpFA8.tag=0,((_tmpFA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmpFA9.tag=0,((_tmpFA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFAA[0]=& _tmpFA9,((_tmpFAA[1]=& _tmpFA8,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmpFAB="type mismatch: expecting %s but found %s",_tag_dyneither(_tmpFAB,sizeof(char),41))),_tag_dyneither(_tmpFAA,sizeof(void*),2)))))))))))));}{
int _tmp5C4=0;_npop_handler(0);return _tmp5C4;};}}}{
# 1958
int _tmp5C5=1;_npop_handler(0);return _tmp5C5;};
# 1937
;_pop_region(r);}
# 1963
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_stmttmp36=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5C6=_stmttmp36;_LL3A3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6;if(_tmp5C7->tag != 5)goto _LL3A5;}_LL3A4:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A2;_LL3A5:;_LL3A6:
 return 0;_LL3A2:;}
# 1969
return 1;}
# 1972
int Cyc_Tcutil_is_integral_type(void*t){
void*_stmttmp37=Cyc_Tcutil_compress(t);void*_tmp5C8=_stmttmp37;_LL3A8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5C9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C8;if(_tmp5C9->tag != 6)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5CA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5C8;if(_tmp5CA->tag != 19)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5CB=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5C8;if(_tmp5CB->tag != 13)goto _LL3AE;}_LL3AD:
 goto _LL3AF;_LL3AE: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5C8;if(_tmp5CC->tag != 14)goto _LL3B0;}_LL3AF:
 return 1;_LL3B0:;_LL3B1:
 return 0;_LL3A7:;}
# 1983
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1987
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmpFAE;void*_tmpFAD;(_tmpFAD=0,Cyc_Tcutil_warn(e->loc,((_tmpFAE="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFAE,sizeof(char),44))),_tag_dyneither(_tmpFAD,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 1993
return 0;}
# 1997
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2001
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmpFB1;void*_tmpFB0;(_tmpFB0=0,Cyc_Tcutil_warn(e->loc,((_tmpFB1="integral size mismatch; conversion supplied",_tag_dyneither(_tmpFB1,sizeof(char),44))),_tag_dyneither(_tmpFB0,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2007
return 0;}
# 2012
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2020
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2022
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmpFB2;struct _tuple0 _stmttmp38=(_tmpFB2.f1=t1,((_tmpFB2.f2=t2,_tmpFB2)));struct _tuple0 _tmp5D1=_stmttmp38;struct Cyc_Absyn_PtrInfo _tmp5D3;struct Cyc_Absyn_PtrInfo _tmp5D5;void*_tmp5D7;struct Cyc_Absyn_Tqual _tmp5D8;struct Cyc_Absyn_Exp*_tmp5D9;union Cyc_Absyn_Constraint*_tmp5DA;void*_tmp5DC;struct Cyc_Absyn_Tqual _tmp5DD;struct Cyc_Absyn_Exp*_tmp5DE;union Cyc_Absyn_Constraint*_tmp5DF;_LL3B3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D1.f1;if(_tmp5D2->tag != 5)goto _LL3B5;else{_tmp5D3=_tmp5D2->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D1.f2;if(_tmp5D4->tag != 5)goto _LL3B5;else{_tmp5D5=_tmp5D4->f1;}};_LL3B4: {
# 2026
int okay=1;
# 2028
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D3.ptr_atts).nullable,(_tmp5D5.ptr_atts).nullable))
# 2031
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D3.ptr_atts).nullable);
# 2033
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D3.ptr_atts).bounds,(_tmp5D5.ptr_atts).bounds)){
# 2036
struct _tuple0 _tmpFB3;struct _tuple0 _stmttmp39=(_tmpFB3.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D3.ptr_atts).bounds),((_tmpFB3.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp5D5.ptr_atts).bounds),_tmpFB3)));
# 2036
struct _tuple0 _tmp5E2=_stmttmp39;struct Cyc_Absyn_Exp*_tmp5E8;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Exp*_tmp5ED;_LL3BC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E3->tag != 1)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E4->tag != 0)goto _LL3BE;};_LL3BD:
# 2038
 goto _LL3BF;_LL3BE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E5->tag != 0)goto _LL3C0;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E6=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E6->tag != 0)goto _LL3C0;};_LL3BF:
 okay=1;goto _LL3BB;_LL3C0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5E7->tag != 1)goto _LL3C2;else{_tmp5E8=_tmp5E7->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5E9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5E9->tag != 1)goto _LL3C2;else{_tmp5EA=_tmp5E9->f1;}};_LL3C1:
# 2041
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp5EA,_tmp5E8);
# 2045
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D5.ptr_atts).zero_term)){
const char*_tmpFB6;void*_tmpFB5;(_tmpFB5=0,Cyc_Tcutil_warn(loc,((_tmpFB6="implicit cast to shorter array",_tag_dyneither(_tmpFB6,sizeof(char),31))),_tag_dyneither(_tmpFB5,sizeof(void*),0)));}
goto _LL3BB;_LL3C2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5EB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E2.f1;if(_tmp5EB->tag != 0)goto _LL3BB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5EC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5E2.f2;if(_tmp5EC->tag != 1)goto _LL3BB;else{_tmp5ED=_tmp5EC->f1;}};_LL3C3:
# 2050
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D3.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp5D5.ptr_atts).bounds))
goto _LL3BB;
okay=0;
goto _LL3BB;_LL3BB:;}
# 2059
okay=okay  && (!(_tmp5D3.elt_tq).real_const  || (_tmp5D5.elt_tq).real_const);
# 2062
if(!Cyc_Tcutil_unify((_tmp5D3.ptr_atts).rgn,(_tmp5D5.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp5D3.ptr_atts).rgn,(_tmp5D5.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmpFBB;void*_tmpFBA[2];struct Cyc_String_pa_PrintArg_struct _tmpFB9;struct Cyc_String_pa_PrintArg_struct _tmpFB8;(_tmpFB8.tag=0,((_tmpFB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2067
Cyc_Absynpp_typ2string((_tmp5D5.ptr_atts).rgn)),((_tmpFB9.tag=0,((_tmpFB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2066
Cyc_Absynpp_typ2string((_tmp5D3.ptr_atts).rgn)),((_tmpFBA[0]=& _tmpFB9,((_tmpFBA[1]=& _tmpFB8,Cyc_Tcutil_warn(loc,((_tmpFBB="implicit cast form region %s to region %s",_tag_dyneither(_tmpFBB,sizeof(char),42))),_tag_dyneither(_tmpFBA,sizeof(void*),2)))))))))))));}}else{
# 2068
okay=0;}}
# 2072
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp5D3.ptr_atts).zero_term,(_tmp5D5.ptr_atts).zero_term) || 
# 2075
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp5D3.ptr_atts).zero_term) && (_tmp5D5.elt_tq).real_const);{
# 2083
int _tmp5F5=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp5D5.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2086
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp5D5.ptr_atts).zero_term);
# 2089
okay=okay  && (Cyc_Tcutil_unify(_tmp5D3.elt_typ,_tmp5D5.elt_typ) || 
(_tmp5F5  && ((_tmp5D5.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp5D5.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp5D3.elt_typ,_tmp5D5.elt_typ));
# 2092
return okay;};}_LL3B5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5D6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D1.f1;if(_tmp5D6->tag != 8)goto _LL3B7;else{_tmp5D7=(_tmp5D6->f1).elt_type;_tmp5D8=(_tmp5D6->f1).tq;_tmp5D9=(_tmp5D6->f1).num_elts;_tmp5DA=(_tmp5D6->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5DB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D1.f2;if(_tmp5DB->tag != 8)goto _LL3B7;else{_tmp5DC=(_tmp5DB->f1).elt_type;_tmp5DD=(_tmp5DB->f1).tq;_tmp5DE=(_tmp5DB->f1).num_elts;_tmp5DF=(_tmp5DB->f1).zero_term;}};_LL3B6: {
# 2096
int okay;
# 2098
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5DA,_tmp5DF) && (
(_tmp5D9 != 0  && _tmp5DE != 0) && Cyc_Evexp_same_const_exp(_tmp5D9,_tmp5DE));
# 2101
return(okay  && Cyc_Tcutil_unify(_tmp5D7,_tmp5DC)) && (!_tmp5D8.real_const  || _tmp5DD.real_const);}_LL3B7:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5E0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D1.f1;if(_tmp5E0->tag != 19)goto _LL3B9;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D1.f2;if(_tmp5E1->tag != 6)goto _LL3B9;};_LL3B8:
# 2103
 return 0;_LL3B9:;_LL3BA:
# 2105
 return Cyc_Tcutil_unify(t1,t2);_LL3B2:;};}
# 2109
int Cyc_Tcutil_is_pointer_type(void*t){
void*_stmttmp3A=Cyc_Tcutil_compress(t);void*_tmp5F7=_stmttmp3A;_LL3C5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F7;if(_tmp5F8->tag != 5)goto _LL3C7;}_LL3C6:
 return 1;_LL3C7:;_LL3C8:
 return 0;_LL3C4:;}
# 2115
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_stmttmp3B=Cyc_Tcutil_compress(t);void*_tmp5F9=_stmttmp3B;void*_tmp5FB;_LL3CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F9;if(_tmp5FA->tag != 5)goto _LL3CC;else{_tmp5FB=(_tmp5FA->f1).elt_typ;}}_LL3CB:
 return _tmp5FB;_LL3CC:;_LL3CD: {
const char*_tmpFBE;void*_tmpFBD;(_tmpFBD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBE="pointer_elt_type",_tag_dyneither(_tmpFBE,sizeof(char),17))),_tag_dyneither(_tmpFBD,sizeof(void*),0)));}_LL3C9:;}
# 2121
void*Cyc_Tcutil_pointer_region(void*t){
void*_stmttmp3C=Cyc_Tcutil_compress(t);void*_tmp5FE=_stmttmp3C;struct Cyc_Absyn_PtrAtts*_tmp600;_LL3CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FE;if(_tmp5FF->tag != 5)goto _LL3D1;else{_tmp600=(struct Cyc_Absyn_PtrAtts*)&(_tmp5FF->f1).ptr_atts;}}_LL3D0:
 return _tmp600->rgn;_LL3D1:;_LL3D2: {
const char*_tmpFC1;void*_tmpFC0;(_tmpFC0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC1="pointer_elt_type",_tag_dyneither(_tmpFC1,sizeof(char),17))),_tag_dyneither(_tmpFC0,sizeof(void*),0)));}_LL3CE:;}
# 2128
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_stmttmp3D=Cyc_Tcutil_compress(t);void*_tmp603=_stmttmp3D;void*_tmp605;_LL3D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp604=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp603;if(_tmp604->tag != 5)goto _LL3D6;else{_tmp605=((_tmp604->f1).ptr_atts).rgn;}}_LL3D5:
# 2131
*rgn=_tmp605;
return 1;_LL3D6:;_LL3D7:
 return 0;_LL3D3:;}
# 2139
int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dyneither_ptr){
void*_stmttmp3E=Cyc_Tcutil_compress(t);void*_tmp606=_stmttmp3E;union Cyc_Absyn_Constraint*_tmp608;_LL3D9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp606;if(_tmp607->tag != 5)goto _LL3DB;else{_tmp608=((_tmp607->f1).ptr_atts).bounds;}}_LL3DA:
# 2142
*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp608)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL3DB:;_LL3DC:
# 2145
 return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;_LL3D8:;}
# 2150
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_stmttmp3F=e->r;void*_tmp609=_stmttmp3F;struct _dyneither_ptr _tmp60F;void*_tmp611;struct Cyc_Absyn_Exp*_tmp612;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60A->tag != 0)goto _LL3E0;else{if(((_tmp60A->f1).Int_c).tag != 5)goto _LL3E0;if(((struct _tuple7)((_tmp60A->f1).Int_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60B->tag != 0)goto _LL3E2;else{if(((_tmp60B->f1).Char_c).tag != 2)goto _LL3E2;if(((struct _tuple5)((_tmp60B->f1).Char_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60C->tag != 0)goto _LL3E4;else{if(((_tmp60C->f1).Short_c).tag != 4)goto _LL3E4;if(((struct _tuple6)((_tmp60C->f1).Short_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
 goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60D->tag != 0)goto _LL3E6;else{if(((_tmp60D->f1).LongLong_c).tag != 6)goto _LL3E6;if(((struct _tuple8)((_tmp60D->f1).LongLong_c).val).f2 != 0)goto _LL3E6;}}_LL3E5:
# 2156
 return 1;_LL3E6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp60E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp60E->tag != 0)goto _LL3E8;else{if(((_tmp60E->f1).Wchar_c).tag != 3)goto _LL3E8;_tmp60F=(struct _dyneither_ptr)((_tmp60E->f1).Wchar_c).val;}}_LL3E7: {
# 2158
unsigned long _tmp613=Cyc_strlen((struct _dyneither_ptr)_tmp60F);
int i=0;
if(_tmp613 >= 2  && *((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),1))== 'x'  && _tmp613 >= 3) && *((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp613;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp60F,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2168
return 0;}}_LL3E8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp610=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp609;if(_tmp610->tag != 13)goto _LL3EA;else{_tmp611=(void*)_tmp610->f1;_tmp612=_tmp610->f2;}}_LL3E9:
 return Cyc_Tcutil_is_zero(_tmp612) && Cyc_Tcutil_admits_zero(_tmp611);_LL3EA:;_LL3EB:
 return 0;_LL3DD:;}
# 2174
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2181
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2186
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2191
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2198
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2203
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2208
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp615;enum Cyc_Absyn_AliasQual _tmp616;struct Cyc_Absyn_Kind*_tmp614=ka;_tmp615=_tmp614->kind;_tmp616=_tmp614->aliasqual;
switch(_tmp616){case Cyc_Absyn_Aliasable: _LL3EC:
# 2212
 switch(_tmp615){case Cyc_Absyn_AnyKind: _LL3EE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3EF:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3F0:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3F1:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F2:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F3:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3ED:
# 2221
 switch(_tmp615){case Cyc_Absyn_AnyKind: _LL3F6:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F7:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F8:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F9:
 return& Cyc_Tcutil_urko;default: _LL3FA:
 break;}
# 2228
break;case Cyc_Absyn_Top: _LL3F5:
# 2230
 switch(_tmp615){case Cyc_Absyn_AnyKind: _LL3FD:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FE:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FF:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL400:
 return& Cyc_Tcutil_trko;default: _LL401:
 break;}
# 2237
break;}{
# 2239
const char*_tmpFC5;void*_tmpFC4[1];struct Cyc_String_pa_PrintArg_struct _tmpFC3;(_tmpFC3.tag=0,((_tmpFC3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFC4[0]=& _tmpFC3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC5="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmpFC5,sizeof(char),26))),_tag_dyneither(_tmpFC4,sizeof(void*),1)))))));};}
# 2242
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2250
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2257
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2262
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2267
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2272
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2277
enum Cyc_Absyn_KindQual _tmp61B;enum Cyc_Absyn_AliasQual _tmp61C;struct Cyc_Absyn_Kind*_tmp61A=ka;_tmp61B=_tmp61A->kind;_tmp61C=_tmp61A->aliasqual;
switch(_tmp61C){case Cyc_Absyn_Aliasable: _LL403:
# 2280
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL405:
 return& ab;case Cyc_Absyn_MemKind: _LL406:
 return& mb;case Cyc_Absyn_BoxKind: _LL407:
 return& bb;case Cyc_Absyn_RgnKind: _LL408:
 return& rb;case Cyc_Absyn_EffKind: _LL409:
 return& eb;case Cyc_Absyn_IntKind: _LL40A:
 return& ib;}case Cyc_Absyn_Unique: _LL404:
# 2289
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL40D:
 return& uab;case Cyc_Absyn_MemKind: _LL40E:
 return& umb;case Cyc_Absyn_BoxKind: _LL40F:
 return& ubb;case Cyc_Absyn_RgnKind: _LL410:
 return& urb;default: _LL411:
 break;}
# 2296
break;case Cyc_Absyn_Top: _LL40C:
# 2298
 switch(_tmp61B){case Cyc_Absyn_AnyKind: _LL414:
 return& tab;case Cyc_Absyn_MemKind: _LL415:
 return& tmb;case Cyc_Absyn_BoxKind: _LL416:
 return& tbb;case Cyc_Absyn_RgnKind: _LL417:
 return& trb;default: _LL418:
 break;}
# 2305
break;}{
# 2307
const char*_tmpFC9;void*_tmpFC8[1];struct Cyc_String_pa_PrintArg_struct _tmpFC7;(_tmpFC7.tag=0,((_tmpFC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmpFC8[0]=& _tmpFC7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC9="kind_to_b: bad kind %s\n",_tag_dyneither(_tmpFC9,sizeof(char),24))),_tag_dyneither(_tmpFC8,sizeof(void*),1)))))));};}
# 2310
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2313
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2315
struct Cyc_Core_Opt*_tmpFCA;return(_tmpFCA=_cycalloc(sizeof(*_tmpFCA)),((_tmpFCA->v=Cyc_Tcutil_kind_to_bound(k),_tmpFCA)));}
# 2320
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpFCD;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFCC;e1->r=(void*)((_tmpFCC=_cycalloc(sizeof(*_tmpFCC)),((_tmpFCC[0]=((_tmpFCD.tag=0,((_tmpFCD.f1=Cyc_Absyn_Null_c,_tmpFCD)))),_tmpFCC))));}{
struct Cyc_Core_Opt*_tmp631=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpFD3;struct Cyc_Absyn_PtrInfo _tmpFD2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpFD1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp632=(_tmpFD1=_cycalloc(sizeof(*_tmpFD1)),((_tmpFD1[0]=((_tmpFD3.tag=5,((_tmpFD3.f1=((_tmpFD2.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp631),((_tmpFD2.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmpFD2.ptr_atts=(
((_tmpFD2.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp631),(((_tmpFD2.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmpFD2.ptr_atts).bounds=
# 2328
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpFD2.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpFD2.ptr_atts).ptrloc=0,_tmpFD2.ptr_atts)))))))))),_tmpFD2)))))),_tmpFD3)))),_tmpFD1)));
e1->topt=(void*)_tmp632;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmpFD8;void*_tmpFD7[2];struct Cyc_String_pa_PrintArg_struct _tmpFD6;struct Cyc_String_pa_PrintArg_struct _tmpFD5;(_tmpFD5.tag=0,((_tmpFD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmpFD6.tag=0,((_tmpFD6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmpFD7[0]=& _tmpFD6,((_tmpFD7[1]=& _tmpFD5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD8="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmpFD8,sizeof(char),56))),_tag_dyneither(_tmpFD7,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2338
return 0;}
# 2341
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL41A: {
const char*_tmpFD9;return(_tmpFD9="unknown",_tag_dyneither(_tmpFD9,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL41B: {
const char*_tmpFDA;return(_tmpFDA="no coercion",_tag_dyneither(_tmpFDA,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41C: {
const char*_tmpFDB;return(_tmpFDB="null check",_tag_dyneither(_tmpFDB,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41D: {
const char*_tmpFDC;return(_tmpFDC="other coercion",_tag_dyneither(_tmpFDC,sizeof(char),15));}}}
# 2350
int Cyc_Tcutil_warn_alias_coerce=0;
# 2356
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2360
static int counter=0;
struct _dyneither_ptr*_tmpFE9;const char*_tmpFE8;void*_tmpFE7[1];struct Cyc_Int_pa_PrintArg_struct _tmpFE6;struct _tuple2*_tmpFE5;struct _tuple2*v=(_tmpFE5=_cycalloc(sizeof(*_tmpFE5)),((_tmpFE5->f1=Cyc_Absyn_Loc_n,((_tmpFE5->f2=((_tmpFE9=_cycalloc(sizeof(*_tmpFE9)),((_tmpFE9[0]=(struct _dyneither_ptr)((_tmpFE6.tag=1,((_tmpFE6.f1=(unsigned long)counter ++,((_tmpFE7[0]=& _tmpFE6,Cyc_aprintf(((_tmpFE8="__aliasvar%d",_tag_dyneither(_tmpFE8,sizeof(char),13))),_tag_dyneither(_tmpFE7,sizeof(void*),1)))))))),_tmpFE9)))),_tmpFE5)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpFEC;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpFEB;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmpFEB=_cycalloc(sizeof(*_tmpFEB)),((_tmpFEB[0]=((_tmpFEC.tag=4,((_tmpFEC.f1=vd,_tmpFEC)))),_tmpFEB)))),e->loc);
# 2369
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2371
{void*_stmttmp40=Cyc_Tcutil_compress(e_typ);void*_tmp63E=_stmttmp40;void*_tmp640;struct Cyc_Absyn_Tqual _tmp641;void*_tmp642;union Cyc_Absyn_Constraint*_tmp643;union Cyc_Absyn_Constraint*_tmp644;union Cyc_Absyn_Constraint*_tmp645;struct Cyc_Absyn_PtrLoc*_tmp646;_LL420: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp63F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63E;if(_tmp63F->tag != 5)goto _LL422;else{_tmp640=(_tmp63F->f1).elt_typ;_tmp641=(_tmp63F->f1).elt_tq;_tmp642=((_tmp63F->f1).ptr_atts).rgn;_tmp643=((_tmp63F->f1).ptr_atts).nullable;_tmp644=((_tmp63F->f1).ptr_atts).bounds;_tmp645=((_tmp63F->f1).ptr_atts).zero_term;_tmp646=((_tmp63F->f1).ptr_atts).ptrloc;}}_LL421:
# 2373
{void*_stmttmp41=Cyc_Tcutil_compress(_tmp642);void*_tmp647=_stmttmp41;void**_tmp649;struct Cyc_Core_Opt*_tmp64A;_LL425: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp648=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp647;if(_tmp648->tag != 1)goto _LL427;else{_tmp649=(void**)((void**)& _tmp648->f2);_tmp64A=_tmp648->f4;}}_LL426: {
# 2375
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFEF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFEE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp64B=(_tmpFEE=_cycalloc(sizeof(*_tmpFEE)),((_tmpFEE[0]=((_tmpFEF.tag=2,((_tmpFEF.f1=tv,_tmpFEF)))),_tmpFEE)));
*_tmp649=(void*)_tmp64B;
goto _LL424;}_LL427:;_LL428:
 goto _LL424;_LL424:;}
# 2380
goto _LL41F;_LL422:;_LL423:
 goto _LL41F;_LL41F:;}
# 2384
e->topt=0;
vd->initializer=e;{
# 2388
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e->loc);
# 2390
struct _tuple15 _tmpFF0;return(_tmpFF0.f1=d,((_tmpFF0.f2=ve,_tmpFF0)));};}
# 2395
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2398
struct _RegionHandle _tmp657=_new_region("r");struct _RegionHandle*r=& _tmp657;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2403
void*_stmttmp42=Cyc_Tcutil_compress(wants_typ);void*_tmp658=_stmttmp42;void*_tmp65A;_LL42A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp659=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp658;if(_tmp659->tag != 5)goto _LL42C;else{_tmp65A=((_tmp659->f1).ptr_atts).rgn;}}_LL42B: {
# 2405
void*_stmttmp43=Cyc_Tcutil_compress(_tmp65A);void*_tmp65C=_stmttmp43;_LL431: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp65D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp65C;if(_tmp65D->tag != 20)goto _LL433;}_LL432: {
int _tmp65E=0;_npop_handler(0);return _tmp65E;}_LL433:;_LL434: {
# 2408
struct Cyc_Absyn_Kind*_tmp65F=Cyc_Tcutil_typ_kind(_tmp65A);
int _tmp660=_tmp65F->kind == Cyc_Absyn_RgnKind  && _tmp65F->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp660;}_LL430:;}_LL42C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp65B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp658;if(_tmp65B->tag != 17)goto _LL42E;}_LL42D: {
# 2412
const char*_tmpFF5;void*_tmpFF4[2];struct Cyc_String_pa_PrintArg_struct _tmpFF3;struct Cyc_String_pa_PrintArg_struct _tmpFF2;(_tmpFF2.tag=0,((_tmpFF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmpFF3.tag=0,((_tmpFF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmpFF4[0]=& _tmpFF3,((_tmpFF4[1]=& _tmpFF2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFF5="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmpFF5,sizeof(char),42))),_tag_dyneither(_tmpFF4,sizeof(void*),2)))))))))))));}_LL42E:;_LL42F: {
int _tmp665=0;_npop_handler(0);return _tmp665;}_LL429:;}{
# 2417
int _tmp666=0;_npop_handler(0);return _tmp666;};
# 2399
;_pop_region(r);}
# 2421
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2426
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2428
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2430
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmpFFA;void*_tmpFF9[2];struct Cyc_String_pa_PrintArg_struct _tmpFF8;struct Cyc_String_pa_PrintArg_struct _tmpFF7;(_tmpFF7.tag=0,((_tmpFF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFF8.tag=0,((_tmpFF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFF9[0]=& _tmpFF8,((_tmpFF9[1]=& _tmpFF7,Cyc_Tcutil_warn(e->loc,((_tmpFFA="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmpFFA,sizeof(char),53))),_tag_dyneither(_tmpFF9,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2439
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp1000;void*_tmpFFF[3];struct Cyc_String_pa_PrintArg_struct _tmpFFE;struct Cyc_String_pa_PrintArg_struct _tmpFFD;struct Cyc_String_pa_PrintArg_struct _tmpFFC;(_tmpFFC.tag=0,((_tmpFFC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmpFFD.tag=0,((_tmpFFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpFFE.tag=0,((_tmpFFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpFFF[0]=& _tmpFFE,((_tmpFFF[1]=& _tmpFFD,((_tmpFFF[2]=& _tmpFFC,Cyc_Tcutil_warn(e->loc,((_tmp1000="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp1000,sizeof(char),40))),_tag_dyneither(_tmpFFF,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2446
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2454
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp1005;void*_tmp1004[2];struct Cyc_String_pa_PrintArg_struct _tmp1003;struct Cyc_String_pa_PrintArg_struct _tmp1002;(_tmp1002.tag=0,((_tmp1002.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1003.tag=0,((_tmp1003.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1004[0]=& _tmp1003,((_tmp1004[1]=& _tmp1002,Cyc_Tcutil_warn(e->loc,((_tmp1005="implicit cast from %s to %s",_tag_dyneither(_tmp1005,sizeof(char),28))),_tag_dyneither(_tmp1004,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2460
return 0;}}}}}
# 2467
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2470
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2474
int Cyc_Tcutil_coerceable(void*t){
void*_stmttmp44=Cyc_Tcutil_compress(t);void*_tmp674=_stmttmp44;_LL436: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp675=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp674;if(_tmp675->tag != 6)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp676=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp674;if(_tmp676->tag != 7)goto _LL43A;}_LL439:
 return 1;_LL43A:;_LL43B:
 return 0;_LL435:;}
# 2492 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2496
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2499
struct _tuple23 _stmttmp45=*env;struct Cyc_List_List*_tmp678;struct _RegionHandle*_tmp679;struct Cyc_Tcenv_Tenv*_tmp67A;int _tmp67B;struct _tuple23 _tmp677=_stmttmp45;_tmp678=_tmp677.f1;_tmp679=_tmp677.f2;_tmp67A=_tmp677.f3;_tmp67B=_tmp677.f4;{
# 2501
void*_tmp67C=_tmp678 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp679,_tmp678,x->type);
struct Cyc_List_List*_tmp67D=Cyc_Tcutil_flatten_typ(_tmp679,_tmp67B,_tmp67A,_tmp67C);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp67D)== 1){
struct _tuple12*_tmp1008;struct Cyc_List_List*_tmp1007;return(_tmp1007=_region_malloc(_tmp679,sizeof(*_tmp1007)),((_tmp1007->hd=((_tmp1008=_region_malloc(_tmp679,sizeof(*_tmp1008)),((_tmp1008->f1=x->tq,((_tmp1008->f2=_tmp67C,_tmp1008)))))),((_tmp1007->tl=0,_tmp1007)))));}else{
return _tmp67D;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2507
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2509
struct _tuple24 _stmttmp46=*env;struct _RegionHandle*_tmp681;struct Cyc_Tcenv_Tenv*_tmp682;int _tmp683;struct _tuple24 _tmp680=_stmttmp46;_tmp681=_tmp680.f1;_tmp682=_tmp680.f2;_tmp683=_tmp680.f3;{
struct _tuple12 _stmttmp47=*x;struct Cyc_Absyn_Tqual _tmp685;void*_tmp686;struct _tuple12 _tmp684=_stmttmp47;_tmp685=_tmp684.f1;_tmp686=_tmp684.f2;{
struct Cyc_List_List*_tmp687=Cyc_Tcutil_flatten_typ(_tmp681,_tmp683,_tmp682,_tmp686);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp687)== 1){
struct _tuple12*_tmp100B;struct Cyc_List_List*_tmp100A;return(_tmp100A=_region_malloc(_tmp681,sizeof(*_tmp100A)),((_tmp100A->hd=((_tmp100B=_region_malloc(_tmp681,sizeof(*_tmp100B)),((_tmp100B->f1=_tmp685,((_tmp100B->f2=_tmp686,_tmp100B)))))),((_tmp100A->tl=0,_tmp100A)))));}else{
return _tmp687;}};};}
# 2516
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2520
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp68A=t1;struct Cyc_List_List*_tmp68D;struct Cyc_Absyn_Aggrdecl*_tmp68F;struct Cyc_List_List*_tmp690;struct Cyc_List_List*_tmp692;_LL43D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp68B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp68A;if(_tmp68B->tag != 0)goto _LL43F;}_LL43E:
 return 0;_LL43F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68A;if(_tmp68C->tag != 10)goto _LL441;else{_tmp68D=_tmp68C->f1;}}_LL440: {
# 2525
struct _tuple24 _tmp100C;struct _tuple24 _tmp693=(_tmp100C.f1=r,((_tmp100C.f2=te,((_tmp100C.f3=flatten,_tmp100C)))));
# 2527
struct Cyc_List_List*_tmp694=_tmp68D;struct _tuple12*_tmp695;struct Cyc_List_List*_tmp696;_LL448: if(_tmp694 != 0)goto _LL44A;_LL449:
 return 0;_LL44A: if(_tmp694 == 0)goto _LL447;_tmp695=(struct _tuple12*)_tmp694->hd;_tmp696=_tmp694->tl;_LL44B: {
# 2530
struct Cyc_List_List*_tmp697=Cyc_Tcutil_rcopy_tqt(& _tmp693,_tmp695);
_tmp693.f3=0;{
struct Cyc_List_List*_tmp698=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp693,_tmp68D);
struct Cyc_List_List*_tmp100D;struct Cyc_List_List*_tmp699=(_tmp100D=_region_malloc(r,sizeof(*_tmp100D)),((_tmp100D->hd=_tmp697,((_tmp100D->tl=_tmp698,_tmp100D)))));
struct Cyc_List_List*_tmp100E;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp100E=_region_malloc(r,sizeof(*_tmp100E)),((_tmp100E->hd=_tmp697,((_tmp100E->tl=_tmp698,_tmp100E)))))));};}_LL447:;}_LL441: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp68E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68A;if(_tmp68E->tag != 11)goto _LL443;else{if((((_tmp68E->f1).aggr_info).KnownAggr).tag != 2)goto _LL443;_tmp68F=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp68E->f1).aggr_info).KnownAggr).val);_tmp690=(_tmp68E->f1).targs;}}_LL442:
# 2538
 if(((_tmp68F->kind == Cyc_Absyn_UnionA  || _tmp68F->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68F->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68F->impl))->rgn_po != 0){
# 2540
struct _tuple12*_tmp1011;struct Cyc_List_List*_tmp1010;return(_tmp1010=_region_malloc(r,sizeof(*_tmp1010)),((_tmp1010->hd=((_tmp1011=_region_malloc(r,sizeof(*_tmp1011)),((_tmp1011->f1=Cyc_Absyn_empty_tqual(0),((_tmp1011->f2=t1,_tmp1011)))))),((_tmp1010->tl=0,_tmp1010)))));}{
struct Cyc_List_List*_tmp69F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp68F->tvs,_tmp690);
struct _tuple23 _tmp1012;struct _tuple23 env=(_tmp1012.f1=_tmp69F,((_tmp1012.f2=r,((_tmp1012.f3=te,((_tmp1012.f4=flatten,_tmp1012)))))));
struct Cyc_List_List*_stmttmp48=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68F->impl))->fields;struct Cyc_List_List*_tmp6A0=_stmttmp48;struct Cyc_Absyn_Aggrfield*_tmp6A1;struct Cyc_List_List*_tmp6A2;_LL44D: if(_tmp6A0 != 0)goto _LL44F;_LL44E:
 return 0;_LL44F: if(_tmp6A0 == 0)goto _LL44C;_tmp6A1=(struct Cyc_Absyn_Aggrfield*)_tmp6A0->hd;_tmp6A2=_tmp6A0->tl;_LL450: {
# 2546
struct Cyc_List_List*_tmp6A3=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A1);
env.f4=0;{
struct Cyc_List_List*_tmp6A4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6A2);
struct Cyc_List_List*_tmp1013;struct Cyc_List_List*_tmp6A5=(_tmp1013=_region_malloc(r,sizeof(*_tmp1013)),((_tmp1013->hd=_tmp6A3,((_tmp1013->tl=_tmp6A4,_tmp1013)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6A5);};}_LL44C:;};_LL443: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp691=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68A;if(_tmp691->tag != 12)goto _LL445;else{if(_tmp691->f1 != Cyc_Absyn_StructA)goto _LL445;_tmp692=_tmp691->f2;}}_LL444: {
# 2553
struct _tuple23 _tmp1014;struct _tuple23 env=(_tmp1014.f1=0,((_tmp1014.f2=r,((_tmp1014.f3=te,((_tmp1014.f4=flatten,_tmp1014)))))));
struct Cyc_List_List*_tmp6A8=_tmp692;struct Cyc_Absyn_Aggrfield*_tmp6A9;struct Cyc_List_List*_tmp6AA;_LL452: if(_tmp6A8 != 0)goto _LL454;_LL453:
 return 0;_LL454: if(_tmp6A8 == 0)goto _LL451;_tmp6A9=(struct Cyc_Absyn_Aggrfield*)_tmp6A8->hd;_tmp6AA=_tmp6A8->tl;_LL455: {
# 2557
struct Cyc_List_List*_tmp6AB=Cyc_Tcutil_flatten_typ_f(& env,_tmp6A9);
env.f4=0;{
struct Cyc_List_List*_tmp6AC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6AA);
struct Cyc_List_List*_tmp1015;struct Cyc_List_List*_tmp6AD=(_tmp1015=_region_malloc(r,sizeof(*_tmp1015)),((_tmp1015->hd=_tmp6AB,((_tmp1015->tl=_tmp6AC,_tmp1015)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6AD);};}_LL451:;}_LL445:;_LL446:
# 2563
 goto _LL43C;_LL43C:;};}{
# 2566
struct _tuple12*_tmp1018;struct Cyc_List_List*_tmp1017;return(_tmp1017=_region_malloc(r,sizeof(*_tmp1017)),((_tmp1017->hd=((_tmp1018=_region_malloc(r,sizeof(*_tmp1018)),((_tmp1018->f1=Cyc_Absyn_empty_tqual(0),((_tmp1018->f2=t1,_tmp1018)))))),((_tmp1017->tl=0,_tmp1017)))));};}
# 2570
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_stmttmp49=(void*)t->hd;void*_tmp6B2=_stmttmp49;_LL457: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6B3=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6B2;if(_tmp6B3->tag != 23)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp6B4=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6B2;if(_tmp6B4->tag != 4)goto _LL45B;}_LL45A:
 goto _LL45C;_LL45B: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6B5=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6B2;if(_tmp6B5->tag != 20)goto _LL45D;}_LL45C:
# 2576
 continue;_LL45D:;_LL45E:
# 2578
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL456:;}}
# 2581
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2584
return 1;}
# 2587
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp1019;struct _tuple0 _stmttmp4A=(_tmp1019.f1=Cyc_Tcutil_compress(t1),((_tmp1019.f2=Cyc_Tcutil_compress(t2),_tmp1019)));struct _tuple0 _tmp6B6=_stmttmp4A;enum Cyc_Absyn_Size_of _tmp6B8;enum Cyc_Absyn_Size_of _tmp6BA;_LL460:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6B7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B6.f1;if(_tmp6B7->tag != 6)goto _LL462;else{_tmp6B8=_tmp6B7->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6B9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6B6.f2;if(_tmp6B9->tag != 6)goto _LL462;else{_tmp6BA=_tmp6B9->f2;}};_LL461:
# 2590
 return(_tmp6B8 == _tmp6BA  || _tmp6B8 == Cyc_Absyn_Int_sz  && _tmp6BA == Cyc_Absyn_Long_sz) || 
_tmp6B8 == Cyc_Absyn_Long_sz  && _tmp6BA == Cyc_Absyn_Int_sz;_LL462:;_LL463:
 return 0;_LL45F:;}
# 2598
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2601
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2606
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp101A;struct _tuple0 _stmttmp4B=(_tmp101A.f1=t1,((_tmp101A.f2=t2,_tmp101A)));struct _tuple0 _tmp6BC=_stmttmp4B;void*_tmp6BE;struct Cyc_Absyn_Tqual _tmp6BF;void*_tmp6C0;union Cyc_Absyn_Constraint*_tmp6C1;union Cyc_Absyn_Constraint*_tmp6C2;union Cyc_Absyn_Constraint*_tmp6C3;void*_tmp6C5;struct Cyc_Absyn_Tqual _tmp6C6;void*_tmp6C7;union Cyc_Absyn_Constraint*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;union Cyc_Absyn_Constraint*_tmp6CA;struct Cyc_Absyn_Datatypedecl*_tmp6CC;struct Cyc_Absyn_Datatypefield*_tmp6CD;struct Cyc_List_List*_tmp6CE;struct Cyc_Absyn_Datatypedecl*_tmp6D0;struct Cyc_List_List*_tmp6D1;struct Cyc_Absyn_FnInfo _tmp6D3;struct Cyc_Absyn_FnInfo _tmp6D5;_LL465:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6BD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BC.f1;if(_tmp6BD->tag != 5)goto _LL467;else{_tmp6BE=(_tmp6BD->f1).elt_typ;_tmp6BF=(_tmp6BD->f1).elt_tq;_tmp6C0=((_tmp6BD->f1).ptr_atts).rgn;_tmp6C1=((_tmp6BD->f1).ptr_atts).nullable;_tmp6C2=((_tmp6BD->f1).ptr_atts).bounds;_tmp6C3=((_tmp6BD->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6BC.f2;if(_tmp6C4->tag != 5)goto _LL467;else{_tmp6C5=(_tmp6C4->f1).elt_typ;_tmp6C6=(_tmp6C4->f1).elt_tq;_tmp6C7=((_tmp6C4->f1).ptr_atts).rgn;_tmp6C8=((_tmp6C4->f1).ptr_atts).nullable;_tmp6C9=((_tmp6C4->f1).ptr_atts).bounds;_tmp6CA=((_tmp6C4->f1).ptr_atts).zero_term;}};_LL466:
# 2614
 if(_tmp6BF.real_const  && !_tmp6C6.real_const)
return 0;
# 2617
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C1,_tmp6C8) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C1)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C8))
return 0;
# 2621
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6C3,_tmp6CA) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C3)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CA))
return 0;
# 2625
if((!Cyc_Tcutil_unify(_tmp6C0,_tmp6C7) && !Cyc_Tcenv_region_outlives(te,_tmp6C0,_tmp6C7)) && !
Cyc_Tcutil_subtype(te,assume,_tmp6C0,_tmp6C7))
return 0;
# 2629
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6C2,_tmp6C9)){
struct _tuple0 _tmp101B;struct _tuple0 _stmttmp4E=(_tmp101B.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6C2),((_tmp101B.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6C9),_tmp101B)));struct _tuple0 _tmp6D6=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp6DA;struct Cyc_Absyn_Exp*_tmp6DC;_LL46E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D6.f1;if(_tmp6D7->tag != 1)goto _LL470;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6D8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6D6.f2;if(_tmp6D8->tag != 0)goto _LL470;};_LL46F:
 goto _LL46D;_LL470:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D6.f1;if(_tmp6D9->tag != 1)goto _LL472;else{_tmp6DA=_tmp6D9->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6DB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D6.f2;if(_tmp6DB->tag != 1)goto _LL472;else{_tmp6DC=_tmp6DB->f1;}};_LL471:
# 2633
 if(!Cyc_Evexp_lte_const_exp(_tmp6DC,_tmp6DA))
return 0;
goto _LL46D;_LL472:;_LL473:
 return 0;_LL46D:;}
# 2641
if(!_tmp6C6.real_const  && _tmp6BF.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp6C5)))
return 0;}{
# 2647
int _tmp6DE=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp6C9,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp6CA);
# 2653
struct _tuple0*_tmp101E;struct Cyc_List_List*_tmp101D;return(_tmp6DE  && Cyc_Tcutil_ptrsubtype(te,((_tmp101D=_cycalloc(sizeof(*_tmp101D)),((_tmp101D->hd=((_tmp101E=_cycalloc(sizeof(*_tmp101E)),((_tmp101E->f1=t1,((_tmp101E->f2=t2,_tmp101E)))))),((_tmp101D->tl=assume,_tmp101D)))))),_tmp6BE,_tmp6C5) || Cyc_Tcutil_unify(_tmp6BE,_tmp6C5)) || Cyc_Tcutil_isomorphic(_tmp6BE,_tmp6C5);};_LL467:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6CB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6BC.f1;if(_tmp6CB->tag != 4)goto _LL469;else{if((((_tmp6CB->f1).field_info).KnownDatatypefield).tag != 2)goto _LL469;_tmp6CC=((struct _tuple3)(((_tmp6CB->f1).field_info).KnownDatatypefield).val).f1;_tmp6CD=((struct _tuple3)(((_tmp6CB->f1).field_info).KnownDatatypefield).val).f2;_tmp6CE=(_tmp6CB->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6CF=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6BC.f2;if(_tmp6CF->tag != 3)goto _LL469;else{if((((_tmp6CF->f1).datatype_info).KnownDatatype).tag != 2)goto _LL469;_tmp6D0=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp6CF->f1).datatype_info).KnownDatatype).val);_tmp6D1=(_tmp6CF->f1).targs;}};_LL468:
# 2659
 if(_tmp6CC != _tmp6D0  && Cyc_Absyn_qvar_cmp(_tmp6CC->name,_tmp6D0->name)!= 0)return 0;
# 2661
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6CE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D1))return 0;
for(0;_tmp6CE != 0;(_tmp6CE=_tmp6CE->tl,_tmp6D1=_tmp6D1->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp6CE->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6D1))->hd))return 0;}
return 1;_LL469:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D2=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6BC.f1;if(_tmp6D2->tag != 9)goto _LL46B;else{_tmp6D3=_tmp6D2->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6D4=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6BC.f2;if(_tmp6D4->tag != 9)goto _LL46B;else{_tmp6D5=_tmp6D4->f1;}};_LL46A:
# 2668
 if(_tmp6D3.tvars != 0  || _tmp6D5.tvars != 0){
struct Cyc_List_List*_tmp6E1=_tmp6D3.tvars;
struct Cyc_List_List*_tmp6E2=_tmp6D5.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E1)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E2))return 0;{
struct _RegionHandle _tmp6E3=_new_region("r");struct _RegionHandle*r=& _tmp6E3;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp6E1 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp6E1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp6E2))->hd)->kind)){int _tmp6E4=0;_npop_handler(0);return _tmp6E4;}
{struct _tuple16*_tmp1028;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1027;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1026;struct Cyc_List_List*_tmp1025;inst=((_tmp1025=_region_malloc(r,sizeof(*_tmp1025)),((_tmp1025->hd=((_tmp1028=_region_malloc(r,sizeof(*_tmp1028)),((_tmp1028->f1=(struct Cyc_Absyn_Tvar*)_tmp6E2->hd,((_tmp1028->f2=(void*)((_tmp1026=_cycalloc(sizeof(*_tmp1026)),((_tmp1026[0]=((_tmp1027.tag=2,((_tmp1027.f1=(struct Cyc_Absyn_Tvar*)_tmp6E1->hd,_tmp1027)))),_tmp1026)))),_tmp1028)))))),((_tmp1025->tl=inst,_tmp1025))))));}
_tmp6E1=_tmp6E1->tl;
_tmp6E2=_tmp6E2->tl;}
# 2680
if(inst != 0){
_tmp6D3.tvars=0;
_tmp6D5.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp102E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp102D;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp102B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp102A;int _tmp6ED=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp102A=_cycalloc(sizeof(*_tmp102A)),((_tmp102A[0]=((_tmp102B.tag=9,((_tmp102B.f1=_tmp6D3,_tmp102B)))),_tmp102A)))),(void*)((_tmp102D=_cycalloc(sizeof(*_tmp102D)),((_tmp102D[0]=((_tmp102E.tag=9,((_tmp102E.f1=_tmp6D5,_tmp102E)))),_tmp102D)))));_npop_handler(0);return _tmp6ED;};}}
# 2673
;_pop_region(r);};}
# 2688
if(!Cyc_Tcutil_subtype(te,assume,_tmp6D3.ret_typ,_tmp6D5.ret_typ))return 0;{
struct Cyc_List_List*_tmp6EE=_tmp6D3.args;
struct Cyc_List_List*_tmp6EF=_tmp6D5.args;
# 2693
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6EE)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6EF))return 0;
# 2695
for(0;_tmp6EE != 0;(_tmp6EE=_tmp6EE->tl,_tmp6EF=_tmp6EF->tl)){
struct _tuple10 _stmttmp4C=*((struct _tuple10*)_tmp6EE->hd);struct Cyc_Absyn_Tqual _tmp6F1;void*_tmp6F2;struct _tuple10 _tmp6F0=_stmttmp4C;_tmp6F1=_tmp6F0.f2;_tmp6F2=_tmp6F0.f3;{
struct _tuple10 _stmttmp4D=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp6EF))->hd);struct Cyc_Absyn_Tqual _tmp6F4;void*_tmp6F5;struct _tuple10 _tmp6F3=_stmttmp4D;_tmp6F4=_tmp6F3.f2;_tmp6F5=_tmp6F3.f3;
# 2699
if(_tmp6F4.real_const  && !_tmp6F1.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp6F5,_tmp6F2))
return 0;};}
# 2703
if(_tmp6D3.c_varargs != _tmp6D5.c_varargs)return 0;
if(_tmp6D3.cyc_varargs != 0  && _tmp6D5.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp6F6=*_tmp6D3.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp6F7=*_tmp6D5.cyc_varargs;
# 2708
if((_tmp6F7.tq).real_const  && !(_tmp6F6.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp6F7.type,_tmp6F6.type))
return 0;}else{
if(_tmp6D3.cyc_varargs != 0  || _tmp6D5.cyc_varargs != 0)return 0;}
# 2713
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp6D3.effect),(void*)_check_null(_tmp6D5.effect)))return 0;
# 2715
if(!Cyc_Tcutil_sub_rgnpo(_tmp6D3.rgn_po,_tmp6D5.rgn_po))return 0;
# 2717
if(!Cyc_Tcutil_sub_attributes(_tmp6D3.attributes,_tmp6D5.attributes))return 0;
# 2719
return 1;};_LL46B:;_LL46C:
 return 0;_LL464:;};}
# 2731 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2733
struct _RegionHandle _tmp6F9=_new_region("temp");struct _RegionHandle*temp=& _tmp6F9;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp6FA=0;_npop_handler(0);return _tmp6FA;}{
struct _tuple12*_stmttmp4F=(struct _tuple12*)tqs1->hd;struct Cyc_Absyn_Tqual _tmp6FC;void*_tmp6FD;struct _tuple12*_tmp6FB=_stmttmp4F;_tmp6FC=_tmp6FB->f1;_tmp6FD=_tmp6FB->f2;{
struct _tuple12*_stmttmp50=(struct _tuple12*)tqs2->hd;struct Cyc_Absyn_Tqual _tmp6FF;void*_tmp700;struct _tuple12*_tmp6FE=_stmttmp50;_tmp6FF=_tmp6FE->f1;_tmp700=_tmp6FE->f2;
# 2741
if(_tmp6FC.real_const  && !_tmp6FF.real_const){int _tmp701=0;_npop_handler(0);return _tmp701;}
# 2743
if((_tmp6FF.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp700))) && 
Cyc_Tcutil_subtype(te,assume,_tmp6FD,_tmp700))
# 2746
continue;
# 2748
if(Cyc_Tcutil_unify(_tmp6FD,_tmp700))
# 2750
continue;
# 2752
if(Cyc_Tcutil_isomorphic(_tmp6FD,_tmp700))
# 2754
continue;{
# 2757
int _tmp702=0;_npop_handler(0);return _tmp702;};};};}{
# 2759
int _tmp703=1;_npop_handler(0);return _tmp703;};}
# 2734
;_pop_region(temp);}
# 2763
static int Cyc_Tcutil_is_char_type(void*t){
void*_stmttmp51=Cyc_Tcutil_compress(t);void*_tmp704=_stmttmp51;_LL475: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp705=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp704;if(_tmp705->tag != 6)goto _LL477;else{if(_tmp705->f2 != Cyc_Absyn_Char_sz)goto _LL477;}}_LL476:
 return 1;_LL477:;_LL478:
 return 0;_LL474:;}
# 2772
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2778
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp706=t2;_LL47A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp707=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp706;if(_tmp707->tag != 6)goto _LL47C;else{if(_tmp707->f2 != Cyc_Absyn_Int_sz)goto _LL47C;}}_LL47B:
# 2782
 goto _LL47D;_LL47C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp708=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp706;if(_tmp708->tag != 6)goto _LL47E;else{if(_tmp708->f2 != Cyc_Absyn_Long_sz)goto _LL47E;}}_LL47D:
# 2784
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL479;_LL47E:;_LL47F:
 goto _LL479;_LL479:;}{
# 2788
void*_tmp709=t1;void*_tmp70B;struct Cyc_Absyn_Tqual _tmp70C;void*_tmp70D;union Cyc_Absyn_Constraint*_tmp70E;union Cyc_Absyn_Constraint*_tmp70F;union Cyc_Absyn_Constraint*_tmp710;void*_tmp712;struct Cyc_Absyn_Tqual _tmp713;struct Cyc_Absyn_Exp*_tmp714;union Cyc_Absyn_Constraint*_tmp715;struct Cyc_Absyn_Enumdecl*_tmp717;void*_tmp71B;_LL481: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp709;if(_tmp70A->tag != 5)goto _LL483;else{_tmp70B=(_tmp70A->f1).elt_typ;_tmp70C=(_tmp70A->f1).elt_tq;_tmp70D=((_tmp70A->f1).ptr_atts).rgn;_tmp70E=((_tmp70A->f1).ptr_atts).nullable;_tmp70F=((_tmp70A->f1).ptr_atts).bounds;_tmp710=((_tmp70A->f1).ptr_atts).zero_term;}}_LL482:
# 2796
{void*_tmp71C=t2;void*_tmp71E;struct Cyc_Absyn_Tqual _tmp71F;void*_tmp720;union Cyc_Absyn_Constraint*_tmp721;union Cyc_Absyn_Constraint*_tmp722;union Cyc_Absyn_Constraint*_tmp723;_LL490: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp71D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp71C;if(_tmp71D->tag != 5)goto _LL492;else{_tmp71E=(_tmp71D->f1).elt_typ;_tmp71F=(_tmp71D->f1).elt_tq;_tmp720=((_tmp71D->f1).ptr_atts).rgn;_tmp721=((_tmp71D->f1).ptr_atts).nullable;_tmp722=((_tmp71D->f1).ptr_atts).bounds;_tmp723=((_tmp71D->f1).ptr_atts).zero_term;}}_LL491: {
# 2800
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp1031;struct Cyc_List_List*_tmp1030;struct Cyc_List_List*_tmp724=(_tmp1030=_cycalloc(sizeof(*_tmp1030)),((_tmp1030->hd=((_tmp1031=_cycalloc(sizeof(*_tmp1031)),((_tmp1031->f1=t1,((_tmp1031->f2=t2,_tmp1031)))))),((_tmp1030->tl=0,_tmp1030)))));
int _tmp725=_tmp71F.real_const  || !_tmp70C.real_const;
# 2814 "tcutil.cyc"
int _tmp726=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp722,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp723);
# 2818
int _tmp727=_tmp725  && (
((_tmp726  && Cyc_Tcutil_ptrsubtype(te,_tmp724,_tmp70B,_tmp71E) || 
Cyc_Tcutil_unify(_tmp70B,_tmp71E)) || Cyc_Tcutil_isomorphic(_tmp70B,_tmp71E)) || Cyc_Tcutil_unify(_tmp71E,(void*)& Cyc_Absyn_VoidType_val));
# 2822
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp710,_tmp723) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp723);
# 2826
int _tmp728=_tmp727?0:((Cyc_Tcutil_bits_only(_tmp70B) && Cyc_Tcutil_is_char_type(_tmp71E)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp723)) && (
_tmp71F.real_const  || !_tmp70C.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp70F,_tmp722);
if(!bounds_ok  && !_tmp728){
struct _tuple0 _tmp1032;struct _tuple0 _stmttmp52=(_tmp1032.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp70F),((_tmp1032.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp722),_tmp1032)));struct _tuple0 _tmp729=_stmttmp52;struct Cyc_Absyn_Exp*_tmp72B;struct Cyc_Absyn_Exp*_tmp72D;_LL495:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp729.f1;if(_tmp72A->tag != 1)goto _LL497;else{_tmp72B=_tmp72A->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp729.f2;if(_tmp72C->tag != 1)goto _LL497;else{_tmp72D=_tmp72C->f1;}};_LL496:
# 2836
 if(Cyc_Evexp_lte_const_exp(_tmp72D,_tmp72B))
bounds_ok=1;
goto _LL494;_LL497:;_LL498:
# 2841
 bounds_ok=1;goto _LL494;_LL494:;}
# 2843
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp70E) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp721))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2848
if(((bounds_ok  && zeroterm_ok) && (_tmp727  || _tmp728)) && (
Cyc_Tcutil_unify(_tmp70D,_tmp720) || Cyc_Tcenv_region_outlives(te,_tmp70D,_tmp720)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL492:;_LL493:
 goto _LL48F;_LL48F:;}
# 2854
return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp711=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp709;if(_tmp711->tag != 8)goto _LL485;else{_tmp712=(_tmp711->f1).elt_type;_tmp713=(_tmp711->f1).tq;_tmp714=(_tmp711->f1).num_elts;_tmp715=(_tmp711->f1).zero_term;}}_LL484:
# 2856
{void*_tmp731=t2;void*_tmp733;struct Cyc_Absyn_Tqual _tmp734;struct Cyc_Absyn_Exp*_tmp735;union Cyc_Absyn_Constraint*_tmp736;_LL49A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp732=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp731;if(_tmp732->tag != 8)goto _LL49C;else{_tmp733=(_tmp732->f1).elt_type;_tmp734=(_tmp732->f1).tq;_tmp735=(_tmp732->f1).num_elts;_tmp736=(_tmp732->f1).zero_term;}}_LL49B: {
# 2858
int okay;
okay=
(((_tmp714 != 0  && _tmp735 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp715,_tmp736)) && 
Cyc_Evexp_lte_const_exp(_tmp735,_tmp714)) && 
Cyc_Evexp_lte_const_exp(_tmp714,_tmp735);
return
# 2865
(okay  && Cyc_Tcutil_unify(_tmp712,_tmp733)) && (!_tmp713.real_const  || _tmp734.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL49C:;_LL49D:
# 2867
 return Cyc_Absyn_Unknown_coercion;_LL499:;}
# 2869
return Cyc_Absyn_Unknown_coercion;_LL485: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp716=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp709;if(_tmp716->tag != 13)goto _LL487;else{_tmp717=_tmp716->f2;}}_LL486:
# 2873
{void*_tmp737=t2;struct Cyc_Absyn_Enumdecl*_tmp739;_LL49F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp738=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp737;if(_tmp738->tag != 13)goto _LL4A1;else{_tmp739=_tmp738->f2;}}_LL4A0:
# 2875
 if((_tmp717->fields != 0  && _tmp739->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp717->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp739->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL49E;_LL4A1:;_LL4A2:
 goto _LL49E;_LL49E:;}
# 2881
goto _LL488;_LL487: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp718=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp709;if(_tmp718->tag != 6)goto _LL489;}_LL488:
 goto _LL48A;_LL489: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp719=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp709;if(_tmp719->tag != 7)goto _LL48B;}_LL48A:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL48B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp71A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp709;if(_tmp71A->tag != 15)goto _LL48D;else{_tmp71B=(void*)_tmp71A->f1;}}_LL48C:
# 2886
{void*_tmp73A=t2;void*_tmp73C;_LL4A4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp73B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp73A;if(_tmp73B->tag != 15)goto _LL4A6;else{_tmp73C=(void*)_tmp73B->f1;}}_LL4A5:
# 2888
 if(Cyc_Tcenv_region_outlives(te,_tmp71B,_tmp73C))return Cyc_Absyn_No_coercion;
goto _LL4A3;_LL4A6:;_LL4A7:
 goto _LL4A3;_LL4A3:;}
# 2892
return Cyc_Absyn_Unknown_coercion;_LL48D:;_LL48E:
 return Cyc_Absyn_Unknown_coercion;_LL480:;};}
# 2898
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp73D=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1035;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1034;e->r=(void*)((_tmp1034=_cycalloc(sizeof(*_tmp1034)),((_tmp1034[0]=((_tmp1035.tag=13,((_tmp1035.f1=t,((_tmp1035.f2=_tmp73D,((_tmp1035.f3=0,((_tmp1035.f4=c,_tmp1035)))))))))),_tmp1034))));}
e->topt=t;}}
# 2906
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_stmttmp53=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp740=_stmttmp53;_LL4A9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp741=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp740;if(_tmp741->tag != 6)goto _LL4AB;}_LL4AA:
 goto _LL4AC;_LL4AB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp742=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp740;if(_tmp742->tag != 13)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp743=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp740;if(_tmp743->tag != 14)goto _LL4AF;}_LL4AE:
 goto _LL4B0;_LL4AF: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp744=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp740;if(_tmp744->tag != 19)goto _LL4B1;}_LL4B0:
 return 1;_LL4B1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp745=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp740;if(_tmp745->tag != 1)goto _LL4B3;}_LL4B2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4B3:;_LL4B4:
 return 0;_LL4A8:;}
# 2917
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_stmttmp54=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp746=_stmttmp54;_LL4B6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp747=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp746;if(_tmp747->tag != 7)goto _LL4B8;}_LL4B7:
 return 1;_LL4B8:;_LL4B9:
 return 0;_LL4B5:;};}
# 2926
int Cyc_Tcutil_is_function_type(void*t){
void*_stmttmp55=Cyc_Tcutil_compress(t);void*_tmp748=_stmttmp55;_LL4BB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp748;if(_tmp749->tag != 9)goto _LL4BD;}_LL4BC:
 return 1;_LL4BD:;_LL4BE:
 return 0;_LL4BA:;}
# 2933
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1036;struct _tuple0 _stmttmp56=(_tmp1036.f1=t1,((_tmp1036.f2=t2,_tmp1036)));struct _tuple0 _tmp74A=_stmttmp56;int _tmp74C;int _tmp74E;_LL4C0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp74B->tag != 7)goto _LL4C2;else{_tmp74C=_tmp74B->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp74D->tag != 7)goto _LL4C2;else{_tmp74E=_tmp74D->f1;}};_LL4C1:
# 2936
 if(_tmp74C != 0  && _tmp74C != 1)return t1;else{
if(_tmp74E != 0  && _tmp74E != 1)return t2;else{
if(_tmp74C >= _tmp74E)return t1;else{
return t2;}}}_LL4C2: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp74F->tag != 7)goto _LL4C4;}_LL4C3:
 return t1;_LL4C4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp750=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp750->tag != 7)goto _LL4C6;}_LL4C5:
 return t2;_LL4C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp751=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp751->tag != 6)goto _LL4C8;else{if(_tmp751->f1 != Cyc_Absyn_Unsigned)goto _LL4C8;if(_tmp751->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C8;}}_LL4C7:
 goto _LL4C9;_LL4C8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp752=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp752->tag != 6)goto _LL4CA;else{if(_tmp752->f1 != Cyc_Absyn_Unsigned)goto _LL4CA;if(_tmp752->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CA;}}_LL4C9:
 return Cyc_Absyn_ulonglong_typ;_LL4CA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp753=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp753->tag != 6)goto _LL4CC;else{if(_tmp753->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CC;}}_LL4CB:
 goto _LL4CD;_LL4CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp754=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp754->tag != 6)goto _LL4CE;else{if(_tmp754->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CE;}}_LL4CD:
 return Cyc_Absyn_slonglong_typ;_LL4CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp755=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp755->tag != 6)goto _LL4D0;else{if(_tmp755->f1 != Cyc_Absyn_Unsigned)goto _LL4D0;if(_tmp755->f2 != Cyc_Absyn_Long_sz)goto _LL4D0;}}_LL4CF:
 goto _LL4D1;_LL4D0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp756=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp756->tag != 6)goto _LL4D2;else{if(_tmp756->f1 != Cyc_Absyn_Unsigned)goto _LL4D2;if(_tmp756->f2 != Cyc_Absyn_Long_sz)goto _LL4D2;}}_LL4D1:
 return Cyc_Absyn_ulong_typ;_LL4D2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp757=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp757->tag != 6)goto _LL4D4;else{if(_tmp757->f1 != Cyc_Absyn_Unsigned)goto _LL4D4;if(_tmp757->f2 != Cyc_Absyn_Int_sz)goto _LL4D4;}}_LL4D3:
# 2949
 goto _LL4D5;_LL4D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp758=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp758->tag != 6)goto _LL4D6;else{if(_tmp758->f1 != Cyc_Absyn_Unsigned)goto _LL4D6;if(_tmp758->f2 != Cyc_Absyn_Int_sz)goto _LL4D6;}}_LL4D5:
 return Cyc_Absyn_uint_typ;_LL4D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f1;if(_tmp759->tag != 6)goto _LL4D8;else{if(_tmp759->f2 != Cyc_Absyn_Long_sz)goto _LL4D8;}}_LL4D7:
 goto _LL4D9;_LL4D8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74A.f2;if(_tmp75A->tag != 6)goto _LL4DA;else{if(_tmp75A->f2 != Cyc_Absyn_Long_sz)goto _LL4DA;}}_LL4D9:
 return Cyc_Absyn_slong_typ;_LL4DA:;_LL4DB:
 return Cyc_Absyn_sint_typ;_LL4BF:;}
# 2958
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_stmttmp57=e->r;void*_tmp75C=_stmttmp57;_LL4DD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp75D=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp75C;if(_tmp75D->tag != 3)goto _LL4DF;else{if(_tmp75D->f2 != 0)goto _LL4DF;}}_LL4DE:
{const char*_tmp1039;void*_tmp1038;(_tmp1038=0,Cyc_Tcutil_warn(e->loc,((_tmp1039="assignment in test",_tag_dyneither(_tmp1039,sizeof(char),19))),_tag_dyneither(_tmp1038,sizeof(void*),0)));}goto _LL4DC;_LL4DF:;_LL4E0:
 goto _LL4DC;_LL4DC:;}
# 2972 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp103A;struct _tuple0 _stmttmp58=(_tmp103A.f1=c1,((_tmp103A.f2=c2,_tmp103A)));struct _tuple0 _tmp760=_stmttmp58;struct Cyc_Absyn_Kind*_tmp762;struct Cyc_Absyn_Kind*_tmp764;struct Cyc_Core_Opt**_tmp766;struct Cyc_Core_Opt**_tmp768;struct Cyc_Core_Opt**_tmp76A;struct Cyc_Absyn_Kind*_tmp76B;struct Cyc_Absyn_Kind*_tmp76D;struct Cyc_Absyn_Kind*_tmp76F;struct Cyc_Core_Opt**_tmp771;struct Cyc_Absyn_Kind*_tmp772;struct Cyc_Core_Opt**_tmp774;struct Cyc_Absyn_Kind*_tmp775;struct Cyc_Core_Opt**_tmp777;struct Cyc_Absyn_Kind*_tmp778;_LL4E2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp761=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp761->tag != 0)goto _LL4E4;else{_tmp762=_tmp761->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp763=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp763->tag != 0)goto _LL4E4;else{_tmp764=_tmp763->f1;}};_LL4E3:
 return _tmp762 == _tmp764;_LL4E4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp765=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp765->tag != 1)goto _LL4E6;else{_tmp766=(struct Cyc_Core_Opt**)& _tmp765->f1;}}_LL4E5:
{struct Cyc_Core_Opt*_tmp103B;*_tmp766=((_tmp103B=_cycalloc(sizeof(*_tmp103B)),((_tmp103B->v=c1,_tmp103B))));}return 1;_LL4E6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp767=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp767->tag != 1)goto _LL4E8;else{_tmp768=(struct Cyc_Core_Opt**)& _tmp767->f1;}}_LL4E7:
{struct Cyc_Core_Opt*_tmp103C;*_tmp768=((_tmp103C=_cycalloc(sizeof(*_tmp103C)),((_tmp103C->v=c2,_tmp103C))));}return 1;_LL4E8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp769=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp769->tag != 2)goto _LL4EA;else{_tmp76A=(struct Cyc_Core_Opt**)& _tmp769->f1;_tmp76B=_tmp769->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp76C->tag != 0)goto _LL4EA;else{_tmp76D=_tmp76C->f1;}};_LL4E9:
# 2980
 if(Cyc_Tcutil_kind_leq(_tmp76D,_tmp76B)){
{struct Cyc_Core_Opt*_tmp103D;*_tmp76A=((_tmp103D=_cycalloc(sizeof(*_tmp103D)),((_tmp103D->v=c2,_tmp103D))));}return 1;}else{
return 0;}_LL4EA:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp76E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp76E->tag != 0)goto _LL4EC;else{_tmp76F=_tmp76E->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp770=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp770->tag != 2)goto _LL4EC;else{_tmp771=(struct Cyc_Core_Opt**)& _tmp770->f1;_tmp772=_tmp770->f2;}};_LL4EB:
# 2984
 if(Cyc_Tcutil_kind_leq(_tmp76F,_tmp772)){
{struct Cyc_Core_Opt*_tmp103E;*_tmp771=((_tmp103E=_cycalloc(sizeof(*_tmp103E)),((_tmp103E->v=c1,_tmp103E))));}return 1;}else{
return 0;}_LL4EC:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp773=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f1;if(_tmp773->tag != 2)goto _LL4E1;else{_tmp774=(struct Cyc_Core_Opt**)& _tmp773->f1;_tmp775=_tmp773->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp776=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp760.f2;if(_tmp776->tag != 2)goto _LL4E1;else{_tmp777=(struct Cyc_Core_Opt**)& _tmp776->f1;_tmp778=_tmp776->f2;}};_LL4ED:
# 2988
 if(Cyc_Tcutil_kind_leq(_tmp775,_tmp778)){
{struct Cyc_Core_Opt*_tmp103F;*_tmp777=((_tmp103F=_cycalloc(sizeof(*_tmp103F)),((_tmp103F->v=c1,_tmp103F))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp778,_tmp775)){
{struct Cyc_Core_Opt*_tmp1040;*_tmp774=((_tmp1040=_cycalloc(sizeof(*_tmp1040)),((_tmp1040->v=c2,_tmp1040))));}return 1;}else{
return 0;}}_LL4E1:;};}
# 2997
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3002
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1044;void*_tmp1043[1];struct Cyc_Int_pa_PrintArg_struct _tmp1042;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1042.tag=1,((_tmp1042.f1=(unsigned long)i,((_tmp1043[0]=& _tmp1042,Cyc_aprintf(((_tmp1044="#%d",_tag_dyneither(_tmp1044,sizeof(char),4))),_tag_dyneither(_tmp1043,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1047;struct Cyc_Absyn_Tvar*_tmp1046;return(_tmp1046=_cycalloc(sizeof(*_tmp1046)),((_tmp1046->name=((_tmp1047=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1047[0]=(struct _dyneither_ptr)s,_tmp1047)))),((_tmp1046->identity=- 1,((_tmp1046->kind=k,_tmp1046)))))));}
# 3009
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp785=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp785,sizeof(char),0))== '#';}
# 3014
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp104B;void*_tmp104A[1];struct Cyc_String_pa_PrintArg_struct _tmp1049;(_tmp1049.tag=0,((_tmp1049.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp104A[0]=& _tmp1049,Cyc_printf(((_tmp104B="%s",_tag_dyneither(_tmp104B,sizeof(char),3))),_tag_dyneither(_tmp104A,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp104C;struct _dyneither_ptr _tmp789=Cyc_strconcat(((_tmp104C="`",_tag_dyneither(_tmp104C,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp104F;char _tmp104E;struct _dyneither_ptr _tmp104D;(_tmp104D=_dyneither_ptr_plus(_tmp789,sizeof(char),1),((_tmp104E=*((char*)_check_dyneither_subscript(_tmp104D,sizeof(char),0)),((_tmp104F='t',((_get_dyneither_size(_tmp104D,sizeof(char))== 1  && (_tmp104E == '\000'  && _tmp104F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp104D.curr)=_tmp104F)))))));}{
struct _dyneither_ptr*_tmp1050;t->name=((_tmp1050=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1050[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp789),_tmp1050))));};};}
# 3023
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3025
struct _tuple10*_tmp1051;return(_tmp1051=_cycalloc(sizeof(*_tmp1051)),((_tmp1051->f1=(*x).f1,((_tmp1051->f2=(*x).f2,((_tmp1051->f3=(*x).f3,_tmp1051)))))));}
# 3028
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3035
struct Cyc_List_List*_tmp790=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1052;_tmp790=((_tmp1052=_cycalloc(sizeof(*_tmp1052)),((_tmp1052->hd=(void*)atts->hd,((_tmp1052->tl=_tmp790,_tmp1052))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1058;struct Cyc_Absyn_FnInfo _tmp1057;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1056;return(void*)((_tmp1056=_cycalloc(sizeof(*_tmp1056)),((_tmp1056[0]=((_tmp1058.tag=9,((_tmp1058.f1=((_tmp1057.tvars=fd->tvs,((_tmp1057.effect=fd->effect,((_tmp1057.ret_tqual=fd->ret_tqual,((_tmp1057.ret_typ=fd->ret_type,((_tmp1057.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1057.c_varargs=fd->c_varargs,((_tmp1057.cyc_varargs=fd->cyc_varargs,((_tmp1057.rgn_po=fd->rgn_po,((_tmp1057.attributes=_tmp790,_tmp1057)))))))))))))))))),_tmp1058)))),_tmp1056))));};}
# 3044
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3048
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp796;void*_tmp797;struct _tuple12*_tmp795=pr;_tmp796=_tmp795->f1;_tmp797=_tmp795->f2;
if(_tmp797 == t)return pr;else{
struct _tuple12*_tmp1059;return(_tmp1059=_cycalloc(sizeof(*_tmp1059)),((_tmp1059->f1=_tmp796,((_tmp1059->f2=t,_tmp1059)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3055
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3057
struct _tuple26*_tmp105C;struct _tuple27*_tmp105B;return(_tmp105B=_region_malloc(rgn,sizeof(*_tmp105B)),((_tmp105B->f1=((_tmp105C=_region_malloc(rgn,sizeof(*_tmp105C)),((_tmp105C->f1=(*y).f1,((_tmp105C->f2=(*y).f2,_tmp105C)))))),((_tmp105B->f2=(*y).f3,_tmp105B)))));}
# 3059
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3061
struct _tuple10 _stmttmp59=*orig_arg;struct _dyneither_ptr*_tmp79C;struct Cyc_Absyn_Tqual _tmp79D;void*_tmp79E;struct _tuple10 _tmp79B=_stmttmp59;_tmp79C=_tmp79B.f1;_tmp79D=_tmp79B.f2;_tmp79E=_tmp79B.f3;
if(t == _tmp79E)return orig_arg;{
struct _tuple10*_tmp105D;return(_tmp105D=_cycalloc(sizeof(*_tmp105D)),((_tmp105D->f1=_tmp79C,((_tmp105D->f2=_tmp79D,((_tmp105D->f3=t,_tmp105D)))))));};}
# 3065
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3068
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3073
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3075
struct Cyc_Absyn_Exp*_tmp105E;return(_tmp105E=_cycalloc(sizeof(*_tmp105E)),((_tmp105E->topt=old->topt,((_tmp105E->r=r,((_tmp105E->loc=old->loc,((_tmp105E->annot=old->annot,_tmp105E)))))))));}
# 3080
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_stmttmp5A=e->r;void*_tmp7A1=_stmttmp5A;enum Cyc_Absyn_Primop _tmp7A7;struct Cyc_List_List*_tmp7A8;struct Cyc_Absyn_Exp*_tmp7AA;struct Cyc_Absyn_Exp*_tmp7AB;struct Cyc_Absyn_Exp*_tmp7AC;struct Cyc_Absyn_Exp*_tmp7AE;struct Cyc_Absyn_Exp*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Exp*_tmp7B4;struct Cyc_Absyn_Exp*_tmp7B5;void*_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B8;int _tmp7B9;enum Cyc_Absyn_Coercion _tmp7BA;void*_tmp7BC;struct Cyc_Absyn_Exp*_tmp7BE;void*_tmp7C0;struct Cyc_List_List*_tmp7C1;void*_tmp7C3;_LL4EF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7A2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A2->tag != 0)goto _LL4F1;}_LL4F0:
 goto _LL4F2;_LL4F1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp7A3=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A3->tag != 31)goto _LL4F3;}_LL4F2:
 goto _LL4F4;_LL4F3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp7A4=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A4->tag != 32)goto _LL4F5;}_LL4F4:
 goto _LL4F6;_LL4F5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp7A5=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A5->tag != 1)goto _LL4F7;}_LL4F6:
 return e;_LL4F7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp7A6=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A6->tag != 2)goto _LL4F9;else{_tmp7A7=_tmp7A6->f1;_tmp7A8=_tmp7A6->f2;}}_LL4F8:
# 3088
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A8)== 1){
struct Cyc_Absyn_Exp*_tmp7C4=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A8))->hd;
struct Cyc_Absyn_Exp*_tmp7C5=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C4);
if(_tmp7C5 == _tmp7C4)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1064;struct Cyc_Absyn_Exp*_tmp1063[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1062;return Cyc_Tcutil_copye(e,(void*)((_tmp1062=_cycalloc(sizeof(*_tmp1062)),((_tmp1062[0]=((_tmp1064.tag=2,((_tmp1064.f1=_tmp7A7,((_tmp1064.f2=((_tmp1063[0]=_tmp7C5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1063,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1064)))))),_tmp1062)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7A8)== 2){
struct Cyc_Absyn_Exp*_tmp7C9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A8))->hd;
struct Cyc_Absyn_Exp*_tmp7CA=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp7A8->tl))->hd;
struct Cyc_Absyn_Exp*_tmp7CB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7C9);
struct Cyc_Absyn_Exp*_tmp7CC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7CA);
if(_tmp7CB == _tmp7C9  && _tmp7CC == _tmp7CA)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp106A;struct Cyc_Absyn_Exp*_tmp1069[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1068;return Cyc_Tcutil_copye(e,(void*)((_tmp1068=_cycalloc(sizeof(*_tmp1068)),((_tmp1068[0]=((_tmp106A.tag=2,((_tmp106A.f1=_tmp7A7,((_tmp106A.f2=((_tmp1069[1]=_tmp7CC,((_tmp1069[0]=_tmp7CB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1069,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp106A)))))),_tmp1068)))));};}else{
const char*_tmp106D;void*_tmp106C;return(_tmp106C=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106D="primop does not have 1 or 2 args!",_tag_dyneither(_tmp106D,sizeof(char),34))),_tag_dyneither(_tmp106C,sizeof(void*),0)));}}_LL4F9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7A9=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7A9->tag != 5)goto _LL4FB;else{_tmp7AA=_tmp7A9->f1;_tmp7AB=_tmp7A9->f2;_tmp7AC=_tmp7A9->f3;}}_LL4FA: {
# 3102
struct Cyc_Absyn_Exp*_tmp7D2=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AA);
struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AB);
struct Cyc_Absyn_Exp*_tmp7D4=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AC);
if((_tmp7D2 == _tmp7AA  && _tmp7D3 == _tmp7AB) && _tmp7D4 == _tmp7AC)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp1070;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp106F;return Cyc_Tcutil_copye(e,(void*)((_tmp106F=_cycalloc(sizeof(*_tmp106F)),((_tmp106F[0]=((_tmp1070.tag=5,((_tmp1070.f1=_tmp7D2,((_tmp1070.f2=_tmp7D3,((_tmp1070.f3=_tmp7D4,_tmp1070)))))))),_tmp106F)))));};}_LL4FB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp7AD=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7AD->tag != 6)goto _LL4FD;else{_tmp7AE=_tmp7AD->f1;_tmp7AF=_tmp7AD->f2;}}_LL4FC: {
# 3108
struct Cyc_Absyn_Exp*_tmp7D7=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AE);
struct Cyc_Absyn_Exp*_tmp7D8=Cyc_Tcutil_rsubsexp(r,inst,_tmp7AF);
if(_tmp7D7 == _tmp7AE  && _tmp7D8 == _tmp7AF)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1073;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1072;return Cyc_Tcutil_copye(e,(void*)((_tmp1072=_cycalloc(sizeof(*_tmp1072)),((_tmp1072[0]=((_tmp1073.tag=6,((_tmp1073.f1=_tmp7D7,((_tmp1073.f2=_tmp7D8,_tmp1073)))))),_tmp1072)))));};}_LL4FD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp7B0=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7B0->tag != 7)goto _LL4FF;else{_tmp7B1=_tmp7B0->f1;_tmp7B2=_tmp7B0->f2;}}_LL4FE: {
# 3113
struct Cyc_Absyn_Exp*_tmp7DB=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B1);
struct Cyc_Absyn_Exp*_tmp7DC=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B2);
if(_tmp7DB == _tmp7B1  && _tmp7DC == _tmp7B2)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1076;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1075;return Cyc_Tcutil_copye(e,(void*)((_tmp1075=_cycalloc(sizeof(*_tmp1075)),((_tmp1075[0]=((_tmp1076.tag=7,((_tmp1076.f1=_tmp7DB,((_tmp1076.f2=_tmp7DC,_tmp1076)))))),_tmp1075)))));};}_LL4FF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp7B3=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7B3->tag != 8)goto _LL501;else{_tmp7B4=_tmp7B3->f1;_tmp7B5=_tmp7B3->f2;}}_LL500: {
# 3118
struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B4);
struct Cyc_Absyn_Exp*_tmp7E0=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B5);
if(_tmp7DF == _tmp7B4  && _tmp7E0 == _tmp7B5)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp1079;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1078;return Cyc_Tcutil_copye(e,(void*)((_tmp1078=_cycalloc(sizeof(*_tmp1078)),((_tmp1078[0]=((_tmp1079.tag=8,((_tmp1079.f1=_tmp7DF,((_tmp1079.f2=_tmp7E0,_tmp1079)))))),_tmp1078)))));};}_LL501: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp7B6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7B6->tag != 13)goto _LL503;else{_tmp7B7=(void*)_tmp7B6->f1;_tmp7B8=_tmp7B6->f2;_tmp7B9=_tmp7B6->f3;_tmp7BA=_tmp7B6->f4;}}_LL502: {
# 3123
struct Cyc_Absyn_Exp*_tmp7E3=Cyc_Tcutil_rsubsexp(r,inst,_tmp7B8);
void*_tmp7E4=Cyc_Tcutil_rsubstitute(r,inst,_tmp7B7);
if(_tmp7E3 == _tmp7B8  && _tmp7E4 == _tmp7B7)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp107C;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp107B;return Cyc_Tcutil_copye(e,(void*)((_tmp107B=_cycalloc(sizeof(*_tmp107B)),((_tmp107B[0]=((_tmp107C.tag=13,((_tmp107C.f1=_tmp7E4,((_tmp107C.f2=_tmp7E3,((_tmp107C.f3=_tmp7B9,((_tmp107C.f4=_tmp7BA,_tmp107C)))))))))),_tmp107B)))));};}_LL503: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp7BB=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7BB->tag != 16)goto _LL505;else{_tmp7BC=(void*)_tmp7BB->f1;}}_LL504: {
# 3128
void*_tmp7E7=Cyc_Tcutil_rsubstitute(r,inst,_tmp7BC);
if(_tmp7E7 == _tmp7BC)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp107F;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp107E;return Cyc_Tcutil_copye(e,(void*)((_tmp107E=_cycalloc(sizeof(*_tmp107E)),((_tmp107E[0]=((_tmp107F.tag=16,((_tmp107F.f1=_tmp7E7,_tmp107F)))),_tmp107E)))));};}_LL505: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp7BD=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7BD->tag != 17)goto _LL507;else{_tmp7BE=_tmp7BD->f1;}}_LL506: {
# 3132
struct Cyc_Absyn_Exp*_tmp7EA=Cyc_Tcutil_rsubsexp(r,inst,_tmp7BE);
if(_tmp7EA == _tmp7BE)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp1082;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1081;return Cyc_Tcutil_copye(e,(void*)((_tmp1081=_cycalloc(sizeof(*_tmp1081)),((_tmp1081[0]=((_tmp1082.tag=17,((_tmp1082.f1=_tmp7EA,_tmp1082)))),_tmp1081)))));};}_LL507: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp7BF=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7BF->tag != 18)goto _LL509;else{_tmp7C0=(void*)_tmp7BF->f1;_tmp7C1=_tmp7BF->f2;}}_LL508: {
# 3136
void*_tmp7ED=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C0);
if(_tmp7ED == _tmp7C0)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1085;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1084;return Cyc_Tcutil_copye(e,(void*)((_tmp1084=_cycalloc(sizeof(*_tmp1084)),((_tmp1084[0]=((_tmp1085.tag=18,((_tmp1085.f1=_tmp7ED,((_tmp1085.f2=_tmp7C1,_tmp1085)))))),_tmp1084)))));};}_LL509: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp7C2=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7A1;if(_tmp7C2->tag != 38)goto _LL50B;else{_tmp7C3=(void*)_tmp7C2->f1;}}_LL50A: {
# 3140
void*_tmp7F0=Cyc_Tcutil_rsubstitute(r,inst,_tmp7C3);
if(_tmp7F0 == _tmp7C3)return e;{
# 3143
void*_stmttmp5B=Cyc_Tcutil_compress(_tmp7F0);void*_tmp7F1=_stmttmp5B;struct Cyc_Absyn_Exp*_tmp7F3;_LL50E: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F2=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp7F1;if(_tmp7F2->tag != 18)goto _LL510;else{_tmp7F3=_tmp7F2->f1;}}_LL50F:
 return _tmp7F3;_LL510:;_LL511: {
# 3146
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1088;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1087;return Cyc_Tcutil_copye(e,(void*)((_tmp1087=_cycalloc(sizeof(*_tmp1087)),((_tmp1087[0]=((_tmp1088.tag=38,((_tmp1088.f1=_tmp7F0,_tmp1088)))),_tmp1087)))));}_LL50D:;};}_LL50B:;_LL50C: {
# 3149
const char*_tmp108B;void*_tmp108A;return(_tmp108A=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp108B="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp108B,sizeof(char),46))),_tag_dyneither(_tmp108A,sizeof(void*),0)));}_LL4EE:;}
# 3153
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3156
void*_tmp7F8=f->type;
struct Cyc_Absyn_Exp*_tmp7F9=f->requires_clause;
void*_tmp7FA=Cyc_Tcutil_rsubstitute(r,inst,_tmp7F8);
struct Cyc_Absyn_Exp*_tmp7FB=_tmp7F9 == 0?0: Cyc_Tcutil_rsubsexp(r,inst,_tmp7F9);
if(_tmp7F8 == _tmp7FA  && _tmp7F9 == _tmp7FB)return f;else{
struct Cyc_Absyn_Aggrfield*_tmp108C;return(_tmp108C=_cycalloc(sizeof(*_tmp108C)),((_tmp108C->name=f->name,((_tmp108C->tq=f->tq,((_tmp108C->type=_tmp7FA,((_tmp108C->width=f->width,((_tmp108C->attributes=f->attributes,((_tmp108C->requires_clause=_tmp7FB,_tmp108C)))))))))))));}}
# 3166
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3169
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp7FD=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp7FE=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp7FF=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp7FD);
struct Cyc_List_List*_tmp800=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp7FE);
if(_tmp7FF == _tmp7FD  && _tmp800 == _tmp7FE)return fs;{
# 3176
struct Cyc_List_List*_tmp108D;return(_tmp108D=_cycalloc(sizeof(*_tmp108D)),((_tmp108D->hd=_tmp7FF,((_tmp108D->tl=_tmp800,_tmp108D)))));};};}
# 3179
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3182
struct _tuple1 _stmttmp5C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct Cyc_List_List*_tmp803;struct Cyc_List_List*_tmp804;struct _tuple1 _tmp802=_stmttmp5C;_tmp803=_tmp802.f1;_tmp804=_tmp802.f2;{
struct Cyc_List_List*_tmp805=Cyc_Tcutil_substs(rgn,inst,_tmp803);
struct Cyc_List_List*_tmp806=Cyc_Tcutil_substs(rgn,inst,_tmp804);
if(_tmp805 == _tmp803  && _tmp806 == _tmp804)
return rgn_po;else{
# 3188
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp805,_tmp806);}};}
# 3191
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3194
void*_stmttmp5D=Cyc_Tcutil_compress(t);void*_tmp807=_stmttmp5D;struct Cyc_Absyn_Tvar*_tmp809;union Cyc_Absyn_AggrInfoU _tmp80B;struct Cyc_List_List*_tmp80C;union Cyc_Absyn_DatatypeInfoU _tmp80E;struct Cyc_List_List*_tmp80F;union Cyc_Absyn_DatatypeFieldInfoU _tmp811;struct Cyc_List_List*_tmp812;struct _tuple2*_tmp814;struct Cyc_List_List*_tmp815;struct Cyc_Absyn_Typedefdecl*_tmp816;void*_tmp817;void*_tmp819;struct Cyc_Absyn_Tqual _tmp81A;struct Cyc_Absyn_Exp*_tmp81B;union Cyc_Absyn_Constraint*_tmp81C;unsigned int _tmp81D;void*_tmp81F;struct Cyc_Absyn_Tqual _tmp820;void*_tmp821;union Cyc_Absyn_Constraint*_tmp822;union Cyc_Absyn_Constraint*_tmp823;union Cyc_Absyn_Constraint*_tmp824;struct Cyc_List_List*_tmp826;void*_tmp827;struct Cyc_Absyn_Tqual _tmp828;void*_tmp829;struct Cyc_List_List*_tmp82A;int _tmp82B;struct Cyc_Absyn_VarargInfo*_tmp82C;struct Cyc_List_List*_tmp82D;struct Cyc_List_List*_tmp82E;struct Cyc_List_List*_tmp830;enum Cyc_Absyn_AggrKind _tmp832;struct Cyc_List_List*_tmp833;void*_tmp835;void*_tmp837;void*_tmp839;void*_tmp83A;void*_tmp83C;struct Cyc_Absyn_Exp*_tmp83E;void*_tmp848;void*_tmp84A;struct Cyc_List_List*_tmp84C;_LL513: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp808=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp807;if(_tmp808->tag != 2)goto _LL515;else{_tmp809=_tmp808->f1;}}_LL514: {
# 3197
struct _handler_cons _tmp84E;_push_handler(& _tmp84E);{int _tmp850=0;if(setjmp(_tmp84E.handler))_tmp850=1;if(!_tmp850){{void*_tmp851=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp809);_npop_handler(0);return _tmp851;};_pop_handler();}else{void*_tmp84F=(void*)_exn_thrown;void*_tmp853=_tmp84F;void*_tmp855;_LL54A: {struct Cyc_Core_Not_found_exn_struct*_tmp854=(struct Cyc_Core_Not_found_exn_struct*)_tmp853;if(_tmp854->tag != Cyc_Core_Not_found)goto _LL54C;}_LL54B:
 return t;_LL54C: _tmp855=_tmp853;_LL54D:(void)_throw(_tmp855);_LL549:;}};}_LL515: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp80A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp807;if(_tmp80A->tag != 11)goto _LL517;else{_tmp80B=(_tmp80A->f1).aggr_info;_tmp80C=(_tmp80A->f1).targs;}}_LL516: {
# 3200
struct Cyc_List_List*_tmp856=Cyc_Tcutil_substs(rgn,inst,_tmp80C);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1093;struct Cyc_Absyn_AggrInfo _tmp1092;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1091;return _tmp856 == _tmp80C?t:(void*)((_tmp1091=_cycalloc(sizeof(*_tmp1091)),((_tmp1091[0]=((_tmp1093.tag=11,((_tmp1093.f1=((_tmp1092.aggr_info=_tmp80B,((_tmp1092.targs=_tmp856,_tmp1092)))),_tmp1093)))),_tmp1091))));}_LL517: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp80D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp807;if(_tmp80D->tag != 3)goto _LL519;else{_tmp80E=(_tmp80D->f1).datatype_info;_tmp80F=(_tmp80D->f1).targs;}}_LL518: {
# 3203
struct Cyc_List_List*_tmp85A=Cyc_Tcutil_substs(rgn,inst,_tmp80F);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp1099;struct Cyc_Absyn_DatatypeInfo _tmp1098;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1097;return _tmp85A == _tmp80F?t:(void*)(
(_tmp1097=_cycalloc(sizeof(*_tmp1097)),((_tmp1097[0]=((_tmp1099.tag=3,((_tmp1099.f1=((_tmp1098.datatype_info=_tmp80E,((_tmp1098.targs=_tmp85A,_tmp1098)))),_tmp1099)))),_tmp1097))));}_LL519: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp810=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp807;if(_tmp810->tag != 4)goto _LL51B;else{_tmp811=(_tmp810->f1).field_info;_tmp812=(_tmp810->f1).targs;}}_LL51A: {
# 3207
struct Cyc_List_List*_tmp85E=Cyc_Tcutil_substs(rgn,inst,_tmp812);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp109F;struct Cyc_Absyn_DatatypeFieldInfo _tmp109E;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp109D;return _tmp85E == _tmp812?t:(void*)(
(_tmp109D=_cycalloc(sizeof(*_tmp109D)),((_tmp109D[0]=((_tmp109F.tag=4,((_tmp109F.f1=((_tmp109E.field_info=_tmp811,((_tmp109E.targs=_tmp85E,_tmp109E)))),_tmp109F)))),_tmp109D))));}_LL51B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp813=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp807;if(_tmp813->tag != 17)goto _LL51D;else{_tmp814=_tmp813->f1;_tmp815=_tmp813->f2;_tmp816=_tmp813->f3;_tmp817=(void*)_tmp813->f4;}}_LL51C: {
# 3211
struct Cyc_List_List*_tmp862=Cyc_Tcutil_substs(rgn,inst,_tmp815);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp10A2;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp10A1;return _tmp862 == _tmp815?t:(void*)((_tmp10A1=_cycalloc(sizeof(*_tmp10A1)),((_tmp10A1[0]=((_tmp10A2.tag=17,((_tmp10A2.f1=_tmp814,((_tmp10A2.f2=_tmp862,((_tmp10A2.f3=_tmp816,((_tmp10A2.f4=_tmp817,_tmp10A2)))))))))),_tmp10A1))));}_LL51D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp818=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp807;if(_tmp818->tag != 8)goto _LL51F;else{_tmp819=(_tmp818->f1).elt_type;_tmp81A=(_tmp818->f1).tq;_tmp81B=(_tmp818->f1).num_elts;_tmp81C=(_tmp818->f1).zero_term;_tmp81D=(_tmp818->f1).zt_loc;}}_LL51E: {
# 3214
void*_tmp865=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp819);
struct Cyc_Absyn_Exp*_tmp866=_tmp81B == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp81B);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp10A8;struct Cyc_Absyn_ArrayInfo _tmp10A7;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp10A6;return _tmp865 == _tmp819  && _tmp866 == _tmp81B?t:(void*)(
(_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((_tmp10A6[0]=((_tmp10A8.tag=8,((_tmp10A8.f1=((_tmp10A7.elt_type=_tmp865,((_tmp10A7.tq=_tmp81A,((_tmp10A7.num_elts=_tmp866,((_tmp10A7.zero_term=_tmp81C,((_tmp10A7.zt_loc=_tmp81D,_tmp10A7)))))))))),_tmp10A8)))),_tmp10A6))));}_LL51F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp81E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp807;if(_tmp81E->tag != 5)goto _LL521;else{_tmp81F=(_tmp81E->f1).elt_typ;_tmp820=(_tmp81E->f1).elt_tq;_tmp821=((_tmp81E->f1).ptr_atts).rgn;_tmp822=((_tmp81E->f1).ptr_atts).nullable;_tmp823=((_tmp81E->f1).ptr_atts).bounds;_tmp824=((_tmp81E->f1).ptr_atts).zero_term;}}_LL520: {
# 3219
void*_tmp86A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp81F);
void*_tmp86B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp821);
union Cyc_Absyn_Constraint*_tmp86C=_tmp823;
{void*_stmttmp60=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp823);void*_tmp86D=_stmttmp60;struct Cyc_Absyn_Exp*_tmp86F;_LL54F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp86E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp86D;if(_tmp86E->tag != 1)goto _LL551;else{_tmp86F=_tmp86E->f1;}}_LL550: {
# 3224
struct Cyc_Absyn_Exp*_tmp870=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp86F);
if(_tmp870 != _tmp86F){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp10AB;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp10AA;_tmp86C=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp10AA=_cycalloc(sizeof(*_tmp10AA)),((_tmp10AA[0]=((_tmp10AB.tag=1,((_tmp10AB.f1=_tmp870,_tmp10AB)))),_tmp10AA)))));}
goto _LL54E;}_LL551:;_LL552:
 goto _LL54E;_LL54E:;}
# 3230
if((_tmp86A == _tmp81F  && _tmp86B == _tmp821) && _tmp86C == _tmp823)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10B1;struct Cyc_Absyn_PtrInfo _tmp10B0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10AF;return(void*)((_tmp10AF=_cycalloc(sizeof(*_tmp10AF)),((_tmp10AF[0]=((_tmp10B1.tag=5,((_tmp10B1.f1=((_tmp10B0.elt_typ=_tmp86A,((_tmp10B0.elt_tq=_tmp820,((_tmp10B0.ptr_atts=(((_tmp10B0.ptr_atts).rgn=_tmp86B,(((_tmp10B0.ptr_atts).nullable=_tmp822,(((_tmp10B0.ptr_atts).bounds=_tmp86C,(((_tmp10B0.ptr_atts).zero_term=_tmp824,(((_tmp10B0.ptr_atts).ptrloc=0,_tmp10B0.ptr_atts)))))))))),_tmp10B0)))))),_tmp10B1)))),_tmp10AF))));};}_LL521: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp825=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp807;if(_tmp825->tag != 9)goto _LL523;else{_tmp826=(_tmp825->f1).tvars;_tmp827=(_tmp825->f1).effect;_tmp828=(_tmp825->f1).ret_tqual;_tmp829=(_tmp825->f1).ret_typ;_tmp82A=(_tmp825->f1).args;_tmp82B=(_tmp825->f1).c_varargs;_tmp82C=(_tmp825->f1).cyc_varargs;_tmp82D=(_tmp825->f1).rgn_po;_tmp82E=(_tmp825->f1).attributes;}}_LL522:
# 3235
{struct Cyc_List_List*_tmp876=_tmp826;for(0;_tmp876 != 0;_tmp876=_tmp876->tl){
struct _tuple16*_tmp10BB;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10BA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10B9;struct Cyc_List_List*_tmp10B8;inst=((_tmp10B8=_region_malloc(rgn,sizeof(*_tmp10B8)),((_tmp10B8->hd=((_tmp10BB=_region_malloc(rgn,sizeof(*_tmp10BB)),((_tmp10BB->f1=(struct Cyc_Absyn_Tvar*)_tmp876->hd,((_tmp10BB->f2=(void*)((_tmp10B9=_cycalloc(sizeof(*_tmp10B9)),((_tmp10B9[0]=((_tmp10BA.tag=2,((_tmp10BA.f1=(struct Cyc_Absyn_Tvar*)_tmp876->hd,_tmp10BA)))),_tmp10B9)))),_tmp10BB)))))),((_tmp10B8->tl=inst,_tmp10B8))))));}}{
struct _tuple1 _stmttmp5E=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp82A));
# 3237
struct Cyc_List_List*_tmp87C;struct Cyc_List_List*_tmp87D;struct _tuple1 _tmp87B=_stmttmp5E;_tmp87C=_tmp87B.f1;_tmp87D=_tmp87B.f2;{
# 3239
struct Cyc_List_List*_tmp87E=_tmp82A;
struct Cyc_List_List*_tmp87F=Cyc_Tcutil_substs(rgn,inst,_tmp87D);
if(_tmp87F != _tmp87D)
_tmp87E=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp82A,_tmp87F);{
void*eff2;
if(_tmp827 == 0)
eff2=0;else{
# 3247
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp827);
if(new_eff == _tmp827)
eff2=_tmp827;else{
# 3251
eff2=new_eff;}}{
# 3253
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp82C == 0)
cyc_varargs2=0;else{
# 3257
struct Cyc_Absyn_VarargInfo _stmttmp5F=*_tmp82C;struct _dyneither_ptr*_tmp881;struct Cyc_Absyn_Tqual _tmp882;void*_tmp883;int _tmp884;struct Cyc_Absyn_VarargInfo _tmp880=_stmttmp5F;_tmp881=_tmp880.name;_tmp882=_tmp880.tq;_tmp883=_tmp880.type;_tmp884=_tmp880.inject;{
void*_tmp885=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp883);
if(_tmp885 == _tmp883)cyc_varargs2=_tmp82C;else{
# 3261
struct Cyc_Absyn_VarargInfo*_tmp10BC;cyc_varargs2=((_tmp10BC=_cycalloc(sizeof(*_tmp10BC)),((_tmp10BC->name=_tmp881,((_tmp10BC->tq=_tmp882,((_tmp10BC->type=_tmp885,((_tmp10BC->inject=_tmp884,_tmp10BC))))))))));}};}{
# 3263
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp82D);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10C2;struct Cyc_Absyn_FnInfo _tmp10C1;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10C0;return(void*)(
(_tmp10C0=_cycalloc(sizeof(*_tmp10C0)),((_tmp10C0[0]=((_tmp10C2.tag=9,((_tmp10C2.f1=((_tmp10C1.tvars=_tmp826,((_tmp10C1.effect=eff2,((_tmp10C1.ret_tqual=_tmp828,((_tmp10C1.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp829),((_tmp10C1.args=_tmp87E,((_tmp10C1.c_varargs=_tmp82B,((_tmp10C1.cyc_varargs=cyc_varargs2,((_tmp10C1.rgn_po=rgn_po2,((_tmp10C1.attributes=_tmp82E,_tmp10C1)))))))))))))))))),_tmp10C2)))),_tmp10C0))));};};};};};_LL523: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp82F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp807;if(_tmp82F->tag != 10)goto _LL525;else{_tmp830=_tmp82F->f1;}}_LL524: {
# 3268
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp88A=_tmp830;for(0;_tmp88A != 0;_tmp88A=_tmp88A->tl){
void*_tmp88B=(*((struct _tuple12*)_tmp88A->hd)).f2;
void*_tmp88C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp88B);
if(_tmp88B != _tmp88C)
change=1;{
# 3276
struct Cyc_List_List*_tmp10C3;ts2=((_tmp10C3=_region_malloc(rgn,sizeof(*_tmp10C3)),((_tmp10C3->hd=_tmp88C,((_tmp10C3->tl=ts2,_tmp10C3))))));};}}
# 3278
if(!change)
return t;{
struct Cyc_List_List*_tmp88E=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp830,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp10C6;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp10C5;return(void*)((_tmp10C5=_cycalloc(sizeof(*_tmp10C5)),((_tmp10C5[0]=((_tmp10C6.tag=10,((_tmp10C6.f1=_tmp88E,_tmp10C6)))),_tmp10C5))));};}_LL525: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp831=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp807;if(_tmp831->tag != 12)goto _LL527;else{_tmp832=_tmp831->f1;_tmp833=_tmp831->f2;}}_LL526: {
# 3283
struct Cyc_List_List*_tmp891=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp833);
if(_tmp833 == _tmp891)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp10C9;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp10C8;return(void*)((_tmp10C8=_cycalloc(sizeof(*_tmp10C8)),((_tmp10C8[0]=((_tmp10C9.tag=12,((_tmp10C9.f1=_tmp832,((_tmp10C9.f2=_tmp891,_tmp10C9)))))),_tmp10C8))));};}_LL527: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp834=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp807;if(_tmp834->tag != 1)goto _LL529;else{_tmp835=(void*)_tmp834->f2;}}_LL528:
# 3287
 if(_tmp835 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp835);else{
return t;}_LL529: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp836=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp807;if(_tmp836->tag != 15)goto _LL52B;else{_tmp837=(void*)_tmp836->f1;}}_LL52A: {
# 3290
void*_tmp894=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp837);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp10CC;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp10CB;return _tmp894 == _tmp837?t:(void*)((_tmp10CB=_cycalloc(sizeof(*_tmp10CB)),((_tmp10CB[0]=((_tmp10CC.tag=15,((_tmp10CC.f1=_tmp894,_tmp10CC)))),_tmp10CB))));}_LL52B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp838=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp807;if(_tmp838->tag != 16)goto _LL52D;else{_tmp839=(void*)_tmp838->f1;_tmp83A=(void*)_tmp838->f2;}}_LL52C: {
# 3293
void*_tmp897=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp839);
void*_tmp898=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83A);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp10CF;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp10CE;return _tmp897 == _tmp839  && _tmp898 == _tmp83A?t:(void*)((_tmp10CE=_cycalloc(sizeof(*_tmp10CE)),((_tmp10CE[0]=((_tmp10CF.tag=16,((_tmp10CF.f1=_tmp897,((_tmp10CF.f2=_tmp898,_tmp10CF)))))),_tmp10CE))));}_LL52D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp83B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp807;if(_tmp83B->tag != 19)goto _LL52F;else{_tmp83C=(void*)_tmp83B->f1;}}_LL52E: {
# 3297
void*_tmp89B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp83C);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp10D2;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp10D1;return _tmp89B == _tmp83C?t:(void*)((_tmp10D1=_cycalloc(sizeof(*_tmp10D1)),((_tmp10D1[0]=((_tmp10D2.tag=19,((_tmp10D2.f1=_tmp89B,_tmp10D2)))),_tmp10D1))));}_LL52F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp83D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp807;if(_tmp83D->tag != 18)goto _LL531;else{_tmp83E=_tmp83D->f1;}}_LL530: {
# 3300
struct Cyc_Absyn_Exp*_tmp89E=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp83E);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp10D5;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp10D4;return _tmp89E == _tmp83E?t:(void*)((_tmp10D4=_cycalloc(sizeof(*_tmp10D4)),((_tmp10D4[0]=((_tmp10D5.tag=18,((_tmp10D5.f1=_tmp89E,_tmp10D5)))),_tmp10D4))));}_LL531: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp83F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp807;if(_tmp83F->tag != 13)goto _LL533;}_LL532:
 goto _LL534;_LL533: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp840=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp807;if(_tmp840->tag != 14)goto _LL535;}_LL534:
 goto _LL536;_LL535: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp841=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp807;if(_tmp841->tag != 0)goto _LL537;}_LL536:
 goto _LL538;_LL537: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp842=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp807;if(_tmp842->tag != 6)goto _LL539;}_LL538:
 goto _LL53A;_LL539: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp843=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp807;if(_tmp843->tag != 7)goto _LL53B;}_LL53A:
 goto _LL53C;_LL53B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp844=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp807;if(_tmp844->tag != 22)goto _LL53D;}_LL53C:
 goto _LL53E;_LL53D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp845=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp807;if(_tmp845->tag != 21)goto _LL53F;}_LL53E:
 goto _LL540;_LL53F: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp846=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp807;if(_tmp846->tag != 20)goto _LL541;}_LL540:
 return t;_LL541: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp847=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp807;if(_tmp847->tag != 25)goto _LL543;else{_tmp848=(void*)_tmp847->f1;}}_LL542: {
# 3311
void*_tmp8A1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp848);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp10D8;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp10D7;return _tmp8A1 == _tmp848?t:(void*)((_tmp10D7=_cycalloc(sizeof(*_tmp10D7)),((_tmp10D7[0]=((_tmp10D8.tag=25,((_tmp10D8.f1=_tmp8A1,_tmp10D8)))),_tmp10D7))));}_LL543: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp849=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp807;if(_tmp849->tag != 23)goto _LL545;else{_tmp84A=(void*)_tmp849->f1;}}_LL544: {
# 3314
void*_tmp8A4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp84A);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp10DB;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp10DA;return _tmp8A4 == _tmp84A?t:(void*)((_tmp10DA=_cycalloc(sizeof(*_tmp10DA)),((_tmp10DA[0]=((_tmp10DB.tag=23,((_tmp10DB.f1=_tmp8A4,_tmp10DB)))),_tmp10DA))));}_LL545: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp84B=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp807;if(_tmp84B->tag != 24)goto _LL547;else{_tmp84C=_tmp84B->f1;}}_LL546: {
# 3317
struct Cyc_List_List*_tmp8A7=Cyc_Tcutil_substs(rgn,inst,_tmp84C);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp10DE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp10DD;return _tmp8A7 == _tmp84C?t:(void*)((_tmp10DD=_cycalloc(sizeof(*_tmp10DD)),((_tmp10DD[0]=((_tmp10DE.tag=24,((_tmp10DE.f1=_tmp8A7,_tmp10DE)))),_tmp10DD))));}_LL547: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp84D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp807;if(_tmp84D->tag != 26)goto _LL512;}_LL548: {
const char*_tmp10E1;void*_tmp10E0;(_tmp10E0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10E1="found typedecltype in rsubs",_tag_dyneither(_tmp10E1,sizeof(char),28))),_tag_dyneither(_tmp10E0,sizeof(void*),0)));}_LL512:;}
# 3323
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3326
if(ts == 0)
return 0;{
void*_tmp8AC=(void*)ts->hd;
struct Cyc_List_List*_tmp8AD=ts->tl;
void*_tmp8AE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8AC);
struct Cyc_List_List*_tmp8AF=Cyc_Tcutil_substs(rgn,inst,_tmp8AD);
if(_tmp8AC == _tmp8AE  && _tmp8AD == _tmp8AF)
return ts;{
struct Cyc_List_List*_tmp10E2;return(_tmp10E2=_cycalloc(sizeof(*_tmp10E2)),((_tmp10E2->hd=_tmp8AE,((_tmp10E2->tl=_tmp8AF,_tmp10E2)))));};};}
# 3337
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3344
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp8B1=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10E5;struct _tuple16*_tmp10E4;return(_tmp10E4=_cycalloc(sizeof(*_tmp10E4)),((_tmp10E4->f1=tv,((_tmp10E4->f2=Cyc_Absyn_new_evar(_tmp8B1,((_tmp10E5=_cycalloc(sizeof(*_tmp10E5)),((_tmp10E5->v=s,_tmp10E5))))),_tmp10E4)))));}
# 3349
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3351
struct Cyc_List_List*_tmp8B5;struct _RegionHandle*_tmp8B6;struct _tuple17*_tmp8B4=env;_tmp8B5=_tmp8B4->f1;_tmp8B6=_tmp8B4->f2;{
struct Cyc_Core_Opt*_tmp8B7=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp10E8;struct _tuple16*_tmp10E7;return(_tmp10E7=_region_malloc(_tmp8B6,sizeof(*_tmp10E7)),((_tmp10E7->f1=tv,((_tmp10E7->f2=Cyc_Absyn_new_evar(_tmp8B7,((_tmp10E8=_cycalloc(sizeof(*_tmp10E8)),((_tmp10E8->v=_tmp8B5,_tmp10E8))))),_tmp10E7)))));};}
# 3361
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3365
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp10EE;void*_tmp10ED[3];struct Cyc_String_pa_PrintArg_struct _tmp10EC;struct Cyc_String_pa_PrintArg_struct _tmp10EB;struct Cyc_String_pa_PrintArg_struct _tmp10EA;(_tmp10EA.tag=0,((_tmp10EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp10EB.tag=0,((_tmp10EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp10EC.tag=0,((_tmp10EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp10ED[0]=& _tmp10EC,((_tmp10ED[1]=& _tmp10EB,((_tmp10ED[2]=& _tmp10EA,Cyc_Tcutil_terr(loc,((_tmp10EE="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp10EE,sizeof(char),59))),_tag_dyneither(_tmp10ED,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp10F1;void*_tmp10F0;(_tmp10F0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F1="same type variable has different identity!",_tag_dyneither(_tmp10F1,sizeof(char),43))),_tag_dyneither(_tmp10F0,sizeof(void*),0)));}}
return tvs;}}}
# 3379
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp10F2;return(_tmp10F2=_cycalloc(sizeof(*_tmp10F2)),((_tmp10F2->hd=tv,((_tmp10F2->tl=tvs,_tmp10F2)))));};}
# 3385
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3387
if(tv->identity == - 1){
const char*_tmp10F5;void*_tmp10F4;(_tmp10F4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F5="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp10F5,sizeof(char),39))),_tag_dyneither(_tmp10F4,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3391
struct Cyc_Absyn_Tvar*_tmp8C4=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp8C4->identity == - 1){
const char*_tmp10F8;void*_tmp10F7;(_tmp10F7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F8="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp10F8,sizeof(char),41))),_tag_dyneither(_tmp10F7,sizeof(void*),0)));}
if(_tmp8C4->identity == tv->identity)
return tvs;}}{
# 3398
struct Cyc_List_List*_tmp10F9;return(_tmp10F9=_cycalloc(sizeof(*_tmp10F9)),((_tmp10F9->hd=tv,((_tmp10F9->tl=tvs,_tmp10F9)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3404
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3409
if(tv->identity == - 1){
const char*_tmp10FC;void*_tmp10FB;(_tmp10FB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FC="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp10FC,sizeof(char),44))),_tag_dyneither(_tmp10FB,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3413
struct _tuple28*_stmttmp61=(struct _tuple28*)tvs2->hd;struct Cyc_Absyn_Tvar*_tmp8CB;int*_tmp8CC;struct _tuple28*_tmp8CA=_stmttmp61;_tmp8CB=_tmp8CA->f1;_tmp8CC=(int*)& _tmp8CA->f2;
if(_tmp8CB->identity == - 1){
const char*_tmp10FF;void*_tmp10FE;(_tmp10FE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FF="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp10FF,sizeof(char),46))),_tag_dyneither(_tmp10FE,sizeof(void*),0)));}
if(_tmp8CB->identity == tv->identity){
*_tmp8CC=*_tmp8CC  || b;
return tvs;}}}{
# 3421
struct _tuple28*_tmp1102;struct Cyc_List_List*_tmp1101;return(_tmp1101=_region_malloc(r,sizeof(*_tmp1101)),((_tmp1101->hd=((_tmp1102=_region_malloc(r,sizeof(*_tmp1102)),((_tmp1102->f1=tv,((_tmp1102->f2=b,_tmp1102)))))),((_tmp1101->tl=tvs,_tmp1101)))));};}
# 3425
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3427
if(tv->identity == - 1){
const char*_tmp1106;void*_tmp1105[1];struct Cyc_String_pa_PrintArg_struct _tmp1104;(_tmp1104.tag=0,((_tmp1104.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp1105[0]=& _tmp1104,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1106="bound tvar id for %s is NULL",_tag_dyneither(_tmp1106,sizeof(char),29))),_tag_dyneither(_tmp1105,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp1107;return(_tmp1107=_cycalloc(sizeof(*_tmp1107)),((_tmp1107->hd=tv,((_tmp1107->tl=tvs,_tmp1107)))));};}struct _tuple29{void*f1;int f2;};
# 3436
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3439
void*_stmttmp62=Cyc_Tcutil_compress(e);void*_tmp8D5=_stmttmp62;int _tmp8D7;_LL554: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8D6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8D5;if(_tmp8D6->tag != 1)goto _LL556;else{_tmp8D7=_tmp8D6->f3;}}_LL555:
# 3441
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_stmttmp63=(struct _tuple29*)es2->hd;void*_tmp8D9;int*_tmp8DA;struct _tuple29*_tmp8D8=_stmttmp63;_tmp8D9=_tmp8D8->f1;_tmp8DA=(int*)& _tmp8D8->f2;{
void*_stmttmp64=Cyc_Tcutil_compress(_tmp8D9);void*_tmp8DB=_stmttmp64;int _tmp8DD;_LL559: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8DC=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8DB;if(_tmp8DC->tag != 1)goto _LL55B;else{_tmp8DD=_tmp8DC->f3;}}_LL55A:
# 3445
 if(_tmp8D7 == _tmp8DD){
if(b != *_tmp8DA)*_tmp8DA=1;
return es;}
# 3449
goto _LL558;_LL55B:;_LL55C:
 goto _LL558;_LL558:;};}}{
# 3453
struct _tuple29*_tmp110A;struct Cyc_List_List*_tmp1109;return(_tmp1109=_region_malloc(r,sizeof(*_tmp1109)),((_tmp1109->hd=((_tmp110A=_region_malloc(r,sizeof(*_tmp110A)),((_tmp110A->f1=e,((_tmp110A->f2=b,_tmp110A)))))),((_tmp1109->tl=es,_tmp1109)))));};_LL556:;_LL557:
 return es;_LL553:;}
# 3458
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3461
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3470
if(!present){struct Cyc_List_List*_tmp110B;r=((_tmp110B=_region_malloc(rgn,sizeof(*_tmp110B)),((_tmp110B->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp110B->tl=r,_tmp110B))))));}}
# 3472
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3477
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3481
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _stmttmp65=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp8E2;int _tmp8E3;struct _tuple28 _tmp8E1=_stmttmp65;_tmp8E2=_tmp8E1.f1;_tmp8E3=_tmp8E1.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp8E2->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3491
if(!present){struct Cyc_List_List*_tmp110C;res=((_tmp110C=_region_malloc(r,sizeof(*_tmp110C)),((_tmp110C->hd=(struct _tuple28*)tvs->hd,((_tmp110C->tl=res,_tmp110C))))));}};}
# 3493
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3497
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3499
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp1110;void*_tmp110F[1];struct Cyc_String_pa_PrintArg_struct _tmp110E;(_tmp110E.tag=0,((_tmp110E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp110F[0]=& _tmp110E,Cyc_Tcutil_terr(loc,((_tmp1110="bitfield %s does not have constant width",_tag_dyneither(_tmp1110,sizeof(char),41))),_tag_dyneither(_tmp110F,sizeof(void*),1)))))));}else{
# 3504
struct _tuple14 _stmttmp66=Cyc_Evexp_eval_const_uint_exp(width);unsigned int _tmp8E9;int _tmp8EA;struct _tuple14 _tmp8E8=_stmttmp66;_tmp8E9=_tmp8E8.f1;_tmp8EA=_tmp8E8.f2;
if(!_tmp8EA){
const char*_tmp1113;void*_tmp1112;(_tmp1112=0,Cyc_Tcutil_terr(loc,((_tmp1113="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1113,sizeof(char),45))),_tag_dyneither(_tmp1112,sizeof(void*),0)));}
w=_tmp8E9;}{
# 3509
void*_stmttmp67=Cyc_Tcutil_compress(field_typ);void*_tmp8ED=_stmttmp67;enum Cyc_Absyn_Size_of _tmp8EF;_LL55E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8EE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8ED;if(_tmp8EE->tag != 6)goto _LL560;else{_tmp8EF=_tmp8EE->f2;}}_LL55F:
# 3512
 switch(_tmp8EF){case Cyc_Absyn_Char_sz: _LL562:
 if(w > 8){const char*_tmp1116;void*_tmp1115;(_tmp1115=0,Cyc_Tcutil_terr(loc,((_tmp1116="bitfield larger than type",_tag_dyneither(_tmp1116,sizeof(char),26))),_tag_dyneither(_tmp1115,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL563:
 if(w > 16){const char*_tmp1119;void*_tmp1118;(_tmp1118=0,Cyc_Tcutil_terr(loc,((_tmp1119="bitfield larger than type",_tag_dyneither(_tmp1119,sizeof(char),26))),_tag_dyneither(_tmp1118,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL564:
 goto _LL565;case Cyc_Absyn_Int_sz: _LL565:
# 3517
 if(w > 32){const char*_tmp111C;void*_tmp111B;(_tmp111B=0,Cyc_Tcutil_terr(loc,((_tmp111C="bitfield larger than type",_tag_dyneither(_tmp111C,sizeof(char),26))),_tag_dyneither(_tmp111B,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL566:
# 3519
 if(w > 64){const char*_tmp111F;void*_tmp111E;(_tmp111E=0,Cyc_Tcutil_terr(loc,((_tmp111F="bitfield larger than type",_tag_dyneither(_tmp111F,sizeof(char),26))),_tag_dyneither(_tmp111E,sizeof(void*),0)));}break;}
# 3521
goto _LL55D;_LL560:;_LL561:
# 3523
{const char*_tmp1124;void*_tmp1123[2];struct Cyc_String_pa_PrintArg_struct _tmp1122;struct Cyc_String_pa_PrintArg_struct _tmp1121;(_tmp1121.tag=0,((_tmp1121.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp1122.tag=0,((_tmp1122.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1123[0]=& _tmp1122,((_tmp1123[1]=& _tmp1121,Cyc_Tcutil_terr(loc,((_tmp1124="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1124,sizeof(char),52))),_tag_dyneither(_tmp1123,sizeof(void*),2)))))))))))));}
goto _LL55D;_LL55D:;};}}
# 3530
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_stmttmp68=(void*)atts->hd;void*_tmp8FC=_stmttmp68;_LL569: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp8FD=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp8FC;if(_tmp8FD->tag != 7)goto _LL56B;}_LL56A:
 continue;_LL56B: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp8FE=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp8FC;if(_tmp8FE->tag != 6)goto _LL56D;}_LL56C:
 continue;_LL56D:;_LL56E: {
const char*_tmp1129;void*_tmp1128[2];struct Cyc_String_pa_PrintArg_struct _tmp1127;struct Cyc_String_pa_PrintArg_struct _tmp1126;(_tmp1126.tag=0,((_tmp1126.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1127.tag=0,((_tmp1127.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1128[0]=& _tmp1127,((_tmp1128[1]=& _tmp1126,Cyc_Tcutil_terr(loc,((_tmp1129="bad attribute %s on member %s",_tag_dyneither(_tmp1129,sizeof(char),30))),_tag_dyneither(_tmp1128,sizeof(void*),2)))))))))))));}_LL568:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3554
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3558
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp903=t;struct Cyc_Absyn_Typedefdecl*_tmp905;void*_tmp906;_LL570: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp904=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp903;if(_tmp904->tag != 17)goto _LL572;else{_tmp905=_tmp904->f3;_tmp906=(void*)_tmp904->f4;}}_LL571:
# 3561
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp905))->tq).real_const  || (_tmp905->tq).print_const){
if(declared_const){const char*_tmp112C;void*_tmp112B;(_tmp112B=0,Cyc_Tcutil_warn(loc,((_tmp112C="extra const",_tag_dyneither(_tmp112C,sizeof(char),12))),_tag_dyneither(_tmp112B,sizeof(void*),0)));}
return 1;}
# 3566
if((unsigned int)_tmp906)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp906);else{
return declared_const;}_LL572:;_LL573:
 return declared_const;_LL56F:;}
# 3573
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_stmttmp69=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp909=_stmttmp69;void*_tmp90B;_LL575: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp90A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp909;if(_tmp90A->tag != 5)goto _LL577;else{_tmp90B=((_tmp90A->f1).ptr_atts).rgn;}}_LL576:
# 3578
{void*_stmttmp6A=Cyc_Tcutil_compress(_tmp90B);void*_tmp90C=_stmttmp6A;struct Cyc_Absyn_Tvar*_tmp90E;_LL57A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp90D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp90C;if(_tmp90D->tag != 2)goto _LL57C;else{_tmp90E=_tmp90D->f1;}}_LL57B:
# 3580
 return Cyc_Absyn_tvar_cmp(tvar,_tmp90E)== 0;_LL57C:;_LL57D:
 goto _LL579;_LL579:;}
# 3583
goto _LL574;_LL577:;_LL578:
 goto _LL574;_LL574:;}}else{
# 3589
return 1;}
return 0;}
# 3593
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3596
void*_stmttmp6B=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp90F=_stmttmp6B;_LL57F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp910=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp90F;if(_tmp910->tag != 2)goto _LL581;else{if((_tmp910->f2)->kind != Cyc_Absyn_RgnKind)goto _LL581;if((_tmp910->f2)->aliasqual != Cyc_Absyn_Top)goto _LL581;}}_LL580:
 goto _LL582;_LL581: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp911=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp90F;if(_tmp911->tag != 0)goto _LL583;else{if((_tmp911->f1)->kind != Cyc_Absyn_RgnKind)goto _LL583;if((_tmp911->f1)->aliasqual != Cyc_Absyn_Top)goto _LL583;}}_LL582:
# 3605
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3608
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3614
return& Cyc_Tcutil_trk;_LL583:;_LL584:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL57E:;}
# 3649 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3652
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3659
{void*_stmttmp6C=Cyc_Tcutil_compress(t);void*_tmp912=_stmttmp6C;struct Cyc_Core_Opt**_tmp915;void**_tmp916;struct Cyc_Absyn_Tvar*_tmp918;void*_tmp91A;void***_tmp91B;struct Cyc_List_List*_tmp91D;struct _tuple2*_tmp91F;struct Cyc_Absyn_Enumdecl**_tmp920;union Cyc_Absyn_DatatypeInfoU*_tmp922;struct Cyc_List_List**_tmp923;union Cyc_Absyn_DatatypeFieldInfoU*_tmp925;struct Cyc_List_List*_tmp926;void*_tmp928;struct Cyc_Absyn_Tqual*_tmp929;void*_tmp92A;union Cyc_Absyn_Constraint*_tmp92B;union Cyc_Absyn_Constraint*_tmp92C;union Cyc_Absyn_Constraint*_tmp92D;void*_tmp92F;struct Cyc_Absyn_Exp*_tmp931;void*_tmp935;struct Cyc_Absyn_Tqual*_tmp936;struct Cyc_Absyn_Exp**_tmp937;union Cyc_Absyn_Constraint*_tmp938;unsigned int _tmp939;struct Cyc_List_List**_tmp93B;void**_tmp93C;struct Cyc_Absyn_Tqual*_tmp93D;void*_tmp93E;struct Cyc_List_List*_tmp93F;int _tmp940;struct Cyc_Absyn_VarargInfo*_tmp941;struct Cyc_List_List*_tmp942;struct Cyc_List_List*_tmp943;struct Cyc_List_List*_tmp945;enum Cyc_Absyn_AggrKind _tmp947;struct Cyc_List_List*_tmp948;union Cyc_Absyn_AggrInfoU*_tmp94A;struct Cyc_List_List**_tmp94B;struct _tuple2*_tmp94D;struct Cyc_List_List**_tmp94E;struct Cyc_Absyn_Typedefdecl**_tmp94F;void**_tmp950;void*_tmp955;void*_tmp957;void*_tmp958;void*_tmp95A;void*_tmp95C;struct Cyc_List_List*_tmp95E;_LL586: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp913=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp912;if(_tmp913->tag != 0)goto _LL588;}_LL587:
 goto _LL585;_LL588: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp914=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp912;if(_tmp914->tag != 1)goto _LL58A;else{_tmp915=(struct Cyc_Core_Opt**)& _tmp914->f1;_tmp916=(void**)((void**)& _tmp914->f2);}}_LL589:
# 3663
 if(*_tmp915 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp915))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp915))->v,expected_kind))
*_tmp915=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3668
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp916=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3671
*_tmp916=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp112F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp112E;struct Cyc_Absyn_Tvar*_tmp95F=Cyc_Tcutil_new_tvar((void*)((_tmp112E=_cycalloc(sizeof(*_tmp112E)),((_tmp112E[0]=((_tmp112F.tag=2,((_tmp112F.f1=0,((_tmp112F.f2=expected_kind,_tmp112F)))))),_tmp112E)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1132;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1131;*_tmp916=(void*)((_tmp1131=_cycalloc(sizeof(*_tmp1131)),((_tmp1131[0]=((_tmp1132.tag=2,((_tmp1132.f1=_tmp95F,_tmp1132)))),_tmp1131))));}
_tmp918=_tmp95F;goto _LL58B;}else{
# 3677
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3679
goto _LL585;_LL58A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp917=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp912;if(_tmp917->tag != 2)goto _LL58C;else{_tmp918=_tmp917->f1;}}_LL58B:
# 3681
{void*_stmttmp8F=Cyc_Absyn_compress_kb(_tmp918->kind);void*_tmp964=_stmttmp8F;struct Cyc_Core_Opt**_tmp966;_LL5BD: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp965=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp964;if(_tmp965->tag != 1)goto _LL5BF;else{_tmp966=(struct Cyc_Core_Opt**)& _tmp965->f1;}}_LL5BE:
# 3683
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1138;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1137;struct Cyc_Core_Opt*_tmp1136;*_tmp966=((_tmp1136=_cycalloc(sizeof(*_tmp1136)),((_tmp1136->v=(void*)((_tmp1138=_cycalloc(sizeof(*_tmp1138)),((_tmp1138[0]=((_tmp1137.tag=2,((_tmp1137.f1=0,((_tmp1137.f2=expected_kind,_tmp1137)))))),_tmp1138)))),_tmp1136))));}goto _LL5BC;_LL5BF:;_LL5C0:
 goto _LL5BC;_LL5BC:;}
# 3688
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp918);
# 3691
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp918,put_in_effect);
# 3693
{void*_stmttmp90=Cyc_Absyn_compress_kb(_tmp918->kind);void*_tmp96A=_stmttmp90;struct Cyc_Core_Opt**_tmp96C;struct Cyc_Absyn_Kind*_tmp96D;_LL5C2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp96B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp96A;if(_tmp96B->tag != 2)goto _LL5C4;else{_tmp96C=(struct Cyc_Core_Opt**)& _tmp96B->f1;_tmp96D=_tmp96B->f2;}}_LL5C3:
# 3695
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp96D)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp113E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp113D;struct Cyc_Core_Opt*_tmp113C;*_tmp96C=((_tmp113C=_cycalloc(sizeof(*_tmp113C)),((_tmp113C->v=(void*)((_tmp113E=_cycalloc(sizeof(*_tmp113E)),((_tmp113E[0]=((_tmp113D.tag=2,((_tmp113D.f1=0,((_tmp113D.f2=expected_kind,_tmp113D)))))),_tmp113E)))),_tmp113C))));}
goto _LL5C1;_LL5C4:;_LL5C5:
 goto _LL5C1;_LL5C1:;}
# 3700
goto _LL585;_LL58C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp919=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp912;if(_tmp919->tag != 26)goto _LL58E;else{_tmp91A=(_tmp919->f1)->r;_tmp91B=(void***)& _tmp919->f2;}}_LL58D: {
# 3706
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp971=_tmp91A;struct Cyc_Absyn_Aggrdecl*_tmp973;struct Cyc_Absyn_Enumdecl*_tmp975;struct Cyc_Absyn_Datatypedecl*_tmp977;_LL5C7: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp972=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp971;if(_tmp972->tag != 0)goto _LL5C9;else{_tmp973=_tmp972->f1;}}_LL5C8:
# 3709
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp973);goto _LL5C6;_LL5C9: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp974=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp971;if(_tmp974->tag != 1)goto _LL5CB;else{_tmp975=_tmp974->f1;}}_LL5CA:
# 3711
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp975);goto _LL5C6;_LL5CB: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp976=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp971;if(_tmp976->tag != 2)goto _LL5C6;else{_tmp977=_tmp976->f1;}}_LL5CC:
# 3713
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp977);goto _LL5C6;_LL5C6:;}
# 3715
{void**_tmp113F;*_tmp91B=((_tmp113F=_cycalloc(sizeof(*_tmp113F)),((_tmp113F[0]=new_t,_tmp113F))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL58E: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp91C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp912;if(_tmp91C->tag != 14)goto _LL590;else{_tmp91D=_tmp91C->f1;}}_LL58F: {
# 3720
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp979=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp979;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp91D != 0;_tmp91D=_tmp91D->tl){
struct Cyc_Absyn_Enumfield*_tmp97A=(struct Cyc_Absyn_Enumfield*)_tmp91D->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp97A->name).f2)){
const char*_tmp1143;void*_tmp1142[1];struct Cyc_String_pa_PrintArg_struct _tmp1141;(_tmp1141.tag=0,((_tmp1141.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp97A->name).f2),((_tmp1142[0]=& _tmp1141,Cyc_Tcutil_terr(_tmp97A->loc,((_tmp1143="duplicate enum field name %s",_tag_dyneither(_tmp1143,sizeof(char),29))),_tag_dyneither(_tmp1142,sizeof(void*),1)))))));}else{
# 3729
struct Cyc_List_List*_tmp1144;prev_fields=((_tmp1144=_region_malloc(uprev_rgn,sizeof(*_tmp1144)),((_tmp1144->hd=(*_tmp97A->name).f2,((_tmp1144->tl=prev_fields,_tmp1144))))));}
# 3731
if(_tmp97A->tag == 0)
_tmp97A->tag=Cyc_Absyn_uint_exp(tag_count,_tmp97A->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp97A->tag))){
const char*_tmp1148;void*_tmp1147[1];struct Cyc_String_pa_PrintArg_struct _tmp1146;(_tmp1146.tag=0,((_tmp1146.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp97A->name).f2),((_tmp1147[0]=& _tmp1146,Cyc_Tcutil_terr(loc,((_tmp1148="enum field %s: expression is not constant",_tag_dyneither(_tmp1148,sizeof(char),42))),_tag_dyneither(_tmp1147,sizeof(void*),1)))))));}}{
# 3736
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp97A->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _stmttmp8E=(*_tmp97A->name).f1;union Cyc_Absyn_Nmspace _tmp982=_stmttmp8E;_LL5CE: if((_tmp982.Rel_n).tag != 1)goto _LL5D0;_LL5CF:
# 3740
(*_tmp97A->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5CD;_LL5D0:;_LL5D1:
 goto _LL5CD;_LL5CD:;}{
# 3743
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp114E;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp114D;struct _tuple29*_tmp114C;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp97A->name).f2,(
(_tmp114C=_cycalloc(sizeof(*_tmp114C)),((_tmp114C->f1=(void*)((_tmp114E=_cycalloc(sizeof(*_tmp114E)),((_tmp114E[0]=((_tmp114D.tag=4,((_tmp114D.f1=t,((_tmp114D.f2=_tmp97A,_tmp114D)))))),_tmp114E)))),((_tmp114C->f2=1,_tmp114C)))))));};};}}
# 3747
_npop_handler(0);goto _LL585;
# 3721
;_pop_region(uprev_rgn);}_LL590: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp91E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp912;if(_tmp91E->tag != 13)goto _LL592;else{_tmp91F=_tmp91E->f1;_tmp920=(struct Cyc_Absyn_Enumdecl**)& _tmp91E->f2;}}_LL591:
# 3749
 if(*_tmp920 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp920))->fields == 0){
struct _handler_cons _tmp986;_push_handler(& _tmp986);{int _tmp988=0;if(setjmp(_tmp986.handler))_tmp988=1;if(!_tmp988){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp91F);
*_tmp920=*ed;}
# 3751
;_pop_handler();}else{void*_tmp987=(void*)_exn_thrown;void*_tmp98A=_tmp987;void*_tmp98C;_LL5D3: {struct Cyc_Dict_Absent_exn_struct*_tmp98B=(struct Cyc_Dict_Absent_exn_struct*)_tmp98A;if(_tmp98B->tag != Cyc_Dict_Absent)goto _LL5D5;}_LL5D4: {
# 3755
struct Cyc_Tcenv_Genv*_tmp98D=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp114F;struct Cyc_Absyn_Enumdecl*_tmp98E=(_tmp114F=_cycalloc(sizeof(*_tmp114F)),((_tmp114F->sc=Cyc_Absyn_Extern,((_tmp114F->name=_tmp91F,((_tmp114F->fields=0,_tmp114F)))))));
Cyc_Tc_tcEnumdecl(te,_tmp98D,loc,_tmp98E);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp91F);
*_tmp920=*ed;
goto _LL5D2;};}_LL5D5: _tmp98C=_tmp98A;_LL5D6:(void)_throw(_tmp98C);_LL5D2:;}};}{
# 3764
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp920);
# 3766
*_tmp91F=(ed->name)[0];
goto _LL585;};_LL592: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp921=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp912;if(_tmp921->tag != 3)goto _LL594;else{_tmp922=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp921->f1).datatype_info;_tmp923=(struct Cyc_List_List**)&(_tmp921->f1).targs;}}_LL593: {
# 3769
struct Cyc_List_List*_tmp990=*_tmp923;
{union Cyc_Absyn_DatatypeInfoU _stmttmp8C=*_tmp922;union Cyc_Absyn_DatatypeInfoU _tmp991=_stmttmp8C;struct _tuple2*_tmp992;int _tmp993;struct Cyc_Absyn_Datatypedecl*_tmp994;_LL5D8: if((_tmp991.UnknownDatatype).tag != 1)goto _LL5DA;_tmp992=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp991.UnknownDatatype).val).name;_tmp993=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp991.UnknownDatatype).val).is_extensible;_LL5D9: {
# 3772
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp995;_push_handler(& _tmp995);{int _tmp997=0;if(setjmp(_tmp995.handler))_tmp997=1;if(!_tmp997){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp992);;_pop_handler();}else{void*_tmp996=(void*)_exn_thrown;void*_tmp999=_tmp996;void*_tmp99B;_LL5DD: {struct Cyc_Dict_Absent_exn_struct*_tmp99A=(struct Cyc_Dict_Absent_exn_struct*)_tmp999;if(_tmp99A->tag != Cyc_Dict_Absent)goto _LL5DF;}_LL5DE: {
# 3776
struct Cyc_Tcenv_Genv*_tmp99C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp1150;struct Cyc_Absyn_Datatypedecl*_tmp99D=(_tmp1150=_cycalloc(sizeof(*_tmp1150)),((_tmp1150->sc=Cyc_Absyn_Extern,((_tmp1150->name=_tmp992,((_tmp1150->tvs=0,((_tmp1150->fields=0,((_tmp1150->is_extensible=_tmp993,_tmp1150)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmp99C,loc,_tmp99D);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp992);
# 3781
if(_tmp990 != 0){
{const char*_tmp1154;void*_tmp1153[1];struct Cyc_String_pa_PrintArg_struct _tmp1152;(_tmp1152.tag=0,((_tmp1152.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp992)),((_tmp1153[0]=& _tmp1152,Cyc_Tcutil_terr(loc,((_tmp1154="declare parameterized datatype %s before using",_tag_dyneither(_tmp1154,sizeof(char),47))),_tag_dyneither(_tmp1153,sizeof(void*),1)))))));}
return cvtenv;}
# 3786
goto _LL5DC;}_LL5DF: _tmp99B=_tmp999;_LL5E0:(void)_throw(_tmp99B);_LL5DC:;}};}
# 3792
if(_tmp993  && !(*tudp)->is_extensible){
const char*_tmp1158;void*_tmp1157[1];struct Cyc_String_pa_PrintArg_struct _tmp1156;(_tmp1156.tag=0,((_tmp1156.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp992)),((_tmp1157[0]=& _tmp1156,Cyc_Tcutil_terr(loc,((_tmp1158="datatype %s was not declared @extensible",_tag_dyneither(_tmp1158,sizeof(char),41))),_tag_dyneither(_tmp1157,sizeof(void*),1)))))));}
*_tmp922=Cyc_Absyn_KnownDatatype(tudp);
_tmp994=*tudp;goto _LL5DB;}_LL5DA: if((_tmp991.KnownDatatype).tag != 2)goto _LL5D7;_tmp994=*((struct Cyc_Absyn_Datatypedecl**)(_tmp991.KnownDatatype).val);_LL5DB: {
# 3799
struct Cyc_List_List*tvs=_tmp994->tvs;
for(0;_tmp990 != 0  && tvs != 0;(_tmp990=_tmp990->tl,tvs=tvs->tl)){
void*t=(void*)_tmp990->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3805
{struct _tuple0 _tmp1159;struct _tuple0 _stmttmp8D=(_tmp1159.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1159.f2=t,_tmp1159)));struct _tuple0 _tmp9A5=_stmttmp8D;struct Cyc_Absyn_Tvar*_tmp9A8;_LL5E2:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9A5.f1;if(_tmp9A6->tag != 1)goto _LL5E4;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9A7=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9A5.f2;if(_tmp9A7->tag != 2)goto _LL5E4;else{_tmp9A8=_tmp9A7->f1;}};_LL5E3:
# 3807
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9A8);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9A8,1);
continue;_LL5E4:;_LL5E5:
 goto _LL5E1;_LL5E1:;}{
# 3812
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3816
if(_tmp990 != 0){
const char*_tmp115D;void*_tmp115C[1];struct Cyc_String_pa_PrintArg_struct _tmp115B;(_tmp115B.tag=0,((_tmp115B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp994->name)),((_tmp115C[0]=& _tmp115B,Cyc_Tcutil_terr(loc,((_tmp115D="too many type arguments for datatype %s",_tag_dyneither(_tmp115D,sizeof(char),40))),_tag_dyneither(_tmp115C,sizeof(void*),1)))))));}
if(tvs != 0){
# 3821
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp115E;hidden_ts=((_tmp115E=_cycalloc(sizeof(*_tmp115E)),((_tmp115E->hd=e,((_tmp115E->tl=hidden_ts,_tmp115E))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3828
*_tmp923=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp923,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3830
goto _LL5D7;}_LL5D7:;}
# 3832
goto _LL585;}_LL594: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp924=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp912;if(_tmp924->tag != 4)goto _LL596;else{_tmp925=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp924->f1).field_info;_tmp926=(_tmp924->f1).targs;}}_LL595:
# 3835
{union Cyc_Absyn_DatatypeFieldInfoU _stmttmp89=*_tmp925;union Cyc_Absyn_DatatypeFieldInfoU _tmp9AE=_stmttmp89;struct _tuple2*_tmp9AF;struct _tuple2*_tmp9B0;int _tmp9B1;struct Cyc_Absyn_Datatypedecl*_tmp9B2;struct Cyc_Absyn_Datatypefield*_tmp9B3;_LL5E7: if((_tmp9AE.UnknownDatatypefield).tag != 1)goto _LL5E9;_tmp9AF=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AE.UnknownDatatypefield).val).datatype_name;_tmp9B0=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AE.UnknownDatatypefield).val).field_name;_tmp9B1=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9AE.UnknownDatatypefield).val).is_extensible;_LL5E8: {
# 3837
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp9B4;_push_handler(& _tmp9B4);{int _tmp9B6=0;if(setjmp(_tmp9B4.handler))_tmp9B6=1;if(!_tmp9B6){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9AF);;_pop_handler();}else{void*_tmp9B5=(void*)_exn_thrown;void*_tmp9B8=_tmp9B5;void*_tmp9BA;_LL5EC: {struct Cyc_Dict_Absent_exn_struct*_tmp9B9=(struct Cyc_Dict_Absent_exn_struct*)_tmp9B8;if(_tmp9B9->tag != Cyc_Dict_Absent)goto _LL5EE;}_LL5ED:
# 3841
{const char*_tmp1162;void*_tmp1161[1];struct Cyc_String_pa_PrintArg_struct _tmp1160;(_tmp1160.tag=0,((_tmp1160.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9AF)),((_tmp1161[0]=& _tmp1160,Cyc_Tcutil_terr(loc,((_tmp1162="unbound datatype %s",_tag_dyneither(_tmp1162,sizeof(char),20))),_tag_dyneither(_tmp1161,sizeof(void*),1)))))));}
return cvtenv;_LL5EE: _tmp9BA=_tmp9B8;_LL5EF:(void)_throw(_tmp9BA);_LL5EB:;}};}
# 3844
{struct _handler_cons _tmp9BE;_push_handler(& _tmp9BE);{int _tmp9C0=0;if(setjmp(_tmp9BE.handler))_tmp9C0=1;if(!_tmp9C0){
{struct _RegionHandle _tmp9C1=_new_region("r");struct _RegionHandle*r=& _tmp9C1;_push_region(r);
{void*_stmttmp8B=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp9B0,0);void*_tmp9C2=_stmttmp8B;struct Cyc_Absyn_Datatypedecl*_tmp9C4;struct Cyc_Absyn_Datatypefield*_tmp9C5;_LL5F1: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp9C3=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp9C2;if(_tmp9C3->tag != 2)goto _LL5F3;else{_tmp9C4=_tmp9C3->f1;_tmp9C5=_tmp9C3->f2;}}_LL5F2:
# 3848
 tuf=_tmp9C5;
tud=_tmp9C4;
if(_tmp9B1  && !tud->is_extensible){
const char*_tmp1166;void*_tmp1165[1];struct Cyc_String_pa_PrintArg_struct _tmp1164;(_tmp1164.tag=0,((_tmp1164.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AF)),((_tmp1165[0]=& _tmp1164,Cyc_Tcutil_terr(loc,((_tmp1166="datatype %s was not declared @extensible",_tag_dyneither(_tmp1166,sizeof(char),41))),_tag_dyneither(_tmp1165,sizeof(void*),1)))))));}
goto _LL5F0;_LL5F3:;_LL5F4:
{const char*_tmp116B;void*_tmp116A[2];struct Cyc_String_pa_PrintArg_struct _tmp1169;struct Cyc_String_pa_PrintArg_struct _tmp1168;(_tmp1168.tag=0,((_tmp1168.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AF)),((_tmp1169.tag=0,((_tmp1169.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B0)),((_tmp116A[0]=& _tmp1169,((_tmp116A[1]=& _tmp1168,Cyc_Tcutil_terr(loc,((_tmp116B="unbound field %s in type datatype %s",_tag_dyneither(_tmp116B,sizeof(char),37))),_tag_dyneither(_tmp116A,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmp9CD=cvtenv;_npop_handler(1);return _tmp9CD;};_LL5F0:;}
# 3846
;_pop_region(r);}
# 3845
;_pop_handler();}else{void*_tmp9BF=(void*)_exn_thrown;void*_tmp9CF=_tmp9BF;void*_tmp9D1;_LL5F6: {struct Cyc_Dict_Absent_exn_struct*_tmp9D0=(struct Cyc_Dict_Absent_exn_struct*)_tmp9CF;if(_tmp9D0->tag != Cyc_Dict_Absent)goto _LL5F8;}_LL5F7:
# 3861
{const char*_tmp1170;void*_tmp116F[2];struct Cyc_String_pa_PrintArg_struct _tmp116E;struct Cyc_String_pa_PrintArg_struct _tmp116D;(_tmp116D.tag=0,((_tmp116D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9AF)),((_tmp116E.tag=0,((_tmp116E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B0)),((_tmp116F[0]=& _tmp116E,((_tmp116F[1]=& _tmp116D,Cyc_Tcutil_terr(loc,((_tmp1170="unbound field %s in type datatype %s",_tag_dyneither(_tmp1170,sizeof(char),37))),_tag_dyneither(_tmp116F,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F8: _tmp9D1=_tmp9CF;_LL5F9:(void)_throw(_tmp9D1);_LL5F5:;}};}
# 3865
*_tmp925=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp9B2=tud;_tmp9B3=tuf;goto _LL5EA;}_LL5E9: if((_tmp9AE.KnownDatatypefield).tag != 2)goto _LL5E6;_tmp9B2=((struct _tuple3)(_tmp9AE.KnownDatatypefield).val).f1;_tmp9B3=((struct _tuple3)(_tmp9AE.KnownDatatypefield).val).f2;_LL5EA: {
# 3869
struct Cyc_List_List*tvs=_tmp9B2->tvs;
for(0;_tmp926 != 0  && tvs != 0;(_tmp926=_tmp926->tl,tvs=tvs->tl)){
void*t=(void*)_tmp926->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3875
{struct _tuple0 _tmp1171;struct _tuple0 _stmttmp8A=(_tmp1171.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1171.f2=t,_tmp1171)));struct _tuple0 _tmp9D6=_stmttmp8A;struct Cyc_Absyn_Tvar*_tmp9D9;_LL5FB:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9D7=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9D6.f1;if(_tmp9D7->tag != 1)goto _LL5FD;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9D8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9D6.f2;if(_tmp9D8->tag != 2)goto _LL5FD;else{_tmp9D9=_tmp9D8->f1;}};_LL5FC:
# 3877
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9D9);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9D9,1);
continue;_LL5FD:;_LL5FE:
 goto _LL5FA;_LL5FA:;}{
# 3882
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3886
if(_tmp926 != 0){
const char*_tmp1176;void*_tmp1175[2];struct Cyc_String_pa_PrintArg_struct _tmp1174;struct Cyc_String_pa_PrintArg_struct _tmp1173;(_tmp1173.tag=0,((_tmp1173.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B3->name)),((_tmp1174.tag=0,((_tmp1174.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B2->name)),((_tmp1175[0]=& _tmp1174,((_tmp1175[1]=& _tmp1173,Cyc_Tcutil_terr(loc,((_tmp1176="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1176,sizeof(char),43))),_tag_dyneither(_tmp1175,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp117B;void*_tmp117A[2];struct Cyc_String_pa_PrintArg_struct _tmp1179;struct Cyc_String_pa_PrintArg_struct _tmp1178;(_tmp1178.tag=0,((_tmp1178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp9B3->name)),((_tmp1179.tag=0,((_tmp1179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B2->name)),((_tmp117A[0]=& _tmp1179,((_tmp117A[1]=& _tmp1178,Cyc_Tcutil_terr(loc,((_tmp117B="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp117B,sizeof(char),42))),_tag_dyneither(_tmp117A,sizeof(void*),2)))))))))))));}
goto _LL5E6;}_LL5E6:;}
# 3894
goto _LL585;_LL596: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp927=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp912;if(_tmp927->tag != 5)goto _LL598;else{_tmp928=(_tmp927->f1).elt_typ;_tmp929=(struct Cyc_Absyn_Tqual*)&(_tmp927->f1).elt_tq;_tmp92A=((_tmp927->f1).ptr_atts).rgn;_tmp92B=((_tmp927->f1).ptr_atts).nullable;_tmp92C=((_tmp927->f1).ptr_atts).bounds;_tmp92D=((_tmp927->f1).ptr_atts).zero_term;}}_LL597: {
# 3897
int is_zero_terminated;
# 3899
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp928,1);
_tmp929->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp929->print_const,_tmp928);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _stmttmp84=expected_kind->aliasqual;switch(_stmttmp84){case Cyc_Absyn_Aliasable: _LL5FF:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL600:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL601:
 k=& Cyc_Tcutil_trk;break;}}
# 3907
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp92A,1);
{union Cyc_Absyn_Constraint*_stmttmp85=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp92D);union Cyc_Absyn_Constraint*_tmp9E3=_stmttmp85;_LL604: if((_tmp9E3->No_constr).tag != 3)goto _LL606;_LL605:
# 3912
{void*_stmttmp86=Cyc_Tcutil_compress(_tmp928);void*_tmp9E4=_stmttmp86;_LL60B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9E5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9E4;if(_tmp9E5->tag != 6)goto _LL60D;else{if(_tmp9E5->f2 != Cyc_Absyn_Char_sz)goto _LL60D;}}_LL60C:
# 3914
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp92D,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL60A;_LL60D:;_LL60E:
# 3918
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp92D,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL60A;_LL60A:;}
# 3922
goto _LL603;_LL606: if((_tmp9E3->Eq_constr).tag != 1)goto _LL608;if((int)(_tmp9E3->Eq_constr).val != 1)goto _LL608;_LL607:
# 3925
 if(!Cyc_Tcutil_admits_zero(_tmp928)){
const char*_tmp117F;void*_tmp117E[1];struct Cyc_String_pa_PrintArg_struct _tmp117D;(_tmp117D.tag=0,((_tmp117D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp928)),((_tmp117E[0]=& _tmp117D,Cyc_Tcutil_terr(loc,((_tmp117F="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp117F,sizeof(char),53))),_tag_dyneither(_tmp117E,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL603;_LL608:;_LL609:
# 3931
 is_zero_terminated=0;
goto _LL603;_LL603:;}
# 3935
{void*_stmttmp87=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp92C);void*_tmp9E9=_stmttmp87;struct Cyc_Absyn_Exp*_tmp9EC;_LL610: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp9EA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9E9;if(_tmp9EA->tag != 0)goto _LL612;}_LL611:
 goto _LL60F;_LL612: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp9EB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9E9;if(_tmp9EB->tag != 1)goto _LL60F;else{_tmp9EC=_tmp9EB->f1;}}_LL613: {
# 3938
struct _RegionHandle _tmp9ED=_new_region("temp");struct _RegionHandle*temp=& _tmp9ED;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9EE=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9EE,0,_tmp9EC);}
# 3942
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9EC,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9EC)){
const char*_tmp1182;void*_tmp1181;(_tmp1181=0,Cyc_Tcutil_terr(loc,((_tmp1182="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp1182,sizeof(char),49))),_tag_dyneither(_tmp1181,sizeof(void*),0)));}{
struct _tuple14 _stmttmp88=Cyc_Evexp_eval_const_uint_exp(_tmp9EC);unsigned int _tmp9F2;int _tmp9F3;struct _tuple14 _tmp9F1=_stmttmp88;_tmp9F2=_tmp9F1.f1;_tmp9F3=_tmp9F1.f2;
if(is_zero_terminated  && (!_tmp9F3  || _tmp9F2 < 1)){
const char*_tmp1185;void*_tmp1184;(_tmp1184=0,Cyc_Tcutil_terr(loc,((_tmp1185="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp1185,sizeof(char),55))),_tag_dyneither(_tmp1184,sizeof(void*),0)));}
_npop_handler(0);goto _LL60F;};
# 3938
;_pop_region(temp);}_LL60F:;}
# 3950
goto _LL585;};}_LL598: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp92E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp912;if(_tmp92E->tag != 19)goto _LL59A;else{_tmp92F=(void*)_tmp92E->f1;}}_LL599:
# 3952
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp92F,1);goto _LL585;_LL59A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp930=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp912;if(_tmp930->tag != 18)goto _LL59C;else{_tmp931=_tmp930->f1;}}_LL59B: {
# 3957
struct _RegionHandle _tmp9F6=_new_region("temp");struct _RegionHandle*temp=& _tmp9F6;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp9F7=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp9F7,0,_tmp931);}
# 3961
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp931)){
const char*_tmp1188;void*_tmp1187;(_tmp1187=0,Cyc_Tcutil_terr(loc,((_tmp1188="valueof_t requires an int expression",_tag_dyneither(_tmp1188,sizeof(char),37))),_tag_dyneither(_tmp1187,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp931,te,cvtenv);
_npop_handler(0);goto _LL585;
# 3957
;_pop_region(temp);}_LL59C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp932=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp912;if(_tmp932->tag != 6)goto _LL59E;}_LL59D:
# 3965
 goto _LL59F;_LL59E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp933=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp912;if(_tmp933->tag != 7)goto _LL5A0;}_LL59F:
 goto _LL585;_LL5A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp934=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp912;if(_tmp934->tag != 8)goto _LL5A2;else{_tmp935=(_tmp934->f1).elt_type;_tmp936=(struct Cyc_Absyn_Tqual*)&(_tmp934->f1).tq;_tmp937=(struct Cyc_Absyn_Exp**)&(_tmp934->f1).num_elts;_tmp938=(_tmp934->f1).zero_term;_tmp939=(_tmp934->f1).zt_loc;}}_LL5A1: {
# 3970
struct Cyc_Absyn_Exp*_tmp9FA=*_tmp937;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp935,1);
_tmp936->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp936->print_const,_tmp935);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_stmttmp82=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp938);union Cyc_Absyn_Constraint*_tmp9FB=_stmttmp82;_LL615: if((_tmp9FB->No_constr).tag != 3)goto _LL617;_LL616:
# 3977
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp938,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 3992 "tcutil.cyc"
goto _LL614;_LL617: if((_tmp9FB->Eq_constr).tag != 1)goto _LL619;if((int)(_tmp9FB->Eq_constr).val != 1)goto _LL619;_LL618:
# 3995
 if(!Cyc_Tcutil_admits_zero(_tmp935)){
const char*_tmp118C;void*_tmp118B[1];struct Cyc_String_pa_PrintArg_struct _tmp118A;(_tmp118A.tag=0,((_tmp118A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp935)),((_tmp118B[0]=& _tmp118A,Cyc_Tcutil_terr(loc,((_tmp118C="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp118C,sizeof(char),51))),_tag_dyneither(_tmp118B,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL614;_LL619:;_LL61A:
# 4001
 is_zero_terminated=0;
goto _LL614;_LL614:;}
# 4006
if(_tmp9FA == 0){
# 4008
if(is_zero_terminated)
# 4010
*_tmp937=(_tmp9FA=Cyc_Absyn_uint_exp(1,0));else{
# 4013
{const char*_tmp118F;void*_tmp118E;(_tmp118E=0,Cyc_Tcutil_warn(loc,((_tmp118F="array bound defaults to 1 here",_tag_dyneither(_tmp118F,sizeof(char),31))),_tag_dyneither(_tmp118E,sizeof(void*),0)));}
*_tmp937=(_tmp9FA=Cyc_Absyn_uint_exp(1,0));}}{
# 4017
struct _RegionHandle _tmpA01=_new_region("temp");struct _RegionHandle*temp=& _tmpA01;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA02=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA02,0,_tmp9FA);}
# 4021
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9FA)){
const char*_tmp1192;void*_tmp1191;(_tmp1191=0,Cyc_Tcutil_terr(loc,((_tmp1192="array bounds expression is not an unsigned int",_tag_dyneither(_tmp1192,sizeof(char),47))),_tag_dyneither(_tmp1191,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9FA,te,cvtenv);{
# 4028
struct _tuple14 _stmttmp83=Cyc_Evexp_eval_const_uint_exp(_tmp9FA);unsigned int _tmpA06;int _tmpA07;struct _tuple14 _tmpA05=_stmttmp83;_tmpA06=_tmpA05.f1;_tmpA07=_tmpA05.f2;
# 4030
if((is_zero_terminated  && _tmpA07) && _tmpA06 < 1){
const char*_tmp1195;void*_tmp1194;(_tmp1194=0,Cyc_Tcutil_warn(loc,((_tmp1195="zero terminated array cannot have zero elements",_tag_dyneither(_tmp1195,sizeof(char),48))),_tag_dyneither(_tmp1194,sizeof(void*),0)));}
# 4033
if((_tmpA07  && _tmpA06 < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp1198;void*_tmp1197;(_tmp1197=0,Cyc_Tcutil_warn(loc,((_tmp1198="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp1198,sizeof(char),75))),_tag_dyneither(_tmp1197,sizeof(void*),0)));}
*_tmp937=Cyc_Absyn_uint_exp(1,0);}
# 4037
_npop_handler(0);goto _LL585;};
# 4017
;_pop_region(temp);};};}_LL5A2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp93A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp912;if(_tmp93A->tag != 9)goto _LL5A4;else{_tmp93B=(struct Cyc_List_List**)&(_tmp93A->f1).tvars;_tmp93C=(void**)&(_tmp93A->f1).effect;_tmp93D=(struct Cyc_Absyn_Tqual*)&(_tmp93A->f1).ret_tqual;_tmp93E=(_tmp93A->f1).ret_typ;_tmp93F=(_tmp93A->f1).args;_tmp940=(_tmp93A->f1).c_varargs;_tmp941=(_tmp93A->f1).cyc_varargs;_tmp942=(_tmp93A->f1).rgn_po;_tmp943=(_tmp93A->f1).attributes;}}_LL5A3: {
# 4044
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp943 != 0;_tmp943=_tmp943->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp943->hd)){
const char*_tmp119C;void*_tmp119B[1];struct Cyc_String_pa_PrintArg_struct _tmp119A;(_tmp119A.tag=0,((_tmp119A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp943->hd)),((_tmp119B[0]=& _tmp119A,Cyc_Tcutil_terr(loc,((_tmp119C="bad function type attribute %s",_tag_dyneither(_tmp119C,sizeof(char),31))),_tag_dyneither(_tmp119B,sizeof(void*),1)))))));}{
void*_stmttmp70=(void*)_tmp943->hd;void*_tmpA0F=_stmttmp70;enum Cyc_Absyn_Format_Type _tmpA14;int _tmpA15;int _tmpA16;_LL61C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpA10=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA0F;if(_tmpA10->tag != 1)goto _LL61E;}_LL61D:
# 4057
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL61B;_LL61E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA11=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA0F;if(_tmpA11->tag != 2)goto _LL620;}_LL61F:
# 4059
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL61B;_LL620: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA12=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA0F;if(_tmpA12->tag != 3)goto _LL622;}_LL621:
# 4061
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL61B;_LL622: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA13=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA0F;if(_tmpA13->tag != 19)goto _LL624;else{_tmpA14=_tmpA13->f1;_tmpA15=_tmpA13->f2;_tmpA16=_tmpA13->f3;}}_LL623:
# 4063
 if(!seen_format){
seen_format=1;
ft=_tmpA14;
fmt_desc_arg=_tmpA15;
fmt_arg_start=_tmpA16;}else{
# 4069
const char*_tmp119F;void*_tmp119E;(_tmp119E=0,Cyc_Tcutil_terr(loc,((_tmp119F="function can't have multiple format attributes",_tag_dyneither(_tmp119F,sizeof(char),47))),_tag_dyneither(_tmp119E,sizeof(void*),0)));}
goto _LL61B;_LL624:;_LL625:
 goto _LL61B;_LL61B:;};}
# 4074
if(num_convs > 1){
const char*_tmp11A2;void*_tmp11A1;(_tmp11A1=0,Cyc_Tcutil_terr(loc,((_tmp11A2="function can't have multiple calling conventions",_tag_dyneither(_tmp11A2,sizeof(char),49))),_tag_dyneither(_tmp11A1,sizeof(void*),0)));}
# 4079
Cyc_Tcutil_check_unique_tvars(loc,*_tmp93B);
{struct Cyc_List_List*b=*_tmp93B;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_stmttmp71=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmpA1B=_stmttmp71;_LL627: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA1C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA1B;if(_tmpA1C->tag != 0)goto _LL629;else{if((_tmpA1C->f1)->kind != Cyc_Absyn_MemKind)goto _LL629;}}_LL628:
# 4085
{const char*_tmp11A6;void*_tmp11A5[1];struct Cyc_String_pa_PrintArg_struct _tmp11A4;(_tmp11A4.tag=0,((_tmp11A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp11A5[0]=& _tmp11A4,Cyc_Tcutil_terr(loc,((_tmp11A6="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp11A6,sizeof(char),51))),_tag_dyneither(_tmp11A5,sizeof(void*),1)))))));}
goto _LL626;_LL629:;_LL62A:
 goto _LL626;_LL626:;};}}{
# 4093
struct _RegionHandle _tmpA20=_new_region("newr");struct _RegionHandle*newr=& _tmpA20;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp11A7;struct Cyc_Tcutil_CVTEnv _tmpA21=
(_tmp11A7.r=newr,((_tmp11A7.kind_env=cvtenv.kind_env,((_tmp11A7.free_vars=0,((_tmp11A7.free_evars=0,((_tmp11A7.generalize_evars=cvtenv.generalize_evars,((_tmp11A7.fn_result=1,_tmp11A7)))))))))));
# 4099
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_tmk,_tmp93E,1);
_tmp93D->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp93D->print_const,_tmp93E);
_tmpA21.fn_result=0;
{struct Cyc_List_List*a=_tmp93F;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpA22=(struct _tuple10*)a->hd;
void*_tmpA23=(*_tmpA22).f3;
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_tmk,_tmpA23,1);{
int _tmpA24=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpA22).f2).print_const,_tmpA23);
((*_tmpA22).f2).real_const=_tmpA24;
# 4110
if(Cyc_Tcutil_is_array(_tmpA23)){
# 4113
void*_tmpA25=Cyc_Absyn_new_evar(0,0);
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_rk,_tmpA25,1);
(*_tmpA22).f3=Cyc_Tcutil_promote_array(_tmpA23,_tmpA25);}};}}
# 4120
if(_tmp941 != 0){
if(_tmp940){const char*_tmp11AA;void*_tmp11A9;(_tmp11A9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11AA="both c_vararg and cyc_vararg",_tag_dyneither(_tmp11AA,sizeof(char),29))),_tag_dyneither(_tmp11A9,sizeof(void*),0)));}{
struct Cyc_Absyn_VarargInfo _stmttmp72=*_tmp941;void*_tmpA29;int _tmpA2A;struct Cyc_Absyn_VarargInfo _tmpA28=_stmttmp72;_tmpA29=_tmpA28.type;_tmpA2A=_tmpA28.inject;
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_tmk,_tmpA29,1);
(_tmp941->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp941->tq).print_const,_tmpA29);
# 4126
if(_tmpA2A){
void*_stmttmp73=Cyc_Tcutil_compress(_tmpA29);void*_tmpA2B=_stmttmp73;void*_tmpA2D;union Cyc_Absyn_Constraint*_tmpA2E;union Cyc_Absyn_Constraint*_tmpA2F;_LL62C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA2C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA2B;if(_tmpA2C->tag != 5)goto _LL62E;else{_tmpA2D=(_tmpA2C->f1).elt_typ;_tmpA2E=((_tmpA2C->f1).ptr_atts).bounds;_tmpA2F=((_tmpA2C->f1).ptr_atts).zero_term;}}_LL62D:
# 4129
{void*_stmttmp74=Cyc_Tcutil_compress(_tmpA2D);void*_tmpA30=_stmttmp74;_LL631: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA31=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA30;if(_tmpA31->tag != 3)goto _LL633;}_LL632:
# 4131
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpA2F)){
const char*_tmp11AD;void*_tmp11AC;(_tmp11AC=0,Cyc_Tcutil_terr(loc,((_tmp11AD="can't inject into a zeroterm pointer",_tag_dyneither(_tmp11AD,sizeof(char),37))),_tag_dyneither(_tmp11AC,sizeof(void*),0)));}
{void*_stmttmp75=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpA2E);void*_tmpA34=_stmttmp75;_LL636: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA35=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA34;if(_tmpA35->tag != 0)goto _LL638;}_LL637:
# 4135
{const char*_tmp11B0;void*_tmp11AF;(_tmp11AF=0,Cyc_Tcutil_terr(loc,((_tmp11B0="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp11B0,sizeof(char),44))),_tag_dyneither(_tmp11AF,sizeof(void*),0)));}
goto _LL635;_LL638:;_LL639:
 goto _LL635;_LL635:;}
# 4139
goto _LL630;_LL633:;_LL634:
{const char*_tmp11B3;void*_tmp11B2;(_tmp11B2=0,Cyc_Tcutil_terr(loc,((_tmp11B3="can't inject a non-datatype type",_tag_dyneither(_tmp11B3,sizeof(char),33))),_tag_dyneither(_tmp11B2,sizeof(void*),0)));}goto _LL630;_LL630:;}
# 4142
goto _LL62B;_LL62E:;_LL62F:
{const char*_tmp11B6;void*_tmp11B5;(_tmp11B5=0,Cyc_Tcutil_terr(loc,((_tmp11B6="expecting a datatype pointer type",_tag_dyneither(_tmp11B6,sizeof(char),34))),_tag_dyneither(_tmp11B5,sizeof(void*),0)));}goto _LL62B;_LL62B:;}};}
# 4148
if(seen_format){
int _tmpA3C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp93F);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpA3C) || fmt_arg_start < 0) || 
# 4152
_tmp941 == 0  && fmt_arg_start != 0) || 
_tmp941 != 0  && fmt_arg_start != _tmpA3C + 1){
const char*_tmp11B9;void*_tmp11B8;(_tmp11B8=0,Cyc_Tcutil_terr(loc,((_tmp11B9="bad format descriptor",_tag_dyneither(_tmp11B9,sizeof(char),22))),_tag_dyneither(_tmp11B8,sizeof(void*),0)));}else{
# 4157
struct _tuple10 _stmttmp76=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp93F,fmt_desc_arg - 1);void*_tmpA40;struct _tuple10 _tmpA3F=_stmttmp76;_tmpA40=_tmpA3F.f3;
# 4159
{void*_stmttmp77=Cyc_Tcutil_compress(_tmpA40);void*_tmpA41=_stmttmp77;void*_tmpA43;union Cyc_Absyn_Constraint*_tmpA44;union Cyc_Absyn_Constraint*_tmpA45;_LL63B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA42=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA41;if(_tmpA42->tag != 5)goto _LL63D;else{_tmpA43=(_tmpA42->f1).elt_typ;_tmpA44=((_tmpA42->f1).ptr_atts).bounds;_tmpA45=((_tmpA42->f1).ptr_atts).zero_term;}}_LL63C:
# 4162
{struct _tuple0 _tmp11BA;struct _tuple0 _stmttmp78=(_tmp11BA.f1=Cyc_Tcutil_compress(_tmpA43),((_tmp11BA.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA44),_tmp11BA)));struct _tuple0 _tmpA46=_stmttmp78;_LL640:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA47=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA46.f1;if(_tmpA47->tag != 6)goto _LL642;else{if(_tmpA47->f1 != Cyc_Absyn_None)goto _LL642;if(_tmpA47->f2 != Cyc_Absyn_Char_sz)goto _LL642;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA48=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA46.f2;if(_tmpA48->tag != 0)goto _LL642;};_LL641:
 goto _LL63F;_LL642:;_LL643:
{const char*_tmp11BD;void*_tmp11BC;(_tmp11BC=0,Cyc_Tcutil_terr(loc,((_tmp11BD="format descriptor is not a char ? type",_tag_dyneither(_tmp11BD,sizeof(char),39))),_tag_dyneither(_tmp11BC,sizeof(void*),0)));}goto _LL63F;_LL63F:;}
# 4166
goto _LL63A;_LL63D:;_LL63E:
{const char*_tmp11C0;void*_tmp11BF;(_tmp11BF=0,Cyc_Tcutil_terr(loc,((_tmp11C0="format descriptor is not a char ? type",_tag_dyneither(_tmp11C0,sizeof(char),39))),_tag_dyneither(_tmp11BF,sizeof(void*),0)));}goto _LL63A;_LL63A:;}
# 4169
if(fmt_arg_start != 0){
# 4173
int problem;
{struct _tuple30 _tmp11C1;struct _tuple30 _stmttmp79=(_tmp11C1.f1=ft,((_tmp11C1.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp941))->type)),_tmp11C1)));struct _tuple30 _tmpA4E=_stmttmp79;struct Cyc_Absyn_Datatypedecl*_tmpA50;struct Cyc_Absyn_Datatypedecl*_tmpA52;_LL645: if(_tmpA4E.f1 != Cyc_Absyn_Printf_ft)goto _LL647;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA4F=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA4E.f2;if(_tmpA4F->tag != 3)goto _LL647;else{if((((_tmpA4F->f1).datatype_info).KnownDatatype).tag != 2)goto _LL647;_tmpA50=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA4F->f1).datatype_info).KnownDatatype).val);}};_LL646:
# 4176
 problem=Cyc_Absyn_qvar_cmp(_tmpA50->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL644;_LL647: if(_tmpA4E.f1 != Cyc_Absyn_Scanf_ft)goto _LL649;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpA51=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA4E.f2;if(_tmpA51->tag != 3)goto _LL649;else{if((((_tmpA51->f1).datatype_info).KnownDatatype).tag != 2)goto _LL649;_tmpA52=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpA51->f1).datatype_info).KnownDatatype).val);}};_LL648:
# 4178
 problem=Cyc_Absyn_qvar_cmp(_tmpA52->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL644;_LL649:;_LL64A:
# 4180
 problem=1;goto _LL644;_LL644:;}
# 4182
if(problem){
const char*_tmp11C4;void*_tmp11C3;(_tmp11C3=0,Cyc_Tcutil_terr(loc,((_tmp11C4="format attribute and vararg types don't match",_tag_dyneither(_tmp11C4,sizeof(char),46))),_tag_dyneither(_tmp11C3,sizeof(void*),0)));}}}}
# 4190
{struct Cyc_List_List*rpo=_tmp942;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_stmttmp7A=(struct _tuple0*)rpo->hd;void*_tmpA57;void*_tmpA58;struct _tuple0*_tmpA56=_stmttmp7A;_tmpA57=_tmpA56->f1;_tmpA58=_tmpA56->f2;
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_ek,_tmpA57,1);
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_trk,_tmpA58,1);}}
# 4196
if(*_tmp93C != 0)
_tmpA21=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA21,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp93C),1);else{
# 4199
struct Cyc_List_List*effect=0;
# 4204
{struct Cyc_List_List*tvs=_tmpA21.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _stmttmp7B=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpA5A;int _tmpA5B;struct _tuple28 _tmpA59=_stmttmp7B;_tmpA5A=_tmpA59.f1;_tmpA5B=_tmpA59.f2;
if(!_tmpA5B)continue;{
void*_stmttmp7C=Cyc_Absyn_compress_kb(_tmpA5A->kind);void*_tmpA5C=_stmttmp7C;struct Cyc_Core_Opt**_tmpA5E;struct Cyc_Absyn_Kind*_tmpA5F;struct Cyc_Absyn_Kind*_tmpA61;struct Cyc_Core_Opt**_tmpA65;struct Cyc_Core_Opt**_tmpA68;_LL64C:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA5D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA5D->tag != 2)goto _LL64E;else{_tmpA5E=(struct Cyc_Core_Opt**)& _tmpA5D->f1;_tmpA5F=_tmpA5D->f2;}}if(!(_tmpA5F->kind == Cyc_Absyn_RgnKind))goto _LL64E;_LL64D:
# 4210
 if(_tmpA5F->aliasqual == Cyc_Absyn_Top){
*_tmpA5E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpA61=_tmpA5F;goto _LL64F;}
# 4213
*_tmpA5E=Cyc_Tcutil_kind_to_bound_opt(_tmpA5F);_tmpA61=_tmpA5F;goto _LL64F;_LL64E:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA60=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA60->tag != 0)goto _LL650;else{_tmpA61=_tmpA60->f1;}}if(!(_tmpA61->kind == Cyc_Absyn_RgnKind))goto _LL650;_LL64F:
# 4215
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11D3;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11D2;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11D1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11D0;struct Cyc_List_List*_tmp11CF;effect=((_tmp11CF=_cycalloc(sizeof(*_tmp11CF)),((_tmp11CF->hd=(void*)((_tmp11D3=_cycalloc(sizeof(*_tmp11D3)),((_tmp11D3[0]=((_tmp11D0.tag=23,((_tmp11D0.f1=(void*)((_tmp11D2=_cycalloc(sizeof(*_tmp11D2)),((_tmp11D2[0]=((_tmp11D1.tag=2,((_tmp11D1.f1=_tmpA5A,_tmp11D1)))),_tmp11D2)))),_tmp11D0)))),_tmp11D3)))),((_tmp11CF->tl=effect,_tmp11CF))))));}goto _LL64B;_LL650: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA62=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA62->tag != 2)goto _LL652;else{if((_tmpA62->f2)->kind != Cyc_Absyn_IntKind)goto _LL652;}}_LL651:
 goto _LL653;_LL652: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA63=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA63->tag != 0)goto _LL654;else{if((_tmpA63->f1)->kind != Cyc_Absyn_IntKind)goto _LL654;}}_LL653:
 goto _LL64B;_LL654: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA64=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA64->tag != 2)goto _LL656;else{_tmpA65=(struct Cyc_Core_Opt**)& _tmpA64->f1;if((_tmpA64->f2)->kind != Cyc_Absyn_EffKind)goto _LL656;}}_LL655:
# 4219
*_tmpA65=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL657;_LL656: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA66=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA66->tag != 0)goto _LL658;else{if((_tmpA66->f1)->kind != Cyc_Absyn_EffKind)goto _LL658;}}_LL657:
# 4221
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11D9;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11D8;struct Cyc_List_List*_tmp11D7;effect=((_tmp11D7=_cycalloc(sizeof(*_tmp11D7)),((_tmp11D7->hd=(void*)((_tmp11D9=_cycalloc(sizeof(*_tmp11D9)),((_tmp11D9[0]=((_tmp11D8.tag=2,((_tmp11D8.f1=_tmpA5A,_tmp11D8)))),_tmp11D9)))),((_tmp11D7->tl=effect,_tmp11D7))))));}goto _LL64B;_LL658: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA67=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA5C;if(_tmpA67->tag != 1)goto _LL65A;else{_tmpA68=(struct Cyc_Core_Opt**)& _tmpA67->f1;}}_LL659:
# 4223
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11DF;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp11DE;struct Cyc_Core_Opt*_tmp11DD;*_tmpA68=((_tmp11DD=_cycalloc(sizeof(*_tmp11DD)),((_tmp11DD->v=(void*)((_tmp11DF=_cycalloc(sizeof(*_tmp11DF)),((_tmp11DF[0]=((_tmp11DE.tag=2,((_tmp11DE.f1=0,((_tmp11DE.f2=& Cyc_Tcutil_ak,_tmp11DE)))))),_tmp11DF)))),_tmp11DD))));}goto _LL65B;_LL65A:;_LL65B:
# 4226
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11EE;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11ED;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11EC;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11EB;struct Cyc_List_List*_tmp11EA;effect=((_tmp11EA=_cycalloc(sizeof(*_tmp11EA)),((_tmp11EA->hd=(void*)((_tmp11EE=_cycalloc(sizeof(*_tmp11EE)),((_tmp11EE[0]=((_tmp11EB.tag=25,((_tmp11EB.f1=(void*)((_tmp11ED=_cycalloc(sizeof(*_tmp11ED)),((_tmp11ED[0]=((_tmp11EC.tag=2,((_tmp11EC.f1=_tmpA5A,_tmp11EC)))),_tmp11ED)))),_tmp11EB)))),_tmp11EE)))),((_tmp11EA->tl=effect,_tmp11EA))))));}goto _LL64B;_LL64B:;};}}
# 4230
{struct Cyc_List_List*ts=_tmpA21.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _stmttmp7D=*((struct _tuple29*)ts->hd);void*_tmpA7A;int _tmpA7B;struct _tuple29 _tmpA79=_stmttmp7D;_tmpA7A=_tmpA79.f1;_tmpA7B=_tmpA79.f2;
if(!_tmpA7B)continue;{
struct Cyc_Absyn_Kind*_stmttmp7E=Cyc_Tcutil_typ_kind(_tmpA7A);struct Cyc_Absyn_Kind*_tmpA7C=_stmttmp7E;_LL65D: if(_tmpA7C->kind != Cyc_Absyn_RgnKind)goto _LL65F;_LL65E:
# 4235
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11F4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11F3;struct Cyc_List_List*_tmp11F2;effect=((_tmp11F2=_cycalloc(sizeof(*_tmp11F2)),((_tmp11F2->hd=(void*)((_tmp11F4=_cycalloc(sizeof(*_tmp11F4)),((_tmp11F4[0]=((_tmp11F3.tag=23,((_tmp11F3.f1=_tmpA7A,_tmp11F3)))),_tmp11F4)))),((_tmp11F2->tl=effect,_tmp11F2))))));}goto _LL65C;_LL65F: if(_tmpA7C->kind != Cyc_Absyn_EffKind)goto _LL661;_LL660:
# 4237
{struct Cyc_List_List*_tmp11F5;effect=((_tmp11F5=_cycalloc(sizeof(*_tmp11F5)),((_tmp11F5->hd=_tmpA7A,((_tmp11F5->tl=effect,_tmp11F5))))));}goto _LL65C;_LL661: if(_tmpA7C->kind != Cyc_Absyn_IntKind)goto _LL663;_LL662:
 goto _LL65C;_LL663:;_LL664:
# 4240
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11FB;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11FA;struct Cyc_List_List*_tmp11F9;effect=((_tmp11F9=_cycalloc(sizeof(*_tmp11F9)),((_tmp11F9->hd=(void*)((_tmp11FB=_cycalloc(sizeof(*_tmp11FB)),((_tmp11FB[0]=((_tmp11FA.tag=25,((_tmp11FA.f1=_tmpA7A,_tmp11FA)))),_tmp11FB)))),((_tmp11F9->tl=effect,_tmp11F9))))));}goto _LL65C;_LL65C:;};}}{
# 4243
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11FE;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11FD;*_tmp93C=(void*)((_tmp11FD=_cycalloc(sizeof(*_tmp11FD)),((_tmp11FD[0]=((_tmp11FE.tag=24,((_tmp11FE.f1=effect,_tmp11FE)))),_tmp11FD))));};}
# 4249
if(*_tmp93B != 0){
struct Cyc_List_List*bs=*_tmp93B;for(0;bs != 0;bs=bs->tl){
void*_stmttmp7F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmpA86=_stmttmp7F;struct Cyc_Core_Opt**_tmpA88;struct Cyc_Core_Opt**_tmpA8A;struct Cyc_Core_Opt**_tmpA8C;struct Cyc_Core_Opt**_tmpA8E;struct Cyc_Core_Opt**_tmpA90;struct Cyc_Core_Opt**_tmpA92;struct Cyc_Core_Opt**_tmpA94;struct Cyc_Core_Opt**_tmpA96;struct Cyc_Core_Opt**_tmpA98;struct Cyc_Core_Opt**_tmpA9A;struct Cyc_Absyn_Kind*_tmpA9B;_LL666: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA87=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA87->tag != 1)goto _LL668;else{_tmpA88=(struct Cyc_Core_Opt**)& _tmpA87->f1;}}_LL667:
# 4253
{const char*_tmp1202;void*_tmp1201[1];struct Cyc_String_pa_PrintArg_struct _tmp1200;(_tmp1200.tag=0,((_tmp1200.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1201[0]=& _tmp1200,Cyc_Tcutil_warn(loc,((_tmp1202="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1202,sizeof(char),47))),_tag_dyneither(_tmp1201,sizeof(void*),1)))))));}
# 4255
_tmpA8A=_tmpA88;goto _LL669;_LL668: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA89=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA89->tag != 2)goto _LL66A;else{_tmpA8A=(struct Cyc_Core_Opt**)& _tmpA89->f1;if((_tmpA89->f2)->kind != Cyc_Absyn_BoxKind)goto _LL66A;if((_tmpA89->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66A;}}_LL669:
 _tmpA8C=_tmpA8A;goto _LL66B;_LL66A: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA8B->tag != 2)goto _LL66C;else{_tmpA8C=(struct Cyc_Core_Opt**)& _tmpA8B->f1;if((_tmpA8B->f2)->kind != Cyc_Absyn_MemKind)goto _LL66C;if((_tmpA8B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66C;}}_LL66B:
 _tmpA8E=_tmpA8C;goto _LL66D;_LL66C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA8D->tag != 2)goto _LL66E;else{_tmpA8E=(struct Cyc_Core_Opt**)& _tmpA8D->f1;if((_tmpA8D->f2)->kind != Cyc_Absyn_MemKind)goto _LL66E;if((_tmpA8D->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL66E;}}_LL66D:
 _tmpA90=_tmpA8E;goto _LL66F;_LL66E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA8F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA8F->tag != 2)goto _LL670;else{_tmpA90=(struct Cyc_Core_Opt**)& _tmpA8F->f1;if((_tmpA8F->f2)->kind != Cyc_Absyn_AnyKind)goto _LL670;if((_tmpA8F->f2)->aliasqual != Cyc_Absyn_Top)goto _LL670;}}_LL66F:
 _tmpA92=_tmpA90;goto _LL671;_LL670: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA91=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA91->tag != 2)goto _LL672;else{_tmpA92=(struct Cyc_Core_Opt**)& _tmpA91->f1;if((_tmpA91->f2)->kind != Cyc_Absyn_AnyKind)goto _LL672;if((_tmpA91->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL672;}}_LL671:
# 4261
*_tmpA92=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL665;_LL672: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA93=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA93->tag != 2)goto _LL674;else{_tmpA94=(struct Cyc_Core_Opt**)& _tmpA93->f1;if((_tmpA93->f2)->kind != Cyc_Absyn_MemKind)goto _LL674;if((_tmpA93->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL674;}}_LL673:
 _tmpA96=_tmpA94;goto _LL675;_LL674: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA95=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA95->tag != 2)goto _LL676;else{_tmpA96=(struct Cyc_Core_Opt**)& _tmpA95->f1;if((_tmpA95->f2)->kind != Cyc_Absyn_AnyKind)goto _LL676;if((_tmpA95->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL676;}}_LL675:
# 4264
*_tmpA96=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL665;_LL676: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA97=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA97->tag != 2)goto _LL678;else{_tmpA98=(struct Cyc_Core_Opt**)& _tmpA97->f1;if((_tmpA97->f2)->kind != Cyc_Absyn_RgnKind)goto _LL678;if((_tmpA97->f2)->aliasqual != Cyc_Absyn_Top)goto _LL678;}}_LL677:
# 4266
*_tmpA98=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL665;_LL678: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA99=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA99->tag != 2)goto _LL67A;else{_tmpA9A=(struct Cyc_Core_Opt**)& _tmpA99->f1;_tmpA9B=_tmpA99->f2;}}_LL679:
# 4268
*_tmpA9A=Cyc_Tcutil_kind_to_bound_opt(_tmpA9B);goto _LL665;_LL67A: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA9C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA86;if(_tmpA9C->tag != 0)goto _LL67C;else{if((_tmpA9C->f1)->kind != Cyc_Absyn_MemKind)goto _LL67C;}}_LL67B:
# 4270
{const char*_tmp1205;void*_tmp1204;(_tmp1204=0,Cyc_Tcutil_terr(loc,((_tmp1205="functions can't abstract M types",_tag_dyneither(_tmp1205,sizeof(char),33))),_tag_dyneither(_tmp1204,sizeof(void*),0)));}goto _LL665;_LL67C:;_LL67D:
 goto _LL665;_LL665:;}}
# 4275
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpA21.kind_env,*_tmp93B);
_tmpA21.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpA21.r,_tmpA21.free_vars,*_tmp93B);
# 4278
{struct Cyc_List_List*tvs=_tmpA21.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _stmttmp80=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpAA3;int _tmpAA4;struct _tuple28 _tmpAA2=_stmttmp80;_tmpAA3=_tmpAA2.f1;_tmpAA4=_tmpAA2.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAA3,_tmpAA4);}}{
# 4283
struct Cyc_List_List*evs=_tmpA21.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _stmttmp81=*((struct _tuple29*)evs->hd);void*_tmpAA6;int _tmpAA7;struct _tuple29 _tmpAA5=_stmttmp81;_tmpAA6=_tmpAA5.f1;_tmpAA7=_tmpAA5.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpAA6,_tmpAA7);}};}
# 4288
_npop_handler(0);goto _LL585;
# 4093
;_pop_region(newr);};}_LL5A4: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp944=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp912;if(_tmp944->tag != 10)goto _LL5A6;else{_tmp945=_tmp944->f1;}}_LL5A5:
# 4291
 for(0;_tmp945 != 0;_tmp945=_tmp945->tl){
struct _tuple12*_tmpAA9=(struct _tuple12*)_tmp945->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpAA9).f2,1);
((*_tmpAA9).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAA9).f1).print_const,(*_tmpAA9).f2);}
# 4297
goto _LL585;_LL5A6: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp946=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp912;if(_tmp946->tag != 12)goto _LL5A8;else{_tmp947=_tmp946->f1;_tmp948=_tmp946->f2;}}_LL5A7: {
# 4301
struct _RegionHandle _tmpAAA=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpAAA;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp948 != 0;_tmp948=_tmp948->tl){
struct Cyc_Absyn_Aggrfield*_stmttmp6F=(struct Cyc_Absyn_Aggrfield*)_tmp948->hd;struct _dyneither_ptr*_tmpAAC;struct Cyc_Absyn_Tqual*_tmpAAD;void*_tmpAAE;struct Cyc_Absyn_Exp*_tmpAAF;struct Cyc_List_List*_tmpAB0;struct Cyc_Absyn_Exp*_tmpAB1;struct Cyc_Absyn_Aggrfield*_tmpAAB=_stmttmp6F;_tmpAAC=_tmpAAB->name;_tmpAAD=(struct Cyc_Absyn_Tqual*)& _tmpAAB->tq;_tmpAAE=_tmpAAB->type;_tmpAAF=_tmpAAB->width;_tmpAB0=_tmpAAB->attributes;_tmpAB1=_tmpAAB->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpAAC)){
const char*_tmp1209;void*_tmp1208[1];struct Cyc_String_pa_PrintArg_struct _tmp1207;(_tmp1207.tag=0,((_tmp1207.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAAC),((_tmp1208[0]=& _tmp1207,Cyc_Tcutil_terr(loc,((_tmp1209="duplicate field %s",_tag_dyneither(_tmp1209,sizeof(char),19))),_tag_dyneither(_tmp1208,sizeof(void*),1)))))));}
{const char*_tmp120A;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpAAC,((_tmp120A="",_tag_dyneither(_tmp120A,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp120B;prev_fields=((_tmp120B=_region_malloc(aprev_rgn,sizeof(*_tmp120B)),((_tmp120B->hd=_tmpAAC,((_tmp120B->tl=prev_fields,_tmp120B))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpAAE,1);
_tmpAAD->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpAAD->print_const,_tmpAAE);
if((_tmp947 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpAAE)) && _tmpAB1 == 0){
# 4313
const char*_tmp120F;void*_tmp120E[1];struct Cyc_String_pa_PrintArg_struct _tmp120D;(_tmp120D.tag=0,((_tmp120D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpAAC),((_tmp120E[0]=& _tmp120D,Cyc_Tcutil_warn(loc,((_tmp120F="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp120F,sizeof(char),74))),_tag_dyneither(_tmp120E,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpAAE,_tmpAAF,_tmpAAC);
Cyc_Tcutil_check_field_atts(loc,_tmpAAC,_tmpAB0);
if(_tmpAB1 != 0){
# 4318
if(_tmp947 != Cyc_Absyn_UnionA){
const char*_tmp1212;void*_tmp1211;(_tmp1211=0,Cyc_Tcutil_terr(loc,((_tmp1212="@requires clause is only allowed on union members",_tag_dyneither(_tmp1212,sizeof(char),50))),_tag_dyneither(_tmp1211,sizeof(void*),0)));}{
struct _RegionHandle _tmpABC=_new_region("temp");struct _RegionHandle*temp=& _tmpABC;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpABD=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpABD,0,_tmpAB1);}
# 4324
if(!Cyc_Tcutil_is_integral(_tmpAB1)){
const char*_tmp1216;void*_tmp1215[1];struct Cyc_String_pa_PrintArg_struct _tmp1214;(_tmp1214.tag=0,((_tmp1214.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpAB1->topt))),((_tmp1215[0]=& _tmp1214,Cyc_Tcutil_terr(loc,((_tmp1216="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp1216,sizeof(char),54))),_tag_dyneither(_tmp1215,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAB1,te,cvtenv);
# 4320
;_pop_region(temp);};}}}
# 4331
_npop_handler(0);goto _LL585;
# 4301
;_pop_region(aprev_rgn);}_LL5A8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp949=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp912;if(_tmp949->tag != 11)goto _LL5AA;else{_tmp94A=(union Cyc_Absyn_AggrInfoU*)&(_tmp949->f1).aggr_info;_tmp94B=(struct Cyc_List_List**)&(_tmp949->f1).targs;}}_LL5A9:
# 4334
{union Cyc_Absyn_AggrInfoU _stmttmp6D=*_tmp94A;union Cyc_Absyn_AggrInfoU _tmpAC1=_stmttmp6D;enum Cyc_Absyn_AggrKind _tmpAC2;struct _tuple2*_tmpAC3;struct Cyc_Core_Opt*_tmpAC4;struct Cyc_Absyn_Aggrdecl*_tmpAC5;_LL67F: if((_tmpAC1.UnknownAggr).tag != 1)goto _LL681;_tmpAC2=((struct _tuple4)(_tmpAC1.UnknownAggr).val).f1;_tmpAC3=((struct _tuple4)(_tmpAC1.UnknownAggr).val).f2;_tmpAC4=((struct _tuple4)(_tmpAC1.UnknownAggr).val).f3;_LL680: {
# 4336
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpAC6;_push_handler(& _tmpAC6);{int _tmpAC8=0;if(setjmp(_tmpAC6.handler))_tmpAC8=1;if(!_tmpAC8){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAC3);{
struct Cyc_Absyn_Aggrdecl*_tmpAC9=*adp;
if(_tmpAC9->kind != _tmpAC2){
if(_tmpAC9->kind == Cyc_Absyn_StructA){
const char*_tmp121B;void*_tmp121A[2];struct Cyc_String_pa_PrintArg_struct _tmp1219;struct Cyc_String_pa_PrintArg_struct _tmp1218;(_tmp1218.tag=0,((_tmp1218.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp1219.tag=0,((_tmp1219.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4342
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp121A[0]=& _tmp1219,((_tmp121A[1]=& _tmp1218,Cyc_Tcutil_terr(loc,((_tmp121B="expecting struct %s instead of union %s",_tag_dyneither(_tmp121B,sizeof(char),40))),_tag_dyneither(_tmp121A,sizeof(void*),2)))))))))))));}else{
# 4345
const char*_tmp1220;void*_tmp121F[2];struct Cyc_String_pa_PrintArg_struct _tmp121E;struct Cyc_String_pa_PrintArg_struct _tmp121D;(_tmp121D.tag=0,((_tmp121D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp121E.tag=0,((_tmp121E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4345
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp121F[0]=& _tmp121E,((_tmp121F[1]=& _tmp121D,Cyc_Tcutil_terr(loc,((_tmp1220="expecting union %s instead of struct %s",_tag_dyneither(_tmp1220,sizeof(char),40))),_tag_dyneither(_tmp121F,sizeof(void*),2)))))))))))));}}
# 4348
if((unsigned int)_tmpAC4  && (int)_tmpAC4->v){
if(!((unsigned int)_tmpAC9->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAC9->impl))->tagged){
const char*_tmp1224;void*_tmp1223[1];struct Cyc_String_pa_PrintArg_struct _tmp1222;(_tmp1222.tag=0,((_tmp1222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp1223[0]=& _tmp1222,Cyc_Tcutil_terr(loc,((_tmp1224="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp1224,sizeof(char),42))),_tag_dyneither(_tmp1223,sizeof(void*),1)))))));}}
# 4354
*_tmp94A=Cyc_Absyn_KnownAggr(adp);};
# 4338
;_pop_handler();}else{void*_tmpAC7=(void*)_exn_thrown;void*_tmpAD6=_tmpAC7;void*_tmpAD8;_LL684: {struct Cyc_Dict_Absent_exn_struct*_tmpAD7=(struct Cyc_Dict_Absent_exn_struct*)_tmpAD6;if(_tmpAD7->tag != Cyc_Dict_Absent)goto _LL686;}_LL685: {
# 4358
struct Cyc_Tcenv_Genv*_tmpAD9=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1225;struct Cyc_Absyn_Aggrdecl*_tmpADA=(_tmp1225=_cycalloc(sizeof(*_tmp1225)),((_tmp1225->kind=_tmpAC2,((_tmp1225->sc=Cyc_Absyn_Extern,((_tmp1225->name=_tmpAC3,((_tmp1225->tvs=0,((_tmp1225->impl=0,((_tmp1225->attributes=0,_tmp1225)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpAD9,loc,_tmpADA);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpAC3);
*_tmp94A=Cyc_Absyn_KnownAggr(adp);
# 4364
if(*_tmp94B != 0){
{const char*_tmp1229;void*_tmp1228[1];struct Cyc_String_pa_PrintArg_struct _tmp1227;(_tmp1227.tag=0,((_tmp1227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpAC3)),((_tmp1228[0]=& _tmp1227,Cyc_Tcutil_terr(loc,((_tmp1229="declare parameterized type %s before using",_tag_dyneither(_tmp1229,sizeof(char),43))),_tag_dyneither(_tmp1228,sizeof(void*),1)))))));}
return cvtenv;}
# 4369
goto _LL683;}_LL686: _tmpAD8=_tmpAD6;_LL687:(void)_throw(_tmpAD8);_LL683:;}};}
# 4371
_tmpAC5=*adp;goto _LL682;}_LL681: if((_tmpAC1.KnownAggr).tag != 2)goto _LL67E;_tmpAC5=*((struct Cyc_Absyn_Aggrdecl**)(_tmpAC1.KnownAggr).val);_LL682: {
# 4373
struct Cyc_List_List*tvs=_tmpAC5->tvs;
struct Cyc_List_List*ts=*_tmp94B;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpADF=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpAE0=(void*)ts->hd;
# 4381
{struct _tuple0 _tmp122A;struct _tuple0 _stmttmp6E=(_tmp122A.f1=Cyc_Absyn_compress_kb(_tmpADF->kind),((_tmp122A.f2=_tmpAE0,_tmp122A)));struct _tuple0 _tmpAE1=_stmttmp6E;struct Cyc_Absyn_Tvar*_tmpAE4;_LL689:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAE2=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAE1.f1;if(_tmpAE2->tag != 1)goto _LL68B;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAE3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAE1.f2;if(_tmpAE3->tag != 2)goto _LL68B;else{_tmpAE4=_tmpAE3->f1;}};_LL68A:
# 4383
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpAE4);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpAE4,1);
continue;_LL68B:;_LL68C:
 goto _LL688;_LL688:;}{
# 4388
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4392
if(ts != 0){
const char*_tmp122E;void*_tmp122D[1];struct Cyc_String_pa_PrintArg_struct _tmp122C;(_tmp122C.tag=0,((_tmp122C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpAC5->name)),((_tmp122D[0]=& _tmp122C,Cyc_Tcutil_terr(loc,((_tmp122E="too many parameters for type %s",_tag_dyneither(_tmp122E,sizeof(char),32))),_tag_dyneither(_tmp122D,sizeof(void*),1)))))));}
if(tvs != 0){
# 4396
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp122F;hidden_ts=((_tmp122F=_cycalloc(sizeof(*_tmp122F)),((_tmp122F->hd=e,((_tmp122F->tl=hidden_ts,_tmp122F))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4403
*_tmp94B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp94B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL67E:;}
# 4406
goto _LL585;_LL5AA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp94C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp912;if(_tmp94C->tag != 17)goto _LL5AC;else{_tmp94D=_tmp94C->f1;_tmp94E=(struct Cyc_List_List**)& _tmp94C->f2;_tmp94F=(struct Cyc_Absyn_Typedefdecl**)& _tmp94C->f3;_tmp950=(void**)((void**)& _tmp94C->f4);}}_LL5AB: {
# 4409
struct Cyc_List_List*targs=*_tmp94E;
# 4411
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpAEA;_push_handler(& _tmpAEA);{int _tmpAEC=0;if(setjmp(_tmpAEA.handler))_tmpAEC=1;if(!_tmpAEC){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp94D);;_pop_handler();}else{void*_tmpAEB=(void*)_exn_thrown;void*_tmpAEE=_tmpAEB;void*_tmpAF0;_LL68E: {struct Cyc_Dict_Absent_exn_struct*_tmpAEF=(struct Cyc_Dict_Absent_exn_struct*)_tmpAEE;if(_tmpAEF->tag != Cyc_Dict_Absent)goto _LL690;}_LL68F:
# 4414
{const char*_tmp1233;void*_tmp1232[1];struct Cyc_String_pa_PrintArg_struct _tmp1231;(_tmp1231.tag=0,((_tmp1231.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp94D)),((_tmp1232[0]=& _tmp1231,Cyc_Tcutil_terr(loc,((_tmp1233="unbound typedef name %s",_tag_dyneither(_tmp1233,sizeof(char),24))),_tag_dyneither(_tmp1232,sizeof(void*),1)))))));}
return cvtenv;_LL690: _tmpAF0=_tmpAEE;_LL691:(void)_throw(_tmpAF0);_LL68D:;}};}
# 4417
*_tmp94F=td;
# 4419
_tmp94D[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpAF4=_new_region("temp");struct _RegionHandle*temp=& _tmpAF4;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4425
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4430
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1236;struct Cyc_List_List*_tmp1235;inst=((_tmp1235=_region_malloc(temp,sizeof(*_tmp1235)),((_tmp1235->hd=((_tmp1236=_region_malloc(temp,sizeof(*_tmp1236)),((_tmp1236->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1236->f2=(void*)ts->hd,_tmp1236)))))),((_tmp1235->tl=inst,_tmp1235))))));};}
# 4434
if(ts != 0){
const char*_tmp123A;void*_tmp1239[1];struct Cyc_String_pa_PrintArg_struct _tmp1238;(_tmp1238.tag=0,((_tmp1238.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp94D)),((_tmp1239[0]=& _tmp1238,Cyc_Tcutil_terr(loc,((_tmp123A="too many parameters for typedef %s",_tag_dyneither(_tmp123A,sizeof(char),35))),_tag_dyneither(_tmp1239,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4439
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp123B;hidden_ts=((_tmp123B=_cycalloc(sizeof(*_tmp123B)),((_tmp123B->hd=e,((_tmp123B->tl=hidden_ts,_tmp123B))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp123E;struct Cyc_List_List*_tmp123D;inst=((_tmp123D=_cycalloc(sizeof(*_tmp123D)),((_tmp123D->hd=((_tmp123E=_cycalloc(sizeof(*_tmp123E)),((_tmp123E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp123E->f2=e,_tmp123E)))))),((_tmp123D->tl=inst,_tmp123D))))));};}
# 4447
*_tmp94E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4449
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp950=new_typ;}}
# 4456
_npop_handler(0);goto _LL585;
# 4422
;_pop_region(temp);};}_LL5AC: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp951=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp912;if(_tmp951->tag != 22)goto _LL5AE;}_LL5AD:
# 4457
 goto _LL5AF;_LL5AE: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp952=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp912;if(_tmp952->tag != 21)goto _LL5B0;}_LL5AF:
 goto _LL5B1;_LL5B0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp953=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp912;if(_tmp953->tag != 20)goto _LL5B2;}_LL5B1:
 goto _LL585;_LL5B2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp954=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp912;if(_tmp954->tag != 15)goto _LL5B4;else{_tmp955=(void*)_tmp954->f1;}}_LL5B3:
# 4461
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp955,1);goto _LL585;_LL5B4: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp956=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp912;if(_tmp956->tag != 16)goto _LL5B6;else{_tmp957=(void*)_tmp956->f1;_tmp958=(void*)_tmp956->f2;}}_LL5B5:
# 4464
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp957,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp958,1);
goto _LL585;_LL5B6: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp959=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp912;if(_tmp959->tag != 23)goto _LL5B8;else{_tmp95A=(void*)_tmp959->f1;}}_LL5B7:
# 4468
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp95A,1);goto _LL585;_LL5B8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp95B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp912;if(_tmp95B->tag != 25)goto _LL5BA;else{_tmp95C=(void*)_tmp95B->f1;}}_LL5B9:
# 4470
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp95C,1);goto _LL585;_LL5BA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp95D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp912;if(_tmp95D->tag != 24)goto _LL585;else{_tmp95E=_tmp95D->f1;}}_LL5BB:
# 4472
 for(0;_tmp95E != 0;_tmp95E=_tmp95E->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp95E->hd,1);}
goto _LL585;_LL585:;}
# 4476
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpAFD=t;struct Cyc_Core_Opt*_tmpAFF;void*_tmpB00;_LL693: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpAFE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpAFD;if(_tmpAFE->tag != 1)goto _LL695;else{_tmpAFF=_tmpAFE->f1;_tmpB00=(void*)_tmpAFE->f2;}}_LL694: {
# 4479
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpB01=_tmpAFF;struct Cyc_Absyn_Kind*_tmpB02;_LL698: if(_tmpB01 != 0)goto _LL69A;_LL699:
{const char*_tmp123F;s=((_tmp123F="kind=NULL ",_tag_dyneither(_tmp123F,sizeof(char),11)));}goto _LL697;_LL69A: if(_tmpB01 == 0)goto _LL697;_tmpB02=(struct Cyc_Absyn_Kind*)_tmpB01->v;_LL69B:
{const char*_tmp1243;void*_tmp1242[1];struct Cyc_String_pa_PrintArg_struct _tmp1241;s=(struct _dyneither_ptr)((_tmp1241.tag=0,((_tmp1241.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB02)),((_tmp1242[0]=& _tmp1241,Cyc_aprintf(((_tmp1243="kind=%s ",_tag_dyneither(_tmp1243,sizeof(char),9))),_tag_dyneither(_tmp1242,sizeof(void*),1))))))));}goto _LL697;_LL697:;}
# 4484
if(_tmpB00 == 0){
const char*_tmp1247;void*_tmp1246[1];struct Cyc_String_pa_PrintArg_struct _tmp1245;s=(struct _dyneither_ptr)((_tmp1245.tag=0,((_tmp1245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1246[0]=& _tmp1245,Cyc_aprintf(((_tmp1247="%s ref=NULL",_tag_dyneither(_tmp1247,sizeof(char),12))),_tag_dyneither(_tmp1246,sizeof(void*),1))))))));}else{
# 4487
const char*_tmp124C;void*_tmp124B[2];struct Cyc_String_pa_PrintArg_struct _tmp124A;struct Cyc_String_pa_PrintArg_struct _tmp1249;s=(struct _dyneither_ptr)((_tmp1249.tag=0,((_tmp1249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB00)),((_tmp124A.tag=0,((_tmp124A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp124B[0]=& _tmp124A,((_tmp124B[1]=& _tmp1249,Cyc_aprintf(((_tmp124C="%s ref=%s",_tag_dyneither(_tmp124C,sizeof(char),10))),_tag_dyneither(_tmp124B,sizeof(void*),2))))))))))))));}
# 4489
goto _LL692;}_LL695:;_LL696:
 goto _LL692;_LL692:;}{
# 4492
const char*_tmp1252;void*_tmp1251[3];struct Cyc_String_pa_PrintArg_struct _tmp1250;struct Cyc_String_pa_PrintArg_struct _tmp124F;struct Cyc_String_pa_PrintArg_struct _tmp124E;(_tmp124E.tag=0,((_tmp124E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp124F.tag=0,((_tmp124F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1250.tag=0,((_tmp1250.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1251[0]=& _tmp1250,((_tmp1251[1]=& _tmp124F,((_tmp1251[2]=& _tmp124E,Cyc_Tcutil_terr(loc,((_tmp1252="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1252,sizeof(char),51))),_tag_dyneither(_tmp1251,sizeof(void*),3)))))))))))))))))));};}
# 4495
return cvtenv;}
# 4503
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4505
{void*_stmttmp91=e->r;void*_tmpB13=_stmttmp91;struct Cyc_List_List*_tmpB19;struct Cyc_Absyn_Exp*_tmpB1B;struct Cyc_Absyn_Exp*_tmpB1C;struct Cyc_Absyn_Exp*_tmpB1D;struct Cyc_Absyn_Exp*_tmpB1F;struct Cyc_Absyn_Exp*_tmpB20;struct Cyc_Absyn_Exp*_tmpB22;struct Cyc_Absyn_Exp*_tmpB23;struct Cyc_Absyn_Exp*_tmpB25;struct Cyc_Absyn_Exp*_tmpB26;void*_tmpB28;struct Cyc_Absyn_Exp*_tmpB29;void*_tmpB2B;void*_tmpB2D;void*_tmpB2F;struct Cyc_Absyn_Exp*_tmpB31;_LL69D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpB14=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB14->tag != 0)goto _LL69F;}_LL69E:
 goto _LL6A0;_LL69F: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpB15=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB15->tag != 31)goto _LL6A1;}_LL6A0:
 goto _LL6A2;_LL6A1: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpB16=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB16->tag != 32)goto _LL6A3;}_LL6A2:
 goto _LL6A4;_LL6A3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpB17=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB17->tag != 1)goto _LL6A5;}_LL6A4:
 goto _LL69C;_LL6A5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpB18=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB18->tag != 2)goto _LL6A7;else{_tmpB19=_tmpB18->f2;}}_LL6A6:
# 4511
 for(0;_tmpB19 != 0;_tmpB19=_tmpB19->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpB19->hd,te,cvtenv);}
goto _LL69C;_LL6A7: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpB1A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB1A->tag != 5)goto _LL6A9;else{_tmpB1B=_tmpB1A->f1;_tmpB1C=_tmpB1A->f2;_tmpB1D=_tmpB1A->f3;}}_LL6A8:
# 4515
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1B,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB1D,te,cvtenv);
goto _LL69C;_LL6A9: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpB1E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB1E->tag != 6)goto _LL6AB;else{_tmpB1F=_tmpB1E->f1;_tmpB20=_tmpB1E->f2;}}_LL6AA:
 _tmpB22=_tmpB1F;_tmpB23=_tmpB20;goto _LL6AC;_LL6AB: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpB21=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB21->tag != 7)goto _LL6AD;else{_tmpB22=_tmpB21->f1;_tmpB23=_tmpB21->f2;}}_LL6AC:
 _tmpB25=_tmpB22;_tmpB26=_tmpB23;goto _LL6AE;_LL6AD: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpB24=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB24->tag != 8)goto _LL6AF;else{_tmpB25=_tmpB24->f1;_tmpB26=_tmpB24->f2;}}_LL6AE:
# 4522
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB25,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB26,te,cvtenv);
goto _LL69C;_LL6AF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpB27=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB27->tag != 13)goto _LL6B1;else{_tmpB28=(void*)_tmpB27->f1;_tmpB29=_tmpB27->f2;}}_LL6B0:
# 4526
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB29,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB28,1);
goto _LL69C;_LL6B1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpB2A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB2A->tag != 18)goto _LL6B3;else{_tmpB2B=(void*)_tmpB2A->f1;}}_LL6B2:
 _tmpB2D=_tmpB2B;goto _LL6B4;_LL6B3: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpB2C=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB2C->tag != 16)goto _LL6B5;else{_tmpB2D=(void*)_tmpB2C->f1;}}_LL6B4:
# 4531
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpB2D,1);
goto _LL69C;_LL6B5: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpB2E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB2E->tag != 38)goto _LL6B7;else{_tmpB2F=(void*)_tmpB2E->f1;}}_LL6B6:
# 4534
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpB2F,1);
goto _LL69C;_LL6B7: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpB30=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB13;if(_tmpB30->tag != 17)goto _LL6B9;else{_tmpB31=_tmpB30->f1;}}_LL6B8:
# 4537
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB31,te,cvtenv);
goto _LL69C;_LL6B9:;_LL6BA: {
# 4540
const char*_tmp1255;void*_tmp1254;(_tmp1254=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1255="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1255,sizeof(char),66))),_tag_dyneither(_tmp1254,sizeof(void*),0)));}_LL69C:;}
# 4542
return cvtenv;}
# 4545
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4550
struct Cyc_List_List*_tmpB34=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4553
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _stmttmp92=*((struct _tuple28*)vs->hd);struct Cyc_Absyn_Tvar*_tmpB36;struct _tuple28 _tmpB35=_stmttmp92;_tmpB36=_tmpB35.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpB34,_tmpB36);}}
# 4561
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _stmttmp93=*((struct _tuple29*)evs->hd);void*_tmpB38;struct _tuple29 _tmpB37=_stmttmp93;_tmpB38=_tmpB37.f1;{
void*_stmttmp94=Cyc_Tcutil_compress(_tmpB38);void*_tmpB39=_stmttmp94;struct Cyc_Core_Opt**_tmpB3B;_LL6BC: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB3A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB39;if(_tmpB3A->tag != 1)goto _LL6BE;else{_tmpB3B=(struct Cyc_Core_Opt**)& _tmpB3A->f4;}}_LL6BD:
# 4565
 if(*_tmpB3B == 0){
struct Cyc_Core_Opt*_tmp1256;*_tmpB3B=((_tmp1256=_cycalloc(sizeof(*_tmp1256)),((_tmp1256->v=_tmpB34,_tmp1256))));}else{
# 4569
struct Cyc_List_List*_tmpB3D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpB3B))->v;
struct Cyc_List_List*_tmpB3E=0;
for(0;_tmpB3D != 0;_tmpB3D=_tmpB3D->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpB34,(struct Cyc_Absyn_Tvar*)_tmpB3D->hd)){
struct Cyc_List_List*_tmp1257;_tmpB3E=((_tmp1257=_cycalloc(sizeof(*_tmp1257)),((_tmp1257->hd=(struct Cyc_Absyn_Tvar*)_tmpB3D->hd,((_tmp1257->tl=_tmpB3E,_tmp1257))))));}}{
struct Cyc_Core_Opt*_tmp1258;*_tmpB3B=((_tmp1258=_cycalloc(sizeof(*_tmp1258)),((_tmp1258->v=_tmpB3E,_tmp1258))));};}
# 4576
goto _LL6BB;_LL6BE:;_LL6BF:
 goto _LL6BB;_LL6BB:;};}}
# 4580
return cvt;}
# 4586
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4588
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_stmttmp95=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpB41=_stmttmp95;_LL6C1: if(_tmpB41->kind != Cyc_Absyn_RgnKind)goto _LL6C3;if(_tmpB41->aliasqual != Cyc_Absyn_Unique)goto _LL6C3;_LL6C2:
# 4592
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp125B;void*_tmp125A;(_tmp125A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp125B="can't unify evar with unique region!",_tag_dyneither(_tmp125B,sizeof(char),37))),_tag_dyneither(_tmp125A,sizeof(void*),0)));}
goto _LL6C0;_LL6C3: if(_tmpB41->kind != Cyc_Absyn_RgnKind)goto _LL6C5;if(_tmpB41->aliasqual != Cyc_Absyn_Aliasable)goto _LL6C5;_LL6C4:
 goto _LL6C6;_LL6C5: if(_tmpB41->kind != Cyc_Absyn_RgnKind)goto _LL6C7;if(_tmpB41->aliasqual != Cyc_Absyn_Top)goto _LL6C7;_LL6C6:
# 4597
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp125E;void*_tmp125D;(_tmp125D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp125E="can't unify evar with heap!",_tag_dyneither(_tmp125E,sizeof(char),28))),_tag_dyneither(_tmp125D,sizeof(void*),0)));}
goto _LL6C0;_LL6C7: if(_tmpB41->kind != Cyc_Absyn_EffKind)goto _LL6C9;_LL6C8:
# 4600
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1261;void*_tmp1260;(_tmp1260=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1261="can't unify evar with {}!",_tag_dyneither(_tmp1261,sizeof(char),26))),_tag_dyneither(_tmp1260,sizeof(void*),0)));}
goto _LL6C0;_LL6C9:;_LL6CA:
# 4603
{const char*_tmp1266;void*_tmp1265[2];struct Cyc_String_pa_PrintArg_struct _tmp1264;struct Cyc_String_pa_PrintArg_struct _tmp1263;(_tmp1263.tag=0,((_tmp1263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t)),((_tmp1264.tag=0,((_tmp1264.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1265[0]=& _tmp1264,((_tmp1265[1]=& _tmp1263,Cyc_Tcutil_terr(loc,((_tmp1266="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1266,sizeof(char),52))),_tag_dyneither(_tmp1265,sizeof(void*),2)))))))))))));}
goto _LL6C0;_LL6C0:;}}
# 4615
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpB4C=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpB4D=_new_region("temp");struct _RegionHandle*temp=& _tmpB4D;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp1267;struct Cyc_Tcutil_CVTEnv _tmpB4E=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1267.r=temp,((_tmp1267.kind_env=_tmpB4C,((_tmp1267.free_vars=0,((_tmp1267.free_evars=0,((_tmp1267.generalize_evars=generalize_evars,((_tmp1267.fn_result=0,_tmp1267)))))))))))),& Cyc_Tcutil_tmk,t);
# 4623
struct Cyc_List_List*_tmpB4F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB4E.free_vars);
struct Cyc_List_List*_tmpB50=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB4E.free_evars);
# 4627
if(_tmpB4C != 0){
struct Cyc_List_List*_tmpB51=0;
{struct Cyc_List_List*_tmpB52=_tmpB4F;for(0;(unsigned int)_tmpB52;_tmpB52=_tmpB52->tl){
struct Cyc_Absyn_Tvar*_tmpB53=(struct Cyc_Absyn_Tvar*)_tmpB52->hd;
int found=0;
{struct Cyc_List_List*_tmpB54=_tmpB4C;for(0;(unsigned int)_tmpB54;_tmpB54=_tmpB54->tl){
if(Cyc_Absyn_tvar_cmp(_tmpB53,(struct Cyc_Absyn_Tvar*)_tmpB54->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp1268;_tmpB51=((_tmp1268=_region_malloc(temp,sizeof(*_tmp1268)),((_tmp1268->hd=(struct Cyc_Absyn_Tvar*)_tmpB52->hd,((_tmp1268->tl=_tmpB51,_tmp1268))))));}}}
# 4637
_tmpB4F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB51);}
# 4643
{struct Cyc_List_List*x=_tmpB4F;for(0;x != 0;x=x->tl){
void*_stmttmp96=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmpB56=_stmttmp96;struct Cyc_Core_Opt**_tmpB58;struct Cyc_Core_Opt**_tmpB5A;struct Cyc_Core_Opt**_tmpB5C;struct Cyc_Core_Opt**_tmpB5E;struct Cyc_Core_Opt**_tmpB60;struct Cyc_Core_Opt**_tmpB62;struct Cyc_Core_Opt**_tmpB64;struct Cyc_Absyn_Kind*_tmpB65;enum Cyc_Absyn_AliasQual _tmpB67;_LL6CC: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB57=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB56;if(_tmpB57->tag != 1)goto _LL6CE;else{_tmpB58=(struct Cyc_Core_Opt**)& _tmpB57->f1;}}_LL6CD:
 _tmpB5A=_tmpB58;goto _LL6CF;_LL6CE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB59=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB56;if(_tmpB59->tag != 2)goto _LL6D0;else{_tmpB5A=(struct Cyc_Core_Opt**)& _tmpB59->f1;if((_tmpB59->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6D0;if((_tmpB59->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D0;}}_LL6CF:
 _tmpB5C=_tmpB5A;goto _LL6D1;_LL6D0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB5B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB56;if(_tmpB5B->tag != 2)goto _LL6D2;else{_tmpB5C=(struct Cyc_Core_Opt**)& _tmpB5B->f1;if((_tmpB5B->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D2;if((_tmpB5B->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D2;}}_LL6D1:
 _tmpB5E=_tmpB5C;goto _LL6D3;_LL6D2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB5D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB56;if(_tmpB5D->tag != 2)goto _LL6D4;else{_tmpB5E=(struct Cyc_Core_Opt**)& _tmpB5D->f1;if((_tmpB5D->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D4;if((_tmpB5D->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6D4;}}_LL6D3:
# 4649
*_tmpB5E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6CB;_LL6D4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB5F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB56;if(_tmpB5F->tag != 2)goto _LL6D6;else{_tmpB60=(struct Cyc_Core_Opt**)& _tmpB5F->f1;if((_tmpB5F->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D6;if((_tmpB5F->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6D6;}}_LL6D5:
# 4651
*_tmpB60=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6CB;_LL6D6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB61=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB56;if(_tmpB61->tag != 2)goto _LL6D8;else{_tmpB62=(struct Cyc_Core_Opt**)& _tmpB61->f1;if((_tmpB61->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6D8;if((_tmpB61->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D8;}}_LL6D7:
# 4653
*_tmpB62=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6CB;_LL6D8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB63=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB56;if(_tmpB63->tag != 2)goto _LL6DA;else{_tmpB64=(struct Cyc_Core_Opt**)& _tmpB63->f1;_tmpB65=_tmpB63->f2;}}_LL6D9:
# 4655
*_tmpB64=Cyc_Tcutil_kind_to_bound_opt(_tmpB65);goto _LL6CB;_LL6DA: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB66=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB56;if(_tmpB66->tag != 0)goto _LL6DC;else{if((_tmpB66->f1)->kind != Cyc_Absyn_MemKind)goto _LL6DC;_tmpB67=(_tmpB66->f1)->aliasqual;}}_LL6DB:
# 4657
{const char*_tmp126F;void*_tmp126E[2];struct Cyc_String_pa_PrintArg_struct _tmp126D;struct Cyc_Absyn_Kind*_tmp126C;struct Cyc_String_pa_PrintArg_struct _tmp126B;(_tmp126B.tag=0,((_tmp126B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp126C=_cycalloc_atomic(sizeof(*_tmp126C)),((_tmp126C->kind=Cyc_Absyn_MemKind,((_tmp126C->aliasqual=_tmpB67,_tmp126C)))))))),((_tmp126D.tag=0,((_tmp126D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp126E[0]=& _tmp126D,((_tmp126E[1]=& _tmp126B,Cyc_Tcutil_terr(loc,((_tmp126F="type variable %s cannot have kind %s",_tag_dyneither(_tmp126F,sizeof(char),37))),_tag_dyneither(_tmp126E,sizeof(void*),2)))))))))))));}
goto _LL6CB;_LL6DC:;_LL6DD:
 goto _LL6CB;_LL6CB:;}}
# 4664
if(_tmpB4F != 0  || _tmpB50 != 0){
{void*_stmttmp97=Cyc_Tcutil_compress(t);void*_tmpB6D=_stmttmp97;struct Cyc_List_List**_tmpB6F;void*_tmpB70;struct Cyc_Absyn_Tqual _tmpB71;void*_tmpB72;struct Cyc_List_List*_tmpB73;int _tmpB74;struct Cyc_Absyn_VarargInfo*_tmpB75;struct Cyc_List_List*_tmpB76;struct Cyc_List_List*_tmpB77;_LL6DF: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB6E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB6D;if(_tmpB6E->tag != 9)goto _LL6E1;else{_tmpB6F=(struct Cyc_List_List**)&(_tmpB6E->f1).tvars;_tmpB70=(_tmpB6E->f1).effect;_tmpB71=(_tmpB6E->f1).ret_tqual;_tmpB72=(_tmpB6E->f1).ret_typ;_tmpB73=(_tmpB6E->f1).args;_tmpB74=(_tmpB6E->f1).c_varargs;_tmpB75=(_tmpB6E->f1).cyc_varargs;_tmpB76=(_tmpB6E->f1).rgn_po;_tmpB77=(_tmpB6E->f1).attributes;}}_LL6E0:
# 4667
 if(*_tmpB6F == 0){
# 4669
*_tmpB6F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpB4F);
_tmpB4F=0;}
# 4672
goto _LL6DE;_LL6E1:;_LL6E2:
 goto _LL6DE;_LL6DE:;}
# 4675
if(_tmpB4F != 0){
const char*_tmp1273;void*_tmp1272[1];struct Cyc_String_pa_PrintArg_struct _tmp1271;(_tmp1271.tag=0,((_tmp1271.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpB4F->hd)->name),((_tmp1272[0]=& _tmp1271,Cyc_Tcutil_terr(loc,((_tmp1273="unbound type variable %s",_tag_dyneither(_tmp1273,sizeof(char),25))),_tag_dyneither(_tmp1272,sizeof(void*),1)))))));}
Cyc_Tcutil_check_free_evars(_tmpB50,t,loc);}}
# 4619
;_pop_region(temp);}
# 4686
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4689
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_stmttmp98=Cyc_Tcutil_compress(t);void*_tmpB7C=_stmttmp98;struct Cyc_List_List*_tmpB7E;void*_tmpB7F;struct Cyc_Absyn_Tqual _tmpB80;void*_tmpB81;struct Cyc_List_List*_tmpB82;_LL6E4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB7D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB7C;if(_tmpB7D->tag != 9)goto _LL6E6;else{_tmpB7E=(_tmpB7D->f1).tvars;_tmpB7F=(_tmpB7D->f1).effect;_tmpB80=(_tmpB7D->f1).ret_tqual;_tmpB81=(_tmpB7D->f1).ret_typ;_tmpB82=(_tmpB7D->f1).args;}}_LL6E5:
# 4692
 fd->tvs=_tmpB7E;
fd->effect=_tmpB7F;
{struct Cyc_List_List*_tmpB83=fd->args;for(0;_tmpB83 != 0;(_tmpB83=_tmpB83->tl,_tmpB82=_tmpB82->tl)){
# 4696
(*((struct _tuple10*)_tmpB83->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpB82))->hd)).f2;
(*((struct _tuple10*)_tmpB83->hd)).f3=(*((struct _tuple10*)_tmpB82->hd)).f3;}}
# 4699
fd->ret_tqual=_tmpB80;
fd->ret_type=_tmpB81;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpB81);
goto _LL6E3;_LL6E6:;_LL6E7: {
const char*_tmp1276;void*_tmp1275;(_tmp1275=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1276="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1276,sizeof(char),38))),_tag_dyneither(_tmp1275,sizeof(void*),0)));}_LL6E3:;}{
# 4706
struct _RegionHandle _tmpB86=_new_region("r");struct _RegionHandle*r=& _tmpB86;_push_region(r);{
const char*_tmp1277;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp1277="function declaration has repeated parameter",_tag_dyneither(_tmp1277,sizeof(char),44))));}
# 4711
fd->cached_typ=t;
# 4706
;_pop_region(r);};}
# 4716
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4719
struct _RegionHandle _tmpB88=_new_region("r");struct _RegionHandle*r=& _tmpB88;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp1278;struct Cyc_Tcutil_CVTEnv _tmpB89=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1278.r=r,((_tmp1278.kind_env=bound_tvars,((_tmp1278.free_vars=0,((_tmp1278.free_evars=0,((_tmp1278.generalize_evars=0,((_tmp1278.fn_result=0,_tmp1278)))))))))))),expected_kind,t);
# 4723
struct Cyc_List_List*_tmpB8A=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpB89.free_vars),bound_tvars);
# 4725
struct Cyc_List_List*_tmpB8B=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpB89.free_evars);
{struct Cyc_List_List*fs=_tmpB8A;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpB8C=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp127D;void*_tmp127C[2];struct Cyc_String_pa_PrintArg_struct _tmp127B;struct Cyc_String_pa_PrintArg_struct _tmp127A;(_tmp127A.tag=0,((_tmp127A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp127B.tag=0,((_tmp127B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB8C),((_tmp127C[0]=& _tmp127B,((_tmp127C[1]=& _tmp127A,Cyc_Tcutil_terr(loc,((_tmp127D="unbound type variable %s in type %s",_tag_dyneither(_tmp127D,sizeof(char),36))),_tag_dyneither(_tmp127C,sizeof(void*),2)))))))))))));}}
# 4730
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmpB8B,t,loc);}
# 4720
;_pop_region(r);}
# 4735
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4740
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4746
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4749
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp1282;void*_tmp1281[2];struct Cyc_String_pa_PrintArg_struct _tmp1280;struct Cyc_String_pa_PrintArg_struct _tmp127F;(_tmp127F.tag=0,((_tmp127F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp1280.tag=0,((_tmp1280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1281[0]=& _tmp1280,((_tmp1281[1]=& _tmp127F,Cyc_Tcutil_terr(loc,((_tmp1282="%s: %s",_tag_dyneither(_tmp1282,sizeof(char),7))),_tag_dyneither(_tmp1281,sizeof(void*),2)))))))))))));}}}}
# 4755
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4759
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4763
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp1283;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1283="duplicate type variable",_tag_dyneither(_tmp1283,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4777 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4782
struct _RegionHandle _tmpB97=_new_region("temp");struct _RegionHandle*temp=& _tmpB97;_push_region(temp);
# 4786
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp1284;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp1284="",_tag_dyneither(_tmp1284,sizeof(char),1))))!= 0){
struct _tuple31*_tmp1287;struct Cyc_List_List*_tmp1286;fields=((_tmp1286=_region_malloc(temp,sizeof(*_tmp1286)),((_tmp1286->hd=((_tmp1287=_region_malloc(temp,sizeof(*_tmp1287)),((_tmp1287->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1287->f2=0,_tmp1287)))))),((_tmp1286->tl=fields,_tmp1286))))));}}}
# 4791
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4793
const char*_tmp1289;const char*_tmp1288;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp1289="struct",_tag_dyneither(_tmp1289,sizeof(char),7)):((_tmp1288="union",_tag_dyneither(_tmp1288,sizeof(char),6)));
# 4796
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple32*_stmttmp99=(struct _tuple32*)des->hd;struct Cyc_List_List*_tmpB9C;void*_tmpB9D;struct _tuple32*_tmpB9B=_stmttmp99;_tmpB9C=_tmpB9B->f1;_tmpB9D=_tmpB9B->f2;
if(_tmpB9C == 0){
# 4801
struct Cyc_List_List*_tmpB9E=fields;
for(0;_tmpB9E != 0;_tmpB9E=_tmpB9E->tl){
if(!(*((struct _tuple31*)_tmpB9E->hd)).f2){
(*((struct _tuple31*)_tmpB9E->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp128F;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp128E;struct Cyc_List_List*_tmp128D;(*((struct _tuple32*)des->hd)).f1=((_tmp128D=_cycalloc(sizeof(*_tmp128D)),((_tmp128D->hd=(void*)((_tmp128F=_cycalloc(sizeof(*_tmp128F)),((_tmp128F[0]=((_tmp128E.tag=1,((_tmp128E.f1=((*((struct _tuple31*)_tmpB9E->hd)).f1)->name,_tmp128E)))),_tmp128F)))),((_tmp128D->tl=0,_tmp128D))))));}
{struct _tuple33*_tmp1292;struct Cyc_List_List*_tmp1291;ans=((_tmp1291=_region_malloc(rgn,sizeof(*_tmp1291)),((_tmp1291->hd=((_tmp1292=_region_malloc(rgn,sizeof(*_tmp1292)),((_tmp1292->f1=(*((struct _tuple31*)_tmpB9E->hd)).f1,((_tmp1292->f2=_tmpB9D,_tmp1292)))))),((_tmp1291->tl=ans,_tmp1291))))));}
break;}}
# 4809
if(_tmpB9E == 0){
const char*_tmp1296;void*_tmp1295[1];struct Cyc_String_pa_PrintArg_struct _tmp1294;(_tmp1294.tag=0,((_tmp1294.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1295[0]=& _tmp1294,Cyc_Tcutil_terr(loc,((_tmp1296="too many arguments to %s",_tag_dyneither(_tmp1296,sizeof(char),25))),_tag_dyneither(_tmp1295,sizeof(void*),1)))))));}}else{
if(_tmpB9C->tl != 0){
# 4813
const char*_tmp1299;void*_tmp1298;(_tmp1298=0,Cyc_Tcutil_terr(loc,((_tmp1299="multiple designators are not yet supported",_tag_dyneither(_tmp1299,sizeof(char),43))),_tag_dyneither(_tmp1298,sizeof(void*),0)));}else{
# 4816
void*_stmttmp9A=(void*)_tmpB9C->hd;void*_tmpBA9=_stmttmp9A;struct _dyneither_ptr*_tmpBAC;_LL6E9: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpBAA=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpBA9;if(_tmpBAA->tag != 0)goto _LL6EB;}_LL6EA:
# 4818
{const char*_tmp129D;void*_tmp129C[1];struct Cyc_String_pa_PrintArg_struct _tmp129B;(_tmp129B.tag=0,((_tmp129B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp129C[0]=& _tmp129B,Cyc_Tcutil_terr(loc,((_tmp129D="array designator used in argument to %s",_tag_dyneither(_tmp129D,sizeof(char),40))),_tag_dyneither(_tmp129C,sizeof(void*),1)))))));}
goto _LL6E8;_LL6EB: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpBAB=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpBA9;if(_tmpBAB->tag != 1)goto _LL6E8;else{_tmpBAC=_tmpBAB->f1;}}_LL6EC: {
# 4821
struct Cyc_List_List*_tmpBB0=fields;
for(0;_tmpBB0 != 0;_tmpBB0=_tmpBB0->tl){
if(Cyc_strptrcmp(_tmpBAC,((*((struct _tuple31*)_tmpBB0->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpBB0->hd)).f2){
const char*_tmp12A1;void*_tmp12A0[1];struct Cyc_String_pa_PrintArg_struct _tmp129F;(_tmp129F.tag=0,((_tmp129F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBAC),((_tmp12A0[0]=& _tmp129F,Cyc_Tcutil_terr(loc,((_tmp12A1="member %s has already been used as an argument",_tag_dyneither(_tmp12A1,sizeof(char),47))),_tag_dyneither(_tmp12A0,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpBB0->hd)).f2=1;
{struct _tuple33*_tmp12A4;struct Cyc_List_List*_tmp12A3;ans=((_tmp12A3=_region_malloc(rgn,sizeof(*_tmp12A3)),((_tmp12A3->hd=((_tmp12A4=_region_malloc(rgn,sizeof(*_tmp12A4)),((_tmp12A4->f1=(*((struct _tuple31*)_tmpBB0->hd)).f1,((_tmp12A4->f2=_tmpB9D,_tmp12A4)))))),((_tmp12A3->tl=ans,_tmp12A3))))));}
break;}}
# 4830
if(_tmpBB0 == 0){
const char*_tmp12A8;void*_tmp12A7[1];struct Cyc_String_pa_PrintArg_struct _tmp12A6;(_tmp12A6.tag=0,((_tmp12A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpBAC),((_tmp12A7[0]=& _tmp12A6,Cyc_Tcutil_terr(loc,((_tmp12A8="bad field designator %s",_tag_dyneither(_tmp12A8,sizeof(char),24))),_tag_dyneither(_tmp12A7,sizeof(void*),1)))))));}
goto _LL6E8;}_LL6E8:;}}}
# 4835
if(aggr_kind == Cyc_Absyn_StructA)
# 4837
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp12AB;void*_tmp12AA;(_tmp12AA=0,Cyc_Tcutil_terr(loc,((_tmp12AB="too few arguments to struct",_tag_dyneither(_tmp12AB,sizeof(char),28))),_tag_dyneither(_tmp12AA,sizeof(void*),0)));}
break;}}else{
# 4844
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp12AE;void*_tmp12AD;(_tmp12AD=0,Cyc_Tcutil_terr(loc,((_tmp12AE="only one member of a union is allowed",_tag_dyneither(_tmp12AE,sizeof(char),38))),_tag_dyneither(_tmp12AD,sizeof(void*),0)));}
found=1;}}
# 4851
if(!found){const char*_tmp12B1;void*_tmp12B0;(_tmp12B0=0,Cyc_Tcutil_terr(loc,((_tmp12B1="missing member for union",_tag_dyneither(_tmp12B1,sizeof(char),25))),_tag_dyneither(_tmp12B0,sizeof(void*),0)));}}{
# 4854
struct Cyc_List_List*_tmpBBF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpBBF;};};}
# 4786
;_pop_region(temp);}
# 4857
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_stmttmp9B=Cyc_Tcutil_compress(t);void*_tmpBC2=_stmttmp9B;void*_tmpBC4;union Cyc_Absyn_Constraint*_tmpBC5;_LL6EE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBC3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBC2;if(_tmpBC3->tag != 5)goto _LL6F0;else{_tmpBC4=(_tmpBC3->f1).elt_typ;_tmpBC5=((_tmpBC3->f1).ptr_atts).bounds;}}_LL6EF: {
# 4860
void*_stmttmp9C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpBC5);void*_tmpBC6=_stmttmp9C;_LL6F3: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBC7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBC6;if(_tmpBC7->tag != 0)goto _LL6F5;}_LL6F4:
# 4862
*elt_typ_dest=_tmpBC4;
return 1;_LL6F5:;_LL6F6:
 return 0;_LL6F2:;}_LL6F0:;_LL6F1:
# 4866
 return 0;_LL6ED:;}
# 4870
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_stmttmp9D=Cyc_Tcutil_compress(t);void*_tmpBC8=_stmttmp9D;void*_tmpBCA;union Cyc_Absyn_Constraint*_tmpBCB;_LL6F8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBC9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBC8;if(_tmpBC9->tag != 5)goto _LL6FA;else{_tmpBCA=(_tmpBC9->f1).elt_typ;_tmpBCB=((_tmpBC9->f1).ptr_atts).zero_term;}}_LL6F9:
# 4873
*elt_typ_dest=_tmpBCA;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBCB);_LL6FA:;_LL6FB:
 return 0;_LL6F7:;}
# 4881
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4883
void*_stmttmp9E=Cyc_Tcutil_compress(t);void*_tmpBCC=_stmttmp9E;void*_tmpBCE;union Cyc_Absyn_Constraint*_tmpBCF;union Cyc_Absyn_Constraint*_tmpBD0;void*_tmpBD2;struct Cyc_Absyn_Tqual _tmpBD3;struct Cyc_Absyn_Exp*_tmpBD4;union Cyc_Absyn_Constraint*_tmpBD5;_LL6FD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBCD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpBCC;if(_tmpBCD->tag != 5)goto _LL6FF;else{_tmpBCE=(_tmpBCD->f1).elt_typ;_tmpBCF=((_tmpBCD->f1).ptr_atts).bounds;_tmpBD0=((_tmpBCD->f1).ptr_atts).zero_term;}}_LL6FE:
# 4885
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD0)){
*ptr_type=t;
*elt_type=_tmpBCE;
{void*_stmttmp9F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpBCF);void*_tmpBD6=_stmttmp9F;_LL704: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpBD7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpBD6;if(_tmpBD7->tag != 0)goto _LL706;}_LL705:
*is_dyneither=1;goto _LL703;_LL706:;_LL707:
*is_dyneither=0;goto _LL703;_LL703:;}
# 4892
return 1;}else{
return 0;}_LL6FF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpBD1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpBCC;if(_tmpBD1->tag != 8)goto _LL701;else{_tmpBD2=(_tmpBD1->f1).elt_type;_tmpBD3=(_tmpBD1->f1).tq;_tmpBD4=(_tmpBD1->f1).num_elts;_tmpBD5=(_tmpBD1->f1).zero_term;}}_LL700:
# 4895
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBD5)){
*elt_type=_tmpBD2;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val);
return 1;}else{
return 0;}_LL701:;_LL702:
 return 0;_LL6FC:;}
# 4908
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4910
void*_stmttmpA0=e1->r;void*_tmpBD8=_stmttmpA0;struct Cyc_Absyn_Exp*_tmpBDB;struct Cyc_Absyn_Exp*_tmpBDD;struct Cyc_Absyn_Exp*_tmpBDF;struct Cyc_Absyn_Exp*_tmpBE1;struct Cyc_Absyn_Exp*_tmpBE3;struct Cyc_Absyn_Exp*_tmpBE5;_LL709: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBD9=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBD8;if(_tmpBD9->tag != 13)goto _LL70B;}_LL70A: {
# 4912
const char*_tmp12B5;void*_tmp12B4[1];struct Cyc_String_pa_PrintArg_struct _tmp12B3;(_tmp12B3.tag=0,((_tmp12B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12B4[0]=& _tmp12B3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B5="we have a cast in a lhs:  %s",_tag_dyneither(_tmp12B5,sizeof(char),29))),_tag_dyneither(_tmp12B4,sizeof(void*),1)))))));}_LL70B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpBDA=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBD8;if(_tmpBDA->tag != 19)goto _LL70D;else{_tmpBDB=_tmpBDA->f1;}}_LL70C:
 _tmpBDD=_tmpBDB;goto _LL70E;_LL70D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpBDC=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD8;if(_tmpBDC->tag != 22)goto _LL70F;else{_tmpBDD=_tmpBDC->f1;}}_LL70E:
# 4915
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBDD->topt),ptr_type,is_dyneither,elt_type);_LL70F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpBDE=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBD8;if(_tmpBDE->tag != 21)goto _LL711;else{_tmpBDF=_tmpBDE->f1;}}_LL710:
 _tmpBE1=_tmpBDF;goto _LL712;_LL711: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpBE0=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBD8;if(_tmpBE0->tag != 20)goto _LL713;else{_tmpBE1=_tmpBE0->f1;}}_LL712:
# 4919
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBE1->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12B9;void*_tmp12B8[1];struct Cyc_String_pa_PrintArg_struct _tmp12B7;(_tmp12B7.tag=0,((_tmp12B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12B8[0]=& _tmp12B7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12B9="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp12B9,sizeof(char),51))),_tag_dyneither(_tmp12B8,sizeof(void*),1)))))));}
return 0;_LL713: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpBE2=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBD8;if(_tmpBE2->tag != 12)goto _LL715;else{_tmpBE3=_tmpBE2->f1;}}_LL714:
 _tmpBE5=_tmpBE3;goto _LL716;_LL715: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpBE4=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBD8;if(_tmpBE4->tag != 11)goto _LL717;else{_tmpBE5=_tmpBE4->f1;}}_LL716:
# 4925
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpBE5->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp12BD;void*_tmp12BC[1];struct Cyc_String_pa_PrintArg_struct _tmp12BB;(_tmp12BB.tag=0,((_tmp12BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp12BC[0]=& _tmp12BB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12BD="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp12BD,sizeof(char),49))),_tag_dyneither(_tmp12BC,sizeof(void*),1)))))));}
return 0;_LL717: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBE6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBD8;if(_tmpBE6->tag != 1)goto _LL719;}_LL718:
 return 0;_LL719:;_LL71A: {
# 4931
const char*_tmp12C1;void*_tmp12C0[1];struct Cyc_String_pa_PrintArg_struct _tmp12BF;(_tmp12BF.tag=0,((_tmp12BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp12C0[0]=& _tmp12BF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12C1="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp12C1,sizeof(char),39))),_tag_dyneither(_tmp12C0,sizeof(void*),1)))))));}_LL708:;}
# 4935
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4946
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_stmttmpA1=Cyc_Tcutil_compress(r);void*_tmpBF3=_stmttmpA1;struct Cyc_Absyn_Tvar*_tmpBF7;_LL71C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpBF4=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpBF3;if(_tmpBF4->tag != 22)goto _LL71E;}_LL71D:
 return !must_be_unique;_LL71E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpBF5=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpBF3;if(_tmpBF5->tag != 21)goto _LL720;}_LL71F:
 return 1;_LL720: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBF6=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBF3;if(_tmpBF6->tag != 2)goto _LL722;else{_tmpBF7=_tmpBF6->f1;}}_LL721: {
# 4951
struct Cyc_Absyn_Kind*_stmttmpA2=Cyc_Tcutil_tvar_kind(_tmpBF7,& Cyc_Tcutil_rk);enum Cyc_Absyn_KindQual _tmpBF9;enum Cyc_Absyn_AliasQual _tmpBFA;struct Cyc_Absyn_Kind*_tmpBF8=_stmttmpA2;_tmpBF9=_tmpBF8->kind;_tmpBFA=_tmpBF8->aliasqual;
if(_tmpBF9 == Cyc_Absyn_RgnKind  && (_tmpBFA == Cyc_Absyn_Unique  || _tmpBFA == Cyc_Absyn_Top  && !must_be_unique)){
void*_stmttmpA3=Cyc_Absyn_compress_kb(_tmpBF7->kind);void*_tmpBFB=_stmttmpA3;struct Cyc_Core_Opt**_tmpBFD;_LL725: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBFC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBFB;if(_tmpBFC->tag != 2)goto _LL727;else{_tmpBFD=(struct Cyc_Core_Opt**)& _tmpBFC->f1;if((_tmpBFC->f2)->kind != Cyc_Absyn_RgnKind)goto _LL727;if((_tmpBFC->f2)->aliasqual != Cyc_Absyn_Top)goto _LL727;}}_LL726:
# 4955
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12C7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12C6;struct Cyc_Core_Opt*_tmp12C5;*_tmpBFD=((_tmp12C5=_cycalloc(sizeof(*_tmp12C5)),((_tmp12C5->v=(void*)((_tmp12C7=_cycalloc(sizeof(*_tmp12C7)),((_tmp12C7[0]=((_tmp12C6.tag=2,((_tmp12C6.f1=0,((_tmp12C6.f2=& Cyc_Tcutil_rk,_tmp12C6)))))),_tmp12C7)))),_tmp12C5))));}
return 0;_LL727:;_LL728:
 return 1;_LL724:;}
# 4960
return 0;}_LL722:;_LL723:
 return 0;_LL71B:;}
# 4967
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_stmttmpA4=Cyc_Tcutil_compress(t);void*_tmpC01=_stmttmpA4;void*_tmpC03;struct Cyc_Absyn_Tvar*_tmpC05;_LL72A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC02=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC01;if(_tmpC02->tag != 5)goto _LL72C;else{_tmpC03=((_tmpC02->f1).ptr_atts).rgn;}}_LL72B:
# 4970
 return Cyc_Tcutil_is_noalias_region(_tmpC03,must_be_unique);_LL72C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC04=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC01;if(_tmpC04->tag != 2)goto _LL72E;else{_tmpC05=_tmpC04->f1;}}_LL72D: {
# 4972
struct Cyc_Absyn_Kind*_stmttmpA5=Cyc_Tcutil_tvar_kind(_tmpC05,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmpC07;enum Cyc_Absyn_AliasQual _tmpC08;struct Cyc_Absyn_Kind*_tmpC06=_stmttmpA5;_tmpC07=_tmpC06->kind;_tmpC08=_tmpC06->aliasqual;
switch(_tmpC07){case Cyc_Absyn_BoxKind: _LL730:
 goto _LL731;case Cyc_Absyn_AnyKind: _LL731: goto _LL732;case Cyc_Absyn_MemKind: _LL732:
 if(_tmpC08 == Cyc_Absyn_Unique  || _tmpC08 == Cyc_Absyn_Top  && !must_be_unique){
void*_stmttmpA6=Cyc_Absyn_compress_kb(_tmpC05->kind);void*_tmpC09=_stmttmpA6;struct Cyc_Core_Opt**_tmpC0B;enum Cyc_Absyn_KindQual _tmpC0C;_LL735: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC0A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC09;if(_tmpC0A->tag != 2)goto _LL737;else{_tmpC0B=(struct Cyc_Core_Opt**)& _tmpC0A->f1;_tmpC0C=(_tmpC0A->f2)->kind;if((_tmpC0A->f2)->aliasqual != Cyc_Absyn_Top)goto _LL737;}}_LL736:
# 4978
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12D1;struct Cyc_Absyn_Kind*_tmp12D0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12CF;struct Cyc_Core_Opt*_tmp12CE;*_tmpC0B=((_tmp12CE=_cycalloc(sizeof(*_tmp12CE)),((_tmp12CE->v=(void*)((_tmp12D1=_cycalloc(sizeof(*_tmp12D1)),((_tmp12D1[0]=((_tmp12CF.tag=2,((_tmp12CF.f1=0,((_tmp12CF.f2=((_tmp12D0=_cycalloc_atomic(sizeof(*_tmp12D0)),((_tmp12D0->kind=_tmpC0C,((_tmp12D0->aliasqual=Cyc_Absyn_Aliasable,_tmp12D0)))))),_tmp12CF)))))),_tmp12D1)))),_tmp12CE))));}
return 0;_LL737:;_LL738:
# 4982
 return 1;_LL734:;}
# 4985
return 0;default: _LL733:
 return 0;}}_LL72E:;_LL72F:
# 4988
 return 0;_LL729:;}
# 4991
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpC11=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpC11,0))return 1;{
void*_tmpC12=_tmpC11;struct Cyc_List_List*_tmpC14;struct Cyc_Absyn_Aggrdecl**_tmpC16;struct Cyc_List_List*_tmpC17;struct Cyc_List_List*_tmpC19;union Cyc_Absyn_DatatypeInfoU _tmpC1C;struct Cyc_List_List*_tmpC1D;union Cyc_Absyn_DatatypeFieldInfoU _tmpC1F;struct Cyc_List_List*_tmpC20;_LL73B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC13=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC12;if(_tmpC13->tag != 10)goto _LL73D;else{_tmpC14=_tmpC13->f1;}}_LL73C:
# 4996
 while(_tmpC14 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpC14->hd)).f2))return 1;
_tmpC14=_tmpC14->tl;}
# 5000
return 0;_LL73D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC15=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC12;if(_tmpC15->tag != 11)goto _LL73F;else{if((((_tmpC15->f1).aggr_info).KnownAggr).tag != 2)goto _LL73F;_tmpC16=(struct Cyc_Absyn_Aggrdecl**)(((_tmpC15->f1).aggr_info).KnownAggr).val;_tmpC17=(_tmpC15->f1).targs;}}_LL73E:
# 5002
 if((*_tmpC16)->impl == 0)return 0;else{
# 5004
struct Cyc_List_List*_tmpC21=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpC16)->tvs,_tmpC17);
struct Cyc_List_List*_tmpC22=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpC16)->impl))->fields;
void*t;
while(_tmpC22 != 0){
t=_tmpC21 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpC22->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpC21,((struct Cyc_Absyn_Aggrfield*)_tmpC22->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpC22=_tmpC22->tl;}
# 5012
return 0;}_LL73F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC18=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC12;if(_tmpC18->tag != 12)goto _LL741;else{_tmpC19=_tmpC18->f2;}}_LL740:
# 5015
 while(_tmpC19 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpC19->hd)->type))return 1;
_tmpC19=_tmpC19->tl;}
# 5019
return 0;_LL741: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC1A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC12;if(_tmpC1A->tag != 11)goto _LL743;else{if((((_tmpC1A->f1).aggr_info).UnknownAggr).tag != 1)goto _LL743;}}_LL742:
# 5022
 return 0;_LL743: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpC1B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpC12;if(_tmpC1B->tag != 3)goto _LL745;else{_tmpC1C=(_tmpC1B->f1).datatype_info;_tmpC1D=(_tmpC1B->f1).targs;}}_LL744: {
# 5024
union Cyc_Absyn_DatatypeInfoU _tmpC23=_tmpC1C;struct _tuple2*_tmpC24;int _tmpC25;struct Cyc_List_List*_tmpC26;struct Cyc_Core_Opt*_tmpC27;_LL74A: if((_tmpC23.UnknownDatatype).tag != 1)goto _LL74C;_tmpC24=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC23.UnknownDatatype).val).name;_tmpC25=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC23.UnknownDatatype).val).is_extensible;_LL74B:
# 5027
 return 0;_LL74C: if((_tmpC23.KnownDatatype).tag != 2)goto _LL749;_tmpC26=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC23.KnownDatatype).val))->tvs;_tmpC27=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC23.KnownDatatype).val))->fields;_LL74D:
# 5030
 return 0;_LL749:;}_LL745: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpC1E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpC12;if(_tmpC1E->tag != 4)goto _LL747;else{_tmpC1F=(_tmpC1E->f1).field_info;_tmpC20=(_tmpC1E->f1).targs;}}_LL746: {
# 5033
union Cyc_Absyn_DatatypeFieldInfoU _tmpC28=_tmpC1F;struct Cyc_Absyn_Datatypedecl*_tmpC29;struct Cyc_Absyn_Datatypefield*_tmpC2A;_LL74F: if((_tmpC28.UnknownDatatypefield).tag != 1)goto _LL751;_LL750:
# 5036
 return 0;_LL751: if((_tmpC28.KnownDatatypefield).tag != 2)goto _LL74E;_tmpC29=((struct _tuple3)(_tmpC28.KnownDatatypefield).val).f1;_tmpC2A=((struct _tuple3)(_tmpC28.KnownDatatypefield).val).f2;_LL752: {
# 5038
struct Cyc_List_List*_tmpC2B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpC29->tvs,_tmpC20);
struct Cyc_List_List*_tmpC2C=_tmpC2A->typs;
while(_tmpC2C != 0){
_tmpC11=_tmpC2B == 0?(*((struct _tuple12*)_tmpC2C->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpC2B,(*((struct _tuple12*)_tmpC2C->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpC11))return 1;
_tmpC2C=_tmpC2C->tl;}
# 5045
return 0;}_LL74E:;}_LL747:;_LL748:
# 5047
 return 0;_LL73A:;};}
# 5051
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_stmttmpA7=Cyc_Tcutil_compress(t);void*_tmpC2D=_stmttmpA7;struct Cyc_Absyn_Aggrdecl*_tmpC2F;struct Cyc_List_List*_tmpC30;struct Cyc_List_List*_tmpC32;_LL754: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC2E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC2D;if(_tmpC2E->tag != 11)goto _LL756;else{if((((_tmpC2E->f1).aggr_info).KnownAggr).tag != 2)goto _LL756;_tmpC2F=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC2E->f1).aggr_info).KnownAggr).val);_tmpC30=(_tmpC2E->f1).targs;}}_LL755:
# 5054
 _tmpC32=_tmpC2F->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC2F->impl))->fields;goto _LL757;_LL756: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC31=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC2D;if(_tmpC31->tag != 12)goto _LL758;else{_tmpC32=_tmpC31->f2;}}_LL757: {
# 5056
struct Cyc_Absyn_Aggrfield*_tmpC33=Cyc_Absyn_lookup_field(_tmpC32,f);
{struct Cyc_Absyn_Aggrfield*_tmpC34=_tmpC33;void*_tmpC35;_LL75B: if(_tmpC34 != 0)goto _LL75D;_LL75C: {
const char*_tmp12D4;void*_tmp12D3;(_tmp12D3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D4="is_noalias_field: missing field",_tag_dyneither(_tmp12D4,sizeof(char),32))),_tag_dyneither(_tmp12D3,sizeof(void*),0)));}_LL75D: if(_tmpC34 == 0)goto _LL75A;_tmpC35=_tmpC34->type;_LL75E:
# 5060
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpC35);_LL75A:;}
# 5062
return 0;}_LL758:;_LL759: {
# 5064
const char*_tmp12D8;void*_tmp12D7[1];struct Cyc_String_pa_PrintArg_struct _tmp12D6;(_tmp12D6.tag=0,((_tmp12D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp12D7[0]=& _tmp12D6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12D8="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp12D8,sizeof(char),36))),_tag_dyneither(_tmp12D7,sizeof(void*),1)))))));}_LL753:;}
# 5072
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_stmttmpA8=e->r;void*_tmpC3B=_stmttmpA8;struct Cyc_Absyn_Exp*_tmpC3F;struct Cyc_Absyn_Exp*_tmpC41;struct Cyc_Absyn_Exp*_tmpC43;struct _dyneither_ptr*_tmpC44;struct Cyc_Absyn_Exp*_tmpC46;struct Cyc_Absyn_Exp*_tmpC47;struct Cyc_Absyn_Exp*_tmpC49;struct Cyc_Absyn_Exp*_tmpC4A;struct Cyc_Absyn_Exp*_tmpC4C;struct Cyc_Absyn_Exp*_tmpC4E;struct Cyc_Absyn_Stmt*_tmpC50;_LL760: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC3C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC3B;if(_tmpC3C->tag != 1)goto _LL762;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC3D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpC3C->f2);if(_tmpC3D->tag != 1)goto _LL762;}}_LL761:
 return 0;_LL762: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC3E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC3B;if(_tmpC3E->tag != 21)goto _LL764;else{_tmpC3F=_tmpC3E->f1;}}_LL763:
 _tmpC41=_tmpC3F;goto _LL765;_LL764: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC40=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC3B;if(_tmpC40->tag != 19)goto _LL766;else{_tmpC41=_tmpC40->f1;}}_LL765:
# 5077
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpC41->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpC41);_LL766: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC42=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC3B;if(_tmpC42->tag != 20)goto _LL768;else{_tmpC43=_tmpC42->f1;_tmpC44=_tmpC42->f2;}}_LL767:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC43);_LL768: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC45=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC3B;if(_tmpC45->tag != 22)goto _LL76A;else{_tmpC46=_tmpC45->f1;_tmpC47=_tmpC45->f2;}}_LL769: {
# 5081
void*_stmttmpAA=Cyc_Tcutil_compress((void*)_check_null(_tmpC46->topt));void*_tmpC51=_stmttmpAA;_LL775: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpC52=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpC51;if(_tmpC52->tag != 10)goto _LL777;}_LL776:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC46);_LL777:;_LL778:
 return 0;_LL774:;}_LL76A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC48=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC3B;if(_tmpC48->tag != 5)goto _LL76C;else{_tmpC49=_tmpC48->f2;_tmpC4A=_tmpC48->f3;}}_LL76B:
# 5086
 return Cyc_Tcutil_is_noalias_path(r,_tmpC49) && Cyc_Tcutil_is_noalias_path(r,_tmpC4A);_LL76C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC4B=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC3B;if(_tmpC4B->tag != 8)goto _LL76E;else{_tmpC4C=_tmpC4B->f2;}}_LL76D:
 _tmpC4E=_tmpC4C;goto _LL76F;_LL76E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC4D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC3B;if(_tmpC4D->tag != 13)goto _LL770;else{_tmpC4E=_tmpC4D->f2;}}_LL76F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC4E);_LL770: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpC4F=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC3B;if(_tmpC4F->tag != 36)goto _LL772;else{_tmpC50=_tmpC4F->f1;}}_LL771:
# 5090
 while(1){
void*_stmttmpA9=_tmpC50->r;void*_tmpC53=_stmttmpA9;struct Cyc_Absyn_Stmt*_tmpC55;struct Cyc_Absyn_Stmt*_tmpC56;struct Cyc_Absyn_Decl*_tmpC58;struct Cyc_Absyn_Stmt*_tmpC59;struct Cyc_Absyn_Exp*_tmpC5B;_LL77A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpC54=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpC53;if(_tmpC54->tag != 2)goto _LL77C;else{_tmpC55=_tmpC54->f1;_tmpC56=_tmpC54->f2;}}_LL77B:
 _tmpC50=_tmpC56;goto _LL779;_LL77C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpC57=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpC53;if(_tmpC57->tag != 12)goto _LL77E;else{_tmpC58=_tmpC57->f1;_tmpC59=_tmpC57->f2;}}_LL77D:
 _tmpC50=_tmpC59;goto _LL779;_LL77E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpC5A=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpC53;if(_tmpC5A->tag != 1)goto _LL780;else{_tmpC5B=_tmpC5A->f1;}}_LL77F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpC5B);_LL780:;_LL781: {
const char*_tmp12DB;void*_tmp12DA;(_tmp12DA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12DB="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp12DB,sizeof(char),40))),_tag_dyneither(_tmp12DA,sizeof(void*),0)));}_LL779:;}_LL772:;_LL773:
# 5098
 return 1;_LL75F:;}
# 5115 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5117
struct _tuple18 _tmp12DC;struct _tuple18 bogus_ans=(_tmp12DC.f1=0,((_tmp12DC.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12DC)));
void*_stmttmpAB=e->r;void*_tmpC5E=_stmttmpAB;struct _tuple2*_tmpC60;void*_tmpC61;struct Cyc_Absyn_Exp*_tmpC63;struct _dyneither_ptr*_tmpC64;int _tmpC65;struct Cyc_Absyn_Exp*_tmpC67;struct _dyneither_ptr*_tmpC68;int _tmpC69;struct Cyc_Absyn_Exp*_tmpC6B;struct Cyc_Absyn_Exp*_tmpC6D;struct Cyc_Absyn_Exp*_tmpC6E;_LL783: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC5F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC5E;if(_tmpC5F->tag != 1)goto _LL785;else{_tmpC60=_tmpC5F->f1;_tmpC61=(void*)_tmpC5F->f2;}}_LL784: {
# 5121
void*_tmpC6F=_tmpC61;struct Cyc_Absyn_Vardecl*_tmpC73;struct Cyc_Absyn_Vardecl*_tmpC75;struct Cyc_Absyn_Vardecl*_tmpC77;struct Cyc_Absyn_Vardecl*_tmpC79;_LL790: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpC70=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpC6F;if(_tmpC70->tag != 0)goto _LL792;}_LL791:
 goto _LL793;_LL792: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC71=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpC6F;if(_tmpC71->tag != 2)goto _LL794;}_LL793:
 return bogus_ans;_LL794: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpC72=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpC6F;if(_tmpC72->tag != 1)goto _LL796;else{_tmpC73=_tmpC72->f1;}}_LL795: {
# 5125
void*_stmttmpB4=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpC7A=_stmttmpB4;_LL79D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC7B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC7A;if(_tmpC7B->tag != 8)goto _LL79F;}_LL79E: {
# 5127
struct _tuple18 _tmp12DD;return(_tmp12DD.f1=1,((_tmp12DD.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12DD)));}_LL79F:;_LL7A0: {
struct _tuple18 _tmp12DE;return(_tmp12DE.f1=(_tmpC73->tq).real_const,((_tmp12DE.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp12DE)));}_LL79C:;}_LL796: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC74=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpC6F;if(_tmpC74->tag != 4)goto _LL798;else{_tmpC75=_tmpC74->f1;}}_LL797: {
# 5131
void*_stmttmpB3=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpC7E=_stmttmpB3;_LL7A2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC7F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC7E;if(_tmpC7F->tag != 8)goto _LL7A4;}_LL7A3: {
struct _tuple18 _tmp12DF;return(_tmp12DF.f1=1,((_tmp12DF.f2=(void*)_check_null(_tmpC75->rgn),_tmp12DF)));}_LL7A4:;_LL7A5:
# 5134
 _tmpC75->escapes=1;{
struct _tuple18 _tmp12E0;return(_tmp12E0.f1=(_tmpC75->tq).real_const,((_tmp12E0.f2=(void*)_check_null(_tmpC75->rgn),_tmp12E0)));};_LL7A1:;}_LL798: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpC76=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpC6F;if(_tmpC76->tag != 5)goto _LL79A;else{_tmpC77=_tmpC76->f1;}}_LL799:
# 5137
 _tmpC79=_tmpC77;goto _LL79B;_LL79A: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpC78=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpC6F;if(_tmpC78->tag != 3)goto _LL78F;else{_tmpC79=_tmpC78->f1;}}_LL79B:
# 5139
 _tmpC79->escapes=1;{
struct _tuple18 _tmp12E1;return(_tmp12E1.f1=(_tmpC79->tq).real_const,((_tmp12E1.f2=(void*)_check_null(_tmpC79->rgn),_tmp12E1)));};_LL78F:;}_LL785: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC62=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC5E;if(_tmpC62->tag != 20)goto _LL787;else{_tmpC63=_tmpC62->f1;_tmpC64=_tmpC62->f2;_tmpC65=_tmpC62->f3;}}_LL786:
# 5144
 if(_tmpC65)return bogus_ans;{
# 5147
void*_stmttmpB0=Cyc_Tcutil_compress((void*)_check_null(_tmpC63->topt));void*_tmpC83=_stmttmpB0;struct Cyc_List_List*_tmpC85;struct Cyc_Absyn_Aggrdecl*_tmpC87;_LL7A7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC84=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC83;if(_tmpC84->tag != 12)goto _LL7A9;else{_tmpC85=_tmpC84->f2;}}_LL7A8: {
# 5149
struct Cyc_Absyn_Aggrfield*_tmpC88=Cyc_Absyn_lookup_field(_tmpC85,_tmpC64);
if(_tmpC88 != 0  && _tmpC88->width == 0){
struct _tuple18 _stmttmpB2=Cyc_Tcutil_addressof_props(te,_tmpC63);int _tmpC8A;void*_tmpC8B;struct _tuple18 _tmpC89=_stmttmpB2;_tmpC8A=_tmpC89.f1;_tmpC8B=_tmpC89.f2;{
struct _tuple18 _tmp12E2;return(_tmp12E2.f1=(_tmpC88->tq).real_const  || _tmpC8A,((_tmp12E2.f2=_tmpC8B,_tmp12E2)));};}
# 5154
return bogus_ans;}_LL7A9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC86=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC83;if(_tmpC86->tag != 11)goto _LL7AB;else{if((((_tmpC86->f1).aggr_info).KnownAggr).tag != 2)goto _LL7AB;_tmpC87=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC86->f1).aggr_info).KnownAggr).val);}}_LL7AA: {
# 5156
struct Cyc_Absyn_Aggrfield*_tmpC8D=Cyc_Absyn_lookup_decl_field(_tmpC87,_tmpC64);
if(_tmpC8D != 0  && _tmpC8D->width == 0){
struct _tuple18 _stmttmpB1=Cyc_Tcutil_addressof_props(te,_tmpC63);int _tmpC8F;void*_tmpC90;struct _tuple18 _tmpC8E=_stmttmpB1;_tmpC8F=_tmpC8E.f1;_tmpC90=_tmpC8E.f2;{
struct _tuple18 _tmp12E3;return(_tmp12E3.f1=(_tmpC8D->tq).real_const  || _tmpC8F,((_tmp12E3.f2=_tmpC90,_tmp12E3)));};}
# 5161
return bogus_ans;}_LL7AB:;_LL7AC:
 return bogus_ans;_LL7A6:;};_LL787: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC66=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC5E;if(_tmpC66->tag != 21)goto _LL789;else{_tmpC67=_tmpC66->f1;_tmpC68=_tmpC66->f2;_tmpC69=_tmpC66->f3;}}_LL788:
# 5166
 if(_tmpC69)return bogus_ans;{
# 5170
void*_stmttmpAE=Cyc_Tcutil_compress((void*)_check_null(_tmpC67->topt));void*_tmpC92=_stmttmpAE;void*_tmpC94;void*_tmpC95;_LL7AE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC93=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC92;if(_tmpC93->tag != 5)goto _LL7B0;else{_tmpC94=(_tmpC93->f1).elt_typ;_tmpC95=((_tmpC93->f1).ptr_atts).rgn;}}_LL7AF: {
# 5172
struct Cyc_Absyn_Aggrfield*finfo;
{void*_stmttmpAF=Cyc_Tcutil_compress(_tmpC94);void*_tmpC96=_stmttmpAF;struct Cyc_List_List*_tmpC98;struct Cyc_Absyn_Aggrdecl*_tmpC9A;_LL7B3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpC97=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpC96;if(_tmpC97->tag != 12)goto _LL7B5;else{_tmpC98=_tmpC97->f2;}}_LL7B4:
# 5175
 finfo=Cyc_Absyn_lookup_field(_tmpC98,_tmpC68);goto _LL7B2;_LL7B5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpC99=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpC96;if(_tmpC99->tag != 11)goto _LL7B7;else{if((((_tmpC99->f1).aggr_info).KnownAggr).tag != 2)goto _LL7B7;_tmpC9A=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpC99->f1).aggr_info).KnownAggr).val);}}_LL7B6:
# 5177
 finfo=Cyc_Absyn_lookup_decl_field(_tmpC9A,_tmpC68);goto _LL7B2;_LL7B7:;_LL7B8:
 return bogus_ans;_LL7B2:;}
# 5180
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp12E4;return(_tmp12E4.f1=(finfo->tq).real_const,((_tmp12E4.f2=_tmpC95,_tmp12E4)));}
return bogus_ans;}_LL7B0:;_LL7B1:
 return bogus_ans;_LL7AD:;};_LL789: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC6A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC5E;if(_tmpC6A->tag != 19)goto _LL78B;else{_tmpC6B=_tmpC6A->f1;}}_LL78A: {
# 5187
void*_stmttmpAD=Cyc_Tcutil_compress((void*)_check_null(_tmpC6B->topt));void*_tmpC9C=_stmttmpAD;struct Cyc_Absyn_Tqual _tmpC9E;void*_tmpC9F;_LL7BA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC9D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC9C;if(_tmpC9D->tag != 5)goto _LL7BC;else{_tmpC9E=(_tmpC9D->f1).elt_tq;_tmpC9F=((_tmpC9D->f1).ptr_atts).rgn;}}_LL7BB: {
# 5189
struct _tuple18 _tmp12E5;return(_tmp12E5.f1=_tmpC9E.real_const,((_tmp12E5.f2=_tmpC9F,_tmp12E5)));}_LL7BC:;_LL7BD:
 return bogus_ans;_LL7B9:;}_LL78B: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC6C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC5E;if(_tmpC6C->tag != 22)goto _LL78D;else{_tmpC6D=_tmpC6C->f1;_tmpC6E=_tmpC6C->f2;}}_LL78C: {
# 5195
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpC6D->topt));
void*_tmpCA1=t;struct Cyc_List_List*_tmpCA3;struct Cyc_Absyn_Tqual _tmpCA5;void*_tmpCA6;struct Cyc_Absyn_Tqual _tmpCA8;_LL7BF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCA2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCA1;if(_tmpCA2->tag != 10)goto _LL7C1;else{_tmpCA3=_tmpCA2->f1;}}_LL7C0: {
# 5199
struct _tuple14 _stmttmpAC=Cyc_Evexp_eval_const_uint_exp(_tmpC6E);unsigned int _tmpCAA;int _tmpCAB;struct _tuple14 _tmpCA9=_stmttmpAC;_tmpCAA=_tmpCA9.f1;_tmpCAB=_tmpCA9.f2;
if(!_tmpCAB)
return bogus_ans;{
struct _tuple12*_tmpCAC=Cyc_Absyn_lookup_tuple_field(_tmpCA3,(int)_tmpCAA);
if(_tmpCAC != 0){
struct _tuple18 _tmp12E6;return(_tmp12E6.f1=((*_tmpCAC).f1).real_const,((_tmp12E6.f2=(Cyc_Tcutil_addressof_props(te,_tmpC6D)).f2,_tmp12E6)));}
return bogus_ans;};}_LL7C1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCA4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCA1;if(_tmpCA4->tag != 5)goto _LL7C3;else{_tmpCA5=(_tmpCA4->f1).elt_tq;_tmpCA6=((_tmpCA4->f1).ptr_atts).rgn;}}_LL7C2: {
# 5207
struct _tuple18 _tmp12E7;return(_tmp12E7.f1=_tmpCA5.real_const,((_tmp12E7.f2=_tmpCA6,_tmp12E7)));}_LL7C3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCA7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCA1;if(_tmpCA7->tag != 8)goto _LL7C5;else{_tmpCA8=(_tmpCA7->f1).tq;}}_LL7C4: {
# 5213
struct _tuple18 _tmp12E8;return(_tmp12E8.f1=_tmpCA8.real_const,((_tmp12E8.f2=(Cyc_Tcutil_addressof_props(te,_tmpC6D)).f2,_tmp12E8)));}_LL7C5:;_LL7C6:
 return bogus_ans;_LL7BE:;}_LL78D:;_LL78E:
# 5217
{const char*_tmp12EB;void*_tmp12EA;(_tmp12EA=0,Cyc_Tcutil_terr(e->loc,((_tmp12EB="unary & applied to non-lvalue",_tag_dyneither(_tmp12EB,sizeof(char),30))),_tag_dyneither(_tmp12EA,sizeof(void*),0)));}
return bogus_ans;_LL782:;}
# 5224
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_stmttmpB5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpCB3=_stmttmpB5;struct Cyc_Absyn_Exp*_tmpCB6;_LL7C8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCB4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCB3;if(_tmpCB4->tag != 0)goto _LL7CA;}_LL7C9:
 return;_LL7CA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCB5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCB3;if(_tmpCB5->tag != 1)goto _LL7C7;else{_tmpCB6=_tmpCB5->f1;}}_LL7CB: {
# 5229
struct _tuple14 _stmttmpB6=Cyc_Evexp_eval_const_uint_exp(_tmpCB6);unsigned int _tmpCB8;int _tmpCB9;struct _tuple14 _tmpCB7=_stmttmpB6;_tmpCB8=_tmpCB7.f1;_tmpCB9=_tmpCB7.f2;
if(_tmpCB9  && _tmpCB8 <= i){
const char*_tmp12F0;void*_tmp12EF[2];struct Cyc_Int_pa_PrintArg_struct _tmp12EE;struct Cyc_Int_pa_PrintArg_struct _tmp12ED;(_tmp12ED.tag=1,((_tmp12ED.f1=(unsigned long)((int)i),((_tmp12EE.tag=1,((_tmp12EE.f1=(unsigned long)((int)_tmpCB8),((_tmp12EF[0]=& _tmp12EE,((_tmp12EF[1]=& _tmp12ED,Cyc_Tcutil_terr(loc,((_tmp12F0="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp12F0,sizeof(char),39))),_tag_dyneither(_tmp12EF,sizeof(void*),2)))))))))))));}
return;}_LL7C7:;};}
# 5236
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5240
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_stmttmpB7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpCBE=_stmttmpB7;struct Cyc_Absyn_Exp*_tmpCC0;_LL7CD: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCBF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpCBE;if(_tmpCBF->tag != 1)goto _LL7CF;else{_tmpCC0=_tmpCBF->f1;}}_LL7CE: {
# 5243
struct _tuple14 _stmttmpB8=Cyc_Evexp_eval_const_uint_exp(_tmpCC0);unsigned int _tmpCC2;int _tmpCC3;struct _tuple14 _tmpCC1=_stmttmpB8;_tmpCC2=_tmpCC1.f1;_tmpCC3=_tmpCC1.f2;
return _tmpCC3  && _tmpCC2 == 1;}_LL7CF:;_LL7D0:
 return 0;_LL7CC:;}
# 5249
int Cyc_Tcutil_bits_only(void*t){
void*_stmttmpB9=Cyc_Tcutil_compress(t);void*_tmpCC4=_stmttmpB9;void*_tmpCCB;union Cyc_Absyn_Constraint*_tmpCCC;struct Cyc_List_List*_tmpCCE;struct Cyc_Absyn_Aggrdecl*_tmpCD1;struct Cyc_List_List*_tmpCD2;struct Cyc_List_List*_tmpCD4;_LL7D2: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpCC5=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpCC4;if(_tmpCC5->tag != 0)goto _LL7D4;}_LL7D3:
 goto _LL7D5;_LL7D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpCC6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpCC4;if(_tmpCC6->tag != 6)goto _LL7D6;}_LL7D5:
 goto _LL7D7;_LL7D6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpCC7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpCC4;if(_tmpCC7->tag != 7)goto _LL7D8;}_LL7D7:
 return 1;_LL7D8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpCC8=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpCC4;if(_tmpCC8->tag != 13)goto _LL7DA;}_LL7D9:
 goto _LL7DB;_LL7DA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpCC9=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpCC4;if(_tmpCC9->tag != 14)goto _LL7DC;}_LL7DB:
 return 0;_LL7DC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCCA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCC4;if(_tmpCCA->tag != 8)goto _LL7DE;else{_tmpCCB=(_tmpCCA->f1).elt_type;_tmpCCC=(_tmpCCA->f1).zero_term;}}_LL7DD:
# 5259
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCCC) && Cyc_Tcutil_bits_only(_tmpCCB);_LL7DE: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCCD=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCC4;if(_tmpCCD->tag != 10)goto _LL7E0;else{_tmpCCE=_tmpCCD->f1;}}_LL7DF:
# 5261
 for(0;_tmpCCE != 0;_tmpCCE=_tmpCCE->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpCCE->hd)).f2))return 0;}
return 1;_LL7E0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCCF=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCC4;if(_tmpCCF->tag != 11)goto _LL7E2;else{if((((_tmpCCF->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7E2;}}_LL7E1:
 return 0;_LL7E2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCD0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCC4;if(_tmpCD0->tag != 11)goto _LL7E4;else{if((((_tmpCD0->f1).aggr_info).KnownAggr).tag != 2)goto _LL7E4;_tmpCD1=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCD0->f1).aggr_info).KnownAggr).val);_tmpCD2=(_tmpCD0->f1).targs;}}_LL7E3:
# 5266
 if(_tmpCD1->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCD1->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpCD5=_new_region("rgn");struct _RegionHandle*rgn=& _tmpCD5;_push_region(rgn);
{struct Cyc_List_List*_tmpCD6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpCD1->tvs,_tmpCD2);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCD1->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpCD6,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpCD7=0;_npop_handler(0);return _tmpCD7;}}}{
int _tmpCD8=1;_npop_handler(0);return _tmpCD8;};}
# 5273
;_pop_region(rgn);};};_LL7E4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCD3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCC4;if(_tmpCD3->tag != 12)goto _LL7E6;else{_tmpCD4=_tmpCD3->f2;}}_LL7E5:
# 5279
 for(0;_tmpCD4 != 0;_tmpCD4=_tmpCD4->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpCD4->hd)->type))return 0;}
return 1;_LL7E6:;_LL7E7:
 return 0;_LL7D1:;}
# 5287
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_stmttmpBA=e->r;void*_tmpCD9=_stmttmpBA;struct Cyc_Absyn_Exp*_tmpCDC;struct Cyc_Absyn_Exp*_tmpCDE;_LL7E9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCDA=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCD9;if(_tmpCDA->tag != 1)goto _LL7EB;}_LL7EA:
 return 1;_LL7EB: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCDB=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCD9;if(_tmpCDB->tag != 11)goto _LL7ED;else{_tmpCDC=_tmpCDB->f1;}}_LL7EC:
 _tmpCDE=_tmpCDC;goto _LL7EE;_LL7ED: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpCDD=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCD9;if(_tmpCDD->tag != 12)goto _LL7EF;else{_tmpCDE=_tmpCDD->f1;}}_LL7EE:
 e=_tmpCDE;continue;_LL7EF:;_LL7F0:
# 5294
 return 0;_LL7E8:;}}
# 5304
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_stmttmpBB=e->r;void*_tmpCDF=_stmttmpBB;struct _tuple2*_tmpCE7;void*_tmpCE8;struct Cyc_Absyn_Exp*_tmpCEA;struct Cyc_Absyn_Exp*_tmpCEB;struct Cyc_Absyn_Exp*_tmpCEC;struct Cyc_Absyn_Exp*_tmpCEE;struct Cyc_Absyn_Exp*_tmpCEF;struct Cyc_Absyn_Exp*_tmpCF1;struct Cyc_Absyn_Exp*_tmpCF3;void*_tmpCF5;struct Cyc_Absyn_Exp*_tmpCF6;void*_tmpCF8;struct Cyc_Absyn_Exp*_tmpCF9;struct Cyc_Absyn_Exp*_tmpCFB;struct Cyc_Absyn_Exp*_tmpCFD;struct Cyc_Absyn_Exp*_tmpCFE;struct Cyc_Absyn_Exp*_tmpD00;struct Cyc_List_List*_tmpD02;struct Cyc_List_List*_tmpD04;struct Cyc_List_List*_tmpD06;enum Cyc_Absyn_Primop _tmpD08;struct Cyc_List_List*_tmpD09;struct Cyc_List_List*_tmpD0B;struct Cyc_List_List*_tmpD0D;_LL7F2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpCE0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCE0->tag != 0)goto _LL7F4;}_LL7F3:
 goto _LL7F5;_LL7F4: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCE1=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCE1->tag != 16)goto _LL7F6;}_LL7F5:
 goto _LL7F7;_LL7F6: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpCE2=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCE2->tag != 17)goto _LL7F8;}_LL7F7:
 goto _LL7F9;_LL7F8: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCE3=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCE3->tag != 18)goto _LL7FA;}_LL7F9:
 goto _LL7FB;_LL7FA: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpCE4=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCE4->tag != 31)goto _LL7FC;}_LL7FB:
 goto _LL7FD;_LL7FC: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpCE5=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCE5->tag != 32)goto _LL7FE;}_LL7FD:
 return 1;_LL7FE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCE6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCE6->tag != 1)goto _LL800;else{_tmpCE7=_tmpCE6->f1;_tmpCE8=(void*)_tmpCE6->f2;}}_LL7FF: {
# 5315
void*_tmpD0E=_tmpCE8;struct Cyc_Absyn_Vardecl*_tmpD11;struct Cyc_Absyn_Vardecl*_tmpD13;_LL821: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD0F=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD0E;if(_tmpD0F->tag != 2)goto _LL823;}_LL822:
 return 1;_LL823: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD10=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD0E;if(_tmpD10->tag != 1)goto _LL825;else{_tmpD11=_tmpD10->f1;}}_LL824: {
# 5318
void*_stmttmpBD=Cyc_Tcutil_compress(_tmpD11->type);void*_tmpD15=_stmttmpBD;_LL82C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD16=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD15;if(_tmpD16->tag != 8)goto _LL82E;}_LL82D:
 goto _LL82F;_LL82E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD17=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD15;if(_tmpD17->tag != 9)goto _LL830;}_LL82F:
 return 1;_LL830:;_LL831:
 return var_okay;_LL82B:;}_LL825: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD12=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD0E;if(_tmpD12->tag != 4)goto _LL827;else{_tmpD13=_tmpD12->f1;}}_LL826:
# 5325
 if(_tmpD13->sc == Cyc_Absyn_Static){
void*_stmttmpBC=Cyc_Tcutil_compress(_tmpD13->type);void*_tmpD18=_stmttmpBC;_LL833: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD19=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD18;if(_tmpD19->tag != 8)goto _LL835;}_LL834:
 goto _LL836;_LL835: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD1A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD18;if(_tmpD1A->tag != 9)goto _LL837;}_LL836:
 return 1;_LL837:;_LL838:
 return var_okay;_LL832:;}else{
# 5332
return var_okay;}_LL827: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD14=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD0E;if(_tmpD14->tag != 0)goto _LL829;}_LL828:
 return 0;_LL829:;_LL82A:
 return var_okay;_LL820:;}_LL800: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCE9=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCE9->tag != 5)goto _LL802;else{_tmpCEA=_tmpCE9->f1;_tmpCEB=_tmpCE9->f2;_tmpCEC=_tmpCE9->f3;}}_LL801:
# 5338
 return(Cyc_Tcutil_cnst_exp(0,_tmpCEA) && 
Cyc_Tcutil_cnst_exp(0,_tmpCEB)) && 
Cyc_Tcutil_cnst_exp(0,_tmpCEC);_LL802: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpCED=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCED->tag != 8)goto _LL804;else{_tmpCEE=_tmpCED->f1;_tmpCEF=_tmpCED->f2;}}_LL803:
# 5342
 return Cyc_Tcutil_cnst_exp(0,_tmpCEE) && Cyc_Tcutil_cnst_exp(0,_tmpCEF);_LL804: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCF0=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCF0->tag != 11)goto _LL806;else{_tmpCF1=_tmpCF0->f1;}}_LL805:
 _tmpCF3=_tmpCF1;goto _LL807;_LL806: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpCF2=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCF2->tag != 12)goto _LL808;else{_tmpCF3=_tmpCF2->f1;}}_LL807:
# 5345
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpCF3);_LL808: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCF4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCF4->tag != 13)goto _LL80A;else{_tmpCF5=(void*)_tmpCF4->f1;_tmpCF6=_tmpCF4->f2;if(_tmpCF4->f4 != Cyc_Absyn_No_coercion)goto _LL80A;}}_LL809:
# 5347
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpCF6);_LL80A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCF7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCF7->tag != 13)goto _LL80C;else{_tmpCF8=(void*)_tmpCF7->f1;_tmpCF9=_tmpCF7->f2;}}_LL80B:
# 5350
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpCF9);_LL80C: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpCFA=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCFA->tag != 14)goto _LL80E;else{_tmpCFB=_tmpCFA->f1;}}_LL80D:
# 5352
 return Cyc_Tcutil_cnst_exp(1,_tmpCFB);_LL80E: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCFC=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCFC->tag != 26)goto _LL810;else{_tmpCFD=_tmpCFC->f2;_tmpCFE=_tmpCFC->f3;}}_LL80F:
# 5354
 return Cyc_Tcutil_cnst_exp(0,_tmpCFD) && Cyc_Tcutil_cnst_exp(0,_tmpCFE);_LL810: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpCFF=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpCFF->tag != 27)goto _LL812;else{_tmpD00=_tmpCFF->f1;}}_LL811:
# 5356
 return Cyc_Tcutil_cnst_exp(0,_tmpD00);_LL812: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpD01=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpD01->tag != 25)goto _LL814;else{_tmpD02=_tmpD01->f1;}}_LL813:
 _tmpD04=_tmpD02;goto _LL815;_LL814: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD03=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpD03->tag != 29)goto _LL816;else{_tmpD04=_tmpD03->f2;}}_LL815:
 _tmpD06=_tmpD04;goto _LL817;_LL816: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpD05=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpD05->tag != 28)goto _LL818;else{_tmpD06=_tmpD05->f3;}}_LL817:
# 5360
 for(0;_tmpD06 != 0;_tmpD06=_tmpD06->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpD06->hd)).f2))
return 0;}
return 1;_LL818: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD07=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpD07->tag != 2)goto _LL81A;else{_tmpD08=_tmpD07->f1;_tmpD09=_tmpD07->f2;}}_LL819:
# 5365
 _tmpD0B=_tmpD09;goto _LL81B;_LL81A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpD0A=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpD0A->tag != 23)goto _LL81C;else{_tmpD0B=_tmpD0A->f1;}}_LL81B:
 _tmpD0D=_tmpD0B;goto _LL81D;_LL81C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpD0C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpCDF;if(_tmpD0C->tag != 30)goto _LL81E;else{_tmpD0D=_tmpD0C->f1;}}_LL81D:
# 5368
 for(0;_tmpD0D != 0;_tmpD0D=_tmpD0D->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpD0D->hd))
return 0;}
return 1;_LL81E:;_LL81F:
 return 0;_LL7F1:;}
# 5376
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5380
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5382
int Cyc_Tcutil_supports_default(void*t){
void*_stmttmpBE=Cyc_Tcutil_compress(t);void*_tmpD1B=_stmttmpBE;union Cyc_Absyn_Constraint*_tmpD20;union Cyc_Absyn_Constraint*_tmpD21;void*_tmpD23;struct Cyc_List_List*_tmpD25;union Cyc_Absyn_AggrInfoU _tmpD27;struct Cyc_List_List*_tmpD28;struct Cyc_List_List*_tmpD2A;_LL83A: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD1C=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD1B;if(_tmpD1C->tag != 0)goto _LL83C;}_LL83B:
 goto _LL83D;_LL83C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD1D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD1B;if(_tmpD1D->tag != 6)goto _LL83E;}_LL83D:
 goto _LL83F;_LL83E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD1E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD1B;if(_tmpD1E->tag != 7)goto _LL840;}_LL83F:
 return 1;_LL840: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD1F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1B;if(_tmpD1F->tag != 5)goto _LL842;else{_tmpD20=((_tmpD1F->f1).ptr_atts).nullable;_tmpD21=((_tmpD1F->f1).ptr_atts).bounds;}}_LL841: {
# 5389
void*_stmttmpBF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD21);void*_tmpD2D=_stmttmpBF;_LL851: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD2E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD2D;if(_tmpD2E->tag != 0)goto _LL853;}_LL852:
 return 1;_LL853:;_LL854:
# 5392
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpD20);_LL850:;}_LL842: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD22=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD1B;if(_tmpD22->tag != 8)goto _LL844;else{_tmpD23=(_tmpD22->f1).elt_type;}}_LL843:
# 5395
 return Cyc_Tcutil_supports_default(_tmpD23);_LL844: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD24=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD1B;if(_tmpD24->tag != 10)goto _LL846;else{_tmpD25=_tmpD24->f1;}}_LL845:
# 5397
 for(0;_tmpD25 != 0;_tmpD25=_tmpD25->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpD25->hd)).f2))return 0;}
return 1;_LL846: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD26=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD1B;if(_tmpD26->tag != 11)goto _LL848;else{_tmpD27=(_tmpD26->f1).aggr_info;_tmpD28=(_tmpD26->f1).targs;}}_LL847: {
# 5401
struct Cyc_Absyn_Aggrdecl*_tmpD2F=Cyc_Absyn_get_known_aggrdecl(_tmpD27);
if(_tmpD2F->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD2F->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpD2F->tvs,_tmpD28,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD2F->impl))->fields);}_LL848: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD29=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD1B;if(_tmpD29->tag != 12)goto _LL84A;else{_tmpD2A=_tmpD29->f2;}}_LL849:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpD2A);_LL84A: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD2B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD1B;if(_tmpD2B->tag != 14)goto _LL84C;}_LL84B:
# 5407
 goto _LL84D;_LL84C: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD2C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD1B;if(_tmpD2C->tag != 13)goto _LL84E;}_LL84D:
 return 1;_LL84E:;_LL84F:
# 5410
 return 0;_LL839:;}
# 5415
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpD30=t;struct Cyc_Absyn_Typedefdecl*_tmpD32;_LL856: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpD31=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpD30;if(_tmpD31->tag != 17)goto _LL858;else{_tmpD32=_tmpD31->f3;}}_LL857:
# 5418
 if(_tmpD32 != 0){
struct Cyc_Absyn_Tqual _tmpD33=_tmpD32->tq;
if(((_tmpD33.print_const  || _tmpD33.q_volatile) || _tmpD33.q_restrict) || _tmpD33.real_const){
# 5423
const char*_tmp12F4;void*_tmp12F3[1];struct Cyc_String_pa_PrintArg_struct _tmp12F2;(_tmp12F2.tag=0,((_tmp12F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp12F3[0]=& _tmp12F2,Cyc_Tcutil_warn(loc,((_tmp12F4="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp12F4,sizeof(char),44))),_tag_dyneither(_tmp12F3,sizeof(void*),1)))))));}}
# 5426
goto _LL855;_LL858:;_LL859:
 goto _LL855;_LL855:;}
# 5431
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5433
struct _RegionHandle _tmpD37=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD37;_push_region(rgn);{
struct Cyc_List_List*_tmpD38=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpD39=1;_npop_handler(0);return _tmpD39;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpD38,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpD3A=0;_npop_handler(0);return _tmpD3A;}}}{
# 5442
int _tmpD3B=1;_npop_handler(0);return _tmpD3B;};
# 5433
;_pop_region(rgn);}
# 5448
int Cyc_Tcutil_admits_zero(void*t){
void*_stmttmpC0=Cyc_Tcutil_compress(t);void*_tmpD3C=_stmttmpC0;union Cyc_Absyn_Constraint*_tmpD40;union Cyc_Absyn_Constraint*_tmpD41;_LL85B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD3D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD3C;if(_tmpD3D->tag != 6)goto _LL85D;}_LL85C:
 goto _LL85E;_LL85D: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD3E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD3C;if(_tmpD3E->tag != 7)goto _LL85F;}_LL85E:
 return 1;_LL85F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD3F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD3C;if(_tmpD3F->tag != 5)goto _LL861;else{_tmpD40=((_tmpD3F->f1).ptr_atts).nullable;_tmpD41=((_tmpD3F->f1).ptr_atts).bounds;}}_LL860: {
# 5453
void*_stmttmpC1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpD41);void*_tmpD42=_stmttmpC1;_LL864: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD43=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD42;if(_tmpD43->tag != 0)goto _LL866;}_LL865:
# 5457
 return 0;_LL866:;_LL867:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD40);_LL863:;}_LL861:;_LL862:
# 5460
 return 0;_LL85A:;}
# 5464
int Cyc_Tcutil_is_noreturn(void*t){
{void*_stmttmpC2=Cyc_Tcutil_compress(t);void*_tmpD44=_stmttmpC2;void*_tmpD46;struct Cyc_List_List*_tmpD48;_LL869: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD45=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD44;if(_tmpD45->tag != 5)goto _LL86B;else{_tmpD46=(_tmpD45->f1).elt_typ;}}_LL86A:
 return Cyc_Tcutil_is_noreturn(_tmpD46);_LL86B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD47=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD44;if(_tmpD47->tag != 9)goto _LL86D;else{_tmpD48=(_tmpD47->f1).attributes;}}_LL86C:
# 5468
 for(0;_tmpD48 != 0;_tmpD48=_tmpD48->tl){
void*_stmttmpC3=(void*)_tmpD48->hd;void*_tmpD49=_stmttmpC3;_LL870: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpD4A=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpD49;if(_tmpD4A->tag != 4)goto _LL872;}_LL871:
 return 1;_LL872:;_LL873:
 continue;_LL86F:;}
# 5473
goto _LL868;_LL86D:;_LL86E:
 goto _LL868;_LL868:;}
# 5476
return 0;}
# 5481
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_stmttmpC4=Cyc_Tcutil_compress(t);void*_tmpD4B=_stmttmpC4;struct Cyc_List_List**_tmpD4D;_LL875: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD4C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpD4B;if(_tmpD4C->tag != 9)goto _LL877;else{_tmpD4D=(struct Cyc_List_List**)&(_tmpD4C->f1).attributes;}}_LL876: {
# 5484
struct Cyc_List_List*_tmpD4E=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpD4D,(void*)atts->hd)){
struct Cyc_List_List*_tmp12F5;*_tmpD4D=((_tmp12F5=_cycalloc(sizeof(*_tmp12F5)),((_tmp12F5->hd=(void*)atts->hd,((_tmp12F5->tl=*_tmpD4D,_tmp12F5))))));}}else{
# 5491
struct Cyc_List_List*_tmp12F6;_tmpD4E=((_tmp12F6=_cycalloc(sizeof(*_tmp12F6)),((_tmp12F6->hd=(void*)atts->hd,((_tmp12F6->tl=_tmpD4E,_tmp12F6))))));}}
return _tmpD4E;}_LL877:;_LL878: {
const char*_tmp12F9;void*_tmp12F8;(_tmp12F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F9="transfer_fn_type_atts",_tag_dyneither(_tmp12F9,sizeof(char),22))),_tag_dyneither(_tmp12F8,sizeof(void*),0)));}_LL874:;}
# 5498
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_stmttmpC5=Cyc_Tcutil_compress(t);void*_tmpD53=_stmttmpC5;struct Cyc_Absyn_PtrInfo*_tmpD55;struct Cyc_Absyn_Exp*_tmpD57;_LL87A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD54=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD53;if(_tmpD54->tag != 5)goto _LL87C;else{_tmpD55=(struct Cyc_Absyn_PtrInfo*)& _tmpD54->f1;}}_LL87B:
# 5502
{void*_stmttmpC6=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpD55->ptr_atts).bounds);void*_tmpD58=_stmttmpC6;struct Cyc_Absyn_Exp*_tmpD5A;_LL881: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpD59=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpD58;if(_tmpD59->tag != 1)goto _LL883;else{_tmpD5A=_tmpD59->f1;}}_LL882:
 bound=_tmpD5A;goto _LL880;_LL883:;_LL884:
 goto _LL880;_LL880:;}
# 5506
goto _LL879;_LL87C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD56=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD53;if(_tmpD56->tag != 8)goto _LL87E;else{_tmpD57=(_tmpD56->f1).num_elts;}}_LL87D:
# 5508
 bound=_tmpD57;
goto _LL879;_LL87E:;_LL87F:
 goto _LL879;_LL879:;}
# 5512
return bound;}
# 5517
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpD5B=b;struct Cyc_Absyn_Vardecl*_tmpD5D;struct Cyc_Absyn_Vardecl*_tmpD5F;struct Cyc_Absyn_Vardecl*_tmpD61;struct Cyc_Absyn_Vardecl*_tmpD63;_LL886: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD5C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpD5B;if(_tmpD5C->tag != 5)goto _LL888;else{_tmpD5D=_tmpD5C->f1;}}_LL887:
 _tmpD5F=_tmpD5D;goto _LL889;_LL888: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD5E=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD5B;if(_tmpD5E->tag != 4)goto _LL88A;else{_tmpD5F=_tmpD5E->f1;}}_LL889:
 _tmpD61=_tmpD5F;goto _LL88B;_LL88A: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpD60=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpD5B;if(_tmpD60->tag != 3)goto _LL88C;else{_tmpD61=_tmpD60->f1;}}_LL88B:
 _tmpD63=_tmpD61;goto _LL88D;_LL88C: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD62=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD5B;if(_tmpD62->tag != 1)goto _LL88E;else{_tmpD63=_tmpD62->f1;}}_LL88D:
# 5523
 if(!_tmpD63->escapes)return _tmpD63;
goto _LL885;_LL88E:;_LL88F:
 goto _LL885;_LL885:;}
# 5527
return 0;}
# 5531
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpD64=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0){struct Cyc_List_List*_tmp12FA;_tmpD64=((_tmp12FA=_cycalloc(sizeof(*_tmp12FA)),((_tmp12FA->hd=*((void**)_check_null((void**)x->hd)),((_tmp12FA->tl=_tmpD64,_tmp12FA))))));}}}
return _tmpD64;}
# 5538
int Cyc_Tcutil_is_array(void*t){
# 5540
void*_stmttmpC7=Cyc_Tcutil_compress(t);void*_tmpD66=_stmttmpC7;_LL891: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD67=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD66;if(_tmpD67->tag != 8)goto _LL893;}_LL892:
# 5542
 return 1;_LL893:;_LL894:
 return 0;_LL890:;}
# 5547
void*Cyc_Tcutil_promote_array(void*t,void*rgn){
# 5549
void*_stmttmpC8=Cyc_Tcutil_compress(t);void*_tmpD68=_stmttmpC8;void*_tmpD6A;struct Cyc_Absyn_Tqual _tmpD6B;struct Cyc_Absyn_Exp*_tmpD6C;union Cyc_Absyn_Constraint*_tmpD6D;unsigned int _tmpD6E;_LL896: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD69=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD68;if(_tmpD69->tag != 8)goto _LL898;else{_tmpD6A=(_tmpD69->f1).elt_type;_tmpD6B=(_tmpD69->f1).tq;_tmpD6C=(_tmpD69->f1).num_elts;_tmpD6D=(_tmpD69->f1).zero_term;_tmpD6E=(_tmpD69->f1).zt_loc;}}_LL897: {
# 5552
void*b;
if(_tmpD6C == 0  || _tmpD6C->topt == 0)
# 5555
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5559
struct Cyc_Absyn_Exp*bnd=_tmpD6C;
void*_stmttmpC9=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpD6F=_stmttmpC9;void*_tmpD71;_LL89B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpD70=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpD6F;if(_tmpD70->tag != 19)goto _LL89D;else{_tmpD71=(void*)_tmpD70->f1;}}_LL89C:
# 5562
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp12FD;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12FC;b=(void*)((_tmp12FC=_cycalloc(sizeof(*_tmp12FC)),((_tmp12FC[0]=((_tmp12FD.tag=1,((_tmp12FD.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpD71,0),0,Cyc_Absyn_No_coercion,0),_tmp12FD)))),_tmp12FC))));}
goto _LL89A;_LL89D:;_LL89E:
# 5565
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1300;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12FF;b=(void*)((_tmp12FF=_cycalloc(sizeof(*_tmp12FF)),((_tmp12FF[0]=((_tmp1300.tag=1,((_tmp1300.f1=bnd,_tmp1300)))),_tmp12FF))));}else{
# 5568
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL89A:;}
# 5571
return Cyc_Absyn_atb_typ(_tmpD6A,rgn,_tmpD6B,b,_tmpD6D);}_LL898:;_LL899:
# 5574
 return t;_LL895:;}
