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
void _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void _pop_dynregion();

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
  _zero_arr_inplace_plus_other_fn(t,(void***)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_otherr(t,x,i) \
  _zero_arr_inplace_plus_post_other_fn(t,(void***)(x),(i),__FILE__,__LINE__)

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
      _curr != (unsigned char*)0) \
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
 struct Cyc_Core_Opt{void*v;};
# 123 "core.h"
int Cyc_Core_ptrcmp(void*,void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 36 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 469 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 476
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 494
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 508
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 874 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 892
extern void*Cyc_Absyn_uint_type;
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 40 "tcutil.h"
int Cyc_Tcutil_is_integral_type(void*);
# 63
void*Cyc_Tcutil_pointer_elt_type(void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};
# 65 "assndef.h"
void*Cyc_AssnDef_zero (void);
void*Cyc_AssnDef_one (void);
# 71
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*,void*);
void*Cyc_AssnDef_cast(void*,void*);
void*Cyc_AssnDef_fresh_var(void*);
# 76
void*Cyc_AssnDef_get_term_type(void*);
# 80
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 92
int Cyc_AssnDef_assncmp(void*,void*);
# 94
struct _fat_ptr Cyc_AssnDef_assn2string(void*);
# 97
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 108 "assndef.h"
void Cyc_AssnDef_reset_hash_cons_table (void);
# 115
void*Cyc_AssnDef_kill(void*);
# 117
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*,void*,void*);
# 122
void*Cyc_AssnDef_eq(void*,void*);
void*Cyc_AssnDef_neq(void*,void*);
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 130
void*Cyc_AssnDef_reduce(void*);
void*Cyc_AssnDef_subst_term(void*,struct Cyc_Absyn_Vardecl*,void*);
# 133
int Cyc_AssnDef_simple_prove(void*,void*);
# 135
void*Cyc_AssnDef_kill_mem_term(void*);
# 52 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 71 "flags.h"
extern int Cyc_Flags_allpaths;struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 126 "dict.h"
extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict,void*,void**);
# 193
extern struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 79
extern struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 114
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_Vcgen_Env;
# 108 "vcgen.cyc"
static int Cyc_Vcgen_simple_implies(void**a1,void**a2){
({void*_Tmp0=Cyc_AssnDef_reduce(*a1);*a1=_Tmp0;});
({void*_Tmp0=Cyc_AssnDef_reduce(*a2);*a2=_Tmp0;});
return Cyc_AssnDef_simple_prove(*a1,*a2);}
# 115
static int Cyc_Vcgen_implies(void**a1,void**a2){
return Cyc_Vcgen_simple_implies(a1,a2)|| Cyc_PrattProver_constraint_prove(*a1,*a2);}
# 130
static struct Cyc_Set_Set*Cyc_Vcgen_widen_it(struct Cyc_Set_Set*sopt,void*a){
if(sopt==0)return 0;{
struct Cyc_Set_Set*s=sopt;
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 2: switch((int)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2){case Cyc_AssnDef_Eq: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;void*t2=_Tmp1;
# 135
s=Cyc_Set_insert(s,a);{
# 139
void*topt1=Cyc_AssnDef_get_term_type(t1);
void*topt2=Cyc_AssnDef_get_term_type(t2);
if(topt1!=0 && Cyc_Tcutil_is_integral_type(topt1)||
 topt2!=0 && Cyc_Tcutil_is_integral_type(topt2)){
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t2,t1));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t2,t1));});}
# 148
return s;}}case Cyc_AssnDef_SLt: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;void*t2=_Tmp1;
# 150
s=Cyc_Set_insert(s,a);
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
return s;}case Cyc_AssnDef_ULt: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;void*t2=_Tmp1;
# 154
s=Cyc_Set_insert(s,a);
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
return s;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
return Cyc_Set_insert(s,a);}}case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return({struct Cyc_Set_Set*_Tmp3=Cyc_Vcgen_widen_it(s,a1);Cyc_Vcgen_widen_it(_Tmp3,a2);});}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 160
struct Cyc_Set_Set*s1=Cyc_Vcgen_widen_it(s,a1);
struct Cyc_Set_Set*s2=Cyc_Vcgen_widen_it(s,a2);
if(s1==0)return s2;
if(s2==0)return s1;
return Cyc_Set_intersect(s1,s2);}case 0:
 return s;case 1:
 return 0;default:
 return({struct Cyc_Set_Set*_Tmp3=s;Cyc_Vcgen_widen_it(_Tmp3,Cyc_AssnDef_reduce(a));});};}}
# 171
static void*Cyc_Vcgen_widen(void*a){
struct Cyc_Set_Set*sopt=({struct Cyc_Set_Set*_Tmp0=Cyc_Set_empty(Cyc_AssnDef_assncmp);Cyc_Vcgen_widen_it(_Tmp0,a);});
if(sopt==0)return(void*)& Cyc_AssnDef_false_assn;
return Cyc_Set_fold(Cyc_AssnDef_and,sopt,(void*)& Cyc_AssnDef_true_assn);}
# 181
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Vardecl*);
# 183
static void*Cyc_Vcgen_kill_var(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
if(vd->escapes)
a=Cyc_AssnDef_kill(a);else{
# 187
void*v2=Cyc_Vcgen_fresh_nonescvar(env,vd);
a=Cyc_AssnDef_subst(vd,v2,a);}
# 190
return a;}
# 194
static void*Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,void*a,void*tgt,void*src){
void*_Tmp0;if(tgt==0)
# 199
return Cyc_AssnDef_kill(a);else{if(*((int*)tgt)==2){_Tmp0=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)tgt)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
# 205
if(vd->escapes){
a=Cyc_AssnDef_kill(a);
src=src==0?0: Cyc_AssnDef_kill_mem_term(src);}else{
# 209
void*v2=Cyc_Vcgen_fresh_nonescvar(env,vd);
a=Cyc_AssnDef_subst(vd,v2,a);
src=src==0?0: Cyc_AssnDef_subst_term(src,vd,v2);}
# 214
return({void*_Tmp1=a;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(tgt,src));});}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp1="vcgen do_assign";_tag_fat(_Tmp1,sizeof(char),16U);}),_tag_fat(0U,sizeof(void*),0));}};}struct Cyc_Vcgen_SharedEnv{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;void**try_assn;void**exp_stmt;void*res_term;void**res_assn;int widen_paths;};struct Cyc_Vcgen_Env{struct Cyc_Vcgen_SharedEnv*shared;struct Cyc_Dict_Dict state_counter;struct Cyc_Hashtable_Table**assn_info;};
# 257 "vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 259
static void*Cyc_Vcgen_may_widen(struct Cyc_Vcgen_Env*env,void*a){
return env->shared->widen_paths?Cyc_Vcgen_widen(a): a;}
# 263
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info,void**res_assn,void*res_term){
# 265
struct Cyc_Hashtable_Table*succ_table=({(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;})(tables->succ_tables,fd);
struct Cyc_Hashtable_Table*assn_table=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;})(57,({(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;}));
struct Cyc_Vcgen_SharedEnv*senv;senv=_cycalloc(sizeof(struct Cyc_Vcgen_SharedEnv)),senv->assn_table=assn_table,senv->succ_table=succ_table,senv->try_assn=0,senv->exp_stmt=0,senv->res_term=res_term,senv->res_assn=res_assn,senv->widen_paths=!Cyc_Flags_allpaths;{
# 274
struct Cyc_Dict_Dict d=({(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty;})(({(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp;}));
return({struct Cyc_Vcgen_Env*_Tmp0=_cycalloc(sizeof(struct Cyc_Vcgen_Env));_Tmp0->shared=senv,_Tmp0->state_counter=d,_Tmp0->assn_info=assn_info;_Tmp0;});}}
# 278
static struct Cyc_Vcgen_Env*Cyc_Vcgen_copy_env(struct Cyc_Vcgen_Env*env){
return({struct Cyc_Vcgen_Env*_Tmp0=_cycalloc(sizeof(struct Cyc_Vcgen_Env));*_Tmp0=*env;_Tmp0;});}
# 284
static int Cyc_Vcgen_max_counter(int dummy,struct Cyc_Absyn_Vardecl*vd,int i,int j){
return i < j?j: i;}
# 288
static void Cyc_Vcgen_merge_env(struct Cyc_Vcgen_Env*old_env,struct Cyc_Vcgen_Env*new_env){
({struct Cyc_Dict_Dict _Tmp0=({(struct Cyc_Dict_Dict(*)(int(*)(int,struct Cyc_Absyn_Vardecl*,int,int),int,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_union_two_c;})(Cyc_Vcgen_max_counter,0,old_env->state_counter,new_env->state_counter);old_env->state_counter=_Tmp0;});}
# 296
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){
int res=0;
({(int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int*))Cyc_Dict_lookup_bool;})(env->state_counter,vd,& res);
({struct Cyc_Dict_Dict _Tmp0=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int))Cyc_Dict_insert;})(env->state_counter,vd,res + 1);env->state_counter=_Tmp0;});
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct));_Tmp0->tag=3,_Tmp0->f1=vd,_Tmp0->f2=res,_Tmp0->f3=vd->type;_Tmp0;});}
# 305
static void*Cyc_Vcgen_pre_stmt_update(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***popt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})(env->shared->assn_table,s);
if(popt!=0){
void**p=*popt;
void*new_precondition=Cyc_AssnDef_or(ain,*p);
*p=new_precondition;
return new_precondition;}else{
# 313
return ain;}}
# 317
static void**Cyc_Vcgen_get_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){
static void*false_pointer=(void*)& Cyc_AssnDef_false_assn;
void***popt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})(env->shared->assn_table,s);
if(popt!=0)
return*popt;else{
# 323
return& false_pointer;}}
# 328
static void*Cyc_Vcgen_loop_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})(env->shared->assn_table,s);
if(aprevopt!=0){
void**aprev=*aprevopt;
# 333
ain=Cyc_Vcgen_widen(Cyc_AssnDef_or(ain,*aprev));
*aprev=ain;}else{
# 336
({struct Cyc_Hashtable_Table*_Tmp0=env->shared->assn_table;struct Cyc_Absyn_Stmt*_Tmp1=s;({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert;})(_Tmp0,_Tmp1,({void**_Tmp2=_cycalloc(sizeof(void*));*_Tmp2=ain;_Tmp2;}));});}
# 338
return ain;}
# 343
static void*Cyc_Vcgen_forward_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})(env->shared->assn_table,s);
if(aprevopt!=0){
void**aprev=*aprevopt;
ain=({struct Cyc_Vcgen_Env*_Tmp0=env;Cyc_Vcgen_may_widen(_Tmp0,Cyc_AssnDef_or(ain,*aprev));});
*aprev=ain;}else{
# 350
({struct Cyc_Hashtable_Table*_Tmp0=env->shared->assn_table;struct Cyc_Absyn_Stmt*_Tmp1=s;({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert;})(_Tmp0,_Tmp1,({void**_Tmp2=_cycalloc(sizeof(void*));*_Tmp2=ain;_Tmp2;}));});}
# 352
return ain;}
# 357
static void Cyc_Vcgen_update_try_assn(struct Cyc_Vcgen_Env*env,void*a){
if(env->shared->try_assn==0)return;
a=({struct Cyc_Vcgen_Env*_Tmp0=env;Cyc_Vcgen_may_widen(_Tmp0,Cyc_AssnDef_or(*_check_null(env->shared->try_assn),a));});
*_check_null(env->shared->try_assn)=a;}
# 364
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*t,void*a,void*topt){
struct Cyc_Vcgen_SharedEnv*shared=env->shared;
if(shared->res_assn==0)return;
if(shared->res_term!=0)
({void*_Tmp0=({void*_Tmp1=({void*_Tmp2=a;Cyc_AssnDef_and(_Tmp2,Cyc_AssnDef_eq(shared->res_term,t));});Cyc_AssnDef_or(_Tmp1,*_check_null(shared->res_assn));});*_check_null(shared->res_assn)=_Tmp0;});else{
# 370
({void*_Tmp0=Cyc_AssnDef_or(a,*_check_null(shared->res_assn));*_check_null(shared->res_assn)=_Tmp0;});}}
# 375
struct Cyc_Hashtable_Table**Cyc_Vcgen_new_assn_info (void){
struct Cyc_Hashtable_Table*assn_info=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create;})(57,({(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Exp*))Cyc_Vcgen_hash_ptr;}));
return({struct Cyc_Hashtable_Table**_Tmp0=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));*_Tmp0=assn_info;_Tmp0;});}struct _tuple13{void*f1;void*f2;};
# 380
struct _tuple13 Cyc_Vcgen_exp2ctxt_assn(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e){
struct _tuple13**result=({(struct _tuple13**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Hashtable_lookup_opt;})(*_check_null(assn_info),e);
if(result==0)
# 385
return({struct _tuple13 _Tmp0;_Tmp0.f1=(void*)& Cyc_AssnDef_true_assn,_Tmp0.f2=(void*)& Cyc_AssnDef_false_assn;_Tmp0;});else{
# 387
return*(*result);}}
# 391
static void Cyc_Vcgen_insert_assn_info(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e,struct _tuple13*ctxt_assn){
# 399
({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct _tuple13*))Cyc_Hashtable_insert;})(*_check_null(assn_info),e,ctxt_assn);
return;}
# 403
static struct Cyc_Absyn_Exp*Cyc_Vcgen_zero_exp (void){
struct Cyc_Absyn_Exp*ans=Cyc_Absyn_uint_exp(0U,0U);
ans->topt=Cyc_Absyn_uint_type;
return ans;}
# 418
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 425
static struct _tuple13 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);struct _tuple14{struct Cyc_List_List*f1;void*f2;};
# 434
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);
# 436
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);
# 438
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Decl*,void*);
# 440
static struct _fat_ptr Cyc_Vcgen_stmt2shortstring(struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 4: _Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 443
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(e);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="if (%s) ...";_tag_fat(_Tmp6,sizeof(char),12U);}),_tag_fat(_Tmp5,sizeof(void*),1));});}case 5: _Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 445
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(e);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="while (%s) ...";_tag_fat(_Tmp6,sizeof(char),15U);}),_tag_fat(_Tmp5,sizeof(void*),1));});}case 9: _Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f1;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 447
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(e1);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=Cyc_Absynpp_exp2string(e2);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_exp2string(e3);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="for (%s;%s;%s) ...";_tag_fat(_Tmp8,sizeof(char),19U);}),_tag_fat(_Tmp7,sizeof(void*),3));});}case 10: _Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 450
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(e);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="switch (%s) ...";_tag_fat(_Tmp6,sizeof(char),16U);}),_tag_fat(_Tmp5,sizeof(void*),1));});}case 14: _Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 452
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(e);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="do ... while (%s)";_tag_fat(_Tmp6,sizeof(char),18U);}),_tag_fat(_Tmp5,sizeof(void*),1));});}case 15: _Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp3;
# 454
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Vcgen_stmt2shortstring(s);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="try %s catch ...";_tag_fat(_Tmp6,sizeof(char),17U);}),_tag_fat(_Tmp5,sizeof(void*),1));});}case 2: _Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp3;
return Cyc_Vcgen_stmt2shortstring(s);}default:  {
# 457
struct _fat_ptr _Tmp4=Cyc_Absynpp_stmt2string(s);struct _fat_ptr s=_Tmp4;
unsigned max=_get_fat_size(s,sizeof(char))< 11U?_get_fat_size(s,sizeof(char)): 11U;
struct _fat_ptr res=({unsigned _Tmp5=sizeof(char)* max;_tag_fat(_cycalloc_atomic(_Tmp5),1U,_Tmp5);});
{int i=0;for(0;(unsigned)i < max - 1U;++ i){
({char _Tmp5=*((const char*)_check_fat_subscript(s,sizeof(char),i));*((char*)_check_fat_subscript(res,sizeof(char),i))=_Tmp5;});}}
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=res;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="%s...";_tag_fat(_Tmp7,sizeof(char),6U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}};}
# 517 "vcgen.cyc"
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
# 520
LOOP:
 ain=Cyc_Vcgen_pre_stmt_update(env,s,ain);{
# 532 "vcgen.cyc"
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 0:
 return ain;case 1: _Tmp6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 535
struct _tuple13 _Tmp7=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{void*t=_Tmp9;void*aout=_Tmp8;
# 538
if(env->shared->exp_stmt!=0)
*_check_null(env->shared->exp_stmt)=t;
return aout;}}case 2: _Tmp6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;struct Cyc_Absyn_Stmt*s2=_Tmp5;
# 542
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,ain);
s=s2;
goto LOOP;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1==0){
# 546
Cyc_Vcgen_update_return(env,0,ain,0);
return(void*)& Cyc_AssnDef_false_assn;}else{_Tmp6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 549
struct _tuple13 _Tmp7=Cyc_Vcgen_vcgen_rexp_nodebug(env,_check_null(e),ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{void*t=_Tmp9;void*aout=_Tmp8;
Cyc_Vcgen_update_return(env,t,aout,e->topt);
return(void*)& Cyc_AssnDef_false_assn;}}}case 4: _Tmp6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;struct Cyc_Absyn_Stmt*s2=_Tmp4;
# 553
struct _tuple13 _Tmp7=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{void*at=_Tmp9;void*af=_Tmp8;
struct Cyc_Vcgen_Env*env2=Cyc_Vcgen_copy_env(env);
at=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,at);
af=Cyc_Vcgen_vcgen_stmt_nodebug(env2,s2,af);
Cyc_Vcgen_merge_env(env,env2);
return({struct Cyc_Vcgen_Env*_TmpA=env;Cyc_Vcgen_may_widen(_TmpA,Cyc_AssnDef_or(at,af));});}}case 5: _Tmp6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f1;_Tmp5=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f2;_Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*dummy=_Tmp5;struct Cyc_Absyn_Stmt*s=_Tmp4;
# 565
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_Tmp7=e->r;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp7)==7){_Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f1;_Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Exp*eleft=_Tmp9;struct Cyc_Absyn_Exp*eright=_Tmp8;
# 570
{void*_TmpA=eleft->r;void*_TmpB;if(*((int*)_TmpA)==42){_TmpB=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TmpA)->f1;{struct Cyc_Absyn_Exp*eassn=_TmpB;
# 572
struct _tuple13 _TmpC=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{void*tm=_TmpE;void*an=_TmpD;
loop_inv=Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn).f1;
e=eright;
ain=an;
goto _LL33;}}}else{
# 578
goto _LL33;}_LL33:;}
# 580
goto _LL2E;}}else{
# 582
goto _LL2E;}_LL2E:;}
# 584
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy,ain);
ain=Cyc_AssnDef_and(ain,loop_inv);{
struct _tuple13 _Tmp7=Cyc_Vcgen_vcgen_test(env,e,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{void*at=_Tmp9;void*af=_Tmp8;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 591
({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=Cyc_AssnDef_assn2string(loop_inv);_TmpB.f1=_TmpC;});_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({struct _fat_ptr _TmpD=Cyc_AssnDef_assn2string(ain);_TmpC.f1=_TmpD;});_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_Warn_err(e->loc,({const char*_TmpD="cannot prove loop invariant %s from %s\n";_tag_fat(_TmpD,sizeof(char),40U);}),_tag_fat(_TmpC,sizeof(void*),2));});
return(void*)& Cyc_AssnDef_true_assn;}
# 595
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;}}}}case 7:
# 597
 goto _LL12;{
# 600
struct Cyc_Absyn_Stmt*dest=({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})(env->shared->succ_table,s);
Cyc_Vcgen_loop_assn(env,dest,ain);
return(void*)& Cyc_AssnDef_false_assn;}case 6: _LL12: {
# 607
struct Cyc_Absyn_Stmt*dest=({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})(env->shared->succ_table,s);
if(dest==0)
Cyc_Vcgen_update_return(env,0,ain,0);else{
# 611
Cyc_Vcgen_forward_assn(env,dest,ain);}
return(void*)& Cyc_AssnDef_false_assn;}case 9: _Tmp6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f2;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f1;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f2;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Stmt*dummy2=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;struct Cyc_Absyn_Stmt*dummy3=_Tmp2;struct Cyc_Absyn_Stmt*s=_Tmp1;
# 614
struct _tuple13 _Tmp7=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_Tmp8;_Tmp8=_Tmp7.f2;{void*a=_Tmp8;
ain=a;{
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_Tmp9=e2->r;void*_TmpA;void*_TmpB;if(*((int*)_Tmp9)==7){_TmpB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp9)->f1;_TmpA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp9)->f2;{struct Cyc_Absyn_Exp*eleft=_TmpB;struct Cyc_Absyn_Exp*eright=_TmpA;
# 621
{void*_TmpC=eleft->r;void*_TmpD;if(*((int*)_TmpC)==42){_TmpD=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TmpC)->f1;{struct Cyc_Absyn_Exp*eassn=_TmpD;
# 623
struct _tuple13 _TmpE=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);void*_TmpF;void*_Tmp10;_Tmp10=_TmpE.f1;_TmpF=_TmpE.f2;{void*tm=_Tmp10;void*an=_TmpF;
loop_inv=Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn).f1;
e2=eright;
ain=an;
goto _LL46;}}}else{
# 629
goto _LL46;}_LL46:;}
# 631
goto _LL41;}}else{
# 633
goto _LL41;}_LL41:;}
# 635
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy2,ain);
ain=Cyc_AssnDef_and(ain,loop_inv);{
struct _tuple13 _Tmp9=Cyc_Vcgen_vcgen_test(env,e2,ain);void*_TmpA;void*_TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{void*at=_TmpB;void*af=_TmpA;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
# 641
ain=Cyc_Vcgen_pre_stmt_update(env,dummy3,ain);
ain=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,ain).f2;
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 645
({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,({
struct _fat_ptr _TmpE=Cyc_AssnDef_assn2string(loop_inv);_TmpD.f1=_TmpE;});_TmpD;});struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,({struct _fat_ptr _TmpF=Cyc_AssnDef_assn2string(ain);_TmpE.f1=_TmpF;});_TmpE;});void*_TmpE[2];_TmpE[0]=& _TmpC,_TmpE[1]=& _TmpD;Cyc_Warn_err(e2->loc,({const char*_TmpF="cannot prove loop invariant %s from %s\n";_tag_fat(_TmpF,sizeof(char),40U);}),_tag_fat(_TmpE,sizeof(void*),2));});
return(void*)& Cyc_AssnDef_true_assn;}
# 649
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy2))&&
 Cyc_Vcgen_implies(& ain,& loop_inv))
return Cyc_AssnDef_and(loop_inv,af);}}}}}}case 14: _Tmp6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f2;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_Absyn_Exp*e=_Tmp5;struct Cyc_Absyn_Stmt*dummy=_Tmp4;
# 656
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_Tmp7=e->r;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp7)==7){_Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f1;_Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Exp*eleft=_Tmp9;struct Cyc_Absyn_Exp*eright=_Tmp8;
# 661
{void*_TmpA=eleft->r;void*_TmpB;if(*((int*)_TmpA)==42){_TmpB=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TmpA)->f1;{struct Cyc_Absyn_Exp*eassn=_TmpB;
# 663
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
struct _tuple13 _TmpC=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{void*tm=_TmpE;void*an=_TmpD;
loop_inv=Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn).f1;
e=eright;
ain=an;
goto _LL56;}}}}else{
# 670
goto _LL56;}_LL56:;}
# 672
goto _LL51;}}else{
# 674
goto _LL51;}_LL51:;}
# 676
while(1){
ain=Cyc_AssnDef_and(ain,loop_inv);
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 681
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=Cyc_AssnDef_assn2string(loop_inv);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_AssnDef_assn2string(ain);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_err(e->loc,({const char*_TmpA="cannot prove loop invariant %s from %s\n";_tag_fat(_TmpA,sizeof(char),40U);}),_tag_fat(_Tmp9,sizeof(void*),2));});
return(void*)& Cyc_AssnDef_true_assn;}{
# 685
struct _tuple13 _Tmp7=Cyc_Vcgen_vcgen_test(env,e,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{void*at=_Tmp9;void*af=_Tmp8;
ain=({struct Cyc_Vcgen_Env*_TmpA=env;Cyc_Vcgen_may_widen(_TmpA,({void*_TmpB=*Cyc_Vcgen_get_assn(env,dummy);Cyc_AssnDef_or(_TmpB,ain);}));});
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;
Cyc_Vcgen_loop_assn(env,dummy,ain);}}}}case 10: _Tmp6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_List_List*scs=_Tmp5;
# 696
struct _tuple13 _Tmp7=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{void*t=_Tmp9;void*a=_Tmp8;
return Cyc_Vcgen_vcgen_switch(env,scs,a);}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2==0)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp7="vcgen fallthru";_tag_fat(_Tmp7,sizeof(char),15U);}),_tag_fat(0U,sizeof(void*),0));else{_Tmp6=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp6;struct Cyc_Absyn_Switch_clause*dest=_Tmp5;
# 701
struct _tuple14 _Tmp7=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{struct Cyc_List_List*ts=_Tmp9;void*a=_Tmp8;
if(ts!=0){
# 704
struct Cyc_List_List*x=Cyc_Tcutil_filter_nulls(Cyc_List_split((struct Cyc_List_List*)_check_null(dest->pat_vars)->v).f1);
# 706
for(1;x!=0;(x=x->tl,ts=ts->tl)){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)x->hd;
if(vd!=0)
a=({struct Cyc_Vcgen_Env*_TmpA=env;void*_TmpB=a;void*_TmpC=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_TmpD=_cycalloc(sizeof(struct Cyc_AssnDef_Var_AssnDef_Term_struct));_TmpD->tag=2,_TmpD->f1=vd;_TmpD;});Cyc_Vcgen_do_assign(_TmpA,_TmpB,_TmpC,(void*)_check_null(ts)->hd);});}}
# 712
Cyc_Vcgen_forward_assn(env,dest->body,a);
return(void*)& Cyc_AssnDef_false_assn;}}}case 12: _Tmp6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;
# 715
ain=Cyc_Vcgen_vcgen_local_decl(env,d,ain);
s=s1;
goto LOOP;}case 13: _Tmp6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;
# 723
ain=(void*)& Cyc_AssnDef_true_assn;
s=s1;
goto LOOP;}case 8:
 return(void*)& Cyc_AssnDef_false_assn;default: _Tmp6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_List_List*scs=_Tmp5;
# 729
void**old_try_assn=env->shared->try_assn;
# 731
({void**_Tmp7=({void**_Tmp8=_cycalloc(sizeof(void*));*_Tmp8=(void*)& Cyc_AssnDef_false_assn;_Tmp8;});env->shared->try_assn=_Tmp7;});
# 734
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
# 736
void*new_try_assn=*_check_null(env->shared->try_assn);
env->shared->try_assn=old_try_assn;
# 740
return({void*_Tmp7=ain;Cyc_AssnDef_or(_Tmp7,Cyc_Vcgen_vcgen_switch(env,scs,new_try_assn));});}}};}}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 744
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*scs,void*ain){
# 748
void*res=(void*)& Cyc_AssnDef_false_assn;
for(1;scs!=0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*sc=(struct Cyc_Absyn_Switch_clause*)scs->hd;
void*a=ain;
{struct Cyc_List_List*pvs=(struct Cyc_List_List*)_check_null(sc->pat_vars)->v;for(0;pvs!=0;pvs=pvs->tl){
struct _tuple15*_Tmp0=(struct _tuple15*)pvs->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{struct Cyc_Absyn_Vardecl**vdopt=_Tmp2;struct Cyc_Absyn_Exp*eopt=_Tmp1;
if(vdopt!=0)
# 759
a=Cyc_Vcgen_kill_var(env,*vdopt,a);}}}
# 763
if(sc->where_clause!=0)
a=Cyc_Vcgen_vcgen_test(env,_check_null(sc->where_clause),a).f1;
res=({void*_Tmp0=res;Cyc_AssnDef_or(_Tmp0,Cyc_Vcgen_vcgen_stmt_nodebug(env,sc->body,a));});}
# 767
return res;}
# 770
static void*Cyc_Vcgen_add_vardecl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
# 773
struct Cyc_AssnDef_Var_AssnDef_Term_struct*tv;tv=_cycalloc(sizeof(struct Cyc_AssnDef_Var_AssnDef_Term_struct)),tv->tag=2,tv->f1=vd;
a=Cyc_Vcgen_kill_var(env,vd,a);
if(vd->initializer!=0){
struct _tuple13 _Tmp0=Cyc_Vcgen_vcgen_rexp_nodebug(env,_check_null(vd->initializer),a);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t=_Tmp2;void*a1=_Tmp1;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}
# 779
return a;}
# 782
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a){
void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
return Cyc_Vcgen_add_vardecl(env,vd,a);}case 2: _Tmp3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Pat*p=_Tmp3;struct Cyc_Core_Opt*vds=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
# 786
struct _tuple13 _Tmp4=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,a);void*_Tmp5;_Tmp5=_Tmp4.f2;{void*a=_Tmp5;
{struct Cyc_List_List*_Tmp6=(struct Cyc_List_List*)_check_null(vds)->v;struct Cyc_List_List*vds=_Tmp6;for(0;vds!=0;vds=vds->tl){
struct _tuple15 _Tmp7=*((struct _tuple15*)vds->hd);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{struct Cyc_Absyn_Vardecl**vdopt=_Tmp9;struct Cyc_Absyn_Exp*eopt=_Tmp8;
if(vdopt!=0){
if(eopt==0)a=Cyc_Vcgen_add_vardecl(env,*vdopt,a);else{
# 792
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*t=_TmpC;void*a1=_TmpB;
struct Cyc_AssnDef_Var_AssnDef_Term_struct*tv;tv=_cycalloc(sizeof(struct Cyc_AssnDef_Var_AssnDef_Term_struct)),tv->tag=2,tv->f1=*vdopt;
a=Cyc_Vcgen_do_assign(env,a1,(void*)tv,t);}}}}}}
# 798
return a;}}case 3: _Tmp3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp3;
# 800
for(1;vds!=0;vds=vds->tl){
a=Cyc_Vcgen_add_vardecl(env,(struct Cyc_Absyn_Vardecl*)vds->hd,a);}
return a;}case 4: _Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Exp*eopt=_Tmp2;
# 804
if(eopt!=0){
struct _tuple13 _Tmp4=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f1;_Tmp5=_Tmp4.f2;{void*t=_Tmp6;void*a1=_Tmp5;
a=a1;}}
# 809
a=Cyc_Vcgen_add_vardecl(env,vd,a);
return a;}default:
 return a;};}
# 817
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress(_check_null(e1->topt));
void*t2=Cyc_Absyn_compress(_check_null(e2->topt));
struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1==Cyc_Absyn_Unsigned)
return 1;else{if(*((int*)_Tmp0.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1==Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_Tmp0.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1==Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_Tmp0.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f2)->f1)->f1==Cyc_Absyn_Unsigned){_LL3:
 return 1;}else{if(*((int*)_Tmp0.f1)==3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_Tmp0.f1)==3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_Tmp0.f1)==3){if(*((int*)_Tmp0.f2)==3)
return 1;else{goto _LL7;}}else{_LL7:
 return 0;}}};}
# 833
static struct _tuple13 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_Tmp0=e->r;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 6: _Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 836
struct _tuple13 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{void*a1t=_Tmp7;void*a1f=_Tmp6;
struct _tuple13 _Tmp8=Cyc_Vcgen_vcgen_test(env,e2,a1t);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{void*a2t=_TmpA;void*a2f=_Tmp9;
struct _tuple13 _TmpB=Cyc_Vcgen_vcgen_test(env,e3,a1f);void*_TmpC;void*_TmpD;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{void*a3t=_TmpD;void*a3f=_TmpC;
return({struct _tuple13 _TmpE;({void*_TmpF=Cyc_AssnDef_or(a2t,a3t);_TmpE.f1=_TmpF;}),({void*_TmpF=Cyc_AssnDef_or(a2f,a3f);_TmpE.f2=_TmpF;});_TmpE;});}}}}case 7: _Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 841
struct _tuple13 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{void*a1t=_Tmp7;void*a1f=_Tmp6;
struct _tuple13 _Tmp8=Cyc_Vcgen_vcgen_test(env,e2,a1t);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{void*a2t=_TmpA;void*a2f=_Tmp9;
return({struct _tuple13 _TmpB;_TmpB.f1=a2t,({void*_TmpC=Cyc_AssnDef_or(a1f,a2f);_TmpB.f2=_TmpC;});_TmpB;});}}}case 8: _Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 845
struct _tuple13 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{void*a1t=_Tmp7;void*a1f=_Tmp6;
struct _tuple13 _Tmp8=Cyc_Vcgen_vcgen_test(env,e2,a1f);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{void*a2t=_TmpA;void*a2f=_Tmp9;
return({struct _tuple13 _TmpB;({void*_TmpC=Cyc_AssnDef_or(a1t,a2t);_TmpB.f1=_TmpC;}),_TmpB.f2=a2f;_TmpB;});}}}case 9: _Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 849
struct _tuple13 _Tmp5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_Tmp6;_Tmp6=_Tmp5.f2;{void*ain=_Tmp6;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1==Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){_Tmp4=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;{struct Cyc_Absyn_Exp*e1=_Tmp4;
# 852
struct _tuple13 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{void*at=_Tmp7;void*af=_Tmp6;
return({struct _tuple13 _Tmp8;_Tmp8.f1=af,_Tmp8.f2=at;_Tmp8;});}}}else{goto _LLD;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl)->tl==0){_Tmp1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;_Tmp3=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->tl->hd;{enum Cyc_Absyn_Primop p=_Tmp1;struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 855
struct _tuple13 _Tmp5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{void*t1=_Tmp7;void*a1=_Tmp6;
struct _tuple13 _Tmp8=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{void*t2=_TmpA;void*a2=_Tmp9;
void*at=a2;
void*af=a2;
switch((int)p){case Cyc_Absyn_Eq:
# 861
 at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_eq(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_neq(t1,t2));});goto _LL30;case Cyc_Absyn_Neq:
# 863
 at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_neq(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_eq(t1,t2));});goto _LL30;case Cyc_Absyn_Lt:
# 865
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ult(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ulte(t2,t1));});}else{
# 868
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slt(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slte(t2,t1));});}
# 870
goto _LL30;case Cyc_Absyn_Lte:
# 872
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ulte(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ult(t2,t1));});}else{
# 875
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slte(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slt(t2,t1));});}
# 877
goto _LL30;case Cyc_Absyn_Gt:
# 879
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ult(t2,t1));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ulte(t1,t2));});}else{
# 882
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slt(t2,t1));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slte(t1,t2));});}
# 884
goto _LL30;case Cyc_Absyn_Gte:
# 886
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ulte(t2,t1));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ult(t1,t2));});}else{
# 889
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slte(t2,t1));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slt(t1,t2));});}
# 891
goto _LL30;default:
 goto _LL30;}_LL30:;
# 894
return({struct _tuple13 _TmpB;_TmpB.f1=at,_TmpB.f2=af;_TmpB;});}}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}default: _LLD: {
# 896
struct _tuple13 _Tmp5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{void*t=_Tmp7;void*aout=_Tmp6;
return({struct _tuple13 _Tmp8;({void*_Tmp9=({void*_TmpA=aout;Cyc_AssnDef_and(_TmpA,({void*_TmpB=Cyc_AssnDef_one();Cyc_AssnDef_ulte(_TmpB,t);}));});_Tmp8.f1=_Tmp9;}),({void*_Tmp9=({void*_TmpA=aout;Cyc_AssnDef_and(_TmpA,({void*_TmpB=t;Cyc_AssnDef_eq(_TmpB,Cyc_AssnDef_zero());}));});_Tmp8.f2=_Tmp9;});_Tmp8;});}}};}
# 901
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
for(1;es!=0;es=es->tl){
a=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a).f2;}
return a;}struct _tuple16{struct Cyc_List_List**f1;void*f2;};
# 907
static struct _tuple16 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
struct Cyc_List_List*res=0;
int valid=1;
for(1;es!=0;es=es->tl){
struct _tuple13 _Tmp0=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t=_Tmp2;void*aout=_Tmp1;
a=aout;
if(valid && t!=0)
res=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=t,_Tmp3->tl=res;_Tmp3;});else{
# 916
valid=0;}}}
# 919
return({struct _tuple16 _Tmp0;valid?({struct Cyc_List_List**_Tmp1=({struct Cyc_List_List**_Tmp2=_cycalloc(sizeof(struct Cyc_List_List*));({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(res);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0.f1=_Tmp1;}):(_Tmp0.f1=0),_Tmp0.f2=a;_Tmp0;});}
# 922
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
struct _tuple13 _Tmp0=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*topt=_Tmp2;void*a=_Tmp1;
return({struct _tuple13 _Tmp3;topt==0?({void*_Tmp4=Cyc_AssnDef_fresh_var(e->topt);_Tmp3.f1=_Tmp4;}):(_Tmp3.f1=topt),_Tmp3.f2=a;_Tmp3;});}}
# 927
static struct _tuple14 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain){
struct Cyc_List_List*res=0;
for(1;es!=0;es=es->tl){
struct _tuple13 _Tmp0=Cyc_Vcgen_vcgen_rexp_always(env,(struct Cyc_Absyn_Exp*)es->hd,ain);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t=_Tmp2;void*a=_Tmp1;
ain=a;
res=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=t,_Tmp3->tl=res;_Tmp3;});}}
# 934
return({struct _tuple14 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(res);_Tmp0.f1=_Tmp1;}),_Tmp0.f2=ain;_Tmp0;});}
# 937
static struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){
void*_Tmp0;switch(*((int*)b)){case 1: _Tmp0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}case 2: _Tmp0=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp0;
return fd->fn_vardecl;}case 0:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp1="vcgen bind2vardecl";_tag_fat(_Tmp1,sizeof(char),19U);}),_tag_fat(0U,sizeof(void*),0));case 3: _Tmp0=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}case 4: _Tmp0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}default: _Tmp0=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}};}
# 948
static void*Cyc_Vcgen_varopt(void*b){
struct Cyc_Absyn_Vardecl*vdopt=Cyc_Vcgen_bind2vardecl(b);
if(vdopt==0)return 0;
return(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_Var_AssnDef_Term_struct));_Tmp0->tag=2,_Tmp0->f1=vdopt;_Tmp0;});}
# 954
static struct _tuple13 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp1;
return({struct _tuple13 _Tmp2;({void*_Tmp3=Cyc_Vcgen_varopt(b);_Tmp2.f1=_Tmp3;}),_Tmp2.f2=ain;_Tmp2;});}}else{
return({struct _tuple13 _Tmp2;_Tmp2.f1=0,({void*_Tmp3=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain).f2;_Tmp2.f2=_Tmp3;});_Tmp2;});};}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*f4;struct Cyc_Absyn_Vardecl*f5;};
# 963
static struct _tuple17 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){
return({struct _tuple17 _Tmp0;_Tmp0.f1=fi->args,_Tmp0.f2=fi->arg_vardecls,_Tmp0.f3=fi->requires_clause,_Tmp0.f4=fi->ensures_clause,_Tmp0.f5=fi->return_value;_Tmp0;});}
# 967
static struct _tuple17 Cyc_Vcgen_get_requires_and_ensures(void*topt){
if(topt!=0){
void*t=Cyc_Tcutil_pointer_elt_type(topt);
void*_Tmp0;if(*((int*)t)==5){_Tmp0=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo*fi=(struct Cyc_Absyn_FnInfo*)_Tmp0;
return Cyc_Vcgen_get_requires_and_ensures_info(fi);}}else{
goto _LL0;}_LL0:;}
# 975
return({struct _tuple17 _Tmp0;_Tmp0.f1=0,_Tmp0.f2=0,_Tmp0.f3=0,_Tmp0.f4=0,_Tmp0.f5=0;_Tmp0;});}
# 978
static void Cyc_Vcgen_check_requires(unsigned loc,struct Cyc_Vcgen_Env*env,struct Cyc_List_List*inputs,struct Cyc_List_List*argvds,void*a,struct Cyc_Absyn_Exp*reqopt){
# 982
if(reqopt!=0){
struct Cyc_Absyn_Exp*req=reqopt;
# 986
for(1;argvds!=0 && inputs!=0;(argvds=argvds->tl,inputs=inputs->tl)){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)argvds->hd;
a=({void*_Tmp0=a;Cyc_AssnDef_and(_Tmp0,({void*_Tmp1=(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_Var_AssnDef_Term_struct));_Tmp2->tag=2,_Tmp2->f1=vd;_Tmp2;});Cyc_AssnDef_eq(_Tmp1,(void*)inputs->hd);}));});}{
# 991
struct _tuple13 _Tmp0=Cyc_Vcgen_vcgen_test(env,reqopt,(void*)& Cyc_AssnDef_true_assn);void*_Tmp1;_Tmp1=_Tmp0.f1;{void*at=_Tmp1;
if(!Cyc_Vcgen_implies(& a,& at))
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({
struct _fat_ptr _Tmp4=Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at));_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(a));_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err(loc,({const char*_Tmp5="cannot prove @requires %s from %s";_tag_fat(_Tmp5,sizeof(char),34U);}),_tag_fat(_Tmp4,sizeof(void*),2));});}}}}
# 998
static void*Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,void*ain){
return lt;}
# 1002
static int Cyc_Vcgen_is_unsigned_int_exp(struct Cyc_Absyn_Exp*e){
void*t=e->topt;
if(t==0)return 0;{
enum Cyc_Absyn_Size_of _Tmp0;if(t!=0){if(*((int*)t)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1==Cyc_Absyn_Unsigned){_Tmp0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Size_of sz=_Tmp0;
# 1008
if((int)sz==3 ||(int)sz==2)
return 1;else{
return 0;}}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1015
static struct _tuple13 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
if(Cyc_Tcutil_is_const_exp(e)){
# 1018
{void*_Tmp0=e->r;struct _fat_ptr _Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr s=_Tmp1;
# 1020
if(Cyc_strcmp(s,({const char*_Tmp2="print_assn";_tag_fat(_Tmp2,sizeof(char),11U);}))==0){
struct _fat_ptr seg_str=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=seg_str;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain));_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="%s: current assn is %s\n";_tag_fat(_Tmp5,sizeof(char),24U);}),_tag_fat(_Tmp4,sizeof(void*),2));});}
# 1024
if(Cyc_strcmp(s,({const char*_Tmp2="all_paths";_tag_fat(_Tmp2,sizeof(char),10U);}))==0)
env->shared->widen_paths=0;
# 1027
if(Cyc_strcmp(s,({const char*_Tmp2="one_path";_tag_fat(_Tmp2,sizeof(char),9U);}))==0)
env->shared->widen_paths=1;
# 1030
return({struct _tuple13 _Tmp2;({void*_Tmp3=Cyc_AssnDef_zero();_Tmp2.f1=_Tmp3;}),_Tmp2.f2=ain;_Tmp2;});}}else{
goto _LL0;}_LL0:;}{
# 1033
struct _tuple12 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{unsigned cn=_Tmp2;int known=_Tmp1;
if(known)return({struct _tuple13 _Tmp3;({void*_Tmp4=(void*)({struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_AssnDef_Uint_AssnDef_Term_struct));_Tmp5->tag=0,_Tmp5->f1=cn;_Tmp5;});_Tmp3.f1=_Tmp4;}),_Tmp3.f2=ain;_Tmp3;});}}}{
# 1036
void*_Tmp0=e->r;struct _fat_ptr _Tmp1;struct Cyc_Absyn_MallocInfo _Tmp2;enum Cyc_Absyn_Coercion _Tmp3;enum Cyc_Absyn_Incrementor _Tmp4;void*_Tmp5;void*_Tmp6;enum Cyc_Absyn_Primop _Tmp7;void*_Tmp8;union Cyc_Absyn_Cnst _Tmp9;switch(*((int*)_Tmp0)){case 0: _Tmp9=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst c=_Tmp9;
# 1039
return({struct _tuple13 _TmpA;({void*_TmpB=(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_TmpC=_cycalloc(sizeof(struct Cyc_AssnDef_Const_AssnDef_Term_struct));_TmpC->tag=1,_TmpC->f1=e;_TmpC;});_TmpA.f1=_TmpB;}),_TmpA.f2=ain;_TmpA;});}case 1: _Tmp8=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp8;
return({struct _tuple13 _TmpA;({void*_TmpB=Cyc_Vcgen_varopt(b);_TmpA.f1=_TmpB;}),_TmpA.f2=ain;_TmpA;});}case 3: _Tmp7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp7;struct Cyc_List_List*es=_Tmp8;
# 1042
struct _tuple14 _TmpA=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{struct Cyc_List_List*ts=_TmpC;void*aout=_TmpB;
void*t=0;
if(ts!=0)t=Cyc_AssnDef_primop(p,ts,e->topt);
# 1047
if((int)p==4 && Cyc_Vcgen_is_unsigned_int_exp(e)){
# 1049
void*_TmpD;void*_TmpE;if(ts!=0){if(((struct Cyc_List_List*)ts)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)ts)->tl)->tl==0){_TmpE=(void*)ts->hd;_TmpD=(void*)ts->tl->hd;{void*t1=_TmpE;void*t2=_TmpD;
# 1051
aout=({void*_TmpF=aout;Cyc_AssnDef_and(_TmpF,Cyc_AssnDef_ult(t,t2));});
goto _LL62;}}else{goto _LL65;}}else{goto _LL65;}}else{_LL65:
 goto _LL62;}_LL62:;}
# 1056
return({struct _tuple13 _TmpD;_TmpD.f1=t,_TmpD.f2=aout;_TmpD;});}}case 4: _Tmp8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Core_Opt*popt=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 1058
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_lexp(env,e1,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*lt=_TmpC;void*a1=_TmpB;
struct _tuple13 _TmpD=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);void*_TmpE;void*_TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{void*t2=_TmpF;void*a2=_TmpE;
if(popt!=0){
void*t1=Cyc_Vcgen_deref_lterm(env,lt,a2);
if(t1==0 || t2==0)
t2=0;else{
# 1065
t2=({enum Cyc_Absyn_Primop _Tmp10=(enum Cyc_Absyn_Primop)popt->v;struct Cyc_List_List*_Tmp11=({void*_Tmp12[2];_Tmp12[0]=t1,_Tmp12[1]=t2;Cyc_List_list(_tag_fat(_Tmp12,sizeof(void*),2));});Cyc_AssnDef_primop(_Tmp10,_Tmp11,e->topt);});}}{
# 1067
void*a=Cyc_Vcgen_do_assign(env,a2,lt,t2);
return({struct _tuple13 _Tmp10;_Tmp10.f1=lt,_Tmp10.f2=a;_Tmp10;});}}}}case 5: _Tmp8=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp8;enum Cyc_Absyn_Incrementor i=_Tmp4;
# 1070
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_lexp(env,e,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*lt=_TmpC;void*a1=_TmpB;
void*t1=Cyc_Vcgen_deref_lterm(env,lt,a1);
void*res=0;
void*assn=0;
switch((int)i){case Cyc_Absyn_PreInc:
# 1076
 if(t1!=0)assn=({struct Cyc_List_List*_TmpD=({void*_TmpE[2];_TmpE[0]=t1,({void*_TmpF=Cyc_AssnDef_one();_TmpE[1]=_TmpF;});Cyc_List_list(_tag_fat(_TmpE,sizeof(void*),2));});Cyc_AssnDef_primop(0U,_TmpD,e->topt);});
res=assn;
goto _LL70;case Cyc_Absyn_PostInc:
# 1080
 res=t1;
if(t1!=0)assn=({struct Cyc_List_List*_TmpD=({void*_TmpE[2];_TmpE[0]=t1,({void*_TmpF=Cyc_AssnDef_one();_TmpE[1]=_TmpF;});Cyc_List_list(_tag_fat(_TmpE,sizeof(void*),2));});Cyc_AssnDef_primop(0U,_TmpD,e->topt);});
goto _LL70;case Cyc_Absyn_PreDec:
# 1084
 if(t1!=0)assn=({struct Cyc_List_List*_TmpD=({void*_TmpE[2];_TmpE[0]=t1,({void*_TmpF=Cyc_AssnDef_one();_TmpE[1]=_TmpF;});Cyc_List_list(_tag_fat(_TmpE,sizeof(void*),2));});Cyc_AssnDef_primop(2U,_TmpD,e->topt);});
res=assn;
goto _LL70;case Cyc_Absyn_PostDec:
# 1088
 res=t1;
if(t1!=0)assn=({struct Cyc_List_List*_TmpD=({void*_TmpE[2];_TmpE[0]=t1,({void*_TmpF=Cyc_AssnDef_one();_TmpE[1]=_TmpF;});Cyc_List_list(_tag_fat(_TmpE,sizeof(void*),2));});Cyc_AssnDef_primop(2U,_TmpD,e->topt);});
goto _LL70;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpD="vcgen vcgen_rexp increment_e";_tag_fat(_TmpD,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));}_LL70:;{
# 1093
void*a=Cyc_Vcgen_do_assign(env,a1,lt,assn);
return({struct _tuple13 _TmpD;_TmpD.f1=res,_TmpD.f2=a;_TmpD;});}}}case 6: _Tmp8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;struct Cyc_Absyn_Exp*e3=_Tmp5;
# 1096
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*at=_TmpC;void*af=_TmpB;
struct _tuple13 _TmpD=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);void*_TmpE;void*_TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{void*t1=_TmpF;void*at=_TmpE;
struct _tuple13 _Tmp10=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,af);void*_Tmp11;void*_Tmp12;_Tmp12=_Tmp10.f1;_Tmp11=_Tmp10.f2;{void*t2=_Tmp12;void*af=_Tmp11;
if(t1!=0 && t2!=0){
void*v=Cyc_AssnDef_fresh_var(e->topt);
at=({void*_Tmp13=at;Cyc_AssnDef_and(_Tmp13,Cyc_AssnDef_eq(v,t1));});
af=({void*_Tmp13=af;Cyc_AssnDef_and(_Tmp13,Cyc_AssnDef_eq(v,t2));});
return({struct _tuple13 _Tmp13;_Tmp13.f1=v,({void*_Tmp14=({struct Cyc_Vcgen_Env*_Tmp15=env;Cyc_Vcgen_may_widen(_Tmp15,Cyc_AssnDef_or(at,af));});_Tmp13.f2=_Tmp14;});_Tmp13;});}else{
# 1105
return({struct _tuple13 _Tmp13;_Tmp13.f1=0,({void*_Tmp14=({struct Cyc_Vcgen_Env*_Tmp15=env;Cyc_Vcgen_may_widen(_Tmp15,Cyc_AssnDef_or(at,af));});_Tmp13.f2=_Tmp14;});_Tmp13;});}}}}}case 7: _Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1107
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*at=_TmpC;void*af=_TmpB;
struct _tuple13 _TmpD=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);void*_TmpE;void*_TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{void*t=_TmpF;void*at=_TmpE;
void*v=Cyc_AssnDef_fresh_var(e->topt);
at=({void*_Tmp10=at;Cyc_AssnDef_and(_Tmp10,Cyc_AssnDef_eq(v,t));});
af=({void*_Tmp10=af;Cyc_AssnDef_and(_Tmp10,({void*_Tmp11=v;Cyc_AssnDef_eq(_Tmp11,Cyc_AssnDef_zero());}));});
return({struct _tuple13 _Tmp10;_Tmp10.f1=v,({void*_Tmp11=({struct Cyc_Vcgen_Env*_Tmp12=env;Cyc_Vcgen_may_widen(_Tmp12,Cyc_AssnDef_or(at,af));});_Tmp10.f2=_Tmp11;});_Tmp10;});}}}case 8: _Tmp8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1114
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*at=_TmpC;void*af=_TmpB;
struct _tuple13 _TmpD=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,af);void*_TmpE;void*_TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{void*t=_TmpF;void*af=_TmpE;
void*v=Cyc_AssnDef_fresh_var(e->topt);
at=({void*_Tmp10=at;Cyc_AssnDef_and(_Tmp10,({void*_Tmp11=v;Cyc_AssnDef_eq(_Tmp11,Cyc_AssnDef_one());}));});
af=({void*_Tmp10=af;Cyc_AssnDef_and(_Tmp10,Cyc_AssnDef_eq(v,t));});
return({struct _tuple13 _Tmp10;_Tmp10.f1=v,({void*_Tmp11=({struct Cyc_Vcgen_Env*_Tmp12=env;Cyc_Vcgen_may_widen(_Tmp12,Cyc_AssnDef_or(at,af));});_Tmp10.f2=_Tmp11;});_Tmp10;});}}}case 9: _Tmp8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1121
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_TmpB;_TmpB=_TmpA.f2;{void*a=_TmpB;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a);}}case 10: _Tmp8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_List_List*es=_Tmp6;
# 1124
struct _tuple14 _TmpA=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{struct Cyc_List_List*ts=_TmpC;void*a=_TmpB;
struct _tuple13 _TmpD=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,a);void*_TmpE;_TmpE=_TmpD.f2;{void*a2=_TmpE;
# 1127
struct _tuple17 _TmpF=Cyc_Vcgen_get_requires_and_ensures(e1->topt);void*_Tmp10;void*_Tmp11;void*_Tmp12;void*_Tmp13;void*_Tmp14;_Tmp14=_TmpF.f1;_Tmp13=_TmpF.f2;_Tmp12=_TmpF.f3;_Tmp11=_TmpF.f4;_Tmp10=_TmpF.f5;{struct Cyc_List_List*args=_Tmp14;struct Cyc_List_List*argvds=_Tmp13;struct Cyc_Absyn_Exp*reqopt=_Tmp12;struct Cyc_Absyn_Exp*ensopt=_Tmp11;struct Cyc_Absyn_Vardecl*ret_value=_Tmp10;
Cyc_Vcgen_check_requires(e->loc,env,ts,argvds,a2,reqopt);
# 1131
a2=Cyc_AssnDef_kill(a2);
Cyc_Vcgen_update_try_assn(env,a2);
# 1134
if(Cyc_Atts_is_noreturn_fn_type(_check_null(e1->topt)))a2=(void*)& Cyc_AssnDef_false_assn;
if(ensopt==0)
return({struct _tuple13 _Tmp15;_Tmp15.f1=0,_Tmp15.f2=a2;_Tmp15;});{
struct Cyc_Absyn_Exp*ensures=ensopt;
# 1139
struct _tuple13 _Tmp15=Cyc_Vcgen_vcgen_test(env,ensures,a2);void*_Tmp16;_Tmp16=_Tmp15.f1;{void*at=_Tmp16;
# 1141
for(1;args!=0 && ts!=0;(args=args->tl,ts=ts->tl)){
struct _tuple9*_Tmp17=(struct _tuple9*)args->hd;void*_Tmp18;_Tmp18=_Tmp17->f1;{struct _fat_ptr*vdopt=_Tmp18;
if(vdopt!=0){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)_check_null(argvds)->hd;
argvds=argvds->tl;
at=Cyc_AssnDef_subst(vd,(void*)ts->hd,at);}}}
# 1150
if(ret_value!=0){
void*v=Cyc_AssnDef_fresh_var(ret_value->type);
# 1153
at=Cyc_AssnDef_subst(ret_value,v,at);
return({struct _tuple13 _Tmp17;_Tmp17.f1=v,_Tmp17.f2=at;_Tmp17;});}
# 1156
return({struct _tuple13 _Tmp17;_Tmp17.f1=0,_Tmp17.f2=at;_Tmp17;});}}}}}}case 11: _Tmp8=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 1158
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);void*_TmpB;_TmpB=_TmpA.f2;{void*a=_TmpB;
Cyc_Vcgen_update_try_assn(env,a);
return({struct _tuple13 _TmpC;_TmpC.f1=0,_TmpC.f2=(void*)& Cyc_AssnDef_false_assn;_TmpC;});}}case 12: _Tmp8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 13: _Tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 14: _Tmp8=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*tp=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp6;enum Cyc_Absyn_Coercion c=_Tmp3;
# 1166
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*topt=_TmpC;void*a=_TmpB;
# 1174
{struct _tuple13 _TmpD=({struct _tuple13 _TmpE;({void*_TmpF=Cyc_Absyn_compress(tp);_TmpE.f1=_TmpF;}),({void*_TmpF=Cyc_Absyn_compress(_check_null(e->topt));_TmpE.f2=_TmpF;});_TmpE;});enum Cyc_Absyn_Size_of _TmpE;enum Cyc_Absyn_Sign _TmpF;enum Cyc_Absyn_Size_of _Tmp10;enum Cyc_Absyn_Sign _Tmp11;if(*((int*)_TmpD.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpD.f1)->f1)==1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpD.f1)->f1)->f2){case Cyc_Absyn_Int_sz: if(*((int*)_TmpD.f2)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpD.f2)->f1)){case 4:
# 1178
 return({struct _tuple13 _Tmp12;_Tmp12.f1=topt,_Tmp12.f2=a;_Tmp12;});case 1: goto _LLAD;default: goto _LLAF;}else{goto _LLAF;}case Cyc_Absyn_Long_sz: if(*((int*)_TmpD.f2)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpD.f2)->f1)){case 4:
# 1180
 return({struct _tuple13 _Tmp12;_Tmp12.f1=topt,_Tmp12.f2=a;_Tmp12;});case 1: goto _LLAD;default: goto _LLAF;}else{goto _LLAF;}default: if(*((int*)_TmpD.f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpD.f2)->f1)==1){_LLAD: _Tmp11=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpD.f1)->f1)->f1;_Tmp10=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpD.f1)->f1)->f2;_TmpF=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpD.f2)->f1)->f1;_TmpE=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpD.f2)->f1)->f2;{enum Cyc_Absyn_Sign u1=_Tmp11;enum Cyc_Absyn_Size_of s1=_Tmp10;enum Cyc_Absyn_Sign u2=_TmpF;enum Cyc_Absyn_Size_of s2=_TmpE;
# 1186
s1=(int)s1==3?2U: s1;
s2=(int)s2==3?2U: s2;
if((int)s1==(int)s2)return({struct _tuple13 _Tmp12;_Tmp12.f1=topt,_Tmp12.f2=a;_Tmp12;});
goto _LLA8;}}else{goto _LLAF;}}else{goto _LLAF;}}else{goto _LLAF;}}else{_LLAF:
 goto _LLA8;}_LLA8:;}
# 1192
if((int)c==1){
if(topt==0)return({struct _tuple13 _TmpD;_TmpD.f1=0,_TmpD.f2=a;_TmpD;});{
void*t=topt;
return({struct _tuple13 _TmpD;({void*_TmpE=Cyc_AssnDef_cast(tp,t);_TmpD.f1=_TmpE;}),_TmpD.f2=a;_TmpD;});}}else{
# 1198
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple13 _TmpD;topt==0?_TmpD.f1=0:({void*_TmpE=Cyc_AssnDef_cast(tp,topt);_TmpD.f1=_TmpE;}),_TmpD.f2=a;_TmpD;});}}}case 16: _Tmp8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*eopt=_Tmp8;struct Cyc_Absyn_Exp*e=_Tmp6;
# 1202
if(eopt!=0)
ain=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,ain).f2;
return({struct _tuple13 _TmpA;_TmpA.f1=0,({void*_TmpB=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain).f2;_TmpA.f2=_TmpB;});_TmpA;});}case 32:
# 1207
 goto _LL2A;case 33: _LL2A:
 goto _LL2C;case 17: _LL2C:
 goto _LL2E;case 18: _LL2E:
 goto _LL30;case 39: _LL30:
 goto _LL32;case 19: _LL32:
 return({struct _tuple13 _TmpA;({void*_TmpB=(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_TmpC=_cycalloc(sizeof(struct Cyc_AssnDef_Const_AssnDef_Term_struct));_TmpC->tag=1,_TmpC->f1=e;_TmpC;});_TmpA.f1=_TmpB;}),_TmpA.f2=ain;_TmpA;});case 21: _Tmp8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 1215
_Tmp8=e;goto _LL36;
ain=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain).f2;
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple13 _TmpA;_TmpA.f1=0,_TmpA.f2=ain;_TmpA;});}case 38: _Tmp8=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL36: {struct Cyc_Absyn_Exp*e=_Tmp8;
# 1220
_Tmp8=e;goto _LL38;}case 15: _Tmp8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL38: {struct Cyc_Absyn_Exp*e=_Tmp8;
# 1222
return({struct _tuple13 _TmpA;_TmpA.f1=0,({void*_TmpB=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain).f2;_TmpA.f2=_TmpB;});_TmpA;});}case 20: _Tmp8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 1224
_Tmp8=e;goto _LL3C;}case 22: _Tmp8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_Tmp8;
_Tmp8=e;_Tmp6=Cyc_Vcgen_zero_exp();goto _LL3E;}case 23: _Tmp8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL3E: {struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1227
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*t1=_TmpC;void*a1=_TmpB;
struct _tuple13 _TmpD=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);void*_TmpE;void*_TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{void*t2=_TmpF;void*a2=_TmpE;
{void*_Tmp10=Cyc_Absyn_compress(_check_null(e1->topt));if(*((int*)_Tmp10)==6)
goto _LLB7;else{
# 1232
Cyc_Vcgen_update_try_assn(env,a2);{
void*term_numelts;void*term_typebound;
# 1235
if(t2==0)goto _LLB7;
term_numelts=({struct Cyc_List_List*_Tmp11=({void*_Tmp12[1];_Tmp12[0]=t1;Cyc_List_list(_tag_fat(_Tmp12,sizeof(void*),1));});Cyc_AssnDef_primop(18U,_Tmp11,Cyc_Absyn_uint_type);});{
void*inbound_assn=Cyc_AssnDef_ult(t2,term_numelts);
({struct Cyc_Hashtable_Table**_Tmp11=env->assn_info;struct Cyc_Absyn_Exp*_Tmp12=e1;Cyc_Vcgen_insert_assn_info(_Tmp11,_Tmp12,({struct _tuple13*_Tmp13=_cycalloc(sizeof(struct _tuple13));_Tmp13->f1=a2,_Tmp13->f2=inbound_assn;_Tmp13;}));});
a2=Cyc_AssnDef_and(a2,inbound_assn);}}}_LLB7:;}
# 1241
return({struct _tuple13 _Tmp10;_Tmp10.f1=0,_Tmp10.f2=a2;_Tmp10;});}}}case 31: _Tmp8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp8;
# 1243
_Tmp8=es;goto _LL42;}case 24: _Tmp8=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL42: {struct Cyc_List_List*es=_Tmp8;
return({struct _tuple13 _TmpA;_TmpA.f1=0,({void*_TmpB=Cyc_Vcgen_vcgen_rexps_nosave(env,es,ain);_TmpA.f2=_TmpB;});_TmpA;});}case 25: _Tmp8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*dles=_Tmp8;
# 1246
_Tmp8=dles;goto _LL46;}case 26: _Tmp8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL46: {struct Cyc_List_List*dles=_Tmp8;
_Tmp8=dles;goto _LL48;}case 29: _Tmp8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL48: {struct Cyc_List_List*dles=_Tmp8;
_Tmp8=dles;goto _LL4A;}case 36: _Tmp8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL4A: {struct Cyc_List_List*dles=_Tmp8;
_Tmp8=dles;goto _LL4C;}case 30: _Tmp8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL4C: {struct Cyc_List_List*dles=_Tmp8;
# 1251
for(1;dles!=0;dles=dles->tl){
struct _tuple18*_TmpA=(struct _tuple18*)dles->hd;void*_TmpB;_TmpB=_TmpA->f2;{struct Cyc_Absyn_Exp*e=_TmpB;
ain=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain).f2;}}
# 1255
return({struct _tuple13 _TmpA;_TmpA.f1=0,_TmpA.f2=ain;_TmpA;});}case 27: _Tmp8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 1259
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*t1=_TmpC;void*a=_TmpB;
struct Cyc_AssnDef_Var_AssnDef_Term_struct*v;v=_cycalloc(sizeof(struct Cyc_AssnDef_Var_AssnDef_Term_struct)),v->tag=2,v->f1=vd;
# 1262
a=({struct Cyc_Vcgen_Env*_TmpD=env;void*_TmpE=a;void*_TmpF=(void*)v;Cyc_Vcgen_do_assign(_TmpD,_TmpE,_TmpF,Cyc_AssnDef_zero());});
while(1){
void*at=({void*_TmpD=a;Cyc_AssnDef_and(_TmpD,Cyc_AssnDef_ult((void*)v,t1));});
void*af=({void*_TmpD=a;Cyc_AssnDef_and(_TmpD,Cyc_AssnDef_ulte(t1,(void*)v));});
struct _tuple13 _TmpD=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);void*_TmpE;_TmpE=_TmpD.f2;{void*abody=_TmpE;
abody=({struct Cyc_Vcgen_Env*_TmpF=env;void*_Tmp10=abody;void*_Tmp11=(void*)v;Cyc_Vcgen_do_assign(_TmpF,_Tmp10,_Tmp11,({struct Cyc_List_List*_Tmp12=({void*_Tmp13[2];_Tmp13[0]=(void*)v,({void*_Tmp14=Cyc_AssnDef_one();_Tmp13[1]=_Tmp14;});Cyc_List_list(_tag_fat(_Tmp13,sizeof(void*),2));});Cyc_AssnDef_primop(0U,_Tmp12,vd->type);}));});
if(Cyc_Vcgen_simple_implies(& abody,& a)){a=af;break;}
# 1270
a=Cyc_Vcgen_widen(Cyc_AssnDef_or(abody,a));}}
# 1272
return({struct _tuple13 _TmpD;_TmpD.f1=0,_TmpD.f2=a;_TmpD;});}}case 28: _Tmp8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 1274
return({struct _tuple13 _TmpA;_TmpA.f1=0,({void*_TmpB=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain).f2;_TmpA.f2=_TmpB;});_TmpA;});}case 34: _Tmp2=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_MallocInfo mi=_Tmp2;
# 1276
if(mi.rgn!=0)
ain=Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.rgn,ain).f2;
return({struct _tuple13 _TmpA;_TmpA.f1=0,({void*_TmpB=Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.num_elts,ain).f2;_TmpA.f2=_TmpB;});_TmpA;});}case 35: _Tmp8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1280
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_lexp(env,e1,ain);void*_TmpB;void*_TmpC;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*lt1=_TmpC;void*a1=_TmpB;
Cyc_Vcgen_update_try_assn(env,a1);{
struct _tuple13 _TmpD=Cyc_Vcgen_vcgen_lexp(env,e2,a1);void*_TmpE;void*_TmpF;_TmpF=_TmpD.f1;_TmpE=_TmpD.f2;{void*lt2=_TmpF;void*a2=_TmpE;
Cyc_Vcgen_update_try_assn(env,a2);{
void*t1=Cyc_Vcgen_deref_lterm(env,lt1,a2);
void*t2=Cyc_Vcgen_deref_lterm(env,lt2,a2);
a2=Cyc_Vcgen_do_assign(env,a2,lt1,t2);
a2=Cyc_Vcgen_do_assign(env,a2,lt2,t1);
return({struct _tuple13 _Tmp10;_Tmp10.f1=0,_Tmp10.f2=a2;_Tmp10;});}}}}}case 37: _Tmp8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp8;
# 1290
void**old_exp_stmt=env->shared->exp_stmt;
({void**_TmpA=({void**_TmpB=_cycalloc(sizeof(void*));*_TmpB=0;_TmpB;});env->shared->exp_stmt=_TmpA;});{
void*aout=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);
void*t=*_check_null(env->shared->exp_stmt);
env->shared->exp_stmt=old_exp_stmt;
return({struct _tuple13 _TmpA;_TmpA.f1=t,_TmpA.f2=aout;_TmpA;});}}case 40:
 return({struct _tuple13 _TmpA;_TmpA.f1=0,_TmpA.f2=ain;_TmpA;});case 41: _Tmp8=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 2: _Tmp1=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr s=_Tmp1;
return({struct _tuple13 _TmpA;({void*_TmpB=Cyc_AssnDef_zero();_TmpA.f1=_TmpB;}),_TmpA.f2=ain;_TmpA;});}default: _Tmp8=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
# 1300
struct _tuple13 _TmpA=Cyc_Vcgen_vcgen_test(env,e1,(void*)& Cyc_AssnDef_true_assn);void*_TmpB;_TmpB=_TmpA.f1;{void*at=_TmpB;
if(!Cyc_Vcgen_implies(& ain,& at))
({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,({
struct _fat_ptr _TmpE=Cyc_AssnDef_assn2string(at);_TmpD.f1=_TmpE;});_TmpD;});struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,({struct _fat_ptr _TmpF=Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain));_TmpE.f1=_TmpF;});_TmpE;});void*_TmpE[2];_TmpE[0]=& _TmpC,_TmpE[1]=& _TmpD;Cyc_Warn_err(e->loc,({const char*_TmpF="cannot prove @assert(%s) \nfrom %s";_tag_fat(_TmpF,sizeof(char),34U);}),_tag_fat(_TmpE,sizeof(void*),2));});
return({struct _tuple13 _TmpC;({void*_TmpD=Cyc_AssnDef_one();_TmpC.f1=_TmpD;}),({void*_TmpD=Cyc_AssnDef_and(ain,at);_TmpC.f2=_TmpD;});_TmpC;});}}};}}
# 1310
void Cyc_Vcgen_vcgen_fundecl(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info){
void*a=(void*)& Cyc_AssnDef_true_assn;
# 1313
struct _tuple17 _Tmp0=Cyc_Vcgen_get_requires_and_ensures_info(& fd->i);void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp0.f1;_Tmp4=_Tmp0.f2;_Tmp3=_Tmp0.f3;_Tmp2=_Tmp0.f4;_Tmp1=_Tmp0.f5;{struct Cyc_List_List*args=_Tmp5;struct Cyc_List_List*arvds=_Tmp4;struct Cyc_Absyn_Exp*reqopt=_Tmp3;struct Cyc_Absyn_Exp*ensopt=_Tmp2;struct Cyc_Absyn_Vardecl*ret_value=_Tmp1;
void**result_assn=ensopt==0?0:({void**_Tmp6=_cycalloc(sizeof(void*));*_Tmp6=(void*)& Cyc_AssnDef_false_assn;_Tmp6;});
void*res_term=ret_value==0?0:(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Var_AssnDef_Term_struct));_Tmp6->tag=2,_Tmp6->f1=ret_value;_Tmp6;});
struct Cyc_Vcgen_Env*env=Cyc_Vcgen_initial_env(tables,fd,assn_info,result_assn,res_term);
if(reqopt!=0)
# 1319
a=Cyc_Vcgen_vcgen_test(env,reqopt,a).f1;
# 1321
Cyc_Vcgen_vcgen_stmt_nodebug(env,fd->body,a);
# 1323
if(ensopt!=0){
struct Cyc_Absyn_Exp*ens=ensopt;
struct _tuple13 _Tmp6=Cyc_Vcgen_vcgen_test(env,ens,(void*)& Cyc_AssnDef_true_assn);void*_Tmp7;_Tmp7=_Tmp6.f1;{void*at=_Tmp7;
struct Cyc_Vcgen_SharedEnv*senv=env->shared;
void*res_assn_opt=*_check_null(senv->res_assn);
void*res_assn=(void*)& Cyc_AssnDef_false_assn;
if(res_assn_opt!=0)res_assn=res_assn_opt;
if(!Cyc_Vcgen_implies(& res_assn,& at))
({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({
struct _fat_ptr _TmpA=Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at));_Tmp9.f1=_TmpA;});_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(res_assn));_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_Warn_err(ens->loc,({const char*_TmpB="cannot prove the @ensures clause %s from %s";_tag_fat(_TmpB,sizeof(char),44U);}),_tag_fat(_TmpA,sizeof(void*),2));});}}
# 1334
Cyc_AssnDef_reset_hash_cons_table();}}
