#!/bin/bash

#FIXME autoset variable
VERSION="21.10.01"
RELEASE="01"

# generate rpm broker
cd ../../centreon-broker/packaging/script/
./rpmbuild.sh $VERSION $RELEASE

# generate rpm clib
cd /centreon-clib/packaging/script/
./rpmbuild.sh $VERSION $RELEASE
rpm -i centreon-clib-$VERSION-$RELEASE.el8.x86_64.rpm
rpm -i centreon-clib-debuginfo-$VERSION-$RELEASE.el8.x86_64.rpm
rpm -i centreon-clib-devel-$VERSION-$RELEASE.el8.x86_64.rpm

# generate rpm engine
cd /centreon-engine/packaging/script/
./rpmbuild.sh $VERSION $RELEASE

# generate rpm connector
cd /centreon-connector/packaging/script/
./rpmbuild.sh $VERSION $RELEASE


