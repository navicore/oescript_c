SUBDIRS = .

noinst_PROGRAMS = runme

runme_SOURCES = lex.yy.c runme.tab.c runme.tab.h
BUILT_SOURCES = lex.yy.c runme.tab.c runme.tab.h
CLEANFILES = lex.yy.c runme.tab.c runme.tab.h runme.output
runme_LDFLAGS = @OE_TESTER_LDFLAGS@

lex.yy.c:	Makefile prepare
runme.tab.c:	Makefile prepare
prepare:	runme.l runme.y
	bison -v -d runme.y
	flex runme.l

