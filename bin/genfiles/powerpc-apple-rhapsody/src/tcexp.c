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
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
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
Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);void*Cyc_Absyn_pointer_expand(
void*,int fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};struct _dyneither_ptr Cyc_vrprintf(struct
_RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dyneither_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(
void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*
r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,
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
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;
struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Tcenv_Tenv*
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
Cyc_Absyn_Exp*e);int Cyc_Evexp_okay_szofarg(void*t);void Cyc_Tcstmt_tcStmt(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct Cyc_List_List*Cyc_Formatstr_get_format_typs(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,struct Cyc_Position_Segment*);
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct _dyneither_ptr,struct Cyc_Position_Segment*);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,
void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);extern int Cyc_Tcexp_in_stmt_exp;
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessNumelts_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
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
_dyneither_ptr msg,struct _dyneither_ptr ap){{void*_tmp4ED;(_tmp4ED=0,Cyc_Tcutil_terr(
loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),
_tag_dyneither(_tmp4ED,sizeof(void*),0)));}if(topt == 0)return Cyc_Absyn_wildtyp(
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
_tmp4F0;struct Cyc_Absyn_Var_e_struct*_tmp4EF;(void*)(e->r=(void*)((void*)((
_tmp4EF=_cycalloc(sizeof(*_tmp4EF)),((_tmp4EF[0]=((_tmp4F0.tag=1,((_tmp4F0.f1=q,((
_tmp4F0.f2=(void*)_tmp6,_tmp4F0)))))),_tmp4EF))))));}goto _LL0;_LL3: if(*((int*)
_tmp5)!= 3)goto _LL5;_tmp7=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f1;_tmp8=((
struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f2;_LL4:{struct Cyc_Absyn_Enum_e_struct
_tmp4F3;struct Cyc_Absyn_Enum_e_struct*_tmp4F2;(void*)(e->r=(void*)((void*)((
_tmp4F2=_cycalloc(sizeof(*_tmp4F2)),((_tmp4F2[0]=((_tmp4F3.tag=33,((_tmp4F3.f1=q,((
_tmp4F3.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7,((_tmp4F3.f3=(struct Cyc_Absyn_Enumfield*)
_tmp8,_tmp4F3)))))))),_tmp4F2))))));}goto _LL0;_LL5: if(*((int*)_tmp5)!= 4)goto
_LL7;_tmp9=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;_tmpA=((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;_LL6:{struct Cyc_Absyn_AnonEnum_e_struct
_tmp4F6;struct Cyc_Absyn_AnonEnum_e_struct*_tmp4F5;(void*)(e->r=(void*)((void*)((
_tmp4F5=_cycalloc(sizeof(*_tmp4F5)),((_tmp4F5[0]=((_tmp4F6.tag=34,((_tmp4F6.f1=q,((
_tmp4F6.f2=(void*)_tmp9,((_tmp4F6.f3=(struct Cyc_Absyn_Enumfield*)_tmpA,_tmp4F6)))))))),
_tmp4F5))))));}goto _LL0;_LL7: if(*((int*)_tmp5)!= 2)goto _LL9;_tmpB=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f1;_tmpC=((struct Cyc_Tcenv_TunionRes_struct*)_tmp5)->f2;_LL8:{struct Cyc_Absyn_Tunion_e_struct
_tmp4F9;struct Cyc_Absyn_Tunion_e_struct*_tmp4F8;(void*)(e->r=(void*)((void*)((
_tmp4F8=_cycalloc(sizeof(*_tmp4F8)),((_tmp4F8[0]=((_tmp4F9.tag=32,((_tmp4F9.f1=0,((
_tmp4F9.f2=_tmpB,((_tmp4F9.f3=_tmpC,_tmp4F9)))))))),_tmp4F8))))));}goto _LL0;_LL9:
if(*((int*)_tmp5)!= 1)goto _LL0;_LLA:{const char*_tmp4FD;void*_tmp4FC[1];struct Cyc_String_pa_struct
_tmp4FB;(_tmp4FB.tag=0,((_tmp4FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(q)),((_tmp4FC[0]=& _tmp4FB,Cyc_Tcutil_terr(e->loc,((
_tmp4FD="bad occurrence of type name %s",_tag_dyneither(_tmp4FD,sizeof(char),31))),
_tag_dyneither(_tmp4FC,sizeof(void*),1)))))));}{struct Cyc_Absyn_Var_e_struct
_tmp500;struct Cyc_Absyn_Var_e_struct*_tmp4FF;(void*)(e->r=(void*)((void*)((
_tmp4FF=_cycalloc(sizeof(*_tmp4FF)),((_tmp4FF[0]=((_tmp500.tag=1,((_tmp500.f1=q,((
_tmp500.f2=(void*)((void*)0),_tmp500)))))),_tmp4FF))))));}goto _LL0;_LL0:;};
_pop_region(r);};_pop_handler();}else{void*_tmp2=(void*)_exn_thrown;void*_tmp1B=
_tmp2;_LLC: if(_tmp1B != Cyc_Dict_Absent)goto _LLE;_LLD:{struct Cyc_Absyn_Var_e_struct
_tmp503;struct Cyc_Absyn_Var_e_struct*_tmp502;(void*)(e->r=(void*)((void*)((
_tmp502=_cycalloc(sizeof(*_tmp502)),((_tmp502[0]=((_tmp503.tag=1,((_tmp503.f1=q,((
_tmp503.f2=(void*)((void*)0),_tmp503)))))),_tmp502))))));}goto _LLB;_LLE:;_LLF:(
void)_throw(_tmp1B);_LLB:;}}}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e);static
struct _tuple8*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){struct _tuple8*
_tmp504;return(_tmp504=_cycalloc(sizeof(*_tmp504)),((_tmp504->f1=0,((_tmp504->f2=
e,_tmp504)))));}static void Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,
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
_tmp507;struct Cyc_Absyn_FnCall_e_struct*_tmp506;(void*)(e->r=(void*)((void*)((
_tmp506=_cycalloc(sizeof(*_tmp506)),((_tmp506[0]=((_tmp507.tag=11,((_tmp507.f1=
e1,((_tmp507.f2=es,((_tmp507.f3=0,_tmp507)))))))),_tmp506))))));}_npop_handler(1);
return;_LL18: if(*((int*)_tmp25)!= 2)goto _LL1A;_tmp27=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp25)->f1;_tmp28=((struct Cyc_Tcenv_TunionRes_struct*)_tmp25)->f2;_LL19: if(
_tmp28->typs == 0){const char*_tmp50B;void*_tmp50A[1];struct Cyc_String_pa_struct
_tmp509;(_tmp509.tag=0,((_tmp509.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp28->name)),((_tmp50A[0]=& _tmp509,Cyc_Tcutil_terr(e->loc,((
_tmp50B="%s is a constant, not a function",_tag_dyneither(_tmp50B,sizeof(char),
33))),_tag_dyneither(_tmp50A,sizeof(void*),1)))))));}{struct Cyc_Absyn_Tunion_e_struct
_tmp50E;struct Cyc_Absyn_Tunion_e_struct*_tmp50D;(void*)(e->r=(void*)((void*)((
_tmp50D=_cycalloc(sizeof(*_tmp50D)),((_tmp50D[0]=((_tmp50E.tag=32,((_tmp50E.f1=
es,((_tmp50E.f2=_tmp27,((_tmp50E.f3=_tmp28,_tmp50E)))))))),_tmp50D))))));}
_npop_handler(1);return;_LL1A: if(*((int*)_tmp25)!= 1)goto _LL1C;_tmp29=((struct
Cyc_Tcenv_AggrRes_struct*)_tmp25)->f1;_LL1B: {struct Cyc_List_List*_tmp31=((
struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);{struct Cyc_Absyn_Struct_e_struct
_tmp511;struct Cyc_Absyn_Struct_e_struct*_tmp510;(void*)(e->r=(void*)((void*)((
_tmp510=_cycalloc(sizeof(*_tmp510)),((_tmp510[0]=((_tmp511.tag=30,((_tmp511.f1=
_tmp29->name,((_tmp511.f2=0,((_tmp511.f3=_tmp31,((_tmp511.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp29,_tmp511)))))))))),_tmp510))))));}_npop_handler(1);return;}_LL1C: if(*((int*)
_tmp25)!= 4)goto _LL1E;_LL1D: goto _LL1F;_LL1E: if(*((int*)_tmp25)!= 3)goto _LL15;
_LL1F:{const char*_tmp515;void*_tmp514[1];struct Cyc_String_pa_struct _tmp513;(
_tmp513.tag=0,((_tmp513.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp20)),((_tmp514[0]=& _tmp513,Cyc_Tcutil_terr(e->loc,((_tmp515="%s is an enum constructor, not a function",
_tag_dyneither(_tmp515,sizeof(char),42))),_tag_dyneither(_tmp514,sizeof(void*),1)))))));}
_npop_handler(1);return;_LL15:;};_pop_region(r);};_pop_handler();}else{void*
_tmp22=(void*)_exn_thrown;void*_tmp38=_tmp22;_LL21: if(_tmp38 != Cyc_Dict_Absent)
goto _LL23;_LL22:{struct Cyc_Absyn_FnCall_e_struct _tmp518;struct Cyc_Absyn_FnCall_e_struct*
_tmp517;(void*)(e->r=(void*)((void*)((_tmp517=_cycalloc(sizeof(*_tmp517)),((
_tmp517[0]=((_tmp518.tag=11,((_tmp518.f1=e1,((_tmp518.f2=es,((_tmp518.f3=0,
_tmp518)))))))),_tmp517))))));}return;_LL23:;_LL24:(void)_throw(_tmp38);_LL20:;}}}
_LL13:;_LL14:{struct Cyc_Absyn_FnCall_e_struct _tmp51B;struct Cyc_Absyn_FnCall_e_struct*
_tmp51A;(void*)(e->r=(void*)((void*)((_tmp51A=_cycalloc(sizeof(*_tmp51A)),((
_tmp51A[0]=((_tmp51B.tag=11,((_tmp51B.f1=e1,((_tmp51B.f2=es,((_tmp51B.f3=0,
_tmp51B)))))))),_tmp51A))))));}return;_LL10:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des);static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
des){if(topt == 0){{struct Cyc_Absyn_Array_e_struct _tmp51E;struct Cyc_Absyn_Array_e_struct*
_tmp51D;(void*)(e->r=(void*)((void*)((_tmp51D=_cycalloc(sizeof(*_tmp51D)),((
_tmp51D[0]=((_tmp51E.tag=28,((_tmp51E.f1=des,_tmp51E)))),_tmp51D))))));}return;}{
void*t=*topt;void*_tmp3F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp40;
union Cyc_Absyn_AggrInfoU_union _tmp41;struct Cyc_Absyn_ArrayInfo _tmp42;void*_tmp43;
struct Cyc_Absyn_Tqual _tmp44;_LL26: if(_tmp3F <= (void*)4)goto _LL2C;if(*((int*)
_tmp3F)!= 10)goto _LL28;_tmp40=((struct Cyc_Absyn_AggrType_struct*)_tmp3F)->f1;
_tmp41=_tmp40.aggr_info;_LL27:{union Cyc_Absyn_AggrInfoU_union _tmp45=_tmp41;
struct Cyc_Absyn_Aggrdecl**_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp47;_LL2F: if((
_tmp45.UnknownAggr).tag != 0)goto _LL31;_LL30: {const char*_tmp521;void*_tmp520;(
_tmp520=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp521="struct type not properly set",_tag_dyneither(_tmp521,sizeof(char),29))),
_tag_dyneither(_tmp520,sizeof(void*),0)));}_LL31: if((_tmp45.KnownAggr).tag != 1)
goto _LL2E;_tmp46=(_tmp45.KnownAggr).f1;_tmp47=*_tmp46;_LL32: {struct Cyc_Absyn_Struct_e_struct
_tmp524;struct Cyc_Absyn_Struct_e_struct*_tmp523;(void*)(e->r=(void*)((void*)((
_tmp523=_cycalloc(sizeof(*_tmp523)),((_tmp523[0]=((_tmp524.tag=30,((_tmp524.f1=
_tmp47->name,((_tmp524.f2=0,((_tmp524.f3=des,((_tmp524.f4=(struct Cyc_Absyn_Aggrdecl*)
_tmp47,_tmp524)))))))))),_tmp523))))));}_LL2E:;}goto _LL25;_LL28: if(*((int*)
_tmp3F)!= 7)goto _LL2A;_tmp42=((struct Cyc_Absyn_ArrayType_struct*)_tmp3F)->f1;
_tmp43=(void*)_tmp42.elt_type;_tmp44=_tmp42.tq;_LL29:{struct Cyc_Absyn_Array_e_struct
_tmp527;struct Cyc_Absyn_Array_e_struct*_tmp526;(void*)(e->r=(void*)((void*)((
_tmp526=_cycalloc(sizeof(*_tmp526)),((_tmp526[0]=((_tmp527.tag=28,((_tmp527.f1=
des,_tmp527)))),_tmp526))))));}goto _LL25;_LL2A: if(*((int*)_tmp3F)!= 11)goto _LL2C;
_LL2B:{struct Cyc_Absyn_AnonStruct_e_struct _tmp52A;struct Cyc_Absyn_AnonStruct_e_struct*
_tmp529;(void*)(e->r=(void*)((void*)((_tmp529=_cycalloc(sizeof(*_tmp529)),((
_tmp529[0]=((_tmp52A.tag=31,((_tmp52A.f1=(void*)t,((_tmp52A.f2=des,_tmp52A)))))),
_tmp529))))));}goto _LL25;_LL2C:;_LL2D:{struct Cyc_Absyn_Array_e_struct _tmp52D;
struct Cyc_Absyn_Array_e_struct*_tmp52C;(void*)(e->r=(void*)((void*)((_tmp52C=
_cycalloc(sizeof(*_tmp52C)),((_tmp52C[0]=((_tmp52D.tag=28,((_tmp52D.f1=des,
_tmp52D)))),_tmp52C))))));}goto _LL25;_LL25:;}}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es);static void Cyc_Tcexp_tcExpList(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)es->hd);}}void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){Cyc_Tcutil_check_contains_assign(
e);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_sint_typ,e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp532;void*_tmp531[2];struct Cyc_String_pa_struct _tmp530;
struct Cyc_String_pa_struct _tmp52F;(_tmp52F.tag=0,((_tmp52F.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp530.tag=0,((_tmp530.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)msg_part),((_tmp531[0]=& _tmp530,((_tmp531[1]=& _tmp52F,Cyc_Tcutil_terr(
e->loc,((_tmp532="test of %s has type %s instead of integral or * type",
_tag_dyneither(_tmp532,sizeof(char),53))),_tag_dyneither(_tmp531,sizeof(void*),2)))))))))))));}}
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
Cyc_Absyn_char_typ;goto _LL55;_LL55:;}{union Cyc_Absyn_Cnst_union _tmp533;*c=(union
Cyc_Absyn_Cnst_union)(((_tmp533.Char_c).tag=0,(((_tmp533.Char_c).f1=(void*)
_tmp60,(((_tmp533.Char_c).f2=(char)_tmp5D,_tmp533))))));}goto _LL46;_LL49: if(*((
int*)_tmp5F)!= 5)goto _LL4B;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f1;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5F)->f2;if((int)
_tmp63 != 1)goto _LL4B;_LL4A: t=_tmp62 == (void*)1?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;{
union Cyc_Absyn_Cnst_union _tmp534;*c=(union Cyc_Absyn_Cnst_union)(((_tmp534.Short_c).tag=
1,(((_tmp534.Short_c).f1=(void*)_tmp62,(((_tmp534.Short_c).f2=(short)_tmp5D,
_tmp534))))));}goto _LL46;_LL4B: if(*((int*)_tmp5F)!= 5)goto _LL4D;_tmp64=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp65 != 2)goto _LL4D;_LL4C: t=_tmp64 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4D: if(*((int*)_tmp5F)!= 5)goto _LL4F;_tmp66=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5F)->f1;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5F)->f2;if((int)_tmp67 != 3)goto _LL4F;_LL4E: t=_tmp66 == (void*)1?Cyc_Absyn_uint_typ:
Cyc_Absyn_sint_typ;goto _LL46;_LL4F: if(*((int*)_tmp5F)!= 4)goto _LL51;if(!(_tmp5D
== 0))goto _LL51;_LL50:{struct Cyc_Absyn_Const_e_struct _tmp53A;union Cyc_Absyn_Cnst_union
_tmp539;struct Cyc_Absyn_Const_e_struct*_tmp538;(void*)(e->r=(void*)((void*)((
_tmp538=_cycalloc(sizeof(*_tmp538)),((_tmp538[0]=((_tmp53A.tag=0,((_tmp53A.f1=(
union Cyc_Absyn_Cnst_union)(((_tmp539.Null_c).tag=6,_tmp539)),_tmp53A)))),_tmp538))))));}{
struct Cyc_List_List*_tmp6F=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct
_tmp54D;struct Cyc_Core_Opt*_tmp54C;struct Cyc_Absyn_PtrAtts _tmp54B;struct Cyc_Core_Opt*
_tmp54A;struct Cyc_Absyn_PtrInfo _tmp549;struct Cyc_Absyn_PointerType_struct*
_tmp548;t=(void*)((_tmp548=_cycalloc(sizeof(*_tmp548)),((_tmp548[0]=((_tmp54D.tag=
4,((_tmp54D.f1=((_tmp549.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ak,((_tmp54C=_cycalloc(sizeof(*_tmp54C)),((_tmp54C->v=_tmp6F,_tmp54C))))),((
_tmp549.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp549.ptr_atts=((_tmp54B.rgn=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,((_tmp54A=_cycalloc(
sizeof(*_tmp54A)),((_tmp54A->v=_tmp6F,_tmp54A))))),((_tmp54B.nullable=Cyc_Absyn_true_conref,((
_tmp54B.bounds=Cyc_Absyn_empty_conref(),((_tmp54B.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp54B.ptrloc=0,_tmp54B)))))))))),_tmp549)))))),
_tmp54D)))),_tmp548))));}goto _LL46;}_LL51: if(*((int*)_tmp5F)!= 18)goto _LL53;
_tmp68=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp5F)->f1;_LL52: {struct Cyc_Absyn_ValueofType_struct
_tmp550;struct Cyc_Absyn_ValueofType_struct*_tmp54F;struct Cyc_Absyn_ValueofType_struct*
_tmp76=(_tmp54F=_cycalloc(sizeof(*_tmp54F)),((_tmp54F[0]=((_tmp550.tag=17,((
_tmp550.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5D,0),_tmp550)))),_tmp54F)));if(
!Cyc_Tcutil_unify(_tmp68,(void*)_tmp76)){{const char*_tmp555;void*_tmp554[2];
struct Cyc_String_pa_struct _tmp553;struct Cyc_String_pa_struct _tmp552;(_tmp552.tag=
0,((_tmp552.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)_tmp76)),((_tmp553.tag=0,((_tmp553.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp68)),((_tmp554[0]=& _tmp553,((_tmp554[1]=&
_tmp552,Cyc_Tcutil_terr(loc,((_tmp555="expecting %s but found %s",_tag_dyneither(
_tmp555,sizeof(char),26))),_tag_dyneither(_tmp554,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{struct Cyc_Absyn_TagType_struct _tmp558;struct Cyc_Absyn_TagType_struct*
_tmp557;t=(void*)((_tmp557=_cycalloc(sizeof(*_tmp557)),((_tmp557[0]=((_tmp558.tag=
18,((_tmp558.f1=(void*)((void*)_tmp76),_tmp558)))),_tmp557))));}goto _LL46;}_LL53:;
_LL54: t=_tmp5C == (void*)1?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;goto _LL46;_LL46:;}
goto _LL33;_LL42: if((_tmp56.String_c).tag != 5)goto _LL44;_tmp5E=(_tmp56.String_c).f1;
_LL43: {int len=(int)_get_dyneither_size(_tmp5E,sizeof(char));union Cyc_Absyn_Cnst_union
_tmp559;struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((
_tmp559.Int_c).tag=2,(((_tmp559.Int_c).f1=(void*)((void*)1),(((_tmp559.Int_c).f2=
len,_tmp559)))))),loc);{struct Cyc_Core_Opt*_tmp55A;elen->topt=((_tmp55A=
_cycalloc(sizeof(*_tmp55A)),((_tmp55A->v=(void*)Cyc_Absyn_uint_typ,_tmp55A))));}{
struct Cyc_Absyn_Upper_b_struct _tmp55D;struct Cyc_Absyn_Upper_b_struct*_tmp55C;t=
Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)((
_tmp55C=_cycalloc(sizeof(*_tmp55C)),((_tmp55C[0]=((_tmp55D.tag=0,((_tmp55D.f1=
elen,_tmp55D)))),_tmp55C)))),Cyc_Absyn_true_conref);}if(topt != 0){void*_tmp82=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp83;struct Cyc_Absyn_Tqual
_tmp84;_LL5D: if(_tmp82 <= (void*)4)goto _LL61;if(*((int*)_tmp82)!= 7)goto _LL5F;
_tmp83=((struct Cyc_Absyn_ArrayType_struct*)_tmp82)->f1;_tmp84=_tmp83.tq;_LL5E:
return Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,_tmp84,(struct Cyc_Absyn_Exp*)elen,((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),0);_LL5F: if(*((int*)
_tmp82)!= 4)goto _LL61;_LL60: if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(
te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp55E;e->topt=((_tmp55E=_cycalloc(sizeof(*
_tmp55E)),((_tmp55E->v=(void*)t,_tmp55E))));}Cyc_Tcutil_unchecked_cast(te,e,*
topt,(void*)3);t=*topt;}else{{struct Cyc_Absyn_Upper_b_struct _tmp561;struct Cyc_Absyn_Upper_b_struct*
_tmp560;t=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,Cyc_Tcenv_lookup_opt_type_vars(te)),Cyc_Absyn_const_tqual(0),(void*)((
_tmp560=_cycalloc(sizeof(*_tmp560)),((_tmp560[0]=((_tmp561.tag=0,((_tmp561.f1=
elen,_tmp561)))),_tmp560)))),Cyc_Absyn_true_conref);}if(!Cyc_Tcutil_unify(*topt,
t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){{struct Cyc_Core_Opt*_tmp562;e->topt=((
_tmp562=_cycalloc(sizeof(*_tmp562)),((_tmp562->v=(void*)t,_tmp562))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);t=*topt;}}goto _LL5C;_LL61:;_LL62: goto _LL5C;_LL5C:;}return t;}
_LL44: if((_tmp56.Null_c).tag != 6)goto _LL33;_LL45: {struct Cyc_List_List*_tmp8A=
Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Absyn_PointerType_struct _tmp575;struct
Cyc_Core_Opt*_tmp574;struct Cyc_Absyn_PtrAtts _tmp573;struct Cyc_Core_Opt*_tmp572;
struct Cyc_Absyn_PtrInfo _tmp571;struct Cyc_Absyn_PointerType_struct*_tmp570;t=(
void*)((_tmp570=_cycalloc(sizeof(*_tmp570)),((_tmp570[0]=((_tmp575.tag=4,((
_tmp575.f1=((_tmp571.elt_typ=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,((
_tmp574=_cycalloc(sizeof(*_tmp574)),((_tmp574->v=_tmp8A,_tmp574))))),((_tmp571.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp571.ptr_atts=((_tmp573.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,((_tmp572=_cycalloc(sizeof(*_tmp572)),((
_tmp572->v=_tmp8A,_tmp572))))),((_tmp573.nullable=Cyc_Absyn_true_conref,((
_tmp573.bounds=Cyc_Absyn_empty_conref(),((_tmp573.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp573.ptrloc=0,_tmp573)))))))))),_tmp571)))))),
_tmp575)))),_tmp570))));}goto _LL33;}_LL33:;}return t;}static void*Cyc_Tcexp_tcVar(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q,void*b);
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp91=b;struct Cyc_Absyn_Vardecl*_tmp92;struct
Cyc_Absyn_Fndecl*_tmp93;struct Cyc_Absyn_Vardecl*_tmp94;struct Cyc_Absyn_Vardecl*
_tmp95;struct Cyc_Absyn_Vardecl*_tmp96;_LL64: if((int)_tmp91 != 0)goto _LL66;_LL65: {
const char*_tmp579;void*_tmp578[1];struct Cyc_String_pa_struct _tmp577;return(
_tmp577.tag=0,((_tmp577.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
q)),((_tmp578[0]=& _tmp577,Cyc_Tcexp_expr_err(te,loc,0,((_tmp579="undeclared identifier: %s",
_tag_dyneither(_tmp579,sizeof(char),26))),_tag_dyneither(_tmp578,sizeof(void*),1)))))));}
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
Cyc_Absyn_Nmspace_union _tmp57A;(*q).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp57A.Loc_n).tag=
0,_tmp57A));}return(void*)_tmp96->type;_LL63:;}static void Cyc_Tcexp_check_format_args(
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
_tmp57D;void*_tmp57C;(_tmp57C=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA3->hd)->loc,((
_tmp57D="Cannot consume non-unique paths; do swap instead",_tag_dyneither(
_tmp57D,sizeof(char),49))),_tag_dyneither(_tmp57C,sizeof(void*),0)));}}}return;
_LL70:;}if(opt_args != 0){struct Cyc_List_List*_tmpA6=(struct Cyc_List_List*)
opt_args->v;for(0;desc_types != 0  && _tmpA6 != 0;(desc_types=desc_types->tl,_tmpA6=
_tmpA6->tl)){void*t=(void*)desc_types->hd;struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)
_tmpA6->hd;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(te,e,t)){{
const char*_tmp582;void*_tmp581[2];struct Cyc_String_pa_struct _tmp580;struct Cyc_String_pa_struct
_tmp57F;(_tmp57F.tag=0,((_tmp57F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp580.tag=0,((_tmp580.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp581[0]=& _tmp580,((_tmp581[1]=& _tmp57F,Cyc_Tcutil_terr(e->loc,((_tmp582="descriptor has type \n%s\n but argument has type \n%s",
_tag_dyneither(_tmp582,sizeof(char),51))),_tag_dyneither(_tmp581,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e)){const char*_tmp585;void*_tmp584;(_tmp584=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmpA6->hd)->loc,((_tmp585="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp585,sizeof(char),49))),_tag_dyneither(_tmp584,sizeof(void*),0)));}}
if(desc_types != 0){const char*_tmp588;void*_tmp587;(_tmp587=0,Cyc_Tcutil_terr(fmt->loc,((
_tmp588="too few arguments",_tag_dyneither(_tmp588,sizeof(char),18))),
_tag_dyneither(_tmp587,sizeof(void*),0)));}if(_tmpA6 != 0){const char*_tmp58B;void*
_tmp58A;(_tmp58A=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpA6->hd)->loc,((
_tmp58B="too many arguments",_tag_dyneither(_tmp58B,sizeof(char),19))),
_tag_dyneither(_tmp58A,sizeof(void*),0)));}}}static void*Cyc_Tcexp_tcUnPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmpB1=p;_LL78: if((
int)_tmpB1 != 0)goto _LL7A;_LL79: goto _LL7B;_LL7A: if((int)_tmpB1 != 2)goto _LL7C;
_LL7B: if(!Cyc_Tcutil_is_numeric(e)){const char*_tmp58F;void*_tmp58E[1];struct Cyc_String_pa_struct
_tmp58D;(_tmp58D.tag=0,((_tmp58D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp58E[0]=& _tmp58D,Cyc_Tcutil_terr(loc,((_tmp58F="expecting arithmetic type but found %s",
_tag_dyneither(_tmp58F,sizeof(char),39))),_tag_dyneither(_tmp58E,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL7C: if((int)_tmpB1
!= 11)goto _LL7E;_LL7D: Cyc_Tcutil_check_contains_assign(e);if(!Cyc_Tcutil_coerce_to_bool(
te,e)){const char*_tmp593;void*_tmp592[1];struct Cyc_String_pa_struct _tmp591;(
_tmp591.tag=0,((_tmp591.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp592[0]=& _tmp591,Cyc_Tcutil_terr(loc,((_tmp593="expecting integral or * type but found %s",
_tag_dyneither(_tmp593,sizeof(char),42))),_tag_dyneither(_tmp592,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;_LL7E: if((int)_tmpB1 != 12)goto _LL80;_LL7F: if(!Cyc_Tcutil_is_integral(
e)){const char*_tmp597;void*_tmp596[1];struct Cyc_String_pa_struct _tmp595;(_tmp595.tag=
0,((_tmp595.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp596[0]=& _tmp595,Cyc_Tcutil_terr(loc,((_tmp597="expecting integral type but found %s",
_tag_dyneither(_tmp597,sizeof(char),37))),_tag_dyneither(_tmp596,sizeof(void*),1)))))));}
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
_LL90:;_LL91: {const char*_tmp59A;void*_tmp599;(_tmp599=0,Cyc_Tcutil_terr(loc,((
_tmp59A="can't apply numelts to pointer/array of abstract length",_tag_dyneither(
_tmp59A,sizeof(char),56))),_tag_dyneither(_tmp599,sizeof(void*),0)));}_LL8B:;}
goto _LL84;_LL89:;_LL8A: {const char*_tmp59E;void*_tmp59D[1];struct Cyc_String_pa_struct
_tmp59C;(_tmp59C.tag=0,((_tmp59C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp59D[0]=& _tmp59C,Cyc_Tcutil_terr(loc,((_tmp59E="numelts requires pointer or array type, not %s",
_tag_dyneither(_tmp59E,sizeof(char),47))),_tag_dyneither(_tmp59D,sizeof(void*),1)))))));}
_LL84:;}return Cyc_Absyn_uint_typ;_LL82:;_LL83: {const char*_tmp5A1;void*_tmp5A0;(
_tmp5A0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp5A1="Non-unary primop",_tag_dyneither(_tmp5A1,sizeof(char),17))),
_tag_dyneither(_tmp5A0,sizeof(void*),0)));}_LL77:;}static void*Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*
checker)(struct Cyc_Absyn_Exp*));static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
if(!checker(e1)){{const char*_tmp5A5;void*_tmp5A4[1];struct Cyc_String_pa_struct
_tmp5A3;(_tmp5A3.tag=0,((_tmp5A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp5A4[0]=& _tmp5A3,Cyc_Tcutil_terr(e1->loc,((_tmp5A5="type %s cannot be used here",
_tag_dyneither(_tmp5A5,sizeof(char),28))),_tag_dyneither(_tmp5A4,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}if(!checker(e2)){{
const char*_tmp5A9;void*_tmp5A8[1];struct Cyc_String_pa_struct _tmp5A7;(_tmp5A7.tag=
0,((_tmp5A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((_tmp5A8[0]=& _tmp5A7,
Cyc_Tcutil_terr(e2->loc,((_tmp5A9="type %s cannot be used here",_tag_dyneither(
_tmp5A9,sizeof(char),28))),_tag_dyneither(_tmp5A8,sizeof(void*),1)))))));}return
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
_tmp5AC;void*_tmp5AB;(_tmp5AB=0,Cyc_Tcutil_terr(e1->loc,((_tmp5AC="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5AC,sizeof(char),50))),_tag_dyneither(_tmp5AB,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp5AF;void*_tmp5AE;(_tmp5AE=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5AF="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5AF,sizeof(char),54))),_tag_dyneither(_tmp5AE,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){const char*_tmp5B3;void*_tmp5B2[1];struct
Cyc_String_pa_struct _tmp5B1;(_tmp5B1.tag=0,((_tmp5B1.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp5B2[0]=& _tmp5B1,Cyc_Tcutil_terr(
e2->loc,((_tmp5B3="expecting int but found %s",_tag_dyneither(_tmp5B3,sizeof(
char),27))),_tag_dyneither(_tmp5B2,sizeof(void*),1)))))));}_tmpD6=Cyc_Absyn_compress_conref(
_tmpD6);{union Cyc_Absyn_Constraint_union _tmpDF=_tmpD6->v;void*_tmpE0;void*_tmpE1;
struct Cyc_Absyn_Exp*_tmpE2;_LL98: if((_tmpDF.Eq_constr).tag != 0)goto _LL9A;_tmpE0=(
_tmpDF.Eq_constr).f1;if((int)_tmpE0 != 0)goto _LL9A;_LL99: return t1;_LL9A: if((
_tmpDF.Eq_constr).tag != 0)goto _LL9C;_tmpE1=(_tmpDF.Eq_constr).f1;if(_tmpE1 <= (
void*)1)goto _LL9C;if(*((int*)_tmpE1)!= 0)goto _LL9C;_tmpE2=((struct Cyc_Absyn_Upper_b_struct*)
_tmpE1)->f1;_LL9B: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpD7)){const char*_tmp5B6;void*_tmp5B5;(_tmp5B5=0,Cyc_Tcutil_warn(e1->loc,((
_tmp5B6="pointer arithmetic on thin, zero-terminated pointer may be expensive.",
_tag_dyneither(_tmp5B6,sizeof(char),70))),_tag_dyneither(_tmp5B5,sizeof(void*),0)));}{
struct Cyc_Absyn_PointerType_struct _tmp5C0;struct Cyc_Absyn_PtrAtts _tmp5BF;struct
Cyc_Absyn_PtrInfo _tmp5BE;struct Cyc_Absyn_PointerType_struct*_tmp5BD;struct Cyc_Absyn_PointerType_struct*
_tmpE5=(_tmp5BD=_cycalloc(sizeof(*_tmp5BD)),((_tmp5BD[0]=((_tmp5C0.tag=4,((
_tmp5C0.f1=((_tmp5BE.elt_typ=(void*)_tmpD1,((_tmp5BE.elt_tq=_tmpD2,((_tmp5BE.ptr_atts=((
_tmp5BF.rgn=(void*)_tmpD4,((_tmp5BF.nullable=Cyc_Absyn_true_conref,((_tmp5BF.bounds=
Cyc_Absyn_bounds_dyneither_conref,((_tmp5BF.zero_term=_tmpD7,((_tmp5BF.ptrloc=0,
_tmp5BF)))))))))),_tmp5BE)))))),_tmp5C0)))),_tmp5BD)));Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmpE5,(void*)3);return(void*)_tmpE5;}_LL9C:;_LL9D:{union Cyc_Absyn_Constraint_union
_tmp5C1;_tmpD6->v=(union Cyc_Absyn_Constraint_union)(((_tmp5C1.Eq_constr).tag=0,(((
_tmp5C1.Eq_constr).f1=(void*)((void*)0),_tmp5C1))));}return t1;_LL97:;}_LL95:;
_LL96: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LL92:;}
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;
void*t1_elt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(
t2)){if(!Cyc_Tcutil_unify(t1,t2)){{const char*_tmp5C6;void*_tmp5C5[2];struct Cyc_String_pa_struct
_tmp5C4;struct Cyc_String_pa_struct _tmp5C3;(_tmp5C3.tag=0,((_tmp5C3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v)),((_tmp5C4.tag=0,((_tmp5C4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v)),((_tmp5C5[0]=& _tmp5C4,((_tmp5C5[1]=& _tmp5C3,Cyc_Tcutil_terr(
e1->loc,((_tmp5C6="pointer arithmetic on values of different types (%s != %s)",
_tag_dyneither(_tmp5C6,sizeof(char),59))),_tag_dyneither(_tmp5C5,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return Cyc_Absyn_sint_typ;}else{if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t1_elt),(void*)1)){const char*_tmp5C9;void*_tmp5C8;(_tmp5C8=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5C9="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5C9,sizeof(char),50))),_tag_dyneither(_tmp5C8,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1)){const char*_tmp5CC;void*_tmp5CB;(_tmp5CB=0,
Cyc_Tcutil_terr(e1->loc,((_tmp5CC="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5CC,sizeof(char),54))),_tag_dyneither(_tmp5CB,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){{const char*_tmp5D1;void*_tmp5D0[2];struct
Cyc_String_pa_struct _tmp5CF;struct Cyc_String_pa_struct _tmp5CE;(_tmp5CE.tag=0,((
_tmp5CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp5CF.tag=0,((_tmp5CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp5D0[0]=& _tmp5CF,((_tmp5D0[1]=& _tmp5CE,Cyc_Tcutil_terr(
e2->loc,((_tmp5D1="expecting either %s or int but found %s",_tag_dyneither(
_tmp5D1,sizeof(char),40))),_tag_dyneither(_tmp5D0,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t1;}}if(Cyc_Tcutil_is_pointer_type(t1))Cyc_Tcutil_unchecked_cast(
te,e1,Cyc_Absyn_sint_typ,(void*)3);if(Cyc_Tcutil_is_pointer_type(t2))Cyc_Tcutil_unchecked_cast(
te,e2,Cyc_Absyn_sint_typ,(void*)3);return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcAnyBinop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(e1);int e2_is_num=Cyc_Tcutil_is_numeric(
e2);void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(e1_is_num  && e2_is_num)return Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_unify(t1,
t2) && (Cyc_Tcutil_typ_kind(t1)== (void*)2  || Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,Cyc_Tcenv_lookup_opt_type_vars(te)))))return
Cyc_Absyn_sint_typ;else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(
te,e2,t1,(void*)3);return Cyc_Absyn_sint_typ;}else{if(Cyc_Tcutil_silent_castable(
te,loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2,(void*)3);return Cyc_Absyn_sint_typ;}
else{if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{{struct _tuple0 _tmp5D2;struct _tuple0 _tmpF8=(_tmp5D2.f1=
Cyc_Tcutil_compress(t1),((_tmp5D2.f2=Cyc_Tcutil_compress(t2),_tmp5D2)));void*
_tmpF9;struct Cyc_Absyn_PtrInfo _tmpFA;void*_tmpFB;void*_tmpFC;struct Cyc_Absyn_PtrInfo
_tmpFD;void*_tmpFE;_LL9F: _tmpF9=_tmpF8.f1;if(_tmpF9 <= (void*)4)goto _LLA1;if(*((
int*)_tmpF9)!= 4)goto _LLA1;_tmpFA=((struct Cyc_Absyn_PointerType_struct*)_tmpF9)->f1;
_tmpFB=(void*)_tmpFA.elt_typ;_tmpFC=_tmpF8.f2;if(_tmpFC <= (void*)4)goto _LLA1;if(*((
int*)_tmpFC)!= 4)goto _LLA1;_tmpFD=((struct Cyc_Absyn_PointerType_struct*)_tmpFC)->f1;
_tmpFE=(void*)_tmpFD.elt_typ;_LLA0: if(Cyc_Tcutil_unify(_tmpFB,_tmpFE))return Cyc_Absyn_sint_typ;
goto _LL9E;_LLA1:;_LLA2: goto _LL9E;_LL9E:;}{const char*_tmp5D7;void*_tmp5D6[2];
struct Cyc_String_pa_struct _tmp5D5;struct Cyc_String_pa_struct _tmp5D4;(_tmp5D4.tag=
0,((_tmp5D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp5D5.tag=0,((_tmp5D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp5D6[0]=& _tmp5D5,((_tmp5D6[1]=& _tmp5D4,Cyc_Tcutil_terr(
loc,((_tmp5D7="comparison not allowed between %s and %s",_tag_dyneither(_tmp5D7,
sizeof(char),41))),_tag_dyneither(_tmp5D6,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcBinPrimop(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*_tmp103=p;_LLA4: if((int)_tmp103 != 0)goto _LLA6;
_LLA5: return Cyc_Tcexp_tcPlus(te,e1,e2);_LLA6: if((int)_tmp103 != 2)goto _LLA8;_LLA7:
return Cyc_Tcexp_tcMinus(te,e1,e2);_LLA8: if((int)_tmp103 != 1)goto _LLAA;_LLA9: goto
_LLAB;_LLAA: if((int)_tmp103 != 3)goto _LLAC;_LLAB: return Cyc_Tcexp_tcArithBinop(te,
e1,e2,Cyc_Tcutil_is_numeric);_LLAC: if((int)_tmp103 != 4)goto _LLAE;_LLAD: goto _LLAF;
_LLAE: if((int)_tmp103 != 13)goto _LLB0;_LLAF: goto _LLB1;_LLB0: if((int)_tmp103 != 14)
goto _LLB2;_LLB1: goto _LLB3;_LLB2: if((int)_tmp103 != 15)goto _LLB4;_LLB3: goto _LLB5;
_LLB4: if((int)_tmp103 != 16)goto _LLB6;_LLB5: goto _LLB7;_LLB6: if((int)_tmp103 != 17)
goto _LLB8;_LLB7: goto _LLB9;_LLB8: if((int)_tmp103 != 18)goto _LLBA;_LLB9: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_integral);_LLBA: if((int)_tmp103 != 5)goto _LLBC;_LLBB: goto
_LLBD;_LLBC: if((int)_tmp103 != 6)goto _LLBE;_LLBD: goto _LLBF;_LLBE: if((int)_tmp103
!= 7)goto _LLC0;_LLBF: goto _LLC1;_LLC0: if((int)_tmp103 != 8)goto _LLC2;_LLC1: goto
_LLC3;_LLC2: if((int)_tmp103 != 9)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)_tmp103
!= 10)goto _LLC6;_LLC5: return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_LLC6:;_LLC7: {
const char*_tmp5DA;void*_tmp5D9;(_tmp5D9=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5DA="bad binary primop",
_tag_dyneither(_tmp5DA,sizeof(char),18))),_tag_dyneither(_tmp5D9,sizeof(void*),0)));}
_LLA3:;}static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*p,struct Cyc_List_List*es);static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)2  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
es)== 1)return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);Cyc_Tcexp_tcExpList(te,es);{void*t;switch(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LLC8: {const char*_tmp5DD;void*
_tmp5DC;return(_tmp5DC=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp5DD="primitive operator has 0 arguments",
_tag_dyneither(_tmp5DD,sizeof(char),35))),_tag_dyneither(_tmp5DC,sizeof(void*),0)));}
case 1: _LLC9: t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCA: t=Cyc_Tcexp_tcBinPrimop(
te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(es))->tl))->hd);break;default: _LLCB: {const char*_tmp5E0;void*_tmp5DF;
return(_tmp5DF=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp5E0="primitive operator has > 2 arguments",
_tag_dyneither(_tmp5E0,sizeof(char),37))),_tag_dyneither(_tmp5DF,sizeof(void*),0)));}}
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
_tmp114);_tmp123=_tmp122.f1;_tmp124=_tmp122.f2;if(!_tmp124){{const char*_tmp5E3;
void*_tmp5E2;(_tmp5E2=0,Cyc_Tcutil_terr(e->loc,((_tmp5E3="tuple projection cannot use sizeof or offsetof",
_tag_dyneither(_tmp5E3,sizeof(char),47))),_tag_dyneither(_tmp5E2,sizeof(void*),0)));}
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
te,_tmp11B);return;_LLE2:;_LLE3: goto _LLCD;_LLCD:;}{const char*_tmp5E7;void*
_tmp5E6[1];struct Cyc_String_pa_struct _tmp5E5;(_tmp5E5.tag=0,((_tmp5E5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp5E6[0]=&
_tmp5E5,Cyc_Tcutil_terr(e->loc,((_tmp5E7="attempt to write a const location: %s",
_tag_dyneither(_tmp5E7,sizeof(char),38))),_tag_dyneither(_tmp5E6,sizeof(void*),1)))))));}}
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,void*i);static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(!Cyc_Absyn_is_lvalue(e)){const char*
_tmp5EA;void*_tmp5E9;return(_tmp5E9=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp5EA="increment/decrement of non-lvalue",
_tag_dyneither(_tmp5EA,sizeof(char),34))),_tag_dyneither(_tmp5E9,sizeof(void*),0)));}
Cyc_Tcexp_check_writable(te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v;if(!Cyc_Tcutil_is_numeric(e)){void*telt=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt) || Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == (void*)0  || i == (
void*)1)){if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)1)){const char*
_tmp5ED;void*_tmp5EC;(_tmp5EC=0,Cyc_Tcutil_terr(e->loc,((_tmp5ED="can't perform arithmetic on abstract pointer type",
_tag_dyneither(_tmp5ED,sizeof(char),50))),_tag_dyneither(_tmp5EC,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t)){const char*_tmp5F0;void*_tmp5EF;(_tmp5EF=0,
Cyc_Tcutil_terr(e->loc,((_tmp5F0="can't perform arithmetic on non-aliasing pointer type",
_tag_dyneither(_tmp5F0,sizeof(char),54))),_tag_dyneither(_tmp5EF,sizeof(void*),0)));}}
else{const char*_tmp5F4;void*_tmp5F3[1];struct Cyc_String_pa_struct _tmp5F2;(
_tmp5F2.tag=0,((_tmp5F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5F3[0]=& _tmp5F2,Cyc_Tcutil_terr(e->loc,((_tmp5F4="expecting arithmetic or ? type but found %s",
_tag_dyneither(_tmp5F4,sizeof(char),44))),_tag_dyneither(_tmp5F3,sizeof(void*),1)))))));}}
return t;}}static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3);static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3){{const char*_tmp5F5;Cyc_Tcexp_tcTest(te,e1,((_tmp5F5="conditional expression",
_tag_dyneither(_tmp5F5,sizeof(char),23))));}Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(
te,topt,e3);{void*t=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,Cyc_Tcenv_lookup_opt_type_vars(
te));struct Cyc_List_List _tmp5F6;struct Cyc_List_List _tmp14E=(_tmp5F6.hd=e3,((
_tmp5F6.tl=0,_tmp5F6)));struct Cyc_List_List _tmp5F7;struct Cyc_List_List _tmp14F=(
_tmp5F7.hd=e2,((_tmp5F7.tl=(struct Cyc_List_List*)& _tmp14E,_tmp5F7)));if(!Cyc_Tcutil_coerce_list(
te,t,(struct Cyc_List_List*)& _tmp14F)){{const char*_tmp5FC;void*_tmp5FB[2];struct
Cyc_String_pa_struct _tmp5FA;struct Cyc_String_pa_struct _tmp5F9;(_tmp5F9.tag=0,((
_tmp5F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e3->topt))->v)),((_tmp5FA.tag=0,((
_tmp5FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v)),((_tmp5FB[0]=& _tmp5FA,((
_tmp5FB[1]=& _tmp5F9,Cyc_Tcutil_terr(loc,((_tmp5FC="conditional clause types do not match: %s != %s",
_tag_dyneither(_tmp5FC,sizeof(char),48))),_tag_dyneither(_tmp5FB,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}return t;}}static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{const char*_tmp5FD;Cyc_Tcexp_tcTest(
te,e1,((_tmp5FD="logical-and expression",_tag_dyneither(_tmp5FD,sizeof(char),23))));}{
const char*_tmp5FE;Cyc_Tcexp_tcTest(te,e2,((_tmp5FE="logical-and expression",
_tag_dyneither(_tmp5FE,sizeof(char),23))));}return Cyc_Absyn_sint_typ;}static void*
Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{
const char*_tmp5FF;Cyc_Tcexp_tcTest(te,e1,((_tmp5FF="logical-or expression",
_tag_dyneither(_tmp5FF,sizeof(char),22))));}{const char*_tmp600;Cyc_Tcexp_tcTest(
te,e2,((_tmp600="logical-or expression",_tag_dyneither(_tmp600,sizeof(char),22))));}
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
int*)_tmp15B)!= 7)goto _LL10F;_LL10E:{const char*_tmp603;void*_tmp602;(_tmp602=0,
Cyc_Tcutil_terr(loc,((_tmp603="cannot assign to an array",_tag_dyneither(_tmp603,
sizeof(char),26))),_tag_dyneither(_tmp602,sizeof(void*),0)));}goto _LL10C;_LL10F:;
_LL110: goto _LL10C;_LL10C:;}if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),(void*)
1)){const char*_tmp606;void*_tmp605;(_tmp605=0,Cyc_Tcutil_terr(loc,((_tmp606="type is abstract (can't determine size).",
_tag_dyneither(_tmp606,sizeof(char),41))),_tag_dyneither(_tmp605,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp609;void*_tmp608;return(_tmp608=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp609="assignment to non-lvalue",_tag_dyneither(_tmp609,sizeof(
char),25))),_tag_dyneither(_tmp608,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);if(po == 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(
e2)){const char*_tmp60C;void*_tmp60B;(_tmp60B=0,Cyc_Tcutil_terr(e2->loc,((_tmp60C="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp60C,sizeof(char),49))),_tag_dyneither(_tmp60B,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){const char*_tmp611;void*_tmp610[2];struct
Cyc_String_pa_struct _tmp60F;struct Cyc_String_pa_struct _tmp60E;void*_tmp164=(
_tmp60E.tag=0,((_tmp60E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp60F.tag=0,((_tmp60F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp610[0]=& _tmp60F,((_tmp610[1]=& _tmp60E,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp611="type mismatch: %s != %s",_tag_dyneither(_tmp611,sizeof(
char),24))),_tag_dyneither(_tmp610,sizeof(void*),2)))))))))))));Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp164;}}else{void*_tmp169=(void*)po->v;
void*_tmp16A=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp169,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp16A,t1) || Cyc_Tcutil_coerceable(_tmp16A) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp616;void*_tmp615[2];struct Cyc_String_pa_struct _tmp614;struct Cyc_String_pa_struct
_tmp613;void*_tmp16B=(_tmp613.tag=0,((_tmp613.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp614.tag=0,((_tmp614.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp615[0]=&
_tmp614,((_tmp615[1]=& _tmp613,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp616="Cannot use this operator in an assignment when the arguments have types %s and %s",
_tag_dyneither(_tmp616,sizeof(char),82))),_tag_dyneither(_tmp615,sizeof(void*),2)))))))))))));
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
fields->hd);}}{const char*_tmp61B;void*_tmp61A[2];struct Cyc_String_pa_struct
_tmp619;struct Cyc_String_pa_struct _tmp618;(_tmp618.tag=0,((_tmp618.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp619.tag=
0,((_tmp619.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
tu)),((_tmp61A[0]=& _tmp619,((_tmp61A[1]=& _tmp618,Cyc_Tcutil_terr(e->loc,((
_tmp61B="can't find a field in %s to inject a value of type %s",_tag_dyneither(
_tmp61B,sizeof(char),54))),_tag_dyneither(_tmp61A,sizeof(void*),2)))))))))))));}
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
e1);if(!Cyc_Tcutil_coerce_arg(_tmp184,e1,t2)){{const char*_tmp620;void*_tmp61F[2];
struct Cyc_String_pa_struct _tmp61E;struct Cyc_String_pa_struct _tmp61D;(_tmp61D.tag=
0,((_tmp61D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp61E.tag=0,((_tmp61E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp61F[0]=& _tmp61E,((_tmp61F[1]=& _tmp61D,Cyc_Tcutil_terr(e1->loc,((_tmp620="actual argument has type \n\t%s\n but formal has type \n\t%s.",
_tag_dyneither(_tmp620,sizeof(char),57))),_tag_dyneither(_tmp61F,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v,t2);Cyc_Tcutil_explain_failure();}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1)){
const char*_tmp623;void*_tmp622;(_tmp622=0,Cyc_Tcutil_terr(e1->loc,((_tmp623="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp623,sizeof(char),49))),_tag_dyneither(_tmp622,sizeof(void*),0)));}
_tmp182=_tmp182->tl;_tmp193=_tmp193->tl;}{int args_already_checked=0;{struct Cyc_List_List*
a=_tmp197;for(0;a != 0;a=a->tl){void*_tmp19E=(void*)a->hd;void*_tmp19F;int _tmp1A0;
int _tmp1A1;_LL125: if(_tmp19E <= (void*)17)goto _LL127;if(*((int*)_tmp19E)!= 3)goto
_LL127;_tmp19F=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp19E)->f1;_tmp1A0=((
struct Cyc_Absyn_Format_att_struct*)_tmp19E)->f2;_tmp1A1=((struct Cyc_Absyn_Format_att_struct*)
_tmp19E)->f3;_LL126:{struct _handler_cons _tmp1A2;_push_handler(& _tmp1A2);{int
_tmp1A4=0;if(setjmp(_tmp1A2.handler))_tmp1A4=1;if(!_tmp1A4){{struct Cyc_Absyn_Exp*
_tmp1A5=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,
_tmp1A0 - 1);struct Cyc_Core_Opt*fmt_args;if(_tmp1A1 == 0)fmt_args=0;else{struct Cyc_Core_Opt*
_tmp624;fmt_args=((_tmp624=_cycalloc(sizeof(*_tmp624)),((_tmp624->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1A1 - 1),_tmp624))));}
args_already_checked=1;{struct _RegionHandle _tmp1A7=_new_region("temp");struct
_RegionHandle*temp=& _tmp1A7;_push_region(temp);{void*_tmp1A8=_tmp19F;_LL12A: if((
int)_tmp1A8 != 0)goto _LL12C;_LL12B: Cyc_Tcexp_check_format_args(_tmp184,_tmp1A5,
fmt_args,temp,Cyc_Formatstr_get_format_typs);goto _LL129;_LL12C: if((int)_tmp1A8 != 
1)goto _LL129;_LL12D: Cyc_Tcexp_check_format_args(_tmp184,_tmp1A5,fmt_args,temp,
Cyc_Formatstr_get_scanf_typs);goto _LL129;_LL129:;};_pop_region(temp);}};
_pop_handler();}else{void*_tmp1A3=(void*)_exn_thrown;void*_tmp1AA=_tmp1A3;_LL12F:
if(_tmp1AA != Cyc_List_Nth)goto _LL131;_LL130:{const char*_tmp627;void*_tmp626;(
_tmp626=0,Cyc_Tcutil_terr(loc,((_tmp627="bad format arguments",_tag_dyneither(
_tmp627,sizeof(char),21))),_tag_dyneither(_tmp626,sizeof(void*),0)));}goto _LL12E;
_LL131:;_LL132:(void)_throw(_tmp1AA);_LL12E:;}}}goto _LL124;_LL127:;_LL128: goto
_LL124;_LL124:;}}if(_tmp193 != 0){const char*_tmp62A;void*_tmp629;(_tmp629=0,Cyc_Tcutil_terr(
loc,((_tmp62A="too few arguments for function",_tag_dyneither(_tmp62A,sizeof(
char),31))),_tag_dyneither(_tmp629,sizeof(void*),0)));}else{if((_tmp182 != 0  || 
_tmp194) || _tmp195 != 0){if(_tmp194)for(0;_tmp182 != 0;_tmp182=_tmp182->tl){Cyc_Tcexp_tcExp(
_tmp184,0,(struct Cyc_Absyn_Exp*)_tmp182->hd);}else{if(_tmp195 == 0){const char*
_tmp62D;void*_tmp62C;(_tmp62C=0,Cyc_Tcutil_terr(loc,((_tmp62D="too many arguments for function",
_tag_dyneither(_tmp62D,sizeof(char),32))),_tag_dyneither(_tmp62C,sizeof(void*),0)));}
else{void*_tmp1B2;int _tmp1B3;struct Cyc_Absyn_VarargInfo _tmp1B1=*_tmp195;_tmp1B2=(
void*)_tmp1B1.type;_tmp1B3=_tmp1B1.inject;{struct Cyc_Absyn_VarargCallInfo*
_tmp62E;struct Cyc_Absyn_VarargCallInfo*_tmp1B4=(_tmp62E=_cycalloc(sizeof(*
_tmp62E)),((_tmp62E->num_varargs=0,((_tmp62E->injectors=0,((_tmp62E->vai=(struct
Cyc_Absyn_VarargInfo*)_tmp195,_tmp62E)))))));*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp1B4;if(!_tmp1B3)for(0;_tmp182 != 0;_tmp182=_tmp182->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp182->hd;++ _tmp1B4->num_varargs;Cyc_Tcexp_tcExp(
_tmp184,(void**)& _tmp1B2,e1);if(!Cyc_Tcutil_coerce_arg(_tmp184,e1,_tmp1B2)){{
const char*_tmp633;void*_tmp632[2];struct Cyc_String_pa_struct _tmp631;struct Cyc_String_pa_struct
_tmp630;(_tmp630.tag=0,((_tmp630.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),((
_tmp631.tag=0,((_tmp631.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp1B2)),((_tmp632[0]=& _tmp631,((_tmp632[1]=& _tmp630,Cyc_Tcutil_terr(loc,((
_tmp633="vararg requires type %s but argument has type %s",_tag_dyneither(
_tmp633,sizeof(char),49))),_tag_dyneither(_tmp632,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1B2)
 && !Cyc_Tcutil_is_noalias_path(e1)){const char*_tmp636;void*_tmp635;(_tmp635=0,
Cyc_Tcutil_terr(e1->loc,((_tmp636="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp636,sizeof(char),49))),_tag_dyneither(_tmp635,sizeof(void*),0)));}}
else{void*_tmp1BB=Cyc_Tcutil_compress(_tmp1B2);struct Cyc_Absyn_TunionInfo _tmp1BC;
union Cyc_Absyn_TunionInfoU_union _tmp1BD;struct Cyc_Absyn_Tuniondecl**_tmp1BE;
struct Cyc_Absyn_Tuniondecl*_tmp1BF;struct Cyc_List_List*_tmp1C0;struct Cyc_Core_Opt*
_tmp1C1;_LL134: if(_tmp1BB <= (void*)4)goto _LL136;if(*((int*)_tmp1BB)!= 2)goto
_LL136;_tmp1BC=((struct Cyc_Absyn_TunionType_struct*)_tmp1BB)->f1;_tmp1BD=_tmp1BC.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp1BB)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL136;_tmp1BE=(_tmp1BD.KnownTunion).f1;_tmp1BF=*_tmp1BE;_tmp1C0=
_tmp1BC.targs;_tmp1C1=_tmp1BC.rgn;_LL135: {struct Cyc_Absyn_Tuniondecl*_tmp1C2=*
Cyc_Tcenv_lookup_tuniondecl(_tmp184,loc,_tmp1BF->name);struct Cyc_List_List*
fields=0;if(_tmp1C2->fields == 0){const char*_tmp63A;void*_tmp639[1];struct Cyc_String_pa_struct
_tmp638;(_tmp638.tag=0,((_tmp638.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1B2)),((_tmp639[0]=& _tmp638,Cyc_Tcutil_terr(loc,((
_tmp63A="can't inject into %s",_tag_dyneither(_tmp63A,sizeof(char),21))),
_tag_dyneither(_tmp639,sizeof(void*),1)))))));}else{fields=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C2->fields))->v;}if(!Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C1))->v,Cyc_Tcenv_curr_rgn(_tmp184))){const
char*_tmp63D;void*_tmp63C;(_tmp63C=0,Cyc_Tcutil_terr(loc,((_tmp63D="bad region for injected varargs",
_tag_dyneither(_tmp63D,sizeof(char),32))),_tag_dyneither(_tmp63C,sizeof(void*),0)));}{
struct _RegionHandle _tmp1C8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1C8;
_push_region(rgn);{struct Cyc_List_List*_tmp1C9=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp1C2->tvs,_tmp1C0);for(0;_tmp182 != 0;_tmp182=
_tmp182->tl){++ _tmp1B4->num_varargs;{struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)
_tmp182->hd;if(!args_already_checked){Cyc_Tcexp_tcExp(_tmp184,0,e1);if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v) && !Cyc_Tcutil_is_noalias_path(
e1)){const char*_tmp640;void*_tmp63F;(_tmp63F=0,Cyc_Tcutil_terr(e1->loc,((_tmp640="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp640,sizeof(char),49))),_tag_dyneither(_tmp63F,sizeof(void*),0)));}}{
struct Cyc_Absyn_Tunionfield*_tmp1CC=Cyc_Tcexp_tcInjection(_tmp184,e1,_tmp1B2,rgn,
_tmp1C9,fields);if(_tmp1CC != 0){struct Cyc_List_List*_tmp641;_tmp1B4->injectors=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1B4->injectors,((_tmp641=_cycalloc(sizeof(*_tmp641)),((_tmp641->hd=(struct
Cyc_Absyn_Tunionfield*)_tmp1CC,((_tmp641->tl=0,_tmp641)))))));}}}}};_pop_region(
rgn);}goto _LL133;}_LL136:;_LL137:{const char*_tmp644;void*_tmp643;(_tmp643=0,Cyc_Tcutil_terr(
loc,((_tmp644="bad inject vararg type",_tag_dyneither(_tmp644,sizeof(char),23))),
_tag_dyneither(_tmp643,sizeof(void*),0)));}goto _LL133;_LL133:;}}}}}}Cyc_Tcenv_check_effect_accessible(
_tmp184,loc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp191))->v);Cyc_Tcenv_check_rgn_partial_order(
_tmp184,loc,_tmp196);{void*_tmp1D1=_tmp192;_npop_handler(0);return _tmp1D1;}}
_LL122:;_LL123: {const char*_tmp647;void*_tmp646;void*_tmp1D4=(_tmp646=0,Cyc_Tcexp_expr_err(
_tmp184,loc,topt,((_tmp647="expected pointer to function",_tag_dyneither(_tmp647,
sizeof(char),29))),_tag_dyneither(_tmp646,sizeof(void*),0)));_npop_handler(0);
return _tmp1D4;}_LL11F:;}_LL11D:;_LL11E: {const char*_tmp64A;void*_tmp649;void*
_tmp1D7=(_tmp649=0,Cyc_Tcexp_expr_err(_tmp184,loc,topt,((_tmp64A="expected pointer to function",
_tag_dyneither(_tmp64A,sizeof(char),29))),_tag_dyneither(_tmp649,sizeof(void*),0)));
_npop_handler(0);return _tmp1D7;}_LL11A:;}};_pop_region(ter);}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e);static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,
e);if(!Cyc_Tcutil_coerce_arg(te,e,Cyc_Absyn_exn_typ)){const char*_tmp64E;void*
_tmp64D[1];struct Cyc_String_pa_struct _tmp64C;(_tmp64C.tag=0,((_tmp64C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v)),((_tmp64D[0]=& _tmp64C,Cyc_Tcutil_terr(loc,((_tmp64E="expected xtunion exn but found %s",
_tag_dyneither(_tmp64E,sizeof(char),34))),_tag_dyneither(_tmp64D,sizeof(void*),1)))))));}
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
struct _tuple5*_tmp651;struct Cyc_List_List*_tmp650;instantiation=((_tmp650=
_region_malloc(temp,sizeof(*_tmp650)),((_tmp650->hd=((_tmp651=_region_malloc(
temp,sizeof(*_tmp651)),((_tmp651->f1=(struct Cyc_Absyn_Tvar*)_tmp1E6->hd,((
_tmp651->f2=(void*)ts->hd,_tmp651)))))),((_tmp650->tl=instantiation,_tmp650))))));}}
if(ts != 0){const char*_tmp654;void*_tmp653;void*_tmp1F3=(_tmp653=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp654="too many type variables in instantiation",_tag_dyneither(
_tmp654,sizeof(char),41))),_tag_dyneither(_tmp653,sizeof(void*),0)));
_npop_handler(0);return _tmp1F3;}{struct Cyc_Absyn_FnType_struct _tmp65A;struct Cyc_Absyn_FnInfo
_tmp659;struct Cyc_Absyn_FnType_struct*_tmp658;void*new_fn_typ=Cyc_Tcutil_rsubstitute(
temp,instantiation,(void*)((_tmp658=_cycalloc(sizeof(*_tmp658)),((_tmp658[0]=((
_tmp65A.tag=8,((_tmp65A.f1=((_tmp659.tvars=_tmp1E6,((_tmp659.effect=_tmp1E7,((
_tmp659.ret_typ=(void*)_tmp1E8,((_tmp659.args=_tmp1E9,((_tmp659.c_varargs=
_tmp1EA,((_tmp659.cyc_varargs=_tmp1EB,((_tmp659.rgn_po=_tmp1EC,((_tmp659.attributes=
_tmp1ED,_tmp659)))))))))))))))),_tmp65A)))),_tmp658)))));struct Cyc_Absyn_PointerType_struct
_tmp664;struct Cyc_Absyn_PtrAtts _tmp663;struct Cyc_Absyn_PtrInfo _tmp662;struct Cyc_Absyn_PointerType_struct*
_tmp661;void*_tmp1F8=(void*)((_tmp661=_cycalloc(sizeof(*_tmp661)),((_tmp661[0]=((
_tmp664.tag=4,((_tmp664.f1=((_tmp662.elt_typ=(void*)new_fn_typ,((_tmp662.elt_tq=
_tmp1DE,((_tmp662.ptr_atts=((_tmp663.rgn=(void*)_tmp1E0,((_tmp663.nullable=
_tmp1E1,((_tmp663.bounds=_tmp1E2,((_tmp663.zero_term=_tmp1E3,((_tmp663.ptrloc=0,
_tmp663)))))))))),_tmp662)))))),_tmp664)))),_tmp661))));_npop_handler(0);return
_tmp1F8;}};_pop_region(temp);}_LL140:;_LL141: goto _LL13D;_LL13D:;}goto _LL138;
_LL13B:;_LL13C: goto _LL138;_LL138:;}{const char*_tmp668;void*_tmp667[1];struct Cyc_String_pa_struct
_tmp666;return(_tmp666.tag=0,((_tmp666.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp667[0]=& _tmp666,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp668="expecting polymorphic type but found %s",_tag_dyneither(
_tmp668,sizeof(char),40))),_tag_dyneither(_tmp667,sizeof(void*),1)))))));}}}
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,void**c);static void*Cyc_Tcexp_tcCast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*t,struct
Cyc_Absyn_Exp*e,void**c){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,t);Cyc_Tcexp_tcExp(te,(void**)& t,e);{void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;if(Cyc_Tcutil_silent_castable(te,loc,t2,t))*((void**)
_check_null(c))=(void*)1;else{void*crc=Cyc_Tcutil_castable(te,loc,t2,t);if(crc != (
void*)0)*((void**)_check_null(c))=crc;else{if(Cyc_Tcutil_zero_to_null(te,t,e))*((
void**)_check_null(c))=(void*)1;else{Cyc_Tcutil_unify(t2,t);{const char*_tmp66D;
void*_tmp66C[2];struct Cyc_String_pa_struct _tmp66B;struct Cyc_String_pa_struct
_tmp66A;void*_tmp1FF=(_tmp66A.tag=0,((_tmp66A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp66B.tag=0,((_tmp66B.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp66C[0]=&
_tmp66B,((_tmp66C[1]=& _tmp66A,Cyc_Tcexp_expr_err(te,loc,(void**)& t,((_tmp66D="cannot cast %s to %s",
_tag_dyneither(_tmp66D,sizeof(char),21))),_tag_dyneither(_tmp66C,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();return _tmp1FF;}}}}{struct _tuple0 _tmp66E;struct
_tuple0 _tmp205=(_tmp66E.f1=(void*)e->r,((_tmp66E.f2=Cyc_Tcutil_compress(t),
_tmp66E)));void*_tmp206;struct Cyc_Absyn_MallocInfo _tmp207;int _tmp208;void*
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
_LL149: if((Cyc_Evexp_eval_const_uint_exp(_tmp210)).f1 == 1){const char*_tmp671;
void*_tmp670;(_tmp670=0,Cyc_Tcutil_warn(loc,((_tmp671="cast from ? pointer to * pointer will lose size information",
_tag_dyneither(_tmp671,sizeof(char),60))),_tag_dyneither(_tmp670,sizeof(void*),0)));}
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
void**_tmp672;_tmp213=((_tmp672=_cycalloc(sizeof(*_tmp672)),((_tmp672[0]=_tmp217,
_tmp672))));}_tmp214=_tmp218;goto _LL14C;_LL14F:;_LL150: goto _LL14C;_LL14C:;}{
struct _RegionHandle _tmp21C=_new_region("r");struct _RegionHandle*r=& _tmp21C;
_push_region(r);Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_clear_notreadctxt(r,te),_tmp213,
e);;_pop_region(r);}if(Cyc_Tcutil_is_noalias_path(e)){const char*_tmp675;void*
_tmp674;(_tmp674=0,Cyc_Tcutil_terr(e->loc,((_tmp675="Cannot take the address of an alias-free path",
_tag_dyneither(_tmp675,sizeof(char),46))),_tag_dyneither(_tmp674,sizeof(void*),0)));}{
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
Cyc_Evexp_okay_szofarg(t)){const char*_tmp679;void*_tmp678[1];struct Cyc_String_pa_struct
_tmp677;(_tmp677.tag=0,((_tmp677.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp678[0]=& _tmp677,Cyc_Tcutil_terr(loc,((_tmp679="sizeof applied to type %s, which has unknown size here",
_tag_dyneither(_tmp679,sizeof(char),55))),_tag_dyneither(_tmp678,sizeof(void*),1)))))));}
if(topt != 0){void*_tmp229=Cyc_Tcutil_compress(*topt);void*_tmp22A;_LL15C: if(
_tmp229 <= (void*)4)goto _LL15E;if(*((int*)_tmp229)!= 18)goto _LL15E;_tmp22A=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp229)->f1;_LL15D: {struct Cyc_Absyn_Exp*
_tmp22B=Cyc_Absyn_sizeoftyp_exp(t,0);struct Cyc_Absyn_ValueofType_struct _tmp67C;
struct Cyc_Absyn_ValueofType_struct*_tmp67B;struct Cyc_Absyn_ValueofType_struct*
_tmp22C=(_tmp67B=_cycalloc(sizeof(*_tmp67B)),((_tmp67B[0]=((_tmp67C.tag=17,((
_tmp67C.f1=_tmp22B,_tmp67C)))),_tmp67B)));if(Cyc_Tcutil_unify(_tmp22A,(void*)
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
const char*_tmp680;void*_tmp67F[1];struct Cyc_String_pa_struct _tmp67E;(_tmp67E.tag=
0,((_tmp67E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp230),((_tmp67F[
0]=& _tmp67E,Cyc_Tcutil_terr(loc,((_tmp680="no field of struct/union has name %s",
_tag_dyneither(_tmp680,sizeof(char),37))),_tag_dyneither(_tmp67F,sizeof(void*),1)))))));}
bad_type=0;goto _LL165;_LL168: if(*((int*)_tmp232)!= 11)goto _LL16A;_tmp236=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp232)->f2;_LL169: if(!((int(*)(int(*pred)(
struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp230,
_tmp236)){const char*_tmp684;void*_tmp683[1];struct Cyc_String_pa_struct _tmp682;(
_tmp682.tag=0,((_tmp682.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp230),((
_tmp683[0]=& _tmp682,Cyc_Tcutil_terr(loc,((_tmp684="no field of struct/union has name %s",
_tag_dyneither(_tmp684,sizeof(char),37))),_tag_dyneither(_tmp683,sizeof(void*),1)))))));}
bad_type=0;goto _LL165;_LL16A:;_LL16B: goto _LL165;_LL165:;}if(bad_type){const char*
_tmp688;void*_tmp687[1];struct Cyc_String_pa_struct _tmp686;(_tmp686.tag=0,((
_tmp686.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp687[0]=& _tmp686,Cyc_Tcutil_terr(loc,((_tmp688="%s is not a known struct/union type",
_tag_dyneither(_tmp688,sizeof(char),36))),_tag_dyneither(_tmp687,sizeof(void*),1)))))));}
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
<= _tmp231){const char*_tmp68D;void*_tmp68C[2];struct Cyc_Int_pa_struct _tmp68B;
struct Cyc_Int_pa_struct _tmp68A;(_tmp68A.tag=1,((_tmp68A.f1=(unsigned long)((int)
_tmp231),((_tmp68B.tag=1,((_tmp68B.f1=(unsigned long)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp244),((_tmp68C[0]=& _tmp68B,((_tmp68C[1]=& _tmp68A,Cyc_Tcutil_terr(
loc,((_tmp68D="struct/union has too few components: %d <= %d",_tag_dyneither(
_tmp68D,sizeof(char),46))),_tag_dyneither(_tmp68C,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL16C;_LL171: if(*((int*)_tmp240)!= 9)goto _LL173;_tmp245=((struct
Cyc_Absyn_TupleType_struct*)_tmp240)->f1;_LL172: if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp245)<= _tmp231){const char*_tmp692;void*_tmp691[2];struct
Cyc_Int_pa_struct _tmp690;struct Cyc_Int_pa_struct _tmp68F;(_tmp68F.tag=1,((_tmp68F.f1=(
unsigned long)((int)_tmp231),((_tmp690.tag=1,((_tmp690.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp245),((_tmp691[0]=& _tmp690,((_tmp691[
1]=& _tmp68F,Cyc_Tcutil_terr(loc,((_tmp692="tuple has too few components: %d <= %d",
_tag_dyneither(_tmp692,sizeof(char),39))),_tag_dyneither(_tmp691,sizeof(void*),2)))))))))))));}
bad_type=0;goto _LL16C;_LL173: if(*((int*)_tmp240)!= 3)goto _LL175;_tmp246=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp240)->f1;_tmp247=_tmp246.field_info;if((((((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp240)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL175;_tmp248=(_tmp247.KnownTunionfield).f2;_LL174: if(((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp248->typs)< _tmp231){const char*_tmp697;
void*_tmp696[2];struct Cyc_Int_pa_struct _tmp695;struct Cyc_Int_pa_struct _tmp694;(
_tmp694.tag=1,((_tmp694.f1=(unsigned long)((int)_tmp231),((_tmp695.tag=1,((
_tmp695.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp248->typs),((_tmp696[0]=& _tmp695,((_tmp696[1]=& _tmp694,Cyc_Tcutil_terr(loc,((
_tmp697="tunionfield has too few components: %d < %d",_tag_dyneither(_tmp697,
sizeof(char),44))),_tag_dyneither(_tmp696,sizeof(void*),2)))))))))))));}bad_type=
0;goto _LL16C;_LL175:;_LL176: goto _LL16C;_LL16C:;}if(bad_type){const char*_tmp69B;
void*_tmp69A[1];struct Cyc_String_pa_struct _tmp699;(_tmp699.tag=0,((_tmp699.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp69A[
0]=& _tmp699,Cyc_Tcutil_terr(loc,((_tmp69B="%s is not a known type",
_tag_dyneither(_tmp69B,sizeof(char),23))),_tag_dyneither(_tmp69A,sizeof(void*),1)))))));}
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
Cyc_Tcutil_typ_kind(_tmp25C),(void*)1)){const char*_tmp69E;void*_tmp69D;(_tmp69D=
0,Cyc_Tcutil_terr(loc,((_tmp69E="can't dereference abstract pointer type",
_tag_dyneither(_tmp69E,sizeof(char),40))),_tag_dyneither(_tmp69D,sizeof(void*),0)));}{
void*_tmp263=_tmp25C;_npop_handler(0);return _tmp263;}_LL17A:;_LL17B: {const char*
_tmp6A2;void*_tmp6A1[1];struct Cyc_String_pa_struct _tmp6A0;void*_tmp267=(_tmp6A0.tag=
0,((_tmp6A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp6A1[0]=& _tmp6A0,Cyc_Tcexp_expr_err(_tmp259,loc,topt,((_tmp6A2="expecting * or @ type but found %s",
_tag_dyneither(_tmp6A2,sizeof(char),35))),_tag_dyneither(_tmp6A1,sizeof(void*),1)))))));
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
if(_tmp272 == 0){const char*_tmp6A7;void*_tmp6A6[2];struct Cyc_String_pa_struct
_tmp6A5;struct Cyc_String_pa_struct _tmp6A4;return(_tmp6A4.tag=0,((_tmp6A4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6A5.tag=0,((_tmp6A5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp26C->name)),((
_tmp6A6[0]=& _tmp6A5,((_tmp6A6[1]=& _tmp6A4,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6A7="type %s has no %s field",_tag_dyneither(_tmp6A7,sizeof(char),24))),
_tag_dyneither(_tmp6A6,sizeof(void*),2)))))))))))));}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp26C->impl))->exist_vars != 0){const char*_tmp6AA;void*_tmp6A9;
return(_tmp6A9=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6AA="must use pattern-matching to access fields of existential types",
_tag_dyneither(_tmp6AA,sizeof(char),64))),_tag_dyneither(_tmp6A9,sizeof(void*),0)));}{
void*t2;{struct _RegionHandle _tmp279=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp279;_push_region(rgn);{struct Cyc_List_List*_tmp27A=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp26C->tvs,_tmp26D);t2=Cyc_Tcutil_rsubstitute(rgn,
_tmp27A,(void*)_tmp272->type);if(((void*)_tmp26C->kind == (void*)1  && !Cyc_Tcenv_in_notreadctxt(
te)) && !Cyc_Tcutil_bits_only(t2)){const char*_tmp6AE;void*_tmp6AD[1];struct Cyc_String_pa_struct
_tmp6AC;void*_tmp27E=(_tmp6AC.tag=0,((_tmp6AC.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp6AD[0]=& _tmp6AC,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6AE="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp6AE,sizeof(char),56))),_tag_dyneither(_tmp6AD,sizeof(void*),1)))))));
_npop_handler(0);return _tmp27E;}};_pop_region(rgn);}return t2;}}_LL17F: if(*((int*)
_tmp268)!= 11)goto _LL181;_tmp26E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp268)->f1;_tmp26F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp268)->f2;_LL180: {
struct Cyc_Absyn_Aggrfield*_tmp27F=Cyc_Absyn_lookup_field(_tmp26F,f);if(_tmp27F == 
0){const char*_tmp6B2;void*_tmp6B1[1];struct Cyc_String_pa_struct _tmp6B0;return(
_tmp6B0.tag=0,((_tmp6B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6B1[0]=& _tmp6B0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6B2="type has no %s field",
_tag_dyneither(_tmp6B2,sizeof(char),21))),_tag_dyneither(_tmp6B1,sizeof(void*),1)))))));}
if((_tmp26E == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp27F->type)){const char*_tmp6B6;void*_tmp6B5[1];struct Cyc_String_pa_struct
_tmp6B4;return(_tmp6B4.tag=0,((_tmp6B4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp6B5[0]=& _tmp6B4,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6B6="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp6B6,sizeof(char),56))),_tag_dyneither(_tmp6B5,sizeof(void*),1)))))));}return(
void*)_tmp27F->type;}_LL181: if(*((int*)_tmp268)!= 7)goto _LL183;{const char*
_tmp6B7;if(!(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp6B7="size",_tag_dyneither(
_tmp6B7,sizeof(char),5))))== 0))goto _LL183;}_LL182: goto _LL184;_LL183: if(*((int*)
_tmp268)!= 4)goto _LL185;{const char*_tmp6B8;if(!(Cyc_strcmp((struct _dyneither_ptr)*
f,((_tmp6B8="size",_tag_dyneither(_tmp6B8,sizeof(char),5))))== 0))goto _LL185;}
_LL184:{const char*_tmp6BC;void*_tmp6BB[1];struct Cyc_String_pa_struct _tmp6BA;(
_tmp6BA.tag=0,((_tmp6BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp6BB[0]=& _tmp6BA,Cyc_Tcutil_warn(e->loc,((_tmp6BC="deprecated `.size' used here -- change to numelts(%s)",
_tag_dyneither(_tmp6BC,sizeof(char),54))),_tag_dyneither(_tmp6BB,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Primop_e_struct _tmp6C2;struct Cyc_List_List*_tmp6C1;struct Cyc_Absyn_Primop_e_struct*
_tmp6C0;(void*)(outer_e->r=(void*)((void*)((_tmp6C0=_cycalloc(sizeof(*_tmp6C0)),((
_tmp6C0[0]=((_tmp6C2.tag=3,((_tmp6C2.f1=(void*)((void*)19),((_tmp6C2.f2=((
_tmp6C1=_cycalloc(sizeof(*_tmp6C1)),((_tmp6C1->hd=e,((_tmp6C1->tl=0,_tmp6C1)))))),
_tmp6C2)))))),_tmp6C0))))));}return Cyc_Absyn_uint_typ;_LL185:;_LL186: {const char*
_tmp6C3;if(Cyc_strcmp((struct _dyneither_ptr)*f,((_tmp6C3="size",_tag_dyneither(
_tmp6C3,sizeof(char),5))))== 0){const char*_tmp6C7;void*_tmp6C6[1];struct Cyc_String_pa_struct
_tmp6C5;return(_tmp6C5.tag=0,((_tmp6C5.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp6C6[0]=& _tmp6C5,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6C7="expecting struct, union, or array, found %s",
_tag_dyneither(_tmp6C7,sizeof(char),44))),_tag_dyneither(_tmp6C6,sizeof(void*),1)))))));}
else{const char*_tmp6CB;void*_tmp6CA[1];struct Cyc_String_pa_struct _tmp6C9;return(
_tmp6C9.tag=0,((_tmp6C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp6CA[0]=& _tmp6C9,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp6CB="expecting struct or union, found %s",_tag_dyneither(
_tmp6CB,sizeof(char),36))),_tag_dyneither(_tmp6CA,sizeof(void*),1)))))));}}
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
if(_tmp2A3 == 0){const char*_tmp6D0;void*_tmp6CF[2];struct Cyc_String_pa_struct
_tmp6CE;struct Cyc_String_pa_struct _tmp6CD;return(_tmp6CD.tag=0,((_tmp6CD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6CE.tag=0,((_tmp6CE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp29F->name)),((
_tmp6CF[0]=& _tmp6CE,((_tmp6CF[1]=& _tmp6CD,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6D0="type %s has no %s field",_tag_dyneither(_tmp6D0,sizeof(char),24))),
_tag_dyneither(_tmp6CF,sizeof(void*),2)))))))))))));}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp29F->impl))->exist_vars != 0){const char*_tmp6D3;void*_tmp6D2;
return(_tmp6D2=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6D3="must use pattern-matching to access fields of existential types",
_tag_dyneither(_tmp6D3,sizeof(char),64))),_tag_dyneither(_tmp6D2,sizeof(void*),0)));}{
void*t3;{struct _RegionHandle _tmp2AA=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp2AA;_push_region(rgn);{struct Cyc_List_List*_tmp2AB=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp29F->tvs,_tmp2A0);t3=Cyc_Tcutil_rsubstitute(rgn,
_tmp2AB,(void*)_tmp2A3->type);};_pop_region(rgn);}if(((void*)_tmp29F->kind == (
void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only(t3)){const
char*_tmp6D7;void*_tmp6D6[1];struct Cyc_String_pa_struct _tmp6D5;return(_tmp6D5.tag=
0,((_tmp6D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp6D6[0]=&
_tmp6D5,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6D7="cannot read union member %s since it is not `bits-only'",
_tag_dyneither(_tmp6D7,sizeof(char),56))),_tag_dyneither(_tmp6D6,sizeof(void*),1)))))));}
return t3;}}_LL18F: if(*((int*)_tmp29B)!= 11)goto _LL191;_tmp2A1=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp29B)->f1;_tmp2A2=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp29B)->f2;_LL190: {
struct Cyc_Absyn_Aggrfield*_tmp2AF=Cyc_Absyn_lookup_field(_tmp2A2,f);if(_tmp2AF == 
0){const char*_tmp6DB;void*_tmp6DA[1];struct Cyc_String_pa_struct _tmp6D9;return(
_tmp6D9.tag=0,((_tmp6D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((
_tmp6DA[0]=& _tmp6D9,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6DB="type has no %s field",
_tag_dyneither(_tmp6DB,sizeof(char),21))),_tag_dyneither(_tmp6DA,sizeof(void*),1)))))));}
if((_tmp2A1 == (void*)1  && !Cyc_Tcenv_in_notreadctxt(te)) && !Cyc_Tcutil_bits_only((
void*)_tmp2AF->type)){const char*_tmp6DF;void*_tmp6DE[1];struct Cyc_String_pa_struct
_tmp6DD;return(_tmp6DD.tag=0,((_tmp6DD.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*f),((_tmp6DE[0]=& _tmp6DD,Cyc_Tcexp_expr_err(te,loc,topt,((
_tmp6DF="cannot read union member %s since it is not `bits-only'",_tag_dyneither(
_tmp6DF,sizeof(char),56))),_tag_dyneither(_tmp6DE,sizeof(void*),1)))))));}return(
void*)_tmp2AF->type;}_LL191:;_LL192: goto _LL18C;_LL18C:;}goto _LL187;_LL18A:;
_LL18B: goto _LL187;_LL187:;}{const char*_tmp6E3;void*_tmp6E2[1];struct Cyc_String_pa_struct
_tmp6E1;return(_tmp6E1.tag=0,((_tmp6E1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp6E2[0]=& _tmp6E1,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp6E3="expecting struct or union pointer, found %s",
_tag_dyneither(_tmp6E3,sizeof(char),44))),_tag_dyneither(_tmp6E2,sizeof(void*),1)))))));}}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index);static void*Cyc_Tcexp_ithTupleType(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*ts,
struct Cyc_Absyn_Exp*index){unsigned int _tmp2BA;int _tmp2BB;struct _tuple7 _tmp2B9=
Cyc_Evexp_eval_const_uint_exp(index);_tmp2BA=_tmp2B9.f1;_tmp2BB=_tmp2B9.f2;if(!
_tmp2BB){const char*_tmp6E6;void*_tmp6E5;return(_tmp6E5=0,Cyc_Tcexp_expr_err(te,
loc,0,((_tmp6E6="tuple projection cannot use sizeof or offsetof",_tag_dyneither(
_tmp6E6,sizeof(char),47))),_tag_dyneither(_tmp6E5,sizeof(void*),0)));}{struct
_handler_cons _tmp2BE;_push_handler(& _tmp2BE);{int _tmp2C0=0;if(setjmp(_tmp2BE.handler))
_tmp2C0=1;if(!_tmp2C0){{void*_tmp2C1=(*((struct _tuple9*(*)(struct Cyc_List_List*x,
int n))Cyc_List_nth)(ts,(int)_tmp2BA)).f2;_npop_handler(0);return _tmp2C1;};
_pop_handler();}else{void*_tmp2BF=(void*)_exn_thrown;void*_tmp2C3=_tmp2BF;_LL194:
if(_tmp2C3 != Cyc_List_Nth)goto _LL196;_LL195: {const char*_tmp6EB;void*_tmp6EA[2];
struct Cyc_Int_pa_struct _tmp6E9;struct Cyc_Int_pa_struct _tmp6E8;return(_tmp6E8.tag=
1,((_tmp6E8.f1=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
ts),((_tmp6E9.tag=1,((_tmp6E9.f1=(unsigned long)((int)_tmp2BA),((_tmp6EA[0]=&
_tmp6E9,((_tmp6EA[1]=& _tmp6E8,Cyc_Tcexp_expr_err(te,loc,0,((_tmp6EB="index is %d but tuple has only %d fields",
_tag_dyneither(_tmp6EB,sizeof(char),41))),_tag_dyneither(_tmp6EA,sizeof(void*),2)))))))))))));}
_LL196:;_LL197:(void)_throw(_tmp2C3);_LL193:;}}}}static void*Cyc_Tcexp_tcSubscript(
struct Cyc_Tcenv_Tenv*te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*
te_orig,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){struct _RegionHandle _tmp2C8=_new_region("r");struct
_RegionHandle*r=& _tmp2C8;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp2C9=Cyc_Tcenv_clear_notreadctxt(
r,te_orig);Cyc_Tcexp_tcExp(_tmp2C9,0,e1);Cyc_Tcexp_tcExp(_tmp2C9,0,e2);{void*t1=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*
t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2C9,e2)){const char*_tmp6EF;void*_tmp6EE[1];
struct Cyc_String_pa_struct _tmp6ED;void*_tmp2CD=(_tmp6ED.tag=0,((_tmp6ED.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((
_tmp6EE[0]=& _tmp6ED,Cyc_Tcexp_expr_err(_tmp2C9,e2->loc,topt,((_tmp6EF="expecting int subscript, found %s",
_tag_dyneither(_tmp6EF,sizeof(char),34))),_tag_dyneither(_tmp6EE,sizeof(void*),1)))))));
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
_LL19F;_LL19F:;}if(emit_warning){const char*_tmp6F2;void*_tmp6F1;(_tmp6F1=0,Cyc_Tcutil_warn(
e2->loc,((_tmp6F2="subscript on thin, zero-terminated pointer could be expensive.",
_tag_dyneither(_tmp6F2,sizeof(char),63))),_tag_dyneither(_tmp6F1,sizeof(void*),0)));}}
else{if(Cyc_Tcutil_is_const_exp(_tmp2C9,e2)){unsigned int _tmp2E4;int _tmp2E5;
struct _tuple7 _tmp2E3=Cyc_Evexp_eval_const_uint_exp(e2);_tmp2E4=_tmp2E3.f1;
_tmp2E5=_tmp2E3.f2;if(_tmp2E5)Cyc_Tcutil_check_bound(loc,_tmp2E4,_tmp2D4);}else{
if(Cyc_Tcutil_is_bound_one(_tmp2D4) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp2D5)){const char*_tmp6F5;void*_tmp6F4;(_tmp6F4=0,Cyc_Tcutil_warn(
e1->loc,((_tmp6F5="subscript applied to pointer to one object",_tag_dyneither(
_tmp6F5,sizeof(char),43))),_tag_dyneither(_tmp6F4,sizeof(void*),0)));}Cyc_Tcutil_check_nonzero_bound(
loc,_tmp2D4);}}Cyc_Tcenv_check_rgn_accessible(_tmp2C9,loc,_tmp2D3);if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp2D0),(void*)1)){const char*_tmp6F8;void*_tmp6F7;(_tmp6F7=
0,Cyc_Tcutil_terr(e1->loc,((_tmp6F8="can't subscript an abstract pointer",
_tag_dyneither(_tmp6F8,sizeof(char),36))),_tag_dyneither(_tmp6F7,sizeof(void*),0)));}{
void*_tmp2EA=_tmp2D0;_npop_handler(0);return _tmp2EA;}_LL19B: if(*((int*)_tmp2CE)
!= 9)goto _LL19D;_tmp2D6=((struct Cyc_Absyn_TupleType_struct*)_tmp2CE)->f1;_LL19C: {
void*_tmp2EB=Cyc_Tcexp_ithTupleType(_tmp2C9,loc,_tmp2D6,e2);_npop_handler(0);
return _tmp2EB;}_LL19D:;_LL19E: {const char*_tmp6FC;void*_tmp6FB[1];struct Cyc_String_pa_struct
_tmp6FA;void*_tmp2EF=(_tmp6FA.tag=0,((_tmp6FA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp6FB[0]=& _tmp6FA,Cyc_Tcexp_expr_err(
_tmp2C9,loc,topt,((_tmp6FC="subscript applied to %s",_tag_dyneither(_tmp6FC,
sizeof(char),24))),_tag_dyneither(_tmp6FB,sizeof(void*),1)))))));_npop_handler(0);
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
_tuple9*_tmp6FF;struct Cyc_List_List*_tmp6FE;fields=((_tmp6FE=_cycalloc(sizeof(*
_tmp6FE)),((_tmp6FE->hd=((_tmp6FF=_cycalloc(sizeof(*_tmp6FF)),((_tmp6FF->f1=(*((
struct _tuple9*)_tmp2F1->hd)).f1,((_tmp6FF->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp6FF)))))),((_tmp6FE->tl=
fields,_tmp6FE))))));}}done=1;goto _LL1A6;_LL1A9:;_LL1AA: goto _LL1A6;_LL1A6:;}if(!
done)for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(struct Cyc_Absyn_Exp*)
es->hd);{struct _tuple9*_tmp702;struct Cyc_List_List*_tmp701;fields=((_tmp701=
_cycalloc(sizeof(*_tmp701)),((_tmp701->hd=((_tmp702=_cycalloc(sizeof(*_tmp702)),((
_tmp702->f1=Cyc_Absyn_empty_tqual(0),((_tmp702->f2=(void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v,_tmp702)))))),((_tmp701->tl=
fields,_tmp701))))));}}{struct Cyc_Absyn_TupleType_struct _tmp705;struct Cyc_Absyn_TupleType_struct*
_tmp704;return(void*)((_tmp704=_cycalloc(sizeof(*_tmp704)),((_tmp704[0]=((
_tmp705.tag=9,((_tmp705.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields),_tmp705)))),_tmp704))));}}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple2*t,struct Cyc_List_List*
des);static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple2*t,struct Cyc_List_List*des){const char*_tmp708;void*
_tmp707;return(_tmp707=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp708="tcCompoundLit",
_tag_dyneither(_tmp708,sizeof(char),14))),_tag_dyneither(_tmp707,sizeof(void*),0)));}
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
_tmp70E;union Cyc_Absyn_Cnst_union _tmp70D;struct Cyc_Absyn_Const_e_struct*_tmp70C;
struct Cyc_Absyn_Const_e_struct*_tmp2FD=(_tmp70C=_cycalloc(sizeof(*_tmp70C)),((
_tmp70C[0]=((_tmp70E.tag=0,((_tmp70E.f1=(union Cyc_Absyn_Cnst_union)(((_tmp70D.Int_c).tag=
2,(((_tmp70D.Int_c).f1=(void*)((void*)1),(((_tmp70D.Int_c).f2=_tmp2FC,_tmp70D)))))),
_tmp70E)))),_tmp70C)));struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)
_tmp2FD,loc);if(zero_term){struct Cyc_Absyn_Exp*_tmp2FE=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp2FC - 1);if(!Cyc_Tcutil_is_zero(
_tmp2FE)){const char*_tmp711;void*_tmp710;(_tmp710=0,Cyc_Tcutil_terr(_tmp2FE->loc,((
_tmp711="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp711,
sizeof(char),45))),_tag_dyneither(_tmp710,sizeof(void*),0)));}}{struct Cyc_Core_Opt*
_tmp712;sz_exp->topt=((_tmp712=_cycalloc(sizeof(*_tmp712)),((_tmp712->v=(void*)
Cyc_Absyn_uint_typ,_tmp712))));}res_t2=Cyc_Absyn_array_typ(res,Cyc_Absyn_empty_tqual(
0),(struct Cyc_Absyn_Exp*)sz_exp,zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,
0);{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){Cyc_Tcexp_tcExpInitializer(
te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(!Cyc_Tcutil_coerce_list(te,res,
es)){const char*_tmp716;void*_tmp715[1];struct Cyc_String_pa_struct _tmp714;(
_tmp714.tag=0,((_tmp714.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
res)),((_tmp715[0]=& _tmp714,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,((_tmp716="elements of array do not all have the same type (%s)",
_tag_dyneither(_tmp716,sizeof(char),53))),_tag_dyneither(_tmp715,sizeof(void*),1)))))));}};
_pop_region(r);}{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*
ds=(*((struct _tuple8*)des->hd)).f1;if(ds != 0){void*_tmp308=(void*)ds->hd;struct
Cyc_Absyn_Exp*_tmp309;_LL1AC: if(*((int*)_tmp308)!= 1)goto _LL1AE;_LL1AD:{const
char*_tmp719;void*_tmp718;(_tmp718=0,Cyc_Tcutil_terr(loc,((_tmp719="only array index designators are supported",
_tag_dyneither(_tmp719,sizeof(char),43))),_tag_dyneither(_tmp718,sizeof(void*),0)));}
goto _LL1AB;_LL1AE: if(*((int*)_tmp308)!= 0)goto _LL1AB;_tmp309=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp308)->f1;_LL1AF: Cyc_Tcexp_tcExpInitializer(te,0,_tmp309);{unsigned int
_tmp30D;int _tmp30E;struct _tuple7 _tmp30C=Cyc_Evexp_eval_const_uint_exp(_tmp309);
_tmp30D=_tmp30C.f1;_tmp30E=_tmp30C.f2;if(!_tmp30E){const char*_tmp71C;void*
_tmp71B;(_tmp71B=0,Cyc_Tcutil_terr(_tmp309->loc,((_tmp71C="index designator cannot use sizeof or offsetof",
_tag_dyneither(_tmp71C,sizeof(char),47))),_tag_dyneither(_tmp71B,sizeof(void*),0)));}
else{if(_tmp30D != offset){const char*_tmp721;void*_tmp720[2];struct Cyc_Int_pa_struct
_tmp71F;struct Cyc_Int_pa_struct _tmp71E;(_tmp71E.tag=1,((_tmp71E.f1=(
unsigned long)((int)_tmp30D),((_tmp71F.tag=1,((_tmp71F.f1=(unsigned long)offset,((
_tmp720[0]=& _tmp71F,((_tmp720[1]=& _tmp71E,Cyc_Tcutil_terr(_tmp309->loc,((_tmp721="expecting index designator %d but found %d",
_tag_dyneither(_tmp721,sizeof(char),43))),_tag_dyneither(_tmp720,sizeof(void*),2)))))))))))));}}
goto _LL1AB;}_LL1AB:;}}}return res_t2;}static void*Cyc_Tcexp_tcComprehension(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term);static
void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*
body,int*is_zero_term){Cyc_Tcexp_tcExp(te,0,bound);{void*_tmp315=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(bound->topt))->v);_LL1B1: if(_tmp315 <= (
void*)4)goto _LL1B3;if(*((int*)_tmp315)!= 18)goto _LL1B3;_LL1B2: goto _LL1B0;_LL1B3:;
_LL1B4: if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){const char*_tmp725;void*_tmp724[
1];struct Cyc_String_pa_struct _tmp723;(_tmp723.tag=0,((_tmp723.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v)),((_tmp724[0]=& _tmp723,Cyc_Tcutil_terr(bound->loc,((
_tmp725="expecting unsigned int, found %s",_tag_dyneither(_tmp725,sizeof(char),
33))),_tag_dyneither(_tmp724,sizeof(void*),1)))))));}_LL1B0:;}if(!(vd->tq).real_const){
const char*_tmp728;void*_tmp727;(_tmp727=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp728="comprehension index variable is not declared const!",
_tag_dyneither(_tmp728,sizeof(char),52))),_tag_dyneither(_tmp727,sizeof(void*),0)));}{
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
if(!Cyc_Tcutil_is_const_exp(_tmp31C,bound)){const char*_tmp72B;void*_tmp72A;(
_tmp72A=0,Cyc_Tcutil_terr(bound->loc,((_tmp72B="bound is not constant",
_tag_dyneither(_tmp72B,sizeof(char),22))),_tag_dyneither(_tmp72A,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(_tmp31C,body)){const char*_tmp72E;void*_tmp72D;(
_tmp72D=0,Cyc_Tcutil_terr(bound->loc,((_tmp72E="body is not constant",
_tag_dyneither(_tmp72E,sizeof(char),21))),_tag_dyneither(_tmp72D,sizeof(void*),0)));}}
if(_tmp31F != 0  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,*
_tmp31F)){struct Cyc_Absyn_Exp*_tmp32E=Cyc_Absyn_uint_exp(1,0);{struct Cyc_Core_Opt*
_tmp72F;_tmp32E->topt=((_tmp72F=_cycalloc(sizeof(*_tmp72F)),((_tmp72F->v=(void*)
Cyc_Absyn_uint_typ,_tmp72F))));}bound=Cyc_Absyn_add_exp(bound,_tmp32E,0);{struct
Cyc_Core_Opt*_tmp730;bound->topt=((_tmp730=_cycalloc(sizeof(*_tmp730)),((_tmp730->v=(
void*)Cyc_Absyn_uint_typ,_tmp730))));}*is_zero_term=1;}{void*_tmp331=Cyc_Absyn_array_typ(
t,_tmp31E == 0?Cyc_Absyn_empty_tqual(0):*_tmp31E,(struct Cyc_Absyn_Exp*)bound,
_tmp31F == 0?Cyc_Absyn_false_conref:*_tmp31F,0);_npop_handler(0);return _tmp331;}}}};
_pop_region(r);}}struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*
f2;};static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*
args,struct Cyc_Absyn_Aggrdecl**ad_opt);static void*Cyc_Tcexp_tcStruct(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**
ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){struct Cyc_Absyn_Aggrdecl*
ad;if(*ad_opt != 0)ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);else{{
struct _handler_cons _tmp332;_push_handler(& _tmp332);{int _tmp334=0;if(setjmp(
_tmp332.handler))_tmp334=1;if(!_tmp334){ad=*Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);;
_pop_handler();}else{void*_tmp333=(void*)_exn_thrown;void*_tmp336=_tmp333;_LL1BD:
if(_tmp336 != Cyc_Dict_Absent)goto _LL1BF;_LL1BE:{const char*_tmp734;void*_tmp733[1];
struct Cyc_String_pa_struct _tmp732;(_tmp732.tag=0,((_tmp732.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp733[0]=&
_tmp732,Cyc_Tcutil_terr(loc,((_tmp734="unbound struct name %s",_tag_dyneither(
_tmp734,sizeof(char),23))),_tag_dyneither(_tmp733,sizeof(void*),1)))))));}return
topt != 0?*topt:(void*)0;_LL1BF:;_LL1C0:(void)_throw(_tmp336);_LL1BC:;}}}*ad_opt=(
struct Cyc_Absyn_Aggrdecl*)ad;*tn=ad->name;}if((void*)ad->kind == (void*)1){const
char*_tmp737;void*_tmp736;(_tmp736=0,Cyc_Tcutil_terr(loc,((_tmp737="expecting struct but found union",
_tag_dyneither(_tmp737,sizeof(char),33))),_tag_dyneither(_tmp736,sizeof(void*),0)));}
if(ad->impl == 0){{const char*_tmp73A;void*_tmp739;(_tmp739=0,Cyc_Tcutil_terr(loc,((
_tmp73A="can't build abstract struct",_tag_dyneither(_tmp73A,sizeof(char),28))),
_tag_dyneither(_tmp739,sizeof(void*),0)));}return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));}{struct _RegionHandle _tmp33E=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp33E;_push_region(rgn);{struct _tuple4 _tmp73B;struct _tuple4 _tmp33F=(_tmp73B.f1=
Cyc_Tcenv_lookup_type_vars(te),((_tmp73B.f2=rgn,_tmp73B)));struct Cyc_List_List*
_tmp340=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct
_tuple4*,struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp33F,ad->tvs);struct Cyc_List_List*_tmp341=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp33F,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars);struct Cyc_List_List*_tmp342=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp340);struct Cyc_List_List*_tmp343=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct
_tuple5*))Cyc_Core_snd,_tmp341);struct Cyc_List_List*_tmp344=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp340,_tmp341);struct Cyc_Absyn_AggrType_struct _tmp74A;union Cyc_Absyn_AggrInfoU_union
_tmp749;struct Cyc_Absyn_Aggrdecl**_tmp748;struct Cyc_Absyn_AggrInfo _tmp747;struct
Cyc_Absyn_AggrType_struct*_tmp746;struct Cyc_Absyn_AggrType_struct*_tmp345=(
_tmp746=_cycalloc(sizeof(*_tmp746)),((_tmp746[0]=((_tmp74A.tag=10,((_tmp74A.f1=((
_tmp747.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp749.KnownAggr).tag=1,(((
_tmp749.KnownAggr).f1=((_tmp748=_cycalloc(sizeof(*_tmp748)),((_tmp748[0]=ad,
_tmp748)))),_tmp749)))),((_tmp747.targs=_tmp342,_tmp747)))),_tmp74A)))),_tmp746)));
struct Cyc_List_List*_tmp346=*ts;struct Cyc_List_List*_tmp347=_tmp343;while(
_tmp346 != 0  && _tmp347 != 0){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)0,1,(void*)_tmp346->hd);Cyc_Tcutil_unify((void*)_tmp346->hd,(void*)
_tmp347->hd);_tmp346=_tmp346->tl;_tmp347=_tmp347->tl;}if(_tmp346 != 0){const char*
_tmp74D;void*_tmp74C;(_tmp74C=0,Cyc_Tcutil_terr(loc,((_tmp74D="too many explicit witness types",
_tag_dyneither(_tmp74D,sizeof(char),32))),_tag_dyneither(_tmp74C,sizeof(void*),0)));}*
ts=_tmp343;if(topt != 0)Cyc_Tcutil_unify((void*)_tmp345,*topt);{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp34B;struct Cyc_Absyn_Aggrfield*
_tmp34C;struct Cyc_Absyn_Exp*_tmp34D;struct _tuple10*_tmp34A=(struct _tuple10*)
fields->hd;_tmp34B=*_tmp34A;_tmp34C=_tmp34B.f1;_tmp34D=_tmp34B.f2;{void*_tmp34E=
Cyc_Tcutil_rsubstitute(rgn,_tmp344,(void*)_tmp34C->type);Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp34E,_tmp34D);if(!Cyc_Tcutil_coerce_arg(te,_tmp34D,_tmp34E)){{
const char*_tmp754;void*_tmp753[4];struct Cyc_String_pa_struct _tmp752;struct Cyc_String_pa_struct
_tmp751;struct Cyc_String_pa_struct _tmp750;struct Cyc_String_pa_struct _tmp74F;(
_tmp74F.tag=0,((_tmp74F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp34D->topt))->v)),((_tmp750.tag=0,((
_tmp750.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
_tmp34E)),((_tmp751.tag=0,((_tmp751.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp752.tag=0,((_tmp752.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp34C->name),((_tmp753[0]=&
_tmp752,((_tmp753[1]=& _tmp751,((_tmp753[2]=& _tmp750,((_tmp753[3]=& _tmp74F,Cyc_Tcutil_terr(
_tmp34D->loc,((_tmp754="field %s of struct %s expects type %s != %s",
_tag_dyneither(_tmp754,sizeof(char),44))),_tag_dyneither(_tmp753,sizeof(void*),4)))))))))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}}{struct Cyc_List_List*_tmp355=0;{struct Cyc_List_List*
_tmp356=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;
_tmp356 != 0;_tmp356=_tmp356->tl){struct _tuple0*_tmp757;struct Cyc_List_List*
_tmp756;_tmp355=((_tmp756=_cycalloc(sizeof(*_tmp756)),((_tmp756->hd=((_tmp757=
_cycalloc(sizeof(*_tmp757)),((_tmp757->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp344,(*((
struct _tuple0*)_tmp356->hd)).f1),((_tmp757->f2=Cyc_Tcutil_rsubstitute(rgn,
_tmp344,(*((struct _tuple0*)_tmp356->hd)).f2),_tmp757)))))),((_tmp756->tl=_tmp355,
_tmp756))))));}}_tmp355=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp355);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp355);{void*_tmp359=(void*)
_tmp345;_npop_handler(0);return _tmp359;}}}};_pop_region(rgn);}}static void*Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*ts,struct Cyc_List_List*
args);static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){{struct _RegionHandle _tmp360=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp360;_push_region(rgn);{void*_tmp361=Cyc_Tcutil_compress(
ts);void*_tmp362;struct Cyc_List_List*_tmp363;_LL1C2: if(_tmp361 <= (void*)4)goto
_LL1C4;if(*((int*)_tmp361)!= 11)goto _LL1C4;_tmp362=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp361)->f1;_tmp363=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp361)->f2;_LL1C3:
if(_tmp362 == (void*)1){const char*_tmp75A;void*_tmp759;(_tmp759=0,Cyc_Tcutil_terr(
loc,((_tmp75A="expecting struct but found union",_tag_dyneither(_tmp75A,sizeof(
char),33))),_tag_dyneither(_tmp759,sizeof(void*),0)));}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp363);for(0;fields != 0;fields=fields->tl){struct _tuple10 _tmp367;
struct Cyc_Absyn_Aggrfield*_tmp368;struct Cyc_Absyn_Exp*_tmp369;struct _tuple10*
_tmp366=(struct _tuple10*)fields->hd;_tmp367=*_tmp366;_tmp368=_tmp367.f1;_tmp369=
_tmp367.f2;Cyc_Tcexp_tcExpInitializer(te,(void**)((void**)((void*)& _tmp368->type)),
_tmp369);if(!Cyc_Tcutil_coerce_arg(te,_tmp369,(void*)_tmp368->type)){{const char*
_tmp760;void*_tmp75F[3];struct Cyc_String_pa_struct _tmp75E;struct Cyc_String_pa_struct
_tmp75D;struct Cyc_String_pa_struct _tmp75C;(_tmp75C.tag=0,((_tmp75C.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp369->topt))->v)),((_tmp75D.tag=0,((_tmp75D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmp368->type)),((
_tmp75E.tag=0,((_tmp75E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp368->name),((
_tmp75F[0]=& _tmp75E,((_tmp75F[1]=& _tmp75D,((_tmp75F[2]=& _tmp75C,Cyc_Tcutil_terr(
_tmp369->loc,((_tmp760="field %s of struct expects type %s != %s",_tag_dyneither(
_tmp760,sizeof(char),41))),_tag_dyneither(_tmp75F,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}goto _LL1C1;}_LL1C4:;_LL1C5: {const char*_tmp763;
void*_tmp762;(_tmp762=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp763="tcAnonStruct: wrong type",_tag_dyneither(_tmp763,
sizeof(char),25))),_tag_dyneither(_tmp762,sizeof(void*),0)));}_LL1C1:;};
_pop_region(rgn);}return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
es,struct Cyc_Absyn_Tuniondecl*tud,struct Cyc_Absyn_Tunionfield*tuf);static void*
Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*tud,
struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp371=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp371;_push_region(rgn);{struct _tuple4 _tmp764;struct
_tuple4 _tmp372=(_tmp764.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp764.f2=rgn,
_tmp764)));struct Cyc_List_List*_tmp373=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp372,tud->tvs);struct Cyc_List_List*_tmp374=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp373);struct Cyc_Absyn_TunionFieldType_struct _tmp76E;union Cyc_Absyn_TunionFieldInfoU_union
_tmp76D;struct Cyc_Absyn_TunionFieldInfo _tmp76C;struct Cyc_Absyn_TunionFieldType_struct*
_tmp76B;void*res=(void*)((_tmp76B=_cycalloc(sizeof(*_tmp76B)),((_tmp76B[0]=((
_tmp76E.tag=3,((_tmp76E.f1=((_tmp76C.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)(((
_tmp76D.KnownTunionfield).tag=1,(((_tmp76D.KnownTunionfield).f1=tud,(((_tmp76D.KnownTunionfield).f2=
tuf,_tmp76D)))))),((_tmp76C.targs=_tmp374,_tmp76C)))),_tmp76E)))),_tmp76B))));
if(topt != 0){void*_tmp375=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo
_tmp376;struct Cyc_List_List*_tmp377;struct Cyc_Core_Opt*_tmp378;_LL1C7: if(_tmp375
<= (void*)4)goto _LL1CB;if(*((int*)_tmp375)!= 3)goto _LL1C9;_LL1C8: Cyc_Tcutil_unify(*
topt,res);goto _LL1C6;_LL1C9: if(*((int*)_tmp375)!= 2)goto _LL1CB;_tmp376=((struct
Cyc_Absyn_TunionType_struct*)_tmp375)->f1;_tmp377=_tmp376.targs;_tmp378=_tmp376.rgn;
_LL1CA:{struct Cyc_List_List*a=_tmp374;for(0;a != 0  && _tmp377 != 0;(a=a->tl,
_tmp377=_tmp377->tl)){Cyc_Tcutil_unify((void*)a->hd,(void*)_tmp377->hd);}}if(tud->is_flat
 || tuf->typs == 0  && es == 0){{struct Cyc_Core_Opt*_tmp76F;e->topt=((_tmp76F=
_cycalloc(sizeof(*_tmp76F)),((_tmp76F->v=(void*)res,_tmp76F))));}{struct Cyc_Absyn_TunionType_struct
_tmp77E;union Cyc_Absyn_TunionInfoU_union _tmp77D;struct Cyc_Absyn_Tuniondecl**
_tmp77C;struct Cyc_Absyn_TunionInfo _tmp77B;struct Cyc_Absyn_TunionType_struct*
_tmp77A;res=(void*)((_tmp77A=_cycalloc(sizeof(*_tmp77A)),((_tmp77A[0]=((_tmp77E.tag=
2,((_tmp77E.f1=((_tmp77B.tunion_info=(union Cyc_Absyn_TunionInfoU_union)(((
_tmp77D.KnownTunion).tag=1,(((_tmp77D.KnownTunion).f1=((_tmp77C=_cycalloc(
sizeof(*_tmp77C)),((_tmp77C[0]=tud,_tmp77C)))),_tmp77D)))),((_tmp77B.targs=
_tmp374,((_tmp77B.rgn=_tmp378,_tmp77B)))))),_tmp77E)))),_tmp77A))));}Cyc_Tcutil_unchecked_cast(
te,e,res,(void*)1);}goto _LL1C6;_LL1CB:;_LL1CC: goto _LL1C6;_LL1C6:;}{struct Cyc_List_List*
ts=tuf->typs;for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){struct Cyc_Absyn_Exp*e=(
struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(rgn,_tmp373,(*((struct
_tuple9*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){{const char*_tmp786;const char*_tmp785;void*_tmp784[3];struct Cyc_String_pa_struct
_tmp783;struct Cyc_String_pa_struct _tmp782;struct Cyc_String_pa_struct _tmp781;(
_tmp781.tag=0,((_tmp781.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt
== 0?(struct _dyneither_ptr)((_tmp786="?",_tag_dyneither(_tmp786,sizeof(char),2))):
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v))),((
_tmp782.tag=0,((_tmp782.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp783.tag=0,((_tmp783.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp784[0]=& _tmp783,((_tmp784[1]=& _tmp782,((
_tmp784[2]=& _tmp781,Cyc_Tcutil_terr(e->loc,((_tmp785="tunion constructor %s expects argument of type %s but this argument has type %s",
_tag_dyneither(_tmp785,sizeof(char),80))),_tag_dyneither(_tmp784,sizeof(void*),3)))))))))))))))))));}
Cyc_Tcutil_explain_failure();}}if(es != 0){const char*_tmp78A;void*_tmp789[1];
struct Cyc_String_pa_struct _tmp788;void*_tmp388=(_tmp788.tag=0,((_tmp788.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp789[0]=& _tmp788,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,
topt,((_tmp78A="too many arguments for tunion constructor %s",_tag_dyneither(
_tmp78A,sizeof(char),45))),_tag_dyneither(_tmp789,sizeof(void*),1)))))));
_npop_handler(0);return _tmp388;}if(ts != 0){const char*_tmp78E;void*_tmp78D[1];
struct Cyc_String_pa_struct _tmp78C;void*_tmp38C=(_tmp78C.tag=0,((_tmp78C.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((
_tmp78D[0]=& _tmp78C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp78E="too few arguments for tunion constructor %s",
_tag_dyneither(_tmp78E,sizeof(char),44))),_tag_dyneither(_tmp78D,sizeof(void*),1)))))));
_npop_handler(0);return _tmp38C;}{void*_tmp38D=res;_npop_handler(0);return _tmp38D;}}};
_pop_region(rgn);}static int Cyc_Tcexp_zeroable_type(void*t);static int Cyc_Tcexp_zeroable_type(
void*t){void*_tmp393=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp394;
struct Cyc_Absyn_PtrAtts _tmp395;struct Cyc_Absyn_Conref*_tmp396;struct Cyc_Absyn_ArrayInfo
_tmp397;void*_tmp398;struct Cyc_List_List*_tmp399;struct Cyc_Absyn_AggrInfo _tmp39A;
union Cyc_Absyn_AggrInfoU_union _tmp39B;struct Cyc_List_List*_tmp39C;struct Cyc_List_List*
_tmp39D;_LL1CE: if((int)_tmp393 != 0)goto _LL1D0;_LL1CF: return 1;_LL1D0: if(_tmp393 <= (
void*)4)goto _LL1DC;if(*((int*)_tmp393)!= 0)goto _LL1D2;_LL1D1: goto _LL1D3;_LL1D2:
if(*((int*)_tmp393)!= 1)goto _LL1D4;_LL1D3: goto _LL1D5;_LL1D4: if(*((int*)_tmp393)
!= 2)goto _LL1D6;_LL1D5: goto _LL1D7;_LL1D6: if(*((int*)_tmp393)!= 3)goto _LL1D8;
_LL1D7: return 0;_LL1D8: if(*((int*)_tmp393)!= 4)goto _LL1DA;_tmp394=((struct Cyc_Absyn_PointerType_struct*)
_tmp393)->f1;_tmp395=_tmp394.ptr_atts;_tmp396=_tmp395.nullable;_LL1D9: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp396);_LL1DA: if(*((
int*)_tmp393)!= 5)goto _LL1DC;_LL1DB: goto _LL1DD;_LL1DC: if((int)_tmp393 != 1)goto
_LL1DE;_LL1DD: goto _LL1DF;_LL1DE: if(_tmp393 <= (void*)4)goto _LL1F8;if(*((int*)
_tmp393)!= 6)goto _LL1E0;_LL1DF: return 1;_LL1E0: if(*((int*)_tmp393)!= 7)goto _LL1E2;
_tmp397=((struct Cyc_Absyn_ArrayType_struct*)_tmp393)->f1;_tmp398=(void*)_tmp397.elt_type;
_LL1E1: return Cyc_Tcexp_zeroable_type(_tmp398);_LL1E2: if(*((int*)_tmp393)!= 8)
goto _LL1E4;_LL1E3: return 0;_LL1E4: if(*((int*)_tmp393)!= 9)goto _LL1E6;_tmp399=((
struct Cyc_Absyn_TupleType_struct*)_tmp393)->f1;_LL1E5: for(0;(unsigned int)
_tmp399;_tmp399=_tmp399->tl){if(!Cyc_Tcexp_zeroable_type((*((struct _tuple9*)
_tmp399->hd)).f2))return 0;}return 1;_LL1E6: if(*((int*)_tmp393)!= 10)goto _LL1E8;
_tmp39A=((struct Cyc_Absyn_AggrType_struct*)_tmp393)->f1;_tmp39B=_tmp39A.aggr_info;
_tmp39C=_tmp39A.targs;_LL1E7: {struct Cyc_Absyn_Aggrdecl*_tmp39E=Cyc_Absyn_get_known_aggrdecl(
_tmp39B);if(_tmp39E->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp39E->impl))->exist_vars != 0)return 0;{struct _RegionHandle _tmp39F=_new_region("r");
struct _RegionHandle*r=& _tmp39F;_push_region(r);{struct Cyc_List_List*_tmp3A0=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp39E->tvs,_tmp39C);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp39E->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp3A0,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3A1=0;_npop_handler(0);
return _tmp3A1;}}}{int _tmp3A2=1;_npop_handler(0);return _tmp3A2;}};_pop_region(r);}}
_LL1E8: if(*((int*)_tmp393)!= 12)goto _LL1EA;_LL1E9: return 1;_LL1EA: if(*((int*)
_tmp393)!= 18)goto _LL1EC;_LL1EB: return 1;_LL1EC: if(*((int*)_tmp393)!= 11)goto
_LL1EE;_tmp39D=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp393)->f2;_LL1ED: for(0;
_tmp39D != 0;_tmp39D=_tmp39D->tl){if(!Cyc_Tcexp_zeroable_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp39D->hd)->type))return 0;}return 1;_LL1EE: if(*((int*)_tmp393)!= 13)goto _LL1F0;
_LL1EF: return 1;_LL1F0: if(*((int*)_tmp393)!= 16)goto _LL1F2;_LL1F1: return 0;_LL1F2:
if(*((int*)_tmp393)!= 15)goto _LL1F4;_LL1F3: return 0;_LL1F4: if(*((int*)_tmp393)!= 
14)goto _LL1F6;_LL1F5: return 0;_LL1F6: if(*((int*)_tmp393)!= 17)goto _LL1F8;_LL1F7:
goto _LL1F9;_LL1F8: if((int)_tmp393 != 2)goto _LL1FA;_LL1F9: goto _LL1FB;_LL1FA: if((
int)_tmp393 != 3)goto _LL1FC;_LL1FB: goto _LL1FD;_LL1FC: if(_tmp393 <= (void*)4)goto
_LL1FE;if(*((int*)_tmp393)!= 19)goto _LL1FE;_LL1FD: goto _LL1FF;_LL1FE: if(_tmp393 <= (
void*)4)goto _LL200;if(*((int*)_tmp393)!= 20)goto _LL200;_LL1FF: goto _LL201;_LL200:
if(_tmp393 <= (void*)4)goto _LL1CD;if(*((int*)_tmp393)!= 21)goto _LL1CD;_LL201: {
const char*_tmp792;void*_tmp791[1];struct Cyc_String_pa_struct _tmp790;(_tmp790.tag=
0,((_tmp790.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp791[0]=& _tmp790,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp792="bad type `%s' in zeroable type",_tag_dyneither(
_tmp792,sizeof(char),31))),_tag_dyneither(_tmp791,sizeof(void*),1)))))));}_LL1CD:;}
static void Cyc_Tcexp_check_malloc_type(int allow_zero,struct Cyc_Position_Segment*
loc,void**topt,void*t);static void Cyc_Tcexp_check_malloc_type(int allow_zero,
struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(t) || 
allow_zero  && Cyc_Tcexp_zeroable_type(t))return;if(topt != 0){void*_tmp3A6=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp3A7;void*_tmp3A8;_LL203: if(_tmp3A6 <= (void*)4)
goto _LL205;if(*((int*)_tmp3A6)!= 4)goto _LL205;_tmp3A7=((struct Cyc_Absyn_PointerType_struct*)
_tmp3A6)->f1;_tmp3A8=(void*)_tmp3A7.elt_typ;_LL204: Cyc_Tcutil_unify(_tmp3A8,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL202;_LL205:;_LL206: goto _LL202;_LL202:;}{const char*_tmp79B;const char*
_tmp79A;const char*_tmp799;void*_tmp798[2];struct Cyc_String_pa_struct _tmp797;
struct Cyc_String_pa_struct _tmp796;(_tmp796.tag=0,((_tmp796.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp797.tag=0,((
_tmp797.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct
_dyneither_ptr)((_tmp79A="calloc",_tag_dyneither(_tmp79A,sizeof(char),7))):(
struct _dyneither_ptr)((_tmp79B="malloc",_tag_dyneither(_tmp79B,sizeof(char),7))))),((
_tmp798[0]=& _tmp797,((_tmp798[1]=& _tmp796,Cyc_Tcutil_terr(loc,((_tmp799="%s cannot be used with type %s\n\t(type needs initialization)",
_tag_dyneither(_tmp799,sizeof(char),60))),_tag_dyneither(_tmp798,sizeof(void*),2)))))))))))));}}
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,struct Cyc_Absyn_Exp**e,int*
is_calloc,int*is_fat);static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*ropt,void***t,
struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){void*rgn=(void*)2;if(ropt != 0){
struct Cyc_Absyn_RgnHandleType_struct _tmp79E;struct Cyc_Absyn_RgnHandleType_struct*
_tmp79D;void*expected_type=(void*)((_tmp79D=_cycalloc(sizeof(*_tmp79D)),((
_tmp79D[0]=((_tmp79E.tag=14,((_tmp79E.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp79E)))),_tmp79D))));void*
handle_type=Cyc_Tcexp_tcExp(te,(void**)& expected_type,(struct Cyc_Absyn_Exp*)ropt);
void*_tmp3AF=Cyc_Tcutil_compress(handle_type);void*_tmp3B0;_LL208: if(_tmp3AF <= (
void*)4)goto _LL20A;if(*((int*)_tmp3AF)!= 14)goto _LL20A;_tmp3B0=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp3AF)->f1;_LL209: rgn=_tmp3B0;Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);goto _LL207;_LL20A:;_LL20B:{const char*_tmp7A2;void*_tmp7A1[1];struct
Cyc_String_pa_struct _tmp7A0;(_tmp7A0.tag=0,((_tmp7A0.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type)),((_tmp7A1[0]=& _tmp7A0,
Cyc_Tcutil_terr(ropt->loc,((_tmp7A2="expecting region_t type but found %s",
_tag_dyneither(_tmp7A2,sizeof(char),37))),_tag_dyneither(_tmp7A1,sizeof(void*),1)))))));}
goto _LL207;_LL207:;}Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_typ,*e);{void*
elt_type;struct Cyc_Absyn_Exp*num_elts;int one_elt;if(*is_calloc){if(*t == 0){const
char*_tmp7A5;void*_tmp7A4;(_tmp7A4=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7A5="calloc with empty type",
_tag_dyneither(_tmp7A5,sizeof(char),23))),_tag_dyneither(_tmp7A4,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)1,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;one_elt=0;}else{void*_tmp3B8=(void*)(*e)->r;void*_tmp3B9;void*
_tmp3BA;struct Cyc_List_List*_tmp3BB;struct Cyc_List_List _tmp3BC;struct Cyc_Absyn_Exp*
_tmp3BD;struct Cyc_List_List*_tmp3BE;struct Cyc_List_List _tmp3BF;struct Cyc_Absyn_Exp*
_tmp3C0;struct Cyc_List_List*_tmp3C1;_LL20D: if(*((int*)_tmp3B8)!= 18)goto _LL20F;
_tmp3B9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3B8)->f1;_LL20E:
elt_type=_tmp3B9;{void**_tmp7A6;*t=(void**)((_tmp7A6=_cycalloc(sizeof(*_tmp7A6)),((
_tmp7A6[0]=elt_type,_tmp7A6))));}num_elts=Cyc_Absyn_uint_exp(1,0);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_typ,num_elts);one_elt=1;goto _LL20C;_LL20F: if(*((int*)
_tmp3B8)!= 3)goto _LL211;_tmp3BA=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B8)->f1;if((int)_tmp3BA != 1)goto _LL211;_tmp3BB=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B8)->f2;if(_tmp3BB == 0)goto _LL211;_tmp3BC=*_tmp3BB;_tmp3BD=(struct Cyc_Absyn_Exp*)
_tmp3BC.hd;_tmp3BE=_tmp3BC.tl;if(_tmp3BE == 0)goto _LL211;_tmp3BF=*_tmp3BE;_tmp3C0=(
struct Cyc_Absyn_Exp*)_tmp3BF.hd;_tmp3C1=_tmp3BF.tl;if(_tmp3C1 != 0)goto _LL211;
_LL210:{struct _tuple0 _tmp7A7;struct _tuple0 _tmp3C4=(_tmp7A7.f1=(void*)_tmp3BD->r,((
_tmp7A7.f2=(void*)_tmp3C0->r,_tmp7A7)));void*_tmp3C5;void*_tmp3C6;void*_tmp3C7;
void*_tmp3C8;_LL214: _tmp3C5=_tmp3C4.f1;if(*((int*)_tmp3C5)!= 18)goto _LL216;
_tmp3C6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C5)->f1;_LL215: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp3C6);elt_type=_tmp3C6;{void**_tmp7A8;*t=(void**)((_tmp7A8=
_cycalloc(sizeof(*_tmp7A8)),((_tmp7A8[0]=elt_type,_tmp7A8))));}num_elts=_tmp3C0;
one_elt=0;goto _LL213;_LL216: _tmp3C7=_tmp3C4.f2;if(*((int*)_tmp3C7)!= 18)goto
_LL218;_tmp3C8=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C7)->f1;_LL217:
Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3C8);elt_type=_tmp3C8;{void**_tmp7A9;*
t=(void**)((_tmp7A9=_cycalloc(sizeof(*_tmp7A9)),((_tmp7A9[0]=elt_type,_tmp7A9))));}
num_elts=_tmp3BD;one_elt=0;goto _LL213;_LL218:;_LL219: goto No_sizeof;_LL213:;}goto
_LL20C;_LL211:;_LL212: No_sizeof: elt_type=Cyc_Absyn_char_typ;{void**_tmp7AA;*t=(
void**)((_tmp7AA=_cycalloc(sizeof(*_tmp7AA)),((_tmp7AA[0]=elt_type,_tmp7AA))));}
num_elts=*e;one_elt=0;goto _LL20C;_LL20C:;}*e=num_elts;*is_fat=!one_elt;{void*
_tmp3CC=elt_type;struct Cyc_Absyn_AggrInfo _tmp3CD;union Cyc_Absyn_AggrInfoU_union
_tmp3CE;struct Cyc_Absyn_Aggrdecl**_tmp3CF;struct Cyc_Absyn_Aggrdecl*_tmp3D0;
_LL21B: if(_tmp3CC <= (void*)4)goto _LL21D;if(*((int*)_tmp3CC)!= 10)goto _LL21D;
_tmp3CD=((struct Cyc_Absyn_AggrType_struct*)_tmp3CC)->f1;_tmp3CE=_tmp3CD.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp3CC)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL21D;_tmp3CF=(_tmp3CE.KnownAggr).f1;_tmp3D0=*_tmp3CF;_LL21C: if(
_tmp3D0->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3D0->impl))->exist_vars
!= 0){const char*_tmp7AD;void*_tmp7AC;(_tmp7AC=0,Cyc_Tcutil_terr(loc,((_tmp7AD="malloc with existential types not yet implemented",
_tag_dyneither(_tmp7AD,sizeof(char),50))),_tag_dyneither(_tmp7AC,sizeof(void*),0)));}
goto _LL21A;_LL21D:;_LL21E: goto _LL21A;_LL21A:;}{void*(*_tmp3D3)(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term)=Cyc_Absyn_at_typ;
struct Cyc_Absyn_Conref*_tmp3D4=Cyc_Absyn_false_conref;if(topt != 0){void*_tmp3D5=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp3D6;struct Cyc_Absyn_PtrAtts
_tmp3D7;struct Cyc_Absyn_Conref*_tmp3D8;struct Cyc_Absyn_Conref*_tmp3D9;_LL220: if(
_tmp3D5 <= (void*)4)goto _LL222;if(*((int*)_tmp3D5)!= 4)goto _LL222;_tmp3D6=((
struct Cyc_Absyn_PointerType_struct*)_tmp3D5)->f1;_tmp3D7=_tmp3D6.ptr_atts;
_tmp3D8=_tmp3D7.nullable;_tmp3D9=_tmp3D7.zero_term;_LL221: _tmp3D4=_tmp3D9;if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp3D8))_tmp3D3=
Cyc_Absyn_star_typ;if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp3D9) && !(*is_calloc)){{const char*_tmp7B0;void*_tmp7AF;(_tmp7AF=0,Cyc_Tcutil_warn(
loc,((_tmp7B0="converting malloc to calloc to ensure zero-termination",
_tag_dyneither(_tmp7B0,sizeof(char),55))),_tag_dyneither(_tmp7AF,sizeof(void*),0)));}*
is_calloc=1;}goto _LL21F;_LL222:;_LL223: goto _LL21F;_LL21F:;}if(!one_elt)_tmp3D3=
Cyc_Absyn_dyneither_typ;return _tmp3D3(elt_type,rgn,Cyc_Absyn_empty_tqual(0),
_tmp3D4);}}}static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Tcexp_tcSwap(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((
void**)((void*)&((struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v;{void*_tmp3DC=Cyc_Tcutil_compress(t1);_LL225: if(
_tmp3DC <= (void*)4)goto _LL227;if(*((int*)_tmp3DC)!= 7)goto _LL227;_LL226:{const
char*_tmp7B3;void*_tmp7B2;(_tmp7B2=0,Cyc_Tcutil_terr(loc,((_tmp7B3="cannot assign to an array",
_tag_dyneither(_tmp7B3,sizeof(char),26))),_tag_dyneither(_tmp7B2,sizeof(void*),0)));}
goto _LL224;_LL227:;_LL228: goto _LL224;_LL224:;}{int ign_1=0;if(!Cyc_Tcutil_is_pointer_or_boxed(
t1,& ign_1)){const char*_tmp7B6;void*_tmp7B5;(_tmp7B5=0,Cyc_Tcutil_terr(loc,((
_tmp7B6="Swap not allowed for non-pointer or non-word-sized types.",
_tag_dyneither(_tmp7B6,sizeof(char),58))),_tag_dyneither(_tmp7B5,sizeof(void*),0)));}
if(!Cyc_Absyn_is_lvalue(e1)){const char*_tmp7B9;void*_tmp7B8;return(_tmp7B8=0,Cyc_Tcexp_expr_err(
te,e1->loc,topt,((_tmp7B9="swap non-lvalue",_tag_dyneither(_tmp7B9,sizeof(char),
16))),_tag_dyneither(_tmp7B8,sizeof(void*),0)));}if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp7BC;void*_tmp7BB;return(_tmp7BB=0,Cyc_Tcexp_expr_err(te,e2->loc,
topt,((_tmp7BC="swap non-lvalue",_tag_dyneither(_tmp7BC,sizeof(char),16))),
_tag_dyneither(_tmp7BB,sizeof(void*),0)));}{void*t_ign1=(void*)0;void*t_ign2=(
void*)0;int b_ign1=0;if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp7BF;void*_tmp7BE;return(_tmp7BE=0,Cyc_Tcexp_expr_err(te,e1->loc,
topt,((_tmp7BF="swap value in zeroterm array",_tag_dyneither(_tmp7BF,sizeof(char),
29))),_tag_dyneither(_tmp7BE,sizeof(void*),0)));}if(Cyc_Tcutil_is_zero_ptr_deref(
e2,& t_ign1,& b_ign1,& t_ign2)){const char*_tmp7C2;void*_tmp7C1;return(_tmp7C1=0,Cyc_Tcexp_expr_err(
te,e2->loc,topt,((_tmp7C2="swap value in zeroterm array",_tag_dyneither(_tmp7C2,
sizeof(char),29))),_tag_dyneither(_tmp7C1,sizeof(void*),0)));}Cyc_Tcexp_check_writable(
te,e1);Cyc_Tcexp_check_writable(te,e2);if(!Cyc_Tcutil_unify(t1,t2)){const char*
_tmp7C7;void*_tmp7C6[2];struct Cyc_String_pa_struct _tmp7C5;struct Cyc_String_pa_struct
_tmp7C4;void*_tmp3E9=(_tmp7C4.tag=0,((_tmp7C4.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp7C5.tag=0,((_tmp7C5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7C6[0]=&
_tmp7C5,((_tmp7C6[1]=& _tmp7C4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C7="type mismatch: %s != %s",
_tag_dyneither(_tmp7C7,sizeof(char),24))),_tag_dyneither(_tmp7C6,sizeof(void*),2)))))))))))));
return _tmp3E9;}return(void*)0;}}}}int Cyc_Tcexp_in_stmt_exp=0;static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s);static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Stmt*s){{struct _RegionHandle _tmp3EE=_new_region("r");
struct _RegionHandle*r=& _tmp3EE;_push_region(r);{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,te,s),s,1);Cyc_Tcexp_in_stmt_exp=
old_stmt_exp_state;};_pop_region(r);}Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));while(1){void*_tmp3EF=(void*)s->r;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_Absyn_Stmt*
_tmp3F1;struct Cyc_Absyn_Stmt*_tmp3F2;struct Cyc_Absyn_Decl*_tmp3F3;struct Cyc_Absyn_Stmt*
_tmp3F4;_LL22A: if(_tmp3EF <= (void*)1)goto _LL230;if(*((int*)_tmp3EF)!= 0)goto
_LL22C;_tmp3F0=((struct Cyc_Absyn_Exp_s_struct*)_tmp3EF)->f1;_LL22B: return(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v;_LL22C: if(*((int*)_tmp3EF)!= 
1)goto _LL22E;_tmp3F1=((struct Cyc_Absyn_Seq_s_struct*)_tmp3EF)->f1;_tmp3F2=((
struct Cyc_Absyn_Seq_s_struct*)_tmp3EF)->f2;_LL22D: s=_tmp3F2;continue;_LL22E: if(*((
int*)_tmp3EF)!= 11)goto _LL230;_tmp3F3=((struct Cyc_Absyn_Decl_s_struct*)_tmp3EF)->f1;
_tmp3F4=((struct Cyc_Absyn_Decl_s_struct*)_tmp3EF)->f2;_LL22F: s=_tmp3F4;continue;
_LL230:;_LL231: {const char*_tmp7CA;void*_tmp7C9;return(_tmp7C9=0,Cyc_Tcexp_expr_err(
te,loc,topt,((_tmp7CA="statement expression must end with expression",
_tag_dyneither(_tmp7CA,sizeof(char),46))),_tag_dyneither(_tmp7C9,sizeof(void*),0)));}
_LL229:;}}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1);static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)2;if(rgn_handle != 0){struct Cyc_Absyn_RgnHandleType_struct
_tmp7CD;struct Cyc_Absyn_RgnHandleType_struct*_tmp7CC;void*expected_type=(void*)((
_tmp7CC=_cycalloc(sizeof(*_tmp7CC)),((_tmp7CC[0]=((_tmp7CD.tag=14,((_tmp7CD.f1=(
void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(
te)),_tmp7CD)))),_tmp7CC))));void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)rgn_handle);void*_tmp3F7=Cyc_Tcutil_compress(
handle_type);void*_tmp3F8;_LL233: if(_tmp3F7 <= (void*)4)goto _LL235;if(*((int*)
_tmp3F7)!= 14)goto _LL235;_tmp3F8=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp3F7)->f1;_LL234: rgn=_tmp3F8;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto
_LL232;_LL235:;_LL236:{const char*_tmp7D1;void*_tmp7D0[1];struct Cyc_String_pa_struct
_tmp7CF;(_tmp7CF.tag=0,((_tmp7CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp7D0[0]=& _tmp7CF,Cyc_Tcutil_terr(
rgn_handle->loc,((_tmp7D1="expecting region_t type but found %s",_tag_dyneither(
_tmp7D1,sizeof(char),37))),_tag_dyneither(_tmp7D0,sizeof(void*),1)))))));}goto
_LL232;_LL232:;}{void*_tmp3FE=(void*)e1->r;struct Cyc_Core_Opt*_tmp3FF;struct Cyc_List_List*
_tmp400;struct Cyc_List_List*_tmp401;union Cyc_Absyn_Cnst_union _tmp402;struct
_dyneither_ptr _tmp403;_LL238: if(*((int*)_tmp3FE)!= 29)goto _LL23A;_LL239: {void*
_tmp404=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*_tmp405=Cyc_Tcutil_compress(
_tmp404);struct Cyc_Absyn_ArrayInfo _tmp406;void*_tmp407;struct Cyc_Absyn_Tqual
_tmp408;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Conref*_tmp40A;_LL243: if(
_tmp405 <= (void*)4)goto _LL245;if(*((int*)_tmp405)!= 7)goto _LL245;_tmp406=((
struct Cyc_Absyn_ArrayType_struct*)_tmp405)->f1;_tmp407=(void*)_tmp406.elt_type;
_tmp408=_tmp406.tq;_tmp409=_tmp406.num_elts;_tmp40A=_tmp406.zero_term;_LL244: {
struct Cyc_Absyn_Exp*bnd=(struct Cyc_Absyn_Exp*)_check_null(_tmp409);void*b;{void*
_tmp40B=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(bnd->topt))->v);
_LL248:;_LL249: if(Cyc_Tcutil_is_const_exp(te,bnd)){struct Cyc_Absyn_Upper_b_struct
_tmp7D4;struct Cyc_Absyn_Upper_b_struct*_tmp7D3;b=(void*)((_tmp7D3=_cycalloc(
sizeof(*_tmp7D3)),((_tmp7D3[0]=((_tmp7D4.tag=0,((_tmp7D4.f1=bnd,_tmp7D4)))),
_tmp7D3))));}else{b=(void*)0;}_LL247:;}{struct Cyc_Absyn_PointerType_struct
_tmp7DE;struct Cyc_Absyn_PtrAtts _tmp7DD;struct Cyc_Absyn_PtrInfo _tmp7DC;struct Cyc_Absyn_PointerType_struct*
_tmp7DB;void*res_typ=(void*)((_tmp7DB=_cycalloc(sizeof(*_tmp7DB)),((_tmp7DB[0]=((
_tmp7DE.tag=4,((_tmp7DE.f1=((_tmp7DC.elt_typ=(void*)_tmp407,((_tmp7DC.elt_tq=
_tmp408,((_tmp7DC.ptr_atts=((_tmp7DD.rgn=(void*)rgn,((_tmp7DD.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp7DD.bounds=Cyc_Absyn_new_conref(
b),((_tmp7DD.zero_term=_tmp40A,((_tmp7DD.ptrloc=0,_tmp7DD)))))))))),_tmp7DC)))))),
_tmp7DE)))),_tmp7DB))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp7DF;e->topt=((_tmp7DF=_cycalloc(
sizeof(*_tmp7DF)),((_tmp7DF->v=(void*)res_typ,_tmp7DF))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL245:;_LL246: {const char*
_tmp7E2;void*_tmp7E1;(_tmp7E1=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7E2="tcNew: comprehension returned non-array type",
_tag_dyneither(_tmp7E2,sizeof(char),45))),_tag_dyneither(_tmp7E1,sizeof(void*),0)));}
_LL242:;}_LL23A: if(*((int*)_tmp3FE)!= 37)goto _LL23C;_tmp3FF=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp3FE)->f1;_tmp400=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp3FE)->f2;
_LL23B:{struct Cyc_Absyn_Array_e_struct _tmp7E5;struct Cyc_Absyn_Array_e_struct*
_tmp7E4;(void*)(e1->r=(void*)((void*)((_tmp7E4=_cycalloc(sizeof(*_tmp7E4)),((
_tmp7E4[0]=((_tmp7E5.tag=28,((_tmp7E5.f1=_tmp400,_tmp7E5)))),_tmp7E4))))));}
_tmp401=_tmp400;goto _LL23D;_LL23C: if(*((int*)_tmp3FE)!= 28)goto _LL23E;_tmp401=((
struct Cyc_Absyn_Array_e_struct*)_tmp3FE)->f1;_LL23D: {void**elt_typ_opt=0;int
zero_term=0;if(topt != 0){void*_tmp417=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp418;void*_tmp419;void**_tmp41A;struct Cyc_Absyn_Tqual _tmp41B;struct Cyc_Absyn_PtrAtts
_tmp41C;struct Cyc_Absyn_Conref*_tmp41D;_LL24B: if(_tmp417 <= (void*)4)goto _LL24D;
if(*((int*)_tmp417)!= 4)goto _LL24D;_tmp418=((struct Cyc_Absyn_PointerType_struct*)
_tmp417)->f1;_tmp419=(void*)_tmp418.elt_typ;_tmp41A=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp417)->f1).elt_typ;_tmp41B=_tmp418.elt_tq;_tmp41C=_tmp418.ptr_atts;_tmp41D=
_tmp41C.zero_term;_LL24C: elt_typ_opt=(void**)_tmp41A;zero_term=((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp41D);goto _LL24A;_LL24D:;
_LL24E: goto _LL24A;_LL24A:;}{void*_tmp41E=Cyc_Tcexp_tcArray(te,e1->loc,
elt_typ_opt,zero_term,_tmp401);{struct Cyc_Core_Opt*_tmp7E6;e1->topt=((_tmp7E6=
_cycalloc(sizeof(*_tmp7E6)),((_tmp7E6->v=(void*)_tmp41E,_tmp7E6))));}{void*
res_typ;{void*_tmp420=Cyc_Tcutil_compress(_tmp41E);struct Cyc_Absyn_ArrayInfo
_tmp421;void*_tmp422;struct Cyc_Absyn_Tqual _tmp423;struct Cyc_Absyn_Exp*_tmp424;
struct Cyc_Absyn_Conref*_tmp425;_LL250: if(_tmp420 <= (void*)4)goto _LL252;if(*((int*)
_tmp420)!= 7)goto _LL252;_tmp421=((struct Cyc_Absyn_ArrayType_struct*)_tmp420)->f1;
_tmp422=(void*)_tmp421.elt_type;_tmp423=_tmp421.tq;_tmp424=_tmp421.num_elts;
_tmp425=_tmp421.zero_term;_LL251:{struct Cyc_Absyn_PointerType_struct _tmp7FB;
struct Cyc_Absyn_PtrAtts _tmp7FA;struct Cyc_Absyn_Upper_b_struct _tmp7F9;struct Cyc_Absyn_Upper_b_struct*
_tmp7F8;struct Cyc_Absyn_PtrInfo _tmp7F7;struct Cyc_Absyn_PointerType_struct*
_tmp7F6;res_typ=(void*)((_tmp7F6=_cycalloc(sizeof(*_tmp7F6)),((_tmp7F6[0]=((
_tmp7FB.tag=4,((_tmp7FB.f1=((_tmp7F7.elt_typ=(void*)_tmp422,((_tmp7F7.elt_tq=
_tmp423,((_tmp7F7.ptr_atts=((_tmp7FA.rgn=(void*)rgn,((_tmp7FA.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((_tmp7FA.bounds=Cyc_Absyn_new_conref((
void*)((_tmp7F8=_cycalloc(sizeof(*_tmp7F8)),((_tmp7F8[0]=((_tmp7F9.tag=0,((
_tmp7F9.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp424),_tmp7F9)))),_tmp7F8))))),((
_tmp7FA.zero_term=_tmp425,((_tmp7FA.ptrloc=0,_tmp7FA)))))))))),_tmp7F7)))))),
_tmp7FB)))),_tmp7F6))));}if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp7FC;e->topt=((_tmp7FC=_cycalloc(
sizeof(*_tmp7FC)),((_tmp7FC->v=(void*)res_typ,_tmp7FC))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}goto _LL24F;_LL252:;_LL253: {const char*
_tmp7FF;void*_tmp7FE;(_tmp7FE=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7FF="tcExpNoPromote on Array_e returned non-array type",
_tag_dyneither(_tmp7FF,sizeof(char),50))),_tag_dyneither(_tmp7FE,sizeof(void*),0)));}
_LL24F:;}return res_typ;}}}_LL23E: if(*((int*)_tmp3FE)!= 0)goto _LL240;_tmp402=((
struct Cyc_Absyn_Const_e_struct*)_tmp3FE)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp3FE)->f1).String_c).tag != 5)goto _LL240;_tmp403=(_tmp402.String_c).f1;_LL23F: {
void*_tmp42F=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(
void*)0,Cyc_Absyn_true_conref);void*_tmp430=Cyc_Tcexp_tcExp(te,(void**)& _tmp42F,
e1);struct Cyc_Absyn_Upper_b_struct _tmp802;struct Cyc_Absyn_Upper_b_struct*_tmp801;
return Cyc_Absyn_atb_typ(_tmp430,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmp801=
_cycalloc(sizeof(*_tmp801)),((_tmp801[0]=((_tmp802.tag=0,((_tmp802.f1=Cyc_Absyn_uint_exp(
1,0),_tmp802)))),_tmp801)))),Cyc_Absyn_false_conref);}_LL240:;_LL241: {void**
topt2=0;if(topt != 0){void*_tmp433=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo
_tmp434;void*_tmp435;void**_tmp436;struct Cyc_Absyn_Tqual _tmp437;_LL255: if(
_tmp433 <= (void*)4)goto _LL259;if(*((int*)_tmp433)!= 4)goto _LL257;_tmp434=((
struct Cyc_Absyn_PointerType_struct*)_tmp433)->f1;_tmp435=(void*)_tmp434.elt_typ;
_tmp436=(void**)&(((struct Cyc_Absyn_PointerType_struct*)_tmp433)->f1).elt_typ;
_tmp437=_tmp434.elt_tq;_LL256: topt2=(void**)_tmp436;goto _LL254;_LL257: if(*((int*)
_tmp433)!= 2)goto _LL259;_LL258:{void**_tmp803;topt2=((_tmp803=_cycalloc(sizeof(*
_tmp803)),((_tmp803[0]=*topt,_tmp803))));}goto _LL254;_LL259:;_LL25A: goto _LL254;
_LL254:;}{void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);struct Cyc_Absyn_PointerType_struct
_tmp80D;struct Cyc_Absyn_PtrAtts _tmp80C;struct Cyc_Absyn_PtrInfo _tmp80B;struct Cyc_Absyn_PointerType_struct*
_tmp80A;void*res_typ=(void*)((_tmp80A=_cycalloc(sizeof(*_tmp80A)),((_tmp80A[0]=((
_tmp80D.tag=4,((_tmp80D.f1=((_tmp80B.elt_typ=(void*)telt,((_tmp80B.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmp80B.ptr_atts=((_tmp80C.rgn=(void*)rgn,((_tmp80C.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(),((_tmp80C.bounds=Cyc_Absyn_bounds_one_conref,((_tmp80C.zero_term=
Cyc_Absyn_false_conref,((_tmp80C.ptrloc=0,_tmp80C)))))))))),_tmp80B)))))),
_tmp80D)))),_tmp80A))));if(topt != 0){if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt)){{struct Cyc_Core_Opt*_tmp80E;e->topt=((_tmp80E=_cycalloc(
sizeof(*_tmp80E)),((_tmp80E->v=(void*)res_typ,_tmp80E))));}Cyc_Tcutil_unchecked_cast(
te,e,*topt,(void*)3);res_typ=*topt;}}return res_typ;}}_LL237:;}}void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp43E=t;struct Cyc_Absyn_ArrayInfo
_tmp43F;void*_tmp440;struct Cyc_Absyn_Tqual _tmp441;struct Cyc_Absyn_Exp*_tmp442;
struct Cyc_Absyn_Conref*_tmp443;_LL25C: if(_tmp43E <= (void*)4)goto _LL25E;if(*((int*)
_tmp43E)!= 7)goto _LL25E;_tmp43F=((struct Cyc_Absyn_ArrayType_struct*)_tmp43E)->f1;
_tmp440=(void*)_tmp43F.elt_type;_tmp441=_tmp43F.tq;_tmp442=_tmp43F.num_elts;
_tmp443=_tmp43F.zero_term;_LL25D: {void*_tmp445;struct _tuple6 _tmp444=Cyc_Tcutil_addressof_props(
te,e);_tmp445=_tmp444.f2;{struct Cyc_Absyn_Upper_b_struct _tmp811;struct Cyc_Absyn_Upper_b_struct*
_tmp810;void*_tmp446=_tmp442 == 0?(void*)((void*)0):(void*)((_tmp810=_cycalloc(
sizeof(*_tmp810)),((_tmp810[0]=((_tmp811.tag=0,((_tmp811.f1=(struct Cyc_Absyn_Exp*)
_tmp442,_tmp811)))),_tmp810))));t=Cyc_Absyn_atb_typ(_tmp440,_tmp445,_tmp441,
_tmp446,_tmp443);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)t);
return t;}}_LL25E:;_LL25F: return t;_LL25B:;}void*Cyc_Tcexp_tcExpInitializer(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(
e)){{const char*_tmp819;const char*_tmp818;void*_tmp817[3];struct Cyc_String_pa_struct
_tmp816;struct Cyc_String_pa_struct _tmp815;struct Cyc_String_pa_struct _tmp814;(
_tmp814.tag=0,((_tmp814.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp815.tag=0,((_tmp815.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((
_tmp816.tag=0,((_tmp816.f1=(struct _dyneither_ptr)(topt == 0?(_tmp819="NULL",
_tag_dyneither(_tmp819,sizeof(char),5)):(struct _dyneither_ptr)Cyc_Absynpp_typ2string(*
topt)),((_tmp817[0]=& _tmp816,((_tmp817[1]=& _tmp815,((_tmp817[2]=& _tmp814,Cyc_fprintf(
Cyc_stderr,((_tmp818="topt=%s, e->topt->v=%s, e=%s\n",_tag_dyneither(_tmp818,
sizeof(char),30))),_tag_dyneither(_tmp817,sizeof(void*),3)))))))))))))))))));}{
const char*_tmp81C;void*_tmp81B;(_tmp81B=0,Cyc_Tcutil_terr(e->loc,((_tmp81C="Cannot consume non-unique paths; do swap instead",
_tag_dyneither(_tmp81C,sizeof(char),49))),_tag_dyneither(_tmp81B,sizeof(void*),0)));}}{
void*_tmp451=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp452;_LL261: if(*((int*)
_tmp451)!= 28)goto _LL263;_LL262: goto _LL264;_LL263: if(*((int*)_tmp451)!= 29)goto
_LL265;_LL264: goto _LL266;_LL265: if(*((int*)_tmp451)!= 0)goto _LL267;_tmp452=((
struct Cyc_Absyn_Const_e_struct*)_tmp451)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp451)->f1).String_c).tag != 5)goto _LL267;_LL266: return t;_LL267:;_LL268: t=Cyc_Tcutil_compress(
t);{void*_tmp453=t;struct Cyc_Absyn_ArrayInfo _tmp454;void*_tmp455;struct Cyc_Absyn_Tqual
_tmp456;struct Cyc_Absyn_Exp*_tmp457;struct Cyc_Absyn_Conref*_tmp458;_LL26A: if(
_tmp453 <= (void*)4)goto _LL26C;if(*((int*)_tmp453)!= 7)goto _LL26C;_tmp454=((
struct Cyc_Absyn_ArrayType_struct*)_tmp453)->f1;_tmp455=(void*)_tmp454.elt_type;
_tmp456=_tmp454.tq;_tmp457=_tmp454.num_elts;_tmp458=_tmp454.zero_term;_LL26B: {
void*_tmp45A;struct _tuple6 _tmp459=Cyc_Tcutil_addressof_props(te,e);_tmp45A=
_tmp459.f2;{struct Cyc_Absyn_Upper_b_struct _tmp81F;struct Cyc_Absyn_Upper_b_struct*
_tmp81E;void*b=_tmp457 == 0?(void*)((void*)0):(void*)((_tmp81E=_cycalloc(sizeof(*
_tmp81E)),((_tmp81E[0]=((_tmp81F.tag=0,((_tmp81F.f1=(struct Cyc_Absyn_Exp*)
_tmp457,_tmp81F)))),_tmp81E))));t=Cyc_Absyn_atb_typ(_tmp455,_tmp45A,_tmp456,b,
_tmp458);Cyc_Tcutil_unchecked_cast(te,e,t,(void*)3);return t;}}_LL26C:;_LL26D:
return t;_LL269:;}_LL260:;}}static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(struct
Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp45D=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp45E;_LL26F: if(*((int*)_tmp45D)!= 13)goto _LL271;_tmp45E=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp45D)->f1;_LL270: Cyc_Tcexp_tcExpNoInst(
te,topt,_tmp45E);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp45E->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp45E->topt))->v,
0));e->topt=_tmp45E->topt;goto _LL26E;_LL271:;_LL272: Cyc_Tcexp_tcExpNoInst(te,
topt,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0));{void*_tmp45F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp460;void*_tmp461;struct Cyc_Absyn_Tqual _tmp462;struct Cyc_Absyn_PtrAtts _tmp463;
void*_tmp464;struct Cyc_Absyn_Conref*_tmp465;struct Cyc_Absyn_Conref*_tmp466;
struct Cyc_Absyn_Conref*_tmp467;_LL274: if(_tmp45F <= (void*)4)goto _LL276;if(*((int*)
_tmp45F)!= 4)goto _LL276;_tmp460=((struct Cyc_Absyn_PointerType_struct*)_tmp45F)->f1;
_tmp461=(void*)_tmp460.elt_typ;_tmp462=_tmp460.elt_tq;_tmp463=_tmp460.ptr_atts;
_tmp464=(void*)_tmp463.rgn;_tmp465=_tmp463.nullable;_tmp466=_tmp463.bounds;
_tmp467=_tmp463.zero_term;_LL275:{void*_tmp468=Cyc_Tcutil_compress(_tmp461);
struct Cyc_Absyn_FnInfo _tmp469;struct Cyc_List_List*_tmp46A;struct Cyc_Core_Opt*
_tmp46B;void*_tmp46C;struct Cyc_List_List*_tmp46D;int _tmp46E;struct Cyc_Absyn_VarargInfo*
_tmp46F;struct Cyc_List_List*_tmp470;struct Cyc_List_List*_tmp471;_LL279: if(
_tmp468 <= (void*)4)goto _LL27B;if(*((int*)_tmp468)!= 8)goto _LL27B;_tmp469=((
struct Cyc_Absyn_FnType_struct*)_tmp468)->f1;_tmp46A=_tmp469.tvars;_tmp46B=
_tmp469.effect;_tmp46C=(void*)_tmp469.ret_typ;_tmp46D=_tmp469.args;_tmp46E=
_tmp469.c_varargs;_tmp46F=_tmp469.cyc_varargs;_tmp470=_tmp469.rgn_po;_tmp471=
_tmp469.attributes;_LL27A: if(_tmp46A != 0){struct _RegionHandle _tmp472=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp472;_push_region(rgn);{struct _tuple4 _tmp820;struct
_tuple4 _tmp473=(_tmp820.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp820.f2=rgn,
_tmp820)));struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp473,_tmp46A);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);struct Cyc_Absyn_FnType_struct _tmp826;struct Cyc_Absyn_FnInfo
_tmp825;struct Cyc_Absyn_FnType_struct*_tmp824;void*ftyp=Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)((_tmp824=_cycalloc(sizeof(*_tmp824)),((_tmp824[0]=((_tmp826.tag=
8,((_tmp826.f1=((_tmp825.tvars=0,((_tmp825.effect=_tmp46B,((_tmp825.ret_typ=(
void*)_tmp46C,((_tmp825.args=_tmp46D,((_tmp825.c_varargs=_tmp46E,((_tmp825.cyc_varargs=
_tmp46F,((_tmp825.rgn_po=_tmp470,((_tmp825.attributes=_tmp471,_tmp825)))))))))))))))),
_tmp826)))),_tmp824)))));struct Cyc_Absyn_PointerType_struct _tmp830;struct Cyc_Absyn_PtrAtts
_tmp82F;struct Cyc_Absyn_PtrInfo _tmp82E;struct Cyc_Absyn_PointerType_struct*
_tmp82D;struct Cyc_Absyn_PointerType_struct*_tmp474=(_tmp82D=_cycalloc(sizeof(*
_tmp82D)),((_tmp82D[0]=((_tmp830.tag=4,((_tmp830.f1=((_tmp82E.elt_typ=(void*)
ftyp,((_tmp82E.elt_tq=_tmp462,((_tmp82E.ptr_atts=((_tmp82F.rgn=(void*)_tmp464,((
_tmp82F.nullable=_tmp465,((_tmp82F.bounds=_tmp466,((_tmp82F.zero_term=_tmp467,((
_tmp82F.ptrloc=0,_tmp82F)))))))))),_tmp82E)))))),_tmp830)))),_tmp82D)));struct
Cyc_Absyn_Exp*_tmp475=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Instantiate_e_struct
_tmp833;struct Cyc_Absyn_Instantiate_e_struct*_tmp832;(void*)(e->r=(void*)((void*)((
_tmp832=_cycalloc(sizeof(*_tmp832)),((_tmp832[0]=((_tmp833.tag=14,((_tmp833.f1=
_tmp475,((_tmp833.f2=ts,_tmp833)))))),_tmp832))))));}{struct Cyc_Core_Opt*_tmp834;
e->topt=((_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834->v=(void*)((void*)
_tmp474),_tmp834))));}};_pop_region(rgn);}goto _LL278;_LL27B:;_LL27C: goto _LL278;
_LL278:;}goto _LL273;_LL276:;_LL277: goto _LL273;_LL273:;}goto _LL26E;_LL26E:;}
return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpNoInst(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*
loc=e->loc;void*t;{void*_tmp481=(void*)e->r;struct Cyc_Absyn_Exp*_tmp482;struct
_tuple1*_tmp483;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_List_List*_tmp485;struct
Cyc_Core_Opt*_tmp486;struct Cyc_List_List*_tmp487;union Cyc_Absyn_Cnst_union
_tmp488;union Cyc_Absyn_Cnst_union*_tmp489;struct _tuple1*_tmp48A;void*_tmp48B;
void*_tmp48C;struct Cyc_List_List*_tmp48D;struct Cyc_Absyn_Exp*_tmp48E;void*
_tmp48F;struct Cyc_Absyn_Exp*_tmp490;struct Cyc_Core_Opt*_tmp491;struct Cyc_Absyn_Exp*
_tmp492;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*
_tmp495;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_Absyn_Exp*
_tmp498;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*
_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_List_List*_tmp49D;struct Cyc_Absyn_VarargCallInfo*
_tmp49E;struct Cyc_Absyn_VarargCallInfo**_tmp49F;struct Cyc_Absyn_Exp*_tmp4A0;
struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_List_List*_tmp4A2;void*_tmp4A3;struct Cyc_Absyn_Exp*
_tmp4A4;void*_tmp4A5;void**_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A7;struct Cyc_Absyn_Exp*
_tmp4A8;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4AA;void*_tmp4AB;
void*_tmp4AC;void*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AE;struct Cyc_Absyn_Exp*
_tmp4AF;struct _dyneither_ptr*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct
_dyneither_ptr*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B3;struct Cyc_Absyn_Exp*_tmp4B4;
struct Cyc_List_List*_tmp4B5;struct _tuple2*_tmp4B6;struct Cyc_List_List*_tmp4B7;
struct Cyc_List_List*_tmp4B8;struct Cyc_Absyn_Stmt*_tmp4B9;struct Cyc_Absyn_Vardecl*
_tmp4BA;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BC;int _tmp4BD;int*
_tmp4BE;struct _tuple1*_tmp4BF;struct _tuple1**_tmp4C0;struct Cyc_List_List*_tmp4C1;
struct Cyc_List_List**_tmp4C2;struct Cyc_List_List*_tmp4C3;struct Cyc_Absyn_Aggrdecl*
_tmp4C4;struct Cyc_Absyn_Aggrdecl**_tmp4C5;void*_tmp4C6;struct Cyc_List_List*
_tmp4C7;struct Cyc_List_List*_tmp4C8;struct Cyc_Absyn_Tuniondecl*_tmp4C9;struct Cyc_Absyn_Tunionfield*
_tmp4CA;struct _tuple1*_tmp4CB;struct _tuple1**_tmp4CC;struct Cyc_Absyn_Enumdecl*
_tmp4CD;struct Cyc_Absyn_Enumfield*_tmp4CE;struct _tuple1*_tmp4CF;struct _tuple1**
_tmp4D0;void*_tmp4D1;struct Cyc_Absyn_Enumfield*_tmp4D2;struct Cyc_Absyn_MallocInfo
_tmp4D3;int _tmp4D4;int*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D6;void**_tmp4D7;void***
_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp**_tmp4DA;int _tmp4DB;int*
_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DE;void*_tmp4DF;
_LL27E: if(*((int*)_tmp481)!= 13)goto _LL280;_tmp482=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp481)->f1;_LL27F: Cyc_Tcexp_tcExpNoInst(te,0,_tmp482);return;_LL280: if(*((int*)
_tmp481)!= 2)goto _LL282;_tmp483=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp481)->f1;
_LL281: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp483);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL282: if(*((int*)_tmp481)!= 10)goto _LL284;_tmp484=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp481)->f1;_tmp485=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp481)->f2;_LL283:
Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp484,_tmp485);Cyc_Tcexp_tcExpNoInst(te,topt,
e);return;_LL284: if(*((int*)_tmp481)!= 37)goto _LL286;_tmp486=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp481)->f1;_tmp487=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp481)->f2;
_LL285: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp487);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL286: if(*((int*)_tmp481)!= 0)goto _LL288;_tmp488=((struct Cyc_Absyn_Const_e_struct*)
_tmp481)->f1;_tmp489=(union Cyc_Absyn_Cnst_union*)&((struct Cyc_Absyn_Const_e_struct*)
_tmp481)->f1;_LL287: t=Cyc_Tcexp_tcConst(te,loc,topt,(union Cyc_Absyn_Cnst_union*)
_tmp489,e);goto _LL27D;_LL288: if(*((int*)_tmp481)!= 1)goto _LL28A;_tmp48A=((struct
Cyc_Absyn_Var_e_struct*)_tmp481)->f1;_tmp48B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp481)->f2;_LL289: t=Cyc_Tcexp_tcVar(te,loc,_tmp48A,_tmp48B);goto _LL27D;_LL28A:
if(*((int*)_tmp481)!= 3)goto _LL28C;_tmp48C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp481)->f1;_tmp48D=((struct Cyc_Absyn_Primop_e_struct*)_tmp481)->f2;_LL28B: t=
Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp48C,_tmp48D);goto _LL27D;_LL28C: if(*((int*)
_tmp481)!= 5)goto _LL28E;_tmp48E=((struct Cyc_Absyn_Increment_e_struct*)_tmp481)->f1;
_tmp48F=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp481)->f2;_LL28D: t=Cyc_Tcexp_tcIncrement(
te,loc,topt,_tmp48E,_tmp48F);goto _LL27D;_LL28E: if(*((int*)_tmp481)!= 4)goto
_LL290;_tmp490=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp481)->f1;_tmp491=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp481)->f2;_tmp492=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp481)->f3;_LL28F: t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp490,_tmp491,_tmp492);
goto _LL27D;_LL290: if(*((int*)_tmp481)!= 6)goto _LL292;_tmp493=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp481)->f1;_tmp494=((struct Cyc_Absyn_Conditional_e_struct*)_tmp481)->f2;
_tmp495=((struct Cyc_Absyn_Conditional_e_struct*)_tmp481)->f3;_LL291: t=Cyc_Tcexp_tcConditional(
te,loc,topt,_tmp493,_tmp494,_tmp495);goto _LL27D;_LL292: if(*((int*)_tmp481)!= 7)
goto _LL294;_tmp496=((struct Cyc_Absyn_And_e_struct*)_tmp481)->f1;_tmp497=((struct
Cyc_Absyn_And_e_struct*)_tmp481)->f2;_LL293: t=Cyc_Tcexp_tcAnd(te,loc,_tmp496,
_tmp497);goto _LL27D;_LL294: if(*((int*)_tmp481)!= 8)goto _LL296;_tmp498=((struct
Cyc_Absyn_Or_e_struct*)_tmp481)->f1;_tmp499=((struct Cyc_Absyn_Or_e_struct*)
_tmp481)->f2;_LL295: t=Cyc_Tcexp_tcOr(te,loc,_tmp498,_tmp499);goto _LL27D;_LL296:
if(*((int*)_tmp481)!= 9)goto _LL298;_tmp49A=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp481)->f1;_tmp49B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp481)->f2;_LL297: t=
Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp49A,_tmp49B);goto _LL27D;_LL298: if(*((int*)
_tmp481)!= 11)goto _LL29A;_tmp49C=((struct Cyc_Absyn_FnCall_e_struct*)_tmp481)->f1;
_tmp49D=((struct Cyc_Absyn_FnCall_e_struct*)_tmp481)->f2;_tmp49E=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp481)->f3;_tmp49F=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp481)->f3;_LL299: t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp49C,_tmp49D,_tmp49F);
goto _LL27D;_LL29A: if(*((int*)_tmp481)!= 12)goto _LL29C;_tmp4A0=((struct Cyc_Absyn_Throw_e_struct*)
_tmp481)->f1;_LL29B: t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4A0);goto _LL27D;_LL29C:
if(*((int*)_tmp481)!= 14)goto _LL29E;_tmp4A1=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp481)->f1;_tmp4A2=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp481)->f2;_LL29D:
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4A1,_tmp4A2);goto _LL27D;_LL29E: if(*((
int*)_tmp481)!= 15)goto _LL2A0;_tmp4A3=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp481)->f1;_tmp4A4=((struct Cyc_Absyn_Cast_e_struct*)_tmp481)->f2;_tmp4A5=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp481)->f4;_tmp4A6=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp481)->f4);_LL29F: t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4A3,_tmp4A4,(void**)
_tmp4A6);goto _LL27D;_LL2A0: if(*((int*)_tmp481)!= 16)goto _LL2A2;_tmp4A7=((struct
Cyc_Absyn_Address_e_struct*)_tmp481)->f1;_LL2A1: t=Cyc_Tcexp_tcAddress(te,loc,e,
topt,_tmp4A7);goto _LL27D;_LL2A2: if(*((int*)_tmp481)!= 17)goto _LL2A4;_tmp4A8=((
struct Cyc_Absyn_New_e_struct*)_tmp481)->f1;_tmp4A9=((struct Cyc_Absyn_New_e_struct*)
_tmp481)->f2;_LL2A3: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4A8,e,_tmp4A9);goto _LL27D;
_LL2A4: if(*((int*)_tmp481)!= 19)goto _LL2A6;_tmp4AA=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp481)->f1;_LL2A5: {void*_tmp4E0=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4AA);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4E0);goto _LL27D;}_LL2A6: if(*((int*)_tmp481)!= 18)goto _LL2A8;
_tmp4AB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp481)->f1;_LL2A7: t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp4AB);goto _LL27D;_LL2A8: if(*((int*)_tmp481)!= 20)goto _LL2AA;
_tmp4AC=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp481)->f1;_tmp4AD=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp481)->f2;_LL2A9: t=Cyc_Tcexp_tcOffsetof(te,
loc,topt,_tmp4AC,_tmp4AD);goto _LL27D;_LL2AA: if(*((int*)_tmp481)!= 21)goto _LL2AC;
_LL2AB:{const char*_tmp837;void*_tmp836;(_tmp836=0,Cyc_Tcutil_terr(loc,((_tmp837="gen() not in top-level initializer",
_tag_dyneither(_tmp837,sizeof(char),35))),_tag_dyneither(_tmp836,sizeof(void*),0)));}
return;_LL2AC: if(*((int*)_tmp481)!= 22)goto _LL2AE;_tmp4AE=((struct Cyc_Absyn_Deref_e_struct*)
_tmp481)->f1;_LL2AD: t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4AE);goto _LL27D;_LL2AE:
if(*((int*)_tmp481)!= 23)goto _LL2B0;_tmp4AF=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp481)->f1;_tmp4B0=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp481)->f2;_LL2AF:
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4AF,_tmp4B0);goto _LL27D;_LL2B0: if(*((
int*)_tmp481)!= 24)goto _LL2B2;_tmp4B1=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp481)->f1;_tmp4B2=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp481)->f2;_LL2B1: t=
Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4B1,_tmp4B2);goto _LL27D;_LL2B2: if(*((int*)
_tmp481)!= 25)goto _LL2B4;_tmp4B3=((struct Cyc_Absyn_Subscript_e_struct*)_tmp481)->f1;
_tmp4B4=((struct Cyc_Absyn_Subscript_e_struct*)_tmp481)->f2;_LL2B3: t=Cyc_Tcexp_tcSubscript(
te,loc,topt,_tmp4B3,_tmp4B4);goto _LL27D;_LL2B4: if(*((int*)_tmp481)!= 26)goto
_LL2B6;_tmp4B5=((struct Cyc_Absyn_Tuple_e_struct*)_tmp481)->f1;_LL2B5: t=Cyc_Tcexp_tcTuple(
te,loc,topt,_tmp4B5);goto _LL27D;_LL2B6: if(*((int*)_tmp481)!= 27)goto _LL2B8;
_tmp4B6=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp481)->f1;_tmp4B7=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp481)->f2;_LL2B7: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4B6,_tmp4B7);goto
_LL27D;_LL2B8: if(*((int*)_tmp481)!= 28)goto _LL2BA;_tmp4B8=((struct Cyc_Absyn_Array_e_struct*)
_tmp481)->f1;_LL2B9: {void**elt_topt=0;int zero_term=0;if(topt != 0){void*_tmp4E3=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_ArrayInfo _tmp4E4;void*_tmp4E5;void**
_tmp4E6;struct Cyc_Absyn_Conref*_tmp4E7;_LL2CF: if(_tmp4E3 <= (void*)4)goto _LL2D1;
if(*((int*)_tmp4E3)!= 7)goto _LL2D1;_tmp4E4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4E3)->f1;_tmp4E5=(void*)_tmp4E4.elt_type;_tmp4E6=(void**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp4E3)->f1).elt_type;_tmp4E7=_tmp4E4.zero_term;_LL2D0: elt_topt=(void**)_tmp4E6;
zero_term=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp4E7);
goto _LL2CE;_LL2D1:;_LL2D2: goto _LL2CE;_LL2CE:;}t=Cyc_Tcexp_tcArray(te,loc,
elt_topt,zero_term,_tmp4B8);goto _LL27D;}_LL2BA: if(*((int*)_tmp481)!= 38)goto
_LL2BC;_tmp4B9=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp481)->f1;_LL2BB: t=Cyc_Tcexp_tcStmtExp(
te,loc,topt,_tmp4B9);goto _LL27D;_LL2BC: if(*((int*)_tmp481)!= 29)goto _LL2BE;
_tmp4BA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp481)->f1;_tmp4BB=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp481)->f2;_tmp4BC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp481)->f3;_tmp4BD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp481)->f4;
_tmp4BE=(int*)&((struct Cyc_Absyn_Comprehension_e_struct*)_tmp481)->f4;_LL2BD: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4BA,_tmp4BB,_tmp4BC,_tmp4BE);goto
_LL27D;_LL2BE: if(*((int*)_tmp481)!= 30)goto _LL2C0;_tmp4BF=((struct Cyc_Absyn_Struct_e_struct*)
_tmp481)->f1;_tmp4C0=(struct _tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp481)->f1;_tmp4C1=((struct Cyc_Absyn_Struct_e_struct*)_tmp481)->f2;_tmp4C2=(
struct Cyc_List_List**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp481)->f2;_tmp4C3=((
struct Cyc_Absyn_Struct_e_struct*)_tmp481)->f3;_tmp4C4=((struct Cyc_Absyn_Struct_e_struct*)
_tmp481)->f4;_tmp4C5=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Struct_e_struct*)
_tmp481)->f4;_LL2BF: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp4C0,_tmp4C2,_tmp4C3,
_tmp4C5);goto _LL27D;_LL2C0: if(*((int*)_tmp481)!= 31)goto _LL2C2;_tmp4C6=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp481)->f1;_tmp4C7=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp481)->f2;_LL2C1: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4C6,_tmp4C7);goto _LL27D;
_LL2C2: if(*((int*)_tmp481)!= 32)goto _LL2C4;_tmp4C8=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp481)->f1;_tmp4C9=((struct Cyc_Absyn_Tunion_e_struct*)_tmp481)->f2;_tmp4CA=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp481)->f3;_LL2C3: t=Cyc_Tcexp_tcTunion(te,loc,
topt,e,_tmp4C8,_tmp4C9,_tmp4CA);goto _LL27D;_LL2C4: if(*((int*)_tmp481)!= 33)goto
_LL2C6;_tmp4CB=((struct Cyc_Absyn_Enum_e_struct*)_tmp481)->f1;_tmp4CC=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp481)->f1;_tmp4CD=((struct Cyc_Absyn_Enum_e_struct*)
_tmp481)->f2;_tmp4CE=((struct Cyc_Absyn_Enum_e_struct*)_tmp481)->f3;_LL2C5:*
_tmp4CC=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp4CE))->name;{struct Cyc_Absyn_EnumType_struct
_tmp83A;struct Cyc_Absyn_EnumType_struct*_tmp839;t=(void*)((_tmp839=_cycalloc(
sizeof(*_tmp839)),((_tmp839[0]=((_tmp83A.tag=12,((_tmp83A.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp4CD))->name,((_tmp83A.f2=_tmp4CD,_tmp83A)))))),_tmp839))));}goto
_LL27D;_LL2C6: if(*((int*)_tmp481)!= 34)goto _LL2C8;_tmp4CF=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp481)->f1;_tmp4D0=(struct _tuple1**)&((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp481)->f1;_tmp4D1=(void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp481)->f2;
_tmp4D2=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp481)->f3;_LL2C7:*_tmp4D0=((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp4D2))->name;t=_tmp4D1;goto _LL27D;
_LL2C8: if(*((int*)_tmp481)!= 35)goto _LL2CA;_tmp4D3=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp481)->f1;_tmp4D4=_tmp4D3.is_calloc;_tmp4D5=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp481)->f1).is_calloc;_tmp4D6=_tmp4D3.rgn;_tmp4D7=_tmp4D3.elt_type;_tmp4D8=(
void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp481)->f1).elt_type;_tmp4D9=
_tmp4D3.num_elts;_tmp4DA=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp481)->f1).num_elts;_tmp4DB=_tmp4D3.fat_result;_tmp4DC=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp481)->f1).fat_result;_LL2C9: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4D6,_tmp4D8,
_tmp4DA,_tmp4D5,_tmp4DC);goto _LL27D;_LL2CA: if(*((int*)_tmp481)!= 36)goto _LL2CC;
_tmp4DD=((struct Cyc_Absyn_Swap_e_struct*)_tmp481)->f1;_tmp4DE=((struct Cyc_Absyn_Swap_e_struct*)
_tmp481)->f2;_LL2CB: t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4DD,_tmp4DE);goto _LL27D;
_LL2CC: if(*((int*)_tmp481)!= 39)goto _LL27D;_tmp4DF=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmp481)->f1;_LL2CD: if(!te->allow_valueof){const char*_tmp83D;void*_tmp83C;(
_tmp83C=0,Cyc_Tcutil_terr(e->loc,((_tmp83D="valueof(-) can only occur within types",
_tag_dyneither(_tmp83D,sizeof(char),39))),_tag_dyneither(_tmp83C,sizeof(void*),0)));}
t=Cyc_Absyn_sint_typ;goto _LL27D;_LL27D:;}{struct Cyc_Core_Opt*_tmp83E;e->topt=((
_tmp83E=_cycalloc(sizeof(*_tmp83E)),((_tmp83E->v=(void*)t,_tmp83E))));}}