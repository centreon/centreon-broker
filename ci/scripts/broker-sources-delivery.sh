#!/bin/bash
set -e

. ./common.sh

##### STARTING #####
if [[ -z "$1" ]]; then
    echo "Must provide PROJECT argument like centreon-broker " 1>&2
    exit 1
fi

PROJECT=$1

echo -n "#####The Delivered project is $PROJECT#####"
echo -n "#####GET $PROJECT VERSION#####"

cmakelists=CMakeLists.txt

case $PROJECT in

  centreon-broker)
    major=`grep 'set(CENTREON_BROKER_MAJOR' "$cmakelists" | cut -d ' ' -f 2 | cut -d ')' -f 1`
    minor=`grep 'set(CENTREON_BROKER_MINOR' "$cmakelists" | cut -d ' ' -f 2 | cut -d ')' -f 1`
    patch=`grep 'set(CENTREON_BROKER_PATCH' "$cmakelists" | cut -d ' ' -f 2 | cut -d ')' -f 1`
    ;;

  *)
    break
    ;;
esac

export VERSION="$major.$minor.$patch"
export MAJOR="$major.$minor"

echo -n "#####GET $PROJECT RELEASE#####"
COMMIT=`git log -1 HEAD --pretty=format:%h`
now=`date +%s`
if [ "$BUILD" '=' 'RELEASE' ] ; then
    export RELEASE="$BUILD_NUMBER"
else
    export RELEASE="$now.$COMMIT"
fi

echo -n "#####GET $PROJECT COMMITER#####"
COMMITTER=`git show --format='%cN <%cE>' HEAD | head -n 1`

echo -n "#####ARCHIVING $PROJECT#####"
tar czf "$PROJECT-$VERSION.tar.gz" "$PROJECT"    

echo -n "#####DELIVER $PROJECT SOURCES#####"
put_internal_source "$PROJECT" "$PROJECT-$VERSION-$RELEASE" "$PROJECT-$VERSION.tar.gz"

echo -n "#####EXPORTING $PROJECT GLOBAL VARIABLES#####"
cat > source.properties << EOF
PROJECT=$PROJECT
VERSION=$VERSION
RELEASE=$RELEASE
COMMIT=$COMMIT
COMMITTER=$COMMITTER
MAJOR=$MAJOR
EOF



