#!/bin/bash
set -e 
set -x

source common.sh

# Check arguments
if [ -z "$VERSION" -o -z "$RELEASE" ] ; then
  echo "You need to specify VERSION / RELEASE variables"
  exit 1
fi

MAJOR=`echo $VERSION | cut -d . -f 1,2`
ENGINEEL7RPMS=`echo output/*engine*.el7.*.rpm`
ENGINEEL8RPMS=`echo output/*engine*.el8.*.rpm`
BROKEREL7RPMS=`echo output/*broker*.el7.*.rpm`
BROKEREL8RPMS=`echo output/*broker*.el8.*.rpm`
CLIBEL7RPMS=`echo output/*clib*.el7.*.rpm`
CLIBEL8RPMS=`echo output/*clib*.el8.*.rpm`
CONNECTOREL7RPMS=`echo output/*connector*.el7.*.rpm`
CONNECTOREL8RPMS=`echo output/*connector*.el8.*.rpm`

# Publish RPMs
if [ "$BUILD" '=' 'QA' ]
then
  put_rpms "standard" "$MAJOR" "el7" "unstable" "x86_64" "engine" "centreon-engine-$VERSION-$RELEASE" $ENGINEEL7RPMS
  put_rpms "standard" "$MAJOR" "el7" "unstable" "x86_64" "broker" "centreon-broker-$VERSION-$RELEASE" $BROKEREL7RPMS
  put_rpms "standard" "$MAJOR" "el7" "unstable" "x86_64" "connector" "centreon-connector-$VERSION-$RELEASE" $CONNECTOREL7RPMS
  put_rpms "standard" "$MAJOR" "el7" "unstable" "x86_64" "clib" "centreon-clib-$VERSION-$RELEASE" $CLIBEL7RPMS

  put_rpms "standard" "$MAJOR" "el8" "unstable" "x86_64" "engine" "centreon-engine-$VERSION-$RELEASE" $ENGINEEL8RPMS
  put_rpms "standard" "$MAJOR" "el8" "unstable" "x86_64" "broker" "centreon-broker-$VERSION-$RELEASE" $BROKEREL8RPMS
  put_rpms "standard" "$MAJOR" "el8" "unstable" "x86_64" "connector" "centreon-connector-$VERSION-$RELEASE" $CONNECTOREL8RPMS
  put_rpms "standard" "$MAJOR" "el8" "unstable" "x86_64" "clib" "centreon-clib-$VERSION-$RELEASE" $CLIBEL8RPMS
elif [ "$BUILD" '=' 'RELEASE' ]
then
  copy_internal_source_to_testing "standard" "engine" "centreon-engine-$VERSION-$RELEASE"
  copy_internal_source_to_testing "standard" "broker" "centreon-broker-$VERSION-$RELEASE"
  copy_internal_source_to_testing "standard" "clib" "centreon-clib-$VERSION-$RELEASE"
  copy_internal_source_to_testing "standard" "connector" "centreon-connector-$VERSION-$RELEASE"
  put_rpms "standard" "$MAJOR" "el7" "testing" "x86_64" "engine" "centreon-engine-$VERSION-$RELEASE" $ENGINEEL7RPMS
  put_rpms "standard" "$MAJOR" "el7" "testing" "x86_64" "broker" "centreon-broker-$VERSION-$RELEASE" $BROKEREL7RPMS
  put_rpms "standard" "$MAJOR" "el7" "testing" "x86_64" "connector" "centreon-connector-$VERSION-$RELEASE" $CONNECTOREL7RPMS
  put_rpms "standard" "$MAJOR" "el7" "testing" "x86_64" "clib" "centreon-clib-$VERSION-$RELEASE" $CLIBEL7RPMS

  put_rpms "standard" "$MAJOR" "el8" "testing" "x86_64" "engine" "centreon-engine-$VERSION-$RELEASE" $ENGINEEL8RPMS
  put_rpms "standard" "$MAJOR" "el8" "testing" "x86_64" "broker" "centreon-broker-$VERSION-$RELEASE" $BROKEREL8RPMS
  put_rpms "standard" "$MAJOR" "el8" "testing" "x86_64" "connector" "centreon-connector-$VERSION-$RELEASE" $CONNECTOREL8RPMS
  put_rpms "standard" "$MAJOR" "el8" "testing" "x86_64" "clib" "centreon-clib-$VERSION-$RELEASE" $CLIBEL8RPMS
fi