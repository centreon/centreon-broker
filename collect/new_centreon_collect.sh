#!/bin/bash
RACINE_SOURCE="/opt"


# dossier racine du nouveau centreon collect
mkdir $RACINE_SOURCE/centreon-collect
cd $RACINE_SOURCE/centreon-collect/
git init


# preparation des different repo pour le deplacement
cd $RACINE_SOURCE/centreon-broker/
mkdir centreon-broker
git mv * -k centreon-broker/
git rm .gitignore
git commit -m "move to collect"

cd $RACINE_SOURCE/centreon-engine/
mkdir centreon-engine
git mv * -k centreon-engine/
git rm .gitignore
git commit -m "move to collect"

cd $RACINE_SOURCE/centreon-clib/
mkdir centreon-clib
git mv * -k centreon-clib/
git rm .gitignore
git commit -m "move to collect"

cd $RACINE_SOURCE/centreon-connectors/
mkdir centreon-connectors
git mv * -k centreon-connectors/
git rm .gitignore
git commit -m "move to collect"

cd $RACINE_SOURCE/centreon-tests/
mkdir centreon-tests
git mv * -k centreon-tests/
git rm .gitignore
git commit -m "move to collect"

# recuperation des source et historique dans centreon-collect
cd $RACINE_SOURCE/centreon-collect/
git remote add centreon-broker $RACINE_SOURCE/centreon-broker/
git pull centreon-broker MON-4724-Road-to-collect

git remote add centreon-engine $RACINE_SOURCE/centreon-engine/
git pull centreon-engine master

git remote add centreon-clib $RACINE_SOURCE/centreon-clib/
git pull centreon-clib master

git remote add centreon-connectors $RACINE_SOURCE/centreon-connectors/
git pull centreon-connectors master

git remote add centreon-tests $RACINE_SOURCE/centreon-tests/
git pull centreon-tests master



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

# copie des fichier collect
cp centreon-broker/collect/* .
cp centreon-broker/collect/.gitignore .
chmod 775 cmake.sh
./cmake.sh

# clean des repo
cd $RACINE_SOURCE/centreon-broker/
git reset --hard HEAD~1
rm -rf centreon-broker

cd $RACINE_SOURCE/centreon-engine/
git reset --hard HEAD~1
rm -rf centreon-engine

cd $RACINE_SOURCE/centreon-clib/
git reset --hard HEAD~1
rm -rf centreon-clib

cd $RACINE_SOURCE/centreon-connectors/
git reset --hard HEAD~1
rm -rf centreon-connectors

cd $RACINE_SOURCE/centreon-tests/
git reset --hard HEAD~1
rm -rf centreon-tests

# suppression des remote 
cd $RACINE_SOURCE/centreon-collect/
git remote rm centreon-broker
git remote rm centreon-clib
git remote rm centreon-connectors
git remote rm centreon-engine
git remote rm centreon-tests


