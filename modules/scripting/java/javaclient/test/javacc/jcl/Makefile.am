
SUBDIRS = .

check_PROGRAMS = 

EXTRA_DIST = script_runner 

include_HEADERS = 

TESTS = script_runner
CLASSPATH=.

all-local:
	chmod +x script_runner
	@JAVAC@ -classpath ${CLASSPATH} -sourcepath . *.java

javacc:
	${HOME}/javacc-5.0/bin/javacc jcl.jj


clean-local:
	rm -f *.class

