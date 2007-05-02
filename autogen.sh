#! /bin/sh
# CERTI bootstrap
# $Id: autogen.sh,v 1.3 2007/05/02 11:55:17 erk Exp $

echo [bootstrapping CERTI...]
set -x
libtoolize --force --copy
aclocal
autoheader
automake --add-missing --copy
autoconf

# $Id: autogen.sh,v 1.3 2007/05/02 11:55:17 erk Exp $
