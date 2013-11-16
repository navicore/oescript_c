SUBDIRS = . test

include_HEADERS = 

EXTRA_DIST = 

godir = ${bindir}
go_DATA = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) -I$(top_builddir)/modules/scripting/swig @OE_LIB_CFLAGS@ @GO_CFLAGS@
_libadd = $(__libadd) @EVENT_LIBS@ @GO_LDFLAGS@

lib_LTLIBRARIES = liboescript_go.la 

_liboescript_go_la_SOURCES = OeScript_go_wrap.c

liboescript_go_la_SOURCES = ${_liboescript_go_la_SOURCES}

liboescript_go_la_LIBADD = ${_libadd}
liboescript_go_la_CFLAGS = ${_cflags}
liboescript_go_la_LDFLAGS = @OE_LIB_LDFLAGS@

swig:
	swig -I${top_builddir}/corelib -Wall -go -o ./OeScript_go_wrap.c ../../OeScript.i

