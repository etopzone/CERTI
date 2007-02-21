#! /bin/sh
# CERTI bootstrap
# $Id: autogen.sh,v 1.2 2007/02/21 10:21:15 rousse Exp $

echo [bootstrapping CERTI `cat VERSION` ...]
set -x
libtoolize --force --copy
aclocal
autoheader
automake --add-missing --copy
autoconf

# $Id: autogen.sh,v 1.2 2007/02/21 10:21:15 rousse Exp $
