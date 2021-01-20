=======================
Centreon Broker 19.10.6
=======================

*********
Bug fixes
*********

Bam reporting
=============
When availabilities are computed, durations can be doubled. This new version
fixes this issue.

Bam reporting dimensions computation
====================================
If there are retention files, dimensions computation could fail because of
conflicts between new block computation and old ones (the ones in the
retention).

=======================
Centreon Broker 19.10.5
=======================

************
Enhancements
************

Stream connector
================
The Stream connector cache has three new functions that are get_notes(),
get_notes_url() and get_action_url(). They can be used on hosts or on services.
To use them on hosts, you just have to give the host id as parameter. To use
them on services, you just have to give the host id and the service id as
parameters. All this is detailed in the Broker documentation.

It is also possible to get the severity of a host or a service. We provide now
the function broker_cache:get_severity(host_id, service_id). If you just give
the host_id, we suppose you want a host severity.

=======================
Centreon Broker 19.10.4
=======================

*********
Bug fixes
*********

Influxdb connector and retention
================================

If a retention is configured on the influxdb server and centreon-broker sends
too old data compared to this retention, the connector ends with an error and
centreon-broker pushes data in retention instead of throwing them away.
This patch fixes that.

Bad mod bam kpi relations
=========================

Some BA / KPI relation where wrong in BAM reporting. The kpi event was not
linked to the good BA.

=======================
Centreon Broker 19.10.3
=======================

*********
Bug fixes
*********

Downtimes deletion
==================

When downtimes were deleted, they were badly deleted and this could create
memory issues. The change on the Broker side is due to NEB headers changes.

=======================
Centreon Broker 19.10.2
=======================

*********
Bug fixes
*********

Fixed downtimes and notifications
=================================

When two contiguous downtimes are parametrized. It was possible to have
notifications of critical devices between them. This version of broker is
needed because of changes in the Engine timed_event class.

Connection lost when One peer retention is active
=================================================

When a central and a poller are using One peer retention and the poller get
disconnected then reconnection will never occur.

=======================
Centreon Broker 19.10.1
=======================

************
Enhancements
************

Stream connector
================

A new function broker.url_encode(string) has been added to the stream connector
that URL encodes strings.

=======================
Centreon Broker 19.10
=======================

*********
Bug fixes
*********

Remote Server
=============

Sql requests blocking several Centreon functionalities have been fixed.

RRD
===

Centreon Broker now supports last RRDtool versions.

Support for XOR in BAM
======================

Xor was buggy in BAM boolean rules. It is now fixed

Cache fixes
===========
Cache was lost on some streams (SQL, Graphite, InfluxDB).

************
Enhancements
************

Cmake cleanup
=============

The build directory is now gone away. We use now cmake as intended, this
solves issues with some ide (like kdevelop)...

Strict mode
===========
A lot of sql requests were not compatible with MariaDB/MySql strict mode. They
have been fixed.

New json library
================

We have remove the yajl code from the code base. We now use json11 framework
from dropbox folks.

Switch to C++11
================

Centreon Broker now is C++11 compatible.
