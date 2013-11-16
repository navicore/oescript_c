SUBDIRS = . test

include_HEADERS = 

EXTRA_DIST = 

luadir = ${bindir}
lua_DATA = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) @LUA_CFLAGS@ @OE_LIB_CFLAGS@
_libadd = $(__libadd) @LUA_LIBS@

_liboescript_lua_la_SOURCES = OeScript_lua_wrap.c

liboescript_lua_la_SOURCES = ${_liboescript_lua_la_SOURCES}
oe_cflags=${_cflags}
oe_libadd=${_libadd}

lib_LTLIBRARIES = liboescript_lua.la 

liboescript_lua_la_LIBADD = ${oe_libadd}
liboescript_lua_la_CFLAGS = ${oe_cflags}
liboescript_lua_la_LDFLAGS = @OE_LIB_LDFLAGS@ -module

swig:
	swig -I${top_builddir}/corelib -Wall -lua -o ./OeScript_lua_wrap.c ../../OeScript.i

luaapiscriptdir = ${datarootdir}/lua/bin

uninstall-local:
	rm ${luaapiscriptdir}/oescriptimpl.so

install-data-hook:
	mkdir -p ${luaapiscriptdir}
	ln -f -s ${libdir}/liboescript_lua.so ${luaapiscriptdir}/oescriptimpl.so

