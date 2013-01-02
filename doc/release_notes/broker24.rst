===================
Centreon Broker 2.4
===================

**********
What's new
**********

Improve buffer management
=========================

To limit memory usage, you can use new buffer type, name "temporary" to
store data on the hard disk. This feature is very usefull when you have
many data in retention file.

Manage RRD graph
================

Centreon-Broker provide some new features to manage RRD graph

* Disable status graph to save I/O
* Rebuild graph with database history (like centstorage)
* Remove metrics (like centstorage)

New configuration check tool
============================

You can check Centreon-Broker configuration from the command line with
'-c' option.

File module
===========

Now you can split output into multiple files.

Stats module
============

Some informations are now available with the stats module

* The current queued events number
* The read offset in files

***************
Important notes
***************

* Make sure to stop or restart Centreon-Broker with the init script.
  Otherwise, possible issues can arise if Centreon-Broker is not
  shutdown properly.
* Centreon-Broker checks the database integrity, if for some reason your
  database was corrupted, Centreon-Broker will stop sending data into the
  database and put these data into failover output instead.
