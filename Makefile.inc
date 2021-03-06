# Makefile.inc.  Generated from Makefile.inc.in by configure.
VERSION=1.0
build=i686-pc-linux-gnu
target=i686-pc-linux-gnu
CC=gcc
CFLAGS=-march=i686
TARGET_CC=gcc -b i686-pc-linux-gnu
TARGET_CFLAGS=-march=i686
CYCDIR=/home/kjam/cyclone
prefix=/usr/local
exec_prefix=${prefix}
INC_INSTALL=${prefix}/include/cyclone
BIN_INSTALL=${exec_prefix}/bin
LIB_INSTALL=${exec_prefix}/lib/cyclone

RANLIB=ranlib
TARGET_RANLIB=ranlib
TARGET_AR=ar
EXE=
O=o
LDFLAGS=
SIZEOF_WCHAR_T=4
WCHAR_T_UNSIGNED=0
BANSHEE=/home/kjam/cyclone/banshee
BANSHEEENGINEDIR=$(BANSHEE)/engine
BANSHEEREGIONDIR=$(BANSHEE)/libcompat

HAVE_PTHREAD=yes
HAVE_PCRE=no
LIBPCRE=
HAVE_SQLITE3=no
LIBSQLITE3=
HAVE_SSL=no
LIBSSL=

SHELL=/bin/sh
A=a
# What we do when using VC++
ifdef MS
  O=obj
  A=lib
endif

# needed for booting
CYCLONE_BOOT_LIBS:=ap\
        apq\
        arg\
        assert\
        buffer\
        core\
        dict\
        filename\
        fn\
        graph\
        hashtable\
	iter\
        lexing\
        lineno\
        list\
        position\
        pp\
	printf\
	scanf\
        set\
        sexp\
        slowdict\
        string\
        xarray\
        xp

# other bits of the standard library
CYCLONE_LIBS:=array\
        base64\
        bitvec\
	execinfo\
        getopt\
        getopt1\
        queue\
        rope\
        splay\
	tmp

CYCLONE_H:=ap.h\
        apq.h\
	arg.h\
	array.h\
	assert.h\
        base64.h\
	binding.h\
	bitvec.h\
	buffer.h\
        callgraph.h\
	core.h\
	currgn.h\
	crypt.h\
	dict.h\
	execinfo.h\
	filename.h\
	fn.h\
	gc_ifc.h\
	getopt.h\
	hashtable.h\
	interproc.h\
	bansheeif.h\
	iter.h\
	lexing.h\
	lineno.h\
	list.h\
	position.h\
	pp.h\
	queue.h\
	rope.h\
	set.h\
        sexp.h\
	slowdict.h\
	splay.h\
	string.h\
	strings.h\
	sys/cioctl.h\
	sys/ioctl.h\
	xarray.h\
	xp.h\
	warn.h\
        flags.h\
        lower.h\
	subtype.h


#need this in CYCLONE_SRCS
BANSHEE_IF:=bansheeif
BANSHEE_STUB:=bansheeif_stub

CYCLONE_SRCS:=absyn\
	 warn\
         flags\
         kinds\
         parse_tab\
         parse_errors\
         lex\
         absynpp\
	 absyndump\
         tcenv\
	 unify\
	 attributes\
         tcutil\
         tctyp\
         tcstmt\
         tcpat\
         tcexp\
	 formatstr\
         evexp\
         callgraph\
         tc\
	 currgn\
         binding\
	 interproc\
         jump_analysis\
         cf_flowinfo\
         new_control_flow\
         assndef\
         pratt_prover\
         vcgen\
	 insert_checks\
         toc\
	 remove_aggregates\
	 remove_labels\
         toseqc\
         tovc\
	 low_treeshake\
	 rename_temps\
	 remove_casts\
         interface\
	 subtype\
	 cifc\
	 tcdecl\
         lower\
         port\
         specsfile\
	 cyclone

CYCDOC_SRCS:=cycdoc\
	warn\
        flags\
        parse_errors\
	lex\
	parse_tab\
	absyn\
	unify\
        attributes\
	tcutil\
	kinds\
	tctyp\
	tcenv\
	tcexp\
	currgn\
        binding\
	tcstmt\
	tcpat\
        callgraph\
        tc\
	toc\
	cifc\
	tcdecl\
        jump_analysis\
	new_control_flow\
        assndef\
        pratt_prover\
        vcgen\
	insert_checks\
	cf_flowinfo\
	formatstr\
	evexp\
	subtype\
	absynpp\
	$(BANSHEE_STUB)

BUILDLIB_SRCS:=buildlib\
        specsfile\
	warn\
        flags\
        parse_errors\
	lex\
	kinds\
	parse_tab\
	unify\
        attributes\
	tcutil\
	tctyp\
	absynpp\
        callgraph\
	tc\
	cifc\
	tcdecl\
	tcexp\
	currgn\
        binding\
	tcstmt\
	formatstr\
	evexp\
	tcenv\
	tcpat\
        jump_analysis\
	new_control_flow\
	assndef\
        pratt_prover\
        vcgen\
	insert_checks\
	cf_flowinfo\
        toc\
	subtype\
	absyn\
	$(BANSHEE_STUB)

# We can't optimize these files because the use try/catch and GCC 4.x
# and our implementation of try/catch do not get along.
NOOPT_SRCS:= parse_tab parse_tab_a parse_tab_pg pratt_prover binding callgraph cifc cyclone interface tc tcenv tcexp tcpat tctyp tcutil toc unify

# Add XML library .h files to CYCLONE_H
ifndef NO_XML_LIB
CYCLONE_H := $(CYCLONE_H) xml/xml.h xml/xmldump.h xml/xmlparse.h
endif

# Files for the runtime system
RUNTIME:=runtime_cyc \
	runtime_stack \
	runtime_exception \
	runtime_zeroterm \
	runtime_memory \
        bget

CYCLIB:=libcyc.$(A)
CYCBOOTLIB:=libcycboot.$(A)
CYCCOMP:=cyclone$(EXE)
CYCINFCOMP:=cyclone-inf$(EXE)

C_SRCS:=$(addsuffix .c, $(CYCLONE_SRCS) $(BANSHEE_STUB) $(BANSHEE_IF))
C_BOOT_LIBS:=$(addsuffix .c, $(CYCLONE_BOOT_LIBS))
C_LIBS:=$(addsuffix .c, $(CYCLONE_LIBS))
O_SRCS:=$(addsuffix .$(O), $(CYCLONE_SRCS) $(BANSHEE_STUB))
BANSHEE_O_SRCS:=$(addsuffix .$(O), $(CYCLONE_SRCS) $(BANSHEE_IF))
ALL_O_SRCS:=$(addsuffix .$(O), $(CYCLONE_SRCS) $(BANSHEE_STUB) $(BANSHEE_IF))
O_BOOT_LIBS:=$(addsuffix .$(O), $(CYCLONE_BOOT_LIBS))
O_LIBS:=$(addsuffix .$(O), $(CYCLONE_LIBS))
A_SRCS:=$(addsuffix _a.$(O), $(CYCLONE_SRCS) $(BANSHEE_STUB))
A_BOOT_LIBS:=$(addsuffix _a.$(O), $(CYCLONE_BOOT_LIBS))
A_LIBS:=$(addsuffix _a.$(O), $(CYCLONE_LIBS))
PG_SRCS:=$(addsuffix _pg.$(O), $(CYCLONE_SRCS) $(BANSHEE_STUB))
PG_BOOT_LIBS:=$(addsuffix _pg.$(O), $(CYCLONE_BOOT_LIBS))
PG_LIBS:=$(addsuffix _pg.$(O), $(CYCLONE_LIBS))
NOCHECK_BOOT_LIBS:=$(addsuffix _nocheck.$(O), $(CYCLONE_BOOT_LIBS))
NOCHECK_LIBS:=$(addsuffix _nocheck.$(O), $(CYCLONE_LIBS))
C_RUNTIME:=$(addsuffix .c, $(RUNTIME))
O_RUNTIME:=$(addsuffix .$(O), $(RUNTIME))
A_RUNTIME:=$(addsuffix _a.$(O), $(RUNTIME))
PG_RUNTIME:=$(addsuffix _pg.$(O), $(RUNTIME))
O_NOOPT_SRCS:=$(addsuffix .$(O),$(NOOPT_SRCS))
OPTFLAG:=-O

UPDATE_SRCS:=$(addsuffix .c, $(CYCLONE_SRCS) $(BANSHEE_STUB) $(BANSHEE_IF) cycdoc buildlib)
