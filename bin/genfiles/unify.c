#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator  The corresponding definitions are in file lib/runtime_*.c */
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

/* Need one of these per thread (see runtime_stack.c). The runtime maintains 
   a stack that contains either _handler_cons structs or _RegionHandle structs.
   The tag is 0 for a handler_cons and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; 
  struct _RuntimeStack *next;
  void (*cleanup)(struct _RuntimeStack *frame);
};

#ifndef offsetof
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Fat pointers */
struct _fat_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Regions */
struct _RegionPage
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
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
// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void   _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void   _pop_dynregion();

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons *);
void _push_region(struct _RegionHandle *);
void _npop_handler(int);
void _pop_handler();
void _pop_region();

#ifndef _throw
void* _throw_null_fn(const char*,unsigned);
void* _throw_arraybounds_fn(const char*,unsigned);
void* _throw_badalloc_fn(const char*,unsigned);
void* _throw_match_fn(const char*,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ typeof(ptr) _cks_null = (ptr); \
     if (!_cks_null) _throw_null(); \
     _cks_null; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index)\
   (((char*)ptr) + (elt_sz)*(index))
#ifdef NO_CYC_NULL_CHECKS
#define _check_known_subscript_null _check_known_subscript_notnull
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr);\
  int _index = (index);\
  if (!_cks_ptr) _throw_null(); \
  _cks_ptr + (elt_sz)*_index; })
#endif
#define _zero_arr_plus_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_char_fn _zero_arr_plus_fn
#define _zero_arr_plus_short_fn _zero_arr_plus_fn
#define _zero_arr_plus_int_fn _zero_arr_plus_fn
#define _zero_arr_plus_float_fn _zero_arr_plus_fn
#define _zero_arr_plus_double_fn _zero_arr_plus_fn
#define _zero_arr_plus_longdouble_fn _zero_arr_plus_fn
#define _zero_arr_plus_voidstar_fn _zero_arr_plus_fn
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= (bound)) _throw_arraybounds(); \
  _cks_ptr + (elt_sz)*_cks_index; })
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_index = (index); \
  if (_cks_index >= (bound)) _throw_arraybounds(); \
  _cks_ptr + (elt_sz)*_cks_index; })

/* _zero_arr_plus_*_fn(x,sz,i,filename,lineno) adds i to zero-terminated ptr
   x that has at least sz elements */
char* _zero_arr_plus_char_fn(char*,unsigned,int,const char*,unsigned);
short* _zero_arr_plus_short_fn(short*,unsigned,int,const char*,unsigned);
int* _zero_arr_plus_int_fn(int*,unsigned,int,const char*,unsigned);
float* _zero_arr_plus_float_fn(float*,unsigned,int,const char*,unsigned);
double* _zero_arr_plus_double_fn(double*,unsigned,int,const char*,unsigned);
long double* _zero_arr_plus_longdouble_fn(long double*,unsigned,int,const char*, unsigned);
void** _zero_arr_plus_voidstar_fn(void**,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
int _get_zero_arr_size_char(const char*,unsigned);
int _get_zero_arr_size_short(const short*,unsigned);
int _get_zero_arr_size_int(const int*,unsigned);
int _get_zero_arr_size_float(const float*,unsigned);
int _get_zero_arr_size_double(const double*,unsigned);
int _get_zero_arr_size_longdouble(const long double*,unsigned);
int _get_zero_arr_size_voidstar(const void**,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_short_fn(short**,int,const char*,unsigned);
int* _zero_arr_inplace_plus_int(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_longdouble_fn(long double**,int,const char*,unsigned);
void** _zero_arr_inplace_plus_voidstar_fn(void***,int,const char*,unsigned);
/* like the previous functions, but does post-addition (as in e++) */
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_post_short_fn(short**x,int,const char*,unsigned);
int* _zero_arr_inplace_plus_post_int_fn(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_post_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_post_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_post_longdouble_fn(long double**,int,const char *,unsigned);
void** _zero_arr_inplace_plus_post_voidstar_fn(void***,int,const char*,unsigned);
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
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ({ \
  struct _fat_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
    _throw_arraybounds(); \
  _curr; })
#endif

#define _tag_fat(tcurr,elt_sz,num_elts) ({ \
  struct _fat_ptr _ans; \
  unsigned _num_elts = (num_elts);\
  _ans.base = _ans.curr = (void*)(tcurr); \
  /* JGM: if we're tagging NULL, ignore num_elts */ \
  _ans.last_plus_one = _ans.base ? (_ans.base + (elt_sz) * _num_elts) : 0; \
  _ans; })

#define _get_fat_size(arr,elt_sz) \
  ({struct _fat_ptr _arr = (arr); \
    unsigned char *_arr_curr=_arr.curr; \
    unsigned char *_arr_last=_arr.last_plus_one; \
    (_arr_curr < _arr.base || _arr_curr >= _arr_last) ? 0 : \
    ((_arr_last - _arr_curr) / (elt_sz));})

#define _fat_ptr_plus(arr,elt_sz,change) ({ \
  struct _fat_ptr _ans = (arr); \
  int _change = (change);\
  _ans.curr += (elt_sz) * _change;\
  _ans; })
#define _fat_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _fat_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += (elt_sz) * (change);\
  *_arr_ptr; })
#define _fat_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _fat_ptr * _arr_ptr = (arr_ptr); \
  struct _fat_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += (elt_sz) * (change);\
  _ans; })

//Not a macro since initialization order matters. Defined in runtime_zeroterm.c.
struct _fat_ptr _fat_ptr_decrease_size(struct _fat_ptr,unsigned sz,unsigned numelts);

#ifdef CYC_GC_PTHREAD_REDIRECTS
# define pthread_create GC_pthread_create
# define pthread_sigmask GC_pthread_sigmask
# define pthread_join GC_pthread_join
# define pthread_detach GC_pthread_detach
# define dlopen GC_dlopen
#endif
/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);

#if(defined(__linux__) && defined(__KERNEL__))
void *cyc_vmalloc(unsigned);
void cyc_vfree(void *);
#endif
// bound the allocation size to be < MAX_ALLOC_SIZE. See macros below for usage.
#define MAX_MALLOC_SIZE (1 << 28)
void* _bounded_GC_malloc(int,const char*,int);
void* _bounded_GC_malloc_atomic(int,const char*,int);
void* _bounded_GC_calloc(unsigned,unsigned,const char*,int);
void* _bounded_GC_calloc_atomic(unsigned,unsigned,const char*,int);
/* these macros are overridden below ifdef CYC_REGION_PROFILE */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n) _bounded_GC_malloc(n,__FILE__,__LINE__)
#define _cycalloc_atomic(n) _bounded_GC_malloc_atomic(n,__FILE__,__LINE__)
#define _cyccalloc(n,s) _bounded_GC_calloc(n,s,__FILE__,__LINE__)
#define _cyccalloc_atomic(n,s) _bounded_GC_calloc_atomic(n,s,__FILE__,__LINE__)
#endif

static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long) x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,unsigned,unsigned,const char *,const char*,int);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];
# 190 "list.h"
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 46 "position.h"
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 173
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 176
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 181
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 870 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 901
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);
# 930
void*Cyc_Absyn_bounds_one (void);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 71 "warn.h"
void*Cyc_Warn_impos2(struct _fat_ptr);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 76
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 131 "relations-ap.h"
int Cyc_Relations_check_logical_implication(struct Cyc_List_List*,struct Cyc_List_List*);
# 100 "tcutil.h"
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 140
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 197
void*Cyc_Tcutil_normalize_effect(void*);
# 246
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 250
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_tycon_cmp(void*,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 71
struct _fat_ptr Cyc_Kinds_kind2string(struct Cyc_Absyn_Kind*);
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 54 "attributes.h"
int Cyc_Atts_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 58
int Cyc_Atts_equiv_fn_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 28 "unify.h"
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
# 32
void Cyc_Unify_occurs(void*,struct _RegionHandle*,struct Cyc_List_List*,void*);char Cyc_Unify_Unify[6U]="Unify";struct Cyc_Unify_Unify_exn_struct{char*tag;};
# 35 "unify.cyc"
struct Cyc_Unify_Unify_exn_struct Cyc_Unify_Unify_val={Cyc_Unify_Unify};struct _tuple11{void*f1;void*f2;};
# 38
static struct _tuple11 Cyc_Unify_ts_failure={0,0};struct _tuple12{int f1;int f2;};
static struct _tuple12 Cyc_Unify_tqs_const={0,0};
static struct _fat_ptr Cyc_Unify_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 42
static void Cyc_Unify_fail_because(struct _fat_ptr reason){
Cyc_Unify_failure_reason=reason;
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 50
void Cyc_Unify_explain_failure (void){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)
return;
Cyc_fflush(Cyc_stderr);
# 56
if(Cyc_strcmp(({const char*_Tmp0="(qualifiers don't match)";_tag_fat(_Tmp0,sizeof(char),25U);}),Cyc_Unify_failure_reason)== 0){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_Unify_failure_reason;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="  %s\n";_tag_fat(_Tmp2,sizeof(char),6U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return;}
# 61
if(Cyc_strcmp(({const char*_Tmp0="(function effects do not match)";_tag_fat(_Tmp0,sizeof(char),32U);}),Cyc_Unify_failure_reason)== 0){
struct Cyc_Absynpp_Params p=Cyc_Absynpp_tc_params_r;
p.print_all_effects=1;
Cyc_Absynpp_set_params(& p);}{
# 66
struct _tuple11 _Tmp0=Cyc_Unify_ts_failure;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t1f=_Tmp2;void*t2f=_Tmp1;
struct _fat_ptr s1=(unsigned)t1f?Cyc_Absynpp_typ2string(t1f):({const char*_Tmp3="<?>";_tag_fat(_Tmp3,sizeof(char),4U);});
struct _fat_ptr s2=(unsigned)t2f?Cyc_Absynpp_typ2string(t2f):({const char*_Tmp3="<?>";_tag_fat(_Tmp3,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s1;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="  %s";_tag_fat(_Tmp5,sizeof(char),5U);}),_tag_fat(_Tmp4,sizeof(void*),1));});
pos +=_get_fat_size(s1,sizeof(char));
if(pos + 5 >= 80){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="\n\t";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));
pos=8;}else{
# 76
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3=" ";_tag_fat(_Tmp3,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));
++ pos;}
# 79
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="and ";_tag_fat(_Tmp3,sizeof(char),5U);}),_tag_fat(0U,sizeof(void*),0));
pos +=4;
if((unsigned)pos + _get_fat_size(s2,sizeof(char))>= 80U){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="\n\t";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));
pos=8;}
# 85
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s2;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="%s ";_tag_fat(_Tmp5,sizeof(char),4U);}),_tag_fat(_Tmp4,sizeof(void*),1));});
pos +=_get_fat_size(s2,sizeof(char))+ 1U;
if(pos + 17 >= 80){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="\n\t";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));
pos=8;}
# 91
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="are not compatible. ";_tag_fat(_Tmp3,sizeof(char),21U);}),_tag_fat(0U,sizeof(void*),0));
pos +=17;
if((char*)Cyc_Unify_failure_reason.curr != (char*)(_tag_fat(0,0,0)).curr){
if(({unsigned long _Tmp3=(unsigned long)pos;_Tmp3 + Cyc_strlen(Cyc_Unify_failure_reason);})>= 80U)
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="\n\t";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=Cyc_Unify_failure_reason;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="%s";_tag_fat(_Tmp5,sizeof(char),3U);}),_tag_fat(_Tmp4,sizeof(void*),1));});}
# 98
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="\n";_tag_fat(_Tmp3,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_fflush(Cyc_stderr);}}}
# 104
static int Cyc_Unify_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
# 106
if(r1 == r2)return 1;
return Cyc_Relations_check_logical_implication(r1,r2)&&
 Cyc_Relations_check_logical_implication(r2,r1);}
# 111
int Cyc_Unify_unify_kindbound(void*kb1,void*kb2){
struct _tuple11 _stmttmp0=({struct _tuple11 _Tmp0;({void*_Tmp1=Cyc_Kinds_compress_kb(kb1);_Tmp0.f1=_Tmp1;}),({void*_Tmp1=Cyc_Kinds_compress_kb(kb2);_Tmp0.f2=_Tmp1;});_Tmp0;});struct _tuple11 _Tmp0=_stmttmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0.f1)){case 0: switch(*((int*)_Tmp0.f2)){case 0: _Tmp4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Kind*k1=_Tmp4;struct Cyc_Absyn_Kind*k2=_Tmp3;
return k1 == k2;}case 2: _Tmp4=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;_Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f1;_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f2;_LL8: {struct Cyc_Absyn_Kind*k1=_Tmp4;struct Cyc_Core_Opt**x=_Tmp3;struct Cyc_Absyn_Kind*k2=_Tmp2;
# 122
if(!Cyc_Kinds_kind_leq(k1,k2))
return 0;
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=kb1;_Tmp6;});*x=_Tmp5;});
return 1;}default: goto _LLB;}case 2: switch(*((int*)_Tmp0.f2)){case 0:
# 114
 return Cyc_Unify_unify_kindbound(kb2,kb1);case 2: _Tmp4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f2;_Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f1;_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f2;{struct Cyc_Core_Opt**y=_Tmp4;struct Cyc_Absyn_Kind*k1=_Tmp3;struct Cyc_Core_Opt**x=(struct Cyc_Core_Opt**)_Tmp2;struct Cyc_Absyn_Kind*k2=_Tmp1;
# 116
if(Cyc_Kinds_kind_leq(k2,k1)){
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=kb2;_Tmp6;});*y=_Tmp5;});
return 1;}
# 120
_Tmp4=k1;_Tmp3=x;_Tmp2=k2;goto _LL8;}default: _LLB: _Tmp4=_Tmp0.f1;_Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0.f2)->f1;_LLC: {void*y=_Tmp4;struct Cyc_Core_Opt**x=_Tmp3;
# 128
({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=y;_Tmp6;});*x=_Tmp5;});
return 1;}}default: _Tmp4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0.f1)->f1;_Tmp3=_Tmp0.f2;{struct Cyc_Core_Opt**x=(struct Cyc_Core_Opt**)_Tmp4;void*y=_Tmp3;
# 126
_Tmp4=y;_Tmp3=x;goto _LLC;}};}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};
# 135
void Cyc_Unify_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Absyn_compress(t);{
void*_Tmp0=t;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;struct Cyc_Absyn_PtrInfo _Tmp6;void*_Tmp7;void*_Tmp8;switch(*((int*)_Tmp0)){case 2: _Tmp8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp8;
# 139
if(!({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,env,tv))
Cyc_Unify_fail_because(({const char*_Tmp9="(type variable would escape scope)";_tag_fat(_Tmp9,sizeof(char),35U);}));
goto _LL0;}case 1: _Tmp8=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f2;_Tmp7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0)->f4;{void*rg=_Tmp8;struct Cyc_Core_Opt**sopt=_Tmp7;
# 143
if(t == evar)
Cyc_Unify_fail_because(({const char*_Tmp9="(occurs check)";_tag_fat(_Tmp9,sizeof(char),15U);}));
if(rg != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB="occurs check: constrained Evar in compressed type";_tag_fat(_TmpB,sizeof(char),50U);});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpA,sizeof(void*),1));});{
# 148
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)(_check_null(*sopt))->v;for(0;s != 0 && !problem;s=s->tl){
if(!({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
problem=1;}}
# 153
if(problem){
struct Cyc_List_List*result=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)(_check_null(*sopt))->v;for(0;s != 0;s=s->tl){
if(({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
result=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=(struct Cyc_Absyn_Tvar*)s->hd,_Tmp9->tl=result;_Tmp9;});}}
({struct Cyc_Core_Opt*_Tmp9=({struct Cyc_Core_Opt*_TmpA=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpA->v=result;_TmpA;});*sopt=_Tmp9;});}
# 160
goto _LL0;}}case 3: _Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo pinfo=_Tmp6;
# 162
Cyc_Unify_occurs(evar,r,env,pinfo.elt_type);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).rgn);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).nullable);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).bounds);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).zero_term);
Cyc_Unify_occurs(evar,r,env,(pinfo.ptr_atts).autoreleased);
goto _LL0;}case 4: _Tmp8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).zero_term;{void*t2=_Tmp8;void*zt=_Tmp7;
# 171
Cyc_Unify_occurs(evar,r,env,t2);
Cyc_Unify_occurs(evar,r,env,zt);
goto _LL0;}case 5: _Tmp8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).tvars;_Tmp7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).effect;_Tmp5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ret_tqual;_Tmp4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).ret_type;_Tmp3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).args;_Tmp2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).cyc_varargs;_Tmp1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1).rgn_po;{struct Cyc_List_List*tvs=_Tmp8;void*eff=_Tmp7;struct Cyc_Absyn_Tqual rt_tq=_Tmp5;void*rt=_Tmp4;struct Cyc_List_List*args=_Tmp3;struct Cyc_Absyn_VarargInfo*cyc_varargs=_Tmp2;struct Cyc_List_List*rgn_po=_Tmp1;
# 176
env=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rappend;})(r,tvs,env);
if(eff != 0)
Cyc_Unify_occurs(evar,r,env,eff);
Cyc_Unify_occurs(evar,r,env,rt);
for(1;args != 0;args=args->tl){
Cyc_Unify_occurs(evar,r,env,(*((struct _tuple8*)args->hd)).f3);}
if(cyc_varargs != 0)
Cyc_Unify_occurs(evar,r,env,cyc_varargs->type);
for(1;rgn_po != 0;rgn_po=rgn_po->tl){
struct _tuple11*_stmttmp1=(struct _tuple11*)rgn_po->hd;struct _tuple11*_Tmp9=_stmttmp1;void*_TmpA;void*_TmpB;_TmpB=_Tmp9->f1;_TmpA=_Tmp9->f2;{void*r1=_TmpB;void*r2=_TmpA;
Cyc_Unify_occurs(evar,r,env,r1);
Cyc_Unify_occurs(evar,r,env,r2);}}
# 189
goto _LL0;}case 6: _Tmp8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_List_List*args=_Tmp8;
# 191
for(1;args != 0;args=args->tl){
Cyc_Unify_occurs(evar,r,env,(*((struct _tuple13*)args->hd)).f2);}
goto _LL0;}case 7: _Tmp8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*fs=_Tmp8;
# 196
for(1;fs != 0;fs=fs->tl){
Cyc_Unify_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);}
goto _LL0;}case 8: _Tmp8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp8;
_Tmp8=ts;goto _LL12;}case 0: _Tmp8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;_LL12: {struct Cyc_List_List*ts=_Tmp8;
# 201
for(1;ts != 0;ts=ts->tl){
Cyc_Unify_occurs(evar,r,env,(void*)ts->hd);}
goto _LL0;}default:
# 206
 goto _LL0;}_LL0:;}}
# 210
static void Cyc_Unify_unify_it(void*,void*);
# 213
int Cyc_Unify_unify(void*t1,void*t2){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
Cyc_Unify_unify_it(t1,t2);{
int _Tmp2=1;_npop_handler(0);return _Tmp2;}
# 215
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3=_Tmp2;void*_Tmp4;if(((struct Cyc_Unify_Unify_exn_struct*)_Tmp3)->tag == Cyc_Unify_Unify)
# 223
return 0;else{_Tmp4=_Tmp3;{void*exn=_Tmp4;(int)_rethrow(exn);}};}}}
# 228
static void Cyc_Unify_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(1;t1 != 0 && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Unify_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0 || t2 != 0)
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 236
static void Cyc_Unify_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const && !tq1.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="tq1 real_const not set.";_tag_fat(_Tmp2,sizeof(char),24U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
if(tq2.print_const && !tq2.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="tq2 real_const not set.";_tag_fat(_Tmp2,sizeof(char),24U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
# 242
if((tq1.real_const != tq2.real_const || tq1.q_volatile != tq2.q_volatile)|| tq1.q_restrict != tq2.q_restrict){
# 245
Cyc_Unify_ts_failure=({struct _tuple11 _Tmp0;_Tmp0.f1=t1,_Tmp0.f2=t2;_Tmp0;});
Cyc_Unify_tqs_const=({struct _tuple12 _Tmp0;_Tmp0.f1=tq1.real_const,_Tmp0.f2=tq2.real_const;_Tmp0;});
Cyc_Unify_failure_reason=({const char*_Tmp0="(qualifiers don't match)";_tag_fat(_Tmp0,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Unify_Unify_val);}
# 251
Cyc_Unify_tqs_const=({struct _tuple12 _Tmp0;_Tmp0.f1=0,_Tmp0.f2=0;_Tmp0;});}
# 265 "unify.cyc"
static int Cyc_Unify_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2)&& Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2)&& Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 275
static int Cyc_Unify_unify_const_exp_opt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
if(e1 == 0 && e2 == 0)
return 1;
if(e1 == 0 || e2 == 0)
return 0;
return Cyc_Evexp_same_uint_const_exp(e1,e2);}struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple15{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 284
static void Cyc_Unify_unify_it(void*t1,void*t2){
Cyc_Unify_ts_failure=({struct _tuple11 _Tmp0;_Tmp0.f1=t1,_Tmp0.f2=t2;_Tmp0;});
Cyc_Unify_failure_reason=_tag_fat(0,0,0);
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(t1 == t2)return;{
struct _tuple11 _stmttmp2=({struct _tuple11 _Tmp0;_Tmp0.f1=t2,_Tmp0.f2=t1;_Tmp0;});struct _tuple11 _Tmp0=_stmttmp2;enum Cyc_Absyn_AggrKind _Tmp1;enum Cyc_Absyn_AggrKind _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;int _TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;int _Tmp10;void*_Tmp11;void*_Tmp12;void*_Tmp13;void*_Tmp14;void*_Tmp15;struct Cyc_Absyn_Tqual _Tmp16;void*_Tmp17;void*_Tmp18;struct Cyc_Absyn_Tqual _Tmp19;void*_Tmp1A;void*_Tmp1B;void*_Tmp1C;void*_Tmp1D;void*_Tmp1E;if(*((int*)_Tmp0.f1)== 1){if(*((int*)_Tmp0.f2)== 1){_Tmp1E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp1C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f1)->f4;_Tmp1B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f2)->f1;_Tmp1A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f2)->f4;{struct Cyc_Core_Opt**kind1opt=_Tmp1E;void**t2r=_Tmp1D;struct Cyc_Core_Opt**s1opt=_Tmp1C;struct Cyc_Core_Opt**kind2opt=_Tmp1B;struct Cyc_Core_Opt**s2opt=_Tmp1A;
# 292
if(({struct Cyc_Absyn_Kind*_Tmp1F=(struct Cyc_Absyn_Kind*)(_check_null(*kind1opt))->v;Cyc_Kinds_kind_leq(_Tmp1F,(struct Cyc_Absyn_Kind*)(_check_null(*kind2opt))->v);}))
*kind2opt=*kind1opt;else{
if(({struct Cyc_Absyn_Kind*_Tmp1F=(struct Cyc_Absyn_Kind*)(_check_null(*kind2opt))->v;Cyc_Kinds_kind_leq(_Tmp1F,(struct Cyc_Absyn_Kind*)(_check_null(*kind1opt))->v);}))
*kind1opt=*kind2opt;else{
# 297
Cyc_Unify_fail_because(({const char*_Tmp1F="(kinds are incompatible)";_tag_fat(_Tmp1F,sizeof(char),25U);}));}}
*t2r=t1;{
# 300
struct Cyc_List_List*s1=(struct Cyc_List_List*)(_check_null(*s1opt))->v;
struct Cyc_List_List*s2=(struct Cyc_List_List*)(_check_null(*s2opt))->v;
if(s1 == s2)return;
for(1;s1 != 0;s1=s1->tl){
if(!({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,s2,(struct Cyc_Absyn_Tvar*)s1->hd))
break;}
if(s1 == 0){
*s2opt=*s1opt;
return;}
# 310
s1=(struct Cyc_List_List*)(_check_null(*s1opt))->v;
for(1;s2 != 0;s2=s2->tl){
if(!({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,s1,(struct Cyc_Absyn_Tvar*)s2->hd))
break;}
if(s2 == 0){
*s1opt=*s2opt;
return;}
# 318
s2=(struct Cyc_List_List*)(_check_null(*s2opt))->v;{
struct Cyc_List_List*ans=0;
for(1;s2 != 0;s2=s2->tl){
if(({(int(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_Absyn_Tvar*))Cyc_List_mem;})(Cyc_Tcutil_fast_tvar_cmp,s1,(struct Cyc_Absyn_Tvar*)s2->hd))
ans=({struct Cyc_List_List*_Tmp1F=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1F->hd=(struct Cyc_Absyn_Tvar*)s2->hd,_Tmp1F->tl=ans;_Tmp1F;});}
({struct Cyc_Core_Opt*_Tmp1F=({struct Cyc_Core_Opt*_Tmp20=({struct Cyc_Core_Opt*_Tmp21=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp21->v=ans;_Tmp21;});*s2opt=_Tmp20;});*s1opt=_Tmp1F;});
return;}}}}else{
# 354
Cyc_Unify_unify_it(t2,t1);return;}}else{if(*((int*)_Tmp0.f2)== 1){_Tmp1E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f2)->f1;_Tmp1D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f2)->f2;_Tmp1C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp0.f2)->f4;{struct Cyc_Core_Opt*kind1=_Tmp1E;void**ref1_ref=_Tmp1D;struct Cyc_Core_Opt*s1opt=_Tmp1C;
# 329
Cyc_Unify_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)(_check_null(s1opt))->v,t2);{
struct Cyc_Absyn_Kind*kind2=Cyc_Tcutil_type_kind(t2);
# 333
if(Cyc_Kinds_kind_leq(kind2,(struct Cyc_Absyn_Kind*)(_check_null(kind1))->v)){
*ref1_ref=t2;
return;}{
# 340
void*_Tmp1F=t2;struct Cyc_Absyn_PtrInfo _Tmp20;if(*((int*)_Tmp1F)== 3){_Tmp20=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp1F)->f1;if((int)((struct Cyc_Absyn_Kind*)kind1->v)->kind == 2){struct Cyc_Absyn_PtrInfo pinfo=_Tmp20;
# 342
void*c=Cyc_Absyn_compress((pinfo.ptr_atts).bounds);
void*_Tmp21=c;if(*((int*)_Tmp21)== 1){
# 345
({void*_Tmp22=c;Cyc_Unify_unify(_Tmp22,Cyc_Absyn_bounds_one());});
*ref1_ref=t2;
return;}else{
Cyc_Unify_fail_because(({const char*_Tmp22="(kinds are incompatible)";_tag_fat(_Tmp22,sizeof(char),25U);}));};}else{goto _LL2A;}}else{_LL2A:
# 350
 Cyc_Unify_fail_because(({const char*_Tmp21="(kinds are incompatible)";_tag_fat(_Tmp21,sizeof(char),25U);}));};}}}}else{if(*((int*)_Tmp0.f1)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)== 9)
# 356
goto _LLA;else{if(*((int*)_Tmp0.f2)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)== 9)goto _LL9;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)== 8)goto _LLB;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)== 8)goto _LLD;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)== 10)goto _LLF;else{if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)== 10)goto _LL11;else{_Tmp1E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp1C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1;_Tmp1B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f2;{void*c1=_Tmp1E;struct Cyc_List_List*ts1=_Tmp1D;void*c2=_Tmp1C;struct Cyc_List_List*ts2=_Tmp1B;
# 367
if(Cyc_Tcutil_tycon_cmp(c1,c2)!= 0)
Cyc_Unify_fail_because(({const char*_Tmp1F="(different type constructors)";_tag_fat(_Tmp1F,sizeof(char),30U);}));
Cyc_Unify_unify_list(ts1,ts2);
return;}}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)){case 8: _LLB: _LLC:
# 358
 goto _LLE;case 10: _LLF: _LL10:
# 360
 goto _LL12;default: goto _LL25;}}}}else{if(*((int*)_Tmp0.f2)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)){case 9: _LL9: _LLA:
# 357
 goto _LLC;case 8: _LLD: _LLE:
# 359
 goto _LL10;case 10: _LL11: _LL12:
# 362
 if(!Cyc_Unify_unify_effect(t1,t2))
Cyc_Unify_fail_because(({const char*_Tmp1F="(effects don't unify)";_tag_fat(_Tmp1F,sizeof(char),22U);}));
return;default: switch(*((int*)_Tmp0.f1)){case 2: goto _LL25;case 3: goto _LL25;case 9: goto _LL25;case 4: goto _LL25;case 5: goto _LL25;case 6: goto _LL25;case 7: goto _LL25;case 8: goto _LL25;default: goto _LL25;}}else{switch(*((int*)_Tmp0.f1)){case 2: if(*((int*)_Tmp0.f2)== 2){_Tmp1E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Tvar*tv2=_Tmp1E;struct Cyc_Absyn_Tvar*tv1=_Tmp1D;
# 374
if(tv2->identity != tv1->identity)
Cyc_Unify_fail_because(({const char*_Tmp1F="(variable types are not the same)";_tag_fat(_Tmp1F,sizeof(char),34U);}));
return;}}else{goto _LL25;}case 3: if(*((int*)_Tmp0.f2)== 3){_Tmp1E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1).elt_type;_Tmp19=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1).elt_tq;_Tmp1D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1).ptr_atts).rgn;_Tmp1C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1).ptr_atts).nullable;_Tmp1B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1).ptr_atts).bounds;_Tmp1A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1).ptr_atts).zero_term;_Tmp18=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1).ptr_atts).autoreleased;_Tmp17=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1).elt_type;_Tmp16=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1).elt_tq;_Tmp15=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1).ptr_atts).rgn;_Tmp14=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1).ptr_atts).nullable;_Tmp13=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1).ptr_atts).bounds;_Tmp12=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1).ptr_atts).zero_term;_Tmp11=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1).ptr_atts).autoreleased;{void*t2a=_Tmp1E;struct Cyc_Absyn_Tqual tqual2a=_Tmp19;void*rgn2=_Tmp1D;void*null2a=_Tmp1C;void*b2=_Tmp1B;void*zt2=_Tmp1A;void*rel2=_Tmp18;void*t1a=_Tmp17;struct Cyc_Absyn_Tqual tqual1a=_Tmp16;void*rgn1=_Tmp15;void*null1a=_Tmp14;void*b1=_Tmp13;void*zt1=_Tmp12;void*rel1=_Tmp11;
# 380
Cyc_Unify_unify_it(t1a,t2a);
Cyc_Unify_unify_it(rgn2,rgn1);{
struct _fat_ptr orig_failure=Cyc_Unify_failure_reason;
if(!Cyc_Unify_unify(zt1,zt2)){
Cyc_Unify_ts_failure=({struct _tuple11 _Tmp1F;_Tmp1F.f1=t1,_Tmp1F.f2=t2;_Tmp1F;});
Cyc_Unify_fail_because(({const char*_Tmp1F="(not both zero terminated)";_tag_fat(_Tmp1F,sizeof(char),27U);}));}
# 387
if(!Cyc_Unify_unify(rel1,rel2)){
Cyc_Unify_ts_failure=({struct _tuple11 _Tmp1F;_Tmp1F.f1=t1,_Tmp1F.f2=t2;_Tmp1F;});
Cyc_Unify_fail_because(({const char*_Tmp1F="(not both autoreleased)";_tag_fat(_Tmp1F,sizeof(char),24U);}));}
# 391
Cyc_Unify_unify_tqual(tqual1a,t1a,tqual2a,t2a);
if(!Cyc_Unify_unify(b1,b2)){
Cyc_Unify_ts_failure=({struct _tuple11 _Tmp1F;_Tmp1F.f1=t1,_Tmp1F.f2=t2;_Tmp1F;});
Cyc_Unify_fail_because(({const char*_Tmp1F="(different pointer bounds)";_tag_fat(_Tmp1F,sizeof(char),27U);}));}{
# 397
void*_stmttmp3=Cyc_Absyn_compress(b1);void*_Tmp1F=_stmttmp3;if(*((int*)_Tmp1F)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1F)->f1)== 14){
# 399
Cyc_Unify_failure_reason=orig_failure;
return;}else{goto _LL34;}}else{_LL34:
# 402
 Cyc_Unify_failure_reason=({const char*_Tmp20="(incompatible pointer types)";_tag_fat(_Tmp20,sizeof(char),29U);});
Cyc_Unify_unify_it(null1a,null2a);
return;};}}}}else{goto _LL25;}case 9: if(*((int*)_Tmp0.f2)== 9){_Tmp1E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1E;struct Cyc_Absyn_Exp*e2=_Tmp1D;
# 408
if(!Cyc_Evexp_same_uint_const_exp(e1,e2))
Cyc_Unify_fail_because(({const char*_Tmp1F="(cannot prove expressions are the same)";_tag_fat(_Tmp1F,sizeof(char),40U);}));
return;}}else{goto _LL25;}case 4: if(*((int*)_Tmp0.f2)== 4){_Tmp1E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1).elt_type;_Tmp19=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1).tq;_Tmp1D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1).num_elts;_Tmp1C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f1)->f1).zero_term;_Tmp1B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1).elt_type;_Tmp16=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1).tq;_Tmp1A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1).num_elts;_Tmp18=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f2)->f1).zero_term;{void*t2a=_Tmp1E;struct Cyc_Absyn_Tqual tq2a=_Tmp19;struct Cyc_Absyn_Exp*e1=_Tmp1D;void*zt1=_Tmp1C;void*t1a=_Tmp1B;struct Cyc_Absyn_Tqual tq1a=_Tmp16;struct Cyc_Absyn_Exp*e2=_Tmp1A;void*zt2=_Tmp18;
# 414
Cyc_Unify_unify_it(t1a,t2a);
Cyc_Unify_unify_tqual(tq1a,t1a,tq2a,t2a);
Cyc_Unify_failure_reason=({const char*_Tmp1F="(not both zero terminated)";_tag_fat(_Tmp1F,sizeof(char),27U);});
Cyc_Unify_unify_it(zt1,zt2);
if(!Cyc_Unify_unify_const_exp_opt(e1,e2))
Cyc_Unify_fail_because(({const char*_Tmp1F="(different array sizes)";_tag_fat(_Tmp1F,sizeof(char),24U);}));
return;}}else{goto _LL25;}case 5: if(*((int*)_Tmp0.f2)== 5){_Tmp1E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).tvars;_Tmp1D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).effect;_Tmp19=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).ret_tqual;_Tmp1C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).ret_type;_Tmp1B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).args;_Tmp10=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).c_varargs;_Tmp1A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).cyc_varargs;_Tmp18=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).rgn_po;_Tmp17=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).attributes;_Tmp15=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).requires_clause;_Tmp14=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).requires_relns;_Tmp13=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).ensures_clause;_Tmp12=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).ensures_relns;_Tmp11=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1).return_value;_TmpF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).tvars;_TmpE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).effect;_Tmp16=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).ret_tqual;_TmpD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).ret_type;_TmpC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).args;_TmpB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).c_varargs;_TmpA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).cyc_varargs;_Tmp9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).rgn_po;_Tmp8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).attributes;_Tmp7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).requires_clause;_Tmp6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).requires_relns;_Tmp5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).ensures_clause;_Tmp4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).ensures_relns;_Tmp3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f2)->f1).return_value;{struct Cyc_List_List*tvs2=_Tmp1E;void*eff2=_Tmp1D;struct Cyc_Absyn_Tqual rt_tq2=_Tmp19;void*rt2=_Tmp1C;struct Cyc_List_List*args2=_Tmp1B;int c_vararg2=_Tmp10;struct Cyc_Absyn_VarargInfo*cyc_vararg2=_Tmp1A;struct Cyc_List_List*rpo2=_Tmp18;struct Cyc_List_List*atts2=_Tmp17;struct Cyc_Absyn_Exp*req2=_Tmp15;struct Cyc_List_List*req_relns2=_Tmp14;struct Cyc_Absyn_Exp*ens2=_Tmp13;struct Cyc_List_List*ens_relns2=_Tmp12;struct Cyc_Absyn_Vardecl*return_value2=_Tmp11;struct Cyc_List_List*tvs1=_TmpF;void*eff1=_TmpE;struct Cyc_Absyn_Tqual rt_tq1=_Tmp16;void*rt1=_TmpD;struct Cyc_List_List*args1=_TmpC;int c_vararg1=_TmpB;struct Cyc_Absyn_VarargInfo*cyc_vararg1=_TmpA;struct Cyc_List_List*rpo1=_Tmp9;struct Cyc_List_List*atts1=_Tmp8;struct Cyc_Absyn_Exp*req1=_Tmp7;struct Cyc_List_List*req_relns1=_Tmp6;struct Cyc_Absyn_Exp*ens1=_Tmp5;struct Cyc_List_List*ens_relns1=_Tmp4;struct Cyc_Absyn_Vardecl*return_value1=_Tmp3;
# 424
{struct _RegionHandle _Tmp1F=_new_region("rgn");struct _RegionHandle*rgn=& _Tmp1F;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(tvs2 == 0)
Cyc_Unify_fail_because(({const char*_Tmp20="(second function type has too few type variables)";_tag_fat(_Tmp20,sizeof(char),50U);}));{
void*kb1=((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind;
void*kb2=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
if(!Cyc_Unify_unify_kindbound(kb1,kb2))
Cyc_Unify_fail_because(({struct Cyc_String_pa_PrintArg_struct _Tmp20=({struct Cyc_String_pa_PrintArg_struct _Tmp21;_Tmp21.tag=0,({
struct _fat_ptr _Tmp22=Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)tvs1->hd);_Tmp21.f1=_Tmp22;});_Tmp21;});struct Cyc_String_pa_PrintArg_struct _Tmp21=({struct Cyc_String_pa_PrintArg_struct _Tmp22;_Tmp22.tag=0,({
struct _fat_ptr _Tmp23=Cyc_Kinds_kind2string(Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvs1->hd,& Cyc_Kinds_bk));_Tmp22.f1=_Tmp23;});_Tmp22;});struct Cyc_String_pa_PrintArg_struct _Tmp22=({struct Cyc_String_pa_PrintArg_struct _Tmp23;_Tmp23.tag=0,({
# 436
struct _fat_ptr _Tmp24=Cyc_Kinds_kind2string(Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvs2->hd,& Cyc_Kinds_bk));_Tmp23.f1=_Tmp24;});_Tmp23;});void*_Tmp23[3];_Tmp23[0]=& _Tmp20,_Tmp23[1]=& _Tmp21,_Tmp23[2]=& _Tmp22;Cyc_aprintf(({const char*_Tmp24="(type var %s has different kinds %s and %s)";_tag_fat(_Tmp24,sizeof(char),44U);}),_tag_fat(_Tmp23,sizeof(void*),3));}));
# 438
inst=({struct Cyc_List_List*_Tmp20=_region_malloc(rgn,sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp21=({struct _tuple14*_Tmp22=_region_malloc(rgn,sizeof(struct _tuple14));_Tmp22->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_Tmp23=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_Tmp22->f2=_Tmp23;});_Tmp22;});_Tmp20->hd=_Tmp21;}),_Tmp20->tl=inst;_Tmp20;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}}
# 442
if(tvs2 != 0)
Cyc_Unify_fail_because(({const char*_Tmp20="(second function type has too many type variables)";_tag_fat(_Tmp20,sizeof(char),51U);}));
if(inst != 0){
({void*_Tmp20=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp21=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp21->tag=5,(_Tmp21->f1).tvars=0,(_Tmp21->f1).effect=eff1,(_Tmp21->f1).ret_tqual=rt_tq1,(_Tmp21->f1).ret_type=rt1,(_Tmp21->f1).args=args1,(_Tmp21->f1).c_varargs=c_vararg1,(_Tmp21->f1).cyc_varargs=cyc_vararg1,(_Tmp21->f1).rgn_po=rpo1,(_Tmp21->f1).attributes=atts1,(_Tmp21->f1).requires_clause=req1,(_Tmp21->f1).requires_relns=req_relns1,(_Tmp21->f1).ensures_clause=ens1,(_Tmp21->f1).ensures_relns=ens_relns1,(_Tmp21->f1).return_value=return_value1;_Tmp21;});Cyc_Unify_unify_it(_Tmp20,({
# 448
struct _RegionHandle*_Tmp21=rgn;struct Cyc_List_List*_Tmp22=inst;Cyc_Tcutil_rsubstitute(_Tmp21,_Tmp22,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp23=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));
_Tmp23->tag=5,(_Tmp23->f1).tvars=0,(_Tmp23->f1).effect=eff2,(_Tmp23->f1).ret_tqual=rt_tq2,(_Tmp23->f1).ret_type=rt2,(_Tmp23->f1).args=args2,(_Tmp23->f1).c_varargs=c_vararg2,(_Tmp23->f1).cyc_varargs=cyc_vararg2,(_Tmp23->f1).rgn_po=rpo2,(_Tmp23->f1).attributes=atts2,(_Tmp23->f1).requires_clause=req2,(_Tmp23->f1).requires_relns=req_relns2,(_Tmp23->f1).ensures_clause=ens2,(_Tmp23->f1).ensures_relns=ens_relns2,(_Tmp23->f1).return_value=return_value2;_Tmp23;}));}));});
# 453
_npop_handler(0);return;}}
# 425
;_pop_region();}
# 456
Cyc_Unify_unify_it(rt1,rt2);
Cyc_Unify_unify_tqual(rt_tq1,rt1,rt_tq2,rt2);
for(1;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple8 _stmttmp4=*((struct _tuple8*)args1->hd);struct _tuple8 _Tmp1F=_stmttmp4;void*_Tmp20;struct Cyc_Absyn_Tqual _Tmp21;_Tmp21=_Tmp1F.f2;_Tmp20=_Tmp1F.f3;{struct Cyc_Absyn_Tqual tqa=_Tmp21;void*ta=_Tmp20;
struct _tuple8 _stmttmp5=*((struct _tuple8*)args2->hd);struct _tuple8 _Tmp22=_stmttmp5;void*_Tmp23;struct Cyc_Absyn_Tqual _Tmp24;_Tmp24=_Tmp22.f2;_Tmp23=_Tmp22.f3;{struct Cyc_Absyn_Tqual tqb=_Tmp24;void*tb=_Tmp23;
Cyc_Unify_unify_it(ta,tb);
Cyc_Unify_unify_tqual(tqa,ta,tqb,tb);}}}
# 464
Cyc_Unify_ts_failure=({struct _tuple11 _Tmp1F;_Tmp1F.f1=t1,_Tmp1F.f2=t2;_Tmp1F;});
if(args1 != 0 || args2 != 0)
Cyc_Unify_fail_because(({const char*_Tmp1F="(function types have different number of arguments)";_tag_fat(_Tmp1F,sizeof(char),52U);}));
if(c_vararg1 != c_vararg2)
Cyc_Unify_fail_because(({const char*_Tmp1F="(only one function type takes C varargs)";_tag_fat(_Tmp1F,sizeof(char),41U);}));
# 470
{struct _tuple15 _stmttmp6=({struct _tuple15 _Tmp1F;_Tmp1F.f1=cyc_vararg1,_Tmp1F.f2=cyc_vararg2;_Tmp1F;});struct _tuple15 _Tmp1F=_stmttmp6;int _Tmp20;void*_Tmp21;struct Cyc_Absyn_Tqual _Tmp22;void*_Tmp23;int _Tmp24;void*_Tmp25;struct Cyc_Absyn_Tqual _Tmp26;void*_Tmp27;if(_Tmp1F.f1 == 0){if(_Tmp1F.f2 == 0)
goto _LL3C;else{
goto _LL42;}}else{if(_Tmp1F.f2 == 0){_LL42:
 Cyc_Unify_fail_because(({const char*_Tmp28="(only one function type takes varargs)";_tag_fat(_Tmp28,sizeof(char),39U);}));}else{_Tmp27=(_Tmp1F.f1)->name;_Tmp26=(_Tmp1F.f1)->tq;_Tmp25=(_Tmp1F.f1)->type;_Tmp24=(_Tmp1F.f1)->inject;_Tmp23=(_Tmp1F.f2)->name;_Tmp22=(_Tmp1F.f2)->tq;_Tmp21=(_Tmp1F.f2)->type;_Tmp20=(_Tmp1F.f2)->inject;{struct _fat_ptr*n1=_Tmp27;struct Cyc_Absyn_Tqual tq1=_Tmp26;void*tp1=_Tmp25;int i1=_Tmp24;struct _fat_ptr*n2=_Tmp23;struct Cyc_Absyn_Tqual tq2=_Tmp22;void*tp2=_Tmp21;int i2=_Tmp20;
# 475
Cyc_Unify_unify_it(tp1,tp2);
Cyc_Unify_unify_tqual(tq1,tp1,tq2,tp2);
if(i1 != i2)
Cyc_Unify_fail_because(({const char*_Tmp28="(only one function type injects varargs)";_tag_fat(_Tmp28,sizeof(char),41U);}));}}}_LL3C:;}{
# 482
int bad_effect;
if(eff1 == 0 && eff2 == 0)
bad_effect=0;else{
if(eff1 == 0 || eff2 == 0)
bad_effect=1;else{
# 488
bad_effect=!Cyc_Unify_unify_effect(eff1,eff2);}}
Cyc_Unify_ts_failure=({struct _tuple11 _Tmp1F;_Tmp1F.f1=t1,_Tmp1F.f2=t2;_Tmp1F;});
if(bad_effect)
Cyc_Unify_fail_because(({struct Cyc_String_pa_PrintArg_struct _Tmp1F=({struct Cyc_String_pa_PrintArg_struct _Tmp20;_Tmp20.tag=0,({struct _fat_ptr _Tmp21=(unsigned)eff1?Cyc_Absynpp_typ2string(eff1):({const char*_Tmp22="-";_tag_fat(_Tmp22,sizeof(char),2U);});_Tmp20.f1=_Tmp21;});_Tmp20;});struct Cyc_String_pa_PrintArg_struct _Tmp20=({struct Cyc_String_pa_PrintArg_struct _Tmp21;_Tmp21.tag=0,({
struct _fat_ptr _Tmp22=(unsigned)eff2?Cyc_Absynpp_typ2string(eff2):({const char*_Tmp23="-";_tag_fat(_Tmp23,sizeof(char),2U);});_Tmp21.f1=_Tmp22;});_Tmp21;});void*_Tmp21[2];_Tmp21[0]=& _Tmp1F,_Tmp21[1]=& _Tmp20;Cyc_aprintf(({const char*_Tmp22="(function effects (%s,%s) do not match)";_tag_fat(_Tmp22,sizeof(char),40U);}),_tag_fat(_Tmp21,sizeof(void*),2));}));
if(!Cyc_Atts_equiv_fn_atts(atts2,atts1))
Cyc_Unify_fail_because(({const char*_Tmp1F="(function types have different attributes)";_tag_fat(_Tmp1F,sizeof(char),43U);}));
if(!Cyc_Tcutil_same_rgn_po(rpo2,rpo1))
Cyc_Unify_fail_because(({const char*_Tmp1F="(function types have different region lifetime orderings)";_tag_fat(_Tmp1F,sizeof(char),58U);}));
if(!Cyc_Unify_check_logical_equivalence(req_relns1,req_relns2))
Cyc_Unify_fail_because(({const char*_Tmp1F="(@requires clauses not equivalent)";_tag_fat(_Tmp1F,sizeof(char),35U);}));
if(!Cyc_Unify_check_logical_equivalence(ens_relns1,ens_relns2))
Cyc_Unify_fail_because(({const char*_Tmp1F="(@ensures clauses not equivalent)";_tag_fat(_Tmp1F,sizeof(char),34U);}));
return;}}}else{goto _LL25;}case 6: if(*((int*)_Tmp0.f2)== 6){_Tmp1E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_List_List*ts2=_Tmp1E;struct Cyc_List_List*ts1=_Tmp1D;
# 504
for(1;ts1 != 0 && ts2 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
struct _tuple13 _stmttmp7=*((struct _tuple13*)ts1->hd);struct _tuple13 _Tmp1F=_stmttmp7;void*_Tmp20;struct Cyc_Absyn_Tqual _Tmp21;_Tmp21=_Tmp1F.f1;_Tmp20=_Tmp1F.f2;{struct Cyc_Absyn_Tqual tqa=_Tmp21;void*ta=_Tmp20;
struct _tuple13 _stmttmp8=*((struct _tuple13*)ts2->hd);struct _tuple13 _Tmp22=_stmttmp8;void*_Tmp23;struct Cyc_Absyn_Tqual _Tmp24;_Tmp24=_Tmp22.f1;_Tmp23=_Tmp22.f2;{struct Cyc_Absyn_Tqual tqb=_Tmp24;void*tb=_Tmp23;
Cyc_Unify_unify_it(ta,tb);
Cyc_Unify_unify_tqual(tqa,ta,tqb,tb);}}}
# 510
if(ts1 == 0 && ts2 == 0)
return;
Cyc_Unify_ts_failure=({struct _tuple11 _Tmp1F;_Tmp1F.f1=t1,_Tmp1F.f2=t2;_Tmp1F;});
Cyc_Unify_fail_because(({const char*_Tmp1F="(tuple types have different numbers of components)";_tag_fat(_Tmp1F,sizeof(char),51U);}));}}else{goto _LL25;}case 7: if(*((int*)_Tmp0.f2)== 7){_Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f2)->f1;_Tmp1D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f2)->f2;{enum Cyc_Absyn_AggrKind k2=_Tmp2;struct Cyc_List_List*fs2=_Tmp1E;enum Cyc_Absyn_AggrKind k1=_Tmp1;struct Cyc_List_List*fs1=_Tmp1D;
# 516
if((int)k1 != (int)k2)
Cyc_Unify_fail_because(({const char*_Tmp1F="(struct and union type)";_tag_fat(_Tmp1F,sizeof(char),24U);}));
for(1;fs1 != 0 && fs2 != 0;(fs1=fs1->tl,fs2=fs2->tl)){
struct Cyc_Absyn_Aggrfield*f1=(struct Cyc_Absyn_Aggrfield*)fs1->hd;
struct Cyc_Absyn_Aggrfield*f2=(struct Cyc_Absyn_Aggrfield*)fs2->hd;
if(Cyc_strptrcmp(f1->name,f2->name)!= 0)
Cyc_Unify_fail_because(({const char*_Tmp1F="(different member names)";_tag_fat(_Tmp1F,sizeof(char),25U);}));
Cyc_Unify_unify_it(f1->type,f2->type);
Cyc_Unify_unify_tqual(f1->tq,f1->type,f2->tq,f2->type);
Cyc_Unify_ts_failure=({struct _tuple11 _Tmp1F;_Tmp1F.f1=t1,_Tmp1F.f2=t2;_Tmp1F;});
if(!Cyc_Atts_same_atts(f1->attributes,f2->attributes))
Cyc_Unify_fail_because(({const char*_Tmp1F="(different attributes on member)";_tag_fat(_Tmp1F,sizeof(char),33U);}));
if(!Cyc_Unify_unify_const_exp_opt(f1->width,f2->width))
Cyc_Unify_fail_because(({const char*_Tmp1F="(different bitfield widths on member)";_tag_fat(_Tmp1F,sizeof(char),38U);}));
if(!Cyc_Unify_unify_const_exp_opt(f1->requires_clause,f2->requires_clause))
Cyc_Unify_fail_because(({const char*_Tmp1F="(different @requires clauses on member)";_tag_fat(_Tmp1F,sizeof(char),40U);}));}
# 533
if(fs1 != 0 || fs2 != 0)
Cyc_Unify_fail_because(({const char*_Tmp1F="(different number of members)";_tag_fat(_Tmp1F,sizeof(char),30U);}));
return;}}else{goto _LL25;}case 8: if(*((int*)_Tmp0.f2)== 8){_Tmp1E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp1D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f1)->f3;_Tmp1C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f2)->f2;_Tmp1B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f2)->f3;{struct Cyc_List_List*ts1=_Tmp1E;struct Cyc_Absyn_Typedefdecl*td1=_Tmp1D;struct Cyc_List_List*ts2=_Tmp1C;struct Cyc_Absyn_Typedefdecl*td2=_Tmp1B;
# 538
if(td1 != td2)
Cyc_Unify_fail_because(({const char*_Tmp1F="(different abstract typedefs)";_tag_fat(_Tmp1F,sizeof(char),30U);}));
Cyc_Unify_failure_reason=({const char*_Tmp1F="(type parameters to typedef differ)";_tag_fat(_Tmp1F,sizeof(char),36U);});
Cyc_Unify_unify_list(ts1,ts2);
return;}}else{goto _LL25;}default: _LL25:
(int)_throw((void*)& Cyc_Unify_Unify_val);}}}}};}}
