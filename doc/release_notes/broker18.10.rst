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
