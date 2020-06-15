=======================
Centreon Broker 18.10.2
=======================

*********
Bug fixes
*********

Escape backslash in JSON encoding/decoding functions
====================================================

In previous versions backslash was not handled as a special character
leading to invalid JSON encoding and decoding.

Return an error in Lua perfdata parsing function
================================================

Before this fix when we were calling parse_perfdata with an invalid
perf string, the lua was not working.

************
Enhancements
************

Send event when poller is unresponsive
======================================

A new event named responsive_instance was added. This is especially
useful for Centreon Map.

Support *centreon* systemd service
==================================

The *cbd* service is now wanted by the *centreon* service.

=======================
Centreon Broker 18.10.1
=======================

*********
Bug fixes
*********

Stream connector cache
======================

The cache containing bam objects was always empty.

************
Enhancements
************

Stream connector default path
=============================

Now, it is possible to split a stream connector into several files if
they are stored in the same directory. The global path and cpath are
modified to also contained the main script directory.

Stream connector new function
=============================

The Lua broker namespace contains a new function parse_perfdata to
parse perfdata.
