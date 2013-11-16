SUBDIRS = . test

include_HEADERS = 

EXTRA_DIST = 

javadir = ${bindir}
java_DATA = 

include $(top_builddir)/modules/scripting/swig/mfilepaths.mk

_cflags = $(__cflags) @OE_LIB_CFLAGS@ @JNI_CFLAGS@
_libadd = $(__libadd)

oe_cflags=${_cflags}
oe_libadd=${_libadd}

lib_LTLIBRARIES = liboescript_java.la 

_liboescript_java_la_SOURCES = OeScript_java_wrap.c

liboescript_java_la_SOURCES = ${_liboescript_java_la_SOURCES}

liboescript_java_la_LIBADD = ${oe_libadd}
liboescript_java_la_CFLAGS = ${oe_cflags}
liboescript_java_la_LDFLAGS = @OE_LIB_LDFLAGS@


all-local:
	mkdir -p out
	CLASSPATH=.
	@JAVAC@ -sourcepath src/java src/java/org/oescript/java/swig/*.java -d out
	jar cvf oejavaimpl.jar -C out .

clean-local:
	rm -f oe*.jar
	rm -rf out

swig:
	mkdir -p src/java/org/oescript/java/swig
	swig -I${top_builddir}/corelib -Wall -java -package org.oescript.java.swig -outdir src/java/org/oescript/java/swig -o ./OeScript_java_wrap.c ../OeScript.i

