#!/bin/bash
set -e

source /opt/rh/devtoolset-9/enable

#FIXME autoset variable
VERSION="21.10.01"
RELEASE="01"
echo "################################################## BUILDING BROKER ##################################################"
# generate rpm broker
cd /src/centreon-broker/packaging/script/
./rpmbuild.sh $VERSION $RELEASE
echo "################################################## BUILDING CLIB ##################################################"

# generate rpm clib
cd /src/centreon-clib/packaging/script/
./rpmbuild.sh $VERSION $RELEASE
echo "################################################## INSTALL CLIB ##################################################"

rpm -i centreon-clib-$VERSION-$RELEASE.el7.x86_64.rpm
rpm -i centreon-clib-debuginfo-$VERSION-$RELEASE.el7.x86_64.rpm
rpm -i centreon-clib-devel-$VERSION-$RELEASE.el7.x86_64.rpm
echo "################################################## BUILDING ENGINE ##################################################"

# generate rpm engine
cd /src/centreon-engine/packaging/script/
./rpmbuild.sh $VERSION $RELEASE
echo "################################################## BUILDING CONNECTOR ##################################################"

# generate rpm connector
cd /src/centreon-connector/packaging/script/
./rpmbuild.sh $VERSION $RELEASE


