#!/bin/sh
# Run this to generate all the initial makefiles, etc.

#export AUTOCONF_VERSION=2.62
#export AUTOMAKE_VERSION=1.9

touch NEWS README AUTHORS ChangeLog
mkdir -p scripts

LIBTOOLIZE=libtoolize
SYSNAME=`uname`
if [ "x$SYSNAME" = "xDarwin" ] ; then
  LIBTOOLIZE=glibtoolize
  echo "using glibtoolize"
fi

echo "aclocal..." && aclocal -I m4 &&
echo "autoheader..." && autoheader &&
echo "libtoolize..." && $LIBTOOLIZE --automake --copy&&
echo "autoconf..." && autoconf &&
echo "automake..." && automake -a --add-missing --copy

