#!/bin/bash
set -e

if [ -z "$VERSION" -o -z "$RELEASE" -o -z "$DISTRIB" ] ; then
  echo "You need to specify VERSION / RELEASE variables"
  exit 1
fi

echo "################################################## BUILDING BROKER ##################################################"
# generate rpm broker
if [ ! -d /root/rpmbuild/SOURCES ] ; then
    mkdir -p /root/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
fi

echo "################################################## BUILDING broker $(pwd) $(ls)" 
mkdir centreon-broker-$VERSION
echo "################################################## BUILDING broker $(ls)"
cp -r centreon-broker/* centreon-broker-$VERSION
tar -czf centreon-broker-$VERSION.tar.gz centreon-broker-$VERSION centreon-broker/cmake.sh
mv centreon-broker-$VERSION.tar.gz /root/rpmbuild/SOURCES/
rm -rf centreon-broker-$VERSION
rpmbuild -ba centreon-broker/packaging/rpm/centreon-broker.spectemplate -D "VERSION $VERSION" -D "RELEASE $RELEASE"

# cleaning and according permissions to slave to delivery rpms
pwd
rm -rf *.rpm
cp -r /root/rpmbuild/RPMS/x86_64/*.rpm .
cp -r /root/rpmbuild/RPMS/x86_64/*.rpm centreon-broker
ls -l
chmod 777 *.rpm