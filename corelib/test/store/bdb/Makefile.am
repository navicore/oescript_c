check_PROGRAMS = test_db test_db_tuple test_db_threads test_db_txn test_db_lease test_holder_marshall
oe_ldadd = $(top_builddir)/corelib/liboecore.la @EVENT_LIBS@ @DB_LIBS@
oe_cflags = -I$(top_srcdir)/corelib @OE_LIB_CFLAGS@

test_db_SOURCES = test_db.c
test_db_LDADD = ${oe_ldadd}
test_db_CFLAGS = ${oe_cflags}
test_db_LDFLAGS = @OE_TESTER_LDFLAGS@

test_db_tuple_SOURCES = test_db_tuple.c
test_db_tuple_LDADD = ${oe_ldadd}
test_db_tuple_CFLAGS = ${oe_cflags}
test_db_tuple_LDFLAGS = @OE_TESTER_LDFLAGS@

test_db_threads_SOURCES = test_db_threads.c
test_db_threads_LDADD = ${oe_ldadd}
test_db_threads_CFLAGS = ${oe_cflags}
test_db_threads_LDFLAGS = @OE_TESTER_LDFLAGS@

test_db_txn_SOURCES = test_db_txn.c
test_db_txn_LDADD = ${oe_ldadd}
test_db_txn_CFLAGS = ${oe_cflags}
test_db_txn_LDFLAGS = @OE_TESTER_LDFLAGS@

test_db_lease_SOURCES = test_db_lease.c
test_db_lease_LDADD = ${oe_ldadd}
test_db_lease_CFLAGS = ${oe_cflags}
test_db_lease_LDFLAGS = @OE_TESTER_LDFLAGS@

test_holder_marshall_SOURCES = test_holder_marshall.c
test_holder_marshall_LDADD = ${oe_ldadd}
test_holder_marshall_CFLAGS = ${oe_cflags}
test_holder_marshall_LDFLAGS = @OE_TESTER_LDFLAGS@

TESTS = test_db test_db_tuple test_db_threads test_db_txn test_db_lease test_holder_marshall

all-local:
	mkdir -p data

clean-local:
	rm -rf data
	rm -rf __db.*
	rm -rf log.0*

