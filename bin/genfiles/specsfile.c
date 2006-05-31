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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 90
extern int Cyc_fgetc(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 271 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_file_open(struct _fat_ptr,struct _fat_ptr);
# 300
extern int isspace(int);
# 313
extern char*getenv(const char*);extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 336
extern void*Cyc_List_assoc_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 364
extern struct _fat_ptr Cyc_List_to_array(struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
extern int Cyc_strncmp(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 105 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 110
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);
# 30 "filename.h"
extern struct _fat_ptr Cyc_Filename_concat(struct _fat_ptr,struct _fat_ptr);
# 69 "arg.h"
extern int Cyc_Arg_current;
# 71
extern void Cyc_Arg_parse(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);
# 29 "specsfile.h"
extern struct _fat_ptr Cyc_Specsfile_target_arch;
# 31
extern struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
# 37
extern struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
extern struct _fat_ptr Cyc_Specsfile_def_lib_path;
# 31 "specsfile.cyc"
extern char*Cdef_lib_path;
extern char*Carch;
# 38
struct _fat_ptr Cyc_Specsfile_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
void Cyc_Specsfile_set_target_arch(struct _fat_ptr s){
Cyc_Specsfile_target_arch=s;}
# 43
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path=0;
void Cyc_Specsfile_add_cyclone_exec_path(struct _fat_ptr s){struct _fat_ptr _T0;unsigned long _T1;struct _fat_ptr _T2;struct Cyc_List_List*_T3;struct _fat_ptr*_T4;
unsigned long len=Cyc_strlen(s);
if(len > 2U)goto _TL0;return;_TL0: _T0=s;_T1=len - 2U;_T2=
Cyc_substring(_T0,2,_T1);{struct _fat_ptr dir=_T2;{struct Cyc_List_List*_T5=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T6=_cycalloc(sizeof(struct _fat_ptr));
*_T6=dir;_T4=(struct _fat_ptr*)_T6;}_T5->hd=_T4;_T5->tl=Cyc_Specsfile_cyclone_exec_path;_T3=(struct Cyc_List_List*)_T5;}Cyc_Specsfile_cyclone_exec_path=_T3;}}struct _tuple0{struct _fat_ptr*f0;struct _fat_ptr*f1;};
# 57
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _fat_ptr file){struct _fat_ptr _T0;char*_T1;char*_T2;const char*_T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc___cycFILE*_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;int _T8;struct Cyc_String_pa_PrintArg_struct _T9;struct Cyc_Int_pa_PrintArg_struct _TA;int _TB;struct Cyc___cycFILE*_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct Cyc_String_pa_PrintArg_struct _TF;struct Cyc___cycFILE*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;char*_T13;int _T14;char*_T15;char*_T16;char*_T17;int _T18;char*_T19;char*_T1A;int _T1B;struct Cyc_String_pa_PrintArg_struct _T1C;struct Cyc___cycFILE*_T1D;struct _fat_ptr _T1E;struct _fat_ptr _T1F;int _T20;struct Cyc_List_List*_T21;struct _tuple0*_T22;struct _fat_ptr*_T23;char*_T24;struct _fat_ptr _T25;struct _fat_ptr _T26;struct _fat_ptr*_T27;struct _fat_ptr _T28;struct _fat_ptr _T29;char*_T2A;int _T2B;char*_T2C;int _T2D;char*_T2E;char*_T2F;char*_T30;int _T31;char*_T32;char*_T33;int _T34;struct Cyc_String_pa_PrintArg_struct _T35;struct Cyc_String_pa_PrintArg_struct _T36;char*_T37;struct Cyc___cycFILE*_T38;struct _fat_ptr _T39;struct _fat_ptr _T3A;struct Cyc_List_List*_T3B;struct _tuple0*_T3C;struct _fat_ptr*_T3D;char*_T3E;struct _fat_ptr _T3F;struct _fat_ptr _T40;struct _fat_ptr*_T41;char*_T42;struct _fat_ptr _T43;struct _fat_ptr _T44;struct Cyc_List_List*_T45;
struct Cyc_List_List*result=0;
int c;
int i;
char strname[256U];
char strvalue[4096U];_T0=file;_T1=_untag_fat_ptr_check_bound(_T0,sizeof(char),1U);_T2=_check_null(_T1);_T3=(const char*)_T2;{
struct Cyc___cycFILE*spec_file=Cyc_fopen(_T3,"r");
# 65
if(spec_file!=0)goto _TL2;{struct Cyc_String_pa_PrintArg_struct _T46;_T46.tag=0;
_T46.f1=file;_T4=_T46;}{struct Cyc_String_pa_PrintArg_struct _T46=_T4;void*_T47[1];_T47[0]=& _T46;_T5=Cyc_stderr;_T6=_tag_fat("Error opening spec file %s\n",sizeof(char),28U);_T7=_tag_fat(_T47,sizeof(void*),1);Cyc_fprintf(_T5,_T6,_T7);}
Cyc_fflush(Cyc_stderr);
return 0;_TL2:
# 71
 _TL4: if(1)goto _TL5;else{goto _TL6;}
_TL5: _TL7: if(1)goto _TL8;else{goto _TL9;}
_TL8: c=Cyc_fgetc(spec_file);_T8=
isspace(c);if(!_T8)goto _TLA;goto _TL7;_TLA:
 if(c!=42)goto _TLC;goto _TL9;_TLC:
 if(c==-1)goto _TLE;{struct Cyc_String_pa_PrintArg_struct _T46;_T46.tag=0;
# 79
_T46.f1=file;_T9=_T46;}{struct Cyc_String_pa_PrintArg_struct _T46=_T9;{struct Cyc_Int_pa_PrintArg_struct _T47;_T47.tag=1;_TB=c;_T47.f1=(unsigned long)_TB;_TA=_T47;}{struct Cyc_Int_pa_PrintArg_struct _T47=_TA;void*_T48[2];_T48[0]=& _T46;_T48[1]=& _T47;_TC=Cyc_stderr;_TD=
# 78
_tag_fat("Error reading spec file %s: unexpected character '%c'\n",sizeof(char),55U);_TE=_tag_fat(_T48,sizeof(void*),2);Cyc_fprintf(_TC,_TD,_TE);}}
# 80
Cyc_fflush(Cyc_stderr);goto _TLF;_TLE: _TLF: goto CLEANUP_AND_RETURN;goto _TL7;_TL9:
# 84
 JUST_AFTER_STAR:
 i=0;
_TL10: if(1)goto _TL11;else{goto _TL12;}
_TL11: c=Cyc_fgetc(spec_file);
if(c!=-1)goto _TL13;{struct Cyc_String_pa_PrintArg_struct _T46;_T46.tag=0;
# 91
_T46.f1=file;_TF=_T46;}{struct Cyc_String_pa_PrintArg_struct _T46=_TF;void*_T47[1];_T47[0]=& _T46;_T10=Cyc_stderr;_T11=
# 90
_tag_fat("Error reading spec file %s: unexpected EOF\n",sizeof(char),44U);_T12=_tag_fat(_T47,sizeof(void*),1);Cyc_fprintf(_T10,_T11,_T12);}
# 92
Cyc_fflush(Cyc_stderr);goto CLEANUP_AND_RETURN;_TL13:
# 95
 if(c!=58)goto _TL15;_T13=strname;_T14=i;_T15=_check_known_subscript_notnull(_T13,256U,sizeof(char),_T14);_T16=(char*)_T15;
*_T16='\000';goto _TL12;_TL15: _T17=strname;_T18=i;_T19=_check_known_subscript_notnull(_T17,256U,sizeof(char),_T18);_T1A=(char*)_T19;_T1B=c;
# 99
*_T1A=(char)_T1B;
i=i + 1;
if(i < 256)goto _TL17;{struct Cyc_String_pa_PrintArg_struct _T46;_T46.tag=0;
# 104
_T46.f1=file;_T1C=_T46;}{struct Cyc_String_pa_PrintArg_struct _T46=_T1C;void*_T47[1];_T47[0]=& _T46;_T1D=Cyc_stderr;_T1E=
# 103
_tag_fat("Error reading spec file %s: string name too long\n",sizeof(char),50U);_T1F=_tag_fat(_T47,sizeof(void*),1);Cyc_fprintf(_T1D,_T1E,_T1F);}
# 105
Cyc_fflush(Cyc_stderr);goto CLEANUP_AND_RETURN;_TL17: goto _TL10;_TL12:
# 109
 _TL19: if(1)goto _TL1A;else{goto _TL1B;}
_TL1A: c=Cyc_fgetc(spec_file);_T20=
isspace(c);if(!_T20)goto _TL1C;goto _TL19;_TL1C: goto _TL1B;goto _TL19;_TL1B:
# 114
 if(c!=42)goto _TL1E;{struct Cyc_List_List*_T46=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_T47=_cycalloc(sizeof(struct _tuple0));{struct _fat_ptr*_T48=_cycalloc(sizeof(struct _fat_ptr));_T24=strname;_T25=
# 116
_tag_fat(_T24,sizeof(char),256U);_T26=Cyc_strdup(_T25);*_T48=_T26;_T23=(struct _fat_ptr*)_T48;}_T47->f0=_T23;{struct _fat_ptr*_T48=_cycalloc(sizeof(struct _fat_ptr));_T28=
_tag_fat("",sizeof(char),1U);_T29=Cyc_strdup(_T28);*_T48=_T29;_T27=(struct _fat_ptr*)_T48;}_T47->f1=_T27;_T22=(struct _tuple0*)_T47;}
# 116
_T46->hd=_T22;
_T46->tl=result;_T21=(struct Cyc_List_List*)_T46;}
# 116
result=_T21;goto JUST_AFTER_STAR;_TL1E: _T2A=strvalue;_T2B=c;
# 121
_T2A[0]=(char)_T2B;
i=1;
_TL20: if(1)goto _TL21;else{goto _TL22;}
_TL21: c=Cyc_fgetc(spec_file);
if(c==-1)goto _TL25;else{goto _TL27;}_TL27: if(c==10)goto _TL25;else{goto _TL26;}_TL26: if(c==13)goto _TL25;else{goto _TL23;}_TL25: _T2C=strvalue;_T2D=i;_T2E=_check_known_subscript_notnull(_T2C,4096U,sizeof(char),_T2D);_T2F=(char*)_T2E;*_T2F='\000';goto _TL22;_TL23: _T30=strvalue;_T31=i;_T32=_check_known_subscript_notnull(_T30,4096U,sizeof(char),_T31);_T33=(char*)_T32;_T34=c;
*_T33=(char)_T34;
i=i + 1;
if(i < 4096)goto _TL28;{struct Cyc_String_pa_PrintArg_struct _T46;_T46.tag=0;
# 131
_T46.f1=file;_T35=_T46;}{struct Cyc_String_pa_PrintArg_struct _T46=_T35;{struct Cyc_String_pa_PrintArg_struct _T47;_T47.tag=0;_T37=strname;_T47.f1=_tag_fat(_T37,sizeof(char),256U);_T36=_T47;}{struct Cyc_String_pa_PrintArg_struct _T47=_T36;void*_T48[2];_T48[0]=& _T46;_T48[1]=& _T47;_T38=Cyc_stderr;_T39=
# 130
_tag_fat("Error reading spec file %s: value of %s too long\n",sizeof(char),50U);_T3A=_tag_fat(_T48,sizeof(void*),2);Cyc_fprintf(_T38,_T39,_T3A);}}
# 132
Cyc_fflush(Cyc_stderr);goto CLEANUP_AND_RETURN;_TL28: goto _TL20;_TL22:{struct Cyc_List_List*_T46=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_T47=_cycalloc(sizeof(struct _tuple0));{struct _fat_ptr*_T48=_cycalloc(sizeof(struct _fat_ptr));_T3E=strname;_T3F=
# 137
_tag_fat(_T3E,sizeof(char),256U);_T40=Cyc_strdup(_T3F);*_T48=_T40;_T3D=(struct _fat_ptr*)_T48;}_T47->f0=_T3D;{struct _fat_ptr*_T48=_cycalloc(sizeof(struct _fat_ptr));_T42=strvalue;_T43=
_tag_fat(_T42,sizeof(char),4096U);_T44=Cyc_strdup(_T43);*_T48=_T44;_T41=(struct _fat_ptr*)_T48;}_T47->f1=_T41;_T3C=(struct _tuple0*)_T47;}
# 137
_T46->hd=_T3C;
_T46->tl=result;_T3B=(struct Cyc_List_List*)_T46;}
# 137
result=_T3B;
# 139
if(c!=-1)goto _TL2A;goto CLEANUP_AND_RETURN;_TL2A: goto _TL4;_TL6:
# 142
 CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);_T45=result;
return _T45;}}
# 148
struct _fat_ptr Cyc_Specsfile_split_specs(struct _fat_ptr cmdline){struct _fat_ptr _T0;unsigned char*_T1;char*_T2;struct _fat_ptr _T3;int _T4;unsigned long _T5;unsigned long _T6;struct _fat_ptr _T7;unsigned char*_T8;const char*_T9;int _TA;char _TB;int _TC;struct _fat_ptr _TD;unsigned char*_TE;const char*_TF;int _T10;char _T11;int _T12;int _T13;int _T14;unsigned long _T15;unsigned long _T16;struct _fat_ptr _T17;unsigned char*_T18;const char*_T19;int _T1A;char _T1B;int _T1C;struct _fat_ptr _T1D;unsigned char*_T1E;const char*_T1F;int _T20;char _T21;int _T22;int _T23;struct _fat_ptr _T24;unsigned char*_T25;const char*_T26;int _T27;char _T28;int _T29;int _T2A;unsigned long _T2B;unsigned long _T2C;struct _fat_ptr _T2D;unsigned char*_T2E;const char*_T2F;int _T30;char _T31;int _T32;char*_T33;int _T34;char*_T35;char*_T36;struct _fat_ptr _T37;unsigned char*_T38;const char*_T39;int _T3A;char*_T3B;int _T3C;char*_T3D;char*_T3E;struct _fat_ptr _T3F;unsigned char*_T40;const char*_T41;int _T42;char*_T43;int _T44;char*_T45;char*_T46;struct Cyc_List_List*_T47;struct _fat_ptr*_T48;char*_T49;struct _fat_ptr _T4A;struct _fat_ptr _T4B;struct Cyc_List_List*_T4C;struct _fat_ptr*_T4D;struct _fat_ptr _T4E;struct _fat_ptr _T4F;struct _fat_ptr*_T50;unsigned _T51;struct _fat_ptr _T52;unsigned _T53;struct _fat_ptr _T54;unsigned char*_T55;struct _fat_ptr**_T56;unsigned _T57;int _T58;struct _fat_ptr*_T59;struct _fat_ptr _T5A;_T0=cmdline;_T1=_T0.curr;_T2=(char*)_T1;
if(_T2!=0)goto _TL2C;_T3=_tag_fat(0,0,0);return _T3;_TL2C: {
unsigned long n=Cyc_strlen(cmdline);
struct Cyc_List_List*l=0;
char buf[4096U];
int i=0;
int j=0;
if(n <= 4096U)goto _TL2E;goto DONE;_TL2E:
 _TL30: if(1)goto _TL31;else{goto _TL32;}
# 158
_TL31: _TL33: if(1)goto _TL34;else{goto _TL35;}
_TL34: _T4=i;_T5=(unsigned long)_T4;_T6=n;if(_T5 < _T6)goto _TL36;goto DONE;_TL36: _T7=cmdline;_T8=_T7.curr;_T9=(const char*)_T8;_TA=i;_TB=_T9[_TA];_TC=(int)_TB;
if(_TC!=0)goto _TL38;goto DONE;_TL38: _TD=cmdline;_TE=_TD.curr;_TF=(const char*)_TE;_T10=i;_T11=_TF[_T10];_T12=(int)_T11;_T13=
isspace(_T12);if(_T13)goto _TL3A;else{goto _TL3C;}_TL3C: goto _TL35;_TL3A:
 i=i + 1;goto _TL33;_TL35:
# 164
 j=0;
# 169
_TL3D: if(1)goto _TL3E;else{goto _TL3F;}
_TL3E: _T14=i;_T15=(unsigned long)_T14;_T16=n;if(_T15 < _T16)goto _TL40;goto _TL3F;_TL40: _T17=cmdline;_T18=_T17.curr;_T19=(const char*)_T18;_T1A=i;_T1B=_T19[_T1A];_T1C=(int)_T1B;
if(_T1C!=0)goto _TL42;goto _TL3F;_TL42: _T1D=cmdline;_T1E=_T1D.curr;_T1F=(const char*)_T1E;_T20=i;_T21=_T1F[_T20];_T22=(int)_T21;_T23=
isspace(_T22);if(!_T23)goto _TL44;goto _TL3F;_TL44: _T24=cmdline;_T25=_T24.curr;_T26=(const char*)_T25;_T27=i;_T28=_T26[_T27];_T29=(int)_T28;
if(_T29!=92)goto _TL46;
# 175
i=i + 1;_T2A=i;_T2B=(unsigned long)_T2A;_T2C=n;
if(_T2B < _T2C)goto _TL48;goto _TL3F;_TL48: _T2D=cmdline;_T2E=_T2D.curr;_T2F=(const char*)_T2E;_T30=i;_T31=_T2F[_T30];_T32=(int)_T31;
if(_T32!=0)goto _TL4A;goto _TL3F;_TL4A: _T33=buf;_T34=j;_T35=_check_known_subscript_notnull(_T33,4096U,sizeof(char),_T34);_T36=(char*)_T35;_T37=cmdline;_T38=_T37.curr;_T39=(const char*)_T38;_T3A=i;
*_T36=_T39[_T3A];
j=j + 1;goto _TL47;
# 182
_TL46: _T3B=buf;_T3C=j;_T3D=_check_known_subscript_notnull(_T3B,4096U,sizeof(char),_T3C);_T3E=(char*)_T3D;_T3F=cmdline;_T40=_T3F.curr;_T41=(const char*)_T40;_T42=i;*_T3E=_T41[_T42];
j=j + 1;_TL47:
# 185
 i=i + 1;goto _TL3D;_TL3F:
# 187
 if(j >= 4096)goto _TL4C;_T43=buf;_T44=j;_T45=_check_known_subscript_notnull(_T43,4096U,sizeof(char),_T44);_T46=(char*)_T45;
*_T46='\000';goto _TL4D;_TL4C: _TL4D:{struct Cyc_List_List*_T5B=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T5C=_cycalloc(sizeof(struct _fat_ptr));_T49=buf;_T4A=
# 190
_tag_fat(_T49,sizeof(char),4096U);_T4B=Cyc_strdup(_T4A);*_T5C=_T4B;_T48=(struct _fat_ptr*)_T5C;}_T5B->hd=_T48;_T5B->tl=l;_T47=(struct Cyc_List_List*)_T5B;}l=_T47;goto _TL30;_TL32:
# 192
 DONE:
 l=Cyc_List_imp_rev(l);{struct Cyc_List_List*_T5B=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T5C=_cycalloc(sizeof(struct _fat_ptr));
*_T5C=_tag_fat("",sizeof(char),1U);_T4D=(struct _fat_ptr*)_T5C;}_T5B->hd=_T4D;_T5B->tl=l;_T4C=(struct Cyc_List_List*)_T5B;}l=_T4C;{
struct _fat_ptr ptrarray=Cyc_List_to_array(l);_T4F=ptrarray;{unsigned _T5B=
_get_fat_size(_T4F,sizeof(struct _fat_ptr*));_T51=_check_times(_T5B,sizeof(struct _fat_ptr));{struct _fat_ptr*_T5C=_cycalloc(_T51);_T52=ptrarray;{unsigned _T5D=_get_fat_size(_T52,sizeof(struct _fat_ptr*));unsigned k;k=0;_TL51: if(k < _T5D)goto _TL4F;else{goto _TL50;}_TL4F: _T53=k;_T54=ptrarray;_T55=_T54.curr;_T56=(struct _fat_ptr**)_T55;_T57=k;_T58=(int)_T57;_T59=_T56[_T58];_T5C[_T53]=*_T59;k=k + 1;goto _TL51;_TL50:;}_T50=(struct _fat_ptr*)_T5C;}_T4E=_tag_fat(_T50,sizeof(struct _fat_ptr),_T5B);}{struct _fat_ptr result=_T4E;_T5A=result;
return _T5A;}}}}
# 200
struct _fat_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _fat_ptr spec_name){struct _handler_cons*_T0;int*_T1;int _T2;struct _fat_ptr*(*_T3)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*);void*(*_T4)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T5)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T6;struct _fat_ptr*_T7;struct _fat_ptr*_T8;struct _fat_ptr*_T9;void*_TA;struct Cyc_Core_Not_found_exn_struct*_TB;char*_TC;char*_TD;struct _fat_ptr _TE;struct _handler_cons _TF;_T0=& _TF;_push_handler(_T0);{int _T10=0;_T1=_TF.handler;_T2=setjmp(_T1);if(!_T2)goto _TL52;_T10=1;goto _TL53;_TL52: _TL53: if(_T10)goto _TL54;else{goto _TL56;}_TL56: _T4=Cyc_List_assoc_cmp;{
# 202
struct _fat_ptr*(*_T11)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(struct _fat_ptr*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))_T4;_T3=_T11;}_T5=Cyc_strptrcmp;_T6=specs;_T7=& spec_name;_T8=(struct _fat_ptr*)_T7;_T9=_T3(_T5,_T6,_T8);{struct _fat_ptr _T11=*_T9;_npop_handler(0);return _T11;}_pop_handler();goto _TL55;_TL54: _TA=Cyc_Core_get_exn_thrown();{void*_T11=(void*)_TA;void*_T12;_TB=(struct Cyc_Core_Not_found_exn_struct*)_T11;_TC=_TB->tag;_TD=Cyc_Core_Not_found;if(_TC!=_TD)goto _TL57;_TE=
# 205
_tag_fat(0,0,0);return _TE;_TL57: _T12=_T11;{void*exn=_T12;_rethrow(exn);};}_TL55:;}}
# 209
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path=0;
struct _fat_ptr Cyc_Specsfile_def_lib_path={(void*)0,(void*)0,(void*)(0 + 0)};
# 214
static struct Cyc_List_List*Cyc_Specsfile_also_subdir(struct Cyc_List_List*dirs,struct _fat_ptr subdir){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_List_List*_T3;struct _fat_ptr*_T4;struct Cyc_List_List*_T5;void*_T6;struct _fat_ptr*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;
# 216
struct Cyc_List_List*l=0;
_TL5C: if(dirs!=0)goto _TL5A;else{goto _TL5B;}
_TL5A:{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));_T1=dirs;_T2=_T1->hd;_TD->hd=(struct _fat_ptr*)_T2;_TD->tl=l;_T0=(struct Cyc_List_List*)_TD;}l=_T0;{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_TE=_cycalloc(sizeof(struct _fat_ptr));_T5=dirs;_T6=_T5->hd;_T7=(struct _fat_ptr*)_T6;_T8=*_T7;_T9=subdir;_TA=
Cyc_Filename_concat(_T8,_T9);*_TE=_TA;_T4=(struct _fat_ptr*)_TE;}_TD->hd=_T4;_TD->tl=l;_T3=(struct Cyc_List_List*)_TD;}l=_T3;_TB=dirs;
# 217
dirs=_TB->tl;goto _TL5C;_TL5B:
# 221
 l=Cyc_List_imp_rev(l);_TC=l;
return _TC;}
# 236 "specsfile.cyc"
struct _fat_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _fat_ptr),int(*anonflagfun)(struct _fat_ptr),struct _fat_ptr errmsg,struct _fat_ptr argv){struct _fat_ptr _T0;unsigned _T1;struct _fat_ptr _T2;int _T3;int*_T4;unsigned _T5;int _T6;unsigned _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;int _TA;char*_TB;struct _fat_ptr*_TC;struct _fat_ptr _TD;int _TE;struct _fat_ptr _TF;unsigned char*_T10;int*_T11;int _T12;struct _fat_ptr _T13;struct _fat_ptr _T14;unsigned char*_T15;struct _fat_ptr*_T16;int _T17;struct _fat_ptr _T18;int _T19;struct _fat_ptr _T1A;unsigned char*_T1B;int*_T1C;int _T1D;struct _fat_ptr _T1E;int _T1F;char*_T20;int*_T21;struct _fat_ptr _T22;int _T23;struct _fat_ptr*_T24;unsigned _T25;int _T26;unsigned _T27;struct _fat_ptr _T28;struct _fat_ptr _T29;int _T2A;struct _fat_ptr*_T2B;unsigned _T2C;int _T2D;unsigned _T2E;struct _fat_ptr _T2F;struct _fat_ptr _T30;char*_T31;struct _fat_ptr*_T32;struct _fat_ptr _T33;char*_T34;struct _fat_ptr*_T35;struct _fat_ptr _T36;char*_T37;struct _fat_ptr*_T38;struct _fat_ptr _T39;int _T3A;char*_T3B;int*_T3C;int _T3D;struct _fat_ptr _T3E;int _T3F;int _T40;char*_T41;struct _fat_ptr*_T42;struct _fat_ptr _T43;unsigned char*_T44;struct _fat_ptr*_T45;int _T46;struct _fat_ptr _T47;int _T48;int _T49;char*_T4A;struct _fat_ptr*_T4B;struct _fat_ptr _T4C;unsigned char*_T4D;struct _fat_ptr*_T4E;int _T4F;struct Cyc_List_List*_T50;void(*_T51)(struct _fat_ptr);int(*_T52)(struct _fat_ptr);struct _fat_ptr _T53;struct _fat_ptr _T54;struct _fat_ptr _T55;unsigned char*_T56;char*_T57;struct _fat_ptr _T58;void*_T59;void*_T5A;unsigned _T5B;struct _fat_ptr _T5C;void*_T5D;void*_T5E;unsigned _T5F;struct _fat_ptr _T60;unsigned char*_T61;char*_T62;struct Cyc_List_List*_T63;struct _fat_ptr*_T64;struct _fat_ptr _T65;void*_T66;void*_T67;unsigned _T68;unsigned long _T69;struct Cyc_List_List*_T6A;struct _fat_ptr*_T6B;struct _fat_ptr _T6C;struct _fat_ptr _T6D;struct _fat_ptr _T6E;struct _fat_ptr _T6F;_T0=argv;_T1=
# 244
_get_fat_size(_T0,sizeof(struct _fat_ptr));{int argc=(int)_T1;_T3=argc;{unsigned _T70=(unsigned)_T3;_T5=_check_times(_T70,sizeof(int));{int*_T71=_cycalloc_atomic(_T5);_T6=argc;{unsigned _T72=(unsigned)_T6;unsigned i;i=0;_TL60: if(i < _T72)goto _TL5E;else{goto _TL5F;}_TL5E: _T7=i;
_T71[_T7]=0;i=i + 1;goto _TL60;_TL5F:;}_T4=(int*)_T71;}_T2=_tag_fat(_T4,sizeof(int),_T70);}{struct _fat_ptr bindices=_T2;
int numbindices=0;
int i;int j;int k;
i=1;_TL64: if(i < argc)goto _TL62;else{goto _TL63;}
_TL62: _T8=_tag_fat("-B",sizeof(char),3U);_T9=argv;_TA=i;_TB=_check_fat_subscript(_T9,sizeof(struct _fat_ptr),_TA);_TC=(struct _fat_ptr*)_TB;_TD=*_TC;_TE=Cyc_strncmp(_T8,_TD,2U);if(_TE!=0)goto _TL65;_TF=bindices;_T10=_TF.curr;_T11=(int*)_T10;_T12=i;
_T11[_T12]=1;
numbindices=numbindices + 1;goto _TL66;
# 253
_TL65: _T13=_tag_fat("-b",sizeof(char),3U);_T14=argv;_T15=_T14.curr;_T16=(struct _fat_ptr*)_T15;_T17=i;_T18=_T16[_T17];_T19=Cyc_strcmp(_T13,_T18);if(_T19!=0)goto _TL67;_T1A=bindices;_T1B=_T1A.curr;_T1C=(int*)_T1B;_T1D=i;
_T1C[_T1D]=1;
numbindices=numbindices + 1;
i=i + 1;if(i < argc)goto _TL69;goto _TL63;_TL69: _T1E=bindices;_T1F=i;_T20=_check_fat_subscript(_T1E,sizeof(int),_T1F);_T21=(int*)_T20;
*_T21=1;
numbindices=numbindices + 1;goto _TL68;_TL67: _TL68: _TL66:
# 248
 i=i + 1;goto _TL64;_TL63: _T23=numbindices + 1;{unsigned _T70=(unsigned)_T23;_T25=_check_times(_T70,sizeof(struct _fat_ptr));{struct _fat_ptr*_T71=_cycalloc(_T25);_T26=numbindices + 1;{unsigned _T72=(unsigned)_T26;unsigned n;n=0;_TL6E: if(n < _T72)goto _TL6C;else{goto _TL6D;}_TL6C: _T27=n;_T28=
# 265
_tag_fat(0,0,0);_T71[_T27]=_T28;n=n + 1;goto _TL6E;_TL6D:;}_T24=(struct _fat_ptr*)_T71;}_T22=_tag_fat(_T24,sizeof(struct _fat_ptr),_T70);}{struct _fat_ptr bargs=_T22;_T2A=argc - numbindices;{unsigned _T70=(unsigned)_T2A;_T2C=_check_times(_T70,sizeof(struct _fat_ptr));{struct _fat_ptr*_T71=_cycalloc(_T2C);_T2D=argc - numbindices;{unsigned _T72=(unsigned)_T2D;unsigned n;n=0;_TL72: if(n < _T72)goto _TL70;else{goto _TL71;}_TL70: _T2E=n;_T2F=
_tag_fat(0,0,0);_T71[_T2E]=_T2F;n=n + 1;goto _TL72;_TL71:;}_T2B=(struct _fat_ptr*)_T71;}_T29=_tag_fat(_T2B,sizeof(struct _fat_ptr),_T70);}{struct _fat_ptr otherargs=_T29;_T30=bargs;_T31=_check_fat_subscript(_T30,sizeof(struct _fat_ptr),0);_T32=(struct _fat_ptr*)_T31;_T33=otherargs;_T34=_check_fat_subscript(_T33,sizeof(struct _fat_ptr),0);_T35=(struct _fat_ptr*)_T34;_T36=argv;_T37=_check_fat_subscript(_T36,sizeof(struct _fat_ptr),0);_T38=(struct _fat_ptr*)_T37;
*_T35=*_T38;*_T32=*_T35;
k=1;j=k;i=j;_TL76: if(i < argc)goto _TL74;else{goto _TL75;}
_TL74: _T39=bindices;_T3A=i;_T3B=_check_fat_subscript(_T39,sizeof(int),_T3A);_T3C=(int*)_T3B;_T3D=*_T3C;if(!_T3D)goto _TL77;_T3E=bargs;_T3F=j;j=_T3F + 1;_T40=_T3F;_T41=_check_fat_subscript(_T3E,sizeof(struct _fat_ptr),_T40);_T42=(struct _fat_ptr*)_T41;_T43=argv;_T44=_T43.curr;_T45=(struct _fat_ptr*)_T44;_T46=i;*_T42=_T45[_T46];goto _TL78;
_TL77: _T47=otherargs;_T48=k;k=_T48 + 1;_T49=_T48;_T4A=_check_fat_subscript(_T47,sizeof(struct _fat_ptr),_T49);_T4B=(struct _fat_ptr*)_T4A;_T4C=argv;_T4D=_T4C.curr;_T4E=(struct _fat_ptr*)_T4D;_T4F=i;*_T4B=_T4E[_T4F];_TL78:
# 268
 i=i + 1;goto _TL76;_TL75:
# 273
 Cyc_Arg_current=0;_T50=specs;_T51=anonfun;_T52=anonflagfun;_T53=errmsg;_T54=bargs;
Cyc_Arg_parse(_T50,_T51,_T52,_T53,_T54);_T55=Cyc_Specsfile_target_arch;_T56=_T55.curr;_T57=(char*)_T56;
# 278
if(_T57!=0)goto _TL79;{char*_T70=Carch;_T59=(void*)_T70;_T5A=(void*)_T70;_T5B=_get_zero_arr_size_char(_T5A,1U);_T58=_tag_fat(_T59,sizeof(char),_T5B);}Cyc_Specsfile_target_arch=_T58;goto _TL7A;_TL79: _TL7A:{char*_T70=
# 287 "specsfile.cyc"
getenv("CYCLONE_EXEC_PREFIX");_T5D=(void*)_T70;_T5E=(void*)_T70;_T5F=_get_zero_arr_size_char(_T5E,1U);_T5C=_tag_fat(_T5D,sizeof(char),_T5F);}{struct _fat_ptr cyclone_exec_prefix=_T5C;_T60=cyclone_exec_prefix;_T61=_T60.curr;_T62=(char*)_T61;
if(_T62==0)goto _TL7B;{struct Cyc_List_List*_T70=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T71=_cycalloc(sizeof(struct _fat_ptr));
*_T71=cyclone_exec_prefix;_T64=(struct _fat_ptr*)_T71;}_T70->hd=_T64;_T70->tl=Cyc_Specsfile_cyclone_exec_path;_T63=(struct Cyc_List_List*)_T70;}Cyc_Specsfile_cyclone_exec_path=_T63;goto _TL7C;_TL7B: _TL7C:{char*_T70=Cdef_lib_path;_T66=(void*)_T70;_T67=(void*)_T70;_T68=_get_zero_arr_size_char(_T67,1U);_T65=_tag_fat(_T66,sizeof(char),_T68);}
Cyc_Specsfile_def_lib_path=_T65;_T69=
Cyc_strlen(Cyc_Specsfile_def_lib_path);if(_T69 <= 0U)goto _TL7D;{struct Cyc_List_List*_T70=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T71=_cycalloc(sizeof(struct _fat_ptr));_T6C=Cyc_Specsfile_def_lib_path;_T6D=
# 293
_tag_fat("cyc-lib",sizeof(char),8U);_T6E=Cyc_Filename_concat(_T6C,_T6D);*_T71=_T6E;_T6B=(struct _fat_ptr*)_T71;}_T70->hd=_T6B;
_T70->tl=Cyc_Specsfile_cyclone_exec_path;_T6A=(struct Cyc_List_List*)_T70;}
# 292
Cyc_Specsfile_cyclone_exec_path=_T6A;goto _TL7E;_TL7D: _TL7E:
# 295
 Cyc_Specsfile_cyclone_exec_path=Cyc_List_imp_rev(Cyc_Specsfile_cyclone_exec_path);
Cyc_Specsfile_cyclone_arch_path=Cyc_Specsfile_also_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);_T6F=otherargs;
return _T6F;}}}}}}
# 301
static int Cyc_Specsfile_file_exists(struct _fat_ptr file){struct _handler_cons*_T0;int*_T1;int _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;void*_T5;
struct Cyc___cycFILE*f=0;{struct _handler_cons _T6;_T0=& _T6;_push_handler(_T0);{int _T7=0;_T1=_T6.handler;_T2=setjmp(_T1);if(!_T2)goto _TL7F;_T7=1;goto _TL80;_TL7F: _TL80: if(_T7)goto _TL81;else{goto _TL83;}_TL83: _T3=file;_T4=
_tag_fat("r",sizeof(char),2U);f=Cyc_file_open(_T3,_T4);_pop_handler();goto _TL82;_TL81: _T5=Cyc_Core_get_exn_thrown();{void*_T8=(void*)_T5;goto _LL0;_LL0:;}_TL82:;}}
if(f!=0)goto _TL84;return 0;
_TL84: Cyc_fclose(f);return 1;}
# 310
static struct _fat_ptr*Cyc_Specsfile_find(struct Cyc_List_List*dirs,struct _fat_ptr file){struct _fat_ptr _T0;unsigned char*_T1;char*_T2;struct Cyc_List_List*_T3;void*_T4;struct _fat_ptr*_T5;struct _fat_ptr _T6;unsigned char*_T7;char*_T8;unsigned long _T9;int _TA;struct _fat_ptr*_TB;struct Cyc_List_List*_TC;_T0=file;_T1=_T0.curr;_T2=(char*)_T1;
if(_T2!=0)goto _TL86;return 0;_TL86:
 _TL8B: if(dirs!=0)goto _TL89;else{goto _TL8A;}
_TL89: _T3=dirs;_T4=_T3->hd;_T5=(struct _fat_ptr*)_T4;{struct _fat_ptr dir=*_T5;_T6=dir;_T7=_T6.curr;_T8=(char*)_T7;
if(_T8==0)goto _TL8E;else{goto _TL8F;}_TL8F: _T9=Cyc_strlen(dir);if(_T9==0U)goto _TL8E;else{goto _TL8C;}_TL8E: goto _TL88;_TL8C: {
struct _fat_ptr s=Cyc_Filename_concat(dir,file);_TA=
Cyc_Specsfile_file_exists(s);if(!_TA)goto _TL90;{struct _fat_ptr*_TD=_cycalloc(sizeof(struct _fat_ptr));*_TD=s;_TB=(struct _fat_ptr*)_TD;}return _TB;_TL90:;}}_TL88: _TC=dirs;
# 312
dirs=_TC->tl;goto _TL8B;_TL8A:
# 318
 return 0;}
# 322
static struct _fat_ptr Cyc_Specsfile_sprint_list(struct Cyc_List_List*dirs){struct Cyc_List_List*_T0;void*_T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;unsigned char*_T4;char*_T5;unsigned long _T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_List_List*_T9;struct _fat_ptr _TA;
struct _fat_ptr tmp=_tag_fat("",sizeof(char),1U);
_TL95: if(dirs!=0)goto _TL93;else{goto _TL94;}
_TL93: _T0=dirs;_T1=_T0->hd;_T2=(struct _fat_ptr*)_T1;{struct _fat_ptr dir=*_T2;_T3=dir;_T4=_T3.curr;_T5=(char*)_T4;
if(_T5==0)goto _TL98;else{goto _TL99;}_TL99: _T6=Cyc_strlen(dir);if(_T6==0U)goto _TL98;else{goto _TL96;}_TL98: goto _TL92;_TL96: _T7=dir;_T8=
_tag_fat(":",sizeof(char),2U);dir=Cyc_strconcat(_T7,_T8);
tmp=Cyc_strconcat(dir,tmp);}_TL92: _T9=dirs;
# 324
dirs=_T9->tl;goto _TL95;_TL94: _TA=tmp;
# 330
return _TA;}
# 333
static struct _fat_ptr Cyc_Specsfile_do_find(struct Cyc_List_List*dirs,struct _fat_ptr file){struct Cyc_String_pa_PrintArg_struct _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct Cyc___cycFILE*_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr*_T6;struct _fat_ptr _T7;
struct _fat_ptr*f=Cyc_Specsfile_find(dirs,file);
if(f!=0)goto _TL9A;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;
# 337
_T8.f1=file;_T0=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T0;{struct Cyc_String_pa_PrintArg_struct _T9;_T9.tag=0;_T9.f1=Cyc_Specsfile_sprint_list(dirs);_T1=_T9;}{struct Cyc_String_pa_PrintArg_struct _T9=_T1;void*_TA[2];_TA[0]=& _T8;_TA[1]=& _T9;_T2=Cyc_stderr;_T3=
# 336
_tag_fat("Error: can't find internal compiler file %s in path %s\n",sizeof(char),56U);_T4=_tag_fat(_TA,sizeof(void*),2);Cyc_fprintf(_T2,_T3,_T4);}}
# 338
Cyc_fflush(Cyc_stderr);_T5=
_tag_fat(0,0,0);return _T5;_TL9A: _T6=f;_T7=*_T6;
# 341
return _T7;}
# 344
struct _fat_ptr Cyc_Specsfile_find_in_arch_path(struct _fat_ptr s){struct _fat_ptr _T0;_T0=
Cyc_Specsfile_do_find(Cyc_Specsfile_cyclone_arch_path,s);return _T0;}
# 348
struct _fat_ptr Cyc_Specsfile_find_in_exec_path(struct _fat_ptr s){struct _fat_ptr _T0;_T0=
Cyc_Specsfile_do_find(Cyc_Specsfile_cyclone_exec_path,s);return _T0;}
