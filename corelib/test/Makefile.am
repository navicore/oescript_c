SUBDIRS = . net security protocol store jcl

oe_ldadd = ${top_srcdir}/corelib/liboecore.la @EVENT_LIBS@
oe_cflags = -I$(top_srcdir)/corelib @OE_TESTER_CFLAGS@

include_HEADERS = 

check_PROGRAMS = test_libevent test_timer_dispatcher test_thread_dispatcher test_nothreads_dispatcher test_group_dispatcher test_kernel test_oek_msg test_oecommon test_except test_oec_values test_store_holder test_json_serialize test_json_deserialize test_json_kernel test_sighandler
#check_PROGRAMS = test_libevent test_timer_dispatcher test_thread_dispatcher test_nothreads_dispatcher test_group_dispatcher test_kernel test_oek_msg test_oecommon test_except test_oec_values test_store_holder test_json_serialize test_json_deserialize test_json_kernel test_sighandler test_dataobject

#test_dataobject_SOURCES = test_dataobject.c
#test_dataobject_LDADD = ${oe_ldadd}
#test_dataobject_CFLAGS = ${oe_cflags}
#test_dataobject_LDFLAGS = @OE_TESTER_LDFLAGS@

test_libevent_SOURCES = test_libevent.c
test_libevent_LDADD = ${oe_ldadd}
test_libevent_CFLAGS = ${oe_cflags}
test_libevent_LDFLAGS = @OE_TESTER_LDFLAGS@

test_timer_dispatcher_SOURCES = test_timer_dispatcher.c sn_handler.c
test_timer_dispatcher_LDADD = ${oe_ldadd}
test_timer_dispatcher_CFLAGS = ${oe_cflags} 
test_timer_dispatcher_LDFLAGS = @OE_TESTER_LDFLAGS@ 

test_group_dispatcher_SOURCES = test_group_dispatcher.c sn_handler.c
test_group_dispatcher_LDADD = ${oe_ldadd}
test_group_dispatcher_CFLAGS = ${oe_cflags}
test_group_dispatcher_LDFLAGS = @OE_TESTER_LDFLAGS@ 

test_thread_dispatcher_SOURCES = test_thread_dispatcher.c
test_thread_dispatcher_LDADD = ${oe_ldadd}
test_thread_dispatcher_CFLAGS = ${oe_cflags}
test_thread_dispatcher_LDFLAGS = @OE_TESTER_LDFLAGS@

test_nothreads_dispatcher_SOURCES = test_nothreads_dispatcher.c
test_nothreads_dispatcher_LDADD = ${oe_ldadd}
test_nothreads_dispatcher_CFLAGS = ${oe_cflags}
test_nothreads_dispatcher_LDFLAGS = @OE_TESTER_LDFLAGS@

test_kernel_SOURCES = test_kernel.c
test_kernel_LDADD = ${oe_ldadd}
test_kernel_CFLAGS = ${oe_cflags}
test_kernel_LDFLAGS = @OE_TESTER_LDFLAGS@

test_oek_msg_SOURCES = test_oek_msg.c
test_oek_msg_LDADD = ${oe_ldadd}
test_oek_msg_CFLAGS = ${oe_cflags}
test_oek_msg_LDFLAGS = @OE_TESTER_LDFLAGS@

test_oecommon_SOURCES = test_oecommon.c
test_oecommon_LDADD = ${oe_ldadd}
test_oecommon_CFLAGS = ${oe_cflags}
test_oecommon_LDFLAGS = @OE_TESTER_LDFLAGS@

test_oec_values_SOURCES = test_oec_values.c
test_oec_values_LDADD = ${oe_ldadd}
test_oec_values_CFLAGS = ${oe_cflags}
test_oec_values_LDFLAGS = @OE_TESTER_LDFLAGS@

test_except_SOURCES = test_except.c
test_except_LDADD = ${oe_ldadd}
test_except_CFLAGS = ${oe_cflags}
test_except_LDFLAGS = @OE_TESTER_LDFLAGS@

test_store_holder_SOURCES = test_store_holder.c
test_store_holder_LDADD = ${oe_ldadd}
test_store_holder_CFLAGS = ${oe_cflags}
test_store_holder_LDFLAGS = @OE_TESTER_LDFLAGS@

test_json_serialize_SOURCES = test_json_serialize.c
test_json_serialize_LDADD = ${oe_ldadd}
test_json_serialize_CFLAGS = ${oe_cflags}
test_json_serialize_LDFLAGS = @OE_TESTER_LDFLAGS@

test_json_deserialize_SOURCES = test_json_deserialize.c
test_json_deserialize_LDADD = ${oe_ldadd}
test_json_deserialize_CFLAGS = ${oe_cflags}
test_json_deserialize_LDFLAGS = @OE_TESTER_LDFLAGS@

test_json_kernel_SOURCES = test_json_kernel.c
test_json_kernel_LDADD = ${oe_ldadd}
test_json_kernel_CFLAGS = ${oe_cflags}
test_json_kernel_LDFLAGS = @OE_TESTER_LDFLAGS@

test_sighandler_SOURCES = test_sighandler.c
test_sighandler_LDADD = ${oe_ldadd}
test_sighandler_CFLAGS = ${oe_cflags}
test_sighandler_LDFLAGS = @OE_TESTER_LDFLAGS@

#TESTS = test_libevent test_timer_dispatcher test_thread_dispatcher test_nothreads_dispatcher test_group_dispatcher test_kernel test_oek_msg test_oecommon test_except test_oec_values test_store_holder test_json_serialize test_json_deserialize test_json_kernel test_sighandler test_dataobject
TESTS = test_libevent test_timer_dispatcher test_thread_dispatcher test_nothreads_dispatcher test_group_dispatcher test_kernel test_oek_msg test_oecommon test_except test_oec_values test_store_holder test_json_serialize test_json_deserialize test_json_kernel test_sighandler

all-local:
	mkdir -p data

clean-local:
	rm -rf data
	rm -rf __db.*
	rm -rf log.0*

