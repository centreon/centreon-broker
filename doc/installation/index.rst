.. _user_installation:

############
Installation
############

Centreon recommends using its official packages from the Centreon
Entreprise Server (CES) repository. Most of Centreon' endorsed
software are available as RPM packages.

Alternatively, you can build and install your own version of this
software by following the :ref:`user_installation_using_sources`.

Before installing the software, please make sure that your platform is
up to date, especially when using virtualization tools (VMware drivers
notably).

**************
Using packages
**************

Centreon provides RPM for its products through Centreon Entreprise
Server (CES). Open source products are freely available from our
repository.

These packages are available for CentOS 5 and CentOS 6.

.. _user_installation_packages_prerequisites:

Prerequisites
=============

In order to use RPM from the CES repository, you have to install the
appropriate repository.

CentOS 5
--------

Run the following commands as privileged user ::

  $ wget http://yum.centreon.com/standard/2.2/noarch/RPMS/ces-release-2.2-4.noarch.rpm
  $ yum install --nogpgcheck ces-release-2.2-4.noarch.rpm
  $ rm -f ces-release-2.2-4.noarch.rpm
  $ yum clean all

CentOS 6
--------

Run the following commands as privileged user ::

  $ wget http://yum.centreon.com/standard/3.0/stable/noarch/RPMS/ces-release-3.0-1.noarch.rpm
  $ yum install --nogpgcheck ces-release-3.0-1.noarch.rpm
  $ rm -f ces-release-3.0-1.noarch.rpm
  $ yum clean all

Install
=======

Run the following commands as privileged user ::

  $ yum install centreon-broker centreon-broker-core centreon-broker-cbd centreon-broker-cbmod centreon-broker-storage

All dependencies are automatically installed from Centreon repositories.

.. _user_installation_using_sources:

*************
Using sources
*************

To build Centreon Broker, you will need the following external
dependencies:

* a C++ compilation environment.
* CMake **(>= 2.8)**, a cross-platform build system.
* the Qt **(>= 4.7.4)** framework with QtCore, QtSql and QtXml modules.
* RRDTool development files (for the RRD module).
* GnuTLS **(>= 2.0)**, a secure communications library.

This program is compatible only with Unix-like platforms (Linux,
FreeBSD, Solaris, ...).

.. _user_installation_sources_prerequisites:

Prerequisites
=============

If you decide to build Centreon Broker from sources, we heavily
recommand that you create dedicated system user and group for
security purposes.

On all systems the commands to create a user and a group both named
**centreon-broker** are as follow (need to run these as root) ::

  $ groupadd centreon-broker
  $ useradd -g centreon-broker -m -r -d /var/lib/centreon-broker centreon-broker

Please note that these user and group will be used in the next steps. If
you decide to change user and/or group name here, please do so in
further steps too.

CentOS
------

In CentOS you need to add manually cmake. After that you can
install binary packages. Either use the Package Manager or the
yum tool to install them. You should check packages version when
necessary.

Package required to build:

=========================== ========================== ================================
Software                    Package Name               Description
=========================== ========================== ================================
C++ compilation environment gcc gcc-c++ make           Mandatory tools to compile.
CMake **(>= 2.8)**          cmake                      Read the build script and
                                                       prepare sources for compilation.
Qt **(>= 4.7.4)**           qt4-devel                  Centreon Broker require Qt
                                                       core framework.
                            qt4-mysql                  MySQL drivers for Qt. Useful if
                                                       you're using DB output (with
                                                       Centreon for example).
RRDTool                     rrdtool-devel              Development files for RRD file
                                                       (graph) creation and update.
GnuTLS **(>= 2.0)**         gnutls-devel (el6)         Development files for gnutls.
                            compat-gnutls2-devel (el5)
=========================== ========================== ================================

#. Install basic compilation tools ::

   $ yum install gcc gcc-c++ make

#. Install Centreon repository

   You need to install Centreon Entreprise Server (CES) repos file as
   explained :ref:`user_installation_packages_prerequisites` to use some
   specific package version.

#. Install RRDTool ::

   $ yum install rrdtool-devel

#. Install cmake ::

   $ yum install cmake

#. Install Qt framework ::

   $ yum install qt4-devel qt4-mysql

#. Install GnuTLS (for CentOS 5) ::

   $ yum install compat-gnutls-devel

#. Install GnuTLS (for CentOS 6) ::

   $ yum install gnutls-devel

#. Add qt directory into the PATH environment variable ::

   $ updatedb
   $ export PATH="$PATH:$(dirname $(locate /bin/qmake | head -n 1))"

Debian/Ubuntu
-------------

In recent Debian/Ubuntu versions, necessary software is available as
binary packages from distribution repositories. Either use the Package
Manager or the apt-get tool to install them. You should check packages
version when necessary.

Package required to build:

=========================== ================ ================================
Software                    Package Name     Description
=========================== ================ ================================
C++ compilation environment build-essential  Mandatory tools to compile.
CMake **(>= 2.8)**          cmake            Read the build script and
                                             prepare sources for compilation.
Qt **(>= 4.7.4)**           libqt4-dev       Centreon Broker require Qt
                                             core framework.
                            libqt4-sql-mysql MySQL drivers for Qt. Useful if
                                             you're using DB output (with
                                             Centreon for example).
RRDTool                     librrd-dev       Development files for RRD file
                                             (graph) creation and update.
GnuTLS **(>= 2.0)**         libgnutls28-dev  Development files for gnutls.
=========================== ================ ================================

#. Install compilation tools ::

     $ apt-get install build-essential cmake

#. Install RRDTool ::

     $ apt-get install librrd-dev

#. Install Qt framework ::

     $ apt-get install libqt4-dev libqt4-sql-mysql

#. Install GnuTLS ::

     $ apt-get install libgnutls28-dev

OpenSUSE
--------

In recent OpenSUSE versions, necessary software is available as binary
packages from OpenSUSE repositories. Either use the Package Manager or
the zypper tool to install them. You should check packages version
when necessary.

Package required to build:

=========================== ================= ================================
Software                    Package Name      Description
=========================== ================= ================================
C++ compilation environment gcc gcc-c++ make  Mandatory tools to compile.
CMake **(>= 2.8)**          cmake             Read the build script and
                                              prepare sources for compilation.
Qt **(>= 4.7.4)**           libqt4-devel      Centreon Broker require Qt
                                              core framework.
                            libqt4-sql-mysql  MySQL drivers for Qt. Useful if
                                              you're using DB output (with
                                              Centreon for example).
RRDTool                     rrdtool-devel     Development files for RRD file
                                              (graph) creation and update.
GnuTLS **(>= 2.0)**         libgnutls-devel   Development files for gnutls.
=========================== ================= ================================

#. Install compilation tools ::

     $ zypper install gcc gcc-c++ make cmake libqt4-devel rrdtool-devel

#. Install RRDTool ::

     $ zypper install rrdtool-devel

#. Install Qt framework ::

     $ zypper install libqt4-devel libqt4-sql-mysql

#. Install GnuTls ::

     $ zypper install libgnutls-devel


Raspberry Pi (Raspbian)
-----------------------

The recommended distribution for the Raspberry Pi is Raspbian, a derivative
of Debian. See the Debian section.


Build
=====

Get sources
-----------

Centreon Broker can be checked out from GitHub at
https://github.com/centreon/centreon-broker. On a Linux box with git
installed run this ::

  $ git clone https://github.com/centreon/centreon-broker

Or You can get the latest Centreon Broker's sources from its
`download website <http://www.centreon.com/Centreon-Extensions/centreon-broker-download.html>`_
Once downloaded, extract it ::

  $ tar xzf centreon-broker.tar.gz

Configuration
-------------

At the root of the project directory you'll find a build directory
which holds build scripts. Generate the Makefile by running the
following commands ::

  $ cd /path_to_centreon_broker/build
  $ cmake .

Your Centreon Broker can be tweaked to your particular needs using
CMake's variable system. Variables can be set like this ::

  $ cmake -D<variable1>=<value1> [-D<variable2>=<value2>] .

Here's the list of variables available and their description:

======================= ==================================================== ============================================
Variable                Description                                          Default value
======================= ==================================================== ============================================
WITH_DAEMONS            Set a list of Centreon Broker sysv start up script.  OFF
WITH_GROUP              Set the group for Centreon Broker installation.      root
WITH_MODULE_BBDO        Build BBDO module.                                   ON
WITH_MODULE_COMPRESSION Build compression module.                            ON
WITH_MODULE_CORRELATION Build correlation module.                            ON
WITH_MODULE_FILE        Build file module.                                   ON
WITH_MODULE_LOCAL       Build local module.                                  ON
WITH_MODULE_NDO         Build NDO module.                                    ON
WITH_MODULE_NEB         Build NEB module.                                    ON
WITH_MODULE_RRD         Build RRD module.                                    ON
WITH_MODULE_SQL         Build SQL module.                                    ON
WITH_MODULE_STATS       Build stats module.                                  ON
WITH_MODULE_STORAGE     Build storage module.                                ON
WITH_MODULE_TCP         Build TCP module.                                    ON
WITH_MODULE_TLS         Build TLS module.                                    ON
WITH_PREFIX             Base directory for Centreon Broker installation. If  ``/usr/local``
                        other prefixes are expressed as relative paths, they
                        are relative to this path.
WITH_PREFIX_BIN         Path in which binaries will be installed.            ``${WITH_PREFIX}/bin``
WITH_PREFIX_CONF        Define specific directory for Centreon Engine        ``${WITH_PREFIX}/etc``
                        configuration.
WITH_PREFIX_INC         Define specific directory for Centreon Broker        ``${WITH_PREFIX}/include/centreon-broker``
                        headers.
WITH_PREFIX_LIB         Where shared objects (like cbmod.so) will be         ``${WITH_PREFIX}/lib``
                        installed.
WITH_PREFIX_MODULES     Where Centreon Broker modules will be installed.     ``${WITH_PREFIX_LIB}/centreon-broker``
WITH_STARTUP_DIR        Define the startup directory.                        Generaly in ``/etc/init.d`` or ``/etc/init``
WITH_STARTUP_SCRIPT     Generate and install startup script.                 auto detection
WITH_TESTING            Enable build of unit tests. Disabled by default.     OFF
WITH_USER               Set the user for Centreon Broker installation.       root
======================= ==================================================== ============================================

If you enable testing variable you can set some variable to add more unit test:

====================================== ==================================================== =========================
Variable                               Description                                          Default value
====================================== ==================================================== =========================
WITH_DB_HOST                           Hostname to connect on database.                     localhost
WITH_DB_PASSWORD                       Password to connect on database.                     ""
WITH_DB_PORT                           Port to connect on database.                         3306
WITH_DB_TYPE                           Database type (only "mysql" is available)            OFF
WITH_DB_USER                           User to connect on database.                         ""
WITH_MONITORING_ENGINE                 Enable testing with monitoring engine (set to        OFF
                                       "nagios" or "entengine").
WITH_MONITORING_ENGINE_INTERVAL_LENGTH Set the monitoring engine interval (in seconds).     1
WITH_MONITORING_ENGINE_MODULES         Add monitoring engine modules.                       ""
====================================== ==================================================== =========================

Example ::

  $ cmake \
      -DWITH_DAEMONS='central-broker;central-rrd' \
      -DWITH_GROUP=centreon-broker \
      -DWITH_PREFIX=/usr \
      -DWITH_PREFIX_BIN=/usr/sbin \
      -DWITH_PREFIX_CONF=/etc/centreon-broker \
      -DWITH_PREFIX_INC=/usr/include/centreon-broker \
      -DWITH_PREFIX_LIB=/usr/lib/nagios \
      -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker \
      -DWITH_STARTUP_DIR=/etc/init.d \
      -DWITH_STARTUP_SCRIPT=auto \
      -DWITH_TESTING=0 \
      -DWITH_USER=centreon-broker .

At this step, the software will check for existence and usability of the
rerequisites. If one cannot be found, an appropriate error message will
be printed. Otherwise an installation summary will be printed.

.. note::
  If you need to change the options you used to compile your software,
  you might want to remove the *CMakeCache.txt* file that is in the
  *build* directory. This will remove cache entries that might have been
  computed during the last configuration step.

Compilation
-----------

Once properly configured, the compilation process is really simple::

  $ make

And wait until compilation completes.

Install
=======

Once compiled, the following command must be run as privileged user to
finish installation ::

  $ make install

And wait for its completion.

Check-Up
========

After a successful installation, you should check for the existence of
some of the following files.

============================================ ===========================
File                                         Description
============================================ ===========================
``${WITH_PREFIX_BIN}/cbd``                   Centreon Broker daemon.
``${WITH_PREFIX_LIB}/cbmod.so``              Centreon Broker NEB module.
``${WITH_PREFIX_MODULES}/10-neb.so``         NEB module.
``${WITH_PREFIX_MODULES}/20-correlation.so`` Correlation module.
``${WITH_PREFIX_MODULES}/20-storage.so``     Storage module.
``${WITH_PREFIX_MODULES}/50-file.so``        File module.
``${WITH_PREFIX_MODULES}/50-local.so``       Local module.
``${WITH_PREFIX_MODULES}/50-tcp.so``         TCP module.
``${WITH_PREFIX_MODULES}/60-compression.so`` Compression module.
``${WITH_PREFIX_MODULES}/60-tls.so``         TLS (encryption) module.
``${WITH_PREFIX_MODULES}/70-rrd.so``         RRD module.
``${WITH_PREFIX_MODULES}/80-bbdo.so``        BBDO module.
``${WITH_PREFIX_MODULES}/80-ndo.so``         NDO module.
``${WITH_PREFIX_MODULES}/80-sql.so``         SQL module.
============================================ ===========================
