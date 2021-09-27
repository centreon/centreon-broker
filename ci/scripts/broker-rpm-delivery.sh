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
BROKEREL7RPMS=`echo output/*broker*.el7.*.rpm`
BROKEREL8RPMS=`echo output/*broker*.el8.*.rpm`

# Publish RPMs
if [ "$BUILD" '=' 'QA' ]
then
  put_rpms "standard" "$MAJOR" "el7" "unstable" "x86_64" "broker" "centreon-broker-$VERSION-$RELEASE" $BROKEREL7RPMS

  put_rpms "standard" "$MAJOR" "el8" "unstable" "x86_64" "broker" "centreon-broker-$VERSION-$RELEASE" $BROKEREL8RPMS
elif [ "$BUILD" '=' 'RELEASE' ]
then
  copy_internal_source_to_testing "standard" "broker" "centreon-broker-$VERSION-$RELEASE"
  put_rpms "standard" "$MAJOR" "el7" "testing" "x86_64" "broker" "centreon-broker-$VERSION-$RELEASE" $BROKEREL7RPMS

  put_rpms "standard" "$MAJOR" "el8" "testing" "x86_64" "broker" "centreon-broker-$VERSION-$RELEASE" $BROKEREL8RPMS
fi