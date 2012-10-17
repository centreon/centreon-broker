.. _user_installation:

############
Installation
############

Merethis recommend using its official packages from the Centreon
Entreprise Server (CES) repository. Most of Merethis' endorsed
software are available as RPM packages.

Alternatively, you can build and install your own version of this
software by following the :ref:`user_installation_using_sources`.

**************
Using packages
**************

Merethis provides RPM for its products through Centreon Entreprise
Server (CES). Open source products are freely available from our
repository.

These packages have been successfully tested with CentOS 5 and RedHat 5.

.. _user_installation_packages_prerequisites:

Prerequisites
=============

In order to use RPM from the CES repository, you have to install the
appropriate repo file. Run the following command as privileged user::

  $ wget http://yum.centreon.com/standard/ces-standard.repo -O /etc/yum.repos.d/ces-standard.repo

The repo file is now installed.

Install
=======

Run the following commands as privileged user::

  $ yum clean all
  $ yum install centreon-broker centreon-broker-core centreon-broker-cbd centreon-broker-cbmod centreon-broker-storage

All dependencies are automatically installed from Merethis repositories.

.. _user_installation_using_sources:

*************
Using sources
*************

To build Centreon Broker, you will need the following external
dependencies:

* a C++ compilation environment.
* CMake **(>=2.8)**, a cross-platform build system.
* the Qt **(>=4.5)** framework with QtCore, QtSql and QtXml modules.
* RRDTool development files (for the RRD module).

This program is compatible only with Unix-like platforms (Linux,
FreeBSD, Solaris, ...).

.. _user_installation_sources_prerequisites:

Prerequisites
=============

CentOS 5.x
----------

In CentOS 5.x you need to add manually cmake. After that you can
install binary packages. Either use the Package Manager or the
yum tool to install them. You should check packages version when
necessary.

Package required to build:

=========================== ================= ================================
Software                    Package Name      Description
=========================== ================= ================================
C++ compilation environment gcc gcc-c++ make  Mandatory tools to compile.
CMake **(>= 2.8)**          cmake             Read the build script and
                                              prepare sources for compilation.
Qt **(>= 4.5)**             qt4-devel         Centreon Broker require Qt
                                              core framework.
                            qt4-mysql         MySQL drivers for Qt. Useful if
                                              you're using DB output (with
                                              Centreon for example).
RRDTool                     rrdtool-devel     Development files for RRD file
                                              (graph) creation and update.
=========================== ================= ================================

#. Get and install cmake form official website::

    $ wget http://www.cmake.org/files/v2.8/cmake-2.8.9-Linux-i386.sh
    $ sh cmake-2.8.9-Linux-i386.sh
    $ y
    $ y
    $ mv cmake-2.8.9-Linux-i386 /usr/local/cmake

#. Add cmake directory into the PATH environment variable::

    $ export PATH="$PATH:/usr/local/cmake/bin"

#. Install basic compilation tools::

    $ yum install gcc gcc-c++ make

#. Install Qt framework

You need to install Centreon Entreprise Server (CES) repo file as
explained :ref:`user_installation_packages_prerequisites`.

    $ yum clean all
    $ yum install qt4-devel

#. Add qt directory into the PATH environment variable::

    $ updatedb
    $ export PATH="$PATH:$(dirname $(locate /bin/qmake | head -n 1))"

CentOS 6.x
----------

FIXME

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
Qt **(>= 4.5)**             libqt4-dev       Centreon Broker require Qt
                                             core framework.
                            libqt4-sql-mysql MySQL drivers for Qt. Useful if
                                             you're using DB output (with
                                             Centreon for example).
RRDTool                     librrd-dev       Development files for RRD file
                                             (graph) creation and update.
=========================== ================ ================================

#. Install compilation tools::

    $ apt-get install build-essential cmake libqt4-dev librrd-dev

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
Qt **(>= 4.5)**             libqt4-devel      Centreon Broker require Qt
                                              core framework.
RRDTool                     rrdtool-devel     Development files for RRD file
                                              (graph) creation and update.
=========================== ================= ================================

#. Install compilation tools::

    $ zypper install gcc gcc-c++ make cmake libqt4-devel rrdtool-devel

Build
=====

Get sources
-----------

Centreon Broker can be checked out from Merethis's git server at
http://svn.modules.centreon.com/centreon-broker. On a Linux box with git
installed this is just a matter of::

  $ svn checkout http://svn.modules.centreon.com/centreon-broker

Or You can get the latest Centreon Broker's sources from its
`download website <http://www.centreon.com/Centreon-Extensions/centreon-broker-download.html>`_
Once downloaded, extract it::

  $ tar xzf centreon-broker.tar.gz

Configuration
-------------

At the root of the project directory you'll find a build directory
which holds build scripts. Generate the Makefile by running the
following command::

  $ cd /path_to_centreon_broker/build
  $ cmake .

Checking of necessary components is performed and if successfully
executed a summary of your configuration is printed.

Variables
~~~~~~~~~

Your Centreon Broker can be tweaked to your particular needs using
CMake's variable system. Variables can be set like this::

  $ cmake -D<variable1>=<value1> [-D<variable2>=<value2>] .

Here's the list of variables available and their description:

=================== ==================================================== ======================================
Variable            Description                                          Default value
=================== ==================================================== ======================================
WITH_DAEMONS        Set a list of Centreon Broker sysv start up script.  OFF
WITH_GROUP          Set the group for Centreon Broker installation.      root
WITH_PREFIX         Base directory for Centreon Broker installation. If  ${WITH_PREFIX}/bin
                    other prefixes are expressed as relative paths, they
                    are relative to this path. /usr/local
WITH_PREFIX_BIN     Path in which binaries will be installed.            ${WITH_PREFIX}/etc
WITH_PREFIX_CONF    Define specific directory for Centreon Engine
                    configuration.
WITH_PREFIX_INC     Define specific directory for Centreon Broker        ${WITH_PREFIX}/include/centreon-broker
                    headers.
WITH_PREFIX_LIB     Where shared objects (like cbmod.so) will be         ${WITH_PREFIX}/lib
                    installed.
WITH_PREFIX_MODULES Where Centreon Broker modules will be installed.     ${WITH_PREFIX_LIB}/centreon-broker
WITH_STARTUP_DIR    Define the startup directory.                        Generaly in /etc/init.d or /etc/init
WITH_STARTUP_SCRIPT Generate and install startup script.                 auto detection
WITH_TESTING        Enable build of unit tests. Disabled by default.     OFF
WITH_USER           Set the user for Centreon Broker installation.       root
=================== ==================================================== ======================================

Example::

  $ cmake \
      -DWITH_DAEMONS='central-broker;central-rrd' \
      -DWITH_GROUP=root \
      -DWITH_PREFIX=/usr \
      -DWITH_PREFIX_BIN=/usr/sbin \
      -DWITH_PREFIX_CONF=/etc/centreon-broker \
      -DWITH_PREFIX_INC=/usr/include/centreon-broker \
      -DWITH_PREFIX_LIB=/usr/lib/nagios \
      -DWITH_PREFIX_MODULES=/usr/share/centreon/lib/centreon-broker \
      -DWITH_STARTUP_DIR=/etc/init.d \
      -DWITH_STARTUP_SCRIPT=auto \
      -DWITH_TESTING=0 \
      -DWITH_USER=root .

At this step, the software will check for existence and usability of the
rerequisites. If one cannot be found, an appropriate error message will
be printed. Otherwise an installation summary will be printed.

.. NOTE::
  If you need to change the options you used to compile your software,
  you might want to remove the *CMakeLists.txt* file that is in the
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
finish installation::

  $ make install

And wait for its completion.

Check-Up
========

After a successful installation, you should check for the existence of
some of the following files.

======================================== ===========================
File                                     Description
======================================== ===========================
${WITH_PREFIX_BIN}/cbd                   Centreon Broker daemon.
${WITH_PREFIX_LIB}/cbmod.so              Centreon Broker NEB module.
${WITH_PREFIX_MODULES}/10-neb.so         NEB module.
${WITH_PREFIX_MODULES}/20-correlation.so Correlation module.
${WITH_PREFIX_MODULES}/20-storage.so     Storage module.
${WITH_PREFIX_MODULES}/50-file.so        File module.
${WITH_PREFIX_MODULES}/50-local.so       Local module.
${WITH_PREFIX_MODULES}/50-tcp.so         TCP module.
${WITH_PREFIX_MODULES}/60-compression.so Compression module.
${WITH_PREFIX_MODULES}/70-rrd.so         RRD module.
${WITH_PREFIX_MODULES}/80-ndo.so         NDO module.
${WITH_PREFIX_MODULES}/80-sql.so         SQL module.
======================================== ===========================
