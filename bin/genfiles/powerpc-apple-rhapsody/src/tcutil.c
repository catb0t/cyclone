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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);int
Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{char*tag;};
extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{char*tag;};
struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);struct _dyneither_ptr Cyc_vrprintf(
struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
15];struct Cyc_FileCloseError_struct{char*tag;};extern char Cyc_FileOpenError[14];
struct Cyc_FileOpenError_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_copy(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(
struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_struct{char*tag;};struct
Cyc_Position_Segment;struct Cyc_List_List*Cyc_Position_strings_of_segments(struct
Cyc_List_List*);struct Cyc_Position_Lex_struct{int tag;};struct Cyc_Position_Parse_struct{
int tag;};struct Cyc_Position_Elab_struct{int tag;};struct Cyc_Position_Error{struct
_dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_struct{
char*tag;};extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct
_union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{
int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};
union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple2{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};enum Cyc_Absyn_Scope{Cyc_Absyn_Static
 = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC
 = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;
int q_restrict;int real_const;struct Cyc_Position_Segment*loc;};enum Cyc_Absyn_Size_of{
Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz
 = 3,Cyc_Absyn_LongLong_sz  = 4};enum Cyc_Absyn_Kind{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind
 = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_UniqueRgnKind  = 4,Cyc_Absyn_TopRgnKind
 = 5,Cyc_Absyn_EffKind  = 6,Cyc_Absyn_IntKind  = 7};enum Cyc_Absyn_Sign{Cyc_Absyn_Signed
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
int tag;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};extern
struct Cyc_Absyn_DynEither_b_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{
struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;
union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*
ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct
Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct
Cyc_Absyn_Thin_ptrqual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_struct{int tag;
};struct Cyc_Absyn_Zeroterm_ptrqual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_struct{
int tag;};struct Cyc_Absyn_Notnull_ptrqual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_struct{
int tag;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct
_union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo
val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**
val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{
struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);
struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;
struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct
_tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;
struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**
val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;
struct _union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU
Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union
Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_VoidType_struct{
int tag;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of
f2;};struct Cyc_Absyn_FloatType_struct{int tag;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple2*f1;
struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct
Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_struct{
int tag;};struct Cyc_Absyn_UniqueRgn_struct{int tag;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};extern struct Cyc_Absyn_HeapRgn_struct
Cyc_Absyn_HeapRgn_val;extern struct Cyc_Absyn_UniqueRgn_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_VoidType_struct Cyc_Absyn_VoidType_val;extern struct Cyc_Absyn_FloatType_struct
Cyc_Absyn_FloatType_val;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{int tag;
struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft
 = 1};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_struct{
int tag;};struct Cyc_Absyn_Cdecl_att_struct{int tag;};struct Cyc_Absyn_Fastcall_att_struct{
int tag;};struct Cyc_Absyn_Noreturn_att_struct{int tag;};struct Cyc_Absyn_Const_att_struct{
int tag;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_struct{
int tag;};struct Cyc_Absyn_Section_att_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Nocommon_att_struct{int tag;};struct Cyc_Absyn_Shared_att_struct{
int tag;};struct Cyc_Absyn_Unused_att_struct{int tag;};struct Cyc_Absyn_Weak_att_struct{
int tag;};struct Cyc_Absyn_Dllimport_att_struct{int tag;};struct Cyc_Absyn_Dllexport_att_struct{
int tag;};struct Cyc_Absyn_No_instrument_function_att_struct{int tag;};struct Cyc_Absyn_Constructor_att_struct{
int tag;};struct Cyc_Absyn_Destructor_att_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_struct{
int tag;};struct Cyc_Absyn_Format_att_struct{int tag;enum Cyc_Absyn_Format_Type f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_struct{
int tag;};struct Cyc_Absyn_Mode_att_struct{int tag;struct _dyneither_ptr f1;};struct
Cyc_Absyn_Carray_mod_struct{int tag;union Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*f1;union
Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*f3;};struct Cyc_Absyn_Pointer_mod_struct{
int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct
_union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};
struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign
f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{
enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};
struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{
int tag;struct _tuple8 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr
val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;enum 
Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div
 = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt
 = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 
12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift
 = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec
 = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct
Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion
 = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{
int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;
int fat_result;};struct Cyc_Absyn_Const_e_struct{int tag;union Cyc_Absyn_Cnst f1;};
struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple2*f1;};struct Cyc_Absyn_Primop_e_struct{int tag;enum Cyc_Absyn_Primop
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple9{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple9*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*
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
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;
};struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;
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
int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct
_tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Unresolved_b_struct{int tag;
};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{
int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;
struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int
is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;
void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{
struct _tuple2*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;enum 
Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct
_tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;
};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct
Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual
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
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_struct{
int tag;};struct Cyc_Absyn_Portoff_d_struct{int tag;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_struct{
char*tag;};int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_constr(
void*y,union Cyc_Absyn_Constraint*x);extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;void*Cyc_Absyn_compress_kb(
void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct
Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct
_dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,
struct _dyneither_ptr*);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};struct
_tuple11*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);struct
_dyneither_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(void*a);
struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};struct _tuple12 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(enum Cyc_Absyn_Kind);struct _dyneither_ptr
Cyc_Absynpp_kindbound2string(void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7];struct
Cyc_Set_Absent_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
8];struct Cyc_Dict_Present_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct
Cyc_Dict_Absent_struct{char*tag;};struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};struct
_RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct
_tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*
r1,void*r2);unsigned long Cyc_strlen(struct _dyneither_ptr s);int Cyc_strcmp(struct
_dyneither_ptr s1,struct _dyneither_ptr s2);int Cyc_strptrcmp(struct _dyneither_ptr*
s1,struct _dyneither_ptr*s2);struct _dyneither_ptr Cyc_strconcat(struct
_dyneither_ptr,struct _dyneither_ptr);struct _tuple13{unsigned int f1;int f2;};
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);int Cyc_Tcutil_kind_leq(enum Cyc_Absyn_Kind k1,enum Cyc_Absyn_Kind k2);enum Cyc_Absyn_Kind
Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,enum Cyc_Absyn_Kind def);enum Cyc_Absyn_Kind
Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_region(
void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;
extern struct Cyc_Core_Opt Cyc_Tcutil_urk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ek;extern struct Cyc_Core_Opt Cyc_Tcutil_ik;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(enum Cyc_Absyn_Kind k);void*Cyc_Tcutil_kind_to_bound(
enum Cyc_Absyn_Kind k);int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple14{
struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*
kb);int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple14*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple15{struct Cyc_List_List*
f1;struct _RegionHandle*f2;};struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct
_tuple15*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,enum Cyc_Absyn_Kind k,int
allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _dyneither_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,union Cyc_Absyn_Constraint*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,union Cyc_Absyn_Constraint*);int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(
void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,
int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*
e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple16{int f1;void*f2;
};struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Tcutil_bits_only(
void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);int Cyc_Tcutil_supports_default(void*);
int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct
Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);void
Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;enum Cyc_Cyclone_C_Compilers{
Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";
struct Cyc_Tcutil_Unify_struct{char*tag;};struct Cyc_Tcutil_Unify_struct Cyc_Tcutil_Unify_val={
Cyc_Tcutil_Unify};void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq1_const=0;void*Cyc_Tcutil_t2_failure=(
void*)& Cyc_Absyn_VoidType_val;int Cyc_Tcutil_tq2_const=0;struct _dyneither_ptr Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure();
void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors >= Cyc_Position_max_errors)
return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{const char*_tmpE64;const char*
_tmpE63;const char*_tmpE62;void*_tmpE61[2];struct Cyc_String_pa_struct _tmpE60;
struct Cyc_String_pa_struct _tmpE5F;struct _dyneither_ptr s1=(struct _dyneither_ptr)((
_tmpE5F.tag=0,((_tmpE5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure)),((_tmpE60.tag=0,((_tmpE60.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq1_const?(
_tmpE63="const ",_tag_dyneither(_tmpE63,sizeof(char),7)):((_tmpE64="",
_tag_dyneither(_tmpE64,sizeof(char),1)))),((_tmpE61[0]=& _tmpE60,((_tmpE61[1]=&
_tmpE5F,Cyc_aprintf(((_tmpE62="%s%s",_tag_dyneither(_tmpE62,sizeof(char),5))),
_tag_dyneither(_tmpE61,sizeof(void*),2))))))))))))));const char*_tmpE6D;const char*
_tmpE6C;const char*_tmpE6B;void*_tmpE6A[2];struct Cyc_String_pa_struct _tmpE69;
struct Cyc_String_pa_struct _tmpE68;struct _dyneither_ptr s2=(struct _dyneither_ptr)((
_tmpE68.tag=0,((_tmpE68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure)),((_tmpE69.tag=0,((_tmpE69.f1=(struct _dyneither_ptr)(Cyc_Tcutil_tq2_const?(
_tmpE6C="const ",_tag_dyneither(_tmpE6C,sizeof(char),7)):((_tmpE6D="",
_tag_dyneither(_tmpE6D,sizeof(char),1)))),((_tmpE6A[0]=& _tmpE69,((_tmpE6A[1]=&
_tmpE68,Cyc_aprintf(((_tmpE6B="%s%s",_tag_dyneither(_tmpE6B,sizeof(char),5))),
_tag_dyneither(_tmpE6A,sizeof(void*),2))))))))))))));int pos=2;{const char*_tmpE71;
void*_tmpE70[1];struct Cyc_String_pa_struct _tmpE6F;(_tmpE6F.tag=0,((_tmpE6F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpE70[0]=& _tmpE6F,Cyc_fprintf(
Cyc_stderr,((_tmpE71="  %s",_tag_dyneither(_tmpE71,sizeof(char),5))),
_tag_dyneither(_tmpE70,sizeof(void*),1)))))));}pos +=_get_dyneither_size(s1,
sizeof(char));if(pos + 5 >= 80){{const char*_tmpE74;void*_tmpE73;(_tmpE73=0,Cyc_fprintf(
Cyc_stderr,((_tmpE74="\n\t",_tag_dyneither(_tmpE74,sizeof(char),3))),
_tag_dyneither(_tmpE73,sizeof(void*),0)));}pos=8;}else{{const char*_tmpE77;void*
_tmpE76;(_tmpE76=0,Cyc_fprintf(Cyc_stderr,((_tmpE77=" ",_tag_dyneither(_tmpE77,
sizeof(char),2))),_tag_dyneither(_tmpE76,sizeof(void*),0)));}++ pos;}{const char*
_tmpE7A;void*_tmpE79;(_tmpE79=0,Cyc_fprintf(Cyc_stderr,((_tmpE7A="and ",
_tag_dyneither(_tmpE7A,sizeof(char),5))),_tag_dyneither(_tmpE79,sizeof(void*),0)));}
pos +=4;if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){{const char*_tmpE7D;
void*_tmpE7C;(_tmpE7C=0,Cyc_fprintf(Cyc_stderr,((_tmpE7D="\n\t",_tag_dyneither(
_tmpE7D,sizeof(char),3))),_tag_dyneither(_tmpE7C,sizeof(void*),0)));}pos=8;}{
const char*_tmpE81;void*_tmpE80[1];struct Cyc_String_pa_struct _tmpE7F;(_tmpE7F.tag=
0,((_tmpE7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpE80[0]=&
_tmpE7F,Cyc_fprintf(Cyc_stderr,((_tmpE81="%s ",_tag_dyneither(_tmpE81,sizeof(
char),4))),_tag_dyneither(_tmpE80,sizeof(void*),1)))))));}pos +=
_get_dyneither_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const char*_tmpE84;void*
_tmpE83;(_tmpE83=0,Cyc_fprintf(Cyc_stderr,((_tmpE84="\n\t",_tag_dyneither(
_tmpE84,sizeof(char),3))),_tag_dyneither(_tmpE83,sizeof(void*),0)));}pos=8;}{
const char*_tmpE87;void*_tmpE86;(_tmpE86=0,Cyc_fprintf(Cyc_stderr,((_tmpE87="are not compatible. ",
_tag_dyneither(_tmpE87,sizeof(char),21))),_tag_dyneither(_tmpE86,sizeof(void*),0)));}
pos +=17;if(Cyc_Tcutil_failure_reason.curr != (_tag_dyneither(0,0,0)).curr){if(pos
+ Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){const char*
_tmpE8A;void*_tmpE89;(_tmpE89=0,Cyc_fprintf(Cyc_stderr,((_tmpE8A="\n\t",
_tag_dyneither(_tmpE8A,sizeof(char),3))),_tag_dyneither(_tmpE89,sizeof(void*),0)));}{
const char*_tmpE8E;void*_tmpE8D[1];struct Cyc_String_pa_struct _tmpE8C;(_tmpE8C.tag=
0,((_tmpE8C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((
_tmpE8D[0]=& _tmpE8C,Cyc_fprintf(Cyc_stderr,((_tmpE8E="%s",_tag_dyneither(_tmpE8E,
sizeof(char),3))),_tag_dyneither(_tmpE8D,sizeof(void*),1)))))));};}{const char*
_tmpE91;void*_tmpE90;(_tmpE90=0,Cyc_fprintf(Cyc_stderr,((_tmpE91="\n",
_tag_dyneither(_tmpE91,sizeof(char),2))),_tag_dyneither(_tmpE90,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct
Cyc_Position_Segment*loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){struct
_dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{
const char*_tmpE95;void*_tmpE94[1];struct Cyc_String_pa_struct _tmpE93;(_tmpE93.tag=
0,((_tmpE93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpE94[0]=&
_tmpE93,Cyc_fprintf(Cyc_stderr,((_tmpE95="Compiler Error (Tcutil::impos): %s\n",
_tag_dyneither(_tmpE95,sizeof(char),36))),_tag_dyneither(_tmpE94,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{struct Cyc_Core_Impossible_struct
_tmpE98;struct Cyc_Core_Impossible_struct*_tmpE97;(int)_throw((void*)((_tmpE97=
_cycalloc(sizeof(*_tmpE97)),((_tmpE97[0]=((_tmpE98.tag=Cyc_Core_Impossible,((
_tmpE98.f1=msg,_tmpE98)))),_tmpE97)))));};}static struct _dyneither_ptr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv);static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct
Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs);void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpE9D;void*_tmpE9C[2];struct Cyc_String_pa_struct _tmpE9B;struct Cyc_String_pa_struct
_tmpE9A;(_tmpE9A.tag=0,((_tmpE9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpE9B.tag=
0,((_tmpE9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpE9C[0]=& _tmpE9B,((_tmpE9C[1]=& _tmpE9A,Cyc_fprintf(
Cyc_stderr,((_tmpE9D="%s::%s ",_tag_dyneither(_tmpE9D,sizeof(char),8))),
_tag_dyneither(_tmpE9C,sizeof(void*),2)))))))))))));}{const char*_tmpEA0;void*
_tmpE9F;(_tmpE9F=0,Cyc_fprintf(Cyc_stderr,((_tmpEA0="\n",_tag_dyneither(_tmpEA0,
sizeof(char),2))),_tag_dyneither(_tmpE9F,sizeof(void*),0)));}Cyc_fflush((struct
Cyc___cycFILE*)Cyc_stderr);}static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct
Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);{struct Cyc_List_List*_tmpEA1;Cyc_Tcutil_warning_segs=((_tmpEA1=_cycalloc(
sizeof(*_tmpEA1)),((_tmpEA1->hd=sg,((_tmpEA1->tl=Cyc_Tcutil_warning_segs,_tmpEA1))))));}{
struct _dyneither_ptr*_tmpEA4;struct Cyc_List_List*_tmpEA3;Cyc_Tcutil_warning_msgs=((
_tmpEA3=_cycalloc(sizeof(*_tmpEA3)),((_tmpEA3->hd=((_tmpEA4=_cycalloc(sizeof(*
_tmpEA4)),((_tmpEA4[0]=msg,_tmpEA4)))),((_tmpEA3->tl=Cyc_Tcutil_warning_msgs,
_tmpEA3))))));};}void Cyc_Tcutil_flush_warnings();void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)return;{const char*_tmpEA7;void*_tmpEA6;(_tmpEA6=0,
Cyc_fprintf(Cyc_stderr,((_tmpEA7="***Warnings***\n",_tag_dyneither(_tmpEA7,
sizeof(char),16))),_tag_dyneither(_tmpEA6,sizeof(void*),0)));}{struct Cyc_List_List*
_tmp36=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmpEAC;
void*_tmpEAB[2];struct Cyc_String_pa_struct _tmpEAA;struct Cyc_String_pa_struct
_tmpEA9;(_tmpEA9.tag=0,((_tmpEA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((
_tmpEAA.tag=0,((_tmpEAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((
struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp36))->hd)),((
_tmpEAB[0]=& _tmpEAA,((_tmpEAB[1]=& _tmpEA9,Cyc_fprintf(Cyc_stderr,((_tmpEAC="%s: %s\n",
_tag_dyneither(_tmpEAC,sizeof(char),8))),_tag_dyneither(_tmpEAB,sizeof(void*),2)))))))))))));}
_tmp36=_tmp36->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{
const char*_tmpEAF;void*_tmpEAE;(_tmpEAE=0,Cyc_fprintf(Cyc_stderr,((_tmpEAF="**************\n",
_tag_dyneither(_tmpEAF,sizeof(char),16))),_tag_dyneither(_tmpEAE,sizeof(void*),0)));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);};}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2);static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return tv1->identity - tv2->identity;}void*Cyc_Tcutil_compress(void*t);void*
Cyc_Tcutil_compress(void*t){void*_tmp3D=t;struct Cyc_Core_Opt*_tmp3F;void**_tmp41;
void**_tmp43;void***_tmp44;struct Cyc_Core_Opt*_tmp46;struct Cyc_Core_Opt**_tmp47;
struct Cyc_Absyn_Exp*_tmp49;_LL1: {struct Cyc_Absyn_Evar_struct*_tmp3E=(struct Cyc_Absyn_Evar_struct*)
_tmp3D;if(_tmp3E->tag != 1)goto _LL3;else{_tmp3F=_tmp3E->f2;if(_tmp3F != 0)goto _LL3;}}
_LL2: goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_struct*_tmp40=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp40->tag != 18)goto _LL5;else{_tmp41=_tmp40->f4;if(_tmp41 != 0)goto
_LL5;}}_LL4: return t;_LL5: {struct Cyc_Absyn_TypedefType_struct*_tmp42=(struct Cyc_Absyn_TypedefType_struct*)
_tmp3D;if(_tmp42->tag != 18)goto _LL7;else{_tmp43=_tmp42->f4;_tmp44=(void***)&
_tmp42->f4;}}_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp44)));
if(t2 != *((void**)_check_null(*_tmp44))){void**_tmpEB0;*_tmp44=((_tmpEB0=
_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0[0]=t2,_tmpEB0))));}return t2;}_LL7: {struct
Cyc_Absyn_Evar_struct*_tmp45=(struct Cyc_Absyn_Evar_struct*)_tmp3D;if(_tmp45->tag
!= 1)goto _LL9;else{_tmp46=_tmp45->f2;_tmp47=(struct Cyc_Core_Opt**)& _tmp45->f2;}}
_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp47))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp47))->v){
struct Cyc_Core_Opt*_tmpEB1;*_tmp47=((_tmpEB1=_cycalloc(sizeof(*_tmpEB1)),((
_tmpEB1->v=t2,_tmpEB1))));}return t2;}_LL9: {struct Cyc_Absyn_ValueofType_struct*
_tmp48=(struct Cyc_Absyn_ValueofType_struct*)_tmp3D;if(_tmp48->tag != 19)goto _LLB;
else{_tmp49=_tmp48->f1;}}_LLA: Cyc_Evexp_eval_const_uint_exp(_tmp49);{void*_tmp4C=
_tmp49->r;void*_tmp4E;_LLE: {struct Cyc_Absyn_Valueof_e_struct*_tmp4D=(struct Cyc_Absyn_Valueof_e_struct*)
_tmp4C;if(_tmp4D->tag != 40)goto _LL10;else{_tmp4E=(void*)_tmp4D->f1;}}_LLF: return
_tmp4E;_LL10:;_LL11: return t;_LLD:;};_LLB:;_LLC: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){return((
struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_type,ts);}static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr);static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(
union Cyc_Absyn_Constraint*cptr){union Cyc_Absyn_Constraint*_tmp4F=cptr;union Cyc_Absyn_Constraint
_tmp50;int _tmp51;union Cyc_Absyn_Constraint _tmp52;void*_tmp53;union Cyc_Absyn_Constraint
_tmp54;union Cyc_Absyn_Constraint*_tmp55;_LL13: _tmp50=*_tmp4F;if((_tmp50.No_constr).tag
!= 3)goto _LL15;_tmp51=(int)(_tmp50.No_constr).val;_LL14: return Cyc_Absyn_empty_conref();
_LL15: _tmp52=*_tmp4F;if((_tmp52.Eq_constr).tag != 1)goto _LL17;_tmp53=(void*)(
_tmp52.Eq_constr).val;_LL16: return Cyc_Absyn_new_conref(_tmp53);_LL17: _tmp54=*
_tmp4F;if((_tmp54.Forward_constr).tag != 2)goto _LL12;_tmp55=(union Cyc_Absyn_Constraint*)(
_tmp54.Forward_constr).val;_LL18: return Cyc_Tcutil_copy_conref(_tmp55);_LL12:;}
static void*Cyc_Tcutil_copy_kindbound(void*kb);static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp56=Cyc_Absyn_compress_kb(kb);enum Cyc_Absyn_Kind _tmp59;_LL1A: {
struct Cyc_Absyn_Unknown_kb_struct*_tmp57=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp56;if(_tmp57->tag != 1)goto _LL1C;}_LL1B: {struct Cyc_Absyn_Unknown_kb_struct
_tmpEB4;struct Cyc_Absyn_Unknown_kb_struct*_tmpEB3;return(void*)((_tmpEB3=
_cycalloc(sizeof(*_tmpEB3)),((_tmpEB3[0]=((_tmpEB4.tag=1,((_tmpEB4.f1=0,_tmpEB4)))),
_tmpEB3))));}_LL1C: {struct Cyc_Absyn_Less_kb_struct*_tmp58=(struct Cyc_Absyn_Less_kb_struct*)
_tmp56;if(_tmp58->tag != 2)goto _LL1E;else{_tmp59=_tmp58->f2;}}_LL1D: {struct Cyc_Absyn_Less_kb_struct
_tmpEB7;struct Cyc_Absyn_Less_kb_struct*_tmpEB6;return(void*)((_tmpEB6=_cycalloc(
sizeof(*_tmpEB6)),((_tmpEB6[0]=((_tmpEB7.tag=2,((_tmpEB7.f1=0,((_tmpEB7.f2=
_tmp59,_tmpEB7)))))),_tmpEB6))));}_LL1E:;_LL1F: return kb;_LL19:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmpEB8;return(_tmpEB8=_cycalloc(
sizeof(*_tmpEB8)),((_tmpEB8->name=tv->name,((_tmpEB8->identity=- 1,((_tmpEB8->kind=
Cyc_Tcutil_copy_kindbound(tv->kind),_tmpEB8)))))));}static struct _tuple9*Cyc_Tcutil_copy_arg(
struct _tuple9*arg);static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9 _tmp60;struct Cyc_Core_Opt*_tmp61;struct Cyc_Absyn_Tqual _tmp62;void*
_tmp63;struct _tuple9*_tmp5F=arg;_tmp60=*_tmp5F;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;
_tmp63=_tmp60.f3;{struct _tuple9*_tmpEB9;return(_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((
_tmpEB9->f1=_tmp61,((_tmpEB9->f2=_tmp62,((_tmpEB9->f3=Cyc_Tcutil_copy_type(
_tmp63),_tmpEB9)))))));};}static struct _tuple11*Cyc_Tcutil_copy_tqt(struct
_tuple11*arg);static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){
struct _tuple11 _tmp66;struct Cyc_Absyn_Tqual _tmp67;void*_tmp68;struct _tuple11*
_tmp65=arg;_tmp66=*_tmp65;_tmp67=_tmp66.f1;_tmp68=_tmp66.f2;{struct _tuple11*
_tmpEBA;return(_tmpEBA=_cycalloc(sizeof(*_tmpEBA)),((_tmpEBA->f1=_tmp67,((
_tmpEBA->f2=Cyc_Tcutil_copy_type(_tmp68),_tmpEBA)))));};}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmpEBB;return(_tmpEBB=_cycalloc(sizeof(*_tmpEBB)),((_tmpEBB->name=f->name,((
_tmpEBB->tq=f->tq,((_tmpEBB->type=Cyc_Tcutil_copy_type(f->type),((_tmpEBB->width=
f->width,((_tmpEBB->attributes=f->attributes,_tmpEBB)))))))))));}static struct
_tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x){struct _tuple0 _tmp6C;void*_tmp6D;void*_tmp6E;struct _tuple0*
_tmp6B=x;_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;{struct _tuple0*_tmpEBC;
return(_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC->f1=Cyc_Tcutil_copy_type(
_tmp6D),((_tmpEBC->f2=Cyc_Tcutil_copy_type(_tmp6E),_tmpEBC)))));};}static struct
Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f);
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*
f){struct Cyc_Absyn_Enumfield*_tmpEBD;return(_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((
_tmpEBD->name=f->name,((_tmpEBD->tag=f->tag,((_tmpEBD->loc=f->loc,_tmpEBD)))))));}
void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_copy_type(void*t){void*_tmp71=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp75;struct Cyc_Absyn_DatatypeInfo
_tmp77;union Cyc_Absyn_DatatypeInfoU _tmp78;struct Cyc_List_List*_tmp79;struct Cyc_Absyn_DatatypeFieldInfo
_tmp7B;union Cyc_Absyn_DatatypeFieldInfoU _tmp7C;struct Cyc_List_List*_tmp7D;struct
Cyc_Absyn_PtrInfo _tmp7F;void*_tmp80;struct Cyc_Absyn_Tqual _tmp81;struct Cyc_Absyn_PtrAtts
_tmp82;void*_tmp83;union Cyc_Absyn_Constraint*_tmp84;union Cyc_Absyn_Constraint*
_tmp85;union Cyc_Absyn_Constraint*_tmp86;struct Cyc_Absyn_PtrLoc*_tmp87;struct Cyc_Absyn_ArrayInfo
_tmp8C;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8E;struct Cyc_Absyn_Exp*_tmp8F;union
Cyc_Absyn_Constraint*_tmp90;struct Cyc_Position_Segment*_tmp91;struct Cyc_Absyn_FnInfo
_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_Core_Opt*_tmp95;void*_tmp96;struct
Cyc_List_List*_tmp97;int _tmp98;struct Cyc_Absyn_VarargInfo*_tmp99;struct Cyc_List_List*
_tmp9A;struct Cyc_List_List*_tmp9B;struct Cyc_List_List*_tmp9D;struct Cyc_Absyn_AggrInfo
_tmp9F;union Cyc_Absyn_AggrInfoU _tmpA0;struct _tuple4 _tmpA1;enum Cyc_Absyn_AggrKind
_tmpA2;struct _tuple2*_tmpA3;struct Cyc_Core_Opt*_tmpA4;struct Cyc_List_List*_tmpA5;
struct Cyc_Absyn_AggrInfo _tmpA7;union Cyc_Absyn_AggrInfoU _tmpA8;struct Cyc_Absyn_Aggrdecl**
_tmpA9;struct Cyc_List_List*_tmpAA;enum Cyc_Absyn_AggrKind _tmpAC;struct Cyc_List_List*
_tmpAD;struct _tuple2*_tmpAF;struct Cyc_Absyn_Enumdecl*_tmpB0;struct Cyc_List_List*
_tmpB2;void*_tmpB4;struct Cyc_Absyn_Exp*_tmpB6;void*_tmpB8;void*_tmpBA;void*
_tmpBB;struct _tuple2*_tmpBD;struct Cyc_List_List*_tmpBE;struct Cyc_Absyn_Typedefdecl*
_tmpBF;void*_tmpC3;struct Cyc_List_List*_tmpC5;void*_tmpC7;_LL21: {struct Cyc_Absyn_VoidType_struct*
_tmp72=(struct Cyc_Absyn_VoidType_struct*)_tmp71;if(_tmp72->tag != 0)goto _LL23;}
_LL22: goto _LL24;_LL23: {struct Cyc_Absyn_Evar_struct*_tmp73=(struct Cyc_Absyn_Evar_struct*)
_tmp71;if(_tmp73->tag != 1)goto _LL25;}_LL24: return t;_LL25: {struct Cyc_Absyn_VarType_struct*
_tmp74=(struct Cyc_Absyn_VarType_struct*)_tmp71;if(_tmp74->tag != 2)goto _LL27;
else{_tmp75=_tmp74->f1;}}_LL26: {struct Cyc_Absyn_VarType_struct _tmpEC0;struct Cyc_Absyn_VarType_struct*
_tmpEBF;return(void*)((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF[0]=((
_tmpEC0.tag=2,((_tmpEC0.f1=Cyc_Tcutil_copy_tvar(_tmp75),_tmpEC0)))),_tmpEBF))));}
_LL27: {struct Cyc_Absyn_DatatypeType_struct*_tmp76=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp71;if(_tmp76->tag != 3)goto _LL29;else{_tmp77=_tmp76->f1;_tmp78=_tmp77.datatype_info;
_tmp79=_tmp77.targs;}}_LL28: {struct Cyc_Absyn_DatatypeType_struct _tmpEC6;struct
Cyc_Absyn_DatatypeInfo _tmpEC5;struct Cyc_Absyn_DatatypeType_struct*_tmpEC4;return(
void*)((_tmpEC4=_cycalloc(sizeof(*_tmpEC4)),((_tmpEC4[0]=((_tmpEC6.tag=3,((
_tmpEC6.f1=((_tmpEC5.datatype_info=_tmp78,((_tmpEC5.targs=Cyc_Tcutil_copy_types(
_tmp79),_tmpEC5)))),_tmpEC6)))),_tmpEC4))));}_LL29: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp7A=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp71;if(_tmp7A->tag != 4)goto
_LL2B;else{_tmp7B=_tmp7A->f1;_tmp7C=_tmp7B.field_info;_tmp7D=_tmp7B.targs;}}
_LL2A: {struct Cyc_Absyn_DatatypeFieldType_struct _tmpECC;struct Cyc_Absyn_DatatypeFieldInfo
_tmpECB;struct Cyc_Absyn_DatatypeFieldType_struct*_tmpECA;return(void*)((_tmpECA=
_cycalloc(sizeof(*_tmpECA)),((_tmpECA[0]=((_tmpECC.tag=4,((_tmpECC.f1=((_tmpECB.field_info=
_tmp7C,((_tmpECB.targs=Cyc_Tcutil_copy_types(_tmp7D),_tmpECB)))),_tmpECC)))),
_tmpECA))));}_LL2B: {struct Cyc_Absyn_PointerType_struct*_tmp7E=(struct Cyc_Absyn_PointerType_struct*)
_tmp71;if(_tmp7E->tag != 5)goto _LL2D;else{_tmp7F=_tmp7E->f1;_tmp80=_tmp7F.elt_typ;
_tmp81=_tmp7F.elt_tq;_tmp82=_tmp7F.ptr_atts;_tmp83=_tmp82.rgn;_tmp84=_tmp82.nullable;
_tmp85=_tmp82.bounds;_tmp86=_tmp82.zero_term;_tmp87=_tmp82.ptrloc;}}_LL2C: {void*
_tmpD0=Cyc_Tcutil_copy_type(_tmp80);void*_tmpD1=Cyc_Tcutil_copy_type(_tmp83);
union Cyc_Absyn_Constraint*_tmpD2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp84);struct Cyc_Absyn_Tqual _tmpD3=_tmp81;union
Cyc_Absyn_Constraint*_tmpD4=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
cptr))Cyc_Tcutil_copy_conref)(_tmp85);union Cyc_Absyn_Constraint*_tmpD5=((union
Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(
_tmp86);struct Cyc_Absyn_PointerType_struct _tmpED6;struct Cyc_Absyn_PtrAtts _tmpED5;
struct Cyc_Absyn_PtrInfo _tmpED4;struct Cyc_Absyn_PointerType_struct*_tmpED3;return(
void*)((_tmpED3=_cycalloc(sizeof(*_tmpED3)),((_tmpED3[0]=((_tmpED6.tag=5,((
_tmpED6.f1=((_tmpED4.elt_typ=_tmpD0,((_tmpED4.elt_tq=_tmpD3,((_tmpED4.ptr_atts=((
_tmpED5.rgn=_tmpD1,((_tmpED5.nullable=_tmpD2,((_tmpED5.bounds=_tmpD4,((_tmpED5.zero_term=
_tmpD5,((_tmpED5.ptrloc=_tmp87,_tmpED5)))))))))),_tmpED4)))))),_tmpED6)))),
_tmpED3))));}_LL2D: {struct Cyc_Absyn_IntType_struct*_tmp88=(struct Cyc_Absyn_IntType_struct*)
_tmp71;if(_tmp88->tag != 6)goto _LL2F;}_LL2E: goto _LL30;_LL2F: {struct Cyc_Absyn_FloatType_struct*
_tmp89=(struct Cyc_Absyn_FloatType_struct*)_tmp71;if(_tmp89->tag != 7)goto _LL31;}
_LL30: goto _LL32;_LL31: {struct Cyc_Absyn_DoubleType_struct*_tmp8A=(struct Cyc_Absyn_DoubleType_struct*)
_tmp71;if(_tmp8A->tag != 8)goto _LL33;}_LL32: return t;_LL33: {struct Cyc_Absyn_ArrayType_struct*
_tmp8B=(struct Cyc_Absyn_ArrayType_struct*)_tmp71;if(_tmp8B->tag != 9)goto _LL35;
else{_tmp8C=_tmp8B->f1;_tmp8D=_tmp8C.elt_type;_tmp8E=_tmp8C.tq;_tmp8F=_tmp8C.num_elts;
_tmp90=_tmp8C.zero_term;_tmp91=_tmp8C.zt_loc;}}_LL34: {struct Cyc_Absyn_ArrayType_struct
_tmpEDC;struct Cyc_Absyn_ArrayInfo _tmpEDB;struct Cyc_Absyn_ArrayType_struct*
_tmpEDA;return(void*)((_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA[0]=((
_tmpEDC.tag=9,((_tmpEDC.f1=((_tmpEDB.elt_type=Cyc_Tcutil_copy_type(_tmp8D),((
_tmpEDB.tq=_tmp8E,((_tmpEDB.num_elts=_tmp8F,((_tmpEDB.zero_term=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp90),((_tmpEDB.zt_loc=
_tmp91,_tmpEDB)))))))))),_tmpEDC)))),_tmpEDA))));}_LL35: {struct Cyc_Absyn_FnType_struct*
_tmp92=(struct Cyc_Absyn_FnType_struct*)_tmp71;if(_tmp92->tag != 10)goto _LL37;
else{_tmp93=_tmp92->f1;_tmp94=_tmp93.tvars;_tmp95=_tmp93.effect;_tmp96=_tmp93.ret_typ;
_tmp97=_tmp93.args;_tmp98=_tmp93.c_varargs;_tmp99=_tmp93.cyc_varargs;_tmp9A=
_tmp93.rgn_po;_tmp9B=_tmp93.attributes;}}_LL36: {struct Cyc_List_List*_tmpDD=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp94);struct Cyc_Core_Opt*
_tmpEDD;struct Cyc_Core_Opt*_tmpDE=_tmp95 == 0?0:((_tmpEDD=_cycalloc(sizeof(*
_tmpEDD)),((_tmpEDD->v=Cyc_Tcutil_copy_type((void*)_tmp95->v),_tmpEDD))));void*
_tmpDF=Cyc_Tcutil_copy_type(_tmp96);struct Cyc_List_List*_tmpE0=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp97);int _tmpE1=_tmp98;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp99 != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_tmp99;struct Cyc_Absyn_VarargInfo*
_tmpEDE;cyc_varargs2=((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE->name=cv->name,((
_tmpEDE->tq=cv->tq,((_tmpEDE->type=Cyc_Tcutil_copy_type(cv->type),((_tmpEDE->inject=
cv->inject,_tmpEDE))))))))));}{struct Cyc_List_List*_tmpE3=((struct Cyc_List_List*(*)(
struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmp9A);struct Cyc_List_List*_tmpE4=_tmp9B;struct Cyc_Absyn_FnType_struct _tmpEE4;
struct Cyc_Absyn_FnInfo _tmpEE3;struct Cyc_Absyn_FnType_struct*_tmpEE2;return(void*)((
_tmpEE2=_cycalloc(sizeof(*_tmpEE2)),((_tmpEE2[0]=((_tmpEE4.tag=10,((_tmpEE4.f1=((
_tmpEE3.tvars=_tmpDD,((_tmpEE3.effect=_tmpDE,((_tmpEE3.ret_typ=_tmpDF,((_tmpEE3.args=
_tmpE0,((_tmpEE3.c_varargs=_tmpE1,((_tmpEE3.cyc_varargs=cyc_varargs2,((_tmpEE3.rgn_po=
_tmpE3,((_tmpEE3.attributes=_tmpE4,_tmpEE3)))))))))))))))),_tmpEE4)))),_tmpEE2))));};}
_LL37: {struct Cyc_Absyn_TupleType_struct*_tmp9C=(struct Cyc_Absyn_TupleType_struct*)
_tmp71;if(_tmp9C->tag != 11)goto _LL39;else{_tmp9D=_tmp9C->f1;}}_LL38: {struct Cyc_Absyn_TupleType_struct
_tmpEE7;struct Cyc_Absyn_TupleType_struct*_tmpEE6;return(void*)((_tmpEE6=
_cycalloc(sizeof(*_tmpEE6)),((_tmpEE6[0]=((_tmpEE7.tag=11,((_tmpEE7.f1=((struct
Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp9D),_tmpEE7)))),_tmpEE6))));}_LL39: {
struct Cyc_Absyn_AggrType_struct*_tmp9E=(struct Cyc_Absyn_AggrType_struct*)_tmp71;
if(_tmp9E->tag != 12)goto _LL3B;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9F.aggr_info;if((
_tmpA0.UnknownAggr).tag != 1)goto _LL3B;_tmpA1=(struct _tuple4)(_tmpA0.UnknownAggr).val;
_tmpA2=_tmpA1.f1;_tmpA3=_tmpA1.f2;_tmpA4=_tmpA1.f3;_tmpA5=_tmp9F.targs;}}_LL3A: {
struct Cyc_Absyn_AggrType_struct _tmpEED;struct Cyc_Absyn_AggrInfo _tmpEEC;struct Cyc_Absyn_AggrType_struct*
_tmpEEB;return(void*)((_tmpEEB=_cycalloc(sizeof(*_tmpEEB)),((_tmpEEB[0]=((
_tmpEED.tag=12,((_tmpEED.f1=((_tmpEEC.aggr_info=Cyc_Absyn_UnknownAggr(_tmpA2,
_tmpA3,_tmpA4),((_tmpEEC.targs=Cyc_Tcutil_copy_types(_tmpA5),_tmpEEC)))),_tmpEED)))),
_tmpEEB))));}_LL3B: {struct Cyc_Absyn_AggrType_struct*_tmpA6=(struct Cyc_Absyn_AggrType_struct*)
_tmp71;if(_tmpA6->tag != 12)goto _LL3D;else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA7.aggr_info;
if((_tmpA8.KnownAggr).tag != 2)goto _LL3D;_tmpA9=(struct Cyc_Absyn_Aggrdecl**)(
_tmpA8.KnownAggr).val;_tmpAA=_tmpA7.targs;}}_LL3C: {struct Cyc_Absyn_AggrType_struct
_tmpEF3;struct Cyc_Absyn_AggrInfo _tmpEF2;struct Cyc_Absyn_AggrType_struct*_tmpEF1;
return(void*)((_tmpEF1=_cycalloc(sizeof(*_tmpEF1)),((_tmpEF1[0]=((_tmpEF3.tag=12,((
_tmpEF3.f1=((_tmpEF2.aggr_info=Cyc_Absyn_KnownAggr(_tmpA9),((_tmpEF2.targs=Cyc_Tcutil_copy_types(
_tmpAA),_tmpEF2)))),_tmpEF3)))),_tmpEF1))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpAB=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp71;if(_tmpAB->tag != 13)goto
_LL3F;else{_tmpAC=_tmpAB->f1;_tmpAD=_tmpAB->f2;}}_LL3E: {struct Cyc_Absyn_AnonAggrType_struct
_tmpEF6;struct Cyc_Absyn_AnonAggrType_struct*_tmpEF5;return(void*)((_tmpEF5=
_cycalloc(sizeof(*_tmpEF5)),((_tmpEF5[0]=((_tmpEF6.tag=13,((_tmpEF6.f1=_tmpAC,((
_tmpEF6.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpAD),_tmpEF6)))))),
_tmpEF5))));}_LL3F: {struct Cyc_Absyn_EnumType_struct*_tmpAE=(struct Cyc_Absyn_EnumType_struct*)
_tmp71;if(_tmpAE->tag != 14)goto _LL41;else{_tmpAF=_tmpAE->f1;_tmpB0=_tmpAE->f2;}}
_LL40: {struct Cyc_Absyn_EnumType_struct _tmpEF9;struct Cyc_Absyn_EnumType_struct*
_tmpEF8;return(void*)((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8[0]=((
_tmpEF9.tag=14,((_tmpEF9.f1=_tmpAF,((_tmpEF9.f2=_tmpB0,_tmpEF9)))))),_tmpEF8))));}
_LL41: {struct Cyc_Absyn_AnonEnumType_struct*_tmpB1=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp71;if(_tmpB1->tag != 15)goto _LL43;else{_tmpB2=_tmpB1->f1;}}_LL42: {struct Cyc_Absyn_AnonEnumType_struct
_tmpEFC;struct Cyc_Absyn_AnonEnumType_struct*_tmpEFB;return(void*)((_tmpEFB=
_cycalloc(sizeof(*_tmpEFB)),((_tmpEFB[0]=((_tmpEFC.tag=15,((_tmpEFC.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpB2),_tmpEFC)))),
_tmpEFB))));}_LL43: {struct Cyc_Absyn_TagType_struct*_tmpB3=(struct Cyc_Absyn_TagType_struct*)
_tmp71;if(_tmpB3->tag != 20)goto _LL45;else{_tmpB4=(void*)_tmpB3->f1;}}_LL44: {
struct Cyc_Absyn_TagType_struct _tmpEFF;struct Cyc_Absyn_TagType_struct*_tmpEFE;
return(void*)((_tmpEFE=_cycalloc(sizeof(*_tmpEFE)),((_tmpEFE[0]=((_tmpEFF.tag=20,((
_tmpEFF.f1=(void*)Cyc_Tcutil_copy_type(_tmpB4),_tmpEFF)))),_tmpEFE))));}_LL45: {
struct Cyc_Absyn_ValueofType_struct*_tmpB5=(struct Cyc_Absyn_ValueofType_struct*)
_tmp71;if(_tmpB5->tag != 19)goto _LL47;else{_tmpB6=_tmpB5->f1;}}_LL46: {struct Cyc_Absyn_ValueofType_struct
_tmpF02;struct Cyc_Absyn_ValueofType_struct*_tmpF01;return(void*)((_tmpF01=
_cycalloc(sizeof(*_tmpF01)),((_tmpF01[0]=((_tmpF02.tag=19,((_tmpF02.f1=_tmpB6,
_tmpF02)))),_tmpF01))));}_LL47: {struct Cyc_Absyn_RgnHandleType_struct*_tmpB7=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp71;if(_tmpB7->tag != 16)goto _LL49;else{
_tmpB8=(void*)_tmpB7->f1;}}_LL48: {struct Cyc_Absyn_RgnHandleType_struct _tmpF05;
struct Cyc_Absyn_RgnHandleType_struct*_tmpF04;return(void*)((_tmpF04=_cycalloc(
sizeof(*_tmpF04)),((_tmpF04[0]=((_tmpF05.tag=16,((_tmpF05.f1=(void*)Cyc_Tcutil_copy_type(
_tmpB8),_tmpF05)))),_tmpF04))));}_LL49: {struct Cyc_Absyn_DynRgnType_struct*
_tmpB9=(struct Cyc_Absyn_DynRgnType_struct*)_tmp71;if(_tmpB9->tag != 17)goto _LL4B;
else{_tmpBA=(void*)_tmpB9->f1;_tmpBB=(void*)_tmpB9->f2;}}_LL4A: {struct Cyc_Absyn_DynRgnType_struct
_tmpF08;struct Cyc_Absyn_DynRgnType_struct*_tmpF07;return(void*)((_tmpF07=
_cycalloc(sizeof(*_tmpF07)),((_tmpF07[0]=((_tmpF08.tag=17,((_tmpF08.f1=(void*)
Cyc_Tcutil_copy_type(_tmpBA),((_tmpF08.f2=(void*)Cyc_Tcutil_copy_type(_tmpBB),
_tmpF08)))))),_tmpF07))));}_LL4B: {struct Cyc_Absyn_TypedefType_struct*_tmpBC=(
struct Cyc_Absyn_TypedefType_struct*)_tmp71;if(_tmpBC->tag != 18)goto _LL4D;else{
_tmpBD=_tmpBC->f1;_tmpBE=_tmpBC->f2;_tmpBF=_tmpBC->f3;}}_LL4C: {struct Cyc_Absyn_TypedefType_struct
_tmpF0B;struct Cyc_Absyn_TypedefType_struct*_tmpF0A;return(void*)((_tmpF0A=
_cycalloc(sizeof(*_tmpF0A)),((_tmpF0A[0]=((_tmpF0B.tag=18,((_tmpF0B.f1=_tmpBD,((
_tmpF0B.f2=Cyc_Tcutil_copy_types(_tmpBE),((_tmpF0B.f3=_tmpBF,((_tmpF0B.f4=0,
_tmpF0B)))))))))),_tmpF0A))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_struct*_tmpC0=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp71;if(_tmpC0->tag != 22)goto _LL4F;}_LL4E:
goto _LL50;_LL4F: {struct Cyc_Absyn_HeapRgn_struct*_tmpC1=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp71;if(_tmpC1->tag != 21)goto _LL51;}_LL50: return t;_LL51: {struct Cyc_Absyn_AccessEff_struct*
_tmpC2=(struct Cyc_Absyn_AccessEff_struct*)_tmp71;if(_tmpC2->tag != 23)goto _LL53;
else{_tmpC3=(void*)_tmpC2->f1;}}_LL52: {struct Cyc_Absyn_AccessEff_struct _tmpF0E;
struct Cyc_Absyn_AccessEff_struct*_tmpF0D;return(void*)((_tmpF0D=_cycalloc(
sizeof(*_tmpF0D)),((_tmpF0D[0]=((_tmpF0E.tag=23,((_tmpF0E.f1=(void*)Cyc_Tcutil_copy_type(
_tmpC3),_tmpF0E)))),_tmpF0D))));}_LL53: {struct Cyc_Absyn_JoinEff_struct*_tmpC4=(
struct Cyc_Absyn_JoinEff_struct*)_tmp71;if(_tmpC4->tag != 24)goto _LL55;else{_tmpC5=
_tmpC4->f1;}}_LL54: {struct Cyc_Absyn_JoinEff_struct _tmpF11;struct Cyc_Absyn_JoinEff_struct*
_tmpF10;return(void*)((_tmpF10=_cycalloc(sizeof(*_tmpF10)),((_tmpF10[0]=((
_tmpF11.tag=24,((_tmpF11.f1=Cyc_Tcutil_copy_types(_tmpC5),_tmpF11)))),_tmpF10))));}
_LL55: {struct Cyc_Absyn_RgnsEff_struct*_tmpC6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp71;if(_tmpC6->tag != 25)goto _LL20;else{_tmpC7=(void*)_tmpC6->f1;}}_LL56: {
struct Cyc_Absyn_RgnsEff_struct _tmpF14;struct Cyc_Absyn_RgnsEff_struct*_tmpF13;
return(void*)((_tmpF13=_cycalloc(sizeof(*_tmpF13)),((_tmpF13[0]=((_tmpF14.tag=25,((
_tmpF14.f1=(void*)Cyc_Tcutil_copy_type(_tmpC7),_tmpF14)))),_tmpF13))));}_LL20:;}
struct _tuple17{enum Cyc_Absyn_Kind f1;enum Cyc_Absyn_Kind f2;};int Cyc_Tcutil_kind_leq(
enum Cyc_Absyn_Kind k1,enum Cyc_Absyn_Kind k2);int Cyc_Tcutil_kind_leq(enum Cyc_Absyn_Kind
k1,enum Cyc_Absyn_Kind k2){if(k1 == k2)return 1;{struct _tuple17 _tmpF15;struct
_tuple17 _tmp108=(_tmpF15.f1=k1,((_tmpF15.f2=k2,_tmpF15)));enum Cyc_Absyn_Kind
_tmp109;enum Cyc_Absyn_Kind _tmp10A;enum Cyc_Absyn_Kind _tmp10B;enum Cyc_Absyn_Kind
_tmp10C;enum Cyc_Absyn_Kind _tmp10D;enum Cyc_Absyn_Kind _tmp10E;enum Cyc_Absyn_Kind
_tmp10F;enum Cyc_Absyn_Kind _tmp110;enum Cyc_Absyn_Kind _tmp111;enum Cyc_Absyn_Kind
_tmp112;_LL58: _tmp109=_tmp108.f1;if(_tmp109 != Cyc_Absyn_BoxKind)goto _LL5A;
_tmp10A=_tmp108.f2;if(_tmp10A != Cyc_Absyn_MemKind)goto _LL5A;_LL59: goto _LL5B;
_LL5A: _tmp10B=_tmp108.f1;if(_tmp10B != Cyc_Absyn_BoxKind)goto _LL5C;_tmp10C=
_tmp108.f2;if(_tmp10C != Cyc_Absyn_AnyKind)goto _LL5C;_LL5B: goto _LL5D;_LL5C:
_tmp10D=_tmp108.f1;if(_tmp10D != Cyc_Absyn_MemKind)goto _LL5E;_tmp10E=_tmp108.f2;
if(_tmp10E != Cyc_Absyn_AnyKind)goto _LL5E;_LL5D: goto _LL5F;_LL5E: _tmp10F=_tmp108.f1;
if(_tmp10F != Cyc_Absyn_RgnKind)goto _LL60;_tmp110=_tmp108.f2;if(_tmp110 != Cyc_Absyn_TopRgnKind)
goto _LL60;_LL5F: goto _LL61;_LL60: _tmp111=_tmp108.f1;if(_tmp111 != Cyc_Absyn_UniqueRgnKind)
goto _LL62;_tmp112=_tmp108.f2;if(_tmp112 != Cyc_Absyn_TopRgnKind)goto _LL62;_LL61:
return 1;_LL62:;_LL63: return 0;_LL57:;};}static int Cyc_Tcutil_is_region_kind(enum 
Cyc_Absyn_Kind k);static int Cyc_Tcutil_is_region_kind(enum Cyc_Absyn_Kind k){
switch(k){case Cyc_Absyn_RgnKind: _LL64: goto _LL65;case Cyc_Absyn_TopRgnKind: _LL65:
goto _LL66;case Cyc_Absyn_UniqueRgnKind: _LL66: return 1;default: _LL67: return 0;}}
enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,enum Cyc_Absyn_Kind
def);enum Cyc_Absyn_Kind Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,enum Cyc_Absyn_Kind
def){void*_tmp113=Cyc_Absyn_compress_kb(tv->kind);enum Cyc_Absyn_Kind _tmp115;
enum Cyc_Absyn_Kind _tmp117;_LL6A: {struct Cyc_Absyn_Eq_kb_struct*_tmp114=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp113;if(_tmp114->tag != 0)goto _LL6C;else{_tmp115=
_tmp114->f1;}}_LL6B: return _tmp115;_LL6C: {struct Cyc_Absyn_Less_kb_struct*_tmp116=(
struct Cyc_Absyn_Less_kb_struct*)_tmp113;if(_tmp116->tag != 2)goto _LL6E;else{
_tmp117=_tmp116->f2;}}_LL6D: return _tmp117;_LL6E:;_LL6F:{struct Cyc_Absyn_Less_kb_struct
_tmpF18;struct Cyc_Absyn_Less_kb_struct*_tmpF17;tv->kind=(void*)((_tmpF17=
_cycalloc(sizeof(*_tmpF17)),((_tmpF17[0]=((_tmpF18.tag=2,((_tmpF18.f1=0,((
_tmpF18.f2=def,_tmpF18)))))),_tmpF17))));}return def;_LL69:;}int Cyc_Tcutil_unify_kindbound(
void*kb1,void*kb2);int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){struct
_tuple0 _tmpF19;struct _tuple0 _tmp11B=(_tmpF19.f1=Cyc_Absyn_compress_kb(kb1),((
_tmpF19.f2=Cyc_Absyn_compress_kb(kb2),_tmpF19)));void*_tmp11C;enum Cyc_Absyn_Kind
_tmp11E;void*_tmp11F;enum Cyc_Absyn_Kind _tmp121;void*_tmp122;struct Cyc_Core_Opt*
_tmp124;struct Cyc_Core_Opt**_tmp125;enum Cyc_Absyn_Kind _tmp126;void*_tmp127;
enum Cyc_Absyn_Kind _tmp129;void*_tmp12A;enum Cyc_Absyn_Kind _tmp12C;void*_tmp12D;
struct Cyc_Core_Opt*_tmp12F;struct Cyc_Core_Opt**_tmp130;enum Cyc_Absyn_Kind
_tmp131;void*_tmp132;struct Cyc_Core_Opt*_tmp134;struct Cyc_Core_Opt**_tmp135;
enum Cyc_Absyn_Kind _tmp136;void*_tmp137;struct Cyc_Core_Opt*_tmp139;struct Cyc_Core_Opt**
_tmp13A;enum Cyc_Absyn_Kind _tmp13B;void*_tmp13C;struct Cyc_Core_Opt*_tmp13E;
struct Cyc_Core_Opt**_tmp13F;void*_tmp140;void*_tmp141;void*_tmp142;struct Cyc_Core_Opt*
_tmp144;struct Cyc_Core_Opt**_tmp145;_LL71: _tmp11C=_tmp11B.f1;{struct Cyc_Absyn_Eq_kb_struct*
_tmp11D=(struct Cyc_Absyn_Eq_kb_struct*)_tmp11C;if(_tmp11D->tag != 0)goto _LL73;
else{_tmp11E=_tmp11D->f1;}};_tmp11F=_tmp11B.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp120=(struct Cyc_Absyn_Eq_kb_struct*)_tmp11F;if(_tmp120->tag != 0)goto _LL73;
else{_tmp121=_tmp120->f1;}};_LL72: return _tmp11E == _tmp121;_LL73: _tmp122=_tmp11B.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp123=(struct Cyc_Absyn_Less_kb_struct*)_tmp122;
if(_tmp123->tag != 2)goto _LL75;else{_tmp124=_tmp123->f1;_tmp125=(struct Cyc_Core_Opt**)&
_tmp123->f1;_tmp126=_tmp123->f2;}};_tmp127=_tmp11B.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp128=(struct Cyc_Absyn_Eq_kb_struct*)_tmp127;if(_tmp128->tag != 0)goto _LL75;
else{_tmp129=_tmp128->f1;}};_LL74: if(Cyc_Tcutil_kind_leq(_tmp129,_tmp126)){{void**
_tmpF1A;*_tmp125=(struct Cyc_Core_Opt*)((_tmpF1A=_cycalloc(sizeof(*_tmpF1A)),((
_tmpF1A[0]=kb2,_tmpF1A))));}return 1;}else{return 0;}_LL75: _tmp12A=_tmp11B.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp12B=(struct Cyc_Absyn_Eq_kb_struct*)_tmp12A;if(
_tmp12B->tag != 0)goto _LL77;else{_tmp12C=_tmp12B->f1;}};_tmp12D=_tmp11B.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp12E=(struct Cyc_Absyn_Less_kb_struct*)_tmp12D;
if(_tmp12E->tag != 2)goto _LL77;else{_tmp12F=_tmp12E->f1;_tmp130=(struct Cyc_Core_Opt**)&
_tmp12E->f1;_tmp131=_tmp12E->f2;}};_LL76: if(Cyc_Tcutil_kind_leq(_tmp12C,_tmp131)){{
void**_tmpF1B;*_tmp130=(struct Cyc_Core_Opt*)((_tmpF1B=_cycalloc(sizeof(*_tmpF1B)),((
_tmpF1B[0]=kb1,_tmpF1B))));}return 1;}else{return 0;}_LL77: _tmp132=_tmp11B.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp133=(struct Cyc_Absyn_Less_kb_struct*)_tmp132;
if(_tmp133->tag != 2)goto _LL79;else{_tmp134=_tmp133->f1;_tmp135=(struct Cyc_Core_Opt**)&
_tmp133->f1;_tmp136=_tmp133->f2;}};_tmp137=_tmp11B.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp138=(struct Cyc_Absyn_Less_kb_struct*)_tmp137;if(_tmp138->tag != 2)goto _LL79;
else{_tmp139=_tmp138->f1;_tmp13A=(struct Cyc_Core_Opt**)& _tmp138->f1;_tmp13B=
_tmp138->f2;}};_LL78: if(Cyc_Tcutil_kind_leq(_tmp136,_tmp13B)){{void**_tmpF1C;*
_tmp13A=(struct Cyc_Core_Opt*)((_tmpF1C=_cycalloc(sizeof(*_tmpF1C)),((_tmpF1C[0]=
kb1,_tmpF1C))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp13B,_tmp136)){{void**
_tmpF1D;*_tmp135=(struct Cyc_Core_Opt*)((_tmpF1D=_cycalloc(sizeof(*_tmpF1D)),((
_tmpF1D[0]=kb2,_tmpF1D))));}return 1;}else{return 0;}}_LL79: _tmp13C=_tmp11B.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmp13D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp13C;if(_tmp13D->tag != 1)goto _LL7B;else{_tmp13E=_tmp13D->f1;_tmp13F=(struct
Cyc_Core_Opt**)& _tmp13D->f1;}};_tmp140=_tmp11B.f2;_LL7A: _tmp141=_tmp140;_tmp145=
_tmp13F;goto _LL7C;_LL7B: _tmp141=_tmp11B.f1;_tmp142=_tmp11B.f2;{struct Cyc_Absyn_Unknown_kb_struct*
_tmp143=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp142;if(_tmp143->tag != 1)goto
_LL70;else{_tmp144=_tmp143->f1;_tmp145=(struct Cyc_Core_Opt**)& _tmp143->f1;}};
_LL7C:{void**_tmpF1E;*_tmp145=(struct Cyc_Core_Opt*)((_tmpF1E=_cycalloc(sizeof(*
_tmpF1E)),((_tmpF1E[0]=_tmp141,_tmpF1E))));}return 1;_LL70:;}struct _tuple14 Cyc_Tcutil_swap_kind(
void*t,void*kb);struct _tuple14 Cyc_Tcutil_swap_kind(void*t,void*kb){void*_tmp14B=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp14D;_LL7E: {struct Cyc_Absyn_VarType_struct*
_tmp14C=(struct Cyc_Absyn_VarType_struct*)_tmp14B;if(_tmp14C->tag != 2)goto _LL80;
else{_tmp14D=_tmp14C->f1;}}_LL7F: {void*_tmp14E=_tmp14D->kind;_tmp14D->kind=kb;{
struct _tuple14 _tmpF1F;return(_tmpF1F.f1=_tmp14D,((_tmpF1F.f2=_tmp14E,_tmpF1F)));};}
_LL80:;_LL81: {const char*_tmpF23;void*_tmpF22[1];struct Cyc_String_pa_struct
_tmpF21;(_tmpF21.tag=0,((_tmpF21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpF22[0]=& _tmpF21,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF23="swap_kind: cannot update the kind of %s",
_tag_dyneither(_tmpF23,sizeof(char),40))),_tag_dyneither(_tmpF22,sizeof(void*),1)))))));}
_LL7D:;}enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(void*t);enum Cyc_Absyn_Kind Cyc_Tcutil_typ_kind(
void*t){void*_tmp153=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmp155;struct Cyc_Absyn_Tvar*
_tmp157;enum Cyc_Absyn_Size_of _tmp15A;struct Cyc_Absyn_DatatypeFieldInfo _tmp166;
union Cyc_Absyn_DatatypeFieldInfoU _tmp167;struct _tuple3 _tmp168;struct Cyc_Absyn_Datatypedecl*
_tmp169;struct Cyc_Absyn_Datatypefield*_tmp16A;struct Cyc_Absyn_DatatypeFieldInfo
_tmp16C;union Cyc_Absyn_DatatypeFieldInfoU _tmp16D;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmp16E;struct Cyc_Absyn_AggrInfo _tmp170;union Cyc_Absyn_AggrInfoU _tmp171;struct
_tuple4 _tmp172;struct Cyc_Absyn_AggrInfo _tmp174;union Cyc_Absyn_AggrInfoU _tmp175;
struct Cyc_Absyn_Aggrdecl**_tmp176;struct Cyc_Absyn_Aggrdecl*_tmp177;struct Cyc_Absyn_Aggrdecl
_tmp178;struct Cyc_Absyn_AggrdeclImpl*_tmp179;struct Cyc_Absyn_PtrInfo _tmp17C;
struct Cyc_Absyn_Typedefdecl*_tmp182;_LL83: {struct Cyc_Absyn_Evar_struct*_tmp154=(
struct Cyc_Absyn_Evar_struct*)_tmp153;if(_tmp154->tag != 1)goto _LL85;else{_tmp155=
_tmp154->f1;}}_LL84: return(enum Cyc_Absyn_Kind)((struct Cyc_Core_Opt*)_check_null(
_tmp155))->v;_LL85: {struct Cyc_Absyn_VarType_struct*_tmp156=(struct Cyc_Absyn_VarType_struct*)
_tmp153;if(_tmp156->tag != 2)goto _LL87;else{_tmp157=_tmp156->f1;}}_LL86: return Cyc_Tcutil_tvar_kind(
_tmp157,Cyc_Absyn_BoxKind);_LL87: {struct Cyc_Absyn_VoidType_struct*_tmp158=(
struct Cyc_Absyn_VoidType_struct*)_tmp153;if(_tmp158->tag != 0)goto _LL89;}_LL88:
return Cyc_Absyn_MemKind;_LL89: {struct Cyc_Absyn_IntType_struct*_tmp159=(struct
Cyc_Absyn_IntType_struct*)_tmp153;if(_tmp159->tag != 6)goto _LL8B;else{_tmp15A=
_tmp159->f2;}}_LL8A: return(_tmp15A == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Int_sz
 || _tmp15A == (enum Cyc_Absyn_Size_of)Cyc_Absyn_Long_sz)?Cyc_Absyn_BoxKind: Cyc_Absyn_MemKind;
_LL8B: {struct Cyc_Absyn_FloatType_struct*_tmp15B=(struct Cyc_Absyn_FloatType_struct*)
_tmp153;if(_tmp15B->tag != 7)goto _LL8D;}_LL8C: goto _LL8E;_LL8D: {struct Cyc_Absyn_DoubleType_struct*
_tmp15C=(struct Cyc_Absyn_DoubleType_struct*)_tmp153;if(_tmp15C->tag != 8)goto
_LL8F;}_LL8E: goto _LL90;_LL8F: {struct Cyc_Absyn_FnType_struct*_tmp15D=(struct Cyc_Absyn_FnType_struct*)
_tmp153;if(_tmp15D->tag != 10)goto _LL91;}_LL90: return Cyc_Absyn_MemKind;_LL91: {
struct Cyc_Absyn_DynRgnType_struct*_tmp15E=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp153;if(_tmp15E->tag != 17)goto _LL93;}_LL92: goto _LL94;_LL93: {struct Cyc_Absyn_EnumType_struct*
_tmp15F=(struct Cyc_Absyn_EnumType_struct*)_tmp153;if(_tmp15F->tag != 14)goto _LL95;}
_LL94: goto _LL96;_LL95: {struct Cyc_Absyn_AnonEnumType_struct*_tmp160=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp153;if(_tmp160->tag != 15)goto _LL97;}_LL96: goto _LL98;_LL97: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp161=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp153;if(_tmp161->tag != 16)goto
_LL99;}_LL98: return Cyc_Absyn_BoxKind;_LL99: {struct Cyc_Absyn_UniqueRgn_struct*
_tmp162=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp153;if(_tmp162->tag != 22)goto
_LL9B;}_LL9A: return Cyc_Absyn_UniqueRgnKind;_LL9B: {struct Cyc_Absyn_HeapRgn_struct*
_tmp163=(struct Cyc_Absyn_HeapRgn_struct*)_tmp153;if(_tmp163->tag != 21)goto _LL9D;}
_LL9C: return Cyc_Absyn_RgnKind;_LL9D: {struct Cyc_Absyn_DatatypeType_struct*
_tmp164=(struct Cyc_Absyn_DatatypeType_struct*)_tmp153;if(_tmp164->tag != 3)goto
_LL9F;}_LL9E: return Cyc_Absyn_AnyKind;_LL9F: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp165=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp153;if(_tmp165->tag != 4)
goto _LLA1;else{_tmp166=_tmp165->f1;_tmp167=_tmp166.field_info;if((_tmp167.KnownDatatypefield).tag
!= 2)goto _LLA1;_tmp168=(struct _tuple3)(_tmp167.KnownDatatypefield).val;_tmp169=
_tmp168.f1;_tmp16A=_tmp168.f2;}}_LLA0: return Cyc_Absyn_MemKind;_LLA1: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp16B=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp153;if(_tmp16B->tag != 4)
goto _LLA3;else{_tmp16C=_tmp16B->f1;_tmp16D=_tmp16C.field_info;if((_tmp16D.UnknownDatatypefield).tag
!= 1)goto _LLA3;_tmp16E=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp16D.UnknownDatatypefield).val;}}
_LLA2: {const char*_tmpF26;void*_tmpF25;(_tmpF25=0,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF26="typ_kind: Unresolved DatatypeFieldType",
_tag_dyneither(_tmpF26,sizeof(char),39))),_tag_dyneither(_tmpF25,sizeof(void*),0)));}
_LLA3: {struct Cyc_Absyn_AggrType_struct*_tmp16F=(struct Cyc_Absyn_AggrType_struct*)
_tmp153;if(_tmp16F->tag != 12)goto _LLA5;else{_tmp170=_tmp16F->f1;_tmp171=_tmp170.aggr_info;
if((_tmp171.UnknownAggr).tag != 1)goto _LLA5;_tmp172=(struct _tuple4)(_tmp171.UnknownAggr).val;}}
_LLA4: return Cyc_Absyn_AnyKind;_LLA5: {struct Cyc_Absyn_AggrType_struct*_tmp173=(
struct Cyc_Absyn_AggrType_struct*)_tmp153;if(_tmp173->tag != 12)goto _LLA7;else{
_tmp174=_tmp173->f1;_tmp175=_tmp174.aggr_info;if((_tmp175.KnownAggr).tag != 2)
goto _LLA7;_tmp176=(struct Cyc_Absyn_Aggrdecl**)(_tmp175.KnownAggr).val;_tmp177=*
_tmp176;_tmp178=*_tmp177;_tmp179=_tmp178.impl;}}_LLA6: return _tmp179 == 0?Cyc_Absyn_AnyKind:
Cyc_Absyn_MemKind;_LLA7: {struct Cyc_Absyn_AnonAggrType_struct*_tmp17A=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp153;if(_tmp17A->tag != 13)goto _LLA9;}_LLA8: return Cyc_Absyn_MemKind;_LLA9: {
struct Cyc_Absyn_PointerType_struct*_tmp17B=(struct Cyc_Absyn_PointerType_struct*)
_tmp153;if(_tmp17B->tag != 5)goto _LLAB;else{_tmp17C=_tmp17B->f1;}}_LLAA: {void*
_tmp188=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,(_tmp17C.ptr_atts).bounds);_LLBC: {struct Cyc_Absyn_DynEither_b_struct*
_tmp189=(struct Cyc_Absyn_DynEither_b_struct*)_tmp188;if(_tmp189->tag != 0)goto
_LLBE;}_LLBD: return Cyc_Absyn_MemKind;_LLBE: {struct Cyc_Absyn_Upper_b_struct*
_tmp18A=(struct Cyc_Absyn_Upper_b_struct*)_tmp188;if(_tmp18A->tag != 1)goto _LLBB;}
_LLBF: return Cyc_Absyn_BoxKind;_LLBB:;}_LLAB: {struct Cyc_Absyn_ValueofType_struct*
_tmp17D=(struct Cyc_Absyn_ValueofType_struct*)_tmp153;if(_tmp17D->tag != 19)goto
_LLAD;}_LLAC: return Cyc_Absyn_IntKind;_LLAD: {struct Cyc_Absyn_TagType_struct*
_tmp17E=(struct Cyc_Absyn_TagType_struct*)_tmp153;if(_tmp17E->tag != 20)goto _LLAF;}
_LLAE: return Cyc_Absyn_BoxKind;_LLAF: {struct Cyc_Absyn_ArrayType_struct*_tmp17F=(
struct Cyc_Absyn_ArrayType_struct*)_tmp153;if(_tmp17F->tag != 9)goto _LLB1;}_LLB0:
goto _LLB2;_LLB1: {struct Cyc_Absyn_TupleType_struct*_tmp180=(struct Cyc_Absyn_TupleType_struct*)
_tmp153;if(_tmp180->tag != 11)goto _LLB3;}_LLB2: return Cyc_Absyn_MemKind;_LLB3: {
struct Cyc_Absyn_TypedefType_struct*_tmp181=(struct Cyc_Absyn_TypedefType_struct*)
_tmp153;if(_tmp181->tag != 18)goto _LLB5;else{_tmp182=_tmp181->f3;}}_LLB4: if(
_tmp182 == 0  || _tmp182->kind == 0){const char*_tmpF2A;void*_tmpF29[1];struct Cyc_String_pa_struct
_tmpF28;(_tmpF28.tag=0,((_tmpF28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmpF29[0]=& _tmpF28,((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF2A="typ_kind: typedef found: %s",
_tag_dyneither(_tmpF2A,sizeof(char),28))),_tag_dyneither(_tmpF29,sizeof(void*),1)))))));}
return(enum Cyc_Absyn_Kind)((struct Cyc_Core_Opt*)_check_null(_tmp182->kind))->v;
_LLB5: {struct Cyc_Absyn_AccessEff_struct*_tmp183=(struct Cyc_Absyn_AccessEff_struct*)
_tmp153;if(_tmp183->tag != 23)goto _LLB7;}_LLB6: goto _LLB8;_LLB7: {struct Cyc_Absyn_JoinEff_struct*
_tmp184=(struct Cyc_Absyn_JoinEff_struct*)_tmp153;if(_tmp184->tag != 24)goto _LLB9;}
_LLB8: goto _LLBA;_LLB9: {struct Cyc_Absyn_RgnsEff_struct*_tmp185=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp153;if(_tmp185->tag != 25)goto _LL82;}_LLBA: return Cyc_Absyn_EffKind;_LL82:;}
int Cyc_Tcutil_unify(void*t1,void*t2);int Cyc_Tcutil_unify(void*t1,void*t2){struct
_handler_cons _tmp18E;_push_handler(& _tmp18E);{int _tmp190=0;if(setjmp(_tmp18E.handler))
_tmp190=1;if(!_tmp190){Cyc_Tcutil_unify_it(t1,t2);{int _tmp191=1;_npop_handler(0);
return _tmp191;};;_pop_handler();}else{void*_tmp18F=(void*)_exn_thrown;void*
_tmp193=_tmp18F;_LLC1: {struct Cyc_Tcutil_Unify_struct*_tmp194=(struct Cyc_Tcutil_Unify_struct*)
_tmp193;if(_tmp194->tag != Cyc_Tcutil_Unify)goto _LLC3;}_LLC2: return 0;_LLC3:;_LLC4:(
void)_throw(_tmp193);_LLC0:;}};}static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(
t);{void*_tmp195=t;struct Cyc_Absyn_Tvar*_tmp197;struct Cyc_Core_Opt*_tmp199;
struct Cyc_Core_Opt*_tmp19A;struct Cyc_Core_Opt**_tmp19B;struct Cyc_Absyn_PtrInfo
_tmp19D;struct Cyc_Absyn_ArrayInfo _tmp19F;void*_tmp1A0;struct Cyc_Absyn_FnInfo
_tmp1A2;struct Cyc_List_List*_tmp1A3;struct Cyc_Core_Opt*_tmp1A4;void*_tmp1A5;
struct Cyc_List_List*_tmp1A6;int _tmp1A7;struct Cyc_Absyn_VarargInfo*_tmp1A8;struct
Cyc_List_List*_tmp1A9;struct Cyc_List_List*_tmp1AA;struct Cyc_List_List*_tmp1AC;
struct Cyc_Absyn_DatatypeInfo _tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_List_List*
_tmp1B1;struct Cyc_Absyn_DatatypeFieldInfo _tmp1B3;struct Cyc_List_List*_tmp1B4;
struct Cyc_Absyn_AggrInfo _tmp1B6;struct Cyc_List_List*_tmp1B7;struct Cyc_List_List*
_tmp1B9;void*_tmp1BB;void*_tmp1BD;void*_tmp1BF;void*_tmp1C1;struct Cyc_List_List*
_tmp1C3;_LLC6: {struct Cyc_Absyn_VarType_struct*_tmp196=(struct Cyc_Absyn_VarType_struct*)
_tmp195;if(_tmp196->tag != 2)goto _LLC8;else{_tmp197=_tmp196->f1;}}_LLC7: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp197)){{
const char*_tmpF2B;Cyc_Tcutil_failure_reason=((_tmpF2B="(type variable would escape scope)",
_tag_dyneither(_tmpF2B,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
goto _LLC5;_LLC8: {struct Cyc_Absyn_Evar_struct*_tmp198=(struct Cyc_Absyn_Evar_struct*)
_tmp195;if(_tmp198->tag != 1)goto _LLCA;else{_tmp199=_tmp198->f2;_tmp19A=_tmp198->f4;
_tmp19B=(struct Cyc_Core_Opt**)& _tmp198->f4;}}_LLC9: if(t == evar){{const char*
_tmpF2C;Cyc_Tcutil_failure_reason=((_tmpF2C="(occurs check)",_tag_dyneither(
_tmpF2C,sizeof(char),15)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{if(
_tmp199 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp199->v);else{int problem=0;{
struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*
_tmp19B))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}
if(problem){struct Cyc_List_List*_tmp1C6=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(*_tmp19B))->v;for(0;s != 0;s=s->tl){if(((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)
s->hd)){struct Cyc_List_List*_tmpF2D;_tmp1C6=((_tmpF2D=_cycalloc(sizeof(*_tmpF2D)),((
_tmpF2D->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpF2D->tl=_tmp1C6,_tmpF2D))))));}}}{
struct Cyc_Core_Opt*_tmpF2E;*_tmp19B=((_tmpF2E=_cycalloc(sizeof(*_tmpF2E)),((
_tmpF2E->v=_tmp1C6,_tmpF2E))));};}}}goto _LLC5;_LLCA: {struct Cyc_Absyn_PointerType_struct*
_tmp19C=(struct Cyc_Absyn_PointerType_struct*)_tmp195;if(_tmp19C->tag != 5)goto
_LLCC;else{_tmp19D=_tmp19C->f1;}}_LLCB: Cyc_Tcutil_occurs(evar,r,env,_tmp19D.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp19D.ptr_atts).rgn);goto _LLC5;_LLCC: {struct Cyc_Absyn_ArrayType_struct*
_tmp19E=(struct Cyc_Absyn_ArrayType_struct*)_tmp195;if(_tmp19E->tag != 9)goto _LLCE;
else{_tmp19F=_tmp19E->f1;_tmp1A0=_tmp19F.elt_type;}}_LLCD: Cyc_Tcutil_occurs(evar,
r,env,_tmp1A0);goto _LLC5;_LLCE: {struct Cyc_Absyn_FnType_struct*_tmp1A1=(struct
Cyc_Absyn_FnType_struct*)_tmp195;if(_tmp1A1->tag != 10)goto _LLD0;else{_tmp1A2=
_tmp1A1->f1;_tmp1A3=_tmp1A2.tvars;_tmp1A4=_tmp1A2.effect;_tmp1A5=_tmp1A2.ret_typ;
_tmp1A6=_tmp1A2.args;_tmp1A7=_tmp1A2.c_varargs;_tmp1A8=_tmp1A2.cyc_varargs;
_tmp1A9=_tmp1A2.rgn_po;_tmp1AA=_tmp1A2.attributes;}}_LLCF: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp1A3,env);if(_tmp1A4 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp1A4->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp1A5);for(0;_tmp1A6 != 0;_tmp1A6=_tmp1A6->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple9*)_tmp1A6->hd)).f3);}if(_tmp1A8 != 0)Cyc_Tcutil_occurs(
evar,r,env,_tmp1A8->type);for(0;_tmp1A9 != 0;_tmp1A9=_tmp1A9->tl){struct _tuple0
_tmp1CA;void*_tmp1CB;void*_tmp1CC;struct _tuple0*_tmp1C9=(struct _tuple0*)_tmp1A9->hd;
_tmp1CA=*_tmp1C9;_tmp1CB=_tmp1CA.f1;_tmp1CC=_tmp1CA.f2;Cyc_Tcutil_occurs(evar,r,
env,_tmp1CB);Cyc_Tcutil_occurs(evar,r,env,_tmp1CC);}goto _LLC5;_LLD0: {struct Cyc_Absyn_TupleType_struct*
_tmp1AB=(struct Cyc_Absyn_TupleType_struct*)_tmp195;if(_tmp1AB->tag != 11)goto
_LLD2;else{_tmp1AC=_tmp1AB->f1;}}_LLD1: for(0;_tmp1AC != 0;_tmp1AC=_tmp1AC->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)_tmp1AC->hd)).f2);}goto _LLC5;
_LLD2: {struct Cyc_Absyn_DatatypeType_struct*_tmp1AD=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp195;if(_tmp1AD->tag != 3)goto _LLD4;else{_tmp1AE=_tmp1AD->f1;_tmp1AF=_tmp1AE.targs;}}
_LLD3: Cyc_Tcutil_occurslist(evar,r,env,_tmp1AF);goto _LLC5;_LLD4: {struct Cyc_Absyn_TypedefType_struct*
_tmp1B0=(struct Cyc_Absyn_TypedefType_struct*)_tmp195;if(_tmp1B0->tag != 18)goto
_LLD6;else{_tmp1B1=_tmp1B0->f2;}}_LLD5: _tmp1B4=_tmp1B1;goto _LLD7;_LLD6: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp1B2=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp195;if(_tmp1B2->tag != 4)goto _LLD8;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=_tmp1B3.targs;}}
_LLD7: _tmp1B7=_tmp1B4;goto _LLD9;_LLD8: {struct Cyc_Absyn_AggrType_struct*_tmp1B5=(
struct Cyc_Absyn_AggrType_struct*)_tmp195;if(_tmp1B5->tag != 12)goto _LLDA;else{
_tmp1B6=_tmp1B5->f1;_tmp1B7=_tmp1B6.targs;}}_LLD9: Cyc_Tcutil_occurslist(evar,r,
env,_tmp1B7);goto _LLC5;_LLDA: {struct Cyc_Absyn_AnonAggrType_struct*_tmp1B8=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp195;if(_tmp1B8->tag != 13)goto _LLDC;
else{_tmp1B9=_tmp1B8->f2;}}_LLDB: for(0;_tmp1B9 != 0;_tmp1B9=_tmp1B9->tl){Cyc_Tcutil_occurs(
evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1B9->hd)->type);}goto _LLC5;_LLDC: {
struct Cyc_Absyn_TagType_struct*_tmp1BA=(struct Cyc_Absyn_TagType_struct*)_tmp195;
if(_tmp1BA->tag != 20)goto _LLDE;else{_tmp1BB=(void*)_tmp1BA->f1;}}_LLDD: _tmp1BD=
_tmp1BB;goto _LLDF;_LLDE: {struct Cyc_Absyn_AccessEff_struct*_tmp1BC=(struct Cyc_Absyn_AccessEff_struct*)
_tmp195;if(_tmp1BC->tag != 23)goto _LLE0;else{_tmp1BD=(void*)_tmp1BC->f1;}}_LLDF:
_tmp1BF=_tmp1BD;goto _LLE1;_LLE0: {struct Cyc_Absyn_RgnHandleType_struct*_tmp1BE=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp195;if(_tmp1BE->tag != 16)goto _LLE2;
else{_tmp1BF=(void*)_tmp1BE->f1;}}_LLE1: _tmp1C1=_tmp1BF;goto _LLE3;_LLE2: {struct
Cyc_Absyn_RgnsEff_struct*_tmp1C0=(struct Cyc_Absyn_RgnsEff_struct*)_tmp195;if(
_tmp1C0->tag != 25)goto _LLE4;else{_tmp1C1=(void*)_tmp1C0->f1;}}_LLE3: Cyc_Tcutil_occurs(
evar,r,env,_tmp1C1);goto _LLC5;_LLE4: {struct Cyc_Absyn_JoinEff_struct*_tmp1C2=(
struct Cyc_Absyn_JoinEff_struct*)_tmp195;if(_tmp1C2->tag != 24)goto _LLE6;else{
_tmp1C3=_tmp1C2->f1;}}_LLE5: Cyc_Tcutil_occurslist(evar,r,env,_tmp1C3);goto _LLC5;
_LLE6:;_LLE7: goto _LLC5;_LLC5:;};}static void Cyc_Tcutil_occurslist(void*evar,
struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void
Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,
struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(
void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2);static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2);static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,
struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const
char*_tmpF31;void*_tmpF30;(_tmpF30=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF31="tq1 real_const not set.",
_tag_dyneither(_tmpF31,sizeof(char),24))),_tag_dyneither(_tmpF30,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){const char*_tmpF34;void*_tmpF33;(_tmpF33=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmpF34="tq2 real_const not set.",_tag_dyneither(_tmpF34,sizeof(char),24))),
_tag_dyneither(_tmpF33,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmpF35;Cyc_Tcutil_failure_reason=((_tmpF35="(qualifiers don't match)",
_tag_dyneither(_tmpF35,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=0;}int Cyc_Tcutil_equal_tqual(struct
Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual
tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.real_const == tq2.real_const  && tq1.q_volatile
== tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,
struct _dyneither_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct
_dyneither_ptr reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(
y);if(x == y)return;{union Cyc_Absyn_Constraint*_tmp1D2=x;union Cyc_Absyn_Constraint
_tmp1D3;int _tmp1D4;union Cyc_Absyn_Constraint _tmp1D5;void*_tmp1D6;union Cyc_Absyn_Constraint
_tmp1D7;union Cyc_Absyn_Constraint*_tmp1D8;_LLE9: _tmp1D3=*_tmp1D2;if((_tmp1D3.No_constr).tag
!= 3)goto _LLEB;_tmp1D4=(int)(_tmp1D3.No_constr).val;_LLEA:{union Cyc_Absyn_Constraint
_tmpF36;*x=(((_tmpF36.Forward_constr).val=y,(((_tmpF36.Forward_constr).tag=2,
_tmpF36))));}return;_LLEB: _tmp1D5=*_tmp1D2;if((_tmp1D5.Eq_constr).tag != 1)goto
_LLED;_tmp1D6=(void*)(_tmp1D5.Eq_constr).val;_LLEC: {union Cyc_Absyn_Constraint*
_tmp1DA=y;union Cyc_Absyn_Constraint _tmp1DB;int _tmp1DC;union Cyc_Absyn_Constraint
_tmp1DD;void*_tmp1DE;union Cyc_Absyn_Constraint _tmp1DF;union Cyc_Absyn_Constraint*
_tmp1E0;_LLF0: _tmp1DB=*_tmp1DA;if((_tmp1DB.No_constr).tag != 3)goto _LLF2;_tmp1DC=(
int)(_tmp1DB.No_constr).val;_LLF1:*y=*x;return;_LLF2: _tmp1DD=*_tmp1DA;if((
_tmp1DD.Eq_constr).tag != 1)goto _LLF4;_tmp1DE=(void*)(_tmp1DD.Eq_constr).val;
_LLF3: if(cmp(_tmp1D6,_tmp1DE)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)& Cyc_Tcutil_Unify_val);}return;_LLF4: _tmp1DF=*_tmp1DA;if((_tmp1DF.Forward_constr).tag
!= 2)goto _LLEF;_tmp1E0=(union Cyc_Absyn_Constraint*)(_tmp1DF.Forward_constr).val;
_LLF5: {const char*_tmpF39;void*_tmpF38;(_tmpF38=0,Cyc_Tcutil_impos(((_tmpF39="unify_conref: forward after compress(2)",
_tag_dyneither(_tmpF39,sizeof(char),40))),_tag_dyneither(_tmpF38,sizeof(void*),0)));}
_LLEF:;}_LLED: _tmp1D7=*_tmp1D2;if((_tmp1D7.Forward_constr).tag != 2)goto _LLE8;
_tmp1D8=(union Cyc_Absyn_Constraint*)(_tmp1D7.Forward_constr).val;_LLEE: {const
char*_tmpF3C;void*_tmpF3B;(_tmpF3B=0,Cyc_Tcutil_impos(((_tmpF3C="unify_conref: forward after compress",
_tag_dyneither(_tmpF3C,sizeof(char),37))),_tag_dyneither(_tmpF3B,sizeof(void*),0)));}
_LLE8:;};}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,
void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){struct
_handler_cons _tmp1E5;_push_handler(& _tmp1E5);{int _tmp1E7=0;if(setjmp(_tmp1E5.handler))
_tmp1E7=1;if(!_tmp1E7){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _dyneither_ptr)
_tag_dyneither(0,0,0));{int _tmp1E8=1;_npop_handler(0);return _tmp1E8;};;
_pop_handler();}else{void*_tmp1E6=(void*)_exn_thrown;void*_tmp1EA=_tmp1E6;_LLF7: {
struct Cyc_Tcutil_Unify_struct*_tmp1EB=(struct Cyc_Tcutil_Unify_struct*)_tmp1EA;
if(_tmp1EB->tag != Cyc_Tcutil_Unify)goto _LLF9;}_LLF8: return 0;_LLF9:;_LLFA:(void)
_throw(_tmp1EA);_LLF6:;}};}static int Cyc_Tcutil_boundscmp(void*b1,void*b2);static
int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple0 _tmpF3D;struct _tuple0
_tmp1ED=(_tmpF3D.f1=b1,((_tmpF3D.f2=b2,_tmpF3D)));void*_tmp1EE;void*_tmp1F0;void*
_tmp1F2;void*_tmp1F4;void*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F8;void*_tmp1F9;
struct Cyc_Absyn_Exp*_tmp1FB;_LLFC: _tmp1EE=_tmp1ED.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp1EF=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1EE;if(_tmp1EF->tag != 0)goto
_LLFE;};_tmp1F0=_tmp1ED.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp1F1=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp1F0;if(_tmp1F1->tag != 0)goto _LLFE;};_LLFD:
return 0;_LLFE: _tmp1F2=_tmp1ED.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp1F3=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp1F2;if(_tmp1F3->tag != 0)goto _LL100;};
_LLFF: return - 1;_LL100: _tmp1F4=_tmp1ED.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp1F5=(struct Cyc_Absyn_DynEither_b_struct*)_tmp1F4;if(_tmp1F5->tag != 0)goto
_LL102;};_LL101: return 1;_LL102: _tmp1F6=_tmp1ED.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp1F7=(struct Cyc_Absyn_Upper_b_struct*)_tmp1F6;if(_tmp1F7->tag != 1)goto _LLFB;
else{_tmp1F8=_tmp1F7->f1;}};_tmp1F9=_tmp1ED.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp1FA=(struct Cyc_Absyn_Upper_b_struct*)_tmp1F9;if(_tmp1FA->tag != 1)goto _LLFB;
else{_tmp1FB=_tmp1FA->f1;}};_LL103: return Cyc_Evexp_const_exp_cmp(_tmp1F8,_tmp1FB);
_LLFB:;}static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2);static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple0 _tmpF3E;struct _tuple0 _tmp1FD=(_tmpF3E.f1=b1,((
_tmpF3E.f2=b2,_tmpF3E)));void*_tmp1FE;void*_tmp200;void*_tmp202;void*_tmp204;
void*_tmp206;struct Cyc_Absyn_Exp*_tmp208;void*_tmp209;struct Cyc_Absyn_Exp*
_tmp20B;_LL105: _tmp1FE=_tmp1FD.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp1FF=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp1FE;if(_tmp1FF->tag != 0)goto _LL107;};
_tmp200=_tmp1FD.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp201=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp200;if(_tmp201->tag != 0)goto _LL107;};_LL106: return 0;_LL107: _tmp202=_tmp1FD.f1;{
struct Cyc_Absyn_DynEither_b_struct*_tmp203=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp202;if(_tmp203->tag != 0)goto _LL109;};_LL108: return - 1;_LL109: _tmp204=_tmp1FD.f2;{
struct Cyc_Absyn_DynEither_b_struct*_tmp205=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp204;if(_tmp205->tag != 0)goto _LL10B;};_LL10A: return 1;_LL10B: _tmp206=_tmp1FD.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp207=(struct Cyc_Absyn_Upper_b_struct*)_tmp206;
if(_tmp207->tag != 1)goto _LL104;else{_tmp208=_tmp207->f1;}};_tmp209=_tmp1FD.f2;{
struct Cyc_Absyn_Upper_b_struct*_tmp20A=(struct Cyc_Absyn_Upper_b_struct*)_tmp209;
if(_tmp20A->tag != 1)goto _LL104;else{_tmp20B=_tmp20A->f1;}};_LL10C: return Cyc_Evexp_const_exp_cmp(
_tmp208,_tmp20B);_LL104:;}static int Cyc_Tcutil_attribute_case_number(void*att);
static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp20C=att;_LL10E: {
struct Cyc_Absyn_Regparm_att_struct*_tmp20D=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp20C;if(_tmp20D->tag != 0)goto _LL110;}_LL10F: return 0;_LL110: {struct Cyc_Absyn_Stdcall_att_struct*
_tmp20E=(struct Cyc_Absyn_Stdcall_att_struct*)_tmp20C;if(_tmp20E->tag != 1)goto
_LL112;}_LL111: return 1;_LL112: {struct Cyc_Absyn_Cdecl_att_struct*_tmp20F=(struct
Cyc_Absyn_Cdecl_att_struct*)_tmp20C;if(_tmp20F->tag != 2)goto _LL114;}_LL113:
return 2;_LL114: {struct Cyc_Absyn_Fastcall_att_struct*_tmp210=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmp20C;if(_tmp210->tag != 3)goto _LL116;}_LL115: return 3;_LL116: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp211=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp20C;if(_tmp211->tag != 4)goto
_LL118;}_LL117: return 4;_LL118: {struct Cyc_Absyn_Const_att_struct*_tmp212=(struct
Cyc_Absyn_Const_att_struct*)_tmp20C;if(_tmp212->tag != 5)goto _LL11A;}_LL119:
return 5;_LL11A: {struct Cyc_Absyn_Aligned_att_struct*_tmp213=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp20C;if(_tmp213->tag != 6)goto _LL11C;}_LL11B: return 6;_LL11C: {struct Cyc_Absyn_Packed_att_struct*
_tmp214=(struct Cyc_Absyn_Packed_att_struct*)_tmp20C;if(_tmp214->tag != 7)goto
_LL11E;}_LL11D: return 7;_LL11E: {struct Cyc_Absyn_Section_att_struct*_tmp215=(
struct Cyc_Absyn_Section_att_struct*)_tmp20C;if(_tmp215->tag != 8)goto _LL120;}
_LL11F: return 8;_LL120: {struct Cyc_Absyn_Nocommon_att_struct*_tmp216=(struct Cyc_Absyn_Nocommon_att_struct*)
_tmp20C;if(_tmp216->tag != 9)goto _LL122;}_LL121: return 9;_LL122: {struct Cyc_Absyn_Shared_att_struct*
_tmp217=(struct Cyc_Absyn_Shared_att_struct*)_tmp20C;if(_tmp217->tag != 10)goto
_LL124;}_LL123: return 10;_LL124: {struct Cyc_Absyn_Unused_att_struct*_tmp218=(
struct Cyc_Absyn_Unused_att_struct*)_tmp20C;if(_tmp218->tag != 11)goto _LL126;}
_LL125: return 11;_LL126: {struct Cyc_Absyn_Weak_att_struct*_tmp219=(struct Cyc_Absyn_Weak_att_struct*)
_tmp20C;if(_tmp219->tag != 12)goto _LL128;}_LL127: return 12;_LL128: {struct Cyc_Absyn_Dllimport_att_struct*
_tmp21A=(struct Cyc_Absyn_Dllimport_att_struct*)_tmp20C;if(_tmp21A->tag != 13)goto
_LL12A;}_LL129: return 13;_LL12A: {struct Cyc_Absyn_Dllexport_att_struct*_tmp21B=(
struct Cyc_Absyn_Dllexport_att_struct*)_tmp20C;if(_tmp21B->tag != 14)goto _LL12C;}
_LL12B: return 14;_LL12C: {struct Cyc_Absyn_No_instrument_function_att_struct*
_tmp21C=(struct Cyc_Absyn_No_instrument_function_att_struct*)_tmp20C;if(_tmp21C->tag
!= 15)goto _LL12E;}_LL12D: return 15;_LL12E: {struct Cyc_Absyn_Constructor_att_struct*
_tmp21D=(struct Cyc_Absyn_Constructor_att_struct*)_tmp20C;if(_tmp21D->tag != 16)
goto _LL130;}_LL12F: return 16;_LL130: {struct Cyc_Absyn_Destructor_att_struct*
_tmp21E=(struct Cyc_Absyn_Destructor_att_struct*)_tmp20C;if(_tmp21E->tag != 17)
goto _LL132;}_LL131: return 17;_LL132: {struct Cyc_Absyn_No_check_memory_usage_att_struct*
_tmp21F=(struct Cyc_Absyn_No_check_memory_usage_att_struct*)_tmp20C;if(_tmp21F->tag
!= 18)goto _LL134;}_LL133: return 18;_LL134: {struct Cyc_Absyn_Format_att_struct*
_tmp220=(struct Cyc_Absyn_Format_att_struct*)_tmp20C;if(_tmp220->tag != 19)goto
_LL136;}_LL135: return 19;_LL136: {struct Cyc_Absyn_Initializes_att_struct*_tmp221=(
struct Cyc_Absyn_Initializes_att_struct*)_tmp20C;if(_tmp221->tag != 20)goto _LL138;}
_LL137: return 20;_LL138:;_LL139: return 21;_LL10D:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpF3F;struct _tuple0 _tmp223=(_tmpF3F.f1=att1,((_tmpF3F.f2=att2,
_tmpF3F)));void*_tmp224;int _tmp226;void*_tmp227;int _tmp229;void*_tmp22A;int
_tmp22C;void*_tmp22D;int _tmp22F;void*_tmp230;int _tmp232;void*_tmp233;int _tmp235;
void*_tmp236;struct _dyneither_ptr _tmp238;void*_tmp239;struct _dyneither_ptr
_tmp23B;void*_tmp23C;enum Cyc_Absyn_Format_Type _tmp23E;int _tmp23F;int _tmp240;
void*_tmp241;enum Cyc_Absyn_Format_Type _tmp243;int _tmp244;int _tmp245;_LL13B:
_tmp224=_tmp223.f1;{struct Cyc_Absyn_Regparm_att_struct*_tmp225=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp224;if(_tmp225->tag != 0)goto _LL13D;else{_tmp226=_tmp225->f1;}};_tmp227=
_tmp223.f2;{struct Cyc_Absyn_Regparm_att_struct*_tmp228=(struct Cyc_Absyn_Regparm_att_struct*)
_tmp227;if(_tmp228->tag != 0)goto _LL13D;else{_tmp229=_tmp228->f1;}};_LL13C:
_tmp22C=_tmp226;_tmp22F=_tmp229;goto _LL13E;_LL13D: _tmp22A=_tmp223.f1;{struct Cyc_Absyn_Initializes_att_struct*
_tmp22B=(struct Cyc_Absyn_Initializes_att_struct*)_tmp22A;if(_tmp22B->tag != 20)
goto _LL13F;else{_tmp22C=_tmp22B->f1;}};_tmp22D=_tmp223.f2;{struct Cyc_Absyn_Initializes_att_struct*
_tmp22E=(struct Cyc_Absyn_Initializes_att_struct*)_tmp22D;if(_tmp22E->tag != 20)
goto _LL13F;else{_tmp22F=_tmp22E->f1;}};_LL13E: _tmp232=_tmp22C;_tmp235=_tmp22F;
goto _LL140;_LL13F: _tmp230=_tmp223.f1;{struct Cyc_Absyn_Aligned_att_struct*_tmp231=(
struct Cyc_Absyn_Aligned_att_struct*)_tmp230;if(_tmp231->tag != 6)goto _LL141;else{
_tmp232=_tmp231->f1;}};_tmp233=_tmp223.f2;{struct Cyc_Absyn_Aligned_att_struct*
_tmp234=(struct Cyc_Absyn_Aligned_att_struct*)_tmp233;if(_tmp234->tag != 6)goto
_LL141;else{_tmp235=_tmp234->f1;}};_LL140: return Cyc_Core_intcmp(_tmp232,_tmp235);
_LL141: _tmp236=_tmp223.f1;{struct Cyc_Absyn_Section_att_struct*_tmp237=(struct Cyc_Absyn_Section_att_struct*)
_tmp236;if(_tmp237->tag != 8)goto _LL143;else{_tmp238=_tmp237->f1;}};_tmp239=
_tmp223.f2;{struct Cyc_Absyn_Section_att_struct*_tmp23A=(struct Cyc_Absyn_Section_att_struct*)
_tmp239;if(_tmp23A->tag != 8)goto _LL143;else{_tmp23B=_tmp23A->f1;}};_LL142: return
Cyc_strcmp((struct _dyneither_ptr)_tmp238,(struct _dyneither_ptr)_tmp23B);_LL143:
_tmp23C=_tmp223.f1;{struct Cyc_Absyn_Format_att_struct*_tmp23D=(struct Cyc_Absyn_Format_att_struct*)
_tmp23C;if(_tmp23D->tag != 19)goto _LL145;else{_tmp23E=_tmp23D->f1;_tmp23F=_tmp23D->f2;
_tmp240=_tmp23D->f3;}};_tmp241=_tmp223.f2;{struct Cyc_Absyn_Format_att_struct*
_tmp242=(struct Cyc_Absyn_Format_att_struct*)_tmp241;if(_tmp242->tag != 19)goto
_LL145;else{_tmp243=_tmp242->f1;_tmp244=_tmp242->f2;_tmp245=_tmp242->f3;}};
_LL144: {int _tmp246=Cyc_Core_intcmp((int)((unsigned int)_tmp23E),(int)((
unsigned int)_tmp243));if(_tmp246 != 0)return _tmp246;{int _tmp247=Cyc_Core_intcmp(
_tmp23F,_tmp244);if(_tmp247 != 0)return _tmp247;return Cyc_Core_intcmp(_tmp240,
_tmp245);};}_LL145:;_LL146: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(
att1),Cyc_Tcutil_attribute_case_number(att2));_LL13A:;}static int Cyc_Tcutil_equal_att(
void*a1,void*a2);static int Cyc_Tcutil_equal_att(void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(
a1,a2)== 0;}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2);int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,
a2))return 0;}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!((int(*)(int(*
pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(
void*)a->hd,a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of(af->type);}static struct
_tuple14*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv);static struct
_tuple14*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;switch(Cyc_Tcutil_tvar_kind(
tv,Cyc_Absyn_BoxKind)){case Cyc_Absyn_UniqueRgnKind: _LL147: t=(void*)& Cyc_Absyn_UniqueRgn_val;
break;case Cyc_Absyn_RgnKind: _LL148: t=(void*)& Cyc_Absyn_HeapRgn_val;break;case Cyc_Absyn_EffKind:
_LL149: t=Cyc_Absyn_empty_effect;break;case Cyc_Absyn_IntKind: _LL14A:{struct Cyc_Absyn_ValueofType_struct
_tmpF42;struct Cyc_Absyn_ValueofType_struct*_tmpF41;t=(void*)((_tmpF41=_cycalloc(
sizeof(*_tmpF41)),((_tmpF41[0]=((_tmpF42.tag=19,((_tmpF42.f1=Cyc_Absyn_uint_exp(
0,0),_tmpF42)))),_tmpF41))));}break;default: _LL14B: t=Cyc_Absyn_sint_typ;break;}{
struct _tuple14*_tmpF43;return(_tmpF43=_cycalloc(sizeof(*_tmpF43)),((_tmpF43->f1=
tv,((_tmpF43->f2=t,_tmpF43)))));};}static void*Cyc_Tcutil_rgns_of(void*t);static
void*Cyc_Tcutil_rgns_of(void*t){void*_tmp24B=Cyc_Tcutil_compress(t);void*_tmp256;
void*_tmp258;void*_tmp259;struct Cyc_Absyn_DatatypeInfo _tmp25B;struct Cyc_List_List*
_tmp25C;struct Cyc_Absyn_PtrInfo _tmp25E;void*_tmp25F;struct Cyc_Absyn_PtrAtts
_tmp260;void*_tmp261;struct Cyc_Absyn_ArrayInfo _tmp263;void*_tmp264;struct Cyc_List_List*
_tmp266;struct Cyc_Absyn_DatatypeFieldInfo _tmp268;struct Cyc_List_List*_tmp269;
struct Cyc_Absyn_AggrInfo _tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_List_List*
_tmp26E;struct Cyc_Absyn_FnInfo _tmp271;struct Cyc_List_List*_tmp272;struct Cyc_Core_Opt*
_tmp273;void*_tmp274;struct Cyc_List_List*_tmp275;struct Cyc_Absyn_VarargInfo*
_tmp276;struct Cyc_List_List*_tmp277;void*_tmp27D;struct Cyc_List_List*_tmp27F;
_LL14E: {struct Cyc_Absyn_VoidType_struct*_tmp24C=(struct Cyc_Absyn_VoidType_struct*)
_tmp24B;if(_tmp24C->tag != 0)goto _LL150;}_LL14F: goto _LL151;_LL150: {struct Cyc_Absyn_FloatType_struct*
_tmp24D=(struct Cyc_Absyn_FloatType_struct*)_tmp24B;if(_tmp24D->tag != 7)goto
_LL152;}_LL151: goto _LL153;_LL152: {struct Cyc_Absyn_DoubleType_struct*_tmp24E=(
struct Cyc_Absyn_DoubleType_struct*)_tmp24B;if(_tmp24E->tag != 8)goto _LL154;}
_LL153: goto _LL155;_LL154: {struct Cyc_Absyn_EnumType_struct*_tmp24F=(struct Cyc_Absyn_EnumType_struct*)
_tmp24B;if(_tmp24F->tag != 14)goto _LL156;}_LL155: goto _LL157;_LL156: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp250=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp24B;if(_tmp250->tag != 15)goto
_LL158;}_LL157: goto _LL159;_LL158: {struct Cyc_Absyn_ValueofType_struct*_tmp251=(
struct Cyc_Absyn_ValueofType_struct*)_tmp24B;if(_tmp251->tag != 19)goto _LL15A;}
_LL159: goto _LL15B;_LL15A: {struct Cyc_Absyn_IntType_struct*_tmp252=(struct Cyc_Absyn_IntType_struct*)
_tmp24B;if(_tmp252->tag != 6)goto _LL15C;}_LL15B: return Cyc_Absyn_empty_effect;
_LL15C: {struct Cyc_Absyn_Evar_struct*_tmp253=(struct Cyc_Absyn_Evar_struct*)
_tmp24B;if(_tmp253->tag != 1)goto _LL15E;}_LL15D: goto _LL15F;_LL15E: {struct Cyc_Absyn_VarType_struct*
_tmp254=(struct Cyc_Absyn_VarType_struct*)_tmp24B;if(_tmp254->tag != 2)goto _LL160;}
_LL15F: switch(Cyc_Tcutil_typ_kind(t)){case Cyc_Absyn_RgnKind: _LL182: goto _LL183;
case Cyc_Absyn_UniqueRgnKind: _LL183: goto _LL184;case Cyc_Absyn_TopRgnKind: _LL184: {
struct Cyc_Absyn_AccessEff_struct _tmpF46;struct Cyc_Absyn_AccessEff_struct*_tmpF45;
return(void*)((_tmpF45=_cycalloc(sizeof(*_tmpF45)),((_tmpF45[0]=((_tmpF46.tag=23,((
_tmpF46.f1=(void*)t,_tmpF46)))),_tmpF45))));}case Cyc_Absyn_EffKind: _LL185: return
t;case Cyc_Absyn_IntKind: _LL186: return Cyc_Absyn_empty_effect;default: _LL187: {
struct Cyc_Absyn_RgnsEff_struct _tmpF49;struct Cyc_Absyn_RgnsEff_struct*_tmpF48;
return(void*)((_tmpF48=_cycalloc(sizeof(*_tmpF48)),((_tmpF48[0]=((_tmpF49.tag=25,((
_tmpF49.f1=(void*)t,_tmpF49)))),_tmpF48))));}}_LL160: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp255=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp24B;if(_tmp255->tag != 16)goto
_LL162;else{_tmp256=(void*)_tmp255->f1;}}_LL161: {struct Cyc_Absyn_AccessEff_struct
_tmpF4C;struct Cyc_Absyn_AccessEff_struct*_tmpF4B;return(void*)((_tmpF4B=
_cycalloc(sizeof(*_tmpF4B)),((_tmpF4B[0]=((_tmpF4C.tag=23,((_tmpF4C.f1=(void*)
_tmp256,_tmpF4C)))),_tmpF4B))));}_LL162: {struct Cyc_Absyn_DynRgnType_struct*
_tmp257=(struct Cyc_Absyn_DynRgnType_struct*)_tmp24B;if(_tmp257->tag != 17)goto
_LL164;else{_tmp258=(void*)_tmp257->f1;_tmp259=(void*)_tmp257->f2;}}_LL163: {
struct Cyc_Absyn_AccessEff_struct _tmpF4F;struct Cyc_Absyn_AccessEff_struct*_tmpF4E;
return(void*)((_tmpF4E=_cycalloc(sizeof(*_tmpF4E)),((_tmpF4E[0]=((_tmpF4F.tag=23,((
_tmpF4F.f1=(void*)_tmp259,_tmpF4F)))),_tmpF4E))));}_LL164: {struct Cyc_Absyn_DatatypeType_struct*
_tmp25A=(struct Cyc_Absyn_DatatypeType_struct*)_tmp24B;if(_tmp25A->tag != 3)goto
_LL166;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25B.targs;}}_LL165: {struct Cyc_List_List*
ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_rgns_of,_tmp25C);struct Cyc_Absyn_JoinEff_struct _tmpF52;struct Cyc_Absyn_JoinEff_struct*
_tmpF51;return Cyc_Tcutil_normalize_effect((void*)((_tmpF51=_cycalloc(sizeof(*
_tmpF51)),((_tmpF51[0]=((_tmpF52.tag=24,((_tmpF52.f1=ts,_tmpF52)))),_tmpF51)))));}
_LL166: {struct Cyc_Absyn_PointerType_struct*_tmp25D=(struct Cyc_Absyn_PointerType_struct*)
_tmp24B;if(_tmp25D->tag != 5)goto _LL168;else{_tmp25E=_tmp25D->f1;_tmp25F=_tmp25E.elt_typ;
_tmp260=_tmp25E.ptr_atts;_tmp261=_tmp260.rgn;}}_LL167: {struct Cyc_Absyn_JoinEff_struct
_tmpF61;struct Cyc_Absyn_AccessEff_struct*_tmpF60;struct Cyc_Absyn_AccessEff_struct
_tmpF5F;void*_tmpF5E[2];struct Cyc_Absyn_JoinEff_struct*_tmpF5D;return Cyc_Tcutil_normalize_effect((
void*)((_tmpF5D=_cycalloc(sizeof(*_tmpF5D)),((_tmpF5D[0]=((_tmpF61.tag=24,((
_tmpF61.f1=((_tmpF5E[1]=Cyc_Tcutil_rgns_of(_tmp25F),((_tmpF5E[0]=(void*)((
_tmpF60=_cycalloc(sizeof(*_tmpF60)),((_tmpF60[0]=((_tmpF5F.tag=23,((_tmpF5F.f1=(
void*)_tmp261,_tmpF5F)))),_tmpF60)))),((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF5E,sizeof(void*),2)))))),
_tmpF61)))),_tmpF5D)))));}_LL168: {struct Cyc_Absyn_ArrayType_struct*_tmp262=(
struct Cyc_Absyn_ArrayType_struct*)_tmp24B;if(_tmp262->tag != 9)goto _LL16A;else{
_tmp263=_tmp262->f1;_tmp264=_tmp263.elt_type;}}_LL169: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp264));_LL16A: {struct Cyc_Absyn_TupleType_struct*_tmp265=(
struct Cyc_Absyn_TupleType_struct*)_tmp24B;if(_tmp265->tag != 11)goto _LL16C;else{
_tmp266=_tmp265->f1;}}_LL16B: {struct Cyc_List_List*_tmp28F=0;for(0;_tmp266 != 0;
_tmp266=_tmp266->tl){struct Cyc_List_List*_tmpF62;_tmp28F=((_tmpF62=_cycalloc(
sizeof(*_tmpF62)),((_tmpF62->hd=(*((struct _tuple11*)_tmp266->hd)).f2,((_tmpF62->tl=
_tmp28F,_tmpF62))))));}_tmp269=_tmp28F;goto _LL16D;}_LL16C: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp267=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp24B;if(_tmp267->tag != 4)
goto _LL16E;else{_tmp268=_tmp267->f1;_tmp269=_tmp268.targs;}}_LL16D: _tmp26C=
_tmp269;goto _LL16F;_LL16E: {struct Cyc_Absyn_AggrType_struct*_tmp26A=(struct Cyc_Absyn_AggrType_struct*)
_tmp24B;if(_tmp26A->tag != 12)goto _LL170;else{_tmp26B=_tmp26A->f1;_tmp26C=_tmp26B.targs;}}
_LL16F: {struct Cyc_Absyn_JoinEff_struct _tmpF65;struct Cyc_Absyn_JoinEff_struct*
_tmpF64;return Cyc_Tcutil_normalize_effect((void*)((_tmpF64=_cycalloc(sizeof(*
_tmpF64)),((_tmpF64[0]=((_tmpF65.tag=24,((_tmpF65.f1=((struct Cyc_List_List*(*)(
void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp26C),
_tmpF65)))),_tmpF64)))));}_LL170: {struct Cyc_Absyn_AnonAggrType_struct*_tmp26D=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp24B;if(_tmp26D->tag != 13)goto _LL172;
else{_tmp26E=_tmp26D->f2;}}_LL171: {struct Cyc_Absyn_JoinEff_struct _tmpF68;struct
Cyc_Absyn_JoinEff_struct*_tmpF67;return Cyc_Tcutil_normalize_effect((void*)((
_tmpF67=_cycalloc(sizeof(*_tmpF67)),((_tmpF67[0]=((_tmpF68.tag=24,((_tmpF68.f1=((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp26E),_tmpF68)))),_tmpF67)))));}
_LL172: {struct Cyc_Absyn_TagType_struct*_tmp26F=(struct Cyc_Absyn_TagType_struct*)
_tmp24B;if(_tmp26F->tag != 20)goto _LL174;}_LL173: return Cyc_Absyn_empty_effect;
_LL174: {struct Cyc_Absyn_FnType_struct*_tmp270=(struct Cyc_Absyn_FnType_struct*)
_tmp24B;if(_tmp270->tag != 10)goto _LL176;else{_tmp271=_tmp270->f1;_tmp272=_tmp271.tvars;
_tmp273=_tmp271.effect;_tmp274=_tmp271.ret_typ;_tmp275=_tmp271.args;_tmp276=
_tmp271.cyc_varargs;_tmp277=_tmp271.rgn_po;}}_LL175: {void*_tmp295=Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp272),(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp273))->v);return Cyc_Tcutil_normalize_effect(_tmp295);}_LL176: {
struct Cyc_Absyn_UniqueRgn_struct*_tmp278=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp24B;if(_tmp278->tag != 22)goto _LL178;}_LL177: goto _LL179;_LL178: {struct Cyc_Absyn_HeapRgn_struct*
_tmp279=(struct Cyc_Absyn_HeapRgn_struct*)_tmp24B;if(_tmp279->tag != 21)goto _LL17A;}
_LL179: return Cyc_Absyn_empty_effect;_LL17A: {struct Cyc_Absyn_AccessEff_struct*
_tmp27A=(struct Cyc_Absyn_AccessEff_struct*)_tmp24B;if(_tmp27A->tag != 23)goto
_LL17C;}_LL17B: goto _LL17D;_LL17C: {struct Cyc_Absyn_JoinEff_struct*_tmp27B=(
struct Cyc_Absyn_JoinEff_struct*)_tmp24B;if(_tmp27B->tag != 24)goto _LL17E;}_LL17D:
return t;_LL17E: {struct Cyc_Absyn_RgnsEff_struct*_tmp27C=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp24B;if(_tmp27C->tag != 25)goto _LL180;else{_tmp27D=(void*)_tmp27C->f1;}}_LL17F:
return Cyc_Tcutil_rgns_of(_tmp27D);_LL180: {struct Cyc_Absyn_TypedefType_struct*
_tmp27E=(struct Cyc_Absyn_TypedefType_struct*)_tmp24B;if(_tmp27E->tag != 18)goto
_LL14D;else{_tmp27F=_tmp27E->f2;}}_LL181: {struct Cyc_Absyn_JoinEff_struct _tmpF6B;
struct Cyc_Absyn_JoinEff_struct*_tmpF6A;return Cyc_Tcutil_normalize_effect((void*)((
_tmpF6A=_cycalloc(sizeof(*_tmpF6A)),((_tmpF6A[0]=((_tmpF6B.tag=24,((_tmpF6B.f1=((
struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_rgns_of,_tmp27F),_tmpF6B)))),_tmpF6A)))));}_LL14D:;}void*Cyc_Tcutil_normalize_effect(
void*e);void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp298=e;struct Cyc_List_List*_tmp29A;struct Cyc_List_List**_tmp29B;void*_tmp29D;
_LL18A: {struct Cyc_Absyn_JoinEff_struct*_tmp299=(struct Cyc_Absyn_JoinEff_struct*)
_tmp298;if(_tmp299->tag != 24)goto _LL18C;else{_tmp29A=_tmp299->f1;_tmp29B=(struct
Cyc_List_List**)& _tmp299->f1;}}_LL18B: {int redo_join=0;{struct Cyc_List_List*effs=*
_tmp29B;for(0;effs != 0;effs=effs->tl){void*_tmp29E=(void*)effs->hd;effs->hd=(
void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp29E));{void*_tmp29F=(
void*)effs->hd;void*_tmp2A2;_LL191: {struct Cyc_Absyn_JoinEff_struct*_tmp2A0=(
struct Cyc_Absyn_JoinEff_struct*)_tmp29F;if(_tmp2A0->tag != 24)goto _LL193;}_LL192:
goto _LL194;_LL193: {struct Cyc_Absyn_AccessEff_struct*_tmp2A1=(struct Cyc_Absyn_AccessEff_struct*)
_tmp29F;if(_tmp2A1->tag != 23)goto _LL195;else{_tmp2A2=(void*)_tmp2A1->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp2A3=(struct Cyc_Absyn_HeapRgn_struct*)_tmp2A2;if(
_tmp2A3->tag != 21)goto _LL195;};}}_LL194: redo_join=1;goto _LL190;_LL195:;_LL196:
goto _LL190;_LL190:;};}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{
struct Cyc_List_List*effs=*_tmp29B;for(0;effs != 0;effs=effs->tl){void*_tmp2A4=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp2A6;void*_tmp2A8;_LL198: {struct Cyc_Absyn_JoinEff_struct*
_tmp2A5=(struct Cyc_Absyn_JoinEff_struct*)_tmp2A4;if(_tmp2A5->tag != 24)goto _LL19A;
else{_tmp2A6=_tmp2A5->f1;}}_LL199: effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp2A6,effects);goto _LL197;_LL19A: {
struct Cyc_Absyn_AccessEff_struct*_tmp2A7=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2A4;if(_tmp2A7->tag != 23)goto _LL19C;else{_tmp2A8=(void*)_tmp2A7->f1;{struct
Cyc_Absyn_HeapRgn_struct*_tmp2A9=(struct Cyc_Absyn_HeapRgn_struct*)_tmp2A8;if(
_tmp2A9->tag != 21)goto _LL19C;};}}_LL19B: goto _LL197;_LL19C:;_LL19D:{struct Cyc_List_List*
_tmpF6C;effects=((_tmpF6C=_cycalloc(sizeof(*_tmpF6C)),((_tmpF6C->hd=_tmp2A4,((
_tmpF6C->tl=effects,_tmpF6C))))));}goto _LL197;_LL197:;}}*_tmp29B=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);return e;};}_LL18C: {struct Cyc_Absyn_RgnsEff_struct*
_tmp29C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp298;if(_tmp29C->tag != 25)goto _LL18E;
else{_tmp29D=(void*)_tmp29C->f1;}}_LL18D: {void*_tmp2AB=Cyc_Tcutil_compress(
_tmp29D);_LL19F: {struct Cyc_Absyn_Evar_struct*_tmp2AC=(struct Cyc_Absyn_Evar_struct*)
_tmp2AB;if(_tmp2AC->tag != 1)goto _LL1A1;}_LL1A0: goto _LL1A2;_LL1A1: {struct Cyc_Absyn_VarType_struct*
_tmp2AD=(struct Cyc_Absyn_VarType_struct*)_tmp2AB;if(_tmp2AD->tag != 2)goto _LL1A3;}
_LL1A2: return e;_LL1A3:;_LL1A4: return Cyc_Tcutil_rgns_of(_tmp29D);_LL19E:;}_LL18E:;
_LL18F: return e;_LL189:;};}static void*Cyc_Tcutil_dummy_fntype(void*eff);static
void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct _tmpF76;
struct Cyc_Core_Opt*_tmpF75;struct Cyc_Absyn_FnInfo _tmpF74;struct Cyc_Absyn_FnType_struct*
_tmpF73;struct Cyc_Absyn_FnType_struct*_tmp2AE=(_tmpF73=_cycalloc(sizeof(*_tmpF73)),((
_tmpF73[0]=((_tmpF76.tag=10,((_tmpF76.f1=((_tmpF74.tvars=0,((_tmpF74.effect=((
_tmpF75=_cycalloc(sizeof(*_tmpF75)),((_tmpF75->v=eff,_tmpF75)))),((_tmpF74.ret_typ=(
void*)& Cyc_Absyn_VoidType_val,((_tmpF74.args=0,((_tmpF74.c_varargs=0,((_tmpF74.cyc_varargs=
0,((_tmpF74.rgn_po=0,((_tmpF74.attributes=0,_tmpF74)))))))))))))))),_tmpF76)))),
_tmpF73)));return Cyc_Absyn_atb_typ((void*)_tmp2AE,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,
void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (
void*)& Cyc_Absyn_UniqueRgn_val)return 1;{void*_tmp2B3=Cyc_Tcutil_compress(e);void*
_tmp2B5;struct Cyc_List_List*_tmp2B7;void*_tmp2B9;struct Cyc_Core_Opt*_tmp2BB;
struct Cyc_Core_Opt*_tmp2BC;struct Cyc_Core_Opt**_tmp2BD;struct Cyc_Core_Opt*
_tmp2BE;_LL1A6: {struct Cyc_Absyn_AccessEff_struct*_tmp2B4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2B3;if(_tmp2B4->tag != 23)goto _LL1A8;else{_tmp2B5=(void*)_tmp2B4->f1;}}_LL1A7:
if(constrain)return Cyc_Tcutil_unify(r,_tmp2B5);_tmp2B5=Cyc_Tcutil_compress(
_tmp2B5);if(r == _tmp2B5)return 1;{struct _tuple0 _tmpF77;struct _tuple0 _tmp2C0=(
_tmpF77.f1=r,((_tmpF77.f2=_tmp2B5,_tmpF77)));void*_tmp2C1;struct Cyc_Absyn_Tvar*
_tmp2C3;void*_tmp2C4;struct Cyc_Absyn_Tvar*_tmp2C6;_LL1B1: _tmp2C1=_tmp2C0.f1;{
struct Cyc_Absyn_VarType_struct*_tmp2C2=(struct Cyc_Absyn_VarType_struct*)_tmp2C1;
if(_tmp2C2->tag != 2)goto _LL1B3;else{_tmp2C3=_tmp2C2->f1;}};_tmp2C4=_tmp2C0.f2;{
struct Cyc_Absyn_VarType_struct*_tmp2C5=(struct Cyc_Absyn_VarType_struct*)_tmp2C4;
if(_tmp2C5->tag != 2)goto _LL1B3;else{_tmp2C6=_tmp2C5->f1;}};_LL1B2: return Cyc_Absyn_tvar_cmp(
_tmp2C3,_tmp2C6)== 0;_LL1B3:;_LL1B4: return 0;_LL1B0:;};_LL1A8: {struct Cyc_Absyn_JoinEff_struct*
_tmp2B6=(struct Cyc_Absyn_JoinEff_struct*)_tmp2B3;if(_tmp2B6->tag != 24)goto _LL1AA;
else{_tmp2B7=_tmp2B6->f1;}}_LL1A9: for(0;_tmp2B7 != 0;_tmp2B7=_tmp2B7->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp2B7->hd))return 1;}return 0;_LL1AA: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2B8=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2B3;if(_tmp2B8->tag != 25)goto _LL1AC;
else{_tmp2B9=(void*)_tmp2B8->f1;}}_LL1AB: {void*_tmp2C7=Cyc_Tcutil_rgns_of(
_tmp2B9);void*_tmp2C9;_LL1B6: {struct Cyc_Absyn_RgnsEff_struct*_tmp2C8=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp2C7;if(_tmp2C8->tag != 25)goto _LL1B8;else{_tmp2C9=(void*)_tmp2C8->f1;}}_LL1B7:
if(!constrain)return 0;{void*_tmp2CA=Cyc_Tcutil_compress(_tmp2C9);struct Cyc_Core_Opt*
_tmp2CC;struct Cyc_Core_Opt*_tmp2CD;struct Cyc_Core_Opt**_tmp2CE;struct Cyc_Core_Opt*
_tmp2CF;_LL1BB: {struct Cyc_Absyn_Evar_struct*_tmp2CB=(struct Cyc_Absyn_Evar_struct*)
_tmp2CA;if(_tmp2CB->tag != 1)goto _LL1BD;else{_tmp2CC=_tmp2CB->f1;_tmp2CD=_tmp2CB->f2;
_tmp2CE=(struct Cyc_Core_Opt**)& _tmp2CB->f2;_tmp2CF=_tmp2CB->f4;}}_LL1BC: {void*
_tmp2D0=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp2CF);Cyc_Tcutil_occurs(
_tmp2D0,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2CF))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmpF86;struct Cyc_Absyn_AccessEff_struct*
_tmpF85;struct Cyc_Absyn_AccessEff_struct _tmpF84;void*_tmpF83[2];struct Cyc_Absyn_JoinEff_struct*
_tmpF82;void*_tmp2D1=Cyc_Tcutil_dummy_fntype((void*)((_tmpF82=_cycalloc(sizeof(*
_tmpF82)),((_tmpF82[0]=((_tmpF86.tag=24,((_tmpF86.f1=((_tmpF83[1]=(void*)((
_tmpF85=_cycalloc(sizeof(*_tmpF85)),((_tmpF85[0]=((_tmpF84.tag=23,((_tmpF84.f1=(
void*)r,_tmpF84)))),_tmpF85)))),((_tmpF83[0]=_tmp2D0,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF83,sizeof(void*),2)))))),
_tmpF86)))),_tmpF82)))));{struct Cyc_Core_Opt*_tmpF87;*_tmp2CE=((_tmpF87=
_cycalloc(sizeof(*_tmpF87)),((_tmpF87->v=_tmp2D1,_tmpF87))));}return 1;};}_LL1BD:;
_LL1BE: return 0;_LL1BA:;};_LL1B8:;_LL1B9: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp2C7);_LL1B5:;}_LL1AC: {struct Cyc_Absyn_Evar_struct*_tmp2BA=(
struct Cyc_Absyn_Evar_struct*)_tmp2B3;if(_tmp2BA->tag != 1)goto _LL1AE;else{_tmp2BB=
_tmp2BA->f1;_tmp2BC=_tmp2BA->f2;_tmp2BD=(struct Cyc_Core_Opt**)& _tmp2BA->f2;
_tmp2BE=_tmp2BA->f4;}}_LL1AD: if(_tmp2BB == 0  || (enum Cyc_Absyn_Kind)_tmp2BB->v != 
Cyc_Absyn_EffKind){const char*_tmpF8A;void*_tmpF89;(_tmpF89=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF8A="effect evar has wrong kind",
_tag_dyneither(_tmpF8A,sizeof(char),27))),_tag_dyneither(_tmpF89,sizeof(void*),0)));}
if(!constrain)return 0;{void*_tmp2DA=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp2BE);Cyc_Tcutil_occurs(_tmp2DA,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2BE))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpF9F;struct Cyc_List_List*_tmpF9E;struct Cyc_Absyn_AccessEff_struct _tmpF9D;
struct Cyc_Absyn_AccessEff_struct*_tmpF9C;struct Cyc_List_List*_tmpF9B;struct Cyc_Absyn_JoinEff_struct*
_tmpF9A;struct Cyc_Absyn_JoinEff_struct*_tmp2DB=(_tmpF9A=_cycalloc(sizeof(*
_tmpF9A)),((_tmpF9A[0]=((_tmpF9F.tag=24,((_tmpF9F.f1=((_tmpF9B=_cycalloc(sizeof(*
_tmpF9B)),((_tmpF9B->hd=_tmp2DA,((_tmpF9B->tl=((_tmpF9E=_cycalloc(sizeof(*
_tmpF9E)),((_tmpF9E->hd=(void*)((_tmpF9C=_cycalloc(sizeof(*_tmpF9C)),((_tmpF9C[0]=((
_tmpF9D.tag=23,((_tmpF9D.f1=(void*)r,_tmpF9D)))),_tmpF9C)))),((_tmpF9E->tl=0,
_tmpF9E)))))),_tmpF9B)))))),_tmpF9F)))),_tmpF9A)));{struct Cyc_Core_Opt*_tmpFA0;*
_tmp2BD=((_tmpFA0=_cycalloc(sizeof(*_tmpFA0)),((_tmpFA0->v=(void*)_tmp2DB,
_tmpFA0))));}return 1;};};_LL1AE:;_LL1AF: return 0;_LL1A5:;};}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(int
may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp2E3=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp2E6;void*_tmp2E8;struct Cyc_Core_Opt*
_tmp2EA;struct Cyc_Core_Opt*_tmp2EB;struct Cyc_Core_Opt**_tmp2EC;struct Cyc_Core_Opt*
_tmp2ED;_LL1C0: {struct Cyc_Absyn_AccessEff_struct*_tmp2E4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp2E3;if(_tmp2E4->tag != 23)goto _LL1C2;}_LL1C1: return 0;_LL1C2: {struct Cyc_Absyn_JoinEff_struct*
_tmp2E5=(struct Cyc_Absyn_JoinEff_struct*)_tmp2E3;if(_tmp2E5->tag != 24)goto _LL1C4;
else{_tmp2E6=_tmp2E5->f1;}}_LL1C3: for(0;_tmp2E6 != 0;_tmp2E6=_tmp2E6->tl){if(Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,(void*)_tmp2E6->hd))return 1;}return 0;_LL1C4: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2E7=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2E3;if(_tmp2E7->tag != 25)goto _LL1C6;
else{_tmp2E8=(void*)_tmp2E7->f1;}}_LL1C5: _tmp2E8=Cyc_Tcutil_compress(_tmp2E8);
if(t == _tmp2E8)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2E8);{
void*_tmp2EE=Cyc_Tcutil_rgns_of(t);void*_tmp2F0;_LL1CB: {struct Cyc_Absyn_RgnsEff_struct*
_tmp2EF=(struct Cyc_Absyn_RgnsEff_struct*)_tmp2EE;if(_tmp2EF->tag != 25)goto _LL1CD;
else{_tmp2F0=(void*)_tmp2EF->f1;}}_LL1CC: {struct _tuple0 _tmpFA1;struct _tuple0
_tmp2F2=(_tmpFA1.f1=Cyc_Tcutil_compress(_tmp2F0),((_tmpFA1.f2=_tmp2E8,_tmpFA1)));
void*_tmp2F3;struct Cyc_Absyn_Tvar*_tmp2F5;void*_tmp2F6;struct Cyc_Absyn_Tvar*
_tmp2F8;_LL1D0: _tmp2F3=_tmp2F2.f1;{struct Cyc_Absyn_VarType_struct*_tmp2F4=(
struct Cyc_Absyn_VarType_struct*)_tmp2F3;if(_tmp2F4->tag != 2)goto _LL1D2;else{
_tmp2F5=_tmp2F4->f1;}};_tmp2F6=_tmp2F2.f2;{struct Cyc_Absyn_VarType_struct*
_tmp2F7=(struct Cyc_Absyn_VarType_struct*)_tmp2F6;if(_tmp2F7->tag != 2)goto _LL1D2;
else{_tmp2F8=_tmp2F7->f1;}};_LL1D1: return Cyc_Tcutil_unify(t,_tmp2E8);_LL1D2:;
_LL1D3: return _tmp2F0 == _tmp2E8;_LL1CF:;}_LL1CD:;_LL1CE: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,t,_tmp2EE);_LL1CA:;};_LL1C6: {struct Cyc_Absyn_Evar_struct*
_tmp2E9=(struct Cyc_Absyn_Evar_struct*)_tmp2E3;if(_tmp2E9->tag != 1)goto _LL1C8;
else{_tmp2EA=_tmp2E9->f1;_tmp2EB=_tmp2E9->f2;_tmp2EC=(struct Cyc_Core_Opt**)&
_tmp2E9->f2;_tmp2ED=_tmp2E9->f4;}}_LL1C7: if(_tmp2EA == 0  || (enum Cyc_Absyn_Kind)
_tmp2EA->v != Cyc_Absyn_EffKind){const char*_tmpFA4;void*_tmpFA3;(_tmpFA3=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFA4="effect evar has wrong kind",
_tag_dyneither(_tmpFA4,sizeof(char),27))),_tag_dyneither(_tmpFA3,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{void*_tmp2FB=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp2ED);Cyc_Tcutil_occurs(_tmp2FB,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp2ED))->v,t);{struct Cyc_Absyn_JoinEff_struct
_tmpFB9;struct Cyc_List_List*_tmpFB8;struct Cyc_Absyn_RgnsEff_struct _tmpFB7;struct
Cyc_Absyn_RgnsEff_struct*_tmpFB6;struct Cyc_List_List*_tmpFB5;struct Cyc_Absyn_JoinEff_struct*
_tmpFB4;struct Cyc_Absyn_JoinEff_struct*_tmp2FC=(_tmpFB4=_cycalloc(sizeof(*
_tmpFB4)),((_tmpFB4[0]=((_tmpFB9.tag=24,((_tmpFB9.f1=((_tmpFB5=_cycalloc(sizeof(*
_tmpFB5)),((_tmpFB5->hd=_tmp2FB,((_tmpFB5->tl=((_tmpFB8=_cycalloc(sizeof(*
_tmpFB8)),((_tmpFB8->hd=(void*)((_tmpFB6=_cycalloc(sizeof(*_tmpFB6)),((_tmpFB6[0]=((
_tmpFB7.tag=25,((_tmpFB7.f1=(void*)t,_tmpFB7)))),_tmpFB6)))),((_tmpFB8->tl=0,
_tmpFB8)))))),_tmpFB5)))))),_tmpFB9)))),_tmpFB4)));{struct Cyc_Core_Opt*_tmpFBA;*
_tmp2EC=((_tmpFBA=_cycalloc(sizeof(*_tmpFBA)),((_tmpFBA->v=(void*)_tmp2FC,
_tmpFBA))));}return 1;};};_LL1C8:;_LL1C9: return 0;_LL1BF:;};}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e);static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp304=e;struct Cyc_Absyn_Tvar*_tmp306;struct Cyc_List_List*_tmp308;void*
_tmp30A;struct Cyc_Core_Opt*_tmp30C;struct Cyc_Core_Opt*_tmp30D;struct Cyc_Core_Opt**
_tmp30E;struct Cyc_Core_Opt*_tmp30F;_LL1D5: {struct Cyc_Absyn_VarType_struct*
_tmp305=(struct Cyc_Absyn_VarType_struct*)_tmp304;if(_tmp305->tag != 2)goto _LL1D7;
else{_tmp306=_tmp305->f1;}}_LL1D6: return Cyc_Absyn_tvar_cmp(v,_tmp306)== 0;_LL1D7: {
struct Cyc_Absyn_JoinEff_struct*_tmp307=(struct Cyc_Absyn_JoinEff_struct*)_tmp304;
if(_tmp307->tag != 24)goto _LL1D9;else{_tmp308=_tmp307->f1;}}_LL1D8: for(0;_tmp308
!= 0;_tmp308=_tmp308->tl){if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(
void*)_tmp308->hd))return 1;}return 0;_LL1D9: {struct Cyc_Absyn_RgnsEff_struct*
_tmp309=(struct Cyc_Absyn_RgnsEff_struct*)_tmp304;if(_tmp309->tag != 25)goto _LL1DB;
else{_tmp30A=(void*)_tmp309->f1;}}_LL1DA: {void*_tmp310=Cyc_Tcutil_rgns_of(
_tmp30A);void*_tmp312;_LL1E0: {struct Cyc_Absyn_RgnsEff_struct*_tmp311=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp310;if(_tmp311->tag != 25)goto _LL1E2;else{_tmp312=(void*)_tmp311->f1;}}_LL1E1:
if(!may_constrain_evars)return 0;{void*_tmp313=Cyc_Tcutil_compress(_tmp312);
struct Cyc_Core_Opt*_tmp315;struct Cyc_Core_Opt*_tmp316;struct Cyc_Core_Opt**
_tmp317;struct Cyc_Core_Opt*_tmp318;_LL1E5: {struct Cyc_Absyn_Evar_struct*_tmp314=(
struct Cyc_Absyn_Evar_struct*)_tmp313;if(_tmp314->tag != 1)goto _LL1E7;else{_tmp315=
_tmp314->f1;_tmp316=_tmp314->f2;_tmp317=(struct Cyc_Core_Opt**)& _tmp314->f2;
_tmp318=_tmp314->f4;}}_LL1E6: {void*_tmp319=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp318);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp318))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_struct _tmpFC9;struct Cyc_Absyn_VarType_struct*_tmpFC8;
struct Cyc_Absyn_VarType_struct _tmpFC7;void*_tmpFC6[2];struct Cyc_Absyn_JoinEff_struct*
_tmpFC5;void*_tmp31A=Cyc_Tcutil_dummy_fntype((void*)((_tmpFC5=_cycalloc(sizeof(*
_tmpFC5)),((_tmpFC5[0]=((_tmpFC9.tag=24,((_tmpFC9.f1=((_tmpFC6[1]=(void*)((
_tmpFC8=_cycalloc(sizeof(*_tmpFC8)),((_tmpFC8[0]=((_tmpFC7.tag=2,((_tmpFC7.f1=v,
_tmpFC7)))),_tmpFC8)))),((_tmpFC6[0]=_tmp319,((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFC6,sizeof(void*),2)))))),
_tmpFC9)))),_tmpFC5)))));{struct Cyc_Core_Opt*_tmpFCA;*_tmp317=((_tmpFCA=
_cycalloc(sizeof(*_tmpFCA)),((_tmpFCA->v=_tmp31A,_tmpFCA))));}return 1;};}_LL1E7:;
_LL1E8: return 0;_LL1E4:;};_LL1E2:;_LL1E3: return Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,_tmp310);_LL1DF:;}_LL1DB: {struct Cyc_Absyn_Evar_struct*
_tmp30B=(struct Cyc_Absyn_Evar_struct*)_tmp304;if(_tmp30B->tag != 1)goto _LL1DD;
else{_tmp30C=_tmp30B->f1;_tmp30D=_tmp30B->f2;_tmp30E=(struct Cyc_Core_Opt**)&
_tmp30B->f2;_tmp30F=_tmp30B->f4;}}_LL1DC: if(_tmp30C == 0  || (enum Cyc_Absyn_Kind)
_tmp30C->v != Cyc_Absyn_EffKind){const char*_tmpFCD;void*_tmpFCC;(_tmpFCC=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFCD="effect evar has wrong kind",
_tag_dyneither(_tmpFCD,sizeof(char),27))),_tag_dyneither(_tmpFCC,sizeof(void*),0)));}{
void*_tmp323=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp30F);if(
!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp30F))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpFE2;struct Cyc_List_List*_tmpFE1;struct Cyc_Absyn_VarType_struct _tmpFE0;struct
Cyc_Absyn_VarType_struct*_tmpFDF;struct Cyc_List_List*_tmpFDE;struct Cyc_Absyn_JoinEff_struct*
_tmpFDD;struct Cyc_Absyn_JoinEff_struct*_tmp324=(_tmpFDD=_cycalloc(sizeof(*
_tmpFDD)),((_tmpFDD[0]=((_tmpFE2.tag=24,((_tmpFE2.f1=((_tmpFDE=_cycalloc(sizeof(*
_tmpFDE)),((_tmpFDE->hd=_tmp323,((_tmpFDE->tl=((_tmpFE1=_cycalloc(sizeof(*
_tmpFE1)),((_tmpFE1->hd=(void*)((_tmpFDF=_cycalloc(sizeof(*_tmpFDF)),((_tmpFDF[0]=((
_tmpFE0.tag=2,((_tmpFE0.f1=v,_tmpFE0)))),_tmpFDF)))),((_tmpFE1->tl=0,_tmpFE1)))))),
_tmpFDE)))))),_tmpFE2)))),_tmpFDD)));{struct Cyc_Core_Opt*_tmpFE3;*_tmp30E=((
_tmpFE3=_cycalloc(sizeof(*_tmpFE3)),((_tmpFE3->v=(void*)_tmp324,_tmpFE3))));}
return 1;};};_LL1DD:;_LL1DE: return 0;_LL1D4:;};}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp32C=e;struct Cyc_List_List*_tmp32E;void*_tmp330;_LL1EA: {struct Cyc_Absyn_JoinEff_struct*
_tmp32D=(struct Cyc_Absyn_JoinEff_struct*)_tmp32C;if(_tmp32D->tag != 24)goto _LL1EC;
else{_tmp32E=_tmp32D->f1;}}_LL1EB: for(0;_tmp32E != 0;_tmp32E=_tmp32E->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp32E->hd))return 1;}return 0;_LL1EC: {struct Cyc_Absyn_RgnsEff_struct*
_tmp32F=(struct Cyc_Absyn_RgnsEff_struct*)_tmp32C;if(_tmp32F->tag != 25)goto _LL1EE;
else{_tmp330=(void*)_tmp32F->f1;}}_LL1ED: {void*_tmp332=Cyc_Tcutil_rgns_of(
_tmp330);void*_tmp334;_LL1F3: {struct Cyc_Absyn_RgnsEff_struct*_tmp333=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp332;if(_tmp333->tag != 25)goto _LL1F5;else{_tmp334=(void*)_tmp333->f1;}}_LL1F4:
return 0;_LL1F5:;_LL1F6: return Cyc_Tcutil_evar_in_effect(evar,_tmp332);_LL1F2:;}
_LL1EE: {struct Cyc_Absyn_Evar_struct*_tmp331=(struct Cyc_Absyn_Evar_struct*)
_tmp32C;if(_tmp331->tag != 1)goto _LL1F0;}_LL1EF: return evar == e;_LL1F0:;_LL1F1:
return 0;_LL1E9:;};}int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,
void*e2);int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
void*_tmp335=Cyc_Tcutil_compress(e1);struct Cyc_List_List*_tmp337;void*_tmp339;
struct Cyc_Absyn_Tvar*_tmp33B;void*_tmp33D;struct Cyc_Core_Opt*_tmp33F;struct Cyc_Core_Opt**
_tmp340;struct Cyc_Core_Opt*_tmp341;_LL1F8: {struct Cyc_Absyn_JoinEff_struct*
_tmp336=(struct Cyc_Absyn_JoinEff_struct*)_tmp335;if(_tmp336->tag != 24)goto _LL1FA;
else{_tmp337=_tmp336->f1;}}_LL1F9: for(0;_tmp337 != 0;_tmp337=_tmp337->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp337->hd,e2))return 0;}return 1;_LL1FA: {struct Cyc_Absyn_AccessEff_struct*
_tmp338=(struct Cyc_Absyn_AccessEff_struct*)_tmp335;if(_tmp338->tag != 23)goto
_LL1FC;else{_tmp339=(void*)_tmp338->f1;}}_LL1FB: return Cyc_Tcutil_region_in_effect(
0,_tmp339,e2) || may_constrain_evars  && Cyc_Tcutil_unify(_tmp339,(void*)& Cyc_Absyn_HeapRgn_val);
_LL1FC: {struct Cyc_Absyn_VarType_struct*_tmp33A=(struct Cyc_Absyn_VarType_struct*)
_tmp335;if(_tmp33A->tag != 2)goto _LL1FE;else{_tmp33B=_tmp33A->f1;}}_LL1FD: return
Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp33B,e2);_LL1FE: {struct Cyc_Absyn_RgnsEff_struct*
_tmp33C=(struct Cyc_Absyn_RgnsEff_struct*)_tmp335;if(_tmp33C->tag != 25)goto _LL200;
else{_tmp33D=(void*)_tmp33C->f1;}}_LL1FF: {void*_tmp342=Cyc_Tcutil_rgns_of(
_tmp33D);void*_tmp344;_LL205: {struct Cyc_Absyn_RgnsEff_struct*_tmp343=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp342;if(_tmp343->tag != 25)goto _LL207;else{_tmp344=(void*)_tmp343->f1;}}_LL206:
return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp344,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp344,Cyc_Absyn_sint_typ);_LL207:;
_LL208: return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp342,e2);_LL204:;}
_LL200: {struct Cyc_Absyn_Evar_struct*_tmp33E=(struct Cyc_Absyn_Evar_struct*)
_tmp335;if(_tmp33E->tag != 1)goto _LL202;else{_tmp33F=_tmp33E->f2;_tmp340=(struct
Cyc_Core_Opt**)& _tmp33E->f2;_tmp341=_tmp33E->f4;}}_LL201: if(!Cyc_Tcutil_evar_in_effect(
e1,e2)){struct Cyc_Core_Opt*_tmpFE4;*_tmp340=((_tmpFE4=_cycalloc(sizeof(*_tmpFE4)),((
_tmpFE4->v=Cyc_Absyn_empty_effect,_tmpFE4))));}return 1;_LL202:;_LL203: {const
char*_tmpFE8;void*_tmpFE7[1];struct Cyc_String_pa_struct _tmpFE6;(_tmpFE6.tag=0,((
_tmpFE6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e1)),((_tmpFE7[0]=& _tmpFE6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmpFE8="subset_effect: bad effect: %s",_tag_dyneither(
_tmpFE8,sizeof(char),30))),_tag_dyneither(_tmpFE7,sizeof(void*),1)))))));}_LL1F7:;}
static int Cyc_Tcutil_unify_effect(void*e1,void*e2);static int Cyc_Tcutil_unify_effect(
void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(e1);e2=Cyc_Tcutil_normalize_effect(
e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct
Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,
struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0 _tmp34A;void*_tmp34B;void*_tmp34C;struct _tuple0*_tmp349=(struct
_tuple0*)r1->hd;_tmp34A=*_tmp349;_tmp34B=_tmp34A.f1;_tmp34C=_tmp34A.f2;{int found=
_tmp34B == (void*)& Cyc_Absyn_HeapRgn_val;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 
0  && !found;r2=r2->tl){struct _tuple0 _tmp34E;void*_tmp34F;void*_tmp350;struct
_tuple0*_tmp34D=(struct _tuple0*)r2->hd;_tmp34E=*_tmp34D;_tmp34F=_tmp34E.f1;
_tmp350=_tmp34E.f2;if(Cyc_Tcutil_unify(_tmp34B,_tmp34F) && Cyc_Tcutil_unify(
_tmp34C,_tmp350)){found=1;break;}}}if(!found)return 0;};}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple18{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple19{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2);void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dyneither_ptr)_tag_dyneither(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp351=t1;struct Cyc_Core_Opt*_tmp353;struct Cyc_Core_Opt*
_tmp354;struct Cyc_Core_Opt**_tmp355;struct Cyc_Core_Opt*_tmp356;_LL20A: {struct
Cyc_Absyn_Evar_struct*_tmp352=(struct Cyc_Absyn_Evar_struct*)_tmp351;if(_tmp352->tag
!= 1)goto _LL20C;else{_tmp353=_tmp352->f1;_tmp354=_tmp352->f2;_tmp355=(struct Cyc_Core_Opt**)&
_tmp352->f2;_tmp356=_tmp352->f4;}}_LL20B: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp356))->v,t2);{enum 
Cyc_Absyn_Kind _tmp357=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp357,(
enum Cyc_Absyn_Kind)((struct Cyc_Core_Opt*)_check_null(_tmp353))->v)){{struct Cyc_Core_Opt*
_tmpFE9;*_tmp355=((_tmpFE9=_cycalloc(sizeof(*_tmpFE9)),((_tmpFE9->v=t2,_tmpFE9))));}
return;}else{{void*_tmp359=t2;struct Cyc_Core_Opt*_tmp35B;struct Cyc_Core_Opt**
_tmp35C;struct Cyc_Core_Opt*_tmp35D;struct Cyc_Absyn_PtrInfo _tmp35F;_LL20F: {
struct Cyc_Absyn_Evar_struct*_tmp35A=(struct Cyc_Absyn_Evar_struct*)_tmp359;if(
_tmp35A->tag != 1)goto _LL211;else{_tmp35B=_tmp35A->f2;_tmp35C=(struct Cyc_Core_Opt**)&
_tmp35A->f2;_tmp35D=_tmp35A->f4;}}_LL210: {struct Cyc_List_List*_tmp360=(struct
Cyc_List_List*)_tmp356->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp35D))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*
compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp360,(struct
Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmpFEA;Cyc_Tcutil_failure_reason=((_tmpFEA="(type variable would escape scope)",
_tag_dyneither(_tmpFEA,sizeof(char),35)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
if(Cyc_Tcutil_kind_leq((enum Cyc_Absyn_Kind)_tmp353->v,_tmp357)){{struct Cyc_Core_Opt*
_tmpFEB;*_tmp35C=((_tmpFEB=_cycalloc(sizeof(*_tmpFEB)),((_tmpFEB->v=t1,_tmpFEB))));}
return;}{const char*_tmpFEC;Cyc_Tcutil_failure_reason=((_tmpFEC="(kinds are incompatible)",
_tag_dyneither(_tmpFEC,sizeof(char),25)));}goto _LL20E;}_LL211:{struct Cyc_Absyn_PointerType_struct*
_tmp35E=(struct Cyc_Absyn_PointerType_struct*)_tmp359;if(_tmp35E->tag != 5)goto
_LL213;else{_tmp35F=_tmp35E->f1;}}if(!((enum Cyc_Absyn_Kind)_tmp353->v == Cyc_Absyn_BoxKind))
goto _LL213;_LL212: {union Cyc_Absyn_Constraint*_tmp364=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp35F.ptr_atts).bounds);{
union Cyc_Absyn_Constraint*_tmp365=_tmp364;union Cyc_Absyn_Constraint _tmp366;int
_tmp367;_LL216: _tmp366=*_tmp365;if((_tmp366.No_constr).tag != 3)goto _LL218;
_tmp367=(int)(_tmp366.No_constr).val;_LL217:{struct _union_Constraint_Eq_constr*
_tmpFED;(_tmpFED=& _tmp364->Eq_constr,((_tmpFED->tag=1,_tmpFED->val=Cyc_Absyn_bounds_one)));}{
struct Cyc_Core_Opt*_tmpFEE;*_tmp355=((_tmpFEE=_cycalloc(sizeof(*_tmpFEE)),((
_tmpFEE->v=t2,_tmpFEE))));}return;_LL218:;_LL219: goto _LL215;_LL215:;}goto _LL20E;}
_LL213:;_LL214: goto _LL20E;_LL20E:;}{const char*_tmpFEF;Cyc_Tcutil_failure_reason=((
_tmpFEF="(kinds are incompatible)",_tag_dyneither(_tmpFEF,sizeof(char),25)));}(
int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL20C:;_LL20D: goto _LL209;_LL209:;}{
struct _tuple0 _tmpFF0;struct _tuple0 _tmp36C=(_tmpFF0.f1=t2,((_tmpFF0.f2=t1,_tmpFF0)));
void*_tmp36D;void*_tmp36F;void*_tmp371;void*_tmp373;struct Cyc_Absyn_Tvar*_tmp375;
void*_tmp376;struct Cyc_Absyn_Tvar*_tmp378;void*_tmp379;struct Cyc_Absyn_AggrInfo
_tmp37B;union Cyc_Absyn_AggrInfoU _tmp37C;struct Cyc_List_List*_tmp37D;void*_tmp37E;
struct Cyc_Absyn_AggrInfo _tmp380;union Cyc_Absyn_AggrInfoU _tmp381;struct Cyc_List_List*
_tmp382;void*_tmp383;struct _tuple2*_tmp385;void*_tmp386;struct _tuple2*_tmp388;
void*_tmp389;struct Cyc_List_List*_tmp38B;void*_tmp38C;struct Cyc_List_List*
_tmp38E;void*_tmp38F;struct Cyc_Absyn_DatatypeInfo _tmp391;union Cyc_Absyn_DatatypeInfoU
_tmp392;struct Cyc_Absyn_Datatypedecl**_tmp393;struct Cyc_Absyn_Datatypedecl*
_tmp394;struct Cyc_List_List*_tmp395;void*_tmp396;struct Cyc_Absyn_DatatypeInfo
_tmp398;union Cyc_Absyn_DatatypeInfoU _tmp399;struct Cyc_Absyn_Datatypedecl**
_tmp39A;struct Cyc_Absyn_Datatypedecl*_tmp39B;struct Cyc_List_List*_tmp39C;void*
_tmp39D;struct Cyc_Absyn_DatatypeFieldInfo _tmp39F;union Cyc_Absyn_DatatypeFieldInfoU
_tmp3A0;struct _tuple3 _tmp3A1;struct Cyc_Absyn_Datatypedecl*_tmp3A2;struct Cyc_Absyn_Datatypefield*
_tmp3A3;struct Cyc_List_List*_tmp3A4;void*_tmp3A5;struct Cyc_Absyn_DatatypeFieldInfo
_tmp3A7;union Cyc_Absyn_DatatypeFieldInfoU _tmp3A8;struct _tuple3 _tmp3A9;struct Cyc_Absyn_Datatypedecl*
_tmp3AA;struct Cyc_Absyn_Datatypefield*_tmp3AB;struct Cyc_List_List*_tmp3AC;void*
_tmp3AD;struct Cyc_Absyn_PtrInfo _tmp3AF;void*_tmp3B0;struct Cyc_Absyn_Tqual _tmp3B1;
struct Cyc_Absyn_PtrAtts _tmp3B2;void*_tmp3B3;union Cyc_Absyn_Constraint*_tmp3B4;
union Cyc_Absyn_Constraint*_tmp3B5;union Cyc_Absyn_Constraint*_tmp3B6;void*_tmp3B7;
struct Cyc_Absyn_PtrInfo _tmp3B9;void*_tmp3BA;struct Cyc_Absyn_Tqual _tmp3BB;struct
Cyc_Absyn_PtrAtts _tmp3BC;void*_tmp3BD;union Cyc_Absyn_Constraint*_tmp3BE;union Cyc_Absyn_Constraint*
_tmp3BF;union Cyc_Absyn_Constraint*_tmp3C0;void*_tmp3C1;enum Cyc_Absyn_Sign
_tmp3C3;enum Cyc_Absyn_Size_of _tmp3C4;void*_tmp3C5;enum Cyc_Absyn_Sign _tmp3C7;
enum Cyc_Absyn_Size_of _tmp3C8;void*_tmp3C9;void*_tmp3CB;void*_tmp3CD;int _tmp3CF;
void*_tmp3D0;int _tmp3D2;void*_tmp3D3;void*_tmp3D5;void*_tmp3D6;void*_tmp3D8;void*
_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DB;void*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DE;
void*_tmp3DF;struct Cyc_Absyn_ArrayInfo _tmp3E1;void*_tmp3E2;struct Cyc_Absyn_Tqual
_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E4;union Cyc_Absyn_Constraint*_tmp3E5;void*
_tmp3E6;struct Cyc_Absyn_ArrayInfo _tmp3E8;void*_tmp3E9;struct Cyc_Absyn_Tqual
_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EB;union Cyc_Absyn_Constraint*_tmp3EC;void*
_tmp3ED;struct Cyc_Absyn_FnInfo _tmp3EF;struct Cyc_List_List*_tmp3F0;struct Cyc_Core_Opt*
_tmp3F1;void*_tmp3F2;struct Cyc_List_List*_tmp3F3;int _tmp3F4;struct Cyc_Absyn_VarargInfo*
_tmp3F5;struct Cyc_List_List*_tmp3F6;struct Cyc_List_List*_tmp3F7;void*_tmp3F8;
struct Cyc_Absyn_FnInfo _tmp3FA;struct Cyc_List_List*_tmp3FB;struct Cyc_Core_Opt*
_tmp3FC;void*_tmp3FD;struct Cyc_List_List*_tmp3FE;int _tmp3FF;struct Cyc_Absyn_VarargInfo*
_tmp400;struct Cyc_List_List*_tmp401;struct Cyc_List_List*_tmp402;void*_tmp403;
struct Cyc_List_List*_tmp405;void*_tmp406;struct Cyc_List_List*_tmp408;void*
_tmp409;enum Cyc_Absyn_AggrKind _tmp40B;struct Cyc_List_List*_tmp40C;void*_tmp40D;
enum Cyc_Absyn_AggrKind _tmp40F;struct Cyc_List_List*_tmp410;void*_tmp411;void*
_tmp413;void*_tmp415;void*_tmp417;void*_tmp419;void*_tmp41B;void*_tmp41C;void*
_tmp41E;void*_tmp41F;void*_tmp421;void*_tmp422;void*_tmp423;void*_tmp425;void*
_tmp426;void*_tmp427;void*_tmp429;void*_tmp42B;void*_tmp42D;void*_tmp42F;void*
_tmp431;_LL21B: _tmp36D=_tmp36C.f1;{struct Cyc_Absyn_Evar_struct*_tmp36E=(struct
Cyc_Absyn_Evar_struct*)_tmp36D;if(_tmp36E->tag != 1)goto _LL21D;};_LL21C: Cyc_Tcutil_unify_it(
t2,t1);return;_LL21D: _tmp36F=_tmp36C.f1;{struct Cyc_Absyn_VoidType_struct*_tmp370=(
struct Cyc_Absyn_VoidType_struct*)_tmp36F;if(_tmp370->tag != 0)goto _LL21F;};
_tmp371=_tmp36C.f2;{struct Cyc_Absyn_VoidType_struct*_tmp372=(struct Cyc_Absyn_VoidType_struct*)
_tmp371;if(_tmp372->tag != 0)goto _LL21F;};_LL21E: return;_LL21F: _tmp373=_tmp36C.f1;{
struct Cyc_Absyn_VarType_struct*_tmp374=(struct Cyc_Absyn_VarType_struct*)_tmp373;
if(_tmp374->tag != 2)goto _LL221;else{_tmp375=_tmp374->f1;}};_tmp376=_tmp36C.f2;{
struct Cyc_Absyn_VarType_struct*_tmp377=(struct Cyc_Absyn_VarType_struct*)_tmp376;
if(_tmp377->tag != 2)goto _LL221;else{_tmp378=_tmp377->f1;}};_LL220: {struct
_dyneither_ptr*_tmp433=_tmp375->name;struct _dyneither_ptr*_tmp434=_tmp378->name;
int _tmp435=_tmp375->identity;int _tmp436=_tmp378->identity;if(_tmp436 == _tmp435)
return;{const char*_tmpFF1;Cyc_Tcutil_failure_reason=((_tmpFF1="(variable types are not the same)",
_tag_dyneither(_tmpFF1,sizeof(char),34)));}goto _LL21A;}_LL221: _tmp379=_tmp36C.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp37A=(struct Cyc_Absyn_AggrType_struct*)
_tmp379;if(_tmp37A->tag != 12)goto _LL223;else{_tmp37B=_tmp37A->f1;_tmp37C=_tmp37B.aggr_info;
_tmp37D=_tmp37B.targs;}};_tmp37E=_tmp36C.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp37F=(struct Cyc_Absyn_AggrType_struct*)_tmp37E;if(_tmp37F->tag != 12)goto
_LL223;else{_tmp380=_tmp37F->f1;_tmp381=_tmp380.aggr_info;_tmp382=_tmp380.targs;}};
_LL222: {enum Cyc_Absyn_AggrKind _tmp439;struct _tuple2*_tmp43A;struct _tuple12
_tmp438=Cyc_Absyn_aggr_kinded_name(_tmp381);_tmp439=_tmp438.f1;_tmp43A=_tmp438.f2;{
enum Cyc_Absyn_AggrKind _tmp43C;struct _tuple2*_tmp43D;struct _tuple12 _tmp43B=Cyc_Absyn_aggr_kinded_name(
_tmp37C);_tmp43C=_tmp43B.f1;_tmp43D=_tmp43B.f2;if(_tmp439 != _tmp43C){{const char*
_tmpFF2;Cyc_Tcutil_failure_reason=((_tmpFF2="(struct and union type)",
_tag_dyneither(_tmpFF2,sizeof(char),24)));}goto _LL21A;}if(Cyc_Absyn_qvar_cmp(
_tmp43A,_tmp43D)!= 0){{const char*_tmpFF3;Cyc_Tcutil_failure_reason=((_tmpFF3="(different type name)",
_tag_dyneither(_tmpFF3,sizeof(char),22)));}goto _LL21A;}Cyc_Tcutil_unify_list(
_tmp382,_tmp37D);return;};}_LL223: _tmp383=_tmp36C.f1;{struct Cyc_Absyn_EnumType_struct*
_tmp384=(struct Cyc_Absyn_EnumType_struct*)_tmp383;if(_tmp384->tag != 14)goto
_LL225;else{_tmp385=_tmp384->f1;}};_tmp386=_tmp36C.f2;{struct Cyc_Absyn_EnumType_struct*
_tmp387=(struct Cyc_Absyn_EnumType_struct*)_tmp386;if(_tmp387->tag != 14)goto
_LL225;else{_tmp388=_tmp387->f1;}};_LL224: if(Cyc_Absyn_qvar_cmp(_tmp385,_tmp388)
== 0)return;{const char*_tmpFF4;Cyc_Tcutil_failure_reason=((_tmpFF4="(different enum types)",
_tag_dyneither(_tmpFF4,sizeof(char),23)));}goto _LL21A;_LL225: _tmp389=_tmp36C.f1;{
struct Cyc_Absyn_AnonEnumType_struct*_tmp38A=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp389;if(_tmp38A->tag != 15)goto _LL227;else{_tmp38B=_tmp38A->f1;}};_tmp38C=
_tmp36C.f2;{struct Cyc_Absyn_AnonEnumType_struct*_tmp38D=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp38C;if(_tmp38D->tag != 15)goto _LL227;else{_tmp38E=_tmp38D->f1;}};_LL226: {int
bad=0;for(0;_tmp38B != 0  && _tmp38E != 0;(_tmp38B=_tmp38B->tl,_tmp38E=_tmp38E->tl)){
struct Cyc_Absyn_Enumfield*_tmp441=(struct Cyc_Absyn_Enumfield*)_tmp38B->hd;struct
Cyc_Absyn_Enumfield*_tmp442=(struct Cyc_Absyn_Enumfield*)_tmp38E->hd;if(Cyc_Absyn_qvar_cmp(
_tmp441->name,_tmp442->name)!= 0){{const char*_tmpFF5;Cyc_Tcutil_failure_reason=((
_tmpFF5="(different names for enum fields)",_tag_dyneither(_tmpFF5,sizeof(char),
34)));}bad=1;break;}if(_tmp441->tag == _tmp442->tag)continue;if(_tmp441->tag == 0
 || _tmp442->tag == 0){{const char*_tmpFF6;Cyc_Tcutil_failure_reason=((_tmpFF6="(different tag values for enum fields)",
_tag_dyneither(_tmpFF6,sizeof(char),39)));}bad=1;break;}if(!Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp441->tag),(struct Cyc_Absyn_Exp*)_check_null(
_tmp442->tag))){{const char*_tmpFF7;Cyc_Tcutil_failure_reason=((_tmpFF7="(different tag values for enum fields)",
_tag_dyneither(_tmpFF7,sizeof(char),39)));}bad=1;break;}}if(bad)goto _LL21A;if(
_tmp38B == 0  && _tmp38E == 0)return;{const char*_tmpFF8;Cyc_Tcutil_failure_reason=((
_tmpFF8="(different number of fields for enums)",_tag_dyneither(_tmpFF8,sizeof(
char),39)));}goto _LL21A;}_LL227: _tmp38F=_tmp36C.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp390=(struct Cyc_Absyn_DatatypeType_struct*)_tmp38F;if(_tmp390->tag != 3)goto
_LL229;else{_tmp391=_tmp390->f1;_tmp392=_tmp391.datatype_info;if((_tmp392.KnownDatatype).tag
!= 2)goto _LL229;_tmp393=(struct Cyc_Absyn_Datatypedecl**)(_tmp392.KnownDatatype).val;
_tmp394=*_tmp393;_tmp395=_tmp391.targs;}};_tmp396=_tmp36C.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp397=(struct Cyc_Absyn_DatatypeType_struct*)_tmp396;if(_tmp397->tag != 3)goto
_LL229;else{_tmp398=_tmp397->f1;_tmp399=_tmp398.datatype_info;if((_tmp399.KnownDatatype).tag
!= 2)goto _LL229;_tmp39A=(struct Cyc_Absyn_Datatypedecl**)(_tmp399.KnownDatatype).val;
_tmp39B=*_tmp39A;_tmp39C=_tmp398.targs;}};_LL228: if(_tmp394 == _tmp39B  || Cyc_Absyn_qvar_cmp(
_tmp394->name,_tmp39B->name)== 0){Cyc_Tcutil_unify_list(_tmp39C,_tmp395);return;}
goto _LL21A;_LL229: _tmp39D=_tmp36C.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp39E=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp39D;if(_tmp39E->tag != 4)
goto _LL22B;else{_tmp39F=_tmp39E->f1;_tmp3A0=_tmp39F.field_info;if((_tmp3A0.KnownDatatypefield).tag
!= 2)goto _LL22B;_tmp3A1=(struct _tuple3)(_tmp3A0.KnownDatatypefield).val;_tmp3A2=
_tmp3A1.f1;_tmp3A3=_tmp3A1.f2;_tmp3A4=_tmp39F.targs;}};_tmp3A5=_tmp36C.f2;{
struct Cyc_Absyn_DatatypeFieldType_struct*_tmp3A6=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp3A5;if(_tmp3A6->tag != 4)goto _LL22B;else{_tmp3A7=_tmp3A6->f1;_tmp3A8=_tmp3A7.field_info;
if((_tmp3A8.KnownDatatypefield).tag != 2)goto _LL22B;_tmp3A9=(struct _tuple3)(
_tmp3A8.KnownDatatypefield).val;_tmp3AA=_tmp3A9.f1;_tmp3AB=_tmp3A9.f2;_tmp3AC=
_tmp3A7.targs;}};_LL22A: if((_tmp3A2 == _tmp3AA  || Cyc_Absyn_qvar_cmp(_tmp3A2->name,
_tmp3AA->name)== 0) && (_tmp3A3 == _tmp3AB  || Cyc_Absyn_qvar_cmp(_tmp3A3->name,
_tmp3AB->name)== 0)){Cyc_Tcutil_unify_list(_tmp3AC,_tmp3A4);return;}{const char*
_tmpFF9;Cyc_Tcutil_failure_reason=((_tmpFF9="(different datatype field types)",
_tag_dyneither(_tmpFF9,sizeof(char),33)));}goto _LL21A;_LL22B: _tmp3AD=_tmp36C.f1;{
struct Cyc_Absyn_PointerType_struct*_tmp3AE=(struct Cyc_Absyn_PointerType_struct*)
_tmp3AD;if(_tmp3AE->tag != 5)goto _LL22D;else{_tmp3AF=_tmp3AE->f1;_tmp3B0=_tmp3AF.elt_typ;
_tmp3B1=_tmp3AF.elt_tq;_tmp3B2=_tmp3AF.ptr_atts;_tmp3B3=_tmp3B2.rgn;_tmp3B4=
_tmp3B2.nullable;_tmp3B5=_tmp3B2.bounds;_tmp3B6=_tmp3B2.zero_term;}};_tmp3B7=
_tmp36C.f2;{struct Cyc_Absyn_PointerType_struct*_tmp3B8=(struct Cyc_Absyn_PointerType_struct*)
_tmp3B7;if(_tmp3B8->tag != 5)goto _LL22D;else{_tmp3B9=_tmp3B8->f1;_tmp3BA=_tmp3B9.elt_typ;
_tmp3BB=_tmp3B9.elt_tq;_tmp3BC=_tmp3B9.ptr_atts;_tmp3BD=_tmp3BC.rgn;_tmp3BE=
_tmp3BC.nullable;_tmp3BF=_tmp3BC.bounds;_tmp3C0=_tmp3BC.zero_term;}};_LL22C: Cyc_Tcutil_unify_it(
_tmp3BA,_tmp3B0);Cyc_Tcutil_unify_it(_tmp3B3,_tmp3BD);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{const char*_tmpFFA;((void(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))
Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3C0,_tmp3B6,((_tmpFFA="(not both zero terminated)",
_tag_dyneither(_tmpFFA,sizeof(char),27))));}Cyc_Tcutil_unify_tqual(_tmp3BB,
_tmp3BA,_tmp3B1,_tmp3B0);{const char*_tmpFFB;((void(*)(int(*cmp)(void*,void*),
union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr
reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp3BF,_tmp3B5,((
_tmpFFB="(different pointer bounds)",_tag_dyneither(_tmpFFB,sizeof(char),27))));}{
void*_tmp44A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmp3BF);_LL256: {struct Cyc_Absyn_DynEither_b_struct*
_tmp44B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp44A;if(_tmp44B->tag != 0)goto
_LL258;}_LL257: return;_LL258:;_LL259: goto _LL255;_LL255:;}{const char*_tmpFFC;((
void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp3BE,_tmp3B4,((_tmpFFC="(incompatible pointer types)",_tag_dyneither(_tmpFFC,
sizeof(char),29))));}return;_LL22D: _tmp3C1=_tmp36C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp3C2=(struct Cyc_Absyn_IntType_struct*)_tmp3C1;if(_tmp3C2->tag != 6)goto _LL22F;
else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C2->f2;}};_tmp3C5=_tmp36C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp3C6=(struct Cyc_Absyn_IntType_struct*)_tmp3C5;if(_tmp3C6->tag != 6)goto _LL22F;
else{_tmp3C7=_tmp3C6->f1;_tmp3C8=_tmp3C6->f2;}};_LL22E: if(_tmp3C7 == _tmp3C3  && ((
_tmp3C8 == _tmp3C4  || _tmp3C8 == Cyc_Absyn_Int_sz  && _tmp3C4 == Cyc_Absyn_Long_sz)
 || _tmp3C8 == Cyc_Absyn_Long_sz  && _tmp3C4 == Cyc_Absyn_Int_sz))return;{const char*
_tmpFFD;Cyc_Tcutil_failure_reason=((_tmpFFD="(different integral types)",
_tag_dyneither(_tmpFFD,sizeof(char),27)));}goto _LL21A;_LL22F: _tmp3C9=_tmp36C.f1;{
struct Cyc_Absyn_FloatType_struct*_tmp3CA=(struct Cyc_Absyn_FloatType_struct*)
_tmp3C9;if(_tmp3CA->tag != 7)goto _LL231;};_tmp3CB=_tmp36C.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp3CC=(struct Cyc_Absyn_FloatType_struct*)_tmp3CB;if(_tmp3CC->tag != 7)goto
_LL231;};_LL230: return;_LL231: _tmp3CD=_tmp36C.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp3CE=(struct Cyc_Absyn_DoubleType_struct*)_tmp3CD;if(_tmp3CE->tag != 8)goto
_LL233;else{_tmp3CF=_tmp3CE->f1;}};_tmp3D0=_tmp36C.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp3D1=(struct Cyc_Absyn_DoubleType_struct*)_tmp3D0;if(_tmp3D1->tag != 8)goto
_LL233;else{_tmp3D2=_tmp3D1->f1;}};_LL232: if(_tmp3CF == _tmp3D2)return;goto _LL21A;
_LL233: _tmp3D3=_tmp36C.f1;{struct Cyc_Absyn_TagType_struct*_tmp3D4=(struct Cyc_Absyn_TagType_struct*)
_tmp3D3;if(_tmp3D4->tag != 20)goto _LL235;else{_tmp3D5=(void*)_tmp3D4->f1;}};
_tmp3D6=_tmp36C.f2;{struct Cyc_Absyn_TagType_struct*_tmp3D7=(struct Cyc_Absyn_TagType_struct*)
_tmp3D6;if(_tmp3D7->tag != 20)goto _LL235;else{_tmp3D8=(void*)_tmp3D7->f1;}};
_LL234: Cyc_Tcutil_unify_it(_tmp3D5,_tmp3D8);return;_LL235: _tmp3D9=_tmp36C.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp3DA=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3D9;if(_tmp3DA->tag != 19)goto _LL237;else{_tmp3DB=_tmp3DA->f1;}};_tmp3DC=
_tmp36C.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp3DD=(struct Cyc_Absyn_ValueofType_struct*)
_tmp3DC;if(_tmp3DD->tag != 19)goto _LL237;else{_tmp3DE=_tmp3DD->f1;}};_LL236: if(!
Cyc_Evexp_same_const_exp(_tmp3DB,_tmp3DE)){{const char*_tmpFFE;Cyc_Tcutil_failure_reason=((
_tmpFFE="(cannot prove expressions are the same)",_tag_dyneither(_tmpFFE,sizeof(
char),40)));}goto _LL21A;}return;_LL237: _tmp3DF=_tmp36C.f1;{struct Cyc_Absyn_ArrayType_struct*
_tmp3E0=(struct Cyc_Absyn_ArrayType_struct*)_tmp3DF;if(_tmp3E0->tag != 9)goto
_LL239;else{_tmp3E1=_tmp3E0->f1;_tmp3E2=_tmp3E1.elt_type;_tmp3E3=_tmp3E1.tq;
_tmp3E4=_tmp3E1.num_elts;_tmp3E5=_tmp3E1.zero_term;}};_tmp3E6=_tmp36C.f2;{struct
Cyc_Absyn_ArrayType_struct*_tmp3E7=(struct Cyc_Absyn_ArrayType_struct*)_tmp3E6;
if(_tmp3E7->tag != 9)goto _LL239;else{_tmp3E8=_tmp3E7->f1;_tmp3E9=_tmp3E8.elt_type;
_tmp3EA=_tmp3E8.tq;_tmp3EB=_tmp3E8.num_elts;_tmp3EC=_tmp3E8.zero_term;}};_LL238:
Cyc_Tcutil_unify_it(_tmp3E9,_tmp3E2);Cyc_Tcutil_unify_tqual(_tmp3EA,_tmp3E9,
_tmp3E3,_tmp3E2);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const char*
_tmpFFF;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp3E5,_tmp3EC,((_tmpFFF="(not both zero terminated)",_tag_dyneither(_tmpFFF,
sizeof(char),27))));}if(_tmp3E4 == _tmp3EB)return;if(_tmp3E4 == 0  || _tmp3EB == 0)
goto _LL21A;if(Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp3E4,(struct Cyc_Absyn_Exp*)
_tmp3EB))return;{const char*_tmp1000;Cyc_Tcutil_failure_reason=((_tmp1000="(different array sizes)",
_tag_dyneither(_tmp1000,sizeof(char),24)));}goto _LL21A;_LL239: _tmp3ED=_tmp36C.f1;{
struct Cyc_Absyn_FnType_struct*_tmp3EE=(struct Cyc_Absyn_FnType_struct*)_tmp3ED;
if(_tmp3EE->tag != 10)goto _LL23B;else{_tmp3EF=_tmp3EE->f1;_tmp3F0=_tmp3EF.tvars;
_tmp3F1=_tmp3EF.effect;_tmp3F2=_tmp3EF.ret_typ;_tmp3F3=_tmp3EF.args;_tmp3F4=
_tmp3EF.c_varargs;_tmp3F5=_tmp3EF.cyc_varargs;_tmp3F6=_tmp3EF.rgn_po;_tmp3F7=
_tmp3EF.attributes;}};_tmp3F8=_tmp36C.f2;{struct Cyc_Absyn_FnType_struct*_tmp3F9=(
struct Cyc_Absyn_FnType_struct*)_tmp3F8;if(_tmp3F9->tag != 10)goto _LL23B;else{
_tmp3FA=_tmp3F9->f1;_tmp3FB=_tmp3FA.tvars;_tmp3FC=_tmp3FA.effect;_tmp3FD=_tmp3FA.ret_typ;
_tmp3FE=_tmp3FA.args;_tmp3FF=_tmp3FA.c_varargs;_tmp400=_tmp3FA.cyc_varargs;
_tmp401=_tmp3FA.rgn_po;_tmp402=_tmp3FA.attributes;}};_LL23A: {int done=0;struct
_RegionHandle _tmp451=_new_region("rgn");struct _RegionHandle*rgn=& _tmp451;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp3FB != 0){if(_tmp3F0 == 0){{
const char*_tmp1001;Cyc_Tcutil_failure_reason=((_tmp1001="(second function type has too few type variables)",
_tag_dyneither(_tmp1001,sizeof(char),50)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
void*_tmp453=((struct Cyc_Absyn_Tvar*)_tmp3FB->hd)->kind;void*_tmp454=((struct Cyc_Absyn_Tvar*)
_tmp3F0->hd)->kind;if(!Cyc_Tcutil_unify_kindbound(_tmp453,_tmp454)){{const char*
_tmp1007;void*_tmp1006[3];struct Cyc_String_pa_struct _tmp1005;struct Cyc_String_pa_struct
_tmp1004;struct Cyc_String_pa_struct _tmp1003;Cyc_Tcutil_failure_reason=(struct
_dyneither_ptr)((_tmp1003.tag=0,((_tmp1003.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp3F0->hd,Cyc_Absyn_BoxKind))),((_tmp1004.tag=0,((_tmp1004.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp3FB->hd,Cyc_Absyn_BoxKind))),((_tmp1005.tag=0,((
_tmp1005.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)_tmp3FB->hd)),((_tmp1006[0]=& _tmp1005,((_tmp1006[1]=&
_tmp1004,((_tmp1006[2]=& _tmp1003,Cyc_aprintf(((_tmp1007="(type var %s has different kinds %s and %s)",
_tag_dyneither(_tmp1007,sizeof(char),44))),_tag_dyneither(_tmp1006,sizeof(void*),
3))))))))))))))))))));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{struct
_tuple14*_tmp1011;struct Cyc_Absyn_VarType_struct _tmp1010;struct Cyc_Absyn_VarType_struct*
_tmp100F;struct Cyc_List_List*_tmp100E;inst=((_tmp100E=_region_malloc(rgn,sizeof(*
_tmp100E)),((_tmp100E->hd=((_tmp1011=_region_malloc(rgn,sizeof(*_tmp1011)),((
_tmp1011->f1=(struct Cyc_Absyn_Tvar*)_tmp3F0->hd,((_tmp1011->f2=(void*)((_tmp100F=
_cycalloc(sizeof(*_tmp100F)),((_tmp100F[0]=((_tmp1010.tag=2,((_tmp1010.f1=(
struct Cyc_Absyn_Tvar*)_tmp3FB->hd,_tmp1010)))),_tmp100F)))),_tmp1011)))))),((
_tmp100E->tl=inst,_tmp100E))))));}_tmp3FB=_tmp3FB->tl;_tmp3F0=_tmp3F0->tl;};}if(
_tmp3F0 != 0){{const char*_tmp1012;Cyc_Tcutil_failure_reason=((_tmp1012="(second function type has too many type variables)",
_tag_dyneither(_tmp1012,sizeof(char),51)));}_npop_handler(0);goto _LL21A;}if(inst
!= 0){{struct Cyc_Absyn_FnType_struct _tmp101E;struct Cyc_Absyn_FnInfo _tmp101D;
struct Cyc_Absyn_FnType_struct*_tmp101C;struct Cyc_Absyn_FnType_struct _tmp1018;
struct Cyc_Absyn_FnInfo _tmp1017;struct Cyc_Absyn_FnType_struct*_tmp1016;Cyc_Tcutil_unify_it((
void*)((_tmp1016=_cycalloc(sizeof(*_tmp1016)),((_tmp1016[0]=((_tmp1018.tag=10,((
_tmp1018.f1=((_tmp1017.tvars=0,((_tmp1017.effect=_tmp3FC,((_tmp1017.ret_typ=
_tmp3FD,((_tmp1017.args=_tmp3FE,((_tmp1017.c_varargs=_tmp3FF,((_tmp1017.cyc_varargs=
_tmp400,((_tmp1017.rgn_po=_tmp401,((_tmp1017.attributes=_tmp402,_tmp1017)))))))))))))))),
_tmp1018)))),_tmp1016)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmp101C=
_cycalloc(sizeof(*_tmp101C)),((_tmp101C[0]=((_tmp101E.tag=10,((_tmp101E.f1=((
_tmp101D.tvars=0,((_tmp101D.effect=_tmp3F1,((_tmp101D.ret_typ=_tmp3F2,((_tmp101D.args=
_tmp3F3,((_tmp101D.c_varargs=_tmp3F4,((_tmp101D.cyc_varargs=_tmp3F5,((_tmp101D.rgn_po=
_tmp3F6,((_tmp101D.attributes=_tmp3F7,_tmp101D)))))))))))))))),_tmp101E)))),
_tmp101C))))));}done=1;}}if(done){_npop_handler(0);return;}Cyc_Tcutil_unify_it(
_tmp3FD,_tmp3F2);for(0;_tmp3FE != 0  && _tmp3F3 != 0;(_tmp3FE=_tmp3FE->tl,_tmp3F3=
_tmp3F3->tl)){struct Cyc_Absyn_Tqual _tmp466;void*_tmp467;struct _tuple9 _tmp465=*((
struct _tuple9*)_tmp3FE->hd);_tmp466=_tmp465.f2;_tmp467=_tmp465.f3;{struct Cyc_Absyn_Tqual
_tmp469;void*_tmp46A;struct _tuple9 _tmp468=*((struct _tuple9*)_tmp3F3->hd);_tmp469=
_tmp468.f2;_tmp46A=_tmp468.f3;Cyc_Tcutil_unify_it(_tmp467,_tmp46A);Cyc_Tcutil_unify_tqual(
_tmp466,_tmp467,_tmp469,_tmp46A);};}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp3FE != 0  || _tmp3F3 != 0){{const char*_tmp101F;Cyc_Tcutil_failure_reason=((
_tmp101F="(function types have different number of arguments)",_tag_dyneither(
_tmp101F,sizeof(char),52)));}_npop_handler(0);goto _LL21A;}if(_tmp3FF != _tmp3F4){{
const char*_tmp1020;Cyc_Tcutil_failure_reason=((_tmp1020="(only one function type takes C varargs)",
_tag_dyneither(_tmp1020,sizeof(char),41)));}_npop_handler(0);goto _LL21A;}{int
bad_cyc_vararg=0;{struct _tuple18 _tmp1021;struct _tuple18 _tmp46E=(_tmp1021.f1=
_tmp400,((_tmp1021.f2=_tmp3F5,_tmp1021)));struct Cyc_Absyn_VarargInfo*_tmp46F;
struct Cyc_Absyn_VarargInfo*_tmp470;struct Cyc_Absyn_VarargInfo*_tmp471;struct Cyc_Absyn_VarargInfo*
_tmp472;struct Cyc_Absyn_VarargInfo*_tmp473;struct Cyc_Absyn_VarargInfo _tmp474;
struct Cyc_Core_Opt*_tmp475;struct Cyc_Absyn_Tqual _tmp476;void*_tmp477;int _tmp478;
struct Cyc_Absyn_VarargInfo*_tmp479;struct Cyc_Absyn_VarargInfo _tmp47A;struct Cyc_Core_Opt*
_tmp47B;struct Cyc_Absyn_Tqual _tmp47C;void*_tmp47D;int _tmp47E;_LL25B: _tmp46F=
_tmp46E.f1;if(_tmp46F != 0)goto _LL25D;_tmp470=_tmp46E.f2;if(_tmp470 != 0)goto
_LL25D;_LL25C: goto _LL25A;_LL25D: _tmp471=_tmp46E.f1;if(_tmp471 != 0)goto _LL25F;
_LL25E: goto _LL260;_LL25F: _tmp472=_tmp46E.f2;if(_tmp472 != 0)goto _LL261;_LL260:
bad_cyc_vararg=1;{const char*_tmp1022;Cyc_Tcutil_failure_reason=((_tmp1022="(only one function type takes varargs)",
_tag_dyneither(_tmp1022,sizeof(char),39)));}goto _LL25A;_LL261: _tmp473=_tmp46E.f1;
if(_tmp473 == 0)goto _LL25A;_tmp474=*_tmp473;_tmp475=_tmp474.name;_tmp476=_tmp474.tq;
_tmp477=_tmp474.type;_tmp478=_tmp474.inject;_tmp479=_tmp46E.f2;if(_tmp479 == 0)
goto _LL25A;_tmp47A=*_tmp479;_tmp47B=_tmp47A.name;_tmp47C=_tmp47A.tq;_tmp47D=
_tmp47A.type;_tmp47E=_tmp47A.inject;_LL262: Cyc_Tcutil_unify_it(_tmp477,_tmp47D);
Cyc_Tcutil_unify_tqual(_tmp476,_tmp477,_tmp47C,_tmp47D);if(_tmp478 != _tmp47E){
bad_cyc_vararg=1;{const char*_tmp1023;Cyc_Tcutil_failure_reason=((_tmp1023="(only one function type injects varargs)",
_tag_dyneither(_tmp1023,sizeof(char),41)));};}goto _LL25A;_LL25A:;}if(
bad_cyc_vararg){_npop_handler(0);goto _LL21A;}{int bad_effect=0;{struct _tuple19
_tmp1024;struct _tuple19 _tmp482=(_tmp1024.f1=_tmp3FC,((_tmp1024.f2=_tmp3F1,
_tmp1024)));struct Cyc_Core_Opt*_tmp483;struct Cyc_Core_Opt*_tmp484;struct Cyc_Core_Opt*
_tmp485;struct Cyc_Core_Opt*_tmp486;_LL264: _tmp483=_tmp482.f1;if(_tmp483 != 0)goto
_LL266;_tmp484=_tmp482.f2;if(_tmp484 != 0)goto _LL266;_LL265: goto _LL263;_LL266:
_tmp485=_tmp482.f1;if(_tmp485 != 0)goto _LL268;_LL267: goto _LL269;_LL268: _tmp486=
_tmp482.f2;if(_tmp486 != 0)goto _LL26A;_LL269: bad_effect=1;goto _LL263;_LL26A:;
_LL26B: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3FC))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F1))->v);
goto _LL263;_LL263:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmp1025;Cyc_Tcutil_failure_reason=((_tmp1025="(function type effects do not unify)",
_tag_dyneither(_tmp1025,sizeof(char),37)));}_npop_handler(0);goto _LL21A;}if(!Cyc_Tcutil_same_atts(
_tmp3F7,_tmp402)){{const char*_tmp1026;Cyc_Tcutil_failure_reason=((_tmp1026="(function types have different attributes)",
_tag_dyneither(_tmp1026,sizeof(char),43)));}_npop_handler(0);goto _LL21A;}if(!Cyc_Tcutil_same_rgn_po(
_tmp3F6,_tmp401)){{const char*_tmp1027;Cyc_Tcutil_failure_reason=((_tmp1027="(function types have different region lifetime orderings)",
_tag_dyneither(_tmp1027,sizeof(char),58)));}_npop_handler(0);goto _LL21A;}
_npop_handler(0);return;};};;_pop_region(rgn);}_LL23B: _tmp403=_tmp36C.f1;{struct
Cyc_Absyn_TupleType_struct*_tmp404=(struct Cyc_Absyn_TupleType_struct*)_tmp403;
if(_tmp404->tag != 11)goto _LL23D;else{_tmp405=_tmp404->f1;}};_tmp406=_tmp36C.f2;{
struct Cyc_Absyn_TupleType_struct*_tmp407=(struct Cyc_Absyn_TupleType_struct*)
_tmp406;if(_tmp407->tag != 11)goto _LL23D;else{_tmp408=_tmp407->f1;}};_LL23C: for(0;
_tmp408 != 0  && _tmp405 != 0;(_tmp408=_tmp408->tl,_tmp405=_tmp405->tl)){struct Cyc_Absyn_Tqual
_tmp48B;void*_tmp48C;struct _tuple11 _tmp48A=*((struct _tuple11*)_tmp408->hd);
_tmp48B=_tmp48A.f1;_tmp48C=_tmp48A.f2;{struct Cyc_Absyn_Tqual _tmp48E;void*_tmp48F;
struct _tuple11 _tmp48D=*((struct _tuple11*)_tmp405->hd);_tmp48E=_tmp48D.f1;_tmp48F=
_tmp48D.f2;Cyc_Tcutil_unify_it(_tmp48C,_tmp48F);Cyc_Tcutil_unify_tqual(_tmp48B,
_tmp48C,_tmp48E,_tmp48F);};}if(_tmp408 == 0  && _tmp405 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmp1028;Cyc_Tcutil_failure_reason=((
_tmp1028="(tuple types have different numbers of components)",_tag_dyneither(
_tmp1028,sizeof(char),51)));}goto _LL21A;_LL23D: _tmp409=_tmp36C.f1;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp40A=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp409;if(_tmp40A->tag != 13)goto
_LL23F;else{_tmp40B=_tmp40A->f1;_tmp40C=_tmp40A->f2;}};_tmp40D=_tmp36C.f2;{
struct Cyc_Absyn_AnonAggrType_struct*_tmp40E=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp40D;if(_tmp40E->tag != 13)goto _LL23F;else{_tmp40F=_tmp40E->f1;_tmp410=_tmp40E->f2;}};
_LL23E: if(_tmp40F != _tmp40B){{const char*_tmp1029;Cyc_Tcutil_failure_reason=((
_tmp1029="(struct and union type)",_tag_dyneither(_tmp1029,sizeof(char),24)));}
goto _LL21A;}for(0;_tmp410 != 0  && _tmp40C != 0;(_tmp410=_tmp410->tl,_tmp40C=
_tmp40C->tl)){struct Cyc_Absyn_Aggrfield*_tmp492=(struct Cyc_Absyn_Aggrfield*)
_tmp410->hd;struct Cyc_Absyn_Aggrfield*_tmp493=(struct Cyc_Absyn_Aggrfield*)
_tmp40C->hd;if(Cyc_strptrcmp(_tmp492->name,_tmp493->name)!= 0){{const char*
_tmp102A;Cyc_Tcutil_failure_reason=((_tmp102A="(different member names)",
_tag_dyneither(_tmp102A,sizeof(char),25)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
Cyc_Tcutil_unify_it(_tmp492->type,_tmp493->type);Cyc_Tcutil_unify_tqual(_tmp492->tq,
_tmp492->type,_tmp493->tq,_tmp493->type);if(!Cyc_Tcutil_same_atts(_tmp492->attributes,
_tmp493->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmp102B;Cyc_Tcutil_failure_reason=((_tmp102B="(different attributes on member)",
_tag_dyneither(_tmp102B,sizeof(char),33)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
if((_tmp492->width != 0  && _tmp493->width == 0  || _tmp493->width != 0  && _tmp492->width
== 0) || (_tmp492->width != 0  && _tmp493->width != 0) && !Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp492->width),(struct Cyc_Absyn_Exp*)
_check_null(_tmp493->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{
const char*_tmp102C;Cyc_Tcutil_failure_reason=((_tmp102C="(different bitfield widths on member)",
_tag_dyneither(_tmp102C,sizeof(char),38)));}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
if(_tmp410 == 0  && _tmp40C == 0)return;Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmp102D;Cyc_Tcutil_failure_reason=((_tmp102D="(different number of members)",
_tag_dyneither(_tmp102D,sizeof(char),30)));}goto _LL21A;_LL23F: _tmp411=_tmp36C.f1;{
struct Cyc_Absyn_HeapRgn_struct*_tmp412=(struct Cyc_Absyn_HeapRgn_struct*)_tmp411;
if(_tmp412->tag != 21)goto _LL241;};_tmp413=_tmp36C.f2;{struct Cyc_Absyn_HeapRgn_struct*
_tmp414=(struct Cyc_Absyn_HeapRgn_struct*)_tmp413;if(_tmp414->tag != 21)goto _LL241;};
_LL240: return;_LL241: _tmp415=_tmp36C.f1;{struct Cyc_Absyn_UniqueRgn_struct*
_tmp416=(struct Cyc_Absyn_UniqueRgn_struct*)_tmp415;if(_tmp416->tag != 22)goto
_LL243;};_tmp417=_tmp36C.f2;{struct Cyc_Absyn_UniqueRgn_struct*_tmp418=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp417;if(_tmp418->tag != 22)goto _LL243;};_LL242: return;_LL243: _tmp419=_tmp36C.f1;{
struct Cyc_Absyn_RgnHandleType_struct*_tmp41A=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp419;if(_tmp41A->tag != 16)goto _LL245;else{_tmp41B=(void*)_tmp41A->f1;}};
_tmp41C=_tmp36C.f2;{struct Cyc_Absyn_RgnHandleType_struct*_tmp41D=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp41C;if(_tmp41D->tag != 16)goto _LL245;else{_tmp41E=(void*)_tmp41D->f1;}};
_LL244: Cyc_Tcutil_unify_it(_tmp41B,_tmp41E);return;_LL245: _tmp41F=_tmp36C.f1;{
struct Cyc_Absyn_DynRgnType_struct*_tmp420=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp41F;if(_tmp420->tag != 17)goto _LL247;else{_tmp421=(void*)_tmp420->f1;_tmp422=(
void*)_tmp420->f2;}};_tmp423=_tmp36C.f2;{struct Cyc_Absyn_DynRgnType_struct*
_tmp424=(struct Cyc_Absyn_DynRgnType_struct*)_tmp423;if(_tmp424->tag != 17)goto
_LL247;else{_tmp425=(void*)_tmp424->f1;_tmp426=(void*)_tmp424->f2;}};_LL246: Cyc_Tcutil_unify_it(
_tmp421,_tmp425);Cyc_Tcutil_unify_it(_tmp422,_tmp426);return;_LL247: _tmp427=
_tmp36C.f1;{struct Cyc_Absyn_JoinEff_struct*_tmp428=(struct Cyc_Absyn_JoinEff_struct*)
_tmp427;if(_tmp428->tag != 24)goto _LL249;};_LL248: goto _LL24A;_LL249: _tmp429=
_tmp36C.f2;{struct Cyc_Absyn_JoinEff_struct*_tmp42A=(struct Cyc_Absyn_JoinEff_struct*)
_tmp429;if(_tmp42A->tag != 24)goto _LL24B;};_LL24A: goto _LL24C;_LL24B: _tmp42B=
_tmp36C.f1;{struct Cyc_Absyn_AccessEff_struct*_tmp42C=(struct Cyc_Absyn_AccessEff_struct*)
_tmp42B;if(_tmp42C->tag != 23)goto _LL24D;};_LL24C: goto _LL24E;_LL24D: _tmp42D=
_tmp36C.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp42E=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp42D;if(_tmp42E->tag != 25)goto _LL24F;};_LL24E: goto _LL250;_LL24F: _tmp42F=
_tmp36C.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp430=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp42F;if(_tmp430->tag != 25)goto _LL251;};_LL250: goto _LL252;_LL251: _tmp431=
_tmp36C.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp432=(struct Cyc_Absyn_AccessEff_struct*)
_tmp431;if(_tmp432->tag != 23)goto _LL253;};_LL252: if(Cyc_Tcutil_unify_effect(t1,
t2))return;{const char*_tmp102E;Cyc_Tcutil_failure_reason=((_tmp102E="(effects don't unify)",
_tag_dyneither(_tmp102E,sizeof(char),22)));}goto _LL21A;_LL253:;_LL254: goto _LL21A;
_LL21A:;}(int)_throw((void*)& Cyc_Tcutil_Unify_val);}int Cyc_Tcutil_star_cmp(int(*
cmp)(void*,void*),void*a1,void*a2);int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),
void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 0  && a2
== 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}static int
Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);static
int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){int
_tmp499=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int _tmp49A=(
tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp499,_tmp49A);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y);static int Cyc_Tcutil_conrefs_cmp(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp49B=x;union Cyc_Absyn_Constraint _tmp49C;int _tmp49D;
union Cyc_Absyn_Constraint _tmp49E;void*_tmp49F;union Cyc_Absyn_Constraint _tmp4A0;
union Cyc_Absyn_Constraint*_tmp4A1;_LL26D: _tmp49C=*_tmp49B;if((_tmp49C.No_constr).tag
!= 3)goto _LL26F;_tmp49D=(int)(_tmp49C.No_constr).val;_LL26E: return - 1;_LL26F:
_tmp49E=*_tmp49B;if((_tmp49E.Eq_constr).tag != 1)goto _LL271;_tmp49F=(void*)(
_tmp49E.Eq_constr).val;_LL270: {union Cyc_Absyn_Constraint*_tmp4A2=y;union Cyc_Absyn_Constraint
_tmp4A3;int _tmp4A4;union Cyc_Absyn_Constraint _tmp4A5;void*_tmp4A6;union Cyc_Absyn_Constraint
_tmp4A7;union Cyc_Absyn_Constraint*_tmp4A8;_LL274: _tmp4A3=*_tmp4A2;if((_tmp4A3.No_constr).tag
!= 3)goto _LL276;_tmp4A4=(int)(_tmp4A3.No_constr).val;_LL275: return 1;_LL276:
_tmp4A5=*_tmp4A2;if((_tmp4A5.Eq_constr).tag != 1)goto _LL278;_tmp4A6=(void*)(
_tmp4A5.Eq_constr).val;_LL277: return cmp(_tmp49F,_tmp4A6);_LL278: _tmp4A7=*_tmp4A2;
if((_tmp4A7.Forward_constr).tag != 2)goto _LL273;_tmp4A8=(union Cyc_Absyn_Constraint*)(
_tmp4A7.Forward_constr).val;_LL279: {const char*_tmp1031;void*_tmp1030;(_tmp1030=
0,Cyc_Tcutil_impos(((_tmp1031="unify_conref: forward after compress(2)",
_tag_dyneither(_tmp1031,sizeof(char),40))),_tag_dyneither(_tmp1030,sizeof(void*),
0)));}_LL273:;}_LL271: _tmp4A0=*_tmp49B;if((_tmp4A0.Forward_constr).tag != 2)goto
_LL26C;_tmp4A1=(union Cyc_Absyn_Constraint*)(_tmp4A0.Forward_constr).val;_LL272: {
const char*_tmp1034;void*_tmp1033;(_tmp1033=0,Cyc_Tcutil_impos(((_tmp1034="unify_conref: forward after compress",
_tag_dyneither(_tmp1034,sizeof(char),37))),_tag_dyneither(_tmp1033,sizeof(void*),
0)));}_LL26C:;};}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct
_tuple11*tqt2){struct _tuple11 _tmp4AE;struct Cyc_Absyn_Tqual _tmp4AF;void*_tmp4B0;
struct _tuple11*_tmp4AD=tqt1;_tmp4AE=*_tmp4AD;_tmp4AF=_tmp4AE.f1;_tmp4B0=_tmp4AE.f2;{
struct _tuple11 _tmp4B2;struct Cyc_Absyn_Tqual _tmp4B3;void*_tmp4B4;struct _tuple11*
_tmp4B1=tqt2;_tmp4B2=*_tmp4B1;_tmp4B3=_tmp4B2.f1;_tmp4B4=_tmp4B2.f2;{int _tmp4B5=
Cyc_Tcutil_tqual_cmp(_tmp4AF,_tmp4B3);if(_tmp4B5 != 0)return _tmp4B5;return Cyc_Tcutil_typecmp(
_tmp4B0,_tmp4B4);};};}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp4B6=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp4B6 != 0)return _tmp4B6;{int _tmp4B7=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp4B7 != 0)return _tmp4B7;{int _tmp4B8=Cyc_Tcutil_typecmp(f1->type,f2->type);if(
_tmp4B8 != 0)return _tmp4B8;{int _tmp4B9=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,
f2->attributes);if(_tmp4B9 != 0)return _tmp4B9;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,f1->width,f2->width);};};};}static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);static int Cyc_Tcutil_enumfield_cmp(
struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){int _tmp4BA=Cyc_Absyn_qvar_cmp(
e1->name,e2->name);if(_tmp4BA != 0)return _tmp4BA;return((int(*)(int(*cmp)(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(
void*t);static int Cyc_Tcutil_type_case_number(void*t){void*_tmp4BB=t;_LL27B: {
struct Cyc_Absyn_VoidType_struct*_tmp4BC=(struct Cyc_Absyn_VoidType_struct*)
_tmp4BB;if(_tmp4BC->tag != 0)goto _LL27D;}_LL27C: return 0;_LL27D: {struct Cyc_Absyn_Evar_struct*
_tmp4BD=(struct Cyc_Absyn_Evar_struct*)_tmp4BB;if(_tmp4BD->tag != 1)goto _LL27F;}
_LL27E: return 1;_LL27F: {struct Cyc_Absyn_VarType_struct*_tmp4BE=(struct Cyc_Absyn_VarType_struct*)
_tmp4BB;if(_tmp4BE->tag != 2)goto _LL281;}_LL280: return 2;_LL281: {struct Cyc_Absyn_DatatypeType_struct*
_tmp4BF=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4BB;if(_tmp4BF->tag != 3)goto
_LL283;}_LL282: return 3;_LL283: {struct Cyc_Absyn_DatatypeFieldType_struct*_tmp4C0=(
struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4BB;if(_tmp4C0->tag != 4)goto _LL285;}
_LL284: return 4;_LL285: {struct Cyc_Absyn_PointerType_struct*_tmp4C1=(struct Cyc_Absyn_PointerType_struct*)
_tmp4BB;if(_tmp4C1->tag != 5)goto _LL287;}_LL286: return 5;_LL287: {struct Cyc_Absyn_IntType_struct*
_tmp4C2=(struct Cyc_Absyn_IntType_struct*)_tmp4BB;if(_tmp4C2->tag != 6)goto _LL289;}
_LL288: return 6;_LL289: {struct Cyc_Absyn_FloatType_struct*_tmp4C3=(struct Cyc_Absyn_FloatType_struct*)
_tmp4BB;if(_tmp4C3->tag != 7)goto _LL28B;}_LL28A: return 7;_LL28B: {struct Cyc_Absyn_DoubleType_struct*
_tmp4C4=(struct Cyc_Absyn_DoubleType_struct*)_tmp4BB;if(_tmp4C4->tag != 8)goto
_LL28D;}_LL28C: return 8;_LL28D: {struct Cyc_Absyn_ArrayType_struct*_tmp4C5=(struct
Cyc_Absyn_ArrayType_struct*)_tmp4BB;if(_tmp4C5->tag != 9)goto _LL28F;}_LL28E:
return 9;_LL28F: {struct Cyc_Absyn_FnType_struct*_tmp4C6=(struct Cyc_Absyn_FnType_struct*)
_tmp4BB;if(_tmp4C6->tag != 10)goto _LL291;}_LL290: return 10;_LL291: {struct Cyc_Absyn_TupleType_struct*
_tmp4C7=(struct Cyc_Absyn_TupleType_struct*)_tmp4BB;if(_tmp4C7->tag != 11)goto
_LL293;}_LL292: return 11;_LL293: {struct Cyc_Absyn_AggrType_struct*_tmp4C8=(struct
Cyc_Absyn_AggrType_struct*)_tmp4BB;if(_tmp4C8->tag != 12)goto _LL295;}_LL294:
return 12;_LL295: {struct Cyc_Absyn_AnonAggrType_struct*_tmp4C9=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmp4BB;if(_tmp4C9->tag != 13)goto _LL297;}_LL296: return 14;_LL297: {struct Cyc_Absyn_EnumType_struct*
_tmp4CA=(struct Cyc_Absyn_EnumType_struct*)_tmp4BB;if(_tmp4CA->tag != 14)goto
_LL299;}_LL298: return 16;_LL299: {struct Cyc_Absyn_AnonEnumType_struct*_tmp4CB=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp4BB;if(_tmp4CB->tag != 15)goto _LL29B;}
_LL29A: return 17;_LL29B: {struct Cyc_Absyn_RgnHandleType_struct*_tmp4CC=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp4BB;if(_tmp4CC->tag != 16)goto _LL29D;}_LL29C: return 18;_LL29D: {struct Cyc_Absyn_TypedefType_struct*
_tmp4CD=(struct Cyc_Absyn_TypedefType_struct*)_tmp4BB;if(_tmp4CD->tag != 18)goto
_LL29F;}_LL29E: return 19;_LL29F: {struct Cyc_Absyn_UniqueRgn_struct*_tmp4CE=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp4BB;if(_tmp4CE->tag != 22)goto _LL2A1;}
_LL2A0: return 20;_LL2A1: {struct Cyc_Absyn_HeapRgn_struct*_tmp4CF=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp4BB;if(_tmp4CF->tag != 21)goto _LL2A3;}_LL2A2: return 21;_LL2A3: {struct Cyc_Absyn_AccessEff_struct*
_tmp4D0=(struct Cyc_Absyn_AccessEff_struct*)_tmp4BB;if(_tmp4D0->tag != 23)goto
_LL2A5;}_LL2A4: return 22;_LL2A5: {struct Cyc_Absyn_JoinEff_struct*_tmp4D1=(struct
Cyc_Absyn_JoinEff_struct*)_tmp4BB;if(_tmp4D1->tag != 24)goto _LL2A7;}_LL2A6: return
23;_LL2A7: {struct Cyc_Absyn_RgnsEff_struct*_tmp4D2=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp4BB;if(_tmp4D2->tag != 25)goto _LL2A9;}_LL2A8: return 24;_LL2A9: {struct Cyc_Absyn_TagType_struct*
_tmp4D3=(struct Cyc_Absyn_TagType_struct*)_tmp4BB;if(_tmp4D3->tag != 20)goto _LL2AB;}
_LL2AA: return 26;_LL2AB: {struct Cyc_Absyn_DynRgnType_struct*_tmp4D4=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp4BB;if(_tmp4D4->tag != 17)goto _LL2AD;}_LL2AC: return 27;_LL2AD: {struct Cyc_Absyn_ValueofType_struct*
_tmp4D5=(struct Cyc_Absyn_ValueofType_struct*)_tmp4BB;if(_tmp4D5->tag != 19)goto
_LL27A;}_LL2AE: return 28;_LL27A:;}int Cyc_Tcutil_typecmp(void*t1,void*t2);int Cyc_Tcutil_typecmp(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == 
t2)return 0;{int _tmp4D6=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp4D6 != 0)return _tmp4D6;{struct _tuple0 _tmp1035;struct _tuple0 _tmp4D8=(
_tmp1035.f1=t2,((_tmp1035.f2=t1,_tmp1035)));void*_tmp4D9;void*_tmp4DB;void*
_tmp4DD;struct Cyc_Absyn_Tvar*_tmp4DF;void*_tmp4E0;struct Cyc_Absyn_Tvar*_tmp4E2;
void*_tmp4E3;struct Cyc_Absyn_AggrInfo _tmp4E5;union Cyc_Absyn_AggrInfoU _tmp4E6;
struct Cyc_List_List*_tmp4E7;void*_tmp4E8;struct Cyc_Absyn_AggrInfo _tmp4EA;union
Cyc_Absyn_AggrInfoU _tmp4EB;struct Cyc_List_List*_tmp4EC;void*_tmp4ED;struct
_tuple2*_tmp4EF;void*_tmp4F0;struct _tuple2*_tmp4F2;void*_tmp4F3;struct Cyc_List_List*
_tmp4F5;void*_tmp4F6;struct Cyc_List_List*_tmp4F8;void*_tmp4F9;struct Cyc_Absyn_DatatypeInfo
_tmp4FB;union Cyc_Absyn_DatatypeInfoU _tmp4FC;struct Cyc_Absyn_Datatypedecl**
_tmp4FD;struct Cyc_Absyn_Datatypedecl*_tmp4FE;struct Cyc_List_List*_tmp4FF;void*
_tmp500;struct Cyc_Absyn_DatatypeInfo _tmp502;union Cyc_Absyn_DatatypeInfoU _tmp503;
struct Cyc_Absyn_Datatypedecl**_tmp504;struct Cyc_Absyn_Datatypedecl*_tmp505;
struct Cyc_List_List*_tmp506;void*_tmp507;struct Cyc_Absyn_DatatypeFieldInfo
_tmp509;union Cyc_Absyn_DatatypeFieldInfoU _tmp50A;struct _tuple3 _tmp50B;struct Cyc_Absyn_Datatypedecl*
_tmp50C;struct Cyc_Absyn_Datatypefield*_tmp50D;struct Cyc_List_List*_tmp50E;void*
_tmp50F;struct Cyc_Absyn_DatatypeFieldInfo _tmp511;union Cyc_Absyn_DatatypeFieldInfoU
_tmp512;struct _tuple3 _tmp513;struct Cyc_Absyn_Datatypedecl*_tmp514;struct Cyc_Absyn_Datatypefield*
_tmp515;struct Cyc_List_List*_tmp516;void*_tmp517;struct Cyc_Absyn_PtrInfo _tmp519;
void*_tmp51A;struct Cyc_Absyn_Tqual _tmp51B;struct Cyc_Absyn_PtrAtts _tmp51C;void*
_tmp51D;union Cyc_Absyn_Constraint*_tmp51E;union Cyc_Absyn_Constraint*_tmp51F;
union Cyc_Absyn_Constraint*_tmp520;void*_tmp521;struct Cyc_Absyn_PtrInfo _tmp523;
void*_tmp524;struct Cyc_Absyn_Tqual _tmp525;struct Cyc_Absyn_PtrAtts _tmp526;void*
_tmp527;union Cyc_Absyn_Constraint*_tmp528;union Cyc_Absyn_Constraint*_tmp529;
union Cyc_Absyn_Constraint*_tmp52A;void*_tmp52B;enum Cyc_Absyn_Sign _tmp52D;enum 
Cyc_Absyn_Size_of _tmp52E;void*_tmp52F;enum Cyc_Absyn_Sign _tmp531;enum Cyc_Absyn_Size_of
_tmp532;void*_tmp533;void*_tmp535;void*_tmp537;int _tmp539;void*_tmp53A;int
_tmp53C;void*_tmp53D;struct Cyc_Absyn_ArrayInfo _tmp53F;void*_tmp540;struct Cyc_Absyn_Tqual
_tmp541;struct Cyc_Absyn_Exp*_tmp542;union Cyc_Absyn_Constraint*_tmp543;void*
_tmp544;struct Cyc_Absyn_ArrayInfo _tmp546;void*_tmp547;struct Cyc_Absyn_Tqual
_tmp548;struct Cyc_Absyn_Exp*_tmp549;union Cyc_Absyn_Constraint*_tmp54A;void*
_tmp54B;struct Cyc_Absyn_FnInfo _tmp54D;struct Cyc_List_List*_tmp54E;struct Cyc_Core_Opt*
_tmp54F;void*_tmp550;struct Cyc_List_List*_tmp551;int _tmp552;struct Cyc_Absyn_VarargInfo*
_tmp553;struct Cyc_List_List*_tmp554;struct Cyc_List_List*_tmp555;void*_tmp556;
struct Cyc_Absyn_FnInfo _tmp558;struct Cyc_List_List*_tmp559;struct Cyc_Core_Opt*
_tmp55A;void*_tmp55B;struct Cyc_List_List*_tmp55C;int _tmp55D;struct Cyc_Absyn_VarargInfo*
_tmp55E;struct Cyc_List_List*_tmp55F;struct Cyc_List_List*_tmp560;void*_tmp561;
struct Cyc_List_List*_tmp563;void*_tmp564;struct Cyc_List_List*_tmp566;void*
_tmp567;enum Cyc_Absyn_AggrKind _tmp569;struct Cyc_List_List*_tmp56A;void*_tmp56B;
enum Cyc_Absyn_AggrKind _tmp56D;struct Cyc_List_List*_tmp56E;void*_tmp56F;void*
_tmp571;void*_tmp572;void*_tmp574;void*_tmp575;void*_tmp577;void*_tmp578;void*
_tmp579;void*_tmp57B;void*_tmp57C;void*_tmp57D;void*_tmp57F;void*_tmp580;void*
_tmp582;void*_tmp583;struct Cyc_Absyn_Exp*_tmp585;void*_tmp586;struct Cyc_Absyn_Exp*
_tmp588;void*_tmp589;void*_tmp58B;void*_tmp58D;void*_tmp58F;void*_tmp591;void*
_tmp593;_LL2B0: _tmp4D9=_tmp4D8.f1;{struct Cyc_Absyn_Evar_struct*_tmp4DA=(struct
Cyc_Absyn_Evar_struct*)_tmp4D9;if(_tmp4DA->tag != 1)goto _LL2B2;};_tmp4DB=_tmp4D8.f2;{
struct Cyc_Absyn_Evar_struct*_tmp4DC=(struct Cyc_Absyn_Evar_struct*)_tmp4DB;if(
_tmp4DC->tag != 1)goto _LL2B2;};_LL2B1: {const char*_tmp1038;void*_tmp1037;(
_tmp1037=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1038="typecmp: can only compare closed types",_tag_dyneither(_tmp1038,
sizeof(char),39))),_tag_dyneither(_tmp1037,sizeof(void*),0)));}_LL2B2: _tmp4DD=
_tmp4D8.f1;{struct Cyc_Absyn_VarType_struct*_tmp4DE=(struct Cyc_Absyn_VarType_struct*)
_tmp4DD;if(_tmp4DE->tag != 2)goto _LL2B4;else{_tmp4DF=_tmp4DE->f1;}};_tmp4E0=
_tmp4D8.f2;{struct Cyc_Absyn_VarType_struct*_tmp4E1=(struct Cyc_Absyn_VarType_struct*)
_tmp4E0;if(_tmp4E1->tag != 2)goto _LL2B4;else{_tmp4E2=_tmp4E1->f1;}};_LL2B3: return
Cyc_Core_intcmp(_tmp4E2->identity,_tmp4DF->identity);_LL2B4: _tmp4E3=_tmp4D8.f1;{
struct Cyc_Absyn_AggrType_struct*_tmp4E4=(struct Cyc_Absyn_AggrType_struct*)
_tmp4E3;if(_tmp4E4->tag != 12)goto _LL2B6;else{_tmp4E5=_tmp4E4->f1;_tmp4E6=_tmp4E5.aggr_info;
_tmp4E7=_tmp4E5.targs;}};_tmp4E8=_tmp4D8.f2;{struct Cyc_Absyn_AggrType_struct*
_tmp4E9=(struct Cyc_Absyn_AggrType_struct*)_tmp4E8;if(_tmp4E9->tag != 12)goto
_LL2B6;else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4EA.aggr_info;_tmp4EC=_tmp4EA.targs;}};
_LL2B5: {struct _tuple2*_tmp598;struct _tuple12 _tmp597=Cyc_Absyn_aggr_kinded_name(
_tmp4E6);_tmp598=_tmp597.f2;{struct _tuple2*_tmp59A;struct _tuple12 _tmp599=Cyc_Absyn_aggr_kinded_name(
_tmp4EB);_tmp59A=_tmp599.f2;{int _tmp59B=Cyc_Absyn_qvar_cmp(_tmp598,_tmp59A);if(
_tmp59B != 0)return _tmp59B;else{return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp4E7,_tmp4EC);}};};}
_LL2B6: _tmp4ED=_tmp4D8.f1;{struct Cyc_Absyn_EnumType_struct*_tmp4EE=(struct Cyc_Absyn_EnumType_struct*)
_tmp4ED;if(_tmp4EE->tag != 14)goto _LL2B8;else{_tmp4EF=_tmp4EE->f1;}};_tmp4F0=
_tmp4D8.f2;{struct Cyc_Absyn_EnumType_struct*_tmp4F1=(struct Cyc_Absyn_EnumType_struct*)
_tmp4F0;if(_tmp4F1->tag != 14)goto _LL2B8;else{_tmp4F2=_tmp4F1->f1;}};_LL2B7:
return Cyc_Absyn_qvar_cmp(_tmp4EF,_tmp4F2);_LL2B8: _tmp4F3=_tmp4D8.f1;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp4F4=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp4F3;if(_tmp4F4->tag != 15)goto
_LL2BA;else{_tmp4F5=_tmp4F4->f1;}};_tmp4F6=_tmp4D8.f2;{struct Cyc_Absyn_AnonEnumType_struct*
_tmp4F7=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp4F6;if(_tmp4F7->tag != 15)goto
_LL2BA;else{_tmp4F8=_tmp4F7->f1;}};_LL2B9: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp4F5,_tmp4F8);_LL2BA: _tmp4F9=_tmp4D8.f1;{struct Cyc_Absyn_DatatypeType_struct*
_tmp4FA=(struct Cyc_Absyn_DatatypeType_struct*)_tmp4F9;if(_tmp4FA->tag != 3)goto
_LL2BC;else{_tmp4FB=_tmp4FA->f1;_tmp4FC=_tmp4FB.datatype_info;if((_tmp4FC.KnownDatatype).tag
!= 2)goto _LL2BC;_tmp4FD=(struct Cyc_Absyn_Datatypedecl**)(_tmp4FC.KnownDatatype).val;
_tmp4FE=*_tmp4FD;_tmp4FF=_tmp4FB.targs;}};_tmp500=_tmp4D8.f2;{struct Cyc_Absyn_DatatypeType_struct*
_tmp501=(struct Cyc_Absyn_DatatypeType_struct*)_tmp500;if(_tmp501->tag != 3)goto
_LL2BC;else{_tmp502=_tmp501->f1;_tmp503=_tmp502.datatype_info;if((_tmp503.KnownDatatype).tag
!= 2)goto _LL2BC;_tmp504=(struct Cyc_Absyn_Datatypedecl**)(_tmp503.KnownDatatype).val;
_tmp505=*_tmp504;_tmp506=_tmp502.targs;}};_LL2BB: if(_tmp505 == _tmp4FE)return 0;{
int _tmp59C=Cyc_Absyn_qvar_cmp(_tmp505->name,_tmp4FE->name);if(_tmp59C != 0)return
_tmp59C;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp506,_tmp4FF);};_LL2BC: _tmp507=
_tmp4D8.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*_tmp508=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp507;if(_tmp508->tag != 4)goto _LL2BE;else{_tmp509=_tmp508->f1;_tmp50A=_tmp509.field_info;
if((_tmp50A.KnownDatatypefield).tag != 2)goto _LL2BE;_tmp50B=(struct _tuple3)(
_tmp50A.KnownDatatypefield).val;_tmp50C=_tmp50B.f1;_tmp50D=_tmp50B.f2;_tmp50E=
_tmp509.targs;}};_tmp50F=_tmp4D8.f2;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp510=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp50F;if(_tmp510->tag != 4)
goto _LL2BE;else{_tmp511=_tmp510->f1;_tmp512=_tmp511.field_info;if((_tmp512.KnownDatatypefield).tag
!= 2)goto _LL2BE;_tmp513=(struct _tuple3)(_tmp512.KnownDatatypefield).val;_tmp514=
_tmp513.f1;_tmp515=_tmp513.f2;_tmp516=_tmp511.targs;}};_LL2BD: if(_tmp514 == 
_tmp50C)return 0;{int _tmp59D=Cyc_Absyn_qvar_cmp(_tmp50C->name,_tmp514->name);if(
_tmp59D != 0)return _tmp59D;{int _tmp59E=Cyc_Absyn_qvar_cmp(_tmp50D->name,_tmp515->name);
if(_tmp59E != 0)return _tmp59E;return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp516,_tmp50E);};};
_LL2BE: _tmp517=_tmp4D8.f1;{struct Cyc_Absyn_PointerType_struct*_tmp518=(struct Cyc_Absyn_PointerType_struct*)
_tmp517;if(_tmp518->tag != 5)goto _LL2C0;else{_tmp519=_tmp518->f1;_tmp51A=_tmp519.elt_typ;
_tmp51B=_tmp519.elt_tq;_tmp51C=_tmp519.ptr_atts;_tmp51D=_tmp51C.rgn;_tmp51E=
_tmp51C.nullable;_tmp51F=_tmp51C.bounds;_tmp520=_tmp51C.zero_term;}};_tmp521=
_tmp4D8.f2;{struct Cyc_Absyn_PointerType_struct*_tmp522=(struct Cyc_Absyn_PointerType_struct*)
_tmp521;if(_tmp522->tag != 5)goto _LL2C0;else{_tmp523=_tmp522->f1;_tmp524=_tmp523.elt_typ;
_tmp525=_tmp523.elt_tq;_tmp526=_tmp523.ptr_atts;_tmp527=_tmp526.rgn;_tmp528=
_tmp526.nullable;_tmp529=_tmp526.bounds;_tmp52A=_tmp526.zero_term;}};_LL2BF: {
int _tmp59F=Cyc_Tcutil_typecmp(_tmp524,_tmp51A);if(_tmp59F != 0)return _tmp59F;{int
_tmp5A0=Cyc_Tcutil_typecmp(_tmp527,_tmp51D);if(_tmp5A0 != 0)return _tmp5A0;{int
_tmp5A1=Cyc_Tcutil_tqual_cmp(_tmp525,_tmp51B);if(_tmp5A1 != 0)return _tmp5A1;{int
_tmp5A2=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp529,_tmp51F);if(_tmp5A2 != 0)
return _tmp5A2;{int _tmp5A3=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp52A,
_tmp520);if(_tmp5A3 != 0)return _tmp5A3;{void*_tmp5A4=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp529);_LL2E5: {struct Cyc_Absyn_DynEither_b_struct*
_tmp5A5=(struct Cyc_Absyn_DynEither_b_struct*)_tmp5A4;if(_tmp5A5->tag != 0)goto
_LL2E7;}_LL2E6: return 0;_LL2E7:;_LL2E8: goto _LL2E4;_LL2E4:;}return((int(*)(int(*
cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp528,_tmp51E);};};};};}_LL2C0: _tmp52B=_tmp4D8.f1;{struct Cyc_Absyn_IntType_struct*
_tmp52C=(struct Cyc_Absyn_IntType_struct*)_tmp52B;if(_tmp52C->tag != 6)goto _LL2C2;
else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52C->f2;}};_tmp52F=_tmp4D8.f2;{struct Cyc_Absyn_IntType_struct*
_tmp530=(struct Cyc_Absyn_IntType_struct*)_tmp52F;if(_tmp530->tag != 6)goto _LL2C2;
else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}};_LL2C1: if(_tmp531 != _tmp52D)
return Cyc_Core_intcmp((int)((unsigned int)_tmp531),(int)((unsigned int)_tmp52D));
if(_tmp532 != _tmp52E)return Cyc_Core_intcmp((int)((unsigned int)_tmp532),(int)((
unsigned int)_tmp52E));return 0;_LL2C2: _tmp533=_tmp4D8.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp534=(struct Cyc_Absyn_FloatType_struct*)_tmp533;if(_tmp534->tag != 7)goto
_LL2C4;};_tmp535=_tmp4D8.f2;{struct Cyc_Absyn_FloatType_struct*_tmp536=(struct Cyc_Absyn_FloatType_struct*)
_tmp535;if(_tmp536->tag != 7)goto _LL2C4;};_LL2C3: return 0;_LL2C4: _tmp537=_tmp4D8.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp538=(struct Cyc_Absyn_DoubleType_struct*)
_tmp537;if(_tmp538->tag != 8)goto _LL2C6;else{_tmp539=_tmp538->f1;}};_tmp53A=
_tmp4D8.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp53B=(struct Cyc_Absyn_DoubleType_struct*)
_tmp53A;if(_tmp53B->tag != 8)goto _LL2C6;else{_tmp53C=_tmp53B->f1;}};_LL2C5: if(
_tmp539 == _tmp53C)return 0;else{if(_tmp539)return - 1;else{return 1;}}_LL2C6: _tmp53D=
_tmp4D8.f1;{struct Cyc_Absyn_ArrayType_struct*_tmp53E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp53D;if(_tmp53E->tag != 9)goto _LL2C8;else{_tmp53F=_tmp53E->f1;_tmp540=_tmp53F.elt_type;
_tmp541=_tmp53F.tq;_tmp542=_tmp53F.num_elts;_tmp543=_tmp53F.zero_term;}};_tmp544=
_tmp4D8.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp545=(struct Cyc_Absyn_ArrayType_struct*)
_tmp544;if(_tmp545->tag != 9)goto _LL2C8;else{_tmp546=_tmp545->f1;_tmp547=_tmp546.elt_type;
_tmp548=_tmp546.tq;_tmp549=_tmp546.num_elts;_tmp54A=_tmp546.zero_term;}};_LL2C7: {
int _tmp5A6=Cyc_Tcutil_tqual_cmp(_tmp548,_tmp541);if(_tmp5A6 != 0)return _tmp5A6;{
int _tmp5A7=Cyc_Tcutil_typecmp(_tmp547,_tmp540);if(_tmp5A7 != 0)return _tmp5A7;{int
_tmp5A8=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp543,_tmp54A);if(_tmp5A8 != 0)
return _tmp5A8;if(_tmp542 == _tmp549)return 0;if(_tmp542 == 0  || _tmp549 == 0){const
char*_tmp103B;void*_tmp103A;(_tmp103A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp103B="missing expression in array index",
_tag_dyneither(_tmp103B,sizeof(char),34))),_tag_dyneither(_tmp103A,sizeof(void*),
0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct
Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
_tmp542,_tmp549);};};}_LL2C8: _tmp54B=_tmp4D8.f1;{struct Cyc_Absyn_FnType_struct*
_tmp54C=(struct Cyc_Absyn_FnType_struct*)_tmp54B;if(_tmp54C->tag != 10)goto _LL2CA;
else{_tmp54D=_tmp54C->f1;_tmp54E=_tmp54D.tvars;_tmp54F=_tmp54D.effect;_tmp550=
_tmp54D.ret_typ;_tmp551=_tmp54D.args;_tmp552=_tmp54D.c_varargs;_tmp553=_tmp54D.cyc_varargs;
_tmp554=_tmp54D.rgn_po;_tmp555=_tmp54D.attributes;}};_tmp556=_tmp4D8.f2;{struct
Cyc_Absyn_FnType_struct*_tmp557=(struct Cyc_Absyn_FnType_struct*)_tmp556;if(
_tmp557->tag != 10)goto _LL2CA;else{_tmp558=_tmp557->f1;_tmp559=_tmp558.tvars;
_tmp55A=_tmp558.effect;_tmp55B=_tmp558.ret_typ;_tmp55C=_tmp558.args;_tmp55D=
_tmp558.c_varargs;_tmp55E=_tmp558.cyc_varargs;_tmp55F=_tmp558.rgn_po;_tmp560=
_tmp558.attributes;}};_LL2C9: {const char*_tmp103E;void*_tmp103D;(_tmp103D=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp103E="typecmp: function types not handled",_tag_dyneither(_tmp103E,sizeof(
char),36))),_tag_dyneither(_tmp103D,sizeof(void*),0)));}_LL2CA: _tmp561=_tmp4D8.f1;{
struct Cyc_Absyn_TupleType_struct*_tmp562=(struct Cyc_Absyn_TupleType_struct*)
_tmp561;if(_tmp562->tag != 11)goto _LL2CC;else{_tmp563=_tmp562->f1;}};_tmp564=
_tmp4D8.f2;{struct Cyc_Absyn_TupleType_struct*_tmp565=(struct Cyc_Absyn_TupleType_struct*)
_tmp564;if(_tmp565->tag != 11)goto _LL2CC;else{_tmp566=_tmp565->f1;}};_LL2CB:
return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp566,
_tmp563);_LL2CC: _tmp567=_tmp4D8.f1;{struct Cyc_Absyn_AnonAggrType_struct*_tmp568=(
struct Cyc_Absyn_AnonAggrType_struct*)_tmp567;if(_tmp568->tag != 13)goto _LL2CE;
else{_tmp569=_tmp568->f1;_tmp56A=_tmp568->f2;}};_tmp56B=_tmp4D8.f2;{struct Cyc_Absyn_AnonAggrType_struct*
_tmp56C=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp56B;if(_tmp56C->tag != 13)goto
_LL2CE;else{_tmp56D=_tmp56C->f1;_tmp56E=_tmp56C->f2;}};_LL2CD: if(_tmp56D != 
_tmp569){if(_tmp56D == Cyc_Absyn_StructA)return - 1;else{return 1;}}return((int(*)(
int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp56E,
_tmp56A);_LL2CE: _tmp56F=_tmp4D8.f1;{struct Cyc_Absyn_RgnHandleType_struct*_tmp570=(
struct Cyc_Absyn_RgnHandleType_struct*)_tmp56F;if(_tmp570->tag != 16)goto _LL2D0;
else{_tmp571=(void*)_tmp570->f1;}};_tmp572=_tmp4D8.f2;{struct Cyc_Absyn_RgnHandleType_struct*
_tmp573=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp572;if(_tmp573->tag != 16)goto
_LL2D0;else{_tmp574=(void*)_tmp573->f1;}};_LL2CF: return Cyc_Tcutil_typecmp(
_tmp571,_tmp574);_LL2D0: _tmp575=_tmp4D8.f1;{struct Cyc_Absyn_DynRgnType_struct*
_tmp576=(struct Cyc_Absyn_DynRgnType_struct*)_tmp575;if(_tmp576->tag != 17)goto
_LL2D2;else{_tmp577=(void*)_tmp576->f1;_tmp578=(void*)_tmp576->f2;}};_tmp579=
_tmp4D8.f2;{struct Cyc_Absyn_DynRgnType_struct*_tmp57A=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp579;if(_tmp57A->tag != 17)goto _LL2D2;else{_tmp57B=(void*)_tmp57A->f1;_tmp57C=(
void*)_tmp57A->f2;}};_LL2D1: {int _tmp5AD=Cyc_Tcutil_typecmp(_tmp577,_tmp57B);if(
_tmp5AD != 0)return _tmp5AD;else{return Cyc_Tcutil_typecmp(_tmp578,_tmp57C);}}
_LL2D2: _tmp57D=_tmp4D8.f1;{struct Cyc_Absyn_TagType_struct*_tmp57E=(struct Cyc_Absyn_TagType_struct*)
_tmp57D;if(_tmp57E->tag != 20)goto _LL2D4;else{_tmp57F=(void*)_tmp57E->f1;}};
_tmp580=_tmp4D8.f2;{struct Cyc_Absyn_TagType_struct*_tmp581=(struct Cyc_Absyn_TagType_struct*)
_tmp580;if(_tmp581->tag != 20)goto _LL2D4;else{_tmp582=(void*)_tmp581->f1;}};
_LL2D3: return Cyc_Tcutil_typecmp(_tmp57F,_tmp582);_LL2D4: _tmp583=_tmp4D8.f1;{
struct Cyc_Absyn_ValueofType_struct*_tmp584=(struct Cyc_Absyn_ValueofType_struct*)
_tmp583;if(_tmp584->tag != 19)goto _LL2D6;else{_tmp585=_tmp584->f1;}};_tmp586=
_tmp4D8.f2;{struct Cyc_Absyn_ValueofType_struct*_tmp587=(struct Cyc_Absyn_ValueofType_struct*)
_tmp586;if(_tmp587->tag != 19)goto _LL2D6;else{_tmp588=_tmp587->f1;}};_LL2D5:
return Cyc_Evexp_const_exp_cmp(_tmp585,_tmp588);_LL2D6: _tmp589=_tmp4D8.f1;{struct
Cyc_Absyn_JoinEff_struct*_tmp58A=(struct Cyc_Absyn_JoinEff_struct*)_tmp589;if(
_tmp58A->tag != 24)goto _LL2D8;};_LL2D7: goto _LL2D9;_LL2D8: _tmp58B=_tmp4D8.f2;{
struct Cyc_Absyn_JoinEff_struct*_tmp58C=(struct Cyc_Absyn_JoinEff_struct*)_tmp58B;
if(_tmp58C->tag != 24)goto _LL2DA;};_LL2D9: goto _LL2DB;_LL2DA: _tmp58D=_tmp4D8.f1;{
struct Cyc_Absyn_AccessEff_struct*_tmp58E=(struct Cyc_Absyn_AccessEff_struct*)
_tmp58D;if(_tmp58E->tag != 23)goto _LL2DC;};_LL2DB: goto _LL2DD;_LL2DC: _tmp58F=
_tmp4D8.f1;{struct Cyc_Absyn_RgnsEff_struct*_tmp590=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp58F;if(_tmp590->tag != 25)goto _LL2DE;};_LL2DD: goto _LL2DF;_LL2DE: _tmp591=
_tmp4D8.f2;{struct Cyc_Absyn_RgnsEff_struct*_tmp592=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp591;if(_tmp592->tag != 25)goto _LL2E0;};_LL2DF: goto _LL2E1;_LL2E0: _tmp593=
_tmp4D8.f2;{struct Cyc_Absyn_AccessEff_struct*_tmp594=(struct Cyc_Absyn_AccessEff_struct*)
_tmp593;if(_tmp594->tag != 23)goto _LL2E2;};_LL2E1: {const char*_tmp1041;void*
_tmp1040;(_tmp1040=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1041="typecmp: effects not handled",_tag_dyneither(
_tmp1041,sizeof(char),29))),_tag_dyneither(_tmp1040,sizeof(void*),0)));}_LL2E2:;
_LL2E3: {const char*_tmp1044;void*_tmp1043;(_tmp1043=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1044="Unmatched case in typecmp",
_tag_dyneither(_tmp1044,sizeof(char),26))),_tag_dyneither(_tmp1043,sizeof(void*),
0)));}_LL2AF:;};};}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp5B2=Cyc_Tcutil_compress(t);_LL2EA: {struct Cyc_Absyn_IntType_struct*
_tmp5B3=(struct Cyc_Absyn_IntType_struct*)_tmp5B2;if(_tmp5B3->tag != 6)goto _LL2EC;}
_LL2EB: goto _LL2ED;_LL2EC: {struct Cyc_Absyn_FloatType_struct*_tmp5B4=(struct Cyc_Absyn_FloatType_struct*)
_tmp5B2;if(_tmp5B4->tag != 7)goto _LL2EE;}_LL2ED: goto _LL2EF;_LL2EE: {struct Cyc_Absyn_DoubleType_struct*
_tmp5B5=(struct Cyc_Absyn_DoubleType_struct*)_tmp5B2;if(_tmp5B5->tag != 8)goto
_LL2F0;}_LL2EF: goto _LL2F1;_LL2F0: {struct Cyc_Absyn_EnumType_struct*_tmp5B6=(
struct Cyc_Absyn_EnumType_struct*)_tmp5B2;if(_tmp5B6->tag != 14)goto _LL2F2;}_LL2F1:
goto _LL2F3;_LL2F2: {struct Cyc_Absyn_AnonEnumType_struct*_tmp5B7=(struct Cyc_Absyn_AnonEnumType_struct*)
_tmp5B2;if(_tmp5B7->tag != 15)goto _LL2F4;}_LL2F3: return 1;_LL2F4:;_LL2F5: return 0;
_LL2E9:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple0 _tmp1045;struct _tuple0 _tmp5B9=(_tmp1045.f1=t1,((_tmp1045.f2=t2,_tmp1045)));
void*_tmp5BA;int _tmp5BC;void*_tmp5BD;int _tmp5BF;void*_tmp5C0;void*_tmp5C2;void*
_tmp5C4;void*_tmp5C6;void*_tmp5C8;void*_tmp5CA;void*_tmp5CC;void*_tmp5CE;void*
_tmp5D0;void*_tmp5D2;void*_tmp5D4;enum Cyc_Absyn_Size_of _tmp5D6;void*_tmp5D7;
enum Cyc_Absyn_Size_of _tmp5D9;void*_tmp5DA;enum Cyc_Absyn_Size_of _tmp5DC;void*
_tmp5DD;enum Cyc_Absyn_Size_of _tmp5DF;void*_tmp5E0;enum Cyc_Absyn_Size_of _tmp5E2;
void*_tmp5E3;enum Cyc_Absyn_Size_of _tmp5E5;void*_tmp5E6;enum Cyc_Absyn_Size_of
_tmp5E8;void*_tmp5E9;enum Cyc_Absyn_Size_of _tmp5EB;void*_tmp5EC;void*_tmp5EE;
enum Cyc_Absyn_Size_of _tmp5F0;void*_tmp5F1;void*_tmp5F3;enum Cyc_Absyn_Size_of
_tmp5F5;void*_tmp5F6;enum Cyc_Absyn_Size_of _tmp5F8;void*_tmp5F9;enum Cyc_Absyn_Size_of
_tmp5FB;void*_tmp5FC;enum Cyc_Absyn_Size_of _tmp5FE;void*_tmp5FF;void*_tmp601;
enum Cyc_Absyn_Size_of _tmp603;void*_tmp604;enum Cyc_Absyn_Size_of _tmp606;void*
_tmp607;enum Cyc_Absyn_Size_of _tmp609;void*_tmp60A;enum Cyc_Absyn_Size_of _tmp60C;
void*_tmp60D;enum Cyc_Absyn_Size_of _tmp60F;void*_tmp610;enum Cyc_Absyn_Size_of
_tmp612;void*_tmp613;enum Cyc_Absyn_Size_of _tmp615;void*_tmp616;void*_tmp618;
enum Cyc_Absyn_Size_of _tmp61A;_LL2F7: _tmp5BA=_tmp5B9.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp5BB=(struct Cyc_Absyn_DoubleType_struct*)_tmp5BA;if(_tmp5BB->tag != 8)goto
_LL2F9;else{_tmp5BC=_tmp5BB->f1;}};_tmp5BD=_tmp5B9.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp5BE=(struct Cyc_Absyn_DoubleType_struct*)_tmp5BD;if(_tmp5BE->tag != 8)goto
_LL2F9;else{_tmp5BF=_tmp5BE->f1;}};_LL2F8: return !_tmp5BF  && _tmp5BC;_LL2F9:
_tmp5C0=_tmp5B9.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp5C1=(struct Cyc_Absyn_DoubleType_struct*)
_tmp5C0;if(_tmp5C1->tag != 8)goto _LL2FB;};_tmp5C2=_tmp5B9.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp5C3=(struct Cyc_Absyn_FloatType_struct*)_tmp5C2;if(_tmp5C3->tag != 7)goto
_LL2FB;};_LL2FA: goto _LL2FC;_LL2FB: _tmp5C4=_tmp5B9.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp5C5=(struct Cyc_Absyn_DoubleType_struct*)_tmp5C4;if(_tmp5C5->tag != 8)goto
_LL2FD;};_tmp5C6=_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp5C7=(struct Cyc_Absyn_IntType_struct*)
_tmp5C6;if(_tmp5C7->tag != 6)goto _LL2FD;};_LL2FC: goto _LL2FE;_LL2FD: _tmp5C8=
_tmp5B9.f1;{struct Cyc_Absyn_DoubleType_struct*_tmp5C9=(struct Cyc_Absyn_DoubleType_struct*)
_tmp5C8;if(_tmp5C9->tag != 8)goto _LL2FF;};_tmp5CA=_tmp5B9.f2;{struct Cyc_Absyn_TagType_struct*
_tmp5CB=(struct Cyc_Absyn_TagType_struct*)_tmp5CA;if(_tmp5CB->tag != 20)goto _LL2FF;};
_LL2FE: goto _LL300;_LL2FF: _tmp5CC=_tmp5B9.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp5CD=(struct Cyc_Absyn_FloatType_struct*)_tmp5CC;if(_tmp5CD->tag != 7)goto
_LL301;};_tmp5CE=_tmp5B9.f2;{struct Cyc_Absyn_TagType_struct*_tmp5CF=(struct Cyc_Absyn_TagType_struct*)
_tmp5CE;if(_tmp5CF->tag != 20)goto _LL301;};_LL300: goto _LL302;_LL301: _tmp5D0=
_tmp5B9.f1;{struct Cyc_Absyn_FloatType_struct*_tmp5D1=(struct Cyc_Absyn_FloatType_struct*)
_tmp5D0;if(_tmp5D1->tag != 7)goto _LL303;};_tmp5D2=_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*
_tmp5D3=(struct Cyc_Absyn_IntType_struct*)_tmp5D2;if(_tmp5D3->tag != 6)goto _LL303;};
_LL302: return 1;_LL303: _tmp5D4=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*_tmp5D5=(
struct Cyc_Absyn_IntType_struct*)_tmp5D4;if(_tmp5D5->tag != 6)goto _LL305;else{
_tmp5D6=_tmp5D5->f2;if(_tmp5D6 != Cyc_Absyn_LongLong_sz)goto _LL305;}};_tmp5D7=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp5D8=(struct Cyc_Absyn_IntType_struct*)
_tmp5D7;if(_tmp5D8->tag != 6)goto _LL305;else{_tmp5D9=_tmp5D8->f2;if(_tmp5D9 != Cyc_Absyn_LongLong_sz)
goto _LL305;}};_LL304: return 0;_LL305: _tmp5DA=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5DB=(struct Cyc_Absyn_IntType_struct*)_tmp5DA;if(_tmp5DB->tag != 6)goto _LL307;
else{_tmp5DC=_tmp5DB->f2;if(_tmp5DC != Cyc_Absyn_LongLong_sz)goto _LL307;}};_LL306:
return 1;_LL307: _tmp5DD=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*_tmp5DE=(
struct Cyc_Absyn_IntType_struct*)_tmp5DD;if(_tmp5DE->tag != 6)goto _LL309;else{
_tmp5DF=_tmp5DE->f2;if(_tmp5DF != Cyc_Absyn_Long_sz)goto _LL309;}};_tmp5E0=_tmp5B9.f2;{
struct Cyc_Absyn_IntType_struct*_tmp5E1=(struct Cyc_Absyn_IntType_struct*)_tmp5E0;
if(_tmp5E1->tag != 6)goto _LL309;else{_tmp5E2=_tmp5E1->f2;if(_tmp5E2 != Cyc_Absyn_Int_sz)
goto _LL309;}};_LL308: goto _LL30A;_LL309: _tmp5E3=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5E4=(struct Cyc_Absyn_IntType_struct*)_tmp5E3;if(_tmp5E4->tag != 6)goto _LL30B;
else{_tmp5E5=_tmp5E4->f2;if(_tmp5E5 != Cyc_Absyn_Int_sz)goto _LL30B;}};_tmp5E6=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp5E7=(struct Cyc_Absyn_IntType_struct*)
_tmp5E6;if(_tmp5E7->tag != 6)goto _LL30B;else{_tmp5E8=_tmp5E7->f2;if(_tmp5E8 != Cyc_Absyn_Long_sz)
goto _LL30B;}};_LL30A: return 0;_LL30B: _tmp5E9=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5EA=(struct Cyc_Absyn_IntType_struct*)_tmp5E9;if(_tmp5EA->tag != 6)goto _LL30D;
else{_tmp5EB=_tmp5EA->f2;if(_tmp5EB != Cyc_Absyn_Long_sz)goto _LL30D;}};_tmp5EC=
_tmp5B9.f2;{struct Cyc_Absyn_FloatType_struct*_tmp5ED=(struct Cyc_Absyn_FloatType_struct*)
_tmp5EC;if(_tmp5ED->tag != 7)goto _LL30D;};_LL30C: goto _LL30E;_LL30D: _tmp5EE=
_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*_tmp5EF=(struct Cyc_Absyn_IntType_struct*)
_tmp5EE;if(_tmp5EF->tag != 6)goto _LL30F;else{_tmp5F0=_tmp5EF->f2;if(_tmp5F0 != Cyc_Absyn_Int_sz)
goto _LL30F;}};_tmp5F1=_tmp5B9.f2;{struct Cyc_Absyn_FloatType_struct*_tmp5F2=(
struct Cyc_Absyn_FloatType_struct*)_tmp5F1;if(_tmp5F2->tag != 7)goto _LL30F;};
_LL30E: goto _LL310;_LL30F: _tmp5F3=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5F4=(struct Cyc_Absyn_IntType_struct*)_tmp5F3;if(_tmp5F4->tag != 6)goto _LL311;
else{_tmp5F5=_tmp5F4->f2;if(_tmp5F5 != Cyc_Absyn_Long_sz)goto _LL311;}};_tmp5F6=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp5F7=(struct Cyc_Absyn_IntType_struct*)
_tmp5F6;if(_tmp5F7->tag != 6)goto _LL311;else{_tmp5F8=_tmp5F7->f2;if(_tmp5F8 != Cyc_Absyn_Short_sz)
goto _LL311;}};_LL310: goto _LL312;_LL311: _tmp5F9=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp5FA=(struct Cyc_Absyn_IntType_struct*)_tmp5F9;if(_tmp5FA->tag != 6)goto _LL313;
else{_tmp5FB=_tmp5FA->f2;if(_tmp5FB != Cyc_Absyn_Int_sz)goto _LL313;}};_tmp5FC=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp5FD=(struct Cyc_Absyn_IntType_struct*)
_tmp5FC;if(_tmp5FD->tag != 6)goto _LL313;else{_tmp5FE=_tmp5FD->f2;if(_tmp5FE != Cyc_Absyn_Short_sz)
goto _LL313;}};_LL312: goto _LL314;_LL313: _tmp5FF=_tmp5B9.f1;{struct Cyc_Absyn_TagType_struct*
_tmp600=(struct Cyc_Absyn_TagType_struct*)_tmp5FF;if(_tmp600->tag != 20)goto _LL315;};
_tmp601=_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp602=(struct Cyc_Absyn_IntType_struct*)
_tmp601;if(_tmp602->tag != 6)goto _LL315;else{_tmp603=_tmp602->f2;if(_tmp603 != Cyc_Absyn_Short_sz)
goto _LL315;}};_LL314: goto _LL316;_LL315: _tmp604=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp605=(struct Cyc_Absyn_IntType_struct*)_tmp604;if(_tmp605->tag != 6)goto _LL317;
else{_tmp606=_tmp605->f2;if(_tmp606 != Cyc_Absyn_Long_sz)goto _LL317;}};_tmp607=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp608=(struct Cyc_Absyn_IntType_struct*)
_tmp607;if(_tmp608->tag != 6)goto _LL317;else{_tmp609=_tmp608->f2;if(_tmp609 != Cyc_Absyn_Char_sz)
goto _LL317;}};_LL316: goto _LL318;_LL317: _tmp60A=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp60B=(struct Cyc_Absyn_IntType_struct*)_tmp60A;if(_tmp60B->tag != 6)goto _LL319;
else{_tmp60C=_tmp60B->f2;if(_tmp60C != Cyc_Absyn_Int_sz)goto _LL319;}};_tmp60D=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp60E=(struct Cyc_Absyn_IntType_struct*)
_tmp60D;if(_tmp60E->tag != 6)goto _LL319;else{_tmp60F=_tmp60E->f2;if(_tmp60F != Cyc_Absyn_Char_sz)
goto _LL319;}};_LL318: goto _LL31A;_LL319: _tmp610=_tmp5B9.f1;{struct Cyc_Absyn_IntType_struct*
_tmp611=(struct Cyc_Absyn_IntType_struct*)_tmp610;if(_tmp611->tag != 6)goto _LL31B;
else{_tmp612=_tmp611->f2;if(_tmp612 != Cyc_Absyn_Short_sz)goto _LL31B;}};_tmp613=
_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp614=(struct Cyc_Absyn_IntType_struct*)
_tmp613;if(_tmp614->tag != 6)goto _LL31B;else{_tmp615=_tmp614->f2;if(_tmp615 != Cyc_Absyn_Char_sz)
goto _LL31B;}};_LL31A: goto _LL31C;_LL31B: _tmp616=_tmp5B9.f1;{struct Cyc_Absyn_TagType_struct*
_tmp617=(struct Cyc_Absyn_TagType_struct*)_tmp616;if(_tmp617->tag != 20)goto _LL31D;};
_tmp618=_tmp5B9.f2;{struct Cyc_Absyn_IntType_struct*_tmp619=(struct Cyc_Absyn_IntType_struct*)
_tmp618;if(_tmp619->tag != 6)goto _LL31D;else{_tmp61A=_tmp619->f2;if(_tmp61A != Cyc_Absyn_Char_sz)
goto _LL31D;}};_LL31C: return 1;_LL31D:;_LL31E: return 0;_LL2F6:;};}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct _RegionHandle*
_tmp61B=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*
el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(
t1)){if(max_arith_type == 0  || Cyc_Tcutil_will_lose_precision(t1,(void*)
max_arith_type->v)){struct Cyc_Core_Opt*_tmp1046;max_arith_type=((_tmp1046=
_region_malloc(_tmp61B,sizeof(*_tmp1046)),((_tmp1046->v=t1,_tmp1046))));}}}}if(
max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))return 0;}}{
struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmp104B;void*_tmp104A[2];struct
Cyc_String_pa_struct _tmp1049;struct Cyc_String_pa_struct _tmp1048;(_tmp1048.tag=0,((
_tmp1048.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((
_tmp1049.tag=0,((_tmp1049.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp104A[0]=& _tmp1049,((_tmp104A[1]=& _tmp1048,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmp104B="type mismatch: expecting %s but found %s",
_tag_dyneither(_tmp104B,sizeof(char),41))),_tag_dyneither(_tmp104A,sizeof(void*),
2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp621=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL320: {struct Cyc_Absyn_PointerType_struct*
_tmp622=(struct Cyc_Absyn_PointerType_struct*)_tmp621;if(_tmp622->tag != 5)goto
_LL322;}_LL321: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);
goto _LL31F;_LL322:;_LL323: return 0;_LL31F:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t);int Cyc_Tcutil_is_integral_type(void*t){void*_tmp623=Cyc_Tcutil_compress(
t);_LL325: {struct Cyc_Absyn_IntType_struct*_tmp624=(struct Cyc_Absyn_IntType_struct*)
_tmp623;if(_tmp624->tag != 6)goto _LL327;}_LL326: goto _LL328;_LL327: {struct Cyc_Absyn_TagType_struct*
_tmp625=(struct Cyc_Absyn_TagType_struct*)_tmp623;if(_tmp625->tag != 20)goto _LL329;}
_LL328: goto _LL32A;_LL329: {struct Cyc_Absyn_EnumType_struct*_tmp626=(struct Cyc_Absyn_EnumType_struct*)
_tmp623;if(_tmp626->tag != 14)goto _LL32B;}_LL32A: goto _LL32C;_LL32B: {struct Cyc_Absyn_AnonEnumType_struct*
_tmp627=(struct Cyc_Absyn_AnonEnumType_struct*)_tmp623;if(_tmp627->tag != 15)goto
_LL32D;}_LL32C: return 1;_LL32D:;_LL32E: return 0;_LL324:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmp104E;void*_tmp104D;(_tmp104D=0,Cyc_Tcutil_warn(e->loc,((_tmp104E="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp104E,sizeof(char),44))),_tag_dyneither(_tmp104D,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ))
return 1;if(Cyc_Tcutil_is_integral_type((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const char*_tmp1051;void*_tmp1050;(
_tmp1050=0,Cyc_Tcutil_warn(e->loc,((_tmp1051="integral size mismatch; conversion supplied",
_tag_dyneither(_tmp1051,sizeof(char),44))),_tag_dyneither(_tmp1050,sizeof(void*),
0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}return 0;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*assume,void*t1,void*t2);int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmp1052;struct _tuple0 _tmp62D=(
_tmp1052.f1=t1,((_tmp1052.f2=t2,_tmp1052)));void*_tmp62E;struct Cyc_Absyn_PtrInfo
_tmp630;void*_tmp631;struct Cyc_Absyn_PtrInfo _tmp633;void*_tmp634;struct Cyc_Absyn_ArrayInfo
_tmp636;void*_tmp637;struct Cyc_Absyn_Tqual _tmp638;struct Cyc_Absyn_Exp*_tmp639;
union Cyc_Absyn_Constraint*_tmp63A;void*_tmp63B;struct Cyc_Absyn_ArrayInfo _tmp63D;
void*_tmp63E;struct Cyc_Absyn_Tqual _tmp63F;struct Cyc_Absyn_Exp*_tmp640;union Cyc_Absyn_Constraint*
_tmp641;void*_tmp642;void*_tmp644;_LL330: _tmp62E=_tmp62D.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp62F=(struct Cyc_Absyn_PointerType_struct*)_tmp62E;if(_tmp62F->tag != 5)goto
_LL332;else{_tmp630=_tmp62F->f1;}};_tmp631=_tmp62D.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp632=(struct Cyc_Absyn_PointerType_struct*)_tmp631;if(_tmp632->tag != 5)goto
_LL332;else{_tmp633=_tmp632->f1;}};_LL331: {int okay=1;if(!((int(*)(int(*cmp)(int,
int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp630.ptr_atts).nullable,(_tmp633.ptr_atts).nullable))okay=!((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp630.ptr_atts).nullable);
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp630.ptr_atts).bounds,(
_tmp633.ptr_atts).bounds)){struct _tuple0 _tmp1053;struct _tuple0 _tmp647=(_tmp1053.f1=((
void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(
_tmp630.ptr_atts).bounds),((_tmp1053.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp633.ptr_atts).bounds),
_tmp1053)));void*_tmp648;void*_tmp64A;void*_tmp64C;void*_tmp64E;void*_tmp650;
struct Cyc_Absyn_Exp*_tmp652;void*_tmp653;struct Cyc_Absyn_Exp*_tmp655;void*
_tmp656;void*_tmp658;struct Cyc_Absyn_Exp*_tmp65A;_LL339: _tmp648=_tmp647.f1;{
struct Cyc_Absyn_Upper_b_struct*_tmp649=(struct Cyc_Absyn_Upper_b_struct*)_tmp648;
if(_tmp649->tag != 1)goto _LL33B;};_tmp64A=_tmp647.f2;{struct Cyc_Absyn_DynEither_b_struct*
_tmp64B=(struct Cyc_Absyn_DynEither_b_struct*)_tmp64A;if(_tmp64B->tag != 0)goto
_LL33B;};_LL33A: goto _LL33C;_LL33B: _tmp64C=_tmp647.f1;{struct Cyc_Absyn_DynEither_b_struct*
_tmp64D=(struct Cyc_Absyn_DynEither_b_struct*)_tmp64C;if(_tmp64D->tag != 0)goto
_LL33D;};_tmp64E=_tmp647.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp64F=(struct
Cyc_Absyn_DynEither_b_struct*)_tmp64E;if(_tmp64F->tag != 0)goto _LL33D;};_LL33C:
okay=1;goto _LL338;_LL33D: _tmp650=_tmp647.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp651=(struct Cyc_Absyn_Upper_b_struct*)_tmp650;if(_tmp651->tag != 1)goto _LL33F;
else{_tmp652=_tmp651->f1;}};_tmp653=_tmp647.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp654=(struct Cyc_Absyn_Upper_b_struct*)_tmp653;if(_tmp654->tag != 1)goto _LL33F;
else{_tmp655=_tmp654->f1;}};_LL33E: okay=okay  && Cyc_Evexp_lte_const_exp(_tmp655,
_tmp652);if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,(_tmp633.ptr_atts).zero_term)){const char*_tmp1056;void*_tmp1055;(_tmp1055=0,
Cyc_Tcutil_warn(loc,((_tmp1056="implicit cast to shorter array",_tag_dyneither(
_tmp1056,sizeof(char),31))),_tag_dyneither(_tmp1055,sizeof(void*),0)));}goto
_LL338;_LL33F: _tmp656=_tmp647.f1;{struct Cyc_Absyn_DynEither_b_struct*_tmp657=(
struct Cyc_Absyn_DynEither_b_struct*)_tmp656;if(_tmp657->tag != 0)goto _LL338;};
_tmp658=_tmp647.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp659=(struct Cyc_Absyn_Upper_b_struct*)
_tmp658;if(_tmp659->tag != 1)goto _LL338;else{_tmp65A=_tmp659->f1;}};_LL340: if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp630.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp633.ptr_atts).bounds))goto _LL338;okay=0;goto
_LL338;_LL338:;}okay=okay  && (!(_tmp630.elt_tq).real_const  || (_tmp633.elt_tq).real_const);
okay=okay  && (Cyc_Tcutil_unify((_tmp630.ptr_atts).rgn,(_tmp633.ptr_atts).rgn)
 || Cyc_Tcenv_region_outlives(te,(_tmp630.ptr_atts).rgn,(_tmp633.ptr_atts).rgn));
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp630.ptr_atts).zero_term,(
_tmp633.ptr_atts).zero_term) || ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
1,(_tmp630.ptr_atts).zero_term) && (_tmp633.elt_tq).real_const);okay=okay  && (
Cyc_Tcutil_unify(_tmp630.elt_typ,_tmp633.elt_typ) || Cyc_Tcutil_ptrsubtype(te,0,
_tmp630.elt_typ,_tmp633.elt_typ));return okay;}_LL332: _tmp634=_tmp62D.f1;{struct
Cyc_Absyn_ArrayType_struct*_tmp635=(struct Cyc_Absyn_ArrayType_struct*)_tmp634;
if(_tmp635->tag != 9)goto _LL334;else{_tmp636=_tmp635->f1;_tmp637=_tmp636.elt_type;
_tmp638=_tmp636.tq;_tmp639=_tmp636.num_elts;_tmp63A=_tmp636.zero_term;}};_tmp63B=
_tmp62D.f2;{struct Cyc_Absyn_ArrayType_struct*_tmp63C=(struct Cyc_Absyn_ArrayType_struct*)
_tmp63B;if(_tmp63C->tag != 9)goto _LL334;else{_tmp63D=_tmp63C->f1;_tmp63E=_tmp63D.elt_type;
_tmp63F=_tmp63D.tq;_tmp640=_tmp63D.num_elts;_tmp641=_tmp63D.zero_term;}};_LL333: {
int okay;okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp63A,_tmp641) && ((_tmp639 != 0
 && _tmp640 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp639,(
struct Cyc_Absyn_Exp*)_tmp640));return(okay  && Cyc_Tcutil_unify(_tmp637,_tmp63E))
 && (!_tmp638.real_const  || _tmp63F.real_const);}_LL334: _tmp642=_tmp62D.f1;{
struct Cyc_Absyn_TagType_struct*_tmp643=(struct Cyc_Absyn_TagType_struct*)_tmp642;
if(_tmp643->tag != 20)goto _LL336;};_tmp644=_tmp62D.f2;{struct Cyc_Absyn_IntType_struct*
_tmp645=(struct Cyc_Absyn_IntType_struct*)_tmp644;if(_tmp645->tag != 6)goto _LL336;};
_LL335: return 0;_LL336:;_LL337: return Cyc_Tcutil_unify(t1,t2);_LL32F:;};}int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*_tmp65D=Cyc_Tcutil_compress(t);
_LL342: {struct Cyc_Absyn_PointerType_struct*_tmp65E=(struct Cyc_Absyn_PointerType_struct*)
_tmp65D;if(_tmp65E->tag != 5)goto _LL344;}_LL343: return 1;_LL344:;_LL345: return 0;
_LL341:;}void*Cyc_Tcutil_pointer_elt_type(void*t);void*Cyc_Tcutil_pointer_elt_type(
void*t){void*_tmp65F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp661;void*
_tmp662;_LL347: {struct Cyc_Absyn_PointerType_struct*_tmp660=(struct Cyc_Absyn_PointerType_struct*)
_tmp65F;if(_tmp660->tag != 5)goto _LL349;else{_tmp661=_tmp660->f1;_tmp662=_tmp661.elt_typ;}}
_LL348: return _tmp662;_LL349:;_LL34A: {const char*_tmp1059;void*_tmp1058;(_tmp1058=
0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1059="pointer_elt_type",_tag_dyneither(_tmp1059,sizeof(char),17))),
_tag_dyneither(_tmp1058,sizeof(void*),0)));}_LL346:;}void*Cyc_Tcutil_pointer_region(
void*t);void*Cyc_Tcutil_pointer_region(void*t){void*_tmp665=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp667;struct Cyc_Absyn_PtrAtts _tmp668;struct Cyc_Absyn_PtrAtts*
_tmp669;_LL34C: {struct Cyc_Absyn_PointerType_struct*_tmp666=(struct Cyc_Absyn_PointerType_struct*)
_tmp665;if(_tmp666->tag != 5)goto _LL34E;else{_tmp667=_tmp666->f1;_tmp668=_tmp667.ptr_atts;
_tmp669=(struct Cyc_Absyn_PtrAtts*)&(_tmp666->f1).ptr_atts;}}_LL34D: return _tmp669->rgn;
_LL34E:;_LL34F: {const char*_tmp105C;void*_tmp105B;(_tmp105B=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp105C="pointer_elt_type",
_tag_dyneither(_tmp105C,sizeof(char),17))),_tag_dyneither(_tmp105B,sizeof(void*),
0)));}_LL34B:;}int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);int Cyc_Tcutil_rgn_of_pointer(
void*t,void**rgn){void*_tmp66C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp66E;struct Cyc_Absyn_PtrAtts _tmp66F;void*_tmp670;_LL351: {struct Cyc_Absyn_PointerType_struct*
_tmp66D=(struct Cyc_Absyn_PointerType_struct*)_tmp66C;if(_tmp66D->tag != 5)goto
_LL353;else{_tmp66E=_tmp66D->f1;_tmp66F=_tmp66E.ptr_atts;_tmp670=_tmp66F.rgn;}}
_LL352:*rgn=_tmp670;return 1;_LL353:;_LL354: return 0;_LL350:;}int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*
is_dyneither_ptr){void*_tmp671=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp673;struct Cyc_Absyn_PtrAtts _tmp674;union Cyc_Absyn_Constraint*_tmp675;_LL356: {
struct Cyc_Absyn_PointerType_struct*_tmp672=(struct Cyc_Absyn_PointerType_struct*)
_tmp671;if(_tmp672->tag != 5)goto _LL358;else{_tmp673=_tmp672->f1;_tmp674=_tmp673.ptr_atts;
_tmp675=_tmp674.bounds;}}_LL357:*is_dyneither_ptr=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp675)== (void*)& Cyc_Absyn_DynEither_b_val;
return 1;_LL358:;_LL359: return Cyc_Tcutil_typ_kind(t)== Cyc_Absyn_BoxKind;_LL355:;}
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e){void*_tmp676=e->r;union Cyc_Absyn_Cnst _tmp678;struct _tuple7 _tmp679;int _tmp67A;
union Cyc_Absyn_Cnst _tmp67C;struct _tuple5 _tmp67D;char _tmp67E;union Cyc_Absyn_Cnst
_tmp680;struct _tuple6 _tmp681;short _tmp682;union Cyc_Absyn_Cnst _tmp684;struct
_tuple8 _tmp685;long long _tmp686;void*_tmp688;struct Cyc_Absyn_Exp*_tmp689;_LL35B: {
struct Cyc_Absyn_Const_e_struct*_tmp677=(struct Cyc_Absyn_Const_e_struct*)_tmp676;
if(_tmp677->tag != 0)goto _LL35D;else{_tmp678=_tmp677->f1;if((_tmp678.Int_c).tag != 
4)goto _LL35D;_tmp679=(struct _tuple7)(_tmp678.Int_c).val;_tmp67A=_tmp679.f2;if(
_tmp67A != 0)goto _LL35D;}}_LL35C: goto _LL35E;_LL35D: {struct Cyc_Absyn_Const_e_struct*
_tmp67B=(struct Cyc_Absyn_Const_e_struct*)_tmp676;if(_tmp67B->tag != 0)goto _LL35F;
else{_tmp67C=_tmp67B->f1;if((_tmp67C.Char_c).tag != 2)goto _LL35F;_tmp67D=(struct
_tuple5)(_tmp67C.Char_c).val;_tmp67E=_tmp67D.f2;if(_tmp67E != 0)goto _LL35F;}}
_LL35E: goto _LL360;_LL35F: {struct Cyc_Absyn_Const_e_struct*_tmp67F=(struct Cyc_Absyn_Const_e_struct*)
_tmp676;if(_tmp67F->tag != 0)goto _LL361;else{_tmp680=_tmp67F->f1;if((_tmp680.Short_c).tag
!= 3)goto _LL361;_tmp681=(struct _tuple6)(_tmp680.Short_c).val;_tmp682=_tmp681.f2;
if(_tmp682 != 0)goto _LL361;}}_LL360: goto _LL362;_LL361: {struct Cyc_Absyn_Const_e_struct*
_tmp683=(struct Cyc_Absyn_Const_e_struct*)_tmp676;if(_tmp683->tag != 0)goto _LL363;
else{_tmp684=_tmp683->f1;if((_tmp684.LongLong_c).tag != 5)goto _LL363;_tmp685=(
struct _tuple8)(_tmp684.LongLong_c).val;_tmp686=_tmp685.f2;if(_tmp686 != 0)goto
_LL363;}}_LL362: return 1;_LL363: {struct Cyc_Absyn_Cast_e_struct*_tmp687=(struct
Cyc_Absyn_Cast_e_struct*)_tmp676;if(_tmp687->tag != 15)goto _LL365;else{_tmp688=(
void*)_tmp687->f1;_tmp689=_tmp687->f2;}}_LL364: return Cyc_Tcutil_is_zero(_tmp689)
 && Cyc_Tcutil_admits_zero(_tmp688);_LL365:;_LL366: return 0;_LL35A:;}struct Cyc_Core_Opt
Cyc_Tcutil_trk={(void*)Cyc_Absyn_TopRgnKind};struct Cyc_Core_Opt Cyc_Tcutil_urk={(
void*)Cyc_Absyn_UniqueRgnKind};struct Cyc_Core_Opt Cyc_Tcutil_rk={(void*)Cyc_Absyn_RgnKind};
struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)Cyc_Absyn_AnyKind};struct Cyc_Core_Opt Cyc_Tcutil_bk={(
void*)Cyc_Absyn_BoxKind};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)Cyc_Absyn_MemKind};
struct Cyc_Core_Opt Cyc_Tcutil_ik={(void*)Cyc_Absyn_IntKind};struct Cyc_Core_Opt Cyc_Tcutil_ek={(
void*)Cyc_Absyn_EffKind};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(enum Cyc_Absyn_Kind
k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(enum Cyc_Absyn_Kind k){switch(k){
case Cyc_Absyn_AnyKind: _LL367: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ak;case Cyc_Absyn_MemKind:
_LL368: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mk;case Cyc_Absyn_BoxKind: _LL369:
return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;case Cyc_Absyn_RgnKind: _LL36A: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk;case Cyc_Absyn_UniqueRgnKind: _LL36B: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_urk;case Cyc_Absyn_TopRgnKind: _LL36C: return(
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk;case Cyc_Absyn_EffKind: _LL36D: return(struct
Cyc_Core_Opt*)& Cyc_Tcutil_ek;case Cyc_Absyn_IntKind: _LL36E: return(struct Cyc_Core_Opt*)&
Cyc_Tcutil_ik;}}static void**Cyc_Tcutil_kind_to_b(enum Cyc_Absyn_Kind k);static
void**Cyc_Tcutil_kind_to_b(enum Cyc_Absyn_Kind k){static struct Cyc_Absyn_Eq_kb_struct
ab_v={0,Cyc_Absyn_AnyKind};static struct Cyc_Absyn_Eq_kb_struct mb_v={0,Cyc_Absyn_MemKind};
static struct Cyc_Absyn_Eq_kb_struct bb_v={0,Cyc_Absyn_BoxKind};static struct Cyc_Absyn_Eq_kb_struct
rb_v={0,Cyc_Absyn_RgnKind};static struct Cyc_Absyn_Eq_kb_struct ub_v={0,Cyc_Absyn_UniqueRgnKind};
static struct Cyc_Absyn_Eq_kb_struct tb_v={0,Cyc_Absyn_TopRgnKind};static struct Cyc_Absyn_Eq_kb_struct
eb_v={0,Cyc_Absyn_EffKind};static struct Cyc_Absyn_Eq_kb_struct ib_v={0,Cyc_Absyn_IntKind};
static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;static void*ub=(void*)& ub_v;static void*tb=(void*)& tb_v;
static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;switch(k){case Cyc_Absyn_AnyKind:
_LL370: return& ab;case Cyc_Absyn_MemKind: _LL371: return& mb;case Cyc_Absyn_BoxKind:
_LL372: return& bb;case Cyc_Absyn_RgnKind: _LL373: return& rb;case Cyc_Absyn_UniqueRgnKind:
_LL374: return& ub;case Cyc_Absyn_TopRgnKind: _LL375: return& tb;case Cyc_Absyn_EffKind:
_LL376: return& eb;case Cyc_Absyn_IntKind: _LL377: return& ib;}}void*Cyc_Tcutil_kind_to_bound(
enum Cyc_Absyn_Kind k);void*Cyc_Tcutil_kind_to_bound(enum Cyc_Absyn_Kind k){return*
Cyc_Tcutil_kind_to_b(k);}struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(enum 
Cyc_Absyn_Kind k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(enum Cyc_Absyn_Kind
k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmp105F;struct Cyc_Absyn_Const_e_struct*
_tmp105E;e1->r=(void*)((_tmp105E=_cycalloc(sizeof(*_tmp105E)),((_tmp105E[0]=((
_tmp105F.tag=0,((_tmp105F.f1=Cyc_Absyn_Null_c,_tmp105F)))),_tmp105E))));}{struct
Cyc_Core_Opt*_tmp694=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmp1069;struct Cyc_Absyn_PtrAtts _tmp1068;struct Cyc_Absyn_PtrInfo _tmp1067;struct
Cyc_Absyn_PointerType_struct*_tmp1066;struct Cyc_Absyn_PointerType_struct*_tmp695=(
_tmp1066=_cycalloc(sizeof(*_tmp1066)),((_tmp1066[0]=((_tmp1069.tag=5,((_tmp1069.f1=((
_tmp1067.elt_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp694),((
_tmp1067.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp1067.ptr_atts=((_tmp1068.rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,_tmp694),((_tmp1068.nullable=Cyc_Absyn_true_conref,((
_tmp1068.bounds=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp1068.zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((
_tmp1068.ptrloc=0,_tmp1068)))))))))),_tmp1067)))))),_tmp1069)))),_tmp1066)));((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp695);return Cyc_Tcutil_coerce_arg(
te,e1,t2);};}return 0;}struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion
c);struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL379: {const char*_tmp106A;return(
_tmp106A="unknown",_tag_dyneither(_tmp106A,sizeof(char),8));}case Cyc_Absyn_No_coercion:
_LL37A: {const char*_tmp106B;return(_tmp106B="no coercion",_tag_dyneither(
_tmp106B,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL37B: {const char*
_tmp106C;return(_tmp106C="null check",_tag_dyneither(_tmp106C,sizeof(char),11));}
case Cyc_Absyn_Other_coercion: _LL37C: {const char*_tmp106D;return(_tmp106D="other coercion",
_tag_dyneither(_tmp106D,sizeof(char),15));}}}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t2);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);enum Cyc_Absyn_Coercion c;if(Cyc_Tcutil_unify(t1,t2))
return 1;if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){const char*_tmp1072;void*_tmp1071[2];
struct Cyc_String_pa_struct _tmp1070;struct Cyc_String_pa_struct _tmp106F;(_tmp106F.tag=
0,((_tmp106F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmp1070.tag=0,((_tmp1070.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp1071[0]=& _tmp1070,((_tmp1071[1]=& _tmp106F,Cyc_Tcutil_warn(
e->loc,((_tmp1072="integral size mismatch; %s -> %s conversion supplied",
_tag_dyneither(_tmp1072,sizeof(char),53))),_tag_dyneither(_tmp1071,sizeof(void*),
2)))))))))))));}Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);return 1;}
else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2,Cyc_Absyn_Other_coercion);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,
e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != Cyc_Absyn_NonNull_to_Null){const char*
_tmp1077;void*_tmp1076[2];struct Cyc_String_pa_struct _tmp1075;struct Cyc_String_pa_struct
_tmp1074;(_tmp1074.tag=0,((_tmp1074.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp1075.tag=0,((_tmp1075.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((
_tmp1076[0]=& _tmp1075,((_tmp1076[1]=& _tmp1074,Cyc_Tcutil_warn(e->loc,((_tmp1077="implicit cast from %s to %s",
_tag_dyneither(_tmp1077,sizeof(char),28))),_tag_dyneither(_tmp1076,sizeof(void*),
2)))))))))))));}return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp6A6=Cyc_Tcutil_compress(t);
_LL37F: {struct Cyc_Absyn_IntType_struct*_tmp6A7=(struct Cyc_Absyn_IntType_struct*)
_tmp6A6;if(_tmp6A7->tag != 6)goto _LL381;}_LL380: goto _LL382;_LL381: {struct Cyc_Absyn_FloatType_struct*
_tmp6A8=(struct Cyc_Absyn_FloatType_struct*)_tmp6A6;if(_tmp6A8->tag != 7)goto
_LL383;}_LL382: goto _LL384;_LL383: {struct Cyc_Absyn_DoubleType_struct*_tmp6A9=(
struct Cyc_Absyn_DoubleType_struct*)_tmp6A6;if(_tmp6A9->tag != 8)goto _LL385;}
_LL384: return 1;_LL385:;_LL386: return 0;_LL37E:;}static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple11*Cyc_Tcutil_flatten_typ_f(
struct _tuple15*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp6AB;
struct _RegionHandle*_tmp6AC;struct _tuple15 _tmp6AA=*env;_tmp6AB=_tmp6AA.f1;
_tmp6AC=_tmp6AA.f2;{struct _tuple11*_tmp1078;return(_tmp1078=_region_malloc(
_tmp6AC,sizeof(*_tmp1078)),((_tmp1078->f1=x->tq,((_tmp1078->f2=Cyc_Tcutil_rsubstitute(
_tmp6AC,_tmp6AB,x->type),_tmp1078)))));};}static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x);static struct _tuple11*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple11*x){struct Cyc_Absyn_Tqual _tmp6AF;void*
_tmp6B0;struct _tuple11 _tmp6AE=*x;_tmp6AF=_tmp6AE.f1;_tmp6B0=_tmp6AE.f2;{struct
_tuple11*_tmp1079;return(_tmp1079=_region_malloc(r,sizeof(*_tmp1079)),((_tmp1079->f1=
_tmp6AF,((_tmp1079->f2=_tmp6B0,_tmp1079)))));};}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp6B2=t1;struct Cyc_List_List*_tmp6B5;struct Cyc_Absyn_AggrInfo
_tmp6B7;union Cyc_Absyn_AggrInfoU _tmp6B8;struct Cyc_Absyn_Aggrdecl**_tmp6B9;struct
Cyc_Absyn_Aggrdecl*_tmp6BA;struct Cyc_List_List*_tmp6BB;enum Cyc_Absyn_AggrKind
_tmp6BD;struct Cyc_List_List*_tmp6BE;_LL388: {struct Cyc_Absyn_VoidType_struct*
_tmp6B3=(struct Cyc_Absyn_VoidType_struct*)_tmp6B2;if(_tmp6B3->tag != 0)goto _LL38A;}
_LL389: return 0;_LL38A: {struct Cyc_Absyn_TupleType_struct*_tmp6B4=(struct Cyc_Absyn_TupleType_struct*)
_tmp6B2;if(_tmp6B4->tag != 11)goto _LL38C;else{_tmp6B5=_tmp6B4->f1;}}_LL38B: return((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_RegionHandle*,struct _tuple11*),struct _RegionHandle*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp6B5);_LL38C: {struct Cyc_Absyn_AggrType_struct*
_tmp6B6=(struct Cyc_Absyn_AggrType_struct*)_tmp6B2;if(_tmp6B6->tag != 12)goto
_LL38E;else{_tmp6B7=_tmp6B6->f1;_tmp6B8=_tmp6B7.aggr_info;if((_tmp6B8.KnownAggr).tag
!= 2)goto _LL38E;_tmp6B9=(struct Cyc_Absyn_Aggrdecl**)(_tmp6B8.KnownAggr).val;
_tmp6BA=*_tmp6B9;_tmp6BB=_tmp6B7.targs;}}_LL38D: if(((_tmp6BA->kind == Cyc_Absyn_UnionA
 || _tmp6BA->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6BA->impl))->exist_vars
!= 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6BA->impl))->rgn_po != 0){
struct _tuple11*_tmp107C;struct Cyc_List_List*_tmp107B;return(_tmp107B=
_region_malloc(r,sizeof(*_tmp107B)),((_tmp107B->hd=((_tmp107C=_region_malloc(r,
sizeof(*_tmp107C)),((_tmp107C->f1=Cyc_Absyn_const_tqual(0),((_tmp107C->f2=t1,
_tmp107C)))))),((_tmp107B->tl=0,_tmp107B)))));}{struct Cyc_List_List*_tmp6C1=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp6BA->tvs,_tmp6BB);struct _tuple15
_tmp107D;struct _tuple15 env=(_tmp107D.f1=_tmp6C1,((_tmp107D.f2=r,_tmp107D)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct
_tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))
Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp6BA->impl))->fields);};_LL38E: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp6BC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp6B2;if(_tmp6BC->tag != 13)goto
_LL390;else{_tmp6BD=_tmp6BC->f1;if(_tmp6BD != Cyc_Absyn_StructA)goto _LL390;
_tmp6BE=_tmp6BC->f2;}}_LL38F: {struct _tuple15 _tmp107E;struct _tuple15 env=(
_tmp107E.f1=0,((_tmp107E.f2=r,_tmp107E)));return((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple11*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),
struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,&
env,_tmp6BE);}_LL390:;_LL391: {struct _tuple11*_tmp1081;struct Cyc_List_List*
_tmp1080;return(_tmp1080=_region_malloc(r,sizeof(*_tmp1080)),((_tmp1080->hd=((
_tmp1081=_region_malloc(r,sizeof(*_tmp1081)),((_tmp1081->f1=Cyc_Absyn_const_tqual(
0),((_tmp1081->f2=t1,_tmp1081)))))),((_tmp1080->tl=0,_tmp1080)))));}_LL387:;};}
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2);
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){{
struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp6C6=(void*)t->hd;_LL393: {
struct Cyc_Absyn_Pure_att_struct*_tmp6C7=(struct Cyc_Absyn_Pure_att_struct*)
_tmp6C6;if(_tmp6C7->tag != 21)goto _LL395;}_LL394: goto _LL396;_LL395: {struct Cyc_Absyn_Noreturn_att_struct*
_tmp6C8=(struct Cyc_Absyn_Noreturn_att_struct*)_tmp6C6;if(_tmp6C8->tag != 4)goto
_LL397;}_LL396: goto _LL398;_LL397: {struct Cyc_Absyn_Initializes_att_struct*
_tmp6C9=(struct Cyc_Absyn_Initializes_att_struct*)_tmp6C6;if(_tmp6C9->tag != 20)
goto _LL399;}_LL398: continue;_LL399:;_LL39A: if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,
a2))return 0;_LL392:;}}for(0;a2 != 0;a2=a2->tl){if(!((int(*)(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)
a2->hd,a1))return 0;}return 1;}static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmp1082;struct _tuple0 _tmp6CB=(_tmp1082.f1=t1,((_tmp1082.f2=t2,_tmp1082)));void*
_tmp6CC;struct Cyc_Absyn_PtrInfo _tmp6CE;void*_tmp6CF;struct Cyc_Absyn_Tqual _tmp6D0;
struct Cyc_Absyn_PtrAtts _tmp6D1;void*_tmp6D2;union Cyc_Absyn_Constraint*_tmp6D3;
union Cyc_Absyn_Constraint*_tmp6D4;union Cyc_Absyn_Constraint*_tmp6D5;void*_tmp6D6;
struct Cyc_Absyn_PtrInfo _tmp6D8;void*_tmp6D9;struct Cyc_Absyn_Tqual _tmp6DA;struct
Cyc_Absyn_PtrAtts _tmp6DB;void*_tmp6DC;union Cyc_Absyn_Constraint*_tmp6DD;union Cyc_Absyn_Constraint*
_tmp6DE;union Cyc_Absyn_Constraint*_tmp6DF;void*_tmp6E0;struct Cyc_Absyn_DatatypeFieldInfo
_tmp6E2;union Cyc_Absyn_DatatypeFieldInfoU _tmp6E3;struct _tuple3 _tmp6E4;struct Cyc_Absyn_Datatypedecl*
_tmp6E5;struct Cyc_Absyn_Datatypefield*_tmp6E6;struct Cyc_List_List*_tmp6E7;void*
_tmp6E8;struct Cyc_Absyn_DatatypeInfo _tmp6EA;union Cyc_Absyn_DatatypeInfoU _tmp6EB;
struct Cyc_Absyn_Datatypedecl**_tmp6EC;struct Cyc_Absyn_Datatypedecl*_tmp6ED;
struct Cyc_List_List*_tmp6EE;void*_tmp6EF;struct Cyc_Absyn_FnInfo _tmp6F1;void*
_tmp6F2;struct Cyc_Absyn_FnInfo _tmp6F4;_LL39C: _tmp6CC=_tmp6CB.f1;{struct Cyc_Absyn_PointerType_struct*
_tmp6CD=(struct Cyc_Absyn_PointerType_struct*)_tmp6CC;if(_tmp6CD->tag != 5)goto
_LL39E;else{_tmp6CE=_tmp6CD->f1;_tmp6CF=_tmp6CE.elt_typ;_tmp6D0=_tmp6CE.elt_tq;
_tmp6D1=_tmp6CE.ptr_atts;_tmp6D2=_tmp6D1.rgn;_tmp6D3=_tmp6D1.nullable;_tmp6D4=
_tmp6D1.bounds;_tmp6D5=_tmp6D1.zero_term;}};_tmp6D6=_tmp6CB.f2;{struct Cyc_Absyn_PointerType_struct*
_tmp6D7=(struct Cyc_Absyn_PointerType_struct*)_tmp6D6;if(_tmp6D7->tag != 5)goto
_LL39E;else{_tmp6D8=_tmp6D7->f1;_tmp6D9=_tmp6D8.elt_typ;_tmp6DA=_tmp6D8.elt_tq;
_tmp6DB=_tmp6D8.ptr_atts;_tmp6DC=_tmp6DB.rgn;_tmp6DD=_tmp6DB.nullable;_tmp6DE=
_tmp6DB.bounds;_tmp6DF=_tmp6DB.zero_term;}};_LL39D: if(_tmp6D0.real_const  && !
_tmp6DA.real_const)return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6D3,
_tmp6DD) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6D3)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6DD))return 0;if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,
union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp6D5,
_tmp6DF) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6D5)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp6DF))return 0;if(!Cyc_Tcutil_unify(_tmp6D2,_tmp6DC) && !Cyc_Tcenv_region_outlives(
te,_tmp6D2,_tmp6DC))return 0;if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,
_tmp6D4,_tmp6DE)){struct _tuple0 _tmp1083;struct _tuple0 _tmp6F6=(_tmp1083.f1=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6D4),((_tmp1083.f2=((void*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp6DE),_tmp1083)));void*
_tmp6F7;void*_tmp6F9;void*_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FD;void*_tmp6FE;
struct Cyc_Absyn_Exp*_tmp700;_LL3A5: _tmp6F7=_tmp6F6.f1;{struct Cyc_Absyn_Upper_b_struct*
_tmp6F8=(struct Cyc_Absyn_Upper_b_struct*)_tmp6F7;if(_tmp6F8->tag != 1)goto _LL3A7;};
_tmp6F9=_tmp6F6.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmp6FA=(struct Cyc_Absyn_DynEither_b_struct*)
_tmp6F9;if(_tmp6FA->tag != 0)goto _LL3A7;};_LL3A6: goto _LL3A4;_LL3A7: _tmp6FB=
_tmp6F6.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp6FC=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6FB;if(_tmp6FC->tag != 1)goto _LL3A9;else{_tmp6FD=_tmp6FC->f1;}};_tmp6FE=
_tmp6F6.f2;{struct Cyc_Absyn_Upper_b_struct*_tmp6FF=(struct Cyc_Absyn_Upper_b_struct*)
_tmp6FE;if(_tmp6FF->tag != 1)goto _LL3A9;else{_tmp700=_tmp6FF->f1;}};_LL3A8: if(!
Cyc_Evexp_lte_const_exp(_tmp700,_tmp6FD))return 0;goto _LL3A4;_LL3A9:;_LL3AA:
return 0;_LL3A4:;}{struct _tuple0*_tmp1086;struct Cyc_List_List*_tmp1085;return Cyc_Tcutil_ptrsubtype(
te,((_tmp1085=_cycalloc(sizeof(*_tmp1085)),((_tmp1085->hd=((_tmp1086=_cycalloc(
sizeof(*_tmp1086)),((_tmp1086->f1=t1,((_tmp1086->f2=t2,_tmp1086)))))),((_tmp1085->tl=
assume,_tmp1085)))))),_tmp6CF,_tmp6D9);};_LL39E: _tmp6E0=_tmp6CB.f1;{struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp6E1=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp6E0;if(_tmp6E1->tag != 4)
goto _LL3A0;else{_tmp6E2=_tmp6E1->f1;_tmp6E3=_tmp6E2.field_info;if((_tmp6E3.KnownDatatypefield).tag
!= 2)goto _LL3A0;_tmp6E4=(struct _tuple3)(_tmp6E3.KnownDatatypefield).val;_tmp6E5=
_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;_tmp6E7=_tmp6E2.targs;}};_tmp6E8=_tmp6CB.f2;{
struct Cyc_Absyn_DatatypeType_struct*_tmp6E9=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp6E8;if(_tmp6E9->tag != 3)goto _LL3A0;else{_tmp6EA=_tmp6E9->f1;_tmp6EB=_tmp6EA.datatype_info;
if((_tmp6EB.KnownDatatype).tag != 2)goto _LL3A0;_tmp6EC=(struct Cyc_Absyn_Datatypedecl**)(
_tmp6EB.KnownDatatype).val;_tmp6ED=*_tmp6EC;_tmp6EE=_tmp6EA.targs;}};_LL39F: if(
_tmp6E5 != _tmp6ED  && Cyc_Absyn_qvar_cmp(_tmp6E5->name,_tmp6ED->name)!= 0)return 0;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E7)!= ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp6EE))return 0;for(0;_tmp6E7 != 0;(_tmp6E7=
_tmp6E7->tl,_tmp6EE=_tmp6EE->tl)){if(!Cyc_Tcutil_unify((void*)_tmp6E7->hd,(void*)((
struct Cyc_List_List*)_check_null(_tmp6EE))->hd))return 0;}return 1;_LL3A0: _tmp6EF=
_tmp6CB.f1;{struct Cyc_Absyn_FnType_struct*_tmp6F0=(struct Cyc_Absyn_FnType_struct*)
_tmp6EF;if(_tmp6F0->tag != 10)goto _LL3A2;else{_tmp6F1=_tmp6F0->f1;}};_tmp6F2=
_tmp6CB.f2;{struct Cyc_Absyn_FnType_struct*_tmp6F3=(struct Cyc_Absyn_FnType_struct*)
_tmp6F2;if(_tmp6F3->tag != 10)goto _LL3A2;else{_tmp6F4=_tmp6F3->f1;}};_LL3A1: if(
_tmp6F1.tvars != 0  || _tmp6F4.tvars != 0){struct Cyc_List_List*_tmp703=_tmp6F1.tvars;
struct Cyc_List_List*_tmp704=_tmp6F4.tvars;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp703)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp704))return 0;{
struct _RegionHandle*_tmp705=Cyc_Tcenv_get_fnrgn(te);struct Cyc_List_List*inst=0;
while(_tmp703 != 0){if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)
_tmp703->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp704))->hd)->kind))return 0;{struct _tuple14*_tmp1090;struct Cyc_Absyn_VarType_struct
_tmp108F;struct Cyc_Absyn_VarType_struct*_tmp108E;struct Cyc_List_List*_tmp108D;
inst=((_tmp108D=_region_malloc(_tmp705,sizeof(*_tmp108D)),((_tmp108D->hd=((
_tmp1090=_region_malloc(_tmp705,sizeof(*_tmp1090)),((_tmp1090->f1=(struct Cyc_Absyn_Tvar*)
_tmp704->hd,((_tmp1090->f2=(void*)((_tmp108E=_cycalloc(sizeof(*_tmp108E)),((
_tmp108E[0]=((_tmp108F.tag=2,((_tmp108F.f1=(struct Cyc_Absyn_Tvar*)_tmp703->hd,
_tmp108F)))),_tmp108E)))),_tmp1090)))))),((_tmp108D->tl=inst,_tmp108D))))));}
_tmp703=_tmp703->tl;_tmp704=_tmp704->tl;}if(inst != 0){_tmp6F1.tvars=0;_tmp6F4.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmp1096;struct Cyc_Absyn_FnType_struct*_tmp1095;
struct Cyc_Absyn_FnType_struct _tmp1093;struct Cyc_Absyn_FnType_struct*_tmp1092;
return Cyc_Tcutil_subtype(te,assume,(void*)((_tmp1092=_cycalloc(sizeof(*_tmp1092)),((
_tmp1092[0]=((_tmp1093.tag=10,((_tmp1093.f1=_tmp6F1,_tmp1093)))),_tmp1092)))),(
void*)((_tmp1095=_cycalloc(sizeof(*_tmp1095)),((_tmp1095[0]=((_tmp1096.tag=10,((
_tmp1096.f1=_tmp6F4,_tmp1096)))),_tmp1095)))));};}};}if(!Cyc_Tcutil_subtype(te,
assume,_tmp6F1.ret_typ,_tmp6F4.ret_typ))return 0;{struct Cyc_List_List*_tmp70E=
_tmp6F1.args;struct Cyc_List_List*_tmp70F=_tmp6F4.args;if(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp70E)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp70F))return 0;for(0;_tmp70E != 0;(_tmp70E=_tmp70E->tl,_tmp70F=_tmp70F->tl)){
struct Cyc_Absyn_Tqual _tmp711;void*_tmp712;struct _tuple9 _tmp710=*((struct _tuple9*)
_tmp70E->hd);_tmp711=_tmp710.f2;_tmp712=_tmp710.f3;{struct Cyc_Absyn_Tqual _tmp714;
void*_tmp715;struct _tuple9 _tmp713=*((struct _tuple9*)((struct Cyc_List_List*)
_check_null(_tmp70F))->hd);_tmp714=_tmp713.f2;_tmp715=_tmp713.f3;if(_tmp714.real_const
 && !_tmp711.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp715,_tmp712))return
0;};}if(_tmp6F1.c_varargs != _tmp6F4.c_varargs)return 0;if(_tmp6F1.cyc_varargs != 0
 && _tmp6F4.cyc_varargs != 0){struct Cyc_Absyn_VarargInfo _tmp716=*_tmp6F1.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp717=*_tmp6F4.cyc_varargs;if((_tmp717.tq).real_const
 && !(_tmp716.tq).real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp717.type,
_tmp716.type))return 0;}else{if(_tmp6F1.cyc_varargs != 0  || _tmp6F4.cyc_varargs != 
0)return 0;}if(!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp6F1.effect))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp6F4.effect))->v))
return 0;if(!Cyc_Tcutil_sub_rgnpo(_tmp6F1.rgn_po,_tmp6F4.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(
_tmp6F1.attributes,_tmp6F4.attributes))return 0;return 1;};_LL3A2:;_LL3A3: return 0;
_LL39B:;};}static int Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(
void*t1,void*t2){struct _tuple0 _tmp1097;struct _tuple0 _tmp719=(_tmp1097.f1=Cyc_Tcutil_compress(
t1),((_tmp1097.f2=Cyc_Tcutil_compress(t2),_tmp1097)));void*_tmp71A;enum Cyc_Absyn_Size_of
_tmp71C;void*_tmp71D;enum Cyc_Absyn_Size_of _tmp71F;_LL3AC: _tmp71A=_tmp719.f1;{
struct Cyc_Absyn_IntType_struct*_tmp71B=(struct Cyc_Absyn_IntType_struct*)_tmp71A;
if(_tmp71B->tag != 6)goto _LL3AE;else{_tmp71C=_tmp71B->f2;}};_tmp71D=_tmp719.f2;{
struct Cyc_Absyn_IntType_struct*_tmp71E=(struct Cyc_Absyn_IntType_struct*)_tmp71D;
if(_tmp71E->tag != 6)goto _LL3AE;else{_tmp71F=_tmp71E->f2;}};_LL3AD: return(_tmp71C
== _tmp71F  || _tmp71C == Cyc_Absyn_Int_sz  && _tmp71F == Cyc_Absyn_Long_sz) || 
_tmp71C == Cyc_Absyn_Long_sz  && _tmp71F == Cyc_Absyn_Int_sz;_LL3AE:;_LL3AF: return 0;
_LL3AB:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct _RegionHandle*_tmp720=Cyc_Tcenv_get_fnrgn(
te);struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp720,te,t1);struct Cyc_List_List*
tqs2=Cyc_Tcutil_flatten_typ(_tmp720,te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=
tqs1->tl)){if(tqs1 == 0)return 0;{struct _tuple11 _tmp722;struct Cyc_Absyn_Tqual
_tmp723;void*_tmp724;struct _tuple11*_tmp721=(struct _tuple11*)tqs1->hd;_tmp722=*
_tmp721;_tmp723=_tmp722.f1;_tmp724=_tmp722.f2;{struct _tuple11 _tmp726;struct Cyc_Absyn_Tqual
_tmp727;void*_tmp728;struct _tuple11*_tmp725=(struct _tuple11*)tqs2->hd;_tmp726=*
_tmp725;_tmp727=_tmp726.f1;_tmp728=_tmp726.f2;if(_tmp727.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp724,_tmp728))continue;else{if(Cyc_Tcutil_unify(_tmp724,_tmp728))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp724,_tmp728))continue;else{return 0;}}}};};}
return 1;}static int Cyc_Tcutil_is_char_type(void*t);static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp729=Cyc_Tcutil_compress(t);enum Cyc_Absyn_Size_of _tmp72B;_LL3B1: {
struct Cyc_Absyn_IntType_struct*_tmp72A=(struct Cyc_Absyn_IntType_struct*)_tmp729;
if(_tmp72A->tag != 6)goto _LL3B3;else{_tmp72B=_tmp72A->f2;if(_tmp72B != Cyc_Absyn_Char_sz)
goto _LL3B3;}}_LL3B2: return 1;_LL3B3:;_LL3B4: return 0;_LL3B0:;}enum Cyc_Absyn_Coercion
Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*
t1,void*t2);enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);
if(t2 == (void*)& Cyc_Absyn_VoidType_val)return Cyc_Absyn_No_coercion;{void*_tmp72C=
t2;enum Cyc_Absyn_Size_of _tmp72E;enum Cyc_Absyn_Size_of _tmp730;_LL3B6: {struct
Cyc_Absyn_IntType_struct*_tmp72D=(struct Cyc_Absyn_IntType_struct*)_tmp72C;if(
_tmp72D->tag != 6)goto _LL3B8;else{_tmp72E=_tmp72D->f2;if(_tmp72E != Cyc_Absyn_Int_sz)
goto _LL3B8;}}_LL3B7: goto _LL3B9;_LL3B8: {struct Cyc_Absyn_IntType_struct*_tmp72F=(
struct Cyc_Absyn_IntType_struct*)_tmp72C;if(_tmp72F->tag != 6)goto _LL3BA;else{
_tmp730=_tmp72F->f2;if(_tmp730 != Cyc_Absyn_Long_sz)goto _LL3BA;}}_LL3B9: if(Cyc_Tcutil_typ_kind(
t1)== Cyc_Absyn_BoxKind)return Cyc_Absyn_No_coercion;goto _LL3B5;_LL3BA:;_LL3BB:
goto _LL3B5;_LL3B5:;}{void*_tmp731=t1;struct Cyc_Absyn_PtrInfo _tmp733;void*_tmp734;
struct Cyc_Absyn_Tqual _tmp735;struct Cyc_Absyn_PtrAtts _tmp736;void*_tmp737;union
Cyc_Absyn_Constraint*_tmp738;union Cyc_Absyn_Constraint*_tmp739;union Cyc_Absyn_Constraint*
_tmp73A;struct Cyc_Absyn_ArrayInfo _tmp73C;void*_tmp73D;struct Cyc_Absyn_Tqual
_tmp73E;struct Cyc_Absyn_Exp*_tmp73F;union Cyc_Absyn_Constraint*_tmp740;struct Cyc_Absyn_Enumdecl*
_tmp742;void*_tmp747;_LL3BD: {struct Cyc_Absyn_PointerType_struct*_tmp732=(struct
Cyc_Absyn_PointerType_struct*)_tmp731;if(_tmp732->tag != 5)goto _LL3BF;else{
_tmp733=_tmp732->f1;_tmp734=_tmp733.elt_typ;_tmp735=_tmp733.elt_tq;_tmp736=
_tmp733.ptr_atts;_tmp737=_tmp736.rgn;_tmp738=_tmp736.nullable;_tmp739=_tmp736.bounds;
_tmp73A=_tmp736.zero_term;}}_LL3BE:{void*_tmp748=t2;struct Cyc_Absyn_PtrInfo
_tmp74A;void*_tmp74B;struct Cyc_Absyn_Tqual _tmp74C;struct Cyc_Absyn_PtrAtts _tmp74D;
void*_tmp74E;union Cyc_Absyn_Constraint*_tmp74F;union Cyc_Absyn_Constraint*_tmp750;
union Cyc_Absyn_Constraint*_tmp751;_LL3CE: {struct Cyc_Absyn_PointerType_struct*
_tmp749=(struct Cyc_Absyn_PointerType_struct*)_tmp748;if(_tmp749->tag != 5)goto
_LL3D0;else{_tmp74A=_tmp749->f1;_tmp74B=_tmp74A.elt_typ;_tmp74C=_tmp74A.elt_tq;
_tmp74D=_tmp74A.ptr_atts;_tmp74E=_tmp74D.rgn;_tmp74F=_tmp74D.nullable;_tmp750=
_tmp74D.bounds;_tmp751=_tmp74D.zero_term;}}_LL3CF: {enum Cyc_Absyn_Coercion
coercion=Cyc_Absyn_Other_coercion;struct _tuple0*_tmp109A;struct Cyc_List_List*
_tmp1099;struct Cyc_List_List*_tmp752=(_tmp1099=_cycalloc(sizeof(*_tmp1099)),((
_tmp1099->hd=((_tmp109A=_cycalloc(sizeof(*_tmp109A)),((_tmp109A->f1=t1,((
_tmp109A->f2=t2,_tmp109A)))))),((_tmp1099->tl=0,_tmp1099)))));int _tmp753=Cyc_Tcutil_ptrsubtype(
te,_tmp752,_tmp734,_tmp74B) && (!_tmp735.real_const  || _tmp74C.real_const);Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp73A,
_tmp751) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp751);
int _tmp754=_tmp753?0:((Cyc_Tcutil_bits_only(_tmp734) && Cyc_Tcutil_is_char_type(
_tmp74B)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmp751)) && (_tmp74C.real_const  || !_tmp735.real_const);int bounds_ok=((int(*)(
int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))
Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp739,_tmp750);if(!
bounds_ok  && !_tmp754){struct _tuple0 _tmp109B;struct _tuple0 _tmp756=(_tmp109B.f1=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp739),((_tmp109B.f2=((
void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp750),_tmp109B)));
void*_tmp757;struct Cyc_Absyn_Exp*_tmp759;void*_tmp75A;struct Cyc_Absyn_Exp*
_tmp75C;_LL3D3: _tmp757=_tmp756.f1;{struct Cyc_Absyn_Upper_b_struct*_tmp758=(
struct Cyc_Absyn_Upper_b_struct*)_tmp757;if(_tmp758->tag != 1)goto _LL3D5;else{
_tmp759=_tmp758->f1;}};_tmp75A=_tmp756.f2;{struct Cyc_Absyn_Upper_b_struct*
_tmp75B=(struct Cyc_Absyn_Upper_b_struct*)_tmp75A;if(_tmp75B->tag != 1)goto _LL3D5;
else{_tmp75C=_tmp75B->f1;}};_LL3D4: if(Cyc_Evexp_lte_const_exp(_tmp75C,_tmp759))
bounds_ok=1;goto _LL3D2;_LL3D5:;_LL3D6: bounds_ok=1;goto _LL3D2;_LL3D2:;}if(((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp738) && !((int(*)(
int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp74F))coercion=Cyc_Absyn_NonNull_to_Null;
if(((bounds_ok  && zeroterm_ok) && (_tmp753  || _tmp754)) && (Cyc_Tcutil_unify(
_tmp737,_tmp74E) || Cyc_Tcenv_region_outlives(te,_tmp737,_tmp74E)))return
coercion;else{return Cyc_Absyn_Unknown_coercion;}};}_LL3D0:;_LL3D1: goto _LL3CD;
_LL3CD:;}return Cyc_Absyn_Unknown_coercion;_LL3BF: {struct Cyc_Absyn_ArrayType_struct*
_tmp73B=(struct Cyc_Absyn_ArrayType_struct*)_tmp731;if(_tmp73B->tag != 9)goto
_LL3C1;else{_tmp73C=_tmp73B->f1;_tmp73D=_tmp73C.elt_type;_tmp73E=_tmp73C.tq;
_tmp73F=_tmp73C.num_elts;_tmp740=_tmp73C.zero_term;}}_LL3C0:{void*_tmp75F=t2;
struct Cyc_Absyn_ArrayInfo _tmp761;void*_tmp762;struct Cyc_Absyn_Tqual _tmp763;
struct Cyc_Absyn_Exp*_tmp764;union Cyc_Absyn_Constraint*_tmp765;_LL3D8: {struct Cyc_Absyn_ArrayType_struct*
_tmp760=(struct Cyc_Absyn_ArrayType_struct*)_tmp75F;if(_tmp760->tag != 9)goto
_LL3DA;else{_tmp761=_tmp760->f1;_tmp762=_tmp761.elt_type;_tmp763=_tmp761.tq;
_tmp764=_tmp761.num_elts;_tmp765=_tmp761.zero_term;}}_LL3D9: {int okay;okay=((
_tmp73F != 0  && _tmp764 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*
x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp740,
_tmp765)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp764,(struct Cyc_Absyn_Exp*)
_tmp73F);return(okay  && Cyc_Tcutil_unify(_tmp73D,_tmp762)) && (!_tmp73E.real_const
 || _tmp763.real_const)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;}
_LL3DA:;_LL3DB: return Cyc_Absyn_Unknown_coercion;_LL3D7:;}return Cyc_Absyn_Unknown_coercion;
_LL3C1: {struct Cyc_Absyn_EnumType_struct*_tmp741=(struct Cyc_Absyn_EnumType_struct*)
_tmp731;if(_tmp741->tag != 14)goto _LL3C3;else{_tmp742=_tmp741->f2;}}_LL3C2:{void*
_tmp766=t2;struct Cyc_Absyn_Enumdecl*_tmp768;_LL3DD: {struct Cyc_Absyn_EnumType_struct*
_tmp767=(struct Cyc_Absyn_EnumType_struct*)_tmp766;if(_tmp767->tag != 14)goto
_LL3DF;else{_tmp768=_tmp767->f2;}}_LL3DE: if((_tmp742->fields != 0  && _tmp768->fields
!= 0) && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp742->fields))->v)>= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp768->fields))->v))return Cyc_Absyn_No_coercion;goto _LL3DC;_LL3DF:;_LL3E0: goto
_LL3DC;_LL3DC:;}goto _LL3C4;_LL3C3: {struct Cyc_Absyn_IntType_struct*_tmp743=(
struct Cyc_Absyn_IntType_struct*)_tmp731;if(_tmp743->tag != 6)goto _LL3C5;}_LL3C4:
goto _LL3C6;_LL3C5: {struct Cyc_Absyn_FloatType_struct*_tmp744=(struct Cyc_Absyn_FloatType_struct*)
_tmp731;if(_tmp744->tag != 7)goto _LL3C7;}_LL3C6: goto _LL3C8;_LL3C7: {struct Cyc_Absyn_DoubleType_struct*
_tmp745=(struct Cyc_Absyn_DoubleType_struct*)_tmp731;if(_tmp745->tag != 8)goto
_LL3C9;}_LL3C8: return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;
_LL3C9: {struct Cyc_Absyn_RgnHandleType_struct*_tmp746=(struct Cyc_Absyn_RgnHandleType_struct*)
_tmp731;if(_tmp746->tag != 16)goto _LL3CB;else{_tmp747=(void*)_tmp746->f1;}}_LL3CA:{
void*_tmp769=t2;void*_tmp76B;_LL3E2: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp76A=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp769;if(_tmp76A->tag != 16)goto
_LL3E4;else{_tmp76B=(void*)_tmp76A->f1;}}_LL3E3: if(Cyc_Tcenv_region_outlives(te,
_tmp747,_tmp76B))return Cyc_Absyn_No_coercion;goto _LL3E1;_LL3E4:;_LL3E5: goto
_LL3E1;_LL3E1:;}return Cyc_Absyn_Unknown_coercion;_LL3CB:;_LL3CC: return Cyc_Absyn_Unknown_coercion;
_LL3BC:;};}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,void*t,enum Cyc_Absyn_Coercion c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){if(!Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*
_tmp76C=Cyc_Absyn_copy_exp(e);{struct Cyc_Absyn_Cast_e_struct _tmp109E;struct Cyc_Absyn_Cast_e_struct*
_tmp109D;e->r=(void*)((_tmp109D=_cycalloc(sizeof(*_tmp109D)),((_tmp109D[0]=((
_tmp109E.tag=15,((_tmp109E.f1=(void*)t,((_tmp109E.f2=_tmp76C,((_tmp109E.f3=0,((
_tmp109E.f4=c,_tmp109E)))))))))),_tmp109D))));}{struct Cyc_Core_Opt*_tmp109F;e->topt=((
_tmp109F=_cycalloc(sizeof(*_tmp109F)),((_tmp109F->v=t,_tmp109F))));};}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp770=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL3E7: {struct Cyc_Absyn_IntType_struct*_tmp771=(struct Cyc_Absyn_IntType_struct*)
_tmp770;if(_tmp771->tag != 6)goto _LL3E9;}_LL3E8: goto _LL3EA;_LL3E9: {struct Cyc_Absyn_EnumType_struct*
_tmp772=(struct Cyc_Absyn_EnumType_struct*)_tmp770;if(_tmp772->tag != 14)goto
_LL3EB;}_LL3EA: goto _LL3EC;_LL3EB: {struct Cyc_Absyn_AnonEnumType_struct*_tmp773=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp770;if(_tmp773->tag != 15)goto _LL3ED;}
_LL3EC: goto _LL3EE;_LL3ED: {struct Cyc_Absyn_TagType_struct*_tmp774=(struct Cyc_Absyn_TagType_struct*)
_tmp770;if(_tmp774->tag != 20)goto _LL3EF;}_LL3EE: return 1;_LL3EF: {struct Cyc_Absyn_Evar_struct*
_tmp775=(struct Cyc_Absyn_Evar_struct*)_tmp770;if(_tmp775->tag != 1)goto _LL3F1;}
_LL3F0: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_Absyn_sint_typ);_LL3F1:;_LL3F2: return 0;_LL3E6:;}int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e))return 1;{void*_tmp776=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL3F4: {struct Cyc_Absyn_FloatType_struct*_tmp777=(
struct Cyc_Absyn_FloatType_struct*)_tmp776;if(_tmp777->tag != 7)goto _LL3F6;}_LL3F5:
goto _LL3F7;_LL3F6: {struct Cyc_Absyn_DoubleType_struct*_tmp778=(struct Cyc_Absyn_DoubleType_struct*)
_tmp776;if(_tmp778->tag != 8)goto _LL3F8;}_LL3F7: return 1;_LL3F8:;_LL3F9: return 0;
_LL3F3:;};}int Cyc_Tcutil_is_function_type(void*t);int Cyc_Tcutil_is_function_type(
void*t){void*_tmp779=Cyc_Tcutil_compress(t);_LL3FB: {struct Cyc_Absyn_FnType_struct*
_tmp77A=(struct Cyc_Absyn_FnType_struct*)_tmp779;if(_tmp77A->tag != 10)goto _LL3FD;}
_LL3FC: return 1;_LL3FD:;_LL3FE: return 0;_LL3FA:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmp10A0;struct _tuple0 _tmp77C=(_tmp10A0.f1=t1,((_tmp10A0.f2=t2,_tmp10A0)));
void*_tmp77D;int _tmp77F;void*_tmp780;int _tmp782;void*_tmp783;void*_tmp785;void*
_tmp787;void*_tmp789;void*_tmp78B;enum Cyc_Absyn_Sign _tmp78D;enum Cyc_Absyn_Size_of
_tmp78E;void*_tmp78F;enum Cyc_Absyn_Sign _tmp791;enum Cyc_Absyn_Size_of _tmp792;
void*_tmp793;enum Cyc_Absyn_Size_of _tmp795;void*_tmp796;enum Cyc_Absyn_Size_of
_tmp798;void*_tmp799;enum Cyc_Absyn_Sign _tmp79B;enum Cyc_Absyn_Size_of _tmp79C;
void*_tmp79D;enum Cyc_Absyn_Sign _tmp79F;enum Cyc_Absyn_Size_of _tmp7A0;void*
_tmp7A1;enum Cyc_Absyn_Sign _tmp7A3;enum Cyc_Absyn_Size_of _tmp7A4;void*_tmp7A5;
enum Cyc_Absyn_Sign _tmp7A7;enum Cyc_Absyn_Size_of _tmp7A8;void*_tmp7A9;enum Cyc_Absyn_Size_of
_tmp7AB;void*_tmp7AC;enum Cyc_Absyn_Size_of _tmp7AE;_LL400: _tmp77D=_tmp77C.f1;{
struct Cyc_Absyn_DoubleType_struct*_tmp77E=(struct Cyc_Absyn_DoubleType_struct*)
_tmp77D;if(_tmp77E->tag != 8)goto _LL402;else{_tmp77F=_tmp77E->f1;}};_tmp780=
_tmp77C.f2;{struct Cyc_Absyn_DoubleType_struct*_tmp781=(struct Cyc_Absyn_DoubleType_struct*)
_tmp780;if(_tmp781->tag != 8)goto _LL402;else{_tmp782=_tmp781->f1;}};_LL401: if(
_tmp77F)return t1;else{return t2;}_LL402: _tmp783=_tmp77C.f1;{struct Cyc_Absyn_DoubleType_struct*
_tmp784=(struct Cyc_Absyn_DoubleType_struct*)_tmp783;if(_tmp784->tag != 8)goto
_LL404;};_LL403: return t1;_LL404: _tmp785=_tmp77C.f2;{struct Cyc_Absyn_DoubleType_struct*
_tmp786=(struct Cyc_Absyn_DoubleType_struct*)_tmp785;if(_tmp786->tag != 8)goto
_LL406;};_LL405: return t2;_LL406: _tmp787=_tmp77C.f1;{struct Cyc_Absyn_FloatType_struct*
_tmp788=(struct Cyc_Absyn_FloatType_struct*)_tmp787;if(_tmp788->tag != 7)goto
_LL408;};_LL407: goto _LL409;_LL408: _tmp789=_tmp77C.f2;{struct Cyc_Absyn_FloatType_struct*
_tmp78A=(struct Cyc_Absyn_FloatType_struct*)_tmp789;if(_tmp78A->tag != 7)goto
_LL40A;};_LL409: return(void*)& Cyc_Absyn_FloatType_val;_LL40A: _tmp78B=_tmp77C.f1;{
struct Cyc_Absyn_IntType_struct*_tmp78C=(struct Cyc_Absyn_IntType_struct*)_tmp78B;
if(_tmp78C->tag != 6)goto _LL40C;else{_tmp78D=_tmp78C->f1;if(_tmp78D != Cyc_Absyn_Unsigned)
goto _LL40C;_tmp78E=_tmp78C->f2;if(_tmp78E != Cyc_Absyn_LongLong_sz)goto _LL40C;}};
_LL40B: goto _LL40D;_LL40C: _tmp78F=_tmp77C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp790=(struct Cyc_Absyn_IntType_struct*)_tmp78F;if(_tmp790->tag != 6)goto _LL40E;
else{_tmp791=_tmp790->f1;if(_tmp791 != Cyc_Absyn_Unsigned)goto _LL40E;_tmp792=
_tmp790->f2;if(_tmp792 != Cyc_Absyn_LongLong_sz)goto _LL40E;}};_LL40D: return Cyc_Absyn_ulonglong_typ;
_LL40E: _tmp793=_tmp77C.f1;{struct Cyc_Absyn_IntType_struct*_tmp794=(struct Cyc_Absyn_IntType_struct*)
_tmp793;if(_tmp794->tag != 6)goto _LL410;else{_tmp795=_tmp794->f2;if(_tmp795 != Cyc_Absyn_LongLong_sz)
goto _LL410;}};_LL40F: goto _LL411;_LL410: _tmp796=_tmp77C.f2;{struct Cyc_Absyn_IntType_struct*
_tmp797=(struct Cyc_Absyn_IntType_struct*)_tmp796;if(_tmp797->tag != 6)goto _LL412;
else{_tmp798=_tmp797->f2;if(_tmp798 != Cyc_Absyn_LongLong_sz)goto _LL412;}};_LL411:
return Cyc_Absyn_slonglong_typ;_LL412: _tmp799=_tmp77C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp79A=(struct Cyc_Absyn_IntType_struct*)_tmp799;if(_tmp79A->tag != 6)goto _LL414;
else{_tmp79B=_tmp79A->f1;if(_tmp79B != Cyc_Absyn_Unsigned)goto _LL414;_tmp79C=
_tmp79A->f2;if(_tmp79C != Cyc_Absyn_Long_sz)goto _LL414;}};_LL413: goto _LL415;
_LL414: _tmp79D=_tmp77C.f2;{struct Cyc_Absyn_IntType_struct*_tmp79E=(struct Cyc_Absyn_IntType_struct*)
_tmp79D;if(_tmp79E->tag != 6)goto _LL416;else{_tmp79F=_tmp79E->f1;if(_tmp79F != Cyc_Absyn_Unsigned)
goto _LL416;_tmp7A0=_tmp79E->f2;if(_tmp7A0 != Cyc_Absyn_Long_sz)goto _LL416;}};
_LL415: return Cyc_Absyn_ulong_typ;_LL416: _tmp7A1=_tmp77C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7A2=(struct Cyc_Absyn_IntType_struct*)_tmp7A1;if(_tmp7A2->tag != 6)goto _LL418;
else{_tmp7A3=_tmp7A2->f1;if(_tmp7A3 != Cyc_Absyn_Unsigned)goto _LL418;_tmp7A4=
_tmp7A2->f2;if(_tmp7A4 != Cyc_Absyn_Int_sz)goto _LL418;}};_LL417: goto _LL419;_LL418:
_tmp7A5=_tmp77C.f2;{struct Cyc_Absyn_IntType_struct*_tmp7A6=(struct Cyc_Absyn_IntType_struct*)
_tmp7A5;if(_tmp7A6->tag != 6)goto _LL41A;else{_tmp7A7=_tmp7A6->f1;if(_tmp7A7 != Cyc_Absyn_Unsigned)
goto _LL41A;_tmp7A8=_tmp7A6->f2;if(_tmp7A8 != Cyc_Absyn_Int_sz)goto _LL41A;}};
_LL419: return Cyc_Absyn_uint_typ;_LL41A: _tmp7A9=_tmp77C.f1;{struct Cyc_Absyn_IntType_struct*
_tmp7AA=(struct Cyc_Absyn_IntType_struct*)_tmp7A9;if(_tmp7AA->tag != 6)goto _LL41C;
else{_tmp7AB=_tmp7AA->f2;if(_tmp7AB != Cyc_Absyn_Long_sz)goto _LL41C;}};_LL41B:
goto _LL41D;_LL41C: _tmp7AC=_tmp77C.f2;{struct Cyc_Absyn_IntType_struct*_tmp7AD=(
struct Cyc_Absyn_IntType_struct*)_tmp7AC;if(_tmp7AD->tag != 6)goto _LL41E;else{
_tmp7AE=_tmp7AD->f2;if(_tmp7AE != Cyc_Absyn_Long_sz)goto _LL41E;}};_LL41D: return
Cyc_Absyn_slong_typ;_LL41E:;_LL41F: return Cyc_Absyn_sint_typ;_LL3FF:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp7AF=e->r;struct Cyc_Core_Opt*_tmp7B1;_LL421: {struct Cyc_Absyn_AssignOp_e_struct*
_tmp7B0=(struct Cyc_Absyn_AssignOp_e_struct*)_tmp7AF;if(_tmp7B0->tag != 4)goto
_LL423;else{_tmp7B1=_tmp7B0->f2;if(_tmp7B1 != 0)goto _LL423;}}_LL422:{const char*
_tmp10A3;void*_tmp10A2;(_tmp10A2=0,Cyc_Tcutil_warn(e->loc,((_tmp10A3="assignment in test",
_tag_dyneither(_tmp10A3,sizeof(char),19))),_tag_dyneither(_tmp10A2,sizeof(void*),
0)));}goto _LL420;_LL423:;_LL424: goto _LL420;_LL420:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmp10A4;struct _tuple0 _tmp7B5=(
_tmp10A4.f1=c1,((_tmp10A4.f2=c2,_tmp10A4)));void*_tmp7B6;enum Cyc_Absyn_Kind
_tmp7B8;void*_tmp7B9;enum Cyc_Absyn_Kind _tmp7BB;void*_tmp7BC;struct Cyc_Core_Opt*
_tmp7BE;struct Cyc_Core_Opt**_tmp7BF;void*_tmp7C0;struct Cyc_Core_Opt*_tmp7C2;
struct Cyc_Core_Opt**_tmp7C3;void*_tmp7C4;struct Cyc_Core_Opt*_tmp7C6;struct Cyc_Core_Opt**
_tmp7C7;enum Cyc_Absyn_Kind _tmp7C8;void*_tmp7C9;enum Cyc_Absyn_Kind _tmp7CB;void*
_tmp7CC;enum Cyc_Absyn_Kind _tmp7CE;void*_tmp7CF;struct Cyc_Core_Opt*_tmp7D1;
struct Cyc_Core_Opt**_tmp7D2;enum Cyc_Absyn_Kind _tmp7D3;void*_tmp7D4;struct Cyc_Core_Opt*
_tmp7D6;struct Cyc_Core_Opt**_tmp7D7;enum Cyc_Absyn_Kind _tmp7D8;void*_tmp7D9;
struct Cyc_Core_Opt*_tmp7DB;struct Cyc_Core_Opt**_tmp7DC;enum Cyc_Absyn_Kind
_tmp7DD;_LL426: _tmp7B6=_tmp7B5.f1;{struct Cyc_Absyn_Eq_kb_struct*_tmp7B7=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp7B6;if(_tmp7B7->tag != 0)goto _LL428;else{_tmp7B8=
_tmp7B7->f1;}};_tmp7B9=_tmp7B5.f2;{struct Cyc_Absyn_Eq_kb_struct*_tmp7BA=(struct
Cyc_Absyn_Eq_kb_struct*)_tmp7B9;if(_tmp7BA->tag != 0)goto _LL428;else{_tmp7BB=
_tmp7BA->f1;}};_LL427: return _tmp7B8 == _tmp7BB;_LL428: _tmp7BC=_tmp7B5.f2;{struct
Cyc_Absyn_Unknown_kb_struct*_tmp7BD=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp7BC;
if(_tmp7BD->tag != 1)goto _LL42A;else{_tmp7BE=_tmp7BD->f1;_tmp7BF=(struct Cyc_Core_Opt**)&
_tmp7BD->f1;}};_LL429:{struct Cyc_Core_Opt*_tmp10A5;*_tmp7BF=((_tmp10A5=_cycalloc(
sizeof(*_tmp10A5)),((_tmp10A5->v=c1,_tmp10A5))));}return 1;_LL42A: _tmp7C0=_tmp7B5.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmp7C1=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmp7C0;if(_tmp7C1->tag != 1)goto _LL42C;else{_tmp7C2=_tmp7C1->f1;_tmp7C3=(struct
Cyc_Core_Opt**)& _tmp7C1->f1;}};_LL42B:{struct Cyc_Core_Opt*_tmp10A6;*_tmp7C3=((
_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((_tmp10A6->v=c2,_tmp10A6))));}return 1;
_LL42C: _tmp7C4=_tmp7B5.f1;{struct Cyc_Absyn_Less_kb_struct*_tmp7C5=(struct Cyc_Absyn_Less_kb_struct*)
_tmp7C4;if(_tmp7C5->tag != 2)goto _LL42E;else{_tmp7C6=_tmp7C5->f1;_tmp7C7=(struct
Cyc_Core_Opt**)& _tmp7C5->f1;_tmp7C8=_tmp7C5->f2;}};_tmp7C9=_tmp7B5.f2;{struct Cyc_Absyn_Eq_kb_struct*
_tmp7CA=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7C9;if(_tmp7CA->tag != 0)goto _LL42E;
else{_tmp7CB=_tmp7CA->f1;}};_LL42D: if(Cyc_Tcutil_kind_leq(_tmp7CB,_tmp7C8)){{
struct Cyc_Core_Opt*_tmp10A7;*_tmp7C7=((_tmp10A7=_cycalloc(sizeof(*_tmp10A7)),((
_tmp10A7->v=c2,_tmp10A7))));}return 1;}else{return 0;}_LL42E: _tmp7CC=_tmp7B5.f1;{
struct Cyc_Absyn_Eq_kb_struct*_tmp7CD=(struct Cyc_Absyn_Eq_kb_struct*)_tmp7CC;if(
_tmp7CD->tag != 0)goto _LL430;else{_tmp7CE=_tmp7CD->f1;}};_tmp7CF=_tmp7B5.f2;{
struct Cyc_Absyn_Less_kb_struct*_tmp7D0=(struct Cyc_Absyn_Less_kb_struct*)_tmp7CF;
if(_tmp7D0->tag != 2)goto _LL430;else{_tmp7D1=_tmp7D0->f1;_tmp7D2=(struct Cyc_Core_Opt**)&
_tmp7D0->f1;_tmp7D3=_tmp7D0->f2;}};_LL42F: if(Cyc_Tcutil_kind_leq(_tmp7CE,_tmp7D3)){{
struct Cyc_Core_Opt*_tmp10A8;*_tmp7D2=((_tmp10A8=_cycalloc(sizeof(*_tmp10A8)),((
_tmp10A8->v=c1,_tmp10A8))));}return 1;}else{return 0;}_LL430: _tmp7D4=_tmp7B5.f1;{
struct Cyc_Absyn_Less_kb_struct*_tmp7D5=(struct Cyc_Absyn_Less_kb_struct*)_tmp7D4;
if(_tmp7D5->tag != 2)goto _LL425;else{_tmp7D6=_tmp7D5->f1;_tmp7D7=(struct Cyc_Core_Opt**)&
_tmp7D5->f1;_tmp7D8=_tmp7D5->f2;}};_tmp7D9=_tmp7B5.f2;{struct Cyc_Absyn_Less_kb_struct*
_tmp7DA=(struct Cyc_Absyn_Less_kb_struct*)_tmp7D9;if(_tmp7DA->tag != 2)goto _LL425;
else{_tmp7DB=_tmp7DA->f1;_tmp7DC=(struct Cyc_Core_Opt**)& _tmp7DA->f1;_tmp7DD=
_tmp7DA->f2;}};_LL431: if(Cyc_Tcutil_kind_leq(_tmp7D8,_tmp7DD)){{struct Cyc_Core_Opt*
_tmp10A9;*_tmp7DC=((_tmp10A9=_cycalloc(sizeof(*_tmp10A9)),((_tmp10A9->v=c1,
_tmp10A9))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp7DD,_tmp7D8)){{struct Cyc_Core_Opt*
_tmp10AA;*_tmp7D7=((_tmp10AA=_cycalloc(sizeof(*_tmp10AA)),((_tmp10AA->v=c2,
_tmp10AA))));}return 1;}else{return 0;}}_LL425:;};}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmp10AE;void*_tmp10AD[1];struct Cyc_Int_pa_struct _tmp10AC;struct
_dyneither_ptr s=(struct _dyneither_ptr)((_tmp10AC.tag=1,((_tmp10AC.f1=(
unsigned long)i,((_tmp10AD[0]=& _tmp10AC,Cyc_aprintf(((_tmp10AE="#%d",
_tag_dyneither(_tmp10AE,sizeof(char),4))),_tag_dyneither(_tmp10AD,sizeof(void*),
1))))))));struct _dyneither_ptr*_tmp10B1;struct Cyc_Absyn_Tvar*_tmp10B0;return(
_tmp10B0=_cycalloc(sizeof(*_tmp10B0)),((_tmp10B0->name=((_tmp10B1=_cycalloc(
sizeof(struct _dyneither_ptr)* 1),((_tmp10B1[0]=s,_tmp10B1)))),((_tmp10B0->identity=
- 1,((_tmp10B0->kind=k,_tmp10B0)))))));}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){struct _dyneither_ptr
_tmp7E9=*t->name;return*((const char*)_check_dyneither_subscript(_tmp7E9,sizeof(
char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t);void
Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){{const char*_tmp10B5;void*
_tmp10B4[1];struct Cyc_String_pa_struct _tmp10B3;(_tmp10B3.tag=0,((_tmp10B3.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp10B4[0]=& _tmp10B3,
Cyc_printf(((_tmp10B5="%s",_tag_dyneither(_tmp10B5,sizeof(char),3))),
_tag_dyneither(_tmp10B4,sizeof(void*),1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))
return;{const char*_tmp10B6;struct _dyneither_ptr _tmp7ED=Cyc_strconcat(((_tmp10B6="`",
_tag_dyneither(_tmp10B6,sizeof(char),2))),(struct _dyneither_ptr)*t->name);{char
_tmp10B9;char _tmp10B8;struct _dyneither_ptr _tmp10B7;(_tmp10B7=_dyneither_ptr_plus(
_tmp7ED,sizeof(char),1),((_tmp10B8=*((char*)_check_dyneither_subscript(_tmp10B7,
sizeof(char),0)),((_tmp10B9='t',((_get_dyneither_size(_tmp10B7,sizeof(char))== 1
 && (_tmp10B8 == '\000'  && _tmp10B9 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp10B7.curr)=_tmp10B9)))))));}{struct _dyneither_ptr*_tmp10BA;t->name=((
_tmp10BA=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp10BA[0]=(struct
_dyneither_ptr)_tmp7ED,_tmp10BA))));};};}struct _tuple20{struct _dyneither_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(
struct _tuple20*x);static struct _tuple9*Cyc_Tcutil_fndecl2typ_f(struct _tuple20*x){
struct Cyc_Core_Opt*_tmp10BD;struct _tuple9*_tmp10BC;return(_tmp10BC=_cycalloc(
sizeof(*_tmp10BC)),((_tmp10BC->f1=(struct Cyc_Core_Opt*)((_tmp10BD=_cycalloc(
sizeof(*_tmp10BD)),((_tmp10BD->v=(*x).f1,_tmp10BD)))),((_tmp10BC->f2=(*x).f2,((
_tmp10BC->f3=(*x).f3,_tmp10BC)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){
struct Cyc_List_List*_tmp7F5=0;{struct Cyc_List_List*atts=fd->attributes;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct Cyc_List_List*
_tmp10BE;_tmp7F5=((_tmp10BE=_cycalloc(sizeof(*_tmp10BE)),((_tmp10BE->hd=(void*)
atts->hd,((_tmp10BE->tl=_tmp7F5,_tmp10BE))))));}}}{struct Cyc_Absyn_FnType_struct
_tmp10C4;struct Cyc_Absyn_FnInfo _tmp10C3;struct Cyc_Absyn_FnType_struct*_tmp10C2;
return(void*)((_tmp10C2=_cycalloc(sizeof(*_tmp10C2)),((_tmp10C2[0]=((_tmp10C4.tag=
10,((_tmp10C4.f1=((_tmp10C3.tvars=fd->tvs,((_tmp10C3.effect=fd->effect,((
_tmp10C3.ret_typ=fd->ret_type,((_tmp10C3.args=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple20*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmp10C3.c_varargs=fd->c_varargs,((_tmp10C3.cyc_varargs=fd->cyc_varargs,((
_tmp10C3.rgn_po=fd->rgn_po,((_tmp10C3.attributes=_tmp7F5,_tmp10C3)))))))))))))))),
_tmp10C4)))),_tmp10C2))));};}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple21{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple21*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple21*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t);void*Cyc_Tcutil_snd_tqt(struct _tuple11*
t){return(*t).f2;}static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*
t);static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){struct
_tuple11*_tmp10C5;return(_tmp10C5=_cycalloc(sizeof(*_tmp10C5)),((_tmp10C5->f1=(*
pr).f1,((_tmp10C5->f2=t,_tmp10C5)))));}struct _tuple22{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Tqual f2;};struct _tuple23{struct _tuple22*f1;void*f2;};static
struct _tuple23*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y);
static struct _tuple23*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct
_tuple9*y){struct _tuple22*_tmp10C8;struct _tuple23*_tmp10C7;return(_tmp10C7=
_region_malloc(rgn,sizeof(*_tmp10C7)),((_tmp10C7->f1=((_tmp10C8=_region_malloc(
rgn,sizeof(*_tmp10C8)),((_tmp10C8->f1=(*y).f1,((_tmp10C8->f2=(*y).f2,_tmp10C8)))))),((
_tmp10C7->f2=(*y).f3,_tmp10C7)))));}static struct _tuple9*Cyc_Tcutil_substitute_f2(
struct _tuple23*w);static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple23*w){
struct _tuple22*_tmp7FE;void*_tmp7FF;struct _tuple23 _tmp7FD=*w;_tmp7FE=_tmp7FD.f1;
_tmp7FF=_tmp7FD.f2;{struct Cyc_Core_Opt*_tmp801;struct Cyc_Absyn_Tqual _tmp802;
struct _tuple22 _tmp800=*_tmp7FE;_tmp801=_tmp800.f1;_tmp802=_tmp800.f2;{struct
_tuple9*_tmp10C9;return(_tmp10C9=_cycalloc(sizeof(*_tmp10C9)),((_tmp10C9->f1=
_tmp801,((_tmp10C9->f2=_tmp802,((_tmp10C9->f3=_tmp7FF,_tmp10C9)))))));};};}
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmp10CA;return(
_tmp10CA=_cycalloc(sizeof(*_tmp10CA)),((_tmp10CA->name=f->name,((_tmp10CA->tq=f->tq,((
_tmp10CA->type=t,((_tmp10CA->width=f->width,((_tmp10CA->attributes=f->attributes,
_tmp10CA)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct
Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r);static struct Cyc_Absyn_Exp*
Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){struct Cyc_Absyn_Exp*_tmp10CB;
return(_tmp10CB=_cycalloc(sizeof(*_tmp10CB)),((_tmp10CB->topt=old->topt,((
_tmp10CB->r=r,((_tmp10CB->loc=old->loc,((_tmp10CB->annot=old->annot,_tmp10CB)))))))));}
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*
inst,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct
_RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){void*_tmp806=e->r;
enum Cyc_Absyn_Primop _tmp80C;struct Cyc_List_List*_tmp80D;struct Cyc_Absyn_Exp*
_tmp80F;struct Cyc_Absyn_Exp*_tmp810;struct Cyc_Absyn_Exp*_tmp811;struct Cyc_Absyn_Exp*
_tmp813;struct Cyc_Absyn_Exp*_tmp814;struct Cyc_Absyn_Exp*_tmp816;struct Cyc_Absyn_Exp*
_tmp817;struct Cyc_Absyn_Exp*_tmp819;struct Cyc_Absyn_Exp*_tmp81A;void*_tmp81C;
struct Cyc_Absyn_Exp*_tmp81D;int _tmp81E;enum Cyc_Absyn_Coercion _tmp81F;void*
_tmp821;struct Cyc_Absyn_Exp*_tmp823;void*_tmp825;void*_tmp826;void*_tmp828;
_LL433: {struct Cyc_Absyn_Const_e_struct*_tmp807=(struct Cyc_Absyn_Const_e_struct*)
_tmp806;if(_tmp807->tag != 0)goto _LL435;}_LL434: goto _LL436;_LL435: {struct Cyc_Absyn_Enum_e_struct*
_tmp808=(struct Cyc_Absyn_Enum_e_struct*)_tmp806;if(_tmp808->tag != 33)goto _LL437;}
_LL436: goto _LL438;_LL437: {struct Cyc_Absyn_AnonEnum_e_struct*_tmp809=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp806;if(_tmp809->tag != 34)goto _LL439;}_LL438: goto _LL43A;_LL439: {struct Cyc_Absyn_Var_e_struct*
_tmp80A=(struct Cyc_Absyn_Var_e_struct*)_tmp806;if(_tmp80A->tag != 1)goto _LL43B;}
_LL43A: return e;_LL43B: {struct Cyc_Absyn_Primop_e_struct*_tmp80B=(struct Cyc_Absyn_Primop_e_struct*)
_tmp806;if(_tmp80B->tag != 3)goto _LL43D;else{_tmp80C=_tmp80B->f1;_tmp80D=_tmp80B->f2;}}
_LL43C: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp80D)== 1){struct
Cyc_Absyn_Exp*_tmp829=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp80D))->hd;struct Cyc_Absyn_Exp*_tmp82A=Cyc_Tcutil_rsubsexp(r,inst,_tmp829);
if(_tmp82A == _tmp829)return e;{struct Cyc_Absyn_Primop_e_struct _tmp10D1;struct Cyc_Absyn_Exp*
_tmp10D0[1];struct Cyc_Absyn_Primop_e_struct*_tmp10CF;return Cyc_Tcutil_copye(e,(
void*)((_tmp10CF=_cycalloc(sizeof(*_tmp10CF)),((_tmp10CF[0]=((_tmp10D1.tag=3,((
_tmp10D1.f1=_tmp80C,((_tmp10D1.f2=((_tmp10D0[0]=_tmp82A,((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp10D0,sizeof(struct Cyc_Absyn_Exp*),
1)))),_tmp10D1)))))),_tmp10CF)))));};}else{if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp80D)== 2){struct Cyc_Absyn_Exp*_tmp82E=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp80D))->hd;struct Cyc_Absyn_Exp*_tmp82F=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp80D->tl))->hd;struct
Cyc_Absyn_Exp*_tmp830=Cyc_Tcutil_rsubsexp(r,inst,_tmp82E);struct Cyc_Absyn_Exp*
_tmp831=Cyc_Tcutil_rsubsexp(r,inst,_tmp82F);if(_tmp830 == _tmp82E  && _tmp831 == 
_tmp82F)return e;{struct Cyc_Absyn_Primop_e_struct _tmp10D7;struct Cyc_Absyn_Exp*
_tmp10D6[2];struct Cyc_Absyn_Primop_e_struct*_tmp10D5;return Cyc_Tcutil_copye(e,(
void*)((_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5[0]=((_tmp10D7.tag=3,((
_tmp10D7.f1=_tmp80C,((_tmp10D7.f2=((_tmp10D6[1]=_tmp831,((_tmp10D6[0]=_tmp830,((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp10D6,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp10D7)))))),_tmp10D5)))));};}
else{const char*_tmp10DA;void*_tmp10D9;return(_tmp10D9=0,((struct Cyc_Absyn_Exp*(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10DA="primop does not have 1 or 2 args!",
_tag_dyneither(_tmp10DA,sizeof(char),34))),_tag_dyneither(_tmp10D9,sizeof(void*),
0)));}}_LL43D: {struct Cyc_Absyn_Conditional_e_struct*_tmp80E=(struct Cyc_Absyn_Conditional_e_struct*)
_tmp806;if(_tmp80E->tag != 6)goto _LL43F;else{_tmp80F=_tmp80E->f1;_tmp810=_tmp80E->f2;
_tmp811=_tmp80E->f3;}}_LL43E: {struct Cyc_Absyn_Exp*_tmp837=Cyc_Tcutil_rsubsexp(r,
inst,_tmp80F);struct Cyc_Absyn_Exp*_tmp838=Cyc_Tcutil_rsubsexp(r,inst,_tmp810);
struct Cyc_Absyn_Exp*_tmp839=Cyc_Tcutil_rsubsexp(r,inst,_tmp811);if((_tmp837 == 
_tmp80F  && _tmp838 == _tmp810) && _tmp839 == _tmp811)return e;{struct Cyc_Absyn_Conditional_e_struct
_tmp10DD;struct Cyc_Absyn_Conditional_e_struct*_tmp10DC;return Cyc_Tcutil_copye(e,(
void*)((_tmp10DC=_cycalloc(sizeof(*_tmp10DC)),((_tmp10DC[0]=((_tmp10DD.tag=6,((
_tmp10DD.f1=_tmp837,((_tmp10DD.f2=_tmp838,((_tmp10DD.f3=_tmp839,_tmp10DD)))))))),
_tmp10DC)))));};}_LL43F: {struct Cyc_Absyn_And_e_struct*_tmp812=(struct Cyc_Absyn_And_e_struct*)
_tmp806;if(_tmp812->tag != 7)goto _LL441;else{_tmp813=_tmp812->f1;_tmp814=_tmp812->f2;}}
_LL440: {struct Cyc_Absyn_Exp*_tmp83C=Cyc_Tcutil_rsubsexp(r,inst,_tmp813);struct
Cyc_Absyn_Exp*_tmp83D=Cyc_Tcutil_rsubsexp(r,inst,_tmp814);if(_tmp83C == _tmp813
 && _tmp83D == _tmp814)return e;{struct Cyc_Absyn_And_e_struct _tmp10E0;struct Cyc_Absyn_And_e_struct*
_tmp10DF;return Cyc_Tcutil_copye(e,(void*)((_tmp10DF=_cycalloc(sizeof(*_tmp10DF)),((
_tmp10DF[0]=((_tmp10E0.tag=7,((_tmp10E0.f1=_tmp83C,((_tmp10E0.f2=_tmp83D,
_tmp10E0)))))),_tmp10DF)))));};}_LL441: {struct Cyc_Absyn_Or_e_struct*_tmp815=(
struct Cyc_Absyn_Or_e_struct*)_tmp806;if(_tmp815->tag != 8)goto _LL443;else{_tmp816=
_tmp815->f1;_tmp817=_tmp815->f2;}}_LL442: {struct Cyc_Absyn_Exp*_tmp840=Cyc_Tcutil_rsubsexp(
r,inst,_tmp816);struct Cyc_Absyn_Exp*_tmp841=Cyc_Tcutil_rsubsexp(r,inst,_tmp817);
if(_tmp840 == _tmp816  && _tmp841 == _tmp817)return e;{struct Cyc_Absyn_Or_e_struct
_tmp10E3;struct Cyc_Absyn_Or_e_struct*_tmp10E2;return Cyc_Tcutil_copye(e,(void*)((
_tmp10E2=_cycalloc(sizeof(*_tmp10E2)),((_tmp10E2[0]=((_tmp10E3.tag=8,((_tmp10E3.f1=
_tmp840,((_tmp10E3.f2=_tmp841,_tmp10E3)))))),_tmp10E2)))));};}_LL443: {struct Cyc_Absyn_SeqExp_e_struct*
_tmp818=(struct Cyc_Absyn_SeqExp_e_struct*)_tmp806;if(_tmp818->tag != 9)goto _LL445;
else{_tmp819=_tmp818->f1;_tmp81A=_tmp818->f2;}}_LL444: {struct Cyc_Absyn_Exp*
_tmp844=Cyc_Tcutil_rsubsexp(r,inst,_tmp819);struct Cyc_Absyn_Exp*_tmp845=Cyc_Tcutil_rsubsexp(
r,inst,_tmp81A);if(_tmp844 == _tmp819  && _tmp845 == _tmp81A)return e;{struct Cyc_Absyn_SeqExp_e_struct
_tmp10E6;struct Cyc_Absyn_SeqExp_e_struct*_tmp10E5;return Cyc_Tcutil_copye(e,(void*)((
_tmp10E5=_cycalloc(sizeof(*_tmp10E5)),((_tmp10E5[0]=((_tmp10E6.tag=9,((_tmp10E6.f1=
_tmp844,((_tmp10E6.f2=_tmp845,_tmp10E6)))))),_tmp10E5)))));};}_LL445: {struct Cyc_Absyn_Cast_e_struct*
_tmp81B=(struct Cyc_Absyn_Cast_e_struct*)_tmp806;if(_tmp81B->tag != 15)goto _LL447;
else{_tmp81C=(void*)_tmp81B->f1;_tmp81D=_tmp81B->f2;_tmp81E=_tmp81B->f3;_tmp81F=
_tmp81B->f4;}}_LL446: {struct Cyc_Absyn_Exp*_tmp848=Cyc_Tcutil_rsubsexp(r,inst,
_tmp81D);void*_tmp849=Cyc_Tcutil_rsubstitute(r,inst,_tmp81C);if(_tmp848 == 
_tmp81D  && _tmp849 == _tmp81C)return e;{struct Cyc_Absyn_Cast_e_struct _tmp10E9;
struct Cyc_Absyn_Cast_e_struct*_tmp10E8;return Cyc_Tcutil_copye(e,(void*)((
_tmp10E8=_cycalloc(sizeof(*_tmp10E8)),((_tmp10E8[0]=((_tmp10E9.tag=15,((_tmp10E9.f1=(
void*)_tmp849,((_tmp10E9.f2=_tmp848,((_tmp10E9.f3=_tmp81E,((_tmp10E9.f4=_tmp81F,
_tmp10E9)))))))))),_tmp10E8)))));};}_LL447: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp820=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp806;if(_tmp820->tag != 18)goto
_LL449;else{_tmp821=(void*)_tmp820->f1;}}_LL448: {void*_tmp84C=Cyc_Tcutil_rsubstitute(
r,inst,_tmp821);if(_tmp84C == _tmp821)return e;{struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp10EC;struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp10EB;return Cyc_Tcutil_copye(e,(
void*)((_tmp10EB=_cycalloc(sizeof(*_tmp10EB)),((_tmp10EB[0]=((_tmp10EC.tag=18,((
_tmp10EC.f1=(void*)_tmp84C,_tmp10EC)))),_tmp10EB)))));};}_LL449: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmp822=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp806;if(_tmp822->tag != 19)goto
_LL44B;else{_tmp823=_tmp822->f1;}}_LL44A: {struct Cyc_Absyn_Exp*_tmp84F=Cyc_Tcutil_rsubsexp(
r,inst,_tmp823);if(_tmp84F == _tmp823)return e;{struct Cyc_Absyn_Sizeofexp_e_struct
_tmp10EF;struct Cyc_Absyn_Sizeofexp_e_struct*_tmp10EE;return Cyc_Tcutil_copye(e,(
void*)((_tmp10EE=_cycalloc(sizeof(*_tmp10EE)),((_tmp10EE[0]=((_tmp10EF.tag=19,((
_tmp10EF.f1=_tmp84F,_tmp10EF)))),_tmp10EE)))));};}_LL44B: {struct Cyc_Absyn_Offsetof_e_struct*
_tmp824=(struct Cyc_Absyn_Offsetof_e_struct*)_tmp806;if(_tmp824->tag != 20)goto
_LL44D;else{_tmp825=(void*)_tmp824->f1;_tmp826=(void*)_tmp824->f2;}}_LL44C: {
void*_tmp852=Cyc_Tcutil_rsubstitute(r,inst,_tmp825);if(_tmp852 == _tmp825)return e;{
struct Cyc_Absyn_Offsetof_e_struct _tmp10F2;struct Cyc_Absyn_Offsetof_e_struct*
_tmp10F1;return Cyc_Tcutil_copye(e,(void*)((_tmp10F1=_cycalloc(sizeof(*_tmp10F1)),((
_tmp10F1[0]=((_tmp10F2.tag=20,((_tmp10F2.f1=(void*)_tmp852,((_tmp10F2.f2=(void*)
_tmp826,_tmp10F2)))))),_tmp10F1)))));};}_LL44D: {struct Cyc_Absyn_Valueof_e_struct*
_tmp827=(struct Cyc_Absyn_Valueof_e_struct*)_tmp806;if(_tmp827->tag != 40)goto
_LL44F;else{_tmp828=(void*)_tmp827->f1;}}_LL44E: {void*_tmp855=Cyc_Tcutil_rsubstitute(
r,inst,_tmp828);if(_tmp855 == _tmp828)return e;{struct Cyc_Absyn_Valueof_e_struct
_tmp10F5;struct Cyc_Absyn_Valueof_e_struct*_tmp10F4;return Cyc_Tcutil_copye(e,(
void*)((_tmp10F4=_cycalloc(sizeof(*_tmp10F4)),((_tmp10F4[0]=((_tmp10F5.tag=40,((
_tmp10F5.f1=(void*)_tmp855,_tmp10F5)))),_tmp10F4)))));};}_LL44F:;_LL450: {const
char*_tmp10F8;void*_tmp10F7;return(_tmp10F7=0,((struct Cyc_Absyn_Exp*(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F8="non-type-level-expression in Tcutil::rsubsexp",
_tag_dyneither(_tmp10F8,sizeof(char),46))),_tag_dyneither(_tmp10F7,sizeof(void*),
0)));}_LL432:;}void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,void*t){void*_tmp85A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp85C;
struct Cyc_Absyn_AggrInfo _tmp85E;union Cyc_Absyn_AggrInfoU _tmp85F;struct Cyc_List_List*
_tmp860;struct Cyc_Absyn_DatatypeInfo _tmp862;union Cyc_Absyn_DatatypeInfoU _tmp863;
struct Cyc_List_List*_tmp864;struct Cyc_Absyn_DatatypeFieldInfo _tmp866;union Cyc_Absyn_DatatypeFieldInfoU
_tmp867;struct Cyc_List_List*_tmp868;struct _tuple2*_tmp86A;struct Cyc_List_List*
_tmp86B;struct Cyc_Absyn_Typedefdecl*_tmp86C;void**_tmp86D;struct Cyc_Absyn_ArrayInfo
_tmp86F;void*_tmp870;struct Cyc_Absyn_Tqual _tmp871;struct Cyc_Absyn_Exp*_tmp872;
union Cyc_Absyn_Constraint*_tmp873;struct Cyc_Position_Segment*_tmp874;struct Cyc_Absyn_PtrInfo
_tmp876;void*_tmp877;struct Cyc_Absyn_Tqual _tmp878;struct Cyc_Absyn_PtrAtts _tmp879;
void*_tmp87A;union Cyc_Absyn_Constraint*_tmp87B;union Cyc_Absyn_Constraint*_tmp87C;
union Cyc_Absyn_Constraint*_tmp87D;struct Cyc_Absyn_FnInfo _tmp87F;struct Cyc_List_List*
_tmp880;struct Cyc_Core_Opt*_tmp881;void*_tmp882;struct Cyc_List_List*_tmp883;int
_tmp884;struct Cyc_Absyn_VarargInfo*_tmp885;struct Cyc_List_List*_tmp886;struct Cyc_List_List*
_tmp887;struct Cyc_List_List*_tmp889;enum Cyc_Absyn_AggrKind _tmp88B;struct Cyc_List_List*
_tmp88C;struct Cyc_Core_Opt*_tmp88E;void*_tmp890;void*_tmp892;void*_tmp893;void*
_tmp895;struct Cyc_Absyn_Exp*_tmp897;void*_tmp8A1;void*_tmp8A3;struct Cyc_List_List*
_tmp8A5;_LL452: {struct Cyc_Absyn_VarType_struct*_tmp85B=(struct Cyc_Absyn_VarType_struct*)
_tmp85A;if(_tmp85B->tag != 2)goto _LL454;else{_tmp85C=_tmp85B->f1;}}_LL453: {
struct _handler_cons _tmp8A6;_push_handler(& _tmp8A6);{int _tmp8A8=0;if(setjmp(
_tmp8A6.handler))_tmp8A8=1;if(!_tmp8A8){{void*_tmp8A9=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp85C);_npop_handler(0);return
_tmp8A9;};_pop_handler();}else{void*_tmp8A7=(void*)_exn_thrown;void*_tmp8AB=
_tmp8A7;_LL487: {struct Cyc_Core_Not_found_struct*_tmp8AC=(struct Cyc_Core_Not_found_struct*)
_tmp8AB;if(_tmp8AC->tag != Cyc_Core_Not_found)goto _LL489;}_LL488: return t;_LL489:;
_LL48A:(void)_throw(_tmp8AB);_LL486:;}};}_LL454: {struct Cyc_Absyn_AggrType_struct*
_tmp85D=(struct Cyc_Absyn_AggrType_struct*)_tmp85A;if(_tmp85D->tag != 12)goto
_LL456;else{_tmp85E=_tmp85D->f1;_tmp85F=_tmp85E.aggr_info;_tmp860=_tmp85E.targs;}}
_LL455: {struct Cyc_List_List*_tmp8AD=Cyc_Tcutil_substs(rgn,inst,_tmp860);struct
Cyc_Absyn_AggrType_struct _tmp10FE;struct Cyc_Absyn_AggrInfo _tmp10FD;struct Cyc_Absyn_AggrType_struct*
_tmp10FC;return _tmp8AD == _tmp860?t:(void*)((_tmp10FC=_cycalloc(sizeof(*_tmp10FC)),((
_tmp10FC[0]=((_tmp10FE.tag=12,((_tmp10FE.f1=((_tmp10FD.aggr_info=_tmp85F,((
_tmp10FD.targs=_tmp8AD,_tmp10FD)))),_tmp10FE)))),_tmp10FC))));}_LL456: {struct
Cyc_Absyn_DatatypeType_struct*_tmp861=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp85A;if(_tmp861->tag != 3)goto _LL458;else{_tmp862=_tmp861->f1;_tmp863=_tmp862.datatype_info;
_tmp864=_tmp862.targs;}}_LL457: {struct Cyc_List_List*_tmp8B1=Cyc_Tcutil_substs(
rgn,inst,_tmp864);struct Cyc_Absyn_DatatypeType_struct _tmp1104;struct Cyc_Absyn_DatatypeInfo
_tmp1103;struct Cyc_Absyn_DatatypeType_struct*_tmp1102;return _tmp8B1 == _tmp864?t:(
void*)((_tmp1102=_cycalloc(sizeof(*_tmp1102)),((_tmp1102[0]=((_tmp1104.tag=3,((
_tmp1104.f1=((_tmp1103.datatype_info=_tmp863,((_tmp1103.targs=_tmp8B1,_tmp1103)))),
_tmp1104)))),_tmp1102))));}_LL458: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmp865=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp85A;if(_tmp865->tag != 4)
goto _LL45A;else{_tmp866=_tmp865->f1;_tmp867=_tmp866.field_info;_tmp868=_tmp866.targs;}}
_LL459: {struct Cyc_List_List*_tmp8B5=Cyc_Tcutil_substs(rgn,inst,_tmp868);struct
Cyc_Absyn_DatatypeFieldType_struct _tmp110A;struct Cyc_Absyn_DatatypeFieldInfo
_tmp1109;struct Cyc_Absyn_DatatypeFieldType_struct*_tmp1108;return _tmp8B5 == 
_tmp868?t:(void*)((_tmp1108=_cycalloc(sizeof(*_tmp1108)),((_tmp1108[0]=((
_tmp110A.tag=4,((_tmp110A.f1=((_tmp1109.field_info=_tmp867,((_tmp1109.targs=
_tmp8B5,_tmp1109)))),_tmp110A)))),_tmp1108))));}_LL45A: {struct Cyc_Absyn_TypedefType_struct*
_tmp869=(struct Cyc_Absyn_TypedefType_struct*)_tmp85A;if(_tmp869->tag != 18)goto
_LL45C;else{_tmp86A=_tmp869->f1;_tmp86B=_tmp869->f2;_tmp86C=_tmp869->f3;_tmp86D=
_tmp869->f4;}}_LL45B: {struct Cyc_List_List*_tmp8B9=Cyc_Tcutil_substs(rgn,inst,
_tmp86B);struct Cyc_Absyn_TypedefType_struct _tmp110D;struct Cyc_Absyn_TypedefType_struct*
_tmp110C;return _tmp8B9 == _tmp86B?t:(void*)((_tmp110C=_cycalloc(sizeof(*_tmp110C)),((
_tmp110C[0]=((_tmp110D.tag=18,((_tmp110D.f1=_tmp86A,((_tmp110D.f2=_tmp8B9,((
_tmp110D.f3=_tmp86C,((_tmp110D.f4=_tmp86D,_tmp110D)))))))))),_tmp110C))));}
_LL45C: {struct Cyc_Absyn_ArrayType_struct*_tmp86E=(struct Cyc_Absyn_ArrayType_struct*)
_tmp85A;if(_tmp86E->tag != 9)goto _LL45E;else{_tmp86F=_tmp86E->f1;_tmp870=_tmp86F.elt_type;
_tmp871=_tmp86F.tq;_tmp872=_tmp86F.num_elts;_tmp873=_tmp86F.zero_term;_tmp874=
_tmp86F.zt_loc;}}_LL45D: {void*_tmp8BC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp870);
struct Cyc_Absyn_ArrayType_struct _tmp1113;struct Cyc_Absyn_ArrayInfo _tmp1112;
struct Cyc_Absyn_ArrayType_struct*_tmp1111;return _tmp8BC == _tmp870?t:(void*)((
_tmp1111=_cycalloc(sizeof(*_tmp1111)),((_tmp1111[0]=((_tmp1113.tag=9,((_tmp1113.f1=((
_tmp1112.elt_type=_tmp8BC,((_tmp1112.tq=_tmp871,((_tmp1112.num_elts=_tmp872,((
_tmp1112.zero_term=_tmp873,((_tmp1112.zt_loc=_tmp874,_tmp1112)))))))))),_tmp1113)))),
_tmp1111))));}_LL45E: {struct Cyc_Absyn_PointerType_struct*_tmp875=(struct Cyc_Absyn_PointerType_struct*)
_tmp85A;if(_tmp875->tag != 5)goto _LL460;else{_tmp876=_tmp875->f1;_tmp877=_tmp876.elt_typ;
_tmp878=_tmp876.elt_tq;_tmp879=_tmp876.ptr_atts;_tmp87A=_tmp879.rgn;_tmp87B=
_tmp879.nullable;_tmp87C=_tmp879.bounds;_tmp87D=_tmp879.zero_term;}}_LL45F: {
void*_tmp8C0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp877);void*_tmp8C1=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp87A);union Cyc_Absyn_Constraint*_tmp8C2=_tmp87C;{void*_tmp8C3=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,
_tmp87C);struct Cyc_Absyn_Exp*_tmp8C5;_LL48C: {struct Cyc_Absyn_Upper_b_struct*
_tmp8C4=(struct Cyc_Absyn_Upper_b_struct*)_tmp8C3;if(_tmp8C4->tag != 1)goto _LL48E;
else{_tmp8C5=_tmp8C4->f1;}}_LL48D: {struct Cyc_Absyn_Exp*_tmp8C6=Cyc_Tcutil_rsubsexp(
rgn,inst,_tmp8C5);if(_tmp8C6 != _tmp8C5){struct Cyc_Absyn_Upper_b_struct _tmp1116;
struct Cyc_Absyn_Upper_b_struct*_tmp1115;_tmp8C2=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1115=_cycalloc(sizeof(*_tmp1115)),((
_tmp1115[0]=((_tmp1116.tag=1,((_tmp1116.f1=_tmp8C6,_tmp1116)))),_tmp1115)))));}
goto _LL48B;}_LL48E:;_LL48F: goto _LL48B;_LL48B:;}if((_tmp8C0 == _tmp877  && _tmp8C1
== _tmp87A) && _tmp8C2 == _tmp87C)return t;{struct Cyc_Absyn_PointerType_struct
_tmp1120;struct Cyc_Absyn_PtrAtts _tmp111F;struct Cyc_Absyn_PtrInfo _tmp111E;struct
Cyc_Absyn_PointerType_struct*_tmp111D;return(void*)((_tmp111D=_cycalloc(sizeof(*
_tmp111D)),((_tmp111D[0]=((_tmp1120.tag=5,((_tmp1120.f1=((_tmp111E.elt_typ=
_tmp8C0,((_tmp111E.elt_tq=_tmp878,((_tmp111E.ptr_atts=((_tmp111F.rgn=_tmp8C1,((
_tmp111F.nullable=_tmp87B,((_tmp111F.bounds=_tmp8C2,((_tmp111F.zero_term=_tmp87D,((
_tmp111F.ptrloc=0,_tmp111F)))))))))),_tmp111E)))))),_tmp1120)))),_tmp111D))));};}
_LL460: {struct Cyc_Absyn_FnType_struct*_tmp87E=(struct Cyc_Absyn_FnType_struct*)
_tmp85A;if(_tmp87E->tag != 10)goto _LL462;else{_tmp87F=_tmp87E->f1;_tmp880=_tmp87F.tvars;
_tmp881=_tmp87F.effect;_tmp882=_tmp87F.ret_typ;_tmp883=_tmp87F.args;_tmp884=
_tmp87F.c_varargs;_tmp885=_tmp87F.cyc_varargs;_tmp886=_tmp87F.rgn_po;_tmp887=
_tmp87F.attributes;}}_LL461:{struct Cyc_List_List*_tmp8CD=_tmp880;for(0;_tmp8CD != 
0;_tmp8CD=_tmp8CD->tl){struct _tuple14*_tmp112A;struct Cyc_Absyn_VarType_struct
_tmp1129;struct Cyc_Absyn_VarType_struct*_tmp1128;struct Cyc_List_List*_tmp1127;
inst=((_tmp1127=_region_malloc(rgn,sizeof(*_tmp1127)),((_tmp1127->hd=((_tmp112A=
_region_malloc(rgn,sizeof(*_tmp112A)),((_tmp112A->f1=(struct Cyc_Absyn_Tvar*)
_tmp8CD->hd,((_tmp112A->f2=(void*)((_tmp1128=_cycalloc(sizeof(*_tmp1128)),((
_tmp1128[0]=((_tmp1129.tag=2,((_tmp1129.f1=(struct Cyc_Absyn_Tvar*)_tmp8CD->hd,
_tmp1129)))),_tmp1128)))),_tmp112A)))))),((_tmp1127->tl=inst,_tmp1127))))));}}{
struct Cyc_List_List*_tmp8D3;struct Cyc_List_List*_tmp8D4;struct _tuple1 _tmp8D2=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple23*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp883));
_tmp8D3=_tmp8D2.f1;_tmp8D4=_tmp8D2.f2;{struct Cyc_List_List*_tmp8D5=Cyc_Tcutil_substs(
rgn,inst,_tmp8D4);struct Cyc_List_List*_tmp8D6=((struct Cyc_List_List*(*)(struct
_tuple9*(*f)(struct _tuple23*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp8D3,_tmp8D5));struct Cyc_Core_Opt*
eff2;if(_tmp881 == 0)eff2=0;else{void*_tmp8D7=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp881->v);if(_tmp8D7 == (void*)_tmp881->v)eff2=_tmp881;else{struct Cyc_Core_Opt*
_tmp112B;eff2=((_tmp112B=_cycalloc(sizeof(*_tmp112B)),((_tmp112B->v=_tmp8D7,
_tmp112B))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp885 == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp8DA;struct Cyc_Absyn_Tqual _tmp8DB;void*
_tmp8DC;int _tmp8DD;struct Cyc_Absyn_VarargInfo _tmp8D9=*_tmp885;_tmp8DA=_tmp8D9.name;
_tmp8DB=_tmp8D9.tq;_tmp8DC=_tmp8D9.type;_tmp8DD=_tmp8D9.inject;{void*_tmp8DE=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8DC);if(_tmp8DE == _tmp8DC)cyc_varargs2=_tmp885;else{struct Cyc_Absyn_VarargInfo*
_tmp112C;cyc_varargs2=((_tmp112C=_cycalloc(sizeof(*_tmp112C)),((_tmp112C->name=
_tmp8DA,((_tmp112C->tq=_tmp8DB,((_tmp112C->type=_tmp8DE,((_tmp112C->inject=
_tmp8DD,_tmp112C))))))))));}};}{struct Cyc_List_List*rgn_po2;struct Cyc_List_List*
_tmp8E1;struct Cyc_List_List*_tmp8E2;struct _tuple1 _tmp8E0=((struct _tuple1(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,_tmp886);_tmp8E1=_tmp8E0.f1;_tmp8E2=_tmp8E0.f2;{struct Cyc_List_List*
_tmp8E3=Cyc_Tcutil_substs(rgn,inst,_tmp8E1);struct Cyc_List_List*_tmp8E4=Cyc_Tcutil_substs(
rgn,inst,_tmp8E2);if(_tmp8E3 == _tmp8E1  && _tmp8E4 == _tmp8E2)rgn_po2=_tmp886;
else{rgn_po2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_zip)(_tmp8E3,_tmp8E4);}{struct Cyc_Absyn_FnType_struct _tmp1132;struct
Cyc_Absyn_FnInfo _tmp1131;struct Cyc_Absyn_FnType_struct*_tmp1130;return(void*)((
_tmp1130=_cycalloc(sizeof(*_tmp1130)),((_tmp1130[0]=((_tmp1132.tag=10,((_tmp1132.f1=((
_tmp1131.tvars=_tmp880,((_tmp1131.effect=eff2,((_tmp1131.ret_typ=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp882),((_tmp1131.args=_tmp8D6,((_tmp1131.c_varargs=_tmp884,((
_tmp1131.cyc_varargs=cyc_varargs2,((_tmp1131.rgn_po=rgn_po2,((_tmp1131.attributes=
_tmp887,_tmp1131)))))))))))))))),_tmp1132)))),_tmp1130))));};};};};};};_LL462: {
struct Cyc_Absyn_TupleType_struct*_tmp888=(struct Cyc_Absyn_TupleType_struct*)
_tmp85A;if(_tmp888->tag != 11)goto _LL464;else{_tmp889=_tmp888->f1;}}_LL463: {
struct Cyc_List_List*_tmp8E8=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp889);struct Cyc_List_List*_tmp8E9=Cyc_Tcutil_substs(rgn,inst,_tmp8E8);if(
_tmp8E9 == _tmp8E8)return t;{struct Cyc_List_List*_tmp8EA=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp889,_tmp8E9);struct Cyc_Absyn_TupleType_struct
_tmp1135;struct Cyc_Absyn_TupleType_struct*_tmp1134;return(void*)((_tmp1134=
_cycalloc(sizeof(*_tmp1134)),((_tmp1134[0]=((_tmp1135.tag=11,((_tmp1135.f1=
_tmp8EA,_tmp1135)))),_tmp1134))));};}_LL464: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp88A=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp85A;if(_tmp88A->tag != 13)goto
_LL466;else{_tmp88B=_tmp88A->f1;_tmp88C=_tmp88A->f2;}}_LL465: {struct Cyc_List_List*
_tmp8ED=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp88C);struct
Cyc_List_List*_tmp8EE=Cyc_Tcutil_substs(rgn,inst,_tmp8ED);if(_tmp8EE == _tmp8ED)
return t;{struct Cyc_List_List*_tmp8EF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp88C,_tmp8EE);struct Cyc_Absyn_AnonAggrType_struct
_tmp1138;struct Cyc_Absyn_AnonAggrType_struct*_tmp1137;return(void*)((_tmp1137=
_cycalloc(sizeof(*_tmp1137)),((_tmp1137[0]=((_tmp1138.tag=13,((_tmp1138.f1=
_tmp88B,((_tmp1138.f2=_tmp8EF,_tmp1138)))))),_tmp1137))));};}_LL466: {struct Cyc_Absyn_Evar_struct*
_tmp88D=(struct Cyc_Absyn_Evar_struct*)_tmp85A;if(_tmp88D->tag != 1)goto _LL468;
else{_tmp88E=_tmp88D->f2;}}_LL467: if(_tmp88E != 0)return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp88E->v);else{return t;}_LL468: {struct Cyc_Absyn_RgnHandleType_struct*
_tmp88F=(struct Cyc_Absyn_RgnHandleType_struct*)_tmp85A;if(_tmp88F->tag != 16)goto
_LL46A;else{_tmp890=(void*)_tmp88F->f1;}}_LL469: {void*_tmp8F2=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp890);struct Cyc_Absyn_RgnHandleType_struct _tmp113B;struct Cyc_Absyn_RgnHandleType_struct*
_tmp113A;return _tmp8F2 == _tmp890?t:(void*)((_tmp113A=_cycalloc(sizeof(*_tmp113A)),((
_tmp113A[0]=((_tmp113B.tag=16,((_tmp113B.f1=(void*)_tmp8F2,_tmp113B)))),_tmp113A))));}
_LL46A: {struct Cyc_Absyn_DynRgnType_struct*_tmp891=(struct Cyc_Absyn_DynRgnType_struct*)
_tmp85A;if(_tmp891->tag != 17)goto _LL46C;else{_tmp892=(void*)_tmp891->f1;_tmp893=(
void*)_tmp891->f2;}}_LL46B: {void*_tmp8F5=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp892);void*_tmp8F6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp893);struct Cyc_Absyn_DynRgnType_struct
_tmp113E;struct Cyc_Absyn_DynRgnType_struct*_tmp113D;return _tmp8F5 == _tmp892  && 
_tmp8F6 == _tmp893?t:(void*)((_tmp113D=_cycalloc(sizeof(*_tmp113D)),((_tmp113D[0]=((
_tmp113E.tag=17,((_tmp113E.f1=(void*)_tmp8F5,((_tmp113E.f2=(void*)_tmp8F6,
_tmp113E)))))),_tmp113D))));}_LL46C: {struct Cyc_Absyn_TagType_struct*_tmp894=(
struct Cyc_Absyn_TagType_struct*)_tmp85A;if(_tmp894->tag != 20)goto _LL46E;else{
_tmp895=(void*)_tmp894->f1;}}_LL46D: {void*_tmp8F9=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp895);struct Cyc_Absyn_TagType_struct _tmp1141;struct Cyc_Absyn_TagType_struct*
_tmp1140;return _tmp8F9 == _tmp895?t:(void*)((_tmp1140=_cycalloc(sizeof(*_tmp1140)),((
_tmp1140[0]=((_tmp1141.tag=20,((_tmp1141.f1=(void*)_tmp8F9,_tmp1141)))),_tmp1140))));}
_LL46E: {struct Cyc_Absyn_ValueofType_struct*_tmp896=(struct Cyc_Absyn_ValueofType_struct*)
_tmp85A;if(_tmp896->tag != 19)goto _LL470;else{_tmp897=_tmp896->f1;}}_LL46F: {
struct Cyc_Absyn_Exp*_tmp8FC=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp897);struct Cyc_Absyn_ValueofType_struct
_tmp1144;struct Cyc_Absyn_ValueofType_struct*_tmp1143;return _tmp8FC == _tmp897?t:(
void*)((_tmp1143=_cycalloc(sizeof(*_tmp1143)),((_tmp1143[0]=((_tmp1144.tag=19,((
_tmp1144.f1=_tmp8FC,_tmp1144)))),_tmp1143))));}_LL470: {struct Cyc_Absyn_EnumType_struct*
_tmp898=(struct Cyc_Absyn_EnumType_struct*)_tmp85A;if(_tmp898->tag != 14)goto
_LL472;}_LL471: goto _LL473;_LL472: {struct Cyc_Absyn_AnonEnumType_struct*_tmp899=(
struct Cyc_Absyn_AnonEnumType_struct*)_tmp85A;if(_tmp899->tag != 15)goto _LL474;}
_LL473: goto _LL475;_LL474: {struct Cyc_Absyn_VoidType_struct*_tmp89A=(struct Cyc_Absyn_VoidType_struct*)
_tmp85A;if(_tmp89A->tag != 0)goto _LL476;}_LL475: goto _LL477;_LL476: {struct Cyc_Absyn_IntType_struct*
_tmp89B=(struct Cyc_Absyn_IntType_struct*)_tmp85A;if(_tmp89B->tag != 6)goto _LL478;}
_LL477: goto _LL479;_LL478: {struct Cyc_Absyn_FloatType_struct*_tmp89C=(struct Cyc_Absyn_FloatType_struct*)
_tmp85A;if(_tmp89C->tag != 7)goto _LL47A;}_LL479: goto _LL47B;_LL47A: {struct Cyc_Absyn_DoubleType_struct*
_tmp89D=(struct Cyc_Absyn_DoubleType_struct*)_tmp85A;if(_tmp89D->tag != 8)goto
_LL47C;}_LL47B: goto _LL47D;_LL47C: {struct Cyc_Absyn_UniqueRgn_struct*_tmp89E=(
struct Cyc_Absyn_UniqueRgn_struct*)_tmp85A;if(_tmp89E->tag != 22)goto _LL47E;}
_LL47D: goto _LL47F;_LL47E: {struct Cyc_Absyn_HeapRgn_struct*_tmp89F=(struct Cyc_Absyn_HeapRgn_struct*)
_tmp85A;if(_tmp89F->tag != 21)goto _LL480;}_LL47F: return t;_LL480: {struct Cyc_Absyn_RgnsEff_struct*
_tmp8A0=(struct Cyc_Absyn_RgnsEff_struct*)_tmp85A;if(_tmp8A0->tag != 25)goto _LL482;
else{_tmp8A1=(void*)_tmp8A0->f1;}}_LL481: {void*_tmp8FF=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp8A1);struct Cyc_Absyn_RgnsEff_struct _tmp1147;struct Cyc_Absyn_RgnsEff_struct*
_tmp1146;return _tmp8FF == _tmp8A1?t:(void*)((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((
_tmp1146[0]=((_tmp1147.tag=25,((_tmp1147.f1=(void*)_tmp8FF,_tmp1147)))),_tmp1146))));}
_LL482: {struct Cyc_Absyn_AccessEff_struct*_tmp8A2=(struct Cyc_Absyn_AccessEff_struct*)
_tmp85A;if(_tmp8A2->tag != 23)goto _LL484;else{_tmp8A3=(void*)_tmp8A2->f1;}}_LL483: {
void*_tmp902=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A3);struct Cyc_Absyn_AccessEff_struct
_tmp114A;struct Cyc_Absyn_AccessEff_struct*_tmp1149;return _tmp902 == _tmp8A3?t:(
void*)((_tmp1149=_cycalloc(sizeof(*_tmp1149)),((_tmp1149[0]=((_tmp114A.tag=23,((
_tmp114A.f1=(void*)_tmp902,_tmp114A)))),_tmp1149))));}_LL484: {struct Cyc_Absyn_JoinEff_struct*
_tmp8A4=(struct Cyc_Absyn_JoinEff_struct*)_tmp85A;if(_tmp8A4->tag != 24)goto _LL451;
else{_tmp8A5=_tmp8A4->f1;}}_LL485: {struct Cyc_List_List*_tmp905=Cyc_Tcutil_substs(
rgn,inst,_tmp8A5);struct Cyc_Absyn_JoinEff_struct _tmp114D;struct Cyc_Absyn_JoinEff_struct*
_tmp114C;return _tmp905 == _tmp8A5?t:(void*)((_tmp114C=_cycalloc(sizeof(*_tmp114C)),((
_tmp114C[0]=((_tmp114D.tag=24,((_tmp114D.f1=_tmp905,_tmp114D)))),_tmp114C))));}
_LL451:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 
0)return 0;{void*_tmp908=(void*)ts->hd;struct Cyc_List_List*_tmp909=ts->tl;void*
_tmp90A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp908);struct Cyc_List_List*_tmp90B=Cyc_Tcutil_substs(
rgn,inst,_tmp909);if(_tmp908 == _tmp90A  && _tmp909 == _tmp90B)return ts;{struct Cyc_List_List*
_tmp114E;return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmp114E=_cycalloc(
sizeof(*_tmp114E)),((_tmp114E->hd=_tmp90A,((_tmp114E->tl=_tmp90B,_tmp114E)))))));};};}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple14*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp90D=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,
Cyc_Absyn_BoxKind));struct Cyc_Core_Opt*_tmp1151;struct _tuple14*_tmp1150;return(
_tmp1150=_cycalloc(sizeof(*_tmp1150)),((_tmp1150->f1=tv,((_tmp1150->f2=Cyc_Absyn_new_evar(
_tmp90D,((_tmp1151=_cycalloc(sizeof(*_tmp1151)),((_tmp1151->v=s,_tmp1151))))),
_tmp1150)))));}struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple15*env,
struct Cyc_Absyn_Tvar*tv);struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct
_tuple15*env,struct Cyc_Absyn_Tvar*tv){struct _tuple15 _tmp911;struct Cyc_List_List*
_tmp912;struct _RegionHandle*_tmp913;struct _tuple15*_tmp910=env;_tmp911=*_tmp910;
_tmp912=_tmp911.f1;_tmp913=_tmp911.f2;{struct Cyc_Core_Opt*_tmp914=Cyc_Tcutil_kind_to_opt(
Cyc_Tcutil_tvar_kind(tv,Cyc_Absyn_BoxKind));struct Cyc_Core_Opt*_tmp1154;struct
_tuple14*_tmp1153;return(_tmp1153=_region_malloc(_tmp913,sizeof(*_tmp1153)),((
_tmp1153->f1=tv,((_tmp1153->f2=Cyc_Absyn_new_evar(_tmp914,((_tmp1154=_cycalloc(
sizeof(*_tmp1154)),((_tmp1154->v=_tmp912,_tmp1154))))),_tmp1153)))));};}static
struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->kind;void*k2=tv->kind;if(!Cyc_Tcutil_constrain_kinds(k1,k2)){const
char*_tmp115A;void*_tmp1159[3];struct Cyc_String_pa_struct _tmp1158;struct Cyc_String_pa_struct
_tmp1157;struct Cyc_String_pa_struct _tmp1156;(_tmp1156.tag=0,((_tmp1156.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2)),((
_tmp1157.tag=0,((_tmp1157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(
k1)),((_tmp1158.tag=0,((_tmp1158.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
tv->name),((_tmp1159[0]=& _tmp1158,((_tmp1159[1]=& _tmp1157,((_tmp1159[2]=&
_tmp1156,Cyc_Tcutil_terr(loc,((_tmp115A="type variable %s is used with inconsistent kinds %s and %s",
_tag_dyneither(_tmp115A,sizeof(char),59))),_tag_dyneither(_tmp1159,sizeof(void*),
3)))))))))))))))))));}if(tv->identity == - 1)tv->identity=((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity;else{if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp115D;void*_tmp115C;(_tmp115C=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp115D="same type variable has different identity!",
_tag_dyneither(_tmp115D,sizeof(char),43))),_tag_dyneither(_tmp115C,sizeof(void*),
0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct Cyc_List_List*
_tmp115E;return(_tmp115E=_cycalloc(sizeof(*_tmp115E)),((_tmp115E->hd=tv,((
_tmp115E->tl=tvs,_tmp115E)))));};}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1161;void*_tmp1160;(_tmp1160=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1161="fast_add_free_tvar: bad identity in tv",
_tag_dyneither(_tmp1161,sizeof(char),39))),_tag_dyneither(_tmp1160,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*
_tmp921=(struct Cyc_Absyn_Tvar*)tvs2->hd;if(_tmp921->identity == - 1){const char*
_tmp1164;void*_tmp1163;(_tmp1163=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1164="fast_add_free_tvar: bad identity in tvs2",
_tag_dyneither(_tmp1164,sizeof(char),41))),_tag_dyneither(_tmp1163,sizeof(void*),
0)));}if(_tmp921->identity == tv->identity)return tvs;}}{struct Cyc_List_List*
_tmp1165;return(_tmp1165=_cycalloc(sizeof(*_tmp1165)),((_tmp1165->hd=tv,((
_tmp1165->tl=tvs,_tmp1165)))));};}struct _tuple24{struct Cyc_Absyn_Tvar*f1;int f2;}
;static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct
_RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b);static
struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmp1168;void*_tmp1167;(_tmp1167=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1168="fast_add_free_tvar_bool: bad identity in tv",
_tag_dyneither(_tmp1168,sizeof(char),44))),_tag_dyneither(_tmp1167,sizeof(void*),
0)));}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){struct _tuple24
_tmp928;struct Cyc_Absyn_Tvar*_tmp929;int _tmp92A;int*_tmp92B;struct _tuple24*
_tmp927=(struct _tuple24*)tvs2->hd;_tmp928=*_tmp927;_tmp929=_tmp928.f1;_tmp92A=
_tmp928.f2;_tmp92B=(int*)&(*_tmp927).f2;if(_tmp929->identity == - 1){const char*
_tmp116B;void*_tmp116A;(_tmp116A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp116B="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dyneither(_tmp116B,sizeof(char),46))),_tag_dyneither(_tmp116A,sizeof(void*),
0)));}if(_tmp929->identity == tv->identity){*_tmp92B=*_tmp92B  || b;return tvs;}}}{
struct _tuple24*_tmp116E;struct Cyc_List_List*_tmp116D;return(_tmp116D=
_region_malloc(r,sizeof(*_tmp116D)),((_tmp116D->hd=((_tmp116E=_region_malloc(r,
sizeof(*_tmp116E)),((_tmp116E->f1=tv,((_tmp116E->f2=b,_tmp116E)))))),((_tmp116D->tl=
tvs,_tmp116D)))));};}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct
Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmp1172;void*_tmp1171[1];struct Cyc_String_pa_struct _tmp1170;(_tmp1170.tag=0,((
_tmp1170.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmp1171[0]=& _tmp1170,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1172="bound tvar id for %s is NULL",
_tag_dyneither(_tmp1172,sizeof(char),29))),_tag_dyneither(_tmp1171,sizeof(void*),
1)))))));}{struct Cyc_List_List*_tmp1173;return(_tmp1173=_cycalloc(sizeof(*
_tmp1173)),((_tmp1173->hd=tv,((_tmp1173->tl=tvs,_tmp1173)))));};}struct _tuple25{
void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct
_RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp934=Cyc_Tcutil_compress(e);int _tmp936;_LL491: {struct Cyc_Absyn_Evar_struct*
_tmp935=(struct Cyc_Absyn_Evar_struct*)_tmp934;if(_tmp935->tag != 1)goto _LL493;
else{_tmp936=_tmp935->f3;}}_LL492:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=
es2->tl){struct _tuple25 _tmp938;void*_tmp939;int _tmp93A;int*_tmp93B;struct
_tuple25*_tmp937=(struct _tuple25*)es2->hd;_tmp938=*_tmp937;_tmp939=_tmp938.f1;
_tmp93A=_tmp938.f2;_tmp93B=(int*)&(*_tmp937).f2;{void*_tmp93C=Cyc_Tcutil_compress(
_tmp939);int _tmp93E;_LL496: {struct Cyc_Absyn_Evar_struct*_tmp93D=(struct Cyc_Absyn_Evar_struct*)
_tmp93C;if(_tmp93D->tag != 1)goto _LL498;else{_tmp93E=_tmp93D->f3;}}_LL497: if(
_tmp936 == _tmp93E){if(b != *_tmp93B)*_tmp93B=1;return es;}goto _LL495;_LL498:;
_LL499: goto _LL495;_LL495:;};}}{struct _tuple25*_tmp1176;struct Cyc_List_List*
_tmp1175;return(_tmp1175=_region_malloc(r,sizeof(*_tmp1175)),((_tmp1175->hd=((
_tmp1176=_region_malloc(r,sizeof(*_tmp1176)),((_tmp1176->f1=e,((_tmp1176->f2=b,
_tmp1176)))))),((_tmp1175->tl=es,_tmp1175)))));};_LL493:;_LL494: return es;_LL490:;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,
struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmp1177;r=((_tmp1177=_region_malloc(rgn,sizeof(*_tmp1177)),((
_tmp1177->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1177->tl=r,_tmp1177))))));}}r=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs);static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs){struct Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp943;int _tmp944;struct _tuple24 _tmp942=*((struct _tuple24*)
tvs->hd);_tmp943=_tmp942.f1;_tmp944=_tmp942.f2;{int present=0;{struct Cyc_List_List*
b=btvs;for(0;b != 0;b=b->tl){if(_tmp943->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;break;}}}if(!present){struct Cyc_List_List*_tmp1178;res=((_tmp1178=
_region_malloc(r,sizeof(*_tmp1178)),((_tmp1178->hd=(struct _tuple24*)tvs->hd,((
_tmp1178->tl=res,_tmp1178))))));}};}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dyneither_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmp117C;void*_tmp117B[1];struct Cyc_String_pa_struct
_tmp117A;(_tmp117A.tag=0,((_tmp117A.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp117B[0]=& _tmp117A,Cyc_Tcutil_terr(loc,((_tmp117C="bitfield %s does not have constant width",
_tag_dyneither(_tmp117C,sizeof(char),41))),_tag_dyneither(_tmp117B,sizeof(void*),
1)))))));}else{unsigned int _tmp94A;int _tmp94B;struct _tuple13 _tmp949=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)width);_tmp94A=_tmp949.f1;_tmp94B=_tmp949.f2;if(!_tmp94B){
const char*_tmp117F;void*_tmp117E;(_tmp117E=0,Cyc_Tcutil_terr(loc,((_tmp117F="bitfield width cannot use sizeof or offsetof",
_tag_dyneither(_tmp117F,sizeof(char),45))),_tag_dyneither(_tmp117E,sizeof(void*),
0)));}w=_tmp94A;}{void*_tmp94E=Cyc_Tcutil_compress(field_typ);enum Cyc_Absyn_Size_of
_tmp950;_LL49B: {struct Cyc_Absyn_IntType_struct*_tmp94F=(struct Cyc_Absyn_IntType_struct*)
_tmp94E;if(_tmp94F->tag != 6)goto _LL49D;else{_tmp950=_tmp94F->f2;}}_LL49C: switch(
_tmp950){case Cyc_Absyn_Char_sz: _LL49F: if(w > 8){const char*_tmp1182;void*_tmp1181;(
_tmp1181=0,Cyc_Tcutil_terr(loc,((_tmp1182="bitfield larger than type",
_tag_dyneither(_tmp1182,sizeof(char),26))),_tag_dyneither(_tmp1181,sizeof(void*),
0)));}break;case Cyc_Absyn_Short_sz: _LL4A0: if(w > 16){const char*_tmp1185;void*
_tmp1184;(_tmp1184=0,Cyc_Tcutil_terr(loc,((_tmp1185="bitfield larger than type",
_tag_dyneither(_tmp1185,sizeof(char),26))),_tag_dyneither(_tmp1184,sizeof(void*),
0)));}break;case Cyc_Absyn_Long_sz: _LL4A1: goto _LL4A2;case Cyc_Absyn_Int_sz: _LL4A2:
if(w > 32){const char*_tmp1188;void*_tmp1187;(_tmp1187=0,Cyc_Tcutil_terr(loc,((
_tmp1188="bitfield larger than type",_tag_dyneither(_tmp1188,sizeof(char),26))),
_tag_dyneither(_tmp1187,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz:
_LL4A3: if(w > 64){const char*_tmp118B;void*_tmp118A;(_tmp118A=0,Cyc_Tcutil_terr(
loc,((_tmp118B="bitfield larger than type",_tag_dyneither(_tmp118B,sizeof(char),
26))),_tag_dyneither(_tmp118A,sizeof(void*),0)));}break;}goto _LL49A;_LL49D:;
_LL49E:{const char*_tmp1190;void*_tmp118F[2];struct Cyc_String_pa_struct _tmp118E;
struct Cyc_String_pa_struct _tmp118D;(_tmp118D.tag=0,((_tmp118D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_typ)),((
_tmp118E.tag=0,((_tmp118E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((
_tmp118F[0]=& _tmp118E,((_tmp118F[1]=& _tmp118D,Cyc_Tcutil_terr(loc,((_tmp1190="bitfield %s must have integral type but has type %s",
_tag_dyneither(_tmp1190,sizeof(char),52))),_tag_dyneither(_tmp118F,sizeof(void*),
2)))))))))))));}goto _LL49A;_LL49A:;};}}static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts);
static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct
_dyneither_ptr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*
_tmp95D=(void*)atts->hd;_LL4A6: {struct Cyc_Absyn_Packed_att_struct*_tmp95E=(
struct Cyc_Absyn_Packed_att_struct*)_tmp95D;if(_tmp95E->tag != 7)goto _LL4A8;}
_LL4A7: continue;_LL4A8: {struct Cyc_Absyn_Aligned_att_struct*_tmp95F=(struct Cyc_Absyn_Aligned_att_struct*)
_tmp95D;if(_tmp95F->tag != 6)goto _LL4AA;}_LL4A9: continue;_LL4AA:;_LL4AB: {const
char*_tmp1195;void*_tmp1194[2];struct Cyc_String_pa_struct _tmp1193;struct Cyc_String_pa_struct
_tmp1192;(_tmp1192.tag=0,((_tmp1192.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*fn),((_tmp1193.tag=0,((_tmp1193.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1194[0]=&
_tmp1193,((_tmp1194[1]=& _tmp1192,Cyc_Tcutil_terr(loc,((_tmp1195="bad attribute %s on member %s",
_tag_dyneither(_tmp1195,sizeof(char),30))),_tag_dyneither(_tmp1194,sizeof(void*),
2)))))))))))));}_LL4A5:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct
Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*
free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp964=t;struct
Cyc_Absyn_Typedefdecl*_tmp966;void**_tmp967;_LL4AD: {struct Cyc_Absyn_TypedefType_struct*
_tmp965=(struct Cyc_Absyn_TypedefType_struct*)_tmp964;if(_tmp965->tag != 18)goto
_LL4AF;else{_tmp966=_tmp965->f3;_tmp967=_tmp965->f4;}}_LL4AE: if((((struct Cyc_Absyn_Typedefdecl*)
_check_null(_tmp966))->tq).real_const  || (_tmp966->tq).print_const){if(
declared_const){const char*_tmp1198;void*_tmp1197;(_tmp1197=0,Cyc_Tcutil_warn(loc,((
_tmp1198="extra const",_tag_dyneither(_tmp1198,sizeof(char),12))),_tag_dyneither(
_tmp1197,sizeof(void*),0)));}return 1;}if((unsigned int)_tmp967)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp967);else{return declared_const;}_LL4AF:;_LL4B0: return
declared_const;_LL4AC:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
struct _tuple26{enum Cyc_Absyn_Format_Type f1;void*f2;};static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,enum Cyc_Absyn_Kind expected_kind,void*t,int
put_in_effect);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(struct
Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,
enum Cyc_Absyn_Kind expected_kind,void*t,int put_in_effect){static struct Cyc_Core_Opt
urgn={(void*)((void*)& Cyc_Absyn_UniqueRgn_val)};static struct Cyc_Core_Opt hrgn={(
void*)((void*)& Cyc_Absyn_HeapRgn_val)};{void*_tmp96A=Cyc_Tcutil_compress(t);
struct Cyc_Core_Opt*_tmp96D;struct Cyc_Core_Opt**_tmp96E;struct Cyc_Core_Opt*
_tmp96F;struct Cyc_Core_Opt**_tmp970;struct Cyc_Absyn_Tvar*_tmp972;struct Cyc_List_List*
_tmp974;struct _tuple2*_tmp976;struct Cyc_Absyn_Enumdecl*_tmp977;struct Cyc_Absyn_Enumdecl**
_tmp978;struct Cyc_Absyn_DatatypeInfo _tmp97A;union Cyc_Absyn_DatatypeInfoU _tmp97B;
union Cyc_Absyn_DatatypeInfoU*_tmp97C;struct Cyc_List_List*_tmp97D;struct Cyc_List_List**
_tmp97E;struct Cyc_Absyn_DatatypeFieldInfo _tmp980;union Cyc_Absyn_DatatypeFieldInfoU
_tmp981;union Cyc_Absyn_DatatypeFieldInfoU*_tmp982;struct Cyc_List_List*_tmp983;
struct Cyc_Absyn_PtrInfo _tmp985;void*_tmp986;struct Cyc_Absyn_Tqual _tmp987;struct
Cyc_Absyn_Tqual*_tmp988;struct Cyc_Absyn_PtrAtts _tmp989;void*_tmp98A;union Cyc_Absyn_Constraint*
_tmp98B;union Cyc_Absyn_Constraint*_tmp98C;union Cyc_Absyn_Constraint*_tmp98D;void*
_tmp98F;struct Cyc_Absyn_Exp*_tmp991;struct Cyc_Absyn_ArrayInfo _tmp996;void*
_tmp997;struct Cyc_Absyn_Tqual _tmp998;struct Cyc_Absyn_Tqual*_tmp999;struct Cyc_Absyn_Exp*
_tmp99A;struct Cyc_Absyn_Exp**_tmp99B;union Cyc_Absyn_Constraint*_tmp99C;struct Cyc_Position_Segment*
_tmp99D;struct Cyc_Absyn_FnInfo _tmp99F;struct Cyc_List_List*_tmp9A0;struct Cyc_List_List**
_tmp9A1;struct Cyc_Core_Opt*_tmp9A2;struct Cyc_Core_Opt**_tmp9A3;void*_tmp9A4;
struct Cyc_List_List*_tmp9A5;int _tmp9A6;struct Cyc_Absyn_VarargInfo*_tmp9A7;struct
Cyc_List_List*_tmp9A8;struct Cyc_List_List*_tmp9A9;struct Cyc_List_List*_tmp9AB;
enum Cyc_Absyn_AggrKind _tmp9AD;struct Cyc_List_List*_tmp9AE;struct Cyc_Absyn_AggrInfo
_tmp9B0;union Cyc_Absyn_AggrInfoU _tmp9B1;union Cyc_Absyn_AggrInfoU*_tmp9B2;struct
Cyc_List_List*_tmp9B3;struct Cyc_List_List**_tmp9B4;struct _tuple2*_tmp9B6;struct
Cyc_List_List*_tmp9B7;struct Cyc_List_List**_tmp9B8;struct Cyc_Absyn_Typedefdecl*
_tmp9B9;struct Cyc_Absyn_Typedefdecl**_tmp9BA;void**_tmp9BB;void***_tmp9BC;void*
_tmp9C0;void*_tmp9C2;void*_tmp9C3;void*_tmp9C5;void*_tmp9C7;struct Cyc_List_List*
_tmp9C9;_LL4B2: {struct Cyc_Absyn_VoidType_struct*_tmp96B=(struct Cyc_Absyn_VoidType_struct*)
_tmp96A;if(_tmp96B->tag != 0)goto _LL4B4;}_LL4B3: goto _LL4B1;_LL4B4: {struct Cyc_Absyn_Evar_struct*
_tmp96C=(struct Cyc_Absyn_Evar_struct*)_tmp96A;if(_tmp96C->tag != 1)goto _LL4B6;
else{_tmp96D=_tmp96C->f1;_tmp96E=(struct Cyc_Core_Opt**)& _tmp96C->f1;_tmp96F=
_tmp96C->f2;_tmp970=(struct Cyc_Core_Opt**)& _tmp96C->f2;}}_LL4B5: if(*_tmp96E == 0)*
_tmp96E=Cyc_Tcutil_kind_to_opt(expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars)
 && Cyc_Tcutil_is_region_kind(expected_kind)){if(expected_kind == Cyc_Absyn_UniqueRgnKind)*
_tmp970=(struct Cyc_Core_Opt*)& urgn;else{*_tmp970=(struct Cyc_Core_Opt*)& hrgn;}}
else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Less_kb_struct _tmp119B;struct
Cyc_Absyn_Less_kb_struct*_tmp119A;struct Cyc_Absyn_Tvar*_tmp9CA=Cyc_Tcutil_new_tvar((
void*)((_tmp119A=_cycalloc(sizeof(*_tmp119A)),((_tmp119A[0]=((_tmp119B.tag=2,((
_tmp119B.f1=0,((_tmp119B.f2=expected_kind,_tmp119B)))))),_tmp119A)))));{struct
Cyc_Absyn_VarType_struct*_tmp11A1;struct Cyc_Absyn_VarType_struct _tmp11A0;struct
Cyc_Core_Opt*_tmp119F;*_tmp970=((_tmp119F=_cycalloc(sizeof(*_tmp119F)),((
_tmp119F->v=(void*)((_tmp11A1=_cycalloc(sizeof(*_tmp11A1)),((_tmp11A1[0]=((
_tmp11A0.tag=2,((_tmp11A0.f1=_tmp9CA,_tmp11A0)))),_tmp11A1)))),_tmp119F))));}
_tmp972=_tmp9CA;goto _LL4B7;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto _LL4B1;_LL4B6: {struct Cyc_Absyn_VarType_struct*
_tmp971=(struct Cyc_Absyn_VarType_struct*)_tmp96A;if(_tmp971->tag != 2)goto _LL4B8;
else{_tmp972=_tmp971->f1;}}_LL4B7:{void*_tmp9D0=Cyc_Absyn_compress_kb(_tmp972->kind);
struct Cyc_Core_Opt*_tmp9D2;struct Cyc_Core_Opt**_tmp9D3;_LL4E7: {struct Cyc_Absyn_Unknown_kb_struct*
_tmp9D1=(struct Cyc_Absyn_Unknown_kb_struct*)_tmp9D0;if(_tmp9D1->tag != 1)goto
_LL4E9;else{_tmp9D2=_tmp9D1->f1;_tmp9D3=(struct Cyc_Core_Opt**)& _tmp9D1->f1;}}
_LL4E8:{struct Cyc_Absyn_Less_kb_struct*_tmp11A7;struct Cyc_Absyn_Less_kb_struct
_tmp11A6;struct Cyc_Core_Opt*_tmp11A5;*_tmp9D3=((_tmp11A5=_cycalloc(sizeof(*
_tmp11A5)),((_tmp11A5->v=(void*)((_tmp11A7=_cycalloc(sizeof(*_tmp11A7)),((
_tmp11A7[0]=((_tmp11A6.tag=2,((_tmp11A6.f1=0,((_tmp11A6.f2=expected_kind,
_tmp11A6)))))),_tmp11A7)))),_tmp11A5))));}goto _LL4E6;_LL4E9:;_LL4EA: goto _LL4E6;
_LL4E6:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp972);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp972,put_in_effect);{void*_tmp9D7=Cyc_Absyn_compress_kb(_tmp972->kind);struct
Cyc_Core_Opt*_tmp9D9;struct Cyc_Core_Opt**_tmp9DA;enum Cyc_Absyn_Kind _tmp9DB;
_LL4EC: {struct Cyc_Absyn_Less_kb_struct*_tmp9D8=(struct Cyc_Absyn_Less_kb_struct*)
_tmp9D7;if(_tmp9D8->tag != 2)goto _LL4EE;else{_tmp9D9=_tmp9D8->f1;_tmp9DA=(struct
Cyc_Core_Opt**)& _tmp9D8->f1;_tmp9DB=_tmp9D8->f2;}}_LL4ED: if(Cyc_Tcutil_kind_leq(
expected_kind,_tmp9DB)){struct Cyc_Absyn_Less_kb_struct*_tmp11AD;struct Cyc_Absyn_Less_kb_struct
_tmp11AC;struct Cyc_Core_Opt*_tmp11AB;*_tmp9DA=((_tmp11AB=_cycalloc(sizeof(*
_tmp11AB)),((_tmp11AB->v=(void*)((_tmp11AD=_cycalloc(sizeof(*_tmp11AD)),((
_tmp11AD[0]=((_tmp11AC.tag=2,((_tmp11AC.f1=0,((_tmp11AC.f2=expected_kind,
_tmp11AC)))))),_tmp11AD)))),_tmp11AB))));}goto _LL4EB;_LL4EE:;_LL4EF: goto _LL4EB;
_LL4EB:;}goto _LL4B1;_LL4B8: {struct Cyc_Absyn_AnonEnumType_struct*_tmp973=(struct
Cyc_Absyn_AnonEnumType_struct*)_tmp96A;if(_tmp973->tag != 15)goto _LL4BA;else{
_tmp974=_tmp973->f1;}}_LL4B9: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle*_tmp9DF=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp974 != 0;_tmp974=_tmp974->tl){
struct Cyc_Absyn_Enumfield*_tmp9E0=(struct Cyc_Absyn_Enumfield*)_tmp974->hd;if(((
int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp9E0->name).f2)){
const char*_tmp11B1;void*_tmp11B0[1];struct Cyc_String_pa_struct _tmp11AF;(_tmp11AF.tag=
0,((_tmp11AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp9E0->name).f2),((
_tmp11B0[0]=& _tmp11AF,Cyc_Tcutil_terr(_tmp9E0->loc,((_tmp11B1="duplicate enum field name %s",
_tag_dyneither(_tmp11B1,sizeof(char),29))),_tag_dyneither(_tmp11B0,sizeof(void*),
1)))))));}else{struct Cyc_List_List*_tmp11B2;prev_fields=((_tmp11B2=
_region_malloc(_tmp9DF,sizeof(*_tmp11B2)),((_tmp11B2->hd=(*_tmp9E0->name).f2,((
_tmp11B2->tl=prev_fields,_tmp11B2))))));}if(_tmp9E0->tag == 0)_tmp9E0->tag=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp9E0->loc);else{if(!Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmp9E0->tag))){const char*_tmp11B6;void*
_tmp11B5[1];struct Cyc_String_pa_struct _tmp11B4;(_tmp11B4.tag=0,((_tmp11B4.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp9E0->name).f2),((_tmp11B5[0]=&
_tmp11B4,Cyc_Tcutil_terr(loc,((_tmp11B6="enum field %s: expression is not constant",
_tag_dyneither(_tmp11B6,sizeof(char),42))),_tag_dyneither(_tmp11B5,sizeof(void*),
1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp9E0->tag))).f1;tag_count=t1 + 1;(*_tmp9E0->name).f1=Cyc_Absyn_Abs_n(
te->ns);{struct Cyc_Tcenv_AnonEnumRes_struct*_tmp11BC;struct Cyc_Tcenv_AnonEnumRes_struct
_tmp11BB;struct _tuple25*_tmp11BA;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct
Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple25*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp9E0->name).f2,(struct _tuple25*)((_tmp11BA=_cycalloc(sizeof(*_tmp11BA)),((
_tmp11BA->f1=(void*)((_tmp11BC=_cycalloc(sizeof(*_tmp11BC)),((_tmp11BC[0]=((
_tmp11BB.tag=4,((_tmp11BB.f1=(void*)t,((_tmp11BB.f2=_tmp9E0,_tmp11BB)))))),
_tmp11BC)))),((_tmp11BA->f2=1,_tmp11BA)))))));};};}}goto _LL4B1;}_LL4BA: {struct
Cyc_Absyn_EnumType_struct*_tmp975=(struct Cyc_Absyn_EnumType_struct*)_tmp96A;if(
_tmp975->tag != 14)goto _LL4BC;else{_tmp976=_tmp975->f1;_tmp977=_tmp975->f2;
_tmp978=(struct Cyc_Absyn_Enumdecl**)& _tmp975->f2;}}_LL4BB: if(*_tmp978 == 0  || ((
struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp978))->fields == 0){struct
_handler_cons _tmp9EB;_push_handler(& _tmp9EB);{int _tmp9ED=0;if(setjmp(_tmp9EB.handler))
_tmp9ED=1;if(!_tmp9ED){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp976);*_tmp978=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmp9EC=(void*)_exn_thrown;void*_tmp9EF=_tmp9EC;_LL4F1: {struct Cyc_Dict_Absent_struct*
_tmp9F0=(struct Cyc_Dict_Absent_struct*)_tmp9EF;if(_tmp9F0->tag != Cyc_Dict_Absent)
goto _LL4F3;}_LL4F2: {struct Cyc_Tcenv_Genv*_tmp9F1=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp11BD;struct Cyc_Absyn_Enumdecl*_tmp9F2=(_tmp11BD=
_cycalloc(sizeof(*_tmp11BD)),((_tmp11BD->sc=Cyc_Absyn_Extern,((_tmp11BD->name=
_tmp976,((_tmp11BD->fields=0,_tmp11BD)))))));Cyc_Tc_tcEnumdecl(te,_tmp9F1,loc,
_tmp9F2);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp976);*
_tmp978=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL4F0;};}_LL4F3:;_LL4F4:(void)_throw(
_tmp9EF);_LL4F0:;}};}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp978);*_tmp976=(ed->name)[_check_known_subscript_notnull(1,0)];
goto _LL4B1;};_LL4BC: {struct Cyc_Absyn_DatatypeType_struct*_tmp979=(struct Cyc_Absyn_DatatypeType_struct*)
_tmp96A;if(_tmp979->tag != 3)goto _LL4BE;else{_tmp97A=_tmp979->f1;_tmp97B=_tmp97A.datatype_info;
_tmp97C=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp979->f1).datatype_info;_tmp97D=
_tmp97A.targs;_tmp97E=(struct Cyc_List_List**)&(_tmp979->f1).targs;}}_LL4BD: {
struct Cyc_List_List*_tmp9F4=*_tmp97E;{union Cyc_Absyn_DatatypeInfoU _tmp9F5=*
_tmp97C;struct Cyc_Absyn_UnknownDatatypeInfo _tmp9F6;struct _tuple2*_tmp9F7;int
_tmp9F8;struct Cyc_Absyn_Datatypedecl**_tmp9F9;struct Cyc_Absyn_Datatypedecl*
_tmp9FA;_LL4F6: if((_tmp9F5.UnknownDatatype).tag != 1)goto _LL4F8;_tmp9F6=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmp9F5.UnknownDatatype).val;_tmp9F7=_tmp9F6.name;
_tmp9F8=_tmp9F6.is_extensible;_LL4F7: {struct Cyc_Absyn_Datatypedecl**tudp;{
struct _handler_cons _tmp9FB;_push_handler(& _tmp9FB);{int _tmp9FD=0;if(setjmp(
_tmp9FB.handler))_tmp9FD=1;if(!_tmp9FD){tudp=Cyc_Tcenv_lookup_datatypedecl(te,
loc,_tmp9F7);;_pop_handler();}else{void*_tmp9FC=(void*)_exn_thrown;void*_tmp9FF=
_tmp9FC;_LL4FB: {struct Cyc_Dict_Absent_struct*_tmpA00=(struct Cyc_Dict_Absent_struct*)
_tmp9FF;if(_tmpA00->tag != Cyc_Dict_Absent)goto _LL4FD;}_LL4FC: {struct Cyc_Tcenv_Genv*
_tmpA01=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Datatypedecl*_tmp11BE;struct Cyc_Absyn_Datatypedecl*
_tmpA02=(_tmp11BE=_cycalloc(sizeof(*_tmp11BE)),((_tmp11BE->sc=Cyc_Absyn_Extern,((
_tmp11BE->name=_tmp9F7,((_tmp11BE->tvs=0,((_tmp11BE->fields=0,((_tmp11BE->is_extensible=
_tmp9F8,_tmp11BE)))))))))));Cyc_Tc_tcDatatypedecl(te,_tmpA01,loc,_tmpA02);tudp=
Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp9F7);if(_tmp9F4 != 0){{const char*
_tmp11C2;void*_tmp11C1[1];struct Cyc_String_pa_struct _tmp11C0;(_tmp11C0.tag=0,((
_tmp11C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9F7)),((_tmp11C1[0]=& _tmp11C0,Cyc_Tcutil_terr(loc,((_tmp11C2="declare parameterized datatype %s before using",
_tag_dyneither(_tmp11C2,sizeof(char),47))),_tag_dyneither(_tmp11C1,sizeof(void*),
1)))))));}return cvtenv;}goto _LL4FA;}_LL4FD:;_LL4FE:(void)_throw(_tmp9FF);_LL4FA:;}};}
if(_tmp9F8  && !(*tudp)->is_extensible){const char*_tmp11C6;void*_tmp11C5[1];
struct Cyc_String_pa_struct _tmp11C4;(_tmp11C4.tag=0,((_tmp11C4.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9F7)),((
_tmp11C5[0]=& _tmp11C4,Cyc_Tcutil_terr(loc,((_tmp11C6="datatype %s was not declared @extensible",
_tag_dyneither(_tmp11C6,sizeof(char),41))),_tag_dyneither(_tmp11C5,sizeof(void*),
1)))))));}*_tmp97C=Cyc_Absyn_KnownDatatype(tudp);_tmp9FA=*tudp;goto _LL4F9;}
_LL4F8: if((_tmp9F5.KnownDatatype).tag != 2)goto _LL4F5;_tmp9F9=(struct Cyc_Absyn_Datatypedecl**)(
_tmp9F5.KnownDatatype).val;_tmp9FA=*_tmp9F9;_LL4F9: {struct Cyc_List_List*tvs=
_tmp9FA->tvs;for(0;_tmp9F4 != 0  && tvs != 0;(_tmp9F4=_tmp9F4->tl,tvs=tvs->tl)){
void*t=(void*)_tmp9F4->hd;struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;{
struct _tuple0 _tmp11C7;struct _tuple0 _tmpA0B=(_tmp11C7.f1=Cyc_Absyn_compress_kb(tv->kind),((
_tmp11C7.f2=t,_tmp11C7)));void*_tmpA0C;void*_tmpA0E;struct Cyc_Absyn_Tvar*_tmpA10;
_LL500: _tmpA0C=_tmpA0B.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpA0D=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA0C;if(_tmpA0D->tag != 1)goto _LL502;};_tmpA0E=_tmpA0B.f2;{struct Cyc_Absyn_VarType_struct*
_tmpA0F=(struct Cyc_Absyn_VarType_struct*)_tmpA0E;if(_tmpA0F->tag != 2)goto _LL502;
else{_tmpA10=_tmpA0F->f1;}};_LL501: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpA10);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpA10,1);continue;_LL502:;_LL503: goto _LL4FF;_LL4FF:;}{
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind(tv,Cyc_Absyn_BoxKind);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmp9F4 != 0){const char*_tmp11CB;void*_tmp11CA[1];
struct Cyc_String_pa_struct _tmp11C9;(_tmp11C9.tag=0,((_tmp11C9.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9FA->name)),((
_tmp11CA[0]=& _tmp11C9,Cyc_Tcutil_terr(loc,((_tmp11CB="too many type arguments for datatype %s",
_tag_dyneither(_tmp11CB,sizeof(char),40))),_tag_dyneither(_tmp11CA,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
enum Cyc_Absyn_Kind k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp11CC;hidden_ts=((
_tmp11CC=_cycalloc(sizeof(*_tmp11CC)),((_tmp11CC->hd=e,((_tmp11CC->tl=hidden_ts,
_tmp11CC))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}*
_tmp97E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(*_tmp97E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}goto _LL4F5;}_LL4F5:;}goto _LL4B1;}_LL4BE: {struct
Cyc_Absyn_DatatypeFieldType_struct*_tmp97F=(struct Cyc_Absyn_DatatypeFieldType_struct*)
_tmp96A;if(_tmp97F->tag != 4)goto _LL4C0;else{_tmp980=_tmp97F->f1;_tmp981=_tmp980.field_info;
_tmp982=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp97F->f1).field_info;_tmp983=
_tmp980.targs;}}_LL4BF:{union Cyc_Absyn_DatatypeFieldInfoU _tmpA15=*_tmp982;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmpA16;struct _tuple2*_tmpA17;struct _tuple2*
_tmpA18;int _tmpA19;struct _tuple3 _tmpA1A;struct Cyc_Absyn_Datatypedecl*_tmpA1B;
struct Cyc_Absyn_Datatypefield*_tmpA1C;_LL505: if((_tmpA15.UnknownDatatypefield).tag
!= 1)goto _LL507;_tmpA16=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA15.UnknownDatatypefield).val;
_tmpA17=_tmpA16.datatype_name;_tmpA18=_tmpA16.field_name;_tmpA19=_tmpA16.is_extensible;
_LL506: {struct Cyc_Absyn_Datatypedecl*tud;struct Cyc_Absyn_Datatypefield*tuf;{
struct _handler_cons _tmpA1D;_push_handler(& _tmpA1D);{int _tmpA1F=0;if(setjmp(
_tmpA1D.handler))_tmpA1F=1;if(!_tmpA1F){*Cyc_Tcenv_lookup_datatypedecl(te,loc,
_tmpA17);;_pop_handler();}else{void*_tmpA1E=(void*)_exn_thrown;void*_tmpA21=
_tmpA1E;_LL50A: {struct Cyc_Dict_Absent_struct*_tmpA22=(struct Cyc_Dict_Absent_struct*)
_tmpA21;if(_tmpA22->tag != Cyc_Dict_Absent)goto _LL50C;}_LL50B:{const char*_tmp11D0;
void*_tmp11CF[1];struct Cyc_String_pa_struct _tmp11CE;(_tmp11CE.tag=0,((_tmp11CE.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA17)),((
_tmp11CF[0]=& _tmp11CE,Cyc_Tcutil_terr(loc,((_tmp11D0="unbound datatype %s",
_tag_dyneither(_tmp11D0,sizeof(char),20))),_tag_dyneither(_tmp11CF,sizeof(void*),
1)))))));}return cvtenv;_LL50C:;_LL50D:(void)_throw(_tmpA21);_LL509:;}};}{struct
_handler_cons _tmpA26;_push_handler(& _tmpA26);{int _tmpA28=0;if(setjmp(_tmpA26.handler))
_tmpA28=1;if(!_tmpA28){{struct _RegionHandle*_tmpA29=Cyc_Tcenv_get_fnrgn(te);void*
_tmpA2A=Cyc_Tcenv_lookup_ordinary(_tmpA29,te,loc,_tmpA18);struct Cyc_Absyn_Datatypedecl*
_tmpA2C;struct Cyc_Absyn_Datatypefield*_tmpA2D;_LL50F: {struct Cyc_Tcenv_DatatypeRes_struct*
_tmpA2B=(struct Cyc_Tcenv_DatatypeRes_struct*)_tmpA2A;if(_tmpA2B->tag != 2)goto
_LL511;else{_tmpA2C=_tmpA2B->f1;_tmpA2D=_tmpA2B->f2;}}_LL510: tuf=_tmpA2D;tud=
_tmpA2C;if(_tmpA19  && !tud->is_extensible){const char*_tmp11D4;void*_tmp11D3[1];
struct Cyc_String_pa_struct _tmp11D2;(_tmp11D2.tag=0,((_tmp11D2.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA17)),((
_tmp11D3[0]=& _tmp11D2,Cyc_Tcutil_terr(loc,((_tmp11D4="datatype %s was not declared @extensible",
_tag_dyneither(_tmp11D4,sizeof(char),41))),_tag_dyneither(_tmp11D3,sizeof(void*),
1)))))));}goto _LL50E;_LL511:;_LL512:{const char*_tmp11D9;void*_tmp11D8[2];struct
Cyc_String_pa_struct _tmp11D7;struct Cyc_String_pa_struct _tmp11D6;(_tmp11D6.tag=0,((
_tmp11D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA17)),((_tmp11D7.tag=0,((_tmp11D7.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA18)),((_tmp11D8[0]=& _tmp11D7,((
_tmp11D8[1]=& _tmp11D6,Cyc_Tcutil_terr(loc,((_tmp11D9="unbound field %s in type datatype %s",
_tag_dyneither(_tmp11D9,sizeof(char),37))),_tag_dyneither(_tmp11D8,sizeof(void*),
2)))))))))))));}{struct Cyc_Tcutil_CVTEnv _tmpA35=cvtenv;_npop_handler(0);return
_tmpA35;};_LL50E:;};_pop_handler();}else{void*_tmpA27=(void*)_exn_thrown;void*
_tmpA37=_tmpA27;_LL514: {struct Cyc_Dict_Absent_struct*_tmpA38=(struct Cyc_Dict_Absent_struct*)
_tmpA37;if(_tmpA38->tag != Cyc_Dict_Absent)goto _LL516;}_LL515:{const char*_tmp11DE;
void*_tmp11DD[2];struct Cyc_String_pa_struct _tmp11DC;struct Cyc_String_pa_struct
_tmp11DB;(_tmp11DB.tag=0,((_tmp11DB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA17)),((_tmp11DC.tag=0,((_tmp11DC.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA18)),((
_tmp11DD[0]=& _tmp11DC,((_tmp11DD[1]=& _tmp11DB,Cyc_Tcutil_terr(loc,((_tmp11DE="unbound field %s in type datatype %s",
_tag_dyneither(_tmp11DE,sizeof(char),37))),_tag_dyneither(_tmp11DD,sizeof(void*),
2)))))))))))));}return cvtenv;_LL516:;_LL517:(void)_throw(_tmpA37);_LL513:;}};}*
_tmp982=Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmpA1B=tud;_tmpA1C=tuf;goto _LL508;}
_LL507: if((_tmpA15.KnownDatatypefield).tag != 2)goto _LL504;_tmpA1A=(struct _tuple3)(
_tmpA15.KnownDatatypefield).val;_tmpA1B=_tmpA1A.f1;_tmpA1C=_tmpA1A.f2;_LL508: {
struct Cyc_List_List*tvs=_tmpA1B->tvs;for(0;_tmp983 != 0  && tvs != 0;(_tmp983=
_tmp983->tl,tvs=tvs->tl)){void*t=(void*)_tmp983->hd;struct Cyc_Absyn_Tvar*tv=(
struct Cyc_Absyn_Tvar*)tvs->hd;{struct _tuple0 _tmp11DF;struct _tuple0 _tmpA3E=(
_tmp11DF.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp11DF.f2=t,_tmp11DF)));void*
_tmpA3F;void*_tmpA41;struct Cyc_Absyn_Tvar*_tmpA43;_LL519: _tmpA3F=_tmpA3E.f1;{
struct Cyc_Absyn_Unknown_kb_struct*_tmpA40=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpA3F;if(_tmpA40->tag != 1)goto _LL51B;};_tmpA41=_tmpA3E.f2;{struct Cyc_Absyn_VarType_struct*
_tmpA42=(struct Cyc_Absyn_VarType_struct*)_tmpA41;if(_tmpA42->tag != 2)goto _LL51B;
else{_tmpA43=_tmpA42->f1;}};_LL51A: cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,
cvtenv.kind_env,_tmpA43);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpA43,1);continue;_LL51B:;_LL51C: goto _LL518;_LL518:;}{
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind(tv,Cyc_Absyn_BoxKind);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,t,1);};}if(_tmp983 != 0){const char*_tmp11E4;void*_tmp11E3[2];
struct Cyc_String_pa_struct _tmp11E2;struct Cyc_String_pa_struct _tmp11E1;(_tmp11E1.tag=
0,((_tmp11E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA1C->name)),((_tmp11E2.tag=0,((_tmp11E2.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1B->name)),((_tmp11E3[0]=& _tmp11E2,((
_tmp11E3[1]=& _tmp11E1,Cyc_Tcutil_terr(loc,((_tmp11E4="too many type arguments for datatype %s.%s",
_tag_dyneither(_tmp11E4,sizeof(char),43))),_tag_dyneither(_tmp11E3,sizeof(void*),
2)))))))))))));}if(tvs != 0){const char*_tmp11E9;void*_tmp11E8[2];struct Cyc_String_pa_struct
_tmp11E7;struct Cyc_String_pa_struct _tmp11E6;(_tmp11E6.tag=0,((_tmp11E6.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA1C->name)),((
_tmp11E7.tag=0,((_tmp11E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpA1B->name)),((_tmp11E8[0]=& _tmp11E7,((_tmp11E8[1]=& _tmp11E6,Cyc_Tcutil_terr(
loc,((_tmp11E9="too few type arguments for datatype %s.%s",_tag_dyneither(
_tmp11E9,sizeof(char),42))),_tag_dyneither(_tmp11E8,sizeof(void*),2)))))))))))));}
goto _LL504;}_LL504:;}goto _LL4B1;_LL4C0: {struct Cyc_Absyn_PointerType_struct*
_tmp984=(struct Cyc_Absyn_PointerType_struct*)_tmp96A;if(_tmp984->tag != 5)goto
_LL4C2;else{_tmp985=_tmp984->f1;_tmp986=_tmp985.elt_typ;_tmp987=_tmp985.elt_tq;
_tmp988=(struct Cyc_Absyn_Tqual*)&(_tmp984->f1).elt_tq;_tmp989=_tmp985.ptr_atts;
_tmp98A=_tmp989.rgn;_tmp98B=_tmp989.nullable;_tmp98C=_tmp989.bounds;_tmp98D=
_tmp989.zero_term;}}_LL4C1: {int is_zero_terminated;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_AnyKind,_tmp986,1);_tmp988->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp988->print_const,_tmp986);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,Cyc_Absyn_TopRgnKind,_tmp98A,1);{union Cyc_Absyn_Constraint*_tmpA4C=((
union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(
_tmp98D);union Cyc_Absyn_Constraint _tmpA4D;int _tmpA4E;union Cyc_Absyn_Constraint
_tmpA4F;int _tmpA50;_LL51E: _tmpA4D=*_tmpA4C;if((_tmpA4D.No_constr).tag != 3)goto
_LL520;_tmpA4E=(int)(_tmpA4D.No_constr).val;_LL51F:{void*_tmpA51=Cyc_Tcutil_compress(
_tmp986);enum Cyc_Absyn_Size_of _tmpA53;_LL525: {struct Cyc_Absyn_IntType_struct*
_tmpA52=(struct Cyc_Absyn_IntType_struct*)_tmpA51;if(_tmpA52->tag != 6)goto _LL527;
else{_tmpA53=_tmpA52->f2;if(_tmpA53 != Cyc_Absyn_Char_sz)goto _LL527;}}_LL526:((
int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp98D,Cyc_Absyn_true_conref);
is_zero_terminated=1;goto _LL524;_LL527:;_LL528:((int(*)(int(*cmp)(int,int),union
Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp98D,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL524;
_LL524:;}goto _LL51D;_LL520: _tmpA4F=*_tmpA4C;if((_tmpA4F.Eq_constr).tag != 1)goto
_LL522;_tmpA50=(int)(_tmpA4F.Eq_constr).val;if(_tmpA50 != 1)goto _LL522;_LL521: if(
!Cyc_Tcutil_admits_zero(_tmp986)){const char*_tmp11ED;void*_tmp11EC[1];struct Cyc_String_pa_struct
_tmp11EB;(_tmp11EB.tag=0,((_tmp11EB.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(_tmp986)),((_tmp11EC[0]=& _tmp11EB,Cyc_Tcutil_terr(
loc,((_tmp11ED="cannot have a pointer to zero-terminated %s elements",
_tag_dyneither(_tmp11ED,sizeof(char),53))),_tag_dyneither(_tmp11EC,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL51D;_LL522:;_LL523: is_zero_terminated=0;
goto _LL51D;_LL51D:;}{void*_tmpA57=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))
Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp98C);struct Cyc_Absyn_Exp*
_tmpA5A;_LL52A: {struct Cyc_Absyn_DynEither_b_struct*_tmpA58=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpA57;if(_tmpA58->tag != 0)goto _LL52C;}_LL52B: goto _LL529;_LL52C: {struct Cyc_Absyn_Upper_b_struct*
_tmpA59=(struct Cyc_Absyn_Upper_b_struct*)_tmpA57;if(_tmpA59->tag != 1)goto _LL529;
else{_tmpA5A=_tmpA59->f1;}}_LL52D: {struct _RegionHandle*_tmpA5B=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpA5C=Cyc_Tcenv_allow_valueof(_tmpA5B,te);Cyc_Tcexp_tcExp(
_tmpA5C,0,_tmpA5A);}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA5A,te,
cvtenv);if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA5A)){const char*_tmp11F0;void*
_tmp11EF;(_tmp11EF=0,Cyc_Tcutil_terr(loc,((_tmp11F0="pointer bounds expression is not an unsigned int",
_tag_dyneither(_tmp11F0,sizeof(char),49))),_tag_dyneither(_tmp11EF,sizeof(void*),
0)));}{unsigned int _tmpA60;int _tmpA61;struct _tuple13 _tmpA5F=Cyc_Evexp_eval_const_uint_exp(
_tmpA5A);_tmpA60=_tmpA5F.f1;_tmpA61=_tmpA5F.f2;if(is_zero_terminated  && (!
_tmpA61  || _tmpA60 < 1)){const char*_tmp11F3;void*_tmp11F2;(_tmp11F2=0,Cyc_Tcutil_terr(
loc,((_tmp11F3="zero-terminated pointer cannot point to empty sequence",
_tag_dyneither(_tmp11F3,sizeof(char),55))),_tag_dyneither(_tmp11F2,sizeof(void*),
0)));}goto _LL529;};}_LL529:;}goto _LL4B1;}_LL4C2: {struct Cyc_Absyn_TagType_struct*
_tmp98E=(struct Cyc_Absyn_TagType_struct*)_tmp96A;if(_tmp98E->tag != 20)goto _LL4C4;
else{_tmp98F=(void*)_tmp98E->f1;}}_LL4C3: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_IntKind,_tmp98F,1);goto _LL4B1;_LL4C4: {struct Cyc_Absyn_ValueofType_struct*
_tmp990=(struct Cyc_Absyn_ValueofType_struct*)_tmp96A;if(_tmp990->tag != 19)goto
_LL4C6;else{_tmp991=_tmp990->f1;}}_LL4C5: {struct _RegionHandle*_tmpA64=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_Tcenv_Tenv*_tmpA65=Cyc_Tcenv_allow_valueof(_tmpA64,te);Cyc_Tcexp_tcExp(
_tmpA65,0,_tmp991);}if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp991)){const char*
_tmp11F6;void*_tmp11F5;(_tmp11F5=0,Cyc_Tcutil_terr(loc,((_tmp11F6="valueof_t requires an int expression",
_tag_dyneither(_tmp11F6,sizeof(char),37))),_tag_dyneither(_tmp11F5,sizeof(void*),
0)));}cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp991,te,cvtenv);goto
_LL4B1;}_LL4C6: {struct Cyc_Absyn_IntType_struct*_tmp992=(struct Cyc_Absyn_IntType_struct*)
_tmp96A;if(_tmp992->tag != 6)goto _LL4C8;}_LL4C7: goto _LL4C9;_LL4C8: {struct Cyc_Absyn_FloatType_struct*
_tmp993=(struct Cyc_Absyn_FloatType_struct*)_tmp96A;if(_tmp993->tag != 7)goto
_LL4CA;}_LL4C9: goto _LL4CB;_LL4CA: {struct Cyc_Absyn_DoubleType_struct*_tmp994=(
struct Cyc_Absyn_DoubleType_struct*)_tmp96A;if(_tmp994->tag != 8)goto _LL4CC;}
_LL4CB: goto _LL4B1;_LL4CC: {struct Cyc_Absyn_ArrayType_struct*_tmp995=(struct Cyc_Absyn_ArrayType_struct*)
_tmp96A;if(_tmp995->tag != 9)goto _LL4CE;else{_tmp996=_tmp995->f1;_tmp997=_tmp996.elt_type;
_tmp998=_tmp996.tq;_tmp999=(struct Cyc_Absyn_Tqual*)&(_tmp995->f1).tq;_tmp99A=
_tmp996.num_elts;_tmp99B=(struct Cyc_Absyn_Exp**)&(_tmp995->f1).num_elts;_tmp99C=
_tmp996.zero_term;_tmp99D=_tmp996.zt_loc;}}_LL4CD: {struct Cyc_Absyn_Exp*_tmpA68=*
_tmp99B;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_MemKind,
_tmp997,1);_tmp999->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp999->print_const,
_tmp997);{int is_zero_terminated;{union Cyc_Absyn_Constraint*_tmpA69=((union Cyc_Absyn_Constraint*(*)(
union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp99C);union Cyc_Absyn_Constraint
_tmpA6A;int _tmpA6B;union Cyc_Absyn_Constraint _tmpA6C;int _tmpA6D;_LL52F: _tmpA6A=*
_tmpA69;if((_tmpA6A.No_constr).tag != 3)goto _LL531;_tmpA6B=(int)(_tmpA6A.No_constr).val;
_LL530:((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp99C,Cyc_Absyn_false_conref);
is_zero_terminated=0;goto _LL52E;_LL531: _tmpA6C=*_tmpA69;if((_tmpA6C.Eq_constr).tag
!= 1)goto _LL533;_tmpA6D=(int)(_tmpA6C.Eq_constr).val;if(_tmpA6D != 1)goto _LL533;
_LL532: if(!Cyc_Tcutil_admits_zero(_tmp997)){const char*_tmp11FA;void*_tmp11F9[1];
struct Cyc_String_pa_struct _tmp11F8;(_tmp11F8.tag=0,((_tmp11F8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp997)),((
_tmp11F9[0]=& _tmp11F8,Cyc_Tcutil_terr(loc,((_tmp11FA="cannot have a zero-terminated array of %s elements",
_tag_dyneither(_tmp11FA,sizeof(char),51))),_tag_dyneither(_tmp11F9,sizeof(void*),
1)))))));}is_zero_terminated=1;goto _LL52E;_LL533:;_LL534: is_zero_terminated=0;
goto _LL52E;_LL52E:;}if(_tmpA68 == 0){if(is_zero_terminated)*_tmp99B=(_tmpA68=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));else{{const char*_tmp11FD;void*
_tmp11FC;(_tmp11FC=0,Cyc_Tcutil_warn(loc,((_tmp11FD="array bound defaults to 1 here",
_tag_dyneither(_tmp11FD,sizeof(char),31))),_tag_dyneither(_tmp11FC,sizeof(void*),
0)));}*_tmp99B=(_tmpA68=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmpA68);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmpA68)){const char*_tmp1200;void*_tmp11FF;(_tmp11FF=0,Cyc_Tcutil_terr(loc,((
_tmp1200="array bounds expression is not constant",_tag_dyneither(_tmp1200,
sizeof(char),40))),_tag_dyneither(_tmp11FF,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_tmpA68)){const char*_tmp1203;void*_tmp1202;(_tmp1202=0,
Cyc_Tcutil_terr(loc,((_tmp1203="array bounds expression is not an unsigned int",
_tag_dyneither(_tmp1203,sizeof(char),47))),_tag_dyneither(_tmp1202,sizeof(void*),
0)));}{unsigned int _tmpA78;int _tmpA79;struct _tuple13 _tmpA77=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmpA68);_tmpA78=_tmpA77.f1;_tmpA79=_tmpA77.f2;if((
is_zero_terminated  && _tmpA79) && _tmpA78 < 1){const char*_tmp1206;void*_tmp1205;(
_tmp1205=0,Cyc_Tcutil_warn(loc,((_tmp1206="zero terminated array cannot have zero elements",
_tag_dyneither(_tmp1206,sizeof(char),48))),_tag_dyneither(_tmp1205,sizeof(void*),
0)));}if((_tmpA79  && _tmpA78 < 1) && Cyc_Cyclone_tovc_r){{const char*_tmp1209;void*
_tmp1208;(_tmp1208=0,Cyc_Tcutil_warn(loc,((_tmp1209="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dyneither(_tmp1209,sizeof(char),75))),_tag_dyneither(_tmp1208,sizeof(void*),
0)));}*_tmp99B=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0);}goto _LL4B1;};};}
_LL4CE: {struct Cyc_Absyn_FnType_struct*_tmp99E=(struct Cyc_Absyn_FnType_struct*)
_tmp96A;if(_tmp99E->tag != 10)goto _LL4D0;else{_tmp99F=_tmp99E->f1;_tmp9A0=_tmp99F.tvars;
_tmp9A1=(struct Cyc_List_List**)&(_tmp99E->f1).tvars;_tmp9A2=_tmp99F.effect;
_tmp9A3=(struct Cyc_Core_Opt**)&(_tmp99E->f1).effect;_tmp9A4=_tmp99F.ret_typ;
_tmp9A5=_tmp99F.args;_tmp9A6=_tmp99F.c_varargs;_tmp9A7=_tmp99F.cyc_varargs;
_tmp9A8=_tmp99F.rgn_po;_tmp9A9=_tmp99F.attributes;}}_LL4CF: {int num_convs=0;int
seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;enum Cyc_Absyn_Format_Type
ft=Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp9A9 != 0;
_tmp9A9=_tmp9A9->tl){if(!Cyc_Absyn_fntype_att((void*)_tmp9A9->hd)){const char*
_tmp120D;void*_tmp120C[1];struct Cyc_String_pa_struct _tmp120B;(_tmp120B.tag=0,((
_tmp120B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((
void*)_tmp9A9->hd)),((_tmp120C[0]=& _tmp120B,Cyc_Tcutil_terr(loc,((_tmp120D="bad function type attribute %s",
_tag_dyneither(_tmp120D,sizeof(char),31))),_tag_dyneither(_tmp120C,sizeof(void*),
1)))))));}{void*_tmpA81=(void*)_tmp9A9->hd;enum Cyc_Absyn_Format_Type _tmpA86;int
_tmpA87;int _tmpA88;_LL536: {struct Cyc_Absyn_Stdcall_att_struct*_tmpA82=(struct
Cyc_Absyn_Stdcall_att_struct*)_tmpA81;if(_tmpA82->tag != 1)goto _LL538;}_LL537: if(
!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL535;_LL538: {struct Cyc_Absyn_Cdecl_att_struct*
_tmpA83=(struct Cyc_Absyn_Cdecl_att_struct*)_tmpA81;if(_tmpA83->tag != 2)goto
_LL53A;}_LL539: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL535;_LL53A: {
struct Cyc_Absyn_Fastcall_att_struct*_tmpA84=(struct Cyc_Absyn_Fastcall_att_struct*)
_tmpA81;if(_tmpA84->tag != 3)goto _LL53C;}_LL53B: if(!seen_fastcall){seen_fastcall=
1;++ num_convs;}goto _LL535;_LL53C: {struct Cyc_Absyn_Format_att_struct*_tmpA85=(
struct Cyc_Absyn_Format_att_struct*)_tmpA81;if(_tmpA85->tag != 19)goto _LL53E;else{
_tmpA86=_tmpA85->f1;_tmpA87=_tmpA85->f2;_tmpA88=_tmpA85->f3;}}_LL53D: if(!
seen_format){seen_format=1;ft=_tmpA86;fmt_desc_arg=_tmpA87;fmt_arg_start=_tmpA88;}
else{const char*_tmp1210;void*_tmp120F;(_tmp120F=0,Cyc_Tcutil_terr(loc,((_tmp1210="function can't have multiple format attributes",
_tag_dyneither(_tmp1210,sizeof(char),47))),_tag_dyneither(_tmp120F,sizeof(void*),
0)));}goto _LL535;_LL53E:;_LL53F: goto _LL535;_LL535:;};}if(num_convs > 1){const char*
_tmp1213;void*_tmp1212;(_tmp1212=0,Cyc_Tcutil_terr(loc,((_tmp1213="function can't have multiple calling conventions",
_tag_dyneither(_tmp1213,sizeof(char),49))),_tag_dyneither(_tmp1212,sizeof(void*),
0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmp9A1);{struct Cyc_List_List*b=*
_tmp9A1;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)
b->hd);{void*_tmpA8D=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);
enum Cyc_Absyn_Kind _tmpA8F;_LL541: {struct Cyc_Absyn_Eq_kb_struct*_tmpA8E=(struct
Cyc_Absyn_Eq_kb_struct*)_tmpA8D;if(_tmpA8E->tag != 0)goto _LL543;else{_tmpA8F=
_tmpA8E->f1;if(_tmpA8F != Cyc_Absyn_MemKind)goto _LL543;}}_LL542:{const char*
_tmp1217;void*_tmp1216[1];struct Cyc_String_pa_struct _tmp1215;(_tmp1215.tag=0,((
_tmp1215.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name),((_tmp1216[0]=& _tmp1215,Cyc_Tcutil_terr(loc,((_tmp1217="function attempts to abstract Mem type variable %s",
_tag_dyneither(_tmp1217,sizeof(char),51))),_tag_dyneither(_tmp1216,sizeof(void*),
1)))))));}goto _LL540;_LL543:;_LL544: goto _LL540;_LL540:;};}}{struct _RegionHandle*
_tmpA93=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_Tcutil_CVTEnv _tmp1218;struct Cyc_Tcutil_CVTEnv
_tmpA94=(_tmp1218.r=_tmpA93,((_tmp1218.kind_env=cvtenv.kind_env,((_tmp1218.free_vars=
0,((_tmp1218.free_evars=0,((_tmp1218.generalize_evars=cvtenv.generalize_evars,((
_tmp1218.fn_result=1,_tmp1218)))))))))));_tmpA94=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA94,Cyc_Absyn_MemKind,_tmp9A4,1);_tmpA94.fn_result=0;{struct Cyc_List_List*
a=_tmp9A5;for(0;a != 0;a=a->tl){struct _tuple9*_tmpA95=(struct _tuple9*)a->hd;void*
_tmpA96=(*_tmpA95).f3;_tmpA94=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpA94,Cyc_Absyn_MemKind,
_tmpA96,1);((*_tmpA95).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,((*
_tmpA95).f2).print_const,_tmpA96);}}if(_tmp9A7 != 0){if(_tmp9A6){const char*
_tmp121B;void*_tmp121A;(_tmp121A=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp121B="both c_vararg and cyc_vararg",
_tag_dyneither(_tmp121B,sizeof(char),29))),_tag_dyneither(_tmp121A,sizeof(void*),
0)));}{void*_tmpA9A;int _tmpA9B;struct Cyc_Absyn_VarargInfo _tmpA99=*_tmp9A7;
_tmpA9A=_tmpA99.type;_tmpA9B=_tmpA99.inject;_tmpA94=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA94,Cyc_Absyn_MemKind,_tmpA9A,1);(_tmp9A7->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp9A7->tq).print_const,_tmpA9A);if(_tmpA9B){void*_tmpA9C=Cyc_Tcutil_compress(
_tmpA9A);struct Cyc_Absyn_PtrInfo _tmpA9E;void*_tmpA9F;struct Cyc_Absyn_PtrAtts
_tmpAA0;union Cyc_Absyn_Constraint*_tmpAA1;union Cyc_Absyn_Constraint*_tmpAA2;
_LL546: {struct Cyc_Absyn_PointerType_struct*_tmpA9D=(struct Cyc_Absyn_PointerType_struct*)
_tmpA9C;if(_tmpA9D->tag != 5)goto _LL548;else{_tmpA9E=_tmpA9D->f1;_tmpA9F=_tmpA9E.elt_typ;
_tmpAA0=_tmpA9E.ptr_atts;_tmpAA1=_tmpAA0.bounds;_tmpAA2=_tmpAA0.zero_term;}}
_LL547:{void*_tmpAA3=Cyc_Tcutil_compress(_tmpA9F);_LL54B: {struct Cyc_Absyn_DatatypeType_struct*
_tmpAA4=(struct Cyc_Absyn_DatatypeType_struct*)_tmpAA3;if(_tmpAA4->tag != 3)goto
_LL54D;}_LL54C: if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(
0,_tmpAA2)){const char*_tmp121E;void*_tmp121D;(_tmp121D=0,Cyc_Tcutil_terr(loc,((
_tmp121E="can't inject into a zeroterm pointer",_tag_dyneither(_tmp121E,sizeof(
char),37))),_tag_dyneither(_tmp121D,sizeof(void*),0)));}{void*_tmpAA7=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,
_tmpAA1);_LL550: {struct Cyc_Absyn_DynEither_b_struct*_tmpAA8=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpAA7;if(_tmpAA8->tag != 0)goto _LL552;}_LL551:{const char*_tmp1221;void*_tmp1220;(
_tmp1220=0,Cyc_Tcutil_terr(loc,((_tmp1221="can't inject into a fat pointer to datatype",
_tag_dyneither(_tmp1221,sizeof(char),44))),_tag_dyneither(_tmp1220,sizeof(void*),
0)));}goto _LL54F;_LL552:;_LL553: goto _LL54F;_LL54F:;}goto _LL54A;_LL54D:;_LL54E:{
const char*_tmp1224;void*_tmp1223;(_tmp1223=0,Cyc_Tcutil_terr(loc,((_tmp1224="can't inject a non-datatype type",
_tag_dyneither(_tmp1224,sizeof(char),33))),_tag_dyneither(_tmp1223,sizeof(void*),
0)));}goto _LL54A;_LL54A:;}goto _LL545;_LL548:;_LL549:{const char*_tmp1227;void*
_tmp1226;(_tmp1226=0,Cyc_Tcutil_terr(loc,((_tmp1227="expecting a datatype pointer type",
_tag_dyneither(_tmp1227,sizeof(char),34))),_tag_dyneither(_tmp1226,sizeof(void*),
0)));}goto _LL545;_LL545:;}};}if(seen_format){int _tmpAAF=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp9A5);if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpAAF) || 
fmt_arg_start < 0) || _tmp9A7 == 0  && fmt_arg_start != 0) || _tmp9A7 != 0  && 
fmt_arg_start != _tmpAAF + 1){const char*_tmp122A;void*_tmp1229;(_tmp1229=0,Cyc_Tcutil_terr(
loc,((_tmp122A="bad format descriptor",_tag_dyneither(_tmp122A,sizeof(char),22))),
_tag_dyneither(_tmp1229,sizeof(void*),0)));}else{void*_tmpAB3;struct _tuple9
_tmpAB2=*((struct _tuple9*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp9A5,
fmt_desc_arg - 1);_tmpAB3=_tmpAB2.f3;{void*_tmpAB4=Cyc_Tcutil_compress(_tmpAB3);
struct Cyc_Absyn_PtrInfo _tmpAB6;void*_tmpAB7;struct Cyc_Absyn_PtrAtts _tmpAB8;union
Cyc_Absyn_Constraint*_tmpAB9;union Cyc_Absyn_Constraint*_tmpABA;_LL555: {struct
Cyc_Absyn_PointerType_struct*_tmpAB5=(struct Cyc_Absyn_PointerType_struct*)
_tmpAB4;if(_tmpAB5->tag != 5)goto _LL557;else{_tmpAB6=_tmpAB5->f1;_tmpAB7=_tmpAB6.elt_typ;
_tmpAB8=_tmpAB6.ptr_atts;_tmpAB9=_tmpAB8.bounds;_tmpABA=_tmpAB8.zero_term;}}
_LL556:{struct _tuple0 _tmp122B;struct _tuple0 _tmpABC=(_tmp122B.f1=Cyc_Tcutil_compress(
_tmpAB7),((_tmp122B.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,_tmpAB9),_tmp122B)));void*_tmpABD;enum Cyc_Absyn_Sign
_tmpABF;enum Cyc_Absyn_Size_of _tmpAC0;void*_tmpAC1;_LL55A: _tmpABD=_tmpABC.f1;{
struct Cyc_Absyn_IntType_struct*_tmpABE=(struct Cyc_Absyn_IntType_struct*)_tmpABD;
if(_tmpABE->tag != 6)goto _LL55C;else{_tmpABF=_tmpABE->f1;if(_tmpABF != Cyc_Absyn_None)
goto _LL55C;_tmpAC0=_tmpABE->f2;if(_tmpAC0 != Cyc_Absyn_Char_sz)goto _LL55C;}};
_tmpAC1=_tmpABC.f2;{struct Cyc_Absyn_DynEither_b_struct*_tmpAC2=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpAC1;if(_tmpAC2->tag != 0)goto _LL55C;};_LL55B: goto _LL559;_LL55C:;_LL55D:{const
char*_tmp122E;void*_tmp122D;(_tmp122D=0,Cyc_Tcutil_terr(loc,((_tmp122E="format descriptor is not a char ? type",
_tag_dyneither(_tmp122E,sizeof(char),39))),_tag_dyneither(_tmp122D,sizeof(void*),
0)));}goto _LL559;_LL559:;}goto _LL554;_LL557:;_LL558:{const char*_tmp1231;void*
_tmp1230;(_tmp1230=0,Cyc_Tcutil_terr(loc,((_tmp1231="format descriptor is not a char ? type",
_tag_dyneither(_tmp1231,sizeof(char),39))),_tag_dyneither(_tmp1230,sizeof(void*),
0)));}goto _LL554;_LL554:;}if(fmt_arg_start != 0){int problem;{struct _tuple26
_tmp1232;struct _tuple26 _tmpAC8=(_tmp1232.f1=ft,((_tmp1232.f2=Cyc_Tcutil_compress(
Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp9A7))->type)),
_tmp1232)));enum Cyc_Absyn_Format_Type _tmpAC9;void*_tmpACA;struct Cyc_Absyn_DatatypeInfo
_tmpACC;union Cyc_Absyn_DatatypeInfoU _tmpACD;struct Cyc_Absyn_Datatypedecl**
_tmpACE;struct Cyc_Absyn_Datatypedecl*_tmpACF;enum Cyc_Absyn_Format_Type _tmpAD0;
void*_tmpAD1;struct Cyc_Absyn_DatatypeInfo _tmpAD3;union Cyc_Absyn_DatatypeInfoU
_tmpAD4;struct Cyc_Absyn_Datatypedecl**_tmpAD5;struct Cyc_Absyn_Datatypedecl*
_tmpAD6;_LL55F: _tmpAC9=_tmpAC8.f1;if(_tmpAC9 != Cyc_Absyn_Printf_ft)goto _LL561;
_tmpACA=_tmpAC8.f2;{struct Cyc_Absyn_DatatypeType_struct*_tmpACB=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpACA;if(_tmpACB->tag != 3)goto _LL561;else{_tmpACC=_tmpACB->f1;_tmpACD=_tmpACC.datatype_info;
if((_tmpACD.KnownDatatype).tag != 2)goto _LL561;_tmpACE=(struct Cyc_Absyn_Datatypedecl**)(
_tmpACD.KnownDatatype).val;_tmpACF=*_tmpACE;}};_LL560: problem=Cyc_Absyn_qvar_cmp(
_tmpACF->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL55E;_LL561: _tmpAD0=
_tmpAC8.f1;if(_tmpAD0 != Cyc_Absyn_Scanf_ft)goto _LL563;_tmpAD1=_tmpAC8.f2;{struct
Cyc_Absyn_DatatypeType_struct*_tmpAD2=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpAD1;if(_tmpAD2->tag != 3)goto _LL563;else{_tmpAD3=_tmpAD2->f1;_tmpAD4=_tmpAD3.datatype_info;
if((_tmpAD4.KnownDatatype).tag != 2)goto _LL563;_tmpAD5=(struct Cyc_Absyn_Datatypedecl**)(
_tmpAD4.KnownDatatype).val;_tmpAD6=*_tmpAD5;}};_LL562: problem=Cyc_Absyn_qvar_cmp(
_tmpAD6->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL55E;_LL563:;_LL564:
problem=1;goto _LL55E;_LL55E:;}if(problem){const char*_tmp1235;void*_tmp1234;(
_tmp1234=0,Cyc_Tcutil_terr(loc,((_tmp1235="format attribute and vararg types don't match",
_tag_dyneither(_tmp1235,sizeof(char),46))),_tag_dyneither(_tmp1234,sizeof(void*),
0)));}}}}{struct Cyc_List_List*rpo=_tmp9A8;for(0;rpo != 0;rpo=rpo->tl){struct
_tuple0 _tmpADA;void*_tmpADB;void*_tmpADC;struct _tuple0*_tmpAD9=(struct _tuple0*)
rpo->hd;_tmpADA=*_tmpAD9;_tmpADB=_tmpADA.f1;_tmpADC=_tmpADA.f2;_tmpA94=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA94,Cyc_Absyn_EffKind,_tmpADB,1);_tmpA94=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA94,Cyc_Absyn_TopRgnKind,_tmpADC,1);}}if(*_tmp9A3 != 0)_tmpA94=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpA94,Cyc_Absyn_EffKind,(void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp9A3))->v,1);else{struct Cyc_List_List*effect=0;{struct Cyc_List_List*tvs=
_tmpA94.free_vars;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpADE;int
_tmpADF;struct _tuple24 _tmpADD=*((struct _tuple24*)tvs->hd);_tmpADE=_tmpADD.f1;
_tmpADF=_tmpADD.f2;if(!_tmpADF)continue;{void*_tmpAE0=Cyc_Absyn_compress_kb(
_tmpADE->kind);struct Cyc_Core_Opt*_tmpAE2;struct Cyc_Core_Opt**_tmpAE3;enum Cyc_Absyn_Kind
_tmpAE4;struct Cyc_Core_Opt*_tmpAE6;struct Cyc_Core_Opt**_tmpAE7;enum Cyc_Absyn_Kind
_tmpAE8;enum Cyc_Absyn_Kind _tmpAEA;enum Cyc_Absyn_Kind _tmpAEC;enum Cyc_Absyn_Kind
_tmpAEE;struct Cyc_Core_Opt*_tmpAF0;struct Cyc_Core_Opt**_tmpAF1;enum Cyc_Absyn_Kind
_tmpAF2;enum Cyc_Absyn_Kind _tmpAF4;struct Cyc_Core_Opt*_tmpAF6;struct Cyc_Core_Opt**
_tmpAF7;_LL566: {struct Cyc_Absyn_Less_kb_struct*_tmpAE1=(struct Cyc_Absyn_Less_kb_struct*)
_tmpAE0;if(_tmpAE1->tag != 2)goto _LL568;else{_tmpAE2=_tmpAE1->f1;_tmpAE3=(struct
Cyc_Core_Opt**)& _tmpAE1->f1;_tmpAE4=_tmpAE1->f2;if(_tmpAE4 != Cyc_Absyn_TopRgnKind)
goto _LL568;}}_LL567: _tmpAE7=_tmpAE3;_tmpAE8=Cyc_Absyn_RgnKind;goto _LL569;_LL568:{
struct Cyc_Absyn_Less_kb_struct*_tmpAE5=(struct Cyc_Absyn_Less_kb_struct*)_tmpAE0;
if(_tmpAE5->tag != 2)goto _LL56A;else{_tmpAE6=_tmpAE5->f1;_tmpAE7=(struct Cyc_Core_Opt**)&
_tmpAE5->f1;_tmpAE8=_tmpAE5->f2;}}if(!(_tmpAE8 == Cyc_Absyn_RgnKind  || _tmpAE8 == 
Cyc_Absyn_UniqueRgnKind))goto _LL56A;_LL569:*_tmpAE7=Cyc_Tcutil_kind_to_bound_opt(
_tmpAE8);_tmpAEA=_tmpAE8;goto _LL56B;_LL56A:{struct Cyc_Absyn_Eq_kb_struct*_tmpAE9=(
struct Cyc_Absyn_Eq_kb_struct*)_tmpAE0;if(_tmpAE9->tag != 0)goto _LL56C;else{
_tmpAEA=_tmpAE9->f1;}}if(!((_tmpAEA == Cyc_Absyn_RgnKind  || _tmpAEA == Cyc_Absyn_UniqueRgnKind)
 || _tmpAEA == Cyc_Absyn_TopRgnKind))goto _LL56C;_LL56B:{struct Cyc_Absyn_AccessEff_struct*
_tmp1244;struct Cyc_Absyn_VarType_struct*_tmp1243;struct Cyc_Absyn_VarType_struct
_tmp1242;struct Cyc_Absyn_AccessEff_struct _tmp1241;struct Cyc_List_List*_tmp1240;
effect=((_tmp1240=_cycalloc(sizeof(*_tmp1240)),((_tmp1240->hd=(void*)((_tmp1244=
_cycalloc(sizeof(*_tmp1244)),((_tmp1244[0]=((_tmp1241.tag=23,((_tmp1241.f1=(void*)((
void*)((_tmp1243=_cycalloc(sizeof(*_tmp1243)),((_tmp1243[0]=((_tmp1242.tag=2,((
_tmp1242.f1=_tmpADE,_tmp1242)))),_tmp1243))))),_tmp1241)))),_tmp1244)))),((
_tmp1240->tl=effect,_tmp1240))))));}goto _LL565;_LL56C: {struct Cyc_Absyn_Less_kb_struct*
_tmpAEB=(struct Cyc_Absyn_Less_kb_struct*)_tmpAE0;if(_tmpAEB->tag != 2)goto _LL56E;
else{_tmpAEC=_tmpAEB->f2;if(_tmpAEC != Cyc_Absyn_IntKind)goto _LL56E;}}_LL56D: goto
_LL56F;_LL56E: {struct Cyc_Absyn_Eq_kb_struct*_tmpAED=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpAE0;if(_tmpAED->tag != 0)goto _LL570;else{_tmpAEE=_tmpAED->f1;if(_tmpAEE != Cyc_Absyn_IntKind)
goto _LL570;}}_LL56F: goto _LL565;_LL570: {struct Cyc_Absyn_Less_kb_struct*_tmpAEF=(
struct Cyc_Absyn_Less_kb_struct*)_tmpAE0;if(_tmpAEF->tag != 2)goto _LL572;else{
_tmpAF0=_tmpAEF->f1;_tmpAF1=(struct Cyc_Core_Opt**)& _tmpAEF->f1;_tmpAF2=_tmpAEF->f2;
if(_tmpAF2 != Cyc_Absyn_EffKind)goto _LL572;}}_LL571:*_tmpAF1=Cyc_Tcutil_kind_to_bound_opt(
Cyc_Absyn_EffKind);goto _LL573;_LL572: {struct Cyc_Absyn_Eq_kb_struct*_tmpAF3=(
struct Cyc_Absyn_Eq_kb_struct*)_tmpAE0;if(_tmpAF3->tag != 0)goto _LL574;else{
_tmpAF4=_tmpAF3->f1;if(_tmpAF4 != Cyc_Absyn_EffKind)goto _LL574;}}_LL573:{struct
Cyc_Absyn_VarType_struct*_tmp124A;struct Cyc_Absyn_VarType_struct _tmp1249;struct
Cyc_List_List*_tmp1248;effect=((_tmp1248=_cycalloc(sizeof(*_tmp1248)),((_tmp1248->hd=(
void*)((_tmp124A=_cycalloc(sizeof(*_tmp124A)),((_tmp124A[0]=((_tmp1249.tag=2,((
_tmp1249.f1=_tmpADE,_tmp1249)))),_tmp124A)))),((_tmp1248->tl=effect,_tmp1248))))));}
goto _LL565;_LL574: {struct Cyc_Absyn_Unknown_kb_struct*_tmpAF5=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpAE0;if(_tmpAF5->tag != 1)goto _LL576;else{_tmpAF6=_tmpAF5->f1;_tmpAF7=(struct
Cyc_Core_Opt**)& _tmpAF5->f1;}}_LL575:{struct Cyc_Absyn_Less_kb_struct*_tmp1250;
struct Cyc_Absyn_Less_kb_struct _tmp124F;struct Cyc_Core_Opt*_tmp124E;*_tmpAF7=((
_tmp124E=_cycalloc(sizeof(*_tmp124E)),((_tmp124E->v=(void*)((_tmp1250=_cycalloc(
sizeof(*_tmp1250)),((_tmp1250[0]=((_tmp124F.tag=2,((_tmp124F.f1=0,((_tmp124F.f2=
Cyc_Absyn_AnyKind,_tmp124F)))))),_tmp1250)))),_tmp124E))));}goto _LL577;_LL576:;
_LL577:{struct Cyc_Absyn_RgnsEff_struct*_tmp125F;struct Cyc_Absyn_VarType_struct*
_tmp125E;struct Cyc_Absyn_VarType_struct _tmp125D;struct Cyc_Absyn_RgnsEff_struct
_tmp125C;struct Cyc_List_List*_tmp125B;effect=((_tmp125B=_cycalloc(sizeof(*
_tmp125B)),((_tmp125B->hd=(void*)((_tmp125F=_cycalloc(sizeof(*_tmp125F)),((
_tmp125F[0]=((_tmp125C.tag=25,((_tmp125C.f1=(void*)((void*)((_tmp125E=_cycalloc(
sizeof(*_tmp125E)),((_tmp125E[0]=((_tmp125D.tag=2,((_tmp125D.f1=_tmpADE,_tmp125D)))),
_tmp125E))))),_tmp125C)))),_tmp125F)))),((_tmp125B->tl=effect,_tmp125B))))));}
goto _LL565;_LL565:;};}}{struct Cyc_List_List*ts=_tmpA94.free_evars;for(0;ts != 0;
ts=ts->tl){void*_tmpB09;int _tmpB0A;struct _tuple25 _tmpB08=*((struct _tuple25*)ts->hd);
_tmpB09=_tmpB08.f1;_tmpB0A=_tmpB08.f2;if(!_tmpB0A)continue;switch(Cyc_Tcutil_typ_kind(
_tmpB09)){case Cyc_Absyn_TopRgnKind: _LL578: goto _LL579;case Cyc_Absyn_UniqueRgnKind:
_LL579: goto _LL57A;case Cyc_Absyn_RgnKind: _LL57A:{struct Cyc_Absyn_AccessEff_struct*
_tmp1265;struct Cyc_Absyn_AccessEff_struct _tmp1264;struct Cyc_List_List*_tmp1263;
effect=((_tmp1263=_cycalloc(sizeof(*_tmp1263)),((_tmp1263->hd=(void*)((_tmp1265=
_cycalloc(sizeof(*_tmp1265)),((_tmp1265[0]=((_tmp1264.tag=23,((_tmp1264.f1=(void*)
_tmpB09,_tmp1264)))),_tmp1265)))),((_tmp1263->tl=effect,_tmp1263))))));}break;
case Cyc_Absyn_EffKind: _LL57B:{struct Cyc_List_List*_tmp1266;effect=((_tmp1266=
_cycalloc(sizeof(*_tmp1266)),((_tmp1266->hd=_tmpB09,((_tmp1266->tl=effect,
_tmp1266))))));}break;case Cyc_Absyn_IntKind: _LL57C: break;default: _LL57D:{struct
Cyc_Absyn_RgnsEff_struct*_tmp126C;struct Cyc_Absyn_RgnsEff_struct _tmp126B;struct
Cyc_List_List*_tmp126A;effect=((_tmp126A=_cycalloc(sizeof(*_tmp126A)),((_tmp126A->hd=(
void*)((_tmp126C=_cycalloc(sizeof(*_tmp126C)),((_tmp126C[0]=((_tmp126B.tag=25,((
_tmp126B.f1=(void*)_tmpB09,_tmp126B)))),_tmp126C)))),((_tmp126A->tl=effect,
_tmp126A))))));}break;}}}{struct Cyc_Absyn_JoinEff_struct*_tmp1272;struct Cyc_Absyn_JoinEff_struct
_tmp1271;struct Cyc_Core_Opt*_tmp1270;*_tmp9A3=((_tmp1270=_cycalloc(sizeof(*
_tmp1270)),((_tmp1270->v=(void*)((_tmp1272=_cycalloc(sizeof(*_tmp1272)),((
_tmp1272[0]=((_tmp1271.tag=24,((_tmp1271.f1=effect,_tmp1271)))),_tmp1272)))),
_tmp1270))));};}if(*_tmp9A1 != 0){struct Cyc_List_List*bs=*_tmp9A1;for(0;bs != 0;bs=
bs->tl){void*_tmpB15=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmpB17;struct Cyc_Core_Opt**_tmpB18;struct Cyc_Core_Opt*
_tmpB1A;struct Cyc_Core_Opt**_tmpB1B;enum Cyc_Absyn_Kind _tmpB1C;struct Cyc_Core_Opt*
_tmpB1E;struct Cyc_Core_Opt**_tmpB1F;enum Cyc_Absyn_Kind _tmpB20;struct Cyc_Core_Opt*
_tmpB22;struct Cyc_Core_Opt**_tmpB23;enum Cyc_Absyn_Kind _tmpB24;struct Cyc_Core_Opt*
_tmpB26;struct Cyc_Core_Opt**_tmpB27;enum Cyc_Absyn_Kind _tmpB28;enum Cyc_Absyn_Kind
_tmpB2A;_LL580: {struct Cyc_Absyn_Unknown_kb_struct*_tmpB16=(struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB15;if(_tmpB16->tag != 1)goto _LL582;else{_tmpB17=_tmpB16->f1;_tmpB18=(struct
Cyc_Core_Opt**)& _tmpB16->f1;}}_LL581:{const char*_tmp1276;void*_tmp1275[1];struct
Cyc_String_pa_struct _tmp1274;(_tmp1274.tag=0,((_tmp1274.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1275[0]=&
_tmp1274,Cyc_Tcutil_warn(loc,((_tmp1276="Type variable %s unconstrained, assuming boxed",
_tag_dyneither(_tmp1276,sizeof(char),47))),_tag_dyneither(_tmp1275,sizeof(void*),
1)))))));}_tmpB1B=_tmpB18;goto _LL583;_LL582: {struct Cyc_Absyn_Less_kb_struct*
_tmpB19=(struct Cyc_Absyn_Less_kb_struct*)_tmpB15;if(_tmpB19->tag != 2)goto _LL584;
else{_tmpB1A=_tmpB19->f1;_tmpB1B=(struct Cyc_Core_Opt**)& _tmpB19->f1;_tmpB1C=
_tmpB19->f2;if(_tmpB1C != Cyc_Absyn_MemKind)goto _LL584;}}_LL583: _tmpB1F=_tmpB1B;
goto _LL585;_LL584: {struct Cyc_Absyn_Less_kb_struct*_tmpB1D=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB15;if(_tmpB1D->tag != 2)goto _LL586;else{_tmpB1E=_tmpB1D->f1;_tmpB1F=(struct
Cyc_Core_Opt**)& _tmpB1D->f1;_tmpB20=_tmpB1D->f2;if(_tmpB20 != Cyc_Absyn_AnyKind)
goto _LL586;}}_LL585:*_tmpB1F=Cyc_Tcutil_kind_to_bound_opt(Cyc_Absyn_BoxKind);
goto _LL57F;_LL586: {struct Cyc_Absyn_Less_kb_struct*_tmpB21=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB15;if(_tmpB21->tag != 2)goto _LL588;else{_tmpB22=_tmpB21->f1;_tmpB23=(struct
Cyc_Core_Opt**)& _tmpB21->f1;_tmpB24=_tmpB21->f2;if(_tmpB24 != Cyc_Absyn_TopRgnKind)
goto _LL588;}}_LL587:*_tmpB23=Cyc_Tcutil_kind_to_bound_opt(Cyc_Absyn_RgnKind);
goto _LL57F;_LL588: {struct Cyc_Absyn_Less_kb_struct*_tmpB25=(struct Cyc_Absyn_Less_kb_struct*)
_tmpB15;if(_tmpB25->tag != 2)goto _LL58A;else{_tmpB26=_tmpB25->f1;_tmpB27=(struct
Cyc_Core_Opt**)& _tmpB25->f1;_tmpB28=_tmpB25->f2;}}_LL589:*_tmpB27=Cyc_Tcutil_kind_to_bound_opt(
_tmpB28);goto _LL57F;_LL58A: {struct Cyc_Absyn_Eq_kb_struct*_tmpB29=(struct Cyc_Absyn_Eq_kb_struct*)
_tmpB15;if(_tmpB29->tag != 0)goto _LL58C;else{_tmpB2A=_tmpB29->f1;if(_tmpB2A != Cyc_Absyn_MemKind)
goto _LL58C;}}_LL58B:{const char*_tmp1279;void*_tmp1278;(_tmp1278=0,Cyc_Tcutil_terr(
loc,((_tmp1279="functions can't abstract M types",_tag_dyneither(_tmp1279,
sizeof(char),33))),_tag_dyneither(_tmp1278,sizeof(void*),0)));}goto _LL57F;_LL58C:;
_LL58D: goto _LL57F;_LL57F:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,
_tmpA94.kind_env,*_tmp9A1);_tmpA94.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmpA94.r,_tmpA94.free_vars,*_tmp9A1);{struct Cyc_List_List*tvs=_tmpA94.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmpB31;int _tmpB32;struct
_tuple24 _tmpB30=*((struct _tuple24*)tvs->hd);_tmpB31=_tmpB30.f1;_tmpB32=_tmpB30.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmpB31,_tmpB32);}}{struct Cyc_List_List*evs=_tmpA94.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmpB34;int _tmpB35;struct _tuple25 _tmpB33=*((struct _tuple25*)evs->hd);
_tmpB34=_tmpB33.f1;_tmpB35=_tmpB33.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmpB34,_tmpB35);}};}goto _LL4B1;};}_LL4D0: {struct Cyc_Absyn_TupleType_struct*
_tmp9AA=(struct Cyc_Absyn_TupleType_struct*)_tmp96A;if(_tmp9AA->tag != 11)goto
_LL4D2;else{_tmp9AB=_tmp9AA->f1;}}_LL4D1: for(0;_tmp9AB != 0;_tmp9AB=_tmp9AB->tl){
struct _tuple11*_tmpB37=(struct _tuple11*)_tmp9AB->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_MemKind,(*_tmpB37).f2,1);((*_tmpB37).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmpB37).f1).print_const,(*_tmpB37).f2);}goto _LL4B1;_LL4D2: {struct Cyc_Absyn_AnonAggrType_struct*
_tmp9AC=(struct Cyc_Absyn_AnonAggrType_struct*)_tmp96A;if(_tmp9AC->tag != 13)goto
_LL4D4;else{_tmp9AD=_tmp9AC->f1;_tmp9AE=_tmp9AC->f2;}}_LL4D3: {struct
_RegionHandle*_tmpB38=Cyc_Tcenv_get_fnrgn(te);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp9AE != 0;_tmp9AE=_tmp9AE->tl){struct Cyc_Absyn_Aggrfield _tmpB3A;struct
_dyneither_ptr*_tmpB3B;struct Cyc_Absyn_Tqual _tmpB3C;struct Cyc_Absyn_Tqual*
_tmpB3D;void*_tmpB3E;struct Cyc_Absyn_Exp*_tmpB3F;struct Cyc_List_List*_tmpB40;
struct Cyc_Absyn_Aggrfield*_tmpB39=(struct Cyc_Absyn_Aggrfield*)_tmp9AE->hd;
_tmpB3A=*_tmpB39;_tmpB3B=_tmpB3A.name;_tmpB3C=_tmpB3A.tq;_tmpB3D=(struct Cyc_Absyn_Tqual*)&(*
_tmpB39).tq;_tmpB3E=_tmpB3A.type;_tmpB3F=_tmpB3A.width;_tmpB40=_tmpB3A.attributes;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpB3B)){
const char*_tmp127D;void*_tmp127C[1];struct Cyc_String_pa_struct _tmp127B;(_tmp127B.tag=
0,((_tmp127B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB3B),((
_tmp127C[0]=& _tmp127B,Cyc_Tcutil_terr(loc,((_tmp127D="duplicate field %s",
_tag_dyneither(_tmp127D,sizeof(char),19))),_tag_dyneither(_tmp127C,sizeof(void*),
1)))))));}{const char*_tmp127E;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpB3B,((
_tmp127E="",_tag_dyneither(_tmp127E,sizeof(char),1))))!= 0){struct Cyc_List_List*
_tmp127F;prev_fields=((_tmp127F=_region_malloc(_tmpB38,sizeof(*_tmp127F)),((
_tmp127F->hd=_tmpB3B,((_tmp127F->tl=prev_fields,_tmp127F))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,Cyc_Absyn_MemKind,_tmpB3E,1);_tmpB3D->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmpB3D->print_const,_tmpB3E);if(_tmp9AD == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(
_tmpB3E)){const char*_tmp1283;void*_tmp1282[1];struct Cyc_String_pa_struct _tmp1281;(
_tmp1281.tag=0,((_tmp1281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmpB3B),((_tmp1282[0]=& _tmp1281,Cyc_Tcutil_warn(loc,((_tmp1283="union member %s is not `bits-only' so it can only be written and not read",
_tag_dyneither(_tmp1283,sizeof(char),74))),_tag_dyneither(_tmp1282,sizeof(void*),
1)))))));}Cyc_Tcutil_check_bitfield(loc,te,_tmpB3E,_tmpB3F,_tmpB3B);Cyc_Tcutil_check_field_atts(
loc,_tmpB3B,_tmpB40);}}goto _LL4B1;}_LL4D4: {struct Cyc_Absyn_AggrType_struct*
_tmp9AF=(struct Cyc_Absyn_AggrType_struct*)_tmp96A;if(_tmp9AF->tag != 12)goto
_LL4D6;else{_tmp9B0=_tmp9AF->f1;_tmp9B1=_tmp9B0.aggr_info;_tmp9B2=(union Cyc_Absyn_AggrInfoU*)&(
_tmp9AF->f1).aggr_info;_tmp9B3=_tmp9B0.targs;_tmp9B4=(struct Cyc_List_List**)&(
_tmp9AF->f1).targs;}}_LL4D5:{union Cyc_Absyn_AggrInfoU _tmpB49=*_tmp9B2;struct
_tuple4 _tmpB4A;enum Cyc_Absyn_AggrKind _tmpB4B;struct _tuple2*_tmpB4C;struct Cyc_Core_Opt*
_tmpB4D;struct Cyc_Absyn_Aggrdecl**_tmpB4E;struct Cyc_Absyn_Aggrdecl*_tmpB4F;
_LL58F: if((_tmpB49.UnknownAggr).tag != 1)goto _LL591;_tmpB4A=(struct _tuple4)(
_tmpB49.UnknownAggr).val;_tmpB4B=_tmpB4A.f1;_tmpB4C=_tmpB4A.f2;_tmpB4D=_tmpB4A.f3;
_LL590: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmpB50;_push_handler(&
_tmpB50);{int _tmpB52=0;if(setjmp(_tmpB50.handler))_tmpB52=1;if(!_tmpB52){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpB4C);{struct Cyc_Absyn_Aggrdecl*_tmpB53=*adp;if(_tmpB53->kind != 
_tmpB4B){if(_tmpB53->kind == Cyc_Absyn_StructA){const char*_tmp1288;void*_tmp1287[
2];struct Cyc_String_pa_struct _tmp1286;struct Cyc_String_pa_struct _tmp1285;(
_tmp1285.tag=0,((_tmp1285.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4C)),((_tmp1286.tag=0,((_tmp1286.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4C)),((_tmp1287[0]=& _tmp1286,((
_tmp1287[1]=& _tmp1285,Cyc_Tcutil_terr(loc,((_tmp1288="expecting struct %s instead of union %s",
_tag_dyneither(_tmp1288,sizeof(char),40))),_tag_dyneither(_tmp1287,sizeof(void*),
2)))))))))))));}else{const char*_tmp128D;void*_tmp128C[2];struct Cyc_String_pa_struct
_tmp128B;struct Cyc_String_pa_struct _tmp128A;(_tmp128A.tag=0,((_tmp128A.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB4C)),((
_tmp128B.tag=0,((_tmp128B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4C)),((_tmp128C[0]=& _tmp128B,((_tmp128C[1]=& _tmp128A,Cyc_Tcutil_terr(loc,((
_tmp128D="expecting union %s instead of struct %s",_tag_dyneither(_tmp128D,
sizeof(char),40))),_tag_dyneither(_tmp128C,sizeof(void*),2)))))))))))));}}if((
unsigned int)_tmpB4D  && (int)_tmpB4D->v){if(!((unsigned int)_tmpB53->impl) || !((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB53->impl))->tagged){const char*
_tmp1291;void*_tmp1290[1];struct Cyc_String_pa_struct _tmp128F;(_tmp128F.tag=0,((
_tmp128F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4C)),((_tmp1290[0]=& _tmp128F,Cyc_Tcutil_terr(loc,((_tmp1291="@tagged qualfiers don't agree on union %s",
_tag_dyneither(_tmp1291,sizeof(char),42))),_tag_dyneither(_tmp1290,sizeof(void*),
1)))))));}}*_tmp9B2=Cyc_Absyn_KnownAggr(adp);};;_pop_handler();}else{void*
_tmpB51=(void*)_exn_thrown;void*_tmpB60=_tmpB51;_LL594: {struct Cyc_Dict_Absent_struct*
_tmpB61=(struct Cyc_Dict_Absent_struct*)_tmpB60;if(_tmpB61->tag != Cyc_Dict_Absent)
goto _LL596;}_LL595: {struct Cyc_Tcenv_Genv*_tmpB62=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp1292;struct Cyc_Absyn_Aggrdecl*_tmpB63=(_tmp1292=
_cycalloc(sizeof(*_tmp1292)),((_tmp1292->kind=_tmpB4B,((_tmp1292->sc=Cyc_Absyn_Extern,((
_tmp1292->name=_tmpB4C,((_tmp1292->tvs=0,((_tmp1292->impl=0,((_tmp1292->attributes=
0,_tmp1292)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmpB62,loc,_tmpB63);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmpB4C);*_tmp9B2=Cyc_Absyn_KnownAggr(adp);if(*_tmp9B4 != 0){{const char*
_tmp1296;void*_tmp1295[1];struct Cyc_String_pa_struct _tmp1294;(_tmp1294.tag=0,((
_tmp1294.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4C)),((_tmp1295[0]=& _tmp1294,Cyc_Tcutil_terr(loc,((_tmp1296="declare parameterized type %s before using",
_tag_dyneither(_tmp1296,sizeof(char),43))),_tag_dyneither(_tmp1295,sizeof(void*),
1)))))));}return cvtenv;}goto _LL593;}_LL596:;_LL597:(void)_throw(_tmpB60);_LL593:;}};}
_tmpB4F=*adp;goto _LL592;}_LL591: if((_tmpB49.KnownAggr).tag != 2)goto _LL58E;
_tmpB4E=(struct Cyc_Absyn_Aggrdecl**)(_tmpB49.KnownAggr).val;_tmpB4F=*_tmpB4E;
_LL592: {struct Cyc_List_List*tvs=_tmpB4F->tvs;struct Cyc_List_List*ts=*_tmp9B4;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){struct Cyc_Absyn_Tvar*_tmpB68=(
struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmpB69=(void*)ts->hd;{struct _tuple0 _tmp1297;
struct _tuple0 _tmpB6B=(_tmp1297.f1=Cyc_Absyn_compress_kb(_tmpB68->kind),((
_tmp1297.f2=_tmpB69,_tmp1297)));void*_tmpB6C;void*_tmpB6E;struct Cyc_Absyn_Tvar*
_tmpB70;_LL599: _tmpB6C=_tmpB6B.f1;{struct Cyc_Absyn_Unknown_kb_struct*_tmpB6D=(
struct Cyc_Absyn_Unknown_kb_struct*)_tmpB6C;if(_tmpB6D->tag != 1)goto _LL59B;};
_tmpB6E=_tmpB6B.f2;{struct Cyc_Absyn_VarType_struct*_tmpB6F=(struct Cyc_Absyn_VarType_struct*)
_tmpB6E;if(_tmpB6F->tag != 2)goto _LL59B;else{_tmpB70=_tmpB6F->f1;}};_LL59A: cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB70);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmpB70,1);continue;_LL59B:;_LL59C: goto _LL598;_LL598:;}{
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);};}if(ts != 
0){const char*_tmp129B;void*_tmp129A[1];struct Cyc_String_pa_struct _tmp1299;(
_tmp1299.tag=0,((_tmp1299.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmpB4F->name)),((_tmp129A[0]=& _tmp1299,Cyc_Tcutil_terr(loc,((_tmp129B="too many parameters for type %s",
_tag_dyneither(_tmp129B,sizeof(char),32))),_tag_dyneither(_tmp129A,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp129C;hidden_ts=((
_tmp129C=_cycalloc(sizeof(*_tmp129C)),((_tmp129C->hd=e,((_tmp129C->tl=hidden_ts,
_tmp129C))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}*
_tmp9B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(*_tmp9B4,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(hidden_ts));}}_LL58E:;}goto _LL4B1;_LL4D6: {struct Cyc_Absyn_TypedefType_struct*
_tmp9B5=(struct Cyc_Absyn_TypedefType_struct*)_tmp96A;if(_tmp9B5->tag != 18)goto
_LL4D8;else{_tmp9B6=_tmp9B5->f1;_tmp9B7=_tmp9B5->f2;_tmp9B8=(struct Cyc_List_List**)&
_tmp9B5->f2;_tmp9B9=_tmp9B5->f3;_tmp9BA=(struct Cyc_Absyn_Typedefdecl**)& _tmp9B5->f3;
_tmp9BB=_tmp9B5->f4;_tmp9BC=(void***)& _tmp9B5->f4;}}_LL4D7: {struct Cyc_List_List*
targs=*_tmp9B8;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmpB75;
_push_handler(& _tmpB75);{int _tmpB77=0;if(setjmp(_tmpB75.handler))_tmpB77=1;if(!
_tmpB77){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp9B6);;_pop_handler();}else{
void*_tmpB76=(void*)_exn_thrown;void*_tmpB79=_tmpB76;_LL59E: {struct Cyc_Dict_Absent_struct*
_tmpB7A=(struct Cyc_Dict_Absent_struct*)_tmpB79;if(_tmpB7A->tag != Cyc_Dict_Absent)
goto _LL5A0;}_LL59F:{const char*_tmp12A0;void*_tmp129F[1];struct Cyc_String_pa_struct
_tmp129E;(_tmp129E.tag=0,((_tmp129E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9B6)),((_tmp129F[0]=& _tmp129E,Cyc_Tcutil_terr(
loc,((_tmp12A0="unbound typedef name %s",_tag_dyneither(_tmp12A0,sizeof(char),24))),
_tag_dyneither(_tmp129F,sizeof(void*),1)))))));}return cvtenv;_LL5A0:;_LL5A1:(
void)_throw(_tmpB79);_LL59D:;}};}*_tmp9BA=(struct Cyc_Absyn_Typedefdecl*)td;
_tmp9B6[0]=(td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;struct _RegionHandle*_tmpB7E=Cyc_Tcenv_get_fnrgn(
te);{struct Cyc_List_List*inst=0;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);{struct
_tuple14*_tmp12A3;struct Cyc_List_List*_tmp12A2;inst=((_tmp12A2=_region_malloc(
_tmpB7E,sizeof(*_tmp12A2)),((_tmp12A2->hd=((_tmp12A3=_region_malloc(_tmpB7E,
sizeof(*_tmp12A3)),((_tmp12A3->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp12A3->f2=(
void*)ts->hd,_tmp12A3)))))),((_tmp12A2->tl=inst,_tmp12A2))))));};}if(ts != 0){
const char*_tmp12A7;void*_tmp12A6[1];struct Cyc_String_pa_struct _tmp12A5;(_tmp12A5.tag=
0,((_tmp12A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp9B6)),((_tmp12A6[0]=& _tmp12A5,Cyc_Tcutil_terr(loc,((_tmp12A7="too many parameters for typedef %s",
_tag_dyneither(_tmp12A7,sizeof(char),35))),_tag_dyneither(_tmp12A6,sizeof(void*),
1)))))));}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){
enum Cyc_Absyn_Kind k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,Cyc_Absyn_BoxKind);
void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmp12A8;hidden_ts=((
_tmp12A8=_cycalloc(sizeof(*_tmp12A8)),((_tmp12A8->hd=e,((_tmp12A8->tl=hidden_ts,
_tmp12A8))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple14*_tmp12AB;struct Cyc_List_List*_tmp12AA;inst=(struct Cyc_List_List*)((
_tmp12AA=_cycalloc(sizeof(*_tmp12AA)),((_tmp12AA->hd=(struct _tuple14*)((_tmp12AB=
_cycalloc(sizeof(*_tmp12AB)),((_tmp12AB->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((
_tmp12AB->f2=e,_tmp12AB)))))),((_tmp12AA->tl=inst,_tmp12AA))))));};}*_tmp9B8=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_rsubstitute(_tmpB7E,inst,(
void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);void**_tmp12AC;*_tmp9BC=((
_tmp12AC=_cycalloc(sizeof(*_tmp12AC)),((_tmp12AC[0]=new_typ,_tmp12AC))));}}goto
_LL4B1;};}_LL4D8: {struct Cyc_Absyn_UniqueRgn_struct*_tmp9BD=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmp96A;if(_tmp9BD->tag != 22)goto _LL4DA;}_LL4D9: goto _LL4DB;_LL4DA: {struct Cyc_Absyn_HeapRgn_struct*
_tmp9BE=(struct Cyc_Absyn_HeapRgn_struct*)_tmp96A;if(_tmp9BE->tag != 21)goto _LL4DC;}
_LL4DB: goto _LL4B1;_LL4DC: {struct Cyc_Absyn_RgnHandleType_struct*_tmp9BF=(struct
Cyc_Absyn_RgnHandleType_struct*)_tmp96A;if(_tmp9BF->tag != 16)goto _LL4DE;else{
_tmp9C0=(void*)_tmp9BF->f1;}}_LL4DD: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,Cyc_Absyn_TopRgnKind,_tmp9C0,1);goto _LL4B1;_LL4DE: {struct Cyc_Absyn_DynRgnType_struct*
_tmp9C1=(struct Cyc_Absyn_DynRgnType_struct*)_tmp96A;if(_tmp9C1->tag != 17)goto
_LL4E0;else{_tmp9C2=(void*)_tmp9C1->f1;_tmp9C3=(void*)_tmp9C1->f2;}}_LL4DF:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_RgnKind,_tmp9C2,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_RgnKind,_tmp9C3,1);
goto _LL4B1;_LL4E0: {struct Cyc_Absyn_AccessEff_struct*_tmp9C4=(struct Cyc_Absyn_AccessEff_struct*)
_tmp96A;if(_tmp9C4->tag != 23)goto _LL4E2;else{_tmp9C5=(void*)_tmp9C4->f1;}}_LL4E1:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_TopRgnKind,_tmp9C5,
1);goto _LL4B1;_LL4E2: {struct Cyc_Absyn_RgnsEff_struct*_tmp9C6=(struct Cyc_Absyn_RgnsEff_struct*)
_tmp96A;if(_tmp9C6->tag != 25)goto _LL4E4;else{_tmp9C7=(void*)_tmp9C6->f1;}}_LL4E3:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,Cyc_Absyn_AnyKind,_tmp9C7,1);
goto _LL4B1;_LL4E4: {struct Cyc_Absyn_JoinEff_struct*_tmp9C8=(struct Cyc_Absyn_JoinEff_struct*)
_tmp96A;if(_tmp9C8->tag != 24)goto _LL4B1;else{_tmp9C9=_tmp9C8->f1;}}_LL4E5: for(0;
_tmp9C9 != 0;_tmp9C9=_tmp9C9->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,Cyc_Absyn_EffKind,(void*)_tmp9C9->hd,1);}goto _LL4B1;_LL4B1:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmpB88=t;struct Cyc_Core_Opt*
_tmpB8A;struct Cyc_Core_Opt*_tmpB8B;_LL5A3: {struct Cyc_Absyn_Evar_struct*_tmpB89=(
struct Cyc_Absyn_Evar_struct*)_tmpB88;if(_tmpB89->tag != 1)goto _LL5A5;else{_tmpB8A=
_tmpB89->f1;_tmpB8B=_tmpB89->f2;}}_LL5A4: {struct _dyneither_ptr s;{struct Cyc_Core_Opt*
_tmpB8C=_tmpB8A;struct Cyc_Core_Opt _tmpB8D;enum Cyc_Absyn_Kind _tmpB8E;_LL5A8: if(
_tmpB8C != 0)goto _LL5AA;_LL5A9:{const char*_tmp12AD;s=((_tmp12AD="kind=NULL ",
_tag_dyneither(_tmp12AD,sizeof(char),11)));}goto _LL5A7;_LL5AA: if(_tmpB8C == 0)
goto _LL5A7;_tmpB8D=*_tmpB8C;_tmpB8E=(enum Cyc_Absyn_Kind)_tmpB8D.v;_LL5AB:{const
char*_tmp12B1;void*_tmp12B0[1];struct Cyc_String_pa_struct _tmp12AF;s=(struct
_dyneither_ptr)((_tmp12AF.tag=0,((_tmp12AF.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB8E)),((_tmp12B0[0]=& _tmp12AF,Cyc_aprintf(((
_tmp12B1="kind=%s ",_tag_dyneither(_tmp12B1,sizeof(char),9))),_tag_dyneither(
_tmp12B0,sizeof(void*),1))))))));}goto _LL5A7;_LL5A7:;}{struct Cyc_Core_Opt*
_tmpB93=_tmpB8B;struct Cyc_Core_Opt _tmpB94;void*_tmpB95;_LL5AD: if(_tmpB93 != 0)
goto _LL5AF;_LL5AE:{const char*_tmp12B5;void*_tmp12B4[1];struct Cyc_String_pa_struct
_tmp12B3;s=(struct _dyneither_ptr)((_tmp12B3.tag=0,((_tmp12B3.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)s),((_tmp12B4[0]=& _tmp12B3,Cyc_aprintf(((
_tmp12B5="%s ref=NULL",_tag_dyneither(_tmp12B5,sizeof(char),12))),_tag_dyneither(
_tmp12B4,sizeof(void*),1))))))));}goto _LL5AC;_LL5AF: if(_tmpB93 == 0)goto _LL5AC;
_tmpB94=*_tmpB93;_tmpB95=(void*)_tmpB94.v;_LL5B0:{const char*_tmp12BA;void*
_tmp12B9[2];struct Cyc_String_pa_struct _tmp12B8;struct Cyc_String_pa_struct
_tmp12B7;s=(struct _dyneither_ptr)((_tmp12B7.tag=0,((_tmp12B7.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB95)),((
_tmp12B8.tag=0,((_tmp12B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((
_tmp12B9[0]=& _tmp12B8,((_tmp12B9[1]=& _tmp12B7,Cyc_aprintf(((_tmp12BA="%s ref=%s",
_tag_dyneither(_tmp12BA,sizeof(char),10))),_tag_dyneither(_tmp12B9,sizeof(void*),
2))))))))))))));}goto _LL5AC;_LL5AC:;}{const char*_tmp12BE;void*_tmp12BD[1];struct
Cyc_String_pa_struct _tmp12BC;(_tmp12BC.tag=0,((_tmp12BC.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)s),((_tmp12BD[0]=& _tmp12BC,Cyc_fprintf(Cyc_stderr,((
_tmp12BE="evar info: %s\n",_tag_dyneither(_tmp12BE,sizeof(char),15))),
_tag_dyneither(_tmp12BD,sizeof(void*),1)))))));}goto _LL5A2;}_LL5A5:;_LL5A6: goto
_LL5A2;_LL5A2:;}{const char*_tmp12C4;void*_tmp12C3[3];struct Cyc_String_pa_struct
_tmp12C2;struct Cyc_String_pa_struct _tmp12C1;struct Cyc_String_pa_struct _tmp12C0;(
_tmp12C0.tag=0,((_tmp12C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
expected_kind)),((_tmp12C1.tag=0,((_tmp12C1.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp12C2.tag=0,((
_tmp12C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp12C3[0]=& _tmp12C2,((_tmp12C3[1]=& _tmp12C1,((_tmp12C3[2]=& _tmp12C0,Cyc_Tcutil_terr(
loc,((_tmp12C4="type %s has kind %s but as used here needs kind %s",
_tag_dyneither(_tmp12C4,sizeof(char),51))),_tag_dyneither(_tmp12C3,sizeof(void*),
3)))))))))))))))))));};}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){{void*_tmpBA5=e->r;
struct Cyc_List_List*_tmpBAB;struct Cyc_Absyn_Exp*_tmpBAD;struct Cyc_Absyn_Exp*
_tmpBAE;struct Cyc_Absyn_Exp*_tmpBAF;struct Cyc_Absyn_Exp*_tmpBB1;struct Cyc_Absyn_Exp*
_tmpBB2;struct Cyc_Absyn_Exp*_tmpBB4;struct Cyc_Absyn_Exp*_tmpBB5;struct Cyc_Absyn_Exp*
_tmpBB7;struct Cyc_Absyn_Exp*_tmpBB8;void*_tmpBBA;struct Cyc_Absyn_Exp*_tmpBBB;
void*_tmpBBD;void*_tmpBBF;void*_tmpBC1;struct Cyc_Absyn_Exp*_tmpBC3;_LL5B2: {
struct Cyc_Absyn_Const_e_struct*_tmpBA6=(struct Cyc_Absyn_Const_e_struct*)_tmpBA5;
if(_tmpBA6->tag != 0)goto _LL5B4;}_LL5B3: goto _LL5B5;_LL5B4: {struct Cyc_Absyn_Enum_e_struct*
_tmpBA7=(struct Cyc_Absyn_Enum_e_struct*)_tmpBA5;if(_tmpBA7->tag != 33)goto _LL5B6;}
_LL5B5: goto _LL5B7;_LL5B6: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpBA8=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpBA5;if(_tmpBA8->tag != 34)goto _LL5B8;}_LL5B7: goto _LL5B9;_LL5B8: {struct Cyc_Absyn_Var_e_struct*
_tmpBA9=(struct Cyc_Absyn_Var_e_struct*)_tmpBA5;if(_tmpBA9->tag != 1)goto _LL5BA;}
_LL5B9: goto _LL5B1;_LL5BA: {struct Cyc_Absyn_Primop_e_struct*_tmpBAA=(struct Cyc_Absyn_Primop_e_struct*)
_tmpBA5;if(_tmpBAA->tag != 3)goto _LL5BC;else{_tmpBAB=_tmpBAA->f2;}}_LL5BB: for(0;
_tmpBAB != 0;_tmpBAB=_tmpBAB->tl){cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((
struct Cyc_Absyn_Exp*)_tmpBAB->hd,te,cvtenv);}goto _LL5B1;_LL5BC: {struct Cyc_Absyn_Conditional_e_struct*
_tmpBAC=(struct Cyc_Absyn_Conditional_e_struct*)_tmpBA5;if(_tmpBAC->tag != 6)goto
_LL5BE;else{_tmpBAD=_tmpBAC->f1;_tmpBAE=_tmpBAC->f2;_tmpBAF=_tmpBAC->f3;}}_LL5BD:
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBAD,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpBAE,te,cvtenv);cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBAF,te,
cvtenv);goto _LL5B1;_LL5BE: {struct Cyc_Absyn_And_e_struct*_tmpBB0=(struct Cyc_Absyn_And_e_struct*)
_tmpBA5;if(_tmpBB0->tag != 7)goto _LL5C0;else{_tmpBB1=_tmpBB0->f1;_tmpBB2=_tmpBB0->f2;}}
_LL5BF: _tmpBB4=_tmpBB1;_tmpBB5=_tmpBB2;goto _LL5C1;_LL5C0: {struct Cyc_Absyn_Or_e_struct*
_tmpBB3=(struct Cyc_Absyn_Or_e_struct*)_tmpBA5;if(_tmpBB3->tag != 8)goto _LL5C2;
else{_tmpBB4=_tmpBB3->f1;_tmpBB5=_tmpBB3->f2;}}_LL5C1: _tmpBB7=_tmpBB4;_tmpBB8=
_tmpBB5;goto _LL5C3;_LL5C2: {struct Cyc_Absyn_SeqExp_e_struct*_tmpBB6=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpBA5;if(_tmpBB6->tag != 9)goto _LL5C4;else{_tmpBB7=_tmpBB6->f1;_tmpBB8=_tmpBB6->f2;}}
_LL5C3: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB7,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB8,te,cvtenv);goto _LL5B1;_LL5C4: {
struct Cyc_Absyn_Cast_e_struct*_tmpBB9=(struct Cyc_Absyn_Cast_e_struct*)_tmpBA5;
if(_tmpBB9->tag != 15)goto _LL5C6;else{_tmpBBA=(void*)_tmpBB9->f1;_tmpBBB=_tmpBB9->f2;}}
_LL5C5: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBBB,te,cvtenv);cvtenv=
Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,Cyc_Absyn_AnyKind,_tmpBBA,1);goto
_LL5B1;_LL5C6: {struct Cyc_Absyn_Offsetof_e_struct*_tmpBBC=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpBA5;if(_tmpBBC->tag != 20)goto _LL5C8;else{_tmpBBD=(void*)_tmpBBC->f1;}}_LL5C7:
_tmpBBF=_tmpBBD;goto _LL5C9;_LL5C8: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpBBE=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpBA5;if(_tmpBBE->tag != 18)goto _LL5CA;
else{_tmpBBF=(void*)_tmpBBE->f1;}}_LL5C9: cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,
te,cvtenv,Cyc_Absyn_AnyKind,_tmpBBF,1);goto _LL5B1;_LL5CA: {struct Cyc_Absyn_Valueof_e_struct*
_tmpBC0=(struct Cyc_Absyn_Valueof_e_struct*)_tmpBA5;if(_tmpBC0->tag != 40)goto
_LL5CC;else{_tmpBC1=(void*)_tmpBC0->f1;}}_LL5CB: cvtenv=Cyc_Tcutil_i_check_valid_type(
e->loc,te,cvtenv,Cyc_Absyn_IntKind,_tmpBC1,1);goto _LL5B1;_LL5CC: {struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpBC2=(struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpBA5;if(_tmpBC2->tag != 19)goto
_LL5CE;else{_tmpBC3=_tmpBC2->f1;}}_LL5CD: cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(
_tmpBC3,te,cvtenv);goto _LL5B1;_LL5CE:;_LL5CF: {const char*_tmp12C7;void*_tmp12C6;(
_tmp12C6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp12C7="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",
_tag_dyneither(_tmp12C7,sizeof(char),66))),_tag_dyneither(_tmp12C6,sizeof(void*),
0)));}_LL5B1:;}return cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,enum Cyc_Absyn_Kind expected_kind,void*t);static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvt,enum Cyc_Absyn_Kind expected_kind,void*t){struct Cyc_List_List*_tmpBC6=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);{struct Cyc_List_List*
vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){struct Cyc_Absyn_Tvar*_tmpBC8;struct
_tuple24 _tmpBC7=*((struct _tuple24*)vs->hd);_tmpBC8=_tmpBC7.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(
_tmpBC6,_tmpBC8);}}{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
void*_tmpBCA;struct _tuple25 _tmpBC9=*((struct _tuple25*)evs->hd);_tmpBCA=_tmpBC9.f1;{
void*_tmpBCB=Cyc_Tcutil_compress(_tmpBCA);struct Cyc_Core_Opt*_tmpBCD;struct Cyc_Core_Opt**
_tmpBCE;_LL5D1: {struct Cyc_Absyn_Evar_struct*_tmpBCC=(struct Cyc_Absyn_Evar_struct*)
_tmpBCB;if(_tmpBCC->tag != 1)goto _LL5D3;else{_tmpBCD=_tmpBCC->f4;_tmpBCE=(struct
Cyc_Core_Opt**)& _tmpBCC->f4;}}_LL5D2: if(*_tmpBCE == 0){struct Cyc_Core_Opt*
_tmp12C8;*_tmpBCE=((_tmp12C8=_cycalloc(sizeof(*_tmp12C8)),((_tmp12C8->v=_tmpBC6,
_tmp12C8))));}else{struct Cyc_List_List*_tmpBD0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpBCE))->v;struct Cyc_List_List*_tmpBD1=0;for(0;_tmpBD0 != 0;
_tmpBD0=_tmpBD0->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmpBC6,(struct Cyc_Absyn_Tvar*)_tmpBD0->hd)){struct Cyc_List_List*_tmp12C9;
_tmpBD1=((_tmp12C9=_cycalloc(sizeof(*_tmp12C9)),((_tmp12C9->hd=(struct Cyc_Absyn_Tvar*)
_tmpBD0->hd,((_tmp12C9->tl=_tmpBD1,_tmp12C9))))));}}{struct Cyc_Core_Opt*_tmp12CA;*
_tmpBCE=((_tmp12CA=_cycalloc(sizeof(*_tmp12CA)),((_tmp12CA->v=_tmpBD1,_tmp12CA))));};}
goto _LL5D0;_LL5D3:;_LL5D4: goto _LL5D0;_LL5D0:;};}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmpBD4=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle*_tmpBD5=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv
_tmp12CB;struct Cyc_Tcutil_CVTEnv _tmpBD6=Cyc_Tcutil_check_valid_type(loc,te,((
_tmp12CB.r=_tmpBD5,((_tmp12CB.kind_env=_tmpBD4,((_tmp12CB.free_vars=0,((_tmp12CB.free_evars=
0,((_tmp12CB.generalize_evars=generalize_evars,((_tmp12CB.fn_result=0,_tmp12CB)))))))))))),
Cyc_Absyn_MemKind,t);struct Cyc_List_List*_tmpBD7=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple24*),struct Cyc_List_List*
x))Cyc_List_rmap)(_tmpBD5,(struct Cyc_Absyn_Tvar*(*)(struct _tuple24*))Cyc_Core_fst,
_tmpBD6.free_vars);struct Cyc_List_List*_tmpBD8=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _tuple25*),struct Cyc_List_List*x))Cyc_List_rmap)(
_tmpBD5,(void*(*)(struct _tuple25*))Cyc_Core_fst,_tmpBD6.free_evars);if(_tmpBD4 != 
0){struct Cyc_List_List*_tmpBD9=0;{struct Cyc_List_List*_tmpBDA=_tmpBD7;for(0;(
unsigned int)_tmpBDA;_tmpBDA=_tmpBDA->tl){struct Cyc_Absyn_Tvar*_tmpBDB=(struct
Cyc_Absyn_Tvar*)_tmpBDA->hd;int found=0;{struct Cyc_List_List*_tmpBDC=_tmpBD4;for(
0;(unsigned int)_tmpBDC;_tmpBDC=_tmpBDC->tl){if(Cyc_Absyn_tvar_cmp(_tmpBDB,(
struct Cyc_Absyn_Tvar*)_tmpBDC->hd)== 0){found=1;break;}}}if(!found){struct Cyc_List_List*
_tmp12CC;_tmpBD9=((_tmp12CC=_region_malloc(_tmpBD5,sizeof(*_tmp12CC)),((_tmp12CC->hd=(
struct Cyc_Absyn_Tvar*)_tmpBDA->hd,((_tmp12CC->tl=_tmpBD9,_tmp12CC))))));}}}
_tmpBD7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmpBD9);}{struct Cyc_List_List*x=_tmpBD7;for(0;x != 0;x=x->tl){void*_tmpBDE=Cyc_Absyn_compress_kb(((
struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmpBE0;struct Cyc_Core_Opt**
_tmpBE1;struct Cyc_Core_Opt*_tmpBE3;struct Cyc_Core_Opt**_tmpBE4;enum Cyc_Absyn_Kind
_tmpBE5;struct Cyc_Core_Opt*_tmpBE7;struct Cyc_Core_Opt**_tmpBE8;enum Cyc_Absyn_Kind
_tmpBE9;struct Cyc_Core_Opt*_tmpBEB;struct Cyc_Core_Opt**_tmpBEC;enum Cyc_Absyn_Kind
_tmpBED;enum Cyc_Absyn_Kind _tmpBEF;_LL5D6: {struct Cyc_Absyn_Unknown_kb_struct*
_tmpBDF=(struct Cyc_Absyn_Unknown_kb_struct*)_tmpBDE;if(_tmpBDF->tag != 1)goto
_LL5D8;else{_tmpBE0=_tmpBDF->f1;_tmpBE1=(struct Cyc_Core_Opt**)& _tmpBDF->f1;}}
_LL5D7: _tmpBE4=_tmpBE1;goto _LL5D9;_LL5D8: {struct Cyc_Absyn_Less_kb_struct*
_tmpBE2=(struct Cyc_Absyn_Less_kb_struct*)_tmpBDE;if(_tmpBE2->tag != 2)goto _LL5DA;
else{_tmpBE3=_tmpBE2->f1;_tmpBE4=(struct Cyc_Core_Opt**)& _tmpBE2->f1;_tmpBE5=
_tmpBE2->f2;if(_tmpBE5 != Cyc_Absyn_MemKind)goto _LL5DA;}}_LL5D9:*_tmpBE4=Cyc_Tcutil_kind_to_bound_opt(
Cyc_Absyn_BoxKind);goto _LL5D5;_LL5DA: {struct Cyc_Absyn_Less_kb_struct*_tmpBE6=(
struct Cyc_Absyn_Less_kb_struct*)_tmpBDE;if(_tmpBE6->tag != 2)goto _LL5DC;else{
_tmpBE7=_tmpBE6->f1;_tmpBE8=(struct Cyc_Core_Opt**)& _tmpBE6->f1;_tmpBE9=_tmpBE6->f2;
if(_tmpBE9 != Cyc_Absyn_TopRgnKind)goto _LL5DC;}}_LL5DB:*_tmpBE8=Cyc_Tcutil_kind_to_bound_opt(
Cyc_Absyn_RgnKind);goto _LL5D5;_LL5DC: {struct Cyc_Absyn_Less_kb_struct*_tmpBEA=(
struct Cyc_Absyn_Less_kb_struct*)_tmpBDE;if(_tmpBEA->tag != 2)goto _LL5DE;else{
_tmpBEB=_tmpBEA->f1;_tmpBEC=(struct Cyc_Core_Opt**)& _tmpBEA->f1;_tmpBED=_tmpBEA->f2;}}
_LL5DD:*_tmpBEC=Cyc_Tcutil_kind_to_bound_opt(_tmpBED);goto _LL5D5;_LL5DE: {struct
Cyc_Absyn_Eq_kb_struct*_tmpBEE=(struct Cyc_Absyn_Eq_kb_struct*)_tmpBDE;if(_tmpBEE->tag
!= 0)goto _LL5E0;else{_tmpBEF=_tmpBEE->f1;if(_tmpBEF != Cyc_Absyn_MemKind)goto
_LL5E0;}}_LL5DF:{const char*_tmp12D0;void*_tmp12CF[1];struct Cyc_String_pa_struct
_tmp12CE;(_tmp12CE.tag=0,((_tmp12CE.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp12CF[
0]=& _tmp12CE,Cyc_Tcutil_terr(loc,((_tmp12D0="type variable %s cannot have kind M",
_tag_dyneither(_tmp12D0,sizeof(char),36))),_tag_dyneither(_tmp12CF,sizeof(void*),
1)))))));}goto _LL5D5;_LL5E0:;_LL5E1: goto _LL5D5;_LL5D5:;}}if(_tmpBD7 != 0  || 
_tmpBD8 != 0){{void*_tmpBF3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpBF5;
struct Cyc_List_List*_tmpBF6;struct Cyc_List_List**_tmpBF7;struct Cyc_Core_Opt*
_tmpBF8;void*_tmpBF9;struct Cyc_List_List*_tmpBFA;int _tmpBFB;struct Cyc_Absyn_VarargInfo*
_tmpBFC;struct Cyc_List_List*_tmpBFD;struct Cyc_List_List*_tmpBFE;_LL5E3: {struct
Cyc_Absyn_FnType_struct*_tmpBF4=(struct Cyc_Absyn_FnType_struct*)_tmpBF3;if(
_tmpBF4->tag != 10)goto _LL5E5;else{_tmpBF5=_tmpBF4->f1;_tmpBF6=_tmpBF5.tvars;
_tmpBF7=(struct Cyc_List_List**)&(_tmpBF4->f1).tvars;_tmpBF8=_tmpBF5.effect;
_tmpBF9=_tmpBF5.ret_typ;_tmpBFA=_tmpBF5.args;_tmpBFB=_tmpBF5.c_varargs;_tmpBFC=
_tmpBF5.cyc_varargs;_tmpBFD=_tmpBF5.rgn_po;_tmpBFE=_tmpBF5.attributes;}}_LL5E4:
if(*_tmpBF7 == 0){*_tmpBF7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(
_tmpBD7);_tmpBD7=0;}goto _LL5E2;_LL5E5:;_LL5E6: goto _LL5E2;_LL5E2:;}if(_tmpBD7 != 0){
const char*_tmp12D4;void*_tmp12D3[1];struct Cyc_String_pa_struct _tmp12D2;(_tmp12D2.tag=
0,((_tmp12D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)
_tmpBD7->hd)->name),((_tmp12D3[0]=& _tmp12D2,Cyc_Tcutil_terr(loc,((_tmp12D4="unbound type variable %s",
_tag_dyneither(_tmp12D4,sizeof(char),25))),_tag_dyneither(_tmp12D3,sizeof(void*),
1)))))));}if(_tmpBD8 != 0)for(0;_tmpBD8 != 0;_tmpBD8=_tmpBD8->tl){void*e=(void*)
_tmpBD8->hd;switch(Cyc_Tcutil_typ_kind(e)){case Cyc_Absyn_UniqueRgnKind: _LL5E7:
if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*_tmp12D7;void*
_tmp12D6;(_tmp12D6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp12D7="can't unify evar with unique region!",
_tag_dyneither(_tmp12D7,sizeof(char),37))),_tag_dyneither(_tmp12D6,sizeof(void*),
0)));}break;case Cyc_Absyn_TopRgnKind: _LL5E8: goto _LL5E9;case Cyc_Absyn_RgnKind:
_LL5E9: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp12DA;
void*_tmp12D9;(_tmp12D9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp12DA="can't unify evar with heap!",_tag_dyneither(
_tmp12DA,sizeof(char),28))),_tag_dyneither(_tmp12D9,sizeof(void*),0)));}break;
case Cyc_Absyn_EffKind: _LL5EA: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){
const char*_tmp12DD;void*_tmp12DC;(_tmp12DC=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12DD="can't unify evar with {}!",
_tag_dyneither(_tmp12DD,sizeof(char),26))),_tag_dyneither(_tmp12DC,sizeof(void*),
0)));}break;default: _LL5EB:{const char*_tmp12E2;void*_tmp12E1[2];struct Cyc_String_pa_struct
_tmp12E0;struct Cyc_String_pa_struct _tmp12DF;(_tmp12DF.tag=0,((_tmp12DF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp12E0.tag=0,((_tmp12E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp12E1[0]=& _tmp12E0,((_tmp12E1[1]=& _tmp12DF,Cyc_Tcutil_terr(loc,((
_tmp12E2="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp12E2,sizeof(char),52))),_tag_dyneither(_tmp12E1,sizeof(void*),2)))))))))))));}
break;}}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmpC0D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpC0F;struct Cyc_List_List*
_tmpC10;struct Cyc_Core_Opt*_tmpC11;void*_tmpC12;_LL5EE: {struct Cyc_Absyn_FnType_struct*
_tmpC0E=(struct Cyc_Absyn_FnType_struct*)_tmpC0D;if(_tmpC0E->tag != 10)goto _LL5F0;
else{_tmpC0F=_tmpC0E->f1;_tmpC10=_tmpC0F.tvars;_tmpC11=_tmpC0F.effect;_tmpC12=
_tmpC0F.ret_typ;}}_LL5EF: fd->tvs=_tmpC10;fd->effect=_tmpC11;goto _LL5ED;_LL5F0:;
_LL5F1: {const char*_tmp12E5;void*_tmp12E4;(_tmp12E4=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12E5="check_fndecl_valid_type: not a FnType",
_tag_dyneither(_tmp12E5,sizeof(char),38))),_tag_dyneither(_tmp12E4,sizeof(void*),
0)));}_LL5ED:;}{struct _RegionHandle*_tmpC15=Cyc_Tcenv_get_fnrgn(te);{const char*
_tmp12E6;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple20*),struct Cyc_List_List*x))
Cyc_List_rmap)(_tmpC15,(struct _dyneither_ptr*(*)(struct _tuple20*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmp12E6="function declaration has repeated parameter",
_tag_dyneither(_tmp12E6,sizeof(char),44))));}{struct Cyc_Core_Opt*_tmp12E7;fd->cached_typ=((
_tmp12E7=_cycalloc(sizeof(*_tmp12E7)),((_tmp12E7->v=t,_tmp12E7))));};};}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,enum Cyc_Absyn_Kind expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,enum Cyc_Absyn_Kind expected_kind,int allow_evars,void*t){struct
_RegionHandle*_tmpC18=Cyc_Tcenv_get_fnrgn(te);struct Cyc_Tcutil_CVTEnv _tmp12E8;
struct Cyc_Tcutil_CVTEnv _tmpC19=Cyc_Tcutil_check_valid_type(loc,te,((_tmp12E8.r=
_tmpC18,((_tmp12E8.kind_env=bound_tvars,((_tmp12E8.free_vars=0,((_tmp12E8.free_evars=
0,((_tmp12E8.generalize_evars=0,((_tmp12E8.fn_result=0,_tmp12E8)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmpC1A=Cyc_Tcutil_remove_bound_tvars(
_tmpC18,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple24*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmpC18,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple24*))Cyc_Core_fst,_tmpC19.free_vars),bound_tvars);struct Cyc_List_List*
_tmpC1B=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple25*),
struct Cyc_List_List*x))Cyc_List_rmap)(_tmpC18,(void*(*)(struct _tuple25*))Cyc_Core_fst,
_tmpC19.free_evars);{struct Cyc_List_List*fs=_tmpC1A;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpC1C=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmp12ED;void*_tmp12EC[2];struct Cyc_String_pa_struct _tmp12EB;struct Cyc_String_pa_struct
_tmp12EA;(_tmp12EA.tag=0,((_tmp12EA.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp12EB.tag=0,((_tmp12EB.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)*_tmpC1C),((_tmp12EC[0]=& _tmp12EB,((
_tmp12EC[1]=& _tmp12EA,Cyc_Tcutil_terr(loc,((_tmp12ED="unbound type variable %s in type %s",
_tag_dyneither(_tmp12ED,sizeof(char),36))),_tag_dyneither(_tmp12EC,sizeof(void*),
2)))))))))))));}}if(!allow_evars  && _tmpC1B != 0)for(0;_tmpC1B != 0;_tmpC1B=
_tmpC1B->tl){void*e=(void*)_tmpC1B->hd;switch(Cyc_Tcutil_typ_kind(e)){case Cyc_Absyn_UniqueRgnKind:
_LL5F2: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){const char*
_tmp12F0;void*_tmp12EF;(_tmp12EF=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F0="can't unify evar with unique region!",
_tag_dyneither(_tmp12F0,sizeof(char),37))),_tag_dyneither(_tmp12EF,sizeof(void*),
0)));}break;case Cyc_Absyn_TopRgnKind: _LL5F3: goto _LL5F4;case Cyc_Absyn_RgnKind:
_LL5F4: if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp12F3;
void*_tmp12F2;(_tmp12F2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(((_tmp12F3="can't unify evar with heap!",_tag_dyneither(
_tmp12F3,sizeof(char),28))),_tag_dyneither(_tmp12F2,sizeof(void*),0)));}break;
case Cyc_Absyn_EffKind: _LL5F5: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){
const char*_tmp12F6;void*_tmp12F5;(_tmp12F5=0,((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12F6="can't unify evar with {}!",
_tag_dyneither(_tmp12F6,sizeof(char),26))),_tag_dyneither(_tmp12F5,sizeof(void*),
0)));}break;default: _LL5F6:{const char*_tmp12FB;void*_tmp12FA[2];struct Cyc_String_pa_struct
_tmp12F9;struct Cyc_String_pa_struct _tmp12F8;(_tmp12F8.tag=0,((_tmp12F8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((
_tmp12F9.tag=0,((_tmp12F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
e)),((_tmp12FA[0]=& _tmp12F9,((_tmp12FA[1]=& _tmp12F8,Cyc_Tcutil_terr(loc,((
_tmp12FB="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(
_tmp12FB,sizeof(char),52))),_tag_dyneither(_tmp12FA,sizeof(void*),2)))))))))))));}
break;}}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)tv->identity=Cyc_Tcutil_new_tvar_id();}
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg);static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){for(0;vs != 0;vs=
vs->tl){struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)
vs->hd,(void*)vs2->hd)== 0){const char*_tmp1300;void*_tmp12FF[2];struct Cyc_String_pa_struct
_tmp12FE;struct Cyc_String_pa_struct _tmp12FD;(_tmp12FD.tag=0,((_tmp12FD.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)a2string((void*)vs->hd)),((_tmp12FE.tag=
0,((_tmp12FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp12FF[0]=&
_tmp12FE,((_tmp12FF[1]=& _tmp12FD,Cyc_Tcutil_terr(loc,((_tmp1300="%s: %s",
_tag_dyneither(_tmp1300,sizeof(char),7))),_tag_dyneither(_tmp12FF,sizeof(void*),
2)))))))))))));}}}}static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s);static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct
_dyneither_ptr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg);void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr msg){((
void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct
_dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,
vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(struct
Cyc_Position_Segment*loc,struct Cyc_List_List*tvs);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){const char*_tmp1301;((
void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),
struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,
tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1301="duplicate type variable",
_tag_dyneither(_tmp1301,sizeof(char),24))));}struct _tuple27{struct Cyc_Absyn_Aggrfield*
f1;int f2;};struct _tuple28{struct Cyc_List_List*f1;void*f2;};struct _tuple29{struct
Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields);struct Cyc_List_List*
Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*
sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*sd_fields=sdfields;
for(0;sd_fields != 0;sd_fields=sd_fields->tl){const char*_tmp1302;if(Cyc_strcmp((
struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((
_tmp1302="",_tag_dyneither(_tmp1302,sizeof(char),1))))!= 0){struct _tuple27*
_tmp1305;struct Cyc_List_List*_tmp1304;fields=((_tmp1304=_cycalloc(sizeof(*
_tmp1304)),((_tmp1304->hd=((_tmp1305=_cycalloc(sizeof(*_tmp1305)),((_tmp1305->f1=(
struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1305->f2=0,_tmp1305)))))),((
_tmp1304->tl=fields,_tmp1304))))));}}}fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fields);{const char*_tmp1307;const char*_tmp1306;struct
_dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp1307="struct",
_tag_dyneither(_tmp1307,sizeof(char),7)):((_tmp1306="union",_tag_dyneither(
_tmp1306,sizeof(char),6)));struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){
struct _tuple28 _tmpC35;struct Cyc_List_List*_tmpC36;void*_tmpC37;struct _tuple28*
_tmpC34=(struct _tuple28*)des->hd;_tmpC35=*_tmpC34;_tmpC36=_tmpC35.f1;_tmpC37=
_tmpC35.f2;if(_tmpC36 == 0){struct Cyc_List_List*_tmpC38=fields;for(0;_tmpC38 != 0;
_tmpC38=_tmpC38->tl){if(!(*((struct _tuple27*)_tmpC38->hd)).f2){(*((struct
_tuple27*)_tmpC38->hd)).f2=1;{struct Cyc_Absyn_FieldName_struct*_tmp130D;struct
Cyc_Absyn_FieldName_struct _tmp130C;struct Cyc_List_List*_tmp130B;(*((struct
_tuple28*)des->hd)).f1=(struct Cyc_List_List*)((_tmp130B=_cycalloc(sizeof(*
_tmp130B)),((_tmp130B->hd=(void*)((_tmp130D=_cycalloc(sizeof(*_tmp130D)),((
_tmp130D[0]=((_tmp130C.tag=1,((_tmp130C.f1=((*((struct _tuple27*)_tmpC38->hd)).f1)->name,
_tmp130C)))),_tmp130D)))),((_tmp130B->tl=0,_tmp130B))))));}{struct _tuple29*
_tmp1310;struct Cyc_List_List*_tmp130F;ans=((_tmp130F=_region_malloc(rgn,sizeof(*
_tmp130F)),((_tmp130F->hd=((_tmp1310=_region_malloc(rgn,sizeof(*_tmp1310)),((
_tmp1310->f1=(*((struct _tuple27*)_tmpC38->hd)).f1,((_tmp1310->f2=_tmpC37,
_tmp1310)))))),((_tmp130F->tl=ans,_tmp130F))))));}break;}}if(_tmpC38 == 0){const
char*_tmp1314;void*_tmp1313[1];struct Cyc_String_pa_struct _tmp1312;(_tmp1312.tag=
0,((_tmp1312.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((
_tmp1313[0]=& _tmp1312,Cyc_Tcutil_terr(loc,((_tmp1314="too many arguments to %s",
_tag_dyneither(_tmp1314,sizeof(char),25))),_tag_dyneither(_tmp1313,sizeof(void*),
1)))))));}}else{if(_tmpC36->tl != 0){const char*_tmp1317;void*_tmp1316;(_tmp1316=0,
Cyc_Tcutil_terr(loc,((_tmp1317="multiple designators are not yet supported",
_tag_dyneither(_tmp1317,sizeof(char),43))),_tag_dyneither(_tmp1316,sizeof(void*),
0)));}else{void*_tmpC43=(void*)_tmpC36->hd;struct _dyneither_ptr*_tmpC46;_LL5F9: {
struct Cyc_Absyn_ArrayElement_struct*_tmpC44=(struct Cyc_Absyn_ArrayElement_struct*)
_tmpC43;if(_tmpC44->tag != 0)goto _LL5FB;}_LL5FA:{const char*_tmp131B;void*_tmp131A[
1];struct Cyc_String_pa_struct _tmp1319;(_tmp1319.tag=0,((_tmp1319.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp131A[0]=& _tmp1319,Cyc_Tcutil_terr(
loc,((_tmp131B="array designator used in argument to %s",_tag_dyneither(_tmp131B,
sizeof(char),40))),_tag_dyneither(_tmp131A,sizeof(void*),1)))))));}goto _LL5F8;
_LL5FB: {struct Cyc_Absyn_FieldName_struct*_tmpC45=(struct Cyc_Absyn_FieldName_struct*)
_tmpC43;if(_tmpC45->tag != 1)goto _LL5F8;else{_tmpC46=_tmpC45->f1;}}_LL5FC: {
struct Cyc_List_List*_tmpC4A=fields;for(0;_tmpC4A != 0;_tmpC4A=_tmpC4A->tl){if(Cyc_strptrcmp(
_tmpC46,((*((struct _tuple27*)_tmpC4A->hd)).f1)->name)== 0){if((*((struct _tuple27*)
_tmpC4A->hd)).f2){const char*_tmp131F;void*_tmp131E[1];struct Cyc_String_pa_struct
_tmp131D;(_tmp131D.tag=0,((_tmp131D.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)*_tmpC46),((_tmp131E[0]=& _tmp131D,Cyc_Tcutil_terr(loc,((_tmp131F="member %s has already been used as an argument",
_tag_dyneither(_tmp131F,sizeof(char),47))),_tag_dyneither(_tmp131E,sizeof(void*),
1)))))));}(*((struct _tuple27*)_tmpC4A->hd)).f2=1;{struct _tuple29*_tmp1322;struct
Cyc_List_List*_tmp1321;ans=((_tmp1321=_region_malloc(rgn,sizeof(*_tmp1321)),((
_tmp1321->hd=((_tmp1322=_region_malloc(rgn,sizeof(*_tmp1322)),((_tmp1322->f1=(*((
struct _tuple27*)_tmpC4A->hd)).f1,((_tmp1322->f2=_tmpC37,_tmp1322)))))),((
_tmp1321->tl=ans,_tmp1321))))));}break;}}if(_tmpC4A == 0){const char*_tmp1326;void*
_tmp1325[1];struct Cyc_String_pa_struct _tmp1324;(_tmp1324.tag=0,((_tmp1324.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC46),((_tmp1325[0]=& _tmp1324,
Cyc_Tcutil_terr(loc,((_tmp1326="bad field designator %s",_tag_dyneither(_tmp1326,
sizeof(char),24))),_tag_dyneither(_tmp1325,sizeof(void*),1)))))));}goto _LL5F8;}
_LL5F8:;}}}if(aggr_kind == Cyc_Absyn_StructA)for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple27*)fields->hd)).f2){{const char*_tmp1329;void*_tmp1328;(
_tmp1328=0,Cyc_Tcutil_terr(loc,((_tmp1329="too few arguments to struct",
_tag_dyneither(_tmp1329,sizeof(char),28))),_tag_dyneither(_tmp1328,sizeof(void*),
0)));}break;}}else{int found=0;for(0;fields != 0;fields=fields->tl){if((*((struct
_tuple27*)fields->hd)).f2){if(found){const char*_tmp132C;void*_tmp132B;(_tmp132B=
0,Cyc_Tcutil_terr(loc,((_tmp132C="only one member of a union is allowed",
_tag_dyneither(_tmp132C,sizeof(char),38))),_tag_dyneither(_tmp132B,sizeof(void*),
0)));}found=1;}}if(!found){const char*_tmp132F;void*_tmp132E;(_tmp132E=0,Cyc_Tcutil_terr(
loc,((_tmp132F="missing member for union",_tag_dyneither(_tmp132F,sizeof(char),
25))),_tag_dyneither(_tmp132E,sizeof(void*),0)));}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);};}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpC5B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpC5D;
void*_tmpC5E;struct Cyc_Absyn_PtrAtts _tmpC5F;union Cyc_Absyn_Constraint*_tmpC60;
_LL5FE: {struct Cyc_Absyn_PointerType_struct*_tmpC5C=(struct Cyc_Absyn_PointerType_struct*)
_tmpC5B;if(_tmpC5C->tag != 5)goto _LL600;else{_tmpC5D=_tmpC5C->f1;_tmpC5E=_tmpC5D.elt_typ;
_tmpC5F=_tmpC5D.ptr_atts;_tmpC60=_tmpC5F.bounds;}}_LL5FF: {void*_tmpC61=((void*(*)(
void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,
_tmpC60);_LL603: {struct Cyc_Absyn_DynEither_b_struct*_tmpC62=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpC61;if(_tmpC62->tag != 0)goto _LL605;}_LL604:*elt_typ_dest=_tmpC5E;return 1;
_LL605:;_LL606: return 0;_LL602:;}_LL600:;_LL601: return 0;_LL5FD:;}int Cyc_Tcutil_is_zero_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpC63=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpC65;
void*_tmpC66;struct Cyc_Absyn_PtrAtts _tmpC67;union Cyc_Absyn_Constraint*_tmpC68;
_LL608: {struct Cyc_Absyn_PointerType_struct*_tmpC64=(struct Cyc_Absyn_PointerType_struct*)
_tmpC63;if(_tmpC64->tag != 5)goto _LL60A;else{_tmpC65=_tmpC64->f1;_tmpC66=_tmpC65.elt_typ;
_tmpC67=_tmpC65.ptr_atts;_tmpC68=_tmpC67.zero_term;}}_LL609:*elt_typ_dest=
_tmpC66;return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,
_tmpC68);_LL60A:;_LL60B: return 0;_LL607:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dyneither,void**elt_type){void*_tmpC69=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpC6B;void*_tmpC6C;struct Cyc_Absyn_PtrAtts _tmpC6D;
union Cyc_Absyn_Constraint*_tmpC6E;union Cyc_Absyn_Constraint*_tmpC6F;struct Cyc_Absyn_ArrayInfo
_tmpC71;void*_tmpC72;struct Cyc_Absyn_Tqual _tmpC73;struct Cyc_Absyn_Exp*_tmpC74;
union Cyc_Absyn_Constraint*_tmpC75;_LL60D: {struct Cyc_Absyn_PointerType_struct*
_tmpC6A=(struct Cyc_Absyn_PointerType_struct*)_tmpC69;if(_tmpC6A->tag != 5)goto
_LL60F;else{_tmpC6B=_tmpC6A->f1;_tmpC6C=_tmpC6B.elt_typ;_tmpC6D=_tmpC6B.ptr_atts;
_tmpC6E=_tmpC6D.bounds;_tmpC6F=_tmpC6D.zero_term;}}_LL60E: if(((int(*)(int y,union
Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC6F)){*ptr_type=t;*elt_type=
_tmpC6C;{void*_tmpC76=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
Cyc_Absyn_bounds_one,_tmpC6E);_LL614: {struct Cyc_Absyn_DynEither_b_struct*
_tmpC77=(struct Cyc_Absyn_DynEither_b_struct*)_tmpC76;if(_tmpC77->tag != 0)goto
_LL616;}_LL615:*is_dyneither=1;goto _LL613;_LL616:;_LL617:*is_dyneither=0;goto
_LL613;_LL613:;}return 1;}else{return 0;}_LL60F: {struct Cyc_Absyn_ArrayType_struct*
_tmpC70=(struct Cyc_Absyn_ArrayType_struct*)_tmpC69;if(_tmpC70->tag != 9)goto
_LL611;else{_tmpC71=_tmpC70->f1;_tmpC72=_tmpC71.elt_type;_tmpC73=_tmpC71.tq;
_tmpC74=_tmpC71.num_elts;_tmpC75=_tmpC71.zero_term;}}_LL610: if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC75)){*elt_type=_tmpC72;*
is_dyneither=0;{struct Cyc_Absyn_PointerType_struct _tmp1344;struct Cyc_Absyn_PtrAtts
_tmp1343;struct Cyc_Absyn_Upper_b_struct _tmp1342;struct Cyc_Absyn_Upper_b_struct*
_tmp1341;struct Cyc_Absyn_PtrInfo _tmp1340;struct Cyc_Absyn_PointerType_struct*
_tmp133F;*ptr_type=(void*)((_tmp133F=_cycalloc(sizeof(*_tmp133F)),((_tmp133F[0]=((
_tmp1344.tag=5,((_tmp1344.f1=((_tmp1340.elt_typ=_tmpC72,((_tmp1340.elt_tq=
_tmpC73,((_tmp1340.ptr_atts=((_tmp1343.rgn=(void*)& Cyc_Absyn_HeapRgn_val,((
_tmp1343.nullable=Cyc_Absyn_false_conref,((_tmp1343.bounds=((union Cyc_Absyn_Constraint*(*)(
void*x))Cyc_Absyn_new_conref)((void*)((_tmp1341=_cycalloc(sizeof(*_tmp1341)),((
_tmp1341[0]=((_tmp1342.tag=1,((_tmp1342.f1=(struct Cyc_Absyn_Exp*)_check_null(
_tmpC74),_tmp1342)))),_tmp1341))))),((_tmp1343.zero_term=_tmpC75,((_tmp1343.ptrloc=
0,_tmp1343)))))))))),_tmp1340)))))),_tmp1344)))),_tmp133F))));}return 1;}else{
return 0;}_LL611:;_LL612: return 0;_LL60C:;}int Cyc_Tcutil_is_zero_ptr_deref(struct
Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){void*
_tmpC7E=e1->r;struct Cyc_Absyn_Exp*_tmpC81;struct Cyc_Absyn_Exp*_tmpC83;struct Cyc_Absyn_Exp*
_tmpC85;struct Cyc_Absyn_Exp*_tmpC87;struct Cyc_Absyn_Exp*_tmpC89;struct Cyc_Absyn_Exp*
_tmpC8B;_LL619: {struct Cyc_Absyn_Cast_e_struct*_tmpC7F=(struct Cyc_Absyn_Cast_e_struct*)
_tmpC7E;if(_tmpC7F->tag != 15)goto _LL61B;}_LL61A: {const char*_tmp1348;void*
_tmp1347[1];struct Cyc_String_pa_struct _tmp1346;(_tmp1346.tag=0,((_tmp1346.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((
_tmp1347[0]=& _tmp1346,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(((_tmp1348="we have a cast in a lhs:  %s",_tag_dyneither(
_tmp1348,sizeof(char),29))),_tag_dyneither(_tmp1347,sizeof(void*),1)))))));}
_LL61B: {struct Cyc_Absyn_Deref_e_struct*_tmpC80=(struct Cyc_Absyn_Deref_e_struct*)
_tmpC7E;if(_tmpC80->tag != 22)goto _LL61D;else{_tmpC81=_tmpC80->f1;}}_LL61C:
_tmpC83=_tmpC81;goto _LL61E;_LL61D: {struct Cyc_Absyn_Subscript_e_struct*_tmpC82=(
struct Cyc_Absyn_Subscript_e_struct*)_tmpC7E;if(_tmpC82->tag != 25)goto _LL61F;
else{_tmpC83=_tmpC82->f1;}}_LL61E: return Cyc_Tcutil_is_zero_ptr_type((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpC83->topt))->v,ptr_type,is_dyneither,
elt_type);_LL61F: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpC84=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpC7E;if(_tmpC84->tag != 24)goto _LL621;else{_tmpC85=_tmpC84->f1;}}_LL620:
_tmpC87=_tmpC85;goto _LL622;_LL621: {struct Cyc_Absyn_AggrMember_e_struct*_tmpC86=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpC7E;if(_tmpC86->tag != 23)goto _LL623;
else{_tmpC87=_tmpC86->f1;}}_LL622: if(Cyc_Tcutil_is_zero_ptr_type((void*)((struct
Cyc_Core_Opt*)_check_null(_tmpC87->topt))->v,ptr_type,is_dyneither,elt_type)){
const char*_tmp134C;void*_tmp134B[1];struct Cyc_String_pa_struct _tmp134A;(_tmp134A.tag=
0,((_tmp134A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp134B[0]=& _tmp134A,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp134C="found zero pointer aggregate member assignment: %s",
_tag_dyneither(_tmp134C,sizeof(char),51))),_tag_dyneither(_tmp134B,sizeof(void*),
1)))))));}return 0;_LL623: {struct Cyc_Absyn_Instantiate_e_struct*_tmpC88=(struct
Cyc_Absyn_Instantiate_e_struct*)_tmpC7E;if(_tmpC88->tag != 14)goto _LL625;else{
_tmpC89=_tmpC88->f1;}}_LL624: _tmpC8B=_tmpC89;goto _LL626;_LL625: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpC8A=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpC7E;if(_tmpC8A->tag != 13)
goto _LL627;else{_tmpC8B=_tmpC8A->f1;}}_LL626: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpC8B->topt))->v,ptr_type,is_dyneither,
elt_type)){const char*_tmp1350;void*_tmp134F[1];struct Cyc_String_pa_struct
_tmp134E;(_tmp134E.tag=0,((_tmp134E.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp134F[0]=& _tmp134E,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1350="found zero pointer instantiate/noinstantiate: %s",
_tag_dyneither(_tmp1350,sizeof(char),49))),_tag_dyneither(_tmp134F,sizeof(void*),
1)))))));}return 0;_LL627: {struct Cyc_Absyn_Var_e_struct*_tmpC8C=(struct Cyc_Absyn_Var_e_struct*)
_tmpC7E;if(_tmpC8C->tag != 1)goto _LL629;}_LL628: return 0;_LL629:;_LL62A: {const
char*_tmp1354;void*_tmp1353[1];struct Cyc_String_pa_struct _tmp1352;(_tmp1352.tag=
0,((_tmp1352.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1353[0]=& _tmp1352,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1354="found bad lhs in is_zero_ptr_deref: %s",
_tag_dyneither(_tmp1354,sizeof(char),39))),_tag_dyneither(_tmp1353,sizeof(void*),
1)))))));}_LL618:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t);int Cyc_Tcutil_is_tagged_pointer_typ(
void*t){void*ignore=(void*)& Cyc_Absyn_VoidType_val;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpC99=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpC9C;_LL62C: {
struct Cyc_Absyn_UniqueRgn_struct*_tmpC9A=(struct Cyc_Absyn_UniqueRgn_struct*)
_tmpC99;if(_tmpC9A->tag != 22)goto _LL62E;}_LL62D: return 1;_LL62E: {struct Cyc_Absyn_VarType_struct*
_tmpC9B=(struct Cyc_Absyn_VarType_struct*)_tmpC99;if(_tmpC9B->tag != 2)goto _LL630;
else{_tmpC9C=_tmpC9B->f1;}}_LL62F: if(Cyc_Tcutil_tvar_kind(_tmpC9C,Cyc_Absyn_RgnKind)
== Cyc_Absyn_UniqueRgnKind  || Cyc_Tcutil_tvar_kind(_tmpC9C,Cyc_Absyn_RgnKind)== 
Cyc_Absyn_TopRgnKind){void*_tmpC9D=Cyc_Absyn_compress_kb(_tmpC9C->kind);struct
Cyc_Core_Opt*_tmpC9F;struct Cyc_Core_Opt**_tmpCA0;enum Cyc_Absyn_Kind _tmpCA1;
_LL633: {struct Cyc_Absyn_Less_kb_struct*_tmpC9E=(struct Cyc_Absyn_Less_kb_struct*)
_tmpC9D;if(_tmpC9E->tag != 2)goto _LL635;else{_tmpC9F=_tmpC9E->f1;_tmpCA0=(struct
Cyc_Core_Opt**)& _tmpC9E->f1;_tmpCA1=_tmpC9E->f2;if(_tmpCA1 != Cyc_Absyn_TopRgnKind)
goto _LL635;}}_LL634:{struct Cyc_Absyn_Less_kb_struct*_tmp135A;struct Cyc_Absyn_Less_kb_struct
_tmp1359;struct Cyc_Core_Opt*_tmp1358;*_tmpCA0=((_tmp1358=_cycalloc(sizeof(*
_tmp1358)),((_tmp1358->v=(void*)((_tmp135A=_cycalloc(sizeof(*_tmp135A)),((
_tmp135A[0]=((_tmp1359.tag=2,((_tmp1359.f1=0,((_tmp1359.f2=Cyc_Absyn_RgnKind,
_tmp1359)))))),_tmp135A)))),_tmp1358))));}return 0;_LL635:;_LL636: return 1;_LL632:;}
return 0;_LL630:;_LL631: return 0;_LL62B:;}int Cyc_Tcutil_is_noalias_pointer(void*t);
int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpCA5=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpCA7;struct Cyc_Absyn_PtrAtts _tmpCA8;void*_tmpCA9;
_LL638: {struct Cyc_Absyn_PointerType_struct*_tmpCA6=(struct Cyc_Absyn_PointerType_struct*)
_tmpCA5;if(_tmpCA6->tag != 5)goto _LL63A;else{_tmpCA7=_tmpCA6->f1;_tmpCA8=_tmpCA7.ptr_atts;
_tmpCA9=_tmpCA8.rgn;}}_LL639: return Cyc_Tcutil_is_noalias_region(_tmpCA9);_LL63A:;
_LL63B: return 0;_LL637:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct
_RegionHandle*rgn,void*t){void*_tmpCAA=Cyc_Tcutil_compress(t);if(Cyc_Tcutil_is_noalias_pointer(
_tmpCAA))return 1;{void*_tmpCAB=_tmpCAA;struct Cyc_List_List*_tmpCAD;struct Cyc_Absyn_AggrInfo
_tmpCAF;union Cyc_Absyn_AggrInfoU _tmpCB0;struct Cyc_Absyn_Aggrdecl**_tmpCB1;struct
Cyc_List_List*_tmpCB2;struct Cyc_List_List*_tmpCB4;struct Cyc_Absyn_AggrInfo
_tmpCB6;union Cyc_Absyn_AggrInfoU _tmpCB7;struct _tuple4 _tmpCB8;struct Cyc_Absyn_DatatypeInfo
_tmpCBA;union Cyc_Absyn_DatatypeInfoU _tmpCBB;struct Cyc_List_List*_tmpCBC;struct
Cyc_Absyn_DatatypeFieldInfo _tmpCBE;union Cyc_Absyn_DatatypeFieldInfoU _tmpCBF;
struct Cyc_List_List*_tmpCC0;_LL63D: {struct Cyc_Absyn_TupleType_struct*_tmpCAC=(
struct Cyc_Absyn_TupleType_struct*)_tmpCAB;if(_tmpCAC->tag != 11)goto _LL63F;else{
_tmpCAD=_tmpCAC->f1;}}_LL63E: while(_tmpCAD != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,(*((struct _tuple11*)_tmpCAD->hd)).f2))return 1;_tmpCAD=_tmpCAD->tl;}return 0;
_LL63F: {struct Cyc_Absyn_AggrType_struct*_tmpCAE=(struct Cyc_Absyn_AggrType_struct*)
_tmpCAB;if(_tmpCAE->tag != 12)goto _LL641;else{_tmpCAF=_tmpCAE->f1;_tmpCB0=_tmpCAF.aggr_info;
if((_tmpCB0.KnownAggr).tag != 2)goto _LL641;_tmpCB1=(struct Cyc_Absyn_Aggrdecl**)(
_tmpCB0.KnownAggr).val;_tmpCB2=_tmpCAF.targs;}}_LL640: if((*_tmpCB1)->impl == 0)
return 0;else{struct Cyc_List_List*_tmpCC1=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmpCB1)->tvs,_tmpCB2);struct Cyc_List_List*_tmpCC2=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpCB1)->impl))->fields;void*t;
while(_tmpCC2 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpCC1,((struct Cyc_Absyn_Aggrfield*)
_tmpCC2->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpCC2=_tmpCC2->tl;}return 0;}_LL641: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpCB3=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpCAB;if(_tmpCB3->tag != 13)goto
_LL643;else{_tmpCB4=_tmpCB3->f2;}}_LL642: while(_tmpCB4 != 0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
rgn,((struct Cyc_Absyn_Aggrfield*)_tmpCB4->hd)->type))return 1;_tmpCB4=_tmpCB4->tl;}
return 0;_LL643: {struct Cyc_Absyn_AggrType_struct*_tmpCB5=(struct Cyc_Absyn_AggrType_struct*)
_tmpCAB;if(_tmpCB5->tag != 12)goto _LL645;else{_tmpCB6=_tmpCB5->f1;_tmpCB7=_tmpCB6.aggr_info;
if((_tmpCB7.UnknownAggr).tag != 1)goto _LL645;_tmpCB8=(struct _tuple4)(_tmpCB7.UnknownAggr).val;}}
_LL644: {const char*_tmp135D;void*_tmp135C;(_tmp135C=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp135D="got unknown aggr in is_noalias_aggr",
_tag_dyneither(_tmp135D,sizeof(char),36))),_tag_dyneither(_tmp135C,sizeof(void*),
0)));}_LL645: {struct Cyc_Absyn_DatatypeType_struct*_tmpCB9=(struct Cyc_Absyn_DatatypeType_struct*)
_tmpCAB;if(_tmpCB9->tag != 3)goto _LL647;else{_tmpCBA=_tmpCB9->f1;_tmpCBB=_tmpCBA.datatype_info;
_tmpCBC=_tmpCBA.targs;}}_LL646: {union Cyc_Absyn_DatatypeInfoU _tmpCC5=_tmpCBB;
struct Cyc_Absyn_UnknownDatatypeInfo _tmpCC6;struct _tuple2*_tmpCC7;int _tmpCC8;
struct Cyc_Absyn_Datatypedecl**_tmpCC9;struct Cyc_Absyn_Datatypedecl*_tmpCCA;
struct Cyc_Absyn_Datatypedecl _tmpCCB;struct Cyc_List_List*_tmpCCC;struct Cyc_Core_Opt*
_tmpCCD;_LL64C: if((_tmpCC5.UnknownDatatype).tag != 1)goto _LL64E;_tmpCC6=(struct
Cyc_Absyn_UnknownDatatypeInfo)(_tmpCC5.UnknownDatatype).val;_tmpCC7=_tmpCC6.name;
_tmpCC8=_tmpCC6.is_extensible;_LL64D: {const char*_tmp1360;void*_tmp135F;(
_tmp135F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1360="got unknown datatype in is_noalias_aggr",_tag_dyneither(_tmp1360,
sizeof(char),40))),_tag_dyneither(_tmp135F,sizeof(void*),0)));}_LL64E: if((
_tmpCC5.KnownDatatype).tag != 2)goto _LL64B;_tmpCC9=(struct Cyc_Absyn_Datatypedecl**)(
_tmpCC5.KnownDatatype).val;_tmpCCA=*_tmpCC9;_tmpCCB=*_tmpCCA;_tmpCCC=_tmpCCB.tvs;
_tmpCCD=_tmpCCB.fields;_LL64F: return 0;_LL64B:;}_LL647: {struct Cyc_Absyn_DatatypeFieldType_struct*
_tmpCBD=(struct Cyc_Absyn_DatatypeFieldType_struct*)_tmpCAB;if(_tmpCBD->tag != 4)
goto _LL649;else{_tmpCBE=_tmpCBD->f1;_tmpCBF=_tmpCBE.field_info;_tmpCC0=_tmpCBE.targs;}}
_LL648: {union Cyc_Absyn_DatatypeFieldInfoU _tmpCD0=_tmpCBF;struct Cyc_Absyn_UnknownDatatypeFieldInfo
_tmpCD1;struct _tuple3 _tmpCD2;struct Cyc_Absyn_Datatypedecl*_tmpCD3;struct Cyc_Absyn_Datatypefield*
_tmpCD4;_LL651: if((_tmpCD0.UnknownDatatypefield).tag != 1)goto _LL653;_tmpCD1=(
struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpCD0.UnknownDatatypefield).val;
_LL652: {const char*_tmp1363;void*_tmp1362;(_tmp1362=0,((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1363="got unknown datatype field in is_noalias_aggr",
_tag_dyneither(_tmp1363,sizeof(char),46))),_tag_dyneither(_tmp1362,sizeof(void*),
0)));}_LL653: if((_tmpCD0.KnownDatatypefield).tag != 2)goto _LL650;_tmpCD2=(struct
_tuple3)(_tmpCD0.KnownDatatypefield).val;_tmpCD3=_tmpCD2.f1;_tmpCD4=_tmpCD2.f2;
_LL654: {struct Cyc_List_List*_tmpCD7=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpCD3->tvs,_tmpCC0);struct Cyc_List_List*_tmpCD8=
_tmpCD4->typs;while(_tmpCD8 != 0){_tmpCAA=Cyc_Tcutil_rsubstitute(rgn,_tmpCD7,(*((
struct _tuple11*)_tmpCD8->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,
_tmpCAA))return 1;_tmpCD8=_tmpCD8->tl;}return 0;}_LL650:;}_LL649:;_LL64A: return 0;
_LL63C:;};}static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,
struct _dyneither_ptr*f);static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*
r,void*t,struct _dyneither_ptr*f){void*_tmpCD9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpCDB;union Cyc_Absyn_AggrInfoU _tmpCDC;struct Cyc_Absyn_Aggrdecl**_tmpCDD;struct
Cyc_Absyn_Aggrdecl*_tmpCDE;struct Cyc_List_List*_tmpCDF;struct Cyc_List_List*
_tmpCE1;_LL656: {struct Cyc_Absyn_AggrType_struct*_tmpCDA=(struct Cyc_Absyn_AggrType_struct*)
_tmpCD9;if(_tmpCDA->tag != 12)goto _LL658;else{_tmpCDB=_tmpCDA->f1;_tmpCDC=_tmpCDB.aggr_info;
if((_tmpCDC.KnownAggr).tag != 2)goto _LL658;_tmpCDD=(struct Cyc_Absyn_Aggrdecl**)(
_tmpCDC.KnownAggr).val;_tmpCDE=*_tmpCDD;_tmpCDF=_tmpCDB.targs;}}_LL657: _tmpCE1=
_tmpCDE->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCDE->impl))->fields;
goto _LL659;_LL658: {struct Cyc_Absyn_AnonAggrType_struct*_tmpCE0=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpCD9;if(_tmpCE0->tag != 13)goto _LL65A;else{_tmpCE1=_tmpCE0->f2;}}_LL659: {
struct Cyc_Absyn_Aggrfield*_tmpCE2=Cyc_Absyn_lookup_field(_tmpCE1,f);{struct Cyc_Absyn_Aggrfield*
_tmpCE3=_tmpCE2;struct Cyc_Absyn_Aggrfield _tmpCE4;void*_tmpCE5;_LL65D: if(_tmpCE3
!= 0)goto _LL65F;_LL65E: {const char*_tmp1366;void*_tmp1365;(_tmp1365=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1366="is_noalias_field: missing field",
_tag_dyneither(_tmp1366,sizeof(char),32))),_tag_dyneither(_tmp1365,sizeof(void*),
0)));}_LL65F: if(_tmpCE3 == 0)goto _LL65C;_tmpCE4=*_tmpCE3;_tmpCE5=_tmpCE4.type;
_LL660: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpCE5);_LL65C:;}return 0;}
_LL65A:;_LL65B: {const char*_tmp136A;void*_tmp1369[1];struct Cyc_String_pa_struct
_tmp1368;(_tmp1368.tag=0,((_tmp1368.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1369[0]=& _tmp1368,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp136A="is_noalias_field: invalid type |%s|",
_tag_dyneither(_tmp136A,sizeof(char),36))),_tag_dyneither(_tmp1369,sizeof(void*),
1)))))));}_LL655:;}static int Cyc_Tcutil_is_noalias_path_aux(struct _RegionHandle*r,
struct Cyc_Absyn_Exp*e,int ignore_leaf);static int Cyc_Tcutil_is_noalias_path_aux(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e,int ignore_leaf){void*_tmpCEB=e->r;
void*_tmpCFC;struct Cyc_Absyn_Exp*_tmpD02;struct _dyneither_ptr*_tmpD03;struct Cyc_Absyn_Exp*
_tmpD05;struct Cyc_Absyn_Exp*_tmpD06;void*_tmpD0F;void*_tmpD12;void*_tmpD15;
struct Cyc_Absyn_Exp*_tmpD18;struct Cyc_Absyn_Exp*_tmpD1A;struct Cyc_Absyn_Exp*
_tmpD1C;struct Cyc_Absyn_Exp*_tmpD1E;void*_tmpD20;struct Cyc_Absyn_Exp*_tmpD21;
struct Cyc_Absyn_Stmt*_tmpD23;_LL662: {struct Cyc_Absyn_Increment_e_struct*_tmpCEC=(
struct Cyc_Absyn_Increment_e_struct*)_tmpCEB;if(_tmpCEC->tag != 5)goto _LL664;}
_LL663: goto _LL665;_LL664: {struct Cyc_Absyn_And_e_struct*_tmpCED=(struct Cyc_Absyn_And_e_struct*)
_tmpCEB;if(_tmpCED->tag != 7)goto _LL666;}_LL665: goto _LL667;_LL666: {struct Cyc_Absyn_Or_e_struct*
_tmpCEE=(struct Cyc_Absyn_Or_e_struct*)_tmpCEB;if(_tmpCEE->tag != 8)goto _LL668;}
_LL667: goto _LL669;_LL668: {struct Cyc_Absyn_Throw_e_struct*_tmpCEF=(struct Cyc_Absyn_Throw_e_struct*)
_tmpCEB;if(_tmpCEF->tag != 12)goto _LL66A;}_LL669: goto _LL66B;_LL66A: {struct Cyc_Absyn_Address_e_struct*
_tmpCF0=(struct Cyc_Absyn_Address_e_struct*)_tmpCEB;if(_tmpCF0->tag != 16)goto
_LL66C;}_LL66B: goto _LL66D;_LL66C: {struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpCF1=(
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpCEB;if(_tmpCF1->tag != 18)goto _LL66E;}
_LL66D: goto _LL66F;_LL66E: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpCF2=(struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpCEB;if(_tmpCF2->tag != 19)goto _LL670;}_LL66F: goto _LL671;_LL670: {struct Cyc_Absyn_Offsetof_e_struct*
_tmpCF3=(struct Cyc_Absyn_Offsetof_e_struct*)_tmpCEB;if(_tmpCF3->tag != 20)goto
_LL672;}_LL671: goto _LL673;_LL672: {struct Cyc_Absyn_Gentyp_e_struct*_tmpCF4=(
struct Cyc_Absyn_Gentyp_e_struct*)_tmpCEB;if(_tmpCF4->tag != 21)goto _LL674;}_LL673:
goto _LL675;_LL674: {struct Cyc_Absyn_CompoundLit_e_struct*_tmpCF5=(struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpCEB;if(_tmpCF5->tag != 27)goto _LL676;}_LL675: goto _LL677;_LL676: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpCF6=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpCEB;if(_tmpCF6->tag != 29)
goto _LL678;}_LL677: goto _LL679;_LL678: {struct Cyc_Absyn_Array_e_struct*_tmpCF7=(
struct Cyc_Absyn_Array_e_struct*)_tmpCEB;if(_tmpCF7->tag != 28)goto _LL67A;}_LL679:
goto _LL67B;_LL67A: {struct Cyc_Absyn_Enum_e_struct*_tmpCF8=(struct Cyc_Absyn_Enum_e_struct*)
_tmpCEB;if(_tmpCF8->tag != 33)goto _LL67C;}_LL67B: goto _LL67D;_LL67C: {struct Cyc_Absyn_AnonEnum_e_struct*
_tmpCF9=(struct Cyc_Absyn_AnonEnum_e_struct*)_tmpCEB;if(_tmpCF9->tag != 34)goto
_LL67E;}_LL67D: goto _LL67F;_LL67E: {struct Cyc_Absyn_Swap_e_struct*_tmpCFA=(struct
Cyc_Absyn_Swap_e_struct*)_tmpCEB;if(_tmpCFA->tag != 36)goto _LL680;}_LL67F: goto
_LL681;_LL680: {struct Cyc_Absyn_Var_e_struct*_tmpCFB=(struct Cyc_Absyn_Var_e_struct*)
_tmpCEB;if(_tmpCFB->tag != 1)goto _LL682;else{_tmpCFC=(void*)_tmpCFB->f2;{struct
Cyc_Absyn_Global_b_struct*_tmpCFD=(struct Cyc_Absyn_Global_b_struct*)_tmpCFC;if(
_tmpCFD->tag != 1)goto _LL682;};}}_LL681: goto _LL683;_LL682: {struct Cyc_Absyn_Primop_e_struct*
_tmpCFE=(struct Cyc_Absyn_Primop_e_struct*)_tmpCEB;if(_tmpCFE->tag != 3)goto _LL684;}
_LL683: return 0;_LL684: {struct Cyc_Absyn_Deref_e_struct*_tmpCFF=(struct Cyc_Absyn_Deref_e_struct*)
_tmpCEB;if(_tmpCFF->tag != 22)goto _LL686;}_LL685: goto _LL687;_LL686: {struct Cyc_Absyn_AggrArrow_e_struct*
_tmpD00=(struct Cyc_Absyn_AggrArrow_e_struct*)_tmpCEB;if(_tmpD00->tag != 24)goto
_LL688;}_LL687: return 0;_LL688: {struct Cyc_Absyn_AggrMember_e_struct*_tmpD01=(
struct Cyc_Absyn_AggrMember_e_struct*)_tmpCEB;if(_tmpD01->tag != 23)goto _LL68A;
else{_tmpD02=_tmpD01->f1;_tmpD03=_tmpD01->f2;}}_LL689: return(ignore_leaf  || Cyc_Tcutil_is_noalias_field(
r,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD02->topt))->v,_tmpD03)) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpD02,0);_LL68A: {struct Cyc_Absyn_Subscript_e_struct*_tmpD04=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpCEB;if(_tmpD04->tag != 25)goto _LL68C;else{_tmpD05=_tmpD04->f1;_tmpD06=_tmpD04->f2;}}
_LL68B: {void*_tmpD24=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpD05->topt))->v);struct Cyc_Absyn_PtrInfo _tmpD26;void*_tmpD27;
struct Cyc_List_List*_tmpD29;_LL6AF: {struct Cyc_Absyn_PointerType_struct*_tmpD25=(
struct Cyc_Absyn_PointerType_struct*)_tmpD24;if(_tmpD25->tag != 5)goto _LL6B1;else{
_tmpD26=_tmpD25->f1;_tmpD27=_tmpD26.elt_typ;}}_LL6B0: return 0;_LL6B1: {struct Cyc_Absyn_TupleType_struct*
_tmpD28=(struct Cyc_Absyn_TupleType_struct*)_tmpD24;if(_tmpD28->tag != 11)goto
_LL6B3;else{_tmpD29=_tmpD28->f1;}}_LL6B2: {unsigned int _tmpD2B;int _tmpD2C;struct
_tuple13 _tmpD2A=Cyc_Evexp_eval_const_uint_exp(_tmpD06);_tmpD2B=_tmpD2A.f1;
_tmpD2C=_tmpD2A.f2;if(!_tmpD2C){const char*_tmp136D;void*_tmp136C;(_tmp136C=0,((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp136D="is_noalias_path_aux: non-constant subscript",_tag_dyneither(_tmp136D,
sizeof(char),44))),_tag_dyneither(_tmp136C,sizeof(void*),0)));}{struct
_handler_cons _tmpD2F;_push_handler(& _tmpD2F);{int _tmpD31=0;if(setjmp(_tmpD2F.handler))
_tmpD31=1;if(!_tmpD31){{void*_tmpD32=(*((struct _tuple11*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmpD29,(int)_tmpD2B)).f2;int _tmpD33=(ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
r,_tmpD32)) && Cyc_Tcutil_is_noalias_path_aux(r,_tmpD05,0);_npop_handler(0);
return _tmpD33;};_pop_handler();}else{void*_tmpD30=(void*)_exn_thrown;void*
_tmpD35=_tmpD30;_LL6B6: {struct Cyc_List_Nth_struct*_tmpD36=(struct Cyc_List_Nth_struct*)
_tmpD35;if(_tmpD36->tag != Cyc_List_Nth)goto _LL6B8;}_LL6B7: {const char*_tmp1370;
void*_tmp136F;return(_tmp136F=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1370="is_noalias_path_aux: out-of-bounds subscript",
_tag_dyneither(_tmp1370,sizeof(char),45))),_tag_dyneither(_tmp136F,sizeof(void*),
0)));}_LL6B8:;_LL6B9:(void)_throw(_tmpD35);_LL6B5:;}};};}_LL6B3:;_LL6B4: {const
char*_tmp1373;void*_tmp1372;(_tmp1372=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1373="is_noalias_path_aux: subscript on non-pointer/tuple",
_tag_dyneither(_tmp1373,sizeof(char),52))),_tag_dyneither(_tmp1372,sizeof(void*),
0)));}_LL6AE:;}_LL68C: {struct Cyc_Absyn_Datatype_e_struct*_tmpD07=(struct Cyc_Absyn_Datatype_e_struct*)
_tmpCEB;if(_tmpD07->tag != 32)goto _LL68E;}_LL68D: goto _LL68F;_LL68E: {struct Cyc_Absyn_Tuple_e_struct*
_tmpD08=(struct Cyc_Absyn_Tuple_e_struct*)_tmpCEB;if(_tmpD08->tag != 26)goto _LL690;}
_LL68F: goto _LL691;_LL690: {struct Cyc_Absyn_Aggregate_e_struct*_tmpD09=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpCEB;if(_tmpD09->tag != 30)goto _LL692;}_LL691: goto _LL693;_LL692: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpD0A=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpCEB;if(_tmpD0A->tag != 31)goto
_LL694;}_LL693: goto _LL695;_LL694: {struct Cyc_Absyn_Const_e_struct*_tmpD0B=(
struct Cyc_Absyn_Const_e_struct*)_tmpCEB;if(_tmpD0B->tag != 0)goto _LL696;}_LL695:
goto _LL697;_LL696: {struct Cyc_Absyn_Malloc_e_struct*_tmpD0C=(struct Cyc_Absyn_Malloc_e_struct*)
_tmpCEB;if(_tmpD0C->tag != 35)goto _LL698;}_LL697: goto _LL699;_LL698: {struct Cyc_Absyn_New_e_struct*
_tmpD0D=(struct Cyc_Absyn_New_e_struct*)_tmpCEB;if(_tmpD0D->tag != 17)goto _LL69A;}
_LL699: goto _LL69B;_LL69A: {struct Cyc_Absyn_Var_e_struct*_tmpD0E=(struct Cyc_Absyn_Var_e_struct*)
_tmpCEB;if(_tmpD0E->tag != 1)goto _LL69C;else{_tmpD0F=(void*)_tmpD0E->f2;{struct
Cyc_Absyn_Local_b_struct*_tmpD10=(struct Cyc_Absyn_Local_b_struct*)_tmpD0F;if(
_tmpD10->tag != 4)goto _LL69C;};}}_LL69B: goto _LL69D;_LL69C: {struct Cyc_Absyn_Var_e_struct*
_tmpD11=(struct Cyc_Absyn_Var_e_struct*)_tmpCEB;if(_tmpD11->tag != 1)goto _LL69E;
else{_tmpD12=(void*)_tmpD11->f2;{struct Cyc_Absyn_Pat_b_struct*_tmpD13=(struct Cyc_Absyn_Pat_b_struct*)
_tmpD12;if(_tmpD13->tag != 5)goto _LL69E;};}}_LL69D: goto _LL69F;_LL69E: {struct Cyc_Absyn_Var_e_struct*
_tmpD14=(struct Cyc_Absyn_Var_e_struct*)_tmpCEB;if(_tmpD14->tag != 1)goto _LL6A0;
else{_tmpD15=(void*)_tmpD14->f2;{struct Cyc_Absyn_Param_b_struct*_tmpD16=(struct
Cyc_Absyn_Param_b_struct*)_tmpD15;if(_tmpD16->tag != 3)goto _LL6A0;};}}_LL69F: {
int _tmpD3B=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);return _tmpD3B;}_LL6A0: {struct Cyc_Absyn_Conditional_e_struct*
_tmpD17=(struct Cyc_Absyn_Conditional_e_struct*)_tmpCEB;if(_tmpD17->tag != 6)goto
_LL6A2;else{_tmpD18=_tmpD17->f2;}}_LL6A1: _tmpD1A=_tmpD18;goto _LL6A3;_LL6A2: {
struct Cyc_Absyn_SeqExp_e_struct*_tmpD19=(struct Cyc_Absyn_SeqExp_e_struct*)
_tmpCEB;if(_tmpD19->tag != 9)goto _LL6A4;else{_tmpD1A=_tmpD19->f2;}}_LL6A3: _tmpD1C=
_tmpD1A;goto _LL6A5;_LL6A4: {struct Cyc_Absyn_AssignOp_e_struct*_tmpD1B=(struct Cyc_Absyn_AssignOp_e_struct*)
_tmpCEB;if(_tmpD1B->tag != 4)goto _LL6A6;else{_tmpD1C=_tmpD1B->f1;}}_LL6A5: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpD1C,ignore_leaf);_LL6A6: {struct Cyc_Absyn_FnCall_e_struct*
_tmpD1D=(struct Cyc_Absyn_FnCall_e_struct*)_tmpCEB;if(_tmpD1D->tag != 11)goto
_LL6A8;else{_tmpD1E=_tmpD1D->f1;}}_LL6A7: {void*_tmpD3C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD1E->topt))->v);struct Cyc_Absyn_FnInfo
_tmpD3E;void*_tmpD3F;struct Cyc_Absyn_PtrInfo _tmpD41;void*_tmpD42;_LL6BB: {struct
Cyc_Absyn_FnType_struct*_tmpD3D=(struct Cyc_Absyn_FnType_struct*)_tmpD3C;if(
_tmpD3D->tag != 10)goto _LL6BD;else{_tmpD3E=_tmpD3D->f1;_tmpD3F=_tmpD3E.ret_typ;}}
_LL6BC: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD3F);
_LL6BD: {struct Cyc_Absyn_PointerType_struct*_tmpD40=(struct Cyc_Absyn_PointerType_struct*)
_tmpD3C;if(_tmpD40->tag != 5)goto _LL6BF;else{_tmpD41=_tmpD40->f1;_tmpD42=_tmpD41.elt_typ;}}
_LL6BE:{void*_tmpD43=Cyc_Tcutil_compress(_tmpD42);struct Cyc_Absyn_FnInfo _tmpD45;
void*_tmpD46;_LL6C2: {struct Cyc_Absyn_FnType_struct*_tmpD44=(struct Cyc_Absyn_FnType_struct*)
_tmpD43;if(_tmpD44->tag != 10)goto _LL6C4;else{_tmpD45=_tmpD44->f1;_tmpD46=_tmpD45.ret_typ;}}
_LL6C3: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD46);
_LL6C4:;_LL6C5: goto _LL6C1;_LL6C1:;}goto _LL6C0;_LL6BF:;_LL6C0: {const char*
_tmp1377;void*_tmp1376[1];struct Cyc_String_pa_struct _tmp1375;(_tmp1375.tag=0,((
_tmp1375.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD1E->topt))->v)),((_tmp1376[0]=&
_tmp1375,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((
_tmp1377="Fncall has non function type %s",_tag_dyneither(_tmp1377,sizeof(char),
32))),_tag_dyneither(_tmp1376,sizeof(void*),1)))))));}_LL6BA:;}_LL6A8: {struct
Cyc_Absyn_Cast_e_struct*_tmpD1F=(struct Cyc_Absyn_Cast_e_struct*)_tmpCEB;if(
_tmpD1F->tag != 15)goto _LL6AA;else{_tmpD20=(void*)_tmpD1F->f1;_tmpD21=_tmpD1F->f2;}}
_LL6A9: return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD20) && Cyc_Tcutil_is_noalias_path_aux(
r,_tmpD21,1);_LL6AA: {struct Cyc_Absyn_StmtExp_e_struct*_tmpD22=(struct Cyc_Absyn_StmtExp_e_struct*)
_tmpCEB;if(_tmpD22->tag != 38)goto _LL6AC;else{_tmpD23=_tmpD22->f1;}}_LL6AB: while(
1){void*_tmpD4A=_tmpD23->r;struct Cyc_Absyn_Stmt*_tmpD4C;struct Cyc_Absyn_Stmt*
_tmpD4D;struct Cyc_Absyn_Decl*_tmpD4F;struct Cyc_Absyn_Stmt*_tmpD50;struct Cyc_Absyn_Exp*
_tmpD52;_LL6C7: {struct Cyc_Absyn_Seq_s_struct*_tmpD4B=(struct Cyc_Absyn_Seq_s_struct*)
_tmpD4A;if(_tmpD4B->tag != 2)goto _LL6C9;else{_tmpD4C=_tmpD4B->f1;_tmpD4D=_tmpD4B->f2;}}
_LL6C8: _tmpD23=_tmpD4D;goto _LL6C6;_LL6C9: {struct Cyc_Absyn_Decl_s_struct*_tmpD4E=(
struct Cyc_Absyn_Decl_s_struct*)_tmpD4A;if(_tmpD4E->tag != 12)goto _LL6CB;else{
_tmpD4F=_tmpD4E->f1;_tmpD50=_tmpD4E->f2;}}_LL6CA: _tmpD23=_tmpD50;goto _LL6C6;
_LL6CB: {struct Cyc_Absyn_Exp_s_struct*_tmpD51=(struct Cyc_Absyn_Exp_s_struct*)
_tmpD4A;if(_tmpD51->tag != 1)goto _LL6CD;else{_tmpD52=_tmpD51->f1;}}_LL6CC: return
Cyc_Tcutil_is_noalias_path_aux(r,_tmpD52,ignore_leaf);_LL6CD:;_LL6CE: {const char*
_tmp137A;void*_tmp1379;(_tmp1379=0,((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp137A="is_noalias_stmt_exp: ill-formed StmtExp",
_tag_dyneither(_tmp137A,sizeof(char),40))),_tag_dyneither(_tmp1379,sizeof(void*),
0)));}_LL6C6:;}_LL6AC:;_LL6AD: return 0;_LL661:;}int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(
struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
r,e,0);}struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);struct _tuple16 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){struct _tuple16 _tmp137B;struct _tuple16 bogus_ans=(
_tmp137B.f1=0,((_tmp137B.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp137B)));void*
_tmpD55=e->r;struct _tuple2*_tmpD57;void*_tmpD58;struct Cyc_Absyn_Exp*_tmpD5A;
struct _dyneither_ptr*_tmpD5B;int _tmpD5C;struct Cyc_Absyn_Exp*_tmpD5E;struct
_dyneither_ptr*_tmpD5F;int _tmpD60;struct Cyc_Absyn_Exp*_tmpD62;struct Cyc_Absyn_Exp*
_tmpD64;struct Cyc_Absyn_Exp*_tmpD65;_LL6D0: {struct Cyc_Absyn_Var_e_struct*
_tmpD56=(struct Cyc_Absyn_Var_e_struct*)_tmpD55;if(_tmpD56->tag != 1)goto _LL6D2;
else{_tmpD57=_tmpD56->f1;_tmpD58=(void*)_tmpD56->f2;}}_LL6D1: {void*_tmpD66=
_tmpD58;struct Cyc_Absyn_Vardecl*_tmpD6A;struct Cyc_Absyn_Vardecl*_tmpD6C;struct
Cyc_Absyn_Vardecl*_tmpD6E;struct Cyc_Absyn_Vardecl*_tmpD70;_LL6DD: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpD67=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpD66;if(_tmpD67->tag != 0)goto
_LL6DF;}_LL6DE: goto _LL6E0;_LL6DF: {struct Cyc_Absyn_Funname_b_struct*_tmpD68=(
struct Cyc_Absyn_Funname_b_struct*)_tmpD66;if(_tmpD68->tag != 2)goto _LL6E1;}_LL6E0:
return bogus_ans;_LL6E1: {struct Cyc_Absyn_Global_b_struct*_tmpD69=(struct Cyc_Absyn_Global_b_struct*)
_tmpD66;if(_tmpD69->tag != 1)goto _LL6E3;else{_tmpD6A=_tmpD69->f1;}}_LL6E2: {void*
_tmpD71=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL6EA: {struct Cyc_Absyn_ArrayType_struct*_tmpD72=(struct Cyc_Absyn_ArrayType_struct*)
_tmpD71;if(_tmpD72->tag != 9)goto _LL6EC;}_LL6EB: {struct _tuple16 _tmp137C;return(
_tmp137C.f1=1,((_tmp137C.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp137C)));}_LL6EC:;
_LL6ED: {struct _tuple16 _tmp137D;return(_tmp137D.f1=(_tmpD6A->tq).real_const,((
_tmp137D.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp137D)));}_LL6E9:;}_LL6E3: {struct
Cyc_Absyn_Local_b_struct*_tmpD6B=(struct Cyc_Absyn_Local_b_struct*)_tmpD66;if(
_tmpD6B->tag != 4)goto _LL6E5;else{_tmpD6C=_tmpD6B->f1;}}_LL6E4: {void*_tmpD75=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL6EF: {struct Cyc_Absyn_ArrayType_struct*
_tmpD76=(struct Cyc_Absyn_ArrayType_struct*)_tmpD75;if(_tmpD76->tag != 9)goto
_LL6F1;}_LL6F0: {struct _tuple16 _tmp137E;return(_tmp137E.f1=1,((_tmp137E.f2=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmpD6C->rgn))->v,_tmp137E)));}_LL6F1:;_LL6F2:
_tmpD6C->escapes=1;{struct _tuple16 _tmp137F;return(_tmp137F.f1=(_tmpD6C->tq).real_const,((
_tmp137F.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpD6C->rgn))->v,_tmp137F)));};
_LL6EE:;}_LL6E5: {struct Cyc_Absyn_Pat_b_struct*_tmpD6D=(struct Cyc_Absyn_Pat_b_struct*)
_tmpD66;if(_tmpD6D->tag != 5)goto _LL6E7;else{_tmpD6E=_tmpD6D->f1;}}_LL6E6: _tmpD70=
_tmpD6E;goto _LL6E8;_LL6E7: {struct Cyc_Absyn_Param_b_struct*_tmpD6F=(struct Cyc_Absyn_Param_b_struct*)
_tmpD66;if(_tmpD6F->tag != 3)goto _LL6DC;else{_tmpD70=_tmpD6F->f1;}}_LL6E8: _tmpD70->escapes=
1;{struct _tuple16 _tmp1380;return(_tmp1380.f1=(_tmpD70->tq).real_const,((_tmp1380.f2=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD70->rgn))->v,_tmp1380)));};_LL6DC:;}
_LL6D2: {struct Cyc_Absyn_AggrMember_e_struct*_tmpD59=(struct Cyc_Absyn_AggrMember_e_struct*)
_tmpD55;if(_tmpD59->tag != 23)goto _LL6D4;else{_tmpD5A=_tmpD59->f1;_tmpD5B=_tmpD59->f2;
_tmpD5C=_tmpD59->f3;}}_LL6D3: if(_tmpD5C)return bogus_ans;{void*_tmpD7A=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD5A->topt))->v);struct Cyc_List_List*
_tmpD7C;struct Cyc_Absyn_AggrInfo _tmpD7E;union Cyc_Absyn_AggrInfoU _tmpD7F;struct
Cyc_Absyn_Aggrdecl**_tmpD80;struct Cyc_Absyn_Aggrdecl*_tmpD81;_LL6F4: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpD7B=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpD7A;if(_tmpD7B->tag != 13)goto
_LL6F6;else{_tmpD7C=_tmpD7B->f2;}}_LL6F5: {struct Cyc_Absyn_Aggrfield*_tmpD82=Cyc_Absyn_lookup_field(
_tmpD7C,_tmpD5B);if(_tmpD82 != 0  && _tmpD82->width != 0){struct _tuple16 _tmp1381;
return(_tmp1381.f1=(_tmpD82->tq).real_const,((_tmp1381.f2=(Cyc_Tcutil_addressof_props(
te,_tmpD5A)).f2,_tmp1381)));}return bogus_ans;}_LL6F6: {struct Cyc_Absyn_AggrType_struct*
_tmpD7D=(struct Cyc_Absyn_AggrType_struct*)_tmpD7A;if(_tmpD7D->tag != 12)goto
_LL6F8;else{_tmpD7E=_tmpD7D->f1;_tmpD7F=_tmpD7E.aggr_info;if((_tmpD7F.KnownAggr).tag
!= 2)goto _LL6F8;_tmpD80=(struct Cyc_Absyn_Aggrdecl**)(_tmpD7F.KnownAggr).val;
_tmpD81=*_tmpD80;}}_LL6F7: {struct Cyc_Absyn_Aggrfield*_tmpD84=Cyc_Absyn_lookup_decl_field(
_tmpD81,_tmpD5B);if(_tmpD84 != 0  && _tmpD84->width != 0){struct _tuple16 _tmp1382;
return(_tmp1382.f1=(_tmpD84->tq).real_const,((_tmp1382.f2=(Cyc_Tcutil_addressof_props(
te,_tmpD5A)).f2,_tmp1382)));}return bogus_ans;}_LL6F8:;_LL6F9: return bogus_ans;
_LL6F3:;};_LL6D4: {struct Cyc_Absyn_AggrArrow_e_struct*_tmpD5D=(struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpD55;if(_tmpD5D->tag != 24)goto _LL6D6;else{_tmpD5E=_tmpD5D->f1;_tmpD5F=_tmpD5D->f2;
_tmpD60=_tmpD5D->f3;}}_LL6D5: if(_tmpD60)return bogus_ans;{void*_tmpD86=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpD5E->topt))->v);struct Cyc_Absyn_PtrInfo
_tmpD88;void*_tmpD89;struct Cyc_Absyn_PtrAtts _tmpD8A;void*_tmpD8B;_LL6FB: {struct
Cyc_Absyn_PointerType_struct*_tmpD87=(struct Cyc_Absyn_PointerType_struct*)
_tmpD86;if(_tmpD87->tag != 5)goto _LL6FD;else{_tmpD88=_tmpD87->f1;_tmpD89=_tmpD88.elt_typ;
_tmpD8A=_tmpD88.ptr_atts;_tmpD8B=_tmpD8A.rgn;}}_LL6FC: {struct Cyc_Absyn_Aggrfield*
finfo;{void*_tmpD8C=Cyc_Tcutil_compress(_tmpD89);struct Cyc_List_List*_tmpD8E;
struct Cyc_Absyn_AggrInfo _tmpD90;union Cyc_Absyn_AggrInfoU _tmpD91;struct Cyc_Absyn_Aggrdecl**
_tmpD92;struct Cyc_Absyn_Aggrdecl*_tmpD93;_LL700: {struct Cyc_Absyn_AnonAggrType_struct*
_tmpD8D=(struct Cyc_Absyn_AnonAggrType_struct*)_tmpD8C;if(_tmpD8D->tag != 13)goto
_LL702;else{_tmpD8E=_tmpD8D->f2;}}_LL701: finfo=Cyc_Absyn_lookup_field(_tmpD8E,
_tmpD5F);goto _LL6FF;_LL702: {struct Cyc_Absyn_AggrType_struct*_tmpD8F=(struct Cyc_Absyn_AggrType_struct*)
_tmpD8C;if(_tmpD8F->tag != 12)goto _LL704;else{_tmpD90=_tmpD8F->f1;_tmpD91=_tmpD90.aggr_info;
if((_tmpD91.KnownAggr).tag != 2)goto _LL704;_tmpD92=(struct Cyc_Absyn_Aggrdecl**)(
_tmpD91.KnownAggr).val;_tmpD93=*_tmpD92;}}_LL703: finfo=Cyc_Absyn_lookup_decl_field(
_tmpD93,_tmpD5F);goto _LL6FF;_LL704:;_LL705: return bogus_ans;_LL6FF:;}if(finfo != 0
 && finfo->width != 0){struct _tuple16 _tmp1383;return(_tmp1383.f1=(finfo->tq).real_const,((
_tmp1383.f2=_tmpD8B,_tmp1383)));}return bogus_ans;}_LL6FD:;_LL6FE: return bogus_ans;
_LL6FA:;};_LL6D6: {struct Cyc_Absyn_Deref_e_struct*_tmpD61=(struct Cyc_Absyn_Deref_e_struct*)
_tmpD55;if(_tmpD61->tag != 22)goto _LL6D8;else{_tmpD62=_tmpD61->f1;}}_LL6D7: {void*
_tmpD95=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpD62->topt))->v);
struct Cyc_Absyn_PtrInfo _tmpD97;struct Cyc_Absyn_Tqual _tmpD98;struct Cyc_Absyn_PtrAtts
_tmpD99;void*_tmpD9A;_LL707: {struct Cyc_Absyn_PointerType_struct*_tmpD96=(struct
Cyc_Absyn_PointerType_struct*)_tmpD95;if(_tmpD96->tag != 5)goto _LL709;else{
_tmpD97=_tmpD96->f1;_tmpD98=_tmpD97.elt_tq;_tmpD99=_tmpD97.ptr_atts;_tmpD9A=
_tmpD99.rgn;}}_LL708: {struct _tuple16 _tmp1384;return(_tmp1384.f1=_tmpD98.real_const,((
_tmp1384.f2=_tmpD9A,_tmp1384)));}_LL709:;_LL70A: return bogus_ans;_LL706:;}_LL6D8: {
struct Cyc_Absyn_Subscript_e_struct*_tmpD63=(struct Cyc_Absyn_Subscript_e_struct*)
_tmpD55;if(_tmpD63->tag != 25)goto _LL6DA;else{_tmpD64=_tmpD63->f1;_tmpD65=_tmpD63->f2;}}
_LL6D9: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpD64->topt))->v);void*_tmpD9C=t;struct Cyc_List_List*_tmpD9E;struct Cyc_Absyn_PtrInfo
_tmpDA0;struct Cyc_Absyn_Tqual _tmpDA1;struct Cyc_Absyn_PtrAtts _tmpDA2;void*_tmpDA3;
struct Cyc_Absyn_ArrayInfo _tmpDA5;struct Cyc_Absyn_Tqual _tmpDA6;_LL70C: {struct Cyc_Absyn_TupleType_struct*
_tmpD9D=(struct Cyc_Absyn_TupleType_struct*)_tmpD9C;if(_tmpD9D->tag != 11)goto
_LL70E;else{_tmpD9E=_tmpD9D->f1;}}_LL70D: {unsigned int _tmpDA8;int _tmpDA9;struct
_tuple13 _tmpDA7=Cyc_Evexp_eval_const_uint_exp(_tmpD65);_tmpDA8=_tmpDA7.f1;
_tmpDA9=_tmpDA7.f2;if(!_tmpDA9)return bogus_ans;{struct _tuple11*_tmpDAA=Cyc_Absyn_lookup_tuple_field(
_tmpD9E,(int)_tmpDA8);if(_tmpDAA != 0){struct _tuple16 _tmp1385;return(_tmp1385.f1=((*
_tmpDAA).f1).real_const,((_tmp1385.f2=(Cyc_Tcutil_addressof_props(te,_tmpD64)).f2,
_tmp1385)));}return bogus_ans;};}_LL70E: {struct Cyc_Absyn_PointerType_struct*
_tmpD9F=(struct Cyc_Absyn_PointerType_struct*)_tmpD9C;if(_tmpD9F->tag != 5)goto
_LL710;else{_tmpDA0=_tmpD9F->f1;_tmpDA1=_tmpDA0.elt_tq;_tmpDA2=_tmpDA0.ptr_atts;
_tmpDA3=_tmpDA2.rgn;}}_LL70F: {struct _tuple16 _tmp1386;return(_tmp1386.f1=_tmpDA1.real_const,((
_tmp1386.f2=_tmpDA3,_tmp1386)));}_LL710: {struct Cyc_Absyn_ArrayType_struct*
_tmpDA4=(struct Cyc_Absyn_ArrayType_struct*)_tmpD9C;if(_tmpDA4->tag != 9)goto
_LL712;else{_tmpDA5=_tmpDA4->f1;_tmpDA6=_tmpDA5.tq;}}_LL711: {struct _tuple16
_tmp1387;return(_tmp1387.f1=_tmpDA6.real_const,((_tmp1387.f2=(Cyc_Tcutil_addressof_props(
te,_tmpD64)).f2,_tmp1387)));}_LL712:;_LL713: return bogus_ans;_LL70B:;}_LL6DA:;
_LL6DB:{const char*_tmp138A;void*_tmp1389;(_tmp1389=0,Cyc_Tcutil_terr(e->loc,((
_tmp138A="unary & applied to non-lvalue",_tag_dyneither(_tmp138A,sizeof(char),30))),
_tag_dyneither(_tmp1389,sizeof(void*),0)));}return bogus_ans;_LL6CF:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpDB1=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpDB3;void*_tmpDB4;struct Cyc_Absyn_Tqual
_tmpDB5;union Cyc_Absyn_Constraint*_tmpDB6;_LL715: {struct Cyc_Absyn_ArrayType_struct*
_tmpDB2=(struct Cyc_Absyn_ArrayType_struct*)_tmpDB1;if(_tmpDB2->tag != 9)goto
_LL717;else{_tmpDB3=_tmpDB2->f1;_tmpDB4=_tmpDB3.elt_type;_tmpDB5=_tmpDB3.tq;
_tmpDB6=_tmpDB3.zero_term;}}_LL716: {void*_tmpDB8;struct _tuple16 _tmpDB7=Cyc_Tcutil_addressof_props(
te,e);_tmpDB8=_tmpDB7.f2;{struct Cyc_Absyn_Upper_b_struct _tmp138D;struct Cyc_Absyn_Upper_b_struct*
_tmp138C;return Cyc_Absyn_atb_typ(_tmpDB4,_tmpDB8,_tmpDB5,(void*)((_tmp138C=
_cycalloc(sizeof(*_tmp138C)),((_tmp138C[0]=((_tmp138D.tag=1,((_tmp138D.f1=e,
_tmp138D)))),_tmp138C)))),_tmpDB6);};}_LL717:;_LL718: return e_typ;_LL714:;}void
Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union Cyc_Absyn_Constraint*
b);void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,union
Cyc_Absyn_Constraint*b){b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*
x))Cyc_Absyn_compress_conref)(b);{void*_tmpDBB=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);struct Cyc_Absyn_Exp*_tmpDBE;
_LL71A: {struct Cyc_Absyn_DynEither_b_struct*_tmpDBC=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpDBB;if(_tmpDBC->tag != 0)goto _LL71C;}_LL71B: return;_LL71C: {struct Cyc_Absyn_Upper_b_struct*
_tmpDBD=(struct Cyc_Absyn_Upper_b_struct*)_tmpDBB;if(_tmpDBD->tag != 1)goto _LL719;
else{_tmpDBE=_tmpDBD->f1;}}_LL71D: {unsigned int _tmpDC0;int _tmpDC1;struct
_tuple13 _tmpDBF=Cyc_Evexp_eval_const_uint_exp(_tmpDBE);_tmpDC0=_tmpDBF.f1;
_tmpDC1=_tmpDBF.f2;if(_tmpDC1  && _tmpDC0 <= i){const char*_tmp1392;void*_tmp1391[2];
struct Cyc_Int_pa_struct _tmp1390;struct Cyc_Int_pa_struct _tmp138F;(_tmp138F.tag=1,((
_tmp138F.f1=(unsigned long)((int)i),((_tmp1390.tag=1,((_tmp1390.f1=(
unsigned long)((int)_tmpDC0),((_tmp1391[0]=& _tmp1390,((_tmp1391[1]=& _tmp138F,Cyc_Tcutil_terr(
loc,((_tmp1392="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1392,
sizeof(char),39))),_tag_dyneither(_tmp1391,sizeof(void*),2)))))))))))));}return;}
_LL719:;};}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,
union Cyc_Absyn_Constraint*b);void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,union Cyc_Absyn_Constraint*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
union Cyc_Absyn_Constraint*b);int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*
b){void*_tmpDC6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((
void*)& Cyc_Absyn_DynEither_b_val,b);struct Cyc_Absyn_Exp*_tmpDC8;_LL71F: {struct
Cyc_Absyn_Upper_b_struct*_tmpDC7=(struct Cyc_Absyn_Upper_b_struct*)_tmpDC6;if(
_tmpDC7->tag != 1)goto _LL721;else{_tmpDC8=_tmpDC7->f1;}}_LL720: {unsigned int
_tmpDCA;int _tmpDCB;struct _tuple13 _tmpDC9=Cyc_Evexp_eval_const_uint_exp(_tmpDC8);
_tmpDCA=_tmpDC9.f1;_tmpDCB=_tmpDC9.f2;return _tmpDCB  && _tmpDCA == 1;}_LL721:;
_LL722: return 0;_LL71E:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpDCC=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpDD4;
void*_tmpDD5;union Cyc_Absyn_Constraint*_tmpDD6;struct Cyc_List_List*_tmpDD8;
struct Cyc_Absyn_AggrInfo _tmpDDA;union Cyc_Absyn_AggrInfoU _tmpDDB;struct _tuple4
_tmpDDC;struct Cyc_Absyn_AggrInfo _tmpDDE;union Cyc_Absyn_AggrInfoU _tmpDDF;struct
Cyc_Absyn_Aggrdecl**_tmpDE0;struct Cyc_Absyn_Aggrdecl*_tmpDE1;struct Cyc_List_List*
_tmpDE2;struct Cyc_List_List*_tmpDE4;_LL724: {struct Cyc_Absyn_VoidType_struct*
_tmpDCD=(struct Cyc_Absyn_VoidType_struct*)_tmpDCC;if(_tmpDCD->tag != 0)goto _LL726;}
_LL725: goto _LL727;_LL726: {struct Cyc_Absyn_IntType_struct*_tmpDCE=(struct Cyc_Absyn_IntType_struct*)
_tmpDCC;if(_tmpDCE->tag != 6)goto _LL728;}_LL727: goto _LL729;_LL728: {struct Cyc_Absyn_FloatType_struct*
_tmpDCF=(struct Cyc_Absyn_FloatType_struct*)_tmpDCC;if(_tmpDCF->tag != 7)goto
_LL72A;}_LL729: goto _LL72B;_LL72A: {struct Cyc_Absyn_DoubleType_struct*_tmpDD0=(
struct Cyc_Absyn_DoubleType_struct*)_tmpDCC;if(_tmpDD0->tag != 8)goto _LL72C;}
_LL72B: return 1;_LL72C: {struct Cyc_Absyn_EnumType_struct*_tmpDD1=(struct Cyc_Absyn_EnumType_struct*)
_tmpDCC;if(_tmpDD1->tag != 14)goto _LL72E;}_LL72D: goto _LL72F;_LL72E: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpDD2=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpDCC;if(_tmpDD2->tag != 15)goto
_LL730;}_LL72F: return 0;_LL730: {struct Cyc_Absyn_ArrayType_struct*_tmpDD3=(struct
Cyc_Absyn_ArrayType_struct*)_tmpDCC;if(_tmpDD3->tag != 9)goto _LL732;else{_tmpDD4=
_tmpDD3->f1;_tmpDD5=_tmpDD4.elt_type;_tmpDD6=_tmpDD4.zero_term;}}_LL731: return !((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpDD6) && Cyc_Tcutil_bits_only(
_tmpDD5);_LL732: {struct Cyc_Absyn_TupleType_struct*_tmpDD7=(struct Cyc_Absyn_TupleType_struct*)
_tmpDCC;if(_tmpDD7->tag != 11)goto _LL734;else{_tmpDD8=_tmpDD7->f1;}}_LL733: for(0;
_tmpDD8 != 0;_tmpDD8=_tmpDD8->tl){if(!Cyc_Tcutil_bits_only((*((struct _tuple11*)
_tmpDD8->hd)).f2))return 0;}return 1;_LL734: {struct Cyc_Absyn_AggrType_struct*
_tmpDD9=(struct Cyc_Absyn_AggrType_struct*)_tmpDCC;if(_tmpDD9->tag != 12)goto
_LL736;else{_tmpDDA=_tmpDD9->f1;_tmpDDB=_tmpDDA.aggr_info;if((_tmpDDB.UnknownAggr).tag
!= 1)goto _LL736;_tmpDDC=(struct _tuple4)(_tmpDDB.UnknownAggr).val;}}_LL735: return
0;_LL736: {struct Cyc_Absyn_AggrType_struct*_tmpDDD=(struct Cyc_Absyn_AggrType_struct*)
_tmpDCC;if(_tmpDDD->tag != 12)goto _LL738;else{_tmpDDE=_tmpDDD->f1;_tmpDDF=_tmpDDE.aggr_info;
if((_tmpDDF.KnownAggr).tag != 2)goto _LL738;_tmpDE0=(struct Cyc_Absyn_Aggrdecl**)(
_tmpDDF.KnownAggr).val;_tmpDE1=*_tmpDE0;_tmpDE2=_tmpDDE.targs;}}_LL737: if(
_tmpDE1->impl == 0)return 0;{struct _RegionHandle _tmpDE5=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpDE5;_push_region(rgn);{struct Cyc_List_List*_tmpDE6=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpDE1->tvs,_tmpDE2);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDE1->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpDE6,((
struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpDE7=0;_npop_handler(0);return
_tmpDE7;}}}{int _tmpDE8=1;_npop_handler(0);return _tmpDE8;};};_pop_region(rgn);};
_LL738: {struct Cyc_Absyn_AnonAggrType_struct*_tmpDE3=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpDCC;if(_tmpDE3->tag != 13)goto _LL73A;else{_tmpDE4=_tmpDE3->f2;}}_LL739: for(0;
_tmpDE4 != 0;_tmpDE4=_tmpDE4->tl){if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)
_tmpDE4->hd)->type))return 0;}return 1;_LL73A:;_LL73B: return 0;_LL723:;}struct
_tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e);static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmpDE9=e->r;
struct _tuple2*_tmpDF2;void*_tmpDF3;struct Cyc_Absyn_Exp*_tmpDF5;struct Cyc_Absyn_Exp*
_tmpDF6;struct Cyc_Absyn_Exp*_tmpDF7;struct Cyc_Absyn_Exp*_tmpDF9;struct Cyc_Absyn_Exp*
_tmpDFA;struct Cyc_Absyn_Exp*_tmpDFC;struct Cyc_Absyn_Exp*_tmpDFE;void*_tmpE00;
struct Cyc_Absyn_Exp*_tmpE01;enum Cyc_Absyn_Coercion _tmpE02;void*_tmpE04;struct
Cyc_Absyn_Exp*_tmpE05;struct Cyc_Absyn_Exp*_tmpE07;struct Cyc_Absyn_Exp*_tmpE09;
struct Cyc_Absyn_Exp*_tmpE0A;struct Cyc_List_List*_tmpE0C;struct Cyc_List_List*
_tmpE0E;struct Cyc_List_List*_tmpE10;enum Cyc_Absyn_Primop _tmpE12;struct Cyc_List_List*
_tmpE13;struct Cyc_List_List*_tmpE15;struct Cyc_List_List*_tmpE17;_LL73D: {struct
Cyc_Absyn_Const_e_struct*_tmpDEA=(struct Cyc_Absyn_Const_e_struct*)_tmpDE9;if(
_tmpDEA->tag != 0)goto _LL73F;}_LL73E: goto _LL740;_LL73F: {struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpDEB=(struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpDE9;if(_tmpDEB->tag != 18)goto
_LL741;}_LL740: goto _LL742;_LL741: {struct Cyc_Absyn_Sizeofexp_e_struct*_tmpDEC=(
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpDE9;if(_tmpDEC->tag != 19)goto _LL743;}
_LL742: goto _LL744;_LL743: {struct Cyc_Absyn_Offsetof_e_struct*_tmpDED=(struct Cyc_Absyn_Offsetof_e_struct*)
_tmpDE9;if(_tmpDED->tag != 20)goto _LL745;}_LL744: goto _LL746;_LL745: {struct Cyc_Absyn_Gentyp_e_struct*
_tmpDEE=(struct Cyc_Absyn_Gentyp_e_struct*)_tmpDE9;if(_tmpDEE->tag != 21)goto
_LL747;}_LL746: goto _LL748;_LL747: {struct Cyc_Absyn_Enum_e_struct*_tmpDEF=(struct
Cyc_Absyn_Enum_e_struct*)_tmpDE9;if(_tmpDEF->tag != 33)goto _LL749;}_LL748: goto
_LL74A;_LL749: {struct Cyc_Absyn_AnonEnum_e_struct*_tmpDF0=(struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpDE9;if(_tmpDF0->tag != 34)goto _LL74B;}_LL74A: return 1;_LL74B: {struct Cyc_Absyn_Var_e_struct*
_tmpDF1=(struct Cyc_Absyn_Var_e_struct*)_tmpDE9;if(_tmpDF1->tag != 1)goto _LL74D;
else{_tmpDF2=_tmpDF1->f1;_tmpDF3=(void*)_tmpDF1->f2;}}_LL74C: {void*_tmpE18=
_tmpDF3;struct Cyc_Absyn_Vardecl*_tmpE1B;_LL76C: {struct Cyc_Absyn_Funname_b_struct*
_tmpE19=(struct Cyc_Absyn_Funname_b_struct*)_tmpE18;if(_tmpE19->tag != 2)goto
_LL76E;}_LL76D: return 1;_LL76E: {struct Cyc_Absyn_Global_b_struct*_tmpE1A=(struct
Cyc_Absyn_Global_b_struct*)_tmpE18;if(_tmpE1A->tag != 1)goto _LL770;else{_tmpE1B=
_tmpE1A->f1;}}_LL76F: {void*_tmpE1D=Cyc_Tcutil_compress(_tmpE1B->type);_LL775: {
struct Cyc_Absyn_ArrayType_struct*_tmpE1E=(struct Cyc_Absyn_ArrayType_struct*)
_tmpE1D;if(_tmpE1E->tag != 9)goto _LL777;}_LL776: goto _LL778;_LL777: {struct Cyc_Absyn_FnType_struct*
_tmpE1F=(struct Cyc_Absyn_FnType_struct*)_tmpE1D;if(_tmpE1F->tag != 10)goto _LL779;}
_LL778: return 1;_LL779:;_LL77A: return var_okay;_LL774:;}_LL770: {struct Cyc_Absyn_Unresolved_b_struct*
_tmpE1C=(struct Cyc_Absyn_Unresolved_b_struct*)_tmpE18;if(_tmpE1C->tag != 0)goto
_LL772;}_LL771: return 0;_LL772:;_LL773: return var_okay;_LL76B:;}_LL74D: {struct Cyc_Absyn_Conditional_e_struct*
_tmpDF4=(struct Cyc_Absyn_Conditional_e_struct*)_tmpDE9;if(_tmpDF4->tag != 6)goto
_LL74F;else{_tmpDF5=_tmpDF4->f1;_tmpDF6=_tmpDF4->f2;_tmpDF7=_tmpDF4->f3;}}_LL74E:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpDF5) && Cyc_Tcutil_cnst_exp(te,0,_tmpDF6))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpDF7);_LL74F: {struct Cyc_Absyn_SeqExp_e_struct*
_tmpDF8=(struct Cyc_Absyn_SeqExp_e_struct*)_tmpDE9;if(_tmpDF8->tag != 9)goto _LL751;
else{_tmpDF9=_tmpDF8->f1;_tmpDFA=_tmpDF8->f2;}}_LL750: return Cyc_Tcutil_cnst_exp(
te,0,_tmpDF9) && Cyc_Tcutil_cnst_exp(te,0,_tmpDFA);_LL751: {struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpDFB=(struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpDE9;if(_tmpDFB->tag != 13)
goto _LL753;else{_tmpDFC=_tmpDFB->f1;}}_LL752: _tmpDFE=_tmpDFC;goto _LL754;_LL753: {
struct Cyc_Absyn_Instantiate_e_struct*_tmpDFD=(struct Cyc_Absyn_Instantiate_e_struct*)
_tmpDE9;if(_tmpDFD->tag != 14)goto _LL755;else{_tmpDFE=_tmpDFD->f1;}}_LL754: return
Cyc_Tcutil_cnst_exp(te,var_okay,_tmpDFE);_LL755: {struct Cyc_Absyn_Cast_e_struct*
_tmpDFF=(struct Cyc_Absyn_Cast_e_struct*)_tmpDE9;if(_tmpDFF->tag != 15)goto _LL757;
else{_tmpE00=(void*)_tmpDFF->f1;_tmpE01=_tmpDFF->f2;_tmpE02=_tmpDFF->f4;if(
_tmpE02 != Cyc_Absyn_No_coercion)goto _LL757;}}_LL756: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpE01);_LL757: {struct Cyc_Absyn_Cast_e_struct*_tmpE03=(struct Cyc_Absyn_Cast_e_struct*)
_tmpDE9;if(_tmpE03->tag != 15)goto _LL759;else{_tmpE04=(void*)_tmpE03->f1;_tmpE05=
_tmpE03->f2;}}_LL758: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpE05);_LL759: {
struct Cyc_Absyn_Address_e_struct*_tmpE06=(struct Cyc_Absyn_Address_e_struct*)
_tmpDE9;if(_tmpE06->tag != 16)goto _LL75B;else{_tmpE07=_tmpE06->f1;}}_LL75A: return
Cyc_Tcutil_cnst_exp(te,1,_tmpE07);_LL75B: {struct Cyc_Absyn_Comprehension_e_struct*
_tmpE08=(struct Cyc_Absyn_Comprehension_e_struct*)_tmpDE9;if(_tmpE08->tag != 29)
goto _LL75D;else{_tmpE09=_tmpE08->f2;_tmpE0A=_tmpE08->f3;}}_LL75C: return Cyc_Tcutil_cnst_exp(
te,0,_tmpE09) && Cyc_Tcutil_cnst_exp(te,0,_tmpE0A);_LL75D: {struct Cyc_Absyn_Array_e_struct*
_tmpE0B=(struct Cyc_Absyn_Array_e_struct*)_tmpDE9;if(_tmpE0B->tag != 28)goto _LL75F;
else{_tmpE0C=_tmpE0B->f1;}}_LL75E: _tmpE0E=_tmpE0C;goto _LL760;_LL75F: {struct Cyc_Absyn_AnonStruct_e_struct*
_tmpE0D=(struct Cyc_Absyn_AnonStruct_e_struct*)_tmpDE9;if(_tmpE0D->tag != 31)goto
_LL761;else{_tmpE0E=_tmpE0D->f2;}}_LL760: _tmpE10=_tmpE0E;goto _LL762;_LL761: {
struct Cyc_Absyn_Aggregate_e_struct*_tmpE0F=(struct Cyc_Absyn_Aggregate_e_struct*)
_tmpDE9;if(_tmpE0F->tag != 30)goto _LL763;else{_tmpE10=_tmpE0F->f3;}}_LL762: for(0;
_tmpE10 != 0;_tmpE10=_tmpE10->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple30*)
_tmpE10->hd)).f2))return 0;}return 1;_LL763: {struct Cyc_Absyn_Primop_e_struct*
_tmpE11=(struct Cyc_Absyn_Primop_e_struct*)_tmpDE9;if(_tmpE11->tag != 3)goto _LL765;
else{_tmpE12=_tmpE11->f1;_tmpE13=_tmpE11->f2;}}_LL764: _tmpE15=_tmpE13;goto _LL766;
_LL765: {struct Cyc_Absyn_Tuple_e_struct*_tmpE14=(struct Cyc_Absyn_Tuple_e_struct*)
_tmpDE9;if(_tmpE14->tag != 26)goto _LL767;else{_tmpE15=_tmpE14->f1;}}_LL766:
_tmpE17=_tmpE15;goto _LL768;_LL767: {struct Cyc_Absyn_Datatype_e_struct*_tmpE16=(
struct Cyc_Absyn_Datatype_e_struct*)_tmpDE9;if(_tmpE16->tag != 32)goto _LL769;else{
_tmpE17=_tmpE16->f1;}}_LL768: for(0;_tmpE17 != 0;_tmpE17=_tmpE17->tl){if(!Cyc_Tcutil_cnst_exp(
te,0,(struct Cyc_Absyn_Exp*)_tmpE17->hd))return 0;}return 1;_LL769:;_LL76A: return 0;
_LL73C:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpE20=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpE26;struct Cyc_Absyn_PtrAtts _tmpE27;union Cyc_Absyn_Constraint*
_tmpE28;union Cyc_Absyn_Constraint*_tmpE29;struct Cyc_Absyn_ArrayInfo _tmpE2B;void*
_tmpE2C;struct Cyc_List_List*_tmpE2E;struct Cyc_Absyn_AggrInfo _tmpE30;union Cyc_Absyn_AggrInfoU
_tmpE31;struct Cyc_List_List*_tmpE32;struct Cyc_List_List*_tmpE34;_LL77C: {struct
Cyc_Absyn_VoidType_struct*_tmpE21=(struct Cyc_Absyn_VoidType_struct*)_tmpE20;if(
_tmpE21->tag != 0)goto _LL77E;}_LL77D: goto _LL77F;_LL77E: {struct Cyc_Absyn_IntType_struct*
_tmpE22=(struct Cyc_Absyn_IntType_struct*)_tmpE20;if(_tmpE22->tag != 6)goto _LL780;}
_LL77F: goto _LL781;_LL780: {struct Cyc_Absyn_FloatType_struct*_tmpE23=(struct Cyc_Absyn_FloatType_struct*)
_tmpE20;if(_tmpE23->tag != 7)goto _LL782;}_LL781: goto _LL783;_LL782: {struct Cyc_Absyn_DoubleType_struct*
_tmpE24=(struct Cyc_Absyn_DoubleType_struct*)_tmpE20;if(_tmpE24->tag != 8)goto
_LL784;}_LL783: return 1;_LL784: {struct Cyc_Absyn_PointerType_struct*_tmpE25=(
struct Cyc_Absyn_PointerType_struct*)_tmpE20;if(_tmpE25->tag != 5)goto _LL786;else{
_tmpE26=_tmpE25->f1;_tmpE27=_tmpE26.ptr_atts;_tmpE28=_tmpE27.nullable;_tmpE29=
_tmpE27.bounds;}}_LL785: {void*_tmpE37=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE29);_LL795: {
struct Cyc_Absyn_DynEither_b_struct*_tmpE38=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE37;if(_tmpE38->tag != 0)goto _LL797;}_LL796: return 1;_LL797:;_LL798: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpE28);_LL794:;}
_LL786: {struct Cyc_Absyn_ArrayType_struct*_tmpE2A=(struct Cyc_Absyn_ArrayType_struct*)
_tmpE20;if(_tmpE2A->tag != 9)goto _LL788;else{_tmpE2B=_tmpE2A->f1;_tmpE2C=_tmpE2B.elt_type;}}
_LL787: return Cyc_Tcutil_supports_default(_tmpE2C);_LL788: {struct Cyc_Absyn_TupleType_struct*
_tmpE2D=(struct Cyc_Absyn_TupleType_struct*)_tmpE20;if(_tmpE2D->tag != 11)goto
_LL78A;else{_tmpE2E=_tmpE2D->f1;}}_LL789: for(0;_tmpE2E != 0;_tmpE2E=_tmpE2E->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple11*)_tmpE2E->hd)).f2))return 0;}
return 1;_LL78A: {struct Cyc_Absyn_AggrType_struct*_tmpE2F=(struct Cyc_Absyn_AggrType_struct*)
_tmpE20;if(_tmpE2F->tag != 12)goto _LL78C;else{_tmpE30=_tmpE2F->f1;_tmpE31=_tmpE30.aggr_info;
_tmpE32=_tmpE30.targs;}}_LL78B: {struct Cyc_Absyn_Aggrdecl*_tmpE39=Cyc_Absyn_get_known_aggrdecl(
_tmpE31);if(_tmpE39->impl == 0)return 0;if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpE39->impl))->exist_vars != 0)return 0;return Cyc_Tcutil_fields_support_default(
_tmpE39->tvs,_tmpE32,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE39->impl))->fields);}
_LL78C: {struct Cyc_Absyn_AnonAggrType_struct*_tmpE33=(struct Cyc_Absyn_AnonAggrType_struct*)
_tmpE20;if(_tmpE33->tag != 13)goto _LL78E;else{_tmpE34=_tmpE33->f2;}}_LL78D: return
Cyc_Tcutil_fields_support_default(0,0,_tmpE34);_LL78E: {struct Cyc_Absyn_AnonEnumType_struct*
_tmpE35=(struct Cyc_Absyn_AnonEnumType_struct*)_tmpE20;if(_tmpE35->tag != 15)goto
_LL790;}_LL78F: goto _LL791;_LL790: {struct Cyc_Absyn_EnumType_struct*_tmpE36=(
struct Cyc_Absyn_EnumType_struct*)_tmpE20;if(_tmpE36->tag != 14)goto _LL792;}_LL791:
return 1;_LL792:;_LL793: return 0;_LL77B:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);static int
Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,
struct Cyc_List_List*fs){struct _RegionHandle _tmpE3A=_new_region("rgn");struct
_RegionHandle*rgn=& _tmpE3A;_push_region(rgn);{struct Cyc_List_List*_tmpE3B=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){
void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpE3B,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpE3C=0;_npop_handler(0);return _tmpE3C;}}}{
int _tmpE3D=1;_npop_handler(0);return _tmpE3D;};;_pop_region(rgn);}int Cyc_Tcutil_admits_zero(
void*t);int Cyc_Tcutil_admits_zero(void*t){void*_tmpE3E=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmpE43;struct Cyc_Absyn_PtrAtts _tmpE44;union Cyc_Absyn_Constraint*
_tmpE45;union Cyc_Absyn_Constraint*_tmpE46;_LL79A: {struct Cyc_Absyn_IntType_struct*
_tmpE3F=(struct Cyc_Absyn_IntType_struct*)_tmpE3E;if(_tmpE3F->tag != 6)goto _LL79C;}
_LL79B: goto _LL79D;_LL79C: {struct Cyc_Absyn_FloatType_struct*_tmpE40=(struct Cyc_Absyn_FloatType_struct*)
_tmpE3E;if(_tmpE40->tag != 7)goto _LL79E;}_LL79D: goto _LL79F;_LL79E: {struct Cyc_Absyn_DoubleType_struct*
_tmpE41=(struct Cyc_Absyn_DoubleType_struct*)_tmpE3E;if(_tmpE41->tag != 8)goto
_LL7A0;}_LL79F: return 1;_LL7A0: {struct Cyc_Absyn_PointerType_struct*_tmpE42=(
struct Cyc_Absyn_PointerType_struct*)_tmpE3E;if(_tmpE42->tag != 5)goto _LL7A2;else{
_tmpE43=_tmpE42->f1;_tmpE44=_tmpE43.ptr_atts;_tmpE45=_tmpE44.nullable;_tmpE46=
_tmpE44.bounds;}}_LL7A1: {void*_tmpE47=((void*(*)(void*y,union Cyc_Absyn_Constraint*
x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE46);_LL7A5: {
struct Cyc_Absyn_DynEither_b_struct*_tmpE48=(struct Cyc_Absyn_DynEither_b_struct*)
_tmpE47;if(_tmpE48->tag != 0)goto _LL7A7;}_LL7A6: return 0;_LL7A7:;_LL7A8: return((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE45);_LL7A4:;}
_LL7A2:;_LL7A3: return 0;_LL799:;}int Cyc_Tcutil_is_noreturn(void*t);int Cyc_Tcutil_is_noreturn(
void*t){{void*_tmpE49=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpE4B;void*
_tmpE4C;struct Cyc_Absyn_FnInfo _tmpE4E;struct Cyc_List_List*_tmpE4F;_LL7AA: {
struct Cyc_Absyn_PointerType_struct*_tmpE4A=(struct Cyc_Absyn_PointerType_struct*)
_tmpE49;if(_tmpE4A->tag != 5)goto _LL7AC;else{_tmpE4B=_tmpE4A->f1;_tmpE4C=_tmpE4B.elt_typ;}}
_LL7AB: return Cyc_Tcutil_is_noreturn(_tmpE4C);_LL7AC: {struct Cyc_Absyn_FnType_struct*
_tmpE4D=(struct Cyc_Absyn_FnType_struct*)_tmpE49;if(_tmpE4D->tag != 10)goto _LL7AE;
else{_tmpE4E=_tmpE4D->f1;_tmpE4F=_tmpE4E.attributes;}}_LL7AD: for(0;_tmpE4F != 0;
_tmpE4F=_tmpE4F->tl){void*_tmpE50=(void*)_tmpE4F->hd;_LL7B1: {struct Cyc_Absyn_Noreturn_att_struct*
_tmpE51=(struct Cyc_Absyn_Noreturn_att_struct*)_tmpE50;if(_tmpE51->tag != 4)goto
_LL7B3;}_LL7B2: return 1;_LL7B3:;_LL7B4: continue;_LL7B0:;}goto _LL7A9;_LL7AE:;
_LL7AF: goto _LL7A9;_LL7A9:;}return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts){void*_tmpE52=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmpE54;struct Cyc_List_List*_tmpE55;struct Cyc_List_List**_tmpE56;_LL7B6: {struct
Cyc_Absyn_FnType_struct*_tmpE53=(struct Cyc_Absyn_FnType_struct*)_tmpE52;if(
_tmpE53->tag != 10)goto _LL7B8;else{_tmpE54=_tmpE53->f1;_tmpE55=_tmpE54.attributes;
_tmpE56=(struct Cyc_List_List**)&(_tmpE53->f1).attributes;}}_LL7B7: {struct Cyc_List_List*
_tmpE57=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1393;*_tmpE56=((_tmp1393=_cycalloc(sizeof(*_tmp1393)),((
_tmp1393->hd=(void*)atts->hd,((_tmp1393->tl=*_tmpE56,_tmp1393))))));}else{struct
Cyc_List_List*_tmp1394;_tmpE57=((_tmp1394=_cycalloc(sizeof(*_tmp1394)),((
_tmp1394->hd=(void*)atts->hd,((_tmp1394->tl=_tmpE57,_tmp1394))))));}}return
_tmpE57;}_LL7B8:;_LL7B9: {const char*_tmp1397;void*_tmp1396;(_tmp1396=0,((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1397="transfer_fn_type_atts",
_tag_dyneither(_tmp1397,sizeof(char),22))),_tag_dyneither(_tmp1396,sizeof(void*),
0)));}_LL7B5:;}
