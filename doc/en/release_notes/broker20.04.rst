=======================
Centreon Broker 20.04.8
=======================

*********
Bug fixes
*********

UTF-8 encoding
==============
The UTF-8 chek/encoding is moved from engine to cbmod. This is easier to update
for Centreon users. Also, the check algorithm is fixed. Some strings could be
considered as UTF-8 strings whereas they were not.

=======================
Centreon Broker 20.04.7
=======================

************
Enhancements
************

Conflict manager
================
Code improved and should be faster during Broker restart.

=======================
Centreon Broker 20.04.6
=======================

*********
Bug fixes
*********

Broker reload with BAM configuration
====================================
Centreon broker could not recover if configured with bam, after a restart of the
database.

Segfault possible during a Mariadb server restart
=================================================
Centreon broker could crash when the database server was restarted. This version
fixes this bug.

BAM module could never recovery after a Mariadb server restart
==============================================================
BAM module is better managed on database server reload/restart.

=======================
Centreon Broker 20.04.5
=======================

*********
Bug fixes
*********

Not ascii characters badly encoded in database
==============================================
If a checkout output contains not ascii characters, they are badly transformed
and the string looses its sense. This is fixed.

=======================
Centreon Broker 20.04.4
=======================

*********
Bug fixes
*********

Neb Service Status Check were badly handled
===========================================
Service status check were badly handled. For example the field
command_line was not updated in service. This bug was introduced
in 20.04.3.

=======================
Centreon Broker 20.04.3
=======================

*********
Bug fixes
*********

Events were badly acknowledged after being sent to the database
===============================================================
SQL/storage dis not ack all the events. This produced retention files.

Long events could be corrupted
==============================
There was a bug in the long events management.

Filter on events entering in storage
====================================
A bug on this filter is now fixed.

Retention files
===============
A regression was introduced. All the retention files could not be read.

MariaDB strict mode
===================
The strict mode implies that strings too long for a column break queries. To
avoid this, we truncate too long strings and set a warning log for users.

=======================
Centreon Broker 20.04.2
=======================

*********
Bug fixes
*********

BBDO is sending corrupted data
==============================
Data could be badly sent. Now it is fixed.

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
Centreon Broker 20.04.1
=======================

************
Enhancements
************

Perfdata parser
===============
The parser is less strict. It tries to keep good metrics among bad ones.

New Lua function in the streamconnector
=======================================
There is a new function broker.stat(filename) to get informations about the
filename.

*********
Bug fixes
*********

Strict mode of the database
===========================
Too long strings to insert in database are cut so that cbd continues to work.
This will be improved in a future Broker version. A warning is logged so that
the user can change his configuration to avoid that.

Perfdata parsing
================
Special characters like '\\r' were not parsed correctly.

conflict manager
================
In case of bad configurations concerning the database, cbd can crash. This is
fixed with with new version.

=======================
Centreon Broker 20.04.0
=======================

************
Enhancements
************

Removal of Qt
=============
Broker does not need Qt anymore.

Lua
===

New function to decode a Json string into a Lua table.

Json
=====
Switch from Xml config to Json. We used json11 toolkit, and remove
all ref for yajl from sources.

Network
========
Switch from QtNetwork to Asio. We start an effort to avoir copy in network
buffers.

Optimization
============
Migration of the code to C++11.

Better tests Coverage
======================
We now have 370+ tests (+280%). It allow us to have a better code coverage
of the code base.

Stream connector
================

The stream connector is now asynchronous. If it has to execute a script that
is too slow, it won't slow down Broker. Broker will just return messages
complaining about the slowness of the script.

Another change, now when a stream connector crashes, Broker does not terminate
but just returns an error message containing the Lua interpreter error.

*********
Bug fixes
*********

Influxdb connector and retention
================================

If a retention is configured on the influxdb server and centreon-broker sends
too old data compared to this retention, the connector ends with an error and
centreon-broker pushes data in retention instead of throwing them away.
This patch fixes that.
