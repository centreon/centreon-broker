# Changelog

## 20.10.10

### Fixes

*storage*

Waiting longer for conflict manager to be connected

*bam*

A ba configured with best status was initialized in state OK. And when KPI were
added, there status was always worst than OK (best case was OK). So even if all
the kpi were in critical, the state appeared as OK.

## 20.10.9

Update the headers.
## 20.10.8

### Fixes

*lua*

The Lua cache should no more disappear.

*tcp*

A keepalive is added on the acceptor side.

The flush() method from tcp\_connection could get stuck in cases of retention.

*rrd*

New loop to check deleted index every 5 minutes and delete metric file associated.

*sql*

There was still some code on index data that considered index\_id as uint32.
This was particularly the case in the metrics cache. It is fixed now.

Hostgroups, comments and others transverse stuffs have each one now their own
connection to the database, and we do not choose anymore the connection with
less activity. This fixes possible deadlocks in the database.

Errors encountered by the conflict\_manager were fatal, each time it stopped
after one error. This is fixed now.

*lua*

Objects could stay on the lua exchange stack between the lua machine and C++.

## 20.10.7

### Fixes

*sql*

When a connection to the db is lost, we try to reestablish it. This change fixes
an error "Mysql server has gone away" we often have in the BAM availabilities
computations.

*bbdo*

When the connection of an acceptor is reversed, if cbd is stopped when there is
no peer, cbd does not stop. This patch fixes this issue.

## 20.10.6

### Bug fix

*cbmod*

A link issue in cbmod caused a crash in centengine. This new version fixes it.

*SQL*

Broker stores its connections to the database in an array. Once they are
established, it does not test if they are still valid after a laps of time.
But we know that MariaDB closes inactive connections. So here, we have added
a check to verify if a connection is still ok.
A possible deadlock due to the mysql connections has been removed when cbd is
stopped.

*gRPC*

A memory leak has been fixed.
The reflection was not used and in the new version on the conan-center it does
not compile anymore as is. We remove it for now.

*TLS*

GnuTLS query was not understood on RedHat 8 and Centos8.

*BBDO serialized events*

Converts theses events into trace.

*TCP*

Connections can fail when many pollers establish connection to cbd. This should
be fixed with this new version.

The calls to get remote endpoints from a socket were not protected and could
fail ; this made cbd/centengine to stop.

### Build

repair the compilation for Raspberry PI

## 20.10.5

`Release date to be defined`

### Enhancement

*Build*

cbd could not build anymore since the bintray closure. cbd dependencies have
been moved to the conan-center. The build is back.

*Bam*

*ba worst/best states computation*

There was a bug in the computation of these states.

*ba impact and unknown state*

Impact on unknown state is handled now for kpi ba.

*meta-services*

Meta-services are removed from broker. This code was not used and could produce
bugs. For example, services/hosts with host\_id=0.

*Storage*

We optimize queries by removing useless join. A join with hosts table is not
necessary in certain queries.

### Bug fix

*bbdo*

fix Index_id of metrics tables are now in unsigned int64_t.

## 20.10.4

`Release date to be defined`

### Bugfixes

*Config*

retry\_interval attribute in configuration file seems to be have a safe behavior
with a decimal value, but but with a chain of character broker quit without
understanble error message, now broker quits cleanly with a warning that we can
see in by using journalctl command.

*TCP*

If we have an issue on the network between cbd and centengine, it is possible
that the acceptor among them keeps for an indefined time its connections to the
other. This leads to socket in CLOSE\_WAIT state.

*streamconnector*

There is a new function broker.md5(str) provided by the streamconnector that
computes the md5 checksum of the given string str.

*perfdata*

If a perfdata contains infinity or nan values, its insertion in database could
fail. This fixed now by inserting a NULL value instead. When the perfdata parser
encounters an error during its work, now it returns logs with the host id and
the service id of the associated service, so it is easier for the user to debug
his check.

Index\_id of index\_data and metrics tables are now in unsigned int64\_t.

*Log file configuration*

log file configuration is applied even if the configuration contains errors.
Now '.' is allowed in names.

*Broker termination*

When broker is badly configured and the user wants to stop it, it may hang and
never stop. This new version fixes this issue.

There were also dangling pointers in the bbdo manager that regularly lead to
segfault when it was unloaded. It is fixed now.

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

*TLS*

TLS common name check impossible if it is too long
add params tls_hostname

## 20.10.3

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

## 20.10.2

`December 16, 2020`

> Known behaviours:
>
>   - If TLS encryption is configured to use private key/certificate couple
>     for IPv4/6 input/output endpoints, **both ends must be updated**
>     to ensure communication.
>
>   - If you use Centreon MAP with TLS encryption, make sure to **update MAP
>     server** to version >= 20.10.2.

### Bugfixes

*TLS*

Credentials were not loaded by the TLS connector anymore. This is fixed
with this new version.

*Custom variables*

They were updated several times in the database. It is fixed now.

*Build*

GnuTLS requirement now matches compilation version.

*BAM*

Reporting events were not stored into database because of truncated
Business Activities names causing *duplicate entry* errors.

## 20.10.1

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

## 20.10.0

`October 21, 2020`

> Known behaviours:
>
>   - If Broker on a Poller or Remote Server is not upgraded to 20.10
>     (or with a version prior to 20.04.9), the communication between said
>     Poller or Remote Server and an upgraded Central may not work.
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

### Enhancements

*Watchdog*

The watchdog is lighter and does no more need all the broker libraries.

### Bugfixes

- Contains all fixes up to version 20.04.9
