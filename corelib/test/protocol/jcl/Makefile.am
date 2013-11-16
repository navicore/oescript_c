check_PROGRAMS = test_jcl_server test_run_jcl_server

oe_ldadd = $(top_builddir)/corelib/liboecore.la @EVENT_LIBS@ @DB_LIBS@

oe_cflags = -I$(top_srcdir)/corelib @OE_TESTER_CFLAGS@

oe_ldflags = @OE_TESTER_LDFLAGS@

test_jcl_server_SOURCES = test_jcl_server.c
test_jcl_server_LDADD = ${oe_ldadd}
test_jcl_server_CFLAGS = ${oe_cflags}
test_jcl_server_LDFLAGS = ${oe_ldflags}

test_run_jcl_server_SOURCES = test_run_jcl_server.c
test_run_jcl_server_LDADD = ${oe_ldadd}
test_run_jcl_server_CFLAGS = ${oe_cflags}
test_run_jcl_server_LDFLAGS = ${oe_ldflags}

TESTS = test_jcl_server

