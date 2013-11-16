SUBDIRS = . test

include_HEADERS = 

EXTRA_DIST = 

pythondir = ${bindir}
python_DATA = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) @OE_LIB_CFLAGS@ @PYTHON_CPPFLAGS@
_libadd = $(__libadd) @PYTHON_LDFLAGS@ @PYTHON_EXTRA_LDFLAGS@ @PYTHON_EXTRA_LIBS@

lib_LTLIBRARIES = liboescript_python.la 

liboescript_python_la_SOURCES = OeScript_python_wrap.c

liboescript_python_la_LIBADD = ${_libadd}
liboescript_python_la_CFLAGS = ${_cflags}
liboescript_python_la_LDFLAGS = @OE_LIB_LDFLAGS@ -module

swig:
	swig -I${top_builddir}/corelib -Wall -python -o ./OeScript_python_wrap.c ../../OeScript.i

#pythonapiscriptdir = ${datarootdir}/python/bin
#pythonapiscript_DATA = mod/lib/oescript.py oescriptimpl.py

#install-data-hook:
#	ln -f -s ${libdir}/liboescript_python.so ${pythonapiscriptdir}/_oescriptimpl.so

