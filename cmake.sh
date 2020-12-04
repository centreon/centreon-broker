#!/bin/bash

if [ "$1" = "-f" ] ; then
  force=1
  shift
fi

if [ -r /etc/centos-release ] ; then
  maj="centos$(cat /etc/centos-release | awk '{print $4}' | cut -f1 -d'.')"
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
  if [[ ! -x /usr/bin/python3 ]] ; then
    yum -y install python3
  else
    echo "python3 already installed"
  fi
  if ! rpm -qa | grep "python3-pip" ; then
    yum -y install python3-pip
  else
    echo "pip3 already installed"
  fi
elif [ -r /etc/issue ] ; then
  maj=$(cat /etc/issue | awk '{print $1}')
  v=$(cmake --version)
  if [[ $v =~ "version 3" ]] ; then
    cmake='cmake'
  elif [ $maj = "Debian" ] ; then
    if dpkg -l cmake ; then
      echo "Bad version of cmake..."
      exit 1
    else
      apt install -y cmake
      cmake='cmake'
    fi
  else
    echo "Bad version of cmake..."
    exit 1
  fi
  if [[ ! -x /usr/bin/python3 ]] ; then
    apt install -y python3
  else
    echo "python3 already installed"
  fi
  if ! dpkg -l python3-pip ; then
    apt install -y python3-pip
  else
    echo "pip3 already installed"
  fi
fi

pip3 install conan --upgrade

my_id=$(id -u)
if [ $my_id -eq 0 ] ; then
  conan='conan'
else
  conan='~/.local/bin/conan'
fi
$conan remote add centreon https://api.bintray.com/conan/centreon/centreon

good=$(gcc --version | awk '/gcc/ && ($3+0)>5.0{print 1}')

if [ ! -d build ] ; then
  mkdir build
else
  echo "'build' directory already there"
fi

if [ "$force" = "1" ] ; then
  rm -rf build
  mkdir build
fi
cd build

if [ $good -eq 1 ] ; then
  $conan install .. --remote centreon -s compiler.libcxx=libstdc++11
else
  $conan install .. --remote centreon -s compiler.libcxx=libstdc++
fi

CXXFLAGS="-Wall -Wextra" $cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_PREFIX=/usr -DWITH_PREFIX_BIN=/usr/sbin -DWITH_USER=centreon-broker -DWITH_GROUP=centreon-broker -DWITH_CONFIG_PREFIX=/etc/centreon-broker -DWITH_TESTING=On -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker -DWITH_PREFIX_CONF=/etc/centreon-broker -DWITH_PREFIX_LIB=/usr/lib64/nagios -DWITH_MODULE_SIMU=On $* ..

