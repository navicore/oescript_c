SUBDIRS = . test

include_HEADERS = 

EXTRA_DIST = 

clispdir = ${bindir}
clisp_DATA = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) @OE_LIB_CFLAGS@ @CLISP_CFLAGS@
_libadd = $(__libadd) @CLISP_LIBS@

oe_cflags=${_cflags}
oe_libadd=${_libadd}

lib_LTLIBRARIES = liboescript_clisp.la 

_liboescript_clisp_la_SOURCES = OeScript_clisp_wrap.c

liboescript_clisp_la_SOURCES = ${_liboescript_clisp_la_SOURCES}

liboescript_clisp_la_LIBADD = ${oe_libadd}
liboescript_clisp_la_CFLAGS = ${oe_cflags}
liboescript_clisp_la_LDFLAGS = @OE_LIB_LDFLAGS@

swig:
	swig -I${top_builddir}/corelib -Wall -clisp -o ./OeScript_clisp_wrap.c ../OeScript.i

