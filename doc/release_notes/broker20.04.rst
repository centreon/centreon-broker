=======================
Centreon Broker 20.04
=======================

************
Enhancements
************

Removal of Qt
=============
Broker does not need Qt anymore.

JSon
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

Better test Coverage
====================
We now have 370+ tests (+280%). It allow us to have a better code coverage
of the code base.

Stream connector
================

The stream connector is now asynchronous. If it has to execute a script that
is too slow, it won't slow down Broker. Broker will just return messages
complaining about the slowness of the script.

Another change, is when the script crashes, Broker does not finish with an
exception, it writes an error containing the Lua interpreter error but it will
try to execute the script next time.
