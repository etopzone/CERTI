#! /bin/sh
# CERTI bootstrap
# $Id: autogen.sh,v 1.1.2.1 2006/05/14 17:41:09 breholee Exp $

echo [bootstrapping CERTI `cat VERSION` ...]
set -x
libtoolize --force --copy
aclocal
autoheader
automake --add-missing --copy
autoconf

# $Id: autogen.sh,v 1.1.2.1 2006/05/14 17:41:09 breholee Exp $
