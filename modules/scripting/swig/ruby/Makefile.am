SUBDIRS = . test

include_HEADERS = 

EXTRA_DIST = 

rubydir = ${bindir}
ruby_DATA = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) -I$(top_builddir)/modules/scripting/swig @OE_LIB_CFLAGS@ @RUBY_CPPFLAGS@

_libadd = $(__libadd) @RUBY_LDFLAGS@ @RUBY_EXTRA_LIBS@

lib_LTLIBRARIES = liboescript_ruby.la 

_liboescript_ruby_la_SOURCES = OeScript_ruby_wrap.c

liboescript_ruby_la_SOURCES = ${_liboescript_ruby_la_SOURCES}

liboescript_ruby_la_LIBADD = ${_libadd}
liboescript_ruby_la_CFLAGS = ${_cflags}
liboescript_ruby_la_LDFLAGS = @OE_LIB_LDFLAGS@

swig:
	swig -I${top_builddir}/corelib -Wall -ruby -o ./OeScript_ruby_wrap.c ../OeScript.i

