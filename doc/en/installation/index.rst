.. _user_installation:

############
Installation
############

Centreon recommends using its official packages from the Centreon Open Sources
version available free of charge on our repository (ex CES). Most of Centreon endorsed
software are available as RPM packages.

Alternatively, you can build and install your own version of this
software by following the :ref:`user_installation_using_sources`.

Before installing the software, please make sure that your platform is
up to date, especially when using virtualization tools (VMware drivers
notably).

**************
Using packages
**************

Centreon provides RPM for its products through Centreon Open Sources (COS) version
available free of charge on our repository, Open source products are
freely available from our repository.

These packages are available for CentOS 6 and 7 version.

.. _user_installation_packages_prerequisites:

Prerequisites
=============

In order to use RPM from the COS repository, you have to install the
appropriate repository.

CentOS 6
--------

Run the following commands as privileged user ::

  $ wget http://yum.centreon.com/standard/3.4/el6/stable/noarch/RPMS/centreon-release-3.4-4.el6.noarch.rpm
  $ yum install --nogpgcheck -y centreon-release-3.4-4.el6.noarch.rpm
  $ rm -f centreon-release-3.4-4.el6.noarch.rpm
  $ yum clean all


CentOS 7
--------

Run the following commands as privileged user ::

  $ wget http://yum.centreon.com/standard/3.4/el7/stable/noarch/RPMS/centreon-release-3.4-4.el7.centos.noarch.rpm
  $ yum install --nogpgcheck -y centreon-release-3.4-4.el7.centos.noarch.rpm
  $ rm -f centreon-release-3.4-4.el7.centos.noarch.rpm
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
* GnuTLS **(>= 2.8)**, a secure communications library.
* Lua library **(>= 5.1)**, a powerful, light-weight programming language.
* Git, a popular version control system.

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

Either use the Package Manager or the yum tool to install them. You should check
packages version when necessary.

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
GnuTLS **(>= 2.8)**         gnutls-devel               Development files for gnutls.
Lua library **(>= 5.1)**    lua-devel                  A powerful, light-weight
                                                       programming language.
Git                         git                        A popular version control
                                                       system.
=========================== ========================== ================================

#. Install basic compilation tools ::

   $ yum install gcc gcc-c++ make

#. Install Centreon repository

   You need to install Centreon Open Sources (COS) repository file as
   explained :ref:`user_installation_packages_prerequisites` to use some
   specific package version.

#. Install cmake ::

   $ yum install cmake

#. Install lua-devel ::

   $ yum install lua-devel

#. Install RRDTool ::

   $ yum install rrdtool-devel

#. Install Qt framework ::

   $ yum install qt4-devel qt4-mysql

#. Install GnuTLS ::

   $ yum install gnutls-devel

#. Ensure that the qmake binary is in the PATH environment variable.

   Depending on your Qt installation, qmake could already be available
   or in a path like /usr/lib64/qt4/bin/.

   ::

     $ export PATH="$PATH:/usr/lib64/qt4/bin"
     $ qmake --version # (or qmake-qt4 --version)
     QMake version 2.01a
     Using Qt version 4.8.7 in /usr/lib/x86_64-linux-gnu

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
GnuTLS **(>= 2.8)**         libgnutls28-dev  Development files for gnutls.
Lua library **(>= 5.1)**    liblua5.3-dev /  A powerful, light-weight
                            liblua5.2-dev    programming language.
Git                         git              A popular version control system.
=========================== ================ ================================

#. Install compilation tools ::

   $ apt-get install build-essential

#. Install cmake ::

   $ apt-get install cmake

#. Imstall lua-dev

   For Debian Jessie / Ubuntu 14.04 ::

      $ apt-get install lua5.2-dev

   For Debian Stretch / Ubuntu Xenial ::

      $ apt-get install lua5.3-dev

#. Install RRDTool ::

   $ apt-get install librrd-dev

#. Install Qt framework ::

   $ apt-get install libqt4-dev libqt4-sql-mysql

#. Install GnuTLS ::

   $ apt-get install libgnutls28-dev

#. Install git ::

   $ apt-get install git

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
GnuTLS **(>= 2.8)**         libgnutls-devel   Development files for gnutls.
Lua library **(>= 5.1)**    lua-devel         A powerful, light-weight
                                              programming language.
Git                         git               A popular version control
                                              system.
=========================== ================= ================================

#. Install compilation tools ::

   $ zypper install gcc gcc-c++ make cmake libqt4-devel rrdtool-devel

#. Install RRDTool ::

   $ zypper install rrdtool-devel

#. Install Qt framework ::

   $ zypper install libqt4-devel libqt4-sql-mysql

#. Install GnuTls ::

   $ zypper install libgnutls-devel

#. Install Lua ::

   $ zypper install lua-devel

#. Install Git ::

   $ zypper install git

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
`download website <https://download.centreon.com/>`_
Once downloaded, extract it ::

  $ tar xzf centreon-broker.tar.gz

Configuration
-------------

At the root of the project directory you'll find a build directory
which holds build scripts. Generate the Makefile by running the
following commands ::

  $ cd /path_to_centreon_broker
  $ mkdir build
  $ cd build
  $ cmake ..

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
WITH_PREFIX_VAR         Centreon Broker runtime directory.                   ``${WITH_PREFIX}/var``
WITH_STARTUP_DIR        Define the startup directory.                        Generaly in ``/etc/init.d`` or ``/etc/init``
WITH_STARTUP_SCRIPT     Generate and install startup script.                 auto detection
WITH_TESTING            Enable build of unit tests. Disabled by default.     OFF
WITH_USER               Set the user for Centreon Broker installation.       root
======================= ==================================================== ============================================

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
      -DWITH_PREFIX_VAR=/var/lib/centreon-broker \
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

Once properly configured, the compilation process is really simple, from the
*build* directory, launch the command::

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
``${WITH_PREFIX_BIN}/cbwd``                  Centreon Broker watchdog.
``${WITH_PREFIX_LIB}/cbmod.so``              Centreon Broker NEB module.
``${WITH_PREFIX_MODULES}/05-dumper.so``      Dumper module.
``${WITH_PREFIX_MODULES}/10-neb.so``         NEB module.
``${WITH_PREFIX_MODULES}/15-stats.so``       Statistics module.
``${WITH_PREFIX_MODULES}/20-bam.so``         Centreon Broker BAM module.
``${WITH_PREFIX_MODULES}/20-storage.so``     Storage module.
``${WITH_PREFIX_MODULES}/30-correlation.so`` Correlation module.
``${WITH_PREFIX_MODULES}/50-tcp.so``         TCP module.
``${WITH_PREFIX_MODULES}/60-tls.so``         TLS (encryption) module.
``${WITH_PREFIX_MODULES}/70-rrd.so``         RRD module.
``${WITH_PREFIX_MODULES}/80-sql.so``         SQL module.
============================================ ===========================
