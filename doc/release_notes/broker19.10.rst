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
