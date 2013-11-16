/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OESCRIPT_CORE_RES_EN_US_H
#define OESCRIPT_CORE_RES_EN_US_H

#include <config.h>

///////////
/// OEK ///
///////////

#define OEK_STR_GET_NOT_FOUND               "OEK_get not found for txnid: %lld %s\n"

#define OEK_STR_GET_FOUND                   "OEK_get found %d items for txnid: %lld %s\n"

#define OEK_STR_PUT_FAILED                  "put failed"

#define OEK_STR_NO_CALLBACK                 "no oek write cb for txnid: %lld corid: %lld\n"

#define OEK_STR_GET_FAILED                  "get failed"

#define OEK_STR_STA_FAILED                  "sta failed"

#define OEK_STR_UPL_FAILED                  "upl failed"

#define OEK_STR_UPT_FAILED                  "upt failed"

#define OEK_STR_CAN_NOT_STORE               "can not store data"

#define OEK_STR_CAN_NOT_STA                 "can not start transaction"

#define OEK_STR_LEASE_NOT_FOUND             "lease not found"

#define OEK_STR_LEASE_UPDATE_FAILED         "lease update failed"

#define OEK_STR_NULL_TXNID                  "trying to update a null txn (txnid is 0)"

#define OEK_STR_TXN_NOT_FOUND               "transaction not found"

//probably bad networking. the client and server don't agree on how many opperations
// are enlisted in a txn
#define OEK_STR_WRONG_ENLISTED_COUNT_MSG    "incorrect txn enlisted count"
#define OEK_STR_BOGUS_TXN                   "txn is inactive"

#define OEK_STR_TXN_EXPIRED                 "trying to update txn that has an expired txn lease"
#define OEK_STR_TXN_ALREADY_COMMITTED       "trying to update txn that was already committed"
#define OEK_STR_TXN_ALREADY_ROLLEDBACK      "trying to update txn that was already rolledback"

#define OEK_STR_TXN_UPDATE_FAILED           "transaction update failed"

#define OEK_STR_TXN_UPDATE_ERR              "%s. tid: %lld reason: %s\n"

#define OEK_STR_GET_DL_RETRY                "OEK_get deadlock retry %d for txnid: %lld %s %s\n"

#define OEK_STR_PUT_DL_RETRY                "oek_put deadlock retry %d\n"

#define OEK_STR_STA_DL_RETRY                "oek_sta deadlock retry %d\n"

#define OEK_STR_UPL_DL_RETRY                "oek_upl deadlock retry %d\n"

#define OEK_STR_UPT_DL_RETRY                "oek_upt deadlock retry %d for tid: %lld\n"

#define OEK_STR_SETUP                       "setting up " PACKAGE_NAME " kernel version " PACKAGE_VERSION "\n"

/////////////
/// OeNet ///
/////////////

#define OENET_STR_BUFF_ERR_CLOSE            "buffered_on_error closing socket client\n"

#define OENET_STR_BUFF_WRITE_CLOSE          "buffered_on_write orderly closing socket client\n"

#define OENET_STR_CLOSING_SOCK_CLIENT       "closing socket client\n"

#define OENET_STR_NOT_CLOSING_SOCK_CLIENT   "postponing closing socket client\n"

#define OENET_STR_CLOSED_SOCK_CLIENT        "closed socket client\n"

#define OENET_STR_ACCEPT_FAILED             "oenet_on_accept accept failed"

#define OENET_STR_FAILED_SET_NB             "failed to set client socket non-blocking"

#define OENET_STR_LISTEN_NEW_CLIENT         "listening on new socket\n"

#define OENET_STR_SETUP                     PACKAGE_NAME " oenet " PACKAGE_VERSION " compiled " __DATE__ " " __TIME__ " listening on port %d\n"

#define OENET_STR_CAN_NOT_BIND              "can not start/bind server listener socket\n"

#define OENET_CLEANUP_PENDING_TIMOUT        "cleanup_conn call but event still has pending timeout check.\n"

#define OENET_CLEANUP_NO_READ_CB            "buffered_on_error called but no read callback to trigger gc.\n"

//err msgs
//#define OEJSON_ERR_CAN_NOT_PARSE      "can not parse oejson msg"

#define OEJSON_PARTIAL_READ           "JsonNet _buffered_on_read !endbm. partial read?\n"

#define OEJSON_BAD_PARSE              "can not parse oejson msg, rc: %i, json: %s\n"

#define OEP_JSON_READMSG    "json read msg:\n%s\n"
#define OEP_JSON_PEEK_CID   "json peek cid:\n%i\n"
#define OEP_JSON_WRITEMSG   "json write msg:\n%s\n"

//OEJCL
#define OEJCL_PARTIAL_READ   "OeJclNet _buffered_on_read !endbm. partial read?\n"
#define OEJCL_BAD_PARSE      "can not parse OeJcl msg: %s\n"
#define OEP_OEJCL_READMSG    "OeJclNet read msg:\n%s\n"
#define OEP_OEJCL_PEEK_CID   "OeJclNet peek cid:\n%i\n"
#define OEP_OEJCL_WRITEMSG   "OeJclNet write msg:\n%s\n"

///////////
/// OEC ///
///////////

#define OEC_STR_MEM_ALLOC_ERROR             "memory allocation error\n"

#define OEC_DISPATCHER_STATS_1              "Oed_Dispatcher '%s' queue sz: %ld. total items processed: %ld\n"
#define OEC_TDISPATCHER_STATS_1             "Oed_Thread_Dispatcher queue sz: %ld. total items processed: %ld\n"

#define OEC_SIGNALHANDLER_DIAG              "calling diag functions\n"
#define OEC_SIGNALHANDLER_DESTROY           "calling destroy functions\n"
#define OEC_SIGNALHANDLER_SHUTDOWN          "calling shutdown functions\n"
#define OEC_SIGNALHANDLER_SIGINT            "got SIGINT.  Terminating...\n"
#define OEC_SIGNALHANDLER_SIGQUIT           "got SIGQUIT.  printing diag info...\n"

#define OEC_FUTURE_FREE_NOT_DONE            "Oec_Future_free but not done.  free will be done at 'set'\n"
#define OEC_FUTURE_SET_BUT_PENDING_FREE     "Oec_Future_set but there is a pending 'free'.  freeing now.\n"

///////////
/// OED ///
///////////

#define OED_STR_MEM_ALLOC_ERROR "dispatcher exec memory allocation error\n"

#endif

#ifdef __cplusplus
}
#endif 

