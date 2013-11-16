SUBDIRS = . test

include_HEADERS = 

EXTRA_DIST = 

phpdir = ${bindir}
php_DATA = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) @OE_LIB_CFLAGS@ @PHP_CFLAGS@

_libadd = $(__libadd)

lib_LTLIBRARIES = liboescript_php.la 

_liboescript_php_la_SOURCES = OeScript_php_wrap.c

liboescript_php_la_SOURCES = ${_liboescript_php_la_SOURCES}

liboescript_php_la_LIBADD = ${_libadd}
liboescript_php_la_CFLAGS = ${_cflags}
liboescript_php_la_LDFLAGS = @OE_LIB_LDFLAGS@

swig:
	swig -I${top_builddir}/corelib -Wall -php -o ./OeScript_php_wrap.c ../OeScript.i

