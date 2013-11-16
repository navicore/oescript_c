check_PROGRAMS = 

EXTRA_DIST = script_runner test_swig_api.java

include_HEADERS = 

TESTS = script_runner

all-local:
	chmod +x script_runner
	cp ../oejavaimpl.jar .
	ln -f -s ../.libs/liboescript_java@OE_SHSUF@ ./liboescriptimpl@OE_SHSUF@
	CLASSPATH=oejavaimpl.jar:.
	@JAVAC@ -classpath oejavaimpl.jar:. -sourcepath . *.java
	mkdir -p data

clean-local:
	rm -f ./oejavaimpl.jar
	rm -f ./liboescriptimpl@OE_SHSUF@
	rm -f ./*.class
	rm -rf ./data
	rm -f ./mysigfile*
	rm -f ./db_error*

