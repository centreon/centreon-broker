===================
Centreon Broker 2.7
===================

**********
What's new
**********

Data cleanup
============

A specific thread of the SQL module now delete outdated data from the
database to avoid ever growing tables.

Instance outdating
==================

Monitoring instances can now be configured to flag all of its services
to *UNKNOWN* when some timeout is reached. This allows you to easily
detect instances that have some connection issues and avoid you to
visualize unfresh states.

New statistics system
=====================

A new statistics system has been integrated in this version. However
this is still and experimental feature that has not yet been enabled by
default.

***************
Important notes
***************

* The statistics module has been renamed from *05-stats.so* to
  *15-stats.so*. If you made a manual installation, you should remove
  the former.
