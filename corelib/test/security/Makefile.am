if OE_USE_PAM
SUBDIRS = . pam
else
SUBDIRS = .
endif

check_PROGRAMS = test_security_api

oe_ldadd = $(top_builddir)/corelib/liboecore.la

oe_cflags = -I$(top_srcdir)/corelib @OE_TESTER_CFLAGS@

oe_ldflags = @OE_TESTER_LDFLAGS@

test_security_api_SOURCES = test_security_api.c
test_security_api_LDADD = ${oe_ldadd}
test_security_api_CFLAGS = ${oe_cflags}
test_security_api_LDFLAGS = ${oe_ldflags}

TESTS = test_security_api

