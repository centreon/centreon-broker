#!/bin/sh
################################################################################
# Copyright 2005-2011 MERETHIS
# Centreon is developped by : Julien Mathis and Romain Le Merlus under
# GPL Licence 2.0.
# 
# This program is free software; you can redistribute it and/or modify it under 
# the terms of the GNU General Public License as published by the Free Software 
# Foundation ; either version 2 of the License.
# 
# This program is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
# PARTICULAR PURPOSE. See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with 
# this program; if not, see <http://www.gnu.org/licenses>.
# 
# Linking this program statically or dynamically with other modules is making a 
# combined work based on this program. Thus, the terms and conditions of the GNU 
# General Public License cover the whole combination.
# 
# As a special exception, the copyright holders of this program give MERETHIS 
# permission to link this program with independent modules to produce an executable, 
# regardless of the license terms of these independent modules, and to copy and 
# distribute the resulting executable under terms of MERETHIS choice, provided that 
# MERETHIS also meet, for each linked independent module, the terms  and conditions 
# of the license of that module. An independent module is a module which is not 
# derived from this program. If you modify this program, you may extend this 
# exception to your version of the program, but you are not obliged to do so. If you
# do not wish to do so, delete this exception statement from your version.
# 
# For more information : contact@centreon.com
# 
# SVN : $URL:
# SVN : $Id:
#
####################################################################################
#
# Script init
#
### BEGIN INIT INFO Suse
# Provides:       cbd
# Required-Start:
# Required-Stop:
# Default-Start:  3 5
# Default-Stop: 0 1 6
# Description: Centreon Broker
### END INIT INFO

### BEGIN INIT INFO Redhat
# chkconfig: - 71 31
# description: Centreon Broker
# processname: cbd
# config: TO_CHANGE
# pidfile: TO_CHANGE
### END INIT INFO

. /etc/rc.d/init.d/functions

DAEMON_NAME=""

CONFIG_PATH=/etc/centreon-broker

CBD=/usr/sbin/cbd
PID_PATH=/var/run/
STOP_TIMEOUT=10

if [ -z "${DAEMON_NAME}" ]; then
  echo "The cbd name isn't set"
  exit 1
fi

if [ ! -x "${CBD}" ]; then
  echo "The cbd binary can't be run."
  exit 1
fi

if [ ! -e "${CONFIG_PATH}/${DAEMON_NAME}.xml" ]; then
  echo "The configuration file isn't found"
  exit 1
fi

prog=cbd-${DAEMON_NAME}
pidfile=${PID_PATH}/${DAEMON_NAME}.pid

start() {
  if [ -f ${pidfile} ]; then
      is_running=$(ps -edf | grep "$CBD ${CONFIG_PATH}/${DAEMON_NAME}.xml" | grep -v grep | wc -l )
      if [ $is_running = 0 ]; then
	  rm -f $pidfile
      else
          echo -n "$prog ${DAEMON_NAME} already running"
          failure "cbd startup"
          echo
	  return 1
      fi
  fi

  echo -n $"Starting $prog ${DAEMON_NAME}: "
  ${CBD} ${CONFIG_PATH}/${DAEMON_NAME}.xml >/dev/null 2>&1 &
  RETVAL=$?
  PID=$!
  if [ ${RETVAL} = 0 ]; then
    echo ${PID} > ${pidfile}
    success $"cbd startup"
  else
    failure $"cbd startup"
  fi
  echo
  return ${RETVAL}
}

stop() {
  echo -n $"Stopping $prog ${DAEMON_NAME}: "
  killproc -p ${pidfile} -d ${STOP_TIMEOUT} $CBD
  RETVAL=$?
  echo
  [ ${RETVAL} = 0 ] && rm -f ${pidfile}
}

case "$1" in
  start)
    start
    ;;
  stop)
    stop
    ;;
  restart)
    stop
    start
    ;;
  status)
    status -p ${pidfile} ${cbd}
    RETVAL=$?
    ;;
  *)
    echo $"Usage: $prog {start|stop|restart|status}"
    exit 1
esac

exit ${RETVAL}
