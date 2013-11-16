check_PROGRAMS = test_sqlite
oe_ldadd = $(top_builddir)/corelib/liboecore.la @EVENT_LIBS@ @SQLITE_LIBS@
oe_cflags = -I$(top_srcdir)/corelib @OE_LIB_CFLAGS@

test_sqlite_SOURCES = test_sqlite.c
test_sqlite_LDADD = ${oe_ldadd}
test_sqlite_CFLAGS = ${oe_cflags}
test_sqlite_LDFLAGS = @OE_TESTER_LDFLAGS@

TESTS = test_sqlite

all-local:
	mkdir -p data

clean-local:
	rm -rf data

