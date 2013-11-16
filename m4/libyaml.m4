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

#        #include YAML_HEADER

AC_DEFUN([AX_LIBYAML],
[
  AC_ARG_WITH([yaml-header],
      [  --with-yaml-header=PATH Set the name of the libyaml header file],
      [YAML_HEADER="$withval"]
      )

  AC_ARG_WITH([yaml-lib],
      [  --with-yaml-lib=PATH    Set the name of the libyaml lib],
      [YAML_LIBS="-l$withval"]
      )

  #presuming if YAML_HEADER overide is set that you know what you are doing
  if test -z $YAML_HEADER ; then #override skipping

  old_LIBS="$LIBS"

  AC_CHECK_HEADERS([yaml.h],
      [YAML_HEADER="yaml.h"],
      [AC_MSG_WARN([no yaml.h found. Please install and/or set CFLAGS and LDFLAGS])]
      )

  AC_SEARCH_LIBS([yaml_emitter_initialize],[yaml-1.1 yaml],
      [YAML_LIBS="$LIBS"],
      [AC_MSG_WARN([libyaml not found])]
      )

  LIBS="$old_LIBS"

  fi #end override skipping

  if test -z $YAML_HEADER ; then
    AM_CONDITIONAL(OE_USE_YAML,false)
    AC_MSG_RESULT([libyaml not found. OE_USE_YAML=false])
    ifelse([$3], , :, [$3])
  else
    AM_CONDITIONAL(OE_USE_YAML,true)
    AC_MSG_RESULT([libyaml found. OE_USE_YAML=true])
    AC_DEFINE([OE_USE_YAML],[],[compile support for libyaml])
    AC_DEFINE_UNQUOTED(YAML_HEADER, ["$YAML_HEADER"], ["libyaml header file"])
    AC_SUBST(YAML_LIBS)
    ifelse([$2], , :, [$2])
  fi
])

