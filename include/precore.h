// This is a Cyclone include file, but the actual definitions are
// made in C code.  So, this file is used to automatically produce
// a C include file, for use with the C definitions.

#ifndef _PRECORE_H_
#define _PRECORE_H_

#ifndef _CYC_GENERATE_PRECORE_C_
#include <types.h>
#endif 

typedef char *{0} Cstring;
// a boxed and tagged string: struct {unsigned int sz; Cstring *contents;}@
typedef const char ? string_t;
typedef char ? mstring_t;

typedef string_t @stringptr_t;
typedef mstring_t @mstringptr_t;

#ifndef bool
typedef int bool;
#endif
#ifndef false 
#define false (0)
#endif
#ifndef true
#define true (1)
#endif
extern "C" void exit(int) __attribute__((noreturn)) ;
extern "C" `a abort() __attribute__((noreturn));

#endif /* _PRECORE_H_ */