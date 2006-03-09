#! /bin/sh

echo CERTI `cat VERSION` ...
set -x
libtoolize --force --copy
aclocal
autoheader
automake --add-missing --copy
autoconf
