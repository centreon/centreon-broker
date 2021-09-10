#!/bin/bash
set -e

##### STARTING #####
if [[ -z "$1" ]]; then
    echo "Must provide PROJECT argument like centreon-broker / centreon-clib ..." 1>&2
    exit 1
fi

PROJECT=$1

case $PROJECT in

  centreon-broker)
    PROJECT_NAME="Centreon Broker"
    ;;
  centreon-clib)
    PROJECT_NAME="Centreon Clib"
    ;;  
  centreon-engine)
    PROJECT_NAME="Centreon Engine"
    ;;
  centreon-connector)
    PROJECT_NAME="Centreon Connector"
    ;;
  *)
    break
    ;;
esac

cd $PROJECT
run-clang-tidy-6.0.py -checks='*' -p . -j9

echo "BRANCH_NAME      -> $BRANCH_NAME"
echo "PROJECT_TITLE    -> $PROJECT"
echo "PROJECT_NAME     -> $PROJECT_NAME"
echo "PROJECT_VERSION  -> $VERSION"
sed -i -e "s/{PROJECT_TITLE}/$PROJECT/g" sonar-project.properties
sed -i -e "s/{PROJECT_NAME}/$PROJECT_NAME/g" sonar-project.properties
sed -i -e "s/{PROJECT_VERSION}/$VERSION/g" sonar-project.properties
sonar-scanner
