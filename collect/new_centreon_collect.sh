#!/bin/bash

# dossier racine du nouveau centreon collect
mkdir /opt/centreon-collect
cd /opt/centreon-collect/


cp -r ../centreon-broker .
cp -r ../centreon-clib .
cp -r ../centreon-tests .
cp -r ../centreon-engine .
cp -r ../centreon-connectors .



# modification des CMakeLists

sed -i 's/WITH_PREFIX_CONF/WITH_PREFIX_CONF_BROKER/' centreon-broker/CMakeLists.txt
sed -i 's/WITH_PREFIX_LIB/WITH_PREFIX_LIB_BROKER/' centreon-broker/CMakeLists.txt
sed -i 's/WITH_USER/WITH_USER_BROKER/' centreon-broker/CMakeLists.txt
sed -i 's/WITH_GROUP/WITH_GROUP_BROKER/' centreon-broker/CMakeLists.txt

sed -i 's/WITH_PREFIX_LIB/WITH_PREFIX_LIB_CLIB/' centreon-clib/CMakeLists.txt

sed -i 's/CMAKE_SOURCE_DIR/PROJECT_SOURCE_DIR/' centreon-connectors/CMakeLists.txt
sed -i 's/add_executable(ut/add_executable(ut_connectors/' centreon-connectors/CMakeLists.txt
sed -i 's/target_link_libraries(ut/target_link_libraries(ut_connectors/' centreon-connectors/CMakeLists.txt
sed -i 's/CMAKE_SOURCE_DIR/PROJECT_SOURCE_DIR/' centreon-connectors/perl/CMakeLists.txt
sed -i 's/CMAKE_SOURCE_DIR/PROJECT_SOURCE_DIR/' centreon-connectors/ssh/CMakeLists.txt

sed -i 's/WITH_PREFIX_CONF/WITH_PREFIX_CONF_ENGINE/' centreon-engine/CMakeLists.txt
sed -i 's/WITH_PREFIX_LIB/WITH_PREFIX_LIB_ENGINE/' centreon-engine/CMakeLists.txt
sed -i 's/WITH_USER/WITH_USER_ENGINE/' centreon-engine/CMakeLists.txt
sed -i 's/WITH_GROUP/WITH_GROUP_ENGINE/' centreon-engine/CMakeLists.txt
sed -i 's/add_executable(rpc_client/add_executable(rpc_client_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/target_link_libraries(rpc_client/target_link_libraries(rpc_client_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/add_executable(ut/add_executable(ut_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/COMMAND ut/COMMAND ut_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/EXECUTABLE ut/EXECUTABLE ut_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/DEPENDENCIES ut/DEPENDENCIES ut_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/target_link_libraries(ut/target_link_libraries(ut_engine/' centreon-engine/tests/CMakeLists.txt

mv centreon-broker/collect/* .

chmod 775 cmake.sh
./cmake.sh

