SUBDIRS = . test

include_HEADERS = 

EXTRA_DIST = 

guiledir = ${bindir}
guile_DATA = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) @OE_LIB_CFLAGS@ @GUILE_CFLAGS@
_libadd = $(__libadd) @GUILE_LDFLAGS@

lib_LTLIBRARIES = liboescript_guile.la 

_liboescript_guile_la_SOURCES = OeScript_guile_wrap.c

liboescript_guile_la_SOURCES = ${_liboescript_guile_la_SOURCES}

liboescript_guile_la_LIBADD = ${_libadd}
liboescript_guile_la_CFLAGS = ${_cflags}
liboescript_guile_la_LDFLAGS = @OE_LIB_LDFLAGS@

swig:
	swig -I${top_builddir}/corelib -Wall -guile -scm -Linkage 'module' -package onextent/lib -o ./OeScript_guile_wrap.c ../OeScript.i

