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
 struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 171 "absyn.h"
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 469 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 494
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};
# 874 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 890
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 892
extern void*Cyc_Absyn_uint_type;
# 982
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 74
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;void*f3;};
# 62 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 64
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
void*Cyc_AssnDef_zero (void);
# 71
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*,void*);
# 75
int Cyc_AssnDef_cmp_term(void*,void*);
void*Cyc_AssnDef_get_term_type(void*);
# 80
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 97
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 117 "assndef.h"
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
# 121
void*Cyc_AssnDef_prim(void*,enum Cyc_AssnDef_Primreln,void*);
# 124
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 130
void*Cyc_AssnDef_reduce(void*);
# 132
void*Cyc_AssnDef_not(void*);
# 28 "pratt_prover.cyc"
enum Cyc_PrattProver_DistanceInfo{Cyc_PrattProver_Unsigned_valid =1,Cyc_PrattProver_Signed_valid =2,Cyc_PrattProver_Both_valid =3};struct Cyc_PrattProver_Dist{int length;void*prim;};struct Cyc_PrattProver_Distance{struct Cyc_PrattProver_Distance*next;void*target;enum Cyc_PrattProver_DistanceInfo dinfo;struct Cyc_PrattProver_Dist unsigned_dist;struct Cyc_PrattProver_Dist signed_dist;};struct Cyc_PrattProver_Row{struct Cyc_PrattProver_Row*next;void*source;struct Cyc_PrattProver_Distance*distance;};struct Cyc_PrattProver_Graph{struct Cyc_PrattProver_Graph*next;struct Cyc_PrattProver_Row*rows;};
# 66 "pratt_prover.cyc"
static void Cyc_PrattProver_print_graphs(struct Cyc_PrattProver_Graph*gs){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="Graphs:\n";_tag_fat(_Tmp0,sizeof(char),9U);}),_tag_fat(0U,sizeof(void*),0));
for(1;gs!=0;gs=gs->next){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="\t{";_tag_fat(_Tmp0,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));
{struct Cyc_PrattProver_Row*ns=gs->rows;for(0;ns!=0;ns=ns->next){
void*t1=ns->source;
struct Cyc_PrattProver_Distance*ds=ns->distance;
if(ds==0)continue;
for(1;ds!=0;ds=ds->next){
void*t2=ds->target;
if((int)ds->dinfo==3)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_AssnDef_term2string(t1);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
struct _fat_ptr _Tmp3=Cyc_AssnDef_term2string(t2);_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)ds->unsigned_dist.length;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_AssnDef_term2string(t1);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=Cyc_AssnDef_term2string(t2);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)ds->signed_dist.length;_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="%s - %s U<= %d, %s - %s S<= %d";_tag_fat(_Tmp7,sizeof(char),31U);}),_tag_fat(_Tmp6,sizeof(void*),6));});else{
if((int)ds->dinfo==2)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_AssnDef_term2string(t1);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
struct _fat_ptr _Tmp3=Cyc_AssnDef_term2string(t2);_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)ds->signed_dist.length;_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="%s - %s S<= %d";_tag_fat(_Tmp4,sizeof(char),15U);}),_tag_fat(_Tmp3,sizeof(void*),3));});else{
# 84
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_AssnDef_term2string(t1);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
struct _fat_ptr _Tmp3=Cyc_AssnDef_term2string(t2);_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)ds->unsigned_dist.length;_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="%s - %s U<= %d";_tag_fat(_Tmp4,sizeof(char),15U);}),_tag_fat(_Tmp3,sizeof(void*),3));});}}
# 87
if(ds->next!=0)Cyc_fprintf(Cyc_stderr,({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}
# 89
if(ns->next!=0)Cyc_fprintf(Cyc_stderr,({const char*_Tmp0=",\n\t ";_tag_fat(_Tmp0,sizeof(char),5U);}),_tag_fat(0U,sizeof(void*),0));}}
# 91
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="}\n";_tag_fat(_Tmp0,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));}}
# 96
static unsigned Cyc_PrattProver_num_graphs(struct Cyc_PrattProver_Graph*gs){
unsigned n=0U;
for(1;gs!=0;gs=gs->next){
++ n;}
return n;}
# 105
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_true_graph (void){return({struct Cyc_PrattProver_Graph*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Graph));_Tmp0->next=0,_Tmp0->rows=0;_Tmp0;});}
# 108
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_copy_distance(struct Cyc_PrattProver_Distance*ds){
struct Cyc_PrattProver_Distance*res=0;
for(1;ds!=0;ds=ds->next){
struct Cyc_PrattProver_Distance*newds;newds=_cycalloc(sizeof(struct Cyc_PrattProver_Distance)),*newds=*ds;
newds->next=res;
res=newds;}
# 115
return res;}
# 118
static struct Cyc_PrattProver_Row*Cyc_PrattProver_copy_rows(struct Cyc_PrattProver_Row*ns){
struct Cyc_PrattProver_Row*res=0;
for(1;ns!=0;ns=ns->next){
res=({struct Cyc_PrattProver_Row*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Row));_Tmp0->next=res,_Tmp0->source=ns->source,({
# 123
struct Cyc_PrattProver_Distance*_Tmp1=Cyc_PrattProver_copy_distance(ns->distance);_Tmp0->distance=_Tmp1;});_Tmp0;});}
# 125
return res;}
# 129
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_copy_graph(struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Graph*res=0;
for(1;g!=0;g=g->next){
res=({struct Cyc_PrattProver_Graph*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Graph));_Tmp0->next=res,({struct Cyc_PrattProver_Row*_Tmp1=Cyc_PrattProver_copy_rows(g->rows);_Tmp0->rows=_Tmp1;});_Tmp0;});}
# 134
return res;}
# 139
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_graph_append(struct Cyc_PrattProver_Graph*g1,struct Cyc_PrattProver_Graph*g2){
if(g1==0)return g2;
if(g2==0)return g1;{
struct Cyc_PrattProver_Graph*p=g1;
{struct Cyc_PrattProver_Graph*x=p->next;for(0;x!=0;(p=x,x=p->next)){;}}
p->next=g2;
return g1;}}
# 148
static int Cyc_PrattProver_add_edge(struct Cyc_PrattProver_Graph*,int,void*,void*,int,void*);
static int Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*,void*,enum Cyc_AssnDef_Primreln,void*);
# 151
static int Cyc_PrattProver_add_node(struct Cyc_PrattProver_Graph*,void*);
# 153
static int Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
return(((Cyc_PrattProver_add_node(g,t2)&&
 Cyc_PrattProver_add_constraint(g,t1,5U,t2))&&
 Cyc_PrattProver_add_constraint(g,t1,3U,t2))&&
 Cyc_PrattProver_add_constraint(g,t2,5U,t1))&&
 Cyc_PrattProver_add_constraint(g,t2,3U,t1);}
# 165
static int Cyc_PrattProver_add_type_info(struct Cyc_PrattProver_Graph*g,void*n){
void*topt=Cyc_AssnDef_get_term_type(n);
if(topt!=0){
void*t=topt;
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_type_bound(t);
if(eopt!=0){
void*t1=({struct Cyc_List_List*_Tmp0=({void*_Tmp1[1];_Tmp1[0]=n;Cyc_List_list(_tag_fat(_Tmp1,sizeof(void*),1));});Cyc_AssnDef_primop(18U,_Tmp0,Cyc_Absyn_uint_type);});
# 173
struct Cyc_Absyn_Exp*e=eopt;
loop: {
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==14){_Tmp2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*tp2=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 177
{void*_Tmp3=Cyc_Absyn_compress(tp2);enum Cyc_Absyn_Size_of _Tmp4;if(*((int*)_Tmp3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==1){_Tmp4=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f2;{enum Cyc_Absyn_Size_of s=_Tmp4;
# 179
if((int)s==2 ||(int)s==3){
e=e2;goto loop;}
# 182
goto _LL5;}}else{goto _LL8;}}else{_LL8:
 goto _LL5;}_LL5:;}
# 185
goto _LL0;}}else{
goto _LL0;}_LL0:;}{
# 188
void*t2=Cyc_AssnDef_cnst(e);
if(!Cyc_PrattProver_add_constraint(g,t2,5U,t1))return 0;
if(!Cyc_PrattProver_add_constraint(g,t2,3U,t1))return 0;}}{
# 192
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1==Cyc_Absyn_Unsigned){
# 194
if(!({struct Cyc_PrattProver_Graph*_Tmp2=g;void*_Tmp3=Cyc_AssnDef_zero();Cyc_PrattProver_add_constraint(_Tmp2,_Tmp3,5U,n);}))return 0;
goto _LLA;}else{goto _LLF;}case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*v=_Tmp1;
# 197
{void*_Tmp2=Cyc_Absyn_compress(v);void*_Tmp3;if(*((int*)_Tmp2)==9){_Tmp3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
({struct Cyc_PrattProver_Graph*_Tmp4=g;void*_Tmp5=n;Cyc_PrattProver_add_eq(_Tmp4,_Tmp5,Cyc_AssnDef_cnst(e));});goto _LL11;}}else{_Tmp3=_Tmp2;{void*v2=_Tmp3;
# 200
if(!({struct Cyc_PrattProver_Graph*_Tmp4=g;void*_Tmp5=n;Cyc_PrattProver_add_eq(_Tmp4,_Tmp5,Cyc_AssnDef_cnst(Cyc_Absyn_valueof_exp(v2,0U)));}))return 0;goto _LL11;}}_LL11:;}
# 202
goto _LLA;}}else{goto _LLF;}default: goto _LLF;}else{_LLF:
 goto _LLA;}_LLA:;}}
# 206
return 1;}
# 210
static int Cyc_PrattProver_add_node(struct Cyc_PrattProver_Graph*g,void*n){
{struct Cyc_PrattProver_Row*ns=g->rows;for(0;ns!=0;ns=ns->next){
if(Cyc_AssnDef_cmp_term(ns->source,n)==0)return 1;}}
# 214
({struct Cyc_PrattProver_Row*_Tmp0=({struct Cyc_PrattProver_Row*_Tmp1=_cycalloc(sizeof(struct Cyc_PrattProver_Row));_Tmp1->next=g->rows,_Tmp1->source=n,_Tmp1->distance=0;_Tmp1;});g->rows=_Tmp0;});{
void*_Tmp0;if(*((int*)n)==4){if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)n)->f1==Cyc_Absyn_Numelts){if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)n)->f2!=0){_Tmp0=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)n)->f2->hd;{void*x=_Tmp0;
# 218
return((({struct Cyc_PrattProver_Graph*_Tmp1=g;void*_Tmp2=n;Cyc_PrattProver_add_edge(_Tmp1,0,_Tmp2,Cyc_AssnDef_zero(),2147483646,0);})&&({
struct Cyc_PrattProver_Graph*_Tmp1=g;void*_Tmp2=n;Cyc_PrattProver_add_edge(_Tmp1,1,_Tmp2,Cyc_AssnDef_zero(),2147483646,0);}))&&({
struct Cyc_PrattProver_Graph*_Tmp1=g;void*_Tmp2=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_Tmp1,0,_Tmp2,n,0,0);}))&&
# 222
 Cyc_PrattProver_add_type_info(g,x);}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 224
 return Cyc_PrattProver_add_type_info(g,n);};}}
# 228
static struct Cyc_PrattProver_Row*Cyc_PrattProver_lookup_row(struct Cyc_PrattProver_Graph*g,void*i){
{struct Cyc_PrattProver_Row*ns=g->rows;for(0;ns!=0;ns=ns->next){
if(Cyc_AssnDef_cmp_term(i,ns->source)==0)return ns;}}
return 0;}
# 234
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_lookup_col(struct Cyc_PrattProver_Row*n,void*j){
{struct Cyc_PrattProver_Distance*ds=_check_null(n)->distance;for(0;ds!=0;ds=ds->next){
if(Cyc_AssnDef_cmp_term(j,ds->target)==0)return ds;}}
return 0;}
# 242
static struct Cyc_PrattProver_Dist*Cyc_PrattProver_lookup_distance(struct Cyc_PrattProver_Row*i,int is_signed,void*j){
static struct Cyc_PrattProver_Dist zero={0,0};
if(i==0)return 0;
if(Cyc_AssnDef_cmp_term(i->source,j)==0)
return& zero;{
struct Cyc_PrattProver_Distance*ds=Cyc_PrattProver_lookup_col(i,j);
if(ds!=0){
if(is_signed &&(int)ds->dinfo & 2)
return& ds->signed_dist;else{
if(!is_signed &&(int)ds->dinfo & 1)
return& ds->unsigned_dist;else{
# 254
return 0;}}}else{
# 256
return 0;}}}
# 264
static int Cyc_PrattProver_insert_distance(struct Cyc_PrattProver_Row*i,int is_signed,void*j,int dist,void*origin){
if(Cyc_AssnDef_cmp_term(_check_null(i)->source,j)==0){
# 267
if(dist!=0){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=Cyc_AssnDef_term2string(i->source);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_AssnDef_term2string(j);_Tmp2.f1=_Tmp3;});_Tmp2;});struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)dist;_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="WARNING: distance %s--0-->%s is changing to %d!\n";_tag_fat(_Tmp4,sizeof(char),49U);}),_tag_fat(_Tmp3,sizeof(void*),3));});
return 0;}
# 272
return 1;}{
# 274
struct Cyc_PrattProver_Distance*ds=Cyc_PrattProver_lookup_col(i,j);
if(ds!=0){
if(is_signed){
if((int)ds->dinfo & 2 && ds->signed_dist.length < dist){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=Cyc_AssnDef_term2string(i->source);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)ds->signed_dist.length;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({
struct _fat_ptr _Tmp4=Cyc_AssnDef_term2string(j);_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)dist;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="WARNING: signed distance %s--%d-->%s is growing to %d!\n";_tag_fat(_Tmp5,sizeof(char),56U);}),_tag_fat(_Tmp4,sizeof(void*),4));});
return 0;}
# 283
ds->dinfo |=2;
ds->signed_dist.length=dist;
ds->signed_dist.prim=origin;}else{
# 287
if((int)ds->dinfo & 1 && ds->unsigned_dist.length < dist){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=Cyc_AssnDef_term2string(i->source);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)ds->unsigned_dist.length;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({
struct _fat_ptr _Tmp4=Cyc_AssnDef_term2string(j);_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)dist;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="WARNING: unsigned distance %s--%d-->%s is growing to %d!\n";_tag_fat(_Tmp5,sizeof(char),58U);}),_tag_fat(_Tmp4,sizeof(void*),4));});
return 0;}
# 293
ds->dinfo |=1;
ds->unsigned_dist.length=dist;
ds->unsigned_dist.prim=origin;}
# 297
return 1;}{
# 299
struct Cyc_PrattProver_Dist d=({struct Cyc_PrattProver_Dist _Tmp0;_Tmp0.length=dist,_Tmp0.prim=origin;_Tmp0;});
({struct Cyc_PrattProver_Distance*_Tmp0=({struct Cyc_PrattProver_Distance*_Tmp1=_cycalloc(sizeof(struct Cyc_PrattProver_Distance));_Tmp1->next=i->distance,_Tmp1->target=j,is_signed?_Tmp1->dinfo=2:(_Tmp1->dinfo=1),_Tmp1->unsigned_dist=d,_Tmp1->signed_dist=d;_Tmp1;});i->distance=_Tmp0;});
return 1;}}}struct _tuple12{int f1;int f2;};
# 308
enum Cyc_PrattProver_Operator{Cyc_PrattProver_PLUS =0U,Cyc_PrattProver_MINUS =1U};
# 318
static struct _tuple12 Cyc_PrattProver_is_signed_overflow(int c1,enum Cyc_PrattProver_Operator op,int c2){
switch((int)op){case Cyc_PrattProver_PLUS:  {
# 321
int sum=c1 + c2;
if((~(c1 ^ c2)& (sum ^ c1))>> 31){
# 324
if(sum > 0)return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=-2147483648;_Tmp0;});else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=2147483647;_Tmp0;});}}else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=0,_Tmp0.f2=sum;_Tmp0;});}}case Cyc_PrattProver_MINUS:  {
# 328
int dif=c1 - c2;
if(((c1 ^ c2)& (c1 ^ dif))>> 31){
# 331
if(dif > 0)return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=-2147483648;_Tmp0;});else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=2147483647;_Tmp0;});}}else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=0,_Tmp0.f2=dif;_Tmp0;});}}default:
# 335
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp0="Pratt_Prover: bad operator in overflow check";_tag_fat(_Tmp0,sizeof(char),45U);}),_tag_fat(0U,sizeof(void*),0));};}
# 343
static struct _tuple12 Cyc_PrattProver_is_unsigned_overflow(unsigned c1,enum Cyc_PrattProver_Operator op,unsigned c2){
# 345
if(op==Cyc_PrattProver_MINUS){
# 347
int dif=(int)(c1 - c2);
if((int)(((c1 ^ c2)& ~(c1 ^ (unsigned)dif))>> 31U)){
# 350
if(dif > 0)return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=-2147483648;_Tmp0;});else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=2147483647;_Tmp0;});}}else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=0,_Tmp0.f2=dif;_Tmp0;});}}else{
# 354
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp0="Pratt_Prover: bad operator in overflow check";_tag_fat(_Tmp0,sizeof(char),45U);}),_tag_fat(0U,sizeof(void*),0));};}
# 365 "pratt_prover.cyc"
static int Cyc_PrattProver_add_edge(struct Cyc_PrattProver_Graph*g,int is_signed,void*i,void*j,int dist,void*origin){
if(!Cyc_PrattProver_add_node(g,i))return 0;
if(!Cyc_PrattProver_add_node(g,j))return 0;{
struct Cyc_PrattProver_Dist*ij_dist=({struct Cyc_PrattProver_Row*_Tmp0=Cyc_PrattProver_lookup_row(g,i);int _Tmp1=is_signed;Cyc_PrattProver_lookup_distance(_Tmp0,_Tmp1,j);});
# 370
if(ij_dist!=0 && ij_dist->length < dist)
return 1;{
struct Cyc_PrattProver_Dist*ji_dist=({struct Cyc_PrattProver_Row*_Tmp0=Cyc_PrattProver_lookup_row(g,j);int _Tmp1=is_signed;Cyc_PrattProver_lookup_distance(_Tmp0,_Tmp1,i);});
# 374
if(ji_dist!=0){
struct _tuple12 _Tmp0=Cyc_PrattProver_is_signed_overflow(ji_dist->length,0U,dist);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int overflow=_Tmp2;int sum=_Tmp1;
if(sum < 0)
# 378
return 0;else{
if(overflow && sum > 0)
# 383
return 1;}}}{
# 389
struct Cyc_PrattProver_Row*jrow=Cyc_PrattProver_lookup_row(g,j);
{struct Cyc_PrattProver_Row*ks=g->rows;for(0;ks!=0;ks=ks->next){
void*k=ks->source;
struct Cyc_PrattProver_Dist*ki_dist=Cyc_PrattProver_lookup_distance(ks,is_signed,i);
if(ki_dist==0)continue;{
struct Cyc_PrattProver_Row*ls=g->rows;for(0;ls!=0;ls=ls->next){
void*l=ls->source;
struct Cyc_PrattProver_Dist*kl_dist=Cyc_PrattProver_lookup_distance(ks,is_signed,l);
# 398
if(kl_dist!=0 && kl_dist->length==-2147483648)continue;{
# 400
struct Cyc_PrattProver_Dist*jl_dist=Cyc_PrattProver_lookup_distance(jrow,is_signed,l);
if(jl_dist==0)continue;{
# 404
int final;
int sum;
struct _tuple12 _Tmp0=Cyc_PrattProver_is_signed_overflow(ki_dist->length,0U,dist);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int of1=_Tmp2;int sum1=_Tmp1;
if(!of1){
# 409
struct _tuple12 _Tmp3=Cyc_PrattProver_is_signed_overflow(sum1,0U,jl_dist->length);int _Tmp4;int _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{int of2=_Tmp5;int sum2=_Tmp4;
final=of2;
sum=sum2;}}else{
# 415
struct _tuple12 _Tmp3=Cyc_PrattProver_is_signed_overflow(ki_dist->length,0U,jl_dist->length);int _Tmp4;int _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{int of1=_Tmp5;int sum1=_Tmp4;
if(!of1){
struct _tuple12 _Tmp6=Cyc_PrattProver_is_signed_overflow(sum1,0U,dist);int _Tmp7;int _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{int of2=_Tmp8;int sum2=_Tmp7;
final=of2;
sum=sum;}}else{
# 423
final=1;
if(dist < 0)
# 426
sum=-2147483648;else{
# 429
sum=2147483647;}}}}
# 433
if(!final || sum < 0){
# 436
if(kl_dist==0 || kl_dist->length > sum)
Cyc_PrattProver_insert_distance(ks,is_signed,l,sum,0);}}}}}}}}
# 441
return 1;}}}}
# 446
static int Cyc_PrattProver_check_graph_consistent(struct Cyc_PrattProver_Graph*g){
{struct Cyc_PrattProver_Row*i=g->rows;for(0;i!=0;i=i->next){
struct Cyc_PrattProver_Row*j=g->rows;for(0;j!=0;j=j->next){
if(i==j)continue;{
struct Cyc_PrattProver_Dist*ij_signed=Cyc_PrattProver_lookup_distance(i,1,j->source);
struct Cyc_PrattProver_Dist*ij_unsigned=Cyc_PrattProver_lookup_distance(i,0,j->source);
struct Cyc_PrattProver_Row*k=g->rows;for(0;k!=0;k=k->next){
if(i==k || j==k)continue;{
struct Cyc_PrattProver_Dist*ik_signed=Cyc_PrattProver_lookup_distance(i,1,k->source);
struct Cyc_PrattProver_Dist*ik_unsigned=Cyc_PrattProver_lookup_distance(i,0,k->source);
struct Cyc_PrattProver_Dist*kj_signed=Cyc_PrattProver_lookup_distance(k,1,j->source);
struct Cyc_PrattProver_Dist*kj_unsigned=Cyc_PrattProver_lookup_distance(k,0,j->source);
if(ik_signed!=0 && kj_signed!=0){
struct _tuple12 _Tmp0=
Cyc_PrattProver_is_signed_overflow(ik_signed->length,0U,kj_signed->length);
# 459
int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int of1=_Tmp2;int sum1=_Tmp1;
# 461
if(!of1){
if(ij_signed==0 || ij_signed->length > sum1){
if(!Cyc_PrattProver_add_edge(g,1,i->source,j->source,sum1,0))
return 0;}}}}
# 468
if(ik_unsigned!=0 && kj_unsigned!=0){
struct _tuple12 _Tmp0=
Cyc_PrattProver_is_signed_overflow(ik_unsigned->length,0U,kj_unsigned->length);
# 469
int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int of2=_Tmp2;int sum2=_Tmp1;
# 471
if(!of2){
if(ij_unsigned==0 || ij_unsigned->length > sum2){
if(!Cyc_PrattProver_add_edge(g,0,i->source,j->source,sum2,0))
return 0;}}}}}}}}}}
# 481
return 1;}
# 484
static struct _tuple11 Cyc_PrattProver_eval_term(void*t){
void*_Tmp0;unsigned _Tmp1;switch(*((int*)t)){case 0: _Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp1;
return({struct _tuple11 _Tmp2;_Tmp2.f1=i,_Tmp2.f2=1;_Tmp2;});}case 1: _Tmp0=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
return Cyc_Evexp_eval_const_uint_exp(e);}default:
 return({struct _tuple11 _Tmp2;_Tmp2.f1=0U,_Tmp2.f2=0;_Tmp2;});};}struct _tuple13{void*f1;int f2;};
# 492
static struct _tuple13 Cyc_PrattProver_break_term(struct Cyc_PrattProver_Graph*g,int is_signed,void*t){
int c=0;
unsigned cu=0U;
void*res=t;
enum Cyc_Absyn_Primop p;
{void*_Tmp0;void*_Tmp1;if(*((int*)t)==4)switch((int)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f1){case Cyc_Absyn_Plus: if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2)->tl)->tl==0){_Tmp1=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2->hd;_Tmp0=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2->tl->hd;{void*t1=_Tmp1;void*t2=_Tmp0;
# 499
p=0U;{
struct _tuple11 _Tmp2=Cyc_PrattProver_eval_term(t1);int _Tmp3;unsigned _Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{unsigned c1=_Tmp4;int okay1=_Tmp3;
struct _tuple11 _Tmp5=Cyc_PrattProver_eval_term(t2);int _Tmp6;unsigned _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{unsigned c2=_Tmp7;int okay2=_Tmp6;
if(okay1){
res=t2;
c=(int)c1;
cu=c1;}else{
if(okay2){
res=t1;
c=(int)c2;
cu=c2;}else{
return({struct _tuple13 _Tmp8;_Tmp8.f1=t,_Tmp8.f2=0;_Tmp8;});}}
goto _LL0;}}}}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}case Cyc_Absyn_Minus: if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2)->tl)->tl==0){_Tmp1=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2->hd;_Tmp0=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2->tl->hd;{void*t1=_Tmp1;void*t2=_Tmp0;
# 513
p=2U;{
struct _tuple11 _Tmp2=Cyc_PrattProver_eval_term(t2);int _Tmp3;unsigned _Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{unsigned c2=_Tmp4;int okay2=_Tmp3;
if(okay2){
res=t1;
c=-(int)c2;
cu=c2;}else{
return({struct _tuple13 _Tmp5;_Tmp5.f1=t,_Tmp5.f2=0;_Tmp5;});}
goto _LL0;}}}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: goto _LL5;}else{_LL5:
# 522
 return({struct _tuple13 _Tmp2;_Tmp2.f1=t,_Tmp2.f2=0;_Tmp2;});}_LL0:;}
# 526
if(is_signed){
if(c==0)return({struct _tuple13 _Tmp0;_Tmp0.f1=res,_Tmp0.f2=0;_Tmp0;});else{
if(c > 0){
# 530
struct Cyc_PrattProver_Dist*dist=({struct Cyc_PrattProver_Row*_Tmp0=Cyc_PrattProver_lookup_row(g,res);Cyc_PrattProver_lookup_distance(_Tmp0,1,Cyc_AssnDef_zero());});
if(dist==0 || dist->length > 2147483647 - c)
return({struct _tuple13 _Tmp0;_Tmp0.f1=t,_Tmp0.f2=0;_Tmp0;});else{
# 534
return({struct _tuple13 _Tmp0;_Tmp0.f1=res,_Tmp0.f2=c;_Tmp0;});}}else{
# 537
struct Cyc_PrattProver_Dist*dist=({struct Cyc_PrattProver_Row*_Tmp0=({struct Cyc_PrattProver_Graph*_Tmp1=g;Cyc_PrattProver_lookup_row(_Tmp1,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_distance(_Tmp0,1,res);});
if(dist==0 || dist->length > c - -2147483648)
return({struct _tuple13 _Tmp0;_Tmp0.f1=t,_Tmp0.f2=0;_Tmp0;});else{
# 544
return({struct _tuple13 _Tmp0;_Tmp0.f1=res,_Tmp0.f2=c;_Tmp0;});}}}}else{
# 548
if((int)p==0){
struct Cyc_PrattProver_Dist*dist=({struct Cyc_PrattProver_Row*_Tmp0=Cyc_PrattProver_lookup_row(g,res);Cyc_PrattProver_lookup_distance(_Tmp0,0,Cyc_AssnDef_zero());});
if(dist==0)return({struct _tuple13 _Tmp0;_Tmp0.f1=t,_Tmp0.f2=0;_Tmp0;});
if(dist->length <= 0)return({struct _tuple13 _Tmp0;_Tmp0.f1=res,_Tmp0.f2=(int)cu;_Tmp0;});
if(dist->length > 0 &&(unsigned)dist->length <= 4294967295U - cu)
return({struct _tuple13 _Tmp0;_Tmp0.f1=res,_Tmp0.f2=(int)cu;_Tmp0;});}else{
# 556
return({struct _tuple13 _Tmp0;_Tmp0.f1=t,_Tmp0.f2=0;_Tmp0;});}}
# 559
return({struct _tuple13 _Tmp0;_Tmp0.f1=t,_Tmp0.f2=0;_Tmp0;});}
# 564
static int Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*g,void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
# 567
struct _tuple11 _Tmp0=Cyc_PrattProver_eval_term(t1);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{unsigned c1=_Tmp2;int okay1=_Tmp1;
struct _tuple11 _Tmp3=Cyc_PrattProver_eval_term(t2);int _Tmp4;unsigned _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{unsigned c2=_Tmp5;int okay2=_Tmp4;
void*origin=Cyc_AssnDef_prim(t1,p,t2);
if(okay1 && okay2){
# 572
switch((int)p){case Cyc_AssnDef_ULt:
 return c1 < c2;case Cyc_AssnDef_SLt:
 return(int)c1 < (int)c2;case Cyc_AssnDef_ULte:
 return c1 <= c2;case Cyc_AssnDef_SLte:
 return(int)c1 <= (int)c2;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp6="Vcgen: found bad primop in add_constraint";_tag_fat(_Tmp6,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));};}else{
# 579
if(okay2){
# 581
switch((int)p){case Cyc_AssnDef_ULt:
# 584
 if(c2==0U)return 0;
c2=c2 - 1U;
goto _LL15;case Cyc_AssnDef_ULte: _LL15:
# 589
 if(c2 <= 2147483647U){
if(!({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t1;void*_Tmp8=Cyc_AssnDef_zero();int _Tmp9=(int)c2;Cyc_PrattProver_add_edge(_Tmp6,0,_Tmp7,_Tmp8,_Tmp9,origin);}))return 0;
# 592
return({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t1;void*_Tmp8=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_Tmp6,1,_Tmp7,_Tmp8,(int)c2,0);});}else{
# 594
struct Cyc_PrattProver_Dist*c1=({struct Cyc_PrattProver_Row*_Tmp6=Cyc_PrattProver_lookup_row(g,t1);Cyc_PrattProver_lookup_distance(_Tmp6,1,Cyc_AssnDef_zero());});
if(c1!=0 && c1->length <= -1)
return({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t1;void*_Tmp8=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_Tmp6,1,_Tmp7,_Tmp8,(int)c2,0);});}
# 599
return 1;case Cyc_AssnDef_SLt:
# 602
 if(c2==2147483648U)return 0;
c2=c2 - 1U;
goto _LL19;case Cyc_AssnDef_SLte: _LL19:
# 607
 if(!({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t1;void*_Tmp8=Cyc_AssnDef_zero();int _Tmp9=(int)c2;Cyc_PrattProver_add_edge(_Tmp6,1,_Tmp7,_Tmp8,_Tmp9,origin);}))return 0;
if((int)c2 > 0){
# 610
struct Cyc_PrattProver_Dist*c1=({struct Cyc_PrattProver_Row*_Tmp6=({struct Cyc_PrattProver_Graph*_Tmp7=g;Cyc_PrattProver_lookup_row(_Tmp7,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_distance(_Tmp6,1,t1);});
if(c1!=0 && c1->length <= 0)
# 613
return({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t1;void*_Tmp8=Cyc_AssnDef_zero();Cyc_PrattProver_add_edge(_Tmp6,0,_Tmp7,_Tmp8,(int)c2,0);});}
# 615
return 1;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp6="Vcgen: found bad primop in add_constraint";_tag_fat(_Tmp6,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));};}else{
# 618
if(okay1){
# 620
switch((int)p){case Cyc_AssnDef_ULt:
# 623
 if(c1==4294967295U)return 0;
c1=c1 + 1U;
goto _LL20;case Cyc_AssnDef_ULte: _LL20:
# 628
 if(c1 > 2147483647U){
# 631
if(!({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t2;Cyc_PrattProver_add_edge(_Tmp6,1,_Tmp7,Cyc_AssnDef_zero(),-1,0);}))return 0;{
int k=-(int)c1;
if(k > 0){
if(!({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=Cyc_AssnDef_zero();void*_Tmp8=t2;Cyc_PrattProver_add_edge(_Tmp6,1,_Tmp7,_Tmp8,k,0);}))return 0;}}}
# 637
if(c1 <= 2147483648U){
# 639
int k=-(int)c1;
return({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=Cyc_AssnDef_zero();void*_Tmp8=t2;int _Tmp9=k;Cyc_PrattProver_add_edge(_Tmp6,0,_Tmp7,_Tmp8,_Tmp9,origin);});}
# 642
return 1;case Cyc_AssnDef_SLt:
# 645
 if(c1==2147483647U)return 0;
c1=c1 + 1U;
goto _LL24;case Cyc_AssnDef_SLte: _LL24:
# 650
 if(c1==2147483648U)return 1;else{
# 652
int k=-(int)c1;
if((int)c1 >= 0){
# 655
if(!({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=Cyc_AssnDef_zero();void*_Tmp8=t2;Cyc_PrattProver_add_edge(_Tmp6,0,_Tmp7,_Tmp8,k,0);}))return 0;}
# 657
return({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=Cyc_AssnDef_zero();void*_Tmp8=t2;int _Tmp9=k;Cyc_PrattProver_add_edge(_Tmp6,1,_Tmp7,_Tmp8,_Tmp9,origin);});}default:
# 659
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp6="Vcgen: found bad primop in add_constraint";_tag_fat(_Tmp6,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));};}}}
# 666
switch((int)p){case Cyc_AssnDef_ULt:  {
# 669
struct _tuple13 _Tmp6=Cyc_PrattProver_break_term(g,0,t1);int _Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{void*tt1=_Tmp8;int c1=_Tmp7;
struct _tuple13 _Tmp9=Cyc_PrattProver_break_term(g,0,t2);int _TmpA;void*_TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{void*tt2=_TmpB;int c2=_TmpA;
struct _tuple12 _TmpC=Cyc_PrattProver_is_unsigned_overflow((unsigned)c2,1U,(unsigned)c1);int _TmpD;int _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{int of=_TmpE;int dist=_TmpD;
if(!of){
struct _tuple12 _TmpF=Cyc_PrattProver_is_unsigned_overflow((unsigned)dist,1U,1U);int _Tmp10;int _Tmp11;_Tmp11=_TmpF.f1;_Tmp10=_TmpF.f2;{int of=_Tmp11;int dist=_Tmp10;
if(!of){
if(!Cyc_PrattProver_add_edge(g,0,tt1,tt2,dist,origin))
return 0;}}}
# 679
return Cyc_PrattProver_add_edge(g,0,t1,t2,-1,origin);}}}}case Cyc_AssnDef_ULte:  {
# 682
struct _tuple13 _Tmp6=Cyc_PrattProver_break_term(g,0,t1);int _Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{void*tt1=_Tmp8;int c1=_Tmp7;
struct _tuple13 _Tmp9=Cyc_PrattProver_break_term(g,0,t2);int _TmpA;void*_TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{void*tt2=_TmpB;int c2=_TmpA;
struct _tuple12 _TmpC=Cyc_PrattProver_is_unsigned_overflow((unsigned)c2,1U,(unsigned)c1);int _TmpD;int _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{int of=_TmpE;int dist=_TmpD;
if(!of){
# 688
if(!Cyc_PrattProver_add_edge(g,0,tt1,tt2,dist,origin))
return 0;}
# 691
return Cyc_PrattProver_add_edge(g,0,t1,t2,0,origin);}}}}case Cyc_AssnDef_SLt:  {
# 693
struct _tuple13 _Tmp6=Cyc_PrattProver_break_term(g,1,t1);int _Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{void*tt1=_Tmp8;int c1=_Tmp7;
struct _tuple13 _Tmp9=Cyc_PrattProver_break_term(g,1,t2);int _TmpA;void*_TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{void*tt2=_TmpB;int c2=_TmpA;
struct _tuple12 _TmpC=Cyc_PrattProver_is_signed_overflow(c2,1U,c1);int _TmpD;int _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{int of=_TmpE;int dist=_TmpD;
if(!of){
struct _tuple12 _TmpF=Cyc_PrattProver_is_signed_overflow(dist,1U,1);int _Tmp10;int _Tmp11;_Tmp11=_TmpF.f1;_Tmp10=_TmpF.f2;{int of=_Tmp11;int dist=_Tmp10;
if(!of){
if(!Cyc_PrattProver_add_edge(g,1,tt1,tt2,dist,origin))
return 0;}}}
# 703
return Cyc_PrattProver_add_edge(g,1,t1,t2,-1,origin);}}}}case Cyc_AssnDef_SLte:  {
# 706
struct _tuple13 _Tmp6=Cyc_PrattProver_break_term(g,1,t1);int _Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{void*tt1=_Tmp8;int c1=_Tmp7;
struct _tuple13 _Tmp9=Cyc_PrattProver_break_term(g,1,t2);int _TmpA;void*_TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{void*tt2=_TmpB;int c2=_TmpA;
struct _tuple12 _TmpC=Cyc_PrattProver_is_signed_overflow(c2,1U,c1);int _TmpD;int _TmpE;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{int of=_TmpE;int dist=_TmpD;
if(!of){
if(!Cyc_PrattProver_add_edge(g,1,tt1,tt2,dist,origin))
return 0;}
# 713
return Cyc_PrattProver_add_edge(g,1,t1,t2,0,origin);}}}}default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp6="Vcgen:found bad primop in add_constraint";_tag_fat(_Tmp6,sizeof(char),41U);}),_tag_fat(0U,sizeof(void*),0));};}}}char Cyc_PrattProver_TooLarge[9U]="TooLarge";struct Cyc_PrattProver_TooLarge_exn_struct{char*tag;};
# 719
struct Cyc_PrattProver_TooLarge_exn_struct Cyc_PrattProver_too_large={Cyc_PrattProver_TooLarge};
# 722
unsigned Cyc_PrattProver_max_paths=33U;
unsigned Cyc_PrattProver_max_paths_seen=0U;
# 731
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_cgraph(struct Cyc_PrattProver_Graph*gs,void*a){
LOOP:
# 735
 if(gs==0)return gs;
{enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL0;case 1:
 gs=0;goto _LL0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 740
gs=Cyc_PrattProver_cgraph(gs,a1);
a=a2;
goto LOOP;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 744
unsigned n=Cyc_PrattProver_num_graphs(gs);
if(Cyc_PrattProver_max_paths_seen < n)Cyc_PrattProver_max_paths_seen=n;
# 747
if(n >= Cyc_PrattProver_max_paths)
(void*)_throw((void*)& Cyc_PrattProver_too_large);{
# 751
struct Cyc_PrattProver_Graph*gs1=gs;
struct Cyc_PrattProver_Graph*gs2=Cyc_PrattProver_copy_graph(gs);
# 754
gs1=Cyc_PrattProver_cgraph(gs1,a1);
# 756
gs2=Cyc_PrattProver_cgraph(gs2,a2);
# 758
gs=Cyc_PrattProver_graph_append(gs1,gs2);
goto _LL0;}}case 2: switch((int)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2){case Cyc_AssnDef_Neq: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;void*t2=_Tmp1;
# 763
a=({void*_Tmp3=({void*_Tmp4=Cyc_AssnDef_slt(t1,t2);Cyc_AssnDef_or(_Tmp4,Cyc_AssnDef_slt(t2,t1));});Cyc_AssnDef_and(_Tmp3,({void*_Tmp4=Cyc_AssnDef_ult(t1,t2);Cyc_AssnDef_or(_Tmp4,Cyc_AssnDef_ult(t2,t1));}));});
goto LOOP;}case Cyc_AssnDef_Eq: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;void*t2=_Tmp1;
# 767
a=({void*_Tmp3=({void*_Tmp4=({void*_Tmp5=Cyc_AssnDef_slte(t1,t2);Cyc_AssnDef_and(_Tmp5,Cyc_AssnDef_slte(t2,t1));});Cyc_AssnDef_and(_Tmp4,Cyc_AssnDef_ulte(t1,t2));});Cyc_AssnDef_and(_Tmp3,Cyc_AssnDef_ulte(t2,t1));});
goto LOOP;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 772
struct Cyc_PrattProver_Graph*prev=0;
{struct Cyc_PrattProver_Graph*g=gs;for(0;g!=0;g=g->next){
if(!Cyc_PrattProver_add_constraint(g,t1,p,t2)){
if(prev==0)
gs=g->next;else{
# 778
prev->next=g->next;}}else{
# 781
prev=g;}}}
# 784
goto _LL0;}}default:
# 787
 a=Cyc_AssnDef_reduce(a);
goto LOOP;}_LL0:;}
# 790
return gs;}
# 796
static int Cyc_PrattProver_consistent(void*a){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
# 799
{struct Cyc_PrattProver_Graph*gs=({struct Cyc_PrattProver_Graph*_Tmp2=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_Tmp2,a);});
# 814 "pratt_prover.cyc"
int _Tmp2=gs!=0;_npop_handler(0);return _Tmp2;}
# 799 "pratt_prover.cyc"
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_PrattProver_TooLarge_exn_struct*)_Tmp2)->tag==Cyc_PrattProver_TooLarge)
# 818 "pratt_prover.cyc"
return 1;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;(void*)_rethrow(exn);}};}}}
# 823
int Cyc_PrattProver_constraint_prove(void*ctxt,void*a){
void*b=({void*_Tmp0=ctxt;Cyc_AssnDef_and(_Tmp0,Cyc_AssnDef_not(a));});
# 828
return !Cyc_PrattProver_consistent(b);}
# 831
static struct Cyc_PrattProver_Row*Cyc_PrattProver_lookup_row_in_rows(struct Cyc_PrattProver_Row*r,void*i){
for(1;r!=0;r=r->next){
if(Cyc_AssnDef_cmp_term(i,r->source)==0)return r;}
# 835
return 0;}
# 838
static struct Cyc_PrattProver_Row*Cyc_PrattProver_or2rows(struct Cyc_PrattProver_Row*g1,struct Cyc_PrattProver_Row*g2){
struct Cyc_PrattProver_Row*res=0;
for(1;g1!=0;g1=g1->next){
struct Cyc_PrattProver_Row*r=Cyc_PrattProver_lookup_row_in_rows(g2,g1->source);
if(r!=0){
# 844
struct Cyc_PrattProver_Row*newrow;newrow=_cycalloc(sizeof(struct Cyc_PrattProver_Row)),*newrow=*g1;
newrow->next=res;
res=newrow;
res->distance=0;{
# 849
struct Cyc_PrattProver_Distance*d=g1->distance;for(0;d!=0;d=d->next){
struct Cyc_PrattProver_Distance*dist_res=0;
if((int)d->dinfo & 2){
int d1=d->signed_dist.length;
struct Cyc_PrattProver_Dist*d2_ptr=Cyc_PrattProver_lookup_distance(r,1,d->target);
if(d2_ptr!=0){
int d2=d2_ptr->length;
struct Cyc_PrattProver_Dist newd=d1 > d2?d->signed_dist:*d2_ptr;
dist_res=({struct Cyc_PrattProver_Distance*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Distance));_Tmp0->target=d->target,_Tmp0->next=res->distance,_Tmp0->dinfo=2,
# 860
_Tmp0->unsigned_dist.length=0,_Tmp0->unsigned_dist.prim=0,_Tmp0->signed_dist=newd;_Tmp0;});}}
# 864
if((int)d->dinfo & 1){
int d1=d->unsigned_dist.length;
struct Cyc_PrattProver_Dist*d2_ptr=Cyc_PrattProver_lookup_distance(r,0,d->target);
if(d2_ptr!=0){
int d2=d2_ptr->length;
struct Cyc_PrattProver_Dist newd=d1 > d2?d->unsigned_dist:*d2_ptr;
if(dist_res!=0){
dist_res->unsigned_dist=newd;
dist_res->dinfo=3;}else{
# 874
dist_res=({struct Cyc_PrattProver_Distance*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Distance));_Tmp0->target=d->target,_Tmp0->next=res->distance,_Tmp0->dinfo=1,_Tmp0->unsigned_dist=newd,
# 878
_Tmp0->signed_dist.length=0,_Tmp0->signed_dist.prim=0;_Tmp0;});}}}
# 882
if(dist_res!=0)
res->distance=dist_res;}}}}
# 888
return res;}
# 891
void*Cyc_PrattProver_int2term(int is_signed,int c){
enum Cyc_Absyn_Sign s=is_signed?0U: 1U;
struct Cyc_Absyn_Exp*e=Cyc_Absyn_int_exp(s,c,0U);
({void*_Tmp0=Cyc_Absyn_int_type(s,2U);e->topt=_Tmp0;});
return Cyc_AssnDef_cnst(e);}
# 898
void*Cyc_PrattProver_edge2assn(void*source,void*target,int weight,int is_signed){
enum Cyc_AssnDef_Primreln s=is_signed?3U: 5U;
enum Cyc_AssnDef_Primreln s1=is_signed?2U: 4U;
void*result=(void*)& Cyc_AssnDef_true_assn;
if(({void*_Tmp0=source;_Tmp0==Cyc_AssnDef_zero();}))
result=({void*_Tmp0=Cyc_PrattProver_int2term(is_signed,- weight);enum Cyc_AssnDef_Primreln _Tmp1=s;Cyc_AssnDef_prim(_Tmp0,_Tmp1,target);});else{
if(({void*_Tmp0=target;_Tmp0==Cyc_AssnDef_zero();}))
result=({void*_Tmp0=source;enum Cyc_AssnDef_Primreln _Tmp1=s;Cyc_AssnDef_prim(_Tmp0,_Tmp1,Cyc_PrattProver_int2term(is_signed,weight));});else{
# 907
switch((int)weight){case 0:
 result=Cyc_AssnDef_prim(source,s,target);goto _LL0;case -1:
 result=Cyc_AssnDef_prim(source,s1,target);goto _LL0;default:
# 915
 goto _LL0;}_LL0:;}}
# 918
return result;}
# 922
void*Cyc_PrattProver_row2assn(struct Cyc_PrattProver_Row*r){
void*a=(void*)& Cyc_AssnDef_true_assn;
for(1;r!=0;r=r->next){
struct Cyc_PrattProver_Distance*d=r->distance;for(0;d!=0;d=d->next){
if((int)d->dinfo & 2){
if(d->signed_dist.prim!=0)
a=Cyc_AssnDef_and(a,_check_null(d->signed_dist.prim));else{
# 930
a=({void*_Tmp0=a;Cyc_AssnDef_and(_Tmp0,Cyc_PrattProver_edge2assn(r->source,d->target,d->signed_dist.length,1));});}}
# 933
if((int)d->dinfo & 1){
if(d->unsigned_dist.prim!=0)
a=Cyc_AssnDef_and(a,_check_null(d->unsigned_dist.prim));else{
# 937
a=({void*_Tmp0=a;Cyc_AssnDef_and(_Tmp0,Cyc_PrattProver_edge2assn(r->source,d->target,d->unsigned_dist.length,0));});}}}}
# 942
return a;}
# 945
void*Cyc_PrattProver_merge_assn(void*a1,void*a2){
# 947
struct Cyc_PrattProver_Graph*g=({struct Cyc_PrattProver_Graph*_Tmp0=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_Tmp0,Cyc_AssnDef_or(a1,a2));});
if(g==0)return(void*)& Cyc_AssnDef_false_assn;{
struct Cyc_PrattProver_Row*row=g->rows;
{struct Cyc_PrattProver_Graph*row1=g->next;for(0;row1!=0;row1=row1->next){
row=Cyc_PrattProver_or2rows(row,row1->rows);}}
# 953
return Cyc_PrattProver_row2assn(row);}}
