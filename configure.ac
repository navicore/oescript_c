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
# Portions created by OnExtent, LLC are Copyright (C) 2008-2010
# OnExtent, LLC. All Rights Reserved.

AC_PREREQ(2.60)
AC_INIT([OeScript Pre Alpha], [0.5.4], [info@onextent.com])
AC_DEFINE([OE_VERSION_MAJ],[0],[server major version])
AC_DEFINE([OE_VERSION_MIN],[5],[server minor version])
AC_DEFINE([OE_VERSION_UPT],[4],[server update version])
AC_CONFIG_AUX_DIR([scripts])
AC_PROG_LIBTOOL
AM_INIT_AUTOMAKE( [-Wall] )
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])
if test "$INSTALL" = "${srcdir}/scripts/install-sh -c"; then
  INSTALL='\$(..)./scripts/install-sh -c'
fi
AM_PROG_CC_C_O
AC_PROG_CC_C99
AC_C_BIGENDIAN                                                                  
# AC_PREFIX_DEFAULT(/opt/onextent/server)
AC_PREFIX_DEFAULT(/usr/local)
AC_PROG_INSTALL

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
    AC_FUNC_ERROR_AT_LINE
    AC_FUNC_FORK
    AC_FUNC_MALLOC
    AC_FUNC_MKTIME
    AC_FUNC_REALLOC
    AC_FUNC_STRCOLL
    AC_CHECK_FUNCS([floor gethostbyname gettimeofday localeconv memchr memset modf pow setlocale socket sqrt strchr strcspn strerror strpbrk strrchr strstr strtoul])

#=============================================================================#
#                               configure lua                                 #
#=============================================================================#

CHECK_OE_LUA([AC_MSG_RESULT([found lua])],[AC_MSG_ERROR([can not find lua])])

#=============================================================================#
#                       configure arena alignment                             #
#=============================================================================#
AC_ARG_WITH([max_align],
    [  --with-max-align=SIZE   Arena memory max alignment padding size],
	[AC_MSG_RESULT([using --with-max-align. arena max alignment padding size $withval])
    AC_DEFINE_UNQUOTED([MAXALIGN],[${withval}],[arena max alignment])]
    )

#=============================================================================#
#                           configure oe debuging                             #
#=============================================================================#
_debug=yes
AC_ARG_ENABLE([nodebug],
        [  --enable-nodebug        Turn off compiling with asserts and
                          compiler debug switches. default is with debug],
	[AC_MSG_RESULT([using --enable-nodebug. asserts and gdb info turned off])
    _debug=no]
    )

AC_ARG_ENABLE([debug-log],
        [  --enable-debug-log      Same as debug but with logging],
	[AC_MSG_RESULT([using --enable-debug-log. asserts, gdb and logging turned on])
    AC_DEFINE([OE_DEBUG],[],[Log debug info to console])
    _debug=yes]
    )

AC_ARG_ENABLE([trace-log],
        [  --enable-trace-log      Same as debug-log but with extra logging],
	[AC_MSG_RESULT([using --enable-trace-log. asserts, gdb and extra logging turned on])
    AC_DEFINE([OE_DEBUG],[],[Log debug info to console])
    AC_DEFINE([OE_TRACE],[],[Log trace info to console])
    _debug=yes]
    )

if test x$_debug = xno ; then
AC_DEFINE([NDEBUG],[],[do not compile in support for asserts])
AC_MSG_RESULT([NDEBUG is set. debug is turned off])
else
AC_DEFINE([LUA_USE_APICHECK],[],[perform lua api checking asserts])
AC_MSG_RESULT([                    lua api checking turned on])

#CFLAGS="-g -O0 -pthread"
CFLAGS="-g -pthread $CFLAGS"
#CFLAGS="-pthread $CFLAGS"

AC_MSG_RESULT([debug is turned on])
fi

_syslog=no
AC_ARG_ENABLE([syslog],
        [  --enable-syslog         log to syslog],
	[AC_MSG_RESULT([using --enable-syslog, logging to syslog])
    _syslog=yes]
    )
if test x$_syslog = xyes ; then
AC_DEFINE([OE_SYSLOG],[],[compiling in support for syslog])
AC_MSG_RESULT([OE_SYSLOG is set. syslog support is on])
else
AC_MSG_RESULT([console logging is on])
fi


CFLAGS="-Wall -Wno-unused $CFLAGS"

#experiment, don't like it, remove it
CFLAGS="$CFLAGS -I/usr/pkg/include -I/usr/local/include -I/opt/local/include -I/usr/include"
LDFLAGS="$LDFLAGS -L/usr/pkg/lib -L/usr/local/lib -L/opt/local/lib -L/usr/lib"

AC_DEFINE([LUA_USE_ULONGJMP],[],[for lua to try _longjmp and _setjmp])

_install_shared_corelib=no
AC_ARG_ENABLE([install-corelib],
        [  --enable-install-corelib so you can link to oescript with C/C++],
	[AC_MSG_RESULT([using --enable-install-corelib])
    _install_shared_corelib=yes]
    )
if test x$_install_shared_corelib = xyes ; then
AM_CONDITIONAL(INSTALL_SHARED_CORELIB, true)
#OE_LIB_LDFLAGS="--gc-sections"
#OE_LIB_CFLAGS="-fvisibility=hidden"
OE_LIB_LDFLAGS=""
OE_LIB_CFLAGS=""
else
AM_CONDITIONAL(INSTALL_SHARED_CORELIB, false)
OE_LIB_LDFLAGS=""
OE_LIB_CFLAGS=""
fi

_static_tests=no
AC_ARG_ENABLE([static-tests],
        [  --enable-static-tests   Use '-static' on unit tests. valgrind and gdb
                          don't like libtool scripts],
	[AC_MSG_RESULT([using --enable-static-tests])
    _static_tests=yes]
    )
if test x$_static_tests = xyes ; then
AC_SUBST(OE_TESTER_LDFLAGS, "-static")
AC_SUBST(OE_TESTER_CFLAGS, "")
else
AC_SUBST(OE_TESTER_LDFLAGS, "")
AC_SUBST(OE_TESTER_CFLAGS, "")
fi

_static_bin=no
AC_ARG_ENABLE([static-bin],
        [  --enable-static-bin     Use '-static' on bin dir entries],
	[AC_MSG_RESULT([using --enable-static-bin])
    _static_bin=yes]
    )
if test x$_static_bin = xyes ; then
AC_SUBST(OE_BIN_LDFLAGS, "-static")
AC_SUBST(OE_BIN_CFLAGS, "")
else
AC_SUBST(OE_BIN_LDFLAGS, "")
AC_SUBST(OE_BIN_CFLAGS, "")
fi

#=============================================================================#
#                           configure oe threading                            #
#=============================================================================#
use_threads=yes
AC_ARG_ENABLE([nothreads],
        [  --enable-nothreads      Specify whether to compile use of posix
                          threads. default is with threads],
	[AC_MSG_RESULT([using --enable-nothreads. server will run in a single thread.])
    use_threads=no]
    )

if test x$use_threads = xyes ; then
AC_CHECK_LIB([pthread],[pthread_create])
AM_CONDITIONAL(OE_USE_THREADS, true)
AC_DEFINE([OE_USE_THREADS],[],[compile in support for posix threads])
else
AM_CONDITIONAL(OE_USE_THREADS, false)
AC_DEFINE([OE_NOTHREADS],[],[do not compile in support for posix threads])
fi

AC_CHECK_FUNCS([memmem])

#=============================================================================#
#                   configure compiling for coverage tests                    #
#=============================================================================#
coverage_testing=no
AC_ARG_ENABLE([coverage],
        [  --enable-coverage       Compile to support code coverage testing and reporting],
	[AC_MSG_RESULT([using --enable-coverage. make sure you also have --with-nodebug set.])
    coverage_testing=yes]
    )

if test x$coverage_testing = xyes ; then
AM_CONDITIONAL(OE_COVERAGE, true)
AC_DEFINE([OE_COVERAGE],[],[compiled with support for code coverage testing])
else
AM_CONDITIONAL(OE_COVERAGE, false)
fi

#=============================================================================#
#                                configure shell                              #
#=============================================================================#
use_readline=yes
AC_ARG_WITH([noreadline],
        [  --with-noreadline       specify whether to use readline],
	[AC_MSG_RESULT([using --with-noreadline.])
    use_readline=no]
    )

if test x$use_readline = xyes ; then
AC_SEARCH_LIBS([add_history], [edit libedit readline],
        [OE_READLINE_LIBS=$LIBS
        AC_DEFINE([LUA_USE_READLINE],[],
            [lua shell will compile support for readline])
        AC_MSG_RESULT([using readline.  do not redistribute binaries.])])
AC_SUBST(OE_READLINE_LIBS)
LIBS=
fi

#=============================================================================#
#                               configure ssl                                 #
#=============================================================================#
use_ssl=yes

AC_ARG_WITH([nossl],
        [  --with-nossl       specify to skip building with openssl support],
	[AC_MSG_RESULT([using --with-nossl.])
    use_ssl=no]
    )

if test x$use_ssl = xyes ; then
AM_CONDITIONAL(OE_USE_SSL,true)
OE_LIB_LDFLAGS="$OE_LIB_LDFLAGS -lssl -lcrypto"
AC_DEFINE([OE_USE_SSL],[],[compile in support for ssl])
else
AM_CONDITIONAL(OE_USE_SSL,false)
fi

#=============================================================================#
#                               configure pam                                 #
#=============================================================================#
use_pam=yes

AC_ARG_WITH([nopam],
        [  --with-nopam       specify to skip building pam support],
	[AC_MSG_RESULT([using --with-nopam.])
    use_pam=no]
    )

if test x$use_pam = xyes ; then
AM_CONDITIONAL(OE_USE_PAM,true)
OE_LIB_LDFLAGS="$OE_LIB_LDFLAGS -lpam -lpam_misc"
AC_DEFINE([OE_USE_PAM],[],[compile in support for pam])
else
AM_CONDITIONAL(OE_USE_PAM,false)
fi

#=============================================================================#
#                               configure compiler                            #
#=============================================================================#
echo "setting compiler flag variables.  build_vendor is $build_vendor build_os is $build_os"

OE_APP_CFLAGS=""
OE_APP_LDFLAGS=""
OE_LIB_LDFLAGS="$OE_LIB_LDFLAGS -release 0.4"
#OE_LIB_CFLAGS=""
#        darwin*)        OE_LIB_LDFLAGS="$OE_LIB_LDFLAGS -module -avoid-version" && OE_LIB_CFLAGS="$OE_LIB_CFLAGS";;
case "$host_os" in
        darwin*)        OE_LIB_LDFLAGS="$OE_LIB_LDFLAGS" && OE_LIB_CFLAGS="$OE_LIB_CFLAGS";;
        *)              ;;
esac
AC_SUBST(OE_LIB_LDFLAGS)
AC_SUBST(OE_LIB_CFLAGS)
AC_SUBST(OE_APP_CFLAGS)
AC_SUBST(OE_APP_LDFLAGS)

#=============================================================================#
#                                                                             #
#                                                                             #
#                              configure stores                               #
#                                                                             #
#                                                                             #
#=============================================================================#

#=============================================================================#
#                               configure db                                  #
#=============================================================================#
use_berkeleydb=yes

AC_ARG_WITH([berkeleydb],
        [  --with-berkeleydb       specify to use berkeleydb],
	[AC_MSG_RESULT([using --with-berkeleydb.])
    use_berkeleydb=yes]
    )

if test x$use_berkeleydb = xyes ; then
AM_CONDITIONAL(OE_USE_BDB,true)
AC_MSG_CHECKING([for berkeleydb installation])
AX_BERKELEY_DB([4.7],
        [OE_STORE_APIS="${OE_STORE_APIS} bdb"],
        [AC_MSG_WARN([Install libdb and/or set CFLAGS and LDFLAGS to help us find it.])]
        )
else
AM_CONDITIONAL(OE_USE_BDB,false)
fi

#=============================================================================#
#                              configure sqlite3                              #
#=============================================================================#
if test -n "$OE_STORE_APIS"; then
    AM_CONDITIONAL(OE_USE_SQLITE3,false)
	AC_MSG_RESULT([not building with sqlite3.])
else
use_sqlite3=yes
AM_CONDITIONAL(OE_USE_SQLITE3,true)
AC_MSG_CHECKING([for sqlite3 installation])

AC_ARG_WITH([sqlite3],
        [  --with-sqlite3          specify to use sqlite3],
	[AC_MSG_RESULT([using --with-sqlite3.])
    use_sqlite3=yes]
    )

if test x$use_sqlite3 = xyes ; then
AX_OE_SQLITE_THREADSAFE()
OE_STORE_APIS="${OE_STORE_APIS} sqlite3"
fi

fi

#=============================================================================#
AC_MSG_CHECKING([for at least one store API])
if test -n "$OE_STORE_APIS"; then
  AC_MSG_RESULT([Store APIs: $OE_STORE_APIS])
else
  AC_MSG_ERROR([No store APIs will be built. See WARNING msgs above.])
fi
AC_SUBST(OE_STORE_APIS)

#=============================================================================#
#                                                                             #
#                                                                             #
#                             configure networking                            #
#                                                                             #
#                                                                             #
#=============================================================================#

#=============================================================================#
#                              configure openssl                              #
#=============================================================================#

AX_OPENSSL([1.0],
        [],
        [AC_MSG_ERROR([Install openssl and/or set CFLAGS and LDFLAGS to help us find it.])]
        )

#=============================================================================#
#                              configure libevent                             #
#=============================================================================#

AX_LIBEVENT([2.0],
        [],
        [AC_MSG_ERROR([Install libevent and/or set CFLAGS and LDFLAGS to help us find it.])]
        )

#=============================================================================#
#                                                                             #
#                                                                             #
#                             configure protocols                             #
#                                                                             #
#                                                                             #
#=============================================================================#

#build oesp by default
OE_LIST_OF_PROTOCOLS_TO_BUILD=""

#=============================================================================#
#                                                                             #
# default support for json
#                                                                             #
#=============================================================================#
OE_LIST_OF_PROTOCOLS_TO_BUILD="${OE_LIST_OF_PROTOCOLS_TO_BUILD} json"

#=============================================================================#
#                                                                             #
#                                                                             #
#                    configure scriping APIs via SWIG                         #
#                                                                             #
#                                                                             #
#=============================================================================#

case "$host_os" in
        darwin*)        OE_SHSUF=".dylib";;
        *)              OE_SHSUF=".so";;
esac
AC_SUBST([OE_SHSUF])


AX_PKG_SWIG(1.3.17, [], [AC_MSG_RESULT([SWIG not found.  This is fine unless you have modified any *.i files.]) ])
OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD=
OE_LIST_OF_SCRIPTING_APIS_TO_BUILD=

#=============================================================================#
#                            configure perl support                           #
#=============================================================================#
_with_perl=no
AC_ARG_WITH([perl],
        [  --with-perl             build perl api],
	[AC_MSG_RESULT([building --with-perl.])
    _with_perl=yes]
    )

if test x$_with_perl = xyes ; then
AX_PERL_EXT_FLAGS([PERLXS_CFLAGS], [PERLXS_LDFLAGS])
AC_SUBST([PERLXS_CFLAGS])
AC_SUBST([PERLXS_LDFLAGS])
OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="perl $OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD"
fi

_perl_module_install=no
AC_ARG_ENABLE([perl-modinst],
        [  --enable-perl-modinst   Specify whether to install the perl
                          module into the site_perl dirs],
	[AC_MSG_RESULT([using --enable-perlmod-inst. installing perl module into site_perl])
    _perl_module_install=yes]
    )

if test x$_perl_module_install = xyes ; then
AM_CONDITIONAL(OE_INSTALL_PERL_MODULE, true)
else
AM_CONDITIONAL(OE_INSTALL_PERL_MODULE, false)
fi

#=============================================================================#
#                           configure python support                          #
#=============================================================================#
_with_python=no
AC_ARG_WITH([python],
        [  --with-python           build python api],
	[AC_MSG_RESULT([building --with-python.])
    _with_python=yes]
    )

if test x$_with_python = xyes ; then
#AX_PYTHON_DEVEL([>='3.1.0'])
AX_PYTHON_DEVEL([>='2.7.0'])
AC_SUBST([PYTHON_CPPFLAGS])
AC_SUBST([PYTHON_LDFLAGS])
AC_SUBST([PYTHON_EXTRA_LIBS])
AC_SUBST([PYTHON_EXTRA_LDFLAGS])
OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="$OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD python"
fi

_python_module_install=no
AC_ARG_ENABLE([python-modinst],
        [  --enable-python-modinst   Specify whether to install the python
                          module into site-packages],
    [AC_MSG_RESULT([using --enable-pythonmod-inst. installing pyton module site-packages])
    _python_module_install=yes]
    )

if test x$_python_module_install = xyes ; then
AM_CONDITIONAL(OE_INSTALL_PYTHON_MODULE, true)
else
AM_CONDITIONAL(OE_INSTALL_PYTHON_MODULE, false)
fi

#=============================================================================#
#                           configure tcl support                             #
#=============================================================================#
_with_tcl=no
AC_ARG_WITH([tcl],
        [  --with-tcl              build tcl api],
	[AC_MSG_RESULT([building --with-tcl.])
    _with_tcl=yes]
    )

if test x$_with_tcl = xyes ; then
AX_PROG_TCL([8.3])
AX_OE_TCL([])

OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="$OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD tcl"
fi

#=============================================================================#
#                            configure ruby support                           #
#=============================================================================#
_with_ruby=no
AC_ARG_WITH([ruby],
        [  --with-ruby             build ruby api (** not implemented yet **)],
	[AC_MSG_RESULT([building --with-ruby.])
    _with_ruby=yes]
    )

if test x$_with_ruby = xyes ; then
AX_RUBY_DEVEL([])
AC_SUBST([RUBY_CPPFLAGS])
AC_SUBST([RUBY_LDFLAGS])
AC_SUBST([RUBY_EXTRA_LIBS])
OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="$OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD ruby"
fi

#=============================================================================#
#                         configure node.js support                           #
#=============================================================================#
_with_nodejs=no
AC_ARG_WITH([nodejs],
        [  --with-nodejs             build node.js api],
	[AC_MSG_RESULT([building --with-nodejs.])
    _with_nodejs=yes]
    )

if test x$_with_nodejs = xyes ; then
OE_LIST_OF_SCRIPTING_APIS_TO_BUILD="nodejs $OE_LIST_OF_SCRIPTING_APIS_TO_BUILD"
fi

#=============================================================================#
#                            configure java support                           #
#=============================================================================#
_with_java=no
AC_ARG_WITH([java],
        [  --with-java             build java api],
	[AC_MSG_RESULT([building --with-java.])
    _with_java=yes]
    )

if test x$_with_java = xyes ; then

OE_LIST_OF_SCRIPTING_APIS_TO_BUILD="java $OE_LIST_OF_SCRIPTING_APIS_TO_BUILD"

AX_OEJAVA

for JNI_INCLUDE_DIR in $JNI_INCLUDE_DIRS
do
	JNI_CFLAGS="$JNI_CFLAGS -I$JNI_INCLUDE_DIR"
done

AC_SUBST([JNI_CFLAGS])
AC_SUBST([JAVAC])
AC_MSG_RESULT([replaced @JAVAC@ with $JAVAC])
AC_SUBST([JAVA])
AC_MSG_RESULT([replaced @JAVA@ with $JAVA])
AC_SUBST([JAVA_HOME])
AC_MSG_RESULT([replaced @JAVA_HOME@ with $JAVA_HOME])

OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="$OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD java"
fi

#=============================================================================#
#                            configure php support                            #
#=============================================================================#
_with_php=no
AC_ARG_WITH([php],
        [  --with-php              build php api],
	[AC_MSG_RESULT([building --with-php.])
    _with_php=yes]
    )

if test x$_with_php = xyes ; then
AX_OE_PHP_DEVEL([])
AC_SUBST([PHP_CFLAGS])
AC_SUBST([PHP_LDFLAGS])
OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="$OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD php"
fi

#=============================================================================#
#                            configure lua support                            #
#=============================================================================#
_with_swig_lua=no
AC_ARG_WITH([lua],
        [  --with-lua              build lua api],
	[AC_MSG_RESULT([building --with-lua.])
    _with_swig_lua=yes]
    )

if test x$_with_swig_lua = xyes ; then
OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="$OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD lua"
fi

#=============================================================================#
#                           configure guile support                           #
#=============================================================================#
_with_guile=no
AC_ARG_WITH([guile],
        [  --with-guile            build guile api],
	[AC_MSG_RESULT([building --with-guile.])
    _with_guile=yes]
    )

if test x$_with_guile = xyes ; then
AX_OE_GUILE_DEVEL([])
AC_SUBST([GUILE_CFLAGS])
AC_SUBST([GUILE_LDFLAGS])
OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="$OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD guile"
fi

#=============================================================================#
#                           configure clisp support                           #
#=============================================================================#
_with_clisp=no
AC_ARG_WITH([clisp],
        [  --with-clisp            build clisp api],
	[AC_MSG_RESULT([building --with-clisp.])
    _with_clisp=yes]
    )

if test x$_with_clisp = xyes ; then
CL_CLISP([])
OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="$OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD clisp"
fi

#=============================================================================#
#                           configure R support                               #
#=============================================================================#
_with_R=no
AC_ARG_WITH([R],
        [  --with-R              build R api],
	[AC_MSG_RESULT([building --with-R.])
    _with_R=yes]
    )

if test x$_with_R = xyes ; then
AX_OE_R_DEVEL([])
AC_SUBST([R_CFLAGS])
AC_SUBST([R_LDFLAGS])
OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="$OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD R"
fi

#=============================================================================#
#                           configure go support                               #
#=============================================================================#
_with_go=no
AC_ARG_WITH([go],
        [  --with-go              build go api],
	[AC_MSG_RESULT([building --with-go.])
    _with_go=yes]
    )

if test x$_with_go = xyes ; then
AX_OE_GO_DEVEL([])
AC_SUBST([GO_CFLAGS])
AC_SUBST([GO_LDFLAGS])
OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD="$OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD go"
fi

#=============================================================================#

#=============================================================================#
#                                                                             #
#                                                                             #
#                    END of configure scriping APIs via SWIG                  #
#                                                                             #
#                                                                             #
#=============================================================================#

#=============================================================================#
#                       is list of protocols adequate?                        #
#=============================================================================#
AC_MSG_CHECKING([the list of protocols])
AC_SUBST(OE_LIST_OF_PROTOCOLS_TO_BUILD)
if test -n "$OE_LIST_OF_PROTOCOLS_TO_BUILD"; then
  AC_MSG_RESULT([Protocols: $OE_LIST_OF_PROTOCOLS_TO_BUILD])
else
  AC_MSG_ERROR([Protocols to build is empty. see configure --help])
fi

#=============================================================================#
#                                                                             #
#                               SWIG                                          #
#                                                                             #
#=============================================================================#
#                                                                             #
# if you are building any of the lang support api modules, you will be        #
# compiling swig-generated C code.  if you modify OeScript.i you must run the #
# 'make swig' target to regenerate the C source.  The output of swig, the C   #
# source, is checked into git so you do not need to have swig installed to    #
# build the system if you are not modifying the .i files.                     #
#                                                                             #
OE_LIST_OF_SCRIPTING_APIS_TO_BUILD="swig $OE_LIST_OF_SCRIPTING_APIS_TO_BUILD"
AC_SUBST(OE_LIST_OF_SCRIPTING_SWIG_APIS_TO_BUILD)

AC_SUBST(OE_LIST_OF_SCRIPTING_APIS_TO_BUILD)
AC_SUBST(TOP_BUILD_DIR, `pwd`)

#=============================================================================#
#                                                                             #
#                set some substitutions in scripts                            #
#                                                                             #
#=============================================================================#
if test NONE != "$prefix"; then
  AC_SUBST(PREFIX, "${prefix}")
else
  AC_SUBST(PREFIX, "/opt/onextent/server")
fi

#=============================================================================#
#                                                                             #
#                                                                             #
#                           output Makefile.in(s)                             #
#                                                                             #
#                                                                             #
#=============================================================================#

AC_OUTPUT(
        Makefile
        corelib/Makefile
        corelib/test/Makefile
        corelib/test/net/Makefile
        corelib/test/net/libevent/Makefile
        corelib/test/store/Makefile
        corelib/test/store/bdb/Makefile
        corelib/test/store/sqlite3/Makefile
        corelib/test/security/Makefile
        corelib/test/security/pam/Makefile
        corelib/test/protocol/Makefile
        corelib/test/protocol/json/Makefile
        corelib/test/protocol/jcl/Makefile
        corelib/test/jcl/Makefile
        corelib/test/jcl/reentrant/Makefile
        corelib/test/jcl/nonreentrant/Makefile
        modules/Makefile
        modules/scripting/Makefile
        modules/scripting/nodejs/Makefile
        modules/scripting/nodejs/package.json
        modules/scripting/nodejs/wscript
        modules/scripting/swig/Makefile
        modules/scripting/swig/perl/Makefile
        modules/scripting/swig/perl/perllib/Makefile
        modules/scripting/swig/perl/perlmod/Makefile
        modules/scripting/swig/perl/perlmod/lib/oescript.pm
        modules/scripting/swig/perl/perlmod/oescriptimpl.pod
        modules/scripting/swig/perl/perlmod/Build.PL
        modules/scripting/swig/perl/perlmod/OeLinkBin.PL
        modules/scripting/swig/tcl/Makefile
        modules/scripting/swig/tcl/test/Makefile
        modules/scripting/swig/python/Makefile
        modules/scripting/swig/python/pythonmod/Makefile
        modules/scripting/swig/python/pythonmod/setup.py
        modules/scripting/swig/python/pythonlib/Makefile
        modules/scripting/swig/python/pythonlib/test/Makefile
        modules/scripting/swig/ruby/Makefile
        modules/scripting/swig/ruby/test/Makefile
        modules/scripting/swig/php/Makefile
        modules/scripting/swig/php/test/Makefile
        modules/scripting/swig/lua/Makefile
        modules/scripting/swig/lua/luamod/Makefile
        modules/scripting/swig/lua/lualib/Makefile
        modules/scripting/swig/lua/lualib/test/Makefile
        modules/scripting/swig/java/Makefile
        modules/scripting/swig/java/test/Makefile
        modules/scripting/swig/java/test/script_runner
        modules/scripting/swig/guile/Makefile
        modules/scripting/swig/guile/test/Makefile
        modules/scripting/swig/clisp/Makefile
        modules/scripting/swig/clisp/test/Makefile
        modules/scripting/swig/R/Makefile
        modules/scripting/swig/R/rlib/Makefile
        modules/scripting/swig/R/rlib/test/Makefile
        modules/scripting/swig/R/rmod/Makefile
        modules/scripting/swig/R/rmod/DESCRIPTION
        modules/scripting/swig/go/Makefile
        modules/scripting/swig/go/golib/Makefile
        modules/scripting/swig/go/golib/test/Makefile
        modules/scripting/swig/go/gomod/Makefile
        modules/scripting/java/Makefile
        modules/scripting/java/javaclient/Makefile
        modules/scripting/java/javaclient/test/Makefile
        modules/scripting/java/javaclient/test/script_runner
        modules/scripting/java/javaclient/test/antlr/Makefile
        modules/scripting/java/javaclient/test/antlr/script_runner
        modules/scripting/java/javaclient/test/javacc/Makefile
        modules/scripting/java/javaclient/test/javacc/adder/Makefile
        modules/scripting/java/javaclient/test/javacc/adder/script_runner
        modules/scripting/java/javaclient/test/javacc/calc/Makefile
        modules/scripting/java/javaclient/test/javacc/calc/script_runner
        modules/scripting/java/javaclient/test/javacc/jcl/Makefile
        modules/scripting/java/javaclient/test/javacc/jcl/script_runner
        products/Makefile
        products/server/Makefile
        products/server/go/Makefile
        products/server/R/Makefile
        products/server/lua/Makefile
        products/server/guile/Makefile
        products/server/perl/Makefile
        products/server/php/Makefile
        products/server/tcl/Makefile
        products/server/python/Makefile
        products/server/java/Makefile
        products/server/ruby/Makefile,
        echo timestamp > stamp-h
        )

