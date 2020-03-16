#!/bin/bash

v=$(cmake --version)
if [[ $v =~ "version 3" ]] ; then
  cmake='cmake'
else
  if rpm -q cmake3 ; then
    cmake='cmake3'
  else
    yum -y install epel-release cmake3
    cmake='cmake3'
  fi
fi

if [ ! -d build ] ; then
  mkdir build
else
  echo "'build' directory already there"
fi

rm -rf build
mkdir build
cd build
conan install --remote centreon ..

CXXFLAGS="-Wall -Wextra" cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_PREFIX=/usr -DWITH_PREFIX_BIN=/usr/sbin -DWITH_USER=centreon-broker -DWITH_GROUP=centreon-broker -DWITH_CONFIG_PREFIX=/etc/centreon-broker -DWITH_TESTING=On -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker -DWITH_PREFIX_CONF=/etc/centreon-broker -DWITH_PREFIX_LIB=/usr/lib64/nagios -DWITH_MODULE_SIMU=On $* ..
