#!/bin/bash
set -e

source /opt/rh/devtoolset-9/enable

#Cmake
rm -rf ~/.conan/profiles/default
rm -rf /src/build
mkdir /src/build
cd /src/build/
conan install .. -s compiler.libcxx=libstdc++11 --build="*"
CXXFLAGS="-Wall -Wextra" cmake3 -DWITH_CENTREON_CLIB_INCLUDE_DIR=../centreon-clib/inc/ -DWITH_CENTREON_CLIB_LIBRARIES=centreon-clib/ -DCMAKE_BUILD_TYPE=Debug -DWITH_PREFIX=/usr -DWITH_PREFIX_BIN=/usr/sbin -DWITH_USER_BROKER=centreon-broker -DWITH_USER_ENGINE=centreon-engine -DWITH_GROUP_BROKER=centreon-broker -DWITH_GROUP_ENGINE=centreon-engine -DWITH_TESTING=On -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker -DWITH_PREFIX_CONF_BROKER=/etc/centreon-broker -DWITH_PREFIX_LIB_BROKER=/usr/lib64/nagios -DWITH_PREFIX_CONF_ENGINE=/etc/centreon-engine -DWITH_PREFIX_LIB_ENGINE=/usr/lib64/centreon-engine -DWITH_PREFIX_LIB_CLIB=/usr/lib64/ -DWITH_RW_DIR=/var/lib/centreon-engine/rw -DWITH_VAR_DIR=/var/log/centreon-engine -DWITH_MODULE_SIMU=On $* ..

#Build
make
make install

#Test
centreon-broker/test/ut
centreon-engine/tests/ut_engine
centreon-connector/ut_connector
#fixme gtest clib missing


