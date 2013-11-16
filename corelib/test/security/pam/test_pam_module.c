/* The contents of this file are subject to the Apache License
 * Version 2.0 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License 
 * from the file named COPYING and from http://www.apache.org/licenses/.
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * The Original Code is OeScript.
 *
 * The Initial Developer of the Original Code is OnExtent, LLC.
 * Portions created by OnExtent, LLC are Copyright (C) 2008-2011
 * OnExtent, LLC. All Rights Reserved.
 *
 */
#include "config.h"
#include "arena.h"
#include "Subject.h"
#include "PamLoginModule.h"
#undef NDEBUG
#include <assert.h>

static Arena_T _arena;
static Subject _subject;
static Subject _subject2;
static PamLoginModule _pam_mod;
static PamLoginModule _pam_mod2;
static LoginModule _login_mod;
static LoginModule _login_mod2;
static char *_extname = "myspace";
static char *_gname_lst = "something,oescript,ledzep,myledzep,yourledzip!";
/*
tweak these to make sure default works independently of /etc/grp name
*/
static char *_defgname = "oescript991";
static char *_privgname = "oescript";

static char *_uname = "Doghouse Reilly";
static char *_pwd = "john bonham rocks";
static char *_uname2 = "oeuser1";
static char *_pwd2 = "oeuser1";

static AUTH _test_auth(Subject subject, OekMsg msg) {

    AUTH auth = DONTCARE;
    for ( ArrayIterator iter = Subject_get_privileges(subject); ArrayIterator_hasMore(iter); ) {
        Privilege p = ArrayIterator_next(iter);
        auth = Privilege_allow(p, msg);
        if (auth != DONTCARE) {
            break;
        }
    }
    return auth;
}

static void _init() {
    _arena = Arena_new();
    Subject s = Subject_new(_arena);
    Subject_set_extentname(s, _extname);
    Subject_set_username(s, _uname);
    Subject_set_pwd(s, _pwd);
    _subject = s; //for other tests
    _arena = Arena_new();
    s = Subject_new(_arena);
    Subject_set_extentname(s, _extname);
    Subject_set_username(s, _uname2);
    Subject_set_pwd(s, _pwd2);
    _subject2 = s; //for other tests
}

void test_pam_module_api() {

    PamLoginModule m = PamLoginModule_new("oescript", _defgname);
    _pam_mod = m;
    _login_mod = PamLoginModule_new_module(m);
    OekMsg mtemplate = OekMsg_new(GET_CMD, _arena);
    PamLoginModule_add_privilege(_pam_mod, _extname, _privgname, mtemplate);

    PamLoginModule m2 = PamLoginModule_new("oescript", "neverland");
    _pam_mod2 = m2;
    _login_mod2 = PamLoginModule_new_module(m2);
}

void test_login_module_deny() {

    LoginModule_login(_login_mod, _subject);
    assert(!Subject_is_authenticated(_subject));
    ArrayIterator iter = Subject_get_privileges(_subject);
    assert(!ArrayIterator_hasMore(iter));
    ArrayIterator_free(&iter);
}

void test_login_module_allow() {

    LoginModule_login(_login_mod, _subject2);
    assert(Subject_is_authenticated(_subject2));
}

void test_login_module_priv_deny() {

    assert(_test_auth(_subject, OekMsg_new(GET_CMD, _arena)) == DONTCARE);
    assert(_test_auth(_subject, OekMsg_new(PUT_CMD, _arena)) == DONTCARE);
    assert(_test_auth(_subject2, OekMsg_new(PUT_CMD, _arena)) == DONTCARE);
    assert(_test_auth(_subject2, OekMsg_new(GET_CMD, _arena)) == ALLOW);
}

int main() {

    /*
    _init();
    test_pam_module_api();
    assert(_pam_mod);
    assert(_login_mod);

    test_login_module_deny();
    test_login_module_allow();

    test_login_module_priv_deny();
    */

    return 0;
}

