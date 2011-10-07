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
# Description:    Centreon Broker
### END INIT INFO

### BEGIN INIT INFO Redhat
# chkconfig: - 70 30
# description: Centreon Broker
# processname: cbd
# config:
# pidfile:
### END INIT INFO

#Fonctions
status_centbroker () {
    if test ! -f $RunFile; then
		echo "No run file found in $RunFile"
		return 1
    fi
    if ps -p $PID > /dev/null 2>&1; then
		return 0
    else
		return 1
    fi
    return 1
}

printstatus_centbroker()
{

	if status_centbroker > /dev/null 2>&1; then
		echo "centreon-broker  $DeamonType (pid $PID) is running..."
	else
		echo "centreon-broker  $DeamonType is not running"
	fi
}

killproc_centbroker () 
{
	kill $1 $PID
}

pid_centbroker ()
{

	if test ! -f $RunFile; then
		echo "No run file found in $RunFile"
		exit 1
	fi

	PID=`head -n 1 $RunFile`
}

# Variables a modifier
Bin=/usr/local/bin/cbd
DeamonType="central-rrd"
CfgPath=/etc/centreon
StopTimeout=20

#####################################
# -- Dont touch behind this line -- #
#####################################
CfgFile=${CfgPath}/${DeamonType}.xml
RunDir=/var/run/centreon
RunFile=${RunDir}/${DeamonType}.pid


# Source function library
# Solaris doesn't have an rc.d directory, so do a test first
if [ -f /etc/rc.d/init.d/functions ]; then
    . /etc/rc.d/init.d/functions
elif [ -f /etc/init.d/functions ]; then
    . /etc/init.d/functions
fi

# Check that Bin variable is set
if [ -z "${Bin}" ]; then
  echo "The $Bin variable isn't set"
  exit 1
fi

# Check that CentBroker exists.
if [ ! -f $Bin ]; then
    echo "Executable file $Bin not found.  Exiting."
    exit 1
fi

# Check that CentBroker is runable.
if [ ! -x "${Bin}" ]; then
    echo "The $Bin binary can't be run."
    exit 1
fi

# Check that CfgFile exists.
if [ ! -f $CfgFile ]; then
    echo "Configuration file $CfgFile not found.  Exiting."
    exit 1
fi


     
# See how we were called.
case "$1" in

    start)
	    # Check lock file
	    if test -f $RunFile; then
			echo "Error : $RunFile already Exists."
			ISRUNNING=`ps -edf | grep "${Bin} ${CfgFile}" | grep -v grep | wc -l `
			if [ $ISRUNNING = 0 ]
				then
				echo "But no $DeamonType process running"
				rm -f $RunFile
				echo "Removing $DeamonType pid file"
			else 
				echo "$DeamonType is already running"
				exit 1
			fi
	    fi
	    echo -n "Starting $DeamonType"
	    ${Bin} ${CfgFile} >/dev/null 2>&1 &
		 ps -fe | grep  "${Bin} ${CfgFile}" | grep -v grep | awk '{ print $2 }' > $RunFile 
		echo " done."
	    exit 0
    ;;
    
    stop)
	    if test ! -f $RunFile; then
			echo "No run file found in $RunFile"
			exit 1
		fi    
		echo -n "Stopping $DeamonType"
		pid_centbroker

		killproc_centbroker
		
		# Now wait stopping centreon-broker
 		for i in `seq 1 $StopTimeout`;  do
 		    if status_centbroker > /dev/null; then
				echo -n '.'
				sleep 1
 		    else
				rm -f $RunFile
				break
 		    fi
			
		done
	    if status_centbroker > /dev/null; then
			echo ""
			echo "Warning - running $DeamonType did not exit in time"
		else
			echo " done."
	    fi
    ;;
    
    status)
		pid_centbroker
	    printstatus_centbroker
    ;;
    
    restart)
			$0 stop
			$0 start
	;;
	
	reload|force-reload)
		if test ! -f $RunFile; then
			$0 start
		else
			pid_centbroker
			if status_centbroker > /dev/null; then
				printf "Reloading centbroker $DeamonType configuration..."
				killproc_centbroker -HUP
				echo "done"
			else
				$0 stop
				$0 start
			fi
		fi
    ;;
       
    *)
	    echo "Usage: $DeamonType {start|stop|restart|reload|force-reload|status}"
    	exit 1
    ;;
    
esac
# End of this script

 

