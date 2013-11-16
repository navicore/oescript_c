/* 
 * OeScript http://www.oescript.net
 * Copyright 2012 Ed Sweeney, all rights reserved.
 */

#include "LoginModule.h"
#include "mem.h"
#include "assert.h"
#include "Subject.h"

#define T LoginModule

T LoginModule_new(
    LOGIN_MODULE_login *login_func, 
    LOGIN_MODULE_free *free_func, 
                  void *impl) {

    assert(login_func);
    assert(impl);
    T _this_;
    _this_ = Mem_calloc(1, sizeof *_this_, __FILE__, __LINE__);
    _this_->login_func = login_func;
    _this_->free_func = free_func;
    _this_->impl = impl;

    return _this_;
}

void LoginModule_login( T _this_, Subject s) {
    assert(_this_->login_func);
    assert(_this_->impl);
    assert(s);
    _this_->login_func(_this_->impl, s);
}

void LoginModule_free( T* _this_p ) {
    T _this_ = *_this_p;
    if (_this_->free_func) {
        _this_->free_func(_this_->impl);
    }
    Mem_free(_this_, __FILE__, __LINE__);
}

#undef T

