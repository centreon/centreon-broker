======================
Centreon Broker 3.0.15
======================

************
Enhancements
************

Socket state retrieval in lua module
====================================

A new method get_state() is added to the broker_socket to check the connection
state.

======================
Centreon Broker 3.0.14
======================

*********
Bug fixes
*********

BAM segfault
============

Sometimes, it was possible that Broker crashes while it was applying a new
BAM configuration. This could be seen particularly when the ressource attached
to a kpi was disabled.

************
Enhancements
************

Lua module
==========

The Lua module provides new cache methods to access to index mappings, instance
names and metric mappings.

======================
Centreon Broker 3.0.13
======================

**********
What's new
**********

New Lua module
==============

This module exposes a part of Centreon Broker API through the Lua interpreter
and the user has just to fill it with his needs. Lua versions supported are
5.1, 5.2 and 5.3.

======================
Centreon Broker 3.0.12
======================

*********
Bug fixes
*********

Escape dots in Graphite
=======================

Dot (.) is a special character in Graphite. For this reason it is now
replaced by an underscore (_).

Correct default connection port for InfluxDB
============================================

The default connection port used by InfluxDB endpoints was improperly
set. Now its default value is set to 8086.

Link checking in BAM boolean expressions
========================================

In some cases, BAM boolean expression can be improperly calculated
because of missing links.

Delete unused queue files
=========================

All TCP connections, even transient, use at least an (empty) queue file
that remains on disk. Such files are now deleted on connection
termination.

======================
Centreon Broker 3.0.11
======================

*********
Bug fixes
*********

Properly build graphs when interval length is not default
=========================================================

The interval length property was always assumed to be 60 seconds. This
prevented graphs to be correctly built when using the non-default
interval length (usually 1 second).

Do not crash when log files does not have correct permissions
=============================================================

Self explanatory.

Rewrite compression/decompression functions
===========================================

Instead of using Qt's interface to compress/uncompress, Centreon Broker
now uses zlib directly. This notably prevents crashes on CentOS 6 when
uncompressing corrupted files.

======================
Centreon Broker 3.0.10
======================

*********
Bug fixes
*********

Graph rebuild cancellation
==========================

In previous versions, Centreon Broker kept a cache of all hosts and
services needed to be rebuilt. However such operations are slow and in
some cases operators might want to cancel ongoing rebuild. In this
version each host and service is treated individually. Therefore upon
cancellation rebuild will stop after the current host or service has
been rebuilt.

Flag graphs when rebuilding
===========================

Flag graphs as rebuilding upon processing. This was caused by
uncommitted SQL transactions.

Properly process downtimes sent simultaneously on same host/service
===================================================================

Downtimes same at the same time on the same host/service where
improperly processed and were not stored in database.

Performance improvement for transaction-based endpoints
=======================================================

This fix reduces thread contention on a single mutex when using
transactions. The two most typical cases are the real-time monitoring
(SQL) and performance data graphing (storage) endpoints.

=====================
Centreon Broker 3.0.9
=====================

*********
Bug fixes
*********

Restore multi-value insertion in data_bin
=========================================

In the 3.0.4 release of Centreon Broker, we introduced prepared
statements for insertion in the data_bin and logs table. In large setups
this change significantly reduced performances. For this reason we
reverted the changes for the data_bin table and we are now using
multi-values insertions.

Fix downtime inheritance on multi-level BA trees
================================================

With the BAM module, downtime inheritance was not working properly on
BA which had other BAs as KPIs.

************
Enhancements
************

Reduce RRD size
===============

The second RRA is now used to aggregate values by the hour, only if
needed.

=====================
Centreon Broker 3.0.8
=====================

*********
Bug fixes
*********

InfluxDB Line Protocol escape schemes
=====================================

InfluxDB Line Protocol use various escaping schemes that depends on the
query components. All escaping schemes are now implemented.

Improve decompression performance
=================================

Previously the decompression buffer was always modified after the
successful decompression of a single event leading to many useless
buffer reallocations. Data is now discarded only when the entire
decompression buffer is processed.

Fix random data corruption
==========================

BBDO streams poorly handled stream timeout, which could occured at any
time due to network latency. This lead to random data corruption, most
serious cases reporting real-time monitoring breakage.

=====================
Centreon Broker 3.0.7
=====================

*********
Bug fixes
*********

Fix file percent processed
==========================

The computation of file percent processed as provided in the statistics
file was invalid.

libgcrypt library issues when using GNU TLS 3
=============================================

Starting with its 3.0 version, GNU TLS does not use libgcrypt as
cryptographic backend anymore. Therefore libgcrypt initialization was
unnecessary in such cases.

=====================
Centreon Broker 3.0.6
=====================

*********
Bug fixes
*********

Do not allow write filters on inputs
====================================

The consequence of setting filters on inputs was that event loop could
occur. In this release, inputs can only one way of filters, therefore
preventing event loops.

Retention file hardening
========================

Some users reported issues with retention files. In this release the
most important classes managing retention files were refactored and unit
tested.

Fix inherited BA downtimes on non-default instance
==================================================

In previous versions BA downtime inheritance only worked with the
default Centreon instance created during installation (with ID 1). In
some cases (recreated instance, Poller Display) the instance ID was not
1 and prevented BA downtime inheritance from working.

Detect metrics without valid value
==================================

Metrics without value, in improperly formatted performance data, were
inserted in data_bin with a 0 value, polluting the table.

Do not attempt to acknowledge more events than available
========================================================

The *storage* engine was sometimes generating error logs with the
message "attempt to acknowledge more events than available".

=====================
Centreon Broker 3.0.5
=====================

*********
Bug fixes
*********

Fix a crash by memory corruption
================================

In some cases, the current pointer to the next in-memory event to
process can be corrupted. On high-volume platforms this usually leads
to crashes within minutes.

Print queue file statistics
===========================

Print queue file statistics (if available) for every endpoint.

Correct duplicate BA event durations
====================================

This was caused by an invalid UPDATE query so the BA event durations
were always inserted. The consequence was that UNIQUE keys were
triggered and lead to duplicate errors.

=====================
Centreon Broker 3.0.4
=====================

*********
Bug fixes
*********

BA availabilities with non-standard timeperiods
===============================================

Availabilities were improperly computed when linked to non-standard
timeperiods (exceptions, exclusions, templates, ...).

Use prepared statements on logs and data_bin
============================================

Starting with Centreon 2.8, the default storage engine for the logs and
data_bin tables is now InnoDB. To improve performances with this storage
engine, the query system now use prepared statements and transactions to
insert data in these tables.

Invalid event acknowledgement
=============================

Events read from cache/retention files were improperly acknowledged.
This means that in case of failure following a successful reading of the
file events could be lost. Now events are only acknowledged once
processing is confirmed.

Downtimes not deleted if not started
====================================

The cancellation flag of the downtimes table was not set if a downtime
was deleted before it started.

Fix data insertion in InfluxDB databases
========================================

The InfluxDB was incorrectly inserting data in database for at least
three different reasons : invalid timestamp format, wrong HTTP status
code check and quoted tags. This is now fixed.

Reconnect when using one peer retention mode
============================================

A bug in the TCP layer prevented one peer retention mode connections to
reconnect.

=====================
Centreon Broker 3.0.3
=====================

*********
Bug fixes
*********

Non-OK statuses in BAM expressions were always considered as OK
===============================================================

Non-OK statuses in BAM expressions (such as {CRITICAL}) were always
treated as OKs by the BAM computation engine. Therefore most
expressions using non-OK stasuses were improperly evaluated and
sometimes trigger BA impacts that were not justified.

Use non-standard path in watchdog
=================================

The watchdog was always using */usr/sbin/cbd* to run the Centreon Broker
daemon. This was not aligned with the build variable WITH_PREFIX_BIN.
This should help users of Debian-based distributions in their manual
installations.

=====================
Centreon Broker 3.0.2
=====================

*********
Bug fixes
*********

Cleanup service events after host deletion
==========================================

This fix was introduced along Centreon Engine 1.6.2. It allows service
events to be properly cleaned up after host deletion.

Asynchronous host/service cleanup
=================================

The SQL cleanup thread contained an invalid query that prevented hosts
and services tables to be cleaned up properly.

=====================
Centreon Broker 3.0.1
=====================

**********
What's new
**********

Immediate shutdown
==================

Now even in case of heavy load Centreon Broker will shutdown in seconds
instead of multiple minutes in some circumstances.

Automatic data retention
========================

Centreon Broker fully handle data retention on its own. Configuration
is very limited : a cache directory and a watermark limit.

JSON statistics
===============

For better programatic use of Centreon Broker statistics, these
information are now written in JSON.

Compatibility with Centreon Engine 1.6
======================================

Centreon Engine 1.6 introduces technical changes needed by Centreon
Broker 3 and is therefore not compatible with the earlier releases of
Centreon Engine.

Dropped support of NDO protocol
===============================

The NDO protocol is no longer supported by Centreon Broker. Please use
the BBDO protocol instead, which is far better (lower network footprint,
automatic compression and encryption).
