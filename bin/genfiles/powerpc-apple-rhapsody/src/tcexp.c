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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[16];
extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*
x,int i);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*
r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple1{union Cyc_Absyn_Nmspace_union f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Conref;
struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;
};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*f1;};
struct Cyc_Absyn_No_constr_struct{int tag;};union Cyc_Absyn_Constraint_union{struct
Cyc_Absyn_Eq_constr_struct Eq_constr;struct Cyc_Absyn_Forward_constr_struct
Forward_constr;struct Cyc_Absyn_No_constr_struct No_constr;};struct Cyc_Absyn_Conref{
union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*
name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple1*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*tunion_name;struct
_tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
f1;};union Cyc_Absyn_AggrInfoU_union{struct Cyc_Absyn_UnknownAggr_struct
UnknownAggr;struct Cyc_Absyn_KnownAggr_struct KnownAggr;};struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU_union aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{
int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*
f1;};struct Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};
struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;struct
Cyc_Absyn_Conref*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{int tag;void*f1;char f2;
};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};struct Cyc_Absyn_Int_c_struct{
int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{int tag;void*f1;
long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_String_c_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Null_c_struct{
int tag;};union Cyc_Absyn_Cnst_union{struct Cyc_Absyn_Char_c_struct Char_c;struct Cyc_Absyn_Short_c_struct
Short_c;struct Cyc_Absyn_Int_c_struct Int_c;struct Cyc_Absyn_LongLong_c_struct
LongLong_c;struct Cyc_Absyn_Float_c_struct Float_c;struct Cyc_Absyn_String_c_struct
String_c;struct Cyc_Absyn_Null_c_struct Null_c;};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst_union f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct
_tuple1*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;
};struct Cyc_Absyn_Primop_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct
Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;
struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;
};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple3 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple3 f2;struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple3 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Region_s_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;struct Cyc_Absyn_Stmt*f5;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Alias_s_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;struct Cyc_Absyn_Stmt*f4;};
struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_Char_p_struct{
int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*
Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_schar_typ;
extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_float_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(void*t,void*rgn,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,struct Cyc_Position_Segment*);
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,
struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dyneither_ptr
Cyc_Absynpp_typ2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*,struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_curr_rgn(
struct Cyc_Tcenv_Tenv*);void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,void*rgn);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(
void*t);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*,void*);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);int
Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int
Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);void*Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*,struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*);struct _tuple4{struct Cyc_List_List*f1;struct
_RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*
Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);
int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(void*
t);int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(
void*t);struct _tuple6{int f1;void*f2;};struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_bits_only(void*t);
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct
_tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int
new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*);struct
Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct
Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessNumelts_struct
LessNumelts;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqNumelts_struct
LessEqNumelts;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[
15];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_CfFlowInfo_IsZero[11];extern char Cyc_CfFlowInfo_NotZero[12];struct
Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[
13];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownL_struct{int tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct
Cyc_CfFlowInfo_PlaceL_struct PlaceL;struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;
};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap);static void*Cyc_Tcexp_expr_err(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct
_dyneither_ptr msg,struct _dyneither_ptr ap){{void*_tmp4F8;(_tmp4F8=0,Cyc_Tcutil_terr(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),
_tag_dyneither(_tmp4F8,sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));else{return*topt;}}static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q);static void Cyc_Tcexp_resolve_unknown_id(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct
_handler_cons _tmp1;_push_handler(& _tmp1);{int _tmp3=0;if(setjmp(_tmp1.handler))
_tmp3=1;if(!_tmp3){{struct _RegionHandle _tmp4=_new_region("r");struct
_RegionHandle*r=& _tmp4;_push_region(r);{void*_tmp5=Cyc_Tcenv_lookup_ordinary(r,
te,e->loc,q);void*_tmp6;struct Cyc_Absyn_Enumdecl*_tmp7;struct Cyc_Absyn_Enumfield*
_tmp8;void*_tmp9;struct Cyc_Absyn_Enumfield*_tmpA;struct Cyc_Absyn_Tuniondecl*
_tmpB;struct Cyc_Absyn_Tunionfield*_tmpC;_LL1: if(*((int*)_tmp5)!= 0)goto _LL3;
_tmp6=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;_LL2:{struct Cyc_Absyn_Var_e_struct
_tmp4FB;struct Cyc_Absyn_Var_e_struct*_tmp4FA;(void*)(e->r=(void*)((void*)((
_tmp4FA=_cycalloc(sizeof(*_tmp4FA)),((_tmp4FA[0]=((_tmp4FB.tag=1,((_tmp4FB.f1=q,((
_tmp4FB.f2=(void*)_tmp6,_tmp4FB)))))),_tmp4FA))))));}goto _LL0;_LL3: if(*((int*)
_tmp5)!= 3)goto _LL5;_tmp7=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((
struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f2;_LL4:{struct Cyc_Absyn_Enum_e_struct
_tmp4FE;struct Cyc_Absyn_Enum_e_struct*_tmp4FD;(void*)(e->r=(void*)((void*)((
_tmp4FD=_cycalloc(sizeof(*_tmp4FD)),((_tmp4FD[0]=((_tmp4FE.tag=33,((_tmp4FE.f1=q,((
_tmp4FE.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7,((_tmp4FE.f3=(struct Cyc_Absyn_Enumfield*)
_tmp8,_tmp4FE)))))))),_tmp4FD))))));}goto _LL0;_LL5: if(*((int*)_tmp5)!= 4)goto
_LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;_tmpA=((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:{struct Cyc_Absyn_AnonEnum_e_struct
_tmp501;struct Cyc_Absyn_AnonEnum_e_struct*_tmp500;(void*)(e->r=(void*)((void*)((
_tmp500=_cycalloc(sizeof(*_tmp500)),((_tmp500[0]=((_tmp501.tag=34,((_tmp501.f1=q,((
_tmp501.f2=(void*)_tmp9,((_tmp501.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp501)))))))),
_tmp500))))));}goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f2;_LL8:{struct Cyc_Absyn_Tunion_e_struct
_tmp504;struct Cyc_Absyn_Tunion_e_struct*_tmp503;(void*)(e->r=(void*)((void*)((
_tmp503=_cycalloc(sizeof(*_tmp503)),((_tmp503[0]=((_tmp504.tag=32,((_tmp504.f1=0,((
_tmp504.f2=_tmpB,((_tmp504.f3=_tmpC,_tmp504)))))))),_tmp503))))));}goto _LL0;_LL9:
if(*((int*)_tmp5)!= 1)goto _LL0;_LLA:{const char*_tmp508;void*_tmp507[1];struct Cyc_String_pa_struct
_tmp506;(_tmp506.tag=0,((_tmp506.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp507[0]=& _tmp506,Cyc_Tcutil_terr(e->loc,((
_tmp508="bad occurrence of type name %s",_tag_dyneither(_tmp508,sizeof(char),31))),
_tag_dyneither(_tmp507,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp50B;struct Cyc_Absyn_Var_e_struct*_tmp50A;(void*)(e->r=(void*)((void*)((
_tmp50A=_cycalloc(sizeof(*_tmp50A)),((_tmp50A[0]=((_tmp50B.tag=1,((_tmp50B.f1=q,((
_tmp50B.f2=(void*)((void*)0),_tmp50B)))))),_tmp50A))))));}goto _LL0;_LL0:;};
_pop_region(r);};_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp1B=
_tmp2;_LLC: if(_tmp1B != Cyc_Dict_Absent)goto _LLE;_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp50E;struct Cyc_Absyn_Var_e_struct*_tmp50D;(void*)(e->r=(void*)((void*)((
_tmp50D=_cycalloc(sizeof(*_tmp50D)),((_tmp50D[0]=((_tmp50E.tag=1,((_tmp50E.f1=q,((
_tmp50E.f2=(void*)((void*)0),_tmp50E)))))),_tmp50D))))));}goto _LLB;_LLE:;_LLF:(
void)_throw(_tmp1B);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e);static
struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){struct _tuple8*
_tmp50F;return(_tmp50F=_cycalloc(sizeof(*_tmp50F)),((_tmp50F->f1=0,((_tmp50F->f2=
e,_tmp50F)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es);static void
Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){void*_tmp1F=(void*)e1->r;struct
_tuple1*_tmp20;_LL11: if(*((int*)_tmp1F)!= 2)goto _LL13;_tmp20=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp1F)->f1;_LL12: {struct _handler_cons _tmp21;_push_handler(& _tmp21);{int _tmp23=
0;if(setjmp(_tmp21.handler))_tmp23=1;if(!_tmp23){{struct _RegionHandle _tmp24=
_new_region("r");struct _RegionHandle*r=& _tmp24;_push_region(r);{void*_tmp25=Cyc_Tcenv_lookup_ordinary(
r,te,e1->loc,_tmp20);void*_tmp26;struct Cyc_Absyn_Tuniondecl*_tmp27;struct Cyc_Absyn_Tunionfield*
_tmp28;struct Cyc_Absyn_Aggrdecl*_tmp29;_LL16: if(*((int*)_tmp25)!= 0)goto _LL18;
_tmp26=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp25)->f1;_LL17:{struct Cyc_Absyn_FnCall_e_struct
_tmp512;struct Cyc_Absyn_FnCall_e_struct*_tmp511;(void*)(e->r=(void*)((void*)((
_tmp511=_cycalloc(sizeof(*_tmp511)),((_tmp511[0]=((_tmp512.tag=11,((_tmp512.f1=
e1,((_tmp512.f2=es,((_tmp512.f3=0,_tmp512)))))))),_tmp511))))));}_npop_handler(1);
return;_LL18: if(*((int*)_tmp25)!= 2)goto _LL1A;_tmp27=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp25)->f1;_tmp28=((struct Cyc_Tcenv_TunionRes_struct*)_tmp25)->f2;_LL19: if(
_tmp28->typs == 0){const char*_tmp516;void*_tmp515[1];struct Cyc_String_pa_struct
_tmp514;(_tmp514.tag=0,((_tmp514.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp28->name)),((_tmp515[0]=& _tmp514,Cyc_Tcutil_terr(e->loc,((
_tmp516="%s is a constant, not a function",_tag_dyneither(_tmp516,sizeof(char),
33))),_tag_dyneither(_tmp515,sizeof(void*),1)))))));}{struct Cyc_Absyn_Tunion_e_struct
_tmp519;struct Cyc_Absyn_Tunion_e_struct*_tmp518;(void*)(e->r=(void*)((void*)((
_tmp518=_cycalloc(sizeof(*_tmp518)),((_tmp518[0]=((_tmp519.tag=32,((_tmp519.f1=
es,((_tmp519.f2=_tmp27,((_tmp519.f3=_tmp28,_tmp519)))))))),_tmp518))))));}
_npop_handler(1);return;_LL1A: if(*((int*)_tmp25)!= 1)goto _LL1C;_tmp29=((struct
Cyc_Tcenv_AggrRes_struct*)_tmp25)->f1;_LL1B: {struct Cyc_List_List*_tmp31=((
struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Struct_e_struct
_tmp51C;struct Cyc_Absyn_Struct_e_struct*_tmp51B;(void*)(e->r=(void*)((void*)((
_tmp51B=_cycalloc(sizeof(*_tmp51B)),((_tmp51B[0]=((_tmp51C.tag=30,((_tmp51C.f1=
_tmp29->name,((_tmp51C.f2=0,((_tmp51C.f3=_tmp31,((_tmp51C.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp29,_tmp51C)))))))))),_tmp51B))))));}_npop_handler(1);return;}_LL1C: if(*((int*)
_tmp25)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp25)!= 3)goto _LL15;
_LL1F:{const char*_tmp520;void*_tmp51F[1];struct Cyc_String_pa_struct _tmp51E;(
_tmp51E.tag=0,((_tmp51E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp20)),((_tmp51F[0]=& _tmp51E,Cyc_Tcutil_terr(e->loc,((_tmp520="%s is an enum constructor, not a function",
_tag_dyneither(_tmp520,sizeof(char),42))),_tag_dyneither(_tmp51F,sizeof(void*),1)))))));}
_npop_handler(1);return;_LL15:;};_pop_region(r);};_pop_handler();}else{void*
_tmp22=(void*)_exn_thrown;void*_tmp38=_tmp22;_LL21: if(_tmp38 != Cyc_Dict_Absent)
goto _LL23;_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp523;struct Cyc_Absyn_FnCall_e_struct*
_tmp522;(void*)(e->r=(void*)((void*)((_tmp522=_cycalloc(sizeof(*_tmp522)),((
_tmp522[0]=((_tmp523.tag=11,((_tmp523.f1=e1,((_tmp523.f2=es,((_tmp523.f3=0,
_tmp523)))))))),_tmp522))))));}return;_LL23:;_LL24:(void)_throw(_tmp38);_LL20:;}}}
_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct _tmp526;struct Cyc_Absyn_FnCall_e_struct*
_tmp525;(void*)(e->r=(void*)((void*)((_tmp525=_cycalloc(sizeof(*_tmp525)),((
_tmp525[0]=((_tmp526.tag=11,((_tmp526.f1=e1,((_tmp526.f2=es,((_tmp526.f3=0,
_tmp526)))))))),_tmp525))))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des);static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp529;struct Cyc_Absyn_Array_e_struct*
_tmp528;(void*)(e->r=(void*)((void*)((_tmp528=_cycalloc(sizeof(*_tmp528)),((
_tmp528[0]=((_tmp529.tag=28,((_tmp529.f1=des,_tmp529)))),_tmp528))))));}return;}{
void*t=*topt;void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp40;
union Cyc_Absyn_AggrInfoU_union _tmp41;struct Cyc_Absyn_ArrayInfo _tmp42;void*_tmp43;
struct Cyc_Absyn_Tqual _tmp44;_LL26: if(_tmp3F <= (void*)4)goto _LL2C;if(*((int*)
_tmp3F)!= 10)goto _LL28;_tmp40=((struct Cyc_Absyn_AggrType_struct*)_tmp3F)->f1;
_tmp41=_tmp40.aggr_info;_LL27:{union Cyc_Absyn_AggrInfoU_union _tmp45=_tmp41;
struct Cyc_Absyn_Aggrdecl**_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp47;_LL2F: if((
_tmp45.UnknownAggr).tag != 0)goto _LL31;_LL30: {const char*_tmp52C;void*_tmp52B;(
_tmp52B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp52C="struct type not properly set",_tag_dyneither(_tmp52C,sizeof(char),29))),
_tag_dyneither(_tmp52B,sizeof(void*),0)));}_LL31: if((_tmp45.KnownAggr).tag != 1)
goto _LL2E;_tmp46=(_tmp45.KnownAggr).f1;_tmp47=*_tmp46;_LL32: {struct Cyc_Absyn_Struct_e_struct
_tmp52F;struct Cyc_Absyn_Struct_e_struct*_tmp52E;(void*)(e->r=(void*)((void*)((
_tmp52E=_cycalloc(sizeof(*_tmp52E)),((_tmp52E[0]=((_tmp52F.tag=30,((_tmp52F.f1=
_tmp47->name,((_tmp52F.f2=0,((_tmp52F.f3=des,((_tmp52F.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp47,_tmp52F)))))))))),_tmp52E))))));}_LL2E:;}goto _LL25;_LL28: if(*((int*)
_tmp3F)!= 7)goto _LL2A;_tmp42=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F)->f1;
_tmp43=(void*)_tmp42.elt_type;_tmp44=_tmp42.tq;_LL29:{struct Cyc_Absyn_Array_e_struct
_tmp532;struct Cyc_Absyn_Array_e_struct*_tmp531;(void*)(e->r=(void*)((void*)((
_tmp531=_cycalloc(sizeof(*_tmp531)),((_tmp531[0]=((_tmp532.tag=28,((_tmp532.f1=
des,_tmp532)))),_tmp531))))));}goto _LL25;_LL2A: if(*((int*)_tmp3F)!= 11)goto _LL2C;
_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp535;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp534;(void*)(e->r=(void*)((void*)((_tmp534=_cycalloc(sizeof(*_tmp534)),((
_tmp534[0]=((_tmp535.tag=31,((_tmp535.f1=(void*)t,((_tmp535.f2=des,_tmp535)))))),
_tmp534))))));}goto _LL25;_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp538;
struct Cyc_Absyn_Array_e_struct*_tmp537;(void*)(e->r=(void*)((void*)((_tmp537=
_cycalloc(sizeof(*_tmp537)),((_tmp537[0]=((_tmp538.tag=28,((_tmp538.f1=des,
_tmp538)))),_tmp537))))));}goto _LL25;_LL25:;}}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp53D;void*_tmp53C[2];struct Cyc_String_pa_struct _tmp53B;
struct Cyc_String_pa_struct _tmp53A;(_tmp53A.tag=0,((_tmp53A.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp53B.tag=0,((_tmp53B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp53C[0]=& _tmp53B,((_tmp53C[1]=& _tmp53A,Cyc_Tcutil_terr(
e->loc,((_tmp53D="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp53D,sizeof(char),53))),_tag_dyneither(_tmp53C,sizeof(void*),2)))))))))))));}}
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,union Cyc_Absyn_Cnst_union*c,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcConst(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,union Cyc_Absyn_Cnst_union*
c,struct Cyc_Absyn_Exp*e){void*t;{union Cyc_Absyn_Cnst_union _tmp56=*((union Cyc_Absyn_Cnst_union*)
_check_null(c));void*_tmp57;void*_tmp58;void*_tmp59;void*_tmp5A;void*_tmp5B;void*
_tmp5C;int _tmp5D;struct _dyneither_ptr _tmp5E;_LL34: if((_tmp56.Char_c).tag != 0)
goto _LL36;_tmp57=(_tmp56.Char_c).f1;if((int)_tmp57 != 0)goto _LL36;_LL35: t=Cyc_Absyn_schar_typ;
goto _LL33;_LL36: if((_tmp56.Char_c).tag != 0)goto _LL38;_tmp58=(_tmp56.Char_c).f1;
if((int)_tmp58 != 1)goto _LL38;_LL37: t=Cyc_Absyn_uchar_typ;goto _LL33;_LL38: if((
_tmp56.Char_c).tag != 0)goto _LL3A;_tmp59=(_tmp56.Char_c).f1;if((int)_tmp59 != 2)
goto _LL3A;_LL39: t=Cyc_Absyn_char_typ;goto _LL33;_LL3A: if((_tmp56.Short_c).tag != 1)
goto _LL3C;_tmp5A=(_tmp56.Short_c).f1;_LL3B: t=_tmp5A == (void*)1?Cyc_Absyn_ushort_typ:
Cyc_Absyn_sshort_typ;goto _LL33;_LL3C: if((_tmp56.LongLong_c).tag != 3)goto _LL3E;
_tmp5B=(_tmp56.LongLong_c).f1;_LL3D: t=_tmp5B == (void*)1?Cyc_Absyn_ulonglong_typ:
Cyc_Absyn_slonglong_typ;goto _LL33;_LL3E: if((_tmp56.Float_c).tag != 4)goto _LL40;
_LL3F: t=Cyc_Absyn_float_typ;goto _LL33;_LL40: if((_tmp56.Int_c).tag != 2)goto _LL42;
_tmp5C=(_tmp56.Int_c).f1;_tmp5D=(_tmp56.Int_c).f2;_LL41: if(topt == 0)t=_tmp5C == (
void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{void*_tmp5F=Cyc_Tcutil_compress(*
topt);void*_tmp60;void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;
void*_tmp66;void*_tmp67;void*_tmp68;_LL47: if(_tmp5F <= (void*)4)goto _LL53;if(*((
int*)_tmp5F)!= 5)goto _LL49;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f1;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)
_tmp61 != 0)goto _LL49;_LL48:{void*_tmp69=_tmp60;_LL56: if((int)_tmp69 != 1)goto
_LL58;_LL57: t=Cyc_Absyn_uchar_typ;goto _LL55;_LL58: if((int)_tmp69 != 0)goto _LL5A;
_LL59: t=Cyc_Absyn_schar_typ;goto _LL55;_LL5A: if((int)_tmp69 != 2)goto _LL55;_LL5B: t=
Cyc_Absyn_char_typ;goto _LL55;_LL55:;}{union Cyc_Absyn_Cnst_union _tmp53E;*c=(union
Cyc_Absyn_Cnst_union)(((_tmp53E.Char_c).tag=0,(((_tmp53E.Char_c).f1=(void*)
_tmp60,(((_tmp53E.Char_c).f2=(char)_tmp5D,_tmp53E))))));}goto _LL46;_LL49: if(*((
int*)_tmp5F)!= 5)goto _LL4B;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f1;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)
_tmp63 != 1)goto _LL4B;_LL4A: t=_tmp62 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;{
union Cyc_Absyn_Cnst_union _tmp53F;*c=(union Cyc_Absyn_Cnst_union)(((_tmp53F.Short_c).tag=
1,(((_tmp53F.Short_c).f1=(void*)_tmp62,(((_tmp53F.Short_c).f2=(short)_tmp5D,
_tmp53F))))));}goto _LL46;_LL4B: if(*((int*)_tmp5F)!= 5)goto _LL4D;_tmp64=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp65 != 2)goto _LL4D;_LL4C: t=_tmp64 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(*((int*)_tmp5F)!= 5)goto _LL4F;_tmp66=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp67 != 3)goto _LL4F;_LL4E: t=_tmp66 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4F: if(*((int*)_tmp5F)!= 4)goto _LL51;if(!(_tmp5D
== 0))goto _LL51;_LL50:{struct Cyc_Absyn_Const_e_struct _tmp545;union Cyc_Absyn_Cnst_union
_tmp544;struct Cyc_Absyn_Const_e_struct*_tmp543;(void*)(e->r=(void*)((void*)((
_tmp543=_cycalloc(sizeof(*_tmp543)),((_tmp543[0]=((_tmp545.tag=0,((_tmp545.f1=(
union Cyc_Absyn_Cnst_union)(((_tmp544.Null_c).tag=6,_tmp544)),_tmp545)))),_tmp543))))));}{
struct Cyc_List_List*_tmp6F=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct
_tmp558;struct Cyc_Core_Opt*_tmp557;struct Cyc_Absyn_PtrAtts _tmp556;struct Cyc_Core_Opt*
_tmp555;struct Cyc_Absyn_PtrInfo _tmp554;struct Cyc_Absyn_PointerType_struct*
_tmp553;t=(void*)((_tmp553=_cycalloc(sizeof(*_tmp553)),((_tmp553[0]=((_tmp558.tag=
4,((_tmp558.f1=((_tmp554.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ak,((_tmp557=_cycalloc(sizeof(*_tmp557)),((_tmp557->v=_tmp6F,_tmp557))))),((
_tmp554.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp554.ptr_atts=((_tmp556.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,((_tmp555=_cycalloc(
sizeof(*_tmp555)),((_tmp555->v=_tmp6F,_tmp555))))),((_tmp556.nullable=Cyc_Absyn_true_conref,((
_tmp556.bounds=Cyc_Absyn_empty_conref(),((_tmp556.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp556.ptrloc=0,_tmp556)))))))))),_tmp554)))))),
_tmp558)))),_tmp553))));}goto _LL46;}_LL51: if(*((int*)_tmp5F)!= 18)goto _LL53;
_tmp68=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp5F)->f1;_LL52: {struct Cyc_Absyn_ValueofType_struct
_tmp55B;struct Cyc_Absyn_ValueofType_struct*_tmp55A;struct Cyc_Absyn_ValueofType_struct*
_tmp76=(_tmp55A=_cycalloc(sizeof(*_tmp55A)),((_tmp55A[0]=((_tmp55B.tag=17,((
_tmp55B.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5D,0),_tmp55B)))),_tmp55A)));if(
!Cyc_Tcutil_unify(_tmp68,(void*)_tmp76)){{const char*_tmp560;void*_tmp55F[2];
struct Cyc_String_pa_struct _tmp55E;struct Cyc_String_pa_struct _tmp55D;(_tmp55D.tag=
0,((_tmp55D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp76)),((_tmp55E.tag=0,((_tmp55E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp68)),((_tmp55F[0]=& _tmp55E,((_tmp55F[1]=&
_tmp55D,Cyc_Tcutil_terr(loc,((_tmp560="expecting %s but found %s",_tag_dyneither(
_tmp560,sizeof(char),26))),_tag_dyneither(_tmp55F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct Cyc_Absyn_TagType_struct _tmp563;struct Cyc_Absyn_TagType_struct*
_tmp562;t=(void*)((_tmp562=_cycalloc(sizeof(*_tmp562)),((_tmp562[0]=((_tmp563.tag=
18,((_tmp563.f1=(void*)((void*)_tmp76),_tmp563)))),_tmp562))));}goto _LL46;}_LL53:;
_LL54: t=_tmp5C == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}
goto _LL33;_LL42: if((_tmp56.String_c).tag != 5)goto _LL44;_tmp5E=(_tmp56.String_c).f1;
_LL43: {int len=(int)_get_dyneither_size(_tmp5E,sizeof(char));union Cyc_Absyn_Cnst_union
_tmp564;struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((
_tmp564.Int_c).tag=2,(((_tmp564.Int_c).f1=(void*)((void*)1),(((_tmp564.Int_c).f2=
len,_tmp564)))))),loc);{struct Cyc_Core_Opt*_tmp565;elen->topt=((_tmp565=
_cycalloc(sizeof(*_tmp565)),((_tmp565->v=(void*)Cyc_Absyn_uint_typ,_tmp565))));}{
struct Cyc_Absyn_Upper_b_struct _tmp568;struct Cyc_Absyn_Upper_b_struct*_tmp567;t=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)((
_tmp567=_cycalloc(sizeof(*_tmp567)),((_tmp567[0]=((_tmp568.tag=0,((_tmp568.f1=
elen,_tmp568)))),_tmp567)))),Cyc_Absyn_true_conref);}if(topt != 0){void*_tmp82=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp83;struct Cyc_Absyn_Tqual
_tmp84;_LL5D: if(_tmp82 <= (void*)4)goto _LL61;if(*((int*)_tmp82)!= 7)goto _LL5F;
_tmp83=((struct Cyc_Absyn_ArrayType_struct*)_tmp82)->f1;_tmp84=_tmp83.tq;_LL5E:
return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp84,(struct Cyc_Absyn_Exp*)elen,((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),0);_LL5F: if(*((int*)
_tmp82)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp569;e->topt=((_tmp569=_cycalloc(sizeof(*
_tmp569)),((_tmp569->v=(void*)t,_tmp569))));}Cyc_Tcutil_unchecked_cast(te,e,*
topt,(void*)3);t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp56C;struct Cyc_Absyn_Upper_b_struct*
_tmp56B;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(void*)((
_tmp56B=_cycalloc(sizeof(*_tmp56B)),((_tmp56B[0]=((_tmp56C.tag=0,((_tmp56C.f1=
elen,_tmp56C)))),_tmp56B)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*topt,
t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp56D;e->topt=((
_tmp56D=_cycalloc(sizeof(*_tmp56D)),((_tmp56D->v=(void*)t,_tmp56D))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}
_LL44: if((_tmp56.Null_c).tag != 6)goto _LL33;_LL45: {struct Cyc_List_List*_tmp8A=
Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp580;struct
Cyc_Core_Opt*_tmp57F;struct Cyc_Absyn_PtrAtts _tmp57E;struct Cyc_Core_Opt*_tmp57D;
struct Cyc_Absyn_PtrInfo _tmp57C;struct Cyc_Absyn_PointerType_struct*_tmp57B;t=(
void*)((_tmp57B=_cycalloc(sizeof(*_tmp57B)),((_tmp57B[0]=((_tmp580.tag=4,((
_tmp580.f1=((_tmp57C.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,((
_tmp57F=_cycalloc(sizeof(*_tmp57F)),((_tmp57F->v=_tmp8A,_tmp57F))))),((_tmp57C.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp57C.ptr_atts=((_tmp57E.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,((_tmp57D=_cycalloc(sizeof(*_tmp57D)),((
_tmp57D->v=_tmp8A,_tmp57D))))),((_tmp57E.nullable=Cyc_Absyn_true_conref,((
_tmp57E.bounds=Cyc_Absyn_empty_conref(),((_tmp57E.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp57E.ptrloc=0,_tmp57E)))))))))),_tmp57C)))))),
_tmp580)))),_tmp57B))));}goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b);
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp91=b;struct Cyc_Absyn_Vardecl*_tmp92;struct
Cyc_Absyn_Fndecl*_tmp93;struct Cyc_Absyn_Vardecl*_tmp94;struct Cyc_Absyn_Vardecl*
_tmp95;struct Cyc_Absyn_Vardecl*_tmp96;_LL64: if((int)_tmp91 != 0)goto _LL66;_LL65: {
const char*_tmp584;void*_tmp583[1];struct Cyc_String_pa_struct _tmp582;return(
_tmp582.tag=0,((_tmp582.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q)),((_tmp583[0]=& _tmp582,Cyc_Tcexp_expr_err(te,loc,0,((_tmp584="undeclared identifier: %s",
_tag_dyneither(_tmp584,sizeof(char),26))),_tag_dyneither(_tmp583,sizeof(void*),1)))))));}
_LL66: if(_tmp91 <= (void*)1)goto _LL68;if(*((int*)_tmp91)!= 0)goto _LL68;_tmp92=((
struct Cyc_Absyn_Global_b_struct*)_tmp91)->f1;_LL67:*q=*_tmp92->name;return(void*)
_tmp92->type;_LL68: if(_tmp91 <= (void*)1)goto _LL6A;if(*((int*)_tmp91)!= 1)goto
_LL6A;_tmp93=((struct Cyc_Absyn_Funname_b_struct*)_tmp91)->f1;_LL69:*q=*_tmp93->name;
return Cyc_Tcutil_fndecl2typ(_tmp93);_LL6A: if(_tmp91 <= (void*)1)goto _LL6C;if(*((
int*)_tmp91)!= 4)goto _LL6C;_tmp94=((struct Cyc_Absyn_Pat_b_struct*)_tmp91)->f1;
_LL6B: _tmp95=_tmp94;goto _LL6D;_LL6C: if(_tmp91 <= (void*)1)goto _LL6E;if(*((int*)
_tmp91)!= 3)goto _LL6E;_tmp95=((struct Cyc_Absyn_Local_b_struct*)_tmp91)->f1;_LL6D:
_tmp96=_tmp95;goto _LL6F;_LL6E: if(_tmp91 <= (void*)1)goto _LL63;if(*((int*)_tmp91)
!= 2)goto _LL63;_tmp96=((struct Cyc_Absyn_Param_b_struct*)_tmp91)->f1;_LL6F:{union
Cyc_Absyn_Nmspace_union _tmp585;(*q).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp585.Loc_n).tag=
0,_tmp585));}return(void*)_tmp96->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,
struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,
struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*));static
void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,
struct Cyc_Core_Opt*opt_args,struct _RegionHandle*temp,struct Cyc_List_List*(*
type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,
struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{void*_tmp9B=(void*)
fmt->r;union Cyc_Absyn_Cnst_union _tmp9C;struct _dyneither_ptr _tmp9D;struct Cyc_Absyn_Exp*
_tmp9E;struct Cyc_Absyn_Exp _tmp9F;void*_tmpA0;union Cyc_Absyn_Cnst_union _tmpA1;
struct _dyneither_ptr _tmpA2;_LL71: if(*((int*)_tmp9B)!= 0)goto _LL73;_tmp9C=((
struct Cyc_Absyn_Const_e_struct*)_tmp9B)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp9B)->f1).String_c).tag != 5)goto _LL73;_tmp9D=(_tmp9C.String_c).f1;_LL72:
_tmpA2=_tmp9D;goto _LL74;_LL73: if(*((int*)_tmp9B)!= 15)goto _LL75;_tmp9E=((struct
Cyc_Absyn_Cast_e_struct*)_tmp9B)->f2;_tmp9F=*_tmp9E;_tmpA0=(void*)_tmp9F.r;if(*((
int*)_tmpA0)!= 0)goto _LL75;_tmpA1=((struct Cyc_Absyn_Const_e_struct*)_tmpA0)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpA0)->f1).String_c).tag != 5)goto _LL75;
_tmpA2=(_tmpA1.String_c).f1;_LL74: desc_types=type_getter(temp,te,(struct
_dyneither_ptr)_tmpA2,fmt->loc);goto _LL70;_LL75:;_LL76: if(opt_args != 0){struct
Cyc_List_List*_tmpA3=(struct Cyc_List_List*)opt_args->v;for(0;_tmpA3 != 0;_tmpA3=
_tmpA3->tl){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpA3->hd);if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)_tmpA3->hd)->topt))->v)
 && !Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmpA3->hd)){const char*
_tmp588;void*_tmp587;(_tmp587=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA3->hd)->loc,((
_tmp588="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp588,sizeof(char),49))),_tag_dyneither(_tmp587,sizeof(void*),0)));}}}return;
_LL70:;}if(opt_args != 0){struct Cyc_List_List*_tmpA6=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpA6 != 0;(desc_types=desc_types->tl,_tmpA6=
_tmpA6->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_tmpA6->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){{
const char*_tmp58D;void*_tmp58C[2];struct Cyc_String_pa_struct _tmp58B;struct Cyc_String_pa_struct
_tmp58A;(_tmp58A.tag=0,((_tmp58A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp58B.tag=0,((_tmp58B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp58C[0]=& _tmp58B,((_tmp58C[1]=& _tmp58A,Cyc_Tcutil_terr(e->loc,((_tmp58D="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp58D,sizeof(char),51))),_tag_dyneither(_tmp58C,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e)){const char*_tmp590;void*_tmp58F;(_tmp58F=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmpA6->hd)->loc,((_tmp590="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp590,sizeof(char),49))),_tag_dyneither(_tmp58F,sizeof(void*),0)));}}
if(desc_types != 0){const char*_tmp593;void*_tmp592;(_tmp592=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp593="too few arguments",_tag_dyneither(_tmp593,sizeof(char),18))),
_tag_dyneither(_tmp592,sizeof(void*),0)));}if(_tmpA6 != 0){const char*_tmp596;void*
_tmp595;(_tmp595=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA6->hd)->loc,((
_tmp596="too many arguments",_tag_dyneither(_tmp596,sizeof(char),19))),
_tag_dyneither(_tmp595,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpB1=p;_LL78: if((
int)_tmpB1 != 0)goto _LL7A;_LL79: goto _LL7B;_LL7A: if((int)_tmpB1 != 2)goto _LL7C;
_LL7B: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp59A;void*_tmp599[1];struct Cyc_String_pa_struct
_tmp598;(_tmp598.tag=0,((_tmp598.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp599[0]=& _tmp598,Cyc_Tcutil_terr(loc,((_tmp59A="expecting arithmetic type but found %s",
_tag_dyneither(_tmp59A,sizeof(char),39))),_tag_dyneither(_tmp599,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL7C: if((int)_tmpB1
!= 11)goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp59E;void*_tmp59D[1];struct Cyc_String_pa_struct _tmp59C;(
_tmp59C.tag=0,((_tmp59C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp59D[0]=& _tmp59C,Cyc_Tcutil_terr(loc,((_tmp59E="expecting integral or * type but found %s",
_tag_dyneither(_tmp59E,sizeof(char),42))),_tag_dyneither(_tmp59D,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;_LL7E: if((int)_tmpB1 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(
e)){const char*_tmp5A2;void*_tmp5A1[1];struct Cyc_String_pa_struct _tmp5A0;(_tmp5A0.tag=
0,((_tmp5A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5A1[0]=& _tmp5A0,Cyc_Tcutil_terr(loc,((_tmp5A2="expecting integral type but found %s",
_tag_dyneither(_tmp5A2,sizeof(char),37))),_tag_dyneither(_tmp5A1,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL80: if((int)_tmpB1
!= 19)goto _LL82;_LL81:{void*_tmpBB=t;struct Cyc_Absyn_PtrInfo _tmpBC;struct Cyc_Absyn_PtrAtts
_tmpBD;struct Cyc_Absyn_Conref*_tmpBE;_LL85: if(_tmpBB <= (void*)4)goto _LL89;if(*((
int*)_tmpBB)!= 7)goto _LL87;_LL86: goto _LL84;_LL87: if(*((int*)_tmpBB)!= 4)goto
_LL89;_tmpBC=((struct Cyc_Absyn_PointerType_struct*)_tmpBB)->f1;_tmpBD=_tmpBC.ptr_atts;
_tmpBE=_tmpBD.bounds;_LL88:{union Cyc_Absyn_Constraint_union _tmpBF=(Cyc_Absyn_compress_conref(
_tmpBE))->v;void*_tmpC0;void*_tmpC1;_LL8C: if((_tmpBF.Eq_constr).tag != 0)goto
_LL8E;_tmpC0=(_tmpBF.Eq_constr).f1;if((int)_tmpC0 != 0)goto _LL8E;_LL8D: goto _LL8F;
_LL8E: if((_tmpBF.Eq_constr).tag != 0)goto _LL90;_tmpC1=(_tmpBF.Eq_constr).f1;if(
_tmpC1 <= (void*)1)goto _LL90;if(*((int*)_tmpC1)!= 0)goto _LL90;_LL8F: goto _LL8B;
_LL90:;_LL91: {const char*_tmp5A5;void*_tmp5A4;(_tmp5A4=0,Cyc_Tcutil_terr(loc,((
_tmp5A5="can't apply numelts to pointer/array of abstract length",_tag_dyneither(
_tmp5A5,sizeof(char),56))),_tag_dyneither(_tmp5A4,sizeof(void*),0)));}_LL8B:;}
goto _LL84;_LL89:;_LL8A: {const char*_tmp5A9;void*_tmp5A8[1];struct Cyc_String_pa_struct
_tmp5A7;(_tmp5A7.tag=0,((_tmp5A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp5A8[0]=& _tmp5A7,Cyc_Tcutil_terr(loc,((_tmp5A9="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp5A9,sizeof(char),47))),_tag_dyneither(_tmp5A8,sizeof(void*),1)))))));}
_LL84:;}return Cyc_Absyn_uint_typ;_LL82:;_LL83: {const char*_tmp5AC;void*_tmp5AB;(
_tmp5AB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp5AC="Non-unary primop",_tag_dyneither(_tmp5AC,sizeof(char),17))),
_tag_dyneither(_tmp5AB,sizeof(void*),0)));}_LL77:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*));static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
if(!checker(e1)){{const char*_tmp5B0;void*_tmp5AF[1];struct Cyc_String_pa_struct
_tmp5AE;(_tmp5AE.tag=0,((_tmp5AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp5AF[0]=& _tmp5AE,Cyc_Tcutil_terr(e1->loc,((_tmp5B0="type %s cannot be used here",
_tag_dyneither(_tmp5B0,sizeof(char),28))),_tag_dyneither(_tmp5AF,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}if(!checker(e2)){{
const char*_tmp5B4;void*_tmp5B3[1];struct Cyc_String_pa_struct _tmp5B2;(_tmp5B2.tag=
0,((_tmp5B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((_tmp5B3[0]=& _tmp5B2,
Cyc_Tcutil_terr(e2->loc,((_tmp5B4="type %s cannot be used here",_tag_dyneither(
_tmp5B4,sizeof(char),28))),_tag_dyneither(_tmp5B3,sizeof(void*),1)))))));}return
Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(
t1,t2);}}static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);void*_tmpCF=t1;struct Cyc_Absyn_PtrInfo
_tmpD0;void*_tmpD1;struct Cyc_Absyn_Tqual _tmpD2;struct Cyc_Absyn_PtrAtts _tmpD3;
void*_tmpD4;struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Conref*_tmpD6;struct
Cyc_Absyn_Conref*_tmpD7;_LL93: if(_tmpCF <= (void*)4)goto _LL95;if(*((int*)_tmpCF)
!= 4)goto _LL95;_tmpD0=((struct Cyc_Absyn_PointerType_struct*)_tmpCF)->f1;_tmpD1=(
void*)_tmpD0.elt_typ;_tmpD2=_tmpD0.elt_tq;_tmpD3=_tmpD0.ptr_atts;_tmpD4=(void*)
_tmpD3.rgn;_tmpD5=_tmpD3.nullable;_tmpD6=_tmpD3.bounds;_tmpD7=_tmpD3.zero_term;
_LL94: if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpD1),(void*)1)){const char*
_tmp5B7;void*_tmp5B6;(_tmp5B6=0,Cyc_Tcutil_terr(e1->loc,((_tmp5B7="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5B7,sizeof(char),50))),_tag_dyneither(_tmp5B6,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp5BA;void*_tmp5B9;(_tmp5B9=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5BA="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5BA,sizeof(char),54))),_tag_dyneither(_tmp5B9,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp5BE;void*_tmp5BD[1];struct
Cyc_String_pa_struct _tmp5BC;(_tmp5BC.tag=0,((_tmp5BC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp5BD[0]=& _tmp5BC,Cyc_Tcutil_terr(
e2->loc,((_tmp5BE="expecting int but found %s",_tag_dyneither(_tmp5BE,sizeof(
char),27))),_tag_dyneither(_tmp5BD,sizeof(void*),1)))))));}_tmpD6=Cyc_Absyn_compress_conref(
_tmpD6);{union Cyc_Absyn_Constraint_union _tmpDF=_tmpD6->v;void*_tmpE0;void*_tmpE1;
struct Cyc_Absyn_Exp*_tmpE2;_LL98: if((_tmpDF.Eq_constr).tag != 0)goto _LL9A;_tmpE0=(
_tmpDF.Eq_constr).f1;if((int)_tmpE0 != 0)goto _LL9A;_LL99: return t1;_LL9A: if((
_tmpDF.Eq_constr).tag != 0)goto _LL9C;_tmpE1=(_tmpDF.Eq_constr).f1;if(_tmpE1 <= (
void*)1)goto _LL9C;if(*((int*)_tmpE1)!= 0)goto _LL9C;_tmpE2=((struct Cyc_Absyn_Upper_b_struct*)
_tmpE1)->f1;_LL9B: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpD7)){const char*_tmp5C1;void*_tmp5C0;(_tmp5C0=0,Cyc_Tcutil_warn(e1->loc,((
_tmp5C1="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp5C1,sizeof(char),70))),_tag_dyneither(_tmp5C0,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp5CB;struct Cyc_Absyn_PtrAtts _tmp5CA;struct
Cyc_Absyn_PtrInfo _tmp5C9;struct Cyc_Absyn_PointerType_struct*_tmp5C8;struct Cyc_Absyn_PointerType_struct*
_tmpE5=(_tmp5C8=_cycalloc(sizeof(*_tmp5C8)),((_tmp5C8[0]=((_tmp5CB.tag=4,((
_tmp5CB.f1=((_tmp5C9.elt_typ=(void*)_tmpD1,((_tmp5C9.elt_tq=_tmpD2,((_tmp5C9.ptr_atts=((
_tmp5CA.rgn=(void*)_tmpD4,((_tmp5CA.nullable=Cyc_Absyn_true_conref,((_tmp5CA.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp5CA.zero_term=_tmpD7,((_tmp5CA.ptrloc=0,
_tmp5CA)))))))))),_tmp5C9)))))),_tmp5CB)))),_tmp5C8)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpE5,(void*)3);return(void*)_tmpE5;}_LL9C:;_LL9D:{union Cyc_Absyn_Constraint_union
_tmp5CC;_tmpD6->v=(union Cyc_Absyn_Constraint_union)(((_tmp5CC.Eq_constr).tag=0,(((
_tmp5CC.Eq_constr).f1=(void*)((void*)0),_tmp5CC))));}return t1;_LL97:;}_LL95:;
_LL96: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL92:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*_tmp5D1;void*_tmp5D0[2];struct Cyc_String_pa_struct
_tmp5CF;struct Cyc_String_pa_struct _tmp5CE;(_tmp5CE.tag=0,((_tmp5CE.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp5CF.tag=0,((_tmp5CF.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp5D0[0]=& _tmp5CF,((_tmp5D0[1]=& _tmp5CE,Cyc_Tcutil_terr(
e1->loc,((_tmp5D1="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp5D1,sizeof(char),59))),_tag_dyneither(_tmp5D0,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),(void*)1)){const char*_tmp5D4;void*_tmp5D3;(_tmp5D3=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5D4="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5D4,sizeof(char),50))),_tag_dyneither(_tmp5D3,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp5D7;void*_tmp5D6;(_tmp5D6=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5D7="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5D7,sizeof(char),54))),_tag_dyneither(_tmp5D6,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp5DC;void*_tmp5DB[2];struct
Cyc_String_pa_struct _tmp5DA;struct Cyc_String_pa_struct _tmp5D9;(_tmp5D9.tag=0,((
_tmp5D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp5DA.tag=0,((_tmp5DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp5DB[0]=& _tmp5DA,((_tmp5DB[1]=& _tmp5D9,Cyc_Tcutil_terr(
e2->loc,((_tmp5DC="expecting either %s or int but found %s",_tag_dyneither(
_tmp5DC,sizeof(char),40))),_tag_dyneither(_tmp5DB,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcAnyBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(
e2);void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num  && e2_is_num)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_typ_kind(
t1)== (void*)2  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_bk,Cyc_Tcenv_lookup_opt_type_vars(te)))){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(
te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(
te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{goto pointer_cmp;}}}}}else{pointer_cmp: {struct
_tuple0 _tmp5DD;struct _tuple0 _tmpF8=(_tmp5DD.f1=Cyc_Tcutil_compress(t1),((_tmp5DD.f2=
Cyc_Tcutil_compress(t2),_tmp5DD)));void*_tmpF9;struct Cyc_Absyn_PtrInfo _tmpFA;
void*_tmpFB;void*_tmpFC;struct Cyc_Absyn_PtrInfo _tmpFD;void*_tmpFE;_LL9F: _tmpF9=
_tmpF8.f1;if(_tmpF9 <= (void*)4)goto _LLA1;if(*((int*)_tmpF9)!= 4)goto _LLA1;_tmpFA=((
struct Cyc_Absyn_PointerType_struct*)_tmpF9)->f1;_tmpFB=(void*)_tmpFA.elt_typ;
_tmpFC=_tmpF8.f2;if(_tmpFC <= (void*)4)goto _LLA1;if(*((int*)_tmpFC)!= 4)goto _LLA1;
_tmpFD=((struct Cyc_Absyn_PointerType_struct*)_tmpFC)->f1;_tmpFE=(void*)_tmpFD.elt_typ;
_LLA0: if(Cyc_Tcutil_unify(_tmpFB,_tmpFE))return Cyc_Absyn_sint_typ;goto _LL9E;
_LLA1:;_LLA2: goto _LL9E;_LL9E:;}{const char*_tmp5E2;void*_tmp5E1[2];struct Cyc_String_pa_struct
_tmp5E0;struct Cyc_String_pa_struct _tmp5DF;(_tmp5DF.tag=0,((_tmp5DF.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp5E0.tag=
0,((_tmp5E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp5E1[0]=& _tmp5E0,((_tmp5E1[1]=& _tmp5DF,Cyc_Tcutil_terr(loc,((_tmp5E2="comparison not allowed between %s and %s",
_tag_dyneither(_tmp5E2,sizeof(char),41))),_tag_dyneither(_tmp5E1,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}}}static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*
Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp103=p;
_LLA4: if((int)_tmp103 != 0)goto _LLA6;_LLA5: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLA6:
if((int)_tmp103 != 2)goto _LLA8;_LLA7: return Cyc_Tcexp_tcMinus(te,e1,e2);_LLA8: if((
int)_tmp103 != 1)goto _LLAA;_LLA9: goto _LLAB;_LLAA: if((int)_tmp103 != 3)goto _LLAC;
_LLAB: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLAC: if((int)
_tmp103 != 4)goto _LLAE;_LLAD: goto _LLAF;_LLAE: if((int)_tmp103 != 13)goto _LLB0;_LLAF:
goto _LLB1;_LLB0: if((int)_tmp103 != 14)goto _LLB2;_LLB1: goto _LLB3;_LLB2: if((int)
_tmp103 != 15)goto _LLB4;_LLB3: goto _LLB5;_LLB4: if((int)_tmp103 != 16)goto _LLB6;
_LLB5: goto _LLB7;_LLB6: if((int)_tmp103 != 17)goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((
int)_tmp103 != 18)goto _LLBA;_LLB9: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);
_LLBA: if((int)_tmp103 != 5)goto _LLBC;_LLBB: goto _LLBD;_LLBC: if((int)_tmp103 != 6)
goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp103 != 7)goto _LLC0;_LLBF: goto _LLC1;
_LLC0: if((int)_tmp103 != 8)goto _LLC2;_LLC1: goto _LLC3;_LLC2: if((int)_tmp103 != 9)
goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)_tmp103 != 10)goto _LLC6;_LLC5: return Cyc_Tcexp_tcAnyBinop(
te,loc,e1,e2);_LLC6:;_LLC7: {const char*_tmp5E5;void*_tmp5E4;(_tmp5E4=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5E5="bad binary primop",
_tag_dyneither(_tmp5E5,sizeof(char),18))),_tag_dyneither(_tmp5E4,sizeof(void*),0)));}
_LLA3:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es);static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLC8: {const char*_tmp5E8;void*
_tmp5E7;return(_tmp5E7=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp5E8="primitive operator has 0 arguments",
_tag_dyneither(_tmp5E8,sizeof(char),35))),_tag_dyneither(_tmp5E7,sizeof(void*),0)));}
case 1: _LLC9: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCA: t=Cyc_Tcexp_tcBinPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(es))->tl))->hd);break;default: _LLCB: {const char*_tmp5EB;void*_tmp5EA;
return(_tmp5EA=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp5EB="primitive operator has > 2 arguments",
_tag_dyneither(_tmp5EB,sizeof(char),37))),_tag_dyneither(_tmp5EA,sizeof(void*),0)));}}
return t;}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_check_writable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){{void*_tmp10A=(void*)e->r;void*
_tmp10B;struct Cyc_Absyn_Vardecl*_tmp10C;void*_tmp10D;struct Cyc_Absyn_Vardecl*
_tmp10E;void*_tmp10F;struct Cyc_Absyn_Vardecl*_tmp110;void*_tmp111;struct Cyc_Absyn_Vardecl*
_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*
_tmp115;struct _dyneither_ptr*_tmp116;struct Cyc_Absyn_Exp*_tmp117;struct
_dyneither_ptr*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;
struct Cyc_Absyn_Exp*_tmp11B;_LLCE: if(*((int*)_tmp10A)!= 1)goto _LLD0;_tmp10B=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10A)->f2;if(_tmp10B <= (void*)1)goto
_LLD0;if(*((int*)_tmp10B)!= 2)goto _LLD0;_tmp10C=((struct Cyc_Absyn_Param_b_struct*)
_tmp10B)->f1;_LLCF: _tmp10E=_tmp10C;goto _LLD1;_LLD0: if(*((int*)_tmp10A)!= 1)goto
_LLD2;_tmp10D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10A)->f2;if(_tmp10D <= (
void*)1)goto _LLD2;if(*((int*)_tmp10D)!= 3)goto _LLD2;_tmp10E=((struct Cyc_Absyn_Local_b_struct*)
_tmp10D)->f1;_LLD1: _tmp110=_tmp10E;goto _LLD3;_LLD2: if(*((int*)_tmp10A)!= 1)goto
_LLD4;_tmp10F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10A)->f2;if(_tmp10F <= (
void*)1)goto _LLD4;if(*((int*)_tmp10F)!= 4)goto _LLD4;_tmp110=((struct Cyc_Absyn_Pat_b_struct*)
_tmp10F)->f1;_LLD3: _tmp112=_tmp110;goto _LLD5;_LLD4: if(*((int*)_tmp10A)!= 1)goto
_LLD6;_tmp111=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp10A)->f2;if(_tmp111 <= (
void*)1)goto _LLD6;if(*((int*)_tmp111)!= 0)goto _LLD6;_tmp112=((struct Cyc_Absyn_Global_b_struct*)
_tmp111)->f1;_LLD5: if(!(_tmp112->tq).real_const)return;goto _LLCD;_LLD6: if(*((int*)
_tmp10A)!= 25)goto _LLD8;_tmp113=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10A)->f1;
_tmp114=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10A)->f2;_LLD7:{void*_tmp11C=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp113->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp11D;struct Cyc_Absyn_Tqual _tmp11E;struct Cyc_Absyn_ArrayInfo
_tmp11F;struct Cyc_Absyn_Tqual _tmp120;struct Cyc_List_List*_tmp121;_LLE5: if(
_tmp11C <= (void*)4)goto _LLEB;if(*((int*)_tmp11C)!= 4)goto _LLE7;_tmp11D=((struct
Cyc_Absyn_PointerType_struct*)_tmp11C)->f1;_tmp11E=_tmp11D.elt_tq;_LLE6: _tmp120=
_tmp11E;goto _LLE8;_LLE7: if(*((int*)_tmp11C)!= 7)goto _LLE9;_tmp11F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp11C)->f1;_tmp120=_tmp11F.tq;_LLE8: if(!_tmp120.real_const)return;goto _LLE4;
_LLE9: if(*((int*)_tmp11C)!= 9)goto _LLEB;_tmp121=((struct Cyc_Absyn_TupleType_struct*)
_tmp11C)->f1;_LLEA: {unsigned int _tmp123;int _tmp124;struct _tuple7 _tmp122=Cyc_Evexp_eval_const_uint_exp(
_tmp114);_tmp123=_tmp122.f1;_tmp124=_tmp122.f2;if(!_tmp124){{const char*_tmp5EE;
void*_tmp5ED;(_tmp5ED=0,Cyc_Tcutil_terr(e->loc,((_tmp5EE="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp5EE,sizeof(char),47))),_tag_dyneither(_tmp5ED,sizeof(void*),0)));}
return;}{struct _handler_cons _tmp127;_push_handler(& _tmp127);{int _tmp129=0;if(
setjmp(_tmp127.handler))_tmp129=1;if(!_tmp129){{struct _tuple9 _tmp12B;struct Cyc_Absyn_Tqual
_tmp12C;struct _tuple9*_tmp12A=((struct _tuple9*(*)(struct Cyc_List_List*x,int n))
Cyc_List_nth)(_tmp121,(int)_tmp123);_tmp12B=*_tmp12A;_tmp12C=_tmp12B.f1;if(!
_tmp12C.real_const){_npop_handler(0);return;}};_pop_handler();}else{void*_tmp128=(
void*)_exn_thrown;void*_tmp12E=_tmp128;_LLEE: if(_tmp12E != Cyc_List_Nth)goto _LLF0;
_LLEF: return;_LLF0:;_LLF1:(void)_throw(_tmp12E);_LLED:;}}}goto _LLE4;}_LLEB:;
_LLEC: goto _LLE4;_LLE4:;}goto _LLCD;_LLD8: if(*((int*)_tmp10A)!= 23)goto _LLDA;
_tmp115=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp10A)->f1;_tmp116=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp10A)->f2;_LLD9:{void*_tmp12F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp115->topt))->v);struct Cyc_Absyn_AggrInfo _tmp130;union Cyc_Absyn_AggrInfoU_union
_tmp131;struct Cyc_Absyn_Aggrdecl**_tmp132;struct Cyc_List_List*_tmp133;_LLF3: if(
_tmp12F <= (void*)4)goto _LLF7;if(*((int*)_tmp12F)!= 10)goto _LLF5;_tmp130=((struct
Cyc_Absyn_AggrType_struct*)_tmp12F)->f1;_tmp131=_tmp130.aggr_info;if((((((struct
Cyc_Absyn_AggrType_struct*)_tmp12F)->f1).aggr_info).KnownAggr).tag != 1)goto _LLF5;
_tmp132=(_tmp131.KnownAggr).f1;_LLF4: {struct Cyc_Absyn_Aggrfield*sf=(struct Cyc_Absyn_Aggrdecl**)
_tmp132 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp132,_tmp116);if(sf == 0  || !(sf->tq).real_const)
return;goto _LLF2;}_LLF5: if(*((int*)_tmp12F)!= 11)goto _LLF7;_tmp133=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp12F)->f2;_LLF6: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp133,
_tmp116);if(sf == 0  || !(sf->tq).real_const)return;goto _LLF2;}_LLF7:;_LLF8: goto
_LLF2;_LLF2:;}goto _LLCD;_LLDA: if(*((int*)_tmp10A)!= 24)goto _LLDC;_tmp117=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp10A)->f1;_tmp118=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp10A)->f2;_LLDB:{void*_tmp134=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp117->topt))->v);struct Cyc_Absyn_PtrInfo _tmp135;void*_tmp136;
_LLFA: if(_tmp134 <= (void*)4)goto _LLFC;if(*((int*)_tmp134)!= 4)goto _LLFC;_tmp135=((
struct Cyc_Absyn_PointerType_struct*)_tmp134)->f1;_tmp136=(void*)_tmp135.elt_typ;
_LLFB:{void*_tmp137=Cyc_Tcutil_compress(_tmp136);struct Cyc_Absyn_AggrInfo _tmp138;
union Cyc_Absyn_AggrInfoU_union _tmp139;struct Cyc_Absyn_Aggrdecl**_tmp13A;struct
Cyc_List_List*_tmp13B;_LLFF: if(_tmp137 <= (void*)4)goto _LL103;if(*((int*)_tmp137)
!= 10)goto _LL101;_tmp138=((struct Cyc_Absyn_AggrType_struct*)_tmp137)->f1;_tmp139=
_tmp138.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp137)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL101;_tmp13A=(_tmp139.KnownAggr).f1;_LL100: {struct Cyc_Absyn_Aggrfield*
sf=(struct Cyc_Absyn_Aggrdecl**)_tmp13A == 0?0: Cyc_Absyn_lookup_decl_field(*
_tmp13A,_tmp118);if(sf == 0  || !(sf->tq).real_const)return;goto _LLFE;}_LL101: if(*((
int*)_tmp137)!= 11)goto _LL103;_tmp13B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp137)->f2;_LL102: {struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(
_tmp13B,_tmp118);if(sf == 0  || !(sf->tq).real_const)return;goto _LLFE;}_LL103:;
_LL104: goto _LLFE;_LLFE:;}goto _LLF9;_LLFC:;_LLFD: goto _LLF9;_LLF9:;}goto _LLCD;
_LLDC: if(*((int*)_tmp10A)!= 22)goto _LLDE;_tmp119=((struct Cyc_Absyn_Deref_e_struct*)
_tmp10A)->f1;_LLDD:{void*_tmp13C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp119->topt))->v);struct Cyc_Absyn_PtrInfo _tmp13D;struct Cyc_Absyn_Tqual
_tmp13E;struct Cyc_Absyn_ArrayInfo _tmp13F;struct Cyc_Absyn_Tqual _tmp140;_LL106: if(
_tmp13C <= (void*)4)goto _LL10A;if(*((int*)_tmp13C)!= 4)goto _LL108;_tmp13D=((
struct Cyc_Absyn_PointerType_struct*)_tmp13C)->f1;_tmp13E=_tmp13D.elt_tq;_LL107:
_tmp140=_tmp13E;goto _LL109;_LL108: if(*((int*)_tmp13C)!= 7)goto _LL10A;_tmp13F=((
struct Cyc_Absyn_ArrayType_struct*)_tmp13C)->f1;_tmp140=_tmp13F.tq;_LL109: if(!
_tmp140.real_const)return;goto _LL105;_LL10A:;_LL10B: goto _LL105;_LL105:;}goto
_LLCD;_LLDE: if(*((int*)_tmp10A)!= 13)goto _LLE0;_tmp11A=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp10A)->f1;_LLDF: _tmp11B=_tmp11A;goto _LLE1;_LLE0: if(*((int*)_tmp10A)!= 14)goto
_LLE2;_tmp11B=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp10A)->f1;_LLE1: Cyc_Tcexp_check_writable(
te,_tmp11B);return;_LLE2:;_LLE3: goto _LLCD;_LLCD:;}{const char*_tmp5F2;void*
_tmp5F1[1];struct Cyc_String_pa_struct _tmp5F0;(_tmp5F0.tag=0,((_tmp5F0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp5F1[0]=&
_tmp5F0,Cyc_Tcutil_terr(e->loc,((_tmp5F2="attempt to write a const location: %s",
_tag_dyneither(_tmp5F2,sizeof(char),38))),_tag_dyneither(_tmp5F1,sizeof(void*),1)))))));}}
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,void*i);static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e)){const char*
_tmp5F5;void*_tmp5F4;return(_tmp5F4=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp5F5="increment/decrement of non-lvalue",
_tag_dyneither(_tmp5F5,sizeof(char),34))),_tag_dyneither(_tmp5F4,sizeof(void*),0)));}
Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (void*)0  || i == (
void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1)){const char*
_tmp5F8;void*_tmp5F7;(_tmp5F7=0,Cyc_Tcutil_terr(e->loc,((_tmp5F8="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5F8,sizeof(char),50))),_tag_dyneither(_tmp5F7,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp5FB;void*_tmp5FA;(_tmp5FA=0,
Cyc_Tcutil_terr(e->loc,((_tmp5FB="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5FB,sizeof(char),54))),_tag_dyneither(_tmp5FA,sizeof(void*),0)));}}
else{const char*_tmp5FF;void*_tmp5FE[1];struct Cyc_String_pa_struct _tmp5FD;(
_tmp5FD.tag=0,((_tmp5FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5FE[0]=& _tmp5FD,Cyc_Tcutil_terr(e->loc,((_tmp5FF="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp5FF,sizeof(char),44))),_tag_dyneither(_tmp5FE,sizeof(void*),1)))))));}}
return t;}}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3);static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){{const char*_tmp600;Cyc_Tcexp_tcTest(te,e1,((_tmp600="conditional expression",
_tag_dyneither(_tmp600,sizeof(char),23))));}Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(
te));struct Cyc_List_List _tmp601;struct Cyc_List_List _tmp14E=(_tmp601.hd=e3,((
_tmp601.tl=0,_tmp601)));struct Cyc_List_List _tmp602;struct Cyc_List_List _tmp14F=(
_tmp602.hd=e2,((_tmp602.tl=(struct Cyc_List_List*)& _tmp14E,_tmp602)));if(!Cyc_Tcutil_coerce_list(
te,t,(struct Cyc_List_List*)& _tmp14F)){{const char*_tmp607;void*_tmp606[2];struct
Cyc_String_pa_struct _tmp605;struct Cyc_String_pa_struct _tmp604;(_tmp604.tag=0,((
_tmp604.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v)),((_tmp605.tag=0,((
_tmp605.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((_tmp606[0]=& _tmp605,((
_tmp606[1]=& _tmp604,Cyc_Tcutil_terr(loc,((_tmp607="conditional clause types do not match: %s != %s",
_tag_dyneither(_tmp607,sizeof(char),48))),_tag_dyneither(_tmp606,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{const char*_tmp608;Cyc_Tcexp_tcTest(
te,e1,((_tmp608="logical-and expression",_tag_dyneither(_tmp608,sizeof(char),23))));}{
const char*_tmp609;Cyc_Tcexp_tcTest(te,e2,((_tmp609="logical-and expression",
_tag_dyneither(_tmp609,sizeof(char),23))));}return Cyc_Absyn_sint_typ;}static void*
Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp60A;Cyc_Tcexp_tcTest(te,e1,((_tmp60A="logical-or expression",
_tag_dyneither(_tmp60A,sizeof(char),22))));}{const char*_tmp60B;Cyc_Tcexp_tcTest(
te,e2,((_tmp60B="logical-or expression",_tag_dyneither(_tmp60B,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*
po,struct Cyc_Absyn_Exp*e2){{struct _RegionHandle _tmp15A=_new_region("r");struct
_RegionHandle*r=& _tmp15A;_push_region(r);Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_notreadctxt(
r,te),0,e1);;_pop_region(r);}Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp15B=Cyc_Tcutil_compress(t1);_LL10D: if(_tmp15B <= (void*)4)goto _LL10F;if(*((
int*)_tmp15B)!= 7)goto _LL10F;_LL10E:{const char*_tmp60E;void*_tmp60D;(_tmp60D=0,
Cyc_Tcutil_terr(loc,((_tmp60E="cannot assign to an array",_tag_dyneither(_tmp60E,
sizeof(char),26))),_tag_dyneither(_tmp60D,sizeof(void*),0)));}goto _LL10C;_LL10F:;
_LL110: goto _LL10C;_LL10C:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),(void*)
1)){const char*_tmp611;void*_tmp610;(_tmp610=0,Cyc_Tcutil_terr(loc,((_tmp611="type is abstract (can't determine size).",
_tag_dyneither(_tmp611,sizeof(char),41))),_tag_dyneither(_tmp610,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp614;void*_tmp613;return(_tmp613=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp614="assignment to non-lvalue",_tag_dyneither(_tmp614,sizeof(
char),25))),_tag_dyneither(_tmp613,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);if(po == 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(
e2)){const char*_tmp617;void*_tmp616;(_tmp616=0,Cyc_Tcutil_terr(e2->loc,((_tmp617="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp617,sizeof(char),49))),_tag_dyneither(_tmp616,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp61C;void*_tmp61B[2];struct
Cyc_String_pa_struct _tmp61A;struct Cyc_String_pa_struct _tmp619;void*_tmp164=(
_tmp619.tag=0,((_tmp619.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp61A.tag=0,((_tmp61A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp61B[0]=& _tmp61A,((_tmp61B[1]=& _tmp619,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp61C="type mismatch: %s != %s",_tag_dyneither(_tmp61C,sizeof(
char),24))),_tag_dyneither(_tmp61B,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp164;}}else{void*_tmp169=(void*)po->v;
void*_tmp16A=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp169,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp16A,t1) || Cyc_Tcutil_coerceable(_tmp16A) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp621;void*_tmp620[2];struct Cyc_String_pa_struct _tmp61F;struct Cyc_String_pa_struct
_tmp61E;void*_tmp16B=(_tmp61E.tag=0,((_tmp61E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp61F.tag=0,((_tmp61F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp620[0]=&
_tmp61F,((_tmp620[1]=& _tmp61E,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp621="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp621,sizeof(char),82))),_tag_dyneither(_tmp620,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(_tmp16A,t1);Cyc_Tcutil_explain_failure();return _tmp16B;}return
_tmp16A;}return t1;}}static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,
struct Cyc_List_List*fs);static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){static struct Cyc_Absyn_DoubleType_struct
dbl={6,0};static void*dbl_typ=(void*)& dbl;struct Cyc_List_List*fields;void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp170=Cyc_Tcutil_compress(
t1);void*_tmp171;void*_tmp172;_LL112: if((int)_tmp170 != 1)goto _LL114;_LL113: Cyc_Tcutil_unchecked_cast(
te,e,dbl_typ,(void*)1);t1=dbl_typ;goto _LL111;_LL114: if(_tmp170 <= (void*)4)goto
_LL118;if(*((int*)_tmp170)!= 5)goto _LL116;_tmp171=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp170)->f2;if((int)_tmp171 != 0)goto _LL116;_LL115: goto _LL117;_LL116: if(*((int*)
_tmp170)!= 5)goto _LL118;_tmp172=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp170)->f2;
if((int)_tmp172 != 1)goto _LL118;_LL117: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);t1=Cyc_Absyn_sint_typ;goto _LL111;_LL118:;_LL119: goto _LL111;_LL111:;}
for(fields=fs;fields != 0;fields=fields->tl){struct _tuple1*_tmp174;struct Cyc_List_List*
_tmp175;struct Cyc_Position_Segment*_tmp176;void*_tmp177;struct Cyc_Absyn_Tunionfield
_tmp173=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_tmp174=_tmp173.name;_tmp175=
_tmp173.typs;_tmp176=_tmp173.loc;_tmp177=(void*)_tmp173.sc;if(_tmp175 == 0  || 
_tmp175->tl != 0)continue;{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct
_tuple9*)_tmp175->hd)).f2);if(Cyc_Tcutil_unify(t1,t2))return(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*)fields->hd);}}for(fields=fs;fields != 0;fields=
fields->tl){struct _tuple1*_tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_Position_Segment*
_tmp17B;void*_tmp17C;struct Cyc_Absyn_Tunionfield _tmp178=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_tmp179=_tmp178.name;_tmp17A=_tmp178.typs;_tmp17B=_tmp178.loc;
_tmp17C=(void*)_tmp178.sc;if(_tmp17A == 0  || _tmp17A->tl != 0)continue;{void*t2=
Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple9*)_tmp17A->hd)).f2);if(Cyc_Tcutil_coerce_arg(
te,e,t2))return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}{const char*_tmp626;void*_tmp625[2];struct Cyc_String_pa_struct
_tmp624;struct Cyc_String_pa_struct _tmp623;(_tmp623.tag=0,((_tmp623.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp624.tag=
0,((_tmp624.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
tu)),((_tmp625[0]=& _tmp624,((_tmp625[1]=& _tmp623,Cyc_Tcutil_terr(e->loc,((
_tmp626="can't find a field in %s to inject a value of type %s",_tag_dyneither(
_tmp626,sizeof(char),54))),_tag_dyneither(_tmp625,sizeof(void*),2)))))))))))));}
return 0;}static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info);static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info){struct Cyc_List_List*
_tmp182=args;struct _RegionHandle _tmp183=_new_region("ter");struct _RegionHandle*
ter=& _tmp183;_push_region(ter);{struct Cyc_Tcenv_Tenv*_tmp184=Cyc_Tcenv_new_block(
ter,loc,te_orig);Cyc_Tcexp_tcExp(_tmp184,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp185=t;struct Cyc_Absyn_PtrInfo
_tmp186;void*_tmp187;struct Cyc_Absyn_Tqual _tmp188;struct Cyc_Absyn_PtrAtts _tmp189;
void*_tmp18A;struct Cyc_Absyn_Conref*_tmp18B;struct Cyc_Absyn_Conref*_tmp18C;
struct Cyc_Absyn_Conref*_tmp18D;_LL11B: if(_tmp185 <= (void*)4)goto _LL11D;if(*((int*)
_tmp185)!= 4)goto _LL11D;_tmp186=((struct Cyc_Absyn_PointerType_struct*)_tmp185)->f1;
_tmp187=(void*)_tmp186.elt_typ;_tmp188=_tmp186.elt_tq;_tmp189=_tmp186.ptr_atts;
_tmp18A=(void*)_tmp189.rgn;_tmp18B=_tmp189.nullable;_tmp18C=_tmp189.bounds;
_tmp18D=_tmp189.zero_term;_LL11C: Cyc_Tcenv_check_rgn_accessible(_tmp184,loc,
_tmp18A);Cyc_Tcutil_check_nonzero_bound(loc,_tmp18C);{void*_tmp18E=Cyc_Tcutil_compress(
_tmp187);struct Cyc_Absyn_FnInfo _tmp18F;struct Cyc_List_List*_tmp190;struct Cyc_Core_Opt*
_tmp191;void*_tmp192;struct Cyc_List_List*_tmp193;int _tmp194;struct Cyc_Absyn_VarargInfo*
_tmp195;struct Cyc_List_List*_tmp196;struct Cyc_List_List*_tmp197;_LL120: if(
_tmp18E <= (void*)4)goto _LL122;if(*((int*)_tmp18E)!= 8)goto _LL122;_tmp18F=((
struct Cyc_Absyn_FnType_struct*)_tmp18E)->f1;_tmp190=_tmp18F.tvars;_tmp191=
_tmp18F.effect;_tmp192=(void*)_tmp18F.ret_typ;_tmp193=_tmp18F.args;_tmp194=
_tmp18F.c_varargs;_tmp195=_tmp18F.cyc_varargs;_tmp196=_tmp18F.rgn_po;_tmp197=
_tmp18F.attributes;_LL121: if(topt != 0)Cyc_Tcutil_unify(_tmp192,*topt);while(
_tmp182 != 0  && _tmp193 != 0){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp182->hd;
void*t2=(*((struct _tuple2*)_tmp193->hd)).f3;Cyc_Tcexp_tcExp(_tmp184,(void**)& t2,
e1);if(!Cyc_Tcutil_coerce_arg(_tmp184,e1,t2)){{const char*_tmp62B;void*_tmp62A[2];
struct Cyc_String_pa_struct _tmp629;struct Cyc_String_pa_struct _tmp628;(_tmp628.tag=
0,((_tmp628.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp629.tag=0,((_tmp629.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp62A[0]=& _tmp629,((_tmp62A[1]=& _tmp628,Cyc_Tcutil_terr(e1->loc,((_tmp62B="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp62B,sizeof(char),57))),_tag_dyneither(_tmp62A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1)){
const char*_tmp62E;void*_tmp62D;(_tmp62D=0,Cyc_Tcutil_terr(e1->loc,((_tmp62E="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp62E,sizeof(char),49))),_tag_dyneither(_tmp62D,sizeof(void*),0)));}
_tmp182=_tmp182->tl;_tmp193=_tmp193->tl;}{int args_already_checked=0;{struct Cyc_List_List*
a=_tmp197;for(0;a != 0;a=a->tl){void*_tmp19E=(void*)a->hd;void*_tmp19F;int _tmp1A0;
int _tmp1A1;_LL125: if(_tmp19E <= (void*)17)goto _LL127;if(*((int*)_tmp19E)!= 3)goto
_LL127;_tmp19F=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp19E)->f1;_tmp1A0=((
struct Cyc_Absyn_Format_att_struct*)_tmp19E)->f2;_tmp1A1=((struct Cyc_Absyn_Format_att_struct*)
_tmp19E)->f3;_LL126:{struct _handler_cons _tmp1A2;_push_handler(& _tmp1A2);{int
_tmp1A4=0;if(setjmp(_tmp1A2.handler))_tmp1A4=1;if(!_tmp1A4){{struct Cyc_Absyn_Exp*
_tmp1A5=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp1A0 - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1A1 == 0)fmt_args=0;else{struct Cyc_Core_Opt*
_tmp62F;fmt_args=((_tmp62F=_cycalloc(sizeof(*_tmp62F)),((_tmp62F->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1A1 - 1),_tmp62F))));}
args_already_checked=1;{struct _RegionHandle _tmp1A7=_new_region("temp");struct
_RegionHandle*temp=& _tmp1A7;_push_region(temp);{void*_tmp1A8=_tmp19F;_LL12A: if((
int)_tmp1A8 != 0)goto _LL12C;_LL12B: Cyc_Tcexp_check_format_args(_tmp184,_tmp1A5,
fmt_args,temp,Cyc_Formatstr_get_format_typs);goto _LL129;_LL12C: if((int)_tmp1A8 != 
1)goto _LL129;_LL12D: Cyc_Tcexp_check_format_args(_tmp184,_tmp1A5,fmt_args,temp,
Cyc_Formatstr_get_scanf_typs);goto _LL129;_LL129:;};_pop_region(temp);}};
_pop_handler();}else{void*_tmp1A3=(void*)_exn_thrown;void*_tmp1AA=_tmp1A3;_LL12F:
if(_tmp1AA != Cyc_List_Nth)goto _LL131;_LL130:{const char*_tmp632;void*_tmp631;(
_tmp631=0,Cyc_Tcutil_terr(loc,((_tmp632="bad format arguments",_tag_dyneither(
_tmp632,sizeof(char),21))),_tag_dyneither(_tmp631,sizeof(void*),0)));}goto _LL12E;
_LL131:;_LL132:(void)_throw(_tmp1AA);_LL12E:;}}}goto _LL124;_LL127:;_LL128: goto
_LL124;_LL124:;}}if(_tmp193 != 0){const char*_tmp635;void*_tmp634;(_tmp634=0,Cyc_Tcutil_terr(
loc,((_tmp635="too few arguments for function",_tag_dyneither(_tmp635,sizeof(
char),31))),_tag_dyneither(_tmp634,sizeof(void*),0)));}else{if((_tmp182 != 0  || 
_tmp194) || _tmp195 != 0){if(_tmp194)for(0;_tmp182 != 0;_tmp182=_tmp182->tl){Cyc_Tcexp_tcExp(
_tmp184,0,(struct Cyc_Absyn_Exp*)_tmp182->hd);}else{if(_tmp195 == 0){const char*
_tmp638;void*_tmp637;(_tmp637=0,Cyc_Tcutil_terr(loc,((_tmp638="too many arguments for function",
_tag_dyneither(_tmp638,sizeof(char),32))),_tag_dyneither(_tmp637,sizeof(void*),0)));}
else{void*_tmp1B2;int _tmp1B3;struct Cyc_Absyn_VarargInfo _tmp1B1=*_tmp195;_tmp1B2=(
void*)_tmp1B1.type;_tmp1B3=_tmp1B1.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp639;struct Cyc_Absyn_VarargCallInfo*_tmp1B4=(_tmp639=_cycalloc(sizeof(*
_tmp639)),((_tmp639->num_varargs=0,((_tmp639->injectors=0,((_tmp639->vai=(struct
Cyc_Absyn_VarargInfo*)_tmp195,_tmp639)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp1B4;if(!_tmp1B3)for(0;_tmp182 != 0;_tmp182=_tmp182->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp182->hd;++ _tmp1B4->num_varargs;Cyc_Tcexp_tcExp(
_tmp184,(void**)& _tmp1B2,e1);if(!Cyc_Tcutil_coerce_arg(_tmp184,e1,_tmp1B2)){{
const char*_tmp63E;void*_tmp63D[2];struct Cyc_String_pa_struct _tmp63C;struct Cyc_String_pa_struct
_tmp63B;(_tmp63B.tag=0,((_tmp63B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp63C.tag=0,((_tmp63C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp1B2)),((_tmp63D[0]=& _tmp63C,((_tmp63D[1]=& _tmp63B,Cyc_Tcutil_terr(loc,((
_tmp63E="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp63E,sizeof(char),49))),_tag_dyneither(_tmp63D,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1B2)
 && !Cyc_Tcutil_is_noalias_path(e1)){const char*_tmp641;void*_tmp640;(_tmp640=0,
Cyc_Tcutil_terr(e1->loc,((_tmp641="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp641,sizeof(char),49))),_tag_dyneither(_tmp640,sizeof(void*),0)));}}
else{void*_tmp1BB=Cyc_Tcutil_compress(_tmp1B2);struct Cyc_Absyn_TunionInfo _tmp1BC;
union Cyc_Absyn_TunionInfoU_union _tmp1BD;struct Cyc_Absyn_Tuniondecl**_tmp1BE;
struct Cyc_Absyn_Tuniondecl*_tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_Core_Opt*
_tmp1C1;_LL134: if(_tmp1BB <= (void*)4)goto _LL136;if(*((int*)_tmp1BB)!= 2)goto
_LL136;_tmp1BC=((struct Cyc_Absyn_TunionType_struct*)_tmp1BB)->f1;_tmp1BD=_tmp1BC.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp1BB)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL136;_tmp1BE=(_tmp1BD.KnownTunion).f1;_tmp1BF=*_tmp1BE;_tmp1C0=
_tmp1BC.targs;_tmp1C1=_tmp1BC.rgn;_LL135: {struct Cyc_Absyn_Tuniondecl*_tmp1C2=*
Cyc_Tcenv_lookup_tuniondecl(_tmp184,loc,_tmp1BF->name);struct Cyc_List_List*
fields=0;if(_tmp1C2->fields == 0){const char*_tmp645;void*_tmp644[1];struct Cyc_String_pa_struct
_tmp643;(_tmp643.tag=0,((_tmp643.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1B2)),((_tmp644[0]=& _tmp643,Cyc_Tcutil_terr(loc,((
_tmp645="can't inject into %s",_tag_dyneither(_tmp645,sizeof(char),21))),
_tag_dyneither(_tmp644,sizeof(void*),1)))))));}else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C2->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C1))->v,Cyc_Tcenv_curr_rgn(_tmp184))){const
char*_tmp648;void*_tmp647;(_tmp647=0,Cyc_Tcutil_terr(loc,((_tmp648="bad region for injected varargs",
_tag_dyneither(_tmp648,sizeof(char),32))),_tag_dyneither(_tmp647,sizeof(void*),0)));}{
struct _RegionHandle _tmp1C8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1C8;
_push_region(rgn);{struct Cyc_List_List*_tmp1C9=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1C2->tvs,_tmp1C0);for(0;_tmp182 != 0;_tmp182=
_tmp182->tl){++ _tmp1B4->num_varargs;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)
_tmp182->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(_tmp184,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
e1)){const char*_tmp64B;void*_tmp64A;(_tmp64A=0,Cyc_Tcutil_terr(e1->loc,((_tmp64B="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp64B,sizeof(char),49))),_tag_dyneither(_tmp64A,sizeof(void*),0)));}}{
struct Cyc_Absyn_Tunionfield*_tmp1CC=Cyc_Tcexp_tcInjection(_tmp184,e1,_tmp1B2,rgn,
_tmp1C9,fields);if(_tmp1CC != 0){struct Cyc_List_List*_tmp64C;_tmp1B4->injectors=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1B4->injectors,((_tmp64C=_cycalloc(sizeof(*_tmp64C)),((_tmp64C->hd=(struct
Cyc_Absyn_Tunionfield*)_tmp1CC,((_tmp64C->tl=0,_tmp64C)))))));}}}}};_pop_region(
rgn);}goto _LL133;}_LL136:;_LL137:{const char*_tmp64F;void*_tmp64E;(_tmp64E=0,Cyc_Tcutil_terr(
loc,((_tmp64F="bad inject vararg type",_tag_dyneither(_tmp64F,sizeof(char),23))),
_tag_dyneither(_tmp64E,sizeof(void*),0)));}goto _LL133;_LL133:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp184,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp191))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp184,loc,_tmp196);{void*_tmp1D1=_tmp192;_npop_handler(0);return _tmp1D1;}}
_LL122:;_LL123: {const char*_tmp652;void*_tmp651;void*_tmp1D4=(_tmp651=0,Cyc_Tcexp_expr_err(
_tmp184,loc,topt,((_tmp652="expected pointer to function",_tag_dyneither(_tmp652,
sizeof(char),29))),_tag_dyneither(_tmp651,sizeof(void*),0)));_npop_handler(0);
return _tmp1D4;}_LL11F:;}_LL11D:;_LL11E: {const char*_tmp655;void*_tmp654;void*
_tmp1D7=(_tmp654=0,Cyc_Tcexp_expr_err(_tmp184,loc,topt,((_tmp655="expected pointer to function",
_tag_dyneither(_tmp655,sizeof(char),29))),_tag_dyneither(_tmp654,sizeof(void*),0)));
_npop_handler(0);return _tmp1D7;}_LL11A:;}};_pop_region(ter);}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,
e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ)){const char*_tmp659;void*
_tmp658[1];struct Cyc_String_pa_struct _tmp657;(_tmp657.tag=0,((_tmp657.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp658[0]=& _tmp657,Cyc_Tcutil_terr(loc,((_tmp659="expected xtunion exn but found %s",
_tag_dyneither(_tmp659,sizeof(char),34))),_tag_dyneither(_tmp658,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}static void*Cyc_Tcexp_tcInstantiate(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*ts);static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts){Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
0));{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp1DB=t1;struct Cyc_Absyn_PtrInfo _tmp1DC;void*_tmp1DD;struct Cyc_Absyn_Tqual
_tmp1DE;struct Cyc_Absyn_PtrAtts _tmp1DF;void*_tmp1E0;struct Cyc_Absyn_Conref*
_tmp1E1;struct Cyc_Absyn_Conref*_tmp1E2;struct Cyc_Absyn_Conref*_tmp1E3;_LL139: if(
_tmp1DB <= (void*)4)goto _LL13B;if(*((int*)_tmp1DB)!= 4)goto _LL13B;_tmp1DC=((
struct Cyc_Absyn_PointerType_struct*)_tmp1DB)->f1;_tmp1DD=(void*)_tmp1DC.elt_typ;
_tmp1DE=_tmp1DC.elt_tq;_tmp1DF=_tmp1DC.ptr_atts;_tmp1E0=(void*)_tmp1DF.rgn;
_tmp1E1=_tmp1DF.nullable;_tmp1E2=_tmp1DF.bounds;_tmp1E3=_tmp1DF.zero_term;_LL13A:{
void*_tmp1E4=Cyc_Tcutil_compress(_tmp1DD);struct Cyc_Absyn_FnInfo _tmp1E5;struct
Cyc_List_List*_tmp1E6;struct Cyc_Core_Opt*_tmp1E7;void*_tmp1E8;struct Cyc_List_List*
_tmp1E9;int _tmp1EA;struct Cyc_Absyn_VarargInfo*_tmp1EB;struct Cyc_List_List*
_tmp1EC;struct Cyc_List_List*_tmp1ED;_LL13E: if(_tmp1E4 <= (void*)4)goto _LL140;if(*((
int*)_tmp1E4)!= 8)goto _LL140;_tmp1E5=((struct Cyc_Absyn_FnType_struct*)_tmp1E4)->f1;
_tmp1E6=_tmp1E5.tvars;_tmp1E7=_tmp1E5.effect;_tmp1E8=(void*)_tmp1E5.ret_typ;
_tmp1E9=_tmp1E5.args;_tmp1EA=_tmp1E5.c_varargs;_tmp1EB=_tmp1E5.cyc_varargs;
_tmp1EC=_tmp1E5.rgn_po;_tmp1ED=_tmp1E5.attributes;_LL13F: {struct _RegionHandle
_tmp1EE=_new_region("temp");struct _RegionHandle*temp=& _tmp1EE;_push_region(temp);{
struct Cyc_List_List*instantiation=0;for(0;ts != 0  && _tmp1E6 != 0;(ts=ts->tl,
_tmp1E6=_tmp1E6->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1E6->hd);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);{
struct _tuple5*_tmp65C;struct Cyc_List_List*_tmp65B;instantiation=((_tmp65B=
_region_malloc(temp,sizeof(*_tmp65B)),((_tmp65B->hd=((_tmp65C=_region_malloc(
temp,sizeof(*_tmp65C)),((_tmp65C->f1=(struct Cyc_Absyn_Tvar*)_tmp1E6->hd,((
_tmp65C->f2=(void*)ts->hd,_tmp65C)))))),((_tmp65B->tl=instantiation,_tmp65B))))));}}
if(ts != 0){const char*_tmp65F;void*_tmp65E;void*_tmp1F3=(_tmp65E=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp65F="too many type variables in instantiation",_tag_dyneither(
_tmp65F,sizeof(char),41))),_tag_dyneither(_tmp65E,sizeof(void*),0)));
_npop_handler(0);return _tmp1F3;}{struct Cyc_Absyn_FnType_struct _tmp665;struct Cyc_Absyn_FnInfo
_tmp664;struct Cyc_Absyn_FnType_struct*_tmp663;void*new_fn_typ=Cyc_Tcutil_rsubstitute(
temp,instantiation,(void*)((_tmp663=_cycalloc(sizeof(*_tmp663)),((_tmp663[0]=((
_tmp665.tag=8,((_tmp665.f1=((_tmp664.tvars=_tmp1E6,((_tmp664.effect=_tmp1E7,((
_tmp664.ret_typ=(void*)_tmp1E8,((_tmp664.args=_tmp1E9,((_tmp664.c_varargs=
_tmp1EA,((_tmp664.cyc_varargs=_tmp1EB,((_tmp664.rgn_po=_tmp1EC,((_tmp664.attributes=
_tmp1ED,_tmp664)))))))))))))))),_tmp665)))),_tmp663)))));struct Cyc_Absyn_PointerType_struct
_tmp66F;struct Cyc_Absyn_PtrAtts _tmp66E;struct Cyc_Absyn_PtrInfo _tmp66D;struct Cyc_Absyn_PointerType_struct*
_tmp66C;void*_tmp1F8=(void*)((_tmp66C=_cycalloc(sizeof(*_tmp66C)),((_tmp66C[0]=((
_tmp66F.tag=4,((_tmp66F.f1=((_tmp66D.elt_typ=(void*)new_fn_typ,((_tmp66D.elt_tq=
_tmp1DE,((_tmp66D.ptr_atts=((_tmp66E.rgn=(void*)_tmp1E0,((_tmp66E.nullable=
_tmp1E1,((_tmp66E.bounds=_tmp1E2,((_tmp66E.zero_term=_tmp1E3,((_tmp66E.ptrloc=0,
_tmp66E)))))))))),_tmp66D)))))),_tmp66F)))),_tmp66C))));_npop_handler(0);return
_tmp1F8;}};_pop_region(temp);}_LL140:;_LL141: goto _LL13D;_LL13D:;}goto _LL138;
_LL13B:;_LL13C: goto _LL138;_LL138:;}{const char*_tmp673;void*_tmp672[1];struct Cyc_String_pa_struct
_tmp671;return(_tmp671.tag=0,((_tmp671.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp672[0]=& _tmp671,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp673="expecting polymorphic type but found %s",_tag_dyneither(
_tmp673,sizeof(char),40))),_tag_dyneither(_tmp672,sizeof(void*),1)))))));}}}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,void**c);static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{const char*_tmp678;
void*_tmp677[2];struct Cyc_String_pa_struct _tmp676;struct Cyc_String_pa_struct
_tmp675;void*_tmp1FF=(_tmp675.tag=0,((_tmp675.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp676.tag=0,((_tmp676.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp677[0]=&
_tmp676,((_tmp677[1]=& _tmp675,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp678="cannot cast %s to %s",
_tag_dyneither(_tmp678,sizeof(char),21))),_tag_dyneither(_tmp677,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp1FF;}}}}{struct _tuple0 _tmp679;struct
_tuple0 _tmp205=(_tmp679.f1=(void*)e->r,((_tmp679.f2=Cyc_Tcutil_compress(t),
_tmp679)));void*_tmp206;struct Cyc_Absyn_MallocInfo _tmp207;int _tmp208;void*
_tmp209;struct Cyc_Absyn_PtrInfo _tmp20A;struct Cyc_Absyn_PtrAtts _tmp20B;struct Cyc_Absyn_Conref*
_tmp20C;struct Cyc_Absyn_Conref*_tmp20D;struct Cyc_Absyn_Conref*_tmp20E;_LL143:
_tmp206=_tmp205.f1;if(*((int*)_tmp206)!= 35)goto _LL145;_tmp207=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp206)->f1;_tmp208=_tmp207.fat_result;_tmp209=_tmp205.f2;if(_tmp209 <= (void*)4)
goto _LL145;if(*((int*)_tmp209)!= 4)goto _LL145;_tmp20A=((struct Cyc_Absyn_PointerType_struct*)
_tmp209)->f1;_tmp20B=_tmp20A.ptr_atts;_tmp20C=_tmp20B.nullable;_tmp20D=_tmp20B.bounds;
_tmp20E=_tmp20B.zero_term;_LL144: if((_tmp208  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp20E)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp20C)){void*_tmp20F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp20D);struct Cyc_Absyn_Exp*_tmp210;_LL148: if(_tmp20F <= (void*)1)goto _LL14A;if(*((
int*)_tmp20F)!= 0)goto _LL14A;_tmp210=((struct Cyc_Absyn_Upper_b_struct*)_tmp20F)->f1;
_LL149: if((Cyc_Evexp_eval_const_uint_exp(_tmp210)).f1 == 1){const char*_tmp67C;
void*_tmp67B;(_tmp67B=0,Cyc_Tcutil_warn(loc,((_tmp67C="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp67C,sizeof(char),60))),_tag_dyneither(_tmp67B,sizeof(void*),0)));}
goto _LL147;_LL14A:;_LL14B: goto _LL147;_LL147:;}goto _LL142;_LL145:;_LL146: goto
_LL142;_LL142:;}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){void**_tmp213=0;
struct Cyc_Absyn_Tqual _tmp214=Cyc_Absyn_empty_tqual(0);if(topt != 0){void*_tmp215=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp216;void*_tmp217;struct Cyc_Absyn_Tqual
_tmp218;struct Cyc_Absyn_PtrAtts _tmp219;struct Cyc_Absyn_Conref*_tmp21A;_LL14D: if(
_tmp215 <= (void*)4)goto _LL14F;if(*((int*)_tmp215)!= 4)goto _LL14F;_tmp216=((
struct Cyc_Absyn_PointerType_struct*)_tmp215)->f1;_tmp217=(void*)_tmp216.elt_typ;
_tmp218=_tmp216.elt_tq;_tmp219=_tmp216.ptr_atts;_tmp21A=_tmp219.zero_term;_LL14E:{
void**_tmp67D;_tmp213=((_tmp67D=_cycalloc(sizeof(*_tmp67D)),((_tmp67D[0]=_tmp217,
_tmp67D))));}_tmp214=_tmp218;goto _LL14C;_LL14F:;_LL150: goto _LL14C;_LL14C:;}{
struct _RegionHandle _tmp21C=_new_region("r");struct _RegionHandle*r=& _tmp21C;
_push_region(r);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(r,te),_tmp213,
e);;_pop_region(r);}if(Cyc_Tcutil_is_noalias_path(e)){const char*_tmp680;void*
_tmp67F;(_tmp67F=0,Cyc_Tcutil_terr(e->loc,((_tmp680="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp680,sizeof(char),46))),_tag_dyneither(_tmp67F,sizeof(void*),0)));}{
void*_tmp21F=(void*)e->r;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Exp*_tmp221;
_LL152: if(*((int*)_tmp21F)!= 25)goto _LL154;_tmp220=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp21F)->f1;_tmp221=((struct Cyc_Absyn_Subscript_e_struct*)_tmp21F)->f2;_LL153:{
void*_tmp222=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp220->topt))->v);_LL157: if(_tmp222 <= (void*)4)goto _LL159;if(*((int*)_tmp222)
!= 9)goto _LL159;_LL158: goto _LL156;_LL159:;_LL15A:(void*)(e0->r=(void*)((void*)(
Cyc_Absyn_add_exp(_tmp220,_tmp221,0))->r));return Cyc_Tcexp_tcPlus(te,_tmp220,
_tmp221);_LL156:;}goto _LL151;_LL154:;_LL155: goto _LL151;_LL151:;}{int _tmp224;void*
_tmp225;struct _tuple6 _tmp223=Cyc_Tcutil_addressof_props(te,e);_tmp224=_tmp223.f1;
_tmp225=_tmp223.f2;{struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);if(_tmp224){
tq.print_const=1;tq.real_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,_tmp225,tq,Cyc_Absyn_false_conref);return t;}}}}static
void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,void*t);static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*t){if(te->allow_valueof)return Cyc_Absyn_uint_typ;
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);if(!
Cyc_Evexp_okay_szofarg(t)){const char*_tmp684;void*_tmp683[1];struct Cyc_String_pa_struct
_tmp682;(_tmp682.tag=0,((_tmp682.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp683[0]=& _tmp682,Cyc_Tcutil_terr(loc,((_tmp684="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp684,sizeof(char),55))),_tag_dyneither(_tmp683,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp229=Cyc_Tcutil_compress(*topt);void*_tmp22A;_LL15C: if(
_tmp229 <= (void*)4)goto _LL15E;if(*((int*)_tmp229)!= 18)goto _LL15E;_tmp22A=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp229)->f1;_LL15D: {struct Cyc_Absyn_Exp*
_tmp22B=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct _tmp687;
struct Cyc_Absyn_ValueofType_struct*_tmp686;struct Cyc_Absyn_ValueofType_struct*
_tmp22C=(_tmp686=_cycalloc(sizeof(*_tmp686)),((_tmp686[0]=((_tmp687.tag=17,((
_tmp687.f1=_tmp22B,_tmp687)))),_tmp686)));if(Cyc_Tcutil_unify(_tmp22A,(void*)
_tmp22C))return _tmp229;goto _LL15B;}_LL15E:;_LL15F: goto _LL15B;_LL15B:;}return Cyc_Absyn_uint_typ;}
int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf);int Cyc_Tcexp_structfield_has_name(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*
sf){return Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 
0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n);static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(
loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)1,1,t);{void*_tmp22F=n;struct
_dyneither_ptr*_tmp230;unsigned int _tmp231;_LL161: if(*((int*)_tmp22F)!= 0)goto
_LL163;_tmp230=((struct Cyc_Absyn_StructField_struct*)_tmp22F)->f1;_LL162: {int
bad_type=1;{void*_tmp232=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp233;
union Cyc_Absyn_AggrInfoU_union _tmp234;struct Cyc_Absyn_Aggrdecl**_tmp235;struct
Cyc_List_List*_tmp236;_LL166: if(_tmp232 <= (void*)4)goto _LL16A;if(*((int*)_tmp232)
!= 10)goto _LL168;_tmp233=((struct Cyc_Absyn_AggrType_struct*)_tmp232)->f1;_tmp234=
_tmp233.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp232)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL168;_tmp235=(_tmp234.KnownAggr).f1;_LL167: if((*_tmp235)->impl == 0)
goto _LL165;if(!((int(*)(int(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),
struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp230,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp235)->impl))->fields)){
const char*_tmp68B;void*_tmp68A[1];struct Cyc_String_pa_struct _tmp689;(_tmp689.tag=
0,((_tmp689.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp230),((_tmp68A[
0]=& _tmp689,Cyc_Tcutil_terr(loc,((_tmp68B="no field of struct/union has name %s",
_tag_dyneither(_tmp68B,sizeof(char),37))),_tag_dyneither(_tmp68A,sizeof(void*),1)))))));}
bad_type=0;goto _LL165;_LL168: if(*((int*)_tmp232)!= 11)goto _LL16A;_tmp236=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp232)->f2;_LL169: if(!((int(*)(int(*pred)(
struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp230,
_tmp236)){const char*_tmp68F;void*_tmp68E[1];struct Cyc_String_pa_struct _tmp68D;(
_tmp68D.tag=0,((_tmp68D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp230),((
_tmp68E[0]=& _tmp68D,Cyc_Tcutil_terr(loc,((_tmp68F="no field of struct/union has name %s",
_tag_dyneither(_tmp68F,sizeof(char),37))),_tag_dyneither(_tmp68E,sizeof(void*),1)))))));}
bad_type=0;goto _LL165;_LL16A:;_LL16B: goto _LL165;_LL165:;}if(bad_type){const char*
_tmp693;void*_tmp692[1];struct Cyc_String_pa_struct _tmp691;(_tmp691.tag=0,((
_tmp691.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp692[0]=& _tmp691,Cyc_Tcutil_terr(loc,((_tmp693="%s is not a known struct/union type",
_tag_dyneither(_tmp693,sizeof(char),36))),_tag_dyneither(_tmp692,sizeof(void*),1)))))));}
goto _LL160;}_LL163: if(*((int*)_tmp22F)!= 1)goto _LL160;_tmp231=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp22F)->f1;_LL164: {int bad_type=1;{void*_tmp240=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_AggrInfo _tmp241;union Cyc_Absyn_AggrInfoU_union _tmp242;struct Cyc_Absyn_Aggrdecl**
_tmp243;struct Cyc_List_List*_tmp244;struct Cyc_List_List*_tmp245;struct Cyc_Absyn_TunionFieldInfo
_tmp246;union Cyc_Absyn_TunionFieldInfoU_union _tmp247;struct Cyc_Absyn_Tunionfield*
_tmp248;_LL16D: if(_tmp240 <= (void*)4)goto _LL175;if(*((int*)_tmp240)!= 10)goto
_LL16F;_tmp241=((struct Cyc_Absyn_AggrType_struct*)_tmp240)->f1;_tmp242=_tmp241.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp240)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL16F;_tmp243=(_tmp242.KnownAggr).f1;_LL16E: if((*_tmp243)->impl == 0)
goto _LL16C;_tmp244=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp243)->impl))->fields;
goto _LL170;_LL16F: if(*((int*)_tmp240)!= 11)goto _LL171;_tmp244=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp240)->f2;_LL170: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp244)
<= _tmp231){const char*_tmp698;void*_tmp697[2];struct Cyc_Int_pa_struct _tmp696;
struct Cyc_Int_pa_struct _tmp695;(_tmp695.tag=1,((_tmp695.f1=(unsigned long)((int)
_tmp231),((_tmp696.tag=1,((_tmp696.f1=(unsigned long)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp244),((_tmp697[0]=& _tmp696,((_tmp697[1]=& _tmp695,Cyc_Tcutil_terr(
loc,((_tmp698="struct/union has too few components: %d <= %d",_tag_dyneither(
_tmp698,sizeof(char),46))),_tag_dyneither(_tmp697,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL16C;_LL171: if(*((int*)_tmp240)!= 9)goto _LL173;_tmp245=((struct
Cyc_Absyn_TupleType_struct*)_tmp240)->f1;_LL172: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp245)<= _tmp231){const char*_tmp69D;void*_tmp69C[2];struct
Cyc_Int_pa_struct _tmp69B;struct Cyc_Int_pa_struct _tmp69A;(_tmp69A.tag=1,((_tmp69A.f1=(
unsigned long)((int)_tmp231),((_tmp69B.tag=1,((_tmp69B.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp245),((_tmp69C[0]=& _tmp69B,((_tmp69C[
1]=& _tmp69A,Cyc_Tcutil_terr(loc,((_tmp69D="tuple has too few components: %d <= %d",
_tag_dyneither(_tmp69D,sizeof(char),39))),_tag_dyneither(_tmp69C,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL16C;_LL173: if(*((int*)_tmp240)!= 3)goto _LL175;_tmp246=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp240)->f1;_tmp247=_tmp246.field_info;if((((((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp240)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL175;_tmp248=(_tmp247.KnownTunionfield).f2;_LL174: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp248->typs)< _tmp231){const char*_tmp6A2;
void*_tmp6A1[2];struct Cyc_Int_pa_struct _tmp6A0;struct Cyc_Int_pa_struct _tmp69F;(
_tmp69F.tag=1,((_tmp69F.f1=(unsigned long)((int)_tmp231),((_tmp6A0.tag=1,((
_tmp6A0.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp248->typs),((_tmp6A1[0]=& _tmp6A0,((_tmp6A1[1]=& _tmp69F,Cyc_Tcutil_terr(loc,((
_tmp6A2="tunionfield has too few components: %d < %d",_tag_dyneither(_tmp6A2,
sizeof(char),44))),_tag_dyneither(_tmp6A1,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL16C;_LL175:;_LL176: goto _LL16C;_LL16C:;}if(bad_type){const char*_tmp6A6;
void*_tmp6A5[1];struct Cyc_String_pa_struct _tmp6A4;(_tmp6A4.tag=0,((_tmp6A4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp6A5[
0]=& _tmp6A4,Cyc_Tcutil_terr(loc,((_tmp6A6="%s is not a known type",
_tag_dyneither(_tmp6A6,sizeof(char),23))),_tag_dyneither(_tmp6A5,sizeof(void*),1)))))));}
goto _LL160;}_LL160:;}return Cyc_Absyn_uint_typ;}static void*Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){struct _RegionHandle _tmp258=_new_region("r");
struct _RegionHandle*r=& _tmp258;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp259=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp259,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp25A=t;struct Cyc_Absyn_PtrInfo
_tmp25B;void*_tmp25C;struct Cyc_Absyn_PtrAtts _tmp25D;void*_tmp25E;struct Cyc_Absyn_Conref*
_tmp25F;struct Cyc_Absyn_Conref*_tmp260;_LL178: if(_tmp25A <= (void*)4)goto _LL17A;
if(*((int*)_tmp25A)!= 4)goto _LL17A;_tmp25B=((struct Cyc_Absyn_PointerType_struct*)
_tmp25A)->f1;_tmp25C=(void*)_tmp25B.elt_typ;_tmp25D=_tmp25B.ptr_atts;_tmp25E=(
void*)_tmp25D.rgn;_tmp25F=_tmp25D.bounds;_tmp260=_tmp25D.zero_term;_LL179: Cyc_Tcenv_check_rgn_accessible(
_tmp259,loc,_tmp25E);Cyc_Tcutil_check_nonzero_bound(loc,_tmp25F);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp25C),(void*)1)){const char*_tmp6A9;void*_tmp6A8;(_tmp6A8=
0,Cyc_Tcutil_terr(loc,((_tmp6A9="can't dereference abstract pointer type",
_tag_dyneither(_tmp6A9,sizeof(char),40))),_tag_dyneither(_tmp6A8,sizeof(void*),0)));}{
void*_tmp263=_tmp25C;_npop_handler(0);return _tmp263;}_LL17A:;_LL17B: {const char*
_tmp6AD;void*_tmp6AC[1];struct Cyc_String_pa_struct _tmp6AB;void*_tmp267=(_tmp6AB.tag=
0,((_tmp6AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp6AC[0]=& _tmp6AB,Cyc_Tcexp_expr_err(_tmp259,loc,topt,((_tmp6AD="expecting * or @ type but found %s",
_tag_dyneither(_tmp6AD,sizeof(char),35))),_tag_dyneither(_tmp6AC,sizeof(void*),1)))))));
_npop_handler(0);return _tmp267;}_LL177:;}};_pop_region(r);}static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f);static void*Cyc_Tcexp_tcAggrMember(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){Cyc_Tcexp_tcExpNoPromote(
te,0,e);{void*_tmp268=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_AggrInfo _tmp269;union Cyc_Absyn_AggrInfoU_union
_tmp26A;struct Cyc_Absyn_Aggrdecl**_tmp26B;struct Cyc_Absyn_Aggrdecl*_tmp26C;
struct Cyc_List_List*_tmp26D;void*_tmp26E;struct Cyc_List_List*_tmp26F;_LL17D: if(
_tmp268 <= (void*)4)goto _LL185;if(*((int*)_tmp268)!= 10)goto _LL17F;_tmp269=((
struct Cyc_Absyn_AggrType_struct*)_tmp268)->f1;_tmp26A=_tmp269.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmp268)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL17F;_tmp26B=(_tmp26A.KnownAggr).f1;_tmp26C=*_tmp26B;_tmp26D=_tmp269.targs;
_LL17E: {struct Cyc_Absyn_Aggrfield*_tmp272=Cyc_Absyn_lookup_decl_field(_tmp26C,f);
if(_tmp272 == 0){const char*_tmp6B2;void*_tmp6B1[2];struct Cyc_String_pa_struct
_tmp6B0;struct Cyc_String_pa_struct _tmp6AF;return(_tmp6AF.tag=0,((_tmp6AF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6B0.tag=0,((_tmp6B0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp26C->name)),((
_tmp6B1[0]=& _tmp6B0,((_tmp6B1[1]=& _tmp6AF,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6B2="type %s has no %s field",_tag_dyneither(_tmp6B2,sizeof(char),24))),
_tag_dyneither(_tmp6B1,sizeof(void*),2)))))))))))));}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp26C->impl))->exist_vars != 0){const char*_tmp6B5;void*_tmp6B4;
return(_tmp6B4=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6B5="must use pattern-matching to access fields of existential types",
_tag_dyneither(_tmp6B5,sizeof(char),64))),_tag_dyneither(_tmp6B4,sizeof(void*),0)));}{
void*t2;{struct _RegionHandle _tmp279=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp279;_push_region(rgn);{struct Cyc_List_List*_tmp27A=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp26C->tvs,_tmp26D);t2=Cyc_Tcutil_rsubstitute(rgn,
_tmp27A,(void*)_tmp272->type);if(((void*)_tmp26C->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t2)){const char*_tmp6B9;void*_tmp6B8[1];struct Cyc_String_pa_struct
_tmp6B7;void*_tmp27E=(_tmp6B7.tag=0,((_tmp6B7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp6B8[0]=& _tmp6B7,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6B9="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp6B9,sizeof(char),56))),_tag_dyneither(_tmp6B8,sizeof(void*),1)))))));
_npop_handler(0);return _tmp27E;}};_pop_region(rgn);}return t2;}}_LL17F: if(*((int*)
_tmp268)!= 11)goto _LL181;_tmp26E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp268)->f1;_tmp26F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp268)->f2;_LL180: {
struct Cyc_Absyn_Aggrfield*_tmp27F=Cyc_Absyn_lookup_field(_tmp26F,f);if(_tmp27F == 
0){const char*_tmp6BD;void*_tmp6BC[1];struct Cyc_String_pa_struct _tmp6BB;return(
_tmp6BB.tag=0,((_tmp6BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6BC[0]=& _tmp6BB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6BD="type has no %s field",
_tag_dyneither(_tmp6BD,sizeof(char),21))),_tag_dyneither(_tmp6BC,sizeof(void*),1)))))));}
if((_tmp26E == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp27F->type)){const char*_tmp6C1;void*_tmp6C0[1];struct Cyc_String_pa_struct
_tmp6BF;return(_tmp6BF.tag=0,((_tmp6BF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp6C0[0]=& _tmp6BF,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6C1="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp6C1,sizeof(char),56))),_tag_dyneither(_tmp6C0,sizeof(void*),1)))))));}return(
void*)_tmp27F->type;}_LL181: if(*((int*)_tmp268)!= 7)goto _LL183;{const char*
_tmp6C2;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp6C2="size",_tag_dyneither(
_tmp6C2,sizeof(char),5))))== 0))goto _LL183;}_LL182: goto _LL184;_LL183: if(*((int*)
_tmp268)!= 4)goto _LL185;{const char*_tmp6C3;if(!(Cyc_strcmp((struct _dyneither_ptr)*
f,((_tmp6C3="size",_tag_dyneither(_tmp6C3,sizeof(char),5))))== 0))goto _LL185;}
_LL184:{const char*_tmp6C7;void*_tmp6C6[1];struct Cyc_String_pa_struct _tmp6C5;(
_tmp6C5.tag=0,((_tmp6C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp6C6[0]=& _tmp6C5,Cyc_Tcutil_warn(e->loc,((_tmp6C7="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp6C7,sizeof(char),54))),_tag_dyneither(_tmp6C6,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp6CD;struct Cyc_List_List*_tmp6CC;struct Cyc_Absyn_Primop_e_struct*
_tmp6CB;(void*)(outer_e->r=(void*)((void*)((_tmp6CB=_cycalloc(sizeof(*_tmp6CB)),((
_tmp6CB[0]=((_tmp6CD.tag=3,((_tmp6CD.f1=(void*)((void*)19),((_tmp6CD.f2=((
_tmp6CC=_cycalloc(sizeof(*_tmp6CC)),((_tmp6CC->hd=e,((_tmp6CC->tl=0,_tmp6CC)))))),
_tmp6CD)))))),_tmp6CB))))));}return Cyc_Absyn_uint_typ;_LL185:;_LL186: {const char*
_tmp6CE;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp6CE="size",_tag_dyneither(
_tmp6CE,sizeof(char),5))))== 0){const char*_tmp6D2;void*_tmp6D1[1];struct Cyc_String_pa_struct
_tmp6D0;return(_tmp6D0.tag=0,((_tmp6D0.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp6D1[0]=& _tmp6D0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6D2="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp6D2,sizeof(char),44))),_tag_dyneither(_tmp6D1,sizeof(void*),1)))))));}
else{const char*_tmp6D6;void*_tmp6D5[1];struct Cyc_String_pa_struct _tmp6D4;return(
_tmp6D4.tag=0,((_tmp6D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp6D5[0]=& _tmp6D4,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp6D6="expecting struct or union, found %s",_tag_dyneither(
_tmp6D6,sizeof(char),36))),_tag_dyneither(_tmp6D5,sizeof(void*),1)))))));}}
_LL17C:;}}static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f);static void*Cyc_Tcexp_tcAggrArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*f){{struct _RegionHandle _tmp293=_new_region("r");struct
_RegionHandle*r=& _tmp293;_push_region(r);Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_notreadctxt(
r,te),0,e);;_pop_region(r);}{void*_tmp294=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp295;void*_tmp296;struct Cyc_Absyn_PtrAtts
_tmp297;void*_tmp298;struct Cyc_Absyn_Conref*_tmp299;struct Cyc_Absyn_Conref*
_tmp29A;_LL188: if(_tmp294 <= (void*)4)goto _LL18A;if(*((int*)_tmp294)!= 4)goto
_LL18A;_tmp295=((struct Cyc_Absyn_PointerType_struct*)_tmp294)->f1;_tmp296=(void*)
_tmp295.elt_typ;_tmp297=_tmp295.ptr_atts;_tmp298=(void*)_tmp297.rgn;_tmp299=
_tmp297.bounds;_tmp29A=_tmp297.zero_term;_LL189: Cyc_Tcutil_check_nonzero_bound(
loc,_tmp299);{void*_tmp29B=Cyc_Tcutil_compress(_tmp296);struct Cyc_Absyn_AggrInfo
_tmp29C;union Cyc_Absyn_AggrInfoU_union _tmp29D;struct Cyc_Absyn_Aggrdecl**_tmp29E;
struct Cyc_Absyn_Aggrdecl*_tmp29F;struct Cyc_List_List*_tmp2A0;void*_tmp2A1;struct
Cyc_List_List*_tmp2A2;_LL18D: if(_tmp29B <= (void*)4)goto _LL191;if(*((int*)_tmp29B)
!= 10)goto _LL18F;_tmp29C=((struct Cyc_Absyn_AggrType_struct*)_tmp29B)->f1;_tmp29D=
_tmp29C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp29B)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL18F;_tmp29E=(_tmp29D.KnownAggr).f1;_tmp29F=*_tmp29E;_tmp2A0=_tmp29C.targs;
_LL18E: {struct Cyc_Absyn_Aggrfield*_tmp2A3=Cyc_Absyn_lookup_decl_field(_tmp29F,f);
if(_tmp2A3 == 0){const char*_tmp6DB;void*_tmp6DA[2];struct Cyc_String_pa_struct
_tmp6D9;struct Cyc_String_pa_struct _tmp6D8;return(_tmp6D8.tag=0,((_tmp6D8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6D9.tag=0,((_tmp6D9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp29F->name)),((
_tmp6DA[0]=& _tmp6D9,((_tmp6DA[1]=& _tmp6D8,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6DB="type %s has no %s field",_tag_dyneither(_tmp6DB,sizeof(char),24))),
_tag_dyneither(_tmp6DA,sizeof(void*),2)))))))))))));}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp29F->impl))->exist_vars != 0){const char*_tmp6DE;void*_tmp6DD;
return(_tmp6DD=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6DE="must use pattern-matching to access fields of existential types",
_tag_dyneither(_tmp6DE,sizeof(char),64))),_tag_dyneither(_tmp6DD,sizeof(void*),0)));}{
void*t3;{struct _RegionHandle _tmp2AA=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp2AA;_push_region(rgn);{struct Cyc_List_List*_tmp2AB=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp29F->tvs,_tmp2A0);t3=Cyc_Tcutil_rsubstitute(rgn,
_tmp2AB,(void*)_tmp2A3->type);};_pop_region(rgn);}if(((void*)_tmp29F->kind == (
void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){const
char*_tmp6E2;void*_tmp6E1[1];struct Cyc_String_pa_struct _tmp6E0;return(_tmp6E0.tag=
0,((_tmp6E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6E1[0]=&
_tmp6E0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6E2="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp6E2,sizeof(char),56))),_tag_dyneither(_tmp6E1,sizeof(void*),1)))))));}
return t3;}}_LL18F: if(*((int*)_tmp29B)!= 11)goto _LL191;_tmp2A1=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp29B)->f1;_tmp2A2=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp29B)->f2;_LL190: {
struct Cyc_Absyn_Aggrfield*_tmp2AF=Cyc_Absyn_lookup_field(_tmp2A2,f);if(_tmp2AF == 
0){const char*_tmp6E6;void*_tmp6E5[1];struct Cyc_String_pa_struct _tmp6E4;return(
_tmp6E4.tag=0,((_tmp6E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6E5[0]=& _tmp6E4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6E6="type has no %s field",
_tag_dyneither(_tmp6E6,sizeof(char),21))),_tag_dyneither(_tmp6E5,sizeof(void*),1)))))));}
if((_tmp2A1 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2AF->type)){const char*_tmp6EA;void*_tmp6E9[1];struct Cyc_String_pa_struct
_tmp6E8;return(_tmp6E8.tag=0,((_tmp6E8.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp6E9[0]=& _tmp6E8,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6EA="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp6EA,sizeof(char),56))),_tag_dyneither(_tmp6E9,sizeof(void*),1)))))));}return(
void*)_tmp2AF->type;}_LL191:;_LL192: goto _LL18C;_LL18C:;}goto _LL187;_LL18A:;
_LL18B: goto _LL187;_LL187:;}{const char*_tmp6EE;void*_tmp6ED[1];struct Cyc_String_pa_struct
_tmp6EC;return(_tmp6EC.tag=0,((_tmp6EC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp6ED[0]=& _tmp6EC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6EE="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp6EE,sizeof(char),44))),_tag_dyneither(_tmp6ED,sizeof(void*),1)))))));}}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index);static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2BA;int _tmp2BB;struct _tuple7 _tmp2B9=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2BA=_tmp2B9.f1;_tmp2BB=_tmp2B9.f2;if(!
_tmp2BB){const char*_tmp6F1;void*_tmp6F0;return(_tmp6F0=0,Cyc_Tcexp_expr_err(te,
loc,0,((_tmp6F1="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp6F1,sizeof(char),47))),_tag_dyneither(_tmp6F0,sizeof(void*),0)));}{struct
_handler_cons _tmp2BE;_push_handler(& _tmp2BE);{int _tmp2C0=0;if(setjmp(_tmp2BE.handler))
_tmp2C0=1;if(!_tmp2C0){{void*_tmp2C1=(*((struct _tuple9*(*)(struct Cyc_List_List*x,
int n))Cyc_List_nth)(ts,(int)_tmp2BA)).f2;_npop_handler(0);return _tmp2C1;};
_pop_handler();}else{void*_tmp2BF=(void*)_exn_thrown;void*_tmp2C3=_tmp2BF;_LL194:
if(_tmp2C3 != Cyc_List_Nth)goto _LL196;_LL195: {const char*_tmp6F6;void*_tmp6F5[2];
struct Cyc_Int_pa_struct _tmp6F4;struct Cyc_Int_pa_struct _tmp6F3;return(_tmp6F3.tag=
1,((_tmp6F3.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts),((_tmp6F4.tag=1,((_tmp6F4.f1=(unsigned long)((int)_tmp2BA),((_tmp6F5[0]=&
_tmp6F4,((_tmp6F5[1]=& _tmp6F3,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6F6="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp6F6,sizeof(char),41))),_tag_dyneither(_tmp6F5,sizeof(void*),2)))))))))))));}
_LL196:;_LL197:(void)_throw(_tmp2C3);_LL193:;}}}}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp2C8=_new_region("r");struct
_RegionHandle*r=& _tmp2C8;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp2C9=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp2C9,0,e1);Cyc_Tcexp_tcExp(_tmp2C9,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2C9,e2)){const char*_tmp6FA;void*_tmp6F9[1];
struct Cyc_String_pa_struct _tmp6F8;void*_tmp2CD=(_tmp6F8.tag=0,((_tmp6F8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp6F9[0]=& _tmp6F8,Cyc_Tcexp_expr_err(_tmp2C9,e2->loc,topt,((_tmp6FA="expecting int subscript, found %s",
_tag_dyneither(_tmp6FA,sizeof(char),34))),_tag_dyneither(_tmp6F9,sizeof(void*),1)))))));
_npop_handler(0);return _tmp2CD;}{void*_tmp2CE=t1;struct Cyc_Absyn_PtrInfo _tmp2CF;
void*_tmp2D0;struct Cyc_Absyn_Tqual _tmp2D1;struct Cyc_Absyn_PtrAtts _tmp2D2;void*
_tmp2D3;struct Cyc_Absyn_Conref*_tmp2D4;struct Cyc_Absyn_Conref*_tmp2D5;struct Cyc_List_List*
_tmp2D6;_LL199: if(_tmp2CE <= (void*)4)goto _LL19D;if(*((int*)_tmp2CE)!= 4)goto
_LL19B;_tmp2CF=((struct Cyc_Absyn_PointerType_struct*)_tmp2CE)->f1;_tmp2D0=(void*)
_tmp2CF.elt_typ;_tmp2D1=_tmp2CF.elt_tq;_tmp2D2=_tmp2CF.ptr_atts;_tmp2D3=(void*)
_tmp2D2.rgn;_tmp2D4=_tmp2D2.bounds;_tmp2D5=_tmp2D2.zero_term;_LL19A: if(((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp2D5)){int emit_warning=
1;{union Cyc_Absyn_Constraint_union _tmp2D7=(Cyc_Absyn_compress_conref(_tmp2D4))->v;
void*_tmp2D8;struct Cyc_Absyn_Exp*_tmp2D9;void*_tmp2DA;_LL1A0: if((_tmp2D7.Eq_constr).tag
!= 0)goto _LL1A2;_tmp2D8=(_tmp2D7.Eq_constr).f1;if(_tmp2D8 <= (void*)1)goto _LL1A2;
if(*((int*)_tmp2D8)!= 0)goto _LL1A2;_tmp2D9=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2D8)->f1;_LL1A1: if(Cyc_Tcutil_is_const_exp(_tmp2C9,e2)){unsigned int _tmp2DC;
int _tmp2DD;struct _tuple7 _tmp2DB=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2DC=
_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;if(_tmp2DD){unsigned int _tmp2DF;int _tmp2E0;struct
_tuple7 _tmp2DE=Cyc_Evexp_eval_const_uint_exp(_tmp2D9);_tmp2DF=_tmp2DE.f1;_tmp2E0=
_tmp2DE.f2;if(_tmp2E0  && _tmp2DF > _tmp2DC)emit_warning=0;}}goto _LL19F;_LL1A2: if((
_tmp2D7.Eq_constr).tag != 0)goto _LL1A4;_tmp2DA=(_tmp2D7.Eq_constr).f1;if((int)
_tmp2DA != 0)goto _LL1A4;_LL1A3: emit_warning=0;goto _LL19F;_LL1A4:;_LL1A5: goto
_LL19F;_LL19F:;}if(emit_warning){const char*_tmp6FD;void*_tmp6FC;(_tmp6FC=0,Cyc_Tcutil_warn(
e2->loc,((_tmp6FD="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp6FD,sizeof(char),63))),_tag_dyneither(_tmp6FC,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp2C9,e2)){unsigned int _tmp2E4;int _tmp2E5;
struct _tuple7 _tmp2E3=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2E4=_tmp2E3.f1;
_tmp2E5=_tmp2E3.f2;if(_tmp2E5)Cyc_Tcutil_check_bound(loc,_tmp2E4,_tmp2D4);}else{
if(Cyc_Tcutil_is_bound_one(_tmp2D4) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp2D5)){const char*_tmp700;void*_tmp6FF;(_tmp6FF=0,Cyc_Tcutil_warn(
e1->loc,((_tmp700="subscript applied to pointer to one object",_tag_dyneither(
_tmp700,sizeof(char),43))),_tag_dyneither(_tmp6FF,sizeof(void*),0)));}Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2D4);}}Cyc_Tcenv_check_rgn_accessible(_tmp2C9,loc,_tmp2D3);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2D0),(void*)1)){const char*_tmp703;void*_tmp702;(_tmp702=
0,Cyc_Tcutil_terr(e1->loc,((_tmp703="can't subscript an abstract pointer",
_tag_dyneither(_tmp703,sizeof(char),36))),_tag_dyneither(_tmp702,sizeof(void*),0)));}{
void*_tmp2EA=_tmp2D0;_npop_handler(0);return _tmp2EA;}_LL19B: if(*((int*)_tmp2CE)
!= 9)goto _LL19D;_tmp2D6=((struct Cyc_Absyn_TupleType_struct*)_tmp2CE)->f1;_LL19C: {
void*_tmp2EB=Cyc_Tcexp_ithTupleType(_tmp2C9,loc,_tmp2D6,e2);_npop_handler(0);
return _tmp2EB;}_LL19D:;_LL19E: {const char*_tmp707;void*_tmp706[1];struct Cyc_String_pa_struct
_tmp705;void*_tmp2EF=(_tmp705.tag=0,((_tmp705.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp706[0]=& _tmp705,Cyc_Tcexp_expr_err(
_tmp2C9,loc,topt,((_tmp707="subscript applied to %s",_tag_dyneither(_tmp707,
sizeof(char),24))),_tag_dyneither(_tmp706,sizeof(void*),1)))))));_npop_handler(0);
return _tmp2EF;}_LL198:;}}};_pop_region(r);}static void*Cyc_Tcexp_tcTuple(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_List_List*
es);static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp2F0=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp2F1;
_LL1A7: if(_tmp2F0 <= (void*)4)goto _LL1A9;if(*((int*)_tmp2F0)!= 9)goto _LL1A9;
_tmp2F1=((struct Cyc_Absyn_TupleType_struct*)_tmp2F0)->f1;_LL1A8: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2F1)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(es))goto _LL1A6;for(0;es != 0;(es=es->tl,_tmp2F1=_tmp2F1->tl)){
void*_tmp2F2=(*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp2F1))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp2F2,(struct Cyc_Absyn_Exp*)es->hd);Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple9*)_tmp2F1->hd)).f2);{struct
_tuple9*_tmp70A;struct Cyc_List_List*_tmp709;fields=((_tmp709=_cycalloc(sizeof(*
_tmp709)),((_tmp709->hd=((_tmp70A=_cycalloc(sizeof(*_tmp70A)),((_tmp70A->f1=(*((
struct _tuple9*)_tmp2F1->hd)).f1,((_tmp70A->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp70A)))))),((_tmp709->tl=
fields,_tmp709))))));}}done=1;goto _LL1A6;_LL1A9:;_LL1AA: goto _LL1A6;_LL1A6:;}if(!
done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)
es->hd);{struct _tuple9*_tmp70D;struct Cyc_List_List*_tmp70C;fields=((_tmp70C=
_cycalloc(sizeof(*_tmp70C)),((_tmp70C->hd=((_tmp70D=_cycalloc(sizeof(*_tmp70D)),((
_tmp70D->f1=Cyc_Absyn_empty_tqual(0),((_tmp70D->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp70D)))))),((_tmp70C->tl=
fields,_tmp70C))))));}}{struct Cyc_Absyn_TupleType_struct _tmp710;struct Cyc_Absyn_TupleType_struct*
_tmp70F;return(void*)((_tmp70F=_cycalloc(sizeof(*_tmp70F)),((_tmp70F[0]=((
_tmp710.tag=9,((_tmp710.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmp710)))),_tmp70F))));}}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*t,struct Cyc_List_List*
des);static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple2*t,struct Cyc_List_List*des){const char*_tmp713;void*
_tmp712;return(_tmp712=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp713="tcCompoundLit",
_tag_dyneither(_tmp713,sizeof(char),14))),_tag_dyneither(_tmp712,sizeof(void*),0)));}
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**elt_topt,int zero_term,struct Cyc_List_List*des);static void*Cyc_Tcexp_tcArray(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**elt_topt,int
zero_term,struct Cyc_List_List*des){void*res_t2;{struct _RegionHandle _tmp2FB=
_new_region("r");struct _RegionHandle*r=& _tmp2FB;_push_region(r);{int _tmp2FC=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct
_tuple8*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(
struct _tuple8*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Const_e_struct
_tmp719;union Cyc_Absyn_Cnst_union _tmp718;struct Cyc_Absyn_Const_e_struct*_tmp717;
struct Cyc_Absyn_Const_e_struct*_tmp2FD=(_tmp717=_cycalloc(sizeof(*_tmp717)),((
_tmp717[0]=((_tmp719.tag=0,((_tmp719.f1=(union Cyc_Absyn_Cnst_union)(((_tmp718.Int_c).tag=
2,(((_tmp718.Int_c).f1=(void*)((void*)1),(((_tmp718.Int_c).f2=_tmp2FC,_tmp718)))))),
_tmp719)))),_tmp717)));struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp2FD,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp2FE=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2FC - 1);if(!Cyc_Tcutil_is_zero(
_tmp2FE)){const char*_tmp71C;void*_tmp71B;(_tmp71B=0,Cyc_Tcutil_terr(_tmp2FE->loc,((
_tmp71C="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp71C,
sizeof(char),45))),_tag_dyneither(_tmp71B,sizeof(void*),0)));}}{struct Cyc_Core_Opt*
_tmp71D;sz_exp->topt=((_tmp71D=_cycalloc(sizeof(*_tmp71D)),((_tmp71D->v=(void*)
Cyc_Absyn_uint_typ,_tmp71D))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp721;void*_tmp720[1];struct Cyc_String_pa_struct _tmp71F;(
_tmp71F.tag=0,((_tmp71F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp720[0]=& _tmp71F,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp721="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp721,sizeof(char),53))),_tag_dyneither(_tmp720,sizeof(void*),1)))))));}};
_pop_region(r);}{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*
ds=(*((struct _tuple8*)des->hd)).f1;if(ds != 0){void*_tmp308=(void*)ds->hd;struct
Cyc_Absyn_Exp*_tmp309;_LL1AC: if(*((int*)_tmp308)!= 1)goto _LL1AE;_LL1AD:{const
char*_tmp724;void*_tmp723;(_tmp723=0,Cyc_Tcutil_terr(loc,((_tmp724="only array index designators are supported",
_tag_dyneither(_tmp724,sizeof(char),43))),_tag_dyneither(_tmp723,sizeof(void*),0)));}
goto _LL1AB;_LL1AE: if(*((int*)_tmp308)!= 0)goto _LL1AB;_tmp309=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp308)->f1;_LL1AF: Cyc_Tcexp_tcExpInitializer(te,0,_tmp309);{unsigned int
_tmp30D;int _tmp30E;struct _tuple7 _tmp30C=Cyc_Evexp_eval_const_uint_exp(_tmp309);
_tmp30D=_tmp30C.f1;_tmp30E=_tmp30C.f2;if(!_tmp30E){const char*_tmp727;void*
_tmp726;(_tmp726=0,Cyc_Tcutil_terr(_tmp309->loc,((_tmp727="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp727,sizeof(char),47))),_tag_dyneither(_tmp726,sizeof(void*),0)));}
else{if(_tmp30D != offset){const char*_tmp72C;void*_tmp72B[2];struct Cyc_Int_pa_struct
_tmp72A;struct Cyc_Int_pa_struct _tmp729;(_tmp729.tag=1,((_tmp729.f1=(
unsigned long)((int)_tmp30D),((_tmp72A.tag=1,((_tmp72A.f1=(unsigned long)offset,((
_tmp72B[0]=& _tmp72A,((_tmp72B[1]=& _tmp729,Cyc_Tcutil_terr(_tmp309->loc,((_tmp72C="expecting index designator %d but found %d",
_tag_dyneither(_tmp72C,sizeof(char),43))),_tag_dyneither(_tmp72B,sizeof(void*),2)))))))))))));}}
goto _LL1AB;}_LL1AB:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term);static
void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);{void*_tmp315=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v);_LL1B1: if(_tmp315 <= (
void*)4)goto _LL1B3;if(*((int*)_tmp315)!= 18)goto _LL1B3;_LL1B2: goto _LL1B0;_LL1B3:;
_LL1B4: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*_tmp730;void*_tmp72F[
1];struct Cyc_String_pa_struct _tmp72E;(_tmp72E.tag=0,((_tmp72E.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v)),((_tmp72F[0]=& _tmp72E,Cyc_Tcutil_terr(bound->loc,((
_tmp730="expecting unsigned int, found %s",_tag_dyneither(_tmp730,sizeof(char),
33))),_tag_dyneither(_tmp72F,sizeof(void*),1)))))));}_LL1B0:;}if(!(vd->tq).real_const){
const char*_tmp733;void*_tmp732;(_tmp732=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp733="comprehension index variable is not declared const!",
_tag_dyneither(_tmp733,sizeof(char),52))),_tag_dyneither(_tmp732,sizeof(void*),0)));}{
struct _RegionHandle _tmp31B=_new_region("r");struct _RegionHandle*r=& _tmp31B;
_push_region(r);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);if(te2->le != 
0){te2=Cyc_Tcenv_new_block(r,loc,te2);te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
struct Cyc_Tcenv_Tenv*_tmp31C=te2;struct Cyc_Absyn_PtrInfo pinfo;void**_tmp31D=0;
struct Cyc_Absyn_Tqual*_tmp31E=0;struct Cyc_Absyn_Conref**_tmp31F=0;if(topt != 0){
void*_tmp320=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp321;struct Cyc_Absyn_ArrayInfo
_tmp322;void*_tmp323;void**_tmp324;struct Cyc_Absyn_Tqual _tmp325;struct Cyc_Absyn_Tqual*
_tmp326;struct Cyc_Absyn_Exp*_tmp327;struct Cyc_Absyn_Conref*_tmp328;struct Cyc_Absyn_Conref**
_tmp329;_LL1B6: if(_tmp320 <= (void*)4)goto _LL1BA;if(*((int*)_tmp320)!= 4)goto
_LL1B8;_tmp321=((struct Cyc_Absyn_PointerType_struct*)_tmp320)->f1;_LL1B7: pinfo=
_tmp321;_tmp31D=(void**)((void**)((void*)& pinfo.elt_typ));_tmp31E=(struct Cyc_Absyn_Tqual*)&
pinfo.elt_tq;_tmp31F=(struct Cyc_Absyn_Conref**)&(pinfo.ptr_atts).zero_term;goto
_LL1B5;_LL1B8: if(*((int*)_tmp320)!= 7)goto _LL1BA;_tmp322=((struct Cyc_Absyn_ArrayType_struct*)
_tmp320)->f1;_tmp323=(void*)_tmp322.elt_type;_tmp324=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp320)->f1).elt_type;_tmp325=_tmp322.tq;_tmp326=(struct Cyc_Absyn_Tqual*)&(((
struct Cyc_Absyn_ArrayType_struct*)_tmp320)->f1).tq;_tmp327=_tmp322.num_elts;
_tmp328=_tmp322.zero_term;_tmp329=(struct Cyc_Absyn_Conref**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp320)->f1).zero_term;_LL1B9: _tmp31D=(void**)_tmp324;_tmp31E=(struct Cyc_Absyn_Tqual*)
_tmp326;_tmp31F=(struct Cyc_Absyn_Conref**)_tmp329;goto _LL1B5;_LL1BA:;_LL1BB: goto
_LL1B5;_LL1B5:;}{void*t=Cyc_Tcexp_tcExp(_tmp31C,_tmp31D,body);if(_tmp31C->le == 0){
if(!Cyc_Tcutil_is_const_exp(_tmp31C,bound)){const char*_tmp736;void*_tmp735;(
_tmp735=0,Cyc_Tcutil_terr(bound->loc,((_tmp736="bound is not constant",
_tag_dyneither(_tmp736,sizeof(char),22))),_tag_dyneither(_tmp735,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(_tmp31C,body)){const char*_tmp739;void*_tmp738;(
_tmp738=0,Cyc_Tcutil_terr(bound->loc,((_tmp739="body is not constant",
_tag_dyneither(_tmp739,sizeof(char),21))),_tag_dyneither(_tmp738,sizeof(void*),0)));}}
if(_tmp31F != 0  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,*
_tmp31F)){struct Cyc_Absyn_Exp*_tmp32E=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*
_tmp73A;_tmp32E->topt=((_tmp73A=_cycalloc(sizeof(*_tmp73A)),((_tmp73A->v=(void*)
Cyc_Absyn_uint_typ,_tmp73A))));}bound=Cyc_Absyn_add_exp(bound,_tmp32E,0);{struct
Cyc_Core_Opt*_tmp73B;bound->topt=((_tmp73B=_cycalloc(sizeof(*_tmp73B)),((_tmp73B->v=(
void*)Cyc_Absyn_uint_typ,_tmp73B))));}*is_zero_term=1;}{void*_tmp331=Cyc_Absyn_array_typ(
t,_tmp31E == 0?Cyc_Absyn_empty_tqual(0):*_tmp31E,(struct Cyc_Absyn_Exp*)bound,
_tmp31F == 0?Cyc_Absyn_false_conref:*_tmp31F,0);void*_tmp332=_tmp331;
_npop_handler(0);return _tmp332;}}}};_pop_region(r);}}struct _tuple10{struct Cyc_Absyn_Aggrfield*
f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt);static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**
ad_opt){struct Cyc_Absyn_Aggrdecl*ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)
_check_null(*ad_opt);else{{struct _handler_cons _tmp333;_push_handler(& _tmp333);{
int _tmp335=0;if(setjmp(_tmp333.handler))_tmp335=1;if(!_tmp335){ad=*Cyc_Tcenv_lookup_aggrdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp334=(void*)_exn_thrown;void*_tmp337=
_tmp334;_LL1BD: if(_tmp337 != Cyc_Dict_Absent)goto _LL1BF;_LL1BE:{const char*_tmp73F;
void*_tmp73E[1];struct Cyc_String_pa_struct _tmp73D;(_tmp73D.tag=0,((_tmp73D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((
_tmp73E[0]=& _tmp73D,Cyc_Tcutil_terr(loc,((_tmp73F="unbound struct name %s",
_tag_dyneither(_tmp73F,sizeof(char),23))),_tag_dyneither(_tmp73E,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)0;_LL1BF:;_LL1C0:(void)_throw(_tmp337);_LL1BC:;}}}*
ad_opt=(struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if((void*)ad->kind == (void*)1){
const char*_tmp742;void*_tmp741;(_tmp741=0,Cyc_Tcutil_terr(loc,((_tmp742="expecting struct but found union",
_tag_dyneither(_tmp742,sizeof(char),33))),_tag_dyneither(_tmp741,sizeof(void*),0)));}
if(ad->impl == 0){{const char*_tmp745;void*_tmp744;(_tmp744=0,Cyc_Tcutil_terr(loc,((
_tmp745="can't build abstract struct",_tag_dyneither(_tmp745,sizeof(char),28))),
_tag_dyneither(_tmp744,sizeof(void*),0)));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}{struct _RegionHandle _tmp33F=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp33F;_push_region(rgn);{struct _tuple4 _tmp746;struct _tuple4 _tmp340=(_tmp746.f1=
Cyc_Tcenv_lookup_type_vars(te),((_tmp746.f2=rgn,_tmp746)));struct Cyc_List_List*
_tmp341=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp340,ad->tvs);struct Cyc_List_List*_tmp342=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp340,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp343=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp341);struct Cyc_List_List*_tmp344=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp342);struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp341,_tmp342);struct Cyc_Absyn_AggrType_struct _tmp755;union Cyc_Absyn_AggrInfoU_union
_tmp754;struct Cyc_Absyn_Aggrdecl**_tmp753;struct Cyc_Absyn_AggrInfo _tmp752;struct
Cyc_Absyn_AggrType_struct*_tmp751;struct Cyc_Absyn_AggrType_struct*_tmp346=(
_tmp751=_cycalloc(sizeof(*_tmp751)),((_tmp751[0]=((_tmp755.tag=10,((_tmp755.f1=((
_tmp752.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp754.KnownAggr).tag=1,(((
_tmp754.KnownAggr).f1=((_tmp753=_cycalloc(sizeof(*_tmp753)),((_tmp753[0]=ad,
_tmp753)))),_tmp754)))),((_tmp752.targs=_tmp343,_tmp752)))),_tmp755)))),_tmp751)));
struct Cyc_List_List*_tmp347=*ts;struct Cyc_List_List*_tmp348=_tmp344;while(
_tmp347 != 0  && _tmp348 != 0){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)0,1,(void*)_tmp347->hd);Cyc_Tcutil_unify((void*)_tmp347->hd,(void*)
_tmp348->hd);_tmp347=_tmp347->tl;_tmp348=_tmp348->tl;}if(_tmp347 != 0){const char*
_tmp758;void*_tmp757;(_tmp757=0,Cyc_Tcutil_terr(loc,((_tmp758="too many explicit witness types",
_tag_dyneither(_tmp758,sizeof(char),32))),_tag_dyneither(_tmp757,sizeof(void*),0)));}*
ts=_tmp344;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp346,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp34C;struct Cyc_Absyn_Aggrfield*
_tmp34D;struct Cyc_Absyn_Exp*_tmp34E;struct _tuple10*_tmp34B=(struct _tuple10*)
fields->hd;_tmp34C=*_tmp34B;_tmp34D=_tmp34C.f1;_tmp34E=_tmp34C.f2;{void*_tmp34F=
Cyc_Tcutil_rsubstitute(rgn,_tmp345,(void*)_tmp34D->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp34F,_tmp34E);if(!Cyc_Tcutil_coerce_arg(te,_tmp34E,_tmp34F)){{
const char*_tmp75F;void*_tmp75E[4];struct Cyc_String_pa_struct _tmp75D;struct Cyc_String_pa_struct
_tmp75C;struct Cyc_String_pa_struct _tmp75B;struct Cyc_String_pa_struct _tmp75A;(
_tmp75A.tag=0,((_tmp75A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp34E->topt))->v)),((_tmp75B.tag=0,((
_tmp75B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp34F)),((_tmp75C.tag=0,((_tmp75C.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp75D.tag=0,((_tmp75D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp34D->name),((_tmp75E[0]=&
_tmp75D,((_tmp75E[1]=& _tmp75C,((_tmp75E[2]=& _tmp75B,((_tmp75E[3]=& _tmp75A,Cyc_Tcutil_terr(
_tmp34E->loc,((_tmp75F="field %s of struct %s expects type %s != %s",
_tag_dyneither(_tmp75F,sizeof(char),44))),_tag_dyneither(_tmp75E,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp356=0;{struct Cyc_List_List*
_tmp357=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp357 != 0;_tmp357=_tmp357->tl){struct _tuple0*_tmp762;struct Cyc_List_List*
_tmp761;_tmp356=((_tmp761=_cycalloc(sizeof(*_tmp761)),((_tmp761->hd=((_tmp762=
_cycalloc(sizeof(*_tmp762)),((_tmp762->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp345,(*((
struct _tuple0*)_tmp357->hd)).f1),((_tmp762->f2=Cyc_Tcutil_rsubstitute(rgn,
_tmp345,(*((struct _tuple0*)_tmp357->hd)).f2),_tmp762)))))),((_tmp761->tl=_tmp356,
_tmp761))))));}}_tmp356=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp356);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp356);{void*_tmp35A=(void*)
_tmp346;_npop_handler(0);return _tmp35A;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args);static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){{struct _RegionHandle _tmp361=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp361;_push_region(rgn);{void*_tmp362=Cyc_Tcutil_compress(
ts);void*_tmp363;struct Cyc_List_List*_tmp364;_LL1C2: if(_tmp362 <= (void*)4)goto
_LL1C4;if(*((int*)_tmp362)!= 11)goto _LL1C4;_tmp363=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp362)->f1;_tmp364=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp362)->f2;_LL1C3:
if(_tmp363 == (void*)1){const char*_tmp765;void*_tmp764;(_tmp764=0,Cyc_Tcutil_terr(
loc,((_tmp765="expecting struct but found union",_tag_dyneither(_tmp765,sizeof(
char),33))),_tag_dyneither(_tmp764,sizeof(void*),0)));}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp364);for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp368;
struct Cyc_Absyn_Aggrfield*_tmp369;struct Cyc_Absyn_Exp*_tmp36A;struct _tuple10*
_tmp367=(struct _tuple10*)fields->hd;_tmp368=*_tmp367;_tmp369=_tmp368.f1;_tmp36A=
_tmp368.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)((void**)((void*)& _tmp369->type)),
_tmp36A);if(!Cyc_Tcutil_coerce_arg(te,_tmp36A,(void*)_tmp369->type)){{const char*
_tmp76B;void*_tmp76A[3];struct Cyc_String_pa_struct _tmp769;struct Cyc_String_pa_struct
_tmp768;struct Cyc_String_pa_struct _tmp767;(_tmp767.tag=0,((_tmp767.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp36A->topt))->v)),((_tmp768.tag=0,((_tmp768.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmp369->type)),((
_tmp769.tag=0,((_tmp769.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp369->name),((
_tmp76A[0]=& _tmp769,((_tmp76A[1]=& _tmp768,((_tmp76A[2]=& _tmp767,Cyc_Tcutil_terr(
_tmp36A->loc,((_tmp76B="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp76B,sizeof(char),41))),_tag_dyneither(_tmp76A,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1C1;}_LL1C4:;_LL1C5: {const char*_tmp76E;
void*_tmp76D;(_tmp76D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp76E="tcAnonStruct: wrong type",_tag_dyneither(_tmp76E,
sizeof(char),25))),_tag_dyneither(_tmp76D,sizeof(void*),0)));}_LL1C1:;};
_pop_region(rgn);}return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Absyn_Tuniondecl*tud,struct Cyc_Absyn_Tunionfield*tuf);static void*
Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*tud,
struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp372=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp372;_push_region(rgn);{struct _tuple4 _tmp76F;struct
_tuple4 _tmp373=(_tmp76F.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp76F.f2=rgn,
_tmp76F)));struct Cyc_List_List*_tmp374=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp373,tud->tvs);struct Cyc_List_List*_tmp375=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp374);struct Cyc_Absyn_TunionFieldType_struct _tmp779;union Cyc_Absyn_TunionFieldInfoU_union
_tmp778;struct Cyc_Absyn_TunionFieldInfo _tmp777;struct Cyc_Absyn_TunionFieldType_struct*
_tmp776;void*res=(void*)((_tmp776=_cycalloc(sizeof(*_tmp776)),((_tmp776[0]=((
_tmp779.tag=3,((_tmp779.f1=((_tmp777.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)(((
_tmp778.KnownTunionfield).tag=1,(((_tmp778.KnownTunionfield).f1=tud,(((_tmp778.KnownTunionfield).f2=
tuf,_tmp778)))))),((_tmp777.targs=_tmp375,_tmp777)))),_tmp779)))),_tmp776))));
if(topt != 0){void*_tmp376=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo
_tmp377;struct Cyc_List_List*_tmp378;struct Cyc_Core_Opt*_tmp379;_LL1C7: if(_tmp376
<= (void*)4)goto _LL1CB;if(*((int*)_tmp376)!= 3)goto _LL1C9;_LL1C8: Cyc_Tcutil_unify(*
topt,res);goto _LL1C6;_LL1C9: if(*((int*)_tmp376)!= 2)goto _LL1CB;_tmp377=((struct
Cyc_Absyn_TunionType_struct*)_tmp376)->f1;_tmp378=_tmp377.targs;_tmp379=_tmp377.rgn;
_LL1CA:{struct Cyc_List_List*a=_tmp375;for(0;a != 0  && _tmp378 != 0;(a=a->tl,
_tmp378=_tmp378->tl)){Cyc_Tcutil_unify((void*)a->hd,(void*)_tmp378->hd);}}if(tud->is_flat
 || tuf->typs == 0  && es == 0){{struct Cyc_Core_Opt*_tmp77A;e->topt=((_tmp77A=
_cycalloc(sizeof(*_tmp77A)),((_tmp77A->v=(void*)res,_tmp77A))));}{struct Cyc_Absyn_TunionType_struct
_tmp789;union Cyc_Absyn_TunionInfoU_union _tmp788;struct Cyc_Absyn_Tuniondecl**
_tmp787;struct Cyc_Absyn_TunionInfo _tmp786;struct Cyc_Absyn_TunionType_struct*
_tmp785;res=(void*)((_tmp785=_cycalloc(sizeof(*_tmp785)),((_tmp785[0]=((_tmp789.tag=
2,((_tmp789.f1=((_tmp786.tunion_info=(union Cyc_Absyn_TunionInfoU_union)(((
_tmp788.KnownTunion).tag=1,(((_tmp788.KnownTunion).f1=((_tmp787=_cycalloc(
sizeof(*_tmp787)),((_tmp787[0]=tud,_tmp787)))),_tmp788)))),((_tmp786.targs=
_tmp375,((_tmp786.rgn=_tmp379,_tmp786)))))),_tmp789)))),_tmp785))));}Cyc_Tcutil_unchecked_cast(
te,e,res,(void*)1);}goto _LL1C6;_LL1CB:;_LL1CC: goto _LL1C6;_LL1C6:;}{struct Cyc_List_List*
ts=tuf->typs;for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp374,(*((struct
_tuple9*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){{const char*_tmp791;const char*_tmp790;void*_tmp78F[3];struct Cyc_String_pa_struct
_tmp78E;struct Cyc_String_pa_struct _tmp78D;struct Cyc_String_pa_struct _tmp78C;(
_tmp78C.tag=0,((_tmp78C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt
== 0?(struct _dyneither_ptr)((_tmp791="?",_tag_dyneither(_tmp791,sizeof(char),2))):
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((
_tmp78D.tag=0,((_tmp78D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp78E.tag=0,((_tmp78E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp78F[0]=& _tmp78E,((_tmp78F[1]=& _tmp78D,((
_tmp78F[2]=& _tmp78C,Cyc_Tcutil_terr(e->loc,((_tmp790="tunion constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp790,sizeof(char),80))),_tag_dyneither(_tmp78F,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp795;void*_tmp794[1];
struct Cyc_String_pa_struct _tmp793;void*_tmp389=(_tmp793.tag=0,((_tmp793.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp794[0]=& _tmp793,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp795="too many arguments for tunion constructor %s",_tag_dyneither(
_tmp795,sizeof(char),45))),_tag_dyneither(_tmp794,sizeof(void*),1)))))));
_npop_handler(0);return _tmp389;}if(ts != 0){const char*_tmp799;void*_tmp798[1];
struct Cyc_String_pa_struct _tmp797;void*_tmp38D=(_tmp797.tag=0,((_tmp797.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp798[0]=& _tmp797,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp799="too few arguments for tunion constructor %s",
_tag_dyneither(_tmp799,sizeof(char),44))),_tag_dyneither(_tmp798,sizeof(void*),1)))))));
_npop_handler(0);return _tmp38D;}{void*_tmp38E=res;_npop_handler(0);return _tmp38E;}}};
_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(void*t);static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp394=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp395;
struct Cyc_Absyn_PtrAtts _tmp396;struct Cyc_Absyn_Conref*_tmp397;struct Cyc_Absyn_ArrayInfo
_tmp398;void*_tmp399;struct Cyc_List_List*_tmp39A;struct Cyc_Absyn_AggrInfo _tmp39B;
union Cyc_Absyn_AggrInfoU_union _tmp39C;struct Cyc_List_List*_tmp39D;struct Cyc_List_List*
_tmp39E;_LL1CE: if((int)_tmp394 != 0)goto _LL1D0;_LL1CF: return 1;_LL1D0: if(_tmp394 <= (
void*)4)goto _LL1DC;if(*((int*)_tmp394)!= 0)goto _LL1D2;_LL1D1: goto _LL1D3;_LL1D2:
if(*((int*)_tmp394)!= 1)goto _LL1D4;_LL1D3: goto _LL1D5;_LL1D4: if(*((int*)_tmp394)
!= 2)goto _LL1D6;_LL1D5: goto _LL1D7;_LL1D6: if(*((int*)_tmp394)!= 3)goto _LL1D8;
_LL1D7: return 0;_LL1D8: if(*((int*)_tmp394)!= 4)goto _LL1DA;_tmp395=((struct Cyc_Absyn_PointerType_struct*)
_tmp394)->f1;_tmp396=_tmp395.ptr_atts;_tmp397=_tmp396.nullable;_LL1D9: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp397);_LL1DA: if(*((
int*)_tmp394)!= 5)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if((int)_tmp394 != 1)goto
_LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(_tmp394 <= (void*)4)goto _LL1F8;if(*((int*)
_tmp394)!= 6)goto _LL1E0;_LL1DF: return 1;_LL1E0: if(*((int*)_tmp394)!= 7)goto _LL1E2;
_tmp398=((struct Cyc_Absyn_ArrayType_struct*)_tmp394)->f1;_tmp399=(void*)_tmp398.elt_type;
_LL1E1: return Cyc_Tcexp_zeroable_type(_tmp399);_LL1E2: if(*((int*)_tmp394)!= 8)
goto _LL1E4;_LL1E3: return 0;_LL1E4: if(*((int*)_tmp394)!= 9)goto _LL1E6;_tmp39A=((
struct Cyc_Absyn_TupleType_struct*)_tmp394)->f1;_LL1E5: for(0;(unsigned int)
_tmp39A;_tmp39A=_tmp39A->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp39A->hd)).f2))return 0;}return 1;_LL1E6: if(*((int*)_tmp394)!= 10)goto _LL1E8;
_tmp39B=((struct Cyc_Absyn_AggrType_struct*)_tmp394)->f1;_tmp39C=_tmp39B.aggr_info;
_tmp39D=_tmp39B.targs;_LL1E7: {struct Cyc_Absyn_Aggrdecl*_tmp39F=Cyc_Absyn_get_known_aggrdecl(
_tmp39C);if(_tmp39F->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp39F->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp3A0=_new_region("r");
struct _RegionHandle*r=& _tmp3A0;_push_region(r);{struct Cyc_List_List*_tmp3A1=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp39F->tvs,_tmp39D);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp39F->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp3A1,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3A2=0;_npop_handler(0);
return _tmp3A2;}}}{int _tmp3A3=1;_npop_handler(0);return _tmp3A3;}};_pop_region(r);}}
_LL1E8: if(*((int*)_tmp394)!= 12)goto _LL1EA;_LL1E9: return 1;_LL1EA: if(*((int*)
_tmp394)!= 18)goto _LL1EC;_LL1EB: return 1;_LL1EC: if(*((int*)_tmp394)!= 11)goto
_LL1EE;_tmp39E=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp394)->f2;_LL1ED: for(0;
_tmp39E != 0;_tmp39E=_tmp39E->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp39E->hd)->type))return 0;}return 1;_LL1EE: if(*((int*)_tmp394)!= 13)goto _LL1F0;
_LL1EF: return 1;_LL1F0: if(*((int*)_tmp394)!= 16)goto _LL1F2;_LL1F1: return 0;_LL1F2:
if(*((int*)_tmp394)!= 15)goto _LL1F4;_LL1F3: return 0;_LL1F4: if(*((int*)_tmp394)!= 
14)goto _LL1F6;_LL1F5: return 0;_LL1F6: if(*((int*)_tmp394)!= 17)goto _LL1F8;_LL1F7:
goto _LL1F9;_LL1F8: if((int)_tmp394 != 2)goto _LL1FA;_LL1F9: goto _LL1FB;_LL1FA: if((
int)_tmp394 != 3)goto _LL1FC;_LL1FB: goto _LL1FD;_LL1FC: if(_tmp394 <= (void*)4)goto
_LL1FE;if(*((int*)_tmp394)!= 19)goto _LL1FE;_LL1FD: goto _LL1FF;_LL1FE: if(_tmp394 <= (
void*)4)goto _LL200;if(*((int*)_tmp394)!= 20)goto _LL200;_LL1FF: goto _LL201;_LL200:
if(_tmp394 <= (void*)4)goto _LL1CD;if(*((int*)_tmp394)!= 21)goto _LL1CD;_LL201: {
const char*_tmp79D;void*_tmp79C[1];struct Cyc_String_pa_struct _tmp79B;(_tmp79B.tag=
0,((_tmp79B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp79C[0]=& _tmp79B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp79D="bad type `%s' in zeroable type",_tag_dyneither(
_tmp79D,sizeof(char),31))),_tag_dyneither(_tmp79C,sizeof(void*),1)))))));}_LL1CD:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t);static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || 
allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3A7=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3A8;void*_tmp3A9;_LL203: if(_tmp3A7 <= (void*)4)
goto _LL205;if(*((int*)_tmp3A7)!= 4)goto _LL205;_tmp3A8=((struct Cyc_Absyn_PointerType_struct*)
_tmp3A7)->f1;_tmp3A9=(void*)_tmp3A8.elt_typ;_LL204: Cyc_Tcutil_unify(_tmp3A9,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL202;_LL205:;_LL206: goto _LL202;_LL202:;}{const char*_tmp7A6;const char*
_tmp7A5;const char*_tmp7A4;void*_tmp7A3[2];struct Cyc_String_pa_struct _tmp7A2;
struct Cyc_String_pa_struct _tmp7A1;(_tmp7A1.tag=0,((_tmp7A1.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7A2.tag=0,((
_tmp7A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)((_tmp7A5="calloc",_tag_dyneither(_tmp7A5,sizeof(char),7))):(
struct _dyneither_ptr)((_tmp7A6="malloc",_tag_dyneither(_tmp7A6,sizeof(char),7))))),((
_tmp7A3[0]=& _tmp7A2,((_tmp7A3[1]=& _tmp7A1,Cyc_Tcutil_terr(loc,((_tmp7A4="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp7A4,sizeof(char),60))),_tag_dyneither(_tmp7A3,sizeof(void*),2)))))))))))));}}
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int*
is_calloc,int*is_fat);static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,
struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;if(ropt != 0){
struct Cyc_Absyn_RgnHandleType_struct _tmp7A9;struct Cyc_Absyn_RgnHandleType_struct*
_tmp7A8;void*expected_type=(void*)((_tmp7A8=_cycalloc(sizeof(*_tmp7A8)),((
_tmp7A8[0]=((_tmp7A9.tag=14,((_tmp7A9.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp7A9)))),_tmp7A8))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3B0=Cyc_Tcutil_compress(handle_type);void*_tmp3B1;_LL208: if(_tmp3B0 <= (
void*)4)goto _LL20A;if(*((int*)_tmp3B0)!= 14)goto _LL20A;_tmp3B1=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3B0)->f1;_LL209: rgn=_tmp3B1;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL207;_LL20A:;_LL20B:{const char*_tmp7AD;void*_tmp7AC[1];struct
Cyc_String_pa_struct _tmp7AB;(_tmp7AB.tag=0,((_tmp7AB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp7AC[0]=& _tmp7AB,
Cyc_Tcutil_terr(ropt->loc,((_tmp7AD="expecting region_t type but found %s",
_tag_dyneither(_tmp7AD,sizeof(char),37))),_tag_dyneither(_tmp7AC,sizeof(void*),1)))))));}
goto _LL207;_LL207:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0){const
char*_tmp7B0;void*_tmp7AF;(_tmp7AF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7B0="calloc with empty type",
_tag_dyneither(_tmp7B0,sizeof(char),23))),_tag_dyneither(_tmp7AF,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3B9=(void*)(*e)->r;void*_tmp3BA;void*
_tmp3BB;struct Cyc_List_List*_tmp3BC;struct Cyc_List_List _tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;struct Cyc_List_List*_tmp3BF;struct Cyc_List_List _tmp3C0;struct Cyc_Absyn_Exp*
_tmp3C1;struct Cyc_List_List*_tmp3C2;_LL20D: if(*((int*)_tmp3B9)!= 18)goto _LL20F;
_tmp3BA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3B9)->f1;_LL20E:
elt_type=_tmp3BA;{void**_tmp7B1;*t=(void**)((_tmp7B1=_cycalloc(sizeof(*_tmp7B1)),((
_tmp7B1[0]=elt_type,_tmp7B1))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL20C;_LL20F: if(*((int*)
_tmp3B9)!= 3)goto _LL211;_tmp3BB=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B9)->f1;if((int)_tmp3BB != 1)goto _LL211;_tmp3BC=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B9)->f2;if(_tmp3BC == 0)goto _LL211;_tmp3BD=*_tmp3BC;_tmp3BE=(struct Cyc_Absyn_Exp*)
_tmp3BD.hd;_tmp3BF=_tmp3BD.tl;if(_tmp3BF == 0)goto _LL211;_tmp3C0=*_tmp3BF;_tmp3C1=(
struct Cyc_Absyn_Exp*)_tmp3C0.hd;_tmp3C2=_tmp3C0.tl;if(_tmp3C2 != 0)goto _LL211;
_LL210:{struct _tuple0 _tmp7B2;struct _tuple0 _tmp3C5=(_tmp7B2.f1=(void*)_tmp3BE->r,((
_tmp7B2.f2=(void*)_tmp3C1->r,_tmp7B2)));void*_tmp3C6;void*_tmp3C7;void*_tmp3C8;
void*_tmp3C9;_LL214: _tmp3C6=_tmp3C5.f1;if(*((int*)_tmp3C6)!= 18)goto _LL216;
_tmp3C7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C6)->f1;_LL215: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3C7);elt_type=_tmp3C7;{void**_tmp7B3;*t=(void**)((_tmp7B3=
_cycalloc(sizeof(*_tmp7B3)),((_tmp7B3[0]=elt_type,_tmp7B3))));}num_elts=_tmp3C1;
one_elt=0;goto _LL213;_LL216: _tmp3C8=_tmp3C5.f2;if(*((int*)_tmp3C8)!= 18)goto
_LL218;_tmp3C9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C8)->f1;_LL217:
Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3C9);elt_type=_tmp3C9;{void**_tmp7B4;*
t=(void**)((_tmp7B4=_cycalloc(sizeof(*_tmp7B4)),((_tmp7B4[0]=elt_type,_tmp7B4))));}
num_elts=_tmp3BE;one_elt=0;goto _LL213;_LL218:;_LL219: goto No_sizeof;_LL213:;}goto
_LL20C;_LL211:;_LL212: No_sizeof: elt_type=Cyc_Absyn_char_typ;{void**_tmp7B5;*t=(
void**)((_tmp7B5=_cycalloc(sizeof(*_tmp7B5)),((_tmp7B5[0]=elt_type,_tmp7B5))));}
num_elts=*e;one_elt=0;goto _LL20C;_LL20C:;}*e=num_elts;*is_fat=!one_elt;{void*
_tmp3CD=elt_type;struct Cyc_Absyn_AggrInfo _tmp3CE;union Cyc_Absyn_AggrInfoU_union
_tmp3CF;struct Cyc_Absyn_Aggrdecl**_tmp3D0;struct Cyc_Absyn_Aggrdecl*_tmp3D1;
_LL21B: if(_tmp3CD <= (void*)4)goto _LL21D;if(*((int*)_tmp3CD)!= 10)goto _LL21D;
_tmp3CE=((struct Cyc_Absyn_AggrType_struct*)_tmp3CD)->f1;_tmp3CF=_tmp3CE.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3CD)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL21D;_tmp3D0=(_tmp3CF.KnownAggr).f1;_tmp3D1=*_tmp3D0;_LL21C: if(
_tmp3D1->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D1->impl))->exist_vars
!= 0){const char*_tmp7B8;void*_tmp7B7;(_tmp7B7=0,Cyc_Tcutil_terr(loc,((_tmp7B8="malloc with existential types not yet implemented",
_tag_dyneither(_tmp7B8,sizeof(char),50))),_tag_dyneither(_tmp7B7,sizeof(void*),0)));}
goto _LL21A;_LL21D:;_LL21E: goto _LL21A;_LL21A:;}{void*(*_tmp3D4)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;
struct Cyc_Absyn_Conref*_tmp3D5=Cyc_Absyn_false_conref;if(topt != 0){void*_tmp3D6=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3D7;struct Cyc_Absyn_PtrAtts
_tmp3D8;struct Cyc_Absyn_Conref*_tmp3D9;struct Cyc_Absyn_Conref*_tmp3DA;struct Cyc_Absyn_Conref*
_tmp3DB;_LL220: if(_tmp3D6 <= (void*)4)goto _LL222;if(*((int*)_tmp3D6)!= 4)goto
_LL222;_tmp3D7=((struct Cyc_Absyn_PointerType_struct*)_tmp3D6)->f1;_tmp3D8=
_tmp3D7.ptr_atts;_tmp3D9=_tmp3D8.nullable;_tmp3DA=_tmp3D8.bounds;_tmp3DB=_tmp3D8.zero_term;
_LL221: _tmp3D5=_tmp3DB;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3D9))_tmp3D4=Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp3DB) && !(*is_calloc)){{const char*_tmp7BB;void*
_tmp7BA;(_tmp7BA=0,Cyc_Tcutil_warn(loc,((_tmp7BB="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp7BB,sizeof(char),55))),_tag_dyneither(_tmp7BA,sizeof(void*),0)));}*
is_calloc=1;}{void*_tmp3DE=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp3DA);
struct Cyc_Absyn_Exp*_tmp3DF;_LL225: if((int)_tmp3DE != 0)goto _LL227;_LL226: goto
_LL224;_LL227: if(_tmp3DE <= (void*)1)goto _LL229;if(*((int*)_tmp3DE)!= 0)goto
_LL229;_tmp3DF=((struct Cyc_Absyn_Upper_b_struct*)_tmp3DE)->f1;if(!(!one_elt))
goto _LL229;_LL228: {int _tmp3E0=Cyc_Evexp_c_can_eval(num_elts);if(_tmp3E0  && Cyc_Evexp_same_const_exp(
_tmp3DF,num_elts)){*is_fat=0;return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_conref_val(_tmp3DA),_tmp3D5);}{void*_tmp3E1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(num_elts->topt))->v);void*_tmp3E2;_LL22C:
if(_tmp3E1 <= (void*)4)goto _LL22E;if(*((int*)_tmp3E1)!= 18)goto _LL22E;_tmp3E2=(
void*)((struct Cyc_Absyn_TagType_struct*)_tmp3E1)->f1;_LL22D: {struct Cyc_Absyn_Exp*
_tmp3E3=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp3E2,0),0,(
void*)1,0);if(Cyc_Evexp_same_const_exp(_tmp3E3,_tmp3DF)){*is_fat=0;return Cyc_Absyn_atb_typ(
elt_type,rgn,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_conref_val(_tmp3DA),_tmp3D5);}
goto _LL22B;}_LL22E:;_LL22F: goto _LL22B;_LL22B:;}goto _LL224;}_LL229:;_LL22A: goto
_LL224;_LL224:;}goto _LL21F;_LL222:;_LL223: goto _LL21F;_LL21F:;}if(!one_elt)
_tmp3D4=Cyc_Absyn_dyneither_typ;return _tmp3D4(elt_type,rgn,Cyc_Absyn_empty_tqual(
0),_tmp3D5);}}}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp3E4=Cyc_Tcutil_compress(t1);_LL231: if(
_tmp3E4 <= (void*)4)goto _LL233;if(*((int*)_tmp3E4)!= 7)goto _LL233;_LL232:{const
char*_tmp7BE;void*_tmp7BD;(_tmp7BD=0,Cyc_Tcutil_terr(loc,((_tmp7BE="cannot assign to an array",
_tag_dyneither(_tmp7BE,sizeof(char),26))),_tag_dyneither(_tmp7BD,sizeof(void*),0)));}
goto _LL230;_LL233:;_LL234: goto _LL230;_LL230:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp7C1;void*_tmp7C0;(_tmp7C0=0,Cyc_Tcutil_terr(loc,((
_tmp7C1="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp7C1,sizeof(char),58))),_tag_dyneither(_tmp7C0,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp7C4;void*_tmp7C3;return(_tmp7C3=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp7C4="swap non-lvalue",_tag_dyneither(_tmp7C4,sizeof(char),
16))),_tag_dyneither(_tmp7C3,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp7C7;void*_tmp7C6;return(_tmp7C6=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp7C7="swap non-lvalue",_tag_dyneither(_tmp7C7,sizeof(char),16))),
_tag_dyneither(_tmp7C6,sizeof(void*),0)));}{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp7CA;void*_tmp7C9;return(_tmp7C9=0,Cyc_Tcexp_expr_err(te,e1->loc,
topt,((_tmp7CA="swap value in zeroterm array",_tag_dyneither(_tmp7CA,sizeof(char),
29))),_tag_dyneither(_tmp7C9,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp7CD;void*_tmp7CC;return(_tmp7CC=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp7CD="swap value in zeroterm array",_tag_dyneither(_tmp7CD,
sizeof(char),29))),_tag_dyneither(_tmp7CC,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp7D2;void*_tmp7D1[2];struct Cyc_String_pa_struct _tmp7D0;struct Cyc_String_pa_struct
_tmp7CF;void*_tmp3F1=(_tmp7CF.tag=0,((_tmp7CF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7D0.tag=0,((_tmp7D0.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7D1[0]=&
_tmp7D0,((_tmp7D1[1]=& _tmp7CF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7D2="type mismatch: %s != %s",
_tag_dyneither(_tmp7D2,sizeof(char),24))),_tag_dyneither(_tmp7D1,sizeof(void*),2)))))))))))));
return _tmp3F1;}return(void*)0;}}}}int Cyc_Tcexp_in_stmt_exp=0;static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s);static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){{struct _RegionHandle _tmp3F6=_new_region("r");
struct _RegionHandle*r=& _tmp3F6;_push_region(r);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);Cyc_Tcexp_in_stmt_exp=
old_stmt_exp_state;};_pop_region(r);}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));while(1){void*_tmp3F7=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Stmt*
_tmp3F9;struct Cyc_Absyn_Stmt*_tmp3FA;struct Cyc_Absyn_Decl*_tmp3FB;struct Cyc_Absyn_Stmt*
_tmp3FC;_LL236: if(_tmp3F7 <= (void*)1)goto _LL23C;if(*((int*)_tmp3F7)!= 0)goto
_LL238;_tmp3F8=((struct Cyc_Absyn_Exp_s_struct*)_tmp3F7)->f1;_LL237: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3F8->topt))->v;_LL238: if(*((int*)_tmp3F7)!= 
1)goto _LL23A;_tmp3F9=((struct Cyc_Absyn_Seq_s_struct*)_tmp3F7)->f1;_tmp3FA=((
struct Cyc_Absyn_Seq_s_struct*)_tmp3F7)->f2;_LL239: s=_tmp3FA;continue;_LL23A: if(*((
int*)_tmp3F7)!= 11)goto _LL23C;_tmp3FB=((struct Cyc_Absyn_Decl_s_struct*)_tmp3F7)->f1;
_tmp3FC=((struct Cyc_Absyn_Decl_s_struct*)_tmp3F7)->f2;_LL23B: s=_tmp3FC;continue;
_LL23C:;_LL23D: {const char*_tmp7D5;void*_tmp7D4;return(_tmp7D4=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7D5="statement expression must end with expression",
_tag_dyneither(_tmp7D5,sizeof(char),46))),_tag_dyneither(_tmp7D4,sizeof(void*),0)));}
_LL235:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1);static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct
_tmp7D8;struct Cyc_Absyn_RgnHandleType_struct*_tmp7D7;void*expected_type=(void*)((
_tmp7D7=_cycalloc(sizeof(*_tmp7D7)),((_tmp7D7[0]=((_tmp7D8.tag=14,((_tmp7D8.f1=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp7D8)))),_tmp7D7))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)rgn_handle);void*_tmp3FF=Cyc_Tcutil_compress(
handle_type);void*_tmp400;_LL23F: if(_tmp3FF <= (void*)4)goto _LL241;if(*((int*)
_tmp3FF)!= 14)goto _LL241;_tmp400=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3FF)->f1;_LL240: rgn=_tmp400;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto
_LL23E;_LL241:;_LL242:{const char*_tmp7DC;void*_tmp7DB[1];struct Cyc_String_pa_struct
_tmp7DA;(_tmp7DA.tag=0,((_tmp7DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp7DB[0]=& _tmp7DA,Cyc_Tcutil_terr(
rgn_handle->loc,((_tmp7DC="expecting region_t type but found %s",_tag_dyneither(
_tmp7DC,sizeof(char),37))),_tag_dyneither(_tmp7DB,sizeof(void*),1)))))));}goto
_LL23E;_LL23E:;}{void*_tmp406=(void*)e1->r;struct Cyc_Core_Opt*_tmp407;struct Cyc_List_List*
_tmp408;struct Cyc_List_List*_tmp409;union Cyc_Absyn_Cnst_union _tmp40A;struct
_dyneither_ptr _tmp40B;_LL244: if(*((int*)_tmp406)!= 29)goto _LL246;_LL245: {void*
_tmp40C=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*_tmp40D=Cyc_Tcutil_compress(
_tmp40C);struct Cyc_Absyn_ArrayInfo _tmp40E;void*_tmp40F;struct Cyc_Absyn_Tqual
_tmp410;struct Cyc_Absyn_Exp*_tmp411;struct Cyc_Absyn_Conref*_tmp412;_LL24F: if(
_tmp40D <= (void*)4)goto _LL251;if(*((int*)_tmp40D)!= 7)goto _LL251;_tmp40E=((
struct Cyc_Absyn_ArrayType_struct*)_tmp40D)->f1;_tmp40F=(void*)_tmp40E.elt_type;
_tmp410=_tmp40E.tq;_tmp411=_tmp40E.num_elts;_tmp412=_tmp40E.zero_term;_LL250: {
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp411);void*b;{void*
_tmp413=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);
void*_tmp414;_LL254: if(_tmp413 <= (void*)4)goto _LL256;if(*((int*)_tmp413)!= 18)
goto _LL256;_tmp414=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp413)->f1;_LL255:{
struct Cyc_Absyn_Upper_b_struct _tmp7DF;struct Cyc_Absyn_Upper_b_struct*_tmp7DE;b=(
void*)((_tmp7DE=_cycalloc(sizeof(*_tmp7DE)),((_tmp7DE[0]=((_tmp7DF.tag=0,((
_tmp7DF.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp414,0),
0,(void*)1,0),_tmp7DF)))),_tmp7DE))));}goto _LL253;_LL256:;_LL257: if(Cyc_Tcutil_is_const_exp(
te,bnd)){struct Cyc_Absyn_Upper_b_struct _tmp7E2;struct Cyc_Absyn_Upper_b_struct*
_tmp7E1;b=(void*)((_tmp7E1=_cycalloc(sizeof(*_tmp7E1)),((_tmp7E1[0]=((_tmp7E2.tag=
0,((_tmp7E2.f1=bnd,_tmp7E2)))),_tmp7E1))));}else{b=(void*)0;}_LL253:;}{struct Cyc_Absyn_PointerType_struct
_tmp7EC;struct Cyc_Absyn_PtrAtts _tmp7EB;struct Cyc_Absyn_PtrInfo _tmp7EA;struct Cyc_Absyn_PointerType_struct*
_tmp7E9;void*res_typ=(void*)((_tmp7E9=_cycalloc(sizeof(*_tmp7E9)),((_tmp7E9[0]=((
_tmp7EC.tag=4,((_tmp7EC.f1=((_tmp7EA.elt_typ=(void*)_tmp40F,((_tmp7EA.elt_tq=
_tmp410,((_tmp7EA.ptr_atts=((_tmp7EB.rgn=(void*)rgn,((_tmp7EB.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp7EB.bounds=Cyc_Absyn_new_conref(
b),((_tmp7EB.zero_term=_tmp412,((_tmp7EB.ptrloc=0,_tmp7EB)))))))))),_tmp7EA)))))),
_tmp7EC)))),_tmp7E9))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp7ED;e->topt=((_tmp7ED=_cycalloc(
sizeof(*_tmp7ED)),((_tmp7ED->v=(void*)res_typ,_tmp7ED))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL251:;_LL252: {const char*
_tmp7F0;void*_tmp7EF;(_tmp7EF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7F0="tcNew: comprehension returned non-array type",
_tag_dyneither(_tmp7F0,sizeof(char),45))),_tag_dyneither(_tmp7EF,sizeof(void*),0)));}
_LL24E:;}_LL246: if(*((int*)_tmp406)!= 37)goto _LL248;_tmp407=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp406)->f1;_tmp408=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp406)->f2;
_LL247:{struct Cyc_Absyn_Array_e_struct _tmp7F3;struct Cyc_Absyn_Array_e_struct*
_tmp7F2;(void*)(e1->r=(void*)((void*)((_tmp7F2=_cycalloc(sizeof(*_tmp7F2)),((
_tmp7F2[0]=((_tmp7F3.tag=28,((_tmp7F3.f1=_tmp408,_tmp7F3)))),_tmp7F2))))));}
_tmp409=_tmp408;goto _LL249;_LL248: if(*((int*)_tmp406)!= 28)goto _LL24A;_tmp409=((
struct Cyc_Absyn_Array_e_struct*)_tmp406)->f1;_LL249: {void**elt_typ_opt=0;int
zero_term=0;if(topt != 0){void*_tmp422=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp423;void*_tmp424;void**_tmp425;struct Cyc_Absyn_Tqual _tmp426;struct Cyc_Absyn_PtrAtts
_tmp427;struct Cyc_Absyn_Conref*_tmp428;_LL259: if(_tmp422 <= (void*)4)goto _LL25B;
if(*((int*)_tmp422)!= 4)goto _LL25B;_tmp423=((struct Cyc_Absyn_PointerType_struct*)
_tmp422)->f1;_tmp424=(void*)_tmp423.elt_typ;_tmp425=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp422)->f1).elt_typ;_tmp426=_tmp423.elt_tq;_tmp427=_tmp423.ptr_atts;_tmp428=
_tmp427.zero_term;_LL25A: elt_typ_opt=(void**)_tmp425;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp428);goto _LL258;_LL25B:;
_LL25C: goto _LL258;_LL258:;}{void*_tmp429=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp409);{struct Cyc_Core_Opt*_tmp7F4;e1->topt=((_tmp7F4=
_cycalloc(sizeof(*_tmp7F4)),((_tmp7F4->v=(void*)_tmp429,_tmp7F4))));}{void*
res_typ;{void*_tmp42B=Cyc_Tcutil_compress(_tmp429);struct Cyc_Absyn_ArrayInfo
_tmp42C;void*_tmp42D;struct Cyc_Absyn_Tqual _tmp42E;struct Cyc_Absyn_Exp*_tmp42F;
struct Cyc_Absyn_Conref*_tmp430;_LL25E: if(_tmp42B <= (void*)4)goto _LL260;if(*((int*)
_tmp42B)!= 7)goto _LL260;_tmp42C=((struct Cyc_Absyn_ArrayType_struct*)_tmp42B)->f1;
_tmp42D=(void*)_tmp42C.elt_type;_tmp42E=_tmp42C.tq;_tmp42F=_tmp42C.num_elts;
_tmp430=_tmp42C.zero_term;_LL25F:{struct Cyc_Absyn_PointerType_struct _tmp809;
struct Cyc_Absyn_PtrAtts _tmp808;struct Cyc_Absyn_Upper_b_struct _tmp807;struct Cyc_Absyn_Upper_b_struct*
_tmp806;struct Cyc_Absyn_PtrInfo _tmp805;struct Cyc_Absyn_PointerType_struct*
_tmp804;res_typ=(void*)((_tmp804=_cycalloc(sizeof(*_tmp804)),((_tmp804[0]=((
_tmp809.tag=4,((_tmp809.f1=((_tmp805.elt_typ=(void*)_tmp42D,((_tmp805.elt_tq=
_tmp42E,((_tmp805.ptr_atts=((_tmp808.rgn=(void*)rgn,((_tmp808.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp808.bounds=Cyc_Absyn_new_conref((
void*)((_tmp806=_cycalloc(sizeof(*_tmp806)),((_tmp806[0]=((_tmp807.tag=0,((
_tmp807.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp42F),_tmp807)))),_tmp806))))),((
_tmp808.zero_term=_tmp430,((_tmp808.ptrloc=0,_tmp808)))))))))),_tmp805)))))),
_tmp809)))),_tmp804))));}if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp80A;e->topt=((_tmp80A=_cycalloc(
sizeof(*_tmp80A)),((_tmp80A->v=(void*)res_typ,_tmp80A))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL25D;_LL260:;_LL261: {const char*
_tmp80D;void*_tmp80C;(_tmp80C=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp80D="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp80D,sizeof(char),50))),_tag_dyneither(_tmp80C,sizeof(void*),0)));}
_LL25D:;}return res_typ;}}}_LL24A: if(*((int*)_tmp406)!= 0)goto _LL24C;_tmp40A=((
struct Cyc_Absyn_Const_e_struct*)_tmp406)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp406)->f1).String_c).tag != 5)goto _LL24C;_tmp40B=(_tmp40A.String_c).f1;_LL24B: {
void*_tmp43A=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(
void*)0,Cyc_Absyn_true_conref);void*_tmp43B=Cyc_Tcexp_tcExp(te,(void**)& _tmp43A,
e1);struct Cyc_Absyn_Upper_b_struct _tmp810;struct Cyc_Absyn_Upper_b_struct*_tmp80F;
return Cyc_Absyn_atb_typ(_tmp43B,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp80F=
_cycalloc(sizeof(*_tmp80F)),((_tmp80F[0]=((_tmp810.tag=0,((_tmp810.f1=Cyc_Absyn_uint_exp(
1,0),_tmp810)))),_tmp80F)))),Cyc_Absyn_false_conref);}_LL24C:;_LL24D: {void**
topt2=0;if(topt != 0){void*_tmp43E=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp43F;void*_tmp440;void**_tmp441;struct Cyc_Absyn_Tqual _tmp442;_LL263: if(
_tmp43E <= (void*)4)goto _LL267;if(*((int*)_tmp43E)!= 4)goto _LL265;_tmp43F=((
struct Cyc_Absyn_PointerType_struct*)_tmp43E)->f1;_tmp440=(void*)_tmp43F.elt_typ;
_tmp441=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp43E)->f1).elt_typ;
_tmp442=_tmp43F.elt_tq;_LL264: topt2=(void**)_tmp441;goto _LL262;_LL265: if(*((int*)
_tmp43E)!= 2)goto _LL267;_LL266:{void**_tmp811;topt2=((_tmp811=_cycalloc(sizeof(*
_tmp811)),((_tmp811[0]=*topt,_tmp811))));}goto _LL262;_LL267:;_LL268: goto _LL262;
_LL262:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);struct Cyc_Absyn_PointerType_struct
_tmp81B;struct Cyc_Absyn_PtrAtts _tmp81A;struct Cyc_Absyn_PtrInfo _tmp819;struct Cyc_Absyn_PointerType_struct*
_tmp818;void*res_typ=(void*)((_tmp818=_cycalloc(sizeof(*_tmp818)),((_tmp818[0]=((
_tmp81B.tag=4,((_tmp81B.f1=((_tmp819.elt_typ=(void*)telt,((_tmp819.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp819.ptr_atts=((_tmp81A.rgn=(void*)rgn,((_tmp81A.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp81A.bounds=Cyc_Absyn_bounds_one_conref,((_tmp81A.zero_term=
Cyc_Absyn_false_conref,((_tmp81A.ptrloc=0,_tmp81A)))))))))),_tmp819)))))),
_tmp81B)))),_tmp818))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp81C;e->topt=((_tmp81C=_cycalloc(
sizeof(*_tmp81C)),((_tmp81C->v=(void*)res_typ,_tmp81C))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL243:;}}void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp449=t;struct Cyc_Absyn_ArrayInfo
_tmp44A;void*_tmp44B;struct Cyc_Absyn_Tqual _tmp44C;struct Cyc_Absyn_Exp*_tmp44D;
struct Cyc_Absyn_Conref*_tmp44E;_LL26A: if(_tmp449 <= (void*)4)goto _LL26C;if(*((int*)
_tmp449)!= 7)goto _LL26C;_tmp44A=((struct Cyc_Absyn_ArrayType_struct*)_tmp449)->f1;
_tmp44B=(void*)_tmp44A.elt_type;_tmp44C=_tmp44A.tq;_tmp44D=_tmp44A.num_elts;
_tmp44E=_tmp44A.zero_term;_LL26B: {void*_tmp450;struct _tuple6 _tmp44F=Cyc_Tcutil_addressof_props(
te,e);_tmp450=_tmp44F.f2;{struct Cyc_Absyn_Upper_b_struct _tmp81F;struct Cyc_Absyn_Upper_b_struct*
_tmp81E;void*_tmp451=_tmp44D == 0?(void*)((void*)0):(void*)((_tmp81E=_cycalloc(
sizeof(*_tmp81E)),((_tmp81E[0]=((_tmp81F.tag=0,((_tmp81F.f1=(struct Cyc_Absyn_Exp*)
_tmp44D,_tmp81F)))),_tmp81E))));t=Cyc_Absyn_atb_typ(_tmp44B,_tmp450,_tmp44C,
_tmp451,_tmp44E);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL26C:;_LL26D: return t;_LL269:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(
e)){{const char*_tmp827;const char*_tmp826;void*_tmp825[3];struct Cyc_String_pa_struct
_tmp824;struct Cyc_String_pa_struct _tmp823;struct Cyc_String_pa_struct _tmp822;(
_tmp822.tag=0,((_tmp822.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp823.tag=0,((_tmp823.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp824.tag=0,((_tmp824.f1=(struct _dyneither_ptr)(topt == 0?(_tmp827="NULL",
_tag_dyneither(_tmp827,sizeof(char),5)):(struct _dyneither_ptr)Cyc_Absynpp_typ2string(*
topt)),((_tmp825[0]=& _tmp824,((_tmp825[1]=& _tmp823,((_tmp825[2]=& _tmp822,Cyc_fprintf(
Cyc_stderr,((_tmp826="topt=%s, e->topt->v=%s, e=%s\n",_tag_dyneither(_tmp826,
sizeof(char),30))),_tag_dyneither(_tmp825,sizeof(void*),3)))))))))))))))))));}{
const char*_tmp82A;void*_tmp829;(_tmp829=0,Cyc_Tcutil_terr(e->loc,((_tmp82A="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp82A,sizeof(char),49))),_tag_dyneither(_tmp829,sizeof(void*),0)));}}{
void*_tmp45C=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp45D;_LL26F: if(*((int*)
_tmp45C)!= 28)goto _LL271;_LL270: goto _LL272;_LL271: if(*((int*)_tmp45C)!= 29)goto
_LL273;_LL272: goto _LL274;_LL273: if(*((int*)_tmp45C)!= 0)goto _LL275;_tmp45D=((
struct Cyc_Absyn_Const_e_struct*)_tmp45C)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp45C)->f1).String_c).tag != 5)goto _LL275;_LL274: return t;_LL275:;_LL276: t=Cyc_Tcutil_compress(
t);{void*_tmp45E=t;struct Cyc_Absyn_ArrayInfo _tmp45F;void*_tmp460;struct Cyc_Absyn_Tqual
_tmp461;struct Cyc_Absyn_Exp*_tmp462;struct Cyc_Absyn_Conref*_tmp463;_LL278: if(
_tmp45E <= (void*)4)goto _LL27A;if(*((int*)_tmp45E)!= 7)goto _LL27A;_tmp45F=((
struct Cyc_Absyn_ArrayType_struct*)_tmp45E)->f1;_tmp460=(void*)_tmp45F.elt_type;
_tmp461=_tmp45F.tq;_tmp462=_tmp45F.num_elts;_tmp463=_tmp45F.zero_term;_LL279: {
void*_tmp465;struct _tuple6 _tmp464=Cyc_Tcutil_addressof_props(te,e);_tmp465=
_tmp464.f2;{struct Cyc_Absyn_Upper_b_struct _tmp82D;struct Cyc_Absyn_Upper_b_struct*
_tmp82C;void*b=_tmp462 == 0?(void*)((void*)0):(void*)((_tmp82C=_cycalloc(sizeof(*
_tmp82C)),((_tmp82C[0]=((_tmp82D.tag=0,((_tmp82D.f1=(struct Cyc_Absyn_Exp*)
_tmp462,_tmp82D)))),_tmp82C))));t=Cyc_Absyn_atb_typ(_tmp460,_tmp465,_tmp461,b,
_tmp463);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL27A:;_LL27B:
return t;_LL277:;}_LL26E:;}}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp468=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp469;_LL27D: if(*((int*)_tmp468)!= 13)goto _LL27F;_tmp469=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp468)->f1;_LL27E: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp469);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp469->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp469->topt))->v,
0));e->topt=_tmp469->topt;goto _LL27C;_LL27F:;_LL280: Cyc_Tcexp_tcExpNoInst(te,
topt,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*_tmp46A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp46B;void*_tmp46C;struct Cyc_Absyn_Tqual _tmp46D;struct Cyc_Absyn_PtrAtts _tmp46E;
void*_tmp46F;struct Cyc_Absyn_Conref*_tmp470;struct Cyc_Absyn_Conref*_tmp471;
struct Cyc_Absyn_Conref*_tmp472;_LL282: if(_tmp46A <= (void*)4)goto _LL284;if(*((int*)
_tmp46A)!= 4)goto _LL284;_tmp46B=((struct Cyc_Absyn_PointerType_struct*)_tmp46A)->f1;
_tmp46C=(void*)_tmp46B.elt_typ;_tmp46D=_tmp46B.elt_tq;_tmp46E=_tmp46B.ptr_atts;
_tmp46F=(void*)_tmp46E.rgn;_tmp470=_tmp46E.nullable;_tmp471=_tmp46E.bounds;
_tmp472=_tmp46E.zero_term;_LL283:{void*_tmp473=Cyc_Tcutil_compress(_tmp46C);
struct Cyc_Absyn_FnInfo _tmp474;struct Cyc_List_List*_tmp475;struct Cyc_Core_Opt*
_tmp476;void*_tmp477;struct Cyc_List_List*_tmp478;int _tmp479;struct Cyc_Absyn_VarargInfo*
_tmp47A;struct Cyc_List_List*_tmp47B;struct Cyc_List_List*_tmp47C;_LL287: if(
_tmp473 <= (void*)4)goto _LL289;if(*((int*)_tmp473)!= 8)goto _LL289;_tmp474=((
struct Cyc_Absyn_FnType_struct*)_tmp473)->f1;_tmp475=_tmp474.tvars;_tmp476=
_tmp474.effect;_tmp477=(void*)_tmp474.ret_typ;_tmp478=_tmp474.args;_tmp479=
_tmp474.c_varargs;_tmp47A=_tmp474.cyc_varargs;_tmp47B=_tmp474.rgn_po;_tmp47C=
_tmp474.attributes;_LL288: if(_tmp475 != 0){struct _RegionHandle _tmp47D=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp47D;_push_region(rgn);{struct _tuple4 _tmp82E;struct
_tuple4 _tmp47E=(_tmp82E.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp82E.f2=rgn,
_tmp82E)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp47E,_tmp475);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp834;struct Cyc_Absyn_FnInfo
_tmp833;struct Cyc_Absyn_FnType_struct*_tmp832;void*ftyp=Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmp832=_cycalloc(sizeof(*_tmp832)),((_tmp832[0]=((_tmp834.tag=
8,((_tmp834.f1=((_tmp833.tvars=0,((_tmp833.effect=_tmp476,((_tmp833.ret_typ=(
void*)_tmp477,((_tmp833.args=_tmp478,((_tmp833.c_varargs=_tmp479,((_tmp833.cyc_varargs=
_tmp47A,((_tmp833.rgn_po=_tmp47B,((_tmp833.attributes=_tmp47C,_tmp833)))))))))))))))),
_tmp834)))),_tmp832)))));struct Cyc_Absyn_PointerType_struct _tmp83E;struct Cyc_Absyn_PtrAtts
_tmp83D;struct Cyc_Absyn_PtrInfo _tmp83C;struct Cyc_Absyn_PointerType_struct*
_tmp83B;struct Cyc_Absyn_PointerType_struct*_tmp47F=(_tmp83B=_cycalloc(sizeof(*
_tmp83B)),((_tmp83B[0]=((_tmp83E.tag=4,((_tmp83E.f1=((_tmp83C.elt_typ=(void*)
ftyp,((_tmp83C.elt_tq=_tmp46D,((_tmp83C.ptr_atts=((_tmp83D.rgn=(void*)_tmp46F,((
_tmp83D.nullable=_tmp470,((_tmp83D.bounds=_tmp471,((_tmp83D.zero_term=_tmp472,((
_tmp83D.ptrloc=0,_tmp83D)))))))))),_tmp83C)))))),_tmp83E)))),_tmp83B)));struct
Cyc_Absyn_Exp*_tmp480=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct
_tmp841;struct Cyc_Absyn_Instantiate_e_struct*_tmp840;(void*)(e->r=(void*)((void*)((
_tmp840=_cycalloc(sizeof(*_tmp840)),((_tmp840[0]=((_tmp841.tag=14,((_tmp841.f1=
_tmp480,((_tmp841.f2=ts,_tmp841)))))),_tmp840))))));}{struct Cyc_Core_Opt*_tmp842;
e->topt=((_tmp842=_cycalloc(sizeof(*_tmp842)),((_tmp842->v=(void*)((void*)
_tmp47F),_tmp842))));}};_pop_region(rgn);}goto _LL286;_LL289:;_LL28A: goto _LL286;
_LL286:;}goto _LL281;_LL284:;_LL285: goto _LL281;_LL281:;}goto _LL27C;_LL27C:;}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*
loc=e->loc;void*t;{void*_tmp48C=(void*)e->r;struct Cyc_Absyn_Exp*_tmp48D;struct
_tuple1*_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_List_List*_tmp490;struct
Cyc_Core_Opt*_tmp491;struct Cyc_List_List*_tmp492;union Cyc_Absyn_Cnst_union
_tmp493;union Cyc_Absyn_Cnst_union*_tmp494;struct _tuple1*_tmp495;void*_tmp496;
void*_tmp497;struct Cyc_List_List*_tmp498;struct Cyc_Absyn_Exp*_tmp499;void*
_tmp49A;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Core_Opt*_tmp49C;struct Cyc_Absyn_Exp*
_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*
_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*
_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A5;struct Cyc_Absyn_Exp*
_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_List_List*_tmp4A8;struct Cyc_Absyn_VarargCallInfo*
_tmp4A9;struct Cyc_Absyn_VarargCallInfo**_tmp4AA;struct Cyc_Absyn_Exp*_tmp4AB;
struct Cyc_Absyn_Exp*_tmp4AC;struct Cyc_List_List*_tmp4AD;void*_tmp4AE;struct Cyc_Absyn_Exp*
_tmp4AF;void*_tmp4B0;void**_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;struct Cyc_Absyn_Exp*
_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B5;void*_tmp4B6;
void*_tmp4B7;void*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct Cyc_Absyn_Exp*
_tmp4BA;struct _dyneither_ptr*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BC;struct
_dyneither_ptr*_tmp4BD;struct Cyc_Absyn_Exp*_tmp4BE;struct Cyc_Absyn_Exp*_tmp4BF;
struct Cyc_List_List*_tmp4C0;struct _tuple2*_tmp4C1;struct Cyc_List_List*_tmp4C2;
struct Cyc_List_List*_tmp4C3;struct Cyc_Absyn_Stmt*_tmp4C4;struct Cyc_Absyn_Vardecl*
_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C7;int _tmp4C8;int*
_tmp4C9;struct _tuple1*_tmp4CA;struct _tuple1**_tmp4CB;struct Cyc_List_List*_tmp4CC;
struct Cyc_List_List**_tmp4CD;struct Cyc_List_List*_tmp4CE;struct Cyc_Absyn_Aggrdecl*
_tmp4CF;struct Cyc_Absyn_Aggrdecl**_tmp4D0;void*_tmp4D1;struct Cyc_List_List*
_tmp4D2;struct Cyc_List_List*_tmp4D3;struct Cyc_Absyn_Tuniondecl*_tmp4D4;struct Cyc_Absyn_Tunionfield*
_tmp4D5;struct _tuple1*_tmp4D6;struct _tuple1**_tmp4D7;struct Cyc_Absyn_Enumdecl*
_tmp4D8;struct Cyc_Absyn_Enumfield*_tmp4D9;struct _tuple1*_tmp4DA;struct _tuple1**
_tmp4DB;void*_tmp4DC;struct Cyc_Absyn_Enumfield*_tmp4DD;struct Cyc_Absyn_MallocInfo
_tmp4DE;int _tmp4DF;int*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4E1;void**_tmp4E2;void***
_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Exp**_tmp4E5;int _tmp4E6;int*
_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;void*_tmp4EA;
_LL28C: if(*((int*)_tmp48C)!= 13)goto _LL28E;_tmp48D=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp48C)->f1;_LL28D: Cyc_Tcexp_tcExpNoInst(te,0,_tmp48D);return;_LL28E: if(*((int*)
_tmp48C)!= 2)goto _LL290;_tmp48E=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp48C)->f1;
_LL28F: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp48E);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL290: if(*((int*)_tmp48C)!= 10)goto _LL292;_tmp48F=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp48C)->f1;_tmp490=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp48C)->f2;_LL291:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp48F,_tmp490);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL292: if(*((int*)_tmp48C)!= 37)goto _LL294;_tmp491=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp48C)->f1;_tmp492=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp48C)->f2;
_LL293: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp492);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL294: if(*((int*)_tmp48C)!= 0)goto _LL296;_tmp493=((struct Cyc_Absyn_Const_e_struct*)
_tmp48C)->f1;_tmp494=(union Cyc_Absyn_Cnst_union*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp48C)->f1;_LL295: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst_union*)
_tmp494,e);goto _LL28B;_LL296: if(*((int*)_tmp48C)!= 1)goto _LL298;_tmp495=((struct
Cyc_Absyn_Var_e_struct*)_tmp48C)->f1;_tmp496=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp48C)->f2;_LL297: t=Cyc_Tcexp_tcVar(te,loc,_tmp495,_tmp496);goto _LL28B;_LL298:
if(*((int*)_tmp48C)!= 3)goto _LL29A;_tmp497=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp48C)->f1;_tmp498=((struct Cyc_Absyn_Primop_e_struct*)_tmp48C)->f2;_LL299: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp497,_tmp498);goto _LL28B;_LL29A: if(*((int*)
_tmp48C)!= 5)goto _LL29C;_tmp499=((struct Cyc_Absyn_Increment_e_struct*)_tmp48C)->f1;
_tmp49A=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp48C)->f2;_LL29B: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp499,_tmp49A);goto _LL28B;_LL29C: if(*((int*)_tmp48C)!= 4)goto
_LL29E;_tmp49B=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp48C)->f1;_tmp49C=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp48C)->f2;_tmp49D=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp48C)->f3;_LL29D: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp49B,_tmp49C,_tmp49D);
goto _LL28B;_LL29E: if(*((int*)_tmp48C)!= 6)goto _LL2A0;_tmp49E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp48C)->f1;_tmp49F=((struct Cyc_Absyn_Conditional_e_struct*)_tmp48C)->f2;
_tmp4A0=((struct Cyc_Absyn_Conditional_e_struct*)_tmp48C)->f3;_LL29F: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp49E,_tmp49F,_tmp4A0);goto _LL28B;_LL2A0: if(*((int*)_tmp48C)!= 7)
goto _LL2A2;_tmp4A1=((struct Cyc_Absyn_And_e_struct*)_tmp48C)->f1;_tmp4A2=((struct
Cyc_Absyn_And_e_struct*)_tmp48C)->f2;_LL2A1: t=Cyc_Tcexp_tcAnd(te,loc,_tmp4A1,
_tmp4A2);goto _LL28B;_LL2A2: if(*((int*)_tmp48C)!= 8)goto _LL2A4;_tmp4A3=((struct
Cyc_Absyn_Or_e_struct*)_tmp48C)->f1;_tmp4A4=((struct Cyc_Absyn_Or_e_struct*)
_tmp48C)->f2;_LL2A3: t=Cyc_Tcexp_tcOr(te,loc,_tmp4A3,_tmp4A4);goto _LL28B;_LL2A4:
if(*((int*)_tmp48C)!= 9)goto _LL2A6;_tmp4A5=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp48C)->f1;_tmp4A6=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp48C)->f2;_LL2A5: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4A5,_tmp4A6);goto _LL28B;_LL2A6: if(*((int*)
_tmp48C)!= 11)goto _LL2A8;_tmp4A7=((struct Cyc_Absyn_FnCall_e_struct*)_tmp48C)->f1;
_tmp4A8=((struct Cyc_Absyn_FnCall_e_struct*)_tmp48C)->f2;_tmp4A9=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp48C)->f3;_tmp4AA=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp48C)->f3;_LL2A7: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4A7,_tmp4A8,_tmp4AA);
goto _LL28B;_LL2A8: if(*((int*)_tmp48C)!= 12)goto _LL2AA;_tmp4AB=((struct Cyc_Absyn_Throw_e_struct*)
_tmp48C)->f1;_LL2A9: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4AB);goto _LL28B;_LL2AA:
if(*((int*)_tmp48C)!= 14)goto _LL2AC;_tmp4AC=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp48C)->f1;_tmp4AD=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp48C)->f2;_LL2AB:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4AC,_tmp4AD);goto _LL28B;_LL2AC: if(*((
int*)_tmp48C)!= 15)goto _LL2AE;_tmp4AE=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp48C)->f1;_tmp4AF=((struct Cyc_Absyn_Cast_e_struct*)_tmp48C)->f2;_tmp4B0=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp48C)->f4;_tmp4B1=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp48C)->f4);_LL2AD: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4AE,_tmp4AF,(void**)
_tmp4B1);goto _LL28B;_LL2AE: if(*((int*)_tmp48C)!= 16)goto _LL2B0;_tmp4B2=((struct
Cyc_Absyn_Address_e_struct*)_tmp48C)->f1;_LL2AF: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4B2);goto _LL28B;_LL2B0: if(*((int*)_tmp48C)!= 17)goto _LL2B2;_tmp4B3=((
struct Cyc_Absyn_New_e_struct*)_tmp48C)->f1;_tmp4B4=((struct Cyc_Absyn_New_e_struct*)
_tmp48C)->f2;_LL2B1: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4B3,e,_tmp4B4);goto _LL28B;
_LL2B2: if(*((int*)_tmp48C)!= 19)goto _LL2B4;_tmp4B5=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp48C)->f1;_LL2B3: {void*_tmp4EB=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4B5);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4EB);goto _LL28B;}_LL2B4: if(*((int*)_tmp48C)!= 18)goto _LL2B6;
_tmp4B6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp48C)->f1;_LL2B5: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4B6);goto _LL28B;_LL2B6: if(*((int*)_tmp48C)!= 20)goto _LL2B8;
_tmp4B7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp48C)->f1;_tmp4B8=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp48C)->f2;_LL2B7: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4B7,_tmp4B8);goto _LL28B;_LL2B8: if(*((int*)_tmp48C)!= 21)goto _LL2BA;
_LL2B9:{const char*_tmp845;void*_tmp844;(_tmp844=0,Cyc_Tcutil_terr(loc,((_tmp845="gen() not in top-level initializer",
_tag_dyneither(_tmp845,sizeof(char),35))),_tag_dyneither(_tmp844,sizeof(void*),0)));}
return;_LL2BA: if(*((int*)_tmp48C)!= 22)goto _LL2BC;_tmp4B9=((struct Cyc_Absyn_Deref_e_struct*)
_tmp48C)->f1;_LL2BB: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4B9);goto _LL28B;_LL2BC:
if(*((int*)_tmp48C)!= 23)goto _LL2BE;_tmp4BA=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp48C)->f1;_tmp4BB=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp48C)->f2;_LL2BD:
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4BA,_tmp4BB);goto _LL28B;_LL2BE: if(*((
int*)_tmp48C)!= 24)goto _LL2C0;_tmp4BC=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp48C)->f1;_tmp4BD=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp48C)->f2;_LL2BF: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4BC,_tmp4BD);goto _LL28B;_LL2C0: if(*((int*)
_tmp48C)!= 25)goto _LL2C2;_tmp4BE=((struct Cyc_Absyn_Subscript_e_struct*)_tmp48C)->f1;
_tmp4BF=((struct Cyc_Absyn_Subscript_e_struct*)_tmp48C)->f2;_LL2C1: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4BE,_tmp4BF);goto _LL28B;_LL2C2: if(*((int*)_tmp48C)!= 26)goto
_LL2C4;_tmp4C0=((struct Cyc_Absyn_Tuple_e_struct*)_tmp48C)->f1;_LL2C3: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4C0);goto _LL28B;_LL2C4: if(*((int*)_tmp48C)!= 27)goto _LL2C6;
_tmp4C1=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp48C)->f1;_tmp4C2=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp48C)->f2;_LL2C5: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4C1,_tmp4C2);goto
_LL28B;_LL2C6: if(*((int*)_tmp48C)!= 28)goto _LL2C8;_tmp4C3=((struct Cyc_Absyn_Array_e_struct*)
_tmp48C)->f1;_LL2C7: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4EE=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4EF;void*_tmp4F0;void**
_tmp4F1;struct Cyc_Absyn_Conref*_tmp4F2;_LL2DD: if(_tmp4EE <= (void*)4)goto _LL2DF;
if(*((int*)_tmp4EE)!= 7)goto _LL2DF;_tmp4EF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4EE)->f1;_tmp4F0=(void*)_tmp4EF.elt_type;_tmp4F1=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4EE)->f1).elt_type;_tmp4F2=_tmp4EF.zero_term;_LL2DE: elt_topt=(void**)_tmp4F1;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4F2);
goto _LL2DC;_LL2DF:;_LL2E0: goto _LL2DC;_LL2DC:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4C3);goto _LL28B;}_LL2C8: if(*((int*)_tmp48C)!= 38)goto
_LL2CA;_tmp4C4=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp48C)->f1;_LL2C9: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4C4);goto _LL28B;_LL2CA: if(*((int*)_tmp48C)!= 29)goto _LL2CC;
_tmp4C5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f1;_tmp4C6=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f2;_tmp4C7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp48C)->f3;_tmp4C8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f4;
_tmp4C9=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp48C)->f4;_LL2CB: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4C5,_tmp4C6,_tmp4C7,_tmp4C9);goto
_LL28B;_LL2CC: if(*((int*)_tmp48C)!= 30)goto _LL2CE;_tmp4CA=((struct Cyc_Absyn_Struct_e_struct*)
_tmp48C)->f1;_tmp4CB=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp48C)->f1;_tmp4CC=((struct Cyc_Absyn_Struct_e_struct*)_tmp48C)->f2;_tmp4CD=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp48C)->f2;_tmp4CE=((
struct Cyc_Absyn_Struct_e_struct*)_tmp48C)->f3;_tmp4CF=((struct Cyc_Absyn_Struct_e_struct*)
_tmp48C)->f4;_tmp4D0=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp48C)->f4;_LL2CD: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4CB,_tmp4CD,_tmp4CE,
_tmp4D0);goto _LL28B;_LL2CE: if(*((int*)_tmp48C)!= 31)goto _LL2D0;_tmp4D1=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp48C)->f1;_tmp4D2=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp48C)->f2;_LL2CF: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4D1,_tmp4D2);goto _LL28B;
_LL2D0: if(*((int*)_tmp48C)!= 32)goto _LL2D2;_tmp4D3=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp48C)->f1;_tmp4D4=((struct Cyc_Absyn_Tunion_e_struct*)_tmp48C)->f2;_tmp4D5=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp48C)->f3;_LL2D1: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4D3,_tmp4D4,_tmp4D5);goto _LL28B;_LL2D2: if(*((int*)_tmp48C)!= 33)goto
_LL2D4;_tmp4D6=((struct Cyc_Absyn_Enum_e_struct*)_tmp48C)->f1;_tmp4D7=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp48C)->f1;_tmp4D8=((struct Cyc_Absyn_Enum_e_struct*)
_tmp48C)->f2;_tmp4D9=((struct Cyc_Absyn_Enum_e_struct*)_tmp48C)->f3;_LL2D3:*
_tmp4D7=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4D9))->name;{struct Cyc_Absyn_EnumType_struct
_tmp848;struct Cyc_Absyn_EnumType_struct*_tmp847;t=(void*)((_tmp847=_cycalloc(
sizeof(*_tmp847)),((_tmp847[0]=((_tmp848.tag=12,((_tmp848.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4D8))->name,((_tmp848.f2=_tmp4D8,_tmp848)))))),_tmp847))));}goto
_LL28B;_LL2D4: if(*((int*)_tmp48C)!= 34)goto _LL2D6;_tmp4DA=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp48C)->f1;_tmp4DB=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp48C)->f1;_tmp4DC=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp48C)->f2;
_tmp4DD=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp48C)->f3;_LL2D5:*_tmp4DB=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4DD))->name;t=_tmp4DC;goto _LL28B;
_LL2D6: if(*((int*)_tmp48C)!= 35)goto _LL2D8;_tmp4DE=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48C)->f1;_tmp4DF=_tmp4DE.is_calloc;_tmp4E0=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48C)->f1).is_calloc;_tmp4E1=_tmp4DE.rgn;_tmp4E2=_tmp4DE.elt_type;_tmp4E3=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp48C)->f1).elt_type;_tmp4E4=
_tmp4DE.num_elts;_tmp4E5=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48C)->f1).num_elts;_tmp4E6=_tmp4DE.fat_result;_tmp4E7=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp48C)->f1).fat_result;_LL2D7: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4E1,_tmp4E3,
_tmp4E5,_tmp4E0,_tmp4E7);goto _LL28B;_LL2D8: if(*((int*)_tmp48C)!= 36)goto _LL2DA;
_tmp4E8=((struct Cyc_Absyn_Swap_e_struct*)_tmp48C)->f1;_tmp4E9=((struct Cyc_Absyn_Swap_e_struct*)
_tmp48C)->f2;_LL2D9: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4E8,_tmp4E9);goto _LL28B;
_LL2DA: if(*((int*)_tmp48C)!= 39)goto _LL28B;_tmp4EA=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp48C)->f1;_LL2DB: if(!te->allow_valueof){const char*_tmp84B;void*_tmp84A;(
_tmp84A=0,Cyc_Tcutil_terr(e->loc,((_tmp84B="valueof(-) can only occur within types",
_tag_dyneither(_tmp84B,sizeof(char),39))),_tag_dyneither(_tmp84A,sizeof(void*),0)));}
t=Cyc_Absyn_sint_typ;goto _LL28B;_LL28B:;}{struct Cyc_Core_Opt*_tmp84C;e->topt=((
_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C->v=(void*)t,_tmp84C))));}}
