===================
Centreon Broker 2.5
===================

**********
What's new
**********

BBDO protocol
=============

The new *BBDO* (*Broker Binary Data Objects*) is a protocol especially
designed for performance. It is a lightweight transmission protocol
with feature negotiation (encryption and/or compression).

Retention time per RRD
======================

It is now possible to specify a retention length per metric file.

Range thresholds
================

Centreon Broker now properly handle range thresholds (ie. @42:84).

New encryption module
=====================

Brand new encryption module based on GNU TLS.

Optimized insertion in data_bin
===============================

Use a multi-value insertion statement in the data_bin table to optimize
data insertion and avoid frequent table locking.

Event acknowledgement for DB
============================

Events are now acknowledged when inserted in database. This prevent
missing entries in case of database crash/shutdown/unreachability.

Option to create entries in index_data
======================================

The new parameter *insert_in_index_data* of *storage* endpoints allows
to have a standalone (without Centreon) graph generation system. Use
with caution.

Diagnostic tool
===============

A diagnostic-file generation mechanism used to speed up handling of
incidents at Merethis' support center.


***************
Important notes
***************

* Centreon Broker 2.5 works with Centreon 2.4.5 and next version.
  Centreon 2.4.5 provide new database schema required by Centreon
  Broker 2.5.
* Make sure to stop or restart Centreon-Broker with the init script.
  Otherwise, possible issues can arise if Centreon-Broker is not
  shutdown properly.
* Centreon-Broker checks the database integrity, if for some reason your
  database was corrupted, Centreon-Broker will stop sending data into the
  database and put these data into failover output instead.
