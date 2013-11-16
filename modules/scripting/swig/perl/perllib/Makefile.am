SUBDIRS = .
#note, tests of this shared lib are done by the perl module 
#  package ../oescript/t/ test suite

include_HEADERS = 

EXTRA_DIST = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) @OE_LIB_CFLAGS@ @PERLXS_CFLAGS@
_libadd = $(__libadd) @PERLXS_LDFLAGS@

lib_LTLIBRARIES = liboescript_perl.la 

liboescript_perl_la_SOURCES = OeScript_perl5_wrap.c
liboescript_perl_la_LIBADD = ${_libadd}
liboescript_perl_la_CFLAGS = ${_cflags}
liboescript_perl_la_LDFLAGS = @OE_LIB_LDFLAGS@ -module

swig:
	swig -I${top_builddir}/corelib -Wall -perl5 -o ./OeScript_perl5_wrap.c ../../OeScript.i

#perlapiscriptdir = ${datarootdir}/perl/bin
#perlapiscript_DATA = mod/lib/oescript.pm oescriptimpl.pm

#install-data-hook:
#	ln -f -s ${libdir}/liboescript_perl.so ${perlapiscriptdir}/oescriptimpl.so

