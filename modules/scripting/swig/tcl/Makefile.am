SUBDIRS = . test

include_HEADERS = 

EXTRA_DIST = 

tcldir = ${bindir}
tcl_DATA = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) -I$(top_builddir)/modules/scripting/swig @OE_LIB_CFLAGS@ @OE_TCL_CPPFLAGS@
_libadd = $(__libadd) @EVENT_LIBS@ @OE_TCL_LIBS@

lib_LTLIBRARIES = liboescript_tcl.la 

_liboescript_tcl_la_SOURCES = OeScript_tcl_wrap.c

liboescript_tcl_la_SOURCES = ${_liboescript_tcl_la_SOURCES}

liboescript_tcl_la_LIBADD = ${_libadd}
liboescript_tcl_la_CFLAGS = ${_cflags}
liboescript_tcl_la_LDFLAGS = @OE_LIB_LDFLAGS@ @OE_TCL_CPPFLAGS@ -module

swig:
	swig -I${top_builddir}/corelib -Wall -tcl -o ./OeScript_tcl_wrap.c ../OeScript.i

tclapiscriptdir = ${datarootdir}/tcl/bin
#tclapiscript_DATA = mod/lib/oescript.tcl oescriptimpl.tcl
tclapiscript_DATA = mod/lib/oescript.tcl

install-data-hook:
	ln -f -s ${libdir}/liboescript_tcl.so ${tclapiscriptdir}/oescriptimpl.so

