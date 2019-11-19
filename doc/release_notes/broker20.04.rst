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