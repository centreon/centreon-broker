===================
Centreon Broker 2.9
===================

**********
What's new
**********

Reporting computation
---------------------

In Centreon BAM, reporting is computed by Centreon Broker. In the last version,
When an alerts is longer than a day, the alerts number can be wrong. Now this
function is operating normaly.

Optimization
------------

An optimization	has been made for the processus that update metrics table in
centreon_storage database. Now entries are updated only if the value of the metric
and all parameters are differents that the previous state. This behavior improves
the database use.

Unix socket not supported anymore
---------------------------------

This version of Centreon Broker removes the support of Unix socket for communications
between all broker modules and daemons.	Regarding our statistics, Unix socket is
not used by an enough representative part of users. So we decided to remove this function.

***************
Important notes
***************

* Centreon Broker 2.9.2 fixes a memory leak introduced in Centreon Broker 2.8.0.
  All users are strongly advised to upgrade.
* Centreon Broker 2.9.2 fixes an issue with KPI processing for BAM users. This
  regression was introduced by the 2.9.1 release.
