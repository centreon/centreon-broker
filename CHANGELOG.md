# Changelog

## 21.04.6

### fixes
*rrd*

add query sql to check metrics to delete metric and file associated.

*storage*

Waiting longer for conflict manager to be connected without blocking if cbd
is stopped.

Cache loading queries are parallelized.

*bam*

A ba configured with best status was initialized in state OK. And when KPI were
added, there status was always worst than OK (best case was OK). So even if all
the kpi were in critical, the state appeared as OK.

The cache in bam is lighter, metrics are no
more loaded. And queries to load the cache are parallelized.

*mysql_connection*

A timeout is added on mysql\_ping and this function is less called than before.


## 21.04.5

Update the headers.
## 21.04.4

*lua*

The Lua cache should no more disappear.

*bbdo*

If a broker receives bbdo message it cannot unserialize because of a missing
module, it must be able to acknowledge it. Otherwise, this message is kept
in retention by the peer and this can lead cbd instances to get stuck.

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

## 21.04.3

### Fixes

*sql*

When a connection to the db is lost, we try to reestablish it. This change fixes
a error "MySQL server has gone away" we often have in the BAM availabilities
computations.

*processing*

When the stop event is sent by a peer, we must protect the call by try/catch to
avoid an abortion that could appear.

*bbdo*

When the connection of an acceptor is reversed, if cbd is stopped when there is
no peer, cbd does not stop. This patch fixes this issue.

## 21.04.2

### Bug fix

*cbmod*

A link issue in cbmod caused a crash in centengine. This new version fixes it.

*BAM*

When a new BA is created with new KPI, it is possible to have an issue during
the kpi events insertion in the database with the start time column. This patch
fixes this issue.

*SQL*

Broker stores its connections to the database in an array. Once they are
established, it does not test if they are still valid after a laps of time.
But we know that MariaDB closes inactive connections. So here, we have added
a check to verify if a connection is still ok.
A possible deadlock due to mysql connections when cbd is stopped has been fixed.

*RPC*

A memory leak has been fixed. The reflection module is removed because not used
and also it did not compile anymore since an update on the conan center.

*TLS*

GnuTLS query was not understood on RedHat 8 and Centos8.

*BBDO serialized events*

Converts theses events into trace.

*TCP*

* Connections can fail when many pollers establish connection to cbd. This
should be fixed with this new version.
* When cbd is stopped, sometimes centengine cannot reconnect. This is fixed now.
* if the connection fails to get the remote endpoint, the error was not catched
  and could break cbd/centengine.

*sql*

Add a parameter to select the MySQL UNIX socket

### Build

repair the compilation for Raspberry PI

### Enhancements

*Parser*

Add unit test for the parser, while it is reading configuration file with loggers at null
value.

*Timeranges*

Allowing parsing \r or \n for the parser of timeranges.

## 21.04.1

`Date of the release`

### Bug fixes

*Factories*

If we want two outputs configured with tls but with different configurations,
this was impossible to configure, because the factory can only store one
configuration. This is fixed now.

*Build*

Since the bintray closure, we could not build cbd. Dependencies have been moved
to the conan-center and the build is back.

*Bam*

Meta-services are removed from broker. This code was not used and could produce

### Enhancements

*Storage*

We optimize queries by removing useless join. A join with hosts table is not
necessary in certain queries.

## 21.04.0

`2021 april 20th`

### Enhancements

*Broker exceptions*

They are all based on a small msg\_fmt exception that is lighter than the old
msg one.
