/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "OeJclEncoder.h"
#include "table.h"
#include <assert.h>
#include "oec_log.h"

#define T OeJclEncoder
struct T {
    Arena_T arena;
};


T OeJclEncoder_new(Arena_T arena) {
    assert(arena);
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    return _this_;
}

static char *CON_MSG_TEMPLATE = "connect(cid=%lld extentname=\"%s\" username=\"%s\" password=\"%s\");";
char *_con_cmd_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    ConCmd cmd = OekMsg_get_con_cmd(msg);
    char *extentname = ConCmd_get_extentname(cmd);
    if (extentname == NULL) {
        OE_ERR(0, "error: no extentname\n");
        return NULL;
    }
    char *username = ConCmd_get_username(cmd);
    if (username == NULL) {
        OE_ERR(0, "error: no username\n");
        return NULL;
    }
    char *password = ConCmd_get_pwd(cmd);
    if (password == NULL) {
        OE_ERR(0, "error: no password\n");
        return NULL;
    }
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(CON_MSG_TEMPLATE) + 20 + 
                                 strlen(extentname) + 
                                 strlen(username) + 
                                 strlen(password), __FILE__, __LINE__);
    sprintf(msg_text, CON_MSG_TEMPLATE, cid, extentname, username, password);
    return msg_text;
}

static char *DIS_MSG_TEMPLATE = "dis();";
char *_dis_cmd_serialize(T _this_, const OekMsg msg) {
    return DIS_MSG_TEMPLATE;
}

char *_serialize_tuple(Arena_T arena, DataObject data) {
    char *str = NULL;
    for (Iterator iter = DataObject_iterator(data, true); Iterator_hasMore(iter);) {
        if (str == NULL) {
            str = oec_str_concat(arena, 3, "\"", Iterator_next(iter), "\"");
        } else {
            str = oec_str_concat(arena, 2, str, " ");
            str = oec_str_concat(arena, 4, str, "\"", Iterator_next(iter), "\"");
        }
    }
    return str;
}
char *_serialize_map(Arena_T arena, DataObject data) {
    char *str = NULL;
    for (Iterator iter = DataObject_iterator(data, true); Iterator_hasMore(iter);) {
        if (str == NULL) {
            str = oec_str_concat(arena, 3, "\"", Iterator_next(iter), "\"");
        } else {
            str = oec_str_concat(arena, 2, str, " ");
            str = oec_str_concat(arena, 4, str, "\"", Iterator_next(iter), "\"");
        }
        if (!Iterator_hasMore(iter)) {
            OE_DLOG(0, "syntax error. not a map pair\n");
            return NULL;
        }
        str = oec_str_concat(arena, 2, str, ": ");
        str = oec_str_concat(arena, 4, str, "\"", Iterator_next(iter), "\"");
    }
    return str;
}
static char *PUT_MSG_TEMPLATE_PRE = "write(cid=%lld tid=%lld dur=%ld [";
static char *PUT_MSG_TEMPLATE_SUF = "]);";
char *_put_cmd_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    oe_id tid = OekMsg_get_tid(msg);
    PutCmd cmd = OekMsg_get_put_cmd(msg);
    int dur = PutCmd_get_dur(cmd);
    OE_TLOG(NULL, "OeJclEncoder._put_cmd_serialize cid: %lld tid: %lld dur: %ld\n", cid, tid, dur);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(PUT_MSG_TEMPLATE_PRE) + 60, __FILE__, __LINE__);
    sprintf(msg_text, PUT_MSG_TEMPLATE_PRE, cid, tid, dur);
    if (PutCmd_is_tuple(cmd)) {
        msg_text = oec_str_concat(_this_->arena, 2, msg_text,
                                  _serialize_tuple(_this_->arena, PutCmd_get_data(cmd)));
    } else {
        msg_text = oec_str_concat(_this_->arena, 2, msg_text,
                                  _serialize_map(_this_->arena, PutCmd_get_data(cmd)));
    }
    msg_text = oec_str_concat(_this_->arena, 2, msg_text, PUT_MSG_TEMPLATE_SUF);
                              
    return msg_text;
}

static char *GET_CMD_TEMPLATE_PRE = "%s(timeout=%lld cid=%lld tid=%lld [";
static char *GET_CMD_TEMPLATE_SUF = "]);";
char *_get_cmd_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    oe_id tid = OekMsg_get_tid(msg);
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    bool isdestroy = GetCmd_remove(cmd);
    oe_time timeout = GetCmd_get_timeout(cmd);
    char *msg_text = Arena_alloc(_this_->arena, 
                     strlen(GET_CMD_TEMPLATE_PRE) + 45, __FILE__, __LINE__);
    if (isdestroy) {
        sprintf(msg_text, GET_CMD_TEMPLATE_PRE, "take", timeout, cid, tid);
    } else {
        sprintf(msg_text, GET_CMD_TEMPLATE_PRE, "read", timeout, cid, tid);
    }
    DataObjectList dlist = GetCmd_get_data_objects(cmd);
    Iterator iter = DataObjectList_iterator(dlist, true);
    DataObject dobj = Iterator_next(iter);
    if (GetCmd_is_tuple(cmd)) {
        msg_text = oec_str_concat(_this_->arena, 2, msg_text,
                                  _serialize_tuple(_this_->arena, dobj));
    } else {
        msg_text = oec_str_concat(_this_->arena, 2, msg_text,
                                  _serialize_map(_this_->arena, dobj));
    }
    msg_text = oec_str_concat(_this_->arena, 2, msg_text, GET_CMD_TEMPLATE_SUF);
                              
    return msg_text;
}

static char *STA_MSG_TEMPLATE = "txn(cid=%lld timeout=%ld);";
char *_sta_cmd_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    StaCmd cmd = OekMsg_get_sta_cmd(msg);
    int dur = StaCmd_get_dur(cmd);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(STA_MSG_TEMPLATE) + 40, __FILE__, __LINE__);
    sprintf(msg_text, STA_MSG_TEMPLATE, cid, dur);
                              
    return msg_text;
}

static char *COMMIT_MSG_TEMPLATE = "commit(cid=%lld tid=%lld enlisted=%ld);";
char *_commit_cmd_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    oe_id tid = OekMsg_get_tid(msg);
    UptCmd cmd = OekMsg_get_upt_cmd(msg);
    size_t nenlisted = UptCmd_get_enlisted(cmd);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(COMMIT_MSG_TEMPLATE) + 60, __FILE__, __LINE__);
    sprintf(msg_text, COMMIT_MSG_TEMPLATE, cid, tid, nenlisted);
                              
    return msg_text;
}

//bug, see *.y file fixme
static char *UPL_CMD_TEMPLATE = "cancel(cid=%lld lid=%ld);";
//static char *UPL_CMD_TEMPLATE = "cancel(lid=%lld);";
char *_upl_cmd_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    UplCmd cmd = OekMsg_get_upl_cmd(msg);
    int lid = UplCmd_get_lid(cmd);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(UPL_CMD_TEMPLATE) + 40, __FILE__, __LINE__);
    sprintf(msg_text, UPL_CMD_TEMPLATE, cid, lid);
    //sprintf(msg_text, UPL_CMD_TEMPLATE, lid);
                              
    return msg_text;
}

static char *RB_MSG_TEMPLATE = "cancel(cid=%lld tid=%lld);";
char *_rollback_cmd_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    oe_id tid = OekMsg_get_tid(msg);
    UptCmd cmd = OekMsg_get_upt_cmd(msg);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(RB_MSG_TEMPLATE) + 40, __FILE__, __LINE__);
    sprintf(msg_text, RB_MSG_TEMPLATE, cid, tid);
                              
    return msg_text;
}

static char *CON_RES_TEMPLATE = "response(cmd=connect cid=%lld sid=%lld);";
char *_con_res_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    ConRes res = OekMsg_get_con_res(msg);
    oe_id sid = ConRes_get_sid(res);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(CON_RES_TEMPLATE) + 40, __FILE__, __LINE__);
    sprintf(msg_text, CON_RES_TEMPLATE, cid, sid);
                              
    return msg_text;
}

static char *PUT_RES_TEMPLATE = "response(cmd=write cid=%lld lid=%lld);";
char *_put_res_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    PutRes res = OekMsg_get_put_res(msg);
    int lid = PutRes_get_lease_id(res);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(PUT_RES_TEMPLATE) + 20, __FILE__, __LINE__);
    sprintf(msg_text, PUT_RES_TEMPLATE, (oe_id) cid, (oe_id) lid);
                              
    return msg_text;
}

static char *STA_RES_TEMPLATE = "response(cmd=txn cid=%lld tid=%lld);";
char *_sta_res_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    oe_id tid = OekMsg_get_tid(msg);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(STA_RES_TEMPLATE) + 20, __FILE__, __LINE__);
    sprintf(msg_text, STA_RES_TEMPLATE, (oe_id) cid, (oe_id) tid);
                              
    return msg_text;
}

//static char *OEK_ERR_TEMPLATE = "error(cid=%lld tid=%lld msg=\"%s\");";
static char *OEK_ERR_TEMPLATE = "error(cid=%lld msg=\"%s\");";
char *_err_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    //oe_id tid = OekMsg_get_tid(msg);
    OekErr err = OekMsg_get_err(msg);
    char *emsg = OekErr_get_message(err);
    if (emsg == NULL) {
        emsg = "";
    }
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(OEK_ERR_TEMPLATE) + 40 +
                                 strlen(emsg), __FILE__, __LINE__);
    //sprintf(msg_text, OEK_ERR_TEMPLATE, (oe_id) cid, (oe_id) tid, emsg);
    sprintf(msg_text, OEK_ERR_TEMPLATE, (oe_id) cid, emsg);
                              
    return msg_text;
}

static char *GET_RES_TEMPLATE_PRE = "response(cmd=%s cid=%lld nresults=%ld [";
static char *GET_RES_TEMPLATE_SUF = "]);";
char *_get_res_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    GetRes res = OekMsg_get_get_res(msg);
    bool isdestroyed = GetRes_is_destroyed(res);
    size_t nresults = GetRes_get_nresults(res);
    char *msg_text = Arena_alloc(_this_->arena, 
                     strlen(GET_RES_TEMPLATE_PRE) + 70, __FILE__, __LINE__);
    if (isdestroyed) {
        sprintf(msg_text, GET_RES_TEMPLATE_PRE, "take", cid, nresults);
    } else {
        sprintf(msg_text, GET_RES_TEMPLATE_PRE, "read", cid, nresults);
    }
    //rats... need to know if it is a tuple
    if (GetRes_is_tuple(res)) {
        char *tupstr = _serialize_tuple(_this_->arena, GetRes_get_data(res));
        if (tupstr == NULL) tupstr = "";
        msg_text = oec_str_concat(_this_->arena, 2, msg_text, tupstr);
    } else {
        char *mapstr = _serialize_map(_this_->arena, GetRes_get_data(res));
        if (mapstr == NULL) mapstr = "";
        msg_text = oec_str_concat(_this_->arena, 2, msg_text, mapstr);
    }
    msg_text = oec_str_concat(_this_->arena, 2, msg_text, GET_RES_TEMPLATE_SUF);
                              
    return msg_text;
}

static char *COMMIT_RES_TEMPLATE = "response(cmd=commit cid=%lld);";
char *_commit_res_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    UptRes res = OekMsg_get_upt_res(msg);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(COMMIT_RES_TEMPLATE) + 20, __FILE__, __LINE__);
    sprintf(msg_text, COMMIT_RES_TEMPLATE, (oe_id) cid);
                              
    return msg_text;
}

static char *CANCEL_RES_TEMPLATE = "response(cmd=cancel cid=%lld);";
char *_cancel_res_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    UptRes res = OekMsg_get_upt_res(msg);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(CANCEL_RES_TEMPLATE) + 20, __FILE__, __LINE__);
    sprintf(msg_text, CANCEL_RES_TEMPLATE, (oe_id) cid);
                              
    return msg_text;
}

static char *UPL_RES_TEMPLATE = "response(cmd=cancel cid=%lld);";
char *_upl_res_serialize(T _this_, const OekMsg msg) {
    oe_id cid = OekMsg_get_cid(msg);
    UplRes res = OekMsg_get_upl_res(msg);
    char *msg_text = Arena_alloc(_this_->arena, 
                                 strlen(UPL_RES_TEMPLATE) + 20, __FILE__, __LINE__);
    sprintf(msg_text, UPL_RES_TEMPLATE, (oe_id) cid);
                              
    return msg_text;
}

char *OeJclEncoder_msg_serialize(T _this_, const OekMsg msg) {
    assert(_this_);
    assert(msg);
    UptCmd uptcmd;
    UptRes uptres;
    switch (OekMsg_get_type(msg)) {
    case CON_CMD:
        return _con_cmd_serialize(_this_, msg);
    case DIS_CMD:
        return _dis_cmd_serialize(_this_, msg);
    case PUT_CMD:
        return _put_cmd_serialize(_this_, msg);
    case GET_CMD:
        return _get_cmd_serialize(_this_, msg);
    case STA_CMD:
        return _sta_cmd_serialize(_this_, msg);
    case UPT_CMD:
        uptcmd = OekMsg_get_upt_cmd(msg);
        if (UptCmd_get_status(uptcmd) == OETXN_COMMITTED) {
            return _commit_cmd_serialize(_this_,msg);
        } else {
            return _rollback_cmd_serialize(_this_,msg);
        }
    case CON_RES:
        return _con_res_serialize(_this_, msg);
    case PUT_RES:
        return _put_res_serialize(_this_, msg);
    case GET_RES:
        return _get_res_serialize(_this_, msg);
    case UPT_RES:
        uptres = OekMsg_get_upt_res(msg);
        if (UptRes_get_status(uptres) == OETXN_COMMITTED) {
            return _commit_res_serialize(_this_,msg);
        } else {
            return _cancel_res_serialize(_this_,msg);
        }
    case STA_RES:
        return _sta_res_serialize(_this_, msg);
    case OEK_ERR:
        return _err_serialize(_this_, msg);
    case UPL_CMD:
        return _upl_cmd_serialize(_this_, msg);
    case UPL_RES:
        return _upl_res_serialize(_this_, msg);
    default:
        assert(false);
        return NULL;
    }
}

#undef T

