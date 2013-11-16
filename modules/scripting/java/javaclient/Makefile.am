SUBDIRS = . test

all-local:
	mkdir -p out
	CLASSPATH=.
	@JAVAC@ -sourcepath src/java src/java/org/oescript/util/*.java -d out
	@JAVAC@ -sourcepath src/java src/java/org/oescript/jsonsimple/*.java -d out
	@JAVAC@ -sourcepath src/java src/java/org/oescript/jsonsimple/parser/*.java -d out
	@JAVAC@ -sourcepath src/java src/java/org/oescript/msg/*.java -d out
	@JAVAC@ -sourcepath src/java src/java/org/oescript/api/*.java -d out
	@JAVAC@ -sourcepath src/java src/java/org/oescript/javaclient/*.java -d out

	jar cvf oejava.jar -C out .

clean-local:
	rm -f oejava.jar
	rm -rf out

docdir = $(datadir)/doc/
doc_DATA = json_simple_LICENSE.txt

