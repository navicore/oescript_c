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
#include "oec_log.h"
#include "mem.h"
#undef NDEBUG
#include <assert.h>

#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <stdbool.h>
#include <grp.h>
#include <unistd.h>

static char* pwd = "oeuser1";
static char *user="oeuser1";

int converse(int n,
             const struct pam_message **msg,
             struct pam_response **resp,
             void *data) {

	struct pam_response *aresp;
	char buf[PAM_MAX_RESP_SIZE];
	int i;

	data = data;
	if (n <= 0 || n > PAM_MAX_NUM_MSG)
		return (PAM_CONV_ERR);
	if ((aresp = calloc(n, sizeof *aresp)) == NULL)
		return (PAM_BUF_ERR);
	for (i = 0; i < n; ++i) {
		aresp[i].resp_retcode = 0;
		aresp[i].resp = NULL;
		switch (msg[i]->msg_style) {
        case PAM_PROMPT_ECHO_OFF:
            OE_TLOG(0, "PAM calling converse PAM_PROMPT_ECHO_OFF: %s\n", msg[i]->msg);
			//aresp[i].resp = strdup(getpass(msg[i]->msg));
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
	return (PAM_SUCCESS);
 fail:
        for (i = 0; i < n; ++i) {
                if (aresp[i].resp != NULL) {
                        memset(aresp[i].resp, 0, strlen(aresp[i].resp));
                        free(aresp[i].resp);
                }
        }
        memset(aresp, 0, n * sizeof *aresp);
	*resp = NULL;
	return (PAM_CONV_ERR);
}

static struct pam_conv conv = {
    converse,
    NULL
};


void test_group_list2() {

    long grpbufsz = sysconf(_SC_GETGR_R_SIZE_MAX);

    static int MAX_GRPS = 10;
    gid_t groups[MAX_GRPS];
    int ngroups = MAX_GRPS;
    printf("ngroups b4: %i\n", ngroups);
    int rc = getgrouplist(user, -1, &groups, &ngroups);
    printf("rc: %i\n", rc);
    assert(rc > 0);
    printf("ngroups: %i\n", ngroups);
    for (int i = 0; i < ngroups; i++) {
        printf("looking up %i\n", groups[i]);
        char *buf = Mem_alloc(grpbufsz, __FILE__, __LINE__);
        struct group group;
        struct group *result;
        int rc = getgrgid_r(groups[i], &group, buf, grpbufsz, &result);
        //assert(rc > 0);
        printf("g == r: %s\n", result == &group ? "true" : "false");
        if (!rc) {
            printf("group name: %s\n", group.gr_name);
        }
        Mem_free(buf, __FILE__, __LINE__);
    }
}

void test_group_list() {
    static int MAX_GRPS = 10;
    gid_t groups[MAX_GRPS];
    int ngroups = MAX_GRPS;
    printf("ngroups b4: %i\n", ngroups);
    int rc = getgrouplist(user, -1, &groups, &ngroups);
    printf("rc: %i\n", rc);
    assert(rc > 0);
    printf("ngroups: %i\n", ngroups);
    for (int i = 0; i < ngroups; i++) {
        printf("looking up %i\n", groups[i]);
        struct group *group = getgrgid(groups[i]);
        if (group != NULL) {
            printf("group name: %s\n", group->gr_name);
        }
    }
}

void test_pam_login() {

    pam_handle_t *pamh=NULL;
    int retval;

    retval = pam_start("oescript", user, &conv, &pamh);

    if (retval == PAM_SUCCESS) {
        printf("calling auth\n");
        retval = pam_authenticate(pamh, 0);
    }

    if (retval == PAM_SUCCESS) {
        printf("calling actmgt\n");
        retval = pam_acct_mgmt(pamh, 0);
    }

    if (retval == PAM_SUCCESS) {
        fprintf(stdout, "Authenticated\n");
    } else {
        fprintf(stdout, "Not Authenticated\n");
        assert(false);
    }

    if (pam_end(pamh,retval) != PAM_SUCCESS) {
        pamh = NULL;
        fprintf(stderr, "oescript: failed to release authenticator\n");
        exit(1);
    }

    return( retval == PAM_SUCCESS ? 0:1 );
}

int main(int argc, char *argv[]) {
  /*
    test_pam_login();
    test_group_list2();
   */
    return 0;
}

