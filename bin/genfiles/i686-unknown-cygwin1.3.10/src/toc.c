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
void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion Cyc_Core_new_dynregion();
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];void Cyc_Core_free_dynregion(
struct _DynRegionHandle*);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;
struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct
_dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct Cyc___cycFILE*,
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};int Cyc_vfprintf(struct Cyc___cycFILE*,struct
_dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[19];extern char
Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);int
Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*
cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,
struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,
void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct
_dyneither_ptr elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple1{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct _union_Constraint_Eq_constr{int tag;void*val;};struct
_union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct
_union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr
Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_struct{
int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;
};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;};struct
Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*
ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};
struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*
bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};
struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{
struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct
_tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{void*f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct
_union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple4{void*f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};
struct _tuple5{void*f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5
val;};struct _tuple6{void*f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6
val;};struct _tuple7{void*f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;
struct _tuple7 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};
struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(void*,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(void*,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(
void*,int);union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(void*,long long);union Cyc_Absyn_Cnst
Cyc_Absyn_Float_c(struct _dyneither_ptr);struct Cyc_Absyn_VarargCallInfo{int
num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
Cyc_Absyn_Primop_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_Absyn_Conditional_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;
};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple8{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple8*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};
struct Cyc_Absyn_Swap_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{int tag;struct Cyc_Core_Opt*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Tagcheck_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct
Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};
struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int
tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};
struct Cyc_Absyn_Datatypedecl{void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*
Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(struct
_tuple1*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,
struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(
void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _dyneither_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_tvar_kind(struct
Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*
t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_or_boxed(void*
t,int*is_dyneither_ptr);int Cyc_Tcutil_unify(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple10*);struct _tuple11{
unsigned int f1;int f2;};struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;
struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{int tag;
int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;
};struct Cyc_CfFlowInfo_UniquePlace{struct Cyc_CfFlowInfo_Place place;struct Cyc_List_List*
path;};struct _union_RelnOp_EqualConst{int tag;unsigned int val;};struct _tuple12{
struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_RelnOp_LessVar{int tag;struct
_tuple12 val;};struct _union_RelnOp_LessNumelts{int tag;struct Cyc_Absyn_Vardecl*val;
};struct _union_RelnOp_LessConst{int tag;unsigned int val;};struct
_union_RelnOp_LessEqNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};union Cyc_CfFlowInfo_RelnOp{
struct _union_RelnOp_EqualConst EqualConst;struct _union_RelnOp_LessVar LessVar;
struct _union_RelnOp_LessNumelts LessNumelts;struct _union_RelnOp_LessConst
LessConst;struct _union_RelnOp_LessEqNumelts LessEqNumelts;};struct Cyc_CfFlowInfo_Reln{
struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp rop;};struct Cyc_CfFlowInfo_TagCmp{
void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[15];struct Cyc_CfFlowInfo_HasTagCmps_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_IsZero[11];extern char
Cyc_CfFlowInfo_NotZero[12];struct Cyc_CfFlowInfo_NotZero_struct{char*tag;struct
Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*
val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{
struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct
Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct
_union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};struct
_union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{
struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**datatypes_so_far;
struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple14{
struct _tuple1*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple14*
x,struct _tuple14*y){struct _tuple1*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple14
_tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple1*_tmp4;struct _dyneither_ptr
_tmp5;struct _tuple14 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(
_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(
struct _dyneither_ptr)_tmp5);}}}struct _tuple15{struct Cyc_Absyn_Aggrdecl*f1;void*
f2;};void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState
_tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;
_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct _RegionHandle*d=
_open_dynregion(& _tmp9,_tmp7);{struct _tuple15**v=((struct _tuple15**(*)(struct Cyc_Dict_Dict
d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8,q);if(v == 0){void*_tmpA=
type_maker(q);_npop_handler(0);return _tmpA;}else{struct _tuple15 _tmpC;void*_tmpD;
struct _tuple15*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{void*_tmpE=_tmpD;
_npop_handler(0);return _tmpE;}}};_pop_dynregion(d);}}static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmpF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp10="\n";_tag_dyneither(
_tmp10,sizeof(char),2);}),_tag_dyneither(_tmpF,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);({void*_tmp11=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp12="\n";
_tag_dyneither(_tmp12,sizeof(char),2);}),_tag_dyneither(_tmp11,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}
char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";static char _tmp13[5]="curr";
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp13,_tmp13,_tmp13 + 5};static
struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp14[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp14,_tmp14,_tmp14 + 4};static
struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp15[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp15,_tmp15,_tmp15 + 4};static
struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp16[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp16,_tmp16,_tmp16 + 14};
static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp17[8]="handler";static struct _dyneither_ptr Cyc_Toc_handler_string={
_tmp17,_tmp17,_tmp17 + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp18[14]="_RegionHandle";static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={
_tmp18,_tmp18,_tmp18 + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string;static char _tmp19[17]="_DynRegionHandle";static
struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp19,_tmp19,_tmp19 + 17};
static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmp1A[16]="_DynRegionFrame";static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={
_tmp1A,_tmp1A,_tmp1A + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=&
Cyc_Toc__DynRegionFrame_string;struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(
void*)0,(void*)(0 + 0)};static char _tmp1B[7]="_throw";static struct _dyneither_ptr
Cyc_Toc__throw_str={_tmp1B,_tmp1B,_tmp1B + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={
3,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={
1,& Cyc_Toc__throw_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)& Cyc_Toc__throw_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp1D[7]="setjmp";static struct
_dyneither_ptr Cyc_Toc_setjmp_str={_tmp1D,_tmp1D,_tmp1D + 7};static struct _tuple1
Cyc_Toc_setjmp_pr={{.Loc_n={3,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp1F[
14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp1F,
_tmp1F,_tmp1F + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={3,0}},&
Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={
1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={
0,(void*)& Cyc_Toc__push_handler_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp21[
13]="_pop_handler";static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp21,
_tmp21,_tmp21 + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={3,0}},&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp23[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp23,_tmp23,_tmp23 + 12};
static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={3,0}},& Cyc_Toc__exn_thrown_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp25[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp25,_tmp25,_tmp25 + 14};static struct
_tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={3,0}},& Cyc_Toc__npop_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)&
Cyc_Toc__npop_handler_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp27[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp27,_tmp27,_tmp27 + 12};
static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={3,0}},& Cyc_Toc__check_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp29[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp29,_tmp29,
_tmp29 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={
3,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp2B[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp2B,_tmp2B,_tmp2B + 31};static
struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={3,0}},& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp2D[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp2D,_tmp2D,
_tmp2D + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={3,
0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp2F[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp2F,_tmp2F,_tmp2F + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char
_tmp31[15]="_tag_dyneither";static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={
_tmp31,_tmp31,_tmp31 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={
3,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char
_tmp33[20]="_init_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp33,_tmp33,_tmp33 + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={
3,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={
1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)
Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=&
Cyc_Toc__init_dyneither_ptr_ev;static char _tmp35[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp35,_tmp35,_tmp35
+ 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={3,0}},& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)Cyc_Absyn_EmptyAnnot};static
struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp37[20]="_get_dyneither_size";static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={
_tmp37,_tmp37,_tmp37 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={
1,& Cyc_Toc__get_dyneither_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)
Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=&
Cyc_Toc__get_dyneither_size_ev;static char _tmp39[19]="_get_zero_arr_size";static
struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp39,_tmp39,_tmp39 + 19};
static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={3,0}},& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static
char _tmp3B[24]="_get_zero_arr_size_char";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp3B,_tmp3B,_tmp3B + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)&
Cyc_Toc__get_zero_arr_size_char_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp3D[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp3D,_tmp3D,_tmp3D + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp3F[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp3F,_tmp3F,_tmp3F + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={
1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(
void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=&
Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp41[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp41,_tmp41,
_tmp41 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={3,0}},&
Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={
1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=&
Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp43[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp43,_tmp43,
_tmp43 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={3,
0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp45[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp45,_tmp45,_tmp45 + 30};static
struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={3,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp47[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp47,_tmp47,
_tmp47 + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp49[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp49,_tmp49,_tmp49 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={
1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)
Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=&
Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp4B[15]="_zero_arr_plus";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp4B,_tmp4B,_tmp4B + 15};static
struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)&
Cyc_Toc__zero_arr_plus_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp4D[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp4D,_tmp4D,_tmp4D + 
20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(
void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)Cyc_Absyn_EmptyAnnot};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;
static char _tmp4F[21]="_zero_arr_plus_short";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={
_tmp4F,_tmp4F,_tmp4F + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={
1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(
void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=&
Cyc_Toc__zero_arr_plus_short_ev;static char _tmp51[19]="_zero_arr_plus_int";static
struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp51,_tmp51,_tmp51 + 19};
static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_plus_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(
void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static
char _tmp53[21]="_zero_arr_plus_float";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={
_tmp53,_tmp53,_tmp53 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={
1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(
void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=&
Cyc_Toc__zero_arr_plus_float_ev;static char _tmp55[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp55,_tmp55,
_tmp55 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={
1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(
void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=&
Cyc_Toc__zero_arr_plus_double_ev;static char _tmp57[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp57,_tmp57,
_tmp57 + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp59[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp59,_tmp59,_tmp59 + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)&
Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp5B[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp5B,_tmp5B,_tmp5B + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp5D[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp5D,_tmp5D,_tmp5D + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={
1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(
void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=&
Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp5F[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp5F,_tmp5F,
_tmp5F + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp61[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp61,_tmp61,_tmp61 + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,&
Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp63[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp63,_tmp63,
_tmp63 + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp65[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp65,_tmp65,_tmp65 + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={3,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,&
Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp67[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp67,
_tmp67,_tmp67 + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;
static char _tmp69[34]="_zero_arr_inplace_plus_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp69,_tmp69,
_tmp69 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;
static char _tmp6B[32]="_zero_arr_inplace_plus_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp6B,_tmp6B,_tmp6B + 
32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;
static char _tmp6D[33]="_dyneither_ptr_inplace_plus_post";static struct
_dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp6D,_tmp6D,_tmp6D
+ 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={3,
0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;
static char _tmp6F[28]="_zero_arr_inplace_plus_post";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={
_tmp6F,_tmp6F,_tmp6F + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp71[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp71,_tmp71,_tmp71
+ 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={3,
0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;
static char _tmp73[34]="_zero_arr_inplace_plus_post_short";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp73,_tmp73,
_tmp73 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;
static char _tmp75[32]="_zero_arr_inplace_plus_post_int";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp75,_tmp75,_tmp75 + 
32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={3,0}},&
Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;
static char _tmp77[34]="_zero_arr_inplace_plus_post_float";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp77,_tmp77,
_tmp77 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;
static char _tmp79[35]="_zero_arr_inplace_plus_post_double";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp79,_tmp79,
_tmp79 + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmp7B[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp7B,_tmp7B,
_tmp7B + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmp7D[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp7D,_tmp7D,
_tmp7D + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={
3,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={
0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmp7F[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmp7F,_tmp7F,_tmp7F + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={3,0}},&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp81[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp81,_tmp81,_tmp81 + 11};
static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={3,0}},& Cyc_Toc__cyccalloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmp83[17]="_cycalloc_atomic";static struct
_dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp83,_tmp83,_tmp83 + 17};static
struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={3,0}},& Cyc_Toc__cycalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)&
Cyc_Toc__cycalloc_atomic_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp85[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp85,_tmp85,_tmp85 + 
18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={3,0}},& Cyc_Toc__cyccalloc_atomic_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)&
Cyc_Toc__cyccalloc_atomic_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp87[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp87,_tmp87,_tmp87 + 15};
static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={3,0}},& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)&
Cyc_Toc__region_malloc_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp89[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp89,_tmp89,_tmp89 + 15};
static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={3,0}},& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)&
Cyc_Toc__region_calloc_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp8B[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp8B,_tmp8B,_tmp8B + 13};
static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={3,0}},& Cyc_Toc__check_times_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)&
Cyc_Toc__check_times_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp8D[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp8D,_tmp8D,_tmp8D + 12};
static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={3,0}},& Cyc_Toc__new_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=&
Cyc_Toc__new_region_ev;static char _tmp8F[13]="_push_region";static struct
_dyneither_ptr Cyc_Toc__push_region_str={_tmp8F,_tmp8F,_tmp8F + 13};static struct
_tuple1 Cyc_Toc__push_region_pr={{.Loc_n={3,0}},& Cyc_Toc__push_region_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)&
Cyc_Toc__push_region_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp91[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp91,_tmp91,_tmp91 + 12};
static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={3,0}},& Cyc_Toc__pop_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,
0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=&
Cyc_Toc__pop_region_ev;static char _tmp93[16]="_open_dynregion";static struct
_dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp93,_tmp93,_tmp93 + 16};static
struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)&
Cyc_Toc__open_dynregion_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmp95[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmp95,_tmp95,_tmp95 + 16};
static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__push_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)&
Cyc_Toc__push_dynregion_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmp97[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp97,_tmp97,_tmp97 + 15};
static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={3,0}},& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)&
Cyc_Toc__pop_dynregion_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmp99[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmp99,_tmp99,_tmp99 + 14};
static struct _tuple1 Cyc_Toc__reset_region_pr={{.Loc_n={3,0}},& Cyc_Toc__reset_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)&
Cyc_Toc__reset_region_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmp9B[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp9B,_tmp9B,_tmp9B + 
19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={3,0}},& Cyc_Toc__throw_arraybounds_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(
void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct
Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static
char _tmp9D[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={
_tmp9D,_tmp9D,_tmp9D + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={
3,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={
0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)Cyc_Absyn_EmptyAnnot};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;
static char _tmp9F[11]="_swap_word";static struct _dyneither_ptr Cyc_Toc__swap_word_str={
_tmp9F,_tmp9F,_tmp9F + 11};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={3,0}},&
Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={
1,& Cyc_Toc__swap_word_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)& Cyc_Toc__swap_word_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpA1[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpA1,_tmpA1,_tmpA1 + 16};
static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={3,0}},& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)&
Cyc_Toc__swap_dyneither_re,0,(void*)Cyc_Absyn_EmptyAnnot};static struct Cyc_Absyn_Exp*
Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static struct Cyc_Absyn_AggrType_struct
Cyc_Toc_dyneither_ptr_typ_v={10,{{.UnknownAggr={1,{(void*)0,& Cyc_Toc__dyneither_ptr_pr,
0}}},0}};static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(
skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**_tmpA4=_cycalloc(sizeof(*
_tmpA4));_tmpA4[0]=Cyc_Absyn_skip_stmt(0);_tmpA4;});return*skip_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(
t,e,0,(void*)1,0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));
_tmpA5[0]=({struct Cyc_Absyn_Cast_e_struct _tmpA6;_tmpA6.tag=15;_tmpA6.f1=(void*)t;
_tmpA6.f2=e;_tmpA6.f3=0;_tmpA6.f4=(void*)((void*)1);_tmpA6;});_tmpA5;});}static
void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Deref_e_struct*
_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_Deref_e_struct
_tmpA8;_tmpA8.tag=22;_tmpA8.f1=e;_tmpA8;});_tmpA7;});}static void*Cyc_Toc_subscript_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*
_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_Subscript_e_struct
_tmpAA;_tmpAA.tag=25;_tmpAA.f1=e1;_tmpAA.f2=e2;_tmpAA;});_tmpA9;});}static void*
Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*
_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_StmtExp_e_struct
_tmpAC;_tmpAC.tag=38;_tmpAC.f1=s;_tmpAC;});_tmpAB;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpAD=_cycalloc(
sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpAE;_tmpAE.tag=
18;_tmpAE.f1=(void*)t;_tmpAE;});_tmpAD;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpB0;_tmpB0.tag=11;_tmpB0.f1=e;_tmpB0.f2=es;_tmpB0.f3=0;_tmpB0;});_tmpAF;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_Exp_s_struct _tmpB2;
_tmpB2.tag=0;_tmpB2.f1=e;_tmpB2;});_tmpB1;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_Seq_s_struct _tmpB4;
_tmpB4.tag=1;_tmpB4.f1=s1;_tmpB4.f2=s2;_tmpB4;});_tmpB3;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));
_tmpB5[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpB6;_tmpB6.tag=6;_tmpB6.f1=e1;
_tmpB6.f2=e2;_tmpB6.f3=e3;_tmpB6;});_tmpB5;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpB8;_tmpB8.tag=23;_tmpB8.f1=e;_tmpB8.f2=n;_tmpB8.f3=0;_tmpB8.f4=0;_tmpB8;});
_tmpB7;});}static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpB9=
_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpBA;
_tmpBA.tag=24;_tmpBA.f1=e;_tmpBA.f2=n;_tmpBA.f3=0;_tmpBA.f4=0;_tmpBA;});_tmpB9;});}
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*
ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpBB=_cycalloc(
sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct _tmpBC;_tmpBC.tag=
37;_tmpBC.f1=tdopt;_tmpBC.f2=ds;_tmpBC;});_tmpBB;});}static void*Cyc_Toc_goto_stmt_r(
struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_Goto_s_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_Goto_s_struct
_tmpBE;_tmpBE.tag=7;_tmpBE.f1=v;_tmpBE.f2=s;_tmpBE;});_tmpBD;});}static struct Cyc_Absyn_Const_e_struct
Cyc_Toc_zero_exp={0,{.Int_c={4,{(void*)0,0}}}};struct Cyc_Toc_functionSet{struct
Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct
Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;
struct Cyc_Absyn_Exp*fvoidstar;};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={&
Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,&
Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,&
Cyc_Toc__zero_arr_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={&
Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,&
Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmpC0=t;
void*_tmpC1;int _tmpC2;_LL1: if(_tmpC0 <= (void*)4)goto _LL3;if(*((int*)_tmpC0)!= 5)
goto _LL3;_tmpC1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpC0)->f2;_LL2:{void*
_tmpC3=_tmpC1;_LLC: if((int)_tmpC3 != 0)goto _LLE;_LLD: function=fS->fchar;goto _LLB;
_LLE: if((int)_tmpC3 != 1)goto _LL10;_LLF: function=fS->fshort;goto _LLB;_LL10: if((
int)_tmpC3 != 2)goto _LL12;_LL11: function=fS->fint;goto _LLB;_LL12:;_LL13:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));
_tmpC4[0]=({struct Cyc_Core_Impossible_struct _tmpC5;_tmpC5.tag=Cyc_Core_Impossible;
_tmpC5.f1=({const char*_tmpC6="impossible IntType (not char, short or int)";
_tag_dyneither(_tmpC6,sizeof(char),44);});_tmpC5;});_tmpC4;}));_LLB:;}goto _LL0;
_LL3: if((int)_tmpC0 != 1)goto _LL5;_LL4: function=fS->ffloat;goto _LL0;_LL5: if(
_tmpC0 <= (void*)4)goto _LL9;if(*((int*)_tmpC0)!= 6)goto _LL7;_tmpC2=((struct Cyc_Absyn_DoubleType_struct*)
_tmpC0)->f1;_LL6: switch(_tmpC2){case 1: _LL14: function=fS->flongdouble;break;
default: _LL15: function=fS->fdouble;}goto _LL0;_LL7: if(*((int*)_tmpC0)!= 4)goto
_LL9;_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;_LLA:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7[0]=({struct
Cyc_Core_Impossible_struct _tmpC8;_tmpC8.tag=Cyc_Core_Impossible;_tmpC8.f1=({
const char*_tmpC9="impossible expression type (not int, float, double, or pointer)";
_tag_dyneither(_tmpC9,sizeof(char),64);});_tmpC8;});_tmpC7;}));_LL0:;}return
function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,
struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(fS,Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmpCA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmpCB;void*_tmpCC;_LL18: if(_tmpCA <= (
void*)4)goto _LL1A;if(*((int*)_tmpCA)!= 4)goto _LL1A;_tmpCB=((struct Cyc_Absyn_PointerType_struct*)
_tmpCA)->f1;_tmpCC=_tmpCB.elt_typ;_LL19: return Cyc_Toc_getFunctionType(fS,_tmpCC);
_LL1A:;_LL1B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpCD=
_cycalloc(sizeof(*_tmpCD));_tmpCD[0]=({struct Cyc_Core_Impossible_struct _tmpCE;
_tmpCE.tag=Cyc_Core_Impossible;_tmpCE.f1=({const char*_tmpCF="impossible type (not pointer)";
_tag_dyneither(_tmpCF,sizeof(char),30);});_tmpCE;});_tmpCD;}));_LL17:;}struct
_tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmpD0=e->r;union Cyc_Absyn_Cnst _tmpD1;struct _tuple4
_tmpD2;char _tmpD3;union Cyc_Absyn_Cnst _tmpD4;struct _tuple5 _tmpD5;short _tmpD6;
union Cyc_Absyn_Cnst _tmpD7;struct _tuple6 _tmpD8;int _tmpD9;union Cyc_Absyn_Cnst
_tmpDA;struct _tuple7 _tmpDB;long long _tmpDC;union Cyc_Absyn_Cnst _tmpDD;int _tmpDE;
struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_List_List*_tmpE0;struct Cyc_List_List*_tmpE1;
struct Cyc_List_List*_tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_List_List*_tmpE4;
_LL1D: if(*((int*)_tmpD0)!= 0)goto _LL1F;_tmpD1=((struct Cyc_Absyn_Const_e_struct*)
_tmpD0)->f1;if((_tmpD1.Char_c).tag != 2)goto _LL1F;_tmpD2=(struct _tuple4)(_tmpD1.Char_c).val;
_tmpD3=_tmpD2.f2;_LL1E: return _tmpD3 == '\000';_LL1F: if(*((int*)_tmpD0)!= 0)goto
_LL21;_tmpD4=((struct Cyc_Absyn_Const_e_struct*)_tmpD0)->f1;if((_tmpD4.Short_c).tag
!= 3)goto _LL21;_tmpD5=(struct _tuple5)(_tmpD4.Short_c).val;_tmpD6=_tmpD5.f2;_LL20:
return _tmpD6 == 0;_LL21: if(*((int*)_tmpD0)!= 0)goto _LL23;_tmpD7=((struct Cyc_Absyn_Const_e_struct*)
_tmpD0)->f1;if((_tmpD7.Int_c).tag != 4)goto _LL23;_tmpD8=(struct _tuple6)(_tmpD7.Int_c).val;
_tmpD9=_tmpD8.f2;_LL22: return _tmpD9 == 0;_LL23: if(*((int*)_tmpD0)!= 0)goto _LL25;
_tmpDA=((struct Cyc_Absyn_Const_e_struct*)_tmpD0)->f1;if((_tmpDA.LongLong_c).tag
!= 5)goto _LL25;_tmpDB=(struct _tuple7)(_tmpDA.LongLong_c).val;_tmpDC=_tmpDB.f2;
_LL24: return _tmpDC == 0;_LL25: if(*((int*)_tmpD0)!= 0)goto _LL27;_tmpDD=((struct Cyc_Absyn_Const_e_struct*)
_tmpD0)->f1;if((_tmpDD.Null_c).tag != 1)goto _LL27;_tmpDE=(int)(_tmpDD.Null_c).val;
_LL26: return 1;_LL27: if(*((int*)_tmpD0)!= 15)goto _LL29;_tmpDF=((struct Cyc_Absyn_Cast_e_struct*)
_tmpD0)->f2;_LL28: return Cyc_Toc_is_zero(_tmpDF);_LL29: if(*((int*)_tmpD0)!= 26)
goto _LL2B;_tmpE0=((struct Cyc_Absyn_Tuple_e_struct*)_tmpD0)->f1;_LL2A: return((int(*)(
int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmpE0);_LL2B: if(*((int*)_tmpD0)!= 28)goto _LL2D;_tmpE1=((struct Cyc_Absyn_Array_e_struct*)
_tmpD0)->f1;_LL2C: _tmpE2=_tmpE1;goto _LL2E;_LL2D: if(*((int*)_tmpD0)!= 30)goto
_LL2F;_tmpE2=((struct Cyc_Absyn_Aggregate_e_struct*)_tmpD0)->f3;_LL2E: _tmpE3=
_tmpE2;goto _LL30;_LL2F: if(*((int*)_tmpD0)!= 27)goto _LL31;_tmpE3=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpD0)->f2;_LL30: _tmpE4=_tmpE3;goto _LL32;_LL31: if(*((int*)_tmpD0)!= 37)goto
_LL33;_tmpE4=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpD0)->f2;_LL32: for(0;
_tmpE4 != 0;_tmpE4=_tmpE4->tl){if(!Cyc_Toc_is_zero((*((struct _tuple16*)_tmpE4->hd)).f2))
return 0;}return 1;_LL33:;_LL34: return 0;_LL1C:;}static int Cyc_Toc_is_nullable(void*
t){void*_tmpE5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE6;struct Cyc_Absyn_PtrAtts
_tmpE7;union Cyc_Absyn_Constraint*_tmpE8;_LL36: if(_tmpE5 <= (void*)4)goto _LL38;if(*((
int*)_tmpE5)!= 4)goto _LL38;_tmpE6=((struct Cyc_Absyn_PointerType_struct*)_tmpE5)->f1;
_tmpE7=_tmpE6.ptr_atts;_tmpE8=_tmpE7.nullable;_LL37: return((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmpE8);_LL38:;_LL39:({void*_tmpE9=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmpEA="is_nullable";
_tag_dyneither(_tmpEA,sizeof(char),12);}),_tag_dyneither(_tmpE9,sizeof(void*),0));});
_LL35:;}static char _tmpF7[1]="";static char _tmpF8[8]="*bogus*";static struct _tuple1*
Cyc_Toc_collapse_qvar_tag(struct _tuple1*x,struct _dyneither_ptr tag){struct
_DynRegionHandle*_tmpEC;struct Cyc_Dict_Dict*_tmpED;struct Cyc_Toc_TocState _tmpEB=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpEC=_tmpEB.dyn;_tmpED=
_tmpEB.qvar_tags;{static struct _dyneither_ptr bogus_string={_tmpF8,_tmpF8,_tmpF8 + 
8};static struct _tuple1 bogus_qvar={{.Loc_n={3,0}},& bogus_string};static struct
_tuple14 pair={& bogus_qvar,{_tmpF7,_tmpF7,_tmpF7 + 1}};pair=({struct _tuple14 _tmpEE;
_tmpEE.f1=x;_tmpEE.f2=tag;_tmpEE;});{struct _DynRegionFrame _tmpEF;struct
_RegionHandle*d=_open_dynregion(& _tmpEF,_tmpEC);{struct _tuple1**_tmpF0=((struct
_tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple14*k))Cyc_Dict_lookup_opt)(*_tmpED,(
struct _tuple14*)& pair);if(_tmpF0 != 0){struct _tuple1*_tmpF1=*_tmpF0;_npop_handler(
0);return _tmpF1;}{struct _tuple14*_tmpF2=({struct _tuple14*_tmpF6=_cycalloc(
sizeof(*_tmpF6));_tmpF6->f1=x;_tmpF6->f2=tag;_tmpF6;});struct _tuple1*res=({
struct _tuple1*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->f1=(*x).f1;_tmpF4->f2=({
struct _dyneither_ptr*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=(struct
_dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*x).f2,(struct
_dyneither_ptr)tag);_tmpF5;});_tmpF4;});*_tmpED=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpED,(
struct _tuple14*)_tmpF2,res);{struct _tuple1*_tmpF3=res;_npop_handler(0);return
_tmpF3;}}};_pop_dynregion(d);}}}struct _tuple17{void*f1;struct Cyc_List_List*f2;};
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){struct
_DynRegionHandle*_tmpFA;struct Cyc_List_List**_tmpFB;struct Cyc_Toc_TocState _tmpF9=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmpFA=_tmpF9.dyn;_tmpFB=
_tmpF9.tuple_types;{struct _DynRegionFrame _tmpFC;struct _RegionHandle*d=
_open_dynregion(& _tmpFC,_tmpFA);{struct Cyc_List_List*_tmpFD=*_tmpFB;for(0;_tmpFD
!= 0;_tmpFD=_tmpFD->tl){struct _tuple17 _tmpFF;void*_tmp100;struct Cyc_List_List*
_tmp101;struct _tuple17*_tmpFE=(struct _tuple17*)_tmpFD->hd;_tmpFF=*_tmpFE;_tmp100=
_tmpFF.f1;_tmp101=_tmpFF.f2;{struct Cyc_List_List*_tmp102=tqs0;for(0;_tmp102 != 0
 && _tmp101 != 0;(_tmp102=_tmp102->tl,_tmp101=_tmp101->tl)){if(!Cyc_Tcutil_unify((*((
struct _tuple10*)_tmp102->hd)).f2,(void*)_tmp101->hd))break;}if(_tmp102 == 0  && 
_tmp101 == 0){void*_tmp103=_tmp100;_npop_handler(0);return _tmp103;}}}}{struct
_dyneither_ptr*xname=({struct _dyneither_ptr*_tmp111=_cycalloc(sizeof(*_tmp111));
_tmp111[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp114;_tmp114.tag=1;
_tmp114.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmp112[1]={&
_tmp114};Cyc_aprintf(({const char*_tmp113="_tuple%d";_tag_dyneither(_tmp113,
sizeof(char),9);}),_tag_dyneither(_tmp112,sizeof(void*),1));}});_tmp111;});void*
x=Cyc_Absyn_strct(xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(
d,Cyc_Tcutil_snd_tqt,tqs0);struct Cyc_List_List*_tmp104=0;struct Cyc_List_List*ts2=
ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){_tmp104=({struct Cyc_List_List*
_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->hd=({struct Cyc_Absyn_Aggrfield*
_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->name=Cyc_Absyn_fieldname(i);_tmp106->tq=
Cyc_Toc_mt_tq;_tmp106->type=(void*)ts2->hd;_tmp106->width=0;_tmp106->attributes=
0;_tmp106;});_tmp105->tl=_tmp104;_tmp105;});}}_tmp104=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp104);{struct Cyc_Absyn_Aggrdecl*
_tmp107=({struct Cyc_Absyn_Aggrdecl*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->kind=(
void*)0;_tmp10E->sc=(void*)2;_tmp10E->name=({struct _tuple1*_tmp110=_cycalloc(
sizeof(*_tmp110));_tmp110->f1=Cyc_Absyn_Rel_n(0);_tmp110->f2=xname;_tmp110;});
_tmp10E->tvs=0;_tmp10E->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp10F=_cycalloc(
sizeof(*_tmp10F));_tmp10F->exist_vars=0;_tmp10F->rgn_po=0;_tmp10F->fields=
_tmp104;_tmp10F->tagged=0;_tmp10F;});_tmp10E->attributes=0;_tmp10E;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp109=_cycalloc(sizeof(*_tmp109));
_tmp109[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp10A;_tmp10A.tag=6;_tmp10A.f1=
_tmp107;_tmp10A;});_tmp109;}),0);_tmp108->tl=Cyc_Toc_result_decls;_tmp108;});*
_tmpFB=({struct Cyc_List_List*_tmp10B=_region_malloc(d,sizeof(*_tmp10B));_tmp10B->hd=({
struct _tuple17*_tmp10C=_region_malloc(d,sizeof(*_tmp10C));_tmp10C->f1=x;_tmp10C->f2=
ts;_tmp10C;});_tmp10B->tl=*_tmpFB;_tmp10B;});{void*_tmp10D=x;_npop_handler(0);
return _tmp10D;}}};_pop_dynregion(d);}}struct _tuple1*Cyc_Toc_temp_var(){int
_tmp115=Cyc_Toc_temp_var_counter ++;struct _tuple1*res=({struct _tuple1*_tmp116=
_cycalloc(sizeof(*_tmp116));_tmp116->f1=Cyc_Absyn_Loc_n;_tmp116->f2=({struct
_dyneither_ptr*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117[0]=(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp11A;_tmp11A.tag=1;_tmp11A.f1=(
unsigned int)_tmp115;{void*_tmp118[1]={& _tmp11A};Cyc_aprintf(({const char*_tmp119="_tmp%X";
_tag_dyneither(_tmp119,sizeof(char),7);}),_tag_dyneither(_tmp118,sizeof(void*),1));}});
_tmp117;});_tmp116;});return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp11C;struct Cyc_Xarray_Xarray*_tmp11D;struct Cyc_Toc_TocState
_tmp11B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp11C=
_tmp11B.dyn;_tmp11D=_tmp11B.temp_labels;{struct _DynRegionFrame _tmp11E;struct
_RegionHandle*d=_open_dynregion(& _tmp11E,_tmp11C);{int _tmp11F=Cyc_Toc_fresh_label_counter
++;if(_tmp11F < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp11D)){
struct _dyneither_ptr*_tmp120=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp11D,_tmp11F);_npop_handler(0);return _tmp120;}{struct
_dyneither_ptr*res=({struct _dyneither_ptr*_tmp124=_cycalloc(sizeof(*_tmp124));
_tmp124[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp127;_tmp127.tag=1;
_tmp127.f1=(unsigned int)_tmp11F;{void*_tmp125[1]={& _tmp127};Cyc_aprintf(({const
char*_tmp126="_LL%X";_tag_dyneither(_tmp126,sizeof(char),6);}),_tag_dyneither(
_tmp125,sizeof(void*),1));}});_tmp124;});if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp11D,res)!= _tmp11F)({void*_tmp121=
0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp122="fresh_label: add_ind returned bad index...";_tag_dyneither(
_tmp122,sizeof(char),43);}),_tag_dyneither(_tmp121,sizeof(void*),0));});{struct
_dyneither_ptr*_tmp123=res;_npop_handler(0);return _tmp123;}}};_pop_dynregion(d);}}
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,
struct _tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmp128=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(
name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp128))->hd)->name)
!= 0){if(carries_value  && ((struct Cyc_Absyn_Datatypefield*)_tmp128->hd)->typs != 
0  || !carries_value  && ((struct Cyc_Absyn_Datatypefield*)_tmp128->hd)->typs == 0)
++ ans;_tmp128=_tmp128->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Datatypedecl*td){int ans=0;{struct Cyc_List_List*
_tmp129=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmp129 != 0;_tmp129=_tmp129->tl){if(((struct Cyc_Absyn_Datatypefield*)
_tmp129->hd)->typs == 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);
static struct _tuple8*Cyc_Toc_arg_to_c(struct _tuple8*a){struct Cyc_Core_Opt*_tmp12B;
struct Cyc_Absyn_Tqual _tmp12C;void*_tmp12D;struct _tuple8 _tmp12A=*a;_tmp12B=
_tmp12A.f1;_tmp12C=_tmp12A.f2;_tmp12D=_tmp12A.f3;return({struct _tuple8*_tmp12E=
_cycalloc(sizeof(*_tmp12E));_tmp12E->f1=_tmp12B;_tmp12E->f2=_tmp12C;_tmp12E->f3=
Cyc_Toc_typ_to_c(_tmp12D);_tmp12E;});}static struct _tuple10*Cyc_Toc_typ_to_c_f(
struct _tuple10*x){struct Cyc_Absyn_Tqual _tmp130;void*_tmp131;struct _tuple10
_tmp12F=*x;_tmp130=_tmp12F.f1;_tmp131=_tmp12F.f2;return({struct _tuple10*_tmp132=
_cycalloc(sizeof(*_tmp132));_tmp132->f1=_tmp130;_tmp132->f2=Cyc_Toc_typ_to_c(
_tmp131);_tmp132;});}static void*Cyc_Toc_typ_to_c_array(void*t){void*_tmp133=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_ArrayInfo _tmp134;void*_tmp135;struct Cyc_Absyn_Tqual _tmp136;
struct Cyc_Absyn_Exp*_tmp137;union Cyc_Absyn_Constraint*_tmp138;struct Cyc_Position_Segment*
_tmp139;struct Cyc_Core_Opt*_tmp13A;struct Cyc_Core_Opt _tmp13B;void*_tmp13C;_LL3B:
if(_tmp133 <= (void*)4)goto _LL3F;if(*((int*)_tmp133)!= 7)goto _LL3D;_tmp134=((
struct Cyc_Absyn_ArrayType_struct*)_tmp133)->f1;_tmp135=_tmp134.elt_type;_tmp136=
_tmp134.tq;_tmp137=_tmp134.num_elts;_tmp138=_tmp134.zero_term;_tmp139=_tmp134.zt_loc;
_LL3C: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp135),_tmp136,_tmp137,
Cyc_Absyn_false_conref,_tmp139);_LL3D: if(*((int*)_tmp133)!= 0)goto _LL3F;_tmp13A=((
struct Cyc_Absyn_Evar_struct*)_tmp133)->f2;if(_tmp13A == 0)goto _LL3F;_tmp13B=*
_tmp13A;_tmp13C=(void*)_tmp13B.v;_LL3E: return Cyc_Toc_typ_to_c_array(_tmp13C);
_LL3F:;_LL40: return Cyc_Toc_typ_to_c(t);_LL3A:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->name=f->name;_tmp13D->tq=Cyc_Toc_mt_tq;
_tmp13D->type=Cyc_Toc_typ_to_c(f->type);_tmp13D->width=f->width;_tmp13D->attributes=
f->attributes;_tmp13D;});}static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*
fs){return;}static void*Cyc_Toc_char_star_typ(){static void**cs=0;if(cs == 0)cs=({
void**_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp13E;});return*cs;}static void*
Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0)r=({void**_tmp13F=_cycalloc(sizeof(*
_tmp13F));_tmp13F[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),
Cyc_Toc_mt_tq);_tmp13F;});return*r;}static void*Cyc_Toc_dyn_rgn_typ(){static void**
r=0;if(r == 0)r=({void**_tmp140=_cycalloc(sizeof(*_tmp140));_tmp140[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);_tmp140;});return*r;}
static void*Cyc_Toc_typ_to_c(void*t){void*_tmp141=t;struct Cyc_Core_Opt*_tmp142;
struct Cyc_Core_Opt*_tmp143;struct Cyc_Core_Opt _tmp144;void*_tmp145;struct Cyc_Absyn_Tvar*
_tmp146;struct Cyc_Absyn_DatatypeInfo _tmp147;union Cyc_Absyn_DatatypeInfoU _tmp148;
struct Cyc_Absyn_Datatypedecl**_tmp149;struct Cyc_Absyn_Datatypedecl*_tmp14A;
struct Cyc_Absyn_DatatypeFieldInfo _tmp14B;union Cyc_Absyn_DatatypeFieldInfoU
_tmp14C;struct _tuple2 _tmp14D;struct Cyc_Absyn_Datatypedecl*_tmp14E;struct Cyc_Absyn_Datatypefield*
_tmp14F;struct Cyc_Absyn_PtrInfo _tmp150;void*_tmp151;struct Cyc_Absyn_Tqual _tmp152;
struct Cyc_Absyn_PtrAtts _tmp153;union Cyc_Absyn_Constraint*_tmp154;struct Cyc_Absyn_ArrayInfo
_tmp155;void*_tmp156;struct Cyc_Absyn_Tqual _tmp157;struct Cyc_Absyn_Exp*_tmp158;
struct Cyc_Position_Segment*_tmp159;struct Cyc_Absyn_FnInfo _tmp15A;void*_tmp15B;
struct Cyc_List_List*_tmp15C;int _tmp15D;struct Cyc_Absyn_VarargInfo*_tmp15E;struct
Cyc_List_List*_tmp15F;struct Cyc_List_List*_tmp160;void*_tmp161;struct Cyc_List_List*
_tmp162;struct Cyc_Absyn_AggrInfo _tmp163;union Cyc_Absyn_AggrInfoU _tmp164;struct
Cyc_List_List*_tmp165;struct _tuple1*_tmp166;struct Cyc_List_List*_tmp167;struct
_tuple1*_tmp168;struct Cyc_List_List*_tmp169;struct Cyc_Absyn_Typedefdecl*_tmp16A;
void**_tmp16B;void*_tmp16C;_LL42: if((int)_tmp141 != 0)goto _LL44;_LL43: return t;
_LL44: if(_tmp141 <= (void*)4)goto _LL56;if(*((int*)_tmp141)!= 0)goto _LL46;_tmp142=((
struct Cyc_Absyn_Evar_struct*)_tmp141)->f2;if(_tmp142 != 0)goto _LL46;_LL45: return
Cyc_Absyn_sint_typ;_LL46: if(*((int*)_tmp141)!= 0)goto _LL48;_tmp143=((struct Cyc_Absyn_Evar_struct*)
_tmp141)->f2;if(_tmp143 == 0)goto _LL48;_tmp144=*_tmp143;_tmp145=(void*)_tmp144.v;
_LL47: return Cyc_Toc_typ_to_c(_tmp145);_LL48: if(*((int*)_tmp141)!= 1)goto _LL4A;
_tmp146=((struct Cyc_Absyn_VarType_struct*)_tmp141)->f1;_LL49: if(Cyc_Tcutil_tvar_kind(
_tmp146)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}_LL4A:
if(*((int*)_tmp141)!= 2)goto _LL4C;_tmp147=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp141)->f1;_tmp148=_tmp147.datatype_info;if((_tmp148.KnownDatatype).tag != 2)
goto _LL4C;_tmp149=(struct Cyc_Absyn_Datatypedecl**)(_tmp148.KnownDatatype).val;
_tmp14A=*_tmp149;_LL4B: return Cyc_Absyn_void_star_typ();_LL4C: if(*((int*)_tmp141)
!= 2)goto _LL4E;_LL4D:({void*_tmp16D=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp16E="unresolved DatatypeType";
_tag_dyneither(_tmp16E,sizeof(char),24);}),_tag_dyneither(_tmp16D,sizeof(void*),
0));});_LL4E: if(*((int*)_tmp141)!= 3)goto _LL50;_tmp14B=((struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp141)->f1;_tmp14C=_tmp14B.field_info;if((_tmp14C.KnownDatatypefield).tag != 2)
goto _LL50;_tmp14D=(struct _tuple2)(_tmp14C.KnownDatatypefield).val;_tmp14E=
_tmp14D.f1;_tmp14F=_tmp14D.f2;_LL4F: if(_tmp14F->typs == 0){if(_tmp14E->is_extensible)
return Cyc_Toc_char_star_typ();else{return Cyc_Absyn_uint_typ;}}else{return Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp14F->name,({const char*_tmp16F="_struct";
_tag_dyneither(_tmp16F,sizeof(char),8);})));}_LL50: if(*((int*)_tmp141)!= 3)goto
_LL52;_LL51:({void*_tmp170=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp171="unresolved DatatypeFieldType";
_tag_dyneither(_tmp171,sizeof(char),29);}),_tag_dyneither(_tmp170,sizeof(void*),
0));});_LL52: if(*((int*)_tmp141)!= 4)goto _LL54;_tmp150=((struct Cyc_Absyn_PointerType_struct*)
_tmp141)->f1;_tmp151=_tmp150.elt_typ;_tmp152=_tmp150.elt_tq;_tmp153=_tmp150.ptr_atts;
_tmp154=_tmp153.bounds;_LL53: _tmp151=Cyc_Toc_typ_to_c_array(_tmp151);{void*
_tmp172=Cyc_Absyn_conref_def((void*)((void*)0),_tmp154);_LL7D: if((int)_tmp172 != 
0)goto _LL7F;_LL7E: return Cyc_Toc_dyneither_ptr_typ;_LL7F:;_LL80: return Cyc_Absyn_star_typ(
_tmp151,(void*)2,_tmp152,Cyc_Absyn_false_conref);_LL7C:;}_LL54: if(*((int*)
_tmp141)!= 5)goto _LL56;_LL55: goto _LL57;_LL56: if((int)_tmp141 != 1)goto _LL58;_LL57:
goto _LL59;_LL58: if(_tmp141 <= (void*)4)goto _LL70;if(*((int*)_tmp141)!= 6)goto
_LL5A;_LL59: return t;_LL5A: if(*((int*)_tmp141)!= 7)goto _LL5C;_tmp155=((struct Cyc_Absyn_ArrayType_struct*)
_tmp141)->f1;_tmp156=_tmp155.elt_type;_tmp157=_tmp155.tq;_tmp158=_tmp155.num_elts;
_tmp159=_tmp155.zt_loc;_LL5B: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp156),_tmp157,_tmp158,Cyc_Absyn_false_conref,_tmp159);_LL5C: if(*((int*)
_tmp141)!= 8)goto _LL5E;_tmp15A=((struct Cyc_Absyn_FnType_struct*)_tmp141)->f1;
_tmp15B=_tmp15A.ret_typ;_tmp15C=_tmp15A.args;_tmp15D=_tmp15A.c_varargs;_tmp15E=
_tmp15A.cyc_varargs;_tmp15F=_tmp15A.attributes;_LL5D: {struct Cyc_List_List*
_tmp173=0;for(0;_tmp15F != 0;_tmp15F=_tmp15F->tl){void*_tmp174=(void*)_tmp15F->hd;
_LL82: if((int)_tmp174 != 3)goto _LL84;_LL83: goto _LL85;_LL84: if((int)_tmp174 != 4)
goto _LL86;_LL85: goto _LL87;_LL86: if(_tmp174 <= (void*)17)goto _LL8A;if(*((int*)
_tmp174)!= 3)goto _LL88;_LL87: continue;_LL88: if(*((int*)_tmp174)!= 4)goto _LL8A;
_LL89: continue;_LL8A:;_LL8B: _tmp173=({struct Cyc_List_List*_tmp175=_cycalloc(
sizeof(*_tmp175));_tmp175->hd=(void*)((void*)_tmp15F->hd);_tmp175->tl=_tmp173;
_tmp175;});goto _LL81;_LL81:;}{struct Cyc_List_List*_tmp176=((struct Cyc_List_List*(*)(
struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp15C);if(_tmp15E != 0){void*_tmp177=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp15E->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct _tuple8*
_tmp178=({struct _tuple8*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->f1=_tmp15E->name;
_tmp17A->f2=_tmp15E->tq;_tmp17A->f3=_tmp177;_tmp17A;});_tmp176=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp176,({
struct Cyc_List_List*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->hd=_tmp178;
_tmp179->tl=0;_tmp179;}));}return(void*)({struct Cyc_Absyn_FnType_struct*_tmp17B=
_cycalloc(sizeof(*_tmp17B));_tmp17B[0]=({struct Cyc_Absyn_FnType_struct _tmp17C;
_tmp17C.tag=8;_tmp17C.f1=({struct Cyc_Absyn_FnInfo _tmp17D;_tmp17D.tvars=0;_tmp17D.effect=
0;_tmp17D.ret_typ=Cyc_Toc_typ_to_c(_tmp15B);_tmp17D.args=_tmp176;_tmp17D.c_varargs=
_tmp15D;_tmp17D.cyc_varargs=0;_tmp17D.rgn_po=0;_tmp17D.attributes=_tmp173;
_tmp17D;});_tmp17C;});_tmp17B;});}}_LL5E: if(*((int*)_tmp141)!= 9)goto _LL60;
_tmp160=((struct Cyc_Absyn_TupleType_struct*)_tmp141)->f1;_LL5F: _tmp160=((struct
Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp160);return Cyc_Toc_add_tuple_type(_tmp160);
_LL60: if(*((int*)_tmp141)!= 11)goto _LL62;_tmp161=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp141)->f1;_tmp162=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp141)->f2;_LL61:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp17E=_cycalloc(sizeof(*
_tmp17E));_tmp17E[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp17F;_tmp17F.tag=11;
_tmp17F.f1=(void*)_tmp161;_tmp17F.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp162);_tmp17F;});_tmp17E;});_LL62: if(*((int*)_tmp141)!= 10)goto _LL64;_tmp163=((
struct Cyc_Absyn_AggrType_struct*)_tmp141)->f1;_tmp164=_tmp163.aggr_info;_tmp165=
_tmp163.targs;_LL63:{union Cyc_Absyn_AggrInfoU _tmp180=_tmp164;struct _tuple3
_tmp181;_LL8D: if((_tmp180.UnknownAggr).tag != 1)goto _LL8F;_tmp181=(struct _tuple3)(
_tmp180.UnknownAggr).val;_LL8E: return t;_LL8F:;_LL90: goto _LL8C;_LL8C:;}{struct Cyc_Absyn_Aggrdecl*
_tmp182=Cyc_Absyn_get_known_aggrdecl(_tmp164);if(_tmp182->kind == (void*)1)return
Cyc_Toc_aggrdecl_type(_tmp182->name,Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(
_tmp182->name,Cyc_Absyn_strctq);}}_LL64: if(*((int*)_tmp141)!= 12)goto _LL66;
_tmp166=((struct Cyc_Absyn_EnumType_struct*)_tmp141)->f1;_LL65: return t;_LL66: if(*((
int*)_tmp141)!= 13)goto _LL68;_tmp167=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp141)->f1;_LL67: Cyc_Toc_enumfields_to_c(_tmp167);return t;_LL68: if(*((int*)
_tmp141)!= 16)goto _LL6A;_tmp168=((struct Cyc_Absyn_TypedefType_struct*)_tmp141)->f1;
_tmp169=((struct Cyc_Absyn_TypedefType_struct*)_tmp141)->f2;_tmp16A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp141)->f3;_tmp16B=((struct Cyc_Absyn_TypedefType_struct*)_tmp141)->f4;_LL69:
if(_tmp16B == 0  || Cyc_noexpand_r){if(_tmp169 != 0)return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp184;_tmp184.tag=16;_tmp184.f1=_tmp168;_tmp184.f2=0;_tmp184.f3=_tmp16A;
_tmp184.f4=0;_tmp184;});_tmp183;});else{return t;}}else{return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp186;_tmp186.tag=16;_tmp186.f1=_tmp168;_tmp186.f2=0;_tmp186.f3=_tmp16A;
_tmp186.f4=({void**_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=Cyc_Toc_typ_to_c_array(*
_tmp16B);_tmp187;});_tmp186;});_tmp185;});}_LL6A: if(*((int*)_tmp141)!= 18)goto
_LL6C;_LL6B: return Cyc_Absyn_uint_typ;_LL6C: if(*((int*)_tmp141)!= 14)goto _LL6E;
_tmp16C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp141)->f1;_LL6D: return
Cyc_Toc_rgn_typ();_LL6E: if(*((int*)_tmp141)!= 15)goto _LL70;_LL6F: return Cyc_Toc_dyn_rgn_typ();
_LL70: if((int)_tmp141 != 2)goto _LL72;_LL71:({void*_tmp188=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp189="Toc::typ_to_c: type translation passed the heap region";
_tag_dyneither(_tmp189,sizeof(char),55);}),_tag_dyneither(_tmp188,sizeof(void*),
0));});_LL72: if((int)_tmp141 != 3)goto _LL74;_LL73:({void*_tmp18A=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp18B="Toc::typ_to_c: type translation passed the unique region";
_tag_dyneither(_tmp18B,sizeof(char),57);}),_tag_dyneither(_tmp18A,sizeof(void*),
0));});_LL74: if(_tmp141 <= (void*)4)goto _LL76;if(*((int*)_tmp141)!= 19)goto _LL76;
_LL75: goto _LL77;_LL76: if(_tmp141 <= (void*)4)goto _LL78;if(*((int*)_tmp141)!= 20)
goto _LL78;_LL77: goto _LL79;_LL78: if(_tmp141 <= (void*)4)goto _LL7A;if(*((int*)
_tmp141)!= 21)goto _LL7A;_LL79:({void*_tmp18C=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp18D="Toc::typ_to_c: type translation passed an effect";
_tag_dyneither(_tmp18D,sizeof(char),49);}),_tag_dyneither(_tmp18C,sizeof(void*),
0));});_LL7A: if(_tmp141 <= (void*)4)goto _LL41;if(*((int*)_tmp141)!= 17)goto _LL41;
_LL7B:({void*_tmp18E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp18F="Toc::typ_to_c: type translation passed a valueof_t";
_tag_dyneither(_tmp18F,sizeof(char),51);}),_tag_dyneither(_tmp18E,sizeof(void*),
0));});_LL41:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp190=t;struct Cyc_Absyn_ArrayInfo
_tmp191;void*_tmp192;struct Cyc_Absyn_Tqual _tmp193;_LL92: if(_tmp190 <= (void*)4)
goto _LL94;if(*((int*)_tmp190)!= 7)goto _LL94;_tmp191=((struct Cyc_Absyn_ArrayType_struct*)
_tmp190)->f1;_tmp192=_tmp191.elt_type;_tmp193=_tmp191.tq;_LL93: return Cyc_Toc_cast_it(
Cyc_Absyn_star_typ(_tmp192,(void*)2,_tmp193,Cyc_Absyn_false_conref),e);_LL94:;
_LL95: return Cyc_Toc_cast_it(t,e);_LL91:;}static int Cyc_Toc_atomic_typ(void*t){
void*_tmp194=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp195;void*
_tmp196;struct Cyc_Absyn_AggrInfo _tmp197;union Cyc_Absyn_AggrInfoU _tmp198;struct
Cyc_List_List*_tmp199;struct Cyc_Absyn_DatatypeFieldInfo _tmp19A;union Cyc_Absyn_DatatypeFieldInfoU
_tmp19B;struct _tuple2 _tmp19C;struct Cyc_Absyn_Datatypedecl*_tmp19D;struct Cyc_Absyn_Datatypefield*
_tmp19E;struct Cyc_List_List*_tmp19F;_LL97: if((int)_tmp194 != 0)goto _LL99;_LL98:
return 1;_LL99: if(_tmp194 <= (void*)4)goto _LLA1;if(*((int*)_tmp194)!= 1)goto _LL9B;
_LL9A: return 0;_LL9B: if(*((int*)_tmp194)!= 5)goto _LL9D;_LL9C: goto _LL9E;_LL9D: if(*((
int*)_tmp194)!= 12)goto _LL9F;_LL9E: goto _LLA0;_LL9F: if(*((int*)_tmp194)!= 13)goto
_LLA1;_LLA0: goto _LLA2;_LLA1: if((int)_tmp194 != 1)goto _LLA3;_LLA2: goto _LLA4;_LLA3:
if(_tmp194 <= (void*)4)goto _LLB9;if(*((int*)_tmp194)!= 6)goto _LLA5;_LLA4: goto
_LLA6;_LLA5: if(*((int*)_tmp194)!= 8)goto _LLA7;_LLA6: goto _LLA8;_LLA7: if(*((int*)
_tmp194)!= 18)goto _LLA9;_LLA8: return 1;_LLA9: if(*((int*)_tmp194)!= 7)goto _LLAB;
_tmp195=((struct Cyc_Absyn_ArrayType_struct*)_tmp194)->f1;_tmp196=_tmp195.elt_type;
_LLAA: return Cyc_Toc_atomic_typ(_tmp196);_LLAB: if(*((int*)_tmp194)!= 10)goto _LLAD;
_tmp197=((struct Cyc_Absyn_AggrType_struct*)_tmp194)->f1;_tmp198=_tmp197.aggr_info;
_LLAC:{union Cyc_Absyn_AggrInfoU _tmp1A0=_tmp198;struct _tuple3 _tmp1A1;_LLBC: if((
_tmp1A0.UnknownAggr).tag != 1)goto _LLBE;_tmp1A1=(struct _tuple3)(_tmp1A0.UnknownAggr).val;
_LLBD: return 0;_LLBE:;_LLBF: goto _LLBB;_LLBB:;}{struct Cyc_Absyn_Aggrdecl*_tmp1A2=
Cyc_Absyn_get_known_aggrdecl(_tmp198);if(_tmp1A2->impl == 0)return 0;{struct Cyc_List_List*
_tmp1A3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1A2->impl))->fields;for(
0;_tmp1A3 != 0;_tmp1A3=_tmp1A3->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp1A3->hd)->type))return 0;}}return 1;}_LLAD: if(*((int*)_tmp194)!= 11)goto _LLAF;
_tmp199=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp194)->f2;_LLAE: for(0;_tmp199
!= 0;_tmp199=_tmp199->tl){if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)
_tmp199->hd)->type))return 0;}return 1;_LLAF: if(*((int*)_tmp194)!= 3)goto _LLB1;
_tmp19A=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp194)->f1;_tmp19B=_tmp19A.field_info;
if((_tmp19B.KnownDatatypefield).tag != 2)goto _LLB1;_tmp19C=(struct _tuple2)(
_tmp19B.KnownDatatypefield).val;_tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;_LLB0:
_tmp19F=_tmp19E->typs;goto _LLB2;_LLB1: if(*((int*)_tmp194)!= 9)goto _LLB3;_tmp19F=((
struct Cyc_Absyn_TupleType_struct*)_tmp194)->f1;_LLB2: for(0;_tmp19F != 0;_tmp19F=
_tmp19F->tl){if(!Cyc_Toc_atomic_typ((*((struct _tuple10*)_tmp19F->hd)).f2))return
0;}return 1;_LLB3: if(*((int*)_tmp194)!= 2)goto _LLB5;_LLB4: goto _LLB6;_LLB5: if(*((
int*)_tmp194)!= 4)goto _LLB7;_LLB6: goto _LLB8;_LLB7: if(*((int*)_tmp194)!= 14)goto
_LLB9;_LLB8: return 0;_LLB9:;_LLBA:({struct Cyc_String_pa_struct _tmp1A6;_tmp1A6.tag=
0;_tmp1A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp1A4[1]={& _tmp1A6};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1A5="atomic_typ:  bad type %s";
_tag_dyneither(_tmp1A5,sizeof(char),25);}),_tag_dyneither(_tmp1A4,sizeof(void*),
1));}});_LL96:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp1A7=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1A8;void*_tmp1A9;_LLC1: if(_tmp1A7 <= (void*)4)goto
_LLC3;if(*((int*)_tmp1A7)!= 4)goto _LLC3;_tmp1A8=((struct Cyc_Absyn_PointerType_struct*)
_tmp1A7)->f1;_tmp1A9=_tmp1A8.elt_typ;_LLC2: {void*_tmp1AA=Cyc_Tcutil_compress(
_tmp1A9);_LLC6: if((int)_tmp1AA != 0)goto _LLC8;_LLC7: return 1;_LLC8:;_LLC9: return 0;
_LLC5:;}_LLC3:;_LLC4: return 0;_LLC0:;}static int Cyc_Toc_is_tvar(void*t){void*
_tmp1AB=Cyc_Tcutil_compress(t);_LLCB: if(_tmp1AB <= (void*)4)goto _LLCD;if(*((int*)
_tmp1AB)!= 1)goto _LLCD;_LLCC: return 1;_LLCD:;_LLCE: return 0;_LLCA:;}static int Cyc_Toc_is_void_star_or_tvar(
void*t){return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_tvar(t);}static int Cyc_Toc_is_poly_field(
void*t,struct _dyneither_ptr*f){void*_tmp1AC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1AD;union Cyc_Absyn_AggrInfoU _tmp1AE;struct Cyc_List_List*_tmp1AF;_LLD0: if(
_tmp1AC <= (void*)4)goto _LLD4;if(*((int*)_tmp1AC)!= 10)goto _LLD2;_tmp1AD=((struct
Cyc_Absyn_AggrType_struct*)_tmp1AC)->f1;_tmp1AE=_tmp1AD.aggr_info;_LLD1: {struct
Cyc_Absyn_Aggrdecl*_tmp1B0=Cyc_Absyn_get_known_aggrdecl(_tmp1AE);if(_tmp1B0->impl
== 0)({void*_tmp1B1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1B2="is_poly_field: type missing fields";
_tag_dyneither(_tmp1B2,sizeof(char),35);}),_tag_dyneither(_tmp1B1,sizeof(void*),
0));});_tmp1AF=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1B0->impl))->fields;
goto _LLD3;}_LLD2: if(*((int*)_tmp1AC)!= 11)goto _LLD4;_tmp1AF=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1AC)->f2;_LLD3: {struct Cyc_Absyn_Aggrfield*_tmp1B3=Cyc_Absyn_lookup_field(
_tmp1AF,f);if(_tmp1B3 == 0)({struct Cyc_String_pa_struct _tmp1B6;_tmp1B6.tag=0;
_tmp1B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp1B4[1]={&
_tmp1B6};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1B5="is_poly_field: bad field %s";_tag_dyneither(_tmp1B5,sizeof(
char),28);}),_tag_dyneither(_tmp1B4,sizeof(void*),1));}});return Cyc_Toc_is_void_star_or_tvar(
_tmp1B3->type);}_LLD4:;_LLD5:({struct Cyc_String_pa_struct _tmp1B9;_tmp1B9.tag=0;
_tmp1B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{
void*_tmp1B7[1]={& _tmp1B9};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(({const char*_tmp1B8="is_poly_field: bad type %s";
_tag_dyneither(_tmp1B8,sizeof(char),27);}),_tag_dyneither(_tmp1B7,sizeof(void*),
1));}});_LLCF:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*
_tmp1BA=e->r;struct Cyc_Absyn_Exp*_tmp1BB;struct _dyneither_ptr*_tmp1BC;struct Cyc_Absyn_Exp*
_tmp1BD;struct _dyneither_ptr*_tmp1BE;_LLD7: if(*((int*)_tmp1BA)!= 23)goto _LLD9;
_tmp1BB=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1BA)->f1;_tmp1BC=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp1BA)->f2;_LLD8: return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1BB->topt))->v,_tmp1BC);_LLD9: if(*((int*)_tmp1BA)!= 24)goto _LLDB;
_tmp1BD=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1BA)->f1;_tmp1BE=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1BA)->f2;_LLDA: {void*_tmp1BF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1BD->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1C0;void*_tmp1C1;
_LLDE: if(_tmp1BF <= (void*)4)goto _LLE0;if(*((int*)_tmp1BF)!= 4)goto _LLE0;_tmp1C0=((
struct Cyc_Absyn_PointerType_struct*)_tmp1BF)->f1;_tmp1C1=_tmp1C0.elt_typ;_LLDF:
return Cyc_Toc_is_poly_field(_tmp1C1,_tmp1BE);_LLE0:;_LLE1:({struct Cyc_String_pa_struct
_tmp1C4;_tmp1C4.tag=0;_tmp1C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1BD->topt))->v));{
void*_tmp1C2[1]={& _tmp1C4};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(({const char*_tmp1C3="is_poly_project: bad type %s";
_tag_dyneither(_tmp1C3,sizeof(char),29);}),_tag_dyneither(_tmp1C2,sizeof(void*),
1));}});_LLDD:;}_LLDB:;_LLDC: return 0;_LLD6:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(
struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct
Cyc_List_List*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->hd=s;_tmp1C5->tl=0;
_tmp1C5;}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*
_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->hd=s;_tmp1C6->tl=0;_tmp1C6;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(
Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(s);}
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*
_tmp1C7[2];_tmp1C7[1]=s;_tmp1C7[0]=rgn;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C7,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*
s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*_tmp1C8[2];_tmp1C8[1]=n;
_tmp1C8[0]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1C8,sizeof(struct Cyc_Absyn_Exp*),2));}),0);else{return Cyc_Absyn_fncall_exp(
Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp1C9[2];_tmp1C9[1]=n;_tmp1C9[0]=s;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*_tmp1CA[3];_tmp1CA[2]=n;_tmp1CA[1]=
s;_tmp1CA[0]=rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp1CA,sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({
struct Cyc_List_List*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->hd=e;_tmp1CB->tl=
0;_tmp1CB;}),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){static
struct Cyc_Absyn_Stmt**throw_match_stmt_opt=0;if(throw_match_stmt_opt == 0)
throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp1CC=_cycalloc(sizeof(*_tmp1CC));
_tmp1CC[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(0)),0);
_tmp1CC;});return*throw_match_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*
_tmp1CD=e->r;union Cyc_Absyn_Cnst _tmp1CE;struct _dyneither_ptr _tmp1CF;_LLE3: if(*((
int*)_tmp1CD)!= 0)goto _LLE5;_tmp1CE=((struct Cyc_Absyn_Const_e_struct*)_tmp1CD)->f1;
if((_tmp1CE.String_c).tag != 7)goto _LLE5;_tmp1CF=(struct _dyneither_ptr)(_tmp1CE.String_c).val;
_LLE4: is_string=1;goto _LLE2;_LLE5:;_LLE6: goto _LLE2;_LLE2:;}{struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp1D0=_cycalloc(sizeof(*_tmp1D0));_tmp1D0->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1[0]=({
struct Cyc_Absyn_Var_d_struct _tmp1D2;_tmp1D2.tag=0;_tmp1D2.f1=vd;_tmp1D2;});
_tmp1D1;}),0);_tmp1D0->tl=Cyc_Toc_result_decls;_tmp1D0;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple16*_tmp1D3[3];_tmp1D3[2]=({struct _tuple16*_tmp1D6=_cycalloc(sizeof(*
_tmp1D6));_tmp1D6->f1=0;_tmp1D6->f2=xplussz;_tmp1D6;});_tmp1D3[1]=({struct
_tuple16*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->f1=0;_tmp1D5->f2=xexp;
_tmp1D5;});_tmp1D3[0]=({struct _tuple16*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));
_tmp1D4->f1=0;_tmp1D4->f2=xexp;_tmp1D4;});((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D3,sizeof(struct _tuple16*),3));}),
0);return urm_exp;}}}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct
Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{
struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;struct _RegionHandle*rgn;};
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp1D8;int
_tmp1D9;struct Cyc_Toc_Env*_tmp1D7=nv;_tmp1D8=*_tmp1D7;_tmp1D9=_tmp1D8.toplevel;
return _tmp1D9;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*
nv,struct _tuple1*x){struct Cyc_Toc_Env _tmp1DB;struct Cyc_Dict_Dict _tmp1DC;struct
Cyc_Toc_Env*_tmp1DA=nv;_tmp1DB=*_tmp1DA;_tmp1DC=_tmp1DB.varmap;return((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp1DC,x);}static struct
Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*
_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));_tmp1DD->break_lab=(struct
_dyneither_ptr**)0;_tmp1DD->continue_lab=(struct _dyneither_ptr**)0;_tmp1DD->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp1DD->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp1DD->toplevel=(int)1;_tmp1DD->rgn=(struct _RegionHandle*)
r;_tmp1DD;});}static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp1DF;struct _dyneither_ptr**_tmp1E0;
struct _dyneither_ptr**_tmp1E1;struct Cyc_Toc_FallthruInfo*_tmp1E2;struct Cyc_Dict_Dict
_tmp1E3;int _tmp1E4;struct Cyc_Toc_Env*_tmp1DE=e;_tmp1DF=*_tmp1DE;_tmp1E0=_tmp1DF.break_lab;
_tmp1E1=_tmp1DF.continue_lab;_tmp1E2=_tmp1DF.fallthru_info;_tmp1E3=_tmp1DF.varmap;
_tmp1E4=_tmp1DF.toplevel;return({struct Cyc_Toc_Env*_tmp1E5=_region_malloc(r,
sizeof(*_tmp1E5));_tmp1E5->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp1E0);_tmp1E5->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp1E1);_tmp1E5->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1E2;_tmp1E5->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1E3);_tmp1E5->toplevel=(
int)_tmp1E4;_tmp1E5->rgn=(struct _RegionHandle*)r;_tmp1E5;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp1E7;struct _dyneither_ptr**_tmp1E8;struct _dyneither_ptr**_tmp1E9;struct Cyc_Toc_FallthruInfo*
_tmp1EA;struct Cyc_Dict_Dict _tmp1EB;int _tmp1EC;struct Cyc_Toc_Env*_tmp1E6=e;
_tmp1E7=*_tmp1E6;_tmp1E8=_tmp1E7.break_lab;_tmp1E9=_tmp1E7.continue_lab;_tmp1EA=
_tmp1E7.fallthru_info;_tmp1EB=_tmp1E7.varmap;_tmp1EC=_tmp1E7.toplevel;return({
struct Cyc_Toc_Env*_tmp1ED=_region_malloc(r,sizeof(*_tmp1ED));_tmp1ED->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp1E8);_tmp1ED->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp1E9);_tmp1ED->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp1EA;_tmp1ED->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp1EB);_tmp1ED->toplevel=(int)0;_tmp1ED->rgn=(struct _RegionHandle*)r;_tmp1ED;});}
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*
e){struct Cyc_Toc_Env _tmp1EF;struct _dyneither_ptr**_tmp1F0;struct _dyneither_ptr**
_tmp1F1;struct Cyc_Toc_FallthruInfo*_tmp1F2;struct Cyc_Dict_Dict _tmp1F3;int _tmp1F4;
struct Cyc_Toc_Env*_tmp1EE=e;_tmp1EF=*_tmp1EE;_tmp1F0=_tmp1EF.break_lab;_tmp1F1=
_tmp1EF.continue_lab;_tmp1F2=_tmp1EF.fallthru_info;_tmp1F3=_tmp1EF.varmap;
_tmp1F4=_tmp1EF.toplevel;return({struct Cyc_Toc_Env*_tmp1F5=_region_malloc(r,
sizeof(*_tmp1F5));_tmp1F5->break_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp1F0);_tmp1F5->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp1F1);_tmp1F5->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1F2;_tmp1F5->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp1F3);_tmp1F5->toplevel=(
int)1;_tmp1F5->rgn=(struct _RegionHandle*)r;_tmp1F5;});}static struct Cyc_Toc_Env*
Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,
struct Cyc_Absyn_Exp*y){{union Cyc_Absyn_Nmspace _tmp1F6=(*x).f1;struct Cyc_List_List*
_tmp1F7;_LLE8: if((_tmp1F6.Rel_n).tag != 1)goto _LLEA;_tmp1F7=(struct Cyc_List_List*)(
_tmp1F6.Rel_n).val;_LLE9:({struct Cyc_String_pa_struct _tmp1FA;_tmp1FA.tag=0;
_tmp1FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
x));{void*_tmp1F8[1]={& _tmp1FA};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1F9="Toc::add_varmap on Rel_n: %s\n";
_tag_dyneither(_tmp1F9,sizeof(char),30);}),_tag_dyneither(_tmp1F8,sizeof(void*),
1));}});_LLEA:;_LLEB: goto _LLE7;_LLE7:;}{struct Cyc_Toc_Env _tmp1FC;struct
_dyneither_ptr**_tmp1FD;struct _dyneither_ptr**_tmp1FE;struct Cyc_Toc_FallthruInfo*
_tmp1FF;struct Cyc_Dict_Dict _tmp200;int _tmp201;struct Cyc_Toc_Env*_tmp1FB=e;
_tmp1FC=*_tmp1FB;_tmp1FD=_tmp1FC.break_lab;_tmp1FE=_tmp1FC.continue_lab;_tmp1FF=
_tmp1FC.fallthru_info;_tmp200=_tmp1FC.varmap;_tmp201=_tmp1FC.toplevel;{struct Cyc_Dict_Dict
_tmp202=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp200),x,y);return({struct Cyc_Toc_Env*_tmp203=
_region_malloc(r,sizeof(*_tmp203));_tmp203->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp1FD);_tmp203->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp1FE);_tmp203->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp1FF;_tmp203->varmap=(struct Cyc_Dict_Dict)_tmp202;_tmp203->toplevel=(int)
_tmp201;_tmp203->rgn=(struct _RegionHandle*)r;_tmp203;});}}}static struct Cyc_Toc_Env*
Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp205;struct _dyneither_ptr**_tmp206;struct _dyneither_ptr**_tmp207;struct Cyc_Toc_FallthruInfo*
_tmp208;struct Cyc_Dict_Dict _tmp209;int _tmp20A;struct Cyc_Toc_Env*_tmp204=e;
_tmp205=*_tmp204;_tmp206=_tmp205.break_lab;_tmp207=_tmp205.continue_lab;_tmp208=
_tmp205.fallthru_info;_tmp209=_tmp205.varmap;_tmp20A=_tmp205.toplevel;return({
struct Cyc_Toc_Env*_tmp20B=_region_malloc(r,sizeof(*_tmp20B));_tmp20B->break_lab=(
struct _dyneither_ptr**)0;_tmp20B->continue_lab=(struct _dyneither_ptr**)0;_tmp20B->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp208;_tmp20B->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp209);_tmp20B->toplevel=(int)_tmp20A;_tmp20B->rgn=(struct _RegionHandle*)r;
_tmp20B;});}static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct
_dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*
next_case_env){struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;
fallthru_binders=fallthru_binders->tl){fallthru_vars=({struct Cyc_List_List*
_tmp20C=_region_malloc(r,sizeof(*_tmp20C));_tmp20C->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name;_tmp20C->tl=fallthru_vars;_tmp20C;});}fallthru_vars=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct Cyc_Toc_Env _tmp20E;struct _dyneither_ptr**_tmp20F;struct _dyneither_ptr**
_tmp210;struct Cyc_Toc_FallthruInfo*_tmp211;struct Cyc_Dict_Dict _tmp212;int _tmp213;
struct Cyc_Toc_Env*_tmp20D=e;_tmp20E=*_tmp20D;_tmp20F=_tmp20E.break_lab;_tmp210=
_tmp20E.continue_lab;_tmp211=_tmp20E.fallthru_info;_tmp212=_tmp20E.varmap;
_tmp213=_tmp20E.toplevel;{struct Cyc_Toc_Env _tmp215;struct Cyc_Dict_Dict _tmp216;
struct Cyc_Toc_Env*_tmp214=next_case_env;_tmp215=*_tmp214;_tmp216=_tmp215.varmap;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp219=
_region_malloc(r,sizeof(*_tmp219));_tmp219->label=fallthru_l;_tmp219->binders=
fallthru_vars;_tmp219->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp216);_tmp219;});
return({struct Cyc_Toc_Env*_tmp217=_region_malloc(r,sizeof(*_tmp217));_tmp217->break_lab=(
struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp218=_region_malloc(r,sizeof(*
_tmp218));_tmp218[0]=break_l;_tmp218;});_tmp217->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp210);_tmp217->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp217->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp212);_tmp217->toplevel=(
int)_tmp213;_tmp217->rgn=(struct _RegionHandle*)r;_tmp217;});}}}}static struct Cyc_Toc_Env*
Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct
_dyneither_ptr*break_l){struct Cyc_Toc_Env _tmp21B;struct _dyneither_ptr**_tmp21C;
struct _dyneither_ptr**_tmp21D;struct Cyc_Toc_FallthruInfo*_tmp21E;struct Cyc_Dict_Dict
_tmp21F;int _tmp220;struct Cyc_Toc_Env*_tmp21A=e;_tmp21B=*_tmp21A;_tmp21C=_tmp21B.break_lab;
_tmp21D=_tmp21B.continue_lab;_tmp21E=_tmp21B.fallthru_info;_tmp21F=_tmp21B.varmap;
_tmp220=_tmp21B.toplevel;return({struct Cyc_Toc_Env*_tmp221=_region_malloc(r,
sizeof(*_tmp221));_tmp221->break_lab=(struct _dyneither_ptr**)({struct
_dyneither_ptr**_tmp222=_region_malloc(r,sizeof(*_tmp222));_tmp222[0]=break_l;
_tmp222;});_tmp221->continue_lab=(struct _dyneither_ptr**)((struct _dyneither_ptr**)
_tmp21D);_tmp221->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0;_tmp221->varmap=(
struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp21F);_tmp221->toplevel=(int)_tmp220;_tmp221->rgn=(struct
_RegionHandle*)r;_tmp221;});}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp224;struct _dyneither_ptr**_tmp225;struct _dyneither_ptr**_tmp226;struct Cyc_Toc_FallthruInfo*
_tmp227;struct Cyc_Dict_Dict _tmp228;int _tmp229;struct Cyc_Toc_Env*_tmp223=e;
_tmp224=*_tmp223;_tmp225=_tmp224.break_lab;_tmp226=_tmp224.continue_lab;_tmp227=
_tmp224.fallthru_info;_tmp228=_tmp224.varmap;_tmp229=_tmp224.toplevel;return({
struct Cyc_Toc_Env*_tmp22A=_region_malloc(r,sizeof(*_tmp22A));_tmp22A->break_lab=(
struct _dyneither_ptr**)0;_tmp22A->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp226);_tmp22A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({
struct Cyc_Toc_FallthruInfo*_tmp22B=_region_malloc(r,sizeof(*_tmp22B));_tmp22B->label=
next_l;_tmp22B->binders=0;_tmp22B->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);
_tmp22B;});_tmp22A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp228);_tmp22A->toplevel=(
int)_tmp229;_tmp22A->rgn=(struct _RegionHandle*)r;_tmp22A;});}static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct
Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct
Cyc_Absyn_Exp*e){void*_tmp22C=e->annot;_LLED: if(*((void**)_tmp22C)!= Cyc_CfFlowInfo_UnknownZ)
goto _LLEF;_LLEE: return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LLEF: if(*((void**)_tmp22C)!= Cyc_CfFlowInfo_NotZero)
goto _LLF1;_LLF0: return 0;_LLF1: if(_tmp22C != Cyc_CfFlowInfo_IsZero)goto _LLF3;_LLF2:({
void*_tmp22D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp22E="dereference of NULL pointer";
_tag_dyneither(_tmp22E,sizeof(char),28);}),_tag_dyneither(_tmp22D,sizeof(void*),
0));});return 0;_LLF3: if(_tmp22C != Cyc_Absyn_EmptyAnnot)goto _LLF5;_LLF4: return 0;
_LLF5: if(*((void**)_tmp22C)!= Cyc_CfFlowInfo_HasTagCmps)goto _LLF7;_LLF6:({void*
_tmp22F=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp230="compiler oddity: pointer compared to tag type";
_tag_dyneither(_tmp230,sizeof(char),46);}),_tag_dyneither(_tmp22F,sizeof(void*),
0));});return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LLF7:;_LLF8:({void*_tmp231=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp232="need_null_check";
_tag_dyneither(_tmp232,sizeof(char),16);}),_tag_dyneither(_tmp231,sizeof(void*),
0));});_LLEC:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp233=e->annot;struct Cyc_List_List*_tmp234;struct Cyc_List_List*_tmp235;
_LLFA: if(*((void**)_tmp233)!= Cyc_CfFlowInfo_UnknownZ)goto _LLFC;_tmp234=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp233)->f1;_LLFB: return _tmp234;_LLFC: if(*((
void**)_tmp233)!= Cyc_CfFlowInfo_NotZero)goto _LLFE;_tmp235=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp233)->f1;_LLFD: return _tmp235;_LLFE: if(_tmp233 != Cyc_CfFlowInfo_IsZero)goto
_LL100;_LLFF:({void*_tmp236=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp237="dereference of NULL pointer";
_tag_dyneither(_tmp237,sizeof(char),28);}),_tag_dyneither(_tmp236,sizeof(void*),
0));});return 0;_LL100: if(*((void**)_tmp233)!= Cyc_CfFlowInfo_HasTagCmps)goto
_LL102;_LL101: goto _LL103;_LL102: if(_tmp233 != Cyc_Absyn_EmptyAnnot)goto _LL104;
_LL103: return 0;_LL104:;_LL105:({void*_tmp238=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp239="get_relns";
_tag_dyneither(_tmp239,sizeof(char),10);}),_tag_dyneither(_tmp238,sizeof(void*),
0));});_LLF9:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t){void*
_tmp23A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp23B;struct Cyc_Absyn_PtrAtts
_tmp23C;union Cyc_Absyn_Constraint*_tmp23D;union Cyc_Absyn_Constraint*_tmp23E;
struct Cyc_Absyn_ArrayInfo _tmp23F;struct Cyc_Absyn_Exp*_tmp240;_LL107: if(_tmp23A <= (
void*)4)goto _LL10B;if(*((int*)_tmp23A)!= 4)goto _LL109;_tmp23B=((struct Cyc_Absyn_PointerType_struct*)
_tmp23A)->f1;_tmp23C=_tmp23B.ptr_atts;_tmp23D=_tmp23C.bounds;_tmp23E=_tmp23C.zero_term;
_LL108: {void*_tmp241=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp23D);struct
Cyc_Absyn_Exp*_tmp242;_LL10E: if((int)_tmp241 != 0)goto _LL110;_LL10F: return 0;
_LL110: if(_tmp241 <= (void*)1)goto _LL10D;if(*((int*)_tmp241)!= 0)goto _LL10D;
_tmp242=((struct Cyc_Absyn_Upper_b_struct*)_tmp241)->f1;_LL111: {unsigned int
_tmp244;int _tmp245;struct _tuple11 _tmp243=Cyc_Evexp_eval_const_uint_exp(_tmp242);
_tmp244=_tmp243.f1;_tmp245=_tmp243.f2;return _tmp245  && i <= _tmp244;}_LL10D:;}
_LL109: if(*((int*)_tmp23A)!= 7)goto _LL10B;_tmp23F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp23A)->f1;_tmp240=_tmp23F.num_elts;_LL10A: if(_tmp240 == 0)return 0;{
unsigned int _tmp247;int _tmp248;struct _tuple11 _tmp246=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp240);_tmp247=_tmp246.f1;_tmp248=_tmp246.f2;return
_tmp248  && i <= _tmp247;}_LL10B:;_LL10C: return 0;_LL106:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp249=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp249->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp _tmp24A=_tmp249->rop;
struct Cyc_Absyn_Vardecl*_tmp24B;struct Cyc_Absyn_Vardecl*_tmp24C;_LL113: if((
_tmp24A.LessNumelts).tag != 3)goto _LL115;_tmp24B=(struct Cyc_Absyn_Vardecl*)(
_tmp24A.LessNumelts).val;_LL114: _tmp24C=_tmp24B;goto _LL116;_LL115: if((_tmp24A.LessEqNumelts).tag
!= 5)goto _LL117;_tmp24C=(struct Cyc_Absyn_Vardecl*)(_tmp24A.LessEqNumelts).val;
_LL116: if(_tmp24C == v)return 1;else{goto _LL112;}_LL117:;_LL118: continue;_LL112:;}}
return 0;}static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e){{void*_tmp24D=e->r;void*_tmp24E;struct Cyc_Absyn_Vardecl*
_tmp24F;void*_tmp250;struct Cyc_Absyn_Vardecl*_tmp251;void*_tmp252;struct Cyc_Absyn_Vardecl*
_tmp253;void*_tmp254;struct Cyc_Absyn_Vardecl*_tmp255;void*_tmp256;struct Cyc_List_List*
_tmp257;struct Cyc_List_List _tmp258;struct Cyc_Absyn_Exp*_tmp259;_LL11A: if(*((int*)
_tmp24D)!= 1)goto _LL11C;_tmp24E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp24D)->f2;
if(_tmp24E <= (void*)1)goto _LL11C;if(*((int*)_tmp24E)!= 4)goto _LL11C;_tmp24F=((
struct Cyc_Absyn_Pat_b_struct*)_tmp24E)->f1;_LL11B: _tmp251=_tmp24F;goto _LL11D;
_LL11C: if(*((int*)_tmp24D)!= 1)goto _LL11E;_tmp250=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24D)->f2;if(_tmp250 <= (void*)1)goto _LL11E;if(*((int*)_tmp250)!= 3)goto _LL11E;
_tmp251=((struct Cyc_Absyn_Local_b_struct*)_tmp250)->f1;_LL11D: _tmp253=_tmp251;
goto _LL11F;_LL11E: if(*((int*)_tmp24D)!= 1)goto _LL120;_tmp252=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24D)->f2;if(_tmp252 <= (void*)1)goto _LL120;if(*((int*)_tmp252)!= 0)goto _LL120;
_tmp253=((struct Cyc_Absyn_Global_b_struct*)_tmp252)->f1;_LL11F: _tmp255=_tmp253;
goto _LL121;_LL120: if(*((int*)_tmp24D)!= 1)goto _LL122;_tmp254=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp24D)->f2;if(_tmp254 <= (void*)1)goto _LL122;if(*((int*)_tmp254)!= 2)goto _LL122;
_tmp255=((struct Cyc_Absyn_Param_b_struct*)_tmp254)->f1;_LL121: if(_tmp255->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp255))return 1;goto _LL119;_LL122:
if(*((int*)_tmp24D)!= 3)goto _LL124;_tmp256=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp24D)->f1;if((int)_tmp256 != 19)goto _LL124;_tmp257=((struct Cyc_Absyn_Primop_e_struct*)
_tmp24D)->f2;if(_tmp257 == 0)goto _LL124;_tmp258=*_tmp257;_tmp259=(struct Cyc_Absyn_Exp*)
_tmp258.hd;_LL123:{void*_tmp25A=_tmp259->r;void*_tmp25B;struct Cyc_Absyn_Vardecl*
_tmp25C;void*_tmp25D;struct Cyc_Absyn_Vardecl*_tmp25E;void*_tmp25F;struct Cyc_Absyn_Vardecl*
_tmp260;void*_tmp261;struct Cyc_Absyn_Vardecl*_tmp262;_LL127: if(*((int*)_tmp25A)
!= 1)goto _LL129;_tmp25B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp25A)->f2;if(
_tmp25B <= (void*)1)goto _LL129;if(*((int*)_tmp25B)!= 4)goto _LL129;_tmp25C=((
struct Cyc_Absyn_Pat_b_struct*)_tmp25B)->f1;_LL128: _tmp25E=_tmp25C;goto _LL12A;
_LL129: if(*((int*)_tmp25A)!= 1)goto _LL12B;_tmp25D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp25A)->f2;if(_tmp25D <= (void*)1)goto _LL12B;if(*((int*)_tmp25D)!= 3)goto _LL12B;
_tmp25E=((struct Cyc_Absyn_Local_b_struct*)_tmp25D)->f1;_LL12A: _tmp260=_tmp25E;
goto _LL12C;_LL12B: if(*((int*)_tmp25A)!= 1)goto _LL12D;_tmp25F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp25A)->f2;if(_tmp25F <= (void*)1)goto _LL12D;if(*((int*)_tmp25F)!= 0)goto _LL12D;
_tmp260=((struct Cyc_Absyn_Global_b_struct*)_tmp25F)->f1;_LL12C: _tmp262=_tmp260;
goto _LL12E;_LL12D: if(*((int*)_tmp25A)!= 1)goto _LL12F;_tmp261=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp25A)->f2;if(_tmp261 <= (void*)1)goto _LL12F;if(*((int*)_tmp261)!= 2)goto _LL12F;
_tmp262=((struct Cyc_Absyn_Param_b_struct*)_tmp261)->f1;_LL12E: return _tmp262 == v;
_LL12F:;_LL130: goto _LL126;_LL126:;}goto _LL119;_LL124:;_LL125: goto _LL119;_LL119:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp263=a->r;void*_tmp264;struct Cyc_Absyn_Vardecl*
_tmp265;void*_tmp266;struct Cyc_Absyn_Vardecl*_tmp267;void*_tmp268;struct Cyc_Absyn_Vardecl*
_tmp269;void*_tmp26A;struct Cyc_Absyn_Vardecl*_tmp26B;_LL132: if(*((int*)_tmp263)
!= 1)goto _LL134;_tmp264=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp263)->f2;if(
_tmp264 <= (void*)1)goto _LL134;if(*((int*)_tmp264)!= 4)goto _LL134;_tmp265=((
struct Cyc_Absyn_Pat_b_struct*)_tmp264)->f1;_LL133: _tmp267=_tmp265;goto _LL135;
_LL134: if(*((int*)_tmp263)!= 1)goto _LL136;_tmp266=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp266 <= (void*)1)goto _LL136;if(*((int*)_tmp266)!= 3)goto _LL136;
_tmp267=((struct Cyc_Absyn_Local_b_struct*)_tmp266)->f1;_LL135: _tmp269=_tmp267;
goto _LL137;_LL136: if(*((int*)_tmp263)!= 1)goto _LL138;_tmp268=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp268 <= (void*)1)goto _LL138;if(*((int*)_tmp268)!= 0)goto _LL138;
_tmp269=((struct Cyc_Absyn_Global_b_struct*)_tmp268)->f1;_LL137: _tmp26B=_tmp269;
goto _LL139;_LL138: if(*((int*)_tmp263)!= 1)goto _LL13A;_tmp26A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp263)->f2;if(_tmp26A <= (void*)1)goto _LL13A;if(*((int*)_tmp26A)!= 2)goto _LL13A;
_tmp26B=((struct Cyc_Absyn_Param_b_struct*)_tmp26A)->f1;_LL139: if(_tmp26B->escapes)
return 0;inner_loop: {void*_tmp26C=i->r;void*_tmp26D;struct Cyc_Absyn_Exp*_tmp26E;
union Cyc_Absyn_Cnst _tmp26F;struct _tuple6 _tmp270;void*_tmp271;int _tmp272;union Cyc_Absyn_Cnst
_tmp273;struct _tuple6 _tmp274;void*_tmp275;int _tmp276;union Cyc_Absyn_Cnst _tmp277;
struct _tuple6 _tmp278;void*_tmp279;int _tmp27A;void*_tmp27B;struct Cyc_List_List*
_tmp27C;struct Cyc_List_List _tmp27D;struct Cyc_Absyn_Exp*_tmp27E;struct Cyc_List_List*
_tmp27F;struct Cyc_List_List _tmp280;struct Cyc_Absyn_Exp*_tmp281;void*_tmp282;
struct Cyc_Absyn_Vardecl*_tmp283;void*_tmp284;struct Cyc_Absyn_Vardecl*_tmp285;
void*_tmp286;struct Cyc_Absyn_Vardecl*_tmp287;void*_tmp288;struct Cyc_Absyn_Vardecl*
_tmp289;_LL13D: if(*((int*)_tmp26C)!= 15)goto _LL13F;_tmp26D=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp26C)->f1;_tmp26E=((struct Cyc_Absyn_Cast_e_struct*)_tmp26C)->f2;_LL13E: i=
_tmp26E;goto inner_loop;_LL13F: if(*((int*)_tmp26C)!= 0)goto _LL141;_tmp26F=((
struct Cyc_Absyn_Const_e_struct*)_tmp26C)->f1;if((_tmp26F.Int_c).tag != 4)goto
_LL141;_tmp270=(struct _tuple6)(_tmp26F.Int_c).val;_tmp271=_tmp270.f1;if((int)
_tmp271 != 2)goto _LL141;_tmp272=_tmp270.f2;_LL140: _tmp276=_tmp272;goto _LL142;
_LL141: if(*((int*)_tmp26C)!= 0)goto _LL143;_tmp273=((struct Cyc_Absyn_Const_e_struct*)
_tmp26C)->f1;if((_tmp273.Int_c).tag != 4)goto _LL143;_tmp274=(struct _tuple6)(
_tmp273.Int_c).val;_tmp275=_tmp274.f1;if((int)_tmp275 != 0)goto _LL143;_tmp276=
_tmp274.f2;_LL142: return _tmp276 >= 0  && Cyc_Toc_check_const_array((unsigned int)(
_tmp276 + 1),_tmp26B->type);_LL143: if(*((int*)_tmp26C)!= 0)goto _LL145;_tmp277=((
struct Cyc_Absyn_Const_e_struct*)_tmp26C)->f1;if((_tmp277.Int_c).tag != 4)goto
_LL145;_tmp278=(struct _tuple6)(_tmp277.Int_c).val;_tmp279=_tmp278.f1;if((int)
_tmp279 != 1)goto _LL145;_tmp27A=_tmp278.f2;_LL144: return Cyc_Toc_check_const_array((
unsigned int)(_tmp27A + 1),_tmp26B->type);_LL145: if(*((int*)_tmp26C)!= 3)goto
_LL147;_tmp27B=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp26C)->f1;if((int)
_tmp27B != 4)goto _LL147;_tmp27C=((struct Cyc_Absyn_Primop_e_struct*)_tmp26C)->f2;
if(_tmp27C == 0)goto _LL147;_tmp27D=*_tmp27C;_tmp27E=(struct Cyc_Absyn_Exp*)_tmp27D.hd;
_tmp27F=_tmp27D.tl;if(_tmp27F == 0)goto _LL147;_tmp280=*_tmp27F;_tmp281=(struct Cyc_Absyn_Exp*)
_tmp280.hd;_LL146: return Cyc_Toc_check_leq_size(relns,_tmp26B,_tmp281);_LL147: if(*((
int*)_tmp26C)!= 1)goto _LL149;_tmp282=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp282 <= (void*)1)goto _LL149;if(*((int*)_tmp282)!= 4)goto _LL149;
_tmp283=((struct Cyc_Absyn_Pat_b_struct*)_tmp282)->f1;_LL148: _tmp285=_tmp283;goto
_LL14A;_LL149: if(*((int*)_tmp26C)!= 1)goto _LL14B;_tmp284=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp284 <= (void*)1)goto _LL14B;if(*((int*)_tmp284)!= 3)goto _LL14B;
_tmp285=((struct Cyc_Absyn_Local_b_struct*)_tmp284)->f1;_LL14A: _tmp287=_tmp285;
goto _LL14C;_LL14B: if(*((int*)_tmp26C)!= 1)goto _LL14D;_tmp286=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp286 <= (void*)1)goto _LL14D;if(*((int*)_tmp286)!= 0)goto _LL14D;
_tmp287=((struct Cyc_Absyn_Global_b_struct*)_tmp286)->f1;_LL14C: _tmp289=_tmp287;
goto _LL14E;_LL14D: if(*((int*)_tmp26C)!= 1)goto _LL14F;_tmp288=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp26C)->f2;if(_tmp288 <= (void*)1)goto _LL14F;if(*((int*)_tmp288)!= 2)goto _LL14F;
_tmp289=((struct Cyc_Absyn_Param_b_struct*)_tmp288)->f1;_LL14E: if(_tmp289->escapes)
return 0;{struct Cyc_List_List*_tmp28A=relns;for(0;_tmp28A != 0;_tmp28A=_tmp28A->tl){
struct Cyc_CfFlowInfo_Reln*_tmp28B=(struct Cyc_CfFlowInfo_Reln*)_tmp28A->hd;if(
_tmp28B->vd == _tmp289){union Cyc_CfFlowInfo_RelnOp _tmp28C=_tmp28B->rop;struct Cyc_Absyn_Vardecl*
_tmp28D;struct _tuple12 _tmp28E;struct Cyc_Absyn_Vardecl*_tmp28F;void*_tmp290;
unsigned int _tmp291;_LL152: if((_tmp28C.LessNumelts).tag != 3)goto _LL154;_tmp28D=(
struct Cyc_Absyn_Vardecl*)(_tmp28C.LessNumelts).val;_LL153: if(_tmp26B == _tmp28D)
return 1;else{goto _LL151;}_LL154: if((_tmp28C.LessVar).tag != 2)goto _LL156;_tmp28E=(
struct _tuple12)(_tmp28C.LessVar).val;_tmp28F=_tmp28E.f1;_tmp290=_tmp28E.f2;
_LL155:{struct _tuple0 _tmp293=({struct _tuple0 _tmp292;_tmp292.f1=Cyc_Tcutil_compress(
_tmp290);_tmp292.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
a->topt))->v);_tmp292;});void*_tmp294;void*_tmp295;void*_tmp296;struct Cyc_Absyn_PtrInfo
_tmp297;struct Cyc_Absyn_PtrAtts _tmp298;union Cyc_Absyn_Constraint*_tmp299;_LL15B:
_tmp294=_tmp293.f1;if(_tmp294 <= (void*)4)goto _LL15D;if(*((int*)_tmp294)!= 18)
goto _LL15D;_tmp295=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp294)->f1;_tmp296=
_tmp293.f2;if(_tmp296 <= (void*)4)goto _LL15D;if(*((int*)_tmp296)!= 4)goto _LL15D;
_tmp297=((struct Cyc_Absyn_PointerType_struct*)_tmp296)->f1;_tmp298=_tmp297.ptr_atts;
_tmp299=_tmp298.bounds;_LL15C:{void*_tmp29A=Cyc_Absyn_conref_val(_tmp299);struct
Cyc_Absyn_Exp*_tmp29B;_LL160: if(_tmp29A <= (void*)1)goto _LL162;if(*((int*)_tmp29A)
!= 0)goto _LL162;_tmp29B=((struct Cyc_Absyn_Upper_b_struct*)_tmp29A)->f1;_LL161: {
struct Cyc_Absyn_Exp*_tmp29C=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(
_tmp295,0),0,(void*)1,0);if(Cyc_Evexp_lte_const_exp(_tmp29C,_tmp29B))return 1;
goto _LL15F;}_LL162:;_LL163: goto _LL15F;_LL15F:;}goto _LL15A;_LL15D:;_LL15E: goto
_LL15A;_LL15A:;}{struct Cyc_List_List*_tmp29D=relns;for(0;_tmp29D != 0;_tmp29D=
_tmp29D->tl){struct Cyc_CfFlowInfo_Reln*_tmp29E=(struct Cyc_CfFlowInfo_Reln*)
_tmp29D->hd;if(_tmp29E->vd == _tmp28F){union Cyc_CfFlowInfo_RelnOp _tmp29F=_tmp29E->rop;
struct Cyc_Absyn_Vardecl*_tmp2A0;struct Cyc_Absyn_Vardecl*_tmp2A1;unsigned int
_tmp2A2;struct _tuple12 _tmp2A3;struct Cyc_Absyn_Vardecl*_tmp2A4;_LL165: if((_tmp29F.LessEqNumelts).tag
!= 5)goto _LL167;_tmp2A0=(struct Cyc_Absyn_Vardecl*)(_tmp29F.LessEqNumelts).val;
_LL166: _tmp2A1=_tmp2A0;goto _LL168;_LL167: if((_tmp29F.LessNumelts).tag != 3)goto
_LL169;_tmp2A1=(struct Cyc_Absyn_Vardecl*)(_tmp29F.LessNumelts).val;_LL168: if(
_tmp26B == _tmp2A1)return 1;goto _LL164;_LL169: if((_tmp29F.EqualConst).tag != 1)goto
_LL16B;_tmp2A2=(unsigned int)(_tmp29F.EqualConst).val;_LL16A: return Cyc_Toc_check_const_array(
_tmp2A2,_tmp26B->type);_LL16B: if((_tmp29F.LessVar).tag != 2)goto _LL16D;_tmp2A3=(
struct _tuple12)(_tmp29F.LessVar).val;_tmp2A4=_tmp2A3.f1;_LL16C: if(Cyc_Toc_check_leq_size_var(
relns,_tmp26B,_tmp2A4))return 1;goto _LL164;_LL16D:;_LL16E: goto _LL164;_LL164:;}}}
goto _LL151;_LL156: if((_tmp28C.LessConst).tag != 4)goto _LL158;_tmp291=(
unsigned int)(_tmp28C.LessConst).val;_LL157: return Cyc_Toc_check_const_array(
_tmp291,_tmp26B->type);_LL158:;_LL159: goto _LL151;_LL151:;}}}goto _LL13C;_LL14F:;
_LL150: goto _LL13C;_LL13C:;}goto _LL131;_LL13A:;_LL13B: goto _LL131;_LL131:;}return 0;}
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp2A5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2A6="Missing type in primop ";_tag_dyneither(_tmp2A6,sizeof(char),
24);}),_tag_dyneither(_tmp2A5,sizeof(void*),0));});return Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp2A7=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2A8="Missing type in primop ";
_tag_dyneither(_tmp2A8,sizeof(char),24);}),_tag_dyneither(_tmp2A7,sizeof(void*),
0));});return(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct
_tuple10*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){return({struct _tuple10*_tmp2A9=
_cycalloc(sizeof(*_tmp2A9));_tmp2A9->f1=Cyc_Toc_mt_tq;_tmp2A9->f2=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp2A9;});}static struct
_tuple16*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(
nv,e);return({struct _tuple16*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->f1=0;
_tmp2AA->f2=e;_tmp2AA;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct
Cyc_Toc_Env*nv,struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,
struct Cyc_Absyn_Exp*rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(
pointer){t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*
_tmp2AB=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);
if(rgnopt == 0  || Cyc_Absyn_no_regions)eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(
_tmp2AB): Cyc_Toc_malloc_ptr(_tmp2AB));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
rgnopt;Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,
_tmp2AB);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*
dles,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*
s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct
Cyc_List_List*_tmp2AC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
dles0);for(0;_tmp2AC != 0;_tmp2AC=_tmp2AC->tl){struct _tuple16 _tmp2AE;struct Cyc_List_List*
_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B0;struct _tuple16*_tmp2AD=(struct _tuple16*)
_tmp2AC->hd;_tmp2AE=*_tmp2AD;_tmp2AF=_tmp2AE.f1;_tmp2B0=_tmp2AE.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp2AF == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp2AF->tl != 0)({void*_tmp2B1=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp2B2="multiple designators in array";
_tag_dyneither(_tmp2B2,sizeof(char),30);}),_tag_dyneither(_tmp2B1,sizeof(void*),
0));});{void*_tmp2B3=(void*)_tmp2AF->hd;void*_tmp2B4=_tmp2B3;struct Cyc_Absyn_Exp*
_tmp2B5;_LL170: if(*((int*)_tmp2B4)!= 0)goto _LL172;_tmp2B5=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp2B4)->f1;_LL171: Cyc_Toc_exp_to_c(nv,_tmp2B5);e_index=_tmp2B5;goto _LL16F;
_LL172: if(*((int*)_tmp2B4)!= 1)goto _LL16F;_LL173:({void*_tmp2B6=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp2B7="field name designators in array";_tag_dyneither(_tmp2B7,sizeof(char),32);}),
_tag_dyneither(_tmp2B6,sizeof(void*),0));});_LL16F:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp2B8=_tmp2B0->r;struct Cyc_List_List*
_tmp2B9;struct Cyc_Absyn_Vardecl*_tmp2BA;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*
_tmp2BC;int _tmp2BD;void*_tmp2BE;struct Cyc_List_List*_tmp2BF;_LL175: if(*((int*)
_tmp2B8)!= 28)goto _LL177;_tmp2B9=((struct Cyc_Absyn_Array_e_struct*)_tmp2B8)->f1;
_LL176: s=Cyc_Toc_init_array(nv,lval,_tmp2B9,s);goto _LL174;_LL177: if(*((int*)
_tmp2B8)!= 29)goto _LL179;_tmp2BA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2B8)->f1;_tmp2BB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B8)->f2;
_tmp2BC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2B8)->f3;_tmp2BD=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2B8)->f4;_LL178: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2BA,_tmp2BB,_tmp2BC,_tmp2BD,s,0);goto _LL174;_LL179: if(*((int*)
_tmp2B8)!= 31)goto _LL17B;_tmp2BE=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2B8)->f1;_tmp2BF=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2B8)->f2;_LL17A:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2BE,_tmp2BF,s);goto _LL174;_LL17B:;_LL17C:
Cyc_Toc_exp_to_c(nv,_tmp2B0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp2B0,0),s,0);goto _LL174;_LL174:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp2C0=vd->name;
void*_tmp2C1=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct Cyc_Toc_Env _tmp2C3;
struct _RegionHandle*_tmp2C4;struct Cyc_Toc_Env*_tmp2C2=nv;_tmp2C3=*_tmp2C2;
_tmp2C4=_tmp2C3.rgn;{struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(_tmp2C4,nv,_tmp2C0,
Cyc_Absyn_varb_exp(_tmp2C0,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp2CD=
_cycalloc(sizeof(*_tmp2CD));_tmp2CD[0]=({struct Cyc_Absyn_Local_b_struct _tmp2CE;
_tmp2CE.tag=3;_tmp2CE.f1=vd;_tmp2CE;});_tmp2CD;}),0));struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp2C0,0),Cyc_Absyn_signed_int_exp(
0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp2C0,0),Cyc_Absyn_var_exp(
max,0),0);struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(
_tmp2C0,0),0);struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(
_tmp2C0,0),0);struct Cyc_Absyn_Stmt*body;{void*_tmp2C5=e2->r;struct Cyc_List_List*
_tmp2C6;struct Cyc_Absyn_Vardecl*_tmp2C7;struct Cyc_Absyn_Exp*_tmp2C8;struct Cyc_Absyn_Exp*
_tmp2C9;int _tmp2CA;void*_tmp2CB;struct Cyc_List_List*_tmp2CC;_LL17E: if(*((int*)
_tmp2C5)!= 28)goto _LL180;_tmp2C6=((struct Cyc_Absyn_Array_e_struct*)_tmp2C5)->f1;
_LL17F: body=Cyc_Toc_init_array(nv2,lval,_tmp2C6,Cyc_Toc_skip_stmt_dl());goto
_LL17D;_LL180: if(*((int*)_tmp2C5)!= 29)goto _LL182;_tmp2C7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2C5)->f1;_tmp2C8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2C5)->f2;
_tmp2C9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2C5)->f3;_tmp2CA=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2C5)->f4;_LL181: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp2C7,_tmp2C8,_tmp2C9,_tmp2CA,Cyc_Toc_skip_stmt_dl(),0);goto _LL17D;
_LL182: if(*((int*)_tmp2C5)!= 31)goto _LL184;_tmp2CB=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2C5)->f1;_tmp2CC=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2C5)->f2;_LL183:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp2CB,_tmp2CC,Cyc_Toc_skip_stmt_dl());
goto _LL17D;_LL184:;_LL185: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL17D;_LL17D:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(_tmp2C1,
Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}
return Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp2C0,Cyc_Absyn_uint_typ,0,s2,0),0),s,
0);}}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp2CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp2CF != 0;_tmp2CF=_tmp2CF->tl){struct _tuple16 _tmp2D1;
struct Cyc_List_List*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D3;struct _tuple16*_tmp2D0=(
struct _tuple16*)_tmp2CF->hd;_tmp2D1=*_tmp2D0;_tmp2D2=_tmp2D1.f1;_tmp2D3=_tmp2D1.f2;
if(_tmp2D2 == 0)({void*_tmp2D4=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2D5="empty designator list";
_tag_dyneither(_tmp2D5,sizeof(char),22);}),_tag_dyneither(_tmp2D4,sizeof(void*),
0));});if(_tmp2D2->tl != 0)({void*_tmp2D6=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2D7="too many designators in anonymous struct";
_tag_dyneither(_tmp2D7,sizeof(char),41);}),_tag_dyneither(_tmp2D6,sizeof(void*),
0));});{void*_tmp2D8=(void*)_tmp2D2->hd;struct _dyneither_ptr*_tmp2D9;_LL187: if(*((
int*)_tmp2D8)!= 1)goto _LL189;_tmp2D9=((struct Cyc_Absyn_FieldName_struct*)_tmp2D8)->f1;
_LL188: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp2D9,0);{void*
_tmp2DA=_tmp2D3->r;struct Cyc_List_List*_tmp2DB;struct Cyc_Absyn_Vardecl*_tmp2DC;
struct Cyc_Absyn_Exp*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DE;int _tmp2DF;void*_tmp2E0;
struct Cyc_List_List*_tmp2E1;_LL18C: if(*((int*)_tmp2DA)!= 28)goto _LL18E;_tmp2DB=((
struct Cyc_Absyn_Array_e_struct*)_tmp2DA)->f1;_LL18D: s=Cyc_Toc_init_array(nv,lval,
_tmp2DB,s);goto _LL18B;_LL18E: if(*((int*)_tmp2DA)!= 29)goto _LL190;_tmp2DC=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2DA)->f1;_tmp2DD=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2DA)->f2;_tmp2DE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2DA)->f3;
_tmp2DF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2DA)->f4;_LL18F: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2DC,_tmp2DD,_tmp2DE,_tmp2DF,s,0);goto _LL18B;_LL190: if(*((int*)
_tmp2DA)!= 31)goto _LL192;_tmp2E0=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2DA)->f1;_tmp2E1=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2DA)->f2;_LL191:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2E0,_tmp2E1,s);goto _LL18B;_LL192:;_LL193:
Cyc_Toc_exp_to_c(nv,_tmp2D3);if(Cyc_Toc_is_poly_field(struct_type,_tmp2D9))
_tmp2D3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp2D3);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp2D3,0),0),s,0);goto _LL18B;
_LL18B:;}goto _LL186;}_LL189:;_LL18A:({void*_tmp2E2=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2E3="array designator in struct";
_tag_dyneither(_tmp2E3,sizeof(char),27);}),_tag_dyneither(_tmp2E2,sizeof(void*),
0));});_LL186:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct
Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
struct Cyc_Toc_Env _tmp2E5;struct _RegionHandle*_tmp2E6;struct Cyc_Toc_Env*_tmp2E4=
nv;_tmp2E5=*_tmp2E4;_tmp2E6=_tmp2E5.rgn;{struct Cyc_List_List*_tmp2E7=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmp2E6,Cyc_Toc_tup_to_c,es);void*_tmp2E8=Cyc_Toc_add_tuple_type(
_tmp2E7);struct _tuple1*_tmp2E9=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp2EA=
Cyc_Absyn_var_exp(_tmp2E9,0);struct Cyc_Absyn_Stmt*_tmp2EB=Cyc_Absyn_exp_stmt(
_tmp2EA,0);struct Cyc_Absyn_Exp*(*_tmp2EC)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp2ED=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp2E6,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp2ED);for(0;_tmp2ED != 0;(_tmp2ED=
_tmp2ED->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp2ED->hd;struct
Cyc_Absyn_Exp*lval=_tmp2EC(_tmp2EA,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp2EE=e->r;struct Cyc_List_List*_tmp2EF;struct Cyc_Absyn_Vardecl*_tmp2F0;
struct Cyc_Absyn_Exp*_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F2;int _tmp2F3;_LL195: if(*((
int*)_tmp2EE)!= 28)goto _LL197;_tmp2EF=((struct Cyc_Absyn_Array_e_struct*)_tmp2EE)->f1;
_LL196: _tmp2EB=Cyc_Toc_init_array(nv,lval,_tmp2EF,_tmp2EB);goto _LL194;_LL197: if(*((
int*)_tmp2EE)!= 29)goto _LL199;_tmp2F0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2EE)->f1;_tmp2F1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2EE)->f2;
_tmp2F2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2EE)->f3;_tmp2F3=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2EE)->f4;_LL198: _tmp2EB=Cyc_Toc_init_comprehension(
nv,lval,_tmp2F0,_tmp2F1,_tmp2F2,_tmp2F3,_tmp2EB,0);goto _LL194;_LL199:;_LL19A: Cyc_Toc_exp_to_c(
nv,e);_tmp2EB=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp2EC(
_tmp2EA,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp2EB,0);goto _LL194;_LL194:;}}}
return Cyc_Toc_make_struct(nv,_tmp2E9,_tmp2E8,_tmp2EB,pointer,rgnopt,is_atomic);}}
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct
_dyneither_ptr*f){int i=1;{struct Cyc_List_List*_tmp2F4=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;for(0;_tmp2F4 != 0;_tmp2F4=_tmp2F4->tl){struct Cyc_Absyn_Aggrfield*
_tmp2F5=(struct Cyc_Absyn_Aggrfield*)_tmp2F4->hd;if(Cyc_strcmp((struct
_dyneither_ptr)*_tmp2F5->name,(struct _dyneither_ptr)*f)== 0)return i;++ i;}}({void*
_tmp2F6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2F9;_tmp2F9.tag=0;_tmp2F9.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp2F7[1]={& _tmp2F9};Cyc_aprintf(({
const char*_tmp2F8="get_member_offset %s failed";_tag_dyneither(_tmp2F8,sizeof(
char),28);}),_tag_dyneither(_tmp2F7,sizeof(void*),1));}}),_tag_dyneither(_tmp2F6,
sizeof(void*),0));});}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp2FA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2FB=Cyc_Absyn_var_exp(_tmp2FA,0);struct Cyc_Absyn_Stmt*
_tmp2FC=Cyc_Absyn_exp_stmt(_tmp2FB,0);struct Cyc_Absyn_Exp*(*_tmp2FD)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp2FE=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp2FF=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp300;union Cyc_Absyn_AggrInfoU _tmp301;
_LL19C: if(_tmp2FF <= (void*)4)goto _LL19E;if(*((int*)_tmp2FF)!= 10)goto _LL19E;
_tmp300=((struct Cyc_Absyn_AggrType_struct*)_tmp2FF)->f1;_tmp301=_tmp300.aggr_info;
_LL19D: ad=Cyc_Absyn_get_known_aggrdecl(_tmp301);goto _LL19B;_LL19E:;_LL19F:({void*
_tmp302=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp303="init_struct: bad struct type";_tag_dyneither(_tmp303,sizeof(
char),29);}),_tag_dyneither(_tmp302,sizeof(void*),0));});_LL19B:;}{int
is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct Cyc_Toc_Env _tmp305;struct _RegionHandle*_tmp306;struct Cyc_Toc_Env*_tmp304=
nv;_tmp305=*_tmp304;_tmp306=_tmp305.rgn;{struct Cyc_List_List*_tmp307=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(_tmp306,dles);for(0;
_tmp307 != 0;_tmp307=_tmp307->tl){struct _tuple16 _tmp309;struct Cyc_List_List*
_tmp30A;struct Cyc_Absyn_Exp*_tmp30B;struct _tuple16*_tmp308=(struct _tuple16*)
_tmp307->hd;_tmp309=*_tmp308;_tmp30A=_tmp309.f1;_tmp30B=_tmp309.f2;is_atomic=
is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp30B->topt))->v);
if(_tmp30A == 0)({void*_tmp30C=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp30D="empty designator list";
_tag_dyneither(_tmp30D,sizeof(char),22);}),_tag_dyneither(_tmp30C,sizeof(void*),
0));});if(_tmp30A->tl != 0){struct _tuple1*_tmp30E=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp30F=Cyc_Absyn_var_exp(_tmp30E,0);for(0;_tmp30A != 0;_tmp30A=_tmp30A->tl){void*
_tmp310=(void*)_tmp30A->hd;struct _dyneither_ptr*_tmp311;_LL1A1: if(*((int*)
_tmp310)!= 1)goto _LL1A3;_tmp311=((struct Cyc_Absyn_FieldName_struct*)_tmp310)->f1;
_LL1A2: if(Cyc_Toc_is_poly_field(struct_type,_tmp311))_tmp30F=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp30F);_tmp2FC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp2FD(_tmp2FB,_tmp311,0),_tmp30F,0),0),_tmp2FC,0);goto _LL1A0;_LL1A3:;_LL1A4:({
void*_tmp312=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp313="array designator in struct";_tag_dyneither(_tmp313,sizeof(
char),27);}),_tag_dyneither(_tmp312,sizeof(void*),0));});_LL1A0:;}Cyc_Toc_exp_to_c(
nv,_tmp30B);_tmp2FC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp30F,_tmp30B,0),0),_tmp2FC,0);}else{void*_tmp314=(void*)_tmp30A->hd;struct
_dyneither_ptr*_tmp315;_LL1A6: if(*((int*)_tmp314)!= 1)goto _LL1A8;_tmp315=((
struct Cyc_Absyn_FieldName_struct*)_tmp314)->f1;_LL1A7: {struct Cyc_Absyn_Exp*lval=
_tmp2FD(_tmp2FB,_tmp315,0);if(is_tagged_union){int i=Cyc_Toc_get_member_offset(ad,
_tmp315);struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);struct Cyc_Absyn_Exp*
lhs=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*assn_exp=
Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);_tmp2FC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
assn_exp,0),_tmp2FC,0);lval=Cyc_Absyn_aggrmember_exp(lval,Cyc_Toc_val_sp,0);}{
void*_tmp316=_tmp30B->r;struct Cyc_List_List*_tmp317;struct Cyc_Absyn_Vardecl*
_tmp318;struct Cyc_Absyn_Exp*_tmp319;struct Cyc_Absyn_Exp*_tmp31A;int _tmp31B;void*
_tmp31C;struct Cyc_List_List*_tmp31D;_LL1AB: if(*((int*)_tmp316)!= 28)goto _LL1AD;
_tmp317=((struct Cyc_Absyn_Array_e_struct*)_tmp316)->f1;_LL1AC: _tmp2FC=Cyc_Toc_init_array(
nv,lval,_tmp317,_tmp2FC);goto _LL1AA;_LL1AD: if(*((int*)_tmp316)!= 29)goto _LL1AF;
_tmp318=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp316)->f1;_tmp319=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp316)->f2;_tmp31A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp316)->f3;_tmp31B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp316)->f4;
_LL1AE: _tmp2FC=Cyc_Toc_init_comprehension(nv,lval,_tmp318,_tmp319,_tmp31A,
_tmp31B,_tmp2FC,0);goto _LL1AA;_LL1AF: if(*((int*)_tmp316)!= 31)goto _LL1B1;_tmp31C=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp316)->f1;_tmp31D=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp316)->f2;_LL1B0: _tmp2FC=Cyc_Toc_init_anon_struct(nv,lval,_tmp31C,_tmp31D,
_tmp2FC);goto _LL1AA;_LL1B1:;_LL1B2: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp30B->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp30B);{struct Cyc_Absyn_Aggrfield*_tmp31E=Cyc_Absyn_lookup_decl_field(ad,
_tmp315);if(Cyc_Toc_is_poly_field(struct_type,_tmp315) && !was_ptr_type)_tmp30B=
Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp30B);if(has_exists)_tmp30B=Cyc_Toc_cast_it(
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp31E))->type),
_tmp30B);_tmp2FC=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,
_tmp30B,0),0),_tmp2FC,0);goto _LL1AA;}}_LL1AA:;}goto _LL1A5;}_LL1A8:;_LL1A9:({void*
_tmp31F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp320="array designator in struct";_tag_dyneither(_tmp320,sizeof(
char),27);}),_tag_dyneither(_tmp31F,sizeof(void*),0));});_LL1A5:;}}}return Cyc_Toc_make_struct(
nv,_tmp2FA,_tmp2FE,_tmp2FC,pointer,rgnopt,is_atomic);}}struct _tuple18{struct Cyc_Core_Opt*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp*el,struct _tuple18*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*
pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*
e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321[0]=({struct Cyc_Absyn_Increment_e_struct
_tmp322;_tmp322.tag=5;_tmp322.f1=e1;_tmp322.f2=(void*)incr;_tmp322;});_tmp321;}),
0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp323=e1->r;struct Cyc_Absyn_Stmt*
_tmp324;void*_tmp325;struct Cyc_Absyn_Exp*_tmp326;struct Cyc_Absyn_Exp*_tmp327;
struct _dyneither_ptr*_tmp328;int _tmp329;int _tmp32A;_LL1B4: if(*((int*)_tmp323)!= 
38)goto _LL1B6;_tmp324=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp323)->f1;_LL1B5:
Cyc_Toc_lvalue_assign_stmt(_tmp324,fs,f,f_env);goto _LL1B3;_LL1B6: if(*((int*)
_tmp323)!= 15)goto _LL1B8;_tmp325=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp323)->f1;
_tmp326=((struct Cyc_Absyn_Cast_e_struct*)_tmp323)->f2;_LL1B7: Cyc_Toc_lvalue_assign(
_tmp326,fs,f,f_env);goto _LL1B3;_LL1B8: if(*((int*)_tmp323)!= 23)goto _LL1BA;
_tmp327=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp323)->f1;_tmp328=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp323)->f2;_tmp329=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp323)->f3;_tmp32A=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp323)->f4;_LL1B9: e1->r=_tmp327->r;Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp32B=_cycalloc(sizeof(*_tmp32B));
_tmp32B->hd=_tmp328;_tmp32B->tl=fs;_tmp32B;}),f,f_env);goto _LL1B3;_LL1BA:;_LL1BB: {
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;goto _LL1B3;}_LL1B3:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp32C=s->r;struct Cyc_Absyn_Exp*_tmp32D;struct Cyc_Absyn_Decl*
_tmp32E;struct Cyc_Absyn_Stmt*_tmp32F;struct Cyc_Absyn_Stmt*_tmp330;_LL1BD: if(
_tmp32C <= (void*)1)goto _LL1C3;if(*((int*)_tmp32C)!= 0)goto _LL1BF;_tmp32D=((
struct Cyc_Absyn_Exp_s_struct*)_tmp32C)->f1;_LL1BE: Cyc_Toc_lvalue_assign(_tmp32D,
fs,f,f_env);goto _LL1BC;_LL1BF: if(*((int*)_tmp32C)!= 11)goto _LL1C1;_tmp32E=((
struct Cyc_Absyn_Decl_s_struct*)_tmp32C)->f1;_tmp32F=((struct Cyc_Absyn_Decl_s_struct*)
_tmp32C)->f2;_LL1C0: Cyc_Toc_lvalue_assign_stmt(_tmp32F,fs,f,f_env);goto _LL1BC;
_LL1C1: if(*((int*)_tmp32C)!= 1)goto _LL1C3;_tmp330=((struct Cyc_Absyn_Seq_s_struct*)
_tmp32C)->f2;_LL1C2: Cyc_Toc_lvalue_assign_stmt(_tmp330,fs,f,f_env);goto _LL1BC;
_LL1C3:;_LL1C4:({struct Cyc_String_pa_struct _tmp333;_tmp333.tag=0;_tmp333.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));{void*
_tmp331[1]={& _tmp333};Cyc_Toc_toc_impos(({const char*_tmp332="lvalue_assign_stmt: %s";
_tag_dyneither(_tmp332,sizeof(char),23);}),_tag_dyneither(_tmp331,sizeof(void*),
1));}});_LL1BC:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*
r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*
result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*
_tmp334=_region_malloc(r2,sizeof(*_tmp334));_tmp334->hd=(void*)f((void*)x->hd,
env);_tmp334->tl=0;_tmp334;});prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp335=_region_malloc(r2,sizeof(*
_tmp335));_tmp335->hd=(void*)f((void*)x->hd,env);_tmp335->tl=0;_tmp335;});prev=((
struct Cyc_List_List*)_check_null(prev))->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple16*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){return({struct _tuple16*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->f1=0;
_tmp336->f2=e;_tmp336;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*
t){void*_tmp337=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp338;_LL1C6: if(
_tmp337 <= (void*)4)goto _LL1C8;if(*((int*)_tmp337)!= 4)goto _LL1C8;_tmp338=((
struct Cyc_Absyn_PointerType_struct*)_tmp337)->f1;_LL1C7: return _tmp338;_LL1C8:;
_LL1C9:({void*_tmp339=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp33A="get_ptr_typ: not a pointer!";
_tag_dyneither(_tmp33A,sizeof(char),28);}),_tag_dyneither(_tmp339,sizeof(void*),
0));});_LL1C5:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct
Cyc_Absyn_Exp*res;{void*_tmp33B=Cyc_Tcutil_compress(t);void*_tmp33C;void*_tmp33D;
void*_tmp33E;void*_tmp33F;void*_tmp340;void*_tmp341;void*_tmp342;void*_tmp343;
void*_tmp344;void*_tmp345;_LL1CB: if(_tmp33B <= (void*)4)goto _LL1DB;if(*((int*)
_tmp33B)!= 4)goto _LL1CD;_LL1CC: res=Cyc_Absyn_null_exp(0);goto _LL1CA;_LL1CD: if(*((
int*)_tmp33B)!= 5)goto _LL1CF;_tmp33C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33B)->f1;_tmp33D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33B)->f2;if((
int)_tmp33D != 0)goto _LL1CF;_LL1CE: res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(
_tmp33C,'\000'),0);goto _LL1CA;_LL1CF: if(*((int*)_tmp33B)!= 5)goto _LL1D1;_tmp33E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp33B)->f1;_tmp33F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33B)->f2;if((int)_tmp33F != 1)goto _LL1D1;_LL1D0: res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(
_tmp33E,0),0);goto _LL1CA;_LL1D1: if(*((int*)_tmp33B)!= 12)goto _LL1D3;_LL1D2: goto
_LL1D4;_LL1D3: if(*((int*)_tmp33B)!= 13)goto _LL1D5;_LL1D4: _tmp340=(void*)1;goto
_LL1D6;_LL1D5: if(*((int*)_tmp33B)!= 5)goto _LL1D7;_tmp340=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33B)->f1;_tmp341=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33B)->f2;if((
int)_tmp341 != 2)goto _LL1D7;_LL1D6: _tmp342=_tmp340;goto _LL1D8;_LL1D7: if(*((int*)
_tmp33B)!= 5)goto _LL1D9;_tmp342=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33B)->f1;
_tmp343=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33B)->f2;if((int)_tmp343 != 
3)goto _LL1D9;_LL1D8: res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp342,0),0);goto
_LL1CA;_LL1D9: if(*((int*)_tmp33B)!= 5)goto _LL1DB;_tmp344=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp33B)->f1;_tmp345=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp33B)->f2;if((
int)_tmp345 != 4)goto _LL1DB;_LL1DA: res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(
_tmp344,(long long)0),0);goto _LL1CA;_LL1DB: if((int)_tmp33B != 1)goto _LL1DD;_LL1DC:
goto _LL1DE;_LL1DD: if(_tmp33B <= (void*)4)goto _LL1DF;if(*((int*)_tmp33B)!= 6)goto
_LL1DF;_LL1DE: res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp346="0.0";
_tag_dyneither(_tmp346,sizeof(char),4);})),0);goto _LL1CA;_LL1DF:;_LL1E0:({struct
Cyc_String_pa_struct _tmp349;_tmp349.tag=0;_tmp349.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp347[1]={& _tmp349};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp348="found non-zero type %s in generate_zero";_tag_dyneither(
_tmp348,sizeof(char),40);}),_tag_dyneither(_tmp347,sizeof(void*),1));}});_LL1CA:;}
res->topt=({struct Cyc_Core_Opt*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->v=(
void*)t;_tmp34A;});return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct
Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=
Cyc_Absyn_dyneither_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp34B=Cyc_Toc_typ_to_c(elt_type);void*_tmp34C=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp34D=Cyc_Absyn_cstar_typ(_tmp34B,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmp34E=({struct Cyc_Core_Opt*_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D->v=(
void*)_tmp34D;_tmp38D;});struct Cyc_Absyn_Exp*xinit;{void*_tmp34F=e1->r;struct Cyc_Absyn_Exp*
_tmp350;struct Cyc_Absyn_Exp*_tmp351;struct Cyc_Absyn_Exp*_tmp352;_LL1E2: if(*((int*)
_tmp34F)!= 22)goto _LL1E4;_tmp350=((struct Cyc_Absyn_Deref_e_struct*)_tmp34F)->f1;
_LL1E3: if(!is_dyneither){_tmp350=Cyc_Toc_cast_it(fat_ptr_type,_tmp350);_tmp350->topt=({
struct Cyc_Core_Opt*_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353->v=(void*)
fat_ptr_type;_tmp353;});}Cyc_Toc_exp_to_c(nv,_tmp350);xinit=_tmp350;goto _LL1E1;
_LL1E4: if(*((int*)_tmp34F)!= 25)goto _LL1E6;_tmp351=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp34F)->f1;_tmp352=((struct Cyc_Absyn_Subscript_e_struct*)_tmp34F)->f2;_LL1E5:
if(!is_dyneither){_tmp351=Cyc_Toc_cast_it(fat_ptr_type,_tmp351);_tmp351->topt=({
struct Cyc_Core_Opt*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->v=(void*)
fat_ptr_type;_tmp354;});}Cyc_Toc_exp_to_c(nv,_tmp351);Cyc_Toc_exp_to_c(nv,
_tmp352);xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp355[3];_tmp355[2]=_tmp352;_tmp355[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp355[0]=_tmp351;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp355,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
goto _LL1E1;_LL1E6:;_LL1E7:({void*_tmp356=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp357="found bad lhs for zero-terminated pointer assignment";
_tag_dyneither(_tmp357,sizeof(char),53);}),_tag_dyneither(_tmp356,sizeof(void*),
0));});_LL1E1:;}{struct _tuple1*_tmp358=Cyc_Toc_temp_var();struct Cyc_Toc_Env
_tmp35A;struct _RegionHandle*_tmp35B;struct Cyc_Toc_Env*_tmp359=nv;_tmp35A=*
_tmp359;_tmp35B=_tmp35A.rgn;{struct Cyc_Toc_Env*_tmp35C=Cyc_Toc_add_varmap(
_tmp35B,nv,_tmp358,Cyc_Absyn_var_exp(_tmp358,0));struct Cyc_Absyn_Vardecl*_tmp35D=({
struct Cyc_Absyn_Vardecl*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C->sc=(void*)2;
_tmp38C->name=_tmp358;_tmp38C->tq=Cyc_Toc_mt_tq;_tmp38C->type=_tmp34C;_tmp38C->initializer=(
struct Cyc_Absyn_Exp*)xinit;_tmp38C->rgn=0;_tmp38C->attributes=0;_tmp38C->escapes=
0;_tmp38C;});struct Cyc_Absyn_Local_b_struct*_tmp35E=({struct Cyc_Absyn_Local_b_struct*
_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A[0]=({struct Cyc_Absyn_Local_b_struct
_tmp38B;_tmp38B.tag=3;_tmp38B.f1=_tmp35D;_tmp38B;});_tmp38A;});struct Cyc_Absyn_Exp*
_tmp35F=Cyc_Absyn_varb_exp(_tmp358,(void*)_tmp35E,0);_tmp35F->topt=({struct Cyc_Core_Opt*
_tmp360=_cycalloc(sizeof(*_tmp360));_tmp360->v=(void*)fat_ptr_type;_tmp360;});{
struct Cyc_Absyn_Exp*_tmp361=Cyc_Absyn_deref_exp(_tmp35F,0);_tmp361->topt=({
struct Cyc_Core_Opt*_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362->v=(void*)
elt_type;_tmp362;});Cyc_Toc_exp_to_c(_tmp35C,_tmp361);{struct _tuple1*_tmp363=Cyc_Toc_temp_var();
_tmp35C=Cyc_Toc_add_varmap(_tmp35B,_tmp35C,_tmp363,Cyc_Absyn_var_exp(_tmp363,0));{
struct Cyc_Absyn_Vardecl*_tmp364=({struct Cyc_Absyn_Vardecl*_tmp389=_cycalloc(
sizeof(*_tmp389));_tmp389->sc=(void*)2;_tmp389->name=_tmp363;_tmp389->tq=Cyc_Toc_mt_tq;
_tmp389->type=_tmp34B;_tmp389->initializer=(struct Cyc_Absyn_Exp*)_tmp361;_tmp389->rgn=
0;_tmp389->attributes=0;_tmp389->escapes=0;_tmp389;});struct Cyc_Absyn_Local_b_struct*
_tmp365=({struct Cyc_Absyn_Local_b_struct*_tmp387=_cycalloc(sizeof(*_tmp387));
_tmp387[0]=({struct Cyc_Absyn_Local_b_struct _tmp388;_tmp388.tag=3;_tmp388.f1=
_tmp364;_tmp388;});_tmp387;});struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct
Cyc_Absyn_Exp*_tmp366=Cyc_Absyn_varb_exp(_tmp363,(void*)_tmp365,0);_tmp366->topt=
_tmp361->topt;z_init=Cyc_Absyn_prim2_exp((void*)popt->v,_tmp366,e2,0);z_init->topt=
_tmp366->topt;}Cyc_Toc_exp_to_c(_tmp35C,z_init);{struct _tuple1*_tmp367=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp368=({struct Cyc_Absyn_Vardecl*_tmp386=_cycalloc(
sizeof(*_tmp386));_tmp386->sc=(void*)2;_tmp386->name=_tmp367;_tmp386->tq=Cyc_Toc_mt_tq;
_tmp386->type=_tmp34B;_tmp386->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp386->rgn=
0;_tmp386->attributes=0;_tmp386->escapes=0;_tmp386;});struct Cyc_Absyn_Local_b_struct*
_tmp369=({struct Cyc_Absyn_Local_b_struct*_tmp384=_cycalloc(sizeof(*_tmp384));
_tmp384[0]=({struct Cyc_Absyn_Local_b_struct _tmp385;_tmp385.tag=3;_tmp385.f1=
_tmp368;_tmp385;});_tmp384;});_tmp35C=Cyc_Toc_add_varmap(_tmp35B,_tmp35C,_tmp367,
Cyc_Absyn_var_exp(_tmp367,0));{struct Cyc_Absyn_Exp*_tmp36A=Cyc_Absyn_varb_exp(
_tmp363,(void*)_tmp365,0);_tmp36A->topt=_tmp361->topt;{struct Cyc_Absyn_Exp*
_tmp36B=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp36C=Cyc_Absyn_prim2_exp((
void*)5,_tmp36A,_tmp36B,0);_tmp36C->topt=({struct Cyc_Core_Opt*_tmp36D=_cycalloc(
sizeof(*_tmp36D));_tmp36D->v=(void*)Cyc_Absyn_sint_typ;_tmp36D;});Cyc_Toc_exp_to_c(
_tmp35C,_tmp36C);{struct Cyc_Absyn_Exp*_tmp36E=Cyc_Absyn_varb_exp(_tmp367,(void*)
_tmp369,0);_tmp36E->topt=_tmp361->topt;{struct Cyc_Absyn_Exp*_tmp36F=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp370=Cyc_Absyn_prim2_exp((void*)6,_tmp36E,
_tmp36F,0);_tmp370->topt=({struct Cyc_Core_Opt*_tmp371=_cycalloc(sizeof(*_tmp371));
_tmp371->v=(void*)Cyc_Absyn_sint_typ;_tmp371;});Cyc_Toc_exp_to_c(_tmp35C,_tmp370);{
struct Cyc_List_List*_tmp372=({struct Cyc_Absyn_Exp*_tmp383[2];_tmp383[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp383[0]=Cyc_Absyn_varb_exp(_tmp358,(void*)
_tmp35E,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp383,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp373=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp((
void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp372,0),_tmp373,0);{
struct Cyc_Absyn_Exp*_tmp374=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp36C,
_tmp370,0),0);struct Cyc_Absyn_Stmt*_tmp375=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp376=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp358,(void*)_tmp35E,0),Cyc_Toc_curr_sp,0);_tmp376=Cyc_Toc_cast_it(
_tmp34D,_tmp376);{struct Cyc_Absyn_Exp*_tmp377=Cyc_Absyn_deref_exp(_tmp376,0);
struct Cyc_Absyn_Exp*_tmp378=Cyc_Absyn_assign_exp(_tmp377,Cyc_Absyn_var_exp(
_tmp367,0),0);struct Cyc_Absyn_Stmt*_tmp379=Cyc_Absyn_exp_stmt(_tmp378,0);_tmp379=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp374,_tmp375,Cyc_Absyn_skip_stmt(
0),0),_tmp379,0);_tmp379=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp37A=
_cycalloc(sizeof(*_tmp37A));_tmp37A->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_Absyn_Var_d_struct
_tmp37C;_tmp37C.tag=0;_tmp37C.f1=_tmp368;_tmp37C;});_tmp37B;});_tmp37A->loc=0;
_tmp37A;}),_tmp379,0);_tmp379=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp37D=
_cycalloc(sizeof(*_tmp37D));_tmp37D->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=({struct Cyc_Absyn_Var_d_struct
_tmp37F;_tmp37F.tag=0;_tmp37F.f1=_tmp364;_tmp37F;});_tmp37E;});_tmp37D->loc=0;
_tmp37D;}),_tmp379,0);_tmp379=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp380=
_cycalloc(sizeof(*_tmp380));_tmp380->r=(void*)({struct Cyc_Absyn_Var_d_struct*
_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381[0]=({struct Cyc_Absyn_Var_d_struct
_tmp382;_tmp382.tag=0;_tmp382.f1=_tmp35D;_tmp382;});_tmp381;});_tmp380->loc=0;
_tmp380;}),_tmp379,0);e->r=Cyc_Toc_stmt_exp_r(_tmp379);}}}}}}}}}}}}}}static void*
Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,
struct _dyneither_ptr*f,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,
struct _dyneither_ptr*,struct Cyc_Position_Segment*)){struct Cyc_Absyn_Aggrdecl*ad;{
void*_tmp38E=Cyc_Tcutil_compress(aggrtype);struct Cyc_Absyn_AggrInfo _tmp38F;union
Cyc_Absyn_AggrInfoU _tmp390;_LL1E9: if(_tmp38E <= (void*)4)goto _LL1EB;if(*((int*)
_tmp38E)!= 10)goto _LL1EB;_tmp38F=((struct Cyc_Absyn_AggrType_struct*)_tmp38E)->f1;
_tmp390=_tmp38F.aggr_info;_LL1EA: ad=Cyc_Absyn_get_known_aggrdecl(_tmp390);goto
_LL1E8;_LL1EB:;_LL1EC:({void*_tmp391=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp394;_tmp394.tag=0;_tmp394.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(aggrtype));{void*_tmp392[1]={& _tmp394};Cyc_aprintf(({
const char*_tmp393="expecting union but found %s in check_tagged_union";
_tag_dyneither(_tmp393,sizeof(char),51);}),_tag_dyneither(_tmp392,sizeof(void*),
1));}}),_tag_dyneither(_tmp391,sizeof(void*),0));});_LL1E8:;}{struct _tuple1*
_tmp395=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp396=Cyc_Absyn_var_exp(_tmp395,
0);struct Cyc_Absyn_Exp*_tmp397=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp396,f,0),
Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*_tmp398=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
ad,f),0);struct Cyc_Absyn_Exp*_tmp399=Cyc_Absyn_neq_exp(_tmp397,_tmp398,0);struct
Cyc_Absyn_Exp*_tmp39A=Cyc_Absyn_aggrmember_exp(aggrproj(_tmp396,f,0),Cyc_Toc_val_sp,
0);struct Cyc_Absyn_Stmt*_tmp39B=Cyc_Absyn_exp_stmt(_tmp39A,0);struct Cyc_Absyn_Stmt*
_tmp39C=Cyc_Absyn_ifthenelse_stmt(_tmp399,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),
0);struct Cyc_Absyn_Stmt*_tmp39D=Cyc_Absyn_declare_stmt(_tmp395,e1_c_type,(struct
Cyc_Absyn_Exp*)e1,Cyc_Absyn_seq_stmt(_tmp39C,_tmp39B,0),0);return Cyc_Toc_stmt_exp_r(
_tmp39D);}}static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*
f_tag,void**tagged_member_type,int clear_read){void*_tmp39E=e->r;struct Cyc_Absyn_Exp*
_tmp39F;struct Cyc_Absyn_Exp*_tmp3A0;struct _dyneither_ptr*_tmp3A1;int _tmp3A2;int*
_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A4;struct _dyneither_ptr*_tmp3A5;int _tmp3A6;int*
_tmp3A7;_LL1EE: if(*((int*)_tmp39E)!= 15)goto _LL1F0;_tmp39F=((struct Cyc_Absyn_Cast_e_struct*)
_tmp39E)->f2;_LL1EF:({void*_tmp3A8=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp3A9="cast on lhs!";
_tag_dyneither(_tmp3A9,sizeof(char),13);}),_tag_dyneither(_tmp3A8,sizeof(void*),
0));});_LL1F0: if(*((int*)_tmp39E)!= 23)goto _LL1F2;_tmp3A0=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp39E)->f1;_tmp3A1=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp39E)->f2;_tmp3A2=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp39E)->f4;_tmp3A3=(int*)&((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp39E)->f4;_LL1F1: {void*_tmp3AA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3A0->topt))->v);struct Cyc_Absyn_AggrInfo _tmp3AB;union Cyc_Absyn_AggrInfoU
_tmp3AC;_LL1F7: if(_tmp3AA <= (void*)4)goto _LL1F9;if(*((int*)_tmp3AA)!= 10)goto
_LL1F9;_tmp3AB=((struct Cyc_Absyn_AggrType_struct*)_tmp3AA)->f1;_tmp3AC=_tmp3AB.aggr_info;
_LL1F8: {struct Cyc_Absyn_Aggrdecl*_tmp3AD=Cyc_Absyn_get_known_aggrdecl(_tmp3AC);*
f_tag=Cyc_Toc_get_member_offset(_tmp3AD,_tmp3A1);{struct _dyneither_ptr str=(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp3B2;_tmp3B2.tag=0;_tmp3B2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3A1);{struct Cyc_String_pa_struct
_tmp3B1;_tmp3B1.tag=0;_tmp3B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*
_tmp3AD->name).f2);{void*_tmp3AF[2]={& _tmp3B1,& _tmp3B2};Cyc_aprintf(({const char*
_tmp3B0="_union_%s_%s";_tag_dyneither(_tmp3B0,sizeof(char),13);}),_tag_dyneither(
_tmp3AF,sizeof(void*),2));}}});*tagged_member_type=Cyc_Absyn_strct(({struct
_dyneither_ptr*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE[0]=str;_tmp3AE;}));
if(clear_read)*_tmp3A3=0;return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3AD->impl))->tagged;}}_LL1F9:;_LL1FA: return 0;_LL1F6:;}_LL1F2: if(*((int*)
_tmp39E)!= 24)goto _LL1F4;_tmp3A4=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp39E)->f1;
_tmp3A5=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp39E)->f2;_tmp3A6=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp39E)->f4;_tmp3A7=(int*)&((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp39E)->f4;
_LL1F3: {void*_tmp3B3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3A4->topt))->v);struct Cyc_Absyn_PtrInfo _tmp3B4;void*_tmp3B5;
_LL1FC: if(_tmp3B3 <= (void*)4)goto _LL1FE;if(*((int*)_tmp3B3)!= 4)goto _LL1FE;
_tmp3B4=((struct Cyc_Absyn_PointerType_struct*)_tmp3B3)->f1;_tmp3B5=_tmp3B4.elt_typ;
_LL1FD: {void*_tmp3B6=Cyc_Tcutil_compress(_tmp3B5);struct Cyc_Absyn_AggrInfo
_tmp3B7;union Cyc_Absyn_AggrInfoU _tmp3B8;_LL201: if(_tmp3B6 <= (void*)4)goto _LL203;
if(*((int*)_tmp3B6)!= 10)goto _LL203;_tmp3B7=((struct Cyc_Absyn_AggrType_struct*)
_tmp3B6)->f1;_tmp3B8=_tmp3B7.aggr_info;_LL202: {struct Cyc_Absyn_Aggrdecl*_tmp3B9=
Cyc_Absyn_get_known_aggrdecl(_tmp3B8);*f_tag=Cyc_Toc_get_member_offset(_tmp3B9,
_tmp3A5);{struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp3BE;_tmp3BE.tag=0;_tmp3BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp3A5);{struct Cyc_String_pa_struct _tmp3BD;_tmp3BD.tag=0;_tmp3BD.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*_tmp3B9->name).f2);{void*_tmp3BB[2]={&
_tmp3BD,& _tmp3BE};Cyc_aprintf(({const char*_tmp3BC="_union_%s_%s";_tag_dyneither(
_tmp3BC,sizeof(char),13);}),_tag_dyneither(_tmp3BB,sizeof(void*),2));}}});*
tagged_member_type=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp3BA=_cycalloc(
sizeof(*_tmp3BA));_tmp3BA[0]=str;_tmp3BA;}));if(clear_read)*_tmp3A7=0;return((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3B9->impl))->tagged;}}_LL203:;
_LL204: return 0;_LL200:;}_LL1FE:;_LL1FF: return 0;_LL1FB:;}_LL1F4:;_LL1F5: return 0;
_LL1ED:;}static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*
e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int
tag_num,void*member_type){struct _tuple1*_tmp3BF=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
temp_exp=Cyc_Absyn_var_exp(_tmp3BF,0);struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(
temp_exp,Cyc_Toc_tag_sp,0);struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(
tag_num,0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(
temp_val,popt,e2,0),0);struct Cyc_Absyn_Stmt*s2;if(popt == 0)s2=Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{struct Cyc_Absyn_Exp*_tmp3C0=Cyc_Absyn_neq_exp(
temp_tag,f_tag,0);s2=Cyc_Absyn_ifthenelse_stmt(_tmp3C0,Cyc_Toc_throw_match_stmt(),
Cyc_Toc_skip_stmt_dl(),0);}{struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(
_tmp3BF,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
e1,0),Cyc_Absyn_seq_stmt(s2,s3,0),0);return Cyc_Toc_stmt_exp_r(s1);}}struct
_tuple19{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};struct _tuple20{struct
Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp3C1=e->r;if(e->topt == 0)({
struct Cyc_String_pa_struct _tmp3C4;_tmp3C4.tag=0;_tmp3C4.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp3C2[1]={& _tmp3C4};((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp3C3="exp_to_c: no type for %s";_tag_dyneither(_tmp3C3,sizeof(char),
25);}),_tag_dyneither(_tmp3C2,sizeof(void*),1));}});{void*old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp3C5=_tmp3C1;union Cyc_Absyn_Cnst
_tmp3C6;int _tmp3C7;struct _tuple1*_tmp3C8;void*_tmp3C9;struct _tuple1*_tmp3CA;void*
_tmp3CB;struct Cyc_List_List*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CD;void*_tmp3CE;
struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Core_Opt*_tmp3D0;struct Cyc_Absyn_Exp*
_tmp3D1;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_Absyn_Exp*_tmp3D3;struct Cyc_Absyn_Exp*
_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Exp*_tmp3D6;struct Cyc_Absyn_Exp*
_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Exp*_tmp3D9;struct Cyc_Absyn_Exp*
_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DB;struct Cyc_List_List*_tmp3DC;struct Cyc_Absyn_Exp*
_tmp3DD;struct Cyc_List_List*_tmp3DE;struct Cyc_Absyn_VarargCallInfo*_tmp3DF;
struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_List_List*_tmp3E1;struct Cyc_Absyn_VarargCallInfo*
_tmp3E2;struct Cyc_Absyn_VarargCallInfo _tmp3E3;int _tmp3E4;struct Cyc_List_List*
_tmp3E5;struct Cyc_Absyn_VarargInfo*_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E7;struct Cyc_Absyn_Exp*
_tmp3E8;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_List_List*_tmp3EA;void*_tmp3EB;
void**_tmp3EC;struct Cyc_Absyn_Exp*_tmp3ED;int _tmp3EE;void*_tmp3EF;struct Cyc_Absyn_Exp*
_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F2;struct Cyc_Absyn_Exp*
_tmp3F3;void*_tmp3F4;void*_tmp3F5;void*_tmp3F6;struct _dyneither_ptr*_tmp3F7;void*
_tmp3F8;void*_tmp3F9;unsigned int _tmp3FA;struct Cyc_Absyn_Exp*_tmp3FB;struct Cyc_Absyn_Exp*
_tmp3FC;struct _dyneither_ptr*_tmp3FD;int _tmp3FE;int _tmp3FF;struct Cyc_Absyn_Exp*
_tmp400;struct _dyneither_ptr*_tmp401;int _tmp402;int _tmp403;struct Cyc_Absyn_Exp*
_tmp404;struct Cyc_Absyn_Exp*_tmp405;struct Cyc_List_List*_tmp406;struct Cyc_List_List*
_tmp407;struct Cyc_Absyn_Vardecl*_tmp408;struct Cyc_Absyn_Exp*_tmp409;struct Cyc_Absyn_Exp*
_tmp40A;int _tmp40B;struct _tuple1*_tmp40C;struct Cyc_List_List*_tmp40D;struct Cyc_List_List*
_tmp40E;struct Cyc_Absyn_Aggrdecl*_tmp40F;void*_tmp410;struct Cyc_List_List*
_tmp411;struct Cyc_List_List*_tmp412;struct Cyc_Absyn_Datatypedecl*_tmp413;struct
Cyc_Absyn_Datatypefield*_tmp414;struct Cyc_List_List*_tmp415;struct Cyc_Absyn_Datatypedecl*
_tmp416;struct Cyc_Absyn_Datatypefield*_tmp417;struct Cyc_Absyn_MallocInfo _tmp418;
int _tmp419;struct Cyc_Absyn_Exp*_tmp41A;void**_tmp41B;struct Cyc_Absyn_Exp*_tmp41C;
int _tmp41D;struct Cyc_Absyn_Exp*_tmp41E;struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_Absyn_Exp*
_tmp420;struct _dyneither_ptr*_tmp421;struct Cyc_Absyn_Stmt*_tmp422;_LL206: if(*((
int*)_tmp3C5)!= 0)goto _LL208;_tmp3C6=((struct Cyc_Absyn_Const_e_struct*)_tmp3C5)->f1;
if((_tmp3C6.Null_c).tag != 1)goto _LL208;_tmp3C7=(int)(_tmp3C6.Null_c).val;_LL207: {
struct Cyc_Absyn_Exp*_tmp423=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){if(Cyc_Toc_is_toplevel(nv))e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,
_tmp423,_tmp423))->r;else{e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp424[3];_tmp424[2]=_tmp423;_tmp424[1]=_tmp423;_tmp424[0]=
_tmp423;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp424,sizeof(struct Cyc_Absyn_Exp*),3));}));}}else{e->r=(void*)&
Cyc_Toc_zero_exp;}goto _LL205;}_LL208: if(*((int*)_tmp3C5)!= 0)goto _LL20A;_LL209:
goto _LL205;_LL20A: if(*((int*)_tmp3C5)!= 1)goto _LL20C;_tmp3C8=((struct Cyc_Absyn_Var_e_struct*)
_tmp3C5)->f1;_tmp3C9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3C5)->f2;_LL20B:{
struct _handler_cons _tmp425;_push_handler(& _tmp425);{int _tmp427=0;if(setjmp(
_tmp425.handler))_tmp427=1;if(!_tmp427){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp3C8))->r;;
_pop_handler();}else{void*_tmp426=(void*)_exn_thrown;void*_tmp429=_tmp426;_LL261:
if(_tmp429 != Cyc_Dict_Absent)goto _LL263;_LL262:({struct Cyc_String_pa_struct
_tmp42C;_tmp42C.tag=0;_tmp42C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp3C8));{void*_tmp42A[1]={& _tmp42C};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp42B="Can't find %s in exp_to_c, Var\n";
_tag_dyneither(_tmp42B,sizeof(char),32);}),_tag_dyneither(_tmp42A,sizeof(void*),
1));}});_LL263:;_LL264:(void)_throw(_tmp429);_LL260:;}}}goto _LL205;_LL20C: if(*((
int*)_tmp3C5)!= 2)goto _LL20E;_tmp3CA=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp3C5)->f1;_LL20D:({void*_tmp42D=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp42E="unknownid";
_tag_dyneither(_tmp42E,sizeof(char),10);}),_tag_dyneither(_tmp42D,sizeof(void*),
0));});_LL20E: if(*((int*)_tmp3C5)!= 3)goto _LL210;_tmp3CB=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3C5)->f1;_tmp3CC=((struct Cyc_Absyn_Primop_e_struct*)_tmp3C5)->f2;_LL20F: {
struct Cyc_List_List*_tmp42F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp3CC);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3CC);{void*_tmp430=_tmp3CB;_LL266:
if((int)_tmp430 != 19)goto _LL268;_LL267: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3CC))->hd;{void*_tmp431=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp432;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_PtrInfo _tmp434;void*_tmp435;
struct Cyc_Absyn_PtrAtts _tmp436;union Cyc_Absyn_Constraint*_tmp437;union Cyc_Absyn_Constraint*
_tmp438;union Cyc_Absyn_Constraint*_tmp439;_LL27B: if(_tmp431 <= (void*)4)goto
_LL27F;if(*((int*)_tmp431)!= 7)goto _LL27D;_tmp432=((struct Cyc_Absyn_ArrayType_struct*)
_tmp431)->f1;_tmp433=_tmp432.num_elts;_LL27C: if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)
_check_null(_tmp433)))({void*_tmp43A=0;Cyc_Tcutil_terr(e->loc,({const char*
_tmp43B="can't calculate numelts";_tag_dyneither(_tmp43B,sizeof(char),24);}),
_tag_dyneither(_tmp43A,sizeof(void*),0));});e->r=_tmp433->r;goto _LL27A;_LL27D:
if(*((int*)_tmp431)!= 4)goto _LL27F;_tmp434=((struct Cyc_Absyn_PointerType_struct*)
_tmp431)->f1;_tmp435=_tmp434.elt_typ;_tmp436=_tmp434.ptr_atts;_tmp437=_tmp436.nullable;
_tmp438=_tmp436.bounds;_tmp439=_tmp436.zero_term;_LL27E:{void*_tmp43C=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp438);struct Cyc_Absyn_Exp*_tmp43D;_LL282: if((int)_tmp43C
!= 0)goto _LL284;_LL283: e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,({
struct Cyc_Absyn_Exp*_tmp43E[2];_tmp43E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp435),0);_tmp43E[0]=(struct Cyc_Absyn_Exp*)_tmp3CC->hd;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp43E,sizeof(struct Cyc_Absyn_Exp*),
2));}));goto _LL281;_LL284: if(_tmp43C <= (void*)1)goto _LL281;if(*((int*)_tmp43C)!= 
0)goto _LL281;_tmp43D=((struct Cyc_Absyn_Upper_b_struct*)_tmp43C)->f1;_LL285: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp439)){
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp3CC->hd);e->r=Cyc_Toc_fncall_exp_r(function_e,({struct
Cyc_Absyn_Exp*_tmp43F[2];_tmp43F[1]=_tmp43D;_tmp43F[0]=(struct Cyc_Absyn_Exp*)
_tmp3CC->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp43F,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp437)){if(!Cyc_Evexp_c_can_eval(
_tmp43D))({void*_tmp440=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp441="can't calculate numelts";
_tag_dyneither(_tmp441,sizeof(char),24);}),_tag_dyneither(_tmp440,sizeof(void*),
0));});e->r=Cyc_Toc_conditional_exp_r(arg,_tmp43D,Cyc_Absyn_uint_exp(0,0));}
else{e->r=_tmp43D->r;goto _LL281;}}goto _LL281;_LL281:;}goto _LL27A;_LL27F:;_LL280:({
struct Cyc_String_pa_struct _tmp445;_tmp445.tag=0;_tmp445.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v));{struct Cyc_String_pa_struct _tmp444;_tmp444.tag=0;
_tmp444.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{void*_tmp442[2]={&
_tmp444,& _tmp445};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp443="size primop applied to non-array %s (%s)";_tag_dyneither(
_tmp443,sizeof(char),41);}),_tag_dyneither(_tmp442,sizeof(void*),2));}}});_LL27A:;}
goto _LL265;}_LL268: if((int)_tmp430 != 0)goto _LL26A;_LL269:{void*_tmp446=Cyc_Tcutil_compress((
void*)((struct Cyc_List_List*)_check_null(_tmp42F))->hd);struct Cyc_Absyn_PtrInfo
_tmp447;void*_tmp448;struct Cyc_Absyn_PtrAtts _tmp449;union Cyc_Absyn_Constraint*
_tmp44A;union Cyc_Absyn_Constraint*_tmp44B;_LL287: if(_tmp446 <= (void*)4)goto
_LL289;if(*((int*)_tmp446)!= 4)goto _LL289;_tmp447=((struct Cyc_Absyn_PointerType_struct*)
_tmp446)->f1;_tmp448=_tmp447.elt_typ;_tmp449=_tmp447.ptr_atts;_tmp44A=_tmp449.bounds;
_tmp44B=_tmp449.zero_term;_LL288:{void*_tmp44C=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp44A);struct Cyc_Absyn_Exp*_tmp44D;_LL28C: if((int)_tmp44C != 0)goto _LL28E;
_LL28D: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3CC))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3CC->tl))->hd;e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({
struct Cyc_Absyn_Exp*_tmp44E[3];_tmp44E[2]=e2;_tmp44E[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp448),0);_tmp44E[0]=e1;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp44E,sizeof(struct Cyc_Absyn_Exp*),
3));}));goto _LL28B;}_LL28E: if(_tmp44C <= (void*)1)goto _LL28B;if(*((int*)_tmp44C)
!= 0)goto _LL28B;_tmp44D=((struct Cyc_Absyn_Upper_b_struct*)_tmp44C)->f1;_LL28F:
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp44B)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp3CC))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3CC->tl))->hd;e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*_tmp44F[3];_tmp44F[
2]=e2;_tmp44F[1]=_tmp44D;_tmp44F[0]=e1;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp44F,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r;}goto _LL28B;_LL28B:;}goto _LL286;_LL289:;_LL28A: goto _LL286;_LL286:;}
goto _LL265;_LL26A: if((int)_tmp430 != 2)goto _LL26C;_LL26B: {void*elt_typ=(void*)0;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp42F))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3CC))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3CC->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp42F->tl))->hd)){e1->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e2->r,0),Cyc_Toc_curr_sp);e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({
struct Cyc_Absyn_Exp*_tmp450[3];_tmp450[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);
_tmp450[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp450[0]=e1;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp450,sizeof(struct Cyc_Absyn_Exp*),3));}));}}goto _LL265;}_LL26C: if((int)
_tmp430 != 5)goto _LL26E;_LL26D: goto _LL26F;_LL26E: if((int)_tmp430 != 6)goto _LL270;
_LL26F: goto _LL271;_LL270: if((int)_tmp430 != 7)goto _LL272;_LL271: goto _LL273;_LL272:
if((int)_tmp430 != 9)goto _LL274;_LL273: goto _LL275;_LL274: if((int)_tmp430 != 8)goto
_LL276;_LL275: goto _LL277;_LL276: if((int)_tmp430 != 10)goto _LL278;_LL277: {struct
Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp3CC))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3CC->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)_check_null(
_tmp42F))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp42F->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ(t1))e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(
e1->r,0),Cyc_Toc_curr_sp);if(Cyc_Tcutil_is_tagged_pointer_typ(t2))e2->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);goto _LL265;}_LL278:;_LL279: goto
_LL265;_LL265:;}goto _LL205;}_LL210: if(*((int*)_tmp3C5)!= 5)goto _LL212;_tmp3CD=((
struct Cyc_Absyn_Increment_e_struct*)_tmp3C5)->f1;_tmp3CE=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp3C5)->f2;_LL211: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3CD->topt))->v;void*ptr_type=(void*)0;void*elt_type=(void*)0;int is_dyneither=
0;struct _dyneither_ptr incr_str=({const char*_tmp471="increment";_tag_dyneither(
_tmp471,sizeof(char),10);});if(_tmp3CE == (void*)2  || _tmp3CE == (void*)3)incr_str=({
const char*_tmp451="decrement";_tag_dyneither(_tmp451,sizeof(char),10);});if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp3CD,& ptr_type,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct _tmp454;
_tmp454.tag=0;_tmp454.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);{
void*_tmp452[1]={& _tmp454};Cyc_Tcutil_terr(e->loc,({const char*_tmp453="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dyneither(_tmp453,sizeof(char),74);}),_tag_dyneither(_tmp452,sizeof(void*),
1));}});({void*_tmp455=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp456="in-place inc/dec on zero-term";
_tag_dyneither(_tmp456,sizeof(char),30);}),_tag_dyneither(_tmp455,sizeof(void*),
0));});}{void*tunion_member_type=(void*)0;int f_tag=0;if(Cyc_Toc_is_tagged_union_project(
_tmp3CD,& f_tag,& tunion_member_type,1)){struct Cyc_Absyn_Exp*_tmp457=Cyc_Absyn_signed_int_exp(
1,0);_tmp457->topt=({struct Cyc_Core_Opt*_tmp458=_cycalloc(sizeof(*_tmp458));
_tmp458->v=(void*)Cyc_Absyn_sint_typ;_tmp458;});{void*_tmp459=_tmp3CE;_LL291: if((
int)_tmp459 != 0)goto _LL293;_LL292: e->r=(void*)({struct Cyc_Absyn_AssignOp_e_struct*
_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A[0]=({struct Cyc_Absyn_AssignOp_e_struct
_tmp45B;_tmp45B.tag=4;_tmp45B.f1=_tmp3CD;_tmp45B.f2=({struct Cyc_Core_Opt*_tmp45C=
_cycalloc(sizeof(*_tmp45C));_tmp45C->v=(void*)((void*)0);_tmp45C;});_tmp45B.f3=
_tmp457;_tmp45B;});_tmp45A;});Cyc_Toc_exp_to_c(nv,e);return;_LL293: if((int)
_tmp459 != 2)goto _LL295;_LL294: e->r=(void*)({struct Cyc_Absyn_AssignOp_e_struct*
_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D[0]=({struct Cyc_Absyn_AssignOp_e_struct
_tmp45E;_tmp45E.tag=4;_tmp45E.f1=_tmp3CD;_tmp45E.f2=({struct Cyc_Core_Opt*_tmp45F=
_cycalloc(sizeof(*_tmp45F));_tmp45F->v=(void*)((void*)2);_tmp45F;});_tmp45E.f3=
_tmp457;_tmp45E;});_tmp45D;});Cyc_Toc_exp_to_c(nv,e);return;_LL295:;_LL296:({
struct Cyc_String_pa_struct _tmp462;_tmp462.tag=0;_tmp462.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)incr_str);{void*_tmp460[1]={& _tmp462};Cyc_Tcutil_terr(e->loc,({
const char*_tmp461="in-place post-%s is not supported on @tagged union members";
_tag_dyneither(_tmp461,sizeof(char),59);}),_tag_dyneither(_tmp460,sizeof(void*),
1));}});({void*_tmp463=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp464="in-place inc/dec on @tagged union";
_tag_dyneither(_tmp464,sizeof(char),34);}),_tag_dyneither(_tmp463,sizeof(void*),
0));});_LL290:;}}Cyc_Toc_exp_to_c(nv,_tmp3CD);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp3CE == (void*)1
 || _tmp3CE == (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp3CE == (void*)2  || _tmp3CE == (void*)3)change=- 1;e->r=Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp465[3];_tmp465[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp465[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp465[0]=
Cyc_Absyn_address_exp(_tmp3CD,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp465,sizeof(struct Cyc_Absyn_Exp*),3));}));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
fn_e;{void*_tmp466=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3CD->topt))->v);void*_tmp467;int _tmp468;_LL298: if(_tmp466 <= (void*)4)goto
_LL29A;if(*((int*)_tmp466)!= 5)goto _LL29A;_tmp467=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp466)->f2;_LL299:{void*_tmp469=_tmp467;_LL2A3: if((int)_tmp469 != 0)goto _LL2A5;
_LL2A4: fn_e=_tmp3CE == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_char_e: Cyc_Toc__zero_arr_inplace_plus_char_e;
goto _LL2A2;_LL2A5: if((int)_tmp469 != 1)goto _LL2A7;_LL2A6: fn_e=_tmp3CE == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
goto _LL2A2;_LL2A7: if((int)_tmp469 != 2)goto _LL2A9;_LL2A8: fn_e=_tmp3CE == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_int_e: Cyc_Toc__zero_arr_inplace_plus_int_e;
goto _LL2A2;_LL2A9:;_LL2AA:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A[0]=({struct Cyc_Core_Impossible_struct
_tmp46B;_tmp46B.tag=Cyc_Core_Impossible;_tmp46B.f1=({const char*_tmp46C="impossible IntType (not char, short or int)";
_tag_dyneither(_tmp46C,sizeof(char),44);});_tmp46B;});_tmp46A;}));_LL2A2:;}goto
_LL297;_LL29A: if((int)_tmp466 != 1)goto _LL29C;_LL29B: fn_e=_tmp3CE == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_float_e:
Cyc_Toc__zero_arr_inplace_plus_float_e;goto _LL297;_LL29C: if(_tmp466 <= (void*)4)
goto _LL2A0;if(*((int*)_tmp466)!= 6)goto _LL29E;_tmp468=((struct Cyc_Absyn_DoubleType_struct*)
_tmp466)->f1;_LL29D: switch(_tmp468){case 1: _LL2AB: fn_e=_tmp3CE == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL2AC: fn_e=_tmp3CE == (
void*)1?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL297;_LL29E: if(*((int*)_tmp466)!= 4)goto _LL2A0;_LL29F: fn_e=_tmp3CE == (void*)
1?Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL297;_LL2A0:;_LL2A1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D[0]=({struct Cyc_Core_Impossible_struct
_tmp46E;_tmp46E.tag=Cyc_Core_Impossible;_tmp46E.f1=({const char*_tmp46F="impossible expression type (not int, float, double, or pointer)";
_tag_dyneither(_tmp46F,sizeof(char),64);});_tmp46E;});_tmp46D;}));_LL297:;}e->r=
Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp470[2];_tmp470[1]=Cyc_Absyn_signed_int_exp(
1,0);_tmp470[0]=_tmp3CD;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp470,sizeof(struct Cyc_Absyn_Exp*),2));}));}else{if(elt_typ == (
void*)0  && !Cyc_Absyn_is_lvalue(_tmp3CD)){Cyc_Toc_lvalue_assign(_tmp3CD,0,Cyc_Toc_incr_lvalue,
_tmp3CE);e->r=_tmp3CD->r;}}}goto _LL205;}}}_LL212: if(*((int*)_tmp3C5)!= 4)goto
_LL214;_tmp3CF=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp3C5)->f1;_tmp3D0=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp3C5)->f2;_tmp3D1=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp3C5)->f3;_LL213: {void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3CF->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3D1->topt))->v;int f_tag=0;void*tagged_member_struct_type=(void*)0;if(Cyc_Toc_is_tagged_union_project(
_tmp3CF,& f_tag,& tagged_member_struct_type,1)){Cyc_Toc_exp_to_c(nv,_tmp3CF);Cyc_Toc_exp_to_c(
nv,_tmp3D1);e->r=Cyc_Toc_tagged_union_assignop(_tmp3CF,e1_old_typ,_tmp3D0,
_tmp3D1,e2_old_typ,f_tag,tagged_member_struct_type);return;}{void*ptr_type=(void*)
0;void*elt_type=(void*)0;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp3CF,& ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,
_tmp3CF,_tmp3D0,_tmp3D1,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp3CF);Cyc_Toc_exp_to_c(nv,_tmp3CF);Cyc_Toc_exp_to_c(nv,_tmp3D1);{int done=0;
if(_tmp3D0 != 0){void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*change;{void*_tmp472=(void*)_tmp3D0->v;
_LL2AF: if((int)_tmp472 != 0)goto _LL2B1;_LL2B0: change=_tmp3D1;goto _LL2AE;_LL2B1:
if((int)_tmp472 != 2)goto _LL2B3;_LL2B2: change=Cyc_Absyn_prim1_exp((void*)2,
_tmp3D1,0);goto _LL2AE;_LL2B3:;_LL2B4:({void*_tmp473=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp474="bad t ? pointer arithmetic";
_tag_dyneither(_tmp474,sizeof(char),27);}),_tag_dyneither(_tmp473,sizeof(void*),
0));});_LL2AE:;}done=1;{struct Cyc_Absyn_Exp*_tmp475=Cyc_Toc__dyneither_ptr_inplace_plus_e;
e->r=Cyc_Toc_fncall_exp_r(_tmp475,({struct Cyc_Absyn_Exp*_tmp476[3];_tmp476[2]=
change;_tmp476[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp476[0]=
Cyc_Absyn_address_exp(_tmp3CF,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp476,sizeof(struct Cyc_Absyn_Exp*),3));}));}}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){void*_tmp477=(void*)
_tmp3D0->v;_LL2B6: if((int)_tmp477 != 0)goto _LL2B8;_LL2B7: done=1;e->r=Cyc_Toc_fncall_exp_r(
Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp3CF),({
struct Cyc_Absyn_Exp*_tmp478[2];_tmp478[1]=_tmp3D1;_tmp478[0]=_tmp3CF;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp478,sizeof(struct Cyc_Absyn_Exp*),
2));}));goto _LL2B5;_LL2B8:;_LL2B9:({void*_tmp479=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp47A="bad zero-terminated pointer arithmetic";
_tag_dyneither(_tmp47A,sizeof(char),39);}),_tag_dyneither(_tmp479,sizeof(void*),
0));});_LL2B5:;}}}if(!done){if(e1_poly)_tmp3D1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp3D1->r,0));if(!Cyc_Absyn_is_lvalue(_tmp3CF)){((void(*)(
struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
struct _tuple18*),struct _tuple18*f_env))Cyc_Toc_lvalue_assign)(_tmp3CF,0,Cyc_Toc_assignop_lvalue,({
struct _tuple18*_tmp47B=_cycalloc(sizeof(struct _tuple18)* 1);_tmp47B[0]=({struct
_tuple18 _tmp47C;_tmp47C.f1=_tmp3D0;_tmp47C.f2=_tmp3D1;_tmp47C;});_tmp47B;}));e->r=
_tmp3CF->r;}}goto _LL205;}}}}_LL214: if(*((int*)_tmp3C5)!= 6)goto _LL216;_tmp3D2=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp3C5)->f1;_tmp3D3=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3C5)->f2;_tmp3D4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3C5)->f3;_LL215:
Cyc_Toc_exp_to_c(nv,_tmp3D2);Cyc_Toc_exp_to_c(nv,_tmp3D3);Cyc_Toc_exp_to_c(nv,
_tmp3D4);goto _LL205;_LL216: if(*((int*)_tmp3C5)!= 7)goto _LL218;_tmp3D5=((struct
Cyc_Absyn_And_e_struct*)_tmp3C5)->f1;_tmp3D6=((struct Cyc_Absyn_And_e_struct*)
_tmp3C5)->f2;_LL217: Cyc_Toc_exp_to_c(nv,_tmp3D5);Cyc_Toc_exp_to_c(nv,_tmp3D6);
goto _LL205;_LL218: if(*((int*)_tmp3C5)!= 8)goto _LL21A;_tmp3D7=((struct Cyc_Absyn_Or_e_struct*)
_tmp3C5)->f1;_tmp3D8=((struct Cyc_Absyn_Or_e_struct*)_tmp3C5)->f2;_LL219: Cyc_Toc_exp_to_c(
nv,_tmp3D7);Cyc_Toc_exp_to_c(nv,_tmp3D8);goto _LL205;_LL21A: if(*((int*)_tmp3C5)!= 
9)goto _LL21C;_tmp3D9=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3C5)->f1;_tmp3DA=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp3C5)->f2;_LL21B: Cyc_Toc_exp_to_c(nv,_tmp3D9);
Cyc_Toc_exp_to_c(nv,_tmp3DA);goto _LL205;_LL21C: if(*((int*)_tmp3C5)!= 10)goto
_LL21E;_tmp3DB=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp3C5)->f1;_tmp3DC=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp3C5)->f2;_LL21D: _tmp3DD=_tmp3DB;
_tmp3DE=_tmp3DC;goto _LL21F;_LL21E: if(*((int*)_tmp3C5)!= 11)goto _LL220;_tmp3DD=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp3C5)->f1;_tmp3DE=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3C5)->f2;_tmp3DF=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3C5)->f3;if(_tmp3DF
!= 0)goto _LL220;_LL21F: Cyc_Toc_exp_to_c(nv,_tmp3DD);((void(*)(void(*f)(struct Cyc_Toc_Env*,
struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(
Cyc_Toc_exp_to_c,nv,_tmp3DE);goto _LL205;_LL220: if(*((int*)_tmp3C5)!= 11)goto
_LL222;_tmp3E0=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3C5)->f1;_tmp3E1=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp3C5)->f2;_tmp3E2=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3C5)->f3;if(_tmp3E2 == 0)goto _LL222;_tmp3E3=*_tmp3E2;_tmp3E4=_tmp3E3.num_varargs;
_tmp3E5=_tmp3E3.injectors;_tmp3E6=_tmp3E3.vai;_LL221: {struct Cyc_Toc_Env _tmp47E;
struct _RegionHandle*_tmp47F;struct Cyc_Toc_Env*_tmp47D=nv;_tmp47E=*_tmp47D;
_tmp47F=_tmp47E.rgn;{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp3E4,0);void*cva_type=Cyc_Toc_typ_to_c(_tmp3E6->type);void*
arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp3E1);int num_normargs=num_args - _tmp3E4;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp3E1=_tmp3E1->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3E1))->hd);
new_args=({struct Cyc_List_List*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->hd=(
struct Cyc_Absyn_Exp*)_tmp3E1->hd;_tmp480->tl=new_args;_tmp480;});}}new_args=({
struct Cyc_List_List*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp482[3];_tmp482[2]=
num_varargs_exp;_tmp482[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp482[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp482,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp481->tl=
new_args;_tmp481;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp3E0);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp3E0,new_args,0),0);if(_tmp3E6->inject){
struct Cyc_Absyn_Datatypedecl*tud;{void*_tmp483=Cyc_Tcutil_compress(_tmp3E6->type);
struct Cyc_Absyn_DatatypeInfo _tmp484;union Cyc_Absyn_DatatypeInfoU _tmp485;struct
Cyc_Absyn_Datatypedecl**_tmp486;struct Cyc_Absyn_Datatypedecl*_tmp487;_LL2BB: if(
_tmp483 <= (void*)4)goto _LL2BD;if(*((int*)_tmp483)!= 2)goto _LL2BD;_tmp484=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp483)->f1;_tmp485=_tmp484.datatype_info;
if((_tmp485.KnownDatatype).tag != 2)goto _LL2BD;_tmp486=(struct Cyc_Absyn_Datatypedecl**)(
_tmp485.KnownDatatype).val;_tmp487=*_tmp486;_LL2BC: tud=_tmp487;goto _LL2BA;_LL2BD:;
_LL2BE:({void*_tmp488=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp489="toc: unknown datatype in vararg with inject";
_tag_dyneither(_tmp489,sizeof(char),44);}),_tag_dyneither(_tmp488,sizeof(void*),
0));});_LL2BA:;}{struct _dyneither_ptr vs=({unsigned int _tmp493=(unsigned int)
_tmp3E4;struct _tuple1**_tmp494=(struct _tuple1**)_region_malloc(_tmp47F,
_check_times(sizeof(struct _tuple1*),_tmp493));struct _dyneither_ptr _tmp496=
_tag_dyneither(_tmp494,sizeof(struct _tuple1*),_tmp493);{unsigned int _tmp495=
_tmp493;unsigned int i;for(i=0;i < _tmp495;i ++){_tmp494[i]=Cyc_Toc_temp_var();}}
_tmp496;});if(_tmp3E4 != 0){struct Cyc_List_List*_tmp48A=0;{int i=_tmp3E4 - 1;for(0;
i >= 0;-- i){_tmp48A=({struct Cyc_List_List*_tmp48B=_cycalloc(sizeof(*_tmp48B));
_tmp48B->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct
_tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));
_tmp48B->tl=_tmp48A;_tmp48B;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct
Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp48A,0),s,0);{int i=0;for(0;
_tmp3E1 != 0;(((_tmp3E1=_tmp3E1->tl,_tmp3E5=_tmp3E5->tl)),++ i)){struct Cyc_Absyn_Exp*
arg=(struct Cyc_Absyn_Exp*)_tmp3E1->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Datatypefield _tmp48D;struct
_tuple1*_tmp48E;struct Cyc_List_List*_tmp48F;struct Cyc_Absyn_Datatypefield*
_tmp48C=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(
_tmp3E5))->hd;_tmp48D=*_tmp48C;_tmp48E=_tmp48D.name;_tmp48F=_tmp48D.typs;{void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp48F))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_datatype_tag(
tud,_tmp48E,1),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp48E,({const char*_tmp490="_struct";_tag_dyneither(_tmp490,sizeof(char),8);}))),
0,s,0);}}}}else{struct Cyc_List_List*_tmp491=({struct _tuple16*_tmp492[3];_tmp492[
2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp492[1]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(
0,0));_tmp492[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp492,sizeof(struct
_tuple16*),3));});s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}}}else{{int i=0;for(0;_tmp3E1
!= 0;(_tmp3E1=_tmp3E1->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp3E1->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp3E1->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}e->r=Cyc_Toc_stmt_exp_r(
s);}}goto _LL205;}_LL222: if(*((int*)_tmp3C5)!= 12)goto _LL224;_tmp3E7=((struct Cyc_Absyn_Throw_e_struct*)
_tmp3C5)->f1;_LL223: Cyc_Toc_exp_to_c(nv,_tmp3E7);e->r=(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp3E7),0))->r;goto _LL205;_LL224:
if(*((int*)_tmp3C5)!= 13)goto _LL226;_tmp3E8=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp3C5)->f1;_LL225: Cyc_Toc_exp_to_c(nv,_tmp3E8);goto _LL205;_LL226: if(*((int*)
_tmp3C5)!= 14)goto _LL228;_tmp3E9=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp3C5)->f1;
_tmp3EA=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp3C5)->f2;_LL227: Cyc_Toc_exp_to_c(
nv,_tmp3E9);for(0;_tmp3EA != 0;_tmp3EA=_tmp3EA->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp3EA->hd);if(((k != (void*)6  && k != (void*)3) && k != (void*)4) && k != (
void*)5){{void*_tmp497=Cyc_Tcutil_compress((void*)_tmp3EA->hd);_LL2C0: if(_tmp497
<= (void*)4)goto _LL2C4;if(*((int*)_tmp497)!= 1)goto _LL2C2;_LL2C1: goto _LL2C3;
_LL2C2: if(*((int*)_tmp497)!= 2)goto _LL2C4;_LL2C3: continue;_LL2C4:;_LL2C5: e->r=(
Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),_tmp3E9,0))->r;goto _LL2BF;_LL2BF:;}break;}}goto _LL205;
_LL228: if(*((int*)_tmp3C5)!= 15)goto _LL22A;_tmp3EB=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp3C5)->f1;_tmp3EC=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3C5)->f1);
_tmp3ED=((struct Cyc_Absyn_Cast_e_struct*)_tmp3C5)->f2;_tmp3EE=((struct Cyc_Absyn_Cast_e_struct*)
_tmp3C5)->f3;_tmp3EF=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp3C5)->f4;_LL229: {
void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3ED->topt))->v;void*
new_typ=*_tmp3EC;*_tmp3EC=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,_tmp3ED);{
struct _tuple0 _tmp499=({struct _tuple0 _tmp498;_tmp498.f1=Cyc_Tcutil_compress(
old_t2);_tmp498.f2=Cyc_Tcutil_compress(new_typ);_tmp498;});void*_tmp49A;struct
Cyc_Absyn_PtrInfo _tmp49B;void*_tmp49C;struct Cyc_Absyn_PtrInfo _tmp49D;void*
_tmp49E;struct Cyc_Absyn_PtrInfo _tmp49F;void*_tmp4A0;_LL2C7: _tmp49A=_tmp499.f1;
if(_tmp49A <= (void*)4)goto _LL2C9;if(*((int*)_tmp49A)!= 4)goto _LL2C9;_tmp49B=((
struct Cyc_Absyn_PointerType_struct*)_tmp49A)->f1;_tmp49C=_tmp499.f2;if(_tmp49C <= (
void*)4)goto _LL2C9;if(*((int*)_tmp49C)!= 4)goto _LL2C9;_tmp49D=((struct Cyc_Absyn_PointerType_struct*)
_tmp49C)->f1;_LL2C8: {int _tmp4A1=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,(_tmp49B.ptr_atts).nullable);int _tmp4A2=((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,(_tmp49D.ptr_atts).nullable);void*_tmp4A3=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp49B.ptr_atts).bounds);void*_tmp4A4=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp49D.ptr_atts).bounds);int _tmp4A5=((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp49B.ptr_atts).zero_term);
int _tmp4A6=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(
_tmp49D.ptr_atts).zero_term);{struct _tuple0 _tmp4A8=({struct _tuple0 _tmp4A7;
_tmp4A7.f1=_tmp4A3;_tmp4A7.f2=_tmp4A4;_tmp4A7;});void*_tmp4A9;struct Cyc_Absyn_Exp*
_tmp4AA;void*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AC;void*_tmp4AD;struct Cyc_Absyn_Exp*
_tmp4AE;void*_tmp4AF;void*_tmp4B0;void*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B2;void*
_tmp4B3;void*_tmp4B4;_LL2CE: _tmp4A9=_tmp4A8.f1;if(_tmp4A9 <= (void*)1)goto _LL2D0;
if(*((int*)_tmp4A9)!= 0)goto _LL2D0;_tmp4AA=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4A9)->f1;_tmp4AB=_tmp4A8.f2;if(_tmp4AB <= (void*)1)goto _LL2D0;if(*((int*)
_tmp4AB)!= 0)goto _LL2D0;_tmp4AC=((struct Cyc_Absyn_Upper_b_struct*)_tmp4AB)->f1;
_LL2CF: if((!Cyc_Evexp_c_can_eval(_tmp4AA) || !Cyc_Evexp_c_can_eval(_tmp4AC)) && 
!Cyc_Evexp_same_const_exp(_tmp4AA,_tmp4AC))({void*_tmp4B5=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp4B6="can't validate cast due to potential size differences";
_tag_dyneither(_tmp4B6,sizeof(char),54);}),_tag_dyneither(_tmp4B5,sizeof(void*),
0));});if(_tmp4A1  && !_tmp4A2){if(Cyc_Toc_is_toplevel(nv))({void*_tmp4B7=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp4B8="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp4B8,
sizeof(char),44);}),_tag_dyneither(_tmp4B7,sizeof(void*),0));});if(_tmp3EF != (
void*)2)({struct Cyc_String_pa_struct _tmp4BB;_tmp4BB.tag=0;_tmp4BB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp4B9[1]={&
_tmp4BB};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp4BA="null-check conversion mis-classified: %s";_tag_dyneither(
_tmp4BA,sizeof(char),41);}),_tag_dyneither(_tmp4B9,sizeof(void*),1));}});{int
do_null_check=Cyc_Toc_need_null_check(_tmp3ED);if(do_null_check){if(!_tmp3EE)({
void*_tmp4BC=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp4BD="inserted null check due to implicit cast from * to @ type";
_tag_dyneither(_tmp4BD,sizeof(char),58);}),_tag_dyneither(_tmp4BC,sizeof(void*),
0));});e->r=Cyc_Toc_cast_it_r(*_tmp3EC,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE->hd=_tmp3ED;
_tmp4BE->tl=0;_tmp4BE;}),0));}}}goto _LL2CD;_LL2D0: _tmp4AD=_tmp4A8.f1;if(_tmp4AD
<= (void*)1)goto _LL2D2;if(*((int*)_tmp4AD)!= 0)goto _LL2D2;_tmp4AE=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4AD)->f1;_tmp4AF=_tmp4A8.f2;if((int)_tmp4AF != 0)goto _LL2D2;_LL2D1: if(!Cyc_Evexp_c_can_eval(
_tmp4AE))({void*_tmp4BF=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4C0="cannot perform coercion since numelts cannot be determined statically.";
_tag_dyneither(_tmp4C0,sizeof(char),71);}),_tag_dyneither(_tmp4BF,sizeof(void*),
0));});if(_tmp3EF == (void*)2)({struct Cyc_String_pa_struct _tmp4C3;_tmp4C3.tag=0;
_tmp4C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{
void*_tmp4C1[1]={& _tmp4C3};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp4C2="conversion mis-classified as null-check: %s";
_tag_dyneither(_tmp4C2,sizeof(char),44);}),_tag_dyneither(_tmp4C1,sizeof(void*),
1));}});if(Cyc_Toc_is_toplevel(nv)){if((_tmp4A5  && !(_tmp49D.elt_tq).real_const)
 && !_tmp4A6)_tmp4AE=Cyc_Absyn_prim2_exp((void*)2,_tmp4AE,Cyc_Absyn_uint_exp(1,0),
0);e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp4AE,_tmp3ED))->r;}else{struct
Cyc_Absyn_Exp*_tmp4C4=Cyc_Toc__tag_dyneither_e;if(_tmp4A5){struct _tuple1*_tmp4C5=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp4C6=Cyc_Absyn_var_exp(_tmp4C5,0);
struct Cyc_Absyn_Exp*arg3;{void*_tmp4C7=_tmp3ED->r;union Cyc_Absyn_Cnst _tmp4C8;
struct _dyneither_ptr _tmp4C9;_LL2D7: if(*((int*)_tmp4C7)!= 0)goto _LL2D9;_tmp4C8=((
struct Cyc_Absyn_Const_e_struct*)_tmp4C7)->f1;if((_tmp4C8.String_c).tag != 7)goto
_LL2D9;_tmp4C9=(struct _dyneither_ptr)(_tmp4C8.String_c).val;_LL2D8: arg3=_tmp4AE;
goto _LL2D6;_LL2D9:;_LL2DA: arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(&
Cyc_Toc__get_zero_arr_size_functionSet,_tmp3ED),({struct Cyc_Absyn_Exp*_tmp4CA[2];
_tmp4CA[1]=_tmp4AE;_tmp4CA[0]=_tmp4C6;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CA,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);goto _LL2D6;_LL2D6:;}if(!_tmp4A6  && !(_tmp49D.elt_tq).real_const)arg3=
Cyc_Absyn_prim2_exp((void*)2,arg3,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp4CB=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp49D.elt_typ),0);struct Cyc_Absyn_Exp*
_tmp4CC=Cyc_Absyn_fncall_exp(_tmp4C4,({struct Cyc_Absyn_Exp*_tmp4CE[3];_tmp4CE[2]=
arg3;_tmp4CE[1]=_tmp4CB;_tmp4CE[0]=_tmp4C6;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CE,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);struct Cyc_Absyn_Stmt*_tmp4CD=Cyc_Absyn_exp_stmt(_tmp4CC,0);_tmp4CD=Cyc_Absyn_declare_stmt(
_tmp4C5,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)_tmp3ED,_tmp4CD,0);e->r=
Cyc_Toc_stmt_exp_r(_tmp4CD);}}else{e->r=Cyc_Toc_fncall_exp_r(_tmp4C4,({struct Cyc_Absyn_Exp*
_tmp4CF[3];_tmp4CF[2]=_tmp4AE;_tmp4CF[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp49D.elt_typ),0);_tmp4CF[0]=_tmp3ED;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4CF,sizeof(struct Cyc_Absyn_Exp*),
3));}));}}goto _LL2CD;_LL2D2: _tmp4B0=_tmp4A8.f1;if((int)_tmp4B0 != 0)goto _LL2D4;
_tmp4B1=_tmp4A8.f2;if(_tmp4B1 <= (void*)1)goto _LL2D4;if(*((int*)_tmp4B1)!= 0)goto
_LL2D4;_tmp4B2=((struct Cyc_Absyn_Upper_b_struct*)_tmp4B1)->f1;_LL2D3: if(!Cyc_Evexp_c_can_eval(
_tmp4B2))({void*_tmp4D0=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4D1="cannot perform coercion since numelts cannot be determined statically.";
_tag_dyneither(_tmp4D1,sizeof(char),71);}),_tag_dyneither(_tmp4D0,sizeof(void*),
0));});if(Cyc_Toc_is_toplevel(nv))({void*_tmp4D2=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4D3="can't coerce t? to t* or t@ at the top-level";
_tag_dyneither(_tmp4D3,sizeof(char),45);}),_tag_dyneither(_tmp4D2,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp4D4=_tmp4B2;if(_tmp4A5  && !_tmp4A6)_tmp4D4=Cyc_Absyn_add_exp(
_tmp4B2,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp4D5=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp4D6=Cyc_Absyn_fncall_exp(_tmp4D5,({struct Cyc_Absyn_Exp*
_tmp4D8[3];_tmp4D8[2]=_tmp4D4;_tmp4D8[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp49B.elt_typ),0);_tmp4D8[0]=_tmp3ED;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4D8,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp4A2)_tmp4D6->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp4D7=_cycalloc(sizeof(*_tmp4D7));_tmp4D7->hd=Cyc_Absyn_copy_exp(
_tmp4D6);_tmp4D7->tl=0;_tmp4D7;}));e->r=Cyc_Toc_cast_it_r(*_tmp3EC,_tmp4D6);goto
_LL2CD;}}_LL2D4: _tmp4B3=_tmp4A8.f1;if((int)_tmp4B3 != 0)goto _LL2CD;_tmp4B4=
_tmp4A8.f2;if((int)_tmp4B4 != 0)goto _LL2CD;_LL2D5: DynCast: if((_tmp4A5  && !_tmp4A6)
 && !(_tmp49D.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({void*_tmp4D9=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const
char*_tmp4DA="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dyneither(_tmp4DA,sizeof(char),70);}),_tag_dyneither(_tmp4D9,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp4DB=Cyc_Toc__dyneither_ptr_decrease_size_e;e->r=
Cyc_Toc_fncall_exp_r(_tmp4DB,({struct Cyc_Absyn_Exp*_tmp4DC[3];_tmp4DC[2]=Cyc_Absyn_uint_exp(
1,0);_tmp4DC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp49B.elt_typ),0);
_tmp4DC[0]=_tmp3ED;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp4DC,sizeof(struct Cyc_Absyn_Exp*),3));}));}}goto _LL2CD;_LL2CD:;}
goto _LL2C6;}_LL2C9: _tmp49E=_tmp499.f1;if(_tmp49E <= (void*)4)goto _LL2CB;if(*((int*)
_tmp49E)!= 4)goto _LL2CB;_tmp49F=((struct Cyc_Absyn_PointerType_struct*)_tmp49E)->f1;
_tmp4A0=_tmp499.f2;if(_tmp4A0 <= (void*)4)goto _LL2CB;if(*((int*)_tmp4A0)!= 5)goto
_LL2CB;_LL2CA:{void*_tmp4DD=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(_tmp49F.ptr_atts).bounds);
_LL2DC: if((int)_tmp4DD != 0)goto _LL2DE;_LL2DD: _tmp3ED->r=Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp(_tmp3ED->r,_tmp3ED->loc),Cyc_Toc_curr_sp);goto _LL2DB;_LL2DE:;
_LL2DF: goto _LL2DB;_LL2DB:;}goto _LL2C6;_LL2CB:;_LL2CC: goto _LL2C6;_LL2C6:;}goto
_LL205;}_LL22A: if(*((int*)_tmp3C5)!= 16)goto _LL22C;_tmp3F0=((struct Cyc_Absyn_Address_e_struct*)
_tmp3C5)->f1;_LL22B:{void*_tmp4DE=_tmp3F0->r;struct _tuple1*_tmp4DF;struct Cyc_List_List*
_tmp4E0;struct Cyc_List_List*_tmp4E1;struct Cyc_List_List*_tmp4E2;_LL2E1: if(*((int*)
_tmp4DE)!= 30)goto _LL2E3;_tmp4DF=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4DE)->f1;
_tmp4E0=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4DE)->f2;_tmp4E1=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4DE)->f3;_LL2E2: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp4E5;_tmp4E5.tag=0;_tmp4E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp3F0->loc));{void*_tmp4E3[1]={& _tmp4E5};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp4E4="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp4E4,
sizeof(char),42);}),_tag_dyneither(_tmp4E3,sizeof(void*),1));}});e->r=(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v,_tmp4E0 != 0,1,0,
_tmp4E1,_tmp4DF))->r;goto _LL2E0;_LL2E3: if(*((int*)_tmp4DE)!= 26)goto _LL2E5;
_tmp4E2=((struct Cyc_Absyn_Tuple_e_struct*)_tmp4DE)->f1;_LL2E4: if(Cyc_Toc_is_toplevel(
nv))({struct Cyc_String_pa_struct _tmp4E8;_tmp4E8.tag=0;_tmp4E8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp3F0->loc));{
void*_tmp4E6[1]={& _tmp4E8};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_unimp)(({const char*_tmp4E7="%s: & on non-identifiers at the top-level";
_tag_dyneither(_tmp4E7,sizeof(char),42);}),_tag_dyneither(_tmp4E6,sizeof(void*),
1));}});e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp4E2))->r;goto _LL2E0;_LL2E5:;_LL2E6:
Cyc_Toc_exp_to_c(nv,_tmp3F0);if(!Cyc_Absyn_is_lvalue(_tmp3F0)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
int),int f_env))Cyc_Toc_lvalue_assign)(_tmp3F0,0,Cyc_Toc_address_lvalue,1);e->r=
Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp3F0);}goto _LL2E0;_LL2E0:;}goto _LL205;_LL22C: if(*((int*)_tmp3C5)!= 17)goto
_LL22E;_tmp3F1=((struct Cyc_Absyn_New_e_struct*)_tmp3C5)->f1;_tmp3F2=((struct Cyc_Absyn_New_e_struct*)
_tmp3C5)->f2;_LL22D: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp4EB;_tmp4EB.tag=0;_tmp4EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp3F2->loc));{void*_tmp4E9[1]={& _tmp4EB};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp4EA="%s: new at top-level";_tag_dyneither(_tmp4EA,sizeof(char),21);}),
_tag_dyneither(_tmp4E9,sizeof(void*),1));}});{void*_tmp4EC=_tmp3F2->r;struct Cyc_List_List*
_tmp4ED;struct Cyc_Absyn_Vardecl*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_Absyn_Exp*
_tmp4F0;int _tmp4F1;struct _tuple1*_tmp4F2;struct Cyc_List_List*_tmp4F3;struct Cyc_List_List*
_tmp4F4;struct Cyc_Absyn_Aggrdecl*_tmp4F5;struct Cyc_List_List*_tmp4F6;_LL2E8: if(*((
int*)_tmp4EC)!= 28)goto _LL2EA;_tmp4ED=((struct Cyc_Absyn_Array_e_struct*)_tmp4EC)->f1;
_LL2E9: {struct _tuple1*_tmp4F7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp4F8=
Cyc_Absyn_var_exp(_tmp4F7,0);struct Cyc_Absyn_Stmt*_tmp4F9=Cyc_Toc_init_array(nv,
_tmp4F8,_tmp4ED,Cyc_Absyn_exp_stmt(_tmp4F8,0));void*old_elt_typ;{void*_tmp4FA=
Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp4FB;void*_tmp4FC;struct
Cyc_Absyn_Tqual _tmp4FD;struct Cyc_Absyn_PtrAtts _tmp4FE;union Cyc_Absyn_Constraint*
_tmp4FF;_LL2F3: if(_tmp4FA <= (void*)4)goto _LL2F5;if(*((int*)_tmp4FA)!= 4)goto
_LL2F5;_tmp4FB=((struct Cyc_Absyn_PointerType_struct*)_tmp4FA)->f1;_tmp4FC=
_tmp4FB.elt_typ;_tmp4FD=_tmp4FB.elt_tq;_tmp4FE=_tmp4FB.ptr_atts;_tmp4FF=_tmp4FE.zero_term;
_LL2F4: old_elt_typ=_tmp4FC;goto _LL2F2;_LL2F5:;_LL2F6: old_elt_typ=({void*_tmp500=
0;Cyc_Toc_toc_impos(({const char*_tmp501="exp_to_c:new array expression doesn't have ptr type";
_tag_dyneither(_tmp501,sizeof(char),52);}),_tag_dyneither(_tmp500,sizeof(void*),
0));});_LL2F2:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp502=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp503=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp4ED),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp3F1 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp503);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp3F1;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp503);}e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp4F7,_tmp502,(struct Cyc_Absyn_Exp*)e1,_tmp4F9,0));goto
_LL2E7;}}_LL2EA: if(*((int*)_tmp4EC)!= 29)goto _LL2EC;_tmp4EE=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4EC)->f1;_tmp4EF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4EC)->f2;
_tmp4F0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4EC)->f3;_tmp4F1=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp4EC)->f4;_LL2EB: {int is_dyneither_ptr=0;{
void*_tmp504=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo _tmp505;void*
_tmp506;struct Cyc_Absyn_Tqual _tmp507;struct Cyc_Absyn_PtrAtts _tmp508;union Cyc_Absyn_Constraint*
_tmp509;union Cyc_Absyn_Constraint*_tmp50A;_LL2F8: if(_tmp504 <= (void*)4)goto
_LL2FA;if(*((int*)_tmp504)!= 4)goto _LL2FA;_tmp505=((struct Cyc_Absyn_PointerType_struct*)
_tmp504)->f1;_tmp506=_tmp505.elt_typ;_tmp507=_tmp505.elt_tq;_tmp508=_tmp505.ptr_atts;
_tmp509=_tmp508.bounds;_tmp50A=_tmp508.zero_term;_LL2F9: is_dyneither_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp509)== (void*)0;goto _LL2F7;_LL2FA:;_LL2FB:({void*
_tmp50B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp50C="exp_to_c: comprehension not an array type";_tag_dyneither(
_tmp50C,sizeof(char),42);}),_tag_dyneither(_tmp50B,sizeof(void*),0));});_LL2F7:;}{
struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F0->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,
Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp4EF);{struct Cyc_Absyn_Exp*_tmp50D=Cyc_Absyn_var_exp(
max,0);if(_tmp4F1)_tmp50D=Cyc_Absyn_add_exp(_tmp50D,Cyc_Absyn_uint_exp(1,0),0);{
struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(a,0),
_tmp4EE,Cyc_Absyn_var_exp(max,0),_tmp4F0,_tmp4F1,Cyc_Toc_skip_stmt_dl(),1);
struct Cyc_Toc_Env _tmp50F;struct _RegionHandle*_tmp510;struct Cyc_Toc_Env*_tmp50E=
nv;_tmp50F=*_tmp50E;_tmp510=_tmp50F.rgn;{struct Cyc_List_List*decls=({struct Cyc_List_List*
_tmp521=_region_malloc(_tmp510,sizeof(*_tmp521));_tmp521->hd=({struct _tuple19*
_tmp522=_region_malloc(_tmp510,sizeof(*_tmp522));_tmp522->f1=max;_tmp522->f2=Cyc_Absyn_uint_typ;
_tmp522->f3=(struct Cyc_Absyn_Exp*)_tmp4EF;_tmp522;});_tmp521->tl=0;_tmp521;});
struct Cyc_Absyn_Exp*ai;if(_tmp3F1 == 0  || Cyc_Absyn_no_regions)ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*
_tmp511[2];_tmp511[1]=_tmp50D;_tmp511[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp511,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{struct Cyc_Absyn_Exp*r=(
struct Cyc_Absyn_Exp*)_tmp3F1;Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp512[2];_tmp512[1]=_tmp50D;
_tmp512[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp512,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({
struct Cyc_List_List*_tmp513=_region_malloc(_tmp510,sizeof(*_tmp513));_tmp513->hd=({
struct _tuple19*_tmp514=_region_malloc(_tmp510,sizeof(*_tmp514));_tmp514->f1=a;
_tmp514->f2=ptr_typ;_tmp514->f3=(struct Cyc_Absyn_Exp*)ainit;_tmp514;});_tmp513->tl=
decls;_tmp513;});if(is_dyneither_ptr){struct _tuple1*_tmp515=Cyc_Toc_temp_var();
void*_tmp516=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp517=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp518=Cyc_Absyn_fncall_exp(_tmp517,({struct Cyc_Absyn_Exp*
_tmp51B[3];_tmp51B[2]=_tmp50D;_tmp51B[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);
_tmp51B[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp51B,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
decls=({struct Cyc_List_List*_tmp519=_region_malloc(_tmp510,sizeof(*_tmp519));
_tmp519->hd=({struct _tuple19*_tmp51A=_region_malloc(_tmp510,sizeof(*_tmp51A));
_tmp51A->f1=_tmp515;_tmp51A->f2=_tmp516;_tmp51A->f3=(struct Cyc_Absyn_Exp*)
_tmp518;_tmp51A;});_tmp519->tl=decls;_tmp519;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(_tmp515,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(a,0),0),0);}{struct Cyc_List_List*_tmp51C=decls;for(0;_tmp51C != 
0;_tmp51C=_tmp51C->tl){struct _tuple1*_tmp51E;void*_tmp51F;struct Cyc_Absyn_Exp*
_tmp520;struct _tuple19 _tmp51D=*((struct _tuple19*)_tmp51C->hd);_tmp51E=_tmp51D.f1;
_tmp51F=_tmp51D.f2;_tmp520=_tmp51D.f3;s=Cyc_Absyn_declare_stmt(_tmp51E,_tmp51F,
_tmp520,s,0);}}e->r=Cyc_Toc_stmt_exp_r(s);}}goto _LL2E7;}}}}_LL2EC: if(*((int*)
_tmp4EC)!= 30)goto _LL2EE;_tmp4F2=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4EC)->f1;
_tmp4F3=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4EC)->f2;_tmp4F4=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp4EC)->f3;_tmp4F5=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp4EC)->f4;_LL2ED: e->r=(
Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F2->topt))->v,
_tmp4F3 != 0,1,_tmp3F1,_tmp4F4,_tmp4F2))->r;goto _LL2E7;_LL2EE: if(*((int*)_tmp4EC)
!= 26)goto _LL2F0;_tmp4F6=((struct Cyc_Absyn_Tuple_e_struct*)_tmp4EC)->f1;_LL2EF: e->r=(
Cyc_Toc_init_tuple(nv,1,_tmp3F1,_tmp4F6))->r;goto _LL2E7;_LL2F0:;_LL2F1: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F2->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp523=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp524=Cyc_Absyn_var_exp(_tmp523,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp524,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp3F1 == 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp3F1;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp525=_tmp3F2->r;void*
_tmp526;struct Cyc_Absyn_Exp*_tmp527;_LL2FD: if(*((int*)_tmp525)!= 15)goto _LL2FF;
_tmp526=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp525)->f1;_tmp527=((struct Cyc_Absyn_Cast_e_struct*)
_tmp525)->f2;_LL2FE:{struct _tuple0 _tmp529=({struct _tuple0 _tmp528;_tmp528.f1=Cyc_Tcutil_compress(
_tmp526);_tmp528.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp527->topt))->v);_tmp528;});void*_tmp52A;struct Cyc_Absyn_PtrInfo _tmp52B;void*
_tmp52C;struct Cyc_Absyn_PtrAtts _tmp52D;union Cyc_Absyn_Constraint*_tmp52E;void*
_tmp52F;struct Cyc_Absyn_PtrInfo _tmp530;struct Cyc_Absyn_PtrAtts _tmp531;union Cyc_Absyn_Constraint*
_tmp532;_LL302: _tmp52A=_tmp529.f1;if(_tmp52A <= (void*)4)goto _LL304;if(*((int*)
_tmp52A)!= 4)goto _LL304;_tmp52B=((struct Cyc_Absyn_PointerType_struct*)_tmp52A)->f1;
_tmp52C=_tmp52B.elt_typ;_tmp52D=_tmp52B.ptr_atts;_tmp52E=_tmp52D.bounds;_tmp52F=
_tmp529.f2;if(_tmp52F <= (void*)4)goto _LL304;if(*((int*)_tmp52F)!= 4)goto _LL304;
_tmp530=((struct Cyc_Absyn_PointerType_struct*)_tmp52F)->f1;_tmp531=_tmp530.ptr_atts;
_tmp532=_tmp531.bounds;_LL303:{struct _tuple0 _tmp534=({struct _tuple0 _tmp533;
_tmp533.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp52E);_tmp533.f2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp532);_tmp533;});void*_tmp535;void*_tmp536;struct Cyc_Absyn_Exp*
_tmp537;_LL307: _tmp535=_tmp534.f1;if((int)_tmp535 != 0)goto _LL309;_tmp536=_tmp534.f2;
if(_tmp536 <= (void*)1)goto _LL309;if(*((int*)_tmp536)!= 0)goto _LL309;_tmp537=((
struct Cyc_Absyn_Upper_b_struct*)_tmp536)->f1;_LL308: Cyc_Toc_exp_to_c(nv,_tmp527);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);done=1;{struct Cyc_Absyn_Exp*
_tmp538=Cyc_Toc__init_dyneither_ptr_e;e->r=Cyc_Toc_fncall_exp_r(_tmp538,({struct
Cyc_Absyn_Exp*_tmp539[4];_tmp539[3]=_tmp537;_tmp539[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp52C),0);_tmp539[1]=_tmp527;_tmp539[0]=mexp;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp539,sizeof(struct Cyc_Absyn_Exp*),
4));}));goto _LL306;}_LL309:;_LL30A: goto _LL306;_LL306:;}goto _LL301;_LL304:;_LL305:
goto _LL301;_LL301:;}goto _LL2FC;_LL2FF:;_LL300: goto _LL2FC;_LL2FC:;}if(!done){
struct Cyc_Absyn_Stmt*_tmp53A=Cyc_Absyn_exp_stmt(_tmp524,0);struct Cyc_Absyn_Exp*
_tmp53B=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp3F2);_tmp53A=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp524,_tmp53B,0),_tmp3F2,0),
_tmp53A,0);{void*_tmp53C=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp523,_tmp53C,(struct Cyc_Absyn_Exp*)mexp,_tmp53A,0));}}
goto _LL2E7;}}_LL2E7:;}goto _LL205;_LL22E: if(*((int*)_tmp3C5)!= 19)goto _LL230;
_tmp3F3=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp3C5)->f1;_LL22F: Cyc_Toc_exp_to_c(
nv,_tmp3F3);goto _LL205;_LL230: if(*((int*)_tmp3C5)!= 18)goto _LL232;_tmp3F4=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3C5)->f1;_LL231: e->r=(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp53E;_tmp53E.tag=18;_tmp53E.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3F4);_tmp53E;});
_tmp53D;});goto _LL205;_LL232: if(*((int*)_tmp3C5)!= 21)goto _LL234;_LL233:({void*
_tmp53F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp540="__gen() in code generator";_tag_dyneither(_tmp540,sizeof(char),
26);}),_tag_dyneither(_tmp53F,sizeof(void*),0));});_LL234: if(*((int*)_tmp3C5)!= 
20)goto _LL236;_tmp3F5=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3C5)->f1;
_tmp3F6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3C5)->f2;if(*((int*)
_tmp3F6)!= 0)goto _LL236;_tmp3F7=((struct Cyc_Absyn_StructField_struct*)_tmp3F6)->f1;
_LL235: e->r=(void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp541=_cycalloc(sizeof(*
_tmp541));_tmp541[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp542;_tmp542.tag=20;
_tmp542.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3F5);_tmp542.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543[0]=({
struct Cyc_Absyn_StructField_struct _tmp544;_tmp544.tag=0;_tmp544.f1=_tmp3F7;
_tmp544;});_tmp543;}));_tmp542;});_tmp541;});goto _LL205;_LL236: if(*((int*)
_tmp3C5)!= 20)goto _LL238;_tmp3F8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp3C5)->f1;_tmp3F9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3C5)->f2;
if(*((int*)_tmp3F9)!= 1)goto _LL238;_tmp3FA=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp3F9)->f1;_LL237:{void*_tmp545=Cyc_Tcutil_compress(_tmp3F8);struct Cyc_Absyn_AggrInfo
_tmp546;union Cyc_Absyn_AggrInfoU _tmp547;struct Cyc_List_List*_tmp548;_LL30C: if(
_tmp545 <= (void*)4)goto _LL314;if(*((int*)_tmp545)!= 10)goto _LL30E;_tmp546=((
struct Cyc_Absyn_AggrType_struct*)_tmp545)->f1;_tmp547=_tmp546.aggr_info;_LL30D: {
struct Cyc_Absyn_Aggrdecl*_tmp549=Cyc_Absyn_get_known_aggrdecl(_tmp547);if(
_tmp549->impl == 0)({void*_tmp54A=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp54B="struct fields must be known";
_tag_dyneither(_tmp54B,sizeof(char),28);}),_tag_dyneither(_tmp54A,sizeof(void*),
0));});_tmp548=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp549->impl))->fields;
goto _LL30F;}_LL30E: if(*((int*)_tmp545)!= 11)goto _LL310;_tmp548=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp545)->f2;_LL30F: {struct Cyc_Absyn_Aggrfield*_tmp54C=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp548,(int)_tmp3FA);e->r=(void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D[0]=({
struct Cyc_Absyn_Offsetof_e_struct _tmp54E;_tmp54E.tag=20;_tmp54E.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp3F8);_tmp54E.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp54F=
_cycalloc(sizeof(*_tmp54F));_tmp54F[0]=({struct Cyc_Absyn_StructField_struct
_tmp550;_tmp550.tag=0;_tmp550.f1=_tmp54C->name;_tmp550;});_tmp54F;}));_tmp54E;});
_tmp54D;});goto _LL30B;}_LL310: if(*((int*)_tmp545)!= 9)goto _LL312;_LL311: e->r=(
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp551=_cycalloc(sizeof(*_tmp551));
_tmp551[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp552;_tmp552.tag=20;_tmp552.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp3F8);_tmp552.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*
_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553[0]=({struct Cyc_Absyn_StructField_struct
_tmp554;_tmp554.tag=0;_tmp554.f1=Cyc_Absyn_fieldname((int)(_tmp3FA + 1));_tmp554;});
_tmp553;}));_tmp552;});_tmp551;});goto _LL30B;_LL312: if(*((int*)_tmp545)!= 3)goto
_LL314;_LL313: if(_tmp3FA == 0)e->r=(void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp556;_tmp556.tag=20;_tmp556.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3F8);_tmp556.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp557=_cycalloc(sizeof(*
_tmp557));_tmp557[0]=({struct Cyc_Absyn_StructField_struct _tmp558;_tmp558.tag=0;
_tmp558.f1=Cyc_Toc_tag_sp;_tmp558;});_tmp557;}));_tmp556;});_tmp555;});else{e->r=(
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp559=_cycalloc(sizeof(*_tmp559));
_tmp559[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp55A;_tmp55A.tag=20;_tmp55A.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp3F8);_tmp55A.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*
_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_Absyn_StructField_struct
_tmp55C;_tmp55C.tag=0;_tmp55C.f1=Cyc_Absyn_fieldname((int)_tmp3FA);_tmp55C;});
_tmp55B;}));_tmp55A;});_tmp559;});}goto _LL30B;_LL314:;_LL315:({void*_tmp55D=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp55E="impossible type for offsetof tuple index";_tag_dyneither(
_tmp55E,sizeof(char),41);}),_tag_dyneither(_tmp55D,sizeof(void*),0));});_LL30B:;}
goto _LL205;_LL238: if(*((int*)_tmp3C5)!= 22)goto _LL23A;_tmp3FB=((struct Cyc_Absyn_Deref_e_struct*)
_tmp3C5)->f1;_LL239: {void*_tmp55F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3FB->topt))->v);{void*_tmp560=_tmp55F;struct Cyc_Absyn_PtrInfo
_tmp561;void*_tmp562;struct Cyc_Absyn_Tqual _tmp563;struct Cyc_Absyn_PtrAtts _tmp564;
void*_tmp565;union Cyc_Absyn_Constraint*_tmp566;union Cyc_Absyn_Constraint*_tmp567;
union Cyc_Absyn_Constraint*_tmp568;_LL317: if(_tmp560 <= (void*)4)goto _LL319;if(*((
int*)_tmp560)!= 4)goto _LL319;_tmp561=((struct Cyc_Absyn_PointerType_struct*)
_tmp560)->f1;_tmp562=_tmp561.elt_typ;_tmp563=_tmp561.elt_tq;_tmp564=_tmp561.ptr_atts;
_tmp565=_tmp564.rgn;_tmp566=_tmp564.nullable;_tmp567=_tmp564.bounds;_tmp568=
_tmp564.zero_term;_LL318:{void*_tmp569=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp567);struct Cyc_Absyn_Exp*_tmp56A;_LL31C: if(_tmp569 <= (void*)1)goto _LL31E;if(*((
int*)_tmp569)!= 0)goto _LL31E;_tmp56A=((struct Cyc_Absyn_Upper_b_struct*)_tmp569)->f1;
_LL31D: {int do_null_check=Cyc_Toc_need_null_check(_tmp3FB);Cyc_Toc_exp_to_c(nv,
_tmp3FB);if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp56B=0;Cyc_Tcutil_warn(
e->loc,({const char*_tmp56C="inserted null check due to dereference";
_tag_dyneither(_tmp56C,sizeof(char),39);}),_tag_dyneither(_tmp56B,sizeof(void*),
0));});_tmp3FB->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp55F),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp56D=_cycalloc(sizeof(*_tmp56D));
_tmp56D->hd=Cyc_Absyn_copy_exp(_tmp3FB);_tmp56D->tl=0;_tmp56D;}),0));}if(!((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp568)){unsigned int
_tmp56F;int _tmp570;struct _tuple11 _tmp56E=Cyc_Evexp_eval_const_uint_exp(_tmp56A);
_tmp56F=_tmp56E.f1;_tmp570=_tmp56E.f2;if(!_tmp570  || _tmp56F <= 0)({void*_tmp571=
0;Cyc_Tcutil_terr(e->loc,({const char*_tmp572="cannot determine dereference is in bounds";
_tag_dyneither(_tmp572,sizeof(char),42);}),_tag_dyneither(_tmp571,sizeof(void*),
0));});}goto _LL31B;}_LL31E: if((int)_tmp569 != 0)goto _LL31B;_LL31F: {struct Cyc_Absyn_Exp*
_tmp573=Cyc_Absyn_uint_exp(0,0);_tmp573->topt=({struct Cyc_Core_Opt*_tmp574=
_cycalloc(sizeof(*_tmp574));_tmp574->v=(void*)Cyc_Absyn_uint_typ;_tmp574;});e->r=
Cyc_Toc_subscript_exp_r(_tmp3FB,_tmp573);Cyc_Toc_exp_to_c(nv,e);goto _LL31B;}
_LL31B:;}goto _LL316;_LL319:;_LL31A:({void*_tmp575=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp576="exp_to_c: Deref: non-pointer";
_tag_dyneither(_tmp576,sizeof(char),29);}),_tag_dyneither(_tmp575,sizeof(void*),
0));});_LL316:;}goto _LL205;}_LL23A: if(*((int*)_tmp3C5)!= 23)goto _LL23C;_tmp3FC=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp3C5)->f1;_tmp3FD=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3C5)->f2;_tmp3FE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp3C5)->f3;_tmp3FF=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp3C5)->f4;_LL23B: {int is_poly=Cyc_Toc_is_poly_project(
e);void*e1_cyc_type=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3FC->topt))->v;
Cyc_Toc_exp_to_c(nv,_tmp3FC);if(_tmp3FE  && _tmp3FF)e->r=Cyc_Toc_check_tagged_union(
_tmp3FC,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp3FD,Cyc_Absyn_aggrmember_exp);
if(is_poly)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;goto _LL205;}_LL23C:
if(*((int*)_tmp3C5)!= 24)goto _LL23E;_tmp400=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3C5)->f1;_tmp401=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3C5)->f2;_tmp402=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3C5)->f3;_tmp403=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3C5)->f4;_LL23D: {void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp400->topt))->v);int do_null_check=Cyc_Toc_need_null_check(_tmp400);
Cyc_Toc_exp_to_c(nv,_tmp400);{int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp578;
struct Cyc_Absyn_Tqual _tmp579;struct Cyc_Absyn_PtrAtts _tmp57A;void*_tmp57B;union
Cyc_Absyn_Constraint*_tmp57C;union Cyc_Absyn_Constraint*_tmp57D;union Cyc_Absyn_Constraint*
_tmp57E;struct Cyc_Absyn_PtrInfo _tmp577=Cyc_Toc_get_ptr_type(e1typ);_tmp578=
_tmp577.elt_typ;_tmp579=_tmp577.elt_tq;_tmp57A=_tmp577.ptr_atts;_tmp57B=_tmp57A.rgn;
_tmp57C=_tmp57A.nullable;_tmp57D=_tmp57A.bounds;_tmp57E=_tmp57A.zero_term;{void*
_tmp57F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp57D);struct Cyc_Absyn_Exp*
_tmp580;_LL321: if(_tmp57F <= (void*)1)goto _LL323;if(*((int*)_tmp57F)!= 0)goto
_LL323;_tmp580=((struct Cyc_Absyn_Upper_b_struct*)_tmp57F)->f1;_LL322: {
unsigned int _tmp582;int _tmp583;struct _tuple11 _tmp581=Cyc_Evexp_eval_const_uint_exp(
_tmp580);_tmp582=_tmp581.f1;_tmp583=_tmp581.f2;if(_tmp583){if(_tmp582 < 1)({void*
_tmp584=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp585="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(
_tmp585,sizeof(char),44);}),_tag_dyneither(_tmp584,sizeof(void*),0));});if(
do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp586=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp587="inserted null check due to dereference";_tag_dyneither(
_tmp587,sizeof(char),39);}),_tag_dyneither(_tmp586,sizeof(void*),0));});_tmp400->r=
Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_Absyn_Exp*_tmp588[1];_tmp588[0]=Cyc_Absyn_new_exp(_tmp400->r,0);((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp588,sizeof(struct Cyc_Absyn_Exp*),1));}),0));}}else{if(!Cyc_Evexp_c_can_eval(
_tmp580))({void*_tmp589=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp58A="cannot determine pointer dereference in bounds";
_tag_dyneither(_tmp58A,sizeof(char),47);}),_tag_dyneither(_tmp589,sizeof(void*),
0));});_tmp400->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp58B[4];_tmp58B[3]=
Cyc_Absyn_uint_exp(0,0);_tmp58B[2]=Cyc_Absyn_sizeoftyp_exp(_tmp578,0);_tmp58B[1]=
_tmp580;_tmp58B[0]=Cyc_Absyn_new_exp(_tmp400->r,0);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp58B,sizeof(struct Cyc_Absyn_Exp*),
4));}),0));}goto _LL320;}_LL323: if((int)_tmp57F != 0)goto _LL320;_LL324: {void*ta1=
Cyc_Toc_typ_to_c_array(_tmp578);_tmp400->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
ta1,_tmp579),Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,({struct
Cyc_Absyn_Exp*_tmp58C[3];_tmp58C[2]=Cyc_Absyn_uint_exp(0,0);_tmp58C[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp58C[0]=Cyc_Absyn_new_exp(_tmp400->r,0);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp58C,sizeof(struct Cyc_Absyn_Exp*),
3));}),0));goto _LL320;}_LL320:;}if(_tmp402  && _tmp403)e->r=Cyc_Toc_check_tagged_union(
_tmp400,Cyc_Toc_typ_to_c(e1typ),_tmp578,_tmp401,Cyc_Absyn_aggrarrow_exp);if(
is_poly  && _tmp403)e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL205;}}_LL23E: if(*((int*)_tmp3C5)!= 25)goto _LL240;_tmp404=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp3C5)->f1;_tmp405=((struct Cyc_Absyn_Subscript_e_struct*)_tmp3C5)->f2;_LL23F: {
void*_tmp58D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp404->topt))->v);{void*_tmp58E=_tmp58D;struct Cyc_List_List*_tmp58F;struct Cyc_Absyn_PtrInfo
_tmp590;void*_tmp591;struct Cyc_Absyn_Tqual _tmp592;struct Cyc_Absyn_PtrAtts _tmp593;
void*_tmp594;union Cyc_Absyn_Constraint*_tmp595;union Cyc_Absyn_Constraint*_tmp596;
union Cyc_Absyn_Constraint*_tmp597;_LL326: if(_tmp58E <= (void*)4)goto _LL32A;if(*((
int*)_tmp58E)!= 9)goto _LL328;_tmp58F=((struct Cyc_Absyn_TupleType_struct*)_tmp58E)->f1;
_LL327: Cyc_Toc_exp_to_c(nv,_tmp404);Cyc_Toc_exp_to_c(nv,_tmp405);{unsigned int
_tmp599;int _tmp59A;struct _tuple11 _tmp598=Cyc_Evexp_eval_const_uint_exp(_tmp405);
_tmp599=_tmp598.f1;_tmp59A=_tmp598.f2;if(!_tmp59A)({void*_tmp59B=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp59C="unknown tuple subscript in translation to C";_tag_dyneither(_tmp59C,
sizeof(char),44);}),_tag_dyneither(_tmp59B,sizeof(void*),0));});e->r=Cyc_Toc_aggrmember_exp_r(
_tmp404,Cyc_Absyn_fieldname((int)(_tmp599 + 1)));goto _LL325;}_LL328: if(*((int*)
_tmp58E)!= 4)goto _LL32A;_tmp590=((struct Cyc_Absyn_PointerType_struct*)_tmp58E)->f1;
_tmp591=_tmp590.elt_typ;_tmp592=_tmp590.elt_tq;_tmp593=_tmp590.ptr_atts;_tmp594=
_tmp593.rgn;_tmp595=_tmp593.nullable;_tmp596=_tmp593.bounds;_tmp597=_tmp593.zero_term;
_LL329: {struct Cyc_List_List*_tmp59D=Cyc_Toc_get_relns(_tmp404);int in_bnds=0;{
void*_tmp59E=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp596);_LL32D:;_LL32E:
in_bnds=Cyc_Toc_check_bounds(_tmp59D,_tmp404,_tmp405);if(Cyc_Toc_warn_bounds_checks
 && !in_bnds)({struct Cyc_String_pa_struct _tmp5A1;_tmp5A1.tag=0;_tmp5A1.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*
_tmp59F[1]={& _tmp5A1};Cyc_Tcutil_warn(e->loc,({const char*_tmp5A0="bounds check necessary for %s";
_tag_dyneither(_tmp5A0,sizeof(char),30);}),_tag_dyneither(_tmp59F,sizeof(void*),
1));}});_LL32C:;}Cyc_Toc_exp_to_c(nv,_tmp404);Cyc_Toc_exp_to_c(nv,_tmp405);++ Cyc_Toc_total_bounds_checks;{
void*_tmp5A2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp596);struct Cyc_Absyn_Exp*
_tmp5A3;_LL330: if(_tmp5A2 <= (void*)1)goto _LL332;if(*((int*)_tmp5A2)!= 0)goto
_LL332;_tmp5A3=((struct Cyc_Absyn_Upper_b_struct*)_tmp5A2)->f1;_LL331: {int
possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp595);void*ta1=Cyc_Toc_typ_to_c(_tmp591);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp592);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;else{if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp597)){if(!Cyc_Evexp_c_can_eval(
_tmp5A3))({void*_tmp5A4=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp5A5="cannot determine subscript is in bounds";
_tag_dyneither(_tmp5A5,sizeof(char),40);}),_tag_dyneither(_tmp5A4,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp404);e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
function_e,({struct Cyc_Absyn_Exp*_tmp5A6[3];_tmp5A6[2]=_tmp405;_tmp5A6[1]=
_tmp5A3;_tmp5A6[0]=_tmp404;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5A6,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp5A3))({void*_tmp5A7=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp5A8="cannot determine subscript is in bounds";
_tag_dyneither(_tmp5A8,sizeof(char),40);}),_tag_dyneither(_tmp5A7,sizeof(void*),
0));});if(Cyc_Toc_warn_all_null_deref)({void*_tmp5A9=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp5AA="inserted null check due to dereference";_tag_dyneither(
_tmp5AA,sizeof(char),39);}),_tag_dyneither(_tmp5A9,sizeof(void*),0));});e->r=Cyc_Toc_deref_exp_r(
Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp5AB[4];_tmp5AB[3]=_tmp405;_tmp5AB[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp5AB[1]=_tmp5A3;_tmp5AB[0]=_tmp404;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5AB,sizeof(struct Cyc_Absyn_Exp*),
4));}),0)));}else{_tmp405->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp5AC[2];_tmp5AC[1]=Cyc_Absyn_copy_exp(_tmp405);_tmp5AC[0]=
_tmp5A3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5AC,sizeof(struct Cyc_Absyn_Exp*),2));}));}}}goto _LL32F;}
_LL332: if((int)_tmp5A2 != 0)goto _LL32F;_LL333: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp591);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;e->r=Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp592),Cyc_Absyn_aggrmember_exp(
_tmp404,Cyc_Toc_curr_sp,0)),_tmp405);}else{struct Cyc_Absyn_Exp*_tmp5AD=Cyc_Toc__check_dyneither_subscript_e;
e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp592),Cyc_Absyn_fncall_exp(
_tmp5AD,({struct Cyc_Absyn_Exp*_tmp5AE[3];_tmp5AE[2]=_tmp405;_tmp5AE[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp5AE[0]=_tmp404;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp5AE,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));}goto _LL32F;}
_LL32F:;}goto _LL325;}_LL32A:;_LL32B:({void*_tmp5AF=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5B0="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dyneither(_tmp5B0,sizeof(char),49);}),_tag_dyneither(_tmp5AF,sizeof(void*),
0));});_LL325:;}goto _LL205;}_LL240: if(*((int*)_tmp3C5)!= 26)goto _LL242;_tmp406=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp3C5)->f1;_LL241: if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp406))->r;else{struct Cyc_List_List*_tmp5B1=((
struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp406);void*_tmp5B2=Cyc_Toc_add_tuple_type(
_tmp5B1);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp406 != 0;(_tmp406=_tmp406->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp406->hd);dles=({struct Cyc_List_List*
_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->hd=({struct _tuple16*_tmp5B4=
_cycalloc(sizeof(*_tmp5B4));_tmp5B4->f1=0;_tmp5B4->f2=(struct Cyc_Absyn_Exp*)
_tmp406->hd;_tmp5B4;});_tmp5B3->tl=dles;_tmp5B3;});}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);e->r=Cyc_Toc_unresolvedmem_exp_r(
0,dles);}goto _LL205;_LL242: if(*((int*)_tmp3C5)!= 28)goto _LL244;_tmp407=((struct
Cyc_Absyn_Array_e_struct*)_tmp3C5)->f1;_LL243: e->r=Cyc_Toc_unresolvedmem_exp_r(0,
_tmp407);{struct Cyc_List_List*_tmp5B5=_tmp407;for(0;_tmp5B5 != 0;_tmp5B5=_tmp5B5->tl){
struct _tuple16 _tmp5B7;struct Cyc_Absyn_Exp*_tmp5B8;struct _tuple16*_tmp5B6=(struct
_tuple16*)_tmp5B5->hd;_tmp5B7=*_tmp5B6;_tmp5B8=_tmp5B7.f2;Cyc_Toc_exp_to_c(nv,
_tmp5B8);}}goto _LL205;_LL244: if(*((int*)_tmp3C5)!= 29)goto _LL246;_tmp408=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp3C5)->f1;_tmp409=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3C5)->f2;_tmp40A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3C5)->f3;
_tmp40B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3C5)->f4;_LL245: {
unsigned int _tmp5BA;int _tmp5BB;struct _tuple11 _tmp5B9=Cyc_Evexp_eval_const_uint_exp(
_tmp409);_tmp5BA=_tmp5B9.f1;_tmp5BB=_tmp5B9.f2;{void*_tmp5BC=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp40A->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp40A);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp40A)){if(!_tmp5BB)({
void*_tmp5BD=0;Cyc_Tcutil_terr(_tmp409->loc,({const char*_tmp5BE="cannot determine value of constant";
_tag_dyneither(_tmp5BE,sizeof(char),35);}),_tag_dyneither(_tmp5BD,sizeof(void*),
0));});{unsigned int i=0;for(0;i < _tmp5BA;++ i){es=({struct Cyc_List_List*_tmp5BF=
_cycalloc(sizeof(*_tmp5BF));_tmp5BF->hd=({struct _tuple16*_tmp5C0=_cycalloc(
sizeof(*_tmp5C0));_tmp5C0->f1=0;_tmp5C0->f2=_tmp40A;_tmp5C0;});_tmp5BF->tl=es;
_tmp5BF;});}}if(_tmp40B){struct Cyc_Absyn_Exp*_tmp5C1=Cyc_Toc_cast_it(_tmp5BC,Cyc_Absyn_uint_exp(
0,0));es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));
_tmp5C2->hd=({struct _tuple16*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->f1=0;
_tmp5C3->f2=_tmp5C1;_tmp5C3;});_tmp5C2->tl=0;_tmp5C2;}));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,es);goto _LL205;}}}_LL246: if(*((int*)_tmp3C5)!= 30)goto _LL248;_tmp40C=((struct
Cyc_Absyn_Aggregate_e_struct*)_tmp3C5)->f1;_tmp40D=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmp3C5)->f2;_tmp40E=((struct Cyc_Absyn_Aggregate_e_struct*)_tmp3C5)->f3;_tmp40F=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmp3C5)->f4;_LL247: if(!Cyc_Toc_is_toplevel(
nv))e->r=(Cyc_Toc_init_struct(nv,old_typ,_tmp40D != 0,0,0,_tmp40E,_tmp40C))->r;
else{if(_tmp40F == 0)({void*_tmp5C4=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5C5="Aggregate_e: missing aggrdecl pointer";
_tag_dyneither(_tmp5C5,sizeof(char),38);}),_tag_dyneither(_tmp5C4,sizeof(void*),
0));});{struct Cyc_Absyn_Aggrdecl*sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp40F;struct
Cyc_Toc_Env _tmp5C7;struct _RegionHandle*_tmp5C8;struct Cyc_Toc_Env*_tmp5C6=nv;
_tmp5C7=*_tmp5C6;_tmp5C8=_tmp5C7.rgn;{struct Cyc_List_List*_tmp5C9=((struct Cyc_List_List*(*)(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
void*,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp5C8,e->loc,_tmp40E,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp40F->impl))->tagged){struct
_tuple20 _tmp5CB;struct Cyc_Absyn_Aggrfield*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CD;
struct _tuple20*_tmp5CA=(struct _tuple20*)((struct Cyc_List_List*)_check_null(
_tmp5C9))->hd;_tmp5CB=*_tmp5CA;_tmp5CC=_tmp5CB.f1;_tmp5CD=_tmp5CB.f2;{void*
_tmp5CE=_tmp5CC->type;Cyc_Toc_exp_to_c(nv,_tmp5CD);if(Cyc_Toc_is_void_star_or_tvar(
_tmp5CE))_tmp5CD->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(
_tmp5CD->r,0));{int i=Cyc_Toc_get_member_offset((struct Cyc_Absyn_Aggrdecl*)
_tmp40F,_tmp5CC->name);struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(
i,0);struct Cyc_List_List*_tmp5CF=({struct _tuple16*_tmp5D5[2];_tmp5D5[1]=({struct
_tuple16*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->f1=0;_tmp5D7->f2=_tmp5CD;
_tmp5D7;});_tmp5D5[0]=({struct _tuple16*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));
_tmp5D6->f1=0;_tmp5D6->f2=field_tag_exp;_tmp5D6;});((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D5,sizeof(struct
_tuple16*),2));});struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp5CF,
0);struct Cyc_List_List*ds=({void*_tmp5D2[1];_tmp5D2[0]=(void*)({struct Cyc_Absyn_FieldName_struct*
_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3[0]=({struct Cyc_Absyn_FieldName_struct
_tmp5D4;_tmp5D4.tag=1;_tmp5D4.f1=_tmp5CC->name;_tmp5D4;});_tmp5D3;});Cyc_List_list(
_tag_dyneither(_tmp5D2,sizeof(void*),1));});struct Cyc_List_List*dles=({struct
_tuple16*_tmp5D0[1];_tmp5D0[0]=({struct _tuple16*_tmp5D1=_cycalloc(sizeof(*
_tmp5D1));_tmp5D1->f1=ds;_tmp5D1->f2=umem;_tmp5D1;});((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5D0,sizeof(struct
_tuple16*),1));});e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}}}else{struct Cyc_List_List*
_tmp5D8=0;struct Cyc_List_List*_tmp5D9=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp5D9 != 0;_tmp5D9=_tmp5D9->tl){struct Cyc_List_List*
_tmp5DA=_tmp5C9;for(0;_tmp5DA != 0;_tmp5DA=_tmp5DA->tl){if((*((struct _tuple20*)
_tmp5DA->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp5D9->hd){struct _tuple20
_tmp5DC;struct Cyc_Absyn_Aggrfield*_tmp5DD;struct Cyc_Absyn_Exp*_tmp5DE;struct
_tuple20*_tmp5DB=(struct _tuple20*)_tmp5DA->hd;_tmp5DC=*_tmp5DB;_tmp5DD=_tmp5DC.f1;
_tmp5DE=_tmp5DC.f2;{void*_tmp5DF=_tmp5DD->type;Cyc_Toc_exp_to_c(nv,_tmp5DE);if(
Cyc_Toc_is_void_star_or_tvar(_tmp5DF))_tmp5DE->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp(_tmp5DE->r,0));_tmp5D8=({struct Cyc_List_List*_tmp5E0=_cycalloc(
sizeof(*_tmp5E0));_tmp5E0->hd=({struct _tuple16*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));
_tmp5E1->f1=0;_tmp5E1->f2=_tmp5DE;_tmp5E1;});_tmp5E0->tl=_tmp5D8;_tmp5E0;});
break;}}}}e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp5D8));}}}}goto _LL205;_LL248: if(*((int*)_tmp3C5)!= 31)
goto _LL24A;_tmp410=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3C5)->f1;
_tmp411=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3C5)->f2;_LL249: {struct Cyc_List_List*
fs;{void*_tmp5E2=Cyc_Tcutil_compress(_tmp410);struct Cyc_List_List*_tmp5E3;_LL335:
if(_tmp5E2 <= (void*)4)goto _LL337;if(*((int*)_tmp5E2)!= 11)goto _LL337;_tmp5E3=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp5E2)->f2;_LL336: fs=_tmp5E3;goto _LL334;
_LL337:;_LL338:({struct Cyc_String_pa_struct _tmp5E6;_tmp5E6.tag=0;_tmp5E6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp410));{
void*_tmp5E4[1]={& _tmp5E6};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(({const char*_tmp5E5="anon struct has type %s";
_tag_dyneither(_tmp5E5,sizeof(char),24);}),_tag_dyneither(_tmp5E4,sizeof(void*),
1));}});_LL334:;}{struct Cyc_Toc_Env _tmp5E8;struct _RegionHandle*_tmp5E9;struct Cyc_Toc_Env*
_tmp5E7=nv;_tmp5E8=*_tmp5E7;_tmp5E9=_tmp5E8.rgn;{struct Cyc_List_List*_tmp5EA=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,void*,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(
_tmp5E9,e->loc,_tmp411,(void*)0,fs);for(0;_tmp5EA != 0;_tmp5EA=_tmp5EA->tl){
struct _tuple20 _tmp5EC;struct Cyc_Absyn_Aggrfield*_tmp5ED;struct Cyc_Absyn_Exp*
_tmp5EE;struct _tuple20*_tmp5EB=(struct _tuple20*)_tmp5EA->hd;_tmp5EC=*_tmp5EB;
_tmp5ED=_tmp5EC.f1;_tmp5EE=_tmp5EC.f2;{void*_tmp5EF=_tmp5ED->type;Cyc_Toc_exp_to_c(
nv,_tmp5EE);if(Cyc_Toc_is_void_star_or_tvar(_tmp5EF))_tmp5EE->r=Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp5EE->r,0));}}e->r=Cyc_Toc_unresolvedmem_exp_r(
0,_tmp411);}goto _LL205;}}_LL24A: if(*((int*)_tmp3C5)!= 32)goto _LL24C;_tmp412=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp3C5)->f1;if(_tmp412 != 0)goto _LL24C;
_tmp413=((struct Cyc_Absyn_Datatype_e_struct*)_tmp3C5)->f2;_tmp414=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp3C5)->f3;_LL24B: {struct _tuple1*qv=_tmp414->name;struct Cyc_Absyn_Exp*tag_exp=
_tmp413->is_extensible?Cyc_Absyn_var_exp(qv,0): Cyc_Toc_datatype_tag(_tmp413,qv,0);
e->r=tag_exp->r;goto _LL205;}_LL24C: if(*((int*)_tmp3C5)!= 32)goto _LL24E;_tmp415=((
struct Cyc_Absyn_Datatype_e_struct*)_tmp3C5)->f1;_tmp416=((struct Cyc_Absyn_Datatype_e_struct*)
_tmp3C5)->f2;_tmp417=((struct Cyc_Absyn_Datatype_e_struct*)_tmp3C5)->f3;_LL24D: {
void*datatype_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp5F0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp5F1=Cyc_Absyn_var_exp(_tmp5F0,0);struct Cyc_Absyn_Exp*
member_exp;datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp417->name,({
const char*_tmp5F2="_struct";_tag_dyneither(_tmp5F2,sizeof(char),8);})));tag_exp=
_tmp416->is_extensible?Cyc_Absyn_var_exp(_tmp417->name,0): Cyc_Toc_datatype_tag(
_tmp416,_tmp417->name,1);member_exp=_tmp5F1;{struct Cyc_List_List*_tmp5F3=_tmp417->typs;
if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;_tmp415 != 0;(
_tmp415=_tmp415->tl,_tmp5F3=_tmp5F3->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp415->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp5F3))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);dles=({struct Cyc_List_List*
_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->hd=({struct _tuple16*_tmp5F5=
_cycalloc(sizeof(*_tmp5F5));_tmp5F5->f1=0;_tmp5F5->f2=cur_e;_tmp5F5;});_tmp5F4->tl=
dles;_tmp5F4;});}dles=({struct Cyc_List_List*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));
_tmp5F6->hd=({struct _tuple16*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->f1=0;
_tmp5F7->f2=tag_exp;_tmp5F7;});_tmp5F6->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(dles);_tmp5F6;});e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
else{struct Cyc_List_List*_tmp5F8=({struct Cyc_List_List*_tmp5FE=_cycalloc(sizeof(*
_tmp5FE));_tmp5FE->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,
Cyc_Toc_tag_sp,0),tag_exp,0);_tmp5FE->tl=0;_tmp5FE;});{int i=1;for(0;_tmp415 != 0;(((
_tmp415=_tmp415->tl,i ++)),_tmp5F3=_tmp5F3->tl)){struct Cyc_Absyn_Exp*cur_e=(
struct Cyc_Absyn_Exp*)_tmp415->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct
_tuple10*)((struct Cyc_List_List*)_check_null(_tmp5F3))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star_or_tvar(field_typ))cur_e=Cyc_Toc_cast_it(
field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp5F9=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);_tmp5F8=({struct Cyc_List_List*
_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->hd=_tmp5F9;_tmp5FA->tl=_tmp5F8;
_tmp5FA;});}}}{struct Cyc_Absyn_Stmt*_tmp5FB=Cyc_Absyn_exp_stmt(_tmp5F1,0);struct
Cyc_Absyn_Stmt*_tmp5FC=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));
_tmp5FD->hd=_tmp5FB;_tmp5FD->tl=_tmp5F8;_tmp5FD;})),0);e->r=Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp5F0,datatype_ctype,0,_tmp5FC,0));}}goto _LL205;}}
_LL24E: if(*((int*)_tmp3C5)!= 33)goto _LL250;_LL24F: goto _LL251;_LL250: if(*((int*)
_tmp3C5)!= 34)goto _LL252;_LL251: goto _LL205;_LL252: if(*((int*)_tmp3C5)!= 35)goto
_LL254;_tmp418=((struct Cyc_Absyn_Malloc_e_struct*)_tmp3C5)->f1;_tmp419=_tmp418.is_calloc;
_tmp41A=_tmp418.rgn;_tmp41B=_tmp418.elt_type;_tmp41C=_tmp418.num_elts;_tmp41D=
_tmp418.fat_result;_LL253: {void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(
_tmp41B)));Cyc_Toc_exp_to_c(nv,_tmp41C);if(_tmp41D){struct _tuple1*_tmp5FF=Cyc_Toc_temp_var();
struct _tuple1*_tmp600=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*rexp;if(_tmp419){xexp=_tmp41C;if(_tmp41A != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp41A;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp5FF,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp41B,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp5FF,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp601[3];_tmp601[2]=Cyc_Absyn_var_exp(_tmp5FF,0);_tmp601[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp601[0]=Cyc_Absyn_var_exp(_tmp600,0);((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp601,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp41C,0);if(_tmp41A != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp41A;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(
rgn,Cyc_Absyn_var_exp(_tmp5FF,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp41B,Cyc_Absyn_var_exp(
_tmp5FF,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp602[3];_tmp602[2]=Cyc_Absyn_var_exp(_tmp5FF,0);_tmp602[1]=Cyc_Absyn_uint_exp(
1,0);_tmp602[0]=Cyc_Absyn_var_exp(_tmp600,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp602,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp603=Cyc_Absyn_declare_stmt(_tmp5FF,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp600,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);e->r=
Cyc_Toc_stmt_exp_r(_tmp603);}}else{struct Cyc_Absyn_Exp*_tmp604=Cyc_Absyn_sizeoftyp_exp(
t_c,0);{void*_tmp605=_tmp41C->r;union Cyc_Absyn_Cnst _tmp606;struct _tuple6 _tmp607;
int _tmp608;_LL33A: if(*((int*)_tmp605)!= 0)goto _LL33C;_tmp606=((struct Cyc_Absyn_Const_e_struct*)
_tmp605)->f1;if((_tmp606.Int_c).tag != 4)goto _LL33C;_tmp607=(struct _tuple6)(
_tmp606.Int_c).val;_tmp608=_tmp607.f2;if(_tmp608 != 1)goto _LL33C;_LL33B: goto
_LL339;_LL33C:;_LL33D: _tmp604=Cyc_Absyn_times_exp(_tmp604,_tmp41C,0);goto _LL339;
_LL339:;}if(_tmp41A != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(
struct Cyc_Absyn_Exp*)_tmp41A;Cyc_Toc_exp_to_c(nv,rgn);e->r=(Cyc_Toc_rmalloc_exp(
rgn,_tmp604))->r;}else{e->r=(Cyc_Toc_malloc_exp(*_tmp41B,_tmp604))->r;}}goto
_LL205;}_LL254: if(*((int*)_tmp3C5)!= 36)goto _LL256;_tmp41E=((struct Cyc_Absyn_Swap_e_struct*)
_tmp3C5)->f1;_tmp41F=((struct Cyc_Absyn_Swap_e_struct*)_tmp3C5)->f2;_LL255: {int
is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp41E->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp41F->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dyneither_ptr))({void*_tmp609=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp60A="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";
_tag_dyneither(_tmp60A,sizeof(char),57);}),_tag_dyneither(_tmp609,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}if(!Cyc_Absyn_is_lvalue(_tmp41E))({struct Cyc_String_pa_struct
_tmp60D;_tmp60D.tag=0;_tmp60D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp41E));{void*_tmp60B[1]={& _tmp60D};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp60C="Swap_e: %s is not an l-value\n";
_tag_dyneither(_tmp60C,sizeof(char),30);}),_tag_dyneither(_tmp60B,sizeof(void*),
1));}});if(!Cyc_Absyn_is_lvalue(_tmp41F))({struct Cyc_String_pa_struct _tmp610;
_tmp610.tag=0;_tmp610.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp41F));{void*_tmp60E[1]={& _tmp610};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp60F="Swap_e: %s is not an l-value\n";
_tag_dyneither(_tmp60F,sizeof(char),30);}),_tag_dyneither(_tmp60E,sizeof(void*),
1));}});Cyc_Toc_exp_to_c(nv,_tmp41E);Cyc_Toc_exp_to_c(nv,_tmp41F);e->r=Cyc_Toc_fncall_exp_r(
swap_fn,({struct Cyc_Absyn_Exp*_tmp611[2];_tmp611[1]=Cyc_Absyn_address_exp(
_tmp41F,0);_tmp611[0]=Cyc_Absyn_address_exp(_tmp41E,0);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp611,sizeof(struct Cyc_Absyn_Exp*),
2));}));goto _LL205;}}_LL256: if(*((int*)_tmp3C5)!= 39)goto _LL258;_tmp420=((struct
Cyc_Absyn_Tagcheck_e_struct*)_tmp3C5)->f1;_tmp421=((struct Cyc_Absyn_Tagcheck_e_struct*)
_tmp3C5)->f2;_LL257: {void*_tmp612=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp420->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp420);{void*_tmp613=
_tmp612;struct Cyc_Absyn_AggrInfo _tmp614;union Cyc_Absyn_AggrInfoU _tmp615;struct
Cyc_Absyn_Aggrdecl**_tmp616;struct Cyc_Absyn_Aggrdecl*_tmp617;_LL33F: if(_tmp613 <= (
void*)4)goto _LL341;if(*((int*)_tmp613)!= 10)goto _LL341;_tmp614=((struct Cyc_Absyn_AggrType_struct*)
_tmp613)->f1;_tmp615=_tmp614.aggr_info;if((_tmp615.KnownAggr).tag != 2)goto _LL341;
_tmp616=(struct Cyc_Absyn_Aggrdecl**)(_tmp615.KnownAggr).val;_tmp617=*_tmp616;
_LL340: {struct Cyc_Absyn_Exp*_tmp618=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(
_tmp617,_tmp421),0);struct Cyc_Absyn_Exp*_tmp619=Cyc_Absyn_aggrmember_exp(_tmp420,
_tmp421,0);struct Cyc_Absyn_Exp*_tmp61A=Cyc_Absyn_aggrmember_exp(_tmp619,Cyc_Toc_tag_sp,
0);e->r=(Cyc_Absyn_eq_exp(_tmp61A,_tmp618,0))->r;goto _LL33E;}_LL341:;_LL342:({
void*_tmp61B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp61C="non-aggregate type in tagcheck";_tag_dyneither(_tmp61C,
sizeof(char),31);}),_tag_dyneither(_tmp61B,sizeof(void*),0));});_LL33E:;}goto
_LL205;}_LL258: if(*((int*)_tmp3C5)!= 38)goto _LL25A;_tmp422=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp3C5)->f1;_LL259: Cyc_Toc_stmt_to_c(nv,_tmp422);goto _LL205;_LL25A: if(*((int*)
_tmp3C5)!= 37)goto _LL25C;_LL25B:({void*_tmp61D=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp61E="UnresolvedMem";
_tag_dyneither(_tmp61E,sizeof(char),14);}),_tag_dyneither(_tmp61D,sizeof(void*),
0));});_LL25C: if(*((int*)_tmp3C5)!= 27)goto _LL25E;_LL25D:({void*_tmp61F=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp620="compoundlit";_tag_dyneither(_tmp620,sizeof(char),12);}),_tag_dyneither(
_tmp61F,sizeof(void*),0));});_LL25E: if(*((int*)_tmp3C5)!= 40)goto _LL205;_LL25F:({
void*_tmp621=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp622="valueof(-)";_tag_dyneither(_tmp622,sizeof(char),11);}),
_tag_dyneither(_tmp621,sizeof(void*),0));});_LL205:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple21{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple22{struct _tuple1*f1;void*f2;};struct
_tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple21 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{
void*_tmp623=p->r;struct Cyc_Absyn_Vardecl*_tmp624;struct Cyc_Absyn_Vardecl _tmp625;
struct _tuple1*_tmp626;struct Cyc_Absyn_Pat*_tmp627;struct Cyc_Absyn_Vardecl*
_tmp628;struct Cyc_Absyn_Vardecl _tmp629;struct _tuple1*_tmp62A;struct Cyc_Absyn_Vardecl*
_tmp62B;struct Cyc_Absyn_Pat*_tmp62C;void*_tmp62D;int _tmp62E;char _tmp62F;struct
_dyneither_ptr _tmp630;struct Cyc_Absyn_Enumdecl*_tmp631;struct Cyc_Absyn_Enumfield*
_tmp632;void*_tmp633;struct Cyc_Absyn_Enumfield*_tmp634;struct Cyc_Absyn_Datatypedecl*
_tmp635;struct Cyc_Absyn_Datatypefield*_tmp636;struct Cyc_List_List*_tmp637;struct
Cyc_Absyn_Pat*_tmp638;struct Cyc_Absyn_Pat _tmp639;void*_tmp63A;struct Cyc_Absyn_Datatypedecl*
_tmp63B;struct Cyc_Absyn_Datatypefield*_tmp63C;struct Cyc_List_List*_tmp63D;struct
Cyc_List_List*_tmp63E;struct Cyc_List_List*_tmp63F;struct Cyc_Absyn_AggrInfo*
_tmp640;struct Cyc_Absyn_AggrInfo*_tmp641;struct Cyc_Absyn_AggrInfo _tmp642;union
Cyc_Absyn_AggrInfoU _tmp643;struct Cyc_List_List*_tmp644;struct Cyc_Absyn_Pat*
_tmp645;_LL344: if(_tmp623 <= (void*)2)goto _LL348;if(*((int*)_tmp623)!= 0)goto
_LL346;_tmp624=((struct Cyc_Absyn_Var_p_struct*)_tmp623)->f1;_tmp625=*_tmp624;
_tmp626=_tmp625.name;_tmp627=((struct Cyc_Absyn_Var_p_struct*)_tmp623)->f2;_LL345:
return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp626,r),rgn,t,r,path,
_tmp627,tag_fail_stmt,fail_stmt,decls);_LL346: if(*((int*)_tmp623)!= 2)goto _LL348;
_tmp628=((struct Cyc_Absyn_TagInt_p_struct*)_tmp623)->f2;_tmp629=*_tmp628;_tmp62A=
_tmp629.name;_LL347: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp62A,r);goto _LL349;_LL348:
if((int)_tmp623 != 0)goto _LL34A;_LL349: s=Cyc_Toc_skip_stmt_dl();goto _LL343;_LL34A:
if(_tmp623 <= (void*)2)goto _LL34C;if(*((int*)_tmp623)!= 1)goto _LL34C;_tmp62B=((
struct Cyc_Absyn_Reference_p_struct*)_tmp623)->f1;_tmp62C=((struct Cyc_Absyn_Reference_p_struct*)
_tmp623)->f2;_LL34B: {struct _tuple1*_tmp646=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp647=_region_malloc(rgn,sizeof(*_tmp647));_tmp647->hd=({struct _tuple22*
_tmp648=_region_malloc(rgn,sizeof(*_tmp648));_tmp648->f1=_tmp646;_tmp648->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp648;});_tmp647->tl=decls;_tmp647;});nv=
Cyc_Toc_add_varmap(rgn,nv,_tmp62B->name,Cyc_Absyn_var_exp(_tmp646,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp646,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple21 _tmp649=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp62C,tag_fail_stmt,fail_stmt,decls);_tmp649.f3=Cyc_Absyn_seq_stmt(
s,_tmp649.f3,0);return _tmp649;}}_LL34C: if((int)_tmp623 != 1)goto _LL34E;_LL34D: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL343;_LL34E:
if(_tmp623 <= (void*)2)goto _LL350;if(*((int*)_tmp623)!= 7)goto _LL350;_tmp62D=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp623)->f1;_tmp62E=((struct Cyc_Absyn_Int_p_struct*)
_tmp623)->f2;_LL34F: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp62D,_tmp62E,0),
fail_stmt);goto _LL343;_LL350: if(_tmp623 <= (void*)2)goto _LL352;if(*((int*)_tmp623)
!= 8)goto _LL352;_tmp62F=((struct Cyc_Absyn_Char_p_struct*)_tmp623)->f1;_LL351: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp62F,0),fail_stmt);goto _LL343;_LL352:
if(_tmp623 <= (void*)2)goto _LL354;if(*((int*)_tmp623)!= 9)goto _LL354;_tmp630=((
struct Cyc_Absyn_Float_p_struct*)_tmp623)->f1;_LL353: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp630,0),fail_stmt);goto _LL343;_LL354: if(_tmp623 <= (void*)2)goto _LL356;if(*((
int*)_tmp623)!= 10)goto _LL356;_tmp631=((struct Cyc_Absyn_Enum_p_struct*)_tmp623)->f1;
_tmp632=((struct Cyc_Absyn_Enum_p_struct*)_tmp623)->f2;_LL355: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp64A=_cycalloc(
sizeof(*_tmp64A));_tmp64A[0]=({struct Cyc_Absyn_Enum_e_struct _tmp64B;_tmp64B.tag=
33;_tmp64B.f1=_tmp632->name;_tmp64B.f2=(struct Cyc_Absyn_Enumdecl*)_tmp631;
_tmp64B.f3=(struct Cyc_Absyn_Enumfield*)_tmp632;_tmp64B;});_tmp64A;}),0),
fail_stmt);goto _LL343;_LL356: if(_tmp623 <= (void*)2)goto _LL358;if(*((int*)_tmp623)
!= 11)goto _LL358;_tmp633=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp623)->f1;
_tmp634=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp623)->f2;_LL357: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp64C=_cycalloc(
sizeof(*_tmp64C));_tmp64C[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp64D;_tmp64D.tag=
34;_tmp64D.f1=_tmp634->name;_tmp64D.f2=(void*)_tmp633;_tmp64D.f3=(struct Cyc_Absyn_Enumfield*)
_tmp634;_tmp64D;});_tmp64C;}),0),fail_stmt);goto _LL343;_LL358: if(_tmp623 <= (void*)
2)goto _LL35A;if(*((int*)_tmp623)!= 6)goto _LL35A;_tmp635=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp623)->f1;_tmp636=((struct Cyc_Absyn_Datatype_p_struct*)_tmp623)->f2;_tmp637=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp623)->f3;if(_tmp637 != 0)goto _LL35A;_LL359: {
struct Cyc_Absyn_Exp*cmp_exp;if(_tmp635->is_extensible)cmp_exp=Cyc_Absyn_var_exp(
_tmp636->name,0);else{cmp_exp=Cyc_Toc_datatype_tag(_tmp635,_tmp636->name,0);r=
Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(r,cmp_exp,fail_stmt);
goto _LL343;}_LL35A: if(_tmp623 <= (void*)2)goto _LL35C;if(*((int*)_tmp623)!= 4)goto
_LL35C;_tmp638=((struct Cyc_Absyn_Pointer_p_struct*)_tmp623)->f1;_tmp639=*_tmp638;
_tmp63A=_tmp639.r;if(_tmp63A <= (void*)2)goto _LL35C;if(*((int*)_tmp63A)!= 6)goto
_LL35C;_tmp63B=((struct Cyc_Absyn_Datatype_p_struct*)_tmp63A)->f1;_tmp63C=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp63A)->f2;_tmp63D=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp63A)->f3;if(!(_tmp63D != 0))goto _LL35C;_LL35B: s=Cyc_Toc_skip_stmt_dl();{int
cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp63C->name,({const char*
_tmp65A="_struct";_tag_dyneither(_tmp65A,sizeof(char),8);}));struct Cyc_Absyn_Exp*
rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),
r);struct Cyc_List_List*_tmp64E=_tmp63C->typs;for(0;_tmp63D != 0;(((_tmp63D=
_tmp63D->tl,_tmp64E=((struct Cyc_List_List*)_check_null(_tmp64E))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp64F=(struct Cyc_Absyn_Pat*)_tmp63D->hd;if(_tmp64F->r == (
void*)0)continue;{void*_tmp650=(*((struct _tuple10*)((struct Cyc_List_List*)
_check_null(_tmp64E))->hd)).f2;struct _tuple1*_tmp651=Cyc_Toc_temp_var();void*
_tmp652=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp64F->topt))->v;void*_tmp653=
Cyc_Toc_typ_to_c(_tmp652);struct Cyc_Absyn_Exp*_tmp654=Cyc_Absyn_aggrarrow_exp(
rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star_or_tvar(Cyc_Toc_typ_to_c(
_tmp650)))_tmp654=Cyc_Toc_cast_it(_tmp653,_tmp654);decls=({struct Cyc_List_List*
_tmp655=_region_malloc(rgn,sizeof(*_tmp655));_tmp655->hd=({struct _tuple22*
_tmp656=_region_malloc(rgn,sizeof(*_tmp656));_tmp656->f1=_tmp651;_tmp656->f2=
_tmp653;_tmp656;});_tmp655->tl=decls;_tmp655;});{struct _tuple21 _tmp657=Cyc_Toc_xlate_pat(
nv,rgn,_tmp652,Cyc_Absyn_var_exp(_tmp651,0),_tmp654,_tmp64F,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp657.f1;decls=_tmp657.f2;{struct Cyc_Absyn_Stmt*
_tmp658=_tmp657.f3;struct Cyc_Absyn_Stmt*_tmp659=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp651,0),_tmp654,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp659,_tmp658,
0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(_tmp63B->is_extensible){struct Cyc_Absyn_Exp*
e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp63C->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);}else{struct Cyc_Absyn_Exp*e3=
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),r);struct
Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*e=Cyc_Toc_datatype_tag(
_tmp63B,_tmp63C->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(
e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(tag_fail_stmt != 0){int
max_tag=Cyc_Toc_num_void_tags(_tmp63B);if(max_tag != 0){struct Cyc_Absyn_Exp*
max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);struct Cyc_Absyn_Exp*e5=
Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),max_tag_exp),0);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}}}goto _LL343;}}_LL35C: if(_tmp623 <= (void*)2)goto _LL35E;if(*((int*)
_tmp623)!= 6)goto _LL35E;_tmp63E=((struct Cyc_Absyn_Datatype_p_struct*)_tmp623)->f3;
_LL35D: _tmp63F=_tmp63E;goto _LL35F;_LL35E: if(_tmp623 <= (void*)2)goto _LL360;if(*((
int*)_tmp623)!= 3)goto _LL360;_tmp63F=((struct Cyc_Absyn_Tuple_p_struct*)_tmp623)->f1;
_LL35F: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp63F != 0;(_tmp63F=_tmp63F->tl,
++ cnt)){struct Cyc_Absyn_Pat*_tmp65B=(struct Cyc_Absyn_Pat*)_tmp63F->hd;if(_tmp65B->r
== (void*)0)continue;{struct _tuple1*_tmp65C=Cyc_Toc_temp_var();void*_tmp65D=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp65B->topt))->v;decls=({struct Cyc_List_List*
_tmp65E=_region_malloc(rgn,sizeof(*_tmp65E));_tmp65E->hd=({struct _tuple22*
_tmp65F=_region_malloc(rgn,sizeof(*_tmp65F));_tmp65F->f1=_tmp65C;_tmp65F->f2=Cyc_Toc_typ_to_c(
_tmp65D);_tmp65F;});_tmp65E->tl=decls;_tmp65E;});{struct _tuple21 _tmp660=Cyc_Toc_xlate_pat(
nv,rgn,_tmp65D,Cyc_Absyn_var_exp(_tmp65C,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp65B,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp660.f1;
decls=_tmp660.f2;{struct Cyc_Absyn_Stmt*_tmp661=_tmp660.f3;struct Cyc_Absyn_Stmt*
_tmp662=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp65C,0),Cyc_Absyn_aggrmember_exp(
r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp662,_tmp661,0),0);}}}}goto _LL343;}_LL360: if(_tmp623 <= (void*)2)goto _LL362;
if(*((int*)_tmp623)!= 5)goto _LL362;_tmp640=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp623)->f1;if(_tmp640 != 0)goto _LL362;_LL361:({void*_tmp663=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp664="unresolved aggregate pattern!";
_tag_dyneither(_tmp664,sizeof(char),30);}),_tag_dyneither(_tmp663,sizeof(void*),
0));});_LL362: if(_tmp623 <= (void*)2)goto _LL364;if(*((int*)_tmp623)!= 5)goto
_LL364;_tmp641=((struct Cyc_Absyn_Aggr_p_struct*)_tmp623)->f1;if(_tmp641 == 0)goto
_LL364;_tmp642=*_tmp641;_tmp643=_tmp642.aggr_info;_tmp644=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp623)->f3;_LL363: {struct Cyc_Absyn_Aggrdecl*_tmp665=Cyc_Absyn_get_known_aggrdecl(
_tmp643);if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp665->impl))->tagged){
struct _tuple23 _tmp667;struct Cyc_List_List*_tmp668;struct Cyc_Absyn_Pat*_tmp669;
struct _tuple23*_tmp666=(struct _tuple23*)((struct Cyc_List_List*)_check_null(
_tmp644))->hd;_tmp667=*_tmp666;_tmp668=_tmp667.f1;_tmp669=_tmp667.f2;{struct
_dyneither_ptr*f;{void*_tmp66A=(void*)((struct Cyc_List_List*)_check_null(_tmp668))->hd;
struct _dyneither_ptr*_tmp66B;_LL36D: if(*((int*)_tmp66A)!= 1)goto _LL36F;_tmp66B=((
struct Cyc_Absyn_FieldName_struct*)_tmp66A)->f1;_LL36E: f=_tmp66B;goto _LL36C;
_LL36F:;_LL370:({void*_tmp66C=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp66D="no field name in tagged union pattern";
_tag_dyneither(_tmp66D,sizeof(char),38);}),_tag_dyneither(_tmp66C,sizeof(void*),
0));});_LL36C:;}{struct _tuple1*_tmp66E=Cyc_Toc_temp_var();void*_tmp66F=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp669->topt))->v;void*_tmp670=Cyc_Toc_typ_to_c(
_tmp66F);decls=({struct Cyc_List_List*_tmp671=_region_malloc(rgn,sizeof(*_tmp671));
_tmp671->hd=({struct _tuple22*_tmp672=_region_malloc(rgn,sizeof(*_tmp672));
_tmp672->f1=_tmp66E;_tmp672->f2=_tmp670;_tmp672;});_tmp671->tl=decls;_tmp671;});{
struct Cyc_Absyn_Exp*_tmp673=Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(
path,f,0),Cyc_Toc_val_sp,0);struct Cyc_Absyn_Exp*_tmp674=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_val_sp,0);_tmp674=Cyc_Toc_cast_it(
_tmp670,_tmp674);_tmp673=Cyc_Toc_cast_it(_tmp670,_tmp673);{struct _tuple21 _tmp675=
Cyc_Toc_xlate_pat(nv,rgn,_tmp66F,Cyc_Absyn_var_exp(_tmp66E,0),_tmp673,_tmp669,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp675.f1;decls=_tmp675.f2;{
struct Cyc_Absyn_Stmt*_tmp676=_tmp675.f3;struct Cyc_Absyn_Stmt*_tmp677=Cyc_Toc_if_neq_stmt(
Cyc_Absyn_aggrmember_exp(Cyc_Absyn_aggrmember_exp(r,f,0),Cyc_Toc_tag_sp,0),Cyc_Absyn_signed_int_exp(
Cyc_Toc_get_member_offset(_tmp665,f),0),fail_stmt);struct Cyc_Absyn_Stmt*_tmp678=
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp66E,0),_tmp674,0);s=Cyc_Absyn_seq_stmt(
_tmp677,Cyc_Absyn_seq_stmt(_tmp678,_tmp676,0),0);}}}}}}else{s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp644 != 0;_tmp644=_tmp644->tl){struct _tuple23*_tmp679=(struct _tuple23*)
_tmp644->hd;struct Cyc_Absyn_Pat*_tmp67A=(*_tmp679).f2;if(_tmp67A->r == (void*)0)
continue;{struct _dyneither_ptr*f;{void*_tmp67B=(void*)((struct Cyc_List_List*)
_check_null((*_tmp679).f1))->hd;struct _dyneither_ptr*_tmp67C;_LL372: if(*((int*)
_tmp67B)!= 1)goto _LL374;_tmp67C=((struct Cyc_Absyn_FieldName_struct*)_tmp67B)->f1;
_LL373: f=_tmp67C;goto _LL371;_LL374:;_LL375:(int)_throw((void*)Cyc_Toc_Match_error);
_LL371:;}{struct _tuple1*_tmp67D=Cyc_Toc_temp_var();void*_tmp67E=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp67A->topt))->v;void*_tmp67F=Cyc_Toc_typ_to_c(
_tmp67E);decls=({struct Cyc_List_List*_tmp680=_region_malloc(rgn,sizeof(*_tmp680));
_tmp680->hd=({struct _tuple22*_tmp681=_region_malloc(rgn,sizeof(*_tmp681));
_tmp681->f1=_tmp67D;_tmp681->f2=_tmp67F;_tmp681;});_tmp680->tl=decls;_tmp680;});{
struct _tuple21 _tmp682=Cyc_Toc_xlate_pat(nv,rgn,_tmp67E,Cyc_Absyn_var_exp(_tmp67D,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp67A,(struct Cyc_Absyn_Stmt**)& fail_stmt,
fail_stmt,decls);nv=_tmp682.f1;decls=_tmp682.f2;{struct Cyc_Absyn_Exp*_tmp683=Cyc_Absyn_aggrmember_exp(
r,f,0);if(Cyc_Toc_is_void_star_or_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(
Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp665->impl))->fields,
f)))->type))_tmp683=Cyc_Toc_cast_it(_tmp67F,_tmp683);{struct Cyc_Absyn_Stmt*
_tmp684=_tmp682.f3;struct Cyc_Absyn_Stmt*_tmp685=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp67D,0),_tmp683,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp685,_tmp684,
0),0);}}}}}}}goto _LL343;}_LL364: if(_tmp623 <= (void*)2)goto _LL366;if(*((int*)
_tmp623)!= 4)goto _LL366;_tmp645=((struct Cyc_Absyn_Pointer_p_struct*)_tmp623)->f1;
_LL365: {struct _tuple1*_tmp686=Cyc_Toc_temp_var();void*_tmp687=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp645->topt))->v;decls=({struct Cyc_List_List*_tmp688=
_region_malloc(rgn,sizeof(*_tmp688));_tmp688->hd=({struct _tuple22*_tmp689=
_region_malloc(rgn,sizeof(*_tmp689));_tmp689->f1=_tmp686;_tmp689->f2=Cyc_Toc_typ_to_c(
_tmp687);_tmp689;});_tmp688->tl=decls;_tmp688;});{struct _tuple21 _tmp68A=Cyc_Toc_xlate_pat(
nv,rgn,_tmp687,Cyc_Absyn_var_exp(_tmp686,0),Cyc_Absyn_deref_exp(path,0),_tmp645,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp68A.f1;decls=_tmp68A.f2;{
struct Cyc_Absyn_Stmt*_tmp68B=_tmp68A.f3;struct Cyc_Absyn_Stmt*_tmp68C=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp686,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp68B,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp68C,0);else{s=_tmp68C;}goto _LL343;}}}_LL366: if(_tmp623 <= (void*)2)goto
_LL368;if(*((int*)_tmp623)!= 12)goto _LL368;_LL367:({void*_tmp68D=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp68E="unknownid pat";_tag_dyneither(_tmp68E,sizeof(char),14);}),
_tag_dyneither(_tmp68D,sizeof(void*),0));});_LL368: if(_tmp623 <= (void*)2)goto
_LL36A;if(*((int*)_tmp623)!= 13)goto _LL36A;_LL369:({void*_tmp68F=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp690="unknowncall pat";_tag_dyneither(_tmp690,sizeof(char),16);}),
_tag_dyneither(_tmp68F,sizeof(void*),0));});_LL36A: if(_tmp623 <= (void*)2)goto
_LL343;if(*((int*)_tmp623)!= 14)goto _LL343;_LL36B:({void*_tmp691=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp692="exp pat";_tag_dyneither(_tmp692,sizeof(char),8);}),_tag_dyneither(
_tmp691,sizeof(void*),0));});_LL343:;}return({struct _tuple21 _tmp693;_tmp693.f1=
nv;_tmp693.f2=decls;_tmp693.f3=s;_tmp693;});}struct _tuple24{struct _dyneither_ptr*
f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct
_tuple24*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple24*_tmp694=_region_malloc(r,sizeof(*_tmp694));_tmp694->f1=
Cyc_Toc_fresh_label();_tmp694->f2=Cyc_Toc_fresh_label();_tmp694->f3=sc;_tmp694;});}
static int Cyc_Toc_is_mixed_datatype(void*t){{void*_tmp695=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_DatatypeInfo _tmp696;union Cyc_Absyn_DatatypeInfoU _tmp697;struct
Cyc_Absyn_Datatypedecl**_tmp698;struct Cyc_Absyn_Datatypedecl*_tmp699;_LL377: if(
_tmp695 <= (void*)4)goto _LL379;if(*((int*)_tmp695)!= 2)goto _LL379;_tmp696=((
struct Cyc_Absyn_DatatypeType_struct*)_tmp695)->f1;_tmp697=_tmp696.datatype_info;
if((_tmp697.KnownDatatype).tag != 2)goto _LL379;_tmp698=(struct Cyc_Absyn_Datatypedecl**)(
_tmp697.KnownDatatype).val;_tmp699=*_tmp698;_LL378: {int seen_novalue=0;int
seen_value=0;{struct Cyc_List_List*_tmp69A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp699->fields))->v;for(0;(unsigned int)_tmp69A;_tmp69A=_tmp69A->tl){
if(((struct Cyc_Absyn_Datatypefield*)_tmp69A->hd)->typs == 0)seen_value=1;else{
seen_novalue=1;}if(seen_value  && seen_novalue)return 1;}}goto _LL376;}_LL379:;
_LL37A: goto _LL376;_LL376:;}return 0;}static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*
p){void*_tmp69B=p->r;struct Cyc_Absyn_Pat*_tmp69C;struct Cyc_Absyn_Pat*_tmp69D;
struct Cyc_Absyn_Pat _tmp69E;void*_tmp69F;struct Cyc_Absyn_Datatypedecl*_tmp6A0;
struct Cyc_Absyn_Datatypefield*_tmp6A1;struct Cyc_List_List*_tmp6A2;_LL37C: if(
_tmp69B <= (void*)2)goto _LL380;if(*((int*)_tmp69B)!= 0)goto _LL37E;_tmp69C=((
struct Cyc_Absyn_Var_p_struct*)_tmp69B)->f2;_LL37D: return Cyc_Toc_no_tag_test(
_tmp69C);_LL37E: if(*((int*)_tmp69B)!= 4)goto _LL380;_tmp69D=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp69B)->f1;_tmp69E=*_tmp69D;_tmp69F=_tmp69E.r;if(_tmp69F <= (void*)2)goto _LL380;
if(*((int*)_tmp69F)!= 6)goto _LL380;_tmp6A0=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp69F)->f1;_tmp6A1=((struct Cyc_Absyn_Datatype_p_struct*)_tmp69F)->f2;_tmp6A2=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp69F)->f3;if(!(_tmp6A2 != 0))goto _LL380;
_LL37F: return 0;_LL380:;_LL381: return 1;_LL37B:;}static void Cyc_Toc_xlate_switch(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct
Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*_tmp6A3=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;int leave_as_switch;{void*_tmp6A4=Cyc_Tcutil_compress(
_tmp6A3);_LL383: if(_tmp6A4 <= (void*)4)goto _LL387;if(*((int*)_tmp6A4)!= 5)goto
_LL385;_LL384: goto _LL386;_LL385: if(*((int*)_tmp6A4)!= 12)goto _LL387;_LL386:
leave_as_switch=1;goto _LL382;_LL387:;_LL388: leave_as_switch=0;goto _LL382;_LL382:;}{
struct Cyc_List_List*_tmp6A5=scs;for(0;_tmp6A5 != 0;_tmp6A5=_tmp6A5->tl){if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp6A5->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp6A5->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=
Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp6A6=scs;for(0;_tmp6A6 != 0;_tmp6A6=
_tmp6A6->tl){struct Cyc_Absyn_Stmt*_tmp6A7=((struct Cyc_Absyn_Switch_clause*)
_tmp6A6->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp6A6->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp6A7,0);next_l=Cyc_Toc_fresh_label();{struct Cyc_Toc_Env _tmp6A9;struct
_RegionHandle*_tmp6AA;struct Cyc_Toc_Env*_tmp6A8=nv;_tmp6A9=*_tmp6A8;_tmp6AA=
_tmp6A9.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(_tmp6AA,nv,next_l),
_tmp6A7);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*r=
Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct Cyc_Toc_Env _tmp6AC;struct
_RegionHandle*_tmp6AD;struct Cyc_Toc_Env*_tmp6AB=nv;_tmp6AC=*_tmp6AB;_tmp6AD=
_tmp6AC.rgn;{struct Cyc_Toc_Env*_tmp6AE=Cyc_Toc_share_env(_tmp6AD,nv);struct Cyc_List_List*
lscs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple24*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(_tmp6AD,Cyc_Toc_gen_label,_tmp6AD,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int
is_datatype=Cyc_Toc_is_mixed_datatype(_tmp6A3);int needs_tag_test=is_datatype;{
struct Cyc_List_List*_tmp6AF=lscs;for(0;_tmp6AF != 0;_tmp6AF=_tmp6AF->tl){struct
Cyc_Absyn_Switch_clause*sc=(*((struct _tuple24*)_tmp6AF->hd)).f3;struct
_dyneither_ptr*fail_lab=_tmp6AF->tl == 0?end_l:(*((struct _tuple24*)((struct Cyc_List_List*)
_check_null(_tmp6AF->tl))->hd)).f1;struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(
needs_tag_test  && !Cyc_Toc_no_tag_test(sc->pattern)){{struct Cyc_List_List*
_tmp6B0=_tmp6AF->tl;for(0;(unsigned int)_tmp6B0;_tmp6B0=_tmp6B0->tl){if(Cyc_Toc_no_tag_test(((*((
struct _tuple24*)_tmp6B0->hd)).f3)->pattern)){tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp6B1=_region_malloc(_tmp6AD,sizeof(*_tmp6B1));_tmp6B1[0]=Cyc_Absyn_goto_stmt((*((
struct _tuple24*)_tmp6B0->hd)).f1,0);_tmp6B1;});needs_tag_test=0;break;}}}if(
tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp6B2=_region_malloc(
_tmp6AD,sizeof(*_tmp6B2));_tmp6B2[0]=Cyc_Absyn_goto_stmt(fail_lab,0);_tmp6B2;});}{
struct Cyc_Toc_Env*_tmp6B4;struct Cyc_List_List*_tmp6B5;struct Cyc_Absyn_Stmt*
_tmp6B6;struct _tuple21 _tmp6B3=Cyc_Toc_xlate_pat(_tmp6AE,_tmp6AD,_tmp6A3,r,path,
sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp6B4=_tmp6B3.f1;
_tmp6B5=_tmp6B3.f2;_tmp6B6=_tmp6B3.f3;if(is_datatype  && Cyc_Toc_no_tag_test(sc->pattern))
needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp6B7=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp6B4,_tmp6B7);_tmp6B6=Cyc_Absyn_seq_stmt(
_tmp6B6,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,_tmp6B7,0),Cyc_Absyn_goto_stmt(
fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp6B5;nvs=({struct Cyc_List_List*
_tmp6B8=_region_malloc(_tmp6AD,sizeof(*_tmp6B8));_tmp6B8->hd=_tmp6B4;_tmp6B8->tl=
nvs;_tmp6B8;});test_stmts=({struct Cyc_List_List*_tmp6B9=_region_malloc(_tmp6AD,
sizeof(*_tmp6B9));_tmp6B9->hd=_tmp6B6;_tmp6B9->tl=test_stmts;_tmp6B9;});}}}nvs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple24 _tmp6BB;struct _dyneither_ptr*_tmp6BC;
struct _dyneither_ptr*_tmp6BD;struct Cyc_Absyn_Switch_clause*_tmp6BE;struct
_tuple24*_tmp6BA=(struct _tuple24*)lscs->hd;_tmp6BB=*_tmp6BA;_tmp6BC=_tmp6BB.f1;
_tmp6BD=_tmp6BB.f2;_tmp6BE=_tmp6BB.f3;{struct Cyc_Toc_Env*_tmp6BF=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp6BE->body;
struct Cyc_Toc_Env _tmp6C1;struct _RegionHandle*_tmp6C2;struct Cyc_Toc_Env*_tmp6C0=
_tmp6AE;_tmp6C1=*_tmp6C0;_tmp6C2=_tmp6C1.rgn;if(lscs->tl != 0){struct _tuple24
_tmp6C4;struct _dyneither_ptr*_tmp6C5;struct Cyc_Absyn_Switch_clause*_tmp6C6;
struct _tuple24*_tmp6C3=(struct _tuple24*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;
_tmp6C4=*_tmp6C3;_tmp6C5=_tmp6C4.f2;_tmp6C6=_tmp6C4.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp6C2,_tmp6BF,end_l,_tmp6C5,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp6C6->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(
_tmp6C2,_tmp6BF,end_l),s);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp6BC,(
struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),Cyc_Absyn_label_stmt(
_tmp6BD,s,0),0);stmts=({struct Cyc_List_List*_tmp6C7=_region_malloc(_tmp6AD,
sizeof(*_tmp6C7));_tmp6C7->hd=s;_tmp6C7->tl=stmts;_tmp6C7;});}}{struct Cyc_Absyn_Stmt*
res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),
0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple22 _tmp6C9;struct _tuple1*
_tmp6CA;void*_tmp6CB;struct _tuple22*_tmp6C8=(struct _tuple22*)decls->hd;_tmp6C9=*
_tmp6C8;_tmp6CA=_tmp6C9.f1;_tmp6CB=_tmp6C9.f2;res=Cyc_Absyn_declare_stmt(_tmp6CA,
_tmp6CB,0,res,0);}whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r;}}}}}}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(
struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(
int n){return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({
struct Cyc_List_List*_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0);_tmp6CC->tl=0;_tmp6CC;}),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s){if(n > 0)s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt(s->r,0));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp6CD=s->r;struct Cyc_Absyn_Exp*_tmp6CE;
struct Cyc_Absyn_Stmt*_tmp6CF;struct Cyc_Absyn_Stmt*_tmp6D0;struct Cyc_Absyn_Exp*
_tmp6D1;struct Cyc_Absyn_Exp*_tmp6D2;struct Cyc_Absyn_Stmt*_tmp6D3;struct Cyc_Absyn_Stmt*
_tmp6D4;struct _tuple9 _tmp6D5;struct Cyc_Absyn_Exp*_tmp6D6;struct Cyc_Absyn_Stmt*
_tmp6D7;struct Cyc_Absyn_Stmt*_tmp6D8;struct Cyc_Absyn_Stmt*_tmp6D9;struct Cyc_Absyn_Stmt*
_tmp6DA;struct Cyc_Absyn_Exp*_tmp6DB;struct _tuple9 _tmp6DC;struct Cyc_Absyn_Exp*
_tmp6DD;struct _tuple9 _tmp6DE;struct Cyc_Absyn_Exp*_tmp6DF;struct Cyc_Absyn_Stmt*
_tmp6E0;struct Cyc_Absyn_Exp*_tmp6E1;struct Cyc_List_List*_tmp6E2;struct Cyc_List_List*
_tmp6E3;struct Cyc_Absyn_Switch_clause**_tmp6E4;struct Cyc_Absyn_Decl*_tmp6E5;
struct Cyc_Absyn_Stmt*_tmp6E6;struct _dyneither_ptr*_tmp6E7;struct Cyc_Absyn_Stmt*
_tmp6E8;struct Cyc_Absyn_Stmt*_tmp6E9;struct _tuple9 _tmp6EA;struct Cyc_Absyn_Exp*
_tmp6EB;struct Cyc_Absyn_Stmt*_tmp6EC;struct Cyc_List_List*_tmp6ED;struct Cyc_Absyn_Exp*
_tmp6EE;_LL38A: if((int)_tmp6CD != 0)goto _LL38C;_LL38B: return;_LL38C: if(_tmp6CD <= (
void*)1)goto _LL38E;if(*((int*)_tmp6CD)!= 0)goto _LL38E;_tmp6CE=((struct Cyc_Absyn_Exp_s_struct*)
_tmp6CD)->f1;_LL38D: Cyc_Toc_exp_to_c(nv,_tmp6CE);return;_LL38E: if(_tmp6CD <= (
void*)1)goto _LL390;if(*((int*)_tmp6CD)!= 1)goto _LL390;_tmp6CF=((struct Cyc_Absyn_Seq_s_struct*)
_tmp6CD)->f1;_tmp6D0=((struct Cyc_Absyn_Seq_s_struct*)_tmp6CD)->f2;_LL38F: Cyc_Toc_stmt_to_c(
nv,_tmp6CF);s=_tmp6D0;continue;_LL390: if(_tmp6CD <= (void*)1)goto _LL392;if(*((int*)
_tmp6CD)!= 2)goto _LL392;_tmp6D1=((struct Cyc_Absyn_Return_s_struct*)_tmp6CD)->f1;
_LL391: {struct Cyc_Core_Opt*topt=0;if(_tmp6D1 != 0){topt=({struct Cyc_Core_Opt*
_tmp6EF=_cycalloc(sizeof(*_tmp6EF));_tmp6EF->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp6D1->topt))->v);_tmp6EF;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp6D1);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp6F0=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp6F1=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp6F0,0),0);s->r=(Cyc_Absyn_declare_stmt(
_tmp6F0,(void*)topt->v,_tmp6D1,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),
_tmp6F1,0),0))->r;}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}return;}_LL392:
if(_tmp6CD <= (void*)1)goto _LL394;if(*((int*)_tmp6CD)!= 3)goto _LL394;_tmp6D2=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6CD)->f1;_tmp6D3=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp6CD)->f2;_tmp6D4=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6CD)->f3;_LL393:
Cyc_Toc_exp_to_c(nv,_tmp6D2);Cyc_Toc_stmt_to_c(nv,_tmp6D3);s=_tmp6D4;continue;
_LL394: if(_tmp6CD <= (void*)1)goto _LL396;if(*((int*)_tmp6CD)!= 4)goto _LL396;
_tmp6D5=((struct Cyc_Absyn_While_s_struct*)_tmp6CD)->f1;_tmp6D6=_tmp6D5.f1;
_tmp6D7=((struct Cyc_Absyn_While_s_struct*)_tmp6CD)->f2;_LL395: Cyc_Toc_exp_to_c(
nv,_tmp6D6);{struct Cyc_Toc_Env _tmp6F3;struct _RegionHandle*_tmp6F4;struct Cyc_Toc_Env*
_tmp6F2=nv;_tmp6F3=*_tmp6F2;_tmp6F4=_tmp6F3.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp6F4,nv),_tmp6D7);return;}_LL396: if(_tmp6CD <= (void*)1)goto _LL398;if(*((int*)
_tmp6CD)!= 5)goto _LL398;_tmp6D8=((struct Cyc_Absyn_Break_s_struct*)_tmp6CD)->f1;
_LL397: {struct Cyc_Toc_Env _tmp6F6;struct _dyneither_ptr**_tmp6F7;struct Cyc_Toc_Env*
_tmp6F5=nv;_tmp6F6=*_tmp6F5;_tmp6F7=_tmp6F6.break_lab;if(_tmp6F7 != 0)s->r=Cyc_Toc_goto_stmt_r(*
_tmp6F7,0);{int dest_depth=_tmp6D8 == 0?0: _tmp6D8->try_depth;Cyc_Toc_do_npop_before(
s->try_depth - dest_depth,s);return;}}_LL398: if(_tmp6CD <= (void*)1)goto _LL39A;if(*((
int*)_tmp6CD)!= 6)goto _LL39A;_tmp6D9=((struct Cyc_Absyn_Continue_s_struct*)
_tmp6CD)->f1;_LL399: {struct Cyc_Toc_Env _tmp6F9;struct _dyneither_ptr**_tmp6FA;
struct Cyc_Toc_Env*_tmp6F8=nv;_tmp6F9=*_tmp6F8;_tmp6FA=_tmp6F9.continue_lab;if(
_tmp6FA != 0)s->r=Cyc_Toc_goto_stmt_r(*_tmp6FA,0);_tmp6DA=_tmp6D9;goto _LL39B;}
_LL39A: if(_tmp6CD <= (void*)1)goto _LL39C;if(*((int*)_tmp6CD)!= 7)goto _LL39C;
_tmp6DA=((struct Cyc_Absyn_Goto_s_struct*)_tmp6CD)->f2;_LL39B: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp6DA))->try_depth,s);
return;_LL39C: if(_tmp6CD <= (void*)1)goto _LL39E;if(*((int*)_tmp6CD)!= 8)goto
_LL39E;_tmp6DB=((struct Cyc_Absyn_For_s_struct*)_tmp6CD)->f1;_tmp6DC=((struct Cyc_Absyn_For_s_struct*)
_tmp6CD)->f2;_tmp6DD=_tmp6DC.f1;_tmp6DE=((struct Cyc_Absyn_For_s_struct*)_tmp6CD)->f3;
_tmp6DF=_tmp6DE.f1;_tmp6E0=((struct Cyc_Absyn_For_s_struct*)_tmp6CD)->f4;_LL39D:
Cyc_Toc_exp_to_c(nv,_tmp6DB);Cyc_Toc_exp_to_c(nv,_tmp6DD);Cyc_Toc_exp_to_c(nv,
_tmp6DF);{struct Cyc_Toc_Env _tmp6FC;struct _RegionHandle*_tmp6FD;struct Cyc_Toc_Env*
_tmp6FB=nv;_tmp6FC=*_tmp6FB;_tmp6FD=_tmp6FC.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp6FD,nv),_tmp6E0);return;}_LL39E: if(_tmp6CD <= (void*)1)goto _LL3A0;if(*((int*)
_tmp6CD)!= 9)goto _LL3A0;_tmp6E1=((struct Cyc_Absyn_Switch_s_struct*)_tmp6CD)->f1;
_tmp6E2=((struct Cyc_Absyn_Switch_s_struct*)_tmp6CD)->f2;_LL39F: Cyc_Toc_xlate_switch(
nv,s,_tmp6E1,_tmp6E2);return;_LL3A0: if(_tmp6CD <= (void*)1)goto _LL3A2;if(*((int*)
_tmp6CD)!= 10)goto _LL3A2;_tmp6E3=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp6CD)->f1;
_tmp6E4=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp6CD)->f2;_LL3A1: {struct Cyc_Toc_Env
_tmp6FF;struct Cyc_Toc_FallthruInfo*_tmp700;struct Cyc_Toc_Env*_tmp6FE=nv;_tmp6FF=*
_tmp6FE;_tmp700=_tmp6FF.fallthru_info;if(_tmp700 == 0)({void*_tmp701=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp702="fallthru in unexpected place";_tag_dyneither(_tmp702,sizeof(char),29);}),
_tag_dyneither(_tmp701,sizeof(void*),0));});{struct _dyneither_ptr*_tmp704;struct
Cyc_List_List*_tmp705;struct Cyc_Dict_Dict _tmp706;struct Cyc_Toc_FallthruInfo
_tmp703=*_tmp700;_tmp704=_tmp703.label;_tmp705=_tmp703.binders;_tmp706=_tmp703.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp704,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp6E4)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp707=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp705);struct Cyc_List_List*_tmp708=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp6E3);for(0;_tmp707 != 0;(_tmp707=_tmp707->tl,
_tmp708=_tmp708->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp708))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp706,(
struct _tuple1*)_tmp707->hd),(struct Cyc_Absyn_Exp*)_tmp708->hd,0),s2,0);}s->r=s2->r;
return;}}}}_LL3A2: if(_tmp6CD <= (void*)1)goto _LL3A4;if(*((int*)_tmp6CD)!= 11)goto
_LL3A4;_tmp6E5=((struct Cyc_Absyn_Decl_s_struct*)_tmp6CD)->f1;_tmp6E6=((struct Cyc_Absyn_Decl_s_struct*)
_tmp6CD)->f2;_LL3A3:{void*_tmp709=_tmp6E5->r;struct Cyc_Absyn_Vardecl*_tmp70A;
struct Cyc_Absyn_Pat*_tmp70B;struct Cyc_Absyn_Exp*_tmp70C;struct Cyc_List_List*
_tmp70D;struct Cyc_Absyn_Fndecl*_tmp70E;struct Cyc_Absyn_Tvar*_tmp70F;struct Cyc_Absyn_Vardecl*
_tmp710;int _tmp711;struct Cyc_Absyn_Exp*_tmp712;struct Cyc_Absyn_Exp*_tmp713;
struct Cyc_Absyn_Tvar*_tmp714;struct Cyc_Absyn_Vardecl*_tmp715;_LL3AD: if(_tmp709 <= (
void*)2)goto _LL3B9;if(*((int*)_tmp709)!= 0)goto _LL3AF;_tmp70A=((struct Cyc_Absyn_Var_d_struct*)
_tmp709)->f1;_LL3AE: {struct Cyc_Toc_Env _tmp717;struct _RegionHandle*_tmp718;
struct Cyc_Toc_Env*_tmp716=nv;_tmp717=*_tmp716;_tmp718=_tmp717.rgn;{struct Cyc_Toc_Env*
_tmp719=Cyc_Toc_add_varmap(_tmp718,nv,_tmp70A->name,Cyc_Absyn_varb_exp(_tmp70A->name,(
void*)({struct Cyc_Absyn_Local_b_struct*_tmp71A=_cycalloc(sizeof(*_tmp71A));
_tmp71A[0]=({struct Cyc_Absyn_Local_b_struct _tmp71B;_tmp71B.tag=3;_tmp71B.f1=
_tmp70A;_tmp71B;});_tmp71A;}),0));Cyc_Toc_local_decl_to_c(_tmp719,_tmp719,
_tmp70A,_tmp6E6);}goto _LL3AC;}_LL3AF: if(*((int*)_tmp709)!= 2)goto _LL3B1;_tmp70B=((
struct Cyc_Absyn_Let_d_struct*)_tmp709)->f1;_tmp70C=((struct Cyc_Absyn_Let_d_struct*)
_tmp709)->f3;_LL3B0:{void*_tmp71C=_tmp70B->r;struct Cyc_Absyn_Vardecl*_tmp71D;
struct Cyc_Absyn_Pat*_tmp71E;struct Cyc_Absyn_Pat _tmp71F;void*_tmp720;_LL3BC: if(
_tmp71C <= (void*)2)goto _LL3BE;if(*((int*)_tmp71C)!= 0)goto _LL3BE;_tmp71D=((
struct Cyc_Absyn_Var_p_struct*)_tmp71C)->f1;_tmp71E=((struct Cyc_Absyn_Var_p_struct*)
_tmp71C)->f2;_tmp71F=*_tmp71E;_tmp720=_tmp71F.r;if((int)_tmp720 != 0)goto _LL3BE;
_LL3BD: {struct _tuple1*old_name=_tmp71D->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp71D->name=new_name;_tmp71D->initializer=(struct Cyc_Absyn_Exp*)_tmp70C;
_tmp6E5->r=(void*)({struct Cyc_Absyn_Var_d_struct*_tmp721=_cycalloc(sizeof(*
_tmp721));_tmp721[0]=({struct Cyc_Absyn_Var_d_struct _tmp722;_tmp722.tag=0;_tmp722.f1=
_tmp71D;_tmp722;});_tmp721;});{struct Cyc_Toc_Env _tmp724;struct _RegionHandle*
_tmp725;struct Cyc_Toc_Env*_tmp723=nv;_tmp724=*_tmp723;_tmp725=_tmp724.rgn;{
struct Cyc_Toc_Env*_tmp726=Cyc_Toc_add_varmap(_tmp725,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp727=_cycalloc(sizeof(*
_tmp727));_tmp727[0]=({struct Cyc_Absyn_Local_b_struct _tmp728;_tmp728.tag=3;
_tmp728.f1=_tmp71D;_tmp728;});_tmp727;}),0));Cyc_Toc_local_decl_to_c(_tmp726,nv,
_tmp71D,_tmp6E6);}goto _LL3BB;}}_LL3BE:;_LL3BF: s->r=(Cyc_Toc_letdecl_to_c(nv,
_tmp70B,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp70C->topt))->v,_tmp70C,
_tmp6E6))->r;goto _LL3BB;_LL3BB:;}goto _LL3AC;_LL3B1: if(*((int*)_tmp709)!= 3)goto
_LL3B3;_tmp70D=((struct Cyc_Absyn_Letv_d_struct*)_tmp709)->f1;_LL3B2: {struct Cyc_List_List*
_tmp729=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp70D);
if(_tmp729 == 0)({void*_tmp72A=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp72B="empty Letv_d";
_tag_dyneither(_tmp72B,sizeof(char),13);}),_tag_dyneither(_tmp72A,sizeof(void*),
0));});_tmp6E5->r=(void*)({struct Cyc_Absyn_Var_d_struct*_tmp72C=_cycalloc(
sizeof(*_tmp72C));_tmp72C[0]=({struct Cyc_Absyn_Var_d_struct _tmp72D;_tmp72D.tag=0;
_tmp72D.f1=(struct Cyc_Absyn_Vardecl*)_tmp729->hd;_tmp72D;});_tmp72C;});_tmp729=
_tmp729->tl;for(0;_tmp729 != 0;_tmp729=_tmp729->tl){struct Cyc_Absyn_Decl*_tmp72E=
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp72F=_cycalloc(
sizeof(*_tmp72F));_tmp72F[0]=({struct Cyc_Absyn_Var_d_struct _tmp730;_tmp730.tag=0;
_tmp730.f1=(struct Cyc_Absyn_Vardecl*)_tmp729->hd;_tmp730;});_tmp72F;}),0);s->r=(
Cyc_Absyn_decl_stmt(_tmp72E,Cyc_Absyn_new_stmt(s->r,0),0))->r;}Cyc_Toc_stmt_to_c(
nv,s);goto _LL3AC;}_LL3B3: if(*((int*)_tmp709)!= 1)goto _LL3B5;_tmp70E=((struct Cyc_Absyn_Fn_d_struct*)
_tmp709)->f1;_LL3B4: {struct _tuple1*_tmp731=_tmp70E->name;struct Cyc_Toc_Env
_tmp733;struct _RegionHandle*_tmp734;struct Cyc_Toc_Env*_tmp732=nv;_tmp733=*
_tmp732;_tmp734=_tmp733.rgn;{struct Cyc_Toc_Env*_tmp735=Cyc_Toc_add_varmap(
_tmp734,nv,_tmp70E->name,Cyc_Absyn_var_exp(_tmp731,0));Cyc_Toc_fndecl_to_c(
_tmp735,_tmp70E,0);Cyc_Toc_stmt_to_c(_tmp735,_tmp6E6);}goto _LL3AC;}_LL3B5: if(*((
int*)_tmp709)!= 4)goto _LL3B7;_tmp70F=((struct Cyc_Absyn_Region_d_struct*)_tmp709)->f1;
_tmp710=((struct Cyc_Absyn_Region_d_struct*)_tmp709)->f2;_tmp711=((struct Cyc_Absyn_Region_d_struct*)
_tmp709)->f3;_tmp712=((struct Cyc_Absyn_Region_d_struct*)_tmp709)->f4;_LL3B6: {
struct Cyc_Absyn_Stmt*_tmp736=_tmp6E6;void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct
_tuple1*rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp710->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);struct Cyc_Toc_Env _tmp738;struct _RegionHandle*_tmp739;struct Cyc_Toc_Env*
_tmp737=nv;_tmp738=*_tmp737;_tmp739=_tmp738.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(
_tmp739,nv,x_var,x_exp),_tmp736);if(Cyc_Absyn_no_regions)s->r=(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp736,0))->r;
else{if(_tmp712 == 0)s->r=(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp73A=
_cycalloc(sizeof(*_tmp73A));_tmp73A->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0);_tmp73A->tl=0;_tmp73A;}),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*
_tmp73B[1];_tmp73B[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp73B,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),Cyc_Absyn_seq_stmt(
_tmp736,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct
Cyc_Absyn_Exp*_tmp73C[1];_tmp73C[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp73C,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0),0))->r;else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp712);s->r=(Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),
0,Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(
Cyc_Toc__open_dynregion_e,({struct Cyc_Absyn_Exp*_tmp73D[2];_tmp73D[1]=(struct Cyc_Absyn_Exp*)
_tmp712;_tmp73D[0]=Cyc_Absyn_address_exp(rh_exp,0);((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp73D,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_seq_stmt(_tmp736,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp73E[1];_tmp73E[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp73E,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0))->r;}}return;}_LL3B7: if(*((int*)_tmp709)!= 5)goto _LL3B9;
_tmp713=((struct Cyc_Absyn_Alias_d_struct*)_tmp709)->f1;_tmp714=((struct Cyc_Absyn_Alias_d_struct*)
_tmp709)->f2;_tmp715=((struct Cyc_Absyn_Alias_d_struct*)_tmp709)->f3;_LL3B8: {
struct _tuple1*old_name=_tmp715->name;struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp715->name=new_name;_tmp715->initializer=(struct Cyc_Absyn_Exp*)_tmp713;s->r=(
void*)({struct Cyc_Absyn_Decl_s_struct*_tmp73F=_cycalloc(sizeof(*_tmp73F));
_tmp73F[0]=({struct Cyc_Absyn_Decl_s_struct _tmp740;_tmp740.tag=11;_tmp740.f1=({
struct Cyc_Absyn_Decl*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741->r=(void*)({
struct Cyc_Absyn_Var_d_struct*_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742[0]=({
struct Cyc_Absyn_Var_d_struct _tmp743;_tmp743.tag=0;_tmp743.f1=_tmp715;_tmp743;});
_tmp742;});_tmp741->loc=0;_tmp741;});_tmp740.f2=_tmp6E6;_tmp740;});_tmp73F;});{
struct Cyc_Toc_Env _tmp745;struct _RegionHandle*_tmp746;struct Cyc_Toc_Env*_tmp744=
nv;_tmp745=*_tmp744;_tmp746=_tmp745.rgn;{struct Cyc_Toc_Env*_tmp747=Cyc_Toc_add_varmap(
_tmp746,nv,old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp748=_cycalloc(sizeof(*_tmp748));_tmp748[0]=({struct Cyc_Absyn_Local_b_struct
_tmp749;_tmp749.tag=3;_tmp749.f1=_tmp715;_tmp749;});_tmp748;}),0));Cyc_Toc_local_decl_to_c(
_tmp747,nv,_tmp715,_tmp6E6);}return;}}_LL3B9:;_LL3BA:({void*_tmp74A=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp74B="bad nested declaration within function";_tag_dyneither(_tmp74B,sizeof(
char),39);}),_tag_dyneither(_tmp74A,sizeof(void*),0));});_LL3AC:;}return;_LL3A4:
if(_tmp6CD <= (void*)1)goto _LL3A6;if(*((int*)_tmp6CD)!= 12)goto _LL3A6;_tmp6E7=((
struct Cyc_Absyn_Label_s_struct*)_tmp6CD)->f1;_tmp6E8=((struct Cyc_Absyn_Label_s_struct*)
_tmp6CD)->f2;_LL3A5: s=_tmp6E8;continue;_LL3A6: if(_tmp6CD <= (void*)1)goto _LL3A8;
if(*((int*)_tmp6CD)!= 13)goto _LL3A8;_tmp6E9=((struct Cyc_Absyn_Do_s_struct*)
_tmp6CD)->f1;_tmp6EA=((struct Cyc_Absyn_Do_s_struct*)_tmp6CD)->f2;_tmp6EB=_tmp6EA.f1;
_LL3A7: {struct Cyc_Toc_Env _tmp74D;struct _RegionHandle*_tmp74E;struct Cyc_Toc_Env*
_tmp74C=nv;_tmp74D=*_tmp74C;_tmp74E=_tmp74D.rgn;Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
_tmp74E,nv),_tmp6E9);Cyc_Toc_exp_to_c(nv,_tmp6EB);return;}_LL3A8: if(_tmp6CD <= (
void*)1)goto _LL3AA;if(*((int*)_tmp6CD)!= 14)goto _LL3AA;_tmp6EC=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp6CD)->f1;_tmp6ED=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp6CD)->f2;_LL3A9: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->v=(void*)e_typ;_tmp74F;});{struct
Cyc_Toc_Env _tmp751;struct _RegionHandle*_tmp752;struct Cyc_Toc_Env*_tmp750=nv;
_tmp751=*_tmp750;_tmp752=_tmp751.rgn;{struct Cyc_Toc_Env*_tmp753=Cyc_Toc_add_varmap(
_tmp752,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp753,_tmp6EC);{struct Cyc_Absyn_Stmt*
_tmp754=Cyc_Absyn_seq_stmt(_tmp6EC,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp755=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp756=
Cyc_Absyn_var_exp(_tmp755,0);struct Cyc_Absyn_Vardecl*_tmp757=Cyc_Absyn_new_vardecl(
_tmp755,Cyc_Absyn_exn_typ,0);_tmp756->topt=({struct Cyc_Core_Opt*_tmp758=
_cycalloc(sizeof(*_tmp758));_tmp758->v=(void*)Cyc_Absyn_exn_typ;_tmp758;});{
struct Cyc_Absyn_Pat*_tmp759=({struct Cyc_Absyn_Pat*_tmp769=_cycalloc(sizeof(*
_tmp769));_tmp769->r=(void*)({struct Cyc_Absyn_Var_p_struct*_tmp76B=_cycalloc(
sizeof(*_tmp76B));_tmp76B[0]=({struct Cyc_Absyn_Var_p_struct _tmp76C;_tmp76C.tag=0;
_tmp76C.f1=_tmp757;_tmp76C.f2=({struct Cyc_Absyn_Pat*_tmp76D=_cycalloc(sizeof(*
_tmp76D));_tmp76D->r=(void*)0;_tmp76D->topt=({struct Cyc_Core_Opt*_tmp76E=
_cycalloc(sizeof(*_tmp76E));_tmp76E->v=(void*)Cyc_Absyn_exn_typ;_tmp76E;});
_tmp76D->loc=0;_tmp76D;});_tmp76C;});_tmp76B;});_tmp769->topt=({struct Cyc_Core_Opt*
_tmp76A=_cycalloc(sizeof(*_tmp76A));_tmp76A->v=(void*)Cyc_Absyn_exn_typ;_tmp76A;});
_tmp769->loc=0;_tmp769;});struct Cyc_Absyn_Exp*_tmp75A=Cyc_Absyn_throw_exp(
_tmp756,0);_tmp75A->topt=({struct Cyc_Core_Opt*_tmp75B=_cycalloc(sizeof(*_tmp75B));
_tmp75B->v=(void*)((void*)0);_tmp75B;});{struct Cyc_Absyn_Stmt*_tmp75C=Cyc_Absyn_exp_stmt(
_tmp75A,0);struct Cyc_Absyn_Switch_clause*_tmp75D=({struct Cyc_Absyn_Switch_clause*
_tmp766=_cycalloc(sizeof(*_tmp766));_tmp766->pattern=_tmp759;_tmp766->pat_vars=({
struct Cyc_Core_Opt*_tmp767=_cycalloc(sizeof(*_tmp767));_tmp767->v=({struct Cyc_List_List*
_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768->hd=_tmp757;_tmp768->tl=0;_tmp768;});
_tmp767;});_tmp766->where_clause=0;_tmp766->body=_tmp75C;_tmp766->loc=0;_tmp766;});
struct Cyc_Absyn_Stmt*_tmp75E=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6ED,({struct
Cyc_List_List*_tmp765=_cycalloc(sizeof(*_tmp765));_tmp765->hd=_tmp75D;_tmp765->tl=
0;_tmp765;})),0);Cyc_Toc_stmt_to_c(_tmp753,_tmp75E);{struct Cyc_Absyn_Exp*_tmp75F=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp764=_cycalloc(
sizeof(*_tmp764));_tmp764->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp764->tl=0;_tmp764;}),0);struct Cyc_Absyn_Stmt*_tmp760=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp763=
_cycalloc(sizeof(*_tmp763));_tmp763->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp763->tl=
0;_tmp763;}),0),0);struct Cyc_Absyn_Exp*_tmp761=Cyc_Absyn_int_exp((void*)0,0,0);
struct Cyc_Absyn_Exp*_tmp762=Cyc_Absyn_int_exp((void*)0,1,0);s->r=(Cyc_Absyn_declare_stmt(
h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp760,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp761,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp75F,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp762,0),Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),
_tmp754,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Toc_cast_it(
e_typ,Cyc_Toc__exn_thrown_e),_tmp75E,0),0),0),0),0),0))->r;}}}}}return;}}_LL3AA:
if(_tmp6CD <= (void*)1)goto _LL389;if(*((int*)_tmp6CD)!= 15)goto _LL389;_tmp6EE=((
struct Cyc_Absyn_ResetRegion_s_struct*)_tmp6CD)->f1;_LL3AB: if(Cyc_Absyn_no_regions)
s->r=(void*)0;else{Cyc_Toc_exp_to_c(nv,_tmp6EE);s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(
Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp76F=_cycalloc(sizeof(*_tmp76F));
_tmp76F->hd=_tmp6EE;_tmp76F->tl=0;_tmp76F;}),0));}return;_LL389:;}}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);struct _tuple25{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp770=_new_region("frgn");struct _RegionHandle*frgn=& _tmp770;
_push_region(frgn);{struct Cyc_Toc_Env*_tmp771=Cyc_Toc_share_env(frgn,nv);{struct
Cyc_List_List*_tmp772=f->args;for(0;_tmp772 != 0;_tmp772=_tmp772->tl){struct
_tuple1*_tmp773=({struct _tuple1*_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774->f1=(
union Cyc_Absyn_Nmspace)Cyc_Absyn_Loc_n;_tmp774->f2=(*((struct _tuple25*)_tmp772->hd)).f1;
_tmp774;});(*((struct _tuple25*)_tmp772->hd)).f3=Cyc_Toc_typ_to_c((*((struct
_tuple25*)_tmp772->hd)).f3);_tmp771=Cyc_Toc_add_varmap(frgn,_tmp771,_tmp773,Cyc_Absyn_var_exp(
_tmp773,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(f->body);_npop_handler(0);
return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp776;struct Cyc_Absyn_Tqual
_tmp777;void*_tmp778;int _tmp779;struct Cyc_Absyn_VarargInfo _tmp775=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp776=_tmp775.name;_tmp777=_tmp775.tq;_tmp778=
_tmp775.type;_tmp779=_tmp775.inject;{void*_tmp77A=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp778,(void*)2,_tmp777,Cyc_Absyn_false_conref));struct _tuple1*_tmp77B=({struct
_tuple1*_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E->f1=(union Cyc_Absyn_Nmspace)
Cyc_Absyn_Loc_n;_tmp77E->f2=(struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp776))->v;_tmp77E;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp77C=
_cycalloc(sizeof(*_tmp77C));_tmp77C->hd=({struct _tuple25*_tmp77D=_cycalloc(
sizeof(*_tmp77D));_tmp77D->f1=(struct _dyneither_ptr*)_tmp776->v;_tmp77D->f2=
_tmp777;_tmp77D->f3=_tmp77A;_tmp77D;});_tmp77C->tl=0;_tmp77C;}));_tmp771=Cyc_Toc_add_varmap(
frgn,_tmp771,_tmp77B,Cyc_Absyn_var_exp(_tmp77B,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp77F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp77F != 0;_tmp77F=_tmp77F->tl){((struct Cyc_Absyn_Vardecl*)_tmp77F->hd)->type=
Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp77F->hd)->type);}}Cyc_Toc_stmt_to_c(
Cyc_Toc_clear_toplevel(frgn,_tmp771),f->body);};_pop_region(frgn);}}static void*
Cyc_Toc_scope_to_c(void*s){void*_tmp780=s;_LL3C1: if((int)_tmp780 != 1)goto _LL3C3;
_LL3C2: return(void*)2;_LL3C3: if((int)_tmp780 != 4)goto _LL3C5;_LL3C4: return(void*)
3;_LL3C5:;_LL3C6: return s;_LL3C0:;}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*
ad){struct _tuple1*_tmp781=ad->name;struct _DynRegionHandle*_tmp783;struct Cyc_Dict_Dict*
_tmp784;struct Cyc_Toc_TocState _tmp782=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp783=_tmp782.dyn;_tmp784=_tmp782.aggrs_so_far;{struct _DynRegionFrame _tmp785;
struct _RegionHandle*d=_open_dynregion(& _tmp785,_tmp783);{int seen_defn_before;
struct _tuple15**_tmp786=((struct _tuple15**(*)(struct Cyc_Dict_Dict d,struct _tuple1*
k))Cyc_Dict_lookup_opt)(*_tmp784,_tmp781);if(_tmp786 == 0){seen_defn_before=0;{
struct _tuple15*v;if(ad->kind == (void*)0)v=({struct _tuple15*_tmp787=
_region_malloc(d,sizeof(*_tmp787));_tmp787->f1=ad;_tmp787->f2=Cyc_Absyn_strctq(
_tmp781);_tmp787;});else{v=({struct _tuple15*_tmp788=_region_malloc(d,sizeof(*
_tmp788));_tmp788->f1=ad;_tmp788->f2=Cyc_Absyn_unionq_typ(_tmp781);_tmp788;});}*
_tmp784=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct
_tuple15*v))Cyc_Dict_insert)(*_tmp784,_tmp781,v);}}else{struct _tuple15 _tmp78A;
struct Cyc_Absyn_Aggrdecl*_tmp78B;void*_tmp78C;struct _tuple15*_tmp789=*_tmp786;
_tmp78A=*_tmp789;_tmp78B=_tmp78A.f1;_tmp78C=_tmp78A.f2;if(_tmp78B->impl == 0){*
_tmp784=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct
_tuple15*v))Cyc_Dict_insert)(*_tmp784,_tmp781,({struct _tuple15*_tmp78D=
_region_malloc(d,sizeof(*_tmp78D));_tmp78D->f1=ad;_tmp78D->f2=_tmp78C;_tmp78D;}));
seen_defn_before=0;}else{seen_defn_before=1;}}{struct Cyc_Absyn_Aggrdecl*new_ad=({
struct Cyc_Absyn_Aggrdecl*_tmp7AC=_cycalloc(sizeof(*_tmp7AC));_tmp7AC->kind=ad->kind;
_tmp7AC->sc=(void*)2;_tmp7AC->name=ad->name;_tmp7AC->tvs=0;_tmp7AC->impl=0;
_tmp7AC->attributes=ad->attributes;_tmp7AC;});if(ad->impl != 0  && !
seen_defn_before){new_ad->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp78E=_cycalloc(
sizeof(*_tmp78E));_tmp78E->exist_vars=0;_tmp78E->rgn_po=0;_tmp78E->fields=0;
_tmp78E->tagged=0;_tmp78E;});{struct Cyc_List_List*new_fields=0;{struct Cyc_List_List*
_tmp78F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp78F != 0;_tmp78F=_tmp78F->tl){struct Cyc_Absyn_Aggrfield*_tmp790=(struct Cyc_Absyn_Aggrfield*)
_tmp78F->hd;struct Cyc_Absyn_Aggrfield*_tmp791=({struct Cyc_Absyn_Aggrfield*
_tmp7A7=_cycalloc(sizeof(*_tmp7A7));_tmp7A7->name=_tmp790->name;_tmp7A7->tq=Cyc_Toc_mt_tq;
_tmp7A7->type=Cyc_Toc_typ_to_c_array(_tmp790->type);_tmp7A7->width=_tmp790->width;
_tmp7A7->attributes=_tmp790->attributes;_tmp7A7;});if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->tagged){void*_tmp792=_tmp791->type;struct _dyneither_ptr*
_tmp793=_tmp791->name;struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp7A5;_tmp7A5.tag=0;_tmp7A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp793);{struct Cyc_String_pa_struct _tmp7A4;_tmp7A4.tag=0;_tmp7A4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*(*ad->name).f2);{void*_tmp7A2[2]={&
_tmp7A4,& _tmp7A5};Cyc_aprintf(({const char*_tmp7A3="_union_%s_%s";_tag_dyneither(
_tmp7A3,sizeof(char),13);}),_tag_dyneither(_tmp7A2,sizeof(void*),2));}}});struct
_dyneither_ptr*str=({struct _dyneither_ptr*_tmp7A1=_cycalloc(sizeof(*_tmp7A1));
_tmp7A1[0]=s;_tmp7A1;});struct Cyc_Absyn_Aggrfield*_tmp794=({struct Cyc_Absyn_Aggrfield*
_tmp7A0=_cycalloc(sizeof(*_tmp7A0));_tmp7A0->name=Cyc_Toc_val_sp;_tmp7A0->tq=Cyc_Toc_mt_tq;
_tmp7A0->type=_tmp792;_tmp7A0->width=0;_tmp7A0->attributes=0;_tmp7A0;});struct
Cyc_Absyn_Aggrfield*_tmp795=({struct Cyc_Absyn_Aggrfield*_tmp79F=_cycalloc(
sizeof(*_tmp79F));_tmp79F->name=Cyc_Toc_tag_sp;_tmp79F->tq=Cyc_Toc_mt_tq;_tmp79F->type=
Cyc_Absyn_sint_typ;_tmp79F->width=0;_tmp79F->attributes=0;_tmp79F;});struct Cyc_List_List*
_tmp796=({struct Cyc_Absyn_Aggrfield*_tmp79E[2];_tmp79E[1]=_tmp794;_tmp79E[0]=
_tmp795;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp79E,sizeof(struct Cyc_Absyn_Aggrfield*),2));});struct Cyc_Absyn_Aggrdecl*
_tmp797=({struct Cyc_Absyn_Aggrdecl*_tmp79B=_cycalloc(sizeof(*_tmp79B));_tmp79B->kind=(
void*)0;_tmp79B->sc=(void*)2;_tmp79B->name=({struct _tuple1*_tmp79D=_cycalloc(
sizeof(*_tmp79D));_tmp79D->f1=Cyc_Absyn_Loc_n;_tmp79D->f2=str;_tmp79D;});_tmp79B->tvs=
0;_tmp79B->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp79C=_cycalloc(sizeof(*_tmp79C));
_tmp79C->exist_vars=0;_tmp79C->rgn_po=0;_tmp79C->fields=_tmp796;_tmp79C->tagged=
0;_tmp79C;});_tmp79B->attributes=0;_tmp79B;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp798=_cycalloc(sizeof(*_tmp798));_tmp798->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp799=_cycalloc(sizeof(*_tmp799));_tmp799[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp79A;_tmp79A.tag=6;_tmp79A.f1=_tmp797;_tmp79A;});
_tmp799;}),0);_tmp798->tl=Cyc_Toc_result_decls;_tmp798;});_tmp791->type=Cyc_Absyn_strct(
str);}new_fields=({struct Cyc_List_List*_tmp7A6=_cycalloc(sizeof(*_tmp7A6));
_tmp7A6->hd=_tmp791;_tmp7A6->tl=new_fields;_tmp7A6;});}}(new_ad->impl)->fields=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);}}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7A8=_cycalloc(sizeof(*_tmp7A8));
_tmp7A8->hd=({struct Cyc_Absyn_Decl*_tmp7A9=_cycalloc(sizeof(*_tmp7A9));_tmp7A9->r=(
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));
_tmp7AA[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp7AB;_tmp7AB.tag=6;_tmp7AB.f1=
new_ad;_tmp7AB;});_tmp7AA;});_tmp7A9->loc=0;_tmp7A9;});_tmp7A8->tl=Cyc_Toc_result_decls;
_tmp7A8;});}};_pop_dynregion(d);}}static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*
tud){struct _DynRegionHandle*_tmp7AE;struct Cyc_Set_Set**_tmp7AF;struct Cyc_Toc_TocState
_tmp7AD=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7AE=
_tmp7AD.dyn;_tmp7AF=_tmp7AD.datatypes_so_far;{struct _DynRegionFrame _tmp7B0;
struct _RegionHandle*d=_open_dynregion(& _tmp7B0,_tmp7AE);{struct _tuple1*_tmp7B1=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))
Cyc_Set_member)(*_tmp7AF,_tmp7B1)){_npop_handler(0);return;}*_tmp7AF=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp7AF,_tmp7B1);}{struct Cyc_List_List*_tmp7B2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp7B2 != 0;_tmp7B2=_tmp7B2->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp7B2->hd;if(f->typs != 0){struct Cyc_List_List*
_tmp7B3=0;int i=1;{struct Cyc_List_List*_tmp7B4=f->typs;for(0;_tmp7B4 != 0;(_tmp7B4=
_tmp7B4->tl,i ++)){struct _dyneither_ptr*_tmp7B5=Cyc_Absyn_fieldname(i);struct Cyc_Absyn_Aggrfield*
_tmp7B6=({struct Cyc_Absyn_Aggrfield*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));_tmp7B8->name=
_tmp7B5;_tmp7B8->tq=(*((struct _tuple10*)_tmp7B4->hd)).f1;_tmp7B8->type=Cyc_Toc_typ_to_c_array((*((
struct _tuple10*)_tmp7B4->hd)).f2);_tmp7B8->width=0;_tmp7B8->attributes=0;_tmp7B8;});
_tmp7B3=({struct Cyc_List_List*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7->hd=
_tmp7B6;_tmp7B7->tl=_tmp7B3;_tmp7B7;});}}_tmp7B3=({struct Cyc_List_List*_tmp7B9=
_cycalloc(sizeof(*_tmp7B9));_tmp7B9->hd=({struct Cyc_Absyn_Aggrfield*_tmp7BA=
_cycalloc(sizeof(*_tmp7BA));_tmp7BA->name=Cyc_Toc_tag_sp;_tmp7BA->tq=Cyc_Toc_mt_tq;
_tmp7BA->type=Cyc_Absyn_sint_typ;_tmp7BA->width=0;_tmp7BA->attributes=0;_tmp7BA;});
_tmp7B9->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp7B3);_tmp7B9;});{struct Cyc_Absyn_Aggrdecl*_tmp7BB=({struct Cyc_Absyn_Aggrdecl*
_tmp7BF=_cycalloc(sizeof(*_tmp7BF));_tmp7BF->kind=(void*)0;_tmp7BF->sc=(void*)2;
_tmp7BF->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*_tmp7C1="_struct";
_tag_dyneither(_tmp7C1,sizeof(char),8);}));_tmp7BF->tvs=0;_tmp7BF->impl=({struct
Cyc_Absyn_AggrdeclImpl*_tmp7C0=_cycalloc(sizeof(*_tmp7C0));_tmp7C0->exist_vars=0;
_tmp7C0->rgn_po=0;_tmp7C0->fields=_tmp7B3;_tmp7C0->tagged=0;_tmp7C0;});_tmp7BF->attributes=
0;_tmp7BF;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp7BC=_cycalloc(
sizeof(*_tmp7BC));_tmp7BC->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp7BE;_tmp7BE.tag=6;_tmp7BE.f1=_tmp7BB;_tmp7BE;});_tmp7BD;}),0);_tmp7BC->tl=
Cyc_Toc_result_decls;_tmp7BC;});}}}};_pop_dynregion(d);}}static void Cyc_Toc_xdatatypedecl_to_c(
struct Cyc_Absyn_Datatypedecl*xd){if(xd->fields == 0)return;{struct
_DynRegionHandle*_tmp7C3;struct Cyc_Dict_Dict*_tmp7C4;struct Cyc_Toc_TocState
_tmp7C2=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7C3=
_tmp7C2.dyn;_tmp7C4=_tmp7C2.xdatatypes_so_far;{struct _DynRegionFrame _tmp7C5;
struct _RegionHandle*d=_open_dynregion(& _tmp7C5,_tmp7C3);{struct _tuple1*_tmp7C6=
xd->name;struct Cyc_List_List*_tmp7C7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp7C7 != 0;_tmp7C7=_tmp7C7->tl){struct Cyc_Absyn_Datatypefield*
f=(struct Cyc_Absyn_Datatypefield*)_tmp7C7->hd;struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp7C8=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(
char))+ 4,0);void*_tmp7C9=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp7C8,Cyc_Absyn_false_conref,0);int*_tmp7CA=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp7C4,f->name);int
_tmp7CB;_LL3C8: if(_tmp7CA != 0)goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != (void*)3){char zero='\000';initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp7D2;_tmp7D2.tag=0;_tmp7D2.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn);{struct Cyc_Int_pa_struct _tmp7D1;
_tmp7D1.tag=1;_tmp7D1.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7D0;_tmp7D0.tag=1;_tmp7D0.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7CF;_tmp7CF.tag=1;_tmp7CF.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7CE;_tmp7CE.tag=1;_tmp7CE.f1=(unsigned long)((int)zero);{void*_tmp7CC[5]={&
_tmp7CE,& _tmp7CF,& _tmp7D0,& _tmp7D1,& _tmp7D2};Cyc_aprintf(({const char*_tmp7CD="%c%c%c%c%s";
_tag_dyneither(_tmp7CD,sizeof(char),11);}),_tag_dyneither(_tmp7CC,sizeof(void*),
5));}}}}}}),0);}{struct Cyc_Absyn_Vardecl*_tmp7D3=Cyc_Absyn_new_vardecl(f->name,
_tmp7C9,initopt);_tmp7D3->sc=f->sc;Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp7D4=_cycalloc(sizeof(*_tmp7D4));_tmp7D4->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp7D5=_cycalloc(sizeof(*_tmp7D5));_tmp7D5[0]=({
struct Cyc_Absyn_Var_d_struct _tmp7D6;_tmp7D6.tag=0;_tmp7D6.f1=_tmp7D3;_tmp7D6;});
_tmp7D5;}),0);_tmp7D4->tl=Cyc_Toc_result_decls;_tmp7D4;});*_tmp7C4=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp7C4,f->name,f->sc
!= (void*)3);if(f->typs != 0){struct Cyc_List_List*fields=0;int i=1;{struct Cyc_List_List*
_tmp7D7=f->typs;for(0;_tmp7D7 != 0;(_tmp7D7=_tmp7D7->tl,i ++)){struct
_dyneither_ptr*_tmp7D8=({struct _dyneither_ptr*_tmp7DC=_cycalloc(sizeof(*_tmp7DC));
_tmp7DC[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp7DF;_tmp7DF.tag=1;
_tmp7DF.f1=(unsigned long)i;{void*_tmp7DD[1]={& _tmp7DF};Cyc_aprintf(({const char*
_tmp7DE="f%d";_tag_dyneither(_tmp7DE,sizeof(char),4);}),_tag_dyneither(_tmp7DD,
sizeof(void*),1));}});_tmp7DC;});struct Cyc_Absyn_Aggrfield*_tmp7D9=({struct Cyc_Absyn_Aggrfield*
_tmp7DB=_cycalloc(sizeof(*_tmp7DB));_tmp7DB->name=_tmp7D8;_tmp7DB->tq=(*((struct
_tuple10*)_tmp7D7->hd)).f1;_tmp7DB->type=Cyc_Toc_typ_to_c_array((*((struct
_tuple10*)_tmp7D7->hd)).f2);_tmp7DB->width=0;_tmp7DB->attributes=0;_tmp7DB;});
fields=({struct Cyc_List_List*_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA->hd=
_tmp7D9;_tmp7DA->tl=fields;_tmp7DA;});}}fields=({struct Cyc_List_List*_tmp7E0=
_cycalloc(sizeof(*_tmp7E0));_tmp7E0->hd=({struct Cyc_Absyn_Aggrfield*_tmp7E1=
_cycalloc(sizeof(*_tmp7E1));_tmp7E1->name=Cyc_Toc_tag_sp;_tmp7E1->tq=Cyc_Toc_mt_tq;
_tmp7E1->type=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp7E1->width=
0;_tmp7E1->attributes=0;_tmp7E1;});_tmp7E0->tl=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp7E0;});{struct Cyc_Absyn_Aggrdecl*
_tmp7E2=({struct Cyc_Absyn_Aggrdecl*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));_tmp7E6->kind=(
void*)0;_tmp7E6->sc=(void*)2;_tmp7E6->name=Cyc_Toc_collapse_qvar_tag(f->name,({
const char*_tmp7E8="_struct";_tag_dyneither(_tmp7E8,sizeof(char),8);}));_tmp7E6->tvs=
0;_tmp7E6->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp7E7=_cycalloc(sizeof(*_tmp7E7));
_tmp7E7->exist_vars=0;_tmp7E7->rgn_po=0;_tmp7E7->fields=fields;_tmp7E7->tagged=0;
_tmp7E7;});_tmp7E6->attributes=0;_tmp7E6;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp7E3=_cycalloc(sizeof(*_tmp7E3));_tmp7E3->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp7E4=_cycalloc(sizeof(*_tmp7E4));_tmp7E4[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp7E5;_tmp7E5.tag=6;_tmp7E5.f1=_tmp7E2;_tmp7E5;});
_tmp7E4;}),0);_tmp7E3->tl=Cyc_Toc_result_decls;_tmp7E3;});}}goto _LL3C7;}}_LL3CA:
if(_tmp7CA == 0)goto _LL3CC;_tmp7CB=*_tmp7CA;if(_tmp7CB != 0)goto _LL3CC;_LL3CB: if(f->sc
!= (void*)3){char zero='\000';struct Cyc_Absyn_Exp*_tmp7E9=Cyc_Absyn_string_exp((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp7F4;_tmp7F4.tag=0;_tmp7F4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn);{struct Cyc_Int_pa_struct _tmp7F3;
_tmp7F3.tag=1;_tmp7F3.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7F2;_tmp7F2.tag=1;_tmp7F2.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7F1;_tmp7F1.tag=1;_tmp7F1.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp7F0;_tmp7F0.tag=1;_tmp7F0.f1=(unsigned long)((int)zero);{void*_tmp7EE[5]={&
_tmp7F0,& _tmp7F1,& _tmp7F2,& _tmp7F3,& _tmp7F4};Cyc_aprintf(({const char*_tmp7EF="%c%c%c%c%s";
_tag_dyneither(_tmp7EF,sizeof(char),11);}),_tag_dyneither(_tmp7EE,sizeof(void*),
5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp7EA=Cyc_Absyn_new_vardecl(f->name,
_tmp7C9,(struct Cyc_Absyn_Exp*)_tmp7E9);_tmp7EA->sc=f->sc;Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp7EB=_cycalloc(sizeof(*_tmp7EB));_tmp7EB->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp7EC=_cycalloc(sizeof(*_tmp7EC));_tmp7EC[
0]=({struct Cyc_Absyn_Var_d_struct _tmp7ED;_tmp7ED.tag=0;_tmp7ED.f1=_tmp7EA;
_tmp7ED;});_tmp7EC;}),0);_tmp7EB->tl=Cyc_Toc_result_decls;_tmp7EB;});*_tmp7C4=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp7C4,f->name,1);}goto _LL3C7;_LL3CC:;_LL3CD: goto _LL3C7;_LL3C7:;}};
_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Enumdecl*ed){ed->sc=(void*)2;if(ed->fields != 0)Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){void*old_typ=vd->type;vd->type=
Cyc_Toc_typ_to_c_array(old_typ);if(vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))vd->sc=(void*)2;Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);void*
_tmp7F5=init->r;struct Cyc_Absyn_Vardecl*_tmp7F6;struct Cyc_Absyn_Exp*_tmp7F7;
struct Cyc_Absyn_Exp*_tmp7F8;int _tmp7F9;_LL3CF: if(*((int*)_tmp7F5)!= 29)goto
_LL3D1;_tmp7F6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7F5)->f1;_tmp7F7=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp7F5)->f2;_tmp7F8=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp7F5)->f3;_tmp7F9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7F5)->f4;
_LL3D0: vd->initializer=0;s->r=(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(
vd->name,0),_tmp7F6,_tmp7F7,_tmp7F8,_tmp7F9,Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3CE;_LL3D1:;_LL3D2: if(vd->sc == (void*)0){struct Cyc_Toc_Env _tmp7FB;struct
_RegionHandle*_tmp7FC;struct Cyc_Toc_Env*_tmp7FA=init_nv;_tmp7FB=*_tmp7FA;_tmp7FC=
_tmp7FB.rgn;{struct Cyc_Toc_Env*_tmp7FD=Cyc_Toc_set_toplevel(_tmp7FC,init_nv);Cyc_Toc_exp_to_c(
_tmp7FD,init);}}else{Cyc_Toc_exp_to_c(init_nv,init);}goto _LL3CE;_LL3CE:;}else{
void*_tmp7FE=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo _tmp7FF;void*
_tmp800;struct Cyc_Absyn_Exp*_tmp801;union Cyc_Absyn_Constraint*_tmp802;_LL3D4: if(
_tmp7FE <= (void*)4)goto _LL3D6;if(*((int*)_tmp7FE)!= 7)goto _LL3D6;_tmp7FF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp7FE)->f1;_tmp800=_tmp7FF.elt_type;_tmp801=
_tmp7FF.num_elts;_tmp802=_tmp7FF.zero_term;if(!((int(*)(int y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(0,_tmp802))goto _LL3D6;_LL3D5: if(_tmp801 == 0)({void*
_tmp803=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp804="can't initialize zero-terminated array -- size unknown";
_tag_dyneither(_tmp804,sizeof(char),55);}),_tag_dyneither(_tmp803,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp801;struct Cyc_Absyn_Exp*
_tmp805=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp806=Cyc_Absyn_signed_int_exp(
0,0);s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp805,
_tmp806,0),0),Cyc_Absyn_new_stmt(s->r,0));goto _LL3D3;}_LL3D6:;_LL3D7: goto _LL3D3;
_LL3D3:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(
nv,e);{struct _tuple1*x=Cyc_Toc_temp_var();struct Cyc_Toc_Env _tmp808;struct
_RegionHandle*_tmp809;struct Cyc_Toc_Env*_tmp807=nv;_tmp808=*_tmp807;_tmp809=
_tmp808.rgn;{struct Cyc_Absyn_Stmt*_tmp80A=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp80B=Cyc_Toc_share_env(_tmp809,nv);struct Cyc_Toc_Env*_tmp80D;struct Cyc_List_List*
_tmp80E;struct Cyc_Absyn_Stmt*_tmp80F;struct _tuple21 _tmp80C=Cyc_Toc_xlate_pat(
_tmp80B,_tmp809,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)&
_tmp80A,Cyc_Toc_throw_match_stmt(),0);_tmp80D=_tmp80C.f1;_tmp80E=_tmp80C.f2;
_tmp80F=_tmp80C.f3;Cyc_Toc_stmt_to_c(_tmp80D,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp80F,s,0),0);for(0;_tmp80E != 0;
_tmp80E=_tmp80E->tl){struct _tuple22 _tmp811;struct _tuple1*_tmp812;void*_tmp813;
struct _tuple22*_tmp810=(struct _tuple22*)_tmp80E->hd;_tmp811=*_tmp810;_tmp812=
_tmp811.f1;_tmp813=_tmp811.f2;s=Cyc_Absyn_declare_stmt(_tmp812,_tmp813,0,s,0);}}
return s;}}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp814=e->r;
struct Cyc_Absyn_Exp*_tmp815;struct Cyc_Absyn_Exp*_tmp816;struct Cyc_Absyn_Exp*
_tmp817;struct Cyc_Absyn_Exp*_tmp818;struct Cyc_Absyn_Exp*_tmp819;struct Cyc_Absyn_Exp*
_tmp81A;struct Cyc_Absyn_Exp*_tmp81B;struct Cyc_Absyn_Exp*_tmp81C;struct Cyc_List_List*
_tmp81D;struct Cyc_Absyn_Exp*_tmp81E;struct Cyc_Absyn_Exp*_tmp81F;struct Cyc_Absyn_Exp*
_tmp820;struct Cyc_Absyn_Exp*_tmp821;struct Cyc_Absyn_Exp*_tmp822;struct Cyc_Absyn_Exp*
_tmp823;struct Cyc_Absyn_Exp*_tmp824;struct Cyc_Absyn_Exp*_tmp825;struct Cyc_Absyn_Exp*
_tmp826;struct Cyc_Absyn_Exp*_tmp827;struct Cyc_Absyn_Exp*_tmp828;struct Cyc_Absyn_Exp*
_tmp829;struct Cyc_Absyn_Exp*_tmp82A;struct Cyc_Absyn_Exp*_tmp82B;struct Cyc_Absyn_Exp*
_tmp82C;struct Cyc_Absyn_Exp*_tmp82D;struct Cyc_List_List*_tmp82E;struct Cyc_Absyn_Exp*
_tmp82F;struct Cyc_List_List*_tmp830;void*_tmp831;void**_tmp832;struct Cyc_Absyn_Exp*
_tmp833;struct _tuple8*_tmp834;struct _tuple8 _tmp835;void*_tmp836;void**_tmp837;
struct Cyc_List_List*_tmp838;struct Cyc_List_List*_tmp839;struct Cyc_List_List*
_tmp83A;void*_tmp83B;void**_tmp83C;void*_tmp83D;void**_tmp83E;struct Cyc_Absyn_Stmt*
_tmp83F;struct Cyc_Absyn_MallocInfo _tmp840;struct Cyc_Absyn_MallocInfo*_tmp841;
_LL3D9: if(*((int*)_tmp814)!= 22)goto _LL3DB;_tmp815=((struct Cyc_Absyn_Deref_e_struct*)
_tmp814)->f1;_LL3DA: _tmp816=_tmp815;goto _LL3DC;_LL3DB: if(*((int*)_tmp814)!= 23)
goto _LL3DD;_tmp816=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp814)->f1;_LL3DC:
_tmp817=_tmp816;goto _LL3DE;_LL3DD: if(*((int*)_tmp814)!= 24)goto _LL3DF;_tmp817=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp814)->f1;_LL3DE: _tmp818=_tmp817;goto
_LL3E0;_LL3DF: if(*((int*)_tmp814)!= 16)goto _LL3E1;_tmp818=((struct Cyc_Absyn_Address_e_struct*)
_tmp814)->f1;_LL3E0: _tmp819=_tmp818;goto _LL3E2;_LL3E1: if(*((int*)_tmp814)!= 12)
goto _LL3E3;_tmp819=((struct Cyc_Absyn_Throw_e_struct*)_tmp814)->f1;_LL3E2: _tmp81A=
_tmp819;goto _LL3E4;_LL3E3: if(*((int*)_tmp814)!= 13)goto _LL3E5;_tmp81A=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp814)->f1;_LL3E4: _tmp81B=_tmp81A;goto _LL3E6;
_LL3E5: if(*((int*)_tmp814)!= 19)goto _LL3E7;_tmp81B=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp814)->f1;_LL3E6: _tmp81C=_tmp81B;goto _LL3E8;_LL3E7: if(*((int*)_tmp814)!= 5)
goto _LL3E9;_tmp81C=((struct Cyc_Absyn_Increment_e_struct*)_tmp814)->f1;_LL3E8: Cyc_Toc_exptypes_to_c(
_tmp81C);goto _LL3D8;_LL3E9: if(*((int*)_tmp814)!= 3)goto _LL3EB;_tmp81D=((struct
Cyc_Absyn_Primop_e_struct*)_tmp814)->f2;_LL3EA:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp81D);goto _LL3D8;
_LL3EB: if(*((int*)_tmp814)!= 7)goto _LL3ED;_tmp81E=((struct Cyc_Absyn_And_e_struct*)
_tmp814)->f1;_tmp81F=((struct Cyc_Absyn_And_e_struct*)_tmp814)->f2;_LL3EC: _tmp820=
_tmp81E;_tmp821=_tmp81F;goto _LL3EE;_LL3ED: if(*((int*)_tmp814)!= 8)goto _LL3EF;
_tmp820=((struct Cyc_Absyn_Or_e_struct*)_tmp814)->f1;_tmp821=((struct Cyc_Absyn_Or_e_struct*)
_tmp814)->f2;_LL3EE: _tmp822=_tmp820;_tmp823=_tmp821;goto _LL3F0;_LL3EF: if(*((int*)
_tmp814)!= 9)goto _LL3F1;_tmp822=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp814)->f1;
_tmp823=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp814)->f2;_LL3F0: _tmp824=_tmp822;
_tmp825=_tmp823;goto _LL3F2;_LL3F1: if(*((int*)_tmp814)!= 25)goto _LL3F3;_tmp824=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp814)->f1;_tmp825=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp814)->f2;_LL3F2: _tmp826=_tmp824;_tmp827=_tmp825;goto _LL3F4;_LL3F3: if(*((int*)
_tmp814)!= 36)goto _LL3F5;_tmp826=((struct Cyc_Absyn_Swap_e_struct*)_tmp814)->f1;
_tmp827=((struct Cyc_Absyn_Swap_e_struct*)_tmp814)->f2;_LL3F4: _tmp828=_tmp826;
_tmp829=_tmp827;goto _LL3F6;_LL3F5: if(*((int*)_tmp814)!= 4)goto _LL3F7;_tmp828=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp814)->f1;_tmp829=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp814)->f3;_LL3F6: Cyc_Toc_exptypes_to_c(_tmp828);Cyc_Toc_exptypes_to_c(_tmp829);
goto _LL3D8;_LL3F7: if(*((int*)_tmp814)!= 6)goto _LL3F9;_tmp82A=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp814)->f1;_tmp82B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp814)->f2;
_tmp82C=((struct Cyc_Absyn_Conditional_e_struct*)_tmp814)->f3;_LL3F8: Cyc_Toc_exptypes_to_c(
_tmp82A);Cyc_Toc_exptypes_to_c(_tmp82B);Cyc_Toc_exptypes_to_c(_tmp82C);goto
_LL3D8;_LL3F9: if(*((int*)_tmp814)!= 11)goto _LL3FB;_tmp82D=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp814)->f1;_tmp82E=((struct Cyc_Absyn_FnCall_e_struct*)_tmp814)->f2;_LL3FA:
_tmp82F=_tmp82D;_tmp830=_tmp82E;goto _LL3FC;_LL3FB: if(*((int*)_tmp814)!= 10)goto
_LL3FD;_tmp82F=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp814)->f1;_tmp830=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp814)->f2;_LL3FC: Cyc_Toc_exptypes_to_c(
_tmp82F);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp830);goto _LL3D8;_LL3FD: if(*((int*)_tmp814)!= 15)goto
_LL3FF;_tmp831=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp814)->f1;_tmp832=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp814)->f1);_tmp833=((struct
Cyc_Absyn_Cast_e_struct*)_tmp814)->f2;_LL3FE:*_tmp832=Cyc_Toc_typ_to_c(*_tmp832);
Cyc_Toc_exptypes_to_c(_tmp833);goto _LL3D8;_LL3FF: if(*((int*)_tmp814)!= 27)goto
_LL401;_tmp834=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp814)->f1;_tmp835=*
_tmp834;_tmp836=_tmp835.f3;_tmp837=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp814)->f1).f3;_tmp838=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp814)->f2;
_LL400:*_tmp837=Cyc_Toc_typ_to_c(*_tmp837);_tmp839=_tmp838;goto _LL402;_LL401: if(*((
int*)_tmp814)!= 37)goto _LL403;_tmp839=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp814)->f2;_LL402: _tmp83A=_tmp839;goto _LL404;_LL403: if(*((int*)_tmp814)!= 28)
goto _LL405;_tmp83A=((struct Cyc_Absyn_Array_e_struct*)_tmp814)->f1;_LL404: for(0;
_tmp83A != 0;_tmp83A=_tmp83A->tl){struct Cyc_Absyn_Exp*_tmp843;struct _tuple16
_tmp842=*((struct _tuple16*)_tmp83A->hd);_tmp843=_tmp842.f2;Cyc_Toc_exptypes_to_c(
_tmp843);}goto _LL3D8;_LL405: if(*((int*)_tmp814)!= 20)goto _LL407;_tmp83B=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp814)->f1;_tmp83C=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp814)->f1);_LL406: _tmp83E=_tmp83C;goto
_LL408;_LL407: if(*((int*)_tmp814)!= 18)goto _LL409;_tmp83D=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp814)->f1;_tmp83E=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp814)->f1);_LL408:*_tmp83E=Cyc_Toc_typ_to_c(*_tmp83E);goto _LL3D8;_LL409: if(*((
int*)_tmp814)!= 38)goto _LL40B;_tmp83F=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp814)->f1;_LL40A: Cyc_Toc_stmttypes_to_c(_tmp83F);goto _LL3D8;_LL40B: if(*((int*)
_tmp814)!= 35)goto _LL40D;_tmp840=((struct Cyc_Absyn_Malloc_e_struct*)_tmp814)->f1;
_tmp841=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp814)->f1;
_LL40C: if(_tmp841->elt_type != 0)_tmp841->elt_type=({void**_tmp844=_cycalloc(
sizeof(*_tmp844));_tmp844[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp841->elt_type)));
_tmp844;});Cyc_Toc_exptypes_to_c(_tmp841->num_elts);goto _LL3D8;_LL40D: if(*((int*)
_tmp814)!= 0)goto _LL40F;_LL40E: goto _LL410;_LL40F: if(*((int*)_tmp814)!= 1)goto
_LL411;_LL410: goto _LL412;_LL411: if(*((int*)_tmp814)!= 2)goto _LL413;_LL412: goto
_LL414;_LL413: if(*((int*)_tmp814)!= 33)goto _LL415;_LL414: goto _LL416;_LL415: if(*((
int*)_tmp814)!= 34)goto _LL417;_LL416: goto _LL3D8;_LL417: if(*((int*)_tmp814)!= 31)
goto _LL419;_LL418: goto _LL41A;_LL419: if(*((int*)_tmp814)!= 32)goto _LL41B;_LL41A:
goto _LL41C;_LL41B: if(*((int*)_tmp814)!= 30)goto _LL41D;_LL41C: goto _LL41E;_LL41D:
if(*((int*)_tmp814)!= 29)goto _LL41F;_LL41E: goto _LL420;_LL41F: if(*((int*)_tmp814)
!= 26)goto _LL421;_LL420: goto _LL422;_LL421: if(*((int*)_tmp814)!= 14)goto _LL423;
_LL422: goto _LL424;_LL423: if(*((int*)_tmp814)!= 17)goto _LL425;_LL424: goto _LL426;
_LL425: if(*((int*)_tmp814)!= 40)goto _LL427;_LL426: goto _LL428;_LL427: if(*((int*)
_tmp814)!= 39)goto _LL429;_LL428: goto _LL42A;_LL429: if(*((int*)_tmp814)!= 21)goto
_LL3D8;_LL42A:({void*_tmp845=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp846="Cyclone expression within C code";
_tag_dyneither(_tmp846,sizeof(char),33);}),_tag_dyneither(_tmp845,sizeof(void*),
0));});goto _LL3D8;_LL3D8:;}static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*
d){void*_tmp847=d->r;struct Cyc_Absyn_Vardecl*_tmp848;struct Cyc_Absyn_Fndecl*
_tmp849;struct Cyc_Absyn_Aggrdecl*_tmp84A;struct Cyc_Absyn_Enumdecl*_tmp84B;struct
Cyc_Absyn_Typedefdecl*_tmp84C;_LL42C: if(_tmp847 <= (void*)2)goto _LL448;if(*((int*)
_tmp847)!= 0)goto _LL42E;_tmp848=((struct Cyc_Absyn_Var_d_struct*)_tmp847)->f1;
_LL42D: _tmp848->type=Cyc_Toc_typ_to_c(_tmp848->type);if(_tmp848->initializer != 0)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp848->initializer));
goto _LL42B;_LL42E: if(*((int*)_tmp847)!= 1)goto _LL430;_tmp849=((struct Cyc_Absyn_Fn_d_struct*)
_tmp847)->f1;_LL42F: _tmp849->ret_type=Cyc_Toc_typ_to_c(_tmp849->ret_type);{
struct Cyc_List_List*_tmp84D=_tmp849->args;for(0;_tmp84D != 0;_tmp84D=_tmp84D->tl){(*((
struct _tuple25*)_tmp84D->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple25*)_tmp84D->hd)).f3);}}
goto _LL42B;_LL430: if(*((int*)_tmp847)!= 6)goto _LL432;_tmp84A=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp847)->f1;_LL431: Cyc_Toc_aggrdecl_to_c(_tmp84A);goto _LL42B;_LL432: if(*((int*)
_tmp847)!= 8)goto _LL434;_tmp84B=((struct Cyc_Absyn_Enum_d_struct*)_tmp847)->f1;
_LL433: if(_tmp84B->fields != 0){struct Cyc_List_List*_tmp84E=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp84B->fields))->v;for(0;_tmp84E != 0;_tmp84E=
_tmp84E->tl){struct Cyc_Absyn_Enumfield*_tmp84F=(struct Cyc_Absyn_Enumfield*)
_tmp84E->hd;if(_tmp84F->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp84F->tag));}}goto _LL42B;_LL434: if(*((int*)_tmp847)!= 9)goto
_LL436;_tmp84C=((struct Cyc_Absyn_Typedef_d_struct*)_tmp847)->f1;_LL435: _tmp84C->defn=({
struct Cyc_Core_Opt*_tmp850=_cycalloc(sizeof(*_tmp850));_tmp850->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp84C->defn))->v);_tmp850;});goto
_LL42B;_LL436: if(*((int*)_tmp847)!= 2)goto _LL438;_LL437: goto _LL439;_LL438: if(*((
int*)_tmp847)!= 3)goto _LL43A;_LL439: goto _LL43B;_LL43A: if(*((int*)_tmp847)!= 7)
goto _LL43C;_LL43B: goto _LL43D;_LL43C: if(*((int*)_tmp847)!= 10)goto _LL43E;_LL43D:
goto _LL43F;_LL43E: if(*((int*)_tmp847)!= 11)goto _LL440;_LL43F: goto _LL441;_LL440:
if(*((int*)_tmp847)!= 12)goto _LL442;_LL441: goto _LL443;_LL442: if(*((int*)_tmp847)
!= 13)goto _LL444;_LL443: goto _LL445;_LL444: if(*((int*)_tmp847)!= 4)goto _LL446;
_LL445: goto _LL447;_LL446: if(*((int*)_tmp847)!= 5)goto _LL448;_LL447:({void*
_tmp851=0;Cyc_Tcutil_terr(d->loc,({const char*_tmp852="Cyclone declaration within C code";
_tag_dyneither(_tmp852,sizeof(char),34);}),_tag_dyneither(_tmp851,sizeof(void*),
0));});goto _LL42B;_LL448: if((int)_tmp847 != 0)goto _LL44A;_LL449: goto _LL44B;_LL44A:
if((int)_tmp847 != 1)goto _LL42B;_LL44B: goto _LL42B;_LL42B:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s){void*_tmp853=s->r;struct Cyc_Absyn_Exp*_tmp854;struct Cyc_Absyn_Stmt*
_tmp855;struct Cyc_Absyn_Stmt*_tmp856;struct Cyc_Absyn_Exp*_tmp857;struct Cyc_Absyn_Exp*
_tmp858;struct Cyc_Absyn_Stmt*_tmp859;struct Cyc_Absyn_Stmt*_tmp85A;struct _tuple9
_tmp85B;struct Cyc_Absyn_Exp*_tmp85C;struct Cyc_Absyn_Stmt*_tmp85D;struct Cyc_Absyn_Exp*
_tmp85E;struct _tuple9 _tmp85F;struct Cyc_Absyn_Exp*_tmp860;struct _tuple9 _tmp861;
struct Cyc_Absyn_Exp*_tmp862;struct Cyc_Absyn_Stmt*_tmp863;struct Cyc_Absyn_Exp*
_tmp864;struct Cyc_List_List*_tmp865;struct Cyc_Absyn_Decl*_tmp866;struct Cyc_Absyn_Stmt*
_tmp867;struct Cyc_Absyn_Stmt*_tmp868;struct _tuple9 _tmp869;struct Cyc_Absyn_Exp*
_tmp86A;_LL44D: if(_tmp853 <= (void*)1)goto _LL45F;if(*((int*)_tmp853)!= 0)goto
_LL44F;_tmp854=((struct Cyc_Absyn_Exp_s_struct*)_tmp853)->f1;_LL44E: Cyc_Toc_exptypes_to_c(
_tmp854);goto _LL44C;_LL44F: if(*((int*)_tmp853)!= 1)goto _LL451;_tmp855=((struct
Cyc_Absyn_Seq_s_struct*)_tmp853)->f1;_tmp856=((struct Cyc_Absyn_Seq_s_struct*)
_tmp853)->f2;_LL450: Cyc_Toc_stmttypes_to_c(_tmp855);Cyc_Toc_stmttypes_to_c(
_tmp856);goto _LL44C;_LL451: if(*((int*)_tmp853)!= 2)goto _LL453;_tmp857=((struct
Cyc_Absyn_Return_s_struct*)_tmp853)->f1;_LL452: if(_tmp857 != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp857);goto _LL44C;_LL453: if(*((int*)_tmp853)!= 3)goto
_LL455;_tmp858=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp853)->f1;_tmp859=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp853)->f2;_tmp85A=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp853)->f3;_LL454: Cyc_Toc_exptypes_to_c(_tmp858);Cyc_Toc_stmttypes_to_c(
_tmp859);Cyc_Toc_stmttypes_to_c(_tmp85A);goto _LL44C;_LL455: if(*((int*)_tmp853)!= 
4)goto _LL457;_tmp85B=((struct Cyc_Absyn_While_s_struct*)_tmp853)->f1;_tmp85C=
_tmp85B.f1;_tmp85D=((struct Cyc_Absyn_While_s_struct*)_tmp853)->f2;_LL456: Cyc_Toc_exptypes_to_c(
_tmp85C);Cyc_Toc_stmttypes_to_c(_tmp85D);goto _LL44C;_LL457: if(*((int*)_tmp853)!= 
8)goto _LL459;_tmp85E=((struct Cyc_Absyn_For_s_struct*)_tmp853)->f1;_tmp85F=((
struct Cyc_Absyn_For_s_struct*)_tmp853)->f2;_tmp860=_tmp85F.f1;_tmp861=((struct
Cyc_Absyn_For_s_struct*)_tmp853)->f3;_tmp862=_tmp861.f1;_tmp863=((struct Cyc_Absyn_For_s_struct*)
_tmp853)->f4;_LL458: Cyc_Toc_exptypes_to_c(_tmp85E);Cyc_Toc_exptypes_to_c(_tmp860);
Cyc_Toc_exptypes_to_c(_tmp862);Cyc_Toc_stmttypes_to_c(_tmp863);goto _LL44C;_LL459:
if(*((int*)_tmp853)!= 9)goto _LL45B;_tmp864=((struct Cyc_Absyn_Switch_s_struct*)
_tmp853)->f1;_tmp865=((struct Cyc_Absyn_Switch_s_struct*)_tmp853)->f2;_LL45A: Cyc_Toc_exptypes_to_c(
_tmp864);for(0;_tmp865 != 0;_tmp865=_tmp865->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp865->hd)->body);}goto _LL44C;_LL45B: if(*((int*)
_tmp853)!= 11)goto _LL45D;_tmp866=((struct Cyc_Absyn_Decl_s_struct*)_tmp853)->f1;
_tmp867=((struct Cyc_Absyn_Decl_s_struct*)_tmp853)->f2;_LL45C: Cyc_Toc_decltypes_to_c(
_tmp866);Cyc_Toc_stmttypes_to_c(_tmp867);goto _LL44C;_LL45D: if(*((int*)_tmp853)!= 
13)goto _LL45F;_tmp868=((struct Cyc_Absyn_Do_s_struct*)_tmp853)->f1;_tmp869=((
struct Cyc_Absyn_Do_s_struct*)_tmp853)->f2;_tmp86A=_tmp869.f1;_LL45E: Cyc_Toc_stmttypes_to_c(
_tmp868);Cyc_Toc_exptypes_to_c(_tmp86A);goto _LL44C;_LL45F: if((int)_tmp853 != 0)
goto _LL461;_LL460: goto _LL462;_LL461: if(_tmp853 <= (void*)1)goto _LL463;if(*((int*)
_tmp853)!= 5)goto _LL463;_LL462: goto _LL464;_LL463: if(_tmp853 <= (void*)1)goto
_LL465;if(*((int*)_tmp853)!= 6)goto _LL465;_LL464: goto _LL466;_LL465: if(_tmp853 <= (
void*)1)goto _LL467;if(*((int*)_tmp853)!= 7)goto _LL467;_LL466: goto _LL44C;_LL467:
if(_tmp853 <= (void*)1)goto _LL469;if(*((int*)_tmp853)!= 10)goto _LL469;_LL468: goto
_LL46A;_LL469: if(_tmp853 <= (void*)1)goto _LL46B;if(*((int*)_tmp853)!= 12)goto
_LL46B;_LL46A: goto _LL46C;_LL46B: if(_tmp853 <= (void*)1)goto _LL46D;if(*((int*)
_tmp853)!= 14)goto _LL46D;_LL46C: goto _LL46E;_LL46D: if(_tmp853 <= (void*)1)goto
_LL44C;if(*((int*)_tmp853)!= 15)goto _LL44C;_LL46E:({void*_tmp86B=0;Cyc_Tcutil_terr(
s->loc,({const char*_tmp86C="Cyclone statement in C code";_tag_dyneither(_tmp86C,
sizeof(char),28);}),_tag_dyneither(_tmp86B,sizeof(void*),0));});goto _LL44C;
_LL44C:;}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct
Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))({void*_tmp86D=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp86E="decls_to_c: not at toplevel!";
_tag_dyneither(_tmp86E,sizeof(char),29);}),_tag_dyneither(_tmp86D,sizeof(void*),
0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp86F=d->r;
struct Cyc_Absyn_Vardecl*_tmp870;struct Cyc_Absyn_Fndecl*_tmp871;struct Cyc_Absyn_Aggrdecl*
_tmp872;struct Cyc_Absyn_Datatypedecl*_tmp873;struct Cyc_Absyn_Enumdecl*_tmp874;
struct Cyc_Absyn_Typedefdecl*_tmp875;struct Cyc_List_List*_tmp876;struct Cyc_List_List*
_tmp877;struct Cyc_List_List*_tmp878;struct Cyc_List_List*_tmp879;_LL470: if(
_tmp86F <= (void*)2)goto _LL484;if(*((int*)_tmp86F)!= 0)goto _LL472;_tmp870=((
struct Cyc_Absyn_Var_d_struct*)_tmp86F)->f1;_LL471: {struct _tuple1*_tmp87A=
_tmp870->name;if(_tmp870->sc == (void*)4)_tmp87A=({struct _tuple1*_tmp87B=
_cycalloc(sizeof(*_tmp87B));_tmp87B->f1=Cyc_Absyn_Rel_n(0);_tmp87B->f2=(*_tmp87A).f2;
_tmp87B;});if(_tmp870->initializer != 0){if(cinclude)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp870->initializer));else{Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_check_null(_tmp870->initializer));}}nv=Cyc_Toc_add_varmap(
r,nv,_tmp870->name,Cyc_Absyn_varb_exp(_tmp87A,(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp87C=_cycalloc(sizeof(*_tmp87C));_tmp87C[0]=({struct Cyc_Absyn_Global_b_struct
_tmp87D;_tmp87D.tag=0;_tmp87D.f1=_tmp870;_tmp87D;});_tmp87C;}),0));_tmp870->name=
_tmp87A;_tmp870->sc=Cyc_Toc_scope_to_c(_tmp870->sc);_tmp870->type=Cyc_Toc_typ_to_c_array(
_tmp870->type);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp87E=_cycalloc(
sizeof(*_tmp87E));_tmp87E->hd=d;_tmp87E->tl=Cyc_Toc_result_decls;_tmp87E;});goto
_LL46F;}_LL472: if(*((int*)_tmp86F)!= 1)goto _LL474;_tmp871=((struct Cyc_Absyn_Fn_d_struct*)
_tmp86F)->f1;_LL473: {struct _tuple1*_tmp87F=_tmp871->name;if(_tmp871->sc == (void*)
4){_tmp87F=({struct _tuple1*_tmp880=_cycalloc(sizeof(*_tmp880));_tmp880->f1=Cyc_Absyn_Rel_n(
0);_tmp880->f2=(*_tmp87F).f2;_tmp880;});_tmp871->sc=(void*)2;}nv=Cyc_Toc_add_varmap(
r,nv,_tmp871->name,Cyc_Absyn_var_exp(_tmp87F,0));_tmp871->name=_tmp87F;Cyc_Toc_fndecl_to_c(
nv,_tmp871,cinclude);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp881=
_cycalloc(sizeof(*_tmp881));_tmp881->hd=d;_tmp881->tl=Cyc_Toc_result_decls;
_tmp881;});goto _LL46F;}_LL474: if(*((int*)_tmp86F)!= 2)goto _LL476;_LL475: goto
_LL477;_LL476: if(*((int*)_tmp86F)!= 3)goto _LL478;_LL477:({void*_tmp882=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp883="letdecl at toplevel";_tag_dyneither(_tmp883,sizeof(char),20);}),
_tag_dyneither(_tmp882,sizeof(void*),0));});_LL478: if(*((int*)_tmp86F)!= 4)goto
_LL47A;_LL479:({void*_tmp884=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp885="region decl at toplevel";
_tag_dyneither(_tmp885,sizeof(char),24);}),_tag_dyneither(_tmp884,sizeof(void*),
0));});_LL47A: if(*((int*)_tmp86F)!= 5)goto _LL47C;_LL47B:({void*_tmp886=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp887="alias decl at toplevel";_tag_dyneither(_tmp887,sizeof(char),23);}),
_tag_dyneither(_tmp886,sizeof(void*),0));});_LL47C: if(*((int*)_tmp86F)!= 6)goto
_LL47E;_tmp872=((struct Cyc_Absyn_Aggr_d_struct*)_tmp86F)->f1;_LL47D: Cyc_Toc_aggrdecl_to_c(
_tmp872);goto _LL46F;_LL47E: if(*((int*)_tmp86F)!= 7)goto _LL480;_tmp873=((struct
Cyc_Absyn_Datatype_d_struct*)_tmp86F)->f1;_LL47F: if(_tmp873->is_extensible)Cyc_Toc_xdatatypedecl_to_c(
_tmp873);else{Cyc_Toc_datatypedecl_to_c(_tmp873);}goto _LL46F;_LL480: if(*((int*)
_tmp86F)!= 8)goto _LL482;_tmp874=((struct Cyc_Absyn_Enum_d_struct*)_tmp86F)->f1;
_LL481: Cyc_Toc_enumdecl_to_c(nv,_tmp874);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp888=_cycalloc(sizeof(*_tmp888));_tmp888->hd=d;_tmp888->tl=Cyc_Toc_result_decls;
_tmp888;});goto _LL46F;_LL482: if(*((int*)_tmp86F)!= 9)goto _LL484;_tmp875=((struct
Cyc_Absyn_Typedef_d_struct*)_tmp86F)->f1;_LL483: _tmp875->name=_tmp875->name;
_tmp875->tvs=0;if(_tmp875->defn != 0)_tmp875->defn=({struct Cyc_Core_Opt*_tmp889=
_cycalloc(sizeof(*_tmp889));_tmp889->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp875->defn))->v);_tmp889;});else{void*_tmp88A=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp875->kind))->v;_LL491: if((int)
_tmp88A != 2)goto _LL493;_LL492: _tmp875->defn=({struct Cyc_Core_Opt*_tmp88B=
_cycalloc(sizeof(*_tmp88B));_tmp88B->v=(void*)Cyc_Absyn_void_star_typ();_tmp88B;});
goto _LL490;_LL493:;_LL494: _tmp875->defn=({struct Cyc_Core_Opt*_tmp88C=_cycalloc(
sizeof(*_tmp88C));_tmp88C->v=(void*)((void*)0);_tmp88C;});goto _LL490;_LL490:;}
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp88D=_cycalloc(sizeof(*_tmp88D));
_tmp88D->hd=d;_tmp88D->tl=Cyc_Toc_result_decls;_tmp88D;});goto _LL46F;_LL484: if((
int)_tmp86F != 0)goto _LL486;_LL485: goto _LL487;_LL486: if((int)_tmp86F != 1)goto
_LL488;_LL487: goto _LL46F;_LL488: if(_tmp86F <= (void*)2)goto _LL48A;if(*((int*)
_tmp86F)!= 10)goto _LL48A;_tmp876=((struct Cyc_Absyn_Namespace_d_struct*)_tmp86F)->f2;
_LL489: _tmp877=_tmp876;goto _LL48B;_LL48A: if(_tmp86F <= (void*)2)goto _LL48C;if(*((
int*)_tmp86F)!= 11)goto _LL48C;_tmp877=((struct Cyc_Absyn_Using_d_struct*)_tmp86F)->f2;
_LL48B: _tmp878=_tmp877;goto _LL48D;_LL48C: if(_tmp86F <= (void*)2)goto _LL48E;if(*((
int*)_tmp86F)!= 12)goto _LL48E;_tmp878=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp86F)->f1;_LL48D: nv=Cyc_Toc_decls_to_c(r,nv,_tmp878,top,cinclude);goto _LL46F;
_LL48E: if(_tmp86F <= (void*)2)goto _LL46F;if(*((int*)_tmp86F)!= 13)goto _LL46F;
_tmp879=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp86F)->f1;_LL48F: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp879,top,1);goto _LL46F;_LL46F:;}}return nv;}static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp88F;struct Cyc_Core_NewRegion _tmp88E=Cyc_Core_new_dynregion();
_tmp88F=_tmp88E.dynregion;{struct _DynRegionFrame _tmp890;struct _RegionHandle*d=
_open_dynregion(& _tmp890,_tmp88F);Cyc_Toc_toc_state=({struct Cyc_Toc_TocState*
_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891->dyn=(struct _DynRegionHandle*)
_tmp88F;_tmp891->tuple_types=(struct Cyc_List_List**)({struct Cyc_List_List**
_tmp896=_region_malloc(d,sizeof(*_tmp896));_tmp896[0]=0;_tmp896;});_tmp891->aggrs_so_far=(
struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*_tmp895=_region_malloc(d,sizeof(*
_tmp895));_tmp895[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmp895;});
_tmp891->datatypes_so_far=(struct Cyc_Set_Set**)({struct Cyc_Set_Set**_tmp894=
_region_malloc(d,sizeof(*_tmp894));_tmp894[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);
_tmp894;});_tmp891->xdatatypes_so_far=(struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*
_tmp893=_region_malloc(d,sizeof(*_tmp893));_tmp893[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp);_tmp893;});_tmp891->qvar_tags=(struct Cyc_Dict_Dict*)({
struct Cyc_Dict_Dict*_tmp892=_region_malloc(d,sizeof(*_tmp892));_tmp892[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct
_tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmp892;});_tmp891->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d);_tmp891;});Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=
0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=
0;Cyc_Toc_globals=_tag_dyneither(({struct _dyneither_ptr**_tmp897=_cycalloc(
sizeof(struct _dyneither_ptr*)* 38);_tmp897[0]=& Cyc_Toc__throw_str;_tmp897[1]=&
Cyc_Toc_setjmp_str;_tmp897[2]=& Cyc_Toc__push_handler_str;_tmp897[3]=& Cyc_Toc__pop_handler_str;
_tmp897[4]=& Cyc_Toc__exn_thrown_str;_tmp897[5]=& Cyc_Toc__npop_handler_str;
_tmp897[6]=& Cyc_Toc__check_null_str;_tmp897[7]=& Cyc_Toc__check_known_subscript_null_str;
_tmp897[8]=& Cyc_Toc__check_known_subscript_notnull_str;_tmp897[9]=& Cyc_Toc__check_dyneither_subscript_str;
_tmp897[10]=& Cyc_Toc__dyneither_ptr_str;_tmp897[11]=& Cyc_Toc__tag_dyneither_str;
_tmp897[12]=& Cyc_Toc__init_dyneither_ptr_str;_tmp897[13]=& Cyc_Toc__untag_dyneither_ptr_str;
_tmp897[14]=& Cyc_Toc__get_dyneither_size_str;_tmp897[15]=& Cyc_Toc__get_zero_arr_size_str;
_tmp897[16]=& Cyc_Toc__dyneither_ptr_plus_str;_tmp897[17]=& Cyc_Toc__zero_arr_plus_str;
_tmp897[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str;_tmp897[19]=& Cyc_Toc__zero_arr_inplace_plus_str;
_tmp897[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;_tmp897[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str;
_tmp897[22]=& Cyc_Toc__cycalloc_str;_tmp897[23]=& Cyc_Toc__cyccalloc_str;_tmp897[
24]=& Cyc_Toc__cycalloc_atomic_str;_tmp897[25]=& Cyc_Toc__cyccalloc_atomic_str;
_tmp897[26]=& Cyc_Toc__region_malloc_str;_tmp897[27]=& Cyc_Toc__region_calloc_str;
_tmp897[28]=& Cyc_Toc__check_times_str;_tmp897[29]=& Cyc_Toc__new_region_str;
_tmp897[30]=& Cyc_Toc__push_region_str;_tmp897[31]=& Cyc_Toc__pop_region_str;
_tmp897[32]=& Cyc_Toc__open_dynregion_str;_tmp897[33]=& Cyc_Toc__push_dynregion_str;
_tmp897[34]=& Cyc_Toc__pop_dynregion_str;_tmp897[35]=& Cyc_Toc__reset_region_str;
_tmp897[36]=& Cyc_Toc__throw_arraybounds_str;_tmp897[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str;
_tmp897;}),sizeof(struct _dyneither_ptr*),38);;_pop_dynregion(d);}}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{struct _RegionHandle _tmp898=
_new_region("start");struct _RegionHandle*start=& _tmp898;_push_region(start);Cyc_Toc_decls_to_c(
start,Cyc_Toc_empty_env(start),ds,1,0);{struct _DynRegionHandle*_tmp89A;struct Cyc_Toc_TocState
_tmp899=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp89A=
_tmp899.dyn;Cyc_Core_free_dynregion(_tmp89A);}{struct Cyc_List_List*_tmp89B=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);
_npop_handler(0);return _tmp89B;};_pop_region(start);}}
