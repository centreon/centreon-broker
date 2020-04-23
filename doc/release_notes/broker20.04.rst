=======================
Centreon Broker 20.04.1
=======================

************
Enhancements
************

New Lua function in the streamconnector
=======================================
There is a new function broker.stat(filename) to get informations about the
filename.

*********
Bug fixes
*********

Perfdata parsing
================
Special characters like '\\r' were not parsed correctly.

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

