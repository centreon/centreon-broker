=======================
Centreon Broker 20.04
=======================

************
Enhancements
************

Removal of Qt
=============
Broker does not need Qt anymore.

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
