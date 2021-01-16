#!/bin/bash

show_help() {
cat << EOF
Usage: ${0##*/} -n=[yes|no] -v

This program build Centreon-broker

    -f|--force    : force rebuild
    -r|--release  : Build on release mode
    -h|--help     : help
EOF
}
BUILD_TYPE="Debug"
for i in "$@"
do
  case $i in
    -f|--force)
      force=1
      shift
      ;;
    -r|--release)
      BUILD_TYPE="Release"
      ;;
    -h|--help)
      show_help
      exit 2
      ;;
    *)
            # unknown option
    ;;
  esac
done

# Am I root?
my_id=$(id -u)

if [ -r /etc/centos-release ] ; then
  maj="centos$(cat /etc/centos-release | awk '{print $4}' | cut -f1 -d'.')"
  v=$(cmake --version)
  if [[ $v =~ "version 3" ]] ; then
    cmake='cmake'
  else
    if rpm -q cmake3 ; then
      cmake='cmake3'
    elif [ $maj = "centos7" ] ; then
      yum -y install epel-release cmake3
      cmake='cmake3'
    else
      dnf -y install cmake
      cmake='cmake'
    fi
  fi
  if [[ ! -x /usr/bin/python3 ]] ; then
    yum -y install python3
  else
    echo "python3 already installed"
  fi
  if ! rpm -q python3-pip ; then
    yum -y install python3-pip
  else
    echo "pip3 already installed"
  fi

  if ! rpm -q gcc-c++ ; then
    yum -y install gcc-c++
  fi

  pkgs=(
    ninja-build
    rrdtool-devel
    gnutls-devel
    lua-devel
  )
  for i in "${pkgs[@]}"; do
    if ! rpm -q $i ; then
      if [ $maj = 'centos7' ] ; then
        yum install -y $i
      else
        dnf -y --enablerepo=PowerTools install $i
      fi
    fi
  done
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
  elif [ $maj = "Raspbian" ] ; then
    if $dpkg -l cmake ; then
      echo "Bad version of cmake..."
      exit 1
    else
      echo -e "cmake is not installed, you could enter, as root:\n\tapt install -y cmake\n\n"
      cmake='cmake'
    fi
  else
    echo "Bad version of cmake..."
    exit 1
  fi

  if [ $maj = "Debian" ] ; then
    pkgs=(
      gcc
      g++
      pkg-config
      libmariadb3
      librrd-dev
      libgnutls28-dev
      ninja-build
      liblua5.3-dev
      python3
      python3-pip
    )
    for i in "${pkgs[@]}"; do
      if ! $dpkg -l $i | grep "^ii" ; then
        if [ $my_id -eq 0 ] ; then
          apt install -y $i
        else
          echo -e "The package \"$i\" is not installed, you can install it, as root, with the command:\n\tapt install -y $i\n\n"
          exit 1
        fi
      fi
    done
  fi
  elif [ $maj = "Raspbian" ] ; then
    pkgs=(
      gcc
      g++
      pkg-config
      libmariadb3
      librrd-dev
      libgnutls28-dev
      ninja-build
      liblua5.3-dev
      python3
      python3-pip
    )
    for i in "${pkgs[@]}"; do
      if ! $dpkg -l $i | grep "^ii" ; then
        if [ $my_id -eq 0 ] ; then
          apt install -y $i
        else
          echo -e "The package \"$i\" is not installed, you can install it, as root, with the command:\n\tapt install -y $i\n\n"
          exit 1
        fi
      fi
    done
  fi
  if [[ ! -x /usr/bin/python3 ]] ; then
    if [ $my_id -eq 0 ] ; then
      apt install -y python3
    else
      echo -e "python3 is not installed, you can enter, as root:\n\tapt install -y python3\n\n"
      exit 1
    fi
  else
    echo "python3 already installed"
  fi
  if ! dpkg -l --no-pager python3-pip ; then
    if [ $my_id -eq 0 ] ; then
      apt install -y python3-pip
    else
      echo -e "python3-pip is not installed, you can enter, as root:\n\tapt install -y python3-pip\n\n"
      exit 1
    fi
  else
    echo "pip3 already installed"
  fi


pip3 install conan --upgrade

if [ $my_id -eq 0 ] ; then
  conan='conan'
else
  conan="$HOME/.local/bin/conan"
fi
if ! $conan remote list | grep ^centreon ; then
  $conan remote add centreon https://api.bintray.com/conan/centreon/centreon
fi

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

if [ $maj = "Raspbian" ] ; then
  CXXFLAGS="-Wall -Wextra" $cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DWITH_PREFIX=/usr -DWITH_PREFIX_BIN=/usr/sbin -DWITH_USER=centreon-broker -DWITH_GROUP=centreon-broker -DWITH_CONFIG_PREFIX=/etc/centreon-broker -DWITH_TESTING=On -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker -DWITH_PREFIX_CONF=/etc/centreon-broker -DWITH_PREFIX_LIB=/usr/lib64/nagios -DWITH_MODULE_SIMU=On -DUSE_CXX11_ABI=1 $* ..
elif [ $maj = "Debian" ] ; then
  CXXFLAGS="-Wall -Wextra" $cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DWITH_PREFIX=/usr -DWITH_PREFIX_BIN=/usr/sbin -DWITH_USER=centreon-broker -DWITH_GROUP=centreon-broker -DWITH_CONFIG_PREFIX=/etc/centreon-broker -DWITH_TESTING=On -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker -DWITH_PREFIX_CONF=/etc/centreon-broker -DWITH_PREFIX_LIB=/usr/lib64/nagios -DWITH_MODULE_SIMU=On -DUSE_CXX11_ABI=1 $* ..
else
  CXXFLAGS="-Wall -Wextra" $cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE -DWITH_PREFIX=/usr -DWITH_PREFIX_BIN=/usr/sbin -DWITH_USER=centreon-broker -DWITH_GROUP=centreon-broker -DWITH_CONFIG_PREFIX=/etc/centreon-broker -DWITH_TESTING=On -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker -DWITH_PREFIX_CONF=/etc/centreon-broker -DWITH_PREFIX_LIB=/usr/lib64/nagios -DWITH_MODULE_SIMU=On -DUSE_CXX11_ABI=0 $* ..
fi
