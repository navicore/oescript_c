SUBDIRS = . test

docdir = $(datadir)/doc/
doc_DATA = cii_FILES.txt cjson_LICENSE.txt modp_LICENSE.txt cii_LICENSE.txt  modp_FILES.txt

if INSTALL_SHARED_CORELIB
lib_LTLIBRARIES = liboecore.la
else
noinst_LTLIBRARIES = liboecore.la
endif

MYHEADERS = OeNet.h OeSession.h oescript_core_resources.h oescript_core_resources_en_US.h OeSockClient.h OeNetConn.h Oe_Thread_Dispatcher.h Oed_Dispatcher.h oed_dispatcher_common.h OeStore.h OeStoreQuery.h OeStoreHolder.h OEK_impl.h OEKI.h OEK.h OekMsg.h ConCmd.h DisCmd.h ConRes.h StaCmd.h StaRes.h UplCmd.h UplRes.h UptCmd.h UptRes.h GetCmd.h GetRes.h PutCmd.h PutRes.h OekErr.h oe_common.h arena.h Oec_IdSet.h arenatable.h table.h SKeyTable.h except.h oec_values.h Oec_Future.h Oec_CountdownLatch.h Oec_AList.h list.h set.h arith.h mem.h oec_threads.h oec_log.h DataObject.h DataObjectList.h Iterator.h ArrayIterator.h oec_assert.h modp_b64.h modp_b64_data.h modp_stdint.h arraytoc.h SignalHandler.h oec_file.h json_msg_factory.h cJSON.h OeNetDefaultFactory.h OeLibevent2Factory.h OeLibeventConnFactory.h OepClient.h OepFactory.h OepServer.h oep_resources.h oep_resources_en_US.h oep_utils.h JsonNet.h JsonNet_resources.h JsonNet_resources_en_US.h BuiltInLoginModule.h LoginModule.h OekMsgTemplateMatcher.h Privilege.h SecurityContext.h Subject.h OeStoreDefaultFactory.h OeBdb.h OeBdb_resources.h OeBdb_resources_en_US.h OeScript.h Oes_resources.h Oes_resources_en_US.h OeJclNet.h OeJclDecoder.h OeJclEncoder.h oejcl_parser.h BaseLoginModule.h JsonKernel.h

MYSOURCES = OeNet.c OeSession.c OeSockClient.c OeNetConn.c Oed_Dispatcher.c Oe_Thread_Dispatcher.c oed_dispatcher_common.c OeStoreQuery.c OeStoreHolder.c OeStore.c OEK_impl.c OEK_get.c OEK_put.c OEK_sta.c OEK_upl.c OEK_upt.c OEK.c OekMsg.c ConCmd.c DisCmd.c ConRes.c StaCmd.c StaRes.c UplCmd.c UplRes.c UptCmd.c UptRes.c GetCmd.c GetRes.c PutCmd.c PutRes.c OekErr.c mem.c except.c arena.c Oec_AList.c list.c set.c arith.c oe_prettyprint.c table.c arenatable.c SKeyTable.c Oec_Future.c Oec_CountdownLatch.c oec_values.c Oec_IdSet.c DataObject.c DataObjectList.c Iterator.c ArrayIterator.c arraytoc.c modp_b64.c modp_b64_gen.c SignalHandler.c oec_file.c cJSON.c json_msg_factory.c OeNetDefaultFactory.c OeLibeventConnFactory.c OeLibevent2Factory.c OepClient.c OepFactory.c OepServer.c oep_utils.c JsonNet.c LoginModule.c OekMsgTemplateMatcher.c SecurityContext.c Privilege.c Subject.c OeStoreDefaultFactory.c OeBdb.c OesKernel_impl.c OeJclNet.c OeJclEncoder.c OeJclDecoder.c lex.yy.c oejcl_parser.tab.c oejcl_parser.c BaseLoginModule.c BuiltInLoginModule.c JsonKernel.c

if OE_USE_PAM
PAMHEADERS = PamLoginModule.h
PAMSOURCES = PamLoginModule.c
else
PAMHEADERS =
PAMSOURCES =
endif

include_HEADERS = ${MYHEADERS} ${PAMHEADERS}

liboecore_la_SOURCES = ${MYSOURCES} ${PAMSOURCES}

BUILT_SOURCES = lex.yy.c oejcl_parser.tab.c oejcl_parser.tab.h
CLEANFILES = lex.yy.c oejcl_parser.tab.c oejcl_parser.tab.h

liboecore_la_LIBADD = @OPENSSL_LIBS@ @EVENT_LIBS@ @DB_LIBS@
liboecore_la_CFLAGS = @OE_LIB_CFLAGS@ @OPENSSL_CFLAGS@ @EVENT_CFLAGS@
#math lib for json pow
liboecore_la_LDFLAGS = @OE_LIB_LDFLAGS@ -lpthread -lm

lex.yy.c:	Makefile prepare
oejcl.tab.c:	Makefile prepare
prepare:	oejcl_parser.l oejcl_parser.y
	bison -v -d oejcl_parser.y
	flex oejcl_parser.l

