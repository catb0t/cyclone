/* This is a C header file to be used by the output of the Cyclone
   to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
*/
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#include <setjmp.h>

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
struct _dynforward_ptr {
  unsigned char *curr;
  unsigned char *last_plus_one;
};

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
  char data[0];
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

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static inline void *
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
static inline char *
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
static inline unsigned
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
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ({ \
  typedef _czs_tx = (*orig_x); \
  _czs_tx *_czs_x = (_czs_tx *)(orig_x); \
  unsigned int _czs_sz = (orig_sz); \
  int _czs_i = (orig_i); \
  unsigned int _czs_temp; \
  if ((_czs_x) == 0) _throw_null(); \
  if (_czs_i < 0) _throw_arraybounds(); \
  for (_czs_temp=_czs_sz; _czs_temp < _czs_i; _czs_temp++) \
    if (_czs_x[_czs_temp] == 0) _throw_arraybounds(); \
  _czs_x+_czs_i; })
#endif

/* Calculates the number of elements in a zero-terminated, thin array.
   If non-null, the array is guaranteed to have orig_offset elements. */
#define _get_zero_arr_size(orig_x,orig_offset) ({ \
  typedef _gres_tx = (*orig_x); \
  _gres_tx *_gres_x = (_gres_tx *)(orig_x); \
  unsigned int _gres_offset = (orig_offset); \
  unsigned int _gres = 0; \
  if (_gres_x != 0) { \
     _gres = _gres_offset; \
     _gres_x += _gres_offset - 1; \
     while (*_gres_x != 0) { _gres_x++; _gres++; } \
  } _gres; })

/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus. */
#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })

/* Does in-place increment of a zero-terminated pointer (e.g., x++).
   Note that this expands to call _zero_arr_plus. */
#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })
  


/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
static inline unsigned char *
_check_dynforward_subscript(struct _dynforward_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dynforward_ptr _cus_arr = (arr);
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
#define _check_dynforward_subscript(arr,elt_sz,index) ({ \
  struct _dynforward_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
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
static inline unsigned char *
_check_dynforward_subscript(struct _dynforward_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dynforward_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  if (!_cus_arr.last_plus_one) _throw_null();
  if (_cus_ans >= _cus_arr.last_plus_one)
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
#define _check_dynforward_subscript(arr,elt_sz,index) ({ \
  struct _dynforward_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.last_plus_one) _throw_null(); \
  if (_cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _dyneither_ptr
_tag_dyneither(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
static inline struct _dynforward_ptr
_tag_dynforward(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dynforward_ptr _tag_arr_ans;
  _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.curr + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#define _tag_dynforward(tcurr,elt_sz,num_elts) ({ \
  struct _dynforward_ptr _tag_arr_ans; \
  _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.curr + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _dyneither_ptr *
_init_dyneither_ptr(struct _dyneither_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
static inline struct _dynforward_ptr *
_init_dynforward_ptr(struct _dynforward_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dynforward_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_dyneither_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#define _init_dynforward_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dynforward_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dynforward_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static inline unsigned char *
_untag_dyneither_ptr(struct _dyneither_ptr arr, 
                     unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
static inline unsigned char *
_untag_dynforward_ptr(struct _dynforward_ptr arr, 
                      unsigned elt_sz,unsigned num_elts) {
  struct _dynforward_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
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
#define _untag_dynforward_ptr(arr,elt_sz,num_elts) ({ \
  struct _dynforward_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static inline unsigned
_get_dyneither_size(struct _dyneither_ptr arr,unsigned elt_sz) {
  struct _dyneither_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
static inline unsigned
_get_dynforward_size(struct _dynforward_ptr arr,unsigned elt_sz) {
  struct _dynforward_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr >= _get_arr_size_last) ? 0 :
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
#define _get_dynforward_size(arr,elt_sz) \
  ({struct _dynforward_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
/* Here we have to worry about wrapping around, so if we go past the
 * end, we set the end to 0. */
static inline struct _dynforward_ptr
_dynforward_ptr_plus(struct _dynforward_ptr arr,unsigned elt_sz,int change) {
  struct _dynforward_ptr _ans = (arr);
  unsigned int _dfpp_elts = (((unsigned)_ans.last_plus_one) - 
                             ((unsigned)_ans.curr)) / elt_sz;
  if (change < 0 || ((unsigned)change) > _dfpp_elts)
    _ans.last_plus_one = 0;
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#define _dynforward_ptr_plus(arr,elt_sz,change) ({ \
  struct _dynforward_ptr _ans = (arr); \
  unsigned _dfpp_elt_sz = (elt_sz); \
  int _dfpp_change = (change); \
  unsigned int _dfpp_elts = (((unsigned)_ans.last_plus_one) - \
                            ((unsigned)_ans.curr)) / _dfpp_elt_sz; \
  if (_dfpp_change < 0 || ((unsigned)_dfpp_change) > _dfpp_elts) \
    _ans.last_plus_one = 0; \
  _ans.curr += ((int)(_dfpp_elt_sz))*(_dfpp_change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _dyneither_ptr
_dyneither_ptr_inplace_plus(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,
                            int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
static inline struct _dynforward_ptr
_dynforward_ptr_inplace_plus(struct _dynforward_ptr *arr_ptr,unsigned elt_sz,
                             int change) {
  struct _dynforward_ptr * _arr_ptr = (arr_ptr);
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - 
                             ((unsigned)_arr_ptr->curr)) / elt_sz;
  if (change < 0 || ((unsigned)change) > _dfpp_elts) 
    _arr_ptr->last_plus_one = 0;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#define _dynforward_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dynforward_ptr * _arr_ptr = (arr_ptr); \
  unsigned _dfpp_elt_sz = (elt_sz); \
  int _dfpp_change = (change); \
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - \
                            ((unsigned)_arr_ptr->curr)) / _dfpp_elt_sz; \
  if (_dfpp_change < 0 || ((unsigned)_dfpp_change) > _dfpp_elts) \
    _arr_ptr->last_plus_one = 0; \
  _arr_ptr->curr += ((int)(_dfpp_elt_sz))*(_dfpp_change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static inline struct _dyneither_ptr
_dyneither_ptr_inplace_plus_post(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  struct _dyneither_ptr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
static inline struct _dynforward_ptr
_dynforward_ptr_inplace_plus_post(struct _dynforward_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dynforward_ptr * _arr_ptr = (arr_ptr);
  struct _dynforward_ptr _ans = *_arr_ptr;
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - 
                            ((unsigned)_arr_ptr->curr)) / elt_sz; 
  if (change < 0 || ((unsigned)change) > _dfpp_elts) 
    _arr_ptr->last_plus_one = 0; 
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#define _dynforward_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dynforward_ptr * _arr_ptr = (arr_ptr); \
  struct _dynforward_ptr _ans = *_arr_ptr; \
  unsigned _dfpp_elt_sz = (elt_sz); \
  int _dfpp_change = (change); \
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - \
                            ((unsigned)_arr_ptr->curr)) / _dfpp_elt_sz; \
  if (_dfpp_change < 0 || ((unsigned)_dfpp_change) > _dfpp_elts) \
    _arr_ptr->last_plus_one = 0; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

// Decrease the upper bound on a fat pointer by numelts where sz is
// the size of the pointer's type.  Note that this can't be a macro
// if we're to get initializers right.
static struct 
_dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}
static struct 
_dynforward_ptr _dynforward_ptr_decrease_size(struct _dynforward_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  if (x.last_plus_one != 0)
    x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Convert between the two forms of dynamic pointers */
#ifdef _INLINE_FUNCTIONS 
static struct _dynforward_ptr
_dyneither_to_dynforward(struct _dyneither_ptr p) {
  struct _dynforward_ptr res;
  res.curr = p.curr;
  res.last_plus_one = (p.base == 0) ? 0 : p.last_plus_one;
  return res;
}
static struct _dyneither_ptr
_dynforward_to_dyneither(struct _dynforward_ptr p) {
  struct _dyneither_ptr res;
  res.base = res.curr = p.curr;
  res.last_plus_one = p.last_plus_one;
  if (p.last_plus_one == 0) 
    res.base = 0;
  return res;
}
#else 
#define _dyneither_to_dynforward(_dnfptr) ({ \
  struct _dyneither_ptr _dnfp = (_dnfptr); \
  struct _dynforward_ptr _dnfpres; \
  _dnfpres.curr = _dnfp.curr; \
  _dnfpres.last_plus_one = (_dnfp.base == 0) ? 0 : _dnfp.last_plus_one; \
  _dnfpres; })
#define _dynforward_to_dyneither(_dfnptr) ({ \
  struct _dynforward_ptr _dfnp = (_dfnptr); \
  struct _dyneither_ptr _dfnres; \
  _dfnres.base = _dfnres.curr = _dfnp.curr; \
  _dfnres.last_plus_one = _dfnp.last_plus_one; \
  if (_dfnp.last_plus_one == 0) \
    _dfnres.base = 0; \
  _dfnres; })
#endif 


//// Allocation
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static inline void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
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
 void exit(int);void*abort();struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];int isspace(
int);int isupper(int);typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long
quot;long rem;}Cyc_ldiv_t;void*abort();double atof(const char*);void exit(int);long
strtol(char*,char**,int);unsigned long strtoul(char*,char**,int);typedef struct{
int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdin;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_fscanf(struct Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);
int Cyc_getc(struct Cyc___cycFILE*);int Cyc_scanf(struct _dynforward_ptr,struct
_dynforward_ptr);int Cyc_sscanf(struct _dynforward_ptr,struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_ungetc(int,struct Cyc___cycFILE*);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};static struct _dyneither_ptr Cyc___sccl(char*tab,struct
_dyneither_ptr fmt);static short*Cyc_va_arg_short_ptr(void*a){void*_tmp0=a;short*
_tmp1;unsigned short*_tmp2;_LL1: if(*((int*)_tmp0)!= 0)goto _LL3;_tmp1=((struct Cyc_ShortPtr_sa_struct*)
_tmp0)->f1;_LL2: return _tmp1;_LL3: if(*((int*)_tmp0)!= 1)goto _LL5;_tmp2=((struct
Cyc_UShortPtr_sa_struct*)_tmp0)->f1;_LL4: return(short*)_tmp2;_LL5:;_LL6:(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp3=_cycalloc(sizeof(*
_tmp3));_tmp3[0]=({struct Cyc_Core_Invalid_argument_struct _tmp4;_tmp4.tag=Cyc_Core_Invalid_argument;
_tmp4.f1=({const char*_tmp5="scan expects short pointer";_tag_dynforward(_tmp5,
sizeof(char),_get_zero_arr_size(_tmp5,27));});_tmp4;});_tmp3;}));_LL0:;}static
int*Cyc_va_arg_int_ptr(void*a){void*_tmp6=a;int*_tmp7;unsigned int*_tmp8;_LL8:
if(*((int*)_tmp6)!= 2)goto _LLA;_tmp7=((struct Cyc_IntPtr_sa_struct*)_tmp6)->f1;
_LL9: return _tmp7;_LLA: if(*((int*)_tmp6)!= 3)goto _LLC;_tmp8=((struct Cyc_UIntPtr_sa_struct*)
_tmp6)->f1;_LLB: return(int*)_tmp8;_LLC:;_LLD:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=({struct Cyc_Core_Invalid_argument_struct
_tmpA;_tmpA.tag=Cyc_Core_Invalid_argument;_tmpA.f1=({const char*_tmpB="scan expects int pointer";
_tag_dynforward(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,25));});_tmpA;});
_tmp9;}));_LL7:;}static struct _dynforward_ptr Cyc_va_arg_string_ptr(void*a){void*
_tmpC=a;struct _dynforward_ptr _tmpD;struct _dynforward_ptr _tmpE;_LLF: if(*((int*)
_tmpC)!= 4)goto _LL11;_tmpD=((struct Cyc_StringPtr_sa_struct*)_tmpC)->f1;_LL10:
return _dynforward_ptr_decrease_size(_tmpD,sizeof(char),1);_LL11: if(*((int*)_tmpC)
!= 7)goto _LL13;_tmpE=((struct Cyc_CharPtr_sa_struct*)_tmpC)->f1;_LL12: return _tmpE;
_LL13:;_LL14:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmpF=
_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp10;_tmp10.tag=Cyc_Core_Invalid_argument;_tmp10.f1=({const char*_tmp11="scan expects char pointer";
_tag_dynforward(_tmp11,sizeof(char),_get_zero_arr_size(_tmp11,26));});_tmp10;});
_tmpF;}));_LLE:;}static double*Cyc_va_arg_double_ptr(void*a){void*_tmp12=a;double*
_tmp13;_LL16: if(*((int*)_tmp12)!= 5)goto _LL18;_tmp13=((struct Cyc_DoublePtr_sa_struct*)
_tmp12)->f1;_LL17: return _tmp13;_LL18:;_LL19:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp15;_tmp15.tag=Cyc_Core_Invalid_argument;_tmp15.f1=({const char*_tmp16="scan expects double pointer";
_tag_dynforward(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,28));});_tmp15;});
_tmp14;}));_LL15:;}static float*Cyc_va_arg_float_ptr(void*a){void*_tmp17=a;float*
_tmp18;_LL1B: if(*((int*)_tmp17)!= 6)goto _LL1D;_tmp18=((struct Cyc_FloatPtr_sa_struct*)
_tmp17)->f1;_LL1C: return _tmp18;_LL1D:;_LL1E:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp1A;_tmp1A.tag=Cyc_Core_Invalid_argument;_tmp1A.f1=({const char*_tmp1B="scan expects float pointer";
_tag_dynforward(_tmp1B,sizeof(char),_get_zero_arr_size(_tmp1B,27));});_tmp1A;});
_tmp19;}));_LL1A:;}static struct _dynforward_ptr Cyc_va_arg_char_ptr(void*a){void*
_tmp1C=a;struct _dynforward_ptr _tmp1D;struct _dynforward_ptr _tmp1E;_LL20: if(*((int*)
_tmp1C)!= 7)goto _LL22;_tmp1D=((struct Cyc_CharPtr_sa_struct*)_tmp1C)->f1;_LL21:
return _tmp1D;_LL22: if(*((int*)_tmp1C)!= 4)goto _LL24;_tmp1E=((struct Cyc_StringPtr_sa_struct*)
_tmp1C)->f1;_LL23: return _dynforward_ptr_decrease_size(_tmp1E,sizeof(char),1);
_LL24:;_LL25:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp1F=
_cycalloc(sizeof(*_tmp1F));_tmp1F[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp20;_tmp20.tag=Cyc_Core_Invalid_argument;_tmp20.f1=({const char*_tmp21="scan expects char pointer";
_tag_dynforward(_tmp21,sizeof(char),_get_zero_arr_size(_tmp21,26));});_tmp20;});
_tmp1F;}));_LL1F:;}int Cyc__IO_vfscanf(int(*_IO_getc)(void*),int(*_IO_ungetc)(int,
void*),int(*_IO_peekc)(void*),void*fp,struct _dynforward_ptr fmt0,struct
_dynforward_ptr ap,int*errp){struct _dyneither_ptr fmt=_dynforward_to_dyneither(
fmt0);int c;long long width;struct _dyneither_ptr p=_tag_dyneither(0,0,0);int n;int
flags=0;struct _dyneither_ptr p0=_tag_dyneither(0,0,0);int nassigned;int nread;int
base=0;int use_strtoul=0;char ccltab[256];char buf[351];{unsigned int _tmp3B=350;
unsigned int i;for(i=0;i < _tmp3B;i ++){buf[i]='0';}buf[_tmp3B]=(char)0;}{int
seen_eof=0;static short basefix[17]={10,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
nassigned=0;nread=0;for(0;1;0){c=(int)*((const char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));if(c == 0)
goto done;if(isspace(c)){for(0;1;0){c=_IO_getc(fp);if(c == - 1){seen_eof ++;break;}
if(!isspace(c)){_IO_ungetc(c,fp);break;}nread ++;}continue;}if(c != '%')goto
literal;width=(long long)0;flags=0;again: c=(int)*((const char*)
_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),
sizeof(char),0));switch(c){case '%': _LL26: literal: n=_IO_getc(fp);if(n == - 1)goto
eof_failure;if(n != c){_IO_ungetc(n,fp);goto match_failure;}nread ++;continue;case '*':
_LL27: if(flags)goto control_failure;flags=8;goto again;case 'l': _LL28: if(flags & ~(8
| 64))goto control_failure;flags |=1;goto again;case 'L': _LL29: if(flags & ~(8 | 64))
goto control_failure;flags |=2;goto again;case 'h': _LL2A: if(flags & ~(8 | 64))goto
control_failure;flags |=4;goto again;case '0': _LL2B: goto _LL2C;case '1': _LL2C: goto
_LL2D;case '2': _LL2D: goto _LL2E;case '3': _LL2E: goto _LL2F;case '4': _LL2F: goto _LL30;
case '5': _LL30: goto _LL31;case '6': _LL31: goto _LL32;case '7': _LL32: goto _LL33;case '8':
_LL33: goto _LL34;case '9': _LL34: if(flags & ~(8 | 64))goto control_failure;flags |=64;
width=(width * 10 + c)- '0';goto again;case 'D': _LL35: flags |=1;goto _LL36;case 'd':
_LL36: c=3;use_strtoul=0;base=10;break;case 'i': _LL37: c=3;use_strtoul=0;base=0;
break;case 'O': _LL38: flags |=1;goto _LL39;case 'o': _LL39: c=3;use_strtoul=1;base=8;
break;case 'u': _LL3A: c=3;use_strtoul=1;base=10;break;case 'X': _LL3B: goto _LL3C;case
'x': _LL3C: flags |=256;c=3;use_strtoul=1;base=16;break;case 'E': _LL3D: goto _LL3E;
case 'F': _LL3E: goto _LL3F;case 'e': _LL3F: goto _LL40;case 'f': _LL40: goto _LL41;case 'g':
_LL41: c=4;break;case 's': _LL42: c=2;break;case '[': _LL43: fmt=Cyc___sccl(ccltab,fmt);
flags |=32;c=1;break;case 'c': _LL44: flags |=32;c=0;break;case 'p': _LL45: flags |=16 | 
256;c=3;use_strtoul=1;base=16;break;case 'n': _LL46: if(flags & 8)continue;if(flags & 
4)*Cyc_va_arg_short_ptr(*((void**)_check_dynforward_subscript(ap,sizeof(void*),0)))=(
short)nread;else{if(flags & 1)*Cyc_va_arg_int_ptr(*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0)))=(long)nread;else{*Cyc_va_arg_int_ptr(*((
void**)_check_dynforward_subscript(ap,sizeof(void*),0)))=(int)nread;}}
_dynforward_ptr_inplace_plus_post(& ap,sizeof(void*),1);continue;case '\000': _LL47:
nassigned=- 1;goto done;default: _LL48: if(isupper(c))flags |=1;c=3;use_strtoul=0;
base=10;break;}if(_IO_peekc(fp)== - 1)goto eof_failure;if((flags & 32)== 0){n=
_IO_peekc(fp);while(isspace(n)){n=_IO_getc(fp);nread ++;n=_IO_peekc(fp);if(n == - 1)
goto eof_failure;}}switch(c){case 0: _LL4A: if(width == 0)width=(long long)1;if(flags
& 8){long long sum=(long long)0;for(0;width > 0;0){n=_IO_getc(fp);if(n == - 1  && 
width != 0)goto eof_failure;else{if(n == - 1){seen_eof ++;break;}}sum ++;width --;}
nread +=sum;}else{long long sum=(long long)0;struct _dynforward_ptr _tmp22=Cyc_va_arg_char_ptr(*((
void**)_check_dynforward_subscript(ap,sizeof(void*),0)));
_dynforward_ptr_inplace_plus_post(& ap,sizeof(void*),1);for(0;width > 0;0){n=
_IO_getc(fp);if(n == - 1  && width != 0)goto eof_failure;else{if(n == - 1){seen_eof ++;
break;}}*((char*)_check_dynforward_subscript(_tmp22,sizeof(char),0))=(char)n;
_dynforward_ptr_inplace_plus_post(& _tmp22,sizeof(char),1);sum ++;width --;}nread +=
sum;nassigned ++;}break;case 1: _LL4B: if(width == 0)width=(long long)~ 0;if(flags & 8){
n=0;{int c=_IO_peekc(fp);while((int)ccltab[_check_known_subscript_notnull(256,(
int)((char)c))]){n ++;_IO_getc(fp);if(-- width == 0)break;if((c=_IO_peekc(fp))== - 1){
if(n == 0)goto eof_failure;seen_eof ++;break;}}if(n == 0)goto match_failure;}}else{
struct _dyneither_ptr p4=_dynforward_to_dyneither(Cyc_va_arg_string_ptr(*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0))));
_dynforward_ptr_inplace_plus_post(& ap,sizeof(void*),1);{struct _dyneither_ptr p5=
p4;int c=_IO_peekc(fp);while((int)ccltab[_check_known_subscript_notnull(256,(int)((
char)c))]){if(_get_dyneither_size(p5,sizeof(char))== 0)goto eof_failure;*((char*)
_check_dyneither_subscript(p5,sizeof(char),0))=(char)c;
_dyneither_ptr_inplace_plus_post(& p5,sizeof(char),1);_IO_getc(fp);if(-- width == 0)
break;if((c=_IO_peekc(fp))== - 1){if(p5.curr == p0.curr)goto eof_failure;seen_eof ++;
break;}}n=(p5.curr - p4.curr)/ sizeof(char);if(n == 0)goto match_failure;if(
_get_dyneither_size(p5,sizeof(char))== 0)goto eof_failure;*((char*)
_check_dyneither_subscript(p5,sizeof(char),0))='\000';nassigned ++;}}nread +=n;
break;case 2: _LL4C: if(width == 0)width=(long long)~ 0;if(flags & 8){n=0;{int c=
_IO_peekc(fp);while(!isspace((int)((unsigned char)c))){n ++;_IO_getc(fp);if(--
width == 0)break;if((c=_IO_peekc(fp))== - 1){seen_eof ++;break;}}nread +=n;}}else{
struct _dynforward_ptr _tmp23=Cyc_va_arg_string_ptr(*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0)));
_dynforward_ptr_inplace_plus_post(& ap,sizeof(void*),1);{struct _dynforward_ptr
_tmp24=_tmp23;int c=_IO_peekc(fp);while(!isspace((int)((unsigned char)c))){c=
_IO_getc(fp);if(_get_dynforward_size(_tmp24,sizeof(char))== 0)goto eof_failure;*((
char*)_check_dynforward_subscript(_tmp24,sizeof(char),0))=(char)c;
_dynforward_ptr_inplace_plus_post(& _tmp24,sizeof(char),1);if(-- width == 0)break;
if((c=_IO_peekc(fp))== - 1){seen_eof ++;break;}}if(_get_dynforward_size(_tmp24,
sizeof(char))== 0)goto eof_failure;*((char*)_check_dynforward_subscript(_tmp24,
sizeof(char),0))='\000';nread +=(_tmp24.curr - _tmp23.curr)/ sizeof(char);
nassigned ++;}}continue;case 3: _LL4D: if(width == 0  || width > sizeof(buf)- 1)width=(
long long)(sizeof(buf)- 1);flags |=(64 | 128)| 512;for(p=({char*_tmp25=buf;
_tag_dyneither(_tmp25,sizeof(char),_get_zero_arr_size(_tmp25,351));});width != 0;
width --){c=(int)((unsigned char)_IO_peekc(fp));switch(c){case '0': _LL4F: if(base == 
0){base=8;flags |=256;}if(flags & 512)flags &=~((64 | 512)| 128);else{flags &=~((64 | 
256)| 128);}goto ok;case '1': _LL50: goto _LL51;case '2': _LL51: goto _LL52;case '3': _LL52:
goto _LL53;case '4': _LL53: goto _LL54;case '5': _LL54: goto _LL55;case '6': _LL55: goto
_LL56;case '7': _LL56: base=(int)basefix[_check_known_subscript_notnull(17,base)];
flags &=~((64 | 256)| 128);goto ok;case '8': _LL57: goto _LL58;case '9': _LL58: base=(int)
basefix[_check_known_subscript_notnull(17,base)];if(base <= 8)break;flags &=~((64 | 
256)| 128);goto ok;case 'A': _LL59: goto _LL5A;case 'B': _LL5A: goto _LL5B;case 'C': _LL5B:
goto _LL5C;case 'D': _LL5C: goto _LL5D;case 'E': _LL5D: goto _LL5E;case 'F': _LL5E: goto
_LL5F;case 'a': _LL5F: goto _LL60;case 'b': _LL60: goto _LL61;case 'c': _LL61: goto _LL62;
case 'd': _LL62: goto _LL63;case 'e': _LL63: goto _LL64;case 'f': _LL64: if(base <= 10)
break;flags &=~((64 | 256)| 128);goto ok;case '+': _LL65: goto _LL66;case '-': _LL66: if(
flags & 64){flags &=~ 64;goto ok;}break;case 'x': _LL67: goto _LL68;case 'X': _LL68: if(
flags & 256  && p.curr == (_dyneither_ptr_plus(({char*_tmp26=buf;_tag_dyneither(
_tmp26,sizeof(char),_get_zero_arr_size(_tmp26,351));}),sizeof(char),1)).curr){
base=16;flags &=~ 256;goto ok;}break;default: _LL69: break;}break;ok:({struct
_dyneither_ptr _tmp27=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char
_tmp28=*((char*)_check_dyneither_subscript(_tmp27,sizeof(char),0));char _tmp29=(
char)c;if(_get_dyneither_size(_tmp27,sizeof(char))== 1  && (_tmp28 == '\000'  && 
_tmp29 != '\000'))_throw_arraybounds();*((char*)_tmp27.curr)=_tmp29;});_IO_getc(
fp);if(_IO_peekc(fp)== - 1){seen_eof ++;break;}}if(flags & 128){if(p.curr > ({char*
_tmp2A=buf;_tag_dyneither(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,351));}).curr){
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);_IO_ungetc((int)*((char*)
_check_dyneither_subscript(p,sizeof(char),0)),fp);}goto match_failure;}c=(int)*((
char*)_check_dyneither_subscript(p,sizeof(char),- 1));if(c == 'x'  || c == 'X'){
_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);_IO_ungetc(c,fp);}if((flags & 8)
== 0){unsigned long res;({struct _dyneither_ptr _tmp2B=p;char _tmp2C=*((char*)
_check_dyneither_subscript(_tmp2B,sizeof(char),0));char _tmp2D='\000';if(
_get_dyneither_size(_tmp2B,sizeof(char))== 1  && (_tmp2C == '\000'  && _tmp2D != '\000'))
_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});if(use_strtoul)res=strtoul((
char*)buf,0,base);else{res=(unsigned long)strtol((char*)buf,0,base);}if(flags & 
16)*Cyc_va_arg_int_ptr(*((void**)_check_dynforward_subscript(ap,sizeof(void*),0)))=(
int)res;else{if(flags & 4)*Cyc_va_arg_short_ptr(*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0)))=(short)res;else{if(flags & 1)*
Cyc_va_arg_int_ptr(*((void**)_check_dynforward_subscript(ap,sizeof(void*),0)))=(
int)res;else{*Cyc_va_arg_int_ptr(*((void**)_check_dynforward_subscript(ap,
sizeof(void*),0)))=(int)res;}}}_dynforward_ptr_inplace_plus_post(& ap,sizeof(void*),
1);nassigned ++;}nread +=(p.curr - ({char*_tmp2E=buf;_tag_dyneither(_tmp2E,sizeof(
char),_get_zero_arr_size(_tmp2E,351));}).curr)/ sizeof(char);break;case 4: _LL4E:
if(width == 0  || width > sizeof(buf)- 1)width=(long long)(sizeof(buf)- 1);flags |=((
64 | 128)| 256)| 512;for(p=({char*_tmp2F=buf;_tag_dyneither(_tmp2F,sizeof(char),
_get_zero_arr_size(_tmp2F,351));});width != 0;width --){c=_IO_peekc(fp);switch(c){
case '0': _LL6C: goto _LL6D;case '1': _LL6D: goto _LL6E;case '2': _LL6E: goto _LL6F;case '3':
_LL6F: goto _LL70;case '4': _LL70: goto _LL71;case '5': _LL71: goto _LL72;case '6': _LL72:
goto _LL73;case '7': _LL73: goto _LL74;case '8': _LL74: goto _LL75;case '9': _LL75: flags &=~(
64 | 128);goto fok;case '+': _LL76: goto _LL77;case '-': _LL77: if(flags & 64){flags &=~ 64;
goto fok;}break;case '.': _LL78: if(flags & 256){flags &=~(64 | 256);goto fok;}break;
case 'e': _LL79: goto _LL7A;case 'E': _LL7A: if((flags & (128 | 512))== 512){flags=(flags
& ~(512 | 256)| 64)| 128;goto fok;}break;default: _LL7B: break;}break;fok:({struct
_dyneither_ptr _tmp30=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char
_tmp31=*((char*)_check_dyneither_subscript(_tmp30,sizeof(char),0));char _tmp32=(
char)c;if(_get_dyneither_size(_tmp30,sizeof(char))== 1  && (_tmp31 == '\000'  && 
_tmp32 != '\000'))_throw_arraybounds();*((char*)_tmp30.curr)=_tmp32;});_IO_getc(
fp);if(_IO_peekc(fp)== - 1){seen_eof ++;break;}}if(flags & 128){if(flags & 512){
while(p.curr > ({char*_tmp33=buf;_tag_dyneither(_tmp33,sizeof(char),
_get_zero_arr_size(_tmp33,351));}).curr){_dyneither_ptr_inplace_plus(& p,sizeof(
char),-1);_IO_ungetc((int)*((char*)_check_dyneither_subscript(p,sizeof(char),0)),
fp);}goto match_failure;}_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);c=(int)*((
char*)_check_dyneither_subscript(p,sizeof(char),0));if(c != 'e'  && c != 'E'){
_IO_ungetc(c,fp);_dyneither_ptr_inplace_plus(& p,sizeof(char),-1);c=(int)*((char*)
_check_dyneither_subscript(p,sizeof(char),0));}_IO_ungetc(c,fp);}if((flags & 8)== 
0){double res;({struct _dyneither_ptr _tmp34=p;char _tmp35=*((char*)
_check_dyneither_subscript(_tmp34,sizeof(char),0));char _tmp36='\000';if(
_get_dyneither_size(_tmp34,sizeof(char))== 1  && (_tmp35 == '\000'  && _tmp36 != '\000'))
_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});res=atof((const char*)buf);
if(flags & 1)*Cyc_va_arg_double_ptr(*((void**)_check_dynforward_subscript(ap,
sizeof(void*),0)))=res;else{*Cyc_va_arg_float_ptr(*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0)))=(float)res;}
_dynforward_ptr_inplace_plus_post(& ap,sizeof(void*),1);nassigned ++;}nread +=(p.curr
- ({char*_tmp37=buf;_tag_dyneither(_tmp37,sizeof(char),_get_zero_arr_size(_tmp37,
351));}).curr)/ sizeof(char);break;default: _LL6B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38[0]=({struct Cyc_Core_Impossible_struct
_tmp39;_tmp39.tag=Cyc_Core_Impossible;_tmp39.f1=({const char*_tmp3A="scanf3";
_tag_dynforward(_tmp3A,sizeof(char),_get_zero_arr_size(_tmp3A,7));});_tmp39;});
_tmp38;}));}}eof_failure: seen_eof ++;input_failure: if(nassigned == 0)nassigned=- 1;
control_failure: match_failure: if((unsigned int)errp)*errp |=2;done: if((
unsigned int)errp  && seen_eof)*errp |=1;return nassigned;}}static struct
_dyneither_ptr Cyc___sccl(char*tab,struct _dyneither_ptr fmt){int c;int n;int v;c=(int)*((
const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,
sizeof(char),1),sizeof(char),0));if(c == '^'){v=1;c=(int)*((const char*)
_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),
sizeof(char),0));}else{v=0;}for(n=0;n < 256;n ++){tab[
_check_known_subscript_notnull(256,n)]=(char)v;}if(c == 0)return
_dyneither_ptr_plus(fmt,sizeof(char),- 1);v=1 - v;for(0;1;0){tab[
_check_known_subscript_notnull(256,c)]=(char)v;doswitch: n=(int)*((const char*)
_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),
sizeof(char),0));switch(n){case 0: _LL7E: return _dyneither_ptr_plus(fmt,sizeof(char),
- 1);case '-': _LL7F: n=(int)*((const char*)_check_dyneither_subscript(fmt,sizeof(
char),0));if(n == ']'  || n < c){c=(int)'-';break;}_dyneither_ptr_inplace_plus_post(&
fmt,sizeof(char),1);do{tab[_check_known_subscript_notnull(256,++ c)]=(char)v;}while(
c < n);goto doswitch;break;case ']': _LL80: return fmt;default: _LL81: c=n;break;}}}
static int Cyc_string_getc(struct _dyneither_ptr*sptr){char c;struct _dyneither_ptr s=*
sptr;if((s.curr == ((struct _dyneither_ptr)_tag_dyneither(0,0,0)).curr  || 
_get_dyneither_size(s,sizeof(char))== 0) || (c=*((const char*)
_check_dyneither_subscript(s,sizeof(char),0)))== '\000')return - 1;*sptr=
_dyneither_ptr_plus(s,sizeof(char),1);return(int)c;}static int Cyc_string_ungetc(
int ignore,struct _dyneither_ptr*sptr){*sptr=_dyneither_ptr_plus(*sptr,sizeof(char),
- 1);return 0;}static int Cyc_string_peekc(struct _dyneither_ptr*sptr){char c;struct
_dyneither_ptr s=*sptr;if((s.curr == ((struct _dyneither_ptr)_tag_dyneither(0,0,0)).curr
 || _get_dyneither_size(s,sizeof(char))== 0) || (c=*((const char*)
_check_dyneither_subscript(s,sizeof(char),0)))== '\000')return - 1;return(int)c;}
int Cyc_vsscanf(struct _dynforward_ptr src1,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap){struct _dyneither_ptr src=_dynforward_to_dyneither(src1);int err=
0;return((int(*)(int(*_IO_getc)(struct _dyneither_ptr*),int(*_IO_ungetc)(int,
struct _dyneither_ptr*),int(*_IO_peekc)(struct _dyneither_ptr*),struct
_dyneither_ptr*fp,struct _dynforward_ptr fmt0,struct _dynforward_ptr ap,int*errp))
Cyc__IO_vfscanf)(Cyc_string_getc,Cyc_string_ungetc,Cyc_string_peekc,& src,fmt,ap,(
int*)& err);}int Cyc_sscanf(struct _dynforward_ptr src,struct _dynforward_ptr fmt,
struct _dynforward_ptr ap){return Cyc_vsscanf(src,fmt,ap);}int Cyc_peekc(struct Cyc___cycFILE*
stream){int c=Cyc_fgetc(stream);Cyc_ungetc(c,stream);return c;}int Cyc_vfscanf(
struct Cyc___cycFILE*stream,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){int
err=0;return((int(*)(int(*_IO_getc)(struct Cyc___cycFILE*),int(*_IO_ungetc)(int,
struct Cyc___cycFILE*),int(*_IO_peekc)(struct Cyc___cycFILE*),struct Cyc___cycFILE*
fp,struct _dynforward_ptr fmt0,struct _dynforward_ptr ap,int*errp))Cyc__IO_vfscanf)(
Cyc_getc,Cyc_ungetc,Cyc_peekc,stream,fmt,ap,(int*)& err);}int Cyc_fscanf(struct Cyc___cycFILE*
stream,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){return Cyc_vfscanf(
stream,fmt,ap);}int Cyc_scanf(struct _dynforward_ptr fmt,struct _dynforward_ptr ap){
return Cyc_vfscanf(Cyc_stdin,fmt,ap);}
