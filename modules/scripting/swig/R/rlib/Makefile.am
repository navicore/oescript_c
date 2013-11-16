SUBDIRS = . test

include_HEADERS = 

EXTRA_DIST = 

Rdir = ${bindir}
R_DATA = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) @OE_LIB_CFLAGS@ @R_CFLAGS@
_libadd = $(__libadd) @R_LDFLAGS@

lib_LTLIBRARIES = liboescript_R.la 

_liboescript_R_la_SOURCES = OeScript_R_wrap.c

liboescript_R_la_SOURCES = ${_liboescript_R_la_SOURCES}

liboescript_R_la_LIBADD = ${_libadd}
liboescript_R_la_CFLAGS = ${_cflags}
liboescript_R_la_LDFLAGS = @OE_LIB_LDFLAGS@

swig:
	R_VERSION="2.10.0" swig -I${top_builddir}/corelib -Wall -r -o ./OeScript_R_wrap.c ../../OeScript.i

#this hook nessesary until R pkg and can be sanely supported
Rapiscriptdir = ${datarootdir}/R/bin
Rapiscript_DATA = oescriptimpl.R

install-data-hook:
	mkdir -p ${Rapiscriptdir}
	ln -f -s ${libdir}/liboescript_R.so ${Rapiscriptdir}/oescriptimpl.so

