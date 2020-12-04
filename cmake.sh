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
    if dpkg -l --no-pager cmake ; then
      echo "Bad version of cmake..."
      exit 1
    else
      echo -e "cmake is not installed, you could enter, as root:\n\tapt install -y cmake\n\n"
      cmake='cmake'
    fi
    count=$(dpkg --no-pager -l gcc cmake librrd-dev libgnutls28-dev ninja-build liblua5.3-dev | grep "^ii" | wc -l)
    if [ $count -lt 6 ] ; then
      echo -e "One or packages among these ones, gcc, cmake, librrd-dev, libgnutls28-dev, ninja-build, liblua5.3-dev, are not installed. You could enter, as root:\n\tapt install -y gcc cmake librrd-dev libgnutls28-dev ninja-build liblua5.3-dev\n\n"
      exit 1
    fi
  else
    echo "Bad version of cmake..."
    exit 1
  fi
  if [[ ! -x /usr/bin/python3 ]] ; then
    echo -e "python3 is not installed, you can enter, as root:\n\tapt install -y python3\n\n"
    exit 1
  else
    echo "python3 already installed"
  fi
  if ! dpkg -l --no-pager python3-pip ; then
    echo -e "python3-pip is not installed, you can enter, as root:\n\tapt install -y python3-pip\n\n"
    exit 1
  else
    echo "pip3 already installed"
  fi
fi

pip3 install conan --upgrade

my_id=$(id -u)
if [ $my_id -eq 0 ] ; then
  conan='conan'
else
  conan="$HOME/.local/bin/conan"
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

set -x

if [ $good -eq 1 ] ; then
  $conan install .. --remote centreon -s compiler.libcxx=libstdc++11
else
  $conan install .. --remote centreon -s compiler.libcxx=libstdc++
fi

CXXFLAGS="-Wall -Wextra" $cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_PREFIX=/usr -DWITH_PREFIX_BIN=/usr/sbin -DWITH_USER=centreon-broker -DWITH_GROUP=centreon-broker -DWITH_CONFIG_PREFIX=/etc/centreon-broker -DWITH_TESTING=On -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker -DWITH_PREFIX_CONF=/etc/centreon-broker -DWITH_PREFIX_LIB=/usr/lib64/nagios -DWITH_MODULE_SIMU=On $* ..

