/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "OekMsgTemplateMatcher.h"
#include "OekMsg.h"
#include "stdbool.h"

bool OekMsgTemplateMatcher_match(OekMsg msg, OekMsg mtemplate) {
    OEK_MSG_TYPE msg_mtype = OekMsg_get_type(msg);
    OEK_MSG_TYPE tmp_mtype = OekMsg_get_type(mtemplate);
    if (msg_mtype != tmp_mtype) {
        return false;
    }
    switch (msg_mtype) {
    case CON_CMD:
        //todo: inspect extentname
        return true;
    case GET_CMD:
        //todo: inspect template
        return true;
    case PUT_CMD:
        //todo: inspect template
        return true;
    case STA_CMD:
        return true;
    case UPT_CMD:
        return true;
    case UPL_CMD:
        return true;
    case DIS_CMD:
        return true;

    case CON_RES:
    case GET_RES:
    case PUT_RES:
    case STA_RES:
    case UPT_RES:
    case UPL_RES:
    case OEK_ERR:
    default:
        return false;
    }
}

