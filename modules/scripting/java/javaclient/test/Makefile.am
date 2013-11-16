
SUBDIRS = . antlr javacc

check_PROGRAMS = 

EXTRA_DIST = script_runner 

include_HEADERS = 

TESTS = script_runner

all-local:
	chmod +x script_runner
	cp ../../../swig/java/oejavaimpl.jar .
	ln -f -s ../../../swig/java/.libs/liboescript_java@OE_SHSUF@ ./liboescriptimpl@OE_SHSUF@
	cp ../oejava.jar .
	CLASSPATH=oejava.jar:oejavaimpl.jar:.
	@JAVAC@ -classpath oejava.jar:oejavaimpl.jar:. -sourcepath . *.java
	mkdir -p data

clean-local:
	rm -f oe*.jar
	rm -f ./liboescriptimpl@OE_SHSUF@
	rm -f *.class
	rm -rf ./data
	rm -f ./mysigfile*
	rm -f ./db_error*

