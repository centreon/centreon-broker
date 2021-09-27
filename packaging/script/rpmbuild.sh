#!/bin/bash

show_help() {
cat << EOF
Usage: ${0##*/} -n=[yes|no] -v

This program build Centreon-broker

    -v  : major version
    -r  : release number
    -h  : help
EOF
exit 2
}

VERSION=$1
RELEASE=$2

if [ -z $VERSION ] || [ -z $RELEASE ] ; then
   echo "Some or all of the parameters are empty";
   echo $VERSION;
   echo $RELEASE;
   show_help
fi

ln -s /usr/bin/cmake3 /usr/bin/cmake

# dossier racine du nouveau centreon collect
if [ ! -d /root/rpmbuild/SOURCES ] ; then
    mkdir -p /root/rpmbuild/{BUILD,BUILDROOT,RPMS,SOURCES,SPECS,SRPMS}
fi

cd "../../../"

mkdir centreon-broker-$VERSION
cp -r centreon-broker/* centreon-broker-$VERSION
tar -czf centreon-broker-$VERSION.tar.gz centreon-broker-$VERSION cmake.sh
mv centreon-broker-$VERSION.tar.gz /root/rpmbuild/SOURCES/
rm -rf centreon-broker-$VERSION

rpmbuild -ba centreon-broker/packaging/rpm/centreon-broker.spectemplate -D "VERSION $VERSION" -D "RELEASE $RELEASE"

