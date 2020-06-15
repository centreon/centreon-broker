#####
Usage
#####

****
Help
****

*cbd* prints all flags it accepts with the *-h* (or *--help*) flag.

::

  $> cbd -h
  [1376468351] info:    USAGE: ./cbd [-c] [-d] [-D] [-h] [-v] [<configfile>]
  [1376468351] info:      -c  Check configuration file.
  [1376468351] info:      -d  Enable debug mode.
  [1376468351] info:      -D  Generate a diagnostic file.
  [1376468351] info:      -h  Print this help.
  [1376468351] info:      -v  Print Centreon Broker version.
  [1376468351] info:    Centreon Broker 3.0.0
  [1376468351] info:    Copyright 2009-2015 Centreon
  [1376468351] info:    License ASL 2.0 <http://www.apache.org/licenses/LICENSE-2.0>


*******
Version
*******

*cbd* accepts the *-v* (or *--version*) flag that prints the Centreon
Broker version.

::

  $> cbd -v
  [1376466956] info:    Centreon Broker 3.0.0


*****
Debug
*****

The debug mode can be enabled on *cbd* to print all available log
messages to the standard output. This option is mostly useful when
troubleshooting issues with the configuration file. The flag is *-d* (or
*--debug*).

::

  $> cbd -d centreon-broker.xml
  [1376467933] debug:   module applier: no directory defined
  [1376467933] config:  endpoint applier: loading configuration
  [1376467933] debug:   endpoint applier: 0 inputs and 0 outputs to apply
  [1376467933] debug:   endpoint applier: 0 inputs to create, 0 outputs to create
  [1376467933] info:    Centreon Broker 3.0.0
  [1376467933] info:    Copyright 2009-2015 Centreon
  [1376467933] info:    License ASL 2.0 <http://www.apache.org/licenses/LICENSE-2.0>
  [1376467933] info:    PID: 21105
  [1376467933] info:    Qt compilation version 4.8.4
  [1376467933] info:    Qt runtime version 4.8.4
  [1376467933] info:      Build Key: x86_64 linux g++-4 full-config
  [1376467933] info:      Licensee: Open Source
  [1376467933] info:      Licensed Products: OpenSource
  [1376467933] config:  log applier: applying 2 logging objects
  [1376467933] config:  log applier: creating new logger
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/stats/05-stats.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/stats/05-stats.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/stats/05-stats.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/stats/05-stats.so'
  [1376467933] config:  stats: invalid stats configuration, stats engine is NOT loaded
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/stats/05-stats.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/neb/10-neb.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/neb/10-neb.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/neb/10-neb.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/neb/10-neb.so'
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/neb/10-neb.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/correlation/20-correlation.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/correlation/20-correlation.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/correlation/20-correlation.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/correlation/20-correlation.so'
  [1376467933] config:  correlation: invalid correlation configuration, correlation engine is NOT loaded
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/correlation/20-correlation.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/storage/20-storage.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/storage/20-storage.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/storage/20-storage.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/storage/20-storage.so'
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/storage/20-storage.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/file/50-file.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/file/50-file.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/file/50-file.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/file/50-file.so'
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/file/50-file.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/local/50-local.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/local/50-local.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/local/50-local.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/local/50-local.so'
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/local/50-local.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/tcp/50-tcp.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/tcp/50-tcp.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/tcp/50-tcp.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/tcp/50-tcp.so'
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/tcp/50-tcp.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/compression/60-compression.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/compression/60-compression.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/compression/60-compression.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/compression/60-compression.so'
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/compression/60-compression.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/tls/60-tls.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/tls/60-tls.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/tls/60-tls.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/tls/60-tls.so'
  [1376467933] info:    TLS: loading GNU TLS version 2.12.23
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/tls/60-tls.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/rrd/70-rrd.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/rrd/70-rrd.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/rrd/70-rrd.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/rrd/70-rrd.so'
  [1376467933] info:    RRD: using rrdtool 1.4.7
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/rrd/70-rrd.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/bbdo/80-bbdo.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/bbdo/80-bbdo.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/bbdo/80-bbdo.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/bbdo/80-bbdo.so'
  [1376467933] info:    BBDO: using protocol version 1.0.0
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/bbdo/80-bbdo.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/ndo/80-ndo.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/ndo/80-ndo.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/ndo/80-ndo.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/ndo/80-ndo.so'
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/ndo/80-ndo.so' successfully completed
  [1376467933] config:  module applier: loading module '/home/ganoze/work/centreon-broker/build/sql/80-sql.so'
  [1376467933] debug:   modules: loading library '/home/ganoze/work/centreon-broker/build/sql/80-sql.so'
  [1376467933] info:    modules: library '/home/ganoze/work/centreon-broker/build/sql/80-sql.so' loaded
  [1376467933] debug:   modules: searching initialization routine (symbol broker_module_init) in '/home/ganoze/work/centreon-broker/build/sql/80-sql.so'
  [1376467933] debug:   modules: initialization routine of '/home/ganoze/work/centreon-broker/build/sql/80-sql.so' successfully completed
  [1376467933] debug:   module applier: no directory defined
  [1376467933] config:  applier: 13 modules loaded
  [1376467933] config:  endpoint applier: loading configuration
  [1376467933] debug:   endpoint applier: 0 inputs and 1 outputs to apply
  [1376467933] debug:   endpoint applier: 0 inputs to create, 1 outputs to create
  [1376467933] config:  endpoint applier: creating new endpoint 'SQLOutput'
  [1376467933] debug:   multiplexing: 1 subscribers are registered after insertion
  [1376467933] info:    multiplexing: start with 0 in queue and the recovery temporary file is disable
  [1376467933] debug:   endpoint applier: output thread 0x15fdea0 is registered and ready to run
  [1376467933] debug:   multiplexing: starting
  [1376467933] info:    failover: SQLOutput is starting
  [1376467933] debug:   failover: SQLOutput is launching loop
  [1376467933] debug:   failover: SQLOutput is opening its endpoint
  [1376467933] error:   SQL: could not open SQL database: Unknown database 'centreon_storage' QMYSQL: Unable to connect
  ...


**********
Diagnostic
**********

The diagnostic feature is used when addressing an issue to Centreon'
support center. The *-D* flag (or *--diagnose*) generates a file called
*cbd-diag.tar.gz* containing relevant information about your system and
your Centreon Broker setup. Please provide it for all Broker-related
issues.

To generate a proper diagnostic file you should execute the following
steps :

  - run the Centreon Broker instance with an issue
  - let the issue happen
  - run the diagnostic on the configuration file *while Centreon Broker
    is running*

If you don't know which instance of Centreon Broker is causing the
issue, please run the above procedure on every Centreon Broker instance
(including the cbmod instance). Backup every *cbd-diag.tar.gz* after the
diagnostic to avoid them being overriden and provide them all to the
support center.

::

  $> cbd -D centreon-broker.xml
  [1376469278] config:  log applier: applying 2 logging objects
  [1376469278] info:    diagnostic: Centreon Broker 3.0.0
  [1376469278] info:    diagnostic: using Qt 4.8.4 x86_64 linux g++-4 full-config (compiled with 4.8.4)
  [1376469278] info:    diagnostic: getting disk usage
  [1376469278] info:    diagnostic: getting LSB information
  [1376469278] info:    diagnostic: getting system name
  [1376469278] info:    diagnostic: getting kernel information
  [1376469279] info:    diagnostic: getting network connections information
  [1376469279] info:    diagnostic: getting processes information
  [1376469279] info:    diagnostic: getting packages information
  [1376469279] info:    diagnostic: getting SELinux status
  [1376469279] info:    diagnostic: getting configuration file
  [1376469279] info:    diagnostic: getting modules information
  [1376469279] info:    diagnostic: getting log files
  [1376469279] info:    diagnostic: creating tarball 'cbd-diag.tar.gz'
