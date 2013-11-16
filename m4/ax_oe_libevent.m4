# The contents of this file are subject to the Apache License
# Version 2.0 (the "License"); you may not use this file except in
# compliance with the License. You may obtain a copy of the License 
# from the file named COPYING and from http://www.apache.org/licenses/.
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
# The Original Code is OeScript.
# 
# The Initial Developer of the Original Code is OnExtent, LLC.
# Portions created by OnExtent, LLC are Copyright (C) 2008-2009
# OnExtent, LLC. All Rights Reserved.

AC_DEFUN([AX_LIBEVENT],
[

  AC_ARG_WITH([event-cflags],
      [  --with-event-cflags=PATH   Set the libevent cflags],
      [EVENT_CFLAGS="-l$withval"]
      )

  AC_ARG_WITH([event-lib],
      [  --with-event-lib=PATH   Set the name of the libevent lib],
      [EVENT_LIBS="-l$withval"]
      )

  #presuming if EVENT_CFLAGS overide is set that you know what you are doing
  if test -z $EVENT_CFLAGS ; then #override skipping

  EVENT_CFLAGS=`pkg-config libevent --cflags`
  EVENT_LIBS=`pkg-config libevent --libs`

  old_LIBS="$LIBS"
  old_CFLAGS="$CFLAGS"

  CFLAGS="$EVENT_CFLAGS $CFLAGS"
  LIBS="$EVENT_LIBS $LIBS"

  AC_CHECK_HEADERS([event.h],
      [],
      [AC_MSG_WARN([no event.h found. Please install and/or set CFLAGS and LDFLAGS])]
      )

  AC_CHECK_HEADERS([event2/bufferevent_ssl.h],
      [],
      [AC_MSG_WARN([no bufferevent_ssl.h found. Please install and/or set CFLAGS and LDFLAGS])]
      )

  AC_CHECK_HEADERS([event2/listener.h],
      [],
      [AC_MSG_WARN([no listener.h found. Please install and/or set CFLAGS and LDFLAGS])]
      )

  AC_SEARCH_LIBS([event_loop],[event event2],
      [EVENT_LIBS="$EVENT_LIBS $LIBS"],
      [AC_MSG_WARN([libevent lib not found])]
      )

  AC_SEARCH_LIBS([bufferevent_openssl_get_ssl],[event event_openssl event2],
      [EVENT_LIBS="$EVENT_LIBS $LIBS"],
      [AC_MSG_WARN([libevent openssl lib not found])]
      )

  LIBS="$old_LIBS"
  CFLAGS="$old_CFLAGS"

  fi #end override skipping

  AM_CONDITIONAL(OE_USE_LIBEVENT,true)
  AC_DEFINE([OE_USE_LIBEVENT],[],[compile support for libevent])
  AC_DEFINE_UNQUOTED(EVENT_CFLAGS, ["$EVENT_CFLAGS"], ["libevent cflags"])
  AC_DEFINE_UNQUOTED(EVENT_LIBS, ["$EVENT_LIBS"], ["libevent cflags"])
  AC_SUBST([EVENT_CFLAGS])
  AC_SUBST(EVENT_LIBS)
])

