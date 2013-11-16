/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "OeJclDecoder.h"
#include "SKeyTable.h"
#include "DataObject.h"
#include "DataObjectList.h"
#include "oejcl_parser.h"
#include "oejcl_parser.tab.h"
#include "table.h"
#include "oec_log.h"
#include <assert.h>

#define T OeJclDecoder
struct T {
    Arena_T arena;
    bool resolve_from_results;
    oejcl_ast *next_node;
    oejcl_ast *prev_node; //use to resolve symboles set by result of prev cmd
    Table_T symbols;
    OekMsg result;
};

OekMsg _msg_create(T, oejcl_ast*);
void _set_variables_from_result(T, oejcl_ast*);
void _set_variables_from_assign(T, oejcl_ast*);
void _set_get_data(T, GetCmd, oejcl_ast*);
void _set_get_res_data(T, GetRes, oejcl_ast*);
void _set_put_data(T, PutCmd, oejcl_ast*);
DataObject _get_data_object(T, oejcl_ast*);
DataObjectList _get_data_object_list(T, oejcl_ast*);

T OeJclDecoder_new(Arena_T arena, char *jcl) {
    assert(arena);
    T _this_;
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->arena = arena;
    _this_->resolve_from_results = false;

    assert(jcl);
    int rc = oejcl_parse(arena, jcl, &_this_->next_node);
    if (rc) {
        return NULL;
    }
    _this_->symbols = SKeyTable_new(100);

    return _this_;
}

/**
 * call me until i return NULL. 
 * if i am called more than once, pass me the 
 *   result of the prev cmd i gave you to run. 
 */
OekMsg OeJclDecoder_next_cmd(T _this_, OekMsg result_of_prev_cmd) {
    assert(_this_);
    _this_->result = result_of_prev_cmd;

    oejcl_ast *current = _this_->next_node;
    if (current == NULL) return NULL;
    assert(current->l);

    if (result_of_prev_cmd && _this_->resolve_from_results) {
        _set_variables_from_result(_this_, _this_->prev_node->l);
    }

    _this_->prev_node = current;
    _this_->next_node = current->r;

    if (current->l->nodetype == ASSIGN) {
        _set_variables_from_assign(_this_, current->l);
        return OeJclDecoder_next_cmd(_this_, NULL);
    }

    return _msg_create(_this_, current->l);
}

/*
OekMsg OeJclDecoder_get_result(T _this_) {
    assert(_this_);
    assert(false);
    return NULL;
}
*/

////////////////// ast walker ///////////////
OekMsg _gen_msg_from_dis_stmt(T _this_, oejcl_ast *ast) {
    OekMsg msg = OekMsg_new(DIS_CMD, _this_->arena);
    DisCmd cmd = OekMsg_get_dis_cmd(msg);
    return msg;
}

OekMsg _gen_msg_from_connect_stmt(T _this_, oejcl_ast *ast) {
    OekMsg msg = OekMsg_new(CON_CMD, _this_->arena);
    ConCmd cmd = OekMsg_get_con_cmd(msg);
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        strval* str;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting con cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case EXTENTNAME:
            str = (strval*) ast->l;
            assert(str->nodetype == STRING);
            OE_DLOG(0, "setting con extentname: %s\n", str->str);
            ConCmd_set_extentname(cmd, str->str);
            break;
        case USERNAME:
            str = (strval*) ast->l;
            assert(str->nodetype == STRING);
            OE_DLOG(0, "setting con username: %s\n", str->str);
            ConCmd_set_username(cmd, str->str);
            break;
        case PASSWORD:
            str = (strval*) ast->l;
            assert(str->nodetype == STRING);
            OE_DLOG(0, "setting con pwd: %s\n", str->str);
            ConCmd_set_pwd(cmd, str->str);
            break;
        default:
            OE_DLOG(0, "unknown con msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_upl_stmt(T _this_, oejcl_ast *ast) {
    OekMsg msg = OekMsg_new(UPL_CMD, _this_->arena);
    UplCmd cmd = OekMsg_get_upl_cmd(msg);
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting upl cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case LEASE_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting upl lid: %ld\n", (oe_id) nval->number);
            UplCmd_set_lid(cmd, (oe_id) nval->number);
            break;
        default:
            OE_DLOG(0, "unknown upl msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_read_stmt(T _this_, oejcl_ast *ast) {
    //handle both read and take
    OekMsg msg = OekMsg_new(GET_CMD, _this_->arena);
    GetCmd cmd = OekMsg_get_get_cmd(msg);
    if (ast->nodetype == TAKE) {
        GetCmd_set_remove_option(cmd);
    }
    GetCmd_set_max_responses(cmd, 1);
    GetCmd_set_return_attrs(cmd, true);
    GetCmd_set_return_bytes(cmd, true);
    _set_get_data(_this_, cmd, ast->r);
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case TXN_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting get tid: %ld\n", (oe_id) nval->number);
            OekMsg_set_tid(msg, (oe_id) nval->number);
            break;
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting get cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case TIMEOUT:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting get timeout: %ld\n", (oe_dur) nval->number);
            GetCmd_set_timeout(cmd, nval->number);
            break;
        default:
            OE_DLOG(0, "unknown read msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_write_stmt(T _this_, oejcl_ast *ast) {

    OekMsg msg = OekMsg_new(PUT_CMD, _this_->arena);
    PutCmd cmd = OekMsg_get_put_cmd(msg);
    if (ast->r) {
        _set_put_data(_this_, cmd, ast->r);
    }
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case TXN_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting write tid: %ld\n", (oe_id) nval->number);
            OekMsg_set_tid(msg, (oe_id) nval->number);
            break;
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting write cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case DURATION:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting write duration: %ld\n", (oe_dur) nval->number);
            PutCmd_set_dur(cmd, nval->number);
            break;
        default:
            OE_DLOG(0, "unknown read msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_txn_stmt(T _this_, oejcl_ast *ast) {

    OekMsg msg = OekMsg_new(STA_CMD, _this_->arena);
    StaCmd cmd = OekMsg_get_sta_cmd(msg);
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting txn cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case TIMEOUT:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting txn timeout: %ld\n", (oe_dur) nval->number);
            StaCmd_set_dur(cmd, nval->number);
            break;
        default:
            OE_DLOG(0, "unknown txn msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_upt_stmt(T _this_, oejcl_ast *ast, char status) {

    OekMsg msg = OekMsg_new(UPT_CMD, _this_->arena);
    UptCmd cmd = OekMsg_get_upt_cmd(msg);
    UptCmd_set_status(cmd, status);
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting upt cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case NENLISTED:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting upt nenlisted: %ld\n", (size_t) nval->number);
            UptCmd_set_enlisted(cmd, nval->number);
            break;
        case TXN_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting upt tid: %ld\n", (oe_id) nval->number);
            OekMsg_set_tid(msg, (oe_id) nval->number);
            break;
        default:
            OE_DLOG(0, "unknown upt msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_write_response_stmt(T _this_, oejcl_ast *ast) {
    OekMsg msg = OekMsg_new(PUT_RES, _this_->arena);
    PutRes res = OekMsg_get_put_res(msg);
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting write res cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case LEASE_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting write res lid: %ld\n", (oe_id) nval->number);
            PutRes_set_lease_id(res, (oe_id) nval->number);
            break;
        case CMD:
            //noop
            break;
        default:
            OE_DLOG(0, "unknown write res msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_read_response_stmt(T _this_, oejcl_ast *ast) {
    OekMsg msg = OekMsg_new(GET_RES, _this_->arena);
    GetRes res = OekMsg_get_get_res(msg);
    _set_get_res_data(_this_, res, ast->r);

    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting read res cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case NRESULTS:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting read res nresults: %ld\n", (size_t) nval->number);
            GetRes_set_nresults(res, (size_t) nval->number);
            break;
        case CMD:
            if (ast->l->nodetype == TAKE) {
                GetRes_set_destroyed(res);
            }
            break;
        default:
            OE_DLOG(0, "unknown read res msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_txn_response_stmt(T _this_, oejcl_ast *ast) {
    OekMsg msg = OekMsg_new(STA_RES, _this_->arena);
    StaRes res = OekMsg_get_sta_res(msg);
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting sta res cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case TXN_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting sta res tid: %ld\n", (oe_id) nval->number);
            OekMsg_set_tid(msg, (oe_id) nval->number);
            break;
        case CMD:
            //noop
            break;
        default:
            OE_DLOG(0, "unknown sta res msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_commit_response_stmt(T _this_, oejcl_ast *ast) {
    OekMsg msg = OekMsg_new(UPT_RES, _this_->arena);
    UptRes res = OekMsg_get_upt_res(msg);
    UptRes_set_status(res, OETXN_COMMITTED);
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting commit res cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case CMD:
            //noop
            break;
        default:
            OE_DLOG(0, "unknown commit res msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_cancel_response_stmt(T _this_, oejcl_ast *ast) {
    OekMsg msg = OekMsg_new(UPT_RES, _this_->arena);
    UptRes res = OekMsg_get_upt_res(msg);
    UptRes_set_status(res, OETXN_ROLLEDBACK);
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting cancel res cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case CMD:
            //noop
            break;
        default:
            OE_DLOG(0, "unknown cancel res msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_connect_response_stmt(T _this_, oejcl_ast *ast) {
    OekMsg msg = OekMsg_new(CON_RES, _this_->arena);
    ConRes res = OekMsg_get_con_res(msg);
    ast = ast->l;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting con res cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case SESSION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting con res sid: %ld\n", (oe_id) nval->number);
            ConRes_set_sid(res, (oe_id) nval->number);
            break;
        case CMD:
            //noop
            break;
        default:
            OE_DLOG(0, "unknown con res msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

OekMsg _gen_msg_from_error_stmt(T _this_, oejcl_ast *ast) {
    OekMsg msg = OekMsg_new(OEK_ERR, _this_->arena);
    OekErr err = OekMsg_get_err(msg);
    ast = ast->l;
    strval* str;
    for (;;) {
        if (ast == NULL) break;
        numval *nval;
        switch (ast->nodetype) {
        case CORRELATION_ID:
            nval = (numval*) ast->l;
            assert(nval->nodetype == NUMBER);
            OE_DLOG(0, "setting err cid: %ld\n", (oe_id) nval->number);
            OekMsg_set_cid(msg, (oe_id) nval->number);
            break;
        case MSG:
            str = (strval*) ast->l;
            assert(str->nodetype == STRING);
            OE_DLOG(0, "setting err msg: %s\n", str->str);
            OekErr_set_message(err, str->str);
            break;
        case CMD:
            //noop
            break;
        default:
            OE_DLOG(0, "unknown err msg attr: %i\n", ast->nodetype);
            return NULL;
        }
        ast = ast->r;
    }
    return msg;
}

int _get_res_type(oejcl_ast *ast) {
    if (ast->nodetype == CMD) {
        return (int) ast->l->nodetype;
    }
    if (ast->r) {
        return _get_res_type(ast->r);
    }
    assert(false);
    return NULL;
}

OekMsg _gen_msg_from_response_stmt(T _this_, oejcl_ast *ast) {
    int type = _get_res_type(ast->l);
    switch (type) {
    case CONNECT:
        return _gen_msg_from_connect_response_stmt(_this_,ast);
    case WRITE:
        return _gen_msg_from_write_response_stmt(_this_,ast);
    case READ:
        return _gen_msg_from_read_response_stmt(_this_,ast);
    case TAKE:
        return _gen_msg_from_read_response_stmt(_this_,ast);
    case TXN:
        return _gen_msg_from_txn_response_stmt(_this_,ast);
    case COMMIT:
        return _gen_msg_from_commit_response_stmt(_this_,ast);
    case CANCEL:
        return _gen_msg_from_cancel_response_stmt(_this_,ast);
    default:
        assert(false);
        return NULL;
    }
}

void _set_variables_from_get_list_result(T _this_, oejcl_ast* ast) {
    //    walk prev_node 
    //    for each name in prev node, look up value in result
    //    and update symbol table
    assert(ast);
    ast = ast->l;
    Arena_T arena = _this_->arena;
    if (ast == NULL) return;
    GetRes res = OekMsg_get_get_res(_this_->result);
    DataObject data = GetRes_get_data(res);
    if (data == NULL) return;
    Iterator iter = DataObject_iterator(data, true);

    for (;;) {
        oe_scalar result_str = Iterator_next(iter);
        if (result_str == NULL) {
            break;
        }
        strval* str;
        oejcl_ast* vast;
        switch (ast->nodetype) {
        case STRING:
            //noop
            break;
        case NAME:
            str = (strval*) ast->l;
            vast = Table_get(_this_->symbols, str->str);
            if (vast == NULL) {
                OE_ERR(0, "symbol resolved but never declared: %s\n", str->str);
                assert(false);
            } else {
                OE_TLOG(0, "symboler %s resolved as: %s\n", str->str, result_str);
                //create a str ast of the result_str val
                //replace vast with it
                oejcl_ast* newstr = new_oejcl_str_ast(arena, STRING, result_str);
                Table_put(_this_->symbols, str->str, newstr);
            }
            break;
        default:
            OE_DLOG(0, "unknown fld type: %i\n", ast->nodetype);
            assert(false);
        }

        ast = ast->r;
        if (ast == NULL)  break;
    }
    return data;
}
oe_scalar _get_value_from_map_dataobject(DataObject data, oe_scalar key) {
    for (Iterator iter = DataObject_iterator(data, true); Iterator_hasMore(iter); ) {
        oe_scalar dkey = Iterator_next(iter);
        if (dkey == NULL)  return NULL;
        oe_scalar dvalue = Iterator_next(iter);
        if (dvalue == NULL)  return NULL;
        if (strncmp(key, dkey, strlen(key)) == 0) {
            return dvalue;
        }
    }
    return NULL;
}
void _set_variables_from_get_map_result(T _this_, oejcl_ast* ast) {
    //    walk prev_node 
    //    for each name in prev node, look up value in result
    //    and update symbol table
    //          resolve key
    //          resolve val
    Arena_T arena = _this_->arena;
    assert(ast);
    ast = ast->l;
    GetRes res = OekMsg_get_get_res(_this_->result);
    DataObject data = GetRes_get_data(res);
    if (data == NULL) return;

    for (;;) {

        oe_scalar result_value_str;

        oejcl_ast *key = ast;
        if (key == NULL) return;
        strval* keystr = (strval*) key->l;
        
        oejcl_ast *value = ast->r;
        if (value == NULL) return;
        strval* valstr = (strval*) value->l;

        oejcl_ast* vast;
        //
        // resolve value
        //
        switch (value->nodetype) {
        case STRING:
            //noop
            break;
        case NAME:
            vast = Table_get(_this_->symbols, valstr->str);
            if (vast == NULL) { //this shouldn't be possible
                OE_ERR(0, "map value symbol resolved but never declared: %s\n", 
                       valstr->str);
                assert(false);
            } else {
                //get the value from the dataobject who's key matches the ast key
                result_value_str = _get_value_from_map_dataobject(data, keystr->str);
                if (result_value_str == NULL) {
                    break;
                }
                OE_TLOG(0, "map value symboler %s resolved as: %s\n", 
                        valstr->str, result_value_str);
                //create a str ast of the result_value_str val
                //replace vast with it
                oejcl_ast* newstr = new_oejcl_str_ast(arena, STRING, result_value_str);
                Table_put(_this_->symbols, valstr->str, newstr);
            }
            break;
        default:
            OE_DLOG(0, "unknown fld type: %i\n", ast->nodetype);
            assert(false);
        }

        ast = ast->r; //shift to value
        if (ast == NULL)  break;
        ast = ast->r; //shift to next key
        if (ast == NULL)  break;
    }
    //return data;
    return;
}

void _set_variables_from_get_result(T _this_, oejcl_ast* ast) {
    if (OekMsg_get_type(_this_->result) != GET_RES) {
        assert(false);
    }
    switch (ast->nodetype) {
    case LIST:
        _set_variables_from_get_list_result(_this_, ast);
        break;
    case MAP:
        _set_variables_from_get_map_result(_this_, ast);
        break;
    default:
        OE_DLOG(0, "unknown data type: %i\n", ast->nodetype);
        assert(false);
    }
}
void _set_variables_from_result(T _this_, oejcl_ast* ast) {

    //  create a func for:
    //    read / take
    switch (ast->nodetype) {
    case READ:
    case TAKE:
        OE_TLOG(0, "msg type a read/take\n");
        _set_variables_from_get_result(_this_, ast->r);
        break;
    case WRITE:
    case TXN:
    case DIS:
    case CONNECT:
    case COMMIT:
    case CANCEL:
    case CANCELLEASE:
    case RESPONSE:
    case ERROR:
    default:
        OE_DLOG(0, "msg type not a get\n");
        //noop
    }
}

void _set_variables_from_assign(T _this_, oejcl_ast* current) {
    assert(current->nodetype == ASSIGN);
    assert(current->l->nodetype == NAME);
    strval *nstr = (strval*) current->l->l;
    // update symbol table
    Table_put( _this_->symbols, nstr->str, current->r );
}

OekMsg _msg_create(T _this_, oejcl_ast *ast) {
    switch (ast->nodetype) {
    case READ:
        return _gen_msg_from_read_stmt(_this_, ast);
    case TAKE:
        return _gen_msg_from_read_stmt(_this_, ast);
    case WRITE:
        return _gen_msg_from_write_stmt(_this_, ast);
    case TXN:
        return _gen_msg_from_txn_stmt(_this_, ast);
    case DIS:
        return _gen_msg_from_dis_stmt(_this_, ast);
    case CONNECT:
        return _gen_msg_from_connect_stmt(_this_, ast);
    case COMMIT:
        return _gen_msg_from_upt_stmt(_this_, ast, OETXN_COMMITTED);
    case CANCEL:
        return _gen_msg_from_upt_stmt(_this_, ast, OETXN_ROLLEDBACK);
    case CANCELLEASE:
        return _gen_msg_from_upl_stmt(_this_, ast);
    case RESPONSE:
        return _gen_msg_from_response_stmt(_this_, ast);
    case ERROR:
        return _gen_msg_from_error_stmt(_this_, ast);
    default:
        OE_DLOG(0, "unknown msg type\n");
        assert(false);
    }
}

DataObjectList _get_data_object_list(T _this_, oejcl_ast *ast) {
    assert(ast);
    Arena_T arena = _this_->arena;
    DataObjectList datalist = DataObjectList_new(arena);
    DataObject data = _get_data_object(_this_, ast);
    DataObjectList_add(datalist, data);
    return datalist;
}

DataObject _get_data_object(T _this_, oejcl_ast *ast) {
    assert(ast);
    ast = ast->l;
    Arena_T arena = _this_->arena;
    if (ast == NULL) return NULL;
    DataObject data = DataObject_new(arena);
    for (;;) {
        strval* str;
        strval* rstr;
        oejcl_ast* vast;
        switch (ast->nodetype) {
        case STRING:
            str = (strval*) ast->l;
            DataObject_add_attr(data, str->str);
            break;
        case NAME:
            str = (strval*) ast->l;
            vast = Table_get(_this_->symbols, str->str);
            if (vast == NULL) {
                _this_->resolve_from_results = true; //stay set for life of decoder
                OE_DLOG(0, "unknown symbol: %s, will be resolved from results\n", str->str);
                //set value to UNSET so when it is referenced later with no value
                // you can fail
                Table_put(_this_->symbols, str->str, 
                          new_oejcl_ast(arena, UNSET, NULL, NULL));
                DataObject_add_attr(data, "_");
            } else {
                if (vast->nodetype == UNSET) {
                    //if rstr is UNSET, fail with err??? or null???
                    OE_DLOG(0, "symbol %s can not be resolved.\n", str->str);
                    return NULL;
                } else {
                    rstr = (strval*) vast->l;
                    OE_TLOG(0, "symbol %s replaced with: %s\n", str->str, rstr->str);
                    DataObject_add_attr(data, rstr->str);
                }
            }
            break;
        default:
            OE_DLOG(0, "unknown fld type: %i\n", ast->nodetype);
            assert(false);
        }

        ast = ast->r;
        if (ast == NULL)  break;
    }
    return data;
}

void _set_put_data(T _this_, PutCmd cmd, oejcl_ast *ast) {
    assert(ast);
    switch (ast->nodetype) {
    DataObject data;
    case LIST:
        PutCmd_set_data(cmd, _get_data_object(_this_, ast));
        PutCmd_set_tuple(cmd);
        break;
    case MAP:
        PutCmd_set_data(cmd, _get_data_object(_this_, ast));
        break;
    default:
        OE_DLOG(0, "unknown data type: %i\n", ast->nodetype);
        assert(false);
    }
}
void _set_get_res_data(T _this_, GetRes res, oejcl_ast *ast) {
    //assert(ast);
    if (ast == NULL)  return;
    if (ast->l == NULL)  return;
    DataObject data = _get_data_object(_this_, ast);
    GetRes_set_data(res, data);
    switch (ast->nodetype) {
    case LIST:
        GetRes_set_tuple(res);
        break;
    case MAP:
        break;
    default:
        OE_DLOG(0, "unknown data type: %i\n", ast->nodetype);
        assert(false);
    }
}

void _set_get_data(T _this_, GetCmd cmd, oejcl_ast *ast) {
    assert(ast);
    switch (ast->nodetype) {
    case LIST:
        GetCmd_set_data_objects(cmd, _get_data_object_list(_this_, ast));
        GetCmd_set_tuple(cmd);
        break;
    case MAP:
        GetCmd_set_data_objects(cmd, _get_data_object_list(_this_, ast));
        break;
    default:
        OE_DLOG(0, "unknown data type: %i\n", ast->nodetype);
        assert(false);
    }
}

#undef T

