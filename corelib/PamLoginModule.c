/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <string.h>
#include <grp.h>
#include <unistd.h>

#include "BaseLoginModule.h"
#include "PamLoginModule.h"
#include "assert.h"
#include "Subject.h"
#include "oec_values.h"
#include "arenatable.h"
#include "SKeyTable.h"
#include "Oec_AList.h"
#include "oec_log.h"
#include "mem.h"
#include "arena.h"

#define T PamLoginModule

struct T {

    Arena_T         arena;
    BaseLoginModule basemod;
    char *defgroup;
    char *pamdomain;
};

static void _login( T, Subject);
static void _free( T* );

T PamLoginModule_new(oe_scalar pamdomain, oe_scalar defgroup) {

    T _this_;
    Arena_T arena = Arena_new();
    _this_ = Arena_calloc(arena, 1, sizeof *_this_, __FILE__, __LINE__);
    _this_->defgroup = oec_cpy_str(arena, defgroup);
    _this_->arena = arena;
    _this_->basemod = BaseLoginModule_new(arena);
    _this_->pamdomain = oec_cpy_str(arena, pamdomain);

    return _this_;
}

Arena_T PamLoginModule_get_arena(T _this_) {
    return _this_->arena;
}

LoginModule PamLoginModule_new_module(T _this_) {

    return LoginModule_new(_login, _free, _this_);
}

static void _free( T* _this_p ) {
    T _this_ = *_this_p;
    Arena_dispose(&_this_->arena);
}

int converse(int n,
             const struct pam_message **msg,
             struct pam_response **resp,
             void *data) {

    char *pwd = (char *) data;

    struct pam_response *aresp;
    char buf[PAM_MAX_RESP_SIZE];
    int i;

    data = data;
    if (n <= 0 || n > PAM_MAX_NUM_MSG)
        return(PAM_CONV_ERR);
    if ((aresp = calloc(n, sizeof *aresp)) == NULL)
        return(PAM_BUF_ERR);
    for (i = 0; i < n; ++i) {
        aresp[i].resp_retcode = 0;
        aresp[i].resp = NULL;
        switch (msg[i]->msg_style) {
        case PAM_PROMPT_ECHO_OFF:
            OE_TLOG(0, "PAM calling converse PAM_PROMPT_ECHO_OFF: %s\n", msg[i]->msg);
            aresp[i].resp = strdup(pwd);
            if (aresp[i].resp == NULL)
                goto fail;
            break;
        case PAM_PROMPT_ECHO_ON:
            OE_TLOG(0, "calling converse PAM_PROMPT_ECHO_ON: %s\n", msg[i]->msg);
            fputs(msg[i]->msg, stderr);
            if (fgets(buf, sizeof buf, stdin) == NULL)
                goto fail;
            aresp[i].resp = strdup(buf);
            if (aresp[i].resp == NULL)
                goto fail;
            break;
        case PAM_ERROR_MSG:
            OE_TLOG(0, "calling converse PAM_ERROR_MSG\n");
            fputs(msg[i]->msg, stderr);
            if (strlen(msg[i]->msg) > 0 &&
                msg[i]->msg[strlen(msg[i]->msg) - 1] != '\n')
                fputc('\n', stderr);
            break;
        case PAM_TEXT_INFO:
            OE_TLOG(0, "calling converse PAM_TEXT_INFO: %s\n", msg[i]->msg);
            fputs(msg[i]->msg, stdout);
            if (strlen(msg[i]->msg) > 0 &&
                msg[i]->msg[strlen(msg[i]->msg) - 1] != '\n')
                fputc('\n', stdout);
            break;
        default:
            goto fail;
        }
    }
    *resp = aresp;
    return(PAM_SUCCESS);
    fail:
    for (i = 0; i < n; ++i) {
        if (aresp[i].resp != NULL) {
            memset(aresp[i].resp, 0, strlen(aresp[i].resp));
            free(aresp[i].resp);
        }
    }
    memset(aresp, 0, n * sizeof *aresp);
    *resp = NULL;
    return(PAM_CONV_ERR);
}

/**
    - attempt pam login
    - if login ok,
    - Subject_status_set(s, SUBJECT_AUTHENTICATED);
*/
static void _pamlogin( T _this_, Subject s) {
    assert(_this_);
    assert(s);
    oe_scalar username = Subject_get_username(s);
    if (username == NULL)  return;

    char *pwd = Subject_get_pwd(s);
    struct pam_conv conv = {
        converse,
        pwd
    };
    pam_handle_t *pamh=NULL;
    int retval;

    retval = pam_start(_this_->pamdomain, username, &conv, &pamh);

    if (retval == PAM_SUCCESS) {
        retval = pam_authenticate(pamh, 0);
    }

    if (retval == PAM_SUCCESS) {
        retval = pam_acct_mgmt(pamh, 0);
    }

    if (retval == PAM_SUCCESS) {
        Subject_status_set(s, SUBJECT_AUTHENTICATED);
    } 

    if (pam_end(pamh,retval) != PAM_SUCCESS) {
        pamh = NULL;
        OE_ERR(0, "PamLoginModule failed to release authenticator\n"); //i18n
    }
}

int _get_grplist(T _this_, Arena_T arena, oe_scalar user, oe_scalar** grplist) {

    long grpbufsz = sysconf(_SC_GETGR_R_SIZE_MAX);

    static int MAX_GRPS = 10;
    gid_t groups[MAX_GRPS];
    int ngroups = MAX_GRPS;
    int rc = getgrouplist(user, -1, &groups, &ngroups);
    if (rc <= 0) {
        OE_ILOG(0, "PamLoginModule _get_grplist: no groups for user: %s\n", user);
    }
    oe_scalar *tmplst = Arena_alloc(arena, rc + 2, __FILE__, __LINE__);
    int cnt = rc;
    int idx = 0;
    for (int i = 0; i < cnt; i++) {
        char *buf = Mem_alloc(grpbufsz, __FILE__, __LINE__);
        struct group group;
        struct group *result;
        rc = getgrgid_r(groups[i], &group, buf, grpbufsz, &result);
        if (!rc) {
            tmplst[idx] = oec_cpy_str(arena, (oe_scalar) group.gr_name);
            idx++;
        }
        Mem_free(buf, __FILE__, __LINE__);
    }
    tmplst[idx] = _this_->defgroup == NULL ? NULL : oec_cpy_str(arena, _this_->defgroup);
    tmplst[++idx] = NULL;
    *grplist = tmplst;
    return idx;
}

/**
    - get grps from /etc/groups.db (or wherever, todo:
      configurable db??)
    - BaseLoginModule_assign_privs(_this_->basemod, s, grouplst);
*/
static void _addprivs( T _this_, Subject s) {
    assert(_this_);
    assert(s);
    oe_scalar username = Subject_get_username(s);
    if (username == NULL)  return;
    oe_scalar *grplist;
    Arena_T tmparena = Arena_new();
    int cnt = _get_grplist(_this_, tmparena, username, &grplist);
    BaseLoginModule_assign_privs(_this_->basemod, s, grplist);
    Arena_dispose(&tmparena);
}

/**
    - attempt pam login
    - if login ok,
    - Subject_status_set(s, SUBJECT_AUTHENTICATED);
    - get grps from /etc/groups.db (or wherever, todo: configurable db??)
    - BaseLoginModule_assign_privs(_this_->basemod, s, grouplst);
*/
static void _login( T _this_, Subject s) {
    assert(_this_);
    assert(s);
    _pamlogin(_this_,s);
    if (!Subject_is_authenticated(s)) {
        OE_TLOG(0, "PamLoginModule _login '%s' NOT successfully authenticated\n", Subject_get_username(s)); //i18n
        return;
    }
    _addprivs(_this_,s);
}

int PamLoginModule_add_privilege( T _this_, 
                                  oe_scalar extentname,
                                  oe_scalar groupname,
                                  OekMsg mtemplate) {
    return BaseLoginModule_add_privilege(_this_->basemod, extentname, groupname, mtemplate);
}

#undef T

