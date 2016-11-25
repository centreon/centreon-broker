=====================
Centreon Broker 3.0.2
=====================

*********
Bug fixes
*********

Cleanup service events after host deletion
==========================================

This fix was introduced along Centreon Engine 1.6.2. It allows service
events to be properly cleaned up after host deletion.

Asynchronous host/service cleanup
=================================

The SQL cleanup thread contained an invalid query that prevented hosts
and services tables to be cleaned up properly.

=====================
Centreon Broker 3.0.1
=====================

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

Dropped support of NDO protocol
===============================

The NDO protocol is no longer supported by Centreon Broker. Please use
the BBDO protocol instead, which is far better (lower network footprint,
automatic compression and encryption).
