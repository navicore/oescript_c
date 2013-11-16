check_PROGRAMS = test_pam test_pam_module

oe_ldadd = $(top_builddir)/corelib/liboecore.la

oe_cflags = -I$(top_srcdir)/corelib @OE_TESTER_CFLAGS@

oe_ldflags = @OE_TESTER_LDFLAGS@

test_pam_SOURCES = test_pam.c
test_pam_LDADD = ${oe_ldadd}
test_pam_CFLAGS = ${oe_cflags}
test_pam_LDFLAGS = ${oe_ldflags}

test_pam_module_SOURCES = test_pam_module.c
test_pam_module_LDADD = ${oe_ldadd}
test_pam_module_CFLAGS = ${oe_cflags}
test_pam_module_LDFLAGS = ${oe_ldflags}

TESTS = test_pam test_pam_module

