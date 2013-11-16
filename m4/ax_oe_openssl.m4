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

AC_DEFUN([AX_OPENSSL],
[
  AC_ARG_WITH([openssl-cflags],
      [  --with-openssl-cflags=PATH TO ssl.h],
      [OPENSSL_CFLAGS="$withval"]
      )

  AC_ARG_WITH([openssl-libs],
      [  --with-openssl-libs=PATH TO libopenssl.so],
      [OPENSSL_LIBS="$withval"]
      )

  #presuming if OPENSSL_CFLAGS overide is set that you know what you are doing
  if test -z $OPENSSL_CFLAGS ; then #override skipping

  OPENSSL_CFLAGS=`pkg-config openssl --cflags`
  OPENSSL_LIBS=`pkg-config openssl --libs`

  old_LIBS="$LIBS"
  old_CFLAGS="$CFLAGS"

  AC_CHECK_HEADERS([openssl/ssl.h],
      [],
      [AC_MSG_WARN([no openssl ssl.h found. Please install and/or set CFLAGS and LDFLAGS])]
      )

  AC_CHECK_HEADERS([openssl/err.h],
      [],
      [AC_MSG_WARN([no openssl err.h found. Please install and/or set CFLAGS and LDFLAGS])]
      )

  AC_CHECK_HEADERS([openssl/rand.h],
      [],
      [AC_MSG_WARN([no openssl rand.h found. Please install and/or set CFLAGS and LDFLAGS])]
      )

  AC_SEARCH_LIBS([SSL_shutdown],[ssl],
      [OPENSSL_LIBS="$OPENSSL_LIBS $LIBS"],
      [AC_MSG_WARN([openssl lib not found])]
      )

  LIBS="$old_LIBS"
  CFLAGS="$old_CFLAGS"

  fi #end override skipping

  AM_CONDITIONAL(OE_USE_OPENSSL,true)
  AC_DEFINE([OE_USE_OPENSSL],[],[compile support for openssl])
  AC_DEFINE_UNQUOTED(OPENSSL_CFLAGS, ["$OPENSSL_CFLAGS"], ["openssl cflags"])
  AC_SUBST([OPENSSL_CFLAGS])
  AC_SUBST(OPENSSL_LIBS)
])

