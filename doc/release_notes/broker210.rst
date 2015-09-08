====================
Centreon Broker 2.10
====================

**********
What's new
**********

Database configuration transfer
-------------------------------

Centreon Broker is now able to transfer some database configuration
tables from an instance to another (typically from the central to a
poller). This currently concerns only BA-related tables.

External commands
-----------------

In a model similar to Centreon Engine, Centreon Broker now accepts some
external commands (to transfer a database configuration for example). It
uses a Unix domain socket.

TCP/BBDO layer rewrite
----------------------

In heavily threaded environment the TCP/BBDO mechanisms (especially
within acceptors) caused hard-to-catch errors. Special care was to taken
during to rewrite to ensure that no threading errors could hide behind
our code.
