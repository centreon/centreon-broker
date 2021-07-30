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
dpkg -i centreon-clib-$VERSION-$RELEASE.debian10_amd64.deb
dpkg -i centreon-clib-debuginfo-$VERSION-$RELEASE.debian10_amd64.deb
dpkg -i centreon-clib-devel-$VERSION-$RELEASE.debian10_amd64.deb

# generate rpm engine
cd /centreon-engine/packaging/script/
./rpmbuild.sh $VERSION $RELEASE

# generate rpm connector
cd /centreon-connector/packaging/script/
./rpmbuild.sh $VERSION $RELEASE


