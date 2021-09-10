#!/bin/bash
RACINE_SOURCE="/opt"


# dossier racine du nouveau centreon collect
if [ ! -d $RACINE_SOURCE/centreon-collect ] ; then
    mkdir $RACINE_SOURCE/centreon-collect
    cd $RACINE_SOURCE/centreon-collect/
    git init
else
    rm -rf $RACINE_SOURCE/centreon-collect
    mkdir $RACINE_SOURCE/centreon-collect
    cd $RACINE_SOURCE/centreon-collect/
    git init
fi



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
git rm CHANGELOG.md
git commit -m "move to collect"

cd $RACINE_SOURCE/centreon-clib/
mkdir centreon-clib
git mv * -k centreon-clib/
git rm .gitignore
git rm CHANGELOG.md
git commit -m "move to collect"

cd $RACINE_SOURCE/centreon-connectors/
mkdir centreon-connector
git mv * -k centreon-connector/
git rm .gitignore
# git rm CHANGELOG.md
git commit -m "move to collect"

cd $RACINE_SOURCE/centreon-tests/
mkdir centreon-tests
git mv * -k centreon-tests/
git rm .gitignore
git rm CHANGELOG.md
git commit -m "move to collect"

cd $RACINE_SOURCE/centreon-build/
mkdir centreon-build
git mv * -k centreon-build/
git rm .gitignore
git rm CHANGELOG.md
git commit -m "move to collect"

# recuperation des source et historique dans centreon-collect
cd $RACINE_SOURCE/centreon-collect/
git remote add centreon-broker $RACINE_SOURCE/centreon-broker/
git pull centreon-broker MON-4724-Road-to-collect
git commit

git remote add centreon-engine $RACINE_SOURCE/centreon-engine/
git pull centreon-engine develop
git commit

git remote add centreon-clib $RACINE_SOURCE/centreon-clib/
git pull centreon-clib develop
git commit

git remote add centreon-connector $RACINE_SOURCE/centreon-connectors/
git pull centreon-connector develop
git commit

git remote add centreon-tests $RACINE_SOURCE/centreon-tests/
git pull centreon-tests master
git commit

git remote add centreon-build $RACINE_SOURCE/centreon-build/
git pull centreon-build master
git commit

# modification des CMakeLists
sed -i 's/WITH_PREFIX_CONF/WITH_PREFIX_CONF_BROKER/' centreon-broker/CMakeLists.txt
sed -i 's/WITH_PREFIX_LIB/WITH_PREFIX_LIB_BROKER/' centreon-broker/CMakeLists.txt
sed -i 's/WITH_USER/WITH_USER_BROKER/' centreon-broker/CMakeLists.txt
sed -i 's/WITH_GROUP/WITH_GROUP_BROKER/' centreon-broker/CMakeLists.txt

sed -i 's/WITH_PREFIX_LIB/WITH_PREFIX_LIB_CLIB/' centreon-clib/CMakeLists.txt
sed -i 's/CMAKE_SOURCE_DIR/PROJECT_SOURCE_DIR/' centreon-clib/test/CMakeLists.txt

sed -i 's/CMAKE_SOURCE_DIR/PROJECT_SOURCE_DIR/' centreon-connector/CMakeLists.txt
sed -i 's/add_executable(ut/add_executable(ut_connector/' centreon-connector/CMakeLists.txt
sed -i 's/target_link_libraries(ut/target_link_libraries(ut_connector/' centreon-connector/CMakeLists.txt
sed -i 's/ssh)/ssh)\nlink_directories(${CMAKE_SOURCE_DIR}\/build\/centreon-clib\/)/' centreon-connector/CMakeLists.txt
sed -i 's/LIBSSH2_LIBRARIES})/LIBSSH2_LIBRARIES})\nadd_dependencies(ut_connector centreon_clib_shared)/' centreon-connector/CMakeLists.txt
sed -i 's/${CLIB_LIBRARIES}/centreon_clib/' centreon-connector/CMakeLists.txt
sed -i 's/CMAKE_SOURCE_DIR/PROJECT_SOURCE_DIR/' centreon-connector/perl/CMakeLists.txt
sed -i 's/\/perl\/inc\/)/\/perl\/inc\/)\nlink_directories(${CMAKE_SOURCE_DIR}\/build\/centreon-clib\/)/' centreon-connector/perl/CMakeLists.txt
sed -i 's/pthread)/pthread)\nadd_dependencies(centreon_connector_perl centreon_clib_shared)/' centreon-connector/perl/CMakeLists.txt
sed -i 's/${CLIB_LIBRARIES}/centreon_clib/' centreon-connector/perl/CMakeLists.txt
sed -i 's/CMAKE_SOURCE_DIR/PROJECT_SOURCE_DIR/' centreon-connector/ssh/CMakeLists.txt
sed -i 's/\/ssh\/inc)/\/ssh\/inc)\nlink_directories(${CMAKE_SOURCE_DIR}\/build\/centreon-clib\/)/' centreon-connector/ssh/CMakeLists.txt
sed -i 's/pthread)/pthread)\nadd_dependencies(centreon_connector_ssh centreon_clib_shared)/' centreon-connector/ssh/CMakeLists.txt
sed -i 's/${CLIB_LIBRARIES}/centreon_clib/' centreon-connector/ssh/CMakeLists.txt
sed -i 's/\/perl\/centreon_connector_perl/\/centreon-connector\/perl\/centreon_connector_perl/' centreon-connector/perl/test/connector.cc
sed -i 's/\/..\/perl\/test\/timeout_kill.pl/\/..\/centreon-connector\/perl\/test\/timeout_kill.pl/' centreon-connector/perl/test/connector.cc
sed -i 's/\/..\/perl\/test\/timeout_term.pl/\/..\/centreon-connector\/perl\/test\/timeout_term.pl/' centreon-connector/perl/test/connector.cc

sed -i 's/WITH_PREFIX_CONF/WITH_PREFIX_CONF_ENGINE/' centreon-engine/CMakeLists.txt
sed -i 's/WITH_PREFIX_LIB/WITH_PREFIX_LIB_ENGINE/' centreon-engine/CMakeLists.txt
sed -i 's/WITH_USER/WITH_USER_ENGINE/' centreon-engine/CMakeLists.txt
sed -i 's/WITH_GROUP/WITH_GROUP_ENGINE/' centreon-engine/CMakeLists.txt
sed -i 's/engine_rpc)/engine_rpc)\nadd_dependencies(cce_core centreon_clib_shared)/' centreon-engine/CMakeLists.txt
sed -i 's/${CLIB_LIBRARIES}/centreon_clib/' centreon-engine/CMakeLists.txt
sed -i 's/"1")/"1")\nadd_dependencies(centengine centreon_clib_shared)/' centreon-engine/CMakeLists.txt
sed -i 's/nlohmann_json_LIB_DIRS})/nlohmann_json_LIB_DIRS})\nlink_directories(${CMAKE_SOURCE_DIR}\/build\/centreon-clib\/)/' centreon-engine/CMakeLists.txt
sed -i 's/centenginestats.cc")/centenginestats.cc")\nadd_dependencies("centenginestats" centreon_clib_shared)/' centreon-engine/CMakeLists.txt
sed -i 's/add_executable(rpc_client/add_executable(rpc_client_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/target_link_libraries(rpc_client/target_link_libraries(rpc_client_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/add_executable(ut/add_executable(ut_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/COMMAND ut/COMMAND ut_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/EXECUTABLE ut/EXECUTABLE ut_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/DEPENDENCIES ut/DEPENDENCIES ut_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/target_link_libraries(ut/target_link_libraries(ut_engine/' centreon-engine/tests/CMakeLists.txt
sed -i 's/paths.hh\")/paths.hh\")\nlink_directories(${CMAKE_SOURCE_DIR}\/build\/centreon-clib\/)/' centreon-engine/modules/bench/CMakeLists.txt
sed -i 's/main.cc")/main.cc")\nadd_dependencies("centengine_bench_passive" centreon_clib_shared)/' centreon-engine/modules/bench/CMakeLists.txt
sed -i 's/"${CLIB_LIBRARIES}"/centreon_clib/' centreon-engine/modules/bench/CMakeLists.txt
sed -i 's/inc")/inc")\nlink_directories(${CMAKE_SOURCE_DIR}\/build\/centreon-clib\/)/' centreon-engine/modules/external_commands/CMakeLists.txt
sed -i 's/with libraries./add_dependencies(externalcmd centreon_clib_shared)/' centreon-engine/modules/external_commands/CMakeLists.txt
sed -i 's/"${CLIB_LIBRARIES}"/centreon_clib/' centreon-engine/modules/external_commands/CMakeLists.txt
sed -i 's/PREFIX "")/PREFIX "")\nlink_directories(${CMAKE_SOURCE_DIR}\/build\/centreon-clib\/)/' centreon-engine/src/simumod/CMakeLists.txt
sed -i 's/PARENT_SCOPE)/PARENT_SCOPE)\nadd_dependencies("${SIMUMOD}" centreon_clib_shared)/' centreon-engine/src/simumod/CMakeLists.txt
sed -i 's/"${CLIB_LIBRARIES}"/centreon_clib/' centreon-engine/src/simumod/CMakeLists.txt
sed -i 's/commands")/commands")\nlink_directories(${CMAKE_SOURCE_DIR}\/build\/centreon-clib\/)/' centreon-engine/test/commands/CMakeLists.txt
sed -i 's/connector_test_run.cc")/connector_test_run.cc")\nadd_dependencies("bin_connector_test_run" centreon_clib_shared)/' centreon-engine/test/commands/CMakeLists.txt
sed -i 's/bin_test_run.cc")/bin_test_run.cc")\nadd_dependencies("bin_test_run" centreon_clib_shared)/' centreon-engine/test/commands/CMakeLists.txt
sed -i 's/${CLIB_LIBRARIES}/centreon_clib/' centreon-engine/test/commands/CMakeLists.txt
sed -i 's/tests\/rpc_client/tests\/rpc_client_engine/' centreon-engine/tests/enginerpc/enginerpc.cc

# deplacement des fichier centreon build
# broker
mkdir -p centreon-broker/packaging/rpm
mkdir -p centreon-broker/packaging/script
mkdir -p centreon-broker/packaging/docker
mkdir -p centreon-broker/packaging/jenkins
mv centreon-build/packaging/broker/debian centreon-broker/packaging/rpm/
mv centreon-build/packaging/broker/rpm/21.10/centreon-broker.spectemplate centreon-broker/packaging/rpm/



#engine
mkdir -p centreon-engine/packaging/rpm
mkdir -p centreon-engine/packaging/script
mkdir -p centreon-engine/packaging/docker
mkdir -p centreon-engine/packaging/jenkins
mv centreon-build/packaging/engine/21.10/centreon-engine.spectemplate centreon-engine/packaging/rpm/
mv centreon-build/packaging/engine/debian centreon-engine/packaging/rpm/
mv centreon-build/packaging/engine/centreonengine_integrate_centreon_engine2centreon.sh centreon-engine/packaging/rpm/



# clib
mkdir -p centreon-clib/packaging/rpm
mkdir -p centreon-clib/packaging/script
mkdir -p centreon-clib/packaging/docker
mkdir -p centreon-clib/packaging/jenkins
mv centreon-build/packaging/clib/debian centreon-clib/packaging/rpm/
mv centreon-build/packaging/clib/centreon-clib.spectemplate centreon-clib/packaging/rpm/



# connector
mkdir -p centreon-connector/packaging/rpm
mkdir -p centreon-connector/packaging/script
mkdir -p centreon-connector/packaging/docker
mkdir -p centreon-connector/packaging/jenkins
mv centreon-build/packaging/connector/21.10/centreon-connector.spectemplate centreon-connector/packaging/rpm/centreon-connector.spectemplate



# modification des fichier de packaging

sed -i 's/DWITH_PREFIX_CONF/DWITH_PREFIX_CONF_BROKER/' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/DWITH_PREFIX_LIB/DWITH_PREFIX_LIB_BROKER/' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/DWITH_USER/DWITH_USER_BROKER/' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/DWITH_GROUP/DWITH_GROUP_BROKER/' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/@VERSION@/%{VERSION}/' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/@RELEASE@/%{RELEASE}/' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/%build/%build\npip3 install conan --upgrade/' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/libstdc++11/libstdc++11 --build=missing/' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/%if 0%{?el7}//' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/%define cmake \/usr\/bin\/cmake3//' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/%define cmake \/usr\/bin\/cmake//' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/cmake >= 2.8/cmake3 >= 3.15/' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/"-DNDEBUG -O2 -g -std=c++11" %{cmake}/"-DNDEBUG -O2 -g -std=c++11" cmake3 -j9/' centreon-broker/packaging/rpm/centreon-broker.spectemplate
sed -i 's/%{__make} %{?_smp_mflags}/%{__make} -j9 %{?_smp_mflags}/' centreon-broker/packaging/rpm/centreon-broker.spectemplate

sed -i 's/DWITH_PREFIX_LIB/DWITH_PREFIX_LIB_CLIB/' centreon-clib/packaging/rpm/centreon-clib.spectemplate
sed -i 's/@VERSION@/%{VERSION}/' centreon-clib/packaging/rpm/centreon-clib.spectemplate
sed -i 's/@RELEASE@/%{RELEASE}/' centreon-clib/packaging/rpm/centreon-clib.spectemplate
sed -i 's/%build/%build\npip3 install conan --upgrade\ncpp11=$(gcc --version | awk "\/gcc\/ \&\& ($3+0)>5.0{print 1}")\nif [ $cpp11 -eq 1 ] ; then\n  conan install . -s compiler.libcxx=libstdc++11 --build=missing\nelse\n  conan install . -s compiler.libcxx=libstdc++\nfi\n/' centreon-clib/packaging/rpm/centreon-clib.spectemplate
sed -i 's/%if 0%{centos} > 7//' centreon-clib/packaging/rpm/centreon-clib.spectemplate
sed -i 's/%define cmake \/usr\/bin\/cmake3//' centreon-clib/packaging/rpm/centreon-clib.spectemplate
sed -i 's/%define cmake \/usr\/bin\/cmake//' centreon-clib/packaging/rpm/centreon-clib.spectemplate
sed -i 's/cmake >= 2.8/cmake3 >= 3.15/' centreon-clib/packaging/rpm/centreon-clib.spectemplate
sed -i 's/"-DNDEBUG -g -O2 -Wno-long-long" %{cmake}/"-DNDEBUG -g -O2 -Wno-long-long" cmake3 -j9/' centreon-clib/packaging/rpm/centreon-clib.spectemplate


sed -i 's/@VERSION@/%{VERSION}/' centreon-connector/packaging/rpm/centreon-connector.spectemplate
sed -i 's/@RELEASE@/%{RELEASE}/' centreon-connector/packaging/rpm/centreon-connector.spectemplate
sed -i 's/%build/%build\npip3 install conan --upgrade/' centreon-connector/packaging/rpm/centreon-connector.spectemplate
sed -i 's/libstdc++11/libstdc++11 --build=missing/' centreon-connector/packaging/rpm/centreon-connector.spectemplate
sed -i 's/%if 0%{?el7}//' centreon-connector/packaging/rpm/centreon-connector.spectemplate
sed -i 's/%define cmake \/usr\/bin\/cmake3//' centreon-connector/packaging/rpm/centreon-connector.spectemplate
sed -i 's/%define cmake \/usr\/bin\/cmake//' centreon-connector/packaging/rpm/centreon-connector.spectemplate
sed -i 's/cmake >= 2.8/cmake3 >= 3.15/' centreon-connector/packaging/rpm/centreon-connector.spectemplate
sed -i 's/"-DNDEBUG -g -O2 -std=c++11 -Wno-long-long" %{cmake}/"-DNDEBUG -g -O2 -std=c++11 -Wno-long-long" cmake3 -j9/' centreon-connector/packaging/rpm/centreon-connector.spectemplate

sed -i 's/@VERSION@/%{VERSION}/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/@RELEASE@/%{RELEASE}/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/DWITH_GROUP/DWITH_GROUP_ENGINE/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/DWITH_PREFIX_CONF/DWITH_PREFIX_CONF_ENGINE/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/DWITH_PREFIX_LIB/DWITH_PREFIX_LIB_ENGINE/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/DWITH_USER_ENGINE/DWITH_USER_ENGINE/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/%build/%build\npip3 install conan --upgrade/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/libstdc++11/libstdc++11 --build=missing/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/%if 0%{?el7}//' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's#%define cmake \/usr\/bin\/cmake3//' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/%define cmake \/usr\/bin\/cmake//' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/cmake >= 2.8/cmake3 >= 3.15/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/"-DNDEBUG -g -O2 -Wno-long-long" %{cmake}/"-DNDEBUG -g -O2 -Wno-long-long" cmake3/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
sed -i 's/%{__make} install DESTDIR/%{__make} -j9 install DESTDIR/' centreon-engine/packaging/rpm/centreon-engine.spectemplate
# supression dossier centreon build
rm -rf centreon-build

# copie des fichier collect
cp -r centreon-broker/collect/* .
cp centreon-broker/collect/.gitignore .
chmod 775 rpmbuild.sh
cp rpmbuild.sh centreon-broker/packaging/script/
cp rpmbuild.sh centreon-engine/packaging/script/
cp rpmbuild.sh centreon-clib/packaging/script/
cp rpmbuild.sh centreon-connector/packaging/script/
rm -rf rpmbuild.sh
sed -i 's/broker/engine/' centreon-engine/packaging/script/rpmbuild.sh
sed -i 's/broker/engine/' centreon-engine/packaging/script/rpmbuild.sh
sed -i 's/rpmbuild -ba/cp centreon-engine\/packaging\/rpm\/centreonengine_integrate_centreon_engine2centreon.sh \/root\/rpmbuild\/SOURCES\/\nrpmbuild -ba/' centreon-engine/packaging/script/rpmbuild.sh


sed -i 's/broker/clib/' centreon-clib/packaging/script/rpmbuild.sh
sed -i 's/broker/clib/' centreon-clib/packaging/script/rpmbuild.sh
sed -i 's/broker/connector/' centreon-connector/packaging/script/rpmbuild.sh
sed -i 's/broker/connector/' centreon-connector/packaging/script/rpmbuild.sh

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
rm -rf centreon-connector

cd $RACINE_SOURCE/centreon-tests/
git reset --hard HEAD~1
rm -rf centreon-tests

cd $RACINE_SOURCE/centreon-build/
git reset --hard HEAD~1
rm -rf centreon-build

# suppression des remote 
cd $RACINE_SOURCE/centreon-collect/
git remote rm centreon-broker
git remote rm centreon-clib
git remote rm centreon-connector
git remote rm centreon-engine
git remote rm centreon-tests
git remote rm centreon-build




echo ""
echo ""
echo ""
echo "**********************************************************************************"
echo "ne surtout pas oublier d'enlever les else et endif dans les spec template"
echo "**********************************************************************************"
echo ""
#push dans centreon-collect distant
# git add -A
# git commit -m "road to collect"
# git remote add origin https://github.com/centreon/centreon-collect.git
# git checkout -b develop
# git push origin develop




