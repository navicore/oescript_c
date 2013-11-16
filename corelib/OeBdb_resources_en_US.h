/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#ifdef __cplusplus
extern "C" {
#endif 

#ifndef OE_DB_RES_EN_US_H
#define OE_DB_RES_EN_US_H

#define OEDB_STR_UPDATING_COMMITTED_TXN     "trying to update committed txn\n"
#define OEDB_STR_UPDATING_ROLLEDBACK_TXN     "trying to update rolledback txn\n"
#define OEDB_STR_UPDATING_EXPIRED_TXN     "trying to update expired txn\n"
#define OEDB_STR_NOT_MARSHAL_HDR            "could not marshal holder\n"

#define OEDB_STR_OPEN_CUR_FAILED            "cursor open failed"

#define OEDB_STR_GET_SKIP_EXP               "get is skipping entry due to expired lease\n"

#define OEDB_STR_GET_SKIP_TAKE_LOCK         "get is skipping entry due to take locked lease\n"

#define OEDB_STR_GET_SKIP_WRITE_LOCK        "get is skipping entry due to write locked lease\n"

#define OEDB_STR_ERR_CREATING_ENV           "Error creating env handle: %s\n"

#define OEDB_STR_ERR_CREATING_ENV_CACHE     "Error creating env cache: %s\n"

#define OEDB_STR_ERR_OPENING_ENV            "Error opening env: %s\n"

#define OEDB_STR_NOT_BEGIN_DB_OPEN          "could not begin 'db open' txn"

#define OEDB_STR_SEQ_CREATE_ERR             "db_sequence_create error"

#define OEDB_STR_SEQ_OPEN_ERR               "DB_SEQUENCE->open error"

#define OEDB_STR_TXN_DB_FILENAME            "txn.db"
#define OEDB_STR_HOLDER_DB_FILENAME         "holder.db"
#define OEDB_STR_ID_DB_FILENAME             "id.db"
#define OEDB_STR_OWNER_IDX_FILENAME         "owner.idx"
#define OEDB_STR_PROP_IDX_FILENAME          "attrs.idx"
#define OEDB_STR_EXP_IDX_FILENAME           "exp.idx"

#define OEDB_STR_OPEN_TXN_FAILED            "db open txn failed"
#define OEDB_STR_TXN_IS_NOT_ACTIVE          "txn is not active: %s\n"

#define OEDB_STR_DB_SEQ_CLOSE_ERR           "DB_SEQUENCE close error"

#define OEDB_STR_CREATE_ID_DL_RETRY         "oedb_create_id deadlock retry %d\n"

#define OEDB_STR_CR_ID_DL_RETRY_ERR         "oedb_create_id retry %d. error: %s\n"

#define OEDB_STR_DB_OPEN_FAILED             "Database '%s' open failed."

#define OEDB_STR_PRO_DL_ERR                 "processed db deadlock error rc: %s\n"

#define OEDB_STR_PRO_DL                     "processed %d db deadlock(s) with type %d processor\n"

#define OEDB_STR_LSE_RPR_CUR_ERR            "oedb_lease_reaper cur err: %s\n"

#define OEDB_STR_LSE_RPR_ITEM               "processing lease reaper item %d lid: %lld expires: %s\n"

#define OEDB_TXN_BEGIN_TIME                 "OeBdb begin txn: %lld dur: %lld now: %lld exptime %lld\n"
#define OEDB_TXN_UPDATE_TIME                "OeBdb update txn: %lld dur: %lld now: %lld exptime %lld\n"

#define OEDB_STR_LSE_RPR_DEL_ERR            "oedb_lease_reaper del err: %s\n"

#define OEDB_STR_LSE_RPR_ERR                "oedb_lease_reaper err: %s\n"

#define OEDB_STR_LSE_RPR_COMMIT_ERR         "oedb_lease_reaper commit err: %s\n"

#define OEDB_STR_TXN_BEGIN_ERR              "txn_begin err: %s"

#define OEDB_STR_NOT_GET_H_BY_LID           "could not retrieve holder by lease_id"

#define OEDB_STR_NOT_COMMIT_PUT_HDR         "could not commit 'put' holder"

#define OEDB_STR_NOT_COMMIT_PUT_HDR         "could not commit 'put' holder"

#define OEDB_STR_NOT_COMMIT_UPL             "could not commit 'update lease' txn"

#define OEDB_STR_FOUND_UNLOCKED_COMMIT      "found an unlocked item during commit that still has a tid"

#define OEDB_STR_CONFIG_DEADLOCK_WATCHDOG   "configuring deadlock processing per transaction\n"

#define OEDB_STR_START_DL_WATCHDOG          "starting deadlock watchdog thread\n"

#define OEDB_STR_CAN_NOT_CREATE_DL_WDOG     "can not create deadlock watchdog thread"

#define OEDB_STR_LOG                        "bdb store-> %s\n"

#define OEDB_STR_ERRLOG                     "bdb store error/warn %s-> %s\n"

#define OEDB_STR_DEADLOCK_PLEASE_RETRY      "deadlock, please retry\n"

#define OEDB_STOPPING_STORE                 "stopping store\n"

#define OEDB_TXN_NAME_GET                   "get"
#define OEDB_TXN_NAME_PUT                   "put"
#define OEDB_TXN_NAME_BEGIN                 "begin"
#define OEDB_TXN_NAME_QUERY_TXN             "query"
#define OEDB_TXN_NAME_UPDATE_TXN            "update"
#define OEDB_TXN_NAME_UPDATE_LEASE          "upl"
#define OEDB_TXN_NAME_OPEN_ENV              "open_env"

#define OEDB_WATCHDOG_CLEARED_LOCK          "deadlock watchdog cleared lock\n"
#define OEDB_WATCHDOG_CLEARED_EXPIRED_LOCK  "deadlock watchdog cleared expired lock\n"
#define OEDB_POS_TUPLE_CURSORS              "_position_tuple_cursors for: %s\n"
#define OEDB_PUT_HOLDER                     "OeBeb _put_holder\n"
#define OEDB_SET_MAP_PROPS                  "_set_map_props for: %s\n"
#define OEDB_SET_TUPLE_PROPS                "_set_tuple_props for: %s\n"
#define OEDB_SET_PROPS                      "OeBeb _set_props\n"

#define OEDB_LOG_STATS_FOR_ENV          "----- db stats for environment -----\n"
#define OEDB_LOG_STATS_FOR_ID_DB        "----- db stats for ID DB -----\n"
#define OEDB_LOG_STATS_FOR_TXN_DB       "----- db stats for TXN DB -----\n"
#define OEDB_LOG_STATS_FOR_HOLDER_DB    "----- db stats for HOLDER DB -----\n"
#define OEDB_LOG_STATS_FOR_OBJ_DB       "----- db stats for OBJECT DB -----\n"
#define OEDB_LOG_STATS_FOR_PROP_DB      "----- db stats for PROPERTY DB -----n"
#define OEDB_LOG_STATS_FOR_OWNER_DB     "----- db stats for OWNER DB -----\n"
#define OEDB_LOG_STATS_FOR_EXPTIME_DB   "----- db stats for EXPIRE TIME DB -----\n"
#define OEDB_WRONG_ENLISTED_COUNT       "incorrect txn enlisted count. expected %i but found %i\n"

#endif

#ifdef __cplusplus
}
#endif 

