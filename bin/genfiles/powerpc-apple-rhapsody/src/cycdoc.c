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
dynregion;};struct Cyc_Core_Opt{void*v;};struct _dyneither_ptr Cyc_Core_new_string(
unsigned int);struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct
Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[
8];struct Cyc_Core_Failure_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[
11];struct Cyc_Core_Impossible_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{char*tag;};extern char
Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct
Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
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
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);int puts(const char*);int
remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};void Cyc_file_close(struct Cyc___cycFILE*);extern char
Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_struct{char*tag;struct _dyneither_ptr
f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*
refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int
lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int
lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct
_dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct
_dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*tag;};
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*
tag;};struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*
y);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_strconcat_l(
struct Cyc_List_List*);struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,
struct _dyneither_ptr);struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int
ofs,unsigned long n);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct
_dyneither_ptr line;int line_no;int col;};void Cyc_Lineno_poss_of_abss(struct
_dyneither_ptr filename,struct Cyc_List_List*places);struct _dyneither_ptr Cyc_Filename_chop_extension(
struct _dyneither_ptr);extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{
char*tag;};void Cyc_Position_reset_position(struct _dyneither_ptr);struct Cyc_Position_Segment;
struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{int tag;};
struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long
rem;}Cyc_ldiv_t;void*abort();void exit(int);int system(const char*);extern int Cyc_PP_tex_output;
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct _dyneither_ptr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_struct{char*
tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_struct{
char*tag;};struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dyneither_ptr);};void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),
struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct _union_Nmspace_Rel_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct
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
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;
struct _tuple1*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};
struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct
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
f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
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
_tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct
_tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct
_union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum 
Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7
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
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct
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
struct Cyc_Absyn_Skip_s_struct{int tag;};struct Cyc_Absyn_Exp_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;
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
int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);enum 
Cyc_Storage_class{Cyc_Typedef_sc  = 0,Cyc_Extern_sc  = 1,Cyc_ExternC_sc  = 2,Cyc_Static_sc
 = 3,Cyc_Auto_sc  = 4,Cyc_Register_sc  = 5,Cyc_Abstract_sc  = 6};struct Cyc_Declaration_spec;
struct Cyc_Declarator;struct Cyc_Abstractdeclarator;struct _union_YYSTYPE_Int_tok{
int tag;struct _tuple6 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct
_union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct
_union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct
_union_YYSTYPE_QualId_tok{int tag;struct _tuple1*val;};struct _tuple10{struct Cyc_Position_Segment*
f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct
_union_YYSTYPE_YY1{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY2{int tag;
union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_Absyn_Exp*
val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct
_union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int
tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop
val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*val;};struct
_union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY10{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_Absyn_Pat*
val;};struct _tuple11{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY12{
int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*
val;};struct _tuple12{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct
_union_YYSTYPE_YY14{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY15{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY16{int tag;struct _tuple11*val;};
struct _union_YYSTYPE_YY17{int tag;struct Cyc_Absyn_Fndecl*val;};struct
_union_YYSTYPE_YY18{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY19{
int tag;struct Cyc_Declaration_spec*val;};struct _tuple13{struct Cyc_Declarator*f1;
struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY20{int tag;struct _tuple13*val;};
struct _union_YYSTYPE_YY21{int tag;struct Cyc_List_List*val;};struct
_union_YYSTYPE_YY22{int tag;enum Cyc_Storage_class val;};struct _union_YYSTYPE_YY23{
int tag;void*val;};struct _union_YYSTYPE_YY24{int tag;enum Cyc_Absyn_AggrKind val;};
struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct
_union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{
int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*
val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Declarator*val;};struct
_union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator*val;};struct
_union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope
val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};
struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple14{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
_union_YYSTYPE_YY35{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY36{int tag;
struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};
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
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_for_cycdoc;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);void Cyc_Lex_lex_init(int use_cyclone_keywords);struct Cyc_Position_Segment{int
start;int end;};struct Cyc_MatchDecl_struct{int tag;struct _dyneither_ptr f1;};struct
Cyc_Standalone_struct{int tag;struct _dyneither_ptr f1;};struct _tuple16{int f1;void*
f2;};struct _tuple16*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf);const int Cyc_lex_base[
15]=(const int[15]){0,- 4,0,- 3,1,2,3,0,4,6,7,- 1,8,9,- 2};const int Cyc_lex_backtrk[15]=(
const int[15]){- 1,- 1,3,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_default[
15]=(const int[15]){1,0,- 1,0,- 1,- 1,6,- 1,8,8,8,0,6,6,0};const int Cyc_lex_trans[266]=(
const int[266]){0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,
0,6,0,0,0,0,0,0,0,4,5,7,12,9,2,10,10,13,13,0,11,- 1,14,- 1,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,- 1,- 1,0,- 1,- 1,- 1,- 1};
const int Cyc_lex_check[266]=(const int[266]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,7,- 1,5,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,2,4,5,6,8,0,9,10,12,13,- 1,9,10,12,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,
- 1,6,8,- 1,9,10,12,13};int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*
lbuf);int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;
int base;int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=
lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(15,state)];if(base < 0)return(-
base)- 1;backtrk=Cyc_lex_backtrk[_check_known_subscript_notnull(15,state)];if(
backtrk >= 0){lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(-
state)- 1;else{c=256;}}else{c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,
sizeof(char),lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_lex_check[
_check_known_subscript_notnull(266,base + c)]== state)state=Cyc_lex_trans[
_check_known_subscript_notnull(266,base + c)];else{state=Cyc_lex_default[
_check_known_subscript_notnull(15,state)];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){struct Cyc_Lexing_Error_struct _tmpA5;const char*
_tmpA4;struct Cyc_Lexing_Error_struct*_tmpA3;(int)_throw((void*)((_tmpA3=
_cycalloc(sizeof(*_tmpA3)),((_tmpA3[0]=((_tmpA5.tag=Cyc_Lexing_Error,((_tmpA5.f1=((
_tmpA4="empty token",_tag_dyneither(_tmpA4,sizeof(char),12))),_tmpA5)))),_tmpA3)))));}
else{return lbuf->lex_last_action;}}else{if(c == 256)lbuf->lex_eof_reached=0;}}}
struct _tuple16*Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);struct
_tuple16*Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL0: {int _tmp3=lexbuf->lex_start_pos + 5;
int _tmp4=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 7;struct Cyc_Standalone_struct*
_tmpAB;struct Cyc_Standalone_struct _tmpAA;struct _tuple16*_tmpA9;return(_tmpA9=
_cycalloc(sizeof(*_tmpA9)),((_tmpA9->f1=Cyc_Lexing_lexeme_start(lexbuf),((_tmpA9->f2=(
void*)((_tmpAB=_cycalloc(sizeof(*_tmpAB)),((_tmpAB[0]=((_tmpAA.tag=1,((_tmpAA.f1=
Cyc_substring((struct _dyneither_ptr)lexbuf->lex_buffer,_tmp3,(unsigned long)
_tmp4),_tmpAA)))),_tmpAB)))),_tmpA9)))));}case 1: _LL1: {int _tmp8=lexbuf->lex_start_pos
+ 4;int _tmp9=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 6;struct Cyc_MatchDecl_struct*
_tmpB1;struct Cyc_MatchDecl_struct _tmpB0;struct _tuple16*_tmpAF;return(_tmpAF=
_cycalloc(sizeof(*_tmpAF)),((_tmpAF->f1=Cyc_Lexing_lexeme_start(lexbuf),((_tmpAF->f2=(
void*)((_tmpB1=_cycalloc(sizeof(*_tmpB1)),((_tmpB1[0]=((_tmpB0.tag=0,((_tmpB0.f1=
Cyc_substring((struct _dyneither_ptr)lexbuf->lex_buffer,_tmp8,(unsigned long)
_tmp9),_tmpB0)))),_tmpB1)))),_tmpAF)))));}case 2: _LL2: return 0;case 3: _LL3: return
Cyc_token(lexbuf);default: _LL4:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmpB7;const char*_tmpB6;struct
Cyc_Lexing_Error_struct*_tmpB5;(int)_throw((void*)((_tmpB5=_cycalloc(sizeof(*
_tmpB5)),((_tmpB5[0]=((_tmpB7.tag=Cyc_Lexing_Error,((_tmpB7.f1=((_tmpB6="some action didn't return!",
_tag_dyneither(_tmpB6,sizeof(char),27))),_tmpB7)))),_tmpB5)))));};}struct
_tuple16*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple16*Cyc_token(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,0);}struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_struct{
char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};static void Cyc_dump_begin();
static void Cyc_dump_begin(){puts((const char*)"%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}");}
static void Cyc_dump_middle();static void Cyc_dump_middle(){puts((const char*)"\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}");}
static void Cyc_dump_end();static void Cyc_dump_end(){puts((const char*)"%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}");}
static void Cyc_pr_comment(struct Cyc___cycFILE*outf,struct _dyneither_ptr s);static
void Cyc_pr_comment(struct Cyc___cycFILE*outf,struct _dyneither_ptr s){int depth=0;
int len=(int)Cyc_strlen((struct _dyneither_ptr)s);int i=0;for(0;i < len;++ i){char c=*((
const char*)_check_dyneither_subscript(s,sizeof(char),i));if(c != '['){Cyc_fputc((
int)c,outf);continue;}Cyc_fputs((const char*)"\\texttt{",outf);++ i;++ depth;for(0;
i < len;++ i){char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(c == ']'){-- depth;if(depth == 0){Cyc_fputc((int)'}',outf);break;}}else{if(c == '[')
++ depth;}Cyc_fputc((int)c,outf);}}}static int Cyc_width=50;static void Cyc_set_width(
int w);static void Cyc_set_width(int w){Cyc_width=w;}static struct Cyc_List_List*Cyc_cycdoc_files=
0;static void Cyc_add_other(struct _dyneither_ptr s);static void Cyc_add_other(struct
_dyneither_ptr s){struct _dyneither_ptr*_tmpBA;struct Cyc_List_List*_tmpB9;Cyc_cycdoc_files=((
_tmpB9=_cycalloc(sizeof(*_tmpB9)),((_tmpB9->hd=((_tmpBA=_cycalloc(sizeof(*_tmpBA)),((
_tmpBA[0]=s,_tmpBA)))),((_tmpB9->tl=Cyc_cycdoc_files,_tmpB9))))));}static struct
Cyc_List_List*Cyc_cycargs=0;static void Cyc_add_cycarg(struct _dyneither_ptr s);
static void Cyc_add_cycarg(struct _dyneither_ptr s){struct _dyneither_ptr*_tmpBD;
struct Cyc_List_List*_tmpBC;Cyc_cycargs=((_tmpBC=_cycalloc(sizeof(*_tmpBC)),((
_tmpBC->hd=((_tmpBD=_cycalloc(sizeof(*_tmpBD)),((_tmpBD[0]=s,_tmpBD)))),((_tmpBC->tl=
Cyc_cycargs,_tmpBC))))));}static char _tmp14[8]="cyclone";static struct
_dyneither_ptr Cyc_cyclone_file={_tmp14,_tmp14,_tmp14 + 8};static void Cyc_set_cyclone_file(
struct _dyneither_ptr s);static void Cyc_set_cyclone_file(struct _dyneither_ptr s){Cyc_cyclone_file=
s;}static void Cyc_dumpdecl(struct Cyc_Absyn_Decl*d,struct _dyneither_ptr comment);
static void Cyc_dumpdecl(struct Cyc_Absyn_Decl*d,struct _dyneither_ptr comment){Cyc_dump_begin();{
const char*_tmpC1;void*_tmpC0[1];struct Cyc_String_pa_struct _tmpBF;(_tmpBF.tag=0,((
_tmpBF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(Cyc_Absynpp_decl2doc(
d),50)),((_tmpC0[0]=& _tmpBF,Cyc_printf(((_tmpC1="%s",_tag_dyneither(_tmpC1,
sizeof(char),3))),_tag_dyneither(_tmpC0,sizeof(void*),1)))))));}Cyc_dump_middle();
Cyc_pr_comment(Cyc_stdout,comment);{const char*_tmpC4;void*_tmpC3;(_tmpC3=0,Cyc_printf(((
_tmpC4="\n",_tag_dyneither(_tmpC4,sizeof(char),2))),_tag_dyneither(_tmpC3,
sizeof(void*),0)));}Cyc_dump_end();}static int Cyc_is_other_special(char c);static
int Cyc_is_other_special(char c){switch(c){case '\\': _LL6: goto _LL7;case '"': _LL7:
goto _LL8;case ';': _LL8: goto _LL9;case '&': _LL9: goto _LLA;case '(': _LLA: goto _LLB;case ')':
_LLB: goto _LLC;case '|': _LLC: goto _LLD;case '^': _LLD: goto _LLE;case '<': _LLE: goto _LLF;
case '>': _LLF: goto _LL10;case ' ': _LL10: goto _LL11;case '\n': _LL11: goto _LL12;case '\t':
_LL12: return 1;default: _LL13: return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(
struct _dyneither_ptr s);static void _tmpCD(unsigned int*_tmpCC,unsigned int*_tmpCB,
char**_tmpC9){for(*_tmpCC=0;*_tmpCC < *_tmpCB;(*_tmpCC)++){(*_tmpC9)[*_tmpCC]='\000';}}
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp1A=Cyc_strlen((struct _dyneither_ptr)s);int _tmp1B=0;int _tmp1C=0;{
int i=0;for(0;i < _tmp1A;++ i){char _tmp1D=*((const char*)_check_dyneither_subscript(
s,sizeof(char),i));if(_tmp1D == '\'')++ _tmp1B;else{if(Cyc_is_other_special(_tmp1D))
++ _tmp1C;}}}if(_tmp1B == 0  && _tmp1C == 0)return s;if(_tmp1B == 0){struct
_dyneither_ptr*_tmpC7;struct _dyneither_ptr*_tmpC6[3];return(struct _dyneither_ptr)
Cyc_strconcat_l(((_tmpC6[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct
_dyneither_ptr)),"'",sizeof(char),2),((_tmpC6[1]=((_tmpC7=_cycalloc(sizeof(*
_tmpC7)),((_tmpC7[0]=(struct _dyneither_ptr)s,_tmpC7)))),((_tmpC6[0]=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmpC6,sizeof(struct _dyneither_ptr*),3)))))))));}{unsigned long _tmp22=(_tmp1A + 
_tmp1B)+ _tmp1C;unsigned int _tmpCC;unsigned int _tmpCB;struct _dyneither_ptr _tmpCA;
char*_tmpC9;unsigned int _tmpC8;struct _dyneither_ptr s2=(_tmpC8=_tmp22 + 1,((_tmpC9=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpC8 + 1)),((_tmpCA=
_tag_dyneither(_tmpC9,sizeof(char),_tmpC8 + 1),((((_tmpCB=_tmpC8,((_tmpCD(& _tmpCC,&
_tmpCB,& _tmpC9),_tmpC9[_tmpCB]=(char)0)))),_tmpCA)))))));int _tmp23=0;int _tmp24=0;
for(0;_tmp23 < _tmp1A;++ _tmp23){char _tmp25=*((const char*)
_check_dyneither_subscript(s,sizeof(char),_tmp23));if(_tmp25 == '\''  || Cyc_is_other_special(
_tmp25)){char _tmpD0;char _tmpCF;struct _dyneither_ptr _tmpCE;(_tmpCE=
_dyneither_ptr_plus(s2,sizeof(char),_tmp24 ++),((_tmpCF=*((char*)
_check_dyneither_subscript(_tmpCE,sizeof(char),0)),((_tmpD0='\\',((
_get_dyneither_size(_tmpCE,sizeof(char))== 1  && (_tmpCF == '\000'  && _tmpD0 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpCE.curr)=_tmpD0)))))));}{char _tmpD3;char
_tmpD2;struct _dyneither_ptr _tmpD1;(_tmpD1=_dyneither_ptr_plus(s2,sizeof(char),
_tmp24 ++),((_tmpD2=*((char*)_check_dyneither_subscript(_tmpD1,sizeof(char),0)),((
_tmpD3=_tmp25,((_get_dyneither_size(_tmpD1,sizeof(char))== 1  && (_tmpD2 == '\000'
 && _tmpD3 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpD1.curr)=_tmpD3)))))));};}
return(struct _dyneither_ptr)s2;};}static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp);static struct _dyneither_ptr*Cyc_sh_escape_stringptr(
struct _dyneither_ptr*sp){struct _dyneither_ptr*_tmpD4;return(_tmpD4=_cycalloc(
sizeof(*_tmpD4)),((_tmpD4[0]=Cyc_sh_escape_string(*sp),_tmpD4)));}static struct
Cyc_Lineno_Pos*Cyc_new_pos();static struct Cyc_Lineno_Pos*Cyc_new_pos(){const char*
_tmpD7;struct Cyc_Lineno_Pos*_tmpD6;return(_tmpD6=_cycalloc(sizeof(*_tmpD6)),((
_tmpD6->logical_file=((_tmpD7="",_tag_dyneither(_tmpD7,sizeof(char),1))),((
_tmpD6->line=Cyc_Core_new_string(0),((_tmpD6->line_no=0,((_tmpD6->col=0,_tmpD6)))))))));}
struct _tuple17{int f1;struct Cyc_Lineno_Pos*f2;};static struct _tuple17*Cyc_start2pos(
int x);static struct _tuple17*Cyc_start2pos(int x){struct _tuple17*_tmpD8;return(
_tmpD8=_cycalloc(sizeof(*_tmpD8)),((_tmpD8->f1=x,((_tmpD8->f2=Cyc_new_pos(),
_tmpD8)))));}static int Cyc_decl2start(struct Cyc_Absyn_Decl*d);static int Cyc_decl2start(
struct Cyc_Absyn_Decl*d){return((struct Cyc_Position_Segment*)_check_null(d->loc))->start;}
struct _tuple18{struct Cyc_Lineno_Pos*f1;void*f2;};static struct Cyc_List_List*Cyc_this_file(
struct _dyneither_ptr file,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_this_file(
struct _dyneither_ptr file,struct Cyc_List_List*x){struct Cyc_List_List*_tmp34=0;
for(0;x != 0;x=x->tl){if(Cyc_strcmp((struct _dyneither_ptr)((*((struct _tuple18*)x->hd)).f1)->logical_file,(
struct _dyneither_ptr)file)== 0){struct Cyc_List_List*_tmpD9;_tmp34=((_tmpD9=
_cycalloc(sizeof(*_tmpD9)),((_tmpD9->hd=(struct _tuple18*)x->hd,((_tmpD9->tl=
_tmp34,_tmpD9))))));}}_tmp34=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp34);return _tmp34;}static int Cyc_lineno(struct Cyc_Lineno_Pos*
p);static int Cyc_lineno(struct Cyc_Lineno_Pos*p){return p->line_no;}static struct Cyc_List_List*
Cyc_flatten_decls(struct Cyc_List_List*decls);static struct Cyc_List_List*Cyc_flatten_decls(
struct Cyc_List_List*decls){struct Cyc_List_List*_tmp36=0;while(decls != 0){void*
_tmp37=((struct Cyc_Absyn_Decl*)decls->hd)->r;struct Cyc_List_List*_tmp39;struct
Cyc_List_List*_tmp3B;struct Cyc_List_List*_tmp3D;struct Cyc_List_List*_tmp3F;_LL16: {
struct Cyc_Absyn_Namespace_d_struct*_tmp38=(struct Cyc_Absyn_Namespace_d_struct*)
_tmp37;if(_tmp38->tag != 10)goto _LL18;else{_tmp39=_tmp38->f2;}}_LL17: _tmp3B=
_tmp39;goto _LL19;_LL18: {struct Cyc_Absyn_Using_d_struct*_tmp3A=(struct Cyc_Absyn_Using_d_struct*)
_tmp37;if(_tmp3A->tag != 11)goto _LL1A;else{_tmp3B=_tmp3A->f2;}}_LL19: _tmp3D=
_tmp3B;goto _LL1B;_LL1A: {struct Cyc_Absyn_ExternC_d_struct*_tmp3C=(struct Cyc_Absyn_ExternC_d_struct*)
_tmp37;if(_tmp3C->tag != 12)goto _LL1C;else{_tmp3D=_tmp3C->f1;}}_LL1B: _tmp3F=
_tmp3D;goto _LL1D;_LL1C: {struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3E=(struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp37;if(_tmp3E->tag != 13)goto _LL1E;else{
_tmp3F=_tmp3E->f1;}}_LL1D: decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp3F,decls->tl);goto _LL15;_LL1E: {
struct Cyc_Absyn_Aggr_d_struct*_tmp40=(struct Cyc_Absyn_Aggr_d_struct*)_tmp37;if(
_tmp40->tag != 6)goto _LL20;}_LL1F: goto _LL21;_LL20: {struct Cyc_Absyn_Var_d_struct*
_tmp41=(struct Cyc_Absyn_Var_d_struct*)_tmp37;if(_tmp41->tag != 0)goto _LL22;}_LL21:
goto _LL23;_LL22: {struct Cyc_Absyn_Datatype_d_struct*_tmp42=(struct Cyc_Absyn_Datatype_d_struct*)
_tmp37;if(_tmp42->tag != 7)goto _LL24;}_LL23: goto _LL25;_LL24: {struct Cyc_Absyn_Enum_d_struct*
_tmp43=(struct Cyc_Absyn_Enum_d_struct*)_tmp37;if(_tmp43->tag != 8)goto _LL26;}
_LL25: goto _LL27;_LL26: {struct Cyc_Absyn_Typedef_d_struct*_tmp44=(struct Cyc_Absyn_Typedef_d_struct*)
_tmp37;if(_tmp44->tag != 9)goto _LL28;}_LL27: goto _LL29;_LL28: {struct Cyc_Absyn_Fn_d_struct*
_tmp45=(struct Cyc_Absyn_Fn_d_struct*)_tmp37;if(_tmp45->tag != 1)goto _LL2A;}_LL29:
goto _LL2B;_LL2A: {struct Cyc_Absyn_Let_d_struct*_tmp46=(struct Cyc_Absyn_Let_d_struct*)
_tmp37;if(_tmp46->tag != 2)goto _LL2C;}_LL2B: goto _LL2D;_LL2C: {struct Cyc_Absyn_Letv_d_struct*
_tmp47=(struct Cyc_Absyn_Letv_d_struct*)_tmp37;if(_tmp47->tag != 3)goto _LL2E;}
_LL2D: goto _LL2F;_LL2E: {struct Cyc_Absyn_Porton_d_struct*_tmp48=(struct Cyc_Absyn_Porton_d_struct*)
_tmp37;if(_tmp48->tag != 14)goto _LL30;}_LL2F: goto _LL31;_LL30: {struct Cyc_Absyn_Portoff_d_struct*
_tmp49=(struct Cyc_Absyn_Portoff_d_struct*)_tmp37;if(_tmp49->tag != 15)goto _LL32;}
_LL31: goto _LL33;_LL32: {struct Cyc_Absyn_Region_d_struct*_tmp4A=(struct Cyc_Absyn_Region_d_struct*)
_tmp37;if(_tmp4A->tag != 4)goto _LL34;}_LL33: goto _LL35;_LL34: {struct Cyc_Absyn_Alias_d_struct*
_tmp4B=(struct Cyc_Absyn_Alias_d_struct*)_tmp37;if(_tmp4B->tag != 5)goto _LL15;}
_LL35:{struct Cyc_List_List*_tmpDA;_tmp36=((_tmpDA=_cycalloc(sizeof(*_tmpDA)),((
_tmpDA->hd=(struct Cyc_Absyn_Decl*)decls->hd,((_tmpDA->tl=_tmp36,_tmpDA))))));}
decls=decls->tl;goto _LL15;_LL15:;}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp36);}struct _tuple19{int f1;struct Cyc_Absyn_Decl*f2;};
struct _tuple20{struct Cyc_Lineno_Pos*f1;struct Cyc_Absyn_Decl*f2;};static void Cyc_process_file(
struct _dyneither_ptr filename);static void Cyc_process_file(struct _dyneither_ptr
filename){struct _dyneither_ptr _tmp4D=Cyc_Filename_chop_extension(filename);const
char*_tmpDB;const char*_tmp4E=(const char*)_untag_dyneither_ptr(Cyc_strconcat((
struct _dyneither_ptr)_tmp4D,((_tmpDB=".cyp",_tag_dyneither(_tmpDB,sizeof(char),5)))),
sizeof(char),1);const char*_tmpE2;struct _dyneither_ptr*_tmpE1;const char*_tmpE0;
struct Cyc_List_List*_tmpDF;struct _dyneither_ptr _tmp4F=Cyc_str_sepstr(((_tmpDF=
_cycalloc(sizeof(*_tmpDF)),((_tmpDF->hd=((_tmpE1=_cycalloc(sizeof(*_tmpE1)),((
_tmpE1[0]=(struct _dyneither_ptr)((_tmpE0="",_tag_dyneither(_tmpE0,sizeof(char),1))),
_tmpE1)))),((_tmpDF->tl=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(
struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycargs)),
_tmpDF)))))),((_tmpE2=" ",_tag_dyneither(_tmpE2,sizeof(char),2))));const char*
_tmpEB;const char*_tmpEA;void*_tmpE9[4];struct Cyc_String_pa_struct _tmpE8;struct
Cyc_String_pa_struct _tmpE7;struct Cyc_String_pa_struct _tmpE6;struct Cyc_String_pa_struct
_tmpE5;const char*_tmp50=(const char*)_untag_dyneither_ptr(((_tmpE5.tag=0,((_tmpE5.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename)),((
_tmpE6.tag=0,((_tmpE6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(((
_tmpEB=_tmp4E,_tag_dyneither(_tmpEB,sizeof(char),_get_zero_arr_size_char(_tmpEB,
1)))))),((_tmpE7.tag=0,((_tmpE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp4F),((_tmpE8.tag=0,((_tmpE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_cyclone_file),((_tmpE9[0]=& _tmpE8,((_tmpE9[1]=& _tmpE7,((_tmpE9[2]=& _tmpE6,((
_tmpE9[3]=& _tmpE5,Cyc_aprintf(((_tmpEA="%s %s -E -o %s -x cyc %s",_tag_dyneither(
_tmpEA,sizeof(char),25))),_tag_dyneither(_tmpE9,sizeof(void*),4)))))))))))))))))))))))))),
sizeof(char),1);if(system(_tmp50)!= 0){{const char*_tmpEE;void*_tmpED;(_tmpED=0,
Cyc_fprintf(Cyc_stderr,((_tmpEE="\nError: preprocessing\n",_tag_dyneither(_tmpEE,
sizeof(char),23))),_tag_dyneither(_tmpED,sizeof(void*),0)));}return;}{const char*
_tmpEF;Cyc_Position_reset_position(((_tmpEF=_tmp4E,_tag_dyneither(_tmpEF,sizeof(
char),_get_zero_arr_size_char(_tmpEF,1)))));}{struct Cyc___cycFILE*in_file=(
struct Cyc___cycFILE*)_check_null(Cyc_fopen(_tmp4E,(const char*)"r"));Cyc_Lex_lex_init(
1);{struct Cyc_List_List*_tmp54=Cyc_Parse_parse_file(in_file);Cyc_Lex_lex_init(1);
Cyc_file_close((struct Cyc___cycFILE*)in_file);_tmp54=Cyc_flatten_decls(_tmp54);{
struct Cyc_List_List*_tmp55=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(int),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((struct Cyc_List_List*(*)(int(*
f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_decl2start,
_tmp54));{const char*_tmpF0;Cyc_Lineno_poss_of_abss(((_tmpF0=_tmp4E,
_tag_dyneither(_tmpF0,sizeof(char),_get_zero_arr_size_char(_tmpF0,1)))),_tmp55);}
remove(_tmp4E);{struct Cyc_List_List*_tmp57=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple17*))Cyc_Core_snd,_tmp55),_tmp54);_tmp57=((struct Cyc_List_List*(*)(
struct _dyneither_ptr file,struct Cyc_List_List*x))Cyc_this_file)(filename,_tmp57);{
struct Cyc_List_List*_tmp58=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Lineno_Pos*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((struct Cyc_List_List*(*)(struct
Cyc_Lineno_Pos*(*f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Lineno_Pos*(*)(struct _tuple20*))Cyc_Core_fst,_tmp57)),((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(*f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Decl*(*)(struct _tuple20*))Cyc_Core_snd,_tmp57));struct Cyc___cycFILE*
_tmp59=(struct Cyc___cycFILE*)_check_null(Cyc_fopen((const char*)
_untag_dyneither_ptr(filename,sizeof(char),1),(const char*)"r"));struct Cyc_Lexing_lexbuf*
_tmp5A=Cyc_Lexing_from_file(_tmp59);struct Cyc_List_List*_tmp5B=0;struct _tuple16*
comment;while((comment=((struct _tuple16*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_token)(
_tmp5A))!= 0){struct Cyc_List_List*_tmpF1;_tmp5B=((_tmpF1=_cycalloc(sizeof(*
_tmpF1)),((_tmpF1->hd=(struct _tuple16*)_check_null(comment),((_tmpF1->tl=_tmp5B,
_tmpF1))))));}Cyc_fclose(_tmp59);_tmp5B=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp5B);{struct Cyc_List_List*_tmp5D=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(int),struct Cyc_List_List*x))Cyc_List_map)(Cyc_start2pos,((
struct Cyc_List_List*(*)(int(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((
int(*)(struct _tuple16*))Cyc_Core_fst,_tmp5B));Cyc_Lineno_poss_of_abss(filename,
_tmp5D);{struct Cyc_List_List*_tmp5E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*
f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(
struct _tuple17*))Cyc_Core_snd,_tmp5D),((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple16*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple16*))Cyc_Core_snd,
_tmp5B));_tmp5E=((struct Cyc_List_List*(*)(struct _dyneither_ptr file,struct Cyc_List_List*
x))Cyc_this_file)(filename,_tmp5E);{struct Cyc_List_List*_tmp5F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_List_List*(*)(
int(*f)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_lineno,((
struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*f)(struct _tuple18*),struct Cyc_List_List*
x))Cyc_List_map)((struct Cyc_Lineno_Pos*(*)(struct _tuple18*))Cyc_Core_fst,_tmp5E)),((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple18*))Cyc_Core_snd,_tmp5E));while(_tmp58 != 0  && _tmp5F != 0){
struct _tuple19 _tmp61;int _tmp62;struct Cyc_Absyn_Decl*_tmp63;struct _tuple19*_tmp60=(
struct _tuple19*)_tmp58->hd;_tmp61=*_tmp60;_tmp62=_tmp61.f1;_tmp63=_tmp61.f2;{
struct _tuple16 _tmp65;int _tmp66;void*_tmp67;struct _tuple16*_tmp64=(struct _tuple16*)
_tmp5F->hd;_tmp65=*_tmp64;_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;{void*_tmp68=_tmp67;
struct _dyneither_ptr _tmp6A;struct _dyneither_ptr _tmp6C;_LL37: {struct Cyc_Standalone_struct*
_tmp69=(struct Cyc_Standalone_struct*)_tmp68;if(_tmp69->tag != 1)goto _LL39;else{
_tmp6A=_tmp69->f1;}}_LL38: Cyc_pr_comment(Cyc_stdout,(struct _dyneither_ptr)_tmp6A);{
const char*_tmpF4;void*_tmpF3;(_tmpF3=0,Cyc_printf(((_tmpF4="\n",_tag_dyneither(
_tmpF4,sizeof(char),2))),_tag_dyneither(_tmpF3,sizeof(void*),0)));}_tmp5F=_tmp5F->tl;
goto _LL36;_LL39: {struct Cyc_MatchDecl_struct*_tmp6B=(struct Cyc_MatchDecl_struct*)
_tmp68;if(_tmp6B->tag != 0)goto _LL36;else{_tmp6C=_tmp6B->f1;}}_LL3A: if(_tmp66 < 
_tmp62){_tmp5F=_tmp5F->tl;continue;}if(_tmp58->tl != 0){struct _tuple19 _tmp70;int
_tmp71;struct _tuple19*_tmp6F=(struct _tuple19*)((struct Cyc_List_List*)_check_null(
_tmp58->tl))->hd;_tmp70=*_tmp6F;_tmp71=_tmp70.f1;if(_tmp71 < _tmp66){_tmp58=
_tmp58->tl;continue;}}Cyc_dumpdecl(_tmp63,(struct _dyneither_ptr)_tmp6C);Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stdout);_tmp58=_tmp58->tl;_tmp5F=_tmp5F->tl;goto _LL36;
_LL36:;};};}};};};};};};};};}void GC_blacklist_warn_clear();struct _tuple21{struct
_dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv);int Cyc_main(int argc,struct
_dyneither_ptr argv){GC_blacklist_warn_clear();{struct _tuple21*_tmp14F;const char*
_tmp14E;struct Cyc_Arg_Flag_spec_struct _tmp14D;struct Cyc_Arg_Flag_spec_struct*
_tmp14C;const char*_tmp14B;const char*_tmp14A;struct _tuple21*_tmp149;const char*
_tmp148;struct Cyc_Arg_Flag_spec_struct _tmp147;struct Cyc_Arg_Flag_spec_struct*
_tmp146;const char*_tmp145;const char*_tmp144;struct _tuple21*_tmp143;const char*
_tmp142;struct Cyc_Arg_Flag_spec_struct _tmp141;struct Cyc_Arg_Flag_spec_struct*
_tmp140;const char*_tmp13F;const char*_tmp13E;struct _tuple21*_tmp13D;const char*
_tmp13C;struct Cyc_Arg_Int_spec_struct _tmp13B;struct Cyc_Arg_Int_spec_struct*
_tmp13A;const char*_tmp139;const char*_tmp138;struct _tuple21*_tmp137;const char*
_tmp136;struct Cyc_Arg_String_spec_struct _tmp135;struct Cyc_Arg_String_spec_struct*
_tmp134;const char*_tmp133;const char*_tmp132;struct _tuple21*_tmp131[5];struct Cyc_List_List*
options=(_tmp131[4]=((_tmp14F=_cycalloc(sizeof(*_tmp14F)),((_tmp14F->f1=((
_tmp14A="-B",_tag_dyneither(_tmp14A,sizeof(char),3))),((_tmp14F->f2=1,((_tmp14F->f3=((
_tmp14B="<file>",_tag_dyneither(_tmp14B,sizeof(char),7))),((_tmp14F->f4=(void*)((
_tmp14C=_cycalloc(sizeof(*_tmp14C)),((_tmp14C[0]=((_tmp14D.tag=1,((_tmp14D.f1=
Cyc_add_cycarg,_tmp14D)))),_tmp14C)))),((_tmp14F->f5=((_tmp14E="Add to the list of directories to search for compiler files",
_tag_dyneither(_tmp14E,sizeof(char),60))),_tmp14F)))))))))))),((_tmp131[3]=((
_tmp149=_cycalloc(sizeof(*_tmp149)),((_tmp149->f1=((_tmp144="-I",_tag_dyneither(
_tmp144,sizeof(char),3))),((_tmp149->f2=1,((_tmp149->f3=((_tmp145="<dir>",
_tag_dyneither(_tmp145,sizeof(char),6))),((_tmp149->f4=(void*)((_tmp146=
_cycalloc(sizeof(*_tmp146)),((_tmp146[0]=((_tmp147.tag=1,((_tmp147.f1=Cyc_add_cycarg,
_tmp147)))),_tmp146)))),((_tmp149->f5=((_tmp148="Add to the list of directories to search for include files",
_tag_dyneither(_tmp148,sizeof(char),59))),_tmp149)))))))))))),((_tmp131[2]=((
_tmp143=_cycalloc(sizeof(*_tmp143)),((_tmp143->f1=((_tmp13E="-D",_tag_dyneither(
_tmp13E,sizeof(char),3))),((_tmp143->f2=1,((_tmp143->f3=((_tmp13F="<name>[=<value>]",
_tag_dyneither(_tmp13F,sizeof(char),17))),((_tmp143->f4=(void*)((_tmp140=
_cycalloc(sizeof(*_tmp140)),((_tmp140[0]=((_tmp141.tag=1,((_tmp141.f1=Cyc_add_cycarg,
_tmp141)))),_tmp140)))),((_tmp143->f5=((_tmp142="Pass definition to preprocessor",
_tag_dyneither(_tmp142,sizeof(char),32))),_tmp143)))))))))))),((_tmp131[1]=((
_tmp13D=_cycalloc(sizeof(*_tmp13D)),((_tmp13D->f1=((_tmp138="-w",_tag_dyneither(
_tmp138,sizeof(char),3))),((_tmp13D->f2=0,((_tmp13D->f3=((_tmp139=" <width>",
_tag_dyneither(_tmp139,sizeof(char),9))),((_tmp13D->f4=(void*)((_tmp13A=
_cycalloc(sizeof(*_tmp13A)),((_tmp13A[0]=((_tmp13B.tag=6,((_tmp13B.f1=Cyc_set_width,
_tmp13B)))),_tmp13A)))),((_tmp13D->f5=((_tmp13C="Use <width> as the max width for printing declarations",
_tag_dyneither(_tmp13C,sizeof(char),55))),_tmp13D)))))))))))),((_tmp131[0]=((
_tmp137=_cycalloc(sizeof(*_tmp137)),((_tmp137->f1=((_tmp132="-cyclone",
_tag_dyneither(_tmp132,sizeof(char),9))),((_tmp137->f2=0,((_tmp137->f3=((_tmp133=" <file>",
_tag_dyneither(_tmp133,sizeof(char),8))),((_tmp137->f4=(void*)((_tmp134=
_cycalloc(sizeof(*_tmp134)),((_tmp134[0]=((_tmp135.tag=5,((_tmp135.f1=Cyc_set_cyclone_file,
_tmp135)))),_tmp134)))),((_tmp137->f5=((_tmp136="Use <file> as the cyclone compiler",
_tag_dyneither(_tmp136,sizeof(char),35))),_tmp137)))))))))))),((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp131,sizeof(struct
_tuple21*),5)))))))))));{const char*_tmp150;Cyc_Arg_parse(options,Cyc_add_other,((
_tmp150="Options:",_tag_dyneither(_tmp150,sizeof(char),9))),argv);}if(Cyc_cycdoc_files
== 0){{const char*_tmp151;Cyc_Arg_usage(options,((_tmp151="Usage: cycdoc [options] file1 file2 ...\nOptions:",
_tag_dyneither(_tmp151,sizeof(char),49))));}exit(1);}Cyc_PP_tex_output=1;Cyc_Absynpp_set_params(&
Cyc_Absynpp_tc_params_r);Cyc_Absynpp_print_for_cycdoc=1;{struct Cyc_List_List*
_tmp80=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cycdoc_files);
for(0;_tmp80 != 0;_tmp80=_tmp80->tl){Cyc_process_file(*((struct _dyneither_ptr*)
_tmp80->hd));}}return 0;};}
