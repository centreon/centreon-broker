#!/bin/bash
set -e

#Cmake
rm -rf /src/build
mkdir /src/build
cd /src/build/
DISTRIB=$(lsb_release -rs | cut -f1 -d.)
if [ "$DISTRIB" = "7" ] ; then
    source /opt/rh/devtoolset-9/enable
fi 
conan install .. -s compiler.libcxx=libstdc++11 --build=missing
if [ "$(cat /etc/debian_version)" = "10.10" ] ; then
    CXXFLAGS="-Wall -Wextra" cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_PREFIX=/usr -DWITH_PREFIX_BIN=/usr/sbin -DWITH_USER=centreon-broker -DWITH_GROUP=centreon-broker -DWITH_CONFIG_PREFIX=/etc/centreon-broker -DWITH_TESTING=On -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker -DWITH_PREFIX_CONF=/etc/centreon-broker -DWITH_PREFIX_LIB=/usr/lib64/nagios -DWITH_MODULE_SIMU=On ..
else 
    CXXFLAGS="-Wall -Wextra" cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_PREFIX=/usr -DWITH_PREFIX_BIN=/usr/sbin -DWITH_USER=centreon-broker -DWITH_GROUP=centreon-broker -DWITH_CONFIG_PREFIX=/etc/centreon-broker -DWITH_TESTING=On -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker -DWITH_PREFIX_CONF=/etc/centreon-broker -DWITH_PREFIX_LIB=/usr/lib64/nagios -DWITH_MODULE_SIMU=On ..
fi

#Build
make -j9 
make -j9 install

#Test

cd centreon-broker
test/ut --gtest_output=xml:broker-ut.xml




