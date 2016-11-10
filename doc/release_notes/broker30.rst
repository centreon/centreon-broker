===================
Centreon Broker 3.0
===================

**********
What's new
**********

Immediate shutdown
==================

Now even in case of heavy load Centreon Broker will shutdown in seconds
instead of multiple minutes in some circumstances.

Automatic data retention
========================

Centreon Broker fully handle data retention on its own. Configuration
is very limited : a cache directory and a watermark limit.

JSON statistics
===============

For better programatic use of Centreon Broker statistics, these
information are now written in JSON.

Compatibility with Centreon Engine 1.6
======================================

Centreon Engine 1.6 introduces technical changes needed by Centreon
Broker 3 and is therefore not compatible with the earlier releases of
Centreon Engine.
