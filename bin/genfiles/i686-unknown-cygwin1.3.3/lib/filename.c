// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#include <setjmp.h>

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
// should be size_t, but int is fine.
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

//// Tagged arrays
struct _tagged_arr { 
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// Need one of these per thread (we don't have threads)
// The runtime maintains a stack that contains either _handler_cons
// structs or _RegionHandle structs.  The tag is 0 for a handler_cons
// and 1 for a region handle.  
struct _RuntimeStack {
  int tag; // 0 for an exception handler, 1 for a region handle
  struct _RuntimeStack *next;
};

//// Regions
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
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);

//// Exceptions 
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

//// Built-in Exceptions
extern struct _xtunion_struct ADD_PREFIX(Null_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Null_Exception);
extern struct _xtunion_struct ADD_PREFIX(Array_bounds_struct);
extern struct _xtunion_struct * ADD_PREFIX(Array_bounds);
extern struct _xtunion_struct ADD_PREFIX(Match_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Match_Exception);
extern struct _xtunion_struct ADD_PREFIX(Bad_alloc_struct);
extern struct _xtunion_struct * ADD_PREFIX(Bad_alloc);

//// Built-in Run-time Checks and company
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};struct
_tagged_arr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _tagged_arr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct
_tagged_arr Cyc_Filename_concat(struct _tagged_arr,struct _tagged_arr);struct
_tagged_arr Cyc_Filename_chop_extension(struct _tagged_arr);struct _tagged_arr Cyc_Filename_dirname(
struct _tagged_arr);struct _tagged_arr Cyc_Filename_basename(struct _tagged_arr);int
Cyc_Filename_check_suffix(struct _tagged_arr,struct _tagged_arr);struct _tagged_arr
Cyc_Filename_gnuify(struct _tagged_arr);struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];extern char Cyc_List_Nth[8];struct
_tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct
_tagged_arr Cyc_Std_substring(struct _tagged_arr,int ofs,unsigned int n);struct
_tagged_arr Cyc_Filename_concat(struct _tagged_arr s1,struct _tagged_arr s2){return
Cyc_Std_strconcat(s1,(struct _tagged_arr)Cyc_Std_strconcat(_tag_arr("/",sizeof(
char),2),s2));}struct _tagged_arr Cyc_Filename_chop_extension(struct _tagged_arr
filename){int i=(int)(_get_arr_size(filename,sizeof(char))- 1);while(i >= 0?*((
const char*)_check_unknown_subscript(filename,sizeof(char),i))!= '.': 0){-- i;}if(i
< 0)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0[0]=({struct Cyc_Core_Invalid_argument_struct _tmp1;_tmp1.tag=
Cyc_Core_Invalid_argument;_tmp1.f1=_tag_arr("chop_extension",sizeof(char),15);
_tmp1;});_tmp0;}));return Cyc_Std_substring(filename,0,(unsigned int)i);}struct
_tagged_arr Cyc_Filename_dirname(struct _tagged_arr filename){int i=(int)(
_get_arr_size(filename,sizeof(char))- 1);while(i >= 0?*((const char*)
_check_unknown_subscript(filename,sizeof(char),i))!= '/': 0){-- i;}if(i < 0)return
Cyc_Core_new_string(0);return Cyc_Std_substring(filename,0,(unsigned int)i);}
struct _tagged_arr Cyc_Filename_basename(struct _tagged_arr filename){int i=(int)(
_get_arr_size(filename,sizeof(char))- 1);while(i >= 0?*((const char*)
_check_unknown_subscript(filename,sizeof(char),i))!= '/': 0){-- i;}return Cyc_Std_substring(
filename,i + 1,_get_arr_size(filename,sizeof(char))- (i + 1));}int Cyc_Filename_check_suffix(
struct _tagged_arr filename,struct _tagged_arr suffix){int i=(int)(_get_arr_size(
filename,sizeof(char))- 1);int j=(int)(_get_arr_size(suffix,sizeof(char))- 1);
while(i >= 0?j >= 0: 0){if(*((const char*)_check_unknown_subscript(filename,sizeof(
char),i --))!= *((const char*)_check_unknown_subscript(suffix,sizeof(char),j --)))
return 0;}if(j >= 0)return 0;else{return 1;}}struct _tagged_arr Cyc_Filename_gnuify(
struct _tagged_arr filename){int has_drive_name=_get_arr_size(filename,sizeof(char))
> 1?*((const char*)_check_unknown_subscript(filename,sizeof(char),1))== ':': 0;int
i;int j;struct _tagged_arr ans;int ans_sz;if(has_drive_name){ans_sz=(int)(
_get_arr_size(filename,sizeof(char))+ 1);ans=({unsigned int _tmp2=(unsigned int)
ans_sz;char*_tmp3=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp2));
struct _tagged_arr _tmp5=_tag_arr(_tmp3,sizeof(char),(unsigned int)ans_sz);{
unsigned int _tmp4=_tmp2;unsigned int k;for(k=0;k < _tmp4;k ++){_tmp3[k]='\000';}}
_tmp5;});*((char*)_check_unknown_subscript(ans,sizeof(char),0))=(*((char*)
_check_unknown_subscript(ans,sizeof(char),1))='/');*((char*)
_check_unknown_subscript(ans,sizeof(char),2))=*((const char*)
_check_unknown_subscript(filename,sizeof(char),0));i=3;j=2;}else{ans_sz=(int)
_get_arr_size(filename,sizeof(char));ans=({unsigned int _tmp6=(unsigned int)
ans_sz;char*_tmp7=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp6));
struct _tagged_arr _tmp9=_tag_arr(_tmp7,sizeof(char),(unsigned int)ans_sz);{
unsigned int _tmp8=_tmp6;unsigned int k;for(k=0;k < _tmp8;k ++){_tmp7[k]='\000';}}
_tmp9;});i=0;j=0;}while(i < ans_sz){char c=*((const char*)_check_unknown_subscript(
filename,sizeof(char),j ++));*((char*)_check_unknown_subscript(ans,sizeof(char),i
++))=c == '\\'?'/': c;}return ans;}
