#!/bin/bash
cd ../../
./cmake.sh 

cd build/

make -j5

make install
centreon-broker/test/ut
centreon-engine/tests/ut_engine
centreon-connector/ut_connector
#fixme gtest clib missing

