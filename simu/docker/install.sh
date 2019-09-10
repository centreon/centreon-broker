#!/bin/bash

path=$(dirname $0)

my_path="$PWD"
metrics='/var/lib/centreon/metrics/'
stat='/var/lib/centreon/status/'

if [[ -d "$metrics" ]] ; then
  rm -rf "$metrics"
fi

if [[ -d "$stat" ]] ; then
  rm -rf "$stat"
fi

mkdir -p /usr/share/centreon-broker/lua/neb
mkdir -p /etc/centreon-broker
mkdir -p /var/log/centreon-broker
mkdir -p "$metrics"
mkdir -p "$stat"

rsync -avzt $path/lua /usr/share/centreon-broker/
cp $path/etc/* /etc/centreon-broker/

# Some cleanup
if [[ -f "/tmp/simu.log" ]] ; then
  echo "Simu logs removed"
  rm /tmp/simu.log
fi

if [[ -f "/var/log/centreon-broker/central-broker-master.log" ]] ; then
  echo "Broker logs removed"
  rm /var/log/centreon-broker/central-broker-master.log
fi
