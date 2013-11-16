check_PROGRAMS = test_json_server test_run_json_server

oe_ldadd = $(top_builddir)/corelib/liboecore.la @EVENT_LIBS@ @DB_LIBS@

oe_cflags = -I$(top_srcdir)/corelib @OE_TESTER_CFLAGS@

oe_ldflags = @OE_TESTER_LDFLAGS@

test_json_server_SOURCES = test_json_server.c
test_json_server_LDADD = ${oe_ldadd}
test_json_server_CFLAGS = ${oe_cflags}
test_json_server_LDFLAGS = ${oe_ldflags}

test_run_json_server_SOURCES = test_run_json_server.c
test_run_json_server_LDADD = ${oe_ldadd}
test_run_json_server_CFLAGS = ${oe_cflags}
test_run_json_server_LDFLAGS = ${oe_ldflags}

TESTS = test_json_server

