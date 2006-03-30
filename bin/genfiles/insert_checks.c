#include <setjmp.h>
/* This is a C header used by the output of the Cyclone to
   C translator.  Corresponding definitions are in file lib/runtime_*.c */
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
/* should be size_t but int is fine */
#define offsetof(t,n) ((int)(&(((t*)0)->n)))
#endif

/* Fat pointers */
struct _fat_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Regions */
struct _RegionPage
{ 
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];
};

struct _pool;
struct bget_region_key;
struct _RegionAllocFunctions;

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  struct _RegionAllocFunctions *fcns;
  char               *offset;
  char               *last_plus_one;
  struct _pool *released_ptrs;
  struct bget_region_key *key;
#ifdef CYC_REGION_PROFILE
  const char *name;
#endif
  unsigned used_bytes;
  unsigned wasted_bytes;
};


// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

/* Alias qualifier stuff */
typedef unsigned int _AliasQualHandle_t; // must match aqualt_type() in toc.cyc

struct _RegionHandle _new_region(unsigned int, const char*);
void* _region_malloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned);
void* _region_calloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_region(struct _RegionHandle*);

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons*);
void _push_region(struct _RegionHandle*);
void _npop_handler(int);
void _pop_handler();
void _pop_region();


#ifndef _throw
void* _throw_null_fn(const char*,unsigned);
void* _throw_arraybounds_fn(const char*,unsigned);
void* _throw_badalloc_fn(const char*,unsigned);
void* _throw_match_fn(const char*,unsigned);
void* _throw_assert_fn(const char *,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw_assert() (_throw_assert_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
struct Cyc_Assert_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];
extern char Cyc_Assert[];

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
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_other_fn(t_sz,orig_x,orig_sz,orig_i,f,l)((orig_x)+(orig_i))
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
void* _zero_arr_plus_other_fn(unsigned,void*,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
unsigned _get_zero_arr_size_char(const char*,unsigned);
unsigned _get_zero_arr_size_other(unsigned,const void*,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
// note: must cast result in toc.cyc
void* _zero_arr_inplace_plus_other_fn(unsigned,void**,int,const char*,unsigned);
void* _zero_arr_inplace_plus_post_other_fn(unsigned,void**,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char**)(x),(i),__FILE__,__LINE__)
#define _zero_arr_plus_other(t,x,s,i) \
  (_zero_arr_plus_other_fn(t,x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_other(t,x,i) \
  _zero_arr_inplace_plus_other_fn(t,(void**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_other(t,x,i) \
  _zero_arr_inplace_plus_post_other_fn(t,(void**)(x),(i),__FILE__,__LINE__)

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ((arr).curr)
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ({ \
  struct _fat_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char*)0) \
    _throw_arraybounds(); \
  _curr; })
#define _check_fat_at_base(arr) ({ \
  struct _fat_ptr _arr = (arr); \
  if (_arr.base != _arr.curr) _throw_arraybounds(); \
  _arr; })
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
void cyc_vfree(void*);
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

#define _CYC_MAX_REGION_CONST 0
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, _AliasQualHandle_t aq, unsigned orig_s) {  
  if (r > (struct _RegionHandle*)_CYC_MAX_REGION_CONST && r->curr != 0) { 
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
  return _region_malloc(r,aq,orig_s); 
}

//doesn't make sense to fast malloc with reaps
#ifndef DISABLE_REAPS
#define _fast_region_malloc _region_malloc
#endif

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,unsigned,const char *,const char*,int);
void * _profile_aqual_malloc(_AliasQualHandle_t aq, unsigned int s,const char *file, const char *func, int lineno);
void * _profile_aqual_calloc(_AliasQualHandle_t aq, unsigned int t1,unsigned int t2,const char *file, const char *func, int lineno);
struct _RegionHandle _profile_new_region(unsigned int i, const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(i,n) _profile_new_region(i,n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,aq,n) _profile_region_malloc(rh,aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,aq,n,t) _profile_region_calloc(rh,aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_malloc(aq,n) _profile_aqual_malloc(aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_calloc(aq,n,t) _profile_aqual_calloc(aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif //CYC_REGION_PROFILE
#endif //_CYC_INCLUDE_H
 struct Cyc_Core_Opt{void*v;};
# 128 "core.h"
int Cyc_Core_ptrcmp(void*,void*);
# 173
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 172 "list.h"
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);struct Cyc_Set_Set;struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 168
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
# 529 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 536
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 554
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 568
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;int f3;};struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 923 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_hash_qvar(struct _tuple0*);
# 944
void*Cyc_Absyn_compress(void*);
# 1014
void*Cyc_Absyn_bounds_one (void);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1243
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
extern union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};
# 76
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*,union Cyc_Relations_RelnOp*);
# 87
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*,union Cyc_Relations_RelnOp,enum Cyc_Relations_Relation,union Cyc_Relations_RelnOp,struct Cyc_List_List*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*);extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 48 "tcutil.h"
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp_constrain(void*,void*,int);
# 74
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 227 "tcutil.h"
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 240
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*);
# 254
int Cyc_Tcutil_force_type2bool(int,void*);struct _tuple12{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*);
# 42 "evexp.h"
extern int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 46
void*Cyc_Warn_impos_loc(unsigned,struct _fat_ptr,struct _fat_ptr);
# 26 "flags.h"
extern int Cyc_Flags_verbose;
# 70
extern int Cyc_Flags_warn_bounds_checks;
# 74
extern int Cyc_Flags_warn_all_null_deref;struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 35 "insert_checks.h"
extern struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val;
extern struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullOnly_val;
extern struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val;
extern struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_FatBound_val;
# 232 "assndef.h"
extern struct _fat_ptr Cyc_AssnDef_assn2string(void*);struct Cyc_AssnDef_AssnFn{struct Cyc_List_List*actuals;void*assn;};struct Cyc_AssnDef_ExistAssnFn{struct Cyc_AssnDef_AssnFn*af;struct Cyc_Set_Set*existvars;};struct Cyc_AssnDef_AssnMap{void*assn;struct Cyc_Dict_Dict map;};
# 290
extern struct _fat_ptr Cyc_AssnDef_assnmap2string(struct Cyc_AssnDef_AssnMap);
# 313
extern int Cyc_AssnDef_simple_prove(void*,void*);
# 374 "assndef.h"
void Cyc_AssnDef_reset_hash_cons_table (void);struct Cyc_Vcgen_ExpChecks{struct Cyc_AssnDef_AssnMap ctxt;void*bounds_check;void*null_check;};
# 38 "vcgen.h"
extern void Cyc_Vcgen_vcgen_fundecl(unsigned,struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_Absyn_Fndecl*,struct Cyc_Hashtable_Table**,struct Cyc_Hashtable_Table*,int);
# 44
struct Cyc_Vcgen_ExpChecks*Cyc_Vcgen_exp2ctxt_checks(struct Cyc_Hashtable_Table**,struct Cyc_Absyn_Exp*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 85 "graph.h"
extern struct Cyc_List_List*Cyc_Graph_tsort(struct Cyc_Dict_Dict);
# 30 "callgraph.h"
struct Cyc_Dict_Dict Cyc_Callgraph_compute_callgraph(struct Cyc_List_List*);char Cyc_InsertChecks_FatBound[9U]="FatBound";char Cyc_InsertChecks_NoCheck[8U]="NoCheck";char Cyc_InsertChecks_NullAndFatBound[16U]="NullAndFatBound";char Cyc_InsertChecks_NullAndThinBound[17U]="NullAndThinBound";char Cyc_InsertChecks_NullOnly[9U]="NullOnly";char Cyc_InsertChecks_ThinBound[10U]="ThinBound";
# 64 "insert_checks.cyc"
struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NoCheck_val={Cyc_InsertChecks_NoCheck};
struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullOnly_val={Cyc_InsertChecks_NullOnly};
struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_NullAndFatBound_val={Cyc_InsertChecks_NullAndFatBound};
struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct Cyc_InsertChecks_FatBound_val={Cyc_InsertChecks_FatBound};
# 71
static unsigned Cyc_InsertChecks_total_bounds_checks=0U;
static unsigned Cyc_InsertChecks_types_eliminated_bounds_checks=0U;
static unsigned Cyc_InsertChecks_simple_eliminated_bounds_checks=0U;
static unsigned Cyc_InsertChecks_constraints_eliminated_bounds_checks=0U;
# 76
static unsigned Cyc_InsertChecks_total_null_checks=0U;
static unsigned Cyc_InsertChecks_types_eliminated_null_checks=0U;
static unsigned Cyc_InsertChecks_flow_eliminated_null_checks=0U;
static unsigned Cyc_InsertChecks_simple_eliminated_null_checks=0U;
static unsigned Cyc_InsertChecks_constraints_eliminated_null_checks=0U;
# 82
static void Cyc_InsertChecks_reset_counters (void){
Cyc_InsertChecks_total_bounds_checks=0U;
Cyc_InsertChecks_types_eliminated_bounds_checks=0U;
Cyc_InsertChecks_simple_eliminated_bounds_checks=0U;
Cyc_InsertChecks_constraints_eliminated_bounds_checks=0U;
# 88
Cyc_InsertChecks_total_null_checks=0U;
Cyc_InsertChecks_types_eliminated_null_checks=0U;
Cyc_InsertChecks_flow_eliminated_null_checks=0U;
Cyc_InsertChecks_simple_eliminated_null_checks=0U;
Cyc_InsertChecks_constraints_eliminated_null_checks=0U;}
# 95
static struct Cyc_Absyn_Exp*Cyc_InsertChecks_shared_zero_exp (void){
static struct Cyc_Absyn_Exp*ans=0;
if(!((unsigned)ans))
ans=Cyc_Absyn_uint_exp(0U,0U);
return ans;}struct Cyc_InsertChecks_Env{struct Cyc_JumpAnalysis_Jump_Anal_Result*tables;struct Cyc_Hashtable_Table**assn_info;struct Cyc_Hashtable_Table*fn_precond_info;unsigned pass_counter;int use_precond: 1;int in_sizeof: 1;int at_toplevel: 1;int vcgen_r: 1;};
# 115
static int Cyc_InsertChecks_hash_ptr(void*s){return(int)s;}
# 117
static struct Cyc_InsertChecks_Env Cyc_InsertChecks_toplevel_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,int do_vcgen){
struct Cyc_Hashtable_Table*fn_precond_info=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*)))Cyc_Hashtable_create;_Tmp0;})(313,Cyc_Absyn_qvar_cmp,Cyc_Absyn_hash_qvar);
struct Cyc_InsertChecks_Env _Tmp0;_Tmp0.tables=tables,_Tmp0.assn_info=0,_Tmp0.fn_precond_info=fn_precond_info,_Tmp0.pass_counter=1U,_Tmp0.use_precond=0,_Tmp0.in_sizeof=0,_Tmp0.at_toplevel=1,_Tmp0.vcgen_r=do_vcgen;return _Tmp0;}
# 128
static struct Cyc_InsertChecks_Env Cyc_InsertChecks_fn_body_env(struct Cyc_InsertChecks_Env nv){
struct Cyc_InsertChecks_Env _Tmp0;_Tmp0.tables=nv.tables,_Tmp0.assn_info=nv.assn_info,_Tmp0.fn_precond_info=nv.fn_precond_info,_Tmp0.pass_counter=nv.pass_counter,_Tmp0.use_precond=nv.use_precond,_Tmp0.in_sizeof=0,_Tmp0.at_toplevel=0,_Tmp0.vcgen_r=nv.vcgen_r;return _Tmp0;}
# 132
static struct Cyc_InsertChecks_Env Cyc_InsertChecks_enter_sizeof(struct Cyc_InsertChecks_Env nv){
struct Cyc_InsertChecks_Env _Tmp0;_Tmp0.tables=nv.tables,_Tmp0.assn_info=nv.assn_info,_Tmp0.fn_precond_info=nv.fn_precond_info,_Tmp0.pass_counter=nv.pass_counter,_Tmp0.use_precond=nv.use_precond,_Tmp0.in_sizeof=1,_Tmp0.at_toplevel=nv.at_toplevel,_Tmp0.vcgen_r=nv.vcgen_r;return _Tmp0;}struct _tuple13{int f0;int f1;};
# 137
static struct _tuple13 Cyc_InsertChecks_need_bound_null_check_annot(struct Cyc_Absyn_Exp*e){
int bds_ck;int null_ck;
{void*_Tmp0=e->annot;if(((struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_NoCheck){
# 141
bds_ck=0;
null_ck=0;
goto _LL0;}else{if(((struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_FatBound){
# 145
bds_ck=1;
null_ck=0;
goto _LL0;}else{if(((struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_ThinBound){
# 149
bds_ck=1;
null_ck=0;
goto _LL0;}else{if(((struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_InsertChecks_NullOnly){
# 153
bds_ck=0;
null_ck=1;
goto _LL0;}else{
# 157
bds_ck=1;
null_ck=1;}}}}_LL0:;}{
# 160
struct _tuple13 _Tmp0;_Tmp0.f0=bds_ck,_Tmp0.f1=null_ck;return _Tmp0;}}
# 165
static int Cyc_InsertChecks_need_null_check(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e0){
# 169
if(nv.pass_counter > 1U){
struct _tuple13 _Tmp0=Cyc_InsertChecks_need_bound_null_check_annot(e0);int _Tmp1;_Tmp1=_Tmp0.f1;{int null_ck=_Tmp1;
return null_ck;}}
# 173
if(nv.in_sizeof)
return 0;
# 176
++ Cyc_InsertChecks_total_null_checks;
{void*_Tmp0=Cyc_Absyn_compress(_check_null(e->topt));void*_Tmp1;if(*((int*)_Tmp0)==4){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;{void*n=_Tmp1;
# 179
if(!Cyc_Tcutil_force_type2bool(0,n)){
++ Cyc_InsertChecks_types_eliminated_null_checks;
return 0;}
# 183
goto _LL3;}}else{
({int(*_Tmp2)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_Tmp2;})(e->loc,_tag_fat("need_null_check: non-pointer type",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));}_LL3:;}
# 186
{void*_Tmp0=e->annot;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_UnknownZ)
goto _LL8;else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_NotZero){
# 189
++ Cyc_InsertChecks_flow_eliminated_null_checks;
return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_IsZero){
# 192
Cyc_Warn_err(e->loc,_tag_fat("NULL pointer check will definitely fail",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_Absyn_EmptyAnnot){
# 196
++ Cyc_InsertChecks_flow_eliminated_null_checks;
return 0;}else{
({int(*_Tmp1)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_Tmp1;})(e->loc,_tag_fat("need_null_check: unexpected annotation",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));}}}}_LL8:;}
# 200
if(nv.at_toplevel){
Cyc_Warn_err(e->loc,_tag_fat("cannot perform NULL-pointer check at toplevel",sizeof(char),46U),_tag_fat(0U,sizeof(void*),0));
return 0;}
# 206
return 1;}
# 209
static struct Cyc_List_List*Cyc_InsertChecks_get_relns(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->annot;void*_Tmp1;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_UnknownZ){_Tmp1=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_Tmp0)->f1;{struct Cyc_List_List*r=_Tmp1;
return r;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_NotZero){_Tmp1=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_Tmp0)->f1;{struct Cyc_List_List*r=_Tmp1;
return r;}}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_CfFlowInfo_IsZero)
return 0;else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_Tmp0)->tag==Cyc_Absyn_EmptyAnnot)
return 0;else{
({int(*_Tmp2)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_Tmp2;})(e->loc,_tag_fat("get_relns: unexpected annotation",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));}}}};}
# 220
static int Cyc_InsertChecks_simple_need_bounds_check(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
if(nv.in_sizeof)return 0;
++ Cyc_InsertChecks_total_bounds_checks;
if(nv.at_toplevel)
({int(*_Tmp0)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_Tmp0;})(a->loc,_tag_fat("InsertChecks: memory read at top-level",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));{
# 230
struct Cyc_Absyn_Exp*bound_opt=Cyc_Tcutil_get_type_bound(_check_null(a->topt));
# 232
if(bound_opt==0)return 1;{
struct Cyc_Absyn_Exp*bound=bound_opt;
# 235
if(!Cyc_Tcutil_is_const_exp(bound)|| !Cyc_Tcutil_is_const_exp(i))return 1;{
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_uint_exp(bound);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{unsigned b=_Tmp2;int b_known=_Tmp1;
struct _tuple12 _Tmp3=Cyc_Evexp_eval_const_uint_exp(i);int _Tmp4;unsigned _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{unsigned i=_Tmp5;int i_known=_Tmp4;
# 239
if((!b_known || !i_known)|| i >= b)return 1;
# 241
++ Cyc_InsertChecks_types_eliminated_bounds_checks;
return 0;}}}}}}
# 249
static int Cyc_InsertChecks_need_bounds_check(struct Cyc_InsertChecks_Env nv,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
if(nv.in_sizeof)
return 0;
if(nv.at_toplevel)
({int(*_Tmp0)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_Tmp0;})(a->loc,_tag_fat("InsertChecks: memory read at top-level",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));{
# 260
int bd_valid=0;union Cyc_Relations_RelnOp rop_bd=Cyc_Relations_RConst(0U);
int a_valid=0;union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RConst(0U);
int i_valid=0;union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int z_valid=0;union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
# 266
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_check_null(a->topt));
if(bound!=0)
bd_valid=Cyc_Relations_exp2relnop(bound,& rop_bd);
# 270
{void*_Tmp0=a->r;void*_Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp1;
# 272
struct Cyc_Absyn_Vardecl*x=Cyc_Tcutil_nonesc_vardecl(b);
if(x!=0){
a_valid=1;
rop_a=Cyc_Relations_RNumelts(x);}
# 277
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 281
i_valid=Cyc_Relations_exp2relnop(i,& rop_i);
# 283
if(!i_valid){
inner_loop: {
void*_Tmp0=i->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 14: _Tmp2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
i=e2;goto inner_loop;}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1==Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl!=0){_Tmp2=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;_Tmp1=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->tl->hd;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 288
z_valid=Cyc_Relations_exp2relnop(e2,& rop_z);goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA:
 goto _LL5;}_LL5:;}}{
# 294
union Cyc_Relations_RelnOp rop_left;
# 298
if(!bd_valid && !a_valid)
return 1;
if(bd_valid && a_valid){
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bd,2U,rop_a,relns);
rop_left=rop_a;}else{
if(a_valid)
rop_left=rop_a;else{
# 306
rop_left=rop_bd;}}
# 308
if(i_valid)relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_left,2U,rop_i,relns);else{
if(z_valid)relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_left,3U,rop_z,relns);else{
return 1;}}{
# 315
int ans=Cyc_Relations_consistent_relations(relns);
# 321
return ans;}}}}
# 324
static void Cyc_InsertChecks_do_stmt(struct Cyc_InsertChecks_Env,struct Cyc_Absyn_Stmt*);
static void Cyc_InsertChecks_do_exp(struct Cyc_InsertChecks_Env,struct Cyc_Absyn_Exp*);
# 327
static void Cyc_InsertChecks_do_exp_opt(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*eo){
if(eo!=0)
Cyc_InsertChecks_do_exp(nv,eo);}
# 332
static int Cyc_InsertChecks_need_null_check_vcgen(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*a){
struct Cyc_Vcgen_ExpChecks*ec=Cyc_Vcgen_exp2ctxt_checks(nv.assn_info,a);
int res=Cyc_AssnDef_simple_prove(ec->ctxt.assn,ec->null_check);
if(res)++ Cyc_InsertChecks_simple_eliminated_null_checks;else{
# 337
res=Cyc_PrattProver_constraint_prove(ec->ctxt.assn,ec->null_check);
if(res)++ Cyc_InsertChecks_constraints_eliminated_null_checks;}
# 340
return !res;}
# 342
static int Cyc_InsertChecks_need_bounds_check_vcgen(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*a){
struct Cyc_Vcgen_ExpChecks*ec=Cyc_Vcgen_exp2ctxt_checks(nv.assn_info,a);
int res=Cyc_AssnDef_simple_prove(ec->ctxt.assn,ec->bounds_check);
if(res)++ Cyc_InsertChecks_simple_eliminated_bounds_checks;else{
# 347
res=Cyc_PrattProver_constraint_prove(ec->ctxt.assn,ec->bounds_check);
if(res)++ Cyc_InsertChecks_constraints_eliminated_bounds_checks;}
# 350
return !res;}struct _tuple14{void*f0;void*f1;};struct _tuple15{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple16{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 353
static void Cyc_InsertChecks_do_exp(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;enum Cyc_Absyn_Coercion _Tmp1;int _Tmp2;enum Cyc_Absyn_Incrementor _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 39:
 goto _LL4;case 0: _LL4:
 goto _LL6;case 1: _LL6:
 goto _LL8;case 2: _LL8:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL0;case 30: _Tmp6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp6;
# 360
_Tmp6=es;goto _LLC;}case 3: _Tmp6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLC: {struct Cyc_List_List*es=_Tmp6;
# 362
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
for(1;es!=0;es=es->tl){
Cyc_InsertChecks_do_exp(nv,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 4: _Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 369
_Tmp6=e1;_Tmp5=e2;goto _LL10;}case 9: _Tmp6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
_Tmp6=e1;_Tmp5=e2;goto _LL12;}case 7: _Tmp6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
_Tmp6=e1;_Tmp5=e2;goto _LL14;}case 8: _Tmp6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 373
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,e1);
Cyc_InsertChecks_do_exp(nv,e2);
goto _LL0;}case 6: _Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Exp*e3=_Tmp4;
# 378
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,e1);
Cyc_InsertChecks_do_exp(nv,e2);
Cyc_InsertChecks_do_exp(nv,e3);
goto _LL0;}case 5: _Tmp6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e2=_Tmp6;enum Cyc_Absyn_Incrementor incr=_Tmp3;
# 389
if((((e->annot!=(void*)& Cyc_InsertChecks_NoCheck_val &&
 Cyc_Tcutil_get_type_bound(_check_null(e2->topt))!=0)&&
 Cyc_Tcutil_is_zeroterm_pointer_type(_check_null(e2->topt)))&&
 Cyc_InsertChecks_need_null_check(nv,e2,e))&& Cyc_InsertChecks_need_null_check_vcgen(nv,e2)){
if(Cyc_Flags_warn_all_null_deref)
Cyc_Warn_warn(e->loc,_tag_fat("inserted null check",sizeof(char),20U),_tag_fat(0U,sizeof(void*),0));
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;}else{
# 397
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
Cyc_InsertChecks_do_exp(nv,e2);
goto _LL0;}case 10: _Tmp6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_List_List*es=_Tmp5;
# 403
if(Cyc_InsertChecks_need_null_check(nv,e1,e)&& Cyc_InsertChecks_need_null_check_vcgen(nv,e1))
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;else{
# 406
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
Cyc_InsertChecks_do_exp(nv,e1);
for(1;es!=0;es=es->tl){
Cyc_InsertChecks_do_exp(nv,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 14: _Tmp6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*t=_Tmp6;struct Cyc_Absyn_Exp*e1=_Tmp5;int user_inserted=_Tmp2;enum Cyc_Absyn_Coercion coercion=_Tmp1;
# 415
{struct _tuple14 _Tmp7=({struct _tuple14 _Tmp8;({void*_Tmp9=Cyc_Absyn_compress(_check_null(e1->topt));_Tmp8.f0=_Tmp9;}),({void*_Tmp9=Cyc_Absyn_compress(t);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_Absyn_PtrInfo _Tmp8;struct Cyc_Absyn_PtrInfo _Tmp9;if(*((int*)_Tmp7.f0)==4){if(*((int*)_Tmp7.f1)==4){_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7.f0)->f1;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7.f1)->f1;{struct Cyc_Absyn_PtrInfo p1=_Tmp9;struct Cyc_Absyn_PtrInfo p2=_Tmp8;
# 417
int n1=Cyc_Tcutil_force_type2bool(0,p1.ptr_atts.nullable);
int n2=Cyc_Tcutil_force_type2bool(0,p2.ptr_atts.nullable);
struct Cyc_Absyn_Exp*b1=({void*_TmpA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp_constrain(_TmpA,p1.ptr_atts.bounds,1);});
struct Cyc_Absyn_Exp*b2=({void*_TmpA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp_constrain(_TmpA,p2.ptr_atts.bounds,1);});
# 422
if(((n1 && !n2)&&(int)coercion!=2)&&((unsigned)b1 ||(unsigned)b2))
({int(*_TmpA)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_TmpA;})(e->loc,_tag_fat("null-check conversion mis-classified",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));else{
if(((unsigned)b1 && !((unsigned)b2))&&(int)coercion==2)
({int(*_TmpA)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_TmpA;})(e->loc,_tag_fat("conversion mis-classified as null-check",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));}
{struct _tuple15 _TmpA=({struct _tuple15 _TmpB;_TmpB.f0=b1,_TmpB.f1=b2;_TmpB;});if(_TmpA.f0!=0){if(_TmpA.f1!=0){
# 428
if(!Cyc_Evexp_lte_const_exp(b2,b1))
({int(*_TmpB)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_TmpB;})(e->loc,_tag_fat("InsertChecks: cast to maybe-shorter pointer type",sizeof(char),49U),_tag_fat(0U,sizeof(void*),0));
# 431
if(((n1 && !n2)&& Cyc_InsertChecks_need_null_check(nv,e1,e))&& Cyc_InsertChecks_need_null_check_vcgen(nv,e1)){
if(!user_inserted)
Cyc_Warn_warn(e->loc,
_tag_fat("inserted null check due to implicit cast from * to @ type",sizeof(char),58U),_tag_fat(0U,sizeof(void*),0));
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;}else{
# 437
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}
goto _LL5C;}else{
# 441
if(!Cyc_Evexp_c_can_eval(_check_null(b1)))
Cyc_Warn_err(e->loc,_tag_fat("cannot perform coercion: numelts is statically unknown",sizeof(char),55U),_tag_fat(0U,sizeof(void*),0));
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
goto _LL5C;}}else{if(_TmpA.f1!=0){
# 446
if(nv.at_toplevel)
Cyc_Warn_err(e->loc,_tag_fat("cannot coerce fat-pointer to thin-pointer at toplevel",sizeof(char),54U),_tag_fat(0U,sizeof(void*),0));{
# 451
struct _tuple13 _TmpB=Cyc_InsertChecks_need_bound_null_check_annot(e);int _TmpC;int _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{int bds_ck=_TmpD;int null_ck=_TmpC;
if(bds_ck)
bds_ck=Cyc_InsertChecks_need_bounds_check_vcgen(nv,e1);
# 457
if(bds_ck){
if(!Cyc_Evexp_c_can_eval(_check_null(b2)))
Cyc_Warn_err(e->loc,_tag_fat("cannot perform coercion: numelts is statically unknown",sizeof(char),55U),_tag_fat(0U,sizeof(void*),0));}
# 462
if(((null_ck && !n2)&& Cyc_InsertChecks_need_null_check(nv,e1,e))&& Cyc_InsertChecks_need_null_check_vcgen(nv,e)){
if(!user_inserted)
Cyc_Warn_warn(e->loc,
_tag_fat("inserted null check due to implicit cast from *@fat (?) to @ type",sizeof(char),66U),_tag_fat(0U,sizeof(void*),0));
e->annot=bds_ck?(void*)& Cyc_InsertChecks_NullAndFatBound_val:(void*)& Cyc_InsertChecks_NullOnly_val;}else{
# 468
e->annot=bds_ck?(void*)& Cyc_InsertChecks_FatBound_val:(void*)& Cyc_InsertChecks_NoCheck_val;}
goto _LL5C;}}}else{
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL5C;}}_LL5C:;}
# 472
goto _LL57;}}else{goto _LL5A;}}else{_LL5A:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL57;}_LL57:;}
# 475
Cyc_InsertChecks_do_exp(nv,e1);
goto _LL0;}case 22: _Tmp6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 478
_Tmp6=e1;goto _LL20;}case 20: _Tmp6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;_Tmp5=Cyc_InsertChecks_shared_zero_exp();goto _LL22;}case 23: _Tmp6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL22: {struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 481
{void*_Tmp7=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;struct Cyc_Absyn_Tqual _TmpC;void*_TmpD;if(*((int*)_Tmp7)==4){_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.elt_type;_TmpC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.elt_tq;_TmpB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.ptr_atts.eff;_TmpA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.ptr_atts.nullable;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.ptr_atts.bounds;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp7)->f1.ptr_atts.zero_term;{void*ta=_TmpD;struct Cyc_Absyn_Tqual tq=_TmpC;void*rgn=_TmpB;void*n=_TmpA;void*b=_Tmp9;void*zt=_Tmp8;
# 483
struct _tuple13 _TmpE=Cyc_InsertChecks_need_bound_null_check_annot(e);int _TmpF;int _Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{int bds_ck=_Tmp10;int null_ck=_TmpF;
# 486
if(bds_ck)
bds_ck=Cyc_InsertChecks_simple_need_bounds_check(nv,e1,e2);
if(bds_ck && nv.vcgen_r)
# 490
bds_ck=Cyc_InsertChecks_need_bounds_check_vcgen(nv,e1);
if(bds_ck && Cyc_Flags_warn_bounds_checks)
({struct Cyc_String_pa_PrintArg_struct _Tmp11=({struct Cyc_String_pa_PrintArg_struct _Tmp12;_Tmp12.tag=0,({struct _fat_ptr _Tmp13=Cyc_Absynpp_exp2string(e);_Tmp12.f1=_Tmp13;});_Tmp12;});void*_Tmp12[1];_Tmp12[0]=& _Tmp11;Cyc_Warn_warn(e1->loc,_tag_fat("inserted bounds check at %s",sizeof(char),28U),_tag_fat(_Tmp12,sizeof(void*),1));});{
struct Cyc_Absyn_Exp*bd=({void*_Tmp11=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp_constrain(_Tmp11,b,1);});
int fat_ptr=bd==0;
# 498
if(null_ck){
if(fat_ptr && !bds_ck)
null_ck=0;else{
# 502
null_ck=Cyc_InsertChecks_need_null_check(nv,e1,e)&& Cyc_InsertChecks_need_null_check_vcgen(nv,e1);
if(null_ck && Cyc_Flags_warn_all_null_deref)
Cyc_Warn_warn(e->loc,_tag_fat("inserted null check",sizeof(char),20U),_tag_fat(0U,sizeof(void*),0));}}
# 507
if(bds_ck){
if((unsigned)bd){
if(!Cyc_Evexp_c_can_eval(bd)){
# 513
struct Cyc_Vcgen_ExpChecks*ec=Cyc_Vcgen_exp2ctxt_checks(nv.assn_info,e1);
({unsigned _Tmp11=e->loc;struct _fat_ptr _Tmp12=({struct Cyc_String_pa_PrintArg_struct _Tmp13=({struct Cyc_String_pa_PrintArg_struct _Tmp14;_Tmp14.tag=0,({struct _fat_ptr _Tmp15=Cyc_AssnDef_assn2string(ec->bounds_check);_Tmp14.f1=_Tmp15;});_Tmp14;});void*_Tmp14[1];_Tmp14[0]=& _Tmp13;Cyc_aprintf(_tag_fat("cannot determine subscript is in bounds (%s)",sizeof(char),45U),_tag_fat(_Tmp14,sizeof(void*),1));});Cyc_Warn_err(_Tmp11,_Tmp12,_tag_fat(0U,sizeof(void*),0));});
({struct Cyc_String_pa_PrintArg_struct _Tmp11=({struct Cyc_String_pa_PrintArg_struct _Tmp12;_Tmp12.tag=0,({
struct _fat_ptr _Tmp13=Cyc_AssnDef_assnmap2string(ec->ctxt);_Tmp12.f1=_Tmp13;});_Tmp12;});void*_Tmp12[1];_Tmp12[0]=& _Tmp11;Cyc_fprintf(Cyc_stderr,_tag_fat("[all that I can prove is %s",sizeof(char),28U),_tag_fat(_Tmp12,sizeof(void*),1));});
Cyc_fprintf(Cyc_stderr,_tag_fat("]\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));}
# 519
({void*_Tmp11=
null_ck?(void*)({struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct*_Tmp12=_cycalloc(sizeof(struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct));_Tmp12->tag=Cyc_InsertChecks_NullAndThinBound,_Tmp12->f1=bd;_Tmp12;}):(void*)({struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct*_Tmp12=_cycalloc(sizeof(struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct));_Tmp12->tag=Cyc_InsertChecks_ThinBound,_Tmp12->f1=bd;_Tmp12;});
# 519
e->annot=_Tmp11;});}else{
# 522
e->annot=null_ck?(void*)& Cyc_InsertChecks_NullAndFatBound_val:(void*)& Cyc_InsertChecks_FatBound_val;}}else{
if(null_ck)
e->annot=(void*)& Cyc_InsertChecks_NullOnly_val;else{
# 526
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;}}
goto _LL68;}}}}else{
({int(*_TmpE)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_TmpE;})(e->loc,_tag_fat("InsertChecks: non-pointer-type on dereference",sizeof(char),46U),_tag_fat(0U,sizeof(void*),0));}_LL68:;}
# 530
Cyc_InsertChecks_do_exp(nv,e1);
Cyc_InsertChecks_do_exp(nv,e2);
goto _LL0;}case 18: _Tmp6=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 535
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
({struct Cyc_InsertChecks_Env _Tmp7=Cyc_InsertChecks_enter_sizeof(nv);Cyc_InsertChecks_do_exp(_Tmp7,e1);});
goto _LL0;}case 16: _Tmp6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*rgnopt=_Tmp6;struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*qualopt=_Tmp4;
# 540
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp_opt(nv,rgnopt);
Cyc_InsertChecks_do_exp_opt(nv,qualopt);
Cyc_InsertChecks_do_exp(nv,e1);
goto _LL0;}case 33: _Tmp6=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp5=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp4=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;{struct Cyc_Absyn_Exp*rgnopt=_Tmp6;struct Cyc_Absyn_Exp*aqopt=_Tmp5;struct Cyc_Absyn_Exp*sizeexp=_Tmp4;
# 547
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp_opt(nv,rgnopt);
Cyc_InsertChecks_do_exp_opt(nv,aqopt);
Cyc_InsertChecks_do_exp(nv,sizeexp);
goto _LL0;}case 41: _Tmp6=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 553
_Tmp6=e1;goto _LL2C;}case 40: _Tmp6=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL2E;}case 11: _Tmp6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL30;}case 12: _Tmp6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL30: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL32;}case 13: _Tmp6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL32: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL34;}case 15: _Tmp6=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL34: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL36;}case 42: _Tmp6=((struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_Tmp6;
# 560
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,e1);
goto _LL0;}case 31:
 goto _LL3A;case 32: _LL3A:
 goto _LL3C;case 17: _LL3C:
 goto _LL3E;case 19: _LL3E:
 e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;goto _LL0;case 21: _Tmp6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL42;}case 37: _Tmp6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL42: {struct Cyc_Absyn_Exp*e1=_Tmp6;
_Tmp6=e1;goto _LL44;}case 27: _Tmp6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL44: {struct Cyc_Absyn_Exp*e1=_Tmp6;
# 570
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,e1);
goto _LL0;}case 26: _Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
_Tmp6=e1;_Tmp5=e2;goto _LL48;}case 34: _Tmp6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL48: {struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 575
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
Cyc_InsertChecks_do_exp(nv,e1);
Cyc_InsertChecks_do_exp(nv,e2);
goto _LL0;}case 25: _Tmp6=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*dles=_Tmp6;
_Tmp6=dles;goto _LL4C;}case 28: _Tmp6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL4C: {struct Cyc_List_List*dles=_Tmp6;
_Tmp6=dles;goto _LL4E;}case 29: _Tmp6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL4E: {struct Cyc_List_List*dles=_Tmp6;
# 582
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;
for(1;dles!=0;dles=dles->tl){
Cyc_InsertChecks_do_exp(nv,(*((struct _tuple16*)dles->hd)).f1);}
goto _LL0;}case 36: _Tmp6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp6;
e->annot=(void*)& Cyc_InsertChecks_NoCheck_val;Cyc_InsertChecks_do_stmt(nv,s);goto _LL0;}case 35:
# 588
 goto _LL54;case 24: _LL54:
 goto _LL56;default: _LL56:
({int(*_Tmp7)(unsigned,struct _fat_ptr,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos_loc;_Tmp7;})(e->loc,_tag_fat("InsertChecks, unexpected exp form",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}
# 594
static void Cyc_InsertChecks_do_fndecl(struct Cyc_InsertChecks_Env env,struct Cyc_Absyn_Fndecl*fd){
# 597
if(fd->escapes && env.use_precond)return;else{
# 599
struct Cyc_InsertChecks_Env nv=Cyc_InsertChecks_fn_body_env(env);
({struct Cyc_Hashtable_Table**_Tmp0=({struct Cyc_Hashtable_Table**_Tmp1=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));({struct Cyc_Hashtable_Table*_Tmp2=({struct Cyc_Hashtable_Table*(*_Tmp3)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create;_Tmp3;})(57,({int(*_Tmp3)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp;_Tmp3;}),({int(*_Tmp3)(struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*))Cyc_InsertChecks_hash_ptr;_Tmp3;}));*_Tmp1=_Tmp2;});_Tmp1;});nv.assn_info=_Tmp0;});
if(nv.vcgen_r){
# 608
if(env.use_precond && env.pass_counter <= 1U)
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("Trying to use preconditions in vcgen at the first pass\n",sizeof(char),56U),_tag_fat(0U,sizeof(void*),0));
Cyc_Vcgen_vcgen_fundecl(0U,nv.tables,fd,nv.assn_info,nv.fn_precond_info,nv.use_precond);}
# 627 "insert_checks.cyc"
Cyc_InsertChecks_do_stmt(nv,fd->body);
Cyc_AssnDef_reset_hash_cons_table();}}
# 642 "insert_checks.cyc"
static void Cyc_InsertChecks_do_decl(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
Cyc_InsertChecks_do_exp_opt(nv,vd->initializer);goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 646
goto _LL0;}case 2: _Tmp1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp1;
Cyc_InsertChecks_do_exp(nv,e);goto _LL0;}case 3:
 goto _LL0;case 4: _Tmp1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*eo=_Tmp1;
Cyc_InsertChecks_do_exp_opt(nv,eo);goto _LL0;}case 5:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 8: _LL12:
 goto _LL14;case 13: _LL14:
 goto _LL16;case 14: _LL16:
 goto _LL18;case 15: _LL18:
 goto _LL1A;case 16: _LL1A:
 goto _LL1C;case 12: _LL1C:
 goto _LL0;case 11: _Tmp1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds=_Tmp1;
_Tmp1=ds;goto _LL20;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL20: {struct Cyc_List_List*ds=_Tmp1;
_Tmp1=ds;goto _LL22;}default: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL22: {struct Cyc_List_List*ds=_Tmp1;
# 662
for(1;ds!=0;ds=ds->tl){
Cyc_InsertChecks_do_decl(nv,(struct Cyc_Absyn_Decl*)ds->hd);}}}_LL0:;}
# 667
static void Cyc_InsertChecks_do_scs(struct Cyc_InsertChecks_Env nv,struct Cyc_List_List*scs){
# 670
for(1;scs!=0;scs=scs->tl){
if(((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause!=0)
Cyc_InsertChecks_do_exp(nv,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause);
Cyc_InsertChecks_do_stmt(nv,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}}
# 677
static void Cyc_InsertChecks_do_stmt(struct Cyc_InsertChecks_Env nv,struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 goto _LL0;case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_InsertChecks_do_exp(nv,e);goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
Cyc_InsertChecks_do_stmt(nv,s1);Cyc_InsertChecks_do_stmt(nv,s2);goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp4;
if((unsigned)eopt)Cyc_InsertChecks_do_exp(nv,eopt);goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 684
Cyc_InsertChecks_do_exp(nv,e);Cyc_InsertChecks_do_stmt(nv,s1);Cyc_InsertChecks_do_stmt(nv,s2);goto _LL0;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
_Tmp4=e;_Tmp3=s1;goto _LLE;}case 5: _Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
Cyc_InsertChecks_do_exp(nv,e);Cyc_InsertChecks_do_stmt(nv,s1);goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 688
Cyc_InsertChecks_do_exp(nv,e1);Cyc_InsertChecks_do_exp(nv,e2);Cyc_InsertChecks_do_exp(nv,e3);Cyc_InsertChecks_do_stmt(nv,s1);goto _LL0;}case 6:
 goto _LL14;case 7: _LL14:
 goto _LL16;case 8: _LL16:
 return;case 11: _Tmp4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp4;
# 693
for(1;es!=0;es=es->tl){
Cyc_InsertChecks_do_exp(nv,(struct Cyc_Absyn_Exp*)es->hd);}
goto _LL0;}case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;
Cyc_InsertChecks_do_stmt(nv,s1);goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
Cyc_InsertChecks_do_decl(nv,d);Cyc_InsertChecks_do_stmt(nv,s1);goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*scs=_Tmp3;
Cyc_InsertChecks_do_exp(nv,e);Cyc_InsertChecks_do_scs(nv,scs);goto _LL0;}default: _Tmp4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_List_List*scs=_Tmp3;
Cyc_InsertChecks_do_stmt(nv,s1);Cyc_InsertChecks_do_scs(nv,scs);goto _LL0;}}_LL0:;}
# 703
float Cyc_InsertChecks_percent(int i,int total){
float ans=(float)1;
if(total!=0)
ans=(float)i / (float)total;
# 708
return ans * (float)100;}
# 712
static int Cyc_InsertChecks_visit_esc_fn_exp(struct Cyc_Hashtable_Table*env,struct Cyc_Absyn_Exp*e){
# 714
{void*_Tmp0=e->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)==2){_Tmp1=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 718
if(!((unsigned)({struct Cyc_Absyn_Exp**(*_Tmp2)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*)=(struct Cyc_Absyn_Exp**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Hashtable_lookup_opt;_Tmp2;})(env,e)))
fd->escapes=1;
# 721
goto _LL0;}}else{goto _LL5;}case 10: _Tmp1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
# 723
{void*_Tmp2=e1->r;void*_Tmp3;switch(*((int*)_Tmp2)){case 12: _Tmp3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*f=_Tmp3;
_Tmp3=f;goto _LLB;}case 13: _Tmp3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp2)->f1;_LLB: {struct Cyc_Absyn_Exp*f=_Tmp3;
# 726
({void(*_Tmp4)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Hashtable_insert;_Tmp4;})(env,f,f);
goto _LL7;}default:
# 729
({void(*_Tmp4)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Hashtable_insert;_Tmp4;})(env,e1,e1);
goto _LL7;}_LL7:;}
# 732
goto _LL0;}default: _LL5:
# 734
 goto _LL0;}_LL0:;}
# 736
return 1;}
# 739
static int Cyc_InsertChecks_visit_esc_fn_stmt(struct Cyc_Hashtable_Table*env,struct Cyc_Absyn_Stmt*s){
# 741
return 1;}
# 744
static void Cyc_InsertChecks_mark_escaped_fns(struct Cyc_List_List*ds){
for(1;ds!=0;ds=ds->tl){
struct Cyc_Hashtable_Table*table=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create;_Tmp0;})(313,({int(*_Tmp0)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp;_Tmp0;}),({int(*_Tmp0)(struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*))Cyc_InsertChecks_hash_ptr;_Tmp0;}));
void*_Tmp0=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 749
if(vd->initializer!=0)
({void(*_Tmp2)(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*),struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*),struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;_Tmp2;})(Cyc_InsertChecks_visit_esc_fn_exp,Cyc_InsertChecks_visit_esc_fn_stmt,table,vd->initializer);
# 752
goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 755
if((int)fd->sc!=0)fd->escapes=1;
({void(*_Tmp2)(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*),struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*),struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Absyn_visit_stmt;_Tmp2;})(Cyc_InsertChecks_visit_esc_fn_exp,Cyc_InsertChecks_visit_esc_fn_stmt,table,fd->body);
goto _LL0;}case 2: _Tmp1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp1;
# 759
({void(*_Tmp2)(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*),struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*),struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;_Tmp2;})(Cyc_InsertChecks_visit_esc_fn_exp,Cyc_InsertChecks_visit_esc_fn_stmt,table,e);
goto _LL0;}case 3:
 goto _LL0;case 4: _Tmp1=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*eo=_Tmp1;
# 763
if(eo!=0)
({void(*_Tmp2)(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*),struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*),struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;_Tmp2;})(Cyc_InsertChecks_visit_esc_fn_exp,Cyc_InsertChecks_visit_esc_fn_stmt,table,eo);
# 766
goto _LL0;}case 5:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 8: _LL12:
 goto _LL14;case 13: _LL14:
 goto _LL16;case 14: _LL16:
 goto _LL18;case 15: _LL18:
 goto _LL1A;case 16: _LL1A:
 goto _LL1C;case 12: _LL1C:
 goto _LL0;case 11: _Tmp1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds=_Tmp1;
_Tmp1=ds;goto _LL20;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL20: {struct Cyc_List_List*ds=_Tmp1;
_Tmp1=ds;goto _LL22;}default: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL22: {struct Cyc_List_List*ds=_Tmp1;
# 779
Cyc_InsertChecks_mark_escaped_fns(ds);}}_LL0:;}}
# 783
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*tds,struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,int do_vcgen){
# 785
Cyc_InsertChecks_reset_counters();{
# 787
struct Cyc_InsertChecks_Env nv=Cyc_InsertChecks_toplevel_env(tables,do_vcgen);
# 792
struct Cyc_Dict_Dict cg=Cyc_Callgraph_compute_callgraph(tds);
# 797
Cyc_InsertChecks_mark_escaped_fns(tds);
# 800
for(1;tds!=0;tds=tds->tl){
Cyc_InsertChecks_do_decl(nv,(struct Cyc_Absyn_Decl*)tds->hd);}{
# 804
struct Cyc_List_List*cg_fds=Cyc_Graph_tsort(cg);
# 809
{struct Cyc_List_List*fds=Cyc_List_rev(cg_fds);for(0;fds!=0;fds=fds->tl){
Cyc_InsertChecks_do_fndecl(nv,(struct Cyc_Absyn_Fndecl*)fds->hd);}}
# 822 "insert_checks.cyc"
if(Cyc_Flags_verbose){
unsigned eliminated_bounds_checks=(Cyc_InsertChecks_types_eliminated_bounds_checks + Cyc_InsertChecks_simple_eliminated_bounds_checks)+ Cyc_InsertChecks_constraints_eliminated_bounds_checks;
# 827
unsigned eliminated_null_checks=((Cyc_InsertChecks_types_eliminated_null_checks + Cyc_InsertChecks_flow_eliminated_null_checks)+ Cyc_InsertChecks_simple_eliminated_null_checks)+ Cyc_InsertChecks_constraints_eliminated_null_checks;
# 832
Cyc_fprintf(Cyc_stderr,_tag_fat("****INSERT CHECKS***********************************\n",sizeof(char),54U),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)(eliminated_bounds_checks + eliminated_null_checks));_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)((int)(Cyc_InsertChecks_total_bounds_checks + Cyc_InsertChecks_total_null_checks));_Tmp2;});struct Cyc_Double_pa_PrintArg_struct _Tmp2=({struct Cyc_Double_pa_PrintArg_struct _Tmp3;_Tmp3.tag=2,({
# 836
double _Tmp4=(double)Cyc_InsertChecks_percent((int)(eliminated_bounds_checks + eliminated_null_checks),(int)(Cyc_InsertChecks_total_bounds_checks + Cyc_InsertChecks_total_null_checks));_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_fprintf(Cyc_stderr,_tag_fat("eliminated %d out of a total of %d checks (%g%%).\n",sizeof(char),51U),_tag_fat(_Tmp3,sizeof(void*),3));});
# 838
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)eliminated_bounds_checks);_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)((int)Cyc_InsertChecks_total_bounds_checks);_Tmp2;});struct Cyc_Double_pa_PrintArg_struct _Tmp2=({struct Cyc_Double_pa_PrintArg_struct _Tmp3;_Tmp3.tag=2,({
# 840
double _Tmp4=(double)Cyc_InsertChecks_percent((int)eliminated_bounds_checks,(int)Cyc_InsertChecks_total_bounds_checks);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_fprintf(Cyc_stderr,_tag_fat("eliminated %d out of a total of %d bounds checks (%g%%).\n",sizeof(char),58U),_tag_fat(_Tmp3,sizeof(void*),3));});
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)Cyc_InsertChecks_types_eliminated_bounds_checks);_Tmp1;});struct Cyc_Double_pa_PrintArg_struct _Tmp1=({struct Cyc_Double_pa_PrintArg_struct _Tmp2;_Tmp2.tag=2,({
# 843
double _Tmp3=(double)Cyc_InsertChecks_percent((int)Cyc_InsertChecks_types_eliminated_bounds_checks,(int)Cyc_InsertChecks_total_bounds_checks);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("\tbounds checks eliminated by types            : %d (%g%%)\n",sizeof(char),59U),_tag_fat(_Tmp2,sizeof(void*),2));});
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)Cyc_InsertChecks_simple_eliminated_bounds_checks);_Tmp1;});struct Cyc_Double_pa_PrintArg_struct _Tmp1=({struct Cyc_Double_pa_PrintArg_struct _Tmp2;_Tmp2.tag=2,({
# 846
double _Tmp3=(double)Cyc_InsertChecks_percent((int)Cyc_InsertChecks_simple_eliminated_bounds_checks,(int)Cyc_InsertChecks_total_bounds_checks);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("\tbounds checks eliminated by simple prover    : %d (%g%%)\n",sizeof(char),59U),_tag_fat(_Tmp2,sizeof(void*),2));});
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)Cyc_InsertChecks_constraints_eliminated_bounds_checks);_Tmp1;});struct Cyc_Double_pa_PrintArg_struct _Tmp1=({struct Cyc_Double_pa_PrintArg_struct _Tmp2;_Tmp2.tag=2,({
# 849
double _Tmp3=(double)Cyc_InsertChecks_percent((int)Cyc_InsertChecks_constraints_eliminated_bounds_checks,(int)Cyc_InsertChecks_total_bounds_checks);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("\tbounds checks eliminated by constraint solver: %d (%g%%)\n",sizeof(char),59U),_tag_fat(_Tmp2,sizeof(void*),2));});
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)eliminated_null_checks);_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)((int)Cyc_InsertChecks_total_null_checks);_Tmp2;});struct Cyc_Double_pa_PrintArg_struct _Tmp2=({struct Cyc_Double_pa_PrintArg_struct _Tmp3;_Tmp3.tag=2,({
# 852
double _Tmp4=(double)Cyc_InsertChecks_percent((int)eliminated_null_checks,(int)Cyc_InsertChecks_total_null_checks);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_fprintf(Cyc_stderr,_tag_fat("eliminated %d out of a total of %d null checks (%g%%).\n",sizeof(char),56U),_tag_fat(_Tmp3,sizeof(void*),3));});
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)Cyc_InsertChecks_types_eliminated_null_checks);_Tmp1;});struct Cyc_Double_pa_PrintArg_struct _Tmp1=({struct Cyc_Double_pa_PrintArg_struct _Tmp2;_Tmp2.tag=2,({
# 855
double _Tmp3=(double)Cyc_InsertChecks_percent((int)Cyc_InsertChecks_types_eliminated_null_checks,(int)Cyc_InsertChecks_total_null_checks);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("\tnull checks eliminated by types              : %d (%g%%)\n",sizeof(char),59U),_tag_fat(_Tmp2,sizeof(void*),2));});
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)Cyc_InsertChecks_flow_eliminated_null_checks);_Tmp1;});struct Cyc_Double_pa_PrintArg_struct _Tmp1=({struct Cyc_Double_pa_PrintArg_struct _Tmp2;_Tmp2.tag=2,({
# 858
double _Tmp3=(double)Cyc_InsertChecks_percent((int)Cyc_InsertChecks_flow_eliminated_null_checks,(int)Cyc_InsertChecks_total_null_checks);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("\tnull checks eliminated by flow analysis      : %d (%g%%)\n",sizeof(char),59U),_tag_fat(_Tmp2,sizeof(void*),2));});
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)Cyc_InsertChecks_simple_eliminated_null_checks);_Tmp1;});struct Cyc_Double_pa_PrintArg_struct _Tmp1=({struct Cyc_Double_pa_PrintArg_struct _Tmp2;_Tmp2.tag=2,({
# 861
double _Tmp3=(double)Cyc_InsertChecks_percent((int)Cyc_InsertChecks_simple_eliminated_null_checks,(int)Cyc_InsertChecks_total_null_checks);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("\tnull checks eliminated by simple prover      : %d (%g%%)\n",sizeof(char),59U),_tag_fat(_Tmp2,sizeof(void*),2));});
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)Cyc_InsertChecks_constraints_eliminated_null_checks);_Tmp1;});struct Cyc_Double_pa_PrintArg_struct _Tmp1=({struct Cyc_Double_pa_PrintArg_struct _Tmp2;_Tmp2.tag=2,({
# 864
double _Tmp3=(double)Cyc_InsertChecks_percent((int)Cyc_InsertChecks_constraints_eliminated_null_checks,(int)Cyc_InsertChecks_total_null_checks);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("\tnull checks eliminated by constraint solver  : %d (%g%%)\n",sizeof(char),59U),_tag_fat(_Tmp2,sizeof(void*),2));});
Cyc_fprintf(Cyc_stderr,_tag_fat("****************************************************\n",sizeof(char),54U),_tag_fat(0U,sizeof(void*),0));}}}}
