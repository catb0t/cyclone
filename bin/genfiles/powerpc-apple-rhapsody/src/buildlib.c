#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[192];
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
extern struct _xtunion_struct ADD_PREFIX(Null_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Null_Exception);
extern struct _xtunion_struct ADD_PREFIX(Array_bounds_struct);
extern struct _xtunion_struct * ADD_PREFIX(Array_bounds);
extern struct _xtunion_struct ADD_PREFIX(Match_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Match_Exception);
extern struct _xtunion_struct ADD_PREFIX(Bad_alloc_struct);
extern struct _xtunion_struct * ADD_PREFIX(Bad_alloc);

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
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct _dyneither_ptr Cstring_to_string(char*);struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stdout;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);int Cyc_fputc(int,
struct Cyc___cycFILE*);int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int remove(const char*);int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
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
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{
char*tag;};int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*
x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);struct
Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char
Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{char*tag;};struct Cyc_Iter_Iter Cyc_Set_make_iter(
struct _RegionHandle*rgn,struct Cyc_Set_Set*s);unsigned long Cyc_strlen(struct
_dyneither_ptr s);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*
s2);struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);struct _dyneither_ptr Cyc_substring(
struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);int Cyc_Hashtable_hash_stringptr(struct
_dyneither_ptr*p);struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Filename_chop_extension(struct
_dyneither_ptr);struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};void Cyc_Position_reset_position(
struct _dyneither_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Lex_struct{
int tag;};struct Cyc_Position_Parse_struct{int tag;};struct Cyc_Position_Elab_struct{
int tag;};struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];
struct Cyc_Position_Nocontext_struct{char*tag;};extern char Cyc_Arg_Bad[4];struct
Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];
struct Cyc_Arg_Error_struct{char*tag;};struct Cyc_Arg_Unit_spec_struct{int tag;void(*
f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct _dyneither_ptr,
struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{
int tag;int*f1;};struct Cyc_Arg_String_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*f1)(int);};struct
Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};void Cyc_Arg_usage(
struct Cyc_List_List*,struct _dyneither_ptr);void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr args);struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(
unsigned int n);struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);void
Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);void Cyc_Buffer_add_string(struct
Cyc_Buffer_t*,struct _dyneither_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
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
char*tag;};struct _tuple9{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};struct
_tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);enum Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc
 = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc  = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,
Cyc_Abstract_sc  = 6};struct Cyc_Declaration_spec;struct Cyc_Declarator;struct Cyc_Abstractdeclarator;
struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct
_union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;
struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*
val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple10{
struct Cyc_Position_Segment*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*
f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple10*val;};struct
_union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY4{int
tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{
int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*
val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple11{struct Cyc_List_List*
f1;int f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple11*val;};struct
_union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _tuple12{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY14{int tag;struct _tuple12*val;
};struct _union_YYSTYPE_YY15{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY16{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY17{int tag;
struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_Declaration_spec*val;};struct
_tuple13{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct
_union_YYSTYPE_YY20{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY21{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class
val;};struct _union_YYSTYPE_YY23{int tag;void*val;};struct _union_YYSTYPE_YY24{int
tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual
val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*
val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope
val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};
struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple14{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple7*val;};
struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple15{struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple15*val;};
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
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Dict_T;
struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*
t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{char*tag;};
extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};struct
_tuple16{void*f1;void*f2;};struct _tuple16*Cyc_Dict_rchoose(struct _RegionHandle*r,
struct Cyc_Dict_Dict d);struct _tuple16*Cyc_Dict_rchoose(struct _RegionHandle*,
struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tc_tc(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);typedef
struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;void*
abort();void exit(int);int system(const char*);struct Cyc_timespec{long tv_sec;int
tv_nsec;};struct Cyc_stat{int st_dev;unsigned int st_ino;unsigned short st_mode;
unsigned short st_nlink;unsigned int st_uid;unsigned int st_gid;int st_rdev;struct
Cyc_timespec st_atimespec;struct Cyc_timespec st_mtimespec;struct Cyc_timespec
st_ctimespec;long long st_size;long long st_blocks;unsigned int st_blksize;
unsigned int st_flags;unsigned int st_gen;int st_lspare;long long st_qspare[2];};int
mkdir(const char*pathname,unsigned short mode);struct Cyc_flock{long long l_start;
long long l_len;int l_pid;short l_type;short l_whence;};struct Cyc_Flock_struct{int
tag;struct Cyc_flock*f1;};struct Cyc_Long_struct{int tag;long f1;};int Cyc_open(const
char*,int,struct _dyneither_ptr);struct Cyc_option{struct _dyneither_ptr name;int
has_arg;int*flag;int val;};int chdir(const char*);int close(int);struct
_dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);int isspace(
int);int toupper(int);void Cyc_Lex_lex_init(int use_cyclone_keywords);extern char*
Ccomp;static int Cyc_do_setjmp=0;struct Cyc___cycFILE*Cyc_log_file=0;struct Cyc___cycFILE*
Cyc_cstubs_file=0;struct Cyc___cycFILE*Cyc_cycstubs_file=0;int Cyc_log(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_log(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap){if(Cyc_log_file == 0){{const char*_tmp3A2;void*_tmp3A1;(
_tmp3A1=0,Cyc_fprintf(Cyc_stderr,((_tmp3A2="Internal error: log file is NULL\n",
_tag_dyneither(_tmp3A2,sizeof(char),34))),_tag_dyneither(_tmp3A1,sizeof(void*),0)));}
exit(1);}{int _tmp2=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),
fmt,ap);Cyc_fflush((struct Cyc___cycFILE*)((struct Cyc___cycFILE*)_check_null(Cyc_log_file)));
return _tmp2;}}static struct _dyneither_ptr*Cyc_current_source=0;static struct Cyc_List_List*
Cyc_current_args=0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(
struct _dyneither_ptr*sptr);static void Cyc_add_target(struct _dyneither_ptr*sptr){
struct Cyc_Set_Set**_tmp3A3;Cyc_current_targets=((_tmp3A3=_cycalloc(sizeof(*
_tmp3A3)),((_tmp3A3[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr),_tmp3A3))));}struct _tuple17{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;}
;struct _tuple17*Cyc_line(struct Cyc_Lexing_lexbuf*);int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);int Cyc_token(struct Cyc_Lexing_lexbuf*);int
Cyc_string(struct Cyc_Lexing_lexbuf*);struct Cyc___cycFILE*Cyc_slurp_out=0;int Cyc_slurp(
struct Cyc_Lexing_lexbuf*);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm(
struct Cyc_Lexing_lexbuf*);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf*);struct _tuple18{struct _dyneither_ptr f1;struct
_dyneither_ptr*f2;};struct _tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);int
Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line=(struct _dyneither_ptr){(void*)0,(void*)0,(
void*)(0 + 0)};struct _tuple19{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
struct _tuple20{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct
_tuple20*Cyc_spec(struct Cyc_Lexing_lexbuf*);int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile=(struct _dyneither_ptr){(void*)0,(void*)
0,(void*)(0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=0;struct Cyc_List_List*
Cyc_current_symbols=0;struct Cyc_List_List*Cyc_current_cstubs=0;struct Cyc_List_List*
Cyc_current_cycstubs=0;struct Cyc_List_List*Cyc_current_hstubs=0;struct Cyc_List_List*
Cyc_current_omit_symbols=0;struct Cyc_List_List*Cyc_current_cpp=0;struct Cyc_Buffer_t*
Cyc_specbuf=0;int Cyc_braces_to_match=0;int Cyc_parens_to_match=0;int Cyc_numdef=0;
const int Cyc_lex_base[406]=(const int[406]){0,0,75,192,305,310,311,166,312,91,27,
384,28,523,637,715,283,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,
- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,223,12,- 2,216,20,26,28,33,23,48,70,54,64,
74,100,91,107,94,370,386,111,103,95,122,122,375,414,111,111,153,377,1024,1139,
525,176,210,228,214,216,217,242,498,1253,1368,- 9,654,- 10,224,245,508,1482,1597,
684,- 8,720,- 11,432,510,515,1674,1751,1828,1909,434,465,550,1984,249,249,249,248,
244,254,0,13,4,2065,5,628,2073,2138,660,49,467,6,2099,7,705,2161,2199,820,- 21,
1051,1056,261,314,242,251,258,250,271,281,274,275,278,288,295,282,- 19,292,296,
300,313,321,326,309,325,319,315,326,365,400,417,418,404,400,400,427,431,- 16,422,
421,415,436,433,449,427,449,453,441,445,436,436,- 18,444,438,442,453,464,447,449,
482,489,490,1,4,6,491,492,505,504,516,516,524,554,2,21,556,557,654,19,20,21,592,
555,554,585,592,594,23,650,651,- 13,600,603,658,659,660,622,623,678,679,686,636,
637,693,697,698,645,648,703,704,705,- 12,654,655,1030,- 20,1165,654,664,661,671,
670,665,668,698,700,698,712,1144,730,731,730,743,1258,1170,747,758,748,749,747,
760,1370,752,753,765,778,1375,- 7,- 8,8,1263,2231,9,996,2255,2293,1341,1279,- 49,
1150,- 2,817,- 4,818,997,1028,819,993,1023,1448,820,2320,2363,824,866,868,907,2433,
870,991,- 36,- 42,- 37,2508,- 28,909,- 40,- 25,911,- 27,- 45,- 39,- 48,2583,2612,1467,889,
979,1563,2622,2652,1582,2281,2685,2716,2754,1001,1090,2824,2862,1082,1092,1084,
1127,1148,1204,- 6,- 34,927,2794,- 47,- 30,- 32,- 46,- 29,- 31,- 33,962,2902,963,964,2128,
965,1002,1003,1014,1018,1020,1024,1032,1043,2975,3059,- 23,- 17,- 15,- 22,2239,1077,
- 24,- 41,- 38,- 35,- 26,1282,3141,4,3224,1076,15,1051,1055,1056,1058,1054,1071,1141};
const int Cyc_lex_backtrk[406]=(const int[406]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,
- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,
6,5,2,8,3,5,- 1,6,5,- 1,20,20,20,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,14,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,
4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,
43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,
- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
406]=(const int[406]){- 1,- 1,- 1,297,286,138,23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,
0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,103,- 1,- 1,
- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,-
1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,366,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,
- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,
0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[
3481]=(const int[3481]){0,0,0,0,0,0,0,0,0,0,22,19,28,398,19,28,19,28,100,19,45,45,
45,45,45,22,45,0,0,0,0,0,21,202,213,399,21,22,- 1,- 1,22,- 1,- 1,45,203,45,204,22,
396,396,396,396,396,396,396,396,396,396,31,103,22,214,114,40,227,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,31,245,240,235,396,130,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,394,394,394,394,394,394,
394,394,394,394,121,20,74,67,54,55,56,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,57,58,59,60,394,
61,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,62,63,37,298,299,298,298,299,39,22,64,65,68,69,70,
129,34,34,34,34,34,34,34,34,71,72,298,300,301,75,76,302,303,304,104,104,305,306,
104,307,308,309,310,311,311,311,311,311,311,311,311,311,312,77,313,314,315,104,
19,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,19,- 1,- 1,317,316,101,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,287,318,
37,288,139,139,24,24,139,131,122,94,84,81,85,38,82,86,28,87,24,29,83,25,289,88,
95,139,96,22,26,26,21,21,115,116,117,140,118,119,120,142,191,192,26,35,35,35,35,
35,35,35,35,177,171,162,30,30,30,30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,
150,78,151,152,153,154,66,66,290,155,66,157,158,66,132,123,141,39,73,22,78,159,
143,144,145,160,146,161,27,66,147,31,46,21,73,73,163,164,73,148,165,166,167,113,
113,113,113,113,113,113,113,113,113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
- 1,168,- 1,- 1,113,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,89,89,28,169,89,170,100,172,173,174,
97,97,104,104,97,175,104,112,112,176,178,112,179,89,180,105,105,80,80,105,19,80,
21,97,181,104,182,183,184,185,112,186,187,188,189,190,251,193,105,194,80,195,112,
112,91,196,112,197,198,21,21,21,106,107,106,106,106,106,106,106,106,106,106,106,
21,112,199,200,201,205,206,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,207,208,209,210,106,211,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,47,47,35,212,47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,
230,222,92,223,47,33,33,33,33,33,33,128,128,128,128,128,128,128,128,48,224,92,
225,- 1,226,- 1,228,229,99,99,231,49,99,232,233,234,93,33,33,33,33,33,33,35,35,35,
35,35,35,35,35,99,236,237,238,239,- 1,217,- 1,41,41,218,102,41,101,101,241,242,101,
243,219,50,220,244,91,246,51,52,247,248,249,250,41,53,252,253,268,101,137,137,
137,137,137,137,137,137,263,259,42,42,42,42,42,42,42,42,42,42,221,260,261,262,93,
36,28,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,264,100,265,266,42,267,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,269,270,43,271,272,102,281,33,33,33,33,33,33,33,33,33,33,
276,277,278,279,280,282,283,33,33,33,33,33,33,35,35,35,35,35,35,35,35,284,285,
392,391,365,356,331,44,44,44,44,44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,327,328,329,
326,44,321,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,323,324,325,28,330,359,100,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,388,254,155,138,
44,100,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,78,78,389,363,78,- 1,253,253,190,102,253,295,295,295,295,295,295,295,295,
319,250,364,100,78,229,390,46,273,273,253,384,273,255,255,- 1,360,255,254,176,79,
79,79,79,79,79,79,79,79,79,383,273,361,362,35,100,255,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,320,385,21,35,79,367,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,31,80,80,
274,400,80,267,267,256,401,267,402,393,275,393,393,403,404,257,35,31,31,36,258,
80,405,21,255,255,267,0,255,273,273,0,393,273,19,0,0,79,79,79,79,79,79,79,79,79,
79,255,35,31,31,36,273,36,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,0,0,0,355,79,36,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,256,355,89,272,272,274,0,272,296,
296,257,0,296,0,0,275,355,0,0,0,- 1,89,0,0,296,296,272,393,296,393,393,296,0,21,
31,0,0,90,90,90,90,90,90,90,90,90,90,296,355,21,393,0,0,0,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,0,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,91,92,92,
280,280,92,0,280,285,285,0,0,285,31,31,31,31,31,31,31,31,0,0,0,92,0,280,0,0,0,0,
285,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,90,90,90,0,0,0,0,0,0,0,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,0,0,0,0,90,0,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,
0,357,97,358,358,358,358,358,358,358,358,358,358,0,0,0,0,337,0,337,0,97,338,338,
338,338,338,338,338,338,338,338,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,
0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,36,99,99,0,0,99,338,338,338,338,338,338,338,338,338,338,0,0,0,0,
341,0,341,0,99,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,98,98,98,98,98,
98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,108,108,108,108,108,108,108,108,108,108,
108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,109,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,0,0,- 1,108,0,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,
111,0,0,0,0,0,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,0,0,0,0,109,0,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
113,113,113,113,113,113,113,113,113,113,0,0,0,0,0,0,0,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
28,0,0,124,113,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,0,0,28,0,0,133,125,125,125,125,125,125,
125,125,127,127,127,127,127,127,127,127,127,127,0,0,0,0,0,0,0,127,127,127,127,
127,127,0,0,0,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,31,0,0,- 1,0,385,0,0,
127,127,127,127,127,127,386,386,386,386,386,386,386,386,0,126,127,127,127,127,
127,127,127,127,127,127,31,0,0,0,0,0,0,127,127,127,127,127,127,136,136,136,136,
136,136,136,136,136,136,135,0,0,0,0,0,0,136,136,136,136,136,136,0,0,0,127,127,
127,127,127,127,19,0,0,291,0,0,136,136,136,136,136,136,136,136,136,136,0,136,136,
136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,0,385,292,292,292,292,292,
292,292,292,387,387,387,387,387,387,387,387,0,136,136,136,136,136,136,0,294,294,
294,294,294,294,294,294,294,294,0,0,0,0,0,0,0,294,294,294,294,294,294,0,0,28,342,
342,342,342,342,342,342,342,342,342,0,0,294,294,294,294,294,294,294,294,294,294,
293,294,294,294,294,294,294,294,294,294,294,294,294,0,0,332,0,343,343,343,343,
343,343,343,343,344,344,0,0,0,0,0,0,0,0,0,0,0,334,294,294,294,294,294,294,345,0,
0,0,0,0,0,0,0,346,0,0,347,332,0,333,333,333,333,333,333,333,333,333,333,334,0,0,
0,0,0,0,345,0,0,0,334,0,0,0,0,346,0,335,347,0,0,0,0,0,0,0,336,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,334,0,0,0,0,0,0,335,0,0,0,0,0,0,0,0,336,322,322,322,322,322,322,322,
322,322,322,0,0,0,0,0,0,0,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,0,0,0,0,322,0,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,0,0,0,0,0,0,0,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,0,0,0,0,322,0,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,339,339,339,339,339,339,339,
339,339,339,0,0,0,0,0,0,0,0,0,0,0,340,93,0,0,0,0,332,93,333,333,333,333,333,333,
333,333,333,333,338,338,338,338,338,338,338,338,338,338,0,334,0,0,340,93,0,0,335,
0,0,93,91,0,0,0,0,336,91,0,339,339,339,339,339,339,339,339,339,339,0,0,0,334,0,0,
0,0,0,0,335,340,93,0,91,0,0,0,93,336,91,0,0,342,342,342,342,342,342,342,342,342,
342,0,0,0,0,0,0,0,0,0,0,340,93,93,0,0,0,0,93,93,332,0,343,343,343,343,343,343,
343,343,344,344,0,0,0,0,0,0,0,0,0,0,0,334,0,93,0,0,0,0,353,93,0,0,0,0,0,0,332,
354,344,344,344,344,344,344,344,344,344,344,0,0,0,0,0,334,0,0,0,0,0,334,353,0,0,
0,0,0,351,0,0,354,0,0,0,0,0,352,0,0,358,358,358,358,358,358,358,358,358,358,0,0,
0,334,0,0,0,0,0,0,351,340,93,0,0,0,0,0,93,352,348,348,348,348,348,348,348,348,
348,348,0,0,0,0,0,0,0,348,348,348,348,348,348,340,93,0,0,0,0,0,93,0,0,0,0,0,0,0,
348,348,348,348,348,348,348,348,348,348,0,348,348,348,348,348,348,348,348,348,
348,348,348,0,0,0,368,0,349,0,0,369,0,0,0,0,0,350,0,0,370,370,370,370,370,370,
370,370,0,348,348,348,348,348,348,371,0,0,0,0,349,0,0,0,0,0,0,0,0,350,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,372,0,0,0,0,373,374,0,0,0,375,0,0,0,0,0,0,0,376,0,0,0,377,0,
378,0,379,0,380,381,381,381,381,381,381,381,381,381,381,0,0,0,0,0,0,0,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,0,0,0,0,0,0,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,382,0,0,0,0,0,0,0,0,381,381,
381,381,381,381,381,381,381,381,0,0,0,0,0,0,0,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,0,0,0,0,
0,0,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,22,0,0,395,0,0,0,394,394,394,394,394,394,394,394,394,
394,0,0,0,0,0,0,0,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,0,0,0,0,394,0,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,397,0,0,0,0,0,0,0,396,396,396,396,396,396,396,396,396,396,0,0,0,0,0,0,0,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,0,0,0,0,396,0,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0};const int Cyc_lex_check[3481]=(const int[3481]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,25,29,0,122,124,131,133,288,291,41,41,45,45,41,398,45,- 1,- 1,- 1,- 1,- 1,120,
201,212,0,395,10,12,40,10,12,40,41,202,45,203,20,1,1,1,1,1,1,1,1,1,1,38,48,121,
213,10,38,226,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,129,217,218,
219,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,
2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,
58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,61,62,7,3,3,3,3,3,
7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,30,70,71,3,3,3,74,75,3,3,3,47,47,3,3,
47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,50,85,27,86,17,27,50,17,4,87,94,5,95,5,
6,8,9,18,114,115,116,5,117,118,119,141,143,143,17,34,34,34,34,34,34,34,34,144,
145,146,27,27,27,27,27,27,27,27,65,65,147,148,65,72,72,77,77,72,149,77,150,151,
152,153,66,66,4,154,66,156,157,65,6,8,5,16,72,16,77,158,142,142,142,159,142,160,
17,66,142,27,161,7,73,73,162,163,73,142,164,165,166,11,11,11,11,11,11,11,11,11,
11,103,27,110,103,73,110,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,111,167,130,111,11,130,11,11,11,11,11,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,88,88,66,168,88,169,170,171,172,173,
96,96,104,104,96,174,104,105,105,175,177,105,178,88,179,13,13,80,80,13,73,80,16,
96,180,104,181,182,183,184,105,185,186,187,188,189,191,192,13,193,80,194,112,112,
4,195,112,196,197,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,112,198,199,200,
204,205,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,206,207,208,209,13,210,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,14,14,80,211,14,214,215,32,32,32,32,32,32,32,32,32,32,
92,92,220,221,92,222,14,32,32,32,32,32,32,125,125,125,125,125,125,125,125,14,223,
92,224,103,225,110,227,228,99,99,230,14,99,231,232,233,234,32,32,32,32,32,32,128,
128,128,128,128,128,128,128,99,235,236,237,238,111,216,130,15,15,216,239,15,101,
101,240,241,101,242,216,14,216,243,244,245,14,14,246,247,248,249,15,14,251,252,
256,101,134,134,134,134,134,134,134,134,257,258,15,15,15,15,15,15,15,15,15,15,
216,259,260,261,92,262,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,263,99,264,265,15,266,15,15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,269,15,270,271,101,274,33,33,33,
33,33,33,33,33,33,33,275,276,277,278,279,281,282,33,33,33,33,33,33,137,137,137,
137,137,137,137,137,283,284,300,302,305,309,312,42,42,42,42,42,42,42,42,42,42,14,
33,33,33,33,33,33,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,313,313,313,314,42,317,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,44,44,44,44,44,44,44,44,44,44,315,315,324,15,
327,357,335,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,366,368,369,371,44,335,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,78,78,303,306,78,304,253,253,372,373,253,292,292,
292,292,292,292,292,292,318,374,306,336,78,375,303,376,139,139,253,377,139,140,
140,304,307,140,253,378,78,78,78,78,78,78,78,78,78,78,379,139,307,307,345,336,
140,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,318,387,397,345,78,304,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,
78,78,78,78,78,78,78,78,78,79,79,139,399,79,267,267,140,400,267,401,298,139,298,
298,402,403,140,346,349,350,351,140,79,404,405,255,255,267,- 1,255,273,273,- 1,298,
273,267,- 1,- 1,79,79,79,79,79,79,79,79,79,79,255,346,349,350,351,273,352,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,- 1,- 1,- 1,
353,79,352,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,89,89,255,353,89,272,272,273,- 1,272,289,289,255,- 1,289,- 1,- 1,273,354,- 1,
- 1,- 1,304,89,- 1,- 1,296,296,272,393,296,393,393,289,- 1,289,272,- 1,- 1,89,89,89,89,
89,89,89,89,89,89,296,354,296,393,- 1,- 1,- 1,89,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,89,- 1,89,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,280,280,90,- 1,
280,285,285,- 1,- 1,285,295,295,295,295,295,295,295,295,- 1,- 1,- 1,90,- 1,280,- 1,- 1,-
1,- 1,285,- 1,- 1,280,- 1,- 1,- 1,- 1,285,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,97,97,- 1,308,97,308,308,308,308,308,308,308,308,308,308,- 1,- 1,
- 1,- 1,334,- 1,334,- 1,97,334,334,334,334,334,334,334,334,334,334,- 1,- 1,- 1,- 1,- 1,97,
97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,98,98,- 1,- 1,98,
337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,340,- 1,340,- 1,98,340,340,340,
340,340,340,340,340,340,340,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,- 1,-
1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,106,106,106,106,106,106,106,106,106,106,106,106,106,-
1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,- 1,- 1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,108,108,108,108,
108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
109,- 1,- 1,109,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,
109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,
- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,- 1,-
1,- 1,- 1,- 1,- 1,- 1,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,123,- 1,- 1,123,113,- 1,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,- 1,- 1,132,- 1,- 1,132,123,123,123,123,123,123,123,123,126,126,126,126,126,126,
126,126,126,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,126,126,126,126,126,- 1,- 1,- 1,132,132,
132,132,132,132,132,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,109,- 1,370,- 1,- 1,126,126,
126,126,126,126,370,370,370,370,370,370,370,370,- 1,123,127,127,127,127,127,127,
127,127,127,127,132,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,135,135,135,135,
135,135,135,135,135,135,132,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,- 1,- 1,- 1,
127,127,127,127,127,127,290,- 1,- 1,290,- 1,- 1,136,136,136,136,136,136,136,136,136,
136,- 1,135,135,135,135,135,135,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
386,290,290,290,290,290,290,290,290,386,386,386,386,386,386,386,386,- 1,136,136,
136,136,136,136,- 1,293,293,293,293,293,293,293,293,293,293,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
293,293,293,293,293,293,- 1,- 1,290,341,341,341,341,341,341,341,341,341,341,- 1,- 1,
294,294,294,294,294,294,294,294,294,294,290,293,293,293,293,293,293,294,294,294,
294,294,294,- 1,- 1,310,- 1,310,310,310,310,310,310,310,310,310,310,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,310,294,294,294,294,294,294,310,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,310,- 1,
- 1,310,311,- 1,311,311,311,311,311,311,311,311,311,311,310,- 1,- 1,- 1,- 1,- 1,- 1,310,
- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,310,- 1,311,310,- 1,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
311,316,316,316,316,316,316,316,316,316,316,- 1,- 1,- 1,- 1,- 1,- 1,- 1,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,- 1,- 1,- 1,- 1,316,- 1,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,322,322,322,322,322,322,322,
322,322,322,- 1,- 1,- 1,- 1,- 1,- 1,- 1,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,- 1,- 1,- 1,- 1,322,- 1,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,332,332,332,332,332,332,332,332,332,332,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,332,332,- 1,- 1,- 1,- 1,333,332,333,333,333,333,333,333,333,333,333,333,
338,338,338,338,338,338,338,338,338,338,- 1,333,- 1,- 1,332,332,- 1,- 1,333,- 1,- 1,332,
338,- 1,- 1,- 1,- 1,333,338,- 1,339,339,339,339,339,339,339,339,339,339,- 1,- 1,- 1,333,
- 1,- 1,- 1,- 1,- 1,- 1,333,339,339,- 1,338,- 1,- 1,- 1,339,333,338,- 1,- 1,342,342,342,342,
342,342,342,342,342,342,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,339,339,342,- 1,- 1,- 1,- 1,
339,342,343,- 1,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,343,- 1,342,- 1,- 1,- 1,- 1,343,342,- 1,- 1,- 1,- 1,- 1,- 1,344,343,344,344,344,344,
344,344,344,344,344,344,- 1,- 1,- 1,- 1,- 1,343,- 1,- 1,- 1,- 1,- 1,344,343,- 1,- 1,- 1,- 1,- 1,
344,- 1,- 1,343,- 1,- 1,- 1,- 1,- 1,344,- 1,- 1,358,358,358,358,358,358,358,358,358,358,-
1,- 1,- 1,344,- 1,- 1,- 1,- 1,- 1,- 1,344,358,358,- 1,- 1,- 1,- 1,- 1,358,344,347,347,347,347,
347,347,347,347,347,347,- 1,- 1,- 1,- 1,- 1,- 1,- 1,347,347,347,347,347,347,358,358,- 1,
- 1,- 1,- 1,- 1,358,- 1,- 1,- 1,- 1,- 1,- 1,- 1,348,348,348,348,348,348,348,348,348,348,- 1,
347,347,347,347,347,347,348,348,348,348,348,348,- 1,- 1,- 1,367,- 1,348,- 1,- 1,367,- 1,
- 1,- 1,- 1,- 1,348,- 1,- 1,367,367,367,367,367,367,367,367,- 1,348,348,348,348,348,348,
367,- 1,- 1,- 1,- 1,348,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,348,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,367,- 1,- 1,- 1,- 1,367,367,- 1,- 1,- 1,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,- 1,- 1,- 1,
367,- 1,367,- 1,367,- 1,367,380,380,380,380,380,380,380,380,380,380,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,
380,380,380,380,380,380,380,380,- 1,- 1,- 1,- 1,- 1,- 1,380,380,380,380,380,380,380,
380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,381,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,381,381,381,381,381,381,381,381,381,381,- 1,- 1,- 1,- 1,- 1,-
1,- 1,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,- 1,- 1,- 1,- 1,- 1,- 1,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,394,- 1,-
1,394,- 1,- 1,- 1,394,394,394,394,394,394,394,394,394,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,- 1,- 1,- 1,- 1,394,- 1,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,396,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,396,396,396,396,396,396,396,396,396,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,- 1,- 1,- 1,- 1,396,- 1,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_lex_engine(int
start_state,struct Cyc_Lexing_lexbuf*lbuf);int Cyc_lex_engine(int start_state,
struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;state=start_state;
if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=
- 1;}else{state=(- state)- 1;}while(1){base=Cyc_lex_base[
_check_known_subscript_notnull(406,state)];if(base < 0)return(- base)- 1;backtrk=
Cyc_lex_backtrk[_check_known_subscript_notnull(406,state)];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)return(- state)- 1;else{c=256;}}else{c=(int)*((char*)
_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}if(Cyc_lex_check[_check_known_subscript_notnull(3481,base + c)]
== state)state=Cyc_lex_trans[_check_known_subscript_notnull(3481,base + c)];else{
state=Cyc_lex_default[_check_known_subscript_notnull(406,state)];}if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action == - 1){struct Cyc_Lexing_Error_struct
_tmp3A9;const char*_tmp3A8;struct Cyc_Lexing_Error_struct*_tmp3A7;(int)_throw((
void*)((_tmp3A7=_cycalloc(sizeof(*_tmp3A7)),((_tmp3A7[0]=((_tmp3A9.tag=Cyc_Lexing_Error,((
_tmp3A9.f1=((_tmp3A8="empty token",_tag_dyneither(_tmp3A8,sizeof(char),12))),
_tmp3A9)))),_tmp3A7)))));}else{return lbuf->lex_last_action;}}else{if(c == 256)
lbuf->lex_eof_reached=0;}}}struct _tuple17*Cyc_line_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);struct _tuple17*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0:
_LL0: Cyc_macroname(lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((struct
Cyc_List_List*)_check_null(Cyc_current_args))->tl){struct Cyc_Set_Set**_tmp3AA;
Cyc_current_targets=((_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((_tmp3AA[0]=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(*((
struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dyneither_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),_tmp3AA))));}{struct
_tuple17*_tmp3AB;return(_tmp3AB=_cycalloc(sizeof(*_tmp3AB)),((_tmp3AB->f1=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),((_tmp3AB->f2=*((struct Cyc_Set_Set**)
_check_null(Cyc_current_targets)),_tmp3AB)))));}case 1: _LL1: return Cyc_line(lexbuf);
case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);return Cyc_line_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3B1;const char*_tmp3B0;struct
Cyc_Lexing_Error_struct*_tmp3AF;(int)_throw((void*)((_tmp3AF=_cycalloc(sizeof(*
_tmp3AF)),((_tmp3AF[0]=((_tmp3B1.tag=Cyc_Lexing_Error,((_tmp3B1.f1=((_tmp3B0="some action didn't return!",
_tag_dyneither(_tmp3B0,sizeof(char),27))),_tmp3B1)))),_tmp3AF)))));}}struct
_tuple17*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple17*Cyc_line(struct
Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}int Cyc_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL5:{struct _dyneither_ptr*_tmp3B2;Cyc_current_source=((_tmp3B2=_cycalloc(
sizeof(*_tmp3B2)),((_tmp3B2[0]=(struct _dyneither_ptr)Cyc_substring((struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp3B2))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp3B3;Cyc_current_targets=((_tmp3B3=_cycalloc(sizeof(*
_tmp3B3)),((_tmp3B3[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3B3))));}Cyc_token(
lexbuf);return 0;case 1: _LL6:{struct _dyneither_ptr*_tmp3B4;Cyc_current_source=((
_tmp3B4=_cycalloc(sizeof(*_tmp3B4)),((_tmp3B4[0]=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp3B4))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp3B5;Cyc_current_targets=((_tmp3B5=_cycalloc(sizeof(*
_tmp3B5)),((_tmp3B5[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3B5))));}Cyc_args(
lexbuf);return 0;case 2: _LL7:{struct _dyneither_ptr*_tmp3B6;Cyc_current_source=((
_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp3B6))));}Cyc_current_args=0;{struct Cyc_Set_Set**_tmp3B7;Cyc_current_targets=((
_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp3B7))));}Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3BD;
const char*_tmp3BC;struct Cyc_Lexing_Error_struct*_tmp3BB;(int)_throw((void*)((
_tmp3BB=_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB[0]=((_tmp3BD.tag=Cyc_Lexing_Error,((
_tmp3BD.f1=((_tmp3BC="some action didn't return!",_tag_dyneither(_tmp3BC,sizeof(
char),27))),_tmp3BD)))),_tmp3BB)))));}}int Cyc_macroname(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(
lexbuf,1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_args_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LLA: {struct _dyneither_ptr*_tmp3BE;struct
_dyneither_ptr*_tmp15=(_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE[0]=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
2)),_tmp3BE)));{struct Cyc_List_List*_tmp3BF;Cyc_current_args=((_tmp3BF=_cycalloc(
sizeof(*_tmp3BF)),((_tmp3BF->hd=_tmp15,((_tmp3BF->tl=Cyc_current_args,_tmp3BF))))));}
return Cyc_args(lexbuf);}case 1: _LLB: {struct _dyneither_ptr*_tmp3C0;struct
_dyneither_ptr*_tmp18=(_tmp3C0=_cycalloc(sizeof(*_tmp3C0)),((_tmp3C0[0]=(struct
_dyneither_ptr)Cyc_substring((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(
unsigned long)((Cyc_Lexing_lexeme_end(lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 
1)),_tmp3C0)));{struct Cyc_List_List*_tmp3C1;Cyc_current_args=((_tmp3C1=_cycalloc(
sizeof(*_tmp3C1)),((_tmp3C1->hd=_tmp18,((_tmp3C1->tl=Cyc_current_args,_tmp3C1))))));}
return Cyc_token(lexbuf);}default: _LLC:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3C7;const char*_tmp3C6;struct
Cyc_Lexing_Error_struct*_tmp3C5;(int)_throw((void*)((_tmp3C5=_cycalloc(sizeof(*
_tmp3C5)),((_tmp3C5[0]=((_tmp3C7.tag=Cyc_Lexing_Error,((_tmp3C7.f1=((_tmp3C6="some action didn't return!",
_tag_dyneither(_tmp3C6,sizeof(char),27))),_tmp3C7)))),_tmp3C5)))));}}int Cyc_args(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate);int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLE:{struct
_dyneither_ptr*_tmp3C8;Cyc_add_target(((_tmp3C8=_cycalloc(sizeof(*_tmp3C8)),((
_tmp3C8[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp3C8)))));}return
Cyc_token(lexbuf);case 1: _LLF: return 0;case 2: _LL10: return Cyc_token(lexbuf);case 3:
_LL11: Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL12: return Cyc_token(
lexbuf);case 5: _LL13: return Cyc_token(lexbuf);case 6: _LL14: return Cyc_token(lexbuf);
case 7: _LL15: return Cyc_token(lexbuf);case 8: _LL16: return Cyc_token(lexbuf);case 9:
_LL17: return Cyc_token(lexbuf);case 10: _LL18: return Cyc_token(lexbuf);case 11: _LL19:
return Cyc_token(lexbuf);case 12: _LL1A: return Cyc_token(lexbuf);case 13: _LL1B: return
Cyc_token(lexbuf);case 14: _LL1C: return Cyc_token(lexbuf);case 15: _LL1D: return Cyc_token(
lexbuf);case 16: _LL1E: return Cyc_token(lexbuf);case 17: _LL1F: return Cyc_token(lexbuf);
case 18: _LL20: return Cyc_token(lexbuf);case 19: _LL21: return Cyc_token(lexbuf);case 20:
_LL22: return Cyc_token(lexbuf);case 21: _LL23: return Cyc_token(lexbuf);case 22: _LL24:
return Cyc_token(lexbuf);case 23: _LL25: return Cyc_token(lexbuf);case 24: _LL26: return
Cyc_token(lexbuf);case 25: _LL27: return Cyc_token(lexbuf);case 26: _LL28: return Cyc_token(
lexbuf);case 27: _LL29: return Cyc_token(lexbuf);case 28: _LL2A: return Cyc_token(lexbuf);
case 29: _LL2B: return Cyc_token(lexbuf);case 30: _LL2C: return Cyc_token(lexbuf);case 31:
_LL2D: return Cyc_token(lexbuf);case 32: _LL2E: return Cyc_token(lexbuf);case 33: _LL2F:
return Cyc_token(lexbuf);case 34: _LL30: return Cyc_token(lexbuf);case 35: _LL31: return
Cyc_token(lexbuf);case 36: _LL32: return Cyc_token(lexbuf);case 37: _LL33: return Cyc_token(
lexbuf);case 38: _LL34: return Cyc_token(lexbuf);case 39: _LL35: return Cyc_token(lexbuf);
case 40: _LL36: return Cyc_token(lexbuf);case 41: _LL37: return Cyc_token(lexbuf);case 42:
_LL38: return Cyc_token(lexbuf);case 43: _LL39: return Cyc_token(lexbuf);case 44: _LL3A:
return Cyc_token(lexbuf);case 45: _LL3B: return Cyc_token(lexbuf);case 46: _LL3C: return
Cyc_token(lexbuf);case 47: _LL3D: return Cyc_token(lexbuf);case 48: _LL3E: return Cyc_token(
lexbuf);default: _LL3F:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp3CE;const char*_tmp3CD;struct Cyc_Lexing_Error_struct*
_tmp3CC;(int)_throw((void*)((_tmp3CC=_cycalloc(sizeof(*_tmp3CC)),((_tmp3CC[0]=((
_tmp3CE.tag=Cyc_Lexing_Error,((_tmp3CE.f1=((_tmp3CD="some action didn't return!",
_tag_dyneither(_tmp3CD,sizeof(char),27))),_tmp3CE)))),_tmp3CC)))));}}int Cyc_token(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_token_rec(lexbuf,3);}int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate);int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL41: return Cyc_string(
lexbuf);case 1: _LL42: return 0;case 2: _LL43: return Cyc_string(lexbuf);case 3: _LL44:
return Cyc_string(lexbuf);case 4: _LL45: return Cyc_string(lexbuf);case 5: _LL46: return
Cyc_string(lexbuf);case 6: _LL47: return Cyc_string(lexbuf);case 7: _LL48: return 0;case
8: _LL49: return 0;case 9: _LL4A: return Cyc_string(lexbuf);default: _LL4B:(lexbuf->refill_buff)(
lexbuf);return Cyc_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3D4;const char*_tmp3D3;struct Cyc_Lexing_Error_struct*_tmp3D2;(int)_throw((
void*)((_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2[0]=((_tmp3D4.tag=Cyc_Lexing_Error,((
_tmp3D4.f1=((_tmp3D3="some action didn't return!",_tag_dyneither(_tmp3D3,sizeof(
char),27))),_tmp3D4)))),_tmp3D2)))));}}int Cyc_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(
lexbuf,4);}int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_slurp_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL4D: return 0;case 1: _LL4E: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2: _LL4F: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3D7;void*_tmp3D6;(
_tmp3D6=0,Cyc_log(((_tmp3D7="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp3D7,sizeof(char),44))),_tag_dyneither(_tmp3D6,sizeof(void*),0)));}
return 1;case 3: _LL50: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3DA;void*_tmp3D9;(
_tmp3D9=0,Cyc_log(((_tmp3DA="Warning: declaration of malloc sidestepped\n",
_tag_dyneither(_tmp3DA,sizeof(char),44))),_tag_dyneither(_tmp3D9,sizeof(void*),0)));}
return 1;case 4: _LL51: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3DD;void*_tmp3DC;(
_tmp3DC=0,Cyc_log(((_tmp3DD="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp3DD,sizeof(char),44))),_tag_dyneither(_tmp3DC,sizeof(void*),0)));}
return 1;case 5: _LL52: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_CALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3E0;void*_tmp3DF;(
_tmp3DF=0,Cyc_log(((_tmp3E0="Warning: declaration of calloc sidestepped\n",
_tag_dyneither(_tmp3E0,sizeof(char),44))),_tag_dyneither(_tmp3DF,sizeof(void*),0)));}
return 1;case 6: _LL53: Cyc_fputs((const char*)"__region",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp3E3;void*_tmp3E2;(_tmp3E2=0,Cyc_log(((
_tmp3E3="Warning: use of region sidestepped\n",_tag_dyneither(_tmp3E3,sizeof(
char),36))),_tag_dyneither(_tmp3E2,sizeof(void*),0)));}return 1;case 7: _LL54:
return 1;case 8: _LL55: return 1;case 9: _LL56: return 1;case 10: _LL57: return 1;case 11:
_LL58: return 1;case 12: _LL59: return 1;case 13: _LL5A: Cyc_fputs((const char*)"inline",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 14: _LL5B: Cyc_fputs((
const char*)"inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 15: _LL5C: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 16: _LL5D: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 17: _LL5E: Cyc_fputs((const char*)"int",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18: _LL5F: return 1;
case 19: _LL60: Cyc_parens_to_match=1;while(Cyc_asm(lexbuf)){;}Cyc_fputs((const char*)"0",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp3E6;void*_tmp3E5;(
_tmp3E5=0,Cyc_log(((_tmp3E6="Warning: replacing use of __asm__ with 0\n",
_tag_dyneither(_tmp3E6,sizeof(char),42))),_tag_dyneither(_tmp3E5,sizeof(void*),0)));}
return 1;case 20: _LL61: Cyc_fputc((int)Cyc_Lexing_lexeme_char(lexbuf,0),(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;default: _LL62:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3EC;
const char*_tmp3EB;struct Cyc_Lexing_Error_struct*_tmp3EA;(int)_throw((void*)((
_tmp3EA=_cycalloc(sizeof(*_tmp3EA)),((_tmp3EA[0]=((_tmp3EC.tag=Cyc_Lexing_Error,((
_tmp3EC.f1=((_tmp3EB="some action didn't return!",_tag_dyneither(_tmp3EB,sizeof(
char),27))),_tmp3EC)))),_tmp3EA)))));}}int Cyc_slurp(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,
5);}int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_slurp_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL64: return 0;case 1: _LL65: Cyc_fputc((int)'"',(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2: _LL66:{const char*
_tmp3EF;void*_tmp3EE;(_tmp3EE=0,Cyc_log(((_tmp3EF="Warning: unclosed string\n",
_tag_dyneither(_tmp3EF,sizeof(char),26))),_tag_dyneither(_tmp3EE,sizeof(void*),0)));}{
const char*_tmp3F3;void*_tmp3F2[1];struct Cyc_String_pa_struct _tmp3F1;(_tmp3F1.tag=
0,((_tmp3F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp3F2[0]=& _tmp3F1,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp3F3="%s",_tag_dyneither(_tmp3F3,sizeof(char),3))),_tag_dyneither(_tmp3F2,
sizeof(void*),1)))))));}return 1;case 3: _LL67:{const char*_tmp3F7;void*_tmp3F6[1];
struct Cyc_String_pa_struct _tmp3F5;(_tmp3F5.tag=0,((_tmp3F5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp3F6[0]=&
_tmp3F5,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp3F7="%s",
_tag_dyneither(_tmp3F7,sizeof(char),3))),_tag_dyneither(_tmp3F6,sizeof(void*),1)))))));}
return 1;case 4: _LL68:{const char*_tmp3FB;void*_tmp3FA[1];struct Cyc_String_pa_struct
_tmp3F9;(_tmp3F9.tag=0,((_tmp3F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp3FA[0]=& _tmp3F9,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp3FB="%s",_tag_dyneither(_tmp3FB,sizeof(char),3))),
_tag_dyneither(_tmp3FA,sizeof(void*),1)))))));}return 1;case 5: _LL69:{const char*
_tmp3FF;void*_tmp3FE[1];struct Cyc_String_pa_struct _tmp3FD;(_tmp3FD.tag=0,((
_tmp3FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp3FE[0]=& _tmp3FD,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp3FF="%s",_tag_dyneither(_tmp3FF,sizeof(char),3))),_tag_dyneither(_tmp3FE,
sizeof(void*),1)))))));}return 1;case 6: _LL6A:{const char*_tmp403;void*_tmp402[1];
struct Cyc_String_pa_struct _tmp401;(_tmp401.tag=0,((_tmp401.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp402[0]=&
_tmp401,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp403="%s",
_tag_dyneither(_tmp403,sizeof(char),3))),_tag_dyneither(_tmp402,sizeof(void*),1)))))));}
return 1;case 7: _LL6B:{const char*_tmp407;void*_tmp406[1];struct Cyc_String_pa_struct
_tmp405;(_tmp405.tag=0,((_tmp405.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Lexing_lexeme(lexbuf)),((_tmp406[0]=& _tmp405,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out),((_tmp407="%s",_tag_dyneither(_tmp407,sizeof(char),3))),
_tag_dyneither(_tmp406,sizeof(void*),1)))))));}return 1;case 8: _LL6C:{const char*
_tmp40B;void*_tmp40A[1];struct Cyc_String_pa_struct _tmp409;(_tmp409.tag=0,((
_tmp409.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf)),((
_tmp40A[0]=& _tmp409,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp40B="%s",_tag_dyneither(_tmp40B,sizeof(char),3))),_tag_dyneither(_tmp40A,
sizeof(void*),1)))))));}return 1;default: _LL6D:(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp411;const char*_tmp410;struct Cyc_Lexing_Error_struct*_tmp40F;(int)_throw((
void*)((_tmp40F=_cycalloc(sizeof(*_tmp40F)),((_tmp40F[0]=((_tmp411.tag=Cyc_Lexing_Error,((
_tmp411.f1=((_tmp410="some action didn't return!",_tag_dyneither(_tmp410,sizeof(
char),27))),_tmp411)))),_tmp40F)))));}}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_asm_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL6F: return 0;case 1: _LL70: if(Cyc_parens_to_match
== 1)return 0;-- Cyc_parens_to_match;return 1;case 2: _LL71: ++ Cyc_parens_to_match;
return 1;case 3: _LL72: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL73: while(
Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL74: return 1;case 6: _LL75: return 1;
default: _LL76:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp417;const char*_tmp416;struct Cyc_Lexing_Error_struct*
_tmp415;(int)_throw((void*)((_tmp415=_cycalloc(sizeof(*_tmp415)),((_tmp415[0]=((
_tmp417.tag=Cyc_Lexing_Error,((_tmp417.f1=((_tmp416="some action didn't return!",
_tag_dyneither(_tmp416,sizeof(char),27))),_tmp417)))),_tmp415)))));}}int Cyc_asm(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_asm(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL78:{
const char*_tmp41A;void*_tmp419;(_tmp419=0,Cyc_log(((_tmp41A="Warning: unclosed string\n",
_tag_dyneither(_tmp41A,sizeof(char),26))),_tag_dyneither(_tmp419,sizeof(void*),0)));}
return 0;case 1: _LL79: return 0;case 2: _LL7A:{const char*_tmp41D;void*_tmp41C;(_tmp41C=
0,Cyc_log(((_tmp41D="Warning: unclosed string\n",_tag_dyneither(_tmp41D,sizeof(
char),26))),_tag_dyneither(_tmp41C,sizeof(void*),0)));}return 1;case 3: _LL7B:
return 1;case 4: _LL7C: return 1;case 5: _LL7D: return 1;case 6: _LL7E: return 1;case 7: _LL7F:
return 1;case 8: _LL80: return 1;default: _LL81:(lexbuf->refill_buff)(lexbuf);return
Cyc_asm_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp423;const
char*_tmp422;struct Cyc_Lexing_Error_struct*_tmp421;(int)_throw((void*)((_tmp421=
_cycalloc(sizeof(*_tmp421)),((_tmp421[0]=((_tmp423.tag=Cyc_Lexing_Error,((
_tmp423.f1=((_tmp422="some action didn't return!",_tag_dyneither(_tmp422,sizeof(
char),27))),_tmp423)))),_tmp421)))));}}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(
lexbuf,8);}int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL83:{const char*_tmp426;void*_tmp425;(
_tmp425=0,Cyc_log(((_tmp426="Warning: unclosed comment\n",_tag_dyneither(_tmp426,
sizeof(char),27))),_tag_dyneither(_tmp425,sizeof(void*),0)));}return 0;case 1:
_LL84: return 0;case 2: _LL85: return 1;default: _LL86:(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp42C;const char*_tmp42B;struct Cyc_Lexing_Error_struct*_tmp42A;(int)_throw((
void*)((_tmp42A=_cycalloc(sizeof(*_tmp42A)),((_tmp42A[0]=((_tmp42C.tag=Cyc_Lexing_Error,((
_tmp42C.f1=((_tmp42B="some action didn't return!",_tag_dyneither(_tmp42B,sizeof(
char),27))),_tmp42C)))),_tmp42A)))));}}int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(
lexbuf,9);}struct _tuple18*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);struct _tuple18*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL88:{
const char*_tmp42D;Cyc_current_line=((_tmp42D="#define ",_tag_dyneither(_tmp42D,
sizeof(char),9)));}Cyc_suck_macroname(lexbuf);{struct _tuple18*_tmp42E;return(
_tmp42E=_cycalloc(sizeof(*_tmp42E)),((_tmp42E->f1=Cyc_current_line,((_tmp42E->f2=(
struct _dyneither_ptr*)_check_null(Cyc_current_source),_tmp42E)))));}case 1: _LL89:
return Cyc_suck_line(lexbuf);case 2: _LL8A: return 0;default: _LL8B:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_line_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp434;const char*_tmp433;struct Cyc_Lexing_Error_struct*_tmp432;(int)_throw((
void*)((_tmp432=_cycalloc(sizeof(*_tmp432)),((_tmp432[0]=((_tmp434.tag=Cyc_Lexing_Error,((
_tmp434.f1=((_tmp433="some action didn't return!",_tag_dyneither(_tmp433,sizeof(
char),27))),_tmp434)))),_tmp432)))));}}struct _tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*
lexbuf);struct _tuple18*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(
lexbuf,10);}int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL8D:{struct
_dyneither_ptr*_tmp435;Cyc_current_source=((_tmp435=_cycalloc(sizeof(*_tmp435)),((
_tmp435[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp435))));}Cyc_current_line=(
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);default: _LL8E:(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp43B;const char*_tmp43A;struct Cyc_Lexing_Error_struct*_tmp439;(int)_throw((
void*)((_tmp439=_cycalloc(sizeof(*_tmp439)),((_tmp439[0]=((_tmp43B.tag=Cyc_Lexing_Error,((
_tmp43B.f1=((_tmp43A="some action didn't return!",_tag_dyneither(_tmp43A,sizeof(
char),27))),_tmp43B)))),_tmp439)))));}}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(
lexbuf,11);}int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL90: Cyc_current_line=(
struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL91:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_restofline_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp441;const char*_tmp440;struct Cyc_Lexing_Error_struct*_tmp43F;(int)_throw((
void*)((_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F[0]=((_tmp441.tag=Cyc_Lexing_Error,((
_tmp441.f1=((_tmp440="some action didn't return!",_tag_dyneither(_tmp440,sizeof(
char),27))),_tmp441)))),_tmp43F)))));}}int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(
lexbuf,12);}struct _tuple20*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);struct _tuple20*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL93: return Cyc_spec(
lexbuf);case 1: _LL94: Cyc_current_headerfile=(struct _dyneither_ptr)Cyc_substring((
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=
0;Cyc_current_cstubs=0;Cyc_current_cycstubs=0;Cyc_current_hstubs=0;while(Cyc_commands(
lexbuf)){;}Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_current_hstubs);Cyc_current_cstubs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);Cyc_current_cycstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);{
struct _tuple20*_tmp442;return(_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442->f1=
Cyc_current_headerfile,((_tmp442->f2=Cyc_current_symbols,((_tmp442->f3=Cyc_current_omit_symbols,((
_tmp442->f4=Cyc_current_hstubs,((_tmp442->f5=Cyc_current_cstubs,((_tmp442->f6=
Cyc_current_cycstubs,_tmp442)))))))))))));}case 2: _LL95: return Cyc_spec(lexbuf);
case 3: _LL96: return 0;case 4: _LL97:{const char*_tmp446;void*_tmp445[1];struct Cyc_Int_pa_struct
_tmp444;(_tmp444.tag=1,((_tmp444.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp445[0]=& _tmp444,Cyc_fprintf(Cyc_stderr,((_tmp446="Error in .cys file: expected header file name, found '%c' instead\n",
_tag_dyneither(_tmp446,sizeof(char),67))),_tag_dyneither(_tmp445,sizeof(void*),1)))))));}
return 0;default: _LL98:(lexbuf->refill_buff)(lexbuf);return Cyc_spec_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp44C;const char*_tmp44B;struct Cyc_Lexing_Error_struct*
_tmp44A;(int)_throw((void*)((_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A[0]=((
_tmp44C.tag=Cyc_Lexing_Error,((_tmp44C.f1=((_tmp44B="some action didn't return!",
_tag_dyneither(_tmp44B,sizeof(char),27))),_tmp44C)))),_tmp44A)))));}}struct
_tuple20*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple20*Cyc_spec(struct
Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}int Cyc_commands_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL9A: return 0;case 1: _LL9B: return 0;case 2: _LL9C: Cyc_snarfed_symbols=0;while(
Cyc_snarfsymbols(lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL9D: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LL9E: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple19*_tmp44D;struct _tuple19*x=(_tmp44D=_cycalloc(sizeof(*_tmp44D)),((
_tmp44D->f1=(struct _dyneither_ptr)_tag_dyneither(0,0,0),((_tmp44D->f2=(struct
_dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp44D)))));{struct Cyc_List_List*_tmp44E;Cyc_current_hstubs=((_tmp44E=_cycalloc(
sizeof(*_tmp44E)),((_tmp44E->hd=x,((_tmp44E->tl=Cyc_current_hstubs,_tmp44E))))));}
return 1;}case 5: _LL9F: {struct _dyneither_ptr _tmp72=Cyc_Lexing_lexeme(lexbuf);{
const char*_tmp44F;_dyneither_ptr_inplace_plus(& _tmp72,sizeof(char),(int)Cyc_strlen(((
_tmp44F="hstub",_tag_dyneither(_tmp44F,sizeof(char),6)))));}while(isspace((int)*((
char*)_check_dyneither_subscript(_tmp72,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& _tmp72,sizeof(char),1);}{struct _dyneither_ptr t=
_tmp72;while(!isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp74=Cyc_substring((
struct _dyneither_ptr)_tmp72,0,(unsigned long)((t.curr - _tmp72.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp450;struct _tuple19*x=(_tmp450=
_cycalloc(sizeof(*_tmp450)),((_tmp450->f1=(struct _dyneither_ptr)_tmp74,((_tmp450->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp450)))));{struct Cyc_List_List*_tmp451;Cyc_current_hstubs=((_tmp451=_cycalloc(
sizeof(*_tmp451)),((_tmp451->hd=x,((_tmp451->tl=Cyc_current_hstubs,_tmp451))))));}
return 1;}}}}case 6: _LLA0: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp452;struct
_tuple19*x=(_tmp452=_cycalloc(sizeof(*_tmp452)),((_tmp452->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp452->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp452)))));{struct Cyc_List_List*
_tmp453;Cyc_current_cstubs=((_tmp453=_cycalloc(sizeof(*_tmp453)),((_tmp453->hd=x,((
_tmp453->tl=Cyc_current_cstubs,_tmp453))))));}return 1;}case 7: _LLA1: {struct
_dyneither_ptr _tmp79=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp454;
_dyneither_ptr_inplace_plus(& _tmp79,sizeof(char),(int)Cyc_strlen(((_tmp454="cstub",
_tag_dyneither(_tmp454,sizeof(char),6)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp79,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp79,sizeof(char),1);}{struct _dyneither_ptr t=_tmp79;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp7B=Cyc_substring((
struct _dyneither_ptr)_tmp79,0,(unsigned long)((t.curr - _tmp79.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp455;struct _tuple19*x=(_tmp455=
_cycalloc(sizeof(*_tmp455)),((_tmp455->f1=(struct _dyneither_ptr)_tmp7B,((_tmp455->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp455)))));{struct Cyc_List_List*_tmp456;Cyc_current_cstubs=((_tmp456=_cycalloc(
sizeof(*_tmp456)),((_tmp456->hd=x,((_tmp456->tl=Cyc_current_cstubs,_tmp456))))));}
return 1;}}}}case 8: _LLA2: Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)
Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp457;struct
_tuple19*x=(_tmp457=_cycalloc(sizeof(*_tmp457)),((_tmp457->f1=(struct
_dyneither_ptr)_tag_dyneither(0,0,0),((_tmp457->f2=(struct _dyneither_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp457)))));{struct Cyc_List_List*
_tmp458;Cyc_current_cycstubs=((_tmp458=_cycalloc(sizeof(*_tmp458)),((_tmp458->hd=
x,((_tmp458->tl=Cyc_current_cycstubs,_tmp458))))));}return 1;}case 9: _LLA3: {
struct _dyneither_ptr _tmp80=Cyc_Lexing_lexeme(lexbuf);{const char*_tmp459;
_dyneither_ptr_inplace_plus(& _tmp80,sizeof(char),(int)Cyc_strlen(((_tmp459="cycstub",
_tag_dyneither(_tmp459,sizeof(char),8)))));}while(isspace((int)*((char*)
_check_dyneither_subscript(_tmp80,sizeof(char),0)))){_dyneither_ptr_inplace_plus(&
_tmp80,sizeof(char),1);}{struct _dyneither_ptr t=_tmp80;while(!isspace((int)*((
char*)_check_dyneither_subscript(t,sizeof(char),0)))){
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dyneither_ptr _tmp82=Cyc_substring((
struct _dyneither_ptr)_tmp80,0,(unsigned long)((t.curr - _tmp80.curr)/ sizeof(char)));
Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);
while(Cyc_block(lexbuf)){;}{struct _tuple19*_tmp45A;struct _tuple19*x=(_tmp45A=
_cycalloc(sizeof(*_tmp45A)),((_tmp45A->f1=(struct _dyneither_ptr)_tmp82,((_tmp45A->f2=(
struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp45A)))));{struct Cyc_List_List*_tmp45B;Cyc_current_cycstubs=((_tmp45B=
_cycalloc(sizeof(*_tmp45B)),((_tmp45B->hd=x,((_tmp45B->tl=Cyc_current_cycstubs,
_tmp45B))))));}return 1;}}}}case 10: _LLA4: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_dyneither_ptr*_tmp45C;struct _dyneither_ptr*x=(_tmp45C=_cycalloc(sizeof(*_tmp45C)),((
_tmp45C[0]=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf)),_tmp45C)));{struct Cyc_List_List*_tmp45D;Cyc_current_cpp=((
_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D->hd=x,((_tmp45D->tl=Cyc_current_cpp,
_tmp45D))))));}return 1;}case 11: _LLA5: return 1;case 12: _LLA6: return 1;case 13: _LLA7:{
const char*_tmp461;void*_tmp460[1];struct Cyc_Int_pa_struct _tmp45F;(_tmp45F.tag=1,((
_tmp45F.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp460[0]=&
_tmp45F,Cyc_fprintf(Cyc_stderr,((_tmp461="Error in .cys file: expected command, found '%c' instead\n",
_tag_dyneither(_tmp461,sizeof(char),58))),_tag_dyneither(_tmp460,sizeof(void*),1)))))));}
return 0;default: _LLA8:(lexbuf->refill_buff)(lexbuf);return Cyc_commands_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp467;const char*_tmp466;struct
Cyc_Lexing_Error_struct*_tmp465;(int)_throw((void*)((_tmp465=_cycalloc(sizeof(*
_tmp465)),((_tmp465[0]=((_tmp467.tag=Cyc_Lexing_Error,((_tmp467.f1=((_tmp466="some action didn't return!",
_tag_dyneither(_tmp466,sizeof(char),27))),_tmp467)))),_tmp465)))));}}int Cyc_commands(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_commands_rec(lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLAA:{
struct _dyneither_ptr*_tmp46A;struct Cyc_List_List*_tmp469;Cyc_snarfed_symbols=((
_tmp469=_cycalloc(sizeof(*_tmp469)),((_tmp469->hd=((_tmp46A=_cycalloc(sizeof(*
_tmp46A)),((_tmp46A[0]=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp46A)))),((
_tmp469->tl=Cyc_snarfed_symbols,_tmp469))))));}return 1;case 1: _LLAB: return 1;case 2:
_LLAC: return 0;case 3: _LLAD:{const char*_tmp46D;void*_tmp46C;(_tmp46C=0,Cyc_fprintf(
Cyc_stderr,((_tmp46D="Error in .cys file: unexpected end-of-file\n",
_tag_dyneither(_tmp46D,sizeof(char),44))),_tag_dyneither(_tmp46C,sizeof(void*),0)));}
return 0;case 4: _LLAE:{const char*_tmp471;void*_tmp470[1];struct Cyc_Int_pa_struct
_tmp46F;(_tmp46F.tag=1,((_tmp46F.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp470[0]=& _tmp46F,Cyc_fprintf(Cyc_stderr,((_tmp471="Error in .cys file: expected symbol, found '%c' instead\n",
_tag_dyneither(_tmp471,sizeof(char),57))),_tag_dyneither(_tmp470,sizeof(void*),1)))))));}
return 0;default: _LLAF:(lexbuf->refill_buff)(lexbuf);return Cyc_snarfsymbols_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp477;const char*_tmp476;struct
Cyc_Lexing_Error_struct*_tmp475;(int)_throw((void*)((_tmp475=_cycalloc(sizeof(*
_tmp475)),((_tmp475[0]=((_tmp477.tag=Cyc_Lexing_Error,((_tmp477.f1=((_tmp476="some action didn't return!",
_tag_dyneither(_tmp476,sizeof(char),27))),_tmp477)))),_tmp475)))));}}int Cyc_snarfsymbols(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}int Cyc_block_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLB1:{const char*
_tmp47A;void*_tmp479;(_tmp479=0,Cyc_log(((_tmp47A="Warning: unclosed brace\n",
_tag_dyneither(_tmp47A,sizeof(char),25))),_tag_dyneither(_tmp479,sizeof(void*),0)));}
return 0;case 1: _LLB2: if(Cyc_braces_to_match == 1)return 0;-- Cyc_braces_to_match;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');return 1;case 2: _LLB3: ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;
case 3: _LLB4: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}return 1;case 4: _LLB5:{const char*_tmp47B;Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((_tmp47B="/*",_tag_dyneither(
_tmp47B,sizeof(char),3))));}while(Cyc_block_comment(lexbuf)){;}return 1;case 5:
_LLB6: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLB7: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLB8:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp481;const char*_tmp480;struct Cyc_Lexing_Error_struct*
_tmp47F;(int)_throw((void*)((_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F[0]=((
_tmp481.tag=Cyc_Lexing_Error,((_tmp481.f1=((_tmp480="some action didn't return!",
_tag_dyneither(_tmp480,sizeof(char),27))),_tmp481)))),_tmp47F)))));}}int Cyc_block(
struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){
return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLBA:{
const char*_tmp484;void*_tmp483;(_tmp483=0,Cyc_log(((_tmp484="Warning: unclosed string\n",
_tag_dyneither(_tmp484,sizeof(char),26))),_tag_dyneither(_tmp483,sizeof(void*),0)));}
return 0;case 1: _LLBB: Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
return 0;case 2: _LLBC:{const char*_tmp487;void*_tmp486;(_tmp486=0,Cyc_log(((_tmp487="Warning: unclosed string\n",
_tag_dyneither(_tmp487,sizeof(char),26))),_tag_dyneither(_tmp486,sizeof(void*),0)));}
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct
_dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 3: _LLBD: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 4: _LLBE: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 5: _LLBF: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLC0: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 7: _LLC1: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return
1;case 8: _LLC2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLC3:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_string_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp48D;const char*_tmp48C;struct Cyc_Lexing_Error_struct*_tmp48B;(int)_throw((
void*)((_tmp48B=_cycalloc(sizeof(*_tmp48B)),((_tmp48B[0]=((_tmp48D.tag=Cyc_Lexing_Error,((
_tmp48D.f1=((_tmp48C="some action didn't return!",_tag_dyneither(_tmp48C,sizeof(
char),27))),_tmp48D)))),_tmp48B)))));}}int Cyc_block_string(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC5:{const char*_tmp490;
void*_tmp48F;(_tmp48F=0,Cyc_log(((_tmp490="Warning: unclosed comment\n",
_tag_dyneither(_tmp490,sizeof(char),27))),_tag_dyneither(_tmp48F,sizeof(void*),0)));}
return 0;case 1: _LLC6:{const char*_tmp491;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),((_tmp491="*/",_tag_dyneither(_tmp491,sizeof(char),3))));}
return 0;case 2: _LLC7: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;default: _LLC8:(lexbuf->refill_buff)(
lexbuf);return Cyc_block_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp497;const char*_tmp496;struct Cyc_Lexing_Error_struct*_tmp495;(int)_throw((
void*)((_tmp495=_cycalloc(sizeof(*_tmp495)),((_tmp495[0]=((_tmp497.tag=Cyc_Lexing_Error,((
_tmp497.f1=((_tmp496="some action didn't return!",_tag_dyneither(_tmp496,sizeof(
char),27))),_tmp497)))),_tmp495)))));}}int Cyc_block_comment(struct Cyc_Lexing_lexbuf*
lexbuf);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple21{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e);void Cyc_scan_exp(
struct Cyc_Absyn_Exp*e){void*_tmpAA=((struct Cyc_Absyn_Exp*)_check_null(e))->r;
struct _tuple0*_tmpAC;struct _tuple0 _tmpAD;struct _dyneither_ptr*_tmpAE;struct
_tuple0*_tmpB0;struct _tuple0 _tmpB1;struct _dyneither_ptr*_tmpB2;struct Cyc_List_List*
_tmpB4;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_Absyn_Exp*
_tmpB9;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*
_tmpBD;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*
_tmpC3;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*
_tmpC8;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*
_tmpCC;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*
_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_List_List*
_tmpD5;void*_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_MallocInfo _tmpDA;
int _tmpDB;struct Cyc_Absyn_Exp*_tmpDC;void**_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;
struct Cyc_Absyn_Exp*_tmpE0;void*_tmpE2;void*_tmpE4;struct Cyc_Absyn_Exp*_tmpE6;
struct _dyneither_ptr*_tmpE7;struct Cyc_Absyn_Exp*_tmpE9;struct _dyneither_ptr*
_tmpEA;void*_tmpEC;void*_tmpED;struct Cyc_List_List*_tmpF0;_LLCB: {struct Cyc_Absyn_Var_e_struct*
_tmpAB=(struct Cyc_Absyn_Var_e_struct*)_tmpAA;if(_tmpAB->tag != 1)goto _LLCD;else{
_tmpAC=_tmpAB->f1;_tmpAD=*_tmpAC;_tmpAE=_tmpAD.f2;}}_LLCC: _tmpB2=_tmpAE;goto
_LLCE;_LLCD: {struct Cyc_Absyn_UnknownId_e_struct*_tmpAF=(struct Cyc_Absyn_UnknownId_e_struct*)
_tmpAA;if(_tmpAF->tag != 2)goto _LLCF;else{_tmpB0=_tmpAF->f1;_tmpB1=*_tmpB0;_tmpB2=
_tmpB1.f2;}}_LLCE: Cyc_add_target(_tmpB2);return;_LLCF: {struct Cyc_Absyn_Primop_e_struct*
_tmpB3=(struct Cyc_Absyn_Primop_e_struct*)_tmpAA;if(_tmpB3->tag != 3)goto _LLD1;
else{_tmpB4=_tmpB3->f2;}}_LLD0: for(0;_tmpB4 != 0;_tmpB4=_tmpB4->tl){Cyc_scan_exp((
struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpB4->hd));}return;_LLD1: {struct
Cyc_Absyn_Subscript_e_struct*_tmpB5=(struct Cyc_Absyn_Subscript_e_struct*)_tmpAA;
if(_tmpB5->tag != 25)goto _LLD3;else{_tmpB6=_tmpB5->f1;_tmpB7=_tmpB5->f2;}}_LLD2:
_tmpB9=_tmpB6;_tmpBA=_tmpB7;goto _LLD4;_LLD3: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpB8=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpAA;if(_tmpB8->tag != 9)goto _LLD5;
else{_tmpB9=_tmpB8->f1;_tmpBA=_tmpB8->f2;}}_LLD4: _tmpBC=_tmpB9;_tmpBD=_tmpBA;
goto _LLD6;_LLD5: {struct Cyc_Absyn_AssignOp_e_struct*_tmpBB=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmpAA;if(_tmpBB->tag != 4)goto _LLD7;else{_tmpBC=_tmpBB->f1;_tmpBD=_tmpBB->f3;}}
_LLD6: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBC);Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpBD);return;_LLD7: {struct Cyc_Absyn_Deref_e_struct*_tmpBE=(struct Cyc_Absyn_Deref_e_struct*)
_tmpAA;if(_tmpBE->tag != 22)goto _LLD9;else{_tmpBF=_tmpBE->f1;}}_LLD8: _tmpC1=
_tmpBF;goto _LLDA;_LLD9: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpC0=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpAA;if(_tmpC0->tag != 19)goto _LLDB;else{_tmpC1=_tmpC0->f1;}}_LLDA: _tmpC3=
_tmpC1;goto _LLDC;_LLDB: {struct Cyc_Absyn_Address_e_struct*_tmpC2=(struct Cyc_Absyn_Address_e_struct*)
_tmpAA;if(_tmpC2->tag != 16)goto _LLDD;else{_tmpC3=_tmpC2->f1;}}_LLDC: _tmpC5=
_tmpC3;goto _LLDE;_LLDD: {struct Cyc_Absyn_Increment_e_struct*_tmpC4=(struct Cyc_Absyn_Increment_e_struct*)
_tmpAA;if(_tmpC4->tag != 5)goto _LLDF;else{_tmpC5=_tmpC4->f1;}}_LLDE: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpC5);return;_LLDF: {struct Cyc_Absyn_Conditional_e_struct*
_tmpC6=(struct Cyc_Absyn_Conditional_e_struct*)_tmpAA;if(_tmpC6->tag != 6)goto
_LLE1;else{_tmpC7=_tmpC6->f1;_tmpC8=_tmpC6->f2;_tmpC9=_tmpC6->f3;}}_LLE0: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpC7);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC8);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpC9);return;_LLE1: {struct Cyc_Absyn_And_e_struct*_tmpCA=(
struct Cyc_Absyn_And_e_struct*)_tmpAA;if(_tmpCA->tag != 7)goto _LLE3;else{_tmpCB=
_tmpCA->f1;_tmpCC=_tmpCA->f2;}}_LLE2: _tmpCE=_tmpCB;_tmpCF=_tmpCC;goto _LLE4;_LLE3: {
struct Cyc_Absyn_Or_e_struct*_tmpCD=(struct Cyc_Absyn_Or_e_struct*)_tmpAA;if(
_tmpCD->tag != 8)goto _LLE5;else{_tmpCE=_tmpCD->f1;_tmpCF=_tmpCD->f2;}}_LLE4: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpCE);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCF);return;
_LLE5: {struct Cyc_Absyn_FnCall_e_struct*_tmpD0=(struct Cyc_Absyn_FnCall_e_struct*)
_tmpAA;if(_tmpD0->tag != 11)goto _LLE7;else{_tmpD1=_tmpD0->f1;_tmpD2=_tmpD0->f2;}}
_LLE6: _tmpD4=_tmpD1;_tmpD5=_tmpD2;goto _LLE8;_LLE7: {struct Cyc_Absyn_UnknownCall_e_struct*
_tmpD3=(struct Cyc_Absyn_UnknownCall_e_struct*)_tmpAA;if(_tmpD3->tag != 10)goto
_LLE9;else{_tmpD4=_tmpD3->f1;_tmpD5=_tmpD3->f2;}}_LLE8: Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmpD4);for(0;_tmpD5 != 0;_tmpD5=_tmpD5->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpD5->hd));}return;_LLE9: {struct Cyc_Absyn_Cast_e_struct*
_tmpD6=(struct Cyc_Absyn_Cast_e_struct*)_tmpAA;if(_tmpD6->tag != 15)goto _LLEB;
else{_tmpD7=(void*)_tmpD6->f1;_tmpD8=_tmpD6->f2;}}_LLEA: Cyc_scan_type(_tmpD7);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpD8);return;_LLEB: {struct Cyc_Absyn_Malloc_e_struct*
_tmpD9=(struct Cyc_Absyn_Malloc_e_struct*)_tmpAA;if(_tmpD9->tag != 35)goto _LLED;
else{_tmpDA=_tmpD9->f1;_tmpDB=_tmpDA.is_calloc;_tmpDC=_tmpDA.rgn;_tmpDD=_tmpDA.elt_type;
_tmpDE=_tmpDA.num_elts;}}_LLEC: if(_tmpDC != 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpDC));if(_tmpDD != 0)Cyc_scan_type(*_tmpDD);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpDE);return;_LLED: {struct Cyc_Absyn_Tagcheck_e_struct*
_tmpDF=(struct Cyc_Absyn_Tagcheck_e_struct*)_tmpAA;if(_tmpDF->tag != 39)goto _LLEF;
else{_tmpE0=_tmpDF->f1;}}_LLEE: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE0);
return;_LLEF: {struct Cyc_Absyn_Valueof_e_struct*_tmpE1=(struct Cyc_Absyn_Valueof_e_struct*)
_tmpAA;if(_tmpE1->tag != 40)goto _LLF1;else{_tmpE2=(void*)_tmpE1->f1;}}_LLF0:
_tmpE4=_tmpE2;goto _LLF2;_LLF1: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpE3=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpAA;if(_tmpE3->tag != 18)goto _LLF3;else{
_tmpE4=(void*)_tmpE3->f1;}}_LLF2: Cyc_scan_type(_tmpE4);return;_LLF3: {struct Cyc_Absyn_AggrMember_e_struct*
_tmpE5=(struct Cyc_Absyn_AggrMember_e_struct*)_tmpAA;if(_tmpE5->tag != 23)goto
_LLF5;else{_tmpE6=_tmpE5->f1;_tmpE7=_tmpE5->f2;}}_LLF4: _tmpE9=_tmpE6;_tmpEA=
_tmpE7;goto _LLF6;_LLF5: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpE8=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpAA;if(_tmpE8->tag != 24)goto _LLF7;else{_tmpE9=_tmpE8->f1;_tmpEA=_tmpE8->f2;}}
_LLF6: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpE9);Cyc_add_target(_tmpEA);return;
_LLF7: {struct Cyc_Absyn_Offsetof_e_struct*_tmpEB=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpAA;if(_tmpEB->tag != 20)goto _LLF9;else{_tmpEC=(void*)_tmpEB->f1;_tmpED=(void*)
_tmpEB->f2;}}_LLF8: Cyc_scan_type(_tmpEC);{void*_tmp101=_tmpED;struct
_dyneither_ptr*_tmp103;_LL11E: {struct Cyc_Absyn_StructField_struct*_tmp102=(
struct Cyc_Absyn_StructField_struct*)_tmp101;if(_tmp102->tag != 0)goto _LL120;else{
_tmp103=_tmp102->f1;}}_LL11F: Cyc_add_target(_tmp103);goto _LL11D;_LL120: {struct
Cyc_Absyn_TupleIndex_struct*_tmp104=(struct Cyc_Absyn_TupleIndex_struct*)_tmp101;
if(_tmp104->tag != 1)goto _LL11D;}_LL121: goto _LL11D;_LL11D:;}return;_LLF9: {struct
Cyc_Absyn_Const_e_struct*_tmpEE=(struct Cyc_Absyn_Const_e_struct*)_tmpAA;if(
_tmpEE->tag != 0)goto _LLFB;}_LLFA: return;_LLFB: {struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpEF=(struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpAA;if(_tmpEF->tag != 37)goto
_LLFD;else{_tmpF0=_tmpEF->f2;}}_LLFC: for(0;_tmpF0 != 0;_tmpF0=_tmpF0->tl){struct
_tuple21 _tmp106;struct Cyc_Absyn_Exp*_tmp107;struct _tuple21*_tmp105=(struct
_tuple21*)_tmpF0->hd;_tmp106=*_tmp105;_tmp107=_tmp106.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)
_tmp107);}return;_LLFD: {struct Cyc_Absyn_Swap_e_struct*_tmpF1=(struct Cyc_Absyn_Swap_e_struct*)
_tmpAA;if(_tmpF1->tag != 36)goto _LLFF;}_LLFE:{const char*_tmp49A;void*_tmp499;(
_tmp499=0,Cyc_fprintf(Cyc_stderr,((_tmp49A="Error: unexpected Swap_e\n",
_tag_dyneither(_tmp49A,sizeof(char),26))),_tag_dyneither(_tmp499,sizeof(void*),0)));}
exit(1);return;_LLFF: {struct Cyc_Absyn_StmtExp_e_struct*_tmpF2=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpAA;if(_tmpF2->tag != 38)goto _LL101;}_LL100:{const char*_tmp49D;void*_tmp49C;(
_tmp49C=0,Cyc_fprintf(Cyc_stderr,((_tmp49D="Error: unexpected Stmt_e\n",
_tag_dyneither(_tmp49D,sizeof(char),26))),_tag_dyneither(_tmp49C,sizeof(void*),0)));}
exit(1);return;_LL101: {struct Cyc_Absyn_Throw_e_struct*_tmpF3=(struct Cyc_Absyn_Throw_e_struct*)
_tmpAA;if(_tmpF3->tag != 12)goto _LL103;}_LL102:{const char*_tmp4A0;void*_tmp49F;(
_tmp49F=0,Cyc_fprintf(Cyc_stderr,((_tmp4A0="Error: unexpected Throw_e\n",
_tag_dyneither(_tmp4A0,sizeof(char),27))),_tag_dyneither(_tmp49F,sizeof(void*),0)));}
exit(1);return;_LL103: {struct Cyc_Absyn_NoInstantiate_e_struct*_tmpF4=(struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpAA;if(_tmpF4->tag != 13)goto _LL105;}_LL104:{const char*_tmp4A3;void*_tmp4A2;(
_tmp4A2=0,Cyc_fprintf(Cyc_stderr,((_tmp4A3="Error: unexpected NoInstantiate_e\n",
_tag_dyneither(_tmp4A3,sizeof(char),35))),_tag_dyneither(_tmp4A2,sizeof(void*),0)));}
exit(1);return;_LL105: {struct Cyc_Absyn_Instantiate_e_struct*_tmpF5=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpAA;if(_tmpF5->tag != 14)goto _LL107;}_LL106:{const char*_tmp4A6;void*_tmp4A5;(
_tmp4A5=0,Cyc_fprintf(Cyc_stderr,((_tmp4A6="Error: unexpected Instantiate_e\n",
_tag_dyneither(_tmp4A6,sizeof(char),33))),_tag_dyneither(_tmp4A5,sizeof(void*),0)));}
exit(1);return;_LL107: {struct Cyc_Absyn_New_e_struct*_tmpF6=(struct Cyc_Absyn_New_e_struct*)
_tmpAA;if(_tmpF6->tag != 17)goto _LL109;}_LL108:{const char*_tmp4A9;void*_tmp4A8;(
_tmp4A8=0,Cyc_fprintf(Cyc_stderr,((_tmp4A9="Error: unexpected New_e\n",
_tag_dyneither(_tmp4A9,sizeof(char),25))),_tag_dyneither(_tmp4A8,sizeof(void*),0)));}
exit(1);return;_LL109: {struct Cyc_Absyn_Gentyp_e_struct*_tmpF7=(struct Cyc_Absyn_Gentyp_e_struct*)
_tmpAA;if(_tmpF7->tag != 21)goto _LL10B;}_LL10A:{const char*_tmp4AC;void*_tmp4AB;(
_tmp4AB=0,Cyc_fprintf(Cyc_stderr,((_tmp4AC="Error: unexpected Gentyp_e\n",
_tag_dyneither(_tmp4AC,sizeof(char),28))),_tag_dyneither(_tmp4AB,sizeof(void*),0)));}
exit(1);return;_LL10B: {struct Cyc_Absyn_Tuple_e_struct*_tmpF8=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpAA;if(_tmpF8->tag != 26)goto _LL10D;}_LL10C:{const char*_tmp4AF;void*_tmp4AE;(
_tmp4AE=0,Cyc_fprintf(Cyc_stderr,((_tmp4AF="Error: unexpected Tuple_e\n",
_tag_dyneither(_tmp4AF,sizeof(char),27))),_tag_dyneither(_tmp4AE,sizeof(void*),0)));}
exit(1);return;_LL10D: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpF9=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpAA;if(_tmpF9->tag != 27)goto _LL10F;}_LL10E:{const char*_tmp4B2;void*_tmp4B1;(
_tmp4B1=0,Cyc_fprintf(Cyc_stderr,((_tmp4B2="Error: unexpected CompoundLit_e\n",
_tag_dyneither(_tmp4B2,sizeof(char),33))),_tag_dyneither(_tmp4B1,sizeof(void*),0)));}
exit(1);return;_LL10F: {struct Cyc_Absyn_Array_e_struct*_tmpFA=(struct Cyc_Absyn_Array_e_struct*)
_tmpAA;if(_tmpFA->tag != 28)goto _LL111;}_LL110:{const char*_tmp4B5;void*_tmp4B4;(
_tmp4B4=0,Cyc_fprintf(Cyc_stderr,((_tmp4B5="Error: unexpected Array_e\n",
_tag_dyneither(_tmp4B5,sizeof(char),27))),_tag_dyneither(_tmp4B4,sizeof(void*),0)));}
exit(1);return;_LL111: {struct Cyc_Absyn_Comprehension_e_struct*_tmpFB=(struct Cyc_Absyn_Comprehension_e_struct*)
_tmpAA;if(_tmpFB->tag != 29)goto _LL113;}_LL112:{const char*_tmp4B8;void*_tmp4B7;(
_tmp4B7=0,Cyc_fprintf(Cyc_stderr,((_tmp4B8="Error: unexpected Comprehension_e\n",
_tag_dyneither(_tmp4B8,sizeof(char),35))),_tag_dyneither(_tmp4B7,sizeof(void*),0)));}
exit(1);return;_LL113: {struct Cyc_Absyn_Aggregate_e_struct*_tmpFC=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpAA;if(_tmpFC->tag != 30)goto _LL115;}_LL114:{const char*_tmp4BB;void*_tmp4BA;(
_tmp4BA=0,Cyc_fprintf(Cyc_stderr,((_tmp4BB="Error: unexpected Aggregate_e\n",
_tag_dyneither(_tmp4BB,sizeof(char),31))),_tag_dyneither(_tmp4BA,sizeof(void*),0)));}
exit(1);return;_LL115: {struct Cyc_Absyn_AnonStruct_e_struct*_tmpFD=(struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpAA;if(_tmpFD->tag != 31)goto _LL117;}_LL116:{const char*_tmp4BE;void*_tmp4BD;(
_tmp4BD=0,Cyc_fprintf(Cyc_stderr,((_tmp4BE="Error: unexpected AnonStruct_e\n",
_tag_dyneither(_tmp4BE,sizeof(char),32))),_tag_dyneither(_tmp4BD,sizeof(void*),0)));}
exit(1);return;_LL117: {struct Cyc_Absyn_Datatype_e_struct*_tmpFE=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpAA;if(_tmpFE->tag != 32)goto _LL119;}_LL118:{const char*_tmp4C1;void*_tmp4C0;(
_tmp4C0=0,Cyc_fprintf(Cyc_stderr,((_tmp4C1="Error: unexpected Datatype_e\n",
_tag_dyneither(_tmp4C1,sizeof(char),30))),_tag_dyneither(_tmp4C0,sizeof(void*),0)));}
exit(1);return;_LL119: {struct Cyc_Absyn_Enum_e_struct*_tmpFF=(struct Cyc_Absyn_Enum_e_struct*)
_tmpAA;if(_tmpFF->tag != 33)goto _LL11B;}_LL11A:{const char*_tmp4C4;void*_tmp4C3;(
_tmp4C3=0,Cyc_fprintf(Cyc_stderr,((_tmp4C4="Error: unexpected Enum_e\n",
_tag_dyneither(_tmp4C4,sizeof(char),26))),_tag_dyneither(_tmp4C3,sizeof(void*),0)));}
exit(1);return;_LL11B: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp100=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpAA;if(_tmp100->tag != 34)goto _LLCA;}_LL11C:{const char*_tmp4C7;void*_tmp4C6;(
_tmp4C6=0,Cyc_fprintf(Cyc_stderr,((_tmp4C7="Error: unexpected AnonEnum_e\n",
_tag_dyneither(_tmp4C7,sizeof(char),30))),_tag_dyneither(_tmp4C6,sizeof(void*),0)));}
exit(1);return;_LLCA:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo);void Cyc_scan_exp_opt(
struct Cyc_Absyn_Exp*eo){if((unsigned int)eo)Cyc_scan_exp(eo);return;}void Cyc_scan_type(
void*t);void Cyc_scan_type(void*t){void*_tmp128=t;struct Cyc_Absyn_PtrInfo _tmp12E;
struct Cyc_Absyn_ArrayInfo _tmp130;void*_tmp131;struct Cyc_Absyn_Exp*_tmp132;union
Cyc_Absyn_Constraint*_tmp133;struct Cyc_Absyn_FnInfo _tmp135;struct Cyc_List_List*
_tmp137;struct Cyc_Absyn_AggrInfo _tmp13A;union Cyc_Absyn_AggrInfoU _tmp13B;struct
_tuple0*_tmp13D;struct _tuple0 _tmp13E;struct _dyneither_ptr*_tmp13F;struct _tuple0*
_tmp141;struct _tuple0 _tmp142;struct _dyneither_ptr*_tmp143;_LL123: {struct Cyc_Absyn_VoidType_struct*
_tmp129=(struct Cyc_Absyn_VoidType_struct*)_tmp128;if(_tmp129->tag != 0)goto _LL125;}
_LL124: goto _LL126;_LL125: {struct Cyc_Absyn_IntType_struct*_tmp12A=(struct Cyc_Absyn_IntType_struct*)
_tmp128;if(_tmp12A->tag != 6)goto _LL127;}_LL126: goto _LL128;_LL127: {struct Cyc_Absyn_FloatType_struct*
_tmp12B=(struct Cyc_Absyn_FloatType_struct*)_tmp128;if(_tmp12B->tag != 7)goto
_LL129;}_LL128: goto _LL12A;_LL129: {struct Cyc_Absyn_DoubleType_struct*_tmp12C=(
struct Cyc_Absyn_DoubleType_struct*)_tmp128;if(_tmp12C->tag != 8)goto _LL12B;}
_LL12A: return;_LL12B: {struct Cyc_Absyn_PointerType_struct*_tmp12D=(struct Cyc_Absyn_PointerType_struct*)
_tmp128;if(_tmp12D->tag != 5)goto _LL12D;else{_tmp12E=_tmp12D->f1;}}_LL12C: Cyc_scan_type(
_tmp12E.elt_typ);return;_LL12D: {struct Cyc_Absyn_ArrayType_struct*_tmp12F=(
struct Cyc_Absyn_ArrayType_struct*)_tmp128;if(_tmp12F->tag != 9)goto _LL12F;else{
_tmp130=_tmp12F->f1;_tmp131=_tmp130.elt_type;_tmp132=_tmp130.num_elts;_tmp133=
_tmp130.zero_term;}}_LL12E: Cyc_scan_type(_tmp131);Cyc_scan_exp_opt(_tmp132);
return;_LL12F: {struct Cyc_Absyn_FnType_struct*_tmp134=(struct Cyc_Absyn_FnType_struct*)
_tmp128;if(_tmp134->tag != 10)goto _LL131;else{_tmp135=_tmp134->f1;}}_LL130: Cyc_scan_type(
_tmp135.ret_typ);{struct Cyc_List_List*_tmp152=_tmp135.args;for(0;_tmp152 != 0;
_tmp152=_tmp152->tl){struct _tuple7 _tmp154;void*_tmp155;struct _tuple7*_tmp153=(
struct _tuple7*)_tmp152->hd;_tmp154=*_tmp153;_tmp155=_tmp154.f3;Cyc_scan_type(
_tmp155);}}if(_tmp135.cyc_varargs != 0)Cyc_scan_type((_tmp135.cyc_varargs)->type);
return;_LL131: {struct Cyc_Absyn_AnonAggrType_struct*_tmp136=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp128;if(_tmp136->tag != 13)goto _LL133;else{_tmp137=_tmp136->f2;}}_LL132: for(0;
_tmp137 != 0;_tmp137=_tmp137->tl){Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)
_tmp137->hd)->type);Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp137->hd)->width);}
return;_LL133: {struct Cyc_Absyn_AnonEnumType_struct*_tmp138=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp128;if(_tmp138->tag != 15)goto _LL135;}_LL134: return;_LL135: {struct Cyc_Absyn_AggrType_struct*
_tmp139=(struct Cyc_Absyn_AggrType_struct*)_tmp128;if(_tmp139->tag != 12)goto
_LL137;else{_tmp13A=_tmp139->f1;_tmp13B=_tmp13A.aggr_info;}}_LL136: {struct
_tuple0*_tmp157;struct _tuple0 _tmp158;struct _dyneither_ptr*_tmp159;struct _tuple9
_tmp156=Cyc_Absyn_aggr_kinded_name(_tmp13B);_tmp157=_tmp156.f2;_tmp158=*_tmp157;
_tmp159=_tmp158.f2;_tmp13F=_tmp159;goto _LL138;}_LL137: {struct Cyc_Absyn_EnumType_struct*
_tmp13C=(struct Cyc_Absyn_EnumType_struct*)_tmp128;if(_tmp13C->tag != 14)goto
_LL139;else{_tmp13D=_tmp13C->f1;_tmp13E=*_tmp13D;_tmp13F=_tmp13E.f2;}}_LL138:
_tmp143=_tmp13F;goto _LL13A;_LL139: {struct Cyc_Absyn_TypedefType_struct*_tmp140=(
struct Cyc_Absyn_TypedefType_struct*)_tmp128;if(_tmp140->tag != 18)goto _LL13B;
else{_tmp141=_tmp140->f1;_tmp142=*_tmp141;_tmp143=_tmp142.f2;}}_LL13A: Cyc_add_target(
_tmp143);return;_LL13B: {struct Cyc_Absyn_Evar_struct*_tmp144=(struct Cyc_Absyn_Evar_struct*)
_tmp128;if(_tmp144->tag != 1)goto _LL13D;}_LL13C:{const char*_tmp4CA;void*_tmp4C9;(
_tmp4C9=0,Cyc_fprintf(Cyc_stderr,((_tmp4CA="Error: unexpected Evar\n",
_tag_dyneither(_tmp4CA,sizeof(char),24))),_tag_dyneither(_tmp4C9,sizeof(void*),0)));}
exit(1);return;_LL13D: {struct Cyc_Absyn_VarType_struct*_tmp145=(struct Cyc_Absyn_VarType_struct*)
_tmp128;if(_tmp145->tag != 2)goto _LL13F;}_LL13E:{const char*_tmp4CD;void*_tmp4CC;(
_tmp4CC=0,Cyc_fprintf(Cyc_stderr,((_tmp4CD="Error: unexpected VarType\n",
_tag_dyneither(_tmp4CD,sizeof(char),27))),_tag_dyneither(_tmp4CC,sizeof(void*),0)));}
exit(1);return;_LL13F: {struct Cyc_Absyn_DatatypeType_struct*_tmp146=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp128;if(_tmp146->tag != 3)goto _LL141;}_LL140:{const char*_tmp4D0;void*_tmp4CF;(
_tmp4CF=0,Cyc_fprintf(Cyc_stderr,((_tmp4D0="Error: unexpected DatatypeType\n",
_tag_dyneither(_tmp4D0,sizeof(char),32))),_tag_dyneither(_tmp4CF,sizeof(void*),0)));}
exit(1);return;_LL141: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp147=(struct
Cyc_Absyn_DatatypeFieldType_struct*)_tmp128;if(_tmp147->tag != 4)goto _LL143;}
_LL142:{const char*_tmp4D3;void*_tmp4D2;(_tmp4D2=0,Cyc_fprintf(Cyc_stderr,((
_tmp4D3="Error: unexpected DatatypeFieldType\n",_tag_dyneither(_tmp4D3,sizeof(
char),37))),_tag_dyneither(_tmp4D2,sizeof(void*),0)));}exit(1);return;_LL143: {
struct Cyc_Absyn_TupleType_struct*_tmp148=(struct Cyc_Absyn_TupleType_struct*)
_tmp128;if(_tmp148->tag != 11)goto _LL145;}_LL144:{const char*_tmp4D6;void*_tmp4D5;(
_tmp4D5=0,Cyc_fprintf(Cyc_stderr,((_tmp4D6="Error: unexpected TupleType\n",
_tag_dyneither(_tmp4D6,sizeof(char),29))),_tag_dyneither(_tmp4D5,sizeof(void*),0)));}
exit(1);return;_LL145: {struct Cyc_Absyn_RgnHandleType_struct*_tmp149=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp128;if(_tmp149->tag != 16)goto _LL147;}_LL146:{const char*_tmp4D9;void*_tmp4D8;(
_tmp4D8=0,Cyc_fprintf(Cyc_stderr,((_tmp4D9="Error: unexpected RgnHandleType\n",
_tag_dyneither(_tmp4D9,sizeof(char),33))),_tag_dyneither(_tmp4D8,sizeof(void*),0)));}
exit(1);return;_LL147: {struct Cyc_Absyn_DynRgnType_struct*_tmp14A=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp128;if(_tmp14A->tag != 17)goto _LL149;}_LL148:{const char*_tmp4DC;void*_tmp4DB;(
_tmp4DB=0,Cyc_fprintf(Cyc_stderr,((_tmp4DC="Error: unexpected DynRgnType\n",
_tag_dyneither(_tmp4DC,sizeof(char),30))),_tag_dyneither(_tmp4DB,sizeof(void*),0)));}
exit(1);return;_LL149: {struct Cyc_Absyn_HeapRgn_struct*_tmp14B=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp128;if(_tmp14B->tag != 21)goto _LL14B;}_LL14A:{const char*_tmp4DF;void*_tmp4DE;(
_tmp4DE=0,Cyc_fprintf(Cyc_stderr,((_tmp4DF="Error: unexpected HeapRgn\n",
_tag_dyneither(_tmp4DF,sizeof(char),27))),_tag_dyneither(_tmp4DE,sizeof(void*),0)));}
exit(1);return;_LL14B: {struct Cyc_Absyn_UniqueRgn_struct*_tmp14C=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp128;if(_tmp14C->tag != 22)goto _LL14D;}_LL14C:{const char*_tmp4E2;void*_tmp4E1;(
_tmp4E1=0,Cyc_fprintf(Cyc_stderr,((_tmp4E2="Error: unexpected UniqueRgn\n",
_tag_dyneither(_tmp4E2,sizeof(char),29))),_tag_dyneither(_tmp4E1,sizeof(void*),0)));}
exit(1);return;_LL14D: {struct Cyc_Absyn_AccessEff_struct*_tmp14D=(struct Cyc_Absyn_AccessEff_struct*)
_tmp128;if(_tmp14D->tag != 23)goto _LL14F;}_LL14E:{const char*_tmp4E5;void*_tmp4E4;(
_tmp4E4=0,Cyc_fprintf(Cyc_stderr,((_tmp4E5="Error: unexpected AccessEff\n",
_tag_dyneither(_tmp4E5,sizeof(char),29))),_tag_dyneither(_tmp4E4,sizeof(void*),0)));}
exit(1);return;_LL14F: {struct Cyc_Absyn_JoinEff_struct*_tmp14E=(struct Cyc_Absyn_JoinEff_struct*)
_tmp128;if(_tmp14E->tag != 24)goto _LL151;}_LL150:{const char*_tmp4E8;void*_tmp4E7;(
_tmp4E7=0,Cyc_fprintf(Cyc_stderr,((_tmp4E8="Error: unexpected JoinEff\n",
_tag_dyneither(_tmp4E8,sizeof(char),27))),_tag_dyneither(_tmp4E7,sizeof(void*),0)));}
exit(1);return;_LL151: {struct Cyc_Absyn_RgnsEff_struct*_tmp14F=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp128;if(_tmp14F->tag != 25)goto _LL153;}_LL152:{const char*_tmp4EB;void*_tmp4EA;(
_tmp4EA=0,Cyc_fprintf(Cyc_stderr,((_tmp4EB="Error: unexpected RgnsEff\n",
_tag_dyneither(_tmp4EB,sizeof(char),27))),_tag_dyneither(_tmp4EA,sizeof(void*),0)));}
exit(1);return;_LL153: {struct Cyc_Absyn_TagType_struct*_tmp150=(struct Cyc_Absyn_TagType_struct*)
_tmp128;if(_tmp150->tag != 20)goto _LL155;}_LL154:{const char*_tmp4EE;void*_tmp4ED;(
_tmp4ED=0,Cyc_fprintf(Cyc_stderr,((_tmp4EE="Error: unexpected tag_t\n",
_tag_dyneither(_tmp4EE,sizeof(char),25))),_tag_dyneither(_tmp4ED,sizeof(void*),0)));}
exit(1);return;_LL155: {struct Cyc_Absyn_ValueofType_struct*_tmp151=(struct Cyc_Absyn_ValueofType_struct*)
_tmp128;if(_tmp151->tag != 19)goto _LL122;}_LL156:{const char*_tmp4F1;void*_tmp4F0;(
_tmp4F0=0,Cyc_fprintf(Cyc_stderr,((_tmp4F1="Error: unexpected valueof_t\n",
_tag_dyneither(_tmp4F1,sizeof(char),29))),_tag_dyneither(_tmp4F0,sizeof(void*),0)));}
exit(1);return;_LL122:;}struct _tuple22{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple17*Cyc_scan_decl(struct Cyc_Absyn_Decl*d);struct _tuple17*
Cyc_scan_decl(struct Cyc_Absyn_Decl*d){{struct Cyc_Set_Set**_tmp4F2;Cyc_current_targets=((
_tmp4F2=_cycalloc(sizeof(*_tmp4F2)),((_tmp4F2[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp4F2))));}{void*_tmp177=d->r;struct Cyc_Absyn_Vardecl*_tmp179;struct Cyc_Absyn_Fndecl*
_tmp17B;struct Cyc_Absyn_Aggrdecl*_tmp17D;struct Cyc_Absyn_Enumdecl*_tmp17F;struct
Cyc_Absyn_Typedefdecl*_tmp181;_LL158: {struct Cyc_Absyn_Var_d_struct*_tmp178=(
struct Cyc_Absyn_Var_d_struct*)_tmp177;if(_tmp178->tag != 0)goto _LL15A;else{
_tmp179=_tmp178->f1;}}_LL159: {struct _tuple0 _tmp18E;struct _dyneither_ptr*_tmp18F;
struct _tuple0*_tmp18D=_tmp179->name;_tmp18E=*_tmp18D;_tmp18F=_tmp18E.f2;Cyc_current_source=(
struct _dyneither_ptr*)_tmp18F;Cyc_scan_type(_tmp179->type);Cyc_scan_exp_opt(
_tmp179->initializer);goto _LL157;}_LL15A: {struct Cyc_Absyn_Fn_d_struct*_tmp17A=(
struct Cyc_Absyn_Fn_d_struct*)_tmp177;if(_tmp17A->tag != 1)goto _LL15C;else{_tmp17B=
_tmp17A->f1;}}_LL15B: {struct _tuple0 _tmp191;struct _dyneither_ptr*_tmp192;struct
_tuple0*_tmp190=_tmp17B->name;_tmp191=*_tmp190;_tmp192=_tmp191.f2;Cyc_current_source=(
struct _dyneither_ptr*)_tmp192;Cyc_scan_type(_tmp17B->ret_type);{struct Cyc_List_List*
_tmp193=_tmp17B->args;for(0;_tmp193 != 0;_tmp193=_tmp193->tl){struct _tuple22
_tmp195;void*_tmp196;struct _tuple22*_tmp194=(struct _tuple22*)_tmp193->hd;_tmp195=*
_tmp194;_tmp196=_tmp195.f3;Cyc_scan_type(_tmp196);}}if(_tmp17B->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp17B->cyc_varargs))->type);
if(_tmp17B->is_inline){const char*_tmp4F5;void*_tmp4F4;(_tmp4F4=0,Cyc_fprintf(Cyc_stderr,((
_tmp4F5="Warning: ignoring inline function\n",_tag_dyneither(_tmp4F5,sizeof(char),
35))),_tag_dyneither(_tmp4F4,sizeof(void*),0)));}goto _LL157;}_LL15C: {struct Cyc_Absyn_Aggr_d_struct*
_tmp17C=(struct Cyc_Absyn_Aggr_d_struct*)_tmp177;if(_tmp17C->tag != 6)goto _LL15E;
else{_tmp17D=_tmp17C->f1;}}_LL15D: {struct _tuple0 _tmp19A;struct _dyneither_ptr*
_tmp19B;struct _tuple0*_tmp199=_tmp17D->name;_tmp19A=*_tmp199;_tmp19B=_tmp19A.f2;
Cyc_current_source=(struct _dyneither_ptr*)_tmp19B;if((unsigned int)_tmp17D->impl){{
struct Cyc_List_List*_tmp19C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp17D->impl))->fields;
for(0;_tmp19C != 0;_tmp19C=_tmp19C->tl){struct Cyc_Absyn_Aggrfield*_tmp19D=(struct
Cyc_Absyn_Aggrfield*)_tmp19C->hd;Cyc_scan_type(_tmp19D->type);Cyc_scan_exp_opt(
_tmp19D->width);}}{struct Cyc_List_List*_tmp19E=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp17D->impl))->fields;for(0;_tmp19E != 0;_tmp19E=_tmp19E->tl){;}}}
goto _LL157;}_LL15E: {struct Cyc_Absyn_Enum_d_struct*_tmp17E=(struct Cyc_Absyn_Enum_d_struct*)
_tmp177;if(_tmp17E->tag != 8)goto _LL160;else{_tmp17F=_tmp17E->f1;}}_LL15F: {
struct _tuple0 _tmp1A0;struct _dyneither_ptr*_tmp1A1;struct _tuple0*_tmp19F=_tmp17F->name;
_tmp1A0=*_tmp19F;_tmp1A1=_tmp1A0.f2;Cyc_current_source=(struct _dyneither_ptr*)
_tmp1A1;if((unsigned int)_tmp17F->fields){{struct Cyc_List_List*_tmp1A2=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp17F->fields))->v;for(0;
_tmp1A2 != 0;_tmp1A2=_tmp1A2->tl){struct Cyc_Absyn_Enumfield*_tmp1A3=(struct Cyc_Absyn_Enumfield*)
_tmp1A2->hd;Cyc_scan_exp_opt(_tmp1A3->tag);}}{struct Cyc_List_List*_tmp1A4=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp17F->fields))->v;for(
0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){;}}}goto _LL157;}_LL160: {struct Cyc_Absyn_Typedef_d_struct*
_tmp180=(struct Cyc_Absyn_Typedef_d_struct*)_tmp177;if(_tmp180->tag != 9)goto
_LL162;else{_tmp181=_tmp180->f1;}}_LL161: {struct _tuple0 _tmp1A6;struct
_dyneither_ptr*_tmp1A7;struct _tuple0*_tmp1A5=_tmp181->name;_tmp1A6=*_tmp1A5;
_tmp1A7=_tmp1A6.f2;Cyc_current_source=(struct _dyneither_ptr*)_tmp1A7;if((
unsigned int)_tmp181->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp181->defn))->v);goto _LL157;}_LL162: {struct Cyc_Absyn_Region_d_struct*
_tmp182=(struct Cyc_Absyn_Region_d_struct*)_tmp177;if(_tmp182->tag != 4)goto _LL164;}
_LL163:{const char*_tmp4F8;void*_tmp4F7;(_tmp4F7=0,Cyc_fprintf(Cyc_stderr,((
_tmp4F8="Error: unexpected region declaration",_tag_dyneither(_tmp4F8,sizeof(
char),37))),_tag_dyneither(_tmp4F7,sizeof(void*),0)));}exit(1);_LL164: {struct
Cyc_Absyn_Alias_d_struct*_tmp183=(struct Cyc_Absyn_Alias_d_struct*)_tmp177;if(
_tmp183->tag != 5)goto _LL166;}_LL165:{const char*_tmp4FB;void*_tmp4FA;(_tmp4FA=0,
Cyc_fprintf(Cyc_stderr,((_tmp4FB="Error: unexpected alias declaration",
_tag_dyneither(_tmp4FB,sizeof(char),36))),_tag_dyneither(_tmp4FA,sizeof(void*),0)));}
exit(1);_LL166: {struct Cyc_Absyn_Porton_d_struct*_tmp184=(struct Cyc_Absyn_Porton_d_struct*)
_tmp177;if(_tmp184->tag != 14)goto _LL168;}_LL167:{const char*_tmp4FE;void*_tmp4FD;(
_tmp4FD=0,Cyc_fprintf(Cyc_stderr,((_tmp4FE="Error: unexpected __cyclone_port_on__",
_tag_dyneither(_tmp4FE,sizeof(char),38))),_tag_dyneither(_tmp4FD,sizeof(void*),0)));}
exit(1);return 0;_LL168: {struct Cyc_Absyn_Portoff_d_struct*_tmp185=(struct Cyc_Absyn_Portoff_d_struct*)
_tmp177;if(_tmp185->tag != 15)goto _LL16A;}_LL169:{const char*_tmp501;void*_tmp500;(
_tmp500=0,Cyc_fprintf(Cyc_stderr,((_tmp501="Error: unexpected __cyclone_port_off__",
_tag_dyneither(_tmp501,sizeof(char),39))),_tag_dyneither(_tmp500,sizeof(void*),0)));}
exit(1);return 0;_LL16A: {struct Cyc_Absyn_Let_d_struct*_tmp186=(struct Cyc_Absyn_Let_d_struct*)
_tmp177;if(_tmp186->tag != 2)goto _LL16C;}_LL16B:{const char*_tmp504;void*_tmp503;(
_tmp503=0,Cyc_fprintf(Cyc_stderr,((_tmp504="Error: unexpected let declaration\n",
_tag_dyneither(_tmp504,sizeof(char),35))),_tag_dyneither(_tmp503,sizeof(void*),0)));}
exit(1);return 0;_LL16C: {struct Cyc_Absyn_Datatype_d_struct*_tmp187=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp177;if(_tmp187->tag != 7)goto _LL16E;}_LL16D:{const char*_tmp507;void*_tmp506;(
_tmp506=0,Cyc_fprintf(Cyc_stderr,((_tmp507="Error: unexpected datatype declaration\n",
_tag_dyneither(_tmp507,sizeof(char),40))),_tag_dyneither(_tmp506,sizeof(void*),0)));}
exit(1);return 0;_LL16E: {struct Cyc_Absyn_Letv_d_struct*_tmp188=(struct Cyc_Absyn_Letv_d_struct*)
_tmp177;if(_tmp188->tag != 3)goto _LL170;}_LL16F:{const char*_tmp50A;void*_tmp509;(
_tmp509=0,Cyc_fprintf(Cyc_stderr,((_tmp50A="Error: unexpected let declaration\n",
_tag_dyneither(_tmp50A,sizeof(char),35))),_tag_dyneither(_tmp509,sizeof(void*),0)));}
exit(1);return 0;_LL170: {struct Cyc_Absyn_Namespace_d_struct*_tmp189=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp177;if(_tmp189->tag != 10)goto _LL172;}_LL171:{const char*_tmp50D;void*_tmp50C;(
_tmp50C=0,Cyc_fprintf(Cyc_stderr,((_tmp50D="Error: unexpected namespace declaration\n",
_tag_dyneither(_tmp50D,sizeof(char),41))),_tag_dyneither(_tmp50C,sizeof(void*),0)));}
exit(1);return 0;_LL172: {struct Cyc_Absyn_Using_d_struct*_tmp18A=(struct Cyc_Absyn_Using_d_struct*)
_tmp177;if(_tmp18A->tag != 11)goto _LL174;}_LL173:{const char*_tmp510;void*_tmp50F;(
_tmp50F=0,Cyc_fprintf(Cyc_stderr,((_tmp510="Error: unexpected using declaration\n",
_tag_dyneither(_tmp510,sizeof(char),37))),_tag_dyneither(_tmp50F,sizeof(void*),0)));}
exit(1);return 0;_LL174: {struct Cyc_Absyn_ExternC_d_struct*_tmp18B=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp177;if(_tmp18B->tag != 12)goto _LL176;}_LL175:{const char*_tmp513;void*_tmp512;(
_tmp512=0,Cyc_fprintf(Cyc_stderr,((_tmp513="Error: unexpected extern \"C\" declaration\n",
_tag_dyneither(_tmp513,sizeof(char),42))),_tag_dyneither(_tmp512,sizeof(void*),0)));}
exit(1);return 0;_LL176: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp18C=(struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp177;if(_tmp18C->tag != 13)goto _LL157;}
_LL177:{const char*_tmp516;void*_tmp515;(_tmp515=0,Cyc_fprintf(Cyc_stderr,((
_tmp516="Error: unexpected extern \"C include\" declaration\n",_tag_dyneither(
_tmp516,sizeof(char),50))),_tag_dyneither(_tmp515,sizeof(void*),0)));}exit(1);
return 0;_LL157:;}{struct _tuple17*_tmp517;return(_tmp517=_cycalloc(sizeof(*
_tmp517)),((_tmp517->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),((
_tmp517->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp517)))));}}
struct Cyc_Hashtable_Table*Cyc_new_deps();struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(
107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}struct Cyc_Set_Set*Cyc_find(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x);struct Cyc_Set_Set*Cyc_find(
struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){struct _handler_cons _tmp1BF;
_push_handler(& _tmp1BF);{int _tmp1C1=0;if(setjmp(_tmp1BF.handler))_tmp1C1=1;if(!
_tmp1C1){{struct Cyc_Set_Set*_tmp1C2=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*
t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0);return
_tmp1C2;};_pop_handler();}else{void*_tmp1C0=(void*)_exn_thrown;void*_tmp1C4=
_tmp1C0;_LL179: {struct Cyc_Core_Not_found_struct*_tmp1C5=(struct Cyc_Core_Not_found_struct*)
_tmp1C4;if(_tmp1C5->tag != Cyc_Core_Not_found)goto _LL17B;}_LL17A: return((struct
Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(
Cyc_strptrcmp);_LL17B:;_LL17C:(void)_throw(_tmp1C4);_LL178:;}}}struct Cyc_Set_Set*
Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t);struct Cyc_Set_Set*
Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*
emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct
_dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);struct Cyc_Set_Set*curr;for(curr=
emptyset;init != 0;init=init->tl){curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
struct Cyc_Set_Set*_tmp1C6=curr;struct _dyneither_ptr*_tmp1C7=_init_dyneither_ptr(
_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1);while(((int(*)(struct
Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1C6)> 0){struct Cyc_Set_Set*_tmp1C8=
emptyset;struct Cyc_Iter_Iter _tmp1C9=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*
rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp1C6);while(((
int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp1C9,&
_tmp1C7)){_tmp1C8=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(_tmp1C8,Cyc_find(t,_tmp1C7));}_tmp1C6=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp1C8,curr);curr=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
curr,_tmp1C6);}return curr;}}enum Cyc_buildlib_mode{Cyc_NORMAL  = 0,Cyc_GATHER  = 1,
Cyc_GATHERSCRIPT  = 2,Cyc_FINISH  = 3};static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering();static int Cyc_gathering(){return Cyc_mode == Cyc_GATHER
 || Cyc_mode == Cyc_GATHERSCRIPT;}static struct Cyc___cycFILE*Cyc_script_file=0;int
Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_prscript(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){if(Cyc_script_file == 0){{const
char*_tmp51A;void*_tmp519;(_tmp519=0,Cyc_fprintf(Cyc_stderr,((_tmp51A="Internal error: script file is NULL\n",
_tag_dyneither(_tmp51A,sizeof(char),37))),_tag_dyneither(_tmp519,sizeof(void*),0)));}
exit(1);}return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),
fmt,ap);}int Cyc_force_directory(struct _dyneither_ptr d);int Cyc_force_directory(
struct _dyneither_ptr d){if(Cyc_mode == Cyc_GATHERSCRIPT){const char*_tmp51F;void*
_tmp51E[2];struct Cyc_String_pa_struct _tmp51D;struct Cyc_String_pa_struct _tmp51C;(
_tmp51C.tag=0,((_tmp51C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((
_tmp51D.tag=0,((_tmp51D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d),((
_tmp51E[0]=& _tmp51D,((_tmp51E[1]=& _tmp51C,Cyc_prscript(((_tmp51F="if ! test -e %s; then mkdir %s; fi\n",
_tag_dyneither(_tmp51F,sizeof(char),36))),_tag_dyneither(_tmp51E,sizeof(void*),2)))))))))))));}
else{unsigned short _tmp520[0];int _tmp1D1=Cyc_open((const char*)_check_null(
_untag_dyneither_ptr(d,sizeof(char),1)),0,_tag_dyneither(_tmp520,sizeof(
unsigned short),0));if(_tmp1D1 == - 1){if(mkdir((const char*)_untag_dyneither_ptr(d,
sizeof(char),1),448)== - 1){{const char*_tmp524;void*_tmp523[1];struct Cyc_String_pa_struct
_tmp522;(_tmp522.tag=0,((_tmp522.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
d),((_tmp523[0]=& _tmp522,Cyc_fprintf(Cyc_stderr,((_tmp524="Error: could not create directory %s\n",
_tag_dyneither(_tmp524,sizeof(char),38))),_tag_dyneither(_tmp523,sizeof(void*),1)))))));}
return 1;}}else{close(_tmp1D1);}}return 0;}int Cyc_force_directory_prefixes(struct
_dyneither_ptr file);int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
struct _dyneither_ptr _tmp1D6=Cyc_strdup((struct _dyneither_ptr)file);struct Cyc_List_List*
_tmp1D7=0;while(1){_tmp1D6=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp1D6);
if(Cyc_strlen((struct _dyneither_ptr)_tmp1D6)== 0)break;{struct _dyneither_ptr*
_tmp527;struct Cyc_List_List*_tmp526;_tmp1D7=((_tmp526=_cycalloc(sizeof(*_tmp526)),((
_tmp526->hd=((_tmp527=_cycalloc(sizeof(*_tmp527)),((_tmp527[0]=(struct
_dyneither_ptr)_tmp1D6,_tmp527)))),((_tmp526->tl=_tmp1D7,_tmp526))))));}}for(0;
_tmp1D7 != 0;_tmp1D7=_tmp1D7->tl){if(Cyc_force_directory(*((struct _dyneither_ptr*)
_tmp1D7->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[11]="NO_SUPPORT";struct Cyc_NO_SUPPORT_struct{
char*tag;struct _dyneither_ptr f1;};struct _tuple23{struct Cyc_List_List*f1;struct
Cyc_List_List*f2;};int Cyc_process_file(const char*filename,struct Cyc_List_List*
start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct
Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs);int Cyc_process_file(const char*
filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,
struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*
cycstubs){struct Cyc___cycFILE*maybe;struct Cyc___cycFILE*in_file;struct Cyc___cycFILE*
out_file;int errorcode=0;{const char*_tmp52D;const char*_tmp52C;void*_tmp52B[1];
struct Cyc_String_pa_struct _tmp52A;(_tmp52A.tag=0,((_tmp52A.f1=(struct
_dyneither_ptr)((_tmp52D=filename,_tag_dyneither(_tmp52D,sizeof(char),
_get_zero_arr_size_char(_tmp52D,1)))),((_tmp52B[0]=& _tmp52A,Cyc_fprintf(Cyc_stderr,((
_tmp52C="********************************* %s...\n",_tag_dyneither(_tmp52C,
sizeof(char),41))),_tag_dyneither(_tmp52B,sizeof(void*),1)))))));}if(!Cyc_gathering()){
const char*_tmp533;const char*_tmp532;void*_tmp531[1];struct Cyc_String_pa_struct
_tmp530;(_tmp530.tag=0,((_tmp530.f1=(struct _dyneither_ptr)((_tmp533=filename,
_tag_dyneither(_tmp533,sizeof(char),_get_zero_arr_size_char(_tmp533,1)))),((
_tmp531[0]=& _tmp530,Cyc_log(((_tmp532="\n%s:\n",_tag_dyneither(_tmp532,sizeof(
char),6))),_tag_dyneither(_tmp531,sizeof(void*),1)))))));}{const char*_tmp534;
struct _dyneither_ptr _tmp1E2=Cyc_Filename_basename(((_tmp534=filename,
_tag_dyneither(_tmp534,sizeof(char),_get_zero_arr_size_char(_tmp534,1)))));const
char*_tmp535;struct _dyneither_ptr _tmp1E3=Cyc_Filename_dirname(((_tmp535=filename,
_tag_dyneither(_tmp535,sizeof(char),_get_zero_arr_size_char(_tmp535,1)))));
struct _dyneither_ptr _tmp1E4=Cyc_Filename_chop_extension((struct _dyneither_ptr)
_tmp1E2);const char*_tmp536;const char*_tmp1E5=(const char*)_untag_dyneither_ptr(
Cyc_strconcat((struct _dyneither_ptr)_tmp1E4,((_tmp536=".iA",_tag_dyneither(
_tmp536,sizeof(char),4)))),sizeof(char),1);const char*_tmp53E;void*_tmp53D[1];
struct Cyc_String_pa_struct _tmp53C;const char*_tmp53A;void*_tmp539[1];struct Cyc_String_pa_struct
_tmp538;const char*_tmp1E6=(const char*)_untag_dyneither_ptr(_get_dyneither_size(
_tmp1E3,sizeof(char))== 0?(_tmp53C.tag=0,((_tmp53C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1E4),((_tmp53D[0]=& _tmp53C,Cyc_aprintf(((_tmp53E="%s.iB",
_tag_dyneither(_tmp53E,sizeof(char),6))),_tag_dyneither(_tmp53D,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp1E3,(struct _dyneither_ptr)((
_tmp538.tag=0,((_tmp538.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1E4),((
_tmp539[0]=& _tmp538,Cyc_aprintf(((_tmp53A="%s.iB",_tag_dyneither(_tmp53A,sizeof(
char),6))),_tag_dyneither(_tmp539,sizeof(void*),1))))))))),sizeof(char),1);const
char*_tmp546;void*_tmp545[1];struct Cyc_String_pa_struct _tmp544;const char*_tmp542;
void*_tmp541[1];struct Cyc_String_pa_struct _tmp540;const char*_tmp1E7=(const char*)
_untag_dyneither_ptr(_get_dyneither_size(_tmp1E3,sizeof(char))== 0?(_tmp544.tag=
0,((_tmp544.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1E4),((_tmp545[0]=&
_tmp544,Cyc_aprintf(((_tmp546="%s.iC",_tag_dyneither(_tmp546,sizeof(char),6))),
_tag_dyneither(_tmp545,sizeof(void*),1))))))): Cyc_Filename_concat((struct
_dyneither_ptr)_tmp1E3,(struct _dyneither_ptr)((_tmp540.tag=0,((_tmp540.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1E4),((_tmp541[0]=& _tmp540,Cyc_aprintf(((
_tmp542="%s.iC",_tag_dyneither(_tmp542,sizeof(char),6))),_tag_dyneither(_tmp541,
sizeof(void*),1))))))))),sizeof(char),1);const char*_tmp54E;void*_tmp54D[1];
struct Cyc_String_pa_struct _tmp54C;const char*_tmp54A;void*_tmp549[1];struct Cyc_String_pa_struct
_tmp548;const char*_tmp1E8=(const char*)_untag_dyneither_ptr(_get_dyneither_size(
_tmp1E3,sizeof(char))== 0?(_tmp54C.tag=0,((_tmp54C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1E4),((_tmp54D[0]=& _tmp54C,Cyc_aprintf(((_tmp54E="%s.iD",
_tag_dyneither(_tmp54E,sizeof(char),6))),_tag_dyneither(_tmp54D,sizeof(void*),1))))))):
Cyc_Filename_concat((struct _dyneither_ptr)_tmp1E3,(struct _dyneither_ptr)((
_tmp548.tag=0,((_tmp548.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1E4),((
_tmp549[0]=& _tmp548,Cyc_aprintf(((_tmp54A="%s.iD",_tag_dyneither(_tmp54A,sizeof(
char),6))),_tag_dyneither(_tmp549,sizeof(void*),1))))))))),sizeof(char),1);
struct _handler_cons _tmp1E9;_push_handler(& _tmp1E9);{int _tmp1EB=0;if(setjmp(
_tmp1E9.handler))_tmp1EB=1;if(!_tmp1EB){{const char*_tmp54F;if(Cyc_force_directory_prefixes(((
_tmp54F=filename,_tag_dyneither(_tmp54F,sizeof(char),_get_zero_arr_size_char(
_tmp54F,1)))))){int _tmp1ED=1;_npop_handler(0);return _tmp1ED;}}if(Cyc_mode != Cyc_FINISH){
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cpp);if(Cyc_mode == Cyc_GATHERSCRIPT){{const char*_tmp555;const char*
_tmp554;void*_tmp553[1];struct Cyc_String_pa_struct _tmp552;(_tmp552.tag=0,((
_tmp552.f1=(struct _dyneither_ptr)((_tmp555=_tmp1E5,_tag_dyneither(_tmp555,
sizeof(char),_get_zero_arr_size_char(_tmp555,1)))),((_tmp553[0]=& _tmp552,Cyc_prscript(((
_tmp554="cat >%s <<XXX\n",_tag_dyneither(_tmp554,sizeof(char),15))),
_tag_dyneither(_tmp553,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp1F2=Cyc_current_cpp;
for(0;_tmp1F2 != 0;_tmp1F2=_tmp1F2->tl){const char*_tmp559;void*_tmp558[1];struct
Cyc_String_pa_struct _tmp557;(_tmp557.tag=0,((_tmp557.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp1F2->hd)),((_tmp558[0]=&
_tmp557,Cyc_prscript(((_tmp559="%s",_tag_dyneither(_tmp559,sizeof(char),3))),
_tag_dyneither(_tmp558,sizeof(void*),1)))))));}}{const char*_tmp55F;const char*
_tmp55E;void*_tmp55D[1];struct Cyc_String_pa_struct _tmp55C;(_tmp55C.tag=0,((
_tmp55C.f1=(struct _dyneither_ptr)((_tmp55F=filename,_tag_dyneither(_tmp55F,
sizeof(char),_get_zero_arr_size_char(_tmp55F,1)))),((_tmp55D[0]=& _tmp55C,Cyc_prscript(((
_tmp55E="#include <%s>\n",_tag_dyneither(_tmp55E,sizeof(char),15))),
_tag_dyneither(_tmp55D,sizeof(void*),1)))))));}{const char*_tmp562;void*_tmp561;(
_tmp561=0,Cyc_prscript(((_tmp562="XXX\n",_tag_dyneither(_tmp562,sizeof(char),5))),
_tag_dyneither(_tmp561,sizeof(void*),0)));}{const char*_tmp56B;const char*_tmp56A;
const char*_tmp569;void*_tmp568[2];struct Cyc_String_pa_struct _tmp567;struct Cyc_String_pa_struct
_tmp566;(_tmp566.tag=0,((_tmp566.f1=(struct _dyneither_ptr)((_tmp56B=_tmp1E5,
_tag_dyneither(_tmp56B,sizeof(char),_get_zero_arr_size_char(_tmp56B,1)))),((
_tmp567.tag=0,((_tmp567.f1=(struct _dyneither_ptr)((_tmp56A=_tmp1E6,
_tag_dyneither(_tmp56A,sizeof(char),_get_zero_arr_size_char(_tmp56A,1)))),((
_tmp568[0]=& _tmp567,((_tmp568[1]=& _tmp566,Cyc_prscript(((_tmp569="$GCC -E -dM -o %s -x c %s && \\\n",
_tag_dyneither(_tmp569,sizeof(char),32))),_tag_dyneither(_tmp568,sizeof(void*),2)))))))))))));}{
const char*_tmp574;const char*_tmp573;const char*_tmp572;void*_tmp571[2];struct Cyc_String_pa_struct
_tmp570;struct Cyc_String_pa_struct _tmp56F;(_tmp56F.tag=0,((_tmp56F.f1=(struct
_dyneither_ptr)((_tmp574=_tmp1E5,_tag_dyneither(_tmp574,sizeof(char),
_get_zero_arr_size_char(_tmp574,1)))),((_tmp570.tag=0,((_tmp570.f1=(struct
_dyneither_ptr)((_tmp573=_tmp1E7,_tag_dyneither(_tmp573,sizeof(char),
_get_zero_arr_size_char(_tmp573,1)))),((_tmp571[0]=& _tmp570,((_tmp571[1]=&
_tmp56F,Cyc_prscript(((_tmp572="$GCC -E     -o %s -x c %s;\n",_tag_dyneither(
_tmp572,sizeof(char),28))),_tag_dyneither(_tmp571,sizeof(void*),2)))))))))))));}{
const char*_tmp57A;const char*_tmp579;void*_tmp578[1];struct Cyc_String_pa_struct
_tmp577;(_tmp577.tag=0,((_tmp577.f1=(struct _dyneither_ptr)((_tmp57A=_tmp1E5,
_tag_dyneither(_tmp57A,sizeof(char),_get_zero_arr_size_char(_tmp57A,1)))),((
_tmp578[0]=& _tmp577,Cyc_prscript(((_tmp579="rm %s\n",_tag_dyneither(_tmp579,
sizeof(char),7))),_tag_dyneither(_tmp578,sizeof(void*),1)))))));}}else{maybe=Cyc_fopen(
_tmp1E5,(const char*)"w");if(!((unsigned int)maybe)){{const char*_tmp580;const char*
_tmp57F;void*_tmp57E[1];struct Cyc_String_pa_struct _tmp57D;(_tmp57D.tag=0,((
_tmp57D.f1=(struct _dyneither_ptr)((_tmp580=_tmp1E5,_tag_dyneither(_tmp580,
sizeof(char),_get_zero_arr_size_char(_tmp580,1)))),((_tmp57E[0]=& _tmp57D,Cyc_fprintf(
Cyc_stderr,((_tmp57F="Error: could not create file %s\n",_tag_dyneither(_tmp57F,
sizeof(char),33))),_tag_dyneither(_tmp57E,sizeof(void*),1)))))));}{int _tmp210=1;
_npop_handler(0);return _tmp210;}}out_file=(struct Cyc___cycFILE*)maybe;{struct Cyc_List_List*
_tmp211=Cyc_current_cpp;for(0;_tmp211 != 0;_tmp211=_tmp211->tl){Cyc_fputs((const
char*)_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp211->hd),sizeof(char),1),
out_file);}}{const char*_tmp586;const char*_tmp585;void*_tmp584[1];struct Cyc_String_pa_struct
_tmp583;(_tmp583.tag=0,((_tmp583.f1=(struct _dyneither_ptr)((_tmp586=filename,
_tag_dyneither(_tmp586,sizeof(char),_get_zero_arr_size_char(_tmp586,1)))),((
_tmp584[0]=& _tmp583,Cyc_fprintf(out_file,((_tmp585="#include <%s>\n",
_tag_dyneither(_tmp585,sizeof(char),15))),_tag_dyneither(_tmp584,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{struct _dyneither_ptr _tmp216=Cstring_to_string(Ccomp);const
char*_tmp590;const char*_tmp58F;const char*_tmp58E;void*_tmp58D[3];struct Cyc_String_pa_struct
_tmp58C;struct Cyc_String_pa_struct _tmp58B;struct Cyc_String_pa_struct _tmp58A;char*
cmd=(char*)_untag_dyneither_ptr(((_tmp58A.tag=0,((_tmp58A.f1=(struct
_dyneither_ptr)((_tmp590=_tmp1E5,_tag_dyneither(_tmp590,sizeof(char),
_get_zero_arr_size_char(_tmp590,1)))),((_tmp58B.tag=0,((_tmp58B.f1=(struct
_dyneither_ptr)((_tmp58F=_tmp1E6,_tag_dyneither(_tmp58F,sizeof(char),
_get_zero_arr_size_char(_tmp58F,1)))),((_tmp58C.tag=0,((_tmp58C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp216),((_tmp58D[0]=& _tmp58C,((_tmp58D[1]=&
_tmp58B,((_tmp58D[2]=& _tmp58A,Cyc_aprintf(((_tmp58E="%s -E -dM -o %s -x c %s",
_tag_dyneither(_tmp58E,sizeof(char),24))),_tag_dyneither(_tmp58D,sizeof(void*),3)))))))))))))))))))),
sizeof(char),1);if(!system((const char*)cmd)){{const char*_tmp59A;const char*
_tmp599;const char*_tmp598;void*_tmp597[3];struct Cyc_String_pa_struct _tmp596;
struct Cyc_String_pa_struct _tmp595;struct Cyc_String_pa_struct _tmp594;cmd=(char*)
_untag_dyneither_ptr(((_tmp594.tag=0,((_tmp594.f1=(struct _dyneither_ptr)((
_tmp59A=_tmp1E5,_tag_dyneither(_tmp59A,sizeof(char),_get_zero_arr_size_char(
_tmp59A,1)))),((_tmp595.tag=0,((_tmp595.f1=(struct _dyneither_ptr)((_tmp599=
_tmp1E7,_tag_dyneither(_tmp599,sizeof(char),_get_zero_arr_size_char(_tmp599,1)))),((
_tmp596.tag=0,((_tmp596.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp216),((
_tmp597[0]=& _tmp596,((_tmp597[1]=& _tmp595,((_tmp597[2]=& _tmp594,Cyc_aprintf(((
_tmp598="%s -E -o %s -x c %s",_tag_dyneither(_tmp598,sizeof(char),20))),
_tag_dyneither(_tmp597,sizeof(void*),3)))))))))))))))))))),sizeof(char),1);}
system((const char*)cmd);}remove(_tmp1E5);}}}if(Cyc_gathering()){int _tmp225=0;
_npop_handler(0);return _tmp225;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp1E6,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp5AB;const char*_tmp5AA;const char*_tmp5A9;void*_tmp5A8[1];struct Cyc_String_pa_struct
_tmp5A7;struct Cyc_NO_SUPPORT_struct*_tmp5A6;(int)_throw((void*)((_tmp5A6=
_cycalloc(sizeof(*_tmp5A6)),((_tmp5A6[0]=((_tmp5AB.tag=Cyc_NO_SUPPORT,((_tmp5AB.f1=(
struct _dyneither_ptr)((_tmp5A7.tag=0,((_tmp5A7.f1=(struct _dyneither_ptr)((
_tmp5AA=_tmp1E6,_tag_dyneither(_tmp5AA,sizeof(char),_get_zero_arr_size_char(
_tmp5AA,1)))),((_tmp5A8[0]=& _tmp5A7,Cyc_aprintf(((_tmp5A9="can't open macrosfile %s",
_tag_dyneither(_tmp5A9,sizeof(char),25))),_tag_dyneither(_tmp5A8,sizeof(void*),1)))))))),
_tmp5AB)))),_tmp5A6)))));}in_file=(struct Cyc___cycFILE*)maybe;{struct Cyc_Lexing_lexbuf*
_tmp22C=Cyc_Lexing_from_file(in_file);struct _tuple17*entry;while((entry=((struct
_tuple17*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp22C))!= 0){struct
_tuple17 _tmp22E;struct _dyneither_ptr*_tmp22F;struct Cyc_Set_Set*_tmp230;struct
_tuple17*_tmp22D=(struct _tuple17*)_check_null(entry);_tmp22E=*_tmp22D;_tmp22F=
_tmp22E.f1;_tmp230=_tmp22E.f2;((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp22F,
_tmp230);}Cyc_fclose(in_file);maybe=Cyc_fopen(_tmp1E7,(const char*)"r");if(!((
unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct _tmp5BC;const char*_tmp5BB;const
char*_tmp5BA;void*_tmp5B9[1];struct Cyc_String_pa_struct _tmp5B8;struct Cyc_NO_SUPPORT_struct*
_tmp5B7;(int)_throw((void*)((_tmp5B7=_cycalloc(sizeof(*_tmp5B7)),((_tmp5B7[0]=((
_tmp5BC.tag=Cyc_NO_SUPPORT,((_tmp5BC.f1=(struct _dyneither_ptr)((_tmp5B8.tag=0,((
_tmp5B8.f1=(struct _dyneither_ptr)((_tmp5BB=_tmp1E7,_tag_dyneither(_tmp5BB,
sizeof(char),_get_zero_arr_size_char(_tmp5BB,1)))),((_tmp5B9[0]=& _tmp5B8,Cyc_aprintf(((
_tmp5BA="can't open declsfile %s",_tag_dyneither(_tmp5BA,sizeof(char),24))),
_tag_dyneither(_tmp5B9,sizeof(void*),1)))))))),_tmp5BC)))),_tmp5B7)))));}in_file=(
struct Cyc___cycFILE*)maybe;_tmp22C=Cyc_Lexing_from_file(in_file);Cyc_slurp_out=
Cyc_fopen(_tmp1E8,(const char*)"w");if(!((unsigned int)Cyc_slurp_out)){int _tmp237=
1;_npop_handler(0);return _tmp237;}while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))
Cyc_slurp)(_tmp22C)){;}Cyc_fclose(in_file);Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != Cyc_FINISH)remove(_tmp1E7);maybe=Cyc_fopen(
_tmp1E8,(const char*)"r");if(!((unsigned int)maybe)){int _tmp238=1;_npop_handler(0);
return _tmp238;}in_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp5BD;Cyc_Position_reset_position(((
_tmp5BD=_tmp1E8,_tag_dyneither(_tmp5BD,sizeof(char),_get_zero_arr_size_char(
_tmp5BD,1)))));}Cyc_Lex_lex_init(0);{struct Cyc_List_List*_tmp23A=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init(0);Cyc_fclose(in_file);remove(_tmp1E8);{struct Cyc_List_List*
_tmp23B=_tmp23A;for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){struct _tuple17*_tmp23C=
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp23B->hd);if(_tmp23C == 0)continue;{
struct _tuple17 _tmp23E;struct _dyneither_ptr*_tmp23F;struct Cyc_Set_Set*_tmp240;
struct _tuple17*_tmp23D=(struct _tuple17*)_tmp23C;_tmp23E=*_tmp23D;_tmp23F=_tmp23E.f1;
_tmp240=_tmp23E.f2;{struct Cyc_Set_Set*old;{struct _handler_cons _tmp241;
_push_handler(& _tmp241);{int _tmp243=0;if(setjmp(_tmp241.handler))_tmp243=1;if(!
_tmp243){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key))Cyc_Hashtable_lookup)(t,_tmp23F);;_pop_handler();}else{void*
_tmp242=(void*)_exn_thrown;void*_tmp245=_tmp242;_LL17E: {struct Cyc_Core_Not_found_struct*
_tmp246=(struct Cyc_Core_Not_found_struct*)_tmp245;if(_tmp246->tag != Cyc_Core_Not_found)
goto _LL180;}_LL17F: old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,
struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL17D;_LL180:;_LL181:(
void)_throw(_tmp245);_LL17D:;}}}((void(*)(struct Cyc_Hashtable_Table*t,struct
_dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp23F,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp240,old));}}}}{struct Cyc_Set_Set*_tmp247=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp248=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);{
struct Cyc_List_List*_tmp249=_tmp23A;for(0;_tmp249 != 0;_tmp249=_tmp249->tl){
struct Cyc_Absyn_Decl*_tmp24A=(struct Cyc_Absyn_Decl*)_tmp249->hd;struct
_dyneither_ptr*name;{void*_tmp24B=_tmp24A->r;struct Cyc_Absyn_Vardecl*_tmp24D;
struct Cyc_Absyn_Fndecl*_tmp24F;struct Cyc_Absyn_Aggrdecl*_tmp251;struct Cyc_Absyn_Enumdecl*
_tmp253;struct Cyc_Absyn_Typedefdecl*_tmp255;_LL183: {struct Cyc_Absyn_Var_d_struct*
_tmp24C=(struct Cyc_Absyn_Var_d_struct*)_tmp24B;if(_tmp24C->tag != 0)goto _LL185;
else{_tmp24D=_tmp24C->f1;}}_LL184: {struct _tuple0 _tmp262;struct _dyneither_ptr*
_tmp263;struct _tuple0*_tmp261=_tmp24D->name;_tmp262=*_tmp261;_tmp263=_tmp262.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp263);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp263))name=0;else{
name=(struct _dyneither_ptr*)_tmp263;}goto _LL182;}_LL185: {struct Cyc_Absyn_Fn_d_struct*
_tmp24E=(struct Cyc_Absyn_Fn_d_struct*)_tmp24B;if(_tmp24E->tag != 1)goto _LL187;
else{_tmp24F=_tmp24E->f1;}}_LL186: {struct _tuple0 _tmp265;struct _dyneither_ptr*
_tmp266;struct _tuple0*_tmp264=_tmp24F->name;_tmp265=*_tmp264;_tmp266=_tmp265.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*
elt))Cyc_Set_insert)(defined_symbols,_tmp266);if(((int(*)(int(*compare)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp266))name=0;else{
name=(struct _dyneither_ptr*)_tmp266;}goto _LL182;}_LL187: {struct Cyc_Absyn_Aggr_d_struct*
_tmp250=(struct Cyc_Absyn_Aggr_d_struct*)_tmp24B;if(_tmp250->tag != 6)goto _LL189;
else{_tmp251=_tmp250->f1;}}_LL188: {struct _tuple0 _tmp268;struct _dyneither_ptr*
_tmp269;struct _tuple0*_tmp267=_tmp251->name;_tmp268=*_tmp267;_tmp269=_tmp268.f2;
name=(struct _dyneither_ptr*)_tmp269;goto _LL182;}_LL189: {struct Cyc_Absyn_Enum_d_struct*
_tmp252=(struct Cyc_Absyn_Enum_d_struct*)_tmp24B;if(_tmp252->tag != 8)goto _LL18B;
else{_tmp253=_tmp252->f1;}}_LL18A: {struct _tuple0 _tmp26B;struct _dyneither_ptr*
_tmp26C;struct _tuple0*_tmp26A=_tmp253->name;_tmp26B=*_tmp26A;_tmp26C=_tmp26B.f2;
name=(struct _dyneither_ptr*)_tmp26C;if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp247,(struct _dyneither_ptr*)name)){
struct Cyc_List_List*_tmp5BE;_tmp248=((_tmp5BE=_cycalloc(sizeof(*_tmp5BE)),((
_tmp5BE->hd=_tmp24A,((_tmp5BE->tl=_tmp248,_tmp5BE))))));}else{if((unsigned int)
_tmp253->fields){struct Cyc_List_List*_tmp26E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp253->fields))->v;for(0;_tmp26E != 0;_tmp26E=_tmp26E->tl){struct
Cyc_Absyn_Enumfield*_tmp26F=(struct Cyc_Absyn_Enumfield*)_tmp26E->hd;struct
_tuple0 _tmp271;struct _dyneither_ptr*_tmp272;struct _tuple0*_tmp270=_tmp26F->name;
_tmp271=*_tmp270;_tmp272=_tmp271.f2;if(((int(*)(struct Cyc_Set_Set*s,struct
_dyneither_ptr*elt))Cyc_Set_member)(_tmp247,_tmp272)){{struct Cyc_List_List*
_tmp5BF;_tmp248=((_tmp5BF=_cycalloc(sizeof(*_tmp5BF)),((_tmp5BF->hd=_tmp24A,((
_tmp5BF->tl=_tmp248,_tmp5BF))))));}break;}}}}name=0;goto _LL182;}_LL18B: {struct
Cyc_Absyn_Typedef_d_struct*_tmp254=(struct Cyc_Absyn_Typedef_d_struct*)_tmp24B;
if(_tmp254->tag != 9)goto _LL18D;else{_tmp255=_tmp254->f1;}}_LL18C: {struct _tuple0
_tmp275;struct _dyneither_ptr*_tmp276;struct _tuple0*_tmp274=_tmp255->name;_tmp275=*
_tmp274;_tmp276=_tmp275.f2;name=(struct _dyneither_ptr*)_tmp276;goto _LL182;}
_LL18D: {struct Cyc_Absyn_Porton_d_struct*_tmp256=(struct Cyc_Absyn_Porton_d_struct*)
_tmp24B;if(_tmp256->tag != 14)goto _LL18F;}_LL18E: goto _LL190;_LL18F: {struct Cyc_Absyn_Portoff_d_struct*
_tmp257=(struct Cyc_Absyn_Portoff_d_struct*)_tmp24B;if(_tmp257->tag != 15)goto
_LL191;}_LL190: goto _LL192;_LL191: {struct Cyc_Absyn_Let_d_struct*_tmp258=(struct
Cyc_Absyn_Let_d_struct*)_tmp24B;if(_tmp258->tag != 2)goto _LL193;}_LL192: goto
_LL194;_LL193: {struct Cyc_Absyn_Datatype_d_struct*_tmp259=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp24B;if(_tmp259->tag != 7)goto _LL195;}_LL194: goto _LL196;_LL195: {struct Cyc_Absyn_Letv_d_struct*
_tmp25A=(struct Cyc_Absyn_Letv_d_struct*)_tmp24B;if(_tmp25A->tag != 3)goto _LL197;}
_LL196: goto _LL198;_LL197: {struct Cyc_Absyn_Namespace_d_struct*_tmp25B=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp24B;if(_tmp25B->tag != 10)goto _LL199;}_LL198: goto _LL19A;_LL199: {struct Cyc_Absyn_Using_d_struct*
_tmp25C=(struct Cyc_Absyn_Using_d_struct*)_tmp24B;if(_tmp25C->tag != 11)goto _LL19B;}
_LL19A: goto _LL19C;_LL19B: {struct Cyc_Absyn_ExternC_d_struct*_tmp25D=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp24B;if(_tmp25D->tag != 12)goto _LL19D;}_LL19C: goto _LL19E;_LL19D: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp25E=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp24B;if(_tmp25E->tag != 13)
goto _LL19F;}_LL19E: goto _LL1A0;_LL19F: {struct Cyc_Absyn_Region_d_struct*_tmp25F=(
struct Cyc_Absyn_Region_d_struct*)_tmp24B;if(_tmp25F->tag != 4)goto _LL1A1;}_LL1A0:
goto _LL1A2;_LL1A1: {struct Cyc_Absyn_Alias_d_struct*_tmp260=(struct Cyc_Absyn_Alias_d_struct*)
_tmp24B;if(_tmp260->tag != 5)goto _LL182;}_LL1A2: name=0;goto _LL182;_LL182:;}if(
name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
_tmp247,(struct _dyneither_ptr*)name)){struct Cyc_List_List*_tmp5C0;_tmp248=((
_tmp5C0=_cycalloc(sizeof(*_tmp5C0)),((_tmp5C0->hd=_tmp24A,((_tmp5C0->tl=_tmp248,
_tmp5C0))))));}}}if(!Cyc_do_setjmp){maybe=Cyc_fopen(filename,(const char*)"w");
if(!((unsigned int)maybe)){int _tmp278=1;_npop_handler(0);return _tmp278;}out_file=(
struct Cyc___cycFILE*)maybe;}else{out_file=Cyc_stdout;}{const char*_tmp5C6;const
char*_tmp5C5;void*_tmp5C4[1];struct Cyc_String_pa_struct _tmp5C3;struct
_dyneither_ptr ifdefmacro=(_tmp5C3.tag=0,((_tmp5C3.f1=(struct _dyneither_ptr)((
_tmp5C6=filename,_tag_dyneither(_tmp5C6,sizeof(char),_get_zero_arr_size_char(
_tmp5C6,1)))),((_tmp5C4[0]=& _tmp5C3,Cyc_aprintf(((_tmp5C5="_%s_",_tag_dyneither(
_tmp5C5,sizeof(char),5))),_tag_dyneither(_tmp5C4,sizeof(void*),1)))))));{int
_tmp279=0;for(0;_tmp279 < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp279){
if(((char*)ifdefmacro.curr)[_tmp279]== '.'  || ((char*)ifdefmacro.curr)[_tmp279]
== '/'){char _tmp5C9;char _tmp5C8;struct _dyneither_ptr _tmp5C7;(_tmp5C7=
_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp279),((_tmp5C8=*((char*)
_check_dyneither_subscript(_tmp5C7,sizeof(char),0)),((_tmp5C9='_',((
_get_dyneither_size(_tmp5C7,sizeof(char))== 1  && (_tmp5C8 == '\000'  && _tmp5C9 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp5C7.curr)=_tmp5C9)))))));}else{if(((char*)
ifdefmacro.curr)[_tmp279]!= '_'  && ((char*)ifdefmacro.curr)[_tmp279]!= '/'){char
_tmp5CC;char _tmp5CB;struct _dyneither_ptr _tmp5CA;(_tmp5CA=_dyneither_ptr_plus(
ifdefmacro,sizeof(char),_tmp279),((_tmp5CB=*((char*)_check_dyneither_subscript(
_tmp5CA,sizeof(char),0)),((_tmp5CC=(char)toupper((int)((char*)ifdefmacro.curr)[
_tmp279]),((_get_dyneither_size(_tmp5CA,sizeof(char))== 1  && (_tmp5CB == '\000'
 && _tmp5CC != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5CA.curr)=_tmp5CC)))))));}}}}{
const char*_tmp5D1;void*_tmp5D0[2];struct Cyc_String_pa_struct _tmp5CF;struct Cyc_String_pa_struct
_tmp5CE;(_tmp5CE.tag=0,((_tmp5CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
ifdefmacro),((_tmp5CF.tag=0,((_tmp5CF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)ifdefmacro),((_tmp5D0[0]=& _tmp5CF,((_tmp5D0[1]=& _tmp5CE,Cyc_fprintf(
out_file,((_tmp5D1="#ifndef %s\n#define %s\n",_tag_dyneither(_tmp5D1,sizeof(char),
23))),_tag_dyneither(_tmp5D0,sizeof(void*),2)))))))))))));}{struct Cyc_List_List*
_tmp284=0;struct Cyc_List_List*_tmp285=0;{struct Cyc_List_List*_tmp286=_tmp248;
for(0;_tmp286 != 0;_tmp286=_tmp286->tl){struct Cyc_Absyn_Decl*_tmp287=(struct Cyc_Absyn_Decl*)
_tmp286->hd;int _tmp288=0;struct _dyneither_ptr*name;{void*_tmp289=_tmp287->r;
struct Cyc_Absyn_Vardecl*_tmp28B;struct Cyc_Absyn_Fndecl*_tmp28D;struct Cyc_Absyn_Aggrdecl*
_tmp28F;struct Cyc_Absyn_Enumdecl*_tmp291;struct Cyc_Absyn_Typedefdecl*_tmp293;
_LL1A4: {struct Cyc_Absyn_Var_d_struct*_tmp28A=(struct Cyc_Absyn_Var_d_struct*)
_tmp289;if(_tmp28A->tag != 0)goto _LL1A6;else{_tmp28B=_tmp28A->f1;}}_LL1A5: {
struct _tuple0 _tmp2A0;struct _dyneither_ptr*_tmp2A1;struct _tuple0*_tmp29F=_tmp28B->name;
_tmp2A0=*_tmp29F;_tmp2A1=_tmp2A0.f2;name=(struct _dyneither_ptr*)_tmp2A1;goto
_LL1A3;}_LL1A6: {struct Cyc_Absyn_Fn_d_struct*_tmp28C=(struct Cyc_Absyn_Fn_d_struct*)
_tmp289;if(_tmp28C->tag != 1)goto _LL1A8;else{_tmp28D=_tmp28C->f1;}}_LL1A7: {
struct _tuple0 _tmp2A3;struct _dyneither_ptr*_tmp2A4;struct _tuple0*_tmp2A2=_tmp28D->name;
_tmp2A3=*_tmp2A2;_tmp2A4=_tmp2A3.f2;name=(struct _dyneither_ptr*)_tmp2A4;goto
_LL1A3;}_LL1A8: {struct Cyc_Absyn_Aggr_d_struct*_tmp28E=(struct Cyc_Absyn_Aggr_d_struct*)
_tmp289;if(_tmp28E->tag != 6)goto _LL1AA;else{_tmp28F=_tmp28E->f1;}}_LL1A9: {
struct _tuple0 _tmp2A6;struct _dyneither_ptr*_tmp2A7;struct _tuple0*_tmp2A5=_tmp28F->name;
_tmp2A6=*_tmp2A5;_tmp2A7=_tmp2A6.f2;name=(struct _dyneither_ptr*)_tmp2A7;goto
_LL1A3;}_LL1AA: {struct Cyc_Absyn_Enum_d_struct*_tmp290=(struct Cyc_Absyn_Enum_d_struct*)
_tmp289;if(_tmp290->tag != 8)goto _LL1AC;else{_tmp291=_tmp290->f1;}}_LL1AB: {
struct _tuple0 _tmp2A9;struct _dyneither_ptr*_tmp2AA;struct _tuple0*_tmp2A8=_tmp291->name;
_tmp2A9=*_tmp2A8;_tmp2AA=_tmp2A9.f2;name=(struct _dyneither_ptr*)_tmp2AA;goto
_LL1A3;}_LL1AC: {struct Cyc_Absyn_Typedef_d_struct*_tmp292=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp289;if(_tmp292->tag != 9)goto _LL1AE;else{_tmp293=_tmp292->f1;}}_LL1AD: {
struct _tuple0 _tmp2AC;struct _dyneither_ptr*_tmp2AD;struct _tuple0*_tmp2AB=_tmp293->name;
_tmp2AC=*_tmp2AB;_tmp2AD=_tmp2AC.f2;name=(struct _dyneither_ptr*)_tmp2AD;goto
_LL1A3;}_LL1AE: {struct Cyc_Absyn_Region_d_struct*_tmp294=(struct Cyc_Absyn_Region_d_struct*)
_tmp289;if(_tmp294->tag != 4)goto _LL1B0;}_LL1AF: goto _LL1B1;_LL1B0: {struct Cyc_Absyn_Alias_d_struct*
_tmp295=(struct Cyc_Absyn_Alias_d_struct*)_tmp289;if(_tmp295->tag != 5)goto _LL1B2;}
_LL1B1: goto _LL1B3;_LL1B2: {struct Cyc_Absyn_Porton_d_struct*_tmp296=(struct Cyc_Absyn_Porton_d_struct*)
_tmp289;if(_tmp296->tag != 14)goto _LL1B4;}_LL1B3: goto _LL1B5;_LL1B4: {struct Cyc_Absyn_Portoff_d_struct*
_tmp297=(struct Cyc_Absyn_Portoff_d_struct*)_tmp289;if(_tmp297->tag != 15)goto
_LL1B6;}_LL1B5: goto _LL1B7;_LL1B6: {struct Cyc_Absyn_Let_d_struct*_tmp298=(struct
Cyc_Absyn_Let_d_struct*)_tmp289;if(_tmp298->tag != 2)goto _LL1B8;}_LL1B7: goto
_LL1B9;_LL1B8: {struct Cyc_Absyn_Datatype_d_struct*_tmp299=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp289;if(_tmp299->tag != 7)goto _LL1BA;}_LL1B9: goto _LL1BB;_LL1BA: {struct Cyc_Absyn_Letv_d_struct*
_tmp29A=(struct Cyc_Absyn_Letv_d_struct*)_tmp289;if(_tmp29A->tag != 3)goto _LL1BC;}
_LL1BB: goto _LL1BD;_LL1BC: {struct Cyc_Absyn_Namespace_d_struct*_tmp29B=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp289;if(_tmp29B->tag != 10)goto _LL1BE;}_LL1BD: goto _LL1BF;_LL1BE: {struct Cyc_Absyn_Using_d_struct*
_tmp29C=(struct Cyc_Absyn_Using_d_struct*)_tmp289;if(_tmp29C->tag != 11)goto _LL1C0;}
_LL1BF: goto _LL1C1;_LL1C0: {struct Cyc_Absyn_ExternC_d_struct*_tmp29D=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp289;if(_tmp29D->tag != 12)goto _LL1C2;}_LL1C1: goto _LL1C3;_LL1C2: {struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp29E=(struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp289;if(_tmp29E->tag != 13)
goto _LL1A3;}_LL1C3: name=0;goto _LL1A3;_LL1A3:;}if(!((unsigned int)name) && !
_tmp288)continue;{struct Cyc_List_List*_tmp5D2;_tmp284=((_tmp5D2=_cycalloc(
sizeof(*_tmp5D2)),((_tmp5D2->hd=_tmp287,((_tmp5D2->tl=_tmp284,_tmp5D2))))));}{
struct Cyc_List_List*_tmp5D3;_tmp285=((_tmp5D3=_cycalloc(sizeof(*_tmp5D3)),((
_tmp5D3->hd=name,((_tmp5D3->tl=_tmp285,_tmp5D3))))));}}}{struct _RegionHandle
_tmp2B0=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp2B0;_push_region(
tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp2B1=Cyc_Tcenv_tc_init(tc_rgn);Cyc_Tc_tc(tc_rgn,
_tmp2B1,1,_tmp284);}{struct Cyc_List_List*_tmp2B4;struct Cyc_List_List*_tmp2B5;
struct _tuple23 _tmp5D4;struct _tuple23 _tmp2B3=(_tmp5D4.f1=_tmp284,((_tmp5D4.f2=
_tmp285,_tmp5D4)));_tmp2B4=_tmp2B3.f1;_tmp2B5=_tmp2B3.f2;for(0;_tmp2B4 != 0  && 
_tmp2B5 != 0;(_tmp2B4=_tmp2B4->tl,_tmp2B5=_tmp2B5->tl)){struct Cyc_Absyn_Decl*
_tmp2B6=(struct Cyc_Absyn_Decl*)_tmp2B4->hd;struct _dyneither_ptr*_tmp2B7=(struct
_dyneither_ptr*)_tmp2B5->hd;int _tmp2B8=0;if(!((unsigned int)_tmp2B7))_tmp2B8=1;
if((unsigned int)_tmp2B7){{const char*_tmp5D8;void*_tmp5D7[1];struct Cyc_String_pa_struct
_tmp5D6;ifdefmacro=((_tmp5D6.tag=0,((_tmp5D6.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmp2B7),((_tmp5D7[0]=& _tmp5D6,Cyc_aprintf(((_tmp5D8="_%s_def_",
_tag_dyneither(_tmp5D8,sizeof(char),9))),_tag_dyneither(_tmp5D7,sizeof(void*),1))))))));}{
const char*_tmp5DC;void*_tmp5DB[1];struct Cyc_String_pa_struct _tmp5DA;(_tmp5DA.tag=
0,((_tmp5DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((
_tmp5DB[0]=& _tmp5DA,Cyc_fprintf(out_file,((_tmp5DC="#ifndef %s\n",_tag_dyneither(
_tmp5DC,sizeof(char),12))),_tag_dyneither(_tmp5DB,sizeof(void*),1)))))));}{const
char*_tmp5E0;void*_tmp5DF[1];struct Cyc_String_pa_struct _tmp5DE;(_tmp5DE.tag=0,((
_tmp5DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro),((_tmp5DF[0]=&
_tmp5DE,Cyc_fprintf(out_file,((_tmp5E0="#define %s\n",_tag_dyneither(_tmp5E0,
sizeof(char),12))),_tag_dyneither(_tmp5DF,sizeof(void*),1)))))));}{struct Cyc_Absyn_Decl*
_tmp5E1[1];Cyc_Absynpp_decllist2file(((_tmp5E1[0]=_tmp2B6,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5E1,sizeof(struct Cyc_Absyn_Decl*),
1)))),out_file);}{const char*_tmp5E4;void*_tmp5E3;(_tmp5E3=0,Cyc_fprintf(out_file,((
_tmp5E4="#endif\n",_tag_dyneither(_tmp5E4,sizeof(char),8))),_tag_dyneither(
_tmp5E3,sizeof(void*),0)));}}else{struct Cyc_Absyn_Decl*_tmp5E5[1];Cyc_Absynpp_decllist2file(((
_tmp5E5[0]=_tmp2B6,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5E5,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}}maybe=
Cyc_fopen(_tmp1E6,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp5F6;const char*_tmp5F5;const char*_tmp5F4;void*_tmp5F3[1];struct Cyc_String_pa_struct
_tmp5F2;struct Cyc_NO_SUPPORT_struct*_tmp5F1;(int)_throw((void*)((_tmp5F1=
_cycalloc(sizeof(*_tmp5F1)),((_tmp5F1[0]=((_tmp5F6.tag=Cyc_NO_SUPPORT,((_tmp5F6.f1=(
struct _dyneither_ptr)((_tmp5F2.tag=0,((_tmp5F2.f1=(struct _dyneither_ptr)((
_tmp5F5=_tmp1E6,_tag_dyneither(_tmp5F5,sizeof(char),_get_zero_arr_size_char(
_tmp5F5,1)))),((_tmp5F3[0]=& _tmp5F2,Cyc_aprintf(((_tmp5F4="can't open macrosfile %s",
_tag_dyneither(_tmp5F4,sizeof(char),25))),_tag_dyneither(_tmp5F3,sizeof(void*),1)))))))),
_tmp5F6)))),_tmp5F1)))));}in_file=(struct Cyc___cycFILE*)maybe;_tmp22C=Cyc_Lexing_from_file(
in_file);{struct _tuple18*entry2;while((entry2=((struct _tuple18*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp22C))!= 0){struct _tuple18 _tmp2CD;struct _dyneither_ptr
_tmp2CE;struct _dyneither_ptr*_tmp2CF;struct _tuple18*_tmp2CC=(struct _tuple18*)
_check_null(entry2);_tmp2CD=*_tmp2CC;_tmp2CE=_tmp2CD.f1;_tmp2CF=_tmp2CD.f2;if(((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp247,
_tmp2CF)){{const char*_tmp5FA;void*_tmp5F9[1];struct Cyc_String_pa_struct _tmp5F8;(
_tmp5F8.tag=0,((_tmp5F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CF),((
_tmp5F9[0]=& _tmp5F8,Cyc_fprintf(out_file,((_tmp5FA="#ifndef %s\n",_tag_dyneither(
_tmp5FA,sizeof(char),12))),_tag_dyneither(_tmp5F9,sizeof(void*),1)))))));}{const
char*_tmp5FE;void*_tmp5FD[1];struct Cyc_String_pa_struct _tmp5FC;(_tmp5FC.tag=0,((
_tmp5FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2CE),((_tmp5FD[0]=&
_tmp5FC,Cyc_fprintf(out_file,((_tmp5FE="%s\n",_tag_dyneither(_tmp5FE,sizeof(char),
4))),_tag_dyneither(_tmp5FD,sizeof(void*),1)))))));}{const char*_tmp601;void*
_tmp600;(_tmp600=0,Cyc_fprintf(out_file,((_tmp601="#endif\n",_tag_dyneither(
_tmp601,sizeof(char),8))),_tag_dyneither(_tmp600,sizeof(void*),0)));}}}Cyc_fclose(
in_file);if(Cyc_mode != Cyc_FINISH)remove(_tmp1E6);if(hstubs != 0){struct Cyc_List_List*
_tmp2D8=hstubs;for(0;_tmp2D8 != 0;_tmp2D8=_tmp2D8->tl){struct _tuple19 _tmp2DA;
struct _dyneither_ptr _tmp2DB;struct _dyneither_ptr _tmp2DC;struct _tuple19*_tmp2D9=(
struct _tuple19*)_tmp2D8->hd;_tmp2DA=*_tmp2D9;_tmp2DB=_tmp2DA.f1;_tmp2DC=_tmp2DA.f2;{
struct _dyneither_ptr*_tmp602;if(_tmp2DC.curr != (_tag_dyneither(0,0,0)).curr  && (
_tmp2DB.curr == (_tag_dyneither(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,
struct _dyneither_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp602=_cycalloc(
sizeof(*_tmp602)),((_tmp602[0]=_tmp2DB,_tmp602)))))))Cyc_fputs((const char*)
_untag_dyneither_ptr(_tmp2DC,sizeof(char),1),out_file);else{const char*_tmp606;
void*_tmp605[1];struct Cyc_String_pa_struct _tmp604;(_tmp604.tag=0,((_tmp604.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2DB),((_tmp605[0]=& _tmp604,Cyc_log(((
_tmp606="%s is not supported on this platform\n",_tag_dyneither(_tmp606,sizeof(
char),38))),_tag_dyneither(_tmp605,sizeof(void*),1)))))));}}}}{const char*_tmp609;
void*_tmp608;(_tmp608=0,Cyc_fprintf(out_file,((_tmp609="#endif\n",_tag_dyneither(
_tmp609,sizeof(char),8))),_tag_dyneither(_tmp608,sizeof(void*),0)));}if(Cyc_do_setjmp){
int _tmp2E3=0;_npop_handler(1);return _tmp2E3;}else{Cyc_fclose(out_file);}if(
cstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{struct
Cyc_List_List*_tmp2E4=cstubs;for(0;_tmp2E4 != 0;_tmp2E4=_tmp2E4->tl){struct
_tuple19 _tmp2E6;struct _dyneither_ptr _tmp2E7;struct _dyneither_ptr _tmp2E8;struct
_tuple19*_tmp2E5=(struct _tuple19*)_tmp2E4->hd;_tmp2E6=*_tmp2E5;_tmp2E7=_tmp2E6.f1;
_tmp2E8=_tmp2E6.f2;{struct _dyneither_ptr*_tmp60A;if(_tmp2E8.curr != (
_tag_dyneither(0,0,0)).curr  && (_tmp2E7.curr == (_tag_dyneither(0,0,0)).curr  || ((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
defined_symbols,((_tmp60A=_cycalloc(sizeof(*_tmp60A)),((_tmp60A[0]=_tmp2E7,
_tmp60A)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2E8,sizeof(char),1),
out_file);}}}}out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
const char*_tmp610;const char*_tmp60F;void*_tmp60E[1];struct Cyc_String_pa_struct
_tmp60D;(_tmp60D.tag=0,((_tmp60D.f1=(struct _dyneither_ptr)((_tmp610=filename,
_tag_dyneither(_tmp610,sizeof(char),_get_zero_arr_size_char(_tmp610,1)))),((
_tmp60E[0]=& _tmp60D,Cyc_fprintf(out_file,((_tmp60F="#include <%s>\n\n",
_tag_dyneither(_tmp60F,sizeof(char),16))),_tag_dyneither(_tmp60E,sizeof(void*),1)))))));}
if(cycstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
struct Cyc_List_List*_tmp2EE=cycstubs;for(0;_tmp2EE != 0;_tmp2EE=_tmp2EE->tl){
struct _tuple19 _tmp2F0;struct _dyneither_ptr _tmp2F1;struct _dyneither_ptr _tmp2F2;
struct _tuple19*_tmp2EF=(struct _tuple19*)_tmp2EE->hd;_tmp2F0=*_tmp2EF;_tmp2F1=
_tmp2F0.f1;_tmp2F2=_tmp2F0.f2;{struct _dyneither_ptr*_tmp611;if(_tmp2F2.curr != (
_tag_dyneither(0,0,0)).curr  && (_tmp2F1.curr == (_tag_dyneither(0,0,0)).curr  || ((
int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(
defined_symbols,((_tmp611=_cycalloc(sizeof(*_tmp611)),((_tmp611[0]=_tmp2F1,
_tmp611)))))))Cyc_fputs((const char*)_untag_dyneither_ptr(_tmp2F2,sizeof(char),1),
out_file);}}}{const char*_tmp614;void*_tmp613;(_tmp613=0,Cyc_fprintf(out_file,((
_tmp614="\n",_tag_dyneither(_tmp614,sizeof(char),2))),_tag_dyneither(_tmp613,
sizeof(void*),0)));}}{int _tmp2F6=0;_npop_handler(1);return _tmp2F6;}};_pop_region(
tc_rgn);}}}}}}};_pop_handler();}else{void*_tmp1EA=(void*)_exn_thrown;void*
_tmp2FC=_tmp1EA;struct _dyneither_ptr _tmp2FE;_LL1C5: {struct Cyc_NO_SUPPORT_struct*
_tmp2FD=(struct Cyc_NO_SUPPORT_struct*)_tmp2FC;if(_tmp2FD->tag != Cyc_NO_SUPPORT)
goto _LL1C7;else{_tmp2FE=_tmp2FD->f1;}}_LL1C6:{const char*_tmp618;void*_tmp617[1];
struct Cyc_String_pa_struct _tmp616;(_tmp616.tag=0,((_tmp616.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp2FE),((_tmp617[0]=& _tmp616,Cyc_fprintf(
Cyc_stderr,((_tmp618="No support because %s\n",_tag_dyneither(_tmp618,sizeof(
char),23))),_tag_dyneither(_tmp617,sizeof(void*),1)))))));}goto _LL1C8;_LL1C7:;
_LL1C8: maybe=Cyc_fopen(filename,(const char*)"w");if(!((unsigned int)maybe)){{
const char*_tmp61E;const char*_tmp61D;void*_tmp61C[1];struct Cyc_String_pa_struct
_tmp61B;(_tmp61B.tag=0,((_tmp61B.f1=(struct _dyneither_ptr)((_tmp61E=filename,
_tag_dyneither(_tmp61E,sizeof(char),_get_zero_arr_size_char(_tmp61E,1)))),((
_tmp61C[0]=& _tmp61B,Cyc_fprintf(Cyc_stderr,((_tmp61D="Error: could not create file %s\n",
_tag_dyneither(_tmp61D,sizeof(char),33))),_tag_dyneither(_tmp61C,sizeof(void*),1)))))));}
return 1;}out_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp624;const char*
_tmp623;void*_tmp622[1];struct Cyc_String_pa_struct _tmp621;(_tmp621.tag=0,((
_tmp621.f1=(struct _dyneither_ptr)((_tmp624=filename,_tag_dyneither(_tmp624,
sizeof(char),_get_zero_arr_size_char(_tmp624,1)))),((_tmp622[0]=& _tmp621,Cyc_fprintf(
out_file,((_tmp623="#error -- %s is not supported on this platform\n",
_tag_dyneither(_tmp623,sizeof(char),48))),_tag_dyneither(_tmp622,sizeof(void*),1)))))));}
Cyc_fclose(out_file);{const char*_tmp62A;const char*_tmp629;void*_tmp628[1];struct
Cyc_String_pa_struct _tmp627;(_tmp627.tag=0,((_tmp627.f1=(struct _dyneither_ptr)((
_tmp62A=filename,_tag_dyneither(_tmp62A,sizeof(char),_get_zero_arr_size_char(
_tmp62A,1)))),((_tmp628[0]=& _tmp627,Cyc_fprintf(Cyc_stderr,((_tmp629="Warning: %s will not be supported on this platform\n",
_tag_dyneither(_tmp629,sizeof(char),52))),_tag_dyneither(_tmp628,sizeof(void*),1)))))));}{
const char*_tmp62D;void*_tmp62C;(_tmp62C=0,Cyc_log(((_tmp62D="Not supported on this platform\n",
_tag_dyneither(_tmp62D,sizeof(char),32))),_tag_dyneither(_tmp62C,sizeof(void*),0)));}
remove(_tmp1E6);remove(_tmp1E7);remove(_tmp1E8);return 0;_LL1C9:;_LL1CA:(void)
_throw(_tmp2FC);_LL1C4:;}}}}int Cyc_process_specfile(const char*file,const char*dir);
static void _tmp638(unsigned int*_tmp637,unsigned int*_tmp636,char**_tmp635){for(*
_tmp637=0;*_tmp637 < *_tmp636;(*_tmp637)++){(*_tmp635)[*_tmp637]='\000';}}int Cyc_process_specfile(
const char*file,const char*dir){struct Cyc___cycFILE*_tmp325=Cyc_fopen(file,(const
char*)"r");if(!((unsigned int)_tmp325)){{const char*_tmp633;const char*_tmp632;
void*_tmp631[1];struct Cyc_String_pa_struct _tmp630;(_tmp630.tag=0,((_tmp630.f1=(
struct _dyneither_ptr)((_tmp633=file,_tag_dyneither(_tmp633,sizeof(char),
_get_zero_arr_size_char(_tmp633,1)))),((_tmp631[0]=& _tmp630,Cyc_fprintf(Cyc_stderr,((
_tmp632="Error: could not open %s\n",_tag_dyneither(_tmp632,sizeof(char),26))),
_tag_dyneither(_tmp631,sizeof(void*),1)))))));}return 1;}{struct Cyc___cycFILE*
_tmp32A=(struct Cyc___cycFILE*)_tmp325;char*_tmp63D;unsigned int _tmp63C;char*
_tmp63B;unsigned int _tmp63A;unsigned int _tmp639;struct _dyneither_ptr buf=(_tmp63D=((
_tmp63C=(unsigned int)1024,((_tmp63B=(char*)_cycalloc_atomic(_check_times(
sizeof(char),_tmp63C + 1)),((((_tmp63A=_tmp63C,((_tmp638(& _tmp639,& _tmp63A,&
_tmp63B),_tmp63B[_tmp63A]=(char)0)))),_tmp63B)))))),_tag_dyneither(_tmp63D,
sizeof(char),_get_zero_arr_size_char(_tmp63D,(unsigned int)1024 + 1)));struct
_dyneither_ptr _tmp32B=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));if(
Cyc_mode != Cyc_GATHERSCRIPT){if(chdir(dir)){{const char*_tmp643;const char*_tmp642;
void*_tmp641[1];struct Cyc_String_pa_struct _tmp640;(_tmp640.tag=0,((_tmp640.f1=(
struct _dyneither_ptr)((_tmp643=dir,_tag_dyneither(_tmp643,sizeof(char),
_get_zero_arr_size_char(_tmp643,1)))),((_tmp641[0]=& _tmp640,Cyc_fprintf(Cyc_stderr,((
_tmp642="Error: can't change directory to %s\n",_tag_dyneither(_tmp642,sizeof(
char),37))),_tag_dyneither(_tmp641,sizeof(void*),1)))))));}return 1;}}if(Cyc_mode
== Cyc_GATHER){struct _dyneither_ptr _tmp330=Cstring_to_string(Ccomp);const char*
_tmp647;void*_tmp646[1];struct Cyc_String_pa_struct _tmp645;system((const char*)
_untag_dyneither_ptr(((_tmp645.tag=0,((_tmp645.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)_tmp330),((_tmp646[0]=& _tmp645,Cyc_aprintf(((_tmp647="echo | %s -E -dM - -o INITMACROS.h\n",
_tag_dyneither(_tmp647,sizeof(char),36))),_tag_dyneither(_tmp646,sizeof(void*),1)))))))),
sizeof(char),1));}{struct Cyc_Lexing_lexbuf*_tmp334=Cyc_Lexing_from_file(_tmp32A);
struct _tuple20*entry;while((entry=((struct _tuple20*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_spec)(_tmp334))!= 0){struct _tuple20 _tmp336;struct _dyneither_ptr
_tmp337;struct Cyc_List_List*_tmp338;struct Cyc_List_List*_tmp339;struct Cyc_List_List*
_tmp33A;struct Cyc_List_List*_tmp33B;struct Cyc_List_List*_tmp33C;struct _tuple20*
_tmp335=(struct _tuple20*)_check_null(entry);_tmp336=*_tmp335;_tmp337=_tmp336.f1;
_tmp338=_tmp336.f2;_tmp339=_tmp336.f3;_tmp33A=_tmp336.f4;_tmp33B=_tmp336.f5;
_tmp33C=_tmp336.f6;if(Cyc_process_file((const char*)_untag_dyneither_ptr(_tmp337,
sizeof(char),1),_tmp338,_tmp339,_tmp33A,_tmp33B,_tmp33C))return 1;}Cyc_fclose(
_tmp32A);if(Cyc_mode != Cyc_GATHERSCRIPT){if(chdir((const char*)((char*)
_untag_dyneither_ptr(_tmp32B,sizeof(char),1)))){{const char*_tmp64B;void*_tmp64A[
1];struct Cyc_String_pa_struct _tmp649;(_tmp649.tag=0,((_tmp649.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp32B),((_tmp64A[0]=& _tmp649,Cyc_fprintf(
Cyc_stderr,((_tmp64B="Error: could not change directory to %s\n",_tag_dyneither(
_tmp64B,sizeof(char),41))),_tag_dyneither(_tmp64A,sizeof(void*),1)))))));}return
1;}}return 0;}}}int Cyc_process_setjmp(const char*dir);static void _tmp650(
unsigned int*_tmp64F,unsigned int*_tmp64E,char**_tmp64D){for(*_tmp64F=0;*_tmp64F
< *_tmp64E;(*_tmp64F)++){(*_tmp64D)[*_tmp64F]='\000';}}int Cyc_process_setjmp(
const char*dir){char*_tmp655;unsigned int _tmp654;char*_tmp653;unsigned int _tmp652;
unsigned int _tmp651;struct _dyneither_ptr buf=(_tmp655=((_tmp654=(unsigned int)
1024,((_tmp653=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp654 + 1)),((((
_tmp652=_tmp654,((_tmp650(& _tmp651,& _tmp652,& _tmp653),_tmp653[_tmp652]=(char)0)))),
_tmp653)))))),_tag_dyneither(_tmp655,sizeof(char),_get_zero_arr_size_char(
_tmp655,(unsigned int)1024 + 1)));struct _dyneither_ptr _tmp344=Cyc_getcwd(buf,
_get_dyneither_size(buf,sizeof(char)));if(chdir(dir)){{const char*_tmp65B;const
char*_tmp65A;void*_tmp659[1];struct Cyc_String_pa_struct _tmp658;(_tmp658.tag=0,((
_tmp658.f1=(struct _dyneither_ptr)((_tmp65B=dir,_tag_dyneither(_tmp65B,sizeof(
char),_get_zero_arr_size_char(_tmp65B,1)))),((_tmp659[0]=& _tmp658,Cyc_fprintf(
Cyc_stderr,((_tmp65A="Error: can't change directory to %s\n",_tag_dyneither(
_tmp65A,sizeof(char),37))),_tag_dyneither(_tmp659,sizeof(void*),1)))))));}return
1;}{struct _tuple19*_tmp665;const char*_tmp664;const char*_tmp663;struct _tuple19*
_tmp662[1];struct _dyneither_ptr*_tmp65C[1];if(Cyc_process_file((const char*)"setjmp.h",((
_tmp65C[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"jmp_buf",
sizeof(char),8),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp65C,sizeof(struct _dyneither_ptr*),1)))),0,((_tmp662[0]=((
_tmp665=_cycalloc(sizeof(*_tmp665)),((_tmp665->f1=((_tmp663="setjmp",
_tag_dyneither(_tmp663,sizeof(char),7))),((_tmp665->f2=((_tmp664="extern int setjmp(jmp_buf);\n",
_tag_dyneither(_tmp664,sizeof(char),29))),_tmp665)))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp662,sizeof(struct
_tuple19*),1)))),0,0))return 1;}if(chdir((const char*)((char*)_untag_dyneither_ptr(
_tmp344,sizeof(char),1)))){{const char*_tmp669;void*_tmp668[1];struct Cyc_String_pa_struct
_tmp667;(_tmp667.tag=0,((_tmp667.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp344),((_tmp668[0]=& _tmp667,Cyc_fprintf(Cyc_stderr,((_tmp669="Error: could not change directory to %s\n",
_tag_dyneither(_tmp669,sizeof(char),41))),_tag_dyneither(_tmp668,sizeof(void*),1)))))));}
return 1;}return 0;}static char _tmp356[13]="BUILDLIB.OUT";static struct
_dyneither_ptr Cyc_output_dir={_tmp356,_tmp356,_tmp356 + 13};static void Cyc_set_output_dir(
struct _dyneither_ptr s);static void Cyc_set_output_dir(struct _dyneither_ptr s){Cyc_output_dir=
s;}static struct Cyc_List_List*Cyc_spec_files=0;static void Cyc_add_spec_file(struct
_dyneither_ptr s);static void Cyc_add_spec_file(struct _dyneither_ptr s){struct Cyc_List_List*
_tmp66A;Cyc_spec_files=((_tmp66A=_cycalloc(sizeof(*_tmp66A)),((_tmp66A->hd=(
const char*)_untag_dyneither_ptr(s,sizeof(char),1),((_tmp66A->tl=Cyc_spec_files,
_tmp66A))))));}static void Cyc_set_GATHER();static void Cyc_set_GATHER(){Cyc_mode=
Cyc_GATHER;}static void Cyc_set_GATHERSCRIPT();static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}static void Cyc_set_FINISH();static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}static int Cyc_badparse=0;static void Cyc_unsupported_option(
struct _dyneither_ptr s);static void Cyc_unsupported_option(struct _dyneither_ptr s){{
const char*_tmp66E;void*_tmp66D[1];struct Cyc_String_pa_struct _tmp66C;(_tmp66C.tag=
0,((_tmp66C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp66D[0]=&
_tmp66C,Cyc_fprintf(Cyc_stderr,((_tmp66E="Unsupported option %s\n",
_tag_dyneither(_tmp66E,sizeof(char),23))),_tag_dyneither(_tmp66D,sizeof(void*),1)))))));}
Cyc_badparse=1;}void GC_blacklist_warn_clear();struct _tuple24{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv);int Cyc_main(int argc,struct
_dyneither_ptr argv){GC_blacklist_warn_clear();{struct _tuple24*_tmp6DB;const char*
_tmp6DA;struct Cyc_Arg_Flag_spec_struct _tmp6D9;struct Cyc_Arg_Flag_spec_struct*
_tmp6D8;const char*_tmp6D7;const char*_tmp6D6;struct _tuple24*_tmp6D5;const char*
_tmp6D4;struct Cyc_Arg_Set_spec_struct _tmp6D3;struct Cyc_Arg_Set_spec_struct*
_tmp6D2;const char*_tmp6D1;const char*_tmp6D0;struct _tuple24*_tmp6CF;const char*
_tmp6CE;struct Cyc_Arg_Unit_spec_struct _tmp6CD;struct Cyc_Arg_Unit_spec_struct*
_tmp6CC;const char*_tmp6CB;const char*_tmp6CA;struct _tuple24*_tmp6C9;const char*
_tmp6C8;struct Cyc_Arg_Unit_spec_struct _tmp6C7;struct Cyc_Arg_Unit_spec_struct*
_tmp6C6;const char*_tmp6C5;const char*_tmp6C4;struct _tuple24*_tmp6C3;const char*
_tmp6C2;struct Cyc_Arg_Unit_spec_struct _tmp6C1;struct Cyc_Arg_Unit_spec_struct*
_tmp6C0;const char*_tmp6BF;const char*_tmp6BE;struct _tuple24*_tmp6BD;const char*
_tmp6BC;struct Cyc_Arg_String_spec_struct _tmp6BB;struct Cyc_Arg_String_spec_struct*
_tmp6BA;const char*_tmp6B9;const char*_tmp6B8;struct _tuple24*_tmp6B7[6];struct Cyc_List_List*
options=(_tmp6B7[5]=((_tmp6DB=_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB->f1=((
_tmp6D6="-",_tag_dyneither(_tmp6D6,sizeof(char),2))),((_tmp6DB->f2=1,((_tmp6DB->f3=((
_tmp6D7="",_tag_dyneither(_tmp6D7,sizeof(char),1))),((_tmp6DB->f4=(void*)((
_tmp6D8=_cycalloc(sizeof(*_tmp6D8)),((_tmp6D8[0]=((_tmp6D9.tag=1,((_tmp6D9.f1=
Cyc_unsupported_option,_tmp6D9)))),_tmp6D8)))),((_tmp6DB->f5=((_tmp6DA="",
_tag_dyneither(_tmp6DA,sizeof(char),1))),_tmp6DB)))))))))))),((_tmp6B7[4]=((
_tmp6D5=_cycalloc(sizeof(*_tmp6D5)),((_tmp6D5->f1=((_tmp6D0="-setjmp",
_tag_dyneither(_tmp6D0,sizeof(char),8))),((_tmp6D5->f2=0,((_tmp6D5->f3=((_tmp6D1="",
_tag_dyneither(_tmp6D1,sizeof(char),1))),((_tmp6D5->f4=(void*)((_tmp6D2=
_cycalloc(sizeof(*_tmp6D2)),((_tmp6D2[0]=((_tmp6D3.tag=3,((_tmp6D3.f1=& Cyc_do_setjmp,
_tmp6D3)))),_tmp6D2)))),((_tmp6D5->f5=((_tmp6D4="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",
_tag_dyneither(_tmp6D4,sizeof(char),186))),_tmp6D5)))))))))))),((_tmp6B7[3]=((
_tmp6CF=_cycalloc(sizeof(*_tmp6CF)),((_tmp6CF->f1=((_tmp6CA="-finish",
_tag_dyneither(_tmp6CA,sizeof(char),8))),((_tmp6CF->f2=0,((_tmp6CF->f3=((_tmp6CB="",
_tag_dyneither(_tmp6CB,sizeof(char),1))),((_tmp6CF->f4=(void*)((_tmp6CC=
_cycalloc(sizeof(*_tmp6CC)),((_tmp6CC[0]=((_tmp6CD.tag=0,((_tmp6CD.f1=Cyc_set_FINISH,
_tmp6CD)))),_tmp6CC)))),((_tmp6CF->f5=((_tmp6CE="Produce Cyclone headers from pre-gathered C library info",
_tag_dyneither(_tmp6CE,sizeof(char),57))),_tmp6CF)))))))))))),((_tmp6B7[2]=((
_tmp6C9=_cycalloc(sizeof(*_tmp6C9)),((_tmp6C9->f1=((_tmp6C4="-gatherscript",
_tag_dyneither(_tmp6C4,sizeof(char),14))),((_tmp6C9->f2=0,((_tmp6C9->f3=((
_tmp6C5="",_tag_dyneither(_tmp6C5,sizeof(char),1))),((_tmp6C9->f4=(void*)((
_tmp6C6=_cycalloc(sizeof(*_tmp6C6)),((_tmp6C6[0]=((_tmp6C7.tag=0,((_tmp6C7.f1=
Cyc_set_GATHERSCRIPT,_tmp6C7)))),_tmp6C6)))),((_tmp6C9->f5=((_tmp6C8="Produce a script to gather C library info",
_tag_dyneither(_tmp6C8,sizeof(char),42))),_tmp6C9)))))))))))),((_tmp6B7[1]=((
_tmp6C3=_cycalloc(sizeof(*_tmp6C3)),((_tmp6C3->f1=((_tmp6BE="-gather",
_tag_dyneither(_tmp6BE,sizeof(char),8))),((_tmp6C3->f2=0,((_tmp6C3->f3=((_tmp6BF="",
_tag_dyneither(_tmp6BF,sizeof(char),1))),((_tmp6C3->f4=(void*)((_tmp6C0=
_cycalloc(sizeof(*_tmp6C0)),((_tmp6C0[0]=((_tmp6C1.tag=0,((_tmp6C1.f1=Cyc_set_GATHER,
_tmp6C1)))),_tmp6C0)))),((_tmp6C3->f5=((_tmp6C2="Gather C library info but don't produce Cyclone headers",
_tag_dyneither(_tmp6C2,sizeof(char),56))),_tmp6C3)))))))))))),((_tmp6B7[0]=((
_tmp6BD=_cycalloc(sizeof(*_tmp6BD)),((_tmp6BD->f1=((_tmp6B8="-d",_tag_dyneither(
_tmp6B8,sizeof(char),3))),((_tmp6BD->f2=0,((_tmp6BD->f3=((_tmp6B9=" <file>",
_tag_dyneither(_tmp6B9,sizeof(char),8))),((_tmp6BD->f4=(void*)((_tmp6BA=
_cycalloc(sizeof(*_tmp6BA)),((_tmp6BA[0]=((_tmp6BB.tag=5,((_tmp6BB.f1=Cyc_set_output_dir,
_tmp6BB)))),_tmp6BA)))),((_tmp6BD->f5=((_tmp6BC="Set the output directory to <file>",
_tag_dyneither(_tmp6BC,sizeof(char),35))),_tmp6BD)))))))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6B7,sizeof(struct
_tuple24*),6)))))))))))));{const char*_tmp6DC;Cyc_Arg_parse(options,Cyc_add_spec_file,((
_tmp6DC="Options:",_tag_dyneither(_tmp6DC,sizeof(char),9))),argv);}if((((Cyc_badparse
 || !Cyc_do_setjmp  && Cyc_spec_files == 0) || Cyc_do_setjmp  && Cyc_spec_files != 0)
 || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){{
const char*_tmp6DD;Cyc_Arg_usage(options,((_tmp6DD="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",
_tag_dyneither(_tmp6DD,sizeof(char),59))));}return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){
Cyc_script_file=Cyc_fopen((const char*)"BUILDLIB.sh",(const char*)"w");if(!((
unsigned int)Cyc_script_file)){{const char*_tmp6E0;void*_tmp6DF;(_tmp6DF=0,Cyc_fprintf(
Cyc_stderr,((_tmp6E0="Could not create file BUILDLIB.sh\n",_tag_dyneither(
_tmp6E0,sizeof(char),35))),_tag_dyneither(_tmp6DF,sizeof(void*),0)));}exit(1);}{
const char*_tmp6E3;void*_tmp6E2;(_tmp6E2=0,Cyc_prscript(((_tmp6E3="#!/bin/sh\n",
_tag_dyneither(_tmp6E3,sizeof(char),11))),_tag_dyneither(_tmp6E2,sizeof(void*),0)));}{
const char*_tmp6E6;void*_tmp6E5;(_tmp6E5=0,Cyc_prscript(((_tmp6E6="GCC=\"gcc\"\n",
_tag_dyneither(_tmp6E6,sizeof(char),11))),_tag_dyneither(_tmp6E5,sizeof(void*),0)));}}
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){{
const char*_tmp6EA;void*_tmp6E9[1];struct Cyc_String_pa_struct _tmp6E8;(_tmp6E8.tag=
0,((_tmp6E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp6E9[0]=& _tmp6E8,Cyc_fprintf(Cyc_stderr,((_tmp6EA="Error: could not create directory %s\n",
_tag_dyneither(_tmp6EA,sizeof(char),38))),_tag_dyneither(_tmp6E9,sizeof(void*),1)))))));}
return 1;}if(Cyc_mode == Cyc_GATHERSCRIPT){{const char*_tmp6EE;void*_tmp6ED[1];
struct Cyc_String_pa_struct _tmp6EC;(_tmp6EC.tag=0,((_tmp6EC.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((_tmp6ED[0]=& _tmp6EC,Cyc_prscript(((
_tmp6EE="cd %s\n",_tag_dyneither(_tmp6EE,sizeof(char),7))),_tag_dyneither(
_tmp6ED,sizeof(void*),1)))))));}{const char*_tmp6F1;void*_tmp6F0;(_tmp6F0=0,Cyc_prscript(((
_tmp6F1="echo | $GCC -E -dM - -o INITMACROS.h\n",_tag_dyneither(_tmp6F1,sizeof(
char),38))),_tag_dyneither(_tmp6F0,sizeof(void*),0)));}}if(!Cyc_gathering()){{
const char*_tmp6F2;Cyc_log_file=Cyc_fopen((const char*)_untag_dyneither_ptr(Cyc_Filename_concat(
Cyc_output_dir,((_tmp6F2="BUILDLIB.LOG",_tag_dyneither(_tmp6F2,sizeof(char),13)))),
sizeof(char),1),(const char*)"w");}if(!((unsigned int)Cyc_log_file)){{const char*
_tmp6F6;void*_tmp6F5[1];struct Cyc_String_pa_struct _tmp6F4;(_tmp6F4.tag=0,((
_tmp6F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir),((
_tmp6F5[0]=& _tmp6F4,Cyc_fprintf(Cyc_stderr,((_tmp6F6="Error: could not create log file in directory %s\n",
_tag_dyneither(_tmp6F6,sizeof(char),50))),_tag_dyneither(_tmp6F5,sizeof(void*),1)))))));}
return 1;}if(!Cyc_do_setjmp){{const char*_tmp6F7;Cyc_cstubs_file=Cyc_fopen((const
char*)_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp6F7="cstubs.c",
_tag_dyneither(_tmp6F7,sizeof(char),9)))),sizeof(char),1),(const char*)"w");}if(!((
unsigned int)Cyc_cstubs_file)){{const char*_tmp6FB;void*_tmp6FA[1];struct Cyc_String_pa_struct
_tmp6F9;(_tmp6F9.tag=0,((_tmp6F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp6FA[0]=& _tmp6F9,Cyc_fprintf(Cyc_stderr,((_tmp6FB="Error: could not create cstubs.c in directory %s\n",
_tag_dyneither(_tmp6FB,sizeof(char),50))),_tag_dyneither(_tmp6FA,sizeof(void*),1)))))));}
return 1;}{const char*_tmp6FC;Cyc_cycstubs_file=Cyc_fopen((const char*)
_untag_dyneither_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp6FC="cycstubs.cyc",
_tag_dyneither(_tmp6FC,sizeof(char),13)))),sizeof(char),1),(const char*)"w");}if(
!((unsigned int)Cyc_cycstubs_file)){{const char*_tmp700;void*_tmp6FF[1];struct Cyc_String_pa_struct
_tmp6FE;(_tmp6FE.tag=0,((_tmp6FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_output_dir),((_tmp6FF[0]=& _tmp6FE,Cyc_fprintf(Cyc_stderr,((_tmp700="Error: could not create cycstubs.c in directory %s\n",
_tag_dyneither(_tmp700,sizeof(char),52))),_tag_dyneither(_tmp6FF,sizeof(void*),1)))))));}
return 1;}{const char*_tmp703;void*_tmp702;(_tmp702=0,Cyc_fprintf((struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file),((_tmp703="#include <core.h>\nusing Core;\n\n",
_tag_dyneither(_tmp703,sizeof(char),32))),_tag_dyneither(_tmp702,sizeof(void*),0)));}}}{
const char*outdir=(const char*)_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))return 1;else{for(0;Cyc_spec_files
!= 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){if(
Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,
outdir)){{const char*_tmp706;void*_tmp705;(_tmp705=0,Cyc_fprintf(Cyc_stderr,((
_tmp706="FATAL ERROR -- QUIT!\n",_tag_dyneither(_tmp706,sizeof(char),22))),
_tag_dyneither(_tmp705,sizeof(void*),0)));}exit(1);}}}if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));Cyc_fclose((
struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}return 0;}}}
