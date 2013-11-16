SUBDIRS = . nonreentrant reentrant

include_HEADERS =

oe_ldadd = ${top_srcdir}/corelib/liboecore.la 
oe_cflags = -I$(top_srcdir)/corelib @OE_TESTER_CFLAGS@

check_PROGRAMS = test_oejcl test_decoder test_encoder test_batch

test_oejcl_SOURCES = test_oejcl.c
test_oejcl_LDADD = ${oe_ldadd}
test_oejcl_CFLAGS = ${oe_cflags}
test_oejcl_LDFLAGS = @OE_TESTER_LDFLAGS@

test_decoder_SOURCES = test_decoder.c
test_decoder_LDADD = ${oe_ldadd}
test_decoder_CFLAGS = ${oe_cflags}
test_decoder_LDFLAGS = @OE_TESTER_LDFLAGS@

test_encoder_SOURCES = test_encoder.c
test_encoder_LDADD = ${oe_ldadd}
test_encoder_CFLAGS = ${oe_cflags}
test_encoder_LDFLAGS = @OE_TESTER_LDFLAGS@

test_batch_SOURCES = test_batch.c
test_batch_LDADD = ${oe_ldadd}
test_batch_CFLAGS = ${oe_cflags}
test_batch_LDFLAGS = @OE_TESTER_LDFLAGS@

TESTS = test_oejcl test_decoder test_encoder test_batch

