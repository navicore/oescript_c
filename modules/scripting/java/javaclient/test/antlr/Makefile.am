
SUBDIRS = .

check_PROGRAMS = 

EXTRA_DIST = script_runner 

include_HEADERS = 

TESTS = script_runner
CLASSPATH=lib/antlr-3.4-complete-no-antlrv2.jar:.

all-local:
	chmod +x script_runner
	@JAVA@ -classpath ${CLASSPATH} org.antlr.Tool List.g
	@JAVAC@ -classpath ${CLASSPATH} -sourcepath . *.java

clean-local:
	rm -f *.class
	rm -f List.tokens
	rm -f ListLexer.java
	rm -f ListParser.java

