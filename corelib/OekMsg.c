/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "config.h"
#include <assert.h>
#include "OekMsg.h"
#include "mem.h"

#define T OekMsg

Arena_T OekMsg_get_arena(T _this_) {
    return _this_->arena;
}

void OekMsg_settype(T _this_, OEK_MSG_TYPE mtype) {
    assert(_this_);
    assert(mtype);

    _this_->mtype = mtype;
    Arena_T arena = _this_->arena;

    switch (mtype) {
    case OEK_ERR:
        _this_->data.err = OekErr_new(arena);
        break;
    case DIS_CMD:
        _this_->data.dis_cmd = DisCmd_new(arena);
        break;
    case CON_CMD:
        _this_->data.con_cmd = ConCmd_new(arena);
        break;
    case CON_RES:
        _this_->data.con_res = ConRes_new(arena);
        break;
    case STA_CMD:
        _this_->data.sta_cmd = StaCmd_new(arena);
        break;
    case STA_RES:
        _this_->data.sta_res = StaRes_new(arena);
        break;
    case UPT_CMD:
        _this_->data.upt_cmd = UptCmd_new(arena);
        break;
    case UPT_RES:
        _this_->data.upt_res = UptRes_new(arena);
        break;
    case UPL_CMD:
        _this_->data.upl_cmd = UplCmd_new(arena);
        break;
    case UPL_RES:
        _this_->data.upl_res = UplRes_new(arena);
        break;
    case GET_CMD:
        _this_->data.get_cmd = GetCmd_new(arena);
        break;
    case GET_RES:
        _this_->data.get_res = GetRes_new(arena);
        break;
    case PUT_CMD:
        _this_->data.put_cmd = PutCmd_new(arena);
        break;
    case PUT_RES:
        _this_->data.put_res = PutRes_new(arena);
        break;
    case OEK_NON:
    default:
        assert(false);
    }
}

T OekMsg_new(OEK_MSG_TYPE mtype, Arena_T arena) {
    assert(arena);
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    if (mtype) {
        _this_->mtype = mtype;
        OekMsg_settype(_this_, mtype);
    }
    _this_->valid = true;
    return _this_;
}

OEK_MSG_TYPE OekMsg_get_type(T _this_) {
    return _this_->mtype;
}

void OekMsg_set_cid(T _this_, oe_id cid) {
    _this_->cor_id = cid;
}
oe_id OekMsg_get_cid(T _this_) {
    return _this_->cor_id;
}
void OekMsg_set_tid(T _this_, oe_id tid) {
    _this_->txn_id = tid;
}
oe_id OekMsg_get_tid(T _this_) {
    return _this_->txn_id;
}

OekErr OekMsg_get_err(T _this_) {
    assert(_this_->mtype == OEK_ERR);
    return _this_->data.err;
}

ConCmd OekMsg_get_con_cmd(T _this_) {
    assert(_this_->mtype == CON_CMD);
    return _this_->data.con_cmd;
}
DisCmd OekMsg_get_dis_cmd(T _this_) {
    assert(_this_->mtype == DIS_CMD);
    return _this_->data.dis_cmd;
}
ConRes OekMsg_get_con_res(T _this_) {
    assert(_this_->mtype == CON_RES);
    return _this_->data.con_res;
}

StaCmd OekMsg_get_sta_cmd(T _this_) {
    assert(_this_->mtype == STA_CMD);
    return _this_->data.sta_cmd;
}
StaRes OekMsg_get_sta_res(T _this_) {
    assert(_this_->mtype == STA_RES);
    return _this_->data.sta_res;
}

UptCmd OekMsg_get_upt_cmd(T _this_) {
    assert(_this_->mtype == UPT_CMD);
    return _this_->data.upt_cmd;
}
UptRes OekMsg_get_upt_res(T _this_) {
    assert(_this_->mtype == UPT_RES);
    return _this_->data.upt_res;
}

UplCmd OekMsg_get_upl_cmd(T _this_) {
    assert(_this_->mtype == UPL_CMD);
    return _this_->data.upl_cmd;
}
UplRes OekMsg_get_upl_res(T _this_) {
    assert(_this_->mtype == UPL_RES);
    return _this_->data.upl_res;
}

GetCmd OekMsg_get_get_cmd(T _this_) {
    assert(_this_->mtype == GET_CMD);
    return _this_->data.get_cmd;
}
GetRes OekMsg_get_get_res(T _this_) {
    assert(_this_->mtype == GET_RES);
    return _this_->data.get_res;
}

PutCmd OekMsg_get_put_cmd(T _this_) {
    assert(_this_->mtype == PUT_CMD);
    return _this_->data.put_cmd;
}
PutRes OekMsg_get_put_res(T _this_) {
    assert(_this_->mtype == PUT_RES);
    return _this_->data.put_res;
}

#undef T

