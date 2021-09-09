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

mkdir centreon-broker-$VERSION
cp -r centreon-broker/* centreon-broker-$VERSION
tar -czf centreon-broker-$VERSION.tar.gz centreon-broker-$VERSION cmake.sh
mv centreon-broker-$VERSION.tar.gz /root/rpmbuild/SOURCES/
rm -rf centreon-broker-$VERSION
rpmbuild -ba centreon-broker/packaging/rpm/centreon-broker.spectemplate -D "VERSION $VERSION" -D "RELEASE $RELEASE"


# echo "################################################## BUILDING CLIB ##################################################"

# generate rpm clib
mkdir centreon-clib-$VERSION
cp -r centreon-clib/* centreon-clib-$VERSION
tar -czf centreon-clib-$VERSION.tar.gz centreon-clib-$VERSION cmake.sh
mv centreon-clib-$VERSION.tar.gz /root/rpmbuild/SOURCES/
rm -rf centreon-clib-$VERSION
rpmbuild -ba centreon-clib/packaging/rpm/centreon-clib.spectemplate -D "VERSION $VERSION" -D "RELEASE $RELEASE"

# echo "################################################## INSTALL CLIB ##################################################"

if [ "$DISTRIB" = "el7" ] ; then
  rpm -i /root/rpmbuild/RPMS/x86_64/centreon-clib-$VERSION-$RELEASE.el7.x86_64.rpm
  rpm -i /root/rpmbuild/RPMS/x86_64/centreon-clib-debuginfo-$VERSION-$RELEASE.el7.x86_64.rpm
  rpm -i /root/rpmbuild/RPMS/x86_64/centreon-clib-devel-$VERSION-$RELEASE.el7.x86_64.rpm
else
  rpm -i /root/rpmbuild/RPMS/x86_64/centreon-clib-$VERSION-$RELEASE.el8.x86_64.rpm
  rpm -i /root/rpmbuild/RPMS/x86_64/centreon-clib-debuginfo-$VERSION-$RELEASE.el8.x86_64.rpm
  rpm -i /root/rpmbuild/RPMS/x86_64/centreon-clib-devel-$VERSION-$RELEASE.el8.x86_64.rpm
fi

# echo "################################################## BUILDING ENGINE ##################################################"

# generate rpm engine
mkdir centreon-engine-$VERSION
cp -r centreon-engine/* centreon-engine-$VERSION
tar -czf centreon-engine-$VERSION.tar.gz centreon-engine-$VERSION cmake.sh
mv centreon-engine-$VERSION.tar.gz /root/rpmbuild/SOURCES/
rm -rf centreon-engine-$VERSION

cp centreon-engine/packaging/rpm/centreonengine_integrate_centreon_engine2centreon.sh /root/rpmbuild/SOURCES/
rpmbuild -ba centreon-engine/packaging/rpm/centreon-engine.spectemplate -D "VERSION $VERSION" -D "RELEASE $RELEASE"


# echo "################################################## BUILDING CONNECTOR ##################################################"

# generate rpm connector
mkdir centreon-connector-$VERSION
cp -r centreon-connector/* centreon-connector-$VERSION
tar -czf centreon-connector-$VERSION.tar.gz centreon-connector-$VERSION cmake.sh
mv centreon-connector-$VERSION.tar.gz /root/rpmbuild/SOURCES/
rm -rf centreon-connector-$VERSION

rpmbuild -ba centreon-connector/packaging/rpm/centreon-connector.spectemplate -D "VERSION $VERSION" -D "RELEASE $RELEASE"

# cleaning and according permissions to slave to delivery rpms
rm -rf *.rpm
cp -r /root/rpmbuild/RPMS/x86_64/*.rpm .
chmod 777 *.rpm