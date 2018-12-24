#!/bin/bash

path=$(dirname $0)

rm -rf /var/lib/centreon/metrics/*
rm -rf /var/lib/centreon/status/*
mkdir -p /usr/share/centreon-broker/lua/neb
mkdir -p /etc/centreon-broker
mkdir -p /var/log/centreon-broker
mkdir -p /var/lib/centreon-broker

rsync -avzt $path/lua /usr/share/centreon-broker/
cp $path/etc/* /etc/centreon-broker/
