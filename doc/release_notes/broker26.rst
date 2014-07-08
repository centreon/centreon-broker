===================
Centreon Broker 2.6
===================

**********
What's new
**********

One Peer Retention Mode
=======================

Within highly secured IT infrastructures, it is sometimes necessary to
control who initiates the connection between a server and a poller. The
*One Peer Retention Mode* features, allows you to connect to the poller
from the server **with retention enabled**.

Event Filtering
===============

The new filtering feature of Centreon Broker allows you to filter events
that can be emmitted or received by each endpoint. This can
significantly reduce the network traffic.

Fast RRD creation
=================

Centreon Broker integrates a new RRD creation feature based on
templates (reference files). This heavily reduces the I/O burden caused
by many RRD files creation.

More statistics
===============

When engaged in a retention process, Centreon Broker now provides an
estimate of arrival time and max size used on disk.


***************
Important notes
***************

* Centreon Broker 2.6 works with Centreon 2.5.0 and above. Centreon
  2.5.0 provides a new database schema required by Centreon Broker
  2.6.0.
* Make sure to stop or restart Centreon Broker and Centreon Engine
  before upgrading. No retention file should exist as BBDO protocol
  versions are not compatible.
