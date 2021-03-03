# Changelog

## 20.04.14

`Release date to be defined`

### Enhancements

*TLS configuration*

Sometimes, we want to provide a custom common name instead of the hostname.
This is now possible thanks to the tls\_hostname. Supports for TLS1.0 and TLS1.1
have been removed.

## 20.04.13

`Release date to be defined`

### Bugfixes

*Log file configuration*

log file configuration is applied even if the configuration contains errors.
Now '.' is allowed in names.

*Broker termination*

When broker is badly configured and the user wants to stop it, it may hang and
never stop. This new version fixes this issue.

*Storage rebuilder*

The rebuilder loop has been rewritten. When it is stopped, it is interrupted
correctly.

*Logs*

The new logs (log\_v2) are correctly flushed when cbd is stopped.

*Thread pool*

The thread pool is better stopped now.

*Cache/Retention files*

They are written well written as expected. Before, we could see some of them
not written at all.

*Sigterm handler*

It is better managed, so event if several SIGTERM are received by cbd, it stops
correctly.

*Diagnostic/Check modes*

The diagnostic and the check modes are back and functional.

## 20.04.12

`January 20, 2021`

### Bugfixes

*Conflict manager and comments*

It is possible to lock the database during comments insertion. This new
version fixes that.

*BAM reporting dimensions computation*

If there are retention files, dimensions computation could fail because of
conflicts between new block computation and old ones (the ones in the
retention). There was also an issue of concurrent access to tables during
dimensions computation.

*BAM availabilities rebuild*

When availabilities are rebuilt, durations can be doubled. This new version
fixes this issue.

### Enhancements

*Logs*

Logs are sent to the database in bulk as we already do for customvariables.

*Lua*

There is a new API available for the Lua connector. To use it, scripts
must declare a global variable `broker_api_version=2`. From the user's
point of view, Stream Connectors should work almost the same. In isolate
cases, we could see scripts that do not work with this new API, then you
can always work with Broker API version 1, by setting the variable to 1
or by removing this variable declaration in the script. Why should we
use the v2 version? Because it is faster, really faster.

*TCP connections*

If the connection between two peers is flapping, it may be difficult for one
to reconnect to the other and this could lead to many CLOSE_WAIT on the
acceptor side. This new version fixes this issue.

## 20.04.11

`December 16, 2020`

> Known behaviours:
>
>   - If TLS encryption is configured to use private key/certificate couple
>     for IPv4/6 input/output endpoints, **both ends must be updated**
>     to ensure communication.
>
>   - If you use Centreon MAP with TLS encryption, make sure to **update MAP
>     server** to version >= 20.04.5.

### Bugfixes

*TLS*

Credentials were not loaded by the TLS connector anymore. This is fixed with this
new version.

*Custom variables*

They were updated several times in the database. It is fixed now.

*Build*

GnuTLS requirement now matches compilation version.

*BAM*

Reporting events were not stored into database because of truncated
Business Activities names causing *duplicate entry* errors.

## 20.04.10

`November 25, 2020`

### Bugfixes

*Build*

Build on Centos8 fixed.

*Retention files*

The splitter class is now thread safe and does not need external locks
anymore. It is also far less strict and allows some reading and some
writing at the same time.

*TCP*

Writing on a tcp stream could slow down in case of many retention files. The
issue was essentially in the flush internal function.

### Enhancements

*TCP connections*

TCP streams are really faster, especially when Broker has retention
files and there are a lot of traffic.

*SQL and storage streams*

Those streams have several improvements:

- Events exchanges are much faster, especially when Broker has
  retention files.
- Several queries have been changed to insert data in bulk, it is
  the case for custom variables and metrics.
- There are cases where those streams could crash that have been
  also fixed.

*Statistics*

The thread pool has now its own statistics. For now, we have two
informations that are the number of threads it contains and its latency
in milliseconds that is the duration we have to wait to see a task
executed. We post a task to the thread pool at time T1, it is executed
by the thread pool at time T2, the latency is T2 - T1.

*Command line argument*

It is now possible to set the cbd pool size directly on the command line
with the –pool\_size X argument or -s X.

## 20.04.9

`October 26, 2020`

> Known behaviours:
>
>   - If Broker on a Poller or Remote Server is not upgraded to 20.04.9
>     the communication between said Poller or Remote Server and an
>     upgraded Central may not work.
>
>     As always, we **strongly recommend** to upgrade all components to match
>     the Central server's version.
>
>     However, during an upgrade process, communication can be maintained
>     by making sure Broker's configurations match the following conditions:
>
>     - *TLS encryption* and *compression* are either set to
>       *Auto* or *No* on Central input,
>     - *TLS encryption* and *compression* are either set to
>       *Auto* or *No* on Poller or Remote Server output.
>
>     If the reversed connection mode (*one peer retention*) is used,
>     the Broker upgrade is mandatory.

### Bugfixes

*One peer retention*

A known regression in the 20.04 Broker release was that the one peer
retention did not work correctly. It is fixed with this version.

*Columns contents too large*

In case of strings too large to be inserted in database, strings are
now truncated as if the database was configured in non strict mode.

*Negotiations*

Compression and TLS could fail between Engine and Broker, because of
issues in the negotiation between them. This is now fixed. If you
mix previous 20.04.x cbmod/cbd with this new one, you may continue to
have issues on this subject. We recommend you to do the upgrade of cbmod/cbd
on all of your pollers.

*Database deadlock*

When the database connectors are configured with several connections,
a host downtime could make a deadlock on the database. This is fixed now.

*Map server connection*

When the Map server is restarted, there is no more duplicated connections
from centreon-broker.

*BAM reporting*

BAM availability reporting could miss BAs during its availabilities
computations. This is fixed.

*TCP acceptors*

Sometimes TCP acceptor could badly close sockets. This could lead to
difficulties to reopen connections.

*INITIAL HOST STATE*

If you use BAM, there was an issue on the reporting that could fail
because of a missing initial host state. This is fixed now.

### Enhancements

*TCP connector*

The TCP connector should also be largely improved. It is multithreaded
now and this should improve its performances. A new field in the Broker
configuration file allows to set how many threads run in the pool.

*TCP connections*

TCP connections are managed by a thread pool. When not configured, this
thread pool contains at least 2 threads and can increase up to half the
number of server CPUs. Otherwise, it is possible to configure it in the
TCP endpoint with the 'pool_size' label.

## 20.04.8

`August 18 2020`

### Bugfixes

*UTF-8 encoding*

The UTF-8 chek/encoding is moved from engine to cbmod. This is easier to
update for Centreon users. Also, the check algorithm is fixed. Some
strings could be considered as UTF-8 strings whereas they were not.

## 20.04.7

`August 4 2020`

### Enhancements

*Contention*

Conflict manager configuration is easier and more flexible.

## 20.04.6

`July 6, 2020`

### Bugfixes

*Segfault possible during a Mariadb server restart*

Centreon Broker could crash when the database server was restarted.
This version fixes this bug.

*BAM module could never recovery after a Mariadb server restart*

BAM module is better managed on database server reload/restart.

## 20.04.5

`June 11, 2020`

> Known issues:
>
> - When updating to 20.04.5, some badly encoded characters might block SQL
>   Broker events processing on the Central server. We strongly recommand to
>   update all the Engines to 20.04.3 version while updating to this Broker
>   version.
>
>   If you experience this behaviour, we recommand to downgrade Broker on the
>   Central server using this command:
>
>   ```shell
>   yum downgrade centreon-broker-20.04.4 centreon-broker-cbd-20.04.4 centreon-broker-storage-20.04.4 centreon-broker-cbmod-20.04.4 centreon-broker-core-20.04.4
>   ```
>
>   When the events queue is fully processed, you can update both Broker and
>   Engines.

### Bugfixes

*Not ASCII characters badly encoded in database*

If a check output contains not ASCII characters, they are badly transformed
and the string looses its sense. This is fixed.

## 20.04.4

`May 27, 2020`

### Bugfixes

*NEB service status check were badly handled*

Service status check were badly handled. For example the field
command_line was not updated in service. This bug was introduced
in 20.04.3.

## 20.04.3

`May 26, 2020`

### Bugfixes

*Events were badly acknowledged after being sent to the database*

SQL/storage did not acknowledge all the events. This produced retention files.

*Long events could be corrupted*

There was a bug in the long events management.

*Filter on events entering in storage*

A bug on this filter is now fixed.

*Retention files*

A regression was introduced. All the retention files could not be read.

*MariaDB strict mode*

The strict mode implies that strings too long for a column break queries. To
avoid this, we truncate too long strings and set a warning log for users.

## 20.04.2

`May 13, 2020`

### Bugfixes

*BBDO is sending corrupted data*

Data could be badly sent, leading to CRC errors. Now it is fixed.

### Enhancements

*Stream connector*

The Stream connector cache has three new functions that are get_notes(),
get_notes_url() and get_action_url(). They can be used on hosts or on
services.

To use them on hosts, you just have to give the host id as parameter. To use
them on services, you just have to give the host id and the service id as
parameters. All this is detailed in the Broker documentation.

It is also possible to get the severity of a host or a service. We provide now
the function broker_cache:get_severity(host_id, service_id). If you just give
the host_id, we suppose you want a host severity.

## 20.04.1

`May 12, 2020`

### Bugfixes

*Strict mode of the database*

Too long strings to insert in database are cut so that cbd continues to work.
This will be improved in a future Broker version. A warning is logged so that
the user can change his configuration to avoid that.

*Perfdata parsing*

Special characters like '\\r' were not parsed correctly.

*Conflict manager*

In case of bad configurations concerning the database, cbd can crash. This is
fixed with with new version.

### Enhancements

*Perfdata parser*

The parser is less strict. It tries to keep good metrics among bad ones.

*New Lua function in the Stream Connector*

There is a new function broker.stat(filename) to get informations about the
filename.

## 20.04.0

`April 22, 2020`

### Enhancements

*Removal of Qt*

Broker does not need Qt anymore.

*Steam connector*

New function to decode a JSON string into a Lua table.

The stream connector is now asynchronous. If it has to execute a script that
is too slow, it won't slow down Broker. Broker will just return messages
complaining about the slowness of the script.

Another change, now when a stream connector crashes, Broker does not terminate
but just returns an error message containing the Lua interpreter error.

*Configuration files*

Switch from XML to JSON. We used json11 toolkit, and remove
all ref for yajl from sources.

*Network*

Switch from QtNetwork to Asio. We start an effort to avoir copy in network
buffers.

*Optimization*

Migration of the code to C++11.

*Better tests coverage*

We now have 370+ tests (+280%). It allow us to have a better code coverage
of the code base.

### Bugfixes

*Influxdb connector and retention*

If a retention is configured on the Influxdb server and Centreon Broker sends
too old data compared to this retention, the connector ends with an error and
Broker pushes data in retention instead of throwing them away.
This patch fixes that.
