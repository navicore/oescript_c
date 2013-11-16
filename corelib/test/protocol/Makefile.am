SUBDIRS = . json jcl

check_PROGRAMS = test_async_client

oe_ldadd = $(top_builddir)/corelib/liboecore.la @EVENT_LIBS@

oe_cflags = -I$(top_builddir)/corelib @OE_TESTER_CFLAGS@
oe_ldflags = @OE_TESTER_LDFLAGS@

test_async_client_SOURCES = test_async_client.c
test_async_client_LDADD = ${oe_ldadd}
test_async_client_CFLAGS = ${oe_cflags}
test_async_client_LDFLAGS = ${oe_ldflags}

TESTS = test_async_client

