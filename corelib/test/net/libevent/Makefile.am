check_PROGRAMS = test_net test_net_ssl
oe_libadd = $(top_builddir)/corelib/liboecore.la @EVENT_LIBS@ 
oe_cflags = -I$(top_builddir)/corelib @OE_LIB_CFLAGS@

test_net_SOURCES = test_net.c
test_net_LDADD = ${oe_libadd}
test_net_CFLAGS = ${oe_cflags}
test_net_LDFLAGS = @OE_TESTER_LDFLAGS@

test_net_ssl_SOURCES = test_net_ssl.c
test_net_ssl_LDADD = ${oe_libadd}
test_net_ssl_CFLAGS = ${oe_cflags}
test_net_ssl_LDFLAGS = @OE_TESTER_LDFLAGS@

TESTS = test_net test_net_ssl

