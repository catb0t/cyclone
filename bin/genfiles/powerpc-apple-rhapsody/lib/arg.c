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
  if (!_cus_arr.base) _throw_null();
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
  if (!_cus_arr.base) _throw_null(); \
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
tag;struct _dyneither_ptr f1;};int Cyc_sscanf(struct _dyneither_ptr,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char
Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(
struct _dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strncmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);struct _dyneither_ptr
Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_Arg_Bad[8];
struct Cyc_Arg_Bad_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[
10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*
anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr
args);struct Cyc_Buffer_t;struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);void Cyc_Buffer_add_substring(
struct Cyc_Buffer_t*,struct _dyneither_ptr,int offset,int len);void Cyc_Buffer_add_string(
struct Cyc_Buffer_t*,struct _dyneither_ptr);int isspace(int);void*Cyc___assert_fail(
struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);char Cyc_Arg_Bad[
8]="\000\000\000\000Bad\000";char Cyc_Arg_Error[10]="\000\000\000\000Error\000";
struct Cyc_Arg_Prefix_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Exact_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Unknown_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Arg_Missing_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Arg_Message_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Arg_Wrong_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};
struct _tuple0{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct
_dyneither_ptr f5;};static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct
_dyneither_ptr x);static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct
_dyneither_ptr x){while(l != 0){struct _dyneither_ptr _tmp0=(*((struct _tuple0*)l->hd)).f1;
unsigned long _tmp1=Cyc_strlen((struct _dyneither_ptr)_tmp0);if(_tmp1 > 0  && (*((
struct _tuple0*)l->hd)).f2){if(Cyc_strncmp((struct _dyneither_ptr)x,(struct
_dyneither_ptr)(*((struct _tuple0*)l->hd)).f1,_tmp1)== 0)return(*((struct _tuple0*)
l->hd)).f4;}else{if(Cyc_strcmp((struct _dyneither_ptr)x,(struct _dyneither_ptr)(*((
struct _tuple0*)l->hd)).f1)== 0)return(*((struct _tuple0*)l->hd)).f4;}l=l->tl;}(
int)_throw((void*)Cyc_Core_Not_found);}static struct _dyneither_ptr Cyc_Arg_Justify_break_line(
struct Cyc_Buffer_t*b,int howmuch,struct _dyneither_ptr s);static struct
_dyneither_ptr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int howmuch,struct
_dyneither_ptr s){if(s.curr == ((struct _dyneither_ptr)_tag_dyneither(0,0,0)).curr)
return(struct _dyneither_ptr)_tag_dyneither(0,0,0);if(howmuch < 0)howmuch=0;{
unsigned long _tmp2=Cyc_strlen((struct _dyneither_ptr)s);{const char*_tmp62;const
char*_tmp61;_tmp2 <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct
_dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(((
_tmp61="len <= numelts(s)",_tag_dyneither(_tmp61,sizeof(char),18))),((_tmp62="arg.cyc",
_tag_dyneither(_tmp62,sizeof(char),8))),94);}if(howmuch > _tmp2){Cyc_Buffer_add_string(
b,s);return(struct _dyneither_ptr)_tag_dyneither(0,0,0);}{int i;for(i=howmuch - 1;i
>= 0  && !isspace((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));
-- i){;}if(i < 0)for(i=howmuch?howmuch - 1: 0;(i < _tmp2  && (int)((const char*)s.curr)[
i]) && !isspace((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));
++ i){;}Cyc_Buffer_add_substring(b,s,0,i);{struct _dyneither_ptr whatsleft=(struct
_dyneither_ptr)_tag_dyneither(0,0,0);for(0;(i < _tmp2  && (int)((const char*)s.curr)[
i]) && isspace((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));
++ i){;}if(i < _tmp2  && (int)((const char*)s.curr)[i])whatsleft=_dyneither_ptr_plus(
s,sizeof(char),i);return whatsleft;}}}}void Cyc_Arg_Justify_justify_b(struct Cyc_Buffer_t*
b,int indent,int margin,struct _dyneither_ptr item,struct _dyneither_ptr desc);static
void _tmp68(unsigned int*_tmp67,unsigned int*_tmp66,char**_tmp64){for(*_tmp67=0;*
_tmp67 < *_tmp66;(*_tmp67)++){(*_tmp64)[*_tmp67]='\000';}}static void _tmp71(
unsigned int*_tmp70,unsigned int*_tmp6F,char**_tmp6D){for(*_tmp70=0;*_tmp70 < *
_tmp6F;(*_tmp70)++){(*_tmp6D)[*_tmp70]='\000';}}void Cyc_Arg_Justify_justify_b(
struct Cyc_Buffer_t*b,int indent,int margin,struct _dyneither_ptr item,struct
_dyneither_ptr desc){if(item.curr != ((struct _dyneither_ptr)_tag_dyneither(0,0,0)).curr)
Cyc_Buffer_add_string(b,item);if(desc.curr == ((struct _dyneither_ptr)
_tag_dyneither(0,0,0)).curr)return;if(indent < 0)indent=0;if(margin < 0)margin=0;{
unsigned int _tmp67;unsigned int _tmp66;struct _dyneither_ptr _tmp65;char*_tmp64;
unsigned int _tmp63;struct _dyneither_ptr indentstr=(_tmp63=(unsigned int)(indent + 
2),((_tmp64=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp63 + 1)),((
_tmp65=_tag_dyneither(_tmp64,sizeof(char),_tmp63 + 1),((((_tmp66=_tmp63,((_tmp68(&
_tmp67,& _tmp66,& _tmp64),_tmp64[_tmp66]=(char)0)))),_tmp65)))))));{unsigned int i=
0;for(0;i < indent + 1;++ i){char _tmp6B;char _tmp6A;struct _dyneither_ptr _tmp69;(
_tmp69=_dyneither_ptr_plus(indentstr,sizeof(char),(int)i),((_tmp6A=*((char*)
_check_dyneither_subscript(_tmp69,sizeof(char),0)),((_tmp6B=i == 0?'\n':' ',((
_get_dyneither_size(_tmp69,sizeof(char))== 1  && (_tmp6A == '\000'  && _tmp6B != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp69.curr)=_tmp6B)))))));}}{unsigned long _tmp8=
Cyc_strlen((struct _dyneither_ptr)item);struct _dyneither_ptr itemsep;if(Cyc_strlen((
struct _dyneither_ptr)desc)> 0){if(_tmp8 + 1 > indent)itemsep=indentstr;else{
unsigned int _tmp70;unsigned int _tmp6F;struct _dyneither_ptr _tmp6E;char*_tmp6D;
unsigned int _tmp6C;struct _dyneither_ptr temp=(_tmp6C=(indent - _tmp8)+ 1,((_tmp6D=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp6C + 1)),((_tmp6E=
_tag_dyneither(_tmp6D,sizeof(char),_tmp6C + 1),((((_tmp6F=_tmp6C,((_tmp71(& _tmp70,&
_tmp6F,& _tmp6D),_tmp6D[_tmp6F]=(char)0)))),_tmp6E)))))));{unsigned int i=0;for(0;
i < indent - _tmp8;++ i){char _tmp74;char _tmp73;struct _dyneither_ptr _tmp72;(_tmp72=
_dyneither_ptr_plus(temp,sizeof(char),(int)i),((_tmp73=*((char*)
_check_dyneither_subscript(_tmp72,sizeof(char),0)),((_tmp74=' ',((
_get_dyneither_size(_tmp72,sizeof(char))== 1  && (_tmp73 == '\000'  && _tmp74 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp72.curr)=_tmp74)))))));}}itemsep=temp;}}
else{return;}Cyc_Buffer_add_string(b,(struct _dyneither_ptr)itemsep);while(desc.curr
!= ((struct _dyneither_ptr)_tag_dyneither(0,0,0)).curr){desc=Cyc_Arg_Justify_break_line(
b,margin - indent,desc);if(desc.curr != ((struct _dyneither_ptr)_tag_dyneither(0,0,
0)).curr)Cyc_Buffer_add_string(b,(struct _dyneither_ptr)indentstr);else{const char*
_tmp75;Cyc_Buffer_add_string(b,((_tmp75="\n",_tag_dyneither(_tmp75,sizeof(char),
2))));}}return;}}}void Cyc_Arg_usage(struct Cyc_List_List*speclist,struct
_dyneither_ptr errmsg);void Cyc_Arg_usage(struct Cyc_List_List*speclist,struct
_dyneither_ptr errmsg){{const char*_tmp79;void*_tmp78[1];struct Cyc_String_pa_struct
_tmp77;(_tmp77.tag=0,((_tmp77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
errmsg),((_tmp78[0]=& _tmp77,Cyc_fprintf(Cyc_stderr,((_tmp79="%s\n",
_tag_dyneither(_tmp79,sizeof(char),4))),_tag_dyneither(_tmp78,sizeof(void*),1)))))));}{
struct Cyc_Buffer_t*_tmp18=Cyc_Buffer_create(1024);while(speclist != 0){Cyc_Arg_Justify_justify_b(
_tmp18,12,72,(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)(*((
struct _tuple0*)speclist->hd)).f1,(struct _dyneither_ptr)(*((struct _tuple0*)
speclist->hd)).f3),(*((struct _tuple0*)speclist->hd)).f5);speclist=speclist->tl;}{
const char*_tmp7D;void*_tmp7C[1];struct Cyc_String_pa_struct _tmp7B;(_tmp7B.tag=0,((
_tmp7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Buffer_contents(
_tmp18)),((_tmp7C[0]=& _tmp7B,Cyc_fprintf(Cyc_stderr,((_tmp7D="%s",_tag_dyneither(
_tmp7D,sizeof(char),3))),_tag_dyneither(_tmp7C,sizeof(void*),1)))))));}}}int Cyc_Arg_current=
0;static struct _dyneither_ptr Cyc_Arg_args={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*speclist,struct
_dyneither_ptr errmsg);static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*
speclist,struct _dyneither_ptr errmsg){const char*_tmp7E;struct _dyneither_ptr
progname=prog_pos < _get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr))?*((
struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct
_dyneither_ptr),prog_pos)):((_tmp7E="(?)",_tag_dyneither(_tmp7E,sizeof(char),4)));{
void*_tmp1C=e;struct _dyneither_ptr _tmp1D;struct _dyneither_ptr _tmp1E;struct
_dyneither_ptr _tmp1F;struct _dyneither_ptr _tmp20;struct _dyneither_ptr _tmp21;
struct _dyneither_ptr _tmp22;_LL1: if(*((int*)_tmp1C)!= 0)goto _LL3;_tmp1D=((struct
Cyc_Arg_Unknown_struct*)_tmp1C)->f1;_LL2:{const char*_tmp7F;if(Cyc_strcmp((struct
_dyneither_ptr)_tmp1D,((_tmp7F="-help",_tag_dyneither(_tmp7F,sizeof(char),6))))
!= 0){const char*_tmp84;void*_tmp83[2];struct Cyc_String_pa_struct _tmp82;struct Cyc_String_pa_struct
_tmp81;(_tmp81.tag=0,((_tmp81.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp1D),((_tmp82.tag=0,((_tmp82.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
progname),((_tmp83[0]=& _tmp82,((_tmp83[1]=& _tmp81,Cyc_fprintf(Cyc_stderr,((
_tmp84="%s: unknown option `%s'.\n",_tag_dyneither(_tmp84,sizeof(char),26))),
_tag_dyneither(_tmp83,sizeof(void*),2)))))))))))));}}goto _LL0;_LL3: if(*((int*)
_tmp1C)!= 1)goto _LL5;_tmp1E=((struct Cyc_Arg_Missing_struct*)_tmp1C)->f1;_LL4:{
const char*_tmp89;void*_tmp88[2];struct Cyc_String_pa_struct _tmp87;struct Cyc_String_pa_struct
_tmp86;(_tmp86.tag=0,((_tmp86.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp1E),((_tmp87.tag=0,((_tmp87.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
progname),((_tmp88[0]=& _tmp87,((_tmp88[1]=& _tmp86,Cyc_fprintf(Cyc_stderr,((
_tmp89="%s: option `%s' needs an argument.\n",_tag_dyneither(_tmp89,sizeof(char),
36))),_tag_dyneither(_tmp88,sizeof(void*),2)))))))))))));}goto _LL0;_LL5: if(*((
int*)_tmp1C)!= 3)goto _LL7;_tmp1F=((struct Cyc_Arg_Wrong_struct*)_tmp1C)->f1;
_tmp20=((struct Cyc_Arg_Wrong_struct*)_tmp1C)->f2;_tmp21=((struct Cyc_Arg_Wrong_struct*)
_tmp1C)->f3;_LL6:{const char*_tmp90;void*_tmp8F[4];struct Cyc_String_pa_struct
_tmp8E;struct Cyc_String_pa_struct _tmp8D;struct Cyc_String_pa_struct _tmp8C;struct
Cyc_String_pa_struct _tmp8B;(_tmp8B.tag=0,((_tmp8B.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp21),((_tmp8C.tag=0,((_tmp8C.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp1F),((_tmp8D.tag=0,((_tmp8D.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp20),((_tmp8E.tag=0,((_tmp8E.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)progname),((_tmp8F[0]=& _tmp8E,((_tmp8F[1]=& _tmp8D,((_tmp8F[
2]=& _tmp8C,((_tmp8F[3]=& _tmp8B,Cyc_fprintf(Cyc_stderr,((_tmp90="%s: wrong argument `%s'; option `%s' expects %s.\n",
_tag_dyneither(_tmp90,sizeof(char),50))),_tag_dyneither(_tmp8F,sizeof(void*),4)))))))))))))))))))))))));}
goto _LL0;_LL7: if(*((int*)_tmp1C)!= 2)goto _LL0;_tmp22=((struct Cyc_Arg_Message_struct*)
_tmp1C)->f1;_LL8:{const char*_tmp95;void*_tmp94[2];struct Cyc_String_pa_struct
_tmp93;struct Cyc_String_pa_struct _tmp92;(_tmp92.tag=0,((_tmp92.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp22),((_tmp93.tag=0,((_tmp93.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)progname),((_tmp94[0]=& _tmp93,((_tmp94[1]=&
_tmp92,Cyc_fprintf(Cyc_stderr,((_tmp95="%s: %s.\n",_tag_dyneither(_tmp95,sizeof(
char),9))),_tag_dyneither(_tmp94,sizeof(void*),2)))))))))))));}goto _LL0;_LL0:;}
Cyc_Arg_usage(speclist,errmsg);Cyc_Arg_current=(int)_get_dyneither_size(Cyc_Arg_args,
sizeof(struct _dyneither_ptr));}void Cyc_Arg_parse(struct Cyc_List_List*speclist,
void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct
_dyneither_ptr orig_args);void Cyc_Arg_parse(struct Cyc_List_List*speclist,void(*
anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr
orig_args){Cyc_Arg_args=orig_args;{int initpos=Cyc_Arg_current;unsigned int l=
_get_dyneither_size(Cyc_Arg_args,sizeof(struct _dyneither_ptr));if((*((struct
_dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct
_dyneither_ptr),(int)(l - 1)))).curr == ((struct _dyneither_ptr)_tag_dyneither(0,0,
0)).curr)l=l - 1;++ Cyc_Arg_current;while(Cyc_Arg_current < l){struct _dyneither_ptr
s=*((struct _dyneither_ptr*)_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct
_dyneither_ptr),Cyc_Arg_current));if((s.curr != ((struct _dyneither_ptr)
_tag_dyneither(0,0,0)).curr  && _get_dyneither_size(s,sizeof(char))>= 1) && *((
const char*)_check_dyneither_subscript(s,sizeof(char),0))== '-'){void*action;{
struct _handler_cons _tmp37;_push_handler(& _tmp37);{int _tmp39=0;if(setjmp(_tmp37.handler))
_tmp39=1;if(!_tmp39){action=Cyc_Arg_lookup(speclist,s);;_pop_handler();}else{
void*_tmp38=(void*)_exn_thrown;void*_tmp3B=_tmp38;_LLA: if(_tmp3B != Cyc_Core_Not_found)
goto _LLC;_LLB:{struct Cyc_Arg_Unknown_struct _tmp98;struct Cyc_Arg_Unknown_struct*
_tmp97;Cyc_Arg_stop(initpos,(void*)((_tmp97=_cycalloc(sizeof(*_tmp97)),((_tmp97[
0]=((_tmp98.tag=0,((_tmp98.f1=s,_tmp98)))),_tmp97)))),speclist,errmsg);}return;
_LLC:;_LLD:(void)_throw(_tmp3B);_LL9:;}}}{struct _handler_cons _tmp3E;
_push_handler(& _tmp3E);{int _tmp40=0;if(setjmp(_tmp3E.handler))_tmp40=1;if(!
_tmp40){{void*_tmp41=action;void(*_tmp42)();void(*_tmp43)(struct _dyneither_ptr);
int*_tmp44;int*_tmp45;void(*_tmp46)(struct _dyneither_ptr,struct _dyneither_ptr);
void(*_tmp47)(struct _dyneither_ptr);void(*_tmp48)(int);void(*_tmp49)(struct
_dyneither_ptr);_LLF: if(*((int*)_tmp41)!= 0)goto _LL11;_tmp42=((struct Cyc_Arg_Unit_spec_struct*)
_tmp41)->f1;_LL10: _tmp42();goto _LLE;_LL11: if(*((int*)_tmp41)!= 1)goto _LL13;
_tmp43=((struct Cyc_Arg_Flag_spec_struct*)_tmp41)->f1;_LL12: _tmp43(s);goto _LLE;
_LL13: if(*((int*)_tmp41)!= 3)goto _LL15;_tmp44=((struct Cyc_Arg_Set_spec_struct*)
_tmp41)->f1;_LL14:*_tmp44=1;goto _LLE;_LL15: if(*((int*)_tmp41)!= 4)goto _LL17;
_tmp45=((struct Cyc_Arg_Clear_spec_struct*)_tmp41)->f1;_LL16:*_tmp45=0;goto _LLE;
_LL17: if(*((int*)_tmp41)!= 2)goto _LL19;_tmp46=((struct Cyc_Arg_FlagString_spec_struct*)
_tmp41)->f1;_LL18: if(Cyc_Arg_current + 1 < l){_tmp46(s,*((struct _dyneither_ptr*)
_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current
+ 1)));++ Cyc_Arg_current;}else{struct Cyc_Arg_Missing_struct _tmp9B;struct Cyc_Arg_Missing_struct*
_tmp9A;Cyc_Arg_stop(initpos,(void*)((_tmp9A=_cycalloc(sizeof(*_tmp9A)),((_tmp9A[
0]=((_tmp9B.tag=1,((_tmp9B.f1=s,_tmp9B)))),_tmp9A)))),speclist,errmsg);}goto _LLE;
_LL19: if(*((int*)_tmp41)!= 5)goto _LL1B;_tmp47=((struct Cyc_Arg_String_spec_struct*)
_tmp41)->f1;_LL1A: if(Cyc_Arg_current + 1 < l){_tmp47(*((struct _dyneither_ptr*)
_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current
+ 1)));++ Cyc_Arg_current;}else{struct Cyc_Arg_Missing_struct _tmp9E;struct Cyc_Arg_Missing_struct*
_tmp9D;Cyc_Arg_stop(initpos,(void*)((_tmp9D=_cycalloc(sizeof(*_tmp9D)),((_tmp9D[
0]=((_tmp9E.tag=1,((_tmp9E.f1=s,_tmp9E)))),_tmp9D)))),speclist,errmsg);}goto _LLE;
_LL1B: if(*((int*)_tmp41)!= 6)goto _LL1D;_tmp48=((struct Cyc_Arg_Int_spec_struct*)
_tmp41)->f1;_LL1C: {struct _dyneither_ptr arg=*((struct _dyneither_ptr*)
_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current
+ 1));int n=0;{const char*_tmpA2;void*_tmpA1[1];struct Cyc_IntPtr_sa_struct _tmpA0;
if(((_tmpA0.tag=2,((_tmpA0.f1=& n,((_tmpA1[0]=& _tmpA0,Cyc_sscanf(arg,((_tmpA2="%d",
_tag_dyneither(_tmpA2,sizeof(char),3))),_tag_dyneither(_tmpA1,sizeof(void*),1))))))))
!= 1){{struct Cyc_Arg_Wrong_struct _tmpA8;const char*_tmpA7;struct Cyc_Arg_Wrong_struct*
_tmpA6;Cyc_Arg_stop(initpos,(void*)((_tmpA6=_cycalloc(sizeof(*_tmpA6)),((_tmpA6[
0]=((_tmpA8.tag=3,((_tmpA8.f1=s,((_tmpA8.f2=arg,((_tmpA8.f3=((_tmpA7="an integer",
_tag_dyneither(_tmpA7,sizeof(char),11))),_tmpA8)))))))),_tmpA6)))),speclist,
errmsg);}_npop_handler(0);return;}}_tmp48(n);++ Cyc_Arg_current;goto _LLE;}_LL1D:
if(*((int*)_tmp41)!= 7)goto _LLE;_tmp49=((struct Cyc_Arg_Rest_spec_struct*)_tmp41)->f1;
_LL1E: while(Cyc_Arg_current < l - 1){_tmp49(*((struct _dyneither_ptr*)
_check_dyneither_subscript(Cyc_Arg_args,sizeof(struct _dyneither_ptr),Cyc_Arg_current
+ 1)));++ Cyc_Arg_current;}goto _LLE;_LLE:;};_pop_handler();}else{void*_tmp3F=(
void*)_exn_thrown;void*_tmp55=_tmp3F;struct _dyneither_ptr _tmp56;_LL20: if(*((void**)
_tmp55)!= Cyc_Arg_Bad)goto _LL22;_tmp56=((struct Cyc_Arg_Bad_struct*)_tmp55)->f1;
_LL21:{struct Cyc_Arg_Message_struct _tmpAB;struct Cyc_Arg_Message_struct*_tmpAA;
Cyc_Arg_stop(initpos,(void*)((_tmpAA=_cycalloc(sizeof(*_tmpAA)),((_tmpAA[0]=((
_tmpAB.tag=2,((_tmpAB.f1=_tmp56,_tmpAB)))),_tmpAA)))),speclist,errmsg);}goto
_LL1F;_LL22:;_LL23:(void)_throw(_tmp55);_LL1F:;}}}++ Cyc_Arg_current;}else{{
struct _handler_cons _tmp59;_push_handler(& _tmp59);{int _tmp5B=0;if(setjmp(_tmp59.handler))
_tmp5B=1;if(!_tmp5B){anonfun(s);;_pop_handler();}else{void*_tmp5A=(void*)
_exn_thrown;void*_tmp5D=_tmp5A;struct _dyneither_ptr _tmp5E;_LL25: if(*((void**)
_tmp5D)!= Cyc_Arg_Bad)goto _LL27;_tmp5E=((struct Cyc_Arg_Bad_struct*)_tmp5D)->f1;
_LL26:{struct Cyc_Arg_Message_struct _tmpAE;struct Cyc_Arg_Message_struct*_tmpAD;
Cyc_Arg_stop(initpos,(void*)((_tmpAD=_cycalloc(sizeof(*_tmpAD)),((_tmpAD[0]=((
_tmpAE.tag=2,((_tmpAE.f1=_tmp5E,_tmpAE)))),_tmpAD)))),speclist,errmsg);}goto
_LL24;_LL27:;_LL28:(void)_throw(_tmp5D);_LL24:;}}}++ Cyc_Arg_current;}}}}